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
void Cyc_Tcutil_check_bitfield( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv*
te, void* field_typ, struct Cyc_Absyn_Exp* width, struct _tagged_arr* fn);
extern void Cyc_Tcutil_check_valid_toplevel_type( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, void*); extern void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Fndecl*);
extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, int allow_evars, void*); extern void
Cyc_Tcutil_check_unique_tvars( struct Cyc_Position_Segment*, struct Cyc_List_List*);
extern void Cyc_Tcutil_add_tvar_identities( struct Cyc_List_List*); extern int
Cyc_Tcutil_bits_only( void* t); extern int Cyc_Tcutil_is_const_exp( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e); extern int Cyc_Tcutil_supports_default( void*);
extern void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*, void**, struct
Cyc_Absyn_Exp*); extern void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Stmt* s, int new_block); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int
var_default_init, struct Cyc_List_List* ds); extern struct Cyc_List_List* Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*); extern unsigned char Cyc_Tcdecl_Incompatible[
17u]; struct Cyc_Tcdecl_Xtunionfielddecl{ struct Cyc_Absyn_Tuniondecl* base;
struct Cyc_Absyn_Tunionfield* field; } ; extern struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_structdecl(
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
loc, te, td->tvs,( void*) Cyc_Absyn_AnyKind, 0,( void*) td->defn); ge->typedefs=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Typedefdecl* data)) Cyc_Dict_insert)( ge->typedefs, v, td);}
static void Cyc_Tc_tcStructFields( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct _tagged_arr obj, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs){ struct _RegionHandle _temp249= _new_region();
struct _RegionHandle* uprev_rgn=& _temp249; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0; struct Cyc_List_List* _temp250= fields; for( 0;
_temp250 != 0; _temp250=(( struct Cyc_List_List*) _check_null( _temp250))->tl){
struct Cyc_Absyn_Structfield _temp253; struct Cyc_List_List* _temp254; struct
Cyc_Absyn_Exp* _temp256; void* _temp258; struct Cyc_Absyn_Tqual _temp260; struct
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
loc, te, tvs,( void*) Cyc_Absyn_MemKind, 0, _temp258); Cyc_Tcutil_check_bitfield(
loc, te, _temp258, _temp256, _temp262);}}; _pop_region( uprev_rgn);} static void
Cyc_Tc_tcStructOrUniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct _tagged_arr obj, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl*
sd){ struct _tagged_arr* _temp268=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v)).f2; if( sd->name == 0){({ struct Cyc_Stdio_String_pa_struct
_temp270; _temp270.tag= Cyc_Stdio_String_pa; _temp270.f1=( struct _tagged_arr)
obj;{ void* _temp269[ 1u]={& _temp270}; Cyc_Tcutil_terr( loc, _tag_arr("anonymous %ss are not allowed at top level",
sizeof( unsigned char), 43u), _tag_arr( _temp269, sizeof( void*), 1u));}});
return;}{ struct Cyc_List_List* atts= sd->attributes; for( 0; atts != 0; atts=((
struct Cyc_List_List*) _check_null( atts))->tl){ void* _temp271=( void*)((
struct Cyc_List_List*) _check_null( atts))->hd; _LL273: if( _temp271 ==( void*)
Cyc_Absyn_Packed_att){ goto _LL274;} else{ goto _LL275;} _LL275: if((
unsigned int) _temp271 > 16u?*(( int*) _temp271) == Cyc_Absyn_Aligned_att: 0){
goto _LL276;} else{ goto _LL277;} _LL277: goto _LL278; _LL274: continue; _LL276:
continue; _LL278:({ struct Cyc_Stdio_String_pa_struct _temp282; _temp282.tag=
Cyc_Stdio_String_pa; _temp282.f1=( struct _tagged_arr)* _temp268;{ struct Cyc_Stdio_String_pa_struct
_temp281; _temp281.tag= Cyc_Stdio_String_pa; _temp281.f1=( struct _tagged_arr)
obj;{ struct Cyc_Stdio_String_pa_struct _temp280; _temp280.tag= Cyc_Stdio_String_pa;
_temp280.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*)(( struct
Cyc_List_List*) _check_null( atts))->hd);{ void* _temp279[ 3u]={& _temp280,&
_temp281,& _temp282}; Cyc_Tcutil_terr( loc, _tag_arr("bad attribute %s in %s %s definition",
sizeof( unsigned char), 37u), _tag_arr( _temp279, sizeof( void*), 3u));}}}});
goto _LL272; _LL272:;}}{ struct Cyc_List_List* tvs= sd->tvs; for( 0; tvs != 0;
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ struct Cyc_Absyn_Conref*
_temp283= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd)->kind); void* _temp284=( void*) _temp283->v; void*
_temp292; _LL286: if( _temp284 ==( void*) Cyc_Absyn_No_constr){ goto _LL287;}
else{ goto _LL288;} _LL288: if(( unsigned int) _temp284 > 1u?*(( int*) _temp284)
== Cyc_Absyn_Eq_constr: 0){ _LL293: _temp292=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp284)->f1; if( _temp292 ==( void*) Cyc_Absyn_MemKind){ goto _LL289;} else{
goto _LL290;}} else{ goto _LL290;} _LL290: goto _LL291; _LL287:( void*)(
_temp283->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp294=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp294[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp295; _temp295.tag= Cyc_Absyn_Eq_constr;
_temp295.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp295;}); _temp294;})));
continue; _LL289:({ struct Cyc_Stdio_String_pa_struct _temp299; _temp299.tag=
Cyc_Stdio_String_pa; _temp299.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->name;{ struct Cyc_Stdio_String_pa_struct
_temp298; _temp298.tag= Cyc_Stdio_String_pa; _temp298.f1=( struct _tagged_arr)*
_temp268;{ struct Cyc_Stdio_String_pa_struct _temp297; _temp297.tag= Cyc_Stdio_String_pa;
_temp297.f1=( struct _tagged_arr) obj;{ void* _temp296[ 3u]={& _temp297,&
_temp298,& _temp299}; Cyc_Tcutil_terr( loc, _tag_arr("%s %s attempts to abstract type variable %s of kind M",
sizeof( unsigned char), 54u), _tag_arr( _temp296, sizeof( void*), 3u));}}}});
continue; _LL291: continue; _LL285:;}}{ void* _temp300=(*(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( sd->name))->v)).f1; struct Cyc_List_List*
_temp308; struct Cyc_List_List* _temp310; _LL302: if(( unsigned int) _temp300 >
1u?*(( int*) _temp300) == Cyc_Absyn_Rel_n: 0){ _LL309: _temp308=(( struct Cyc_Absyn_Rel_n_struct*)
_temp300)->f1; if( _temp308 == 0){ goto _LL303;} else{ goto _LL304;}} else{ goto
_LL304;} _LL304: if(( unsigned int) _temp300 > 1u?*(( int*) _temp300) == Cyc_Absyn_Abs_n:
0){ _LL311: _temp310=(( struct Cyc_Absyn_Abs_n_struct*) _temp300)->f1; if(
_temp310 == 0){ goto _LL305;} else{ goto _LL306;}} else{ goto _LL306;} _LL306:
goto _LL307; _LL303: goto _LL301; _LL305: goto _LL301; _LL307:({ void* _temp312[
0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("qualified declarations are not implemented",
sizeof( unsigned char), 43u), _tag_arr( _temp312, sizeof( void*), 0u));});
return; _LL301:;}(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp313=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp313[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp314; _temp314.tag= Cyc_Absyn_Abs_n; _temp314.f1= te->ns; _temp314;});
_temp313;}); Cyc_Tcutil_check_unique_tvars( loc, sd->tvs); Cyc_Tcutil_add_tvar_identities(
sd->tvs);} struct _tuple4{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ;
static void Cyc_Tc_tcStructdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl* sd){ struct
_tagged_arr* _temp315=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
sd->name))->v)).f2; const unsigned char* _temp316="struct"; Cyc_Tc_tcStructOrUniondecl(
te, ge, _tag_arr( _temp316, sizeof( unsigned char), 7u), loc, sd);{ struct Cyc_List_List*
_temp317= sd->tvs;{ struct _tuple4 _temp319=({ struct _tuple4 _temp318; _temp318.f1=
sd->fields; _temp318.f2=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup_opt)( ge->structdecls, _temp315);
_temp318;}); struct Cyc_Core_Opt* _temp329; struct Cyc_Core_Opt* _temp331;
struct Cyc_Core_Opt* _temp333; struct Cyc_Core_Opt* _temp335; struct Cyc_Core_Opt
_temp337; struct Cyc_List_List* _temp338; struct Cyc_Core_Opt* _temp340; struct
Cyc_Core_Opt _temp342; struct Cyc_Absyn_Structdecl** _temp343; struct Cyc_Core_Opt*
_temp345; struct Cyc_Core_Opt _temp347; struct Cyc_List_List* _temp348; struct
Cyc_Core_Opt* _temp350; struct Cyc_Core_Opt _temp352; struct Cyc_Absyn_Structdecl**
_temp353; struct Cyc_Core_Opt* _temp355; _LL321: _LL332: _temp331= _temp319.f1;
if( _temp331 == 0){ goto _LL330;} else{ goto _LL323;} _LL330: _temp329= _temp319.f2;
if( _temp329 == 0){ goto _LL322;} else{ goto _LL323;} _LL323: _LL336: _temp335=
_temp319.f1; if( _temp335 == 0){ goto _LL325;} else{ _temp337=* _temp335; _LL339:
_temp338=( struct Cyc_List_List*) _temp337.v; goto _LL334;} _LL334: _temp333=
_temp319.f2; if( _temp333 == 0){ goto _LL324;} else{ goto _LL325;} _LL325:
_LL346: _temp345= _temp319.f1; if( _temp345 == 0){ goto _LL327;} else{ _temp347=*
_temp345; _LL349: _temp348=( struct Cyc_List_List*) _temp347.v; goto _LL341;}
_LL341: _temp340= _temp319.f2; if( _temp340 == 0){ goto _LL327;} else{ _temp342=*
_temp340; _LL344: _temp343=( struct Cyc_Absyn_Structdecl**) _temp342.v; goto
_LL326;} _LL327: _LL356: _temp355= _temp319.f1; if( _temp355 == 0){ goto _LL351;}
else{ goto _LL320;} _LL351: _temp350= _temp319.f2; if( _temp350 == 0){ goto
_LL320;} else{ _temp352=* _temp350; _LL354: _temp353=( struct Cyc_Absyn_Structdecl**)
_temp352.v; goto _LL328;} _LL322: ge->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl**
data)) Cyc_Dict_insert)( ge->structdecls, _temp315,({ struct Cyc_Absyn_Structdecl**
_temp357=( struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp357[ 0]= sd; _temp357;})); goto _LL320; _LL324: { struct Cyc_Absyn_Structdecl**
_temp358=({ struct Cyc_Absyn_Structdecl** _temp359=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp359[ 0]=({ struct Cyc_Absyn_Structdecl*
_temp360=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp360->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp360->name= sd->name;
_temp360->tvs= _temp317; _temp360->fields= 0; _temp360->attributes= 0; _temp360;});
_temp359;}); ge->structdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl** data)) Cyc_Dict_insert)(
ge->structdecls, _temp315, _temp358); Cyc_Tc_tcStructFields( te, ge, loc,
_tag_arr( _temp316, sizeof( unsigned char), 7u), _temp338, _temp317);* _temp358=
sd; goto _LL320;} _LL326: { struct Cyc_Absyn_Structdecl* _temp361=* _temp343;*
_temp343=({ struct Cyc_Absyn_Structdecl* _temp362=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp362->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp362->name= sd->name; _temp362->tvs= _temp317; _temp362->fields=
0; _temp362->attributes= 0; _temp362;}); Cyc_Tc_tcStructFields( te, ge, loc,
_tag_arr( _temp316, sizeof( unsigned char), 7u), _temp348, _temp317);* _temp343=
_temp361; _temp353= _temp343; goto _LL328;} _LL328: { struct Cyc_Absyn_Structdecl*
_temp363= Cyc_Tcdecl_merge_structdecl(* _temp353, sd, loc, Cyc_Tc_tc_msg); if(
_temp363 == 0){ return;} else{* _temp353=( struct Cyc_Absyn_Structdecl*)
_check_null( _temp363); sd=( struct Cyc_Absyn_Structdecl*) _check_null( _temp363);
goto _LL320;}} _LL320:;} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,
_temp315,({ struct _tuple3* _temp364=( struct _tuple3*) GC_malloc( sizeof(
struct _tuple3)); _temp364->f1=( void*)({ struct Cyc_Tcenv_StructRes_struct*
_temp365=( struct Cyc_Tcenv_StructRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_StructRes_struct));
_temp365[ 0]=({ struct Cyc_Tcenv_StructRes_struct _temp366; _temp366.tag= Cyc_Tcenv_StructRes;
_temp366.f1= sd; _temp366;}); _temp365;}); _temp364->f2= 1; _temp364;}));}}
static void Cyc_Tc_tcUniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl* ud){ struct
_tagged_arr* _temp367=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
ud->name))->v)).f2; const unsigned char* _temp368="union"; Cyc_Tc_tcStructOrUniondecl(
te, ge, _tag_arr( _temp368, sizeof( unsigned char), 6u), loc,( struct Cyc_Absyn_Structdecl*)
ud);{ struct Cyc_List_List* _temp369= ud->tvs; struct _tuple4 _temp371=({ struct
_tuple4 _temp370; _temp370.f1= ud->fields; _temp370.f2=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)( ge->uniondecls,
_temp367); _temp370;}); struct Cyc_Core_Opt* _temp381; struct Cyc_Core_Opt*
_temp383; struct Cyc_Core_Opt* _temp385; struct Cyc_Core_Opt* _temp387; struct
Cyc_Core_Opt _temp389; struct Cyc_List_List* _temp390; struct Cyc_Core_Opt*
_temp392; struct Cyc_Core_Opt _temp394; struct Cyc_Absyn_Uniondecl** _temp395;
struct Cyc_Core_Opt* _temp397; struct Cyc_Core_Opt _temp399; struct Cyc_List_List*
_temp400; struct Cyc_Core_Opt* _temp402; struct Cyc_Core_Opt _temp404; struct
Cyc_Absyn_Uniondecl** _temp405; struct Cyc_Core_Opt* _temp407; _LL373: _LL384:
_temp383= _temp371.f1; if( _temp383 == 0){ goto _LL382;} else{ goto _LL375;}
_LL382: _temp381= _temp371.f2; if( _temp381 == 0){ goto _LL374;} else{ goto
_LL375;} _LL375: _LL388: _temp387= _temp371.f1; if( _temp387 == 0){ goto _LL377;}
else{ _temp389=* _temp387; _LL391: _temp390=( struct Cyc_List_List*) _temp389.v;
goto _LL386;} _LL386: _temp385= _temp371.f2; if( _temp385 == 0){ goto _LL376;}
else{ goto _LL377;} _LL377: _LL398: _temp397= _temp371.f1; if( _temp397 == 0){
goto _LL379;} else{ _temp399=* _temp397; _LL401: _temp400=( struct Cyc_List_List*)
_temp399.v; goto _LL393;} _LL393: _temp392= _temp371.f2; if( _temp392 == 0){
goto _LL379;} else{ _temp394=* _temp392; _LL396: _temp395=( struct Cyc_Absyn_Uniondecl**)
_temp394.v; goto _LL378;} _LL379: _LL408: _temp407= _temp371.f1; if( _temp407 ==
0){ goto _LL403;} else{ goto _LL372;} _LL403: _temp402= _temp371.f2; if(
_temp402 == 0){ goto _LL372;} else{ _temp404=* _temp402; _LL406: _temp405=(
struct Cyc_Absyn_Uniondecl**) _temp404.v; goto _LL380;} _LL374: ge->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)( ge->uniondecls, _temp367,({
struct Cyc_Absyn_Uniondecl** _temp409=( struct Cyc_Absyn_Uniondecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl*)); _temp409[ 0]= ud; _temp409;})); goto
_LL372; _LL376: { struct Cyc_Absyn_Uniondecl** _temp410=({ struct Cyc_Absyn_Uniondecl**
_temp415=( struct Cyc_Absyn_Uniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl*));
_temp415[ 0]=({ struct Cyc_Absyn_Uniondecl* _temp416=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp416->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp416->name= ud->name; _temp416->tvs= _temp369; _temp416->fields=
0; _temp416->attributes= ud->attributes; _temp416;}); _temp415;}); ge->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)( ge->uniondecls, _temp367,
_temp410); Cyc_Tc_tcStructFields( te, ge, loc, _tag_arr( _temp368, sizeof(
unsigned char), 6u), _temp390, _temp369);{ struct Cyc_List_List* f= _temp390;
for( 0; f != 0; f=(( struct Cyc_List_List*) _check_null( f))->tl){ if( ! Cyc_Tcutil_bits_only((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( f))->hd)->type)){({
struct Cyc_Stdio_String_pa_struct _temp414; _temp414.tag= Cyc_Stdio_String_pa;
_temp414.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( f))->hd)->type);{ struct Cyc_Stdio_String_pa_struct
_temp413; _temp413.tag= Cyc_Stdio_String_pa; _temp413.f1=( struct _tagged_arr)*
_temp367;{ struct Cyc_Stdio_String_pa_struct _temp412; _temp412.tag= Cyc_Stdio_String_pa;
_temp412.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( f))->hd)->name;{ void* _temp411[ 3u]={& _temp412,& _temp413,&
_temp414}; Cyc_Tcutil_terr( loc, _tag_arr("field %s of union %s has type %s which is not `bits-only'",
sizeof( unsigned char), 58u), _tag_arr( _temp411, sizeof( void*), 3u));}}}});}}}*
_temp410= ud; goto _LL372;} _LL378: { struct Cyc_Absyn_Uniondecl* _temp417=*
_temp395;* _temp395=({ struct Cyc_Absyn_Uniondecl* _temp418=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp418->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp418->name= ud->name; _temp418->tvs= _temp369; _temp418->fields=
0; _temp418->attributes= ud->attributes; _temp418;}); Cyc_Tc_tcStructFields( te,
ge, loc, _tag_arr( _temp368, sizeof( unsigned char), 6u), _temp400, _temp369);*
_temp395= _temp417; _temp405= _temp395; goto _LL380;} _LL380: { struct Cyc_Absyn_Uniondecl*
_temp419= Cyc_Tcdecl_merge_uniondecl(* _temp405, ud, loc, Cyc_Tc_tc_msg); if(
_temp419 == 0){ return;} else{* _temp405=( struct Cyc_Absyn_Uniondecl*)
_check_null( _temp419); goto _LL372;}} _LL372:;}} struct _tuple5{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; static struct Cyc_List_List* Cyc_Tc_tcTunionFields( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct
_tagged_arr obj, int is_xtunion, struct _tuple0* name, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs, struct Cyc_Absyn_Tuniondecl* tudres){{ struct
Cyc_List_List* _temp420= fields; for( 0; _temp420 != 0; _temp420=(( struct Cyc_List_List*)
_check_null( _temp420))->tl){ struct Cyc_Absyn_Tunionfield* _temp421=( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp420))->hd;{
struct Cyc_List_List* tvs= _temp421->tvs; for( 0; tvs != 0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){ struct Cyc_Absyn_Tvar* _temp422=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd; struct Cyc_Absyn_Conref* _temp423=
Cyc_Absyn_compress_conref( _temp422->kind); void* _temp424=( void*) _temp423->v;
void* _temp432; _LL426: if( _temp424 ==( void*) Cyc_Absyn_No_constr){ goto
_LL427;} else{ goto _LL428;} _LL428: if(( unsigned int) _temp424 > 1u?*(( int*)
_temp424) == Cyc_Absyn_Eq_constr: 0){ _LL433: _temp432=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp424)->f1; if( _temp432 ==( void*) Cyc_Absyn_MemKind){ goto _LL429;} else{
goto _LL430;}} else{ goto _LL430;} _LL430: goto _LL431; _LL427:( void*)(
_temp423->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp434=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp434[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp435; _temp435.tag= Cyc_Absyn_Eq_constr;
_temp435.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp435;}); _temp434;})));
goto _LL425; _LL429:({ struct Cyc_Stdio_String_pa_struct _temp437; _temp437.tag=
Cyc_Stdio_String_pa; _temp437.f1=( struct _tagged_arr)*(* _temp421->name).f2;{
void* _temp436[ 1u]={& _temp437}; Cyc_Tcutil_terr( _temp421->loc, _tag_arr("field %s abstracts type variable of kind M",
sizeof( unsigned char), 43u), _tag_arr( _temp436, sizeof( void*), 1u));}}); goto
_LL425; _LL431: goto _LL425; _LL425:;}}{ struct Cyc_List_List* alltvs=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tvs, _temp421->tvs); Cyc_Tcutil_check_unique_tvars( loc, alltvs); Cyc_Tcutil_add_tvar_identities(
_temp421->tvs);{ struct Cyc_List_List* typs= _temp421->typs; for( 0; typs != 0;
typs=(( struct Cyc_List_List*) _check_null( typs))->tl){ Cyc_Tcutil_check_type(
_temp421->loc, te, alltvs,( void*) Cyc_Absyn_MemKind, 0,(*(( struct _tuple5*)((
struct Cyc_List_List*) _check_null( typs))->hd)).f2);}}{ void* _temp438=(*
_temp421->name).f1; struct Cyc_List_List* _temp448; _LL440: if(( unsigned int)
_temp438 > 1u?*(( int*) _temp438) == Cyc_Absyn_Rel_n: 0){ _LL449: _temp448=((
struct Cyc_Absyn_Rel_n_struct*) _temp438)->f1; if( _temp448 == 0){ goto _LL441;}
else{ goto _LL442;}} else{ goto _LL442;} _LL442: if(( unsigned int) _temp438 > 1u?*((
int*) _temp438) == Cyc_Absyn_Rel_n: 0){ goto _LL443;} else{ goto _LL444;} _LL444:
if(( unsigned int) _temp438 > 1u?*(( int*) _temp438) == Cyc_Absyn_Abs_n: 0){
goto _LL445;} else{ goto _LL446;} _LL446: if( _temp438 ==( void*) Cyc_Absyn_Loc_n){
goto _LL447;} else{ goto _LL439;} _LL441: if( is_xtunion){(* _temp421->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp450=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp450[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp451; _temp451.tag= Cyc_Absyn_Abs_n; _temp451.f1= te->ns; _temp451;});
_temp450;});} else{(* _temp421->name).f1=(* name).f1;} goto _LL439; _LL443:({
void* _temp452[ 0u]={}; Cyc_Tcutil_terr( _temp421->loc, _tag_arr("qualified declarations are not allowed",
sizeof( unsigned char), 39u), _tag_arr( _temp452, sizeof( void*), 0u));}); goto
_LL439; _LL445: goto _LL439; _LL447:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp453=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp453[ 0]=({ struct Cyc_Core_Impossible_struct _temp454; _temp454.tag= Cyc_Core_Impossible;
_temp454.f1= _tag_arr("tcTunionFields: Loc_n", sizeof( unsigned char), 22u);
_temp454;}); _temp453;})); goto _LL439; _LL439:;}}}}{ struct Cyc_List_List*
fields2; if( is_xtunion){ int _temp455= 1; struct Cyc_List_List* _temp456= Cyc_Tcdecl_sort_xtunion_fields(
fields,& _temp455,(* name).f2, loc, Cyc_Tc_tc_msg); if( _temp455){ fields2=
_temp456;} else{ fields2= 0;}} else{ struct _RegionHandle _temp457= _new_region();
struct _RegionHandle* uprev_rgn=& _temp457; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0;{ struct Cyc_List_List* fs= fields; for( 0; fs !=
0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){ struct Cyc_Absyn_Tunionfield*
_temp458=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fs))->hd; if((( int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*),
struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp,
prev_fields,(* _temp458->name).f2)){({ struct Cyc_Stdio_String_pa_struct
_temp461; _temp461.tag= Cyc_Stdio_String_pa; _temp461.f1=( struct _tagged_arr)
obj;{ struct Cyc_Stdio_String_pa_struct _temp460; _temp460.tag= Cyc_Stdio_String_pa;
_temp460.f1=( struct _tagged_arr)*(* _temp458->name).f2;{ void* _temp459[ 2u]={&
_temp460,& _temp461}; Cyc_Tcutil_terr( _temp458->loc, _tag_arr("duplicate field name %s in %s",
sizeof( unsigned char), 30u), _tag_arr( _temp459, sizeof( void*), 2u));}}});}
else{ prev_fields=({ struct Cyc_List_List* _temp462=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp462->hd=( void*)(*
_temp458->name).f2; _temp462->tl= prev_fields; _temp462;});} if(( void*)
_temp458->sc !=( void*) Cyc_Absyn_Public){({ struct Cyc_Stdio_String_pa_struct
_temp464; _temp464.tag= Cyc_Stdio_String_pa; _temp464.f1=( struct _tagged_arr)*(*
_temp458->name).f2;{ void* _temp463[ 1u]={& _temp464}; Cyc_Tcutil_warn( loc,
_tag_arr("ignoring scope of field %s", sizeof( unsigned char), 27u), _tag_arr(
_temp463, sizeof( void*), 1u));}});( void*)( _temp458->sc=( void*)(( void*) Cyc_Absyn_Public));}}}
fields2= fields;}; _pop_region( uprev_rgn);}{ struct Cyc_List_List* _temp465=
fields; for( 0; _temp465 != 0; _temp465=(( struct Cyc_List_List*) _check_null(
_temp465))->tl){ struct Cyc_Absyn_Tunionfield* _temp466=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp465))->hd; ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries,(* _temp466->name).f2,({ struct _tuple3* _temp467=( struct
_tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp467->f1=( void*)({ struct
Cyc_Tcenv_TunionRes_struct* _temp468=( struct Cyc_Tcenv_TunionRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct)); _temp468[ 0]=({ struct
Cyc_Tcenv_TunionRes_struct _temp469; _temp469.tag= Cyc_Tcenv_TunionRes; _temp469.f1=
tudres; _temp469.f2= _temp466; _temp469;}); _temp468;}); _temp467->f2= 1;
_temp467;}));}} return fields2;}} static void Cyc_Tc_tcTuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl*
tud){ struct _tagged_arr* v=(* tud->name).f2; struct _tagged_arr obj; if( tud->is_xtunion){
obj= _tag_arr("xtunion", sizeof( unsigned char), 8u);} else{ obj= _tag_arr("tunion",
sizeof( unsigned char), 7u);}{ struct Cyc_List_List* tvs= tud->tvs;{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 != 0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
struct Cyc_Absyn_Conref* c= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->kind); void* _temp470=( void*)
c->v; void* _temp478; _LL472: if( _temp470 ==( void*) Cyc_Absyn_No_constr){ goto
_LL473;} else{ goto _LL474;} _LL474: if(( unsigned int) _temp470 > 1u?*(( int*)
_temp470) == Cyc_Absyn_Eq_constr: 0){ _LL479: _temp478=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp470)->f1; if( _temp478 ==( void*) Cyc_Absyn_MemKind){ goto _LL475;} else{
goto _LL476;}} else{ goto _LL476;} _LL476: goto _LL477; _LL473:( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp480=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp480[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp481; _temp481.tag= Cyc_Absyn_Eq_constr; _temp481.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp481;}); _temp480;}))); goto _LL471;
_LL475:({ struct Cyc_Stdio_String_pa_struct _temp485; _temp485.tag= Cyc_Stdio_String_pa;
_temp485.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->name;{ struct Cyc_Stdio_String_pa_struct _temp484;
_temp484.tag= Cyc_Stdio_String_pa; _temp484.f1=( struct _tagged_arr)* v;{ struct
Cyc_Stdio_String_pa_struct _temp483; _temp483.tag= Cyc_Stdio_String_pa; _temp483.f1=(
struct _tagged_arr) obj;{ void* _temp482[ 3u]={& _temp483,& _temp484,& _temp485};
Cyc_Tcutil_terr( loc, _tag_arr("%s %s attempts to abstract type variable %s of kind M",
sizeof( unsigned char), 54u), _tag_arr( _temp482, sizeof( void*), 3u));}}}});
goto _LL471; _LL477: goto _LL471; _LL471:;}} Cyc_Tcutil_check_unique_tvars( loc,
tvs); Cyc_Tcutil_add_tvar_identities( tvs);{ struct Cyc_Core_Opt* tud_opt; if(
tud->is_xtunion){{ struct _handler_cons _temp486; _push_handler(& _temp486);{
int _temp488= 0; if( setjmp( _temp486.handler)){ _temp488= 1;} if( ! _temp488){
tud_opt= Cyc_Tcenv_lookup_xtuniondecl( te, loc, tud->name);; _pop_handler();}
else{ void* _temp487=( void*) _exn_thrown; void* _temp490= _temp487; _LL492: if(
_temp490 == Cyc_Dict_Absent){ goto _LL493;} else{ goto _LL494;} _LL494: goto
_LL495; _LL493:({ struct Cyc_Stdio_String_pa_struct _temp497; _temp497.tag= Cyc_Stdio_String_pa;
_temp497.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( tud->name);{ void*
_temp496[ 1u]={& _temp497}; Cyc_Tcutil_terr( loc, _tag_arr("qualified xtunion declaration %s is not an existing xtunion",
sizeof( unsigned char), 60u), _tag_arr( _temp496, sizeof( void*), 1u));}});
return; _LL495:( void) _throw( _temp490); _LL491:;}}} if( tud_opt != 0){ tud->name=(*((
struct Cyc_Absyn_Tuniondecl**)(( struct Cyc_Core_Opt*) _check_null( tud_opt))->v))->name;}
else{(* tud->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp498=(
struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp498[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp499; _temp499.tag= Cyc_Absyn_Abs_n;
_temp499.f1= te->ns; _temp499;}); _temp498;});}} else{{ void* _temp500=(* tud->name).f1;
struct Cyc_List_List* _temp508; _LL502: if(( unsigned int) _temp500 > 1u?*(( int*)
_temp500) == Cyc_Absyn_Rel_n: 0){ _LL509: _temp508=(( struct Cyc_Absyn_Rel_n_struct*)
_temp500)->f1; if( _temp508 == 0){ goto _LL503;} else{ goto _LL504;}} else{ goto
_LL504;} _LL504: if(( unsigned int) _temp500 > 1u?*(( int*) _temp500) == Cyc_Absyn_Abs_n:
0){ goto _LL505;} else{ goto _LL506;} _LL506: goto _LL507; _LL503:(* tud->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp510=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp510[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp511; _temp511.tag= Cyc_Absyn_Abs_n; _temp511.f1= te->ns; _temp511;});
_temp510;}); goto _LL501; _LL505: goto _LL507; _LL507:({ void* _temp512[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("qualified declarations are not implemented",
sizeof( unsigned char), 43u), _tag_arr( _temp512, sizeof( void*), 0u));});
return; _LL501:;} tud_opt=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup_opt)( ge->tuniondecls, v);}{ struct
_tuple4 _temp514=({ struct _tuple4 _temp513; _temp513.f1= tud->fields; _temp513.f2=
tud_opt; _temp513;}); struct Cyc_Core_Opt* _temp524; struct Cyc_Core_Opt*
_temp526; struct Cyc_Core_Opt* _temp528; struct Cyc_Core_Opt* _temp530; struct
Cyc_Core_Opt _temp532; struct Cyc_List_List* _temp533; struct Cyc_List_List**
_temp535; struct Cyc_Core_Opt* _temp536; struct Cyc_Core_Opt _temp538; struct
Cyc_Absyn_Tuniondecl** _temp539; struct Cyc_Core_Opt* _temp541; struct Cyc_Core_Opt
_temp543; struct Cyc_List_List* _temp544; struct Cyc_List_List** _temp546;
struct Cyc_Core_Opt* _temp547; struct Cyc_Core_Opt _temp549; struct Cyc_Absyn_Tuniondecl**
_temp550; struct Cyc_Core_Opt* _temp552; _LL516: _LL527: _temp526= _temp514.f1;
if( _temp526 == 0){ goto _LL525;} else{ goto _LL518;} _LL525: _temp524= _temp514.f2;
if( _temp524 == 0){ goto _LL517;} else{ goto _LL518;} _LL518: _LL531: _temp530=
_temp514.f1; if( _temp530 == 0){ goto _LL520;} else{ _temp532=* _temp530; _LL534:
_temp533=( struct Cyc_List_List*) _temp532.v; _temp535=&(* _temp514.f1).v; goto
_LL529;} _LL529: _temp528= _temp514.f2; if( _temp528 == 0){ goto _LL519;} else{
goto _LL520;} _LL520: _LL542: _temp541= _temp514.f1; if( _temp541 == 0){ goto
_LL522;} else{ _temp543=* _temp541; _LL545: _temp544=( struct Cyc_List_List*)
_temp543.v; _temp546=&(* _temp514.f1).v; goto _LL537;} _LL537: _temp536=
_temp514.f2; if( _temp536 == 0){ goto _LL522;} else{ _temp538=* _temp536; _LL540:
_temp539=( struct Cyc_Absyn_Tuniondecl**) _temp538.v; goto _LL521;} _LL522:
_LL553: _temp552= _temp514.f1; if( _temp552 == 0){ goto _LL548;} else{ goto
_LL515;} _LL548: _temp547= _temp514.f2; if( _temp547 == 0){ goto _LL515;} else{
_temp549=* _temp547; _LL551: _temp550=( struct Cyc_Absyn_Tuniondecl**) _temp549.v;
goto _LL523;} _LL517: ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)(
ge->tuniondecls, v,({ struct Cyc_Absyn_Tuniondecl** _temp554=( struct Cyc_Absyn_Tuniondecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp554[ 0]= tud; _temp554;}));
goto _LL515; _LL519: { struct Cyc_Absyn_Tuniondecl** _temp555=({ struct Cyc_Absyn_Tuniondecl**
_temp556=( struct Cyc_Absyn_Tuniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*));
_temp556[ 0]=({ struct Cyc_Absyn_Tuniondecl* _temp557=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp557->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp557->name= tud->name; _temp557->tvs= tvs; _temp557->fields=
0; _temp557->is_xtunion= tud->is_xtunion; _temp557;}); _temp556;}); ge->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)( ge->tuniondecls, v,
_temp555);* _temp535= Cyc_Tc_tcTunionFields( te, ge, loc, obj, tud->is_xtunion,
tud->name,* _temp535, tvs, tud);* _temp555= tud; goto _LL515;} _LL521: { struct
Cyc_Absyn_Tuniondecl* _temp558=* _temp539;* _temp539=({ struct Cyc_Absyn_Tuniondecl*
_temp559=( struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp559->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp559->name= tud->name;
_temp559->tvs= tvs; _temp559->fields= 0; _temp559->is_xtunion= tud->is_xtunion;
_temp559;});* _temp546= Cyc_Tc_tcTunionFields( te, ge, loc, obj, tud->is_xtunion,
tud->name,* _temp546, tvs, tud);* _temp539= _temp558; _temp550= _temp539; goto
_LL523;} _LL523: { struct Cyc_Absyn_Tuniondecl* _temp560= Cyc_Tcdecl_merge_tuniondecl(*
_temp550, tud, loc, Cyc_Tc_tc_msg); if( _temp560 == 0){ return;} else{* _temp550=(
struct Cyc_Absyn_Tuniondecl*) _check_null( _temp560); goto _LL515;}} _LL515:;}}}}
static void Cyc_Tc_tcEnumdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Enumdecl* ed){ struct
_tagged_arr* v=(* ed->name).f2;{ void* _temp561=(* ed->name).f1; struct Cyc_List_List*
_temp569; struct Cyc_List_List* _temp571; _LL563: if(( unsigned int) _temp561 >
1u?*(( int*) _temp561) == Cyc_Absyn_Rel_n: 0){ _LL570: _temp569=(( struct Cyc_Absyn_Rel_n_struct*)
_temp561)->f1; if( _temp569 == 0){ goto _LL564;} else{ goto _LL565;}} else{ goto
_LL565;} _LL565: if(( unsigned int) _temp561 > 1u?*(( int*) _temp561) == Cyc_Absyn_Abs_n:
0){ _LL572: _temp571=(( struct Cyc_Absyn_Abs_n_struct*) _temp561)->f1; if(
_temp571 == 0){ goto _LL566;} else{ goto _LL567;}} else{ goto _LL567;} _LL567:
goto _LL568; _LL564: goto _LL562; _LL566: goto _LL562; _LL568:({ void* _temp573[
0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("qualified declarations are not implemented",
sizeof( unsigned char), 43u), _tag_arr( _temp573, sizeof( void*), 0u));});
return; _LL562:;}(* ed->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp574=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp574[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp575; _temp575.tag= Cyc_Absyn_Abs_n;
_temp575.f1= te->ns; _temp575;}); _temp574;}); if( ed->fields != 0){ struct
_RegionHandle _temp576= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp576; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0;
unsigned int tag_count= 0; struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( ed->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ struct Cyc_Absyn_Enumfield*
_temp577=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( fs))->hd;
if((( int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*), struct
Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp,
prev_fields,(* _temp577->name).f2)){({ struct Cyc_Stdio_String_pa_struct
_temp579; _temp579.tag= Cyc_Stdio_String_pa; _temp579.f1=( struct _tagged_arr)*(*
_temp577->name).f2;{ void* _temp578[ 1u]={& _temp579}; Cyc_Tcutil_terr( _temp577->loc,
_tag_arr("duplicate field name %s", sizeof( unsigned char), 24u), _tag_arr(
_temp578, sizeof( void*), 1u));}});} else{ prev_fields=({ struct Cyc_List_List*
_temp580=( struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp580->hd=( void*)(* _temp577->name).f2; _temp580->tl= prev_fields; _temp580;});}
if( _temp577->tag == 0){ _temp577->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
tag_count, _temp577->loc);} else{ if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp577->tag))){({ struct Cyc_Stdio_String_pa_struct _temp583;
_temp583.tag= Cyc_Stdio_String_pa; _temp583.f1=( struct _tagged_arr)*(* _temp577->name).f2;{
struct Cyc_Stdio_String_pa_struct _temp582; _temp582.tag= Cyc_Stdio_String_pa;
_temp582.f1=( struct _tagged_arr)* v;{ void* _temp581[ 2u]={& _temp582,&
_temp583}; Cyc_Tcutil_terr( loc, _tag_arr("enum %s, field %s: expression is not constant",
sizeof( unsigned char), 46u), _tag_arr( _temp581, sizeof( void*), 2u));}}});}}{
unsigned int t1= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp577->tag)); tag_count= t1 + 1;(* _temp577->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp584=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp584[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp585; _temp585.tag= Cyc_Absyn_Abs_n; _temp585.f1= te->ns; _temp585;});
_temp584;});}}}; _pop_region( uprev_rgn);}{ struct _handler_cons _temp586;
_push_handler(& _temp586);{ int _temp588= 0; if( setjmp( _temp586.handler)){
_temp588= 1;} if( ! _temp588){{ struct Cyc_Absyn_Enumdecl** _temp589=(( struct
Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
ge->enumdecls, v); struct Cyc_Absyn_Enumdecl* _temp590= Cyc_Tcdecl_merge_enumdecl(*
_temp589, ed, loc, Cyc_Tc_tc_msg); if( _temp590 == 0){ _npop_handler( 0u);
return;}* _temp589=( struct Cyc_Absyn_Enumdecl*) _check_null( _temp590);};
_pop_handler();} else{ void* _temp587=( void*) _exn_thrown; void* _temp592=
_temp587; _LL594: if( _temp592 == Cyc_Dict_Absent){ goto _LL595;} else{ goto
_LL596;} _LL596: goto _LL597; _LL595: { struct Cyc_Absyn_Enumdecl** _temp598=({
struct Cyc_Absyn_Enumdecl** _temp599=( struct Cyc_Absyn_Enumdecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl*)); _temp599[ 0]= ed; _temp599;}); ge->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Enumdecl** data)) Cyc_Dict_insert)( ge->enumdecls, v, _temp598);
goto _LL593;} _LL597:( void) _throw( _temp592); _LL593:;}}} if( ed->fields != 0){
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ed->fields))->v; for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ struct Cyc_Absyn_Enumfield* _temp600=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_List_List*) _check_null( fs))->hd; ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries,(* _temp600->name).f2,({ struct _tuple3* _temp601=( struct
_tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp601->f1=( void*)({ struct
Cyc_Tcenv_EnumRes_struct* _temp602=( struct Cyc_Tcenv_EnumRes_struct*) GC_malloc(
sizeof( struct Cyc_Tcenv_EnumRes_struct)); _temp602[ 0]=({ struct Cyc_Tcenv_EnumRes_struct
_temp603; _temp603.tag= Cyc_Tcenv_EnumRes; _temp603.f1= ed; _temp603.f2=
_temp600; _temp603;}); _temp602;}); _temp601->f2= 1; _temp601;}));}}} static int
Cyc_Tc_okay_externC( struct Cyc_Position_Segment* loc, void* sc){ void* _temp604=
sc; _LL606: if( _temp604 ==( void*) Cyc_Absyn_Static){ goto _LL607;} else{ goto
_LL608;} _LL608: if( _temp604 ==( void*) Cyc_Absyn_Abstract){ goto _LL609;}
else{ goto _LL610;} _LL610: if( _temp604 ==( void*) Cyc_Absyn_Public){ goto
_LL611;} else{ goto _LL612;} _LL612: if( _temp604 ==( void*) Cyc_Absyn_Extern){
goto _LL613;} else{ goto _LL614;} _LL614: if( _temp604 ==( void*) Cyc_Absyn_ExternC){
goto _LL615;} else{ goto _LL605;} _LL607:({ void* _temp616[ 0u]={}; Cyc_Tcutil_warn(
loc, _tag_arr("static declaration nested within extern \"C\"", sizeof(
unsigned char), 44u), _tag_arr( _temp616, sizeof( void*), 0u));}); return 0;
_LL609:({ void* _temp617[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("abstract declaration nested within extern \"C\"",
sizeof( unsigned char), 46u), _tag_arr( _temp617, sizeof( void*), 0u));});
return 0; _LL611: return 1; _LL613: return 1; _LL615:({ void* _temp618[ 0u]={};
Cyc_Tcutil_warn( loc, _tag_arr("nested extern \"C\" declaration", sizeof(
unsigned char), 30u), _tag_arr( _temp618, sizeof( void*), 0u));}); return 1;
_LL605:;} static void Cyc_Tc_tc_decls( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
ds0, int in_externC, int check_var_init){ struct Cyc_Tcenv_Genv* ge=(( struct
Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(
te->ae, te->ns); struct Cyc_List_List* _temp619= ds0; for( 0; _temp619 != 0;
_temp619=(( struct Cyc_List_List*) _check_null( _temp619))->tl){ struct Cyc_Absyn_Decl*
d=( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( _temp619))->hd;
struct Cyc_Position_Segment* loc= d->loc; void* _temp620=( void*) d->r; struct
Cyc_Absyn_Vardecl* _temp646; struct Cyc_Absyn_Fndecl* _temp648; struct Cyc_Absyn_Typedefdecl*
_temp650; struct Cyc_Absyn_Structdecl* _temp652; struct Cyc_Absyn_Uniondecl*
_temp654; struct Cyc_Absyn_Tuniondecl* _temp656; struct Cyc_Absyn_Enumdecl*
_temp658; struct Cyc_List_List* _temp660; struct _tagged_arr* _temp662; struct
Cyc_List_List* _temp664; struct _tuple0* _temp666; struct _tuple0 _temp668;
struct _tagged_arr* _temp669; void* _temp671; struct Cyc_List_List* _temp673;
_LL622: if(*(( int*) _temp620) == Cyc_Absyn_Let_d){ goto _LL623;} else{ goto
_LL624;} _LL624: if(*(( int*) _temp620) == Cyc_Absyn_Letv_d){ goto _LL625;}
else{ goto _LL626;} _LL626: if(*(( int*) _temp620) == Cyc_Absyn_Var_d){ _LL647:
_temp646=(( struct Cyc_Absyn_Var_d_struct*) _temp620)->f1; goto _LL627;} else{
goto _LL628;} _LL628: if(*(( int*) _temp620) == Cyc_Absyn_Fn_d){ _LL649:
_temp648=(( struct Cyc_Absyn_Fn_d_struct*) _temp620)->f1; goto _LL629;} else{
goto _LL630;} _LL630: if(*(( int*) _temp620) == Cyc_Absyn_Typedef_d){ _LL651:
_temp650=(( struct Cyc_Absyn_Typedef_d_struct*) _temp620)->f1; goto _LL631;}
else{ goto _LL632;} _LL632: if(*(( int*) _temp620) == Cyc_Absyn_Struct_d){
_LL653: _temp652=(( struct Cyc_Absyn_Struct_d_struct*) _temp620)->f1; goto
_LL633;} else{ goto _LL634;} _LL634: if(*(( int*) _temp620) == Cyc_Absyn_Union_d){
_LL655: _temp654=(( struct Cyc_Absyn_Union_d_struct*) _temp620)->f1; goto _LL635;}
else{ goto _LL636;} _LL636: if(*(( int*) _temp620) == Cyc_Absyn_Tunion_d){
_LL657: _temp656=(( struct Cyc_Absyn_Tunion_d_struct*) _temp620)->f1; goto
_LL637;} else{ goto _LL638;} _LL638: if(*(( int*) _temp620) == Cyc_Absyn_Enum_d){
_LL659: _temp658=(( struct Cyc_Absyn_Enum_d_struct*) _temp620)->f1; goto _LL639;}
else{ goto _LL640;} _LL640: if(*(( int*) _temp620) == Cyc_Absyn_Namespace_d){
_LL663: _temp662=(( struct Cyc_Absyn_Namespace_d_struct*) _temp620)->f1; goto
_LL661; _LL661: _temp660=(( struct Cyc_Absyn_Namespace_d_struct*) _temp620)->f2;
goto _LL641;} else{ goto _LL642;} _LL642: if(*(( int*) _temp620) == Cyc_Absyn_Using_d){
_LL667: _temp666=(( struct Cyc_Absyn_Using_d_struct*) _temp620)->f1; _temp668=*
_temp666; _LL672: _temp671= _temp668.f1; goto _LL670; _LL670: _temp669= _temp668.f2;
goto _LL665; _LL665: _temp664=(( struct Cyc_Absyn_Using_d_struct*) _temp620)->f2;
goto _LL643;} else{ goto _LL644;} _LL644: if(*(( int*) _temp620) == Cyc_Absyn_ExternC_d){
_LL674: _temp673=(( struct Cyc_Absyn_ExternC_d_struct*) _temp620)->f1; goto
_LL645;} else{ goto _LL621;} _LL623:({ void* _temp675[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("top level let-declarations are not implemented", sizeof(
unsigned char), 47u), _tag_arr( _temp675, sizeof( void*), 0u));}); goto _LL621;
_LL625:({ void* _temp676[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("top level let-declarations are not implemented",
sizeof( unsigned char), 47u), _tag_arr( _temp676, sizeof( void*), 0u));}); goto
_LL621; _LL627: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp646->sc):
0){( void*)( _temp646->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcVardecl(
te, ge, loc, _temp646, check_var_init); goto _LL621; _LL629: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp648->sc): 0){( void*)( _temp648->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcFndecl( te, ge, loc, _temp648); goto _LL621; _LL631: Cyc_Tc_tcTypedefdecl(
te, ge, loc, _temp650); goto _LL621; _LL633: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp652->sc): 0){( void*)( _temp652->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcStructdecl( te, ge, loc, _temp652); goto _LL621; _LL635: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp654->sc): 0){( void*)( _temp654->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcUniondecl( te, ge, loc, _temp654);
goto _LL621; _LL637: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*)
_temp656->sc): 0){( void*)( _temp656->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcTuniondecl( te, ge, loc, _temp656); goto _LL621; _LL639: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp658->sc): 0){( void*)( _temp658->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcEnumdecl( te, ge, loc, _temp658);
goto _LL621; _LL641: { struct Cyc_List_List* _temp677= te->ns; struct Cyc_List_List*
_temp678=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp677,({ struct Cyc_List_List* _temp679=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp679->hd=( void*) _temp662;
_temp679->tl= 0; _temp679;})); if( !(( int(*)( struct Cyc_Set_Set* s, struct
_tagged_arr* elt)) Cyc_Set_member)( ge->namespaces, _temp662)){ ge->namespaces=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)(
ge->namespaces, _temp662); te->ae=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key, struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)( te->ae,
_temp678, Cyc_Tcenv_empty_genv());} te->ns= _temp678; Cyc_Tc_tc_decls( te,
_temp660, in_externC, check_var_init); te->ns= _temp677; goto _LL621;} _LL643: {
struct _tagged_arr* first; struct Cyc_List_List* rest;{ void* _temp680= _temp671;
struct Cyc_List_List* _temp692; struct Cyc_List_List* _temp694; struct Cyc_List_List*
_temp696; struct Cyc_List_List _temp698; struct Cyc_List_List* _temp699; struct
_tagged_arr* _temp701; struct Cyc_List_List* _temp703; struct Cyc_List_List
_temp705; struct Cyc_List_List* _temp706; struct _tagged_arr* _temp708; _LL682:
if( _temp680 ==( void*) Cyc_Absyn_Loc_n){ goto _LL683;} else{ goto _LL684;}
_LL684: if(( unsigned int) _temp680 > 1u?*(( int*) _temp680) == Cyc_Absyn_Rel_n:
0){ _LL693: _temp692=(( struct Cyc_Absyn_Rel_n_struct*) _temp680)->f1; if(
_temp692 == 0){ goto _LL685;} else{ goto _LL686;}} else{ goto _LL686;} _LL686:
if(( unsigned int) _temp680 > 1u?*(( int*) _temp680) == Cyc_Absyn_Abs_n: 0){
_LL695: _temp694=(( struct Cyc_Absyn_Abs_n_struct*) _temp680)->f1; if( _temp694
== 0){ goto _LL687;} else{ goto _LL688;}} else{ goto _LL688;} _LL688: if((
unsigned int) _temp680 > 1u?*(( int*) _temp680) == Cyc_Absyn_Rel_n: 0){ _LL697:
_temp696=(( struct Cyc_Absyn_Rel_n_struct*) _temp680)->f1; if( _temp696 == 0){
goto _LL690;} else{ _temp698=* _temp696; _LL702: _temp701=( struct _tagged_arr*)
_temp698.hd; goto _LL700; _LL700: _temp699= _temp698.tl; goto _LL689;}} else{
goto _LL690;} _LL690: if(( unsigned int) _temp680 > 1u?*(( int*) _temp680) ==
Cyc_Absyn_Abs_n: 0){ _LL704: _temp703=(( struct Cyc_Absyn_Abs_n_struct*)
_temp680)->f1; if( _temp703 == 0){ goto _LL681;} else{ _temp705=* _temp703;
_LL709: _temp708=( struct _tagged_arr*) _temp705.hd; goto _LL707; _LL707:
_temp706= _temp705.tl; goto _LL691;}} else{ goto _LL681;} _LL683: goto _LL685;
_LL685: goto _LL687; _LL687: first= _temp669; rest= 0; goto _LL681; _LL689:
_temp708= _temp701; _temp706= _temp699; goto _LL691; _LL691: first= _temp708;
rest=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp706,({ struct Cyc_List_List* _temp710=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp710->hd=( void*) _temp669;
_temp710->tl= 0; _temp710;})); goto _LL681; _LL681:;}{ struct Cyc_List_List*
_temp711= Cyc_Tcenv_resolve_namespace( te, loc, first, rest); ge->availables=({
struct Cyc_List_List* _temp712=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp712->hd=( void*) _temp711; _temp712->tl= ge->availables;
_temp712;}); Cyc_Tc_tc_decls( te, _temp664, in_externC, check_var_init); ge->availables=((
struct Cyc_List_List*) _check_null( ge->availables))->tl; goto _LL621;}} _LL645:
Cyc_Tc_tc_decls( te, _temp673, 1, check_var_init); goto _LL621; _LL621:;}} void
Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int check_var_init, struct Cyc_List_List*
ds){ Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r); Cyc_Tc_tc_decls( te, ds,
0, check_var_init);} static int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env,
struct Cyc_Absyn_Decl* d); static struct Cyc_List_List* Cyc_Tc_treeshake_f(
struct Cyc_Dict_Dict* env, struct Cyc_List_List* ds){ return(( struct Cyc_List_List*(*)(
int(* f)( struct Cyc_Dict_Dict*, struct Cyc_Absyn_Decl*), struct Cyc_Dict_Dict*
env, struct Cyc_List_List* l)) Cyc_List_filter_c)( Cyc_Tc_vardecl_needed, env,
ds);} struct _tuple6{ struct Cyc_Tcenv_Genv* f1; struct Cyc_Set_Set* f2; } ;
static int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl*
d){ void* _temp713=( void*) d->r; struct Cyc_Absyn_Vardecl* _temp723; struct Cyc_List_List*
_temp725; struct Cyc_List_List** _temp727; struct Cyc_List_List* _temp728;
struct Cyc_List_List** _temp730; _LL715: if(*(( int*) _temp713) == Cyc_Absyn_Var_d){
_LL724: _temp723=(( struct Cyc_Absyn_Var_d_struct*) _temp713)->f1; goto _LL716;}
else{ goto _LL717;} _LL717: if(*(( int*) _temp713) == Cyc_Absyn_Using_d){ _LL726:
_temp725=(( struct Cyc_Absyn_Using_d_struct*) _temp713)->f2; _temp727=&(( struct
Cyc_Absyn_Using_d_struct*) _temp713)->f2; goto _LL718;} else{ goto _LL719;}
_LL719: if(*(( int*) _temp713) == Cyc_Absyn_Namespace_d){ _LL729: _temp728=((
struct Cyc_Absyn_Namespace_d_struct*) _temp713)->f2; _temp730=&(( struct Cyc_Absyn_Namespace_d_struct*)
_temp713)->f2; goto _LL720;} else{ goto _LL721;} _LL721: goto _LL722; _LL716:
if(( void*) _temp723->sc !=( void*) Cyc_Absyn_Extern){ return 1;}{ struct
_tuple0 _temp733; struct _tagged_arr* _temp734; void* _temp736; struct _tuple0*
_temp731= _temp723->name; _temp733=* _temp731; _LL737: _temp736= _temp733.f1;
goto _LL735; _LL735: _temp734= _temp733.f2; goto _LL732; _LL732: { struct Cyc_List_List*
ns;{ void* _temp738= _temp736; struct Cyc_List_List* _temp746; struct Cyc_List_List*
_temp748; _LL740: if( _temp738 ==( void*) Cyc_Absyn_Loc_n){ goto _LL741;} else{
goto _LL742;} _LL742: if(( unsigned int) _temp738 > 1u?*(( int*) _temp738) ==
Cyc_Absyn_Rel_n: 0){ _LL747: _temp746=(( struct Cyc_Absyn_Rel_n_struct*)
_temp738)->f1; goto _LL743;} else{ goto _LL744;} _LL744: if(( unsigned int)
_temp738 > 1u?*(( int*) _temp738) == Cyc_Absyn_Abs_n: 0){ _LL749: _temp748=((
struct Cyc_Absyn_Abs_n_struct*) _temp738)->f1; goto _LL745;} else{ goto _LL739;}
_LL741: ns= 0; goto _LL739; _LL743: ns= _temp746; goto _LL739; _LL745: ns=
_temp748; goto _LL739; _LL739:;}{ struct _tuple6* _temp750=(( struct _tuple6*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( env, ns);
struct Cyc_Tcenv_Genv* _temp751=(* _temp750).f1; int _temp752=(*(( struct
_tuple3*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
_temp751->ordinaries, _temp734)).f2; if( ! _temp752){(* _temp750).f2=(( struct
Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)((*
_temp750).f2, _temp734);} return _temp752;}}} _LL718: _temp730= _temp727; goto
_LL720; _LL720:* _temp730= Cyc_Tc_treeshake_f( env,* _temp730); return 1; _LL722:
return 1; _LL714:;} static int Cyc_Tc_treeshake_remove_f( struct Cyc_Set_Set*
set, struct _tagged_arr* x, void* y){ return !(( int(*)( struct Cyc_Set_Set* s,
struct _tagged_arr* elt)) Cyc_Set_member)( set, x);} static void Cyc_Tc_treeshake_remove(
struct Cyc_List_List* ns, struct _tuple6* nsenv){ struct Cyc_Tcenv_Genv*
_temp753=(* nsenv).f1; struct Cyc_Set_Set* _temp754=(* nsenv).f2; _temp753->ordinaries=((
struct Cyc_Dict_Dict*(*)( int(* f)( struct Cyc_Set_Set*, struct _tagged_arr*,
struct _tuple3*), struct Cyc_Set_Set* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_filter_c)((
int(*)( struct Cyc_Set_Set* set, struct _tagged_arr* x, struct _tuple3* y)) Cyc_Tc_treeshake_remove_f,
_temp754, _temp753->ordinaries);} static struct _tuple6* Cyc_Tc_treeshake_make_env_f(
struct Cyc_Tcenv_Genv* ge){ return({ struct _tuple6* _temp755=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp755->f1= ge; _temp755->f2=(( struct
Cyc_Set_Set*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Set_empty)(
Cyc_String_strptrcmp); _temp755;});} struct Cyc_List_List* Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds){ struct Cyc_Dict_Dict*
_temp756=(( struct Cyc_Dict_Dict*(*)( struct _tuple6*(* f)( struct Cyc_Tcenv_Genv*),
struct Cyc_Dict_Dict* d)) Cyc_Dict_map)( Cyc_Tc_treeshake_make_env_f, te->ae);
struct Cyc_List_List* _temp757= Cyc_Tc_treeshake_f( _temp756, ds);(( void(*)(
void(* f)( struct Cyc_List_List*, struct _tuple6*), struct Cyc_Dict_Dict* d))
Cyc_Dict_iter)( Cyc_Tc_treeshake_remove, _temp756); return _temp757;}
