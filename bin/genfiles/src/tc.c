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
int tag; unsigned int* f1; } ; static const int Cyc_Stdio_ShortPtr_sa= 0; struct
Cyc_Stdio_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int Cyc_Stdio_UShortPtr_sa=
1; struct Cyc_Stdio_UShortPtr_sa_struct{ int tag; unsigned short* f1; } ; static
const int Cyc_Stdio_IntPtr_sa= 2; struct Cyc_Stdio_IntPtr_sa_struct{ int tag;
int* f1; } ; static const int Cyc_Stdio_UIntPtr_sa= 3; struct Cyc_Stdio_UIntPtr_sa_struct{
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
Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos( struct _tagged_arr fmt,
struct _tagged_arr ap); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap); extern void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap);
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
_LL6: return({ void* _temp15[ 0u]={};(( struct Cyc_List_List*(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("transfer_fn_type_atts",
sizeof( unsigned char), 22u), _tag_arr( _temp15, sizeof( void*), 0u));}); _LL2:;}
struct _tuple3{ void* f1; int f2; } ; static void Cyc_Tc_tcVardecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Vardecl*
vd, int check_var_init){ struct _tagged_arr* v=(* vd->name).f2; void* t=( void*)
vd->type; void* sc=( void*) vd->sc; struct Cyc_List_List* atts= vd->attributes;{
void* _temp16=(* vd->name).f1; struct Cyc_List_List* _temp24; struct Cyc_List_List*
_temp26; _LL18: if(( unsigned int) _temp16 > 1u?*(( int*) _temp16) == Cyc_Absyn_Rel_n:
0){ _LL25: _temp24=(( struct Cyc_Absyn_Rel_n_struct*) _temp16)->f1; if( _temp24
== 0){ goto _LL19;} else{ goto _LL20;}} else{ goto _LL20;} _LL20: if((
unsigned int) _temp16 > 1u?*(( int*) _temp16) == Cyc_Absyn_Abs_n: 0){ _LL27:
_temp26=(( struct Cyc_Absyn_Abs_n_struct*) _temp16)->f1; if( _temp26 == 0){ goto
_LL21;} else{ goto _LL22;}} else{ goto _LL22;} _LL22: goto _LL23; _LL19: goto
_LL17; _LL21: goto _LL17; _LL23:({ void* _temp28[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("qualified declarations are not implemented", sizeof( unsigned char),
43u), _tag_arr( _temp28, sizeof( void*), 0u));}); return; _LL17:;}(* vd->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp29=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp29[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp30; _temp30.tag= Cyc_Absyn_Abs_n; _temp30.f1= te->ns; _temp30;}); _temp29;});{
void* _temp31= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp* _temp38; struct
Cyc_Absyn_Tqual _temp40; void* _temp42; _LL33: if(( unsigned int) _temp31 > 4u?*((
int*) _temp31) == Cyc_Absyn_ArrayType: 0){ _LL43: _temp42=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp31)->f1; goto _LL41; _LL41: _temp40=(( struct Cyc_Absyn_ArrayType_struct*)
_temp31)->f2; goto _LL39; _LL39: _temp38=(( struct Cyc_Absyn_ArrayType_struct*)
_temp31)->f3; if( _temp38 == 0){ goto _LL37;} else{ goto _LL35;}} else{ goto
_LL35;} _LL37: if( vd->initializer != 0){ goto _LL34;} else{ goto _LL35;} _LL35:
goto _LL36; _LL34:{ void* _temp44=( void*)(( struct Cyc_Absyn_Exp*) _check_null(
vd->initializer))->r; void* _temp54; struct _tagged_arr _temp56; struct Cyc_List_List*
_temp58; struct Cyc_List_List* _temp60; _LL46: if(*(( int*) _temp44) == Cyc_Absyn_Const_e){
_LL55: _temp54=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp44)->f1; if((
unsigned int) _temp54 > 1u?*(( int*) _temp54) == Cyc_Absyn_String_c: 0){ _LL57:
_temp56=(( struct Cyc_Absyn_String_c_struct*) _temp54)->f1; goto _LL47;} else{
goto _LL48;}} else{ goto _LL48;} _LL48: if(*(( int*) _temp44) == Cyc_Absyn_UnresolvedMem_e){
_LL59: _temp58=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp44)->f2; goto
_LL49;} else{ goto _LL50;} _LL50: if(*(( int*) _temp44) == Cyc_Absyn_Array_e){
_LL61: _temp60=(( struct Cyc_Absyn_Array_e_struct*) _temp44)->f1; goto _LL51;}
else{ goto _LL52;} _LL52: goto _LL53; _LL47: t=( void*)( vd->type=( void*)((
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp62=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp62[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp63; _temp63.tag= Cyc_Absyn_ArrayType; _temp63.f1=(
void*) _temp42; _temp63.f2= _temp40; _temp63.f3=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
_get_arr_size( _temp56, sizeof( unsigned char)), 0); _temp63;}); _temp62;})));
goto _LL45; _LL49: _temp60= _temp58; goto _LL51; _LL51: t=( void*)( vd->type=(
void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp64=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp64[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp65; _temp65.tag= Cyc_Absyn_ArrayType; _temp65.f1=(
void*) _temp42; _temp65.f2= _temp40; _temp65.f3=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp((
unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp60), 0);
_temp65;}); _temp64;}))); goto _LL45; _LL53: goto _LL45; _LL45:;} goto _LL32;
_LL36: goto _LL32; _LL32:;} Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);
if( Cyc_Tcutil_is_function_type( t)){ atts= Cyc_Tc_transfer_fn_type_atts( t,
atts);} if( sc ==( void*) Cyc_Absyn_Extern? 1: sc ==( void*) Cyc_Absyn_ExternC){
if( vd->initializer != 0){({ void* _temp66[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("extern declaration should not have initializer", sizeof( unsigned char),
47u), _tag_arr( _temp66, sizeof( void*), 0u));});}} else{ if( ! Cyc_Tcutil_is_function_type(
t)){ for( 0; atts != 0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){
void* _temp67=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd; _LL69:
if(( unsigned int) _temp67 > 16u?*(( int*) _temp67) == Cyc_Absyn_Aligned_att: 0){
goto _LL70;} else{ goto _LL71;} _LL71: if(( unsigned int) _temp67 > 16u?*(( int*)
_temp67) == Cyc_Absyn_Section_att: 0){ goto _LL72;} else{ goto _LL73;} _LL73:
if( _temp67 ==( void*) Cyc_Absyn_Nocommon_att){ goto _LL74;} else{ goto _LL75;}
_LL75: if( _temp67 ==( void*) Cyc_Absyn_Shared_att){ goto _LL76;} else{ goto
_LL77;} _LL77: if( _temp67 ==( void*) Cyc_Absyn_Unused_att){ goto _LL78;} else{
goto _LL79;} _LL79: if( _temp67 ==( void*) Cyc_Absyn_Weak_att){ goto _LL80;}
else{ goto _LL81;} _LL81: if( _temp67 ==( void*) Cyc_Absyn_Dllimport_att){ goto
_LL82;} else{ goto _LL83;} _LL83: if( _temp67 ==( void*) Cyc_Absyn_Dllexport_att){
goto _LL84;} else{ goto _LL85;} _LL85: goto _LL86; _LL70: continue; _LL72:
continue; _LL74: continue; _LL76: continue; _LL78: continue; _LL80: continue;
_LL82: continue; _LL84: continue; _LL86:({ struct Cyc_Stdio_String_pa_struct
_temp89; _temp89.tag= Cyc_Stdio_String_pa; _temp89.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
vd->name);{ struct Cyc_Stdio_String_pa_struct _temp88; _temp88.tag= Cyc_Stdio_String_pa;
_temp88.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*)(( struct
Cyc_List_List*) _check_null( atts))->hd);{ void* _temp87[ 2u]={& _temp88,&
_temp89}; Cyc_Tcutil_terr( loc, _tag_arr("bad attribute %s for variable %s",
sizeof( unsigned char), 33u), _tag_arr( _temp87, sizeof( void*), 2u));}}}); goto
_LL68; _LL68:;} if( vd->initializer == 0){ if( check_var_init? ! Cyc_Tcutil_supports_default(
t): 0){({ struct Cyc_Stdio_String_pa_struct _temp92; _temp92.tag= Cyc_Stdio_String_pa;
_temp92.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ struct Cyc_Stdio_String_pa_struct
_temp91; _temp91.tag= Cyc_Stdio_String_pa; _temp91.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
vd->name);{ void* _temp90[ 2u]={& _temp91,& _temp92}; Cyc_Tcutil_terr( loc,
_tag_arr("initializer required for variable %s of type %s", sizeof(
unsigned char), 48u), _tag_arr( _temp90, sizeof( void*), 2u));}}});}} else{
struct Cyc_Absyn_Exp* _temp93=( struct Cyc_Absyn_Exp*) _check_null( vd->initializer);
void* _temp94= Cyc_Tcexp_tcExpInitializer( te,( void**)& t, _temp93); if( ! Cyc_Tcutil_coerce_assign(
te, _temp93, t)){({ struct Cyc_Stdio_String_pa_struct _temp98; _temp98.tag= Cyc_Stdio_String_pa;
_temp98.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( _temp94);{ struct Cyc_Stdio_String_pa_struct
_temp97; _temp97.tag= Cyc_Stdio_String_pa; _temp97.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ struct Cyc_Stdio_String_pa_struct _temp96; _temp96.tag= Cyc_Stdio_String_pa;
_temp96.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( vd->name);{ void*
_temp95[ 3u]={& _temp96,& _temp97,& _temp98}; Cyc_Tcutil_terr( loc, _tag_arr("%s is declared with type %s but initialized with type %s",
sizeof( unsigned char), 57u), _tag_arr( _temp95, sizeof( void*), 3u));}}}});}
if( ! Cyc_Tcutil_is_const_exp( te, _temp93)){({ void* _temp99[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("initializer is not a constant expression", sizeof( unsigned char),
41u), _tag_arr( _temp99, sizeof( void*), 0u));});}}} else{ for( 0; atts != 0;
atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ void* _temp100=( void*)((
struct Cyc_List_List*) _check_null( atts))->hd; _LL102: if(( unsigned int)
_temp100 > 16u?*(( int*) _temp100) == Cyc_Absyn_Regparm_att: 0){ goto _LL103;}
else{ goto _LL104;} _LL104: if( _temp100 ==( void*) Cyc_Absyn_Stdcall_att){ goto
_LL105;} else{ goto _LL106;} _LL106: if( _temp100 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL107;} else{ goto _LL108;} _LL108: if( _temp100 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL109;} else{ goto _LL110;} _LL110: if( _temp100 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL111;} else{ goto _LL112;} _LL112: if(( unsigned int) _temp100 > 16u?*((
int*) _temp100) == Cyc_Absyn_Format_att: 0){ goto _LL113;} else{ goto _LL114;}
_LL114: if( _temp100 ==( void*) Cyc_Absyn_Const_att){ goto _LL115;} else{ goto
_LL116;} _LL116: if(( unsigned int) _temp100 > 16u?*(( int*) _temp100) == Cyc_Absyn_Aligned_att:
0){ goto _LL117;} else{ goto _LL118;} _LL118: if( _temp100 ==( void*) Cyc_Absyn_Packed_att){
goto _LL119;} else{ goto _LL120;} _LL120: goto _LL121; _LL103: goto _LL105;
_LL105: goto _LL107; _LL107: goto _LL109; _LL109: goto _LL111; _LL111: goto
_LL113; _LL113: goto _LL115; _LL115:({ void* _temp122[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("tcVardecl: fn type attributes in function var decl",
sizeof( unsigned char), 51u), _tag_arr( _temp122, sizeof( void*), 0u));}); goto
_LL101; _LL117: goto _LL119; _LL119:({ struct Cyc_Stdio_String_pa_struct
_temp124; _temp124.tag= Cyc_Stdio_String_pa; _temp124.f1=( struct _tagged_arr)
Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*) _check_null( atts))->hd);{
void* _temp123[ 1u]={& _temp124}; Cyc_Tcutil_terr( loc, _tag_arr("bad attribute %s in function declaration",
sizeof( unsigned char), 41u), _tag_arr( _temp123, sizeof( void*), 1u));}}); goto
_LL101; _LL121: continue; _LL101:;}}}{ struct _handler_cons _temp125;
_push_handler(& _temp125);{ int _temp127= 0; if( setjmp( _temp125.handler)){
_temp127= 1;} if( ! _temp127){{ struct _tuple3* _temp128=(( struct _tuple3*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->ordinaries,
v); void* _temp129=(* _temp128).f1; void* _temp139; _LL131: if(*(( int*)
_temp129) == Cyc_Tcenv_VarRes){ _LL140: _temp139=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp129)->f1; goto _LL132;} else{ goto _LL133;} _LL133: if(*(( int*) _temp129)
== Cyc_Tcenv_StructRes){ goto _LL134;} else{ goto _LL135;} _LL135: if(*(( int*)
_temp129) == Cyc_Tcenv_TunionRes){ goto _LL136;} else{ goto _LL137;} _LL137: if(*((
int*) _temp129) == Cyc_Tcenv_EnumRes){ goto _LL138;} else{ goto _LL130;} _LL132: {
struct Cyc_Absyn_Global_b_struct* _temp141=({ struct Cyc_Absyn_Global_b_struct*
_temp146=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp146[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp147; _temp147.tag= Cyc_Absyn_Global_b;
_temp147.f1= vd; _temp147;}); _temp146;}); void* _temp142= Cyc_Tcdecl_merge_binding(
_temp139,( void*) _temp141, loc, Cyc_Tc_tc_msg); if( _temp142 ==( void*) Cyc_Absyn_Unresolved_b){
goto _LL130;} if( _temp142 == _temp139?(* _temp128).f2: 0){ goto _LL130;} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple3*
_temp143=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp143->f1=(
void*)({ struct Cyc_Tcenv_VarRes_struct* _temp144=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp144[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp145; _temp145.tag= Cyc_Tcenv_VarRes; _temp145.f1=( void*) _temp142;
_temp145;}); _temp144;}); _temp143->f2= 1; _temp143;})); goto _LL130;} _LL134:({
void* _temp148[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("variable declaration shadows previous struct declaration",
sizeof( unsigned char), 57u), _tag_arr( _temp148, sizeof( void*), 0u));}); goto
_LL130; _LL136:({ void* _temp149[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("variable declaration shadows previous [x]tunion constructor",
sizeof( unsigned char), 60u), _tag_arr( _temp149, sizeof( void*), 0u));}); goto
_LL130; _LL138:({ void* _temp150[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("variable declaration shadows previous enum tag",
sizeof( unsigned char), 47u), _tag_arr( _temp150, sizeof( void*), 0u));}); goto
_LL130; _LL130:;}; _pop_handler();} else{ void* _temp126=( void*) _exn_thrown;
void* _temp152= _temp126; _LL154: if( _temp152 == Cyc_Dict_Absent){ goto _LL155;}
else{ goto _LL156;} _LL156: goto _LL157; _LL155: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple3* _temp158=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp158->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp159=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp159[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp160; _temp160.tag= Cyc_Tcenv_VarRes;
_temp160.f1=( void*)(( void*)({ struct Cyc_Absyn_Global_b_struct* _temp161=(
struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp161[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp162; _temp162.tag= Cyc_Absyn_Global_b;
_temp162.f1= vd; _temp162;}); _temp161;})); _temp160;}); _temp159;}); _temp158->f2=
0; _temp158;})); goto _LL153; _LL157:( void) _throw( _temp152); _LL153:;}}}}
static void Cyc_Tc_tcFndecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl* fd){ struct
_tagged_arr* v=(* fd->name).f2; void* sc=( void*) fd->sc;{ void* _temp163=(* fd->name).f1;
struct Cyc_List_List* _temp171; struct Cyc_List_List* _temp173; _LL165: if((
unsigned int) _temp163 > 1u?*(( int*) _temp163) == Cyc_Absyn_Rel_n: 0){ _LL172:
_temp171=(( struct Cyc_Absyn_Rel_n_struct*) _temp163)->f1; if( _temp171 == 0){
goto _LL166;} else{ goto _LL167;}} else{ goto _LL167;} _LL167: if(( unsigned int)
_temp163 > 1u?*(( int*) _temp163) == Cyc_Absyn_Abs_n: 0){ _LL174: _temp173=((
struct Cyc_Absyn_Abs_n_struct*) _temp163)->f1; goto _LL168;} else{ goto _LL169;}
_LL169: goto _LL170; _LL166: goto _LL164; _LL168:( int) _throw(( void*)({ struct
Cyc_Core_Impossible_struct* _temp175=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp175[ 0]=({ struct
Cyc_Core_Impossible_struct _temp176; _temp176.tag= Cyc_Core_Impossible; _temp176.f1=
_tag_arr("tc: Abs_n in tcFndecl", sizeof( unsigned char), 22u); _temp176;});
_temp175;})); _LL170:({ void* _temp177[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("qualified declarations are not implemented",
sizeof( unsigned char), 43u), _tag_arr( _temp177, sizeof( void*), 0u));});
return; _LL164:;}(* fd->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp178=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp178[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp179; _temp179.tag= Cyc_Absyn_Abs_n;
_temp179.f1= te->ns; _temp179;}); _temp178;}); Cyc_Tcutil_check_fndecl_valid_type(
loc, te, fd);{ void* t= Cyc_Tcutil_fndecl2typ( fd); fd->attributes= Cyc_Tc_transfer_fn_type_atts(
t, fd->attributes);{ struct Cyc_List_List* atts= fd->attributes; for( 0; atts !=
0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ void* _temp180=(
void*)(( struct Cyc_List_List*) _check_null( atts))->hd; _LL182: if( _temp180 ==(
void*) Cyc_Absyn_Packed_att){ goto _LL183;} else{ goto _LL184;} _LL184: if((
unsigned int) _temp180 > 16u?*(( int*) _temp180) == Cyc_Absyn_Aligned_att: 0){
goto _LL185;} else{ goto _LL186;} _LL186: goto _LL187; _LL183: goto _LL185;
_LL185:({ struct Cyc_Stdio_String_pa_struct _temp189; _temp189.tag= Cyc_Stdio_String_pa;
_temp189.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*)(( struct
Cyc_List_List*) _check_null( atts))->hd);{ void* _temp188[ 1u]={& _temp189}; Cyc_Tcutil_terr(
loc, _tag_arr("bad attribute %s for function", sizeof( unsigned char), 30u),
_tag_arr( _temp188, sizeof( void*), 1u));}}); goto _LL181; _LL187: goto _LL181;
_LL181:;}}{ struct _handler_cons _temp190; _push_handler(& _temp190);{ int
_temp192= 0; if( setjmp( _temp190.handler)){ _temp192= 1;} if( ! _temp192){{
struct _tuple3* _temp193=(( struct _tuple3*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup)( ge->ordinaries, v); void* _temp194=(*
_temp193).f1; void* _temp204; _LL196: if(*(( int*) _temp194) == Cyc_Tcenv_VarRes){
_LL205: _temp204=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp194)->f1; goto
_LL197;} else{ goto _LL198;} _LL198: if(*(( int*) _temp194) == Cyc_Tcenv_StructRes){
goto _LL199;} else{ goto _LL200;} _LL200: if(*(( int*) _temp194) == Cyc_Tcenv_TunionRes){
goto _LL201;} else{ goto _LL202;} _LL202: if(*(( int*) _temp194) == Cyc_Tcenv_EnumRes){
goto _LL203;} else{ goto _LL195;} _LL197: { struct Cyc_Absyn_Funname_b_struct*
_temp206=({ struct Cyc_Absyn_Funname_b_struct* _temp211=( struct Cyc_Absyn_Funname_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct)); _temp211[ 0]=({ struct
Cyc_Absyn_Funname_b_struct _temp212; _temp212.tag= Cyc_Absyn_Funname_b; _temp212.f1=
fd; _temp212;}); _temp211;}); void* _temp207= Cyc_Tcdecl_merge_binding( _temp204,(
void*) _temp206, loc, Cyc_Tc_tc_msg); if( _temp207 ==( void*) Cyc_Absyn_Unresolved_b){
goto _LL195;} if( _temp207 == _temp204?(* _temp193).f2: 0){ goto _LL195;} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple3*
_temp208=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp208->f1=(
void*)({ struct Cyc_Tcenv_VarRes_struct* _temp209=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp209[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp210; _temp210.tag= Cyc_Tcenv_VarRes; _temp210.f1=( void*) _temp207;
_temp210;}); _temp209;}); _temp208->f2= 1; _temp208;})); goto _LL195;} _LL199:({
void* _temp213[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("function declaration shadows previous struct declaration",
sizeof( unsigned char), 57u), _tag_arr( _temp213, sizeof( void*), 0u));}); goto
_LL195; _LL201:({ void* _temp214[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("function declaration shadows previous [x]tunion constructor",
sizeof( unsigned char), 60u), _tag_arr( _temp214, sizeof( void*), 0u));}); goto
_LL195; _LL203:({ void* _temp215[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("function declaration shadows previous enum tag",
sizeof( unsigned char), 47u), _tag_arr( _temp215, sizeof( void*), 0u));}); goto
_LL195; _LL195:;}; _pop_handler();} else{ void* _temp191=( void*) _exn_thrown;
void* _temp217= _temp191; _LL219: if( _temp217 == Cyc_Dict_Absent){ goto _LL220;}
else{ goto _LL221;} _LL221: goto _LL222; _LL220: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple3* _temp223=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp223->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp224=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp224[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp225; _temp225.tag= Cyc_Tcenv_VarRes;
_temp225.f1=( void*)(( void*)({ struct Cyc_Absyn_Funname_b_struct* _temp226=(
struct Cyc_Absyn_Funname_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct));
_temp226[ 0]=({ struct Cyc_Absyn_Funname_b_struct _temp227; _temp227.tag= Cyc_Absyn_Funname_b;
_temp227.f1= fd; _temp227;}); _temp226;})); _temp225;}); _temp224;}); _temp223->f2=
0; _temp223;})); goto _LL218; _LL222:( void) _throw( _temp217); _LL218:;}}} te->le=({
struct Cyc_Core_Opt* _temp228=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp228->v=( void*)(( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp229=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp229[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp230; _temp230.tag= Cyc_Tcenv_Outermost;
_temp230.f1=( void*) Cyc_Tcenv_new_fenv( loc, fd); _temp230;}); _temp229;}));
_temp228;}); Cyc_Tcstmt_tcStmt( te, fd->body, 0); if( ! Cyc_Tcenv_all_labels_resolved(
te)){({ void* _temp231[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("function has goto statements to undefined labels",
sizeof( unsigned char), 49u), _tag_arr( _temp231, sizeof( void*), 0u));});} te->le=
0;}} static void Cyc_Tc_tcTypedefdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Typedefdecl* td){ struct
_tagged_arr* v=(* td->name).f2;{ void* _temp232=(* td->name).f1; struct Cyc_List_List*
_temp240; struct Cyc_List_List* _temp242; _LL234: if(( unsigned int) _temp232 >
1u?*(( int*) _temp232) == Cyc_Absyn_Rel_n: 0){ _LL241: _temp240=(( struct Cyc_Absyn_Rel_n_struct*)
_temp232)->f1; if( _temp240 == 0){ goto _LL235;} else{ goto _LL236;}} else{ goto
_LL236;} _LL236: if(( unsigned int) _temp232 > 1u?*(( int*) _temp232) == Cyc_Absyn_Abs_n:
0){ _LL243: _temp242=(( struct Cyc_Absyn_Abs_n_struct*) _temp232)->f1; if(
_temp242 == 0){ goto _LL237;} else{ goto _LL238;}} else{ goto _LL238;} _LL238:
goto _LL239; _LL235: goto _LL233; _LL237: goto _LL233; _LL239:({ void* _temp244[
0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("qualified declarations are not implemented",
sizeof( unsigned char), 43u), _tag_arr( _temp244, sizeof( void*), 0u));});
return; _LL233:;} if((( int(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key))
Cyc_Dict_member)( ge->typedefs, v)){({ struct Cyc_Stdio_String_pa_struct
_temp246; _temp246.tag= Cyc_Stdio_String_pa; _temp246.f1=( struct _tagged_arr)*
v;{ void* _temp245[ 1u]={& _temp246}; Cyc_Tcutil_terr( loc, _tag_arr("redeclaration of typedef %s",
sizeof( unsigned char), 28u), _tag_arr( _temp245, sizeof( void*), 1u));}});
return;}(* td->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp247=(
struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp247[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp248; _temp248.tag= Cyc_Absyn_Abs_n;
_temp248.f1= te->ns; _temp248;}); _temp247;}); Cyc_Tcutil_check_unique_tvars(
loc, td->tvs); Cyc_Tcutil_add_tvar_identities( td->tvs); Cyc_Tcutil_check_type(
loc, te, td->tvs,( void*) Cyc_Absyn_AnyKind,( void*) td->defn); ge->typedefs=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Typedefdecl* data)) Cyc_Dict_insert)( ge->typedefs, v, td);}
static void Cyc_Tc_tcStructFields( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct _tagged_arr obj, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs){ struct _RegionHandle _temp249= _new_region();
struct _RegionHandle* uprev_rgn=& _temp249; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0; struct Cyc_List_List* _temp250= fields; for( 0;
_temp250 != 0; _temp250=(( struct Cyc_List_List*) _check_null( _temp250))->tl){
struct Cyc_Absyn_Structfield _temp253; struct Cyc_List_List* _temp254; struct
Cyc_Core_Opt* _temp256; void* _temp258; struct Cyc_Absyn_Tqual _temp260; struct
_tagged_arr* _temp262; struct Cyc_Absyn_Structfield* _temp251=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp250))->hd; _temp253=* _temp251; _LL263:
_temp262= _temp253.name; goto _LL261; _LL261: _temp260= _temp253.tq; goto _LL259;
_LL259: _temp258=( void*) _temp253.type; goto _LL257; _LL257: _temp256= _temp253.width;
goto _LL255; _LL255: _temp254= _temp253.attributes; goto _LL252; _LL252: if(((
int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List*
l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,
_temp262)){({ struct Cyc_Stdio_String_pa_struct _temp266; _temp266.tag= Cyc_Stdio_String_pa;
_temp266.f1=( struct _tagged_arr) obj;{ struct Cyc_Stdio_String_pa_struct
_temp265; _temp265.tag= Cyc_Stdio_String_pa; _temp265.f1=( struct _tagged_arr)*
_temp262;{ void* _temp264[ 2u]={& _temp265,& _temp266}; Cyc_Tcutil_terr( loc,
_tag_arr("duplicate field %s in %s", sizeof( unsigned char), 25u), _tag_arr(
_temp264, sizeof( void*), 2u));}}});} if( Cyc_String_strcmp(* _temp262, _tag_arr("",
sizeof( unsigned char), 1u)) != 0){ prev_fields=({ struct Cyc_List_List*
_temp267=( struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp267->hd=( void*) _temp262; _temp267->tl= prev_fields; _temp267;});} Cyc_Tcutil_check_type(
loc, te, tvs,( void*) Cyc_Absyn_MemKind, _temp258); if( _temp256 != 0){
unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp256))->v)){({ struct Cyc_Stdio_String_pa_struct
_temp269; _temp269.tag= Cyc_Stdio_String_pa; _temp269.f1=( struct _tagged_arr)*
_temp262;{ void* _temp268[ 1u]={& _temp269}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield %s does not have constant width",
sizeof( unsigned char), 41u), _tag_arr( _temp268, sizeof( void*), 1u));}});}
else{ w= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*)
_check_null( _temp256))->v);}{ void* _temp270= Cyc_Tcutil_compress( _temp258);
void* _temp276; _LL272: if(( unsigned int) _temp270 > 4u?*(( int*) _temp270) ==
Cyc_Absyn_IntType: 0){ _LL277: _temp276=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp270)->f2; goto _LL273;} else{ goto _LL274;} _LL274: goto _LL275; _LL273:{
void* _temp278= _temp276; _LL280: if( _temp278 ==( void*) Cyc_Absyn_B1){ goto
_LL281;} else{ goto _LL282;} _LL282: if( _temp278 ==( void*) Cyc_Absyn_B2){ goto
_LL283;} else{ goto _LL284;} _LL284: if( _temp278 ==( void*) Cyc_Absyn_B4){ goto
_LL285;} else{ goto _LL286;} _LL286: if( _temp278 ==( void*) Cyc_Absyn_B8){ goto
_LL287;} else{ goto _LL279;} _LL281: if( w > 8){({ void* _temp288[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u),
_tag_arr( _temp288, sizeof( void*), 0u));});} goto _LL279; _LL283: if( w > 16){({
void* _temp289[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u), _tag_arr( _temp289, sizeof( void*), 0u));});} goto
_LL279; _LL285: if( w > 32){({ void* _temp290[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("bitfield larger than type", sizeof( unsigned char), 26u), _tag_arr(
_temp290, sizeof( void*), 0u));});} goto _LL279; _LL287: if( w > 64){({ void*
_temp291[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u), _tag_arr( _temp291, sizeof( void*), 0u));});} goto
_LL279; _LL279:;} goto _LL271; _LL275:({ struct Cyc_Stdio_String_pa_struct
_temp294; _temp294.tag= Cyc_Stdio_String_pa; _temp294.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( _temp258);{ struct Cyc_Stdio_String_pa_struct _temp293;
_temp293.tag= Cyc_Stdio_String_pa; _temp293.f1=( struct _tagged_arr)* _temp262;{
void* _temp292[ 2u]={& _temp293,& _temp294}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield %s must have integral type but has type %s",
sizeof( unsigned char), 52u), _tag_arr( _temp292, sizeof( void*), 2u));}}});
goto _LL271; _LL271:;}}}}; _pop_region( uprev_rgn);} static void Cyc_Tc_tcStructOrUniondecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct _tagged_arr obj,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl* sd){ struct
_tagged_arr* _temp295=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
sd->name))->v)).f2; if( sd->name == 0){({ struct Cyc_Stdio_String_pa_struct
_temp297; _temp297.tag= Cyc_Stdio_String_pa; _temp297.f1=( struct _tagged_arr)
obj;{ void* _temp296[ 1u]={& _temp297}; Cyc_Tcutil_terr( loc, _tag_arr("anonymous %ss are not allowed at top level",
sizeof( unsigned char), 43u), _tag_arr( _temp296, sizeof( void*), 1u));}});
return;}{ struct Cyc_List_List* atts= sd->attributes; for( 0; atts != 0; atts=((
struct Cyc_List_List*) _check_null( atts))->tl){ void* _temp298=( void*)((
struct Cyc_List_List*) _check_null( atts))->hd; _LL300: if( _temp298 ==( void*)
Cyc_Absyn_Packed_att){ goto _LL301;} else{ goto _LL302;} _LL302: if((
unsigned int) _temp298 > 16u?*(( int*) _temp298) == Cyc_Absyn_Aligned_att: 0){
goto _LL303;} else{ goto _LL304;} _LL304: goto _LL305; _LL301: continue; _LL303:
continue; _LL305:({ struct Cyc_Stdio_String_pa_struct _temp309; _temp309.tag=
Cyc_Stdio_String_pa; _temp309.f1=( struct _tagged_arr)* _temp295;{ struct Cyc_Stdio_String_pa_struct
_temp308; _temp308.tag= Cyc_Stdio_String_pa; _temp308.f1=( struct _tagged_arr)
obj;{ struct Cyc_Stdio_String_pa_struct _temp307; _temp307.tag= Cyc_Stdio_String_pa;
_temp307.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*)(( struct
Cyc_List_List*) _check_null( atts))->hd);{ void* _temp306[ 3u]={& _temp307,&
_temp308,& _temp309}; Cyc_Tcutil_terr( loc, _tag_arr("bad attribute %s in %s %s definition",
sizeof( unsigned char), 37u), _tag_arr( _temp306, sizeof( void*), 3u));}}}});
goto _LL299; _LL299:;}}{ struct Cyc_List_List* tvs= sd->tvs; for( 0; tvs != 0;
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ struct Cyc_Absyn_Conref*
_temp310= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd)->kind); void* _temp311=( void*) _temp310->v; void*
_temp319; _LL313: if( _temp311 ==( void*) Cyc_Absyn_No_constr){ goto _LL314;}
else{ goto _LL315;} _LL315: if(( unsigned int) _temp311 > 1u?*(( int*) _temp311)
== Cyc_Absyn_Eq_constr: 0){ _LL320: _temp319=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp311)->f1; if( _temp319 ==( void*) Cyc_Absyn_MemKind){ goto _LL316;} else{
goto _LL317;}} else{ goto _LL317;} _LL317: goto _LL318; _LL314:( void*)(
_temp310->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp321=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp321[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp322; _temp322.tag= Cyc_Absyn_Eq_constr;
_temp322.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp322;}); _temp321;})));
continue; _LL316:({ struct Cyc_Stdio_String_pa_struct _temp326; _temp326.tag=
Cyc_Stdio_String_pa; _temp326.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->name;{ struct Cyc_Stdio_String_pa_struct
_temp325; _temp325.tag= Cyc_Stdio_String_pa; _temp325.f1=( struct _tagged_arr)*
_temp295;{ struct Cyc_Stdio_String_pa_struct _temp324; _temp324.tag= Cyc_Stdio_String_pa;
_temp324.f1=( struct _tagged_arr) obj;{ void* _temp323[ 3u]={& _temp324,&
_temp325,& _temp326}; Cyc_Tcutil_terr( loc, _tag_arr("%s %s attempts to abstract type variable %s of kind M",
sizeof( unsigned char), 54u), _tag_arr( _temp323, sizeof( void*), 3u));}}}});
continue; _LL318: continue; _LL312:;}}{ void* _temp327=(*(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( sd->name))->v)).f1; struct Cyc_List_List*
_temp335; struct Cyc_List_List* _temp337; _LL329: if(( unsigned int) _temp327 >
1u?*(( int*) _temp327) == Cyc_Absyn_Rel_n: 0){ _LL336: _temp335=(( struct Cyc_Absyn_Rel_n_struct*)
_temp327)->f1; if( _temp335 == 0){ goto _LL330;} else{ goto _LL331;}} else{ goto
_LL331;} _LL331: if(( unsigned int) _temp327 > 1u?*(( int*) _temp327) == Cyc_Absyn_Abs_n:
0){ _LL338: _temp337=(( struct Cyc_Absyn_Abs_n_struct*) _temp327)->f1; if(
_temp337 == 0){ goto _LL332;} else{ goto _LL333;}} else{ goto _LL333;} _LL333:
goto _LL334; _LL330: goto _LL328; _LL332: goto _LL328; _LL334:({ void* _temp339[
0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("qualified declarations are not implemented",
sizeof( unsigned char), 43u), _tag_arr( _temp339, sizeof( void*), 0u));});
return; _LL328:;}(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp340=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp340[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp341; _temp341.tag= Cyc_Absyn_Abs_n; _temp341.f1= te->ns; _temp341;});
_temp340;}); Cyc_Tcutil_check_unique_tvars( loc, sd->tvs); Cyc_Tcutil_add_tvar_identities(
sd->tvs);} struct _tuple4{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ;
static void Cyc_Tc_tcStructdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl* sd){ struct
_tagged_arr* _temp342=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
sd->name))->v)).f2; const unsigned char* _temp343="struct"; Cyc_Tc_tcStructOrUniondecl(
te, ge, _tag_arr( _temp343, sizeof( unsigned char), 7u), loc, sd);{ struct Cyc_List_List*
_temp344= sd->tvs;{ struct _tuple4 _temp346=({ struct _tuple4 _temp345; _temp345.f1=
sd->fields; _temp345.f2=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup_opt)( ge->structdecls, _temp342);
_temp345;}); struct Cyc_Core_Opt* _temp356; struct Cyc_Core_Opt* _temp358;
struct Cyc_Core_Opt* _temp360; struct Cyc_Core_Opt* _temp362; struct Cyc_Core_Opt
_temp364; struct Cyc_List_List* _temp365; struct Cyc_Core_Opt* _temp367; struct
Cyc_Core_Opt _temp369; struct Cyc_Absyn_Structdecl** _temp370; struct Cyc_Core_Opt*
_temp372; struct Cyc_Core_Opt _temp374; struct Cyc_List_List* _temp375; struct
Cyc_Core_Opt* _temp377; struct Cyc_Core_Opt _temp379; struct Cyc_Absyn_Structdecl**
_temp380; struct Cyc_Core_Opt* _temp382; _LL348: _LL359: _temp358= _temp346.f1;
if( _temp358 == 0){ goto _LL357;} else{ goto _LL350;} _LL357: _temp356= _temp346.f2;
if( _temp356 == 0){ goto _LL349;} else{ goto _LL350;} _LL350: _LL363: _temp362=
_temp346.f1; if( _temp362 == 0){ goto _LL352;} else{ _temp364=* _temp362; _LL366:
_temp365=( struct Cyc_List_List*) _temp364.v; goto _LL361;} _LL361: _temp360=
_temp346.f2; if( _temp360 == 0){ goto _LL351;} else{ goto _LL352;} _LL352:
_LL373: _temp372= _temp346.f1; if( _temp372 == 0){ goto _LL354;} else{ _temp374=*
_temp372; _LL376: _temp375=( struct Cyc_List_List*) _temp374.v; goto _LL368;}
_LL368: _temp367= _temp346.f2; if( _temp367 == 0){ goto _LL354;} else{ _temp369=*
_temp367; _LL371: _temp370=( struct Cyc_Absyn_Structdecl**) _temp369.v; goto
_LL353;} _LL354: _LL383: _temp382= _temp346.f1; if( _temp382 == 0){ goto _LL378;}
else{ goto _LL347;} _LL378: _temp377= _temp346.f2; if( _temp377 == 0){ goto
_LL347;} else{ _temp379=* _temp377; _LL381: _temp380=( struct Cyc_Absyn_Structdecl**)
_temp379.v; goto _LL355;} _LL349: ge->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl**
data)) Cyc_Dict_insert)( ge->structdecls, _temp342,({ struct Cyc_Absyn_Structdecl**
_temp384=( struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp384[ 0]= sd; _temp384;})); goto _LL347; _LL351: { struct Cyc_Absyn_Structdecl**
_temp385=({ struct Cyc_Absyn_Structdecl** _temp386=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp386[ 0]=({ struct Cyc_Absyn_Structdecl*
_temp387=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp387->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp387->name= sd->name;
_temp387->tvs= _temp344; _temp387->fields= 0; _temp387->attributes= 0; _temp387;});
_temp386;}); ge->structdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl** data)) Cyc_Dict_insert)(
ge->structdecls, _temp342, _temp385); Cyc_Tc_tcStructFields( te, ge, loc,
_tag_arr( _temp343, sizeof( unsigned char), 7u), _temp365, _temp344);* _temp385=
sd; goto _LL347;} _LL353: { struct Cyc_Absyn_Structdecl* _temp388=* _temp370;*
_temp370=({ struct Cyc_Absyn_Structdecl* _temp389=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp389->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp389->name= sd->name; _temp389->tvs= _temp344; _temp389->fields=
0; _temp389->attributes= 0; _temp389;}); Cyc_Tc_tcStructFields( te, ge, loc,
_tag_arr( _temp343, sizeof( unsigned char), 7u), _temp375, _temp344);* _temp370=
_temp388; _temp380= _temp370; goto _LL355;} _LL355: { struct Cyc_Absyn_Structdecl*
_temp390= Cyc_Tcdecl_merge_structdecl(* _temp380, sd, loc, Cyc_Tc_tc_msg); if(
_temp390 == 0){ return;} else{* _temp380=( struct Cyc_Absyn_Structdecl*)
_check_null( _temp390); sd=( struct Cyc_Absyn_Structdecl*) _check_null( _temp390);
goto _LL347;}} _LL347:;} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,
_temp342,({ struct _tuple3* _temp391=( struct _tuple3*) GC_malloc( sizeof(
struct _tuple3)); _temp391->f1=( void*)({ struct Cyc_Tcenv_StructRes_struct*
_temp392=( struct Cyc_Tcenv_StructRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_StructRes_struct));
_temp392[ 0]=({ struct Cyc_Tcenv_StructRes_struct _temp393; _temp393.tag= Cyc_Tcenv_StructRes;
_temp393.f1= sd; _temp393;}); _temp392;}); _temp391->f2= 1; _temp391;}));}}
static void Cyc_Tc_tcUniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl* ud){ struct
_tagged_arr* _temp394=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
ud->name))->v)).f2; const unsigned char* _temp395="union"; Cyc_Tc_tcStructOrUniondecl(
te, ge, _tag_arr( _temp395, sizeof( unsigned char), 6u), loc,( struct Cyc_Absyn_Structdecl*)
ud);{ struct Cyc_List_List* _temp396= ud->tvs; struct _tuple4 _temp398=({ struct
_tuple4 _temp397; _temp397.f1= ud->fields; _temp397.f2=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)( ge->uniondecls,
_temp394); _temp397;}); struct Cyc_Core_Opt* _temp408; struct Cyc_Core_Opt*
_temp410; struct Cyc_Core_Opt* _temp412; struct Cyc_Core_Opt* _temp414; struct
Cyc_Core_Opt _temp416; struct Cyc_List_List* _temp417; struct Cyc_Core_Opt*
_temp419; struct Cyc_Core_Opt _temp421; struct Cyc_Absyn_Uniondecl** _temp422;
struct Cyc_Core_Opt* _temp424; struct Cyc_Core_Opt _temp426; struct Cyc_List_List*
_temp427; struct Cyc_Core_Opt* _temp429; struct Cyc_Core_Opt _temp431; struct
Cyc_Absyn_Uniondecl** _temp432; struct Cyc_Core_Opt* _temp434; _LL400: _LL411:
_temp410= _temp398.f1; if( _temp410 == 0){ goto _LL409;} else{ goto _LL402;}
_LL409: _temp408= _temp398.f2; if( _temp408 == 0){ goto _LL401;} else{ goto
_LL402;} _LL402: _LL415: _temp414= _temp398.f1; if( _temp414 == 0){ goto _LL404;}
else{ _temp416=* _temp414; _LL418: _temp417=( struct Cyc_List_List*) _temp416.v;
goto _LL413;} _LL413: _temp412= _temp398.f2; if( _temp412 == 0){ goto _LL403;}
else{ goto _LL404;} _LL404: _LL425: _temp424= _temp398.f1; if( _temp424 == 0){
goto _LL406;} else{ _temp426=* _temp424; _LL428: _temp427=( struct Cyc_List_List*)
_temp426.v; goto _LL420;} _LL420: _temp419= _temp398.f2; if( _temp419 == 0){
goto _LL406;} else{ _temp421=* _temp419; _LL423: _temp422=( struct Cyc_Absyn_Uniondecl**)
_temp421.v; goto _LL405;} _LL406: _LL435: _temp434= _temp398.f1; if( _temp434 ==
0){ goto _LL430;} else{ goto _LL399;} _LL430: _temp429= _temp398.f2; if(
_temp429 == 0){ goto _LL399;} else{ _temp431=* _temp429; _LL433: _temp432=(
struct Cyc_Absyn_Uniondecl**) _temp431.v; goto _LL407;} _LL401: ge->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)( ge->uniondecls, _temp394,({
struct Cyc_Absyn_Uniondecl** _temp436=( struct Cyc_Absyn_Uniondecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl*)); _temp436[ 0]= ud; _temp436;})); goto
_LL399; _LL403: { struct Cyc_Absyn_Uniondecl** _temp437=({ struct Cyc_Absyn_Uniondecl**
_temp442=( struct Cyc_Absyn_Uniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl*));
_temp442[ 0]=({ struct Cyc_Absyn_Uniondecl* _temp443=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp443->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp443->name= ud->name; _temp443->tvs= _temp396; _temp443->fields=
0; _temp443->attributes= ud->attributes; _temp443;}); _temp442;}); ge->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)( ge->uniondecls, _temp394,
_temp437); Cyc_Tc_tcStructFields( te, ge, loc, _tag_arr( _temp395, sizeof(
unsigned char), 6u), _temp417, _temp396);{ struct Cyc_List_List* f= _temp417;
for( 0; f != 0; f=(( struct Cyc_List_List*) _check_null( f))->tl){ if( ! Cyc_Tcutil_bits_only((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( f))->hd)->type)){({
struct Cyc_Stdio_String_pa_struct _temp441; _temp441.tag= Cyc_Stdio_String_pa;
_temp441.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( f))->hd)->type);{ struct Cyc_Stdio_String_pa_struct
_temp440; _temp440.tag= Cyc_Stdio_String_pa; _temp440.f1=( struct _tagged_arr)*
_temp394;{ struct Cyc_Stdio_String_pa_struct _temp439; _temp439.tag= Cyc_Stdio_String_pa;
_temp439.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( f))->hd)->name;{ void* _temp438[ 3u]={& _temp439,& _temp440,&
_temp441}; Cyc_Tcutil_terr( loc, _tag_arr("field %s of union %s has type %s which is not `bits-only'",
sizeof( unsigned char), 58u), _tag_arr( _temp438, sizeof( void*), 3u));}}}});}}}*
_temp437= ud; goto _LL399;} _LL405: { struct Cyc_Absyn_Uniondecl* _temp444=*
_temp422;* _temp422=({ struct Cyc_Absyn_Uniondecl* _temp445=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp445->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp445->name= ud->name; _temp445->tvs= _temp396; _temp445->fields=
0; _temp445->attributes= ud->attributes; _temp445;}); Cyc_Tc_tcStructFields( te,
ge, loc, _tag_arr( _temp395, sizeof( unsigned char), 6u), _temp427, _temp396);*
_temp422= _temp444; _temp432= _temp422; goto _LL407;} _LL407: { struct Cyc_Absyn_Uniondecl*
_temp446= Cyc_Tcdecl_merge_uniondecl(* _temp432, ud, loc, Cyc_Tc_tc_msg); if(
_temp446 == 0){ return;} else{* _temp432=( struct Cyc_Absyn_Uniondecl*)
_check_null( _temp446); goto _LL399;}} _LL399:;}} struct _tuple5{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; static struct Cyc_List_List* Cyc_Tc_tcTunionFields( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct
_tagged_arr obj, int is_xtunion, struct _tuple0* name, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs, struct Cyc_Absyn_Tuniondecl* tudres){{ struct
Cyc_List_List* _temp447= fields; for( 0; _temp447 != 0; _temp447=(( struct Cyc_List_List*)
_check_null( _temp447))->tl){ struct Cyc_Absyn_Tunionfield* _temp448=( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp447))->hd;{
struct Cyc_List_List* tvs= _temp448->tvs; for( 0; tvs != 0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){ struct Cyc_Absyn_Tvar* _temp449=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd; struct Cyc_Absyn_Conref* _temp450=
Cyc_Absyn_compress_conref( _temp449->kind); void* _temp451=( void*) _temp450->v;
void* _temp459; _LL453: if( _temp451 ==( void*) Cyc_Absyn_No_constr){ goto
_LL454;} else{ goto _LL455;} _LL455: if(( unsigned int) _temp451 > 1u?*(( int*)
_temp451) == Cyc_Absyn_Eq_constr: 0){ _LL460: _temp459=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp451)->f1; if( _temp459 ==( void*) Cyc_Absyn_MemKind){ goto _LL456;} else{
goto _LL457;}} else{ goto _LL457;} _LL457: goto _LL458; _LL454:( void*)(
_temp450->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp461=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp461[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp462; _temp462.tag= Cyc_Absyn_Eq_constr;
_temp462.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp462;}); _temp461;})));
goto _LL452; _LL456:({ struct Cyc_Stdio_String_pa_struct _temp464; _temp464.tag=
Cyc_Stdio_String_pa; _temp464.f1=( struct _tagged_arr)*(* _temp448->name).f2;{
void* _temp463[ 1u]={& _temp464}; Cyc_Tcutil_terr( _temp448->loc, _tag_arr("field %s abstracts type variable of kind M",
sizeof( unsigned char), 43u), _tag_arr( _temp463, sizeof( void*), 1u));}}); goto
_LL452; _LL458: goto _LL452; _LL452:;}}{ struct Cyc_List_List* alltvs=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tvs, _temp448->tvs); Cyc_Tcutil_check_unique_tvars( loc, alltvs); Cyc_Tcutil_add_tvar_identities(
_temp448->tvs);{ struct Cyc_List_List* typs= _temp448->typs; for( 0; typs != 0;
typs=(( struct Cyc_List_List*) _check_null( typs))->tl){ Cyc_Tcutil_check_type(
_temp448->loc, te, alltvs,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple5*)((
struct Cyc_List_List*) _check_null( typs))->hd)).f2);}}{ void* _temp465=(*
_temp448->name).f1; struct Cyc_List_List* _temp475; _LL467: if(( unsigned int)
_temp465 > 1u?*(( int*) _temp465) == Cyc_Absyn_Rel_n: 0){ _LL476: _temp475=((
struct Cyc_Absyn_Rel_n_struct*) _temp465)->f1; if( _temp475 == 0){ goto _LL468;}
else{ goto _LL469;}} else{ goto _LL469;} _LL469: if(( unsigned int) _temp465 > 1u?*((
int*) _temp465) == Cyc_Absyn_Rel_n: 0){ goto _LL470;} else{ goto _LL471;} _LL471:
if(( unsigned int) _temp465 > 1u?*(( int*) _temp465) == Cyc_Absyn_Abs_n: 0){
goto _LL472;} else{ goto _LL473;} _LL473: if( _temp465 ==( void*) Cyc_Absyn_Loc_n){
goto _LL474;} else{ goto _LL466;} _LL468: if( is_xtunion){(* _temp448->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp477=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp477[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp478; _temp478.tag= Cyc_Absyn_Abs_n; _temp478.f1= te->ns; _temp478;});
_temp477;});} else{(* _temp448->name).f1=(* name).f1;} goto _LL466; _LL470:({
void* _temp479[ 0u]={}; Cyc_Tcutil_terr( _temp448->loc, _tag_arr("qualified declarations are not allowed",
sizeof( unsigned char), 39u), _tag_arr( _temp479, sizeof( void*), 0u));}); goto
_LL466; _LL472: goto _LL466; _LL474:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp480=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp480[ 0]=({ struct Cyc_Core_Impossible_struct _temp481; _temp481.tag= Cyc_Core_Impossible;
_temp481.f1= _tag_arr("tcTunionFields: Loc_n", sizeof( unsigned char), 22u);
_temp481;}); _temp480;})); goto _LL466; _LL466:;}}}}{ struct Cyc_List_List*
fields2; if( is_xtunion){ int _temp482= 1; struct Cyc_List_List* _temp483= Cyc_Tcdecl_sort_xtunion_fields(
fields,& _temp482,(* name).f2, loc, Cyc_Tc_tc_msg); if( _temp482){ fields2=
_temp483;} else{ fields2= 0;}} else{ struct _RegionHandle _temp484= _new_region();
struct _RegionHandle* uprev_rgn=& _temp484; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0;{ struct Cyc_List_List* fs= fields; for( 0; fs !=
0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){ struct Cyc_Absyn_Tunionfield*
_temp485=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fs))->hd; if((( int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*),
struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp,
prev_fields,(* _temp485->name).f2)){({ struct Cyc_Stdio_String_pa_struct
_temp488; _temp488.tag= Cyc_Stdio_String_pa; _temp488.f1=( struct _tagged_arr)
obj;{ struct Cyc_Stdio_String_pa_struct _temp487; _temp487.tag= Cyc_Stdio_String_pa;
_temp487.f1=( struct _tagged_arr)*(* _temp485->name).f2;{ void* _temp486[ 2u]={&
_temp487,& _temp488}; Cyc_Tcutil_terr( _temp485->loc, _tag_arr("duplicate field name %s in %s",
sizeof( unsigned char), 30u), _tag_arr( _temp486, sizeof( void*), 2u));}}});}
else{ prev_fields=({ struct Cyc_List_List* _temp489=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp489->hd=( void*)(*
_temp485->name).f2; _temp489->tl= prev_fields; _temp489;});} if(( void*)
_temp485->sc !=( void*) Cyc_Absyn_Public){({ struct Cyc_Stdio_String_pa_struct
_temp491; _temp491.tag= Cyc_Stdio_String_pa; _temp491.f1=( struct _tagged_arr)*(*
_temp485->name).f2;{ void* _temp490[ 1u]={& _temp491}; Cyc_Tcutil_warn( loc,
_tag_arr("ignoring scope of field %s", sizeof( unsigned char), 27u), _tag_arr(
_temp490, sizeof( void*), 1u));}});( void*)( _temp485->sc=( void*)(( void*) Cyc_Absyn_Public));}}}
fields2= fields;}; _pop_region( uprev_rgn);}{ struct Cyc_List_List* _temp492=
fields; for( 0; _temp492 != 0; _temp492=(( struct Cyc_List_List*) _check_null(
_temp492))->tl){ struct Cyc_Absyn_Tunionfield* _temp493=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp492))->hd; ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries,(* _temp493->name).f2,({ struct _tuple3* _temp494=( struct
_tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp494->f1=( void*)({ struct
Cyc_Tcenv_TunionRes_struct* _temp495=( struct Cyc_Tcenv_TunionRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct)); _temp495[ 0]=({ struct
Cyc_Tcenv_TunionRes_struct _temp496; _temp496.tag= Cyc_Tcenv_TunionRes; _temp496.f1=
tudres; _temp496.f2= _temp493; _temp496;}); _temp495;}); _temp494->f2= 1;
_temp494;}));}} return fields2;}} static void Cyc_Tc_tcTuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl*
tud){ struct _tagged_arr* v=(* tud->name).f2; struct _tagged_arr obj; if( tud->is_xtunion){
obj= _tag_arr("xtunion", sizeof( unsigned char), 8u);} else{ obj= _tag_arr("tunion",
sizeof( unsigned char), 7u);}{ struct Cyc_List_List* tvs= tud->tvs;{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 != 0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
struct Cyc_Absyn_Conref* c= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->kind); void* _temp497=( void*)
c->v; void* _temp505; _LL499: if( _temp497 ==( void*) Cyc_Absyn_No_constr){ goto
_LL500;} else{ goto _LL501;} _LL501: if(( unsigned int) _temp497 > 1u?*(( int*)
_temp497) == Cyc_Absyn_Eq_constr: 0){ _LL506: _temp505=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp497)->f1; if( _temp505 ==( void*) Cyc_Absyn_MemKind){ goto _LL502;} else{
goto _LL503;}} else{ goto _LL503;} _LL503: goto _LL504; _LL500:( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp507=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp507[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp508; _temp508.tag= Cyc_Absyn_Eq_constr; _temp508.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp508;}); _temp507;}))); goto _LL498;
_LL502:({ struct Cyc_Stdio_String_pa_struct _temp512; _temp512.tag= Cyc_Stdio_String_pa;
_temp512.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->name;{ struct Cyc_Stdio_String_pa_struct _temp511;
_temp511.tag= Cyc_Stdio_String_pa; _temp511.f1=( struct _tagged_arr)* v;{ struct
Cyc_Stdio_String_pa_struct _temp510; _temp510.tag= Cyc_Stdio_String_pa; _temp510.f1=(
struct _tagged_arr) obj;{ void* _temp509[ 3u]={& _temp510,& _temp511,& _temp512};
Cyc_Tcutil_terr( loc, _tag_arr("%s %s attempts to abstract type variable %s of kind M",
sizeof( unsigned char), 54u), _tag_arr( _temp509, sizeof( void*), 3u));}}}});
goto _LL498; _LL504: goto _LL498; _LL498:;}} Cyc_Tcutil_check_unique_tvars( loc,
tvs); Cyc_Tcutil_add_tvar_identities( tvs);{ struct Cyc_Core_Opt* tud_opt; if(
tud->is_xtunion){{ struct _handler_cons _temp513; _push_handler(& _temp513);{
int _temp515= 0; if( setjmp( _temp513.handler)){ _temp515= 1;} if( ! _temp515){
tud_opt= Cyc_Tcenv_lookup_xtuniondecl( te, loc, tud->name);; _pop_handler();}
else{ void* _temp514=( void*) _exn_thrown; void* _temp517= _temp514; _LL519: if(
_temp517 == Cyc_Dict_Absent){ goto _LL520;} else{ goto _LL521;} _LL521: goto
_LL522; _LL520:({ struct Cyc_Stdio_String_pa_struct _temp524; _temp524.tag= Cyc_Stdio_String_pa;
_temp524.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( tud->name);{ void*
_temp523[ 1u]={& _temp524}; Cyc_Tcutil_terr( loc, _tag_arr("qualified xtunion declaration %s is not an existing xtunion",
sizeof( unsigned char), 60u), _tag_arr( _temp523, sizeof( void*), 1u));}});
return; _LL522:( void) _throw( _temp517); _LL518:;}}} if( tud_opt != 0){ tud->name=(*((
struct Cyc_Absyn_Tuniondecl**)(( struct Cyc_Core_Opt*) _check_null( tud_opt))->v))->name;}
else{(* tud->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp525=(
struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp525[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp526; _temp526.tag= Cyc_Absyn_Abs_n;
_temp526.f1= te->ns; _temp526;}); _temp525;});}} else{{ void* _temp527=(* tud->name).f1;
struct Cyc_List_List* _temp535; _LL529: if(( unsigned int) _temp527 > 1u?*(( int*)
_temp527) == Cyc_Absyn_Rel_n: 0){ _LL536: _temp535=(( struct Cyc_Absyn_Rel_n_struct*)
_temp527)->f1; if( _temp535 == 0){ goto _LL530;} else{ goto _LL531;}} else{ goto
_LL531;} _LL531: if(( unsigned int) _temp527 > 1u?*(( int*) _temp527) == Cyc_Absyn_Abs_n:
0){ goto _LL532;} else{ goto _LL533;} _LL533: goto _LL534; _LL530:(* tud->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp537=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp537[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp538; _temp538.tag= Cyc_Absyn_Abs_n; _temp538.f1= te->ns; _temp538;});
_temp537;}); goto _LL528; _LL532: goto _LL534; _LL534:({ void* _temp539[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("qualified declarations are not implemented",
sizeof( unsigned char), 43u), _tag_arr( _temp539, sizeof( void*), 0u));});
return; _LL528:;} tud_opt=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup_opt)( ge->tuniondecls, v);}{ struct
_tuple4 _temp541=({ struct _tuple4 _temp540; _temp540.f1= tud->fields; _temp540.f2=
tud_opt; _temp540;}); struct Cyc_Core_Opt* _temp551; struct Cyc_Core_Opt*
_temp553; struct Cyc_Core_Opt* _temp555; struct Cyc_Core_Opt* _temp557; struct
Cyc_Core_Opt _temp559; struct Cyc_List_List* _temp560; struct Cyc_List_List**
_temp562; struct Cyc_Core_Opt* _temp563; struct Cyc_Core_Opt _temp565; struct
Cyc_Absyn_Tuniondecl** _temp566; struct Cyc_Core_Opt* _temp568; struct Cyc_Core_Opt
_temp570; struct Cyc_List_List* _temp571; struct Cyc_List_List** _temp573;
struct Cyc_Core_Opt* _temp574; struct Cyc_Core_Opt _temp576; struct Cyc_Absyn_Tuniondecl**
_temp577; struct Cyc_Core_Opt* _temp579; _LL543: _LL554: _temp553= _temp541.f1;
if( _temp553 == 0){ goto _LL552;} else{ goto _LL545;} _LL552: _temp551= _temp541.f2;
if( _temp551 == 0){ goto _LL544;} else{ goto _LL545;} _LL545: _LL558: _temp557=
_temp541.f1; if( _temp557 == 0){ goto _LL547;} else{ _temp559=* _temp557; _LL561:
_temp560=( struct Cyc_List_List*) _temp559.v; _temp562=&(* _temp541.f1).v; goto
_LL556;} _LL556: _temp555= _temp541.f2; if( _temp555 == 0){ goto _LL546;} else{
goto _LL547;} _LL547: _LL569: _temp568= _temp541.f1; if( _temp568 == 0){ goto
_LL549;} else{ _temp570=* _temp568; _LL572: _temp571=( struct Cyc_List_List*)
_temp570.v; _temp573=&(* _temp541.f1).v; goto _LL564;} _LL564: _temp563=
_temp541.f2; if( _temp563 == 0){ goto _LL549;} else{ _temp565=* _temp563; _LL567:
_temp566=( struct Cyc_Absyn_Tuniondecl**) _temp565.v; goto _LL548;} _LL549:
_LL580: _temp579= _temp541.f1; if( _temp579 == 0){ goto _LL575;} else{ goto
_LL542;} _LL575: _temp574= _temp541.f2; if( _temp574 == 0){ goto _LL542;} else{
_temp576=* _temp574; _LL578: _temp577=( struct Cyc_Absyn_Tuniondecl**) _temp576.v;
goto _LL550;} _LL544: ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)(
ge->tuniondecls, v,({ struct Cyc_Absyn_Tuniondecl** _temp581=( struct Cyc_Absyn_Tuniondecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp581[ 0]= tud; _temp581;}));
goto _LL542; _LL546: { struct Cyc_Absyn_Tuniondecl** _temp582=({ struct Cyc_Absyn_Tuniondecl**
_temp583=( struct Cyc_Absyn_Tuniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*));
_temp583[ 0]=({ struct Cyc_Absyn_Tuniondecl* _temp584=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp584->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp584->name= tud->name; _temp584->tvs= tvs; _temp584->fields=
0; _temp584->is_xtunion= tud->is_xtunion; _temp584;}); _temp583;}); ge->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)( ge->tuniondecls, v,
_temp582);* _temp562= Cyc_Tc_tcTunionFields( te, ge, loc, obj, tud->is_xtunion,
tud->name,* _temp562, tvs, tud);* _temp582= tud; goto _LL542;} _LL548: { struct
Cyc_Absyn_Tuniondecl* _temp585=* _temp566;* _temp566=({ struct Cyc_Absyn_Tuniondecl*
_temp586=( struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp586->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp586->name= tud->name;
_temp586->tvs= tvs; _temp586->fields= 0; _temp586->is_xtunion= tud->is_xtunion;
_temp586;});* _temp573= Cyc_Tc_tcTunionFields( te, ge, loc, obj, tud->is_xtunion,
tud->name,* _temp573, tvs, tud);* _temp566= _temp585; _temp577= _temp566; goto
_LL550;} _LL550: { struct Cyc_Absyn_Tuniondecl* _temp587= Cyc_Tcdecl_merge_tuniondecl(*
_temp577, tud, loc, Cyc_Tc_tc_msg); if( _temp587 == 0){ return;} else{* _temp577=(
struct Cyc_Absyn_Tuniondecl*) _check_null( _temp587); goto _LL542;}} _LL542:;}}}}
static void Cyc_Tc_tcEnumdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Enumdecl* ed){ struct
_tagged_arr* v=(* ed->name).f2;{ void* _temp588=(* ed->name).f1; struct Cyc_List_List*
_temp596; struct Cyc_List_List* _temp598; _LL590: if(( unsigned int) _temp588 >
1u?*(( int*) _temp588) == Cyc_Absyn_Rel_n: 0){ _LL597: _temp596=(( struct Cyc_Absyn_Rel_n_struct*)
_temp588)->f1; if( _temp596 == 0){ goto _LL591;} else{ goto _LL592;}} else{ goto
_LL592;} _LL592: if(( unsigned int) _temp588 > 1u?*(( int*) _temp588) == Cyc_Absyn_Abs_n:
0){ _LL599: _temp598=(( struct Cyc_Absyn_Abs_n_struct*) _temp588)->f1; if(
_temp598 == 0){ goto _LL593;} else{ goto _LL594;}} else{ goto _LL594;} _LL594:
goto _LL595; _LL591: goto _LL589; _LL593: goto _LL589; _LL595:({ void* _temp600[
0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("qualified declarations are not implemented",
sizeof( unsigned char), 43u), _tag_arr( _temp600, sizeof( void*), 0u));});
return; _LL589:;}(* ed->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp601=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp601[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp602; _temp602.tag= Cyc_Absyn_Abs_n;
_temp602.f1= te->ns; _temp602;}); _temp601;}); if( ed->fields != 0){ struct
_RegionHandle _temp603= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp603; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0;
unsigned int tag_count= 0; struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( ed->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ struct Cyc_Absyn_Enumfield*
_temp604=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( fs))->hd;
if((( int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*), struct
Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp,
prev_fields,(* _temp604->name).f2)){({ struct Cyc_Stdio_String_pa_struct
_temp606; _temp606.tag= Cyc_Stdio_String_pa; _temp606.f1=( struct _tagged_arr)*(*
_temp604->name).f2;{ void* _temp605[ 1u]={& _temp606}; Cyc_Tcutil_terr( _temp604->loc,
_tag_arr("duplicate field name %s", sizeof( unsigned char), 24u), _tag_arr(
_temp605, sizeof( void*), 1u));}});} else{ prev_fields=({ struct Cyc_List_List*
_temp607=( struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp607->hd=( void*)(* _temp604->name).f2; _temp607->tl= prev_fields; _temp607;});}
if( _temp604->tag == 0){ _temp604->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
tag_count, _temp604->loc);} else{ if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp604->tag))){({ struct Cyc_Stdio_String_pa_struct _temp610;
_temp610.tag= Cyc_Stdio_String_pa; _temp610.f1=( struct _tagged_arr)*(* _temp604->name).f2;{
struct Cyc_Stdio_String_pa_struct _temp609; _temp609.tag= Cyc_Stdio_String_pa;
_temp609.f1=( struct _tagged_arr)* v;{ void* _temp608[ 2u]={& _temp609,&
_temp610}; Cyc_Tcutil_terr( loc, _tag_arr("enum %s, field %s: expression is not constant",
sizeof( unsigned char), 46u), _tag_arr( _temp608, sizeof( void*), 2u));}}});}}{
unsigned int t1= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp604->tag)); tag_count= t1 + 1;(* _temp604->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp611=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp611[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp612; _temp612.tag= Cyc_Absyn_Abs_n; _temp612.f1= te->ns; _temp612;});
_temp611;});}}}; _pop_region( uprev_rgn);}{ struct _handler_cons _temp613;
_push_handler(& _temp613);{ int _temp615= 0; if( setjmp( _temp613.handler)){
_temp615= 1;} if( ! _temp615){{ struct Cyc_Absyn_Enumdecl** _temp616=(( struct
Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
ge->enumdecls, v); struct Cyc_Absyn_Enumdecl* _temp617= Cyc_Tcdecl_merge_enumdecl(*
_temp616, ed, loc, Cyc_Tc_tc_msg); if( _temp617 == 0){ _npop_handler( 0u);
return;}* _temp616=( struct Cyc_Absyn_Enumdecl*) _check_null( _temp617);};
_pop_handler();} else{ void* _temp614=( void*) _exn_thrown; void* _temp619=
_temp614; _LL621: if( _temp619 == Cyc_Dict_Absent){ goto _LL622;} else{ goto
_LL623;} _LL623: goto _LL624; _LL622: { struct Cyc_Absyn_Enumdecl** _temp625=({
struct Cyc_Absyn_Enumdecl** _temp626=( struct Cyc_Absyn_Enumdecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl*)); _temp626[ 0]= ed; _temp626;}); ge->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Enumdecl** data)) Cyc_Dict_insert)( ge->enumdecls, v, _temp625);
goto _LL620;} _LL624:( void) _throw( _temp619); _LL620:;}}} if( ed->fields != 0){
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ed->fields))->v; for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ struct Cyc_Absyn_Enumfield* _temp627=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_List_List*) _check_null( fs))->hd; ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries,(* _temp627->name).f2,({ struct _tuple3* _temp628=( struct
_tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp628->f1=( void*)({ struct
Cyc_Tcenv_EnumRes_struct* _temp629=( struct Cyc_Tcenv_EnumRes_struct*) GC_malloc(
sizeof( struct Cyc_Tcenv_EnumRes_struct)); _temp629[ 0]=({ struct Cyc_Tcenv_EnumRes_struct
_temp630; _temp630.tag= Cyc_Tcenv_EnumRes; _temp630.f1= ed; _temp630.f2=
_temp627; _temp630;}); _temp629;}); _temp628->f2= 1; _temp628;}));}}} static int
Cyc_Tc_okay_externC( struct Cyc_Position_Segment* loc, void* sc){ void* _temp631=
sc; _LL633: if( _temp631 ==( void*) Cyc_Absyn_Static){ goto _LL634;} else{ goto
_LL635;} _LL635: if( _temp631 ==( void*) Cyc_Absyn_Abstract){ goto _LL636;}
else{ goto _LL637;} _LL637: if( _temp631 ==( void*) Cyc_Absyn_Public){ goto
_LL638;} else{ goto _LL639;} _LL639: if( _temp631 ==( void*) Cyc_Absyn_Extern){
goto _LL640;} else{ goto _LL641;} _LL641: if( _temp631 ==( void*) Cyc_Absyn_ExternC){
goto _LL642;} else{ goto _LL632;} _LL634:({ void* _temp643[ 0u]={}; Cyc_Tcutil_warn(
loc, _tag_arr("static declaration nested within extern \"C\"", sizeof(
unsigned char), 44u), _tag_arr( _temp643, sizeof( void*), 0u));}); return 0;
_LL636:({ void* _temp644[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("abstract declaration nested within extern \"C\"",
sizeof( unsigned char), 46u), _tag_arr( _temp644, sizeof( void*), 0u));});
return 0; _LL638: return 1; _LL640: return 1; _LL642:({ void* _temp645[ 0u]={};
Cyc_Tcutil_warn( loc, _tag_arr("nested extern \"C\" declaration", sizeof(
unsigned char), 30u), _tag_arr( _temp645, sizeof( void*), 0u));}); return 1;
_LL632:;} static void Cyc_Tc_tc_decls( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
ds0, int in_externC, int check_var_init){ struct Cyc_Tcenv_Genv* ge=(( struct
Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(
te->ae, te->ns); struct Cyc_List_List* _temp646= ds0; for( 0; _temp646 != 0;
_temp646=(( struct Cyc_List_List*) _check_null( _temp646))->tl){ struct Cyc_Absyn_Decl*
d=( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( _temp646))->hd;
struct Cyc_Position_Segment* loc= d->loc; void* _temp647=( void*) d->r; struct
Cyc_Absyn_Vardecl* _temp673; struct Cyc_Absyn_Fndecl* _temp675; struct Cyc_Absyn_Typedefdecl*
_temp677; struct Cyc_Absyn_Structdecl* _temp679; struct Cyc_Absyn_Uniondecl*
_temp681; struct Cyc_Absyn_Tuniondecl* _temp683; struct Cyc_Absyn_Enumdecl*
_temp685; struct Cyc_List_List* _temp687; struct _tagged_arr* _temp689; struct
Cyc_List_List* _temp691; struct _tuple0* _temp693; struct _tuple0 _temp695;
struct _tagged_arr* _temp696; void* _temp698; struct Cyc_List_List* _temp700;
_LL649: if(*(( int*) _temp647) == Cyc_Absyn_Let_d){ goto _LL650;} else{ goto
_LL651;} _LL651: if(*(( int*) _temp647) == Cyc_Absyn_Letv_d){ goto _LL652;}
else{ goto _LL653;} _LL653: if(*(( int*) _temp647) == Cyc_Absyn_Var_d){ _LL674:
_temp673=(( struct Cyc_Absyn_Var_d_struct*) _temp647)->f1; goto _LL654;} else{
goto _LL655;} _LL655: if(*(( int*) _temp647) == Cyc_Absyn_Fn_d){ _LL676:
_temp675=(( struct Cyc_Absyn_Fn_d_struct*) _temp647)->f1; goto _LL656;} else{
goto _LL657;} _LL657: if(*(( int*) _temp647) == Cyc_Absyn_Typedef_d){ _LL678:
_temp677=(( struct Cyc_Absyn_Typedef_d_struct*) _temp647)->f1; goto _LL658;}
else{ goto _LL659;} _LL659: if(*(( int*) _temp647) == Cyc_Absyn_Struct_d){
_LL680: _temp679=(( struct Cyc_Absyn_Struct_d_struct*) _temp647)->f1; goto
_LL660;} else{ goto _LL661;} _LL661: if(*(( int*) _temp647) == Cyc_Absyn_Union_d){
_LL682: _temp681=(( struct Cyc_Absyn_Union_d_struct*) _temp647)->f1; goto _LL662;}
else{ goto _LL663;} _LL663: if(*(( int*) _temp647) == Cyc_Absyn_Tunion_d){
_LL684: _temp683=(( struct Cyc_Absyn_Tunion_d_struct*) _temp647)->f1; goto
_LL664;} else{ goto _LL665;} _LL665: if(*(( int*) _temp647) == Cyc_Absyn_Enum_d){
_LL686: _temp685=(( struct Cyc_Absyn_Enum_d_struct*) _temp647)->f1; goto _LL666;}
else{ goto _LL667;} _LL667: if(*(( int*) _temp647) == Cyc_Absyn_Namespace_d){
_LL690: _temp689=(( struct Cyc_Absyn_Namespace_d_struct*) _temp647)->f1; goto
_LL688; _LL688: _temp687=(( struct Cyc_Absyn_Namespace_d_struct*) _temp647)->f2;
goto _LL668;} else{ goto _LL669;} _LL669: if(*(( int*) _temp647) == Cyc_Absyn_Using_d){
_LL694: _temp693=(( struct Cyc_Absyn_Using_d_struct*) _temp647)->f1; _temp695=*
_temp693; _LL699: _temp698= _temp695.f1; goto _LL697; _LL697: _temp696= _temp695.f2;
goto _LL692; _LL692: _temp691=(( struct Cyc_Absyn_Using_d_struct*) _temp647)->f2;
goto _LL670;} else{ goto _LL671;} _LL671: if(*(( int*) _temp647) == Cyc_Absyn_ExternC_d){
_LL701: _temp700=(( struct Cyc_Absyn_ExternC_d_struct*) _temp647)->f1; goto
_LL672;} else{ goto _LL648;} _LL650:({ void* _temp702[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("top level let-declarations are not implemented", sizeof(
unsigned char), 47u), _tag_arr( _temp702, sizeof( void*), 0u));}); goto _LL648;
_LL652:({ void* _temp703[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("top level let-declarations are not implemented",
sizeof( unsigned char), 47u), _tag_arr( _temp703, sizeof( void*), 0u));}); goto
_LL648; _LL654: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp673->sc):
0){( void*)( _temp673->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcVardecl(
te, ge, loc, _temp673, check_var_init); goto _LL648; _LL656: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp675->sc): 0){( void*)( _temp675->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcFndecl( te, ge, loc, _temp675); goto _LL648; _LL658: Cyc_Tc_tcTypedefdecl(
te, ge, loc, _temp677); goto _LL648; _LL660: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp679->sc): 0){( void*)( _temp679->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcStructdecl( te, ge, loc, _temp679); goto _LL648; _LL662: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp681->sc): 0){( void*)( _temp681->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcUniondecl( te, ge, loc, _temp681);
goto _LL648; _LL664: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*)
_temp683->sc): 0){( void*)( _temp683->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcTuniondecl( te, ge, loc, _temp683); goto _LL648; _LL666: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp685->sc): 0){( void*)( _temp685->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcEnumdecl( te, ge, loc, _temp685);
goto _LL648; _LL668: { struct Cyc_List_List* _temp704= te->ns; struct Cyc_List_List*
_temp705=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp704,({ struct Cyc_List_List* _temp706=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp706->hd=( void*) _temp689;
_temp706->tl= 0; _temp706;})); if( !(( int(*)( struct Cyc_Set_Set* s, struct
_tagged_arr* elt)) Cyc_Set_member)( ge->namespaces, _temp689)){ ge->namespaces=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)(
ge->namespaces, _temp689); te->ae=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key, struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)( te->ae,
_temp705, Cyc_Tcenv_empty_genv());} te->ns= _temp705; Cyc_Tc_tc_decls( te,
_temp687, in_externC, check_var_init); te->ns= _temp704; goto _LL648;} _LL670: {
struct _tagged_arr* first; struct Cyc_List_List* rest;{ void* _temp707= _temp698;
struct Cyc_List_List* _temp719; struct Cyc_List_List* _temp721; struct Cyc_List_List*
_temp723; struct Cyc_List_List _temp725; struct Cyc_List_List* _temp726; struct
_tagged_arr* _temp728; struct Cyc_List_List* _temp730; struct Cyc_List_List
_temp732; struct Cyc_List_List* _temp733; struct _tagged_arr* _temp735; _LL709:
if( _temp707 ==( void*) Cyc_Absyn_Loc_n){ goto _LL710;} else{ goto _LL711;}
_LL711: if(( unsigned int) _temp707 > 1u?*(( int*) _temp707) == Cyc_Absyn_Rel_n:
0){ _LL720: _temp719=(( struct Cyc_Absyn_Rel_n_struct*) _temp707)->f1; if(
_temp719 == 0){ goto _LL712;} else{ goto _LL713;}} else{ goto _LL713;} _LL713:
if(( unsigned int) _temp707 > 1u?*(( int*) _temp707) == Cyc_Absyn_Abs_n: 0){
_LL722: _temp721=(( struct Cyc_Absyn_Abs_n_struct*) _temp707)->f1; if( _temp721
== 0){ goto _LL714;} else{ goto _LL715;}} else{ goto _LL715;} _LL715: if((
unsigned int) _temp707 > 1u?*(( int*) _temp707) == Cyc_Absyn_Rel_n: 0){ _LL724:
_temp723=(( struct Cyc_Absyn_Rel_n_struct*) _temp707)->f1; if( _temp723 == 0){
goto _LL717;} else{ _temp725=* _temp723; _LL729: _temp728=( struct _tagged_arr*)
_temp725.hd; goto _LL727; _LL727: _temp726= _temp725.tl; goto _LL716;}} else{
goto _LL717;} _LL717: if(( unsigned int) _temp707 > 1u?*(( int*) _temp707) ==
Cyc_Absyn_Abs_n: 0){ _LL731: _temp730=(( struct Cyc_Absyn_Abs_n_struct*)
_temp707)->f1; if( _temp730 == 0){ goto _LL708;} else{ _temp732=* _temp730;
_LL736: _temp735=( struct _tagged_arr*) _temp732.hd; goto _LL734; _LL734:
_temp733= _temp732.tl; goto _LL718;}} else{ goto _LL708;} _LL710: goto _LL712;
_LL712: goto _LL714; _LL714: first= _temp696; rest= 0; goto _LL708; _LL716:
_temp735= _temp728; _temp733= _temp726; goto _LL718; _LL718: first= _temp735;
rest=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp733,({ struct Cyc_List_List* _temp737=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp737->hd=( void*) _temp696;
_temp737->tl= 0; _temp737;})); goto _LL708; _LL708:;}{ struct Cyc_List_List*
_temp738= Cyc_Tcenv_resolve_namespace( te, loc, first, rest); ge->availables=({
struct Cyc_List_List* _temp739=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp739->hd=( void*) _temp738; _temp739->tl= ge->availables;
_temp739;}); Cyc_Tc_tc_decls( te, _temp691, in_externC, check_var_init); ge->availables=((
struct Cyc_List_List*) _check_null( ge->availables))->tl; goto _LL648;}} _LL672:
Cyc_Tc_tc_decls( te, _temp700, 1, check_var_init); goto _LL648; _LL648:;}} void
Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int check_var_init, struct Cyc_List_List*
ds){ Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r); Cyc_Tc_tc_decls( te, ds,
0, check_var_init);} static int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env,
struct Cyc_Absyn_Decl* d); static struct Cyc_List_List* Cyc_Tc_treeshake_f(
struct Cyc_Dict_Dict* env, struct Cyc_List_List* ds){ return(( struct Cyc_List_List*(*)(
int(* f)( struct Cyc_Dict_Dict*, struct Cyc_Absyn_Decl*), struct Cyc_Dict_Dict*
env, struct Cyc_List_List* l)) Cyc_List_filter_c)( Cyc_Tc_vardecl_needed, env,
ds);} struct _tuple6{ struct Cyc_Tcenv_Genv* f1; struct Cyc_Set_Set* f2; } ;
static int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl*
d){ void* _temp740=( void*) d->r; struct Cyc_Absyn_Vardecl* _temp750; struct Cyc_List_List*
_temp752; struct Cyc_List_List** _temp754; struct Cyc_List_List* _temp755;
struct Cyc_List_List** _temp757; _LL742: if(*(( int*) _temp740) == Cyc_Absyn_Var_d){
_LL751: _temp750=(( struct Cyc_Absyn_Var_d_struct*) _temp740)->f1; goto _LL743;}
else{ goto _LL744;} _LL744: if(*(( int*) _temp740) == Cyc_Absyn_Using_d){ _LL753:
_temp752=(( struct Cyc_Absyn_Using_d_struct*) _temp740)->f2; _temp754=&(( struct
Cyc_Absyn_Using_d_struct*) _temp740)->f2; goto _LL745;} else{ goto _LL746;}
_LL746: if(*(( int*) _temp740) == Cyc_Absyn_Namespace_d){ _LL756: _temp755=((
struct Cyc_Absyn_Namespace_d_struct*) _temp740)->f2; _temp757=&(( struct Cyc_Absyn_Namespace_d_struct*)
_temp740)->f2; goto _LL747;} else{ goto _LL748;} _LL748: goto _LL749; _LL743:
if(( void*) _temp750->sc !=( void*) Cyc_Absyn_Extern){ return 1;}{ struct
_tuple0 _temp760; struct _tagged_arr* _temp761; void* _temp763; struct _tuple0*
_temp758= _temp750->name; _temp760=* _temp758; _LL764: _temp763= _temp760.f1;
goto _LL762; _LL762: _temp761= _temp760.f2; goto _LL759; _LL759: { struct Cyc_List_List*
ns;{ void* _temp765= _temp763; struct Cyc_List_List* _temp773; struct Cyc_List_List*
_temp775; _LL767: if( _temp765 ==( void*) Cyc_Absyn_Loc_n){ goto _LL768;} else{
goto _LL769;} _LL769: if(( unsigned int) _temp765 > 1u?*(( int*) _temp765) ==
Cyc_Absyn_Rel_n: 0){ _LL774: _temp773=(( struct Cyc_Absyn_Rel_n_struct*)
_temp765)->f1; goto _LL770;} else{ goto _LL771;} _LL771: if(( unsigned int)
_temp765 > 1u?*(( int*) _temp765) == Cyc_Absyn_Abs_n: 0){ _LL776: _temp775=((
struct Cyc_Absyn_Abs_n_struct*) _temp765)->f1; goto _LL772;} else{ goto _LL766;}
_LL768: ns= 0; goto _LL766; _LL770: ns= _temp773; goto _LL766; _LL772: ns=
_temp775; goto _LL766; _LL766:;}{ struct _tuple6* _temp777=(( struct _tuple6*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( env, ns);
struct Cyc_Tcenv_Genv* _temp778=(* _temp777).f1; int _temp779=(*(( struct
_tuple3*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
_temp778->ordinaries, _temp761)).f2; if( ! _temp779){(* _temp777).f2=(( struct
Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)((*
_temp777).f2, _temp761);} return _temp779;}}} _LL745: _temp757= _temp754; goto
_LL747; _LL747:* _temp757= Cyc_Tc_treeshake_f( env,* _temp757); return 1; _LL749:
return 1; _LL741:;} static int Cyc_Tc_treeshake_remove_f( struct Cyc_Set_Set*
set, struct _tagged_arr* x, struct _tuple3* y){ return !(( int(*)( struct Cyc_Set_Set*
s, struct _tagged_arr* elt)) Cyc_Set_member)( set, x);} static void Cyc_Tc_treeshake_remove(
struct Cyc_List_List* ns, struct _tuple6* nsenv){ struct Cyc_Tcenv_Genv*
_temp780=(* nsenv).f1; struct Cyc_Set_Set* _temp781=(* nsenv).f2; _temp780->ordinaries=((
struct Cyc_Dict_Dict*(*)( int(* f)( struct Cyc_Set_Set*, struct _tagged_arr*,
struct _tuple3*), struct Cyc_Set_Set* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_filter_c)(
Cyc_Tc_treeshake_remove_f, _temp781, _temp780->ordinaries);} static struct
_tuple6* Cyc_Tc_treeshake_make_env_f( struct Cyc_Tcenv_Genv* ge){ return({
struct _tuple6* _temp782=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp782->f1= ge; _temp782->f2=(( struct Cyc_Set_Set*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Set_empty)( Cyc_String_strptrcmp);
_temp782;});} struct Cyc_List_List* Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv* te,
struct Cyc_List_List* ds){ struct Cyc_Dict_Dict* _temp783=(( struct Cyc_Dict_Dict*(*)(
struct _tuple6*(* f)( struct Cyc_Tcenv_Genv*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)(
Cyc_Tc_treeshake_make_env_f, te->ae); struct Cyc_List_List* _temp784= Cyc_Tc_treeshake_f(
_temp783, ds);(( void(*)( void(* f)( struct Cyc_List_List*, struct _tuple6*),
struct Cyc_Dict_Dict* d)) Cyc_Dict_iter)( Cyc_Tc_treeshake_remove, _temp783);
return _temp784;}
