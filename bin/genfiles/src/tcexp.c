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
msg, struct _tagged_arr ap){({ void(* _temp0)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp3= loc; struct _tagged_arr _temp4=( struct _tagged_arr) Cyc_Stdio_vraprintf(
Cyc_Core_heap_region, msg, ap); void* _temp1[ 0u]={}; struct _tagged_arr _temp2={(
void*) _temp1,( void*) _temp1,( void*)( _temp1 + 0u)}; _temp0( _temp3, _temp4,
_temp2);}); return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp5=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp5;}));} static struct Cyc_Core_Opt Cyc_Tcexp_rk={( void*)(( void*) 3u)};
static struct Cyc_Core_Opt Cyc_Tcexp_ak={( void*)(( void*) 0u)}; static struct
Cyc_Core_Opt Cyc_Tcexp_bk={( void*)(( void*) 2u)}; static struct Cyc_Core_Opt
Cyc_Tcexp_mk={( void*)(( void*) 1u)}; static void Cyc_Tcexp_resolve_unknown_id(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, struct _tuple1* q){ struct
_handler_cons _temp6; _push_handler(& _temp6);{ int _temp8= 0; if( setjmp(
_temp6.handler)){ _temp8= 1;} if( ! _temp8){{ void* _temp9= Cyc_Tcenv_lookup_ordinary(
te, e->loc, q); void* _temp19; struct Cyc_Absyn_Enumfield* _temp21; struct Cyc_Absyn_Enumdecl*
_temp23; struct Cyc_Absyn_Tunionfield* _temp25; struct Cyc_Absyn_Tuniondecl*
_temp27; _LL11: if(*(( int*) _temp9) == Cyc_Tcenv_VarRes){ _LL20: _temp19=( void*)((
struct Cyc_Tcenv_VarRes_struct*) _temp9)->f1; goto _LL12;} else{ goto _LL13;}
_LL13: if(*(( int*) _temp9) == Cyc_Tcenv_EnumRes){ _LL24: _temp23=(( struct Cyc_Tcenv_EnumRes_struct*)
_temp9)->f1; goto _LL22; _LL22: _temp21=(( struct Cyc_Tcenv_EnumRes_struct*)
_temp9)->f2; goto _LL14;} else{ goto _LL15;} _LL15: if(*(( int*) _temp9) == Cyc_Tcenv_TunionRes){
_LL28: _temp27=(( struct Cyc_Tcenv_TunionRes_struct*) _temp9)->f1; goto _LL26;
_LL26: _temp25=(( struct Cyc_Tcenv_TunionRes_struct*) _temp9)->f2; goto _LL16;}
else{ goto _LL17;} _LL17: if(*(( int*) _temp9) == Cyc_Tcenv_StructRes){ goto
_LL18;} else{ goto _LL10;} _LL12:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct*
_temp29=( struct Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp29[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp30; _temp30.tag= Cyc_Absyn_Var_e;
_temp30.f1= q; _temp30.f2=( void*) _temp19; _temp30;}); _temp29;}))); goto _LL10;
_LL14:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp31=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp31[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp32; _temp32.tag= Cyc_Absyn_Enum_e;
_temp32.f1= q; _temp32.f2=( struct Cyc_Absyn_Enumdecl*) _temp23; _temp32.f3=(
struct Cyc_Absyn_Enumfield*) _temp21; _temp32;}); _temp31;}))); goto _LL10;
_LL16:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_e_struct*
_temp33=( struct Cyc_Absyn_Tunion_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_e_struct));
_temp33[ 0]=({ struct Cyc_Absyn_Tunion_e_struct _temp34; _temp34.tag= Cyc_Absyn_Tunion_e;
_temp34.f1= 0; _temp34.f2= 0; _temp34.f3= 0; _temp34.f4= _temp27; _temp34.f5=
_temp25; _temp34;}); _temp33;}))); goto _LL10; _LL18:({ void(* _temp35)( struct
Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr;
struct Cyc_Position_Segment* _temp39= e->loc; struct _tagged_arr _temp40=
_tag_arr("bad occurrence of struct name %s", sizeof( unsigned char), 33u);
struct Cyc_Stdio_String_pa_struct _temp41; _temp41.tag= Cyc_Stdio_String_pa;
_temp41.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( q);{ void* _temp38=(
void*)& _temp41; void* _temp36[ 1u]={ _temp38}; struct _tagged_arr _temp37={(
void*) _temp36,( void*) _temp36,( void*)( _temp36 + 1u)}; _temp35( _temp39,
_temp40, _temp37);}});( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct*
_temp42=( struct Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp42[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp43; _temp43.tag= Cyc_Absyn_Var_e;
_temp43.f1= q; _temp43.f2=( void*)(( void*) Cyc_Absyn_Unresolved_b); _temp43;});
_temp42;}))); goto _LL10; _LL10:;}; _pop_handler();} else{ void* _temp7=( void*)
_exn_thrown; void* _temp45= _temp7; _LL47: if( _temp45 == Cyc_Dict_Absent){ goto
_LL48;} else{ goto _LL49;} _LL49: goto _LL50; _LL48:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Var_e_struct* _temp51=( struct Cyc_Absyn_Var_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct)); _temp51[ 0]=({ struct Cyc_Absyn_Var_e_struct
_temp52; _temp52.tag= Cyc_Absyn_Var_e; _temp52.f1= q; _temp52.f2=( void*)(( void*)
Cyc_Absyn_Unresolved_b); _temp52;}); _temp51;}))); goto _LL46; _LL50:( void)
_throw( _temp45); _LL46:;}}} struct _tuple7{ struct Cyc_List_List* f1; struct
Cyc_Absyn_Exp* f2; } ; static struct _tuple7* Cyc_Tcexp_make_struct_arg( struct
Cyc_Absyn_Exp* e){ return({ struct _tuple7* _temp53=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp53->f1= 0; _temp53->f2= e; _temp53;});} static
void Cyc_Tcexp_resolve_unknown_fn( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* es){ void* _temp54=( void*)
e1->r; struct _tuple1* _temp60; _LL56: if(*(( int*) _temp54) == Cyc_Absyn_UnknownId_e){
_LL61: _temp60=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp54)->f1; goto _LL57;}
else{ goto _LL58;} _LL58: goto _LL59; _LL57: { struct _handler_cons _temp62;
_push_handler(& _temp62);{ int _temp64= 0; if( setjmp( _temp62.handler)){
_temp64= 1;} if( ! _temp64){{ void* _temp65= Cyc_Tcenv_lookup_ordinary( te, e1->loc,
_temp60); void* _temp75; struct Cyc_Absyn_Tunionfield* _temp77; struct Cyc_Absyn_Tuniondecl*
_temp79; struct Cyc_Absyn_Structdecl* _temp81; _LL67: if(*(( int*) _temp65) ==
Cyc_Tcenv_VarRes){ _LL76: _temp75=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp65)->f1; goto _LL68;} else{ goto _LL69;} _LL69: if(*(( int*) _temp65) ==
Cyc_Tcenv_TunionRes){ _LL80: _temp79=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp65)->f1; goto _LL78; _LL78: _temp77=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp65)->f2; goto _LL70;} else{ goto _LL71;} _LL71: if(*(( int*) _temp65) ==
Cyc_Tcenv_StructRes){ _LL82: _temp81=(( struct Cyc_Tcenv_StructRes_struct*)
_temp65)->f1; goto _LL72;} else{ goto _LL73;} _LL73: if(*(( int*) _temp65) ==
Cyc_Tcenv_EnumRes){ goto _LL74;} else{ goto _LL66;} _LL68:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp83=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp83[ 0]=({ struct Cyc_Absyn_FnCall_e_struct
_temp84; _temp84.tag= Cyc_Absyn_FnCall_e; _temp84.f1= e1; _temp84.f2= es;
_temp84.f3= 0; _temp84;}); _temp83;}))); _npop_handler( 0u); return; _LL70: if(
_temp77->typs == 0){({ void(* _temp85)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp89= e->loc; struct _tagged_arr _temp90= _tag_arr("%s is a constant, not a function",
sizeof( unsigned char), 33u); struct Cyc_Stdio_String_pa_struct _temp91; _temp91.tag=
Cyc_Stdio_String_pa; _temp91.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp77->name);{ void* _temp88=( void*)& _temp91; void* _temp86[ 1u]={ _temp88};
struct _tagged_arr _temp87={( void*) _temp86,( void*) _temp86,( void*)( _temp86
+ 1u)}; _temp85( _temp89, _temp90, _temp87);}});}( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Tunion_e_struct* _temp92=( struct Cyc_Absyn_Tunion_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_e_struct)); _temp92[ 0]=({ struct Cyc_Absyn_Tunion_e_struct
_temp93; _temp93.tag= Cyc_Absyn_Tunion_e; _temp93.f1= 0; _temp93.f2= 0; _temp93.f3=
es; _temp93.f4= _temp79; _temp93.f5= _temp77; _temp93;}); _temp92;})));
_npop_handler( 0u); return; _npop_handler( 0u); return; _LL72: { struct Cyc_List_List*
_temp94=(( struct Cyc_List_List*(*)( struct _tuple7*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcexp_make_struct_arg, es); if(
_temp81->name != 0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp95=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp95[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp96; _temp96.tag= Cyc_Absyn_Struct_e;
_temp96.f1=( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( _temp81->name))->v;
_temp96.f2= 0; _temp96.f3= _temp94; _temp96.f4=( struct Cyc_Absyn_Structdecl*)
_temp81; _temp96;}); _temp95;})));} else{({ void(* _temp97)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp100= e->loc; struct _tagged_arr _temp101= _tag_arr("missing struct name",
sizeof( unsigned char), 20u); void* _temp98[ 0u]={}; struct _tagged_arr _temp99={(
void*) _temp98,( void*) _temp98,( void*)( _temp98 + 0u)}; _temp97( _temp100,
_temp101, _temp99);});( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp102=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp102[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp103; _temp103.tag= Cyc_Absyn_Struct_e;
_temp103.f1= _temp60; _temp103.f2= 0; _temp103.f3= _temp94; _temp103.f4=( struct
Cyc_Absyn_Structdecl*) _temp81; _temp103;}); _temp102;})));} _npop_handler( 0u);
return;} _LL74:({ void(* _temp104)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp108= e->loc; struct _tagged_arr _temp109= _tag_arr("%s is an enum constructor, not a function",
sizeof( unsigned char), 42u); struct Cyc_Stdio_String_pa_struct _temp110;
_temp110.tag= Cyc_Stdio_String_pa; _temp110.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp60);{ void* _temp107=( void*)& _temp110; void* _temp105[ 1u]={ _temp107};
struct _tagged_arr _temp106={( void*) _temp105,( void*) _temp105,( void*)(
_temp105 + 1u)}; _temp104( _temp108, _temp109, _temp106);}}); _npop_handler( 0u);
return; _LL66:;}; _pop_handler();} else{ void* _temp63=( void*) _exn_thrown;
void* _temp112= _temp63; _LL114: if( _temp112 == Cyc_Dict_Absent){ goto _LL115;}
else{ goto _LL116;} _LL116: goto _LL117; _LL115:({ void(* _temp118)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp122= e1->loc; struct _tagged_arr _temp123= _tag_arr("unbound identifier %s",
sizeof( unsigned char), 22u); struct Cyc_Stdio_String_pa_struct _temp124;
_temp124.tag= Cyc_Stdio_String_pa; _temp124.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp60);{ void* _temp121=( void*)& _temp124; void* _temp119[ 1u]={ _temp121};
struct _tagged_arr _temp120={( void*) _temp119,( void*) _temp119,( void*)(
_temp119 + 1u)}; _temp118( _temp122, _temp123, _temp120);}});( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp125=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp125[ 0]=({ struct
Cyc_Absyn_FnCall_e_struct _temp126; _temp126.tag= Cyc_Absyn_FnCall_e; _temp126.f1=
e1; _temp126.f2= es; _temp126.f3= 0; _temp126;}); _temp125;}))); return; _LL117:(
void) _throw( _temp112); _LL113:;}}} _LL59:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_FnCall_e_struct* _temp127=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp127[ 0]=({ struct
Cyc_Absyn_FnCall_e_struct _temp128; _temp128.tag= Cyc_Absyn_FnCall_e; _temp128.f1=
e1; _temp128.f2= es; _temp128.f3= 0; _temp128;}); _temp127;}))); return; _LL55:;}
static void Cyc_Tcexp_resolve_unresolved_mem( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
des){ if( topt == 0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp129=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp129[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp130; _temp130.tag= Cyc_Absyn_Array_e;
_temp130.f1= des; _temp130;}); _temp129;}))); return;}{ void* t=*(( void**)
_check_null( topt)); void* _temp131= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tqual
_temp141; void* _temp143; struct Cyc_Absyn_Structdecl** _temp145; struct Cyc_List_List*
_temp147; struct _tuple1* _temp149; _LL133: if(( unsigned int) _temp131 > 4u?*((
int*) _temp131) == Cyc_Absyn_ArrayType: 0){ _LL144: _temp143=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp131)->f1; goto _LL142; _LL142: _temp141=((
struct Cyc_Absyn_ArrayType_struct*) _temp131)->f2; goto _LL134;} else{ goto
_LL135;} _LL135: if(( unsigned int) _temp131 > 4u?*(( int*) _temp131) == Cyc_Absyn_StructType:
0){ _LL150: _temp149=(( struct Cyc_Absyn_StructType_struct*) _temp131)->f1; goto
_LL148; _LL148: _temp147=(( struct Cyc_Absyn_StructType_struct*) _temp131)->f2;
goto _LL146; _LL146: _temp145=(( struct Cyc_Absyn_StructType_struct*) _temp131)->f3;
goto _LL136;} else{ goto _LL137;} _LL137: if(( unsigned int) _temp131 > 4u?*((
int*) _temp131) == Cyc_Absyn_AnonStructType: 0){ goto _LL138;} else{ goto _LL139;}
_LL139: goto _LL140; _LL134:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp151=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp151[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp152; _temp152.tag= Cyc_Absyn_Array_e;
_temp152.f1= des; _temp152;}); _temp151;}))); goto _LL132; _LL136: if( _temp145
== 0? 1: _temp149 == 0){({ void(* _temp153)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp156= loc; struct _tagged_arr _temp157= _tag_arr("Compiler bug: struct type not properly set",
sizeof( unsigned char), 43u); void* _temp154[ 0u]={}; struct _tagged_arr
_temp155={( void*) _temp154,( void*) _temp154,( void*)( _temp154 + 0u)};
_temp153( _temp156, _temp157, _temp155);});}( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp158=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp158[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp159; _temp159.tag= Cyc_Absyn_Struct_e; _temp159.f1=(
struct _tuple1*) _check_null( _temp149); _temp159.f2= 0; _temp159.f3= des;
_temp159.f4=( struct Cyc_Absyn_Structdecl*)*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp145)); _temp159;}); _temp158;}))); goto _LL132; _LL138:( void*)(
e->r=( void*)(( void*)({ struct Cyc_Absyn_AnonStruct_e_struct* _temp160=( struct
Cyc_Absyn_AnonStruct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonStruct_e_struct));
_temp160[ 0]=({ struct Cyc_Absyn_AnonStruct_e_struct _temp161; _temp161.tag= Cyc_Absyn_AnonStruct_e;
_temp161.f1=( void*) t; _temp161.f2= des; _temp161;}); _temp160;}))); goto
_LL132; _LL140:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp162=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp162[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp163; _temp163.tag= Cyc_Absyn_Array_e;
_temp163.f1= des; _temp163;}); _temp162;}))); goto _LL132; _LL132:;}} static
void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp*
e); static void* Cyc_Tcexp_tcExpNoPromote( struct Cyc_Tcenv_Tenv* te, void**
topt, struct Cyc_Absyn_Exp* e); static void Cyc_Tcexp_tcExpList( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* es){ for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd);}} void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, struct _tagged_arr msg_part){ Cyc_Tcutil_check_contains_assign(
e); Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_sint_t, e); if( ! Cyc_Tcutil_coerce_to_bool(
te, e)){({ void(* _temp164)( struct Cyc_Position_Segment*, struct _tagged_arr
fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp169= e->loc; struct _tagged_arr _temp170= _tag_arr("test of %s has type %s instead of integral or * type",
sizeof( unsigned char), 53u); struct Cyc_Stdio_String_pa_struct _temp172;
_temp172.tag= Cyc_Stdio_String_pa; _temp172.f1=( struct _tagged_arr) msg_part;{
void* _temp167=( void*)& _temp172; struct Cyc_Stdio_String_pa_struct _temp171;
_temp171.tag= Cyc_Stdio_String_pa; _temp171.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp168=(
void*)& _temp171; void* _temp165[ 2u]={ _temp167, _temp168}; struct _tagged_arr
_temp166={( void*) _temp165,( void*) _temp165,( void*)( _temp165 + 2u)};
_temp164( _temp169, _temp170, _temp166);}}});}} static void* Cyc_Tcexp_tcConst(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
c, struct Cyc_Absyn_Exp* e){ void* t;{ void* _temp173= c; void* _temp189; void*
_temp191; void* _temp193; int _temp195; void* _temp197; struct _tagged_arr
_temp199; _LL175: if(( unsigned int) _temp173 > 1u?*(( int*) _temp173) == Cyc_Absyn_Char_c:
0){ _LL190: _temp189=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp173)->f1;
goto _LL176;} else{ goto _LL177;} _LL177: if(( unsigned int) _temp173 > 1u?*((
int*) _temp173) == Cyc_Absyn_Short_c: 0){ _LL192: _temp191=( void*)(( struct Cyc_Absyn_Short_c_struct*)
_temp173)->f1; goto _LL178;} else{ goto _LL179;} _LL179: if(( unsigned int)
_temp173 > 1u?*(( int*) _temp173) == Cyc_Absyn_LongLong_c: 0){ _LL194: _temp193=(
void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp173)->f1; goto _LL180;} else{
goto _LL181;} _LL181: if(( unsigned int) _temp173 > 1u?*(( int*) _temp173) ==
Cyc_Absyn_Float_c: 0){ goto _LL182;} else{ goto _LL183;} _LL183: if((
unsigned int) _temp173 > 1u?*(( int*) _temp173) == Cyc_Absyn_Int_c: 0){ _LL198:
_temp197=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp173)->f1; goto _LL196;
_LL196: _temp195=(( struct Cyc_Absyn_Int_c_struct*) _temp173)->f2; goto _LL184;}
else{ goto _LL185;} _LL185: if(( unsigned int) _temp173 > 1u?*(( int*) _temp173)
== Cyc_Absyn_String_c: 0){ _LL200: _temp199=(( struct Cyc_Absyn_String_c_struct*)
_temp173)->f1; goto _LL186;} else{ goto _LL187;} _LL187: if( _temp173 ==( void*)
Cyc_Absyn_Null_c){ goto _LL188;} else{ goto _LL174;} _LL176: t= _temp189 ==(
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t; goto _LL174;
_LL178: t= _temp191 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;
goto _LL174; _LL180: t= _temp193 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ulonglong_t:
Cyc_Absyn_slonglong_t; goto _LL174; _LL182: t= Cyc_Absyn_float_t; goto _LL174;
_LL184: if( topt == 0){ t= _temp197 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t;} else{ void* _temp201= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); void* _temp211; void* _temp213; void* _temp215; void*
_temp217; void* _temp219; _LL203: if(( unsigned int) _temp201 > 4u?*(( int*)
_temp201) == Cyc_Absyn_IntType: 0){ _LL214: _temp213=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp201)->f1; goto _LL212; _LL212: _temp211=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp201)->f2; if( _temp211 ==( void*) Cyc_Absyn_B1){ goto _LL204;} else{ goto
_LL205;}} else{ goto _LL205;} _LL205: if(( unsigned int) _temp201 > 4u?*(( int*)
_temp201) == Cyc_Absyn_IntType: 0){ _LL218: _temp217=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp201)->f1; goto _LL216; _LL216: _temp215=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp201)->f2; if( _temp215 ==( void*) Cyc_Absyn_B2){ goto _LL206;} else{ goto
_LL207;}} else{ goto _LL207;} _LL207: if(( unsigned int) _temp201 > 4u?*(( int*)
_temp201) == Cyc_Absyn_IntType: 0){ _LL220: _temp219=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp201)->f1; goto _LL208;} else{ goto _LL209;} _LL209: goto _LL210; _LL204: t=
_temp213 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t;(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp221=(
struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp221[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp222; _temp222.tag= Cyc_Absyn_Const_e;
_temp222.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp223=(
struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp223[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp224; _temp224.tag= Cyc_Absyn_Char_c;
_temp224.f1=( void*) _temp213; _temp224.f2=( unsigned char) _temp195; _temp224;});
_temp223;})); _temp222;}); _temp221;}))); goto _LL202; _LL206: t= _temp217 ==(
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp225=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp225[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp226; _temp226.tag= Cyc_Absyn_Const_e; _temp226.f1=( void*)(( void*)({
struct Cyc_Absyn_Short_c_struct* _temp227=( struct Cyc_Absyn_Short_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Short_c_struct)); _temp227[ 0]=({ struct Cyc_Absyn_Short_c_struct
_temp228; _temp228.tag= Cyc_Absyn_Short_c; _temp228.f1=( void*) _temp217;
_temp228.f2=( short) _temp195; _temp228;}); _temp227;})); _temp226;}); _temp225;})));
goto _LL202; _LL208: t= _temp219 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL202; _LL210: t= _temp197 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL202; _LL202:;} goto _LL174; _LL186: {
int len=( int) _get_arr_size( _temp199, sizeof( unsigned char)); struct Cyc_Absyn_Const_e_struct*
_temp229=({ struct Cyc_Absyn_Const_e_struct* _temp266=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp266[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp267; _temp267.tag= Cyc_Absyn_Const_e; _temp267.f1=( void*)(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp268=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp268[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp269; _temp269.tag= Cyc_Absyn_Int_c; _temp269.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp269.f2= len; _temp269;}); _temp268;})); _temp267;}); _temp266;}); struct
Cyc_Absyn_Exp* elen= Cyc_Absyn_new_exp(( void*) _temp229, loc); elen->topt=({
struct Cyc_Core_Opt* _temp230=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp230->v=( void*) Cyc_Absyn_uint_t; _temp230;}); t= Cyc_Absyn_atb_typ(
Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_const_tqual(),( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp231=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp231[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp232; _temp232.tag= Cyc_Absyn_Upper_b; _temp232.f1= elen; _temp232;});
_temp231;})); if( topt != 0){ void* _temp233= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_Absyn_Exp* _temp241; struct Cyc_Absyn_Tqual
_temp243; void* _temp245; struct Cyc_Absyn_PtrInfo _temp247; struct Cyc_Absyn_Conref*
_temp249; struct Cyc_Absyn_Tqual _temp251; struct Cyc_Absyn_Conref* _temp253;
void* _temp255; void* _temp257; _LL235: if(( unsigned int) _temp233 > 4u?*(( int*)
_temp233) == Cyc_Absyn_ArrayType: 0){ _LL246: _temp245=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp233)->f1; goto _LL244; _LL244: _temp243=(( struct Cyc_Absyn_ArrayType_struct*)
_temp233)->f2; goto _LL242; _LL242: _temp241=(( struct Cyc_Absyn_ArrayType_struct*)
_temp233)->f3; goto _LL236;} else{ goto _LL237;} _LL237: if(( unsigned int)
_temp233 > 4u?*(( int*) _temp233) == Cyc_Absyn_PointerType: 0){ _LL248: _temp247=((
struct Cyc_Absyn_PointerType_struct*) _temp233)->f1; _LL258: _temp257=( void*)
_temp247.elt_typ; goto _LL256; _LL256: _temp255=( void*) _temp247.rgn_typ; goto
_LL254; _LL254: _temp253= _temp247.nullable; goto _LL252; _LL252: _temp251=
_temp247.tq; goto _LL250; _LL250: _temp249= _temp247.bounds; goto _LL238;} else{
goto _LL239;} _LL239: goto _LL240; _LL236: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp259=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp259[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp260; _temp260.tag= Cyc_Absyn_ArrayType;
_temp260.f1=( void*) Cyc_Absyn_uchar_t; _temp260.f2= _temp243; _temp260.f3=(
struct Cyc_Absyn_Exp*) elen; _temp260;}); _temp259;}); _LL238: if( ! Cyc_Tcutil_unify(*((
void**) _check_null( topt)), t)? Cyc_Tcutil_silent_castable( te, loc, t,*(( void**)
_check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp261=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp261->v=( void*) t; _temp261;});
Cyc_Tcutil_unchecked_cast( te, e,*(( void**) _check_null( topt))); t=*(( void**)
_check_null( topt));} else{ t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp262=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp262->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp262;})), Cyc_Absyn_const_tqual(),( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp263=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp263[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp264; _temp264.tag= Cyc_Absyn_Upper_b; _temp264.f1= elen; _temp264;});
_temp263;})); if( ! Cyc_Tcutil_unify(*(( void**) _check_null( topt)), t)? Cyc_Tcutil_silent_castable(
te, loc, t,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp265=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp265->v=( void*) t; _temp265;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**)
_check_null( topt))); t=*(( void**) _check_null( topt));}} goto _LL234; _LL240:
goto _LL234; _LL234:;} return t;} _LL188: { struct Cyc_List_List* _temp270= Cyc_Tcenv_lookup_type_vars(
te); t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp271=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp271[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp272; _temp272.tag= Cyc_Absyn_PointerType;
_temp272.f1=({ struct Cyc_Absyn_PtrInfo _temp273; _temp273.elt_typ=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_ak,({ struct Cyc_Core_Opt* _temp275=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp275->v=( void*)
_temp270; _temp275;})); _temp273.rgn_typ=( void*) Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp274=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp274->v=( void*) _temp270;
_temp274;})); _temp273.nullable=(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
1); _temp273.tq= Cyc_Absyn_empty_tqual(); _temp273.bounds= Cyc_Absyn_empty_conref();
_temp273;}); _temp272;}); _temp271;}); goto _LL174;} _LL174:;} return t;} static
void* Cyc_Tcexp_tcVar( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tuple1* q, void* b){ void* _temp276= b; struct Cyc_Absyn_Vardecl*
_temp290; struct Cyc_Absyn_Fndecl* _temp292; struct Cyc_Absyn_Vardecl* _temp294;
struct Cyc_Absyn_Vardecl* _temp296; struct Cyc_Absyn_Vardecl* _temp298; _LL278:
if( _temp276 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL279;} else{ goto _LL280;}
_LL280: if(( unsigned int) _temp276 > 1u?*(( int*) _temp276) == Cyc_Absyn_Global_b:
0){ _LL291: _temp290=(( struct Cyc_Absyn_Global_b_struct*) _temp276)->f1; goto
_LL281;} else{ goto _LL282;} _LL282: if(( unsigned int) _temp276 > 1u?*(( int*)
_temp276) == Cyc_Absyn_Funname_b: 0){ _LL293: _temp292=(( struct Cyc_Absyn_Funname_b_struct*)
_temp276)->f1; goto _LL283;} else{ goto _LL284;} _LL284: if(( unsigned int)
_temp276 > 1u?*(( int*) _temp276) == Cyc_Absyn_Pat_b: 0){ _LL295: _temp294=((
struct Cyc_Absyn_Pat_b_struct*) _temp276)->f1; goto _LL285;} else{ goto _LL286;}
_LL286: if(( unsigned int) _temp276 > 1u?*(( int*) _temp276) == Cyc_Absyn_Local_b:
0){ _LL297: _temp296=(( struct Cyc_Absyn_Local_b_struct*) _temp276)->f1; goto
_LL287;} else{ goto _LL288;} _LL288: if(( unsigned int) _temp276 > 1u?*(( int*)
_temp276) == Cyc_Absyn_Param_b: 0){ _LL299: _temp298=(( struct Cyc_Absyn_Param_b_struct*)
_temp276)->f1; goto _LL289;} else{ goto _LL277;} _LL279: return({ void*(*
_temp300)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct
_tagged_arr msg, struct _tagged_arr ap)= Cyc_Tcexp_expr_err; struct Cyc_Tcenv_Tenv*
_temp304= te; struct Cyc_Position_Segment* _temp305= loc; struct _tagged_arr
_temp306= _tag_arr("unbound identifier %s", sizeof( unsigned char), 22u); struct
Cyc_Stdio_String_pa_struct _temp307; _temp307.tag= Cyc_Stdio_String_pa; _temp307.f1=(
struct _tagged_arr) Cyc_Absynpp_qvar2string( q);{ void* _temp303=( void*)&
_temp307; void* _temp301[ 1u]={ _temp303}; struct _tagged_arr _temp302={( void*)
_temp301,( void*) _temp301,( void*)( _temp301 + 1u)}; _temp300( _temp304,
_temp305, _temp306, _temp302);}}); _LL281: q[ _check_known_subscript_notnull( 1u,
0)]=( _temp290->name)[ _check_known_subscript_notnull( 1u, 0)]; return( void*)
_temp290->type; _LL283: q[ _check_known_subscript_notnull( 1u, 0)]=( _temp292->name)[
_check_known_subscript_notnull( 1u, 0)]; return Cyc_Tcutil_fndecl2typ( _temp292);
_LL285: _temp296= _temp294; goto _LL287; _LL287: _temp298= _temp296; goto _LL289;
_LL289:(* q).f1=( void*) Cyc_Absyn_Loc_n; return( void*) _temp298->type; _LL277:;}
static void Cyc_Tcexp_check_format_args( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
fmt, struct Cyc_Core_Opt* opt_args, struct Cyc_List_List*(* type_getter)( struct
Cyc_Tcenv_Tenv*, struct _tagged_arr, struct Cyc_Position_Segment*)){ struct Cyc_List_List*
desc_types;{ void* _temp308=( void*) fmt->r; void* _temp316; struct _tagged_arr
_temp318; struct Cyc_Absyn_Exp* _temp320; struct Cyc_Absyn_Exp _temp322; void*
_temp323; void* _temp325; struct _tagged_arr _temp327; _LL310: if(*(( int*)
_temp308) == Cyc_Absyn_Const_e){ _LL317: _temp316=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp308)->f1; if(( unsigned int) _temp316 > 1u?*(( int*) _temp316) == Cyc_Absyn_String_c:
0){ _LL319: _temp318=(( struct Cyc_Absyn_String_c_struct*) _temp316)->f1; goto
_LL311;} else{ goto _LL312;}} else{ goto _LL312;} _LL312: if(*(( int*) _temp308)
== Cyc_Absyn_Cast_e){ _LL321: _temp320=(( struct Cyc_Absyn_Cast_e_struct*)
_temp308)->f2; _temp322=* _temp320; _LL324: _temp323=( void*) _temp322.r; if(*((
int*) _temp323) == Cyc_Absyn_Const_e){ _LL326: _temp325=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp323)->f1; if(( unsigned int) _temp325 > 1u?*(( int*) _temp325) == Cyc_Absyn_String_c:
0){ _LL328: _temp327=(( struct Cyc_Absyn_String_c_struct*) _temp325)->f1; goto
_LL313;} else{ goto _LL314;}} else{ goto _LL314;}} else{ goto _LL314;} _LL314:
goto _LL315; _LL311: _temp327= _temp318; goto _LL313; _LL313: desc_types=
type_getter( te, _temp327, fmt->loc); goto _LL309; _LL315: return; _LL309:;} if(
opt_args != 0){ struct Cyc_List_List* _temp329=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( opt_args))->v; for( 0; desc_types != 0? _temp329 !=
0: 0;( desc_types=(( struct Cyc_List_List*) _check_null( desc_types))->tl,
_temp329=(( struct Cyc_List_List*) _check_null( _temp329))->tl)){ void* t=( void*)((
struct Cyc_List_List*) _check_null( desc_types))->hd; struct Cyc_Absyn_Exp* e=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp329))->hd; if(
! Cyc_Tcutil_coerce_arg( te, e, t)){({ void(* _temp330)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp335= e->loc; struct _tagged_arr _temp336= _tag_arr("descriptor has type %s but argument has type %s",
sizeof( unsigned char), 48u); struct Cyc_Stdio_String_pa_struct _temp338;
_temp338.tag= Cyc_Stdio_String_pa; _temp338.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp333=( void*)& _temp338; struct Cyc_Stdio_String_pa_struct
_temp337; _temp337.tag= Cyc_Stdio_String_pa; _temp337.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{
void* _temp334=( void*)& _temp337; void* _temp331[ 2u]={ _temp333, _temp334};
struct _tagged_arr _temp332={( void*) _temp331,( void*) _temp331,( void*)(
_temp331 + 2u)}; _temp330( _temp335, _temp336, _temp332);}}});}} if( desc_types
!= 0){({ void(* _temp339)( struct Cyc_Position_Segment*, struct _tagged_arr fmt,
struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp342=
fmt->loc; struct _tagged_arr _temp343= _tag_arr("too few arguments", sizeof(
unsigned char), 18u); void* _temp340[ 0u]={}; struct _tagged_arr _temp341={(
void*) _temp340,( void*) _temp340,( void*)( _temp340 + 0u)}; _temp339( _temp342,
_temp343, _temp341);});} if( _temp329 != 0){({ void(* _temp344)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp347=(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp329))->hd)->loc; struct _tagged_arr _temp348= _tag_arr("too many arguments",
sizeof( unsigned char), 19u); void* _temp345[ 0u]={}; struct _tagged_arr
_temp346={( void*) _temp345,( void*) _temp345,( void*)( _temp345 + 0u)};
_temp344( _temp347, _temp348, _temp346);});}}} static void* Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
p, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); void* _temp349= p; _LL351: if( _temp349 ==( void*)
Cyc_Absyn_Plus){ goto _LL352;} else{ goto _LL353;} _LL353: if( _temp349 ==( void*)
Cyc_Absyn_Minus){ goto _LL354;} else{ goto _LL355;} _LL355: if( _temp349 ==(
void*) Cyc_Absyn_Not){ goto _LL356;} else{ goto _LL357;} _LL357: if( _temp349 ==(
void*) Cyc_Absyn_Bitnot){ goto _LL358;} else{ goto _LL359;} _LL359: if( _temp349
==( void*) Cyc_Absyn_Size){ goto _LL360;} else{ goto _LL361;} _LL361: goto
_LL362; _LL352: goto _LL354; _LL354: if( ! Cyc_Tcutil_is_numeric( e)){({ void(*
_temp363)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp367= loc;
struct _tagged_arr _temp368= _tag_arr("expecting arithmetic type but found %s",
sizeof( unsigned char), 39u); struct Cyc_Stdio_String_pa_struct _temp369;
_temp369.tag= Cyc_Stdio_String_pa; _temp369.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp366=( void*)& _temp369; void* _temp364[ 1u]={ _temp366}; struct
_tagged_arr _temp365={( void*) _temp364,( void*) _temp364,( void*)( _temp364 + 1u)};
_temp363( _temp367, _temp368, _temp365);}});} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; _LL356: Cyc_Tcutil_check_contains_assign( e); if( !
Cyc_Tcutil_coerce_to_bool( te, e)){({ void(* _temp370)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp374= loc; struct _tagged_arr _temp375= _tag_arr("expecting integral or * type but found %s",
sizeof( unsigned char), 42u); struct Cyc_Stdio_String_pa_struct _temp376;
_temp376.tag= Cyc_Stdio_String_pa; _temp376.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp373=( void*)& _temp376; void* _temp371[ 1u]={ _temp373}; struct
_tagged_arr _temp372={( void*) _temp371,( void*) _temp371,( void*)( _temp371 + 1u)};
_temp370( _temp374, _temp375, _temp372);}});} return Cyc_Absyn_sint_t; _LL358:
if( ! Cyc_Tcutil_is_integral( e)){({ void(* _temp377)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp381= loc; struct _tagged_arr _temp382= _tag_arr("expecting integral type but found %s",
sizeof( unsigned char), 37u); struct Cyc_Stdio_String_pa_struct _temp383;
_temp383.tag= Cyc_Stdio_String_pa; _temp383.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp380=( void*)& _temp383; void* _temp378[ 1u]={ _temp380}; struct
_tagged_arr _temp379={( void*) _temp378,( void*) _temp378,( void*)( _temp378 + 1u)};
_temp377( _temp381, _temp382, _temp379);}});} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; _LL360:{ void* _temp384= t; struct Cyc_Absyn_PtrInfo
_temp392; _LL386: if(( unsigned int) _temp384 > 4u?*(( int*) _temp384) == Cyc_Absyn_ArrayType:
0){ goto _LL387;} else{ goto _LL388;} _LL388: if(( unsigned int) _temp384 > 4u?*((
int*) _temp384) == Cyc_Absyn_PointerType: 0){ _LL393: _temp392=(( struct Cyc_Absyn_PointerType_struct*)
_temp384)->f1; goto _LL389;} else{ goto _LL390;} _LL390: goto _LL391; _LL387:
goto _LL385; _LL389: goto _LL385; _LL391:({ void(* _temp394)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp398= loc; struct _tagged_arr _temp399= _tag_arr("size requires pointer or array type, not %s",
sizeof( unsigned char), 44u); struct Cyc_Stdio_String_pa_struct _temp400;
_temp400.tag= Cyc_Stdio_String_pa; _temp400.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp397=( void*)& _temp400; void* _temp395[ 1u]={ _temp397}; struct
_tagged_arr _temp396={( void*) _temp395,( void*) _temp395,( void*)( _temp395 + 1u)};
_temp394( _temp398, _temp399, _temp396);}}); goto _LL385; _LL385:;} return Cyc_Absyn_uint_t;
_LL362: return({ void*(* _temp401)( struct _tagged_arr fmt, struct _tagged_arr
ap)= Cyc_Tcutil_impos; struct _tagged_arr _temp404= _tag_arr("Non-unary primop",
sizeof( unsigned char), 17u); void* _temp402[ 0u]={}; struct _tagged_arr
_temp403={( void*) _temp402,( void*) _temp402,( void*)( _temp402 + 0u)};
_temp401( _temp404, _temp403);}); _LL350:;} static void* Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
int(* checker)( struct Cyc_Absyn_Exp*)){ if( ! checker( e1)){({ void(* _temp405)(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp409= e1->loc; struct
_tagged_arr _temp410= _tag_arr("type %s cannot be used here", sizeof(
unsigned char), 28u); struct Cyc_Stdio_String_pa_struct _temp411; _temp411.tag=
Cyc_Stdio_String_pa; _temp411.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ void* _temp408=(
void*)& _temp411; void* _temp406[ 1u]={ _temp408}; struct _tagged_arr _temp407={(
void*) _temp406,( void*) _temp406,( void*)( _temp406 + 1u)}; _temp405( _temp409,
_temp410, _temp407);}}); return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt*
_temp412=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp412->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp412;}));} if( !
checker( e2)){({ void(* _temp413)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp417= e2->loc; struct _tagged_arr _temp418= _tag_arr("type %s cannot be used here",
sizeof( unsigned char), 28u); struct Cyc_Stdio_String_pa_struct _temp419;
_temp419.tag= Cyc_Stdio_String_pa; _temp419.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);{ void* _temp416=(
void*)& _temp419; void* _temp414[ 1u]={ _temp416}; struct _tagged_arr _temp415={(
void*) _temp414,( void*) _temp414,( void*)( _temp414 + 1u)}; _temp413( _temp417,
_temp418, _temp415);}}); return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt*
_temp420=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp420->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp420;}));}{ void* t1=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);
void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);
return Cyc_Tcutil_max_arithmetic_type( t1, t2);}} static void* Cyc_Tcexp_tcPlus(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){
void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);
void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);
void* _temp421= t1; struct Cyc_Absyn_PtrInfo _temp427; struct Cyc_Absyn_Conref*
_temp429; struct Cyc_Absyn_Tqual _temp431; struct Cyc_Absyn_Conref* _temp433;
void* _temp435; void* _temp437; _LL423: if(( unsigned int) _temp421 > 4u?*(( int*)
_temp421) == Cyc_Absyn_PointerType: 0){ _LL428: _temp427=(( struct Cyc_Absyn_PointerType_struct*)
_temp421)->f1; _LL438: _temp437=( void*) _temp427.elt_typ; goto _LL436; _LL436:
_temp435=( void*) _temp427.rgn_typ; goto _LL434; _LL434: _temp433= _temp427.nullable;
goto _LL432; _LL432: _temp431= _temp427.tq; goto _LL430; _LL430: _temp429=
_temp427.bounds; goto _LL424;} else{ goto _LL425;} _LL425: goto _LL426; _LL424:
if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){({ void(* _temp439)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp443= e2->loc; struct _tagged_arr _temp444= _tag_arr("expecting int but found %s",
sizeof( unsigned char), 27u); struct Cyc_Stdio_String_pa_struct _temp445;
_temp445.tag= Cyc_Stdio_String_pa; _temp445.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t2);{ void* _temp442=( void*)& _temp445; void* _temp440[ 1u]={ _temp442}; struct
_tagged_arr _temp441={( void*) _temp440,( void*) _temp440,( void*)( _temp440 + 1u)};
_temp439( _temp443, _temp444, _temp441);}});} _temp429= Cyc_Absyn_compress_conref(
_temp429);{ void* _temp446=( void*) _temp429->v; void* _temp454; void* _temp456;
struct Cyc_Absyn_Exp* _temp458; _LL448: if(( unsigned int) _temp446 > 1u?*(( int*)
_temp446) == Cyc_Absyn_Eq_constr: 0){ _LL455: _temp454=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp446)->f1; if( _temp454 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL449;} else{
goto _LL450;}} else{ goto _LL450;} _LL450: if(( unsigned int) _temp446 > 1u?*((
int*) _temp446) == Cyc_Absyn_Eq_constr: 0){ _LL457: _temp456=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp446)->f1; if(( unsigned int) _temp456 > 1u?*((
int*) _temp456) == Cyc_Absyn_Upper_b: 0){ _LL459: _temp458=(( struct Cyc_Absyn_Upper_b_struct*)
_temp456)->f1; goto _LL451;} else{ goto _LL452;}} else{ goto _LL452;} _LL452:
goto _LL453; _LL449: return t1; _LL451: { struct Cyc_Absyn_PointerType_struct*
_temp460=({ struct Cyc_Absyn_PointerType_struct* _temp461=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp461[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp462; _temp462.tag= Cyc_Absyn_PointerType;
_temp462.f1=({ struct Cyc_Absyn_PtrInfo _temp463; _temp463.elt_typ=( void*)
_temp437; _temp463.rgn_typ=( void*) _temp435; _temp463.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 1); _temp463.tq= _temp431; _temp463.bounds= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_Unknown_b); _temp463;}); _temp462;}); _temp461;}); Cyc_Tcutil_unchecked_cast(
te, e1,( void*) _temp460); return( void*) _temp460;} _LL453:( void*)( _temp429->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp464=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp464[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp465; _temp465.tag= Cyc_Absyn_Eq_constr; _temp465.f1=(
void*)(( void*) Cyc_Absyn_Unknown_b); _temp465;}); _temp464;}))); return t1;
_LL447:;} _LL426: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL422:;} static void* Cyc_Tcexp_tcMinus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){ if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){ if( ! Cyc_Tcutil_unify( t1, t2)){({ void(* _temp466)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp471= e1->loc; struct _tagged_arr _temp472= _tag_arr("pointer arithmetic on values of different types (%s != %s)",
sizeof( unsigned char), 59u); struct Cyc_Stdio_String_pa_struct _temp474;
_temp474.tag= Cyc_Stdio_String_pa; _temp474.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ void* _temp469=(
void*)& _temp474; struct Cyc_Stdio_String_pa_struct _temp473; _temp473.tag= Cyc_Stdio_String_pa;
_temp473.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v);{ void* _temp470=( void*)& _temp473; void* _temp467[
2u]={ _temp469, _temp470}; struct _tagged_arr _temp468={( void*) _temp467,( void*)
_temp467,( void*)( _temp467 + 2u)}; _temp466( _temp471, _temp472, _temp468);}}});}
return Cyc_Absyn_sint_t;} else{ if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){({
void(* _temp475)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp480= e2->loc;
struct _tagged_arr _temp481= _tag_arr("expecting either %s or int but found %s",
sizeof( unsigned char), 40u); struct Cyc_Stdio_String_pa_struct _temp483;
_temp483.tag= Cyc_Stdio_String_pa; _temp483.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp478=( void*)& _temp483; struct Cyc_Stdio_String_pa_struct
_temp482; _temp482.tag= Cyc_Stdio_String_pa; _temp482.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t2);{ void* _temp479=( void*)& _temp482; void* _temp476[
2u]={ _temp478, _temp479}; struct _tagged_arr _temp477={( void*) _temp476,( void*)
_temp476,( void*)( _temp476 + 2u)}; _temp475( _temp480, _temp481, _temp477);}}});}
return t1;}} else{ return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);}}
static void* Cyc_Tcexp_tcAnyBinop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ int e1_is_num= Cyc_Tcutil_is_numeric(
e1); int e2_is_num= Cyc_Tcutil_is_numeric( e2); void* t1= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v); if( e1_is_num?
e2_is_num: 0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_unify( t1, t2)?
Cyc_Tcutil_unify( t1, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_bk,({
struct Cyc_Core_Opt* _temp484=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp484->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp484;}))):
0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_silent_castable( te, loc, t2,
t1)){ Cyc_Tcutil_unchecked_cast( te, e2, t1); return Cyc_Absyn_sint_t;} else{
if( Cyc_Tcutil_silent_castable( te, loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te,
e1, t2); return Cyc_Absyn_sint_t;} else{ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v;{ struct _tuple0 _temp486=({ struct _tuple0 _temp485;
_temp485.f1= Cyc_Tcutil_compress( t1); _temp485.f2= Cyc_Tcutil_compress( t2);
_temp485;}); void* _temp492; struct Cyc_Absyn_PtrInfo _temp494; void* _temp496;
void* _temp498; struct Cyc_Absyn_PtrInfo _temp500; void* _temp502; _LL488:
_LL499: _temp498= _temp486.f1; if(( unsigned int) _temp498 > 4u?*(( int*)
_temp498) == Cyc_Absyn_PointerType: 0){ _LL501: _temp500=(( struct Cyc_Absyn_PointerType_struct*)
_temp498)->f1; _LL503: _temp502=( void*) _temp500.elt_typ; goto _LL493;} else{
goto _LL490;} _LL493: _temp492= _temp486.f2; if(( unsigned int) _temp492 > 4u?*((
int*) _temp492) == Cyc_Absyn_PointerType: 0){ _LL495: _temp494=(( struct Cyc_Absyn_PointerType_struct*)
_temp492)->f1; _LL497: _temp496=( void*) _temp494.elt_typ; goto _LL489;} else{
goto _LL490;} _LL490: goto _LL491; _LL489: if( Cyc_Tcutil_unify( _temp502,
_temp496)){ return Cyc_Absyn_sint_t;} goto _LL487; _LL491: goto _LL487; _LL487:;}({
void(* _temp504)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp509= loc;
struct _tagged_arr _temp510= _tag_arr("comparison not allowed between %s and %s",
sizeof( unsigned char), 41u); struct Cyc_Stdio_String_pa_struct _temp512;
_temp512.tag= Cyc_Stdio_String_pa; _temp512.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp507=( void*)& _temp512; struct Cyc_Stdio_String_pa_struct
_temp511; _temp511.tag= Cyc_Stdio_String_pa; _temp511.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t2);{ void* _temp508=( void*)& _temp511; void* _temp505[
2u]={ _temp507, _temp508}; struct _tagged_arr _temp506={( void*) _temp505,( void*)
_temp505,( void*)( _temp505 + 2u)}; _temp504( _temp509, _temp510, _temp506);}}});
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp513=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp513->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp513;}));}}}}} static void* Cyc_Tcexp_tcBinPrimop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* p, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* _temp514= p; _LL516: if( _temp514 ==( void*)
Cyc_Absyn_Plus){ goto _LL517;} else{ goto _LL518;} _LL518: if( _temp514 ==( void*)
Cyc_Absyn_Minus){ goto _LL519;} else{ goto _LL520;} _LL520: if( _temp514 ==(
void*) Cyc_Absyn_Times){ goto _LL521;} else{ goto _LL522;} _LL522: if( _temp514
==( void*) Cyc_Absyn_Div){ goto _LL523;} else{ goto _LL524;} _LL524: if(
_temp514 ==( void*) Cyc_Absyn_Mod){ goto _LL525;} else{ goto _LL526;} _LL526:
if( _temp514 ==( void*) Cyc_Absyn_Bitand){ goto _LL527;} else{ goto _LL528;}
_LL528: if( _temp514 ==( void*) Cyc_Absyn_Bitor){ goto _LL529;} else{ goto
_LL530;} _LL530: if( _temp514 ==( void*) Cyc_Absyn_Bitxor){ goto _LL531;} else{
goto _LL532;} _LL532: if( _temp514 ==( void*) Cyc_Absyn_Bitlshift){ goto _LL533;}
else{ goto _LL534;} _LL534: if( _temp514 ==( void*) Cyc_Absyn_Bitlrshift){ goto
_LL535;} else{ goto _LL536;} _LL536: if( _temp514 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL537;} else{ goto _LL538;} _LL538: if( _temp514 ==( void*) Cyc_Absyn_Eq){
goto _LL539;} else{ goto _LL540;} _LL540: if( _temp514 ==( void*) Cyc_Absyn_Neq){
goto _LL541;} else{ goto _LL542;} _LL542: if( _temp514 ==( void*) Cyc_Absyn_Gt){
goto _LL543;} else{ goto _LL544;} _LL544: if( _temp514 ==( void*) Cyc_Absyn_Lt){
goto _LL545;} else{ goto _LL546;} _LL546: if( _temp514 ==( void*) Cyc_Absyn_Gte){
goto _LL547;} else{ goto _LL548;} _LL548: if( _temp514 ==( void*) Cyc_Absyn_Lte){
goto _LL549;} else{ goto _LL550;} _LL550: goto _LL551; _LL517: return Cyc_Tcexp_tcPlus(
te, e1, e2); _LL519: return Cyc_Tcexp_tcMinus( te, e1, e2); _LL521: goto _LL523;
_LL523: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL525: goto _LL527; _LL527: goto _LL529; _LL529: goto _LL531; _LL531: goto
_LL533; _LL533: goto _LL535; _LL535: goto _LL537; _LL537: return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_integral); _LL539: goto _LL541; _LL541: goto _LL543;
_LL543: goto _LL545; _LL545: goto _LL547; _LL547: goto _LL549; _LL549: return
Cyc_Tcexp_tcAnyBinop( te, loc, e1, e2); _LL551:( int) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp552=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp552[ 0]=({ struct Cyc_Core_Failure_struct
_temp553; _temp553.tag= Cyc_Core_Failure; _temp553.f1= _tag_arr("bad binary primop",
sizeof( unsigned char), 18u); _temp553;}); _temp552;})); _LL515:;} static void*
Cyc_Tcexp_tcPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, void* p, struct Cyc_List_List* es){ if( p ==( void*) Cyc_Absyn_Minus?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es) == 1: 0){ return Cyc_Tcexp_tcExp(
te, topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);}
Cyc_Tcexp_tcExpList( te, es);{ void* t; switch((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es)){ case 0: _LL554: return({ void*(* _temp556)( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tagged_arr msg,
struct _tagged_arr ap)= Cyc_Tcexp_expr_err; struct Cyc_Tcenv_Tenv* _temp559= te;
struct Cyc_Position_Segment* _temp560= loc; struct _tagged_arr _temp561=
_tag_arr("primitive operator has 0 arguments", sizeof( unsigned char), 35u);
void* _temp557[ 0u]={}; struct _tagged_arr _temp558={( void*) _temp557,( void*)
_temp557,( void*)( _temp557 + 0u)}; _temp556( _temp559, _temp560, _temp561,
_temp558);}); case 1: _LL555: t= Cyc_Tcexp_tcUnPrimop( te, loc, topt, p,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd); break; case 2:
_LL562: t= Cyc_Tcexp_tcBinPrimop( te, loc, topt, p,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( es))->tl))->hd);
break; default: _LL563: return({ void*(* _temp565)( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct _tagged_arr msg, struct _tagged_arr ap)=
Cyc_Tcexp_expr_err; struct Cyc_Tcenv_Tenv* _temp568= te; struct Cyc_Position_Segment*
_temp569= loc; struct _tagged_arr _temp570= _tag_arr("primitive operator has > 2 arguments",
sizeof( unsigned char), 37u); void* _temp566[ 0u]={}; struct _tagged_arr
_temp567={( void*) _temp566,( void*) _temp566,( void*)( _temp566 + 0u)};
_temp565( _temp568, _temp569, _temp570, _temp567);});} return t;}} struct
_tuple8{ struct Cyc_Absyn_Tqual f1; void* f2; } ; static void Cyc_Tcexp_check_writable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){{ void* _temp571=( void*) e->r;
void* _temp595; struct Cyc_Absyn_Vardecl* _temp597; void* _temp599; struct Cyc_Absyn_Vardecl*
_temp601; void* _temp603; struct Cyc_Absyn_Vardecl* _temp605; void* _temp607;
struct Cyc_Absyn_Vardecl* _temp609; struct Cyc_Absyn_Exp* _temp611; struct Cyc_Absyn_Exp*
_temp613; struct _tagged_arr* _temp615; struct Cyc_Absyn_Exp* _temp617; struct
_tagged_arr* _temp619; struct Cyc_Absyn_Exp* _temp621; struct Cyc_Absyn_Exp*
_temp623; struct Cyc_Absyn_Exp* _temp625; struct Cyc_Absyn_Exp* _temp627; _LL573:
if(*(( int*) _temp571) == Cyc_Absyn_Var_e){ _LL596: _temp595=( void*)(( struct
Cyc_Absyn_Var_e_struct*) _temp571)->f2; if(( unsigned int) _temp595 > 1u?*(( int*)
_temp595) == Cyc_Absyn_Param_b: 0){ _LL598: _temp597=(( struct Cyc_Absyn_Param_b_struct*)
_temp595)->f1; goto _LL574;} else{ goto _LL575;}} else{ goto _LL575;} _LL575:
if(*(( int*) _temp571) == Cyc_Absyn_Var_e){ _LL600: _temp599=( void*)(( struct
Cyc_Absyn_Var_e_struct*) _temp571)->f2; if(( unsigned int) _temp599 > 1u?*(( int*)
_temp599) == Cyc_Absyn_Local_b: 0){ _LL602: _temp601=(( struct Cyc_Absyn_Local_b_struct*)
_temp599)->f1; goto _LL576;} else{ goto _LL577;}} else{ goto _LL577;} _LL577:
if(*(( int*) _temp571) == Cyc_Absyn_Var_e){ _LL604: _temp603=( void*)(( struct
Cyc_Absyn_Var_e_struct*) _temp571)->f2; if(( unsigned int) _temp603 > 1u?*(( int*)
_temp603) == Cyc_Absyn_Pat_b: 0){ _LL606: _temp605=(( struct Cyc_Absyn_Pat_b_struct*)
_temp603)->f1; goto _LL578;} else{ goto _LL579;}} else{ goto _LL579;} _LL579:
if(*(( int*) _temp571) == Cyc_Absyn_Var_e){ _LL608: _temp607=( void*)(( struct
Cyc_Absyn_Var_e_struct*) _temp571)->f2; if(( unsigned int) _temp607 > 1u?*(( int*)
_temp607) == Cyc_Absyn_Global_b: 0){ _LL610: _temp609=(( struct Cyc_Absyn_Global_b_struct*)
_temp607)->f1; goto _LL580;} else{ goto _LL581;}} else{ goto _LL581;} _LL581:
if(*(( int*) _temp571) == Cyc_Absyn_Subscript_e){ _LL614: _temp613=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp571)->f1; goto _LL612; _LL612: _temp611=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp571)->f2; goto _LL582;} else{ goto _LL583;} _LL583: if(*(( int*) _temp571)
== Cyc_Absyn_StructMember_e){ _LL618: _temp617=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp571)->f1; goto _LL616; _LL616: _temp615=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp571)->f2; goto _LL584;} else{ goto _LL585;} _LL585: if(*(( int*) _temp571)
== Cyc_Absyn_StructArrow_e){ _LL622: _temp621=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp571)->f1; goto _LL620; _LL620: _temp619=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp571)->f2; goto _LL586;} else{ goto _LL587;} _LL587: if(*(( int*) _temp571)
== Cyc_Absyn_Deref_e){ _LL624: _temp623=(( struct Cyc_Absyn_Deref_e_struct*)
_temp571)->f1; goto _LL588;} else{ goto _LL589;} _LL589: if(*(( int*) _temp571)
== Cyc_Absyn_NoInstantiate_e){ _LL626: _temp625=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp571)->f1; goto _LL590;} else{ goto _LL591;} _LL591: if(*(( int*) _temp571)
== Cyc_Absyn_Instantiate_e){ _LL628: _temp627=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp571)->f1; goto _LL592;} else{ goto _LL593;} _LL593: goto _LL594; _LL574:
_temp601= _temp597; goto _LL576; _LL576: _temp605= _temp601; goto _LL578; _LL578:
_temp609= _temp605; goto _LL580; _LL580: if( !( _temp609->tq).q_const){ return;}
goto _LL572; _LL582:{ void* _temp629= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp613->topt))->v); struct Cyc_Absyn_PtrInfo _temp639; struct Cyc_Absyn_Tqual
_temp641; struct Cyc_Absyn_Tqual _temp643; struct Cyc_List_List* _temp645;
_LL631: if(( unsigned int) _temp629 > 4u?*(( int*) _temp629) == Cyc_Absyn_PointerType:
0){ _LL640: _temp639=(( struct Cyc_Absyn_PointerType_struct*) _temp629)->f1;
_LL642: _temp641= _temp639.tq; goto _LL632;} else{ goto _LL633;} _LL633: if((
unsigned int) _temp629 > 4u?*(( int*) _temp629) == Cyc_Absyn_ArrayType: 0){
_LL644: _temp643=(( struct Cyc_Absyn_ArrayType_struct*) _temp629)->f2; goto
_LL634;} else{ goto _LL635;} _LL635: if(( unsigned int) _temp629 > 4u?*(( int*)
_temp629) == Cyc_Absyn_TupleType: 0){ _LL646: _temp645=(( struct Cyc_Absyn_TupleType_struct*)
_temp629)->f1; goto _LL636;} else{ goto _LL637;} _LL637: goto _LL638; _LL632:
_temp643= _temp641; goto _LL634; _LL634: if( ! _temp643.q_const){ return;} goto
_LL630; _LL636: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp611);{
struct _handler_cons _temp647; _push_handler(& _temp647);{ int _temp649= 0; if(
setjmp( _temp647.handler)){ _temp649= 1;} if( ! _temp649){{ struct _tuple8
_temp652; struct Cyc_Absyn_Tqual _temp653; struct _tuple8* _temp650=(( struct
_tuple8*(*)( struct Cyc_List_List* x, int i)) Cyc_List_nth)( _temp645,( int) i);
_temp652=* _temp650; _LL654: _temp653= _temp652.f1; goto _LL651; _LL651: if( !
_temp653.q_const){ _npop_handler( 0u); return;}}; _pop_handler();} else{ void*
_temp648=( void*) _exn_thrown; void* _temp656= _temp648; _LL658: if( _temp656 ==
Cyc_List_Nth){ goto _LL659;} else{ goto _LL660;} _LL660: goto _LL661; _LL659:
return; _LL661:( void) _throw( _temp656); _LL657:;}}} goto _LL630;} _LL638: goto
_LL630; _LL630:;} goto _LL572; _LL584:{ void* _temp662= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp617->topt))->v); struct Cyc_Absyn_Structdecl**
_temp674; struct Cyc_List_List* _temp676; struct Cyc_List_List* _temp678; struct
Cyc_Absyn_Uniondecl** _temp680; _LL664: if(( unsigned int) _temp662 > 4u?*(( int*)
_temp662) == Cyc_Absyn_StructType: 0){ _LL675: _temp674=(( struct Cyc_Absyn_StructType_struct*)
_temp662)->f3; goto _LL665;} else{ goto _LL666;} _LL666: if(( unsigned int)
_temp662 > 4u?*(( int*) _temp662) == Cyc_Absyn_AnonUnionType: 0){ _LL677:
_temp676=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp662)->f1; goto _LL667;}
else{ goto _LL668;} _LL668: if(( unsigned int) _temp662 > 4u?*(( int*) _temp662)
== Cyc_Absyn_AnonStructType: 0){ _LL679: _temp678=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp662)->f1; goto _LL669;} else{ goto _LL670;} _LL670: if(( unsigned int)
_temp662 > 4u?*(( int*) _temp662) == Cyc_Absyn_UnionType: 0){ _LL681: _temp680=((
struct Cyc_Absyn_UnionType_struct*) _temp662)->f3; goto _LL671;} else{ goto
_LL672;} _LL672: goto _LL673; _LL665: { struct Cyc_Absyn_Structfield* sf=
_temp674 == 0? 0: Cyc_Absyn_lookup_struct_field(( struct Cyc_Absyn_Structdecl*)*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp674)), _temp615); if( sf == 0?
1: !((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL663;} _LL667: _temp678= _temp676; goto _LL669; _LL669: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp678, _temp615); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL663;} _LL671: { struct Cyc_Absyn_Structfield*
sf= _temp680 == 0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp680)), _temp615); if( sf == 0? 1:
!((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL663;} _LL673: goto _LL663; _LL663:;} goto _LL572; _LL586:{ void*
_temp682= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp621->topt))->v); struct Cyc_Absyn_PtrInfo _temp688; void* _temp690; _LL684:
if(( unsigned int) _temp682 > 4u?*(( int*) _temp682) == Cyc_Absyn_PointerType: 0){
_LL689: _temp688=(( struct Cyc_Absyn_PointerType_struct*) _temp682)->f1; _LL691:
_temp690=( void*) _temp688.elt_typ; goto _LL685;} else{ goto _LL686;} _LL686:
goto _LL687; _LL685:{ void* _temp692= Cyc_Tcutil_compress( _temp690); struct Cyc_Absyn_Structdecl**
_temp704; struct Cyc_Absyn_Uniondecl** _temp706; struct Cyc_List_List* _temp708;
struct Cyc_List_List* _temp710; _LL694: if(( unsigned int) _temp692 > 4u?*(( int*)
_temp692) == Cyc_Absyn_StructType: 0){ _LL705: _temp704=(( struct Cyc_Absyn_StructType_struct*)
_temp692)->f3; goto _LL695;} else{ goto _LL696;} _LL696: if(( unsigned int)
_temp692 > 4u?*(( int*) _temp692) == Cyc_Absyn_UnionType: 0){ _LL707: _temp706=((
struct Cyc_Absyn_UnionType_struct*) _temp692)->f3; goto _LL697;} else{ goto
_LL698;} _LL698: if(( unsigned int) _temp692 > 4u?*(( int*) _temp692) == Cyc_Absyn_AnonStructType:
0){ _LL709: _temp708=(( struct Cyc_Absyn_AnonStructType_struct*) _temp692)->f1;
goto _LL699;} else{ goto _LL700;} _LL700: if(( unsigned int) _temp692 > 4u?*((
int*) _temp692) == Cyc_Absyn_AnonUnionType: 0){ _LL711: _temp710=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp692)->f1; goto _LL701;} else{ goto _LL702;} _LL702: goto _LL703; _LL695: {
struct Cyc_Absyn_Structfield* sf= _temp704 == 0? 0: Cyc_Absyn_lookup_struct_field((
struct Cyc_Absyn_Structdecl*)*(( struct Cyc_Absyn_Structdecl**) _check_null(
_temp704)), _temp619); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL693;} _LL697: { struct Cyc_Absyn_Structfield*
sf= _temp706 == 0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp706)), _temp619); if( sf == 0? 1:
!((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL693;} _LL699: _temp710= _temp708; goto _LL701; _LL701: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp710, _temp619); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL693;} _LL703: goto _LL693;
_LL693:;} goto _LL683; _LL687: goto _LL683; _LL683:;} goto _LL572; _LL588:{ void*
_temp712= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp623->topt))->v); struct Cyc_Absyn_PtrInfo _temp720; struct Cyc_Absyn_Tqual
_temp722; struct Cyc_Absyn_Tqual _temp724; _LL714: if(( unsigned int) _temp712 >
4u?*(( int*) _temp712) == Cyc_Absyn_PointerType: 0){ _LL721: _temp720=(( struct
Cyc_Absyn_PointerType_struct*) _temp712)->f1; _LL723: _temp722= _temp720.tq;
goto _LL715;} else{ goto _LL716;} _LL716: if(( unsigned int) _temp712 > 4u?*((
int*) _temp712) == Cyc_Absyn_ArrayType: 0){ _LL725: _temp724=(( struct Cyc_Absyn_ArrayType_struct*)
_temp712)->f2; goto _LL717;} else{ goto _LL718;} _LL718: goto _LL719; _LL715:
_temp724= _temp722; goto _LL717; _LL717: if( ! _temp724.q_const){ return;} goto
_LL713; _LL719: goto _LL713; _LL713:;} goto _LL572; _LL590: _temp627= _temp625;
goto _LL592; _LL592: Cyc_Tcexp_check_writable( te, _temp627); return; _LL594:
goto _LL572; _LL572:;}({ void(* _temp726)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp729= e->loc; struct _tagged_arr _temp730= _tag_arr("attempt to write a read-only location",
sizeof( unsigned char), 38u); void* _temp727[ 0u]={}; struct _tagged_arr
_temp728={( void*) _temp727,( void*) _temp727,( void*)( _temp727 + 0u)};
_temp726( _temp729, _temp730, _temp728);});} static void* Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, void* i){ Cyc_Tcexp_tcExpNoPromote( te, 0, e); if( ! Cyc_Absyn_is_lvalue(
e)){ return({ void*(* _temp731)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tagged_arr msg, struct _tagged_arr ap)= Cyc_Tcexp_expr_err; struct
Cyc_Tcenv_Tenv* _temp734= te; struct Cyc_Position_Segment* _temp735= loc; struct
_tagged_arr _temp736= _tag_arr("increment/decrement of non-lvalue", sizeof(
unsigned char), 34u); void* _temp732[ 0u]={}; struct _tagged_arr _temp733={(
void*) _temp732,( void*) _temp732,( void*)( _temp732 + 0u)}; _temp731( _temp734,
_temp735, _temp736, _temp733);});} Cyc_Tcexp_check_writable( te, e);{ void* t=(
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; if( ! Cyc_Tcutil_is_numeric(
e)? ! Cyc_Tcutil_is_tagged_pointer_typ( t): 0){({ void(* _temp737)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp741= e->loc; struct _tagged_arr _temp742= _tag_arr("expecting arithmetic or ? type but found %s",
sizeof( unsigned char), 44u); struct Cyc_Stdio_String_pa_struct _temp743;
_temp743.tag= Cyc_Stdio_String_pa; _temp743.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp740=( void*)& _temp743; void* _temp738[ 1u]={ _temp740}; struct
_tagged_arr _temp739={( void*) _temp738,( void*) _temp738,( void*)( _temp738 + 1u)};
_temp737( _temp741, _temp742, _temp739);}});} return t;}} static void* Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3){ Cyc_Tcexp_tcTest(
te, e1, _tag_arr("conditional expression", sizeof( unsigned char), 23u)); Cyc_Tcexp_tcExp(
te, topt, e2); Cyc_Tcexp_tcExp( te, topt, e3);{ void* t= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_mk,({ struct Cyc_Core_Opt* _temp757=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp757->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp757;})); struct Cyc_List_List* _temp744=({
struct Cyc_List_List* _temp756=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp756->hd=( void*) e3; _temp756->tl= 0; _temp756;});
struct Cyc_List_List* _temp745=({ struct Cyc_List_List* _temp755=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp755->hd=( void*) e2; _temp755->tl=
_temp744; _temp755;}); if( ! Cyc_Tcutil_coerce_list( te, t, _temp745)){({ void(*
_temp746)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp751= loc;
struct _tagged_arr _temp752= _tag_arr("conditional clause types do not match: %s != %s",
sizeof( unsigned char), 48u); struct Cyc_Stdio_String_pa_struct _temp754;
_temp754.tag= Cyc_Stdio_String_pa; _temp754.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);{ void* _temp749=(
void*)& _temp754; struct Cyc_Stdio_String_pa_struct _temp753; _temp753.tag= Cyc_Stdio_String_pa;
_temp753.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e3->topt))->v);{ void* _temp750=( void*)& _temp753; void* _temp747[
2u]={ _temp749, _temp750}; struct _tagged_arr _temp748={( void*) _temp747,( void*)
_temp747,( void*)( _temp747 + 2u)}; _temp746( _temp751, _temp752, _temp748);}}});}
return t;}} static void* Cyc_Tcexp_tcAssignOp( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Core_Opt*
po, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExpNoPromote( te, 0, e1); Cyc_Tcexp_tcExp(
te,( void**)(( void**)(( void*)&(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v)),
e2);{ void* t1=( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v; void*
t2=( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v;{ void* _temp758=
Cyc_Tcutil_compress( t1); _LL760: if(( unsigned int) _temp758 > 4u?*(( int*)
_temp758) == Cyc_Absyn_ArrayType: 0){ goto _LL761;} else{ goto _LL762;} _LL762:
goto _LL763; _LL761:({ void(* _temp764)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp767= loc; struct _tagged_arr _temp768= _tag_arr("cannot assign to an array",
sizeof( unsigned char), 26u); void* _temp765[ 0u]={}; struct _tagged_arr
_temp766={( void*) _temp765,( void*) _temp765,( void*)( _temp765 + 0u)};
_temp764( _temp767, _temp768, _temp766);}); goto _LL759; _LL763: goto _LL759;
_LL759:;} if( ! Cyc_Absyn_is_lvalue( e1)){ return({ void*(* _temp769)( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tagged_arr msg,
struct _tagged_arr ap)= Cyc_Tcexp_expr_err; struct Cyc_Tcenv_Tenv* _temp772= te;
struct Cyc_Position_Segment* _temp773= loc; struct _tagged_arr _temp774=
_tag_arr("assignment to non-lvalue", sizeof( unsigned char), 25u); void*
_temp770[ 0u]={}; struct _tagged_arr _temp771={( void*) _temp770,( void*)
_temp770,( void*)( _temp770 + 0u)}; _temp769( _temp772, _temp773, _temp774,
_temp771);});} Cyc_Tcexp_check_writable( te, e1); if( po == 0){ if( ! Cyc_Tcutil_coerce_assign(
te, e2, t1)){ return({ void*(* _temp775)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tagged_arr msg, struct _tagged_arr ap)= Cyc_Tcexp_expr_err; struct
Cyc_Tcenv_Tenv* _temp780= te; struct Cyc_Position_Segment* _temp781= loc; struct
_tagged_arr _temp782= _tag_arr("type mismatch: %s != %s", sizeof( unsigned char),
24u); struct Cyc_Stdio_String_pa_struct _temp784; _temp784.tag= Cyc_Stdio_String_pa;
_temp784.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t1);{ void* _temp778=(
void*)& _temp784; struct Cyc_Stdio_String_pa_struct _temp783; _temp783.tag= Cyc_Stdio_String_pa;
_temp783.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ void* _temp779=(
void*)& _temp783; void* _temp776[ 2u]={ _temp778, _temp779}; struct _tagged_arr
_temp777={( void*) _temp776,( void*) _temp776,( void*)( _temp776 + 2u)};
_temp775( _temp780, _temp781, _temp782, _temp777);}}});}} else{ void* _temp785=(
void*)(( struct Cyc_Core_Opt*) _check_null( po))->v; void* _temp786= Cyc_Tcexp_tcBinPrimop(
te, loc, 0, _temp785, e1, e2); if( !( Cyc_Tcutil_unify( _temp786, t1)? 1: Cyc_Tcutil_coerceable(
_temp786))){ return({ void*(* _temp787)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tagged_arr msg, struct _tagged_arr ap)= Cyc_Tcexp_expr_err; struct
Cyc_Tcenv_Tenv* _temp790= te; struct Cyc_Position_Segment* _temp791= loc; struct
_tagged_arr _temp792= _tag_arr("Cannot use this operator in an assignment",
sizeof( unsigned char), 42u); void* _temp788[ 0u]={}; struct _tagged_arr
_temp789={( void*) _temp788,( void*) _temp788,( void*)( _temp788 + 0u)};
_temp787( _temp790, _temp791, _temp792, _temp789);});} return _temp786;} return
t2;}} static void* Cyc_Tcexp_tcSeqExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp(
te, 0, e1); Cyc_Tcexp_tcExp( te, topt, e2); return( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v;} static struct Cyc_Absyn_Tunionfield* Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* tu, struct
_RegionHandle* r, struct Cyc_List_List* inst, struct Cyc_List_List* fs){ struct
Cyc_List_List* fields; void* t1=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v;{
void* _temp793= Cyc_Tcutil_compress( t1); void* _temp803; void* _temp805; _LL795:
if( _temp793 ==( void*) Cyc_Absyn_FloatType){ goto _LL796;} else{ goto _LL797;}
_LL797: if(( unsigned int) _temp793 > 4u?*(( int*) _temp793) == Cyc_Absyn_IntType:
0){ _LL804: _temp803=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp793)->f2;
if( _temp803 ==( void*) Cyc_Absyn_B1){ goto _LL798;} else{ goto _LL799;}} else{
goto _LL799;} _LL799: if(( unsigned int) _temp793 > 4u?*(( int*) _temp793) ==
Cyc_Absyn_IntType: 0){ _LL806: _temp805=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp793)->f2; if( _temp805 ==( void*) Cyc_Absyn_B2){ goto _LL800;} else{ goto
_LL801;}} else{ goto _LL801;} _LL801: goto _LL802; _LL796: Cyc_Tcutil_unchecked_cast(
te, e,( void*) Cyc_Absyn_DoubleType); t1=( void*) Cyc_Absyn_DoubleType; goto
_LL794; _LL798: goto _LL800; _LL800: Cyc_Tcutil_unchecked_cast( te, e, Cyc_Absyn_sint_t);
t1= Cyc_Absyn_sint_t; goto _LL794; _LL802: goto _LL794; _LL794:;} for( fields=
fs; fields != 0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){
void* _temp809; struct Cyc_Position_Segment* _temp811; struct Cyc_List_List*
_temp813; struct Cyc_List_List* _temp815; struct _tuple1* _temp817; struct Cyc_Absyn_Tunionfield
_temp807=*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd); _LL818: _temp817= _temp807.name; goto _LL816; _LL816: _temp815=
_temp807.tvs; goto _LL814; _LL814: _temp813= _temp807.typs; goto _LL812; _LL812:
_temp811= _temp807.loc; goto _LL810; _LL810: _temp809=( void*) _temp807.sc; goto
_LL808; _LL808: if(( _temp813 == 0? 1:(( struct Cyc_List_List*) _check_null(
_temp813))->tl != 0)? 1: _temp815 != 0){ continue;}{ void* t2= Cyc_Tcutil_rsubstitute(
r, inst,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp813))->hd)).f2);
if( Cyc_Tcutil_unify( t1, t2)){ return( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( fields))->hd);}}}
for( fields= fs; fields != 0; fields=(( struct Cyc_List_List*) _check_null(
fields))->tl){ void* _temp821; struct Cyc_Position_Segment* _temp823; struct Cyc_List_List*
_temp825; struct Cyc_List_List* _temp827; struct _tuple1* _temp829; struct Cyc_Absyn_Tunionfield
_temp819=*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd); _LL830: _temp829= _temp819.name; goto _LL828; _LL828: _temp827=
_temp819.tvs; goto _LL826; _LL826: _temp825= _temp819.typs; goto _LL824; _LL824:
_temp823= _temp819.loc; goto _LL822; _LL822: _temp821=( void*) _temp819.sc; goto
_LL820; _LL820: if(( _temp825 == 0? 1:(( struct Cyc_List_List*) _check_null(
_temp825))->tl != 0)? 1: _temp827 != 0){ continue;}{ void* t2= Cyc_Tcutil_rsubstitute(
r, inst,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp825))->hd)).f2);
if( Cyc_Tcutil_coerce_arg( te, e, t2)){ return( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( fields))->hd);}}}({
void(* _temp831)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp836= e->loc;
struct _tagged_arr _temp837= _tag_arr("can't find a field in %s to inject a value of type %s",
sizeof( unsigned char), 54u); struct Cyc_Stdio_String_pa_struct _temp839;
_temp839.tag= Cyc_Stdio_String_pa; _temp839.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
tu);{ void* _temp834=( void*)& _temp839; struct Cyc_Stdio_String_pa_struct
_temp838; _temp838.tag= Cyc_Stdio_String_pa; _temp838.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t1);{ void* _temp835=( void*)& _temp838; void* _temp832[
2u]={ _temp834, _temp835}; struct _tagged_arr _temp833={( void*) _temp832,( void*)
_temp832,( void*)( _temp832 + 2u)}; _temp831( _temp836, _temp837, _temp833);}}});
return 0;} static void* Cyc_Tcexp_tcFnCall( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
args, struct Cyc_Absyn_VarargCallInfo** vararg_call_info){ struct Cyc_List_List*
_temp840= args; te= Cyc_Tcenv_new_block( loc, te); Cyc_Tcexp_tcExp( te, 0, e);{
void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp841= t; struct Cyc_Absyn_PtrInfo _temp847; struct Cyc_Absyn_Conref*
_temp849; struct Cyc_Absyn_Tqual _temp851; struct Cyc_Absyn_Conref* _temp853;
void* _temp855; void* _temp857; _LL843: if(( unsigned int) _temp841 > 4u?*(( int*)
_temp841) == Cyc_Absyn_PointerType: 0){ _LL848: _temp847=(( struct Cyc_Absyn_PointerType_struct*)
_temp841)->f1; _LL858: _temp857=( void*) _temp847.elt_typ; goto _LL856; _LL856:
_temp855=( void*) _temp847.rgn_typ; goto _LL854; _LL854: _temp853= _temp847.nullable;
goto _LL852; _LL852: _temp851= _temp847.tq; goto _LL850; _LL850: _temp849=
_temp847.bounds; goto _LL844;} else{ goto _LL845;} _LL845: goto _LL846; _LL844:
Cyc_Tcenv_check_rgn_accessible( te, loc, _temp855); Cyc_Tcutil_check_nonzero_bound(
loc, _temp849);{ void* _temp859= Cyc_Tcutil_compress( _temp857); struct Cyc_Absyn_FnInfo
_temp865; struct Cyc_List_List* _temp867; struct Cyc_List_List* _temp869; struct
Cyc_Absyn_VarargInfo* _temp871; int _temp873; struct Cyc_List_List* _temp875;
void* _temp877; struct Cyc_Core_Opt* _temp879; struct Cyc_List_List* _temp881;
_LL861: if(( unsigned int) _temp859 > 4u?*(( int*) _temp859) == Cyc_Absyn_FnType:
0){ _LL866: _temp865=(( struct Cyc_Absyn_FnType_struct*) _temp859)->f1; _LL882:
_temp881= _temp865.tvars; goto _LL880; _LL880: _temp879= _temp865.effect; goto
_LL878; _LL878: _temp877=( void*) _temp865.ret_typ; goto _LL876; _LL876:
_temp875= _temp865.args; goto _LL874; _LL874: _temp873= _temp865.c_varargs; goto
_LL872; _LL872: _temp871= _temp865.cyc_varargs; goto _LL870; _LL870: _temp869=
_temp865.rgn_po; goto _LL868; _LL868: _temp867= _temp865.attributes; goto _LL862;}
else{ goto _LL863;} _LL863: goto _LL864; _LL862: if( topt != 0){ Cyc_Tcutil_unify(
_temp877,*(( void**) _check_null( topt)));} while( _temp840 != 0? _temp875 != 0:
0) { struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp840))->hd; void* t2=(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp875))->hd)).f3; Cyc_Tcexp_tcExp( te,( void**)& t2, e1); if( !
Cyc_Tcutil_coerce_arg( te, e1, t2)){({ void(* _temp883)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp888= loc; struct _tagged_arr _temp889= _tag_arr("actual argument has type %s but formal has type %s",
sizeof( unsigned char), 51u); struct Cyc_Stdio_String_pa_struct _temp891;
_temp891.tag= Cyc_Stdio_String_pa; _temp891.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ void* _temp886=(
void*)& _temp891; struct Cyc_Stdio_String_pa_struct _temp890; _temp890.tag= Cyc_Stdio_String_pa;
_temp890.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ void* _temp887=(
void*)& _temp890; void* _temp884[ 2u]={ _temp886, _temp887}; struct _tagged_arr
_temp885={( void*) _temp884,( void*) _temp884,( void*)( _temp884 + 2u)};
_temp883( _temp888, _temp889, _temp885);}}});} _temp840=(( struct Cyc_List_List*)
_check_null( _temp840))->tl; _temp875=(( struct Cyc_List_List*) _check_null(
_temp875))->tl;} if( _temp875 != 0){({ void(* _temp892)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp895= loc; struct _tagged_arr _temp896= _tag_arr("too few arguments for function",
sizeof( unsigned char), 31u); void* _temp893[ 0u]={}; struct _tagged_arr
_temp894={( void*) _temp893,( void*) _temp893,( void*)( _temp893 + 0u)};
_temp892( _temp895, _temp896, _temp894);});} else{ if(( _temp840 != 0? 1:
_temp873)? 1: _temp871 != 0){ if( _temp873){ for( 0; _temp840 != 0; _temp840=((
struct Cyc_List_List*) _check_null( _temp840))->tl){ Cyc_Tcexp_tcExp( te, 0,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp840))->hd);}}
else{ if( _temp871 == 0){({ void(* _temp897)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp900= loc; struct _tagged_arr _temp901= _tag_arr("too many arguments for function",
sizeof( unsigned char), 32u); void* _temp898[ 0u]={}; struct _tagged_arr
_temp899={( void*) _temp898,( void*) _temp898,( void*)( _temp898 + 0u)};
_temp897( _temp900, _temp901, _temp899);});} else{ int _temp904; void* _temp906;
void* _temp908; struct Cyc_Absyn_VarargInfo _temp902=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp871)); _LL909: _temp908=( void*) _temp902.type; goto _LL907;
_LL907: _temp906=( void*) _temp902.rgn; goto _LL905; _LL905: _temp904= _temp902.inject;
goto _LL903; _LL903: { struct Cyc_Absyn_VarargCallInfo* _temp910=({ struct Cyc_Absyn_VarargCallInfo*
_temp967=( struct Cyc_Absyn_VarargCallInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargCallInfo));
_temp967->num_varargs= 0; _temp967->injectors= 0; _temp967->vai=( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp871); _temp967;});* vararg_call_info= _temp910; if( ! Cyc_Tcutil_unify(
Cyc_Tcenv_curr_rgn( te), _temp906)){({ void(* _temp911)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp916= loc; struct _tagged_arr _temp917= _tag_arr("I have no idea how this could fail, but it did because the region for the varargs (%s) doesn't unify with the expected new_block region (%s).",
sizeof( unsigned char), 142u); struct Cyc_Stdio_String_pa_struct _temp919;
_temp919.tag= Cyc_Stdio_String_pa; _temp919.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp906);{ void* _temp914=( void*)& _temp919; struct Cyc_Stdio_String_pa_struct
_temp918; _temp918.tag= Cyc_Stdio_String_pa; _temp918.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( Cyc_Tcenv_curr_rgn( te));{ void* _temp915=( void*)&
_temp918; void* _temp912[ 2u]={ _temp914, _temp915}; struct _tagged_arr _temp913={(
void*) _temp912,( void*) _temp912,( void*)( _temp912 + 2u)}; _temp911( _temp916,
_temp917, _temp913);}}});} if( ! _temp904){ for( 0; _temp840 != 0; _temp840=((
struct Cyc_List_List*) _check_null( _temp840))->tl){ struct Cyc_Absyn_Exp* e1=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp840))->hd;((
struct Cyc_Absyn_VarargCallInfo*) _check_null( _temp910))->num_varargs ++; Cyc_Tcexp_tcExp(
te,( void**)& _temp908, e1); if( ! Cyc_Tcutil_coerce_arg( te, e1, _temp908)){({
void(* _temp920)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp925= loc;
struct _tagged_arr _temp926= _tag_arr("vararg requires type %s but argument has type %s",
sizeof( unsigned char), 49u); struct Cyc_Stdio_String_pa_struct _temp928;
_temp928.tag= Cyc_Stdio_String_pa; _temp928.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp908);{ void* _temp923=( void*)& _temp928; struct Cyc_Stdio_String_pa_struct
_temp927; _temp927.tag= Cyc_Stdio_String_pa; _temp927.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{
void* _temp924=( void*)& _temp927; void* _temp921[ 2u]={ _temp923, _temp924};
struct _tagged_arr _temp922={( void*) _temp921,( void*) _temp921,( void*)(
_temp921 + 2u)}; _temp920( _temp925, _temp926, _temp922);}}});}}} else{ void*
_temp929= Cyc_Tcutil_compress( _temp908); struct Cyc_Absyn_TunionInfo _temp935;
void* _temp937; struct Cyc_List_List* _temp939; void* _temp941; struct Cyc_Absyn_Tuniondecl*
_temp943; _LL931: if(( unsigned int) _temp929 > 4u?*(( int*) _temp929) == Cyc_Absyn_TunionType:
0){ _LL936: _temp935=(( struct Cyc_Absyn_TunionType_struct*) _temp929)->f1;
_LL942: _temp941=( void*) _temp935.tunion_info; if(*(( int*) _temp941) == Cyc_Absyn_KnownTunion){
_LL944: _temp943=(( struct Cyc_Absyn_KnownTunion_struct*) _temp941)->f1; goto
_LL940;} else{ goto _LL933;} _LL940: _temp939= _temp935.targs; goto _LL938;
_LL938: _temp937=( void*) _temp935.rgn; goto _LL932;} else{ goto _LL933;} _LL933:
goto _LL934; _LL932: { struct Cyc_Absyn_Tuniondecl* _temp945=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp943->name); if( ! Cyc_Tcutil_unify( _temp937, _temp906)){({ void(*
_temp946)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp949= loc;
struct _tagged_arr _temp950= _tag_arr("bad region for injected varargs", sizeof(
unsigned char), 32u); void* _temp947[ 0u]={}; struct _tagged_arr _temp948={(
void*) _temp947,( void*) _temp947,( void*)( _temp947 + 0u)}; _temp946( _temp949,
_temp950, _temp948);});}{ struct Cyc_List_List* fields= 0; if( _temp945->fields
== 0){({ void(* _temp951)( struct Cyc_Position_Segment*, struct _tagged_arr fmt,
struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp955=
loc; struct _tagged_arr _temp956= _tag_arr("can't inject into %s", sizeof(
unsigned char), 21u); struct Cyc_Stdio_String_pa_struct _temp957; _temp957.tag=
Cyc_Stdio_String_pa; _temp957.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp908);{ void* _temp954=( void*)& _temp957; void* _temp952[ 1u]={ _temp954};
struct _tagged_arr _temp953={( void*) _temp952,( void*) _temp952,( void*)(
_temp952 + 1u)}; _temp951( _temp955, _temp956, _temp953);}});} else{ fields=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp945->fields))->v;}{
struct _RegionHandle _temp958= _new_region(); struct _RegionHandle* rgn=&
_temp958; _push_region( rgn);{ struct Cyc_List_List* _temp959=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp945->tvs, _temp939);
for( 0; _temp840 != 0; _temp840=(( struct Cyc_List_List*) _check_null( _temp840))->tl){((
struct Cyc_Absyn_VarargCallInfo*) _check_null( _temp910))->num_varargs ++;{
struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp840))->hd; Cyc_Tcexp_tcExp( te, 0, e1);{ struct Cyc_Absyn_Tunionfield*
_temp960= Cyc_Tcexp_tcInjection( te, e1, _temp908, rgn, _temp959, fields); if(
_temp960 != 0){(( struct Cyc_Absyn_VarargCallInfo*) _check_null( _temp910))->injectors=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(((
struct Cyc_Absyn_VarargCallInfo*) _check_null( _temp910))->injectors,({ struct
Cyc_List_List* _temp961=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp961->hd=( void*)(( struct Cyc_Absyn_Tunionfield*) _check_null( _temp960));
_temp961->tl= 0; _temp961;}));}}}}}; _pop_region( rgn);} goto _LL930;}} _LL934:({
void(* _temp962)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp965= loc;
struct _tagged_arr _temp966= _tag_arr("bad inject vararg type", sizeof(
unsigned char), 23u); void* _temp963[ 0u]={}; struct _tagged_arr _temp964={(
void*) _temp963,( void*) _temp963,( void*)( _temp963 + 0u)}; _temp962( _temp965,
_temp966, _temp964);}); goto _LL930; _LL930:;}}}}}}{ struct Cyc_List_List* a=
_temp867; for( 0; a != 0; a=(( struct Cyc_List_List*) _check_null( a))->tl){
void* _temp968=( void*)(( struct Cyc_List_List*) _check_null( a))->hd; int
_temp974; int _temp976; void* _temp978; _LL970: if(( unsigned int) _temp968 > 16u?*((
int*) _temp968) == Cyc_Absyn_Format_att: 0){ _LL979: _temp978=( void*)(( struct
Cyc_Absyn_Format_att_struct*) _temp968)->f1; goto _LL977; _LL977: _temp976=((
struct Cyc_Absyn_Format_att_struct*) _temp968)->f2; goto _LL975; _LL975:
_temp974=(( struct Cyc_Absyn_Format_att_struct*) _temp968)->f3; goto _LL971;}
else{ goto _LL972;} _LL972: goto _LL973; _LL971:{ struct _handler_cons _temp980;
_push_handler(& _temp980);{ int _temp982= 0; if( setjmp( _temp980.handler)){
_temp982= 1;} if( ! _temp982){{ struct Cyc_Absyn_Exp* _temp983=(( struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List* x, int i)) Cyc_List_nth)( args, _temp976 - 1); struct Cyc_Core_Opt*
fmt_args; if( _temp974 == 0){ fmt_args= 0;} else{ fmt_args=({ struct Cyc_Core_Opt*
_temp984=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp984->v=( void*)(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, int i))
Cyc_List_nth_tail)( args, _temp974 - 1); _temp984;});}{ void* _temp985= _temp978;
_LL987: if( _temp985 ==( void*) Cyc_Absyn_Printf_ft){ goto _LL988;} else{ goto
_LL989;} _LL989: if( _temp985 ==( void*) Cyc_Absyn_Scanf_ft){ goto _LL990;}
else{ goto _LL986;} _LL988: Cyc_Tcexp_check_format_args( te, _temp983, fmt_args,
Cyc_Formatstr_get_format_typs); goto _LL986; _LL990: Cyc_Tcexp_check_format_args(
te, _temp983, fmt_args, Cyc_Formatstr_get_scanf_typs); goto _LL986; _LL986:;}};
_pop_handler();} else{ void* _temp981=( void*) _exn_thrown; void* _temp992=
_temp981; _LL994: if( _temp992 == Cyc_List_Nth){ goto _LL995;} else{ goto _LL996;}
_LL996: goto _LL997; _LL995:({ void(* _temp998)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp1001= loc; struct _tagged_arr _temp1002= _tag_arr("bad format arguments",
sizeof( unsigned char), 21u); void* _temp999[ 0u]={}; struct _tagged_arr
_temp1000={( void*) _temp999,( void*) _temp999,( void*)( _temp999 + 0u)};
_temp998( _temp1001, _temp1002, _temp1000);}); goto _LL993; _LL997:( void)
_throw( _temp992); _LL993:;}}} goto _LL969; _LL973: goto _LL969; _LL969:;}} Cyc_Tcenv_check_effect_accessible(
te, loc,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp879))->v); Cyc_Tcenv_check_rgn_partial_order(
te, loc, _temp869); return _temp877; _LL864: return({ void*(* _temp1003)( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tagged_arr msg,
struct _tagged_arr ap)= Cyc_Tcexp_expr_err; struct Cyc_Tcenv_Tenv* _temp1006= te;
struct Cyc_Position_Segment* _temp1007= loc; struct _tagged_arr _temp1008=
_tag_arr("expected pointer to function", sizeof( unsigned char), 29u); void*
_temp1004[ 0u]={}; struct _tagged_arr _temp1005={( void*) _temp1004,( void*)
_temp1004,( void*)( _temp1004 + 0u)}; _temp1003( _temp1006, _temp1007, _temp1008,
_temp1005);}); _LL860:;} _LL846: return({ void*(* _temp1009)( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tagged_arr msg, struct _tagged_arr
ap)= Cyc_Tcexp_expr_err; struct Cyc_Tcenv_Tenv* _temp1012= te; struct Cyc_Position_Segment*
_temp1013= loc; struct _tagged_arr _temp1014= _tag_arr("expected pointer to function",
sizeof( unsigned char), 29u); void* _temp1010[ 0u]={}; struct _tagged_arr
_temp1011={( void*) _temp1010,( void*) _temp1010,( void*)( _temp1010 + 0u)};
_temp1009( _temp1012, _temp1013, _temp1014, _temp1011);}); _LL842:;}} static
void* Cyc_Tcexp_tcThrow( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_exn_typ,
e); if( ! Cyc_Tcutil_coerce_use( te, e, Cyc_Absyn_exn_typ)){({ void(* _temp1015)(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp1019= loc; struct _tagged_arr
_temp1020= _tag_arr("expected xtunion exn but found %s", sizeof( unsigned char),
34u); struct Cyc_Stdio_String_pa_struct _temp1021; _temp1021.tag= Cyc_Stdio_String_pa;
_temp1021.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v);{ void* _temp1018=( void*)& _temp1021; void*
_temp1016[ 1u]={ _temp1018}; struct _tagged_arr _temp1017={( void*) _temp1016,(
void*) _temp1016,( void*)( _temp1016 + 1u)}; _temp1015( _temp1019, _temp1020,
_temp1017);}});} return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp1022=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1022->v=(
void*) Cyc_Tcenv_lookup_type_vars( te); _temp1022;}));} static void* Cyc_Tcexp_tcInstantiate(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, struct Cyc_List_List* ts){ Cyc_Tcexp_tcExpNoInst( te, 0, e);(
void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand(
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)));{
void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{
void* _temp1023= t1; struct Cyc_Absyn_PtrInfo _temp1029; struct Cyc_Absyn_Conref*
_temp1031; struct Cyc_Absyn_Tqual _temp1033; struct Cyc_Absyn_Conref* _temp1035;
void* _temp1037; void* _temp1039; _LL1025: if(( unsigned int) _temp1023 > 4u?*((
int*) _temp1023) == Cyc_Absyn_PointerType: 0){ _LL1030: _temp1029=(( struct Cyc_Absyn_PointerType_struct*)
_temp1023)->f1; _LL1040: _temp1039=( void*) _temp1029.elt_typ; goto _LL1038;
_LL1038: _temp1037=( void*) _temp1029.rgn_typ; goto _LL1036; _LL1036: _temp1035=
_temp1029.nullable; goto _LL1034; _LL1034: _temp1033= _temp1029.tq; goto _LL1032;
_LL1032: _temp1031= _temp1029.bounds; goto _LL1026;} else{ goto _LL1027;}
_LL1027: goto _LL1028; _LL1026:{ void* _temp1041= Cyc_Tcutil_compress( _temp1039);
struct Cyc_Absyn_FnInfo _temp1047; struct Cyc_List_List* _temp1049; struct Cyc_List_List*
_temp1051; struct Cyc_Absyn_VarargInfo* _temp1053; int _temp1055; struct Cyc_List_List*
_temp1057; void* _temp1059; struct Cyc_Core_Opt* _temp1061; struct Cyc_List_List*
_temp1063; _LL1043: if(( unsigned int) _temp1041 > 4u?*(( int*) _temp1041) ==
Cyc_Absyn_FnType: 0){ _LL1048: _temp1047=(( struct Cyc_Absyn_FnType_struct*)
_temp1041)->f1; _LL1064: _temp1063= _temp1047.tvars; goto _LL1062; _LL1062:
_temp1061= _temp1047.effect; goto _LL1060; _LL1060: _temp1059=( void*) _temp1047.ret_typ;
goto _LL1058; _LL1058: _temp1057= _temp1047.args; goto _LL1056; _LL1056:
_temp1055= _temp1047.c_varargs; goto _LL1054; _LL1054: _temp1053= _temp1047.cyc_varargs;
goto _LL1052; _LL1052: _temp1051= _temp1047.rgn_po; goto _LL1050; _LL1050:
_temp1049= _temp1047.attributes; goto _LL1044;} else{ goto _LL1045;} _LL1045:
goto _LL1046; _LL1044: { struct Cyc_List_List* instantiation= 0; if( Cyc_List_length(
ts) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1063)){ return({
void*(* _temp1065)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
struct _tagged_arr msg, struct _tagged_arr ap)= Cyc_Tcexp_expr_err; struct Cyc_Tcenv_Tenv*
_temp1068= te; struct Cyc_Position_Segment* _temp1069= loc; struct _tagged_arr
_temp1070= _tag_arr("type instantiation mismatch", sizeof( unsigned char), 28u);
void* _temp1066[ 0u]={}; struct _tagged_arr _temp1067={( void*) _temp1066,( void*)
_temp1066,( void*)( _temp1066 + 0u)}; _temp1065( _temp1068, _temp1069, _temp1070,
_temp1067);});} for( 0; ts != 0;( ts=(( struct Cyc_List_List*) _check_null( ts))->tl,
_temp1063=(( struct Cyc_List_List*) _check_null( _temp1063))->tl)){ void* k= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp1063))->hd)->kind);
Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te), k,( void*)((
struct Cyc_List_List*) _check_null( ts))->hd); instantiation=({ struct Cyc_List_List*
_temp1071=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1071->hd=( void*)({ struct _tuple5* _temp1072=( struct _tuple5*) GC_malloc(
sizeof( struct _tuple5)); _temp1072->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp1063))->hd; _temp1072->f2=( void*)(( struct Cyc_List_List*)
_check_null( ts))->hd; _temp1072;}); _temp1071->tl= instantiation; _temp1071;});}{
void* new_fn_typ= Cyc_Tcutil_substitute( instantiation,( void*)({ struct Cyc_Absyn_FnType_struct*
_temp1076=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1076[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1077; _temp1077.tag= Cyc_Absyn_FnType;
_temp1077.f1=({ struct Cyc_Absyn_FnInfo _temp1078; _temp1078.tvars= 0; _temp1078.effect=
_temp1061; _temp1078.ret_typ=( void*) _temp1059; _temp1078.args= _temp1057;
_temp1078.c_varargs= _temp1055; _temp1078.cyc_varargs= _temp1053; _temp1078.rgn_po=
_temp1051; _temp1078.attributes= _temp1049; _temp1078;}); _temp1077;});
_temp1076;})); void* new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1073=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1073[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1074; _temp1074.tag=
Cyc_Absyn_PointerType; _temp1074.f1=({ struct Cyc_Absyn_PtrInfo _temp1075;
_temp1075.elt_typ=( void*) new_fn_typ; _temp1075.rgn_typ=( void*) _temp1037;
_temp1075.nullable= _temp1035; _temp1075.tq= _temp1033; _temp1075.bounds=
_temp1031; _temp1075;}); _temp1074;}); _temp1073;}); return new_typ;}} _LL1046:
goto _LL1042; _LL1042:;} goto _LL1024; _LL1028: goto _LL1024; _LL1024:;} return({
void*(* _temp1079)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
struct _tagged_arr msg, struct _tagged_arr ap)= Cyc_Tcexp_expr_err; struct Cyc_Tcenv_Tenv*
_temp1083= te; struct Cyc_Position_Segment* _temp1084= loc; struct _tagged_arr
_temp1085= _tag_arr("expecting polymorphic type but found %s", sizeof(
unsigned char), 40u); struct Cyc_Stdio_String_pa_struct _temp1086; _temp1086.tag=
Cyc_Stdio_String_pa; _temp1086.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp1082=( void*)& _temp1086; void* _temp1080[ 1u]={ _temp1082};
struct _tagged_arr _temp1081={( void*) _temp1080,( void*) _temp1080,( void*)(
_temp1080 + 1u)}; _temp1079( _temp1083, _temp1084, _temp1085, _temp1081);}});}}
static void* Cyc_Tcexp_tcCast( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* t, struct Cyc_Absyn_Exp* e){ Cyc_Tcutil_check_type( loc,
te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, t); Cyc_Tcexp_tcExp(
te, 0, e);{ void* t2=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v;
if( ! Cyc_Tcutil_silent_castable( te, loc, t2, t)? ! Cyc_Tcutil_castable( te,
loc, t2, t): 0){ return({ void*(* _temp1087)( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, struct _tagged_arr msg, struct _tagged_arr ap)= Cyc_Tcexp_expr_err;
struct Cyc_Tcenv_Tenv* _temp1092= te; struct Cyc_Position_Segment* _temp1093=
loc; struct _tagged_arr _temp1094= _tag_arr("cannot cast %s to %s", sizeof(
unsigned char), 21u); struct Cyc_Stdio_String_pa_struct _temp1096; _temp1096.tag=
Cyc_Stdio_String_pa; _temp1096.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t2);{ void* _temp1090=( void*)& _temp1096; struct Cyc_Stdio_String_pa_struct
_temp1095; _temp1095.tag= Cyc_Stdio_String_pa; _temp1095.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp1091=( void*)& _temp1095; void*
_temp1088[ 2u]={ _temp1090, _temp1091}; struct _tagged_arr _temp1089={( void*)
_temp1088,( void*) _temp1088,( void*)( _temp1088 + 2u)}; _temp1087( _temp1092,
_temp1093, _temp1094, _temp1089);}}});} return t;}} static void* Cyc_Tcexp_tcAddress(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ void** _temp1097= 0; struct Cyc_Absyn_Tqual _temp1098= Cyc_Absyn_empty_tqual();
if( topt != 0){ void* _temp1099= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_PtrInfo _temp1105; struct Cyc_Absyn_Tqual _temp1107;
void* _temp1109; _LL1101: if(( unsigned int) _temp1099 > 4u?*(( int*) _temp1099)
== Cyc_Absyn_PointerType: 0){ _LL1106: _temp1105=(( struct Cyc_Absyn_PointerType_struct*)
_temp1099)->f1; _LL1110: _temp1109=( void*) _temp1105.elt_typ; goto _LL1108;
_LL1108: _temp1107= _temp1105.tq; goto _LL1102;} else{ goto _LL1103;} _LL1103:
goto _LL1104; _LL1102: _temp1097=({ void** _temp1111=( void**) GC_malloc(
sizeof( void*)); _temp1111[ 0]= _temp1109; _temp1111;}); _temp1098= _temp1107;
goto _LL1100; _LL1104: goto _LL1100; _LL1100:;} Cyc_Tcexp_tcExpNoInst( te,
_temp1097, e);{ void* _temp1112=( void*) e->r; _LL1114: if(*(( int*) _temp1112)
== Cyc_Absyn_Struct_e){ goto _LL1115;} else{ goto _LL1116;} _LL1116: if(*(( int*)
_temp1112) == Cyc_Absyn_Tuple_e){ goto _LL1117;} else{ goto _LL1118;} _LL1118:
goto _LL1119; _LL1115: goto _LL1117; _LL1117:({ void(* _temp1120)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_warn; struct Cyc_Position_Segment*
_temp1123= loc; struct _tagged_arr _temp1124= _tag_arr("& used to allocate",
sizeof( unsigned char), 19u); void* _temp1121[ 0u]={}; struct _tagged_arr
_temp1122={( void*) _temp1121,( void*) _temp1121,( void*)( _temp1121 + 0u)};
_temp1120( _temp1123, _temp1124, _temp1122);});{ void*(* _temp1125)( void* t,
void* rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; if( topt != 0){ void*
_temp1126= Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo
_temp1132; struct Cyc_Absyn_Conref* _temp1134; _LL1128: if(( unsigned int)
_temp1126 > 4u?*(( int*) _temp1126) == Cyc_Absyn_PointerType: 0){ _LL1133:
_temp1132=(( struct Cyc_Absyn_PointerType_struct*) _temp1126)->f1; _LL1135:
_temp1134= _temp1132.nullable; goto _LL1129;} else{ goto _LL1130;} _LL1130: goto
_LL1131; _LL1129: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1134)){ _temp1125= Cyc_Absyn_star_typ;} goto _LL1127; _LL1131: goto _LL1127;
_LL1127:;} return _temp1125(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v,(
void*) Cyc_Absyn_HeapRgn, _temp1098);} _LL1119: goto _LL1113; _LL1113:;}{ void*
_temp1138; int _temp1140; struct _tuple6 _temp1136= Cyc_Tcutil_addressof_props(
te, e); _LL1141: _temp1140= _temp1136.f1; goto _LL1139; _LL1139: _temp1138=
_temp1136.f2; goto _LL1137; _LL1137: { struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual();
if( _temp1140){ tq.q_const= 1;}{ void* t= Cyc_Absyn_at_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, _temp1138, tq); return t;}}}} static void* Cyc_Tcexp_tcSizeof(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind,
t); if( ! Cyc_Evexp_okay_szofarg( t)){({ void(* _temp1142)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp1146= loc; struct _tagged_arr _temp1147= _tag_arr("sizeof applied to type %s, which has unknown size here",
sizeof( unsigned char), 55u); struct Cyc_Stdio_String_pa_struct _temp1148;
_temp1148.tag= Cyc_Stdio_String_pa; _temp1148.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp1145=( void*)& _temp1148; void* _temp1143[ 1u]={ _temp1145};
struct _tagged_arr _temp1144={( void*) _temp1143,( void*) _temp1143,( void*)(
_temp1143 + 1u)}; _temp1142( _temp1146, _temp1147, _temp1144);}});} return Cyc_Absyn_uint_t;}
static void* Cyc_Tcexp_tcOffsetof( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* t, struct _tagged_arr* n){ Cyc_Tcutil_check_type( loc,
te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, t);{ int
bad_type= 1;{ void* _temp1149= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp1157; _LL1151: if(( unsigned int) _temp1149 > 4u?*(( int*) _temp1149) ==
Cyc_Absyn_StructType: 0){ _LL1158: _temp1157=(( struct Cyc_Absyn_StructType_struct*)
_temp1149)->f3; goto _LL1152;} else{ goto _LL1153;} _LL1153: if(( unsigned int)
_temp1149 > 4u?*(( int*) _temp1149) == Cyc_Absyn_AnonStructType: 0){ goto
_LL1154;} else{ goto _LL1155;} _LL1155: goto _LL1156; _LL1152: if( _temp1157 ==
0){ return({ void*(* _temp1159)( struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_impos; struct _tagged_arr _temp1162= _tag_arr("offsetof on unchecked StructType",
sizeof( unsigned char), 33u); void* _temp1160[ 0u]={}; struct _tagged_arr
_temp1161={( void*) _temp1160,( void*) _temp1160,( void*)( _temp1160 + 0u)};
_temp1159( _temp1162, _temp1161);});} if((*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp1157)))->fields == 0){ goto _LL1150;} goto _LL1154; _LL1154:
bad_type= 0; goto _LL1150; _LL1156: goto _LL1150; _LL1150:;} if( bad_type){({
void(* _temp1163)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp1167= loc;
struct _tagged_arr _temp1168= _tag_arr("%s is not a known struct type", sizeof(
unsigned char), 30u); struct Cyc_Stdio_String_pa_struct _temp1169; _temp1169.tag=
Cyc_Stdio_String_pa; _temp1169.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp1166=( void*)& _temp1169; void* _temp1164[ 1u]={ _temp1166};
struct _tagged_arr _temp1165={( void*) _temp1164,( void*) _temp1164,( void*)(
_temp1164 + 1u)}; _temp1163( _temp1167, _temp1168, _temp1165);}});} return Cyc_Absyn_uint_t;}}
static void* Cyc_Tcexp_tcDeref( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp1170= t; struct Cyc_Absyn_PtrInfo _temp1176; struct Cyc_Absyn_Conref*
_temp1178; void* _temp1180; void* _temp1182; _LL1172: if(( unsigned int)
_temp1170 > 4u?*(( int*) _temp1170) == Cyc_Absyn_PointerType: 0){ _LL1177:
_temp1176=(( struct Cyc_Absyn_PointerType_struct*) _temp1170)->f1; _LL1183:
_temp1182=( void*) _temp1176.elt_typ; goto _LL1181; _LL1181: _temp1180=( void*)
_temp1176.rgn_typ; goto _LL1179; _LL1179: _temp1178= _temp1176.bounds; goto
_LL1173;} else{ goto _LL1174;} _LL1174: goto _LL1175; _LL1173: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1180); Cyc_Tcutil_check_nonzero_bound( loc, _temp1178); return
_temp1182; _LL1175: return({ void*(* _temp1184)( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct _tagged_arr msg, struct _tagged_arr ap)=
Cyc_Tcexp_expr_err; struct Cyc_Tcenv_Tenv* _temp1188= te; struct Cyc_Position_Segment*
_temp1189= loc; struct _tagged_arr _temp1190= _tag_arr("expecting * or @ type but found %s",
sizeof( unsigned char), 35u); struct Cyc_Stdio_String_pa_struct _temp1191;
_temp1191.tag= Cyc_Stdio_String_pa; _temp1191.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp1187=( void*)& _temp1191; void* _temp1185[ 1u]={ _temp1187};
struct _tagged_arr _temp1186={( void*) _temp1185,( void*) _temp1185,( void*)(
_temp1185 + 1u)}; _temp1184( _temp1188, _temp1189, _temp1190, _temp1186);}});
_LL1171:;}} static void* Cyc_Tcexp_tcStructMember( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* outer_e,
struct Cyc_Absyn_Exp* e, struct _tagged_arr* f){ Cyc_Tcexp_tcExpNoPromote( te, 0,
e);{ void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v); void* _temp1192= t; struct Cyc_Absyn_Structdecl** _temp1208;
struct Cyc_Absyn_Structdecl* _temp1210; struct Cyc_List_List* _temp1211; struct
_tuple1* _temp1213; struct Cyc_List_List* _temp1215; struct Cyc_List_List*
_temp1217; struct Cyc_Absyn_Uniondecl** _temp1219; struct Cyc_Absyn_Uniondecl*
_temp1221; struct Cyc_List_List* _temp1222; struct _tuple1* _temp1224; _LL1194:
if(( unsigned int) _temp1192 > 4u?*(( int*) _temp1192) == Cyc_Absyn_StructType:
0){ _LL1214: _temp1213=(( struct Cyc_Absyn_StructType_struct*) _temp1192)->f1;
goto _LL1212; _LL1212: _temp1211=(( struct Cyc_Absyn_StructType_struct*)
_temp1192)->f2; goto _LL1209; _LL1209: _temp1208=(( struct Cyc_Absyn_StructType_struct*)
_temp1192)->f3; if( _temp1208 == 0){ goto _LL1196;} else{ _temp1210=* _temp1208;
goto _LL1195;}} else{ goto _LL1196;} _LL1196: if(( unsigned int) _temp1192 > 4u?*((
int*) _temp1192) == Cyc_Absyn_AnonStructType: 0){ _LL1216: _temp1215=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1192)->f1; goto _LL1197;} else{ goto
_LL1198;} _LL1198: if(( unsigned int) _temp1192 > 4u?*(( int*) _temp1192) == Cyc_Absyn_AnonUnionType:
0){ _LL1218: _temp1217=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1192)->f1;
goto _LL1199;} else{ goto _LL1200;} _LL1200: if(( unsigned int) _temp1192 > 4u?*((
int*) _temp1192) == Cyc_Absyn_UnionType: 0){ _LL1225: _temp1224=(( struct Cyc_Absyn_UnionType_struct*)
_temp1192)->f1; goto _LL1223; _LL1223: _temp1222=(( struct Cyc_Absyn_UnionType_struct*)
_temp1192)->f2; goto _LL1220; _LL1220: _temp1219=(( struct Cyc_Absyn_UnionType_struct*)
_temp1192)->f3; if( _temp1219 == 0){ goto _LL1202;} else{ _temp1221=* _temp1219;
goto _LL1201;}} else{ goto _LL1202;} _LL1202: if(( unsigned int) _temp1192 > 4u?*((
int*) _temp1192) == Cyc_Absyn_ArrayType: 0){ goto _LL1226;} else{ goto _LL1204;}
_LL1226: if( Cyc_String_zstrcmp(* f, _tag_arr("size", sizeof( unsigned char), 5u))
== 0){ goto _LL1203;} else{ goto _LL1204;} _LL1204: if(( unsigned int) _temp1192
> 4u?*(( int*) _temp1192) == Cyc_Absyn_PointerType: 0){ goto _LL1227;} else{
goto _LL1206;} _LL1227: if( Cyc_String_zstrcmp(* f, _tag_arr("size", sizeof(
unsigned char), 5u)) == 0){ goto _LL1205;} else{ goto _LL1206;} _LL1206: goto
_LL1207; _LL1195: if( _temp1213 == 0){ return({ void*(* _temp1228)( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tagged_arr msg, struct _tagged_arr
ap)= Cyc_Tcexp_expr_err; struct Cyc_Tcenv_Tenv* _temp1231= te; struct Cyc_Position_Segment*
_temp1232= loc; struct _tagged_arr _temp1233= _tag_arr("unresolved struct type (COMPILER ERROR)",
sizeof( unsigned char), 40u); void* _temp1229[ 0u]={}; struct _tagged_arr
_temp1230={( void*) _temp1229,( void*) _temp1229,( void*)( _temp1229 + 0u)};
_temp1228( _temp1231, _temp1232, _temp1233, _temp1230);});}{ struct Cyc_Absyn_Structfield*
_temp1234= Cyc_Absyn_lookup_struct_field( _temp1210, f); if( _temp1234 == 0){
return({ void*(* _temp1235)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tagged_arr msg, struct _tagged_arr ap)= Cyc_Tcexp_expr_err; struct
Cyc_Tcenv_Tenv* _temp1240= te; struct Cyc_Position_Segment* _temp1241= loc;
struct _tagged_arr _temp1242= _tag_arr("struct %s has no %s field", sizeof(
unsigned char), 26u); struct Cyc_Stdio_String_pa_struct _temp1244; _temp1244.tag=
Cyc_Stdio_String_pa; _temp1244.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp1213));{ void* _temp1238=( void*)& _temp1244;
struct Cyc_Stdio_String_pa_struct _temp1243; _temp1243.tag= Cyc_Stdio_String_pa;
_temp1243.f1=( struct _tagged_arr)* f;{ void* _temp1239=( void*)& _temp1243;
void* _temp1236[ 2u]={ _temp1238, _temp1239}; struct _tagged_arr _temp1237={(
void*) _temp1236,( void*) _temp1236,( void*)( _temp1236 + 2u)}; _temp1235(
_temp1240, _temp1241, _temp1242, _temp1237);}}});}{ void* t2;{ struct
_RegionHandle _temp1245= _new_region(); struct _RegionHandle* rgn=& _temp1245;
_push_region( rgn);{ struct Cyc_List_List* _temp1246=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp1210->tvs, _temp1211);
t2= Cyc_Tcutil_rsubstitute( rgn, _temp1246,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1234))->type);}; _pop_region( rgn);} return t2;}} _LL1197: {
struct Cyc_Absyn_Structfield* _temp1247= Cyc_Absyn_lookup_field( _temp1215, f);
if( _temp1247 == 0){ return({ void*(* _temp1248)( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct _tagged_arr msg, struct _tagged_arr ap)=
Cyc_Tcexp_expr_err; struct Cyc_Tcenv_Tenv* _temp1252= te; struct Cyc_Position_Segment*
_temp1253= loc; struct _tagged_arr _temp1254= _tag_arr("struct has no %s field",
sizeof( unsigned char), 23u); struct Cyc_Stdio_String_pa_struct _temp1255;
_temp1255.tag= Cyc_Stdio_String_pa; _temp1255.f1=( struct _tagged_arr)* f;{ void*
_temp1251=( void*)& _temp1255; void* _temp1249[ 1u]={ _temp1251}; struct
_tagged_arr _temp1250={( void*) _temp1249,( void*) _temp1249,( void*)( _temp1249
+ 1u)}; _temp1248( _temp1252, _temp1253, _temp1254, _temp1250);}});} return(
void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp1247))->type;} _LL1199: {
struct Cyc_Absyn_Structfield* _temp1256= Cyc_Absyn_lookup_field( _temp1217, f);
if( _temp1256 == 0){ return({ void*(* _temp1257)( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct _tagged_arr msg, struct _tagged_arr ap)=
Cyc_Tcexp_expr_err; struct Cyc_Tcenv_Tenv* _temp1261= te; struct Cyc_Position_Segment*
_temp1262= loc; struct _tagged_arr _temp1263= _tag_arr("union has no %s field",
sizeof( unsigned char), 22u); struct Cyc_Stdio_String_pa_struct _temp1264;
_temp1264.tag= Cyc_Stdio_String_pa; _temp1264.f1=( struct _tagged_arr)* f;{ void*
_temp1260=( void*)& _temp1264; void* _temp1258[ 1u]={ _temp1260}; struct
_tagged_arr _temp1259={( void*) _temp1258,( void*) _temp1258,( void*)( _temp1258
+ 1u)}; _temp1257( _temp1261, _temp1262, _temp1263, _temp1259);}});} return(
void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp1256))->type;} _LL1201:
if( _temp1224 == 0){ return({ void*(* _temp1265)( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct _tagged_arr msg, struct _tagged_arr ap)=
Cyc_Tcexp_expr_err; struct Cyc_Tcenv_Tenv* _temp1268= te; struct Cyc_Position_Segment*
_temp1269= loc; struct _tagged_arr _temp1270= _tag_arr("unresolved union type (COMPILER ERROR)",
sizeof( unsigned char), 39u); void* _temp1266[ 0u]={}; struct _tagged_arr
_temp1267={( void*) _temp1266,( void*) _temp1266,( void*)( _temp1266 + 0u)};
_temp1265( _temp1268, _temp1269, _temp1270, _temp1267);});}{ struct Cyc_Absyn_Structfield*
_temp1271= Cyc_Absyn_lookup_union_field( _temp1221, f); if( _temp1271 == 0){
return({ void*(* _temp1272)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tagged_arr msg, struct _tagged_arr ap)= Cyc_Tcexp_expr_err; struct
Cyc_Tcenv_Tenv* _temp1277= te; struct Cyc_Position_Segment* _temp1278= loc;
struct _tagged_arr _temp1279= _tag_arr("union %s has no %s field", sizeof(
unsigned char), 25u); struct Cyc_Stdio_String_pa_struct _temp1281; _temp1281.tag=
Cyc_Stdio_String_pa; _temp1281.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp1224));{ void* _temp1275=( void*)& _temp1281;
struct Cyc_Stdio_String_pa_struct _temp1280; _temp1280.tag= Cyc_Stdio_String_pa;
_temp1280.f1=( struct _tagged_arr)* f;{ void* _temp1276=( void*)& _temp1280;
void* _temp1273[ 2u]={ _temp1275, _temp1276}; struct _tagged_arr _temp1274={(
void*) _temp1273,( void*) _temp1273,( void*)( _temp1273 + 2u)}; _temp1272(
_temp1277, _temp1278, _temp1279, _temp1274);}}});}{ struct Cyc_List_List*
_temp1282=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp1221->tvs, _temp1222); void* _temp1283= Cyc_Tcutil_substitute(
_temp1282,( void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp1271))->type);
return _temp1283;}} _LL1203:( void*)( outer_e->r=( void*)(( void*)({ struct Cyc_Absyn_Primop_e_struct*
_temp1284=( struct Cyc_Absyn_Primop_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct));
_temp1284[ 0]=({ struct Cyc_Absyn_Primop_e_struct _temp1285; _temp1285.tag= Cyc_Absyn_Primop_e;
_temp1285.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1285.f2=({ struct Cyc_List_List*
_temp1286=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1286->hd=( void*) e; _temp1286->tl= 0; _temp1286;}); _temp1285;});
_temp1284;}))); return Cyc_Absyn_uint_t; _LL1205:( void*)( outer_e->r=( void*)((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1287=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1287[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1288; _temp1288.tag= Cyc_Absyn_Primop_e;
_temp1288.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1288.f2=({ struct Cyc_List_List*
_temp1289=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1289->hd=( void*) e; _temp1289->tl= 0; _temp1289;}); _temp1288;});
_temp1287;}))); return Cyc_Absyn_uint_t; _LL1207: if( Cyc_String_zstrcmp(* f,
_tag_arr("size", sizeof( unsigned char), 5u)) == 0){ return({ void*(* _temp1290)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tagged_arr
msg, struct _tagged_arr ap)= Cyc_Tcexp_expr_err; struct Cyc_Tcenv_Tenv*
_temp1294= te; struct Cyc_Position_Segment* _temp1295= loc; struct _tagged_arr
_temp1296= _tag_arr("expecting struct, union or array, found %s", sizeof(
unsigned char), 43u); struct Cyc_Stdio_String_pa_struct _temp1297; _temp1297.tag=
Cyc_Stdio_String_pa; _temp1297.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp1293=( void*)& _temp1297; void* _temp1291[ 1u]={ _temp1293};
struct _tagged_arr _temp1292={( void*) _temp1291,( void*) _temp1291,( void*)(
_temp1291 + 1u)}; _temp1290( _temp1294, _temp1295, _temp1296, _temp1292);}});}
else{ return({ void*(* _temp1298)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tagged_arr msg, struct _tagged_arr ap)= Cyc_Tcexp_expr_err; struct
Cyc_Tcenv_Tenv* _temp1302= te; struct Cyc_Position_Segment* _temp1303= loc;
struct _tagged_arr _temp1304= _tag_arr("expecting struct or union, found %s",
sizeof( unsigned char), 36u); struct Cyc_Stdio_String_pa_struct _temp1305;
_temp1305.tag= Cyc_Stdio_String_pa; _temp1305.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp1301=( void*)& _temp1305; void* _temp1299[ 1u]={ _temp1301};
struct _tagged_arr _temp1300={( void*) _temp1299,( void*) _temp1299,( void*)(
_temp1299 + 1u)}; _temp1298( _temp1302, _temp1303, _temp1304, _temp1300);}});}
_LL1193:;}} static void* Cyc_Tcexp_tcStructArrow( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct
_tagged_arr* f){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp1306= t;
struct Cyc_Absyn_PtrInfo _temp1312; struct Cyc_Absyn_Conref* _temp1314; void*
_temp1316; void* _temp1318; _LL1308: if(( unsigned int) _temp1306 > 4u?*(( int*)
_temp1306) == Cyc_Absyn_PointerType: 0){ _LL1313: _temp1312=(( struct Cyc_Absyn_PointerType_struct*)
_temp1306)->f1; _LL1319: _temp1318=( void*) _temp1312.elt_typ; goto _LL1317;
_LL1317: _temp1316=( void*) _temp1312.rgn_typ; goto _LL1315; _LL1315: _temp1314=
_temp1312.bounds; goto _LL1309;} else{ goto _LL1310;} _LL1310: goto _LL1311;
_LL1309: Cyc_Tcutil_check_nonzero_bound( loc, _temp1314);{ void* _temp1320= Cyc_Tcutil_compress(
_temp1318); struct Cyc_Absyn_Structdecl** _temp1332; struct Cyc_Absyn_Structdecl*
_temp1334; struct Cyc_List_List* _temp1335; struct _tuple1* _temp1337; struct
Cyc_Absyn_Uniondecl** _temp1339; struct Cyc_Absyn_Uniondecl* _temp1341; struct
Cyc_List_List* _temp1342; struct _tuple1* _temp1344; struct Cyc_List_List*
_temp1346; struct Cyc_List_List* _temp1348; _LL1322: if(( unsigned int)
_temp1320 > 4u?*(( int*) _temp1320) == Cyc_Absyn_StructType: 0){ _LL1338:
_temp1337=(( struct Cyc_Absyn_StructType_struct*) _temp1320)->f1; goto _LL1336;
_LL1336: _temp1335=(( struct Cyc_Absyn_StructType_struct*) _temp1320)->f2; goto
_LL1333; _LL1333: _temp1332=(( struct Cyc_Absyn_StructType_struct*) _temp1320)->f3;
if( _temp1332 == 0){ goto _LL1324;} else{ _temp1334=* _temp1332; goto _LL1323;}}
else{ goto _LL1324;} _LL1324: if(( unsigned int) _temp1320 > 4u?*(( int*)
_temp1320) == Cyc_Absyn_UnionType: 0){ _LL1345: _temp1344=(( struct Cyc_Absyn_UnionType_struct*)
_temp1320)->f1; goto _LL1343; _LL1343: _temp1342=(( struct Cyc_Absyn_UnionType_struct*)
_temp1320)->f2; goto _LL1340; _LL1340: _temp1339=(( struct Cyc_Absyn_UnionType_struct*)
_temp1320)->f3; if( _temp1339 == 0){ goto _LL1326;} else{ _temp1341=* _temp1339;
goto _LL1325;}} else{ goto _LL1326;} _LL1326: if(( unsigned int) _temp1320 > 4u?*((
int*) _temp1320) == Cyc_Absyn_AnonStructType: 0){ _LL1347: _temp1346=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1320)->f1; goto _LL1327;} else{ goto
_LL1328;} _LL1328: if(( unsigned int) _temp1320 > 4u?*(( int*) _temp1320) == Cyc_Absyn_AnonUnionType:
0){ _LL1349: _temp1348=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1320)->f1;
goto _LL1329;} else{ goto _LL1330;} _LL1330: goto _LL1331; _LL1323: { struct Cyc_Absyn_Structfield*
_temp1350= Cyc_Absyn_lookup_struct_field( _temp1334, f); if( _temp1350 == 0){
return({ void*(* _temp1351)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tagged_arr msg, struct _tagged_arr ap)= Cyc_Tcexp_expr_err; struct
Cyc_Tcenv_Tenv* _temp1356= te; struct Cyc_Position_Segment* _temp1357= loc;
struct _tagged_arr _temp1358= _tag_arr("struct %s has no %s field", sizeof(
unsigned char), 26u); struct Cyc_Stdio_String_pa_struct _temp1360; _temp1360.tag=
Cyc_Stdio_String_pa; _temp1360.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp1337));{ void* _temp1354=( void*)& _temp1360;
struct Cyc_Stdio_String_pa_struct _temp1359; _temp1359.tag= Cyc_Stdio_String_pa;
_temp1359.f1=( struct _tagged_arr)* f;{ void* _temp1355=( void*)& _temp1359;
void* _temp1352[ 2u]={ _temp1354, _temp1355}; struct _tagged_arr _temp1353={(
void*) _temp1352,( void*) _temp1352,( void*)( _temp1352 + 2u)}; _temp1351(
_temp1356, _temp1357, _temp1358, _temp1353);}}});}{ void* t3;{ struct
_RegionHandle _temp1361= _new_region(); struct _RegionHandle* rgn=& _temp1361;
_push_region( rgn);{ struct Cyc_List_List* _temp1362=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp1334->tvs, _temp1335);
t3= Cyc_Tcutil_rsubstitute( rgn, _temp1362,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1350))->type);}; _pop_region( rgn);} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1316); return t3;}} _LL1325: { struct Cyc_Absyn_Structfield*
_temp1363= Cyc_Absyn_lookup_union_field( _temp1341, f); if( _temp1363 == 0){
return({ void*(* _temp1364)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tagged_arr msg, struct _tagged_arr ap)= Cyc_Tcexp_expr_err; struct
Cyc_Tcenv_Tenv* _temp1369= te; struct Cyc_Position_Segment* _temp1370= loc;
struct _tagged_arr _temp1371= _tag_arr("union %s has no %s field", sizeof(
unsigned char), 25u); struct Cyc_Stdio_String_pa_struct _temp1373; _temp1373.tag=
Cyc_Stdio_String_pa; _temp1373.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp1344));{ void* _temp1367=( void*)& _temp1373;
struct Cyc_Stdio_String_pa_struct _temp1372; _temp1372.tag= Cyc_Stdio_String_pa;
_temp1372.f1=( struct _tagged_arr)* f;{ void* _temp1368=( void*)& _temp1372;
void* _temp1365[ 2u]={ _temp1367, _temp1368}; struct _tagged_arr _temp1366={(
void*) _temp1365,( void*) _temp1365,( void*)( _temp1365 + 2u)}; _temp1364(
_temp1369, _temp1370, _temp1371, _temp1366);}}});}{ struct Cyc_List_List*
_temp1374=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp1341->tvs, _temp1342); void* _temp1375= Cyc_Tcutil_substitute(
_temp1374,( void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp1363))->type);
Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1316); return _temp1375;}} _LL1327: {
struct Cyc_Absyn_Structfield* _temp1376= Cyc_Absyn_lookup_field( _temp1346, f);
if( _temp1376 == 0){ return({ void*(* _temp1377)( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct _tagged_arr msg, struct _tagged_arr ap)=
Cyc_Tcexp_expr_err; struct Cyc_Tcenv_Tenv* _temp1381= te; struct Cyc_Position_Segment*
_temp1382= loc; struct _tagged_arr _temp1383= _tag_arr("struct has no %s field",
sizeof( unsigned char), 23u); struct Cyc_Stdio_String_pa_struct _temp1384;
_temp1384.tag= Cyc_Stdio_String_pa; _temp1384.f1=( struct _tagged_arr)* f;{ void*
_temp1380=( void*)& _temp1384; void* _temp1378[ 1u]={ _temp1380}; struct
_tagged_arr _temp1379={( void*) _temp1378,( void*) _temp1378,( void*)( _temp1378
+ 1u)}; _temp1377( _temp1381, _temp1382, _temp1383, _temp1379);}});} return(
void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp1376))->type;} _LL1329: {
struct Cyc_Absyn_Structfield* _temp1385= Cyc_Absyn_lookup_field( _temp1348, f);
if( _temp1385 == 0){ return({ void*(* _temp1386)( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct _tagged_arr msg, struct _tagged_arr ap)=
Cyc_Tcexp_expr_err; struct Cyc_Tcenv_Tenv* _temp1390= te; struct Cyc_Position_Segment*
_temp1391= loc; struct _tagged_arr _temp1392= _tag_arr("union has no %s field",
sizeof( unsigned char), 22u); struct Cyc_Stdio_String_pa_struct _temp1393;
_temp1393.tag= Cyc_Stdio_String_pa; _temp1393.f1=( struct _tagged_arr)* f;{ void*
_temp1389=( void*)& _temp1393; void* _temp1387[ 1u]={ _temp1389}; struct
_tagged_arr _temp1388={( void*) _temp1387,( void*) _temp1387,( void*)( _temp1387
+ 1u)}; _temp1386( _temp1390, _temp1391, _temp1392, _temp1388);}});} return(
void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp1385))->type;} _LL1331:
return({ void*(* _temp1394)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tagged_arr msg, struct _tagged_arr ap)= Cyc_Tcexp_expr_err; struct
Cyc_Tcenv_Tenv* _temp1398= te; struct Cyc_Position_Segment* _temp1399= loc;
struct _tagged_arr _temp1400= _tag_arr("expecting struct pointer, found %s",
sizeof( unsigned char), 35u); struct Cyc_Stdio_String_pa_struct _temp1401;
_temp1401.tag= Cyc_Stdio_String_pa; _temp1401.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp1397=( void*)& _temp1401; void* _temp1395[ 1u]={ _temp1397};
struct _tagged_arr _temp1396={( void*) _temp1395,( void*) _temp1395,( void*)(
_temp1395 + 1u)}; _temp1394( _temp1398, _temp1399, _temp1400, _temp1396);}});
_LL1321:;} _LL1311: return({ void*(* _temp1402)( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct _tagged_arr msg, struct _tagged_arr ap)=
Cyc_Tcexp_expr_err; struct Cyc_Tcenv_Tenv* _temp1406= te; struct Cyc_Position_Segment*
_temp1407= loc; struct _tagged_arr _temp1408= _tag_arr("expecting struct pointer, found %s",
sizeof( unsigned char), 35u); struct Cyc_Stdio_String_pa_struct _temp1409;
_temp1409.tag= Cyc_Stdio_String_pa; _temp1409.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp1405=( void*)& _temp1409; void* _temp1403[ 1u]={ _temp1405};
struct _tagged_arr _temp1404={( void*) _temp1403,( void*) _temp1403,( void*)(
_temp1403 + 1u)}; _temp1402( _temp1406, _temp1407, _temp1408, _temp1404);}});
_LL1307:;}} static void* Cyc_Tcexp_ithTupleType( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct Cyc_List_List* ts, struct Cyc_Absyn_Exp*
index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp( index); struct
_handler_cons _temp1410; _push_handler(& _temp1410);{ int _temp1412= 0; if(
setjmp( _temp1410.handler)){ _temp1412= 1;} if( ! _temp1412){{ void* _temp1413=(*((
struct _tuple8*(*)( struct Cyc_List_List* x, int i)) Cyc_List_nth)( ts,( int) i)).f2;
_npop_handler( 0u); return _temp1413;}; _pop_handler();} else{ void* _temp1411=(
void*) _exn_thrown; void* _temp1415= _temp1411; _LL1417: if( _temp1415 == Cyc_List_Nth){
goto _LL1418;} else{ goto _LL1419;} _LL1419: goto _LL1420; _LL1418: return({
void*(* _temp1421)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
struct _tagged_arr msg, struct _tagged_arr ap)= Cyc_Tcexp_expr_err; struct Cyc_Tcenv_Tenv*
_temp1426= te; struct Cyc_Position_Segment* _temp1427= loc; struct _tagged_arr
_temp1428= _tag_arr("index is %d but tuple has only %d fields", sizeof(
unsigned char), 41u); struct Cyc_Stdio_Int_pa_struct _temp1430; _temp1430.tag=
Cyc_Stdio_Int_pa; _temp1430.f1=( int) i;{ void* _temp1424=( void*)& _temp1430;
struct Cyc_Stdio_Int_pa_struct _temp1429; _temp1429.tag= Cyc_Stdio_Int_pa;
_temp1429.f1=( int)(( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
ts));{ void* _temp1425=( void*)& _temp1429; void* _temp1422[ 2u]={ _temp1424,
_temp1425}; struct _tagged_arr _temp1423={( void*) _temp1422,( void*) _temp1422,(
void*)( _temp1422 + 2u)}; _temp1421( _temp1426, _temp1427, _temp1428, _temp1423);}}});
_LL1420:( void) _throw( _temp1415); _LL1416:;}}} static void* Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp(
te, 0, e2);{ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ return({
void*(* _temp1431)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
struct _tagged_arr msg, struct _tagged_arr ap)= Cyc_Tcexp_expr_err; struct Cyc_Tcenv_Tenv*
_temp1435= te; struct Cyc_Position_Segment* _temp1436= e2->loc; struct
_tagged_arr _temp1437= _tag_arr("expecting int subscript, found %s", sizeof(
unsigned char), 34u); struct Cyc_Stdio_String_pa_struct _temp1438; _temp1438.tag=
Cyc_Stdio_String_pa; _temp1438.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t2);{ void* _temp1434=( void*)& _temp1438; void* _temp1432[ 1u]={ _temp1434};
struct _tagged_arr _temp1433={( void*) _temp1432,( void*) _temp1432,( void*)(
_temp1432 + 1u)}; _temp1431( _temp1435, _temp1436, _temp1437, _temp1433);}});}{
void* _temp1439= t1; struct Cyc_Absyn_PtrInfo _temp1447; struct Cyc_Absyn_Conref*
_temp1449; struct Cyc_Absyn_Tqual _temp1451; void* _temp1453; void* _temp1455;
struct Cyc_List_List* _temp1457; _LL1441: if(( unsigned int) _temp1439 > 4u?*((
int*) _temp1439) == Cyc_Absyn_PointerType: 0){ _LL1448: _temp1447=(( struct Cyc_Absyn_PointerType_struct*)
_temp1439)->f1; _LL1456: _temp1455=( void*) _temp1447.elt_typ; goto _LL1454;
_LL1454: _temp1453=( void*) _temp1447.rgn_typ; goto _LL1452; _LL1452: _temp1451=
_temp1447.tq; goto _LL1450; _LL1450: _temp1449= _temp1447.bounds; goto _LL1442;}
else{ goto _LL1443;} _LL1443: if(( unsigned int) _temp1439 > 4u?*(( int*)
_temp1439) == Cyc_Absyn_TupleType: 0){ _LL1458: _temp1457=(( struct Cyc_Absyn_TupleType_struct*)
_temp1439)->f1; goto _LL1444;} else{ goto _LL1445;} _LL1445: goto _LL1446;
_LL1442: if( Cyc_Tcutil_is_const_exp( te, e2)){ Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp(
e2), _temp1449);} else{{ void* _temp1459=( void*)( Cyc_Absyn_compress_conref(
_temp1449))->v; void* _temp1465; struct Cyc_Absyn_Exp* _temp1467; struct Cyc_Absyn_Exp
_temp1469; void* _temp1470; void* _temp1472; int _temp1474; void* _temp1476;
_LL1461: if(( unsigned int) _temp1459 > 1u?*(( int*) _temp1459) == Cyc_Absyn_Eq_constr:
0){ _LL1466: _temp1465=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1459)->f1;
if(( unsigned int) _temp1465 > 1u?*(( int*) _temp1465) == Cyc_Absyn_Upper_b: 0){
_LL1468: _temp1467=(( struct Cyc_Absyn_Upper_b_struct*) _temp1465)->f1;
_temp1469=* _temp1467; _LL1471: _temp1470=( void*) _temp1469.r; if(*(( int*)
_temp1470) == Cyc_Absyn_Const_e){ _LL1473: _temp1472=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1470)->f1; if(( unsigned int) _temp1472 > 1u?*(( int*) _temp1472) == Cyc_Absyn_Int_c:
0){ _LL1477: _temp1476=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1472)->f1;
if( _temp1476 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1475;} else{ goto _LL1463;}
_LL1475: _temp1474=(( struct Cyc_Absyn_Int_c_struct*) _temp1472)->f2; if(
_temp1474 == 1){ goto _LL1462;} else{ goto _LL1463;}} else{ goto _LL1463;}}
else{ goto _LL1463;}} else{ goto _LL1463;}} else{ goto _LL1463;} _LL1463: goto
_LL1464; _LL1462:({ void(* _temp1478)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_warn; struct Cyc_Position_Segment*
_temp1481= e1->loc; struct _tagged_arr _temp1482= _tag_arr("subscript applied to pointer to one object",
sizeof( unsigned char), 43u); void* _temp1479[ 0u]={}; struct _tagged_arr
_temp1480={( void*) _temp1479,( void*) _temp1479,( void*)( _temp1479 + 0u)};
_temp1478( _temp1481, _temp1482, _temp1480);}); goto _LL1460; _LL1464: goto
_LL1460; _LL1460:;} Cyc_Tcutil_check_nonzero_bound( loc, _temp1449);} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1453); return _temp1455; _LL1444: return Cyc_Tcexp_ithTupleType(
te, loc, _temp1457, e2); _LL1446: return({ void*(* _temp1483)( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tagged_arr msg, struct _tagged_arr
ap)= Cyc_Tcexp_expr_err; struct Cyc_Tcenv_Tenv* _temp1487= te; struct Cyc_Position_Segment*
_temp1488= loc; struct _tagged_arr _temp1489= _tag_arr("subscript applied to %s",
sizeof( unsigned char), 24u); struct Cyc_Stdio_String_pa_struct _temp1490;
_temp1490.tag= Cyc_Stdio_String_pa; _temp1490.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp1486=( void*)& _temp1490; void* _temp1484[ 1u]={ _temp1486};
struct _tagged_arr _temp1485={( void*) _temp1484,( void*) _temp1484,( void*)(
_temp1484 + 1u)}; _temp1483( _temp1487, _temp1488, _temp1489, _temp1485);}});
_LL1440:;}}} static void* Cyc_Tcexp_tcTuple( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_List_List* es){ int done= 0;
struct Cyc_List_List* fields= 0; if( topt != 0){ void* _temp1491= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_List_List* _temp1497; _LL1493: if((
unsigned int) _temp1491 > 4u?*(( int*) _temp1491) == Cyc_Absyn_TupleType: 0){
_LL1498: _temp1497=(( struct Cyc_Absyn_TupleType_struct*) _temp1491)->f1; goto
_LL1494;} else{ goto _LL1495;} _LL1495: goto _LL1496; _LL1494: if((( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp1497) !=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es)){({ void(* _temp1499)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp1502= loc; struct _tagged_arr _temp1503= _tag_arr("tuple expression has the wrong number of fields",
sizeof( unsigned char), 48u); void* _temp1500[ 0u]={}; struct _tagged_arr
_temp1501={( void*) _temp1500,( void*) _temp1500,( void*)( _temp1500 + 0u)};
_temp1499( _temp1502, _temp1503, _temp1501);}); goto _LL1492;} for( 0; es != 0;(
es=(( struct Cyc_List_List*) _check_null( es))->tl, _temp1497=(( struct Cyc_List_List*)
_check_null( _temp1497))->tl)){ void* _temp1504=(*(( struct _tuple8*)(( struct
Cyc_List_List*) _check_null( _temp1497))->hd)).f2; Cyc_Tcexp_tcExpInitializer(
te,( void**)& _temp1504,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd); fields=({ struct Cyc_List_List* _temp1505=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1505->hd=( void*)({ struct
_tuple8* _temp1506=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8));
_temp1506->f1=(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null(
_temp1497))->hd)).f1; _temp1506->f2=( void*)(( struct Cyc_Core_Opt*) _check_null(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v;
_temp1506;}); _temp1505->tl= fields; _temp1505;});} done= 1; goto _LL1492;
_LL1496: goto _LL1492; _LL1492:;} if( ! done){ for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ Cyc_Tcexp_tcExpInitializer( te, 0,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd); fields=({ struct Cyc_List_List*
_temp1507=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1507->hd=( void*)({ struct _tuple8* _temp1508=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp1508->f1= Cyc_Absyn_empty_tqual(); _temp1508->f2=(
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd)->topt))->v; _temp1508;}); _temp1507->tl=
fields; _temp1507;});}} return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp1509=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp1509[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp1510; _temp1510.tag= Cyc_Absyn_TupleType;
_temp1510.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp1510;}); _temp1509;});} static void* Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
_tuple2* t, struct Cyc_List_List* des){ return({ void*(* _temp1511)( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tagged_arr msg, struct _tagged_arr
ap)= Cyc_Tcexp_expr_err; struct Cyc_Tcenv_Tenv* _temp1514= te; struct Cyc_Position_Segment*
_temp1515= loc; struct _tagged_arr _temp1516= _tag_arr("tcCompoundLit", sizeof(
unsigned char), 14u); void* _temp1512[ 0u]={}; struct _tagged_arr _temp1513={(
void*) _temp1512,( void*) _temp1512,( void*)( _temp1512 + 0u)}; _temp1511(
_temp1514, _temp1515, _temp1516, _temp1513);});} static void* Cyc_Tcexp_tcArray(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** elt_topt,
struct Cyc_List_List* des){ struct Cyc_List_List* es=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple7*)) Cyc_Core_snd, des); void* res= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_mk,({ struct Cyc_Core_Opt* _temp1554=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1554->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp1554;})); struct Cyc_Absyn_Const_e_struct*
_temp1517=({ struct Cyc_Absyn_Const_e_struct* _temp1550=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1550[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1551; _temp1551.tag= Cyc_Absyn_Const_e; _temp1551.f1=(
void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp1552=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp1552[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp1553; _temp1553.tag= Cyc_Absyn_Int_c; _temp1553.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp1553.f2=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es);
_temp1553;}); _temp1552;})); _temp1551;}); _temp1550;}); struct Cyc_Absyn_Exp*
sz_exp= Cyc_Absyn_new_exp(( void*) _temp1517, loc); sz_exp->topt=({ struct Cyc_Core_Opt*
_temp1518=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1518->v=( void*) Cyc_Absyn_uint_t; _temp1518;});{ void* res_t2=( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp1548=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1548[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1549; _temp1549.tag= Cyc_Absyn_ArrayType;
_temp1549.f1=( void*) res; _temp1549.f2= Cyc_Absyn_empty_tqual(); _temp1549.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp1549;}); _temp1548;});{ struct Cyc_List_List*
es2= es; for( 0; es2 != 0; es2=(( struct Cyc_List_List*) _check_null( es2))->tl){
Cyc_Tcexp_tcExpInitializer( te, elt_topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es2))->hd);}} if( ! Cyc_Tcutil_coerce_list( te, res, es)){({ void(*
_temp1519)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp1523=((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->loc;
struct _tagged_arr _temp1524= _tag_arr("elements of array do not all have the same type (%s)",
sizeof( unsigned char), 53u); struct Cyc_Stdio_String_pa_struct _temp1525;
_temp1525.tag= Cyc_Stdio_String_pa; _temp1525.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
res);{ void* _temp1522=( void*)& _temp1525; void* _temp1520[ 1u]={ _temp1522};
struct _tagged_arr _temp1521={( void*) _temp1520,( void*) _temp1520,( void*)(
_temp1520 + 1u)}; _temp1519( _temp1523, _temp1524, _temp1521);}});}{ int offset=
0; for( 0; des != 0;( offset ++, des=(( struct Cyc_List_List*) _check_null( des))->tl)){
struct Cyc_List_List* ds=(*(( struct _tuple7*)(( struct Cyc_List_List*)
_check_null( des))->hd)).f1; if( ds != 0){ void* _temp1526=( void*)(( struct Cyc_List_List*)
_check_null( ds))->hd; struct Cyc_Absyn_Exp* _temp1532; _LL1528: if(*(( int*)
_temp1526) == Cyc_Absyn_FieldName){ goto _LL1529;} else{ goto _LL1530;} _LL1530:
if(*(( int*) _temp1526) == Cyc_Absyn_ArrayElement){ _LL1533: _temp1532=(( struct
Cyc_Absyn_ArrayElement_struct*) _temp1526)->f1; goto _LL1531;} else{ goto
_LL1527;} _LL1529:({ void(* _temp1534)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp1537= loc; struct _tagged_arr _temp1538= _tag_arr("only array index designators are supported",
sizeof( unsigned char), 43u); void* _temp1535[ 0u]={}; struct _tagged_arr
_temp1536={( void*) _temp1535,( void*) _temp1535,( void*)( _temp1535 + 0u)};
_temp1534( _temp1537, _temp1538, _temp1536);}); goto _LL1527; _LL1531: Cyc_Tcexp_tcExpInitializer(
te, 0, _temp1532);{ unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp1532);
if( i != offset){({ void(* _temp1539)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp1544= _temp1532->loc; struct _tagged_arr _temp1545= _tag_arr("expecting index designator %d but found %d",
sizeof( unsigned char), 43u); struct Cyc_Stdio_Int_pa_struct _temp1547;
_temp1547.tag= Cyc_Stdio_Int_pa; _temp1547.f1=( int)(( unsigned int) offset);{
void* _temp1542=( void*)& _temp1547; struct Cyc_Stdio_Int_pa_struct _temp1546;
_temp1546.tag= Cyc_Stdio_Int_pa; _temp1546.f1=( int) i;{ void* _temp1543=( void*)&
_temp1546; void* _temp1540[ 2u]={ _temp1542, _temp1543}; struct _tagged_arr
_temp1541={( void*) _temp1540,( void*) _temp1540,( void*)( _temp1540 + 2u)};
_temp1539( _temp1544, _temp1545, _temp1541);}}});} goto _LL1527;} _LL1527:;}}}
return res_t2;}} static void* Cyc_Tcexp_tcComprehension( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* bound, struct Cyc_Absyn_Exp* body){ Cyc_Tcexp_tcExp( te, 0,
bound); if( ! Cyc_Tcutil_coerce_uint_typ( te, bound)){({ void(* _temp1555)(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp1559= bound->loc; struct
_tagged_arr _temp1560= _tag_arr("expecting unsigned int, found %s", sizeof(
unsigned char), 33u); struct Cyc_Stdio_String_pa_struct _temp1561; _temp1561.tag=
Cyc_Stdio_String_pa; _temp1561.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( bound->topt))->v);{ void* _temp1558=(
void*)& _temp1561; void* _temp1556[ 1u]={ _temp1558}; struct _tagged_arr
_temp1557={( void*) _temp1556,( void*) _temp1556,( void*)( _temp1556 + 1u)};
_temp1555( _temp1559, _temp1560, _temp1557);}});} if( !( vd->tq).q_const){({ int(*
_temp1562)( struct _tagged_arr fmt, struct _tagged_arr ap)=( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos; struct _tagged_arr
_temp1565= _tag_arr("comprehension index variable is not declared const!",
sizeof( unsigned char), 52u); void* _temp1563[ 0u]={}; struct _tagged_arr
_temp1564={( void*) _temp1563,( void*) _temp1563,( void*)( _temp1563 + 0u)};
_temp1562( _temp1565, _temp1564);});} if( te->le != 0){ te= Cyc_Tcenv_new_block(
loc, te); te= Cyc_Tcenv_add_local_var( loc, te, vd);} else{ if( ! Cyc_Tcutil_is_const_exp(
te, bound)){({ void(* _temp1566)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp1569= bound->loc; struct _tagged_arr _temp1570= _tag_arr("bound is not constant",
sizeof( unsigned char), 22u); void* _temp1567[ 0u]={}; struct _tagged_arr
_temp1568={( void*) _temp1567,( void*) _temp1567,( void*)( _temp1567 + 0u)};
_temp1566( _temp1569, _temp1570, _temp1568);});} if( ! Cyc_Tcutil_is_const_exp(
te, body)){({ void(* _temp1571)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp1574= bound->loc; struct _tagged_arr _temp1575= _tag_arr("body is not constant",
sizeof( unsigned char), 21u); void* _temp1572[ 0u]={}; struct _tagged_arr
_temp1573={( void*) _temp1572,( void*) _temp1572,( void*)( _temp1572 + 0u)};
_temp1571( _temp1574, _temp1575, _temp1573);});}}{ struct Cyc_Absyn_PtrInfo
pinfo; void** _temp1576= 0; struct Cyc_Absyn_Tqual* _temp1577= 0; if( topt != 0){
void* _temp1578= Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct
Cyc_Absyn_PtrInfo _temp1586; struct Cyc_Absyn_Exp* _temp1588; struct Cyc_Absyn_Tqual
_temp1590; struct Cyc_Absyn_Tqual* _temp1592; void* _temp1593; void** _temp1595;
_LL1580: if(( unsigned int) _temp1578 > 4u?*(( int*) _temp1578) == Cyc_Absyn_PointerType:
0){ _LL1587: _temp1586=(( struct Cyc_Absyn_PointerType_struct*) _temp1578)->f1;
goto _LL1581;} else{ goto _LL1582;} _LL1582: if(( unsigned int) _temp1578 > 4u?*((
int*) _temp1578) == Cyc_Absyn_ArrayType: 0){ _LL1594: _temp1593=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1578)->f1; _temp1595=&(( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1578)->f1); goto _LL1591; _LL1591:
_temp1590=(( struct Cyc_Absyn_ArrayType_struct*) _temp1578)->f2; _temp1592=&((
struct Cyc_Absyn_ArrayType_struct*) _temp1578)->f2; goto _LL1589; _LL1589:
_temp1588=(( struct Cyc_Absyn_ArrayType_struct*) _temp1578)->f3; goto _LL1583;}
else{ goto _LL1584;} _LL1584: goto _LL1585; _LL1581: pinfo= _temp1586; _temp1576=(
void**)(( void**)(( void*)& pinfo.elt_typ)); _temp1577=( struct Cyc_Absyn_Tqual*)&
pinfo.tq; goto _LL1579; _LL1583: _temp1576=( void**) _temp1595; _temp1577=(
struct Cyc_Absyn_Tqual*) _temp1592; goto _LL1579; _LL1585: goto _LL1579; _LL1579:;}{
void* t= Cyc_Tcexp_tcExp( te, _temp1576, body); return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1596=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1596[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1597; _temp1597.tag= Cyc_Absyn_ArrayType;
_temp1597.f1=( void*) t; _temp1597.f2= _temp1577 == 0? Cyc_Absyn_empty_tqual():*((
struct Cyc_Absyn_Tqual*) _check_null( _temp1577)); _temp1597.f3=( struct Cyc_Absyn_Exp*)
bound; _temp1597;}); _temp1596;});}}} struct _tuple9{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void* Cyc_Tcexp_tcStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct _tuple1** tn, struct
Cyc_Core_Opt** otyps, struct Cyc_List_List* args, struct Cyc_Absyn_Structdecl**
sd_opt){ struct Cyc_Absyn_Structdecl* sd; if(* sd_opt != 0){ sd=( struct Cyc_Absyn_Structdecl*)
_check_null(* sd_opt);} else{{ struct _handler_cons _temp1598; _push_handler(&
_temp1598);{ int _temp1600= 0; if( setjmp( _temp1598.handler)){ _temp1600= 1;}
if( ! _temp1600){ sd=* Cyc_Tcenv_lookup_structdecl( te, loc,* tn);; _pop_handler();}
else{ void* _temp1599=( void*) _exn_thrown; void* _temp1602= _temp1599; _LL1604:
if( _temp1602 == Cyc_Dict_Absent){ goto _LL1605;} else{ goto _LL1606;} _LL1606:
goto _LL1607; _LL1605:({ void(* _temp1608)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp1612= loc; struct _tagged_arr _temp1613= _tag_arr("unbound struct name %s",
sizeof( unsigned char), 23u); struct Cyc_Stdio_String_pa_struct _temp1614;
_temp1614.tag= Cyc_Stdio_String_pa; _temp1614.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(*
tn);{ void* _temp1611=( void*)& _temp1614; void* _temp1609[ 1u]={ _temp1611};
struct _tagged_arr _temp1610={( void*) _temp1609,( void*) _temp1609,( void*)(
_temp1609 + 1u)}; _temp1608( _temp1612, _temp1613, _temp1610);}}); return topt
!= 0?*(( void**) _check_null( topt)):( void*) Cyc_Absyn_VoidType; _LL1607:( void)
_throw( _temp1602); _LL1603:;}}}* sd_opt=( struct Cyc_Absyn_Structdecl*) sd; if(
sd->name != 0){* tn=( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v;}}{
struct _RegionHandle _temp1615= _new_region(); struct _RegionHandle* rgn=&
_temp1615; _push_region( rgn);{ struct _tuple4 _temp1616=({ struct _tuple4
_temp1652; _temp1652.f1= Cyc_Tcenv_lookup_type_vars( te); _temp1652.f2= rgn;
_temp1652;}); struct Cyc_List_List* _temp1617=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1616, sd->tvs); struct Cyc_List_List* _temp1618=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp1617); struct Cyc_Absyn_StructType_struct*
_temp1619=({ struct Cyc_Absyn_StructType_struct* _temp1649=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1649[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1650; _temp1650.tag= Cyc_Absyn_StructType;
_temp1650.f1=( struct _tuple1*)* tn; _temp1650.f2= _temp1618; _temp1650.f3=({
struct Cyc_Absyn_Structdecl** _temp1651=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp1651[ 0]= sd; _temp1651;});
_temp1650;}); _temp1649;}); if( topt != 0){ Cyc_Tcutil_unify(( void*) _temp1619,*((
void**) _check_null( topt)));}* otyps=({ struct Cyc_Core_Opt* _temp1620=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1620->v=( void*)
_temp1618; _temp1620;}); if( sd->fields == 0){({ void(* _temp1621)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp1624= loc; struct _tagged_arr _temp1625= _tag_arr("can't build abstract struct",
sizeof( unsigned char), 28u); void* _temp1622[ 0u]={}; struct _tagged_arr
_temp1623={( void*) _temp1622,( void*) _temp1622,( void*)( _temp1622 + 0u)};
_temp1621( _temp1624, _temp1625, _temp1623);});{ void* _temp1626=( void*)
_temp1619; _npop_handler( 0u); return _temp1626;}}{ struct Cyc_List_List* fields=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd->fields))->v);
for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){
struct _tuple9 _temp1629; struct Cyc_Absyn_Exp* _temp1630; struct Cyc_Absyn_Structfield*
_temp1632; struct _tuple9* _temp1627=( struct _tuple9*)(( struct Cyc_List_List*)
_check_null( fields))->hd; _temp1629=* _temp1627; _LL1633: _temp1632= _temp1629.f1;
goto _LL1631; _LL1631: _temp1630= _temp1629.f2; goto _LL1628; _LL1628: { void*
_temp1634= Cyc_Tcutil_rsubstitute( rgn, _temp1617,( void*) _temp1632->type); Cyc_Tcexp_tcExpInitializer(
te,( void**)& _temp1634, _temp1630); if( ! Cyc_Tcutil_coerce_arg( te, _temp1630,
_temp1634)){({ void(* _temp1635)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp1642= _temp1630->loc; struct _tagged_arr _temp1643= _tag_arr("field %s of struct %s expects type %s != %s",
sizeof( unsigned char), 44u); struct Cyc_Stdio_String_pa_struct _temp1647;
_temp1647.tag= Cyc_Stdio_String_pa; _temp1647.f1=( struct _tagged_arr)*
_temp1632->name;{ void* _temp1638=( void*)& _temp1647; struct Cyc_Stdio_String_pa_struct
_temp1646; _temp1646.tag= Cyc_Stdio_String_pa; _temp1646.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(* tn);{ void* _temp1639=( void*)& _temp1646; struct Cyc_Stdio_String_pa_struct
_temp1645; _temp1645.tag= Cyc_Stdio_String_pa; _temp1645.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( _temp1634);{ void* _temp1640=( void*)& _temp1645; struct
Cyc_Stdio_String_pa_struct _temp1644; _temp1644.tag= Cyc_Stdio_String_pa;
_temp1644.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1630->topt))->v);{ void* _temp1641=( void*)& _temp1644; void*
_temp1636[ 4u]={ _temp1638, _temp1639, _temp1640, _temp1641}; struct _tagged_arr
_temp1637={( void*) _temp1636,( void*) _temp1636,( void*)( _temp1636 + 4u)};
_temp1635( _temp1642, _temp1643, _temp1637);}}}}});}}}{ void* _temp1648=( void*)
_temp1619; _npop_handler( 0u); return _temp1648;}}}; _pop_region( rgn);}} static
void* Cyc_Tcexp_tcAnonStruct( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void* ts, struct Cyc_List_List* args){{ struct _RegionHandle _temp1653=
_new_region(); struct _RegionHandle* rgn=& _temp1653; _push_region( rgn);{ void*
_temp1654= Cyc_Tcutil_compress( ts); struct Cyc_List_List* _temp1660; _LL1656:
if(( unsigned int) _temp1654 > 4u?*(( int*) _temp1654) == Cyc_Absyn_AnonStructType:
0){ _LL1661: _temp1660=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1654)->f1;
goto _LL1657;} else{ goto _LL1658;} _LL1658: goto _LL1659; _LL1657: { struct Cyc_List_List*
fields=(( struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args, _temp1660); for( 0; fields != 0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ struct _tuple9 _temp1664; struct Cyc_Absyn_Exp*
_temp1665; struct Cyc_Absyn_Structfield* _temp1667; struct _tuple9* _temp1662=(
struct _tuple9*)(( struct Cyc_List_List*) _check_null( fields))->hd; _temp1664=*
_temp1662; _LL1668: _temp1667= _temp1664.f1; goto _LL1666; _LL1666: _temp1665=
_temp1664.f2; goto _LL1663; _LL1663: Cyc_Tcexp_tcExpInitializer( te,( void**)((
void**)(( void*)& _temp1667->type)), _temp1665); if( ! Cyc_Tcutil_coerce_arg( te,
_temp1665,( void*) _temp1667->type)){({ void(* _temp1669)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp1675= _temp1665->loc; struct _tagged_arr _temp1676= _tag_arr("field %s of struct expects type %s != %s",
sizeof( unsigned char), 41u); struct Cyc_Stdio_String_pa_struct _temp1679;
_temp1679.tag= Cyc_Stdio_String_pa; _temp1679.f1=( struct _tagged_arr)*
_temp1667->name;{ void* _temp1672=( void*)& _temp1679; struct Cyc_Stdio_String_pa_struct
_temp1678; _temp1678.tag= Cyc_Stdio_String_pa; _temp1678.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*) _temp1667->type);{ void* _temp1673=( void*)&
_temp1678; struct Cyc_Stdio_String_pa_struct _temp1677; _temp1677.tag= Cyc_Stdio_String_pa;
_temp1677.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1665->topt))->v);{ void* _temp1674=( void*)& _temp1677; void*
_temp1670[ 3u]={ _temp1672, _temp1673, _temp1674}; struct _tagged_arr _temp1671={(
void*) _temp1670,( void*) _temp1670,( void*)( _temp1670 + 3u)}; _temp1669(
_temp1675, _temp1676, _temp1671);}}}});}} goto _LL1655;} _LL1659:({ int(*
_temp1680)( struct _tagged_arr fmt, struct _tagged_arr ap)=( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos; struct _tagged_arr
_temp1683= _tag_arr("tcAnonStruct:  not an AnonStructType", sizeof(
unsigned char), 37u); void* _temp1681[ 0u]={}; struct _tagged_arr _temp1682={(
void*) _temp1681,( void*) _temp1681,( void*)( _temp1681 + 0u)}; _temp1680(
_temp1683, _temp1682);}); goto _LL1655; _LL1655:;}; _pop_region( rgn);} return
ts;} static void* Cyc_Tcexp_tcTunion( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_Core_Opt** all_ref, struct
Cyc_Core_Opt** exist_ref, struct Cyc_List_List* es, struct Cyc_Absyn_Tuniondecl*
tud, struct Cyc_Absyn_Tunionfield* tuf){ struct _RegionHandle _temp1684=
_new_region(); struct _RegionHandle* rgn=& _temp1684; _push_region( rgn);{
struct _tuple4 _temp1685=({ struct _tuple4 _temp1749; _temp1749.f1= Cyc_Tcenv_lookup_type_vars(
te); _temp1749.f2= rgn; _temp1749;}); struct Cyc_List_List* _temp1686=(( struct
Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*,
struct Cyc_Absyn_Tvar*), struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)(
rgn, Cyc_Tcutil_r_make_inst_var,& _temp1685, tud->tvs); struct Cyc_List_List*
_temp1687=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple5*(*
f)( struct _tuple4*, struct Cyc_Absyn_Tvar*), struct _tuple4* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,& _temp1685, tuf->tvs);
struct Cyc_List_List* _temp1688=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple5*))
Cyc_Core_snd, _temp1686); struct Cyc_List_List* _temp1689=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp1687); struct Cyc_List_List* _temp1690=((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp1686, _temp1687);* all_ref=({
struct Cyc_Core_Opt* _temp1691=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1691->v=( void*) _temp1688; _temp1691;});* exist_ref=({
struct Cyc_Core_Opt* _temp1692=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1692->v=( void*) _temp1689; _temp1692;});{ void* res=( void*)({
struct Cyc_Absyn_TunionFieldType_struct* _temp1744=( struct Cyc_Absyn_TunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp1744[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp1745; _temp1745.tag= Cyc_Absyn_TunionFieldType;
_temp1745.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp1746; _temp1746.field_info=(
void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct* _temp1747=( struct
Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct Cyc_Absyn_KnownTunionfield_struct));
_temp1747[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct _temp1748; _temp1748.tag=
Cyc_Absyn_KnownTunionfield; _temp1748.f1= tud; _temp1748.f2= tuf; _temp1748;});
_temp1747;})); _temp1746.targs= _temp1688; _temp1746;}); _temp1745;}); _temp1744;});
if( topt != 0){ void* _temp1693= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_TunionInfo _temp1701; void* _temp1703; struct Cyc_List_List*
_temp1705; _LL1695: if(( unsigned int) _temp1693 > 4u?*(( int*) _temp1693) ==
Cyc_Absyn_TunionFieldType: 0){ goto _LL1696;} else{ goto _LL1697;} _LL1697: if((
unsigned int) _temp1693 > 4u?*(( int*) _temp1693) == Cyc_Absyn_TunionType: 0){
_LL1702: _temp1701=(( struct Cyc_Absyn_TunionType_struct*) _temp1693)->f1;
_LL1706: _temp1705= _temp1701.targs; goto _LL1704; _LL1704: _temp1703=( void*)
_temp1701.rgn; goto _LL1698;} else{ goto _LL1699;} _LL1699: goto _LL1700;
_LL1696: Cyc_Tcutil_unify(*(( void**) _check_null( topt)), res); goto _LL1694;
_LL1698:{ struct Cyc_List_List* a= _temp1688; for( 0; a != 0? _temp1705 != 0: 0;(
a=(( struct Cyc_List_List*) _check_null( a))->tl, _temp1705=(( struct Cyc_List_List*)
_check_null( _temp1705))->tl)){ Cyc_Tcutil_unify(( void*)(( struct Cyc_List_List*)
_check_null( a))->hd,( void*)(( struct Cyc_List_List*) _check_null( _temp1705))->hd);}}
if( tuf->typs == 0? es == 0: 0){ e->topt=({ struct Cyc_Core_Opt* _temp1707=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1707->v=(
void*) res; _temp1707;}); res=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1708=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1708[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1709; _temp1709.tag=
Cyc_Absyn_TunionType; _temp1709.f1=({ struct Cyc_Absyn_TunionInfo _temp1710;
_temp1710.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunion_struct*
_temp1711=( struct Cyc_Absyn_KnownTunion_struct*) GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct));
_temp1711[ 0]=({ struct Cyc_Absyn_KnownTunion_struct _temp1712; _temp1712.tag=
Cyc_Absyn_KnownTunion; _temp1712.f1= tud; _temp1712;}); _temp1711;})); _temp1710.targs=
_temp1688; _temp1710.rgn=( void*) _temp1703; _temp1710;}); _temp1709;});
_temp1708;}); Cyc_Tcutil_unchecked_cast( te, e, res);{ void* _temp1713= res;
_npop_handler( 0u); return _temp1713;}} goto _LL1694; _LL1700: goto _LL1694;
_LL1694:;}{ struct Cyc_List_List* ts= tuf->typs; for( 0; es != 0? ts != 0: 0;(
es=(( struct Cyc_List_List*) _check_null( es))->tl, ts=(( struct Cyc_List_List*)
_check_null( ts))->tl)){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd; void* t= Cyc_Tcutil_rsubstitute(
rgn, _temp1690,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( ts))->hd)).f2);
Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e); if( ! Cyc_Tcutil_coerce_arg( te,
e, t)){({ void(* _temp1714)( struct Cyc_Position_Segment*, struct _tagged_arr
fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp1720= e->loc; struct _tagged_arr _temp1721= _tag_arr("tunion constructor %s expects argument of type %s but this argument has type %s",
sizeof( unsigned char), 80u); struct Cyc_Stdio_String_pa_struct _temp1724;
_temp1724.tag= Cyc_Stdio_String_pa; _temp1724.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
tuf->name);{ void* _temp1717=( void*)& _temp1724; struct Cyc_Stdio_String_pa_struct
_temp1723; _temp1723.tag= Cyc_Stdio_String_pa; _temp1723.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp1718=( void*)& _temp1723; struct Cyc_Stdio_String_pa_struct
_temp1722; _temp1722.tag= Cyc_Stdio_String_pa; _temp1722.f1=( struct _tagged_arr)(
e->topt == 0? _tag_arr("?", sizeof( unsigned char), 2u): Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v));{ void* _temp1719=(
void*)& _temp1722; void* _temp1715[ 3u]={ _temp1717, _temp1718, _temp1719};
struct _tagged_arr _temp1716={( void*) _temp1715,( void*) _temp1715,( void*)(
_temp1715 + 3u)}; _temp1714( _temp1720, _temp1721, _temp1716);}}}});}} if( es !=
0){ void* _temp1733=({ void*(* _temp1725)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tagged_arr msg, struct _tagged_arr ap)= Cyc_Tcexp_expr_err; struct
Cyc_Tcenv_Tenv* _temp1729= te; struct Cyc_Position_Segment* _temp1730=(( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->loc; struct
_tagged_arr _temp1731= _tag_arr("too many arguments for tunion constructor %s",
sizeof( unsigned char), 45u); struct Cyc_Stdio_String_pa_struct _temp1732;
_temp1732.tag= Cyc_Stdio_String_pa; _temp1732.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
tuf->name);{ void* _temp1728=( void*)& _temp1732; void* _temp1726[ 1u]={
_temp1728}; struct _tagged_arr _temp1727={( void*) _temp1726,( void*) _temp1726,(
void*)( _temp1726 + 1u)}; _temp1725( _temp1729, _temp1730, _temp1731, _temp1727);}});
_npop_handler( 0u); return _temp1733;} if( ts != 0){ void* _temp1742=({ void*(*
_temp1734)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct
_tagged_arr msg, struct _tagged_arr ap)= Cyc_Tcexp_expr_err; struct Cyc_Tcenv_Tenv*
_temp1738= te; struct Cyc_Position_Segment* _temp1739= loc; struct _tagged_arr
_temp1740= _tag_arr("too few arguments for tunion constructor %s", sizeof(
unsigned char), 44u); struct Cyc_Stdio_String_pa_struct _temp1741; _temp1741.tag=
Cyc_Stdio_String_pa; _temp1741.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
tuf->name);{ void* _temp1737=( void*)& _temp1741; void* _temp1735[ 1u]={
_temp1737}; struct _tagged_arr _temp1736={( void*) _temp1735,( void*) _temp1735,(
void*)( _temp1735 + 1u)}; _temp1734( _temp1738, _temp1739, _temp1740, _temp1736);}});
_npop_handler( 0u); return _temp1742;}{ void* _temp1743= res; _npop_handler( 0u);
return _temp1743;}}}}; _pop_region( rgn);} static void* Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* ropt, void* t){ void* rgn=( void*) Cyc_Absyn_HeapRgn; if( ropt !=
0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1765=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1765[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1766; _temp1766.tag= Cyc_Absyn_RgnHandleType; _temp1766.f1=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp1767=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1767->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp1767;})); _temp1766;}); _temp1765;}); void*
handle_type= Cyc_Tcexp_tcExp( te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)
_check_null( ropt)); void* _temp1750= Cyc_Tcutil_compress( handle_type); void*
_temp1756; _LL1752: if(( unsigned int) _temp1750 > 4u?*(( int*) _temp1750) ==
Cyc_Absyn_RgnHandleType: 0){ _LL1757: _temp1756=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1750)->f1; goto _LL1753;} else{ goto _LL1754;} _LL1754: goto _LL1755;
_LL1753: rgn= _temp1756; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1751; _LL1755:({ void(* _temp1758)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp1762=(( struct Cyc_Absyn_Exp*) _check_null( ropt))->loc; struct _tagged_arr
_temp1763= _tag_arr("expecting region_t type but found %s", sizeof(
unsigned char), 37u); struct Cyc_Stdio_String_pa_struct _temp1764; _temp1764.tag=
Cyc_Stdio_String_pa; _temp1764.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
handle_type);{ void* _temp1761=( void*)& _temp1764; void* _temp1759[ 1u]={
_temp1761}; struct _tagged_arr _temp1760={( void*) _temp1759,( void*) _temp1759,(
void*)( _temp1759 + 1u)}; _temp1758( _temp1762, _temp1763, _temp1760);}}); goto
_LL1751; _LL1751:;} Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, t);{ void* _temp1768= t; struct Cyc_Absyn_TunionFieldInfo
_temp1774; void* _temp1776; struct Cyc_Absyn_Tunionfield* _temp1778; _LL1770:
if(( unsigned int) _temp1768 > 4u?*(( int*) _temp1768) == Cyc_Absyn_TunionFieldType:
0){ _LL1775: _temp1774=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1768)->f1;
_LL1777: _temp1776=( void*) _temp1774.field_info; if(*(( int*) _temp1776) == Cyc_Absyn_KnownTunionfield){
_LL1779: _temp1778=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1776)->f2;
goto _LL1771;} else{ goto _LL1772;}} else{ goto _LL1772;} _LL1772: goto _LL1773;
_LL1771: if( _temp1778->tvs != 0){({ void(* _temp1780)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp1783= loc; struct _tagged_arr _temp1784= _tag_arr("malloc with existential types not yet implemented",
sizeof( unsigned char), 50u); void* _temp1781[ 0u]={}; struct _tagged_arr
_temp1782={( void*) _temp1781,( void*) _temp1781,( void*)( _temp1781 + 0u)};
_temp1780( _temp1783, _temp1784, _temp1782);});} goto _LL1769; _LL1773: goto
_LL1769; _LL1769:;}{ void*(* _temp1785)( void* t, void* rgn, struct Cyc_Absyn_Tqual
tq)= Cyc_Absyn_at_typ; if( topt != 0){ void* _temp1786= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo _temp1792; struct Cyc_Absyn_Conref*
_temp1794; _LL1788: if(( unsigned int) _temp1786 > 4u?*(( int*) _temp1786) ==
Cyc_Absyn_PointerType: 0){ _LL1793: _temp1792=(( struct Cyc_Absyn_PointerType_struct*)
_temp1786)->f1; _LL1795: _temp1794= _temp1792.nullable; goto _LL1789;} else{
goto _LL1790;} _LL1790: goto _LL1791; _LL1789: if((( int(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp1794)){ _temp1785= Cyc_Absyn_star_typ;} goto
_LL1787; _LL1791: goto _LL1787; _LL1787:;} return _temp1785( t, rgn, Cyc_Absyn_empty_tqual());}}
static void* Cyc_Tcexp_tcStmtExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Stmt* s){ Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
te, s), s, 1); Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));
while( 1) { void* _temp1796=( void*) s->r; struct Cyc_Absyn_Exp* _temp1806;
struct Cyc_Absyn_Stmt* _temp1808; struct Cyc_Absyn_Stmt* _temp1810; struct Cyc_Absyn_Stmt*
_temp1812; struct Cyc_Absyn_Decl* _temp1814; _LL1798: if(( unsigned int)
_temp1796 > 1u?*(( int*) _temp1796) == Cyc_Absyn_Exp_s: 0){ _LL1807: _temp1806=((
struct Cyc_Absyn_Exp_s_struct*) _temp1796)->f1; goto _LL1799;} else{ goto
_LL1800;} _LL1800: if(( unsigned int) _temp1796 > 1u?*(( int*) _temp1796) == Cyc_Absyn_Seq_s:
0){ _LL1811: _temp1810=(( struct Cyc_Absyn_Seq_s_struct*) _temp1796)->f1; goto
_LL1809; _LL1809: _temp1808=(( struct Cyc_Absyn_Seq_s_struct*) _temp1796)->f2;
goto _LL1801;} else{ goto _LL1802;} _LL1802: if(( unsigned int) _temp1796 > 1u?*((
int*) _temp1796) == Cyc_Absyn_Decl_s: 0){ _LL1815: _temp1814=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1796)->f1; goto _LL1813; _LL1813: _temp1812=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1796)->f2; goto _LL1803;} else{ goto _LL1804;} _LL1804: goto _LL1805;
_LL1799: return( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1806->topt))->v;
_LL1801: s= _temp1808; continue; _LL1803: s= _temp1812; continue; _LL1805:
return({ void*(* _temp1816)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tagged_arr msg, struct _tagged_arr ap)= Cyc_Tcexp_expr_err; struct
Cyc_Tcenv_Tenv* _temp1819= te; struct Cyc_Position_Segment* _temp1820= loc;
struct _tagged_arr _temp1821= _tag_arr("statement expression must end with expression",
sizeof( unsigned char), 46u); void* _temp1817[ 0u]={}; struct _tagged_arr
_temp1818={( void*) _temp1817,( void*) _temp1817,( void*)( _temp1817 + 0u)};
_temp1816( _temp1819, _temp1820, _temp1821, _temp1818);}); _LL1797:;}} static
void* Cyc_Tcexp_tcCodegen( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Fndecl* fd){ return({ void*(* _temp1822)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tagged_arr
msg, struct _tagged_arr ap)= Cyc_Tcexp_expr_err; struct Cyc_Tcenv_Tenv*
_temp1825= te; struct Cyc_Position_Segment* _temp1826= loc; struct _tagged_arr
_temp1827= _tag_arr("tcCodegen", sizeof( unsigned char), 10u); void* _temp1823[
0u]={}; struct _tagged_arr _temp1824={( void*) _temp1823,( void*) _temp1823,(
void*)( _temp1823 + 0u)}; _temp1822( _temp1825, _temp1826, _temp1827, _temp1824);});}
static void* Cyc_Tcexp_tcFill( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e){ return({ void*(* _temp1828)( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tagged_arr msg,
struct _tagged_arr ap)= Cyc_Tcexp_expr_err; struct Cyc_Tcenv_Tenv* _temp1831= te;
struct Cyc_Position_Segment* _temp1832= loc; struct _tagged_arr _temp1833=
_tag_arr("tcFill", sizeof( unsigned char), 7u); void* _temp1829[ 0u]={}; struct
_tagged_arr _temp1830={( void*) _temp1829,( void*) _temp1829,( void*)( _temp1829
+ 0u)}; _temp1828( _temp1831, _temp1832, _temp1833, _temp1830);});} static void*
Cyc_Tcexp_tcNew( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Exp* rgn_handle, struct Cyc_Absyn_Exp* e, struct
Cyc_Absyn_Exp* e1){ void* rgn=( void*) Cyc_Absyn_HeapRgn; if( rgn_handle != 0){
void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp1849=(
struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp1849[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp1850; _temp1850.tag=
Cyc_Absyn_RgnHandleType; _temp1850.f1=( void*) Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp1851=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1851->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp1851;})); _temp1850;}); _temp1849;}); void* handle_type= Cyc_Tcexp_tcExp(
te,( void**)& expected_type,( struct Cyc_Absyn_Exp*) _check_null( rgn_handle));
void* _temp1834= Cyc_Tcutil_compress( handle_type); void* _temp1840; _LL1836:
if(( unsigned int) _temp1834 > 4u?*(( int*) _temp1834) == Cyc_Absyn_RgnHandleType:
0){ _LL1841: _temp1840=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1834)->f1; goto _LL1837;} else{ goto _LL1838;} _LL1838: goto _LL1839;
_LL1837: rgn= _temp1840; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1835; _LL1839:({ void(* _temp1842)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp1846=(( struct Cyc_Absyn_Exp*) _check_null( rgn_handle))->loc; struct
_tagged_arr _temp1847= _tag_arr("expecting region_t type but found %s", sizeof(
unsigned char), 37u); struct Cyc_Stdio_String_pa_struct _temp1848; _temp1848.tag=
Cyc_Stdio_String_pa; _temp1848.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
handle_type);{ void* _temp1845=( void*)& _temp1848; void* _temp1843[ 1u]={
_temp1845}; struct _tagged_arr _temp1844={( void*) _temp1843,( void*) _temp1843,(
void*)( _temp1843 + 1u)}; _temp1842( _temp1846, _temp1847, _temp1844);}}); goto
_LL1835; _LL1835:;}{ void* _temp1852=( void*) e1->r; struct Cyc_Absyn_Exp*
_temp1864; struct Cyc_Absyn_Exp* _temp1866; struct Cyc_Absyn_Vardecl* _temp1868;
struct Cyc_List_List* _temp1870; struct Cyc_Core_Opt* _temp1872; struct Cyc_List_List*
_temp1874; void* _temp1876; struct _tagged_arr _temp1878; _LL1854: if(*(( int*)
_temp1852) == Cyc_Absyn_Comprehension_e){ _LL1869: _temp1868=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1852)->f1; goto _LL1867; _LL1867: _temp1866=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1852)->f2; goto _LL1865; _LL1865: _temp1864=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1852)->f3; goto _LL1855;} else{ goto _LL1856;} _LL1856: if(*(( int*)
_temp1852) == Cyc_Absyn_UnresolvedMem_e){ _LL1873: _temp1872=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1852)->f1; goto _LL1871; _LL1871: _temp1870=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1852)->f2; goto _LL1857;} else{ goto _LL1858;} _LL1858: if(*(( int*)
_temp1852) == Cyc_Absyn_Array_e){ _LL1875: _temp1874=(( struct Cyc_Absyn_Array_e_struct*)
_temp1852)->f1; goto _LL1859;} else{ goto _LL1860;} _LL1860: if(*(( int*)
_temp1852) == Cyc_Absyn_Const_e){ _LL1877: _temp1876=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1852)->f1; if(( unsigned int) _temp1876 > 1u?*(( int*) _temp1876) == Cyc_Absyn_String_c:
0){ _LL1879: _temp1878=(( struct Cyc_Absyn_String_c_struct*) _temp1876)->f1;
goto _LL1861;} else{ goto _LL1862;}} else{ goto _LL1862;} _LL1862: goto _LL1863;
_LL1855: { void* _temp1880= Cyc_Tcexp_tcExpNoPromote( te, topt, e1); void*
_temp1881= Cyc_Tcutil_compress( _temp1880); struct Cyc_Absyn_Tqual _temp1887;
void* _temp1889; _LL1883: if(( unsigned int) _temp1881 > 4u?*(( int*) _temp1881)
== Cyc_Absyn_ArrayType: 0){ _LL1890: _temp1889=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1881)->f1; goto _LL1888; _LL1888: _temp1887=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1881)->f2; goto _LL1884;} else{ goto _LL1885;} _LL1885: goto _LL1886;
_LL1884: { void* b= Cyc_Tcutil_is_const_exp( te, _temp1866)?( void*)({ struct
Cyc_Absyn_Upper_b_struct* _temp1895=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1895[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1896; _temp1896.tag= Cyc_Absyn_Upper_b; _temp1896.f1=
_temp1866; _temp1896;}); _temp1895;}):( void*) Cyc_Absyn_Unknown_b; void*
res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1892=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1892[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1893; _temp1893.tag= Cyc_Absyn_PointerType;
_temp1893.f1=({ struct Cyc_Absyn_PtrInfo _temp1894; _temp1894.elt_typ=( void*)
_temp1889; _temp1894.rgn_typ=( void*) rgn; _temp1894.nullable=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp1894.tq= _temp1887; _temp1894.bounds= Cyc_Absyn_new_conref(
b); _temp1894;}); _temp1893;}); _temp1892;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*((
void**) _check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc,
res_typ,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp1891=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1891->v=( void*) res_typ; _temp1891;}); Cyc_Tcutil_unchecked_cast( te, e,*((
void**) _check_null( topt))); res_typ=*(( void**) _check_null( topt));}} return
res_typ;} _LL1886: return({ void*(* _temp1897)( struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_impos; struct _tagged_arr _temp1900= _tag_arr("tcNew: comprehension returned non-array type",
sizeof( unsigned char), 45u); void* _temp1898[ 0u]={}; struct _tagged_arr
_temp1899={( void*) _temp1898,( void*) _temp1898,( void*)( _temp1898 + 0u)};
_temp1897( _temp1900, _temp1899);}); _LL1882:;} _LL1857:( void*)( e1->r=( void*)((
void*)({ struct Cyc_Absyn_Array_e_struct* _temp1901=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp1901[ 0]=({ struct
Cyc_Absyn_Array_e_struct _temp1902; _temp1902.tag= Cyc_Absyn_Array_e; _temp1902.f1=
_temp1870; _temp1902;}); _temp1901;}))); _temp1874= _temp1870; goto _LL1859;
_LL1859: { void** elt_typ_opt= 0; if( topt != 0){ void* _temp1903= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo _temp1909; struct Cyc_Absyn_Tqual
_temp1911; void* _temp1913; void** _temp1915; _LL1905: if(( unsigned int)
_temp1903 > 4u?*(( int*) _temp1903) == Cyc_Absyn_PointerType: 0){ _LL1910:
_temp1909=(( struct Cyc_Absyn_PointerType_struct*) _temp1903)->f1; _LL1914:
_temp1913=( void*) _temp1909.elt_typ; _temp1915=&((( struct Cyc_Absyn_PointerType_struct*)
_temp1903)->f1).elt_typ; goto _LL1912; _LL1912: _temp1911= _temp1909.tq; goto
_LL1906;} else{ goto _LL1907;} _LL1907: goto _LL1908; _LL1906: elt_typ_opt=(
void**) _temp1915; goto _LL1904; _LL1908: goto _LL1904; _LL1904:;}{ void*
_temp1916= Cyc_Tcexp_tcExpNoPromote( te, elt_typ_opt, e1); void* res_typ;{ void*
_temp1917= Cyc_Tcutil_compress( _temp1916); struct Cyc_Absyn_Exp* _temp1923;
struct Cyc_Absyn_Tqual _temp1925; void* _temp1927; _LL1919: if(( unsigned int)
_temp1917 > 4u?*(( int*) _temp1917) == Cyc_Absyn_ArrayType: 0){ _LL1928:
_temp1927=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1917)->f1; goto
_LL1926; _LL1926: _temp1925=(( struct Cyc_Absyn_ArrayType_struct*) _temp1917)->f2;
goto _LL1924; _LL1924: _temp1923=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1917)->f3; goto _LL1920;} else{ goto _LL1921;} _LL1921: goto _LL1922;
_LL1920: res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1929=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1929[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1930; _temp1930.tag=
Cyc_Absyn_PointerType; _temp1930.f1=({ struct Cyc_Absyn_PtrInfo _temp1931;
_temp1931.elt_typ=( void*) _temp1927; _temp1931.rgn_typ=( void*) rgn; _temp1931.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1931.tq=
_temp1925; _temp1931.bounds= Cyc_Absyn_new_conref(( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1932=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1932[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1933; _temp1933.tag= Cyc_Absyn_Upper_b;
_temp1933.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1923); _temp1933;});
_temp1932;})); _temp1931;}); _temp1930;}); _temp1929;}); if( topt != 0){ if( !
Cyc_Tcutil_unify(*(( void**) _check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable(
te, loc, res_typ,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp1934=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1934->v=( void*) res_typ; _temp1934;}); Cyc_Tcutil_unchecked_cast( te, e,*((
void**) _check_null( topt))); res_typ=*(( void**) _check_null( topt));}} goto
_LL1918; _LL1922: return({ void*(* _temp1935)( struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_impos; struct _tagged_arr _temp1938= _tag_arr("tcExpNoPromote on Array_e returned non-array type",
sizeof( unsigned char), 50u); void* _temp1936[ 0u]={}; struct _tagged_arr
_temp1937={( void*) _temp1936,( void*) _temp1936,( void*)( _temp1936 + 0u)};
_temp1935( _temp1938, _temp1937);}); _LL1918:;} return res_typ;}} _LL1861: {
void* _temp1939= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t, rgn, Cyc_Absyn_const_tqual(),(
void*) Cyc_Absyn_Unknown_b); void* _temp1940= Cyc_Tcexp_tcExp( te,( void**)&
_temp1939, e1); return Cyc_Absyn_atb_typ( _temp1940, rgn, Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1941=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1941[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1942; _temp1942.tag= Cyc_Absyn_Upper_b; _temp1942.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1942;}); _temp1941;}));} _LL1863: { void** topt2=
0; if( topt != 0){ void* _temp1943= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_PtrInfo _temp1951; struct Cyc_Absyn_Conref* _temp1953;
void* _temp1955; void** _temp1957; _LL1945: if(( unsigned int) _temp1943 > 4u?*((
int*) _temp1943) == Cyc_Absyn_PointerType: 0){ _LL1952: _temp1951=(( struct Cyc_Absyn_PointerType_struct*)
_temp1943)->f1; _LL1956: _temp1955=( void*) _temp1951.elt_typ; _temp1957=&(((
struct Cyc_Absyn_PointerType_struct*) _temp1943)->f1).elt_typ; goto _LL1954;
_LL1954: _temp1953= _temp1951.nullable; goto _LL1946;} else{ goto _LL1947;}
_LL1947: if(( unsigned int) _temp1943 > 4u?*(( int*) _temp1943) == Cyc_Absyn_TunionType:
0){ goto _LL1948;} else{ goto _LL1949;} _LL1949: goto _LL1950; _LL1946: topt2=(
void**) _temp1957; goto _LL1944; _LL1948: topt2=({ void** _temp1958=( void**)
GC_malloc( sizeof( void*)); _temp1958[ 0]=*(( void**) _check_null( topt));
_temp1958;}); goto _LL1944; _LL1950: goto _LL1944; _LL1944:;}{ void* _temp1959=
Cyc_Tcexp_tcExp( te, topt2, e1); void* res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1961=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1961[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1962; _temp1962.tag=
Cyc_Absyn_PointerType; _temp1962.f1=({ struct Cyc_Absyn_PtrInfo _temp1963;
_temp1963.elt_typ=( void*) _temp1959; _temp1963.rgn_typ=( void*) rgn; _temp1963.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1963.tq= Cyc_Absyn_empty_tqual();
_temp1963.bounds= Cyc_Absyn_new_conref(( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1964=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1964[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1965; _temp1965.tag= Cyc_Absyn_Upper_b;
_temp1965.f1= Cyc_Absyn_uint_exp( 1, 0); _temp1965;}); _temp1964;})); _temp1963;});
_temp1962;}); _temp1961;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*(( void**)
_check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,*((
void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1960=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1960->v=(
void*) res_typ; _temp1960;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**)
_check_null( topt))); res_typ=*(( void**) _check_null( topt));}} return res_typ;}}
_LL1853:;}} void* Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv* te, void** topt,
struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te, topt, e); t=
Cyc_Tcutil_compress( t);{ void* _temp1966= t; struct Cyc_Absyn_Exp* _temp1972;
struct Cyc_Absyn_Tqual _temp1974; void* _temp1976; _LL1968: if(( unsigned int)
_temp1966 > 4u?*(( int*) _temp1966) == Cyc_Absyn_ArrayType: 0){ _LL1977:
_temp1976=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1966)->f1; goto
_LL1975; _LL1975: _temp1974=(( struct Cyc_Absyn_ArrayType_struct*) _temp1966)->f2;
goto _LL1973; _LL1973: _temp1972=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1966)->f3; goto _LL1969;} else{ goto _LL1970;} _LL1970: goto _LL1971;
_LL1969: { void* _temp1980; struct _tuple6 _temp1978= Cyc_Tcutil_addressof_props(
te, e); _LL1981: _temp1980= _temp1978.f2; goto _LL1979; _LL1979: { void*
_temp1982= _temp1972 == 0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1983=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1983[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1984; _temp1984.tag= Cyc_Absyn_Upper_b;
_temp1984.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1972); _temp1984;});
_temp1983;}); t= Cyc_Absyn_atb_typ( _temp1976, _temp1980, _temp1974, _temp1982);(
void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) t); return t;}}
_LL1971: return t; _LL1967:;}} void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te,
topt, e); void* _temp1985=( void*) e->r; void* _temp1995; _LL1987: if(*(( int*)
_temp1985) == Cyc_Absyn_Array_e){ goto _LL1988;} else{ goto _LL1989;} _LL1989:
if(*(( int*) _temp1985) == Cyc_Absyn_Comprehension_e){ goto _LL1990;} else{ goto
_LL1991;} _LL1991: if(*(( int*) _temp1985) == Cyc_Absyn_Const_e){ _LL1996:
_temp1995=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1985)->f1; if((
unsigned int) _temp1995 > 1u?*(( int*) _temp1995) == Cyc_Absyn_String_c: 0){
goto _LL1992;} else{ goto _LL1993;}} else{ goto _LL1993;} _LL1993: goto _LL1994;
_LL1988: return t; _LL1990: return t; _LL1992: return t; _LL1994: t= Cyc_Tcutil_compress(
t);{ void* _temp1997= t; struct Cyc_Absyn_Exp* _temp2003; struct Cyc_Absyn_Tqual
_temp2005; void* _temp2007; _LL1999: if(( unsigned int) _temp1997 > 4u?*(( int*)
_temp1997) == Cyc_Absyn_ArrayType: 0){ _LL2008: _temp2007=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1997)->f1; goto _LL2006; _LL2006: _temp2005=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1997)->f2; goto _LL2004; _LL2004: _temp2003=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1997)->f3; goto _LL2000;} else{ goto _LL2001;} _LL2001: goto _LL2002;
_LL2000: { void* _temp2011; struct _tuple6 _temp2009= Cyc_Tcutil_addressof_props(
te, e); _LL2012: _temp2011= _temp2009.f2; goto _LL2010; _LL2010: { void* b=
_temp2003 == 0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp2013=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp2013[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp2014; _temp2014.tag= Cyc_Absyn_Upper_b;
_temp2014.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp2003); _temp2014;});
_temp2013;}); t= Cyc_Absyn_atb_typ( _temp2007, _temp2011, _temp2005, b); Cyc_Tcutil_unchecked_cast(
te, e, t); return t;}} _LL2002: return t; _LL1998:;} _LL1986:;} static void* Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){{ void*
_temp2015=( void*) e->r; struct Cyc_Absyn_Exp* _temp2021; _LL2017: if(*(( int*)
_temp2015) == Cyc_Absyn_NoInstantiate_e){ _LL2022: _temp2021=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp2015)->f1; goto _LL2018;} else{ goto _LL2019;} _LL2019: goto _LL2020;
_LL2018: Cyc_Tcexp_tcExpNoInst( te, topt, _temp2021);( void*)((( struct Cyc_Core_Opt*)
_check_null( _temp2021->topt))->v=( void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp2021->topt))->v))); e->topt=
_temp2021->topt; goto _LL2016; _LL2020: Cyc_Tcexp_tcExpNoInst( te, topt, e);(
void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand(
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)));{
void* _temp2023= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); struct Cyc_Absyn_PtrInfo _temp2029; struct Cyc_Absyn_Conref*
_temp2031; struct Cyc_Absyn_Tqual _temp2033; struct Cyc_Absyn_Conref* _temp2035;
void* _temp2037; void* _temp2039; _LL2025: if(( unsigned int) _temp2023 > 4u?*((
int*) _temp2023) == Cyc_Absyn_PointerType: 0){ _LL2030: _temp2029=(( struct Cyc_Absyn_PointerType_struct*)
_temp2023)->f1; _LL2040: _temp2039=( void*) _temp2029.elt_typ; goto _LL2038;
_LL2038: _temp2037=( void*) _temp2029.rgn_typ; goto _LL2036; _LL2036: _temp2035=
_temp2029.nullable; goto _LL2034; _LL2034: _temp2033= _temp2029.tq; goto _LL2032;
_LL2032: _temp2031= _temp2029.bounds; goto _LL2026;} else{ goto _LL2027;}
_LL2027: goto _LL2028; _LL2026:{ void* _temp2041= Cyc_Tcutil_compress( _temp2039);
struct Cyc_Absyn_FnInfo _temp2047; struct Cyc_List_List* _temp2049; struct Cyc_List_List*
_temp2051; struct Cyc_Absyn_VarargInfo* _temp2053; int _temp2055; struct Cyc_List_List*
_temp2057; void* _temp2059; struct Cyc_Core_Opt* _temp2061; struct Cyc_List_List*
_temp2063; _LL2043: if(( unsigned int) _temp2041 > 4u?*(( int*) _temp2041) ==
Cyc_Absyn_FnType: 0){ _LL2048: _temp2047=(( struct Cyc_Absyn_FnType_struct*)
_temp2041)->f1; _LL2064: _temp2063= _temp2047.tvars; goto _LL2062; _LL2062:
_temp2061= _temp2047.effect; goto _LL2060; _LL2060: _temp2059=( void*) _temp2047.ret_typ;
goto _LL2058; _LL2058: _temp2057= _temp2047.args; goto _LL2056; _LL2056:
_temp2055= _temp2047.c_varargs; goto _LL2054; _LL2054: _temp2053= _temp2047.cyc_varargs;
goto _LL2052; _LL2052: _temp2051= _temp2047.rgn_po; goto _LL2050; _LL2050:
_temp2049= _temp2047.attributes; goto _LL2044;} else{ goto _LL2045;} _LL2045:
goto _LL2046; _LL2044: if( _temp2063 != 0){ struct _RegionHandle _temp2065=
_new_region(); struct _RegionHandle* rgn=& _temp2065; _push_region( rgn);{
struct _tuple4 _temp2066=({ struct _tuple4 _temp2076; _temp2076.f1= Cyc_Tcenv_lookup_type_vars(
te); _temp2076.f2= rgn; _temp2076;}); struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp2066, _temp2063); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, inst); void* ftyp= Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp2073=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2073[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2074; _temp2074.tag= Cyc_Absyn_FnType; _temp2074.f1=({ struct Cyc_Absyn_FnInfo
_temp2075; _temp2075.tvars= 0; _temp2075.effect= _temp2061; _temp2075.ret_typ=(
void*) _temp2059; _temp2075.args= _temp2057; _temp2075.c_varargs= _temp2055;
_temp2075.cyc_varargs= _temp2053; _temp2075.rgn_po= _temp2051; _temp2075.attributes=
_temp2049; _temp2075;}); _temp2074;}); _temp2073;})); void* new_typ=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp2070=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp2070[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp2071; _temp2071.tag= Cyc_Absyn_PointerType;
_temp2071.f1=({ struct Cyc_Absyn_PtrInfo _temp2072; _temp2072.elt_typ=( void*)
ftyp; _temp2072.rgn_typ=( void*) _temp2037; _temp2072.nullable= _temp2035;
_temp2072.tq= _temp2033; _temp2072.bounds= _temp2031; _temp2072;}); _temp2071;});
_temp2070;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Instantiate_e_struct* _temp2067=( struct Cyc_Absyn_Instantiate_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct)); _temp2067[ 0]=({
struct Cyc_Absyn_Instantiate_e_struct _temp2068; _temp2068.tag= Cyc_Absyn_Instantiate_e;
_temp2068.f1= inner; _temp2068.f2= ts; _temp2068;}); _temp2067;}))); e->topt=({
struct Cyc_Core_Opt* _temp2069=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2069->v=( void*) new_typ; _temp2069;});}; _pop_region( rgn);}
goto _LL2042; _LL2046: goto _LL2042; _LL2042:;} goto _LL2024; _LL2028: goto
_LL2024; _LL2024:;} goto _LL2016; _LL2016:;} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v;} static void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment* loc= e->loc;
void* t;{ void* _temp2077=( void*) e->r; struct Cyc_Absyn_Exp* _temp2151; struct
_tuple1* _temp2153; struct Cyc_List_List* _temp2155; struct Cyc_Absyn_Exp*
_temp2157; struct Cyc_List_List* _temp2159; struct Cyc_Core_Opt* _temp2161; void*
_temp2163; void* _temp2165; struct _tuple1* _temp2167; struct Cyc_List_List*
_temp2169; void* _temp2171; void* _temp2173; struct Cyc_Absyn_Exp* _temp2175;
struct Cyc_Absyn_Exp* _temp2177; struct Cyc_Core_Opt* _temp2179; struct Cyc_Absyn_Exp*
_temp2181; struct Cyc_Absyn_Exp* _temp2183; struct Cyc_Absyn_Exp* _temp2185;
struct Cyc_Absyn_Exp* _temp2187; struct Cyc_Absyn_Exp* _temp2189; struct Cyc_Absyn_Exp*
_temp2191; struct Cyc_Absyn_VarargCallInfo* _temp2193; struct Cyc_Absyn_VarargCallInfo**
_temp2195; struct Cyc_List_List* _temp2196; struct Cyc_Absyn_Exp* _temp2198;
struct Cyc_Absyn_Exp* _temp2200; struct Cyc_List_List* _temp2202; struct Cyc_Absyn_Exp*
_temp2204; struct Cyc_Absyn_Exp* _temp2206; void* _temp2208; struct Cyc_Absyn_Exp*
_temp2210; struct Cyc_Absyn_Exp* _temp2212; struct Cyc_Absyn_Exp* _temp2214;
struct Cyc_Absyn_Exp* _temp2216; void* _temp2218; struct _tagged_arr* _temp2220;
void* _temp2222; struct Cyc_Absyn_Exp* _temp2224; struct _tagged_arr* _temp2226;
struct Cyc_Absyn_Exp* _temp2228; struct _tagged_arr* _temp2230; struct Cyc_Absyn_Exp*
_temp2232; struct Cyc_Absyn_Exp* _temp2234; struct Cyc_Absyn_Exp* _temp2236;
struct Cyc_List_List* _temp2238; struct Cyc_List_List* _temp2240; struct _tuple2*
_temp2242; struct Cyc_List_List* _temp2244; struct Cyc_Absyn_Stmt* _temp2246;
struct Cyc_Absyn_Fndecl* _temp2248; struct Cyc_Absyn_Exp* _temp2250; struct Cyc_Absyn_Exp*
_temp2252; struct Cyc_Absyn_Exp* _temp2254; struct Cyc_Absyn_Vardecl* _temp2256;
struct Cyc_Absyn_Structdecl* _temp2258; struct Cyc_Absyn_Structdecl** _temp2260;
struct Cyc_List_List* _temp2261; struct Cyc_Core_Opt* _temp2263; struct Cyc_Core_Opt**
_temp2265; struct _tuple1* _temp2266; struct _tuple1** _temp2268; struct Cyc_List_List*
_temp2269; void* _temp2271; struct Cyc_Absyn_Tunionfield* _temp2273; struct Cyc_Absyn_Tuniondecl*
_temp2275; struct Cyc_List_List* _temp2277; struct Cyc_Core_Opt* _temp2279;
struct Cyc_Core_Opt** _temp2281; struct Cyc_Core_Opt* _temp2282; struct Cyc_Core_Opt**
_temp2284; struct Cyc_Absyn_Enumfield* _temp2285; struct Cyc_Absyn_Enumdecl*
_temp2287; struct _tuple1* _temp2289; struct _tuple1** _temp2291; void*
_temp2292; struct Cyc_Absyn_Exp* _temp2294; _LL2079: if(*(( int*) _temp2077) ==
Cyc_Absyn_NoInstantiate_e){ _LL2152: _temp2151=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp2077)->f1; goto _LL2080;} else{ goto _LL2081;} _LL2081: if(*(( int*)
_temp2077) == Cyc_Absyn_UnknownId_e){ _LL2154: _temp2153=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp2077)->f1; goto _LL2082;} else{ goto _LL2083;} _LL2083: if(*(( int*)
_temp2077) == Cyc_Absyn_UnknownCall_e){ _LL2158: _temp2157=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp2077)->f1; goto _LL2156; _LL2156: _temp2155=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp2077)->f2; goto _LL2084;} else{ goto _LL2085;} _LL2085: if(*(( int*)
_temp2077) == Cyc_Absyn_UnresolvedMem_e){ _LL2162: _temp2161=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp2077)->f1; goto _LL2160; _LL2160: _temp2159=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp2077)->f2; goto _LL2086;} else{ goto _LL2087;} _LL2087: if(*(( int*)
_temp2077) == Cyc_Absyn_Const_e){ _LL2164: _temp2163=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp2077)->f1; goto _LL2088;} else{ goto _LL2089;} _LL2089: if(*(( int*)
_temp2077) == Cyc_Absyn_Var_e){ _LL2168: _temp2167=(( struct Cyc_Absyn_Var_e_struct*)
_temp2077)->f1; goto _LL2166; _LL2166: _temp2165=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp2077)->f2; goto _LL2090;} else{ goto _LL2091;} _LL2091: if(*(( int*)
_temp2077) == Cyc_Absyn_Primop_e){ _LL2172: _temp2171=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp2077)->f1; goto _LL2170; _LL2170: _temp2169=(( struct Cyc_Absyn_Primop_e_struct*)
_temp2077)->f2; goto _LL2092;} else{ goto _LL2093;} _LL2093: if(*(( int*)
_temp2077) == Cyc_Absyn_Increment_e){ _LL2176: _temp2175=(( struct Cyc_Absyn_Increment_e_struct*)
_temp2077)->f1; goto _LL2174; _LL2174: _temp2173=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp2077)->f2; goto _LL2094;} else{ goto _LL2095;} _LL2095: if(*(( int*)
_temp2077) == Cyc_Absyn_AssignOp_e){ _LL2182: _temp2181=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp2077)->f1; goto _LL2180; _LL2180: _temp2179=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp2077)->f2; goto _LL2178; _LL2178: _temp2177=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp2077)->f3; goto _LL2096;} else{ goto _LL2097;} _LL2097: if(*(( int*)
_temp2077) == Cyc_Absyn_Conditional_e){ _LL2188: _temp2187=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp2077)->f1; goto _LL2186; _LL2186: _temp2185=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp2077)->f2; goto _LL2184; _LL2184: _temp2183=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp2077)->f3; goto _LL2098;} else{ goto _LL2099;} _LL2099: if(*(( int*)
_temp2077) == Cyc_Absyn_SeqExp_e){ _LL2192: _temp2191=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp2077)->f1; goto _LL2190; _LL2190: _temp2189=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp2077)->f2; goto _LL2100;} else{ goto _LL2101;} _LL2101: if(*(( int*)
_temp2077) == Cyc_Absyn_FnCall_e){ _LL2199: _temp2198=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp2077)->f1; goto _LL2197; _LL2197: _temp2196=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp2077)->f2; goto _LL2194; _LL2194: _temp2193=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp2077)->f3; _temp2195=&(( struct Cyc_Absyn_FnCall_e_struct*) _temp2077)->f3;
goto _LL2102;} else{ goto _LL2103;} _LL2103: if(*(( int*) _temp2077) == Cyc_Absyn_Throw_e){
_LL2201: _temp2200=(( struct Cyc_Absyn_Throw_e_struct*) _temp2077)->f1; goto
_LL2104;} else{ goto _LL2105;} _LL2105: if(*(( int*) _temp2077) == Cyc_Absyn_Instantiate_e){
_LL2205: _temp2204=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp2077)->f1;
goto _LL2203; _LL2203: _temp2202=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp2077)->f2; goto _LL2106;} else{ goto _LL2107;} _LL2107: if(*(( int*)
_temp2077) == Cyc_Absyn_Cast_e){ _LL2209: _temp2208=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp2077)->f1; goto _LL2207; _LL2207: _temp2206=(( struct Cyc_Absyn_Cast_e_struct*)
_temp2077)->f2; goto _LL2108;} else{ goto _LL2109;} _LL2109: if(*(( int*)
_temp2077) == Cyc_Absyn_Address_e){ _LL2211: _temp2210=(( struct Cyc_Absyn_Address_e_struct*)
_temp2077)->f1; goto _LL2110;} else{ goto _LL2111;} _LL2111: if(*(( int*)
_temp2077) == Cyc_Absyn_New_e){ _LL2215: _temp2214=(( struct Cyc_Absyn_New_e_struct*)
_temp2077)->f1; goto _LL2213; _LL2213: _temp2212=(( struct Cyc_Absyn_New_e_struct*)
_temp2077)->f2; goto _LL2112;} else{ goto _LL2113;} _LL2113: if(*(( int*)
_temp2077) == Cyc_Absyn_Sizeofexp_e){ _LL2217: _temp2216=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp2077)->f1; goto _LL2114;} else{ goto _LL2115;} _LL2115: if(*(( int*)
_temp2077) == Cyc_Absyn_Sizeoftyp_e){ _LL2219: _temp2218=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp2077)->f1; goto _LL2116;} else{ goto _LL2117;} _LL2117: if(*(( int*)
_temp2077) == Cyc_Absyn_Offsetof_e){ _LL2223: _temp2222=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp2077)->f1; goto _LL2221; _LL2221: _temp2220=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp2077)->f2; goto _LL2118;} else{ goto _LL2119;} _LL2119: if(*(( int*)
_temp2077) == Cyc_Absyn_Deref_e){ _LL2225: _temp2224=(( struct Cyc_Absyn_Deref_e_struct*)
_temp2077)->f1; goto _LL2120;} else{ goto _LL2121;} _LL2121: if(*(( int*)
_temp2077) == Cyc_Absyn_StructMember_e){ _LL2229: _temp2228=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp2077)->f1; goto _LL2227; _LL2227: _temp2226=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp2077)->f2; goto _LL2122;} else{ goto _LL2123;} _LL2123: if(*(( int*)
_temp2077) == Cyc_Absyn_StructArrow_e){ _LL2233: _temp2232=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp2077)->f1; goto _LL2231; _LL2231: _temp2230=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp2077)->f2; goto _LL2124;} else{ goto _LL2125;} _LL2125: if(*(( int*)
_temp2077) == Cyc_Absyn_Subscript_e){ _LL2237: _temp2236=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp2077)->f1; goto _LL2235; _LL2235: _temp2234=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp2077)->f2; goto _LL2126;} else{ goto _LL2127;} _LL2127: if(*(( int*)
_temp2077) == Cyc_Absyn_Tuple_e){ _LL2239: _temp2238=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp2077)->f1; goto _LL2128;} else{ goto _LL2129;} _LL2129: if(*(( int*)
_temp2077) == Cyc_Absyn_CompoundLit_e){ _LL2243: _temp2242=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp2077)->f1; goto _LL2241; _LL2241: _temp2240=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp2077)->f2; goto _LL2130;} else{ goto _LL2131;} _LL2131: if(*(( int*)
_temp2077) == Cyc_Absyn_Array_e){ _LL2245: _temp2244=(( struct Cyc_Absyn_Array_e_struct*)
_temp2077)->f1; goto _LL2132;} else{ goto _LL2133;} _LL2133: if(*(( int*)
_temp2077) == Cyc_Absyn_StmtExp_e){ _LL2247: _temp2246=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp2077)->f1; goto _LL2134;} else{ goto _LL2135;} _LL2135: if(*(( int*)
_temp2077) == Cyc_Absyn_Codegen_e){ _LL2249: _temp2248=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp2077)->f1; goto _LL2136;} else{ goto _LL2137;} _LL2137: if(*(( int*)
_temp2077) == Cyc_Absyn_Fill_e){ _LL2251: _temp2250=(( struct Cyc_Absyn_Fill_e_struct*)
_temp2077)->f1; goto _LL2138;} else{ goto _LL2139;} _LL2139: if(*(( int*)
_temp2077) == Cyc_Absyn_Comprehension_e){ _LL2257: _temp2256=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2077)->f1; goto _LL2255; _LL2255: _temp2254=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2077)->f2; goto _LL2253; _LL2253: _temp2252=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2077)->f3; goto _LL2140;} else{ goto _LL2141;} _LL2141: if(*(( int*)
_temp2077) == Cyc_Absyn_Struct_e){ _LL2267: _temp2266=(( struct Cyc_Absyn_Struct_e_struct*)
_temp2077)->f1; _temp2268=&(( struct Cyc_Absyn_Struct_e_struct*) _temp2077)->f1;
goto _LL2264; _LL2264: _temp2263=(( struct Cyc_Absyn_Struct_e_struct*) _temp2077)->f2;
_temp2265=&(( struct Cyc_Absyn_Struct_e_struct*) _temp2077)->f2; goto _LL2262;
_LL2262: _temp2261=(( struct Cyc_Absyn_Struct_e_struct*) _temp2077)->f3; goto
_LL2259; _LL2259: _temp2258=(( struct Cyc_Absyn_Struct_e_struct*) _temp2077)->f4;
_temp2260=&(( struct Cyc_Absyn_Struct_e_struct*) _temp2077)->f4; goto _LL2142;}
else{ goto _LL2143;} _LL2143: if(*(( int*) _temp2077) == Cyc_Absyn_AnonStruct_e){
_LL2272: _temp2271=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp2077)->f1;
goto _LL2270; _LL2270: _temp2269=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp2077)->f2; goto _LL2144;} else{ goto _LL2145;} _LL2145: if(*(( int*)
_temp2077) == Cyc_Absyn_Tunion_e){ _LL2283: _temp2282=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp2077)->f1; _temp2284=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp2077)->f1;
goto _LL2280; _LL2280: _temp2279=(( struct Cyc_Absyn_Tunion_e_struct*) _temp2077)->f2;
_temp2281=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp2077)->f2; goto _LL2278;
_LL2278: _temp2277=(( struct Cyc_Absyn_Tunion_e_struct*) _temp2077)->f3; goto
_LL2276; _LL2276: _temp2275=(( struct Cyc_Absyn_Tunion_e_struct*) _temp2077)->f4;
goto _LL2274; _LL2274: _temp2273=(( struct Cyc_Absyn_Tunion_e_struct*) _temp2077)->f5;
goto _LL2146;} else{ goto _LL2147;} _LL2147: if(*(( int*) _temp2077) == Cyc_Absyn_Enum_e){
_LL2290: _temp2289=(( struct Cyc_Absyn_Enum_e_struct*) _temp2077)->f1; _temp2291=&((
struct Cyc_Absyn_Enum_e_struct*) _temp2077)->f1; goto _LL2288; _LL2288:
_temp2287=(( struct Cyc_Absyn_Enum_e_struct*) _temp2077)->f2; goto _LL2286;
_LL2286: _temp2285=(( struct Cyc_Absyn_Enum_e_struct*) _temp2077)->f3; goto
_LL2148;} else{ goto _LL2149;} _LL2149: if(*(( int*) _temp2077) == Cyc_Absyn_Malloc_e){
_LL2295: _temp2294=(( struct Cyc_Absyn_Malloc_e_struct*) _temp2077)->f1; goto
_LL2293; _LL2293: _temp2292=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp2077)->f2; goto _LL2150;} else{ goto _LL2078;} _LL2080: Cyc_Tcexp_tcExpNoInst(
te, 0, _temp2151); return; _LL2082: Cyc_Tcexp_resolve_unknown_id( te, e,
_temp2153); Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL2084: Cyc_Tcexp_resolve_unknown_fn(
te, e, _temp2157, _temp2155); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL2086: Cyc_Tcexp_resolve_unresolved_mem( te, loc, topt, e, _temp2159); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL2088: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp2163, e);
goto _LL2078; _LL2090: t= Cyc_Tcexp_tcVar( te, loc, _temp2167, _temp2165); goto
_LL2078; _LL2092: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp2171, _temp2169);
goto _LL2078; _LL2094: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp2175,
_temp2173); goto _LL2078; _LL2096: t= Cyc_Tcexp_tcAssignOp( te, loc, topt,
_temp2181, _temp2179, _temp2177); goto _LL2078; _LL2098: t= Cyc_Tcexp_tcConditional(
te, loc, topt, _temp2187, _temp2185, _temp2183); goto _LL2078; _LL2100: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp2191, _temp2189); goto _LL2078; _LL2102: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp2198, _temp2196, _temp2195); goto _LL2078; _LL2104: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp2200); goto _LL2078; _LL2106: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp2204, _temp2202); goto _LL2078; _LL2108: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp2208, _temp2206); goto _LL2078; _LL2110: t= Cyc_Tcexp_tcAddress(
te, loc, topt, _temp2210); goto _LL2078; _LL2112: t= Cyc_Tcexp_tcNew( te, loc,
topt, _temp2214, e, _temp2212); goto _LL2078; _LL2114: { void* _temp2296= Cyc_Tcexp_tcExpNoPromote(
te, 0, _temp2216); t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp2296); goto
_LL2078;} _LL2116: t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp2218); goto
_LL2078; _LL2118: t= Cyc_Tcexp_tcOffsetof( te, loc, topt, _temp2222, _temp2220);
goto _LL2078; _LL2120: t= Cyc_Tcexp_tcDeref( te, loc, topt, _temp2224); goto
_LL2078; _LL2122: t= Cyc_Tcexp_tcStructMember( te, loc, topt, e, _temp2228,
_temp2226); goto _LL2078; _LL2124: t= Cyc_Tcexp_tcStructArrow( te, loc, topt,
_temp2232, _temp2230); goto _LL2078; _LL2126: t= Cyc_Tcexp_tcSubscript( te, loc,
topt, _temp2236, _temp2234); goto _LL2078; _LL2128: t= Cyc_Tcexp_tcTuple( te,
loc, topt, _temp2238); goto _LL2078; _LL2130: t= Cyc_Tcexp_tcCompoundLit( te,
loc, topt, _temp2242, _temp2240); goto _LL2078; _LL2132: { void** elt_topt= 0;
if( topt != 0){ void* _temp2297= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); void* _temp2303; void** _temp2305; _LL2299: if(( unsigned int)
_temp2297 > 4u?*(( int*) _temp2297) == Cyc_Absyn_ArrayType: 0){ _LL2304:
_temp2303=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2297)->f1;
_temp2305=&(( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2297)->f1); goto
_LL2300;} else{ goto _LL2301;} _LL2301: goto _LL2302; _LL2300: elt_topt=( void**)
_temp2305; goto _LL2298; _LL2302: goto _LL2298; _LL2298:;} t= Cyc_Tcexp_tcArray(
te, loc, elt_topt, _temp2244); goto _LL2078;} _LL2134: t= Cyc_Tcexp_tcStmtExp(
te, loc, topt, _temp2246); goto _LL2078; _LL2136: t= Cyc_Tcexp_tcCodegen( te,
loc, topt, _temp2248); goto _LL2078; _LL2138: t= Cyc_Tcexp_tcFill( te, loc, topt,
_temp2250); goto _LL2078; _LL2140: t= Cyc_Tcexp_tcComprehension( te, loc, topt,
_temp2256, _temp2254, _temp2252); goto _LL2078; _LL2142: t= Cyc_Tcexp_tcStruct(
te, loc, topt, _temp2268, _temp2265, _temp2261, _temp2260); goto _LL2078;
_LL2144: t= Cyc_Tcexp_tcAnonStruct( te, loc, _temp2271, _temp2269); goto _LL2078;
_LL2146: t= Cyc_Tcexp_tcTunion( te, loc, topt, e, _temp2284, _temp2281,
_temp2277, _temp2275, _temp2273); goto _LL2078; _LL2148:* _temp2291=(( struct
Cyc_Absyn_Enumfield*) _check_null( _temp2285))->name; t=( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp2306=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp2306[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp2307; _temp2307.tag= Cyc_Absyn_EnumType;
_temp2307.f1=(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp2287))->name;
_temp2307.f2= _temp2287; _temp2307;}); _temp2306;}); goto _LL2078; _LL2150: t=
Cyc_Tcexp_tcMalloc( te, loc, topt, _temp2294, _temp2292); goto _LL2078; _LL2078:;}
e->topt=({ struct Cyc_Core_Opt* _temp2308=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp2308->v=( void*) t; _temp2308;});}
