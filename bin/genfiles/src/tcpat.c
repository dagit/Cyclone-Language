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
void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap(
struct _RegionHandle*, void*(* f)( void*), struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_map_c( void*(* f)( void*, void*), void* env,
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap_c( struct
_RegionHandle*, void*(* f)( void*, void*), void* env, struct Cyc_List_List* x);
extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append( struct
Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_rappend(
struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y); extern
unsigned char Cyc_List_Nth[ 8u]; extern struct Cyc_List_List* Cyc_List_rzip(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x,
struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_tabulate_c( int
n, void*(* f)( void*, int), void* env); struct Cyc_Dict_Dict; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
struct Cyc_Set_Set; extern struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*,
void*)); extern struct Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void*
elt); extern int Cyc_Set_cardinality( struct Cyc_Set_Set* s); extern int Cyc_Set_member(
struct Cyc_Set_Set* s, void* elt); extern unsigned char Cyc_Set_Absent[ 11u];
struct Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[ 19u];
extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern int Cyc_String_strcmp(
struct _tagged_arr s1, struct _tagged_arr s2); extern int Cyc_String_zstrcmp(
struct _tagged_arr, struct _tagged_arr); extern int Cyc_String_zstrptrcmp(
struct _tagged_arr*, struct _tagged_arr*); struct Cyc_Lineno_Pos{ struct
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
15u]; extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref( void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_new_evar( struct Cyc_Core_Opt* k, struct Cyc_Core_Opt*
tenv); extern void* Cyc_Absyn_wildtyp( struct Cyc_Core_Opt*); extern void* Cyc_Absyn_uchar_t;
extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_float_t;
extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple1* x, void*
t, struct Cyc_Absyn_Exp* init); struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct
Cyc_PP_Doc; struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int qvar_to_Cids:
1; int add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int
rewrite_temp_tvars: 1; int print_all_tvars: 1; int print_all_kinds: 1; int
print_using_stmts: 1; int print_externC_stmts: 1; int print_full_evars: 1; int
use_curr_namespace: 1; struct Cyc_List_List* curr_namespace; } ; extern struct
_tagged_arr Cyc_Absynpp_typ2string( void*); extern struct _tagged_arr Cyc_Absynpp_kind2string(
void*); extern struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple1*);
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
struct _tuple1*); extern struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*);
extern void Cyc_Tcenv_check_rgn_accessible( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
void* rgn); extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos(
struct _tagged_arr); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_arr); extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*,
struct _tagged_arr); extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_coerceable(
void*); extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_rsubstitute(
struct _RegionHandle*, struct Cyc_List_List*, void*); struct _tuple4{ struct Cyc_List_List*
f1; struct _RegionHandle* f2; } ; struct _tuple5{ struct Cyc_Absyn_Tvar* f1;
void* f2; } ; extern struct _tuple5* Cyc_Tcutil_r_make_inst_var( struct _tuple4*,
struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_check_unique_vars( struct Cyc_List_List*
vs, struct Cyc_Position_Segment* loc, struct _tagged_arr err_msg); extern struct
Cyc_List_List* Cyc_Tcutil_resolve_struct_designators( struct _RegionHandle* rgn,
struct Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_List_List*
fields); struct _tuple6{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ;
extern struct _tuple6 Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat*
p, void** topt, void** region_opt); extern void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*, struct Cyc_List_List*); struct _tuple7{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Pat* f2; } ; static void Cyc_Tcpat_resolve_pat( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p){ void* _temp0=( void*) p->r; struct _tuple1*
_temp10; struct Cyc_List_List* _temp12; struct Cyc_List_List* _temp14; struct
_tuple1* _temp16; struct Cyc_List_List* _temp18; struct Cyc_List_List* _temp20;
struct _tuple1* _temp22; _LL2: if(( unsigned int) _temp0 > 2u?*(( int*) _temp0)
== Cyc_Absyn_UnknownId_p: 0){ _LL11: _temp10=(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp0)->f1; goto _LL3;} else{ goto _LL4;} _LL4: if(( unsigned int) _temp0 > 2u?*((
int*) _temp0) == Cyc_Absyn_UnknownCall_p: 0){ _LL17: _temp16=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp0)->f1; goto _LL15; _LL15: _temp14=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp0)->f2; goto _LL13; _LL13: _temp12=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp0)->f3; goto _LL5;} else{ goto _LL6;} _LL6: if(( unsigned int) _temp0 > 2u?*((
int*) _temp0) == Cyc_Absyn_UnknownFields_p: 0){ _LL23: _temp22=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp0)->f1; goto _LL21; _LL21: _temp20=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp0)->f2; goto _LL19; _LL19: _temp18=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp0)->f3; goto _LL7;} else{ goto _LL8;} _LL8: goto _LL9; _LL3:{ struct
_handler_cons _temp24; _push_handler(& _temp24);{ int _temp26= 0; if( setjmp(
_temp24.handler)){ _temp26= 1;} if( ! _temp26){{ void* _temp27= Cyc_Tcenv_lookup_ordinary(
te, p->loc, _temp10); struct Cyc_Absyn_Structdecl* _temp37; struct Cyc_Absyn_Tunionfield*
_temp39; struct Cyc_Absyn_Tuniondecl* _temp41; struct Cyc_Absyn_Enumfield*
_temp43; struct Cyc_Absyn_Enumdecl* _temp45; void* _temp47; _LL29: if(*(( int*)
_temp27) == Cyc_Tcenv_StructRes){ _LL38: _temp37=(( struct Cyc_Tcenv_StructRes_struct*)
_temp27)->f1; goto _LL30;} else{ goto _LL31;} _LL31: if(*(( int*) _temp27) ==
Cyc_Tcenv_TunionRes){ _LL42: _temp41=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp27)->f1; goto _LL40; _LL40: _temp39=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp27)->f2; goto _LL32;} else{ goto _LL33;} _LL33: if(*(( int*) _temp27) ==
Cyc_Tcenv_EnumRes){ _LL46: _temp45=(( struct Cyc_Tcenv_EnumRes_struct*) _temp27)->f1;
goto _LL44; _LL44: _temp43=(( struct Cyc_Tcenv_EnumRes_struct*) _temp27)->f2;
goto _LL34;} else{ goto _LL35;} _LL35: if(*(( int*) _temp27) == Cyc_Tcenv_VarRes){
_LL48: _temp47=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp27)->f1; goto
_LL36;} else{ goto _LL28;} _LL30: Cyc_Tcutil_terr( p->loc, _tag_arr("struct tag used without arguments in pattern",
sizeof( unsigned char), 45u)); _npop_handler( 0u); return; _LL32:( void*)( p->r=(
void*)(( void*)({ struct Cyc_Absyn_Tunion_p_struct* _temp49=( struct Cyc_Absyn_Tunion_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct)); _temp49[ 0]=({ struct Cyc_Absyn_Tunion_p_struct
_temp50; _temp50.tag= Cyc_Absyn_Tunion_p; _temp50.f1= _temp41; _temp50.f2=
_temp39; _temp50.f3= 0; _temp50.f4= 0; _temp50;}); _temp49;}))); _npop_handler(
0u); return; _LL34:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_p_struct*
_temp51=( struct Cyc_Absyn_Enum_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_p_struct));
_temp51[ 0]=({ struct Cyc_Absyn_Enum_p_struct _temp52; _temp52.tag= Cyc_Absyn_Enum_p;
_temp52.f1= _temp45; _temp52.f2= _temp43; _temp52;}); _temp51;})));
_npop_handler( 0u); return; _LL36: goto _LL28; _LL28:;}; _pop_handler();} else{
void* _temp25=( void*) _exn_thrown; void* _temp54= _temp25; _LL56: if( _temp54
== Cyc_Dict_Absent){ goto _LL57;} else{ goto _LL58;} _LL58: goto _LL59; _LL57:
goto _LL55; _LL59:( void) _throw( _temp54); _LL55:;}}}{ void* _temp60=(* _temp10).f1;
struct Cyc_List_List* _temp68; _LL62: if( _temp60 ==( void*) Cyc_Absyn_Loc_n){
goto _LL63;} else{ goto _LL64;} _LL64: if(( unsigned int) _temp60 > 1u?*(( int*)
_temp60) == Cyc_Absyn_Rel_n: 0){ _LL69: _temp68=(( struct Cyc_Absyn_Rel_n_struct*)
_temp60)->f1; if( _temp68 == 0){ goto _LL65;} else{ goto _LL66;}} else{ goto
_LL66;} _LL66: goto _LL67; _LL63: goto _LL65; _LL65:(* _temp10).f1=( void*) Cyc_Absyn_Loc_n;(
void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct* _temp70=( struct
Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp70[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp71; _temp71.tag= Cyc_Absyn_Var_p;
_temp71.f1= Cyc_Absyn_new_vardecl( _temp10,( void*) Cyc_Absyn_VoidType, 0);
_temp71;}); _temp70;}))); return; _LL67: Cyc_Tcutil_terr( p->loc, _tag_arr("qualified variable in pattern",
sizeof( unsigned char), 30u)); return; _LL61:;} _LL5:{ struct _handler_cons
_temp72; _push_handler(& _temp72);{ int _temp74= 0; if( setjmp( _temp72.handler)){
_temp74= 1;} if( ! _temp74){{ void* _temp75= Cyc_Tcenv_lookup_ordinary( te, p->loc,
_temp16); struct Cyc_Absyn_Structdecl* _temp85; struct Cyc_Absyn_Tunionfield*
_temp87; struct Cyc_Absyn_Tuniondecl* _temp89; struct Cyc_Absyn_Enumfield*
_temp91; struct Cyc_Absyn_Enumdecl* _temp93; void* _temp95; _LL77: if(*(( int*)
_temp75) == Cyc_Tcenv_StructRes){ _LL86: _temp85=(( struct Cyc_Tcenv_StructRes_struct*)
_temp75)->f1; goto _LL78;} else{ goto _LL79;} _LL79: if(*(( int*) _temp75) ==
Cyc_Tcenv_TunionRes){ _LL90: _temp89=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp75)->f1; goto _LL88; _LL88: _temp87=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp75)->f2; goto _LL80;} else{ goto _LL81;} _LL81: if(*(( int*) _temp75) ==
Cyc_Tcenv_EnumRes){ _LL94: _temp93=(( struct Cyc_Tcenv_EnumRes_struct*) _temp75)->f1;
goto _LL92; _LL92: _temp91=(( struct Cyc_Tcenv_EnumRes_struct*) _temp75)->f2;
goto _LL82;} else{ goto _LL83;} _LL83: if(*(( int*) _temp75) == Cyc_Tcenv_VarRes){
_LL96: _temp95=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp75)->f1; goto
_LL84;} else{ goto _LL76;} _LL78: { struct Cyc_List_List* _temp97= 0; for( 0;
_temp12 != 0; _temp12=(( struct Cyc_List_List*) _check_null( _temp12))->tl){
_temp97=({ struct Cyc_List_List* _temp98=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp98->hd=( void*)({ struct _tuple7* _temp99=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp99->f1= 0; _temp99->f2=(
struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null( _temp12))->hd;
_temp99;}); _temp98->tl= _temp97; _temp98;});}( void*)( p->r=( void*)(( void*)({
struct Cyc_Absyn_Struct_p_struct* _temp100=( struct Cyc_Absyn_Struct_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct)); _temp100[ 0]=({ struct
Cyc_Absyn_Struct_p_struct _temp101; _temp101.tag= Cyc_Absyn_Struct_p; _temp101.f1=
_temp85; _temp101.f2= 0; _temp101.f3= _temp14; _temp101.f4=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp97); _temp101;}); _temp100;})));
_npop_handler( 0u); return;} _LL80:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_p_struct*
_temp102=( struct Cyc_Absyn_Tunion_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct));
_temp102[ 0]=({ struct Cyc_Absyn_Tunion_p_struct _temp103; _temp103.tag= Cyc_Absyn_Tunion_p;
_temp103.f1= _temp89; _temp103.f2= _temp87; _temp103.f3= _temp14; _temp103.f4=
_temp12; _temp103;}); _temp102;}))); _npop_handler( 0u); return; _LL82: Cyc_Tcutil_terr(
p->loc,( struct _tagged_arr)({ struct _tagged_arr _temp104= Cyc_Absynpp_qvar2string(
_temp16); xprintf("bad enum constructor %.*s in pattern", _get_arr_size(
_temp104, 1u), _temp104.curr);})); _npop_handler( 0u); return; _LL84: goto _LL76;
_LL76:;}; _pop_handler();} else{ void* _temp73=( void*) _exn_thrown; void*
_temp106= _temp73; _LL108: if( _temp106 == Cyc_Dict_Absent){ goto _LL109;} else{
goto _LL110;} _LL110: goto _LL111; _LL109: goto _LL107; _LL111:( void) _throw(
_temp106); _LL107:;}}} Cyc_Tcutil_terr( p->loc,( struct _tagged_arr)({ struct
_tagged_arr _temp112= Cyc_Absynpp_qvar2string( _temp16); xprintf("%.*s is not a constructor in pattern",
_get_arr_size( _temp112, 1u), _temp112.curr);})); return; _LL7:{ struct
_handler_cons _temp113; _push_handler(& _temp113);{ int _temp115= 0; if( setjmp(
_temp113.handler)){ _temp115= 1;} if( ! _temp115){{ void* _temp116= Cyc_Tcenv_lookup_ordinary(
te, p->loc, _temp22); struct Cyc_Absyn_Structdecl* _temp122; _LL118: if(*(( int*)
_temp116) == Cyc_Tcenv_StructRes){ _LL123: _temp122=(( struct Cyc_Tcenv_StructRes_struct*)
_temp116)->f1; goto _LL119;} else{ goto _LL120;} _LL120: goto _LL121; _LL119:(
void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_p_struct* _temp124=(
struct Cyc_Absyn_Struct_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct));
_temp124[ 0]=({ struct Cyc_Absyn_Struct_p_struct _temp125; _temp125.tag= Cyc_Absyn_Struct_p;
_temp125.f1= _temp122; _temp125.f2= 0; _temp125.f3= _temp20; _temp125.f4=
_temp18; _temp125;}); _temp124;}))); _npop_handler( 0u); return; _LL121: goto
_LL117; _LL117:;}; _pop_handler();} else{ void* _temp114=( void*) _exn_thrown;
void* _temp127= _temp114; _LL129: if( _temp127 == Cyc_Dict_Absent){ goto _LL130;}
else{ goto _LL131;} _LL131: goto _LL132; _LL130: goto _LL128; _LL132:( void)
_throw( _temp127); _LL128:;}}} Cyc_Tcutil_terr( p->loc,( struct _tagged_arr)({
struct _tagged_arr _temp133= Cyc_Absynpp_qvar2string( _temp22); xprintf("%.*s is not a struct name, but pattern has designators",
_get_arr_size( _temp133, 1u), _temp133.curr);})); return; _LL9: return; _LL1:;}
static struct _tagged_arr* Cyc_Tcpat_get_name( struct Cyc_Absyn_Vardecl* vd){
return(* vd->name).f2;} static struct Cyc_Core_Opt Cyc_Tcpat_mk={( void*)(( void*)
1u)}; static struct Cyc_Core_Opt Cyc_Tcpat_rk={( void*)(( void*) 3u)}; static
struct Cyc_Core_Opt Cyc_Tcpat_ak={( void*)(( void*) 0u)}; static void* Cyc_Tcpat_any_type(
struct Cyc_List_List* s, void** topt){ if( topt != 0){ return*(( void**)
_check_null( topt));} return Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcpat_mk,({
struct Cyc_Core_Opt* _temp134=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp134->v=( void*) s; _temp134;}));} static void* Cyc_Tcpat_num_type(
void** topt, void* numt){ if( topt != 0? Cyc_Tcutil_coerceable(*(( void**)
_check_null( topt))): 0){ return*(( void**) _check_null( topt));}{ void*
_temp135= Cyc_Tcutil_compress( numt); struct Cyc_Absyn_Enumdecl* _temp141;
struct _tuple1* _temp143; _LL137: if(( unsigned int) _temp135 > 4u?*(( int*)
_temp135) == Cyc_Absyn_EnumType: 0){ _LL144: _temp143=(( struct Cyc_Absyn_EnumType_struct*)
_temp135)->f1; goto _LL142; _LL142: _temp141=(( struct Cyc_Absyn_EnumType_struct*)
_temp135)->f2; goto _LL138;} else{ goto _LL139;} _LL139: goto _LL140; _LL138:
if( topt != 0){ return*(( void**) _check_null( topt));} goto _LL136; _LL140:
goto _LL136; _LL136:;} return numt;} static void Cyc_Tcpat_set_vd( struct Cyc_Absyn_Vardecl*
vd, struct Cyc_List_List** v_result_ptr, void* t){( void*)( vd->type=( void*) t);
vd->tq= Cyc_Absyn_empty_tqual();* v_result_ptr=({ struct Cyc_List_List* _temp145=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp145->hd=(
void*) vd; _temp145->tl=* v_result_ptr; _temp145;});} struct _tuple8{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; struct _tuple9{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Pat*
f2; } ; struct _tuple6 Cyc_Tcpat_tcPatRec( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat*
p, void** topt, void** region_opt){ Cyc_Tcpat_resolve_pat( te, p);{ void* t;
struct Cyc_List_List* tv_result= 0; struct Cyc_List_List* v_result= 0;{ void*
_temp146=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp180; struct Cyc_Absyn_Vardecl*
_temp182; int _temp184; void* _temp186; int _temp188; void* _temp190;
unsigned char _temp192; struct _tagged_arr _temp194; struct Cyc_Absyn_Enumfield*
_temp196; struct Cyc_Absyn_Enumdecl* _temp198; struct Cyc_Absyn_Pat* _temp200;
struct Cyc_List_List* _temp202; struct Cyc_List_List* _temp204; struct Cyc_List_List*
_temp206; struct Cyc_Core_Opt* _temp208; struct Cyc_Core_Opt** _temp210; struct
Cyc_Absyn_Structdecl* _temp211; struct Cyc_List_List* _temp213; struct Cyc_List_List*
_temp215; struct Cyc_Absyn_Tunionfield* _temp217; struct Cyc_Absyn_Tuniondecl*
_temp219; struct _tuple1* _temp221; struct Cyc_List_List* _temp223; struct Cyc_List_List*
_temp225; struct _tuple1* _temp227; struct Cyc_List_List* _temp229; struct Cyc_List_List*
_temp231; struct _tuple1* _temp233; _LL148: if( _temp146 ==( void*) Cyc_Absyn_Wild_p){
goto _LL149;} else{ goto _LL150;} _LL150: if(( unsigned int) _temp146 > 2u?*((
int*) _temp146) == Cyc_Absyn_Var_p: 0){ _LL181: _temp180=(( struct Cyc_Absyn_Var_p_struct*)
_temp146)->f1; goto _LL151;} else{ goto _LL152;} _LL152: if(( unsigned int)
_temp146 > 2u?*(( int*) _temp146) == Cyc_Absyn_Reference_p: 0){ _LL183: _temp182=((
struct Cyc_Absyn_Reference_p_struct*) _temp146)->f1; goto _LL153;} else{ goto
_LL154;} _LL154: if(( unsigned int) _temp146 > 2u?*(( int*) _temp146) == Cyc_Absyn_Int_p:
0){ _LL187: _temp186=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp146)->f1;
if( _temp186 ==( void*) Cyc_Absyn_Unsigned){ goto _LL185;} else{ goto _LL156;}
_LL185: _temp184=(( struct Cyc_Absyn_Int_p_struct*) _temp146)->f2; goto _LL155;}
else{ goto _LL156;} _LL156: if(( unsigned int) _temp146 > 2u?*(( int*) _temp146)
== Cyc_Absyn_Int_p: 0){ _LL191: _temp190=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp146)->f1; if( _temp190 ==( void*) Cyc_Absyn_Signed){ goto _LL189;} else{
goto _LL158;} _LL189: _temp188=(( struct Cyc_Absyn_Int_p_struct*) _temp146)->f2;
goto _LL157;} else{ goto _LL158;} _LL158: if(( unsigned int) _temp146 > 2u?*((
int*) _temp146) == Cyc_Absyn_Char_p: 0){ _LL193: _temp192=(( struct Cyc_Absyn_Char_p_struct*)
_temp146)->f1; goto _LL159;} else{ goto _LL160;} _LL160: if(( unsigned int)
_temp146 > 2u?*(( int*) _temp146) == Cyc_Absyn_Float_p: 0){ _LL195: _temp194=((
struct Cyc_Absyn_Float_p_struct*) _temp146)->f1; goto _LL161;} else{ goto _LL162;}
_LL162: if(( unsigned int) _temp146 > 2u?*(( int*) _temp146) == Cyc_Absyn_Enum_p:
0){ _LL199: _temp198=(( struct Cyc_Absyn_Enum_p_struct*) _temp146)->f1; goto
_LL197; _LL197: _temp196=(( struct Cyc_Absyn_Enum_p_struct*) _temp146)->f2; goto
_LL163;} else{ goto _LL164;} _LL164: if( _temp146 ==( void*) Cyc_Absyn_Null_p){
goto _LL165;} else{ goto _LL166;} _LL166: if(( unsigned int) _temp146 > 2u?*((
int*) _temp146) == Cyc_Absyn_Pointer_p: 0){ _LL201: _temp200=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp146)->f1; goto _LL167;} else{ goto _LL168;} _LL168: if(( unsigned int)
_temp146 > 2u?*(( int*) _temp146) == Cyc_Absyn_Tuple_p: 0){ _LL203: _temp202=((
struct Cyc_Absyn_Tuple_p_struct*) _temp146)->f1; goto _LL169;} else{ goto _LL170;}
_LL170: if(( unsigned int) _temp146 > 2u?*(( int*) _temp146) == Cyc_Absyn_Struct_p:
0){ _LL212: _temp211=(( struct Cyc_Absyn_Struct_p_struct*) _temp146)->f1; goto
_LL209; _LL209: _temp208=(( struct Cyc_Absyn_Struct_p_struct*) _temp146)->f2;
_temp210=&(( struct Cyc_Absyn_Struct_p_struct*) _temp146)->f2; goto _LL207;
_LL207: _temp206=(( struct Cyc_Absyn_Struct_p_struct*) _temp146)->f3; goto
_LL205; _LL205: _temp204=(( struct Cyc_Absyn_Struct_p_struct*) _temp146)->f4;
goto _LL171;} else{ goto _LL172;} _LL172: if(( unsigned int) _temp146 > 2u?*((
int*) _temp146) == Cyc_Absyn_Tunion_p: 0){ _LL220: _temp219=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp146)->f1; goto _LL218; _LL218: _temp217=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp146)->f2; goto _LL216; _LL216: _temp215=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp146)->f3; goto _LL214; _LL214: _temp213=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp146)->f4; goto _LL173;} else{ goto _LL174;} _LL174: if(( unsigned int)
_temp146 > 2u?*(( int*) _temp146) == Cyc_Absyn_UnknownId_p: 0){ _LL222: _temp221=((
struct Cyc_Absyn_UnknownId_p_struct*) _temp146)->f1; goto _LL175;} else{ goto
_LL176;} _LL176: if(( unsigned int) _temp146 > 2u?*(( int*) _temp146) == Cyc_Absyn_UnknownCall_p:
0){ _LL228: _temp227=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp146)->f1;
goto _LL226; _LL226: _temp225=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp146)->f2; goto _LL224; _LL224: _temp223=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp146)->f3; goto _LL177;} else{ goto _LL178;} _LL178: if(( unsigned int)
_temp146 > 2u?*(( int*) _temp146) == Cyc_Absyn_UnknownFields_p: 0){ _LL234:
_temp233=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp146)->f1; goto _LL232;
_LL232: _temp231=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp146)->f2;
goto _LL230; _LL230: _temp229=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp146)->f3; goto _LL179;} else{ goto _LL147;} _LL149: t= Cyc_Tcpat_any_type(
Cyc_Tcenv_lookup_type_vars( te), topt); goto _LL147; _LL151: t= Cyc_Tcpat_any_type(
Cyc_Tcenv_lookup_type_vars( te), topt); Cyc_Tcpat_set_vd( _temp180,& v_result, t);
goto _LL147; _LL153: t= Cyc_Tcpat_any_type( Cyc_Tcenv_lookup_type_vars( te),
topt); if( region_opt == 0){ Cyc_Tcutil_terr( p->loc, _tag_arr("* pattern would point into an unknown region",
sizeof( unsigned char), 45u)); goto _LL147;} Cyc_Tcpat_set_vd( _temp182,&
v_result,( void*)({ struct Cyc_Absyn_PointerType_struct* _temp235=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp235[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp236; _temp236.tag= Cyc_Absyn_PointerType;
_temp236.f1=({ struct Cyc_Absyn_PtrInfo _temp237; _temp237.elt_typ=( void*) t;
_temp237.rgn_typ=( void*)*(( void**) _check_null( region_opt)); _temp237.nullable=((
struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)( 0); _temp237.tq= Cyc_Absyn_empty_tqual();
_temp237.bounds=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp237;}); _temp236;}); _temp235;})); goto _LL147; _LL155: t= Cyc_Tcpat_num_type(
topt, Cyc_Absyn_uint_t); goto _LL147; _LL157: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_sint_t);
goto _LL147; _LL159: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_uchar_t); goto
_LL147; _LL161: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_float_t); goto _LL147;
_LL163: t= Cyc_Tcpat_num_type( topt,( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp238=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp238[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp239; _temp239.tag= Cyc_Absyn_EnumType;
_temp239.f1= _temp198->name; _temp239.f2=( struct Cyc_Absyn_Enumdecl*) _temp198;
_temp239;}); _temp238;})); goto _LL147; _LL165: if( topt != 0){ void* _temp240=
Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo
_temp246; _LL242: if(( unsigned int) _temp240 > 4u?*(( int*) _temp240) == Cyc_Absyn_PointerType:
0){ _LL247: _temp246=(( struct Cyc_Absyn_PointerType_struct*) _temp240)->f1;
goto _LL243;} else{ goto _LL244;} _LL244: goto _LL245; _LL243: t=*(( void**)
_check_null( topt)); goto tcpat_end; _LL245: goto _LL241; _LL241:;}{ struct Cyc_List_List*
_temp248= Cyc_Tcenv_lookup_type_vars( te); t=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp249=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp249[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp250; _temp250.tag= Cyc_Absyn_PointerType;
_temp250.f1=({ struct Cyc_Absyn_PtrInfo _temp251; _temp251.elt_typ=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcpat_ak,({ struct Cyc_Core_Opt* _temp253=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp253->v=( void*)
_temp248; _temp253;})); _temp251.rgn_typ=( void*) Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Tcpat_rk,({ struct Cyc_Core_Opt* _temp252=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp252->v=( void*) _temp248;
_temp252;})); _temp251.nullable=(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
1); _temp251.tq= Cyc_Absyn_empty_tqual(); _temp251.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp251;}); _temp250;}); _temp249;}); goto _LL147;}
_LL167: { void* inner_typ=( void*) Cyc_Absyn_VoidType; void** _temp254= 0; if(
topt != 0){ void* _temp255= Cyc_Tcutil_compress(*(( void**) _check_null( topt)));
struct Cyc_Absyn_PtrInfo _temp261; struct Cyc_Absyn_Conref* _temp263; struct Cyc_Absyn_Tqual
_temp265; struct Cyc_Absyn_Conref* _temp267; void* _temp269; void* _temp271;
_LL257: if(( unsigned int) _temp255 > 4u?*(( int*) _temp255) == Cyc_Absyn_PointerType:
0){ _LL262: _temp261=(( struct Cyc_Absyn_PointerType_struct*) _temp255)->f1;
_LL272: _temp271=( void*) _temp261.elt_typ; goto _LL270; _LL270: _temp269=( void*)
_temp261.rgn_typ; goto _LL268; _LL268: _temp267= _temp261.nullable; goto _LL266;
_LL266: _temp265= _temp261.tq; goto _LL264; _LL264: _temp263= _temp261.bounds;
goto _LL258;} else{ goto _LL259;} _LL259: goto _LL260; _LL258: inner_typ=
_temp271; _temp254=( void**)& inner_typ; goto _LL256; _LL260: goto _LL256;
_LL256:;}{ void* ptr_rgn= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcpat_rk,({
struct Cyc_Core_Opt* _temp306=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp306->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp306;}));
struct Cyc_List_List* _temp275; struct Cyc_List_List* _temp277; struct _tuple6
_temp273= Cyc_Tcpat_tcPatRec( te, _temp200, _temp254,( void**)& ptr_rgn); _LL278:
_temp277= _temp273.f1; goto _LL276; _LL276: _temp275= _temp273.f2; goto _LL274;
_LL274: tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp277); v_result=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
v_result, _temp275); if( _temp254 == 0){ void* _temp279= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp200->topt))->v); struct Cyc_Absyn_TunionFieldInfo
_temp285; struct Cyc_List_List* _temp287; void* _temp289; struct Cyc_Absyn_Tunionfield*
_temp291; struct Cyc_Absyn_Tuniondecl* _temp293; _LL281: if(( unsigned int)
_temp279 > 4u?*(( int*) _temp279) == Cyc_Absyn_TunionFieldType: 0){ _LL286:
_temp285=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp279)->f1; _LL290:
_temp289=( void*) _temp285.field_info; if(*(( int*) _temp289) == Cyc_Absyn_KnownTunionfield){
_LL294: _temp293=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp289)->f1;
goto _LL292; _LL292: _temp291=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp289)->f2; goto _LL288;} else{ goto _LL283;} _LL288: _temp287= _temp285.targs;
goto _LL282;} else{ goto _LL283;} _LL283: goto _LL284; _LL282: t=( void*)({
struct Cyc_Absyn_TunionType_struct* _temp295=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp295[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp296; _temp296.tag= Cyc_Absyn_TunionType;
_temp296.f1=({ struct Cyc_Absyn_TunionInfo _temp297; _temp297.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp298=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp298[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp299; _temp299.tag= Cyc_Absyn_KnownTunion;
_temp299.f1= _temp293; _temp299;}); _temp298;})); _temp297.targs= _temp287;
_temp297.rgn=( void*) ptr_rgn; _temp297;}); _temp296;}); _temp295;}); goto
_LL280; _LL284: t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp300=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp300[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp301; _temp301.tag= Cyc_Absyn_PointerType;
_temp301.f1=({ struct Cyc_Absyn_PtrInfo _temp302; _temp302.elt_typ=( void*)((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp200->topt))->v); _temp302.rgn_typ=(
void*) ptr_rgn; _temp302.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp302.tq= Cyc_Absyn_empty_tqual(); _temp302.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp302;}); _temp301;}); _temp300;}); goto _LL280;
_LL280:;} else{ t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp303=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp303[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp304; _temp304.tag= Cyc_Absyn_PointerType;
_temp304.f1=({ struct Cyc_Absyn_PtrInfo _temp305; _temp305.elt_typ=( void*)((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp200->topt))->v); _temp305.rgn_typ=(
void*) ptr_rgn; _temp305.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp305.tq= Cyc_Absyn_empty_tqual(); _temp305.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp305;}); _temp304;}); _temp303;});} goto _LL147;}}
_LL169: { struct Cyc_List_List* pat_ts= 0; struct Cyc_List_List* topt_ts= 0; if(
topt != 0){ void* _temp307= Cyc_Tcutil_compress(*(( void**) _check_null( topt)));
struct Cyc_List_List* _temp313; _LL309: if(( unsigned int) _temp307 > 4u?*(( int*)
_temp307) == Cyc_Absyn_TupleType: 0){ _LL314: _temp313=(( struct Cyc_Absyn_TupleType_struct*)
_temp307)->f1; goto _LL310;} else{ goto _LL311;} _LL311: goto _LL312; _LL310:
topt_ts= _temp313; goto _LL308; _LL312: goto _LL308; _LL308:;} for( 0; _temp202
!= 0; _temp202=(( struct Cyc_List_List*) _check_null( _temp202))->tl){ void**
_temp315= 0; if( topt_ts != 0){ _temp315=( void**)&(*(( struct _tuple8*)((
struct Cyc_List_List*) _check_null( topt_ts))->hd)).f2; topt_ts=(( struct Cyc_List_List*)
_check_null( topt_ts))->tl;}{ struct Cyc_List_List* _temp318; struct Cyc_List_List*
_temp320; struct _tuple6 _temp316= Cyc_Tcpat_tcPatRec( te,( struct Cyc_Absyn_Pat*)((
struct Cyc_List_List*) _check_null( _temp202))->hd, _temp315, region_opt);
_LL321: _temp320= _temp316.f1; goto _LL319; _LL319: _temp318= _temp316.f2; goto
_LL317; _LL317: tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp320); v_result=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
v_result, _temp318); pat_ts=({ struct Cyc_List_List* _temp322=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp322->hd=( void*)({ struct
_tuple8* _temp323=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8));
_temp323->f1= Cyc_Absyn_empty_tqual(); _temp323->f2=( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null(
_temp202))->hd)->topt))->v; _temp323;}); _temp322->tl= pat_ts; _temp322;});}} t=(
void*)({ struct Cyc_Absyn_TupleType_struct* _temp324=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp324[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp325; _temp325.tag= Cyc_Absyn_TupleType; _temp325.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( pat_ts);
_temp325;}); _temp324;}); goto _LL147;} _LL171: if( _temp206 != 0){( int) _throw(((
void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("tcPat: struct<...> not implemented",
sizeof( unsigned char), 35u)));}{ struct _RegionHandle _temp326= _new_region();
struct _RegionHandle* rgn=& _temp326; _push_region( rgn);{ struct _tuple4
_temp327=({ struct _tuple4 _temp351; _temp351.f1= Cyc_Tcenv_lookup_type_vars( te);
_temp351.f2= rgn; _temp351;}); struct Cyc_List_List* _temp328=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp327, _temp211->tvs); struct Cyc_List_List* _temp329=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp328); t=( void*)({ struct Cyc_Absyn_StructType_struct*
_temp330=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp330[ 0]=({ struct Cyc_Absyn_StructType_struct _temp331; _temp331.tag= Cyc_Absyn_StructType;
_temp331.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp211->name))->v); _temp331.f2= _temp329; _temp331.f3=({ struct
Cyc_Absyn_Structdecl** _temp332=( struct Cyc_Absyn_Structdecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl*)); _temp332[ 0]= _temp211; _temp332;});
_temp331;}); _temp330;}); if( topt != 0){ Cyc_Tcutil_unify( t,*(( void**)
_check_null( topt)));}* _temp210=({ struct Cyc_Core_Opt* _temp333=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp333->v=( void*) _temp329;
_temp333;}); if( _temp211->fields == 0){ Cyc_Tcutil_terr( p->loc, _tag_arr("can't destructure an abstract struct",
sizeof( unsigned char), 37u)); _npop_handler( 0u); goto _LL147;}{ struct Cyc_List_List*
fields=(( struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, p->loc, _temp204,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp211->fields))->v); for( 0; fields != 0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ struct _tuple9 _temp336; struct Cyc_Absyn_Pat*
_temp337; struct Cyc_Absyn_Structfield* _temp339; struct _tuple9* _temp334=(
struct _tuple9*)(( struct Cyc_List_List*) _check_null( fields))->hd; _temp336=*
_temp334; _LL340: _temp339= _temp336.f1; goto _LL338; _LL338: _temp337= _temp336.f2;
goto _LL335; _LL335: { void* _temp341= Cyc_Tcutil_rsubstitute( rgn, _temp328,(
void*) _temp339->type); struct Cyc_List_List* _temp344; struct Cyc_List_List*
_temp346; struct _tuple6 _temp342= Cyc_Tcpat_tcPatRec( te, _temp337,( void**)&
_temp341, region_opt); _LL347: _temp346= _temp342.f1; goto _LL345; _LL345:
_temp344= _temp342.f2; goto _LL343; _LL343: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp346); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp344); if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp337->topt))->v, _temp341)){ Cyc_Tcutil_terr(
p->loc,( struct _tagged_arr)({ struct _tagged_arr _temp348=* _temp339->name;
struct _tagged_arr _temp349= Cyc_Absynpp_typ2string( _temp341); struct
_tagged_arr _temp350= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp337->topt))->v); xprintf("field %.*s of struct pattern expects type %.*s != %.*s",
_get_arr_size( _temp348, 1u), _temp348.curr, _get_arr_size( _temp349, 1u),
_temp349.curr, _get_arr_size( _temp350, 1u), _temp350.curr);}));}}}}};
_pop_region( rgn);} goto _LL147; _LL173: if((( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp215) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp217->tvs)){ Cyc_Tcutil_terr( p->loc, _tag_arr("wrong number of existential type variables in pattern",
sizeof( unsigned char), 54u));}{ struct _RegionHandle _temp352= _new_region();
struct _RegionHandle* rgn=& _temp352; _push_region( rgn);{ struct Cyc_List_List*
_temp353= 0;{ struct Cyc_List_List* t= _temp215; for( 0; t != 0; t=(( struct Cyc_List_List*)
_check_null( t))->tl){ struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( t))->hd;{ void* _temp354=( void*)((( struct
Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
tv->kind))->v; void* _temp364; void* _temp366; struct Cyc_Absyn_Conref* _temp368;
_LL356: if(( unsigned int) _temp354 > 1u?*(( int*) _temp354) == Cyc_Absyn_Eq_constr:
0){ _LL365: _temp364=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp354)->f1;
if( _temp364 ==( void*) Cyc_Absyn_BoxKind){ goto _LL357;} else{ goto _LL358;}}
else{ goto _LL358;} _LL358: if(( unsigned int) _temp354 > 1u?*(( int*) _temp354)
== Cyc_Absyn_Eq_constr: 0){ _LL367: _temp366=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp354)->f1; goto _LL359;} else{ goto _LL360;} _LL360: if( _temp354 ==( void*)
Cyc_Absyn_No_constr){ goto _LL361;} else{ goto _LL362;} _LL362: if((
unsigned int) _temp354 > 1u?*(( int*) _temp354) == Cyc_Absyn_Forward_constr: 0){
_LL369: _temp368=(( struct Cyc_Absyn_Forward_constr_struct*) _temp354)->f1; goto
_LL363;} else{ goto _LL355;} _LL357: goto _LL355; _LL359: Cyc_Tcutil_terr( p->loc,(
struct _tagged_arr)({ struct _tagged_arr _temp370= Cyc_Absynpp_qvar2string(
_temp219->name); struct _tagged_arr _temp371=* tv->name; struct _tagged_arr
_temp372= Cyc_Absynpp_kind2string( _temp366); xprintf("tunion member %.*s abstracts type variable %.*s of kind %.*s != B",
_get_arr_size( _temp370, 1u), _temp370.curr, _get_arr_size( _temp371, 1u),
_temp371.curr, _get_arr_size( _temp372, 1u), _temp372.curr);})); goto _LL355;
_LL361:( void*)(( tv->kind)->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp373=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp373[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp374; _temp374.tag= Cyc_Absyn_Eq_constr;
_temp374.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp374;}); _temp373;})));
goto _LL355; _LL363:( int) _throw((( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("forward after compress in conref", sizeof( unsigned char), 33u)));
_LL355:;} _temp353=({ struct Cyc_List_List* _temp375=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp375->hd=( void*)((
void*)({ struct Cyc_Absyn_VarType_struct* _temp376=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp376[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp377; _temp377.tag= Cyc_Absyn_VarType; _temp377.f1= tv; _temp377;});
_temp376;})); _temp375->tl= _temp353; _temp375;});}} _temp353=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp353); tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp215);{ struct Cyc_Tcenv_Tenv* te2= Cyc_Tcenv_add_type_vars( p->loc, te,
_temp215); struct Cyc_List_List* tqts= _temp217->typs; struct Cyc_List_List*
_temp378= Cyc_Tcenv_lookup_type_vars( te); struct _tuple4 _temp379=({ struct
_tuple4 _temp433; _temp433.f1= _temp378; _temp433.f2= rgn; _temp433;}); struct
Cyc_List_List* _temp380=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*), struct _tuple4*
env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp379, _temp219->tvs); struct Cyc_List_List* _temp381=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp217->tvs, _temp353);
struct Cyc_List_List* _temp382=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple5*))
Cyc_Core_snd, _temp380); struct Cyc_List_List* _temp383=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp381); struct Cyc_List_List* _temp384=((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp380, _temp381); if( tqts == 0){
t=( void*)({ struct Cyc_Absyn_TunionType_struct* _temp385=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp385[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp386; _temp386.tag= Cyc_Absyn_TunionType;
_temp386.f1=({ struct Cyc_Absyn_TunionInfo _temp387; _temp387.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp389=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp389[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp390; _temp390.tag= Cyc_Absyn_KnownTunion;
_temp390.f1= _temp219; _temp390;}); _temp389;})); _temp387.targs= _temp382;
_temp387.rgn=( void*) Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcpat_rk,({
struct Cyc_Core_Opt* _temp388=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp388->v=( void*) _temp378; _temp388;})); _temp387;});
_temp386;}); _temp385;});} else{ t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp391=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp391[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp392; _temp392.tag= Cyc_Absyn_TunionFieldType; _temp392.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp393; _temp393.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp394=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp394[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp395; _temp395.tag= Cyc_Absyn_KnownTunionfield; _temp395.f1= _temp219;
_temp395.f2= _temp217; _temp395;}); _temp394;})); _temp393.targs= _temp382;
_temp393;}); _temp392;}); _temp391;});} if( topt != 0? tqts == 0: 0){ void*
_temp396= Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_TunionFieldInfo
_temp404; struct Cyc_Absyn_TunionInfo _temp406; void* _temp408; struct Cyc_List_List*
_temp410; void* _temp412; _LL398: if(( unsigned int) _temp396 > 4u?*(( int*)
_temp396) == Cyc_Absyn_TunionFieldType: 0){ _LL405: _temp404=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp396)->f1; goto _LL399;} else{ goto _LL400;} _LL400: if(( unsigned int)
_temp396 > 4u?*(( int*) _temp396) == Cyc_Absyn_TunionType: 0){ _LL407: _temp406=((
struct Cyc_Absyn_TunionType_struct*) _temp396)->f1; _LL413: _temp412=( void*)
_temp406.tunion_info; goto _LL411; _LL411: _temp410= _temp406.targs; goto _LL409;
_LL409: _temp408=( void*) _temp406.rgn; goto _LL401;} else{ goto _LL402;} _LL402:
goto _LL403; _LL399: t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp414=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp414[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp415; _temp415.tag= Cyc_Absyn_TunionFieldType; _temp415.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp416; _temp416.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp417=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp417[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp418; _temp418.tag= Cyc_Absyn_KnownTunionfield; _temp418.f1= _temp219;
_temp418.f2= _temp217; _temp418;}); _temp417;})); _temp416.targs= _temp382;
_temp416;}); _temp415;}); _temp414;}); goto _LL397; _LL401: { struct Cyc_List_List*
_temp419= _temp382; for( 0; _temp419 != 0? _temp410 != 0: 0;( _temp419=(( struct
Cyc_List_List*) _check_null( _temp419))->tl, _temp410=(( struct Cyc_List_List*)
_check_null( _temp410))->tl)){ Cyc_Tcutil_unify(( void*)(( struct Cyc_List_List*)
_check_null( _temp419))->hd,( void*)(( struct Cyc_List_List*) _check_null(
_temp410))->hd);} goto _LL397;} _LL403: goto _LL397; _LL397:;} for( 0; _temp213
!= 0? tqts != 0: 0;( _temp213=(( struct Cyc_List_List*) _check_null( _temp213))->tl,
tqts=(( struct Cyc_List_List*) _check_null( tqts))->tl)){ struct Cyc_Absyn_Pat*
_temp420=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null( _temp213))->hd;
void* _temp421= Cyc_Tcutil_rsubstitute( rgn, _temp384,(*(( struct _tuple8*)((
struct Cyc_List_List*) _check_null( tqts))->hd)).f2); struct Cyc_List_List*
_temp424; struct Cyc_List_List* _temp426; struct _tuple6 _temp422= Cyc_Tcpat_tcPatRec(
te2, _temp420,( void**)& _temp421, region_opt); _LL427: _temp426= _temp422.f1;
goto _LL425; _LL425: _temp424= _temp422.f2; goto _LL423; _LL423: tv_result=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tv_result, _temp426); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( v_result, _temp424); if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp420->topt))->v, _temp421)){ Cyc_Tcutil_terr(
_temp420->loc,( struct _tagged_arr)({ struct _tagged_arr _temp428= Cyc_Absynpp_qvar2string(
_temp219->name); struct _tagged_arr _temp429= Cyc_Absynpp_typ2string( _temp421);
struct _tagged_arr _temp430= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp420->topt))->v); xprintf("%.*s expects argument type %.*s, not %.*s",
_get_arr_size( _temp428, 1u), _temp428.curr, _get_arr_size( _temp429, 1u),
_temp429.curr, _get_arr_size( _temp430, 1u), _temp430.curr);}));}} if( _temp213
!= 0){ Cyc_Tcutil_terr( p->loc,( struct _tagged_arr)({ struct _tagged_arr
_temp431= Cyc_Absynpp_qvar2string( _temp219->name); xprintf("too many arguments for tunion constructor %.*s",
_get_arr_size( _temp431, 1u), _temp431.curr);}));} if( tqts != 0){ Cyc_Tcutil_terr(
p->loc,( struct _tagged_arr)({ struct _tagged_arr _temp432= Cyc_Absynpp_qvar2string(
_temp219->name); xprintf("too few arguments for tunion constructor %.*s",
_get_arr_size( _temp432, 1u), _temp432.curr);}));}}}; _pop_region( rgn);} goto
_LL147; _LL175: goto _LL177; _LL177: goto _LL179; _LL179: t= Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt* _temp434=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp434->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp434;}));
goto _LL147; _LL147:;} tcpat_end: p->topt=({ struct Cyc_Core_Opt* _temp435=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp435->v=(
void*) t; _temp435;}); return({ struct _tuple6 _temp436; _temp436.f1= tv_result;
_temp436.f2= v_result; _temp436;});}} struct _tuple6 Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p, void** topt, void** region_opt){ struct _tuple6
_temp437= Cyc_Tcpat_tcPatRec( te, p, topt, region_opt);{ struct _RegionHandle
_temp438= _new_region(); struct _RegionHandle* r=& _temp438; _push_region( r);
Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tagged_arr*(* f)( struct Cyc_Absyn_Vardecl*), struct Cyc_List_List* x))
Cyc_List_rmap)( r, Cyc_Tcpat_get_name, _temp437.f2), p->loc, _tag_arr("pattern contains a repeated variable",
sizeof( unsigned char), 37u));; _pop_region( r);} return _temp437;} void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p){ void* _temp439=( void*) p->r;
struct Cyc_Absyn_Pat* _temp451; struct Cyc_List_List* _temp453; struct Cyc_List_List*
_temp455; struct Cyc_Absyn_Tunionfield* _temp457; struct Cyc_Absyn_Tuniondecl*
_temp459; struct Cyc_List_List* _temp461; struct Cyc_List_List* _temp463; struct
Cyc_Core_Opt* _temp465; struct Cyc_Absyn_Structdecl* _temp467; struct Cyc_List_List*
_temp469; _LL441: if(( unsigned int) _temp439 > 2u?*(( int*) _temp439) == Cyc_Absyn_Pointer_p:
0){ _LL452: _temp451=(( struct Cyc_Absyn_Pointer_p_struct*) _temp439)->f1; goto
_LL442;} else{ goto _LL443;} _LL443: if(( unsigned int) _temp439 > 2u?*(( int*)
_temp439) == Cyc_Absyn_Tunion_p: 0){ _LL460: _temp459=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp439)->f1; goto _LL458; _LL458: _temp457=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp439)->f2; goto _LL456; _LL456: _temp455=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp439)->f3; goto _LL454; _LL454: _temp453=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp439)->f4; goto _LL444;} else{ goto _LL445;} _LL445: if(( unsigned int)
_temp439 > 2u?*(( int*) _temp439) == Cyc_Absyn_Struct_p: 0){ _LL468: _temp467=((
struct Cyc_Absyn_Struct_p_struct*) _temp439)->f1; goto _LL466; _LL466: _temp465=((
struct Cyc_Absyn_Struct_p_struct*) _temp439)->f2; goto _LL464; _LL464: _temp463=((
struct Cyc_Absyn_Struct_p_struct*) _temp439)->f3; goto _LL462; _LL462: _temp461=((
struct Cyc_Absyn_Struct_p_struct*) _temp439)->f4; goto _LL446;} else{ goto
_LL447;} _LL447: if(( unsigned int) _temp439 > 2u?*(( int*) _temp439) == Cyc_Absyn_Tuple_p:
0){ _LL470: _temp469=(( struct Cyc_Absyn_Tuple_p_struct*) _temp439)->f1; goto
_LL448;} else{ goto _LL449;} _LL449: goto _LL450; _LL442: Cyc_Tcpat_check_pat_regions(
te, _temp451);{ void* _temp471=( void*)(( struct Cyc_Core_Opt*) _check_null( p->topt))->v;
struct Cyc_Absyn_PtrInfo _temp479; struct Cyc_Absyn_Conref* _temp481; struct Cyc_Absyn_Tqual
_temp483; struct Cyc_Absyn_Conref* _temp485; void* _temp487; void* _temp489;
struct Cyc_Absyn_TunionInfo _temp491; void* _temp493; struct Cyc_List_List*
_temp495; void* _temp497; _LL473: if(( unsigned int) _temp471 > 4u?*(( int*)
_temp471) == Cyc_Absyn_PointerType: 0){ _LL480: _temp479=(( struct Cyc_Absyn_PointerType_struct*)
_temp471)->f1; _LL490: _temp489=( void*) _temp479.elt_typ; goto _LL488; _LL488:
_temp487=( void*) _temp479.rgn_typ; goto _LL486; _LL486: _temp485= _temp479.nullable;
goto _LL484; _LL484: _temp483= _temp479.tq; goto _LL482; _LL482: _temp481=
_temp479.bounds; goto _LL474;} else{ goto _LL475;} _LL475: if(( unsigned int)
_temp471 > 4u?*(( int*) _temp471) == Cyc_Absyn_TunionType: 0){ _LL492: _temp491=((
struct Cyc_Absyn_TunionType_struct*) _temp471)->f1; _LL498: _temp497=( void*)
_temp491.tunion_info; goto _LL496; _LL496: _temp495= _temp491.targs; goto _LL494;
_LL494: _temp493=( void*) _temp491.rgn; goto _LL476;} else{ goto _LL477;} _LL477:
goto _LL478; _LL474: _temp493= _temp487; goto _LL476; _LL476: Cyc_Tcenv_check_rgn_accessible(
te, p->loc, _temp493); return; _LL478:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp499=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp499[ 0]=({ struct Cyc_Core_Impossible_struct _temp500; _temp500.tag= Cyc_Core_Impossible;
_temp500.f1= _tag_arr("check_pat_regions: bad pointer type", sizeof(
unsigned char), 36u); _temp500;}); _temp499;})); _LL472:;} _LL444: for( 0;
_temp453 != 0; _temp453=(( struct Cyc_List_List*) _check_null( _temp453))->tl){
Cyc_Tcpat_check_pat_regions( te,( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp453))->hd);}{ void* _temp501=( void*)(( struct Cyc_Core_Opt*)
_check_null( p->topt))->v; struct Cyc_Absyn_TunionInfo _temp509; void* _temp511;
struct Cyc_List_List* _temp513; void* _temp515; struct Cyc_Absyn_TunionFieldInfo
_temp517; _LL503: if(( unsigned int) _temp501 > 4u?*(( int*) _temp501) == Cyc_Absyn_TunionType:
0){ _LL510: _temp509=(( struct Cyc_Absyn_TunionType_struct*) _temp501)->f1;
_LL516: _temp515=( void*) _temp509.tunion_info; goto _LL514; _LL514: _temp513=
_temp509.targs; goto _LL512; _LL512: _temp511=( void*) _temp509.rgn; goto _LL504;}
else{ goto _LL505;} _LL505: if(( unsigned int) _temp501 > 4u?*(( int*) _temp501)
== Cyc_Absyn_TunionFieldType: 0){ _LL518: _temp517=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp501)->f1; goto _LL506;} else{ goto _LL507;} _LL507: goto _LL508; _LL504:
Cyc_Tcenv_check_rgn_accessible( te, p->loc, _temp511); return; _LL506: return;
_LL508:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp519=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp519[ 0]=({ struct Cyc_Core_Impossible_struct _temp520; _temp520.tag= Cyc_Core_Impossible;
_temp520.f1= _tag_arr("check_pat_regions: bad tunion type", sizeof(
unsigned char), 35u); _temp520;}); _temp519;})); _LL502:;} _LL446: _temp469=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Pat*(* f)( struct _tuple7*), struct
Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Pat*(*)( struct _tuple7*))
Cyc_Core_snd, _temp461); goto _LL448; _LL448: for( 0; _temp469 != 0; _temp469=((
struct Cyc_List_List*) _check_null( _temp469))->tl){ Cyc_Tcpat_check_pat_regions(
te,( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null( _temp469))->hd);}
return; _LL450: return; _LL440:;} static const int Cyc_Tcpat_Name_v= 0; struct
Cyc_Tcpat_Name_v_struct{ int tag; struct _tagged_arr f1; } ; static const int
Cyc_Tcpat_Int_v= 1; struct Cyc_Tcpat_Int_v_struct{ int tag; int f1; } ; struct
Cyc_Tcpat_Con_s{ void* name; int arity; struct Cyc_Core_Opt* span; } ; static
const int Cyc_Tcpat_Any= 0; static const int Cyc_Tcpat_Con= 0; struct Cyc_Tcpat_Con_struct{
int tag; struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2; } ; static int
Cyc_Tcpat_compare_con( struct Cyc_Tcpat_Con_s* c1, struct Cyc_Tcpat_Con_s* c2){
void* _temp521=( void*) c1->name; struct _tagged_arr _temp527; int _temp529;
_LL523: if(*(( int*) _temp521) == Cyc_Tcpat_Name_v){ _LL528: _temp527=(( struct
Cyc_Tcpat_Name_v_struct*) _temp521)->f1; goto _LL524;} else{ goto _LL525;}
_LL525: if(*(( int*) _temp521) == Cyc_Tcpat_Int_v){ _LL530: _temp529=(( struct
Cyc_Tcpat_Int_v_struct*) _temp521)->f1; goto _LL526;} else{ goto _LL522;} _LL524: {
void* _temp531=( void*) c2->name; struct _tagged_arr _temp537; int _temp539;
_LL533: if(*(( int*) _temp531) == Cyc_Tcpat_Name_v){ _LL538: _temp537=(( struct
Cyc_Tcpat_Name_v_struct*) _temp531)->f1; goto _LL534;} else{ goto _LL535;}
_LL535: if(*(( int*) _temp531) == Cyc_Tcpat_Int_v){ _LL540: _temp539=(( struct
Cyc_Tcpat_Int_v_struct*) _temp531)->f1; goto _LL536;} else{ goto _LL532;} _LL534:
return Cyc_String_zstrcmp( _temp527, _temp537); _LL536: return - 1; _LL532:;}
_LL526: { void* _temp541=( void*) c2->name; struct _tagged_arr _temp547; int
_temp549; _LL543: if(*(( int*) _temp541) == Cyc_Tcpat_Name_v){ _LL548: _temp547=((
struct Cyc_Tcpat_Name_v_struct*) _temp541)->f1; goto _LL544;} else{ goto _LL545;}
_LL545: if(*(( int*) _temp541) == Cyc_Tcpat_Int_v){ _LL550: _temp549=(( struct
Cyc_Tcpat_Int_v_struct*) _temp541)->f1; goto _LL546;} else{ goto _LL542;} _LL544:
return 1; _LL546: return _temp529 - _temp549; _LL542:;} _LL522:;} static struct
Cyc_Set_Set* Cyc_Tcpat_empty_con_set(){ return(( struct Cyc_Set_Set*(*)( int(*
comp)( struct Cyc_Tcpat_Con_s*, struct Cyc_Tcpat_Con_s*))) Cyc_Set_empty)( Cyc_Tcpat_compare_con);}
static struct Cyc_Core_Opt Cyc_Tcpat_one_opt={( void*) 1}; static struct Cyc_Core_Opt
Cyc_Tcpat_two_opt={( void*) 2}; static struct Cyc_Core_Opt* Cyc_Tcpat_one_opt_ptr=(
struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt; static unsigned char _temp552[ 5u]="null";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_null_name_value={ 0u,{ _temp552,
_temp552, _temp552 + 5u}}; static unsigned char _temp554[ 4u]="ptr"; static
struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_ptr_name_value={ 0u,{ _temp554,
_temp554, _temp554 + 4u}}; static unsigned char _temp556[ 6u]="tuple"; static
struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={ 0u,{ _temp556,
_temp556, _temp556 + 6u}}; static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_con0={(
void*)(( void*)& Cyc_Tcpat_null_name_value), 0,( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt};
static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_ptr_con0={( void*)(( void*)& Cyc_Tcpat_ptr_name_value),
1,( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt}; static struct Cyc_Tcpat_Con_s Cyc_Tcpat_ptr_con0={(
void*)(( void*)& Cyc_Tcpat_ptr_name_value), 1,( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt};
static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_con=& Cyc_Tcpat_null_con0; static
struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_ptr_con=& Cyc_Tcpat_null_ptr_con0; static
struct Cyc_Tcpat_Con_s* Cyc_Tcpat_ptr_con=& Cyc_Tcpat_ptr_con0; static struct
Cyc_Tcpat_Con_s* Cyc_Tcpat_int_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp557=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp557->name=( void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp558=(
struct Cyc_Tcpat_Int_v_struct*) GC_malloc_atomic( sizeof( struct Cyc_Tcpat_Int_v_struct));
_temp558[ 0]=({ struct Cyc_Tcpat_Int_v_struct _temp559; _temp559.tag= Cyc_Tcpat_Int_v;
_temp559.f1= i; _temp559;}); _temp558;})); _temp557->arity= 0; _temp557->span= 0;
_temp557;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_float_con( struct
_tagged_arr f){ return({ struct Cyc_Tcpat_Con_s* _temp560=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp560->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp561=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp561[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp562; _temp562.tag= Cyc_Tcpat_Name_v; _temp562.f1= f; _temp562;}); _temp561;}));
_temp560->arity= 0; _temp560->span= 0; _temp560;});} static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_char_con( unsigned char c){ return({ struct Cyc_Tcpat_Con_s* _temp563=(
struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp563->name=(
void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp565=( struct Cyc_Tcpat_Int_v_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Tcpat_Int_v_struct)); _temp565[ 0]=({
struct Cyc_Tcpat_Int_v_struct _temp566; _temp566.tag= Cyc_Tcpat_Int_v; _temp566.f1=(
int) c; _temp566;}); _temp565;})); _temp563->arity= 0; _temp563->span=({ struct
Cyc_Core_Opt* _temp564=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct
Cyc_Core_Opt)); _temp564->v=( void*) 256; _temp564;}); _temp563;});} static
struct Cyc_Tcpat_Con_s* Cyc_Tcpat_tuple_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp567=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp567->name=( void*)(( void*)& Cyc_Tcpat_tuple_name_value); _temp567->arity=
i; _temp567->span= Cyc_Tcpat_one_opt_ptr; _temp567;});} static void* Cyc_Tcpat_null_pat(){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp568=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp568[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp569; _temp569.tag= Cyc_Tcpat_Con; _temp569.f1= Cyc_Tcpat_null_con; _temp569.f2=
0; _temp569;}); _temp568;});} static void* Cyc_Tcpat_int_pat( int i){ return(
void*)({ struct Cyc_Tcpat_Con_struct* _temp570=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp570[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp571; _temp571.tag= Cyc_Tcpat_Con; _temp571.f1= Cyc_Tcpat_int_con( i);
_temp571.f2= 0; _temp571;}); _temp570;});} static void* Cyc_Tcpat_char_pat(
unsigned char c){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp572=(
struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp572[ 0]=({ struct Cyc_Tcpat_Con_struct _temp573; _temp573.tag= Cyc_Tcpat_Con;
_temp573.f1= Cyc_Tcpat_char_con( c); _temp573.f2= 0; _temp573;}); _temp572;});}
static void* Cyc_Tcpat_float_pat( struct _tagged_arr f){ return( void*)({ struct
Cyc_Tcpat_Con_struct* _temp574=( struct Cyc_Tcpat_Con_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Con_struct)); _temp574[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp575; _temp575.tag= Cyc_Tcpat_Con; _temp575.f1= Cyc_Tcpat_float_con( f);
_temp575.f2= 0; _temp575;}); _temp574;});} static void* Cyc_Tcpat_null_ptr_pat(
void* p){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp576=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp576[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp577; _temp577.tag= Cyc_Tcpat_Con; _temp577.f1= Cyc_Tcpat_null_ptr_con;
_temp577.f2=({ struct Cyc_List_List* _temp578=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp578->hd=( void*) p; _temp578->tl= 0;
_temp578;}); _temp577;}); _temp576;});} static void* Cyc_Tcpat_ptr_pat( void* p){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp579=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp579[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp580; _temp580.tag= Cyc_Tcpat_Con; _temp580.f1= Cyc_Tcpat_ptr_con; _temp580.f2=({
struct Cyc_List_List* _temp581=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp581->hd=( void*) p; _temp581->tl= 0; _temp581;});
_temp580;}); _temp579;});} static void* Cyc_Tcpat_tuple_pat( struct Cyc_List_List*
ss){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp582=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp582[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp583; _temp583.tag= Cyc_Tcpat_Con; _temp583.f1= Cyc_Tcpat_tuple_con((( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( ss)); _temp583.f2= ss; _temp583;});
_temp582;});} static void* Cyc_Tcpat_con_pat( struct _tagged_arr con_name,
struct Cyc_Core_Opt* span, struct Cyc_List_List* ps){ struct Cyc_Tcpat_Con_s* c=({
struct Cyc_Tcpat_Con_s* _temp586=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof(
struct Cyc_Tcpat_Con_s)); _temp586->name=( void*)(( void*)({ struct Cyc_Tcpat_Name_v_struct*
_temp587=( struct Cyc_Tcpat_Name_v_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct));
_temp587[ 0]=({ struct Cyc_Tcpat_Name_v_struct _temp588; _temp588.tag= Cyc_Tcpat_Name_v;
_temp588.f1= con_name; _temp588;}); _temp587;})); _temp586->arity=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( ps); _temp586->span= span; _temp586;});
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp584=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp584[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp585; _temp585.tag= Cyc_Tcpat_Con; _temp585.f1= c; _temp585.f2= ps; _temp585;});
_temp584;});} static void* Cyc_Tcpat_compile_pat( struct Cyc_Absyn_Pat* p){ void*
s;{ void* _temp589=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp623; int
_temp625; void* _temp627; unsigned char _temp629; struct _tagged_arr _temp631;
struct Cyc_Absyn_Vardecl* _temp633; struct Cyc_Absyn_Pat* _temp635; struct Cyc_List_List*
_temp637; struct Cyc_List_List* _temp639; struct Cyc_Absyn_Tunionfield* _temp641;
struct Cyc_Absyn_Tuniondecl* _temp643; struct Cyc_List_List* _temp645; struct
Cyc_List_List* _temp647; struct Cyc_Absyn_Tunionfield* _temp649; struct Cyc_Absyn_Tuniondecl*
_temp651; struct Cyc_List_List* _temp653; struct Cyc_List_List* _temp655; struct
Cyc_List_List* _temp657; struct Cyc_Core_Opt* _temp659; struct Cyc_Absyn_Structdecl*
_temp661; struct Cyc_Absyn_Enumfield* _temp663; struct Cyc_Absyn_Enumdecl*
_temp665; struct _tuple1* _temp667; struct Cyc_List_List* _temp669; struct Cyc_List_List*
_temp671; struct _tuple1* _temp673; struct Cyc_List_List* _temp675; struct Cyc_List_List*
_temp677; struct _tuple1* _temp679; _LL591: if( _temp589 ==( void*) Cyc_Absyn_Wild_p){
goto _LL592;} else{ goto _LL593;} _LL593: if(( unsigned int) _temp589 > 2u?*((
int*) _temp589) == Cyc_Absyn_Var_p: 0){ _LL624: _temp623=(( struct Cyc_Absyn_Var_p_struct*)
_temp589)->f1; goto _LL594;} else{ goto _LL595;} _LL595: if( _temp589 ==( void*)
Cyc_Absyn_Null_p){ goto _LL596;} else{ goto _LL597;} _LL597: if(( unsigned int)
_temp589 > 2u?*(( int*) _temp589) == Cyc_Absyn_Int_p: 0){ _LL628: _temp627=(
void*)(( struct Cyc_Absyn_Int_p_struct*) _temp589)->f1; goto _LL626; _LL626:
_temp625=(( struct Cyc_Absyn_Int_p_struct*) _temp589)->f2; goto _LL598;} else{
goto _LL599;} _LL599: if(( unsigned int) _temp589 > 2u?*(( int*) _temp589) ==
Cyc_Absyn_Char_p: 0){ _LL630: _temp629=(( struct Cyc_Absyn_Char_p_struct*)
_temp589)->f1; goto _LL600;} else{ goto _LL601;} _LL601: if(( unsigned int)
_temp589 > 2u?*(( int*) _temp589) == Cyc_Absyn_Float_p: 0){ _LL632: _temp631=((
struct Cyc_Absyn_Float_p_struct*) _temp589)->f1; goto _LL602;} else{ goto _LL603;}
_LL603: if(( unsigned int) _temp589 > 2u?*(( int*) _temp589) == Cyc_Absyn_Reference_p:
0){ _LL634: _temp633=(( struct Cyc_Absyn_Reference_p_struct*) _temp589)->f1;
goto _LL604;} else{ goto _LL605;} _LL605: if(( unsigned int) _temp589 > 2u?*((
int*) _temp589) == Cyc_Absyn_Pointer_p: 0){ _LL636: _temp635=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp589)->f1; goto _LL606;} else{ goto _LL607;} _LL607: if(( unsigned int)
_temp589 > 2u?*(( int*) _temp589) == Cyc_Absyn_Tunion_p: 0){ _LL644: _temp643=((
struct Cyc_Absyn_Tunion_p_struct*) _temp589)->f1; goto _LL642; _LL642: _temp641=((
struct Cyc_Absyn_Tunion_p_struct*) _temp589)->f2; goto _LL640; _LL640: _temp639=((
struct Cyc_Absyn_Tunion_p_struct*) _temp589)->f3; goto _LL638; _LL638: _temp637=((
struct Cyc_Absyn_Tunion_p_struct*) _temp589)->f4; if( _temp637 == 0){ goto
_LL608;} else{ goto _LL609;}} else{ goto _LL609;} _LL609: if(( unsigned int)
_temp589 > 2u?*(( int*) _temp589) == Cyc_Absyn_Tunion_p: 0){ _LL652: _temp651=((
struct Cyc_Absyn_Tunion_p_struct*) _temp589)->f1; goto _LL650; _LL650: _temp649=((
struct Cyc_Absyn_Tunion_p_struct*) _temp589)->f2; goto _LL648; _LL648: _temp647=((
struct Cyc_Absyn_Tunion_p_struct*) _temp589)->f3; goto _LL646; _LL646: _temp645=((
struct Cyc_Absyn_Tunion_p_struct*) _temp589)->f4; goto _LL610;} else{ goto
_LL611;} _LL611: if(( unsigned int) _temp589 > 2u?*(( int*) _temp589) == Cyc_Absyn_Tuple_p:
0){ _LL654: _temp653=(( struct Cyc_Absyn_Tuple_p_struct*) _temp589)->f1; goto
_LL612;} else{ goto _LL613;} _LL613: if(( unsigned int) _temp589 > 2u?*(( int*)
_temp589) == Cyc_Absyn_Struct_p: 0){ _LL662: _temp661=(( struct Cyc_Absyn_Struct_p_struct*)
_temp589)->f1; goto _LL660; _LL660: _temp659=(( struct Cyc_Absyn_Struct_p_struct*)
_temp589)->f2; goto _LL658; _LL658: _temp657=(( struct Cyc_Absyn_Struct_p_struct*)
_temp589)->f3; goto _LL656; _LL656: _temp655=(( struct Cyc_Absyn_Struct_p_struct*)
_temp589)->f4; goto _LL614;} else{ goto _LL615;} _LL615: if(( unsigned int)
_temp589 > 2u?*(( int*) _temp589) == Cyc_Absyn_Enum_p: 0){ _LL666: _temp665=((
struct Cyc_Absyn_Enum_p_struct*) _temp589)->f1; goto _LL664; _LL664: _temp663=((
struct Cyc_Absyn_Enum_p_struct*) _temp589)->f2; goto _LL616;} else{ goto _LL617;}
_LL617: if(( unsigned int) _temp589 > 2u?*(( int*) _temp589) == Cyc_Absyn_UnknownId_p:
0){ _LL668: _temp667=(( struct Cyc_Absyn_UnknownId_p_struct*) _temp589)->f1;
goto _LL618;} else{ goto _LL619;} _LL619: if(( unsigned int) _temp589 > 2u?*((
int*) _temp589) == Cyc_Absyn_UnknownCall_p: 0){ _LL674: _temp673=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp589)->f1; goto _LL672; _LL672: _temp671=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp589)->f2; goto _LL670; _LL670: _temp669=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp589)->f3; goto _LL620;} else{ goto _LL621;} _LL621: if(( unsigned int)
_temp589 > 2u?*(( int*) _temp589) == Cyc_Absyn_UnknownFields_p: 0){ _LL680:
_temp679=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp589)->f1; goto _LL678;
_LL678: _temp677=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp589)->f2;
goto _LL676; _LL676: _temp675=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp589)->f3; goto _LL622;} else{ goto _LL590;} _LL592: s=( void*) Cyc_Tcpat_Any;
goto _LL590; _LL594: s=( void*) Cyc_Tcpat_Any; goto _LL590; _LL596: s= Cyc_Tcpat_null_pat();
goto _LL590; _LL598: s= Cyc_Tcpat_int_pat( _temp625); goto _LL590; _LL600: s=
Cyc_Tcpat_char_pat( _temp629); goto _LL590; _LL602: s= Cyc_Tcpat_float_pat(
_temp631); goto _LL590; _LL604: s=( void*) Cyc_Tcpat_Any; goto _LL590; _LL606:{
void* _temp681= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
p->topt))->v); struct Cyc_Absyn_PtrInfo _temp689; struct Cyc_Absyn_Conref*
_temp691; struct Cyc_Absyn_Tqual _temp693; struct Cyc_Absyn_Conref* _temp695;
void* _temp697; void* _temp699; struct Cyc_Absyn_TunionInfo _temp701; _LL683:
if(( unsigned int) _temp681 > 4u?*(( int*) _temp681) == Cyc_Absyn_PointerType: 0){
_LL690: _temp689=(( struct Cyc_Absyn_PointerType_struct*) _temp681)->f1; _LL700:
_temp699=( void*) _temp689.elt_typ; goto _LL698; _LL698: _temp697=( void*)
_temp689.rgn_typ; goto _LL696; _LL696: _temp695= _temp689.nullable; goto _LL694;
_LL694: _temp693= _temp689.tq; goto _LL692; _LL692: _temp691= _temp689.bounds;
goto _LL684;} else{ goto _LL685;} _LL685: if(( unsigned int) _temp681 > 4u?*((
int*) _temp681) == Cyc_Absyn_TunionType: 0){ _LL702: _temp701=(( struct Cyc_Absyn_TunionType_struct*)
_temp681)->f1; goto _LL686;} else{ goto _LL687;} _LL687: goto _LL688; _LL684: {
int is_nullable= 0; int still_working= 1; while( still_working) { void* _temp703=(
void*) _temp695->v; struct Cyc_Absyn_Conref* _temp711; int _temp713; _LL705: if((
unsigned int) _temp703 > 1u?*(( int*) _temp703) == Cyc_Absyn_Forward_constr: 0){
_LL712: _temp711=(( struct Cyc_Absyn_Forward_constr_struct*) _temp703)->f1; goto
_LL706;} else{ goto _LL707;} _LL707: if( _temp703 ==( void*) Cyc_Absyn_No_constr){
goto _LL708;} else{ goto _LL709;} _LL709: if(( unsigned int) _temp703 > 1u?*((
int*) _temp703) == Cyc_Absyn_Eq_constr: 0){ _LL714: _temp713=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp703)->f1; goto _LL710;} else{ goto _LL704;} _LL706:( void*)( _temp695->v=(
void*)(( void*) _temp711->v)); continue; _LL708:( void*)( _temp695->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp715=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp715[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp716; _temp716.tag= Cyc_Absyn_Eq_constr; _temp716.f1=(
void*) 0; _temp716;}); _temp715;}))); is_nullable= 0; still_working= 0; goto
_LL704; _LL710: is_nullable=( int) _temp713; still_working= 0; goto _LL704;
_LL704:;}{ void* ss= Cyc_Tcpat_compile_pat( _temp635); if( is_nullable){ s= Cyc_Tcpat_null_ptr_pat(
ss);} else{ s= Cyc_Tcpat_ptr_pat( ss);} goto _LL682;}} _LL686:{ void* _temp717=(
void*) _temp635->r; struct Cyc_List_List* _temp723; struct Cyc_List_List*
_temp725; struct Cyc_Absyn_Tunionfield* _temp727; struct Cyc_Absyn_Tuniondecl*
_temp729; _LL719: if(( unsigned int) _temp717 > 2u?*(( int*) _temp717) == Cyc_Absyn_Tunion_p:
0){ _LL730: _temp729=(( struct Cyc_Absyn_Tunion_p_struct*) _temp717)->f1; goto
_LL728; _LL728: _temp727=(( struct Cyc_Absyn_Tunion_p_struct*) _temp717)->f2;
goto _LL726; _LL726: _temp725=(( struct Cyc_Absyn_Tunion_p_struct*) _temp717)->f3;
goto _LL724; _LL724: _temp723=(( struct Cyc_Absyn_Tunion_p_struct*) _temp717)->f4;
goto _LL720;} else{ goto _LL721;} _LL721: goto _LL722; _LL720: { struct Cyc_Core_Opt*
span; if( _temp729->is_xtunion){ span= 0;} else{ span=({ struct Cyc_Core_Opt*
_temp731=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct Cyc_Core_Opt));
_temp731->v=( void*)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp729->fields))->v);
_temp731;});} s= Cyc_Tcpat_con_pat(*(* _temp727->name).f2, span,(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp723)); goto _LL718;} _LL722: s=(( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("non-[x]tunion pattern has tunion type", sizeof( unsigned char), 38u));
goto _LL718; _LL718:;} goto _LL682; _LL688: s=(( void*(*)( struct _tagged_arr))
Cyc_Tcutil_impos)( _tag_arr("pointer pattern does not have pointer type!",
sizeof( unsigned char), 44u)); goto _LL682; _LL682:;} goto _LL590; _LL608: {
struct Cyc_Core_Opt* span;{ void* _temp732= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( p->topt))->v); struct Cyc_Absyn_TunionInfo
_temp740; struct Cyc_Absyn_TunionFieldInfo _temp742; _LL734: if(( unsigned int)
_temp732 > 4u?*(( int*) _temp732) == Cyc_Absyn_TunionType: 0){ _LL741: _temp740=((
struct Cyc_Absyn_TunionType_struct*) _temp732)->f1; goto _LL735;} else{ goto
_LL736;} _LL736: if(( unsigned int) _temp732 > 4u?*(( int*) _temp732) == Cyc_Absyn_TunionFieldType:
0){ _LL743: _temp742=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp732)->f1;
goto _LL737;} else{ goto _LL738;} _LL738: goto _LL739; _LL735: if( _temp643->is_xtunion){
span= 0;} else{ span=({ struct Cyc_Core_Opt* _temp744=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp744->v=( void*)(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp643->fields))->v); _temp744;});} goto _LL733; _LL737: span=({
struct Cyc_Core_Opt* _temp745=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof(
struct Cyc_Core_Opt)); _temp745->v=( void*) 1; _temp745;}); goto _LL733; _LL739:
span=(( struct Cyc_Core_Opt*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("void tunion pattern has bad type", sizeof( unsigned char), 33u)); goto
_LL733; _LL733:;} s= Cyc_Tcpat_con_pat(*(* _temp641->name).f2, span, 0); goto
_LL590;} _LL610: _temp653= _temp645; goto _LL612; _LL612: s= Cyc_Tcpat_tuple_pat(((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcpat_compile_pat, _temp653)); goto _LL590; _LL614: {
struct Cyc_List_List* ps= 0;{ struct Cyc_List_List* fields=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp661->fields))->v; for( 0; fields != 0;
fields=(( struct Cyc_List_List*) _check_null( fields))->tl){ int found= Cyc_String_strcmp(*((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( fields))->hd)->name,
_tag_arr("", sizeof( unsigned char), 1u)) == 0;{ struct Cyc_List_List* dlps0=
_temp655; for( 0; ! found? dlps0 != 0: 0; dlps0=(( struct Cyc_List_List*)
_check_null( dlps0))->tl){ struct _tuple7 _temp748; struct Cyc_Absyn_Pat*
_temp749; struct Cyc_List_List* _temp751; struct _tuple7* _temp746=( struct
_tuple7*)(( struct Cyc_List_List*) _check_null( dlps0))->hd; _temp748=* _temp746;
_LL752: _temp751= _temp748.f1; goto _LL750; _LL750: _temp749= _temp748.f2; goto
_LL747; _LL747: { struct Cyc_List_List* _temp753= _temp751; struct Cyc_List_List
_temp759; struct Cyc_List_List* _temp760; void* _temp762; struct _tagged_arr*
_temp764; _LL755: if( _temp753 == 0){ goto _LL757;} else{ _temp759=* _temp753;
_LL763: _temp762=( void*) _temp759.hd; if(*(( int*) _temp762) == Cyc_Absyn_FieldName){
_LL765: _temp764=(( struct Cyc_Absyn_FieldName_struct*) _temp762)->f1; goto
_LL761;} else{ goto _LL757;} _LL761: _temp760= _temp759.tl; if( _temp760 == 0){
goto _LL756;} else{ goto _LL757;}} _LL757: goto _LL758; _LL756: if( Cyc_String_zstrptrcmp(
_temp764,(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd)->name) == 0){ ps=({ struct Cyc_List_List* _temp766=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp766->hd=( void*) Cyc_Tcpat_compile_pat(
_temp749); _temp766->tl= ps; _temp766;}); found= 1;} goto _LL754; _LL758:(( int(*)(
struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("bad designator(s)", sizeof(
unsigned char), 18u)); goto _LL754; _LL754:;}}} if( ! found){(( int(*)( struct
_tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("bad designator", sizeof(
unsigned char), 15u));}}} s= Cyc_Tcpat_tuple_pat( ps); goto _LL590;} _LL616: {
int span=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp665->fields))->v); s= Cyc_Tcpat_con_pat(*(*
_temp663->name).f2,({ struct Cyc_Core_Opt* _temp767=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp767->v=( void*) span;
_temp767;}), 0); goto _LL590;} _LL618: s=(( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("compile_pat: unknownid", sizeof( unsigned char), 23u)); goto _LL590;
_LL620: s=(( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("compile_pat: unknowncall",
sizeof( unsigned char), 25u)); goto _LL590; _LL622: s=(( void*(*)( struct
_tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("compile_pat: unknownfields", sizeof(
unsigned char), 27u)); goto _LL590; _LL590:;} return s;} static const int Cyc_Tcpat_Pos=
0; struct Cyc_Tcpat_Pos_struct{ int tag; struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Tcpat_Neg= 1; struct Cyc_Tcpat_Neg_struct{ int tag;
struct Cyc_Set_Set* f1; } ; static const int Cyc_Tcpat_Failure= 0; static const
int Cyc_Tcpat_Success= 0; struct Cyc_Tcpat_Success_struct{ int tag; void* f1; }
; static const int Cyc_Tcpat_IfEq= 1; struct Cyc_Tcpat_IfEq_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Tcpat_Con_s* f2; void* f3; void* f4; } ;
struct _tuple10{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; struct Cyc_List_List*
f3; } ; static const int Cyc_Tcpat_Yes= 0; static const int Cyc_Tcpat_No= 1;
static const int Cyc_Tcpat_Maybe= 2; static void* Cyc_Tcpat_add_neg( void* td,
struct Cyc_Tcpat_Con_s* c){ void* _temp768= td; struct Cyc_Set_Set* _temp774;
struct Cyc_List_List* _temp776; struct Cyc_Tcpat_Con_s* _temp778; _LL770: if(*((
int*) _temp768) == Cyc_Tcpat_Neg){ _LL775: _temp774=(( struct Cyc_Tcpat_Neg_struct*)
_temp768)->f1; goto _LL771;} else{ goto _LL772;} _LL772: if(*(( int*) _temp768)
== Cyc_Tcpat_Pos){ _LL779: _temp778=(( struct Cyc_Tcpat_Pos_struct*) _temp768)->f1;
goto _LL777; _LL777: _temp776=(( struct Cyc_Tcpat_Pos_struct*) _temp768)->f2;
goto _LL773;} else{ goto _LL769;} _LL771: if((( int(*)( struct Cyc_Set_Set* s,
struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)( _temp774, c)){(( int(*)( struct
_tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("add_neg called when constructor already in set",
sizeof( unsigned char), 47u));} if( c->span != 0?(( int(*)( struct Cyc_Set_Set*
s)) Cyc_Set_cardinality)( _temp774) + 1 >=( int)(( struct Cyc_Core_Opt*)
_check_null( c->span))->v: 0){(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("add_neg called when |cs U {c}| >= span(c)", sizeof( unsigned char), 42u));}
return( void*)({ struct Cyc_Tcpat_Neg_struct* _temp780=( struct Cyc_Tcpat_Neg_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct)); _temp780[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp781; _temp781.tag= Cyc_Tcpat_Neg; _temp781.f1=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_insert)( _temp774,
c); _temp781;}); _temp780;}); _LL773: return(( void*(*)( struct _tagged_arr))
Cyc_Tcutil_impos)( _tag_arr("add_neg called when td is Positive", sizeof(
unsigned char), 35u)); _LL769:;} static void* Cyc_Tcpat_static_match( struct Cyc_Tcpat_Con_s*
c, void* td){ void* _temp782= td; struct Cyc_List_List* _temp788; struct Cyc_Tcpat_Con_s*
_temp790; struct Cyc_Set_Set* _temp792; _LL784: if(*(( int*) _temp782) == Cyc_Tcpat_Pos){
_LL791: _temp790=(( struct Cyc_Tcpat_Pos_struct*) _temp782)->f1; goto _LL789;
_LL789: _temp788=(( struct Cyc_Tcpat_Pos_struct*) _temp782)->f2; goto _LL785;}
else{ goto _LL786;} _LL786: if(*(( int*) _temp782) == Cyc_Tcpat_Neg){ _LL793:
_temp792=(( struct Cyc_Tcpat_Neg_struct*) _temp782)->f1; goto _LL787;} else{
goto _LL783;} _LL785: if( Cyc_Tcpat_compare_con( c, _temp790) == 0){ return(
void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_No;} _LL787: if((( int(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)( _temp792,
c)){ return( void*) Cyc_Tcpat_No;} else{ if( c->span != 0?( int)(( struct Cyc_Core_Opt*)
_check_null( c->span))->v ==(( int(*)( struct Cyc_Set_Set* s)) Cyc_Set_cardinality)(
_temp792) + 1: 0){ return( void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_Maybe;}}
_LL783:;} struct _tuple11{ struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2;
} ; static struct Cyc_List_List* Cyc_Tcpat_augment( struct Cyc_List_List* ctxt,
void* dsc){ struct Cyc_List_List* _temp794= ctxt; struct Cyc_List_List _temp800;
struct Cyc_List_List* _temp801; struct _tuple11* _temp803; struct _tuple11
_temp805; struct Cyc_List_List* _temp806; struct Cyc_Tcpat_Con_s* _temp808;
_LL796: if( _temp794 == 0){ goto _LL797;} else{ goto _LL798;} _LL798: if(
_temp794 == 0){ goto _LL795;} else{ _temp800=* _temp794; _LL804: _temp803=(
struct _tuple11*) _temp800.hd; _temp805=* _temp803; _LL809: _temp808= _temp805.f1;
goto _LL807; _LL807: _temp806= _temp805.f2; goto _LL802; _LL802: _temp801=
_temp800.tl; goto _LL799;} _LL797: return 0; _LL799: return({ struct Cyc_List_List*
_temp810=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp810->hd=( void*)({ struct _tuple11* _temp811=( struct _tuple11*) GC_malloc(
sizeof( struct _tuple11)); _temp811->f1= _temp808; _temp811->f2=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp812=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp812->hd=( void*) dsc; _temp812->tl= _temp806;
_temp812;}); _temp811;}); _temp810->tl= _temp801; _temp810;}); _LL795:;} static
struct Cyc_List_List* Cyc_Tcpat_norm_context( struct Cyc_List_List* ctxt){
struct Cyc_List_List* _temp813= ctxt; struct Cyc_List_List _temp819; struct Cyc_List_List*
_temp820; struct _tuple11* _temp822; struct _tuple11 _temp824; struct Cyc_List_List*
_temp825; struct Cyc_Tcpat_Con_s* _temp827; _LL815: if( _temp813 == 0){ goto
_LL816;} else{ goto _LL817;} _LL817: if( _temp813 == 0){ goto _LL814;} else{
_temp819=* _temp813; _LL823: _temp822=( struct _tuple11*) _temp819.hd; _temp824=*
_temp822; _LL828: _temp827= _temp824.f1; goto _LL826; _LL826: _temp825= _temp824.f2;
goto _LL821; _LL821: _temp820= _temp819.tl; goto _LL818;} _LL816: return((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("norm_context: empty context",
sizeof( unsigned char), 28u)); _LL818: return Cyc_Tcpat_augment( _temp820,( void*)({
struct Cyc_Tcpat_Pos_struct* _temp829=( struct Cyc_Tcpat_Pos_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Pos_struct)); _temp829[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp830; _temp830.tag= Cyc_Tcpat_Pos; _temp830.f1= _temp827; _temp830.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp825);
_temp830;}); _temp829;})); _LL814:;} static void* Cyc_Tcpat_build_desc( struct
Cyc_List_List* ctxt, void* dsc, struct Cyc_List_List* work){ struct _tuple6
_temp832=({ struct _tuple6 _temp831; _temp831.f1= ctxt; _temp831.f2= work;
_temp831;}); struct Cyc_List_List* _temp842; struct Cyc_List_List* _temp844;
struct Cyc_List_List* _temp846; struct Cyc_List_List* _temp848; struct Cyc_List_List*
_temp850; struct Cyc_List_List* _temp852; struct Cyc_List_List* _temp854; struct
Cyc_List_List _temp856; struct Cyc_List_List* _temp857; struct _tuple10*
_temp859; struct _tuple10 _temp861; struct Cyc_List_List* _temp862; struct Cyc_List_List*
_temp864; struct Cyc_List_List* _temp866; struct Cyc_List_List* _temp868; struct
Cyc_List_List _temp870; struct Cyc_List_List* _temp871; struct _tuple11*
_temp873; struct _tuple11 _temp875; struct Cyc_List_List* _temp876; struct Cyc_Tcpat_Con_s*
_temp878; _LL834: _LL845: _temp844= _temp832.f1; if( _temp844 == 0){ goto _LL843;}
else{ goto _LL836;} _LL843: _temp842= _temp832.f2; if( _temp842 == 0){ goto
_LL835;} else{ goto _LL836;} _LL836: _LL849: _temp848= _temp832.f1; if( _temp848
== 0){ goto _LL847;} else{ goto _LL838;} _LL847: _temp846= _temp832.f2; goto
_LL837; _LL838: _LL853: _temp852= _temp832.f1; goto _LL851; _LL851: _temp850=
_temp832.f2; if( _temp850 == 0){ goto _LL839;} else{ goto _LL840;} _LL840:
_LL869: _temp868= _temp832.f1; if( _temp868 == 0){ goto _LL833;} else{ _temp870=*
_temp868; _LL874: _temp873=( struct _tuple11*) _temp870.hd; _temp875=* _temp873;
_LL879: _temp878= _temp875.f1; goto _LL877; _LL877: _temp876= _temp875.f2; goto
_LL872; _LL872: _temp871= _temp870.tl; goto _LL855;} _LL855: _temp854= _temp832.f2;
if( _temp854 == 0){ goto _LL833;} else{ _temp856=* _temp854; _LL860: _temp859=(
struct _tuple10*) _temp856.hd; _temp861=* _temp859; _LL867: _temp866= _temp861.f1;
goto _LL865; _LL865: _temp864= _temp861.f2; goto _LL863; _LL863: _temp862=
_temp861.f3; goto _LL858; _LL858: _temp857= _temp856.tl; goto _LL841;} _LL835:
return dsc; _LL837: return(( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("build_desc: ctxt and work don't match", sizeof( unsigned char), 38u));
_LL839: return(( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("build_desc: ctxt and work don't match",
sizeof( unsigned char), 38u)); _LL841: { struct Cyc_Tcpat_Pos_struct* _temp880=({
struct Cyc_Tcpat_Pos_struct* _temp881=( struct Cyc_Tcpat_Pos_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Pos_struct)); _temp881[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp882; _temp882.tag= Cyc_Tcpat_Pos; _temp882.f1= _temp878; _temp882.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp876),({
struct Cyc_List_List* _temp883=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp883->hd=( void*) dsc; _temp883->tl= _temp862;
_temp883;})); _temp882;}); _temp881;}); return Cyc_Tcpat_build_desc( _temp871,(
void*) _temp880, _temp857);} _LL833:;} static void* Cyc_Tcpat_match( void* p,
struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules); static void* Cyc_Tcpat_or_match(
void* dsc, struct Cyc_List_List* allmrules){ struct Cyc_List_List* _temp884=
allmrules; struct Cyc_List_List _temp890; struct Cyc_List_List* _temp891; struct
_tuple0* _temp893; struct _tuple0 _temp895; void* _temp896; void* _temp898;
_LL886: if( _temp884 == 0){ goto _LL887;} else{ goto _LL888;} _LL888: if(
_temp884 == 0){ goto _LL885;} else{ _temp890=* _temp884; _LL894: _temp893=(
struct _tuple0*) _temp890.hd; _temp895=* _temp893; _LL899: _temp898= _temp895.f1;
goto _LL897; _LL897: _temp896= _temp895.f2; goto _LL892; _LL892: _temp891=
_temp890.tl; goto _LL889;} _LL887: return( void*) Cyc_Tcpat_Failure; _LL889:
return(( void*(*)( void* p, struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules)) Cyc_Tcpat_match)( _temp898, 0, dsc, 0, 0, _temp896, _temp891); _LL885:;}
static void* Cyc_Tcpat_match_compile( struct Cyc_List_List* allmrules){ return((
void*(*)( void* dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)((
void*)({ struct Cyc_Tcpat_Neg_struct* _temp900=( struct Cyc_Tcpat_Neg_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct)); _temp900[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp901; _temp901.tag= Cyc_Tcpat_Neg; _temp901.f1= Cyc_Tcpat_empty_con_set();
_temp901;}); _temp900;}), allmrules);} static void* Cyc_Tcpat_and_match( struct
Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side, struct
Cyc_List_List* rules){ struct Cyc_List_List* _temp902= work; struct Cyc_List_List
_temp910; struct Cyc_List_List* _temp911; struct _tuple10* _temp913; struct
_tuple10 _temp915; struct Cyc_List_List* _temp916; struct Cyc_List_List*
_temp918; struct Cyc_List_List* _temp920; struct Cyc_List_List _temp922; struct
Cyc_List_List* _temp923; struct _tuple10* _temp925; struct _tuple10 _temp927;
struct Cyc_List_List* _temp928; struct Cyc_List_List* _temp930; struct Cyc_List_List*
_temp932; _LL904: if( _temp902 == 0){ goto _LL905;} else{ goto _LL906;} _LL906:
if( _temp902 == 0){ goto _LL908;} else{ _temp910=* _temp902; _LL914: _temp913=(
struct _tuple10*) _temp910.hd; _temp915=* _temp913; _LL921: _temp920= _temp915.f1;
if( _temp920 == 0){ goto _LL919;} else{ goto _LL908;} _LL919: _temp918= _temp915.f2;
if( _temp918 == 0){ goto _LL917;} else{ goto _LL908;} _LL917: _temp916= _temp915.f3;
if( _temp916 == 0){ goto _LL912;} else{ goto _LL908;} _LL912: _temp911= _temp910.tl;
goto _LL907;} _LL908: if( _temp902 == 0){ goto _LL903;} else{ _temp922=*
_temp902; _LL926: _temp925=( struct _tuple10*) _temp922.hd; _temp927=* _temp925;
_LL933: _temp932= _temp927.f1; goto _LL931; _LL931: _temp930= _temp927.f2; goto
_LL929; _LL929: _temp928= _temp927.f3; goto _LL924; _LL924: _temp923= _temp922.tl;
goto _LL909;} _LL905: return( void*)({ struct Cyc_Tcpat_Success_struct* _temp934=(
struct Cyc_Tcpat_Success_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Success_struct));
_temp934[ 0]=({ struct Cyc_Tcpat_Success_struct _temp935; _temp935.tag= Cyc_Tcpat_Success;
_temp935.f1=( void*) right_hand_side; _temp935;}); _temp934;}); _LL907: return((
void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)( Cyc_Tcpat_norm_context(
ctx), _temp911, right_hand_side, rules); _LL909: if(( _temp932 == 0? 1: _temp930
== 0)? 1: _temp928 == 0){ return(( void*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("tcpat:and_match: malformed work frame", sizeof( unsigned char), 38u));}{
struct Cyc_List_List _temp938; struct Cyc_List_List* _temp939; void* _temp941;
struct Cyc_List_List* _temp936=( struct Cyc_List_List*) _check_null( _temp932);
_temp938=* _temp936; _LL942: _temp941=( void*) _temp938.hd; goto _LL940; _LL940:
_temp939= _temp938.tl; goto _LL937; _LL937: { struct Cyc_List_List _temp945;
struct Cyc_List_List* _temp946; struct Cyc_List_List* _temp948; struct Cyc_List_List*
_temp943=( struct Cyc_List_List*) _check_null( _temp930); _temp945=* _temp943;
_LL949: _temp948=( struct Cyc_List_List*) _temp945.hd; goto _LL947; _LL947:
_temp946= _temp945.tl; goto _LL944; _LL944: { struct Cyc_List_List _temp952;
struct Cyc_List_List* _temp953; void* _temp955; struct Cyc_List_List* _temp950=(
struct Cyc_List_List*) _check_null( _temp928); _temp952=* _temp950; _LL956:
_temp955=( void*) _temp952.hd; goto _LL954; _LL954: _temp953= _temp952.tl; goto
_LL951; _LL951: { struct _tuple10* _temp957=({ struct _tuple10* _temp959=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp959->f1= _temp939;
_temp959->f2= _temp946; _temp959->f3= _temp953; _temp959;}); return(( void*(*)(
void* p, struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List* ctx, struct
Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_match)(
_temp941, _temp948, _temp955, ctx,({ struct Cyc_List_List* _temp958=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp958->hd=( void*) _temp957;
_temp958->tl= _temp923; _temp958;}), right_hand_side, rules);}}}} _LL903:;}
static struct Cyc_List_List* Cyc_Tcpat_getdargs( struct Cyc_Tcpat_Con_s* pcon,
void* dsc){ void* _temp960= dsc; struct Cyc_Set_Set* _temp966; struct Cyc_List_List*
_temp968; struct Cyc_Tcpat_Con_s* _temp970; _LL962: if(*(( int*) _temp960) ==
Cyc_Tcpat_Neg){ _LL967: _temp966=(( struct Cyc_Tcpat_Neg_struct*) _temp960)->f1;
goto _LL963;} else{ goto _LL964;} _LL964: if(*(( int*) _temp960) == Cyc_Tcpat_Pos){
_LL971: _temp970=(( struct Cyc_Tcpat_Pos_struct*) _temp960)->f1; goto _LL969;
_LL969: _temp968=(( struct Cyc_Tcpat_Pos_struct*) _temp960)->f2; goto _LL965;}
else{ goto _LL961;} _LL963: { void* any=( void*)({ struct Cyc_Tcpat_Neg_struct*
_temp974=( struct Cyc_Tcpat_Neg_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct));
_temp974[ 0]=({ struct Cyc_Tcpat_Neg_struct _temp975; _temp975.tag= Cyc_Tcpat_Neg;
_temp975.f1= Cyc_Tcpat_empty_con_set(); _temp975;}); _temp974;}); struct Cyc_List_List*
_temp972= 0;{ int i= 0; for( 0; i < pcon->arity; ++ i){ _temp972=({ struct Cyc_List_List*
_temp973=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp973->hd=( void*) any; _temp973->tl= _temp972; _temp973;});}} return
_temp972;} _LL965: return _temp968; _LL961:;} static struct Cyc_List_List* Cyc_Tcpat_getoarg(
struct Cyc_List_List* obj, int i){ return({ struct Cyc_List_List* _temp976=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp976->hd=(
void*)( i + 1); _temp976->tl= obj; _temp976;});} static struct Cyc_List_List*
Cyc_Tcpat_getoargs( struct Cyc_Tcpat_Con_s* pcon, struct Cyc_List_List* obj){
return(( struct Cyc_List_List*(*)( int n, struct Cyc_List_List*(* f)( struct Cyc_List_List*,
int), struct Cyc_List_List* env)) Cyc_List_tabulate_c)( pcon->arity, Cyc_Tcpat_getoarg,
obj);} static void* Cyc_Tcpat_match( void* p, struct Cyc_List_List* obj, void*
dsc, struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules){ void* _temp977= p; struct Cyc_List_List*
_temp983; struct Cyc_Tcpat_Con_s* _temp985; _LL979: if( _temp977 ==( void*) Cyc_Tcpat_Any){
goto _LL980;} else{ goto _LL981;} _LL981: if(( unsigned int) _temp977 > 1u?*((
int*) _temp977) == Cyc_Tcpat_Con: 0){ _LL986: _temp985=(( struct Cyc_Tcpat_Con_struct*)
_temp977)->f1; goto _LL984; _LL984: _temp983=(( struct Cyc_Tcpat_Con_struct*)
_temp977)->f2; goto _LL982;} else{ goto _LL978;} _LL980: return(( void*(*)(
struct Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side,
struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)( Cyc_Tcpat_augment( ctx, dsc),
work, right_hand_side, rules); _LL982: { void* _temp987= Cyc_Tcpat_static_match(
_temp985, dsc); _LL989: if( _temp987 ==( void*) Cyc_Tcpat_Yes){ goto _LL990;}
else{ goto _LL991;} _LL991: if( _temp987 ==( void*) Cyc_Tcpat_No){ goto _LL992;}
else{ goto _LL993;} _LL993: if( _temp987 ==( void*) Cyc_Tcpat_Maybe){ goto
_LL994;} else{ goto _LL988;} _LL990: { struct Cyc_List_List* _temp995=({ struct
Cyc_List_List* _temp1000=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1000->hd=( void*)({ struct _tuple11* _temp1001=( struct _tuple11*)
GC_malloc( sizeof( struct _tuple11)); _temp1001->f1= _temp985; _temp1001->f2= 0;
_temp1001;}); _temp1000->tl= ctx; _temp1000;}); struct _tuple10* _temp996=({
struct _tuple10* _temp999=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp999->f1= _temp983; _temp999->f2= Cyc_Tcpat_getoargs( _temp985, obj);
_temp999->f3= Cyc_Tcpat_getdargs( _temp985, dsc); _temp999;}); struct Cyc_List_List*
_temp997=({ struct Cyc_List_List* _temp998=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp998->hd=( void*) _temp996; _temp998->tl=
work; _temp998;}); return(( void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)(
_temp995, _temp997, right_hand_side, rules);} _LL992: return(( void*(*)( void*
dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)( Cyc_Tcpat_build_desc(
ctx, dsc, work), rules); _LL994: { struct Cyc_List_List* _temp1002=({ struct Cyc_List_List*
_temp1011=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1011->hd=( void*)({ struct _tuple11* _temp1012=( struct _tuple11*)
GC_malloc( sizeof( struct _tuple11)); _temp1012->f1= _temp985; _temp1012->f2= 0;
_temp1012;}); _temp1011->tl= ctx; _temp1011;}); struct _tuple10* _temp1003=({
struct _tuple10* _temp1010=( struct _tuple10*) GC_malloc( sizeof( struct
_tuple10)); _temp1010->f1= _temp983; _temp1010->f2= Cyc_Tcpat_getoargs( _temp985,
obj); _temp1010->f3= Cyc_Tcpat_getdargs( _temp985, dsc); _temp1010;}); struct
Cyc_List_List* _temp1004=({ struct Cyc_List_List* _temp1009=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1009->hd=( void*) _temp1003;
_temp1009->tl= work; _temp1009;}); void* _temp1005=(( void*(*)( struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules)) Cyc_Tcpat_and_match)( _temp1002, _temp1004, right_hand_side, rules);
void* _temp1006=(( void*(*)( void* dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)(
Cyc_Tcpat_build_desc( ctx, Cyc_Tcpat_add_neg( dsc, _temp985), work), rules);
return( void*)({ struct Cyc_Tcpat_IfEq_struct* _temp1007=( struct Cyc_Tcpat_IfEq_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_IfEq_struct)); _temp1007[ 0]=({ struct Cyc_Tcpat_IfEq_struct
_temp1008; _temp1008.tag= Cyc_Tcpat_IfEq; _temp1008.f1= obj; _temp1008.f2=
_temp985; _temp1008.f3=( void*) _temp1005; _temp1008.f4=( void*) _temp1006;
_temp1008;}); _temp1007;});} _LL988:;} _LL978:;} static void Cyc_Tcpat_check_exhaust_overlap(
void* d, void(* not_exhaust)( void*), void* env1, void(* rhs_appears)( void*,
void*), void* env2){ void* _temp1013= d; void* _temp1021; void* _temp1023; void*
_temp1025; struct Cyc_Tcpat_Con_s* _temp1027; struct Cyc_List_List* _temp1029;
_LL1015: if( _temp1013 ==( void*) Cyc_Tcpat_Failure){ goto _LL1016;} else{ goto
_LL1017;} _LL1017: if(( unsigned int) _temp1013 > 1u?*(( int*) _temp1013) == Cyc_Tcpat_Success:
0){ _LL1022: _temp1021=( void*)(( struct Cyc_Tcpat_Success_struct*) _temp1013)->f1;
goto _LL1018;} else{ goto _LL1019;} _LL1019: if(( unsigned int) _temp1013 > 1u?*((
int*) _temp1013) == Cyc_Tcpat_IfEq: 0){ _LL1030: _temp1029=(( struct Cyc_Tcpat_IfEq_struct*)
_temp1013)->f1; goto _LL1028; _LL1028: _temp1027=(( struct Cyc_Tcpat_IfEq_struct*)
_temp1013)->f2; goto _LL1026; _LL1026: _temp1025=( void*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1013)->f3; goto _LL1024; _LL1024: _temp1023=( void*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1013)->f4; goto _LL1020;} else{ goto _LL1014;} _LL1016: not_exhaust( env1);
goto _LL1014; _LL1018: rhs_appears( env2, _temp1021); goto _LL1014; _LL1020:((
void(*)( void* d, void(* not_exhaust)( void*), void* env1, void(* rhs_appears)(
void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)( _temp1025,
not_exhaust, env1, rhs_appears, env2);(( void(*)( void* d, void(* not_exhaust)(
void*), void* env1, void(* rhs_appears)( void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp1023, not_exhaust, env1, rhs_appears, env2); goto _LL1014; _LL1014:;}
struct _tuple12{ int f1; struct Cyc_Position_Segment* f2; } ; struct _tuple13{
void* f1; struct _tuple12* f2; } ; static struct _tuple13* Cyc_Tcpat_get_match(
int* r, struct Cyc_Absyn_Switch_clause* swc){ void* sp0= Cyc_Tcpat_compile_pat(
swc->pattern); struct _tuple12* rhs=({ struct _tuple12* _temp1036=( struct
_tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp1036->f1= 0; _temp1036->f2=(
swc->pattern)->loc; _temp1036;}); void* sp; if( swc->where_clause != 0){ sp= Cyc_Tcpat_tuple_pat(({
struct Cyc_List_List* _temp1031=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1031->hd=( void*) sp0; _temp1031->tl=({ struct Cyc_List_List*
_temp1032=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1032->hd=( void*) Cyc_Tcpat_int_pat(* r); _temp1032->tl= 0; _temp1032;});
_temp1031;}));* r=* r + 1;} else{ sp= Cyc_Tcpat_tuple_pat(({ struct Cyc_List_List*
_temp1033=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1033->hd=( void*) sp0; _temp1033->tl=({ struct Cyc_List_List* _temp1034=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1034->hd=(
void*)(( void*) Cyc_Tcpat_Any); _temp1034->tl= 0; _temp1034;}); _temp1033;}));}
return({ struct _tuple13* _temp1035=( struct _tuple13*) GC_malloc( sizeof(
struct _tuple13)); _temp1035->f1= sp; _temp1035->f2= rhs; _temp1035;});} static
void Cyc_Tcpat_not_exhaust_err( struct Cyc_Position_Segment* loc){ Cyc_Tcutil_terr(
loc, _tag_arr("patterns may not be exhaustive", sizeof( unsigned char), 31u));}
static void Cyc_Tcpat_rule_occurs( int dummy, struct _tuple12* rhs){(* rhs).f1=
1;} void Cyc_Tcpat_check_switch_exhaustive( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* swcs){ int _temp1037= 0; struct Cyc_List_List* _temp1038=((
struct Cyc_List_List*(*)( struct _tuple13*(* f)( int*, struct Cyc_Absyn_Switch_clause*),
int* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcpat_get_match,&
_temp1037, swcs); void* _temp1039=(( void*(*)( struct Cyc_List_List* allmrules))
Cyc_Tcpat_match_compile)( _temp1038);(( void(*)( void* d, void(* not_exhaust)(
struct Cyc_Position_Segment*), struct Cyc_Position_Segment* env1, void(*
rhs_appears)( int, struct _tuple12*), int env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp1039, Cyc_Tcpat_not_exhaust_err, loc, Cyc_Tcpat_rule_occurs, 0); for( 0;
_temp1038 != 0; _temp1038=(( struct Cyc_List_List*) _check_null( _temp1038))->tl){
struct _tuple13 _temp1042; struct _tuple12* _temp1043; struct _tuple12 _temp1045;
struct Cyc_Position_Segment* _temp1046; int _temp1048; void* _temp1050; struct
_tuple13* _temp1040=( struct _tuple13*)(( struct Cyc_List_List*) _check_null(
_temp1038))->hd; _temp1042=* _temp1040; _LL1051: _temp1050= _temp1042.f1; goto
_LL1044; _LL1044: _temp1043= _temp1042.f2; _temp1045=* _temp1043; _LL1049:
_temp1048= _temp1045.f1; goto _LL1047; _LL1047: _temp1046= _temp1045.f2; goto
_LL1041; _LL1041: if( ! _temp1048){ Cyc_Tcutil_terr( _temp1046, _tag_arr("redundant pattern",
sizeof( unsigned char), 18u));}}} struct _tuple14{ struct Cyc_Position_Segment*
f1; int f2; } ; static void Cyc_Tcpat_not_exhaust_warn( struct _tuple14* pr){(*
pr).f2= 0; Cyc_Tcutil_warn((* pr).f1, _tag_arr("pattern not exhaustive", sizeof(
unsigned char), 23u));} static void Cyc_Tcpat_dummy_fn( int i, int j){ return;}
struct _tuple15{ void* f1; int f2; } ; int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Pat* p){ struct Cyc_List_List*
_temp1052=({ struct Cyc_List_List* _temp1056=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1056->hd=( void*)({ struct _tuple15*
_temp1057=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp1057->f1=
Cyc_Tcpat_compile_pat( p); _temp1057->f2= 0; _temp1057;}); _temp1056->tl= 0;
_temp1056;}); void* _temp1053=(( void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
_temp1052); struct _tuple14* _temp1054=({ struct _tuple14* _temp1055=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp1055->f1= loc; _temp1055->f2=
1; _temp1055;});(( void(*)( void* d, void(* not_exhaust)( struct _tuple14*),
struct _tuple14* env1, void(* rhs_appears)( int, int), int env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp1053, Cyc_Tcpat_not_exhaust_warn, _temp1054, Cyc_Tcpat_dummy_fn, 0); return(*
_temp1054).f2;} static struct _tuple13* Cyc_Tcpat_get_match2( struct Cyc_Absyn_Switch_clause*
swc){ void* sp0= Cyc_Tcpat_compile_pat( swc->pattern); struct _tuple12* rhs=({
struct _tuple12* _temp1059=( struct _tuple12*) GC_malloc( sizeof( struct
_tuple12)); _temp1059->f1= 0; _temp1059->f2=( swc->pattern)->loc; _temp1059;});
return({ struct _tuple13* _temp1058=( struct _tuple13*) GC_malloc( sizeof(
struct _tuple13)); _temp1058->f1= sp0; _temp1058->f2= rhs; _temp1058;});} static
void Cyc_Tcpat_not_exhaust_err2( struct Cyc_Position_Segment* loc){;} void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ struct Cyc_List_List*
_temp1060=(( struct Cyc_List_List*(*)( struct _tuple13*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_get_match2, swcs); void*
_temp1061=(( void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
_temp1060);(( void(*)( void* d, void(* not_exhaust)( struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env1, void(* rhs_appears)( int, struct _tuple12*),
int env2)) Cyc_Tcpat_check_exhaust_overlap)( _temp1061, Cyc_Tcpat_not_exhaust_err2,
loc, Cyc_Tcpat_rule_occurs, 0); for( 0; _temp1060 != 0; _temp1060=(( struct Cyc_List_List*)
_check_null( _temp1060))->tl){ struct _tuple13 _temp1064; struct _tuple12*
_temp1065; struct _tuple12 _temp1067; struct Cyc_Position_Segment* _temp1068;
int _temp1070; void* _temp1072; struct _tuple13* _temp1062=( struct _tuple13*)((
struct Cyc_List_List*) _check_null( _temp1060))->hd; _temp1064=* _temp1062;
_LL1073: _temp1072= _temp1064.f1; goto _LL1066; _LL1066: _temp1065= _temp1064.f2;
_temp1067=* _temp1065; _LL1071: _temp1070= _temp1067.f1; goto _LL1069; _LL1069:
_temp1068= _temp1067.f2; goto _LL1063; _LL1063: if( ! _temp1070){ Cyc_Tcutil_terr(
_temp1068, _tag_arr("redundant pattern", sizeof( unsigned char), 18u));}}}