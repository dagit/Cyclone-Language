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
te, p->loc, _temp10); struct Cyc_Absyn_Tunionfield* _temp37; struct Cyc_Absyn_Tuniondecl*
_temp39; struct Cyc_Absyn_Enumfield* _temp41; struct Cyc_Absyn_Enumdecl* _temp43;
_LL29: if(*(( int*) _temp27) == Cyc_Tcenv_StructRes){ goto _LL30;} else{ goto
_LL31;} _LL31: if(*(( int*) _temp27) == Cyc_Tcenv_TunionRes){ _LL40: _temp39=((
struct Cyc_Tcenv_TunionRes_struct*) _temp27)->f1; goto _LL38; _LL38: _temp37=((
struct Cyc_Tcenv_TunionRes_struct*) _temp27)->f2; goto _LL32;} else{ goto _LL33;}
_LL33: if(*(( int*) _temp27) == Cyc_Tcenv_EnumRes){ _LL44: _temp43=(( struct Cyc_Tcenv_EnumRes_struct*)
_temp27)->f1; goto _LL42; _LL42: _temp41=(( struct Cyc_Tcenv_EnumRes_struct*)
_temp27)->f2; goto _LL34;} else{ goto _LL35;} _LL35: if(*(( int*) _temp27) ==
Cyc_Tcenv_VarRes){ goto _LL36;} else{ goto _LL28;} _LL30: Cyc_Tcutil_terr( p->loc,
_tag_arr("struct tag used without arguments in pattern", sizeof( unsigned char),
45u)); _npop_handler( 0u); return; _LL32:( void*)( p->r=( void*)(( void*)({
struct Cyc_Absyn_Tunion_p_struct* _temp45=( struct Cyc_Absyn_Tunion_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct)); _temp45[ 0]=({ struct Cyc_Absyn_Tunion_p_struct
_temp46; _temp46.tag= Cyc_Absyn_Tunion_p; _temp46.f1= _temp39; _temp46.f2=
_temp37; _temp46.f3= 0; _temp46.f4= 0; _temp46;}); _temp45;}))); _npop_handler(
0u); return; _LL34:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_p_struct*
_temp47=( struct Cyc_Absyn_Enum_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_p_struct));
_temp47[ 0]=({ struct Cyc_Absyn_Enum_p_struct _temp48; _temp48.tag= Cyc_Absyn_Enum_p;
_temp48.f1= _temp43; _temp48.f2= _temp41; _temp48;}); _temp47;})));
_npop_handler( 0u); return; _LL36: goto _LL28; _LL28:;}; _pop_handler();} else{
void* _temp25=( void*) _exn_thrown; void* _temp50= _temp25; _LL52: if( _temp50
== Cyc_Dict_Absent){ goto _LL53;} else{ goto _LL54;} _LL54: goto _LL55; _LL53:
goto _LL51; _LL55:( void) _throw( _temp50); _LL51:;}}}{ void* _temp56=(* _temp10).f1;
struct Cyc_List_List* _temp64; _LL58: if( _temp56 ==( void*) Cyc_Absyn_Loc_n){
goto _LL59;} else{ goto _LL60;} _LL60: if(( unsigned int) _temp56 > 1u?*(( int*)
_temp56) == Cyc_Absyn_Rel_n: 0){ _LL65: _temp64=(( struct Cyc_Absyn_Rel_n_struct*)
_temp56)->f1; if( _temp64 == 0){ goto _LL61;} else{ goto _LL62;}} else{ goto
_LL62;} _LL62: goto _LL63; _LL59: goto _LL61; _LL61:(* _temp10).f1=( void*) Cyc_Absyn_Loc_n;(
void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct* _temp66=( struct
Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp66[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp67; _temp67.tag= Cyc_Absyn_Var_p;
_temp67.f1= Cyc_Absyn_new_vardecl( _temp10,( void*) Cyc_Absyn_VoidType, 0);
_temp67;}); _temp66;}))); return; _LL63: Cyc_Tcutil_terr( p->loc, _tag_arr("qualified variable in pattern",
sizeof( unsigned char), 30u)); return; _LL57:;} _LL5:{ struct _handler_cons
_temp68; _push_handler(& _temp68);{ int _temp70= 0; if( setjmp( _temp68.handler)){
_temp70= 1;} if( ! _temp70){{ void* _temp71= Cyc_Tcenv_lookup_ordinary( te, p->loc,
_temp16); struct Cyc_Absyn_Structdecl* _temp81; struct Cyc_Absyn_Tunionfield*
_temp83; struct Cyc_Absyn_Tuniondecl* _temp85; _LL73: if(*(( int*) _temp71) ==
Cyc_Tcenv_StructRes){ _LL82: _temp81=(( struct Cyc_Tcenv_StructRes_struct*)
_temp71)->f1; goto _LL74;} else{ goto _LL75;} _LL75: if(*(( int*) _temp71) ==
Cyc_Tcenv_TunionRes){ _LL86: _temp85=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp71)->f1; goto _LL84; _LL84: _temp83=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp71)->f2; goto _LL76;} else{ goto _LL77;} _LL77: if(*(( int*) _temp71) ==
Cyc_Tcenv_EnumRes){ goto _LL78;} else{ goto _LL79;} _LL79: if(*(( int*) _temp71)
== Cyc_Tcenv_VarRes){ goto _LL80;} else{ goto _LL72;} _LL74: { struct Cyc_List_List*
_temp87= 0; for( 0; _temp12 != 0; _temp12=(( struct Cyc_List_List*) _check_null(
_temp12))->tl){ _temp87=({ struct Cyc_List_List* _temp88=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp88->hd=( void*)({ struct _tuple7*
_temp89=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp89->f1= 0;
_temp89->f2=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null(
_temp12))->hd; _temp89;}); _temp88->tl= _temp87; _temp88;});}( void*)( p->r=(
void*)(( void*)({ struct Cyc_Absyn_Struct_p_struct* _temp90=( struct Cyc_Absyn_Struct_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct)); _temp90[ 0]=({ struct Cyc_Absyn_Struct_p_struct
_temp91; _temp91.tag= Cyc_Absyn_Struct_p; _temp91.f1= _temp81; _temp91.f2= 0;
_temp91.f3= _temp14; _temp91.f4=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp87); _temp91;}); _temp90;}))); _npop_handler( 0u);
return;} _LL76:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_p_struct*
_temp92=( struct Cyc_Absyn_Tunion_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct));
_temp92[ 0]=({ struct Cyc_Absyn_Tunion_p_struct _temp93; _temp93.tag= Cyc_Absyn_Tunion_p;
_temp93.f1= _temp85; _temp93.f2= _temp83; _temp93.f3= _temp14; _temp93.f4=
_temp12; _temp93;}); _temp92;}))); _npop_handler( 0u); return; _LL78: Cyc_Tcutil_terr(
p->loc,( struct _tagged_arr)({ struct _tagged_arr _temp94= Cyc_Absynpp_qvar2string(
_temp16); xprintf("bad enum constructor %.*s in pattern", _get_arr_size( _temp94,
1u), _temp94.curr);})); _npop_handler( 0u); return; _LL80: goto _LL72; _LL72:;};
_pop_handler();} else{ void* _temp69=( void*) _exn_thrown; void* _temp96=
_temp69; _LL98: if( _temp96 == Cyc_Dict_Absent){ goto _LL99;} else{ goto _LL100;}
_LL100: goto _LL101; _LL99: goto _LL97; _LL101:( void) _throw( _temp96); _LL97:;}}}
Cyc_Tcutil_terr( p->loc,( struct _tagged_arr)({ struct _tagged_arr _temp102= Cyc_Absynpp_qvar2string(
_temp16); xprintf("%.*s is not a constructor in pattern", _get_arr_size(
_temp102, 1u), _temp102.curr);})); return; _LL7:{ struct _handler_cons _temp103;
_push_handler(& _temp103);{ int _temp105= 0; if( setjmp( _temp103.handler)){
_temp105= 1;} if( ! _temp105){{ void* _temp106= Cyc_Tcenv_lookup_ordinary( te, p->loc,
_temp22); struct Cyc_Absyn_Structdecl* _temp112; _LL108: if(*(( int*) _temp106)
== Cyc_Tcenv_StructRes){ _LL113: _temp112=(( struct Cyc_Tcenv_StructRes_struct*)
_temp106)->f1; goto _LL109;} else{ goto _LL110;} _LL110: goto _LL111; _LL109:(
void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_p_struct* _temp114=(
struct Cyc_Absyn_Struct_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct));
_temp114[ 0]=({ struct Cyc_Absyn_Struct_p_struct _temp115; _temp115.tag= Cyc_Absyn_Struct_p;
_temp115.f1= _temp112; _temp115.f2= 0; _temp115.f3= _temp20; _temp115.f4=
_temp18; _temp115;}); _temp114;}))); _npop_handler( 0u); return; _LL111: goto
_LL107; _LL107:;}; _pop_handler();} else{ void* _temp104=( void*) _exn_thrown;
void* _temp117= _temp104; _LL119: if( _temp117 == Cyc_Dict_Absent){ goto _LL120;}
else{ goto _LL121;} _LL121: goto _LL122; _LL120: goto _LL118; _LL122:( void)
_throw( _temp117); _LL118:;}}} Cyc_Tcutil_terr( p->loc,( struct _tagged_arr)({
struct _tagged_arr _temp123= Cyc_Absynpp_qvar2string( _temp22); xprintf("%.*s is not a struct name, but pattern has designators",
_get_arr_size( _temp123, 1u), _temp123.curr);})); return; _LL9: return; _LL1:;}
static struct _tagged_arr* Cyc_Tcpat_get_name( struct Cyc_Absyn_Vardecl* vd){
return(* vd->name).f2;} static struct Cyc_Core_Opt Cyc_Tcpat_mk={( void*)(( void*)
1u)}; static struct Cyc_Core_Opt Cyc_Tcpat_rk={( void*)(( void*) 3u)}; static
struct Cyc_Core_Opt Cyc_Tcpat_ak={( void*)(( void*) 0u)}; static void* Cyc_Tcpat_any_type(
struct Cyc_List_List* s, void** topt){ if( topt != 0){ return*(( void**)
_check_null( topt));} return Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcpat_mk,({
struct Cyc_Core_Opt* _temp124=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp124->v=( void*) s; _temp124;}));} static void* Cyc_Tcpat_num_type(
void** topt, void* numt){ if( topt != 0? Cyc_Tcutil_coerceable(*(( void**)
_check_null( topt))): 0){ return*(( void**) _check_null( topt));}{ void*
_temp125= Cyc_Tcutil_compress( numt); _LL127: if(( unsigned int) _temp125 > 4u?*((
int*) _temp125) == Cyc_Absyn_EnumType: 0){ goto _LL128;} else{ goto _LL129;}
_LL129: goto _LL130; _LL128: if( topt != 0){ return*(( void**) _check_null( topt));}
goto _LL126; _LL130: goto _LL126; _LL126:;} return numt;} static void Cyc_Tcpat_set_vd(
struct Cyc_Absyn_Vardecl* vd, struct Cyc_List_List** v_result_ptr, void* t){(
void*)( vd->type=( void*) t); vd->tq= Cyc_Absyn_empty_tqual();* v_result_ptr=({
struct Cyc_List_List* _temp131=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp131->hd=( void*) vd; _temp131->tl=* v_result_ptr;
_temp131;});} struct _tuple8{ struct Cyc_Absyn_Tqual f1; void* f2; } ; struct
_tuple9{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Pat* f2; } ; struct
_tuple6 Cyc_Tcpat_tcPatRec( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p,
void** topt, void** region_opt){ Cyc_Tcpat_resolve_pat( te, p);{ void* t; struct
Cyc_List_List* tv_result= 0; struct Cyc_List_List* v_result= 0;{ void* _temp132=(
void*) p->r; struct Cyc_Absyn_Vardecl* _temp166; struct Cyc_Absyn_Vardecl*
_temp168; void* _temp170; void* _temp172; struct Cyc_Absyn_Enumfield* _temp174;
struct Cyc_Absyn_Enumdecl* _temp176; struct Cyc_Absyn_Pat* _temp178; struct Cyc_List_List*
_temp180; struct Cyc_List_List* _temp182; struct Cyc_List_List* _temp184; struct
Cyc_Core_Opt* _temp186; struct Cyc_Core_Opt** _temp188; struct Cyc_Absyn_Structdecl*
_temp189; struct Cyc_List_List* _temp191; struct Cyc_List_List* _temp193; struct
Cyc_Absyn_Tunionfield* _temp195; struct Cyc_Absyn_Tuniondecl* _temp197; _LL134:
if( _temp132 ==( void*) Cyc_Absyn_Wild_p){ goto _LL135;} else{ goto _LL136;}
_LL136: if(( unsigned int) _temp132 > 2u?*(( int*) _temp132) == Cyc_Absyn_Var_p:
0){ _LL167: _temp166=(( struct Cyc_Absyn_Var_p_struct*) _temp132)->f1; goto
_LL137;} else{ goto _LL138;} _LL138: if(( unsigned int) _temp132 > 2u?*(( int*)
_temp132) == Cyc_Absyn_Reference_p: 0){ _LL169: _temp168=(( struct Cyc_Absyn_Reference_p_struct*)
_temp132)->f1; goto _LL139;} else{ goto _LL140;} _LL140: if(( unsigned int)
_temp132 > 2u?*(( int*) _temp132) == Cyc_Absyn_Int_p: 0){ _LL171: _temp170=(
void*)(( struct Cyc_Absyn_Int_p_struct*) _temp132)->f1; if( _temp170 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL141;} else{ goto _LL142;}} else{ goto _LL142;}
_LL142: if(( unsigned int) _temp132 > 2u?*(( int*) _temp132) == Cyc_Absyn_Int_p:
0){ _LL173: _temp172=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp132)->f1;
if( _temp172 ==( void*) Cyc_Absyn_Signed){ goto _LL143;} else{ goto _LL144;}}
else{ goto _LL144;} _LL144: if(( unsigned int) _temp132 > 2u?*(( int*) _temp132)
== Cyc_Absyn_Char_p: 0){ goto _LL145;} else{ goto _LL146;} _LL146: if((
unsigned int) _temp132 > 2u?*(( int*) _temp132) == Cyc_Absyn_Float_p: 0){ goto
_LL147;} else{ goto _LL148;} _LL148: if(( unsigned int) _temp132 > 2u?*(( int*)
_temp132) == Cyc_Absyn_Enum_p: 0){ _LL177: _temp176=(( struct Cyc_Absyn_Enum_p_struct*)
_temp132)->f1; goto _LL175; _LL175: _temp174=(( struct Cyc_Absyn_Enum_p_struct*)
_temp132)->f2; goto _LL149;} else{ goto _LL150;} _LL150: if( _temp132 ==( void*)
Cyc_Absyn_Null_p){ goto _LL151;} else{ goto _LL152;} _LL152: if(( unsigned int)
_temp132 > 2u?*(( int*) _temp132) == Cyc_Absyn_Pointer_p: 0){ _LL179: _temp178=((
struct Cyc_Absyn_Pointer_p_struct*) _temp132)->f1; goto _LL153;} else{ goto
_LL154;} _LL154: if(( unsigned int) _temp132 > 2u?*(( int*) _temp132) == Cyc_Absyn_Tuple_p:
0){ _LL181: _temp180=(( struct Cyc_Absyn_Tuple_p_struct*) _temp132)->f1; goto
_LL155;} else{ goto _LL156;} _LL156: if(( unsigned int) _temp132 > 2u?*(( int*)
_temp132) == Cyc_Absyn_Struct_p: 0){ _LL190: _temp189=(( struct Cyc_Absyn_Struct_p_struct*)
_temp132)->f1; goto _LL187; _LL187: _temp186=(( struct Cyc_Absyn_Struct_p_struct*)
_temp132)->f2; _temp188=&(( struct Cyc_Absyn_Struct_p_struct*) _temp132)->f2;
goto _LL185; _LL185: _temp184=(( struct Cyc_Absyn_Struct_p_struct*) _temp132)->f3;
goto _LL183; _LL183: _temp182=(( struct Cyc_Absyn_Struct_p_struct*) _temp132)->f4;
goto _LL157;} else{ goto _LL158;} _LL158: if(( unsigned int) _temp132 > 2u?*((
int*) _temp132) == Cyc_Absyn_Tunion_p: 0){ _LL198: _temp197=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp132)->f1; goto _LL196; _LL196: _temp195=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp132)->f2; goto _LL194; _LL194: _temp193=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp132)->f3; goto _LL192; _LL192: _temp191=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp132)->f4; goto _LL159;} else{ goto _LL160;} _LL160: if(( unsigned int)
_temp132 > 2u?*(( int*) _temp132) == Cyc_Absyn_UnknownId_p: 0){ goto _LL161;}
else{ goto _LL162;} _LL162: if(( unsigned int) _temp132 > 2u?*(( int*) _temp132)
== Cyc_Absyn_UnknownCall_p: 0){ goto _LL163;} else{ goto _LL164;} _LL164: if((
unsigned int) _temp132 > 2u?*(( int*) _temp132) == Cyc_Absyn_UnknownFields_p: 0){
goto _LL165;} else{ goto _LL133;} _LL135: t= Cyc_Tcpat_any_type( Cyc_Tcenv_lookup_type_vars(
te), topt); goto _LL133; _LL137: t= Cyc_Tcpat_any_type( Cyc_Tcenv_lookup_type_vars(
te), topt); Cyc_Tcpat_set_vd( _temp166,& v_result, t); goto _LL133; _LL139: t=
Cyc_Tcpat_any_type( Cyc_Tcenv_lookup_type_vars( te), topt); if( region_opt == 0){
Cyc_Tcutil_terr( p->loc, _tag_arr("* pattern would point into an unknown region",
sizeof( unsigned char), 45u)); goto _LL133;} Cyc_Tcpat_set_vd( _temp168,&
v_result,( void*)({ struct Cyc_Absyn_PointerType_struct* _temp199=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp199[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp200; _temp200.tag= Cyc_Absyn_PointerType;
_temp200.f1=({ struct Cyc_Absyn_PtrInfo _temp201; _temp201.elt_typ=( void*) t;
_temp201.rgn_typ=( void*)*(( void**) _check_null( region_opt)); _temp201.nullable=((
struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)( 0); _temp201.tq= Cyc_Absyn_empty_tqual();
_temp201.bounds= Cyc_Absyn_empty_conref(); _temp201;}); _temp200;}); _temp199;}));
goto _LL133; _LL141: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_uint_t); goto _LL133;
_LL143: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_sint_t); goto _LL133; _LL145: t=
Cyc_Tcpat_num_type( topt, Cyc_Absyn_uchar_t); goto _LL133; _LL147: t= Cyc_Tcpat_num_type(
topt, Cyc_Absyn_float_t); goto _LL133; _LL149: t= Cyc_Tcpat_num_type( topt,(
void*)({ struct Cyc_Absyn_EnumType_struct* _temp202=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp202[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp203; _temp203.tag= Cyc_Absyn_EnumType; _temp203.f1=
_temp176->name; _temp203.f2=( struct Cyc_Absyn_Enumdecl*) _temp176; _temp203;});
_temp202;})); goto _LL133; _LL151: if( topt != 0){ void* _temp204= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); _LL206: if(( unsigned int) _temp204 > 4u?*(( int*)
_temp204) == Cyc_Absyn_PointerType: 0){ goto _LL207;} else{ goto _LL208;} _LL208:
goto _LL209; _LL207: t=*(( void**) _check_null( topt)); goto tcpat_end; _LL209:
goto _LL205; _LL205:;}{ struct Cyc_List_List* _temp210= Cyc_Tcenv_lookup_type_vars(
te); t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp211=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp211[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp212; _temp212.tag= Cyc_Absyn_PointerType;
_temp212.f1=({ struct Cyc_Absyn_PtrInfo _temp213; _temp213.elt_typ=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcpat_ak,({ struct Cyc_Core_Opt* _temp215=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp215->v=( void*)
_temp210; _temp215;})); _temp213.rgn_typ=( void*) Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Tcpat_rk,({ struct Cyc_Core_Opt* _temp214=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp214->v=( void*) _temp210;
_temp214;})); _temp213.nullable=(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
1); _temp213.tq= Cyc_Absyn_empty_tqual(); _temp213.bounds= Cyc_Absyn_empty_conref();
_temp213;}); _temp212;}); _temp211;}); goto _LL133;} _LL153: { void* inner_typ=(
void*) Cyc_Absyn_VoidType; void** _temp216= 0; if( topt != 0){ void* _temp217=
Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo
_temp223; void* _temp225; _LL219: if(( unsigned int) _temp217 > 4u?*(( int*)
_temp217) == Cyc_Absyn_PointerType: 0){ _LL224: _temp223=(( struct Cyc_Absyn_PointerType_struct*)
_temp217)->f1; _LL226: _temp225=( void*) _temp223.elt_typ; goto _LL220;} else{
goto _LL221;} _LL221: goto _LL222; _LL220: inner_typ= _temp225; _temp216=( void**)&
inner_typ; goto _LL218; _LL222: goto _LL218; _LL218:;}{ void* ptr_rgn= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcpat_rk,({ struct Cyc_Core_Opt* _temp260=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp260->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp260;})); struct Cyc_List_List* _temp229;
struct Cyc_List_List* _temp231; struct _tuple6 _temp227= Cyc_Tcpat_tcPatRec( te,
_temp178, _temp216,( void**)& ptr_rgn); _LL232: _temp231= _temp227.f1; goto
_LL230; _LL230: _temp229= _temp227.f2; goto _LL228; _LL228: tv_result=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tv_result, _temp231); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( v_result, _temp229); if( _temp216
== 0){ void* _temp233= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp178->topt))->v); struct Cyc_Absyn_TunionFieldInfo _temp239;
struct Cyc_List_List* _temp241; void* _temp243; struct Cyc_Absyn_Tunionfield*
_temp245; struct Cyc_Absyn_Tuniondecl* _temp247; _LL235: if(( unsigned int)
_temp233 > 4u?*(( int*) _temp233) == Cyc_Absyn_TunionFieldType: 0){ _LL240:
_temp239=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp233)->f1; _LL244:
_temp243=( void*) _temp239.field_info; if(*(( int*) _temp243) == Cyc_Absyn_KnownTunionfield){
_LL248: _temp247=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp243)->f1;
goto _LL246; _LL246: _temp245=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp243)->f2; goto _LL242;} else{ goto _LL237;} _LL242: _temp241= _temp239.targs;
goto _LL236;} else{ goto _LL237;} _LL237: goto _LL238; _LL236: t=( void*)({
struct Cyc_Absyn_TunionType_struct* _temp249=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp249[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp250; _temp250.tag= Cyc_Absyn_TunionType;
_temp250.f1=({ struct Cyc_Absyn_TunionInfo _temp251; _temp251.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp252=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp252[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp253; _temp253.tag= Cyc_Absyn_KnownTunion;
_temp253.f1= _temp247; _temp253;}); _temp252;})); _temp251.targs= _temp241;
_temp251.rgn=( void*) ptr_rgn; _temp251;}); _temp250;}); _temp249;}); goto
_LL234; _LL238: t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp254=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp254[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp255; _temp255.tag= Cyc_Absyn_PointerType;
_temp255.f1=({ struct Cyc_Absyn_PtrInfo _temp256; _temp256.elt_typ=( void*)((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp178->topt))->v); _temp256.rgn_typ=(
void*) ptr_rgn; _temp256.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp256.tq= Cyc_Absyn_empty_tqual(); _temp256.bounds= Cyc_Absyn_empty_conref();
_temp256;}); _temp255;}); _temp254;}); goto _LL234; _LL234:;} else{ t=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp257=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp257[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp258; _temp258.tag= Cyc_Absyn_PointerType;
_temp258.f1=({ struct Cyc_Absyn_PtrInfo _temp259; _temp259.elt_typ=( void*)((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp178->topt))->v); _temp259.rgn_typ=(
void*) ptr_rgn; _temp259.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp259.tq= Cyc_Absyn_empty_tqual(); _temp259.bounds= Cyc_Absyn_empty_conref();
_temp259;}); _temp258;}); _temp257;});} goto _LL133;}} _LL155: { struct Cyc_List_List*
pat_ts= 0; struct Cyc_List_List* topt_ts= 0; if( topt != 0){ void* _temp261= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_List_List* _temp267; _LL263: if((
unsigned int) _temp261 > 4u?*(( int*) _temp261) == Cyc_Absyn_TupleType: 0){
_LL268: _temp267=(( struct Cyc_Absyn_TupleType_struct*) _temp261)->f1; goto
_LL264;} else{ goto _LL265;} _LL265: goto _LL266; _LL264: topt_ts= _temp267;
goto _LL262; _LL266: goto _LL262; _LL262:;} for( 0; _temp180 != 0; _temp180=((
struct Cyc_List_List*) _check_null( _temp180))->tl){ void** _temp269= 0; if(
topt_ts != 0){ _temp269=( void**)&(*(( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( topt_ts))->hd)).f2; topt_ts=(( struct Cyc_List_List*) _check_null(
topt_ts))->tl;}{ struct Cyc_List_List* _temp272; struct Cyc_List_List* _temp274;
struct _tuple6 _temp270= Cyc_Tcpat_tcPatRec( te,( struct Cyc_Absyn_Pat*)((
struct Cyc_List_List*) _check_null( _temp180))->hd, _temp269, region_opt);
_LL275: _temp274= _temp270.f1; goto _LL273; _LL273: _temp272= _temp270.f2; goto
_LL271; _LL271: tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp274); v_result=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
v_result, _temp272); pat_ts=({ struct Cyc_List_List* _temp276=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp276->hd=( void*)({ struct
_tuple8* _temp277=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8));
_temp277->f1= Cyc_Absyn_empty_tqual(); _temp277->f2=( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null(
_temp180))->hd)->topt))->v; _temp277;}); _temp276->tl= pat_ts; _temp276;});}} t=(
void*)({ struct Cyc_Absyn_TupleType_struct* _temp278=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp278[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp279; _temp279.tag= Cyc_Absyn_TupleType; _temp279.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( pat_ts);
_temp279;}); _temp278;}); goto _LL133;} _LL157: if( _temp184 != 0){( int) _throw(
Cyc_Tcutil_impos( _tag_arr("tcPat: struct<...> not implemented", sizeof(
unsigned char), 35u)));}{ struct _RegionHandle _temp280= _new_region(); struct
_RegionHandle* rgn=& _temp280; _push_region( rgn);{ struct _tuple4 _temp281=({
struct _tuple4 _temp305; _temp305.f1= Cyc_Tcenv_lookup_type_vars( te); _temp305.f2=
rgn; _temp305;}); struct Cyc_List_List* _temp282=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp281, _temp189->tvs); struct Cyc_List_List* _temp283=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp282); t=( void*)({ struct Cyc_Absyn_StructType_struct*
_temp284=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp284[ 0]=({ struct Cyc_Absyn_StructType_struct _temp285; _temp285.tag= Cyc_Absyn_StructType;
_temp285.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp189->name))->v); _temp285.f2= _temp283; _temp285.f3=({ struct
Cyc_Absyn_Structdecl** _temp286=( struct Cyc_Absyn_Structdecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl*)); _temp286[ 0]= _temp189; _temp286;});
_temp285;}); _temp284;}); if( topt != 0){ Cyc_Tcutil_unify( t,*(( void**)
_check_null( topt)));}* _temp188=({ struct Cyc_Core_Opt* _temp287=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp287->v=( void*) _temp283;
_temp287;}); if( _temp189->fields == 0){ Cyc_Tcutil_terr( p->loc, _tag_arr("can't destructure an abstract struct",
sizeof( unsigned char), 37u)); _npop_handler( 0u); goto _LL133;}{ struct Cyc_List_List*
fields=(( struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, p->loc, _temp182,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp189->fields))->v); for( 0; fields != 0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ struct _tuple9 _temp290; struct Cyc_Absyn_Pat*
_temp291; struct Cyc_Absyn_Structfield* _temp293; struct _tuple9* _temp288=(
struct _tuple9*)(( struct Cyc_List_List*) _check_null( fields))->hd; _temp290=*
_temp288; _LL294: _temp293= _temp290.f1; goto _LL292; _LL292: _temp291= _temp290.f2;
goto _LL289; _LL289: { void* _temp295= Cyc_Tcutil_rsubstitute( rgn, _temp282,(
void*) _temp293->type); struct Cyc_List_List* _temp298; struct Cyc_List_List*
_temp300; struct _tuple6 _temp296= Cyc_Tcpat_tcPatRec( te, _temp291,( void**)&
_temp295, region_opt); _LL301: _temp300= _temp296.f1; goto _LL299; _LL299:
_temp298= _temp296.f2; goto _LL297; _LL297: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp300); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp298); if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp291->topt))->v, _temp295)){ Cyc_Tcutil_terr(
p->loc,( struct _tagged_arr)({ struct _tagged_arr _temp302=* _temp293->name;
struct _tagged_arr _temp303= Cyc_Absynpp_typ2string( _temp295); struct
_tagged_arr _temp304= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp291->topt))->v); xprintf("field %.*s of struct pattern expects type %.*s != %.*s",
_get_arr_size( _temp302, 1u), _temp302.curr, _get_arr_size( _temp303, 1u),
_temp303.curr, _get_arr_size( _temp304, 1u), _temp304.curr);}));}}}}};
_pop_region( rgn);} goto _LL133; _LL159: if((( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp193) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp195->tvs)){ Cyc_Tcutil_terr( p->loc, _tag_arr("wrong number of existential type variables in pattern",
sizeof( unsigned char), 54u));}{ struct _RegionHandle _temp306= _new_region();
struct _RegionHandle* rgn=& _temp306; _push_region( rgn);{ struct Cyc_List_List*
_temp307= 0;{ struct Cyc_List_List* t= _temp193; for( 0; t != 0; t=(( struct Cyc_List_List*)
_check_null( t))->tl){ struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( t))->hd;{ void* _temp308=( void*)( Cyc_Absyn_compress_conref(
tv->kind))->v; void* _temp318; void* _temp320; _LL310: if(( unsigned int)
_temp308 > 1u?*(( int*) _temp308) == Cyc_Absyn_Eq_constr: 0){ _LL319: _temp318=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp308)->f1; if( _temp318 ==(
void*) Cyc_Absyn_BoxKind){ goto _LL311;} else{ goto _LL312;}} else{ goto _LL312;}
_LL312: if(( unsigned int) _temp308 > 1u?*(( int*) _temp308) == Cyc_Absyn_Eq_constr:
0){ _LL321: _temp320=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp308)->f1;
goto _LL313;} else{ goto _LL314;} _LL314: if( _temp308 ==( void*) Cyc_Absyn_No_constr){
goto _LL315;} else{ goto _LL316;} _LL316: if(( unsigned int) _temp308 > 1u?*((
int*) _temp308) == Cyc_Absyn_Forward_constr: 0){ goto _LL317;} else{ goto _LL309;}
_LL311: goto _LL309; _LL313: Cyc_Tcutil_terr( p->loc,( struct _tagged_arr)({
struct _tagged_arr _temp322= Cyc_Absynpp_qvar2string( _temp197->name); struct
_tagged_arr _temp323=* tv->name; struct _tagged_arr _temp324= Cyc_Absynpp_kind2string(
_temp320); xprintf("tunion member %.*s abstracts type variable %.*s of kind %.*s != B",
_get_arr_size( _temp322, 1u), _temp322.curr, _get_arr_size( _temp323, 1u),
_temp323.curr, _get_arr_size( _temp324, 1u), _temp324.curr);})); goto _LL309;
_LL315:( void*)(( tv->kind)->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp325=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp325[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp326; _temp326.tag= Cyc_Absyn_Eq_constr;
_temp326.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp326;}); _temp325;})));
goto _LL309; _LL317:( int) _throw( Cyc_Tcutil_impos( _tag_arr("forward after compress in conref",
sizeof( unsigned char), 33u))); _LL309:;} _temp307=({ struct Cyc_List_List*
_temp327=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp327->hd=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp328=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp328[ 0]=({ struct Cyc_Absyn_VarType_struct _temp329; _temp329.tag= Cyc_Absyn_VarType;
_temp329.f1= tv; _temp329;}); _temp328;})); _temp327->tl= _temp307; _temp327;});}}
_temp307= Cyc_List_imp_rev( _temp307); tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp193);{ struct Cyc_Tcenv_Tenv* te2= Cyc_Tcenv_add_type_vars( p->loc, te,
_temp193); struct Cyc_List_List* tqts= _temp195->typs; struct Cyc_List_List*
_temp330= Cyc_Tcenv_lookup_type_vars( te); struct _tuple4 _temp331=({ struct
_tuple4 _temp379; _temp379.f1= _temp330; _temp379.f2= rgn; _temp379;}); struct
Cyc_List_List* _temp332=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*), struct _tuple4*
env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp331, _temp197->tvs); struct Cyc_List_List* _temp333=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp195->tvs, _temp307);
struct Cyc_List_List* _temp334=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple5*))
Cyc_Core_snd, _temp332); struct Cyc_List_List* _temp335=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp333); struct Cyc_List_List* _temp336=((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp332, _temp333); if( tqts == 0){
t=( void*)({ struct Cyc_Absyn_TunionType_struct* _temp337=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp337[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp338; _temp338.tag= Cyc_Absyn_TunionType;
_temp338.f1=({ struct Cyc_Absyn_TunionInfo _temp339; _temp339.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp341=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp341[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp342; _temp342.tag= Cyc_Absyn_KnownTunion;
_temp342.f1= _temp197; _temp342;}); _temp341;})); _temp339.targs= _temp334;
_temp339.rgn=( void*) Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcpat_rk,({
struct Cyc_Core_Opt* _temp340=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp340->v=( void*) _temp330; _temp340;})); _temp339;});
_temp338;}); _temp337;});} else{ t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp343=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp343[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp344; _temp344.tag= Cyc_Absyn_TunionFieldType; _temp344.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp345; _temp345.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp346=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp346[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp347; _temp347.tag= Cyc_Absyn_KnownTunionfield; _temp347.f1= _temp197;
_temp347.f2= _temp195; _temp347;}); _temp346;})); _temp345.targs= _temp334;
_temp345;}); _temp344;}); _temp343;});} if( topt != 0? tqts == 0: 0){ void*
_temp348= Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_TunionInfo
_temp356; struct Cyc_List_List* _temp358; _LL350: if(( unsigned int) _temp348 >
4u?*(( int*) _temp348) == Cyc_Absyn_TunionFieldType: 0){ goto _LL351;} else{
goto _LL352;} _LL352: if(( unsigned int) _temp348 > 4u?*(( int*) _temp348) ==
Cyc_Absyn_TunionType: 0){ _LL357: _temp356=(( struct Cyc_Absyn_TunionType_struct*)
_temp348)->f1; _LL359: _temp358= _temp356.targs; goto _LL353;} else{ goto _LL354;}
_LL354: goto _LL355; _LL351: t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp360=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp360[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp361; _temp361.tag= Cyc_Absyn_TunionFieldType; _temp361.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp362; _temp362.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp363=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp363[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp364; _temp364.tag= Cyc_Absyn_KnownTunionfield; _temp364.f1= _temp197;
_temp364.f2= _temp195; _temp364;}); _temp363;})); _temp362.targs= _temp334;
_temp362;}); _temp361;}); _temp360;}); goto _LL349; _LL353: { struct Cyc_List_List*
_temp365= _temp334; for( 0; _temp365 != 0? _temp358 != 0: 0;( _temp365=(( struct
Cyc_List_List*) _check_null( _temp365))->tl, _temp358=(( struct Cyc_List_List*)
_check_null( _temp358))->tl)){ Cyc_Tcutil_unify(( void*)(( struct Cyc_List_List*)
_check_null( _temp365))->hd,( void*)(( struct Cyc_List_List*) _check_null(
_temp358))->hd);} goto _LL349;} _LL355: goto _LL349; _LL349:;} for( 0; _temp191
!= 0? tqts != 0: 0;( _temp191=(( struct Cyc_List_List*) _check_null( _temp191))->tl,
tqts=(( struct Cyc_List_List*) _check_null( tqts))->tl)){ struct Cyc_Absyn_Pat*
_temp366=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null( _temp191))->hd;
void* _temp367= Cyc_Tcutil_rsubstitute( rgn, _temp336,(*(( struct _tuple8*)((
struct Cyc_List_List*) _check_null( tqts))->hd)).f2); struct Cyc_List_List*
_temp370; struct Cyc_List_List* _temp372; struct _tuple6 _temp368= Cyc_Tcpat_tcPatRec(
te2, _temp366,( void**)& _temp367, region_opt); _LL373: _temp372= _temp368.f1;
goto _LL371; _LL371: _temp370= _temp368.f2; goto _LL369; _LL369: tv_result=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tv_result, _temp372); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( v_result, _temp370); if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp366->topt))->v, _temp367)){ Cyc_Tcutil_terr(
_temp366->loc,( struct _tagged_arr)({ struct _tagged_arr _temp374= Cyc_Absynpp_qvar2string(
_temp197->name); struct _tagged_arr _temp375= Cyc_Absynpp_typ2string( _temp367);
struct _tagged_arr _temp376= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp366->topt))->v); xprintf("%.*s expects argument type %.*s, not %.*s",
_get_arr_size( _temp374, 1u), _temp374.curr, _get_arr_size( _temp375, 1u),
_temp375.curr, _get_arr_size( _temp376, 1u), _temp376.curr);}));}} if( _temp191
!= 0){ Cyc_Tcutil_terr( p->loc,( struct _tagged_arr)({ struct _tagged_arr
_temp377= Cyc_Absynpp_qvar2string( _temp197->name); xprintf("too many arguments for tunion constructor %.*s",
_get_arr_size( _temp377, 1u), _temp377.curr);}));} if( tqts != 0){ Cyc_Tcutil_terr(
p->loc,( struct _tagged_arr)({ struct _tagged_arr _temp378= Cyc_Absynpp_qvar2string(
_temp197->name); xprintf("too few arguments for tunion constructor %.*s",
_get_arr_size( _temp378, 1u), _temp378.curr);}));}}}; _pop_region( rgn);} goto
_LL133; _LL161: goto _LL163; _LL163: goto _LL165; _LL165: t= Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt* _temp380=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp380->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp380;}));
goto _LL133; _LL133:;} tcpat_end: p->topt=({ struct Cyc_Core_Opt* _temp381=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp381->v=(
void*) t; _temp381;}); return({ struct _tuple6 _temp382; _temp382.f1= tv_result;
_temp382.f2= v_result; _temp382;});}} struct _tuple6 Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p, void** topt, void** region_opt){ struct _tuple6
_temp383= Cyc_Tcpat_tcPatRec( te, p, topt, region_opt);{ struct _RegionHandle
_temp384= _new_region(); struct _RegionHandle* r=& _temp384; _push_region( r);
Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tagged_arr*(* f)( struct Cyc_Absyn_Vardecl*), struct Cyc_List_List* x))
Cyc_List_rmap)( r, Cyc_Tcpat_get_name, _temp383.f2), p->loc, _tag_arr("pattern contains a repeated variable",
sizeof( unsigned char), 37u));; _pop_region( r);} return _temp383;} void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p){ void* _temp385=( void*) p->r;
struct Cyc_Absyn_Pat* _temp397; struct Cyc_List_List* _temp399; struct Cyc_List_List*
_temp401; struct Cyc_List_List* _temp403; _LL387: if(( unsigned int) _temp385 >
2u?*(( int*) _temp385) == Cyc_Absyn_Pointer_p: 0){ _LL398: _temp397=(( struct
Cyc_Absyn_Pointer_p_struct*) _temp385)->f1; goto _LL388;} else{ goto _LL389;}
_LL389: if(( unsigned int) _temp385 > 2u?*(( int*) _temp385) == Cyc_Absyn_Tunion_p:
0){ _LL400: _temp399=(( struct Cyc_Absyn_Tunion_p_struct*) _temp385)->f4; goto
_LL390;} else{ goto _LL391;} _LL391: if(( unsigned int) _temp385 > 2u?*(( int*)
_temp385) == Cyc_Absyn_Struct_p: 0){ _LL402: _temp401=(( struct Cyc_Absyn_Struct_p_struct*)
_temp385)->f4; goto _LL392;} else{ goto _LL393;} _LL393: if(( unsigned int)
_temp385 > 2u?*(( int*) _temp385) == Cyc_Absyn_Tuple_p: 0){ _LL404: _temp403=((
struct Cyc_Absyn_Tuple_p_struct*) _temp385)->f1; goto _LL394;} else{ goto _LL395;}
_LL395: goto _LL396; _LL388: Cyc_Tcpat_check_pat_regions( te, _temp397);{ void*
_temp405=( void*)(( struct Cyc_Core_Opt*) _check_null( p->topt))->v; struct Cyc_Absyn_PtrInfo
_temp413; void* _temp415; struct Cyc_Absyn_TunionInfo _temp417; void* _temp419;
_LL407: if(( unsigned int) _temp405 > 4u?*(( int*) _temp405) == Cyc_Absyn_PointerType:
0){ _LL414: _temp413=(( struct Cyc_Absyn_PointerType_struct*) _temp405)->f1;
_LL416: _temp415=( void*) _temp413.rgn_typ; goto _LL408;} else{ goto _LL409;}
_LL409: if(( unsigned int) _temp405 > 4u?*(( int*) _temp405) == Cyc_Absyn_TunionType:
0){ _LL418: _temp417=(( struct Cyc_Absyn_TunionType_struct*) _temp405)->f1;
_LL420: _temp419=( void*) _temp417.rgn; goto _LL410;} else{ goto _LL411;} _LL411:
goto _LL412; _LL408: _temp419= _temp415; goto _LL410; _LL410: Cyc_Tcenv_check_rgn_accessible(
te, p->loc, _temp419); return; _LL412:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp421=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp421[ 0]=({ struct Cyc_Core_Impossible_struct _temp422; _temp422.tag= Cyc_Core_Impossible;
_temp422.f1= _tag_arr("check_pat_regions: bad pointer type", sizeof(
unsigned char), 36u); _temp422;}); _temp421;})); _LL406:;} _LL390: for( 0;
_temp399 != 0; _temp399=(( struct Cyc_List_List*) _check_null( _temp399))->tl){
Cyc_Tcpat_check_pat_regions( te,( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp399))->hd);}{ void* _temp423=( void*)(( struct Cyc_Core_Opt*)
_check_null( p->topt))->v; struct Cyc_Absyn_TunionInfo _temp431; void* _temp433;
_LL425: if(( unsigned int) _temp423 > 4u?*(( int*) _temp423) == Cyc_Absyn_TunionType:
0){ _LL432: _temp431=(( struct Cyc_Absyn_TunionType_struct*) _temp423)->f1;
_LL434: _temp433=( void*) _temp431.rgn; goto _LL426;} else{ goto _LL427;} _LL427:
if(( unsigned int) _temp423 > 4u?*(( int*) _temp423) == Cyc_Absyn_TunionFieldType:
0){ goto _LL428;} else{ goto _LL429;} _LL429: goto _LL430; _LL426: Cyc_Tcenv_check_rgn_accessible(
te, p->loc, _temp433); return; _LL428: return; _LL430:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp435=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp435[ 0]=({ struct
Cyc_Core_Impossible_struct _temp436; _temp436.tag= Cyc_Core_Impossible; _temp436.f1=
_tag_arr("check_pat_regions: bad tunion type", sizeof( unsigned char), 35u);
_temp436;}); _temp435;})); _LL424:;} _LL392: _temp403=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Pat*(* f)( struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Pat*(*)( struct _tuple7*)) Cyc_Core_snd, _temp401); goto _LL394;
_LL394: for( 0; _temp403 != 0; _temp403=(( struct Cyc_List_List*) _check_null(
_temp403))->tl){ Cyc_Tcpat_check_pat_regions( te,( struct Cyc_Absyn_Pat*)((
struct Cyc_List_List*) _check_null( _temp403))->hd);} return; _LL396: return;
_LL386:;} static const int Cyc_Tcpat_Name_v= 0; struct Cyc_Tcpat_Name_v_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Tcpat_Int_v= 1; struct
Cyc_Tcpat_Int_v_struct{ int tag; int f1; } ; struct Cyc_Tcpat_Con_s{ void* name;
int arity; struct Cyc_Core_Opt* span; } ; static const int Cyc_Tcpat_Any= 0;
static const int Cyc_Tcpat_Con= 0; struct Cyc_Tcpat_Con_struct{ int tag; struct
Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2; } ; static int Cyc_Tcpat_compare_con(
struct Cyc_Tcpat_Con_s* c1, struct Cyc_Tcpat_Con_s* c2){ void* _temp437=( void*)
c1->name; struct _tagged_arr _temp443; int _temp445; _LL439: if(*(( int*)
_temp437) == Cyc_Tcpat_Name_v){ _LL444: _temp443=(( struct Cyc_Tcpat_Name_v_struct*)
_temp437)->f1; goto _LL440;} else{ goto _LL441;} _LL441: if(*(( int*) _temp437)
== Cyc_Tcpat_Int_v){ _LL446: _temp445=(( struct Cyc_Tcpat_Int_v_struct*)
_temp437)->f1; goto _LL442;} else{ goto _LL438;} _LL440: { void* _temp447=( void*)
c2->name; struct _tagged_arr _temp453; _LL449: if(*(( int*) _temp447) == Cyc_Tcpat_Name_v){
_LL454: _temp453=(( struct Cyc_Tcpat_Name_v_struct*) _temp447)->f1; goto _LL450;}
else{ goto _LL451;} _LL451: if(*(( int*) _temp447) == Cyc_Tcpat_Int_v){ goto
_LL452;} else{ goto _LL448;} _LL450: return Cyc_String_zstrcmp( _temp443,
_temp453); _LL452: return - 1; _LL448:;} _LL442: { void* _temp455=( void*) c2->name;
int _temp461; _LL457: if(*(( int*) _temp455) == Cyc_Tcpat_Name_v){ goto _LL458;}
else{ goto _LL459;} _LL459: if(*(( int*) _temp455) == Cyc_Tcpat_Int_v){ _LL462:
_temp461=(( struct Cyc_Tcpat_Int_v_struct*) _temp455)->f1; goto _LL460;} else{
goto _LL456;} _LL458: return 1; _LL460: return _temp445 - _temp461; _LL456:;}
_LL438:;} static struct Cyc_Set_Set* Cyc_Tcpat_empty_con_set(){ return(( struct
Cyc_Set_Set*(*)( int(* comp)( struct Cyc_Tcpat_Con_s*, struct Cyc_Tcpat_Con_s*)))
Cyc_Set_empty)( Cyc_Tcpat_compare_con);} static struct Cyc_Core_Opt Cyc_Tcpat_one_opt={(
void*) 1}; static struct Cyc_Core_Opt Cyc_Tcpat_two_opt={( void*) 2}; static
struct Cyc_Core_Opt* Cyc_Tcpat_one_opt_ptr=( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt;
static unsigned char _temp464[ 5u]="null"; static struct Cyc_Tcpat_Name_v_struct
Cyc_Tcpat_null_name_value={ 0u,{ _temp464, _temp464, _temp464 + 5u}}; static
unsigned char _temp466[ 4u]="ptr"; static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_ptr_name_value={
0u,{ _temp466, _temp466, _temp466 + 4u}}; static unsigned char _temp468[ 6u]="tuple";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={ 0u,{ _temp468,
_temp468, _temp468 + 6u}}; static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_con0={(
void*)(( void*)& Cyc_Tcpat_null_name_value), 0,( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt};
static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_ptr_con0={( void*)(( void*)& Cyc_Tcpat_ptr_name_value),
1,( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt}; static struct Cyc_Tcpat_Con_s Cyc_Tcpat_ptr_con0={(
void*)(( void*)& Cyc_Tcpat_ptr_name_value), 1,( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt};
static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_con=& Cyc_Tcpat_null_con0; static
struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_ptr_con=& Cyc_Tcpat_null_ptr_con0; static
struct Cyc_Tcpat_Con_s* Cyc_Tcpat_ptr_con=& Cyc_Tcpat_ptr_con0; static struct
Cyc_Tcpat_Con_s* Cyc_Tcpat_int_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp469=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp469->name=( void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp470=(
struct Cyc_Tcpat_Int_v_struct*) GC_malloc_atomic( sizeof( struct Cyc_Tcpat_Int_v_struct));
_temp470[ 0]=({ struct Cyc_Tcpat_Int_v_struct _temp471; _temp471.tag= Cyc_Tcpat_Int_v;
_temp471.f1= i; _temp471;}); _temp470;})); _temp469->arity= 0; _temp469->span= 0;
_temp469;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_float_con( struct
_tagged_arr f){ return({ struct Cyc_Tcpat_Con_s* _temp472=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp472->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp473=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp473[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp474; _temp474.tag= Cyc_Tcpat_Name_v; _temp474.f1= f; _temp474;}); _temp473;}));
_temp472->arity= 0; _temp472->span= 0; _temp472;});} static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_char_con( unsigned char c){ return({ struct Cyc_Tcpat_Con_s* _temp475=(
struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp475->name=(
void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp477=( struct Cyc_Tcpat_Int_v_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Tcpat_Int_v_struct)); _temp477[ 0]=({
struct Cyc_Tcpat_Int_v_struct _temp478; _temp478.tag= Cyc_Tcpat_Int_v; _temp478.f1=(
int) c; _temp478;}); _temp477;})); _temp475->arity= 0; _temp475->span=({ struct
Cyc_Core_Opt* _temp476=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct
Cyc_Core_Opt)); _temp476->v=( void*) 256; _temp476;}); _temp475;});} static
struct Cyc_Tcpat_Con_s* Cyc_Tcpat_tuple_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp479=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp479->name=( void*)(( void*)& Cyc_Tcpat_tuple_name_value); _temp479->arity=
i; _temp479->span= Cyc_Tcpat_one_opt_ptr; _temp479;});} static void* Cyc_Tcpat_null_pat(){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp480=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp480[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp481; _temp481.tag= Cyc_Tcpat_Con; _temp481.f1= Cyc_Tcpat_null_con; _temp481.f2=
0; _temp481;}); _temp480;});} static void* Cyc_Tcpat_int_pat( int i){ return(
void*)({ struct Cyc_Tcpat_Con_struct* _temp482=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp482[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp483; _temp483.tag= Cyc_Tcpat_Con; _temp483.f1= Cyc_Tcpat_int_con( i);
_temp483.f2= 0; _temp483;}); _temp482;});} static void* Cyc_Tcpat_char_pat(
unsigned char c){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp484=(
struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp484[ 0]=({ struct Cyc_Tcpat_Con_struct _temp485; _temp485.tag= Cyc_Tcpat_Con;
_temp485.f1= Cyc_Tcpat_char_con( c); _temp485.f2= 0; _temp485;}); _temp484;});}
static void* Cyc_Tcpat_float_pat( struct _tagged_arr f){ return( void*)({ struct
Cyc_Tcpat_Con_struct* _temp486=( struct Cyc_Tcpat_Con_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Con_struct)); _temp486[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp487; _temp487.tag= Cyc_Tcpat_Con; _temp487.f1= Cyc_Tcpat_float_con( f);
_temp487.f2= 0; _temp487;}); _temp486;});} static void* Cyc_Tcpat_null_ptr_pat(
void* p){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp488=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp488[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp489; _temp489.tag= Cyc_Tcpat_Con; _temp489.f1= Cyc_Tcpat_null_ptr_con;
_temp489.f2=({ struct Cyc_List_List* _temp490=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp490->hd=( void*) p; _temp490->tl= 0;
_temp490;}); _temp489;}); _temp488;});} static void* Cyc_Tcpat_ptr_pat( void* p){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp491=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp491[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp492; _temp492.tag= Cyc_Tcpat_Con; _temp492.f1= Cyc_Tcpat_ptr_con; _temp492.f2=({
struct Cyc_List_List* _temp493=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp493->hd=( void*) p; _temp493->tl= 0; _temp493;});
_temp492;}); _temp491;});} static void* Cyc_Tcpat_tuple_pat( struct Cyc_List_List*
ss){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp494=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp494[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp495; _temp495.tag= Cyc_Tcpat_Con; _temp495.f1= Cyc_Tcpat_tuple_con( Cyc_List_length(
ss)); _temp495.f2= ss; _temp495;}); _temp494;});} static void* Cyc_Tcpat_con_pat(
struct _tagged_arr con_name, struct Cyc_Core_Opt* span, struct Cyc_List_List* ps){
struct Cyc_Tcpat_Con_s* c=({ struct Cyc_Tcpat_Con_s* _temp498=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp498->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp499=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp499[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp500; _temp500.tag= Cyc_Tcpat_Name_v; _temp500.f1= con_name; _temp500;});
_temp499;})); _temp498->arity= Cyc_List_length( ps); _temp498->span= span;
_temp498;}); return( void*)({ struct Cyc_Tcpat_Con_struct* _temp496=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp496[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp497; _temp497.tag= Cyc_Tcpat_Con; _temp497.f1= c; _temp497.f2= ps; _temp497;});
_temp496;});} static void* Cyc_Tcpat_compile_pat( struct Cyc_Absyn_Pat* p){ void*
s;{ void* _temp501=( void*) p->r; int _temp535; void* _temp537; unsigned char
_temp539; struct _tagged_arr _temp541; struct Cyc_Absyn_Pat* _temp543; struct
Cyc_List_List* _temp545; struct Cyc_Absyn_Tunionfield* _temp547; struct Cyc_Absyn_Tuniondecl*
_temp549; struct Cyc_List_List* _temp551; struct Cyc_List_List* _temp553; struct
Cyc_List_List* _temp555; struct Cyc_Absyn_Structdecl* _temp557; struct Cyc_Absyn_Enumfield*
_temp559; struct Cyc_Absyn_Enumdecl* _temp561; _LL503: if( _temp501 ==( void*)
Cyc_Absyn_Wild_p){ goto _LL504;} else{ goto _LL505;} _LL505: if(( unsigned int)
_temp501 > 2u?*(( int*) _temp501) == Cyc_Absyn_Var_p: 0){ goto _LL506;} else{
goto _LL507;} _LL507: if( _temp501 ==( void*) Cyc_Absyn_Null_p){ goto _LL508;}
else{ goto _LL509;} _LL509: if(( unsigned int) _temp501 > 2u?*(( int*) _temp501)
== Cyc_Absyn_Int_p: 0){ _LL538: _temp537=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp501)->f1; goto _LL536; _LL536: _temp535=(( struct Cyc_Absyn_Int_p_struct*)
_temp501)->f2; goto _LL510;} else{ goto _LL511;} _LL511: if(( unsigned int)
_temp501 > 2u?*(( int*) _temp501) == Cyc_Absyn_Char_p: 0){ _LL540: _temp539=((
struct Cyc_Absyn_Char_p_struct*) _temp501)->f1; goto _LL512;} else{ goto _LL513;}
_LL513: if(( unsigned int) _temp501 > 2u?*(( int*) _temp501) == Cyc_Absyn_Float_p:
0){ _LL542: _temp541=(( struct Cyc_Absyn_Float_p_struct*) _temp501)->f1; goto
_LL514;} else{ goto _LL515;} _LL515: if(( unsigned int) _temp501 > 2u?*(( int*)
_temp501) == Cyc_Absyn_Reference_p: 0){ goto _LL516;} else{ goto _LL517;} _LL517:
if(( unsigned int) _temp501 > 2u?*(( int*) _temp501) == Cyc_Absyn_Pointer_p: 0){
_LL544: _temp543=(( struct Cyc_Absyn_Pointer_p_struct*) _temp501)->f1; goto
_LL518;} else{ goto _LL519;} _LL519: if(( unsigned int) _temp501 > 2u?*(( int*)
_temp501) == Cyc_Absyn_Tunion_p: 0){ _LL550: _temp549=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp501)->f1; goto _LL548; _LL548: _temp547=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp501)->f2; goto _LL546; _LL546: _temp545=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp501)->f4; if( _temp545 == 0){ goto _LL520;} else{ goto _LL521;}} else{ goto
_LL521;} _LL521: if(( unsigned int) _temp501 > 2u?*(( int*) _temp501) == Cyc_Absyn_Tunion_p:
0){ _LL552: _temp551=(( struct Cyc_Absyn_Tunion_p_struct*) _temp501)->f4; goto
_LL522;} else{ goto _LL523;} _LL523: if(( unsigned int) _temp501 > 2u?*(( int*)
_temp501) == Cyc_Absyn_Tuple_p: 0){ _LL554: _temp553=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp501)->f1; goto _LL524;} else{ goto _LL525;} _LL525: if(( unsigned int)
_temp501 > 2u?*(( int*) _temp501) == Cyc_Absyn_Struct_p: 0){ _LL558: _temp557=((
struct Cyc_Absyn_Struct_p_struct*) _temp501)->f1; goto _LL556; _LL556: _temp555=((
struct Cyc_Absyn_Struct_p_struct*) _temp501)->f4; goto _LL526;} else{ goto
_LL527;} _LL527: if(( unsigned int) _temp501 > 2u?*(( int*) _temp501) == Cyc_Absyn_Enum_p:
0){ _LL562: _temp561=(( struct Cyc_Absyn_Enum_p_struct*) _temp501)->f1; goto
_LL560; _LL560: _temp559=(( struct Cyc_Absyn_Enum_p_struct*) _temp501)->f2; goto
_LL528;} else{ goto _LL529;} _LL529: if(( unsigned int) _temp501 > 2u?*(( int*)
_temp501) == Cyc_Absyn_UnknownId_p: 0){ goto _LL530;} else{ goto _LL531;} _LL531:
if(( unsigned int) _temp501 > 2u?*(( int*) _temp501) == Cyc_Absyn_UnknownCall_p:
0){ goto _LL532;} else{ goto _LL533;} _LL533: if(( unsigned int) _temp501 > 2u?*((
int*) _temp501) == Cyc_Absyn_UnknownFields_p: 0){ goto _LL534;} else{ goto
_LL502;} _LL504: s=( void*) Cyc_Tcpat_Any; goto _LL502; _LL506: s=( void*) Cyc_Tcpat_Any;
goto _LL502; _LL508: s= Cyc_Tcpat_null_pat(); goto _LL502; _LL510: s= Cyc_Tcpat_int_pat(
_temp535); goto _LL502; _LL512: s= Cyc_Tcpat_char_pat( _temp539); goto _LL502;
_LL514: s= Cyc_Tcpat_float_pat( _temp541); goto _LL502; _LL516: s=( void*) Cyc_Tcpat_Any;
goto _LL502; _LL518:{ void* _temp563= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( p->topt))->v); struct Cyc_Absyn_PtrInfo _temp571; struct Cyc_Absyn_Conref*
_temp573; _LL565: if(( unsigned int) _temp563 > 4u?*(( int*) _temp563) == Cyc_Absyn_PointerType:
0){ _LL572: _temp571=(( struct Cyc_Absyn_PointerType_struct*) _temp563)->f1;
_LL574: _temp573= _temp571.nullable; goto _LL566;} else{ goto _LL567;} _LL567:
if(( unsigned int) _temp563 > 4u?*(( int*) _temp563) == Cyc_Absyn_TunionType: 0){
goto _LL568;} else{ goto _LL569;} _LL569: goto _LL570; _LL566: { int is_nullable=
0; int still_working= 1; while( still_working) { void* _temp575=( void*)
_temp573->v; struct Cyc_Absyn_Conref* _temp583; int _temp585; _LL577: if((
unsigned int) _temp575 > 1u?*(( int*) _temp575) == Cyc_Absyn_Forward_constr: 0){
_LL584: _temp583=(( struct Cyc_Absyn_Forward_constr_struct*) _temp575)->f1; goto
_LL578;} else{ goto _LL579;} _LL579: if( _temp575 ==( void*) Cyc_Absyn_No_constr){
goto _LL580;} else{ goto _LL581;} _LL581: if(( unsigned int) _temp575 > 1u?*((
int*) _temp575) == Cyc_Absyn_Eq_constr: 0){ _LL586: _temp585=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp575)->f1; goto _LL582;} else{ goto _LL576;} _LL578:( void*)( _temp573->v=(
void*)(( void*) _temp583->v)); continue; _LL580:( void*)( _temp573->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp587=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp587[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp588; _temp588.tag= Cyc_Absyn_Eq_constr; _temp588.f1=(
void*) 0; _temp588;}); _temp587;}))); is_nullable= 0; still_working= 0; goto
_LL576; _LL582: is_nullable=( int) _temp585; still_working= 0; goto _LL576;
_LL576:;}{ void* ss= Cyc_Tcpat_compile_pat( _temp543); if( is_nullable){ s= Cyc_Tcpat_null_ptr_pat(
ss);} else{ s= Cyc_Tcpat_ptr_pat( ss);} goto _LL564;}} _LL568:{ void* _temp589=(
void*) _temp543->r; struct Cyc_List_List* _temp595; struct Cyc_Absyn_Tunionfield*
_temp597; struct Cyc_Absyn_Tuniondecl* _temp599; _LL591: if(( unsigned int)
_temp589 > 2u?*(( int*) _temp589) == Cyc_Absyn_Tunion_p: 0){ _LL600: _temp599=((
struct Cyc_Absyn_Tunion_p_struct*) _temp589)->f1; goto _LL598; _LL598: _temp597=((
struct Cyc_Absyn_Tunion_p_struct*) _temp589)->f2; goto _LL596; _LL596: _temp595=((
struct Cyc_Absyn_Tunion_p_struct*) _temp589)->f4; goto _LL592;} else{ goto
_LL593;} _LL593: goto _LL594; _LL592: { struct Cyc_Core_Opt* span; if( _temp599->is_xtunion){
span= 0;} else{ span=({ struct Cyc_Core_Opt* _temp601=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp601->v=( void*)(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp599->fields))->v); _temp601;});} s= Cyc_Tcpat_con_pat(*(*
_temp597->name).f2, span,(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat, _temp595)); goto
_LL590;} _LL594: s= Cyc_Tcutil_impos( _tag_arr("non-[x]tunion pattern has tunion type",
sizeof( unsigned char), 38u)); goto _LL590; _LL590:;} goto _LL564; _LL570: s=
Cyc_Tcutil_impos( _tag_arr("pointer pattern does not have pointer type!",
sizeof( unsigned char), 44u)); goto _LL564; _LL564:;} goto _LL502; _LL520: {
struct Cyc_Core_Opt* span;{ void* _temp602= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( p->topt))->v); _LL604: if(( unsigned int)
_temp602 > 4u?*(( int*) _temp602) == Cyc_Absyn_TunionType: 0){ goto _LL605;}
else{ goto _LL606;} _LL606: if(( unsigned int) _temp602 > 4u?*(( int*) _temp602)
== Cyc_Absyn_TunionFieldType: 0){ goto _LL607;} else{ goto _LL608;} _LL608: goto
_LL609; _LL605: if( _temp549->is_xtunion){ span= 0;} else{ span=({ struct Cyc_Core_Opt*
_temp610=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct Cyc_Core_Opt));
_temp610->v=( void*)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp549->fields))->v);
_temp610;});} goto _LL603; _LL607: span=({ struct Cyc_Core_Opt* _temp611=(
struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp611->v=(
void*) 1; _temp611;}); goto _LL603; _LL609: span=(( struct Cyc_Core_Opt*(*)(
struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("void tunion pattern has bad type",
sizeof( unsigned char), 33u)); goto _LL603; _LL603:;} s= Cyc_Tcpat_con_pat(*(*
_temp547->name).f2, span, 0); goto _LL502;} _LL522: _temp553= _temp551; goto
_LL524; _LL524: s= Cyc_Tcpat_tuple_pat((( struct Cyc_List_List*(*)( void*(* f)(
struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp553)); goto _LL502; _LL526: { struct Cyc_List_List* ps= 0;{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp557->fields))->v;
for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){
int found= Cyc_String_strcmp(*(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fields))->hd)->name, _tag_arr("", sizeof( unsigned char), 1u)) == 0;{
struct Cyc_List_List* dlps0= _temp555; for( 0; ! found? dlps0 != 0: 0; dlps0=((
struct Cyc_List_List*) _check_null( dlps0))->tl){ struct _tuple7 _temp614;
struct Cyc_Absyn_Pat* _temp615; struct Cyc_List_List* _temp617; struct _tuple7*
_temp612=( struct _tuple7*)(( struct Cyc_List_List*) _check_null( dlps0))->hd;
_temp614=* _temp612; _LL618: _temp617= _temp614.f1; goto _LL616; _LL616:
_temp615= _temp614.f2; goto _LL613; _LL613: { struct Cyc_List_List* _temp619=
_temp617; struct Cyc_List_List _temp625; struct Cyc_List_List* _temp626; void*
_temp628; struct _tagged_arr* _temp630; _LL621: if( _temp619 == 0){ goto _LL623;}
else{ _temp625=* _temp619; _LL629: _temp628=( void*) _temp625.hd; if(*(( int*)
_temp628) == Cyc_Absyn_FieldName){ _LL631: _temp630=(( struct Cyc_Absyn_FieldName_struct*)
_temp628)->f1; goto _LL627;} else{ goto _LL623;} _LL627: _temp626= _temp625.tl;
if( _temp626 == 0){ goto _LL622;} else{ goto _LL623;}} _LL623: goto _LL624;
_LL622: if( Cyc_String_zstrptrcmp( _temp630,(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( fields))->hd)->name) == 0){ ps=({ struct Cyc_List_List*
_temp632=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp632->hd=( void*) Cyc_Tcpat_compile_pat( _temp615); _temp632->tl= ps;
_temp632;}); found= 1;} goto _LL620; _LL624:(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("bad designator(s)", sizeof( unsigned char), 18u)); goto _LL620; _LL620:;}}}
if( ! found){(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("bad designator",
sizeof( unsigned char), 15u));}}} s= Cyc_Tcpat_tuple_pat( ps); goto _LL502;}
_LL528: { int span=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp561->fields))->v);
s= Cyc_Tcpat_con_pat(*(* _temp559->name).f2,({ struct Cyc_Core_Opt* _temp633=(
struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp633->v=(
void*) span; _temp633;}), 0); goto _LL502;} _LL530: s= Cyc_Tcutil_impos(
_tag_arr("compile_pat: unknownid", sizeof( unsigned char), 23u)); goto _LL502;
_LL532: s= Cyc_Tcutil_impos( _tag_arr("compile_pat: unknowncall", sizeof(
unsigned char), 25u)); goto _LL502; _LL534: s= Cyc_Tcutil_impos( _tag_arr("compile_pat: unknownfields",
sizeof( unsigned char), 27u)); goto _LL502; _LL502:;} return s;} static const
int Cyc_Tcpat_Pos= 0; struct Cyc_Tcpat_Pos_struct{ int tag; struct Cyc_Tcpat_Con_s*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Tcpat_Neg= 1; struct Cyc_Tcpat_Neg_struct{
int tag; struct Cyc_Set_Set* f1; } ; static const int Cyc_Tcpat_Failure= 0;
static const int Cyc_Tcpat_Success= 0; struct Cyc_Tcpat_Success_struct{ int tag;
void* f1; } ; static const int Cyc_Tcpat_IfEq= 1; struct Cyc_Tcpat_IfEq_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Tcpat_Con_s* f2; void* f3; void*
f4; } ; struct _tuple10{ struct Cyc_List_List* f1; struct Cyc_List_List* f2;
struct Cyc_List_List* f3; } ; static const int Cyc_Tcpat_Yes= 0; static const
int Cyc_Tcpat_No= 1; static const int Cyc_Tcpat_Maybe= 2; static void* Cyc_Tcpat_add_neg(
void* td, struct Cyc_Tcpat_Con_s* c){ void* _temp634= td; struct Cyc_Set_Set*
_temp640; _LL636: if(*(( int*) _temp634) == Cyc_Tcpat_Neg){ _LL641: _temp640=((
struct Cyc_Tcpat_Neg_struct*) _temp634)->f1; goto _LL637;} else{ goto _LL638;}
_LL638: if(*(( int*) _temp634) == Cyc_Tcpat_Pos){ goto _LL639;} else{ goto
_LL635;} _LL637: if((( int(*)( struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s*
elt)) Cyc_Set_member)( _temp640, c)){(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("add_neg called when constructor already in set", sizeof( unsigned char),
47u));} if( c->span != 0?(( int(*)( struct Cyc_Set_Set* s)) Cyc_Set_cardinality)(
_temp640) + 1 >=( int)(( struct Cyc_Core_Opt*) _check_null( c->span))->v: 0){((
int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("add_neg called when |cs U {c}| >= span(c)",
sizeof( unsigned char), 42u));} return( void*)({ struct Cyc_Tcpat_Neg_struct*
_temp642=( struct Cyc_Tcpat_Neg_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct));
_temp642[ 0]=({ struct Cyc_Tcpat_Neg_struct _temp643; _temp643.tag= Cyc_Tcpat_Neg;
_temp643.f1=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s*
elt)) Cyc_Set_insert)( _temp640, c); _temp643;}); _temp642;}); _LL639: return
Cyc_Tcutil_impos( _tag_arr("add_neg called when td is Positive", sizeof(
unsigned char), 35u)); _LL635:;} static void* Cyc_Tcpat_static_match( struct Cyc_Tcpat_Con_s*
c, void* td){ void* _temp644= td; struct Cyc_Tcpat_Con_s* _temp650; struct Cyc_Set_Set*
_temp652; _LL646: if(*(( int*) _temp644) == Cyc_Tcpat_Pos){ _LL651: _temp650=((
struct Cyc_Tcpat_Pos_struct*) _temp644)->f1; goto _LL647;} else{ goto _LL648;}
_LL648: if(*(( int*) _temp644) == Cyc_Tcpat_Neg){ _LL653: _temp652=(( struct Cyc_Tcpat_Neg_struct*)
_temp644)->f1; goto _LL649;} else{ goto _LL645;} _LL647: if( Cyc_Tcpat_compare_con(
c, _temp650) == 0){ return( void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_No;}
_LL649: if((( int(*)( struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)(
_temp652, c)){ return( void*) Cyc_Tcpat_No;} else{ if( c->span != 0?( int)((
struct Cyc_Core_Opt*) _check_null( c->span))->v ==(( int(*)( struct Cyc_Set_Set*
s)) Cyc_Set_cardinality)( _temp652) + 1: 0){ return( void*) Cyc_Tcpat_Yes;}
else{ return( void*) Cyc_Tcpat_Maybe;}} _LL645:;} struct _tuple11{ struct Cyc_Tcpat_Con_s*
f1; struct Cyc_List_List* f2; } ; static struct Cyc_List_List* Cyc_Tcpat_augment(
struct Cyc_List_List* ctxt, void* dsc){ struct Cyc_List_List* _temp654= ctxt;
struct Cyc_List_List _temp660; struct Cyc_List_List* _temp661; struct _tuple11*
_temp663; struct _tuple11 _temp665; struct Cyc_List_List* _temp666; struct Cyc_Tcpat_Con_s*
_temp668; _LL656: if( _temp654 == 0){ goto _LL657;} else{ goto _LL658;} _LL658:
if( _temp654 == 0){ goto _LL655;} else{ _temp660=* _temp654; _LL664: _temp663=(
struct _tuple11*) _temp660.hd; _temp665=* _temp663; _LL669: _temp668= _temp665.f1;
goto _LL667; _LL667: _temp666= _temp665.f2; goto _LL662; _LL662: _temp661=
_temp660.tl; goto _LL659;} _LL657: return 0; _LL659: return({ struct Cyc_List_List*
_temp670=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp670->hd=( void*)({ struct _tuple11* _temp671=( struct _tuple11*) GC_malloc(
sizeof( struct _tuple11)); _temp671->f1= _temp668; _temp671->f2=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp672=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp672->hd=( void*) dsc; _temp672->tl= _temp666;
_temp672;}); _temp671;}); _temp670->tl= _temp661; _temp670;}); _LL655:;} static
struct Cyc_List_List* Cyc_Tcpat_norm_context( struct Cyc_List_List* ctxt){
struct Cyc_List_List* _temp673= ctxt; struct Cyc_List_List _temp679; struct Cyc_List_List*
_temp680; struct _tuple11* _temp682; struct _tuple11 _temp684; struct Cyc_List_List*
_temp685; struct Cyc_Tcpat_Con_s* _temp687; _LL675: if( _temp673 == 0){ goto
_LL676;} else{ goto _LL677;} _LL677: if( _temp673 == 0){ goto _LL674;} else{
_temp679=* _temp673; _LL683: _temp682=( struct _tuple11*) _temp679.hd; _temp684=*
_temp682; _LL688: _temp687= _temp684.f1; goto _LL686; _LL686: _temp685= _temp684.f2;
goto _LL681; _LL681: _temp680= _temp679.tl; goto _LL678;} _LL676: return((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("norm_context: empty context",
sizeof( unsigned char), 28u)); _LL678: return Cyc_Tcpat_augment( _temp680,( void*)({
struct Cyc_Tcpat_Pos_struct* _temp689=( struct Cyc_Tcpat_Pos_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Pos_struct)); _temp689[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp690; _temp690.tag= Cyc_Tcpat_Pos; _temp690.f1= _temp687; _temp690.f2= Cyc_List_rev(
_temp685); _temp690;}); _temp689;})); _LL674:;} static void* Cyc_Tcpat_build_desc(
struct Cyc_List_List* ctxt, void* dsc, struct Cyc_List_List* work){ struct
_tuple6 _temp692=({ struct _tuple6 _temp691; _temp691.f1= ctxt; _temp691.f2=
work; _temp691;}); struct Cyc_List_List* _temp702; struct Cyc_List_List*
_temp704; struct Cyc_List_List* _temp706; struct Cyc_List_List* _temp708; struct
Cyc_List_List* _temp710; struct Cyc_List_List _temp712; struct Cyc_List_List*
_temp713; struct _tuple10* _temp715; struct _tuple10 _temp717; struct Cyc_List_List*
_temp718; struct Cyc_List_List* _temp720; struct Cyc_List_List _temp722; struct
Cyc_List_List* _temp723; struct _tuple11* _temp725; struct _tuple11 _temp727;
struct Cyc_List_List* _temp728; struct Cyc_Tcpat_Con_s* _temp730; _LL694: _LL705:
_temp704= _temp692.f1; if( _temp704 == 0){ goto _LL703;} else{ goto _LL696;}
_LL703: _temp702= _temp692.f2; if( _temp702 == 0){ goto _LL695;} else{ goto
_LL696;} _LL696: _LL707: _temp706= _temp692.f1; if( _temp706 == 0){ goto _LL697;}
else{ goto _LL698;} _LL698: _LL709: _temp708= _temp692.f2; if( _temp708 == 0){
goto _LL699;} else{ goto _LL700;} _LL700: _LL721: _temp720= _temp692.f1; if(
_temp720 == 0){ goto _LL693;} else{ _temp722=* _temp720; _LL726: _temp725=(
struct _tuple11*) _temp722.hd; _temp727=* _temp725; _LL731: _temp730= _temp727.f1;
goto _LL729; _LL729: _temp728= _temp727.f2; goto _LL724; _LL724: _temp723=
_temp722.tl; goto _LL711;} _LL711: _temp710= _temp692.f2; if( _temp710 == 0){
goto _LL693;} else{ _temp712=* _temp710; _LL716: _temp715=( struct _tuple10*)
_temp712.hd; _temp717=* _temp715; _LL719: _temp718= _temp717.f3; goto _LL714;
_LL714: _temp713= _temp712.tl; goto _LL701;} _LL695: return dsc; _LL697: return
Cyc_Tcutil_impos( _tag_arr("build_desc: ctxt and work don't match", sizeof(
unsigned char), 38u)); _LL699: return Cyc_Tcutil_impos( _tag_arr("build_desc: ctxt and work don't match",
sizeof( unsigned char), 38u)); _LL701: { struct Cyc_Tcpat_Pos_struct* _temp732=({
struct Cyc_Tcpat_Pos_struct* _temp733=( struct Cyc_Tcpat_Pos_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Pos_struct)); _temp733[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp734; _temp734.tag= Cyc_Tcpat_Pos; _temp734.f1= _temp730; _temp734.f2= Cyc_List_append(
Cyc_List_rev( _temp728),({ struct Cyc_List_List* _temp735=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp735->hd=( void*) dsc; _temp735->tl=
_temp718; _temp735;})); _temp734;}); _temp733;}); return Cyc_Tcpat_build_desc(
_temp723,( void*) _temp732, _temp713);} _LL693:;} static void* Cyc_Tcpat_match(
void* p, struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List* ctx, struct
Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List* rules); static
void* Cyc_Tcpat_or_match( void* dsc, struct Cyc_List_List* allmrules){ struct
Cyc_List_List* _temp736= allmrules; struct Cyc_List_List _temp742; struct Cyc_List_List*
_temp743; struct _tuple0* _temp745; struct _tuple0 _temp747; void* _temp748;
void* _temp750; _LL738: if( _temp736 == 0){ goto _LL739;} else{ goto _LL740;}
_LL740: if( _temp736 == 0){ goto _LL737;} else{ _temp742=* _temp736; _LL746:
_temp745=( struct _tuple0*) _temp742.hd; _temp747=* _temp745; _LL751: _temp750=
_temp747.f1; goto _LL749; _LL749: _temp748= _temp747.f2; goto _LL744; _LL744:
_temp743= _temp742.tl; goto _LL741;} _LL739: return( void*) Cyc_Tcpat_Failure;
_LL741: return Cyc_Tcpat_match( _temp750, 0, dsc, 0, 0, _temp748, _temp743);
_LL737:;} static void* Cyc_Tcpat_match_compile( struct Cyc_List_List* allmrules){
return Cyc_Tcpat_or_match(( void*)({ struct Cyc_Tcpat_Neg_struct* _temp752=(
struct Cyc_Tcpat_Neg_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct));
_temp752[ 0]=({ struct Cyc_Tcpat_Neg_struct _temp753; _temp753.tag= Cyc_Tcpat_Neg;
_temp753.f1= Cyc_Tcpat_empty_con_set(); _temp753;}); _temp752;}), allmrules);}
static void* Cyc_Tcpat_and_match( struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules){ struct Cyc_List_List*
_temp754= work; struct Cyc_List_List _temp762; struct Cyc_List_List* _temp763;
struct _tuple10* _temp765; struct _tuple10 _temp767; struct Cyc_List_List*
_temp768; struct Cyc_List_List* _temp770; struct Cyc_List_List* _temp772; struct
Cyc_List_List _temp774; struct Cyc_List_List* _temp775; struct _tuple10*
_temp777; struct _tuple10 _temp779; struct Cyc_List_List* _temp780; struct Cyc_List_List*
_temp782; struct Cyc_List_List* _temp784; _LL756: if( _temp754 == 0){ goto
_LL757;} else{ goto _LL758;} _LL758: if( _temp754 == 0){ goto _LL760;} else{
_temp762=* _temp754; _LL766: _temp765=( struct _tuple10*) _temp762.hd; _temp767=*
_temp765; _LL773: _temp772= _temp767.f1; if( _temp772 == 0){ goto _LL771;} else{
goto _LL760;} _LL771: _temp770= _temp767.f2; if( _temp770 == 0){ goto _LL769;}
else{ goto _LL760;} _LL769: _temp768= _temp767.f3; if( _temp768 == 0){ goto
_LL764;} else{ goto _LL760;} _LL764: _temp763= _temp762.tl; goto _LL759;} _LL760:
if( _temp754 == 0){ goto _LL755;} else{ _temp774=* _temp754; _LL778: _temp777=(
struct _tuple10*) _temp774.hd; _temp779=* _temp777; _LL785: _temp784= _temp779.f1;
goto _LL783; _LL783: _temp782= _temp779.f2; goto _LL781; _LL781: _temp780=
_temp779.f3; goto _LL776; _LL776: _temp775= _temp774.tl; goto _LL761;} _LL757:
return( void*)({ struct Cyc_Tcpat_Success_struct* _temp786=( struct Cyc_Tcpat_Success_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Success_struct)); _temp786[ 0]=({ struct Cyc_Tcpat_Success_struct
_temp787; _temp787.tag= Cyc_Tcpat_Success; _temp787.f1=( void*) right_hand_side;
_temp787;}); _temp786;}); _LL759: return Cyc_Tcpat_and_match( Cyc_Tcpat_norm_context(
ctx), _temp763, right_hand_side, rules); _LL761: if(( _temp784 == 0? 1: _temp782
== 0)? 1: _temp780 == 0){ return Cyc_Tcutil_impos( _tag_arr("tcpat:and_match: malformed work frame",
sizeof( unsigned char), 38u));}{ struct Cyc_List_List _temp790; struct Cyc_List_List*
_temp791; void* _temp793; struct Cyc_List_List* _temp788=( struct Cyc_List_List*)
_check_null( _temp784); _temp790=* _temp788; _LL794: _temp793=( void*) _temp790.hd;
goto _LL792; _LL792: _temp791= _temp790.tl; goto _LL789; _LL789: { struct Cyc_List_List
_temp797; struct Cyc_List_List* _temp798; struct Cyc_List_List* _temp800; struct
Cyc_List_List* _temp795=( struct Cyc_List_List*) _check_null( _temp782);
_temp797=* _temp795; _LL801: _temp800=( struct Cyc_List_List*) _temp797.hd; goto
_LL799; _LL799: _temp798= _temp797.tl; goto _LL796; _LL796: { struct Cyc_List_List
_temp804; struct Cyc_List_List* _temp805; void* _temp807; struct Cyc_List_List*
_temp802=( struct Cyc_List_List*) _check_null( _temp780); _temp804=* _temp802;
_LL808: _temp807=( void*) _temp804.hd; goto _LL806; _LL806: _temp805= _temp804.tl;
goto _LL803; _LL803: { struct _tuple10* _temp809=({ struct _tuple10* _temp811=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp811->f1= _temp791;
_temp811->f2= _temp798; _temp811->f3= _temp805; _temp811;}); return Cyc_Tcpat_match(
_temp793, _temp800, _temp807, ctx,({ struct Cyc_List_List* _temp810=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp810->hd=( void*) _temp809;
_temp810->tl= _temp775; _temp810;}), right_hand_side, rules);}}}} _LL755:;}
static struct Cyc_List_List* Cyc_Tcpat_getdargs( struct Cyc_Tcpat_Con_s* pcon,
void* dsc){ void* _temp812= dsc; struct Cyc_Set_Set* _temp818; struct Cyc_List_List*
_temp820; _LL814: if(*(( int*) _temp812) == Cyc_Tcpat_Neg){ _LL819: _temp818=((
struct Cyc_Tcpat_Neg_struct*) _temp812)->f1; goto _LL815;} else{ goto _LL816;}
_LL816: if(*(( int*) _temp812) == Cyc_Tcpat_Pos){ _LL821: _temp820=(( struct Cyc_Tcpat_Pos_struct*)
_temp812)->f2; goto _LL817;} else{ goto _LL813;} _LL815: { void* any=( void*)({
struct Cyc_Tcpat_Neg_struct* _temp824=( struct Cyc_Tcpat_Neg_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Neg_struct)); _temp824[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp825; _temp825.tag= Cyc_Tcpat_Neg; _temp825.f1= Cyc_Tcpat_empty_con_set();
_temp825;}); _temp824;}); struct Cyc_List_List* _temp822= 0;{ int i= 0; for( 0;
i < pcon->arity; ++ i){ _temp822=({ struct Cyc_List_List* _temp823=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp823->hd=( void*) any; _temp823->tl=
_temp822; _temp823;});}} return _temp822;} _LL817: return _temp820; _LL813:;}
static struct Cyc_List_List* Cyc_Tcpat_getoarg( struct Cyc_List_List* obj, int i){
return({ struct Cyc_List_List* _temp826=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp826->hd=( void*)( i + 1); _temp826->tl= obj;
_temp826;});} static struct Cyc_List_List* Cyc_Tcpat_getoargs( struct Cyc_Tcpat_Con_s*
pcon, struct Cyc_List_List* obj){ return(( struct Cyc_List_List*(*)( int n,
struct Cyc_List_List*(* f)( struct Cyc_List_List*, int), struct Cyc_List_List*
env)) Cyc_List_tabulate_c)( pcon->arity, Cyc_Tcpat_getoarg, obj);} static void*
Cyc_Tcpat_match( void* p, struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules){ void* _temp827= p; struct Cyc_List_List* _temp833; struct Cyc_Tcpat_Con_s*
_temp835; _LL829: if( _temp827 ==( void*) Cyc_Tcpat_Any){ goto _LL830;} else{
goto _LL831;} _LL831: if(( unsigned int) _temp827 > 1u?*(( int*) _temp827) ==
Cyc_Tcpat_Con: 0){ _LL836: _temp835=(( struct Cyc_Tcpat_Con_struct*) _temp827)->f1;
goto _LL834; _LL834: _temp833=(( struct Cyc_Tcpat_Con_struct*) _temp827)->f2;
goto _LL832;} else{ goto _LL828;} _LL830: return Cyc_Tcpat_and_match( Cyc_Tcpat_augment(
ctx, dsc), work, right_hand_side, rules); _LL832: { void* _temp837= Cyc_Tcpat_static_match(
_temp835, dsc); _LL839: if( _temp837 ==( void*) Cyc_Tcpat_Yes){ goto _LL840;}
else{ goto _LL841;} _LL841: if( _temp837 ==( void*) Cyc_Tcpat_No){ goto _LL842;}
else{ goto _LL843;} _LL843: if( _temp837 ==( void*) Cyc_Tcpat_Maybe){ goto
_LL844;} else{ goto _LL838;} _LL840: { struct Cyc_List_List* _temp845=({ struct
Cyc_List_List* _temp850=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp850->hd=( void*)({ struct _tuple11* _temp851=( struct _tuple11*) GC_malloc(
sizeof( struct _tuple11)); _temp851->f1= _temp835; _temp851->f2= 0; _temp851;});
_temp850->tl= ctx; _temp850;}); struct _tuple10* _temp846=({ struct _tuple10*
_temp849=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp849->f1=
_temp833; _temp849->f2= Cyc_Tcpat_getoargs( _temp835, obj); _temp849->f3= Cyc_Tcpat_getdargs(
_temp835, dsc); _temp849;}); struct Cyc_List_List* _temp847=({ struct Cyc_List_List*
_temp848=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp848->hd=( void*) _temp846; _temp848->tl= work; _temp848;}); return Cyc_Tcpat_and_match(
_temp845, _temp847, right_hand_side, rules);} _LL842: return Cyc_Tcpat_or_match(
Cyc_Tcpat_build_desc( ctx, dsc, work), rules); _LL844: { struct Cyc_List_List*
_temp852=({ struct Cyc_List_List* _temp861=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp861->hd=( void*)({ struct _tuple11*
_temp862=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp862->f1=
_temp835; _temp862->f2= 0; _temp862;}); _temp861->tl= ctx; _temp861;}); struct
_tuple10* _temp853=({ struct _tuple10* _temp860=( struct _tuple10*) GC_malloc(
sizeof( struct _tuple10)); _temp860->f1= _temp833; _temp860->f2= Cyc_Tcpat_getoargs(
_temp835, obj); _temp860->f3= Cyc_Tcpat_getdargs( _temp835, dsc); _temp860;});
struct Cyc_List_List* _temp854=({ struct Cyc_List_List* _temp859=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp859->hd=( void*) _temp853;
_temp859->tl= work; _temp859;}); void* _temp855= Cyc_Tcpat_and_match( _temp852,
_temp854, right_hand_side, rules); void* _temp856= Cyc_Tcpat_or_match( Cyc_Tcpat_build_desc(
ctx, Cyc_Tcpat_add_neg( dsc, _temp835), work), rules); return( void*)({ struct
Cyc_Tcpat_IfEq_struct* _temp857=( struct Cyc_Tcpat_IfEq_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_IfEq_struct)); _temp857[ 0]=({ struct Cyc_Tcpat_IfEq_struct
_temp858; _temp858.tag= Cyc_Tcpat_IfEq; _temp858.f1= obj; _temp858.f2= _temp835;
_temp858.f3=( void*) _temp855; _temp858.f4=( void*) _temp856; _temp858;});
_temp857;});} _LL838:;} _LL828:;} static void Cyc_Tcpat_check_exhaust_overlap(
void* d, void(* not_exhaust)( void*), void* env1, void(* rhs_appears)( void*,
void*), void* env2){ void* _temp863= d; void* _temp871; void* _temp873; void*
_temp875; _LL865: if( _temp863 ==( void*) Cyc_Tcpat_Failure){ goto _LL866;}
else{ goto _LL867;} _LL867: if(( unsigned int) _temp863 > 1u?*(( int*) _temp863)
== Cyc_Tcpat_Success: 0){ _LL872: _temp871=( void*)(( struct Cyc_Tcpat_Success_struct*)
_temp863)->f1; goto _LL868;} else{ goto _LL869;} _LL869: if(( unsigned int)
_temp863 > 1u?*(( int*) _temp863) == Cyc_Tcpat_IfEq: 0){ _LL876: _temp875=( void*)((
struct Cyc_Tcpat_IfEq_struct*) _temp863)->f3; goto _LL874; _LL874: _temp873=(
void*)(( struct Cyc_Tcpat_IfEq_struct*) _temp863)->f4; goto _LL870;} else{ goto
_LL864;} _LL866: not_exhaust( env1); goto _LL864; _LL868: rhs_appears( env2,
_temp871); goto _LL864; _LL870: Cyc_Tcpat_check_exhaust_overlap( _temp875,
not_exhaust, env1, rhs_appears, env2); Cyc_Tcpat_check_exhaust_overlap( _temp873,
not_exhaust, env1, rhs_appears, env2); goto _LL864; _LL864:;} struct _tuple12{
int f1; struct Cyc_Position_Segment* f2; } ; struct _tuple13{ void* f1; struct
_tuple12* f2; } ; static struct _tuple13* Cyc_Tcpat_get_match( int* r, struct
Cyc_Absyn_Switch_clause* swc){ void* sp0= Cyc_Tcpat_compile_pat( swc->pattern);
struct _tuple12* rhs=({ struct _tuple12* _temp882=( struct _tuple12*) GC_malloc(
sizeof( struct _tuple12)); _temp882->f1= 0; _temp882->f2=( swc->pattern)->loc;
_temp882;}); void* sp; if( swc->where_clause != 0){ sp= Cyc_Tcpat_tuple_pat(({
struct Cyc_List_List* _temp877=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp877->hd=( void*) sp0; _temp877->tl=({ struct Cyc_List_List*
_temp878=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp878->hd=( void*) Cyc_Tcpat_int_pat(* r); _temp878->tl= 0; _temp878;});
_temp877;}));* r=* r + 1;} else{ sp= Cyc_Tcpat_tuple_pat(({ struct Cyc_List_List*
_temp879=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp879->hd=( void*) sp0; _temp879->tl=({ struct Cyc_List_List* _temp880=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp880->hd=(
void*)(( void*) Cyc_Tcpat_Any); _temp880->tl= 0; _temp880;}); _temp879;}));}
return({ struct _tuple13* _temp881=( struct _tuple13*) GC_malloc( sizeof( struct
_tuple13)); _temp881->f1= sp; _temp881->f2= rhs; _temp881;});} static void Cyc_Tcpat_not_exhaust_err(
struct Cyc_Position_Segment* loc){ Cyc_Tcutil_terr( loc, _tag_arr("patterns may not be exhaustive",
sizeof( unsigned char), 31u));} static void Cyc_Tcpat_rule_occurs( int dummy,
struct _tuple12* rhs){(* rhs).f1= 1;} void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ int _temp883= 0;
struct Cyc_List_List* _temp884=(( struct Cyc_List_List*(*)( struct _tuple13*(* f)(
int*, struct Cyc_Absyn_Switch_clause*), int* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Tcpat_get_match,& _temp883, swcs); void* _temp885=(( void*(*)( struct Cyc_List_List*
allmrules)) Cyc_Tcpat_match_compile)( _temp884);(( void(*)( void* d, void(*
not_exhaust)( struct Cyc_Position_Segment*), struct Cyc_Position_Segment* env1,
void(* rhs_appears)( int, struct _tuple12*), int env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp885, Cyc_Tcpat_not_exhaust_err, loc, Cyc_Tcpat_rule_occurs, 0); for( 0;
_temp884 != 0; _temp884=(( struct Cyc_List_List*) _check_null( _temp884))->tl){
struct _tuple13 _temp888; struct _tuple12* _temp889; struct _tuple12 _temp891;
struct Cyc_Position_Segment* _temp892; int _temp894; struct _tuple13* _temp886=(
struct _tuple13*)(( struct Cyc_List_List*) _check_null( _temp884))->hd; _temp888=*
_temp886; _LL890: _temp889= _temp888.f2; _temp891=* _temp889; _LL895: _temp894=
_temp891.f1; goto _LL893; _LL893: _temp892= _temp891.f2; goto _LL887; _LL887:
if( ! _temp894){ Cyc_Tcutil_terr( _temp892, _tag_arr("redundant pattern",
sizeof( unsigned char), 18u));}}} struct _tuple14{ struct Cyc_Position_Segment*
f1; int f2; } ; static void Cyc_Tcpat_not_exhaust_warn( struct _tuple14* pr){(*
pr).f2= 0; Cyc_Tcutil_warn((* pr).f1, _tag_arr("pattern not exhaustive", sizeof(
unsigned char), 23u));} static void Cyc_Tcpat_dummy_fn( int i, int j){ return;}
struct _tuple15{ void* f1; int f2; } ; int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Pat* p){ struct Cyc_List_List*
_temp896=({ struct Cyc_List_List* _temp900=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp900->hd=( void*)({ struct _tuple15*
_temp901=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp901->f1=
Cyc_Tcpat_compile_pat( p); _temp901->f2= 0; _temp901;}); _temp900->tl= 0;
_temp900;}); void* _temp897=(( void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
_temp896); struct _tuple14* _temp898=({ struct _tuple14* _temp899=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp899->f1= loc; _temp899->f2=
1; _temp899;});(( void(*)( void* d, void(* not_exhaust)( struct _tuple14*),
struct _tuple14* env1, void(* rhs_appears)( int, int), int env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp897, Cyc_Tcpat_not_exhaust_warn, _temp898, Cyc_Tcpat_dummy_fn, 0); return(*
_temp898).f2;} static struct _tuple13* Cyc_Tcpat_get_match2( struct Cyc_Absyn_Switch_clause*
swc){ void* sp0= Cyc_Tcpat_compile_pat( swc->pattern); struct _tuple12* rhs=({
struct _tuple12* _temp903=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12));
_temp903->f1= 0; _temp903->f2=( swc->pattern)->loc; _temp903;}); return({ struct
_tuple13* _temp902=( struct _tuple13*) GC_malloc( sizeof( struct _tuple13));
_temp902->f1= sp0; _temp902->f2= rhs; _temp902;});} static void Cyc_Tcpat_not_exhaust_err2(
struct Cyc_Position_Segment* loc){;} void Cyc_Tcpat_check_catch_overlap( struct
Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ struct Cyc_List_List*
_temp904=(( struct Cyc_List_List*(*)( struct _tuple13*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_get_match2, swcs); void*
_temp905=(( void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
_temp904);(( void(*)( void* d, void(* not_exhaust)( struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env1, void(* rhs_appears)( int, struct _tuple12*),
int env2)) Cyc_Tcpat_check_exhaust_overlap)( _temp905, Cyc_Tcpat_not_exhaust_err2,
loc, Cyc_Tcpat_rule_occurs, 0); for( 0; _temp904 != 0; _temp904=(( struct Cyc_List_List*)
_check_null( _temp904))->tl){ struct _tuple13 _temp908; struct _tuple12*
_temp909; struct _tuple12 _temp911; struct Cyc_Position_Segment* _temp912; int
_temp914; struct _tuple13* _temp906=( struct _tuple13*)(( struct Cyc_List_List*)
_check_null( _temp904))->hd; _temp908=* _temp906; _LL910: _temp909= _temp908.f2;
_temp911=* _temp909; _LL915: _temp914= _temp911.f1; goto _LL913; _LL913:
_temp912= _temp911.f2; goto _LL907; _LL907: if( ! _temp914){ Cyc_Tcutil_terr(
_temp912, _tag_arr("redundant pattern", sizeof( unsigned char), 18u));}}}