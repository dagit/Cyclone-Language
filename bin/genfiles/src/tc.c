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
unsigned char*); struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ;
extern int Cyc_List_length( struct Cyc_List_List* x); extern unsigned char Cyc_List_List_empty[
15u]; extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List* y); extern
unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_List_mem( int(* compare)( void*,
void*), struct Cyc_List_List* l, void* x); extern struct Cyc_List_List* Cyc_List_filter_c(
int(* f)( void*, void*), void* env, struct Cyc_List_List* l); struct Cyc_Stdio___sFILE;
extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; static const int Cyc_Stdio_String_pa= 0; struct Cyc_Stdio_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_Int_pa= 1; struct
Cyc_Stdio_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Stdio_Double_pa=
2; struct Cyc_Stdio_Double_pa_struct{ int tag; double f1; } ; static const int
Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{ int tag; short*
f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
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
_tagged_arr Cyc_Tc_tc_msg_c={ _temp0, _temp0, _temp0 +  1u}; static struct
_tagged_arr* Cyc_Tc_tc_msg=( struct _tagged_arr*)& Cyc_Tc_tc_msg_c; static
struct Cyc_List_List* Cyc_Tc_transfer_fn_type_atts( void* t, struct Cyc_List_List*
atts){ void* _temp1= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp7;
struct Cyc_List_List* _temp9; struct Cyc_List_List** _temp11; _LL3: if((
unsigned int) _temp1 >  4u?*(( int*) _temp1) ==  Cyc_Absyn_FnType: 0){ _LL8:
_temp7=(( struct Cyc_Absyn_FnType_struct*) _temp1)->f1; _LL10: _temp9= _temp7.attributes;
_temp11=&((( struct Cyc_Absyn_FnType_struct*) _temp1)->f1).attributes; goto _LL4;}
else{ goto _LL5;} _LL5: goto _LL6; _LL4: { struct Cyc_List_List* _temp12= 0;
for( 0; atts !=  0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ if(
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
_temp26; _LL18: if(( unsigned int) _temp16 >  1u?*(( int*) _temp16) ==  Cyc_Absyn_Rel_n:
0){ _LL25: _temp24=(( struct Cyc_Absyn_Rel_n_struct*) _temp16)->f1; if( _temp24
==  0){ goto _LL19;} else{ goto _LL20;}} else{ goto _LL20;} _LL20: if((
unsigned int) _temp16 >  1u?*(( int*) _temp16) ==  Cyc_Absyn_Abs_n: 0){ _LL27:
_temp26=(( struct Cyc_Absyn_Abs_n_struct*) _temp16)->f1; if( _temp26 ==  0){
goto _LL21;} else{ goto _LL22;}} else{ goto _LL22;} _LL22: goto _LL23; _LL19:
goto _LL17; _LL21: goto _LL17; _LL23:({ void* _temp28[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("qualified declarations are not implemented", sizeof(
unsigned char), 43u), _tag_arr( _temp28, sizeof( void*), 0u));}); return; _LL17:;}(*
vd->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp29=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp29[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp30; _temp30.tag= Cyc_Absyn_Abs_n; _temp30.f1= te->ns; _temp30;}); _temp29;});{
void* _temp31= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp* _temp38; struct
Cyc_Absyn_Tqual _temp40; void* _temp42; _LL33: if(( unsigned int) _temp31 >  4u?*((
int*) _temp31) ==  Cyc_Absyn_ArrayType: 0){ _LL43: _temp42=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp31)->f1; goto _LL41; _LL41: _temp40=(( struct Cyc_Absyn_ArrayType_struct*)
_temp31)->f2; goto _LL39; _LL39: _temp38=(( struct Cyc_Absyn_ArrayType_struct*)
_temp31)->f3; if( _temp38 ==  0){ goto _LL37;} else{ goto _LL35;}} else{ goto
_LL35;} _LL37: if( vd->initializer !=  0){ goto _LL34;} else{ goto _LL35;} _LL35:
goto _LL36; _LL34:{ void* _temp44=( void*)(( struct Cyc_Absyn_Exp*) _check_null(
vd->initializer))->r; void* _temp56; struct _tagged_arr _temp58; struct Cyc_Absyn_Exp*
_temp60; struct Cyc_List_List* _temp62; struct Cyc_List_List* _temp64; _LL46:
if(*(( int*) _temp44) ==  Cyc_Absyn_Const_e){ _LL57: _temp56=( void*)(( struct
Cyc_Absyn_Const_e_struct*) _temp44)->f1; if(( unsigned int) _temp56 >  1u?*((
int*) _temp56) ==  Cyc_Absyn_String_c: 0){ _LL59: _temp58=(( struct Cyc_Absyn_String_c_struct*)
_temp56)->f1; goto _LL47;} else{ goto _LL48;}} else{ goto _LL48;} _LL48: if(*((
int*) _temp44) ==  Cyc_Absyn_Comprehension_e){ _LL61: _temp60=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp44)->f2; goto _LL49;} else{ goto _LL50;} _LL50: if(*(( int*) _temp44) == 
Cyc_Absyn_UnresolvedMem_e){ _LL63: _temp62=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp44)->f2; goto _LL51;} else{ goto _LL52;} _LL52: if(*(( int*) _temp44) == 
Cyc_Absyn_Array_e){ _LL65: _temp64=(( struct Cyc_Absyn_Array_e_struct*) _temp44)->f1;
goto _LL53;} else{ goto _LL54;} _LL54: goto _LL55; _LL47: t=( void*)( vd->type=(
void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp66=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp66[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp67; _temp67.tag= Cyc_Absyn_ArrayType; _temp67.f1=(
void*) _temp42; _temp67.f2= _temp40; _temp67.f3=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
_get_arr_size( _temp58, sizeof( unsigned char)), 0); _temp67;}); _temp66;})));
goto _LL45; _LL49: t=( void*)( vd->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp68=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp68[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp69; _temp69.tag= Cyc_Absyn_ArrayType;
_temp69.f1=( void*) _temp42; _temp69.f2= _temp40; _temp69.f3=( struct Cyc_Absyn_Exp*)
_temp60; _temp69;}); _temp68;}))); goto _LL45; _LL51: _temp64= _temp62; goto
_LL53; _LL53: t=( void*)( vd->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp70=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp70[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp71; _temp71.tag= Cyc_Absyn_ArrayType;
_temp71.f1=( void*) _temp42; _temp71.f2= _temp40; _temp71.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp64), 0); _temp71;}); _temp70;}))); goto _LL45; _LL55: goto _LL45; _LL45:;}
goto _LL32; _LL36: goto _LL32; _LL32:;} Cyc_Tcutil_check_valid_toplevel_type(
loc, te, t); if( Cyc_Tcutil_is_function_type( t)){ atts= Cyc_Tc_transfer_fn_type_atts(
t, atts);} if( sc == ( void*) Cyc_Absyn_Extern? 1: sc == ( void*) Cyc_Absyn_ExternC){
if( vd->initializer !=  0){({ void* _temp72[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("extern declaration should not have initializer", sizeof( unsigned char),
47u), _tag_arr( _temp72, sizeof( void*), 0u));});}} else{ if( ! Cyc_Tcutil_is_function_type(
t)){ for( 0; atts !=  0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){
void* _temp73=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd; _LL75:
if(( unsigned int) _temp73 >  16u?*(( int*) _temp73) ==  Cyc_Absyn_Aligned_att:
0){ goto _LL76;} else{ goto _LL77;} _LL77: if(( unsigned int) _temp73 >  16u?*((
int*) _temp73) ==  Cyc_Absyn_Section_att: 0){ goto _LL78;} else{ goto _LL79;}
_LL79: if( _temp73 == ( void*) Cyc_Absyn_Nocommon_att){ goto _LL80;} else{ goto
_LL81;} _LL81: if( _temp73 == ( void*) Cyc_Absyn_Shared_att){ goto _LL82;} else{
goto _LL83;} _LL83: if( _temp73 == ( void*) Cyc_Absyn_Unused_att){ goto _LL84;}
else{ goto _LL85;} _LL85: if( _temp73 == ( void*) Cyc_Absyn_Weak_att){ goto
_LL86;} else{ goto _LL87;} _LL87: if( _temp73 == ( void*) Cyc_Absyn_Dllimport_att){
goto _LL88;} else{ goto _LL89;} _LL89: if( _temp73 == ( void*) Cyc_Absyn_Dllexport_att){
goto _LL90;} else{ goto _LL91;} _LL91: goto _LL92; _LL76: continue; _LL78:
continue; _LL80: continue; _LL82: continue; _LL84: continue; _LL86: continue;
_LL88: continue; _LL90: continue; _LL92:({ struct Cyc_Stdio_String_pa_struct
_temp95; _temp95.tag= Cyc_Stdio_String_pa; _temp95.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
vd->name);{ struct Cyc_Stdio_String_pa_struct _temp94; _temp94.tag= Cyc_Stdio_String_pa;
_temp94.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*)(( struct
Cyc_List_List*) _check_null( atts))->hd);{ void* _temp93[ 2u]={& _temp94,&
_temp95}; Cyc_Tcutil_terr( loc, _tag_arr("bad attribute %s for variable %s",
sizeof( unsigned char), 33u), _tag_arr( _temp93, sizeof( void*), 2u));}}}); goto
_LL74; _LL74:;} if( vd->initializer ==  0){ if( check_var_init? ! Cyc_Tcutil_supports_default(
t): 0){({ struct Cyc_Stdio_String_pa_struct _temp98; _temp98.tag= Cyc_Stdio_String_pa;
_temp98.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ struct Cyc_Stdio_String_pa_struct
_temp97; _temp97.tag= Cyc_Stdio_String_pa; _temp97.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
vd->name);{ void* _temp96[ 2u]={& _temp97,& _temp98}; Cyc_Tcutil_terr( loc,
_tag_arr("initializer required for variable %s of type %s", sizeof(
unsigned char), 48u), _tag_arr( _temp96, sizeof( void*), 2u));}}});}} else{
struct Cyc_Absyn_Exp* _temp99=( struct Cyc_Absyn_Exp*) _check_null( vd->initializer);
void* _temp100= Cyc_Tcexp_tcExpInitializer( te,( void**)& t, _temp99); if( ! Cyc_Tcutil_coerce_assign(
te, _temp99, t)){({ struct Cyc_Stdio_String_pa_struct _temp104; _temp104.tag=
Cyc_Stdio_String_pa; _temp104.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp100);{ struct Cyc_Stdio_String_pa_struct _temp103; _temp103.tag= Cyc_Stdio_String_pa;
_temp103.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ struct Cyc_Stdio_String_pa_struct
_temp102; _temp102.tag= Cyc_Stdio_String_pa; _temp102.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( vd->name);{ void* _temp101[ 3u]={& _temp102,& _temp103,&
_temp104}; Cyc_Tcutil_terr( loc, _tag_arr("%s is declared with type %s but initialized with type %s",
sizeof( unsigned char), 57u), _tag_arr( _temp101, sizeof( void*), 3u));}}}});}
if( ! Cyc_Tcutil_is_const_exp( te, _temp99)){({ void* _temp105[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("initializer is not a constant expression", sizeof( unsigned char),
41u), _tag_arr( _temp105, sizeof( void*), 0u));});}}} else{ for( 0; atts !=  0;
atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ void* _temp106=( void*)((
struct Cyc_List_List*) _check_null( atts))->hd; _LL108: if(( unsigned int)
_temp106 >  16u?*(( int*) _temp106) ==  Cyc_Absyn_Regparm_att: 0){ goto _LL109;}
else{ goto _LL110;} _LL110: if( _temp106 == ( void*) Cyc_Absyn_Stdcall_att){
goto _LL111;} else{ goto _LL112;} _LL112: if( _temp106 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL113;} else{ goto _LL114;} _LL114: if( _temp106 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL115;} else{ goto _LL116;} _LL116: if( _temp106 == ( void*) Cyc_Absyn_Noreturn_att){
goto _LL117;} else{ goto _LL118;} _LL118: if(( unsigned int) _temp106 >  16u?*((
int*) _temp106) ==  Cyc_Absyn_Format_att: 0){ goto _LL119;} else{ goto _LL120;}
_LL120: if( _temp106 == ( void*) Cyc_Absyn_Const_att){ goto _LL121;} else{ goto
_LL122;} _LL122: if(( unsigned int) _temp106 >  16u?*(( int*) _temp106) ==  Cyc_Absyn_Aligned_att:
0){ goto _LL123;} else{ goto _LL124;} _LL124: if( _temp106 == ( void*) Cyc_Absyn_Packed_att){
goto _LL125;} else{ goto _LL126;} _LL126: goto _LL127; _LL109: goto _LL111;
_LL111: goto _LL113; _LL113: goto _LL115; _LL115: goto _LL117; _LL117: goto
_LL119; _LL119: goto _LL121; _LL121:({ void* _temp128[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("tcVardecl: fn type attributes in function var decl",
sizeof( unsigned char), 51u), _tag_arr( _temp128, sizeof( void*), 0u));}); goto
_LL107; _LL123: goto _LL125; _LL125:({ struct Cyc_Stdio_String_pa_struct
_temp130; _temp130.tag= Cyc_Stdio_String_pa; _temp130.f1=( struct _tagged_arr)
Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*) _check_null( atts))->hd);{
void* _temp129[ 1u]={& _temp130}; Cyc_Tcutil_terr( loc, _tag_arr("bad attribute %s in function declaration",
sizeof( unsigned char), 41u), _tag_arr( _temp129, sizeof( void*), 1u));}}); goto
_LL107; _LL127: continue; _LL107:;}}}{ struct _handler_cons _temp131;
_push_handler(& _temp131);{ int _temp133= 0; if( setjmp( _temp131.handler)){
_temp133= 1;} if( ! _temp133){{ struct _tuple3* _temp134=(( struct _tuple3*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( ge->ordinaries,
v); void* _temp135=(* _temp134).f1; void* _temp145; _LL137: if(*(( int*)
_temp135) ==  Cyc_Tcenv_VarRes){ _LL146: _temp145=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp135)->f1; goto _LL138;} else{ goto _LL139;} _LL139: if(*(( int*) _temp135)
==  Cyc_Tcenv_StructRes){ goto _LL140;} else{ goto _LL141;} _LL141: if(*(( int*)
_temp135) ==  Cyc_Tcenv_TunionRes){ goto _LL142;} else{ goto _LL143;} _LL143:
if(*(( int*) _temp135) ==  Cyc_Tcenv_EnumRes){ goto _LL144;} else{ goto _LL136;}
_LL138: { struct Cyc_Absyn_Global_b_struct* _temp147=({ struct Cyc_Absyn_Global_b_struct*
_temp152=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp152[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp153; _temp153.tag= Cyc_Absyn_Global_b;
_temp153.f1= vd; _temp153;}); _temp152;}); void* _temp148= Cyc_Tcdecl_merge_binding(
_temp145,( void*) _temp147, loc, Cyc_Tc_tc_msg); if( _temp148 == ( void*) Cyc_Absyn_Unresolved_b){
goto _LL136;} if( _temp148 ==  _temp145?(* _temp134).f2: 0){ goto _LL136;} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple3*
_temp149=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp149->f1=(
void*)({ struct Cyc_Tcenv_VarRes_struct* _temp150=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp150[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp151; _temp151.tag= Cyc_Tcenv_VarRes; _temp151.f1=( void*) _temp148;
_temp151;}); _temp150;}); _temp149->f2= 1; _temp149;})); goto _LL136;} _LL140:({
void* _temp154[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("variable declaration shadows previous struct declaration",
sizeof( unsigned char), 57u), _tag_arr( _temp154, sizeof( void*), 0u));}); goto
_LL136; _LL142:({ void* _temp155[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("variable declaration shadows previous [x]tunion constructor",
sizeof( unsigned char), 60u), _tag_arr( _temp155, sizeof( void*), 0u));}); goto
_LL136; _LL144:({ void* _temp156[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("variable declaration shadows previous enum tag",
sizeof( unsigned char), 47u), _tag_arr( _temp156, sizeof( void*), 0u));}); goto
_LL136; _LL136:;}; _pop_handler();} else{ void* _temp132=( void*) _exn_thrown;
void* _temp158= _temp132; _LL160: if( _temp158 ==  Cyc_Dict_Absent){ goto _LL161;}
else{ goto _LL162;} _LL162: goto _LL163; _LL161: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple3* _temp164=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp164->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp165=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp165[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp166; _temp166.tag= Cyc_Tcenv_VarRes;
_temp166.f1=( void*)(( void*)({ struct Cyc_Absyn_Global_b_struct* _temp167=(
struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp167[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp168; _temp168.tag= Cyc_Absyn_Global_b;
_temp168.f1= vd; _temp168;}); _temp167;})); _temp166;}); _temp165;}); _temp164->f2=
0; _temp164;})); goto _LL159; _LL163:( void) _throw( _temp158); _LL159:;}}}}
static void Cyc_Tc_tcFndecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl* fd){ struct
_tagged_arr* v=(* fd->name).f2; void* sc=( void*) fd->sc;{ void* _temp169=(* fd->name).f1;
struct Cyc_List_List* _temp177; struct Cyc_List_List* _temp179; _LL171: if((
unsigned int) _temp169 >  1u?*(( int*) _temp169) ==  Cyc_Absyn_Rel_n: 0){ _LL178:
_temp177=(( struct Cyc_Absyn_Rel_n_struct*) _temp169)->f1; if( _temp177 ==  0){
goto _LL172;} else{ goto _LL173;}} else{ goto _LL173;} _LL173: if(( unsigned int)
_temp169 >  1u?*(( int*) _temp169) ==  Cyc_Absyn_Abs_n: 0){ _LL180: _temp179=((
struct Cyc_Absyn_Abs_n_struct*) _temp169)->f1; goto _LL174;} else{ goto _LL175;}
_LL175: goto _LL176; _LL172: goto _LL170; _LL174:( int) _throw(( void*)({ struct
Cyc_Core_Impossible_struct* _temp181=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp181[ 0]=({ struct
Cyc_Core_Impossible_struct _temp182; _temp182.tag= Cyc_Core_Impossible; _temp182.f1=
_tag_arr("tc: Abs_n in tcFndecl", sizeof( unsigned char), 22u); _temp182;});
_temp181;})); _LL176:({ void* _temp183[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("qualified declarations are not implemented",
sizeof( unsigned char), 43u), _tag_arr( _temp183, sizeof( void*), 0u));});
return; _LL170:;}(* fd->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp184=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp184[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp185; _temp185.tag= Cyc_Absyn_Abs_n;
_temp185.f1= te->ns; _temp185;}); _temp184;}); Cyc_Tcutil_check_fndecl_valid_type(
loc, te, fd);{ void* t= Cyc_Tcutil_fndecl2typ( fd); fd->attributes= Cyc_Tc_transfer_fn_type_atts(
t, fd->attributes);{ struct Cyc_List_List* atts= fd->attributes; for( 0; atts != 
0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ void* _temp186=(
void*)(( struct Cyc_List_List*) _check_null( atts))->hd; _LL188: if( _temp186 == (
void*) Cyc_Absyn_Packed_att){ goto _LL189;} else{ goto _LL190;} _LL190: if((
unsigned int) _temp186 >  16u?*(( int*) _temp186) ==  Cyc_Absyn_Aligned_att: 0){
goto _LL191;} else{ goto _LL192;} _LL192: goto _LL193; _LL189: goto _LL191;
_LL191:({ struct Cyc_Stdio_String_pa_struct _temp195; _temp195.tag= Cyc_Stdio_String_pa;
_temp195.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*)(( struct
Cyc_List_List*) _check_null( atts))->hd);{ void* _temp194[ 1u]={& _temp195}; Cyc_Tcutil_terr(
loc, _tag_arr("bad attribute %s for function", sizeof( unsigned char), 30u),
_tag_arr( _temp194, sizeof( void*), 1u));}}); goto _LL187; _LL193: goto _LL187;
_LL187:;}}{ struct _handler_cons _temp196; _push_handler(& _temp196);{ int
_temp198= 0; if( setjmp( _temp196.handler)){ _temp198= 1;} if( ! _temp198){{
struct _tuple3* _temp199=(( struct _tuple3*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup)( ge->ordinaries, v); void* _temp200=(*
_temp199).f1; void* _temp210; _LL202: if(*(( int*) _temp200) ==  Cyc_Tcenv_VarRes){
_LL211: _temp210=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp200)->f1; goto
_LL203;} else{ goto _LL204;} _LL204: if(*(( int*) _temp200) ==  Cyc_Tcenv_StructRes){
goto _LL205;} else{ goto _LL206;} _LL206: if(*(( int*) _temp200) ==  Cyc_Tcenv_TunionRes){
goto _LL207;} else{ goto _LL208;} _LL208: if(*(( int*) _temp200) ==  Cyc_Tcenv_EnumRes){
goto _LL209;} else{ goto _LL201;} _LL203: { struct Cyc_Absyn_Funname_b_struct*
_temp212=({ struct Cyc_Absyn_Funname_b_struct* _temp217=( struct Cyc_Absyn_Funname_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct)); _temp217[ 0]=({ struct
Cyc_Absyn_Funname_b_struct _temp218; _temp218.tag= Cyc_Absyn_Funname_b; _temp218.f1=
fd; _temp218;}); _temp217;}); void* _temp213= Cyc_Tcdecl_merge_binding( _temp210,(
void*) _temp212, loc, Cyc_Tc_tc_msg); if( _temp213 == ( void*) Cyc_Absyn_Unresolved_b){
goto _LL201;} if( _temp213 ==  _temp210?(* _temp199).f2: 0){ goto _LL201;} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple3*
_temp214=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp214->f1=(
void*)({ struct Cyc_Tcenv_VarRes_struct* _temp215=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp215[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp216; _temp216.tag= Cyc_Tcenv_VarRes; _temp216.f1=( void*) _temp213;
_temp216;}); _temp215;}); _temp214->f2= 1; _temp214;})); goto _LL201;} _LL205:({
void* _temp219[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("function declaration shadows previous struct declaration",
sizeof( unsigned char), 57u), _tag_arr( _temp219, sizeof( void*), 0u));}); goto
_LL201; _LL207:({ void* _temp220[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("function declaration shadows previous [x]tunion constructor",
sizeof( unsigned char), 60u), _tag_arr( _temp220, sizeof( void*), 0u));}); goto
_LL201; _LL209:({ void* _temp221[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("function declaration shadows previous enum tag",
sizeof( unsigned char), 47u), _tag_arr( _temp221, sizeof( void*), 0u));}); goto
_LL201; _LL201:;}; _pop_handler();} else{ void* _temp197=( void*) _exn_thrown;
void* _temp223= _temp197; _LL225: if( _temp223 ==  Cyc_Dict_Absent){ goto _LL226;}
else{ goto _LL227;} _LL227: goto _LL228; _LL226: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple3* _temp229=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp229->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp230=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp230[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp231; _temp231.tag= Cyc_Tcenv_VarRes;
_temp231.f1=( void*)(( void*)({ struct Cyc_Absyn_Funname_b_struct* _temp232=(
struct Cyc_Absyn_Funname_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct));
_temp232[ 0]=({ struct Cyc_Absyn_Funname_b_struct _temp233; _temp233.tag= Cyc_Absyn_Funname_b;
_temp233.f1= fd; _temp233;}); _temp232;})); _temp231;}); _temp230;}); _temp229->f2=
0; _temp229;})); goto _LL224; _LL228:( void) _throw( _temp223); _LL224:;}}} te->le=({
struct Cyc_Core_Opt* _temp234=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp234->v=( void*)(( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp235=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp235[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp236; _temp236.tag= Cyc_Tcenv_Outermost;
_temp236.f1=( void*) Cyc_Tcenv_new_fenv( loc, fd); _temp236;}); _temp235;}));
_temp234;}); Cyc_Tcstmt_tcStmt( te, fd->body, 0); if( ! Cyc_Tcenv_all_labels_resolved(
te)){({ void* _temp237[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("function has goto statements to undefined labels",
sizeof( unsigned char), 49u), _tag_arr( _temp237, sizeof( void*), 0u));});} te->le=
0;}} static void Cyc_Tc_tcTypedefdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Typedefdecl* td){ struct
_tagged_arr* v=(* td->name).f2;{ void* _temp238=(* td->name).f1; struct Cyc_List_List*
_temp246; struct Cyc_List_List* _temp248; _LL240: if(( unsigned int) _temp238 > 
1u?*(( int*) _temp238) ==  Cyc_Absyn_Rel_n: 0){ _LL247: _temp246=(( struct Cyc_Absyn_Rel_n_struct*)
_temp238)->f1; if( _temp246 ==  0){ goto _LL241;} else{ goto _LL242;}} else{
goto _LL242;} _LL242: if(( unsigned int) _temp238 >  1u?*(( int*) _temp238) == 
Cyc_Absyn_Abs_n: 0){ _LL249: _temp248=(( struct Cyc_Absyn_Abs_n_struct*)
_temp238)->f1; if( _temp248 ==  0){ goto _LL243;} else{ goto _LL244;}} else{
goto _LL244;} _LL244: goto _LL245; _LL241: goto _LL239; _LL243: goto _LL239;
_LL245:({ void* _temp250[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("qualified declarations are not implemented",
sizeof( unsigned char), 43u), _tag_arr( _temp250, sizeof( void*), 0u));});
return; _LL239:;} if((( int(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key))
Cyc_Dict_member)( ge->typedefs, v)){({ struct Cyc_Stdio_String_pa_struct
_temp252; _temp252.tag= Cyc_Stdio_String_pa; _temp252.f1=( struct _tagged_arr)*
v;{ void* _temp251[ 1u]={& _temp252}; Cyc_Tcutil_terr( loc, _tag_arr("redeclaration of typedef %s",
sizeof( unsigned char), 28u), _tag_arr( _temp251, sizeof( void*), 1u));}});
return;}(* td->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp253=(
struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp253[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp254; _temp254.tag= Cyc_Absyn_Abs_n;
_temp254.f1= te->ns; _temp254;}); _temp253;}); Cyc_Tcutil_check_unique_tvars(
loc, td->tvs); Cyc_Tcutil_add_tvar_identities( td->tvs); Cyc_Tcutil_check_type(
loc, te, td->tvs,( void*) Cyc_Absyn_AnyKind, 0,( void*) td->defn); ge->typedefs=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Typedefdecl* data)) Cyc_Dict_insert)( ge->typedefs, v, td);}
static void Cyc_Tc_tcStructFields( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct _tagged_arr obj, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs){ struct _RegionHandle _temp255= _new_region();
struct _RegionHandle* uprev_rgn=& _temp255; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0; struct Cyc_List_List* _temp256= fields; for( 0;
_temp256 !=  0; _temp256=(( struct Cyc_List_List*) _check_null( _temp256))->tl){
struct Cyc_Absyn_Structfield _temp259; struct Cyc_List_List* _temp260; struct
Cyc_Absyn_Exp* _temp262; void* _temp264; struct Cyc_Absyn_Tqual _temp266; struct
_tagged_arr* _temp268; struct Cyc_Absyn_Structfield* _temp257=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp256))->hd; _temp259=* _temp257; _LL269:
_temp268= _temp259.name; goto _LL267; _LL267: _temp266= _temp259.tq; goto _LL265;
_LL265: _temp264=( void*) _temp259.type; goto _LL263; _LL263: _temp262= _temp259.width;
goto _LL261; _LL261: _temp260= _temp259.attributes; goto _LL258; _LL258: if(((
int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List*
l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,
_temp268)){({ struct Cyc_Stdio_String_pa_struct _temp272; _temp272.tag= Cyc_Stdio_String_pa;
_temp272.f1=( struct _tagged_arr) obj;{ struct Cyc_Stdio_String_pa_struct
_temp271; _temp271.tag= Cyc_Stdio_String_pa; _temp271.f1=( struct _tagged_arr)*
_temp268;{ void* _temp270[ 2u]={& _temp271,& _temp272}; Cyc_Tcutil_terr( loc,
_tag_arr("duplicate field %s in %s", sizeof( unsigned char), 25u), _tag_arr(
_temp270, sizeof( void*), 2u));}}});} if( Cyc_String_strcmp(* _temp268, _tag_arr("",
sizeof( unsigned char), 1u)) !=  0){ prev_fields=({ struct Cyc_List_List*
_temp273=( struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp273->hd=( void*) _temp268; _temp273->tl= prev_fields; _temp273;});} Cyc_Tcutil_check_type(
loc, te, tvs,( void*) Cyc_Absyn_MemKind, 0, _temp264); Cyc_Tcutil_check_bitfield(
loc, te, _temp264, _temp262, _temp268);}}; _pop_region( uprev_rgn);} static void
Cyc_Tc_tcStructOrUniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct _tagged_arr obj, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl*
sd){ struct _tagged_arr* _temp274=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v)).f2; if( sd->name ==  0){({ struct Cyc_Stdio_String_pa_struct
_temp276; _temp276.tag= Cyc_Stdio_String_pa; _temp276.f1=( struct _tagged_arr)
obj;{ void* _temp275[ 1u]={& _temp276}; Cyc_Tcutil_terr( loc, _tag_arr("anonymous %ss are not allowed at top level",
sizeof( unsigned char), 43u), _tag_arr( _temp275, sizeof( void*), 1u));}});
return;}{ struct Cyc_List_List* atts= sd->attributes; for( 0; atts !=  0; atts=((
struct Cyc_List_List*) _check_null( atts))->tl){ void* _temp277=( void*)((
struct Cyc_List_List*) _check_null( atts))->hd; _LL279: if( _temp277 == ( void*)
Cyc_Absyn_Packed_att){ goto _LL280;} else{ goto _LL281;} _LL281: if((
unsigned int) _temp277 >  16u?*(( int*) _temp277) ==  Cyc_Absyn_Aligned_att: 0){
goto _LL282;} else{ goto _LL283;} _LL283: goto _LL284; _LL280: continue; _LL282:
continue; _LL284:({ struct Cyc_Stdio_String_pa_struct _temp288; _temp288.tag=
Cyc_Stdio_String_pa; _temp288.f1=( struct _tagged_arr)* _temp274;{ struct Cyc_Stdio_String_pa_struct
_temp287; _temp287.tag= Cyc_Stdio_String_pa; _temp287.f1=( struct _tagged_arr)
obj;{ struct Cyc_Stdio_String_pa_struct _temp286; _temp286.tag= Cyc_Stdio_String_pa;
_temp286.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*)(( struct
Cyc_List_List*) _check_null( atts))->hd);{ void* _temp285[ 3u]={& _temp286,&
_temp287,& _temp288}; Cyc_Tcutil_terr( loc, _tag_arr("bad attribute %s in %s %s definition",
sizeof( unsigned char), 37u), _tag_arr( _temp285, sizeof( void*), 3u));}}}});
goto _LL278; _LL278:;}}{ struct Cyc_List_List* tvs= sd->tvs; for( 0; tvs !=  0;
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ struct Cyc_Absyn_Conref*
_temp289= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd)->kind); void* _temp290=( void*) _temp289->v; void*
_temp298; _LL292: if( _temp290 == ( void*) Cyc_Absyn_No_constr){ goto _LL293;}
else{ goto _LL294;} _LL294: if(( unsigned int) _temp290 >  1u?*(( int*) _temp290)
==  Cyc_Absyn_Eq_constr: 0){ _LL299: _temp298=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp290)->f1; if( _temp298 == ( void*) Cyc_Absyn_MemKind){ goto _LL295;} else{
goto _LL296;}} else{ goto _LL296;} _LL296: goto _LL297; _LL293:( void*)(
_temp289->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp300=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp300[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp301; _temp301.tag= Cyc_Absyn_Eq_constr;
_temp301.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp301;}); _temp300;})));
continue; _LL295:({ struct Cyc_Stdio_String_pa_struct _temp305; _temp305.tag=
Cyc_Stdio_String_pa; _temp305.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->name;{ struct Cyc_Stdio_String_pa_struct
_temp304; _temp304.tag= Cyc_Stdio_String_pa; _temp304.f1=( struct _tagged_arr)*
_temp274;{ struct Cyc_Stdio_String_pa_struct _temp303; _temp303.tag= Cyc_Stdio_String_pa;
_temp303.f1=( struct _tagged_arr) obj;{ void* _temp302[ 3u]={& _temp303,&
_temp304,& _temp305}; Cyc_Tcutil_terr( loc, _tag_arr("%s %s attempts to abstract type variable %s of kind M",
sizeof( unsigned char), 54u), _tag_arr( _temp302, sizeof( void*), 3u));}}}});
continue; _LL297: continue; _LL291:;}}{ void* _temp306=(*(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( sd->name))->v)).f1; struct Cyc_List_List*
_temp314; struct Cyc_List_List* _temp316; _LL308: if(( unsigned int) _temp306 > 
1u?*(( int*) _temp306) ==  Cyc_Absyn_Rel_n: 0){ _LL315: _temp314=(( struct Cyc_Absyn_Rel_n_struct*)
_temp306)->f1; if( _temp314 ==  0){ goto _LL309;} else{ goto _LL310;}} else{
goto _LL310;} _LL310: if(( unsigned int) _temp306 >  1u?*(( int*) _temp306) == 
Cyc_Absyn_Abs_n: 0){ _LL317: _temp316=(( struct Cyc_Absyn_Abs_n_struct*)
_temp306)->f1; if( _temp316 ==  0){ goto _LL311;} else{ goto _LL312;}} else{
goto _LL312;} _LL312: goto _LL313; _LL309: goto _LL307; _LL311: goto _LL307;
_LL313:({ void* _temp318[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("qualified declarations are not implemented",
sizeof( unsigned char), 43u), _tag_arr( _temp318, sizeof( void*), 0u));});
return; _LL307:;}(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp319=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp319[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp320; _temp320.tag= Cyc_Absyn_Abs_n; _temp320.f1= te->ns; _temp320;});
_temp319;}); Cyc_Tcutil_check_unique_tvars( loc, sd->tvs); Cyc_Tcutil_add_tvar_identities(
sd->tvs);} struct _tuple4{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ;
static void Cyc_Tc_tcStructdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl* sd){ struct
_tagged_arr* _temp321=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
sd->name))->v)).f2; const unsigned char* _temp322="struct"; Cyc_Tc_tcStructOrUniondecl(
te, ge, _tag_arr( _temp322, sizeof( unsigned char), 7u), loc, sd);{ struct Cyc_List_List*
_temp323= sd->tvs;{ struct _tuple4 _temp325=({ struct _tuple4 _temp324; _temp324.f1=
sd->fields; _temp324.f2=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup_opt)( ge->structdecls, _temp321);
_temp324;}); struct Cyc_Core_Opt* _temp335; struct Cyc_Core_Opt* _temp337;
struct Cyc_Core_Opt* _temp339; struct Cyc_Core_Opt* _temp341; struct Cyc_Core_Opt
_temp343; struct Cyc_List_List* _temp344; struct Cyc_Core_Opt* _temp346; struct
Cyc_Core_Opt _temp348; struct Cyc_Absyn_Structdecl** _temp349; struct Cyc_Core_Opt*
_temp351; struct Cyc_Core_Opt _temp353; struct Cyc_List_List* _temp354; struct
Cyc_Core_Opt* _temp356; struct Cyc_Core_Opt _temp358; struct Cyc_Absyn_Structdecl**
_temp359; struct Cyc_Core_Opt* _temp361; _LL327: _LL338: _temp337= _temp325.f1;
if( _temp337 ==  0){ goto _LL336;} else{ goto _LL329;} _LL336: _temp335=
_temp325.f2; if( _temp335 ==  0){ goto _LL328;} else{ goto _LL329;} _LL329:
_LL342: _temp341= _temp325.f1; if( _temp341 ==  0){ goto _LL331;} else{ _temp343=*
_temp341; _LL345: _temp344=( struct Cyc_List_List*) _temp343.v; goto _LL340;}
_LL340: _temp339= _temp325.f2; if( _temp339 ==  0){ goto _LL330;} else{ goto
_LL331;} _LL331: _LL352: _temp351= _temp325.f1; if( _temp351 ==  0){ goto _LL333;}
else{ _temp353=* _temp351; _LL355: _temp354=( struct Cyc_List_List*) _temp353.v;
goto _LL347;} _LL347: _temp346= _temp325.f2; if( _temp346 ==  0){ goto _LL333;}
else{ _temp348=* _temp346; _LL350: _temp349=( struct Cyc_Absyn_Structdecl**)
_temp348.v; goto _LL332;} _LL333: _LL362: _temp361= _temp325.f1; if( _temp361 == 
0){ goto _LL357;} else{ goto _LL326;} _LL357: _temp356= _temp325.f2; if(
_temp356 ==  0){ goto _LL326;} else{ _temp358=* _temp356; _LL360: _temp359=(
struct Cyc_Absyn_Structdecl**) _temp358.v; goto _LL334;} _LL328: ge->structdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Structdecl** data)) Cyc_Dict_insert)( ge->structdecls, _temp321,({
struct Cyc_Absyn_Structdecl** _temp363=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp363[ 0]= sd; _temp363;}));
goto _LL326; _LL330: { struct Cyc_Absyn_Structdecl** _temp364=({ struct Cyc_Absyn_Structdecl**
_temp365=( struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp365[ 0]=({ struct Cyc_Absyn_Structdecl* _temp366=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp366->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp366->name= sd->name; _temp366->tvs= _temp323; _temp366->fields=
0; _temp366->attributes= 0; _temp366;}); _temp365;}); ge->structdecls=(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl**
data)) Cyc_Dict_insert)( ge->structdecls, _temp321, _temp364); Cyc_Tc_tcStructFields(
te, ge, loc, _tag_arr( _temp322, sizeof( unsigned char), 7u), _temp344, _temp323);*
_temp364= sd; goto _LL326;} _LL332: { struct Cyc_Absyn_Structdecl* _temp367=*
_temp349;* _temp349=({ struct Cyc_Absyn_Structdecl* _temp368=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp368->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp368->name= sd->name; _temp368->tvs= _temp323; _temp368->fields=
0; _temp368->attributes= 0; _temp368;}); Cyc_Tc_tcStructFields( te, ge, loc,
_tag_arr( _temp322, sizeof( unsigned char), 7u), _temp354, _temp323);* _temp349=
_temp367; _temp359= _temp349; goto _LL334;} _LL334: { struct Cyc_Absyn_Structdecl*
_temp369= Cyc_Tcdecl_merge_structdecl(* _temp359, sd, loc, Cyc_Tc_tc_msg); if(
_temp369 ==  0){ return;} else{* _temp359=( struct Cyc_Absyn_Structdecl*)
_check_null( _temp369); sd=( struct Cyc_Absyn_Structdecl*) _check_null( _temp369);
goto _LL326;}} _LL326:;} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,
_temp321,({ struct _tuple3* _temp370=( struct _tuple3*) GC_malloc( sizeof(
struct _tuple3)); _temp370->f1=( void*)({ struct Cyc_Tcenv_StructRes_struct*
_temp371=( struct Cyc_Tcenv_StructRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_StructRes_struct));
_temp371[ 0]=({ struct Cyc_Tcenv_StructRes_struct _temp372; _temp372.tag= Cyc_Tcenv_StructRes;
_temp372.f1= sd; _temp372;}); _temp371;}); _temp370->f2= 1; _temp370;}));}}
static void Cyc_Tc_tcUniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl* ud){ struct
_tagged_arr* _temp373=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
ud->name))->v)).f2; const unsigned char* _temp374="union"; Cyc_Tc_tcStructOrUniondecl(
te, ge, _tag_arr( _temp374, sizeof( unsigned char), 6u), loc,( struct Cyc_Absyn_Structdecl*)
ud);{ struct Cyc_List_List* _temp375= ud->tvs; struct _tuple4 _temp377=({ struct
_tuple4 _temp376; _temp376.f1= ud->fields; _temp376.f2=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)( ge->uniondecls,
_temp373); _temp376;}); struct Cyc_Core_Opt* _temp387; struct Cyc_Core_Opt*
_temp389; struct Cyc_Core_Opt* _temp391; struct Cyc_Core_Opt* _temp393; struct
Cyc_Core_Opt _temp395; struct Cyc_List_List* _temp396; struct Cyc_Core_Opt*
_temp398; struct Cyc_Core_Opt _temp400; struct Cyc_Absyn_Uniondecl** _temp401;
struct Cyc_Core_Opt* _temp403; struct Cyc_Core_Opt _temp405; struct Cyc_List_List*
_temp406; struct Cyc_Core_Opt* _temp408; struct Cyc_Core_Opt _temp410; struct
Cyc_Absyn_Uniondecl** _temp411; struct Cyc_Core_Opt* _temp413; _LL379: _LL390:
_temp389= _temp377.f1; if( _temp389 ==  0){ goto _LL388;} else{ goto _LL381;}
_LL388: _temp387= _temp377.f2; if( _temp387 ==  0){ goto _LL380;} else{ goto
_LL381;} _LL381: _LL394: _temp393= _temp377.f1; if( _temp393 ==  0){ goto _LL383;}
else{ _temp395=* _temp393; _LL397: _temp396=( struct Cyc_List_List*) _temp395.v;
goto _LL392;} _LL392: _temp391= _temp377.f2; if( _temp391 ==  0){ goto _LL382;}
else{ goto _LL383;} _LL383: _LL404: _temp403= _temp377.f1; if( _temp403 ==  0){
goto _LL385;} else{ _temp405=* _temp403; _LL407: _temp406=( struct Cyc_List_List*)
_temp405.v; goto _LL399;} _LL399: _temp398= _temp377.f2; if( _temp398 ==  0){
goto _LL385;} else{ _temp400=* _temp398; _LL402: _temp401=( struct Cyc_Absyn_Uniondecl**)
_temp400.v; goto _LL384;} _LL385: _LL414: _temp413= _temp377.f1; if( _temp413 == 
0){ goto _LL409;} else{ goto _LL378;} _LL409: _temp408= _temp377.f2; if(
_temp408 ==  0){ goto _LL378;} else{ _temp410=* _temp408; _LL412: _temp411=(
struct Cyc_Absyn_Uniondecl**) _temp410.v; goto _LL386;} _LL380: ge->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)( ge->uniondecls, _temp373,({
struct Cyc_Absyn_Uniondecl** _temp415=( struct Cyc_Absyn_Uniondecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl*)); _temp415[ 0]= ud; _temp415;})); goto
_LL378; _LL382: { struct Cyc_Absyn_Uniondecl** _temp416=({ struct Cyc_Absyn_Uniondecl**
_temp421=( struct Cyc_Absyn_Uniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl*));
_temp421[ 0]=({ struct Cyc_Absyn_Uniondecl* _temp422=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp422->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp422->name= ud->name; _temp422->tvs= _temp375; _temp422->fields=
0; _temp422->attributes= ud->attributes; _temp422;}); _temp421;}); ge->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)( ge->uniondecls, _temp373,
_temp416); Cyc_Tc_tcStructFields( te, ge, loc, _tag_arr( _temp374, sizeof(
unsigned char), 6u), _temp396, _temp375);{ struct Cyc_List_List* f= _temp396;
for( 0; f !=  0; f=(( struct Cyc_List_List*) _check_null( f))->tl){ if( ! Cyc_Tcutil_bits_only((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( f))->hd)->type)){({
struct Cyc_Stdio_String_pa_struct _temp420; _temp420.tag= Cyc_Stdio_String_pa;
_temp420.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( f))->hd)->type);{ struct Cyc_Stdio_String_pa_struct
_temp419; _temp419.tag= Cyc_Stdio_String_pa; _temp419.f1=( struct _tagged_arr)*
_temp373;{ struct Cyc_Stdio_String_pa_struct _temp418; _temp418.tag= Cyc_Stdio_String_pa;
_temp418.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( f))->hd)->name;{ void* _temp417[ 3u]={& _temp418,& _temp419,&
_temp420}; Cyc_Tcutil_terr( loc, _tag_arr("field %s of union %s has type %s which is not `bits-only'",
sizeof( unsigned char), 58u), _tag_arr( _temp417, sizeof( void*), 3u));}}}});}}}*
_temp416= ud; goto _LL378;} _LL384: { struct Cyc_Absyn_Uniondecl* _temp423=*
_temp401;* _temp401=({ struct Cyc_Absyn_Uniondecl* _temp424=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp424->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp424->name= ud->name; _temp424->tvs= _temp375; _temp424->fields=
0; _temp424->attributes= ud->attributes; _temp424;}); Cyc_Tc_tcStructFields( te,
ge, loc, _tag_arr( _temp374, sizeof( unsigned char), 6u), _temp406, _temp375);*
_temp401= _temp423; _temp411= _temp401; goto _LL386;} _LL386: { struct Cyc_Absyn_Uniondecl*
_temp425= Cyc_Tcdecl_merge_uniondecl(* _temp411, ud, loc, Cyc_Tc_tc_msg); if(
_temp425 ==  0){ return;} else{* _temp411=( struct Cyc_Absyn_Uniondecl*)
_check_null( _temp425); goto _LL378;}} _LL378:;}} struct _tuple5{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; static struct Cyc_List_List* Cyc_Tc_tcTunionFields( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct
_tagged_arr obj, int is_xtunion, struct _tuple0* name, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs, struct Cyc_Absyn_Tuniondecl* tudres){{ struct
Cyc_List_List* _temp426= fields; for( 0; _temp426 !=  0; _temp426=(( struct Cyc_List_List*)
_check_null( _temp426))->tl){ struct Cyc_Absyn_Tunionfield* _temp427=( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp426))->hd;{
struct Cyc_List_List* tvs= _temp427->tvs; for( 0; tvs !=  0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){ struct Cyc_Absyn_Tvar* _temp428=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd; struct Cyc_Absyn_Conref* _temp429=
Cyc_Absyn_compress_conref( _temp428->kind); void* _temp430=( void*) _temp429->v;
void* _temp438; _LL432: if( _temp430 == ( void*) Cyc_Absyn_No_constr){ goto
_LL433;} else{ goto _LL434;} _LL434: if(( unsigned int) _temp430 >  1u?*(( int*)
_temp430) ==  Cyc_Absyn_Eq_constr: 0){ _LL439: _temp438=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp430)->f1; if( _temp438 == ( void*) Cyc_Absyn_MemKind){ goto _LL435;} else{
goto _LL436;}} else{ goto _LL436;} _LL436: goto _LL437; _LL433:( void*)(
_temp429->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp440=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp440[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp441; _temp441.tag= Cyc_Absyn_Eq_constr;
_temp441.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp441;}); _temp440;})));
goto _LL431; _LL435:({ struct Cyc_Stdio_String_pa_struct _temp443; _temp443.tag=
Cyc_Stdio_String_pa; _temp443.f1=( struct _tagged_arr)*(* _temp427->name).f2;{
void* _temp442[ 1u]={& _temp443}; Cyc_Tcutil_terr( _temp427->loc, _tag_arr("field %s abstracts type variable of kind M",
sizeof( unsigned char), 43u), _tag_arr( _temp442, sizeof( void*), 1u));}}); goto
_LL431; _LL437: goto _LL431; _LL431:;}}{ struct Cyc_List_List* alltvs=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tvs, _temp427->tvs); Cyc_Tcutil_check_unique_tvars( loc, alltvs); Cyc_Tcutil_add_tvar_identities(
_temp427->tvs);{ struct Cyc_List_List* typs= _temp427->typs; for( 0; typs !=  0;
typs=(( struct Cyc_List_List*) _check_null( typs))->tl){ Cyc_Tcutil_check_type(
_temp427->loc, te, alltvs,( void*) Cyc_Absyn_MemKind, 0,(*(( struct _tuple5*)((
struct Cyc_List_List*) _check_null( typs))->hd)).f2);}}{ void* _temp444=(*
_temp427->name).f1; struct Cyc_List_List* _temp454; _LL446: if(( unsigned int)
_temp444 >  1u?*(( int*) _temp444) ==  Cyc_Absyn_Rel_n: 0){ _LL455: _temp454=((
struct Cyc_Absyn_Rel_n_struct*) _temp444)->f1; if( _temp454 ==  0){ goto _LL447;}
else{ goto _LL448;}} else{ goto _LL448;} _LL448: if(( unsigned int) _temp444 > 
1u?*(( int*) _temp444) ==  Cyc_Absyn_Rel_n: 0){ goto _LL449;} else{ goto _LL450;}
_LL450: if(( unsigned int) _temp444 >  1u?*(( int*) _temp444) ==  Cyc_Absyn_Abs_n:
0){ goto _LL451;} else{ goto _LL452;} _LL452: if( _temp444 == ( void*) Cyc_Absyn_Loc_n){
goto _LL453;} else{ goto _LL445;} _LL447: if( is_xtunion){(* _temp427->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp456=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp456[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp457; _temp457.tag= Cyc_Absyn_Abs_n; _temp457.f1= te->ns; _temp457;});
_temp456;});} else{(* _temp427->name).f1=(* name).f1;} goto _LL445; _LL449:({
void* _temp458[ 0u]={}; Cyc_Tcutil_terr( _temp427->loc, _tag_arr("qualified declarations are not allowed",
sizeof( unsigned char), 39u), _tag_arr( _temp458, sizeof( void*), 0u));}); goto
_LL445; _LL451: goto _LL445; _LL453:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp459=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp459[ 0]=({ struct Cyc_Core_Impossible_struct _temp460; _temp460.tag= Cyc_Core_Impossible;
_temp460.f1= _tag_arr("tcTunionFields: Loc_n", sizeof( unsigned char), 22u);
_temp460;}); _temp459;})); goto _LL445; _LL445:;}}}}{ struct Cyc_List_List*
fields2; if( is_xtunion){ int _temp461= 1; struct Cyc_List_List* _temp462= Cyc_Tcdecl_sort_xtunion_fields(
fields,& _temp461,(* name).f2, loc, Cyc_Tc_tc_msg); if( _temp461){ fields2=
_temp462;} else{ fields2= 0;}} else{ struct _RegionHandle _temp463= _new_region();
struct _RegionHandle* uprev_rgn=& _temp463; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0;{ struct Cyc_List_List* fs= fields; for( 0; fs != 
0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){ struct Cyc_Absyn_Tunionfield*
_temp464=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fs))->hd; if((( int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*),
struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp,
prev_fields,(* _temp464->name).f2)){({ struct Cyc_Stdio_String_pa_struct
_temp467; _temp467.tag= Cyc_Stdio_String_pa; _temp467.f1=( struct _tagged_arr)
obj;{ struct Cyc_Stdio_String_pa_struct _temp466; _temp466.tag= Cyc_Stdio_String_pa;
_temp466.f1=( struct _tagged_arr)*(* _temp464->name).f2;{ void* _temp465[ 2u]={&
_temp466,& _temp467}; Cyc_Tcutil_terr( _temp464->loc, _tag_arr("duplicate field name %s in %s",
sizeof( unsigned char), 30u), _tag_arr( _temp465, sizeof( void*), 2u));}}});}
else{ prev_fields=({ struct Cyc_List_List* _temp468=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp468->hd=( void*)(*
_temp464->name).f2; _temp468->tl= prev_fields; _temp468;});} if(( void*)
_temp464->sc != ( void*) Cyc_Absyn_Public){({ struct Cyc_Stdio_String_pa_struct
_temp470; _temp470.tag= Cyc_Stdio_String_pa; _temp470.f1=( struct _tagged_arr)*(*
_temp464->name).f2;{ void* _temp469[ 1u]={& _temp470}; Cyc_Tcutil_warn( loc,
_tag_arr("ignoring scope of field %s", sizeof( unsigned char), 27u), _tag_arr(
_temp469, sizeof( void*), 1u));}});( void*)( _temp464->sc=( void*)(( void*) Cyc_Absyn_Public));}}}
fields2= fields;}; _pop_region( uprev_rgn);}{ struct Cyc_List_List* _temp471=
fields; for( 0; _temp471 !=  0; _temp471=(( struct Cyc_List_List*) _check_null(
_temp471))->tl){ struct Cyc_Absyn_Tunionfield* _temp472=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp471))->hd; ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries,(* _temp472->name).f2,({ struct _tuple3* _temp473=( struct
_tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp473->f1=( void*)({ struct
Cyc_Tcenv_TunionRes_struct* _temp474=( struct Cyc_Tcenv_TunionRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct)); _temp474[ 0]=({ struct
Cyc_Tcenv_TunionRes_struct _temp475; _temp475.tag= Cyc_Tcenv_TunionRes; _temp475.f1=
tudres; _temp475.f2= _temp472; _temp475;}); _temp474;}); _temp473->f2= 1;
_temp473;}));}} return fields2;}} static void Cyc_Tc_tcTuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl*
tud){ struct _tagged_arr* v=(* tud->name).f2; struct _tagged_arr obj; if( tud->is_xtunion){
obj= _tag_arr("xtunion", sizeof( unsigned char), 8u);} else{ obj= _tag_arr("tunion",
sizeof( unsigned char), 7u);}{ struct Cyc_List_List* tvs= tud->tvs;{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 !=  0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
struct Cyc_Absyn_Conref* c= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->kind); void* _temp476=( void*)
c->v; void* _temp484; _LL478: if( _temp476 == ( void*) Cyc_Absyn_No_constr){
goto _LL479;} else{ goto _LL480;} _LL480: if(( unsigned int) _temp476 >  1u?*((
int*) _temp476) ==  Cyc_Absyn_Eq_constr: 0){ _LL485: _temp484=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp476)->f1; if( _temp484 == ( void*) Cyc_Absyn_MemKind){
goto _LL481;} else{ goto _LL482;}} else{ goto _LL482;} _LL482: goto _LL483;
_LL479:( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp486=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp486[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp487; _temp487.tag= Cyc_Absyn_Eq_constr;
_temp487.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp487;}); _temp486;})));
goto _LL477; _LL481:({ struct Cyc_Stdio_String_pa_struct _temp491; _temp491.tag=
Cyc_Stdio_String_pa; _temp491.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->name;{ struct Cyc_Stdio_String_pa_struct
_temp490; _temp490.tag= Cyc_Stdio_String_pa; _temp490.f1=( struct _tagged_arr)*
v;{ struct Cyc_Stdio_String_pa_struct _temp489; _temp489.tag= Cyc_Stdio_String_pa;
_temp489.f1=( struct _tagged_arr) obj;{ void* _temp488[ 3u]={& _temp489,&
_temp490,& _temp491}; Cyc_Tcutil_terr( loc, _tag_arr("%s %s attempts to abstract type variable %s of kind M",
sizeof( unsigned char), 54u), _tag_arr( _temp488, sizeof( void*), 3u));}}}});
goto _LL477; _LL483: goto _LL477; _LL477:;}} Cyc_Tcutil_check_unique_tvars( loc,
tvs); Cyc_Tcutil_add_tvar_identities( tvs);{ struct Cyc_Core_Opt* tud_opt; if(
tud->is_xtunion){{ struct _handler_cons _temp492; _push_handler(& _temp492);{
int _temp494= 0; if( setjmp( _temp492.handler)){ _temp494= 1;} if( ! _temp494){
tud_opt= Cyc_Tcenv_lookup_xtuniondecl( te, loc, tud->name);; _pop_handler();}
else{ void* _temp493=( void*) _exn_thrown; void* _temp496= _temp493; _LL498: if(
_temp496 ==  Cyc_Dict_Absent){ goto _LL499;} else{ goto _LL500;} _LL500: goto
_LL501; _LL499:({ struct Cyc_Stdio_String_pa_struct _temp503; _temp503.tag= Cyc_Stdio_String_pa;
_temp503.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( tud->name);{ void*
_temp502[ 1u]={& _temp503}; Cyc_Tcutil_terr( loc, _tag_arr("qualified xtunion declaration %s is not an existing xtunion",
sizeof( unsigned char), 60u), _tag_arr( _temp502, sizeof( void*), 1u));}});
return; _LL501:( void) _throw( _temp496); _LL497:;}}} if( tud_opt !=  0){ tud->name=(*((
struct Cyc_Absyn_Tuniondecl**)(( struct Cyc_Core_Opt*) _check_null( tud_opt))->v))->name;}
else{(* tud->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp504=(
struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp504[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp505; _temp505.tag= Cyc_Absyn_Abs_n;
_temp505.f1= te->ns; _temp505;}); _temp504;});}} else{{ void* _temp506=(* tud->name).f1;
struct Cyc_List_List* _temp514; _LL508: if(( unsigned int) _temp506 >  1u?*((
int*) _temp506) ==  Cyc_Absyn_Rel_n: 0){ _LL515: _temp514=(( struct Cyc_Absyn_Rel_n_struct*)
_temp506)->f1; if( _temp514 ==  0){ goto _LL509;} else{ goto _LL510;}} else{
goto _LL510;} _LL510: if(( unsigned int) _temp506 >  1u?*(( int*) _temp506) == 
Cyc_Absyn_Abs_n: 0){ goto _LL511;} else{ goto _LL512;} _LL512: goto _LL513;
_LL509:(* tud->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp516=(
struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp516[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp517; _temp517.tag= Cyc_Absyn_Abs_n;
_temp517.f1= te->ns; _temp517;}); _temp516;}); goto _LL507; _LL511: goto _LL513;
_LL513:({ void* _temp518[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("qualified declarations are not implemented",
sizeof( unsigned char), 43u), _tag_arr( _temp518, sizeof( void*), 0u));});
return; _LL507:;} tud_opt=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup_opt)( ge->tuniondecls, v);}{ struct
_tuple4 _temp520=({ struct _tuple4 _temp519; _temp519.f1= tud->fields; _temp519.f2=
tud_opt; _temp519;}); struct Cyc_Core_Opt* _temp530; struct Cyc_Core_Opt*
_temp532; struct Cyc_Core_Opt* _temp534; struct Cyc_Core_Opt* _temp536; struct
Cyc_Core_Opt _temp538; struct Cyc_List_List* _temp539; struct Cyc_List_List**
_temp541; struct Cyc_Core_Opt* _temp542; struct Cyc_Core_Opt _temp544; struct
Cyc_Absyn_Tuniondecl** _temp545; struct Cyc_Core_Opt* _temp547; struct Cyc_Core_Opt
_temp549; struct Cyc_List_List* _temp550; struct Cyc_List_List** _temp552;
struct Cyc_Core_Opt* _temp553; struct Cyc_Core_Opt _temp555; struct Cyc_Absyn_Tuniondecl**
_temp556; struct Cyc_Core_Opt* _temp558; _LL522: _LL533: _temp532= _temp520.f1;
if( _temp532 ==  0){ goto _LL531;} else{ goto _LL524;} _LL531: _temp530=
_temp520.f2; if( _temp530 ==  0){ goto _LL523;} else{ goto _LL524;} _LL524:
_LL537: _temp536= _temp520.f1; if( _temp536 ==  0){ goto _LL526;} else{ _temp538=*
_temp536; _LL540: _temp539=( struct Cyc_List_List*) _temp538.v; _temp541=&(*
_temp520.f1).v; goto _LL535;} _LL535: _temp534= _temp520.f2; if( _temp534 ==  0){
goto _LL525;} else{ goto _LL526;} _LL526: _LL548: _temp547= _temp520.f1; if(
_temp547 ==  0){ goto _LL528;} else{ _temp549=* _temp547; _LL551: _temp550=(
struct Cyc_List_List*) _temp549.v; _temp552=&(* _temp520.f1).v; goto _LL543;}
_LL543: _temp542= _temp520.f2; if( _temp542 ==  0){ goto _LL528;} else{ _temp544=*
_temp542; _LL546: _temp545=( struct Cyc_Absyn_Tuniondecl**) _temp544.v; goto
_LL527;} _LL528: _LL559: _temp558= _temp520.f1; if( _temp558 ==  0){ goto _LL554;}
else{ goto _LL521;} _LL554: _temp553= _temp520.f2; if( _temp553 ==  0){ goto
_LL521;} else{ _temp555=* _temp553; _LL557: _temp556=( struct Cyc_Absyn_Tuniondecl**)
_temp555.v; goto _LL529;} _LL523: ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl**
data)) Cyc_Dict_insert)( ge->tuniondecls, v,({ struct Cyc_Absyn_Tuniondecl**
_temp560=( struct Cyc_Absyn_Tuniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*));
_temp560[ 0]= tud; _temp560;})); goto _LL521; _LL525: { struct Cyc_Absyn_Tuniondecl**
_temp561=({ struct Cyc_Absyn_Tuniondecl** _temp562=( struct Cyc_Absyn_Tuniondecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp562[ 0]=({ struct Cyc_Absyn_Tuniondecl*
_temp563=( struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp563->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp563->name= tud->name;
_temp563->tvs= tvs; _temp563->fields= 0; _temp563->is_xtunion= tud->is_xtunion;
_temp563;}); _temp562;}); ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl** data))
Cyc_Dict_insert)( ge->tuniondecls, v, _temp561);* _temp541= Cyc_Tc_tcTunionFields(
te, ge, loc, obj, tud->is_xtunion, tud->name,* _temp541, tvs, tud);* _temp561=
tud; goto _LL521;} _LL527: { struct Cyc_Absyn_Tuniondecl* _temp564=* _temp545;*
_temp545=({ struct Cyc_Absyn_Tuniondecl* _temp565=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp565->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp565->name= tud->name; _temp565->tvs= tvs; _temp565->fields=
0; _temp565->is_xtunion= tud->is_xtunion; _temp565;});* _temp552= Cyc_Tc_tcTunionFields(
te, ge, loc, obj, tud->is_xtunion, tud->name,* _temp552, tvs, tud);* _temp545=
_temp564; _temp556= _temp545; goto _LL529;} _LL529: { struct Cyc_Absyn_Tuniondecl*
_temp566= Cyc_Tcdecl_merge_tuniondecl(* _temp556, tud, loc, Cyc_Tc_tc_msg); if(
_temp566 ==  0){ return;} else{* _temp556=( struct Cyc_Absyn_Tuniondecl*)
_check_null( _temp566); goto _LL521;}} _LL521:;}}}} static void Cyc_Tc_tcEnumdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Enumdecl* ed){ struct _tagged_arr* v=(* ed->name).f2;{
void* _temp567=(* ed->name).f1; struct Cyc_List_List* _temp575; struct Cyc_List_List*
_temp577; _LL569: if(( unsigned int) _temp567 >  1u?*(( int*) _temp567) ==  Cyc_Absyn_Rel_n:
0){ _LL576: _temp575=(( struct Cyc_Absyn_Rel_n_struct*) _temp567)->f1; if(
_temp575 ==  0){ goto _LL570;} else{ goto _LL571;}} else{ goto _LL571;} _LL571:
if(( unsigned int) _temp567 >  1u?*(( int*) _temp567) ==  Cyc_Absyn_Abs_n: 0){
_LL578: _temp577=(( struct Cyc_Absyn_Abs_n_struct*) _temp567)->f1; if( _temp577
==  0){ goto _LL572;} else{ goto _LL573;}} else{ goto _LL573;} _LL573: goto
_LL574; _LL570: goto _LL568; _LL572: goto _LL568; _LL574:({ void* _temp579[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("qualified declarations are not implemented",
sizeof( unsigned char), 43u), _tag_arr( _temp579, sizeof( void*), 0u));});
return; _LL568:;}(* ed->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp580=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp580[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp581; _temp581.tag= Cyc_Absyn_Abs_n;
_temp581.f1= te->ns; _temp581;}); _temp580;}); if( ed->fields !=  0){ struct
_RegionHandle _temp582= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp582; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0;
unsigned int tag_count= 0; struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( ed->fields))->v; for( 0; fs !=  0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ struct Cyc_Absyn_Enumfield*
_temp583=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( fs))->hd;
if((( int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*), struct
Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp,
prev_fields,(* _temp583->name).f2)){({ struct Cyc_Stdio_String_pa_struct
_temp585; _temp585.tag= Cyc_Stdio_String_pa; _temp585.f1=( struct _tagged_arr)*(*
_temp583->name).f2;{ void* _temp584[ 1u]={& _temp585}; Cyc_Tcutil_terr( _temp583->loc,
_tag_arr("duplicate field name %s", sizeof( unsigned char), 24u), _tag_arr(
_temp584, sizeof( void*), 1u));}});} else{ prev_fields=({ struct Cyc_List_List*
_temp586=( struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp586->hd=( void*)(* _temp583->name).f2; _temp586->tl= prev_fields; _temp586;});}
if( _temp583->tag ==  0){ _temp583->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
tag_count, _temp583->loc);} else{ if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp583->tag))){({ struct Cyc_Stdio_String_pa_struct _temp589;
_temp589.tag= Cyc_Stdio_String_pa; _temp589.f1=( struct _tagged_arr)*(* _temp583->name).f2;{
struct Cyc_Stdio_String_pa_struct _temp588; _temp588.tag= Cyc_Stdio_String_pa;
_temp588.f1=( struct _tagged_arr)* v;{ void* _temp587[ 2u]={& _temp588,&
_temp589}; Cyc_Tcutil_terr( loc, _tag_arr("enum %s, field %s: expression is not constant",
sizeof( unsigned char), 46u), _tag_arr( _temp587, sizeof( void*), 2u));}}});}}{
unsigned int t1= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp583->tag)); tag_count= t1 +  1;(* _temp583->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp590=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp590[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp591; _temp591.tag= Cyc_Absyn_Abs_n; _temp591.f1= te->ns; _temp591;});
_temp590;});}}}; _pop_region( uprev_rgn);}{ struct _handler_cons _temp592;
_push_handler(& _temp592);{ int _temp594= 0; if( setjmp( _temp592.handler)){
_temp594= 1;} if( ! _temp594){{ struct Cyc_Absyn_Enumdecl** _temp595=(( struct
Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
ge->enumdecls, v); struct Cyc_Absyn_Enumdecl* _temp596= Cyc_Tcdecl_merge_enumdecl(*
_temp595, ed, loc, Cyc_Tc_tc_msg); if( _temp596 ==  0){ _npop_handler( 0u);
return;}* _temp595=( struct Cyc_Absyn_Enumdecl*) _check_null( _temp596);};
_pop_handler();} else{ void* _temp593=( void*) _exn_thrown; void* _temp598=
_temp593; _LL600: if( _temp598 ==  Cyc_Dict_Absent){ goto _LL601;} else{ goto
_LL602;} _LL602: goto _LL603; _LL601: { struct Cyc_Absyn_Enumdecl** _temp604=({
struct Cyc_Absyn_Enumdecl** _temp605=( struct Cyc_Absyn_Enumdecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl*)); _temp605[ 0]= ed; _temp605;}); ge->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Enumdecl** data)) Cyc_Dict_insert)( ge->enumdecls, v, _temp604);
goto _LL599;} _LL603:( void) _throw( _temp598); _LL599:;}}} if( ed->fields !=  0){
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ed->fields))->v; for( 0; fs !=  0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ struct Cyc_Absyn_Enumfield* _temp606=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_List_List*) _check_null( fs))->hd; ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries,(* _temp606->name).f2,({ struct _tuple3* _temp607=( struct
_tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp607->f1=( void*)({ struct
Cyc_Tcenv_EnumRes_struct* _temp608=( struct Cyc_Tcenv_EnumRes_struct*) GC_malloc(
sizeof( struct Cyc_Tcenv_EnumRes_struct)); _temp608[ 0]=({ struct Cyc_Tcenv_EnumRes_struct
_temp609; _temp609.tag= Cyc_Tcenv_EnumRes; _temp609.f1= ed; _temp609.f2=
_temp606; _temp609;}); _temp608;}); _temp607->f2= 1; _temp607;}));}}} static int
Cyc_Tc_okay_externC( struct Cyc_Position_Segment* loc, void* sc){ void* _temp610=
sc; _LL612: if( _temp610 == ( void*) Cyc_Absyn_Static){ goto _LL613;} else{ goto
_LL614;} _LL614: if( _temp610 == ( void*) Cyc_Absyn_Abstract){ goto _LL615;}
else{ goto _LL616;} _LL616: if( _temp610 == ( void*) Cyc_Absyn_Public){ goto
_LL617;} else{ goto _LL618;} _LL618: if( _temp610 == ( void*) Cyc_Absyn_Extern){
goto _LL619;} else{ goto _LL620;} _LL620: if( _temp610 == ( void*) Cyc_Absyn_ExternC){
goto _LL621;} else{ goto _LL611;} _LL613:({ void* _temp622[ 0u]={}; Cyc_Tcutil_warn(
loc, _tag_arr("static declaration nested within extern \"C\"", sizeof(
unsigned char), 44u), _tag_arr( _temp622, sizeof( void*), 0u));}); return 0;
_LL615:({ void* _temp623[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("abstract declaration nested within extern \"C\"",
sizeof( unsigned char), 46u), _tag_arr( _temp623, sizeof( void*), 0u));});
return 0; _LL617: return 1; _LL619: return 1; _LL621:({ void* _temp624[ 0u]={};
Cyc_Tcutil_warn( loc, _tag_arr("nested extern \"C\" declaration", sizeof(
unsigned char), 30u), _tag_arr( _temp624, sizeof( void*), 0u));}); return 1;
_LL611:;} static void Cyc_Tc_tc_decls( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
ds0, int in_externC, int check_var_init){ struct Cyc_Tcenv_Genv* ge=(( struct
Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(
te->ae, te->ns); struct Cyc_List_List* _temp625= ds0; for( 0; _temp625 !=  0;
_temp625=(( struct Cyc_List_List*) _check_null( _temp625))->tl){ struct Cyc_Absyn_Decl*
d=( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( _temp625))->hd;
struct Cyc_Position_Segment* loc= d->loc; void* _temp626=( void*) d->r; struct
Cyc_Absyn_Vardecl* _temp652; struct Cyc_Absyn_Fndecl* _temp654; struct Cyc_Absyn_Typedefdecl*
_temp656; struct Cyc_Absyn_Structdecl* _temp658; struct Cyc_Absyn_Uniondecl*
_temp660; struct Cyc_Absyn_Tuniondecl* _temp662; struct Cyc_Absyn_Enumdecl*
_temp664; struct Cyc_List_List* _temp666; struct _tagged_arr* _temp668; struct
Cyc_List_List* _temp670; struct _tuple0* _temp672; struct _tuple0 _temp674;
struct _tagged_arr* _temp675; void* _temp677; struct Cyc_List_List* _temp679;
_LL628: if(*(( int*) _temp626) ==  Cyc_Absyn_Let_d){ goto _LL629;} else{ goto
_LL630;} _LL630: if(*(( int*) _temp626) ==  Cyc_Absyn_Letv_d){ goto _LL631;}
else{ goto _LL632;} _LL632: if(*(( int*) _temp626) ==  Cyc_Absyn_Var_d){ _LL653:
_temp652=(( struct Cyc_Absyn_Var_d_struct*) _temp626)->f1; goto _LL633;} else{
goto _LL634;} _LL634: if(*(( int*) _temp626) ==  Cyc_Absyn_Fn_d){ _LL655:
_temp654=(( struct Cyc_Absyn_Fn_d_struct*) _temp626)->f1; goto _LL635;} else{
goto _LL636;} _LL636: if(*(( int*) _temp626) ==  Cyc_Absyn_Typedef_d){ _LL657:
_temp656=(( struct Cyc_Absyn_Typedef_d_struct*) _temp626)->f1; goto _LL637;}
else{ goto _LL638;} _LL638: if(*(( int*) _temp626) ==  Cyc_Absyn_Struct_d){
_LL659: _temp658=(( struct Cyc_Absyn_Struct_d_struct*) _temp626)->f1; goto
_LL639;} else{ goto _LL640;} _LL640: if(*(( int*) _temp626) ==  Cyc_Absyn_Union_d){
_LL661: _temp660=(( struct Cyc_Absyn_Union_d_struct*) _temp626)->f1; goto _LL641;}
else{ goto _LL642;} _LL642: if(*(( int*) _temp626) ==  Cyc_Absyn_Tunion_d){
_LL663: _temp662=(( struct Cyc_Absyn_Tunion_d_struct*) _temp626)->f1; goto
_LL643;} else{ goto _LL644;} _LL644: if(*(( int*) _temp626) ==  Cyc_Absyn_Enum_d){
_LL665: _temp664=(( struct Cyc_Absyn_Enum_d_struct*) _temp626)->f1; goto _LL645;}
else{ goto _LL646;} _LL646: if(*(( int*) _temp626) ==  Cyc_Absyn_Namespace_d){
_LL669: _temp668=(( struct Cyc_Absyn_Namespace_d_struct*) _temp626)->f1; goto
_LL667; _LL667: _temp666=(( struct Cyc_Absyn_Namespace_d_struct*) _temp626)->f2;
goto _LL647;} else{ goto _LL648;} _LL648: if(*(( int*) _temp626) ==  Cyc_Absyn_Using_d){
_LL673: _temp672=(( struct Cyc_Absyn_Using_d_struct*) _temp626)->f1; _temp674=*
_temp672; _LL678: _temp677= _temp674.f1; goto _LL676; _LL676: _temp675= _temp674.f2;
goto _LL671; _LL671: _temp670=(( struct Cyc_Absyn_Using_d_struct*) _temp626)->f2;
goto _LL649;} else{ goto _LL650;} _LL650: if(*(( int*) _temp626) ==  Cyc_Absyn_ExternC_d){
_LL680: _temp679=(( struct Cyc_Absyn_ExternC_d_struct*) _temp626)->f1; goto
_LL651;} else{ goto _LL627;} _LL629:({ void* _temp681[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("top level let-declarations are not implemented", sizeof(
unsigned char), 47u), _tag_arr( _temp681, sizeof( void*), 0u));}); goto _LL627;
_LL631:({ void* _temp682[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("top level let-declarations are not implemented",
sizeof( unsigned char), 47u), _tag_arr( _temp682, sizeof( void*), 0u));}); goto
_LL627; _LL633: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp652->sc):
0){( void*)( _temp652->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcVardecl(
te, ge, loc, _temp652, check_var_init); goto _LL627; _LL635: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp654->sc): 0){( void*)( _temp654->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcFndecl( te, ge, loc, _temp654); goto _LL627; _LL637: Cyc_Tc_tcTypedefdecl(
te, ge, loc, _temp656); goto _LL627; _LL639: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp658->sc): 0){( void*)( _temp658->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcStructdecl( te, ge, loc, _temp658); goto _LL627; _LL641: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp660->sc): 0){( void*)( _temp660->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcUniondecl( te, ge, loc, _temp660);
goto _LL627; _LL643: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*)
_temp662->sc): 0){( void*)( _temp662->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcTuniondecl( te, ge, loc, _temp662); goto _LL627; _LL645: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp664->sc): 0){( void*)( _temp664->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcEnumdecl( te, ge, loc, _temp664);
goto _LL627; _LL647: { struct Cyc_List_List* _temp683= te->ns; struct Cyc_List_List*
_temp684=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp683,({ struct Cyc_List_List* _temp685=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp685->hd=( void*) _temp668;
_temp685->tl= 0; _temp685;})); if( !(( int(*)( struct Cyc_Set_Set* s, struct
_tagged_arr* elt)) Cyc_Set_member)( ge->namespaces, _temp668)){ ge->namespaces=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)(
ge->namespaces, _temp668); te->ae=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key, struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)( te->ae,
_temp684, Cyc_Tcenv_empty_genv());} te->ns= _temp684; Cyc_Tc_tc_decls( te,
_temp666, in_externC, check_var_init); te->ns= _temp683; goto _LL627;} _LL649: {
struct _tagged_arr* first; struct Cyc_List_List* rest;{ void* _temp686= _temp677;
struct Cyc_List_List* _temp698; struct Cyc_List_List* _temp700; struct Cyc_List_List*
_temp702; struct Cyc_List_List _temp704; struct Cyc_List_List* _temp705; struct
_tagged_arr* _temp707; struct Cyc_List_List* _temp709; struct Cyc_List_List
_temp711; struct Cyc_List_List* _temp712; struct _tagged_arr* _temp714; _LL688:
if( _temp686 == ( void*) Cyc_Absyn_Loc_n){ goto _LL689;} else{ goto _LL690;}
_LL690: if(( unsigned int) _temp686 >  1u?*(( int*) _temp686) ==  Cyc_Absyn_Rel_n:
0){ _LL699: _temp698=(( struct Cyc_Absyn_Rel_n_struct*) _temp686)->f1; if(
_temp698 ==  0){ goto _LL691;} else{ goto _LL692;}} else{ goto _LL692;} _LL692:
if(( unsigned int) _temp686 >  1u?*(( int*) _temp686) ==  Cyc_Absyn_Abs_n: 0){
_LL701: _temp700=(( struct Cyc_Absyn_Abs_n_struct*) _temp686)->f1; if( _temp700
==  0){ goto _LL693;} else{ goto _LL694;}} else{ goto _LL694;} _LL694: if((
unsigned int) _temp686 >  1u?*(( int*) _temp686) ==  Cyc_Absyn_Rel_n: 0){ _LL703:
_temp702=(( struct Cyc_Absyn_Rel_n_struct*) _temp686)->f1; if( _temp702 ==  0){
goto _LL696;} else{ _temp704=* _temp702; _LL708: _temp707=( struct _tagged_arr*)
_temp704.hd; goto _LL706; _LL706: _temp705= _temp704.tl; goto _LL695;}} else{
goto _LL696;} _LL696: if(( unsigned int) _temp686 >  1u?*(( int*) _temp686) == 
Cyc_Absyn_Abs_n: 0){ _LL710: _temp709=(( struct Cyc_Absyn_Abs_n_struct*)
_temp686)->f1; if( _temp709 ==  0){ goto _LL687;} else{ _temp711=* _temp709;
_LL715: _temp714=( struct _tagged_arr*) _temp711.hd; goto _LL713; _LL713:
_temp712= _temp711.tl; goto _LL697;}} else{ goto _LL687;} _LL689: goto _LL691;
_LL691: goto _LL693; _LL693: first= _temp675; rest= 0; goto _LL687; _LL695:
_temp714= _temp707; _temp712= _temp705; goto _LL697; _LL697: first= _temp714;
rest=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp712,({ struct Cyc_List_List* _temp716=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp716->hd=( void*) _temp675;
_temp716->tl= 0; _temp716;})); goto _LL687; _LL687:;}{ struct Cyc_List_List*
_temp717= Cyc_Tcenv_resolve_namespace( te, loc, first, rest); ge->availables=({
struct Cyc_List_List* _temp718=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp718->hd=( void*) _temp717; _temp718->tl= ge->availables;
_temp718;}); Cyc_Tc_tc_decls( te, _temp670, in_externC, check_var_init); ge->availables=((
struct Cyc_List_List*) _check_null( ge->availables))->tl; goto _LL627;}} _LL651:
Cyc_Tc_tc_decls( te, _temp679, 1, check_var_init); goto _LL627; _LL627:;}} void
Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int check_var_init, struct Cyc_List_List*
ds){ Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r); Cyc_Tc_tc_decls( te, ds,
0, check_var_init);} static int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env,
struct Cyc_Absyn_Decl* d); static struct Cyc_List_List* Cyc_Tc_treeshake_f(
struct Cyc_Dict_Dict* env, struct Cyc_List_List* ds){ return(( struct Cyc_List_List*(*)(
int(* f)( struct Cyc_Dict_Dict*, struct Cyc_Absyn_Decl*), struct Cyc_Dict_Dict*
env, struct Cyc_List_List* l)) Cyc_List_filter_c)( Cyc_Tc_vardecl_needed, env,
ds);} struct _tuple6{ struct Cyc_Tcenv_Genv* f1; struct Cyc_Set_Set* f2; } ;
static int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl*
d){ void* _temp719=( void*) d->r; struct Cyc_Absyn_Vardecl* _temp729; struct Cyc_List_List*
_temp731; struct Cyc_List_List** _temp733; struct Cyc_List_List* _temp734;
struct Cyc_List_List** _temp736; _LL721: if(*(( int*) _temp719) ==  Cyc_Absyn_Var_d){
_LL730: _temp729=(( struct Cyc_Absyn_Var_d_struct*) _temp719)->f1; goto _LL722;}
else{ goto _LL723;} _LL723: if(*(( int*) _temp719) ==  Cyc_Absyn_Using_d){
_LL732: _temp731=(( struct Cyc_Absyn_Using_d_struct*) _temp719)->f2; _temp733=&((
struct Cyc_Absyn_Using_d_struct*) _temp719)->f2; goto _LL724;} else{ goto _LL725;}
_LL725: if(*(( int*) _temp719) ==  Cyc_Absyn_Namespace_d){ _LL735: _temp734=((
struct Cyc_Absyn_Namespace_d_struct*) _temp719)->f2; _temp736=&(( struct Cyc_Absyn_Namespace_d_struct*)
_temp719)->f2; goto _LL726;} else{ goto _LL727;} _LL727: goto _LL728; _LL722:
if(( void*) _temp729->sc != ( void*) Cyc_Absyn_Extern){ return 1;}{ struct
_tuple0 _temp739; struct _tagged_arr* _temp740; void* _temp742; struct _tuple0*
_temp737= _temp729->name; _temp739=* _temp737; _LL743: _temp742= _temp739.f1;
goto _LL741; _LL741: _temp740= _temp739.f2; goto _LL738; _LL738: { struct Cyc_List_List*
ns;{ void* _temp744= _temp742; struct Cyc_List_List* _temp752; struct Cyc_List_List*
_temp754; _LL746: if( _temp744 == ( void*) Cyc_Absyn_Loc_n){ goto _LL747;} else{
goto _LL748;} _LL748: if(( unsigned int) _temp744 >  1u?*(( int*) _temp744) == 
Cyc_Absyn_Rel_n: 0){ _LL753: _temp752=(( struct Cyc_Absyn_Rel_n_struct*)
_temp744)->f1; goto _LL749;} else{ goto _LL750;} _LL750: if(( unsigned int)
_temp744 >  1u?*(( int*) _temp744) ==  Cyc_Absyn_Abs_n: 0){ _LL755: _temp754=((
struct Cyc_Absyn_Abs_n_struct*) _temp744)->f1; goto _LL751;} else{ goto _LL745;}
_LL747: ns= 0; goto _LL745; _LL749: ns= _temp752; goto _LL745; _LL751: ns=
_temp754; goto _LL745; _LL745:;}{ struct _tuple6* _temp756=(( struct _tuple6*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( env, ns);
struct Cyc_Tcenv_Genv* _temp757=(* _temp756).f1; int _temp758=(*(( struct
_tuple3*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
_temp757->ordinaries, _temp740)).f2; if( ! _temp758){(* _temp756).f2=(( struct
Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)((*
_temp756).f2, _temp740);} return _temp758;}}} _LL724: _temp736= _temp733; goto
_LL726; _LL726:* _temp736= Cyc_Tc_treeshake_f( env,* _temp736); return 1; _LL728:
return 1; _LL720:;} static int Cyc_Tc_treeshake_remove_f( struct Cyc_Set_Set*
set, struct _tagged_arr* x, void* y){ return !(( int(*)( struct Cyc_Set_Set* s,
struct _tagged_arr* elt)) Cyc_Set_member)( set, x);} static void Cyc_Tc_treeshake_remove(
struct Cyc_List_List* ns, struct _tuple6* nsenv){ struct Cyc_Tcenv_Genv*
_temp759=(* nsenv).f1; struct Cyc_Set_Set* _temp760=(* nsenv).f2; _temp759->ordinaries=((
struct Cyc_Dict_Dict*(*)( int(* f)( struct Cyc_Set_Set*, struct _tagged_arr*,
struct _tuple3*), struct Cyc_Set_Set* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_filter_c)((
int(*)( struct Cyc_Set_Set* set, struct _tagged_arr* x, struct _tuple3* y)) Cyc_Tc_treeshake_remove_f,
_temp760, _temp759->ordinaries);} static struct _tuple6* Cyc_Tc_treeshake_make_env_f(
struct Cyc_Tcenv_Genv* ge){ return({ struct _tuple6* _temp761=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp761->f1= ge; _temp761->f2=(( struct
Cyc_Set_Set*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Set_empty)(
Cyc_String_strptrcmp); _temp761;});} struct Cyc_List_List* Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds){ struct Cyc_Dict_Dict*
_temp762=(( struct Cyc_Dict_Dict*(*)( struct _tuple6*(* f)( struct Cyc_Tcenv_Genv*),
struct Cyc_Dict_Dict* d)) Cyc_Dict_map)( Cyc_Tc_treeshake_make_env_f, te->ae);
struct Cyc_List_List* _temp763= Cyc_Tc_treeshake_f( _temp762, ds);(( void(*)(
void(* f)( struct Cyc_List_List*, struct _tuple6*), struct Cyc_Dict_Dict* d))
Cyc_Dict_iter)( Cyc_Tc_treeshake_remove, _temp762); return _temp763;}
