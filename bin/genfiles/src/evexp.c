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
_tagged_arr desc; } ; extern struct Cyc_Position_Error* Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*, struct _tagged_arr); extern unsigned char Cyc_Position_Nocontext[
14u]; extern void Cyc_Position_post_error( struct Cyc_Position_Error*); struct
Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern
unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_String_pa=
0; struct Cyc_Stdio_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_Int_pa= 1; struct Cyc_Stdio_Int_pa_struct{ int tag;
unsigned int f1; } ; static const int Cyc_Stdio_Double_pa= 2; struct Cyc_Stdio_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern struct _tagged_arr Cyc_Stdio_xprintf(
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
struct _tagged_arr); extern void* Cyc_Tcutil_compress( void* t); extern
unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); extern int
Cyc_Evexp_okay_szofarg( void* t); void Cyc_Evexp_exp_err( struct Cyc_Position_Segment*
loc, struct _tagged_arr msg){ Cyc_Position_post_error( Cyc_Position_mk_err_elab(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp0)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr _temp4= _tag_arr("Exp_err: %s",
sizeof( unsigned char), 12u); struct Cyc_Stdio_String_pa_struct _temp5; _temp5.tag=
Cyc_Stdio_String_pa; _temp5.f1=( struct _tagged_arr) msg;{ void* _temp3=( void*)&
_temp5; void* _temp1[ 1u]={ _temp3}; struct _tagged_arr _temp2={( void*) _temp1,(
void*) _temp1,( void*)( _temp1 + 1u)}; _temp0( _temp4, _temp2);}})));} void* Cyc_Evexp_eval_const_exp(
struct Cyc_Absyn_Exp* e); void* Cyc_Evexp_promote_const( void* cn){ void* _temp6=
cn; unsigned char _temp14; void* _temp16; short _temp18; void* _temp20; _LL8:
if(( unsigned int) _temp6 > 1u?*(( int*) _temp6) == Cyc_Absyn_Char_c: 0){ _LL17:
_temp16=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp6)->f1; goto _LL15;
_LL15: _temp14=(( struct Cyc_Absyn_Char_c_struct*) _temp6)->f2; goto _LL9;}
else{ goto _LL10;} _LL10: if(( unsigned int) _temp6 > 1u?*(( int*) _temp6) ==
Cyc_Absyn_Short_c: 0){ _LL21: _temp20=( void*)(( struct Cyc_Absyn_Short_c_struct*)
_temp6)->f1; goto _LL19; _LL19: _temp18=(( struct Cyc_Absyn_Short_c_struct*)
_temp6)->f2; goto _LL11;} else{ goto _LL12;} _LL12: goto _LL13; _LL9: return(
void*)({ struct Cyc_Absyn_Int_c_struct* _temp22=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp22[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp23; _temp23.tag= Cyc_Absyn_Int_c; _temp23.f1=( void*) _temp16; _temp23.f2=(
int) _temp14; _temp23;}); _temp22;}); _LL11: return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp24=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp24[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp25; _temp25.tag= Cyc_Absyn_Int_c;
_temp25.f1=( void*) _temp20; _temp25.f2=( int) _temp18; _temp25;}); _temp24;});
_LL13: return cn; _LL7:;} unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp*
e){ void* _temp26= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp( e)); int
_temp32; _LL28: if(( unsigned int) _temp26 > 1u?*(( int*) _temp26) == Cyc_Absyn_Int_c:
0){ _LL33: _temp32=(( struct Cyc_Absyn_Int_c_struct*) _temp26)->f2; goto _LL29;}
else{ goto _LL30;} _LL30: goto _LL31; _LL29: return( unsigned int) _temp32;
_LL31: Cyc_Evexp_exp_err( e->loc, _tag_arr("expecting unsigned int", sizeof(
unsigned char), 23u)); return 0u; _LL27:;} int Cyc_Evexp_eval_const_bool_exp(
struct Cyc_Absyn_Exp* e){ void* _temp34= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e)); int _temp44; long long _temp46; _LL36: if(( unsigned int) _temp34 > 1u?*((
int*) _temp34) == Cyc_Absyn_Int_c: 0){ _LL45: _temp44=(( struct Cyc_Absyn_Int_c_struct*)
_temp34)->f2; goto _LL37;} else{ goto _LL38;} _LL38: if(( unsigned int) _temp34
> 1u?*(( int*) _temp34) == Cyc_Absyn_LongLong_c: 0){ _LL47: _temp46=(( struct
Cyc_Absyn_LongLong_c_struct*) _temp34)->f2; goto _LL39;} else{ goto _LL40;}
_LL40: if( _temp34 ==( void*) Cyc_Absyn_Null_c){ goto _LL41;} else{ goto _LL42;}
_LL42: goto _LL43; _LL37: return _temp44 != 0; _LL39: return _temp46 != 0; _LL41:
return 0; _LL43: Cyc_Evexp_exp_err( e->loc, _tag_arr("expecting bool", sizeof(
unsigned char), 15u)); return 0; _LL35:;} struct _tuple3{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; int Cyc_Evexp_okay_szofarg( void* t){ void* _temp48= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Tvar* _temp98; struct Cyc_Core_Opt* _temp100; struct Cyc_Core_Opt
_temp102; void* _temp103; struct Cyc_Absyn_TunionFieldInfo _temp105; void*
_temp107; struct Cyc_Absyn_Tunionfield* _temp109; struct Cyc_List_List* _temp111;
struct Cyc_Absyn_PtrInfo _temp113; struct Cyc_Absyn_Conref* _temp115; struct Cyc_Absyn_Exp*
_temp117; void* _temp119; struct Cyc_Absyn_Structdecl** _temp121; struct Cyc_List_List*
_temp123; struct Cyc_Absyn_Uniondecl** _temp125; struct Cyc_List_List* _temp127;
struct Cyc_List_List* _temp129; _LL50: if( _temp48 ==( void*) Cyc_Absyn_VoidType){
goto _LL51;} else{ goto _LL52;} _LL52: if(( unsigned int) _temp48 > 4u?*(( int*)
_temp48) == Cyc_Absyn_VarType: 0){ _LL99: _temp98=(( struct Cyc_Absyn_VarType_struct*)
_temp48)->f1; goto _LL53;} else{ goto _LL54;} _LL54: if(( unsigned int) _temp48
> 4u?*(( int*) _temp48) == Cyc_Absyn_Evar: 0){ _LL101: _temp100=(( struct Cyc_Absyn_Evar_struct*)
_temp48)->f1; if( _temp100 == 0){ goto _LL56;} else{ _temp102=* _temp100; _LL104:
_temp103=( void*) _temp102.v; if( _temp103 ==( void*) Cyc_Absyn_BoxKind){ goto
_LL55;} else{ goto _LL56;}}} else{ goto _LL56;} _LL56: if(( unsigned int)
_temp48 > 4u?*(( int*) _temp48) == Cyc_Absyn_Evar: 0){ goto _LL57;} else{ goto
_LL58;} _LL58: if(( unsigned int) _temp48 > 4u?*(( int*) _temp48) == Cyc_Absyn_TunionType:
0){ goto _LL59;} else{ goto _LL60;} _LL60: if(( unsigned int) _temp48 > 4u?*((
int*) _temp48) == Cyc_Absyn_TunionFieldType: 0){ _LL106: _temp105=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp48)->f1; _LL108: _temp107=( void*) _temp105.field_info; if(*(( int*)
_temp107) == Cyc_Absyn_KnownTunionfield){ _LL110: _temp109=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp107)->f2; goto _LL61;} else{ goto _LL62;}} else{ goto _LL62;} _LL62: if((
unsigned int) _temp48 > 4u?*(( int*) _temp48) == Cyc_Absyn_TupleType: 0){ _LL112:
_temp111=(( struct Cyc_Absyn_TupleType_struct*) _temp48)->f1; goto _LL63;} else{
goto _LL64;} _LL64: if(( unsigned int) _temp48 > 4u?*(( int*) _temp48) == Cyc_Absyn_TunionFieldType:
0){ goto _LL65;} else{ goto _LL66;} _LL66: if(( unsigned int) _temp48 > 4u?*((
int*) _temp48) == Cyc_Absyn_PointerType: 0){ _LL114: _temp113=(( struct Cyc_Absyn_PointerType_struct*)
_temp48)->f1; _LL116: _temp115= _temp113.bounds; goto _LL67;} else{ goto _LL68;}
_LL68: if(( unsigned int) _temp48 > 4u?*(( int*) _temp48) == Cyc_Absyn_IntType:
0){ goto _LL69;} else{ goto _LL70;} _LL70: if( _temp48 ==( void*) Cyc_Absyn_FloatType){
goto _LL71;} else{ goto _LL72;} _LL72: if( _temp48 ==( void*) Cyc_Absyn_DoubleType){
goto _LL73;} else{ goto _LL74;} _LL74: if(( unsigned int) _temp48 > 4u?*(( int*)
_temp48) == Cyc_Absyn_ArrayType: 0){ _LL120: _temp119=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp48)->f1; goto _LL118; _LL118: _temp117=(( struct Cyc_Absyn_ArrayType_struct*)
_temp48)->f3; goto _LL75;} else{ goto _LL76;} _LL76: if(( unsigned int) _temp48
> 4u?*(( int*) _temp48) == Cyc_Absyn_FnType: 0){ goto _LL77;} else{ goto _LL78;}
_LL78: if(( unsigned int) _temp48 > 4u?*(( int*) _temp48) == Cyc_Absyn_RgnHandleType:
0){ goto _LL79;} else{ goto _LL80;} _LL80: if(( unsigned int) _temp48 > 4u?*((
int*) _temp48) == Cyc_Absyn_StructType: 0){ _LL122: _temp121=(( struct Cyc_Absyn_StructType_struct*)
_temp48)->f3; goto _LL81;} else{ goto _LL82;} _LL82: if(( unsigned int) _temp48
> 4u?*(( int*) _temp48) == Cyc_Absyn_AnonStructType: 0){ _LL124: _temp123=((
struct Cyc_Absyn_AnonStructType_struct*) _temp48)->f1; goto _LL83;} else{ goto
_LL84;} _LL84: if(( unsigned int) _temp48 > 4u?*(( int*) _temp48) == Cyc_Absyn_UnionType:
0){ _LL128: _temp127=(( struct Cyc_Absyn_UnionType_struct*) _temp48)->f2; goto
_LL126; _LL126: _temp125=(( struct Cyc_Absyn_UnionType_struct*) _temp48)->f3;
goto _LL85;} else{ goto _LL86;} _LL86: if(( unsigned int) _temp48 > 4u?*(( int*)
_temp48) == Cyc_Absyn_AnonUnionType: 0){ _LL130: _temp129=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp48)->f1; goto _LL87;} else{ goto _LL88;} _LL88: if(( unsigned int) _temp48
> 4u?*(( int*) _temp48) == Cyc_Absyn_EnumType: 0){ goto _LL89;} else{ goto _LL90;}
_LL90: if( _temp48 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL91;} else{ goto _LL92;}
_LL92: if(( unsigned int) _temp48 > 4u?*(( int*) _temp48) == Cyc_Absyn_JoinEff:
0){ goto _LL93;} else{ goto _LL94;} _LL94: if(( unsigned int) _temp48 > 4u?*((
int*) _temp48) == Cyc_Absyn_AccessEff: 0){ goto _LL95;} else{ goto _LL96;} _LL96:
if(( unsigned int) _temp48 > 4u?*(( int*) _temp48) == Cyc_Absyn_TypedefType: 0){
goto _LL97;} else{ goto _LL49;} _LL51: return 0; _LL53: { void* _temp131= Cyc_Absyn_conref_val(
_temp98->kind); _LL133: if( _temp131 ==( void*) Cyc_Absyn_BoxKind){ goto _LL134;}
else{ goto _LL135;} _LL135: goto _LL136; _LL134: return 1; _LL136: return 0;
_LL132:;} _LL55: return 1; _LL57: return 0; _LL59: return 1; _LL61: _temp111=
_temp109->typs; goto _LL63; _LL63: for( 0; _temp111 != 0; _temp111=(( struct Cyc_List_List*)
_check_null( _temp111))->tl){ if( ! Cyc_Evexp_okay_szofarg((*(( struct _tuple3*)((
struct Cyc_List_List*) _check_null( _temp111))->hd)).f2)){ return 0;}} return 1;
_LL65: return 0; _LL67: { void* _temp137=( void*)( Cyc_Absyn_compress_conref(
_temp115))->v; void* _temp145; _LL139: if(( unsigned int) _temp137 > 1u?*(( int*)
_temp137) == Cyc_Absyn_Eq_constr: 0){ _LL146: _temp145=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp137)->f1; if( _temp145 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL140;} else{
goto _LL141;}} else{ goto _LL141;} _LL141: if(( unsigned int) _temp137 > 1u?*((
int*) _temp137) == Cyc_Absyn_Eq_constr: 0){ goto _LL142;} else{ goto _LL143;}
_LL143: goto _LL144; _LL140: return 1; _LL142: return 1; _LL144: return 0;
_LL138:;} _LL69: return 1; _LL71: return 1; _LL73: return 1; _LL75: return
_temp117 != 0; _LL77: return 0; _LL79: return 1; _LL81: if( _temp121 == 0){
return(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("szof on unchecked StructType",
sizeof( unsigned char), 29u));}{ struct Cyc_Absyn_Structdecl* _temp147=*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp121)); if( _temp147->fields ==
0){ return 0;} _temp123=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp147->fields))->v; goto _LL83;} _LL83: for( 0; _temp123 != 0;
_temp123=(( struct Cyc_List_List*) _check_null( _temp123))->tl){ if( ! Cyc_Evexp_okay_szofarg((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp123))->hd)->type)){ return 0;}} return 1; _LL85: if( _temp125 == 0){ return((
int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("szof on unchecked UnionType",
sizeof( unsigned char), 28u));}{ struct Cyc_Absyn_Uniondecl* _temp148=*(( struct
Cyc_Absyn_Uniondecl**) _check_null( _temp125)); if( _temp148->fields == 0){
return 0;} _temp129=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp148->fields))->v; goto _LL87;} _LL87: for( 0; _temp129 != 0; _temp129=((
struct Cyc_List_List*) _check_null( _temp129))->tl){ if( ! Cyc_Evexp_okay_szofarg((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp129))->hd)->type)){ return 0;}} return 1; _LL89: return 1; _LL91: return 0;
_LL93: return 0; _LL95: return 0; _LL97: return(( int(*)( struct _tagged_arr))
Cyc_Tcutil_impos)(( struct _tagged_arr)({ struct _tagged_arr(* _temp149)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr
_temp153= _tag_arr("szof typedeftype %s", sizeof( unsigned char), 20u); struct
Cyc_Stdio_String_pa_struct _temp154; _temp154.tag= Cyc_Stdio_String_pa; _temp154.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp152=( void*)&
_temp154; void* _temp150[ 1u]={ _temp152}; struct _tagged_arr _temp151={( void*)
_temp150,( void*) _temp150,( void*)( _temp150 + 1u)}; _temp149( _temp153,
_temp151);}})); _LL49:;} struct _tuple4{ void* f1; void* f2; } ; void* Cyc_Evexp_eval_const_unprimop(
void* p, struct Cyc_Absyn_Exp* e){ void* cn= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e)); struct _tuple4 _temp156=({ struct _tuple4 _temp155; _temp155.f1= p;
_temp155.f2= cn; _temp155;}); void* _temp170; void* _temp172; int _temp174; void*
_temp176; void* _temp178; void* _temp180; int _temp182; void* _temp184; void*
_temp186; void* _temp188; int _temp190; void* _temp192; void* _temp194; void*
_temp196; _LL158: _LL171: _temp170= _temp156.f1; if( _temp170 ==( void*) Cyc_Absyn_Plus){
goto _LL159;} else{ goto _LL160;} _LL160: _LL179: _temp178= _temp156.f1; if(
_temp178 ==( void*) Cyc_Absyn_Minus){ goto _LL173;} else{ goto _LL162;} _LL173:
_temp172= _temp156.f2; if(( unsigned int) _temp172 > 1u?*(( int*) _temp172) ==
Cyc_Absyn_Int_c: 0){ _LL177: _temp176=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp172)->f1; goto _LL175; _LL175: _temp174=(( struct Cyc_Absyn_Int_c_struct*)
_temp172)->f2; goto _LL161;} else{ goto _LL162;} _LL162: _LL187: _temp186=
_temp156.f1; if( _temp186 ==( void*) Cyc_Absyn_Bitnot){ goto _LL181;} else{ goto
_LL164;} _LL181: _temp180= _temp156.f2; if(( unsigned int) _temp180 > 1u?*(( int*)
_temp180) == Cyc_Absyn_Int_c: 0){ _LL185: _temp184=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp180)->f1; goto _LL183; _LL183: _temp182=(( struct Cyc_Absyn_Int_c_struct*)
_temp180)->f2; goto _LL163;} else{ goto _LL164;} _LL164: _LL193: _temp192=
_temp156.f1; if( _temp192 ==( void*) Cyc_Absyn_Not){ goto _LL189;} else{ goto
_LL166;} _LL189: _temp188= _temp156.f2; if(( unsigned int) _temp188 > 1u?*(( int*)
_temp188) == Cyc_Absyn_Int_c: 0){ _LL191: _temp190=(( struct Cyc_Absyn_Int_c_struct*)
_temp188)->f2; goto _LL165;} else{ goto _LL166;} _LL166: _LL197: _temp196=
_temp156.f1; if( _temp196 ==( void*) Cyc_Absyn_Not){ goto _LL195;} else{ goto
_LL168;} _LL195: _temp194= _temp156.f2; if( _temp194 ==( void*) Cyc_Absyn_Null_c){
goto _LL167;} else{ goto _LL168;} _LL168: goto _LL169; _LL159: return cn; _LL161:
return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp198=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp198[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp199; _temp199.tag= Cyc_Absyn_Int_c; _temp199.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp199.f2= - _temp174; _temp199;}); _temp198;}); _LL163: return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp200=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp200[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp201; _temp201.tag= Cyc_Absyn_Int_c; _temp201.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp201.f2= ~ _temp182; _temp201;}); _temp200;}); _LL165: return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp202=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp202[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp203; _temp203.tag= Cyc_Absyn_Int_c; _temp203.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp203.f2= _temp190 == 0? 1: 0; _temp203;}); _temp202;}); _LL167: return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp204=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp204[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp205; _temp205.tag= Cyc_Absyn_Int_c; _temp205.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp205.f2= 1; _temp205;}); _temp204;}); _LL169: Cyc_Evexp_exp_err( e->loc,
_tag_arr("bad constant expression", sizeof( unsigned char), 24u)); return cn;
_LL157:;} struct _tuple5{ void* f1; int f2; } ; void* Cyc_Evexp_eval_const_binprimop(
void* p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ void* _temp206=
Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp( e1)); void* _temp207= Cyc_Evexp_promote_const(
Cyc_Evexp_eval_const_exp( e2)); void* s1; void* s2; int i1; int i2;{ void*
_temp208= _temp206; int _temp214; void* _temp216; _LL210: if(( unsigned int)
_temp208 > 1u?*(( int*) _temp208) == Cyc_Absyn_Int_c: 0){ _LL217: _temp216=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp208)->f1; goto _LL215; _LL215:
_temp214=(( struct Cyc_Absyn_Int_c_struct*) _temp208)->f2; goto _LL211;} else{
goto _LL212;} _LL212: goto _LL213; _LL211: s1= _temp216; i1= _temp214; goto
_LL209; _LL213: Cyc_Evexp_exp_err( e1->loc, _tag_arr("bad constant expression",
sizeof( unsigned char), 24u)); return _temp206; _LL209:;}{ void* _temp218=
_temp207; int _temp224; void* _temp226; _LL220: if(( unsigned int) _temp218 > 1u?*((
int*) _temp218) == Cyc_Absyn_Int_c: 0){ _LL227: _temp226=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp218)->f1; goto _LL225; _LL225: _temp224=(( struct Cyc_Absyn_Int_c_struct*)
_temp218)->f2; goto _LL221;} else{ goto _LL222;} _LL222: goto _LL223; _LL221: s2=
_temp226; i2= _temp224; goto _LL219; _LL223: Cyc_Evexp_exp_err( e2->loc,
_tag_arr("bad constant expression", sizeof( unsigned char), 24u)); return
_temp206; _LL219:;}{ void* _temp228= p; _LL230: if( _temp228 ==( void*) Cyc_Absyn_Div){
goto _LL231;} else{ goto _LL232;} _LL232: if( _temp228 ==( void*) Cyc_Absyn_Mod){
goto _LL233;} else{ goto _LL234;} _LL234: goto _LL235; _LL231: goto _LL233;
_LL233: if( i2 == 0){ Cyc_Evexp_exp_err( e2->loc, _tag_arr("division by zero in constant expression",
sizeof( unsigned char), 40u)); return _temp206;} goto _LL229; _LL235: goto
_LL229; _LL229:;}{ int has_u_arg= s1 ==( void*) Cyc_Absyn_Unsigned? 1: s2 ==(
void*) Cyc_Absyn_Unsigned; unsigned int u1=( unsigned int) i1; unsigned int u2=(
unsigned int) i2; int i3= 0; unsigned int u3= 0; int b3= 1; int use_i3= 0; int
use_u3= 0; int use_b3= 0;{ struct _tuple5 _temp237=({ struct _tuple5 _temp236;
_temp236.f1= p; _temp236.f2= has_u_arg; _temp236;}); int _temp293; void*
_temp295; int _temp297; void* _temp299; int _temp301; void* _temp303; int
_temp305; void* _temp307; int _temp309; void* _temp311; int _temp313; void*
_temp315; int _temp317; void* _temp319; int _temp321; void* _temp323; int
_temp325; void* _temp327; int _temp329; void* _temp331; void* _temp333; void*
_temp335; int _temp337; void* _temp339; int _temp341; void* _temp343; int
_temp345; void* _temp347; int _temp349; void* _temp351; int _temp353; void*
_temp355; int _temp357; void* _temp359; int _temp361; void* _temp363; int
_temp365; void* _temp367; void* _temp369; void* _temp371; void* _temp373; void*
_temp375; void* _temp377; void* _temp379; _LL239: _LL296: _temp295= _temp237.f1;
if( _temp295 ==( void*) Cyc_Absyn_Plus){ goto _LL294;} else{ goto _LL241;}
_LL294: _temp293= _temp237.f2; if( _temp293 == 0){ goto _LL240;} else{ goto
_LL241;} _LL241: _LL300: _temp299= _temp237.f1; if( _temp299 ==( void*) Cyc_Absyn_Times){
goto _LL298;} else{ goto _LL243;} _LL298: _temp297= _temp237.f2; if( _temp297 ==
0){ goto _LL242;} else{ goto _LL243;} _LL243: _LL304: _temp303= _temp237.f1; if(
_temp303 ==( void*) Cyc_Absyn_Minus){ goto _LL302;} else{ goto _LL245;} _LL302:
_temp301= _temp237.f2; if( _temp301 == 0){ goto _LL244;} else{ goto _LL245;}
_LL245: _LL308: _temp307= _temp237.f1; if( _temp307 ==( void*) Cyc_Absyn_Div){
goto _LL306;} else{ goto _LL247;} _LL306: _temp305= _temp237.f2; if( _temp305 ==
0){ goto _LL246;} else{ goto _LL247;} _LL247: _LL312: _temp311= _temp237.f1; if(
_temp311 ==( void*) Cyc_Absyn_Mod){ goto _LL310;} else{ goto _LL249;} _LL310:
_temp309= _temp237.f2; if( _temp309 == 0){ goto _LL248;} else{ goto _LL249;}
_LL249: _LL316: _temp315= _temp237.f1; if( _temp315 ==( void*) Cyc_Absyn_Plus){
goto _LL314;} else{ goto _LL251;} _LL314: _temp313= _temp237.f2; if( _temp313 ==
1){ goto _LL250;} else{ goto _LL251;} _LL251: _LL320: _temp319= _temp237.f1; if(
_temp319 ==( void*) Cyc_Absyn_Times){ goto _LL318;} else{ goto _LL253;} _LL318:
_temp317= _temp237.f2; if( _temp317 == 1){ goto _LL252;} else{ goto _LL253;}
_LL253: _LL324: _temp323= _temp237.f1; if( _temp323 ==( void*) Cyc_Absyn_Minus){
goto _LL322;} else{ goto _LL255;} _LL322: _temp321= _temp237.f2; if( _temp321 ==
1){ goto _LL254;} else{ goto _LL255;} _LL255: _LL328: _temp327= _temp237.f1; if(
_temp327 ==( void*) Cyc_Absyn_Div){ goto _LL326;} else{ goto _LL257;} _LL326:
_temp325= _temp237.f2; if( _temp325 == 1){ goto _LL256;} else{ goto _LL257;}
_LL257: _LL332: _temp331= _temp237.f1; if( _temp331 ==( void*) Cyc_Absyn_Mod){
goto _LL330;} else{ goto _LL259;} _LL330: _temp329= _temp237.f2; if( _temp329 ==
1){ goto _LL258;} else{ goto _LL259;} _LL259: _LL334: _temp333= _temp237.f1; if(
_temp333 ==( void*) Cyc_Absyn_Eq){ goto _LL260;} else{ goto _LL261;} _LL261:
_LL336: _temp335= _temp237.f1; if( _temp335 ==( void*) Cyc_Absyn_Neq){ goto
_LL262;} else{ goto _LL263;} _LL263: _LL340: _temp339= _temp237.f1; if( _temp339
==( void*) Cyc_Absyn_Gt){ goto _LL338;} else{ goto _LL265;} _LL338: _temp337=
_temp237.f2; if( _temp337 == 0){ goto _LL264;} else{ goto _LL265;} _LL265:
_LL344: _temp343= _temp237.f1; if( _temp343 ==( void*) Cyc_Absyn_Lt){ goto
_LL342;} else{ goto _LL267;} _LL342: _temp341= _temp237.f2; if( _temp341 == 0){
goto _LL266;} else{ goto _LL267;} _LL267: _LL348: _temp347= _temp237.f1; if(
_temp347 ==( void*) Cyc_Absyn_Gte){ goto _LL346;} else{ goto _LL269;} _LL346:
_temp345= _temp237.f2; if( _temp345 == 0){ goto _LL268;} else{ goto _LL269;}
_LL269: _LL352: _temp351= _temp237.f1; if( _temp351 ==( void*) Cyc_Absyn_Lte){
goto _LL350;} else{ goto _LL271;} _LL350: _temp349= _temp237.f2; if( _temp349 ==
0){ goto _LL270;} else{ goto _LL271;} _LL271: _LL356: _temp355= _temp237.f1; if(
_temp355 ==( void*) Cyc_Absyn_Gt){ goto _LL354;} else{ goto _LL273;} _LL354:
_temp353= _temp237.f2; if( _temp353 == 1){ goto _LL272;} else{ goto _LL273;}
_LL273: _LL360: _temp359= _temp237.f1; if( _temp359 ==( void*) Cyc_Absyn_Lt){
goto _LL358;} else{ goto _LL275;} _LL358: _temp357= _temp237.f2; if( _temp357 ==
1){ goto _LL274;} else{ goto _LL275;} _LL275: _LL364: _temp363= _temp237.f1; if(
_temp363 ==( void*) Cyc_Absyn_Gte){ goto _LL362;} else{ goto _LL277;} _LL362:
_temp361= _temp237.f2; if( _temp361 == 1){ goto _LL276;} else{ goto _LL277;}
_LL277: _LL368: _temp367= _temp237.f1; if( _temp367 ==( void*) Cyc_Absyn_Lte){
goto _LL366;} else{ goto _LL279;} _LL366: _temp365= _temp237.f2; if( _temp365 ==
1){ goto _LL278;} else{ goto _LL279;} _LL279: _LL370: _temp369= _temp237.f1; if(
_temp369 ==( void*) Cyc_Absyn_Bitand){ goto _LL280;} else{ goto _LL281;} _LL281:
_LL372: _temp371= _temp237.f1; if( _temp371 ==( void*) Cyc_Absyn_Bitor){ goto
_LL282;} else{ goto _LL283;} _LL283: _LL374: _temp373= _temp237.f1; if( _temp373
==( void*) Cyc_Absyn_Bitxor){ goto _LL284;} else{ goto _LL285;} _LL285: _LL376:
_temp375= _temp237.f1; if( _temp375 ==( void*) Cyc_Absyn_Bitlshift){ goto _LL286;}
else{ goto _LL287;} _LL287: _LL378: _temp377= _temp237.f1; if( _temp377 ==( void*)
Cyc_Absyn_Bitlrshift){ goto _LL288;} else{ goto _LL289;} _LL289: _LL380:
_temp379= _temp237.f1; if( _temp379 ==( void*) Cyc_Absyn_Bitarshift){ goto
_LL290;} else{ goto _LL291;} _LL291: goto _LL292; _LL240: i3= i1 + i2; use_i3= 1;
goto _LL238; _LL242: i3= i1 * i2; use_i3= 1; goto _LL238; _LL244: i3= i1 - i2;
use_i3= 1; goto _LL238; _LL246: i3= i1 / i2; use_i3= 1; goto _LL238; _LL248: i3=
i1 % i2; use_i3= 1; goto _LL238; _LL250: u3= u1 + u2; use_u3= 1; goto _LL238;
_LL252: u3= u1 * u2; use_u3= 1; goto _LL238; _LL254: u3= u1 - u2; use_u3= 1;
goto _LL238; _LL256: u3= u1 / u2; use_u3= 1; goto _LL238; _LL258: u3= u1 % u2;
use_u3= 1; goto _LL238; _LL260: b3= i1 == i2; use_b3= 1; goto _LL238; _LL262: b3=
i1 != i2; use_b3= 1; goto _LL238; _LL264: b3= i1 > i2; use_b3= 1; goto _LL238;
_LL266: b3= i1 < i2; use_b3= 1; goto _LL238; _LL268: b3= i1 >= i2; use_b3= 1;
goto _LL238; _LL270: b3= i1 <= i2; use_b3= 1; goto _LL238; _LL272: b3= u1 > u2;
use_b3= 1; goto _LL238; _LL274: b3= u1 < u2; use_b3= 1; goto _LL238; _LL276: b3=
u1 >= u2; use_b3= 1; goto _LL238; _LL278: b3= u1 <= u2; use_b3= 1; goto _LL238;
_LL280: u3= u1 & u2; use_u3= 1; goto _LL238; _LL282: u3= u1 | u2; use_u3= 1;
goto _LL238; _LL284: u3= u1 ^ u2; use_u3= 1; goto _LL238; _LL286: u3= u1 << u2;
use_u3= 1; goto _LL238; _LL288: u3= u1 >> u2; use_u3= 1; goto _LL238; _LL290:
Cyc_Evexp_exp_err( e1->loc, _tag_arr(">>> NOT IMPLEMENTED", sizeof(
unsigned char), 20u)); return _temp206; _LL292: Cyc_Evexp_exp_err( e1->loc,
_tag_arr("bad constant expression", sizeof( unsigned char), 24u)); return
_temp206; _LL238:;} if( use_i3){ return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp381=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp381[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp382; _temp382.tag= Cyc_Absyn_Int_c;
_temp382.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp382.f2= i3; _temp382;});
_temp381;});} if( use_u3){ return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp383=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp383[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp384; _temp384.tag= Cyc_Absyn_Int_c;
_temp384.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp384.f2=( int) u3;
_temp384;}); _temp383;});} if( use_b3){ return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp385=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp385[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp386; _temp386.tag= Cyc_Absyn_Int_c;
_temp386.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp386.f2= b3? 1: 0; _temp386;});
_temp385;});}( int) _throw(( void*)({ struct Cyc_Core_Unreachable_struct*
_temp387=( struct Cyc_Core_Unreachable_struct*) GC_malloc( sizeof( struct Cyc_Core_Unreachable_struct));
_temp387[ 0]=({ struct Cyc_Core_Unreachable_struct _temp388; _temp388.tag= Cyc_Core_Unreachable;
_temp388.f1= _tag_arr("Evexp::eval_const_binop", sizeof( unsigned char), 24u);
_temp388;}); _temp387;}));}} void* Cyc_Evexp_eval_const_exp( struct Cyc_Absyn_Exp*
e){ void* cn;{ void* _temp389=( void*) e->r; void* _temp409; struct Cyc_Absyn_Exp*
_temp411; struct Cyc_Absyn_Exp* _temp413; struct Cyc_Absyn_Exp* _temp415; struct
Cyc_List_List* _temp417; void* _temp419; struct Cyc_Absyn_Exp* _temp421; void*
_temp423; struct Cyc_Absyn_Enumfield* _temp425; _LL391: if(*(( int*) _temp389)
== Cyc_Absyn_Const_e){ _LL410: _temp409=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp389)->f1; goto _LL392;} else{ goto _LL393;} _LL393: if(*(( int*) _temp389)
== Cyc_Absyn_Conditional_e){ _LL416: _temp415=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp389)->f1; goto _LL414; _LL414: _temp413=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp389)->f2; goto _LL412; _LL412: _temp411=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp389)->f3; goto _LL394;} else{ goto _LL395;} _LL395: if(*(( int*) _temp389)
== Cyc_Absyn_Primop_e){ _LL420: _temp419=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp389)->f1; goto _LL418; _LL418: _temp417=(( struct Cyc_Absyn_Primop_e_struct*)
_temp389)->f2; goto _LL396;} else{ goto _LL397;} _LL397: if(*(( int*) _temp389)
== Cyc_Absyn_Sizeoftyp_e){ goto _LL398;} else{ goto _LL399;} _LL399: if(*(( int*)
_temp389) == Cyc_Absyn_Sizeofexp_e){ goto _LL400;} else{ goto _LL401;} _LL401:
if(*(( int*) _temp389) == Cyc_Absyn_Offsetof_e){ goto _LL402;} else{ goto _LL403;}
_LL403: if(*(( int*) _temp389) == Cyc_Absyn_Cast_e){ _LL424: _temp423=( void*)((
struct Cyc_Absyn_Cast_e_struct*) _temp389)->f1; goto _LL422; _LL422: _temp421=((
struct Cyc_Absyn_Cast_e_struct*) _temp389)->f2; goto _LL404;} else{ goto _LL405;}
_LL405: if(*(( int*) _temp389) == Cyc_Absyn_Enum_e){ _LL426: _temp425=(( struct
Cyc_Absyn_Enum_e_struct*) _temp389)->f3; goto _LL406;} else{ goto _LL407;}
_LL407: goto _LL408; _LL392: return _temp409; _LL394: cn= Cyc_Evexp_eval_const_bool_exp(
_temp415)? Cyc_Evexp_eval_const_exp( _temp413): Cyc_Evexp_eval_const_exp(
_temp411); goto _LL390; _LL396: if( _temp417 == 0){ Cyc_Evexp_exp_err( e->loc,
_tag_arr("bad static expression (no args to primop)", sizeof( unsigned char), 42u));
return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp427=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp427[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp428; _temp428.tag= Cyc_Absyn_Int_c; _temp428.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp428.f2= 0; _temp428;}); _temp427;});} if((( struct Cyc_List_List*)
_check_null( _temp417))->tl == 0){ cn= Cyc_Evexp_eval_const_unprimop( _temp419,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp417))->hd);}
else{ if((( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp417))->tl))->tl != 0){ Cyc_Evexp_exp_err( e->loc, _tag_arr("bad static expression (too many args to primop)",
sizeof( unsigned char), 48u)); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp429=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp429[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp430; _temp430.tag= Cyc_Absyn_Int_c;
_temp430.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp430.f2= 0; _temp430;});
_temp429;});} else{ cn= Cyc_Evexp_eval_const_binprimop( _temp419,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp417))->hd,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp417))->tl))->hd);}} goto _LL390; _LL398: goto _LL400; _LL400: Cyc_Evexp_exp_err(
e->loc, _tag_arr("sizeof(...) is not an integral constant expression in Cyclone",
sizeof( unsigned char), 62u)); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp431=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp431[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp432; _temp432.tag= Cyc_Absyn_Int_c;
_temp432.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp432.f2= 0; _temp432;});
_temp431;}); _LL402: Cyc_Evexp_exp_err( e->loc, _tag_arr("offsetof(...) is not an integral constant expression in Cyclone",
sizeof( unsigned char), 64u)); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp433=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp433[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp434; _temp434.tag= Cyc_Absyn_Int_c;
_temp434.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp434.f2= 0; _temp434;});
_temp433;}); _LL404: cn= Cyc_Evexp_eval_const_exp( _temp421);{ struct _tuple4
_temp436=({ struct _tuple4 _temp435; _temp435.f1= Cyc_Tcutil_compress( _temp423);
_temp435.f2= cn; _temp435;}); void* _temp446; unsigned char _temp448; void*
_temp450; void* _temp452; void* _temp454; void* _temp456; void* _temp458; short
_temp460; void* _temp462; void* _temp464; void* _temp466; void* _temp468; void*
_temp470; int _temp472; void* _temp474; void* _temp476; void* _temp478; void*
_temp480; _LL438: _LL453: _temp452= _temp436.f1; if(( unsigned int) _temp452 > 4u?*((
int*) _temp452) == Cyc_Absyn_IntType: 0){ _LL457: _temp456=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp452)->f1; goto _LL455; _LL455: _temp454=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp452)->f2; goto _LL447;} else{ goto _LL440;} _LL447: _temp446= _temp436.f2;
if(( unsigned int) _temp446 > 1u?*(( int*) _temp446) == Cyc_Absyn_Char_c: 0){
_LL451: _temp450=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp446)->f1; goto
_LL449; _LL449: _temp448=(( struct Cyc_Absyn_Char_c_struct*) _temp446)->f2; goto
_LL439;} else{ goto _LL440;} _LL440: _LL465: _temp464= _temp436.f1; if((
unsigned int) _temp464 > 4u?*(( int*) _temp464) == Cyc_Absyn_IntType: 0){ _LL469:
_temp468=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp464)->f1; goto _LL467;
_LL467: _temp466=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp464)->f2;
goto _LL459;} else{ goto _LL442;} _LL459: _temp458= _temp436.f2; if((
unsigned int) _temp458 > 1u?*(( int*) _temp458) == Cyc_Absyn_Short_c: 0){ _LL463:
_temp462=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp458)->f1; goto _LL461;
_LL461: _temp460=(( struct Cyc_Absyn_Short_c_struct*) _temp458)->f2; goto _LL441;}
else{ goto _LL442;} _LL442: _LL477: _temp476= _temp436.f1; if(( unsigned int)
_temp476 > 4u?*(( int*) _temp476) == Cyc_Absyn_IntType: 0){ _LL481: _temp480=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp476)->f1; goto _LL479; _LL479:
_temp478=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp476)->f2; goto _LL471;}
else{ goto _LL444;} _LL471: _temp470= _temp436.f2; if(( unsigned int) _temp470 >
1u?*(( int*) _temp470) == Cyc_Absyn_Int_c: 0){ _LL475: _temp474=( void*)((
struct Cyc_Absyn_Int_c_struct*) _temp470)->f1; goto _LL473; _LL473: _temp472=((
struct Cyc_Absyn_Int_c_struct*) _temp470)->f2; goto _LL443;} else{ goto _LL444;}
_LL444: goto _LL445; _LL439: _temp468= _temp456; _temp466= _temp454; _temp462=
_temp450; _temp460=( short) _temp448; goto _LL441; _LL441: _temp480= _temp468;
_temp478= _temp466; _temp474= _temp462; _temp472=( int) _temp460; goto _LL443;
_LL443: if( _temp480 != _temp474){ cn=( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp482=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp482[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp483; _temp483.tag= Cyc_Absyn_Int_c;
_temp483.f1=( void*) _temp480; _temp483.f2= _temp472; _temp483;}); _temp482;});}
goto _LL437; _LL445: Cyc_Evexp_exp_err( e->loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp484)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf;
struct _tagged_arr _temp488= _tag_arr("eval_const: cannot cast to %s", sizeof(
unsigned char), 30u); struct Cyc_Stdio_String_pa_struct _temp489; _temp489.tag=
Cyc_Stdio_String_pa; _temp489.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp423);{ void* _temp487=( void*)& _temp489; void* _temp485[ 1u]={ _temp487};
struct _tagged_arr _temp486={( void*) _temp485,( void*) _temp485,( void*)(
_temp485 + 1u)}; _temp484( _temp488, _temp486);}})); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp490=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp490[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp491; _temp491.tag= Cyc_Absyn_Int_c;
_temp491.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp491.f2= 0; _temp491;});
_temp490;}); _LL437:;} goto _LL390; _LL406: return Cyc_Evexp_eval_const_exp((
struct Cyc_Absyn_Exp*) _check_null((( struct Cyc_Absyn_Enumfield*) _check_null(
_temp425))->tag)); _LL408: Cyc_Evexp_exp_err( e->loc, _tag_arr("bad static expression",
sizeof( unsigned char), 22u)); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp492=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp492[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp493; _temp493.tag= Cyc_Absyn_Int_c;
_temp493.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp493.f2= 0; _temp493;});
_temp492;}); _LL390:;}( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp494=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp494[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp495; _temp495.tag= Cyc_Absyn_Const_e;
_temp495.f1=( void*) cn; _temp495;}); _temp494;}))); return cn;}