 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_dirent{ int d_ino; int d_off; unsigned short
d_reclen; unsigned char d_type; unsigned char d_name[ 256u]; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern
struct _tagged_arr Cyc_Core_new_string( int); extern int Cyc_Core_intcmp( int,
int); extern int Cyc_Core_ptrcmp( void**, void**); extern unsigned char Cyc_Core_InvalidArg[
15u]; struct Cyc_Core_InvalidArg_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); extern int system( unsigned char*); extern int*
__errno(); struct Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[
19u]; extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; struct Cyc_List_List{ void* hd;
struct Cyc_List_List* tl; } ; extern unsigned char Cyc_List_List_empty[ 15u];
extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List* y); extern
struct Cyc_List_List* Cyc_List_rappend( struct _RegionHandle*, struct Cyc_List_List*
x, struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u]; extern int
Cyc_List_memq( struct Cyc_List_List* l, void* x); extern int Cyc_String_zstrptrcmp(
struct _tagged_arr*, struct _tagged_arr*); struct Cyc_Dict_Dict; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* comp)( void*, void*)); extern
struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key, void*
data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key); extern
int Cyc_Dict_lookup_bool( struct Cyc_Dict_Dict* d, void* key, void** ans_place);
extern void* Cyc_Dict_fold2_c( void*(* f)( void*, void*, void*, void*, void*),
void* inner_env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2, void* accum);
struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Lineno_Pos{
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
15u]; static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_arr); extern void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*, struct _tagged_arr); extern int Cyc_Tcutil_bits_only(
void* t); struct Cyc_CfFlowInfo_Place; static const int Cyc_CfFlowInfo_VarRoot=
0; struct Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
} ; static const int Cyc_CfFlowInfo_MallocPt= 1; struct Cyc_CfFlowInfo_MallocPt_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_CfFlowInfo_StructF=
0; struct Cyc_CfFlowInfo_StructF_struct{ int tag; struct _tagged_arr* f1; } ;
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
int tag; struct Cyc_Dict_Dict* f1; } ; extern void* Cyc_CfFlowInfo_mkLeafPI(
void* esc, void* il); extern void* Cyc_CfFlowInfo_lookup_place( struct Cyc_Dict_Dict*
d, struct Cyc_CfFlowInfo_Place* place); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_insert_place(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place, void* pinfo);
extern int Cyc_CfFlowInfo_isAllInit( struct Cyc_Dict_Dict* pinfo_dict, void*
pinfo); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_pointsto( void* pinfo,
struct Cyc_Dict_Dict* d); extern void* Cyc_CfFlowInfo_assign_unknown_dict( void*,
void*); extern void* Cyc_CfFlowInfo_join_flow( void*, void*); extern int Cyc_CfFlowInfo_flow_lessthan_approx(
void* f1, void* f2); struct Cyc_NewControlFlow_AnalEnv{ struct Cyc_Dict_Dict*
roots; int in_try; void* tryflow; } ; extern void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv* env, void* new_flow); extern void* Cyc_NewControlFlow_cf_analyze_stmt(
struct Cyc_NewControlFlow_AnalEnv*, struct Cyc_Absyn_Stmt*, void*); extern
unsigned char Cyc_CfAbsexp_BadAbsexp[ 14u]; static const int Cyc_CfAbsexp_OrderedG=
0; static const int Cyc_CfAbsexp_UnorderedG= 1; static const int Cyc_CfAbsexp_OneofG=
2; extern void Cyc_CfAbsexp_check_absexp( void*); extern struct _tagged_arr Cyc_CfAbsexp_absexp2string(
void* ae, int depth); extern void* Cyc_CfAbsexp_mkAnyOrderG(); extern void* Cyc_CfAbsexp_mkUnknownOp();
extern void* Cyc_CfAbsexp_mkAddressOp( void* ao); extern void* Cyc_CfAbsexp_mkLocalOp(
struct Cyc_Absyn_Vardecl* vd); extern void* Cyc_CfAbsexp_mkMemberOp( void* ao,
void* f); extern void* Cyc_CfAbsexp_mkMallocOp( struct Cyc_Absyn_Exp* e); extern
void* Cyc_CfAbsexp_mkDerefOp( void* ao); extern void* Cyc_CfAbsexp_mkBottomAE();
extern void* Cyc_CfAbsexp_mkSkipAE(); extern void* Cyc_CfAbsexp_mkUseAE( void*
ao); extern void* Cyc_CfAbsexp_mkAssignAE( void* l, void* r); extern void* Cyc_CfAbsexp_mkMallocAE(
struct Cyc_Absyn_Exp* e); extern void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt*
s); extern void* Cyc_CfAbsexp_mkGroupAE( void* g, void* ae1, void* ae2); extern
void* Cyc_CfAbsexp_mkGroupAE_l( void* g, struct Cyc_List_List* ael); extern int
Cyc_CfAbsexp_isUnknownOp( void*); extern void* Cyc_CfAbsexp_eval_absexp( struct
Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Position_Segment* loc, void* ae,
void* in_flow); unsigned char Cyc_CfAbsexp_BadAbsexp[ 14u]="\000\000\000\000BadAbsexp";
void* Cyc_CfAbsexp_mkAnyOrderG(){ return( void*) Cyc_CfAbsexp_OrderedG;} static
const int Cyc_CfAbsexp_BottomAE= 0; static const int Cyc_CfAbsexp_SkipAE= 1;
static const int Cyc_CfAbsexp_UseAE= 0; struct Cyc_CfAbsexp_UseAE_struct{ int
tag; void* f1; } ; static const int Cyc_CfAbsexp_AssignAE= 1; struct Cyc_CfAbsexp_AssignAE_struct{
int tag; void* f1; void* f2; } ; static const int Cyc_CfAbsexp_MallocAE= 2;
struct Cyc_CfAbsexp_MallocAE_struct{ int tag; void* f1; int f2; } ; static const
int Cyc_CfAbsexp_StmtAE= 3; struct Cyc_CfAbsexp_StmtAE_struct{ int tag; struct
Cyc_Absyn_Stmt* f1; } ; static const int Cyc_CfAbsexp_GroupAE= 4; struct Cyc_CfAbsexp_GroupAE_struct{
int tag; void* f1; struct Cyc_List_List* f2; } ; static const int Cyc_CfAbsexp_UnknownOp=
0; static const int Cyc_CfAbsexp_AddressOp= 0; struct Cyc_CfAbsexp_AddressOp_struct{
int tag; void* f1; } ; static const int Cyc_CfAbsexp_DerefOp= 1; struct Cyc_CfAbsexp_DerefOp_struct{
int tag; void* f1; } ; static const int Cyc_CfAbsexp_MemberOp= 2; struct Cyc_CfAbsexp_MemberOp_struct{
int tag; void* f1; void* f2; } ; static const int Cyc_CfAbsexp_LocalOp= 3;
struct Cyc_CfAbsexp_LocalOp_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_CfAbsexp_MallocOp= 4; struct Cyc_CfAbsexp_MallocOp_struct{
int tag; struct Cyc_Absyn_Exp* f1; int f2; } ; static void Cyc_CfAbsexp_ok_address_arg(
void* ao){ void* _temp0= ao; _LL2: if(( unsigned int) _temp0 > 1u?*(( int*)
_temp0) == Cyc_CfAbsexp_LocalOp: 0){ goto _LL3;} else{ goto _LL4;} _LL4: if((
unsigned int) _temp0 > 1u?*(( int*) _temp0) == Cyc_CfAbsexp_MemberOp: 0){ goto
_LL5;} else{ goto _LL6;} _LL6: if(( unsigned int) _temp0 > 1u?*(( int*) _temp0)
== Cyc_CfAbsexp_MallocOp: 0){ goto _LL7;} else{ goto _LL8;} _LL8: goto _LL9;
_LL3: return; _LL5: return; _LL7: return; _LL9:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp);
_LL1:;} static void Cyc_CfAbsexp_ok_member_arg( void* ao){ void* _temp10= ao;
_LL12: if(( unsigned int) _temp10 > 1u?*(( int*) _temp10) == Cyc_CfAbsexp_LocalOp:
0){ goto _LL13;} else{ goto _LL14;} _LL14: if(( unsigned int) _temp10 > 1u?*((
int*) _temp10) == Cyc_CfAbsexp_MemberOp: 0){ goto _LL15;} else{ goto _LL16;}
_LL16: if(( unsigned int) _temp10 > 1u?*(( int*) _temp10) == Cyc_CfAbsexp_MallocOp:
0){ goto _LL17;} else{ goto _LL18;} _LL18: if(( unsigned int) _temp10 > 1u?*((
int*) _temp10) == Cyc_CfAbsexp_DerefOp: 0){ goto _LL19;} else{ goto _LL20;}
_LL20: goto _LL21; _LL13: return; _LL15: return; _LL17: return; _LL19: return;
_LL21:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL11:;} static void Cyc_CfAbsexp_ok_deref_arg(
void* ao){ void* _temp22= ao; _LL24: if(( unsigned int) _temp22 > 1u?*(( int*)
_temp22) == Cyc_CfAbsexp_LocalOp: 0){ goto _LL25;} else{ goto _LL26;} _LL26: if((
unsigned int) _temp22 > 1u?*(( int*) _temp22) == Cyc_CfAbsexp_MemberOp: 0){ goto
_LL27;} else{ goto _LL28;} _LL28: if(( unsigned int) _temp22 > 1u?*(( int*)
_temp22) == Cyc_CfAbsexp_DerefOp: 0){ goto _LL29;} else{ goto _LL30;} _LL30:
goto _LL31; _LL25: return; _LL27: return; _LL29: return; _LL31:( int) _throw((
void*) Cyc_CfAbsexp_BadAbsexp); _LL23:;} static void Cyc_CfAbsexp_ok_lvalue(
void* ao){ void* _temp32= ao; _LL34: if( _temp32 ==( void*) Cyc_CfAbsexp_UnknownOp){
goto _LL35;} else{ goto _LL36;} _LL36: if(( unsigned int) _temp32 > 1u?*(( int*)
_temp32) == Cyc_CfAbsexp_AddressOp: 0){ goto _LL37;} else{ goto _LL38;} _LL38:
goto _LL39; _LL35: goto _LL37; _LL37:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp);
_LL39: return; _LL33:;} static int Cyc_CfAbsexp_num_mallocpts= 0; static struct
Cyc_Dict_Dict** Cyc_CfAbsexp_mallocpt_dict= 0; int Cyc_CfAbsexp_mallocpt_int(
struct Cyc_Absyn_Exp* e){ if( Cyc_CfAbsexp_mallocpt_dict == 0){ struct Cyc_Dict_Dict*
d=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*)))
Cyc_Dict_empty)(( int(*)( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*)) Cyc_Core_ptrcmp);
Cyc_CfAbsexp_mallocpt_dict=({ struct Cyc_Dict_Dict** _temp40=( struct Cyc_Dict_Dict**)
GC_malloc( sizeof( struct Cyc_Dict_Dict*) * 1); _temp40[ 0]= d; _temp40;});}{
int i= 0; if( !(( int(*)( struct Cyc_Dict_Dict* d, struct Cyc_Absyn_Exp* key,
int* ans_place)) Cyc_Dict_lookup_bool)(*(( struct Cyc_Dict_Dict**) _check_null(
Cyc_CfAbsexp_mallocpt_dict)), e,& i)){*(( struct Cyc_Dict_Dict**) _check_null(
Cyc_CfAbsexp_mallocpt_dict))=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_Absyn_Exp* key, int data)) Cyc_Dict_insert)(*(( struct Cyc_Dict_Dict**)
_check_null( Cyc_CfAbsexp_mallocpt_dict)), e,( i= ++ Cyc_CfAbsexp_num_mallocpts));}
return i;}} void* Cyc_CfAbsexp_mkUnknownOp(){ return( void*) Cyc_CfAbsexp_UnknownOp;}
void* Cyc_CfAbsexp_mkAddressOp( void* ao){ Cyc_CfAbsexp_ok_address_arg( ao);
return( void*)({ struct Cyc_CfAbsexp_AddressOp_struct* _temp41=( struct Cyc_CfAbsexp_AddressOp_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_AddressOp_struct)); _temp41[ 0]=({ struct
Cyc_CfAbsexp_AddressOp_struct _temp42; _temp42.tag= Cyc_CfAbsexp_AddressOp;
_temp42.f1=( void*) ao; _temp42;}); _temp41;});} void* Cyc_CfAbsexp_mkLocalOp(
struct Cyc_Absyn_Vardecl* vd){ return( void*)({ struct Cyc_CfAbsexp_LocalOp_struct*
_temp43=( struct Cyc_CfAbsexp_LocalOp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_LocalOp_struct));
_temp43[ 0]=({ struct Cyc_CfAbsexp_LocalOp_struct _temp44; _temp44.tag= Cyc_CfAbsexp_LocalOp;
_temp44.f1= vd; _temp44;}); _temp43;});} void* Cyc_CfAbsexp_mkMemberOp( void* ao,
void* f){ Cyc_CfAbsexp_ok_member_arg( ao); return( void*)({ struct Cyc_CfAbsexp_MemberOp_struct*
_temp45=( struct Cyc_CfAbsexp_MemberOp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_MemberOp_struct));
_temp45[ 0]=({ struct Cyc_CfAbsexp_MemberOp_struct _temp46; _temp46.tag= Cyc_CfAbsexp_MemberOp;
_temp46.f1=( void*) ao; _temp46.f2=( void*) f; _temp46;}); _temp45;});} void*
Cyc_CfAbsexp_mkMallocOp( struct Cyc_Absyn_Exp* e){ return( void*)({ struct Cyc_CfAbsexp_MallocOp_struct*
_temp47=( struct Cyc_CfAbsexp_MallocOp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_MallocOp_struct));
_temp47[ 0]=({ struct Cyc_CfAbsexp_MallocOp_struct _temp48; _temp48.tag= Cyc_CfAbsexp_MallocOp;
_temp48.f1= e; _temp48.f2= Cyc_CfAbsexp_mallocpt_int( e); _temp48;}); _temp47;});}
void* Cyc_CfAbsexp_mkDerefOp( void* ao){ Cyc_CfAbsexp_ok_deref_arg( ao); return(
void*)({ struct Cyc_CfAbsexp_DerefOp_struct* _temp49=( struct Cyc_CfAbsexp_DerefOp_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_DerefOp_struct)); _temp49[ 0]=({ struct
Cyc_CfAbsexp_DerefOp_struct _temp50; _temp50.tag= Cyc_CfAbsexp_DerefOp; _temp50.f1=(
void*) ao; _temp50;}); _temp49;});} void* Cyc_CfAbsexp_mkBottomAE(){ return(
void*) Cyc_CfAbsexp_BottomAE;} void* Cyc_CfAbsexp_mkSkipAE(){ return( void*) Cyc_CfAbsexp_SkipAE;}
void* Cyc_CfAbsexp_mkMallocAE( struct Cyc_Absyn_Exp* e){ return( void*)({ struct
Cyc_CfAbsexp_MallocAE_struct* _temp51=( struct Cyc_CfAbsexp_MallocAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_MallocAE_struct)); _temp51[ 0]=({ struct
Cyc_CfAbsexp_MallocAE_struct _temp52; _temp52.tag= Cyc_CfAbsexp_MallocAE;
_temp52.f1=( void*)(( void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp53=(
struct Cyc_CfFlowInfo_MallocPt_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp53[ 0]=({ struct Cyc_CfFlowInfo_MallocPt_struct _temp54; _temp54.tag= Cyc_CfFlowInfo_MallocPt;
_temp54.f1= e; _temp54;}); _temp53;})); _temp52.f2= Cyc_CfAbsexp_mallocpt_int( e);
_temp52;}); _temp51;});} void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt* s){
return( void*)({ struct Cyc_CfAbsexp_StmtAE_struct* _temp55=( struct Cyc_CfAbsexp_StmtAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_StmtAE_struct)); _temp55[ 0]=({ struct
Cyc_CfAbsexp_StmtAE_struct _temp56; _temp56.tag= Cyc_CfAbsexp_StmtAE; _temp56.f1=
s; _temp56;}); _temp55;});} void* Cyc_CfAbsexp_mkUseAE( void* ao){ void* _temp57=
ao; _LL59: if( _temp57 ==( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL60;} else{
goto _LL61;} _LL61: goto _LL62; _LL60: return( void*) Cyc_CfAbsexp_SkipAE; _LL62:
return( void*)({ struct Cyc_CfAbsexp_UseAE_struct* _temp63=( struct Cyc_CfAbsexp_UseAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_UseAE_struct)); _temp63[ 0]=({ struct Cyc_CfAbsexp_UseAE_struct
_temp64; _temp64.tag= Cyc_CfAbsexp_UseAE; _temp64.f1=( void*) ao; _temp64;});
_temp63;}); _LL58:;} struct _tuple3{ void* f1; void* f2; } ; void* Cyc_CfAbsexp_mkAssignAE(
void* l, void* r){ struct _tuple3 _temp66=({ struct _tuple3 _temp65; _temp65.f1=
l; _temp65.f2= r; _temp65;}); void* _temp74; void* _temp76; void* _temp78; _LL68:
_LL77: _temp76= _temp66.f1; if( _temp76 ==( void*) Cyc_CfAbsexp_UnknownOp){ goto
_LL75;} else{ goto _LL70;} _LL75: _temp74= _temp66.f2; if( _temp74 ==( void*)
Cyc_CfAbsexp_UnknownOp){ goto _LL69;} else{ goto _LL70;} _LL70: _LL79: _temp78=
_temp66.f1; if( _temp78 ==( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL71;} else{
goto _LL72;} _LL72: goto _LL73; _LL69: return( void*) Cyc_CfAbsexp_SkipAE; _LL71:
return Cyc_CfAbsexp_mkUseAE( r); _LL73: Cyc_CfAbsexp_ok_lvalue( l); return( void*)({
struct Cyc_CfAbsexp_AssignAE_struct* _temp80=( struct Cyc_CfAbsexp_AssignAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_AssignAE_struct)); _temp80[ 0]=({ struct
Cyc_CfAbsexp_AssignAE_struct _temp81; _temp81.tag= Cyc_CfAbsexp_AssignAE;
_temp81.f1=( void*) l; _temp81.f2=( void*) r; _temp81;}); _temp80;}); _LL67:;}
struct _tuple4{ void* f1; void* f2; void* f3; } ; void* Cyc_CfAbsexp_mkGroupAE(
void* g, void* ae1, void* ae2){ struct _tuple4 _temp83=({ struct _tuple4 _temp82;
_temp82.f1= g; _temp82.f2= ae1; _temp82.f3= ae2; _temp82;}); void* _temp106;
void* _temp109; void* _temp111; void* _temp113; void* _temp115; void* _temp117;
void* _temp119; void* _temp121; void* _temp124; struct Cyc_List_List* _temp126;
void* _temp128; void* _temp130; struct Cyc_List_List* _temp132; void* _temp134;
void* _temp136; void* _temp139; struct Cyc_List_List* _temp141; void* _temp143;
void* _temp145; void* _temp147; struct Cyc_List_List* _temp149; void* _temp151;
void* _temp153; void* _temp156; struct Cyc_List_List* _temp158; void* _temp160;
void* _temp162; _LL85: _LL107: _temp106= _temp83.f3; if( _temp106 ==( void*) Cyc_CfAbsexp_SkipAE){
goto _LL105;} else{ goto _LL87;} _LL105: if( g !=( void*) Cyc_CfAbsexp_OneofG){
goto _LL86;} else{ goto _LL87;} _LL87: _LL110: _temp109= _temp83.f2; if(
_temp109 ==( void*) Cyc_CfAbsexp_SkipAE){ goto _LL108;} else{ goto _LL89;}
_LL108: if( g !=( void*) Cyc_CfAbsexp_OneofG){ goto _LL88;} else{ goto _LL89;}
_LL89: _LL114: _temp113= _temp83.f1; if( _temp113 ==( void*) Cyc_CfAbsexp_OneofG){
goto _LL112;} else{ goto _LL91;} _LL112: _temp111= _temp83.f3; if( _temp111 ==(
void*) Cyc_CfAbsexp_BottomAE){ goto _LL90;} else{ goto _LL91;} _LL91: _LL118:
_temp117= _temp83.f1; if( _temp117 ==( void*) Cyc_CfAbsexp_OneofG){ goto _LL116;}
else{ goto _LL93;} _LL116: _temp115= _temp83.f2; if( _temp115 ==( void*) Cyc_CfAbsexp_BottomAE){
goto _LL92;} else{ goto _LL93;} _LL93: _LL122: _temp121= _temp83.f1; if(
_temp121 ==( void*) Cyc_CfAbsexp_OrderedG){ goto _LL120;} else{ goto _LL95;}
_LL120: _temp119= _temp83.f2; if( _temp119 ==( void*) Cyc_CfAbsexp_BottomAE){
goto _LL94;} else{ goto _LL95;} _LL95: _LL137: _temp136= _temp83.f1; goto _LL131;
_LL131: _temp130= _temp83.f2; if(( unsigned int) _temp130 > 2u?*(( int*)
_temp130) == Cyc_CfAbsexp_GroupAE: 0){ _LL135: _temp134=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp130)->f1; goto _LL133; _LL133: _temp132=(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp130)->f2; goto _LL125;} else{ goto _LL97;} _LL125: _temp124= _temp83.f3;
if(( unsigned int) _temp124 > 2u?*(( int*) _temp124) == Cyc_CfAbsexp_GroupAE: 0){
_LL129: _temp128=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp124)->f1;
goto _LL127; _LL127: _temp126=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp124)->f2;
goto _LL123;} else{ goto _LL97;} _LL123: if( _temp136 == _temp134? _temp136 ==
_temp128: 0){ goto _LL96;} else{ goto _LL97;} _LL97: _LL146: _temp145= _temp83.f1;
goto _LL140; _LL140: _temp139= _temp83.f2; if(( unsigned int) _temp139 > 2u?*((
int*) _temp139) == Cyc_CfAbsexp_GroupAE: 0){ _LL144: _temp143=( void*)(( struct
Cyc_CfAbsexp_GroupAE_struct*) _temp139)->f1; goto _LL142; _LL142: _temp141=((
struct Cyc_CfAbsexp_GroupAE_struct*) _temp139)->f2; goto _LL138;} else{ goto
_LL99;} _LL138: if( _temp145 == _temp143? _temp145 !=( void*) Cyc_CfAbsexp_OrderedG:
0){ goto _LL98;} else{ goto _LL99;} _LL99: _LL154: _temp153= _temp83.f1; if(
_temp153 ==( void*) Cyc_CfAbsexp_OrderedG){ goto _LL148;} else{ goto _LL101;}
_LL148: _temp147= _temp83.f2; if(( unsigned int) _temp147 > 2u?*(( int*)
_temp147) == Cyc_CfAbsexp_GroupAE: 0){ _LL152: _temp151=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp147)->f1; if( _temp151 ==( void*) Cyc_CfAbsexp_OrderedG){ goto _LL150;}
else{ goto _LL101;} _LL150: _temp149=(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp147)->f2; goto _LL100;} else{ goto _LL101;} _LL101: _LL163: _temp162=
_temp83.f1; goto _LL157; _LL157: _temp156= _temp83.f3; if(( unsigned int)
_temp156 > 2u?*(( int*) _temp156) == Cyc_CfAbsexp_GroupAE: 0){ _LL161: _temp160=(
void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp156)->f1; goto _LL159; _LL159:
_temp158=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp156)->f2; goto _LL155;}
else{ goto _LL103;} _LL155: if( _temp162 == _temp160){ goto _LL102;} else{ goto
_LL103;} _LL103: goto _LL104; _LL86: return ae1; _LL88: return ae2; _LL90:
return ae1; _LL92: return ae2; _LL94: return( void*) Cyc_CfAbsexp_BottomAE;
_LL96: return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct* _temp164=( struct
Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp164[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp165; _temp165.tag= Cyc_CfAbsexp_GroupAE;
_temp165.f1=( void*) _temp136; _temp165.f2= Cyc_List_append( _temp132, _temp126);
_temp165;}); _temp164;}); _LL98: return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp166=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp166[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp167; _temp167.tag= Cyc_CfAbsexp_GroupAE;
_temp167.f1=( void*) _temp145; _temp167.f2=({ struct Cyc_List_List* _temp168=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp168[
0]=({ struct Cyc_List_List _temp169; _temp169.hd=( void*) ae2; _temp169.tl=
_temp141; _temp169;}); _temp168;}); _temp167;}); _temp166;}); _LL100: return(
void*)({ struct Cyc_CfAbsexp_GroupAE_struct* _temp170=( struct Cyc_CfAbsexp_GroupAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct)); _temp170[ 0]=({ struct
Cyc_CfAbsexp_GroupAE_struct _temp171; _temp171.tag= Cyc_CfAbsexp_GroupAE;
_temp171.f1=( void*)(( void*) Cyc_CfAbsexp_OrderedG); _temp171.f2= Cyc_List_append(
_temp149,({ struct Cyc_List_List* _temp172=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List) * 1); _temp172[ 0]=({ struct Cyc_List_List
_temp173; _temp173.hd=( void*) ae2; _temp173.tl= 0; _temp173;}); _temp172;}));
_temp171;}); _temp170;}); _LL102: return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp174=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp174[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp175; _temp175.tag= Cyc_CfAbsexp_GroupAE;
_temp175.f1=( void*) _temp162; _temp175.f2=({ struct Cyc_List_List* _temp176=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp176[
0]=({ struct Cyc_List_List _temp177; _temp177.hd=( void*) ae1; _temp177.tl=
_temp158; _temp177;}); _temp176;}); _temp175;}); _temp174;}); _LL104: return(
void*)({ struct Cyc_CfAbsexp_GroupAE_struct* _temp178=( struct Cyc_CfAbsexp_GroupAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct)); _temp178[ 0]=({ struct
Cyc_CfAbsexp_GroupAE_struct _temp179; _temp179.tag= Cyc_CfAbsexp_GroupAE;
_temp179.f1=( void*) g; _temp179.f2=({ struct Cyc_List_List* _temp180=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp180[ 0]=({
struct Cyc_List_List _temp181; _temp181.hd=( void*) ae1; _temp181.tl=({ struct
Cyc_List_List* _temp182=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)
* 1); _temp182[ 0]=({ struct Cyc_List_List _temp183; _temp183.hd=( void*) ae2;
_temp183.tl= 0; _temp183;}); _temp182;}); _temp181;}); _temp180;}); _temp179;});
_temp178;}); _LL84:;} void* Cyc_CfAbsexp_mkGroupAE_l( void* g, struct Cyc_List_List*
ael){ if( ael == 0){ return( void*) Cyc_CfAbsexp_SkipAE;} if((( struct Cyc_List_List*)
_check_null( ael))->tl == 0){ return( void*)(( struct Cyc_List_List*)
_check_null( ael))->hd;} return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp184=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp184[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp185; _temp185.tag= Cyc_CfAbsexp_GroupAE;
_temp185.f1=( void*) g; _temp185.f2= ael; _temp185;}); _temp184;});} int Cyc_CfAbsexp_isUnknownOp(
void* ao){ return ao ==( void*) Cyc_CfAbsexp_UnknownOp;} struct _tagged_arr Cyc_CfAbsexp_absop2string(
void* ao){ void* _temp186= ao; void* _temp200; struct Cyc_Absyn_Vardecl*
_temp202; void* _temp204; void* _temp206; int _temp208; struct Cyc_Absyn_Exp*
_temp210; void* _temp212; _LL188: if( _temp186 ==( void*) Cyc_CfAbsexp_UnknownOp){
goto _LL189;} else{ goto _LL190;} _LL190: if(( unsigned int) _temp186 > 1u?*((
int*) _temp186) == Cyc_CfAbsexp_AddressOp: 0){ _LL201: _temp200=( void*)((
struct Cyc_CfAbsexp_AddressOp_struct*) _temp186)->f1; goto _LL191;} else{ goto
_LL192;} _LL192: if(( unsigned int) _temp186 > 1u?*(( int*) _temp186) == Cyc_CfAbsexp_LocalOp:
0){ _LL203: _temp202=(( struct Cyc_CfAbsexp_LocalOp_struct*) _temp186)->f1; goto
_LL193;} else{ goto _LL194;} _LL194: if(( unsigned int) _temp186 > 1u?*(( int*)
_temp186) == Cyc_CfAbsexp_MemberOp: 0){ _LL207: _temp206=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp186)->f1; goto _LL205; _LL205: _temp204=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp186)->f2; goto _LL195;} else{ goto _LL196;} _LL196: if(( unsigned int)
_temp186 > 1u?*(( int*) _temp186) == Cyc_CfAbsexp_MallocOp: 0){ _LL211: _temp210=((
struct Cyc_CfAbsexp_MallocOp_struct*) _temp186)->f1; goto _LL209; _LL209:
_temp208=(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp186)->f2; goto _LL197;}
else{ goto _LL198;} _LL198: if(( unsigned int) _temp186 > 1u?*(( int*) _temp186)
== Cyc_CfAbsexp_DerefOp: 0){ _LL213: _temp212=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*)
_temp186)->f1; goto _LL199;} else{ goto _LL187;} _LL189: return _tag_arr("Unknown",
sizeof( unsigned char), 8u); _LL191: return( struct _tagged_arr)({ struct
_tagged_arr _temp214= Cyc_CfAbsexp_absop2string( _temp200); xprintf("& %.*s",
_get_arr_size( _temp214, 1u), _temp214.curr);}); _LL193: return*(* _temp202->name).f2;
_LL195: { struct _tagged_arr fs;{ void* _temp215= _temp204; struct _tagged_arr*
_temp221; int _temp223; _LL217: if(*(( int*) _temp215) == Cyc_CfFlowInfo_StructF){
_LL222: _temp221=(( struct Cyc_CfFlowInfo_StructF_struct*) _temp215)->f1; goto
_LL218;} else{ goto _LL219;} _LL219: if(*(( int*) _temp215) == Cyc_CfFlowInfo_TupleF){
_LL224: _temp223=(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp215)->f1; goto
_LL220;} else{ goto _LL216;} _LL218: fs=* _temp221; goto _LL216; _LL220: fs=(
struct _tagged_arr) xprintf("%d", _temp223); goto _LL216; _LL216:;} return(
struct _tagged_arr)({ struct _tagged_arr _temp225= Cyc_CfAbsexp_absop2string(
_temp206); struct _tagged_arr _temp226= fs; xprintf("%.*s.%.*s", _get_arr_size(
_temp225, 1u), _temp225.curr, _get_arr_size( _temp226, 1u), _temp226.curr);});}
_LL197: return( struct _tagged_arr) xprintf("<mpt%d>", _temp208); _LL199: return(
struct _tagged_arr)({ struct _tagged_arr _temp227= Cyc_CfAbsexp_absop2string(
_temp212); xprintf("(*%.*s)", _get_arr_size( _temp227, 1u), _temp227.curr);});
_LL187:;} struct _tagged_arr Cyc_CfAbsexp_absexp2string( void* ae, int depth){
struct _tagged_arr ans= Cyc_Core_new_string( depth);{ int i= 0; for( 0; i <
_get_arr_size( ans, sizeof( unsigned char)); ++ i){*(( unsigned char*)
_check_unknown_subscript( ans, sizeof( unsigned char), i))=' ';}}{ void*
_temp228= ae; void* _temp246; int _temp248; void* _temp250; struct Cyc_Absyn_Exp*
_temp252; void* _temp254; void* _temp256; struct Cyc_List_List* _temp258; void*
_temp260; struct Cyc_Absyn_Stmt* _temp262; _LL230: if( _temp228 ==( void*) Cyc_CfAbsexp_SkipAE){
goto _LL231;} else{ goto _LL232;} _LL232: if(( unsigned int) _temp228 > 2u?*((
int*) _temp228) == Cyc_CfAbsexp_UseAE: 0){ _LL247: _temp246=( void*)(( struct
Cyc_CfAbsexp_UseAE_struct*) _temp228)->f1; goto _LL233;} else{ goto _LL234;}
_LL234: if(( unsigned int) _temp228 > 2u?*(( int*) _temp228) == Cyc_CfAbsexp_MallocAE:
0){ _LL251: _temp250=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp228)->f1;
if(*(( int*) _temp250) == Cyc_CfFlowInfo_MallocPt){ _LL253: _temp252=(( struct
Cyc_CfFlowInfo_MallocPt_struct*) _temp250)->f1; goto _LL249;} else{ goto _LL236;}
_LL249: _temp248=(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp228)->f2; goto
_LL235;} else{ goto _LL236;} _LL236: if(( unsigned int) _temp228 > 2u?*(( int*)
_temp228) == Cyc_CfAbsexp_AssignAE: 0){ _LL257: _temp256=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp228)->f1; goto _LL255; _LL255: _temp254=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp228)->f2; goto _LL237;} else{ goto _LL238;} _LL238: if(( unsigned int)
_temp228 > 2u?*(( int*) _temp228) == Cyc_CfAbsexp_GroupAE: 0){ _LL261: _temp260=(
void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp228)->f1; goto _LL259; _LL259:
_temp258=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp228)->f2; goto _LL239;}
else{ goto _LL240;} _LL240: if(( unsigned int) _temp228 > 2u?*(( int*) _temp228)
== Cyc_CfAbsexp_StmtAE: 0){ _LL263: _temp262=(( struct Cyc_CfAbsexp_StmtAE_struct*)
_temp228)->f1; goto _LL241;} else{ goto _LL242;} _LL242: if( _temp228 ==( void*)
Cyc_CfAbsexp_BottomAE){ goto _LL243;} else{ goto _LL244;} _LL244: goto _LL245;
_LL231: ans=({ struct _tagged_arr _temp264=( struct _tagged_arr) ans; xprintf("%.*sskip",
_get_arr_size( _temp264, 1u), _temp264.curr);}); goto _LL229; _LL233: ans=({
struct _tagged_arr _temp265=( struct _tagged_arr) ans; struct _tagged_arr
_temp266= Cyc_CfAbsexp_absop2string( _temp246); xprintf("%.*suse %.*s",
_get_arr_size( _temp265, 1u), _temp265.curr, _get_arr_size( _temp266, 1u),
_temp266.curr);}); goto _LL229; _LL235: ans=({ struct _tagged_arr _temp267=(
struct _tagged_arr) ans; int _temp268= _temp248; xprintf("%.*smalloc %i",
_get_arr_size( _temp267, 1u), _temp267.curr, _temp268);}); goto _LL229; _LL237:
ans=({ struct _tagged_arr _temp269=( struct _tagged_arr) ans; struct _tagged_arr
_temp270= Cyc_CfAbsexp_absop2string( _temp256); struct _tagged_arr _temp271= Cyc_CfAbsexp_absop2string(
_temp254); xprintf("%.*sassign %.*s %.*s", _get_arr_size( _temp269, 1u),
_temp269.curr, _get_arr_size( _temp270, 1u), _temp270.curr, _get_arr_size(
_temp271, 1u), _temp271.curr);}); goto _LL229; _LL239:{ void* _temp272= _temp260;
_LL274: if( _temp272 ==( void*) Cyc_CfAbsexp_OrderedG){ goto _LL275;} else{ goto
_LL276;} _LL276: if( _temp272 ==( void*) Cyc_CfAbsexp_UnorderedG){ goto _LL277;}
else{ goto _LL278;} _LL278: if( _temp272 ==( void*) Cyc_CfAbsexp_OneofG){ goto
_LL279;} else{ goto _LL273;} _LL275: ans=({ struct _tagged_arr _temp280=( struct
_tagged_arr) ans; xprintf("%.*sordered", _get_arr_size( _temp280, 1u), _temp280.curr);});
goto _LL273; _LL277: ans=({ struct _tagged_arr _temp281=( struct _tagged_arr)
ans; xprintf("%.*sunordered", _get_arr_size( _temp281, 1u), _temp281.curr);});
goto _LL273; _LL279: ans=({ struct _tagged_arr _temp282=( struct _tagged_arr)
ans; xprintf("%.*soneof", _get_arr_size( _temp282, 1u), _temp282.curr);}); goto
_LL273; _LL273:;} for( 0; _temp258 != 0; _temp258=(( struct Cyc_List_List*)
_check_null( _temp258))->tl){ ans=({ struct _tagged_arr _temp283=( struct
_tagged_arr) ans; struct _tagged_arr _temp284= Cyc_CfAbsexp_absexp2string(( void*)((
struct Cyc_List_List*) _check_null( _temp258))->hd, depth + 1); xprintf("%.*s\n%.*s",
_get_arr_size( _temp283, 1u), _temp283.curr, _get_arr_size( _temp284, 1u),
_temp284.curr);});} goto _LL229; _LL241: ans=({ struct _tagged_arr _temp285=(
struct _tagged_arr) ans; xprintf("%.*s<<stmt>>", _get_arr_size( _temp285, 1u),
_temp285.curr);}); goto _LL229; _LL243: ans=({ struct _tagged_arr _temp286=(
struct _tagged_arr) ans; xprintf("%.*sbottom", _get_arr_size( _temp286, 1u),
_temp286.curr);}); goto _LL229; _LL245:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp);
_LL229:;} return( struct _tagged_arr) ans;} static void Cyc_CfAbsexp_check_absop(
void* ao){ void* _temp287= ao; void* _temp301; void* _temp303; void* _temp305;
_LL289: if( _temp287 ==( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL290;} else{
goto _LL291;} _LL291: if(( unsigned int) _temp287 > 1u?*(( int*) _temp287) ==
Cyc_CfAbsexp_AddressOp: 0){ _LL302: _temp301=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*)
_temp287)->f1; goto _LL292;} else{ goto _LL293;} _LL293: if(( unsigned int)
_temp287 > 1u?*(( int*) _temp287) == Cyc_CfAbsexp_DerefOp: 0){ _LL304: _temp303=(
void*)(( struct Cyc_CfAbsexp_DerefOp_struct*) _temp287)->f1; goto _LL294;} else{
goto _LL295;} _LL295: if(( unsigned int) _temp287 > 1u?*(( int*) _temp287) ==
Cyc_CfAbsexp_MemberOp: 0){ _LL306: _temp305=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp287)->f1; goto _LL296;} else{ goto _LL297;} _LL297: if(( unsigned int)
_temp287 > 1u?*(( int*) _temp287) == Cyc_CfAbsexp_LocalOp: 0){ goto _LL298;}
else{ goto _LL299;} _LL299: if(( unsigned int) _temp287 > 1u?*(( int*) _temp287)
== Cyc_CfAbsexp_MallocOp: 0){ goto _LL300;} else{ goto _LL288;} _LL290: return;
_LL292: Cyc_CfAbsexp_check_absop( _temp301); Cyc_CfAbsexp_ok_address_arg(
_temp301); return; _LL294: Cyc_CfAbsexp_check_absop( _temp303); Cyc_CfAbsexp_ok_deref_arg(
_temp303); return; _LL296: Cyc_CfAbsexp_check_absop( _temp305); Cyc_CfAbsexp_ok_member_arg(
_temp305); return; _LL298: return; _LL300: return; _LL288:;} void Cyc_CfAbsexp_check_absexp(
void* ae){ void* _temp307= ae; void* _temp325; struct Cyc_Absyn_Stmt* _temp327;
struct Cyc_List_List* _temp329; void* _temp331; void* _temp333; void* _temp335;
_LL309: if( _temp307 ==( void*) Cyc_CfAbsexp_BottomAE){ goto _LL310;} else{ goto
_LL311;} _LL311: if( _temp307 ==( void*) Cyc_CfAbsexp_SkipAE){ goto _LL312;}
else{ goto _LL313;} _LL313: if(( unsigned int) _temp307 > 2u?*(( int*) _temp307)
== Cyc_CfAbsexp_MallocAE: 0){ _LL326: _temp325=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp307)->f1; if(*(( int*) _temp325) == Cyc_CfFlowInfo_VarRoot){ goto _LL314;}
else{ goto _LL315;}} else{ goto _LL315;} _LL315: if(( unsigned int) _temp307 > 2u?*((
int*) _temp307) == Cyc_CfAbsexp_MallocAE: 0){ goto _LL316;} else{ goto _LL317;}
_LL317: if(( unsigned int) _temp307 > 2u?*(( int*) _temp307) == Cyc_CfAbsexp_StmtAE:
0){ _LL328: _temp327=(( struct Cyc_CfAbsexp_StmtAE_struct*) _temp307)->f1; goto
_LL318;} else{ goto _LL319;} _LL319: if(( unsigned int) _temp307 > 2u?*(( int*)
_temp307) == Cyc_CfAbsexp_GroupAE: 0){ _LL330: _temp329=(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp307)->f2; goto _LL320;} else{ goto _LL321;} _LL321: if(( unsigned int)
_temp307 > 2u?*(( int*) _temp307) == Cyc_CfAbsexp_UseAE: 0){ _LL332: _temp331=(
void*)(( struct Cyc_CfAbsexp_UseAE_struct*) _temp307)->f1; goto _LL322;} else{
goto _LL323;} _LL323: if(( unsigned int) _temp307 > 2u?*(( int*) _temp307) ==
Cyc_CfAbsexp_AssignAE: 0){ _LL336: _temp335=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp307)->f1; goto _LL334; _LL334: _temp333=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp307)->f2; goto _LL324;} else{ goto _LL308;} _LL310: return; _LL312: return;
_LL314:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL316: return; _LL318:
return; _LL320: for( 0; _temp329 != 0; _temp329=(( struct Cyc_List_List*)
_check_null( _temp329))->tl){ Cyc_CfAbsexp_check_absexp(( void*)(( struct Cyc_List_List*)
_check_null( _temp329))->hd);} return; _LL322: Cyc_CfAbsexp_check_absop(
_temp331); return; _LL324: Cyc_CfAbsexp_check_absop( _temp335); Cyc_CfAbsexp_check_absop(
_temp333); Cyc_CfAbsexp_ok_lvalue( _temp335); return; _LL308:;} void* Cyc_CfAbsexp_eval_absop_r(
struct Cyc_Dict_Dict* pinfo_dict, void* ao); void* Cyc_CfAbsexp_eval_absop_l(
struct Cyc_Dict_Dict* pinfo_dict, void* ao){ void* _temp337= ao; struct Cyc_Absyn_Vardecl*
_temp351; struct Cyc_Absyn_Exp* _temp353; void* _temp355; void* _temp357; void*
_temp359; _LL339: if(( unsigned int) _temp337 > 1u?*(( int*) _temp337) == Cyc_CfAbsexp_LocalOp:
0){ _LL352: _temp351=(( struct Cyc_CfAbsexp_LocalOp_struct*) _temp337)->f1; goto
_LL340;} else{ goto _LL341;} _LL341: if(( unsigned int) _temp337 > 1u?*(( int*)
_temp337) == Cyc_CfAbsexp_MallocOp: 0){ _LL354: _temp353=(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp337)->f1; goto _LL342;} else{ goto _LL343;} _LL343: if(( unsigned int)
_temp337 > 1u?*(( int*) _temp337) == Cyc_CfAbsexp_MemberOp: 0){ _LL358: _temp357=(
void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp337)->f1; goto _LL356;
_LL356: _temp355=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp337)->f2;
goto _LL344;} else{ goto _LL345;} _LL345: if(( unsigned int) _temp337 > 1u?*((
int*) _temp337) == Cyc_CfAbsexp_DerefOp: 0){ _LL360: _temp359=( void*)(( struct
Cyc_CfAbsexp_DerefOp_struct*) _temp337)->f1; goto _LL346;} else{ goto _LL347;}
_LL347: if( _temp337 ==( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL348;} else{
goto _LL349;} _LL349: if(( unsigned int) _temp337 > 1u?*(( int*) _temp337) ==
Cyc_CfAbsexp_AddressOp: 0){ goto _LL350;} else{ goto _LL338;} _LL340: return(
void*)({ struct Cyc_CfFlowInfo_MustPointTo_struct* _temp361=( struct Cyc_CfFlowInfo_MustPointTo_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MustPointTo_struct)); _temp361[ 0]=({
struct Cyc_CfFlowInfo_MustPointTo_struct _temp362; _temp362.tag= Cyc_CfFlowInfo_MustPointTo;
_temp362.f1=({ struct Cyc_CfFlowInfo_Place* _temp363=( struct Cyc_CfFlowInfo_Place*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_Place)); _temp363->root=( void*)(( void*)({
struct Cyc_CfFlowInfo_VarRoot_struct* _temp364=( struct Cyc_CfFlowInfo_VarRoot_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp364[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp365; _temp365.tag= Cyc_CfFlowInfo_VarRoot;
_temp365.f1= _temp351; _temp365;}); _temp364;})); _temp363->fields= 0; _temp363;});
_temp362;}); _temp361;}); _LL342: return( void*)({ struct Cyc_CfFlowInfo_MustPointTo_struct*
_temp366=( struct Cyc_CfFlowInfo_MustPointTo_struct*) GC_malloc( sizeof( struct
Cyc_CfFlowInfo_MustPointTo_struct)); _temp366[ 0]=({ struct Cyc_CfFlowInfo_MustPointTo_struct
_temp367; _temp367.tag= Cyc_CfFlowInfo_MustPointTo; _temp367.f1=({ struct Cyc_CfFlowInfo_Place*
_temp368=( struct Cyc_CfFlowInfo_Place*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_Place));
_temp368->root=( void*)(( void*)({ struct Cyc_CfFlowInfo_MallocPt_struct*
_temp369=( struct Cyc_CfFlowInfo_MallocPt_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp369[ 0]=({ struct Cyc_CfFlowInfo_MallocPt_struct _temp370; _temp370.tag=
Cyc_CfFlowInfo_MallocPt; _temp370.f1= _temp353; _temp370;}); _temp369;}));
_temp368->fields= 0; _temp368;}); _temp367;}); _temp366;}); _LL344: { void*
_temp371= Cyc_CfAbsexp_eval_absop_l( pinfo_dict, _temp357); void* _temp372=
_temp371; struct Cyc_CfFlowInfo_Place* _temp378; struct Cyc_CfFlowInfo_Place
_temp380; struct Cyc_List_List* _temp381; void* _temp383; _LL374: if(*(( int*)
_temp372) == Cyc_CfFlowInfo_UnknownIS){ goto _LL375;} else{ goto _LL376;} _LL376:
if(*(( int*) _temp372) == Cyc_CfFlowInfo_MustPointTo){ _LL379: _temp378=((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp372)->f1; _temp380=* _temp378;
_LL384: _temp383=( void*) _temp380.root; goto _LL382; _LL382: _temp381= _temp380.fields;
goto _LL377;} else{ goto _LL373;} _LL375: return _temp371; _LL377: return( void*)({
struct Cyc_CfFlowInfo_MustPointTo_struct* _temp385=( struct Cyc_CfFlowInfo_MustPointTo_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MustPointTo_struct)); _temp385[ 0]=({
struct Cyc_CfFlowInfo_MustPointTo_struct _temp386; _temp386.tag= Cyc_CfFlowInfo_MustPointTo;
_temp386.f1=({ struct Cyc_CfFlowInfo_Place* _temp387=( struct Cyc_CfFlowInfo_Place*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_Place)); _temp387->root=( void*)
_temp383; _temp387->fields= Cyc_List_append( _temp381,({ struct Cyc_List_List*
_temp388=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp388->hd=( void*) _temp355; _temp388->tl= 0; _temp388;})); _temp387;});
_temp386;}); _temp385;}); _LL373:;} _LL346: { void* _temp389= Cyc_CfAbsexp_eval_absop_r(
pinfo_dict, _temp359); void* _temp390= _temp389; void* _temp398; _LL392: if(*((
int*) _temp390) == Cyc_CfFlowInfo_LeafPI){ _LL399: _temp398=( void*)(( struct
Cyc_CfFlowInfo_LeafPI_struct*) _temp390)->f1; goto _LL393;} else{ goto _LL394;}
_LL394: if(*(( int*) _temp390) == Cyc_CfFlowInfo_TuplePI){ goto _LL395;} else{
goto _LL396;} _LL396: if(*(( int*) _temp390) == Cyc_CfFlowInfo_StructPI){ goto
_LL397;} else{ goto _LL391;} _LL393: return _temp398; _LL395: goto _LL397;
_LL397:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL391:;} _LL348: goto
_LL350; _LL350:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL338:;} void*
Cyc_CfAbsexp_eval_absop_r( struct Cyc_Dict_Dict* pinfo_dict, void* ao){ void*
_temp400= ao; struct Cyc_Absyn_Vardecl* _temp414; struct Cyc_Absyn_Exp* _temp416;
void* _temp418; void* _temp420; void* _temp422; void* _temp424; _LL402: if((
unsigned int) _temp400 > 1u?*(( int*) _temp400) == Cyc_CfAbsexp_LocalOp: 0){
_LL415: _temp414=(( struct Cyc_CfAbsexp_LocalOp_struct*) _temp400)->f1; goto
_LL403;} else{ goto _LL404;} _LL404: if(( unsigned int) _temp400 > 1u?*(( int*)
_temp400) == Cyc_CfAbsexp_MallocOp: 0){ _LL417: _temp416=(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp400)->f1; goto _LL405;} else{ goto _LL406;} _LL406: if(( unsigned int)
_temp400 > 1u?*(( int*) _temp400) == Cyc_CfAbsexp_MemberOp: 0){ _LL421: _temp420=(
void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp400)->f1; goto _LL419;
_LL419: _temp418=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp400)->f2;
goto _LL407;} else{ goto _LL408;} _LL408: if(( unsigned int) _temp400 > 1u?*((
int*) _temp400) == Cyc_CfAbsexp_DerefOp: 0){ _LL423: _temp422=( void*)(( struct
Cyc_CfAbsexp_DerefOp_struct*) _temp400)->f1; goto _LL409;} else{ goto _LL410;}
_LL410: if( _temp400 ==( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL411;} else{
goto _LL412;} _LL412: if(( unsigned int) _temp400 > 1u?*(( int*) _temp400) ==
Cyc_CfAbsexp_AddressOp: 0){ _LL425: _temp424=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*)
_temp400)->f1; goto _LL413;} else{ goto _LL401;} _LL403: return Cyc_Dict_lookup(
pinfo_dict,( void*)({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp426=( struct
Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp426[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp427; _temp427.tag= Cyc_CfFlowInfo_VarRoot;
_temp427.f1= _temp414; _temp427;}); _temp426;})); _LL405: return Cyc_Dict_lookup(
pinfo_dict,( void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp428=( struct
Cyc_CfFlowInfo_MallocPt_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp428[ 0]=({ struct Cyc_CfFlowInfo_MallocPt_struct _temp429; _temp429.tag=
Cyc_CfFlowInfo_MallocPt; _temp429.f1= _temp416; _temp429;}); _temp428;}));
_LL407: { void* _temp430= Cyc_CfAbsexp_eval_absop_r( pinfo_dict, _temp420);
struct _tuple3 _temp432=({ struct _tuple3 _temp431; _temp431.f1= _temp430;
_temp431.f2= _temp418; _temp431;}); void* _temp442; void* _temp444; void*
_temp446; int _temp448; void* _temp450; struct Cyc_Dict_Dict* _temp452; void*
_temp454; struct _tagged_arr* _temp456; void* _temp458; struct Cyc_Dict_Dict*
_temp460; _LL434: _LL443: _temp442= _temp432.f1; if(*(( int*) _temp442) == Cyc_CfFlowInfo_LeafPI){
_LL445: _temp444=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp442)->f1;
if(*(( int*) _temp444) == Cyc_CfFlowInfo_UnknownIS){ goto _LL435;} else{ goto
_LL436;}} else{ goto _LL436;} _LL436: _LL451: _temp450= _temp432.f1; if(*(( int*)
_temp450) == Cyc_CfFlowInfo_TuplePI){ _LL453: _temp452=(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp450)->f1; goto _LL447;} else{ goto _LL438;} _LL447: _temp446= _temp432.f2;
if(*(( int*) _temp446) == Cyc_CfFlowInfo_TupleF){ _LL449: _temp448=(( struct Cyc_CfFlowInfo_TupleF_struct*)
_temp446)->f1; goto _LL437;} else{ goto _LL438;} _LL438: _LL459: _temp458=
_temp432.f1; if(*(( int*) _temp458) == Cyc_CfFlowInfo_StructPI){ _LL461:
_temp460=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp458)->f1; goto _LL455;}
else{ goto _LL440;} _LL455: _temp454= _temp432.f2; if(*(( int*) _temp454) == Cyc_CfFlowInfo_StructF){
_LL457: _temp456=(( struct Cyc_CfFlowInfo_StructF_struct*) _temp454)->f1; goto
_LL439;} else{ goto _LL440;} _LL440: goto _LL441; _LL435: return _temp430;
_LL437: return(( void*(*)( struct Cyc_Dict_Dict* d, int key)) Cyc_Dict_lookup)(
_temp452, _temp448); _LL439: return(( void*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup)( _temp460, _temp456); _LL441:( int) _throw((
void*) Cyc_CfAbsexp_BadAbsexp); _LL433:;} _LL409: { void* _temp462= Cyc_CfAbsexp_eval_absop_r(
pinfo_dict, _temp422); void* _temp463= _temp462; void* _temp473; void* _temp475;
void* _temp477; void* _temp479; void* _temp481; struct Cyc_CfFlowInfo_Place*
_temp483; _LL465: if(*(( int*) _temp463) == Cyc_CfFlowInfo_LeafPI){ _LL474:
_temp473=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp463)->f1; if(*((
int*) _temp473) == Cyc_CfFlowInfo_UnknownIS){ _LL478: _temp477=( void*)(( struct
Cyc_CfFlowInfo_UnknownIS_struct*) _temp473)->f1; goto _LL476; _LL476: _temp475=(
void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp473)->f2; if( _temp475 ==(
void*) Cyc_CfFlowInfo_ThisIL){ goto _LL466;} else{ goto _LL467;}} else{ goto
_LL467;}} else{ goto _LL467;} _LL467: if(*(( int*) _temp463) == Cyc_CfFlowInfo_LeafPI){
_LL480: _temp479=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp463)->f1;
if(*(( int*) _temp479) == Cyc_CfFlowInfo_UnknownIS){ goto _LL468;} else{ goto
_LL469;}} else{ goto _LL469;} _LL469: if(*(( int*) _temp463) == Cyc_CfFlowInfo_LeafPI){
_LL482: _temp481=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp463)->f1;
if(*(( int*) _temp481) == Cyc_CfFlowInfo_MustPointTo){ _LL484: _temp483=((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp481)->f1; goto _LL470;} else{
goto _LL471;}} else{ goto _LL471;} _LL471: goto _LL472; _LL466: return Cyc_CfFlowInfo_mkLeafPI((
void*) Cyc_CfFlowInfo_Esc,( void*) Cyc_CfFlowInfo_NoneIL); _LL468: return
_temp462; _LL470: return Cyc_CfFlowInfo_lookup_place( pinfo_dict, _temp483);
_LL472:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL464:;} _LL411: return
Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,( void*) Cyc_CfFlowInfo_AllIL);
_LL413: return( void*)({ struct Cyc_CfFlowInfo_LeafPI_struct* _temp485=( struct
Cyc_CfFlowInfo_LeafPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_LeafPI_struct));
_temp485[ 0]=({ struct Cyc_CfFlowInfo_LeafPI_struct _temp486; _temp486.tag= Cyc_CfFlowInfo_LeafPI;
_temp486.f1=( void*) Cyc_CfAbsexp_eval_absop_l( pinfo_dict, _temp424); _temp486;});
_temp485;}); _LL401:;} void* Cyc_CfAbsexp_assign_escape( struct Cyc_List_List**
escaping_states, void* old_pinfo, void* new_pinfo); struct Cyc_Dict_Dict* Cyc_CfAbsexp_assign_escape_f(
struct Cyc_List_List** escaping_states, void* key, void* b1, void* b2, struct
Cyc_Dict_Dict* accum){ return Cyc_Dict_insert( accum, key, Cyc_CfAbsexp_assign_escape(
escaping_states, b1, b2));} void* Cyc_CfAbsexp_assign_escape( struct Cyc_List_List**
escaping_states, void* old_pinfo, void* new_pinfo){ struct _tuple3 _temp488=({
struct _tuple3 _temp487; _temp487.f1= old_pinfo; _temp487.f2= new_pinfo;
_temp487;}); void* _temp506; void* _temp508; void* _temp510; void* _temp512;
void* _temp514; void* _temp516; void* _temp518; void* _temp520; void* _temp522;
void* _temp524; void* _temp526; void* _temp528; void* _temp530; void* _temp532;
void* _temp534; void* _temp536; void* _temp538; void* _temp540; void* _temp542;
void* _temp544; void* _temp546; void* _temp548; void* _temp550; struct Cyc_Dict_Dict*
_temp552; void* _temp554; struct Cyc_Dict_Dict* _temp556; void* _temp558; struct
Cyc_Dict_Dict* _temp560; void* _temp562; struct Cyc_Dict_Dict* _temp564; _LL490:
_LL513: _temp512= _temp488.f1; if(*(( int*) _temp512) == Cyc_CfFlowInfo_LeafPI){
_LL515: _temp514=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp512)->f1;
if(*(( int*) _temp514) == Cyc_CfFlowInfo_MustPointTo){ goto _LL507;} else{ goto
_LL492;}} else{ goto _LL492;} _LL507: _temp506= _temp488.f2; if(*(( int*)
_temp506) == Cyc_CfFlowInfo_LeafPI){ _LL509: _temp508=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp506)->f1; if(*(( int*) _temp508) == Cyc_CfFlowInfo_UnknownIS){ _LL511:
_temp510=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp508)->f2; goto
_LL491;} else{ goto _LL492;}} else{ goto _LL492;} _LL492: _LL523: _temp522=
_temp488.f1; if(*(( int*) _temp522) == Cyc_CfFlowInfo_LeafPI){ _LL525: _temp524=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp522)->f1; if(*(( int*)
_temp524) == Cyc_CfFlowInfo_UnknownIS){ _LL527: _temp526=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp524)->f1; if( _temp526 ==( void*) Cyc_CfFlowInfo_Unesc){ goto _LL517;}
else{ goto _LL494;}} else{ goto _LL494;}} else{ goto _LL494;} _LL517: _temp516=
_temp488.f2; if(*(( int*) _temp516) == Cyc_CfFlowInfo_LeafPI){ _LL519: _temp518=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp516)->f1; if(*(( int*)
_temp518) == Cyc_CfFlowInfo_UnknownIS){ _LL521: _temp520=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp518)->f2; goto _LL493;} else{ goto _LL494;}} else{ goto _LL494;} _LL494:
_LL531: _temp530= _temp488.f1; if(*(( int*) _temp530) == Cyc_CfFlowInfo_LeafPI){
_LL533: _temp532=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp530)->f1;
if(*(( int*) _temp532) == Cyc_CfFlowInfo_MustPointTo){ goto _LL529;} else{ goto
_LL496;}} else{ goto _LL496;} _LL529: _temp528= _temp488.f2; if(*(( int*)
_temp528) == Cyc_CfFlowInfo_LeafPI){ goto _LL495;} else{ goto _LL496;} _LL496:
_LL537: _temp536= _temp488.f1; if(*(( int*) _temp536) == Cyc_CfFlowInfo_LeafPI){
_LL539: _temp538=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp536)->f1;
if(*(( int*) _temp538) == Cyc_CfFlowInfo_UnknownIS){ _LL541: _temp540=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp538)->f1; if( _temp540 ==( void*)
Cyc_CfFlowInfo_Unesc){ goto _LL535;} else{ goto _LL498;}} else{ goto _LL498;}}
else{ goto _LL498;} _LL535: _temp534= _temp488.f2; if(*(( int*) _temp534) == Cyc_CfFlowInfo_LeafPI){
goto _LL497;} else{ goto _LL498;} _LL498: _LL545: _temp544= _temp488.f1; if(*((
int*) _temp544) == Cyc_CfFlowInfo_LeafPI){ _LL547: _temp546=( void*)(( struct
Cyc_CfFlowInfo_LeafPI_struct*) _temp544)->f1; if(*(( int*) _temp546) == Cyc_CfFlowInfo_UnknownIS){
_LL549: _temp548=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp546)->f1;
if( _temp548 ==( void*) Cyc_CfFlowInfo_Esc){ goto _LL543;} else{ goto _LL500;}}
else{ goto _LL500;}} else{ goto _LL500;} _LL543: _temp542= _temp488.f2; if(*((
int*) _temp542) == Cyc_CfFlowInfo_LeafPI){ goto _LL499;} else{ goto _LL500;}
_LL500: _LL555: _temp554= _temp488.f1; if(*(( int*) _temp554) == Cyc_CfFlowInfo_TuplePI){
_LL557: _temp556=(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp554)->f1; goto
_LL551;} else{ goto _LL502;} _LL551: _temp550= _temp488.f2; if(*(( int*)
_temp550) == Cyc_CfFlowInfo_TuplePI){ _LL553: _temp552=(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp550)->f1; goto _LL501;} else{ goto _LL502;} _LL502: _LL563: _temp562=
_temp488.f1; if(*(( int*) _temp562) == Cyc_CfFlowInfo_StructPI){ _LL565:
_temp564=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp562)->f1; goto _LL559;}
else{ goto _LL504;} _LL559: _temp558= _temp488.f2; if(*(( int*) _temp558) == Cyc_CfFlowInfo_StructPI){
_LL561: _temp560=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp558)->f1; goto
_LL503;} else{ goto _LL504;} _LL504: goto _LL505; _LL491: _temp520= _temp510;
goto _LL493; _LL493: return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Unesc,
_temp520); _LL495: goto _LL497; _LL497: return new_pinfo; _LL499:*
escaping_states=({ struct Cyc_List_List* _temp566=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp566->hd=( void*) new_pinfo;
_temp566->tl=* escaping_states; _temp566;}); return Cyc_CfFlowInfo_mkLeafPI((
void*) Cyc_CfFlowInfo_Esc,( void*) Cyc_CfFlowInfo_AllIL); _LL501: return( void*)({
struct Cyc_CfFlowInfo_TuplePI_struct* _temp567=( struct Cyc_CfFlowInfo_TuplePI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct)); _temp567[ 0]=({
struct Cyc_CfFlowInfo_TuplePI_struct _temp568; _temp568.tag= Cyc_CfFlowInfo_TuplePI;
_temp568.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*(* f)( struct Cyc_List_List**,
int, void*, void*, struct Cyc_Dict_Dict*), struct Cyc_List_List** inner_env,
struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2, struct Cyc_Dict_Dict* accum))
Cyc_Dict_fold2_c)(( struct Cyc_Dict_Dict*(*)( struct Cyc_List_List**
escaping_states, int key, void* b1, void* b2, struct Cyc_Dict_Dict* accum)) Cyc_CfAbsexp_assign_escape_f,
escaping_states, _temp556, _temp552,(( struct Cyc_Dict_Dict*(*)( int(* comp)(
int, int))) Cyc_Dict_empty)( Cyc_Core_intcmp)); _temp568;}); _temp567;}); _LL503:
return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct* _temp569=( struct Cyc_CfFlowInfo_StructPI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct)); _temp569[ 0]=({
struct Cyc_CfFlowInfo_StructPI_struct _temp570; _temp570.tag= Cyc_CfFlowInfo_StructPI;
_temp570.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*(* f)( struct Cyc_List_List**,
struct _tagged_arr*, void*, void*, struct Cyc_Dict_Dict*), struct Cyc_List_List**
inner_env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2, struct Cyc_Dict_Dict*
accum)) Cyc_Dict_fold2_c)(( struct Cyc_Dict_Dict*(*)( struct Cyc_List_List**
escaping_states, struct _tagged_arr* key, void* b1, void* b2, struct Cyc_Dict_Dict*
accum)) Cyc_CfAbsexp_assign_escape_f, escaping_states, _temp564, _temp560,((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_String_zstrptrcmp)); _temp570;}); _temp569;}); _LL505:( int)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp571=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp571[ 0]=({ struct
Cyc_Core_Impossible_struct _temp572; _temp572.tag= Cyc_Core_Impossible; _temp572.f1=
_tag_arr("bad pinfos in assign_escape", sizeof( unsigned char), 28u); _temp572;});
_temp571;})); _LL489:;} int Cyc_CfAbsexp_is_ok_malloc_assign( void* lval, struct
Cyc_List_List* ok_mallocs){ void* _temp573= lval; struct Cyc_Absyn_Exp* _temp581;
void* _temp583; _LL575: if(( unsigned int) _temp573 > 1u?*(( int*) _temp573) ==
Cyc_CfAbsexp_MallocOp: 0){ _LL582: _temp581=(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp573)->f1; goto _LL576;} else{ goto _LL577;} _LL577: if(( unsigned int)
_temp573 > 1u?*(( int*) _temp573) == Cyc_CfAbsexp_MemberOp: 0){ _LL584: _temp583=(
void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp573)->f1; goto _LL578;}
else{ goto _LL579;} _LL579: goto _LL580; _LL576: return(( int(*)( struct Cyc_List_List*
l, struct Cyc_Absyn_Exp* x)) Cyc_List_memq)( ok_mallocs, _temp581); _LL578:
return Cyc_CfAbsexp_is_ok_malloc_assign( _temp583, ok_mallocs); _LL580: return 0;
_LL574:;} struct _tuple5{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; }
; static struct _tuple5 Cyc_CfAbsexp_unordered_mallocs( struct _RegionHandle*
rgn, void* ae, struct Cyc_List_List* ok_mallocs){ struct Cyc_List_List* _temp585=
0; struct Cyc_List_List* _temp586= 0;{ void* _temp587= ae; void* _temp609; void*
_temp611; void* _temp613; struct Cyc_Absyn_Exp* _temp615; struct Cyc_Absyn_Stmt*
_temp617; struct Cyc_List_List* _temp619; void* _temp621; struct Cyc_List_List*
_temp623; void* _temp625; struct Cyc_List_List* _temp627; void* _temp629; _LL589:
if( _temp587 ==( void*) Cyc_CfAbsexp_BottomAE){ goto _LL590;} else{ goto _LL591;}
_LL591: if( _temp587 ==( void*) Cyc_CfAbsexp_SkipAE){ goto _LL592;} else{ goto
_LL593;} _LL593: if(( unsigned int) _temp587 > 2u?*(( int*) _temp587) == Cyc_CfAbsexp_UseAE:
0){ goto _LL594;} else{ goto _LL595;} _LL595: if(( unsigned int) _temp587 > 2u?*((
int*) _temp587) == Cyc_CfAbsexp_AssignAE: 0){ _LL612: _temp611=( void*)(( struct
Cyc_CfAbsexp_AssignAE_struct*) _temp587)->f1; goto _LL610; _LL610: _temp609=(
void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp587)->f2; goto _LL596;}
else{ goto _LL597;} _LL597: if(( unsigned int) _temp587 > 2u?*(( int*) _temp587)
== Cyc_CfAbsexp_MallocAE: 0){ _LL614: _temp613=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp587)->f1; if(*(( int*) _temp613) == Cyc_CfFlowInfo_MallocPt){ _LL616:
_temp615=(( struct Cyc_CfFlowInfo_MallocPt_struct*) _temp613)->f1; goto _LL598;}
else{ goto _LL599;}} else{ goto _LL599;} _LL599: if(( unsigned int) _temp587 > 2u?*((
int*) _temp587) == Cyc_CfAbsexp_MallocAE: 0){ goto _LL600;} else{ goto _LL601;}
_LL601: if(( unsigned int) _temp587 > 2u?*(( int*) _temp587) == Cyc_CfAbsexp_StmtAE:
0){ _LL618: _temp617=(( struct Cyc_CfAbsexp_StmtAE_struct*) _temp587)->f1; goto
_LL602;} else{ goto _LL603;} _LL603: if(( unsigned int) _temp587 > 2u?*(( int*)
_temp587) == Cyc_CfAbsexp_GroupAE: 0){ _LL622: _temp621=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp587)->f1; if( _temp621 ==( void*) Cyc_CfAbsexp_OrderedG){ goto _LL620;}
else{ goto _LL605;} _LL620: _temp619=(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp587)->f2; goto _LL604;} else{ goto _LL605;} _LL605: if(( unsigned int)
_temp587 > 2u?*(( int*) _temp587) == Cyc_CfAbsexp_GroupAE: 0){ _LL626: _temp625=(
void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp587)->f1; if( _temp625 ==(
void*) Cyc_CfAbsexp_OneofG){ goto _LL624;} else{ goto _LL607;} _LL624: _temp623=((
struct Cyc_CfAbsexp_GroupAE_struct*) _temp587)->f2; goto _LL606;} else{ goto
_LL607;} _LL607: if(( unsigned int) _temp587 > 2u?*(( int*) _temp587) == Cyc_CfAbsexp_GroupAE:
0){ _LL630: _temp629=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp587)->f1;
if( _temp629 ==( void*) Cyc_CfAbsexp_UnorderedG){ goto _LL628;} else{ goto
_LL588;} _LL628: _temp627=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp587)->f2;
goto _LL608;} else{ goto _LL588;} _LL590: goto _LL588; _LL592: goto _LL588;
_LL594: goto _LL588; _LL596: if( Cyc_CfAbsexp_is_ok_malloc_assign( _temp611,
ok_mallocs)){ _temp585=({ struct Cyc_List_List* _temp631=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp631->hd=( void*) ae;
_temp631->tl= _temp585; _temp631;});} goto _LL588; _LL598: _temp586=({ struct
Cyc_List_List* _temp632=( struct Cyc_List_List*) _region_malloc( rgn, sizeof(
struct Cyc_List_List)); _temp632->hd=( void*) _temp615; _temp632->tl= _temp586;
_temp632;}); goto _LL588; _LL600:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp633=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp633[ 0]=({ struct Cyc_Core_Impossible_struct _temp634; _temp634.tag= Cyc_Core_Impossible;
_temp634.f1= _tag_arr("local variable in MallocAE", sizeof( unsigned char), 27u);
_temp634;}); _temp633;})); _LL602: goto _LL588; _LL604: for( 0; _temp619 != 0;
_temp619=(( struct Cyc_List_List*) _check_null( _temp619))->tl){ struct Cyc_List_List*
_temp637; struct Cyc_List_List* _temp639; struct _tuple5 _temp635= Cyc_CfAbsexp_unordered_mallocs(
rgn,( void*)(( struct Cyc_List_List*) _check_null( _temp619))->hd,(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rappend)(
rgn, _temp586, ok_mallocs)); _LL640: _temp639= _temp635.f1; goto _LL638; _LL638:
_temp637= _temp635.f2; goto _LL636; _LL636: _temp585= Cyc_List_rappend( rgn,
_temp639, _temp585); _temp586=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rappend)( rgn,
_temp637, _temp586);} goto _LL588; _LL606: goto _LL588; _LL608: for( 0; _temp627
!= 0; _temp627=(( struct Cyc_List_List*) _check_null( _temp627))->tl){ struct
Cyc_List_List* _temp643; struct Cyc_List_List* _temp645; struct _tuple5 _temp641=
Cyc_CfAbsexp_unordered_mallocs( rgn,( void*)(( struct Cyc_List_List*)
_check_null( _temp627))->hd, ok_mallocs); _LL646: _temp645= _temp641.f1; goto
_LL644; _LL644: _temp643= _temp641.f2; goto _LL642; _LL642: _temp585= Cyc_List_rappend(
rgn, _temp645, _temp585); _temp586=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rappend)(
rgn, _temp643, _temp586);} goto _LL588; _LL588:;} return({ struct _tuple5
_temp647; _temp647.f1= _temp585; _temp647.f2= _temp586; _temp647;});} void* Cyc_CfAbsexp_eval_absexp(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Position_Segment* loc, void*
ae, void* in_flow){ struct Cyc_Dict_Dict* pinfo_dict;{ void* _temp648= in_flow;
struct Cyc_Dict_Dict* _temp654; _LL650: if( _temp648 ==( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL651;} else{ goto _LL652;} _LL652: if(( unsigned int) _temp648 > 1u?*((
int*) _temp648) == Cyc_CfFlowInfo_InitsFL: 0){ _LL655: _temp654=(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp648)->f1; goto _LL653;} else{ goto _LL649;} _LL651: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL653: pinfo_dict= _temp654; goto _LL649; _LL649:;}{ void* _temp656= ae; void*
_temp676; void* _temp678; void* _temp680; void* _temp682; struct Cyc_Absyn_Stmt*
_temp684; struct Cyc_List_List* _temp686; void* _temp688; struct Cyc_List_List*
_temp690; void* _temp692; struct Cyc_List_List* _temp694; void* _temp696; _LL658:
if(( unsigned int) _temp656 > 2u?*(( int*) _temp656) == Cyc_CfAbsexp_AssignAE: 0){
_LL679: _temp678=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp656)->f1;
goto _LL677; _LL677: _temp676=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp656)->f2; goto _LL659;} else{ goto _LL660;} _LL660: if(( unsigned int)
_temp656 > 2u?*(( int*) _temp656) == Cyc_CfAbsexp_UseAE: 0){ _LL681: _temp680=(
void*)(( struct Cyc_CfAbsexp_UseAE_struct*) _temp656)->f1; goto _LL661;} else{
goto _LL662;} _LL662: if(( unsigned int) _temp656 > 2u?*(( int*) _temp656) ==
Cyc_CfAbsexp_MallocAE: 0){ _LL683: _temp682=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp656)->f1; goto _LL663;} else{ goto _LL664;} _LL664: if( _temp656 ==( void*)
Cyc_CfAbsexp_BottomAE){ goto _LL665;} else{ goto _LL666;} _LL666: if( _temp656
==( void*) Cyc_CfAbsexp_SkipAE){ goto _LL667;} else{ goto _LL668;} _LL668: if((
unsigned int) _temp656 > 2u?*(( int*) _temp656) == Cyc_CfAbsexp_StmtAE: 0){
_LL685: _temp684=(( struct Cyc_CfAbsexp_StmtAE_struct*) _temp656)->f1; goto
_LL669;} else{ goto _LL670;} _LL670: if(( unsigned int) _temp656 > 2u?*(( int*)
_temp656) == Cyc_CfAbsexp_GroupAE: 0){ _LL689: _temp688=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp656)->f1; if( _temp688 ==( void*) Cyc_CfAbsexp_OrderedG){ goto _LL687;}
else{ goto _LL672;} _LL687: _temp686=(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp656)->f2; goto _LL671;} else{ goto _LL672;} _LL672: if(( unsigned int)
_temp656 > 2u?*(( int*) _temp656) == Cyc_CfAbsexp_GroupAE: 0){ _LL693: _temp692=(
void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp656)->f1; if( _temp692 ==(
void*) Cyc_CfAbsexp_OneofG){ goto _LL691;} else{ goto _LL674;} _LL691: _temp690=((
struct Cyc_CfAbsexp_GroupAE_struct*) _temp656)->f2; goto _LL673;} else{ goto
_LL674;} _LL674: if(( unsigned int) _temp656 > 2u?*(( int*) _temp656) == Cyc_CfAbsexp_GroupAE:
0){ _LL697: _temp696=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp656)->f1;
if( _temp696 ==( void*) Cyc_CfAbsexp_UnorderedG){ goto _LL695;} else{ goto
_LL657;} _LL695: _temp694=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp656)->f2;
goto _LL675;} else{ goto _LL657;} _LL659:{ void* _temp698= Cyc_CfAbsexp_eval_absop_l(
pinfo_dict, _temp678); struct Cyc_CfFlowInfo_Place* _temp704; _LL700: if(*(( int*)
_temp698) == Cyc_CfFlowInfo_UnknownIS){ goto _LL701;} else{ goto _LL702;} _LL702:
if(*(( int*) _temp698) == Cyc_CfFlowInfo_MustPointTo){ _LL705: _temp704=((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp698)->f1; goto _LL703;} else{
goto _LL699;} _LL701: goto _LL699; _LL703: { void* _temp706= Cyc_CfFlowInfo_lookup_place(
pinfo_dict, _temp704); void* _temp707= Cyc_CfAbsexp_eval_absop_r( pinfo_dict,
_temp676);{ struct _tuple3 _temp709=({ struct _tuple3 _temp708; _temp708.f1=
_temp706; _temp708.f2= _temp707; _temp708;}); void* _temp717; void* _temp719;
void* _temp721; void* _temp723; void* _temp725; void* _temp727; void* _temp729;
void* _temp731; _LL711: _LL724: _temp723= _temp709.f1; if(*(( int*) _temp723) ==
Cyc_CfFlowInfo_TuplePI){ goto _LL718;} else{ goto _LL713;} _LL718: _temp717=
_temp709.f2; if(*(( int*) _temp717) == Cyc_CfFlowInfo_LeafPI){ _LL720: _temp719=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp717)->f1; if(*(( int*)
_temp719) == Cyc_CfFlowInfo_UnknownIS){ _LL722: _temp721=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp719)->f2; goto _LL712;} else{ goto _LL713;}} else{ goto _LL713;} _LL713:
_LL732: _temp731= _temp709.f1; if(*(( int*) _temp731) == Cyc_CfFlowInfo_StructPI){
goto _LL726;} else{ goto _LL715;} _LL726: _temp725= _temp709.f2; if(*(( int*)
_temp725) == Cyc_CfFlowInfo_LeafPI){ _LL728: _temp727=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp725)->f1; if(*(( int*) _temp727) == Cyc_CfFlowInfo_UnknownIS){ _LL730:
_temp729=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp727)->f2; goto
_LL714;} else{ goto _LL715;}} else{ goto _LL715;} _LL715: goto _LL716; _LL712:
_temp729= _temp721; goto _LL714; _LL714: _temp707= Cyc_CfFlowInfo_assign_unknown_dict(
_temp729, _temp706); goto _LL710; _LL716: goto _LL710; _LL710:;}{ struct Cyc_List_List*
_temp733= 0; void* _temp734= Cyc_CfAbsexp_assign_escape(& _temp733, _temp706,
_temp707); for( 0; _temp733 != 0; _temp733=(( struct Cyc_List_List*) _check_null(
_temp733))->tl){ void* _temp735=( void*)(( struct Cyc_List_List*) _check_null(
_temp733))->hd; if( ! Cyc_CfFlowInfo_isAllInit( pinfo_dict, _temp735)){ Cyc_Tcutil_terr(
loc, _tag_arr("uninitialized value assigned to escaped place", sizeof(
unsigned char), 46u)); return( void*) Cyc_CfFlowInfo_BottomFL;} pinfo_dict= Cyc_CfFlowInfo_escape_pointsto(
_temp735, pinfo_dict);} pinfo_dict= Cyc_CfFlowInfo_insert_place( pinfo_dict,
_temp704, _temp734);{ struct Cyc_CfFlowInfo_InitsFL_struct* _temp736=({ struct
Cyc_CfFlowInfo_InitsFL_struct* _temp737=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp737[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp738; _temp738.tag= Cyc_CfFlowInfo_InitsFL;
_temp738.f1= pinfo_dict; _temp738;}); _temp737;}); Cyc_NewControlFlow_update_tryflow(
env,( void*) _temp736); return( void*) _temp736;}}} _LL699:;} _temp680= _temp676;
goto _LL661; _LL661: { void* _temp739= Cyc_CfAbsexp_eval_absop_r( pinfo_dict,
_temp680); if( ! Cyc_CfFlowInfo_isAllInit( pinfo_dict, _temp739)){ void*
_temp740= _temp680; struct Cyc_Absyn_Vardecl* _temp746; _LL742: if((
unsigned int) _temp740 > 1u?*(( int*) _temp740) == Cyc_CfAbsexp_LocalOp: 0){
_LL747: _temp746=(( struct Cyc_CfAbsexp_LocalOp_struct*) _temp740)->f1; goto
_LL743;} else{ goto _LL744;} _LL744: goto _LL745; _LL743: if( Cyc_Tcutil_bits_only((
void*) _temp746->type)){ Cyc_Tcutil_warn( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp748=*(* _temp746->name).f2; xprintf("pointer-free %.*s may not be fully initialized",
_get_arr_size( _temp748, 1u), _temp748.curr);})); goto _LL741;} goto _LL745;
_LL745: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp749=
Cyc_CfAbsexp_absop2string( _temp680); xprintf("%.*s must be fully initialized to be used here",
_get_arr_size( _temp749, 1u), _temp749.curr);})); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL741:;}{ struct Cyc_Dict_Dict* _temp750= Cyc_CfFlowInfo_escape_pointsto(
_temp739, pinfo_dict); if( _temp750 == pinfo_dict){ return in_flow;}{ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp751=({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp752=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp752[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp753; _temp753.tag= Cyc_CfFlowInfo_InitsFL;
_temp753.f1= pinfo_dict; _temp753;}); _temp752;}); Cyc_NewControlFlow_update_tryflow(
env,( void*) _temp751); return( void*) _temp751;}}} _LL663: return( void*)({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp754=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp754[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp755; _temp755.tag= Cyc_CfFlowInfo_InitsFL;
_temp755.f1= Cyc_Dict_insert( pinfo_dict, _temp682, Cyc_Dict_lookup( env->roots,
_temp682)); _temp755;}); _temp754;}); _LL665: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL667: return in_flow; _LL669: return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp684, in_flow); _LL671: for( 0; _temp686 != 0; _temp686=(( struct Cyc_List_List*)
_check_null( _temp686))->tl){ in_flow= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)((
struct Cyc_List_List*) _check_null( _temp686))->hd, in_flow);} return in_flow;
_LL673: { void* out_flow=( void*) Cyc_CfFlowInfo_BottomFL; for( 0; _temp690 != 0;
_temp690=(( struct Cyc_List_List*) _check_null( _temp690))->tl){ out_flow= Cyc_CfFlowInfo_join_flow(
out_flow, Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct Cyc_List_List*)
_check_null( _temp690))->hd, in_flow));} return out_flow;} _LL675: { void*
_temp756= in_flow; do { in_flow= _temp756;{ struct Cyc_List_List* ael2= _temp694;
for( 0; ael2 != 0; ael2=(( struct Cyc_List_List*) _check_null( ael2))->tl){
_temp756= Cyc_CfFlowInfo_join_flow( _temp756, Cyc_CfAbsexp_eval_absexp( env, loc,(
void*)(( struct Cyc_List_List*) _check_null( ael2))->hd, _temp756));}}} while (
! Cyc_CfFlowInfo_flow_lessthan_approx( _temp756, in_flow));{ struct Cyc_Dict_Dict*
pinfo_dict;{ void* _temp757= in_flow; struct Cyc_Dict_Dict* _temp763; _LL759:
if( _temp757 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL760;} else{ goto
_LL761;} _LL761: if(( unsigned int) _temp757 > 1u?*(( int*) _temp757) == Cyc_CfFlowInfo_InitsFL:
0){ _LL764: _temp763=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp757)->f1;
goto _LL762;} else{ goto _LL758;} _LL760: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL762: pinfo_dict= _temp763; goto _LL758; _LL758:;}{ struct _RegionHandle
_temp765= _new_region(); struct _RegionHandle* spine_rgn=& _temp765;
_push_region( spine_rgn);{ struct Cyc_List_List* _temp768; struct Cyc_List_List*
_temp770; struct _tuple5 _temp766= Cyc_CfAbsexp_unordered_mallocs( spine_rgn, ae,
0); _LL771: _temp770= _temp766.f1; goto _LL769; _LL769: _temp768= _temp766.f2;
goto _LL767; _LL767: for( 0; _temp768 != 0; _temp768=(( struct Cyc_List_List*)
_check_null( _temp768))->tl){ struct Cyc_CfFlowInfo_MallocPt_struct* _temp772=({
struct Cyc_CfFlowInfo_MallocPt_struct* _temp773=( struct Cyc_CfFlowInfo_MallocPt_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp773[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp774; _temp774.tag= Cyc_CfFlowInfo_MallocPt;
_temp774.f1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp768))->hd; _temp774;}); _temp773;}); pinfo_dict= Cyc_Dict_insert(
pinfo_dict,( void*) _temp772, Cyc_Dict_lookup( env->roots,( void*) _temp772));}
_temp756=( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp775=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp775[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp776; _temp776.tag= Cyc_CfFlowInfo_InitsFL;
_temp776.f1= pinfo_dict; _temp776;}); _temp775;}); for( 0; _temp770 != 0;
_temp770=(( struct Cyc_List_List*) _check_null( _temp770))->tl){ _temp756= Cyc_CfAbsexp_eval_absexp(
env, loc,( void*)(( struct Cyc_List_List*) _check_null( _temp770))->hd, _temp756);}};
_pop_region( spine_rgn);} return _temp756;}} _LL657:;}}