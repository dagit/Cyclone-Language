 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
extern unsigned char Cyc_Core_Invalid_argument[ 21u]; struct Cyc_Core_Invalid_argument_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, unsigned int);
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; extern struct Cyc_List_List* Cyc_List_list(
struct _tagged_arr); extern unsigned char Cyc_List_List_empty[ 15u]; extern
struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x);
extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append( struct
Cyc_List_List* x, struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u];
extern struct Cyc_List_List* Cyc_List_filter( int(* f)( void*), struct Cyc_List_List*
x); extern unsigned char Cyc_Arg_Bad[ 8u]; struct Cyc_Arg_Bad_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Arg_Error[
10u]; static const int Cyc_Arg_Unit_spec= 0; struct Cyc_Arg_Unit_spec_struct{
int tag; void(* f1)(); } ; static const int Cyc_Arg_Flag_spec= 1; struct Cyc_Arg_Flag_spec_struct{
int tag; void(* f1)( struct _tagged_arr); } ; static const int Cyc_Arg_FlagString_spec=
2; struct Cyc_Arg_FlagString_spec_struct{ int tag; void(* f1)( struct
_tagged_arr, struct _tagged_arr); } ; static const int Cyc_Arg_Set_spec= 3;
struct Cyc_Arg_Set_spec_struct{ int tag; int* f1; } ; static const int Cyc_Arg_Clear_spec=
4; struct Cyc_Arg_Clear_spec_struct{ int tag; int* f1; } ; static const int Cyc_Arg_String_spec=
5; struct Cyc_Arg_String_spec_struct{ int tag; void(* f1)( struct _tagged_arr);
} ; static const int Cyc_Arg_Int_spec= 6; struct Cyc_Arg_Int_spec_struct{ int
tag; void(* f1)( int); } ; static const int Cyc_Arg_Rest_spec= 7; struct Cyc_Arg_Rest_spec_struct{
int tag; void(* f1)( struct _tagged_arr); } ; extern void Cyc_Arg_parse( struct
Cyc_List_List* specs, void(* anonfun)( struct _tagged_arr), struct _tagged_arr
errmsg, struct _tagged_arr args); struct Cyc_Std___sFILE; extern struct Cyc_Std___sFILE*
Cyc_Std_stdout; extern struct Cyc_Std___sFILE* Cyc_Std_stderr; extern int Cyc_Std_remove(
struct _tagged_arr); extern int Cyc_Std_fclose( struct Cyc_Std___sFILE*); extern
int Cyc_Std_fflush( struct Cyc_Std___sFILE*); extern struct Cyc_Std___sFILE* Cyc_Std_fopen(
struct _tagged_arr __filename, struct _tagged_arr __modes); extern unsigned char
Cyc_Std_FileCloseError[ 19u]; extern unsigned char Cyc_Std_FileOpenError[ 18u];
struct Cyc_Std_FileOpenError_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern struct Cyc_Std___sFILE* Cyc_Std_file_open( struct _tagged_arr fname,
struct _tagged_arr mode); extern void Cyc_Std_file_close( struct Cyc_Std___sFILE*);
static const int Cyc_Std_String_pa= 0; struct Cyc_Std_String_pa_struct{ int tag;
struct _tagged_arr f1; } ; static const int Cyc_Std_Int_pa= 1; struct Cyc_Std_Int_pa_struct{
int tag; unsigned int f1; } ; static const int Cyc_Std_Double_pa= 2; struct Cyc_Std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa= 3; struct Cyc_Std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_Std_fprintf( struct Cyc_Std___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); extern int Cyc_Std_printf( struct
_tagged_arr fmt, struct _tagged_arr); extern struct _tagged_arr Cyc_Std_aprintf(
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_Std_ShortPtr_sa=
0; struct Cyc_Std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_Std_IntPtr_sa= 2; struct Cyc_Std_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_Std_UIntPtr_sa= 3; struct Cyc_Std_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Std_StringPtr_sa= 4; struct
Cyc_Std_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static const
int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{ int tag; double*
f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; extern int system( unsigned char*); struct Cyc_Std__Div{
int quot; int rem; } ; struct Cyc_Std__Ldiv{ int quot; int rem; } ; extern int
abs( int __x); extern int atexit( void(* __func)()); extern struct Cyc_Std__Div
div( int __numer, int __denom); extern struct Cyc_Std__Ldiv ldiv( int __numer,
int __denom); extern int random(); extern void srandom( unsigned int __seed);
extern int rand(); extern void srand( unsigned int __seed); extern int rand_r(
unsigned int* __seed); extern int grantpt( int __fd); extern int unlockpt( int
__fd); extern struct _tagged_arr Cyc_Std_getenv( struct _tagged_arr); extern
unsigned int Cyc_Std_strlen( struct _tagged_arr s); extern int Cyc_Std_strcmp(
struct _tagged_arr s1, struct _tagged_arr s2); extern struct _tagged_arr Cyc_Std_strconcat(
struct _tagged_arr, struct _tagged_arr); extern struct _tagged_arr Cyc_Std_strconcat_l(
struct Cyc_List_List*); extern struct _tagged_arr Cyc_Std_str_sepstr( struct Cyc_List_List*,
struct _tagged_arr); extern struct _tagged_arr Cyc_Std_substring( struct
_tagged_arr, int ofs, unsigned int n); extern struct _tagged_arr Cyc_Std_strchr(
struct _tagged_arr s, unsigned char c); struct Cyc_Dict_Dict; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
extern struct _tagged_arr Cyc_Filename_concat( struct _tagged_arr, struct
_tagged_arr); extern struct _tagged_arr Cyc_Filename_chop_extension( struct
_tagged_arr); extern int Cyc_Filename_check_suffix( struct _tagged_arr, struct
_tagged_arr); struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct
_tagged_arr line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[
9u]; extern void Cyc_Position_reset_position( struct _tagged_arr); struct Cyc_Position_Segment;
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; extern
int Cyc_Position_error_p(); struct _tuple0{ void* f1; struct _tagged_arr* f2; }
; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref; struct
Cyc_Absyn_PtrInfo; struct Cyc_Absyn_VarargInfo; struct Cyc_Absyn_FnInfo; struct
Cyc_Absyn_TunionInfo; struct Cyc_Absyn_TunionFieldInfo; struct Cyc_Absyn_VarargCallInfo;
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
type; int inject; } ; struct Cyc_Absyn_FnInfo{ struct Cyc_List_List* tvars;
struct Cyc_Core_Opt* effect; void* ret_typ; struct Cyc_List_List* args; int
c_varargs; struct Cyc_Absyn_VarargInfo* cyc_varargs; struct Cyc_List_List*
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
Cyc_Absyn_Tqual tq; void* type; struct Cyc_Absyn_Exp* width; struct Cyc_List_List*
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
15u]; extern unsigned char Cyc_Lexing_Error[ 10u]; struct Cyc_Lexing_Error_struct{
unsigned char* tag; struct _tagged_arr f1; } ; struct Cyc_Lexing_lexbuf{ void(*
refill_buff)( struct Cyc_Lexing_lexbuf*); void* refill_state; struct _tagged_arr
lex_buffer; int lex_buffer_len; int lex_abs_pos; int lex_start_pos; int
lex_curr_pos; int lex_last_pos; int lex_last_action; int lex_eof_reached; } ;
struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_arr,
int, void*); void* read_fun_state; } ; struct Cyc_Lexing_lex_tables{ struct
_tagged_arr lex_base; struct _tagged_arr lex_backtrk; struct _tagged_arr
lex_default; struct _tagged_arr lex_trans; struct _tagged_arr lex_check; } ;
extern struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Std___sFILE* f);
struct Cyc_Declaration_spec; struct Cyc_Declarator; struct Cyc_Abstractdeclarator;
extern unsigned char Cyc_AbstractDeclarator_tok[ 27u]; struct Cyc_AbstractDeclarator_tok_struct{
unsigned char* tag; struct Cyc_Abstractdeclarator* f1; } ; extern unsigned char
Cyc_AttributeList_tok[ 22u]; struct Cyc_AttributeList_tok_struct{ unsigned char*
tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_Attribute_tok[ 18u];
struct Cyc_Attribute_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_Bool_tok[ 13u]; struct Cyc_Bool_tok_struct{ unsigned char* tag;
int f1; } ; extern unsigned char Cyc_Char_tok[ 13u]; struct Cyc_Char_tok_struct{
unsigned char* tag; unsigned char f1; } ; extern unsigned char Cyc_DeclList_tok[
17u]; struct Cyc_DeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_DeclSpec_tok[ 17u]; struct Cyc_DeclSpec_tok_struct{
unsigned char* tag; struct Cyc_Declaration_spec* f1; } ; extern unsigned char
Cyc_DeclaratorExpoptList_tok[ 29u]; struct Cyc_DeclaratorExpoptList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_DeclaratorExpopt_tok[
25u]; struct _tuple3{ struct Cyc_Declarator* f1; struct Cyc_Absyn_Exp* f2; } ;
struct Cyc_DeclaratorExpopt_tok_struct{ unsigned char* tag; struct _tuple3* f1;
} ; extern unsigned char Cyc_Declarator_tok[ 19u]; struct Cyc_Declarator_tok_struct{
unsigned char* tag; struct Cyc_Declarator* f1; } ; extern unsigned char Cyc_DesignatorList_tok[
23u]; struct Cyc_DesignatorList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Designator_tok[ 19u]; struct Cyc_Designator_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_EnumfieldList_tok[ 22u];
struct Cyc_EnumfieldList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Enumfield_tok[ 18u]; struct Cyc_Enumfield_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Enumfield* f1; } ; extern unsigned char Cyc_ExpList_tok[
16u]; struct Cyc_ExpList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Exp_tok[ 12u]; struct Cyc_Exp_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Exp* f1; } ; extern unsigned char Cyc_FieldPatternList_tok[
25u]; struct Cyc_FieldPatternList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_FieldPattern_tok[ 21u]; struct _tuple4{ struct
Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; struct Cyc_FieldPattern_tok_struct{
unsigned char* tag; struct _tuple4* f1; } ; extern unsigned char Cyc_FnDecl_tok[
15u]; struct Cyc_FnDecl_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Fndecl*
f1; } ; extern unsigned char Cyc_IdList_tok[ 15u]; struct Cyc_IdList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_InitDeclList_tok[
21u]; struct Cyc_InitDeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_InitDecl_tok[ 17u]; struct Cyc_InitDecl_tok_struct{
unsigned char* tag; struct _tuple3* f1; } ; extern unsigned char Cyc_InitializerList_tok[
24u]; struct Cyc_InitializerList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Int_tok[ 12u]; struct _tuple5{ void* f1; int f2;
} ; struct Cyc_Int_tok_struct{ unsigned char* tag; struct _tuple5* f1; } ;
extern unsigned char Cyc_Kind_tok[ 13u]; struct Cyc_Kind_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Okay_tok[ 13u];
extern unsigned char Cyc_ParamDeclListBool_tok[ 26u]; struct _tuple6{ struct Cyc_List_List*
f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt* f4; struct Cyc_List_List*
f5; } ; struct Cyc_ParamDeclListBool_tok_struct{ unsigned char* tag; struct
_tuple6* f1; } ; extern unsigned char Cyc_ParamDeclList_tok[ 22u]; struct Cyc_ParamDeclList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_ParamDecl_tok[
18u]; struct Cyc_ParamDecl_tok_struct{ unsigned char* tag; struct _tuple1* f1; }
; extern unsigned char Cyc_PatternList_tok[ 20u]; struct Cyc_PatternList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_Pattern_tok[
16u]; struct Cyc_Pattern_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Pat*
f1; } ; extern unsigned char Cyc_Pointer_Sort_tok[ 21u]; struct Cyc_Pointer_Sort_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Primop_tok[ 15u];
struct Cyc_Primop_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_Primopopt_tok[ 18u]; struct Cyc_Primopopt_tok_struct{
unsigned char* tag; struct Cyc_Core_Opt* f1; } ; extern unsigned char Cyc_QualId_tok[
15u]; struct Cyc_QualId_tok_struct{ unsigned char* tag; struct _tuple0* f1; } ;
extern unsigned char Cyc_QualSpecList_tok[ 21u]; struct _tuple7{ struct Cyc_Absyn_Tqual
f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; } ; struct Cyc_QualSpecList_tok_struct{
unsigned char* tag; struct _tuple7* f1; } ; extern unsigned char Cyc_Rgnorder_tok[
17u]; struct Cyc_Rgnorder_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Scope_tok[ 14u]; struct Cyc_Scope_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Short_tok[ 14u];
struct Cyc_Short_tok_struct{ unsigned char* tag; short f1; } ; extern
unsigned char Cyc_Stmt_tok[ 13u]; struct Cyc_Stmt_tok_struct{ unsigned char* tag;
struct Cyc_Absyn_Stmt* f1; } ; extern unsigned char Cyc_StorageClass_tok[ 21u];
struct Cyc_StorageClass_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_String_tok[ 15u]; struct Cyc_String_tok_struct{ unsigned char*
tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Stringopt_tok[ 18u];
struct Cyc_Stringopt_tok_struct{ unsigned char* tag; struct Cyc_Core_Opt* f1; }
; extern unsigned char Cyc_StructFieldDeclListList_tok[ 32u]; struct Cyc_StructFieldDeclListList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_StructFieldDeclList_tok[
28u]; struct Cyc_StructFieldDeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_StructOrUnion_tok[ 22u]; struct Cyc_StructOrUnion_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_SwitchCClauseList_tok[
26u]; struct Cyc_SwitchCClauseList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_SwitchClauseList_tok[ 25u]; struct Cyc_SwitchClauseList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_TunionFieldList_tok[
24u]; struct Cyc_TunionFieldList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_TunionField_tok[ 20u]; struct Cyc_TunionField_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Tunionfield* f1; } ; extern unsigned char
Cyc_TypeList_tok[ 17u]; struct Cyc_TypeList_tok_struct{ unsigned char* tag;
struct Cyc_List_List* f1; } ; extern unsigned char Cyc_TypeModifierList_tok[ 25u];
struct Cyc_TypeModifierList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_TypeOpt_tok[ 16u]; struct Cyc_TypeOpt_tok_struct{
unsigned char* tag; struct Cyc_Core_Opt* f1; } ; extern unsigned char Cyc_TypeQual_tok[
17u]; struct Cyc_TypeQual_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Tqual
f1; } ; extern unsigned char Cyc_TypeSpecifier_tok[ 22u]; struct Cyc_TypeSpecifier_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Type_tok[ 13u];
struct Cyc_Type_tok_struct{ unsigned char* tag; void* f1; } ; struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
} ; struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{
int expand_typedefs: 1; int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1;
int decls_first: 1; int rewrite_temp_tvars: 1; int print_all_tvars: 1; int
print_all_kinds: 1; int print_using_stmts: 1; int print_externC_stmts: 1; int
print_full_evars: 1; int use_curr_namespace: 1; struct Cyc_List_List*
curr_namespace; } ; extern void Cyc_Absynpp_set_params( struct Cyc_Absynpp_Params*
fs); extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r; extern struct
Cyc_Absynpp_Params Cyc_Absynpp_c_params_r; extern void Cyc_Absynpp_decllist2file(
struct Cyc_List_List* tdl, struct Cyc_Std___sFILE* f); extern void Cyc_Absyndump_set_params(
struct Cyc_Absynpp_Params* fs); extern void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List* tdl, struct Cyc_Std___sFILE* f); struct Cyc_Set_Set;
extern unsigned char Cyc_Set_Absent[ 11u]; static const int Cyc_Tcenv_VarRes= 0;
struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes=
1; struct Cyc_Tcenv_StructRes_struct{ int tag; struct Cyc_Absyn_Structdecl* f1;
} ; static const int Cyc_Tcenv_TunionRes= 2; struct Cyc_Tcenv_TunionRes_struct{
int tag; struct Cyc_Absyn_Tuniondecl* f1; struct Cyc_Absyn_Tunionfield* f2; } ;
static const int Cyc_Tcenv_EnumRes= 3; struct Cyc_Tcenv_EnumRes_struct{ int tag;
struct Cyc_Absyn_Enumdecl* f1; struct Cyc_Absyn_Enumfield* f2; } ; struct Cyc_Tcenv_Genv{
struct Cyc_Set_Set* namespaces; struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict*
uniondecls; struct Cyc_Dict_Dict* tuniondecls; struct Cyc_Dict_Dict* enumdecls;
struct Cyc_Dict_Dict* typedefs; struct Cyc_Dict_Dict* ordinaries; struct Cyc_List_List*
availables; } ; struct Cyc_Tcenv_Fenv; static const int Cyc_Tcenv_NotLoop_j= 0;
static const int Cyc_Tcenv_CaseEnd_j= 1; static const int Cyc_Tcenv_FnEnd_j= 2;
static const int Cyc_Tcenv_Stmt_j= 0; struct Cyc_Tcenv_Stmt_j_struct{ int tag;
struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Tcenv_Outermost= 0; struct
Cyc_Tcenv_Outermost_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_Frame=
1; struct Cyc_Tcenv_Frame_struct{ int tag; void* f1; void* f2; } ; static const
int Cyc_Tcenv_Hidden= 2; struct Cyc_Tcenv_Hidden_struct{ int tag; void* f1; void*
f2; } ; struct Cyc_Tcenv_Tenv{ struct Cyc_List_List* ns; struct Cyc_Dict_Dict*
ae; struct Cyc_Core_Opt* le; } ; extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init();
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_flush_warnings();
extern void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int var_default_init, struct
Cyc_List_List* ds); extern struct Cyc_List_List* Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List*); extern struct Cyc_List_List* Cyc_Toc_toc( struct Cyc_List_List*
ds); extern struct Cyc_List_List* Cyc_Tovc_tovc( struct Cyc_List_List* decls);
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
int tag; struct Cyc_Dict_Dict* f1; } ; struct Cyc_NewControlFlow_AnalEnv{ struct
Cyc_Dict_Dict* roots; int in_try; void* tryflow; } ; extern void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds); struct Cyc_Interface_I; extern struct Cyc_Interface_I*
Cyc_Interface_empty(); extern struct Cyc_Interface_I* Cyc_Interface_final();
extern struct Cyc_Interface_I* Cyc_Interface_extract( struct Cyc_Dict_Dict* ae);
struct _tuple8{ struct _tagged_arr f1; struct _tagged_arr f2; } ; extern int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple8* info);
extern struct Cyc_Interface_I* Cyc_Interface_get_and_merge_list( struct Cyc_Interface_I*(*
get)( void*), struct Cyc_List_List* la, struct Cyc_List_List* linfo); extern
struct Cyc_Interface_I* Cyc_Interface_parse( struct Cyc_Std___sFILE*); extern
void Cyc_Interface_save( struct Cyc_Interface_I*, struct Cyc_Std___sFILE*);
extern struct Cyc_Interface_I* Cyc_Interface_load( struct Cyc_Std___sFILE*);
static unsigned char _temp0[ 6u]="0.1.1"; static struct _tagged_arr Cyc_version={
_temp0, _temp0, _temp0 +  6u}; extern void Cyc_Lex_lex_init(); static int Cyc_pp_r=
0; static int Cyc_noexpand_r= 0; static int Cyc_noshake_r= 0; static int Cyc_stop_after_cpp_r=
0; static int Cyc_parseonly_r= 0; static int Cyc_tc_r= 0; static int Cyc_ic_r= 0;
static int Cyc_toc_r= 0; static int Cyc_stop_after_objectfile_r= 0; static int
Cyc_stop_after_asmfile_r= 0; static int Cyc_tovc_r= 0; static int Cyc_v_r= 0;
static int Cyc_save_temps_r= 0; static int Cyc_save_c_r= 0; static int Cyc_nogc_r=
0; static int Cyc_pa_r= 0; static int Cyc_add_cyc_namespace_r= 1; static int Cyc_print_full_evars_r=
0; static int Cyc_print_all_tvars_r= 0; static int Cyc_print_all_kinds_r= 0;
static struct _tagged_arr* Cyc_output_file= 0; static void Cyc_set_output_file(
struct _tagged_arr s){ Cyc_output_file=({ struct _tagged_arr* _temp1=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp1[ 0]= s; _temp1;});}
extern unsigned char* Cdef_inc_path; extern unsigned char* Cdef_lib_path; extern
unsigned char* Carch; static unsigned char _temp2[ 33u]="gcc -x c -E -U__GNUC__ -nostdinc";
static struct _tagged_arr Cyc_cpp={ _temp2, _temp2, _temp2 +  33u}; static void
Cyc_set_cpp( struct _tagged_arr s){ Cyc_cpp= s;} static struct Cyc_List_List*
Cyc_cppargs= 0; static void Cyc_add_cpparg( struct _tagged_arr s){ Cyc_cppargs=({
struct Cyc_List_List* _temp3=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp3->hd=( void*)({ struct _tagged_arr* _temp4=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp4[ 0]= s; _temp4;});
_temp3->tl= Cyc_cppargs; _temp3;});} static void Cyc_print_version(){({ struct
Cyc_Std_String_pa_struct _temp6; _temp6.tag= Cyc_Std_String_pa; _temp6.f1=(
struct _tagged_arr) Cyc_version;{ void* _temp5[ 1u]={& _temp6}; Cyc_Std_printf(
_tag_arr("The Cyclone compiler, version %s\n", sizeof( unsigned char), 34u),
_tag_arr( _temp5, sizeof( void*), 1u));}});({ struct Cyc_Std_String_pa_struct
_temp8; _temp8.tag= Cyc_Std_String_pa; _temp8.f1=( struct _tagged_arr)
Cstring_to_string( Carch);{ void* _temp7[ 1u]={& _temp8}; Cyc_Std_printf(
_tag_arr("Compiled for architecture: %s\n", sizeof( unsigned char), 31u),
_tag_arr( _temp7, sizeof( void*), 1u));}});({ struct Cyc_Std_String_pa_struct
_temp10; _temp10.tag= Cyc_Std_String_pa; _temp10.f1=( struct _tagged_arr)
Cstring_to_string( Cdef_lib_path);{ void* _temp9[ 1u]={& _temp10}; Cyc_Std_printf(
_tag_arr("Standard library directory: %s\n", sizeof( unsigned char), 32u),
_tag_arr( _temp9, sizeof( void*), 1u));}});({ struct Cyc_Std_String_pa_struct
_temp12; _temp12.tag= Cyc_Std_String_pa; _temp12.f1=( struct _tagged_arr)
Cstring_to_string( Cdef_inc_path);{ void* _temp11[ 1u]={& _temp12}; Cyc_Std_printf(
_tag_arr("Standard include directory: %s\n", sizeof( unsigned char), 32u),
_tag_arr( _temp11, sizeof( void*), 1u));}}); exit( 0);} static int Cyc_is_cyclone_sourcefile(
struct _tagged_arr s){ unsigned int _temp13= Cyc_Std_strlen( s); if( _temp13 <= 
4){ return 0;} else{ return Cyc_Std_strcmp( _tagged_arr_plus( s, sizeof(
unsigned char),( int)( _temp13 -  4)), _tag_arr(".cyc", sizeof( unsigned char),
5u)) ==  0;}} static struct Cyc_List_List* Cyc_cyclone_lib_path= 0; static void
Cyc_add_cyclone_lib_path( struct _tagged_arr s){ unsigned int _temp14= Cyc_Std_strlen(
s); if( _temp14 <=  2){ return;}{ struct _tagged_arr _temp15=( struct
_tagged_arr) Cyc_Std_substring( s, 2, _temp14 -  2); Cyc_cyclone_lib_path=({
struct Cyc_List_List* _temp16=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp16->hd=( void*)({ struct _tagged_arr* _temp17=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp17[ 0]= _temp15;
_temp17;}); _temp16->tl= Cyc_cyclone_lib_path; _temp16;});}} static struct Cyc_List_List*
Cyc_ccargs= 0; static void Cyc_add_ccarg( struct _tagged_arr s){ Cyc_ccargs=({
struct Cyc_List_List* _temp18=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp18->hd=( void*)({ struct _tagged_arr* _temp19=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp19[ 0]= s; _temp19;});
_temp18->tl= Cyc_ccargs; _temp18;});} static void Cyc_add_ccarg2( struct
_tagged_arr flag, struct _tagged_arr arg){ Cyc_ccargs=({ struct Cyc_List_List*
_temp20=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp20->hd=( void*)({ struct _tagged_arr* _temp23=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp23[ 0]= arg; _temp23;}); _temp20->tl=({
struct Cyc_List_List* _temp21=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp21->hd=( void*)({ struct _tagged_arr* _temp22=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp22[ 0]= flag;
_temp22;}); _temp21->tl= Cyc_ccargs; _temp21;}); _temp20;});} static struct Cyc_List_List*
Cyc_libargs= 0; static void Cyc_add_libarg( struct _tagged_arr s){ Cyc_libargs=({
struct Cyc_List_List* _temp24=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp24->hd=( void*)({ struct _tagged_arr* _temp25=( struct
_tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp25[ 0]= s; _temp25;});
_temp24->tl= Cyc_libargs; _temp24;});} static void Cyc_set_save_temps(){ Cyc_save_temps_r=
1; Cyc_add_ccarg( _tag_arr("-save-temps", sizeof( unsigned char), 12u));} static
int Cyc_produce_dependencies= 0; static void Cyc_set_produce_dependencies(){ Cyc_stop_after_cpp_r=
1; Cyc_produce_dependencies= 1; Cyc_add_cpparg( _tag_arr("-M", sizeof(
unsigned char), 3u));} static struct _tagged_arr* Cyc_dependencies_target= 0;
static void Cyc_set_dependencies_target( struct _tagged_arr s){ Cyc_dependencies_target=({
struct _tagged_arr* _temp26=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp26[ 0]= s; _temp26;});} static void Cyc_set_stop_after_objectfile(){
Cyc_stop_after_objectfile_r= 1; Cyc_add_ccarg( _tag_arr("-c", sizeof(
unsigned char), 3u));} static void Cyc_set_nocyc(){ Cyc_add_cyc_namespace_r= 0;
Cyc_add_ccarg( _tag_arr("-DNO_CYC_PREFIX", sizeof( unsigned char), 16u));}
static void Cyc_set_pa(){ Cyc_pa_r= 1; Cyc_add_ccarg( _tag_arr("-DCYC_REGION_PROFILE",
sizeof( unsigned char), 21u));} static void Cyc_set_stop_after_asmfile(){ Cyc_stop_after_asmfile_r=
1; Cyc_add_ccarg( _tag_arr("-S", sizeof( unsigned char), 3u));} static const int
Cyc_DEFAULTINPUT= 0; static const int Cyc_CYCLONEFILE= 1; static void* Cyc_intype=(
void*) 0u; static void Cyc_set_inputtype( struct _tagged_arr s){ if( Cyc_Std_strcmp(
s, _tag_arr("cyc", sizeof( unsigned char), 4u)) ==  0){ Cyc_intype=( void*) Cyc_CYCLONEFILE;}
else{ if( Cyc_Std_strcmp( s, _tag_arr("none", sizeof( unsigned char), 5u)) ==  0){
Cyc_intype=( void*) Cyc_DEFAULTINPUT;} else{({ struct Cyc_Std_String_pa_struct
_temp28; _temp28.tag= Cyc_Std_String_pa; _temp28.f1=( struct _tagged_arr) s;{
void* _temp27[ 1u]={& _temp28}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Input type '%s' not supported\n",
sizeof( unsigned char), 31u), _tag_arr( _temp27, sizeof( void*), 1u));}});}}}
static struct Cyc_List_List* Cyc_cyclone_files= 0; static void Cyc_add_other(
struct _tagged_arr s){ if( Cyc_intype == ( void*) Cyc_CYCLONEFILE? 1: Cyc_is_cyclone_sourcefile(
s)){ Cyc_cyclone_files=({ struct Cyc_List_List* _temp29=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp29->hd=( void*)({ struct
_tagged_arr* _temp30=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp30[ 0]= s; _temp30;}); _temp29->tl= Cyc_cyclone_files;
_temp29;});{ struct _tagged_arr _temp31= Cyc_Filename_chop_extension( s); struct
_tagged_arr _temp32= Cyc_Std_strconcat(( struct _tagged_arr) _temp31, _tag_arr(".c",
sizeof( unsigned char), 3u)); Cyc_add_ccarg(( struct _tagged_arr) _temp32);}}
else{ Cyc_add_ccarg( s);}} static void Cyc_remove_file( struct _tagged_arr s){
if( Cyc_save_temps_r){ return;} else{ Cyc_Std_remove( s);}} int Cyc_compile_failure=
0; struct Cyc_Std___sFILE* Cyc_try_file_open( struct _tagged_arr filename,
struct _tagged_arr mode, struct _tagged_arr msg_part){ struct _handler_cons
_temp33; _push_handler(& _temp33);{ int _temp35= 0; if( setjmp( _temp33.handler)){
_temp35= 1;} if( ! _temp35){{ struct Cyc_Std___sFILE* _temp36=( struct Cyc_Std___sFILE*)
Cyc_Std_file_open( filename, mode); _npop_handler( 0u); return _temp36;};
_pop_handler();} else{ void* _temp34=( void*) _exn_thrown; void* _temp38=
_temp34; _LL40: goto _LL41; _LL42: goto _LL43; _LL41: Cyc_compile_failure= 1;({
struct Cyc_Std_String_pa_struct _temp46; _temp46.tag= Cyc_Std_String_pa; _temp46.f1=(
struct _tagged_arr) filename;{ struct Cyc_Std_String_pa_struct _temp45; _temp45.tag=
Cyc_Std_String_pa; _temp45.f1=( struct _tagged_arr) msg_part;{ void* _temp44[ 2u]={&
_temp45,& _temp46}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\nError: couldn't open %s %s\n",
sizeof( unsigned char), 29u), _tag_arr( _temp44, sizeof( void*), 2u));}}}); Cyc_Std_fflush((
struct Cyc_Std___sFILE*) Cyc_Std_stderr); return 0; _LL43:( void) _throw(
_temp38); _LL39:;}}} struct Cyc_List_List* Cyc_do_stage( struct _tagged_arr
stage_name, struct Cyc_List_List*(* f)( void*, struct Cyc_List_List*), void* env,
struct Cyc_List_List* tds){ struct _tagged_arr exn_string= _tag_arr("", sizeof(
unsigned char), 1u); struct _tagged_arr explain_string= _tag_arr("", sizeof(
unsigned char), 1u); int other_exn= 0; void* ex=( void*)({ struct Cyc_Core_Impossible_struct*
_temp75=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp75[ 0]=({ struct Cyc_Core_Impossible_struct _temp76; _temp76.tag= Cyc_Core_Impossible;
_temp76.f1= _tag_arr("", sizeof( unsigned char), 1u); _temp76;}); _temp75;});
struct Cyc_List_List* _temp47= 0;{ struct _handler_cons _temp48; _push_handler(&
_temp48);{ int _temp50= 0; if( setjmp( _temp48.handler)){ _temp50= 1;} if( !
_temp50){ _temp47= f( env, tds);; _pop_handler();} else{ void* _temp49=( void*)
_exn_thrown; void* _temp52= _temp49; struct _tagged_arr _temp64; struct
_tagged_arr _temp66; _LL54: if(*(( void**) _temp52) ==  Cyc_Core_Impossible){
_LL65: _temp64=(( struct Cyc_Core_Impossible_struct*) _temp52)->f1; goto _LL55;}
else{ goto _LL56;} _LL56: if( _temp52 ==  Cyc_Dict_Absent){ goto _LL57;} else{
goto _LL58;} _LL58: if(*(( void**) _temp52) ==  Cyc_Core_Invalid_argument){
_LL67: _temp66=(( struct Cyc_Core_Invalid_argument_struct*) _temp52)->f1; goto
_LL59;} else{ goto _LL60;} _LL60: goto _LL61; _LL62: goto _LL63; _LL55:
exn_string= _tag_arr("Exception Core::Impossible", sizeof( unsigned char), 27u);
explain_string= _temp64; goto _LL53; _LL57: exn_string= _tag_arr("Exception Dict::Absent",
sizeof( unsigned char), 23u); goto _LL53; _LL59: exn_string= _tag_arr("Exception Core::Invalid_argument",
sizeof( unsigned char), 33u); explain_string= _temp66; goto _LL53; _LL61: ex=
_temp52; other_exn= 1; exn_string= _tag_arr("Uncaught exception", sizeof(
unsigned char), 19u); goto _LL53; _LL63:( void) _throw( _temp52); _LL53:;}}} if(
Cyc_Position_error_p()){ Cyc_compile_failure= 1;} if( Cyc_Std_strcmp( exn_string,
_tag_arr("", sizeof( unsigned char), 1u)) !=  0){ Cyc_compile_failure= 1;({
struct Cyc_Std_String_pa_struct _temp71; _temp71.tag= Cyc_Std_String_pa; _temp71.f1=(
struct _tagged_arr) explain_string;{ struct Cyc_Std_String_pa_struct _temp70;
_temp70.tag= Cyc_Std_String_pa; _temp70.f1=( struct _tagged_arr) stage_name;{
struct Cyc_Std_String_pa_struct _temp69; _temp69.tag= Cyc_Std_String_pa; _temp69.f1=(
struct _tagged_arr) exn_string;{ void* _temp68[ 3u]={& _temp69,& _temp70,&
_temp71}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\n%s thrown during %s: %s",
sizeof( unsigned char), 25u), _tag_arr( _temp68, sizeof( void*), 3u));}}}});}
if( Cyc_compile_failure){({ void* _temp72[ 0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("\nCOMPILATION FAILED!\n", sizeof( unsigned char), 22u), _tag_arr(
_temp72, sizeof( void*), 0u));}); Cyc_Std_fflush(( struct Cyc_Std___sFILE*) Cyc_Std_stderr);
if( other_exn){( int) _throw( ex);} return _temp47;} else{ if( Cyc_v_r){({
struct Cyc_Std_String_pa_struct _temp74; _temp74.tag= Cyc_Std_String_pa; _temp74.f1=(
struct _tagged_arr) stage_name;{ void* _temp73[ 1u]={& _temp74}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("%s completed.\n", sizeof( unsigned char), 15u),
_tag_arr( _temp73, sizeof( void*), 1u));}}); Cyc_Std_fflush(( struct Cyc_Std___sFILE*)
Cyc_Std_stderr); return _temp47;}} return _temp47;} struct Cyc_List_List* Cyc_do_parse(
struct Cyc_Std___sFILE* f, struct Cyc_List_List* ignore){ Cyc_Lex_lex_init();{
struct Cyc_List_List* _temp77= Cyc_Parse_parse_file( f); Cyc_Lex_lex_init();
return _temp77;}} struct Cyc_List_List* Cyc_do_typecheck( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* tds){ Cyc_Tc_tc( te, 1, tds); if( ! Cyc_noshake_r){
tds= Cyc_Tc_treeshake( te, tds);} return tds;} struct Cyc_List_List* Cyc_do_cfcheck(
int ignore, struct Cyc_List_List* tds){ Cyc_NewControlFlow_cf_check( tds);
return tds;} struct _tuple9{ struct Cyc_Tcenv_Tenv* f1; struct Cyc_Std___sFILE*
f2; struct Cyc_Std___sFILE* f3; } ; struct Cyc_List_List* Cyc_do_interface(
struct _tuple9* params, struct Cyc_List_List* tds){ struct _tuple9 _temp80;
struct Cyc_Std___sFILE* _temp81; struct Cyc_Std___sFILE* _temp83; struct Cyc_Tcenv_Tenv*
_temp85; struct _tuple9* _temp78= params; _temp80=* _temp78; _LL86: _temp85=
_temp80.f1; goto _LL84; _LL84: _temp83= _temp80.f2; goto _LL82; _LL82: _temp81=
_temp80.f3; goto _LL79; _LL79: { struct Cyc_Interface_I* _temp87= Cyc_Interface_extract(
_temp85->ae); if( _temp83 ==  0){ Cyc_Interface_save( _temp87, _temp81);} else{
struct Cyc_Interface_I* _temp88= Cyc_Interface_parse(( struct Cyc_Std___sFILE*)
_check_null( _temp83)); if( ! Cyc_Interface_is_subinterface( _temp88, _temp87,({
struct _tuple8* _temp89=( struct _tuple8*) _cycalloc( sizeof( struct _tuple8));
_temp89->f1= _tag_arr("written interface", sizeof( unsigned char), 18u); _temp89->f2=
_tag_arr("maximal interface", sizeof( unsigned char), 18u); _temp89;}))){ Cyc_compile_failure=
1;} else{ Cyc_Interface_save( _temp88, _temp81);}} return tds;}} struct Cyc_List_List*
Cyc_do_translate( int ignore, struct Cyc_List_List* tds){ return Cyc_Toc_toc(
tds);} struct Cyc_List_List* Cyc_do_tovc( int ignore, struct Cyc_List_List* tds){
return Cyc_Tovc_tovc( tds);} struct Cyc_List_List* Cyc_do_print( struct Cyc_Std___sFILE*
out_file, struct Cyc_List_List* tds){ struct Cyc_Absynpp_Params params_r; if(
Cyc_tc_r){ params_r= Cyc_Absynpp_cyc_params_r;} else{ params_r= Cyc_Absynpp_c_params_r;}
params_r.expand_typedefs= ! Cyc_noexpand_r; params_r.to_VC= Cyc_tovc_r; params_r.add_cyc_prefix=
Cyc_add_cyc_namespace_r; params_r.print_full_evars= Cyc_print_full_evars_r;
params_r.print_all_tvars= Cyc_print_all_tvars_r; params_r.print_all_kinds= Cyc_print_all_kinds_r;
if( Cyc_pp_r){ Cyc_Absynpp_set_params(& params_r); Cyc_Absynpp_decllist2file(
tds, out_file);} else{ Cyc_Absyndump_set_params(& params_r); Cyc_Absyndump_dumpdecllist2file(
tds, out_file);} Cyc_Std_fflush(( struct Cyc_Std___sFILE*) out_file); return tds;}
static struct Cyc_List_List* Cyc_cfiles= 0; static void Cyc_remove_cfiles(){ if(
! Cyc_save_c_r){ for( 0; Cyc_cfiles !=  0; Cyc_cfiles=(( struct Cyc_List_List*)
_check_null( Cyc_cfiles))->tl){ Cyc_remove_file(*(( struct _tagged_arr*)((
struct Cyc_List_List*) _check_null( Cyc_cfiles))->hd));}}} static struct Cyc_List_List*
Cyc_split_by_char( struct _tagged_arr s, unsigned char c){ if( s.curr == ((
struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ return 0;}{ struct Cyc_List_List*
_temp90= 0; unsigned int _temp91= Cyc_Std_strlen( s); while( _temp91 >  0) {
struct _tagged_arr _temp92= Cyc_Std_strchr( s, c); if( _temp92.curr == (( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ _temp90=({ struct Cyc_List_List*
_temp93=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp93->hd=( void*)({ struct _tagged_arr* _temp94=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp94[ 0]= s; _temp94;}); _temp93->tl=
_temp90; _temp93;}); break;} else{ _temp90=({ struct Cyc_List_List* _temp95=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp95->hd=(
void*)({ struct _tagged_arr* _temp96=( struct _tagged_arr*) _cycalloc( sizeof(
struct _tagged_arr)); _temp96[ 0]=( struct _tagged_arr) Cyc_Std_substring( s, 0,(
unsigned int)(((( struct _tagged_arr) _temp92).curr -  s.curr) /  sizeof(
unsigned char))); _temp96;}); _temp95->tl= _temp90; _temp95;}); _temp91 -=(((
struct _tagged_arr) _temp92).curr -  s.curr) /  sizeof( unsigned char); s=
_tagged_arr_plus( _temp92, sizeof( unsigned char), 1);}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp90);}} static int Cyc_file_exists(
struct _tagged_arr file){ struct Cyc_Std___sFILE* f= 0;{ struct _handler_cons
_temp97; _push_handler(& _temp97);{ int _temp99= 0; if( setjmp( _temp97.handler)){
_temp99= 1;} if( ! _temp99){ f=( struct Cyc_Std___sFILE*) Cyc_Std_file_open(
file, _tag_arr("r", sizeof( unsigned char), 2u));; _pop_handler();} else{ void*
_temp98=( void*) _exn_thrown; void* _temp101= _temp98; _LL103: goto _LL104;
_LL105: goto _LL106; _LL104: goto _LL102; _LL106:( void) _throw( _temp101);
_LL102:;}}} if( f ==  0){ return 0;} else{ Cyc_Std_fclose(( struct Cyc_Std___sFILE*)
_check_null( f)); return 1;}} static struct _tagged_arr* Cyc_find( struct Cyc_List_List*
dirs, struct _tagged_arr file){ if( file.curr == (( struct _tagged_arr) _tag_arr(
0u, 0u, 0u)).curr){ return 0;} for( 0; dirs !=  0; dirs=(( struct Cyc_List_List*)
_check_null( dirs))->tl){ struct _tagged_arr _temp107=*(( struct _tagged_arr*)((
struct Cyc_List_List*) _check_null( dirs))->hd); if( _temp107.curr == (( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? 1: Cyc_Std_strlen( _temp107) ==  0){
continue;}{ struct _tagged_arr s=( struct _tagged_arr) Cyc_Filename_concat(
_temp107, file); if( Cyc_file_exists( s)){ return({ struct _tagged_arr* _temp108=(
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp108[ 0]= s;
_temp108;});}}} return 0;} static struct _tagged_arr Cyc_do_find( struct Cyc_List_List*
dirs, struct _tagged_arr file){ struct _tagged_arr* _temp109= Cyc_find( dirs,
file); if( _temp109 ==  0){({ struct Cyc_Std_String_pa_struct _temp111; _temp111.tag=
Cyc_Std_String_pa; _temp111.f1=( struct _tagged_arr) file;{ void* _temp110[ 1u]={&
_temp111}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: can't find internal compiler file %s",
sizeof( unsigned char), 44u), _tag_arr( _temp110, sizeof( void*), 1u));}}); Cyc_compile_failure=
1; Cyc_remove_cfiles(); exit( 1);} return*(( struct _tagged_arr*) _check_null(
_temp109));} static int Cyc_is_other_special( unsigned char c){ switch( c){ case
'\\': _LL112: goto _LL113; case '"': _LL113: goto _LL114; case ';': _LL114: goto
_LL115; case '&': _LL115: goto _LL116; case '(': _LL116: goto _LL117; case ')':
_LL117: goto _LL118; case '|': _LL118: goto _LL119; case '^': _LL119: goto
_LL120; case '<': _LL120: goto _LL121; case '>': _LL121: goto _LL122; case ' ':
_LL122: goto _LL123; case '\n': _LL123: goto _LL124; case '\t': _LL124: return 1;
default: _LL125: return 0;}} static struct _tagged_arr Cyc_sh_escape_string(
struct _tagged_arr s){ unsigned int _temp127= Cyc_Std_strlen( s); int _temp128=
0; int _temp129= 0;{ int i= 0; for( 0; i <  _temp127; i ++){ unsigned char
_temp130=*(( const unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), i)); if( _temp130 == '\''){ _temp128 ++;} else{ if( Cyc_is_other_special(
_temp130)){ _temp129 ++;}}}} if( _temp128 ==  0? _temp129 ==  0: 0){ return s;}
if( _temp128 ==  0){ return( struct _tagged_arr) Cyc_Std_strconcat_l(({ struct
_tagged_arr* _temp131[ 3u]; _temp131[ 2u]= _init_tag_arr(( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)),"'", sizeof( unsigned char), 2u);
_temp131[ 1u]=({ struct _tagged_arr* _temp132=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp132[ 0]=( struct _tagged_arr) s; _temp132;});
_temp131[ 0u]= _init_tag_arr(( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)),"'", sizeof( unsigned char), 2u);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp131, sizeof( struct
_tagged_arr*), 3u));}));}{ unsigned int _temp133=( _temp127 +  _temp128) + 
_temp129; struct _tagged_arr _temp134=({ unsigned int _temp138= _temp133 +  1;
unsigned char* _temp139=( unsigned char*) _cycalloc_atomic( _check_times(
sizeof( unsigned char), _temp138)); struct _tagged_arr _temp141= _tag_arr(
_temp139, sizeof( unsigned char), _temp133 +  1);{ unsigned int _temp140=
_temp138; unsigned int i; for( i= 0; i <  _temp140; i ++){ _temp139[ i]='\000';}};
_temp141;}); int _temp135= 0; int _temp136= 0; for( 0; _temp135 <  _temp127;
_temp135 ++){ unsigned char _temp137=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), _temp135)); if( _temp137 == '\''?
1: Cyc_is_other_special( _temp137)){*(( unsigned char*) _check_unknown_subscript(
_temp134, sizeof( unsigned char), _temp136 ++))='\\';}*(( unsigned char*)
_check_unknown_subscript( _temp134, sizeof( unsigned char), _temp136 ++))=
_temp137;} return( struct _tagged_arr) _temp134;}} static struct _tagged_arr*
Cyc_sh_escape_stringptr( struct _tagged_arr* sp){ return({ struct _tagged_arr*
_temp142=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp142[ 0]= Cyc_sh_escape_string(* sp); _temp142;});} static void Cyc_process_file(
struct _tagged_arr filename){ struct _tagged_arr _temp143= Cyc_Filename_chop_extension(
filename); struct _tagged_arr _temp144= Cyc_Std_strconcat(( struct _tagged_arr)
_temp143, _tag_arr(".cyp", sizeof( unsigned char), 5u)); struct _tagged_arr
_temp145= Cyc_Std_strconcat(( struct _tagged_arr) _temp143, _tag_arr(".cyci",
sizeof( unsigned char), 6u)); struct _tagged_arr _temp146= Cyc_Std_strconcat((
struct _tagged_arr) _temp143, _tag_arr(".cycio", sizeof( unsigned char), 7u));
struct _tagged_arr _temp147= Cyc_Std_strconcat(( struct _tagged_arr) _temp143,
_tag_arr(".c", sizeof( unsigned char), 3u)); if( Cyc_v_r){({ struct Cyc_Std_String_pa_struct
_temp149; _temp149.tag= Cyc_Std_String_pa; _temp149.f1=( struct _tagged_arr)
filename;{ void* _temp148[ 1u]={& _temp149}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("Compiling %s\n", sizeof( unsigned char), 14u), _tag_arr( _temp148,
sizeof( void*), 1u));}});}{ struct Cyc_Std___sFILE* f0= Cyc_try_file_open(
filename, _tag_arr("r", sizeof( unsigned char), 2u), _tag_arr("input file",
sizeof( unsigned char), 11u)); if( Cyc_compile_failure){ return;} Cyc_Std_fclose((
struct Cyc_Std___sFILE*) _check_null( f0));{ struct _tagged_arr _temp150= Cyc_Std_str_sepstr(({
struct Cyc_List_List* _temp204=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp204->hd=( void*) _init_tag_arr(( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)),"", sizeof( unsigned char), 1u);
_temp204->tl=(( struct Cyc_List_List*(*)( struct _tagged_arr*(* f)( struct
_tagged_arr*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_sh_escape_stringptr,((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( Cyc_cppargs));
_temp204;}), _tag_arr(" ", sizeof( unsigned char), 2u)); struct _tagged_arr
stdinc_string; struct _tagged_arr def_inc_path=( struct _tagged_arr)
Cstring_to_string( Cdef_inc_path); if( Cyc_Std_strlen( def_inc_path) >  0){
stdinc_string=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp152;
_temp152.tag= Cyc_Std_String_pa; _temp152.f1=( struct _tagged_arr) def_inc_path;{
void* _temp151[ 1u]={& _temp152}; Cyc_Std_aprintf( _tag_arr(" -I%s", sizeof(
unsigned char), 6u), _tag_arr( _temp151, sizeof( void*), 1u));}});} else{
stdinc_string= _tag_arr("", sizeof( unsigned char), 1u);}{ struct _tagged_arr
_temp153= Cyc_Std_getenv( _tag_arr("CYCLONE_INCLUDE_PATH", sizeof( unsigned char),
21u)); if( _temp153.curr != ( _tag_arr( 0u, 0u, 0u)).curr){ struct Cyc_List_List*
_temp154= Cyc_split_by_char(( struct _tagged_arr) _temp153,':'); struct
_tagged_arr _temp155= Cyc_Std_str_sepstr(({ struct Cyc_List_List* _temp159=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp159->hd=(
void*) _init_tag_arr(( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)),"", sizeof( unsigned char), 1u); _temp159->tl=(( struct Cyc_List_List*(*)(
struct _tagged_arr*(* f)( struct _tagged_arr*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_sh_escape_stringptr, _temp154); _temp159;}), _tag_arr(" -I", sizeof(
unsigned char), 4u)); stdinc_string=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct
_temp158; _temp158.tag= Cyc_Std_String_pa; _temp158.f1=( struct _tagged_arr)
stdinc_string;{ struct Cyc_Std_String_pa_struct _temp157; _temp157.tag= Cyc_Std_String_pa;
_temp157.f1=( struct _tagged_arr) _temp155;{ void* _temp156[ 2u]={& _temp157,&
_temp158}; Cyc_Std_aprintf( _tag_arr("%s%s", sizeof( unsigned char), 5u),
_tag_arr( _temp156, sizeof( void*), 2u));}}});}{ struct _tagged_arr ofile_string;
if( Cyc_stop_after_cpp_r){ if( Cyc_output_file !=  0){ ofile_string=( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp161; _temp161.tag= Cyc_Std_String_pa;
_temp161.f1=( struct _tagged_arr)*(( struct _tagged_arr*) _check_null( Cyc_output_file));{
void* _temp160[ 1u]={& _temp161}; Cyc_Std_aprintf( _tag_arr(" > %s", sizeof(
unsigned char), 6u), _tag_arr( _temp160, sizeof( void*), 1u));}});} else{
ofile_string= _tag_arr("", sizeof( unsigned char), 1u);}} else{ ofile_string=(
struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp163; _temp163.tag=
Cyc_Std_String_pa; _temp163.f1=( struct _tagged_arr) Cyc_sh_escape_string((
struct _tagged_arr) _temp144);{ void* _temp162[ 1u]={& _temp163}; Cyc_Std_aprintf(
_tag_arr(" > %s", sizeof( unsigned char), 6u), _tag_arr( _temp162, sizeof( void*),
1u));}});}{ struct _tagged_arr fixup_string; if( Cyc_produce_dependencies){ if(
Cyc_dependencies_target ==  0){ fixup_string= _tag_arr(" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'",
sizeof( unsigned char), 35u);} else{ fixup_string=( struct _tagged_arr)({ struct
Cyc_Std_String_pa_struct _temp165; _temp165.tag= Cyc_Std_String_pa; _temp165.f1=(
struct _tagged_arr)*(( struct _tagged_arr*) _check_null( Cyc_dependencies_target));{
void* _temp164[ 1u]={& _temp165}; Cyc_Std_aprintf( _tag_arr(" | sed 's/^.*\\.cyc\\.o:/%s:/'",
sizeof( unsigned char), 29u), _tag_arr( _temp164, sizeof( void*), 1u));}});}}
else{ fixup_string= _tag_arr("", sizeof( unsigned char), 1u);}{ struct
_tagged_arr arch=( struct _tagged_arr) Cstring_to_string( Carch); struct
_tagged_arr _temp166=({ struct Cyc_Std_String_pa_struct _temp203; _temp203.tag=
Cyc_Std_String_pa; _temp203.f1=( struct _tagged_arr) ofile_string;{ struct Cyc_Std_String_pa_struct
_temp202; _temp202.tag= Cyc_Std_String_pa; _temp202.f1=( struct _tagged_arr)
fixup_string;{ struct Cyc_Std_String_pa_struct _temp201; _temp201.tag= Cyc_Std_String_pa;
_temp201.f1=( struct _tagged_arr) Cyc_sh_escape_string( filename);{ struct Cyc_Std_String_pa_struct
_temp200; _temp200.tag= Cyc_Std_String_pa; _temp200.f1=( struct _tagged_arr)
stdinc_string;{ struct Cyc_Std_String_pa_struct _temp199; _temp199.tag= Cyc_Std_String_pa;
_temp199.f1=( struct _tagged_arr) _temp150;{ struct Cyc_Std_String_pa_struct
_temp198; _temp198.tag= Cyc_Std_String_pa; _temp198.f1=( struct _tagged_arr)
arch;{ struct Cyc_Std_String_pa_struct _temp197; _temp197.tag= Cyc_Std_String_pa;
_temp197.f1=( struct _tagged_arr) Cyc_cpp;{ void* _temp196[ 7u]={& _temp197,&
_temp198,& _temp199,& _temp200,& _temp201,& _temp202,& _temp203}; Cyc_Std_aprintf(
_tag_arr("%s '-D_PLATFORM_INCLUDES_=\"arch/%s.h\"'%s%s %s%s%s", sizeof(
unsigned char), 50u), _tag_arr( _temp196, sizeof( void*), 7u));}}}}}}}}); if(
Cyc_v_r){({ struct Cyc_Std_String_pa_struct _temp168; _temp168.tag= Cyc_Std_String_pa;
_temp168.f1=( struct _tagged_arr) _temp166;{ void* _temp167[ 1u]={& _temp168};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s\n", sizeof( unsigned char), 4u),
_tag_arr( _temp167, sizeof( void*), 1u));}});} if( system( string_to_Cstring((
struct _tagged_arr) _temp166)) !=  0){ Cyc_compile_failure= 1;({ void* _temp169[
0u]={}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("\nError: preprocessing\n",
sizeof( unsigned char), 23u), _tag_arr( _temp169, sizeof( void*), 0u));});
return;} if( Cyc_stop_after_cpp_r){ return;} Cyc_Position_reset_position((
struct _tagged_arr) _temp144);{ struct Cyc_Std___sFILE* in_file= Cyc_try_file_open((
struct _tagged_arr) _temp144, _tag_arr("r", sizeof( unsigned char), 2u),
_tag_arr("file", sizeof( unsigned char), 5u)); if( Cyc_compile_failure){ return;}{
struct Cyc_List_List* tds= 0;{ struct _handler_cons _temp170; _push_handler(&
_temp170);{ int _temp172= 0; if( setjmp( _temp170.handler)){ _temp172= 1;} if( !
_temp172){ tds=(( struct Cyc_List_List*(*)( struct _tagged_arr stage_name,
struct Cyc_List_List*(* f)( struct Cyc_Std___sFILE*, struct Cyc_List_List*),
struct Cyc_Std___sFILE* env, struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("parsing",
sizeof( unsigned char), 8u), Cyc_do_parse,( struct Cyc_Std___sFILE*) _check_null(
in_file), tds);; _pop_handler();} else{ void* _temp171=( void*) _exn_thrown;
void* _temp174= _temp171; _LL176: goto _LL177; _LL178: goto _LL179; _LL177: Cyc_Std_file_close((
struct Cyc_Std___sFILE*) _check_null( in_file)); Cyc_remove_file(( struct
_tagged_arr) _temp144);( int) _throw( _temp174); _LL179:( void) _throw( _temp174);
_LL175:;}}} Cyc_Std_file_close(( struct Cyc_Std___sFILE*) _check_null( in_file));
if( Cyc_compile_failure){ Cyc_remove_file(( struct _tagged_arr) _temp144);
return;}{ struct Cyc_Tcenv_Tenv* _temp180= Cyc_Tcenv_tc_init(); if( Cyc_parseonly_r){
goto PRINTC;} tds=(( struct Cyc_List_List*(*)( struct _tagged_arr stage_name,
struct Cyc_List_List*(* f)( struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*),
struct Cyc_Tcenv_Tenv* env, struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("type checking",
sizeof( unsigned char), 14u), Cyc_do_typecheck, _temp180, tds); if( Cyc_compile_failure){
Cyc_remove_file(( struct _tagged_arr) _temp144); return;} tds=(( struct Cyc_List_List*(*)(
struct _tagged_arr stage_name, struct Cyc_List_List*(* f)( int, struct Cyc_List_List*),
int env, struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("control-flow checking",
sizeof( unsigned char), 22u), Cyc_do_cfcheck, 1, tds); if( ! Cyc_compile_failure){
Cyc_Tcutil_flush_warnings();} Cyc_remove_file(( struct _tagged_arr) _temp144);
if( Cyc_compile_failure){ return;} if( Cyc_ic_r){ struct Cyc_Std___sFILE*
inter_file= Cyc_Std_fopen(( struct _tagged_arr) _temp145, _tag_arr("r", sizeof(
unsigned char), 2u)); struct Cyc_Std___sFILE* inter_objfile= Cyc_try_file_open((
struct _tagged_arr) _temp146, _tag_arr("w", sizeof( unsigned char), 2u),
_tag_arr("interface object file", sizeof( unsigned char), 22u)); if(
inter_objfile ==  0){ Cyc_compile_failure= 1; return;} Cyc_Position_reset_position((
struct _tagged_arr) _temp145); tds=(( struct Cyc_List_List*(*)( struct
_tagged_arr stage_name, struct Cyc_List_List*(* f)( struct _tuple9*, struct Cyc_List_List*),
struct _tuple9* env, struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("interface checking",
sizeof( unsigned char), 19u), Cyc_do_interface,({ struct _tuple9* _temp181=(
struct _tuple9*) _cycalloc( sizeof( struct _tuple9)); _temp181->f1= _temp180;
_temp181->f2= inter_file; _temp181->f3=( struct Cyc_Std___sFILE*) _check_null(
inter_objfile); _temp181;}), tds); if( inter_file !=  0){ Cyc_Std_file_close((
struct Cyc_Std___sFILE*) _check_null( inter_file));} Cyc_Std_file_close(( struct
Cyc_Std___sFILE*) _check_null( inter_objfile));} if( Cyc_tc_r){ goto PRINTC;}
tds=(( struct Cyc_List_List*(*)( struct _tagged_arr stage_name, struct Cyc_List_List*(*
f)( int, struct Cyc_List_List*), int env, struct Cyc_List_List* tds)) Cyc_do_stage)(
_tag_arr("translation to C", sizeof( unsigned char), 17u), Cyc_do_translate, 1,
tds); if( Cyc_compile_failure){ return;} if( Cyc_tovc_r){ tds=(( struct Cyc_List_List*(*)(
struct _tagged_arr stage_name, struct Cyc_List_List*(* f)( int, struct Cyc_List_List*),
int env, struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("post-pass to VC",
sizeof( unsigned char), 16u), Cyc_do_tovc, 1, tds);} if( Cyc_compile_failure){
return;} PRINTC: if( tds !=  0){ struct Cyc_Std___sFILE* out_file; if( Cyc_parseonly_r?
1: Cyc_tc_r){ if( Cyc_output_file !=  0){ out_file= Cyc_try_file_open(*(( struct
_tagged_arr*) _check_null( Cyc_output_file)), _tag_arr("w", sizeof(
unsigned char), 2u), _tag_arr("output file", sizeof( unsigned char), 12u));}
else{ out_file=( struct Cyc_Std___sFILE*) Cyc_Std_stdout;}} else{ if( Cyc_toc_r?
Cyc_output_file !=  0: 0){ out_file= Cyc_try_file_open(*(( struct _tagged_arr*)
_check_null( Cyc_output_file)), _tag_arr("w", sizeof( unsigned char), 2u),
_tag_arr("output file", sizeof( unsigned char), 12u));} else{ out_file= Cyc_try_file_open((
struct _tagged_arr) _temp147, _tag_arr("w", sizeof( unsigned char), 2u),
_tag_arr("output file", sizeof( unsigned char), 12u));}} if( Cyc_compile_failure){
return;}{ struct _handler_cons _temp182; _push_handler(& _temp182);{ int
_temp184= 0; if( setjmp( _temp182.handler)){ _temp184= 1;} if( ! _temp184){ tds=((
struct Cyc_List_List*(*)( struct _tagged_arr stage_name, struct Cyc_List_List*(*
f)( struct Cyc_Std___sFILE*, struct Cyc_List_List*), struct Cyc_Std___sFILE* env,
struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("printing", sizeof(
unsigned char), 9u), Cyc_do_print,( struct Cyc_Std___sFILE*) _check_null(
out_file), tds);; _pop_handler();} else{ void* _temp183=( void*) _exn_thrown;
void* _temp186= _temp183; _LL188: goto _LL189; _LL190: goto _LL191; _LL189: Cyc_compile_failure=
1; Cyc_Std_file_close(( struct Cyc_Std___sFILE*) _check_null( out_file)); Cyc_cfiles=({
struct Cyc_List_List* _temp192=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp192->hd=( void*)({ struct _tagged_arr* _temp193=(
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp193[ 0]=(
struct _tagged_arr) _temp147; _temp193;}); _temp192->tl= Cyc_cfiles; _temp192;});(
int) _throw( _temp186); _LL191:( void) _throw( _temp186); _LL187:;}}} Cyc_Std_file_close((
struct Cyc_Std___sFILE*) _check_null( out_file)); Cyc_cfiles=({ struct Cyc_List_List*
_temp194=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp194->hd=( void*)({ struct _tagged_arr* _temp195=( struct _tagged_arr*)
_cycalloc( sizeof( struct _tagged_arr)); _temp195[ 0]=( struct _tagged_arr)
_temp147; _temp195;}); _temp194->tl= Cyc_cfiles; _temp194;});}}}}}}}}}}} static
unsigned char _temp205[ 8u]="<final>"; static struct _tagged_arr Cyc_final_str={
_temp205, _temp205, _temp205 +  8u}; static struct _tagged_arr* Cyc_final_strptr=&
Cyc_final_str; static struct Cyc_Interface_I* Cyc_read_cycio( struct _tagged_arr*
n){ if( n == ( struct _tagged_arr*) Cyc_final_strptr){ return Cyc_Interface_final();}{
struct _tagged_arr basename;{ struct _handler_cons _temp206; _push_handler(&
_temp206);{ int _temp208= 0; if( setjmp( _temp206.handler)){ _temp208= 1;} if( !
_temp208){ basename=( struct _tagged_arr) Cyc_Filename_chop_extension(* n);;
_pop_handler();} else{ void* _temp207=( void*) _exn_thrown; void* _temp210=
_temp207; _LL212: if(*(( void**) _temp210) ==  Cyc_Core_Invalid_argument){ goto
_LL213;} else{ goto _LL214;} _LL214: goto _LL215; _LL213: basename=* n; goto
_LL211; _LL215:( void) _throw( _temp210); _LL211:;}}}{ struct _tagged_arr
_temp216= Cyc_Std_strconcat( basename, _tag_arr(".cycio", sizeof( unsigned char),
7u)); struct Cyc_Std___sFILE* _temp217= Cyc_try_file_open(( struct _tagged_arr)
_temp216, _tag_arr("r", sizeof( unsigned char), 2u), _tag_arr("interface object file",
sizeof( unsigned char), 22u)); if( _temp217 ==  0){ Cyc_compile_failure= 1; Cyc_remove_cfiles();
exit( 1);} Cyc_Position_reset_position(( struct _tagged_arr) _temp216);{ struct
Cyc_Interface_I* _temp218= Cyc_Interface_load(( struct Cyc_Std___sFILE*)
_check_null( _temp217)); Cyc_Std_file_close(( struct Cyc_Std___sFILE*)
_check_null( _temp217)); return _temp218;}}}} static int Cyc_is_cfile( struct
_tagged_arr* n){ return*(( const unsigned char*) _check_unknown_subscript(* n,
sizeof( unsigned char), 0)) != '-';} struct _tuple10{ struct _tagged_arr f1; int
f2; struct _tagged_arr f3; void* f4; struct _tagged_arr f5; } ; int Cyc_main(
int argc, struct _tagged_arr argv){ struct Cyc_List_List* options=({ struct
_tuple10* _temp293[ 40u]; _temp293[ 39u]=({ struct _tuple10* _temp411=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp411->f1= _tag_arr("-printfullevars",
sizeof( unsigned char), 16u); _temp411->f2= 0; _temp411->f3= _tag_arr("",
sizeof( unsigned char), 1u); _temp411->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp412=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp412[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp413; _temp413.tag= Cyc_Arg_Set_spec;
_temp413.f1=& Cyc_print_full_evars_r; _temp413;}); _temp412;}); _temp411->f5=
_tag_arr("Print full information for evars (type debugging)", sizeof(
unsigned char), 50u); _temp411;}); _temp293[ 38u]=({ struct _tuple10* _temp408=(
struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp408->f1= _tag_arr("-printallkinds",
sizeof( unsigned char), 15u); _temp408->f2= 0; _temp408->f3= _tag_arr("",
sizeof( unsigned char), 1u); _temp408->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp409=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp409[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp410; _temp410.tag= Cyc_Arg_Set_spec;
_temp410.f1=& Cyc_print_all_kinds_r; _temp410;}); _temp409;}); _temp408->f5=
_tag_arr("Always print kinds of type variables", sizeof( unsigned char), 37u);
_temp408;}); _temp293[ 37u]=({ struct _tuple10* _temp405=( struct _tuple10*)
_cycalloc( sizeof( struct _tuple10)); _temp405->f1= _tag_arr("-printalltvars",
sizeof( unsigned char), 15u); _temp405->f2= 0; _temp405->f3= _tag_arr("",
sizeof( unsigned char), 1u); _temp405->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp406=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp406[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp407; _temp407.tag= Cyc_Arg_Set_spec;
_temp407.f1=& Cyc_print_all_tvars_r; _temp407;}); _temp406;}); _temp405->f5=
_tag_arr("Print all type variables (even implicit default effects)", sizeof(
unsigned char), 57u); _temp405;}); _temp293[ 36u]=({ struct _tuple10* _temp402=(
struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp402->f1= _tag_arr("-noexpandtypedefs",
sizeof( unsigned char), 18u); _temp402->f2= 0; _temp402->f3= _tag_arr("",
sizeof( unsigned char), 1u); _temp402->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp403=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp403[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp404; _temp404.tag= Cyc_Arg_Set_spec;
_temp404.f1=& Cyc_noexpand_r; _temp404;}); _temp403;}); _temp402->f5= _tag_arr("Don't expand typedefs in pretty printing",
sizeof( unsigned char), 41u); _temp402;}); _temp293[ 35u]=({ struct _tuple10*
_temp399=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp399->f1=
_tag_arr("-noremoveunused", sizeof( unsigned char), 16u); _temp399->f2= 0;
_temp399->f3= _tag_arr("", sizeof( unsigned char), 1u); _temp399->f4=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp400=( struct Cyc_Arg_Set_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp400[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp401; _temp401.tag= Cyc_Arg_Set_spec; _temp401.f1=& Cyc_noshake_r; _temp401;});
_temp400;}); _temp399->f5= _tag_arr("Don't remove externed variables that aren't used",
sizeof( unsigned char), 49u); _temp399;}); _temp293[ 34u]=({ struct _tuple10*
_temp396=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp396->f1=
_tag_arr("-nogc", sizeof( unsigned char), 6u); _temp396->f2= 0; _temp396->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp396->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp397=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp397[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp398; _temp398.tag= Cyc_Arg_Set_spec;
_temp398.f1=& Cyc_nogc_r; _temp398;}); _temp397;}); _temp396->f5= _tag_arr("Don't link in the garbage collector",
sizeof( unsigned char), 36u); _temp396;}); _temp293[ 33u]=({ struct _tuple10*
_temp393=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp393->f1=
_tag_arr("-nocyc", sizeof( unsigned char), 7u); _temp393->f2= 0; _temp393->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp393->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp394=( struct Cyc_Arg_Unit_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp394[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp395; _temp395.tag= Cyc_Arg_Unit_spec;
_temp395.f1= Cyc_set_nocyc; _temp395;}); _temp394;}); _temp393->f5= _tag_arr("Don't add implicit namespace Cyc",
sizeof( unsigned char), 33u); _temp393;}); _temp293[ 32u]=({ struct _tuple10*
_temp390=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp390->f1=
_tag_arr("-use-cpp", sizeof( unsigned char), 9u); _temp390->f2= 0; _temp390->f3=
_tag_arr("<path>", sizeof( unsigned char), 7u); _temp390->f4=( void*)({ struct
Cyc_Arg_String_spec_struct* _temp391=( struct Cyc_Arg_String_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_String_spec_struct)); _temp391[ 0]=({ struct
Cyc_Arg_String_spec_struct _temp392; _temp392.tag= Cyc_Arg_String_spec; _temp392.f1=
Cyc_set_cpp; _temp392;}); _temp391;}); _temp390->f5= _tag_arr("Indicate which preprocessor to use",
sizeof( unsigned char), 35u); _temp390;}); _temp293[ 31u]=({ struct _tuple10*
_temp387=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp387->f1=
_tag_arr("-save-c", sizeof( unsigned char), 8u); _temp387->f2= 0; _temp387->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp387->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp388=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp388[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp389; _temp389.tag= Cyc_Arg_Set_spec;
_temp389.f1=& Cyc_save_c_r; _temp389;}); _temp388;}); _temp387->f5= _tag_arr("Don't delete temporary C files",
sizeof( unsigned char), 31u); _temp387;}); _temp293[ 30u]=({ struct _tuple10*
_temp384=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp384->f1=
_tag_arr("-save-temps", sizeof( unsigned char), 12u); _temp384->f2= 0; _temp384->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp384->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp385=( struct Cyc_Arg_Unit_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp385[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp386; _temp386.tag= Cyc_Arg_Unit_spec;
_temp386.f1= Cyc_set_save_temps; _temp386;}); _temp385;}); _temp384->f5=
_tag_arr("Don't delete temporary files", sizeof( unsigned char), 29u); _temp384;});
_temp293[ 29u]=({ struct _tuple10* _temp381=( struct _tuple10*) _cycalloc(
sizeof( struct _tuple10)); _temp381->f1= _tag_arr("-tovc", sizeof( unsigned char),
6u); _temp381->f2= 0; _temp381->f3= _tag_arr("", sizeof( unsigned char), 1u);
_temp381->f4=( void*)({ struct Cyc_Arg_Set_spec_struct* _temp382=( struct Cyc_Arg_Set_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp382[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp383; _temp383.tag= Cyc_Arg_Set_spec; _temp383.f1=& Cyc_tovc_r; _temp383;});
_temp382;}); _temp381->f5= _tag_arr("Avoid gcc extensions in C output", sizeof(
unsigned char), 33u); _temp381;}); _temp293[ 28u]=({ struct _tuple10* _temp378=(
struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp378->f1= _tag_arr("-up",
sizeof( unsigned char), 4u); _temp378->f2= 0; _temp378->f3= _tag_arr("", sizeof(
unsigned char), 1u); _temp378->f4=( void*)({ struct Cyc_Arg_Clear_spec_struct*
_temp379=( struct Cyc_Arg_Clear_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Clear_spec_struct));
_temp379[ 0]=({ struct Cyc_Arg_Clear_spec_struct _temp380; _temp380.tag= Cyc_Arg_Clear_spec;
_temp380.f1=& Cyc_pp_r; _temp380;}); _temp379;}); _temp378->f5= _tag_arr("Ugly print",
sizeof( unsigned char), 11u); _temp378;}); _temp293[ 27u]=({ struct _tuple10*
_temp375=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp375->f1=
_tag_arr("-pp", sizeof( unsigned char), 4u); _temp375->f2= 0; _temp375->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp375->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp376=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp376[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp377; _temp377.tag= Cyc_Arg_Set_spec;
_temp377.f1=& Cyc_pp_r; _temp377;}); _temp376;}); _temp375->f5= _tag_arr("Pretty print",
sizeof( unsigned char), 13u); _temp375;}); _temp293[ 26u]=({ struct _tuple10*
_temp372=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp372->f1=
_tag_arr("-ic", sizeof( unsigned char), 4u); _temp372->f2= 0; _temp372->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp372->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp373=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp373[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp374; _temp374.tag= Cyc_Arg_Set_spec;
_temp374.f1=& Cyc_ic_r; _temp374;}); _temp373;}); _temp372->f5= _tag_arr("Activate the link-checker",
sizeof( unsigned char), 26u); _temp372;}); _temp293[ 25u]=({ struct _tuple10*
_temp369=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp369->f1=
_tag_arr("-stopafter-toc", sizeof( unsigned char), 15u); _temp369->f2= 0;
_temp369->f3= _tag_arr("", sizeof( unsigned char), 1u); _temp369->f4=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp370=( struct Cyc_Arg_Set_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp370[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp371; _temp371.tag= Cyc_Arg_Set_spec; _temp371.f1=& Cyc_toc_r; _temp371;});
_temp370;}); _temp369->f5= _tag_arr("Stop after translation to C", sizeof(
unsigned char), 28u); _temp369;}); _temp293[ 24u]=({ struct _tuple10* _temp366=(
struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp366->f1= _tag_arr("-stopafter-tc",
sizeof( unsigned char), 14u); _temp366->f2= 0; _temp366->f3= _tag_arr("",
sizeof( unsigned char), 1u); _temp366->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp367=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp367[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp368; _temp368.tag= Cyc_Arg_Set_spec;
_temp368.f1=& Cyc_tc_r; _temp368;}); _temp367;}); _temp366->f5= _tag_arr("Stop after type checking",
sizeof( unsigned char), 25u); _temp366;}); _temp293[ 23u]=({ struct _tuple10*
_temp363=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp363->f1=
_tag_arr("-stopafter-parse", sizeof( unsigned char), 17u); _temp363->f2= 0;
_temp363->f3= _tag_arr("", sizeof( unsigned char), 1u); _temp363->f4=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp364=( struct Cyc_Arg_Set_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp364[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp365; _temp365.tag= Cyc_Arg_Set_spec; _temp365.f1=& Cyc_parseonly_r;
_temp365;}); _temp364;}); _temp363->f5= _tag_arr("Stop after parsing", sizeof(
unsigned char), 19u); _temp363;}); _temp293[ 22u]=({ struct _tuple10* _temp360=(
struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp360->f1= _tag_arr("-E",
sizeof( unsigned char), 3u); _temp360->f2= 0; _temp360->f3= _tag_arr("", sizeof(
unsigned char), 1u); _temp360->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp361=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp361[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp362; _temp362.tag= Cyc_Arg_Set_spec;
_temp362.f1=& Cyc_stop_after_cpp_r; _temp362;}); _temp361;}); _temp360->f5=
_tag_arr("Stop after preprocessing", sizeof( unsigned char), 25u); _temp360;});
_temp293[ 21u]=({ struct _tuple10* _temp357=( struct _tuple10*) _cycalloc(
sizeof( struct _tuple10)); _temp357->f1= _tag_arr("-MT", sizeof( unsigned char),
4u); _temp357->f2= 0; _temp357->f3= _tag_arr(" <target>", sizeof( unsigned char),
10u); _temp357->f4=( void*)({ struct Cyc_Arg_String_spec_struct* _temp358=(
struct Cyc_Arg_String_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_String_spec_struct));
_temp358[ 0]=({ struct Cyc_Arg_String_spec_struct _temp359; _temp359.tag= Cyc_Arg_String_spec;
_temp359.f1= Cyc_set_dependencies_target; _temp359;}); _temp358;}); _temp357->f5=
_tag_arr("Give target for dependencies", sizeof( unsigned char), 29u); _temp357;});
_temp293[ 20u]=({ struct _tuple10* _temp354=( struct _tuple10*) _cycalloc(
sizeof( struct _tuple10)); _temp354->f1= _tag_arr("-MG", sizeof( unsigned char),
4u); _temp354->f2= 0; _temp354->f3= _tag_arr("", sizeof( unsigned char), 1u);
_temp354->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct* _temp355=( struct Cyc_Arg_Flag_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp355[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp356; _temp356.tag= Cyc_Arg_Flag_spec; _temp356.f1= Cyc_add_cpparg; _temp356;});
_temp355;}); _temp354->f5= _tag_arr("When producing dependencies assume that missing files are generated",
sizeof( unsigned char), 68u); _temp354;}); _temp293[ 19u]=({ struct _tuple10*
_temp351=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp351->f1=
_tag_arr("-M", sizeof( unsigned char), 3u); _temp351->f2= 0; _temp351->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp351->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp352=( struct Cyc_Arg_Unit_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp352[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp353; _temp353.tag= Cyc_Arg_Unit_spec;
_temp353.f1= Cyc_set_produce_dependencies; _temp353;}); _temp352;}); _temp351->f5=
_tag_arr("Produce dependencies", sizeof( unsigned char), 21u); _temp351;});
_temp293[ 18u]=({ struct _tuple10* _temp348=( struct _tuple10*) _cycalloc(
sizeof( struct _tuple10)); _temp348->f1= _tag_arr("-S", sizeof( unsigned char),
3u); _temp348->f2= 0; _temp348->f3= _tag_arr("", sizeof( unsigned char), 1u);
_temp348->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct* _temp349=( struct Cyc_Arg_Unit_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp349[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp350; _temp350.tag= Cyc_Arg_Unit_spec; _temp350.f1= Cyc_set_stop_after_asmfile;
_temp350;}); _temp349;}); _temp348->f5= _tag_arr("Stop after producing assembly code",
sizeof( unsigned char), 35u); _temp348;}); _temp293[ 17u]=({ struct _tuple10*
_temp345=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp345->f1=
_tag_arr("-pa", sizeof( unsigned char), 4u); _temp345->f2= 0; _temp345->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp345->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp346=( struct Cyc_Arg_Unit_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp346[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp347; _temp347.tag= Cyc_Arg_Unit_spec;
_temp347.f1= Cyc_set_pa; _temp347;}); _temp346;}); _temp345->f5= _tag_arr("Compile for profiling with aprof",
sizeof( unsigned char), 33u); _temp345;}); _temp293[ 16u]=({ struct _tuple10*
_temp342=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp342->f1=
_tag_arr("-pg", sizeof( unsigned char), 4u); _temp342->f2= 0; _temp342->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp342->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp343=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp343[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp344; _temp344.tag= Cyc_Arg_Flag_spec;
_temp344.f1= Cyc_add_ccarg; _temp344;}); _temp343;}); _temp342->f5= _tag_arr("Compile for profiling with gprof",
sizeof( unsigned char), 33u); _temp342;}); _temp293[ 15u]=({ struct _tuple10*
_temp339=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp339->f1=
_tag_arr("-p", sizeof( unsigned char), 3u); _temp339->f2= 0; _temp339->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp339->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp340=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp340[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp341; _temp341.tag= Cyc_Arg_Flag_spec;
_temp341.f1= Cyc_add_ccarg; _temp341;}); _temp340;}); _temp339->f5= _tag_arr("Compile for profiling with prof",
sizeof( unsigned char), 32u); _temp339;}); _temp293[ 14u]=({ struct _tuple10*
_temp336=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp336->f1=
_tag_arr("-g", sizeof( unsigned char), 3u); _temp336->f2= 0; _temp336->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp336->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp337=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp337[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp338; _temp338.tag= Cyc_Arg_Flag_spec;
_temp338.f1= Cyc_add_ccarg; _temp338;}); _temp337;}); _temp336->f5= _tag_arr("Compile for debugging",
sizeof( unsigned char), 22u); _temp336;}); _temp293[ 13u]=({ struct _tuple10*
_temp333=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp333->f1=
_tag_arr("-O3", sizeof( unsigned char), 4u); _temp333->f2= 0; _temp333->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp333->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp334=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp334[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp335; _temp335.tag= Cyc_Arg_Flag_spec;
_temp335.f1= Cyc_add_ccarg; _temp335;}); _temp334;}); _temp333->f5= _tag_arr("Even more optimization",
sizeof( unsigned char), 23u); _temp333;}); _temp293[ 12u]=({ struct _tuple10*
_temp330=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp330->f1=
_tag_arr("-O2", sizeof( unsigned char), 4u); _temp330->f2= 0; _temp330->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp330->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp331=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp331[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp332; _temp332.tag= Cyc_Arg_Flag_spec;
_temp332.f1= Cyc_add_ccarg; _temp332;}); _temp331;}); _temp330->f5= _tag_arr("A higher level of optimization",
sizeof( unsigned char), 31u); _temp330;}); _temp293[ 11u]=({ struct _tuple10*
_temp327=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp327->f1=
_tag_arr("-O", sizeof( unsigned char), 3u); _temp327->f2= 0; _temp327->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp327->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp328=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp328[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp329; _temp329.tag= Cyc_Arg_Flag_spec;
_temp329.f1= Cyc_add_ccarg; _temp329;}); _temp328;}); _temp327->f5= _tag_arr("Optimize",
sizeof( unsigned char), 9u); _temp327;}); _temp293[ 10u]=({ struct _tuple10*
_temp324=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp324->f1=
_tag_arr("-s", sizeof( unsigned char), 3u); _temp324->f2= 0; _temp324->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp324->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp325=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp325[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp326; _temp326.tag= Cyc_Arg_Flag_spec;
_temp326.f1= Cyc_add_ccarg; _temp326;}); _temp325;}); _temp324->f5= _tag_arr("Remove all symbol table and relocation info from executable",
sizeof( unsigned char), 60u); _temp324;}); _temp293[ 9u]=({ struct _tuple10*
_temp321=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp321->f1=
_tag_arr("-x", sizeof( unsigned char), 3u); _temp321->f2= 0; _temp321->f3=
_tag_arr(" <language>", sizeof( unsigned char), 12u); _temp321->f4=( void*)({
struct Cyc_Arg_String_spec_struct* _temp322=( struct Cyc_Arg_String_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_String_spec_struct)); _temp322[ 0]=({ struct
Cyc_Arg_String_spec_struct _temp323; _temp323.tag= Cyc_Arg_String_spec; _temp323.f1=
Cyc_set_inputtype; _temp323;}); _temp322;}); _temp321->f5= _tag_arr("Specify <language> for the following input files",
sizeof( unsigned char), 49u); _temp321;}); _temp293[ 8u]=({ struct _tuple10*
_temp318=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp318->f1=
_tag_arr("-c", sizeof( unsigned char), 3u); _temp318->f2= 0; _temp318->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp318->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp319=( struct Cyc_Arg_Unit_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp319[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp320; _temp320.tag= Cyc_Arg_Unit_spec;
_temp320.f1= Cyc_set_stop_after_objectfile; _temp320;}); _temp319;}); _temp318->f5=
_tag_arr("Produce an object file instead of an executable; do not link", sizeof(
unsigned char), 61u); _temp318;}); _temp293[ 7u]=({ struct _tuple10* _temp315=(
struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp315->f1= _tag_arr("-l",
sizeof( unsigned char), 3u); _temp315->f2= 1; _temp315->f3= _tag_arr("<file>",
sizeof( unsigned char), 7u); _temp315->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp316=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp316[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp317; _temp317.tag= Cyc_Arg_Flag_spec;
_temp317.f1= Cyc_add_libarg; _temp317;}); _temp316;}); _temp315->f5= _tag_arr("Library file",
sizeof( unsigned char), 13u); _temp315;}); _temp293[ 6u]=({ struct _tuple10*
_temp312=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp312->f1=
_tag_arr("-L", sizeof( unsigned char), 3u); _temp312->f2= 1; _temp312->f3=
_tag_arr("<dir>", sizeof( unsigned char), 6u); _temp312->f4=( void*)({ struct
Cyc_Arg_Flag_spec_struct* _temp313=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc(
sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp313[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp314; _temp314.tag= Cyc_Arg_Flag_spec; _temp314.f1= Cyc_add_cpparg; _temp314;});
_temp313;}); _temp312->f5= _tag_arr("Add to the list of directories for -l",
sizeof( unsigned char), 38u); _temp312;}); _temp293[ 5u]=({ struct _tuple10*
_temp309=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp309->f1=
_tag_arr("-I", sizeof( unsigned char), 3u); _temp309->f2= 1; _temp309->f3=
_tag_arr("<dir>", sizeof( unsigned char), 6u); _temp309->f4=( void*)({ struct
Cyc_Arg_Flag_spec_struct* _temp310=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc(
sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp310[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp311; _temp311.tag= Cyc_Arg_Flag_spec; _temp311.f1= Cyc_add_cpparg; _temp311;});
_temp310;}); _temp309->f5= _tag_arr("Add to the list of directories to search for include files",
sizeof( unsigned char), 59u); _temp309;}); _temp293[ 4u]=({ struct _tuple10*
_temp306=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp306->f1=
_tag_arr("-B", sizeof( unsigned char), 3u); _temp306->f2= 1; _temp306->f3=
_tag_arr("<file>", sizeof( unsigned char), 7u); _temp306->f4=( void*)({ struct
Cyc_Arg_Flag_spec_struct* _temp307=( struct Cyc_Arg_Flag_spec_struct*) _cycalloc(
sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp307[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp308; _temp308.tag= Cyc_Arg_Flag_spec; _temp308.f1= Cyc_add_cyclone_lib_path;
_temp308;}); _temp307;}); _temp306->f5= _tag_arr("Add to the list of directories to search for compiler files",
sizeof( unsigned char), 60u); _temp306;}); _temp293[ 3u]=({ struct _tuple10*
_temp303=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp303->f1=
_tag_arr("-D", sizeof( unsigned char), 3u); _temp303->f2= 1; _temp303->f3=
_tag_arr("<name>[=<value>]", sizeof( unsigned char), 17u); _temp303->f4=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp304=( struct Cyc_Arg_Flag_spec_struct*)
_cycalloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp304[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp305; _temp305.tag= Cyc_Arg_Flag_spec; _temp305.f1= Cyc_add_cpparg; _temp305;});
_temp304;}); _temp303->f5= _tag_arr("Pass definition to preprocessor", sizeof(
unsigned char), 32u); _temp303;}); _temp293[ 2u]=({ struct _tuple10* _temp300=(
struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp300->f1= _tag_arr("-o",
sizeof( unsigned char), 3u); _temp300->f2= 0; _temp300->f3= _tag_arr(" <file>",
sizeof( unsigned char), 8u); _temp300->f4=( void*)({ struct Cyc_Arg_String_spec_struct*
_temp301=( struct Cyc_Arg_String_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_String_spec_struct));
_temp301[ 0]=({ struct Cyc_Arg_String_spec_struct _temp302; _temp302.tag= Cyc_Arg_String_spec;
_temp302.f1= Cyc_set_output_file; _temp302;}); _temp301;}); _temp300->f5=
_tag_arr("Set the output file name to <file>", sizeof( unsigned char), 35u);
_temp300;}); _temp293[ 1u]=({ struct _tuple10* _temp297=( struct _tuple10*)
_cycalloc( sizeof( struct _tuple10)); _temp297->f1= _tag_arr("--version",
sizeof( unsigned char), 10u); _temp297->f2= 0; _temp297->f3= _tag_arr("",
sizeof( unsigned char), 1u); _temp297->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp298=( struct Cyc_Arg_Unit_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp298[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp299; _temp299.tag= Cyc_Arg_Unit_spec;
_temp299.f1= Cyc_print_version; _temp299;}); _temp298;}); _temp297->f5= _tag_arr("Print version number and exit",
sizeof( unsigned char), 30u); _temp297;}); _temp293[ 0u]=({ struct _tuple10*
_temp294=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp294->f1=
_tag_arr("-v", sizeof( unsigned char), 3u); _temp294->f2= 0; _temp294->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp294->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp295=( struct Cyc_Arg_Set_spec_struct*) _cycalloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp295[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp296; _temp296.tag= Cyc_Arg_Set_spec;
_temp296.f1=& Cyc_v_r; _temp296;}); _temp295;}); _temp294->f5= _tag_arr("Print compilation stages verbosely",
sizeof( unsigned char), 35u); _temp294;});(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp293, sizeof( struct _tuple10*), 40u));});
Cyc_Arg_parse( options, Cyc_add_other, _tag_arr("Options:", sizeof(
unsigned char), 9u), argv);{ struct Cyc_List_List* _temp219=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( Cyc_cyclone_files); for( 0; _temp219 != 
0; _temp219=(( struct Cyc_List_List*) _check_null( _temp219))->tl){ Cyc_process_file(*((
struct _tagged_arr*)(( struct Cyc_List_List*) _check_null( _temp219))->hd)); if(
Cyc_compile_failure){ return 1;}}} if((( Cyc_stop_after_cpp_r? 1: Cyc_parseonly_r)?
1: Cyc_tc_r)? 1: Cyc_toc_r){ return 0;} if( Cyc_ccargs ==  0){ return 0;}{
struct _tagged_arr cyclone_exec_prefix=( struct _tagged_arr) Cyc_Std_getenv(
_tag_arr("CYCLONE_EXEC_PREFIX", sizeof( unsigned char), 20u)); if(
cyclone_exec_prefix.curr != (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){
Cyc_cyclone_lib_path=({ struct Cyc_List_List* _temp220=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp220->hd=( void*)({ struct
_tagged_arr* _temp221=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp221[ 0]= cyclone_exec_prefix; _temp221;}); _temp220->tl= Cyc_cyclone_lib_path;
_temp220;});}{ struct _tagged_arr def_lib_path=( struct _tagged_arr)
Cstring_to_string( Cdef_lib_path); if( Cyc_Std_strlen( def_lib_path) >  0){ Cyc_cyclone_lib_path=({
struct Cyc_List_List* _temp222=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp222->hd=( void*)({ struct _tagged_arr* _temp223=(
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp223[ 0]=
def_lib_path; _temp223;}); _temp222->tl= Cyc_cyclone_lib_path; _temp222;});} Cyc_cyclone_lib_path=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_cyclone_lib_path);{
struct _tagged_arr _temp224= Cyc_do_find( Cyc_cyclone_lib_path, _tag_arr("include/cyc_include.h",
sizeof( unsigned char), 22u)); Cyc_ccargs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( Cyc_ccargs);{ struct _tagged_arr _temp225= Cyc_Std_str_sepstr(((
struct Cyc_List_List*(*)( struct _tagged_arr*(* f)( struct _tagged_arr*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_sh_escape_stringptr, Cyc_ccargs), _tag_arr(" ",
sizeof( unsigned char), 2u)); Cyc_libargs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( Cyc_libargs);{ struct _tagged_arr _temp226=
Cyc_Std_str_sepstr(({ struct Cyc_List_List* _temp292=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp292->hd=( void*) _init_tag_arr((
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)),"", sizeof(
unsigned char), 1u); _temp292->tl=(( struct Cyc_List_List*(*)( struct
_tagged_arr*(* f)( struct _tagged_arr*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_sh_escape_stringptr, Cyc_libargs); _temp292;}), _tag_arr(" ", sizeof(
unsigned char), 2u)); struct Cyc_List_List* stdlib; struct _tagged_arr
stdlib_string; int _temp227=(( Cyc_stop_after_asmfile_r? 1: Cyc_stop_after_objectfile_r)?
1:( Cyc_output_file !=  0? Cyc_Filename_check_suffix(*(( struct _tagged_arr*)
_check_null( Cyc_output_file)), _tag_arr(".a", sizeof( unsigned char), 3u)): 0))?
1:( Cyc_output_file !=  0? Cyc_Filename_check_suffix(*(( struct _tagged_arr*)
_check_null( Cyc_output_file)), _tag_arr(".lib", sizeof( unsigned char), 5u)): 0);
if( _temp227){ stdlib= 0; stdlib_string= _tag_arr("", sizeof( unsigned char), 1u);}
else{ struct _tagged_arr libcyc_filename= Cyc_pa_r? _tag_arr("libcyc_a.a",
sizeof( unsigned char), 11u): _tag_arr("libcyc.a", sizeof( unsigned char), 9u);
struct _tagged_arr nogc_filename= Cyc_pa_r? _tag_arr("nogc_a.a", sizeof(
unsigned char), 9u): _tag_arr("nogc.a", sizeof( unsigned char), 7u); struct
_tagged_arr gc_filename= _tag_arr("gc.a", sizeof( unsigned char), 5u); struct
_tagged_arr _temp228= Cyc_do_find( Cyc_cyclone_lib_path, libcyc_filename);
struct _tagged_arr _temp229= Cyc_nogc_r? Cyc_do_find( Cyc_cyclone_lib_path,
nogc_filename): Cyc_do_find( Cyc_cyclone_lib_path, gc_filename); stdlib=({
struct _tagged_arr* _temp230[ 1u]; _temp230[ 0u]=({ struct _tagged_arr* _temp231=(
struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr)); _temp231[ 0]=
_temp228; _temp231;});(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp230, sizeof( struct _tagged_arr*), 1u));}); stdlib_string=(
struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp234; _temp234.tag=
Cyc_Std_String_pa; _temp234.f1=( struct _tagged_arr) _temp229;{ struct Cyc_Std_String_pa_struct
_temp233; _temp233.tag= Cyc_Std_String_pa; _temp233.f1=( struct _tagged_arr)
_temp228;{ void* _temp232[ 2u]={& _temp233,& _temp234}; Cyc_Std_aprintf(
_tag_arr(" %s %s", sizeof( unsigned char), 7u), _tag_arr( _temp232, sizeof( void*),
2u));}}});} if( Cyc_ic_r){ struct _handler_cons _temp235; _push_handler(&
_temp235);{ int _temp237= 0; if( setjmp( _temp235.handler)){ _temp237= 1;} if( !
_temp237){ Cyc_ccargs=(( struct Cyc_List_List*(*)( int(* f)( struct _tagged_arr*),
struct Cyc_List_List* x)) Cyc_List_filter)( Cyc_is_cfile, Cyc_ccargs); Cyc_libargs=((
struct Cyc_List_List*(*)( int(* f)( struct _tagged_arr*), struct Cyc_List_List*
x)) Cyc_List_filter)( Cyc_is_cfile, Cyc_libargs);{ struct Cyc_List_List*
_temp238=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( stdlib,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_ccargs, Cyc_libargs)); if( !
_temp227){ _temp238=({ struct Cyc_List_List* _temp239=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp239->hd=( void*) Cyc_final_strptr;
_temp239->tl= _temp238; _temp239;});}{ struct Cyc_Interface_I* _temp240=((
struct Cyc_Interface_I*(*)( struct Cyc_Interface_I*(* get)( struct _tagged_arr*),
struct Cyc_List_List* la, struct Cyc_List_List* linfo)) Cyc_Interface_get_and_merge_list)(
Cyc_read_cycio, _temp238, _temp238); if( _temp240 ==  0){({ void* _temp241[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("Error: interfaces incompatible\n",
sizeof( unsigned char), 32u), _tag_arr( _temp241, sizeof( void*), 0u));}); Cyc_compile_failure=
1; Cyc_remove_cfiles();{ int _temp242= 1; _npop_handler( 0u); return _temp242;}}
if( _temp227){ if( Cyc_output_file !=  0){ struct _tagged_arr _temp243=({ struct
Cyc_Std_String_pa_struct _temp247; _temp247.tag= Cyc_Std_String_pa; _temp247.f1=(
struct _tagged_arr) Cyc_Filename_chop_extension(*(( struct _tagged_arr*)
_check_null( Cyc_output_file)));{ void* _temp246[ 1u]={& _temp247}; Cyc_Std_aprintf(
_tag_arr("%s.cycio", sizeof( unsigned char), 9u), _tag_arr( _temp246, sizeof(
void*), 1u));}}); struct Cyc_Std___sFILE* _temp244= Cyc_try_file_open(( struct
_tagged_arr) _temp243, _tag_arr("w", sizeof( unsigned char), 2u), _tag_arr("interface object file",
sizeof( unsigned char), 22u)); if( _temp244 ==  0){ Cyc_compile_failure= 1; Cyc_remove_cfiles();{
int _temp245= 1; _npop_handler( 0u); return _temp245;}} Cyc_Interface_save((
struct Cyc_Interface_I*) _check_null( _temp240),( struct Cyc_Std___sFILE*)
_check_null( _temp244)); Cyc_Std_file_close(( struct Cyc_Std___sFILE*)
_check_null( _temp244));}} else{ if( ! Cyc_Interface_is_subinterface( Cyc_Interface_empty(),(
struct Cyc_Interface_I*) _check_null( _temp240),({ struct _tuple8* _temp248=(
struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp248->f1= _tag_arr("empty interface",
sizeof( unsigned char), 16u); _temp248->f2= _tag_arr("global interface", sizeof(
unsigned char), 17u); _temp248;}))){({ void* _temp249[ 0u]={}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("Error: some objects are still undefined\n", sizeof(
unsigned char), 41u), _tag_arr( _temp249, sizeof( void*), 0u));}); Cyc_compile_failure=
1; Cyc_remove_cfiles();{ int _temp250= 1; _npop_handler( 0u); return _temp250;}}}}};
_pop_handler();} else{ void* _temp236=( void*) _exn_thrown; void* _temp252=
_temp236; struct _tagged_arr _temp266; struct _tagged_arr _temp268; struct
_tagged_arr _temp270; _LL254: if(*(( void**) _temp252) ==  Cyc_Core_Failure){
_LL267: _temp266=(( struct Cyc_Core_Failure_struct*) _temp252)->f1; goto _LL255;}
else{ goto _LL256;} _LL256: if(*(( void**) _temp252) ==  Cyc_Core_Impossible){
_LL269: _temp268=(( struct Cyc_Core_Impossible_struct*) _temp252)->f1; goto
_LL257;} else{ goto _LL258;} _LL258: if( _temp252 ==  Cyc_Dict_Absent){ goto
_LL259;} else{ goto _LL260;} _LL260: if(*(( void**) _temp252) ==  Cyc_Core_Invalid_argument){
_LL271: _temp270=(( struct Cyc_Core_Invalid_argument_struct*) _temp252)->f1;
goto _LL261;} else{ goto _LL262;} _LL262: goto _LL263; _LL264: goto _LL265;
_LL255:({ struct Cyc_Std_String_pa_struct _temp273; _temp273.tag= Cyc_Std_String_pa;
_temp273.f1=( struct _tagged_arr) _temp266;{ void* _temp272[ 1u]={& _temp273};
Cyc_Std_printf( _tag_arr("Exception Core::Failure %s\n", sizeof( unsigned char),
28u), _tag_arr( _temp272, sizeof( void*), 1u));}}); Cyc_compile_failure= 1; Cyc_remove_cfiles();
return 1; _LL257:({ struct Cyc_Std_String_pa_struct _temp275; _temp275.tag= Cyc_Std_String_pa;
_temp275.f1=( struct _tagged_arr) _temp268;{ void* _temp274[ 1u]={& _temp275};
Cyc_Std_printf( _tag_arr("Exception Core::Impossible %s\n", sizeof(
unsigned char), 31u), _tag_arr( _temp274, sizeof( void*), 1u));}}); Cyc_compile_failure=
1; Cyc_remove_cfiles(); return 1; _LL259:({ void* _temp276[ 0u]={}; Cyc_Std_printf(
_tag_arr("Exception Dict::Absent\n", sizeof( unsigned char), 24u), _tag_arr(
_temp276, sizeof( void*), 0u));}); Cyc_compile_failure= 1; Cyc_remove_cfiles();
return 1; _LL261:({ struct Cyc_Std_String_pa_struct _temp278; _temp278.tag= Cyc_Std_String_pa;
_temp278.f1=( struct _tagged_arr) _temp270;{ void* _temp277[ 1u]={& _temp278};
Cyc_Std_printf( _tag_arr("Exception Core::Invalid_argument %s\n", sizeof(
unsigned char), 37u), _tag_arr( _temp277, sizeof( void*), 1u));}}); Cyc_compile_failure=
1; Cyc_remove_cfiles(); return 1; _LL263:({ void* _temp279[ 0u]={}; Cyc_Std_printf(
_tag_arr("Uncaught exception\n", sizeof( unsigned char), 20u), _tag_arr(
_temp279, sizeof( void*), 0u));}); Cyc_compile_failure= 1; Cyc_remove_cfiles();
return 1; _LL265:( void) _throw( _temp252); _LL253:;}}}{ struct _tagged_arr
_temp280=({ struct Cyc_Std_String_pa_struct _temp289; _temp289.tag= Cyc_Std_String_pa;
_temp289.f1=( struct _tagged_arr) _temp226;{ struct Cyc_Std_String_pa_struct
_temp288; _temp288.tag= Cyc_Std_String_pa; _temp288.f1=( struct _tagged_arr)
stdlib_string;{ struct Cyc_Std_String_pa_struct _temp287; _temp287.tag= Cyc_Std_String_pa;
_temp287.f1=( struct _tagged_arr) _temp225;{ struct Cyc_Std_String_pa_struct
_temp286; _temp286.tag= Cyc_Std_String_pa; _temp286.f1=( struct _tagged_arr)(
Cyc_output_file ==  0? _tag_arr("", sizeof( unsigned char), 1u):( struct
_tagged_arr)({ struct Cyc_Std_String_pa_struct _temp291; _temp291.tag= Cyc_Std_String_pa;
_temp291.f1=( struct _tagged_arr) Cyc_sh_escape_string(*(( struct _tagged_arr*)
_check_null( Cyc_output_file)));{ void* _temp290[ 1u]={& _temp291}; Cyc_Std_aprintf(
_tag_arr(" -o %s", sizeof( unsigned char), 7u), _tag_arr( _temp290, sizeof( void*),
1u));}}));{ struct Cyc_Std_String_pa_struct _temp285; _temp285.tag= Cyc_Std_String_pa;
_temp285.f1=( struct _tagged_arr) _temp224;{ void* _temp284[ 5u]={& _temp285,&
_temp286,& _temp287,& _temp288,& _temp289}; Cyc_Std_aprintf( _tag_arr("gcc -include %s%s %s%s%s",
sizeof( unsigned char), 25u), _tag_arr( _temp284, sizeof( void*), 5u));}}}}}});
if( Cyc_v_r){({ struct Cyc_Std_String_pa_struct _temp282; _temp282.tag= Cyc_Std_String_pa;
_temp282.f1=( struct _tagged_arr) _temp280;{ void* _temp281[ 1u]={& _temp282};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s\n", sizeof( unsigned char), 4u),
_tag_arr( _temp281, sizeof( void*), 1u));}});} if( system( string_to_Cstring((
struct _tagged_arr) _temp280)) !=  0){({ void* _temp283[ 0u]={}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("Error: C compiler failed\n", sizeof( unsigned char),
26u), _tag_arr( _temp283, sizeof( void*), 0u));}); Cyc_compile_failure= 1; Cyc_remove_cfiles();
return 1;} Cyc_remove_cfiles(); return Cyc_compile_failure? 1: 0;}}}}}}}
