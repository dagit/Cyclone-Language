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
unsigned char*); extern int* __errno(); struct Cyc_List_List{ void* hd; struct
Cyc_List_List* tl; } ; extern unsigned char Cyc_List_List_empty[ 15u]; extern
unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char Cyc_List_Nth[ 8u];
struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct _tagged_arr line;
int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[ 9u]; struct
Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse=
1; static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; struct
Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern
unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_String_pa=
0; struct Cyc_Stdio_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_Int_pa= 1; struct Cyc_Stdio_Int_pa_struct{ int tag;
unsigned int f1; } ; static const int Cyc_Stdio_Double_pa= 2; struct Cyc_Stdio_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern struct _tagged_arr Cyc_Stdio_aprintf(
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_Stdio_ShortPtr_sa=
0; struct Cyc_Stdio_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_Stdio_UShortPtr_sa= 1; struct Cyc_Stdio_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_Stdio_IntPtr_sa= 2; struct Cyc_Stdio_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_Stdio_UIntPtr_sa= 3; struct Cyc_Stdio_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Stdio_StringPtr_sa= 4;
struct Cyc_Stdio_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_DoublePtr_sa= 5; struct Cyc_Stdio_DoublePtr_sa_struct{ int
tag; double* f1; } ; static const int Cyc_Stdio_FloatPtr_sa= 6; struct Cyc_Stdio_FloatPtr_sa_struct{
int tag; float* f1; } ; struct _tuple0{ void* f1; struct _tagged_arr* f2; } ;
struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref; struct
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
15u]; extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); struct Cyc_PP_Ppstate;
struct Cyc_PP_Out; struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{ int
expand_typedefs: 1; int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1;
int decls_first: 1; int rewrite_temp_tvars: 1; int print_all_tvars: 1; int
print_all_kinds: 1; int print_using_stmts: 1; int print_externC_stmts: 1; int
print_full_evars: 1; int use_curr_namespace: 1; struct Cyc_List_List*
curr_namespace; } ; extern struct _tagged_arr Cyc_Absynpp_typ2string( void*);
struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[
11u]; static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos(
struct _tagged_arr fmt, struct _tagged_arr ap); extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap);
extern void* Cyc_Tcutil_compress( void* t); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern int Cyc_Evexp_okay_szofarg( void* t); void* Cyc_Evexp_eval_const_exp(
struct Cyc_Absyn_Exp* e); void* Cyc_Evexp_promote_const( void* cn){ void* _temp0=
cn; unsigned char _temp8; void* _temp10; short _temp12; void* _temp14; _LL2: if((
unsigned int) _temp0 > 1u?*(( int*) _temp0) == Cyc_Absyn_Char_c: 0){ _LL11:
_temp10=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp0)->f1; goto _LL9; _LL9:
_temp8=(( struct Cyc_Absyn_Char_c_struct*) _temp0)->f2; goto _LL3;} else{ goto
_LL4;} _LL4: if(( unsigned int) _temp0 > 1u?*(( int*) _temp0) == Cyc_Absyn_Short_c:
0){ _LL15: _temp14=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp0)->f1;
goto _LL13; _LL13: _temp12=(( struct Cyc_Absyn_Short_c_struct*) _temp0)->f2;
goto _LL5;} else{ goto _LL6;} _LL6: goto _LL7; _LL3: return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp16=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp16[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp17; _temp17.tag= Cyc_Absyn_Int_c;
_temp17.f1=( void*) _temp10; _temp17.f2=( int) _temp8; _temp17;}); _temp16;});
_LL5: return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp18=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp18[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp19; _temp19.tag= Cyc_Absyn_Int_c; _temp19.f1=( void*) _temp14; _temp19.f2=(
int) _temp12; _temp19;}); _temp18;}); _LL7: return cn; _LL1:;} unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e){ void* _temp20= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e)); int _temp26; _LL22: if(( unsigned int) _temp20 > 1u?*(( int*) _temp20) ==
Cyc_Absyn_Int_c: 0){ _LL27: _temp26=(( struct Cyc_Absyn_Int_c_struct*) _temp20)->f2;
goto _LL23;} else{ goto _LL24;} _LL24: goto _LL25; _LL23: return( unsigned int)
_temp26; _LL25:({ void(* _temp28)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp31= e->loc; struct _tagged_arr _temp32= _tag_arr("expecting unsigned int",
sizeof( unsigned char), 23u); void* _temp29[ 0u]={}; struct _tagged_arr _temp30={(
void*) _temp29,( void*) _temp29,( void*)( _temp29 + 0u)}; _temp28( _temp31,
_temp32, _temp30);}); return 0u; _LL21:;} int Cyc_Evexp_eval_const_bool_exp(
struct Cyc_Absyn_Exp* e){ void* _temp33= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e)); int _temp43; long long _temp45; _LL35: if(( unsigned int) _temp33 > 1u?*((
int*) _temp33) == Cyc_Absyn_Int_c: 0){ _LL44: _temp43=(( struct Cyc_Absyn_Int_c_struct*)
_temp33)->f2; goto _LL36;} else{ goto _LL37;} _LL37: if(( unsigned int) _temp33
> 1u?*(( int*) _temp33) == Cyc_Absyn_LongLong_c: 0){ _LL46: _temp45=(( struct
Cyc_Absyn_LongLong_c_struct*) _temp33)->f2; goto _LL38;} else{ goto _LL39;}
_LL39: if( _temp33 ==( void*) Cyc_Absyn_Null_c){ goto _LL40;} else{ goto _LL41;}
_LL41: goto _LL42; _LL36: return _temp43 != 0; _LL38: return _temp45 != 0; _LL40:
return 0; _LL42:({ void(* _temp47)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp50= e->loc; struct _tagged_arr _temp51= _tag_arr("expecting bool", sizeof(
unsigned char), 15u); void* _temp48[ 0u]={}; struct _tagged_arr _temp49={( void*)
_temp48,( void*) _temp48,( void*)( _temp48 + 0u)}; _temp47( _temp50, _temp51,
_temp49);}); return 0; _LL34:;} struct _tuple3{ struct Cyc_Absyn_Tqual f1; void*
f2; } ; int Cyc_Evexp_okay_szofarg( void* t){ void* _temp52= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Tvar* _temp102; struct Cyc_Core_Opt* _temp104; struct Cyc_Core_Opt
_temp106; void* _temp107; struct Cyc_Absyn_TunionFieldInfo _temp109; void*
_temp111; struct Cyc_Absyn_Tunionfield* _temp113; struct Cyc_List_List* _temp115;
struct Cyc_Absyn_PtrInfo _temp117; struct Cyc_Absyn_Conref* _temp119; struct Cyc_Absyn_Exp*
_temp121; void* _temp123; struct Cyc_Absyn_Structdecl** _temp125; struct Cyc_List_List*
_temp127; struct Cyc_Absyn_Uniondecl** _temp129; struct Cyc_List_List* _temp131;
struct Cyc_List_List* _temp133; _LL54: if( _temp52 ==( void*) Cyc_Absyn_VoidType){
goto _LL55;} else{ goto _LL56;} _LL56: if(( unsigned int) _temp52 > 4u?*(( int*)
_temp52) == Cyc_Absyn_VarType: 0){ _LL103: _temp102=(( struct Cyc_Absyn_VarType_struct*)
_temp52)->f1; goto _LL57;} else{ goto _LL58;} _LL58: if(( unsigned int) _temp52
> 4u?*(( int*) _temp52) == Cyc_Absyn_Evar: 0){ _LL105: _temp104=(( struct Cyc_Absyn_Evar_struct*)
_temp52)->f1; if( _temp104 == 0){ goto _LL60;} else{ _temp106=* _temp104; _LL108:
_temp107=( void*) _temp106.v; if( _temp107 ==( void*) Cyc_Absyn_BoxKind){ goto
_LL59;} else{ goto _LL60;}}} else{ goto _LL60;} _LL60: if(( unsigned int)
_temp52 > 4u?*(( int*) _temp52) == Cyc_Absyn_Evar: 0){ goto _LL61;} else{ goto
_LL62;} _LL62: if(( unsigned int) _temp52 > 4u?*(( int*) _temp52) == Cyc_Absyn_TunionType:
0){ goto _LL63;} else{ goto _LL64;} _LL64: if(( unsigned int) _temp52 > 4u?*((
int*) _temp52) == Cyc_Absyn_TunionFieldType: 0){ _LL110: _temp109=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp52)->f1; _LL112: _temp111=( void*) _temp109.field_info; if(*(( int*)
_temp111) == Cyc_Absyn_KnownTunionfield){ _LL114: _temp113=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp111)->f2; goto _LL65;} else{ goto _LL66;}} else{ goto _LL66;} _LL66: if((
unsigned int) _temp52 > 4u?*(( int*) _temp52) == Cyc_Absyn_TupleType: 0){ _LL116:
_temp115=(( struct Cyc_Absyn_TupleType_struct*) _temp52)->f1; goto _LL67;} else{
goto _LL68;} _LL68: if(( unsigned int) _temp52 > 4u?*(( int*) _temp52) == Cyc_Absyn_TunionFieldType:
0){ goto _LL69;} else{ goto _LL70;} _LL70: if(( unsigned int) _temp52 > 4u?*((
int*) _temp52) == Cyc_Absyn_PointerType: 0){ _LL118: _temp117=(( struct Cyc_Absyn_PointerType_struct*)
_temp52)->f1; _LL120: _temp119= _temp117.bounds; goto _LL71;} else{ goto _LL72;}
_LL72: if(( unsigned int) _temp52 > 4u?*(( int*) _temp52) == Cyc_Absyn_IntType:
0){ goto _LL73;} else{ goto _LL74;} _LL74: if( _temp52 ==( void*) Cyc_Absyn_FloatType){
goto _LL75;} else{ goto _LL76;} _LL76: if( _temp52 ==( void*) Cyc_Absyn_DoubleType){
goto _LL77;} else{ goto _LL78;} _LL78: if(( unsigned int) _temp52 > 4u?*(( int*)
_temp52) == Cyc_Absyn_ArrayType: 0){ _LL124: _temp123=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp52)->f1; goto _LL122; _LL122: _temp121=(( struct Cyc_Absyn_ArrayType_struct*)
_temp52)->f3; goto _LL79;} else{ goto _LL80;} _LL80: if(( unsigned int) _temp52
> 4u?*(( int*) _temp52) == Cyc_Absyn_FnType: 0){ goto _LL81;} else{ goto _LL82;}
_LL82: if(( unsigned int) _temp52 > 4u?*(( int*) _temp52) == Cyc_Absyn_RgnHandleType:
0){ goto _LL83;} else{ goto _LL84;} _LL84: if(( unsigned int) _temp52 > 4u?*((
int*) _temp52) == Cyc_Absyn_StructType: 0){ _LL126: _temp125=(( struct Cyc_Absyn_StructType_struct*)
_temp52)->f3; goto _LL85;} else{ goto _LL86;} _LL86: if(( unsigned int) _temp52
> 4u?*(( int*) _temp52) == Cyc_Absyn_AnonStructType: 0){ _LL128: _temp127=((
struct Cyc_Absyn_AnonStructType_struct*) _temp52)->f1; goto _LL87;} else{ goto
_LL88;} _LL88: if(( unsigned int) _temp52 > 4u?*(( int*) _temp52) == Cyc_Absyn_UnionType:
0){ _LL132: _temp131=(( struct Cyc_Absyn_UnionType_struct*) _temp52)->f2; goto
_LL130; _LL130: _temp129=(( struct Cyc_Absyn_UnionType_struct*) _temp52)->f3;
goto _LL89;} else{ goto _LL90;} _LL90: if(( unsigned int) _temp52 > 4u?*(( int*)
_temp52) == Cyc_Absyn_AnonUnionType: 0){ _LL134: _temp133=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp52)->f1; goto _LL91;} else{ goto _LL92;} _LL92: if(( unsigned int) _temp52
> 4u?*(( int*) _temp52) == Cyc_Absyn_EnumType: 0){ goto _LL93;} else{ goto _LL94;}
_LL94: if( _temp52 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL95;} else{ goto _LL96;}
_LL96: if(( unsigned int) _temp52 > 4u?*(( int*) _temp52) == Cyc_Absyn_JoinEff:
0){ goto _LL97;} else{ goto _LL98;} _LL98: if(( unsigned int) _temp52 > 4u?*((
int*) _temp52) == Cyc_Absyn_AccessEff: 0){ goto _LL99;} else{ goto _LL100;}
_LL100: if(( unsigned int) _temp52 > 4u?*(( int*) _temp52) == Cyc_Absyn_TypedefType:
0){ goto _LL101;} else{ goto _LL53;} _LL55: return 0; _LL57: { void* _temp135=
Cyc_Absyn_conref_val( _temp102->kind); _LL137: if( _temp135 ==( void*) Cyc_Absyn_BoxKind){
goto _LL138;} else{ goto _LL139;} _LL139: goto _LL140; _LL138: return 1; _LL140:
return 0; _LL136:;} _LL59: return 1; _LL61: return 0; _LL63: return 1; _LL65:
_temp115= _temp113->typs; goto _LL67; _LL67: for( 0; _temp115 != 0; _temp115=((
struct Cyc_List_List*) _check_null( _temp115))->tl){ if( ! Cyc_Evexp_okay_szofarg((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp115))->hd)).f2)){
return 0;}} return 1; _LL69: return 0; _LL71: { void* _temp141=( void*)( Cyc_Absyn_compress_conref(
_temp119))->v; void* _temp149; _LL143: if(( unsigned int) _temp141 > 1u?*(( int*)
_temp141) == Cyc_Absyn_Eq_constr: 0){ _LL150: _temp149=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp141)->f1; if( _temp149 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL144;} else{
goto _LL145;}} else{ goto _LL145;} _LL145: if(( unsigned int) _temp141 > 1u?*((
int*) _temp141) == Cyc_Absyn_Eq_constr: 0){ goto _LL146;} else{ goto _LL147;}
_LL147: goto _LL148; _LL144: return 1; _LL146: return 1; _LL148: return 0;
_LL142:;} _LL73: return 1; _LL75: return 1; _LL77: return 1; _LL79: return
_temp121 != 0; _LL81: return 0; _LL83: return 1; _LL85: if( _temp125 == 0){
return({ int(* _temp151)( struct _tagged_arr fmt, struct _tagged_arr ap)=( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos; struct
_tagged_arr _temp154= _tag_arr("szof on unchecked StructType", sizeof(
unsigned char), 29u); void* _temp152[ 0u]={}; struct _tagged_arr _temp153={(
void*) _temp152,( void*) _temp152,( void*)( _temp152 + 0u)}; _temp151( _temp154,
_temp153);});}{ struct Cyc_Absyn_Structdecl* _temp155=*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp125)); if( _temp155->fields == 0){ return 0;} _temp127=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp155->fields))->v;
goto _LL87;} _LL87: for( 0; _temp127 != 0; _temp127=(( struct Cyc_List_List*)
_check_null( _temp127))->tl){ if( ! Cyc_Evexp_okay_szofarg(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp127))->hd)->type)){ return 0;}} return
1; _LL89: if( _temp129 == 0){ return({ int(* _temp156)( struct _tagged_arr fmt,
struct _tagged_arr ap)=( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos; struct _tagged_arr _temp159= _tag_arr("szof on unchecked UnionType",
sizeof( unsigned char), 28u); void* _temp157[ 0u]={}; struct _tagged_arr
_temp158={( void*) _temp157,( void*) _temp157,( void*)( _temp157 + 0u)};
_temp156( _temp159, _temp158);});}{ struct Cyc_Absyn_Uniondecl* _temp160=*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp129)); if( _temp160->fields == 0){
return 0;} _temp133=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp160->fields))->v; goto _LL91;} _LL91: for( 0; _temp133 != 0; _temp133=((
struct Cyc_List_List*) _check_null( _temp133))->tl){ if( ! Cyc_Evexp_okay_szofarg((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp133))->hd)->type)){ return 0;}} return 1; _LL93: return 1; _LL95: return 0;
_LL97: return 0; _LL99: return 0; _LL101: return({ int(* _temp161)( struct
_tagged_arr fmt, struct _tagged_arr ap)=( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos; struct _tagged_arr _temp164=( struct
_tagged_arr)({ struct _tagged_arr(* _temp165)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp169= _tag_arr("szof typedeftype %s",
sizeof( unsigned char), 20u); struct Cyc_Stdio_String_pa_struct _temp170;
_temp170.tag= Cyc_Stdio_String_pa; _temp170.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp168=( void*)& _temp170; void* _temp166[ 1u]={ _temp168}; struct
_tagged_arr _temp167={( void*) _temp166,( void*) _temp166,( void*)( _temp166 + 1u)};
_temp165( _temp169, _temp167);}}); void* _temp162[ 0u]={}; struct _tagged_arr
_temp163={( void*) _temp162,( void*) _temp162,( void*)( _temp162 + 0u)};
_temp161( _temp164, _temp163);}); _LL53:;} struct _tuple4{ void* f1; void* f2; }
; void* Cyc_Evexp_eval_const_unprimop( void* p, struct Cyc_Absyn_Exp* e){ void*
cn= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp( e)); struct _tuple4
_temp172=({ struct _tuple4 _temp171; _temp171.f1= p; _temp171.f2= cn; _temp171;});
void* _temp186; void* _temp188; int _temp190; void* _temp192; void* _temp194;
void* _temp196; int _temp198; void* _temp200; void* _temp202; void* _temp204;
int _temp206; void* _temp208; void* _temp210; void* _temp212; _LL174: _LL187:
_temp186= _temp172.f1; if( _temp186 ==( void*) Cyc_Absyn_Plus){ goto _LL175;}
else{ goto _LL176;} _LL176: _LL195: _temp194= _temp172.f1; if( _temp194 ==( void*)
Cyc_Absyn_Minus){ goto _LL189;} else{ goto _LL178;} _LL189: _temp188= _temp172.f2;
if(( unsigned int) _temp188 > 1u?*(( int*) _temp188) == Cyc_Absyn_Int_c: 0){
_LL193: _temp192=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp188)->f1; goto
_LL191; _LL191: _temp190=(( struct Cyc_Absyn_Int_c_struct*) _temp188)->f2; goto
_LL177;} else{ goto _LL178;} _LL178: _LL203: _temp202= _temp172.f1; if( _temp202
==( void*) Cyc_Absyn_Bitnot){ goto _LL197;} else{ goto _LL180;} _LL197: _temp196=
_temp172.f2; if(( unsigned int) _temp196 > 1u?*(( int*) _temp196) == Cyc_Absyn_Int_c:
0){ _LL201: _temp200=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp196)->f1;
goto _LL199; _LL199: _temp198=(( struct Cyc_Absyn_Int_c_struct*) _temp196)->f2;
goto _LL179;} else{ goto _LL180;} _LL180: _LL209: _temp208= _temp172.f1; if(
_temp208 ==( void*) Cyc_Absyn_Not){ goto _LL205;} else{ goto _LL182;} _LL205:
_temp204= _temp172.f2; if(( unsigned int) _temp204 > 1u?*(( int*) _temp204) ==
Cyc_Absyn_Int_c: 0){ _LL207: _temp206=(( struct Cyc_Absyn_Int_c_struct*)
_temp204)->f2; goto _LL181;} else{ goto _LL182;} _LL182: _LL213: _temp212=
_temp172.f1; if( _temp212 ==( void*) Cyc_Absyn_Not){ goto _LL211;} else{ goto
_LL184;} _LL211: _temp210= _temp172.f2; if( _temp210 ==( void*) Cyc_Absyn_Null_c){
goto _LL183;} else{ goto _LL184;} _LL184: goto _LL185; _LL175: return cn; _LL177:
return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp214=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp214[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp215; _temp215.tag= Cyc_Absyn_Int_c; _temp215.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp215.f2= - _temp190; _temp215;}); _temp214;}); _LL179: return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp216=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp216[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp217; _temp217.tag= Cyc_Absyn_Int_c; _temp217.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp217.f2= ~ _temp198; _temp217;}); _temp216;}); _LL181: return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp218=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp218[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp219; _temp219.tag= Cyc_Absyn_Int_c; _temp219.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp219.f2= _temp206 == 0? 1: 0; _temp219;}); _temp218;}); _LL183: return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp220=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp220[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp221; _temp221.tag= Cyc_Absyn_Int_c; _temp221.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp221.f2= 1; _temp221;}); _temp220;}); _LL185:({ void(* _temp222)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp225= e->loc; struct _tagged_arr _temp226= _tag_arr("bad constant expression",
sizeof( unsigned char), 24u); void* _temp223[ 0u]={}; struct _tagged_arr
_temp224={( void*) _temp223,( void*) _temp223,( void*)( _temp223 + 0u)};
_temp222( _temp225, _temp226, _temp224);}); return cn; _LL173:;} struct _tuple5{
void* f1; int f2; } ; void* Cyc_Evexp_eval_const_binprimop( void* p, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* _temp227= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e1)); void* _temp228= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp( e2));
void* s1; void* s2; int i1; int i2;{ void* _temp229= _temp227; int _temp235;
void* _temp237; _LL231: if(( unsigned int) _temp229 > 1u?*(( int*) _temp229) ==
Cyc_Absyn_Int_c: 0){ _LL238: _temp237=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp229)->f1; goto _LL236; _LL236: _temp235=(( struct Cyc_Absyn_Int_c_struct*)
_temp229)->f2; goto _LL232;} else{ goto _LL233;} _LL233: goto _LL234; _LL232: s1=
_temp237; i1= _temp235; goto _LL230; _LL234:({ void(* _temp239)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp242= e1->loc; struct _tagged_arr _temp243= _tag_arr("bad constant expression",
sizeof( unsigned char), 24u); void* _temp240[ 0u]={}; struct _tagged_arr
_temp241={( void*) _temp240,( void*) _temp240,( void*)( _temp240 + 0u)};
_temp239( _temp242, _temp243, _temp241);}); return _temp227; _LL230:;}{ void*
_temp244= _temp228; int _temp250; void* _temp252; _LL246: if(( unsigned int)
_temp244 > 1u?*(( int*) _temp244) == Cyc_Absyn_Int_c: 0){ _LL253: _temp252=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp244)->f1; goto _LL251; _LL251:
_temp250=(( struct Cyc_Absyn_Int_c_struct*) _temp244)->f2; goto _LL247;} else{
goto _LL248;} _LL248: goto _LL249; _LL247: s2= _temp252; i2= _temp250; goto
_LL245; _LL249:({ void(* _temp254)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp257= e2->loc; struct _tagged_arr _temp258= _tag_arr("bad constant expression",
sizeof( unsigned char), 24u); void* _temp255[ 0u]={}; struct _tagged_arr
_temp256={( void*) _temp255,( void*) _temp255,( void*)( _temp255 + 0u)};
_temp254( _temp257, _temp258, _temp256);}); return _temp227; _LL245:;}{ void*
_temp259= p; _LL261: if( _temp259 ==( void*) Cyc_Absyn_Div){ goto _LL262;} else{
goto _LL263;} _LL263: if( _temp259 ==( void*) Cyc_Absyn_Mod){ goto _LL264;}
else{ goto _LL265;} _LL265: goto _LL266; _LL262: goto _LL264; _LL264: if( i2 ==
0){({ void(* _temp267)( struct Cyc_Position_Segment*, struct _tagged_arr fmt,
struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp270=
e2->loc; struct _tagged_arr _temp271= _tag_arr("division by zero in constant expression",
sizeof( unsigned char), 40u); void* _temp268[ 0u]={}; struct _tagged_arr
_temp269={( void*) _temp268,( void*) _temp268,( void*)( _temp268 + 0u)};
_temp267( _temp270, _temp271, _temp269);}); return _temp227;} goto _LL260;
_LL266: goto _LL260; _LL260:;}{ int has_u_arg= s1 ==( void*) Cyc_Absyn_Unsigned?
1: s2 ==( void*) Cyc_Absyn_Unsigned; unsigned int u1=( unsigned int) i1;
unsigned int u2=( unsigned int) i2; int i3= 0; unsigned int u3= 0; int b3= 1;
int use_i3= 0; int use_u3= 0; int use_b3= 0;{ struct _tuple5 _temp273=({ struct
_tuple5 _temp272; _temp272.f1= p; _temp272.f2= has_u_arg; _temp272;}); int
_temp329; void* _temp331; int _temp333; void* _temp335; int _temp337; void*
_temp339; int _temp341; void* _temp343; int _temp345; void* _temp347; int
_temp349; void* _temp351; int _temp353; void* _temp355; int _temp357; void*
_temp359; int _temp361; void* _temp363; int _temp365; void* _temp367; void*
_temp369; void* _temp371; int _temp373; void* _temp375; int _temp377; void*
_temp379; int _temp381; void* _temp383; int _temp385; void* _temp387; int
_temp389; void* _temp391; int _temp393; void* _temp395; int _temp397; void*
_temp399; int _temp401; void* _temp403; void* _temp405; void* _temp407; void*
_temp409; void* _temp411; void* _temp413; void* _temp415; _LL275: _LL332:
_temp331= _temp273.f1; if( _temp331 ==( void*) Cyc_Absyn_Plus){ goto _LL330;}
else{ goto _LL277;} _LL330: _temp329= _temp273.f2; if( _temp329 == 0){ goto
_LL276;} else{ goto _LL277;} _LL277: _LL336: _temp335= _temp273.f1; if( _temp335
==( void*) Cyc_Absyn_Times){ goto _LL334;} else{ goto _LL279;} _LL334: _temp333=
_temp273.f2; if( _temp333 == 0){ goto _LL278;} else{ goto _LL279;} _LL279:
_LL340: _temp339= _temp273.f1; if( _temp339 ==( void*) Cyc_Absyn_Minus){ goto
_LL338;} else{ goto _LL281;} _LL338: _temp337= _temp273.f2; if( _temp337 == 0){
goto _LL280;} else{ goto _LL281;} _LL281: _LL344: _temp343= _temp273.f1; if(
_temp343 ==( void*) Cyc_Absyn_Div){ goto _LL342;} else{ goto _LL283;} _LL342:
_temp341= _temp273.f2; if( _temp341 == 0){ goto _LL282;} else{ goto _LL283;}
_LL283: _LL348: _temp347= _temp273.f1; if( _temp347 ==( void*) Cyc_Absyn_Mod){
goto _LL346;} else{ goto _LL285;} _LL346: _temp345= _temp273.f2; if( _temp345 ==
0){ goto _LL284;} else{ goto _LL285;} _LL285: _LL352: _temp351= _temp273.f1; if(
_temp351 ==( void*) Cyc_Absyn_Plus){ goto _LL350;} else{ goto _LL287;} _LL350:
_temp349= _temp273.f2; if( _temp349 == 1){ goto _LL286;} else{ goto _LL287;}
_LL287: _LL356: _temp355= _temp273.f1; if( _temp355 ==( void*) Cyc_Absyn_Times){
goto _LL354;} else{ goto _LL289;} _LL354: _temp353= _temp273.f2; if( _temp353 ==
1){ goto _LL288;} else{ goto _LL289;} _LL289: _LL360: _temp359= _temp273.f1; if(
_temp359 ==( void*) Cyc_Absyn_Minus){ goto _LL358;} else{ goto _LL291;} _LL358:
_temp357= _temp273.f2; if( _temp357 == 1){ goto _LL290;} else{ goto _LL291;}
_LL291: _LL364: _temp363= _temp273.f1; if( _temp363 ==( void*) Cyc_Absyn_Div){
goto _LL362;} else{ goto _LL293;} _LL362: _temp361= _temp273.f2; if( _temp361 ==
1){ goto _LL292;} else{ goto _LL293;} _LL293: _LL368: _temp367= _temp273.f1; if(
_temp367 ==( void*) Cyc_Absyn_Mod){ goto _LL366;} else{ goto _LL295;} _LL366:
_temp365= _temp273.f2; if( _temp365 == 1){ goto _LL294;} else{ goto _LL295;}
_LL295: _LL370: _temp369= _temp273.f1; if( _temp369 ==( void*) Cyc_Absyn_Eq){
goto _LL296;} else{ goto _LL297;} _LL297: _LL372: _temp371= _temp273.f1; if(
_temp371 ==( void*) Cyc_Absyn_Neq){ goto _LL298;} else{ goto _LL299;} _LL299:
_LL376: _temp375= _temp273.f1; if( _temp375 ==( void*) Cyc_Absyn_Gt){ goto
_LL374;} else{ goto _LL301;} _LL374: _temp373= _temp273.f2; if( _temp373 == 0){
goto _LL300;} else{ goto _LL301;} _LL301: _LL380: _temp379= _temp273.f1; if(
_temp379 ==( void*) Cyc_Absyn_Lt){ goto _LL378;} else{ goto _LL303;} _LL378:
_temp377= _temp273.f2; if( _temp377 == 0){ goto _LL302;} else{ goto _LL303;}
_LL303: _LL384: _temp383= _temp273.f1; if( _temp383 ==( void*) Cyc_Absyn_Gte){
goto _LL382;} else{ goto _LL305;} _LL382: _temp381= _temp273.f2; if( _temp381 ==
0){ goto _LL304;} else{ goto _LL305;} _LL305: _LL388: _temp387= _temp273.f1; if(
_temp387 ==( void*) Cyc_Absyn_Lte){ goto _LL386;} else{ goto _LL307;} _LL386:
_temp385= _temp273.f2; if( _temp385 == 0){ goto _LL306;} else{ goto _LL307;}
_LL307: _LL392: _temp391= _temp273.f1; if( _temp391 ==( void*) Cyc_Absyn_Gt){
goto _LL390;} else{ goto _LL309;} _LL390: _temp389= _temp273.f2; if( _temp389 ==
1){ goto _LL308;} else{ goto _LL309;} _LL309: _LL396: _temp395= _temp273.f1; if(
_temp395 ==( void*) Cyc_Absyn_Lt){ goto _LL394;} else{ goto _LL311;} _LL394:
_temp393= _temp273.f2; if( _temp393 == 1){ goto _LL310;} else{ goto _LL311;}
_LL311: _LL400: _temp399= _temp273.f1; if( _temp399 ==( void*) Cyc_Absyn_Gte){
goto _LL398;} else{ goto _LL313;} _LL398: _temp397= _temp273.f2; if( _temp397 ==
1){ goto _LL312;} else{ goto _LL313;} _LL313: _LL404: _temp403= _temp273.f1; if(
_temp403 ==( void*) Cyc_Absyn_Lte){ goto _LL402;} else{ goto _LL315;} _LL402:
_temp401= _temp273.f2; if( _temp401 == 1){ goto _LL314;} else{ goto _LL315;}
_LL315: _LL406: _temp405= _temp273.f1; if( _temp405 ==( void*) Cyc_Absyn_Bitand){
goto _LL316;} else{ goto _LL317;} _LL317: _LL408: _temp407= _temp273.f1; if(
_temp407 ==( void*) Cyc_Absyn_Bitor){ goto _LL318;} else{ goto _LL319;} _LL319:
_LL410: _temp409= _temp273.f1; if( _temp409 ==( void*) Cyc_Absyn_Bitxor){ goto
_LL320;} else{ goto _LL321;} _LL321: _LL412: _temp411= _temp273.f1; if( _temp411
==( void*) Cyc_Absyn_Bitlshift){ goto _LL322;} else{ goto _LL323;} _LL323:
_LL414: _temp413= _temp273.f1; if( _temp413 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL324;} else{ goto _LL325;} _LL325: _LL416: _temp415= _temp273.f1; if(
_temp415 ==( void*) Cyc_Absyn_Bitarshift){ goto _LL326;} else{ goto _LL327;}
_LL327: goto _LL328; _LL276: i3= i1 + i2; use_i3= 1; goto _LL274; _LL278: i3= i1
* i2; use_i3= 1; goto _LL274; _LL280: i3= i1 - i2; use_i3= 1; goto _LL274;
_LL282: i3= i1 / i2; use_i3= 1; goto _LL274; _LL284: i3= i1 % i2; use_i3= 1;
goto _LL274; _LL286: u3= u1 + u2; use_u3= 1; goto _LL274; _LL288: u3= u1 * u2;
use_u3= 1; goto _LL274; _LL290: u3= u1 - u2; use_u3= 1; goto _LL274; _LL292: u3=
u1 / u2; use_u3= 1; goto _LL274; _LL294: u3= u1 % u2; use_u3= 1; goto _LL274;
_LL296: b3= i1 == i2; use_b3= 1; goto _LL274; _LL298: b3= i1 != i2; use_b3= 1;
goto _LL274; _LL300: b3= i1 > i2; use_b3= 1; goto _LL274; _LL302: b3= i1 < i2;
use_b3= 1; goto _LL274; _LL304: b3= i1 >= i2; use_b3= 1; goto _LL274; _LL306: b3=
i1 <= i2; use_b3= 1; goto _LL274; _LL308: b3= u1 > u2; use_b3= 1; goto _LL274;
_LL310: b3= u1 < u2; use_b3= 1; goto _LL274; _LL312: b3= u1 >= u2; use_b3= 1;
goto _LL274; _LL314: b3= u1 <= u2; use_b3= 1; goto _LL274; _LL316: u3= u1 & u2;
use_u3= 1; goto _LL274; _LL318: u3= u1 | u2; use_u3= 1; goto _LL274; _LL320: u3=
u1 ^ u2; use_u3= 1; goto _LL274; _LL322: u3= u1 << u2; use_u3= 1; goto _LL274;
_LL324: u3= u1 >> u2; use_u3= 1; goto _LL274; _LL326:({ void(* _temp417)( struct
Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr;
struct Cyc_Position_Segment* _temp420= e1->loc; struct _tagged_arr _temp421=
_tag_arr(">>> NOT IMPLEMENTED", sizeof( unsigned char), 20u); void* _temp418[ 0u]={};
struct _tagged_arr _temp419={( void*) _temp418,( void*) _temp418,( void*)(
_temp418 + 0u)}; _temp417( _temp420, _temp421, _temp419);}); return _temp227;
_LL328:({ void(* _temp422)( struct Cyc_Position_Segment*, struct _tagged_arr fmt,
struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp425=
e1->loc; struct _tagged_arr _temp426= _tag_arr("bad constant expression",
sizeof( unsigned char), 24u); void* _temp423[ 0u]={}; struct _tagged_arr
_temp424={( void*) _temp423,( void*) _temp423,( void*)( _temp423 + 0u)};
_temp422( _temp425, _temp426, _temp424);}); return _temp227; _LL274:;} if(
use_i3){ return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp427=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp427[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp428; _temp428.tag= Cyc_Absyn_Int_c; _temp428.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp428.f2= i3; _temp428;}); _temp427;});} if( use_u3){ return( void*)({ struct
Cyc_Absyn_Int_c_struct* _temp429=( struct Cyc_Absyn_Int_c_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Int_c_struct)); _temp429[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp430; _temp430.tag= Cyc_Absyn_Int_c; _temp430.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp430.f2=( int) u3; _temp430;}); _temp429;});} if( use_b3){ return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp431=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp431[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp432; _temp432.tag= Cyc_Absyn_Int_c; _temp432.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp432.f2= b3? 1: 0; _temp432;}); _temp431;});}( int) _throw(( void*)({ struct
Cyc_Core_Unreachable_struct* _temp433=( struct Cyc_Core_Unreachable_struct*)
GC_malloc( sizeof( struct Cyc_Core_Unreachable_struct)); _temp433[ 0]=({ struct
Cyc_Core_Unreachable_struct _temp434; _temp434.tag= Cyc_Core_Unreachable;
_temp434.f1= _tag_arr("Evexp::eval_const_binop", sizeof( unsigned char), 24u);
_temp434;}); _temp433;}));}} void* Cyc_Evexp_eval_const_exp( struct Cyc_Absyn_Exp*
e){ void* cn;{ void* _temp435=( void*) e->r; void* _temp455; struct Cyc_Absyn_Exp*
_temp457; struct Cyc_Absyn_Exp* _temp459; struct Cyc_Absyn_Exp* _temp461; struct
Cyc_List_List* _temp463; void* _temp465; struct Cyc_Absyn_Exp* _temp467; void*
_temp469; struct Cyc_Absyn_Enumfield* _temp471; _LL437: if(*(( int*) _temp435)
== Cyc_Absyn_Const_e){ _LL456: _temp455=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp435)->f1; goto _LL438;} else{ goto _LL439;} _LL439: if(*(( int*) _temp435)
== Cyc_Absyn_Conditional_e){ _LL462: _temp461=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp435)->f1; goto _LL460; _LL460: _temp459=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp435)->f2; goto _LL458; _LL458: _temp457=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp435)->f3; goto _LL440;} else{ goto _LL441;} _LL441: if(*(( int*) _temp435)
== Cyc_Absyn_Primop_e){ _LL466: _temp465=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp435)->f1; goto _LL464; _LL464: _temp463=(( struct Cyc_Absyn_Primop_e_struct*)
_temp435)->f2; goto _LL442;} else{ goto _LL443;} _LL443: if(*(( int*) _temp435)
== Cyc_Absyn_Sizeoftyp_e){ goto _LL444;} else{ goto _LL445;} _LL445: if(*(( int*)
_temp435) == Cyc_Absyn_Sizeofexp_e){ goto _LL446;} else{ goto _LL447;} _LL447:
if(*(( int*) _temp435) == Cyc_Absyn_Offsetof_e){ goto _LL448;} else{ goto _LL449;}
_LL449: if(*(( int*) _temp435) == Cyc_Absyn_Cast_e){ _LL470: _temp469=( void*)((
struct Cyc_Absyn_Cast_e_struct*) _temp435)->f1; goto _LL468; _LL468: _temp467=((
struct Cyc_Absyn_Cast_e_struct*) _temp435)->f2; goto _LL450;} else{ goto _LL451;}
_LL451: if(*(( int*) _temp435) == Cyc_Absyn_Enum_e){ _LL472: _temp471=(( struct
Cyc_Absyn_Enum_e_struct*) _temp435)->f3; goto _LL452;} else{ goto _LL453;}
_LL453: goto _LL454; _LL438: return _temp455; _LL440: cn= Cyc_Evexp_eval_const_bool_exp(
_temp461)? Cyc_Evexp_eval_const_exp( _temp459): Cyc_Evexp_eval_const_exp(
_temp457); goto _LL436; _LL442: if( _temp463 == 0){({ void(* _temp473)( struct
Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr;
struct Cyc_Position_Segment* _temp476= e->loc; struct _tagged_arr _temp477=
_tag_arr("bad static expression (no args to primop)", sizeof( unsigned char), 42u);
void* _temp474[ 0u]={}; struct _tagged_arr _temp475={( void*) _temp474,( void*)
_temp474,( void*)( _temp474 + 0u)}; _temp473( _temp476, _temp477, _temp475);});
return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp478=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp478[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp479; _temp479.tag= Cyc_Absyn_Int_c; _temp479.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp479.f2= 0; _temp479;}); _temp478;});} if((( struct Cyc_List_List*)
_check_null( _temp463))->tl == 0){ cn= Cyc_Evexp_eval_const_unprimop( _temp465,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp463))->hd);}
else{ if((( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp463))->tl))->tl != 0){({ void(* _temp480)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp483= e->loc; struct _tagged_arr _temp484= _tag_arr("bad static expression (too many args to primop)",
sizeof( unsigned char), 48u); void* _temp481[ 0u]={}; struct _tagged_arr
_temp482={( void*) _temp481,( void*) _temp481,( void*)( _temp481 + 0u)};
_temp480( _temp483, _temp484, _temp482);}); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp485=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp485[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp486; _temp486.tag= Cyc_Absyn_Int_c;
_temp486.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp486.f2= 0; _temp486;});
_temp485;});} else{ cn= Cyc_Evexp_eval_const_binprimop( _temp465,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp463))->hd,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp463))->tl))->hd);}} goto _LL436; _LL444: goto _LL446; _LL446:({ void(*
_temp487)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp490= e->loc;
struct _tagged_arr _temp491= _tag_arr("sizeof(...) is not an integral constant expression in Cyclone",
sizeof( unsigned char), 62u); void* _temp488[ 0u]={}; struct _tagged_arr
_temp489={( void*) _temp488,( void*) _temp488,( void*)( _temp488 + 0u)};
_temp487( _temp490, _temp491, _temp489);}); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp492=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp492[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp493; _temp493.tag= Cyc_Absyn_Int_c;
_temp493.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp493.f2= 0; _temp493;});
_temp492;}); _LL448:({ void(* _temp494)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp497= e->loc; struct _tagged_arr _temp498= _tag_arr("offsetof(...) is not an integral constant expression in Cyclone",
sizeof( unsigned char), 64u); void* _temp495[ 0u]={}; struct _tagged_arr
_temp496={( void*) _temp495,( void*) _temp495,( void*)( _temp495 + 0u)};
_temp494( _temp497, _temp498, _temp496);}); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp499=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp499[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp500; _temp500.tag= Cyc_Absyn_Int_c;
_temp500.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp500.f2= 0; _temp500;});
_temp499;}); _LL450: cn= Cyc_Evexp_eval_const_exp( _temp467);{ struct _tuple4
_temp502=({ struct _tuple4 _temp501; _temp501.f1= Cyc_Tcutil_compress( _temp469);
_temp501.f2= cn; _temp501;}); void* _temp512; unsigned char _temp514; void*
_temp516; void* _temp518; void* _temp520; void* _temp522; void* _temp524; short
_temp526; void* _temp528; void* _temp530; void* _temp532; void* _temp534; void*
_temp536; int _temp538; void* _temp540; void* _temp542; void* _temp544; void*
_temp546; _LL504: _LL519: _temp518= _temp502.f1; if(( unsigned int) _temp518 > 4u?*((
int*) _temp518) == Cyc_Absyn_IntType: 0){ _LL523: _temp522=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp518)->f1; goto _LL521; _LL521: _temp520=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp518)->f2; goto _LL513;} else{ goto _LL506;} _LL513: _temp512= _temp502.f2;
if(( unsigned int) _temp512 > 1u?*(( int*) _temp512) == Cyc_Absyn_Char_c: 0){
_LL517: _temp516=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp512)->f1; goto
_LL515; _LL515: _temp514=(( struct Cyc_Absyn_Char_c_struct*) _temp512)->f2; goto
_LL505;} else{ goto _LL506;} _LL506: _LL531: _temp530= _temp502.f1; if((
unsigned int) _temp530 > 4u?*(( int*) _temp530) == Cyc_Absyn_IntType: 0){ _LL535:
_temp534=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp530)->f1; goto _LL533;
_LL533: _temp532=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp530)->f2;
goto _LL525;} else{ goto _LL508;} _LL525: _temp524= _temp502.f2; if((
unsigned int) _temp524 > 1u?*(( int*) _temp524) == Cyc_Absyn_Short_c: 0){ _LL529:
_temp528=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp524)->f1; goto _LL527;
_LL527: _temp526=(( struct Cyc_Absyn_Short_c_struct*) _temp524)->f2; goto _LL507;}
else{ goto _LL508;} _LL508: _LL543: _temp542= _temp502.f1; if(( unsigned int)
_temp542 > 4u?*(( int*) _temp542) == Cyc_Absyn_IntType: 0){ _LL547: _temp546=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp542)->f1; goto _LL545; _LL545:
_temp544=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp542)->f2; goto _LL537;}
else{ goto _LL510;} _LL537: _temp536= _temp502.f2; if(( unsigned int) _temp536 >
1u?*(( int*) _temp536) == Cyc_Absyn_Int_c: 0){ _LL541: _temp540=( void*)((
struct Cyc_Absyn_Int_c_struct*) _temp536)->f1; goto _LL539; _LL539: _temp538=((
struct Cyc_Absyn_Int_c_struct*) _temp536)->f2; goto _LL509;} else{ goto _LL510;}
_LL510: goto _LL511; _LL505: _temp534= _temp522; _temp532= _temp520; _temp528=
_temp516; _temp526=( short) _temp514; goto _LL507; _LL507: _temp546= _temp534;
_temp544= _temp532; _temp540= _temp528; _temp538=( int) _temp526; goto _LL509;
_LL509: if( _temp546 != _temp540){ cn=( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp548=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp548[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp549; _temp549.tag= Cyc_Absyn_Int_c;
_temp549.f1=( void*) _temp546; _temp549.f2= _temp538; _temp549;}); _temp548;});}
goto _LL503; _LL511:({ void(* _temp550)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp554= e->loc; struct _tagged_arr _temp555= _tag_arr("eval_const: cannot cast to %s",
sizeof( unsigned char), 30u); struct Cyc_Stdio_String_pa_struct _temp556;
_temp556.tag= Cyc_Stdio_String_pa; _temp556.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp469);{ void* _temp553=( void*)& _temp556; void* _temp551[ 1u]={ _temp553};
struct _tagged_arr _temp552={( void*) _temp551,( void*) _temp551,( void*)(
_temp551 + 1u)}; _temp550( _temp554, _temp555, _temp552);}}); return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp557=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp557[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp558; _temp558.tag= Cyc_Absyn_Int_c; _temp558.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp558.f2= 0; _temp558;}); _temp557;}); _LL503:;} goto _LL436; _LL452: return
Cyc_Evexp_eval_const_exp(( struct Cyc_Absyn_Exp*) _check_null((( struct Cyc_Absyn_Enumfield*)
_check_null( _temp471))->tag)); _LL454:({ void(* _temp559)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp562= e->loc; struct _tagged_arr _temp563= _tag_arr("bad static expression",
sizeof( unsigned char), 22u); void* _temp560[ 0u]={}; struct _tagged_arr
_temp561={( void*) _temp560,( void*) _temp560,( void*)( _temp560 + 0u)};
_temp559( _temp562, _temp563, _temp561);}); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp564=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp564[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp565; _temp565.tag= Cyc_Absyn_Int_c;
_temp565.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp565.f2= 0; _temp565;});
_temp564;}); _LL436:;}( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp566=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp566[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp567; _temp567.tag= Cyc_Absyn_Const_e;
_temp567.f1=( void*) cn; _temp567;}); _temp566;}))); return cn;}