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
ntCsl_to_ntsl( unsigned char**); extern int system( unsigned char*); struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; extern int Cyc_List_length( struct Cyc_List_List*
x); extern unsigned char Cyc_List_List_empty[ 15u]; extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_rmap_c( struct _RegionHandle*, void*(* f)( void*, void*), void* env,
struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[ 18u];
extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List*
y); extern struct Cyc_List_List* Cyc_List_rappend( struct _RegionHandle*, struct
Cyc_List_List* x, struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u];
extern void* Cyc_List_nth( struct Cyc_List_List* x, int i); extern struct Cyc_List_List*
Cyc_List_nth_tail( struct Cyc_List_List* x, int i); extern struct Cyc_List_List*
Cyc_List_zip( struct Cyc_List_List* x, struct Cyc_List_List* y); extern struct
Cyc_List_List* Cyc_List_rzip( struct _RegionHandle* r3, struct _RegionHandle* r4,
struct Cyc_List_List* x, struct Cyc_List_List* y); struct Cyc_Lineno_Pos{ struct
_tagged_arr logical_file; struct _tagged_arr line; int line_no; int col; } ;
extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment; static
const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1; static
const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct _tagged_arr
source; struct Cyc_Position_Segment* seg; void* kind; struct _tagged_arr desc; }
; extern unsigned char Cyc_Position_Nocontext[ 14u]; struct _tuple1{ void* f1;
struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual;
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
static const int Cyc_Absyn_RgnsEff= 18; struct Cyc_Absyn_RgnsEff_struct{ int tag;
void* f1; } ; static const int Cyc_Absyn_NoTypes= 0; struct Cyc_Absyn_NoTypes_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; static
const int Cyc_Absyn_WithTypes= 1; struct Cyc_Absyn_WithTypes_struct{ int tag;
struct Cyc_List_List* f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt*
f4; struct Cyc_List_List* f5; } ; static const int Cyc_Absyn_NonNullable_ps= 0;
struct Cyc_Absyn_NonNullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_Nullable_ps= 1; struct Cyc_Absyn_Nullable_ps_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_TaggedArray_ps=
0; static const int Cyc_Absyn_Printf_ft= 0; static const int Cyc_Absyn_Scanf_ft=
1; static const int Cyc_Absyn_Regparm_att= 0; struct Cyc_Absyn_Regparm_att_struct{
int tag; int f1; } ; static const int Cyc_Absyn_Stdcall_att= 0; static const int
Cyc_Absyn_Cdecl_att= 1; static const int Cyc_Absyn_Fastcall_att= 2; static const
int Cyc_Absyn_Noreturn_att= 3; static const int Cyc_Absyn_Const_att= 4; static
const int Cyc_Absyn_Aligned_att= 1; struct Cyc_Absyn_Aligned_att_struct{ int tag;
int f1; } ; static const int Cyc_Absyn_Packed_att= 5; static const int Cyc_Absyn_Section_att=
2; struct Cyc_Absyn_Section_att_struct{ int tag; struct _tagged_arr f1; } ;
static const int Cyc_Absyn_Nocommon_att= 6; static const int Cyc_Absyn_Shared_att=
7; static const int Cyc_Absyn_Unused_att= 8; static const int Cyc_Absyn_Weak_att=
9; static const int Cyc_Absyn_Dllimport_att= 10; static const int Cyc_Absyn_Dllexport_att=
11; static const int Cyc_Absyn_No_instrument_function_att= 12; static const int
Cyc_Absyn_Constructor_att= 13; static const int Cyc_Absyn_Destructor_att= 14;
static const int Cyc_Absyn_No_check_memory_usage_att= 15; static const int Cyc_Absyn_Format_att=
3; struct Cyc_Absyn_Format_att_struct{ int tag; void* f1; int f2; int f3; } ;
static const int Cyc_Absyn_Carray_mod= 0; static const int Cyc_Absyn_ConstArray_mod=
0; struct Cyc_Absyn_ConstArray_mod_struct{ int tag; struct Cyc_Absyn_Exp* f1; }
; static const int Cyc_Absyn_Pointer_mod= 1; struct Cyc_Absyn_Pointer_mod_struct{
int tag; void* f1; void* f2; struct Cyc_Absyn_Tqual f3; } ; static const int Cyc_Absyn_Function_mod=
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
Cyc_Absyn_Tqual tq; void* type; struct Cyc_Absyn_Exp* width; struct Cyc_List_List*
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
struct Cyc_Tcenv_Tenv*, struct Cyc_List_List* bound_tvars, void* k, int
allow_evars, void*); extern void Cyc_Tcutil_check_nonzero_bound( struct Cyc_Position_Segment*,
struct Cyc_Absyn_Conref*); extern void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment*,
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
_LL7: if(*(( int*) _temp5) ==  Cyc_Tcenv_VarRes){ _LL16: _temp15=( void*)((
struct Cyc_Tcenv_VarRes_struct*) _temp5)->f1; goto _LL8;} else{ goto _LL9;} _LL9:
if(*(( int*) _temp5) ==  Cyc_Tcenv_EnumRes){ _LL20: _temp19=(( struct Cyc_Tcenv_EnumRes_struct*)
_temp5)->f1; goto _LL18; _LL18: _temp17=(( struct Cyc_Tcenv_EnumRes_struct*)
_temp5)->f2; goto _LL10;} else{ goto _LL11;} _LL11: if(*(( int*) _temp5) ==  Cyc_Tcenv_TunionRes){
_LL24: _temp23=(( struct Cyc_Tcenv_TunionRes_struct*) _temp5)->f1; goto _LL22;
_LL22: _temp21=(( struct Cyc_Tcenv_TunionRes_struct*) _temp5)->f2; goto _LL12;}
else{ goto _LL13;} _LL13: if(*(( int*) _temp5) ==  Cyc_Tcenv_StructRes){ goto
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
_LL38: if( _temp36 ==  Cyc_Dict_Absent){ goto _LL39;} else{ goto _LL40;} _LL40:
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
if(*(( int*) _temp45) ==  Cyc_Absyn_UnknownId_e){ _LL52: _temp51=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp45)->f1; goto _LL48;} else{ goto _LL49;} _LL49: goto _LL50; _LL48: { struct
_handler_cons _temp53; _push_handler(& _temp53);{ int _temp55= 0; if( setjmp(
_temp53.handler)){ _temp55= 1;} if( ! _temp55){{ void* _temp56= Cyc_Tcenv_lookup_ordinary(
te, e1->loc, _temp51); void* _temp66; struct Cyc_Absyn_Tunionfield* _temp68;
struct Cyc_Absyn_Tuniondecl* _temp70; struct Cyc_Absyn_Structdecl* _temp72;
_LL58: if(*(( int*) _temp56) ==  Cyc_Tcenv_VarRes){ _LL67: _temp66=( void*)((
struct Cyc_Tcenv_VarRes_struct*) _temp56)->f1; goto _LL59;} else{ goto _LL60;}
_LL60: if(*(( int*) _temp56) ==  Cyc_Tcenv_TunionRes){ _LL71: _temp70=(( struct
Cyc_Tcenv_TunionRes_struct*) _temp56)->f1; goto _LL69; _LL69: _temp68=(( struct
Cyc_Tcenv_TunionRes_struct*) _temp56)->f2; goto _LL61;} else{ goto _LL62;} _LL62:
if(*(( int*) _temp56) ==  Cyc_Tcenv_StructRes){ _LL73: _temp72=(( struct Cyc_Tcenv_StructRes_struct*)
_temp56)->f1; goto _LL63;} else{ goto _LL64;} _LL64: if(*(( int*) _temp56) == 
Cyc_Tcenv_EnumRes){ goto _LL65;} else{ goto _LL57;} _LL59:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp74=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp74[ 0]=({ struct Cyc_Absyn_FnCall_e_struct
_temp75; _temp75.tag= Cyc_Absyn_FnCall_e; _temp75.f1= e1; _temp75.f2= es;
_temp75.f3= 0; _temp75;}); _temp74;}))); _npop_handler( 0u); return; _LL61: if(
_temp68->typs ==  0){({ struct Cyc_Stdio_String_pa_struct _temp77; _temp77.tag=
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
_temp72->name !=  0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
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
void*) _exn_thrown; void* _temp89= _temp54; _LL91: if( _temp89 ==  Cyc_Dict_Absent){
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
struct Cyc_List_List* des){ if( topt ==  0){( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Array_e_struct* _temp101=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp101[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp102; _temp102.tag= Cyc_Absyn_Array_e; _temp102.f1= des; _temp102;});
_temp101;}))); return;}{ void* t=*(( void**) _check_null( topt)); void* _temp103=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tqual _temp113; void* _temp115; struct
Cyc_Absyn_Structdecl** _temp117; struct Cyc_List_List* _temp119; struct _tuple1*
_temp121; _LL105: if(( unsigned int) _temp103 >  4u?*(( int*) _temp103) ==  Cyc_Absyn_ArrayType:
0){ _LL116: _temp115=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp103)->f1;
goto _LL114; _LL114: _temp113=(( struct Cyc_Absyn_ArrayType_struct*) _temp103)->f2;
goto _LL106;} else{ goto _LL107;} _LL107: if(( unsigned int) _temp103 >  4u?*((
int*) _temp103) ==  Cyc_Absyn_StructType: 0){ _LL122: _temp121=(( struct Cyc_Absyn_StructType_struct*)
_temp103)->f1; goto _LL120; _LL120: _temp119=(( struct Cyc_Absyn_StructType_struct*)
_temp103)->f2; goto _LL118; _LL118: _temp117=(( struct Cyc_Absyn_StructType_struct*)
_temp103)->f3; goto _LL108;} else{ goto _LL109;} _LL109: if(( unsigned int)
_temp103 >  4u?*(( int*) _temp103) ==  Cyc_Absyn_AnonStructType: 0){ goto _LL110;}
else{ goto _LL111;} _LL111: goto _LL112; _LL106:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Array_e_struct* _temp123=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp123[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp124; _temp124.tag= Cyc_Absyn_Array_e; _temp124.f1= des; _temp124;});
_temp123;}))); goto _LL104; _LL108: if( _temp117 ==  0? 1: _temp121 ==  0){({
void* _temp125[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("Compiler bug: struct type not properly set",
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
te, struct Cyc_List_List* es){ for( 0; es !=  0; es=(( struct Cyc_List_List*)
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
struct _tagged_arr _temp161; _LL137: if(( unsigned int) _temp135 >  1u?*(( int*)
_temp135) ==  Cyc_Absyn_Char_c: 0){ _LL152: _temp151=( void*)(( struct Cyc_Absyn_Char_c_struct*)
_temp135)->f1; goto _LL138;} else{ goto _LL139;} _LL139: if(( unsigned int)
_temp135 >  1u?*(( int*) _temp135) ==  Cyc_Absyn_Short_c: 0){ _LL154: _temp153=(
void*)(( struct Cyc_Absyn_Short_c_struct*) _temp135)->f1; goto _LL140;} else{
goto _LL141;} _LL141: if(( unsigned int) _temp135 >  1u?*(( int*) _temp135) == 
Cyc_Absyn_LongLong_c: 0){ _LL156: _temp155=( void*)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp135)->f1; goto _LL142;} else{ goto _LL143;} _LL143: if(( unsigned int)
_temp135 >  1u?*(( int*) _temp135) ==  Cyc_Absyn_Float_c: 0){ goto _LL144;}
else{ goto _LL145;} _LL145: if(( unsigned int) _temp135 >  1u?*(( int*) _temp135)
==  Cyc_Absyn_Int_c: 0){ _LL160: _temp159=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp135)->f1; goto _LL158; _LL158: _temp157=(( struct Cyc_Absyn_Int_c_struct*)
_temp135)->f2; goto _LL146;} else{ goto _LL147;} _LL147: if(( unsigned int)
_temp135 >  1u?*(( int*) _temp135) ==  Cyc_Absyn_String_c: 0){ _LL162: _temp161=((
struct Cyc_Absyn_String_c_struct*) _temp135)->f1; goto _LL148;} else{ goto
_LL149;} _LL149: if( _temp135 == ( void*) Cyc_Absyn_Null_c){ goto _LL150;} else{
goto _LL136;} _LL138: t= _temp151 == ( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t:
Cyc_Absyn_schar_t; goto _LL136; _LL140: t= _temp153 == ( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t; goto _LL136; _LL142: t= _temp155 == (
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ulonglong_t: Cyc_Absyn_slonglong_t; goto
_LL136; _LL144: t= Cyc_Absyn_float_t; goto _LL136; _LL146: if( topt ==  0){ t=
_temp159 == ( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;}
else{ void* _temp163= Cyc_Tcutil_compress(*(( void**) _check_null( topt))); void*
_temp173; void* _temp175; void* _temp177; void* _temp179; void* _temp181; _LL165:
if(( unsigned int) _temp163 >  4u?*(( int*) _temp163) ==  Cyc_Absyn_IntType: 0){
_LL176: _temp175=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp163)->f1;
goto _LL174; _LL174: _temp173=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f2; if( _temp173 == ( void*) Cyc_Absyn_B1){ goto _LL166;} else{ goto
_LL167;}} else{ goto _LL167;} _LL167: if(( unsigned int) _temp163 >  4u?*(( int*)
_temp163) ==  Cyc_Absyn_IntType: 0){ _LL180: _temp179=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f1; goto _LL178; _LL178: _temp177=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f2; if( _temp177 == ( void*) Cyc_Absyn_B2){ goto _LL168;} else{ goto
_LL169;}} else{ goto _LL169;} _LL169: if(( unsigned int) _temp163 >  4u?*(( int*)
_temp163) ==  Cyc_Absyn_IntType: 0){ _LL182: _temp181=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp163)->f1; goto _LL170;} else{ goto _LL171;} _LL171: goto _LL172; _LL166: t=
_temp175 == ( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t;(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp183=(
struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp183[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp184; _temp184.tag= Cyc_Absyn_Const_e;
_temp184.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp185=(
struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp185[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp186; _temp186.tag= Cyc_Absyn_Char_c;
_temp186.f1=( void*) _temp175; _temp186.f2=( unsigned char) _temp157; _temp186;});
_temp185;})); _temp184;}); _temp183;}))); goto _LL164; _LL168: t= _temp179 == (
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp187=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp187[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp188; _temp188.tag= Cyc_Absyn_Const_e; _temp188.f1=( void*)(( void*)({
struct Cyc_Absyn_Short_c_struct* _temp189=( struct Cyc_Absyn_Short_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Short_c_struct)); _temp189[ 0]=({ struct Cyc_Absyn_Short_c_struct
_temp190; _temp190.tag= Cyc_Absyn_Short_c; _temp190.f1=( void*) _temp179;
_temp190.f2=( short) _temp157; _temp190;}); _temp189;})); _temp188;}); _temp187;})));
goto _LL164; _LL170: t= _temp181 == ( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL164; _LL172: t= _temp159 == ( void*) Cyc_Absyn_Unsigned?
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
_temp193;})); if( topt !=  0){ void* _temp195= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_Absyn_Exp* _temp203; struct Cyc_Absyn_Tqual
_temp205; void* _temp207; struct Cyc_Absyn_PtrInfo _temp209; struct Cyc_Absyn_Conref*
_temp211; struct Cyc_Absyn_Tqual _temp213; struct Cyc_Absyn_Conref* _temp215;
void* _temp217; void* _temp219; _LL197: if(( unsigned int) _temp195 >  4u?*((
int*) _temp195) ==  Cyc_Absyn_ArrayType: 0){ _LL208: _temp207=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp195)->f1; goto _LL206; _LL206: _temp205=((
struct Cyc_Absyn_ArrayType_struct*) _temp195)->f2; goto _LL204; _LL204: _temp203=((
struct Cyc_Absyn_ArrayType_struct*) _temp195)->f3; goto _LL198;} else{ goto
_LL199;} _LL199: if(( unsigned int) _temp195 >  4u?*(( int*) _temp195) ==  Cyc_Absyn_PointerType:
0){ _LL210: _temp209=(( struct Cyc_Absyn_PointerType_struct*) _temp195)->f1;
_LL220: _temp219=( void*) _temp209.elt_typ; goto _LL218; _LL218: _temp217=( void*)
_temp209.rgn_typ; goto _LL216; _LL216: _temp215= _temp209.nullable; goto _LL214;
_LL214: _temp213= _temp209.tq; goto _LL212; _LL212: _temp211= _temp209.bounds;
goto _LL200;} else{ goto _LL201;} _LL201: goto _LL202; _LL198: return( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp221=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp221[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp222; _temp222.tag= Cyc_Absyn_ArrayType; _temp222.f1=(
void*) Cyc_Absyn_uchar_t; _temp222.f2= _temp205; _temp222.f3=( struct Cyc_Absyn_Exp*)
elen; _temp222;}); _temp221;}); _LL200: if( ! Cyc_Tcutil_unify(*(( void**)
_check_null( topt)), t)? Cyc_Tcutil_silent_castable( te, loc, t,*(( void**)
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
if( _temp238 == ( void*) Cyc_Absyn_Unresolved_b){ goto _LL241;} else{ goto
_LL242;} _LL242: if(( unsigned int) _temp238 >  1u?*(( int*) _temp238) ==  Cyc_Absyn_Global_b:
0){ _LL253: _temp252=(( struct Cyc_Absyn_Global_b_struct*) _temp238)->f1; goto
_LL243;} else{ goto _LL244;} _LL244: if(( unsigned int) _temp238 >  1u?*(( int*)
_temp238) ==  Cyc_Absyn_Funname_b: 0){ _LL255: _temp254=(( struct Cyc_Absyn_Funname_b_struct*)
_temp238)->f1; goto _LL245;} else{ goto _LL246;} _LL246: if(( unsigned int)
_temp238 >  1u?*(( int*) _temp238) ==  Cyc_Absyn_Pat_b: 0){ _LL257: _temp256=((
struct Cyc_Absyn_Pat_b_struct*) _temp238)->f1; goto _LL247;} else{ goto _LL248;}
_LL248: if(( unsigned int) _temp238 >  1u?*(( int*) _temp238) ==  Cyc_Absyn_Local_b:
0){ _LL259: _temp258=(( struct Cyc_Absyn_Local_b_struct*) _temp238)->f1; goto
_LL249;} else{ goto _LL250;} _LL250: if(( unsigned int) _temp238 >  1u?*(( int*)
_temp238) ==  Cyc_Absyn_Param_b: 0){ _LL261: _temp260=(( struct Cyc_Absyn_Param_b_struct*)
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
_temp264) ==  Cyc_Absyn_Const_e){ _LL273: _temp272=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp264)->f1; if(( unsigned int) _temp272 >  1u?*(( int*) _temp272) ==  Cyc_Absyn_String_c:
0){ _LL275: _temp274=(( struct Cyc_Absyn_String_c_struct*) _temp272)->f1; goto
_LL267;} else{ goto _LL268;}} else{ goto _LL268;} _LL268: if(*(( int*) _temp264)
==  Cyc_Absyn_Cast_e){ _LL277: _temp276=(( struct Cyc_Absyn_Cast_e_struct*)
_temp264)->f2; _temp278=* _temp276; _LL280: _temp279=( void*) _temp278.r; if(*((
int*) _temp279) ==  Cyc_Absyn_Const_e){ _LL282: _temp281=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp279)->f1; if(( unsigned int) _temp281 >  1u?*(( int*) _temp281) ==  Cyc_Absyn_String_c:
0){ _LL284: _temp283=(( struct Cyc_Absyn_String_c_struct*) _temp281)->f1; goto
_LL269;} else{ goto _LL270;}} else{ goto _LL270;}} else{ goto _LL270;} _LL270:
goto _LL271; _LL267: _temp283= _temp274; goto _LL269; _LL269: desc_types=
type_getter( te,( struct _tagged_arr) _temp283, fmt->loc); goto _LL265; _LL271:
return; _LL265:;} if( opt_args !=  0){ struct Cyc_List_List* _temp285=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( opt_args))->v; for( 0;
desc_types !=  0? _temp285 !=  0: 0;( desc_types=(( struct Cyc_List_List*)
_check_null( desc_types))->tl, _temp285=(( struct Cyc_List_List*) _check_null(
_temp285))->tl)){ void* t=( void*)(( struct Cyc_List_List*) _check_null(
desc_types))->hd; struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp285))->hd; if( ! Cyc_Tcutil_coerce_arg( te, e, t)){({ struct
Cyc_Stdio_String_pa_struct _temp288; _temp288.tag= Cyc_Stdio_String_pa; _temp288.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v);{ struct Cyc_Stdio_String_pa_struct _temp287;
_temp287.tag= Cyc_Stdio_String_pa; _temp287.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp286[ 2u]={& _temp287,& _temp288}; Cyc_Tcutil_terr( e->loc,
_tag_arr("descriptor has type %s but argument has type %s", sizeof(
unsigned char), 48u), _tag_arr( _temp286, sizeof( void*), 2u));}}});}} if(
desc_types !=  0){({ void* _temp289[ 0u]={}; Cyc_Tcutil_terr( fmt->loc, _tag_arr("too few arguments",
sizeof( unsigned char), 18u), _tag_arr( _temp289, sizeof( void*), 0u));});} if(
_temp285 !=  0){({ void* _temp290[ 0u]={}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp285))->hd)->loc, _tag_arr("too many arguments",
sizeof( unsigned char), 19u), _tag_arr( _temp290, sizeof( void*), 0u));});}}}
static void* Cyc_Tcexp_tcUnPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* p, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp291= p;
_LL293: if( _temp291 == ( void*) Cyc_Absyn_Plus){ goto _LL294;} else{ goto
_LL295;} _LL295: if( _temp291 == ( void*) Cyc_Absyn_Minus){ goto _LL296;} else{
goto _LL297;} _LL297: if( _temp291 == ( void*) Cyc_Absyn_Not){ goto _LL298;}
else{ goto _LL299;} _LL299: if( _temp291 == ( void*) Cyc_Absyn_Bitnot){ goto
_LL300;} else{ goto _LL301;} _LL301: if( _temp291 == ( void*) Cyc_Absyn_Size){
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
_temp319; _LL313: if(( unsigned int) _temp311 >  4u?*(( int*) _temp311) ==  Cyc_Absyn_ArrayType:
0){ goto _LL314;} else{ goto _LL315;} _LL315: if(( unsigned int) _temp311 >  4u?*((
int*) _temp311) ==  Cyc_Absyn_PointerType: 0){ _LL320: _temp319=(( struct Cyc_Absyn_PointerType_struct*)
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
_LL332: if(( unsigned int) _temp330 >  4u?*(( int*) _temp330) ==  Cyc_Absyn_PointerType:
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
_LL352: if(( unsigned int) _temp350 >  1u?*(( int*) _temp350) ==  Cyc_Absyn_Eq_constr:
0){ _LL359: _temp358=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp350)->f1;
if( _temp358 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL353;} else{ goto _LL354;}}
else{ goto _LL354;} _LL354: if(( unsigned int) _temp350 >  1u?*(( int*) _temp350)
==  Cyc_Absyn_Eq_constr: 0){ _LL361: _temp360=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp350)->f1; if(( unsigned int) _temp360 >  1u?*(( int*) _temp360) ==  Cyc_Absyn_Upper_b:
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
_LL391: _temp390= _temp378.f1; if(( unsigned int) _temp390 >  4u?*(( int*)
_temp390) ==  Cyc_Absyn_PointerType: 0){ _LL393: _temp392=(( struct Cyc_Absyn_PointerType_struct*)
_temp390)->f1; _LL395: _temp394=( void*) _temp392.elt_typ; goto _LL385;} else{
goto _LL382;} _LL385: _temp384= _temp378.f2; if(( unsigned int) _temp384 >  4u?*((
int*) _temp384) ==  Cyc_Absyn_PointerType: 0){ _LL387: _temp386=(( struct Cyc_Absyn_PointerType_struct*)
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
void* _temp400= p; _LL402: if( _temp400 == ( void*) Cyc_Absyn_Plus){ goto _LL403;}
else{ goto _LL404;} _LL404: if( _temp400 == ( void*) Cyc_Absyn_Minus){ goto
_LL405;} else{ goto _LL406;} _LL406: if( _temp400 == ( void*) Cyc_Absyn_Times){
goto _LL407;} else{ goto _LL408;} _LL408: if( _temp400 == ( void*) Cyc_Absyn_Div){
goto _LL409;} else{ goto _LL410;} _LL410: if( _temp400 == ( void*) Cyc_Absyn_Mod){
goto _LL411;} else{ goto _LL412;} _LL412: if( _temp400 == ( void*) Cyc_Absyn_Bitand){
goto _LL413;} else{ goto _LL414;} _LL414: if( _temp400 == ( void*) Cyc_Absyn_Bitor){
goto _LL415;} else{ goto _LL416;} _LL416: if( _temp400 == ( void*) Cyc_Absyn_Bitxor){
goto _LL417;} else{ goto _LL418;} _LL418: if( _temp400 == ( void*) Cyc_Absyn_Bitlshift){
goto _LL419;} else{ goto _LL420;} _LL420: if( _temp400 == ( void*) Cyc_Absyn_Bitlrshift){
goto _LL421;} else{ goto _LL422;} _LL422: if( _temp400 == ( void*) Cyc_Absyn_Bitarshift){
goto _LL423;} else{ goto _LL424;} _LL424: if( _temp400 == ( void*) Cyc_Absyn_Eq){
goto _LL425;} else{ goto _LL426;} _LL426: if( _temp400 == ( void*) Cyc_Absyn_Neq){
goto _LL427;} else{ goto _LL428;} _LL428: if( _temp400 == ( void*) Cyc_Absyn_Gt){
goto _LL429;} else{ goto _LL430;} _LL430: if( _temp400 == ( void*) Cyc_Absyn_Lt){
goto _LL431;} else{ goto _LL432;} _LL432: if( _temp400 == ( void*) Cyc_Absyn_Gte){
goto _LL433;} else{ goto _LL434;} _LL434: if( _temp400 == ( void*) Cyc_Absyn_Lte){
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
void** topt, void* p, struct Cyc_List_List* es){ if( p == ( void*) Cyc_Absyn_Minus?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es) ==  1: 0){ return Cyc_Tcexp_tcExp(
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
Cyc_Absyn_Exp* _temp503; _LL449: if(*(( int*) _temp447) ==  Cyc_Absyn_Var_e){
_LL472: _temp471=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp447)->f2; if((
unsigned int) _temp471 >  1u?*(( int*) _temp471) ==  Cyc_Absyn_Param_b: 0){
_LL474: _temp473=(( struct Cyc_Absyn_Param_b_struct*) _temp471)->f1; goto _LL450;}
else{ goto _LL451;}} else{ goto _LL451;} _LL451: if(*(( int*) _temp447) ==  Cyc_Absyn_Var_e){
_LL476: _temp475=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp447)->f2; if((
unsigned int) _temp475 >  1u?*(( int*) _temp475) ==  Cyc_Absyn_Local_b: 0){
_LL478: _temp477=(( struct Cyc_Absyn_Local_b_struct*) _temp475)->f1; goto _LL452;}
else{ goto _LL453;}} else{ goto _LL453;} _LL453: if(*(( int*) _temp447) ==  Cyc_Absyn_Var_e){
_LL480: _temp479=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp447)->f2; if((
unsigned int) _temp479 >  1u?*(( int*) _temp479) ==  Cyc_Absyn_Pat_b: 0){ _LL482:
_temp481=(( struct Cyc_Absyn_Pat_b_struct*) _temp479)->f1; goto _LL454;} else{
goto _LL455;}} else{ goto _LL455;} _LL455: if(*(( int*) _temp447) ==  Cyc_Absyn_Var_e){
_LL484: _temp483=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp447)->f2; if((
unsigned int) _temp483 >  1u?*(( int*) _temp483) ==  Cyc_Absyn_Global_b: 0){
_LL486: _temp485=(( struct Cyc_Absyn_Global_b_struct*) _temp483)->f1; goto
_LL456;} else{ goto _LL457;}} else{ goto _LL457;} _LL457: if(*(( int*) _temp447)
==  Cyc_Absyn_Subscript_e){ _LL490: _temp489=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp447)->f1; goto _LL488; _LL488: _temp487=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp447)->f2; goto _LL458;} else{ goto _LL459;} _LL459: if(*(( int*) _temp447)
==  Cyc_Absyn_StructMember_e){ _LL494: _temp493=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp447)->f1; goto _LL492; _LL492: _temp491=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp447)->f2; goto _LL460;} else{ goto _LL461;} _LL461: if(*(( int*) _temp447)
==  Cyc_Absyn_StructArrow_e){ _LL498: _temp497=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp447)->f1; goto _LL496; _LL496: _temp495=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp447)->f2; goto _LL462;} else{ goto _LL463;} _LL463: if(*(( int*) _temp447)
==  Cyc_Absyn_Deref_e){ _LL500: _temp499=(( struct Cyc_Absyn_Deref_e_struct*)
_temp447)->f1; goto _LL464;} else{ goto _LL465;} _LL465: if(*(( int*) _temp447)
==  Cyc_Absyn_NoInstantiate_e){ _LL502: _temp501=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp447)->f1; goto _LL466;} else{ goto _LL467;} _LL467: if(*(( int*) _temp447)
==  Cyc_Absyn_Instantiate_e){ _LL504: _temp503=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp447)->f1; goto _LL468;} else{ goto _LL469;} _LL469: goto _LL470; _LL450:
_temp477= _temp473; goto _LL452; _LL452: _temp481= _temp477; goto _LL454; _LL454:
_temp485= _temp481; goto _LL456; _LL456: if( !( _temp485->tq).q_const){ return;}
goto _LL448; _LL458:{ void* _temp505= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp489->topt))->v); struct Cyc_Absyn_PtrInfo _temp515; struct Cyc_Absyn_Tqual
_temp517; struct Cyc_Absyn_Tqual _temp519; struct Cyc_List_List* _temp521;
_LL507: if(( unsigned int) _temp505 >  4u?*(( int*) _temp505) ==  Cyc_Absyn_PointerType:
0){ _LL516: _temp515=(( struct Cyc_Absyn_PointerType_struct*) _temp505)->f1;
_LL518: _temp517= _temp515.tq; goto _LL508;} else{ goto _LL509;} _LL509: if((
unsigned int) _temp505 >  4u?*(( int*) _temp505) ==  Cyc_Absyn_ArrayType: 0){
_LL520: _temp519=(( struct Cyc_Absyn_ArrayType_struct*) _temp505)->f2; goto
_LL510;} else{ goto _LL511;} _LL511: if(( unsigned int) _temp505 >  4u?*(( int*)
_temp505) ==  Cyc_Absyn_TupleType: 0){ _LL522: _temp521=(( struct Cyc_Absyn_TupleType_struct*)
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
Cyc_Absyn_Uniondecl** _temp556; _LL540: if(( unsigned int) _temp538 >  4u?*((
int*) _temp538) ==  Cyc_Absyn_StructType: 0){ _LL551: _temp550=(( struct Cyc_Absyn_StructType_struct*)
_temp538)->f3; goto _LL541;} else{ goto _LL542;} _LL542: if(( unsigned int)
_temp538 >  4u?*(( int*) _temp538) ==  Cyc_Absyn_AnonUnionType: 0){ _LL553:
_temp552=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp538)->f1; goto _LL543;}
else{ goto _LL544;} _LL544: if(( unsigned int) _temp538 >  4u?*(( int*) _temp538)
==  Cyc_Absyn_AnonStructType: 0){ _LL555: _temp554=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp538)->f1; goto _LL545;} else{ goto _LL546;} _LL546: if(( unsigned int)
_temp538 >  4u?*(( int*) _temp538) ==  Cyc_Absyn_UnionType: 0){ _LL557: _temp556=((
struct Cyc_Absyn_UnionType_struct*) _temp538)->f3; goto _LL547;} else{ goto
_LL548;} _LL548: goto _LL549; _LL541: { struct Cyc_Absyn_Structfield* sf=
_temp550 ==  0? 0: Cyc_Absyn_lookup_struct_field(( struct Cyc_Absyn_Structdecl*)*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp550)), _temp491); if( sf ==  0?
1: !((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL539;} _LL543: _temp554= _temp552; goto _LL545; _LL545: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp554, _temp491); if( sf ==  0? 1: !((( struct
Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;} goto _LL539;}
_LL547: { struct Cyc_Absyn_Structfield* sf= _temp556 ==  0? 0: Cyc_Absyn_lookup_union_field((
struct Cyc_Absyn_Uniondecl*)*(( struct Cyc_Absyn_Uniondecl**) _check_null(
_temp556)), _temp491); if( sf ==  0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL539;} _LL549: goto _LL539;
_LL539:;} goto _LL448; _LL462:{ void* _temp558= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp497->topt))->v); struct Cyc_Absyn_PtrInfo
_temp564; void* _temp566; _LL560: if(( unsigned int) _temp558 >  4u?*(( int*)
_temp558) ==  Cyc_Absyn_PointerType: 0){ _LL565: _temp564=(( struct Cyc_Absyn_PointerType_struct*)
_temp558)->f1; _LL567: _temp566=( void*) _temp564.elt_typ; goto _LL561;} else{
goto _LL562;} _LL562: goto _LL563; _LL561:{ void* _temp568= Cyc_Tcutil_compress(
_temp566); struct Cyc_Absyn_Structdecl** _temp580; struct Cyc_Absyn_Uniondecl**
_temp582; struct Cyc_List_List* _temp584; struct Cyc_List_List* _temp586; _LL570:
if(( unsigned int) _temp568 >  4u?*(( int*) _temp568) ==  Cyc_Absyn_StructType:
0){ _LL581: _temp580=(( struct Cyc_Absyn_StructType_struct*) _temp568)->f3; goto
_LL571;} else{ goto _LL572;} _LL572: if(( unsigned int) _temp568 >  4u?*(( int*)
_temp568) ==  Cyc_Absyn_UnionType: 0){ _LL583: _temp582=(( struct Cyc_Absyn_UnionType_struct*)
_temp568)->f3; goto _LL573;} else{ goto _LL574;} _LL574: if(( unsigned int)
_temp568 >  4u?*(( int*) _temp568) ==  Cyc_Absyn_AnonStructType: 0){ _LL585:
_temp584=(( struct Cyc_Absyn_AnonStructType_struct*) _temp568)->f1; goto _LL575;}
else{ goto _LL576;} _LL576: if(( unsigned int) _temp568 >  4u?*(( int*) _temp568)
==  Cyc_Absyn_AnonUnionType: 0){ _LL587: _temp586=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp568)->f1; goto _LL577;} else{ goto _LL578;} _LL578: goto _LL579; _LL571: {
struct Cyc_Absyn_Structfield* sf= _temp580 ==  0? 0: Cyc_Absyn_lookup_struct_field((
struct Cyc_Absyn_Structdecl*)*(( struct Cyc_Absyn_Structdecl**) _check_null(
_temp580)), _temp495); if( sf ==  0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL569;} _LL573: { struct Cyc_Absyn_Structfield*
sf= _temp582 ==  0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp582)), _temp495); if( sf ==  0?
1: !((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL569;} _LL575: _temp586= _temp584; goto _LL577; _LL577: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp586, _temp495); if( sf ==  0? 1: !((( struct
Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;} goto _LL569;}
_LL579: goto _LL569; _LL569:;} goto _LL559; _LL563: goto _LL559; _LL559:;} goto
_LL448; _LL464:{ void* _temp588= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp499->topt))->v); struct Cyc_Absyn_PtrInfo _temp596; struct Cyc_Absyn_Tqual
_temp598; struct Cyc_Absyn_Tqual _temp600; _LL590: if(( unsigned int) _temp588 > 
4u?*(( int*) _temp588) ==  Cyc_Absyn_PointerType: 0){ _LL597: _temp596=(( struct
Cyc_Absyn_PointerType_struct*) _temp588)->f1; _LL599: _temp598= _temp596.tq;
goto _LL591;} else{ goto _LL592;} _LL592: if(( unsigned int) _temp588 >  4u?*((
int*) _temp588) ==  Cyc_Absyn_ArrayType: 0){ _LL601: _temp600=(( struct Cyc_Absyn_ArrayType_struct*)
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
Cyc_Tcutil_compress( t1); _LL616: if(( unsigned int) _temp614 >  4u?*(( int*)
_temp614) ==  Cyc_Absyn_ArrayType: 0){ goto _LL617;} else{ goto _LL618;} _LL618:
goto _LL619; _LL617:({ void* _temp620[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("cannot assign to an array",
sizeof( unsigned char), 26u), _tag_arr( _temp620, sizeof( void*), 0u));}); goto
_LL615; _LL619: goto _LL615; _LL615:;} if( ! Cyc_Absyn_is_lvalue( e1)){ return({
void* _temp621[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("assignment to non-lvalue",
sizeof( unsigned char), 25u), _tag_arr( _temp621, sizeof( void*), 0u));});} Cyc_Tcexp_check_writable(
te, e1); if( po ==  0){ if( ! Cyc_Tcutil_coerce_assign( te, e2, t1)){ return({
struct Cyc_Stdio_String_pa_struct _temp624; _temp624.tag= Cyc_Stdio_String_pa;
_temp624.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_Stdio_String_pa_struct
_temp623; _temp623.tag= Cyc_Stdio_String_pa; _temp623.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t1);{ void* _temp622[ 2u]={& _temp623,& _temp624}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("type mismatch: %s != %s", sizeof( unsigned char), 24u),
_tag_arr( _temp622, sizeof( void*), 2u));}}});}} else{ void* _temp625=( void*)((
struct Cyc_Core_Opt*) _check_null( po))->v; void* _temp626= Cyc_Tcexp_tcBinPrimop(
te, loc, 0, _temp625, e1, e2); if( !( Cyc_Tcutil_unify( _temp626, t1)? 1:( Cyc_Tcutil_coerceable(
_temp626)? Cyc_Tcutil_coerceable( t1): 0))){ return({ struct Cyc_Stdio_String_pa_struct
_temp629; _temp629.tag= Cyc_Stdio_String_pa; _temp629.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t2);{ struct Cyc_Stdio_String_pa_struct _temp628;
_temp628.tag= Cyc_Stdio_String_pa; _temp628.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp627[ 2u]={& _temp628,& _temp629}; Cyc_Tcexp_expr_err( te, loc,
_tag_arr("Cannot use this operator in an assignment when the arguments have types %s and %s",
sizeof( unsigned char), 82u), _tag_arr( _temp627, sizeof( void*), 2u));}}});}
return _temp626;} return t1;}} static void* Cyc_Tcexp_tcSeqExp( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp( te,
topt, e2); return( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v;}
static struct Cyc_Absyn_Tunionfield* Cyc_Tcexp_tcInjection( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, void* tu, struct _RegionHandle* r, struct Cyc_List_List*
inst, struct Cyc_List_List* fs){ struct Cyc_List_List* fields; void* t1=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v;{ void* _temp630= Cyc_Tcutil_compress(
t1); void* _temp640; void* _temp642; _LL632: if( _temp630 == ( void*) Cyc_Absyn_FloatType){
goto _LL633;} else{ goto _LL634;} _LL634: if(( unsigned int) _temp630 >  4u?*((
int*) _temp630) ==  Cyc_Absyn_IntType: 0){ _LL641: _temp640=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp630)->f2; if( _temp640 == ( void*) Cyc_Absyn_B1){
goto _LL635;} else{ goto _LL636;}} else{ goto _LL636;} _LL636: if(( unsigned int)
_temp630 >  4u?*(( int*) _temp630) ==  Cyc_Absyn_IntType: 0){ _LL643: _temp642=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp630)->f2; if( _temp642 == ( void*)
Cyc_Absyn_B2){ goto _LL637;} else{ goto _LL638;}} else{ goto _LL638;} _LL638:
goto _LL639; _LL633: Cyc_Tcutil_unchecked_cast( te, e,( void*) Cyc_Absyn_DoubleType);
t1=( void*) Cyc_Absyn_DoubleType; goto _LL631; _LL635: goto _LL637; _LL637: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); t1= Cyc_Absyn_sint_t; goto _LL631; _LL639: goto _LL631;
_LL631:;} for( fields= fs; fields !=  0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ void* _temp646; struct Cyc_Position_Segment*
_temp648; struct Cyc_List_List* _temp650; struct Cyc_List_List* _temp652; struct
_tuple1* _temp654; struct Cyc_Absyn_Tunionfield _temp644=*(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( fields))->hd); _LL655: _temp654= _temp644.name;
goto _LL653; _LL653: _temp652= _temp644.tvs; goto _LL651; _LL651: _temp650=
_temp644.typs; goto _LL649; _LL649: _temp648= _temp644.loc; goto _LL647; _LL647:
_temp646=( void*) _temp644.sc; goto _LL645; _LL645: if(( _temp650 ==  0? 1:((
struct Cyc_List_List*) _check_null( _temp650))->tl !=  0)? 1: _temp652 !=  0){
continue;}{ void* t2= Cyc_Tcutil_rsubstitute( r, inst,(*(( struct _tuple8*)((
struct Cyc_List_List*) _check_null( _temp650))->hd)).f2); if( Cyc_Tcutil_unify(
t1, t2)){ return( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( fields))->hd);}}} for( fields= fs; fields != 
0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){ void* _temp658;
struct Cyc_Position_Segment* _temp660; struct Cyc_List_List* _temp662; struct
Cyc_List_List* _temp664; struct _tuple1* _temp666; struct Cyc_Absyn_Tunionfield
_temp656=*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd); _LL667: _temp666= _temp656.name; goto _LL665; _LL665: _temp664=
_temp656.tvs; goto _LL663; _LL663: _temp662= _temp656.typs; goto _LL661; _LL661:
_temp660= _temp656.loc; goto _LL659; _LL659: _temp658=( void*) _temp656.sc; goto
_LL657; _LL657: if(( _temp662 ==  0? 1:(( struct Cyc_List_List*) _check_null(
_temp662))->tl !=  0)? 1: _temp664 !=  0){ continue;}{ void* t2= Cyc_Tcutil_rsubstitute(
r, inst,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp662))->hd)).f2);
if( Cyc_Tcutil_coerce_arg( te, e, t2)){ return( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( fields))->hd);}}}({
struct Cyc_Stdio_String_pa_struct _temp670; _temp670.tag= Cyc_Stdio_String_pa;
_temp670.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t1);{ struct Cyc_Stdio_String_pa_struct
_temp669; _temp669.tag= Cyc_Stdio_String_pa; _temp669.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( tu);{ void* _temp668[ 2u]={& _temp669,& _temp670}; Cyc_Tcutil_terr(
e->loc, _tag_arr("can't find a field in %s to inject a value of type %s",
sizeof( unsigned char), 54u), _tag_arr( _temp668, sizeof( void*), 2u));}}});
return 0;} static void* Cyc_Tcexp_tcFnCall( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
args, struct Cyc_Absyn_VarargCallInfo** vararg_call_info){ struct Cyc_List_List*
_temp671= args; te= Cyc_Tcenv_new_block( loc, te); Cyc_Tcexp_tcExp( te, 0, e);{
void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp672= t; struct Cyc_Absyn_PtrInfo _temp678; struct Cyc_Absyn_Conref*
_temp680; struct Cyc_Absyn_Tqual _temp682; struct Cyc_Absyn_Conref* _temp684;
void* _temp686; void* _temp688; _LL674: if(( unsigned int) _temp672 >  4u?*((
int*) _temp672) ==  Cyc_Absyn_PointerType: 0){ _LL679: _temp678=(( struct Cyc_Absyn_PointerType_struct*)
_temp672)->f1; _LL689: _temp688=( void*) _temp678.elt_typ; goto _LL687; _LL687:
_temp686=( void*) _temp678.rgn_typ; goto _LL685; _LL685: _temp684= _temp678.nullable;
goto _LL683; _LL683: _temp682= _temp678.tq; goto _LL681; _LL681: _temp680=
_temp678.bounds; goto _LL675;} else{ goto _LL676;} _LL676: goto _LL677; _LL675:
Cyc_Tcenv_check_rgn_accessible( te, loc, _temp686); Cyc_Tcutil_check_nonzero_bound(
loc, _temp680);{ void* _temp690= Cyc_Tcutil_compress( _temp688); struct Cyc_Absyn_FnInfo
_temp696; struct Cyc_List_List* _temp698; struct Cyc_List_List* _temp700; struct
Cyc_Absyn_VarargInfo* _temp702; int _temp704; struct Cyc_List_List* _temp706;
void* _temp708; struct Cyc_Core_Opt* _temp710; struct Cyc_List_List* _temp712;
_LL692: if(( unsigned int) _temp690 >  4u?*(( int*) _temp690) ==  Cyc_Absyn_FnType:
0){ _LL697: _temp696=(( struct Cyc_Absyn_FnType_struct*) _temp690)->f1; _LL713:
_temp712= _temp696.tvars; goto _LL711; _LL711: _temp710= _temp696.effect; goto
_LL709; _LL709: _temp708=( void*) _temp696.ret_typ; goto _LL707; _LL707:
_temp706= _temp696.args; goto _LL705; _LL705: _temp704= _temp696.c_varargs; goto
_LL703; _LL703: _temp702= _temp696.cyc_varargs; goto _LL701; _LL701: _temp700=
_temp696.rgn_po; goto _LL699; _LL699: _temp698= _temp696.attributes; goto _LL693;}
else{ goto _LL694;} _LL694: goto _LL695; _LL693: if( topt !=  0){ Cyc_Tcutil_unify(
_temp708,*(( void**) _check_null( topt)));} while( _temp671 !=  0? _temp706 != 
0: 0) { struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp671))->hd; void* t2=(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp706))->hd)).f3; Cyc_Tcexp_tcExp( te,( void**)& t2, e1); if( !
Cyc_Tcutil_coerce_arg( te, e1, t2)){({ struct Cyc_Stdio_String_pa_struct
_temp716; _temp716.tag= Cyc_Stdio_String_pa; _temp716.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t2);{ struct Cyc_Stdio_String_pa_struct _temp715;
_temp715.tag= Cyc_Stdio_String_pa; _temp715.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ void* _temp714[ 2u]={&
_temp715,& _temp716}; Cyc_Tcutil_terr( loc, _tag_arr("actual argument has type %s but formal has type %s",
sizeof( unsigned char), 51u), _tag_arr( _temp714, sizeof( void*), 2u));}}});}
_temp671=(( struct Cyc_List_List*) _check_null( _temp671))->tl; _temp706=((
struct Cyc_List_List*) _check_null( _temp706))->tl;} if( _temp706 !=  0){({ void*
_temp717[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("too few arguments for function",
sizeof( unsigned char), 31u), _tag_arr( _temp717, sizeof( void*), 0u));});}
else{ if(( _temp671 !=  0? 1: _temp704)? 1: _temp702 !=  0){ if( _temp704){ for(
0; _temp671 !=  0; _temp671=(( struct Cyc_List_List*) _check_null( _temp671))->tl){
Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp671))->hd);}} else{ if( _temp702 ==  0){({ void* _temp718[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("too many arguments for function", sizeof(
unsigned char), 32u), _tag_arr( _temp718, sizeof( void*), 0u));});} else{ int
_temp721; void* _temp723; void* _temp725; struct Cyc_Absyn_VarargInfo _temp719=*((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp702)); _LL726: _temp725=( void*)
_temp719.type; goto _LL724; _LL724: _temp723=( void*) _temp719.rgn; goto _LL722;
_LL722: _temp721= _temp719.inject; goto _LL720; _LL720: { struct Cyc_Absyn_VarargCallInfo*
_temp727=({ struct Cyc_Absyn_VarargCallInfo* _temp759=( struct Cyc_Absyn_VarargCallInfo*)
GC_malloc( sizeof( struct Cyc_Absyn_VarargCallInfo)); _temp759->num_varargs= 0;
_temp759->injectors= 0; _temp759->vai=( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp702); _temp759;});* vararg_call_info=( struct Cyc_Absyn_VarargCallInfo*)
_temp727; if( ! Cyc_Tcutil_unify( Cyc_Tcenv_curr_rgn( te), _temp723)){({ struct
Cyc_Stdio_String_pa_struct _temp730; _temp730.tag= Cyc_Stdio_String_pa; _temp730.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string( Cyc_Tcenv_curr_rgn( te));{ struct
Cyc_Stdio_String_pa_struct _temp729; _temp729.tag= Cyc_Stdio_String_pa; _temp729.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string( _temp723);{ void* _temp728[ 2u]={&
_temp729,& _temp730}; Cyc_Tcutil_terr( loc, _tag_arr("I have no idea how this could fail, but it did because the region for the varargs (%s) doesn't unify with the expected new_block region (%s).",
sizeof( unsigned char), 142u), _tag_arr( _temp728, sizeof( void*), 2u));}}});}
if( ! _temp721){ for( 0; _temp671 !=  0; _temp671=(( struct Cyc_List_List*)
_check_null( _temp671))->tl){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp671))->hd;(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp727))->num_varargs ++; Cyc_Tcexp_tcExp( te,( void**)& _temp725,
e1); if( ! Cyc_Tcutil_coerce_arg( te, e1, _temp725)){({ struct Cyc_Stdio_String_pa_struct
_temp733; _temp733.tag= Cyc_Stdio_String_pa; _temp733.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{
struct Cyc_Stdio_String_pa_struct _temp732; _temp732.tag= Cyc_Stdio_String_pa;
_temp732.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( _temp725);{ void*
_temp731[ 2u]={& _temp732,& _temp733}; Cyc_Tcutil_terr( loc, _tag_arr("vararg requires type %s but argument has type %s",
sizeof( unsigned char), 49u), _tag_arr( _temp731, sizeof( void*), 2u));}}});}}}
else{ void* _temp734= Cyc_Tcutil_compress( _temp725); struct Cyc_Absyn_TunionInfo
_temp740; void* _temp742; struct Cyc_List_List* _temp744; void* _temp746; struct
Cyc_Absyn_Tuniondecl* _temp748; _LL736: if(( unsigned int) _temp734 >  4u?*((
int*) _temp734) ==  Cyc_Absyn_TunionType: 0){ _LL741: _temp740=(( struct Cyc_Absyn_TunionType_struct*)
_temp734)->f1; _LL747: _temp746=( void*) _temp740.tunion_info; if(*(( int*)
_temp746) ==  Cyc_Absyn_KnownTunion){ _LL749: _temp748=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp746)->f1; goto _LL745;} else{ goto _LL738;} _LL745: _temp744= _temp740.targs;
goto _LL743; _LL743: _temp742=( void*) _temp740.rgn; goto _LL737;} else{ goto
_LL738;} _LL738: goto _LL739; _LL737: { struct Cyc_Absyn_Tuniondecl* _temp750=*
Cyc_Tcenv_lookup_tuniondecl( te, loc, _temp748->name); if( ! Cyc_Tcutil_unify(
_temp742, _temp723)){({ void* _temp751[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bad region for injected varargs",
sizeof( unsigned char), 32u), _tag_arr( _temp751, sizeof( void*), 0u));});}{
struct Cyc_List_List* fields= 0; if( _temp750->fields ==  0){({ struct Cyc_Stdio_String_pa_struct
_temp753; _temp753.tag= Cyc_Stdio_String_pa; _temp753.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( _temp725);{ void* _temp752[ 1u]={& _temp753}; Cyc_Tcutil_terr(
loc, _tag_arr("can't inject into %s", sizeof( unsigned char), 21u), _tag_arr(
_temp752, sizeof( void*), 1u));}});} else{ fields=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp750->fields))->v;}{ struct _RegionHandle
_temp754= _new_region(); struct _RegionHandle* rgn=& _temp754; _push_region( rgn);{
struct Cyc_List_List* _temp755=(( struct Cyc_List_List*(*)( struct _RegionHandle*
r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y))
Cyc_List_rzip)( rgn, rgn, _temp750->tvs, _temp744); for( 0; _temp671 !=  0;
_temp671=(( struct Cyc_List_List*) _check_null( _temp671))->tl){(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp727))->num_varargs ++;{ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp671))->hd; Cyc_Tcexp_tcExp( te, 0, e1);{
struct Cyc_Absyn_Tunionfield* _temp756= Cyc_Tcexp_tcInjection( te, e1, _temp725,
rgn, _temp755, fields); if( _temp756 !=  0){(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp727))->injectors=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)((( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp727))->injectors,({ struct Cyc_List_List* _temp757=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp757->hd=( void*)((
struct Cyc_Absyn_Tunionfield*) _check_null( _temp756)); _temp757->tl= 0;
_temp757;}));}}}}}; _pop_region( rgn);} goto _LL735;}} _LL739:({ void* _temp758[
0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bad inject vararg type", sizeof(
unsigned char), 23u), _tag_arr( _temp758, sizeof( void*), 0u));}); goto _LL735;
_LL735:;}}}}}}{ struct Cyc_List_List* a= _temp698; for( 0; a !=  0; a=(( struct
Cyc_List_List*) _check_null( a))->tl){ void* _temp760=( void*)(( struct Cyc_List_List*)
_check_null( a))->hd; int _temp766; int _temp768; void* _temp770; _LL762: if((
unsigned int) _temp760 >  16u?*(( int*) _temp760) ==  Cyc_Absyn_Format_att: 0){
_LL771: _temp770=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp760)->f1;
goto _LL769; _LL769: _temp768=(( struct Cyc_Absyn_Format_att_struct*) _temp760)->f2;
goto _LL767; _LL767: _temp766=(( struct Cyc_Absyn_Format_att_struct*) _temp760)->f3;
goto _LL763;} else{ goto _LL764;} _LL764: goto _LL765; _LL763:{ struct
_handler_cons _temp772; _push_handler(& _temp772);{ int _temp774= 0; if( setjmp(
_temp772.handler)){ _temp774= 1;} if( ! _temp774){{ struct Cyc_Absyn_Exp*
_temp775=(( struct Cyc_Absyn_Exp*(*)( struct Cyc_List_List* x, int i)) Cyc_List_nth)(
args, _temp768 -  1); struct Cyc_Core_Opt* fmt_args; if( _temp766 ==  0){
fmt_args= 0;} else{ fmt_args=({ struct Cyc_Core_Opt* _temp776=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp776->v=( void*)(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, int i)) Cyc_List_nth_tail)( args, _temp766 -  1);
_temp776;});}{ void* _temp777= _temp770; _LL779: if( _temp777 == ( void*) Cyc_Absyn_Printf_ft){
goto _LL780;} else{ goto _LL781;} _LL781: if( _temp777 == ( void*) Cyc_Absyn_Scanf_ft){
goto _LL782;} else{ goto _LL778;} _LL780: Cyc_Tcexp_check_format_args( te,
_temp775, fmt_args, Cyc_Formatstr_get_format_typs); goto _LL778; _LL782: Cyc_Tcexp_check_format_args(
te, _temp775, fmt_args, Cyc_Formatstr_get_scanf_typs); goto _LL778; _LL778:;}};
_pop_handler();} else{ void* _temp773=( void*) _exn_thrown; void* _temp784=
_temp773; _LL786: if( _temp784 ==  Cyc_List_Nth){ goto _LL787;} else{ goto
_LL788;} _LL788: goto _LL789; _LL787:({ void* _temp790[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bad format arguments", sizeof( unsigned char), 21u), _tag_arr(
_temp790, sizeof( void*), 0u));}); goto _LL785; _LL789:( void) _throw( _temp784);
_LL785:;}}} goto _LL761; _LL765: goto _LL761; _LL761:;}} Cyc_Tcenv_check_effect_accessible(
te, loc,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp710))->v); Cyc_Tcenv_check_rgn_partial_order(
te, loc, _temp700); return _temp708; _LL695: return({ void* _temp791[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("expected pointer to function", sizeof(
unsigned char), 29u), _tag_arr( _temp791, sizeof( void*), 0u));}); _LL691:;}
_LL677: return({ void* _temp792[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expected pointer to function",
sizeof( unsigned char), 29u), _tag_arr( _temp792, sizeof( void*), 0u));});
_LL673:;}} static void* Cyc_Tcexp_tcThrow( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_exn_typ,
e); if( ! Cyc_Tcutil_coerce_use( te, e, Cyc_Absyn_exn_typ)){({ struct Cyc_Stdio_String_pa_struct
_temp794; _temp794.tag= Cyc_Stdio_String_pa; _temp794.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{
void* _temp793[ 1u]={& _temp794}; Cyc_Tcutil_terr( loc, _tag_arr("expected xtunion exn but found %s",
sizeof( unsigned char), 34u), _tag_arr( _temp793, sizeof( void*), 1u));}});}
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp795=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp795->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp795;}));} static void* Cyc_Tcexp_tcInstantiate( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* ts){ Cyc_Tcexp_tcExpNoInst( te, 0, e);( void*)((( struct
Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v));{ void* t1= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp796= t1;
struct Cyc_Absyn_PtrInfo _temp802; struct Cyc_Absyn_Conref* _temp804; struct Cyc_Absyn_Tqual
_temp806; struct Cyc_Absyn_Conref* _temp808; void* _temp810; void* _temp812;
_LL798: if(( unsigned int) _temp796 >  4u?*(( int*) _temp796) ==  Cyc_Absyn_PointerType:
0){ _LL803: _temp802=(( struct Cyc_Absyn_PointerType_struct*) _temp796)->f1;
_LL813: _temp812=( void*) _temp802.elt_typ; goto _LL811; _LL811: _temp810=( void*)
_temp802.rgn_typ; goto _LL809; _LL809: _temp808= _temp802.nullable; goto _LL807;
_LL807: _temp806= _temp802.tq; goto _LL805; _LL805: _temp804= _temp802.bounds;
goto _LL799;} else{ goto _LL800;} _LL800: goto _LL801; _LL799:{ void* _temp814=
Cyc_Tcutil_compress( _temp812); struct Cyc_Absyn_FnInfo _temp820; struct Cyc_List_List*
_temp822; struct Cyc_List_List* _temp824; struct Cyc_Absyn_VarargInfo* _temp826;
int _temp828; struct Cyc_List_List* _temp830; void* _temp832; struct Cyc_Core_Opt*
_temp834; struct Cyc_List_List* _temp836; _LL816: if(( unsigned int) _temp814 > 
4u?*(( int*) _temp814) ==  Cyc_Absyn_FnType: 0){ _LL821: _temp820=(( struct Cyc_Absyn_FnType_struct*)
_temp814)->f1; _LL837: _temp836= _temp820.tvars; goto _LL835; _LL835: _temp834=
_temp820.effect; goto _LL833; _LL833: _temp832=( void*) _temp820.ret_typ; goto
_LL831; _LL831: _temp830= _temp820.args; goto _LL829; _LL829: _temp828= _temp820.c_varargs;
goto _LL827; _LL827: _temp826= _temp820.cyc_varargs; goto _LL825; _LL825:
_temp824= _temp820.rgn_po; goto _LL823; _LL823: _temp822= _temp820.attributes;
goto _LL817;} else{ goto _LL818;} _LL818: goto _LL819; _LL817: { struct Cyc_List_List*
instantiation= 0; if( Cyc_List_length( ts) != (( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp836)){ return({ void* _temp838[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("type instantiation mismatch", sizeof( unsigned char), 28u),
_tag_arr( _temp838, sizeof( void*), 0u));});} for( 0; ts !=  0;( ts=(( struct
Cyc_List_List*) _check_null( ts))->tl, _temp836=(( struct Cyc_List_List*)
_check_null( _temp836))->tl)){ void* k= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp836))->hd)->kind); Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te), k, 1,( void*)(( struct Cyc_List_List*)
_check_null( ts))->hd); instantiation=({ struct Cyc_List_List* _temp839=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp839->hd=( void*)({
struct _tuple5* _temp840=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp840->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp836))->hd; _temp840->f2=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd;
_temp840;}); _temp839->tl= instantiation; _temp839;});}{ void* new_fn_typ= Cyc_Tcutil_substitute(
instantiation,( void*)({ struct Cyc_Absyn_FnType_struct* _temp844=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp844[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp845; _temp845.tag= Cyc_Absyn_FnType; _temp845.f1=({ struct Cyc_Absyn_FnInfo
_temp846; _temp846.tvars= 0; _temp846.effect= _temp834; _temp846.ret_typ=( void*)
_temp832; _temp846.args= _temp830; _temp846.c_varargs= _temp828; _temp846.cyc_varargs=
_temp826; _temp846.rgn_po= _temp824; _temp846.attributes= _temp822; _temp846;});
_temp845;}); _temp844;})); void* new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp841=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp841[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp842; _temp842.tag= Cyc_Absyn_PointerType;
_temp842.f1=({ struct Cyc_Absyn_PtrInfo _temp843; _temp843.elt_typ=( void*)
new_fn_typ; _temp843.rgn_typ=( void*) _temp810; _temp843.nullable= _temp808;
_temp843.tq= _temp806; _temp843.bounds= _temp804; _temp843;}); _temp842;});
_temp841;}); return new_typ;}} _LL819: goto _LL815; _LL815:;} goto _LL797;
_LL801: goto _LL797; _LL797:;} return({ struct Cyc_Stdio_String_pa_struct
_temp848; _temp848.tag= Cyc_Stdio_String_pa; _temp848.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t1);{ void* _temp847[ 1u]={& _temp848}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("expecting polymorphic type but found %s", sizeof(
unsigned char), 40u), _tag_arr( _temp847, sizeof( void*), 1u));}});}} static
void* Cyc_Tcexp_tcCast( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* t, struct Cyc_Absyn_Exp* e){ Cyc_Tcutil_check_type( loc,
te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, 1, t); Cyc_Tcexp_tcExp(
te, 0, e);{ void* t2=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v;
if( ! Cyc_Tcutil_silent_castable( te, loc, t2, t)? ! Cyc_Tcutil_castable( te,
loc, t2, t): 0){ return({ struct Cyc_Stdio_String_pa_struct _temp851; _temp851.tag=
Cyc_Stdio_String_pa; _temp851.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
struct Cyc_Stdio_String_pa_struct _temp850; _temp850.tag= Cyc_Stdio_String_pa;
_temp850.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ void* _temp849[
2u]={& _temp850,& _temp851}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("cannot cast %s to %s",
sizeof( unsigned char), 21u), _tag_arr( _temp849, sizeof( void*), 2u));}}});}
return t;}} static void* Cyc_Tcexp_tcAddress( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){ void**
_temp852= 0; struct Cyc_Absyn_Tqual _temp853= Cyc_Absyn_empty_tqual(); if( topt
!=  0){ void* _temp854= Cyc_Tcutil_compress(*(( void**) _check_null( topt)));
struct Cyc_Absyn_PtrInfo _temp860; struct Cyc_Absyn_Tqual _temp862; void*
_temp864; _LL856: if(( unsigned int) _temp854 >  4u?*(( int*) _temp854) ==  Cyc_Absyn_PointerType:
0){ _LL861: _temp860=(( struct Cyc_Absyn_PointerType_struct*) _temp854)->f1;
_LL865: _temp864=( void*) _temp860.elt_typ; goto _LL863; _LL863: _temp862=
_temp860.tq; goto _LL857;} else{ goto _LL858;} _LL858: goto _LL859; _LL857:
_temp852=({ void** _temp866=( void**) GC_malloc( sizeof( void*)); _temp866[ 0]=
_temp864; _temp866;}); _temp853= _temp862; goto _LL855; _LL859: goto _LL855;
_LL855:;} Cyc_Tcexp_tcExpNoInst( te, _temp852, e);{ void* _temp867=( void*) e->r;
_LL869: if(*(( int*) _temp867) ==  Cyc_Absyn_Struct_e){ goto _LL870;} else{ goto
_LL871;} _LL871: if(*(( int*) _temp867) ==  Cyc_Absyn_Tuple_e){ goto _LL872;}
else{ goto _LL873;} _LL873: goto _LL874; _LL870: goto _LL872; _LL872:({ void*
_temp875[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("& used to allocate", sizeof(
unsigned char), 19u), _tag_arr( _temp875, sizeof( void*), 0u));});{ void*(*
_temp876)( void* t, void* rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; if(
topt !=  0){ void* _temp877= Cyc_Tcutil_compress(*(( void**) _check_null( topt)));
struct Cyc_Absyn_PtrInfo _temp883; struct Cyc_Absyn_Conref* _temp885; _LL879:
if(( unsigned int) _temp877 >  4u?*(( int*) _temp877) ==  Cyc_Absyn_PointerType:
0){ _LL884: _temp883=(( struct Cyc_Absyn_PointerType_struct*) _temp877)->f1;
_LL886: _temp885= _temp883.nullable; goto _LL880;} else{ goto _LL881;} _LL881:
goto _LL882; _LL880: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp885)){ _temp876= Cyc_Absyn_star_typ;} goto _LL878; _LL882: goto _LL878;
_LL878:;} return _temp876(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v,(
void*) Cyc_Absyn_HeapRgn, _temp853);} _LL874: goto _LL868; _LL868:;}{ void*
_temp889; int _temp891; struct _tuple6 _temp887= Cyc_Tcutil_addressof_props( te,
e); _LL892: _temp891= _temp887.f1; goto _LL890; _LL890: _temp889= _temp887.f2;
goto _LL888; _LL888: { struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual(); if(
_temp891){ tq.q_const= 1;}{ void* t= Cyc_Absyn_at_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, _temp889, tq); return t;}}}} static void* Cyc_Tcexp_tcSizeof(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind,
1, t); if( ! Cyc_Evexp_okay_szofarg( t)){({ struct Cyc_Stdio_String_pa_struct
_temp894; _temp894.tag= Cyc_Stdio_String_pa; _temp894.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp893[ 1u]={& _temp894}; Cyc_Tcutil_terr(
loc, _tag_arr("sizeof applied to type %s, which has unknown size here", sizeof(
unsigned char), 55u), _tag_arr( _temp893, sizeof( void*), 1u));}});} return Cyc_Absyn_uint_t;}
static void* Cyc_Tcexp_tcOffsetof( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* t, struct _tagged_arr* n){ Cyc_Tcutil_check_type( loc,
te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, 1, t);{ int
bad_type= 1;{ void* _temp895= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp903; _LL897: if(( unsigned int) _temp895 >  4u?*(( int*) _temp895) ==  Cyc_Absyn_StructType:
0){ _LL904: _temp903=(( struct Cyc_Absyn_StructType_struct*) _temp895)->f3; goto
_LL898;} else{ goto _LL899;} _LL899: if(( unsigned int) _temp895 >  4u?*(( int*)
_temp895) ==  Cyc_Absyn_AnonStructType: 0){ goto _LL900;} else{ goto _LL901;}
_LL901: goto _LL902; _LL898: if( _temp903 ==  0){ return({ void* _temp905[ 0u]={};
Cyc_Tcutil_impos( _tag_arr("offsetof on unchecked StructType", sizeof(
unsigned char), 33u), _tag_arr( _temp905, sizeof( void*), 0u));});} if((*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp903)))->fields ==  0){ goto
_LL896;} goto _LL900; _LL900: bad_type= 0; goto _LL896; _LL902: goto _LL896;
_LL896:;} if( bad_type){({ struct Cyc_Stdio_String_pa_struct _temp907; _temp907.tag=
Cyc_Stdio_String_pa; _temp907.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp906[ 1u]={& _temp907}; Cyc_Tcutil_terr( loc, _tag_arr("%s is not a known struct type",
sizeof( unsigned char), 30u), _tag_arr( _temp906, sizeof( void*), 1u));}});}
return Cyc_Absyn_uint_t;}} static void* Cyc_Tcexp_tcDeref( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); void* _temp908= t; struct Cyc_Absyn_PtrInfo _temp914;
struct Cyc_Absyn_Conref* _temp916; void* _temp918; void* _temp920; _LL910: if((
unsigned int) _temp908 >  4u?*(( int*) _temp908) ==  Cyc_Absyn_PointerType: 0){
_LL915: _temp914=(( struct Cyc_Absyn_PointerType_struct*) _temp908)->f1; _LL921:
_temp920=( void*) _temp914.elt_typ; goto _LL919; _LL919: _temp918=( void*)
_temp914.rgn_typ; goto _LL917; _LL917: _temp916= _temp914.bounds; goto _LL911;}
else{ goto _LL912;} _LL912: goto _LL913; _LL911: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp918); Cyc_Tcutil_check_nonzero_bound( loc, _temp916); return
_temp920; _LL913: return({ struct Cyc_Stdio_String_pa_struct _temp923; _temp923.tag=
Cyc_Stdio_String_pa; _temp923.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp922[ 1u]={& _temp923}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expecting * or @ type but found %s",
sizeof( unsigned char), 35u), _tag_arr( _temp922, sizeof( void*), 1u));}});
_LL909:;}} static void* Cyc_Tcexp_tcStructMember( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* outer_e,
struct Cyc_Absyn_Exp* e, struct _tagged_arr* f){ Cyc_Tcexp_tcExpNoPromote( te, 0,
e);{ void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v); void* _temp924= t; struct Cyc_Absyn_Structdecl** _temp940; struct
Cyc_Absyn_Structdecl* _temp942; struct Cyc_List_List* _temp943; struct _tuple1*
_temp945; struct Cyc_List_List* _temp947; struct Cyc_List_List* _temp949; struct
Cyc_Absyn_Uniondecl** _temp951; struct Cyc_Absyn_Uniondecl* _temp953; struct Cyc_List_List*
_temp954; struct _tuple1* _temp956; _LL926: if(( unsigned int) _temp924 >  4u?*((
int*) _temp924) ==  Cyc_Absyn_StructType: 0){ _LL946: _temp945=(( struct Cyc_Absyn_StructType_struct*)
_temp924)->f1; goto _LL944; _LL944: _temp943=(( struct Cyc_Absyn_StructType_struct*)
_temp924)->f2; goto _LL941; _LL941: _temp940=(( struct Cyc_Absyn_StructType_struct*)
_temp924)->f3; if( _temp940 ==  0){ goto _LL928;} else{ _temp942=* _temp940;
goto _LL927;}} else{ goto _LL928;} _LL928: if(( unsigned int) _temp924 >  4u?*((
int*) _temp924) ==  Cyc_Absyn_AnonStructType: 0){ _LL948: _temp947=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp924)->f1; goto _LL929;} else{ goto _LL930;} _LL930: if(( unsigned int)
_temp924 >  4u?*(( int*) _temp924) ==  Cyc_Absyn_AnonUnionType: 0){ _LL950:
_temp949=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp924)->f1; goto _LL931;}
else{ goto _LL932;} _LL932: if(( unsigned int) _temp924 >  4u?*(( int*) _temp924)
==  Cyc_Absyn_UnionType: 0){ _LL957: _temp956=(( struct Cyc_Absyn_UnionType_struct*)
_temp924)->f1; goto _LL955; _LL955: _temp954=(( struct Cyc_Absyn_UnionType_struct*)
_temp924)->f2; goto _LL952; _LL952: _temp951=(( struct Cyc_Absyn_UnionType_struct*)
_temp924)->f3; if( _temp951 ==  0){ goto _LL934;} else{ _temp953=* _temp951;
goto _LL933;}} else{ goto _LL934;} _LL934: if(( unsigned int) _temp924 >  4u?*((
int*) _temp924) ==  Cyc_Absyn_ArrayType: 0){ goto _LL958;} else{ goto _LL936;}
_LL958: if( Cyc_String_zstrcmp(* f, _tag_arr("size", sizeof( unsigned char), 5u))
==  0){ goto _LL935;} else{ goto _LL936;} _LL936: if(( unsigned int) _temp924 > 
4u?*(( int*) _temp924) ==  Cyc_Absyn_PointerType: 0){ goto _LL959;} else{ goto
_LL938;} _LL959: if( Cyc_String_zstrcmp(* f, _tag_arr("size", sizeof(
unsigned char), 5u)) ==  0){ goto _LL937;} else{ goto _LL938;} _LL938: goto
_LL939; _LL927: if( _temp945 ==  0){ return({ void* _temp960[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("unresolved struct type (COMPILER ERROR)", sizeof(
unsigned char), 40u), _tag_arr( _temp960, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Structfield*
_temp961= Cyc_Absyn_lookup_struct_field( _temp942, f); if( _temp961 ==  0){
return({ struct Cyc_Stdio_String_pa_struct _temp964; _temp964.tag= Cyc_Stdio_String_pa;
_temp964.f1=( struct _tagged_arr)* f;{ struct Cyc_Stdio_String_pa_struct
_temp963; _temp963.tag= Cyc_Stdio_String_pa; _temp963.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp945));{ void*
_temp962[ 2u]={& _temp963,& _temp964}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("struct %s has no %s field",
sizeof( unsigned char), 26u), _tag_arr( _temp962, sizeof( void*), 2u));}}});}{
void* t2;{ struct _RegionHandle _temp965= _new_region(); struct _RegionHandle*
rgn=& _temp965; _push_region( rgn);{ struct Cyc_List_List* _temp966=(( struct
Cyc_List_List*(*)( struct _RegionHandle* r3, struct _RegionHandle* r4, struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp942->tvs,
_temp943); t2= Cyc_Tcutil_rsubstitute( rgn, _temp966,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp961))->type);}; _pop_region( rgn);} return t2;}} _LL929: {
struct Cyc_Absyn_Structfield* _temp967= Cyc_Absyn_lookup_field( _temp947, f);
if( _temp967 ==  0){ return({ struct Cyc_Stdio_String_pa_struct _temp969;
_temp969.tag= Cyc_Stdio_String_pa; _temp969.f1=( struct _tagged_arr)* f;{ void*
_temp968[ 1u]={& _temp969}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("struct has no %s field",
sizeof( unsigned char), 23u), _tag_arr( _temp968, sizeof( void*), 1u));}});}
return( void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp967))->type;}
_LL931: { struct Cyc_Absyn_Structfield* _temp970= Cyc_Absyn_lookup_field(
_temp949, f); if( _temp970 ==  0){ return({ struct Cyc_Stdio_String_pa_struct
_temp972; _temp972.tag= Cyc_Stdio_String_pa; _temp972.f1=( struct _tagged_arr)*
f;{ void* _temp971[ 1u]={& _temp972}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("union has no %s field",
sizeof( unsigned char), 22u), _tag_arr( _temp971, sizeof( void*), 1u));}});}
return( void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp970))->type;}
_LL933: if( _temp956 ==  0){ return({ void* _temp973[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("unresolved union type (COMPILER ERROR)", sizeof(
unsigned char), 39u), _tag_arr( _temp973, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Structfield*
_temp974= Cyc_Absyn_lookup_union_field( _temp953, f); if( _temp974 ==  0){
return({ struct Cyc_Stdio_String_pa_struct _temp977; _temp977.tag= Cyc_Stdio_String_pa;
_temp977.f1=( struct _tagged_arr)* f;{ struct Cyc_Stdio_String_pa_struct
_temp976; _temp976.tag= Cyc_Stdio_String_pa; _temp976.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp956));{ void*
_temp975[ 2u]={& _temp976,& _temp977}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("union %s has no %s field",
sizeof( unsigned char), 25u), _tag_arr( _temp975, sizeof( void*), 2u));}}});}{
struct Cyc_List_List* _temp978=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp953->tvs, _temp954); void*
_temp979= Cyc_Tcutil_substitute( _temp978,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp974))->type); return _temp979;}} _LL935:( void*)( outer_e->r=(
void*)(( void*)({ struct Cyc_Absyn_Primop_e_struct* _temp980=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp980[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp981; _temp981.tag= Cyc_Absyn_Primop_e; _temp981.f1=(
void*)(( void*) Cyc_Absyn_Size); _temp981.f2=({ struct Cyc_List_List* _temp982=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp982->hd=(
void*) e; _temp982->tl= 0; _temp982;}); _temp981;}); _temp980;}))); return Cyc_Absyn_uint_t;
_LL937:( void*)( outer_e->r=( void*)(( void*)({ struct Cyc_Absyn_Primop_e_struct*
_temp983=( struct Cyc_Absyn_Primop_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct));
_temp983[ 0]=({ struct Cyc_Absyn_Primop_e_struct _temp984; _temp984.tag= Cyc_Absyn_Primop_e;
_temp984.f1=( void*)(( void*) Cyc_Absyn_Size); _temp984.f2=({ struct Cyc_List_List*
_temp985=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp985->hd=( void*) e; _temp985->tl= 0; _temp985;}); _temp984;}); _temp983;})));
return Cyc_Absyn_uint_t; _LL939: if( Cyc_String_zstrcmp(* f, _tag_arr("size",
sizeof( unsigned char), 5u)) ==  0){ return({ struct Cyc_Stdio_String_pa_struct
_temp987; _temp987.tag= Cyc_Stdio_String_pa; _temp987.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp986[ 1u]={& _temp987}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("expecting struct, union or array, found %s", sizeof(
unsigned char), 43u), _tag_arr( _temp986, sizeof( void*), 1u));}});} else{
return({ struct Cyc_Stdio_String_pa_struct _temp989; _temp989.tag= Cyc_Stdio_String_pa;
_temp989.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp988[ 1u]={&
_temp989}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expecting struct or union, found %s",
sizeof( unsigned char), 36u), _tag_arr( _temp988, sizeof( void*), 1u));}});}
_LL925:;}} static void* Cyc_Tcexp_tcStructArrow( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct
_tagged_arr* f){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp990= t;
struct Cyc_Absyn_PtrInfo _temp996; struct Cyc_Absyn_Conref* _temp998; void*
_temp1000; void* _temp1002; _LL992: if(( unsigned int) _temp990 >  4u?*(( int*)
_temp990) ==  Cyc_Absyn_PointerType: 0){ _LL997: _temp996=(( struct Cyc_Absyn_PointerType_struct*)
_temp990)->f1; _LL1003: _temp1002=( void*) _temp996.elt_typ; goto _LL1001;
_LL1001: _temp1000=( void*) _temp996.rgn_typ; goto _LL999; _LL999: _temp998=
_temp996.bounds; goto _LL993;} else{ goto _LL994;} _LL994: goto _LL995; _LL993:
Cyc_Tcutil_check_nonzero_bound( loc, _temp998);{ void* _temp1004= Cyc_Tcutil_compress(
_temp1002); struct Cyc_Absyn_Structdecl** _temp1016; struct Cyc_Absyn_Structdecl*
_temp1018; struct Cyc_List_List* _temp1019; struct _tuple1* _temp1021; struct
Cyc_Absyn_Uniondecl** _temp1023; struct Cyc_Absyn_Uniondecl* _temp1025; struct
Cyc_List_List* _temp1026; struct _tuple1* _temp1028; struct Cyc_List_List*
_temp1030; struct Cyc_List_List* _temp1032; _LL1006: if(( unsigned int)
_temp1004 >  4u?*(( int*) _temp1004) ==  Cyc_Absyn_StructType: 0){ _LL1022:
_temp1021=(( struct Cyc_Absyn_StructType_struct*) _temp1004)->f1; goto _LL1020;
_LL1020: _temp1019=(( struct Cyc_Absyn_StructType_struct*) _temp1004)->f2; goto
_LL1017; _LL1017: _temp1016=(( struct Cyc_Absyn_StructType_struct*) _temp1004)->f3;
if( _temp1016 ==  0){ goto _LL1008;} else{ _temp1018=* _temp1016; goto _LL1007;}}
else{ goto _LL1008;} _LL1008: if(( unsigned int) _temp1004 >  4u?*(( int*)
_temp1004) ==  Cyc_Absyn_UnionType: 0){ _LL1029: _temp1028=(( struct Cyc_Absyn_UnionType_struct*)
_temp1004)->f1; goto _LL1027; _LL1027: _temp1026=(( struct Cyc_Absyn_UnionType_struct*)
_temp1004)->f2; goto _LL1024; _LL1024: _temp1023=(( struct Cyc_Absyn_UnionType_struct*)
_temp1004)->f3; if( _temp1023 ==  0){ goto _LL1010;} else{ _temp1025=* _temp1023;
goto _LL1009;}} else{ goto _LL1010;} _LL1010: if(( unsigned int) _temp1004 >  4u?*((
int*) _temp1004) ==  Cyc_Absyn_AnonStructType: 0){ _LL1031: _temp1030=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1004)->f1; goto _LL1011;} else{ goto
_LL1012;} _LL1012: if(( unsigned int) _temp1004 >  4u?*(( int*) _temp1004) == 
Cyc_Absyn_AnonUnionType: 0){ _LL1033: _temp1032=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp1004)->f1; goto _LL1013;} else{ goto _LL1014;} _LL1014: goto _LL1015;
_LL1007: { struct Cyc_Absyn_Structfield* _temp1034= Cyc_Absyn_lookup_struct_field(
_temp1018, f); if( _temp1034 ==  0){ return({ struct Cyc_Stdio_String_pa_struct
_temp1037; _temp1037.tag= Cyc_Stdio_String_pa; _temp1037.f1=( struct _tagged_arr)*
f;{ struct Cyc_Stdio_String_pa_struct _temp1036; _temp1036.tag= Cyc_Stdio_String_pa;
_temp1036.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(( struct _tuple1*)
_check_null( _temp1021));{ void* _temp1035[ 2u]={& _temp1036,& _temp1037}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("struct %s has no %s field", sizeof( unsigned char), 26u),
_tag_arr( _temp1035, sizeof( void*), 2u));}}});}{ void* t3;{ struct
_RegionHandle _temp1038= _new_region(); struct _RegionHandle* rgn=& _temp1038;
_push_region( rgn);{ struct Cyc_List_List* _temp1039=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp1018->tvs, _temp1019);
t3= Cyc_Tcutil_rsubstitute( rgn, _temp1039,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1034))->type);}; _pop_region( rgn);} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1000); return t3;}} _LL1009: { struct Cyc_Absyn_Structfield*
_temp1040= Cyc_Absyn_lookup_union_field( _temp1025, f); if( _temp1040 ==  0){
return({ struct Cyc_Stdio_String_pa_struct _temp1043; _temp1043.tag= Cyc_Stdio_String_pa;
_temp1043.f1=( struct _tagged_arr)* f;{ struct Cyc_Stdio_String_pa_struct
_temp1042; _temp1042.tag= Cyc_Stdio_String_pa; _temp1042.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp1028));{ void*
_temp1041[ 2u]={& _temp1042,& _temp1043}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("union %s has no %s field",
sizeof( unsigned char), 25u), _tag_arr( _temp1041, sizeof( void*), 2u));}}});}{
struct Cyc_List_List* _temp1044=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp1025->tvs, _temp1026); void*
_temp1045= Cyc_Tcutil_substitute( _temp1044,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1040))->type); Cyc_Tcenv_check_rgn_accessible( te, loc,
_temp1000); return _temp1045;}} _LL1011: { struct Cyc_Absyn_Structfield*
_temp1046= Cyc_Absyn_lookup_field( _temp1030, f); if( _temp1046 ==  0){ return({
struct Cyc_Stdio_String_pa_struct _temp1048; _temp1048.tag= Cyc_Stdio_String_pa;
_temp1048.f1=( struct _tagged_arr)* f;{ void* _temp1047[ 1u]={& _temp1048}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("struct has no %s field", sizeof( unsigned char), 23u),
_tag_arr( _temp1047, sizeof( void*), 1u));}});} return( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1046))->type;} _LL1013: { struct Cyc_Absyn_Structfield*
_temp1049= Cyc_Absyn_lookup_field( _temp1032, f); if( _temp1049 ==  0){ return({
struct Cyc_Stdio_String_pa_struct _temp1051; _temp1051.tag= Cyc_Stdio_String_pa;
_temp1051.f1=( struct _tagged_arr)* f;{ void* _temp1050[ 1u]={& _temp1051}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("union has no %s field", sizeof( unsigned char), 22u),
_tag_arr( _temp1050, sizeof( void*), 1u));}});} return( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1049))->type;} _LL1015: return({ struct Cyc_Stdio_String_pa_struct
_temp1053; _temp1053.tag= Cyc_Stdio_String_pa; _temp1053.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp1052[ 1u]={& _temp1053}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("expecting struct pointer, found %s", sizeof( unsigned char),
35u), _tag_arr( _temp1052, sizeof( void*), 1u));}}); _LL1005:;} _LL995: return({
struct Cyc_Stdio_String_pa_struct _temp1055; _temp1055.tag= Cyc_Stdio_String_pa;
_temp1055.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1054[
1u]={& _temp1055}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expecting struct pointer, found %s",
sizeof( unsigned char), 35u), _tag_arr( _temp1054, sizeof( void*), 1u));}});
_LL991:;}} static void* Cyc_Tcexp_ithTupleType( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct Cyc_List_List* ts, struct Cyc_Absyn_Exp*
index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp( index); struct
_handler_cons _temp1056; _push_handler(& _temp1056);{ int _temp1058= 0; if(
setjmp( _temp1056.handler)){ _temp1058= 1;} if( ! _temp1058){{ void* _temp1059=(*((
struct _tuple8*(*)( struct Cyc_List_List* x, int i)) Cyc_List_nth)( ts,( int) i)).f2;
_npop_handler( 0u); return _temp1059;}; _pop_handler();} else{ void* _temp1057=(
void*) _exn_thrown; void* _temp1061= _temp1057; _LL1063: if( _temp1061 ==  Cyc_List_Nth){
goto _LL1064;} else{ goto _LL1065;} _LL1065: goto _LL1066; _LL1064: return({
struct Cyc_Stdio_Int_pa_struct _temp1069; _temp1069.tag= Cyc_Stdio_Int_pa;
_temp1069.f1=( int)(( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
ts));{ struct Cyc_Stdio_Int_pa_struct _temp1068; _temp1068.tag= Cyc_Stdio_Int_pa;
_temp1068.f1=( int) i;{ void* _temp1067[ 2u]={& _temp1068,& _temp1069}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("index is %d but tuple has only %d fields", sizeof(
unsigned char), 41u), _tag_arr( _temp1067, sizeof( void*), 2u));}}}); _LL1066:(
void) _throw( _temp1061); _LL1062:;}}} static void* Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp(
te, 0, e2);{ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ return({
struct Cyc_Stdio_String_pa_struct _temp1071; _temp1071.tag= Cyc_Stdio_String_pa;
_temp1071.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ void* _temp1070[
1u]={& _temp1071}; Cyc_Tcexp_expr_err( te, e2->loc, _tag_arr("expecting int subscript, found %s",
sizeof( unsigned char), 34u), _tag_arr( _temp1070, sizeof( void*), 1u));}});}{
void* _temp1072= t1; struct Cyc_Absyn_PtrInfo _temp1080; struct Cyc_Absyn_Conref*
_temp1082; struct Cyc_Absyn_Tqual _temp1084; void* _temp1086; void* _temp1088;
struct Cyc_List_List* _temp1090; _LL1074: if(( unsigned int) _temp1072 >  4u?*((
int*) _temp1072) ==  Cyc_Absyn_PointerType: 0){ _LL1081: _temp1080=(( struct Cyc_Absyn_PointerType_struct*)
_temp1072)->f1; _LL1089: _temp1088=( void*) _temp1080.elt_typ; goto _LL1087;
_LL1087: _temp1086=( void*) _temp1080.rgn_typ; goto _LL1085; _LL1085: _temp1084=
_temp1080.tq; goto _LL1083; _LL1083: _temp1082= _temp1080.bounds; goto _LL1075;}
else{ goto _LL1076;} _LL1076: if(( unsigned int) _temp1072 >  4u?*(( int*)
_temp1072) ==  Cyc_Absyn_TupleType: 0){ _LL1091: _temp1090=(( struct Cyc_Absyn_TupleType_struct*)
_temp1072)->f1; goto _LL1077;} else{ goto _LL1078;} _LL1078: goto _LL1079;
_LL1075: if( Cyc_Tcutil_is_const_exp( te, e2)){ Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp(
e2), _temp1082);} else{{ void* _temp1092=( void*)( Cyc_Absyn_compress_conref(
_temp1082))->v; void* _temp1098; struct Cyc_Absyn_Exp* _temp1100; struct Cyc_Absyn_Exp
_temp1102; void* _temp1103; void* _temp1105; int _temp1107; void* _temp1109;
_LL1094: if(( unsigned int) _temp1092 >  1u?*(( int*) _temp1092) ==  Cyc_Absyn_Eq_constr:
0){ _LL1099: _temp1098=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1092)->f1;
if(( unsigned int) _temp1098 >  1u?*(( int*) _temp1098) ==  Cyc_Absyn_Upper_b: 0){
_LL1101: _temp1100=(( struct Cyc_Absyn_Upper_b_struct*) _temp1098)->f1;
_temp1102=* _temp1100; _LL1104: _temp1103=( void*) _temp1102.r; if(*(( int*)
_temp1103) ==  Cyc_Absyn_Const_e){ _LL1106: _temp1105=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1103)->f1; if(( unsigned int) _temp1105 >  1u?*(( int*) _temp1105) ==  Cyc_Absyn_Int_c:
0){ _LL1110: _temp1109=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1105)->f1;
if( _temp1109 == ( void*) Cyc_Absyn_Unsigned){ goto _LL1108;} else{ goto _LL1096;}
_LL1108: _temp1107=(( struct Cyc_Absyn_Int_c_struct*) _temp1105)->f2; if(
_temp1107 ==  1){ goto _LL1095;} else{ goto _LL1096;}} else{ goto _LL1096;}}
else{ goto _LL1096;}} else{ goto _LL1096;}} else{ goto _LL1096;} _LL1096: goto
_LL1097; _LL1095:({ void* _temp1111[ 0u]={}; Cyc_Tcutil_warn( e1->loc, _tag_arr("subscript applied to pointer to one object",
sizeof( unsigned char), 43u), _tag_arr( _temp1111, sizeof( void*), 0u));}); goto
_LL1093; _LL1097: goto _LL1093; _LL1093:;} Cyc_Tcutil_check_nonzero_bound( loc,
_temp1082);} Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1086); return
_temp1088; _LL1077: return Cyc_Tcexp_ithTupleType( te, loc, _temp1090, e2);
_LL1079: return({ struct Cyc_Stdio_String_pa_struct _temp1113; _temp1113.tag=
Cyc_Stdio_String_pa; _temp1113.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp1112[ 1u]={& _temp1113}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("subscript applied to %s",
sizeof( unsigned char), 24u), _tag_arr( _temp1112, sizeof( void*), 1u));}});
_LL1073:;}}} static void* Cyc_Tcexp_tcTuple( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_List_List* es){ int done= 0;
struct Cyc_List_List* fields= 0; if( topt !=  0){ void* _temp1114= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_List_List* _temp1120; _LL1116: if((
unsigned int) _temp1114 >  4u?*(( int*) _temp1114) ==  Cyc_Absyn_TupleType: 0){
_LL1121: _temp1120=(( struct Cyc_Absyn_TupleType_struct*) _temp1114)->f1; goto
_LL1117;} else{ goto _LL1118;} _LL1118: goto _LL1119; _LL1117: if((( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp1120) != (( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es)){({ void* _temp1122[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("tuple expression has the wrong number of fields", sizeof(
unsigned char), 48u), _tag_arr( _temp1122, sizeof( void*), 0u));}); goto _LL1115;}
for( 0; es !=  0;( es=(( struct Cyc_List_List*) _check_null( es))->tl, _temp1120=((
struct Cyc_List_List*) _check_null( _temp1120))->tl)){ void* _temp1123=(*((
struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp1120))->hd)).f2; Cyc_Tcexp_tcExpInitializer(
te,( void**)& _temp1123,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd); fields=({ struct Cyc_List_List* _temp1124=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1124->hd=( void*)({ struct
_tuple8* _temp1125=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8));
_temp1125->f1=(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null(
_temp1120))->hd)).f1; _temp1125->f2=( void*)(( struct Cyc_Core_Opt*) _check_null(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v;
_temp1125;}); _temp1124->tl= fields; _temp1124;});} done= 1; goto _LL1115;
_LL1119: goto _LL1115; _LL1115:;} if( ! done){ for( 0; es !=  0; es=(( struct
Cyc_List_List*) _check_null( es))->tl){ Cyc_Tcexp_tcExpInitializer( te, 0,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd); fields=({
struct Cyc_List_List* _temp1126=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1126->hd=( void*)({ struct _tuple8* _temp1127=(
struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp1127->f1= Cyc_Absyn_empty_tqual();
_temp1127->f2=( void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd)->topt))->v; _temp1127;});
_temp1126->tl= fields; _temp1126;});}} return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp1128=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp1128[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp1129; _temp1129.tag= Cyc_Absyn_TupleType;
_temp1129.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp1129;}); _temp1128;});} static void* Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
_tuple2* t, struct Cyc_List_List* des){ return({ void* _temp1130[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("tcCompoundLit", sizeof( unsigned char), 14u), _tag_arr(
_temp1130, sizeof( void*), 0u));});} static void* Cyc_Tcexp_tcArray( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** elt_topt, struct Cyc_List_List* des){
struct Cyc_List_List* es=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)(
struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)(
struct _tuple7*)) Cyc_Core_snd, des); void* res= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcexp_mk,({ struct Cyc_Core_Opt* _temp1153=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1153->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp1153;})); struct Cyc_Absyn_Const_e_struct* _temp1131=({ struct Cyc_Absyn_Const_e_struct*
_temp1149=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp1149[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp1150; _temp1150.tag= Cyc_Absyn_Const_e;
_temp1150.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp1151=(
struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp1151[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp1152; _temp1152.tag= Cyc_Absyn_Int_c;
_temp1152.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp1152.f2=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( es); _temp1152;}); _temp1151;}));
_temp1150;}); _temp1149;}); struct Cyc_Absyn_Exp* sz_exp= Cyc_Absyn_new_exp((
void*) _temp1131, loc); sz_exp->topt=({ struct Cyc_Core_Opt* _temp1132=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1132->v=( void*)
Cyc_Absyn_uint_t; _temp1132;});{ void* res_t2=( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1147=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1147[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1148; _temp1148.tag= Cyc_Absyn_ArrayType;
_temp1148.f1=( void*) res; _temp1148.f2= Cyc_Absyn_empty_tqual(); _temp1148.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp1148;}); _temp1147;});{ struct Cyc_List_List*
es2= es; for( 0; es2 !=  0; es2=(( struct Cyc_List_List*) _check_null( es2))->tl){
Cyc_Tcexp_tcExpInitializer( te, elt_topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es2))->hd);}} if( ! Cyc_Tcutil_coerce_list( te, res, es)){({ struct
Cyc_Stdio_String_pa_struct _temp1134; _temp1134.tag= Cyc_Stdio_String_pa;
_temp1134.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( res);{ void*
_temp1133[ 1u]={& _temp1134}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd)->loc, _tag_arr("elements of array do not all have the same type (%s)",
sizeof( unsigned char), 53u), _tag_arr( _temp1133, sizeof( void*), 1u));}});}{
int offset= 0; for( 0; des !=  0;( offset ++, des=(( struct Cyc_List_List*)
_check_null( des))->tl)){ struct Cyc_List_List* ds=(*(( struct _tuple7*)((
struct Cyc_List_List*) _check_null( des))->hd)).f1; if( ds !=  0){ void*
_temp1135=( void*)(( struct Cyc_List_List*) _check_null( ds))->hd; struct Cyc_Absyn_Exp*
_temp1141; _LL1137: if(*(( int*) _temp1135) ==  Cyc_Absyn_FieldName){ goto
_LL1138;} else{ goto _LL1139;} _LL1139: if(*(( int*) _temp1135) ==  Cyc_Absyn_ArrayElement){
_LL1142: _temp1141=(( struct Cyc_Absyn_ArrayElement_struct*) _temp1135)->f1;
goto _LL1140;} else{ goto _LL1136;} _LL1138:({ void* _temp1143[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("only array index designators are supported", sizeof(
unsigned char), 43u), _tag_arr( _temp1143, sizeof( void*), 0u));}); goto _LL1136;
_LL1140: Cyc_Tcexp_tcExpInitializer( te, 0, _temp1141);{ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1141); if( i !=  offset){({ struct Cyc_Stdio_Int_pa_struct _temp1146;
_temp1146.tag= Cyc_Stdio_Int_pa; _temp1146.f1=( int) i;{ struct Cyc_Stdio_Int_pa_struct
_temp1145; _temp1145.tag= Cyc_Stdio_Int_pa; _temp1145.f1=( int)(( unsigned int)
offset);{ void* _temp1144[ 2u]={& _temp1145,& _temp1146}; Cyc_Tcutil_terr(
_temp1141->loc, _tag_arr("expecting index designator %d but found %d", sizeof(
unsigned char), 43u), _tag_arr( _temp1144, sizeof( void*), 2u));}}});} goto
_LL1136;} _LL1136:;}}} return res_t2;}} static void* Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* bound, struct Cyc_Absyn_Exp* body){
Cyc_Tcexp_tcExp( te, 0, bound); if( ! Cyc_Tcutil_coerce_uint_typ( te, bound)){({
struct Cyc_Stdio_String_pa_struct _temp1155; _temp1155.tag= Cyc_Stdio_String_pa;
_temp1155.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( bound->topt))->v);{ void* _temp1154[ 1u]={& _temp1155}; Cyc_Tcutil_terr(
bound->loc, _tag_arr("expecting unsigned int, found %s", sizeof( unsigned char),
33u), _tag_arr( _temp1154, sizeof( void*), 1u));}});} if( !( vd->tq).q_const){({
void* _temp1156[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("comprehension index variable is not declared const!",
sizeof( unsigned char), 52u), _tag_arr( _temp1156, sizeof( void*), 0u));});} if(
te->le !=  0){ te= Cyc_Tcenv_new_block( loc, te); te= Cyc_Tcenv_add_local_var(
loc, te, vd);} else{ if( ! Cyc_Tcutil_is_const_exp( te, bound)){({ void*
_temp1157[ 0u]={}; Cyc_Tcutil_terr( bound->loc, _tag_arr("bound is not constant",
sizeof( unsigned char), 22u), _tag_arr( _temp1157, sizeof( void*), 0u));});} if(
! Cyc_Tcutil_is_const_exp( te, body)){({ void* _temp1158[ 0u]={}; Cyc_Tcutil_terr(
bound->loc, _tag_arr("body is not constant", sizeof( unsigned char), 21u),
_tag_arr( _temp1158, sizeof( void*), 0u));});}}{ struct Cyc_Absyn_PtrInfo pinfo;
void** _temp1159= 0; struct Cyc_Absyn_Tqual* _temp1160= 0; if( topt !=  0){ void*
_temp1161= Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo
_temp1169; struct Cyc_Absyn_Exp* _temp1171; struct Cyc_Absyn_Tqual _temp1173;
struct Cyc_Absyn_Tqual* _temp1175; void* _temp1176; void** _temp1178; _LL1163:
if(( unsigned int) _temp1161 >  4u?*(( int*) _temp1161) ==  Cyc_Absyn_PointerType:
0){ _LL1170: _temp1169=(( struct Cyc_Absyn_PointerType_struct*) _temp1161)->f1;
goto _LL1164;} else{ goto _LL1165;} _LL1165: if(( unsigned int) _temp1161 >  4u?*((
int*) _temp1161) ==  Cyc_Absyn_ArrayType: 0){ _LL1177: _temp1176=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1161)->f1; _temp1178=&(( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1161)->f1); goto _LL1174; _LL1174:
_temp1173=(( struct Cyc_Absyn_ArrayType_struct*) _temp1161)->f2; _temp1175=&((
struct Cyc_Absyn_ArrayType_struct*) _temp1161)->f2; goto _LL1172; _LL1172:
_temp1171=(( struct Cyc_Absyn_ArrayType_struct*) _temp1161)->f3; goto _LL1166;}
else{ goto _LL1167;} _LL1167: goto _LL1168; _LL1164: pinfo= _temp1169; _temp1159=(
void**)(( void**)(( void*)& pinfo.elt_typ)); _temp1160=( struct Cyc_Absyn_Tqual*)&
pinfo.tq; goto _LL1162; _LL1166: _temp1159=( void**) _temp1178; _temp1160=(
struct Cyc_Absyn_Tqual*) _temp1175; goto _LL1162; _LL1168: goto _LL1162; _LL1162:;}{
void* t= Cyc_Tcexp_tcExp( te, _temp1159, body); return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1179=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1179[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1180; _temp1180.tag= Cyc_Absyn_ArrayType;
_temp1180.f1=( void*) t; _temp1180.f2= _temp1160 ==  0? Cyc_Absyn_empty_tqual():*((
struct Cyc_Absyn_Tqual*) _check_null( _temp1160)); _temp1180.f3=( struct Cyc_Absyn_Exp*)
bound; _temp1180;}); _temp1179;});}}} struct _tuple9{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void* Cyc_Tcexp_tcStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct _tuple1** tn, struct
Cyc_Core_Opt** otyps, struct Cyc_List_List* args, struct Cyc_Absyn_Structdecl**
sd_opt){ struct Cyc_Absyn_Structdecl* sd; if(* sd_opt !=  0){ sd=( struct Cyc_Absyn_Structdecl*)
_check_null(* sd_opt);} else{{ struct _handler_cons _temp1181; _push_handler(&
_temp1181);{ int _temp1183= 0; if( setjmp( _temp1181.handler)){ _temp1183= 1;}
if( ! _temp1183){ sd=* Cyc_Tcenv_lookup_structdecl( te, loc,* tn);; _pop_handler();}
else{ void* _temp1182=( void*) _exn_thrown; void* _temp1185= _temp1182; _LL1187:
if( _temp1185 ==  Cyc_Dict_Absent){ goto _LL1188;} else{ goto _LL1189;} _LL1189:
goto _LL1190; _LL1188:({ struct Cyc_Stdio_String_pa_struct _temp1192; _temp1192.tag=
Cyc_Stdio_String_pa; _temp1192.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(*
tn);{ void* _temp1191[ 1u]={& _temp1192}; Cyc_Tcutil_terr( loc, _tag_arr("unbound struct name %s",
sizeof( unsigned char), 23u), _tag_arr( _temp1191, sizeof( void*), 1u));}});
return topt !=  0?*(( void**) _check_null( topt)):( void*) Cyc_Absyn_VoidType;
_LL1190:( void) _throw( _temp1185); _LL1186:;}}}* sd_opt=( struct Cyc_Absyn_Structdecl*)
sd; if( sd->name !=  0){* tn=( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v;}}{ struct _RegionHandle _temp1193= _new_region();
struct _RegionHandle* rgn=& _temp1193; _push_region( rgn);{ struct _tuple4
_temp1194=({ struct _tuple4 _temp1218; _temp1218.f1= Cyc_Tcenv_lookup_type_vars(
te); _temp1218.f2= rgn; _temp1218;}); struct Cyc_List_List* _temp1195=(( struct
Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*,
struct Cyc_Absyn_Tvar*), struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)(
rgn, Cyc_Tcutil_r_make_inst_var,& _temp1194, sd->tvs); struct Cyc_List_List*
_temp1196=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple5*), struct Cyc_List_List*
x)) Cyc_List_map)(( void*(*)( struct _tuple5*)) Cyc_Core_snd, _temp1195); struct
Cyc_Absyn_StructType_struct* _temp1197=({ struct Cyc_Absyn_StructType_struct*
_temp1215=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp1215[ 0]=({ struct Cyc_Absyn_StructType_struct _temp1216; _temp1216.tag=
Cyc_Absyn_StructType; _temp1216.f1=( struct _tuple1*)* tn; _temp1216.f2=
_temp1196; _temp1216.f3=({ struct Cyc_Absyn_Structdecl** _temp1217=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp1217[ 0]= sd; _temp1217;});
_temp1216;}); _temp1215;}); if( topt !=  0){ Cyc_Tcutil_unify(( void*) _temp1197,*((
void**) _check_null( topt)));}* otyps=({ struct Cyc_Core_Opt* _temp1198=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1198->v=( void*)
_temp1196; _temp1198;}); if( sd->fields ==  0){({ void* _temp1199[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("can't build abstract struct", sizeof( unsigned char), 28u),
_tag_arr( _temp1199, sizeof( void*), 0u));});{ void* _temp1200=( void*)
_temp1197; _npop_handler( 0u); return _temp1200;}}{ struct Cyc_List_List* fields=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd->fields))->v);
for( 0; fields !=  0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){
struct _tuple9 _temp1203; struct Cyc_Absyn_Exp* _temp1204; struct Cyc_Absyn_Structfield*
_temp1206; struct _tuple9* _temp1201=( struct _tuple9*)(( struct Cyc_List_List*)
_check_null( fields))->hd; _temp1203=* _temp1201; _LL1207: _temp1206= _temp1203.f1;
goto _LL1205; _LL1205: _temp1204= _temp1203.f2; goto _LL1202; _LL1202: { void*
_temp1208= Cyc_Tcutil_rsubstitute( rgn, _temp1195,( void*) _temp1206->type); Cyc_Tcexp_tcExpInitializer(
te,( void**)& _temp1208, _temp1204); if( ! Cyc_Tcutil_coerce_arg( te, _temp1204,
_temp1208)){({ struct Cyc_Stdio_String_pa_struct _temp1213; _temp1213.tag= Cyc_Stdio_String_pa;
_temp1213.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1204->topt))->v);{ struct Cyc_Stdio_String_pa_struct _temp1212;
_temp1212.tag= Cyc_Stdio_String_pa; _temp1212.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp1208);{ struct Cyc_Stdio_String_pa_struct _temp1211; _temp1211.tag= Cyc_Stdio_String_pa;
_temp1211.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(* tn);{ struct Cyc_Stdio_String_pa_struct
_temp1210; _temp1210.tag= Cyc_Stdio_String_pa; _temp1210.f1=( struct _tagged_arr)*
_temp1206->name;{ void* _temp1209[ 4u]={& _temp1210,& _temp1211,& _temp1212,&
_temp1213}; Cyc_Tcutil_terr( _temp1204->loc, _tag_arr("field %s of struct %s expects type %s != %s",
sizeof( unsigned char), 44u), _tag_arr( _temp1209, sizeof( void*), 4u));}}}}});}}}{
void* _temp1214=( void*) _temp1197; _npop_handler( 0u); return _temp1214;}}};
_pop_region( rgn);}} static void* Cyc_Tcexp_tcAnonStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void* ts, struct Cyc_List_List* args){{
struct _RegionHandle _temp1219= _new_region(); struct _RegionHandle* rgn=&
_temp1219; _push_region( rgn);{ void* _temp1220= Cyc_Tcutil_compress( ts);
struct Cyc_List_List* _temp1226; _LL1222: if(( unsigned int) _temp1220 >  4u?*((
int*) _temp1220) ==  Cyc_Absyn_AnonStructType: 0){ _LL1227: _temp1226=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1220)->f1; goto _LL1223;} else{ goto
_LL1224;} _LL1224: goto _LL1225; _LL1223: { struct Cyc_List_List* fields=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args, _temp1226); for( 0; fields !=  0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ struct _tuple9 _temp1230; struct Cyc_Absyn_Exp*
_temp1231; struct Cyc_Absyn_Structfield* _temp1233; struct _tuple9* _temp1228=(
struct _tuple9*)(( struct Cyc_List_List*) _check_null( fields))->hd; _temp1230=*
_temp1228; _LL1234: _temp1233= _temp1230.f1; goto _LL1232; _LL1232: _temp1231=
_temp1230.f2; goto _LL1229; _LL1229: Cyc_Tcexp_tcExpInitializer( te,( void**)((
void**)(( void*)& _temp1233->type)), _temp1231); if( ! Cyc_Tcutil_coerce_arg( te,
_temp1231,( void*) _temp1233->type)){({ struct Cyc_Stdio_String_pa_struct
_temp1238; _temp1238.tag= Cyc_Stdio_String_pa; _temp1238.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1231->topt))->v);{
struct Cyc_Stdio_String_pa_struct _temp1237; _temp1237.tag= Cyc_Stdio_String_pa;
_temp1237.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*) _temp1233->type);{
struct Cyc_Stdio_String_pa_struct _temp1236; _temp1236.tag= Cyc_Stdio_String_pa;
_temp1236.f1=( struct _tagged_arr)* _temp1233->name;{ void* _temp1235[ 3u]={&
_temp1236,& _temp1237,& _temp1238}; Cyc_Tcutil_terr( _temp1231->loc, _tag_arr("field %s of struct expects type %s != %s",
sizeof( unsigned char), 41u), _tag_arr( _temp1235, sizeof( void*), 3u));}}}});}}
goto _LL1221;} _LL1225:({ void* _temp1239[ 0u]={};(( int(*)( struct _tagged_arr
fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("tcAnonStruct:  not an AnonStructType",
sizeof( unsigned char), 37u), _tag_arr( _temp1239, sizeof( void*), 0u));}); goto
_LL1221; _LL1221:;}; _pop_region( rgn);} return ts;} static void* Cyc_Tcexp_tcTunion(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, struct Cyc_Core_Opt** all_ref, struct Cyc_Core_Opt** exist_ref,
struct Cyc_List_List* es, struct Cyc_Absyn_Tuniondecl* tud, struct Cyc_Absyn_Tunionfield*
tuf){ struct _RegionHandle _temp1240= _new_region(); struct _RegionHandle* rgn=&
_temp1240; _push_region( rgn);{ struct _tuple4 _temp1241=({ struct _tuple4
_temp1286; _temp1286.f1= Cyc_Tcenv_lookup_type_vars( te); _temp1286.f2= rgn;
_temp1286;}); struct Cyc_List_List* _temp1242=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1241, tud->tvs); struct Cyc_List_List* _temp1243=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1241, tuf->tvs); struct Cyc_List_List* _temp1244=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp1242); struct Cyc_List_List* _temp1245=((
struct Cyc_List_List*(*)( void*(* f)( struct _tuple5*), struct Cyc_List_List* x))
Cyc_List_map)(( void*(*)( struct _tuple5*)) Cyc_Core_snd, _temp1243); struct Cyc_List_List*
_temp1246=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp1242, _temp1243);*
all_ref=({ struct Cyc_Core_Opt* _temp1247=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1247->v=( void*) _temp1244; _temp1247;});*
exist_ref=({ struct Cyc_Core_Opt* _temp1248=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1248->v=( void*) _temp1245; _temp1248;});{
void* res=( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp1281=( struct
Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp1281[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp1282; _temp1282.tag=
Cyc_Absyn_TunionFieldType; _temp1282.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp1283; _temp1283.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp1284=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp1284[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp1285; _temp1285.tag= Cyc_Absyn_KnownTunionfield; _temp1285.f1= tud;
_temp1285.f2= tuf; _temp1285;}); _temp1284;})); _temp1283.targs= _temp1244;
_temp1283;}); _temp1282;}); _temp1281;}); if( topt !=  0){ void* _temp1249= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_TunionInfo _temp1257; void*
_temp1259; struct Cyc_List_List* _temp1261; _LL1251: if(( unsigned int)
_temp1249 >  4u?*(( int*) _temp1249) ==  Cyc_Absyn_TunionFieldType: 0){ goto
_LL1252;} else{ goto _LL1253;} _LL1253: if(( unsigned int) _temp1249 >  4u?*((
int*) _temp1249) ==  Cyc_Absyn_TunionType: 0){ _LL1258: _temp1257=(( struct Cyc_Absyn_TunionType_struct*)
_temp1249)->f1; _LL1262: _temp1261= _temp1257.targs; goto _LL1260; _LL1260:
_temp1259=( void*) _temp1257.rgn; goto _LL1254;} else{ goto _LL1255;} _LL1255:
goto _LL1256; _LL1252: Cyc_Tcutil_unify(*(( void**) _check_null( topt)), res);
goto _LL1250; _LL1254:{ struct Cyc_List_List* a= _temp1244; for( 0; a !=  0?
_temp1261 !=  0: 0;( a=(( struct Cyc_List_List*) _check_null( a))->tl, _temp1261=((
struct Cyc_List_List*) _check_null( _temp1261))->tl)){ Cyc_Tcutil_unify(( void*)((
struct Cyc_List_List*) _check_null( a))->hd,( void*)(( struct Cyc_List_List*)
_check_null( _temp1261))->hd);}} if( tuf->typs ==  0? es ==  0: 0){ e->topt=({
struct Cyc_Core_Opt* _temp1263=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1263->v=( void*) res; _temp1263;}); res=( void*)({ struct
Cyc_Absyn_TunionType_struct* _temp1264=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp1264[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp1265; _temp1265.tag= Cyc_Absyn_TunionType;
_temp1265.f1=({ struct Cyc_Absyn_TunionInfo _temp1266; _temp1266.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp1267=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp1267[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp1268; _temp1268.tag= Cyc_Absyn_KnownTunion;
_temp1268.f1= tud; _temp1268;}); _temp1267;})); _temp1266.targs= _temp1244;
_temp1266.rgn=( void*) _temp1259; _temp1266;}); _temp1265;}); _temp1264;}); Cyc_Tcutil_unchecked_cast(
te, e, res);{ void* _temp1269= res; _npop_handler( 0u); return _temp1269;}} goto
_LL1250; _LL1256: goto _LL1250; _LL1250:;}{ struct Cyc_List_List* ts= tuf->typs;
for( 0; es !=  0? ts !=  0: 0;( es=(( struct Cyc_List_List*) _check_null( es))->tl,
ts=(( struct Cyc_List_List*) _check_null( ts))->tl)){ struct Cyc_Absyn_Exp* e=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd; void* t=
Cyc_Tcutil_rsubstitute( rgn, _temp1246,(*(( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( ts))->hd)).f2); Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e);
if( ! Cyc_Tcutil_coerce_arg( te, e, t)){({ struct Cyc_Stdio_String_pa_struct
_temp1273; _temp1273.tag= Cyc_Stdio_String_pa; _temp1273.f1=( struct _tagged_arr)(
e->topt ==  0? _tag_arr("?", sizeof( unsigned char), 2u): Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v));{ struct Cyc_Stdio_String_pa_struct
_temp1272; _temp1272.tag= Cyc_Stdio_String_pa; _temp1272.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ struct Cyc_Stdio_String_pa_struct _temp1271;
_temp1271.tag= Cyc_Stdio_String_pa; _temp1271.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
tuf->name);{ void* _temp1270[ 3u]={& _temp1271,& _temp1272,& _temp1273}; Cyc_Tcutil_terr(
e->loc, _tag_arr("tunion constructor %s expects argument of type %s but this argument has type %s",
sizeof( unsigned char), 80u), _tag_arr( _temp1270, sizeof( void*), 3u));}}}});}}
if( es !=  0){ void* _temp1276=({ struct Cyc_Stdio_String_pa_struct _temp1275;
_temp1275.tag= Cyc_Stdio_String_pa; _temp1275.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
tuf->name);{ void* _temp1274[ 1u]={& _temp1275}; Cyc_Tcexp_expr_err( te,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->loc,
_tag_arr("too many arguments for tunion constructor %s", sizeof( unsigned char),
45u), _tag_arr( _temp1274, sizeof( void*), 1u));}}); _npop_handler( 0u); return
_temp1276;} if( ts !=  0){ void* _temp1279=({ struct Cyc_Stdio_String_pa_struct
_temp1278; _temp1278.tag= Cyc_Stdio_String_pa; _temp1278.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( tuf->name);{ void* _temp1277[ 1u]={& _temp1278}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("too few arguments for tunion constructor %s", sizeof(
unsigned char), 44u), _tag_arr( _temp1277, sizeof( void*), 1u));}});
_npop_handler( 0u); return _temp1279;}{ void* _temp1280= res; _npop_handler( 0u);
return _temp1280;}}}}; _pop_region( rgn);} static void* Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* ropt, void* t){ void* rgn=( void*) Cyc_Absyn_HeapRgn; if( ropt != 
0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1297=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1297[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1298; _temp1298.tag= Cyc_Absyn_RgnHandleType; _temp1298.f1=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({ struct Cyc_Core_Opt* _temp1299=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1299->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp1299;})); _temp1298;}); _temp1297;}); void*
handle_type= Cyc_Tcexp_tcExp( te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)
_check_null( ropt)); void* _temp1287= Cyc_Tcutil_compress( handle_type); void*
_temp1293; _LL1289: if(( unsigned int) _temp1287 >  4u?*(( int*) _temp1287) == 
Cyc_Absyn_RgnHandleType: 0){ _LL1294: _temp1293=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1287)->f1; goto _LL1290;} else{ goto _LL1291;} _LL1291: goto _LL1292;
_LL1290: rgn= _temp1293; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1288; _LL1292:({ struct Cyc_Stdio_String_pa_struct _temp1296; _temp1296.tag=
Cyc_Stdio_String_pa; _temp1296.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
handle_type);{ void* _temp1295[ 1u]={& _temp1296}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)
_check_null( ropt))->loc, _tag_arr("expecting region_t type but found %s",
sizeof( unsigned char), 37u), _tag_arr( _temp1295, sizeof( void*), 1u));}});
goto _LL1288; _LL1288:;} Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, 1, t);{ void* _temp1300= t; struct Cyc_Absyn_TunionFieldInfo
_temp1306; void* _temp1308; struct Cyc_Absyn_Tunionfield* _temp1310; _LL1302:
if(( unsigned int) _temp1300 >  4u?*(( int*) _temp1300) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1307: _temp1306=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1300)->f1;
_LL1309: _temp1308=( void*) _temp1306.field_info; if(*(( int*) _temp1308) == 
Cyc_Absyn_KnownTunionfield){ _LL1311: _temp1310=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1308)->f2; goto _LL1303;} else{ goto _LL1304;}} else{ goto _LL1304;}
_LL1304: goto _LL1305; _LL1303: if( _temp1310->tvs !=  0){({ void* _temp1312[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("malloc with existential types not yet implemented",
sizeof( unsigned char), 50u), _tag_arr( _temp1312, sizeof( void*), 0u));});}
goto _LL1301; _LL1305: goto _LL1301; _LL1301:;}{ void*(* _temp1313)( void* t,
void* rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; if( topt !=  0){ void*
_temp1314= Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo
_temp1320; struct Cyc_Absyn_Conref* _temp1322; _LL1316: if(( unsigned int)
_temp1314 >  4u?*(( int*) _temp1314) ==  Cyc_Absyn_PointerType: 0){ _LL1321:
_temp1320=(( struct Cyc_Absyn_PointerType_struct*) _temp1314)->f1; _LL1323:
_temp1322= _temp1320.nullable; goto _LL1317;} else{ goto _LL1318;} _LL1318: goto
_LL1319; _LL1317: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1322)){ _temp1313= Cyc_Absyn_star_typ;} goto _LL1315; _LL1319: goto _LL1315;
_LL1315:;} return _temp1313( t, rgn, Cyc_Absyn_empty_tqual());}} static void*
Cyc_Tcexp_tcStmtExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Stmt* s){ Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
te, s), s, 1); Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));
while( 1) { void* _temp1324=( void*) s->r; struct Cyc_Absyn_Exp* _temp1334;
struct Cyc_Absyn_Stmt* _temp1336; struct Cyc_Absyn_Stmt* _temp1338; struct Cyc_Absyn_Stmt*
_temp1340; struct Cyc_Absyn_Decl* _temp1342; _LL1326: if(( unsigned int)
_temp1324 >  1u?*(( int*) _temp1324) ==  Cyc_Absyn_Exp_s: 0){ _LL1335: _temp1334=((
struct Cyc_Absyn_Exp_s_struct*) _temp1324)->f1; goto _LL1327;} else{ goto
_LL1328;} _LL1328: if(( unsigned int) _temp1324 >  1u?*(( int*) _temp1324) == 
Cyc_Absyn_Seq_s: 0){ _LL1339: _temp1338=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1324)->f1; goto _LL1337; _LL1337: _temp1336=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1324)->f2; goto _LL1329;} else{ goto _LL1330;} _LL1330: if(( unsigned int)
_temp1324 >  1u?*(( int*) _temp1324) ==  Cyc_Absyn_Decl_s: 0){ _LL1343:
_temp1342=(( struct Cyc_Absyn_Decl_s_struct*) _temp1324)->f1; goto _LL1341;
_LL1341: _temp1340=(( struct Cyc_Absyn_Decl_s_struct*) _temp1324)->f2; goto
_LL1331;} else{ goto _LL1332;} _LL1332: goto _LL1333; _LL1327: return( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1334->topt))->v; _LL1329: s= _temp1336;
continue; _LL1331: s= _temp1340; continue; _LL1333: return({ void* _temp1344[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("statement expression must end with expression",
sizeof( unsigned char), 46u), _tag_arr( _temp1344, sizeof( void*), 0u));});
_LL1325:;}} static void* Cyc_Tcexp_tcCodegen( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Fndecl* fd){ return({
void* _temp1345[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("tcCodegen",
sizeof( unsigned char), 10u), _tag_arr( _temp1345, sizeof( void*), 0u));});}
static void* Cyc_Tcexp_tcFill( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e){ return({ void* _temp1346[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("tcFill", sizeof( unsigned char), 7u),
_tag_arr( _temp1346, sizeof( void*), 0u));});} static void* Cyc_Tcexp_tcNew(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* rgn_handle, struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1){
void* rgn=( void*) Cyc_Absyn_HeapRgn; if( rgn_handle !=  0){ void* expected_type=(
void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp1357=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1357[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1358; _temp1358.tag= Cyc_Absyn_RgnHandleType;
_temp1358.f1=( void*) Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcexp_rk,({
struct Cyc_Core_Opt* _temp1359=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1359->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp1359;}));
_temp1358;}); _temp1357;}); void* handle_type= Cyc_Tcexp_tcExp( te,( void**)&
expected_type,( struct Cyc_Absyn_Exp*) _check_null( rgn_handle)); void*
_temp1347= Cyc_Tcutil_compress( handle_type); void* _temp1353; _LL1349: if((
unsigned int) _temp1347 >  4u?*(( int*) _temp1347) ==  Cyc_Absyn_RgnHandleType:
0){ _LL1354: _temp1353=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1347)->f1; goto _LL1350;} else{ goto _LL1351;} _LL1351: goto _LL1352;
_LL1350: rgn= _temp1353; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1348; _LL1352:({ struct Cyc_Stdio_String_pa_struct _temp1356; _temp1356.tag=
Cyc_Stdio_String_pa; _temp1356.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
handle_type);{ void* _temp1355[ 1u]={& _temp1356}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)
_check_null( rgn_handle))->loc, _tag_arr("expecting region_t type but found %s",
sizeof( unsigned char), 37u), _tag_arr( _temp1355, sizeof( void*), 1u));}});
goto _LL1348; _LL1348:;}{ void* _temp1360=( void*) e1->r; struct Cyc_Absyn_Exp*
_temp1372; struct Cyc_Absyn_Exp* _temp1374; struct Cyc_Absyn_Vardecl* _temp1376;
struct Cyc_List_List* _temp1378; struct Cyc_Core_Opt* _temp1380; struct Cyc_List_List*
_temp1382; void* _temp1384; struct _tagged_arr _temp1386; _LL1362: if(*(( int*)
_temp1360) ==  Cyc_Absyn_Comprehension_e){ _LL1377: _temp1376=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1360)->f1; goto _LL1375; _LL1375: _temp1374=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1360)->f2; goto _LL1373; _LL1373: _temp1372=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1360)->f3; goto _LL1363;} else{ goto _LL1364;} _LL1364: if(*(( int*)
_temp1360) ==  Cyc_Absyn_UnresolvedMem_e){ _LL1381: _temp1380=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1360)->f1; goto _LL1379; _LL1379: _temp1378=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1360)->f2; goto _LL1365;} else{ goto _LL1366;} _LL1366: if(*(( int*)
_temp1360) ==  Cyc_Absyn_Array_e){ _LL1383: _temp1382=(( struct Cyc_Absyn_Array_e_struct*)
_temp1360)->f1; goto _LL1367;} else{ goto _LL1368;} _LL1368: if(*(( int*)
_temp1360) ==  Cyc_Absyn_Const_e){ _LL1385: _temp1384=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1360)->f1; if(( unsigned int) _temp1384 >  1u?*(( int*) _temp1384) ==  Cyc_Absyn_String_c:
0){ _LL1387: _temp1386=(( struct Cyc_Absyn_String_c_struct*) _temp1384)->f1;
goto _LL1369;} else{ goto _LL1370;}} else{ goto _LL1370;} _LL1370: goto _LL1371;
_LL1363: { void* _temp1388= Cyc_Tcexp_tcExpNoPromote( te, topt, e1); void*
_temp1389= Cyc_Tcutil_compress( _temp1388); struct Cyc_Absyn_Tqual _temp1395;
void* _temp1397; _LL1391: if(( unsigned int) _temp1389 >  4u?*(( int*) _temp1389)
==  Cyc_Absyn_ArrayType: 0){ _LL1398: _temp1397=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1389)->f1; goto _LL1396; _LL1396: _temp1395=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1389)->f2; goto _LL1392;} else{ goto _LL1393;} _LL1393: goto _LL1394;
_LL1392: { void* b= Cyc_Tcutil_is_const_exp( te, _temp1374)?( void*)({ struct
Cyc_Absyn_Upper_b_struct* _temp1403=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1403[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1404; _temp1404.tag= Cyc_Absyn_Upper_b; _temp1404.f1=
_temp1374; _temp1404;}); _temp1403;}):( void*) Cyc_Absyn_Unknown_b; void*
res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1400=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1400[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1401; _temp1401.tag= Cyc_Absyn_PointerType;
_temp1401.f1=({ struct Cyc_Absyn_PtrInfo _temp1402; _temp1402.elt_typ=( void*)
_temp1397; _temp1402.rgn_typ=( void*) rgn; _temp1402.nullable=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp1402.tq= _temp1395; _temp1402.bounds= Cyc_Absyn_new_conref(
b); _temp1402;}); _temp1401;}); _temp1400;}); if( topt !=  0){ if( ! Cyc_Tcutil_unify(*((
void**) _check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc,
res_typ,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp1399=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1399->v=( void*) res_typ; _temp1399;}); Cyc_Tcutil_unchecked_cast( te, e,*((
void**) _check_null( topt))); res_typ=*(( void**) _check_null( topt));}} return
res_typ;} _LL1394: return({ void* _temp1405[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("tcNew: comprehension returned non-array type",
sizeof( unsigned char), 45u), _tag_arr( _temp1405, sizeof( void*), 0u));});
_LL1390:;} _LL1365:( void*)( e1->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp1406=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp1406[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp1407; _temp1407.tag= Cyc_Absyn_Array_e;
_temp1407.f1= _temp1378; _temp1407;}); _temp1406;}))); _temp1382= _temp1378;
goto _LL1367; _LL1367: { void** elt_typ_opt= 0; if( topt !=  0){ void* _temp1408=
Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo
_temp1414; struct Cyc_Absyn_Tqual _temp1416; void* _temp1418; void** _temp1420;
_LL1410: if(( unsigned int) _temp1408 >  4u?*(( int*) _temp1408) ==  Cyc_Absyn_PointerType:
0){ _LL1415: _temp1414=(( struct Cyc_Absyn_PointerType_struct*) _temp1408)->f1;
_LL1419: _temp1418=( void*) _temp1414.elt_typ; _temp1420=&((( struct Cyc_Absyn_PointerType_struct*)
_temp1408)->f1).elt_typ; goto _LL1417; _LL1417: _temp1416= _temp1414.tq; goto
_LL1411;} else{ goto _LL1412;} _LL1412: goto _LL1413; _LL1411: elt_typ_opt=(
void**) _temp1420; goto _LL1409; _LL1413: goto _LL1409; _LL1409:;}{ void*
_temp1421= Cyc_Tcexp_tcExpNoPromote( te, elt_typ_opt, e1); void* res_typ;{ void*
_temp1422= Cyc_Tcutil_compress( _temp1421); struct Cyc_Absyn_Exp* _temp1428;
struct Cyc_Absyn_Tqual _temp1430; void* _temp1432; _LL1424: if(( unsigned int)
_temp1422 >  4u?*(( int*) _temp1422) ==  Cyc_Absyn_ArrayType: 0){ _LL1433:
_temp1432=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1422)->f1; goto
_LL1431; _LL1431: _temp1430=(( struct Cyc_Absyn_ArrayType_struct*) _temp1422)->f2;
goto _LL1429; _LL1429: _temp1428=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1422)->f3; goto _LL1425;} else{ goto _LL1426;} _LL1426: goto _LL1427;
_LL1425: res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1434=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1434[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1435; _temp1435.tag=
Cyc_Absyn_PointerType; _temp1435.f1=({ struct Cyc_Absyn_PtrInfo _temp1436;
_temp1436.elt_typ=( void*) _temp1432; _temp1436.rgn_typ=( void*) rgn; _temp1436.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1436.tq=
_temp1430; _temp1436.bounds= Cyc_Absyn_new_conref(( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1437=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1437[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1438; _temp1438.tag= Cyc_Absyn_Upper_b;
_temp1438.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1428); _temp1438;});
_temp1437;})); _temp1436;}); _temp1435;}); _temp1434;}); if( topt !=  0){ if( !
Cyc_Tcutil_unify(*(( void**) _check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable(
te, loc, res_typ,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp1439=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1439->v=( void*) res_typ; _temp1439;}); Cyc_Tcutil_unchecked_cast( te, e,*((
void**) _check_null( topt))); res_typ=*(( void**) _check_null( topt));}} goto
_LL1423; _LL1427: return({ void* _temp1440[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("tcExpNoPromote on Array_e returned non-array type",
sizeof( unsigned char), 50u), _tag_arr( _temp1440, sizeof( void*), 0u));});
_LL1423:;} return res_typ;}} _LL1369: { void* _temp1441= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,
rgn, Cyc_Absyn_const_tqual(),( void*) Cyc_Absyn_Unknown_b); void* _temp1442= Cyc_Tcexp_tcExp(
te,( void**)& _temp1441, e1); return Cyc_Absyn_atb_typ( _temp1442, rgn, Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1443=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1443[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1444; _temp1444.tag= Cyc_Absyn_Upper_b; _temp1444.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1444;}); _temp1443;}));} _LL1371: { void** topt2=
0; if( topt !=  0){ void* _temp1445= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_PtrInfo _temp1453; struct Cyc_Absyn_Conref* _temp1455;
void* _temp1457; void** _temp1459; _LL1447: if(( unsigned int) _temp1445 >  4u?*((
int*) _temp1445) ==  Cyc_Absyn_PointerType: 0){ _LL1454: _temp1453=(( struct Cyc_Absyn_PointerType_struct*)
_temp1445)->f1; _LL1458: _temp1457=( void*) _temp1453.elt_typ; _temp1459=&(((
struct Cyc_Absyn_PointerType_struct*) _temp1445)->f1).elt_typ; goto _LL1456;
_LL1456: _temp1455= _temp1453.nullable; goto _LL1448;} else{ goto _LL1449;}
_LL1449: if(( unsigned int) _temp1445 >  4u?*(( int*) _temp1445) ==  Cyc_Absyn_TunionType:
0){ goto _LL1450;} else{ goto _LL1451;} _LL1451: goto _LL1452; _LL1448: topt2=(
void**) _temp1459; goto _LL1446; _LL1450: topt2=({ void** _temp1460=( void**)
GC_malloc( sizeof( void*)); _temp1460[ 0]=*(( void**) _check_null( topt));
_temp1460;}); goto _LL1446; _LL1452: goto _LL1446; _LL1446:;}{ void* telt= Cyc_Tcexp_tcExp(
te, topt2, e1); void* res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1462=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1462[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1463; _temp1463.tag=
Cyc_Absyn_PointerType; _temp1463.f1=({ struct Cyc_Absyn_PtrInfo _temp1464;
_temp1464.elt_typ=( void*) telt; _temp1464.rgn_typ=( void*) rgn; _temp1464.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1464.tq= Cyc_Absyn_empty_tqual();
_temp1464.bounds= Cyc_Absyn_new_conref(( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1465=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1465[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1466; _temp1466.tag= Cyc_Absyn_Upper_b;
_temp1466.f1= Cyc_Absyn_uint_exp( 1, 0); _temp1466;}); _temp1465;})); _temp1464;});
_temp1463;}); _temp1462;}); if( topt !=  0){ if( ! Cyc_Tcutil_unify(*(( void**)
_check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,*((
void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1461=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1461->v=(
void*) res_typ; _temp1461;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**)
_check_null( topt))); res_typ=*(( void**) _check_null( topt));}} return res_typ;}}
_LL1361:;}} void* Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv* te, void** topt,
struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcutil_compress( Cyc_Tcexp_tcExpNoPromote(
te, topt, e)); void* _temp1467= t; struct Cyc_Absyn_Exp* _temp1473; struct Cyc_Absyn_Tqual
_temp1475; void* _temp1477; _LL1469: if(( unsigned int) _temp1467 >  4u?*(( int*)
_temp1467) ==  Cyc_Absyn_ArrayType: 0){ _LL1478: _temp1477=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1467)->f1; goto _LL1476; _LL1476: _temp1475=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1467)->f2; goto _LL1474; _LL1474: _temp1473=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1467)->f3; goto _LL1470;} else{ goto _LL1471;} _LL1471: goto _LL1472;
_LL1470: { void* _temp1481; struct _tuple6 _temp1479= Cyc_Tcutil_addressof_props(
te, e); _LL1482: _temp1481= _temp1479.f2; goto _LL1480; _LL1480: { void*
_temp1483= _temp1473 ==  0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1484=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1484[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1485; _temp1485.tag= Cyc_Absyn_Upper_b;
_temp1485.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1473); _temp1485;});
_temp1484;}); t= Cyc_Absyn_atb_typ( _temp1477, _temp1481, _temp1475, _temp1483);(
void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) t); return t;}}
_LL1472: return t; _LL1468:;} void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te,
topt, e); void* _temp1486=( void*) e->r; void* _temp1496; _LL1488: if(*(( int*)
_temp1486) ==  Cyc_Absyn_Array_e){ goto _LL1489;} else{ goto _LL1490;} _LL1490:
if(*(( int*) _temp1486) ==  Cyc_Absyn_Comprehension_e){ goto _LL1491;} else{
goto _LL1492;} _LL1492: if(*(( int*) _temp1486) ==  Cyc_Absyn_Const_e){ _LL1497:
_temp1496=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1486)->f1; if((
unsigned int) _temp1496 >  1u?*(( int*) _temp1496) ==  Cyc_Absyn_String_c: 0){
goto _LL1493;} else{ goto _LL1494;}} else{ goto _LL1494;} _LL1494: goto _LL1495;
_LL1489: return t; _LL1491: return t; _LL1493: return t; _LL1495: t= Cyc_Tcutil_compress(
t);{ void* _temp1498= t; struct Cyc_Absyn_Exp* _temp1504; struct Cyc_Absyn_Tqual
_temp1506; void* _temp1508; _LL1500: if(( unsigned int) _temp1498 >  4u?*(( int*)
_temp1498) ==  Cyc_Absyn_ArrayType: 0){ _LL1509: _temp1508=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1498)->f1; goto _LL1507; _LL1507: _temp1506=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1498)->f2; goto _LL1505; _LL1505: _temp1504=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1498)->f3; goto _LL1501;} else{ goto _LL1502;} _LL1502: goto _LL1503;
_LL1501: { void* _temp1512; struct _tuple6 _temp1510= Cyc_Tcutil_addressof_props(
te, e); _LL1513: _temp1512= _temp1510.f2; goto _LL1511; _LL1511: { void* b=
_temp1504 ==  0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1514=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1514[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1515; _temp1515.tag= Cyc_Absyn_Upper_b;
_temp1515.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1504); _temp1515;});
_temp1514;}); t= Cyc_Absyn_atb_typ( _temp1508, _temp1512, _temp1506, b); Cyc_Tcutil_unchecked_cast(
te, e, t); return t;}} _LL1503: return t; _LL1499:;} _LL1487:;} static void* Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){{ void*
_temp1516=( void*) e->r; struct Cyc_Absyn_Exp* _temp1522; _LL1518: if(*(( int*)
_temp1516) ==  Cyc_Absyn_NoInstantiate_e){ _LL1523: _temp1522=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1516)->f1; goto _LL1519;} else{ goto _LL1520;} _LL1520: goto _LL1521;
_LL1519: Cyc_Tcexp_tcExpNoInst( te, topt, _temp1522);( void*)((( struct Cyc_Core_Opt*)
_check_null( _temp1522->topt))->v=( void*) Cyc_Absyn_pointer_expand(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1522->topt))->v)); e->topt= _temp1522->topt;
goto _LL1517; _LL1521: Cyc_Tcexp_tcExpNoInst( te, topt, e);( void*)((( struct
Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v));{ void* _temp1524=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
struct Cyc_Absyn_PtrInfo _temp1530; struct Cyc_Absyn_Conref* _temp1532; struct
Cyc_Absyn_Tqual _temp1534; struct Cyc_Absyn_Conref* _temp1536; void* _temp1538;
void* _temp1540; _LL1526: if(( unsigned int) _temp1524 >  4u?*(( int*) _temp1524)
==  Cyc_Absyn_PointerType: 0){ _LL1531: _temp1530=(( struct Cyc_Absyn_PointerType_struct*)
_temp1524)->f1; _LL1541: _temp1540=( void*) _temp1530.elt_typ; goto _LL1539;
_LL1539: _temp1538=( void*) _temp1530.rgn_typ; goto _LL1537; _LL1537: _temp1536=
_temp1530.nullable; goto _LL1535; _LL1535: _temp1534= _temp1530.tq; goto _LL1533;
_LL1533: _temp1532= _temp1530.bounds; goto _LL1527;} else{ goto _LL1528;}
_LL1528: goto _LL1529; _LL1527:{ void* _temp1542= Cyc_Tcutil_compress( _temp1540);
struct Cyc_Absyn_FnInfo _temp1548; struct Cyc_List_List* _temp1550; struct Cyc_List_List*
_temp1552; struct Cyc_Absyn_VarargInfo* _temp1554; int _temp1556; struct Cyc_List_List*
_temp1558; void* _temp1560; struct Cyc_Core_Opt* _temp1562; struct Cyc_List_List*
_temp1564; _LL1544: if(( unsigned int) _temp1542 >  4u?*(( int*) _temp1542) == 
Cyc_Absyn_FnType: 0){ _LL1549: _temp1548=(( struct Cyc_Absyn_FnType_struct*)
_temp1542)->f1; _LL1565: _temp1564= _temp1548.tvars; goto _LL1563; _LL1563:
_temp1562= _temp1548.effect; goto _LL1561; _LL1561: _temp1560=( void*) _temp1548.ret_typ;
goto _LL1559; _LL1559: _temp1558= _temp1548.args; goto _LL1557; _LL1557:
_temp1556= _temp1548.c_varargs; goto _LL1555; _LL1555: _temp1554= _temp1548.cyc_varargs;
goto _LL1553; _LL1553: _temp1552= _temp1548.rgn_po; goto _LL1551; _LL1551:
_temp1550= _temp1548.attributes; goto _LL1545;} else{ goto _LL1546;} _LL1546:
goto _LL1547; _LL1545: if( _temp1564 !=  0){ struct _RegionHandle _temp1566=
_new_region(); struct _RegionHandle* rgn=& _temp1566; _push_region( rgn);{
struct _tuple4 _temp1567=({ struct _tuple4 _temp1577; _temp1577.f1= Cyc_Tcenv_lookup_type_vars(
te); _temp1577.f2= rgn; _temp1577;}); struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1567, _temp1564); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, inst); void* ftyp= Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1574=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1574[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1575; _temp1575.tag= Cyc_Absyn_FnType; _temp1575.f1=({ struct Cyc_Absyn_FnInfo
_temp1576; _temp1576.tvars= 0; _temp1576.effect= _temp1562; _temp1576.ret_typ=(
void*) _temp1560; _temp1576.args= _temp1558; _temp1576.c_varargs= _temp1556;
_temp1576.cyc_varargs= _temp1554; _temp1576.rgn_po= _temp1552; _temp1576.attributes=
_temp1550; _temp1576;}); _temp1575;}); _temp1574;})); void* new_typ=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp1571=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1571[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1572; _temp1572.tag= Cyc_Absyn_PointerType;
_temp1572.f1=({ struct Cyc_Absyn_PtrInfo _temp1573; _temp1573.elt_typ=( void*)
ftyp; _temp1573.rgn_typ=( void*) _temp1538; _temp1573.nullable= _temp1536;
_temp1573.tq= _temp1534; _temp1573.bounds= _temp1532; _temp1573;}); _temp1572;});
_temp1571;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Instantiate_e_struct* _temp1568=( struct Cyc_Absyn_Instantiate_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct)); _temp1568[ 0]=({
struct Cyc_Absyn_Instantiate_e_struct _temp1569; _temp1569.tag= Cyc_Absyn_Instantiate_e;
_temp1569.f1= inner; _temp1569.f2= ts; _temp1569;}); _temp1568;}))); e->topt=({
struct Cyc_Core_Opt* _temp1570=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1570->v=( void*) new_typ; _temp1570;});}; _pop_region( rgn);}
goto _LL1543; _LL1547: goto _LL1543; _LL1543:;} goto _LL1525; _LL1529: goto
_LL1525; _LL1525:;} goto _LL1517; _LL1517:;} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v;} static void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment* loc= e->loc;
void* t;{ void* _temp1578=( void*) e->r; struct Cyc_Absyn_Exp* _temp1652; struct
_tuple1* _temp1654; struct Cyc_List_List* _temp1656; struct Cyc_Absyn_Exp*
_temp1658; struct Cyc_List_List* _temp1660; struct Cyc_Core_Opt* _temp1662; void*
_temp1664; void* _temp1666; struct _tuple1* _temp1668; struct Cyc_List_List*
_temp1670; void* _temp1672; void* _temp1674; struct Cyc_Absyn_Exp* _temp1676;
struct Cyc_Absyn_Exp* _temp1678; struct Cyc_Core_Opt* _temp1680; struct Cyc_Absyn_Exp*
_temp1682; struct Cyc_Absyn_Exp* _temp1684; struct Cyc_Absyn_Exp* _temp1686;
struct Cyc_Absyn_Exp* _temp1688; struct Cyc_Absyn_Exp* _temp1690; struct Cyc_Absyn_Exp*
_temp1692; struct Cyc_Absyn_VarargCallInfo* _temp1694; struct Cyc_Absyn_VarargCallInfo**
_temp1696; struct Cyc_List_List* _temp1697; struct Cyc_Absyn_Exp* _temp1699;
struct Cyc_Absyn_Exp* _temp1701; struct Cyc_List_List* _temp1703; struct Cyc_Absyn_Exp*
_temp1705; struct Cyc_Absyn_Exp* _temp1707; void* _temp1709; struct Cyc_Absyn_Exp*
_temp1711; struct Cyc_Absyn_Exp* _temp1713; struct Cyc_Absyn_Exp* _temp1715;
struct Cyc_Absyn_Exp* _temp1717; void* _temp1719; struct _tagged_arr* _temp1721;
void* _temp1723; struct Cyc_Absyn_Exp* _temp1725; struct _tagged_arr* _temp1727;
struct Cyc_Absyn_Exp* _temp1729; struct _tagged_arr* _temp1731; struct Cyc_Absyn_Exp*
_temp1733; struct Cyc_Absyn_Exp* _temp1735; struct Cyc_Absyn_Exp* _temp1737;
struct Cyc_List_List* _temp1739; struct Cyc_List_List* _temp1741; struct _tuple2*
_temp1743; struct Cyc_List_List* _temp1745; struct Cyc_Absyn_Stmt* _temp1747;
struct Cyc_Absyn_Fndecl* _temp1749; struct Cyc_Absyn_Exp* _temp1751; struct Cyc_Absyn_Exp*
_temp1753; struct Cyc_Absyn_Exp* _temp1755; struct Cyc_Absyn_Vardecl* _temp1757;
struct Cyc_Absyn_Structdecl* _temp1759; struct Cyc_Absyn_Structdecl** _temp1761;
struct Cyc_List_List* _temp1762; struct Cyc_Core_Opt* _temp1764; struct Cyc_Core_Opt**
_temp1766; struct _tuple1* _temp1767; struct _tuple1** _temp1769; struct Cyc_List_List*
_temp1770; void* _temp1772; struct Cyc_Absyn_Tunionfield* _temp1774; struct Cyc_Absyn_Tuniondecl*
_temp1776; struct Cyc_List_List* _temp1778; struct Cyc_Core_Opt* _temp1780;
struct Cyc_Core_Opt** _temp1782; struct Cyc_Core_Opt* _temp1783; struct Cyc_Core_Opt**
_temp1785; struct Cyc_Absyn_Enumfield* _temp1786; struct Cyc_Absyn_Enumdecl*
_temp1788; struct _tuple1* _temp1790; struct _tuple1** _temp1792; void*
_temp1793; struct Cyc_Absyn_Exp* _temp1795; _LL1580: if(*(( int*) _temp1578) == 
Cyc_Absyn_NoInstantiate_e){ _LL1653: _temp1652=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1578)->f1; goto _LL1581;} else{ goto _LL1582;} _LL1582: if(*(( int*)
_temp1578) ==  Cyc_Absyn_UnknownId_e){ _LL1655: _temp1654=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp1578)->f1; goto _LL1583;} else{ goto _LL1584;} _LL1584: if(*(( int*)
_temp1578) ==  Cyc_Absyn_UnknownCall_e){ _LL1659: _temp1658=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1578)->f1; goto _LL1657; _LL1657: _temp1656=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1578)->f2; goto _LL1585;} else{ goto _LL1586;} _LL1586: if(*(( int*)
_temp1578) ==  Cyc_Absyn_UnresolvedMem_e){ _LL1663: _temp1662=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1578)->f1; goto _LL1661; _LL1661: _temp1660=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1578)->f2; goto _LL1587;} else{ goto _LL1588;} _LL1588: if(*(( int*)
_temp1578) ==  Cyc_Absyn_Const_e){ _LL1665: _temp1664=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1578)->f1; goto _LL1589;} else{ goto _LL1590;} _LL1590: if(*(( int*)
_temp1578) ==  Cyc_Absyn_Var_e){ _LL1669: _temp1668=(( struct Cyc_Absyn_Var_e_struct*)
_temp1578)->f1; goto _LL1667; _LL1667: _temp1666=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1578)->f2; goto _LL1591;} else{ goto _LL1592;} _LL1592: if(*(( int*)
_temp1578) ==  Cyc_Absyn_Primop_e){ _LL1673: _temp1672=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1578)->f1; goto _LL1671; _LL1671: _temp1670=(( struct Cyc_Absyn_Primop_e_struct*)
_temp1578)->f2; goto _LL1593;} else{ goto _LL1594;} _LL1594: if(*(( int*)
_temp1578) ==  Cyc_Absyn_Increment_e){ _LL1677: _temp1676=(( struct Cyc_Absyn_Increment_e_struct*)
_temp1578)->f1; goto _LL1675; _LL1675: _temp1674=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1578)->f2; goto _LL1595;} else{ goto _LL1596;} _LL1596: if(*(( int*)
_temp1578) ==  Cyc_Absyn_AssignOp_e){ _LL1683: _temp1682=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1578)->f1; goto _LL1681; _LL1681: _temp1680=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1578)->f2; goto _LL1679; _LL1679: _temp1678=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1578)->f3; goto _LL1597;} else{ goto _LL1598;} _LL1598: if(*(( int*)
_temp1578) ==  Cyc_Absyn_Conditional_e){ _LL1689: _temp1688=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1578)->f1; goto _LL1687; _LL1687: _temp1686=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1578)->f2; goto _LL1685; _LL1685: _temp1684=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1578)->f3; goto _LL1599;} else{ goto _LL1600;} _LL1600: if(*(( int*)
_temp1578) ==  Cyc_Absyn_SeqExp_e){ _LL1693: _temp1692=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1578)->f1; goto _LL1691; _LL1691: _temp1690=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1578)->f2; goto _LL1601;} else{ goto _LL1602;} _LL1602: if(*(( int*)
_temp1578) ==  Cyc_Absyn_FnCall_e){ _LL1700: _temp1699=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1578)->f1; goto _LL1698; _LL1698: _temp1697=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1578)->f2; goto _LL1695; _LL1695: _temp1694=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1578)->f3; _temp1696=&(( struct Cyc_Absyn_FnCall_e_struct*) _temp1578)->f3;
goto _LL1603;} else{ goto _LL1604;} _LL1604: if(*(( int*) _temp1578) ==  Cyc_Absyn_Throw_e){
_LL1702: _temp1701=(( struct Cyc_Absyn_Throw_e_struct*) _temp1578)->f1; goto
_LL1605;} else{ goto _LL1606;} _LL1606: if(*(( int*) _temp1578) ==  Cyc_Absyn_Instantiate_e){
_LL1706: _temp1705=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp1578)->f1;
goto _LL1704; _LL1704: _temp1703=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1578)->f2; goto _LL1607;} else{ goto _LL1608;} _LL1608: if(*(( int*)
_temp1578) ==  Cyc_Absyn_Cast_e){ _LL1710: _temp1709=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1578)->f1; goto _LL1708; _LL1708: _temp1707=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1578)->f2; goto _LL1609;} else{ goto _LL1610;} _LL1610: if(*(( int*)
_temp1578) ==  Cyc_Absyn_Address_e){ _LL1712: _temp1711=(( struct Cyc_Absyn_Address_e_struct*)
_temp1578)->f1; goto _LL1611;} else{ goto _LL1612;} _LL1612: if(*(( int*)
_temp1578) ==  Cyc_Absyn_New_e){ _LL1716: _temp1715=(( struct Cyc_Absyn_New_e_struct*)
_temp1578)->f1; goto _LL1714; _LL1714: _temp1713=(( struct Cyc_Absyn_New_e_struct*)
_temp1578)->f2; goto _LL1613;} else{ goto _LL1614;} _LL1614: if(*(( int*)
_temp1578) ==  Cyc_Absyn_Sizeofexp_e){ _LL1718: _temp1717=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1578)->f1; goto _LL1615;} else{ goto _LL1616;} _LL1616: if(*(( int*)
_temp1578) ==  Cyc_Absyn_Sizeoftyp_e){ _LL1720: _temp1719=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1578)->f1; goto _LL1617;} else{ goto _LL1618;} _LL1618: if(*(( int*)
_temp1578) ==  Cyc_Absyn_Offsetof_e){ _LL1724: _temp1723=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1578)->f1; goto _LL1722; _LL1722: _temp1721=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1578)->f2; goto _LL1619;} else{ goto _LL1620;} _LL1620: if(*(( int*)
_temp1578) ==  Cyc_Absyn_Deref_e){ _LL1726: _temp1725=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1578)->f1; goto _LL1621;} else{ goto _LL1622;} _LL1622: if(*(( int*)
_temp1578) ==  Cyc_Absyn_StructMember_e){ _LL1730: _temp1729=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1578)->f1; goto _LL1728; _LL1728: _temp1727=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1578)->f2; goto _LL1623;} else{ goto _LL1624;} _LL1624: if(*(( int*)
_temp1578) ==  Cyc_Absyn_StructArrow_e){ _LL1734: _temp1733=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1578)->f1; goto _LL1732; _LL1732: _temp1731=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1578)->f2; goto _LL1625;} else{ goto _LL1626;} _LL1626: if(*(( int*)
_temp1578) ==  Cyc_Absyn_Subscript_e){ _LL1738: _temp1737=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1578)->f1; goto _LL1736; _LL1736: _temp1735=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1578)->f2; goto _LL1627;} else{ goto _LL1628;} _LL1628: if(*(( int*)
_temp1578) ==  Cyc_Absyn_Tuple_e){ _LL1740: _temp1739=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1578)->f1; goto _LL1629;} else{ goto _LL1630;} _LL1630: if(*(( int*)
_temp1578) ==  Cyc_Absyn_CompoundLit_e){ _LL1744: _temp1743=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1578)->f1; goto _LL1742; _LL1742: _temp1741=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1578)->f2; goto _LL1631;} else{ goto _LL1632;} _LL1632: if(*(( int*)
_temp1578) ==  Cyc_Absyn_Array_e){ _LL1746: _temp1745=(( struct Cyc_Absyn_Array_e_struct*)
_temp1578)->f1; goto _LL1633;} else{ goto _LL1634;} _LL1634: if(*(( int*)
_temp1578) ==  Cyc_Absyn_StmtExp_e){ _LL1748: _temp1747=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1578)->f1; goto _LL1635;} else{ goto _LL1636;} _LL1636: if(*(( int*)
_temp1578) ==  Cyc_Absyn_Codegen_e){ _LL1750: _temp1749=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1578)->f1; goto _LL1637;} else{ goto _LL1638;} _LL1638: if(*(( int*)
_temp1578) ==  Cyc_Absyn_Fill_e){ _LL1752: _temp1751=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1578)->f1; goto _LL1639;} else{ goto _LL1640;} _LL1640: if(*(( int*)
_temp1578) ==  Cyc_Absyn_Comprehension_e){ _LL1758: _temp1757=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1578)->f1; goto _LL1756; _LL1756: _temp1755=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1578)->f2; goto _LL1754; _LL1754: _temp1753=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1578)->f3; goto _LL1641;} else{ goto _LL1642;} _LL1642: if(*(( int*)
_temp1578) ==  Cyc_Absyn_Struct_e){ _LL1768: _temp1767=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1578)->f1; _temp1769=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1578)->f1;
goto _LL1765; _LL1765: _temp1764=(( struct Cyc_Absyn_Struct_e_struct*) _temp1578)->f2;
_temp1766=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1578)->f2; goto _LL1763;
_LL1763: _temp1762=(( struct Cyc_Absyn_Struct_e_struct*) _temp1578)->f3; goto
_LL1760; _LL1760: _temp1759=(( struct Cyc_Absyn_Struct_e_struct*) _temp1578)->f4;
_temp1761=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1578)->f4; goto _LL1643;}
else{ goto _LL1644;} _LL1644: if(*(( int*) _temp1578) ==  Cyc_Absyn_AnonStruct_e){
_LL1773: _temp1772=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp1578)->f1;
goto _LL1771; _LL1771: _temp1770=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1578)->f2; goto _LL1645;} else{ goto _LL1646;} _LL1646: if(*(( int*)
_temp1578) ==  Cyc_Absyn_Tunion_e){ _LL1784: _temp1783=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1578)->f1; _temp1785=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1578)->f1;
goto _LL1781; _LL1781: _temp1780=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1578)->f2;
_temp1782=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1578)->f2; goto _LL1779;
_LL1779: _temp1778=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1578)->f3; goto
_LL1777; _LL1777: _temp1776=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1578)->f4;
goto _LL1775; _LL1775: _temp1774=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1578)->f5;
goto _LL1647;} else{ goto _LL1648;} _LL1648: if(*(( int*) _temp1578) ==  Cyc_Absyn_Enum_e){
_LL1791: _temp1790=(( struct Cyc_Absyn_Enum_e_struct*) _temp1578)->f1; _temp1792=&((
struct Cyc_Absyn_Enum_e_struct*) _temp1578)->f1; goto _LL1789; _LL1789:
_temp1788=(( struct Cyc_Absyn_Enum_e_struct*) _temp1578)->f2; goto _LL1787;
_LL1787: _temp1786=(( struct Cyc_Absyn_Enum_e_struct*) _temp1578)->f3; goto
_LL1649;} else{ goto _LL1650;} _LL1650: if(*(( int*) _temp1578) ==  Cyc_Absyn_Malloc_e){
_LL1796: _temp1795=(( struct Cyc_Absyn_Malloc_e_struct*) _temp1578)->f1; goto
_LL1794; _LL1794: _temp1793=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1578)->f2; goto _LL1651;} else{ goto _LL1579;} _LL1581: Cyc_Tcexp_tcExpNoInst(
te, 0, _temp1652); return; _LL1583: Cyc_Tcexp_resolve_unknown_id( te, e,
_temp1654); Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL1585: Cyc_Tcexp_resolve_unknown_fn(
te, e, _temp1658, _temp1656); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL1587: Cyc_Tcexp_resolve_unresolved_mem( te, loc, topt, e, _temp1660); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL1589: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp1664, e);
goto _LL1579; _LL1591: t= Cyc_Tcexp_tcVar( te, loc, _temp1668, _temp1666); goto
_LL1579; _LL1593: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp1672, _temp1670);
goto _LL1579; _LL1595: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp1676,
_temp1674); goto _LL1579; _LL1597: t= Cyc_Tcexp_tcAssignOp( te, loc, topt,
_temp1682, _temp1680, _temp1678); goto _LL1579; _LL1599: t= Cyc_Tcexp_tcConditional(
te, loc, topt, _temp1688, _temp1686, _temp1684); goto _LL1579; _LL1601: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp1692, _temp1690); goto _LL1579; _LL1603: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp1699, _temp1697, _temp1696); goto _LL1579; _LL1605: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp1701); goto _LL1579; _LL1607: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp1705, _temp1703); goto _LL1579; _LL1609: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp1709, _temp1707); goto _LL1579; _LL1611: t= Cyc_Tcexp_tcAddress(
te, loc, topt, _temp1711); goto _LL1579; _LL1613: t= Cyc_Tcexp_tcNew( te, loc,
topt, _temp1715, e, _temp1713); goto _LL1579; _LL1615: { void* _temp1797= Cyc_Tcexp_tcExpNoPromote(
te, 0, _temp1717); t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1797); goto
_LL1579;} _LL1617: t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1719); goto
_LL1579; _LL1619: t= Cyc_Tcexp_tcOffsetof( te, loc, topt, _temp1723, _temp1721);
goto _LL1579; _LL1621: t= Cyc_Tcexp_tcDeref( te, loc, topt, _temp1725); goto
_LL1579; _LL1623: t= Cyc_Tcexp_tcStructMember( te, loc, topt, e, _temp1729,
_temp1727); goto _LL1579; _LL1625: t= Cyc_Tcexp_tcStructArrow( te, loc, topt,
_temp1733, _temp1731); goto _LL1579; _LL1627: t= Cyc_Tcexp_tcSubscript( te, loc,
topt, _temp1737, _temp1735); goto _LL1579; _LL1629: t= Cyc_Tcexp_tcTuple( te,
loc, topt, _temp1739); goto _LL1579; _LL1631: t= Cyc_Tcexp_tcCompoundLit( te,
loc, topt, _temp1743, _temp1741); goto _LL1579; _LL1633: { void** elt_topt= 0;
if( topt !=  0){ void* _temp1798= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); void* _temp1804; void** _temp1806; _LL1800: if(( unsigned int)
_temp1798 >  4u?*(( int*) _temp1798) ==  Cyc_Absyn_ArrayType: 0){ _LL1805:
_temp1804=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1798)->f1;
_temp1806=&(( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1798)->f1); goto
_LL1801;} else{ goto _LL1802;} _LL1802: goto _LL1803; _LL1801: elt_topt=( void**)
_temp1806; goto _LL1799; _LL1803: goto _LL1799; _LL1799:;} t= Cyc_Tcexp_tcArray(
te, loc, elt_topt, _temp1745); goto _LL1579;} _LL1635: t= Cyc_Tcexp_tcStmtExp(
te, loc, topt, _temp1747); goto _LL1579; _LL1637: t= Cyc_Tcexp_tcCodegen( te,
loc, topt, _temp1749); goto _LL1579; _LL1639: t= Cyc_Tcexp_tcFill( te, loc, topt,
_temp1751); goto _LL1579; _LL1641: t= Cyc_Tcexp_tcComprehension( te, loc, topt,
_temp1757, _temp1755, _temp1753); goto _LL1579; _LL1643: t= Cyc_Tcexp_tcStruct(
te, loc, topt, _temp1769, _temp1766, _temp1762, _temp1761); goto _LL1579;
_LL1645: t= Cyc_Tcexp_tcAnonStruct( te, loc, _temp1772, _temp1770); goto _LL1579;
_LL1647: t= Cyc_Tcexp_tcTunion( te, loc, topt, e, _temp1785, _temp1782,
_temp1778, _temp1776, _temp1774); goto _LL1579; _LL1649:* _temp1792=(( struct
Cyc_Absyn_Enumfield*) _check_null( _temp1786))->name; t=( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp1807=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp1807[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp1808; _temp1808.tag= Cyc_Absyn_EnumType;
_temp1808.f1=(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp1788))->name;
_temp1808.f2= _temp1788; _temp1808;}); _temp1807;}); goto _LL1579; _LL1651: t=
Cyc_Tcexp_tcMalloc( te, loc, topt, _temp1795, _temp1793); goto _LL1579; _LL1579:;}
e->topt=({ struct Cyc_Core_Opt* _temp1809=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1809->v=( void*) t; _temp1809;});}
