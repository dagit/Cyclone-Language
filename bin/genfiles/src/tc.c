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
Cyc_List_List* tl; } ; extern int Cyc_List_length( struct Cyc_List_List* x);
extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_List_mem(
int(* compare)( void*, void*), struct Cyc_List_List* l, void* x); extern struct
Cyc_List_List* Cyc_List_filter_c( int(* f)( void*, void*), void* env, struct Cyc_List_List*
l); struct Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[ 19u];
extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
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
int tag; float* f1; } ; extern int Cyc_String_strcmp( struct _tagged_arr s1,
struct _tagged_arr s2); extern int Cyc_String_strptrcmp( struct _tagged_arr* s1,
struct _tagged_arr* s2); extern int Cyc_String_zstrptrcmp( struct _tagged_arr*,
struct _tagged_arr*); struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file;
struct _tagged_arr line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static
const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct
Cyc_Position_Error{ struct _tagged_arr source; struct Cyc_Position_Segment* seg;
void* kind; struct _tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[
14u]; struct _tuple0{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar;
struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref; struct Cyc_Absyn_PtrInfo;
struct Cyc_Absyn_VarargInfo; struct Cyc_Absyn_FnInfo; struct Cyc_Absyn_TunionInfo;
struct Cyc_Absyn_TunionFieldInfo; struct Cyc_Absyn_VarargCallInfo; struct Cyc_Absyn_Exp;
struct Cyc_Absyn_Stmt; struct Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause;
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
x); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct _tagged_arr Cyc_Absyn_attribute2string( void*); extern int Cyc_Absyn_fntype_att(
void* a); struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc; struct
Cyc_Absynpp_Params{ int expand_typedefs: 1; int qvar_to_Cids: 1; int
add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int rewrite_temp_tvars: 1;
int print_all_tvars: 1; int print_all_kinds: 1; int print_using_stmts: 1; int
print_externC_stmts: 1; int print_full_evars: 1; int use_curr_namespace: 1;
struct Cyc_List_List* curr_namespace; } ; extern void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params* fs); extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
extern struct _tagged_arr Cyc_Absynpp_typ2string( void*); extern struct
_tagged_arr Cyc_Absynpp_qvar2string( struct _tuple0*); struct Cyc_Set_Set;
extern struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*, void*)); extern
struct Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt); extern
int Cyc_Set_member( struct Cyc_Set_Set* s, void* elt); extern unsigned char Cyc_Set_Absent[
11u]; struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u]; extern
unsigned char Cyc_Dict_Absent[ 11u]; extern int Cyc_Dict_member( struct Cyc_Dict_Dict*
d, void* key); extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict*
d, void* key, void* data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d,
void* key); extern struct Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict*
d, void* key); extern void Cyc_Dict_iter( void(* f)( void*, void*), struct Cyc_Dict_Dict*
d); extern struct Cyc_Dict_Dict* Cyc_Dict_map( void*(* f)( void*), struct Cyc_Dict_Dict*
d); extern struct Cyc_Dict_Dict* Cyc_Dict_filter_c( int(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_Dict* d); static const int Cyc_Tcenv_VarRes= 0;
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
ae; struct Cyc_Core_Opt* le; } ; extern struct Cyc_Tcenv_Genv* Cyc_Tcenv_empty_genv();
extern struct Cyc_Tcenv_Fenv* Cyc_Tcenv_new_fenv( struct Cyc_Position_Segment*,
struct Cyc_Absyn_Fndecl*); extern struct Cyc_List_List* Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tagged_arr*,
struct Cyc_List_List*); extern struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple0*); extern
int Cyc_Tcenv_all_labels_resolved( struct Cyc_Tcenv_Tenv*); extern unsigned char
Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos( struct _tagged_arr);
extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct _tagged_arr);
extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*, struct _tagged_arr);
extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_is_function_type(
void* t); extern void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*); extern
void Cyc_Tcutil_check_valid_toplevel_type( struct Cyc_Position_Segment*, struct
Cyc_Tcenv_Tenv*, void*); extern void Cyc_Tcutil_check_fndecl_valid_type( struct
Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Fndecl*); extern
void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, void*); extern void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*); extern int Cyc_Tcutil_bits_only( void* t); extern int
Cyc_Tcutil_is_const_exp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e);
extern int Cyc_Tcutil_supports_default( void*); extern void* Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*); extern void Cyc_Tcstmt_tcStmt(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, int new_block); extern
unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); extern
void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int var_default_init, struct Cyc_List_List*
ds); extern struct Cyc_List_List* Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv* te,
struct Cyc_List_List*); extern unsigned char Cyc_Tcdecl_Incompatible[ 17u];
struct Cyc_Tcdecl_Xtunionfielddecl{ struct Cyc_Absyn_Tuniondecl* base; struct
Cyc_Absyn_Tunionfield* field; } ; extern struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_structdecl(
struct Cyc_Absyn_Structdecl* d0, struct Cyc_Absyn_Structdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Uniondecl* Cyc_Tcdecl_merge_uniondecl(
struct Cyc_Absyn_Uniondecl* d0, struct Cyc_Absyn_Uniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_merge_tuniondecl(
struct Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Enumdecl* Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl* d0, struct Cyc_Absyn_Enumdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern void* Cyc_Tcdecl_merge_binding( void* d0,
void* d1, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg); extern
struct Cyc_List_List* Cyc_Tcdecl_sort_xtunion_fields( struct Cyc_List_List* f,
int* res, struct _tagged_arr* v, struct Cyc_Position_Segment* loc, struct
_tagged_arr* msg); static unsigned char _temp0[ 1u]=""; static struct
_tagged_arr Cyc_Tc_tc_msg_c={ _temp0, _temp0, _temp0 + 1u}; static struct
_tagged_arr* Cyc_Tc_tc_msg=( struct _tagged_arr*)& Cyc_Tc_tc_msg_c; static
struct Cyc_List_List* Cyc_Tc_transfer_fn_type_atts( void* t, struct Cyc_List_List*
atts){ void* _temp1= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp7;
struct Cyc_List_List* _temp9; struct Cyc_List_List** _temp11; _LL3: if((
unsigned int) _temp1 > 4u?*(( int*) _temp1) == Cyc_Absyn_FnType: 0){ _LL8:
_temp7=(( struct Cyc_Absyn_FnType_struct*) _temp1)->f1; _LL10: _temp9= _temp7.attributes;
_temp11=&((( struct Cyc_Absyn_FnType_struct*) _temp1)->f1).attributes; goto _LL4;}
else{ goto _LL5;} _LL5: goto _LL6; _LL4: { struct Cyc_List_List* _temp12= 0;
for( 0; atts != 0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ if(
Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*) _check_null( atts))->hd)){*
_temp11=({ struct Cyc_List_List* _temp13=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp13->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); _temp13->tl=* _temp11; _temp13;});} else{ _temp12=({
struct Cyc_List_List* _temp14=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp14->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); _temp14->tl= _temp12; _temp14;});}} return _temp12;}
_LL6: return(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("transfer_fn_type_atts", sizeof( unsigned char), 22u)); _LL2:;} struct
_tuple3{ void* f1; int f2; } ; static void Cyc_Tc_tcVardecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Vardecl*
vd, int check_var_init){ struct _tagged_arr* v=(* vd->name).f2; void* t=( void*)
vd->type; void* sc=( void*) vd->sc; struct Cyc_List_List* atts= vd->attributes;{
void* _temp15=(* vd->name).f1; struct Cyc_List_List* _temp23; struct Cyc_List_List*
_temp25; _LL17: if(( unsigned int) _temp15 > 1u?*(( int*) _temp15) == Cyc_Absyn_Rel_n:
0){ _LL24: _temp23=(( struct Cyc_Absyn_Rel_n_struct*) _temp15)->f1; if( _temp23
== 0){ goto _LL18;} else{ goto _LL19;}} else{ goto _LL19;} _LL19: if((
unsigned int) _temp15 > 1u?*(( int*) _temp15) == Cyc_Absyn_Abs_n: 0){ _LL26:
_temp25=(( struct Cyc_Absyn_Abs_n_struct*) _temp15)->f1; if( _temp25 == 0){ goto
_LL20;} else{ goto _LL21;}} else{ goto _LL21;} _LL21: goto _LL22; _LL18: goto
_LL16; _LL20: goto _LL16; _LL22: Cyc_Tcutil_terr( loc, _tag_arr("qualified declarations are not implemented",
sizeof( unsigned char), 43u)); return; _LL16:;}(* vd->name).f1=( void*)({ struct
Cyc_Absyn_Abs_n_struct* _temp27=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp27[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp28; _temp28.tag= Cyc_Absyn_Abs_n; _temp28.f1= te->ns; _temp28;}); _temp27;});{
void* _temp29= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp* _temp36; struct
Cyc_Absyn_Tqual _temp38; void* _temp40; _LL31: if(( unsigned int) _temp29 > 4u?*((
int*) _temp29) == Cyc_Absyn_ArrayType: 0){ _LL41: _temp40=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp29)->f1; goto _LL39; _LL39: _temp38=(( struct Cyc_Absyn_ArrayType_struct*)
_temp29)->f2; goto _LL37; _LL37: _temp36=(( struct Cyc_Absyn_ArrayType_struct*)
_temp29)->f3; if( _temp36 == 0){ goto _LL35;} else{ goto _LL33;}} else{ goto
_LL33;} _LL35: if( vd->initializer != 0){ goto _LL32;} else{ goto _LL33;} _LL33:
goto _LL34; _LL32:{ void* _temp42=( void*)(( struct Cyc_Absyn_Exp*) _check_null(
vd->initializer))->r; void* _temp52; struct _tagged_arr _temp54; struct Cyc_List_List*
_temp56; struct Cyc_List_List* _temp58; _LL44: if(*(( int*) _temp42) == Cyc_Absyn_Const_e){
_LL53: _temp52=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp42)->f1; if((
unsigned int) _temp52 > 1u?*(( int*) _temp52) == Cyc_Absyn_String_c: 0){ _LL55:
_temp54=(( struct Cyc_Absyn_String_c_struct*) _temp52)->f1; goto _LL45;} else{
goto _LL46;}} else{ goto _LL46;} _LL46: if(*(( int*) _temp42) == Cyc_Absyn_UnresolvedMem_e){
_LL57: _temp56=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp42)->f2; goto
_LL47;} else{ goto _LL48;} _LL48: if(*(( int*) _temp42) == Cyc_Absyn_Array_e){
_LL59: _temp58=(( struct Cyc_Absyn_Array_e_struct*) _temp42)->f1; goto _LL49;}
else{ goto _LL50;} _LL50: goto _LL51; _LL45: t=( void*)( vd->type=( void*)((
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp60=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp60[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp61; _temp61.tag= Cyc_Absyn_ArrayType; _temp61.f1=(
void*) _temp40; _temp61.f2= _temp38; _temp61.f3=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
_get_arr_size( _temp54, sizeof( unsigned char)), 0); _temp61;}); _temp60;})));
goto _LL43; _LL47: _temp58= _temp56; goto _LL49; _LL49: t=( void*)( vd->type=(
void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp62=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp62[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp63; _temp63.tag= Cyc_Absyn_ArrayType; _temp63.f1=(
void*) _temp40; _temp63.f2= _temp38; _temp63.f3=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp((
unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp58), 0);
_temp63;}); _temp62;}))); goto _LL43; _LL51: goto _LL43; _LL43:;} goto _LL30;
_LL34: goto _LL30; _LL30:;} Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);
if( Cyc_Tcutil_is_function_type( t)){ atts= Cyc_Tc_transfer_fn_type_atts( t,
atts);} if( sc ==( void*) Cyc_Absyn_Extern? 1: sc ==( void*) Cyc_Absyn_ExternC){
if( vd->initializer != 0){ Cyc_Tcutil_terr( loc, _tag_arr("extern declaration should not have initializer",
sizeof( unsigned char), 47u));}} else{ if( ! Cyc_Tcutil_is_function_type( t)){
for( 0; atts != 0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ void*
_temp64=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd; _LL66: if((
unsigned int) _temp64 > 16u?*(( int*) _temp64) == Cyc_Absyn_Aligned_att: 0){
goto _LL67;} else{ goto _LL68;} _LL68: if(( unsigned int) _temp64 > 16u?*(( int*)
_temp64) == Cyc_Absyn_Section_att: 0){ goto _LL69;} else{ goto _LL70;} _LL70:
if( _temp64 ==( void*) Cyc_Absyn_Nocommon_att){ goto _LL71;} else{ goto _LL72;}
_LL72: if( _temp64 ==( void*) Cyc_Absyn_Shared_att){ goto _LL73;} else{ goto
_LL74;} _LL74: if( _temp64 ==( void*) Cyc_Absyn_Unused_att){ goto _LL75;} else{
goto _LL76;} _LL76: if( _temp64 ==( void*) Cyc_Absyn_Weak_att){ goto _LL77;}
else{ goto _LL78;} _LL78: if( _temp64 ==( void*) Cyc_Absyn_Dllimport_att){ goto
_LL79;} else{ goto _LL80;} _LL80: if( _temp64 ==( void*) Cyc_Absyn_Dllexport_att){
goto _LL81;} else{ goto _LL82;} _LL82: goto _LL83; _LL67: continue; _LL69:
continue; _LL71: continue; _LL73: continue; _LL75: continue; _LL77: continue;
_LL79: continue; _LL81: continue; _LL83: Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp84)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp89= _tag_arr("bad attribute %s for variable %s",
sizeof( unsigned char), 33u); struct Cyc_Stdio_String_pa_struct _temp91; _temp91.tag=
Cyc_Stdio_String_pa; _temp91.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd);{ void* _temp87=( void*)&
_temp91; struct Cyc_Stdio_String_pa_struct _temp90; _temp90.tag= Cyc_Stdio_String_pa;
_temp90.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( vd->name);{ void*
_temp88=( void*)& _temp90; void* _temp85[ 2u]={ _temp87, _temp88}; struct
_tagged_arr _temp86={( void*) _temp85,( void*) _temp85,( void*)( _temp85 + 2u)};
_temp84( _temp89, _temp86);}}})); goto _LL65; _LL65:;} if( vd->initializer == 0){
if( check_var_init? ! Cyc_Tcutil_supports_default( t): 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp92)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp97= _tag_arr("initializer required for variable %s of type %s",
sizeof( unsigned char), 48u); struct Cyc_Stdio_String_pa_struct _temp99; _temp99.tag=
Cyc_Stdio_String_pa; _temp99.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
vd->name);{ void* _temp95=( void*)& _temp99; struct Cyc_Stdio_String_pa_struct
_temp98; _temp98.tag= Cyc_Stdio_String_pa; _temp98.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp96=( void*)& _temp98; void* _temp93[ 2u]={ _temp95, _temp96};
struct _tagged_arr _temp94={( void*) _temp93,( void*) _temp93,( void*)( _temp93
+ 2u)}; _temp92( _temp97, _temp94);}}}));}} else{ struct Cyc_Absyn_Exp* _temp100=(
struct Cyc_Absyn_Exp*) _check_null( vd->initializer); void* _temp101= Cyc_Tcexp_tcExpInitializer(
te,( void**)& t, _temp100); if( ! Cyc_Tcutil_coerce_assign( te, _temp100, t)){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp102)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp108= _tag_arr("%s is declared with type %s but initialized with type %s",
sizeof( unsigned char), 57u); struct Cyc_Stdio_String_pa_struct _temp111;
_temp111.tag= Cyc_Stdio_String_pa; _temp111.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
vd->name);{ void* _temp105=( void*)& _temp111; struct Cyc_Stdio_String_pa_struct
_temp110; _temp110.tag= Cyc_Stdio_String_pa; _temp110.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp106=( void*)& _temp110; struct Cyc_Stdio_String_pa_struct
_temp109; _temp109.tag= Cyc_Stdio_String_pa; _temp109.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( _temp101);{ void* _temp107=( void*)& _temp109; void*
_temp103[ 3u]={ _temp105, _temp106, _temp107}; struct _tagged_arr _temp104={(
void*) _temp103,( void*) _temp103,( void*)( _temp103 + 3u)}; _temp102( _temp108,
_temp104);}}}}));} if( ! Cyc_Tcutil_is_const_exp( te, _temp100)){ Cyc_Tcutil_terr(
loc, _tag_arr("initializer is not a constant expression", sizeof( unsigned char),
41u));}}} else{ for( 0; atts != 0; atts=(( struct Cyc_List_List*) _check_null(
atts))->tl){ void* _temp112=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd;
_LL114: if(( unsigned int) _temp112 > 16u?*(( int*) _temp112) == Cyc_Absyn_Regparm_att:
0){ goto _LL115;} else{ goto _LL116;} _LL116: if( _temp112 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL117;} else{ goto _LL118;} _LL118: if( _temp112 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL119;} else{ goto _LL120;} _LL120: if( _temp112 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL121;} else{ goto _LL122;} _LL122: if( _temp112 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL123;} else{ goto _LL124;} _LL124: if(( unsigned int) _temp112 > 16u?*((
int*) _temp112) == Cyc_Absyn_Format_att: 0){ goto _LL125;} else{ goto _LL126;}
_LL126: if( _temp112 ==( void*) Cyc_Absyn_Const_att){ goto _LL127;} else{ goto
_LL128;} _LL128: if(( unsigned int) _temp112 > 16u?*(( int*) _temp112) == Cyc_Absyn_Aligned_att:
0){ goto _LL129;} else{ goto _LL130;} _LL130: if( _temp112 ==( void*) Cyc_Absyn_Packed_att){
goto _LL131;} else{ goto _LL132;} _LL132: goto _LL133; _LL115: goto _LL117;
_LL117: goto _LL119; _LL119: goto _LL121; _LL121: goto _LL123; _LL123: goto
_LL125; _LL125: goto _LL127; _LL127:(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("tcVardecl: fn type attributes in function var decl", sizeof(
unsigned char), 51u)); goto _LL113; _LL129: goto _LL131; _LL131: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp134)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp138=
_tag_arr("bad attribute %s in function declaration", sizeof( unsigned char), 41u);
struct Cyc_Stdio_String_pa_struct _temp139; _temp139.tag= Cyc_Stdio_String_pa;
_temp139.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*)(( struct
Cyc_List_List*) _check_null( atts))->hd);{ void* _temp137=( void*)& _temp139;
void* _temp135[ 1u]={ _temp137}; struct _tagged_arr _temp136={( void*) _temp135,(
void*) _temp135,( void*)( _temp135 + 1u)}; _temp134( _temp138, _temp136);}}));
goto _LL113; _LL133: continue; _LL113:;}}}{ struct _handler_cons _temp140;
_push_handler(& _temp140);{ int _temp142= 0; if( setjmp( _temp140.handler)){
_temp142= 1;} if( ! _temp142){{ struct _tuple3* _temp143=(( struct _tuple3*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->ordinaries,
v); void* _temp144=(* _temp143).f1; void* _temp154; _LL146: if(*(( int*)
_temp144) == Cyc_Tcenv_VarRes){ _LL155: _temp154=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp144)->f1; goto _LL147;} else{ goto _LL148;} _LL148: if(*(( int*) _temp144)
== Cyc_Tcenv_StructRes){ goto _LL149;} else{ goto _LL150;} _LL150: if(*(( int*)
_temp144) == Cyc_Tcenv_TunionRes){ goto _LL151;} else{ goto _LL152;} _LL152: if(*((
int*) _temp144) == Cyc_Tcenv_EnumRes){ goto _LL153;} else{ goto _LL145;} _LL147: {
struct Cyc_Absyn_Global_b_struct* _temp156=({ struct Cyc_Absyn_Global_b_struct*
_temp161=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp161[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp162; _temp162.tag= Cyc_Absyn_Global_b;
_temp162.f1= vd; _temp162;}); _temp161;}); void* _temp157= Cyc_Tcdecl_merge_binding(
_temp154,( void*) _temp156, loc, Cyc_Tc_tc_msg); if( _temp157 ==( void*) Cyc_Absyn_Unresolved_b){
goto _LL145;} if( _temp157 == _temp154?(* _temp143).f2: 0){ goto _LL145;} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple3*
_temp158=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp158->f1=(
void*)({ struct Cyc_Tcenv_VarRes_struct* _temp159=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp159[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp160; _temp160.tag= Cyc_Tcenv_VarRes; _temp160.f1=( void*) _temp157;
_temp160;}); _temp159;}); _temp158->f2= 1; _temp158;})); goto _LL145;} _LL149:
Cyc_Tcutil_warn( loc, _tag_arr("variable declaration shadows previous struct declaration",
sizeof( unsigned char), 57u)); goto _LL145; _LL151: Cyc_Tcutil_warn( loc,
_tag_arr("variable declaration shadows previous [x]tunion constructor", sizeof(
unsigned char), 60u)); goto _LL145; _LL153: Cyc_Tcutil_warn( loc, _tag_arr("variable declaration shadows previous enum tag",
sizeof( unsigned char), 47u)); goto _LL145; _LL145:;}; _pop_handler();} else{
void* _temp141=( void*) _exn_thrown; void* _temp164= _temp141; _LL166: if(
_temp164 == Cyc_Dict_Absent){ goto _LL167;} else{ goto _LL168;} _LL168: goto
_LL169; _LL167: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,
v,({ struct _tuple3* _temp170=( struct _tuple3*) GC_malloc( sizeof( struct
_tuple3)); _temp170->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp171=(
struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp171[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp172; _temp172.tag= Cyc_Tcenv_VarRes;
_temp172.f1=( void*)(( void*)({ struct Cyc_Absyn_Global_b_struct* _temp173=(
struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp173[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp174; _temp174.tag= Cyc_Absyn_Global_b;
_temp174.f1= vd; _temp174;}); _temp173;})); _temp172;}); _temp171;}); _temp170->f2=
0; _temp170;})); goto _LL165; _LL169:( void) _throw( _temp164); _LL165:;}}}}
static void Cyc_Tc_tcFndecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl* fd){ struct
_tagged_arr* v=(* fd->name).f2; void* sc=( void*) fd->sc;{ void* _temp175=(* fd->name).f1;
struct Cyc_List_List* _temp183; struct Cyc_List_List* _temp185; _LL177: if((
unsigned int) _temp175 > 1u?*(( int*) _temp175) == Cyc_Absyn_Rel_n: 0){ _LL184:
_temp183=(( struct Cyc_Absyn_Rel_n_struct*) _temp175)->f1; if( _temp183 == 0){
goto _LL178;} else{ goto _LL179;}} else{ goto _LL179;} _LL179: if(( unsigned int)
_temp175 > 1u?*(( int*) _temp175) == Cyc_Absyn_Abs_n: 0){ _LL186: _temp185=((
struct Cyc_Absyn_Abs_n_struct*) _temp175)->f1; goto _LL180;} else{ goto _LL181;}
_LL181: goto _LL182; _LL178: goto _LL176; _LL180:( int) _throw(( void*)({ struct
Cyc_Core_Impossible_struct* _temp187=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp187[ 0]=({ struct
Cyc_Core_Impossible_struct _temp188; _temp188.tag= Cyc_Core_Impossible; _temp188.f1=
_tag_arr("tc: Abs_n in tcFndecl", sizeof( unsigned char), 22u); _temp188;});
_temp187;})); _LL182: Cyc_Tcutil_terr( loc, _tag_arr("qualified declarations are not implemented",
sizeof( unsigned char), 43u)); return; _LL176:;}(* fd->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp189=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp189[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp190; _temp190.tag= Cyc_Absyn_Abs_n; _temp190.f1= te->ns; _temp190;});
_temp189;}); Cyc_Tcutil_check_fndecl_valid_type( loc, te, fd);{ void* t= Cyc_Tcutil_fndecl2typ(
fd); fd->attributes= Cyc_Tc_transfer_fn_type_atts( t, fd->attributes);{ struct
Cyc_List_List* atts= fd->attributes; for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp191=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; _LL193: if( _temp191 ==( void*) Cyc_Absyn_Packed_att){
goto _LL194;} else{ goto _LL195;} _LL195: if(( unsigned int) _temp191 > 16u?*((
int*) _temp191) == Cyc_Absyn_Aligned_att: 0){ goto _LL196;} else{ goto _LL197;}
_LL197: goto _LL198; _LL194: goto _LL196; _LL196: Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp199)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp203= _tag_arr("bad attribute %s for function",
sizeof( unsigned char), 30u); struct Cyc_Stdio_String_pa_struct _temp204;
_temp204.tag= Cyc_Stdio_String_pa; _temp204.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd);{ void* _temp202=( void*)&
_temp204; void* _temp200[ 1u]={ _temp202}; struct _tagged_arr _temp201={( void*)
_temp200,( void*) _temp200,( void*)( _temp200 + 1u)}; _temp199( _temp203,
_temp201);}})); goto _LL192; _LL198: goto _LL192; _LL192:;}}{ struct
_handler_cons _temp205; _push_handler(& _temp205);{ int _temp207= 0; if( setjmp(
_temp205.handler)){ _temp207= 1;} if( ! _temp207){{ struct _tuple3* _temp208=((
struct _tuple3*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
ge->ordinaries, v); void* _temp209=(* _temp208).f1; void* _temp219; _LL211: if(*((
int*) _temp209) == Cyc_Tcenv_VarRes){ _LL220: _temp219=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp209)->f1; goto _LL212;} else{ goto _LL213;} _LL213: if(*(( int*) _temp209)
== Cyc_Tcenv_StructRes){ goto _LL214;} else{ goto _LL215;} _LL215: if(*(( int*)
_temp209) == Cyc_Tcenv_TunionRes){ goto _LL216;} else{ goto _LL217;} _LL217: if(*((
int*) _temp209) == Cyc_Tcenv_EnumRes){ goto _LL218;} else{ goto _LL210;} _LL212: {
struct Cyc_Absyn_Funname_b_struct* _temp221=({ struct Cyc_Absyn_Funname_b_struct*
_temp226=( struct Cyc_Absyn_Funname_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct));
_temp226[ 0]=({ struct Cyc_Absyn_Funname_b_struct _temp227; _temp227.tag= Cyc_Absyn_Funname_b;
_temp227.f1= fd; _temp227;}); _temp226;}); void* _temp222= Cyc_Tcdecl_merge_binding(
_temp219,( void*) _temp221, loc, Cyc_Tc_tc_msg); if( _temp222 ==( void*) Cyc_Absyn_Unresolved_b){
goto _LL210;} if( _temp222 == _temp219?(* _temp208).f2: 0){ goto _LL210;} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple3*
_temp223=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp223->f1=(
void*)({ struct Cyc_Tcenv_VarRes_struct* _temp224=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp224[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp225; _temp225.tag= Cyc_Tcenv_VarRes; _temp225.f1=( void*) _temp222;
_temp225;}); _temp224;}); _temp223->f2= 1; _temp223;})); goto _LL210;} _LL214:
Cyc_Tcutil_warn( loc, _tag_arr("function declaration shadows previous struct declaration",
sizeof( unsigned char), 57u)); goto _LL210; _LL216: Cyc_Tcutil_warn( loc,
_tag_arr("function declaration shadows previous [x]tunion constructor", sizeof(
unsigned char), 60u)); goto _LL210; _LL218: Cyc_Tcutil_warn( loc, _tag_arr("function declaration shadows previous enum tag",
sizeof( unsigned char), 47u)); goto _LL210; _LL210:;}; _pop_handler();} else{
void* _temp206=( void*) _exn_thrown; void* _temp229= _temp206; _LL231: if(
_temp229 == Cyc_Dict_Absent){ goto _LL232;} else{ goto _LL233;} _LL233: goto
_LL234; _LL232: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,
v,({ struct _tuple3* _temp235=( struct _tuple3*) GC_malloc( sizeof( struct
_tuple3)); _temp235->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp236=(
struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp236[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp237; _temp237.tag= Cyc_Tcenv_VarRes;
_temp237.f1=( void*)(( void*)({ struct Cyc_Absyn_Funname_b_struct* _temp238=(
struct Cyc_Absyn_Funname_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct));
_temp238[ 0]=({ struct Cyc_Absyn_Funname_b_struct _temp239; _temp239.tag= Cyc_Absyn_Funname_b;
_temp239.f1= fd; _temp239;}); _temp238;})); _temp237;}); _temp236;}); _temp235->f2=
0; _temp235;})); goto _LL230; _LL234:( void) _throw( _temp229); _LL230:;}}} te->le=({
struct Cyc_Core_Opt* _temp240=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp240->v=( void*)(( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp241=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp241[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp242; _temp242.tag= Cyc_Tcenv_Outermost;
_temp242.f1=( void*) Cyc_Tcenv_new_fenv( loc, fd); _temp242;}); _temp241;}));
_temp240;}); Cyc_Tcstmt_tcStmt( te, fd->body, 0); if( ! Cyc_Tcenv_all_labels_resolved(
te)){ Cyc_Tcutil_terr( loc, _tag_arr("function has goto statements to undefined labels",
sizeof( unsigned char), 49u));} te->le= 0;}} static void Cyc_Tc_tcTypedefdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Typedefdecl* td){ struct _tagged_arr* v=(* td->name).f2;{
void* _temp243=(* td->name).f1; struct Cyc_List_List* _temp251; struct Cyc_List_List*
_temp253; _LL245: if(( unsigned int) _temp243 > 1u?*(( int*) _temp243) == Cyc_Absyn_Rel_n:
0){ _LL252: _temp251=(( struct Cyc_Absyn_Rel_n_struct*) _temp243)->f1; if(
_temp251 == 0){ goto _LL246;} else{ goto _LL247;}} else{ goto _LL247;} _LL247:
if(( unsigned int) _temp243 > 1u?*(( int*) _temp243) == Cyc_Absyn_Abs_n: 0){
_LL254: _temp253=(( struct Cyc_Absyn_Abs_n_struct*) _temp243)->f1; if( _temp253
== 0){ goto _LL248;} else{ goto _LL249;}} else{ goto _LL249;} _LL249: goto
_LL250; _LL246: goto _LL244; _LL248: goto _LL244; _LL250: Cyc_Tcutil_terr( loc,
_tag_arr("qualified declarations are not implemented", sizeof( unsigned char),
43u)); return; _LL244:;} if((( int(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_member)( ge->typedefs, v)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp255)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp259= _tag_arr("redeclaration of typedef %s",
sizeof( unsigned char), 28u); struct Cyc_Stdio_String_pa_struct _temp260;
_temp260.tag= Cyc_Stdio_String_pa; _temp260.f1=( struct _tagged_arr)* v;{ void*
_temp258=( void*)& _temp260; void* _temp256[ 1u]={ _temp258}; struct _tagged_arr
_temp257={( void*) _temp256,( void*) _temp256,( void*)( _temp256 + 1u)};
_temp255( _temp259, _temp257);}})); return;}(* td->name).f1=( void*)({ struct
Cyc_Absyn_Abs_n_struct* _temp261=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp261[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp262; _temp262.tag= Cyc_Absyn_Abs_n; _temp262.f1= te->ns; _temp262;});
_temp261;}); Cyc_Tcutil_check_unique_tvars( loc, td->tvs); Cyc_Tcutil_add_tvar_identities(
td->tvs); Cyc_Tcutil_check_type( loc, te, td->tvs,( void*) Cyc_Absyn_AnyKind,(
void*) td->defn); ge->typedefs=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Typedefdecl* data)) Cyc_Dict_insert)(
ge->typedefs, v, td);} static void Cyc_Tc_tcStructFields( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct
_tagged_arr obj, struct Cyc_List_List* fields, struct Cyc_List_List* tvs){
struct _RegionHandle _temp263= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp263; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0;
struct Cyc_List_List* _temp264= fields; for( 0; _temp264 != 0; _temp264=((
struct Cyc_List_List*) _check_null( _temp264))->tl){ struct Cyc_Absyn_Structfield
_temp267; struct Cyc_List_List* _temp268; struct Cyc_Core_Opt* _temp270; void*
_temp272; struct Cyc_Absyn_Tqual _temp274; struct _tagged_arr* _temp276; struct
Cyc_Absyn_Structfield* _temp265=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp264))->hd; _temp267=* _temp265; _LL277: _temp276= _temp267.name;
goto _LL275; _LL275: _temp274= _temp267.tq; goto _LL273; _LL273: _temp272=( void*)
_temp267.type; goto _LL271; _LL271: _temp270= _temp267.width; goto _LL269;
_LL269: _temp268= _temp267.attributes; goto _LL266; _LL266: if((( int(*)( int(*
compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l,
struct _tagged_arr* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,
_temp276)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp278)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp283= _tag_arr("duplicate field %s in %s", sizeof(
unsigned char), 25u); struct Cyc_Stdio_String_pa_struct _temp285; _temp285.tag=
Cyc_Stdio_String_pa; _temp285.f1=( struct _tagged_arr)* _temp276;{ void*
_temp281=( void*)& _temp285; struct Cyc_Stdio_String_pa_struct _temp284;
_temp284.tag= Cyc_Stdio_String_pa; _temp284.f1=( struct _tagged_arr) obj;{ void*
_temp282=( void*)& _temp284; void* _temp279[ 2u]={ _temp281, _temp282}; struct
_tagged_arr _temp280={( void*) _temp279,( void*) _temp279,( void*)( _temp279 + 2u)};
_temp278( _temp283, _temp280);}}}));} if( Cyc_String_strcmp(* _temp276, _tag_arr("",
sizeof( unsigned char), 1u)) != 0){ prev_fields=({ struct Cyc_List_List*
_temp286=( struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp286->hd=( void*) _temp276; _temp286->tl= prev_fields; _temp286;});} Cyc_Tcutil_check_type(
loc, te, tvs,( void*) Cyc_Absyn_MemKind, _temp272); if( _temp270 != 0){
unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp270))->v)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp287)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp291= _tag_arr("bitfield %s does not have constant width",
sizeof( unsigned char), 41u); struct Cyc_Stdio_String_pa_struct _temp292;
_temp292.tag= Cyc_Stdio_String_pa; _temp292.f1=( struct _tagged_arr)* _temp276;{
void* _temp290=( void*)& _temp292; void* _temp288[ 1u]={ _temp290}; struct
_tagged_arr _temp289={( void*) _temp288,( void*) _temp288,( void*)( _temp288 + 1u)};
_temp287( _temp291, _temp289);}}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp270))->v);}{
void* _temp293= Cyc_Tcutil_compress( _temp272); void* _temp299; _LL295: if((
unsigned int) _temp293 > 4u?*(( int*) _temp293) == Cyc_Absyn_IntType: 0){ _LL300:
_temp299=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp293)->f2; goto _LL296;}
else{ goto _LL297;} _LL297: goto _LL298; _LL296:{ void* _temp301= _temp299;
_LL303: if( _temp301 ==( void*) Cyc_Absyn_B1){ goto _LL304;} else{ goto _LL305;}
_LL305: if( _temp301 ==( void*) Cyc_Absyn_B2){ goto _LL306;} else{ goto _LL307;}
_LL307: if( _temp301 ==( void*) Cyc_Absyn_B4){ goto _LL308;} else{ goto _LL309;}
_LL309: if( _temp301 ==( void*) Cyc_Absyn_B8){ goto _LL310;} else{ goto _LL302;}
_LL304: if( w > 8){ Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u));} goto _LL302; _LL306: if( w > 16){ Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u));} goto
_LL302; _LL308: if( w > 32){ Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u));} goto _LL302; _LL310: if( w > 64){ Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u));} goto
_LL302; _LL302:;} goto _LL294; _LL298: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr(* _temp311)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp316= _tag_arr("bitfield %s must have integral type but has type %s",
sizeof( unsigned char), 52u); struct Cyc_Stdio_String_pa_struct _temp318;
_temp318.tag= Cyc_Stdio_String_pa; _temp318.f1=( struct _tagged_arr)* _temp276;{
void* _temp314=( void*)& _temp318; struct Cyc_Stdio_String_pa_struct _temp317;
_temp317.tag= Cyc_Stdio_String_pa; _temp317.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp272);{ void* _temp315=( void*)& _temp317; void* _temp312[ 2u]={ _temp314,
_temp315}; struct _tagged_arr _temp313={( void*) _temp312,( void*) _temp312,(
void*)( _temp312 + 2u)}; _temp311( _temp316, _temp313);}}})); goto _LL294;
_LL294:;}}}}; _pop_region( uprev_rgn);} static void Cyc_Tc_tcStructOrUniondecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct _tagged_arr obj,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl* sd){ struct
_tagged_arr* _temp319=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
sd->name))->v)).f2; if( sd->name == 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp320)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp324= _tag_arr("anonymous %ss are not allowed at top level",
sizeof( unsigned char), 43u); struct Cyc_Stdio_String_pa_struct _temp325;
_temp325.tag= Cyc_Stdio_String_pa; _temp325.f1=( struct _tagged_arr) obj;{ void*
_temp323=( void*)& _temp325; void* _temp321[ 1u]={ _temp323}; struct _tagged_arr
_temp322={( void*) _temp321,( void*) _temp321,( void*)( _temp321 + 1u)};
_temp320( _temp324, _temp322);}})); return;}{ struct Cyc_List_List* atts= sd->attributes;
for( 0; atts != 0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ void*
_temp326=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd; _LL328: if(
_temp326 ==( void*) Cyc_Absyn_Packed_att){ goto _LL329;} else{ goto _LL330;}
_LL330: if(( unsigned int) _temp326 > 16u?*(( int*) _temp326) == Cyc_Absyn_Aligned_att:
0){ goto _LL331;} else{ goto _LL332;} _LL332: goto _LL333; _LL329: continue;
_LL331: continue; _LL333: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp334)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp340= _tag_arr("bad attribute %s in %s %s definition",
sizeof( unsigned char), 37u); struct Cyc_Stdio_String_pa_struct _temp343;
_temp343.tag= Cyc_Stdio_String_pa; _temp343.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd);{ void* _temp337=( void*)&
_temp343; struct Cyc_Stdio_String_pa_struct _temp342; _temp342.tag= Cyc_Stdio_String_pa;
_temp342.f1=( struct _tagged_arr) obj;{ void* _temp338=( void*)& _temp342;
struct Cyc_Stdio_String_pa_struct _temp341; _temp341.tag= Cyc_Stdio_String_pa;
_temp341.f1=( struct _tagged_arr)* _temp319;{ void* _temp339=( void*)& _temp341;
void* _temp335[ 3u]={ _temp337, _temp338, _temp339}; struct _tagged_arr _temp336={(
void*) _temp335,( void*) _temp335,( void*)( _temp335 + 3u)}; _temp334( _temp340,
_temp336);}}}})); goto _LL327; _LL327:;}}{ struct Cyc_List_List* tvs= sd->tvs;
for( 0; tvs != 0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ struct
Cyc_Absyn_Conref* _temp344= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); void* _temp345=( void*)
_temp344->v; void* _temp353; _LL347: if( _temp345 ==( void*) Cyc_Absyn_No_constr){
goto _LL348;} else{ goto _LL349;} _LL349: if(( unsigned int) _temp345 > 1u?*((
int*) _temp345) == Cyc_Absyn_Eq_constr: 0){ _LL354: _temp353=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp345)->f1; if( _temp353 ==( void*) Cyc_Absyn_MemKind){
goto _LL350;} else{ goto _LL351;}} else{ goto _LL351;} _LL351: goto _LL352;
_LL348:( void*)( _temp344->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp355=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp355[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp356; _temp356.tag= Cyc_Absyn_Eq_constr;
_temp356.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp356;}); _temp355;})));
continue; _LL350: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp357)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp363= _tag_arr("%s %s attempts to abstract type variable %s of kind M",
sizeof( unsigned char), 54u); struct Cyc_Stdio_String_pa_struct _temp366;
_temp366.tag= Cyc_Stdio_String_pa; _temp366.f1=( struct _tagged_arr) obj;{ void*
_temp360=( void*)& _temp366; struct Cyc_Stdio_String_pa_struct _temp365;
_temp365.tag= Cyc_Stdio_String_pa; _temp365.f1=( struct _tagged_arr)* _temp319;{
void* _temp361=( void*)& _temp365; struct Cyc_Stdio_String_pa_struct _temp364;
_temp364.tag= Cyc_Stdio_String_pa; _temp364.f1=( struct _tagged_arr)*(( struct
Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->name;{ void*
_temp362=( void*)& _temp364; void* _temp358[ 3u]={ _temp360, _temp361, _temp362};
struct _tagged_arr _temp359={( void*) _temp358,( void*) _temp358,( void*)(
_temp358 + 3u)}; _temp357( _temp363, _temp359);}}}})); continue; _LL352:
continue; _LL346:;}}{ void* _temp367=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v)).f1; struct Cyc_List_List* _temp375; struct Cyc_List_List*
_temp377; _LL369: if(( unsigned int) _temp367 > 1u?*(( int*) _temp367) == Cyc_Absyn_Rel_n:
0){ _LL376: _temp375=(( struct Cyc_Absyn_Rel_n_struct*) _temp367)->f1; if(
_temp375 == 0){ goto _LL370;} else{ goto _LL371;}} else{ goto _LL371;} _LL371:
if(( unsigned int) _temp367 > 1u?*(( int*) _temp367) == Cyc_Absyn_Abs_n: 0){
_LL378: _temp377=(( struct Cyc_Absyn_Abs_n_struct*) _temp367)->f1; if( _temp377
== 0){ goto _LL372;} else{ goto _LL373;}} else{ goto _LL373;} _LL373: goto
_LL374; _LL370: goto _LL368; _LL372: goto _LL368; _LL374: Cyc_Tcutil_terr( loc,
_tag_arr("qualified declarations are not implemented", sizeof( unsigned char),
43u)); return; _LL368:;}(*(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v)).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp379=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp379[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp380; _temp380.tag= Cyc_Absyn_Abs_n;
_temp380.f1= te->ns; _temp380;}); _temp379;}); Cyc_Tcutil_check_unique_tvars(
loc, sd->tvs); Cyc_Tcutil_add_tvar_identities( sd->tvs);} struct _tuple4{ struct
Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ; static void Cyc_Tc_tcStructdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Structdecl* sd){ struct _tagged_arr* _temp381=(*(( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)).f2; const
unsigned char* _temp382="struct"; Cyc_Tc_tcStructOrUniondecl( te, ge, _tag_arr(
_temp382, sizeof( unsigned char), 7u), loc, sd);{ struct Cyc_List_List* _temp383=
sd->tvs;{ struct _tuple4 _temp385=({ struct _tuple4 _temp384; _temp384.f1= sd->fields;
_temp384.f2=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup_opt)( ge->structdecls, _temp381); _temp384;});
struct Cyc_Core_Opt* _temp395; struct Cyc_Core_Opt* _temp397; struct Cyc_Core_Opt*
_temp399; struct Cyc_Core_Opt* _temp401; struct Cyc_Core_Opt _temp403; struct
Cyc_List_List* _temp404; struct Cyc_Core_Opt* _temp406; struct Cyc_Core_Opt
_temp408; struct Cyc_Absyn_Structdecl** _temp409; struct Cyc_Core_Opt* _temp411;
struct Cyc_Core_Opt _temp413; struct Cyc_List_List* _temp414; struct Cyc_Core_Opt*
_temp416; struct Cyc_Core_Opt _temp418; struct Cyc_Absyn_Structdecl** _temp419;
struct Cyc_Core_Opt* _temp421; _LL387: _LL398: _temp397= _temp385.f1; if(
_temp397 == 0){ goto _LL396;} else{ goto _LL389;} _LL396: _temp395= _temp385.f2;
if( _temp395 == 0){ goto _LL388;} else{ goto _LL389;} _LL389: _LL402: _temp401=
_temp385.f1; if( _temp401 == 0){ goto _LL391;} else{ _temp403=* _temp401; _LL405:
_temp404=( struct Cyc_List_List*) _temp403.v; goto _LL400;} _LL400: _temp399=
_temp385.f2; if( _temp399 == 0){ goto _LL390;} else{ goto _LL391;} _LL391:
_LL412: _temp411= _temp385.f1; if( _temp411 == 0){ goto _LL393;} else{ _temp413=*
_temp411; _LL415: _temp414=( struct Cyc_List_List*) _temp413.v; goto _LL407;}
_LL407: _temp406= _temp385.f2; if( _temp406 == 0){ goto _LL393;} else{ _temp408=*
_temp406; _LL410: _temp409=( struct Cyc_Absyn_Structdecl**) _temp408.v; goto
_LL392;} _LL393: _LL422: _temp421= _temp385.f1; if( _temp421 == 0){ goto _LL417;}
else{ goto _LL386;} _LL417: _temp416= _temp385.f2; if( _temp416 == 0){ goto
_LL386;} else{ _temp418=* _temp416; _LL420: _temp419=( struct Cyc_Absyn_Structdecl**)
_temp418.v; goto _LL394;} _LL388: ge->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl**
data)) Cyc_Dict_insert)( ge->structdecls, _temp381,({ struct Cyc_Absyn_Structdecl**
_temp423=( struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp423[ 0]= sd; _temp423;})); goto _LL386; _LL390: { struct Cyc_Absyn_Structdecl**
_temp424=({ struct Cyc_Absyn_Structdecl** _temp425=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp425[ 0]=({ struct Cyc_Absyn_Structdecl*
_temp426=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp426->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp426->name= sd->name;
_temp426->tvs= _temp383; _temp426->fields= 0; _temp426->attributes= 0; _temp426;});
_temp425;}); ge->structdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl** data)) Cyc_Dict_insert)(
ge->structdecls, _temp381, _temp424); Cyc_Tc_tcStructFields( te, ge, loc,
_tag_arr( _temp382, sizeof( unsigned char), 7u), _temp404, _temp383);* _temp424=
sd; goto _LL386;} _LL392: { struct Cyc_Absyn_Structdecl* _temp427=* _temp409;*
_temp409=({ struct Cyc_Absyn_Structdecl* _temp428=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp428->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp428->name= sd->name; _temp428->tvs= _temp383; _temp428->fields=
0; _temp428->attributes= 0; _temp428;}); Cyc_Tc_tcStructFields( te, ge, loc,
_tag_arr( _temp382, sizeof( unsigned char), 7u), _temp414, _temp383);* _temp409=
_temp427; _temp419= _temp409; goto _LL394;} _LL394: { struct Cyc_Absyn_Structdecl*
_temp429= Cyc_Tcdecl_merge_structdecl(* _temp419, sd, loc, Cyc_Tc_tc_msg); if(
_temp429 == 0){ return;} else{* _temp419=( struct Cyc_Absyn_Structdecl*)
_check_null( _temp429); sd=( struct Cyc_Absyn_Structdecl*) _check_null( _temp429);
goto _LL386;}} _LL386:;} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,
_temp381,({ struct _tuple3* _temp430=( struct _tuple3*) GC_malloc( sizeof(
struct _tuple3)); _temp430->f1=( void*)({ struct Cyc_Tcenv_StructRes_struct*
_temp431=( struct Cyc_Tcenv_StructRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_StructRes_struct));
_temp431[ 0]=({ struct Cyc_Tcenv_StructRes_struct _temp432; _temp432.tag= Cyc_Tcenv_StructRes;
_temp432.f1= sd; _temp432;}); _temp431;}); _temp430->f2= 1; _temp430;}));}}
static void Cyc_Tc_tcUniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl* ud){ struct
_tagged_arr* _temp433=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
ud->name))->v)).f2; const unsigned char* _temp434="union"; Cyc_Tc_tcStructOrUniondecl(
te, ge, _tag_arr( _temp434, sizeof( unsigned char), 6u), loc,( struct Cyc_Absyn_Structdecl*)
ud);{ struct Cyc_List_List* _temp435= ud->tvs; struct _tuple4 _temp437=({ struct
_tuple4 _temp436; _temp436.f1= ud->fields; _temp436.f2=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)( ge->uniondecls,
_temp433); _temp436;}); struct Cyc_Core_Opt* _temp447; struct Cyc_Core_Opt*
_temp449; struct Cyc_Core_Opt* _temp451; struct Cyc_Core_Opt* _temp453; struct
Cyc_Core_Opt _temp455; struct Cyc_List_List* _temp456; struct Cyc_Core_Opt*
_temp458; struct Cyc_Core_Opt _temp460; struct Cyc_Absyn_Uniondecl** _temp461;
struct Cyc_Core_Opt* _temp463; struct Cyc_Core_Opt _temp465; struct Cyc_List_List*
_temp466; struct Cyc_Core_Opt* _temp468; struct Cyc_Core_Opt _temp470; struct
Cyc_Absyn_Uniondecl** _temp471; struct Cyc_Core_Opt* _temp473; _LL439: _LL450:
_temp449= _temp437.f1; if( _temp449 == 0){ goto _LL448;} else{ goto _LL441;}
_LL448: _temp447= _temp437.f2; if( _temp447 == 0){ goto _LL440;} else{ goto
_LL441;} _LL441: _LL454: _temp453= _temp437.f1; if( _temp453 == 0){ goto _LL443;}
else{ _temp455=* _temp453; _LL457: _temp456=( struct Cyc_List_List*) _temp455.v;
goto _LL452;} _LL452: _temp451= _temp437.f2; if( _temp451 == 0){ goto _LL442;}
else{ goto _LL443;} _LL443: _LL464: _temp463= _temp437.f1; if( _temp463 == 0){
goto _LL445;} else{ _temp465=* _temp463; _LL467: _temp466=( struct Cyc_List_List*)
_temp465.v; goto _LL459;} _LL459: _temp458= _temp437.f2; if( _temp458 == 0){
goto _LL445;} else{ _temp460=* _temp458; _LL462: _temp461=( struct Cyc_Absyn_Uniondecl**)
_temp460.v; goto _LL444;} _LL445: _LL474: _temp473= _temp437.f1; if( _temp473 ==
0){ goto _LL469;} else{ goto _LL438;} _LL469: _temp468= _temp437.f2; if(
_temp468 == 0){ goto _LL438;} else{ _temp470=* _temp468; _LL472: _temp471=(
struct Cyc_Absyn_Uniondecl**) _temp470.v; goto _LL446;} _LL440: ge->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)( ge->uniondecls, _temp433,({
struct Cyc_Absyn_Uniondecl** _temp475=( struct Cyc_Absyn_Uniondecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl*)); _temp475[ 0]= ud; _temp475;})); goto
_LL438; _LL442: { struct Cyc_Absyn_Uniondecl** _temp476=({ struct Cyc_Absyn_Uniondecl**
_temp487=( struct Cyc_Absyn_Uniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl*));
_temp487[ 0]=({ struct Cyc_Absyn_Uniondecl* _temp488=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp488->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp488->name= ud->name; _temp488->tvs= _temp435; _temp488->fields=
0; _temp488->attributes= ud->attributes; _temp488;}); _temp487;}); ge->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)( ge->uniondecls, _temp433,
_temp476); Cyc_Tc_tcStructFields( te, ge, loc, _tag_arr( _temp434, sizeof(
unsigned char), 6u), _temp456, _temp435);{ struct Cyc_List_List* f= _temp456;
for( 0; f != 0; f=(( struct Cyc_List_List*) _check_null( f))->tl){ if( ! Cyc_Tcutil_bits_only((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( f))->hd)->type)){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp477)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp483= _tag_arr("field %s of union %s has type %s which is not `bits-only'",
sizeof( unsigned char), 58u); struct Cyc_Stdio_String_pa_struct _temp486;
_temp486.tag= Cyc_Stdio_String_pa; _temp486.f1=( struct _tagged_arr)*(( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( f))->hd)->name;{
void* _temp480=( void*)& _temp486; struct Cyc_Stdio_String_pa_struct _temp485;
_temp485.tag= Cyc_Stdio_String_pa; _temp485.f1=( struct _tagged_arr)* _temp433;{
void* _temp481=( void*)& _temp485; struct Cyc_Stdio_String_pa_struct _temp484;
_temp484.tag= Cyc_Stdio_String_pa; _temp484.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( f))->hd)->type);{
void* _temp482=( void*)& _temp484; void* _temp478[ 3u]={ _temp480, _temp481,
_temp482}; struct _tagged_arr _temp479={( void*) _temp478,( void*) _temp478,(
void*)( _temp478 + 3u)}; _temp477( _temp483, _temp479);}}}}));}}}* _temp476= ud;
goto _LL438;} _LL444: { struct Cyc_Absyn_Uniondecl* _temp489=* _temp461;*
_temp461=({ struct Cyc_Absyn_Uniondecl* _temp490=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp490->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp490->name= ud->name; _temp490->tvs= _temp435; _temp490->fields=
0; _temp490->attributes= ud->attributes; _temp490;}); Cyc_Tc_tcStructFields( te,
ge, loc, _tag_arr( _temp434, sizeof( unsigned char), 6u), _temp466, _temp435);*
_temp461= _temp489; _temp471= _temp461; goto _LL446;} _LL446: { struct Cyc_Absyn_Uniondecl*
_temp491= Cyc_Tcdecl_merge_uniondecl(* _temp471, ud, loc, Cyc_Tc_tc_msg); if(
_temp491 == 0){ return;} else{* _temp471=( struct Cyc_Absyn_Uniondecl*)
_check_null( _temp491); goto _LL438;}} _LL438:;}} struct _tuple5{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; static struct Cyc_List_List* Cyc_Tc_tcTunionFields( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct
_tagged_arr obj, int is_xtunion, struct _tuple0* name, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs, struct Cyc_Absyn_Tuniondecl* tudres){{ struct
Cyc_List_List* _temp492= fields; for( 0; _temp492 != 0; _temp492=(( struct Cyc_List_List*)
_check_null( _temp492))->tl){ struct Cyc_Absyn_Tunionfield* _temp493=( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp492))->hd;{
struct Cyc_List_List* tvs= _temp493->tvs; for( 0; tvs != 0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){ struct Cyc_Absyn_Tvar* _temp494=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd; struct Cyc_Absyn_Conref* _temp495=
Cyc_Absyn_compress_conref( _temp494->kind); void* _temp496=( void*) _temp495->v;
void* _temp504; _LL498: if( _temp496 ==( void*) Cyc_Absyn_No_constr){ goto
_LL499;} else{ goto _LL500;} _LL500: if(( unsigned int) _temp496 > 1u?*(( int*)
_temp496) == Cyc_Absyn_Eq_constr: 0){ _LL505: _temp504=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp496)->f1; if( _temp504 ==( void*) Cyc_Absyn_MemKind){ goto _LL501;} else{
goto _LL502;}} else{ goto _LL502;} _LL502: goto _LL503; _LL499:( void*)(
_temp495->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp506=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp506[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp507; _temp507.tag= Cyc_Absyn_Eq_constr;
_temp507.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp507;}); _temp506;})));
goto _LL497; _LL501: Cyc_Tcutil_terr( _temp493->loc,( struct _tagged_arr)({
struct _tagged_arr(* _temp508)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp512= _tag_arr("field %s abstracts type variable of kind M",
sizeof( unsigned char), 43u); struct Cyc_Stdio_String_pa_struct _temp513;
_temp513.tag= Cyc_Stdio_String_pa; _temp513.f1=( struct _tagged_arr)*(* _temp493->name).f2;{
void* _temp511=( void*)& _temp513; void* _temp509[ 1u]={ _temp511}; struct
_tagged_arr _temp510={( void*) _temp509,( void*) _temp509,( void*)( _temp509 + 1u)};
_temp508( _temp512, _temp510);}})); goto _LL497; _LL503: goto _LL497; _LL497:;}}{
struct Cyc_List_List* alltvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( tvs, _temp493->tvs); Cyc_Tcutil_check_unique_tvars(
loc, alltvs); Cyc_Tcutil_add_tvar_identities( _temp493->tvs);{ struct Cyc_List_List*
typs= _temp493->typs; for( 0; typs != 0; typs=(( struct Cyc_List_List*)
_check_null( typs))->tl){ Cyc_Tcutil_check_type( _temp493->loc, te, alltvs,(
void*) Cyc_Absyn_MemKind,(*(( struct _tuple5*)(( struct Cyc_List_List*)
_check_null( typs))->hd)).f2);}}{ void* _temp514=(* _temp493->name).f1; struct
Cyc_List_List* _temp524; _LL516: if(( unsigned int) _temp514 > 1u?*(( int*)
_temp514) == Cyc_Absyn_Rel_n: 0){ _LL525: _temp524=(( struct Cyc_Absyn_Rel_n_struct*)
_temp514)->f1; if( _temp524 == 0){ goto _LL517;} else{ goto _LL518;}} else{ goto
_LL518;} _LL518: if(( unsigned int) _temp514 > 1u?*(( int*) _temp514) == Cyc_Absyn_Rel_n:
0){ goto _LL519;} else{ goto _LL520;} _LL520: if(( unsigned int) _temp514 > 1u?*((
int*) _temp514) == Cyc_Absyn_Abs_n: 0){ goto _LL521;} else{ goto _LL522;} _LL522:
if( _temp514 ==( void*) Cyc_Absyn_Loc_n){ goto _LL523;} else{ goto _LL515;}
_LL517: if( is_xtunion){(* _temp493->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp526=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp526[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp527; _temp527.tag= Cyc_Absyn_Abs_n;
_temp527.f1= te->ns; _temp527;}); _temp526;});} else{(* _temp493->name).f1=(*
name).f1;} goto _LL515; _LL519: Cyc_Tcutil_terr( _temp493->loc, _tag_arr("qualified declarations are not allowed",
sizeof( unsigned char), 39u)); goto _LL515; _LL521: goto _LL515; _LL523:( int)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp528=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp528[ 0]=({ struct
Cyc_Core_Impossible_struct _temp529; _temp529.tag= Cyc_Core_Impossible; _temp529.f1=
_tag_arr("tcTunionFields: Loc_n", sizeof( unsigned char), 22u); _temp529;});
_temp528;})); goto _LL515; _LL515:;}}}}{ struct Cyc_List_List* fields2; if(
is_xtunion){ int _temp530= 1; struct Cyc_List_List* _temp531= Cyc_Tcdecl_sort_xtunion_fields(
fields,& _temp530,(* name).f2, loc, Cyc_Tc_tc_msg); if( _temp530){ fields2=
_temp531;} else{ fields2= 0;}} else{ struct _RegionHandle _temp532= _new_region();
struct _RegionHandle* uprev_rgn=& _temp532; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0;{ struct Cyc_List_List* fs= fields; for( 0; fs !=
0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){ struct Cyc_Absyn_Tunionfield*
_temp533=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fs))->hd; if((( int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*),
struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp,
prev_fields,(* _temp533->name).f2)){ Cyc_Tcutil_terr( _temp533->loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp534)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp539= _tag_arr("duplicate field name %s in %s",
sizeof( unsigned char), 30u); struct Cyc_Stdio_String_pa_struct _temp541;
_temp541.tag= Cyc_Stdio_String_pa; _temp541.f1=( struct _tagged_arr)*(* _temp533->name).f2;{
void* _temp537=( void*)& _temp541; struct Cyc_Stdio_String_pa_struct _temp540;
_temp540.tag= Cyc_Stdio_String_pa; _temp540.f1=( struct _tagged_arr) obj;{ void*
_temp538=( void*)& _temp540; void* _temp535[ 2u]={ _temp537, _temp538}; struct
_tagged_arr _temp536={( void*) _temp535,( void*) _temp535,( void*)( _temp535 + 2u)};
_temp534( _temp539, _temp536);}}}));} else{ prev_fields=({ struct Cyc_List_List*
_temp542=( struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp542->hd=( void*)(* _temp533->name).f2; _temp542->tl= prev_fields; _temp542;});}
if(( void*) _temp533->sc !=( void*) Cyc_Absyn_Public){ Cyc_Tcutil_warn( loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp543)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp547= _tag_arr("ignoring scope of field %s",
sizeof( unsigned char), 27u); struct Cyc_Stdio_String_pa_struct _temp548;
_temp548.tag= Cyc_Stdio_String_pa; _temp548.f1=( struct _tagged_arr)*(* _temp533->name).f2;{
void* _temp546=( void*)& _temp548; void* _temp544[ 1u]={ _temp546}; struct
_tagged_arr _temp545={( void*) _temp544,( void*) _temp544,( void*)( _temp544 + 1u)};
_temp543( _temp547, _temp545);}}));( void*)( _temp533->sc=( void*)(( void*) Cyc_Absyn_Public));}}}
fields2= fields;}; _pop_region( uprev_rgn);}{ struct Cyc_List_List* _temp549=
fields; for( 0; _temp549 != 0; _temp549=(( struct Cyc_List_List*) _check_null(
_temp549))->tl){ struct Cyc_Absyn_Tunionfield* _temp550=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp549))->hd; ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries,(* _temp550->name).f2,({ struct _tuple3* _temp551=( struct
_tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp551->f1=( void*)({ struct
Cyc_Tcenv_TunionRes_struct* _temp552=( struct Cyc_Tcenv_TunionRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct)); _temp552[ 0]=({ struct
Cyc_Tcenv_TunionRes_struct _temp553; _temp553.tag= Cyc_Tcenv_TunionRes; _temp553.f1=
tudres; _temp553.f2= _temp550; _temp553;}); _temp552;}); _temp551->f2= 1;
_temp551;}));}} return fields2;}} static void Cyc_Tc_tcTuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl*
tud){ struct _tagged_arr* v=(* tud->name).f2; struct _tagged_arr obj; if( tud->is_xtunion){
obj= _tag_arr("xtunion", sizeof( unsigned char), 8u);} else{ obj= _tag_arr("tunion",
sizeof( unsigned char), 7u);}{ struct Cyc_List_List* tvs= tud->tvs;{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 != 0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
struct Cyc_Absyn_Conref* c= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->kind); void* _temp554=( void*)
c->v; void* _temp562; _LL556: if( _temp554 ==( void*) Cyc_Absyn_No_constr){ goto
_LL557;} else{ goto _LL558;} _LL558: if(( unsigned int) _temp554 > 1u?*(( int*)
_temp554) == Cyc_Absyn_Eq_constr: 0){ _LL563: _temp562=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp554)->f1; if( _temp562 ==( void*) Cyc_Absyn_MemKind){ goto _LL559;} else{
goto _LL560;}} else{ goto _LL560;} _LL560: goto _LL561; _LL557:( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp564=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp564[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp565; _temp565.tag= Cyc_Absyn_Eq_constr; _temp565.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp565;}); _temp564;}))); goto _LL555;
_LL559: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp566)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp572= _tag_arr("%s %s attempts to abstract type variable %s of kind M",
sizeof( unsigned char), 54u); struct Cyc_Stdio_String_pa_struct _temp575;
_temp575.tag= Cyc_Stdio_String_pa; _temp575.f1=( struct _tagged_arr) obj;{ void*
_temp569=( void*)& _temp575; struct Cyc_Stdio_String_pa_struct _temp574;
_temp574.tag= Cyc_Stdio_String_pa; _temp574.f1=( struct _tagged_arr)* v;{ void*
_temp570=( void*)& _temp574; struct Cyc_Stdio_String_pa_struct _temp573;
_temp573.tag= Cyc_Stdio_String_pa; _temp573.f1=( struct _tagged_arr)*(( struct
Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->name;{ void*
_temp571=( void*)& _temp573; void* _temp567[ 3u]={ _temp569, _temp570, _temp571};
struct _tagged_arr _temp568={( void*) _temp567,( void*) _temp567,( void*)(
_temp567 + 3u)}; _temp566( _temp572, _temp568);}}}})); goto _LL555; _LL561: goto
_LL555; _LL555:;}} Cyc_Tcutil_check_unique_tvars( loc, tvs); Cyc_Tcutil_add_tvar_identities(
tvs);{ struct Cyc_Core_Opt* tud_opt; if( tud->is_xtunion){{ struct _handler_cons
_temp576; _push_handler(& _temp576);{ int _temp578= 0; if( setjmp( _temp576.handler)){
_temp578= 1;} if( ! _temp578){ tud_opt= Cyc_Tcenv_lookup_xtuniondecl( te, loc,
tud->name);; _pop_handler();} else{ void* _temp577=( void*) _exn_thrown; void*
_temp580= _temp577; _LL582: if( _temp580 == Cyc_Dict_Absent){ goto _LL583;}
else{ goto _LL584;} _LL584: goto _LL585; _LL583: Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp586)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp590= _tag_arr("qualified xtunion declaration %s is not an existing xtunion",
sizeof( unsigned char), 60u); struct Cyc_Stdio_String_pa_struct _temp591;
_temp591.tag= Cyc_Stdio_String_pa; _temp591.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
tud->name);{ void* _temp589=( void*)& _temp591; void* _temp587[ 1u]={ _temp589};
struct _tagged_arr _temp588={( void*) _temp587,( void*) _temp587,( void*)(
_temp587 + 1u)}; _temp586( _temp590, _temp588);}})); return; _LL585:( void)
_throw( _temp580); _LL581:;}}} if( tud_opt != 0){ tud->name=(*(( struct Cyc_Absyn_Tuniondecl**)((
struct Cyc_Core_Opt*) _check_null( tud_opt))->v))->name;} else{(* tud->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp592=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp592[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp593; _temp593.tag= Cyc_Absyn_Abs_n; _temp593.f1= te->ns; _temp593;});
_temp592;});}} else{{ void* _temp594=(* tud->name).f1; struct Cyc_List_List*
_temp602; _LL596: if(( unsigned int) _temp594 > 1u?*(( int*) _temp594) == Cyc_Absyn_Rel_n:
0){ _LL603: _temp602=(( struct Cyc_Absyn_Rel_n_struct*) _temp594)->f1; if(
_temp602 == 0){ goto _LL597;} else{ goto _LL598;}} else{ goto _LL598;} _LL598:
if(( unsigned int) _temp594 > 1u?*(( int*) _temp594) == Cyc_Absyn_Abs_n: 0){
goto _LL599;} else{ goto _LL600;} _LL600: goto _LL601; _LL597:(* tud->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp604=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp604[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp605; _temp605.tag= Cyc_Absyn_Abs_n; _temp605.f1= te->ns; _temp605;});
_temp604;}); goto _LL595; _LL599: goto _LL601; _LL601: Cyc_Tcutil_terr( loc,
_tag_arr("qualified declarations are not implemented", sizeof( unsigned char),
43u)); return; _LL595:;} tud_opt=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)( ge->tuniondecls, v);}{ struct
_tuple4 _temp607=({ struct _tuple4 _temp606; _temp606.f1= tud->fields; _temp606.f2=
tud_opt; _temp606;}); struct Cyc_Core_Opt* _temp617; struct Cyc_Core_Opt*
_temp619; struct Cyc_Core_Opt* _temp621; struct Cyc_Core_Opt* _temp623; struct
Cyc_Core_Opt _temp625; struct Cyc_List_List* _temp626; struct Cyc_List_List**
_temp628; struct Cyc_Core_Opt* _temp629; struct Cyc_Core_Opt _temp631; struct
Cyc_Absyn_Tuniondecl** _temp632; struct Cyc_Core_Opt* _temp634; struct Cyc_Core_Opt
_temp636; struct Cyc_List_List* _temp637; struct Cyc_List_List** _temp639;
struct Cyc_Core_Opt* _temp640; struct Cyc_Core_Opt _temp642; struct Cyc_Absyn_Tuniondecl**
_temp643; struct Cyc_Core_Opt* _temp645; _LL609: _LL620: _temp619= _temp607.f1;
if( _temp619 == 0){ goto _LL618;} else{ goto _LL611;} _LL618: _temp617= _temp607.f2;
if( _temp617 == 0){ goto _LL610;} else{ goto _LL611;} _LL611: _LL624: _temp623=
_temp607.f1; if( _temp623 == 0){ goto _LL613;} else{ _temp625=* _temp623; _LL627:
_temp626=( struct Cyc_List_List*) _temp625.v; _temp628=&(* _temp607.f1).v; goto
_LL622;} _LL622: _temp621= _temp607.f2; if( _temp621 == 0){ goto _LL612;} else{
goto _LL613;} _LL613: _LL635: _temp634= _temp607.f1; if( _temp634 == 0){ goto
_LL615;} else{ _temp636=* _temp634; _LL638: _temp637=( struct Cyc_List_List*)
_temp636.v; _temp639=&(* _temp607.f1).v; goto _LL630;} _LL630: _temp629=
_temp607.f2; if( _temp629 == 0){ goto _LL615;} else{ _temp631=* _temp629; _LL633:
_temp632=( struct Cyc_Absyn_Tuniondecl**) _temp631.v; goto _LL614;} _LL615:
_LL646: _temp645= _temp607.f1; if( _temp645 == 0){ goto _LL641;} else{ goto
_LL608;} _LL641: _temp640= _temp607.f2; if( _temp640 == 0){ goto _LL608;} else{
_temp642=* _temp640; _LL644: _temp643=( struct Cyc_Absyn_Tuniondecl**) _temp642.v;
goto _LL616;} _LL610: ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)(
ge->tuniondecls, v,({ struct Cyc_Absyn_Tuniondecl** _temp647=( struct Cyc_Absyn_Tuniondecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp647[ 0]= tud; _temp647;}));
goto _LL608; _LL612: { struct Cyc_Absyn_Tuniondecl** _temp648=({ struct Cyc_Absyn_Tuniondecl**
_temp649=( struct Cyc_Absyn_Tuniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*));
_temp649[ 0]=({ struct Cyc_Absyn_Tuniondecl* _temp650=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp650->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp650->name= tud->name; _temp650->tvs= tvs; _temp650->fields=
0; _temp650->is_xtunion= tud->is_xtunion; _temp650;}); _temp649;}); ge->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)( ge->tuniondecls, v,
_temp648);* _temp628= Cyc_Tc_tcTunionFields( te, ge, loc, obj, tud->is_xtunion,
tud->name,* _temp628, tvs, tud);* _temp648= tud; goto _LL608;} _LL614: { struct
Cyc_Absyn_Tuniondecl* _temp651=* _temp632;* _temp632=({ struct Cyc_Absyn_Tuniondecl*
_temp652=( struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp652->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp652->name= tud->name;
_temp652->tvs= tvs; _temp652->fields= 0; _temp652->is_xtunion= tud->is_xtunion;
_temp652;});* _temp639= Cyc_Tc_tcTunionFields( te, ge, loc, obj, tud->is_xtunion,
tud->name,* _temp639, tvs, tud);* _temp632= _temp651; _temp643= _temp632; goto
_LL616;} _LL616: { struct Cyc_Absyn_Tuniondecl* _temp653= Cyc_Tcdecl_merge_tuniondecl(*
_temp643, tud, loc, Cyc_Tc_tc_msg); if( _temp653 == 0){ return;} else{* _temp643=(
struct Cyc_Absyn_Tuniondecl*) _check_null( _temp653); goto _LL608;}} _LL608:;}}}}
static void Cyc_Tc_tcEnumdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Enumdecl* ed){ struct
_tagged_arr* v=(* ed->name).f2;{ void* _temp654=(* ed->name).f1; struct Cyc_List_List*
_temp662; struct Cyc_List_List* _temp664; _LL656: if(( unsigned int) _temp654 >
1u?*(( int*) _temp654) == Cyc_Absyn_Rel_n: 0){ _LL663: _temp662=(( struct Cyc_Absyn_Rel_n_struct*)
_temp654)->f1; if( _temp662 == 0){ goto _LL657;} else{ goto _LL658;}} else{ goto
_LL658;} _LL658: if(( unsigned int) _temp654 > 1u?*(( int*) _temp654) == Cyc_Absyn_Abs_n:
0){ _LL665: _temp664=(( struct Cyc_Absyn_Abs_n_struct*) _temp654)->f1; if(
_temp664 == 0){ goto _LL659;} else{ goto _LL660;}} else{ goto _LL660;} _LL660:
goto _LL661; _LL657: goto _LL655; _LL659: goto _LL655; _LL661: Cyc_Tcutil_terr(
loc, _tag_arr("qualified declarations are not implemented", sizeof(
unsigned char), 43u)); return; _LL655:;}(* ed->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp666=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp666[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp667; _temp667.tag= Cyc_Absyn_Abs_n;
_temp667.f1= te->ns; _temp667;}); _temp666;}); if( ed->fields != 0){ struct
_RegionHandle _temp668= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp668; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0;
unsigned int tag_count= 0; struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( ed->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ struct Cyc_Absyn_Enumfield*
_temp669=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( fs))->hd;
if((( int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*), struct
Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp,
prev_fields,(* _temp669->name).f2)){ Cyc_Tcutil_terr( _temp669->loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp670)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp674= _tag_arr("duplicate field name %s",
sizeof( unsigned char), 24u); struct Cyc_Stdio_String_pa_struct _temp675;
_temp675.tag= Cyc_Stdio_String_pa; _temp675.f1=( struct _tagged_arr)*(* _temp669->name).f2;{
void* _temp673=( void*)& _temp675; void* _temp671[ 1u]={ _temp673}; struct
_tagged_arr _temp672={( void*) _temp671,( void*) _temp671,( void*)( _temp671 + 1u)};
_temp670( _temp674, _temp672);}}));} else{ prev_fields=({ struct Cyc_List_List*
_temp676=( struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp676->hd=( void*)(* _temp669->name).f2; _temp676->tl= prev_fields; _temp676;});}
if( _temp669->tag == 0){ _temp669->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
tag_count, _temp669->loc);} else{ if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp669->tag))){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr(* _temp677)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp682= _tag_arr("enum %s, field %s: expression is not constant",
sizeof( unsigned char), 46u); struct Cyc_Stdio_String_pa_struct _temp684;
_temp684.tag= Cyc_Stdio_String_pa; _temp684.f1=( struct _tagged_arr)* v;{ void*
_temp680=( void*)& _temp684; struct Cyc_Stdio_String_pa_struct _temp683;
_temp683.tag= Cyc_Stdio_String_pa; _temp683.f1=( struct _tagged_arr)*(* _temp669->name).f2;{
void* _temp681=( void*)& _temp683; void* _temp678[ 2u]={ _temp680, _temp681};
struct _tagged_arr _temp679={( void*) _temp678,( void*) _temp678,( void*)(
_temp678 + 2u)}; _temp677( _temp682, _temp679);}}}));}}{ unsigned int t1= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp669->tag)); tag_count= t1 + 1;(*
_temp669->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp685=( struct
Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp685[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp686; _temp686.tag= Cyc_Absyn_Abs_n;
_temp686.f1= te->ns; _temp686;}); _temp685;});}}}; _pop_region( uprev_rgn);}{
struct _handler_cons _temp687; _push_handler(& _temp687);{ int _temp689= 0; if(
setjmp( _temp687.handler)){ _temp689= 1;} if( ! _temp689){{ struct Cyc_Absyn_Enumdecl**
_temp690=(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup)( ge->enumdecls, v); struct Cyc_Absyn_Enumdecl*
_temp691= Cyc_Tcdecl_merge_enumdecl(* _temp690, ed, loc, Cyc_Tc_tc_msg); if(
_temp691 == 0){ _npop_handler( 0u); return;}* _temp690=( struct Cyc_Absyn_Enumdecl*)
_check_null( _temp691);}; _pop_handler();} else{ void* _temp688=( void*)
_exn_thrown; void* _temp693= _temp688; _LL695: if( _temp693 == Cyc_Dict_Absent){
goto _LL696;} else{ goto _LL697;} _LL697: goto _LL698; _LL696: { struct Cyc_Absyn_Enumdecl**
_temp699=({ struct Cyc_Absyn_Enumdecl** _temp700=( struct Cyc_Absyn_Enumdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl*)); _temp700[ 0]= ed; _temp700;});
ge->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key, struct Cyc_Absyn_Enumdecl** data)) Cyc_Dict_insert)( ge->enumdecls,
v, _temp699); goto _LL694;} _LL698:( void) _throw( _temp693); _LL694:;}}} if( ed->fields
!= 0){ struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ed->fields))->v; for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ struct Cyc_Absyn_Enumfield* _temp701=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_List_List*) _check_null( fs))->hd; ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries,(* _temp701->name).f2,({ struct _tuple3* _temp702=( struct
_tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp702->f1=( void*)({ struct
Cyc_Tcenv_EnumRes_struct* _temp703=( struct Cyc_Tcenv_EnumRes_struct*) GC_malloc(
sizeof( struct Cyc_Tcenv_EnumRes_struct)); _temp703[ 0]=({ struct Cyc_Tcenv_EnumRes_struct
_temp704; _temp704.tag= Cyc_Tcenv_EnumRes; _temp704.f1= ed; _temp704.f2=
_temp701; _temp704;}); _temp703;}); _temp702->f2= 1; _temp702;}));}}} static int
Cyc_Tc_okay_externC( struct Cyc_Position_Segment* loc, void* sc){ void* _temp705=
sc; _LL707: if( _temp705 ==( void*) Cyc_Absyn_Static){ goto _LL708;} else{ goto
_LL709;} _LL709: if( _temp705 ==( void*) Cyc_Absyn_Abstract){ goto _LL710;}
else{ goto _LL711;} _LL711: if( _temp705 ==( void*) Cyc_Absyn_Public){ goto
_LL712;} else{ goto _LL713;} _LL713: if( _temp705 ==( void*) Cyc_Absyn_Extern){
goto _LL714;} else{ goto _LL715;} _LL715: if( _temp705 ==( void*) Cyc_Absyn_ExternC){
goto _LL716;} else{ goto _LL706;} _LL708: Cyc_Tcutil_warn( loc, _tag_arr("static declaration nested within extern \"C\"",
sizeof( unsigned char), 44u)); return 0; _LL710: Cyc_Tcutil_warn( loc, _tag_arr("abstract declaration nested within extern \"C\"",
sizeof( unsigned char), 46u)); return 0; _LL712: return 1; _LL714: return 1;
_LL716: Cyc_Tcutil_warn( loc, _tag_arr("nested extern \"C\" declaration",
sizeof( unsigned char), 30u)); return 1; _LL706:;} static void Cyc_Tc_tc_decls(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds0, int in_externC, int
check_var_init){ struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, te->ns);
struct Cyc_List_List* _temp717= ds0; for( 0; _temp717 != 0; _temp717=(( struct
Cyc_List_List*) _check_null( _temp717))->tl){ struct Cyc_Absyn_Decl* d=( struct
Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( _temp717))->hd; struct
Cyc_Position_Segment* loc= d->loc; void* _temp718=( void*) d->r; struct Cyc_Absyn_Vardecl*
_temp744; struct Cyc_Absyn_Fndecl* _temp746; struct Cyc_Absyn_Typedefdecl*
_temp748; struct Cyc_Absyn_Structdecl* _temp750; struct Cyc_Absyn_Uniondecl*
_temp752; struct Cyc_Absyn_Tuniondecl* _temp754; struct Cyc_Absyn_Enumdecl*
_temp756; struct Cyc_List_List* _temp758; struct _tagged_arr* _temp760; struct
Cyc_List_List* _temp762; struct _tuple0* _temp764; struct _tuple0 _temp766;
struct _tagged_arr* _temp767; void* _temp769; struct Cyc_List_List* _temp771;
_LL720: if(*(( int*) _temp718) == Cyc_Absyn_Let_d){ goto _LL721;} else{ goto
_LL722;} _LL722: if(*(( int*) _temp718) == Cyc_Absyn_Letv_d){ goto _LL723;}
else{ goto _LL724;} _LL724: if(*(( int*) _temp718) == Cyc_Absyn_Var_d){ _LL745:
_temp744=(( struct Cyc_Absyn_Var_d_struct*) _temp718)->f1; goto _LL725;} else{
goto _LL726;} _LL726: if(*(( int*) _temp718) == Cyc_Absyn_Fn_d){ _LL747:
_temp746=(( struct Cyc_Absyn_Fn_d_struct*) _temp718)->f1; goto _LL727;} else{
goto _LL728;} _LL728: if(*(( int*) _temp718) == Cyc_Absyn_Typedef_d){ _LL749:
_temp748=(( struct Cyc_Absyn_Typedef_d_struct*) _temp718)->f1; goto _LL729;}
else{ goto _LL730;} _LL730: if(*(( int*) _temp718) == Cyc_Absyn_Struct_d){
_LL751: _temp750=(( struct Cyc_Absyn_Struct_d_struct*) _temp718)->f1; goto
_LL731;} else{ goto _LL732;} _LL732: if(*(( int*) _temp718) == Cyc_Absyn_Union_d){
_LL753: _temp752=(( struct Cyc_Absyn_Union_d_struct*) _temp718)->f1; goto _LL733;}
else{ goto _LL734;} _LL734: if(*(( int*) _temp718) == Cyc_Absyn_Tunion_d){
_LL755: _temp754=(( struct Cyc_Absyn_Tunion_d_struct*) _temp718)->f1; goto
_LL735;} else{ goto _LL736;} _LL736: if(*(( int*) _temp718) == Cyc_Absyn_Enum_d){
_LL757: _temp756=(( struct Cyc_Absyn_Enum_d_struct*) _temp718)->f1; goto _LL737;}
else{ goto _LL738;} _LL738: if(*(( int*) _temp718) == Cyc_Absyn_Namespace_d){
_LL761: _temp760=(( struct Cyc_Absyn_Namespace_d_struct*) _temp718)->f1; goto
_LL759; _LL759: _temp758=(( struct Cyc_Absyn_Namespace_d_struct*) _temp718)->f2;
goto _LL739;} else{ goto _LL740;} _LL740: if(*(( int*) _temp718) == Cyc_Absyn_Using_d){
_LL765: _temp764=(( struct Cyc_Absyn_Using_d_struct*) _temp718)->f1; _temp766=*
_temp764; _LL770: _temp769= _temp766.f1; goto _LL768; _LL768: _temp767= _temp766.f2;
goto _LL763; _LL763: _temp762=(( struct Cyc_Absyn_Using_d_struct*) _temp718)->f2;
goto _LL741;} else{ goto _LL742;} _LL742: if(*(( int*) _temp718) == Cyc_Absyn_ExternC_d){
_LL772: _temp771=(( struct Cyc_Absyn_ExternC_d_struct*) _temp718)->f1; goto
_LL743;} else{ goto _LL719;} _LL721: Cyc_Tcutil_terr( loc, _tag_arr("top level let-declarations are not implemented",
sizeof( unsigned char), 47u)); goto _LL719; _LL723: Cyc_Tcutil_terr( loc,
_tag_arr("top level let-declarations are not implemented", sizeof( unsigned char),
47u)); goto _LL719; _LL725: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*)
_temp744->sc): 0){( void*)( _temp744->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcVardecl( te, ge, loc, _temp744, check_var_init); goto _LL719; _LL727:
if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp746->sc): 0){( void*)(
_temp746->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcFndecl( te, ge,
loc, _temp746); goto _LL719; _LL729: Cyc_Tc_tcTypedefdecl( te, ge, loc, _temp748);
goto _LL719; _LL731: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*)
_temp750->sc): 0){( void*)( _temp750->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcStructdecl( te, ge, loc, _temp750); goto _LL719; _LL733: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp752->sc): 0){( void*)( _temp752->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcUniondecl( te, ge, loc, _temp752);
goto _LL719; _LL735: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*)
_temp754->sc): 0){( void*)( _temp754->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcTuniondecl( te, ge, loc, _temp754); goto _LL719; _LL737: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp756->sc): 0){( void*)( _temp756->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcEnumdecl( te, ge, loc, _temp756);
goto _LL719; _LL739: { struct Cyc_List_List* _temp773= te->ns; struct Cyc_List_List*
_temp774=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp773,({ struct Cyc_List_List* _temp775=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp775->hd=( void*) _temp760;
_temp775->tl= 0; _temp775;})); if( !(( int(*)( struct Cyc_Set_Set* s, struct
_tagged_arr* elt)) Cyc_Set_member)( ge->namespaces, _temp760)){ ge->namespaces=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)(
ge->namespaces, _temp760); te->ae=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key, struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)( te->ae,
_temp774, Cyc_Tcenv_empty_genv());} te->ns= _temp774; Cyc_Tc_tc_decls( te,
_temp758, in_externC, check_var_init); te->ns= _temp773; goto _LL719;} _LL741: {
struct _tagged_arr* first; struct Cyc_List_List* rest;{ void* _temp776= _temp769;
struct Cyc_List_List* _temp788; struct Cyc_List_List* _temp790; struct Cyc_List_List*
_temp792; struct Cyc_List_List _temp794; struct Cyc_List_List* _temp795; struct
_tagged_arr* _temp797; struct Cyc_List_List* _temp799; struct Cyc_List_List
_temp801; struct Cyc_List_List* _temp802; struct _tagged_arr* _temp804; _LL778:
if( _temp776 ==( void*) Cyc_Absyn_Loc_n){ goto _LL779;} else{ goto _LL780;}
_LL780: if(( unsigned int) _temp776 > 1u?*(( int*) _temp776) == Cyc_Absyn_Rel_n:
0){ _LL789: _temp788=(( struct Cyc_Absyn_Rel_n_struct*) _temp776)->f1; if(
_temp788 == 0){ goto _LL781;} else{ goto _LL782;}} else{ goto _LL782;} _LL782:
if(( unsigned int) _temp776 > 1u?*(( int*) _temp776) == Cyc_Absyn_Abs_n: 0){
_LL791: _temp790=(( struct Cyc_Absyn_Abs_n_struct*) _temp776)->f1; if( _temp790
== 0){ goto _LL783;} else{ goto _LL784;}} else{ goto _LL784;} _LL784: if((
unsigned int) _temp776 > 1u?*(( int*) _temp776) == Cyc_Absyn_Rel_n: 0){ _LL793:
_temp792=(( struct Cyc_Absyn_Rel_n_struct*) _temp776)->f1; if( _temp792 == 0){
goto _LL786;} else{ _temp794=* _temp792; _LL798: _temp797=( struct _tagged_arr*)
_temp794.hd; goto _LL796; _LL796: _temp795= _temp794.tl; goto _LL785;}} else{
goto _LL786;} _LL786: if(( unsigned int) _temp776 > 1u?*(( int*) _temp776) ==
Cyc_Absyn_Abs_n: 0){ _LL800: _temp799=(( struct Cyc_Absyn_Abs_n_struct*)
_temp776)->f1; if( _temp799 == 0){ goto _LL777;} else{ _temp801=* _temp799;
_LL805: _temp804=( struct _tagged_arr*) _temp801.hd; goto _LL803; _LL803:
_temp802= _temp801.tl; goto _LL787;}} else{ goto _LL777;} _LL779: goto _LL781;
_LL781: goto _LL783; _LL783: first= _temp767; rest= 0; goto _LL777; _LL785:
_temp804= _temp797; _temp802= _temp795; goto _LL787; _LL787: first= _temp804;
rest=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp802,({ struct Cyc_List_List* _temp806=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp806->hd=( void*) _temp767;
_temp806->tl= 0; _temp806;})); goto _LL777; _LL777:;}{ struct Cyc_List_List*
_temp807= Cyc_Tcenv_resolve_namespace( te, loc, first, rest); ge->availables=({
struct Cyc_List_List* _temp808=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp808->hd=( void*) _temp807; _temp808->tl= ge->availables;
_temp808;}); Cyc_Tc_tc_decls( te, _temp762, in_externC, check_var_init); ge->availables=((
struct Cyc_List_List*) _check_null( ge->availables))->tl; goto _LL719;}} _LL743:
Cyc_Tc_tc_decls( te, _temp771, 1, check_var_init); goto _LL719; _LL719:;}} void
Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int check_var_init, struct Cyc_List_List*
ds){ Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r); Cyc_Tc_tc_decls( te, ds,
0, check_var_init);} static int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env,
struct Cyc_Absyn_Decl* d); static struct Cyc_List_List* Cyc_Tc_treeshake_f(
struct Cyc_Dict_Dict* env, struct Cyc_List_List* ds){ return(( struct Cyc_List_List*(*)(
int(* f)( struct Cyc_Dict_Dict*, struct Cyc_Absyn_Decl*), struct Cyc_Dict_Dict*
env, struct Cyc_List_List* l)) Cyc_List_filter_c)( Cyc_Tc_vardecl_needed, env,
ds);} struct _tuple6{ struct Cyc_Tcenv_Genv* f1; struct Cyc_Set_Set* f2; } ;
static int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl*
d){ void* _temp809=( void*) d->r; struct Cyc_Absyn_Vardecl* _temp819; struct Cyc_List_List*
_temp821; struct Cyc_List_List** _temp823; struct Cyc_List_List* _temp824;
struct Cyc_List_List** _temp826; _LL811: if(*(( int*) _temp809) == Cyc_Absyn_Var_d){
_LL820: _temp819=(( struct Cyc_Absyn_Var_d_struct*) _temp809)->f1; goto _LL812;}
else{ goto _LL813;} _LL813: if(*(( int*) _temp809) == Cyc_Absyn_Using_d){ _LL822:
_temp821=(( struct Cyc_Absyn_Using_d_struct*) _temp809)->f2; _temp823=&(( struct
Cyc_Absyn_Using_d_struct*) _temp809)->f2; goto _LL814;} else{ goto _LL815;}
_LL815: if(*(( int*) _temp809) == Cyc_Absyn_Namespace_d){ _LL825: _temp824=((
struct Cyc_Absyn_Namespace_d_struct*) _temp809)->f2; _temp826=&(( struct Cyc_Absyn_Namespace_d_struct*)
_temp809)->f2; goto _LL816;} else{ goto _LL817;} _LL817: goto _LL818; _LL812:
if(( void*) _temp819->sc !=( void*) Cyc_Absyn_Extern){ return 1;}{ struct
_tuple0 _temp829; struct _tagged_arr* _temp830; void* _temp832; struct _tuple0*
_temp827= _temp819->name; _temp829=* _temp827; _LL833: _temp832= _temp829.f1;
goto _LL831; _LL831: _temp830= _temp829.f2; goto _LL828; _LL828: { struct Cyc_List_List*
ns;{ void* _temp834= _temp832; struct Cyc_List_List* _temp842; struct Cyc_List_List*
_temp844; _LL836: if( _temp834 ==( void*) Cyc_Absyn_Loc_n){ goto _LL837;} else{
goto _LL838;} _LL838: if(( unsigned int) _temp834 > 1u?*(( int*) _temp834) ==
Cyc_Absyn_Rel_n: 0){ _LL843: _temp842=(( struct Cyc_Absyn_Rel_n_struct*)
_temp834)->f1; goto _LL839;} else{ goto _LL840;} _LL840: if(( unsigned int)
_temp834 > 1u?*(( int*) _temp834) == Cyc_Absyn_Abs_n: 0){ _LL845: _temp844=((
struct Cyc_Absyn_Abs_n_struct*) _temp834)->f1; goto _LL841;} else{ goto _LL835;}
_LL837: ns= 0; goto _LL835; _LL839: ns= _temp842; goto _LL835; _LL841: ns=
_temp844; goto _LL835; _LL835:;}{ struct _tuple6* _temp846=(( struct _tuple6*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( env, ns);
struct Cyc_Tcenv_Genv* _temp847=(* _temp846).f1; int _temp848=(*(( struct
_tuple3*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
_temp847->ordinaries, _temp830)).f2; if( ! _temp848){(* _temp846).f2=(( struct
Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)((*
_temp846).f2, _temp830);} return _temp848;}}} _LL814: _temp826= _temp823; goto
_LL816; _LL816:* _temp826= Cyc_Tc_treeshake_f( env,* _temp826); return 1; _LL818:
return 1; _LL810:;} static int Cyc_Tc_treeshake_remove_f( struct Cyc_Set_Set*
set, struct _tagged_arr* x, struct _tuple3* y){ return !(( int(*)( struct Cyc_Set_Set*
s, struct _tagged_arr* elt)) Cyc_Set_member)( set, x);} static void Cyc_Tc_treeshake_remove(
struct Cyc_List_List* ns, struct _tuple6* nsenv){ struct Cyc_Tcenv_Genv*
_temp849=(* nsenv).f1; struct Cyc_Set_Set* _temp850=(* nsenv).f2; _temp849->ordinaries=((
struct Cyc_Dict_Dict*(*)( int(* f)( struct Cyc_Set_Set*, struct _tagged_arr*,
struct _tuple3*), struct Cyc_Set_Set* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_filter_c)(
Cyc_Tc_treeshake_remove_f, _temp850, _temp849->ordinaries);} static struct
_tuple6* Cyc_Tc_treeshake_make_env_f( struct Cyc_Tcenv_Genv* ge){ return({
struct _tuple6* _temp851=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp851->f1= ge; _temp851->f2=(( struct Cyc_Set_Set*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Set_empty)( Cyc_String_strptrcmp);
_temp851;});} struct Cyc_List_List* Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv* te,
struct Cyc_List_List* ds){ struct Cyc_Dict_Dict* _temp852=(( struct Cyc_Dict_Dict*(*)(
struct _tuple6*(* f)( struct Cyc_Tcenv_Genv*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)(
Cyc_Tc_treeshake_make_env_f, te->ae); struct Cyc_List_List* _temp853= Cyc_Tc_treeshake_f(
_temp852, ds);(( void(*)( void(* f)( struct Cyc_List_List*, struct _tuple6*),
struct Cyc_Dict_Dict* d)) Cyc_Dict_iter)( Cyc_Tc_treeshake_remove, _temp852);
return _temp853;}