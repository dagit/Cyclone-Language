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
x); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct _tagged_arr Cyc_Absyn_attribute2string( void*); struct Cyc_PP_Ppstate;
struct Cyc_PP_Out; struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{ int
expand_typedefs: 1; int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1;
int decls_first: 1; int rewrite_temp_tvars: 1; int print_all_tvars: 1; int
print_all_kinds: 1; int print_using_stmts: 1; int print_externC_stmts: 1; int
print_full_evars: 1; int use_curr_namespace: 1; struct Cyc_List_List*
curr_namespace; } ; extern void Cyc_Absynpp_set_params( struct Cyc_Absynpp_Params*
fs); extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r; extern struct
_tagged_arr Cyc_Absynpp_typ2string( void*); extern struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple0*); struct Cyc_Set_Set; extern struct Cyc_Set_Set* Cyc_Set_empty(
int(* comp)( void*, void*)); extern struct Cyc_Set_Set* Cyc_Set_insert( struct
Cyc_Set_Set* s, void* elt); extern int Cyc_Set_member( struct Cyc_Set_Set* s,
void* elt); extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[
11u]; extern int Cyc_Dict_member( struct Cyc_Dict_Dict* d, void* key); extern
struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key, void*
data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key); extern
struct Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void* key);
extern void Cyc_Dict_iter( void(* f)( void*, void*), struct Cyc_Dict_Dict* d);
extern struct Cyc_Dict_Dict* Cyc_Dict_map( void*(* f)( void*), struct Cyc_Dict_Dict*
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
for( 0; atts != 0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ void*
_temp13=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd; _LL15: if((
unsigned int) _temp13 > 16u?*(( int*) _temp13) == Cyc_Absyn_Regparm_att: 0){
goto _LL16;} else{ goto _LL17;} _LL17: if( _temp13 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL18;} else{ goto _LL19;} _LL19: if( _temp13 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL20;} else{ goto _LL21;} _LL21: if( _temp13 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL22;} else{ goto _LL23;} _LL23: if( _temp13 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL24;} else{ goto _LL25;} _LL25: if( _temp13 ==( void*) Cyc_Absyn_Const_att){
goto _LL26;} else{ goto _LL27;} _LL27: goto _LL28; _LL16: goto _LL18; _LL18:
goto _LL20; _LL20: goto _LL22; _LL22: goto _LL24; _LL24: goto _LL26; _LL26:*
_temp11=({ struct Cyc_List_List* _temp29=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp29->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); _temp29->tl=* _temp11; _temp29;}); continue; _LL28:
_temp12=({ struct Cyc_List_List* _temp30=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp30->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); _temp30->tl= _temp12; _temp30;}); continue; _LL14:;}
return _temp12;} _LL6: return(( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_Tcutil_impos)( _tag_arr("transfer_fn_type_atts", sizeof( unsigned char), 22u));
_LL2:;} struct _tuple3{ void* f1; int f2; } ; static void Cyc_Tc_tcVardecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Vardecl* vd, int check_var_init){ struct _tagged_arr* v=(*
vd->name).f2; void* t=( void*) vd->type; void* sc=( void*) vd->sc; struct Cyc_List_List*
atts= vd->attributes;{ void* _temp31=(* vd->name).f1; struct Cyc_List_List*
_temp39; struct Cyc_List_List* _temp41; _LL33: if(( unsigned int) _temp31 > 1u?*((
int*) _temp31) == Cyc_Absyn_Rel_n: 0){ _LL40: _temp39=(( struct Cyc_Absyn_Rel_n_struct*)
_temp31)->f1; if( _temp39 == 0){ goto _LL34;} else{ goto _LL35;}} else{ goto
_LL35;} _LL35: if(( unsigned int) _temp31 > 1u?*(( int*) _temp31) == Cyc_Absyn_Abs_n:
0){ _LL42: _temp41=(( struct Cyc_Absyn_Abs_n_struct*) _temp31)->f1; if( _temp41
== 0){ goto _LL36;} else{ goto _LL37;}} else{ goto _LL37;} _LL37: goto _LL38;
_LL34: goto _LL32; _LL36: goto _LL32; _LL38: Cyc_Tcutil_terr( loc, _tag_arr("qualified declarations are not implemented",
sizeof( unsigned char), 43u)); return; _LL32:;}(* vd->name).f1=( void*)({ struct
Cyc_Absyn_Abs_n_struct* _temp43=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp43[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp44; _temp44.tag= Cyc_Absyn_Abs_n; _temp44.f1= te->ns; _temp44;}); _temp43;});{
void* _temp45= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp* _temp52; struct
Cyc_Absyn_Tqual _temp54; void* _temp56; _LL47: if(( unsigned int) _temp45 > 4u?*((
int*) _temp45) == Cyc_Absyn_ArrayType: 0){ _LL57: _temp56=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp45)->f1; goto _LL55; _LL55: _temp54=(( struct Cyc_Absyn_ArrayType_struct*)
_temp45)->f2; goto _LL53; _LL53: _temp52=(( struct Cyc_Absyn_ArrayType_struct*)
_temp45)->f3; if( _temp52 == 0){ goto _LL51;} else{ goto _LL49;}} else{ goto
_LL49;} _LL51: if( vd->initializer != 0){ goto _LL48;} else{ goto _LL49;} _LL49:
goto _LL50; _LL48:{ void* _temp58=( void*)(( struct Cyc_Absyn_Exp*) _check_null(
vd->initializer))->r; void* _temp68; struct _tagged_arr _temp70; struct Cyc_List_List*
_temp72; struct Cyc_List_List* _temp74; _LL60: if(*(( int*) _temp58) == Cyc_Absyn_Const_e){
_LL69: _temp68=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp58)->f1; if((
unsigned int) _temp68 > 1u?*(( int*) _temp68) == Cyc_Absyn_String_c: 0){ _LL71:
_temp70=(( struct Cyc_Absyn_String_c_struct*) _temp68)->f1; goto _LL61;} else{
goto _LL62;}} else{ goto _LL62;} _LL62: if(*(( int*) _temp58) == Cyc_Absyn_UnresolvedMem_e){
_LL73: _temp72=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp58)->f2; goto
_LL63;} else{ goto _LL64;} _LL64: if(*(( int*) _temp58) == Cyc_Absyn_Array_e){
_LL75: _temp74=(( struct Cyc_Absyn_Array_e_struct*) _temp58)->f1; goto _LL65;}
else{ goto _LL66;} _LL66: goto _LL67; _LL61: t=( void*)( vd->type=( void*)((
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp76=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp76[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp77; _temp77.tag= Cyc_Absyn_ArrayType; _temp77.f1=(
void*) _temp56; _temp77.f2= _temp54; _temp77.f3=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
_get_arr_size( _temp70, sizeof( unsigned char)), 0); _temp77;}); _temp76;})));
goto _LL59; _LL63: _temp74= _temp72; goto _LL65; _LL65: t=( void*)( vd->type=(
void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp78=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp78[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp79; _temp79.tag= Cyc_Absyn_ArrayType; _temp79.f1=(
void*) _temp56; _temp79.f2= _temp54; _temp79.f3=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp((
unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp74), 0);
_temp79;}); _temp78;}))); goto _LL59; _LL67: goto _LL59; _LL59:;} goto _LL46;
_LL50: goto _LL46; _LL46:;} Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);
if( Cyc_Tcutil_is_function_type( t)){ atts= Cyc_Tc_transfer_fn_type_atts( t,
atts);} if( sc ==( void*) Cyc_Absyn_Extern? 1: sc ==( void*) Cyc_Absyn_ExternC){
if( vd->initializer != 0){ Cyc_Tcutil_terr( loc, _tag_arr("extern declaration should not have initializer",
sizeof( unsigned char), 47u));}} else{ if( ! Cyc_Tcutil_is_function_type( t)){
for( 0; atts != 0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ void*
_temp80=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd; _LL82: if((
unsigned int) _temp80 > 16u?*(( int*) _temp80) == Cyc_Absyn_Aligned_att: 0){
goto _LL83;} else{ goto _LL84;} _LL84: if(( unsigned int) _temp80 > 16u?*(( int*)
_temp80) == Cyc_Absyn_Section_att: 0){ goto _LL85;} else{ goto _LL86;} _LL86:
if( _temp80 ==( void*) Cyc_Absyn_Nocommon_att){ goto _LL87;} else{ goto _LL88;}
_LL88: if( _temp80 ==( void*) Cyc_Absyn_Shared_att){ goto _LL89;} else{ goto
_LL90;} _LL90: if( _temp80 ==( void*) Cyc_Absyn_Unused_att){ goto _LL91;} else{
goto _LL92;} _LL92: if( _temp80 ==( void*) Cyc_Absyn_Weak_att){ goto _LL93;}
else{ goto _LL94;} _LL94: if( _temp80 ==( void*) Cyc_Absyn_Dllimport_att){ goto
_LL95;} else{ goto _LL96;} _LL96: if( _temp80 ==( void*) Cyc_Absyn_Dllexport_att){
goto _LL97;} else{ goto _LL98;} _LL98: goto _LL99; _LL83: continue; _LL85:
continue; _LL87: continue; _LL89: continue; _LL91: continue; _LL93: continue;
_LL95: continue; _LL97: continue; _LL99: Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp100)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr _temp105= _tag_arr("bad attribute %s for variable %s",
sizeof( unsigned char), 33u); struct Cyc_Stdio_String_pa_struct _temp107;
_temp107.tag= Cyc_Stdio_String_pa; _temp107.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd);{ void* _temp103=( void*)&
_temp107; struct Cyc_Stdio_String_pa_struct _temp106; _temp106.tag= Cyc_Stdio_String_pa;
_temp106.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( vd->name);{ void*
_temp104=( void*)& _temp106; void* _temp101[ 2u]={ _temp103, _temp104}; struct
_tagged_arr _temp102={( void*) _temp101,( void*) _temp101,( void*)( _temp101 + 2u)};
_temp100( _temp105, _temp102);}}})); goto _LL81; _LL81:;} if( vd->initializer ==
0){ if( check_var_init? ! Cyc_Tcutil_supports_default( t): 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp108)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr _temp113=
_tag_arr("initializer required for variable %s of type %s", sizeof(
unsigned char), 48u); struct Cyc_Stdio_String_pa_struct _temp115; _temp115.tag=
Cyc_Stdio_String_pa; _temp115.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
vd->name);{ void* _temp111=( void*)& _temp115; struct Cyc_Stdio_String_pa_struct
_temp114; _temp114.tag= Cyc_Stdio_String_pa; _temp114.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp112=( void*)& _temp114; void* _temp109[
2u]={ _temp111, _temp112}; struct _tagged_arr _temp110={( void*) _temp109,( void*)
_temp109,( void*)( _temp109 + 2u)}; _temp108( _temp113, _temp110);}}}));}} else{
struct Cyc_Absyn_Exp* _temp116=( struct Cyc_Absyn_Exp*) _check_null( vd->initializer);
void* _temp117= Cyc_Tcexp_tcExpInitializer( te,( void**)& t, _temp116); if( !
Cyc_Tcutil_coerce_assign( te, _temp116, t)){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp118)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr _temp124= _tag_arr("%s is declared with type %s but initialized with type %s",
sizeof( unsigned char), 57u); struct Cyc_Stdio_String_pa_struct _temp127;
_temp127.tag= Cyc_Stdio_String_pa; _temp127.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
vd->name);{ void* _temp121=( void*)& _temp127; struct Cyc_Stdio_String_pa_struct
_temp126; _temp126.tag= Cyc_Stdio_String_pa; _temp126.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp122=( void*)& _temp126; struct Cyc_Stdio_String_pa_struct
_temp125; _temp125.tag= Cyc_Stdio_String_pa; _temp125.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( _temp117);{ void* _temp123=( void*)& _temp125; void*
_temp119[ 3u]={ _temp121, _temp122, _temp123}; struct _tagged_arr _temp120={(
void*) _temp119,( void*) _temp119,( void*)( _temp119 + 3u)}; _temp118( _temp124,
_temp120);}}}}));} if( ! Cyc_Tcutil_is_const_exp( te, _temp116)){ Cyc_Tcutil_terr(
loc, _tag_arr("initializer is not a constant expression", sizeof( unsigned char),
41u));}}} else{ for( 0; atts != 0; atts=(( struct Cyc_List_List*) _check_null(
atts))->tl){ void* _temp128=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd;
_LL130: if(( unsigned int) _temp128 > 16u?*(( int*) _temp128) == Cyc_Absyn_Regparm_att:
0){ goto _LL131;} else{ goto _LL132;} _LL132: if( _temp128 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL133;} else{ goto _LL134;} _LL134: if( _temp128 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL135;} else{ goto _LL136;} _LL136: if( _temp128 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL137;} else{ goto _LL138;} _LL138: if( _temp128 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL139;} else{ goto _LL140;} _LL140: if( _temp128 ==( void*) Cyc_Absyn_Const_att){
goto _LL141;} else{ goto _LL142;} _LL142: if(( unsigned int) _temp128 > 16u?*((
int*) _temp128) == Cyc_Absyn_Aligned_att: 0){ goto _LL143;} else{ goto _LL144;}
_LL144: if( _temp128 ==( void*) Cyc_Absyn_Packed_att){ goto _LL145;} else{ goto
_LL146;} _LL146: goto _LL147; _LL131: goto _LL133; _LL133: goto _LL135; _LL135:
goto _LL137; _LL137: goto _LL139; _LL139: goto _LL141; _LL141:(( int(*)( struct
_tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("tcVardecl: fn type attributes in function var decl",
sizeof( unsigned char), 51u)); goto _LL129; _LL143: goto _LL145; _LL145: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp148)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr _temp152=
_tag_arr("bad attribute %s in function declaration", sizeof( unsigned char), 41u);
struct Cyc_Stdio_String_pa_struct _temp153; _temp153.tag= Cyc_Stdio_String_pa;
_temp153.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*)(( struct
Cyc_List_List*) _check_null( atts))->hd);{ void* _temp151=( void*)& _temp153;
void* _temp149[ 1u]={ _temp151}; struct _tagged_arr _temp150={( void*) _temp149,(
void*) _temp149,( void*)( _temp149 + 1u)}; _temp148( _temp152, _temp150);}}));
goto _LL129; _LL147: continue; _LL129:;}}}{ struct _handler_cons _temp154;
_push_handler(& _temp154);{ int _temp156= 0; if( setjmp( _temp154.handler)){
_temp156= 1;} if( ! _temp156){{ struct _tuple3* _temp157=(( struct _tuple3*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->ordinaries,
v); void* _temp158=(* _temp157).f1; void* _temp168; _LL160: if(*(( int*)
_temp158) == Cyc_Tcenv_VarRes){ _LL169: _temp168=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp158)->f1; goto _LL161;} else{ goto _LL162;} _LL162: if(*(( int*) _temp158)
== Cyc_Tcenv_StructRes){ goto _LL163;} else{ goto _LL164;} _LL164: if(*(( int*)
_temp158) == Cyc_Tcenv_TunionRes){ goto _LL165;} else{ goto _LL166;} _LL166: if(*((
int*) _temp158) == Cyc_Tcenv_EnumRes){ goto _LL167;} else{ goto _LL159;} _LL161: {
struct Cyc_Absyn_Global_b_struct* _temp170=({ struct Cyc_Absyn_Global_b_struct*
_temp175=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp175[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp176; _temp176.tag= Cyc_Absyn_Global_b;
_temp176.f1= vd; _temp176;}); _temp175;}); void* _temp171= Cyc_Tcdecl_merge_binding(
_temp168,( void*) _temp170, loc, Cyc_Tc_tc_msg); if( _temp171 ==( void*) Cyc_Absyn_Unresolved_b){
goto _LL159;} if( _temp171 == _temp168?(* _temp157).f2: 0){ goto _LL159;} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple3*
_temp172=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp172->f1=(
void*)({ struct Cyc_Tcenv_VarRes_struct* _temp173=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp173[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp174; _temp174.tag= Cyc_Tcenv_VarRes; _temp174.f1=( void*) _temp171;
_temp174;}); _temp173;}); _temp172->f2= 1; _temp172;})); goto _LL159;} _LL163:
Cyc_Tcutil_warn( loc, _tag_arr("variable declaration shadows previous struct declaration",
sizeof( unsigned char), 57u)); goto _LL159; _LL165: Cyc_Tcutil_warn( loc,
_tag_arr("variable declaration shadows previous [x]tunion constructor", sizeof(
unsigned char), 60u)); goto _LL159; _LL167: Cyc_Tcutil_warn( loc, _tag_arr("variable declaration shadows previous enum tag",
sizeof( unsigned char), 47u)); goto _LL159; _LL159:;}; _pop_handler();} else{
void* _temp155=( void*) _exn_thrown; void* _temp178= _temp155; _LL180: if(
_temp178 == Cyc_Dict_Absent){ goto _LL181;} else{ goto _LL182;} _LL182: goto
_LL183; _LL181: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,
v,({ struct _tuple3* _temp184=( struct _tuple3*) GC_malloc( sizeof( struct
_tuple3)); _temp184->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp185=(
struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp185[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp186; _temp186.tag= Cyc_Tcenv_VarRes;
_temp186.f1=( void*)(( void*)({ struct Cyc_Absyn_Global_b_struct* _temp187=(
struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp187[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp188; _temp188.tag= Cyc_Absyn_Global_b;
_temp188.f1= vd; _temp188;}); _temp187;})); _temp186;}); _temp185;}); _temp184->f2=
0; _temp184;})); goto _LL179; _LL183:( void) _throw( _temp178); _LL179:;}}}}
static void Cyc_Tc_tcFndecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl* fd){ struct
_tagged_arr* v=(* fd->name).f2; void* sc=( void*) fd->sc;{ void* _temp189=(* fd->name).f1;
struct Cyc_List_List* _temp197; struct Cyc_List_List* _temp199; _LL191: if((
unsigned int) _temp189 > 1u?*(( int*) _temp189) == Cyc_Absyn_Rel_n: 0){ _LL198:
_temp197=(( struct Cyc_Absyn_Rel_n_struct*) _temp189)->f1; if( _temp197 == 0){
goto _LL192;} else{ goto _LL193;}} else{ goto _LL193;} _LL193: if(( unsigned int)
_temp189 > 1u?*(( int*) _temp189) == Cyc_Absyn_Abs_n: 0){ _LL200: _temp199=((
struct Cyc_Absyn_Abs_n_struct*) _temp189)->f1; goto _LL194;} else{ goto _LL195;}
_LL195: goto _LL196; _LL192: goto _LL190; _LL194:( int) _throw(( void*)({ struct
Cyc_Core_Impossible_struct* _temp201=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp201[ 0]=({ struct
Cyc_Core_Impossible_struct _temp202; _temp202.tag= Cyc_Core_Impossible; _temp202.f1=
_tag_arr("tc: Abs_n in tcFndecl", sizeof( unsigned char), 22u); _temp202;});
_temp201;})); _LL196: Cyc_Tcutil_terr( loc, _tag_arr("qualified declarations are not implemented",
sizeof( unsigned char), 43u)); return; _LL190:;}(* fd->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp203=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp203[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp204; _temp204.tag= Cyc_Absyn_Abs_n; _temp204.f1= te->ns; _temp204;});
_temp203;}); Cyc_Tcutil_check_fndecl_valid_type( loc, te, fd);{ void* t= Cyc_Tcutil_fndecl2typ(
fd);{ struct Cyc_List_List* atts= fd->attributes; for( 0; atts != 0; atts=((
struct Cyc_List_List*) _check_null( atts))->tl){ void* _temp205=( void*)((
struct Cyc_List_List*) _check_null( atts))->hd; _LL207: if( _temp205 ==( void*)
Cyc_Absyn_Packed_att){ goto _LL208;} else{ goto _LL209;} _LL209: if((
unsigned int) _temp205 > 16u?*(( int*) _temp205) == Cyc_Absyn_Aligned_att: 0){
goto _LL210;} else{ goto _LL211;} _LL211: goto _LL212; _LL208: goto _LL210;
_LL210: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp213)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf;
struct _tagged_arr _temp217= _tag_arr("bad attribute %s for function", sizeof(
unsigned char), 30u); struct Cyc_Stdio_String_pa_struct _temp218; _temp218.tag=
Cyc_Stdio_String_pa; _temp218.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd);{ void* _temp216=( void*)&
_temp218; void* _temp214[ 1u]={ _temp216}; struct _tagged_arr _temp215={( void*)
_temp214,( void*) _temp214,( void*)( _temp214 + 1u)}; _temp213( _temp217,
_temp215);}})); goto _LL206; _LL212: goto _LL206; _LL206:;}}{ struct
_handler_cons _temp219; _push_handler(& _temp219);{ int _temp221= 0; if( setjmp(
_temp219.handler)){ _temp221= 1;} if( ! _temp221){{ struct _tuple3* _temp222=((
struct _tuple3*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
ge->ordinaries, v); void* _temp223=(* _temp222).f1; void* _temp233; _LL225: if(*((
int*) _temp223) == Cyc_Tcenv_VarRes){ _LL234: _temp233=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp223)->f1; goto _LL226;} else{ goto _LL227;} _LL227: if(*(( int*) _temp223)
== Cyc_Tcenv_StructRes){ goto _LL228;} else{ goto _LL229;} _LL229: if(*(( int*)
_temp223) == Cyc_Tcenv_TunionRes){ goto _LL230;} else{ goto _LL231;} _LL231: if(*((
int*) _temp223) == Cyc_Tcenv_EnumRes){ goto _LL232;} else{ goto _LL224;} _LL226: {
struct Cyc_Absyn_Funname_b_struct* _temp235=({ struct Cyc_Absyn_Funname_b_struct*
_temp240=( struct Cyc_Absyn_Funname_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct));
_temp240[ 0]=({ struct Cyc_Absyn_Funname_b_struct _temp241; _temp241.tag= Cyc_Absyn_Funname_b;
_temp241.f1= fd; _temp241;}); _temp240;}); void* _temp236= Cyc_Tcdecl_merge_binding(
_temp233,( void*) _temp235, loc, Cyc_Tc_tc_msg); if( _temp236 ==( void*) Cyc_Absyn_Unresolved_b){
goto _LL224;} if( _temp236 == _temp233?(* _temp222).f2: 0){ goto _LL224;} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple3*
_temp237=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp237->f1=(
void*)({ struct Cyc_Tcenv_VarRes_struct* _temp238=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp238[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp239; _temp239.tag= Cyc_Tcenv_VarRes; _temp239.f1=( void*) _temp236;
_temp239;}); _temp238;}); _temp237->f2= 1; _temp237;})); goto _LL224;} _LL228:
Cyc_Tcutil_warn( loc, _tag_arr("function declaration shadows previous struct declaration",
sizeof( unsigned char), 57u)); goto _LL224; _LL230: Cyc_Tcutil_warn( loc,
_tag_arr("function declaration shadows previous [x]tunion constructor", sizeof(
unsigned char), 60u)); goto _LL224; _LL232: Cyc_Tcutil_warn( loc, _tag_arr("function declaration shadows previous enum tag",
sizeof( unsigned char), 47u)); goto _LL224; _LL224:;}; _pop_handler();} else{
void* _temp220=( void*) _exn_thrown; void* _temp243= _temp220; _LL245: if(
_temp243 == Cyc_Dict_Absent){ goto _LL246;} else{ goto _LL247;} _LL247: goto
_LL248; _LL246: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,
v,({ struct _tuple3* _temp249=( struct _tuple3*) GC_malloc( sizeof( struct
_tuple3)); _temp249->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp250=(
struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp250[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp251; _temp251.tag= Cyc_Tcenv_VarRes;
_temp251.f1=( void*)(( void*)({ struct Cyc_Absyn_Funname_b_struct* _temp252=(
struct Cyc_Absyn_Funname_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct));
_temp252[ 0]=({ struct Cyc_Absyn_Funname_b_struct _temp253; _temp253.tag= Cyc_Absyn_Funname_b;
_temp253.f1= fd; _temp253;}); _temp252;})); _temp251;}); _temp250;}); _temp249->f2=
0; _temp249;})); goto _LL244; _LL248:( void) _throw( _temp243); _LL244:;}}} te->le=({
struct Cyc_Core_Opt* _temp254=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp254->v=( void*)(( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp255=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp255[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp256; _temp256.tag= Cyc_Tcenv_Outermost;
_temp256.f1=( void*) Cyc_Tcenv_new_fenv( loc, fd); _temp256;}); _temp255;}));
_temp254;}); Cyc_Tcstmt_tcStmt( te, fd->body, 0); if( ! Cyc_Tcenv_all_labels_resolved(
te)){ Cyc_Tcutil_terr( loc, _tag_arr("function has goto statements to undefined labels",
sizeof( unsigned char), 49u));} te->le= 0;}} static void Cyc_Tc_tcTypedefdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Typedefdecl* td){ struct _tagged_arr* v=(* td->name).f2;{
void* _temp257=(* td->name).f1; struct Cyc_List_List* _temp265; struct Cyc_List_List*
_temp267; _LL259: if(( unsigned int) _temp257 > 1u?*(( int*) _temp257) == Cyc_Absyn_Rel_n:
0){ _LL266: _temp265=(( struct Cyc_Absyn_Rel_n_struct*) _temp257)->f1; if(
_temp265 == 0){ goto _LL260;} else{ goto _LL261;}} else{ goto _LL261;} _LL261:
if(( unsigned int) _temp257 > 1u?*(( int*) _temp257) == Cyc_Absyn_Abs_n: 0){
_LL268: _temp267=(( struct Cyc_Absyn_Abs_n_struct*) _temp257)->f1; if( _temp267
== 0){ goto _LL262;} else{ goto _LL263;}} else{ goto _LL263;} _LL263: goto
_LL264; _LL260: goto _LL258; _LL262: goto _LL258; _LL264: Cyc_Tcutil_terr( loc,
_tag_arr("qualified declarations are not implemented", sizeof( unsigned char),
43u)); return; _LL258:;} if((( int(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_member)( ge->typedefs, v)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp269)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr _temp273= _tag_arr("redeclaration of typedef %s",
sizeof( unsigned char), 28u); struct Cyc_Stdio_String_pa_struct _temp274;
_temp274.tag= Cyc_Stdio_String_pa; _temp274.f1=( struct _tagged_arr)* v;{ void*
_temp272=( void*)& _temp274; void* _temp270[ 1u]={ _temp272}; struct _tagged_arr
_temp271={( void*) _temp270,( void*) _temp270,( void*)( _temp270 + 1u)};
_temp269( _temp273, _temp271);}})); return;}(* td->name).f1=( void*)({ struct
Cyc_Absyn_Abs_n_struct* _temp275=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp275[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp276; _temp276.tag= Cyc_Absyn_Abs_n; _temp276.f1= te->ns; _temp276;});
_temp275;}); Cyc_Tcutil_check_unique_tvars( loc, td->tvs); Cyc_Tcutil_add_tvar_identities(
td->tvs); Cyc_Tcutil_check_type( loc, te, td->tvs,( void*) Cyc_Absyn_AnyKind,(
void*) td->defn); ge->typedefs=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Typedefdecl* data)) Cyc_Dict_insert)(
ge->typedefs, v, td);} static void Cyc_Tc_tcStructFields( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct
_tagged_arr obj, struct Cyc_List_List* fields, struct Cyc_List_List* tvs){
struct _RegionHandle _temp277= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp277; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0;
struct Cyc_List_List* _temp278= fields; for( 0; _temp278 != 0; _temp278=((
struct Cyc_List_List*) _check_null( _temp278))->tl){ struct Cyc_Absyn_Structfield
_temp281; struct Cyc_List_List* _temp282; struct Cyc_Core_Opt* _temp284; void*
_temp286; struct Cyc_Absyn_Tqual _temp288; struct _tagged_arr* _temp290; struct
Cyc_Absyn_Structfield* _temp279=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp278))->hd; _temp281=* _temp279; _LL291: _temp290= _temp281.name;
goto _LL289; _LL289: _temp288= _temp281.tq; goto _LL287; _LL287: _temp286=( void*)
_temp281.type; goto _LL285; _LL285: _temp284= _temp281.width; goto _LL283;
_LL283: _temp282= _temp281.attributes; goto _LL280; _LL280: if((( int(*)( int(*
compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l,
struct _tagged_arr* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,
_temp290)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp292)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf;
struct _tagged_arr _temp297= _tag_arr("duplicate field %s in %s", sizeof(
unsigned char), 25u); struct Cyc_Stdio_String_pa_struct _temp299; _temp299.tag=
Cyc_Stdio_String_pa; _temp299.f1=( struct _tagged_arr)* _temp290;{ void*
_temp295=( void*)& _temp299; struct Cyc_Stdio_String_pa_struct _temp298;
_temp298.tag= Cyc_Stdio_String_pa; _temp298.f1=( struct _tagged_arr) obj;{ void*
_temp296=( void*)& _temp298; void* _temp293[ 2u]={ _temp295, _temp296}; struct
_tagged_arr _temp294={( void*) _temp293,( void*) _temp293,( void*)( _temp293 + 2u)};
_temp292( _temp297, _temp294);}}}));} if( Cyc_String_strcmp(* _temp290, _tag_arr("",
sizeof( unsigned char), 1u)) != 0){ prev_fields=({ struct Cyc_List_List*
_temp300=( struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp300->hd=( void*) _temp290; _temp300->tl= prev_fields; _temp300;});} Cyc_Tcutil_check_type(
loc, te, tvs,( void*) Cyc_Absyn_MemKind, _temp286); if( _temp284 != 0){
unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp284))->v)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp301)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr _temp305= _tag_arr("bitfield %s does not have constant width",
sizeof( unsigned char), 41u); struct Cyc_Stdio_String_pa_struct _temp306;
_temp306.tag= Cyc_Stdio_String_pa; _temp306.f1=( struct _tagged_arr)* _temp290;{
void* _temp304=( void*)& _temp306; void* _temp302[ 1u]={ _temp304}; struct
_tagged_arr _temp303={( void*) _temp302,( void*) _temp302,( void*)( _temp302 + 1u)};
_temp301( _temp305, _temp303);}}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp284))->v);}{
void* _temp307= Cyc_Tcutil_compress( _temp286); void* _temp313; _LL309: if((
unsigned int) _temp307 > 4u?*(( int*) _temp307) == Cyc_Absyn_IntType: 0){ _LL314:
_temp313=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp307)->f2; goto _LL310;}
else{ goto _LL311;} _LL311: goto _LL312; _LL310:{ void* _temp315= _temp313;
_LL317: if( _temp315 ==( void*) Cyc_Absyn_B1){ goto _LL318;} else{ goto _LL319;}
_LL319: if( _temp315 ==( void*) Cyc_Absyn_B2){ goto _LL320;} else{ goto _LL321;}
_LL321: if( _temp315 ==( void*) Cyc_Absyn_B4){ goto _LL322;} else{ goto _LL323;}
_LL323: if( _temp315 ==( void*) Cyc_Absyn_B8){ goto _LL324;} else{ goto _LL316;}
_LL318: if( w > 8){ Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u));} goto _LL316; _LL320: if( w > 16){ Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u));} goto
_LL316; _LL322: if( w > 32){ Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u));} goto _LL316; _LL324: if( w > 64){ Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u));} goto
_LL316; _LL316:;} goto _LL308; _LL312: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr(* _temp325)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf;
struct _tagged_arr _temp330= _tag_arr("bitfield %s must have integral type but has type %s",
sizeof( unsigned char), 52u); struct Cyc_Stdio_String_pa_struct _temp332;
_temp332.tag= Cyc_Stdio_String_pa; _temp332.f1=( struct _tagged_arr)* _temp290;{
void* _temp328=( void*)& _temp332; struct Cyc_Stdio_String_pa_struct _temp331;
_temp331.tag= Cyc_Stdio_String_pa; _temp331.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp286);{ void* _temp329=( void*)& _temp331; void* _temp326[ 2u]={ _temp328,
_temp329}; struct _tagged_arr _temp327={( void*) _temp326,( void*) _temp326,(
void*)( _temp326 + 2u)}; _temp325( _temp330, _temp327);}}})); goto _LL308;
_LL308:;}}}}; _pop_region( uprev_rgn);} static void Cyc_Tc_tcStructOrUniondecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct _tagged_arr obj,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl* sd){ struct
_tagged_arr* _temp333=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
sd->name))->v)).f2; if( sd->name == 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp334)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr _temp338= _tag_arr("anonymous %ss are not allowed at top level",
sizeof( unsigned char), 43u); struct Cyc_Stdio_String_pa_struct _temp339;
_temp339.tag= Cyc_Stdio_String_pa; _temp339.f1=( struct _tagged_arr) obj;{ void*
_temp337=( void*)& _temp339; void* _temp335[ 1u]={ _temp337}; struct _tagged_arr
_temp336={( void*) _temp335,( void*) _temp335,( void*)( _temp335 + 1u)};
_temp334( _temp338, _temp336);}})); return;}{ struct Cyc_List_List* atts= sd->attributes;
for( 0; atts != 0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ void*
_temp340=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd; _LL342: if(
_temp340 ==( void*) Cyc_Absyn_Packed_att){ goto _LL343;} else{ goto _LL344;}
_LL344: if(( unsigned int) _temp340 > 16u?*(( int*) _temp340) == Cyc_Absyn_Aligned_att:
0){ goto _LL345;} else{ goto _LL346;} _LL346: goto _LL347; _LL343: continue;
_LL345: continue; _LL347: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp348)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf;
struct _tagged_arr _temp354= _tag_arr("bad attribute %s in %s %s definition",
sizeof( unsigned char), 37u); struct Cyc_Stdio_String_pa_struct _temp357;
_temp357.tag= Cyc_Stdio_String_pa; _temp357.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd);{ void* _temp351=( void*)&
_temp357; struct Cyc_Stdio_String_pa_struct _temp356; _temp356.tag= Cyc_Stdio_String_pa;
_temp356.f1=( struct _tagged_arr) obj;{ void* _temp352=( void*)& _temp356;
struct Cyc_Stdio_String_pa_struct _temp355; _temp355.tag= Cyc_Stdio_String_pa;
_temp355.f1=( struct _tagged_arr)* _temp333;{ void* _temp353=( void*)& _temp355;
void* _temp349[ 3u]={ _temp351, _temp352, _temp353}; struct _tagged_arr _temp350={(
void*) _temp349,( void*) _temp349,( void*)( _temp349 + 3u)}; _temp348( _temp354,
_temp350);}}}})); goto _LL341; _LL341:;}}{ struct Cyc_List_List* tvs= sd->tvs;
for( 0; tvs != 0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ struct
Cyc_Absyn_Conref* _temp358= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); void* _temp359=( void*)
_temp358->v; void* _temp367; _LL361: if( _temp359 ==( void*) Cyc_Absyn_No_constr){
goto _LL362;} else{ goto _LL363;} _LL363: if(( unsigned int) _temp359 > 1u?*((
int*) _temp359) == Cyc_Absyn_Eq_constr: 0){ _LL368: _temp367=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp359)->f1; if( _temp367 ==( void*) Cyc_Absyn_MemKind){
goto _LL364;} else{ goto _LL365;}} else{ goto _LL365;} _LL365: goto _LL366;
_LL362:( void*)( _temp358->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp369=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp369[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp370; _temp370.tag= Cyc_Absyn_Eq_constr;
_temp370.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp370;}); _temp369;})));
continue; _LL364: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp371)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf;
struct _tagged_arr _temp377= _tag_arr("%s %s attempts to abstract type variable %s of kind M",
sizeof( unsigned char), 54u); struct Cyc_Stdio_String_pa_struct _temp380;
_temp380.tag= Cyc_Stdio_String_pa; _temp380.f1=( struct _tagged_arr) obj;{ void*
_temp374=( void*)& _temp380; struct Cyc_Stdio_String_pa_struct _temp379;
_temp379.tag= Cyc_Stdio_String_pa; _temp379.f1=( struct _tagged_arr)* _temp333;{
void* _temp375=( void*)& _temp379; struct Cyc_Stdio_String_pa_struct _temp378;
_temp378.tag= Cyc_Stdio_String_pa; _temp378.f1=( struct _tagged_arr)*(( struct
Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->name;{ void*
_temp376=( void*)& _temp378; void* _temp372[ 3u]={ _temp374, _temp375, _temp376};
struct _tagged_arr _temp373={( void*) _temp372,( void*) _temp372,( void*)(
_temp372 + 3u)}; _temp371( _temp377, _temp373);}}}})); continue; _LL366:
continue; _LL360:;}}{ void* _temp381=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v)).f1; struct Cyc_List_List* _temp389; struct Cyc_List_List*
_temp391; _LL383: if(( unsigned int) _temp381 > 1u?*(( int*) _temp381) == Cyc_Absyn_Rel_n:
0){ _LL390: _temp389=(( struct Cyc_Absyn_Rel_n_struct*) _temp381)->f1; if(
_temp389 == 0){ goto _LL384;} else{ goto _LL385;}} else{ goto _LL385;} _LL385:
if(( unsigned int) _temp381 > 1u?*(( int*) _temp381) == Cyc_Absyn_Abs_n: 0){
_LL392: _temp391=(( struct Cyc_Absyn_Abs_n_struct*) _temp381)->f1; if( _temp391
== 0){ goto _LL386;} else{ goto _LL387;}} else{ goto _LL387;} _LL387: goto
_LL388; _LL384: goto _LL382; _LL386: goto _LL382; _LL388: Cyc_Tcutil_terr( loc,
_tag_arr("qualified declarations are not implemented", sizeof( unsigned char),
43u)); return; _LL382:;}(*(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v)).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp393=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp393[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp394; _temp394.tag= Cyc_Absyn_Abs_n;
_temp394.f1= te->ns; _temp394;}); _temp393;}); Cyc_Tcutil_check_unique_tvars(
loc, sd->tvs); Cyc_Tcutil_add_tvar_identities( sd->tvs);} struct _tuple4{ struct
Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ; static void Cyc_Tc_tcStructdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Structdecl* sd){ struct _tagged_arr* _temp395=(*(( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)).f2; const
unsigned char* _temp396="struct"; Cyc_Tc_tcStructOrUniondecl( te, ge, _tag_arr(
_temp396, sizeof( unsigned char), 7u), loc, sd);{ struct Cyc_List_List* _temp397=
sd->tvs;{ struct _tuple4 _temp399=({ struct _tuple4 _temp398; _temp398.f1= sd->fields;
_temp398.f2=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup_opt)( ge->structdecls, _temp395); _temp398;});
struct Cyc_Core_Opt* _temp409; struct Cyc_Core_Opt* _temp411; struct Cyc_Core_Opt*
_temp413; struct Cyc_Core_Opt* _temp415; struct Cyc_Core_Opt _temp417; struct
Cyc_List_List* _temp418; struct Cyc_Core_Opt* _temp420; struct Cyc_Core_Opt
_temp422; struct Cyc_Absyn_Structdecl** _temp423; struct Cyc_Core_Opt* _temp425;
struct Cyc_Core_Opt _temp427; struct Cyc_List_List* _temp428; struct Cyc_Core_Opt*
_temp430; struct Cyc_Core_Opt _temp432; struct Cyc_Absyn_Structdecl** _temp433;
struct Cyc_Core_Opt* _temp435; _LL401: _LL412: _temp411= _temp399.f1; if(
_temp411 == 0){ goto _LL410;} else{ goto _LL403;} _LL410: _temp409= _temp399.f2;
if( _temp409 == 0){ goto _LL402;} else{ goto _LL403;} _LL403: _LL416: _temp415=
_temp399.f1; if( _temp415 == 0){ goto _LL405;} else{ _temp417=* _temp415; _LL419:
_temp418=( struct Cyc_List_List*) _temp417.v; goto _LL414;} _LL414: _temp413=
_temp399.f2; if( _temp413 == 0){ goto _LL404;} else{ goto _LL405;} _LL405:
_LL426: _temp425= _temp399.f1; if( _temp425 == 0){ goto _LL407;} else{ _temp427=*
_temp425; _LL429: _temp428=( struct Cyc_List_List*) _temp427.v; goto _LL421;}
_LL421: _temp420= _temp399.f2; if( _temp420 == 0){ goto _LL407;} else{ _temp422=*
_temp420; _LL424: _temp423=( struct Cyc_Absyn_Structdecl**) _temp422.v; goto
_LL406;} _LL407: _LL436: _temp435= _temp399.f1; if( _temp435 == 0){ goto _LL431;}
else{ goto _LL400;} _LL431: _temp430= _temp399.f2; if( _temp430 == 0){ goto
_LL400;} else{ _temp432=* _temp430; _LL434: _temp433=( struct Cyc_Absyn_Structdecl**)
_temp432.v; goto _LL408;} _LL402: ge->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl**
data)) Cyc_Dict_insert)( ge->structdecls, _temp395,({ struct Cyc_Absyn_Structdecl**
_temp437=( struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp437[ 0]= sd; _temp437;})); goto _LL400; _LL404: { struct Cyc_Absyn_Structdecl**
_temp438=({ struct Cyc_Absyn_Structdecl** _temp439=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp439[ 0]=({ struct Cyc_Absyn_Structdecl*
_temp440=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp440->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp440->name= sd->name;
_temp440->tvs= _temp397; _temp440->fields= 0; _temp440->attributes= 0; _temp440;});
_temp439;}); ge->structdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl** data)) Cyc_Dict_insert)(
ge->structdecls, _temp395, _temp438); Cyc_Tc_tcStructFields( te, ge, loc,
_tag_arr( _temp396, sizeof( unsigned char), 7u), _temp418, _temp397);* _temp438=
sd; goto _LL400;} _LL406: { struct Cyc_Absyn_Structdecl* _temp441=* _temp423;*
_temp423=({ struct Cyc_Absyn_Structdecl* _temp442=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp442->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp442->name= sd->name; _temp442->tvs= _temp397; _temp442->fields=
0; _temp442->attributes= 0; _temp442;}); Cyc_Tc_tcStructFields( te, ge, loc,
_tag_arr( _temp396, sizeof( unsigned char), 7u), _temp428, _temp397);* _temp423=
_temp441; _temp433= _temp423; goto _LL408;} _LL408: { struct Cyc_Absyn_Structdecl*
_temp443= Cyc_Tcdecl_merge_structdecl(* _temp433, sd, loc, Cyc_Tc_tc_msg); if(
_temp443 == 0){ return;} else{* _temp433=( struct Cyc_Absyn_Structdecl*)
_check_null( _temp443); sd=( struct Cyc_Absyn_Structdecl*) _check_null( _temp443);
goto _LL400;}} _LL400:;} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,
_temp395,({ struct _tuple3* _temp444=( struct _tuple3*) GC_malloc( sizeof(
struct _tuple3)); _temp444->f1=( void*)({ struct Cyc_Tcenv_StructRes_struct*
_temp445=( struct Cyc_Tcenv_StructRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_StructRes_struct));
_temp445[ 0]=({ struct Cyc_Tcenv_StructRes_struct _temp446; _temp446.tag= Cyc_Tcenv_StructRes;
_temp446.f1= sd; _temp446;}); _temp445;}); _temp444->f2= 1; _temp444;}));}}
static void Cyc_Tc_tcUniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl* ud){ struct
_tagged_arr* _temp447=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
ud->name))->v)).f2; const unsigned char* _temp448="union"; Cyc_Tc_tcStructOrUniondecl(
te, ge, _tag_arr( _temp448, sizeof( unsigned char), 6u), loc,( struct Cyc_Absyn_Structdecl*)
ud);{ struct Cyc_List_List* _temp449= ud->tvs; struct _tuple4 _temp451=({ struct
_tuple4 _temp450; _temp450.f1= ud->fields; _temp450.f2=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)( ge->uniondecls,
_temp447); _temp450;}); struct Cyc_Core_Opt* _temp461; struct Cyc_Core_Opt*
_temp463; struct Cyc_Core_Opt* _temp465; struct Cyc_Core_Opt* _temp467; struct
Cyc_Core_Opt _temp469; struct Cyc_List_List* _temp470; struct Cyc_Core_Opt*
_temp472; struct Cyc_Core_Opt _temp474; struct Cyc_Absyn_Uniondecl** _temp475;
struct Cyc_Core_Opt* _temp477; struct Cyc_Core_Opt _temp479; struct Cyc_List_List*
_temp480; struct Cyc_Core_Opt* _temp482; struct Cyc_Core_Opt _temp484; struct
Cyc_Absyn_Uniondecl** _temp485; struct Cyc_Core_Opt* _temp487; _LL453: _LL464:
_temp463= _temp451.f1; if( _temp463 == 0){ goto _LL462;} else{ goto _LL455;}
_LL462: _temp461= _temp451.f2; if( _temp461 == 0){ goto _LL454;} else{ goto
_LL455;} _LL455: _LL468: _temp467= _temp451.f1; if( _temp467 == 0){ goto _LL457;}
else{ _temp469=* _temp467; _LL471: _temp470=( struct Cyc_List_List*) _temp469.v;
goto _LL466;} _LL466: _temp465= _temp451.f2; if( _temp465 == 0){ goto _LL456;}
else{ goto _LL457;} _LL457: _LL478: _temp477= _temp451.f1; if( _temp477 == 0){
goto _LL459;} else{ _temp479=* _temp477; _LL481: _temp480=( struct Cyc_List_List*)
_temp479.v; goto _LL473;} _LL473: _temp472= _temp451.f2; if( _temp472 == 0){
goto _LL459;} else{ _temp474=* _temp472; _LL476: _temp475=( struct Cyc_Absyn_Uniondecl**)
_temp474.v; goto _LL458;} _LL459: _LL488: _temp487= _temp451.f1; if( _temp487 ==
0){ goto _LL483;} else{ goto _LL452;} _LL483: _temp482= _temp451.f2; if(
_temp482 == 0){ goto _LL452;} else{ _temp484=* _temp482; _LL486: _temp485=(
struct Cyc_Absyn_Uniondecl**) _temp484.v; goto _LL460;} _LL454: ge->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)( ge->uniondecls, _temp447,({
struct Cyc_Absyn_Uniondecl** _temp489=( struct Cyc_Absyn_Uniondecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl*)); _temp489[ 0]= ud; _temp489;})); goto
_LL452; _LL456: { struct Cyc_Absyn_Uniondecl** _temp490=({ struct Cyc_Absyn_Uniondecl**
_temp501=( struct Cyc_Absyn_Uniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl*));
_temp501[ 0]=({ struct Cyc_Absyn_Uniondecl* _temp502=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp502->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp502->name= ud->name; _temp502->tvs= _temp449; _temp502->fields=
0; _temp502->attributes= ud->attributes; _temp502;}); _temp501;}); ge->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)( ge->uniondecls, _temp447,
_temp490); Cyc_Tc_tcStructFields( te, ge, loc, _tag_arr( _temp448, sizeof(
unsigned char), 6u), _temp470, _temp449);{ struct Cyc_List_List* f= _temp470;
for( 0; f != 0; f=(( struct Cyc_List_List*) _check_null( f))->tl){ if( ! Cyc_Tcutil_bits_only((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( f))->hd)->type)){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp491)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf; struct
_tagged_arr _temp497= _tag_arr("field %s of union %s has type %s which is not `bits-only'",
sizeof( unsigned char), 58u); struct Cyc_Stdio_String_pa_struct _temp500;
_temp500.tag= Cyc_Stdio_String_pa; _temp500.f1=( struct _tagged_arr)*(( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( f))->hd)->name;{
void* _temp494=( void*)& _temp500; struct Cyc_Stdio_String_pa_struct _temp499;
_temp499.tag= Cyc_Stdio_String_pa; _temp499.f1=( struct _tagged_arr)* _temp447;{
void* _temp495=( void*)& _temp499; struct Cyc_Stdio_String_pa_struct _temp498;
_temp498.tag= Cyc_Stdio_String_pa; _temp498.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( f))->hd)->type);{
void* _temp496=( void*)& _temp498; void* _temp492[ 3u]={ _temp494, _temp495,
_temp496}; struct _tagged_arr _temp493={( void*) _temp492,( void*) _temp492,(
void*)( _temp492 + 3u)}; _temp491( _temp497, _temp493);}}}}));}}}* _temp490= ud;
goto _LL452;} _LL458: { struct Cyc_Absyn_Uniondecl* _temp503=* _temp475;*
_temp475=({ struct Cyc_Absyn_Uniondecl* _temp504=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp504->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp504->name= ud->name; _temp504->tvs= _temp449; _temp504->fields=
0; _temp504->attributes= ud->attributes; _temp504;}); Cyc_Tc_tcStructFields( te,
ge, loc, _tag_arr( _temp448, sizeof( unsigned char), 6u), _temp480, _temp449);*
_temp475= _temp503; _temp485= _temp475; goto _LL460;} _LL460: { struct Cyc_Absyn_Uniondecl*
_temp505= Cyc_Tcdecl_merge_uniondecl(* _temp485, ud, loc, Cyc_Tc_tc_msg); if(
_temp505 == 0){ return;} else{* _temp485=( struct Cyc_Absyn_Uniondecl*)
_check_null( _temp505); goto _LL452;}} _LL452:;}} struct _tuple5{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; static struct Cyc_List_List* Cyc_Tc_tcTunionFields( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct
_tagged_arr obj, int is_xtunion, struct _tuple0* name, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs, struct Cyc_Absyn_Tuniondecl* tudres){{ struct
Cyc_List_List* _temp506= fields; for( 0; _temp506 != 0; _temp506=(( struct Cyc_List_List*)
_check_null( _temp506))->tl){ struct Cyc_Absyn_Tunionfield* _temp507=( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp506))->hd;{
struct Cyc_List_List* tvs= _temp507->tvs; for( 0; tvs != 0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){ struct Cyc_Absyn_Tvar* _temp508=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd; struct Cyc_Absyn_Conref* _temp509=
Cyc_Absyn_compress_conref( _temp508->kind); void* _temp510=( void*) _temp509->v;
void* _temp518; _LL512: if( _temp510 ==( void*) Cyc_Absyn_No_constr){ goto
_LL513;} else{ goto _LL514;} _LL514: if(( unsigned int) _temp510 > 1u?*(( int*)
_temp510) == Cyc_Absyn_Eq_constr: 0){ _LL519: _temp518=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp510)->f1; if( _temp518 ==( void*) Cyc_Absyn_MemKind){ goto _LL515;} else{
goto _LL516;}} else{ goto _LL516;} _LL516: goto _LL517; _LL513:( void*)(
_temp509->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp520=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp520[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp521; _temp521.tag= Cyc_Absyn_Eq_constr;
_temp521.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp521;}); _temp520;})));
goto _LL511; _LL515: Cyc_Tcutil_terr( _temp507->loc,( struct _tagged_arr)({
struct _tagged_arr(* _temp522)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf;
struct _tagged_arr _temp526= _tag_arr("field %s abstracts type variable of kind M",
sizeof( unsigned char), 43u); struct Cyc_Stdio_String_pa_struct _temp527;
_temp527.tag= Cyc_Stdio_String_pa; _temp527.f1=( struct _tagged_arr)*(* _temp507->name).f2;{
void* _temp525=( void*)& _temp527; void* _temp523[ 1u]={ _temp525}; struct
_tagged_arr _temp524={( void*) _temp523,( void*) _temp523,( void*)( _temp523 + 1u)};
_temp522( _temp526, _temp524);}})); goto _LL511; _LL517: goto _LL511; _LL511:;}}{
struct Cyc_List_List* alltvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( tvs, _temp507->tvs); Cyc_Tcutil_check_unique_tvars(
loc, alltvs); Cyc_Tcutil_add_tvar_identities( _temp507->tvs);{ struct Cyc_List_List*
typs= _temp507->typs; for( 0; typs != 0; typs=(( struct Cyc_List_List*)
_check_null( typs))->tl){ Cyc_Tcutil_check_type( _temp507->loc, te, alltvs,(
void*) Cyc_Absyn_MemKind,(*(( struct _tuple5*)(( struct Cyc_List_List*)
_check_null( typs))->hd)).f2);}}{ void* _temp528=(* _temp507->name).f1; struct
Cyc_List_List* _temp538; _LL530: if(( unsigned int) _temp528 > 1u?*(( int*)
_temp528) == Cyc_Absyn_Rel_n: 0){ _LL539: _temp538=(( struct Cyc_Absyn_Rel_n_struct*)
_temp528)->f1; if( _temp538 == 0){ goto _LL531;} else{ goto _LL532;}} else{ goto
_LL532;} _LL532: if(( unsigned int) _temp528 > 1u?*(( int*) _temp528) == Cyc_Absyn_Rel_n:
0){ goto _LL533;} else{ goto _LL534;} _LL534: if(( unsigned int) _temp528 > 1u?*((
int*) _temp528) == Cyc_Absyn_Abs_n: 0){ goto _LL535;} else{ goto _LL536;} _LL536:
if( _temp528 ==( void*) Cyc_Absyn_Loc_n){ goto _LL537;} else{ goto _LL529;}
_LL531: if( is_xtunion){(* _temp507->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp540=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp540[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp541; _temp541.tag= Cyc_Absyn_Abs_n;
_temp541.f1= te->ns; _temp541;}); _temp540;});} else{(* _temp507->name).f1=(*
name).f1;} goto _LL529; _LL533: Cyc_Tcutil_terr( _temp507->loc, _tag_arr("qualified declarations are not allowed",
sizeof( unsigned char), 39u)); goto _LL529; _LL535: goto _LL529; _LL537:( int)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp542=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp542[ 0]=({ struct
Cyc_Core_Impossible_struct _temp543; _temp543.tag= Cyc_Core_Impossible; _temp543.f1=
_tag_arr("tcTunionFields: Loc_n", sizeof( unsigned char), 22u); _temp543;});
_temp542;})); goto _LL529; _LL529:;}}}}{ struct Cyc_List_List* fields2; if(
is_xtunion){ int _temp544= 1; struct Cyc_List_List* _temp545= Cyc_Tcdecl_sort_xtunion_fields(
fields,& _temp544,(* name).f2, loc, Cyc_Tc_tc_msg); if( _temp544){ fields2=
_temp545;} else{ fields2= 0;}} else{ struct _RegionHandle _temp546= _new_region();
struct _RegionHandle* uprev_rgn=& _temp546; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0;{ struct Cyc_List_List* fs= fields; for( 0; fs !=
0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){ struct Cyc_Absyn_Tunionfield*
_temp547=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fs))->hd; if((( int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*),
struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp,
prev_fields,(* _temp547->name).f2)){ Cyc_Tcutil_terr( _temp547->loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp548)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr _temp553= _tag_arr("duplicate field name %s in %s",
sizeof( unsigned char), 30u); struct Cyc_Stdio_String_pa_struct _temp555;
_temp555.tag= Cyc_Stdio_String_pa; _temp555.f1=( struct _tagged_arr)*(* _temp547->name).f2;{
void* _temp551=( void*)& _temp555; struct Cyc_Stdio_String_pa_struct _temp554;
_temp554.tag= Cyc_Stdio_String_pa; _temp554.f1=( struct _tagged_arr) obj;{ void*
_temp552=( void*)& _temp554; void* _temp549[ 2u]={ _temp551, _temp552}; struct
_tagged_arr _temp550={( void*) _temp549,( void*) _temp549,( void*)( _temp549 + 2u)};
_temp548( _temp553, _temp550);}}}));} else{ prev_fields=({ struct Cyc_List_List*
_temp556=( struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp556->hd=( void*)(* _temp547->name).f2; _temp556->tl= prev_fields; _temp556;});}
if(( void*) _temp547->sc !=( void*) Cyc_Absyn_Public){ Cyc_Tcutil_warn( loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp557)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr _temp561= _tag_arr("ignoring scope of field %s",
sizeof( unsigned char), 27u); struct Cyc_Stdio_String_pa_struct _temp562;
_temp562.tag= Cyc_Stdio_String_pa; _temp562.f1=( struct _tagged_arr)*(* _temp547->name).f2;{
void* _temp560=( void*)& _temp562; void* _temp558[ 1u]={ _temp560}; struct
_tagged_arr _temp559={( void*) _temp558,( void*) _temp558,( void*)( _temp558 + 1u)};
_temp557( _temp561, _temp559);}}));( void*)( _temp547->sc=( void*)(( void*) Cyc_Absyn_Public));}}}
fields2= fields;}; _pop_region( uprev_rgn);}{ struct Cyc_List_List* _temp563=
fields; for( 0; _temp563 != 0; _temp563=(( struct Cyc_List_List*) _check_null(
_temp563))->tl){ struct Cyc_Absyn_Tunionfield* _temp564=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp563))->hd; ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries,(* _temp564->name).f2,({ struct _tuple3* _temp565=( struct
_tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp565->f1=( void*)({ struct
Cyc_Tcenv_TunionRes_struct* _temp566=( struct Cyc_Tcenv_TunionRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct)); _temp566[ 0]=({ struct
Cyc_Tcenv_TunionRes_struct _temp567; _temp567.tag= Cyc_Tcenv_TunionRes; _temp567.f1=
tudres; _temp567.f2= _temp564; _temp567;}); _temp566;}); _temp565->f2= 1;
_temp565;}));}} return fields2;}} static void Cyc_Tc_tcTuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl*
tud){ struct _tagged_arr* v=(* tud->name).f2; struct _tagged_arr obj; if( tud->is_xtunion){
obj= _tag_arr("xtunion", sizeof( unsigned char), 8u);} else{ obj= _tag_arr("tunion",
sizeof( unsigned char), 7u);}{ struct Cyc_List_List* tvs= tud->tvs;{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 != 0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
struct Cyc_Absyn_Conref* c= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->kind); void* _temp568=( void*)
c->v; void* _temp576; _LL570: if( _temp568 ==( void*) Cyc_Absyn_No_constr){ goto
_LL571;} else{ goto _LL572;} _LL572: if(( unsigned int) _temp568 > 1u?*(( int*)
_temp568) == Cyc_Absyn_Eq_constr: 0){ _LL577: _temp576=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp568)->f1; if( _temp576 ==( void*) Cyc_Absyn_MemKind){ goto _LL573;} else{
goto _LL574;}} else{ goto _LL574;} _LL574: goto _LL575; _LL571:( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp578=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp578[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp579; _temp579.tag= Cyc_Absyn_Eq_constr; _temp579.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp579;}); _temp578;}))); goto _LL569;
_LL573: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp580)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf;
struct _tagged_arr _temp586= _tag_arr("%s %s attempts to abstract type variable %s of kind M",
sizeof( unsigned char), 54u); struct Cyc_Stdio_String_pa_struct _temp589;
_temp589.tag= Cyc_Stdio_String_pa; _temp589.f1=( struct _tagged_arr) obj;{ void*
_temp583=( void*)& _temp589; struct Cyc_Stdio_String_pa_struct _temp588;
_temp588.tag= Cyc_Stdio_String_pa; _temp588.f1=( struct _tagged_arr)* v;{ void*
_temp584=( void*)& _temp588; struct Cyc_Stdio_String_pa_struct _temp587;
_temp587.tag= Cyc_Stdio_String_pa; _temp587.f1=( struct _tagged_arr)*(( struct
Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->name;{ void*
_temp585=( void*)& _temp587; void* _temp581[ 3u]={ _temp583, _temp584, _temp585};
struct _tagged_arr _temp582={( void*) _temp581,( void*) _temp581,( void*)(
_temp581 + 3u)}; _temp580( _temp586, _temp582);}}}})); goto _LL569; _LL575: goto
_LL569; _LL569:;}} Cyc_Tcutil_check_unique_tvars( loc, tvs); Cyc_Tcutil_add_tvar_identities(
tvs);{ struct Cyc_Core_Opt* tud_opt; if( tud->is_xtunion){{ struct _handler_cons
_temp590; _push_handler(& _temp590);{ int _temp592= 0; if( setjmp( _temp590.handler)){
_temp592= 1;} if( ! _temp592){ tud_opt= Cyc_Tcenv_lookup_xtuniondecl( te, loc,
tud->name);; _pop_handler();} else{ void* _temp591=( void*) _exn_thrown; void*
_temp594= _temp591; _LL596: if( _temp594 == Cyc_Dict_Absent){ goto _LL597;}
else{ goto _LL598;} _LL598: goto _LL599; _LL597: Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp600)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr _temp604= _tag_arr("qualified xtunion declaration %s is not an existing xtunion",
sizeof( unsigned char), 60u); struct Cyc_Stdio_String_pa_struct _temp605;
_temp605.tag= Cyc_Stdio_String_pa; _temp605.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
tud->name);{ void* _temp603=( void*)& _temp605; void* _temp601[ 1u]={ _temp603};
struct _tagged_arr _temp602={( void*) _temp601,( void*) _temp601,( void*)(
_temp601 + 1u)}; _temp600( _temp604, _temp602);}})); return; _LL599:( void)
_throw( _temp594); _LL595:;}}} if( tud_opt != 0){ tud->name=(*(( struct Cyc_Absyn_Tuniondecl**)((
struct Cyc_Core_Opt*) _check_null( tud_opt))->v))->name;} else{(* tud->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp606=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp606[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp607; _temp607.tag= Cyc_Absyn_Abs_n; _temp607.f1= te->ns; _temp607;});
_temp606;});}} else{{ void* _temp608=(* tud->name).f1; struct Cyc_List_List*
_temp616; _LL610: if(( unsigned int) _temp608 > 1u?*(( int*) _temp608) == Cyc_Absyn_Rel_n:
0){ _LL617: _temp616=(( struct Cyc_Absyn_Rel_n_struct*) _temp608)->f1; if(
_temp616 == 0){ goto _LL611;} else{ goto _LL612;}} else{ goto _LL612;} _LL612:
if(( unsigned int) _temp608 > 1u?*(( int*) _temp608) == Cyc_Absyn_Abs_n: 0){
goto _LL613;} else{ goto _LL614;} _LL614: goto _LL615; _LL611:(* tud->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp618=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp618[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp619; _temp619.tag= Cyc_Absyn_Abs_n; _temp619.f1= te->ns; _temp619;});
_temp618;}); goto _LL609; _LL613: goto _LL615; _LL615: Cyc_Tcutil_terr( loc,
_tag_arr("qualified declarations are not implemented", sizeof( unsigned char),
43u)); return; _LL609:;} tud_opt=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)( ge->tuniondecls, v);}{ struct
_tuple4 _temp621=({ struct _tuple4 _temp620; _temp620.f1= tud->fields; _temp620.f2=
tud_opt; _temp620;}); struct Cyc_Core_Opt* _temp631; struct Cyc_Core_Opt*
_temp633; struct Cyc_Core_Opt* _temp635; struct Cyc_Core_Opt* _temp637; struct
Cyc_Core_Opt _temp639; struct Cyc_List_List* _temp640; struct Cyc_List_List**
_temp642; struct Cyc_Core_Opt* _temp643; struct Cyc_Core_Opt _temp645; struct
Cyc_Absyn_Tuniondecl** _temp646; struct Cyc_Core_Opt* _temp648; struct Cyc_Core_Opt
_temp650; struct Cyc_List_List* _temp651; struct Cyc_List_List** _temp653;
struct Cyc_Core_Opt* _temp654; struct Cyc_Core_Opt _temp656; struct Cyc_Absyn_Tuniondecl**
_temp657; struct Cyc_Core_Opt* _temp659; _LL623: _LL634: _temp633= _temp621.f1;
if( _temp633 == 0){ goto _LL632;} else{ goto _LL625;} _LL632: _temp631= _temp621.f2;
if( _temp631 == 0){ goto _LL624;} else{ goto _LL625;} _LL625: _LL638: _temp637=
_temp621.f1; if( _temp637 == 0){ goto _LL627;} else{ _temp639=* _temp637; _LL641:
_temp640=( struct Cyc_List_List*) _temp639.v; _temp642=&(* _temp621.f1).v; goto
_LL636;} _LL636: _temp635= _temp621.f2; if( _temp635 == 0){ goto _LL626;} else{
goto _LL627;} _LL627: _LL649: _temp648= _temp621.f1; if( _temp648 == 0){ goto
_LL629;} else{ _temp650=* _temp648; _LL652: _temp651=( struct Cyc_List_List*)
_temp650.v; _temp653=&(* _temp621.f1).v; goto _LL644;} _LL644: _temp643=
_temp621.f2; if( _temp643 == 0){ goto _LL629;} else{ _temp645=* _temp643; _LL647:
_temp646=( struct Cyc_Absyn_Tuniondecl**) _temp645.v; goto _LL628;} _LL629:
_LL660: _temp659= _temp621.f1; if( _temp659 == 0){ goto _LL655;} else{ goto
_LL622;} _LL655: _temp654= _temp621.f2; if( _temp654 == 0){ goto _LL622;} else{
_temp656=* _temp654; _LL658: _temp657=( struct Cyc_Absyn_Tuniondecl**) _temp656.v;
goto _LL630;} _LL624: ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)(
ge->tuniondecls, v,({ struct Cyc_Absyn_Tuniondecl** _temp661=( struct Cyc_Absyn_Tuniondecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp661[ 0]= tud; _temp661;}));
goto _LL622; _LL626: { struct Cyc_Absyn_Tuniondecl** _temp662=({ struct Cyc_Absyn_Tuniondecl**
_temp663=( struct Cyc_Absyn_Tuniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*));
_temp663[ 0]=({ struct Cyc_Absyn_Tuniondecl* _temp664=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp664->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp664->name= tud->name; _temp664->tvs= tvs; _temp664->fields=
0; _temp664->is_xtunion= tud->is_xtunion; _temp664;}); _temp663;}); ge->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)( ge->tuniondecls, v,
_temp662);* _temp642= Cyc_Tc_tcTunionFields( te, ge, loc, obj, tud->is_xtunion,
tud->name,* _temp642, tvs, tud);* _temp662= tud; goto _LL622;} _LL628: { struct
Cyc_Absyn_Tuniondecl* _temp665=* _temp646;* _temp646=({ struct Cyc_Absyn_Tuniondecl*
_temp666=( struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp666->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp666->name= tud->name;
_temp666->tvs= tvs; _temp666->fields= 0; _temp666->is_xtunion= tud->is_xtunion;
_temp666;});* _temp653= Cyc_Tc_tcTunionFields( te, ge, loc, obj, tud->is_xtunion,
tud->name,* _temp653, tvs, tud);* _temp646= _temp665; _temp657= _temp646; goto
_LL630;} _LL630: { struct Cyc_Absyn_Tuniondecl* _temp667= Cyc_Tcdecl_merge_tuniondecl(*
_temp657, tud, loc, Cyc_Tc_tc_msg); if( _temp667 == 0){ return;} else{* _temp657=(
struct Cyc_Absyn_Tuniondecl*) _check_null( _temp667); goto _LL622;}} _LL622:;}}}}
static void Cyc_Tc_tcEnumdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Enumdecl* ed){ struct
_tagged_arr* v=(* ed->name).f2;{ void* _temp668=(* ed->name).f1; struct Cyc_List_List*
_temp676; struct Cyc_List_List* _temp678; _LL670: if(( unsigned int) _temp668 >
1u?*(( int*) _temp668) == Cyc_Absyn_Rel_n: 0){ _LL677: _temp676=(( struct Cyc_Absyn_Rel_n_struct*)
_temp668)->f1; if( _temp676 == 0){ goto _LL671;} else{ goto _LL672;}} else{ goto
_LL672;} _LL672: if(( unsigned int) _temp668 > 1u?*(( int*) _temp668) == Cyc_Absyn_Abs_n:
0){ _LL679: _temp678=(( struct Cyc_Absyn_Abs_n_struct*) _temp668)->f1; if(
_temp678 == 0){ goto _LL673;} else{ goto _LL674;}} else{ goto _LL674;} _LL674:
goto _LL675; _LL671: goto _LL669; _LL673: goto _LL669; _LL675: Cyc_Tcutil_terr(
loc, _tag_arr("qualified declarations are not implemented", sizeof(
unsigned char), 43u)); return; _LL669:;}(* ed->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp680=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp680[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp681; _temp681.tag= Cyc_Absyn_Abs_n;
_temp681.f1= te->ns; _temp681;}); _temp680;}); if( ed->fields != 0){ struct
_RegionHandle _temp682= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp682; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0;
unsigned int tag_count= 0; struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( ed->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ struct Cyc_Absyn_Enumfield*
_temp683=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( fs))->hd;
if((( int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*), struct
Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp,
prev_fields,(* _temp683->name).f2)){ Cyc_Tcutil_terr( _temp683->loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp684)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_xprintf; struct _tagged_arr _temp688= _tag_arr("duplicate field name %s",
sizeof( unsigned char), 24u); struct Cyc_Stdio_String_pa_struct _temp689;
_temp689.tag= Cyc_Stdio_String_pa; _temp689.f1=( struct _tagged_arr)*(* _temp683->name).f2;{
void* _temp687=( void*)& _temp689; void* _temp685[ 1u]={ _temp687}; struct
_tagged_arr _temp686={( void*) _temp685,( void*) _temp685,( void*)( _temp685 + 1u)};
_temp684( _temp688, _temp686);}}));} else{ prev_fields=({ struct Cyc_List_List*
_temp690=( struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp690->hd=( void*)(* _temp683->name).f2; _temp690->tl= prev_fields; _temp690;});}
if( _temp683->tag == 0){ _temp683->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
tag_count, _temp683->loc);} else{ if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp683->tag))){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr(* _temp691)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_xprintf;
struct _tagged_arr _temp696= _tag_arr("enum %s, field %s: expression is not constant",
sizeof( unsigned char), 46u); struct Cyc_Stdio_String_pa_struct _temp698;
_temp698.tag= Cyc_Stdio_String_pa; _temp698.f1=( struct _tagged_arr)* v;{ void*
_temp694=( void*)& _temp698; struct Cyc_Stdio_String_pa_struct _temp697;
_temp697.tag= Cyc_Stdio_String_pa; _temp697.f1=( struct _tagged_arr)*(* _temp683->name).f2;{
void* _temp695=( void*)& _temp697; void* _temp692[ 2u]={ _temp694, _temp695};
struct _tagged_arr _temp693={( void*) _temp692,( void*) _temp692,( void*)(
_temp692 + 2u)}; _temp691( _temp696, _temp693);}}}));}}{ unsigned int t1= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp683->tag)); tag_count= t1 + 1;(*
_temp683->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp699=( struct
Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp699[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp700; _temp700.tag= Cyc_Absyn_Abs_n;
_temp700.f1= te->ns; _temp700;}); _temp699;});}}}; _pop_region( uprev_rgn);}{
struct _handler_cons _temp701; _push_handler(& _temp701);{ int _temp703= 0; if(
setjmp( _temp701.handler)){ _temp703= 1;} if( ! _temp703){{ struct Cyc_Absyn_Enumdecl**
_temp704=(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup)( ge->enumdecls, v); struct Cyc_Absyn_Enumdecl*
_temp705= Cyc_Tcdecl_merge_enumdecl(* _temp704, ed, loc, Cyc_Tc_tc_msg); if(
_temp705 == 0){ _npop_handler( 0u); return;}* _temp704=( struct Cyc_Absyn_Enumdecl*)
_check_null( _temp705);}; _pop_handler();} else{ void* _temp702=( void*)
_exn_thrown; void* _temp707= _temp702; _LL709: if( _temp707 == Cyc_Dict_Absent){
goto _LL710;} else{ goto _LL711;} _LL711: goto _LL712; _LL710: { struct Cyc_Absyn_Enumdecl**
_temp713=({ struct Cyc_Absyn_Enumdecl** _temp714=( struct Cyc_Absyn_Enumdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl*)); _temp714[ 0]= ed; _temp714;});
ge->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key, struct Cyc_Absyn_Enumdecl** data)) Cyc_Dict_insert)( ge->enumdecls,
v, _temp713); goto _LL708;} _LL712:( void) _throw( _temp707); _LL708:;}}} if( ed->fields
!= 0){ struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ed->fields))->v; for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ struct Cyc_Absyn_Enumfield* _temp715=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_List_List*) _check_null( fs))->hd; ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries,(* _temp715->name).f2,({ struct _tuple3* _temp716=( struct
_tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp716->f1=( void*)({ struct
Cyc_Tcenv_EnumRes_struct* _temp717=( struct Cyc_Tcenv_EnumRes_struct*) GC_malloc(
sizeof( struct Cyc_Tcenv_EnumRes_struct)); _temp717[ 0]=({ struct Cyc_Tcenv_EnumRes_struct
_temp718; _temp718.tag= Cyc_Tcenv_EnumRes; _temp718.f1= ed; _temp718.f2=
_temp715; _temp718;}); _temp717;}); _temp716->f2= 1; _temp716;}));}}} static int
Cyc_Tc_okay_externC( struct Cyc_Position_Segment* loc, void* sc){ void* _temp719=
sc; _LL721: if( _temp719 ==( void*) Cyc_Absyn_Static){ goto _LL722;} else{ goto
_LL723;} _LL723: if( _temp719 ==( void*) Cyc_Absyn_Abstract){ goto _LL724;}
else{ goto _LL725;} _LL725: if( _temp719 ==( void*) Cyc_Absyn_Public){ goto
_LL726;} else{ goto _LL727;} _LL727: if( _temp719 ==( void*) Cyc_Absyn_Extern){
goto _LL728;} else{ goto _LL729;} _LL729: if( _temp719 ==( void*) Cyc_Absyn_ExternC){
goto _LL730;} else{ goto _LL720;} _LL722: Cyc_Tcutil_warn( loc, _tag_arr("static declaration nested within extern \"C\"",
sizeof( unsigned char), 44u)); return 0; _LL724: Cyc_Tcutil_warn( loc, _tag_arr("abstract declaration nested within extern \"C\"",
sizeof( unsigned char), 46u)); return 0; _LL726: return 1; _LL728: return 1;
_LL730: Cyc_Tcutil_warn( loc, _tag_arr("nested extern \"C\" declaration",
sizeof( unsigned char), 30u)); return 1; _LL720:;} static void Cyc_Tc_tc_decls(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds0, int in_externC, int
check_var_init){ struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, te->ns);
struct Cyc_List_List* _temp731= ds0; for( 0; _temp731 != 0; _temp731=(( struct
Cyc_List_List*) _check_null( _temp731))->tl){ struct Cyc_Absyn_Decl* d=( struct
Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( _temp731))->hd; struct
Cyc_Position_Segment* loc= d->loc; void* _temp732=( void*) d->r; struct Cyc_Absyn_Vardecl*
_temp758; struct Cyc_Absyn_Fndecl* _temp760; struct Cyc_Absyn_Typedefdecl*
_temp762; struct Cyc_Absyn_Structdecl* _temp764; struct Cyc_Absyn_Uniondecl*
_temp766; struct Cyc_Absyn_Tuniondecl* _temp768; struct Cyc_Absyn_Enumdecl*
_temp770; struct Cyc_List_List* _temp772; struct _tagged_arr* _temp774; struct
Cyc_List_List* _temp776; struct _tuple0* _temp778; struct _tuple0 _temp780;
struct _tagged_arr* _temp781; void* _temp783; struct Cyc_List_List* _temp785;
_LL734: if(*(( int*) _temp732) == Cyc_Absyn_Let_d){ goto _LL735;} else{ goto
_LL736;} _LL736: if(*(( int*) _temp732) == Cyc_Absyn_Letv_d){ goto _LL737;}
else{ goto _LL738;} _LL738: if(*(( int*) _temp732) == Cyc_Absyn_Var_d){ _LL759:
_temp758=(( struct Cyc_Absyn_Var_d_struct*) _temp732)->f1; goto _LL739;} else{
goto _LL740;} _LL740: if(*(( int*) _temp732) == Cyc_Absyn_Fn_d){ _LL761:
_temp760=(( struct Cyc_Absyn_Fn_d_struct*) _temp732)->f1; goto _LL741;} else{
goto _LL742;} _LL742: if(*(( int*) _temp732) == Cyc_Absyn_Typedef_d){ _LL763:
_temp762=(( struct Cyc_Absyn_Typedef_d_struct*) _temp732)->f1; goto _LL743;}
else{ goto _LL744;} _LL744: if(*(( int*) _temp732) == Cyc_Absyn_Struct_d){
_LL765: _temp764=(( struct Cyc_Absyn_Struct_d_struct*) _temp732)->f1; goto
_LL745;} else{ goto _LL746;} _LL746: if(*(( int*) _temp732) == Cyc_Absyn_Union_d){
_LL767: _temp766=(( struct Cyc_Absyn_Union_d_struct*) _temp732)->f1; goto _LL747;}
else{ goto _LL748;} _LL748: if(*(( int*) _temp732) == Cyc_Absyn_Tunion_d){
_LL769: _temp768=(( struct Cyc_Absyn_Tunion_d_struct*) _temp732)->f1; goto
_LL749;} else{ goto _LL750;} _LL750: if(*(( int*) _temp732) == Cyc_Absyn_Enum_d){
_LL771: _temp770=(( struct Cyc_Absyn_Enum_d_struct*) _temp732)->f1; goto _LL751;}
else{ goto _LL752;} _LL752: if(*(( int*) _temp732) == Cyc_Absyn_Namespace_d){
_LL775: _temp774=(( struct Cyc_Absyn_Namespace_d_struct*) _temp732)->f1; goto
_LL773; _LL773: _temp772=(( struct Cyc_Absyn_Namespace_d_struct*) _temp732)->f2;
goto _LL753;} else{ goto _LL754;} _LL754: if(*(( int*) _temp732) == Cyc_Absyn_Using_d){
_LL779: _temp778=(( struct Cyc_Absyn_Using_d_struct*) _temp732)->f1; _temp780=*
_temp778; _LL784: _temp783= _temp780.f1; goto _LL782; _LL782: _temp781= _temp780.f2;
goto _LL777; _LL777: _temp776=(( struct Cyc_Absyn_Using_d_struct*) _temp732)->f2;
goto _LL755;} else{ goto _LL756;} _LL756: if(*(( int*) _temp732) == Cyc_Absyn_ExternC_d){
_LL786: _temp785=(( struct Cyc_Absyn_ExternC_d_struct*) _temp732)->f1; goto
_LL757;} else{ goto _LL733;} _LL735: Cyc_Tcutil_terr( loc, _tag_arr("top level let-declarations are not implemented",
sizeof( unsigned char), 47u)); goto _LL733; _LL737: Cyc_Tcutil_terr( loc,
_tag_arr("top level let-declarations are not implemented", sizeof( unsigned char),
47u)); goto _LL733; _LL739: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*)
_temp758->sc): 0){( void*)( _temp758->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcVardecl( te, ge, loc, _temp758, check_var_init); goto _LL733; _LL741:
if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp760->sc): 0){( void*)(
_temp760->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcFndecl( te, ge,
loc, _temp760); goto _LL733; _LL743: Cyc_Tc_tcTypedefdecl( te, ge, loc, _temp762);
goto _LL733; _LL745: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*)
_temp764->sc): 0){( void*)( _temp764->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcStructdecl( te, ge, loc, _temp764); goto _LL733; _LL747: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp766->sc): 0){( void*)( _temp766->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcUniondecl( te, ge, loc, _temp766);
goto _LL733; _LL749: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*)
_temp768->sc): 0){( void*)( _temp768->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcTuniondecl( te, ge, loc, _temp768); goto _LL733; _LL751: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp770->sc): 0){( void*)( _temp770->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcEnumdecl( te, ge, loc, _temp770);
goto _LL733; _LL753: { struct Cyc_List_List* _temp787= te->ns; struct Cyc_List_List*
_temp788=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp787,({ struct Cyc_List_List* _temp789=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp789->hd=( void*) _temp774;
_temp789->tl= 0; _temp789;})); if( !(( int(*)( struct Cyc_Set_Set* s, struct
_tagged_arr* elt)) Cyc_Set_member)( ge->namespaces, _temp774)){ ge->namespaces=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)(
ge->namespaces, _temp774); te->ae=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key, struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)( te->ae,
_temp788, Cyc_Tcenv_empty_genv());} te->ns= _temp788; Cyc_Tc_tc_decls( te,
_temp772, in_externC, check_var_init); te->ns= _temp787; goto _LL733;} _LL755: {
struct _tagged_arr* first; struct Cyc_List_List* rest;{ void* _temp790= _temp783;
struct Cyc_List_List* _temp802; struct Cyc_List_List* _temp804; struct Cyc_List_List*
_temp806; struct Cyc_List_List _temp808; struct Cyc_List_List* _temp809; struct
_tagged_arr* _temp811; struct Cyc_List_List* _temp813; struct Cyc_List_List
_temp815; struct Cyc_List_List* _temp816; struct _tagged_arr* _temp818; _LL792:
if( _temp790 ==( void*) Cyc_Absyn_Loc_n){ goto _LL793;} else{ goto _LL794;}
_LL794: if(( unsigned int) _temp790 > 1u?*(( int*) _temp790) == Cyc_Absyn_Rel_n:
0){ _LL803: _temp802=(( struct Cyc_Absyn_Rel_n_struct*) _temp790)->f1; if(
_temp802 == 0){ goto _LL795;} else{ goto _LL796;}} else{ goto _LL796;} _LL796:
if(( unsigned int) _temp790 > 1u?*(( int*) _temp790) == Cyc_Absyn_Abs_n: 0){
_LL805: _temp804=(( struct Cyc_Absyn_Abs_n_struct*) _temp790)->f1; if( _temp804
== 0){ goto _LL797;} else{ goto _LL798;}} else{ goto _LL798;} _LL798: if((
unsigned int) _temp790 > 1u?*(( int*) _temp790) == Cyc_Absyn_Rel_n: 0){ _LL807:
_temp806=(( struct Cyc_Absyn_Rel_n_struct*) _temp790)->f1; if( _temp806 == 0){
goto _LL800;} else{ _temp808=* _temp806; _LL812: _temp811=( struct _tagged_arr*)
_temp808.hd; goto _LL810; _LL810: _temp809= _temp808.tl; goto _LL799;}} else{
goto _LL800;} _LL800: if(( unsigned int) _temp790 > 1u?*(( int*) _temp790) ==
Cyc_Absyn_Abs_n: 0){ _LL814: _temp813=(( struct Cyc_Absyn_Abs_n_struct*)
_temp790)->f1; if( _temp813 == 0){ goto _LL791;} else{ _temp815=* _temp813;
_LL819: _temp818=( struct _tagged_arr*) _temp815.hd; goto _LL817; _LL817:
_temp816= _temp815.tl; goto _LL801;}} else{ goto _LL791;} _LL793: goto _LL795;
_LL795: goto _LL797; _LL797: first= _temp781; rest= 0; goto _LL791; _LL799:
_temp818= _temp811; _temp816= _temp809; goto _LL801; _LL801: first= _temp818;
rest=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp816,({ struct Cyc_List_List* _temp820=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp820->hd=( void*) _temp781;
_temp820->tl= 0; _temp820;})); goto _LL791; _LL791:;}{ struct Cyc_List_List*
_temp821= Cyc_Tcenv_resolve_namespace( te, loc, first, rest); ge->availables=({
struct Cyc_List_List* _temp822=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp822->hd=( void*) _temp821; _temp822->tl= ge->availables;
_temp822;}); Cyc_Tc_tc_decls( te, _temp776, in_externC, check_var_init); ge->availables=((
struct Cyc_List_List*) _check_null( ge->availables))->tl; goto _LL733;}} _LL757:
Cyc_Tc_tc_decls( te, _temp785, 1, check_var_init); goto _LL733; _LL733:;}} void
Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int check_var_init, struct Cyc_List_List*
ds){ Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r); Cyc_Tc_tc_decls( te, ds,
0, check_var_init);} static int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env,
struct Cyc_Absyn_Decl* d); static struct Cyc_List_List* Cyc_Tc_treeshake_f(
struct Cyc_Dict_Dict* env, struct Cyc_List_List* ds){ return(( struct Cyc_List_List*(*)(
int(* f)( struct Cyc_Dict_Dict*, struct Cyc_Absyn_Decl*), struct Cyc_Dict_Dict*
env, struct Cyc_List_List* l)) Cyc_List_filter_c)( Cyc_Tc_vardecl_needed, env,
ds);} struct _tuple6{ struct Cyc_Tcenv_Genv* f1; struct Cyc_Set_Set* f2; } ;
static int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl*
d){ void* _temp823=( void*) d->r; struct Cyc_Absyn_Vardecl* _temp833; struct Cyc_List_List*
_temp835; struct Cyc_List_List** _temp837; struct Cyc_List_List* _temp838;
struct Cyc_List_List** _temp840; _LL825: if(*(( int*) _temp823) == Cyc_Absyn_Var_d){
_LL834: _temp833=(( struct Cyc_Absyn_Var_d_struct*) _temp823)->f1; goto _LL826;}
else{ goto _LL827;} _LL827: if(*(( int*) _temp823) == Cyc_Absyn_Using_d){ _LL836:
_temp835=(( struct Cyc_Absyn_Using_d_struct*) _temp823)->f2; _temp837=&(( struct
Cyc_Absyn_Using_d_struct*) _temp823)->f2; goto _LL828;} else{ goto _LL829;}
_LL829: if(*(( int*) _temp823) == Cyc_Absyn_Namespace_d){ _LL839: _temp838=((
struct Cyc_Absyn_Namespace_d_struct*) _temp823)->f2; _temp840=&(( struct Cyc_Absyn_Namespace_d_struct*)
_temp823)->f2; goto _LL830;} else{ goto _LL831;} _LL831: goto _LL832; _LL826:
if(( void*) _temp833->sc !=( void*) Cyc_Absyn_Extern){ return 1;}{ struct
_tuple0 _temp843; struct _tagged_arr* _temp844; void* _temp846; struct _tuple0*
_temp841= _temp833->name; _temp843=* _temp841; _LL847: _temp846= _temp843.f1;
goto _LL845; _LL845: _temp844= _temp843.f2; goto _LL842; _LL842: { struct Cyc_List_List*
ns;{ void* _temp848= _temp846; struct Cyc_List_List* _temp856; struct Cyc_List_List*
_temp858; _LL850: if( _temp848 ==( void*) Cyc_Absyn_Loc_n){ goto _LL851;} else{
goto _LL852;} _LL852: if(( unsigned int) _temp848 > 1u?*(( int*) _temp848) ==
Cyc_Absyn_Rel_n: 0){ _LL857: _temp856=(( struct Cyc_Absyn_Rel_n_struct*)
_temp848)->f1; goto _LL853;} else{ goto _LL854;} _LL854: if(( unsigned int)
_temp848 > 1u?*(( int*) _temp848) == Cyc_Absyn_Abs_n: 0){ _LL859: _temp858=((
struct Cyc_Absyn_Abs_n_struct*) _temp848)->f1; goto _LL855;} else{ goto _LL849;}
_LL851: ns= 0; goto _LL849; _LL853: ns= _temp856; goto _LL849; _LL855: ns=
_temp858; goto _LL849; _LL849:;}{ struct _tuple6* _temp860=(( struct _tuple6*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( env, ns);
struct Cyc_Tcenv_Genv* _temp861=(* _temp860).f1; int _temp862=(*(( struct
_tuple3*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
_temp861->ordinaries, _temp844)).f2; if( ! _temp862){(* _temp860).f2=(( struct
Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)((*
_temp860).f2, _temp844);} return _temp862;}}} _LL828: _temp840= _temp837; goto
_LL830; _LL830:* _temp840= Cyc_Tc_treeshake_f( env,* _temp840); return 1; _LL832:
return 1; _LL824:;} static int Cyc_Tc_treeshake_remove_f( struct Cyc_Set_Set*
set, struct _tagged_arr* x, struct _tuple3* y){ return !(( int(*)( struct Cyc_Set_Set*
s, struct _tagged_arr* elt)) Cyc_Set_member)( set, x);} static void Cyc_Tc_treeshake_remove(
struct Cyc_List_List* ns, struct _tuple6* nsenv){ struct Cyc_Tcenv_Genv*
_temp863=(* nsenv).f1; struct Cyc_Set_Set* _temp864=(* nsenv).f2; _temp863->ordinaries=((
struct Cyc_Dict_Dict*(*)( int(* f)( struct Cyc_Set_Set*, struct _tagged_arr*,
struct _tuple3*), struct Cyc_Set_Set* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_filter_c)(
Cyc_Tc_treeshake_remove_f, _temp864, _temp863->ordinaries);} static struct
_tuple6* Cyc_Tc_treeshake_make_env_f( struct Cyc_Tcenv_Genv* ge){ return({
struct _tuple6* _temp865=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp865->f1= ge; _temp865->f2=(( struct Cyc_Set_Set*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Set_empty)( Cyc_String_strptrcmp);
_temp865;});} struct Cyc_List_List* Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv* te,
struct Cyc_List_List* ds){ struct Cyc_Dict_Dict* _temp866=(( struct Cyc_Dict_Dict*(*)(
struct _tuple6*(* f)( struct Cyc_Tcenv_Genv*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)(
Cyc_Tc_treeshake_make_env_f, te->ae); struct Cyc_List_List* _temp867= Cyc_Tc_treeshake_f(
_temp866, ds);(( void(*)( void(* f)( struct Cyc_List_List*, struct _tuple6*),
struct Cyc_Dict_Dict* d)) Cyc_Dict_iter)( Cyc_Tc_treeshake_remove, _temp866);
return _temp867;}