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
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, int); extern
struct _tagged_arr ntCsl_to_ntsl( unsigned char**); extern int system(
unsigned char*); extern int* __errno(); struct Cyc_Stdio___sFILE; extern struct
Cyc_Stdio___sFILE* Cyc_Stdio_stderr; extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*);
extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; static const int Cyc_Stdio_String_pa= 0; struct Cyc_Stdio_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_Int_pa= 1; struct
Cyc_Stdio_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Stdio_Double_pa=
2; struct Cyc_Stdio_Double_pa_struct{ int tag; double f1; } ; static const int
Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{ int tag; short*
f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_Stdio_fprintf( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); extern struct _tagged_arr Cyc_Stdio_aprintf(
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_Stdio_ShortPtr_sa=
0; struct Cyc_Stdio_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_Stdio_UShortPtr_sa= 1; struct Cyc_Stdio_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_Stdio_IntPtr_sa= 2; struct Cyc_Stdio_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_Stdio_UIntPtr_sa= 3; struct Cyc_Stdio_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Stdio_StringPtr_sa= 4;
struct Cyc_Stdio_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_DoublePtr_sa= 5; struct Cyc_Stdio_DoublePtr_sa_struct{ int
tag; double* f1; } ; static const int Cyc_Stdio_FloatPtr_sa= 6; struct Cyc_Stdio_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_rev( struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List*
y); extern unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Dict_Dict; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* comp)( void*, void*)); extern
int Cyc_Dict_member( struct Cyc_Dict_Dict* d, void* key); extern struct Cyc_Dict_Dict*
Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key, void* data); extern void*
Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key); extern struct Cyc_Core_Opt*
Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void* key); struct Cyc_Set_Set;
extern struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*, void*)); extern
int Cyc_Set_member( struct Cyc_Set_Set* s, void* elt); extern unsigned char Cyc_Set_Absent[
11u]; struct Cyc_SlowDict_Dict; extern unsigned char Cyc_SlowDict_Present[ 12u];
extern unsigned char Cyc_SlowDict_Absent[ 11u]; extern struct Cyc_SlowDict_Dict*
Cyc_SlowDict_empty( int(* comp)( void*, void*)); extern int Cyc_SlowDict_is_empty(
struct Cyc_SlowDict_Dict* d); extern struct Cyc_SlowDict_Dict* Cyc_SlowDict_insert(
struct Cyc_SlowDict_Dict* d, void* key, void* data); extern struct Cyc_Core_Opt*
Cyc_SlowDict_lookup_opt( struct Cyc_SlowDict_Dict* d, void* key); extern struct
Cyc_SlowDict_Dict* Cyc_SlowDict_delete( struct Cyc_SlowDict_Dict* d, void* key);
struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct _tagged_arr line;
int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[ 9u]; struct
Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse=
1; static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; extern
int Cyc_String_zstrptrcmp( struct _tagged_arr*, struct _tagged_arr*); struct Cyc_PP_Ppstate;
struct Cyc_PP_Out; struct Cyc_PP_Doc; struct _tuple0{ void* f1; struct
_tagged_arr* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref;
struct Cyc_Absyn_PtrInfo; struct Cyc_Absyn_VarargInfo; struct Cyc_Absyn_FnInfo;
struct Cyc_Absyn_TunionInfo; struct Cyc_Absyn_TunionFieldInfo; struct Cyc_Absyn_VarargCallInfo;
struct Cyc_Absyn_Exp; struct Cyc_Absyn_Stmt; struct Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause;
struct Cyc_Absyn_SwitchC_clause; struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl;
struct Cyc_Absyn_Uniondecl; struct Cyc_Absyn_Tuniondecl; struct Cyc_Absyn_Tunionfield;
struct Cyc_Absyn_Enumfield; struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Typedefdecl;
struct Cyc_Absyn_Vardecl; struct Cyc_Absyn_Decl; struct Cyc_Absyn_Structfield;
static const int Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0; struct
Cyc_Absyn_Rel_n_struct{ int tag; struct Cyc_List_List* f1; } ; static const int
Cyc_Absyn_Abs_n= 1; struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_Static= 0; static const int Cyc_Absyn_Abstract=
1; static const int Cyc_Absyn_Public= 2; static const int Cyc_Absyn_Extern= 3;
static const int Cyc_Absyn_ExternC= 4; struct Cyc_Absyn_Tqual{ int q_const: 1;
int q_volatile: 1; int q_restrict: 1; } ; static const int Cyc_Absyn_B1= 0;
static const int Cyc_Absyn_B2= 1; static const int Cyc_Absyn_B4= 2; static const
int Cyc_Absyn_B8= 3; static const int Cyc_Absyn_AnyKind= 0; static const int Cyc_Absyn_MemKind=
1; static const int Cyc_Absyn_BoxKind= 2; static const int Cyc_Absyn_RgnKind= 3;
static const int Cyc_Absyn_EffKind= 4; static const int Cyc_Absyn_Signed= 0;
static const int Cyc_Absyn_Unsigned= 1; struct Cyc_Absyn_Conref{ void* v; } ;
static const int Cyc_Absyn_Eq_constr= 0; struct Cyc_Absyn_Eq_constr_struct{ int
tag; void* f1; } ; static const int Cyc_Absyn_Forward_constr= 1; struct Cyc_Absyn_Forward_constr_struct{
int tag; struct Cyc_Absyn_Conref* f1; } ; static const int Cyc_Absyn_No_constr=
0; struct Cyc_Absyn_Tvar{ struct _tagged_arr* name; int* identity; struct Cyc_Absyn_Conref*
kind; } ; static const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b=
0; struct Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
struct Cyc_Absyn_PtrInfo{ void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref*
nullable; struct Cyc_Absyn_Tqual tq; struct Cyc_Absyn_Conref* bounds; } ; struct
Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt* name; struct Cyc_Absyn_Tqual tq; void*
type; void* rgn; int inject; } ; struct Cyc_Absyn_FnInfo{ struct Cyc_List_List*
tvars; struct Cyc_Core_Opt* effect; void* ret_typ; struct Cyc_List_List* args;
int c_varargs; struct Cyc_Absyn_VarargInfo* cyc_varargs; struct Cyc_List_List*
rgn_po; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{
struct _tuple0* name; int is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion=
0; struct Cyc_Absyn_UnknownTunion_struct{ int tag; struct Cyc_Absyn_UnknownTunionInfo
f1; } ; static const int Cyc_Absyn_KnownTunion= 1; struct Cyc_Absyn_KnownTunion_struct{
int tag; struct Cyc_Absyn_Tuniondecl* f1; } ; struct Cyc_Absyn_TunionInfo{ void*
tunion_info; struct Cyc_List_List* targs; void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{
struct _tuple0* tunion_name; struct _tuple0* field_name; int is_xtunion; } ;
static const int Cyc_Absyn_UnknownTunionfield= 0; struct Cyc_Absyn_UnknownTunionfield_struct{
int tag; struct Cyc_Absyn_UnknownTunionFieldInfo f1; } ; static const int Cyc_Absyn_KnownTunionfield=
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
15u]; extern int Cyc_Absyn_varlist_cmp( struct Cyc_List_List*, struct Cyc_List_List*);
extern int Cyc_Absyn_tvar_cmp( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*);
extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref( void* x); extern struct _tuple0* Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Tuniondecl* Cyc_Absyn_exn_tud; extern struct _tuple0*
Cyc_Absyn_null_pointer_exn_name; extern struct _tuple0* Cyc_Absyn_match_exn_name;
extern struct Cyc_Absyn_Tunionfield* Cyc_Absyn_null_pointer_exn_tuf; extern
struct Cyc_Absyn_Tunionfield* Cyc_Absyn_match_exn_tuf; extern void* Cyc_Absyn_tagged_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq); struct Cyc_Absynpp_Params{ int
expand_typedefs: 1; int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1;
int decls_first: 1; int rewrite_temp_tvars: 1; int print_all_tvars: 1; int
print_all_kinds: 1; int print_using_stmts: 1; int print_externC_stmts: 1; int
print_full_evars: 1; int use_curr_namespace: 1; struct Cyc_List_List*
curr_namespace; } ; extern struct _tagged_arr Cyc_Absynpp_typ2string( void*);
extern struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple0*); static
const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1;
} ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
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
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init(); extern struct Cyc_Tcenv_Genv*
Cyc_Tcenv_empty_genv(); extern struct Cyc_Tcenv_Fenv* Cyc_Tcenv_new_fenv( struct
Cyc_Position_Segment*, struct Cyc_Absyn_Fndecl*); extern struct Cyc_List_List*
Cyc_Tcenv_resolve_namespace( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tagged_arr*, struct Cyc_List_List*); extern void* Cyc_Tcenv_lookup_ordinary(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple0*); extern
struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple0*); extern struct Cyc_Absyn_Uniondecl**
Cyc_Tcenv_lookup_uniondecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple0*); extern struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple0*); extern
struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple0*); extern struct Cyc_Absyn_Enumdecl**
Cyc_Tcenv_lookup_enumdecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple0*); extern struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple0*); extern
void* Cyc_Tcenv_return_typ( struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Vardecl*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_pat_var(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Vardecl*);
extern struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars( struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_in_loop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt*
continue_dest); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_switch( struct
Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_fallthru( struct
Cyc_Tcenv_Tenv* te, struct Cyc_List_List* new_tvs, struct Cyc_List_List* vds,
struct Cyc_Absyn_Switch_clause* clause); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_clear_fallthru(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_next(
struct Cyc_Tcenv_Tenv*, void*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_enter_try(
struct Cyc_Tcenv_Tenv* te); extern int Cyc_Tcenv_get_try_depth( struct Cyc_Tcenv_Tenv*
te); extern void Cyc_Tcenv_process_continue( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*,
struct Cyc_Absyn_Stmt**); extern void Cyc_Tcenv_process_break( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*, struct Cyc_Absyn_Stmt**); extern void Cyc_Tcenv_process_goto(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*, struct _tagged_arr*, struct Cyc_Absyn_Stmt**);
struct _tuple3{ struct Cyc_Absyn_Switch_clause* f1; struct Cyc_List_List* f2;
struct Cyc_List_List* f3; } ; extern struct _tuple3* Cyc_Tcenv_process_fallthru(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*, struct Cyc_Absyn_Switch_clause***);
extern struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser( struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_label(
struct Cyc_Tcenv_Tenv*, struct _tagged_arr*, struct Cyc_Absyn_Stmt*); extern int
Cyc_Tcenv_all_labels_resolved( struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_block( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_named_block( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Tvar* name); extern void* Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_region(
struct Cyc_Tcenv_Tenv* te, void* r); extern void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void* rgn); extern void
Cyc_Tcenv_check_effect_accessible( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void* eff); extern int Cyc_Tcenv_region_outlives( struct Cyc_Tcenv_Tenv*,
void* r1, void* r2); extern void Cyc_Tcenv_check_rgn_partial_order( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct Cyc_List_List* po); extern
unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap); extern struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set;
extern void* Cyc_Tcutil_tvar_kind( struct Cyc_Absyn_Tvar* t); extern void* Cyc_Tcutil_compress(
void* t); extern int Cyc_Tcutil_subset_effect( int set_to_empty, void* e1, void*
e2); extern int Cyc_Tcutil_region_in_effect( int constrain, void* r, void* e);
extern void Cyc_Tcutil_check_unique_tvars( struct Cyc_Position_Segment*, struct
Cyc_List_List*); extern struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar( void* k);
extern int* Cyc_Tcutil_new_tvar_id(); extern void Cyc_Tcutil_add_tvar_identity(
struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_add_tvar_identities( struct Cyc_List_List*);
unsigned char Cyc_Tcenv_Env_error[ 14u]="\000\000\000\000Env_error"; void* Cyc_Tcenv_env_err(
struct _tagged_arr msg){({ struct Cyc_Stdio_String_pa_struct _temp1; _temp1.tag=
Cyc_Stdio_String_pa; _temp1.f1=( struct _tagged_arr) msg;{ void* _temp0[ 1u]={&
_temp1}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr, _tag_arr("Error: %s\n", sizeof(
unsigned char), 11u), _tag_arr( _temp0, sizeof( void*), 1u));}}); Cyc_Stdio_fflush((
struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( int) _throw(( void*) Cyc_Tcenv_Env_error);}
struct Cyc_Tcenv_Tenv; struct Cyc_Tcenv_Genv; struct Cyc_Tcenv_CtrlEnv{ void*
continue_stmt; void* break_stmt; struct _tuple3* fallthru_clause; void*
next_stmt; int try_depth; } ; struct Cyc_Tcenv_SharedFenv{ void* return_typ;
struct Cyc_Dict_Dict* seen_labels; struct Cyc_SlowDict_Dict* needed_labels; } ;
struct Cyc_Tcenv_RgnPO{ struct Cyc_Absyn_Tvar* tvar; struct Cyc_List_List*
ancestors; } ; struct Cyc_Tcenv_Fenv{ struct Cyc_Tcenv_SharedFenv* shared;
struct Cyc_List_List* type_vars; struct Cyc_List_List* region_order; struct Cyc_Dict_Dict*
locals; struct Cyc_Absyn_Stmt* encloser; struct Cyc_Tcenv_CtrlEnv* ctrl_env;
void* capability; void* curr_rgn; } ; struct _tuple4{ void* f1; int f2; } ;
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init(){ Cyc_Tcutil_empty_var_set=({ struct
Cyc_Core_Opt* _temp2=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Set_empty)( Cyc_String_zstrptrcmp); _temp2;});{
struct Cyc_Tcenv_Genv* _temp3= Cyc_Tcenv_empty_genv(); struct Cyc_List_List*
_temp4= 0; _temp3->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)(
_temp3->tuniondecls,(* Cyc_Absyn_exn_name).f2,& Cyc_Absyn_exn_tud); _temp3->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct _tuple4* data)) Cyc_Dict_insert)( _temp3->ordinaries,(* Cyc_Absyn_null_pointer_exn_name).f2,({
struct _tuple4* _temp5=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp5->f1=( void*)({ struct Cyc_Tcenv_TunionRes_struct* _temp6=( struct Cyc_Tcenv_TunionRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct)); _temp6[ 0]=({ struct Cyc_Tcenv_TunionRes_struct
_temp7; _temp7.tag= Cyc_Tcenv_TunionRes; _temp7.f1= Cyc_Absyn_exn_tud; _temp7.f2=
Cyc_Absyn_null_pointer_exn_tuf; _temp7;}); _temp6;}); _temp5->f2= 1; _temp5;}));
_temp3->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key, struct _tuple4* data)) Cyc_Dict_insert)( _temp3->ordinaries,(*
Cyc_Absyn_match_exn_name).f2,({ struct _tuple4* _temp8=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp8->f1=( void*)({ struct Cyc_Tcenv_TunionRes_struct*
_temp9=( struct Cyc_Tcenv_TunionRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct));
_temp9[ 0]=({ struct Cyc_Tcenv_TunionRes_struct _temp10; _temp10.tag= Cyc_Tcenv_TunionRes;
_temp10.f1= Cyc_Absyn_exn_tud; _temp10.f2= Cyc_Absyn_match_exn_tuf; _temp10;});
_temp9;}); _temp8->f2= 1; _temp8;}));{ struct Cyc_Dict_Dict* _temp11=(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct
Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)((( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp),
_temp4, _temp3); return({ struct Cyc_Tcenv_Tenv* _temp12=( struct Cyc_Tcenv_Tenv*)
GC_malloc( sizeof( struct Cyc_Tcenv_Tenv)); _temp12->ns= _temp4; _temp12->ae=
_temp11; _temp12->le= 0; _temp12;});}}} static struct Cyc_Tcenv_Genv* Cyc_Tcenv_lookup_namespace(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tagged_arr*
n, struct Cyc_List_List* ns){ return(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, Cyc_Tcenv_resolve_namespace(
te, loc, n, ns));} static struct Cyc_List_List* Cyc_Tcenv_outer_namespace(
struct Cyc_List_List* ns){ if( ns == 0){ return(( struct Cyc_List_List*(*)(
struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv::outer_namespace",
sizeof( unsigned char), 23u));} return(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)((( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( ns)))->tl);} static void Cyc_Tcenv_check_repeat(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void*(* lookup)(
struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v, struct Cyc_List_List*
nss){ for( 0; nss != 0; nss=(( struct Cyc_List_List*) _check_null( nss))->tl){
struct Cyc_Tcenv_Genv* ge2=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,( struct Cyc_List_List*)((
struct Cyc_List_List*) _check_null( nss))->hd); struct _handler_cons _temp13;
_push_handler(& _temp13);{ int _temp15= 0; if( setjmp( _temp13.handler)){
_temp15= 1;} if( ! _temp15){ lookup( ge2, v);({ struct Cyc_Stdio_String_pa_struct
_temp17; _temp17.tag= Cyc_Stdio_String_pa; _temp17.f1=( struct _tagged_arr)* v;{
void* _temp16[ 1u]={& _temp17}; Cyc_Tcutil_terr( loc, _tag_arr("%s is ambiguous",
sizeof( unsigned char), 16u), _tag_arr( _temp16, sizeof( void*), 1u));}});;
_pop_handler();} else{ void* _temp14=( void*) _exn_thrown; void* _temp19=
_temp14; _LL21: if( _temp19 == Cyc_Dict_Absent){ goto _LL22;} else{ goto _LL23;}
_LL23: goto _LL24; _LL22: goto _LL20; _LL24:( void) _throw( _temp19); _LL20:;}}}
return;} static void* Cyc_Tcenv_scoped_lookup( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void*(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_arr*), struct _tagged_arr* v){ struct Cyc_List_List* cns= te->ns; while(
1) { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, cns);{ struct
_handler_cons _temp25; _push_handler(& _temp25);{ int _temp27= 0; if( setjmp(
_temp25.handler)){ _temp27= 1;} if( ! _temp27){{ void* result= lookup( ge, v);
Cyc_Tcenv_check_repeat( te, loc, lookup, v, ge->availables);{ void* _temp28=
result; _npop_handler( 0u); return _temp28;}}; _pop_handler();} else{ void*
_temp26=( void*) _exn_thrown; void* _temp30= _temp26; _LL32: if( _temp30 == Cyc_Dict_Absent){
goto _LL33;} else{ goto _LL34;} _LL34: goto _LL35; _LL33: goto _LL31; _LL35:(
void) _throw( _temp30); _LL31:;}}}{ struct Cyc_List_List* _temp36= ge->availables;
for( 0; _temp36 != 0; _temp36=(( struct Cyc_List_List*) _check_null( _temp36))->tl){
struct Cyc_Tcenv_Genv* ge2=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,( struct Cyc_List_List*)((
struct Cyc_List_List*) _check_null( _temp36))->hd); struct _handler_cons _temp37;
_push_handler(& _temp37);{ int _temp39= 0; if( setjmp( _temp37.handler)){
_temp39= 1;} if( ! _temp39){{ void* result= lookup( ge2, v); Cyc_Tcenv_check_repeat(
te, loc, lookup, v,(( struct Cyc_List_List*) _check_null( _temp36))->tl);{ void*
_temp40= result; _npop_handler( 0u); return _temp40;}}; _pop_handler();} else{
void* _temp38=( void*) _exn_thrown; void* _temp42= _temp38; _LL44: if( _temp42
== Cyc_Dict_Absent){ goto _LL45;} else{ goto _LL46;} _LL46: goto _LL47; _LL45:
goto _LL43; _LL47:( void) _throw( _temp42); _LL43:;}}}} if( cns == 0){( int)
_throw(( void*) Cyc_Dict_Absent);} cns= Cyc_Tcenv_outer_namespace( cns);}}
static void* Cyc_Tcenv_lookup_ordinary_global_f( struct Cyc_Tcenv_Genv* ge,
struct _tagged_arr* v){ struct _tuple4* ans=(( struct _tuple4*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->ordinaries, v);(* ans).f2= 1;
return(* ans).f1;} static void* Cyc_Tcenv_lookup_ordinary_global( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp50;
struct _tagged_arr* _temp51; void* _temp53; struct _tuple0* _temp48= q; _temp50=*
_temp48; _LL54: _temp53= _temp50.f1; goto _LL52; _LL52: _temp51= _temp50.f2;
goto _LL49; _LL49: { void* _temp55= _temp53; struct Cyc_List_List* _temp65;
struct Cyc_List_List* _temp67; struct Cyc_List_List _temp69; struct Cyc_List_List*
_temp70; struct _tagged_arr* _temp72; struct Cyc_List_List* _temp74; _LL57: if(
_temp55 ==( void*) Cyc_Absyn_Loc_n){ goto _LL58;} else{ goto _LL59;} _LL59: if((
unsigned int) _temp55 > 1u?*(( int*) _temp55) == Cyc_Absyn_Rel_n: 0){ _LL66:
_temp65=(( struct Cyc_Absyn_Rel_n_struct*) _temp55)->f1; if( _temp65 == 0){ goto
_LL60;} else{ goto _LL61;}} else{ goto _LL61;} _LL61: if(( unsigned int) _temp55
> 1u?*(( int*) _temp55) == Cyc_Absyn_Rel_n: 0){ _LL68: _temp67=(( struct Cyc_Absyn_Rel_n_struct*)
_temp55)->f1; if( _temp67 == 0){ goto _LL63;} else{ _temp69=* _temp67; _LL73:
_temp72=( struct _tagged_arr*) _temp69.hd; goto _LL71; _LL71: _temp70= _temp69.tl;
goto _LL62;}} else{ goto _LL63;} _LL63: if(( unsigned int) _temp55 > 1u?*(( int*)
_temp55) == Cyc_Absyn_Abs_n: 0){ _LL75: _temp74=(( struct Cyc_Absyn_Abs_n_struct*)
_temp55)->f1; goto _LL64;} else{ goto _LL56;} _LL58: goto _LL60; _LL60: return
Cyc_Tcenv_scoped_lookup( te, loc, Cyc_Tcenv_lookup_ordinary_global_f, _temp51);
_LL62: { struct Cyc_Tcenv_Genv* _temp76= Cyc_Tcenv_lookup_namespace( te, loc,
_temp72, _temp70); return Cyc_Tcenv_lookup_ordinary_global_f( _temp76, _temp51);}
_LL64: return Cyc_Tcenv_lookup_ordinary_global_f((( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp74), _temp51); _LL56:;}} struct Cyc_List_List* Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tagged_arr*
n, struct Cyc_List_List* ns){ struct Cyc_List_List* _temp77= te->ns; struct Cyc_List_List*
_temp78= 0; while( 1) { struct Cyc_Tcenv_Genv* _temp79=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp77);{ struct Cyc_List_List* _temp80= _temp79->availables; for( 0; _temp80
!= 0; _temp80=(( struct Cyc_List_List*) _check_null( _temp80))->tl){ struct Cyc_Tcenv_Genv*
ge2=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( te->ae,( struct Cyc_List_List*)(( struct Cyc_List_List*)
_check_null( _temp80))->hd); if((( int(*)( struct Cyc_Set_Set* s, struct
_tagged_arr* elt)) Cyc_Set_member)( ge2->namespaces, n)){ _temp78=({ struct Cyc_List_List*
_temp81=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp81->hd=( void*)(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( _temp77,({ struct Cyc_List_List* _temp82=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp82->hd=(
void*) n; _temp82->tl= ns; _temp82;})); _temp81->tl= _temp78; _temp81;});}}} if(((
int(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_member)(
_temp79->namespaces, n)){ _temp78=({ struct Cyc_List_List* _temp83=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp83->hd=( void*)(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp77,({
struct Cyc_List_List* _temp84=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp84->hd=( void*) n; _temp84->tl= ns; _temp84;})); _temp83->tl=
_temp78; _temp83;});} if( _temp78 != 0){ if((( struct Cyc_List_List*)
_check_null( _temp78))->tl != 0){({ struct Cyc_Stdio_String_pa_struct _temp86;
_temp86.tag= Cyc_Stdio_String_pa; _temp86.f1=( struct _tagged_arr)* n;{ void*
_temp85[ 1u]={& _temp86}; Cyc_Tcutil_terr( loc, _tag_arr("%s is ambiguous",
sizeof( unsigned char), 16u), _tag_arr( _temp85, sizeof( void*), 1u));}});}
return( struct Cyc_List_List*)(( struct Cyc_List_List*) _check_null( _temp78))->hd;}
if( _temp77 == 0){( int) _throw(( void*) Cyc_Dict_Absent);} _temp77= Cyc_Tcenv_outer_namespace(
_temp77);}} static struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Structdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->structdecls,
v);} struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp89;
struct _tagged_arr* _temp90; void* _temp92; struct _tuple0* _temp87= q; _temp89=*
_temp87; _LL93: _temp92= _temp89.f1; goto _LL91; _LL91: _temp90= _temp89.f2;
goto _LL88; _LL88: { void* _temp94= _temp92; struct Cyc_List_List* _temp104;
struct Cyc_List_List* _temp106; struct Cyc_List_List* _temp108; struct Cyc_List_List
_temp110; struct Cyc_List_List* _temp111; struct _tagged_arr* _temp113; _LL96:
if( _temp94 ==( void*) Cyc_Absyn_Loc_n){ goto _LL97;} else{ goto _LL98;} _LL98:
if(( unsigned int) _temp94 > 1u?*(( int*) _temp94) == Cyc_Absyn_Rel_n: 0){
_LL105: _temp104=(( struct Cyc_Absyn_Rel_n_struct*) _temp94)->f1; if( _temp104
== 0){ goto _LL99;} else{ goto _LL100;}} else{ goto _LL100;} _LL100: if((
unsigned int) _temp94 > 1u?*(( int*) _temp94) == Cyc_Absyn_Abs_n: 0){ _LL107:
_temp106=(( struct Cyc_Absyn_Abs_n_struct*) _temp94)->f1; goto _LL101;} else{
goto _LL102;} _LL102: if(( unsigned int) _temp94 > 1u?*(( int*) _temp94) == Cyc_Absyn_Rel_n:
0){ _LL109: _temp108=(( struct Cyc_Absyn_Rel_n_struct*) _temp94)->f1; if(
_temp108 == 0){ goto _LL95;} else{ _temp110=* _temp108; _LL114: _temp113=(
struct _tagged_arr*) _temp110.hd; goto _LL112; _LL112: _temp111= _temp110.tl;
goto _LL103;}} else{ goto _LL95;} _LL97: goto _LL99; _LL99: return(( struct Cyc_Absyn_Structdecl**(*)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v))
Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_structdecl_f, _temp90);
_LL101: { struct Cyc_Tcenv_Genv* _temp115=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp106);
return(( struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup)( _temp115->structdecls, _temp90);} _LL103: {
struct Cyc_Tcenv_Genv* _temp116= Cyc_Tcenv_lookup_namespace( te, loc, _temp113,
_temp111); return(( struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp116->structdecls, _temp90);}
_LL95:;}} static struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Uniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->uniondecls,
v);} struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp119; struct _tagged_arr* _temp120; void* _temp122; struct _tuple0* _temp117=
q; _temp119=* _temp117; _LL123: _temp122= _temp119.f1; goto _LL121; _LL121:
_temp120= _temp119.f2; goto _LL118; _LL118: { void* _temp124= _temp122; struct
Cyc_List_List* _temp134; struct Cyc_List_List* _temp136; struct Cyc_List_List*
_temp138; struct Cyc_List_List _temp140; struct Cyc_List_List* _temp141; struct
_tagged_arr* _temp143; _LL126: if( _temp124 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL127;} else{ goto _LL128;} _LL128: if(( unsigned int) _temp124 > 1u?*(( int*)
_temp124) == Cyc_Absyn_Rel_n: 0){ _LL135: _temp134=(( struct Cyc_Absyn_Rel_n_struct*)
_temp124)->f1; if( _temp134 == 0){ goto _LL129;} else{ goto _LL130;}} else{ goto
_LL130;} _LL130: if(( unsigned int) _temp124 > 1u?*(( int*) _temp124) == Cyc_Absyn_Abs_n:
0){ _LL137: _temp136=(( struct Cyc_Absyn_Abs_n_struct*) _temp124)->f1; goto
_LL131;} else{ goto _LL132;} _LL132: if(( unsigned int) _temp124 > 1u?*(( int*)
_temp124) == Cyc_Absyn_Rel_n: 0){ _LL139: _temp138=(( struct Cyc_Absyn_Rel_n_struct*)
_temp124)->f1; if( _temp138 == 0){ goto _LL125;} else{ _temp140=* _temp138;
_LL144: _temp143=( struct _tagged_arr*) _temp140.hd; goto _LL142; _LL142:
_temp141= _temp140.tl; goto _LL133;}} else{ goto _LL125;} _LL127: goto _LL129;
_LL129: return(( struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl**(* lookup)( struct
Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v)) Cyc_Tcenv_scoped_lookup)(
te, loc, Cyc_Tcenv_lookup_uniondecl_f, _temp120); _LL131: { struct Cyc_Tcenv_Genv*
_temp145=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( te->ae, _temp136); return(( struct Cyc_Absyn_Uniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp145->uniondecls,
_temp120);} _LL133: { struct Cyc_Tcenv_Genv* _temp146= Cyc_Tcenv_lookup_namespace(
te, loc, _temp143, _temp141); return(( struct Cyc_Absyn_Uniondecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp146->uniondecls,
_temp120);} _LL125:;}} static struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->tuniondecls,
v);} struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp149; struct _tagged_arr* _temp150; void* _temp152; struct _tuple0* _temp147=
q; _temp149=* _temp147; _LL153: _temp152= _temp149.f1; goto _LL151; _LL151:
_temp150= _temp149.f2; goto _LL148; _LL148: { void* _temp154= _temp152; struct
Cyc_List_List* _temp164; struct Cyc_List_List* _temp166; struct Cyc_List_List
_temp168; struct Cyc_List_List* _temp169; struct _tagged_arr* _temp171; struct
Cyc_List_List* _temp173; _LL156: if( _temp154 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL157;} else{ goto _LL158;} _LL158: if(( unsigned int) _temp154 > 1u?*(( int*)
_temp154) == Cyc_Absyn_Rel_n: 0){ _LL165: _temp164=(( struct Cyc_Absyn_Rel_n_struct*)
_temp154)->f1; if( _temp164 == 0){ goto _LL159;} else{ goto _LL160;}} else{ goto
_LL160;} _LL160: if(( unsigned int) _temp154 > 1u?*(( int*) _temp154) == Cyc_Absyn_Rel_n:
0){ _LL167: _temp166=(( struct Cyc_Absyn_Rel_n_struct*) _temp154)->f1; if(
_temp166 == 0){ goto _LL162;} else{ _temp168=* _temp166; _LL172: _temp171=(
struct _tagged_arr*) _temp168.hd; goto _LL170; _LL170: _temp169= _temp168.tl;
goto _LL161;}} else{ goto _LL162;} _LL162: if(( unsigned int) _temp154 > 1u?*((
int*) _temp154) == Cyc_Absyn_Abs_n: 0){ _LL174: _temp173=(( struct Cyc_Absyn_Abs_n_struct*)
_temp154)->f1; goto _LL163;} else{ goto _LL155;} _LL157: goto _LL159; _LL159:
return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Tuniondecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_arr*), struct _tagged_arr* v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_tuniondecl_f,
_temp150); _LL161: { struct Cyc_Tcenv_Genv* _temp175= Cyc_Tcenv_lookup_namespace(
te, loc, _temp171, _temp169); return(( struct Cyc_Absyn_Tuniondecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp175->tuniondecls,
_temp150);} _LL163: { struct Cyc_Tcenv_Genv* _temp176=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp173); return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp176->tuniondecls, _temp150);}
_LL155:;}} static struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_xtuniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->tuniondecls,
v);} struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp179; struct _tagged_arr* _temp180; void* _temp182; struct _tuple0* _temp177=
q; _temp179=* _temp177; _LL183: _temp182= _temp179.f1; goto _LL181; _LL181:
_temp180= _temp179.f2; goto _LL178; _LL178: { void* _temp184= _temp182; struct
Cyc_List_List* _temp194; struct Cyc_List_List* _temp196; struct Cyc_List_List
_temp198; struct Cyc_List_List* _temp199; struct _tagged_arr* _temp201; struct
Cyc_List_List* _temp203; _LL186: if(( unsigned int) _temp184 > 1u?*(( int*)
_temp184) == Cyc_Absyn_Rel_n: 0){ _LL195: _temp194=(( struct Cyc_Absyn_Rel_n_struct*)
_temp184)->f1; if( _temp194 == 0){ goto _LL187;} else{ goto _LL188;}} else{ goto
_LL188;} _LL188: if( _temp184 ==( void*) Cyc_Absyn_Loc_n){ goto _LL189;} else{
goto _LL190;} _LL190: if(( unsigned int) _temp184 > 1u?*(( int*) _temp184) ==
Cyc_Absyn_Rel_n: 0){ _LL197: _temp196=(( struct Cyc_Absyn_Rel_n_struct*)
_temp184)->f1; if( _temp196 == 0){ goto _LL192;} else{ _temp198=* _temp196;
_LL202: _temp201=( struct _tagged_arr*) _temp198.hd; goto _LL200; _LL200:
_temp199= _temp198.tl; goto _LL191;}} else{ goto _LL192;} _LL192: if((
unsigned int) _temp184 > 1u?*(( int*) _temp184) == Cyc_Absyn_Abs_n: 0){ _LL204:
_temp203=(( struct Cyc_Absyn_Abs_n_struct*) _temp184)->f1; goto _LL193;} else{
goto _LL185;} _LL187: { struct _handler_cons _temp205; _push_handler(& _temp205);{
int _temp207= 0; if( setjmp( _temp205.handler)){ _temp207= 1;} if( ! _temp207){{
struct Cyc_Core_Opt* _temp209=({ struct Cyc_Core_Opt* _temp208=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp208->v=( void*)(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v))
Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_xtuniondecl_f, _temp180);
_temp208;}); _npop_handler( 0u); return _temp209;}; _pop_handler();} else{ void*
_temp206=( void*) _exn_thrown; void* _temp211= _temp206; _LL213: if( _temp211 ==
Cyc_Dict_Absent){ goto _LL214;} else{ goto _LL215;} _LL215: goto _LL216; _LL214:
return 0; _LL216:( void) _throw( _temp211); _LL212:;}}} _LL189:({ void* _temp217[
0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("lookup_xtuniondecl: impossible", sizeof(
unsigned char), 31u), _tag_arr( _temp217, sizeof( void*), 0u));}); return 0;
_LL191: { struct Cyc_Tcenv_Genv* ge;{ struct _handler_cons _temp218;
_push_handler(& _temp218);{ int _temp220= 0; if( setjmp( _temp218.handler)){
_temp220= 1;} if( ! _temp220){ ge= Cyc_Tcenv_lookup_namespace( te, loc, _temp201,
_temp199);; _pop_handler();} else{ void* _temp219=( void*) _exn_thrown; void*
_temp222= _temp219; _LL224: if( _temp222 == Cyc_Dict_Absent){ goto _LL225;}
else{ goto _LL226;} _LL226: goto _LL227; _LL225:({ void* _temp228[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bad qualified name for xtunion", sizeof( unsigned char), 31u),
_tag_arr( _temp228, sizeof( void*), 0u));});( int) _throw(( void*) Cyc_Dict_Absent);
_LL227:( void) _throw( _temp222); _LL223:;}}} return({ struct Cyc_Core_Opt*
_temp229=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp229->v=( void*)(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->tuniondecls, _temp180); _temp229;});}
_LL193: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp203); return({
struct Cyc_Core_Opt* _temp230=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp230->v=( void*)(( struct Cyc_Absyn_Tuniondecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->tuniondecls,
_temp180); _temp230;});} _LL185:;}} static struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->enumdecls,
v);} struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp233; struct _tagged_arr* _temp234; void* _temp236; struct _tuple0* _temp231=
q; _temp233=* _temp231; _LL237: _temp236= _temp233.f1; goto _LL235; _LL235:
_temp234= _temp233.f2; goto _LL232; _LL232: { void* _temp238= _temp236; struct
Cyc_List_List* _temp248; struct Cyc_List_List* _temp250; struct Cyc_List_List
_temp252; struct Cyc_List_List* _temp253; struct _tagged_arr* _temp255; struct
Cyc_List_List* _temp257; _LL240: if( _temp238 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL241;} else{ goto _LL242;} _LL242: if(( unsigned int) _temp238 > 1u?*(( int*)
_temp238) == Cyc_Absyn_Rel_n: 0){ _LL249: _temp248=(( struct Cyc_Absyn_Rel_n_struct*)
_temp238)->f1; if( _temp248 == 0){ goto _LL243;} else{ goto _LL244;}} else{ goto
_LL244;} _LL244: if(( unsigned int) _temp238 > 1u?*(( int*) _temp238) == Cyc_Absyn_Rel_n:
0){ _LL251: _temp250=(( struct Cyc_Absyn_Rel_n_struct*) _temp238)->f1; if(
_temp250 == 0){ goto _LL246;} else{ _temp252=* _temp250; _LL256: _temp255=(
struct _tagged_arr*) _temp252.hd; goto _LL254; _LL254: _temp253= _temp252.tl;
goto _LL245;}} else{ goto _LL246;} _LL246: if(( unsigned int) _temp238 > 1u?*((
int*) _temp238) == Cyc_Absyn_Abs_n: 0){ _LL258: _temp257=(( struct Cyc_Absyn_Abs_n_struct*)
_temp238)->f1; goto _LL247;} else{ goto _LL239;} _LL241: goto _LL243; _LL243:
return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Enumdecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_arr*), struct _tagged_arr* v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_enumdecl_f,
_temp234); _LL245: { struct Cyc_Tcenv_Genv* _temp259= Cyc_Tcenv_lookup_namespace(
te, loc, _temp255, _temp253); return(( struct Cyc_Absyn_Enumdecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp259->enumdecls,
_temp234);} _LL247: { struct Cyc_Tcenv_Genv* _temp260=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp257); return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp260->enumdecls, _temp234);}
_LL239:;}} static struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->typedefs,
v);} struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp263; struct _tagged_arr* _temp264; void* _temp266; struct _tuple0* _temp261=
q; _temp263=* _temp261; _LL267: _temp266= _temp263.f1; goto _LL265; _LL265:
_temp264= _temp263.f2; goto _LL262; _LL262: { void* _temp268= _temp266; struct
Cyc_List_List* _temp278; struct Cyc_List_List* _temp280; struct Cyc_List_List
_temp282; struct Cyc_List_List* _temp283; struct _tagged_arr* _temp285; struct
Cyc_List_List* _temp287; _LL270: if( _temp268 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL271;} else{ goto _LL272;} _LL272: if(( unsigned int) _temp268 > 1u?*(( int*)
_temp268) == Cyc_Absyn_Rel_n: 0){ _LL279: _temp278=(( struct Cyc_Absyn_Rel_n_struct*)
_temp268)->f1; if( _temp278 == 0){ goto _LL273;} else{ goto _LL274;}} else{ goto
_LL274;} _LL274: if(( unsigned int) _temp268 > 1u?*(( int*) _temp268) == Cyc_Absyn_Rel_n:
0){ _LL281: _temp280=(( struct Cyc_Absyn_Rel_n_struct*) _temp268)->f1; if(
_temp280 == 0){ goto _LL276;} else{ _temp282=* _temp280; _LL286: _temp285=(
struct _tagged_arr*) _temp282.hd; goto _LL284; _LL284: _temp283= _temp282.tl;
goto _LL275;}} else{ goto _LL276;} _LL276: if(( unsigned int) _temp268 > 1u?*((
int*) _temp268) == Cyc_Absyn_Abs_n: 0){ _LL288: _temp287=(( struct Cyc_Absyn_Abs_n_struct*)
_temp268)->f1; goto _LL277;} else{ goto _LL269;} _LL271: goto _LL273; _LL273:
return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Typedefdecl*(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_arr*), struct _tagged_arr* v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_typedefdecl_f,
_temp264); _LL275: { struct Cyc_Tcenv_Genv* _temp289= Cyc_Tcenv_lookup_namespace(
te, loc, _temp285, _temp283); return(( struct Cyc_Absyn_Typedefdecl*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp289->typedefs,
_temp264);} _LL277: { struct Cyc_Tcenv_Genv* _temp290=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp287); return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp290->typedefs, _temp264);}
_LL269:;}} struct Cyc_Tcenv_Genv* Cyc_Tcenv_empty_genv(){ return({ struct Cyc_Tcenv_Genv*
_temp291=( struct Cyc_Tcenv_Genv*) GC_malloc( sizeof( struct Cyc_Tcenv_Genv));
_temp291->namespaces=( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Tcutil_empty_var_set))->v; _temp291->structdecls=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp291->uniondecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp291->tuniondecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp291->enumdecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp291->typedefs=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp291->ordinaries=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp291->availables= 0; _temp291;});} void* Cyc_Tcenv_get_visible( void* f){
while( 1) { void* _temp292= f; void* _temp300; void* _temp302; void* _temp304;
void* _temp306; _LL294: if(*(( int*) _temp292) == Cyc_Tcenv_Outermost){ _LL301:
_temp300=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp292)->f1; goto
_LL295;} else{ goto _LL296;} _LL296: if(*(( int*) _temp292) == Cyc_Tcenv_Frame){
_LL303: _temp302=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp292)->f1; goto
_LL297;} else{ goto _LL298;} _LL298: if(*(( int*) _temp292) == Cyc_Tcenv_Hidden){
_LL307: _temp306=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp292)->f1; goto
_LL305; _LL305: _temp304=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp292)->f2;
goto _LL299;} else{ goto _LL293;} _LL295: return _temp300; _LL297: return
_temp302; _LL299: f= _temp304; goto _LL293; _LL293:;}} void* Cyc_Tcenv_put_visible(
void* f, void* x){ void* _temp308= f; void* _temp316; void* _temp318; void*
_temp320; void* _temp322; void* _temp324; _LL310: if(*(( int*) _temp308) == Cyc_Tcenv_Outermost){
_LL317: _temp316=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp308)->f1;
goto _LL311;} else{ goto _LL312;} _LL312: if(*(( int*) _temp308) == Cyc_Tcenv_Frame){
_LL321: _temp320=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp308)->f1; goto
_LL319; _LL319: _temp318=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp308)->f2;
goto _LL313;} else{ goto _LL314;} _LL314: if(*(( int*) _temp308) == Cyc_Tcenv_Hidden){
_LL325: _temp324=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp308)->f1; goto
_LL323; _LL323: _temp322=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp308)->f2;
goto _LL315;} else{ goto _LL309;} _LL311: return( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp326=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp326[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp327; _temp327.tag= Cyc_Tcenv_Outermost;
_temp327.f1=( void*) x; _temp327;}); _temp326;}); _LL313: return( void*)({
struct Cyc_Tcenv_Frame_struct* _temp328=( struct Cyc_Tcenv_Frame_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Frame_struct)); _temp328[ 0]=({ struct Cyc_Tcenv_Frame_struct
_temp329; _temp329.tag= Cyc_Tcenv_Frame; _temp329.f1=( void*) x; _temp329.f2=(
void*) _temp318; _temp329;}); _temp328;}); _LL315: return( void*)({ struct Cyc_Tcenv_Hidden_struct*
_temp330=( struct Cyc_Tcenv_Hidden_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Hidden_struct));
_temp330[ 0]=({ struct Cyc_Tcenv_Hidden_struct _temp331; _temp331.tag= Cyc_Tcenv_Hidden;
_temp331.f1=( void*) _temp324; _temp331.f2=( void*) Cyc_Tcenv_put_visible(
_temp322, x); _temp331;}); _temp330;}); _LL309:;} void* Cyc_Tcenv_apply_to_visible(
void*(* g)( void*), void* f){ return Cyc_Tcenv_put_visible( f, g( Cyc_Tcenv_get_visible(
f)));} void* Cyc_Tcenv_map_frames( void*(* g)( void*), void* f){ void* _temp332=
f; void* _temp340; void* _temp342; void* _temp344; void* _temp346; void*
_temp348; _LL334: if(*(( int*) _temp332) == Cyc_Tcenv_Outermost){ _LL341:
_temp340=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp332)->f1; goto
_LL335;} else{ goto _LL336;} _LL336: if(*(( int*) _temp332) == Cyc_Tcenv_Frame){
_LL345: _temp344=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp332)->f1; goto
_LL343; _LL343: _temp342=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp332)->f2;
goto _LL337;} else{ goto _LL338;} _LL338: if(*(( int*) _temp332) == Cyc_Tcenv_Hidden){
_LL349: _temp348=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp332)->f1; goto
_LL347; _LL347: _temp346=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp332)->f2;
goto _LL339;} else{ goto _LL333;} _LL335: return( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp350=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp350[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp351; _temp351.tag= Cyc_Tcenv_Outermost;
_temp351.f1=( void*) g( _temp340); _temp351;}); _temp350;}); _LL337: return(
void*)({ struct Cyc_Tcenv_Frame_struct* _temp352=( struct Cyc_Tcenv_Frame_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Frame_struct)); _temp352[ 0]=({ struct Cyc_Tcenv_Frame_struct
_temp353; _temp353.tag= Cyc_Tcenv_Frame; _temp353.f1=( void*) g( _temp344);
_temp353.f2=( void*) Cyc_Tcenv_map_frames( g, _temp342); _temp353;}); _temp352;});
_LL339: return( void*)({ struct Cyc_Tcenv_Hidden_struct* _temp354=( struct Cyc_Tcenv_Hidden_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Hidden_struct)); _temp354[ 0]=({ struct Cyc_Tcenv_Hidden_struct
_temp355; _temp355.tag= Cyc_Tcenv_Hidden; _temp355.f1=( void*) g( _temp348);
_temp355.f2=( void*) Cyc_Tcenv_map_frames( g, _temp346); _temp355;}); _temp354;});
_LL333:;} struct Cyc_Tcenv_Fenv* Cyc_Tcenv_get_fenv( struct Cyc_Tcenv_Tenv* te){
if( te->le == 0){( int) _throw(( void*) Cyc_Tcenv_Env_error);} return(( struct
Cyc_Tcenv_Fenv*(*)( void* f)) Cyc_Tcenv_get_visible)(( void*)(( struct Cyc_Core_Opt*)
_check_null( te->le))->v);} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_put_fenv( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Fenv* fe){ if( te->le == 0){( int) _throw((
void*) Cyc_Tcenv_Env_error);} return({ struct Cyc_Tcenv_Tenv* _temp356=( struct
Cyc_Tcenv_Tenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Tenv)); _temp356->ns= te->ns;
_temp356->ae= te->ae; _temp356->le=({ struct Cyc_Core_Opt* _temp357=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp357->v=( void*)(( void*(*)( void*
f, struct Cyc_Tcenv_Fenv* x)) Cyc_Tcenv_put_visible)(( void*)(( struct Cyc_Core_Opt*)
_check_null( te->le))->v, fe); _temp357;}); _temp356;});} static int Cyc_Tcenv_region_ancestor(
struct Cyc_List_List* ancestors, struct Cyc_Absyn_Tvar* r){{ struct Cyc_List_List*
a= ancestors; for( 0; a != 0; a=(( struct Cyc_List_List*) _check_null( a))->tl){
if( Cyc_Absyn_tvar_cmp((( struct Cyc_Tcenv_RgnPO*)(( struct Cyc_List_List*)
_check_null( a))->hd)->tvar, r) == 0){ return 1;}}}{ struct Cyc_List_List* a=
ancestors; for( 0; a != 0; a=(( struct Cyc_List_List*) _check_null( a))->tl){
if( Cyc_Tcenv_region_ancestor((( struct Cyc_Tcenv_RgnPO*)(( struct Cyc_List_List*)
_check_null( a))->hd)->ancestors, r)){ return 1;}}} return 0;} static struct Cyc_Tcenv_RgnPO*
Cyc_Tcenv_find_region_in_po( struct Cyc_List_List* po, struct Cyc_Absyn_Tvar* r){
for( 0; po != 0; po=(( struct Cyc_List_List*) _check_null( po))->tl){ if( Cyc_Absyn_tvar_cmp(((
struct Cyc_Tcenv_RgnPO*)(( struct Cyc_List_List*) _check_null( po))->hd)->tvar,
r) == 0){ return( struct Cyc_Tcenv_RgnPO*)(( struct Cyc_List_List*) _check_null(
po))->hd;}} return(( struct Cyc_Tcenv_RgnPO*(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)((
struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp359; _temp359.tag=
Cyc_Stdio_String_pa; _temp359.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Absyn_VarType_struct* _temp360=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp360[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp361; _temp361.tag= Cyc_Absyn_VarType; _temp361.f1= r; _temp361;}); _temp360;}));{
void* _temp358[ 1u]={& _temp359}; Cyc_Stdio_aprintf( _tag_arr("can't find region variable %s in po",
sizeof( unsigned char), 36u), _tag_arr( _temp358, sizeof( void*), 1u));}}));}
static int Cyc_Tcenv_check_region_outlives( struct Cyc_List_List* po, struct Cyc_Absyn_Tvar*
r1, struct Cyc_Absyn_Tvar* r2){ struct Cyc_List_List* r2_ancestors; while( po !=
0? Cyc_Absyn_tvar_cmp((( struct Cyc_Tcenv_RgnPO*)(( struct Cyc_List_List*)
_check_null( po))->hd)->tvar, r2) != 0: 0) { po=(( struct Cyc_List_List*)
_check_null( po))->tl;} if( po == 0){ return 0;} return Cyc_Tcenv_region_ancestor(((
struct Cyc_Tcenv_RgnPO*)(( struct Cyc_List_List*) _check_null( po))->hd)->ancestors,
r1);} struct _tuple5{ void* f1; void* f2; } ; static struct Cyc_List_List* Cyc_Tcenv_initial_region_po(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* tvs, struct Cyc_List_List*
po){ struct Cyc_List_List* result= 0; for( 0; tvs != 0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){ struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd; if( Cyc_Tcutil_tvar_kind( tv) ==(
void*) Cyc_Absyn_RgnKind){ result=({ struct Cyc_List_List* _temp362=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp362->hd=( void*)({ struct Cyc_Tcenv_RgnPO*
_temp363=( struct Cyc_Tcenv_RgnPO*) GC_malloc( sizeof( struct Cyc_Tcenv_RgnPO));
_temp363->tvar= tv; _temp363->ancestors= 0; _temp363;}); _temp362->tl= result;
_temp362;});}} for( 0; po != 0; po=(( struct Cyc_List_List*) _check_null( po))->tl){
struct _tuple5 _temp364=*(( struct _tuple5*)(( struct Cyc_List_List*)
_check_null( po))->hd); void* _temp372; void* _temp374; struct Cyc_Absyn_Tvar*
_temp376; void* _temp378; struct Cyc_Absyn_Tvar* _temp380; void* _temp382; void*
_temp384; _LL366: _LL373: _temp372= _temp364.f1; if( _temp372 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL367;} else{ goto _LL368;} _LL368: _LL379: _temp378= _temp364.f1; if((
unsigned int) _temp378 > 4u?*(( int*) _temp378) == Cyc_Absyn_VarType: 0){ _LL381:
_temp380=(( struct Cyc_Absyn_VarType_struct*) _temp378)->f1; goto _LL375;} else{
goto _LL370;} _LL375: _temp374= _temp364.f2; if(( unsigned int) _temp374 > 4u?*((
int*) _temp374) == Cyc_Absyn_VarType: 0){ _LL377: _temp376=(( struct Cyc_Absyn_VarType_struct*)
_temp374)->f1; goto _LL369;} else{ goto _LL370;} _LL370: _LL385: _temp384=
_temp364.f1; goto _LL383; _LL383: _temp382= _temp364.f2; goto _LL371; _LL367:
goto _LL365; _LL369: if( Cyc_Absyn_tvar_cmp( _temp380, _temp376) == 0? 1: Cyc_Tcenv_check_region_outlives(
result, _temp376, _temp380)){({ struct Cyc_Stdio_String_pa_struct _temp388;
_temp388.tag= Cyc_Stdio_String_pa; _temp388.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Absyn_VarType_struct* _temp391=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp391[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp392; _temp392.tag= Cyc_Absyn_VarType; _temp392.f1= _temp376; _temp392;});
_temp391;}));{ struct Cyc_Stdio_String_pa_struct _temp387; _temp387.tag= Cyc_Stdio_String_pa;
_temp387.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp389=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp389[ 0]=({ struct Cyc_Absyn_VarType_struct _temp390; _temp390.tag= Cyc_Absyn_VarType;
_temp390.f1= _temp380; _temp390;}); _temp389;}));{ void* _temp386[ 2u]={&
_temp387,& _temp388}; Cyc_Tcutil_terr( loc, _tag_arr("bad region lifetime assertion (circularity):%s < %s",
sizeof( unsigned char), 52u), _tag_arr( _temp386, sizeof( void*), 2u));}}});}
else{ struct Cyc_Tcenv_RgnPO* _temp393= Cyc_Tcenv_find_region_in_po( result,
_temp380); struct Cyc_Tcenv_RgnPO* _temp394= Cyc_Tcenv_find_region_in_po( result,
_temp376); _temp394->ancestors=({ struct Cyc_List_List* _temp395=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp395->hd=( void*) _temp393;
_temp395->tl= _temp394->ancestors; _temp395;});} goto _LL365; _LL371:({ struct
Cyc_Stdio_String_pa_struct _temp398; _temp398.tag= Cyc_Stdio_String_pa; _temp398.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string( _temp382);{ struct Cyc_Stdio_String_pa_struct
_temp397; _temp397.tag= Cyc_Stdio_String_pa; _temp397.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( _temp384);{ void* _temp396[ 2u]={& _temp397,& _temp398};
Cyc_Tcutil_terr( loc, _tag_arr("bad region lifetime assertion:: %s < %s",
sizeof( unsigned char), 40u), _tag_arr( _temp396, sizeof( void*), 2u));}}});
goto _LL365; _LL365:;} return result;} static struct Cyc_List_List* Cyc_Tcenv_add_region_po(
struct Cyc_List_List* po, struct Cyc_Absyn_Tvar* tv){ return({ struct Cyc_List_List*
_temp399=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp399->hd=( void*)({ struct Cyc_Tcenv_RgnPO* _temp400=( struct Cyc_Tcenv_RgnPO*)
GC_malloc( sizeof( struct Cyc_Tcenv_RgnPO)); _temp400->tvar= tv; _temp400->ancestors=
po; _temp400;}); _temp399->tl= po; _temp399;});} static struct Cyc_Tcenv_Fenv*
Cyc_Tcenv_copy_fenv_old_ctrl( struct Cyc_Tcenv_Fenv* f){ return({ struct Cyc_Tcenv_Fenv*
_temp401=( struct Cyc_Tcenv_Fenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Fenv));
_temp401->shared= f->shared; _temp401->type_vars= f->type_vars; _temp401->region_order=
f->region_order; _temp401->locals= f->locals; _temp401->encloser= f->encloser;
_temp401->ctrl_env= f->ctrl_env; _temp401->capability=( void*)(( void*) f->capability);
_temp401->curr_rgn=( void*)(( void*) f->curr_rgn); _temp401;});} static struct
Cyc_Tcenv_Fenv* Cyc_Tcenv_copy_fenv_new_ctrl( struct Cyc_Tcenv_Fenv* f){ struct
Cyc_Tcenv_CtrlEnv* c= f->ctrl_env; return({ struct Cyc_Tcenv_Fenv* _temp402=(
struct Cyc_Tcenv_Fenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Fenv)); _temp402->shared=
f->shared; _temp402->type_vars= f->type_vars; _temp402->region_order= f->region_order;
_temp402->locals= f->locals; _temp402->encloser= f->encloser; _temp402->ctrl_env=({
struct Cyc_Tcenv_CtrlEnv* _temp403=( struct Cyc_Tcenv_CtrlEnv*) GC_malloc(
sizeof( struct Cyc_Tcenv_CtrlEnv)); _temp403->continue_stmt=( void*)(( void*) c->continue_stmt);
_temp403->break_stmt=( void*)(( void*) c->break_stmt); _temp403->fallthru_clause=
c->fallthru_clause; _temp403->next_stmt=( void*)(( void*) c->next_stmt);
_temp403->try_depth= c->try_depth; _temp403;}); _temp402->capability=( void*)((
void*) f->capability); _temp402->curr_rgn=( void*)(( void*) f->curr_rgn);
_temp402;});} void* Cyc_Tcenv_return_typ( struct Cyc_Tcenv_Tenv* te){ if( te->le
== 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected return_typ",
sizeof( unsigned char), 29u));} return( void*)(( Cyc_Tcenv_get_fenv( te))->shared)->return_typ;}
struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars( struct Cyc_Tcenv_Tenv* te){
if( te->le == 0){ return 0;} return( Cyc_Tcenv_get_fenv( te))->type_vars;}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment* loc,
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* tvs){ if( te->le == 0){(( int(*)(
struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected add_type_vars",
sizeof( unsigned char), 32u));}{ struct Cyc_Tcenv_Fenv* _temp404= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); Cyc_Tcutil_add_tvar_identities( tvs);{ struct Cyc_List_List*
_temp405=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( tvs, _temp404->type_vars); Cyc_Tcutil_check_unique_tvars(
loc, _temp405); _temp404->type_vars= _temp405; return Cyc_Tcenv_put_fenv( te,
_temp404);}}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Vardecl* vd){ if( te->le == 0){((
int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected add_local_var",
sizeof( unsigned char), 32u));}{ struct Cyc_Tcenv_Fenv* _temp406= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); struct _tagged_arr* _temp407=(* vd->name).f2;{ void*
_temp408=(* vd->name).f1; _LL410: if( _temp408 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL411;} else{ goto _LL412;} _LL412: goto _LL413; _LL411: goto _LL409; _LL413:(
int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp414=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp414[ 0]=({ struct
Cyc_Core_Impossible_struct _temp415; _temp415.tag= Cyc_Core_Impossible; _temp415.f1=
_tag_arr("add_local_var: called with Rel_n", sizeof( unsigned char), 33u);
_temp415;}); _temp414;})); goto _LL409; _LL409:;} _temp406->locals=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, void* data)) Cyc_Dict_insert)(
_temp406->locals, _temp407,( void*)({ struct Cyc_Absyn_Local_b_struct* _temp416=(
struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp416[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp417; _temp417.tag= Cyc_Absyn_Local_b;
_temp417.f1= vd; _temp417;}); _temp416;})); return Cyc_Tcenv_put_fenv( te,
_temp406);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_pat_var( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Vardecl* vd){ if( te->le == 0){((
int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected add_pat_var",
sizeof( unsigned char), 30u));}{ struct Cyc_Tcenv_Fenv* _temp418= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); struct _tagged_arr* _temp419=(* vd->name).f2; _temp418->locals=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key, void*
data)) Cyc_Dict_insert)( _temp418->locals, _temp419,( void*)({ struct Cyc_Absyn_Pat_b_struct*
_temp420=( struct Cyc_Absyn_Pat_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pat_b_struct));
_temp420[ 0]=({ struct Cyc_Absyn_Pat_b_struct _temp421; _temp421.tag= Cyc_Absyn_Pat_b;
_temp421.f1= vd; _temp421;}); _temp420;})); return Cyc_Tcenv_put_fenv( te,
_temp418);}} void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp424; struct
_tagged_arr* _temp425; void* _temp427; struct _tuple0* _temp422= q; _temp424=*
_temp422; _LL428: _temp427= _temp424.f1; goto _LL426; _LL426: _temp425= _temp424.f2;
goto _LL423; _LL423: { void* _temp429= _temp427; struct Cyc_List_List* _temp438;
_LL431: if( _temp429 ==( void*) Cyc_Absyn_Loc_n){ goto _LL432;} else{ goto
_LL433;} _LL433: if(( unsigned int) _temp429 > 1u?*(( int*) _temp429) == Cyc_Absyn_Rel_n:
0){ _LL439: _temp438=(( struct Cyc_Absyn_Rel_n_struct*) _temp429)->f1; if(
_temp438 == 0){ goto _LL437;} else{ goto _LL435;}} else{ goto _LL435;} _LL437:
if( te->le != 0){ goto _LL434;} else{ goto _LL435;} _LL435: goto _LL436; _LL432:
if( te->le == 0){( int) _throw(( void*) Cyc_Dict_Absent);} else{ goto _LL434;}
_LL434: { struct Cyc_Tcenv_Fenv* _temp440= Cyc_Tcenv_get_fenv( te); struct
_handler_cons _temp441; _push_handler(& _temp441);{ int _temp443= 0; if( setjmp(
_temp441.handler)){ _temp443= 1;} if( ! _temp443){{ void* _temp446=( void*)({
struct Cyc_Tcenv_VarRes_struct* _temp444=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp444[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp445; _temp445.tag= Cyc_Tcenv_VarRes; _temp445.f1=( void*)(( void*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp440->locals,
_temp425); _temp445;}); _temp444;}); _npop_handler( 0u); return _temp446;};
_pop_handler();} else{ void* _temp442=( void*) _exn_thrown; void* _temp448=
_temp442; _LL450: if( _temp448 == Cyc_Dict_Absent){ goto _LL451;} else{ goto
_LL452;} _LL452: goto _LL453; _LL451: return Cyc_Tcenv_lookup_ordinary_global(
te, loc, q); _LL453:( void) _throw( _temp448); _LL449:;}}} _LL436: { struct
_handler_cons _temp454; _push_handler(& _temp454);{ int _temp456= 0; if( setjmp(
_temp454.handler)){ _temp456= 1;} if( ! _temp456){{ void* _temp457= Cyc_Tcenv_lookup_ordinary_global(
te, loc, q); _npop_handler( 0u); return _temp457;}; _pop_handler();} else{ void*
_temp455=( void*) _exn_thrown; void* _temp459= _temp455; _LL461: if( _temp459 ==
Cyc_Dict_Absent){ goto _LL462;} else{ goto _LL463;} _LL463: goto _LL464; _LL462:({
struct Cyc_Stdio_String_pa_struct _temp466; _temp466.tag= Cyc_Stdio_String_pa;
_temp466.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( q);{ void* _temp465[
1u]={& _temp466}; Cyc_Tcutil_terr( loc, _tag_arr("unbound identifier: %s",
sizeof( unsigned char), 23u), _tag_arr( _temp465, sizeof( void*), 1u));}});
return( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp467=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp467[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp468; _temp468.tag= Cyc_Tcenv_VarRes; _temp468.f1=( void*)(( void*) Cyc_Absyn_Unresolved_b);
_temp468;}); _temp467;}); _LL464:( void) _throw( _temp459); _LL460:;}}} _LL430:;}}
void Cyc_Tcenv_process_continue( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt*
s, struct Cyc_Absyn_Stmt** sopt){ if( te->le == 0){(( int(*)( struct _tagged_arr
msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected process_continue", sizeof(
unsigned char), 35u));}{ void* _temp469=( void*)(( Cyc_Tcenv_get_fenv( te))->ctrl_env)->continue_stmt;
struct Cyc_Absyn_Stmt* _temp479; _LL471: if(( unsigned int) _temp469 > 3u?*((
int*) _temp469) == Cyc_Tcenv_Stmt_j: 0){ _LL480: _temp479=(( struct Cyc_Tcenv_Stmt_j_struct*)
_temp469)->f1; goto _LL472;} else{ goto _LL473;} _LL473: if( _temp469 ==( void*)
Cyc_Tcenv_NotLoop_j){ goto _LL474;} else{ goto _LL475;} _LL475: if( _temp469 ==(
void*) Cyc_Tcenv_CaseEnd_j){ goto _LL476;} else{ goto _LL477;} _LL477: if(
_temp469 ==( void*) Cyc_Tcenv_FnEnd_j){ goto _LL478;} else{ goto _LL470;} _LL472:
_temp479->non_local_preds=({ struct Cyc_List_List* _temp481=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp481->hd=( void*) s; _temp481->tl=
_temp479->non_local_preds; _temp481;});* sopt=( struct Cyc_Absyn_Stmt*) _temp479;
return; _LL474:({ void* _temp482[ 0u]={}; Cyc_Tcutil_terr( s->loc, _tag_arr("continue not in a loop",
sizeof( unsigned char), 23u), _tag_arr( _temp482, sizeof( void*), 0u));});
return; _LL476:(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: continue dest is CaseEnd",
sizeof( unsigned char), 32u)); return; _LL478:(( int(*)( struct _tagged_arr msg))
Cyc_Tcenv_env_err)( _tag_arr("Tcenv: continue dest is CaseEnd", sizeof(
unsigned char), 32u)); return; _LL470:;}} void Cyc_Tcenv_process_break( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Stmt** sopt){ if(
te->le == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected process_break",
sizeof( unsigned char), 32u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_get_fenv(
te); void* _temp483=( void*)( fe->ctrl_env)->break_stmt; struct Cyc_Absyn_Stmt*
_temp493; _LL485: if(( unsigned int) _temp483 > 3u?*(( int*) _temp483) == Cyc_Tcenv_Stmt_j:
0){ _LL494: _temp493=(( struct Cyc_Tcenv_Stmt_j_struct*) _temp483)->f1; goto
_LL486;} else{ goto _LL487;} _LL487: if( _temp483 ==( void*) Cyc_Tcenv_NotLoop_j){
goto _LL488;} else{ goto _LL489;} _LL489: if( _temp483 ==( void*) Cyc_Tcenv_FnEnd_j){
goto _LL490;} else{ goto _LL491;} _LL491: if( _temp483 ==( void*) Cyc_Tcenv_CaseEnd_j){
goto _LL492;} else{ goto _LL484;} _LL486: _temp493->non_local_preds=({ struct
Cyc_List_List* _temp495=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp495->hd=( void*) s; _temp495->tl= _temp493->non_local_preds; _temp495;});*
sopt=( struct Cyc_Absyn_Stmt*) _temp493; return; _LL488:({ void* _temp496[ 0u]={};
Cyc_Tcutil_terr( s->loc, _tag_arr("break not in a loop or switch", sizeof(
unsigned char), 30u), _tag_arr( _temp496, sizeof( void*), 0u));}); return;
_LL490: if(( void*)( fe->shared)->return_typ !=( void*) Cyc_Absyn_VoidType){({
void* _temp497[ 0u]={}; Cyc_Tcutil_terr( s->loc, _tag_arr("break causes function not to return a value",
sizeof( unsigned char), 44u), _tag_arr( _temp497, sizeof( void*), 0u));});}
return; _LL492:({ void* _temp498[ 0u]={}; Cyc_Tcutil_terr( s->loc, _tag_arr("break causes outer switch clause to implicitly fallthru",
sizeof( unsigned char), 56u), _tag_arr( _temp498, sizeof( void*), 0u));});
return; _LL484:;}} void Cyc_Tcenv_process_goto( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Stmt* s, struct _tagged_arr* l, struct Cyc_Absyn_Stmt** sopt){
if( te->le == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected process_goto", sizeof( unsigned char), 31u));}{
struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_get_fenv( te); struct Cyc_Core_Opt*
_temp499=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
key)) Cyc_Dict_lookup_opt)(( fe->shared)->seen_labels, l); if( _temp499 == 0){
struct Cyc_SlowDict_Dict* _temp500=( fe->shared)->needed_labels; struct Cyc_Core_Opt*
_temp501=(( struct Cyc_Core_Opt*(*)( struct Cyc_SlowDict_Dict* d, struct
_tagged_arr* key)) Cyc_SlowDict_lookup_opt)( _temp500, l); if( _temp501 == 0){
_temp501=({ struct Cyc_Core_Opt* _temp502=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp502->v=( void*) 0; _temp502;});}( fe->shared)->needed_labels=((
struct Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict* d, struct _tagged_arr*
key, struct Cyc_List_List* data)) Cyc_SlowDict_insert)( _temp500, l,({ struct
Cyc_List_List* _temp503=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp503->hd=( void*) s; _temp503->tl=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp501))->v; _temp503;}));} else{(( struct Cyc_Absyn_Stmt*)((
struct Cyc_Core_Opt*) _check_null( _temp499))->v)->non_local_preds=({ struct Cyc_List_List*
_temp504=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp504->hd=( void*) s; _temp504->tl=(( struct Cyc_Absyn_Stmt*)(( struct Cyc_Core_Opt*)
_check_null( _temp499))->v)->non_local_preds; _temp504;});* sopt=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Stmt*)(( struct Cyc_Core_Opt*) _check_null( _temp499))->v);}}}
struct _tuple3* Cyc_Tcenv_process_fallthru( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Switch_clause*** clauseopt){ if( te->le == 0){((
int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected process_fallthru",
sizeof( unsigned char), 35u));}{ struct Cyc_Tcenv_Fenv* _temp505= Cyc_Tcenv_get_fenv(
te); struct _tuple3* _temp506=( _temp505->ctrl_env)->fallthru_clause; if(
_temp506 != 0){(((*(( struct _tuple3*) _check_null( _temp506))).f1)->body)->non_local_preds=({
struct Cyc_List_List* _temp507=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp507->hd=( void*) s; _temp507->tl=(((*(( struct
_tuple3*) _check_null( _temp506))).f1)->body)->non_local_preds; _temp507;});*
clauseopt=({ struct Cyc_Absyn_Switch_clause** _temp508=( struct Cyc_Absyn_Switch_clause**)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause*) * 1); _temp508[ 0]=(*((
struct _tuple3*) _check_null( _temp506))).f1; _temp508;});} return _temp506;}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_fallthru( struct Cyc_Tcenv_Tenv* te, struct
Cyc_List_List* new_tvs, struct Cyc_List_List* vds, struct Cyc_Absyn_Switch_clause*
clause){ if( te->le == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected set_fallthru", sizeof( unsigned char), 31u));}{
struct Cyc_List_List* _temp509= 0; for( 0; vds != 0; vds=(( struct Cyc_List_List*)
_check_null( vds))->tl){ _temp509=({ struct Cyc_List_List* _temp510=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp510->hd=( void*)(( void*)((
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( vds))->hd)->type);
_temp510->tl= _temp509; _temp510;});} _temp509= Cyc_List_imp_rev( _temp509);{
struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));(
fe->ctrl_env)->fallthru_clause=({ struct _tuple3* _temp511=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp511->f1= clause; _temp511->f2= new_tvs;
_temp511->f3= _temp509; _temp511;}); return Cyc_Tcenv_put_fenv( te, fe);}}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_clear_fallthru( struct Cyc_Tcenv_Tenv* te){ if(
te->le == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected clear_fallthru",
sizeof( unsigned char), 33u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te));( fe->ctrl_env)->fallthru_clause= 0; return Cyc_Tcenv_put_fenv(
te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_loop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* continue_dest){ if( te->le == 0){(( int(*)( struct
_tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected set_in_loop",
sizeof( unsigned char), 30u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->continue_stmt=( void*)(( void*)({
struct Cyc_Tcenv_Stmt_j_struct* _temp512=( struct Cyc_Tcenv_Stmt_j_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp512[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct
_temp513; _temp513.tag= Cyc_Tcenv_Stmt_j; _temp513.f1= continue_dest; _temp513;});
_temp512;})));( void*)(( fe->ctrl_env)->break_stmt=( void*)(( void*)( fe->ctrl_env)->next_stmt));(
void*)(( fe->ctrl_env)->next_stmt=( void*)(( void*)({ struct Cyc_Tcenv_Stmt_j_struct*
_temp514=( struct Cyc_Tcenv_Stmt_j_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct));
_temp514[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct _temp515; _temp515.tag= Cyc_Tcenv_Stmt_j;
_temp515.f1= continue_dest; _temp515;}); _temp514;}))); return Cyc_Tcenv_put_fenv(
te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_enter_try( struct Cyc_Tcenv_Tenv* te){
if( te->le == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected enter_try", sizeof( unsigned char), 28u));}{ struct
Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te)); ++(
fe->ctrl_env)->try_depth; return Cyc_Tcenv_put_fenv( te, fe);}} int Cyc_Tcenv_get_try_depth(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( int(*)( struct _tagged_arr msg))
Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected get_try_depth", sizeof(
unsigned char), 32u));} return(( Cyc_Tcenv_get_fenv( te))->ctrl_env)->try_depth;}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_switch( struct Cyc_Tcenv_Tenv* te){ if(
te->le == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected set_in_switch",
sizeof( unsigned char), 32u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->break_stmt=( void*)(( void*)(
fe->ctrl_env)->next_stmt));( void*)(( fe->ctrl_env)->next_stmt=( void*)(( void*)
Cyc_Tcenv_CaseEnd_j)); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_next( struct Cyc_Tcenv_Tenv* te, void* j){ if( te->le == 0){(( int(*)(
struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected set_next",
sizeof( unsigned char), 27u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->next_stmt=( void*) j); return
Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_label(
struct Cyc_Tcenv_Tenv* te, struct _tagged_arr* v, struct Cyc_Absyn_Stmt* s){ if(
te->le == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected add_label",
sizeof( unsigned char), 28u));}{ struct Cyc_Tcenv_Fenv* _temp516= Cyc_Tcenv_get_fenv(
te); struct Cyc_SlowDict_Dict* _temp517=( _temp516->shared)->needed_labels;
struct Cyc_Core_Opt* _temp518=(( struct Cyc_Core_Opt*(*)( struct Cyc_SlowDict_Dict*
d, struct _tagged_arr* key)) Cyc_SlowDict_lookup_opt)( _temp517, v); if(
_temp518 != 0){( _temp516->shared)->needed_labels=(( struct Cyc_SlowDict_Dict*(*)(
struct Cyc_SlowDict_Dict* d, struct _tagged_arr* key)) Cyc_SlowDict_delete)(
_temp517, v);{ struct Cyc_List_List* _temp519=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( _temp518))->v; s->non_local_preds= _temp519; for( 0;
_temp519 != 0; _temp519=(( struct Cyc_List_List*) _check_null( _temp519))->tl){
void* _temp520=( void*)(( struct Cyc_Absyn_Stmt*)(( struct Cyc_List_List*)
_check_null( _temp519))->hd)->r; struct Cyc_Absyn_Stmt* _temp526; struct Cyc_Absyn_Stmt**
_temp528; _LL522: if(( unsigned int) _temp520 > 1u?*(( int*) _temp520) == Cyc_Absyn_Goto_s:
0){ _LL527: _temp526=(( struct Cyc_Absyn_Goto_s_struct*) _temp520)->f2; _temp528=&((
struct Cyc_Absyn_Goto_s_struct*) _temp520)->f2; goto _LL523;} else{ goto _LL524;}
_LL524: goto _LL525; _LL523:* _temp528=( struct Cyc_Absyn_Stmt*) s; goto _LL521;
_LL525:(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: add_label backpatching of non-goto",
sizeof( unsigned char), 42u)); goto _LL521; _LL521:;}}} if((( int(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key)) Cyc_Dict_member)(( _temp516->shared)->seen_labels,
v)){({ struct Cyc_Stdio_String_pa_struct _temp530; _temp530.tag= Cyc_Stdio_String_pa;
_temp530.f1=( struct _tagged_arr)* v;{ void* _temp529[ 1u]={& _temp530}; Cyc_Tcutil_terr(
s->loc, _tag_arr("Repeated label: %s", sizeof( unsigned char), 19u), _tag_arr(
_temp529, sizeof( void*), 1u));}});}( _temp516->shared)->seen_labels=(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Stmt*
data)) Cyc_Dict_insert)(( _temp516->shared)->seen_labels, v, s); return te;}}
int Cyc_Tcenv_all_labels_resolved( struct Cyc_Tcenv_Tenv* te){ return(( int(*)(
struct Cyc_SlowDict_Dict* d)) Cyc_SlowDict_is_empty)((( Cyc_Tcenv_get_fenv( te))->shared)->needed_labels);}
struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser( struct Cyc_Tcenv_Tenv* te){ if(
te->le == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected get_encloser",
sizeof( unsigned char), 31u));} return( Cyc_Tcenv_get_fenv( te))->encloser;}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Stmt* s){ if( te->le == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected set_encloser", sizeof( unsigned char), 31u));}{
struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl( Cyc_Tcenv_get_fenv( te));
fe->encloser= s; return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_region( struct Cyc_Tcenv_Tenv* te, void* r){ if( te->le == 0){((
int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv::add_region: unexpected add_region",
sizeof( unsigned char), 41u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); struct Cyc_Absyn_Tvar* tv;{ void* _temp531= Cyc_Tcutil_compress(
r); struct Cyc_Absyn_Tvar* _temp537; _LL533: if(( unsigned int) _temp531 > 4u?*((
int*) _temp531) == Cyc_Absyn_VarType: 0){ _LL538: _temp537=(( struct Cyc_Absyn_VarType_struct*)
_temp531)->f1; goto _LL534;} else{ goto _LL535;} _LL535: goto _LL536; _LL534: tv=
_temp537; goto _LL532; _LL536: tv=(( struct Cyc_Absyn_Tvar*(*)( struct
_tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("bad add region", sizeof(
unsigned char), 15u)); goto _LL532; _LL532:;} fe->region_order= Cyc_Tcenv_add_region_po(
fe->region_order, tv);( void*)( fe->capability=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp539=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp539[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp540; _temp540.tag= Cyc_Absyn_JoinEff;
_temp540.f1=({ struct Cyc_List_List* _temp541=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp541->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp543=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp543[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp544; _temp544.tag= Cyc_Absyn_AccessEff;
_temp544.f1=( void*) r; _temp544;}); _temp543;})); _temp541->tl=({ struct Cyc_List_List*
_temp542=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp542->hd=( void*)(( void*) fe->capability); _temp542->tl= 0; _temp542;});
_temp541;}); _temp540;}); _temp539;}))); return Cyc_Tcenv_put_fenv( te, fe);}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_named_block( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Tvar* block_rgn){ if( te->le ==
0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected new_block",
sizeof( unsigned char), 28u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); void* block_typ=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp552=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp552[ 0]=({ struct Cyc_Absyn_VarType_struct _temp553; _temp553.tag= Cyc_Absyn_VarType;
_temp553.f1= block_rgn; _temp553;}); _temp552;}); fe->type_vars=({ struct Cyc_List_List*
_temp545=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp545->hd=( void*) block_rgn; _temp545->tl= fe->type_vars; _temp545;}); Cyc_Tcutil_check_unique_tvars(
loc, fe->type_vars); fe->region_order= Cyc_Tcenv_add_region_po( fe->region_order,
block_rgn);( void*)( fe->capability=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp546=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp546[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp547; _temp547.tag= Cyc_Absyn_JoinEff;
_temp547.f1=({ struct Cyc_List_List* _temp548=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp548->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp550=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp550[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp551; _temp551.tag= Cyc_Absyn_AccessEff;
_temp551.f1=( void*) block_typ; _temp551;}); _temp550;})); _temp548->tl=({
struct Cyc_List_List* _temp549=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp549->hd=( void*)(( void*) fe->capability); _temp549->tl=
0; _temp549;}); _temp548;}); _temp547;}); _temp546;})));( void*)( fe->curr_rgn=(
void*) block_typ); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_block( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te){
struct Cyc_Absyn_Tvar* t= Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_RgnKind); Cyc_Tcutil_add_tvar_identity(
t); return Cyc_Tcenv_new_named_block( loc, te, t);} void* Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){ return( void*) Cyc_Absyn_HeapRgn;}
return( void*)( Cyc_Tcenv_get_fenv( te))->curr_rgn;} void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* rgn){ if( te->le
!= 0){ void* capability=( void*)( Cyc_Tcenv_get_fenv( te))->capability; if( Cyc_Tcutil_region_in_effect(
0, rgn, capability)? 1: Cyc_Tcutil_region_in_effect( 1, rgn, capability)){
return;}}({ struct Cyc_Stdio_String_pa_struct _temp555; _temp555.tag= Cyc_Stdio_String_pa;
_temp555.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( rgn);{ void* _temp554[
1u]={& _temp555}; Cyc_Tcutil_terr( loc, _tag_arr("Expression accesses unavailable region %s",
sizeof( unsigned char), 42u), _tag_arr( _temp554, sizeof( void*), 1u));}});} int
Cyc_Tcenv_region_outlives( struct Cyc_Tcenv_Tenv* te, void* rt_a, void* rt_b){
if( rt_a ==( void*) Cyc_Absyn_HeapRgn){ return 1;} if( te->le != 0){ struct
_tuple5 _temp557=({ struct _tuple5 _temp556; _temp556.f1= Cyc_Tcutil_compress(
rt_a); _temp556.f2= Cyc_Tcutil_compress( rt_b); _temp556;}); void* _temp563;
struct Cyc_Absyn_Tvar* _temp565; void* _temp567; struct Cyc_Absyn_Tvar* _temp569;
_LL559: _LL568: _temp567= _temp557.f1; if(( unsigned int) _temp567 > 4u?*(( int*)
_temp567) == Cyc_Absyn_VarType: 0){ _LL570: _temp569=(( struct Cyc_Absyn_VarType_struct*)
_temp567)->f1; goto _LL564;} else{ goto _LL561;} _LL564: _temp563= _temp557.f2;
if(( unsigned int) _temp563 > 4u?*(( int*) _temp563) == Cyc_Absyn_VarType: 0){
_LL566: _temp565=(( struct Cyc_Absyn_VarType_struct*) _temp563)->f1; goto _LL560;}
else{ goto _LL561;} _LL561: goto _LL562; _LL560: return Cyc_Tcenv_check_region_outlives((
Cyc_Tcenv_get_fenv( te))->region_order, _temp569, _temp565); _LL562: return 0;
_LL558:;} return 0;} void Cyc_Tcenv_check_effect_accessible( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void* eff){ void* capability; if( te->le
== 0){ capability=( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp571=( struct
Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp571[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp572; _temp572.tag= Cyc_Absyn_JoinEff;
_temp572.f1= 0; _temp572;}); _temp571;});} else{ struct Cyc_Tcenv_Fenv* _temp573=
Cyc_Tcenv_get_fenv( te); capability=( void*) _temp573->capability;} if( ! Cyc_Tcutil_subset_effect(
1, eff, capability)){({ struct Cyc_Stdio_String_pa_struct _temp576; _temp576.tag=
Cyc_Stdio_String_pa; _temp576.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
eff);{ struct Cyc_Stdio_String_pa_struct _temp575; _temp575.tag= Cyc_Stdio_String_pa;
_temp575.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( capability);{ void*
_temp574[ 2u]={& _temp575,& _temp576}; Cyc_Tcutil_terr( loc, _tag_arr("Capability %s does not cover function's effect %s",
sizeof( unsigned char), 50u), _tag_arr( _temp574, sizeof( void*), 2u));}}});}}
void Cyc_Tcenv_check_rgn_partial_order( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* po){ for( 0; po != 0; po=(( struct Cyc_List_List*)
_check_null( po))->tl){ struct _tuple5 _temp579; void* _temp580; void* _temp582;
struct _tuple5* _temp577=( struct _tuple5*)(( struct Cyc_List_List*) _check_null(
po))->hd; _temp579=* _temp577; _LL583: _temp582= _temp579.f1; goto _LL581;
_LL581: _temp580= _temp579.f2; goto _LL578; _LL578: if( ! Cyc_Tcenv_region_outlives(
te, _temp582, _temp580)){({ struct Cyc_Stdio_String_pa_struct _temp586; _temp586.tag=
Cyc_Stdio_String_pa; _temp586.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp580);{ struct Cyc_Stdio_String_pa_struct _temp585; _temp585.tag= Cyc_Stdio_String_pa;
_temp585.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( _temp582);{ void*
_temp584[ 2u]={& _temp585,& _temp586}; Cyc_Tcutil_terr( loc, _tag_arr("function requires that region %s outlive %s",
sizeof( unsigned char), 44u), _tag_arr( _temp584, sizeof( void*), 2u));}}});}}}
struct _tuple6{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ;
struct Cyc_Tcenv_Fenv* Cyc_Tcenv_new_fenv( struct Cyc_Position_Segment* loc,
struct Cyc_Absyn_Fndecl* fd){ struct Cyc_Dict_Dict* locals=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
struct Cyc_Absyn_Tvar* rgn0=({ struct Cyc_Absyn_Tvar* _temp640=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp640->name=({ struct _tagged_arr*
_temp641=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr));
_temp641[ 0]=( struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp643;
_temp643.tag= Cyc_Stdio_String_pa; _temp643.f1=( struct _tagged_arr)*(* fd->name).f2;{
void* _temp642[ 1u]={& _temp643}; Cyc_Stdio_aprintf( _tag_arr("`%s", sizeof(
unsigned char), 4u), _tag_arr( _temp642, sizeof( void*), 1u));}}); _temp641;});
_temp640->identity= Cyc_Tcutil_new_tvar_id(); _temp640->kind= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_RgnKind); _temp640;}); struct Cyc_List_List* _temp587=({ struct
Cyc_List_List* _temp639=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp639->hd=( void*) rgn0; _temp639->tl= fd->tvs; _temp639;}); Cyc_Tcutil_check_unique_tvars(
loc, _temp587);{ struct Cyc_List_List* _temp588= Cyc_Tcenv_add_region_po( Cyc_Tcenv_initial_region_po(
loc, fd->tvs, fd->rgn_po), rgn0); void* param_rgn=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp637=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp637[ 0]=({ struct Cyc_Absyn_VarType_struct _temp638; _temp638.tag= Cyc_Absyn_VarType;
_temp638.f1= rgn0; _temp638;}); _temp637;}); struct Cyc_List_List* _temp589= 0;{
struct Cyc_List_List* _temp590= fd->args; for( 0; _temp590 != 0; _temp590=((
struct Cyc_List_List*) _check_null( _temp590))->tl){ struct Cyc_Absyn_Vardecl*
_temp591=({ struct Cyc_Absyn_Vardecl* _temp596=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl) * 1); _temp596[ 0]=({ struct Cyc_Absyn_Vardecl
_temp597; _temp597.sc=( void*)(( void*) Cyc_Absyn_Public); _temp597.name=({
struct _tuple0* _temp600=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0) *
1); _temp600[ 0]=({ struct _tuple0 _temp601; _temp601.f1=( void*) Cyc_Absyn_Loc_n;
_temp601.f2=(*(( struct _tuple6*)(( struct Cyc_List_List*) _check_null( _temp590))->hd)).f1;
_temp601;}); _temp600;}); _temp597.tq=(*(( struct _tuple6*)(( struct Cyc_List_List*)
_check_null( _temp590))->hd)).f2; _temp597.type=( void*)(*(( struct _tuple6*)((
struct Cyc_List_List*) _check_null( _temp590))->hd)).f3; _temp597.initializer= 0;
_temp597.rgn=({ struct Cyc_Core_Opt* _temp598=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt) * 1); _temp598[ 0]=({ struct Cyc_Core_Opt _temp599;
_temp599.v=( void*) param_rgn; _temp599;}); _temp598;}); _temp597.attributes= 0;
_temp597;}); _temp596;}); _temp589=({ struct Cyc_List_List* _temp592=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp592[ 0]=({
struct Cyc_List_List _temp593; _temp593.hd=( void*) _temp591; _temp593.tl=
_temp589; _temp593;}); _temp592;}); locals=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key, void* data)) Cyc_Dict_insert)( locals,(*((
struct _tuple6*)(( struct Cyc_List_List*) _check_null( _temp590))->hd)).f1,(
void*)({ struct Cyc_Absyn_Param_b_struct* _temp594=( struct Cyc_Absyn_Param_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Param_b_struct)); _temp594[ 0]=({ struct Cyc_Absyn_Param_b_struct
_temp595; _temp595.tag= Cyc_Absyn_Param_b; _temp595.f1= _temp591; _temp595;});
_temp594;}));}} if( fd->cyc_varargs != 0){ int _temp604; void* _temp606; void*
_temp608; struct Cyc_Absyn_Tqual _temp610; struct Cyc_Core_Opt* _temp612; struct
Cyc_Absyn_VarargInfo _temp602=*(( struct Cyc_Absyn_VarargInfo*) _check_null( fd->cyc_varargs));
_LL613: _temp612= _temp602.name; goto _LL611; _LL611: _temp610= _temp602.tq;
goto _LL609; _LL609: _temp608=( void*) _temp602.type; goto _LL607; _LL607:
_temp606=( void*) _temp602.rgn; goto _LL605; _LL605: _temp604= _temp602.inject;
goto _LL603; _LL603: if( _temp612 != 0){ void* _temp614= Cyc_Absyn_tagged_typ(
_temp608, _temp606, _temp610); struct Cyc_Absyn_Vardecl* _temp615=({ struct Cyc_Absyn_Vardecl*
_temp619=( struct Cyc_Absyn_Vardecl*) GC_malloc( sizeof( struct Cyc_Absyn_Vardecl)
* 1); _temp619[ 0]=({ struct Cyc_Absyn_Vardecl _temp620; _temp620.sc=( void*)((
void*) Cyc_Absyn_Public); _temp620.name=({ struct _tuple0* _temp623=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0) * 1); _temp623[ 0]=({ struct
_tuple0 _temp624; _temp624.f1=( void*) Cyc_Absyn_Loc_n; _temp624.f2=( struct
_tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( _temp612))->v; _temp624;});
_temp623;}); _temp620.tq= Cyc_Absyn_empty_tqual(); _temp620.type=( void*)
_temp614; _temp620.initializer= 0; _temp620.rgn=({ struct Cyc_Core_Opt* _temp621=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1); _temp621[ 0]=({
struct Cyc_Core_Opt _temp622; _temp622.v=( void*) param_rgn; _temp622;});
_temp621;}); _temp620.attributes= 0; _temp620;}); _temp619;}); _temp589=({
struct Cyc_List_List* _temp616=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp616->hd=( void*) _temp615; _temp616->tl= _temp589;
_temp616;}); locals=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key, void* data)) Cyc_Dict_insert)( locals,( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( _temp612))->v,( void*)({ struct Cyc_Absyn_Param_b_struct*
_temp617=( struct Cyc_Absyn_Param_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Param_b_struct));
_temp617[ 0]=({ struct Cyc_Absyn_Param_b_struct _temp618; _temp618.tag= Cyc_Absyn_Param_b;
_temp618.f1= _temp615; _temp618;}); _temp617;}));} else{({ void* _temp625[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("missing name for varargs", sizeof( unsigned char),
25u), _tag_arr( _temp625, sizeof( void*), 0u));});}} fd->param_vardecls=({
struct Cyc_Core_Opt* _temp626=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt) * 1); _temp626[ 0]=({ struct Cyc_Core_Opt _temp627; _temp627.v=(
void*) _temp589; _temp627;}); _temp626;}); return({ struct Cyc_Tcenv_Fenv*
_temp628=( struct Cyc_Tcenv_Fenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Fenv));
_temp628->shared=({ struct Cyc_Tcenv_SharedFenv* _temp636=( struct Cyc_Tcenv_SharedFenv*)
GC_malloc( sizeof( struct Cyc_Tcenv_SharedFenv)); _temp636->return_typ=( void*)((
void*) fd->ret_type); _temp636->seen_labels=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp636->needed_labels=(( struct Cyc_SlowDict_Dict*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_SlowDict_empty)( Cyc_String_zstrptrcmp);
_temp636;}); _temp628->type_vars= _temp587; _temp628->region_order= _temp588;
_temp628->locals= locals; _temp628->encloser= fd->body; _temp628->ctrl_env=({
struct Cyc_Tcenv_CtrlEnv* _temp635=( struct Cyc_Tcenv_CtrlEnv*) GC_malloc(
sizeof( struct Cyc_Tcenv_CtrlEnv)); _temp635->continue_stmt=( void*)(( void*)
Cyc_Tcenv_NotLoop_j); _temp635->break_stmt=( void*)(( void*) Cyc_Tcenv_NotLoop_j);
_temp635->fallthru_clause= 0; _temp635->next_stmt=( void*)(( void*) Cyc_Tcenv_FnEnd_j);
_temp635->try_depth= 0; _temp635;}); _temp628->capability=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp629=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp629[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp630; _temp630.tag= Cyc_Absyn_JoinEff; _temp630.f1=({ struct Cyc_List_List*
_temp631=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp631->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp633=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp633[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp634; _temp634.tag= Cyc_Absyn_AccessEff;
_temp634.f1=( void*) param_rgn; _temp634;}); _temp633;})); _temp631->tl=({
struct Cyc_List_List* _temp632=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp632->hd=( void*)(( void*)(( struct Cyc_Core_Opt*)
_check_null( fd->effect))->v); _temp632->tl= 0; _temp632;}); _temp631;});
_temp630;}); _temp629;})); _temp628->curr_rgn=( void*) param_rgn; _temp628;});}}
