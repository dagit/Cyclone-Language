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
_LL6: return({ struct Cyc_List_List*(* _temp15)( struct _tagged_arr fmt, struct
_tagged_arr ap)=( struct Cyc_List_List*(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos; struct _tagged_arr _temp18= _tag_arr("transfer_fn_type_atts",
sizeof( unsigned char), 22u); void* _temp16[ 0u]={}; struct _tagged_arr _temp17={(
void*) _temp16,( void*) _temp16,( void*)( _temp16 + 0u)}; _temp15( _temp18,
_temp17);}); _LL2:;} struct _tuple3{ void* f1; int f2; } ; static void Cyc_Tc_tcVardecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Vardecl* vd, int check_var_init){ struct _tagged_arr* v=(*
vd->name).f2; void* t=( void*) vd->type; void* sc=( void*) vd->sc; struct Cyc_List_List*
atts= vd->attributes;{ void* _temp19=(* vd->name).f1; struct Cyc_List_List*
_temp27; struct Cyc_List_List* _temp29; _LL21: if(( unsigned int) _temp19 > 1u?*((
int*) _temp19) == Cyc_Absyn_Rel_n: 0){ _LL28: _temp27=(( struct Cyc_Absyn_Rel_n_struct*)
_temp19)->f1; if( _temp27 == 0){ goto _LL22;} else{ goto _LL23;}} else{ goto
_LL23;} _LL23: if(( unsigned int) _temp19 > 1u?*(( int*) _temp19) == Cyc_Absyn_Abs_n:
0){ _LL30: _temp29=(( struct Cyc_Absyn_Abs_n_struct*) _temp19)->f1; if( _temp29
== 0){ goto _LL24;} else{ goto _LL25;}} else{ goto _LL25;} _LL25: goto _LL26;
_LL22: goto _LL20; _LL24: goto _LL20; _LL26:({ void(* _temp31)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp34= loc; struct _tagged_arr _temp35= _tag_arr("qualified declarations are not implemented",
sizeof( unsigned char), 43u); void* _temp32[ 0u]={}; struct _tagged_arr _temp33={(
void*) _temp32,( void*) _temp32,( void*)( _temp32 + 0u)}; _temp31( _temp34,
_temp35, _temp33);}); return; _LL20:;}(* vd->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp36=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp36[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp37; _temp37.tag= Cyc_Absyn_Abs_n;
_temp37.f1= te->ns; _temp37;}); _temp36;});{ void* _temp38= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Exp* _temp45; struct Cyc_Absyn_Tqual _temp47; void* _temp49;
_LL40: if(( unsigned int) _temp38 > 4u?*(( int*) _temp38) == Cyc_Absyn_ArrayType:
0){ _LL50: _temp49=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp38)->f1;
goto _LL48; _LL48: _temp47=(( struct Cyc_Absyn_ArrayType_struct*) _temp38)->f2;
goto _LL46; _LL46: _temp45=(( struct Cyc_Absyn_ArrayType_struct*) _temp38)->f3;
if( _temp45 == 0){ goto _LL44;} else{ goto _LL42;}} else{ goto _LL42;} _LL44:
if( vd->initializer != 0){ goto _LL41;} else{ goto _LL42;} _LL42: goto _LL43;
_LL41:{ void* _temp51=( void*)(( struct Cyc_Absyn_Exp*) _check_null( vd->initializer))->r;
void* _temp61; struct _tagged_arr _temp63; struct Cyc_List_List* _temp65; struct
Cyc_List_List* _temp67; _LL53: if(*(( int*) _temp51) == Cyc_Absyn_Const_e){
_LL62: _temp61=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp51)->f1; if((
unsigned int) _temp61 > 1u?*(( int*) _temp61) == Cyc_Absyn_String_c: 0){ _LL64:
_temp63=(( struct Cyc_Absyn_String_c_struct*) _temp61)->f1; goto _LL54;} else{
goto _LL55;}} else{ goto _LL55;} _LL55: if(*(( int*) _temp51) == Cyc_Absyn_UnresolvedMem_e){
_LL66: _temp65=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp51)->f2; goto
_LL56;} else{ goto _LL57;} _LL57: if(*(( int*) _temp51) == Cyc_Absyn_Array_e){
_LL68: _temp67=(( struct Cyc_Absyn_Array_e_struct*) _temp51)->f1; goto _LL58;}
else{ goto _LL59;} _LL59: goto _LL60; _LL54: t=( void*)( vd->type=( void*)((
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp69=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp69[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp70; _temp70.tag= Cyc_Absyn_ArrayType; _temp70.f1=(
void*) _temp49; _temp70.f2= _temp47; _temp70.f3=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
_get_arr_size( _temp63, sizeof( unsigned char)), 0); _temp70;}); _temp69;})));
goto _LL52; _LL56: _temp67= _temp65; goto _LL58; _LL58: t=( void*)( vd->type=(
void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp71=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp71[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp72; _temp72.tag= Cyc_Absyn_ArrayType; _temp72.f1=(
void*) _temp49; _temp72.f2= _temp47; _temp72.f3=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp((
unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp67), 0);
_temp72;}); _temp71;}))); goto _LL52; _LL60: goto _LL52; _LL52:;} goto _LL39;
_LL43: goto _LL39; _LL39:;} Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);
if( Cyc_Tcutil_is_function_type( t)){ atts= Cyc_Tc_transfer_fn_type_atts( t,
atts);} if( sc ==( void*) Cyc_Absyn_Extern? 1: sc ==( void*) Cyc_Absyn_ExternC){
if( vd->initializer != 0){({ void(* _temp73)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp76= loc; struct _tagged_arr _temp77= _tag_arr("extern declaration should not have initializer",
sizeof( unsigned char), 47u); void* _temp74[ 0u]={}; struct _tagged_arr _temp75={(
void*) _temp74,( void*) _temp74,( void*)( _temp74 + 0u)}; _temp73( _temp76,
_temp77, _temp75);});}} else{ if( ! Cyc_Tcutil_is_function_type( t)){ for( 0;
atts != 0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ void*
_temp78=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd; _LL80: if((
unsigned int) _temp78 > 16u?*(( int*) _temp78) == Cyc_Absyn_Aligned_att: 0){
goto _LL81;} else{ goto _LL82;} _LL82: if(( unsigned int) _temp78 > 16u?*(( int*)
_temp78) == Cyc_Absyn_Section_att: 0){ goto _LL83;} else{ goto _LL84;} _LL84:
if( _temp78 ==( void*) Cyc_Absyn_Nocommon_att){ goto _LL85;} else{ goto _LL86;}
_LL86: if( _temp78 ==( void*) Cyc_Absyn_Shared_att){ goto _LL87;} else{ goto
_LL88;} _LL88: if( _temp78 ==( void*) Cyc_Absyn_Unused_att){ goto _LL89;} else{
goto _LL90;} _LL90: if( _temp78 ==( void*) Cyc_Absyn_Weak_att){ goto _LL91;}
else{ goto _LL92;} _LL92: if( _temp78 ==( void*) Cyc_Absyn_Dllimport_att){ goto
_LL93;} else{ goto _LL94;} _LL94: if( _temp78 ==( void*) Cyc_Absyn_Dllexport_att){
goto _LL95;} else{ goto _LL96;} _LL96: goto _LL97; _LL81: continue; _LL83:
continue; _LL85: continue; _LL87: continue; _LL89: continue; _LL91: continue;
_LL93: continue; _LL95: continue; _LL97:({ void(* _temp98)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp103= loc; struct _tagged_arr _temp104= _tag_arr("bad attribute %s for variable %s",
sizeof( unsigned char), 33u); struct Cyc_Stdio_String_pa_struct _temp106;
_temp106.tag= Cyc_Stdio_String_pa; _temp106.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd);{ void* _temp101=( void*)&
_temp106; struct Cyc_Stdio_String_pa_struct _temp105; _temp105.tag= Cyc_Stdio_String_pa;
_temp105.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( vd->name);{ void*
_temp102=( void*)& _temp105; void* _temp99[ 2u]={ _temp101, _temp102}; struct
_tagged_arr _temp100={( void*) _temp99,( void*) _temp99,( void*)( _temp99 + 2u)};
_temp98( _temp103, _temp104, _temp100);}}}); goto _LL79; _LL79:;} if( vd->initializer
== 0){ if( check_var_init? ! Cyc_Tcutil_supports_default( t): 0){({ void(*
_temp107)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp112= loc;
struct _tagged_arr _temp113= _tag_arr("initializer required for variable %s of type %s",
sizeof( unsigned char), 48u); struct Cyc_Stdio_String_pa_struct _temp115;
_temp115.tag= Cyc_Stdio_String_pa; _temp115.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
vd->name);{ void* _temp110=( void*)& _temp115; struct Cyc_Stdio_String_pa_struct
_temp114; _temp114.tag= Cyc_Stdio_String_pa; _temp114.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp111=( void*)& _temp114; void* _temp108[
2u]={ _temp110, _temp111}; struct _tagged_arr _temp109={( void*) _temp108,( void*)
_temp108,( void*)( _temp108 + 2u)}; _temp107( _temp112, _temp113, _temp109);}}});}}
else{ struct Cyc_Absyn_Exp* _temp116=( struct Cyc_Absyn_Exp*) _check_null( vd->initializer);
void* _temp117= Cyc_Tcexp_tcExpInitializer( te,( void**)& t, _temp116); if( !
Cyc_Tcutil_coerce_assign( te, _temp116, t)){({ void(* _temp118)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp124= loc; struct _tagged_arr _temp125= _tag_arr("%s is declared with type %s but initialized with type %s",
sizeof( unsigned char), 57u); struct Cyc_Stdio_String_pa_struct _temp128;
_temp128.tag= Cyc_Stdio_String_pa; _temp128.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
vd->name);{ void* _temp121=( void*)& _temp128; struct Cyc_Stdio_String_pa_struct
_temp127; _temp127.tag= Cyc_Stdio_String_pa; _temp127.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp122=( void*)& _temp127; struct Cyc_Stdio_String_pa_struct
_temp126; _temp126.tag= Cyc_Stdio_String_pa; _temp126.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( _temp117);{ void* _temp123=( void*)& _temp126; void*
_temp119[ 3u]={ _temp121, _temp122, _temp123}; struct _tagged_arr _temp120={(
void*) _temp119,( void*) _temp119,( void*)( _temp119 + 3u)}; _temp118( _temp124,
_temp125, _temp120);}}}});} if( ! Cyc_Tcutil_is_const_exp( te, _temp116)){({
void(* _temp129)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp132= loc;
struct _tagged_arr _temp133= _tag_arr("initializer is not a constant expression",
sizeof( unsigned char), 41u); void* _temp130[ 0u]={}; struct _tagged_arr
_temp131={( void*) _temp130,( void*) _temp130,( void*)( _temp130 + 0u)};
_temp129( _temp132, _temp133, _temp131);});}}} else{ for( 0; atts != 0; atts=((
struct Cyc_List_List*) _check_null( atts))->tl){ void* _temp134=( void*)((
struct Cyc_List_List*) _check_null( atts))->hd; _LL136: if(( unsigned int)
_temp134 > 16u?*(( int*) _temp134) == Cyc_Absyn_Regparm_att: 0){ goto _LL137;}
else{ goto _LL138;} _LL138: if( _temp134 ==( void*) Cyc_Absyn_Stdcall_att){ goto
_LL139;} else{ goto _LL140;} _LL140: if( _temp134 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL141;} else{ goto _LL142;} _LL142: if( _temp134 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL143;} else{ goto _LL144;} _LL144: if( _temp134 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL145;} else{ goto _LL146;} _LL146: if(( unsigned int) _temp134 > 16u?*((
int*) _temp134) == Cyc_Absyn_Format_att: 0){ goto _LL147;} else{ goto _LL148;}
_LL148: if( _temp134 ==( void*) Cyc_Absyn_Const_att){ goto _LL149;} else{ goto
_LL150;} _LL150: if(( unsigned int) _temp134 > 16u?*(( int*) _temp134) == Cyc_Absyn_Aligned_att:
0){ goto _LL151;} else{ goto _LL152;} _LL152: if( _temp134 ==( void*) Cyc_Absyn_Packed_att){
goto _LL153;} else{ goto _LL154;} _LL154: goto _LL155; _LL137: goto _LL139;
_LL139: goto _LL141; _LL141: goto _LL143; _LL143: goto _LL145; _LL145: goto
_LL147; _LL147: goto _LL149; _LL149:({ int(* _temp156)( struct _tagged_arr fmt,
struct _tagged_arr ap)=( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos; struct _tagged_arr _temp159= _tag_arr("tcVardecl: fn type attributes in function var decl",
sizeof( unsigned char), 51u); void* _temp157[ 0u]={}; struct _tagged_arr
_temp158={( void*) _temp157,( void*) _temp157,( void*)( _temp157 + 0u)};
_temp156( _temp159, _temp158);}); goto _LL135; _LL151: goto _LL153; _LL153:({
void(* _temp160)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp164= loc;
struct _tagged_arr _temp165= _tag_arr("bad attribute %s in function declaration",
sizeof( unsigned char), 41u); struct Cyc_Stdio_String_pa_struct _temp166;
_temp166.tag= Cyc_Stdio_String_pa; _temp166.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd);{ void* _temp163=( void*)&
_temp166; void* _temp161[ 1u]={ _temp163}; struct _tagged_arr _temp162={( void*)
_temp161,( void*) _temp161,( void*)( _temp161 + 1u)}; _temp160( _temp164,
_temp165, _temp162);}}); goto _LL135; _LL155: continue; _LL135:;}}}{ struct
_handler_cons _temp167; _push_handler(& _temp167);{ int _temp169= 0; if( setjmp(
_temp167.handler)){ _temp169= 1;} if( ! _temp169){{ struct _tuple3* _temp170=((
struct _tuple3*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
ge->ordinaries, v); void* _temp171=(* _temp170).f1; void* _temp181; _LL173: if(*((
int*) _temp171) == Cyc_Tcenv_VarRes){ _LL182: _temp181=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp171)->f1; goto _LL174;} else{ goto _LL175;} _LL175: if(*(( int*) _temp171)
== Cyc_Tcenv_StructRes){ goto _LL176;} else{ goto _LL177;} _LL177: if(*(( int*)
_temp171) == Cyc_Tcenv_TunionRes){ goto _LL178;} else{ goto _LL179;} _LL179: if(*((
int*) _temp171) == Cyc_Tcenv_EnumRes){ goto _LL180;} else{ goto _LL172;} _LL174: {
struct Cyc_Absyn_Global_b_struct* _temp183=({ struct Cyc_Absyn_Global_b_struct*
_temp188=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp188[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp189; _temp189.tag= Cyc_Absyn_Global_b;
_temp189.f1= vd; _temp189;}); _temp188;}); void* _temp184= Cyc_Tcdecl_merge_binding(
_temp181,( void*) _temp183, loc, Cyc_Tc_tc_msg); if( _temp184 ==( void*) Cyc_Absyn_Unresolved_b){
goto _LL172;} if( _temp184 == _temp181?(* _temp170).f2: 0){ goto _LL172;} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple3*
_temp185=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp185->f1=(
void*)({ struct Cyc_Tcenv_VarRes_struct* _temp186=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp186[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp187; _temp187.tag= Cyc_Tcenv_VarRes; _temp187.f1=( void*) _temp184;
_temp187;}); _temp186;}); _temp185->f2= 1; _temp185;})); goto _LL172;} _LL176:({
void(* _temp190)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_warn; struct Cyc_Position_Segment* _temp193= loc;
struct _tagged_arr _temp194= _tag_arr("variable declaration shadows previous struct declaration",
sizeof( unsigned char), 57u); void* _temp191[ 0u]={}; struct _tagged_arr
_temp192={( void*) _temp191,( void*) _temp191,( void*)( _temp191 + 0u)};
_temp190( _temp193, _temp194, _temp192);}); goto _LL172; _LL178:({ void(*
_temp195)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_warn; struct Cyc_Position_Segment* _temp198= loc;
struct _tagged_arr _temp199= _tag_arr("variable declaration shadows previous [x]tunion constructor",
sizeof( unsigned char), 60u); void* _temp196[ 0u]={}; struct _tagged_arr
_temp197={( void*) _temp196,( void*) _temp196,( void*)( _temp196 + 0u)};
_temp195( _temp198, _temp199, _temp197);}); goto _LL172; _LL180:({ void(*
_temp200)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_warn; struct Cyc_Position_Segment* _temp203= loc;
struct _tagged_arr _temp204= _tag_arr("variable declaration shadows previous enum tag",
sizeof( unsigned char), 47u); void* _temp201[ 0u]={}; struct _tagged_arr
_temp202={( void*) _temp201,( void*) _temp201,( void*)( _temp201 + 0u)};
_temp200( _temp203, _temp204, _temp202);}); goto _LL172; _LL172:;}; _pop_handler();}
else{ void* _temp168=( void*) _exn_thrown; void* _temp206= _temp168; _LL208: if(
_temp206 == Cyc_Dict_Absent){ goto _LL209;} else{ goto _LL210;} _LL210: goto
_LL211; _LL209: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,
v,({ struct _tuple3* _temp212=( struct _tuple3*) GC_malloc( sizeof( struct
_tuple3)); _temp212->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp213=(
struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp213[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp214; _temp214.tag= Cyc_Tcenv_VarRes;
_temp214.f1=( void*)(( void*)({ struct Cyc_Absyn_Global_b_struct* _temp215=(
struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp215[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp216; _temp216.tag= Cyc_Absyn_Global_b;
_temp216.f1= vd; _temp216;}); _temp215;})); _temp214;}); _temp213;}); _temp212->f2=
0; _temp212;})); goto _LL207; _LL211:( void) _throw( _temp206); _LL207:;}}}}
static void Cyc_Tc_tcFndecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl* fd){ struct
_tagged_arr* v=(* fd->name).f2; void* sc=( void*) fd->sc;{ void* _temp217=(* fd->name).f1;
struct Cyc_List_List* _temp225; struct Cyc_List_List* _temp227; _LL219: if((
unsigned int) _temp217 > 1u?*(( int*) _temp217) == Cyc_Absyn_Rel_n: 0){ _LL226:
_temp225=(( struct Cyc_Absyn_Rel_n_struct*) _temp217)->f1; if( _temp225 == 0){
goto _LL220;} else{ goto _LL221;}} else{ goto _LL221;} _LL221: if(( unsigned int)
_temp217 > 1u?*(( int*) _temp217) == Cyc_Absyn_Abs_n: 0){ _LL228: _temp227=((
struct Cyc_Absyn_Abs_n_struct*) _temp217)->f1; goto _LL222;} else{ goto _LL223;}
_LL223: goto _LL224; _LL220: goto _LL218; _LL222:( int) _throw(( void*)({ struct
Cyc_Core_Impossible_struct* _temp229=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp229[ 0]=({ struct
Cyc_Core_Impossible_struct _temp230; _temp230.tag= Cyc_Core_Impossible; _temp230.f1=
_tag_arr("tc: Abs_n in tcFndecl", sizeof( unsigned char), 22u); _temp230;});
_temp229;})); _LL224:({ void(* _temp231)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp234= loc; struct _tagged_arr _temp235= _tag_arr("qualified declarations are not implemented",
sizeof( unsigned char), 43u); void* _temp232[ 0u]={}; struct _tagged_arr
_temp233={( void*) _temp232,( void*) _temp232,( void*)( _temp232 + 0u)};
_temp231( _temp234, _temp235, _temp233);}); return; _LL218:;}(* fd->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp236=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp236[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp237; _temp237.tag= Cyc_Absyn_Abs_n; _temp237.f1= te->ns; _temp237;});
_temp236;}); Cyc_Tcutil_check_fndecl_valid_type( loc, te, fd);{ void* t= Cyc_Tcutil_fndecl2typ(
fd); fd->attributes= Cyc_Tc_transfer_fn_type_atts( t, fd->attributes);{ struct
Cyc_List_List* atts= fd->attributes; for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp238=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; _LL240: if( _temp238 ==( void*) Cyc_Absyn_Packed_att){
goto _LL241;} else{ goto _LL242;} _LL242: if(( unsigned int) _temp238 > 16u?*((
int*) _temp238) == Cyc_Absyn_Aligned_att: 0){ goto _LL243;} else{ goto _LL244;}
_LL244: goto _LL245; _LL241: goto _LL243; _LL243:({ void(* _temp246)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp250= loc; struct _tagged_arr _temp251= _tag_arr("bad attribute %s for function",
sizeof( unsigned char), 30u); struct Cyc_Stdio_String_pa_struct _temp252;
_temp252.tag= Cyc_Stdio_String_pa; _temp252.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd);{ void* _temp249=( void*)&
_temp252; void* _temp247[ 1u]={ _temp249}; struct _tagged_arr _temp248={( void*)
_temp247,( void*) _temp247,( void*)( _temp247 + 1u)}; _temp246( _temp250,
_temp251, _temp248);}}); goto _LL239; _LL245: goto _LL239; _LL239:;}}{ struct
_handler_cons _temp253; _push_handler(& _temp253);{ int _temp255= 0; if( setjmp(
_temp253.handler)){ _temp255= 1;} if( ! _temp255){{ struct _tuple3* _temp256=((
struct _tuple3*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
ge->ordinaries, v); void* _temp257=(* _temp256).f1; void* _temp267; _LL259: if(*((
int*) _temp257) == Cyc_Tcenv_VarRes){ _LL268: _temp267=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp257)->f1; goto _LL260;} else{ goto _LL261;} _LL261: if(*(( int*) _temp257)
== Cyc_Tcenv_StructRes){ goto _LL262;} else{ goto _LL263;} _LL263: if(*(( int*)
_temp257) == Cyc_Tcenv_TunionRes){ goto _LL264;} else{ goto _LL265;} _LL265: if(*((
int*) _temp257) == Cyc_Tcenv_EnumRes){ goto _LL266;} else{ goto _LL258;} _LL260: {
struct Cyc_Absyn_Funname_b_struct* _temp269=({ struct Cyc_Absyn_Funname_b_struct*
_temp274=( struct Cyc_Absyn_Funname_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct));
_temp274[ 0]=({ struct Cyc_Absyn_Funname_b_struct _temp275; _temp275.tag= Cyc_Absyn_Funname_b;
_temp275.f1= fd; _temp275;}); _temp274;}); void* _temp270= Cyc_Tcdecl_merge_binding(
_temp267,( void*) _temp269, loc, Cyc_Tc_tc_msg); if( _temp270 ==( void*) Cyc_Absyn_Unresolved_b){
goto _LL258;} if( _temp270 == _temp267?(* _temp256).f2: 0){ goto _LL258;} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple3*
_temp271=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp271->f1=(
void*)({ struct Cyc_Tcenv_VarRes_struct* _temp272=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp272[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp273; _temp273.tag= Cyc_Tcenv_VarRes; _temp273.f1=( void*) _temp270;
_temp273;}); _temp272;}); _temp271->f2= 1; _temp271;})); goto _LL258;} _LL262:({
void(* _temp276)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_warn; struct Cyc_Position_Segment* _temp279= loc;
struct _tagged_arr _temp280= _tag_arr("function declaration shadows previous struct declaration",
sizeof( unsigned char), 57u); void* _temp277[ 0u]={}; struct _tagged_arr
_temp278={( void*) _temp277,( void*) _temp277,( void*)( _temp277 + 0u)};
_temp276( _temp279, _temp280, _temp278);}); goto _LL258; _LL264:({ void(*
_temp281)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_warn; struct Cyc_Position_Segment* _temp284= loc;
struct _tagged_arr _temp285= _tag_arr("function declaration shadows previous [x]tunion constructor",
sizeof( unsigned char), 60u); void* _temp282[ 0u]={}; struct _tagged_arr
_temp283={( void*) _temp282,( void*) _temp282,( void*)( _temp282 + 0u)};
_temp281( _temp284, _temp285, _temp283);}); goto _LL258; _LL266:({ void(*
_temp286)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_warn; struct Cyc_Position_Segment* _temp289= loc;
struct _tagged_arr _temp290= _tag_arr("function declaration shadows previous enum tag",
sizeof( unsigned char), 47u); void* _temp287[ 0u]={}; struct _tagged_arr
_temp288={( void*) _temp287,( void*) _temp287,( void*)( _temp287 + 0u)};
_temp286( _temp289, _temp290, _temp288);}); goto _LL258; _LL258:;}; _pop_handler();}
else{ void* _temp254=( void*) _exn_thrown; void* _temp292= _temp254; _LL294: if(
_temp292 == Cyc_Dict_Absent){ goto _LL295;} else{ goto _LL296;} _LL296: goto
_LL297; _LL295: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,
v,({ struct _tuple3* _temp298=( struct _tuple3*) GC_malloc( sizeof( struct
_tuple3)); _temp298->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp299=(
struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp299[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp300; _temp300.tag= Cyc_Tcenv_VarRes;
_temp300.f1=( void*)(( void*)({ struct Cyc_Absyn_Funname_b_struct* _temp301=(
struct Cyc_Absyn_Funname_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct));
_temp301[ 0]=({ struct Cyc_Absyn_Funname_b_struct _temp302; _temp302.tag= Cyc_Absyn_Funname_b;
_temp302.f1= fd; _temp302;}); _temp301;})); _temp300;}); _temp299;}); _temp298->f2=
0; _temp298;})); goto _LL293; _LL297:( void) _throw( _temp292); _LL293:;}}} te->le=({
struct Cyc_Core_Opt* _temp303=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp303->v=( void*)(( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp304=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp304[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp305; _temp305.tag= Cyc_Tcenv_Outermost;
_temp305.f1=( void*) Cyc_Tcenv_new_fenv( loc, fd); _temp305;}); _temp304;}));
_temp303;}); Cyc_Tcstmt_tcStmt( te, fd->body, 0); if( ! Cyc_Tcenv_all_labels_resolved(
te)){({ void(* _temp306)( struct Cyc_Position_Segment*, struct _tagged_arr fmt,
struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp309=
loc; struct _tagged_arr _temp310= _tag_arr("function has goto statements to undefined labels",
sizeof( unsigned char), 49u); void* _temp307[ 0u]={}; struct _tagged_arr
_temp308={( void*) _temp307,( void*) _temp307,( void*)( _temp307 + 0u)};
_temp306( _temp309, _temp310, _temp308);});} te->le= 0;}} static void Cyc_Tc_tcTypedefdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Typedefdecl* td){ struct _tagged_arr* v=(* td->name).f2;{
void* _temp311=(* td->name).f1; struct Cyc_List_List* _temp319; struct Cyc_List_List*
_temp321; _LL313: if(( unsigned int) _temp311 > 1u?*(( int*) _temp311) == Cyc_Absyn_Rel_n:
0){ _LL320: _temp319=(( struct Cyc_Absyn_Rel_n_struct*) _temp311)->f1; if(
_temp319 == 0){ goto _LL314;} else{ goto _LL315;}} else{ goto _LL315;} _LL315:
if(( unsigned int) _temp311 > 1u?*(( int*) _temp311) == Cyc_Absyn_Abs_n: 0){
_LL322: _temp321=(( struct Cyc_Absyn_Abs_n_struct*) _temp311)->f1; if( _temp321
== 0){ goto _LL316;} else{ goto _LL317;}} else{ goto _LL317;} _LL317: goto
_LL318; _LL314: goto _LL312; _LL316: goto _LL312; _LL318:({ void(* _temp323)(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp326= loc; struct _tagged_arr
_temp327= _tag_arr("qualified declarations are not implemented", sizeof(
unsigned char), 43u); void* _temp324[ 0u]={}; struct _tagged_arr _temp325={(
void*) _temp324,( void*) _temp324,( void*)( _temp324 + 0u)}; _temp323( _temp326,
_temp327, _temp325);}); return; _LL312:;} if((( int(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_member)( ge->typedefs, v)){({ void(* _temp328)(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp332= loc; struct _tagged_arr
_temp333= _tag_arr("redeclaration of typedef %s", sizeof( unsigned char), 28u);
struct Cyc_Stdio_String_pa_struct _temp334; _temp334.tag= Cyc_Stdio_String_pa;
_temp334.f1=( struct _tagged_arr)* v;{ void* _temp331=( void*)& _temp334; void*
_temp329[ 1u]={ _temp331}; struct _tagged_arr _temp330={( void*) _temp329,( void*)
_temp329,( void*)( _temp329 + 1u)}; _temp328( _temp332, _temp333, _temp330);}});
return;}(* td->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp335=(
struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp335[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp336; _temp336.tag= Cyc_Absyn_Abs_n;
_temp336.f1= te->ns; _temp336;}); _temp335;}); Cyc_Tcutil_check_unique_tvars(
loc, td->tvs); Cyc_Tcutil_add_tvar_identities( td->tvs); Cyc_Tcutil_check_type(
loc, te, td->tvs,( void*) Cyc_Absyn_AnyKind,( void*) td->defn); ge->typedefs=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Typedefdecl* data)) Cyc_Dict_insert)( ge->typedefs, v, td);}
static void Cyc_Tc_tcStructFields( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct _tagged_arr obj, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs){ struct _RegionHandle _temp337= _new_region();
struct _RegionHandle* uprev_rgn=& _temp337; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0; struct Cyc_List_List* _temp338= fields; for( 0;
_temp338 != 0; _temp338=(( struct Cyc_List_List*) _check_null( _temp338))->tl){
struct Cyc_Absyn_Structfield _temp341; struct Cyc_List_List* _temp342; struct
Cyc_Core_Opt* _temp344; void* _temp346; struct Cyc_Absyn_Tqual _temp348; struct
_tagged_arr* _temp350; struct Cyc_Absyn_Structfield* _temp339=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp338))->hd; _temp341=* _temp339; _LL351:
_temp350= _temp341.name; goto _LL349; _LL349: _temp348= _temp341.tq; goto _LL347;
_LL347: _temp346=( void*) _temp341.type; goto _LL345; _LL345: _temp344= _temp341.width;
goto _LL343; _LL343: _temp342= _temp341.attributes; goto _LL340; _LL340: if(((
int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List*
l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,
_temp350)){({ void(* _temp352)( struct Cyc_Position_Segment*, struct _tagged_arr
fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp357= loc; struct _tagged_arr _temp358= _tag_arr("duplicate field %s in %s",
sizeof( unsigned char), 25u); struct Cyc_Stdio_String_pa_struct _temp360;
_temp360.tag= Cyc_Stdio_String_pa; _temp360.f1=( struct _tagged_arr)* _temp350;{
void* _temp355=( void*)& _temp360; struct Cyc_Stdio_String_pa_struct _temp359;
_temp359.tag= Cyc_Stdio_String_pa; _temp359.f1=( struct _tagged_arr) obj;{ void*
_temp356=( void*)& _temp359; void* _temp353[ 2u]={ _temp355, _temp356}; struct
_tagged_arr _temp354={( void*) _temp353,( void*) _temp353,( void*)( _temp353 + 2u)};
_temp352( _temp357, _temp358, _temp354);}}});} if( Cyc_String_strcmp(* _temp350,
_tag_arr("", sizeof( unsigned char), 1u)) != 0){ prev_fields=({ struct Cyc_List_List*
_temp361=( struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp361->hd=( void*) _temp350; _temp361->tl= prev_fields; _temp361;});} Cyc_Tcutil_check_type(
loc, te, tvs,( void*) Cyc_Absyn_MemKind, _temp346); if( _temp344 != 0){
unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp344))->v)){({ void(* _temp362)( struct
Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr;
struct Cyc_Position_Segment* _temp366= loc; struct _tagged_arr _temp367=
_tag_arr("bitfield %s does not have constant width", sizeof( unsigned char), 41u);
struct Cyc_Stdio_String_pa_struct _temp368; _temp368.tag= Cyc_Stdio_String_pa;
_temp368.f1=( struct _tagged_arr)* _temp350;{ void* _temp365=( void*)& _temp368;
void* _temp363[ 1u]={ _temp365}; struct _tagged_arr _temp364={( void*) _temp363,(
void*) _temp363,( void*)( _temp363 + 1u)}; _temp362( _temp366, _temp367,
_temp364);}});} else{ w= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp344))->v);}{ void* _temp369= Cyc_Tcutil_compress(
_temp346); void* _temp375; _LL371: if(( unsigned int) _temp369 > 4u?*(( int*)
_temp369) == Cyc_Absyn_IntType: 0){ _LL376: _temp375=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp369)->f2; goto _LL372;} else{ goto _LL373;} _LL373: goto _LL374; _LL372:{
void* _temp377= _temp375; _LL379: if( _temp377 ==( void*) Cyc_Absyn_B1){ goto
_LL380;} else{ goto _LL381;} _LL381: if( _temp377 ==( void*) Cyc_Absyn_B2){ goto
_LL382;} else{ goto _LL383;} _LL383: if( _temp377 ==( void*) Cyc_Absyn_B4){ goto
_LL384;} else{ goto _LL385;} _LL385: if( _temp377 ==( void*) Cyc_Absyn_B8){ goto
_LL386;} else{ goto _LL378;} _LL380: if( w > 8){({ void(* _temp387)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp390= loc; struct _tagged_arr _temp391= _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u); void* _temp388[ 0u]={}; struct _tagged_arr
_temp389={( void*) _temp388,( void*) _temp388,( void*)( _temp388 + 0u)};
_temp387( _temp390, _temp391, _temp389);});} goto _LL378; _LL382: if( w > 16){({
void(* _temp392)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp395= loc;
struct _tagged_arr _temp396= _tag_arr("bitfield larger than type", sizeof(
unsigned char), 26u); void* _temp393[ 0u]={}; struct _tagged_arr _temp394={(
void*) _temp393,( void*) _temp393,( void*)( _temp393 + 0u)}; _temp392( _temp395,
_temp396, _temp394);});} goto _LL378; _LL384: if( w > 32){({ void(* _temp397)(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp400= loc; struct _tagged_arr
_temp401= _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u);
void* _temp398[ 0u]={}; struct _tagged_arr _temp399={( void*) _temp398,( void*)
_temp398,( void*)( _temp398 + 0u)}; _temp397( _temp400, _temp401, _temp399);});}
goto _LL378; _LL386: if( w > 64){({ void(* _temp402)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp405= loc; struct _tagged_arr _temp406= _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u); void* _temp403[ 0u]={}; struct _tagged_arr
_temp404={( void*) _temp403,( void*) _temp403,( void*)( _temp403 + 0u)};
_temp402( _temp405, _temp406, _temp404);});} goto _LL378; _LL378:;} goto _LL370;
_LL374:({ void(* _temp407)( struct Cyc_Position_Segment*, struct _tagged_arr fmt,
struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp412=
loc; struct _tagged_arr _temp413= _tag_arr("bitfield %s must have integral type but has type %s",
sizeof( unsigned char), 52u); struct Cyc_Stdio_String_pa_struct _temp415;
_temp415.tag= Cyc_Stdio_String_pa; _temp415.f1=( struct _tagged_arr)* _temp350;{
void* _temp410=( void*)& _temp415; struct Cyc_Stdio_String_pa_struct _temp414;
_temp414.tag= Cyc_Stdio_String_pa; _temp414.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp346);{ void* _temp411=( void*)& _temp414; void* _temp408[ 2u]={ _temp410,
_temp411}; struct _tagged_arr _temp409={( void*) _temp408,( void*) _temp408,(
void*)( _temp408 + 2u)}; _temp407( _temp412, _temp413, _temp409);}}}); goto
_LL370; _LL370:;}}}}; _pop_region( uprev_rgn);} static void Cyc_Tc_tcStructOrUniondecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct _tagged_arr obj,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl* sd){ struct
_tagged_arr* _temp416=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
sd->name))->v)).f2; if( sd->name == 0){({ void(* _temp417)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp421= loc; struct _tagged_arr _temp422= _tag_arr("anonymous %ss are not allowed at top level",
sizeof( unsigned char), 43u); struct Cyc_Stdio_String_pa_struct _temp423;
_temp423.tag= Cyc_Stdio_String_pa; _temp423.f1=( struct _tagged_arr) obj;{ void*
_temp420=( void*)& _temp423; void* _temp418[ 1u]={ _temp420}; struct _tagged_arr
_temp419={( void*) _temp418,( void*) _temp418,( void*)( _temp418 + 1u)};
_temp417( _temp421, _temp422, _temp419);}}); return;}{ struct Cyc_List_List*
atts= sd->attributes; for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp424=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; _LL426: if( _temp424 ==( void*) Cyc_Absyn_Packed_att){
goto _LL427;} else{ goto _LL428;} _LL428: if(( unsigned int) _temp424 > 16u?*((
int*) _temp424) == Cyc_Absyn_Aligned_att: 0){ goto _LL429;} else{ goto _LL430;}
_LL430: goto _LL431; _LL427: continue; _LL429: continue; _LL431:({ void(*
_temp432)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp438= loc;
struct _tagged_arr _temp439= _tag_arr("bad attribute %s in %s %s definition",
sizeof( unsigned char), 37u); struct Cyc_Stdio_String_pa_struct _temp442;
_temp442.tag= Cyc_Stdio_String_pa; _temp442.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd);{ void* _temp435=( void*)&
_temp442; struct Cyc_Stdio_String_pa_struct _temp441; _temp441.tag= Cyc_Stdio_String_pa;
_temp441.f1=( struct _tagged_arr) obj;{ void* _temp436=( void*)& _temp441;
struct Cyc_Stdio_String_pa_struct _temp440; _temp440.tag= Cyc_Stdio_String_pa;
_temp440.f1=( struct _tagged_arr)* _temp416;{ void* _temp437=( void*)& _temp440;
void* _temp433[ 3u]={ _temp435, _temp436, _temp437}; struct _tagged_arr _temp434={(
void*) _temp433,( void*) _temp433,( void*)( _temp433 + 3u)}; _temp432( _temp438,
_temp439, _temp434);}}}}); goto _LL425; _LL425:;}}{ struct Cyc_List_List* tvs=
sd->tvs; for( 0; tvs != 0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){
struct Cyc_Absyn_Conref* _temp443= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); void* _temp444=( void*)
_temp443->v; void* _temp452; _LL446: if( _temp444 ==( void*) Cyc_Absyn_No_constr){
goto _LL447;} else{ goto _LL448;} _LL448: if(( unsigned int) _temp444 > 1u?*((
int*) _temp444) == Cyc_Absyn_Eq_constr: 0){ _LL453: _temp452=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp444)->f1; if( _temp452 ==( void*) Cyc_Absyn_MemKind){
goto _LL449;} else{ goto _LL450;}} else{ goto _LL450;} _LL450: goto _LL451;
_LL447:( void*)( _temp443->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp454=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp454[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp455; _temp455.tag= Cyc_Absyn_Eq_constr;
_temp455.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp455;}); _temp454;})));
continue; _LL449:({ void(* _temp456)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp462= loc; struct _tagged_arr _temp463= _tag_arr("%s %s attempts to abstract type variable %s of kind M",
sizeof( unsigned char), 54u); struct Cyc_Stdio_String_pa_struct _temp466;
_temp466.tag= Cyc_Stdio_String_pa; _temp466.f1=( struct _tagged_arr) obj;{ void*
_temp459=( void*)& _temp466; struct Cyc_Stdio_String_pa_struct _temp465;
_temp465.tag= Cyc_Stdio_String_pa; _temp465.f1=( struct _tagged_arr)* _temp416;{
void* _temp460=( void*)& _temp465; struct Cyc_Stdio_String_pa_struct _temp464;
_temp464.tag= Cyc_Stdio_String_pa; _temp464.f1=( struct _tagged_arr)*(( struct
Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->name;{ void*
_temp461=( void*)& _temp464; void* _temp457[ 3u]={ _temp459, _temp460, _temp461};
struct _tagged_arr _temp458={( void*) _temp457,( void*) _temp457,( void*)(
_temp457 + 3u)}; _temp456( _temp462, _temp463, _temp458);}}}}); continue; _LL451:
continue; _LL445:;}}{ void* _temp467=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v)).f1; struct Cyc_List_List* _temp475; struct Cyc_List_List*
_temp477; _LL469: if(( unsigned int) _temp467 > 1u?*(( int*) _temp467) == Cyc_Absyn_Rel_n:
0){ _LL476: _temp475=(( struct Cyc_Absyn_Rel_n_struct*) _temp467)->f1; if(
_temp475 == 0){ goto _LL470;} else{ goto _LL471;}} else{ goto _LL471;} _LL471:
if(( unsigned int) _temp467 > 1u?*(( int*) _temp467) == Cyc_Absyn_Abs_n: 0){
_LL478: _temp477=(( struct Cyc_Absyn_Abs_n_struct*) _temp467)->f1; if( _temp477
== 0){ goto _LL472;} else{ goto _LL473;}} else{ goto _LL473;} _LL473: goto
_LL474; _LL470: goto _LL468; _LL472: goto _LL468; _LL474:({ void(* _temp479)(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp482= loc; struct _tagged_arr
_temp483= _tag_arr("qualified declarations are not implemented", sizeof(
unsigned char), 43u); void* _temp480[ 0u]={}; struct _tagged_arr _temp481={(
void*) _temp480,( void*) _temp480,( void*)( _temp480 + 0u)}; _temp479( _temp482,
_temp483, _temp481);}); return; _LL468:;}(*(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v)).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp484=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp484[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp485; _temp485.tag= Cyc_Absyn_Abs_n;
_temp485.f1= te->ns; _temp485;}); _temp484;}); Cyc_Tcutil_check_unique_tvars(
loc, sd->tvs); Cyc_Tcutil_add_tvar_identities( sd->tvs);} struct _tuple4{ struct
Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ; static void Cyc_Tc_tcStructdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Structdecl* sd){ struct _tagged_arr* _temp486=(*(( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)).f2; const
unsigned char* _temp487="struct"; Cyc_Tc_tcStructOrUniondecl( te, ge, _tag_arr(
_temp487, sizeof( unsigned char), 7u), loc, sd);{ struct Cyc_List_List* _temp488=
sd->tvs;{ struct _tuple4 _temp490=({ struct _tuple4 _temp489; _temp489.f1= sd->fields;
_temp489.f2=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup_opt)( ge->structdecls, _temp486); _temp489;});
struct Cyc_Core_Opt* _temp500; struct Cyc_Core_Opt* _temp502; struct Cyc_Core_Opt*
_temp504; struct Cyc_Core_Opt* _temp506; struct Cyc_Core_Opt _temp508; struct
Cyc_List_List* _temp509; struct Cyc_Core_Opt* _temp511; struct Cyc_Core_Opt
_temp513; struct Cyc_Absyn_Structdecl** _temp514; struct Cyc_Core_Opt* _temp516;
struct Cyc_Core_Opt _temp518; struct Cyc_List_List* _temp519; struct Cyc_Core_Opt*
_temp521; struct Cyc_Core_Opt _temp523; struct Cyc_Absyn_Structdecl** _temp524;
struct Cyc_Core_Opt* _temp526; _LL492: _LL503: _temp502= _temp490.f1; if(
_temp502 == 0){ goto _LL501;} else{ goto _LL494;} _LL501: _temp500= _temp490.f2;
if( _temp500 == 0){ goto _LL493;} else{ goto _LL494;} _LL494: _LL507: _temp506=
_temp490.f1; if( _temp506 == 0){ goto _LL496;} else{ _temp508=* _temp506; _LL510:
_temp509=( struct Cyc_List_List*) _temp508.v; goto _LL505;} _LL505: _temp504=
_temp490.f2; if( _temp504 == 0){ goto _LL495;} else{ goto _LL496;} _LL496:
_LL517: _temp516= _temp490.f1; if( _temp516 == 0){ goto _LL498;} else{ _temp518=*
_temp516; _LL520: _temp519=( struct Cyc_List_List*) _temp518.v; goto _LL512;}
_LL512: _temp511= _temp490.f2; if( _temp511 == 0){ goto _LL498;} else{ _temp513=*
_temp511; _LL515: _temp514=( struct Cyc_Absyn_Structdecl**) _temp513.v; goto
_LL497;} _LL498: _LL527: _temp526= _temp490.f1; if( _temp526 == 0){ goto _LL522;}
else{ goto _LL491;} _LL522: _temp521= _temp490.f2; if( _temp521 == 0){ goto
_LL491;} else{ _temp523=* _temp521; _LL525: _temp524=( struct Cyc_Absyn_Structdecl**)
_temp523.v; goto _LL499;} _LL493: ge->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl**
data)) Cyc_Dict_insert)( ge->structdecls, _temp486,({ struct Cyc_Absyn_Structdecl**
_temp528=( struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp528[ 0]= sd; _temp528;})); goto _LL491; _LL495: { struct Cyc_Absyn_Structdecl**
_temp529=({ struct Cyc_Absyn_Structdecl** _temp530=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp530[ 0]=({ struct Cyc_Absyn_Structdecl*
_temp531=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp531->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp531->name= sd->name;
_temp531->tvs= _temp488; _temp531->fields= 0; _temp531->attributes= 0; _temp531;});
_temp530;}); ge->structdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl** data)) Cyc_Dict_insert)(
ge->structdecls, _temp486, _temp529); Cyc_Tc_tcStructFields( te, ge, loc,
_tag_arr( _temp487, sizeof( unsigned char), 7u), _temp509, _temp488);* _temp529=
sd; goto _LL491;} _LL497: { struct Cyc_Absyn_Structdecl* _temp532=* _temp514;*
_temp514=({ struct Cyc_Absyn_Structdecl* _temp533=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp533->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp533->name= sd->name; _temp533->tvs= _temp488; _temp533->fields=
0; _temp533->attributes= 0; _temp533;}); Cyc_Tc_tcStructFields( te, ge, loc,
_tag_arr( _temp487, sizeof( unsigned char), 7u), _temp519, _temp488);* _temp514=
_temp532; _temp524= _temp514; goto _LL499;} _LL499: { struct Cyc_Absyn_Structdecl*
_temp534= Cyc_Tcdecl_merge_structdecl(* _temp524, sd, loc, Cyc_Tc_tc_msg); if(
_temp534 == 0){ return;} else{* _temp524=( struct Cyc_Absyn_Structdecl*)
_check_null( _temp534); sd=( struct Cyc_Absyn_Structdecl*) _check_null( _temp534);
goto _LL491;}} _LL491:;} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,
_temp486,({ struct _tuple3* _temp535=( struct _tuple3*) GC_malloc( sizeof(
struct _tuple3)); _temp535->f1=( void*)({ struct Cyc_Tcenv_StructRes_struct*
_temp536=( struct Cyc_Tcenv_StructRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_StructRes_struct));
_temp536[ 0]=({ struct Cyc_Tcenv_StructRes_struct _temp537; _temp537.tag= Cyc_Tcenv_StructRes;
_temp537.f1= sd; _temp537;}); _temp536;}); _temp535->f2= 1; _temp535;}));}}
static void Cyc_Tc_tcUniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl* ud){ struct
_tagged_arr* _temp538=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
ud->name))->v)).f2; const unsigned char* _temp539="union"; Cyc_Tc_tcStructOrUniondecl(
te, ge, _tag_arr( _temp539, sizeof( unsigned char), 6u), loc,( struct Cyc_Absyn_Structdecl*)
ud);{ struct Cyc_List_List* _temp540= ud->tvs; struct _tuple4 _temp542=({ struct
_tuple4 _temp541; _temp541.f1= ud->fields; _temp541.f2=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)( ge->uniondecls,
_temp538); _temp541;}); struct Cyc_Core_Opt* _temp552; struct Cyc_Core_Opt*
_temp554; struct Cyc_Core_Opt* _temp556; struct Cyc_Core_Opt* _temp558; struct
Cyc_Core_Opt _temp560; struct Cyc_List_List* _temp561; struct Cyc_Core_Opt*
_temp563; struct Cyc_Core_Opt _temp565; struct Cyc_Absyn_Uniondecl** _temp566;
struct Cyc_Core_Opt* _temp568; struct Cyc_Core_Opt _temp570; struct Cyc_List_List*
_temp571; struct Cyc_Core_Opt* _temp573; struct Cyc_Core_Opt _temp575; struct
Cyc_Absyn_Uniondecl** _temp576; struct Cyc_Core_Opt* _temp578; _LL544: _LL555:
_temp554= _temp542.f1; if( _temp554 == 0){ goto _LL553;} else{ goto _LL546;}
_LL553: _temp552= _temp542.f2; if( _temp552 == 0){ goto _LL545;} else{ goto
_LL546;} _LL546: _LL559: _temp558= _temp542.f1; if( _temp558 == 0){ goto _LL548;}
else{ _temp560=* _temp558; _LL562: _temp561=( struct Cyc_List_List*) _temp560.v;
goto _LL557;} _LL557: _temp556= _temp542.f2; if( _temp556 == 0){ goto _LL547;}
else{ goto _LL548;} _LL548: _LL569: _temp568= _temp542.f1; if( _temp568 == 0){
goto _LL550;} else{ _temp570=* _temp568; _LL572: _temp571=( struct Cyc_List_List*)
_temp570.v; goto _LL564;} _LL564: _temp563= _temp542.f2; if( _temp563 == 0){
goto _LL550;} else{ _temp565=* _temp563; _LL567: _temp566=( struct Cyc_Absyn_Uniondecl**)
_temp565.v; goto _LL549;} _LL550: _LL579: _temp578= _temp542.f1; if( _temp578 ==
0){ goto _LL574;} else{ goto _LL543;} _LL574: _temp573= _temp542.f2; if(
_temp573 == 0){ goto _LL543;} else{ _temp575=* _temp573; _LL577: _temp576=(
struct Cyc_Absyn_Uniondecl**) _temp575.v; goto _LL551;} _LL545: ge->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)( ge->uniondecls, _temp538,({
struct Cyc_Absyn_Uniondecl** _temp580=( struct Cyc_Absyn_Uniondecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl*)); _temp580[ 0]= ud; _temp580;})); goto
_LL543; _LL547: { struct Cyc_Absyn_Uniondecl** _temp581=({ struct Cyc_Absyn_Uniondecl**
_temp593=( struct Cyc_Absyn_Uniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl*));
_temp593[ 0]=({ struct Cyc_Absyn_Uniondecl* _temp594=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp594->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp594->name= ud->name; _temp594->tvs= _temp540; _temp594->fields=
0; _temp594->attributes= ud->attributes; _temp594;}); _temp593;}); ge->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)( ge->uniondecls, _temp538,
_temp581); Cyc_Tc_tcStructFields( te, ge, loc, _tag_arr( _temp539, sizeof(
unsigned char), 6u), _temp561, _temp540);{ struct Cyc_List_List* f= _temp561;
for( 0; f != 0; f=(( struct Cyc_List_List*) _check_null( f))->tl){ if( ! Cyc_Tcutil_bits_only((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( f))->hd)->type)){({
void(* _temp582)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp588= loc;
struct _tagged_arr _temp589= _tag_arr("field %s of union %s has type %s which is not `bits-only'",
sizeof( unsigned char), 58u); struct Cyc_Stdio_String_pa_struct _temp592;
_temp592.tag= Cyc_Stdio_String_pa; _temp592.f1=( struct _tagged_arr)*(( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( f))->hd)->name;{
void* _temp585=( void*)& _temp592; struct Cyc_Stdio_String_pa_struct _temp591;
_temp591.tag= Cyc_Stdio_String_pa; _temp591.f1=( struct _tagged_arr)* _temp538;{
void* _temp586=( void*)& _temp591; struct Cyc_Stdio_String_pa_struct _temp590;
_temp590.tag= Cyc_Stdio_String_pa; _temp590.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( f))->hd)->type);{
void* _temp587=( void*)& _temp590; void* _temp583[ 3u]={ _temp585, _temp586,
_temp587}; struct _tagged_arr _temp584={( void*) _temp583,( void*) _temp583,(
void*)( _temp583 + 3u)}; _temp582( _temp588, _temp589, _temp584);}}}});}}}*
_temp581= ud; goto _LL543;} _LL549: { struct Cyc_Absyn_Uniondecl* _temp595=*
_temp566;* _temp566=({ struct Cyc_Absyn_Uniondecl* _temp596=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp596->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp596->name= ud->name; _temp596->tvs= _temp540; _temp596->fields=
0; _temp596->attributes= ud->attributes; _temp596;}); Cyc_Tc_tcStructFields( te,
ge, loc, _tag_arr( _temp539, sizeof( unsigned char), 6u), _temp571, _temp540);*
_temp566= _temp595; _temp576= _temp566; goto _LL551;} _LL551: { struct Cyc_Absyn_Uniondecl*
_temp597= Cyc_Tcdecl_merge_uniondecl(* _temp576, ud, loc, Cyc_Tc_tc_msg); if(
_temp597 == 0){ return;} else{* _temp576=( struct Cyc_Absyn_Uniondecl*)
_check_null( _temp597); goto _LL543;}} _LL543:;}} struct _tuple5{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; static struct Cyc_List_List* Cyc_Tc_tcTunionFields( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct
_tagged_arr obj, int is_xtunion, struct _tuple0* name, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs, struct Cyc_Absyn_Tuniondecl* tudres){{ struct
Cyc_List_List* _temp598= fields; for( 0; _temp598 != 0; _temp598=(( struct Cyc_List_List*)
_check_null( _temp598))->tl){ struct Cyc_Absyn_Tunionfield* _temp599=( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp598))->hd;{
struct Cyc_List_List* tvs= _temp599->tvs; for( 0; tvs != 0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){ struct Cyc_Absyn_Tvar* _temp600=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd; struct Cyc_Absyn_Conref* _temp601=
Cyc_Absyn_compress_conref( _temp600->kind); void* _temp602=( void*) _temp601->v;
void* _temp610; _LL604: if( _temp602 ==( void*) Cyc_Absyn_No_constr){ goto
_LL605;} else{ goto _LL606;} _LL606: if(( unsigned int) _temp602 > 1u?*(( int*)
_temp602) == Cyc_Absyn_Eq_constr: 0){ _LL611: _temp610=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp602)->f1; if( _temp610 ==( void*) Cyc_Absyn_MemKind){ goto _LL607;} else{
goto _LL608;}} else{ goto _LL608;} _LL608: goto _LL609; _LL605:( void*)(
_temp601->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp612=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp612[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp613; _temp613.tag= Cyc_Absyn_Eq_constr;
_temp613.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp613;}); _temp612;})));
goto _LL603; _LL607:({ void(* _temp614)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp618= _temp599->loc; struct _tagged_arr _temp619= _tag_arr("field %s abstracts type variable of kind M",
sizeof( unsigned char), 43u); struct Cyc_Stdio_String_pa_struct _temp620;
_temp620.tag= Cyc_Stdio_String_pa; _temp620.f1=( struct _tagged_arr)*(* _temp599->name).f2;{
void* _temp617=( void*)& _temp620; void* _temp615[ 1u]={ _temp617}; struct
_tagged_arr _temp616={( void*) _temp615,( void*) _temp615,( void*)( _temp615 + 1u)};
_temp614( _temp618, _temp619, _temp616);}}); goto _LL603; _LL609: goto _LL603;
_LL603:;}}{ struct Cyc_List_List* alltvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( tvs, _temp599->tvs); Cyc_Tcutil_check_unique_tvars(
loc, alltvs); Cyc_Tcutil_add_tvar_identities( _temp599->tvs);{ struct Cyc_List_List*
typs= _temp599->typs; for( 0; typs != 0; typs=(( struct Cyc_List_List*)
_check_null( typs))->tl){ Cyc_Tcutil_check_type( _temp599->loc, te, alltvs,(
void*) Cyc_Absyn_MemKind,(*(( struct _tuple5*)(( struct Cyc_List_List*)
_check_null( typs))->hd)).f2);}}{ void* _temp621=(* _temp599->name).f1; struct
Cyc_List_List* _temp631; _LL623: if(( unsigned int) _temp621 > 1u?*(( int*)
_temp621) == Cyc_Absyn_Rel_n: 0){ _LL632: _temp631=(( struct Cyc_Absyn_Rel_n_struct*)
_temp621)->f1; if( _temp631 == 0){ goto _LL624;} else{ goto _LL625;}} else{ goto
_LL625;} _LL625: if(( unsigned int) _temp621 > 1u?*(( int*) _temp621) == Cyc_Absyn_Rel_n:
0){ goto _LL626;} else{ goto _LL627;} _LL627: if(( unsigned int) _temp621 > 1u?*((
int*) _temp621) == Cyc_Absyn_Abs_n: 0){ goto _LL628;} else{ goto _LL629;} _LL629:
if( _temp621 ==( void*) Cyc_Absyn_Loc_n){ goto _LL630;} else{ goto _LL622;}
_LL624: if( is_xtunion){(* _temp599->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp633=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp633[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp634; _temp634.tag= Cyc_Absyn_Abs_n;
_temp634.f1= te->ns; _temp634;}); _temp633;});} else{(* _temp599->name).f1=(*
name).f1;} goto _LL622; _LL626:({ void(* _temp635)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp638= _temp599->loc; struct _tagged_arr _temp639= _tag_arr("qualified declarations are not allowed",
sizeof( unsigned char), 39u); void* _temp636[ 0u]={}; struct _tagged_arr
_temp637={( void*) _temp636,( void*) _temp636,( void*)( _temp636 + 0u)};
_temp635( _temp638, _temp639, _temp637);}); goto _LL622; _LL628: goto _LL622;
_LL630:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp640=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp640[ 0]=({ struct Cyc_Core_Impossible_struct _temp641; _temp641.tag= Cyc_Core_Impossible;
_temp641.f1= _tag_arr("tcTunionFields: Loc_n", sizeof( unsigned char), 22u);
_temp641;}); _temp640;})); goto _LL622; _LL622:;}}}}{ struct Cyc_List_List*
fields2; if( is_xtunion){ int _temp642= 1; struct Cyc_List_List* _temp643= Cyc_Tcdecl_sort_xtunion_fields(
fields,& _temp642,(* name).f2, loc, Cyc_Tc_tc_msg); if( _temp642){ fields2=
_temp643;} else{ fields2= 0;}} else{ struct _RegionHandle _temp644= _new_region();
struct _RegionHandle* uprev_rgn=& _temp644; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0;{ struct Cyc_List_List* fs= fields; for( 0; fs !=
0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){ struct Cyc_Absyn_Tunionfield*
_temp645=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fs))->hd; if((( int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*),
struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp,
prev_fields,(* _temp645->name).f2)){({ void(* _temp646)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp651= _temp645->loc; struct _tagged_arr _temp652= _tag_arr("duplicate field name %s in %s",
sizeof( unsigned char), 30u); struct Cyc_Stdio_String_pa_struct _temp654;
_temp654.tag= Cyc_Stdio_String_pa; _temp654.f1=( struct _tagged_arr)*(* _temp645->name).f2;{
void* _temp649=( void*)& _temp654; struct Cyc_Stdio_String_pa_struct _temp653;
_temp653.tag= Cyc_Stdio_String_pa; _temp653.f1=( struct _tagged_arr) obj;{ void*
_temp650=( void*)& _temp653; void* _temp647[ 2u]={ _temp649, _temp650}; struct
_tagged_arr _temp648={( void*) _temp647,( void*) _temp647,( void*)( _temp647 + 2u)};
_temp646( _temp651, _temp652, _temp648);}}});} else{ prev_fields=({ struct Cyc_List_List*
_temp655=( struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp655->hd=( void*)(* _temp645->name).f2; _temp655->tl= prev_fields; _temp655;});}
if(( void*) _temp645->sc !=( void*) Cyc_Absyn_Public){({ void(* _temp656)(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_warn; struct Cyc_Position_Segment* _temp660= loc; struct _tagged_arr
_temp661= _tag_arr("ignoring scope of field %s", sizeof( unsigned char), 27u);
struct Cyc_Stdio_String_pa_struct _temp662; _temp662.tag= Cyc_Stdio_String_pa;
_temp662.f1=( struct _tagged_arr)*(* _temp645->name).f2;{ void* _temp659=( void*)&
_temp662; void* _temp657[ 1u]={ _temp659}; struct _tagged_arr _temp658={( void*)
_temp657,( void*) _temp657,( void*)( _temp657 + 1u)}; _temp656( _temp660,
_temp661, _temp658);}});( void*)( _temp645->sc=( void*)(( void*) Cyc_Absyn_Public));}}}
fields2= fields;}; _pop_region( uprev_rgn);}{ struct Cyc_List_List* _temp663=
fields; for( 0; _temp663 != 0; _temp663=(( struct Cyc_List_List*) _check_null(
_temp663))->tl){ struct Cyc_Absyn_Tunionfield* _temp664=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp663))->hd; ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries,(* _temp664->name).f2,({ struct _tuple3* _temp665=( struct
_tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp665->f1=( void*)({ struct
Cyc_Tcenv_TunionRes_struct* _temp666=( struct Cyc_Tcenv_TunionRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct)); _temp666[ 0]=({ struct
Cyc_Tcenv_TunionRes_struct _temp667; _temp667.tag= Cyc_Tcenv_TunionRes; _temp667.f1=
tudres; _temp667.f2= _temp664; _temp667;}); _temp666;}); _temp665->f2= 1;
_temp665;}));}} return fields2;}} static void Cyc_Tc_tcTuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl*
tud){ struct _tagged_arr* v=(* tud->name).f2; struct _tagged_arr obj; if( tud->is_xtunion){
obj= _tag_arr("xtunion", sizeof( unsigned char), 8u);} else{ obj= _tag_arr("tunion",
sizeof( unsigned char), 7u);}{ struct Cyc_List_List* tvs= tud->tvs;{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 != 0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
struct Cyc_Absyn_Conref* c= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->kind); void* _temp668=( void*)
c->v; void* _temp676; _LL670: if( _temp668 ==( void*) Cyc_Absyn_No_constr){ goto
_LL671;} else{ goto _LL672;} _LL672: if(( unsigned int) _temp668 > 1u?*(( int*)
_temp668) == Cyc_Absyn_Eq_constr: 0){ _LL677: _temp676=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp668)->f1; if( _temp676 ==( void*) Cyc_Absyn_MemKind){ goto _LL673;} else{
goto _LL674;}} else{ goto _LL674;} _LL674: goto _LL675; _LL671:( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp678=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp678[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp679; _temp679.tag= Cyc_Absyn_Eq_constr; _temp679.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp679;}); _temp678;}))); goto _LL669;
_LL673:({ void(* _temp680)( struct Cyc_Position_Segment*, struct _tagged_arr fmt,
struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp686=
loc; struct _tagged_arr _temp687= _tag_arr("%s %s attempts to abstract type variable %s of kind M",
sizeof( unsigned char), 54u); struct Cyc_Stdio_String_pa_struct _temp690;
_temp690.tag= Cyc_Stdio_String_pa; _temp690.f1=( struct _tagged_arr) obj;{ void*
_temp683=( void*)& _temp690; struct Cyc_Stdio_String_pa_struct _temp689;
_temp689.tag= Cyc_Stdio_String_pa; _temp689.f1=( struct _tagged_arr)* v;{ void*
_temp684=( void*)& _temp689; struct Cyc_Stdio_String_pa_struct _temp688;
_temp688.tag= Cyc_Stdio_String_pa; _temp688.f1=( struct _tagged_arr)*(( struct
Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->name;{ void*
_temp685=( void*)& _temp688; void* _temp681[ 3u]={ _temp683, _temp684, _temp685};
struct _tagged_arr _temp682={( void*) _temp681,( void*) _temp681,( void*)(
_temp681 + 3u)}; _temp680( _temp686, _temp687, _temp682);}}}}); goto _LL669;
_LL675: goto _LL669; _LL669:;}} Cyc_Tcutil_check_unique_tvars( loc, tvs); Cyc_Tcutil_add_tvar_identities(
tvs);{ struct Cyc_Core_Opt* tud_opt; if( tud->is_xtunion){{ struct _handler_cons
_temp691; _push_handler(& _temp691);{ int _temp693= 0; if( setjmp( _temp691.handler)){
_temp693= 1;} if( ! _temp693){ tud_opt= Cyc_Tcenv_lookup_xtuniondecl( te, loc,
tud->name);; _pop_handler();} else{ void* _temp692=( void*) _exn_thrown; void*
_temp695= _temp692; _LL697: if( _temp695 == Cyc_Dict_Absent){ goto _LL698;}
else{ goto _LL699;} _LL699: goto _LL700; _LL698:({ void(* _temp701)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp705= loc; struct _tagged_arr _temp706= _tag_arr("qualified xtunion declaration %s is not an existing xtunion",
sizeof( unsigned char), 60u); struct Cyc_Stdio_String_pa_struct _temp707;
_temp707.tag= Cyc_Stdio_String_pa; _temp707.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
tud->name);{ void* _temp704=( void*)& _temp707; void* _temp702[ 1u]={ _temp704};
struct _tagged_arr _temp703={( void*) _temp702,( void*) _temp702,( void*)(
_temp702 + 1u)}; _temp701( _temp705, _temp706, _temp703);}}); return; _LL700:(
void) _throw( _temp695); _LL696:;}}} if( tud_opt != 0){ tud->name=(*(( struct
Cyc_Absyn_Tuniondecl**)(( struct Cyc_Core_Opt*) _check_null( tud_opt))->v))->name;}
else{(* tud->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp708=(
struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp708[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp709; _temp709.tag= Cyc_Absyn_Abs_n;
_temp709.f1= te->ns; _temp709;}); _temp708;});}} else{{ void* _temp710=(* tud->name).f1;
struct Cyc_List_List* _temp718; _LL712: if(( unsigned int) _temp710 > 1u?*(( int*)
_temp710) == Cyc_Absyn_Rel_n: 0){ _LL719: _temp718=(( struct Cyc_Absyn_Rel_n_struct*)
_temp710)->f1; if( _temp718 == 0){ goto _LL713;} else{ goto _LL714;}} else{ goto
_LL714;} _LL714: if(( unsigned int) _temp710 > 1u?*(( int*) _temp710) == Cyc_Absyn_Abs_n:
0){ goto _LL715;} else{ goto _LL716;} _LL716: goto _LL717; _LL713:(* tud->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp720=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp720[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp721; _temp721.tag= Cyc_Absyn_Abs_n; _temp721.f1= te->ns; _temp721;});
_temp720;}); goto _LL711; _LL715: goto _LL717; _LL717:({ void(* _temp722)(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp725= loc; struct _tagged_arr
_temp726= _tag_arr("qualified declarations are not implemented", sizeof(
unsigned char), 43u); void* _temp723[ 0u]={}; struct _tagged_arr _temp724={(
void*) _temp723,( void*) _temp723,( void*)( _temp723 + 0u)}; _temp722( _temp725,
_temp726, _temp724);}); return; _LL711:;} tud_opt=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)( ge->tuniondecls,
v);}{ struct _tuple4 _temp728=({ struct _tuple4 _temp727; _temp727.f1= tud->fields;
_temp727.f2= tud_opt; _temp727;}); struct Cyc_Core_Opt* _temp738; struct Cyc_Core_Opt*
_temp740; struct Cyc_Core_Opt* _temp742; struct Cyc_Core_Opt* _temp744; struct
Cyc_Core_Opt _temp746; struct Cyc_List_List* _temp747; struct Cyc_List_List**
_temp749; struct Cyc_Core_Opt* _temp750; struct Cyc_Core_Opt _temp752; struct
Cyc_Absyn_Tuniondecl** _temp753; struct Cyc_Core_Opt* _temp755; struct Cyc_Core_Opt
_temp757; struct Cyc_List_List* _temp758; struct Cyc_List_List** _temp760;
struct Cyc_Core_Opt* _temp761; struct Cyc_Core_Opt _temp763; struct Cyc_Absyn_Tuniondecl**
_temp764; struct Cyc_Core_Opt* _temp766; _LL730: _LL741: _temp740= _temp728.f1;
if( _temp740 == 0){ goto _LL739;} else{ goto _LL732;} _LL739: _temp738= _temp728.f2;
if( _temp738 == 0){ goto _LL731;} else{ goto _LL732;} _LL732: _LL745: _temp744=
_temp728.f1; if( _temp744 == 0){ goto _LL734;} else{ _temp746=* _temp744; _LL748:
_temp747=( struct Cyc_List_List*) _temp746.v; _temp749=&(* _temp728.f1).v; goto
_LL743;} _LL743: _temp742= _temp728.f2; if( _temp742 == 0){ goto _LL733;} else{
goto _LL734;} _LL734: _LL756: _temp755= _temp728.f1; if( _temp755 == 0){ goto
_LL736;} else{ _temp757=* _temp755; _LL759: _temp758=( struct Cyc_List_List*)
_temp757.v; _temp760=&(* _temp728.f1).v; goto _LL751;} _LL751: _temp750=
_temp728.f2; if( _temp750 == 0){ goto _LL736;} else{ _temp752=* _temp750; _LL754:
_temp753=( struct Cyc_Absyn_Tuniondecl**) _temp752.v; goto _LL735;} _LL736:
_LL767: _temp766= _temp728.f1; if( _temp766 == 0){ goto _LL762;} else{ goto
_LL729;} _LL762: _temp761= _temp728.f2; if( _temp761 == 0){ goto _LL729;} else{
_temp763=* _temp761; _LL765: _temp764=( struct Cyc_Absyn_Tuniondecl**) _temp763.v;
goto _LL737;} _LL731: ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)(
ge->tuniondecls, v,({ struct Cyc_Absyn_Tuniondecl** _temp768=( struct Cyc_Absyn_Tuniondecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp768[ 0]= tud; _temp768;}));
goto _LL729; _LL733: { struct Cyc_Absyn_Tuniondecl** _temp769=({ struct Cyc_Absyn_Tuniondecl**
_temp770=( struct Cyc_Absyn_Tuniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*));
_temp770[ 0]=({ struct Cyc_Absyn_Tuniondecl* _temp771=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp771->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp771->name= tud->name; _temp771->tvs= tvs; _temp771->fields=
0; _temp771->is_xtunion= tud->is_xtunion; _temp771;}); _temp770;}); ge->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)( ge->tuniondecls, v,
_temp769);* _temp749= Cyc_Tc_tcTunionFields( te, ge, loc, obj, tud->is_xtunion,
tud->name,* _temp749, tvs, tud);* _temp769= tud; goto _LL729;} _LL735: { struct
Cyc_Absyn_Tuniondecl* _temp772=* _temp753;* _temp753=({ struct Cyc_Absyn_Tuniondecl*
_temp773=( struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp773->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp773->name= tud->name;
_temp773->tvs= tvs; _temp773->fields= 0; _temp773->is_xtunion= tud->is_xtunion;
_temp773;});* _temp760= Cyc_Tc_tcTunionFields( te, ge, loc, obj, tud->is_xtunion,
tud->name,* _temp760, tvs, tud);* _temp753= _temp772; _temp764= _temp753; goto
_LL737;} _LL737: { struct Cyc_Absyn_Tuniondecl* _temp774= Cyc_Tcdecl_merge_tuniondecl(*
_temp764, tud, loc, Cyc_Tc_tc_msg); if( _temp774 == 0){ return;} else{* _temp764=(
struct Cyc_Absyn_Tuniondecl*) _check_null( _temp774); goto _LL729;}} _LL729:;}}}}
static void Cyc_Tc_tcEnumdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Enumdecl* ed){ struct
_tagged_arr* v=(* ed->name).f2;{ void* _temp775=(* ed->name).f1; struct Cyc_List_List*
_temp783; struct Cyc_List_List* _temp785; _LL777: if(( unsigned int) _temp775 >
1u?*(( int*) _temp775) == Cyc_Absyn_Rel_n: 0){ _LL784: _temp783=(( struct Cyc_Absyn_Rel_n_struct*)
_temp775)->f1; if( _temp783 == 0){ goto _LL778;} else{ goto _LL779;}} else{ goto
_LL779;} _LL779: if(( unsigned int) _temp775 > 1u?*(( int*) _temp775) == Cyc_Absyn_Abs_n:
0){ _LL786: _temp785=(( struct Cyc_Absyn_Abs_n_struct*) _temp775)->f1; if(
_temp785 == 0){ goto _LL780;} else{ goto _LL781;}} else{ goto _LL781;} _LL781:
goto _LL782; _LL778: goto _LL776; _LL780: goto _LL776; _LL782:({ void(* _temp787)(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp790= loc; struct _tagged_arr
_temp791= _tag_arr("qualified declarations are not implemented", sizeof(
unsigned char), 43u); void* _temp788[ 0u]={}; struct _tagged_arr _temp789={(
void*) _temp788,( void*) _temp788,( void*)( _temp788 + 0u)}; _temp787( _temp790,
_temp791, _temp789);}); return; _LL776:;}(* ed->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp792=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp792[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp793; _temp793.tag= Cyc_Absyn_Abs_n;
_temp793.f1= te->ns; _temp793;}); _temp792;}); if( ed->fields != 0){ struct
_RegionHandle _temp794= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp794; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0;
unsigned int tag_count= 0; struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( ed->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ struct Cyc_Absyn_Enumfield*
_temp795=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( fs))->hd;
if((( int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*), struct
Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp,
prev_fields,(* _temp795->name).f2)){({ void(* _temp796)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp800= _temp795->loc; struct _tagged_arr _temp801= _tag_arr("duplicate field name %s",
sizeof( unsigned char), 24u); struct Cyc_Stdio_String_pa_struct _temp802;
_temp802.tag= Cyc_Stdio_String_pa; _temp802.f1=( struct _tagged_arr)*(* _temp795->name).f2;{
void* _temp799=( void*)& _temp802; void* _temp797[ 1u]={ _temp799}; struct
_tagged_arr _temp798={( void*) _temp797,( void*) _temp797,( void*)( _temp797 + 1u)};
_temp796( _temp800, _temp801, _temp798);}});} else{ prev_fields=({ struct Cyc_List_List*
_temp803=( struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp803->hd=( void*)(* _temp795->name).f2; _temp803->tl= prev_fields; _temp803;});}
if( _temp795->tag == 0){ _temp795->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
tag_count, _temp795->loc);} else{ if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp795->tag))){({ void(* _temp804)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp809= loc; struct _tagged_arr _temp810= _tag_arr("enum %s, field %s: expression is not constant",
sizeof( unsigned char), 46u); struct Cyc_Stdio_String_pa_struct _temp812;
_temp812.tag= Cyc_Stdio_String_pa; _temp812.f1=( struct _tagged_arr)* v;{ void*
_temp807=( void*)& _temp812; struct Cyc_Stdio_String_pa_struct _temp811;
_temp811.tag= Cyc_Stdio_String_pa; _temp811.f1=( struct _tagged_arr)*(* _temp795->name).f2;{
void* _temp808=( void*)& _temp811; void* _temp805[ 2u]={ _temp807, _temp808};
struct _tagged_arr _temp806={( void*) _temp805,( void*) _temp805,( void*)(
_temp805 + 2u)}; _temp804( _temp809, _temp810, _temp806);}}});}}{ unsigned int
t1= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*) _check_null( _temp795->tag));
tag_count= t1 + 1;(* _temp795->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp813=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp813[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp814; _temp814.tag= Cyc_Absyn_Abs_n;
_temp814.f1= te->ns; _temp814;}); _temp813;});}}}; _pop_region( uprev_rgn);}{
struct _handler_cons _temp815; _push_handler(& _temp815);{ int _temp817= 0; if(
setjmp( _temp815.handler)){ _temp817= 1;} if( ! _temp817){{ struct Cyc_Absyn_Enumdecl**
_temp818=(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup)( ge->enumdecls, v); struct Cyc_Absyn_Enumdecl*
_temp819= Cyc_Tcdecl_merge_enumdecl(* _temp818, ed, loc, Cyc_Tc_tc_msg); if(
_temp819 == 0){ _npop_handler( 0u); return;}* _temp818=( struct Cyc_Absyn_Enumdecl*)
_check_null( _temp819);}; _pop_handler();} else{ void* _temp816=( void*)
_exn_thrown; void* _temp821= _temp816; _LL823: if( _temp821 == Cyc_Dict_Absent){
goto _LL824;} else{ goto _LL825;} _LL825: goto _LL826; _LL824: { struct Cyc_Absyn_Enumdecl**
_temp827=({ struct Cyc_Absyn_Enumdecl** _temp828=( struct Cyc_Absyn_Enumdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl*)); _temp828[ 0]= ed; _temp828;});
ge->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key, struct Cyc_Absyn_Enumdecl** data)) Cyc_Dict_insert)( ge->enumdecls,
v, _temp827); goto _LL822;} _LL826:( void) _throw( _temp821); _LL822:;}}} if( ed->fields
!= 0){ struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ed->fields))->v; for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ struct Cyc_Absyn_Enumfield* _temp829=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_List_List*) _check_null( fs))->hd; ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries,(* _temp829->name).f2,({ struct _tuple3* _temp830=( struct
_tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp830->f1=( void*)({ struct
Cyc_Tcenv_EnumRes_struct* _temp831=( struct Cyc_Tcenv_EnumRes_struct*) GC_malloc(
sizeof( struct Cyc_Tcenv_EnumRes_struct)); _temp831[ 0]=({ struct Cyc_Tcenv_EnumRes_struct
_temp832; _temp832.tag= Cyc_Tcenv_EnumRes; _temp832.f1= ed; _temp832.f2=
_temp829; _temp832;}); _temp831;}); _temp830->f2= 1; _temp830;}));}}} static int
Cyc_Tc_okay_externC( struct Cyc_Position_Segment* loc, void* sc){ void* _temp833=
sc; _LL835: if( _temp833 ==( void*) Cyc_Absyn_Static){ goto _LL836;} else{ goto
_LL837;} _LL837: if( _temp833 ==( void*) Cyc_Absyn_Abstract){ goto _LL838;}
else{ goto _LL839;} _LL839: if( _temp833 ==( void*) Cyc_Absyn_Public){ goto
_LL840;} else{ goto _LL841;} _LL841: if( _temp833 ==( void*) Cyc_Absyn_Extern){
goto _LL842;} else{ goto _LL843;} _LL843: if( _temp833 ==( void*) Cyc_Absyn_ExternC){
goto _LL844;} else{ goto _LL834;} _LL836:({ void(* _temp845)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_warn; struct Cyc_Position_Segment*
_temp848= loc; struct _tagged_arr _temp849= _tag_arr("static declaration nested within extern \"C\"",
sizeof( unsigned char), 44u); void* _temp846[ 0u]={}; struct _tagged_arr
_temp847={( void*) _temp846,( void*) _temp846,( void*)( _temp846 + 0u)};
_temp845( _temp848, _temp849, _temp847);}); return 0; _LL838:({ void(* _temp850)(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_warn; struct Cyc_Position_Segment* _temp853= loc; struct _tagged_arr
_temp854= _tag_arr("abstract declaration nested within extern \"C\"", sizeof(
unsigned char), 46u); void* _temp851[ 0u]={}; struct _tagged_arr _temp852={(
void*) _temp851,( void*) _temp851,( void*)( _temp851 + 0u)}; _temp850( _temp853,
_temp854, _temp852);}); return 0; _LL840: return 1; _LL842: return 1; _LL844:({
void(* _temp855)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_warn; struct Cyc_Position_Segment* _temp858= loc;
struct _tagged_arr _temp859= _tag_arr("nested extern \"C\" declaration", sizeof(
unsigned char), 30u); void* _temp856[ 0u]={}; struct _tagged_arr _temp857={(
void*) _temp856,( void*) _temp856,( void*)( _temp856 + 0u)}; _temp855( _temp858,
_temp859, _temp857);}); return 1; _LL834:;} static void Cyc_Tc_tc_decls( struct
Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds0, int in_externC, int
check_var_init){ struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, te->ns);
struct Cyc_List_List* _temp860= ds0; for( 0; _temp860 != 0; _temp860=(( struct
Cyc_List_List*) _check_null( _temp860))->tl){ struct Cyc_Absyn_Decl* d=( struct
Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( _temp860))->hd; struct
Cyc_Position_Segment* loc= d->loc; void* _temp861=( void*) d->r; struct Cyc_Absyn_Vardecl*
_temp887; struct Cyc_Absyn_Fndecl* _temp889; struct Cyc_Absyn_Typedefdecl*
_temp891; struct Cyc_Absyn_Structdecl* _temp893; struct Cyc_Absyn_Uniondecl*
_temp895; struct Cyc_Absyn_Tuniondecl* _temp897; struct Cyc_Absyn_Enumdecl*
_temp899; struct Cyc_List_List* _temp901; struct _tagged_arr* _temp903; struct
Cyc_List_List* _temp905; struct _tuple0* _temp907; struct _tuple0 _temp909;
struct _tagged_arr* _temp910; void* _temp912; struct Cyc_List_List* _temp914;
_LL863: if(*(( int*) _temp861) == Cyc_Absyn_Let_d){ goto _LL864;} else{ goto
_LL865;} _LL865: if(*(( int*) _temp861) == Cyc_Absyn_Letv_d){ goto _LL866;}
else{ goto _LL867;} _LL867: if(*(( int*) _temp861) == Cyc_Absyn_Var_d){ _LL888:
_temp887=(( struct Cyc_Absyn_Var_d_struct*) _temp861)->f1; goto _LL868;} else{
goto _LL869;} _LL869: if(*(( int*) _temp861) == Cyc_Absyn_Fn_d){ _LL890:
_temp889=(( struct Cyc_Absyn_Fn_d_struct*) _temp861)->f1; goto _LL870;} else{
goto _LL871;} _LL871: if(*(( int*) _temp861) == Cyc_Absyn_Typedef_d){ _LL892:
_temp891=(( struct Cyc_Absyn_Typedef_d_struct*) _temp861)->f1; goto _LL872;}
else{ goto _LL873;} _LL873: if(*(( int*) _temp861) == Cyc_Absyn_Struct_d){
_LL894: _temp893=(( struct Cyc_Absyn_Struct_d_struct*) _temp861)->f1; goto
_LL874;} else{ goto _LL875;} _LL875: if(*(( int*) _temp861) == Cyc_Absyn_Union_d){
_LL896: _temp895=(( struct Cyc_Absyn_Union_d_struct*) _temp861)->f1; goto _LL876;}
else{ goto _LL877;} _LL877: if(*(( int*) _temp861) == Cyc_Absyn_Tunion_d){
_LL898: _temp897=(( struct Cyc_Absyn_Tunion_d_struct*) _temp861)->f1; goto
_LL878;} else{ goto _LL879;} _LL879: if(*(( int*) _temp861) == Cyc_Absyn_Enum_d){
_LL900: _temp899=(( struct Cyc_Absyn_Enum_d_struct*) _temp861)->f1; goto _LL880;}
else{ goto _LL881;} _LL881: if(*(( int*) _temp861) == Cyc_Absyn_Namespace_d){
_LL904: _temp903=(( struct Cyc_Absyn_Namespace_d_struct*) _temp861)->f1; goto
_LL902; _LL902: _temp901=(( struct Cyc_Absyn_Namespace_d_struct*) _temp861)->f2;
goto _LL882;} else{ goto _LL883;} _LL883: if(*(( int*) _temp861) == Cyc_Absyn_Using_d){
_LL908: _temp907=(( struct Cyc_Absyn_Using_d_struct*) _temp861)->f1; _temp909=*
_temp907; _LL913: _temp912= _temp909.f1; goto _LL911; _LL911: _temp910= _temp909.f2;
goto _LL906; _LL906: _temp905=(( struct Cyc_Absyn_Using_d_struct*) _temp861)->f2;
goto _LL884;} else{ goto _LL885;} _LL885: if(*(( int*) _temp861) == Cyc_Absyn_ExternC_d){
_LL915: _temp914=(( struct Cyc_Absyn_ExternC_d_struct*) _temp861)->f1; goto
_LL886;} else{ goto _LL862;} _LL864:({ void(* _temp916)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp919= loc; struct _tagged_arr _temp920= _tag_arr("top level let-declarations are not implemented",
sizeof( unsigned char), 47u); void* _temp917[ 0u]={}; struct _tagged_arr
_temp918={( void*) _temp917,( void*) _temp917,( void*)( _temp917 + 0u)};
_temp916( _temp919, _temp920, _temp918);}); goto _LL862; _LL866:({ void(*
_temp921)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp924= loc;
struct _tagged_arr _temp925= _tag_arr("top level let-declarations are not implemented",
sizeof( unsigned char), 47u); void* _temp922[ 0u]={}; struct _tagged_arr
_temp923={( void*) _temp922,( void*) _temp922,( void*)( _temp922 + 0u)};
_temp921( _temp924, _temp925, _temp923);}); goto _LL862; _LL868: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp887->sc): 0){( void*)( _temp887->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcVardecl( te, ge, loc, _temp887,
check_var_init); goto _LL862; _LL870: if( in_externC? Cyc_Tc_okay_externC( d->loc,(
void*) _temp889->sc): 0){( void*)( _temp889->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcFndecl( te, ge, loc, _temp889); goto _LL862; _LL872: Cyc_Tc_tcTypedefdecl(
te, ge, loc, _temp891); goto _LL862; _LL874: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp893->sc): 0){( void*)( _temp893->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcStructdecl( te, ge, loc, _temp893); goto _LL862; _LL876: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp895->sc): 0){( void*)( _temp895->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcUniondecl( te, ge, loc, _temp895);
goto _LL862; _LL878: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*)
_temp897->sc): 0){( void*)( _temp897->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcTuniondecl( te, ge, loc, _temp897); goto _LL862; _LL880: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp899->sc): 0){( void*)( _temp899->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcEnumdecl( te, ge, loc, _temp899);
goto _LL862; _LL882: { struct Cyc_List_List* _temp926= te->ns; struct Cyc_List_List*
_temp927=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp926,({ struct Cyc_List_List* _temp928=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp928->hd=( void*) _temp903;
_temp928->tl= 0; _temp928;})); if( !(( int(*)( struct Cyc_Set_Set* s, struct
_tagged_arr* elt)) Cyc_Set_member)( ge->namespaces, _temp903)){ ge->namespaces=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)(
ge->namespaces, _temp903); te->ae=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key, struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)( te->ae,
_temp927, Cyc_Tcenv_empty_genv());} te->ns= _temp927; Cyc_Tc_tc_decls( te,
_temp901, in_externC, check_var_init); te->ns= _temp926; goto _LL862;} _LL884: {
struct _tagged_arr* first; struct Cyc_List_List* rest;{ void* _temp929= _temp912;
struct Cyc_List_List* _temp941; struct Cyc_List_List* _temp943; struct Cyc_List_List*
_temp945; struct Cyc_List_List _temp947; struct Cyc_List_List* _temp948; struct
_tagged_arr* _temp950; struct Cyc_List_List* _temp952; struct Cyc_List_List
_temp954; struct Cyc_List_List* _temp955; struct _tagged_arr* _temp957; _LL931:
if( _temp929 ==( void*) Cyc_Absyn_Loc_n){ goto _LL932;} else{ goto _LL933;}
_LL933: if(( unsigned int) _temp929 > 1u?*(( int*) _temp929) == Cyc_Absyn_Rel_n:
0){ _LL942: _temp941=(( struct Cyc_Absyn_Rel_n_struct*) _temp929)->f1; if(
_temp941 == 0){ goto _LL934;} else{ goto _LL935;}} else{ goto _LL935;} _LL935:
if(( unsigned int) _temp929 > 1u?*(( int*) _temp929) == Cyc_Absyn_Abs_n: 0){
_LL944: _temp943=(( struct Cyc_Absyn_Abs_n_struct*) _temp929)->f1; if( _temp943
== 0){ goto _LL936;} else{ goto _LL937;}} else{ goto _LL937;} _LL937: if((
unsigned int) _temp929 > 1u?*(( int*) _temp929) == Cyc_Absyn_Rel_n: 0){ _LL946:
_temp945=(( struct Cyc_Absyn_Rel_n_struct*) _temp929)->f1; if( _temp945 == 0){
goto _LL939;} else{ _temp947=* _temp945; _LL951: _temp950=( struct _tagged_arr*)
_temp947.hd; goto _LL949; _LL949: _temp948= _temp947.tl; goto _LL938;}} else{
goto _LL939;} _LL939: if(( unsigned int) _temp929 > 1u?*(( int*) _temp929) ==
Cyc_Absyn_Abs_n: 0){ _LL953: _temp952=(( struct Cyc_Absyn_Abs_n_struct*)
_temp929)->f1; if( _temp952 == 0){ goto _LL930;} else{ _temp954=* _temp952;
_LL958: _temp957=( struct _tagged_arr*) _temp954.hd; goto _LL956; _LL956:
_temp955= _temp954.tl; goto _LL940;}} else{ goto _LL930;} _LL932: goto _LL934;
_LL934: goto _LL936; _LL936: first= _temp910; rest= 0; goto _LL930; _LL938:
_temp957= _temp950; _temp955= _temp948; goto _LL940; _LL940: first= _temp957;
rest=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp955,({ struct Cyc_List_List* _temp959=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp959->hd=( void*) _temp910;
_temp959->tl= 0; _temp959;})); goto _LL930; _LL930:;}{ struct Cyc_List_List*
_temp960= Cyc_Tcenv_resolve_namespace( te, loc, first, rest); ge->availables=({
struct Cyc_List_List* _temp961=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp961->hd=( void*) _temp960; _temp961->tl= ge->availables;
_temp961;}); Cyc_Tc_tc_decls( te, _temp905, in_externC, check_var_init); ge->availables=((
struct Cyc_List_List*) _check_null( ge->availables))->tl; goto _LL862;}} _LL886:
Cyc_Tc_tc_decls( te, _temp914, 1, check_var_init); goto _LL862; _LL862:;}} void
Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int check_var_init, struct Cyc_List_List*
ds){ Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r); Cyc_Tc_tc_decls( te, ds,
0, check_var_init);} static int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env,
struct Cyc_Absyn_Decl* d); static struct Cyc_List_List* Cyc_Tc_treeshake_f(
struct Cyc_Dict_Dict* env, struct Cyc_List_List* ds){ return(( struct Cyc_List_List*(*)(
int(* f)( struct Cyc_Dict_Dict*, struct Cyc_Absyn_Decl*), struct Cyc_Dict_Dict*
env, struct Cyc_List_List* l)) Cyc_List_filter_c)( Cyc_Tc_vardecl_needed, env,
ds);} struct _tuple6{ struct Cyc_Tcenv_Genv* f1; struct Cyc_Set_Set* f2; } ;
static int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl*
d){ void* _temp962=( void*) d->r; struct Cyc_Absyn_Vardecl* _temp972; struct Cyc_List_List*
_temp974; struct Cyc_List_List** _temp976; struct Cyc_List_List* _temp977;
struct Cyc_List_List** _temp979; _LL964: if(*(( int*) _temp962) == Cyc_Absyn_Var_d){
_LL973: _temp972=(( struct Cyc_Absyn_Var_d_struct*) _temp962)->f1; goto _LL965;}
else{ goto _LL966;} _LL966: if(*(( int*) _temp962) == Cyc_Absyn_Using_d){ _LL975:
_temp974=(( struct Cyc_Absyn_Using_d_struct*) _temp962)->f2; _temp976=&(( struct
Cyc_Absyn_Using_d_struct*) _temp962)->f2; goto _LL967;} else{ goto _LL968;}
_LL968: if(*(( int*) _temp962) == Cyc_Absyn_Namespace_d){ _LL978: _temp977=((
struct Cyc_Absyn_Namespace_d_struct*) _temp962)->f2; _temp979=&(( struct Cyc_Absyn_Namespace_d_struct*)
_temp962)->f2; goto _LL969;} else{ goto _LL970;} _LL970: goto _LL971; _LL965:
if(( void*) _temp972->sc !=( void*) Cyc_Absyn_Extern){ return 1;}{ struct
_tuple0 _temp982; struct _tagged_arr* _temp983; void* _temp985; struct _tuple0*
_temp980= _temp972->name; _temp982=* _temp980; _LL986: _temp985= _temp982.f1;
goto _LL984; _LL984: _temp983= _temp982.f2; goto _LL981; _LL981: { struct Cyc_List_List*
ns;{ void* _temp987= _temp985; struct Cyc_List_List* _temp995; struct Cyc_List_List*
_temp997; _LL989: if( _temp987 ==( void*) Cyc_Absyn_Loc_n){ goto _LL990;} else{
goto _LL991;} _LL991: if(( unsigned int) _temp987 > 1u?*(( int*) _temp987) ==
Cyc_Absyn_Rel_n: 0){ _LL996: _temp995=(( struct Cyc_Absyn_Rel_n_struct*)
_temp987)->f1; goto _LL992;} else{ goto _LL993;} _LL993: if(( unsigned int)
_temp987 > 1u?*(( int*) _temp987) == Cyc_Absyn_Abs_n: 0){ _LL998: _temp997=((
struct Cyc_Absyn_Abs_n_struct*) _temp987)->f1; goto _LL994;} else{ goto _LL988;}
_LL990: ns= 0; goto _LL988; _LL992: ns= _temp995; goto _LL988; _LL994: ns=
_temp997; goto _LL988; _LL988:;}{ struct _tuple6* _temp999=(( struct _tuple6*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( env, ns);
struct Cyc_Tcenv_Genv* _temp1000=(* _temp999).f1; int _temp1001=(*(( struct
_tuple3*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
_temp1000->ordinaries, _temp983)).f2; if( ! _temp1001){(* _temp999).f2=(( struct
Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)((*
_temp999).f2, _temp983);} return _temp1001;}}} _LL967: _temp979= _temp976; goto
_LL969; _LL969:* _temp979= Cyc_Tc_treeshake_f( env,* _temp979); return 1; _LL971:
return 1; _LL963:;} static int Cyc_Tc_treeshake_remove_f( struct Cyc_Set_Set*
set, struct _tagged_arr* x, struct _tuple3* y){ return !(( int(*)( struct Cyc_Set_Set*
s, struct _tagged_arr* elt)) Cyc_Set_member)( set, x);} static void Cyc_Tc_treeshake_remove(
struct Cyc_List_List* ns, struct _tuple6* nsenv){ struct Cyc_Tcenv_Genv*
_temp1002=(* nsenv).f1; struct Cyc_Set_Set* _temp1003=(* nsenv).f2; _temp1002->ordinaries=((
struct Cyc_Dict_Dict*(*)( int(* f)( struct Cyc_Set_Set*, struct _tagged_arr*,
struct _tuple3*), struct Cyc_Set_Set* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_filter_c)(
Cyc_Tc_treeshake_remove_f, _temp1003, _temp1002->ordinaries);} static struct
_tuple6* Cyc_Tc_treeshake_make_env_f( struct Cyc_Tcenv_Genv* ge){ return({
struct _tuple6* _temp1004=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp1004->f1= ge; _temp1004->f2=(( struct Cyc_Set_Set*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Set_empty)( Cyc_String_strptrcmp);
_temp1004;});} struct Cyc_List_List* Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv* te,
struct Cyc_List_List* ds){ struct Cyc_Dict_Dict* _temp1005=(( struct Cyc_Dict_Dict*(*)(
struct _tuple6*(* f)( struct Cyc_Tcenv_Genv*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)(
Cyc_Tc_treeshake_make_env_f, te->ae); struct Cyc_List_List* _temp1006= Cyc_Tc_treeshake_f(
_temp1005, ds);(( void(*)( void(* f)( struct Cyc_List_List*, struct _tuple6*),
struct Cyc_Dict_Dict* d)) Cyc_Dict_iter)( Cyc_Tc_treeshake_remove, _temp1005);
return _temp1006;}