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
21; static const int Cyc_Absyn_Aprintf= 22; static const int Cyc_Absyn_Scanf= 23;
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
struct _tagged_arr); extern struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set;
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
_temp20= 1;} if( ! _temp20){ lookup( ge2, v); Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp21)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp25= _tag_arr("%s is ambiguous",
sizeof( unsigned char), 16u); struct Cyc_Stdio_String_pa_struct _temp26; _temp26.tag=
Cyc_Stdio_String_pa; _temp26.f1=( struct _tagged_arr)* v;{ void* _temp24=( void*)&
_temp26; void* _temp22[ 1u]={ _temp24}; struct _tagged_arr _temp23={( void*)
_temp22,( void*) _temp22,( void*)( _temp22 + 1u)}; _temp21( _temp25, _temp23);}}));;
_pop_handler();} else{ void* _temp19=( void*) _exn_thrown; void* _temp28=
_temp19; _LL30: if( _temp28 == Cyc_Dict_Absent){ goto _LL31;} else{ goto _LL32;}
_LL32: goto _LL33; _LL31: goto _LL29; _LL33:( void) _throw( _temp28); _LL29:;}}}
return;} static void* Cyc_Tcenv_scoped_lookup( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void*(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_arr*), struct _tagged_arr* v){ struct Cyc_List_List* cns= te->ns; while(
1) { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, cns);{ struct
_handler_cons _temp34; _push_handler(& _temp34);{ int _temp36= 0; if( setjmp(
_temp34.handler)){ _temp36= 1;} if( ! _temp36){{ void* result= lookup( ge, v);
Cyc_Tcenv_check_repeat( te, loc, lookup, v, ge->availables);{ void* _temp37=
result; _npop_handler( 0u); return _temp37;}}; _pop_handler();} else{ void*
_temp35=( void*) _exn_thrown; void* _temp39= _temp35; _LL41: if( _temp39 == Cyc_Dict_Absent){
goto _LL42;} else{ goto _LL43;} _LL43: goto _LL44; _LL42: goto _LL40; _LL44:(
void) _throw( _temp39); _LL40:;}}}{ struct Cyc_List_List* _temp45= ge->availables;
for( 0; _temp45 != 0; _temp45=(( struct Cyc_List_List*) _check_null( _temp45))->tl){
struct Cyc_Tcenv_Genv* ge2=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,( struct Cyc_List_List*)((
struct Cyc_List_List*) _check_null( _temp45))->hd); struct _handler_cons _temp46;
_push_handler(& _temp46);{ int _temp48= 0; if( setjmp( _temp46.handler)){
_temp48= 1;} if( ! _temp48){{ void* result= lookup( ge2, v); Cyc_Tcenv_check_repeat(
te, loc, lookup, v,(( struct Cyc_List_List*) _check_null( _temp45))->tl);{ void*
_temp49= result; _npop_handler( 0u); return _temp49;}}; _pop_handler();} else{
void* _temp47=( void*) _exn_thrown; void* _temp51= _temp47; _LL53: if( _temp51
== Cyc_Dict_Absent){ goto _LL54;} else{ goto _LL55;} _LL55: goto _LL56; _LL54:
goto _LL52; _LL56:( void) _throw( _temp51); _LL52:;}}}} if( cns == 0){( int)
_throw(( void*) Cyc_Dict_Absent);} cns= Cyc_Tcenv_outer_namespace( cns);}}
static void* Cyc_Tcenv_lookup_ordinary_global_f( struct Cyc_Tcenv_Genv* ge,
struct _tagged_arr* v){ struct _tuple4* ans=(( struct _tuple4*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->ordinaries, v);(* ans).f2= 1;
return(* ans).f1;} static void* Cyc_Tcenv_lookup_ordinary_global( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp59;
struct _tagged_arr* _temp60; void* _temp62; struct _tuple0* _temp57= q; _temp59=*
_temp57; _LL63: _temp62= _temp59.f1; goto _LL61; _LL61: _temp60= _temp59.f2;
goto _LL58; _LL58: { void* _temp64= _temp62; struct Cyc_List_List* _temp74;
struct Cyc_List_List* _temp76; struct Cyc_List_List _temp78; struct Cyc_List_List*
_temp79; struct _tagged_arr* _temp81; struct Cyc_List_List* _temp83; _LL66: if(
_temp64 ==( void*) Cyc_Absyn_Loc_n){ goto _LL67;} else{ goto _LL68;} _LL68: if((
unsigned int) _temp64 > 1u?*(( int*) _temp64) == Cyc_Absyn_Rel_n: 0){ _LL75:
_temp74=(( struct Cyc_Absyn_Rel_n_struct*) _temp64)->f1; if( _temp74 == 0){ goto
_LL69;} else{ goto _LL70;}} else{ goto _LL70;} _LL70: if(( unsigned int) _temp64
> 1u?*(( int*) _temp64) == Cyc_Absyn_Rel_n: 0){ _LL77: _temp76=(( struct Cyc_Absyn_Rel_n_struct*)
_temp64)->f1; if( _temp76 == 0){ goto _LL72;} else{ _temp78=* _temp76; _LL82:
_temp81=( struct _tagged_arr*) _temp78.hd; goto _LL80; _LL80: _temp79= _temp78.tl;
goto _LL71;}} else{ goto _LL72;} _LL72: if(( unsigned int) _temp64 > 1u?*(( int*)
_temp64) == Cyc_Absyn_Abs_n: 0){ _LL84: _temp83=(( struct Cyc_Absyn_Abs_n_struct*)
_temp64)->f1; goto _LL73;} else{ goto _LL65;} _LL67: goto _LL69; _LL69: return
Cyc_Tcenv_scoped_lookup( te, loc, Cyc_Tcenv_lookup_ordinary_global_f, _temp60);
_LL71: { struct Cyc_Tcenv_Genv* _temp85= Cyc_Tcenv_lookup_namespace( te, loc,
_temp81, _temp79); return Cyc_Tcenv_lookup_ordinary_global_f( _temp85, _temp60);}
_LL73: return Cyc_Tcenv_lookup_ordinary_global_f((( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp83), _temp60); _LL65:;}} struct Cyc_List_List* Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tagged_arr*
n, struct Cyc_List_List* ns){ struct Cyc_List_List* _temp86= te->ns; struct Cyc_List_List*
_temp87= 0; while( 1) { struct Cyc_Tcenv_Genv* _temp88=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp86);{ struct Cyc_List_List* _temp89= _temp88->availables; for( 0; _temp89
!= 0; _temp89=(( struct Cyc_List_List*) _check_null( _temp89))->tl){ struct Cyc_Tcenv_Genv*
ge2=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( te->ae,( struct Cyc_List_List*)(( struct Cyc_List_List*)
_check_null( _temp89))->hd); if((( int(*)( struct Cyc_Set_Set* s, struct
_tagged_arr* elt)) Cyc_Set_member)( ge2->namespaces, n)){ _temp87=({ struct Cyc_List_List*
_temp90=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp90->hd=( void*)(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( _temp86,({ struct Cyc_List_List* _temp91=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp91->hd=(
void*) n; _temp91->tl= ns; _temp91;})); _temp90->tl= _temp87; _temp90;});}}} if(((
int(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_member)(
_temp88->namespaces, n)){ _temp87=({ struct Cyc_List_List* _temp92=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp92->hd=( void*)(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp86,({
struct Cyc_List_List* _temp93=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp93->hd=( void*) n; _temp93->tl= ns; _temp93;})); _temp92->tl=
_temp87; _temp92;});} if( _temp87 != 0){ if((( struct Cyc_List_List*)
_check_null( _temp87))->tl != 0){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr(* _temp94)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp98= _tag_arr("%s is ambiguous", sizeof( unsigned char),
16u); struct Cyc_Stdio_String_pa_struct _temp99; _temp99.tag= Cyc_Stdio_String_pa;
_temp99.f1=( struct _tagged_arr)* n;{ void* _temp97=( void*)& _temp99; void*
_temp95[ 1u]={ _temp97}; struct _tagged_arr _temp96={( void*) _temp95,( void*)
_temp95,( void*)( _temp95 + 1u)}; _temp94( _temp98, _temp96);}}));} return(
struct Cyc_List_List*)(( struct Cyc_List_List*) _check_null( _temp87))->hd;} if(
_temp86 == 0){( int) _throw(( void*) Cyc_Dict_Absent);} _temp86= Cyc_Tcenv_outer_namespace(
_temp86);}} static struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Structdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->structdecls,
v);} struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp102; struct _tagged_arr* _temp103; void* _temp105; struct _tuple0* _temp100=
q; _temp102=* _temp100; _LL106: _temp105= _temp102.f1; goto _LL104; _LL104:
_temp103= _temp102.f2; goto _LL101; _LL101: { void* _temp107= _temp105; struct
Cyc_List_List* _temp117; struct Cyc_List_List* _temp119; struct Cyc_List_List*
_temp121; struct Cyc_List_List _temp123; struct Cyc_List_List* _temp124; struct
_tagged_arr* _temp126; _LL109: if( _temp107 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL110;} else{ goto _LL111;} _LL111: if(( unsigned int) _temp107 > 1u?*(( int*)
_temp107) == Cyc_Absyn_Rel_n: 0){ _LL118: _temp117=(( struct Cyc_Absyn_Rel_n_struct*)
_temp107)->f1; if( _temp117 == 0){ goto _LL112;} else{ goto _LL113;}} else{ goto
_LL113;} _LL113: if(( unsigned int) _temp107 > 1u?*(( int*) _temp107) == Cyc_Absyn_Abs_n:
0){ _LL120: _temp119=(( struct Cyc_Absyn_Abs_n_struct*) _temp107)->f1; goto
_LL114;} else{ goto _LL115;} _LL115: if(( unsigned int) _temp107 > 1u?*(( int*)
_temp107) == Cyc_Absyn_Rel_n: 0){ _LL122: _temp121=(( struct Cyc_Absyn_Rel_n_struct*)
_temp107)->f1; if( _temp121 == 0){ goto _LL108;} else{ _temp123=* _temp121;
_LL127: _temp126=( struct _tagged_arr*) _temp123.hd; goto _LL125; _LL125:
_temp124= _temp123.tl; goto _LL116;}} else{ goto _LL108;} _LL110: goto _LL112;
_LL112: return(( struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl**(* lookup)(
struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v)) Cyc_Tcenv_scoped_lookup)(
te, loc, Cyc_Tcenv_lookup_structdecl_f, _temp103); _LL114: { struct Cyc_Tcenv_Genv*
_temp128=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( te->ae, _temp119); return(( struct Cyc_Absyn_Structdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp128->structdecls,
_temp103);} _LL116: { struct Cyc_Tcenv_Genv* _temp129= Cyc_Tcenv_lookup_namespace(
te, loc, _temp126, _temp124); return(( struct Cyc_Absyn_Structdecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp129->structdecls,
_temp103);} _LL108:;}} static struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Uniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->uniondecls,
v);} struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp132; struct _tagged_arr* _temp133; void* _temp135; struct _tuple0* _temp130=
q; _temp132=* _temp130; _LL136: _temp135= _temp132.f1; goto _LL134; _LL134:
_temp133= _temp132.f2; goto _LL131; _LL131: { void* _temp137= _temp135; struct
Cyc_List_List* _temp147; struct Cyc_List_List* _temp149; struct Cyc_List_List*
_temp151; struct Cyc_List_List _temp153; struct Cyc_List_List* _temp154; struct
_tagged_arr* _temp156; _LL139: if( _temp137 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL140;} else{ goto _LL141;} _LL141: if(( unsigned int) _temp137 > 1u?*(( int*)
_temp137) == Cyc_Absyn_Rel_n: 0){ _LL148: _temp147=(( struct Cyc_Absyn_Rel_n_struct*)
_temp137)->f1; if( _temp147 == 0){ goto _LL142;} else{ goto _LL143;}} else{ goto
_LL143;} _LL143: if(( unsigned int) _temp137 > 1u?*(( int*) _temp137) == Cyc_Absyn_Abs_n:
0){ _LL150: _temp149=(( struct Cyc_Absyn_Abs_n_struct*) _temp137)->f1; goto
_LL144;} else{ goto _LL145;} _LL145: if(( unsigned int) _temp137 > 1u?*(( int*)
_temp137) == Cyc_Absyn_Rel_n: 0){ _LL152: _temp151=(( struct Cyc_Absyn_Rel_n_struct*)
_temp137)->f1; if( _temp151 == 0){ goto _LL138;} else{ _temp153=* _temp151;
_LL157: _temp156=( struct _tagged_arr*) _temp153.hd; goto _LL155; _LL155:
_temp154= _temp153.tl; goto _LL146;}} else{ goto _LL138;} _LL140: goto _LL142;
_LL142: return(( struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl**(* lookup)( struct
Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v)) Cyc_Tcenv_scoped_lookup)(
te, loc, Cyc_Tcenv_lookup_uniondecl_f, _temp133); _LL144: { struct Cyc_Tcenv_Genv*
_temp158=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( te->ae, _temp149); return(( struct Cyc_Absyn_Uniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp158->uniondecls,
_temp133);} _LL146: { struct Cyc_Tcenv_Genv* _temp159= Cyc_Tcenv_lookup_namespace(
te, loc, _temp156, _temp154); return(( struct Cyc_Absyn_Uniondecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp159->uniondecls,
_temp133);} _LL138:;}} static struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->tuniondecls,
v);} struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp162; struct _tagged_arr* _temp163; void* _temp165; struct _tuple0* _temp160=
q; _temp162=* _temp160; _LL166: _temp165= _temp162.f1; goto _LL164; _LL164:
_temp163= _temp162.f2; goto _LL161; _LL161: { void* _temp167= _temp165; struct
Cyc_List_List* _temp177; struct Cyc_List_List* _temp179; struct Cyc_List_List
_temp181; struct Cyc_List_List* _temp182; struct _tagged_arr* _temp184; struct
Cyc_List_List* _temp186; _LL169: if( _temp167 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL170;} else{ goto _LL171;} _LL171: if(( unsigned int) _temp167 > 1u?*(( int*)
_temp167) == Cyc_Absyn_Rel_n: 0){ _LL178: _temp177=(( struct Cyc_Absyn_Rel_n_struct*)
_temp167)->f1; if( _temp177 == 0){ goto _LL172;} else{ goto _LL173;}} else{ goto
_LL173;} _LL173: if(( unsigned int) _temp167 > 1u?*(( int*) _temp167) == Cyc_Absyn_Rel_n:
0){ _LL180: _temp179=(( struct Cyc_Absyn_Rel_n_struct*) _temp167)->f1; if(
_temp179 == 0){ goto _LL175;} else{ _temp181=* _temp179; _LL185: _temp184=(
struct _tagged_arr*) _temp181.hd; goto _LL183; _LL183: _temp182= _temp181.tl;
goto _LL174;}} else{ goto _LL175;} _LL175: if(( unsigned int) _temp167 > 1u?*((
int*) _temp167) == Cyc_Absyn_Abs_n: 0){ _LL187: _temp186=(( struct Cyc_Absyn_Abs_n_struct*)
_temp167)->f1; goto _LL176;} else{ goto _LL168;} _LL170: goto _LL172; _LL172:
return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Tuniondecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_arr*), struct _tagged_arr* v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_tuniondecl_f,
_temp163); _LL174: { struct Cyc_Tcenv_Genv* _temp188= Cyc_Tcenv_lookup_namespace(
te, loc, _temp184, _temp182); return(( struct Cyc_Absyn_Tuniondecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp188->tuniondecls,
_temp163);} _LL176: { struct Cyc_Tcenv_Genv* _temp189=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp186); return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp189->tuniondecls, _temp163);}
_LL168:;}} static struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_xtuniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->tuniondecls,
v);} struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp192; struct _tagged_arr* _temp193; void* _temp195; struct _tuple0* _temp190=
q; _temp192=* _temp190; _LL196: _temp195= _temp192.f1; goto _LL194; _LL194:
_temp193= _temp192.f2; goto _LL191; _LL191: { void* _temp197= _temp195; struct
Cyc_List_List* _temp207; struct Cyc_List_List* _temp209; struct Cyc_List_List
_temp211; struct Cyc_List_List* _temp212; struct _tagged_arr* _temp214; struct
Cyc_List_List* _temp216; _LL199: if(( unsigned int) _temp197 > 1u?*(( int*)
_temp197) == Cyc_Absyn_Rel_n: 0){ _LL208: _temp207=(( struct Cyc_Absyn_Rel_n_struct*)
_temp197)->f1; if( _temp207 == 0){ goto _LL200;} else{ goto _LL201;}} else{ goto
_LL201;} _LL201: if( _temp197 ==( void*) Cyc_Absyn_Loc_n){ goto _LL202;} else{
goto _LL203;} _LL203: if(( unsigned int) _temp197 > 1u?*(( int*) _temp197) ==
Cyc_Absyn_Rel_n: 0){ _LL210: _temp209=(( struct Cyc_Absyn_Rel_n_struct*)
_temp197)->f1; if( _temp209 == 0){ goto _LL205;} else{ _temp211=* _temp209;
_LL215: _temp214=( struct _tagged_arr*) _temp211.hd; goto _LL213; _LL213:
_temp212= _temp211.tl; goto _LL204;}} else{ goto _LL205;} _LL205: if((
unsigned int) _temp197 > 1u?*(( int*) _temp197) == Cyc_Absyn_Abs_n: 0){ _LL217:
_temp216=(( struct Cyc_Absyn_Abs_n_struct*) _temp197)->f1; goto _LL206;} else{
goto _LL198;} _LL200: { struct _handler_cons _temp218; _push_handler(& _temp218);{
int _temp220= 0; if( setjmp( _temp218.handler)){ _temp220= 1;} if( ! _temp220){{
struct Cyc_Core_Opt* _temp222=({ struct Cyc_Core_Opt* _temp221=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp221->v=( void*)(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_arr*), struct _tagged_arr* v))
Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_xtuniondecl_f, _temp193);
_temp221;}); _npop_handler( 0u); return _temp222;}; _pop_handler();} else{ void*
_temp219=( void*) _exn_thrown; void* _temp224= _temp219; _LL226: if( _temp224 ==
Cyc_Dict_Absent){ goto _LL227;} else{ goto _LL228;} _LL228: goto _LL229; _LL227:
return 0; _LL229:( void) _throw( _temp224); _LL225:;}}} _LL202: Cyc_Tcutil_terr(
loc, _tag_arr("lookup_xtuniondecl: impossible", sizeof( unsigned char), 31u));
return 0; _LL204: { struct Cyc_Tcenv_Genv* ge;{ struct _handler_cons _temp230;
_push_handler(& _temp230);{ int _temp232= 0; if( setjmp( _temp230.handler)){
_temp232= 1;} if( ! _temp232){ ge= Cyc_Tcenv_lookup_namespace( te, loc, _temp214,
_temp212);; _pop_handler();} else{ void* _temp231=( void*) _exn_thrown; void*
_temp234= _temp231; _LL236: if( _temp234 == Cyc_Dict_Absent){ goto _LL237;}
else{ goto _LL238;} _LL238: goto _LL239; _LL237: Cyc_Tcutil_terr( loc, _tag_arr("bad qualified name for xtunion",
sizeof( unsigned char), 31u));( int) _throw(( void*) Cyc_Dict_Absent); _LL239:(
void) _throw( _temp234); _LL235:;}}} return({ struct Cyc_Core_Opt* _temp240=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp240->v=(
void*)(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup)( ge->tuniondecls, _temp193); _temp240;});}
_LL206: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp216); return({
struct Cyc_Core_Opt* _temp241=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp241->v=( void*)(( struct Cyc_Absyn_Tuniondecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->tuniondecls,
_temp193); _temp241;});} _LL198:;}} static struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->enumdecls,
v);} struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp244; struct _tagged_arr* _temp245; void* _temp247; struct _tuple0* _temp242=
q; _temp244=* _temp242; _LL248: _temp247= _temp244.f1; goto _LL246; _LL246:
_temp245= _temp244.f2; goto _LL243; _LL243: { void* _temp249= _temp247; struct
Cyc_List_List* _temp259; struct Cyc_List_List* _temp261; struct Cyc_List_List
_temp263; struct Cyc_List_List* _temp264; struct _tagged_arr* _temp266; struct
Cyc_List_List* _temp268; _LL251: if( _temp249 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL252;} else{ goto _LL253;} _LL253: if(( unsigned int) _temp249 > 1u?*(( int*)
_temp249) == Cyc_Absyn_Rel_n: 0){ _LL260: _temp259=(( struct Cyc_Absyn_Rel_n_struct*)
_temp249)->f1; if( _temp259 == 0){ goto _LL254;} else{ goto _LL255;}} else{ goto
_LL255;} _LL255: if(( unsigned int) _temp249 > 1u?*(( int*) _temp249) == Cyc_Absyn_Rel_n:
0){ _LL262: _temp261=(( struct Cyc_Absyn_Rel_n_struct*) _temp249)->f1; if(
_temp261 == 0){ goto _LL257;} else{ _temp263=* _temp261; _LL267: _temp266=(
struct _tagged_arr*) _temp263.hd; goto _LL265; _LL265: _temp264= _temp263.tl;
goto _LL256;}} else{ goto _LL257;} _LL257: if(( unsigned int) _temp249 > 1u?*((
int*) _temp249) == Cyc_Absyn_Abs_n: 0){ _LL269: _temp268=(( struct Cyc_Absyn_Abs_n_struct*)
_temp249)->f1; goto _LL258;} else{ goto _LL250;} _LL252: goto _LL254; _LL254:
return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Enumdecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_arr*), struct _tagged_arr* v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_enumdecl_f,
_temp245); _LL256: { struct Cyc_Tcenv_Genv* _temp270= Cyc_Tcenv_lookup_namespace(
te, loc, _temp266, _temp264); return(( struct Cyc_Absyn_Enumdecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp270->enumdecls,
_temp245);} _LL258: { struct Cyc_Tcenv_Genv* _temp271=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp268); return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp271->enumdecls, _temp245);}
_LL250:;}} static struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_arr* v){ return(( struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->typedefs,
v);} struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp274; struct _tagged_arr* _temp275; void* _temp277; struct _tuple0* _temp272=
q; _temp274=* _temp272; _LL278: _temp277= _temp274.f1; goto _LL276; _LL276:
_temp275= _temp274.f2; goto _LL273; _LL273: { void* _temp279= _temp277; struct
Cyc_List_List* _temp289; struct Cyc_List_List* _temp291; struct Cyc_List_List
_temp293; struct Cyc_List_List* _temp294; struct _tagged_arr* _temp296; struct
Cyc_List_List* _temp298; _LL281: if( _temp279 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL282;} else{ goto _LL283;} _LL283: if(( unsigned int) _temp279 > 1u?*(( int*)
_temp279) == Cyc_Absyn_Rel_n: 0){ _LL290: _temp289=(( struct Cyc_Absyn_Rel_n_struct*)
_temp279)->f1; if( _temp289 == 0){ goto _LL284;} else{ goto _LL285;}} else{ goto
_LL285;} _LL285: if(( unsigned int) _temp279 > 1u?*(( int*) _temp279) == Cyc_Absyn_Rel_n:
0){ _LL292: _temp291=(( struct Cyc_Absyn_Rel_n_struct*) _temp279)->f1; if(
_temp291 == 0){ goto _LL287;} else{ _temp293=* _temp291; _LL297: _temp296=(
struct _tagged_arr*) _temp293.hd; goto _LL295; _LL295: _temp294= _temp293.tl;
goto _LL286;}} else{ goto _LL287;} _LL287: if(( unsigned int) _temp279 > 1u?*((
int*) _temp279) == Cyc_Absyn_Abs_n: 0){ _LL299: _temp298=(( struct Cyc_Absyn_Abs_n_struct*)
_temp279)->f1; goto _LL288;} else{ goto _LL280;} _LL282: goto _LL284; _LL284:
return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Typedefdecl*(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_arr*), struct _tagged_arr* v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_typedefdecl_f,
_temp275); _LL286: { struct Cyc_Tcenv_Genv* _temp300= Cyc_Tcenv_lookup_namespace(
te, loc, _temp296, _temp294); return(( struct Cyc_Absyn_Typedefdecl*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp300->typedefs,
_temp275);} _LL288: { struct Cyc_Tcenv_Genv* _temp301=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp298); return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp301->typedefs, _temp275);}
_LL280:;}} struct Cyc_Tcenv_Genv* Cyc_Tcenv_empty_genv(){ return({ struct Cyc_Tcenv_Genv*
_temp302=( struct Cyc_Tcenv_Genv*) GC_malloc( sizeof( struct Cyc_Tcenv_Genv));
_temp302->namespaces=( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Tcutil_empty_var_set))->v; _temp302->structdecls=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp302->uniondecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp302->tuniondecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp302->enumdecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp302->typedefs=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp302->ordinaries=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp302->availables= 0; _temp302;});} void* Cyc_Tcenv_get_visible( void* f){
while( 1) { void* _temp303= f; void* _temp311; void* _temp313; void* _temp315;
void* _temp317; _LL305: if(*(( int*) _temp303) == Cyc_Tcenv_Outermost){ _LL312:
_temp311=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp303)->f1; goto
_LL306;} else{ goto _LL307;} _LL307: if(*(( int*) _temp303) == Cyc_Tcenv_Frame){
_LL314: _temp313=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp303)->f1; goto
_LL308;} else{ goto _LL309;} _LL309: if(*(( int*) _temp303) == Cyc_Tcenv_Hidden){
_LL318: _temp317=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp303)->f1; goto
_LL316; _LL316: _temp315=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp303)->f2;
goto _LL310;} else{ goto _LL304;} _LL306: return _temp311; _LL308: return
_temp313; _LL310: f= _temp315; goto _LL304; _LL304:;}} void* Cyc_Tcenv_put_visible(
void* f, void* x){ void* _temp319= f; void* _temp327; void* _temp329; void*
_temp331; void* _temp333; void* _temp335; _LL321: if(*(( int*) _temp319) == Cyc_Tcenv_Outermost){
_LL328: _temp327=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp319)->f1;
goto _LL322;} else{ goto _LL323;} _LL323: if(*(( int*) _temp319) == Cyc_Tcenv_Frame){
_LL332: _temp331=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp319)->f1; goto
_LL330; _LL330: _temp329=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp319)->f2;
goto _LL324;} else{ goto _LL325;} _LL325: if(*(( int*) _temp319) == Cyc_Tcenv_Hidden){
_LL336: _temp335=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp319)->f1; goto
_LL334; _LL334: _temp333=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp319)->f2;
goto _LL326;} else{ goto _LL320;} _LL322: return( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp337=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp337[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp338; _temp338.tag= Cyc_Tcenv_Outermost;
_temp338.f1=( void*) x; _temp338;}); _temp337;}); _LL324: return( void*)({
struct Cyc_Tcenv_Frame_struct* _temp339=( struct Cyc_Tcenv_Frame_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Frame_struct)); _temp339[ 0]=({ struct Cyc_Tcenv_Frame_struct
_temp340; _temp340.tag= Cyc_Tcenv_Frame; _temp340.f1=( void*) x; _temp340.f2=(
void*) _temp329; _temp340;}); _temp339;}); _LL326: return( void*)({ struct Cyc_Tcenv_Hidden_struct*
_temp341=( struct Cyc_Tcenv_Hidden_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Hidden_struct));
_temp341[ 0]=({ struct Cyc_Tcenv_Hidden_struct _temp342; _temp342.tag= Cyc_Tcenv_Hidden;
_temp342.f1=( void*) _temp335; _temp342.f2=( void*) Cyc_Tcenv_put_visible(
_temp333, x); _temp342;}); _temp341;}); _LL320:;} void* Cyc_Tcenv_apply_to_visible(
void*(* g)( void*), void* f){ return Cyc_Tcenv_put_visible( f, g( Cyc_Tcenv_get_visible(
f)));} void* Cyc_Tcenv_map_frames( void*(* g)( void*), void* f){ void* _temp343=
f; void* _temp351; void* _temp353; void* _temp355; void* _temp357; void*
_temp359; _LL345: if(*(( int*) _temp343) == Cyc_Tcenv_Outermost){ _LL352:
_temp351=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp343)->f1; goto
_LL346;} else{ goto _LL347;} _LL347: if(*(( int*) _temp343) == Cyc_Tcenv_Frame){
_LL356: _temp355=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp343)->f1; goto
_LL354; _LL354: _temp353=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp343)->f2;
goto _LL348;} else{ goto _LL349;} _LL349: if(*(( int*) _temp343) == Cyc_Tcenv_Hidden){
_LL360: _temp359=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp343)->f1; goto
_LL358; _LL358: _temp357=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp343)->f2;
goto _LL350;} else{ goto _LL344;} _LL346: return( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp361=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp361[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp362; _temp362.tag= Cyc_Tcenv_Outermost;
_temp362.f1=( void*) g( _temp351); _temp362;}); _temp361;}); _LL348: return(
void*)({ struct Cyc_Tcenv_Frame_struct* _temp363=( struct Cyc_Tcenv_Frame_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Frame_struct)); _temp363[ 0]=({ struct Cyc_Tcenv_Frame_struct
_temp364; _temp364.tag= Cyc_Tcenv_Frame; _temp364.f1=( void*) g( _temp355);
_temp364.f2=( void*) Cyc_Tcenv_map_frames( g, _temp353); _temp364;}); _temp363;});
_LL350: return( void*)({ struct Cyc_Tcenv_Hidden_struct* _temp365=( struct Cyc_Tcenv_Hidden_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Hidden_struct)); _temp365[ 0]=({ struct Cyc_Tcenv_Hidden_struct
_temp366; _temp366.tag= Cyc_Tcenv_Hidden; _temp366.f1=( void*) g( _temp359);
_temp366.f2=( void*) Cyc_Tcenv_map_frames( g, _temp357); _temp366;}); _temp365;});
_LL344:;} struct Cyc_Tcenv_Fenv* Cyc_Tcenv_get_fenv( struct Cyc_Tcenv_Tenv* te){
if( te->le == 0){( int) _throw(( void*) Cyc_Tcenv_Env_error);} return(( struct
Cyc_Tcenv_Fenv*(*)( void* f)) Cyc_Tcenv_get_visible)(( void*)(( struct Cyc_Core_Opt*)
_check_null( te->le))->v);} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_put_fenv( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Fenv* fe){ if( te->le == 0){( int) _throw((
void*) Cyc_Tcenv_Env_error);} return({ struct Cyc_Tcenv_Tenv* _temp367=( struct
Cyc_Tcenv_Tenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Tenv)); _temp367->ns= te->ns;
_temp367->ae= te->ae; _temp367->le=({ struct Cyc_Core_Opt* _temp368=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp368->v=( void*)(( void*(*)( void*
f, struct Cyc_Tcenv_Fenv* x)) Cyc_Tcenv_put_visible)(( void*)(( struct Cyc_Core_Opt*)
_check_null( te->le))->v, fe); _temp368;}); _temp367;});} static int Cyc_Tcenv_region_ancestor(
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
struct _tagged_arr)({ struct _tagged_arr(* _temp369)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp373= _tag_arr("can't find region variable %s in po",
sizeof( unsigned char), 36u); struct Cyc_Stdio_String_pa_struct _temp376;
_temp376.tag= Cyc_Stdio_String_pa; _temp376.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Absyn_VarType_struct* _temp374=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp374[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp375; _temp375.tag= Cyc_Absyn_VarType; _temp375.f1= r; _temp375;}); _temp374;}));{
void* _temp372=( void*)& _temp376; void* _temp370[ 1u]={ _temp372}; struct
_tagged_arr _temp371={( void*) _temp370,( void*) _temp370,( void*)( _temp370 + 1u)};
_temp369( _temp373, _temp371);}}));} static int Cyc_Tcenv_check_region_outlives(
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
void*) Cyc_Absyn_RgnKind){ result=({ struct Cyc_List_List* _temp377=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp377->hd=( void*)({ struct Cyc_Tcenv_RgnPO*
_temp378=( struct Cyc_Tcenv_RgnPO*) GC_malloc( sizeof( struct Cyc_Tcenv_RgnPO));
_temp378->tvar= tv; _temp378->ancestors= 0; _temp378;}); _temp377->tl= result;
_temp377;});}} for( 0; po != 0; po=(( struct Cyc_List_List*) _check_null( po))->tl){
struct _tuple5 _temp379=*(( struct _tuple5*)(( struct Cyc_List_List*)
_check_null( po))->hd); void* _temp387; void* _temp389; struct Cyc_Absyn_Tvar*
_temp391; void* _temp393; struct Cyc_Absyn_Tvar* _temp395; void* _temp397; void*
_temp399; _LL381: _LL388: _temp387= _temp379.f1; if( _temp387 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL382;} else{ goto _LL383;} _LL383: _LL394: _temp393= _temp379.f1; if((
unsigned int) _temp393 > 4u?*(( int*) _temp393) == Cyc_Absyn_VarType: 0){ _LL396:
_temp395=(( struct Cyc_Absyn_VarType_struct*) _temp393)->f1; goto _LL390;} else{
goto _LL385;} _LL390: _temp389= _temp379.f2; if(( unsigned int) _temp389 > 4u?*((
int*) _temp389) == Cyc_Absyn_VarType: 0){ _LL392: _temp391=(( struct Cyc_Absyn_VarType_struct*)
_temp389)->f1; goto _LL384;} else{ goto _LL385;} _LL385: _LL400: _temp399=
_temp379.f1; goto _LL398; _LL398: _temp397= _temp379.f2; goto _LL386; _LL382:
goto _LL380; _LL384: if( Cyc_Absyn_tvar_cmp( _temp395, _temp391) == 0? 1: Cyc_Tcenv_check_region_outlives(
result, _temp391, _temp395)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr(* _temp401)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp406= _tag_arr("bad region lifetime assertion (circularity):%s < %s",
sizeof( unsigned char), 52u); struct Cyc_Stdio_String_pa_struct _temp412;
_temp412.tag= Cyc_Stdio_String_pa; _temp412.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Absyn_VarType_struct* _temp410=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp410[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp411; _temp411.tag= Cyc_Absyn_VarType; _temp411.f1= _temp395; _temp411;});
_temp410;}));{ void* _temp404=( void*)& _temp412; struct Cyc_Stdio_String_pa_struct
_temp409; _temp409.tag= Cyc_Stdio_String_pa; _temp409.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Absyn_VarType_struct* _temp407=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp407[ 0]=({ struct Cyc_Absyn_VarType_struct _temp408; _temp408.tag= Cyc_Absyn_VarType;
_temp408.f1= _temp391; _temp408;}); _temp407;}));{ void* _temp405=( void*)&
_temp409; void* _temp402[ 2u]={ _temp404, _temp405}; struct _tagged_arr _temp403={(
void*) _temp402,( void*) _temp402,( void*)( _temp402 + 2u)}; _temp401( _temp406,
_temp403);}}}));} else{ struct Cyc_Tcenv_RgnPO* _temp413= Cyc_Tcenv_find_region_in_po(
result, _temp395); struct Cyc_Tcenv_RgnPO* _temp414= Cyc_Tcenv_find_region_in_po(
result, _temp391); _temp414->ancestors=({ struct Cyc_List_List* _temp415=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp415->hd=(
void*) _temp413; _temp415->tl= _temp414->ancestors; _temp415;});} goto _LL380;
_LL386: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp416)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp421= _tag_arr("bad region lifetime assertion:: %s < %s",
sizeof( unsigned char), 40u); struct Cyc_Stdio_String_pa_struct _temp423;
_temp423.tag= Cyc_Stdio_String_pa; _temp423.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp399);{ void* _temp419=( void*)& _temp423; struct Cyc_Stdio_String_pa_struct
_temp422; _temp422.tag= Cyc_Stdio_String_pa; _temp422.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( _temp397);{ void* _temp420=( void*)& _temp422; void*
_temp417[ 2u]={ _temp419, _temp420}; struct _tagged_arr _temp418={( void*)
_temp417,( void*) _temp417,( void*)( _temp417 + 2u)}; _temp416( _temp421,
_temp418);}}})); goto _LL380; _LL380:;} return result;} static struct Cyc_List_List*
Cyc_Tcenv_add_region_po( struct Cyc_List_List* po, struct Cyc_Absyn_Tvar* tv){
return({ struct Cyc_List_List* _temp424=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp424->hd=( void*)({ struct Cyc_Tcenv_RgnPO*
_temp425=( struct Cyc_Tcenv_RgnPO*) GC_malloc( sizeof( struct Cyc_Tcenv_RgnPO));
_temp425->tvar= tv; _temp425->ancestors= po; _temp425;}); _temp424->tl= po;
_temp424;});} static struct Cyc_Tcenv_Fenv* Cyc_Tcenv_copy_fenv_old_ctrl( struct
Cyc_Tcenv_Fenv* f){ return({ struct Cyc_Tcenv_Fenv* _temp426=( struct Cyc_Tcenv_Fenv*)
GC_malloc( sizeof( struct Cyc_Tcenv_Fenv)); _temp426->shared= f->shared;
_temp426->type_vars= f->type_vars; _temp426->region_order= f->region_order;
_temp426->locals= f->locals; _temp426->encloser= f->encloser; _temp426->ctrl_env=
f->ctrl_env; _temp426->capability=( void*)(( void*) f->capability); _temp426->curr_rgn=(
void*)(( void*) f->curr_rgn); _temp426;});} static struct Cyc_Tcenv_Fenv* Cyc_Tcenv_copy_fenv_new_ctrl(
struct Cyc_Tcenv_Fenv* f){ struct Cyc_Tcenv_CtrlEnv* c= f->ctrl_env; return({
struct Cyc_Tcenv_Fenv* _temp427=( struct Cyc_Tcenv_Fenv*) GC_malloc( sizeof(
struct Cyc_Tcenv_Fenv)); _temp427->shared= f->shared; _temp427->type_vars= f->type_vars;
_temp427->region_order= f->region_order; _temp427->locals= f->locals; _temp427->encloser=
f->encloser; _temp427->ctrl_env=({ struct Cyc_Tcenv_CtrlEnv* _temp428=( struct
Cyc_Tcenv_CtrlEnv*) GC_malloc( sizeof( struct Cyc_Tcenv_CtrlEnv)); _temp428->continue_stmt=(
void*)(( void*) c->continue_stmt); _temp428->break_stmt=( void*)(( void*) c->break_stmt);
_temp428->fallthru_clause= c->fallthru_clause; _temp428->next_stmt=( void*)((
void*) c->next_stmt); _temp428->try_depth= c->try_depth; _temp428;}); _temp427->capability=(
void*)(( void*) f->capability); _temp427->curr_rgn=( void*)(( void*) f->curr_rgn);
_temp427;});} void* Cyc_Tcenv_return_typ( struct Cyc_Tcenv_Tenv* te){ if( te->le
== 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected return_typ",
sizeof( unsigned char), 29u));} return( void*)(( Cyc_Tcenv_get_fenv( te))->shared)->return_typ;}
struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars( struct Cyc_Tcenv_Tenv* te){
if( te->le == 0){ return 0;} return( Cyc_Tcenv_get_fenv( te))->type_vars;}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment* loc,
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* tvs){ if( te->le == 0){(( int(*)(
struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected add_type_vars",
sizeof( unsigned char), 32u));}{ struct Cyc_Tcenv_Fenv* _temp429= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); Cyc_Tcutil_add_tvar_identities( tvs);{ struct Cyc_List_List*
_temp430=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( tvs, _temp429->type_vars); Cyc_Tcutil_check_unique_tvars(
loc, _temp430); _temp429->type_vars= _temp430; return Cyc_Tcenv_put_fenv( te,
_temp429);}}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Vardecl* vd){ if( te->le == 0){((
int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected add_local_var",
sizeof( unsigned char), 32u));}{ struct Cyc_Tcenv_Fenv* _temp431= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); struct _tagged_arr* _temp432=(* vd->name).f2;{ void*
_temp433=(* vd->name).f1; _LL435: if( _temp433 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL436;} else{ goto _LL437;} _LL437: goto _LL438; _LL436: goto _LL434; _LL438:(
int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp439=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp439[ 0]=({ struct
Cyc_Core_Impossible_struct _temp440; _temp440.tag= Cyc_Core_Impossible; _temp440.f1=
_tag_arr("add_local_var: called with Rel_n", sizeof( unsigned char), 33u);
_temp440;}); _temp439;})); goto _LL434; _LL434:;} _temp431->locals=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, void* data)) Cyc_Dict_insert)(
_temp431->locals, _temp432,( void*)({ struct Cyc_Absyn_Local_b_struct* _temp441=(
struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp441[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp442; _temp442.tag= Cyc_Absyn_Local_b;
_temp442.f1= vd; _temp442;}); _temp441;})); return Cyc_Tcenv_put_fenv( te,
_temp431);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_pat_var( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Vardecl* vd){ if( te->le == 0){((
int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected add_pat_var",
sizeof( unsigned char), 30u));}{ struct Cyc_Tcenv_Fenv* _temp443= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); struct _tagged_arr* _temp444=(* vd->name).f2; _temp443->locals=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key, void*
data)) Cyc_Dict_insert)( _temp443->locals, _temp444,( void*)({ struct Cyc_Absyn_Pat_b_struct*
_temp445=( struct Cyc_Absyn_Pat_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pat_b_struct));
_temp445[ 0]=({ struct Cyc_Absyn_Pat_b_struct _temp446; _temp446.tag= Cyc_Absyn_Pat_b;
_temp446.f1= vd; _temp446;}); _temp445;})); return Cyc_Tcenv_put_fenv( te,
_temp443);}} void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp449; struct
_tagged_arr* _temp450; void* _temp452; struct _tuple0* _temp447= q; _temp449=*
_temp447; _LL453: _temp452= _temp449.f1; goto _LL451; _LL451: _temp450= _temp449.f2;
goto _LL448; _LL448: { void* _temp454= _temp452; struct Cyc_List_List* _temp463;
_LL456: if( _temp454 ==( void*) Cyc_Absyn_Loc_n){ goto _LL457;} else{ goto
_LL458;} _LL458: if(( unsigned int) _temp454 > 1u?*(( int*) _temp454) == Cyc_Absyn_Rel_n:
0){ _LL464: _temp463=(( struct Cyc_Absyn_Rel_n_struct*) _temp454)->f1; if(
_temp463 == 0){ goto _LL462;} else{ goto _LL460;}} else{ goto _LL460;} _LL462:
if( te->le != 0){ goto _LL459;} else{ goto _LL460;} _LL460: goto _LL461; _LL457:
if( te->le == 0){( int) _throw(( void*) Cyc_Dict_Absent);} else{ goto _LL459;}
_LL459: { struct Cyc_Tcenv_Fenv* _temp465= Cyc_Tcenv_get_fenv( te); struct
_handler_cons _temp466; _push_handler(& _temp466);{ int _temp468= 0; if( setjmp(
_temp466.handler)){ _temp468= 1;} if( ! _temp468){{ void* _temp471=( void*)({
struct Cyc_Tcenv_VarRes_struct* _temp469=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp469[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp470; _temp470.tag= Cyc_Tcenv_VarRes; _temp470.f1=( void*)(( void*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp465->locals,
_temp450); _temp470;}); _temp469;}); _npop_handler( 0u); return _temp471;};
_pop_handler();} else{ void* _temp467=( void*) _exn_thrown; void* _temp473=
_temp467; _LL475: if( _temp473 == Cyc_Dict_Absent){ goto _LL476;} else{ goto
_LL477;} _LL477: goto _LL478; _LL476: return Cyc_Tcenv_lookup_ordinary_global(
te, loc, q); _LL478:( void) _throw( _temp473); _LL474:;}}} _LL461: { struct
_handler_cons _temp479; _push_handler(& _temp479);{ int _temp481= 0; if( setjmp(
_temp479.handler)){ _temp481= 1;} if( ! _temp481){{ void* _temp482= Cyc_Tcenv_lookup_ordinary_global(
te, loc, q); _npop_handler( 0u); return _temp482;}; _pop_handler();} else{ void*
_temp480=( void*) _exn_thrown; void* _temp484= _temp480; _LL486: if( _temp484 ==
Cyc_Dict_Absent){ goto _LL487;} else{ goto _LL488;} _LL488: goto _LL489; _LL487:
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp490)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp494= _tag_arr("unbound identifier: %s", sizeof( unsigned char),
23u); struct Cyc_Stdio_String_pa_struct _temp495; _temp495.tag= Cyc_Stdio_String_pa;
_temp495.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( q);{ void* _temp493=(
void*)& _temp495; void* _temp491[ 1u]={ _temp493}; struct _tagged_arr _temp492={(
void*) _temp491,( void*) _temp491,( void*)( _temp491 + 1u)}; _temp490( _temp494,
_temp492);}})); return( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp496=(
struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp496[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp497; _temp497.tag= Cyc_Tcenv_VarRes;
_temp497.f1=( void*)(( void*) Cyc_Absyn_Unresolved_b); _temp497;}); _temp496;});
_LL489:( void) _throw( _temp484); _LL485:;}}} _LL455:;}} void Cyc_Tcenv_process_continue(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Stmt**
sopt){ if( te->le == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected process_continue", sizeof( unsigned char), 35u));}{
void* _temp498=( void*)(( Cyc_Tcenv_get_fenv( te))->ctrl_env)->continue_stmt;
struct Cyc_Absyn_Stmt* _temp508; _LL500: if(( unsigned int) _temp498 > 3u?*((
int*) _temp498) == Cyc_Tcenv_Stmt_j: 0){ _LL509: _temp508=(( struct Cyc_Tcenv_Stmt_j_struct*)
_temp498)->f1; goto _LL501;} else{ goto _LL502;} _LL502: if( _temp498 ==( void*)
Cyc_Tcenv_NotLoop_j){ goto _LL503;} else{ goto _LL504;} _LL504: if( _temp498 ==(
void*) Cyc_Tcenv_CaseEnd_j){ goto _LL505;} else{ goto _LL506;} _LL506: if(
_temp498 ==( void*) Cyc_Tcenv_FnEnd_j){ goto _LL507;} else{ goto _LL499;} _LL501:
_temp508->non_local_preds=({ struct Cyc_List_List* _temp510=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp510->hd=( void*) s; _temp510->tl=
_temp508->non_local_preds; _temp510;});* sopt=( struct Cyc_Absyn_Stmt*) _temp508;
return; _LL503: Cyc_Tcutil_terr( s->loc, _tag_arr("continue not in a loop",
sizeof( unsigned char), 23u)); return; _LL505:(( int(*)( struct _tagged_arr msg))
Cyc_Tcenv_env_err)( _tag_arr("Tcenv: continue dest is CaseEnd", sizeof(
unsigned char), 32u)); return; _LL507:(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: continue dest is CaseEnd", sizeof( unsigned char), 32u));
return; _LL499:;}} void Cyc_Tcenv_process_break( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Stmt** sopt){ if( te->le == 0){(( int(*)(
struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected process_break",
sizeof( unsigned char), 32u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_get_fenv(
te); void* _temp511=( void*)( fe->ctrl_env)->break_stmt; struct Cyc_Absyn_Stmt*
_temp521; _LL513: if(( unsigned int) _temp511 > 3u?*(( int*) _temp511) == Cyc_Tcenv_Stmt_j:
0){ _LL522: _temp521=(( struct Cyc_Tcenv_Stmt_j_struct*) _temp511)->f1; goto
_LL514;} else{ goto _LL515;} _LL515: if( _temp511 ==( void*) Cyc_Tcenv_NotLoop_j){
goto _LL516;} else{ goto _LL517;} _LL517: if( _temp511 ==( void*) Cyc_Tcenv_FnEnd_j){
goto _LL518;} else{ goto _LL519;} _LL519: if( _temp511 ==( void*) Cyc_Tcenv_CaseEnd_j){
goto _LL520;} else{ goto _LL512;} _LL514: _temp521->non_local_preds=({ struct
Cyc_List_List* _temp523=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp523->hd=( void*) s; _temp523->tl= _temp521->non_local_preds; _temp523;});*
sopt=( struct Cyc_Absyn_Stmt*) _temp521; return; _LL516: Cyc_Tcutil_terr( s->loc,
_tag_arr("break not in a loop or switch", sizeof( unsigned char), 30u)); return;
_LL518: if(( void*)( fe->shared)->return_typ !=( void*) Cyc_Absyn_VoidType){ Cyc_Tcutil_terr(
s->loc, _tag_arr("break causes function not to return a value", sizeof(
unsigned char), 44u));} return; _LL520: Cyc_Tcutil_terr( s->loc, _tag_arr("break causes outer switch clause to implicitly fallthru",
sizeof( unsigned char), 56u)); return; _LL512:;}} void Cyc_Tcenv_process_goto(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct _tagged_arr* l,
struct Cyc_Absyn_Stmt** sopt){ if( te->le == 0){(( int(*)( struct _tagged_arr
msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected process_goto", sizeof(
unsigned char), 31u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_get_fenv( te);
struct Cyc_Core_Opt* _temp524=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)(( fe->shared)->seen_labels, l);
if( _temp524 == 0){ struct Cyc_SlowDict_Dict* _temp525=( fe->shared)->needed_labels;
struct Cyc_Core_Opt* _temp526=(( struct Cyc_Core_Opt*(*)( struct Cyc_SlowDict_Dict*
d, struct _tagged_arr* key)) Cyc_SlowDict_lookup_opt)( _temp525, l); if(
_temp526 == 0){ _temp526=({ struct Cyc_Core_Opt* _temp527=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp527->v=( void*) 0; _temp527;});}(
fe->shared)->needed_labels=(( struct Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict*
d, struct _tagged_arr* key, struct Cyc_List_List* data)) Cyc_SlowDict_insert)(
_temp525, l,({ struct Cyc_List_List* _temp528=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp528->hd=( void*) s; _temp528->tl=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp526))->v; _temp528;}));}
else{(( struct Cyc_Absyn_Stmt*)(( struct Cyc_Core_Opt*) _check_null( _temp524))->v)->non_local_preds=({
struct Cyc_List_List* _temp529=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp529->hd=( void*) s; _temp529->tl=(( struct Cyc_Absyn_Stmt*)((
struct Cyc_Core_Opt*) _check_null( _temp524))->v)->non_local_preds; _temp529;});*
sopt=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Stmt*)(( struct Cyc_Core_Opt*)
_check_null( _temp524))->v);}}} struct _tuple3* Cyc_Tcenv_process_fallthru(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Switch_clause***
clauseopt){ if( te->le == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected process_fallthru", sizeof( unsigned char), 35u));}{
struct Cyc_Tcenv_Fenv* _temp530= Cyc_Tcenv_get_fenv( te); struct _tuple3*
_temp531=( _temp530->ctrl_env)->fallthru_clause; if( _temp531 != 0){(((*((
struct _tuple3*) _check_null( _temp531))).f1)->body)->non_local_preds=({ struct
Cyc_List_List* _temp532=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp532->hd=( void*) s; _temp532->tl=(((*(( struct _tuple3*) _check_null(
_temp531))).f1)->body)->non_local_preds; _temp532;});* clauseopt=({ struct Cyc_Absyn_Switch_clause**
_temp533=( struct Cyc_Absyn_Switch_clause**) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause*)
* 1); _temp533[ 0]=(*(( struct _tuple3*) _check_null( _temp531))).f1; _temp533;});}
return _temp531;}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_fallthru( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* new_tvs, struct Cyc_List_List* vds, struct Cyc_Absyn_Switch_clause*
clause){ if( te->le == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected set_fallthru", sizeof( unsigned char), 31u));}{
struct Cyc_List_List* _temp534= 0; for( 0; vds != 0; vds=(( struct Cyc_List_List*)
_check_null( vds))->tl){ _temp534=({ struct Cyc_List_List* _temp535=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp535->hd=( void*)(( void*)((
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( vds))->hd)->type);
_temp535->tl= _temp534; _temp535;});} _temp534= Cyc_List_imp_rev( _temp534);{
struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));(
fe->ctrl_env)->fallthru_clause=({ struct _tuple3* _temp536=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp536->f1= clause; _temp536->f2= new_tvs;
_temp536->f3= _temp534; _temp536;}); return Cyc_Tcenv_put_fenv( te, fe);}}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_clear_fallthru( struct Cyc_Tcenv_Tenv* te){ if(
te->le == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected clear_fallthru",
sizeof( unsigned char), 33u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te));( fe->ctrl_env)->fallthru_clause= 0; return Cyc_Tcenv_put_fenv(
te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_loop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* continue_dest){ if( te->le == 0){(( int(*)( struct
_tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected set_in_loop",
sizeof( unsigned char), 30u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->continue_stmt=( void*)(( void*)({
struct Cyc_Tcenv_Stmt_j_struct* _temp537=( struct Cyc_Tcenv_Stmt_j_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp537[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct
_temp538; _temp538.tag= Cyc_Tcenv_Stmt_j; _temp538.f1= continue_dest; _temp538;});
_temp537;})));( void*)(( fe->ctrl_env)->break_stmt=( void*)(( void*)( fe->ctrl_env)->next_stmt));(
void*)(( fe->ctrl_env)->next_stmt=( void*)(( void*)({ struct Cyc_Tcenv_Stmt_j_struct*
_temp539=( struct Cyc_Tcenv_Stmt_j_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct));
_temp539[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct _temp540; _temp540.tag= Cyc_Tcenv_Stmt_j;
_temp540.f1= continue_dest; _temp540;}); _temp539;}))); return Cyc_Tcenv_put_fenv(
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
sizeof( unsigned char), 28u));}{ struct Cyc_Tcenv_Fenv* _temp541= Cyc_Tcenv_get_fenv(
te); struct Cyc_SlowDict_Dict* _temp542=( _temp541->shared)->needed_labels;
struct Cyc_Core_Opt* _temp543=(( struct Cyc_Core_Opt*(*)( struct Cyc_SlowDict_Dict*
d, struct _tagged_arr* key)) Cyc_SlowDict_lookup_opt)( _temp542, v); if(
_temp543 != 0){( _temp541->shared)->needed_labels=(( struct Cyc_SlowDict_Dict*(*)(
struct Cyc_SlowDict_Dict* d, struct _tagged_arr* key)) Cyc_SlowDict_delete)(
_temp542, v);{ struct Cyc_List_List* _temp544=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( _temp543))->v; s->non_local_preds= _temp544; for( 0;
_temp544 != 0; _temp544=(( struct Cyc_List_List*) _check_null( _temp544))->tl){
void* _temp545=( void*)(( struct Cyc_Absyn_Stmt*)(( struct Cyc_List_List*)
_check_null( _temp544))->hd)->r; struct Cyc_Absyn_Stmt* _temp551; struct Cyc_Absyn_Stmt**
_temp553; _LL547: if(( unsigned int) _temp545 > 1u?*(( int*) _temp545) == Cyc_Absyn_Goto_s:
0){ _LL552: _temp551=(( struct Cyc_Absyn_Goto_s_struct*) _temp545)->f2; _temp553=&((
struct Cyc_Absyn_Goto_s_struct*) _temp545)->f2; goto _LL548;} else{ goto _LL549;}
_LL549: goto _LL550; _LL548:* _temp553=( struct Cyc_Absyn_Stmt*) s; goto _LL546;
_LL550:(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: add_label backpatching of non-goto",
sizeof( unsigned char), 42u)); goto _LL546; _LL546:;}}} if((( int(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key)) Cyc_Dict_member)(( _temp541->shared)->seen_labels,
v)){ Cyc_Tcutil_terr( s->loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp554)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp558= _tag_arr("Repeated label: %s", sizeof(
unsigned char), 19u); struct Cyc_Stdio_String_pa_struct _temp559; _temp559.tag=
Cyc_Stdio_String_pa; _temp559.f1=( struct _tagged_arr)* v;{ void* _temp557=(
void*)& _temp559; void* _temp555[ 1u]={ _temp557}; struct _tagged_arr _temp556={(
void*) _temp555,( void*) _temp555,( void*)( _temp555 + 1u)}; _temp554( _temp558,
_temp556);}}));}( _temp541->shared)->seen_labels=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Stmt* data))
Cyc_Dict_insert)(( _temp541->shared)->seen_labels, v, s); return te;}} int Cyc_Tcenv_all_labels_resolved(
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
Cyc_Tcenv_get_fenv( te)); struct Cyc_Absyn_Tvar* tv;{ void* _temp560= Cyc_Tcutil_compress(
r); struct Cyc_Absyn_Tvar* _temp566; _LL562: if(( unsigned int) _temp560 > 4u?*((
int*) _temp560) == Cyc_Absyn_VarType: 0){ _LL567: _temp566=(( struct Cyc_Absyn_VarType_struct*)
_temp560)->f1; goto _LL563;} else{ goto _LL564;} _LL564: goto _LL565; _LL563: tv=
_temp566; goto _LL561; _LL565: tv=(( struct Cyc_Absyn_Tvar*(*)( struct
_tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("bad add region", sizeof(
unsigned char), 15u)); goto _LL561; _LL561:;} fe->region_order= Cyc_Tcenv_add_region_po(
fe->region_order, tv);( void*)( fe->capability=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp568=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp568[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp569; _temp569.tag= Cyc_Absyn_JoinEff;
_temp569.f1=({ struct Cyc_List_List* _temp570=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp570->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp572=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp572[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp573; _temp573.tag= Cyc_Absyn_AccessEff;
_temp573.f1=( void*) r; _temp573;}); _temp572;})); _temp570->tl=({ struct Cyc_List_List*
_temp571=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp571->hd=( void*)(( void*) fe->capability); _temp571->tl= 0; _temp571;});
_temp570;}); _temp569;}); _temp568;}))); return Cyc_Tcenv_put_fenv( te, fe);}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_named_block( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Tvar* block_rgn){ if( te->le ==
0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcenv_env_err)( _tag_arr("Tcenv: unexpected new_block",
sizeof( unsigned char), 28u));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); void* block_typ=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp581=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp581[ 0]=({ struct Cyc_Absyn_VarType_struct _temp582; _temp582.tag= Cyc_Absyn_VarType;
_temp582.f1= block_rgn; _temp582;}); _temp581;}); fe->type_vars=({ struct Cyc_List_List*
_temp574=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp574->hd=( void*) block_rgn; _temp574->tl= fe->type_vars; _temp574;}); Cyc_Tcutil_check_unique_tvars(
loc, fe->type_vars); fe->region_order= Cyc_Tcenv_add_region_po( fe->region_order,
block_rgn);( void*)( fe->capability=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp575=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp575[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp576; _temp576.tag= Cyc_Absyn_JoinEff;
_temp576.f1=({ struct Cyc_List_List* _temp577=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp577->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp579=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp579[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp580; _temp580.tag= Cyc_Absyn_AccessEff;
_temp580.f1=( void*) block_typ; _temp580;}); _temp579;})); _temp577->tl=({
struct Cyc_List_List* _temp578=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp578->hd=( void*)(( void*) fe->capability); _temp578->tl=
0; _temp578;}); _temp577;}); _temp576;}); _temp575;})));( void*)( fe->curr_rgn=(
void*) block_typ); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_block( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te){
struct Cyc_Absyn_Tvar* t= Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_RgnKind); Cyc_Tcutil_add_tvar_identity(
t); return Cyc_Tcenv_new_named_block( loc, te, t);} void* Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){ return( void*) Cyc_Absyn_HeapRgn;}
return( void*)( Cyc_Tcenv_get_fenv( te))->curr_rgn;} void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* rgn){ if( te->le
!= 0){ void* capability=( void*)( Cyc_Tcenv_get_fenv( te))->capability; if( Cyc_Tcutil_region_in_effect(
0, rgn, capability)? 1: Cyc_Tcutil_region_in_effect( 1, rgn, capability)){
return;}} Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp583)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp587= _tag_arr("Expression accesses unavailable region %s",
sizeof( unsigned char), 42u); struct Cyc_Stdio_String_pa_struct _temp588;
_temp588.tag= Cyc_Stdio_String_pa; _temp588.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
rgn);{ void* _temp586=( void*)& _temp588; void* _temp584[ 1u]={ _temp586};
struct _tagged_arr _temp585={( void*) _temp584,( void*) _temp584,( void*)(
_temp584 + 1u)}; _temp583( _temp587, _temp585);}}));} int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv* te, void* rt_a, void* rt_b){ if( rt_a ==( void*) Cyc_Absyn_HeapRgn){
return 1;} if( te->le != 0){ struct _tuple5 _temp590=({ struct _tuple5 _temp589;
_temp589.f1= Cyc_Tcutil_compress( rt_a); _temp589.f2= Cyc_Tcutil_compress( rt_b);
_temp589;}); void* _temp596; struct Cyc_Absyn_Tvar* _temp598; void* _temp600;
struct Cyc_Absyn_Tvar* _temp602; _LL592: _LL601: _temp600= _temp590.f1; if((
unsigned int) _temp600 > 4u?*(( int*) _temp600) == Cyc_Absyn_VarType: 0){ _LL603:
_temp602=(( struct Cyc_Absyn_VarType_struct*) _temp600)->f1; goto _LL597;} else{
goto _LL594;} _LL597: _temp596= _temp590.f2; if(( unsigned int) _temp596 > 4u?*((
int*) _temp596) == Cyc_Absyn_VarType: 0){ _LL599: _temp598=(( struct Cyc_Absyn_VarType_struct*)
_temp596)->f1; goto _LL593;} else{ goto _LL594;} _LL594: goto _LL595; _LL593:
return Cyc_Tcenv_check_region_outlives(( Cyc_Tcenv_get_fenv( te))->region_order,
_temp602, _temp598); _LL595: return 0; _LL591:;} return 0;} void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* eff){ void*
capability; if( te->le == 0){ capability=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp604=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp604[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp605; _temp605.tag= Cyc_Absyn_JoinEff;
_temp605.f1= 0; _temp605;}); _temp604;});} else{ struct Cyc_Tcenv_Fenv* _temp606=
Cyc_Tcenv_get_fenv( te); capability=( void*) _temp606->capability;} if( ! Cyc_Tcutil_subset_effect(
1, eff, capability)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp607)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp612= _tag_arr("Capability %s does not cover function's effect %s",
sizeof( unsigned char), 50u); struct Cyc_Stdio_String_pa_struct _temp614;
_temp614.tag= Cyc_Stdio_String_pa; _temp614.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
capability);{ void* _temp610=( void*)& _temp614; struct Cyc_Stdio_String_pa_struct
_temp613; _temp613.tag= Cyc_Stdio_String_pa; _temp613.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( eff);{ void* _temp611=( void*)& _temp613; void* _temp608[
2u]={ _temp610, _temp611}; struct _tagged_arr _temp609={( void*) _temp608,( void*)
_temp608,( void*)( _temp608 + 2u)}; _temp607( _temp612, _temp609);}}}));}} void
Cyc_Tcenv_check_rgn_partial_order( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* po){ for( 0; po != 0; po=(( struct Cyc_List_List*)
_check_null( po))->tl){ struct _tuple5 _temp617; void* _temp618; void* _temp620;
struct _tuple5* _temp615=( struct _tuple5*)(( struct Cyc_List_List*) _check_null(
po))->hd; _temp617=* _temp615; _LL621: _temp620= _temp617.f1; goto _LL619;
_LL619: _temp618= _temp617.f2; goto _LL616; _LL616: if( ! Cyc_Tcenv_region_outlives(
te, _temp620, _temp618)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp622)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp627= _tag_arr("function requires that region %s outlive %s",
sizeof( unsigned char), 44u); struct Cyc_Stdio_String_pa_struct _temp629;
_temp629.tag= Cyc_Stdio_String_pa; _temp629.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp620);{ void* _temp625=( void*)& _temp629; struct Cyc_Stdio_String_pa_struct
_temp628; _temp628.tag= Cyc_Stdio_String_pa; _temp628.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( _temp618);{ void* _temp626=( void*)& _temp628; void*
_temp623[ 2u]={ _temp625, _temp626}; struct _tagged_arr _temp624={( void*)
_temp623,( void*) _temp623,( void*)( _temp623 + 2u)}; _temp622( _temp627,
_temp624);}}}));}}} struct _tuple6{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual
f2; void* f3; } ; struct Cyc_Tcenv_Fenv* Cyc_Tcenv_new_fenv( struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Fndecl* fd){ struct Cyc_Dict_Dict* locals=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
struct Cyc_Absyn_Tvar* rgn0=({ struct Cyc_Absyn_Tvar* _temp682=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp682->name=({ struct _tagged_arr*
_temp683=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr));
_temp683[ 0]=( struct _tagged_arr)({ struct _tagged_arr(* _temp684)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp688= _tag_arr("`%s", sizeof( unsigned char), 4u); struct Cyc_Stdio_String_pa_struct
_temp689; _temp689.tag= Cyc_Stdio_String_pa; _temp689.f1=( struct _tagged_arr)*(*
fd->name).f2;{ void* _temp687=( void*)& _temp689; void* _temp685[ 1u]={ _temp687};
struct _tagged_arr _temp686={( void*) _temp685,( void*) _temp685,( void*)(
_temp685 + 1u)}; _temp684( _temp688, _temp686);}}); _temp683;}); _temp682->identity=
Cyc_Tcutil_new_tvar_id(); _temp682->kind= Cyc_Absyn_new_conref(( void*) Cyc_Absyn_RgnKind);
_temp682;}); struct Cyc_List_List* _temp630=({ struct Cyc_List_List* _temp681=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp681->hd=(
void*) rgn0; _temp681->tl= fd->tvs; _temp681;}); Cyc_Tcutil_check_unique_tvars(
loc, _temp630);{ struct Cyc_List_List* _temp631= Cyc_Tcenv_add_region_po( Cyc_Tcenv_initial_region_po(
loc, fd->tvs, fd->rgn_po), rgn0); void* param_rgn=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp679=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp679[ 0]=({ struct Cyc_Absyn_VarType_struct _temp680; _temp680.tag= Cyc_Absyn_VarType;
_temp680.f1= rgn0; _temp680;}); _temp679;}); struct Cyc_List_List* _temp632= 0;{
struct Cyc_List_List* _temp633= fd->args; for( 0; _temp633 != 0; _temp633=((
struct Cyc_List_List*) _check_null( _temp633))->tl){ struct Cyc_Absyn_Vardecl*
_temp634=({ struct Cyc_Absyn_Vardecl* _temp639=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl) * 1); _temp639[ 0]=({ struct Cyc_Absyn_Vardecl
_temp640; _temp640.sc=( void*)(( void*) Cyc_Absyn_Public); _temp640.name=({
struct _tuple0* _temp643=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0) *
1); _temp643[ 0]=({ struct _tuple0 _temp644; _temp644.f1=( void*) Cyc_Absyn_Loc_n;
_temp644.f2=(*(( struct _tuple6*)(( struct Cyc_List_List*) _check_null( _temp633))->hd)).f1;
_temp644;}); _temp643;}); _temp640.tq=(*(( struct _tuple6*)(( struct Cyc_List_List*)
_check_null( _temp633))->hd)).f2; _temp640.type=( void*)(*(( struct _tuple6*)((
struct Cyc_List_List*) _check_null( _temp633))->hd)).f3; _temp640.initializer= 0;
_temp640.rgn=({ struct Cyc_Core_Opt* _temp641=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt) * 1); _temp641[ 0]=({ struct Cyc_Core_Opt _temp642;
_temp642.v=( void*) param_rgn; _temp642;}); _temp641;}); _temp640.attributes= 0;
_temp640;}); _temp639;}); _temp632=({ struct Cyc_List_List* _temp635=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp635[ 0]=({
struct Cyc_List_List _temp636; _temp636.hd=( void*) _temp634; _temp636.tl=
_temp632; _temp636;}); _temp635;}); locals=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key, void* data)) Cyc_Dict_insert)( locals,(*((
struct _tuple6*)(( struct Cyc_List_List*) _check_null( _temp633))->hd)).f1,(
void*)({ struct Cyc_Absyn_Param_b_struct* _temp637=( struct Cyc_Absyn_Param_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Param_b_struct)); _temp637[ 0]=({ struct Cyc_Absyn_Param_b_struct
_temp638; _temp638.tag= Cyc_Absyn_Param_b; _temp638.f1= _temp634; _temp638;});
_temp637;}));}} if( fd->cyc_varargs != 0){ int _temp647; void* _temp649; void*
_temp651; struct Cyc_Absyn_Tqual _temp653; struct Cyc_Core_Opt* _temp655; struct
Cyc_Absyn_VarargInfo _temp645=*(( struct Cyc_Absyn_VarargInfo*) _check_null( fd->cyc_varargs));
_LL656: _temp655= _temp645.name; goto _LL654; _LL654: _temp653= _temp645.tq;
goto _LL652; _LL652: _temp651=( void*) _temp645.type; goto _LL650; _LL650:
_temp649=( void*) _temp645.rgn; goto _LL648; _LL648: _temp647= _temp645.inject;
goto _LL646; _LL646: if( _temp655 != 0){ void* _temp657= Cyc_Absyn_tagged_typ(
_temp651, _temp649, _temp653); struct Cyc_Absyn_Vardecl* _temp658=({ struct Cyc_Absyn_Vardecl*
_temp662=( struct Cyc_Absyn_Vardecl*) GC_malloc( sizeof( struct Cyc_Absyn_Vardecl)
* 1); _temp662[ 0]=({ struct Cyc_Absyn_Vardecl _temp663; _temp663.sc=( void*)((
void*) Cyc_Absyn_Public); _temp663.name=({ struct _tuple0* _temp666=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0) * 1); _temp666[ 0]=({ struct
_tuple0 _temp667; _temp667.f1=( void*) Cyc_Absyn_Loc_n; _temp667.f2=( struct
_tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( _temp655))->v; _temp667;});
_temp666;}); _temp663.tq= Cyc_Absyn_empty_tqual(); _temp663.type=( void*)
_temp657; _temp663.initializer= 0; _temp663.rgn=({ struct Cyc_Core_Opt* _temp664=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1); _temp664[ 0]=({
struct Cyc_Core_Opt _temp665; _temp665.v=( void*) param_rgn; _temp665;});
_temp664;}); _temp663.attributes= 0; _temp663;}); _temp662;}); _temp632=({
struct Cyc_List_List* _temp659=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp659->hd=( void*) _temp658; _temp659->tl= _temp632;
_temp659;}); locals=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key, void* data)) Cyc_Dict_insert)( locals,( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( _temp655))->v,( void*)({ struct Cyc_Absyn_Param_b_struct*
_temp660=( struct Cyc_Absyn_Param_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Param_b_struct));
_temp660[ 0]=({ struct Cyc_Absyn_Param_b_struct _temp661; _temp661.tag= Cyc_Absyn_Param_b;
_temp661.f1= _temp658; _temp661;}); _temp660;}));} else{ Cyc_Tcutil_terr( loc,
_tag_arr("missing name for varargs", sizeof( unsigned char), 25u));}} fd->param_vardecls=({
struct Cyc_Core_Opt* _temp668=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt) * 1); _temp668[ 0]=({ struct Cyc_Core_Opt _temp669; _temp669.v=(
void*) _temp632; _temp669;}); _temp668;}); return({ struct Cyc_Tcenv_Fenv*
_temp670=( struct Cyc_Tcenv_Fenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Fenv));
_temp670->shared=({ struct Cyc_Tcenv_SharedFenv* _temp678=( struct Cyc_Tcenv_SharedFenv*)
GC_malloc( sizeof( struct Cyc_Tcenv_SharedFenv)); _temp678->return_typ=( void*)((
void*) fd->ret_type); _temp678->seen_labels=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp678->needed_labels=(( struct Cyc_SlowDict_Dict*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_SlowDict_empty)( Cyc_String_zstrptrcmp);
_temp678;}); _temp670->type_vars= _temp630; _temp670->region_order= _temp631;
_temp670->locals= locals; _temp670->encloser= fd->body; _temp670->ctrl_env=({
struct Cyc_Tcenv_CtrlEnv* _temp677=( struct Cyc_Tcenv_CtrlEnv*) GC_malloc(
sizeof( struct Cyc_Tcenv_CtrlEnv)); _temp677->continue_stmt=( void*)(( void*)
Cyc_Tcenv_NotLoop_j); _temp677->break_stmt=( void*)(( void*) Cyc_Tcenv_NotLoop_j);
_temp677->fallthru_clause= 0; _temp677->next_stmt=( void*)(( void*) Cyc_Tcenv_FnEnd_j);
_temp677->try_depth= 0; _temp677;}); _temp670->capability=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp671=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp671[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp672; _temp672.tag= Cyc_Absyn_JoinEff; _temp672.f1=({ struct Cyc_List_List*
_temp673=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp673->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp675=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp675[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp676; _temp676.tag= Cyc_Absyn_AccessEff;
_temp676.f1=( void*) param_rgn; _temp676;}); _temp675;})); _temp673->tl=({
struct Cyc_List_List* _temp674=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp674->hd=( void*)(( void*)(( struct Cyc_Core_Opt*)
_check_null( fd->effect))->v); _temp674->tl= 0; _temp674;}); _temp673;});
_temp672;}); _temp671;})); _temp670->curr_rgn=( void*) param_rgn; _temp670;});}}