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
struct _tagged_arr msg){({ int(* _temp0)( struct Cyc_Stdio___sFILE*, struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp4= Cyc_Stdio_stderr; struct _tagged_arr _temp5= _tag_arr("Error: %s\n",
sizeof( unsigned char), 11u); struct Cyc_Stdio_String_pa_struct _temp6; _temp6.tag=
Cyc_Stdio_String_pa; _temp6.f1=( struct _tagged_arr) msg;{ void* _temp3=( void*)&
_temp6; void* _temp1[ 1u]={ _temp3}; struct _tagged_arr _temp2={( void*) _temp1,(
void*) _temp1,( void*)( _temp1 + 1u)}; _temp0( _temp4, _temp5, _temp2);}}); Cyc_Stdio_fflush((
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
Cyc_Core_Opt* _temp7=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp7->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Set_empty)( Cyc_String_zstrptrcmp); _temp7;});{
struct Cyc_Tcenv_Genv* _temp8= Cyc_Tcenv_empty_genv(); struct Cyc_List_List*
_temp9= 0; _temp8->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)(
_temp8->tuniondecls,(* Cyc_Absyn_exn_name).f2,& Cyc_Absyn_exn_tud); _temp8->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct _tuple4* data)) Cyc_Dict_insert)( _temp8->ordinaries,(* Cyc_Absyn_null_pointer_exn_name).f2,({
struct _tuple4* _temp10=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp10->f1=( void*)({ struct Cyc_Tcenv_TunionRes_struct* _temp11=( struct Cyc_Tcenv_TunionRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct)); _temp11[ 0]=({ struct
Cyc_Tcenv_TunionRes_struct _temp12; _temp12.tag= Cyc_Tcenv_TunionRes; _temp12.f1=
Cyc_Absyn_exn_tud; _temp12.f2= Cyc_Absyn_null_pointer_exn_tuf; _temp12;});
_temp11;}); _temp10->f2= 1; _temp10;})); _temp8->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct _tuple4* data)) Cyc_Dict_insert)(
_temp8->ordinaries,(* Cyc_Absyn_match_exn_name).f2,({ struct _tuple4* _temp13=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp13->f1=( void*)({
struct Cyc_Tcenv_TunionRes_struct* _temp14=( struct Cyc_Tcenv_TunionRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct)); _temp14[ 0]=({ struct
Cyc_Tcenv_TunionRes_struct _temp15; _temp15.tag= Cyc_Tcenv_TunionRes; _temp15.f1=
Cyc_Absyn_exn_tud; _temp15.f2= Cyc_Absyn_match_exn_tuf; _temp15;}); _temp14;});
_temp13->f2= 1; _temp13;}));{ struct Cyc_Dict_Dict* _temp16=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Tcenv_Genv* data))
Cyc_Dict_insert)((( struct Cyc_Dict_Dict*(*)( int(* comp)( struct Cyc_List_List*,
struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp), _temp9, _temp8);
return({ struct Cyc_Tcenv_Tenv* _temp17=( struct Cyc_Tcenv_Tenv*) GC_malloc(
sizeof( struct Cyc_Tcenv_Tenv)); _temp17->ns= _temp9; _temp17->ae= _temp16;
_temp17->le= 0; _temp17;});}}} static struct Cyc_Tcenv_Genv* Cyc_Tcenv_lookup_namespace(
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
struct Cyc_List_List*) _check_null( nss))->hd); struct _handler_cons _temp18;
_push_handler(& _temp18);{ int _temp20= 0; if( setjmp( _temp18.handler)){
_temp20= 1;} if( ! _temp20){ lookup( ge2, v);({ void(* _temp21)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp25= loc; struct _tagged_arr _temp26= _tag_arr("%s is ambiguous", sizeof(
unsigned char), 16u); struct Cyc_Stdio_String_pa_struct _temp27; _temp27.tag=
Cyc_Stdio_String_pa; _temp27.f1=( struct _tagged_arr)* v;{ void* _temp24=( void*)&
_temp27; void* _temp22[ 1u]={ _temp24}; struct _tagged_arr _temp23={( void*)
_temp22,( void*) _temp22,( void*)( _temp22 + 1u)}; _temp21( _temp25, _temp26,
_temp23);}});; _pop_handler();} else{ void* _temp19=( void*) _exn_thrown; void*
_temp29= _temp19; _LL31: if( _temp29 == Cyc_Dict_Absent){ goto _LL32;} else{
goto _LL33;} _LL33: goto _LL34; _LL32: goto _LL30; _LL34:( void) _throw( _temp29);
_LL30:;}}} return;} static void* Cyc_Tcenv_scoped_lookup( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void*(* lookup)( struct Cyc_Tcenv_Genv*,
struct _tagged_arr*), struct _tagged_arr* v){ struct Cyc_List_List* cns= te->ns;
while( 1) { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, cns);{ struct
_handler_cons _temp35; _push_handler(& _temp35);{ int _temp37= 0; if( setjmp(
_temp35.handler)){ _temp37= 1;} if( ! _temp37){{ void* result= lookup( ge, v);
Cyc_Tcenv_check_repeat( te, loc, lookup, v, ge->availables);{ void* _temp38=
result; _npop_handler( 0u); return _temp38;}}; _pop_handler();} else{ void*
_temp36=( void*) _exn_thrown; void* _temp40= _temp36; _LL42: if( _temp40 == Cyc_Dict_Absent){
goto _LL43;} else{ goto _LL44;} _LL44: goto _LL45; _LL43: goto _LL41; _LL45:(
void) _throw( _temp40); _LL41:;}}}{ struct Cyc_List_List* _temp46= ge->availables;
for( 0; _temp46 != 0; _temp46=(( struct Cyc_List_List*) _check_null( _temp46))->tl){
struct Cyc_Tcenv_Genv* ge2=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,( struct Cyc_List_List*)((
struct Cyc_List_List*) _check_null( _temp46))->hd); struct _handler_cons _temp47;
_push_handler(& _temp47);{ int _temp49= 0; if( setjmp( _temp47.handler)){
_temp49= 1;} if( ! _temp49){{ void* result= lookup( ge2, v); Cyc_Tcenv_check_repeat(
te, loc, lookup, v,(( struct Cyc_List_List*) _check_null( _temp46))->tl);{ void*
_temp50= result; _npop_handler( 0u); return _temp50;}}; _pop_handler();} else{
void* _temp48=( void*) _exn_thrown; void* _temp52= _temp48; _LL54: if( _temp52
== Cyc_Dict_Absent){ goto _LL55;} else{ goto _LL56;} _LL56: goto _LL57; _LL55:
goto _LL53; _LL57:( void) _throw( _temp52); _LL53:;}}}} if( cns == 0){( int)
_throw(( void*) Cyc_Dict_Absent);} cns= Cyc_Tcenv_outer_namespace( cns);}}
static void* Cyc_Tcenv_lookup_ordinary_global_f( struct Cyc_Tcenv_Genv* ge,
struct _tagged_arr* v){ struct _tuple4* ans=(( struct _tuple4*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->ordinaries, v);(* ans).f2= 1;
return(* ans).f1;} static void* Cyc_Tcenv_lookup_ordinary_global( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp60;
struct _tagged_arr* _temp61; void* _temp63; struct _tuple0* _temp58= q; _temp60=*
_temp58; _LL64: _temp63= _temp60.f1; goto _LL62; _LL62: _temp61= _temp60.f2;
goto _LL59; _LL59: { void* _temp65= _temp63; struct Cyc_List_List* _temp75;
struct Cyc_List_List* _temp77; struct Cyc_List_List _temp79; struct Cyc_List_List*
_temp80; struct _tagged_arr* _temp82; struct Cyc_List_List* _temp84; _LL67: if(
_temp65 ==( void*) Cyc_Absyn_Loc_n){ goto _LL68;} else{ goto _LL69;} _LL69: if((
unsigned int) _temp65 > 1u?*(( int*) _temp65) == Cyc_Absyn_Rel_n: 0){ _LL76:
_temp75=(( struct Cyc_Absyn_Rel_n_struct*) _temp65)->f1; if( _temp75 == 0){ goto
_LL70;} else{ goto _LL71;}} else{ goto _LL71;} _LL71: if(( unsigned int) _temp65
> 1u?*(( int*) _temp65) == Cyc_Absyn_Rel_n: 0){ _LL78: _temp77=(( struct Cyc_Absyn_Rel_n_struct*)
_temp65)->f1; if( _temp77 == 0){ goto _LL73;} else{ _temp79=* _temp77; _LL83:
_temp82=( struct _tagged_arr*) _temp79.hd; goto _LL81; _LL81: _temp80= _temp79.tl;
goto _LL72;}} else{ goto _LL73;} _LL73: if(( unsigned int) _temp65 > 1u?*(( int*)
_temp65) == Cyc_Absyn_Abs_n: 0){ _LL85: _temp84=(( struct Cyc_Absyn_Abs_n_struct*)
_temp65)->f1; goto _LL74;} else{ goto _LL66;} _LL68: goto _LL70; _LL70: return
Cyc_Tcenv_scoped_lookup( te, loc, Cyc_Tcenv_lookup_ordinary_global_f, _temp61);
_LL72: { struct Cyc_Tcenv_Genv* _temp86= Cyc_Tcenv_lookup_namespace( te, loc,
_temp82, _temp80); return Cyc_Tcenv_lookup_ordinary_global_f( _temp86, _temp61);}
_LL74: return Cyc_Tcenv_lookup_ordinary_global_f((( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp84), _temp61); _LL66:;}} struct Cyc_List_List* Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tagged_arr*
n, struct Cyc_List_List* ns){ struct Cyc_List_List* _temp87= te->ns; struct Cyc_List_List*
_temp88= 0; while( 1) { struct Cyc_Tcenv_Genv* _temp89=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp87);{ struct Cyc_List_List* _temp90= _temp89->availables; for( 0; _temp90
!= 0; _temp90=(( struct Cyc_List_List*) _check_null( _temp90))->tl){ struct Cyc_Tcenv_Genv*
ge2=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( te->ae,( struct Cyc_List_List*)(( struct Cyc_List_List*)
_check_null( _temp90))->hd); if((( int(*)( struct Cyc_Set_Set* s, struct
_tagged_arr* elt)) Cyc_Set_member)( ge2->namespaces, n)){ _temp88=({ struct Cyc_List_List*
_temp91=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp91->hd=( void*)(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( _temp87,({ struct Cyc_List_List* _temp92=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp92->hd=(
void*) n; _temp92->tl= ns; _temp92;})); _temp91->tl= _temp88; _temp91;});}}} if(((
int(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_member)(
_temp89->namespaces, n)){ _temp88=({ struct Cyc_List_List* _temp93=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp93->hd=( void*)(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp87,({
struct Cyc_List_List* _temp94=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp94->hd=( void*) n; _temp94->tl= ns; _temp94;})); _temp93->tl=
_temp88; _temp93;});} if( _temp88 != 0){ if((( struct Cyc_List_List*)
_check_null( _temp88))->tl != 0){({ void(* _temp95)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp99= loc; struct _tagged_arr _temp100= _tag_arr("%s is ambiguous", sizeof(
unsigned char), 16u); struct Cyc_Stdio_String_pa_struct _temp101; _temp101.tag=
Cyc_Stdio_String_pa; _temp101.f1=( struct _tagged_arr)* n;{ void* _temp98=( void*)&
_temp101; void* _temp96[ 1u]={ _temp98}; struct _tagged_arr _temp97={( void*)
_temp96,( void*) _temp96,( void*)( _temp96 + 1u)}; _temp95( _temp99, _temp100,
_temp97);}});} return( struct Cyc_List_List*)(( struct Cyc_List_List*)
_check_null( _temp88))->hd;} if( _temp87 == 0){( int) _throw(( void*) Cyc_Dict_Absent);}
_temp87= Cyc_Tcenv_outer_namespace( _temp87);}} static struct Cyc_Absyn_Structdecl**
Cyc_Tcenv_lookup_structdecl_f( struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){
return(( struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup)( ge->structdecls, v);} struct Cyc_Absyn_Structdecl**
Cyc_Tcenv_lookup_structdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tuple0* q){ struct _tuple0 _temp104; struct _tagged_arr* _temp105;
void* _temp107; struct _tuple0* _temp102= q; _temp104=* _temp102; _LL108:
_temp107= _temp104.f1; goto _LL106; _LL106: _temp105= _temp104.f2; goto _LL103;
_LL103: { void* _temp109= _temp107; struct Cyc_List_List* _temp119; struct Cyc_List_List*
_temp121; struct Cyc_List_List* _temp123; struct Cyc_List_List _temp125; struct
Cyc_List_List* _temp126; struct _tagged_arr* _temp128; _LL111: if( _temp109 ==(
void*) Cyc_Absyn_Loc_n){ goto _LL112;} else{ goto _LL113;} _LL113: if((
unsigned int) _temp109 > 1u?*(( int*) _temp109) == Cyc_Absyn_Rel_n: 0){ _LL120:
_temp119=(( struct Cyc_Absyn_Rel_n_struct*) _temp109)->f1; if( _temp119 == 0){
goto _LL114;} else{ goto _LL115;}} else{ goto _LL115;} _LL115: if(( unsigned int)
_temp109 > 1u?*(( int*) _temp109) == Cyc_Absyn_Abs_n: 0){ _LL122: _temp121=((
struct Cyc_Absyn_Abs_n_struct*) _temp109)->f1; goto _LL116;} else{ goto _LL117;}
_LL117: if(( unsigned int) _temp109 > 1u?*(( int*) _temp109) == Cyc_Absyn_Rel_n:
0){ _LL124: _temp123=(( struct Cyc_Absyn_Rel_n_struct*) _temp109)->f1; if(
_temp123 == 0){ goto _LL110;} else{ _temp125=* _temp123; _LL129: _temp128=(
struct _tagged_arr*) _temp125.hd; goto _LL127; _LL127: _temp126= _temp125.tl;
goto _LL118;}} else{ goto _LL110;} _LL112: goto _LL114; _LL114: return(( struct
Cyc_Absyn_Structdecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Structdecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_arr*), struct _tagged_arr* v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_structdecl_f,
_temp105); _LL116: { struct Cyc_Tcenv_Genv* _temp130=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp121); return(( struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp130->structdecls, _temp105);}
_LL118: { struct Cyc_Tcenv_Genv* _temp131= Cyc_Tcenv_lookup_namespace( te, loc,
_temp128, _temp126); return(( struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp131->structdecls, _temp105);}
_LL110:;}} static struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Uniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->uniondecls,
v);} struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp134; struct _tagged_arr* _temp135; void* _temp137; struct _tuple0* _temp132=
q; _temp134=* _temp132; _LL138: _temp137= _temp134.f1; goto _LL136; _LL136:
_temp135= _temp134.f2; goto _LL133; _LL133: { void* _temp139= _temp137; struct
Cyc_List_List* _temp149; struct Cyc_List_List* _temp151; struct Cyc_List_List*
_temp153; struct Cyc_List_List _temp155; struct Cyc_List_List* _temp156; struct
_tagged_arr* _temp158; _LL141: if( _temp139 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL142;} else{ goto _LL143;} _LL143: if(( unsigned int) _temp139 > 1u?*(( int*)
_temp139) == Cyc_Absyn_Rel_n: 0){ _LL150: _temp149=(( struct Cyc_Absyn_Rel_n_struct*)
_temp139)->f1; if( _temp149 == 0){ goto _LL144;} else{ goto _LL145;}} else{ goto
_LL145;} _LL145: if(( unsigned int) _temp139 > 1u?*(( int*) _temp139) == Cyc_Absyn_Abs_n:
0){ _LL152: _temp151=(( struct Cyc_Absyn_Abs_n_struct*) _temp139)->f1; goto
_LL146;} else{ goto _LL147;} _LL147: if(( unsigned int) _temp139 > 1u?*(( int*)
_temp139) == Cyc_Absyn_Rel_n: 0){ _LL154: _temp153=(( struct Cyc_Absyn_Rel_n_struct*)
_temp139)->f1; if( _temp153 == 0){ goto _LL140;} else{ _temp155=* _temp153;
_LL159: _temp158=( struct _tagged_arr*) _temp155.hd; goto _LL157; _LL157:
_temp156= _temp155.tl; goto _LL148;}} else{ goto _LL140;} _LL142: goto _LL144;
_LL144: return(( struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl**(* lookup)( struct
Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v)) Cyc_Tcenv_scoped_lookup)(
te, loc, Cyc_Tcenv_lookup_uniondecl_f, _temp135); _LL146: { struct Cyc_Tcenv_Genv*
_temp160=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( te->ae, _temp151); return(( struct Cyc_Absyn_Uniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp160->uniondecls,
_temp135);} _LL148: { struct Cyc_Tcenv_Genv* _temp161= Cyc_Tcenv_lookup_namespace(
te, loc, _temp158, _temp156); return(( struct Cyc_Absyn_Uniondecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp161->uniondecls,
_temp135);} _LL140:;}} static struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->tuniondecls,
v);} struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp164; struct _tagged_arr* _temp165; void* _temp167; struct _tuple0* _temp162=
q; _temp164=* _temp162; _LL168: _temp167= _temp164.f1; goto _LL166; _LL166:
_temp165= _temp164.f2; goto _LL163; _LL163: { void* _temp169= _temp167; struct
Cyc_List_List* _temp179; struct Cyc_List_List* _temp181; struct Cyc_List_List
_temp183; struct Cyc_List_List* _temp184; struct _tagged_arr* _temp186; struct
Cyc_List_List* _temp188; _LL171: if( _temp169 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL172;} else{ goto _LL173;} _LL173: if(( unsigned int) _temp169 > 1u?*(( int*)
_temp169) == Cyc_Absyn_Rel_n: 0){ _LL180: _temp179=(( struct Cyc_Absyn_Rel_n_struct*)
_temp169)->f1; if( _temp179 == 0){ goto _LL174;} else{ goto _LL175;}} else{ goto
_LL175;} _LL175: if(( unsigned int) _temp169 > 1u?*(( int*) _temp169) == Cyc_Absyn_Rel_n:
0){ _LL182: _temp181=(( struct Cyc_Absyn_Rel_n_struct*) _temp169)->f1; if(
_temp181 == 0){ goto _LL177;} else{ _temp183=* _temp181; _LL187: _temp186=(
struct _tagged_arr*) _temp183.hd; goto _LL185; _LL185: _temp184= _temp183.tl;
goto _LL176;}} else{ goto _LL177;} _LL177: if(( unsigned int) _temp169 > 1u?*((
int*) _temp169) == Cyc_Absyn_Abs_n: 0){ _LL189: _temp188=(( struct Cyc_Absyn_Abs_n_struct*)
_temp169)->f1; goto _LL178;} else{ goto _LL170;} _LL172: goto _LL174; _LL174:
return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Tuniondecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_arr*), struct _tagged_arr* v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_tuniondecl_f,
_temp165); _LL176: { struct Cyc_Tcenv_Genv* _temp190= Cyc_Tcenv_lookup_namespace(
te, loc, _temp186, _temp184); return(( struct Cyc_Absyn_Tuniondecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp190->tuniondecls,
_temp165);} _LL178: { struct Cyc_Tcenv_Genv* _temp191=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp188); return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp191->tuniondecls, _temp165);}
_LL170:;}} static struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_xtuniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->tuniondecls,
v);} struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp194; struct _tagged_arr* _temp195; void* _temp197; struct _tuple0* _temp192=
q; _temp194=* _temp192; _LL198: _temp197= _temp194.f1; goto _LL196; _LL196:
_temp195= _temp194.f2; goto _LL193; _LL193: { void* _temp199= _temp197; struct
Cyc_List_List* _temp209; struct Cyc_List_List* _temp211; struct Cyc_List_List
_temp213; struct Cyc_List_List* _temp214; struct _tagged_arr* _temp216; struct
Cyc_List_List* _temp218; _LL201: if(( unsigned int) _temp199 > 1u?*(( int*)
_temp199) == Cyc_Absyn_Rel_n: 0){ _LL210: _temp209=(( struct Cyc_Absyn_Rel_n_struct*)
_temp199)->f1; if( _temp209 == 0){ goto _LL202;} else{ goto _LL203;}} else{ goto
_LL203;} _LL203: if( _temp199 ==( void*) Cyc_Absyn_Loc_n){ goto _LL204;} else{
goto _LL205;} _LL205: if(( unsigned int) _temp199 > 1u?*(( int*) _temp199) ==
Cyc_Absyn_Rel_n: 0){ _LL212: _temp211=(( struct Cyc_Absyn_Rel_n_struct*)
_temp199)->f1; if( _temp211 == 0){ goto _LL207;} else{ _temp213=* _temp211;
_LL217: _temp216=( struct _tagged_arr*) _temp213.hd; goto _LL215; _LL215:
_temp214= _temp213.tl; goto _LL206;}} else{ goto _LL207;} _LL207: if((
unsigned int) _temp199 > 1u?*(( int*) _temp199) == Cyc_Absyn_Abs_n: 0){ _LL219:
_temp218=(( struct Cyc_Absyn_Abs_n_struct*) _temp199)->f1; goto _LL208;} else{
goto _LL200;} _LL202: { struct _handler_cons _temp220; _push_handler(& _temp220);{
int _temp222= 0; if( setjmp( _temp220.handler)){ _temp222= 1;} if( ! _temp222){{
struct Cyc_Core_Opt* _temp224=({ struct Cyc_Core_Opt* _temp223=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp223->v=( void*)(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v))
Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_xtuniondecl_f, _temp195);
_temp223;}); _npop_handler( 0u); return _temp224;}; _pop_handler();} else{ void*
_temp221=( void*) _exn_thrown; void* _temp226= _temp221; _LL228: if( _temp226 ==
Cyc_Dict_Absent){ goto _LL229;} else{ goto _LL230;} _LL230: goto _LL231; _LL229:
return 0; _LL231:( void) _throw( _temp226); _LL227:;}}} _LL204:({ void(*
_temp232)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp235= loc;
struct _tagged_arr _temp236= _tag_arr("lookup_xtuniondecl: impossible", sizeof(
unsigned char), 31u); void* _temp233[ 0u]={}; struct _tagged_arr _temp234={(
void*) _temp233,( void*) _temp233,( void*)( _temp233 + 0u)}; _temp232( _temp235,
_temp236, _temp234);}); return 0; _LL206: { struct Cyc_Tcenv_Genv* ge;{ struct
_handler_cons _temp237; _push_handler(& _temp237);{ int _temp239= 0; if( setjmp(
_temp237.handler)){ _temp239= 1;} if( ! _temp239){ ge= Cyc_Tcenv_lookup_namespace(
te, loc, _temp216, _temp214);; _pop_handler();} else{ void* _temp238=( void*)
_exn_thrown; void* _temp241= _temp238; _LL243: if( _temp241 == Cyc_Dict_Absent){
goto _LL244;} else{ goto _LL245;} _LL245: goto _LL246; _LL244:({ void(* _temp247)(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp250= loc; struct _tagged_arr
_temp251= _tag_arr("bad qualified name for xtunion", sizeof( unsigned char), 31u);
void* _temp248[ 0u]={}; struct _tagged_arr _temp249={( void*) _temp248,( void*)
_temp248,( void*)( _temp248 + 0u)}; _temp247( _temp250, _temp251, _temp249);});(
int) _throw(( void*) Cyc_Dict_Absent); _LL246:( void) _throw( _temp241); _LL242:;}}}
return({ struct Cyc_Core_Opt* _temp252=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp252->v=( void*)(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->tuniondecls,
_temp195); _temp252;});} _LL208: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp218); return({ struct Cyc_Core_Opt* _temp253=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp253->v=( void*)(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->tuniondecls,
_temp195); _temp253;});} _LL200:;}} static struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->enumdecls,
v);} struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp256; struct _tagged_arr* _temp257; void* _temp259; struct _tuple0* _temp254=
q; _temp256=* _temp254; _LL260: _temp259= _temp256.f1; goto _LL258; _LL258:
_temp257= _temp256.f2; goto _LL255; _LL255: { void* _temp261= _temp259; struct
Cyc_List_List* _temp271; struct Cyc_List_List* _temp273; struct Cyc_List_List
_temp275; struct Cyc_List_List* _temp276; struct _tagged_arr* _temp278; struct
Cyc_List_List* _temp280; _LL263: if( _temp261 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL264;} else{ goto _LL265;} _LL265: if(( unsigned int) _temp261 > 1u?*(( int*)
_temp261) == Cyc_Absyn_Rel_n: 0){ _LL272: _temp271=(( struct Cyc_Absyn_Rel_n_struct*)
_temp261)->f1; if( _temp271 == 0){ goto _LL266;} else{ goto _LL267;}} else{ goto
_LL267;} _LL267: if(( unsigned int) _temp261 > 1u?*(( int*) _temp261) == Cyc_Absyn_Rel_n:
0){ _LL274: _temp273=(( struct Cyc_Absyn_Rel_n_struct*) _temp261)->f1; if(
_temp273 == 0){ goto _LL269;} else{ _temp275=* _temp273; _LL279: _temp278=(
struct _tagged_arr*) _temp275.hd; goto _LL277; _LL277: _temp276= _temp275.tl;
goto _LL268;}} else{ goto _LL269;} _LL269: if(( unsigned int) _temp261 > 1u?*((
int*) _temp261) == Cyc_Absyn_Abs_n: 0){ _LL281: _temp280=(( struct Cyc_Absyn_Abs_n_struct*)
_temp261)->f1; goto _LL270;} else{ goto _LL262;} _LL264: goto _LL266; _LL266:
return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Enumdecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_arr*), struct _tagged_arr* v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_enumdecl_f,
_temp257); _LL268: { struct Cyc_Tcenv_Genv* _temp282= Cyc_Tcenv_lookup_namespace(
te, loc, _temp278, _temp276); return(( struct Cyc_Absyn_Enumdecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp282->enumdecls,
_temp257);} _LL270: { struct Cyc_Tcenv_Genv* _temp283=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp280); return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp283->enumdecls, _temp257);}
_LL262:;}} static struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->typedefs,
v);} struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp286; struct _tagged_arr* _temp287; void* _temp289; struct _tuple0* _temp284=
q; _temp286=* _temp284; _LL290: _temp289= _temp286.f1; goto _LL288; _LL288:
_temp287= _temp286.f2; goto _LL285; _LL285: { void* _temp291= _temp289; struct
Cyc_List_List* _temp301; struct Cyc_List_List* _temp303; struct Cyc_List_List
_temp305; struct Cyc_List_List* _temp306; struct _tagged_arr* _temp308; struct
Cyc_List_List* _temp310; _LL293: if( _temp291 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL294;} else{ goto _LL295;} _LL295: if(( unsigned int) _temp291 > 1u?*(( int*)
_temp291) == Cyc_Absyn_Rel_n: 0){ _LL302: _temp301=(( struct Cyc_Absyn_Rel_n_struct*)
_temp291)->f1; if( _temp301 == 0){ goto _LL296;} else{ goto _LL297;}} else{ goto
_LL297;} _LL297: if(( unsigned int) _temp291 > 1u?*(( int*) _temp291) == Cyc_Absyn_Rel_n:
0){ _LL304: _temp303=(( struct Cyc_Absyn_Rel_n_struct*) _temp291)->f1; if(
_temp303 == 0){ goto _LL299;} else{ _temp305=* _temp303; _LL309: _temp308=(
struct _tagged_arr*) _temp305.hd; goto _LL307; _LL307: _temp306= _temp305.tl;
goto _LL298;}} else{ goto _LL299;} _LL299: if(( unsigned int) _temp291 > 1u?*((
int*) _temp291) == Cyc_Absyn_Abs_n: 0){ _LL311: _temp310=(( struct Cyc_Absyn_Abs_n_struct*)
_temp291)->f1; goto _LL300;} else{ goto _LL292;} _LL294: goto _LL296; _LL296:
return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Typedefdecl*(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_arr*), struct _tagged_arr* v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_typedefdecl_f,
_temp287); _LL298: { struct Cyc_Tcenv_Genv* _temp312= Cyc_Tcenv_lookup_namespace(
te, loc, _temp308, _temp306); return(( struct Cyc_Absyn_Typedefdecl*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp312->typedefs,
_temp287);} _LL300: { struct Cyc_Tcenv_Genv* _temp313=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp310); return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp313->typedefs, _temp287);}
_LL292:;}} struct Cyc_Tcenv_Genv* Cyc_Tcenv_empty_genv(){ return({ struct Cyc_Tcenv_Genv*
_temp314=( struct Cyc_Tcenv_Genv*) GC_malloc( sizeof( struct Cyc_Tcenv_Genv));
_temp314->namespaces=( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Tcutil_empty_var_set))->v; _temp314->structdecls=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp314->uniondecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp314->tuniondecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp314->enumdecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp314->typedefs=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp314->ordinaries=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp314->availables= 0; _temp314;});} void* Cyc_Tcenv_get_visible( void* f){
while( 1) { void* _temp315= f; void* _temp323; void* _temp325; void* _temp327;
void* _temp329; _LL317: if(*(( int*) _temp315) == Cyc_Tcenv_Outermost){ _LL324:
_temp323=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp315)->f1; goto
_LL318;} else{ goto _LL319;} _LL319: if(*(( int*) _temp315) == Cyc_Tcenv_Frame){
_LL326: _temp325=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp315)->f1; goto
_LL320;} else{ goto _LL321;} _LL321: if(*(( int*) _temp315) == Cyc_Tcenv_Hidden){
_LL330: _temp329=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp315)->f1; goto
_LL328; _LL328: _temp327=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp315)->f2;
goto _LL322;} else{ goto _LL316;} _LL318: return _temp323; _LL320: return
_temp325; _LL322: f= _temp327; goto _LL316; _LL316:;}} void* Cyc_Tcenv_put_visible(
void* f, void* x){ void* _temp331= f; void* _temp339; void* _temp341; void*
_temp343; void* _temp345; void* _temp347; _LL333: if(*(( int*) _temp331) == Cyc_Tcenv_Outermost){
_LL340: _temp339=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp331)->f1;
goto _LL334;} else{ goto _LL335;} _LL335: if(*(( int*) _temp331) == Cyc_Tcenv_Frame){
_LL344: _temp343=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp331)->f1; goto
_LL342; _LL342: _temp341=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp331)->f2;
goto _LL336;} else{ goto _LL337;} _LL337: if(*(( int*) _temp331) == Cyc_Tcenv_Hidden){
_LL348: _temp347=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp331)->f1; goto
_LL346; _LL346: _temp345=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp331)->f2;
goto _LL338;} else{ goto _LL332;} _LL334: return( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp349=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp349[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp350; _temp350.tag= Cyc_Tcenv_Outermost;
_temp350.f1=( void*) x; _temp350;}); _temp349;}); _LL336: return( void*)({
struct Cyc_Tcenv_Frame_struct* _temp351=( struct Cyc_Tcenv_Frame_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Frame_struct)); _temp351[ 0]=({ struct Cyc_Tcenv_Frame_struct
_temp352; _temp352.tag= Cyc_Tcenv_Frame; _temp352.f1=( void*) x; _temp352.f2=(
void*) _temp341; _temp352;}); _temp351;}); _LL338: return( void*)({ struct Cyc_Tcenv_Hidden_struct*
_temp353=( struct Cyc_Tcenv_Hidden_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Hidden_struct));
_temp353[ 0]=({ struct Cyc_Tcenv_Hidden_struct _temp354; _temp354.tag= Cyc_Tcenv_Hidden;
_temp354.f1=( void*) _temp347; _temp354.f2=( void*) Cyc_Tcenv_put_visible(
_temp345, x); _temp354;}); _temp353;}); _LL332:;} void* Cyc_Tcenv_apply_to_visible(
void*(* g)( void*), void* f){ return Cyc_Tcenv_put_visible( f, g( Cyc_Tcenv_get_visible(
f)));} void* Cyc_Tcenv_map_frames( void*(* g)( void*), void* f){ void* _temp355=
f; void* _temp363; void* _temp365; void* _temp367; void* _temp369; void*
_temp371; _LL357: if(*(( int*) _temp355) == Cyc_Tcenv_Outermost){ _LL364:
_temp363=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp355)->f1; goto
_LL358;} else{ goto _LL359;} _LL359: if(*(( int*) _temp355) == Cyc_Tcenv_Frame){
_LL368: _temp367=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp355)->f1; goto
_LL366; _LL366: _temp365=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp355)->f2;
goto _LL360;} else{ goto _LL361;} _LL361: if(*(( int*) _temp355) == Cyc_Tcenv_Hidden){
_LL372: _temp371=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp355)->f1; goto
_LL370; _LL370: _temp369=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp355)->f2;
goto _LL362;} else{ goto _LL356;} _LL358: return( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp373=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp373[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp374; _temp374.tag= Cyc_Tcenv_Outermost;
_temp374.f1=( void*) g( _temp363); _temp374;}); _temp373;}); _LL360: return(
void*)({ struct Cyc_Tcenv_Frame_struct* _temp375=( struct Cyc_Tcenv_Frame_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Frame_struct)); _temp375[ 0]=({ struct Cyc_Tcenv_Frame_struct
_temp376; _temp376.tag= Cyc_Tcenv_Frame; _temp376.f1=( void*) g( _temp367);
_temp376.f2=( void*) Cyc_Tcenv_map_frames( g, _temp365); _temp376;}); _temp375;});
_LL362: return( void*)({ struct Cyc_Tcenv_Hidden_struct* _temp377=( struct Cyc_Tcenv_Hidden_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Hidden_struct)); _temp377[ 0]=({ struct Cyc_Tcenv_Hidden_struct
_temp378; _temp378.tag= Cyc_Tcenv_Hidden; _temp378.f1=( void*) g( _temp371);
_temp378.f2=( void*) Cyc_Tcenv_map_frames( g, _temp369); _temp378;}); _temp377;});
_LL356:;} struct Cyc_Tcenv_Fenv* Cyc_Tcenv_get_fenv( struct Cyc_Tcenv_Tenv* te){
if( te->le == 0){( int) _throw(( void*) Cyc_Tcenv_Env_error);} return(( struct
Cyc_Tcenv_Fenv*(*)( void* f)) Cyc_Tcenv_get_visible)(( void*)(( struct Cyc_Core_Opt*)
_check_null( te->le))->v);} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_put_fenv( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Fenv* fe){ if( te->le == 0){( int) _throw((
void*) Cyc_Tcenv_Env_error);} return({ struct Cyc_Tcenv_Tenv* _temp379=( struct
Cyc_Tcenv_Tenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Tenv)); _temp379->ns= te->ns;
_temp379->ae= te->ae; _temp379->le=({ struct Cyc_Core_Opt* _temp380=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp380->v=( void*)(( void*(*)( void*
f, struct Cyc_Tcenv_Fenv* x)) Cyc_Tcenv_put_visible)(( void*)(( struct Cyc_Core_Opt*)
_check_null( te->le))->v, fe); _temp380;}); _temp379;});} static int Cyc_Tcenv_region_ancestor(
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
struct _tagged_arr)({ struct _tagged_arr(* _temp381)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp385= _tag_arr("can't find region variable %s in po",
sizeof( unsigned char), 36u); struct Cyc_Stdio_String_pa_struct _temp388;
_temp388.tag= Cyc_Stdio_String_pa; _temp388.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Absyn_VarType_struct* _temp386=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp386[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp387; _temp387.tag= Cyc_Absyn_VarType; _temp387.f1= r; _temp387;}); _temp386;}));{
void* _temp384=( void*)& _temp388; void* _temp382[ 1u]={ _temp384}; struct
_tagged_arr _temp383={( void*) _temp382,( void*) _temp382,( void*)( _temp382 + 1u)};
_temp381( _temp385, _temp383);}}));} static int Cyc_Tcenv_check_region_outlives(
struct Cyc_List_List* po, struct Cyc_Absyn_Tvar* r1, struct Cyc_Absyn_Tvar* r2){
struct Cyc_List_List* r2_ancestors; while( po != 0? Cyc_Absyn_tvar_cmp((( struct
Cyc_Tcenv_RgnPO*)(( struct Cyc_List_List*) _check_null( po))->hd)->tvar, r2) !=
0: 0) { po=(( struct Cyc_List_List*) _check_null( po))->tl;} if( po == 0){
return 0;} return Cyc_Tcenv_region_ancestor((( struct Cyc_Tcenv_RgnPO*)(( struct
Cyc_List_List*) _check_null( po))->hd)->ancestors, r1);} struct _tuple5{ void*
f1; void* f2; } ; static struct Cyc_List_List* Cyc_Tcenv_initial_region_po(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* tvs, struct Cyc_List_List*
po){ struct Cyc_List_List* result= 0; for( 0; tvs != 0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){ struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd; if( Cyc_Tcutil_tvar_kind( tv) ==(
void*) Cyc_Absyn_RgnKind){ result=({ struct Cyc_List_List* _temp389=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp389->hd=( void*)({ struct Cyc_Tcenv_RgnPO*
_temp390=( struct Cyc_Tcenv_RgnPO*) GC_malloc( sizeof( struct Cyc_Tcenv_RgnPO));
_temp390->tvar= tv; _temp390->ancestors= 0; _temp390;}); _temp389->tl= result;
_temp389;});}} for( 0; po != 0; po=(( struct Cyc_List_List*) _check_null( po))->tl){
struct _tuple5 _temp391=*(( struct _tuple5*)(( struct Cyc_List_List*)
_check_null( po))->hd); void* _temp399; void* _temp401; struct Cyc_Absyn_Tvar*
_temp403; void* _temp405; struct Cyc_Absyn_Tvar* _temp407; void* _temp409; void*
_temp411; _LL393: _LL400: _temp399= _temp391.f1; if( _temp399 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL394;} else{ goto _LL395;} _LL395: _LL406: _temp405= _temp391.f1; if((
unsigned int) _temp405 > 4u?*(( int*) _temp405) == Cyc_Absyn_VarType: 0){ _LL408:
_temp407=(( struct Cyc_Absyn_VarType_struct*) _temp405)->f1; goto _LL402;} else{
goto _LL397;} _LL402: _temp401= _temp391.f2; if(( unsigned int) _temp401 > 4u?*((
int*) _temp401) == Cyc_Absyn_VarType: 0){ _LL404: _temp403=(( struct Cyc_Absyn_VarType_struct*)
_temp401)->f1; goto _LL396;} else{ goto _LL397;} _LL397: _LL412: _temp411=
_temp391.f1; goto _LL410; _LL410: _temp409= _temp391.f2; goto _LL398; _LL394:
goto _LL392; _LL396: if( Cyc_Absyn_tvar_cmp( _temp407, _temp403) == 0? 1: Cyc_Tcenv_check_region_outlives(
result, _temp403, _temp407)){({ void(* _temp413)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp418= loc; struct _tagged_arr _temp419= _tag_arr("bad region lifetime assertion (circularity):%s < %s",
sizeof( unsigned char), 52u); struct Cyc_Stdio_String_pa_struct _temp425;
_temp425.tag= Cyc_Stdio_String_pa; _temp425.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Absyn_VarType_struct* _temp423=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp423[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp424; _temp424.tag= Cyc_Absyn_VarType; _temp424.f1= _temp407; _temp424;});
_temp423;}));{ void* _temp416=( void*)& _temp425; struct Cyc_Stdio_String_pa_struct
_temp422; _temp422.tag= Cyc_Stdio_String_pa; _temp422.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Absyn_VarType_struct* _temp420=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp420[ 0]=({ struct Cyc_Absyn_VarType_struct _temp421; _temp421.tag= Cyc_Absyn_VarType;
_temp421.f1= _temp403; _temp421;}); _temp420;}));{ void* _temp417=( void*)&
_temp422; void* _temp414[ 2u]={ _temp416, _temp417}; struct _tagged_arr _temp415={(
void*) _temp414,( void*) _temp414,( void*)( _temp414 + 2u)}; _temp413( _temp418,
_temp419, _temp415);}}});} else{ struct Cyc_Tcenv_RgnPO* _temp426= Cyc_Tcenv_find_region_in_po(
result, _temp407); struct Cyc_Tcenv_RgnPO* _temp427= Cyc_Tcenv_find_region_in_po(
result, _temp403); _temp427->ancestors=({ struct Cyc_List_List* _temp428=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp428->hd=(
void*) _temp426; _temp428->tl= _temp427->ancestors; _temp428;});} goto _LL392;
_LL398:({ void(* _temp429)( struct Cyc_Position_Segment*, struct _tagged_arr fmt,
struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp434=
loc; struct _tagged_arr _temp435= _tag_arr("bad region lifetime assertion:: %s < %s",
sizeof( unsigned char), 40u); struct Cyc_Stdio_String_pa_struct _temp437;
_temp437.tag= Cyc_Stdio_String_pa; _temp437.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp411);{ void* _temp432=( void*)& _temp437; struct Cyc_Stdio_String_pa_struct
_temp436; _temp436.tag= Cyc_Stdio_String_pa; _temp436.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( _temp409);{ void* _temp433=( void*)& _temp436; void*
_temp430[ 2u]={ _temp432, _temp433}; struct _tagged_arr _temp431={( void*)
_temp430,( void*) _temp430,( void*)( _temp430 + 2u)}; _temp429( _temp434,
_temp435, _temp431);}}}); goto _LL392; _LL392:;} return result;} static struct
Cyc_List_List* Cyc_Tcenv_add_region_po( struct Cyc_List_List* po, struct Cyc_Absyn_Tvar*
tv){ return({ struct Cyc_List_List* _temp438=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp438->hd=( void*)({ struct Cyc_Tcenv_RgnPO*
_temp439=( struct Cyc_Tcenv_RgnPO*) GC_malloc( sizeof( struct Cyc_Tcenv_RgnPO));
_temp439->tvar= tv; _temp439->ancestors= po; _temp439;}); _temp438->tl= po;
_temp438;});} static struct Cyc_Tcenv_Fenv* Cyc_Tcenv_copy_fenv_old_ctrl( struct
Cyc_Tcenv_Fenv* f){ return({ struct Cyc_Tcenv_Fenv* _temp440=( struct Cyc_Tcenv_Fenv*)
GC_malloc( sizeof( struct Cyc_Tcenv_Fenv)); _temp440->shared= f->shared;
_temp440->type_vars= f->type_vars; _temp440->region_order= f->region_order;
_temp440->locals= f->locals; _temp440->encloser= f->encloser; _temp440->ctrl_env=
f->ctrl_env; _temp440->capability=( void*)(( void*) f->capability); _temp440->curr_rgn=(
void*)(( void*) f->curr_rgn); _temp440;});} static struct Cyc_Tcenv_Fenv* Cyc_Tcenv_copy_fenv_new_ctrl(
struct Cyc_Tcenv_Fenv* f){ struct Cyc_Tcenv_CtrlEnv* c= f->ctrl_env; return({
struct Cyc_Tcenv_Fenv* _temp441=( struct Cyc_Tcenv_Fenv*) GC_malloc( sizeof(
struct Cyc_Tcenv_Fenv)); _temp441->shared= f->shared; _temp441->type_vars= f->type_vars;
_temp441->region_order= f->region_order; _temp441->locals= f->locals; _temp441->encloser=
f->encloser; _temp441->ctrl_env=({ struct Cyc_Tcenv_CtrlEnv* _temp442=( struct
Cyc_Tcenv_CtrlEnv*) GC_malloc( sizeof( struct Cyc_Tcenv_CtrlEnv)); _temp442->continue_stmt=(
void*)(( void*) c->continue_stmt); _temp442->break_stmt=( void*)(( void*) c->break_stmt);
_temp442->fallthru_clause= c->fallthru_clause; _temp442->next_stmt=( void*)((
void*) c->next_stmt); _temp442->try_depth= c->try_depth; _temp442;}); _temp441->capability=(
void*)(( void*) f->capability); _temp441->curr_rgn=( void*)(( void*) f->curr_rgn);
_temp441;});} void* Cyc_Tcenv_return_typ( struct Cyc_Tcenv_Tenv* te){ if( te->le
== 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected return_typ",
sizeof( unsigned char), 29u));} return( void*)(( Cyc_Tcenv_get_fenv( te))->shared)->return_typ;}
struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars( struct Cyc_Tcenv_Tenv* te){
if( te->le == 0){ return 0;} return( Cyc_Tcenv_get_fenv( te))->type_vars;}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment* loc,
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* tvs){ if( te->le == 0){(( int(*)(
struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected add_type_vars",
sizeof( unsigned char), 32u));}{ struct Cyc_Tcenv_Fenv* _temp443= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); Cyc_Tcutil_add_tvar_identities( tvs);{ struct Cyc_List_List*
_temp444=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( tvs, _temp443->type_vars); Cyc_Tcutil_check_unique_tvars(
loc, _temp444); _temp443->type_vars= _temp444; return Cyc_Tcenv_put_fenv( te,
_temp443);}}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Vardecl* vd){ if( te->le == 0){((
int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected add_local_var",
sizeof( unsigned char), 32u));}{ struct Cyc_Tcenv_Fenv* _temp445= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); struct _tagged_arr* _temp446=(* vd->name).f2;{ void*
_temp447=(* vd->name).f1; _LL449: if( _temp447 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL450;} else{ goto _LL451;} _LL451: goto _LL452; _LL450: goto _LL448; _LL452:(
int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp453=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp453[ 0]=({ struct
Cyc_Core_Impossible_struct _temp454; _temp454.tag= Cyc_Core_Impossible; _temp454.f1=
_tag_arr("add_local_var: called with Rel_n", sizeof( unsigned char), 33u);
_temp454;}); _temp453;})); goto _LL448; _LL448:;} _temp445->locals=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, void* data)) Cyc_Dict_insert)(
_temp445->locals, _temp446,( void*)({ struct Cyc_Absyn_Local_b_struct* _temp455=(
struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp455[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp456; _temp456.tag= Cyc_Absyn_Local_b;
_temp456.f1= vd; _temp456;}); _temp455;})); return Cyc_Tcenv_put_fenv( te,
_temp445);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_pat_var( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Vardecl* vd){ if( te->le == 0){((
int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected add_pat_var",
sizeof( unsigned char), 30u));}{ struct Cyc_Tcenv_Fenv* _temp457= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); struct _tagged_arr* _temp458=(* vd->name).f2; _temp457->locals=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key, void*
data)) Cyc_Dict_insert)( _temp457->locals, _temp458,( void*)({ struct Cyc_Absyn_Pat_b_struct*
_temp459=( struct Cyc_Absyn_Pat_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pat_b_struct));
_temp459[ 0]=({ struct Cyc_Absyn_Pat_b_struct _temp460; _temp460.tag= Cyc_Absyn_Pat_b;
_temp460.f1= vd; _temp460;}); _temp459;})); return Cyc_Tcenv_put_fenv( te,
_temp457);}} void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp463; struct
_tagged_arr* _temp464; void* _temp466; struct _tuple0* _temp461= q; _temp463=*
_temp461; _LL467: _temp466= _temp463.f1; goto _LL465; _LL465: _temp464= _temp463.f2;
goto _LL462; _LL462: { void* _temp468= _temp466; struct Cyc_List_List* _temp477;
_LL470: if( _temp468 ==( void*) Cyc_Absyn_Loc_n){ goto _LL471;} else{ goto
_LL472;} _LL472: if(( unsigned int) _temp468 > 1u?*(( int*) _temp468) == Cyc_Absyn_Rel_n:
0){ _LL478: _temp477=(( struct Cyc_Absyn_Rel_n_struct*) _temp468)->f1; if(
_temp477 == 0){ goto _LL476;} else{ goto _LL474;}} else{ goto _LL474;} _LL476:
if( te->le != 0){ goto _LL473;} else{ goto _LL474;} _LL474: goto _LL475; _LL471:
if( te->le == 0){( int) _throw(( void*) Cyc_Dict_Absent);} else{ goto _LL473;}
_LL473: { struct Cyc_Tcenv_Fenv* _temp479= Cyc_Tcenv_get_fenv( te); struct
_handler_cons _temp480; _push_handler(& _temp480);{ int _temp482= 0; if( setjmp(
_temp480.handler)){ _temp482= 1;} if( ! _temp482){{ void* _temp485=( void*)({
struct Cyc_Tcenv_VarRes_struct* _temp483=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp483[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp484; _temp484.tag= Cyc_Tcenv_VarRes; _temp484.f1=( void*)(( void*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp479->locals,
_temp464); _temp484;}); _temp483;}); _npop_handler( 0u); return _temp485;};
_pop_handler();} else{ void* _temp481=( void*) _exn_thrown; void* _temp487=
_temp481; _LL489: if( _temp487 == Cyc_Dict_Absent){ goto _LL490;} else{ goto
_LL491;} _LL491: goto _LL492; _LL490: return Cyc_Tcenv_lookup_ordinary_global(
te, loc, q); _LL492:( void) _throw( _temp487); _LL488:;}}} _LL475: { struct
_handler_cons _temp493; _push_handler(& _temp493);{ int _temp495= 0; if( setjmp(
_temp493.handler)){ _temp495= 1;} if( ! _temp495){{ void* _temp496= Cyc_Tcenv_lookup_ordinary_global(
te, loc, q); _npop_handler( 0u); return _temp496;}; _pop_handler();} else{ void*
_temp494=( void*) _exn_thrown; void* _temp498= _temp494; _LL500: if( _temp498 ==
Cyc_Dict_Absent){ goto _LL501;} else{ goto _LL502;} _LL502: goto _LL503; _LL501:({
void(* _temp504)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp508= loc;
struct _tagged_arr _temp509= _tag_arr("unbound identifier: %s", sizeof(
unsigned char), 23u); struct Cyc_Stdio_String_pa_struct _temp510; _temp510.tag=
Cyc_Stdio_String_pa; _temp510.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
q);{ void* _temp507=( void*)& _temp510; void* _temp505[ 1u]={ _temp507}; struct
_tagged_arr _temp506={( void*) _temp505,( void*) _temp505,( void*)( _temp505 + 1u)};
_temp504( _temp508, _temp509, _temp506);}}); return( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp511=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp511[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp512; _temp512.tag= Cyc_Tcenv_VarRes;
_temp512.f1=( void*)(( void*) Cyc_Absyn_Unresolved_b); _temp512;}); _temp511;});
_LL503:( void) _throw( _temp498); _LL499:;}}} _LL469:;}} void Cyc_Tcenv_process_continue(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Stmt**
sopt){ if( te->le == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected process_continue", sizeof( unsigned char), 35u));}{
void* _temp513=( void*)(( Cyc_Tcenv_get_fenv( te))->ctrl_env)->continue_stmt;
struct Cyc_Absyn_Stmt* _temp523; _LL515: if(( unsigned int) _temp513 > 3u?*((
int*) _temp513) == Cyc_Tcenv_Stmt_j: 0){ _LL524: _temp523=(( struct Cyc_Tcenv_Stmt_j_struct*)
_temp513)->f1; goto _LL516;} else{ goto _LL517;} _LL517: if( _temp513 ==( void*)
Cyc_Tcenv_NotLoop_j){ goto _LL518;} else{ goto _LL519;} _LL519: if( _temp513 ==(
void*) Cyc_Tcenv_CaseEnd_j){ goto _LL520;} else{ goto _LL521;} _LL521: if(
_temp513 ==( void*) Cyc_Tcenv_FnEnd_j){ goto _LL522;} else{ goto _LL514;} _LL516:
_temp523->non_local_preds=({ struct Cyc_List_List* _temp525=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp525->hd=( void*) s; _temp525->tl=
_temp523->non_local_preds; _temp525;});* sopt=( struct Cyc_Absyn_Stmt*) _temp523;
return; _LL518:({ void(* _temp526)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp529= s->loc; struct _tagged_arr _temp530= _tag_arr("continue not in a loop",
sizeof( unsigned char), 23u); void* _temp527[ 0u]={}; struct _tagged_arr
_temp528={( void*) _temp527,( void*) _temp527,( void*)( _temp527 + 0u)};
_temp526( _temp529, _temp530, _temp528);}); return; _LL520:(( int(*)( struct
_tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: continue dest is CaseEnd",
sizeof( unsigned char), 32u)); return; _LL522:(( int(*)( struct _tagged_arr msg))
Cyc_Tcenv_env_err)( _tag_arr("Tcenv: continue dest is CaseEnd", sizeof(
unsigned char), 32u)); return; _LL514:;}} void Cyc_Tcenv_process_break( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Stmt** sopt){ if(
te->le == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected process_break",
sizeof( unsigned char), 32u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_get_fenv(
te); void* _temp531=( void*)( fe->ctrl_env)->break_stmt; struct Cyc_Absyn_Stmt*
_temp541; _LL533: if(( unsigned int) _temp531 > 3u?*(( int*) _temp531) == Cyc_Tcenv_Stmt_j:
0){ _LL542: _temp541=(( struct Cyc_Tcenv_Stmt_j_struct*) _temp531)->f1; goto
_LL534;} else{ goto _LL535;} _LL535: if( _temp531 ==( void*) Cyc_Tcenv_NotLoop_j){
goto _LL536;} else{ goto _LL537;} _LL537: if( _temp531 ==( void*) Cyc_Tcenv_FnEnd_j){
goto _LL538;} else{ goto _LL539;} _LL539: if( _temp531 ==( void*) Cyc_Tcenv_CaseEnd_j){
goto _LL540;} else{ goto _LL532;} _LL534: _temp541->non_local_preds=({ struct
Cyc_List_List* _temp543=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp543->hd=( void*) s; _temp543->tl= _temp541->non_local_preds; _temp543;});*
sopt=( struct Cyc_Absyn_Stmt*) _temp541; return; _LL536:({ void(* _temp544)(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp547= s->loc; struct
_tagged_arr _temp548= _tag_arr("break not in a loop or switch", sizeof(
unsigned char), 30u); void* _temp545[ 0u]={}; struct _tagged_arr _temp546={(
void*) _temp545,( void*) _temp545,( void*)( _temp545 + 0u)}; _temp544( _temp547,
_temp548, _temp546);}); return; _LL538: if(( void*)( fe->shared)->return_typ !=(
void*) Cyc_Absyn_VoidType){({ void(* _temp549)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp552= s->loc; struct _tagged_arr _temp553= _tag_arr("break causes function not to return a value",
sizeof( unsigned char), 44u); void* _temp550[ 0u]={}; struct _tagged_arr
_temp551={( void*) _temp550,( void*) _temp550,( void*)( _temp550 + 0u)};
_temp549( _temp552, _temp553, _temp551);});} return; _LL540:({ void(* _temp554)(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp557= s->loc; struct
_tagged_arr _temp558= _tag_arr("break causes outer switch clause to implicitly fallthru",
sizeof( unsigned char), 56u); void* _temp555[ 0u]={}; struct _tagged_arr
_temp556={( void*) _temp555,( void*) _temp555,( void*)( _temp555 + 0u)};
_temp554( _temp557, _temp558, _temp556);}); return; _LL532:;}} void Cyc_Tcenv_process_goto(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct _tagged_arr* l,
struct Cyc_Absyn_Stmt** sopt){ if( te->le == 0){(( int(*)( struct _tagged_arr
msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected process_goto", sizeof(
unsigned char), 31u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_get_fenv( te);
struct Cyc_Core_Opt* _temp559=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)(( fe->shared)->seen_labels, l);
if( _temp559 == 0){ struct Cyc_SlowDict_Dict* _temp560=( fe->shared)->needed_labels;
struct Cyc_Core_Opt* _temp561=(( struct Cyc_Core_Opt*(*)( struct Cyc_SlowDict_Dict*
d, struct _tagged_arr* key)) Cyc_SlowDict_lookup_opt)( _temp560, l); if(
_temp561 == 0){ _temp561=({ struct Cyc_Core_Opt* _temp562=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp562->v=( void*) 0; _temp562;});}(
fe->shared)->needed_labels=(( struct Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict*
d, struct _tagged_arr* key, struct Cyc_List_List* data)) Cyc_SlowDict_insert)(
_temp560, l,({ struct Cyc_List_List* _temp563=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp563->hd=( void*) s; _temp563->tl=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp561))->v; _temp563;}));}
else{(( struct Cyc_Absyn_Stmt*)(( struct Cyc_Core_Opt*) _check_null( _temp559))->v)->non_local_preds=({
struct Cyc_List_List* _temp564=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp564->hd=( void*) s; _temp564->tl=(( struct Cyc_Absyn_Stmt*)((
struct Cyc_Core_Opt*) _check_null( _temp559))->v)->non_local_preds; _temp564;});*
sopt=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Stmt*)(( struct Cyc_Core_Opt*)
_check_null( _temp559))->v);}}} struct _tuple3* Cyc_Tcenv_process_fallthru(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Switch_clause***
clauseopt){ if( te->le == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected process_fallthru", sizeof( unsigned char), 35u));}{
struct Cyc_Tcenv_Fenv* _temp565= Cyc_Tcenv_get_fenv( te); struct _tuple3*
_temp566=( _temp565->ctrl_env)->fallthru_clause; if( _temp566 != 0){(((*((
struct _tuple3*) _check_null( _temp566))).f1)->body)->non_local_preds=({ struct
Cyc_List_List* _temp567=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp567->hd=( void*) s; _temp567->tl=(((*(( struct _tuple3*) _check_null(
_temp566))).f1)->body)->non_local_preds; _temp567;});* clauseopt=({ struct Cyc_Absyn_Switch_clause**
_temp568=( struct Cyc_Absyn_Switch_clause**) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause*)
* 1); _temp568[ 0]=(*(( struct _tuple3*) _check_null( _temp566))).f1; _temp568;});}
return _temp566;}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_fallthru( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* new_tvs, struct Cyc_List_List* vds, struct Cyc_Absyn_Switch_clause*
clause){ if( te->le == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected set_fallthru", sizeof( unsigned char), 31u));}{
struct Cyc_List_List* _temp569= 0; for( 0; vds != 0; vds=(( struct Cyc_List_List*)
_check_null( vds))->tl){ _temp569=({ struct Cyc_List_List* _temp570=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp570->hd=( void*)(( void*)((
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( vds))->hd)->type);
_temp570->tl= _temp569; _temp570;});} _temp569= Cyc_List_imp_rev( _temp569);{
struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));(
fe->ctrl_env)->fallthru_clause=({ struct _tuple3* _temp571=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp571->f1= clause; _temp571->f2= new_tvs;
_temp571->f3= _temp569; _temp571;}); return Cyc_Tcenv_put_fenv( te, fe);}}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_clear_fallthru( struct Cyc_Tcenv_Tenv* te){ if(
te->le == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected clear_fallthru",
sizeof( unsigned char), 33u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te));( fe->ctrl_env)->fallthru_clause= 0; return Cyc_Tcenv_put_fenv(
te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_loop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* continue_dest){ if( te->le == 0){(( int(*)( struct
_tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected set_in_loop",
sizeof( unsigned char), 30u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->continue_stmt=( void*)(( void*)({
struct Cyc_Tcenv_Stmt_j_struct* _temp572=( struct Cyc_Tcenv_Stmt_j_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp572[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct
_temp573; _temp573.tag= Cyc_Tcenv_Stmt_j; _temp573.f1= continue_dest; _temp573;});
_temp572;})));( void*)(( fe->ctrl_env)->break_stmt=( void*)(( void*)( fe->ctrl_env)->next_stmt));(
void*)(( fe->ctrl_env)->next_stmt=( void*)(( void*)({ struct Cyc_Tcenv_Stmt_j_struct*
_temp574=( struct Cyc_Tcenv_Stmt_j_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct));
_temp574[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct _temp575; _temp575.tag= Cyc_Tcenv_Stmt_j;
_temp575.f1= continue_dest; _temp575;}); _temp574;}))); return Cyc_Tcenv_put_fenv(
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
sizeof( unsigned char), 28u));}{ struct Cyc_Tcenv_Fenv* _temp576= Cyc_Tcenv_get_fenv(
te); struct Cyc_SlowDict_Dict* _temp577=( _temp576->shared)->needed_labels;
struct Cyc_Core_Opt* _temp578=(( struct Cyc_Core_Opt*(*)( struct Cyc_SlowDict_Dict*
d, struct _tagged_arr* key)) Cyc_SlowDict_lookup_opt)( _temp577, v); if(
_temp578 != 0){( _temp576->shared)->needed_labels=(( struct Cyc_SlowDict_Dict*(*)(
struct Cyc_SlowDict_Dict* d, struct _tagged_arr* key)) Cyc_SlowDict_delete)(
_temp577, v);{ struct Cyc_List_List* _temp579=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( _temp578))->v; s->non_local_preds= _temp579; for( 0;
_temp579 != 0; _temp579=(( struct Cyc_List_List*) _check_null( _temp579))->tl){
void* _temp580=( void*)(( struct Cyc_Absyn_Stmt*)(( struct Cyc_List_List*)
_check_null( _temp579))->hd)->r; struct Cyc_Absyn_Stmt* _temp586; struct Cyc_Absyn_Stmt**
_temp588; _LL582: if(( unsigned int) _temp580 > 1u?*(( int*) _temp580) == Cyc_Absyn_Goto_s:
0){ _LL587: _temp586=(( struct Cyc_Absyn_Goto_s_struct*) _temp580)->f2; _temp588=&((
struct Cyc_Absyn_Goto_s_struct*) _temp580)->f2; goto _LL583;} else{ goto _LL584;}
_LL584: goto _LL585; _LL583:* _temp588=( struct Cyc_Absyn_Stmt*) s; goto _LL581;
_LL585:(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: add_label backpatching of non-goto",
sizeof( unsigned char), 42u)); goto _LL581; _LL581:;}}} if((( int(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key)) Cyc_Dict_member)(( _temp576->shared)->seen_labels,
v)){({ void(* _temp589)( struct Cyc_Position_Segment*, struct _tagged_arr fmt,
struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp593=
s->loc; struct _tagged_arr _temp594= _tag_arr("Repeated label: %s", sizeof(
unsigned char), 19u); struct Cyc_Stdio_String_pa_struct _temp595; _temp595.tag=
Cyc_Stdio_String_pa; _temp595.f1=( struct _tagged_arr)* v;{ void* _temp592=(
void*)& _temp595; void* _temp590[ 1u]={ _temp592}; struct _tagged_arr _temp591={(
void*) _temp590,( void*) _temp590,( void*)( _temp590 + 1u)}; _temp589( _temp593,
_temp594, _temp591);}});}( _temp576->shared)->seen_labels=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Stmt* data))
Cyc_Dict_insert)(( _temp576->shared)->seen_labels, v, s); return te;}} int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv* te){ return(( int(*)( struct Cyc_SlowDict_Dict* d)) Cyc_SlowDict_is_empty)(((
Cyc_Tcenv_get_fenv( te))->shared)->needed_labels);} struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( int(*)( struct _tagged_arr msg))
Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected get_encloser", sizeof(
unsigned char), 31u));} return( Cyc_Tcenv_get_fenv( te))->encloser;} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s){
if( te->le == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected set_encloser", sizeof( unsigned char), 31u));}{
struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl( Cyc_Tcenv_get_fenv( te));
fe->encloser= s; return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_region( struct Cyc_Tcenv_Tenv* te, void* r){ if( te->le == 0){((
int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv::add_region: unexpected add_region",
sizeof( unsigned char), 41u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); struct Cyc_Absyn_Tvar* tv;{ void* _temp596= Cyc_Tcutil_compress(
r); struct Cyc_Absyn_Tvar* _temp602; _LL598: if(( unsigned int) _temp596 > 4u?*((
int*) _temp596) == Cyc_Absyn_VarType: 0){ _LL603: _temp602=(( struct Cyc_Absyn_VarType_struct*)
_temp596)->f1; goto _LL599;} else{ goto _LL600;} _LL600: goto _LL601; _LL599: tv=
_temp602; goto _LL597; _LL601: tv=(( struct Cyc_Absyn_Tvar*(*)( struct
_tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("bad add region", sizeof(
unsigned char), 15u)); goto _LL597; _LL597:;} fe->region_order= Cyc_Tcenv_add_region_po(
fe->region_order, tv);( void*)( fe->capability=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp604=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp604[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp605; _temp605.tag= Cyc_Absyn_JoinEff;
_temp605.f1=({ struct Cyc_List_List* _temp606=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp606->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp608=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp608[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp609; _temp609.tag= Cyc_Absyn_AccessEff;
_temp609.f1=( void*) r; _temp609;}); _temp608;})); _temp606->tl=({ struct Cyc_List_List*
_temp607=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp607->hd=( void*)(( void*) fe->capability); _temp607->tl= 0; _temp607;});
_temp606;}); _temp605;}); _temp604;}))); return Cyc_Tcenv_put_fenv( te, fe);}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_named_block( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Tvar* block_rgn){ if( te->le ==
0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected new_block",
sizeof( unsigned char), 28u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); void* block_typ=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp617=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp617[ 0]=({ struct Cyc_Absyn_VarType_struct _temp618; _temp618.tag= Cyc_Absyn_VarType;
_temp618.f1= block_rgn; _temp618;}); _temp617;}); fe->type_vars=({ struct Cyc_List_List*
_temp610=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp610->hd=( void*) block_rgn; _temp610->tl= fe->type_vars; _temp610;}); Cyc_Tcutil_check_unique_tvars(
loc, fe->type_vars); fe->region_order= Cyc_Tcenv_add_region_po( fe->region_order,
block_rgn);( void*)( fe->capability=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp611=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp611[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp612; _temp612.tag= Cyc_Absyn_JoinEff;
_temp612.f1=({ struct Cyc_List_List* _temp613=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp613->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp615=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp615[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp616; _temp616.tag= Cyc_Absyn_AccessEff;
_temp616.f1=( void*) block_typ; _temp616;}); _temp615;})); _temp613->tl=({
struct Cyc_List_List* _temp614=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp614->hd=( void*)(( void*) fe->capability); _temp614->tl=
0; _temp614;}); _temp613;}); _temp612;}); _temp611;})));( void*)( fe->curr_rgn=(
void*) block_typ); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_block( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te){
struct Cyc_Absyn_Tvar* t= Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_RgnKind); Cyc_Tcutil_add_tvar_identity(
t); return Cyc_Tcenv_new_named_block( loc, te, t);} void* Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){ return( void*) Cyc_Absyn_HeapRgn;}
return( void*)( Cyc_Tcenv_get_fenv( te))->curr_rgn;} void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* rgn){ if( te->le
!= 0){ void* capability=( void*)( Cyc_Tcenv_get_fenv( te))->capability; if( Cyc_Tcutil_region_in_effect(
0, rgn, capability)? 1: Cyc_Tcutil_region_in_effect( 1, rgn, capability)){
return;}}({ void(* _temp619)( struct Cyc_Position_Segment*, struct _tagged_arr
fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp623= loc; struct _tagged_arr _temp624= _tag_arr("Expression accesses unavailable region %s",
sizeof( unsigned char), 42u); struct Cyc_Stdio_String_pa_struct _temp625;
_temp625.tag= Cyc_Stdio_String_pa; _temp625.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
rgn);{ void* _temp622=( void*)& _temp625; void* _temp620[ 1u]={ _temp622};
struct _tagged_arr _temp621={( void*) _temp620,( void*) _temp620,( void*)(
_temp620 + 1u)}; _temp619( _temp623, _temp624, _temp621);}});} int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv* te, void* rt_a, void* rt_b){ if( rt_a ==( void*) Cyc_Absyn_HeapRgn){
return 1;} if( te->le != 0){ struct _tuple5 _temp627=({ struct _tuple5 _temp626;
_temp626.f1= Cyc_Tcutil_compress( rt_a); _temp626.f2= Cyc_Tcutil_compress( rt_b);
_temp626;}); void* _temp633; struct Cyc_Absyn_Tvar* _temp635; void* _temp637;
struct Cyc_Absyn_Tvar* _temp639; _LL629: _LL638: _temp637= _temp627.f1; if((
unsigned int) _temp637 > 4u?*(( int*) _temp637) == Cyc_Absyn_VarType: 0){ _LL640:
_temp639=(( struct Cyc_Absyn_VarType_struct*) _temp637)->f1; goto _LL634;} else{
goto _LL631;} _LL634: _temp633= _temp627.f2; if(( unsigned int) _temp633 > 4u?*((
int*) _temp633) == Cyc_Absyn_VarType: 0){ _LL636: _temp635=(( struct Cyc_Absyn_VarType_struct*)
_temp633)->f1; goto _LL630;} else{ goto _LL631;} _LL631: goto _LL632; _LL630:
return Cyc_Tcenv_check_region_outlives(( Cyc_Tcenv_get_fenv( te))->region_order,
_temp639, _temp635); _LL632: return 0; _LL628:;} return 0;} void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* eff){ void*
capability; if( te->le == 0){ capability=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp641=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp641[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp642; _temp642.tag= Cyc_Absyn_JoinEff;
_temp642.f1= 0; _temp642;}); _temp641;});} else{ struct Cyc_Tcenv_Fenv* _temp643=
Cyc_Tcenv_get_fenv( te); capability=( void*) _temp643->capability;} if( ! Cyc_Tcutil_subset_effect(
1, eff, capability)){({ void(* _temp644)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp649= loc; struct _tagged_arr _temp650= _tag_arr("Capability %s does not cover function's effect %s",
sizeof( unsigned char), 50u); struct Cyc_Stdio_String_pa_struct _temp652;
_temp652.tag= Cyc_Stdio_String_pa; _temp652.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
capability);{ void* _temp647=( void*)& _temp652; struct Cyc_Stdio_String_pa_struct
_temp651; _temp651.tag= Cyc_Stdio_String_pa; _temp651.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( eff);{ void* _temp648=( void*)& _temp651; void* _temp645[
2u]={ _temp647, _temp648}; struct _tagged_arr _temp646={( void*) _temp645,( void*)
_temp645,( void*)( _temp645 + 2u)}; _temp644( _temp649, _temp650, _temp646);}}});}}
void Cyc_Tcenv_check_rgn_partial_order( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* po){ for( 0; po != 0; po=(( struct Cyc_List_List*)
_check_null( po))->tl){ struct _tuple5 _temp655; void* _temp656; void* _temp658;
struct _tuple5* _temp653=( struct _tuple5*)(( struct Cyc_List_List*) _check_null(
po))->hd; _temp655=* _temp653; _LL659: _temp658= _temp655.f1; goto _LL657;
_LL657: _temp656= _temp655.f2; goto _LL654; _LL654: if( ! Cyc_Tcenv_region_outlives(
te, _temp658, _temp656)){({ void(* _temp660)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp665= loc; struct _tagged_arr _temp666= _tag_arr("function requires that region %s outlive %s",
sizeof( unsigned char), 44u); struct Cyc_Stdio_String_pa_struct _temp668;
_temp668.tag= Cyc_Stdio_String_pa; _temp668.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp658);{ void* _temp663=( void*)& _temp668; struct Cyc_Stdio_String_pa_struct
_temp667; _temp667.tag= Cyc_Stdio_String_pa; _temp667.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( _temp656);{ void* _temp664=( void*)& _temp667; void*
_temp661[ 2u]={ _temp663, _temp664}; struct _tagged_arr _temp662={( void*)
_temp661,( void*) _temp661,( void*)( _temp661 + 2u)}; _temp660( _temp665,
_temp666, _temp662);}}});}}} struct _tuple6{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual
f2; void* f3; } ; struct Cyc_Tcenv_Fenv* Cyc_Tcenv_new_fenv( struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Fndecl* fd){ struct Cyc_Dict_Dict* locals=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
struct Cyc_Absyn_Tvar* rgn0=({ struct Cyc_Absyn_Tvar* _temp726=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp726->name=({ struct _tagged_arr*
_temp727=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr));
_temp727[ 0]=( struct _tagged_arr)({ struct _tagged_arr(* _temp728)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp732= _tag_arr("`%s", sizeof( unsigned char), 4u); struct Cyc_Stdio_String_pa_struct
_temp733; _temp733.tag= Cyc_Stdio_String_pa; _temp733.f1=( struct _tagged_arr)*(*
fd->name).f2;{ void* _temp731=( void*)& _temp733; void* _temp729[ 1u]={ _temp731};
struct _tagged_arr _temp730={( void*) _temp729,( void*) _temp729,( void*)(
_temp729 + 1u)}; _temp728( _temp732, _temp730);}}); _temp727;}); _temp726->identity=
Cyc_Tcutil_new_tvar_id(); _temp726->kind= Cyc_Absyn_new_conref(( void*) Cyc_Absyn_RgnKind);
_temp726;}); struct Cyc_List_List* _temp669=({ struct Cyc_List_List* _temp725=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp725->hd=(
void*) rgn0; _temp725->tl= fd->tvs; _temp725;}); Cyc_Tcutil_check_unique_tvars(
loc, _temp669);{ struct Cyc_List_List* _temp670= Cyc_Tcenv_add_region_po( Cyc_Tcenv_initial_region_po(
loc, fd->tvs, fd->rgn_po), rgn0); void* param_rgn=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp723=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp723[ 0]=({ struct Cyc_Absyn_VarType_struct _temp724; _temp724.tag= Cyc_Absyn_VarType;
_temp724.f1= rgn0; _temp724;}); _temp723;}); struct Cyc_List_List* _temp671= 0;{
struct Cyc_List_List* _temp672= fd->args; for( 0; _temp672 != 0; _temp672=((
struct Cyc_List_List*) _check_null( _temp672))->tl){ struct Cyc_Absyn_Vardecl*
_temp673=({ struct Cyc_Absyn_Vardecl* _temp678=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl) * 1); _temp678[ 0]=({ struct Cyc_Absyn_Vardecl
_temp679; _temp679.sc=( void*)(( void*) Cyc_Absyn_Public); _temp679.name=({
struct _tuple0* _temp682=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0) *
1); _temp682[ 0]=({ struct _tuple0 _temp683; _temp683.f1=( void*) Cyc_Absyn_Loc_n;
_temp683.f2=(*(( struct _tuple6*)(( struct Cyc_List_List*) _check_null( _temp672))->hd)).f1;
_temp683;}); _temp682;}); _temp679.tq=(*(( struct _tuple6*)(( struct Cyc_List_List*)
_check_null( _temp672))->hd)).f2; _temp679.type=( void*)(*(( struct _tuple6*)((
struct Cyc_List_List*) _check_null( _temp672))->hd)).f3; _temp679.initializer= 0;
_temp679.rgn=({ struct Cyc_Core_Opt* _temp680=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt) * 1); _temp680[ 0]=({ struct Cyc_Core_Opt _temp681;
_temp681.v=( void*) param_rgn; _temp681;}); _temp680;}); _temp679.attributes= 0;
_temp679;}); _temp678;}); _temp671=({ struct Cyc_List_List* _temp674=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp674[ 0]=({
struct Cyc_List_List _temp675; _temp675.hd=( void*) _temp673; _temp675.tl=
_temp671; _temp675;}); _temp674;}); locals=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key, void* data)) Cyc_Dict_insert)( locals,(*((
struct _tuple6*)(( struct Cyc_List_List*) _check_null( _temp672))->hd)).f1,(
void*)({ struct Cyc_Absyn_Param_b_struct* _temp676=( struct Cyc_Absyn_Param_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Param_b_struct)); _temp676[ 0]=({ struct Cyc_Absyn_Param_b_struct
_temp677; _temp677.tag= Cyc_Absyn_Param_b; _temp677.f1= _temp673; _temp677;});
_temp676;}));}} if( fd->cyc_varargs != 0){ int _temp686; void* _temp688; void*
_temp690; struct Cyc_Absyn_Tqual _temp692; struct Cyc_Core_Opt* _temp694; struct
Cyc_Absyn_VarargInfo _temp684=*(( struct Cyc_Absyn_VarargInfo*) _check_null( fd->cyc_varargs));
_LL695: _temp694= _temp684.name; goto _LL693; _LL693: _temp692= _temp684.tq;
goto _LL691; _LL691: _temp690=( void*) _temp684.type; goto _LL689; _LL689:
_temp688=( void*) _temp684.rgn; goto _LL687; _LL687: _temp686= _temp684.inject;
goto _LL685; _LL685: if( _temp694 != 0){ void* _temp696= Cyc_Absyn_tagged_typ(
_temp690, _temp688, _temp692); struct Cyc_Absyn_Vardecl* _temp697=({ struct Cyc_Absyn_Vardecl*
_temp701=( struct Cyc_Absyn_Vardecl*) GC_malloc( sizeof( struct Cyc_Absyn_Vardecl)
* 1); _temp701[ 0]=({ struct Cyc_Absyn_Vardecl _temp702; _temp702.sc=( void*)((
void*) Cyc_Absyn_Public); _temp702.name=({ struct _tuple0* _temp705=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0) * 1); _temp705[ 0]=({ struct
_tuple0 _temp706; _temp706.f1=( void*) Cyc_Absyn_Loc_n; _temp706.f2=( struct
_tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( _temp694))->v; _temp706;});
_temp705;}); _temp702.tq= Cyc_Absyn_empty_tqual(); _temp702.type=( void*)
_temp696; _temp702.initializer= 0; _temp702.rgn=({ struct Cyc_Core_Opt* _temp703=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1); _temp703[ 0]=({
struct Cyc_Core_Opt _temp704; _temp704.v=( void*) param_rgn; _temp704;});
_temp703;}); _temp702.attributes= 0; _temp702;}); _temp701;}); _temp671=({
struct Cyc_List_List* _temp698=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp698->hd=( void*) _temp697; _temp698->tl= _temp671;
_temp698;}); locals=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key, void* data)) Cyc_Dict_insert)( locals,( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( _temp694))->v,( void*)({ struct Cyc_Absyn_Param_b_struct*
_temp699=( struct Cyc_Absyn_Param_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Param_b_struct));
_temp699[ 0]=({ struct Cyc_Absyn_Param_b_struct _temp700; _temp700.tag= Cyc_Absyn_Param_b;
_temp700.f1= _temp697; _temp700;}); _temp699;}));} else{({ void(* _temp707)(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp710= loc; struct _tagged_arr
_temp711= _tag_arr("missing name for varargs", sizeof( unsigned char), 25u);
void* _temp708[ 0u]={}; struct _tagged_arr _temp709={( void*) _temp708,( void*)
_temp708,( void*)( _temp708 + 0u)}; _temp707( _temp710, _temp711, _temp709);});}}
fd->param_vardecls=({ struct Cyc_Core_Opt* _temp712=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt) * 1); _temp712[ 0]=({ struct Cyc_Core_Opt
_temp713; _temp713.v=( void*) _temp671; _temp713;}); _temp712;}); return({
struct Cyc_Tcenv_Fenv* _temp714=( struct Cyc_Tcenv_Fenv*) GC_malloc( sizeof(
struct Cyc_Tcenv_Fenv)); _temp714->shared=({ struct Cyc_Tcenv_SharedFenv*
_temp722=( struct Cyc_Tcenv_SharedFenv*) GC_malloc( sizeof( struct Cyc_Tcenv_SharedFenv));
_temp722->return_typ=( void*)(( void*) fd->ret_type); _temp722->seen_labels=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp722->needed_labels=(( struct Cyc_SlowDict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_SlowDict_empty)(
Cyc_String_zstrptrcmp); _temp722;}); _temp714->type_vars= _temp669; _temp714->region_order=
_temp670; _temp714->locals= locals; _temp714->encloser= fd->body; _temp714->ctrl_env=({
struct Cyc_Tcenv_CtrlEnv* _temp721=( struct Cyc_Tcenv_CtrlEnv*) GC_malloc(
sizeof( struct Cyc_Tcenv_CtrlEnv)); _temp721->continue_stmt=( void*)(( void*)
Cyc_Tcenv_NotLoop_j); _temp721->break_stmt=( void*)(( void*) Cyc_Tcenv_NotLoop_j);
_temp721->fallthru_clause= 0; _temp721->next_stmt=( void*)(( void*) Cyc_Tcenv_FnEnd_j);
_temp721->try_depth= 0; _temp721;}); _temp714->capability=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp715=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp715[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp716; _temp716.tag= Cyc_Absyn_JoinEff; _temp716.f1=({ struct Cyc_List_List*
_temp717=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp717->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp719=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp719[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp720; _temp720.tag= Cyc_Absyn_AccessEff;
_temp720.f1=( void*) param_rgn; _temp720;}); _temp719;})); _temp717->tl=({
struct Cyc_List_List* _temp718=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp718->hd=( void*)(( void*)(( struct Cyc_Core_Opt*)
_check_null( fd->effect))->v); _temp718->tl= 0; _temp718;}); _temp717;});
_temp716;}); _temp715;})); _temp714->curr_rgn=( void*) param_rgn; _temp714;});}}