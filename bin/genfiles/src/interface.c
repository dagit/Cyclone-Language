 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_dirent{ int d_ino; int d_off; unsigned short
d_reclen; unsigned char d_type; unsigned char d_name[ 256u]; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern void*
Cyc_Core_identity( void*); extern unsigned char Cyc_Core_InvalidArg[ 15u];
struct Cyc_Core_InvalidArg_struct{ unsigned char* tag; struct _tagged_arr f1; }
; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); extern int system( unsigned char*); extern int*
__errno(); struct Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[
19u]; extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_String_pa=
0; struct Cyc_Stdio_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_Int_pa= 1; struct Cyc_Stdio_Int_pa_struct{ int tag;
unsigned int f1; } ; static const int Cyc_Stdio_Double_pa= 2; struct Cyc_Stdio_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
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
} ; extern unsigned char Cyc_List_List_empty[ 15u]; extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern unsigned char
Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file; struct _tagged_arr line; int line_no; int col;
} ; extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment;
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; struct
_tuple0{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual;
struct Cyc_Absyn_Conref; struct Cyc_Absyn_PtrInfo; struct Cyc_Absyn_VarargInfo;
struct Cyc_Absyn_FnInfo; struct Cyc_Absyn_TunionInfo; struct Cyc_Absyn_TunionFieldInfo;
struct Cyc_Absyn_VarargCallInfo; struct Cyc_Absyn_Exp; struct Cyc_Absyn_Stmt;
struct Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause; struct Cyc_Absyn_SwitchC_clause;
struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl; struct Cyc_Absyn_Uniondecl;
struct Cyc_Absyn_Tuniondecl; struct Cyc_Absyn_Tunionfield; struct Cyc_Absyn_Enumfield;
struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Typedefdecl; struct Cyc_Absyn_Vardecl;
struct Cyc_Absyn_Decl; struct Cyc_Absyn_Structfield; static const int Cyc_Absyn_Loc_n=
0; static const int Cyc_Absyn_Rel_n= 0; struct Cyc_Absyn_Rel_n_struct{ int tag;
struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Abs_n= 1; struct Cyc_Absyn_Abs_n_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Static= 0;
static const int Cyc_Absyn_Abstract= 1; static const int Cyc_Absyn_Public= 2;
static const int Cyc_Absyn_Extern= 3; static const int Cyc_Absyn_ExternC= 4;
struct Cyc_Absyn_Tqual{ int q_const: 1; int q_volatile: 1; int q_restrict: 1; }
; static const int Cyc_Absyn_B1= 0; static const int Cyc_Absyn_B2= 1; static
const int Cyc_Absyn_B4= 2; static const int Cyc_Absyn_B8= 3; static const int
Cyc_Absyn_AnyKind= 0; static const int Cyc_Absyn_MemKind= 1; static const int
Cyc_Absyn_BoxKind= 2; static const int Cyc_Absyn_RgnKind= 3; static const int
Cyc_Absyn_EffKind= 4; static const int Cyc_Absyn_Signed= 0; static const int Cyc_Absyn_Unsigned=
1; struct Cyc_Absyn_Conref{ void* v; } ; static const int Cyc_Absyn_Eq_constr= 0;
struct Cyc_Absyn_Eq_constr_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Forward_constr=
1; struct Cyc_Absyn_Forward_constr_struct{ int tag; struct Cyc_Absyn_Conref* f1;
} ; static const int Cyc_Absyn_No_constr= 0; struct Cyc_Absyn_Tvar{ struct
_tagged_arr* name; int* identity; struct Cyc_Absyn_Conref* kind; } ; static
const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b= 0; struct
Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_PtrInfo{
void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref* nullable; struct Cyc_Absyn_Tqual
tq; struct Cyc_Absyn_Conref* bounds; } ; struct Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt*
name; struct Cyc_Absyn_Tqual tq; void* type; void* rgn; int inject; } ; struct
Cyc_Absyn_FnInfo{ struct Cyc_List_List* tvars; struct Cyc_Core_Opt* effect; void*
ret_typ; struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
cyc_varargs; struct Cyc_List_List* rgn_po; struct Cyc_List_List* attributes; } ;
struct Cyc_Absyn_UnknownTunionInfo{ struct _tuple0* name; int is_xtunion; } ;
static const int Cyc_Absyn_UnknownTunion= 0; struct Cyc_Absyn_UnknownTunion_struct{
int tag; struct Cyc_Absyn_UnknownTunionInfo f1; } ; static const int Cyc_Absyn_KnownTunion=
1; struct Cyc_Absyn_KnownTunion_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
} ; struct Cyc_Absyn_TunionInfo{ void* tunion_info; struct Cyc_List_List* targs;
void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{ struct _tuple0*
tunion_name; struct _tuple0* field_name; int is_xtunion; } ; static const int
Cyc_Absyn_UnknownTunionfield= 0; struct Cyc_Absyn_UnknownTunionfield_struct{ int
tag; struct Cyc_Absyn_UnknownTunionFieldInfo f1; } ; static const int Cyc_Absyn_KnownTunionfield=
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
15u]; extern int Cyc_Absyn_qvar_cmp( struct _tuple0*, struct _tuple0*); extern
int Cyc_Absyn_varlist_cmp( struct Cyc_List_List*, struct Cyc_List_List*); extern
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref( void* x); extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t;
extern void* Cyc_Absyn_sint_t; extern struct _tuple0* Cyc_Absyn_exn_name; extern
struct Cyc_Absyn_Tuniondecl* Cyc_Absyn_exn_tud; extern struct _tuple0* Cyc_Absyn_null_pointer_exn_name;
extern struct _tuple0* Cyc_Absyn_match_exn_name; extern struct Cyc_Absyn_Tunionfield*
Cyc_Absyn_null_pointer_exn_tuf; extern struct Cyc_Absyn_Tunionfield* Cyc_Absyn_match_exn_tuf;
extern void* Cyc_Absyn_tagged_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual tq);
extern struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple0* x,
void* t, struct Cyc_Absyn_Exp* init); extern void* Cyc_Absyn_function_typ(
struct Cyc_List_List* tvs, struct Cyc_Core_Opt* eff_typ, void* ret_typ, struct
Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo* cyc_varargs,
struct Cyc_List_List* rgn_po, struct Cyc_List_List*); struct Cyc_Set_Set; extern
unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; extern unsigned char
Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u]; extern
struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* comp)( void*, void*)); extern int
Cyc_Dict_member( struct Cyc_Dict_Dict* d, void* key); extern struct Cyc_Dict_Dict*
Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key, void* data); extern void*
Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key); extern void* Cyc_Dict_fold(
void*(* f)( void*, void*, void*), struct Cyc_Dict_Dict* d, void* accum); extern
void Cyc_Dict_iter_c( void(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict*
d); extern struct Cyc_Dict_Dict* Cyc_Dict_map( void*(* f)( void*), struct Cyc_Dict_Dict*
d); static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int
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
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init(); struct Cyc_Interface_I;
extern struct Cyc_Interface_I* Cyc_Interface_empty(); extern struct Cyc_Interface_I*
Cyc_Interface_final(); extern struct Cyc_Interface_I* Cyc_Interface_extract(
struct Cyc_Dict_Dict* ae); struct _tuple3{ struct _tagged_arr f1; struct
_tagged_arr f2; } ; extern int Cyc_Interface_is_subinterface( struct Cyc_Interface_I*
i1, struct Cyc_Interface_I* i2, struct _tuple3* info); extern struct Cyc_Interface_I*
Cyc_Interface_merge( struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2,
struct _tuple3* info); extern struct Cyc_Interface_I* Cyc_Interface_merge_list(
struct Cyc_List_List* li, struct Cyc_List_List* linfo); extern struct Cyc_Interface_I*
Cyc_Interface_get_and_merge_list( struct Cyc_Interface_I*(* get)( void*), struct
Cyc_List_List* la, struct Cyc_List_List* linfo); extern void Cyc_Interface_print(
struct Cyc_Interface_I*, struct Cyc_Stdio___sFILE*); extern struct Cyc_Interface_I*
Cyc_Interface_parse( struct Cyc_Stdio___sFILE*); extern void Cyc_Interface_save(
struct Cyc_Interface_I*, struct Cyc_Stdio___sFILE*); extern struct Cyc_Interface_I*
Cyc_Interface_load( struct Cyc_Stdio___sFILE*); struct Cyc_PP_Ppstate; struct
Cyc_PP_Out; struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{ int expand_typedefs: 1;
int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1;
int rewrite_temp_tvars: 1; int print_all_tvars: 1; int print_all_kinds: 1; int
print_using_stmts: 1; int print_externC_stmts: 1; int print_full_evars: 1; int
use_curr_namespace: 1; struct Cyc_List_List* curr_namespace; } ; extern void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params* fs); extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;
extern void Cyc_Absynpp_decllist2file( struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE*
f); extern struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple0*); extern
void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int var_default_init, struct Cyc_List_List*
ds); extern unsigned char Cyc_Lexing_Error[ 10u]; struct Cyc_Lexing_Error_struct{
unsigned char* tag; struct _tagged_arr f1; } ; struct Cyc_Lexing_lexbuf{ void(*
refill_buff)( struct Cyc_Lexing_lexbuf*); void* refill_state; struct _tagged_arr
lex_buffer; int lex_buffer_len; int lex_abs_pos; int lex_start_pos; int
lex_curr_pos; int lex_last_pos; int lex_last_action; int lex_eof_reached; } ;
struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_arr,
int, void*); void* read_fun_state; } ; struct Cyc_Lexing_lex_tables{ struct
_tagged_arr lex_base; struct _tagged_arr lex_backtrk; struct _tagged_arr
lex_default; struct _tagged_arr lex_trans; struct _tagged_arr lex_check; } ;
extern struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE* f);
struct Cyc_Declaration_spec; struct Cyc_Declarator; struct Cyc_Abstractdeclarator;
extern unsigned char Cyc_AbstractDeclarator_tok[ 27u]; struct Cyc_AbstractDeclarator_tok_struct{
unsigned char* tag; struct Cyc_Abstractdeclarator* f1; } ; extern unsigned char
Cyc_AttributeList_tok[ 22u]; struct Cyc_AttributeList_tok_struct{ unsigned char*
tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_Attribute_tok[ 18u];
struct Cyc_Attribute_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_BlockItem_tok[ 18u]; struct Cyc_BlockItem_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Bool_tok[ 13u];
struct Cyc_Bool_tok_struct{ unsigned char* tag; int f1; } ; extern unsigned char
Cyc_Char_tok[ 13u]; struct Cyc_Char_tok_struct{ unsigned char* tag;
unsigned char f1; } ; extern unsigned char Cyc_DeclList_tok[ 17u]; struct Cyc_DeclList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_DeclSpec_tok[
17u]; struct Cyc_DeclSpec_tok_struct{ unsigned char* tag; struct Cyc_Declaration_spec*
f1; } ; extern unsigned char Cyc_DeclaratorExpoptList_tok[ 29u]; struct Cyc_DeclaratorExpoptList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_DeclaratorExpopt_tok[
25u]; struct _tuple4{ struct Cyc_Declarator* f1; struct Cyc_Core_Opt* f2; } ;
struct Cyc_DeclaratorExpopt_tok_struct{ unsigned char* tag; struct _tuple4* f1;
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
f1; } ; extern unsigned char Cyc_FieldPattern_tok[ 21u]; struct _tuple5{ struct
Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; struct Cyc_FieldPattern_tok_struct{
unsigned char* tag; struct _tuple5* f1; } ; extern unsigned char Cyc_FnDecl_tok[
15u]; struct Cyc_FnDecl_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Fndecl*
f1; } ; extern unsigned char Cyc_IdList_tok[ 15u]; struct Cyc_IdList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_InitDeclList_tok[
21u]; struct Cyc_InitDeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_InitDecl_tok[ 17u]; struct _tuple6{ struct Cyc_Declarator*
f1; struct Cyc_Absyn_Exp* f2; } ; struct Cyc_InitDecl_tok_struct{ unsigned char*
tag; struct _tuple6* f1; } ; extern unsigned char Cyc_InitializerList_tok[ 24u];
struct Cyc_InitializerList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Int_tok[ 12u]; struct _tuple7{ void* f1; int f2;
} ; struct Cyc_Int_tok_struct{ unsigned char* tag; struct _tuple7* f1; } ;
extern unsigned char Cyc_Kind_tok[ 13u]; struct Cyc_Kind_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Okay_tok[ 13u];
extern unsigned char Cyc_ParamDeclListBool_tok[ 26u]; struct _tuple8{ struct Cyc_List_List*
f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt* f4; struct Cyc_List_List*
f5; } ; struct Cyc_ParamDeclListBool_tok_struct{ unsigned char* tag; struct
_tuple8* f1; } ; extern unsigned char Cyc_ParamDeclList_tok[ 22u]; struct Cyc_ParamDeclList_tok_struct{
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
extern unsigned char Cyc_QualSpecList_tok[ 21u]; struct _tuple9{ struct Cyc_Absyn_Tqual
f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; } ; struct Cyc_QualSpecList_tok_struct{
unsigned char* tag; struct _tuple9* f1; } ; extern unsigned char Cyc_Rgn_tok[ 12u];
struct Cyc_Rgn_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_Rgnorder_tok[ 17u]; struct Cyc_Rgnorder_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_Scope_tok[
14u]; struct Cyc_Scope_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_Short_tok[ 14u]; struct Cyc_Short_tok_struct{ unsigned char*
tag; short f1; } ; extern unsigned char Cyc_Stmt_tok[ 13u]; struct Cyc_Stmt_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Stmt* f1; } ; extern unsigned char Cyc_StorageClass_tok[
21u]; struct Cyc_StorageClass_tok_struct{ unsigned char* tag; void* f1; } ;
extern unsigned char Cyc_String_tok[ 15u]; struct Cyc_String_tok_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Stringopt_tok[
18u]; struct Cyc_Stringopt_tok_struct{ unsigned char* tag; struct Cyc_Core_Opt*
f1; } ; extern unsigned char Cyc_StructFieldDeclListList_tok[ 32u]; struct Cyc_StructFieldDeclListList_tok_struct{
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
} ; extern int Cyc_String_strptrcmp( struct _tagged_arr* s1, struct _tagged_arr*
s2); extern int Cyc_String_zstrptrcmp( struct _tagged_arr*, struct _tagged_arr*);
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap);
extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_is_function_type(
void* t); extern unsigned char Cyc_Tcdecl_Incompatible[ 17u]; struct Cyc_Tcdecl_Xtunionfielddecl{
struct Cyc_Absyn_Tuniondecl* base; struct Cyc_Absyn_Tunionfield* field; } ;
extern void Cyc_Tcdecl_merr( struct Cyc_Position_Segment* loc, struct
_tagged_arr* msg0, struct _tagged_arr msg1); extern struct Cyc_Absyn_Structdecl*
Cyc_Tcdecl_merge_structdecl( struct Cyc_Absyn_Structdecl* d0, struct Cyc_Absyn_Structdecl*
d1, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg); extern struct
Cyc_Absyn_Uniondecl* Cyc_Tcdecl_merge_uniondecl( struct Cyc_Absyn_Uniondecl* d0,
struct Cyc_Absyn_Uniondecl* d1, struct Cyc_Position_Segment* loc, struct
_tagged_arr* msg); extern struct Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_merge_tuniondecl(
struct Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Enumdecl* Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl* d0, struct Cyc_Absyn_Enumdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Vardecl* Cyc_Tcdecl_merge_vardecl(
struct Cyc_Absyn_Vardecl* d0, struct Cyc_Absyn_Vardecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Typedefdecl* Cyc_Tcdecl_merge_typedefdecl(
struct Cyc_Absyn_Typedefdecl* d0, struct Cyc_Absyn_Typedefdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Tcdecl_Xtunionfielddecl* Cyc_Tcdecl_merge_xtunionfielddecl(
struct Cyc_Tcdecl_Xtunionfielddecl* d0, struct Cyc_Tcdecl_Xtunionfielddecl* d1,
struct Cyc_Position_Segment* loc, struct _tagged_arr* msg); extern void Cyc_Lex_lex_init();
struct Cyc_Interface_Ienv{ struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict*
uniondecls; struct Cyc_Dict_Dict* tuniondecls; struct Cyc_Dict_Dict* enumdecls;
struct Cyc_Dict_Dict* typedefdecls; struct Cyc_Dict_Dict* vardecls; struct Cyc_Dict_Dict*
xtunionfielddecls; } ; struct Cyc_Interface_I{ struct Cyc_Dict_Dict* imports;
struct Cyc_Dict_Dict* exports; } ; static struct Cyc_Interface_Ienv* Cyc_Interface_new_ienv(){
return({ struct Cyc_Interface_Ienv* _temp0=( struct Cyc_Interface_Ienv*)
GC_malloc( sizeof( struct Cyc_Interface_Ienv)); _temp0->structdecls=(( struct
Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp0->uniondecls=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp0->tuniondecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp0->enumdecls=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp0->typedefdecls=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp0->vardecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp0->xtunionfielddecls=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp0;});} static struct Cyc_Interface_Ienv*
Cyc_Interface_lazy_new_ienv(){ static struct Cyc_Interface_Ienv* ie= 0; if( ie
== 0){ ie=( struct Cyc_Interface_Ienv*) Cyc_Interface_new_ienv();} return(
struct Cyc_Interface_Ienv*) _check_null( ie);} static struct Cyc_Interface_Ienv*
Cyc_Interface_skel2i_f( void* ignored){ return Cyc_Interface_new_ienv();} static
struct Cyc_Interface_I* Cyc_Interface_skel2i( struct Cyc_Dict_Dict* skel){
return({ struct Cyc_Interface_I* _temp1=( struct Cyc_Interface_I*) GC_malloc(
sizeof( struct Cyc_Interface_I)); _temp1->imports=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Interface_Ienv*(* f)( void*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)(
Cyc_Interface_skel2i_f, skel); _temp1->exports=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Interface_Ienv*(* f)( void*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)(
Cyc_Interface_skel2i_f, skel); _temp1;});} struct Cyc_Interface_I* Cyc_Interface_empty(){
return({ struct Cyc_Interface_I* _temp2=( struct Cyc_Interface_I*) GC_malloc(
sizeof( struct Cyc_Interface_I)); _temp2->imports=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)(
Cyc_Absyn_varlist_cmp); _temp2->exports=(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp);
_temp2;});} struct Cyc_Interface_I* Cyc_Interface_final(){ struct Cyc_Interface_I*
_temp3= Cyc_Interface_empty(); struct Cyc_Interface_Ienv* _temp4= Cyc_Interface_new_ienv();
struct Cyc_Absyn_Tuniondecl* _temp5=({ struct Cyc_Absyn_Tuniondecl* _temp23=(
struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp23[ 0]=* Cyc_Absyn_exn_tud; _temp23;});( void*)( _temp5->sc=( void*)(( void*)
Cyc_Absyn_Public)); _temp4->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl* data)) Cyc_Dict_insert)(
_temp4->tuniondecls,(* Cyc_Absyn_exn_name).f2, _temp5);{ struct Cyc_Absyn_Tunionfield*
_temp6=({ struct Cyc_Absyn_Tunionfield* _temp22=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp22[ 0]=* Cyc_Absyn_null_pointer_exn_tuf;
_temp22;});( void*)( _temp6->sc=( void*)(( void*) Cyc_Absyn_Public));{ struct
Cyc_Tcdecl_Xtunionfielddecl* _temp7=({ struct Cyc_Tcdecl_Xtunionfielddecl*
_temp21=( struct Cyc_Tcdecl_Xtunionfielddecl*) GC_malloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl));
_temp21->base= _temp5; _temp21->field= _temp6; _temp21;}); _temp4->xtunionfielddecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Tcdecl_Xtunionfielddecl* data)) Cyc_Dict_insert)( _temp4->xtunionfielddecls,(*
Cyc_Absyn_null_pointer_exn_name).f2, _temp7);{ struct Cyc_Absyn_Tunionfield*
_temp8=({ struct Cyc_Absyn_Tunionfield* _temp20=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp20[ 0]=* Cyc_Absyn_match_exn_tuf;
_temp20;});( void*)( _temp8->sc=( void*)(( void*) Cyc_Absyn_Public));{ struct
Cyc_Tcdecl_Xtunionfielddecl* _temp9=({ struct Cyc_Tcdecl_Xtunionfielddecl*
_temp19=( struct Cyc_Tcdecl_Xtunionfielddecl*) GC_malloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl));
_temp19->base= _temp5; _temp19->field= _temp8; _temp19;}); _temp4->xtunionfielddecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Tcdecl_Xtunionfielddecl* data)) Cyc_Dict_insert)( _temp4->xtunionfielddecls,(*
Cyc_Absyn_match_exn_name).f2, _temp9); _temp3->exports=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Interface_Ienv*
data)) Cyc_Dict_insert)( _temp3->exports, 0, _temp4);{ struct Cyc_Interface_Ienv*
_temp10= Cyc_Interface_new_ienv(); struct _tuple0* qmain=({ struct _tuple0*
_temp16=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp16->f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp17=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp17[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp18; _temp18.tag= Cyc_Absyn_Abs_n; _temp18.f1= 0; _temp18;}); _temp17;});
_temp16->f2= _init_tag_arr(( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)),"main", sizeof( unsigned char), 5u); _temp16;}); struct Cyc_Absyn_Vardecl*
_temp11= Cyc_Absyn_new_vardecl( qmain, Cyc_Absyn_function_typ( 0, 0, Cyc_Absyn_sint_t,({
struct Cyc_List_List* _temp12=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp12->hd=( void*)({ struct _tuple1* _temp15=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp15->f1= 0; _temp15->f2= Cyc_Absyn_empty_tqual();
_temp15->f3= Cyc_Absyn_uint_t; _temp15;}); _temp12->tl=({ struct Cyc_List_List*
_temp13=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp13->hd=( void*)({ struct _tuple1* _temp14=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp14->f1= 0; _temp14->f2= Cyc_Absyn_empty_tqual();
_temp14->f3= Cyc_Absyn_tagged_typ( Cyc_Absyn_tagged_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); _temp14;}); _temp13->tl= 0; _temp13;}); _temp12;}), 0,
0, 0, 0), 0);( void*)( _temp11->sc=( void*)(( void*) Cyc_Absyn_Extern)); _temp10->vardecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Vardecl* data)) Cyc_Dict_insert)( _temp10->vardecls,(* qmain).f2,
_temp11); _temp3->imports=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(
_temp3->exports, 0, _temp10); return _temp3;}}}}}} static void Cyc_Interface_err(
struct _tagged_arr msg){({ void(* _temp24)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp27= 0; struct _tagged_arr _temp28= msg; void* _temp25[ 0u]={}; struct
_tagged_arr _temp26={( void*) _temp25,( void*) _temp25,( void*)( _temp25 + 0u)};
_temp24( _temp27, _temp28, _temp26);});} static void Cyc_Interface_fields_err(
struct _tagged_arr sc, struct _tagged_arr t, struct _tuple0* n){ Cyc_Interface_err((
struct _tagged_arr)({ struct _tagged_arr(* _temp29)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp35= _tag_arr("fields of %s %s %s have never been defined",
sizeof( unsigned char), 43u); struct Cyc_Stdio_String_pa_struct _temp38; _temp38.tag=
Cyc_Stdio_String_pa; _temp38.f1=( struct _tagged_arr) sc;{ void* _temp32=( void*)&
_temp38; struct Cyc_Stdio_String_pa_struct _temp37; _temp37.tag= Cyc_Stdio_String_pa;
_temp37.f1=( struct _tagged_arr) t;{ void* _temp33=( void*)& _temp37; struct Cyc_Stdio_String_pa_struct
_temp36; _temp36.tag= Cyc_Stdio_String_pa; _temp36.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
n);{ void* _temp34=( void*)& _temp36; void* _temp30[ 3u]={ _temp32, _temp33,
_temp34}; struct _tagged_arr _temp31={( void*) _temp30,( void*) _temp30,( void*)(
_temp30 + 3u)}; _temp29( _temp35, _temp31);}}}}));} static void Cyc_Interface_body_err(
struct _tagged_arr sc, struct _tuple0* n){ Cyc_Interface_err(( struct
_tagged_arr)({ struct _tagged_arr(* _temp39)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp44= _tag_arr("the body of %s function %s has never been defined",
sizeof( unsigned char), 50u); struct Cyc_Stdio_String_pa_struct _temp46; _temp46.tag=
Cyc_Stdio_String_pa; _temp46.f1=( struct _tagged_arr) sc;{ void* _temp42=( void*)&
_temp46; struct Cyc_Stdio_String_pa_struct _temp45; _temp45.tag= Cyc_Stdio_String_pa;
_temp45.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( n);{ void* _temp43=(
void*)& _temp45; void* _temp40[ 2u]={ _temp42, _temp43}; struct _tagged_arr
_temp41={( void*) _temp40,( void*) _temp40,( void*)( _temp40 + 2u)}; _temp39(
_temp44, _temp41);}}}));} static void Cyc_Interface_static_err( struct
_tagged_arr obj1, struct _tuple0* name1, struct _tagged_arr obj2, struct _tuple0*
name2){ if( obj1.curr !=(( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ Cyc_Interface_err((
struct _tagged_arr)({ struct _tagged_arr(* _temp47)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp54= _tag_arr("declaration of %s %s relies on static %s %s",
sizeof( unsigned char), 44u); struct Cyc_Stdio_String_pa_struct _temp58; _temp58.tag=
Cyc_Stdio_String_pa; _temp58.f1=( struct _tagged_arr) obj1;{ void* _temp50=(
void*)& _temp58; struct Cyc_Stdio_String_pa_struct _temp57; _temp57.tag= Cyc_Stdio_String_pa;
_temp57.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( name1);{ void* _temp51=(
void*)& _temp57; struct Cyc_Stdio_String_pa_struct _temp56; _temp56.tag= Cyc_Stdio_String_pa;
_temp56.f1=( struct _tagged_arr) obj2;{ void* _temp52=( void*)& _temp56; struct
Cyc_Stdio_String_pa_struct _temp55; _temp55.tag= Cyc_Stdio_String_pa; _temp55.f1=(
struct _tagged_arr) Cyc_Absynpp_qvar2string( name2);{ void* _temp53=( void*)&
_temp55; void* _temp48[ 4u]={ _temp50, _temp51, _temp52, _temp53}; struct
_tagged_arr _temp49={( void*) _temp48,( void*) _temp48,( void*)( _temp48 + 4u)};
_temp47( _temp54, _temp49);}}}}}));}} static void Cyc_Interface_abstract_err(
struct _tagged_arr obj1, struct _tuple0* name1, struct _tagged_arr obj2, struct
_tuple0* name2){ if( obj1.curr !=(( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){
Cyc_Interface_err(( struct _tagged_arr)({ struct _tagged_arr(* _temp59)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp66= _tag_arr("declaration of %s %s relies on fields of abstract %s %s",
sizeof( unsigned char), 56u); struct Cyc_Stdio_String_pa_struct _temp70; _temp70.tag=
Cyc_Stdio_String_pa; _temp70.f1=( struct _tagged_arr) obj1;{ void* _temp62=(
void*)& _temp70; struct Cyc_Stdio_String_pa_struct _temp69; _temp69.tag= Cyc_Stdio_String_pa;
_temp69.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( name1);{ void* _temp63=(
void*)& _temp69; struct Cyc_Stdio_String_pa_struct _temp68; _temp68.tag= Cyc_Stdio_String_pa;
_temp68.f1=( struct _tagged_arr) obj2;{ void* _temp64=( void*)& _temp68; struct
Cyc_Stdio_String_pa_struct _temp67; _temp67.tag= Cyc_Stdio_String_pa; _temp67.f1=(
struct _tagged_arr) Cyc_Absynpp_qvar2string( name2);{ void* _temp65=( void*)&
_temp67; void* _temp60[ 4u]={ _temp62, _temp63, _temp64, _temp65}; struct
_tagged_arr _temp61={( void*) _temp60,( void*) _temp60,( void*)( _temp60 + 4u)};
_temp59( _temp66, _temp61);}}}}}));}} struct Cyc_Interface_Seen{ struct Cyc_Dict_Dict*
structs; struct Cyc_Dict_Dict* unions; struct Cyc_Dict_Dict* tunions; } ; static
struct Cyc_Interface_Seen* Cyc_Interface_new_seen(){ return({ struct Cyc_Interface_Seen*
_temp71=( struct Cyc_Interface_Seen*) GC_malloc( sizeof( struct Cyc_Interface_Seen));
_temp71->structs=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp71->unions=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp); _temp71->tunions=(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp71;});} static int Cyc_Interface_check_public_type( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0*
name, void* t); static int Cyc_Interface_check_public_type_list( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0*
name, void*(* f)( void*), struct Cyc_List_List* l){ int _temp72= 1; for( 0; l !=
0; l=(( struct Cyc_List_List*) _check_null( l))->tl){ if( ! Cyc_Interface_check_public_type(
ae, seen, obj, name, f(( void*)(( struct Cyc_List_List*) _check_null( l))->hd))){
_temp72= 0;}} return _temp72;} static int Cyc_Interface_check_public_structdecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Structdecl*
d){ struct _handler_cons _temp73; _push_handler(& _temp73);{ int _temp75= 0; if(
setjmp( _temp73.handler)){ _temp75= 1;} if( ! _temp75){{ int _temp76=(( int(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->structs,(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v);
_npop_handler( 0u); return _temp76;}; _pop_handler();} else{ void* _temp74=(
void*) _exn_thrown; void* _temp78= _temp74; _LL80: if( _temp78 == Cyc_Dict_Absent){
goto _LL81;} else{ goto _LL82;} _LL82: goto _LL83; _LL81: { int _temp84= 1; seen->structs=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)( seen->structs,( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( d->name))->v, _temp84); if( d->fields != 0){ struct Cyc_List_List*
_temp85=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( d->fields))->v;
for( 0; _temp85 != 0; _temp85=(( struct Cyc_List_List*) _check_null( _temp85))->tl){
if( ! Cyc_Interface_check_public_type( ae, seen, _tag_arr("struct", sizeof(
unsigned char), 7u),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v,(
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp85))->hd)->type)){ _temp84= 0;}}} seen->structs=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->structs,(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v, _temp84);
return _temp84;} _LL83:( void) _throw( _temp78); _LL79:;}}} static int Cyc_Interface_check_public_uniondecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Uniondecl*
d){ struct _handler_cons _temp86; _push_handler(& _temp86);{ int _temp88= 0; if(
setjmp( _temp86.handler)){ _temp88= 1;} if( ! _temp88){{ int _temp89=(( int(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->unions,(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v);
_npop_handler( 0u); return _temp89;}; _pop_handler();} else{ void* _temp87=(
void*) _exn_thrown; void* _temp91= _temp87; _LL93: if( _temp91 == Cyc_Dict_Absent){
goto _LL94;} else{ goto _LL95;} _LL95: goto _LL96; _LL94: { int _temp97= 1; seen->unions=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)( seen->unions,( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( d->name))->v, _temp97); if( d->fields != 0){ struct Cyc_List_List*
_temp98=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( d->fields))->v;
for( 0; _temp98 != 0; _temp98=(( struct Cyc_List_List*) _check_null( _temp98))->tl){
if( ! Cyc_Interface_check_public_type( ae, seen, _tag_arr("union", sizeof(
unsigned char), 6u),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v,(
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp98))->hd)->type)){ _temp97= 0;}}} seen->unions=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->unions,(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v, _temp97);
return _temp97;} _LL96:( void) _throw( _temp91); _LL92:;}}} struct _tuple10{
struct Cyc_Absyn_Tqual f1; void* f2; } ; static void* Cyc_Interface_get_type1(
struct _tuple10* x){ return(* x).f2;} static int Cyc_Interface_check_public_tuniondecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Tuniondecl*
d){ struct _handler_cons _temp99; _push_handler(& _temp99);{ int _temp101= 0;
if( setjmp( _temp99.handler)){ _temp101= 1;} if( ! _temp101){{ int _temp102=((
int(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->tunions,
d->name); _npop_handler( 0u); return _temp102;}; _pop_handler();} else{ void*
_temp100=( void*) _exn_thrown; void* _temp104= _temp100; _LL106: if( _temp104 ==
Cyc_Dict_Absent){ goto _LL107;} else{ goto _LL108;} _LL108: goto _LL109; _LL107: {
int _temp110= 1; seen->tunions=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->tunions, d->name,
_temp110); if( d->fields != 0){ struct Cyc_List_List* _temp111=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( d->fields))->v; for( 0; _temp111 != 0;
_temp111=(( struct Cyc_List_List*) _check_null( _temp111))->tl){ if( !(( int(*)(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr
obj, struct _tuple0* name, void*(* f)( struct _tuple10*), struct Cyc_List_List*
l)) Cyc_Interface_check_public_type_list)( ae, seen, _tag_arr("[x]tunion",
sizeof( unsigned char), 10u), d->name, Cyc_Interface_get_type1,(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp111))->hd)->typs)){ _temp110= 0;}}}
seen->tunions=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tuple0* key, int data)) Cyc_Dict_insert)( seen->tunions, d->name, _temp110);
return _temp110;} _LL109:( void) _throw( _temp104); _LL105:;}}} static int Cyc_Interface_check_public_enumdecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Enumdecl*
d){ return 1;} static int Cyc_Interface_check_public_typedefdecl( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Typedefdecl* d){ return
Cyc_Interface_check_public_type( ae, seen,( struct _tagged_arr) _tag_arr( 0u, 0u,
0u), d->name,( void*) d->defn);} static int Cyc_Interface_check_public_vardecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Vardecl*
d){ return Cyc_Interface_check_public_type( ae, seen, _tag_arr("variable",
sizeof( unsigned char), 9u), d->name,( void*) d->type);} static void* Cyc_Interface_get_type2(
struct _tuple1* x){ return(* x).f3;} static struct Cyc_List_List* Cyc_Interface_get_abs_ns(
void* ns){ void* _temp112= ns; struct Cyc_List_List* _temp118; _LL114: if((
unsigned int) _temp112 > 1u?*(( int*) _temp112) == Cyc_Absyn_Abs_n: 0){ _LL119:
_temp118=(( struct Cyc_Absyn_Abs_n_struct*) _temp112)->f1; goto _LL115;} else{
goto _LL116;} _LL116: goto _LL117; _LL115: return _temp118; _LL117:( int) _throw((
void*)({ struct Cyc_Core_InvalidArg_struct* _temp120=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp120[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp121; _temp121.tag= Cyc_Core_InvalidArg; _temp121.f1=
_tag_arr("get_abs_ns", sizeof( unsigned char), 11u); _temp121;}); _temp120;}));
_LL113:;} static int Cyc_Interface_check_public_type( struct Cyc_Dict_Dict* ae,
struct Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0* name,
void* t){ void* _temp122= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp146; void* _temp148; void* _temp150; struct Cyc_Core_Opt* _temp152; struct
Cyc_Core_Opt _temp154; void* _temp155; struct Cyc_Absyn_FnInfo _temp157; struct
Cyc_Absyn_VarargInfo* _temp159; struct Cyc_List_List* _temp161; void* _temp163;
struct Cyc_List_List* _temp165; struct Cyc_Absyn_Structdecl** _temp167; struct
Cyc_Absyn_Structdecl* _temp169; struct Cyc_List_List* _temp170; struct Cyc_Absyn_Uniondecl**
_temp172; struct Cyc_Absyn_Uniondecl* _temp174; struct Cyc_List_List* _temp175;
struct _tuple0* _temp177; struct Cyc_Absyn_TunionInfo _temp179; struct Cyc_List_List*
_temp181; void* _temp183; struct Cyc_Absyn_Tuniondecl* _temp185; struct Cyc_Absyn_TunionFieldInfo
_temp187; struct Cyc_List_List* _temp189; void* _temp191; struct Cyc_Absyn_Tunionfield*
_temp193; struct Cyc_Absyn_Tuniondecl* _temp195; _LL124: if(( unsigned int)
_temp122 > 4u?*(( int*) _temp122) == Cyc_Absyn_PointerType: 0){ _LL147: _temp146=((
struct Cyc_Absyn_PointerType_struct*) _temp122)->f1; _LL149: _temp148=( void*)
_temp146.elt_typ; goto _LL125;} else{ goto _LL126;} _LL126: if(( unsigned int)
_temp122 > 4u?*(( int*) _temp122) == Cyc_Absyn_ArrayType: 0){ _LL151: _temp150=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp122)->f1; goto _LL127;} else{
goto _LL128;} _LL128: if(( unsigned int) _temp122 > 4u?*(( int*) _temp122) ==
Cyc_Absyn_TypedefType: 0){ _LL153: _temp152=(( struct Cyc_Absyn_TypedefType_struct*)
_temp122)->f3; if( _temp152 == 0){ goto _LL130;} else{ _temp154=* _temp152;
_LL156: _temp155=( void*) _temp154.v; goto _LL129;}} else{ goto _LL130;} _LL130:
if(( unsigned int) _temp122 > 4u?*(( int*) _temp122) == Cyc_Absyn_FnType: 0){
_LL158: _temp157=(( struct Cyc_Absyn_FnType_struct*) _temp122)->f1; _LL164:
_temp163=( void*) _temp157.ret_typ; goto _LL162; _LL162: _temp161= _temp157.args;
goto _LL160; _LL160: _temp159= _temp157.cyc_varargs; goto _LL131;} else{ goto
_LL132;} _LL132: if(( unsigned int) _temp122 > 4u?*(( int*) _temp122) == Cyc_Absyn_TupleType:
0){ _LL166: _temp165=(( struct Cyc_Absyn_TupleType_struct*) _temp122)->f1; goto
_LL133;} else{ goto _LL134;} _LL134: if(( unsigned int) _temp122 > 4u?*(( int*)
_temp122) == Cyc_Absyn_StructType: 0){ _LL171: _temp170=(( struct Cyc_Absyn_StructType_struct*)
_temp122)->f2; goto _LL168; _LL168: _temp167=(( struct Cyc_Absyn_StructType_struct*)
_temp122)->f3; if( _temp167 == 0){ goto _LL136;} else{ _temp169=* _temp167; goto
_LL135;}} else{ goto _LL136;} _LL136: if(( unsigned int) _temp122 > 4u?*(( int*)
_temp122) == Cyc_Absyn_UnionType: 0){ _LL176: _temp175=(( struct Cyc_Absyn_UnionType_struct*)
_temp122)->f2; goto _LL173; _LL173: _temp172=(( struct Cyc_Absyn_UnionType_struct*)
_temp122)->f3; if( _temp172 == 0){ goto _LL138;} else{ _temp174=* _temp172; goto
_LL137;}} else{ goto _LL138;} _LL138: if(( unsigned int) _temp122 > 4u?*(( int*)
_temp122) == Cyc_Absyn_EnumType: 0){ _LL178: _temp177=(( struct Cyc_Absyn_EnumType_struct*)
_temp122)->f1; goto _LL139;} else{ goto _LL140;} _LL140: if(( unsigned int)
_temp122 > 4u?*(( int*) _temp122) == Cyc_Absyn_TunionType: 0){ _LL180: _temp179=((
struct Cyc_Absyn_TunionType_struct*) _temp122)->f1; _LL184: _temp183=( void*)
_temp179.tunion_info; if(*(( int*) _temp183) == Cyc_Absyn_KnownTunion){ _LL186:
_temp185=(( struct Cyc_Absyn_KnownTunion_struct*) _temp183)->f1; goto _LL182;}
else{ goto _LL142;} _LL182: _temp181= _temp179.targs; goto _LL141;} else{ goto
_LL142;} _LL142: if(( unsigned int) _temp122 > 4u?*(( int*) _temp122) == Cyc_Absyn_TunionFieldType:
0){ _LL188: _temp187=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp122)->f1;
_LL192: _temp191=( void*) _temp187.field_info; if(*(( int*) _temp191) == Cyc_Absyn_KnownTunionfield){
_LL196: _temp195=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp191)->f1;
goto _LL194; _LL194: _temp193=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp191)->f2; goto _LL190;} else{ goto _LL144;} _LL190: _temp189= _temp187.targs;
goto _LL143;} else{ goto _LL144;} _LL144: goto _LL145; _LL125: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp148); _LL127: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp150); _LL129: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp155); _LL131: { int b=(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0*
name, void*(* f)( struct _tuple1*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name, Cyc_Interface_get_type2, _temp161)? Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp163): 0; if( _temp159 != 0){ void* _temp199; struct
Cyc_Absyn_VarargInfo _temp197=*(( struct Cyc_Absyn_VarargInfo*) _check_null(
_temp159)); _LL200: _temp199=( void*) _temp197.type; goto _LL198; _LL198: b= Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp199);} return b;} _LL133: return(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0*
name, void*(* f)( struct _tuple10*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name, Cyc_Interface_get_type1, _temp165); _LL135: if(( void*)
_temp169->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err( obj, name,
_tag_arr("struct", sizeof( unsigned char), 7u),( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp169->name))->v); return 0;} return Cyc_Interface_check_public_type_list(
ae, seen, obj, name, Cyc_Core_identity, _temp170)? Cyc_Interface_check_public_structdecl(
ae, seen, _temp169): 0; _LL137: if(( void*) _temp174->sc ==( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, name, _tag_arr("union", sizeof( unsigned char), 6u),(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp174->name))->v);
return 0;} return Cyc_Interface_check_public_type_list( ae, seen, obj, name, Cyc_Core_identity,
_temp175)? Cyc_Interface_check_public_uniondecl( ae, seen, _temp174): 0; _LL139: {
struct _tuple0 _temp203; struct _tagged_arr* _temp204; void* _temp206; struct
_tuple0* _temp201= _temp177; _temp203=* _temp201; _LL207: _temp206= _temp203.f1;
goto _LL205; _LL205: _temp204= _temp203.f2; goto _LL202; _LL202: { struct Cyc_List_List*
_temp208= Cyc_Interface_get_abs_ns( _temp206); struct Cyc_Absyn_Enumdecl* ed;{
struct _handler_cons _temp209; _push_handler(& _temp209);{ int _temp211= 0; if(
setjmp( _temp209.handler)){ _temp211= 1;} if( ! _temp211){{ struct Cyc_Tcenv_Genv*
_temp212=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( ae, _temp208); ed=*(( struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp212->enumdecls,
_temp204);}; _pop_handler();} else{ void* _temp210=( void*) _exn_thrown; void*
_temp214= _temp210; _LL216: if( _temp214 == Cyc_Dict_Absent){ goto _LL217;}
else{ goto _LL218;} _LL218: goto _LL219; _LL217:( int) _throw(( void*)({ struct
Cyc_Core_InvalidArg_struct* _temp220=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp220[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp221; _temp221.tag= Cyc_Core_InvalidArg; _temp221.f1=(
struct _tagged_arr)({ struct _tagged_arr(* _temp222)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp226= _tag_arr("check_public_type (can't find enum %s)",
sizeof( unsigned char), 39u); struct Cyc_Stdio_String_pa_struct _temp227;
_temp227.tag= Cyc_Stdio_String_pa; _temp227.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp177);{ void* _temp225=( void*)& _temp227; void* _temp223[ 1u]={ _temp225};
struct _tagged_arr _temp224={( void*) _temp223,( void*) _temp223,( void*)(
_temp223 + 1u)}; _temp222( _temp226, _temp224);}}); _temp221;}); _temp220;}));
_LL219:( void) _throw( _temp214); _LL215:;}}} if(( void*) ed->sc ==( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, _temp177, _tag_arr("enum", sizeof( unsigned char),
5u), ed->name); return 0;} return 1;}} _LL141: { struct _tuple0 _temp230; struct
_tagged_arr* _temp231; void* _temp233; struct _tuple0* _temp228= _temp185->name;
_temp230=* _temp228; _LL234: _temp233= _temp230.f1; goto _LL232; _LL232:
_temp231= _temp230.f2; goto _LL229; _LL229: { struct Cyc_List_List* _temp235=
Cyc_Interface_get_abs_ns( _temp233); struct Cyc_Absyn_Tuniondecl* tud;{ struct
_handler_cons _temp236; _push_handler(& _temp236);{ int _temp238= 0; if( setjmp(
_temp236.handler)){ _temp238= 1;} if( ! _temp238){{ struct Cyc_Tcenv_Genv*
_temp239=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( ae, _temp235); tud=*(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp239->tuniondecls,
_temp231);}; _pop_handler();} else{ void* _temp237=( void*) _exn_thrown; void*
_temp241= _temp237; _LL243: if( _temp241 == Cyc_Dict_Absent){ goto _LL244;}
else{ goto _LL245;} _LL245: goto _LL246; _LL244:( int) _throw(( void*)({ struct
Cyc_Core_InvalidArg_struct* _temp247=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp247[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp248; _temp248.tag= Cyc_Core_InvalidArg; _temp248.f1=(
struct _tagged_arr)({ struct _tagged_arr(* _temp249)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp253= _tag_arr("check_public_type (can't find [x]tunion %s)",
sizeof( unsigned char), 44u); struct Cyc_Stdio_String_pa_struct _temp254;
_temp254.tag= Cyc_Stdio_String_pa; _temp254.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp185->name);{ void* _temp252=( void*)& _temp254; void* _temp250[ 1u]={
_temp252}; struct _tagged_arr _temp251={( void*) _temp250,( void*) _temp250,(
void*)( _temp250 + 1u)}; _temp249( _temp253, _temp251);}}); _temp248;});
_temp247;})); _LL246:( void) _throw( _temp241); _LL242:;}}} if(( void*) tud->sc
==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err( obj, name, _tag_arr("[x]tunion",
sizeof( unsigned char), 10u), tud->name); return 0;} return Cyc_Interface_check_public_type_list(
ae, seen, obj, name, Cyc_Core_identity, _temp181)? Cyc_Interface_check_public_tuniondecl(
ae, seen, tud): 0;}} _LL143: { struct _tuple0 _temp257; struct _tagged_arr*
_temp258; void* _temp260; struct _tuple0* _temp255= _temp195->name; _temp257=*
_temp255; _LL261: _temp260= _temp257.f1; goto _LL259; _LL259: _temp258= _temp257.f2;
goto _LL256; _LL256: { struct Cyc_List_List* _temp262= Cyc_Interface_get_abs_ns(
_temp260); struct Cyc_Absyn_Tuniondecl* tud;{ struct _handler_cons _temp263;
_push_handler(& _temp263);{ int _temp265= 0; if( setjmp( _temp263.handler)){
_temp265= 1;} if( ! _temp265){{ struct Cyc_Tcenv_Genv* _temp266=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( ae,
_temp262); tud=*(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp266->tuniondecls, _temp258);};
_pop_handler();} else{ void* _temp264=( void*) _exn_thrown; void* _temp268=
_temp264; _LL270: if( _temp268 == Cyc_Dict_Absent){ goto _LL271;} else{ goto
_LL272;} _LL272: goto _LL273; _LL271:( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp274=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp274[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp275; _temp275.tag= Cyc_Core_InvalidArg;
_temp275.f1=( struct _tagged_arr)({ struct _tagged_arr(* _temp276)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp280= _tag_arr("check_public_type (can't find [x]tunion %s and search its fields)",
sizeof( unsigned char), 66u); struct Cyc_Stdio_String_pa_struct _temp281;
_temp281.tag= Cyc_Stdio_String_pa; _temp281.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp195->name);{ void* _temp279=( void*)& _temp281; void* _temp277[ 1u]={
_temp279}; struct _tagged_arr _temp278={( void*) _temp277,( void*) _temp277,(
void*)( _temp277 + 1u)}; _temp276( _temp280, _temp278);}}); _temp275;});
_temp274;})); _LL273:( void) _throw( _temp268); _LL269:;}}} if( tud->fields == 0){(
int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp282=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp282[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp283; _temp283.tag= Cyc_Core_InvalidArg; _temp283.f1=(
struct _tagged_arr)({ struct _tagged_arr(* _temp284)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp288= _tag_arr("check_public_type ([x]tunion %s has no fields)",
sizeof( unsigned char), 47u); struct Cyc_Stdio_String_pa_struct _temp289;
_temp289.tag= Cyc_Stdio_String_pa; _temp289.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
tud->name);{ void* _temp287=( void*)& _temp289; void* _temp285[ 1u]={ _temp287};
struct _tagged_arr _temp286={( void*) _temp285,( void*) _temp285,( void*)(
_temp285 + 1u)}; _temp284( _temp288, _temp286);}}); _temp283;}); _temp282;}));}{
struct Cyc_Absyn_Tunionfield* tuf1= 0;{ struct Cyc_List_List* _temp290=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( tud->fields))->v; for( 0;
_temp290 != 0; _temp290=(( struct Cyc_List_List*) _check_null( _temp290))->tl){
if( Cyc_String_strptrcmp((* _temp193->name).f2,(*(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp290))->hd)->name).f2) == 0){ tuf1=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp290))->hd); break;}}} if( tuf1 == 0){( int) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp291=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp291[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp292; _temp292.tag= Cyc_Core_InvalidArg; _temp292.f1=(
struct _tagged_arr)({ struct _tagged_arr(* _temp293)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp297= _tag_arr("check_public_type (can't find [x]tunionfield %s)",
sizeof( unsigned char), 49u); struct Cyc_Stdio_String_pa_struct _temp298;
_temp298.tag= Cyc_Stdio_String_pa; _temp298.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp193->name);{ void* _temp296=( void*)& _temp298; void* _temp294[ 1u]={
_temp296}; struct _tagged_arr _temp295={( void*) _temp294,( void*) _temp294,(
void*)( _temp294 + 1u)}; _temp293( _temp297, _temp295);}}); _temp292;});
_temp291;}));}{ struct Cyc_Absyn_Tunionfield* tuf=( struct Cyc_Absyn_Tunionfield*)
_check_null( tuf1); if(( void*) tud->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err(
obj, name, _tag_arr("[x]tunion", sizeof( unsigned char), 10u), tud->name);
return 0;} if(( void*) tud->sc ==( void*) Cyc_Absyn_Abstract){ Cyc_Interface_abstract_err(
obj, name, _tag_arr("[x]tunion", sizeof( unsigned char), 10u), tud->name);
return 0;} if(( void*) tuf->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err(
obj, name,( struct _tagged_arr)({ struct _tagged_arr(* _temp299)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp303= _tag_arr("field %s of", sizeof( unsigned char), 12u); struct Cyc_Stdio_String_pa_struct
_temp304; _temp304.tag= Cyc_Stdio_String_pa; _temp304.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( tuf->name);{ void* _temp302=( void*)& _temp304; void*
_temp300[ 1u]={ _temp302}; struct _tagged_arr _temp301={( void*) _temp300,( void*)
_temp300,( void*)( _temp300 + 1u)}; _temp299( _temp303, _temp301);}}), tud->name);
return 0;} return Cyc_Interface_check_public_type_list( ae, seen, obj, name, Cyc_Core_identity,
_temp189)? Cyc_Interface_check_public_tuniondecl( ae, seen, tud): 0;}}}} _LL145:
return 1; _LL123:;} struct _tuple11{ struct Cyc_Interface_Ienv* f1; struct Cyc_Interface_Ienv*
f2; int f3; struct Cyc_Dict_Dict* f4; struct Cyc_Interface_Seen* f5; struct Cyc_Interface_I*
f6; } ; static void Cyc_Interface_extract_structdecl( struct _tuple11* env,
struct _tagged_arr* x, struct Cyc_Absyn_Structdecl** dp){ struct _tuple11
_temp307; struct Cyc_Interface_Seen* _temp308; struct Cyc_Dict_Dict* _temp310;
int _temp312; struct Cyc_Interface_Ienv* _temp314; struct Cyc_Interface_Ienv*
_temp316; struct _tuple11* _temp305= env; _temp307=* _temp305; _LL317: _temp316=
_temp307.f1; goto _LL315; _LL315: _temp314= _temp307.f2; goto _LL313; _LL313:
_temp312= _temp307.f3; goto _LL311; _LL311: _temp310= _temp307.f4; goto _LL309;
_LL309: _temp308= _temp307.f5; goto _LL306; _LL306: { struct Cyc_Absyn_Structdecl*
_temp318=* dp; void* _temp319=( void*) _temp318->sc; _LL321: if( _temp319 ==(
void*) Cyc_Absyn_Static){ goto _LL322;} else{ goto _LL323;} _LL323: if( _temp319
==( void*) Cyc_Absyn_Abstract){ goto _LL324;} else{ goto _LL325;} _LL325: if(
_temp319 ==( void*) Cyc_Absyn_Public){ goto _LL326;} else{ goto _LL327;} _LL327:
if( _temp319 ==( void*) Cyc_Absyn_ExternC){ goto _LL328;} else{ goto _LL329;}
_LL329: if( _temp319 ==( void*) Cyc_Absyn_Extern){ goto _LL330;} else{ goto
_LL320;} _LL322: if( _temp312? _temp318->fields == 0: 0){ Cyc_Interface_fields_err(
_tag_arr("static", sizeof( unsigned char), 7u), _tag_arr("struct", sizeof(
unsigned char), 7u),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp318->name))->v);} goto _LL320; _LL324: if( _temp318->fields == 0){ if(
_temp312){ Cyc_Interface_fields_err( _tag_arr("abstract", sizeof( unsigned char),
9u), _tag_arr("struct", sizeof( unsigned char), 7u),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp318->name))->v);}} else{ _temp318=({ struct Cyc_Absyn_Structdecl*
_temp331=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp331[ 0]=* _temp318; _temp331;}); _temp318->fields= 0;} if( Cyc_Interface_check_public_structdecl(
_temp310, _temp308, _temp318)){ _temp314->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)( _temp314->structdecls, x, _temp318);} goto _LL320;
_LL326: if( _temp318->fields == 0){ Cyc_Interface_fields_err( _tag_arr("public",
sizeof( unsigned char), 7u), _tag_arr("struct", sizeof( unsigned char), 7u),(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp318->name))->v);
_temp318=({ struct Cyc_Absyn_Structdecl* _temp332=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp332[ 0]=* _temp318;
_temp332;});( void*)( _temp318->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if(
Cyc_Interface_check_public_structdecl( _temp310, _temp308, _temp318)){ _temp314->structdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)( _temp314->structdecls, x,
_temp318);} goto _LL320; _LL328: goto _LL330; _LL330: if( Cyc_Interface_check_public_structdecl(
_temp310, _temp308, _temp318)){ _temp316->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)( _temp316->structdecls, x, _temp318);} goto _LL320;
_LL320:;}} static void Cyc_Interface_extract_uniondecl( struct _tuple11* env,
struct _tagged_arr* x, struct Cyc_Absyn_Uniondecl** dp){ struct _tuple11
_temp335; struct Cyc_Interface_Seen* _temp336; struct Cyc_Dict_Dict* _temp338;
int _temp340; struct Cyc_Interface_Ienv* _temp342; struct Cyc_Interface_Ienv*
_temp344; struct _tuple11* _temp333= env; _temp335=* _temp333; _LL345: _temp344=
_temp335.f1; goto _LL343; _LL343: _temp342= _temp335.f2; goto _LL341; _LL341:
_temp340= _temp335.f3; goto _LL339; _LL339: _temp338= _temp335.f4; goto _LL337;
_LL337: _temp336= _temp335.f5; goto _LL334; _LL334: { struct Cyc_Absyn_Uniondecl*
_temp346=* dp; void* _temp347=( void*) _temp346->sc; _LL349: if( _temp347 ==(
void*) Cyc_Absyn_Static){ goto _LL350;} else{ goto _LL351;} _LL351: if( _temp347
==( void*) Cyc_Absyn_Abstract){ goto _LL352;} else{ goto _LL353;} _LL353: if(
_temp347 ==( void*) Cyc_Absyn_Public){ goto _LL354;} else{ goto _LL355;} _LL355:
if( _temp347 ==( void*) Cyc_Absyn_ExternC){ goto _LL356;} else{ goto _LL357;}
_LL357: if( _temp347 ==( void*) Cyc_Absyn_Extern){ goto _LL358;} else{ goto
_LL348;} _LL350: if( _temp340? _temp346->fields == 0: 0){ Cyc_Interface_fields_err(
_tag_arr("static", sizeof( unsigned char), 7u), _tag_arr("union", sizeof(
unsigned char), 6u),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp346->name))->v);} goto _LL348; _LL352: if( _temp346->fields == 0){ if(
_temp340){ Cyc_Interface_fields_err( _tag_arr("abstract", sizeof( unsigned char),
9u), _tag_arr("union", sizeof( unsigned char), 6u),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp346->name))->v);}} else{ _temp346=({ struct Cyc_Absyn_Uniondecl*
_temp359=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp359[ 0]=* _temp346; _temp359;}); _temp346->fields= 0;} if( Cyc_Interface_check_public_uniondecl(
_temp338, _temp336, _temp346)){ _temp342->uniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)( _temp342->uniondecls, x, _temp346);} goto _LL348;
_LL354: if( _temp346->fields == 0){ Cyc_Interface_fields_err( _tag_arr("public",
sizeof( unsigned char), 7u), _tag_arr("union", sizeof( unsigned char), 6u),(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp346->name))->v);
_temp346=({ struct Cyc_Absyn_Uniondecl* _temp360=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp360[ 0]=* _temp346;
_temp360;});( void*)( _temp346->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if(
Cyc_Interface_check_public_uniondecl( _temp338, _temp336, _temp346)){ _temp342->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)( _temp342->uniondecls, x,
_temp346);} goto _LL348; _LL356: goto _LL358; _LL358: if( Cyc_Interface_check_public_uniondecl(
_temp338, _temp336, _temp346)){ _temp344->uniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)( _temp344->uniondecls, x, _temp346);} goto _LL348;
_LL348:;}} static void Cyc_Interface_extract_enumdecl( struct _tuple11* env,
struct _tagged_arr* x, struct Cyc_Absyn_Enumdecl** dp){ struct _tuple11 _temp363;
struct Cyc_Interface_Seen* _temp364; struct Cyc_Dict_Dict* _temp366; int
_temp368; struct Cyc_Interface_Ienv* _temp370; struct Cyc_Interface_Ienv*
_temp372; struct _tuple11* _temp361= env; _temp363=* _temp361; _LL373: _temp372=
_temp363.f1; goto _LL371; _LL371: _temp370= _temp363.f2; goto _LL369; _LL369:
_temp368= _temp363.f3; goto _LL367; _LL367: _temp366= _temp363.f4; goto _LL365;
_LL365: _temp364= _temp363.f5; goto _LL362; _LL362: { struct Cyc_Absyn_Enumdecl*
_temp374=* dp; void* _temp375=( void*) _temp374->sc; _LL377: if( _temp375 ==(
void*) Cyc_Absyn_Static){ goto _LL378;} else{ goto _LL379;} _LL379: if( _temp375
==( void*) Cyc_Absyn_Abstract){ goto _LL380;} else{ goto _LL381;} _LL381: if(
_temp375 ==( void*) Cyc_Absyn_Public){ goto _LL382;} else{ goto _LL383;} _LL383:
if( _temp375 ==( void*) Cyc_Absyn_ExternC){ goto _LL384;} else{ goto _LL385;}
_LL385: if( _temp375 ==( void*) Cyc_Absyn_Extern){ goto _LL386;} else{ goto
_LL376;} _LL378: if( _temp368? _temp374->fields == 0: 0){ Cyc_Interface_fields_err(
_tag_arr("static", sizeof( unsigned char), 7u), _tag_arr("enum", sizeof(
unsigned char), 5u), _temp374->name);} goto _LL376; _LL380: if( _temp374->fields
== 0){ if( _temp368){ Cyc_Interface_fields_err( _tag_arr("abstract", sizeof(
unsigned char), 9u), _tag_arr("enum", sizeof( unsigned char), 5u), _temp374->name);}}
else{ _temp374=({ struct Cyc_Absyn_Enumdecl* _temp387=( struct Cyc_Absyn_Enumdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl)); _temp387[ 0]=* _temp374;
_temp387;}); _temp374->fields= 0;} if( Cyc_Interface_check_public_enumdecl(
_temp366, _temp364, _temp374)){ _temp370->enumdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Enumdecl*
data)) Cyc_Dict_insert)( _temp370->enumdecls, x, _temp374);} goto _LL376; _LL382:
if( _temp374->fields == 0){ Cyc_Interface_fields_err( _tag_arr("public", sizeof(
unsigned char), 7u), _tag_arr("enum", sizeof( unsigned char), 5u), _temp374->name);
_temp374=({ struct Cyc_Absyn_Enumdecl* _temp388=( struct Cyc_Absyn_Enumdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl)); _temp388[ 0]=* _temp374;
_temp388;});( void*)( _temp374->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if(
Cyc_Interface_check_public_enumdecl( _temp366, _temp364, _temp374)){ _temp370->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Enumdecl* data)) Cyc_Dict_insert)( _temp370->enumdecls, x,
_temp374);} goto _LL376; _LL384: goto _LL386; _LL386: if( Cyc_Interface_check_public_enumdecl(
_temp366, _temp364, _temp374)){ _temp372->enumdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Enumdecl*
data)) Cyc_Dict_insert)( _temp372->enumdecls, x, _temp374);} goto _LL376; _LL376:;}}
static void Cyc_Interface_extract_xtunionfielddecl( struct Cyc_Interface_I* i,
struct Cyc_Absyn_Tuniondecl* d, struct Cyc_Absyn_Tunionfield* f){ struct Cyc_List_List*
_temp389= 0;{ void* _temp390=(* f->name).f1; struct Cyc_List_List* _temp398;
struct Cyc_List_List* _temp400; _LL392: if(( unsigned int) _temp390 > 1u?*(( int*)
_temp390) == Cyc_Absyn_Rel_n: 0){ _LL399: _temp398=(( struct Cyc_Absyn_Rel_n_struct*)
_temp390)->f1; goto _LL393;} else{ goto _LL394;} _LL394: if(( unsigned int)
_temp390 > 1u?*(( int*) _temp390) == Cyc_Absyn_Abs_n: 0){ _LL401: _temp400=((
struct Cyc_Absyn_Abs_n_struct*) _temp390)->f1; goto _LL395;} else{ goto _LL396;}
_LL396: goto _LL397; _LL393: _temp400= _temp398; goto _LL395; _LL395: _temp389=
_temp400; goto _LL391; _LL397: goto _LL391; _LL391:;}{ struct Cyc_Dict_Dict**
dict;{ void* _temp402=( void*) f->sc; _LL404: if( _temp402 ==( void*) Cyc_Absyn_Static){
goto _LL405;} else{ goto _LL406;} _LL406: if( _temp402 ==( void*) Cyc_Absyn_Extern){
goto _LL407;} else{ goto _LL408;} _LL408: if( _temp402 ==( void*) Cyc_Absyn_Public){
goto _LL409;} else{ goto _LL410;} _LL410: goto _LL411; _LL405: return; _LL407:
dict=& i->imports; goto _LL403; _LL409: dict=& i->exports; goto _LL403; _LL411:(
int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp412=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp412[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp413; _temp413.tag= Cyc_Core_InvalidArg; _temp413.f1=
_tag_arr("add_xtunionfielddecl", sizeof( unsigned char), 21u); _temp413;});
_temp412;})); _LL403:;}{ struct Cyc_Interface_Ienv* env;{ struct _handler_cons
_temp414; _push_handler(& _temp414);{ int _temp416= 0; if( setjmp( _temp414.handler)){
_temp416= 1;} if( ! _temp416){ env=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(* dict, _temp389);; _pop_handler();}
else{ void* _temp415=( void*) _exn_thrown; void* _temp418= _temp415; _LL420: if(
_temp418 == Cyc_Dict_Absent){ goto _LL421;} else{ goto _LL422;} _LL422: goto
_LL423; _LL421: env= Cyc_Interface_new_ienv();* dict=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Interface_Ienv*
data)) Cyc_Dict_insert)(* dict, _temp389, env); goto _LL419; _LL423:( void)
_throw( _temp418); _LL419:;}}} env->xtunionfielddecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Tcdecl_Xtunionfielddecl*
data)) Cyc_Dict_insert)( env->xtunionfielddecls,(* f->name).f2,({ struct Cyc_Tcdecl_Xtunionfielddecl*
_temp424=( struct Cyc_Tcdecl_Xtunionfielddecl*) GC_malloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl));
_temp424->base= d; _temp424->field= f; _temp424;}));}}} static void Cyc_Interface_extract_tuniondecl(
struct _tuple11* env, struct _tagged_arr* x, struct Cyc_Absyn_Tuniondecl** dp){
struct _tuple11 _temp427; struct Cyc_Interface_I* _temp428; struct Cyc_Interface_Seen*
_temp430; struct Cyc_Dict_Dict* _temp432; int _temp434; struct Cyc_Interface_Ienv*
_temp436; struct Cyc_Interface_Ienv* _temp438; struct _tuple11* _temp425= env;
_temp427=* _temp425; _LL439: _temp438= _temp427.f1; goto _LL437; _LL437:
_temp436= _temp427.f2; goto _LL435; _LL435: _temp434= _temp427.f3; goto _LL433;
_LL433: _temp432= _temp427.f4; goto _LL431; _LL431: _temp430= _temp427.f5; goto
_LL429; _LL429: _temp428= _temp427.f6; goto _LL426; _LL426: { struct Cyc_Absyn_Tuniondecl*
_temp440=* dp; void* _temp441=( void*) _temp440->sc; _LL443: if( _temp441 ==(
void*) Cyc_Absyn_Static){ goto _LL444;} else{ goto _LL445;} _LL445: if( _temp441
==( void*) Cyc_Absyn_Abstract){ goto _LL446;} else{ goto _LL447;} _LL447: if(
_temp441 ==( void*) Cyc_Absyn_Public){ goto _LL448;} else{ goto _LL449;} _LL449:
if( _temp441 ==( void*) Cyc_Absyn_ExternC){ goto _LL450;} else{ goto _LL451;}
_LL451: if( _temp441 ==( void*) Cyc_Absyn_Extern){ goto _LL452;} else{ goto
_LL442;} _LL444: if(( ! _temp440->is_xtunion? _temp434: 0)? _temp440->fields ==
0: 0){ Cyc_Interface_fields_err( _tag_arr("static", sizeof( unsigned char), 7u),
_tag_arr("tunion", sizeof( unsigned char), 7u), _temp440->name);} goto _LL442;
_LL446: if( _temp440->fields == 0){ if( ! _temp440->is_xtunion? _temp434: 0){
Cyc_Interface_fields_err( _tag_arr("abstract", sizeof( unsigned char), 9u),
_tag_arr("tunion", sizeof( unsigned char), 7u), _temp440->name);}} else{
_temp440=({ struct Cyc_Absyn_Tuniondecl* _temp453=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp453[ 0]=* _temp440;
_temp453;}); _temp440->fields= 0;} if( Cyc_Interface_check_public_tuniondecl(
_temp432, _temp430, _temp440)){ _temp436->tuniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl*
data)) Cyc_Dict_insert)( _temp436->tuniondecls, x, _temp440);} goto _LL442;
_LL448: _temp440=({ struct Cyc_Absyn_Tuniondecl* _temp454=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp454[ 0]=* _temp440;
_temp454;}); if( ! _temp440->is_xtunion? _temp440->fields == 0: 0){ Cyc_Interface_fields_err(
_tag_arr("public", sizeof( unsigned char), 7u), _tag_arr("tunion", sizeof(
unsigned char), 7u), _temp440->name);( void*)( _temp440->sc=( void*)(( void*)
Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_tuniondecl( _temp432,
_temp430, _temp440)){ if( _temp440->is_xtunion? _temp440->fields != 0: 0){
struct Cyc_List_List* _temp455=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp440->fields))->v; _temp440->fields= 0;{ struct Cyc_List_List*
_temp456= _temp455; for( 0; _temp456 != 0; _temp456=(( struct Cyc_List_List*)
_check_null( _temp456))->tl){ Cyc_Interface_extract_xtunionfielddecl( _temp428,
_temp440,( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp456))->hd);}}} _temp436->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl* data))
Cyc_Dict_insert)( _temp436->tuniondecls, x, _temp440);} goto _LL442; _LL450:(
int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp457=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp457[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp458; _temp458.tag= Cyc_Core_InvalidArg; _temp458.f1=
_tag_arr("extract_tuniondecl", sizeof( unsigned char), 19u); _temp458;});
_temp457;})); _LL452: if( Cyc_Interface_check_public_tuniondecl( _temp432,
_temp430, _temp440)){ if( _temp440->is_xtunion? _temp440->fields != 0: 0){
_temp440=({ struct Cyc_Absyn_Tuniondecl* _temp459=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp459[ 0]=* _temp440;
_temp459;});{ struct Cyc_List_List* _temp460=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( _temp440->fields))->v; _temp440->fields= 0;{ struct
Cyc_List_List* _temp461= _temp460; for( 0; _temp461 != 0; _temp461=(( struct Cyc_List_List*)
_check_null( _temp461))->tl){ Cyc_Interface_extract_xtunionfielddecl( _temp428,
_temp440,( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp461))->hd);}}}} _temp438->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl* data))
Cyc_Dict_insert)( _temp438->tuniondecls, x, _temp440);} goto _LL442; _LL442:;}}
static void Cyc_Interface_extract_typedef( struct _tuple11* env, struct
_tagged_arr* x, struct Cyc_Absyn_Typedefdecl* d){ struct _tuple11 _temp464;
struct Cyc_Interface_Seen* _temp465; struct Cyc_Dict_Dict* _temp467; struct Cyc_Interface_Ienv*
_temp469; struct _tuple11* _temp462= env; _temp464=* _temp462; _LL470: _temp469=
_temp464.f2; goto _LL468; _LL468: _temp467= _temp464.f4; goto _LL466; _LL466:
_temp465= _temp464.f5; goto _LL463; _LL463: if( Cyc_Interface_check_public_typedefdecl(
_temp467, _temp465, d)){ _temp469->typedefdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Typedefdecl*
data)) Cyc_Dict_insert)( _temp469->typedefdecls, x, d);}} static void Cyc_Interface_extract_ordinarie(
struct _tuple11* env, struct _tagged_arr* x, struct _tuple7* v){ struct _tuple11
_temp473; struct Cyc_Interface_Seen* _temp474; struct Cyc_Dict_Dict* _temp476;
int _temp478; struct Cyc_Interface_Ienv* _temp480; struct Cyc_Interface_Ienv*
_temp482; struct _tuple11* _temp471= env; _temp473=* _temp471; _LL483: _temp482=
_temp473.f1; goto _LL481; _LL481: _temp480= _temp473.f2; goto _LL479; _LL479:
_temp478= _temp473.f3; goto _LL477; _LL477: _temp476= _temp473.f4; goto _LL475;
_LL475: _temp474= _temp473.f5; goto _LL472; _LL472: { void* _temp484=(* v).f1;
void* _temp485= _temp484; void* _temp491; _LL487: if(*(( int*) _temp485) == Cyc_Tcenv_VarRes){
_LL492: _temp491=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp485)->f1; goto
_LL488;} else{ goto _LL489;} _LL489: goto _LL490; _LL488:{ void* _temp493=
_temp491; struct Cyc_Absyn_Fndecl* _temp501; struct Cyc_Absyn_Vardecl* _temp503;
_LL495: if(( unsigned int) _temp493 > 1u?*(( int*) _temp493) == Cyc_Absyn_Funname_b:
0){ _LL502: _temp501=(( struct Cyc_Absyn_Funname_b_struct*) _temp493)->f1; goto
_LL496;} else{ goto _LL497;} _LL497: if(( unsigned int) _temp493 > 1u?*(( int*)
_temp493) == Cyc_Absyn_Global_b: 0){ _LL504: _temp503=(( struct Cyc_Absyn_Global_b_struct*)
_temp493)->f1; goto _LL498;} else{ goto _LL499;} _LL499: goto _LL500; _LL496: {
struct Cyc_Absyn_Vardecl* _temp505=({ struct Cyc_Absyn_Vardecl* _temp506=(
struct Cyc_Absyn_Vardecl*) GC_malloc( sizeof( struct Cyc_Absyn_Vardecl));
_temp506->sc=( void*)(( void*) _temp501->sc); _temp506->name= _temp501->name;
_temp506->tq=({ struct Cyc_Absyn_Tqual _temp507; _temp507.q_const= 0; _temp507.q_volatile=
0; _temp507.q_restrict= 0; _temp507;}); _temp506->type=( void*)(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp501->cached_typ))->v); _temp506->initializer=
0; _temp506->rgn= 0; _temp506->attributes= 0; _temp506;}); _temp478= 0; _temp503=
_temp505; goto _LL498;} _LL498: if( _temp503->initializer != 0){ _temp503=({
struct Cyc_Absyn_Vardecl* _temp508=( struct Cyc_Absyn_Vardecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Vardecl)); _temp508[ 0]=* _temp503; _temp508;});
_temp503->initializer= 0;}{ void* _temp509=( void*) _temp503->sc; _LL511: if(
_temp509 ==( void*) Cyc_Absyn_Static){ goto _LL512;} else{ goto _LL513;} _LL513:
if( _temp509 ==( void*) Cyc_Absyn_Abstract){ goto _LL514;} else{ goto _LL515;}
_LL515: if( _temp509 ==( void*) Cyc_Absyn_Public){ goto _LL516;} else{ goto
_LL517;} _LL517: if( _temp509 ==( void*) Cyc_Absyn_ExternC){ goto _LL518;} else{
goto _LL519;} _LL519: if( _temp509 ==( void*) Cyc_Absyn_Extern){ goto _LL520;}
else{ goto _LL510;} _LL512: if( _temp478? Cyc_Tcutil_is_function_type(( void*)
_temp503->type): 0){ Cyc_Interface_body_err( _tag_arr("static", sizeof(
unsigned char), 7u), _temp503->name);} goto _LL510; _LL514:( int) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp521=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp521[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp522; _temp522.tag= Cyc_Core_InvalidArg; _temp522.f1=
_tag_arr("extract_ordinarie", sizeof( unsigned char), 18u); _temp522;});
_temp521;})); _LL516: if( _temp478? Cyc_Tcutil_is_function_type(( void*)
_temp503->type): 0){ Cyc_Interface_body_err( _tag_arr("public", sizeof(
unsigned char), 7u), _temp503->name);} if( Cyc_Interface_check_public_vardecl(
_temp476, _temp474, _temp503)){ _temp480->vardecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Vardecl* data))
Cyc_Dict_insert)( _temp480->vardecls, x, _temp503);} goto _LL510; _LL518: goto
_LL520; _LL520: if( Cyc_Interface_check_public_vardecl( _temp476, _temp474,
_temp503)){ _temp482->vardecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Vardecl* data)) Cyc_Dict_insert)(
_temp482->vardecls, x, _temp503);} goto _LL510; _LL510:;} goto _LL494; _LL500:
goto _LL494; _LL494:;} goto _LL486; _LL490: goto _LL486; _LL486:;}} struct
_tuple12{ struct Cyc_Interface_I* f1; int f2; struct Cyc_Dict_Dict* f3; struct
Cyc_Interface_Seen* f4; } ; static void Cyc_Interface_extract_f( struct _tuple12*
env_f, struct Cyc_List_List* ns, struct Cyc_Tcenv_Genv* ge){ struct _tuple12
_temp525; struct Cyc_Interface_Seen* _temp526; struct Cyc_Dict_Dict* _temp528;
int _temp530; struct Cyc_Interface_I* _temp532; struct _tuple12* _temp523= env_f;
_temp525=* _temp523; _LL533: _temp532= _temp525.f1; goto _LL531; _LL531:
_temp530= _temp525.f2; goto _LL529; _LL529: _temp528= _temp525.f3; goto _LL527;
_LL527: _temp526= _temp525.f4; goto _LL524; _LL524: { struct _tuple11 _temp534=({
struct _tuple11 _temp535; _temp535.f1=(( struct Cyc_Interface_Ienv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp532->imports,
ns); _temp535.f2=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp532->exports, ns); _temp535.f3=
_temp530; _temp535.f4= _temp528; _temp535.f5= _temp526; _temp535.f6= _temp532;
_temp535;});(( void(*)( void(* f)( struct _tuple11*, struct _tagged_arr*, struct
Cyc_Absyn_Structdecl**), struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(
Cyc_Interface_extract_structdecl,& _temp534, ge->structdecls);(( void(*)( void(*
f)( struct _tuple11*, struct _tagged_arr*, struct Cyc_Absyn_Uniondecl**), struct
_tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_uniondecl,&
_temp534, ge->uniondecls);(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_arr*, struct Cyc_Absyn_Tuniondecl**), struct _tuple11* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_tuniondecl,& _temp534, ge->tuniondecls);((
void(*)( void(* f)( struct _tuple11*, struct _tagged_arr*, struct Cyc_Absyn_Enumdecl**),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_enumdecl,&
_temp534, ge->enumdecls);(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_arr*, struct Cyc_Absyn_Typedefdecl*), struct _tuple11* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_typedef,& _temp534, ge->typedefs);((
void(*)( void(* f)( struct _tuple11*, struct _tagged_arr*, struct _tuple7*),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_ordinarie,&
_temp534, ge->ordinaries);}} static struct Cyc_Interface_I* Cyc_Interface_gen_extract(
struct Cyc_Dict_Dict* ae, int check_complete_defs){ struct _tuple12 _temp536=({
struct _tuple12 _temp537; _temp537.f1=(( struct Cyc_Interface_I*(*)( struct Cyc_Dict_Dict*
skel)) Cyc_Interface_skel2i)( ae); _temp537.f2= check_complete_defs; _temp537.f3=
ae; _temp537.f4= Cyc_Interface_new_seen(); _temp537;});(( void(*)( void(* f)(
struct _tuple12*, struct Cyc_List_List*, struct Cyc_Tcenv_Genv*), struct
_tuple12* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_f,&
_temp536, ae); return _temp536.f1;} struct Cyc_Interface_I* Cyc_Interface_extract(
struct Cyc_Dict_Dict* ae){ return Cyc_Interface_gen_extract( ae, 1);} inline
static void Cyc_Interface_check_err( struct _tagged_arr* msg1, struct
_tagged_arr msg2){ Cyc_Tcdecl_merr( 0, msg1, msg2);} struct _tuple13{ int f1;
struct Cyc_Dict_Dict* f2; int(* f3)( void*, void*, struct _tagged_arr*); struct
_tagged_arr f4; struct _tagged_arr* f5; } ; static void Cyc_Interface_incl_dict_f(
struct _tuple13* env, struct _tagged_arr* x, void* y1){ struct _tuple13 _temp540;
struct _tagged_arr* _temp541; struct _tagged_arr _temp543; int(* _temp545)( void*,
void*, struct _tagged_arr*); struct Cyc_Dict_Dict* _temp547; int _temp549; int*
_temp551; struct _tuple13* _temp538= env; _temp540=* _temp538; _LL550: _temp549=
_temp540.f1; _temp551=&(* _temp538).f1; goto _LL548; _LL548: _temp547= _temp540.f2;
goto _LL546; _LL546: _temp545= _temp540.f3; goto _LL544; _LL544: _temp543=
_temp540.f4; goto _LL542; _LL542: _temp541= _temp540.f5; goto _LL539; _LL539: {
struct _handler_cons _temp552; _push_handler(& _temp552);{ int _temp554= 0; if(
setjmp( _temp552.handler)){ _temp554= 1;} if( ! _temp554){{ void* _temp555=((
void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
_temp547, x); if( ! _temp545( y1, _temp555, _temp541)){* _temp551= 0;}};
_pop_handler();} else{ void* _temp553=( void*) _exn_thrown; void* _temp557=
_temp553; _LL559: if( _temp557 == Cyc_Dict_Absent){ goto _LL560;} else{ goto
_LL561;} _LL561: goto _LL562; _LL560: Cyc_Interface_check_err( _temp541,( struct
_tagged_arr)({ struct _tagged_arr(* _temp563)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp568= _tag_arr("%s %s is missing",
sizeof( unsigned char), 17u); struct Cyc_Stdio_String_pa_struct _temp570;
_temp570.tag= Cyc_Stdio_String_pa; _temp570.f1=( struct _tagged_arr) _temp543;{
void* _temp566=( void*)& _temp570; struct Cyc_Stdio_String_pa_struct _temp569;
_temp569.tag= Cyc_Stdio_String_pa; _temp569.f1=( struct _tagged_arr)* x;{ void*
_temp567=( void*)& _temp569; void* _temp564[ 2u]={ _temp566, _temp567}; struct
_tagged_arr _temp565={( void*) _temp564,( void*) _temp564,( void*)( _temp564 + 2u)};
_temp563( _temp568, _temp565);}}}));* _temp551= 0; goto _LL558; _LL562:( void)
_throw( _temp557); _LL558:;}}}} static int Cyc_Interface_incl_dict( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( void*, void*, struct
_tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg){ struct _tuple13
_temp571=({ struct _tuple13 _temp572; _temp572.f1= 1; _temp572.f2= dic2;
_temp572.f3= incl_f; _temp572.f4= t; _temp572.f5= msg; _temp572;});(( void(*)(
void(* f)( struct _tuple13*, struct _tagged_arr*, void*), struct _tuple13* env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_incl_dict_f,& _temp571,
dic1); return _temp571.f1;} static int Cyc_Interface_incl_structdecl( struct Cyc_Absyn_Structdecl*
d0, struct Cyc_Absyn_Structdecl* d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Structdecl*
_temp573= Cyc_Tcdecl_merge_structdecl( d0, d1, 0, msg); if( _temp573 == 0){
return 0;} if(( struct Cyc_Absyn_Structdecl*) d0 != _temp573){ Cyc_Interface_check_err(
msg,( struct _tagged_arr)({ struct _tagged_arr(* _temp574)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp578=
_tag_arr("declaration of struct %s discloses too much information", sizeof(
unsigned char), 56u); struct Cyc_Stdio_String_pa_struct _temp579; _temp579.tag=
Cyc_Stdio_String_pa; _temp579.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d1->name))->v);{ void*
_temp577=( void*)& _temp579; void* _temp575[ 1u]={ _temp577}; struct _tagged_arr
_temp576={( void*) _temp575,( void*) _temp575,( void*)( _temp575 + 1u)};
_temp574( _temp578, _temp576);}})); return 0;} return 1;} static int Cyc_Interface_incl_uniondecl(
struct Cyc_Absyn_Uniondecl* d0, struct Cyc_Absyn_Uniondecl* d1, struct
_tagged_arr* msg){ struct Cyc_Absyn_Uniondecl* _temp580= Cyc_Tcdecl_merge_uniondecl(
d0, d1, 0, msg); if( _temp580 == 0){ return 0;} if(( struct Cyc_Absyn_Uniondecl*)
d0 != _temp580){ Cyc_Interface_check_err( msg,( struct _tagged_arr)({ struct
_tagged_arr(* _temp581)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp585= _tag_arr("declaration of union %s discloses too much information",
sizeof( unsigned char), 55u); struct Cyc_Stdio_String_pa_struct _temp586;
_temp586.tag= Cyc_Stdio_String_pa; _temp586.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d1->name))->v);{ void*
_temp584=( void*)& _temp586; void* _temp582[ 1u]={ _temp584}; struct _tagged_arr
_temp583={( void*) _temp582,( void*) _temp582,( void*)( _temp582 + 1u)};
_temp581( _temp585, _temp583);}})); return 0;} return 1;} static int Cyc_Interface_incl_tuniondecl(
struct Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl* d1, struct
_tagged_arr* msg){ struct Cyc_Absyn_Tuniondecl* _temp587= Cyc_Tcdecl_merge_tuniondecl(
d0, d1, 0, msg); if( _temp587 == 0){ return 0;} if(( struct Cyc_Absyn_Tuniondecl*)
d0 != _temp587){ Cyc_Interface_check_err( msg,( struct _tagged_arr)({ struct
_tagged_arr(* _temp588)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp592= _tag_arr("declaration of tunion %s discloses too much information",
sizeof( unsigned char), 56u); struct Cyc_Stdio_String_pa_struct _temp593;
_temp593.tag= Cyc_Stdio_String_pa; _temp593.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
d1->name);{ void* _temp591=( void*)& _temp593; void* _temp589[ 1u]={ _temp591};
struct _tagged_arr _temp590={( void*) _temp589,( void*) _temp589,( void*)(
_temp589 + 1u)}; _temp588( _temp592, _temp590);}})); return 0;} return 1;}
static int Cyc_Interface_incl_enumdecl( struct Cyc_Absyn_Enumdecl* d0, struct
Cyc_Absyn_Enumdecl* d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Enumdecl*
_temp594= Cyc_Tcdecl_merge_enumdecl( d0, d1, 0, msg); if( _temp594 == 0){ return
0;} if(( struct Cyc_Absyn_Enumdecl*) d0 != _temp594){ Cyc_Interface_check_err(
msg,( struct _tagged_arr)({ struct _tagged_arr(* _temp595)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp599=
_tag_arr("declaration of enum %s discloses too much information", sizeof(
unsigned char), 54u); struct Cyc_Stdio_String_pa_struct _temp600; _temp600.tag=
Cyc_Stdio_String_pa; _temp600.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
d1->name);{ void* _temp598=( void*)& _temp600; void* _temp596[ 1u]={ _temp598};
struct _tagged_arr _temp597={( void*) _temp596,( void*) _temp596,( void*)(
_temp596 + 1u)}; _temp595( _temp599, _temp597);}})); return 0;} return 1;}
static int Cyc_Interface_incl_vardecl( struct Cyc_Absyn_Vardecl* d0, struct Cyc_Absyn_Vardecl*
d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Vardecl* _temp601= Cyc_Tcdecl_merge_vardecl(
d0, d1, 0, msg); if( _temp601 == 0){ return 0;} if(( struct Cyc_Absyn_Vardecl*)
d0 != _temp601){ Cyc_Interface_check_err( msg,( struct _tagged_arr)({ struct
_tagged_arr(* _temp602)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp606= _tag_arr("declaration of variable %s discloses too much information",
sizeof( unsigned char), 58u); struct Cyc_Stdio_String_pa_struct _temp607;
_temp607.tag= Cyc_Stdio_String_pa; _temp607.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
d1->name);{ void* _temp605=( void*)& _temp607; void* _temp603[ 1u]={ _temp605};
struct _tagged_arr _temp604={( void*) _temp603,( void*) _temp603,( void*)(
_temp603 + 1u)}; _temp602( _temp606, _temp604);}})); return 0;} return 1;}
static int Cyc_Interface_incl_typedefdecl( struct Cyc_Absyn_Typedefdecl* d0,
struct Cyc_Absyn_Typedefdecl* d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Typedefdecl*
_temp608= Cyc_Tcdecl_merge_typedefdecl( d0, d1, 0, msg); if( _temp608 == 0){
return 0;} if(( struct Cyc_Absyn_Typedefdecl*) d0 != _temp608){ Cyc_Interface_check_err(
msg,( struct _tagged_arr)({ struct _tagged_arr(* _temp609)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp613=
_tag_arr("declaration of typedef %s discloses too much information", sizeof(
unsigned char), 57u); struct Cyc_Stdio_String_pa_struct _temp614; _temp614.tag=
Cyc_Stdio_String_pa; _temp614.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
d1->name);{ void* _temp612=( void*)& _temp614; void* _temp610[ 1u]={ _temp612};
struct _tagged_arr _temp611={( void*) _temp610,( void*) _temp610,( void*)(
_temp610 + 1u)}; _temp609( _temp613, _temp611);}})); return 0;} return 1;}
static int Cyc_Interface_incl_xtunionfielddecl( struct Cyc_Tcdecl_Xtunionfielddecl*
d0, struct Cyc_Tcdecl_Xtunionfielddecl* d1, struct _tagged_arr* msg){ struct Cyc_Tcdecl_Xtunionfielddecl*
_temp615= Cyc_Tcdecl_merge_xtunionfielddecl( d0, d1, 0, msg); if( _temp615 == 0){
return 0;} if(( struct Cyc_Tcdecl_Xtunionfielddecl*) d0 != _temp615){ Cyc_Interface_check_err(
msg,( struct _tagged_arr)({ struct _tagged_arr(* _temp616)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp620=
_tag_arr("declaration of xtunionfield %s discloses too much information",
sizeof( unsigned char), 62u); struct Cyc_Stdio_String_pa_struct _temp621;
_temp621.tag= Cyc_Stdio_String_pa; _temp621.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
d1->field)->name);{ void* _temp619=( void*)& _temp621; void* _temp617[ 1u]={
_temp619}; struct _tagged_arr _temp618={( void*) _temp617,( void*) _temp617,(
void*)( _temp617 + 1u)}; _temp616( _temp620, _temp618);}})); return 0;} return 1;}
struct Cyc_Core_Opt* Cyc_Interface_compat_merge_dict( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, void**(* merge_f)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_arr*), struct _tagged_arr t,
struct _tagged_arr* msg); static int Cyc_Interface_incl_ienv( struct Cyc_Interface_Ienv*
ie1, struct Cyc_Interface_Ienv* ie2, struct _tagged_arr* msg){ int _temp622=((
int(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)(
struct Cyc_Absyn_Structdecl*, struct Cyc_Absyn_Structdecl*, struct _tagged_arr*),
struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_incl_dict)( ie1->structdecls,
ie2->structdecls, Cyc_Interface_incl_structdecl, _tag_arr("struct", sizeof(
unsigned char), 7u), msg); int _temp623=(( int(*)( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Uniondecl*, struct
Cyc_Absyn_Uniondecl*, struct _tagged_arr*), struct _tagged_arr t, struct
_tagged_arr* msg)) Cyc_Interface_incl_dict)( ie1->uniondecls, ie2->uniondecls,
Cyc_Interface_incl_uniondecl, _tag_arr("union", sizeof( unsigned char), 6u), msg);
int _temp624=(( int(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2,
int(* incl_f)( struct Cyc_Absyn_Tuniondecl*, struct Cyc_Absyn_Tuniondecl*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_incl_dict)(
ie1->tuniondecls, ie2->tuniondecls, Cyc_Interface_incl_tuniondecl, _tag_arr("tunion",
sizeof( unsigned char), 7u), msg); int _temp625=(( int(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Enumdecl*,
struct Cyc_Absyn_Enumdecl*, struct _tagged_arr*), struct _tagged_arr t, struct
_tagged_arr* msg)) Cyc_Interface_incl_dict)( ie1->enumdecls, ie2->enumdecls, Cyc_Interface_incl_enumdecl,
_tag_arr("enum", sizeof( unsigned char), 5u), msg); int _temp626=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)( struct Cyc_Absyn_Typedefdecl*,
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*, struct _tagged_arr*),
struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls, ie2->typedefdecls,(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_strptrcmp),
Cyc_Tcdecl_merge_typedefdecl, _tag_arr("typedef", sizeof( unsigned char), 8u),
msg) != 0; int _temp627=(( int(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, int(* incl_f)( struct Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_incl_dict)(
ie1->vardecls, ie2->vardecls, Cyc_Interface_incl_vardecl, _tag_arr("variable",
sizeof( unsigned char), 9u), msg); int _temp628=(( int(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Tcdecl_Xtunionfielddecl*,
struct Cyc_Tcdecl_Xtunionfielddecl*, struct _tagged_arr*), struct _tagged_arr t,
struct _tagged_arr* msg)) Cyc_Interface_incl_dict)( ie1->xtunionfielddecls, ie2->xtunionfielddecls,
Cyc_Interface_incl_xtunionfielddecl, _tag_arr("xtunionfield", sizeof(
unsigned char), 13u), msg); return((((( _temp622? _temp623: 0)? _temp624: 0)?
_temp625: 0)? _temp626: 0)? _temp627: 0)? _temp628: 0;} struct _tuple14{ int f1;
struct Cyc_Dict_Dict* f2; struct _tagged_arr* f3; } ; static void Cyc_Interface_incl_ns_f(
struct _tuple14* env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie1){
struct _tuple14 _temp631; struct _tagged_arr* _temp632; struct Cyc_Dict_Dict*
_temp634; int _temp636; int* _temp638; struct _tuple14* _temp629= env; _temp631=*
_temp629; _LL637: _temp636= _temp631.f1; _temp638=&(* _temp629).f1; goto _LL635;
_LL635: _temp634= _temp631.f2; goto _LL633; _LL633: _temp632= _temp631.f3; goto
_LL630; _LL630: { struct Cyc_Interface_Ienv* ie2;{ struct _handler_cons _temp639;
_push_handler(& _temp639);{ int _temp641= 0; if( setjmp( _temp639.handler)){
_temp641= 1;} if( ! _temp641){ ie2=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp634, ns);; _pop_handler();}
else{ void* _temp640=( void*) _exn_thrown; void* _temp643= _temp640; _LL645: if(
_temp643 == Cyc_Dict_Absent){ goto _LL646;} else{ goto _LL647;} _LL647: goto
_LL648; _LL646: ie2= Cyc_Interface_lazy_new_ienv(); goto _LL644; _LL648:( void)
_throw( _temp643); _LL644:;}}} if( ! Cyc_Interface_incl_ienv( ie1, ie2, _temp632)){*
_temp638= 0;}}} static int Cyc_Interface_incl_ns( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct _tagged_arr* msg){ struct _tuple14 _temp649=({
struct _tuple14 _temp650; _temp650.f1= 1; _temp650.f2= dic2; _temp650.f3= msg;
_temp650;});(( void(*)( void(* f)( struct _tuple14*, struct Cyc_List_List*,
struct Cyc_Interface_Ienv*), struct _tuple14* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(
Cyc_Interface_incl_ns_f,& _temp649, dic1); return _temp649.f1;} int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple3* info){
struct _handler_cons _temp651; _push_handler(& _temp651);{ int _temp653= 0; if(
setjmp( _temp651.handler)){ _temp653= 1;} if( ! _temp653){{ int _temp654= 1;
struct _tagged_arr* msg= 0; if( info != 0){ msg=({ struct _tagged_arr* _temp655=(
struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp655[ 0]=(
struct _tagged_arr)({ struct _tagged_arr(* _temp656)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp661= _tag_arr("checking inclusion of %s exports into %s exports,",
sizeof( unsigned char), 50u); struct Cyc_Stdio_String_pa_struct _temp663;
_temp663.tag= Cyc_Stdio_String_pa; _temp663.f1=( struct _tagged_arr)(*(( struct
_tuple3*) _check_null( info))).f1;{ void* _temp659=( void*)& _temp663; struct
Cyc_Stdio_String_pa_struct _temp662; _temp662.tag= Cyc_Stdio_String_pa; _temp662.f1=(
struct _tagged_arr)(*(( struct _tuple3*) _check_null( info))).f2;{ void*
_temp660=( void*)& _temp662; void* _temp657[ 2u]={ _temp659, _temp660}; struct
_tagged_arr _temp658={( void*) _temp657,( void*) _temp657,( void*)( _temp657 + 2u)};
_temp656( _temp661, _temp658);}}}); _temp655;});} if( ! Cyc_Interface_incl_ns(
i1->exports, i2->exports, msg)){ _temp654= 0;} if( info != 0){ msg=({ struct
_tagged_arr* _temp664=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp664[ 0]=( struct _tagged_arr)({ struct _tagged_arr(*
_temp665)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp670= _tag_arr("checking inclusion of %s imports into %s imports,",
sizeof( unsigned char), 50u); struct Cyc_Stdio_String_pa_struct _temp672;
_temp672.tag= Cyc_Stdio_String_pa; _temp672.f1=( struct _tagged_arr)(*(( struct
_tuple3*) _check_null( info))).f2;{ void* _temp668=( void*)& _temp672; struct
Cyc_Stdio_String_pa_struct _temp671; _temp671.tag= Cyc_Stdio_String_pa; _temp671.f1=(
struct _tagged_arr)(*(( struct _tuple3*) _check_null( info))).f1;{ void*
_temp669=( void*)& _temp671; void* _temp666[ 2u]={ _temp668, _temp669}; struct
_tagged_arr _temp667={( void*) _temp666,( void*) _temp666,( void*)( _temp666 + 2u)};
_temp665( _temp670, _temp667);}}}); _temp664;});} if( ! Cyc_Interface_incl_ns(
i2->imports, i1->imports, msg)){ _temp654= 0;}{ int _temp673= _temp654;
_npop_handler( 0u); return _temp673;}}; _pop_handler();} else{ void* _temp652=(
void*) _exn_thrown; void* _temp675= _temp652; _LL677: if( _temp675 == Cyc_Tcdecl_Incompatible){
goto _LL678;} else{ goto _LL679;} _LL679: goto _LL680; _LL678: return 0; _LL680:(
void) _throw( _temp675); _LL676:;}}} struct _tuple15{ int f1; struct Cyc_Dict_Dict*
f2; struct Cyc_Dict_Dict* f3; struct Cyc_Dict_Dict* f4; void**(* f5)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_arr*); struct _tagged_arr
f6; struct _tagged_arr* f7; } ; void Cyc_Interface_compat_merge_dict_f( struct
_tuple15* env, struct _tagged_arr* x, void** y2){ struct _tuple15 _temp683;
struct _tagged_arr* _temp684; struct _tagged_arr _temp686; void**(* _temp688)(
void**, void**, struct Cyc_Position_Segment*, struct _tagged_arr*); struct Cyc_Dict_Dict*
_temp690; struct Cyc_Dict_Dict* _temp692; struct Cyc_Dict_Dict* _temp694; struct
Cyc_Dict_Dict** _temp696; int _temp697; int* _temp699; struct _tuple15* _temp681=
env; _temp683=* _temp681; _LL698: _temp697= _temp683.f1; _temp699=&(* _temp681).f1;
goto _LL695; _LL695: _temp694= _temp683.f2; _temp696=&(* _temp681).f2; goto
_LL693; _LL693: _temp692= _temp683.f3; goto _LL691; _LL691: _temp690= _temp683.f4;
goto _LL689; _LL689: _temp688= _temp683.f5; goto _LL687; _LL687: _temp686=
_temp683.f6; goto _LL685; _LL685: _temp684= _temp683.f7; goto _LL682; _LL682: {
void** y;{ struct _handler_cons _temp700; _push_handler(& _temp700);{ int
_temp702= 0; if( setjmp( _temp700.handler)){ _temp702= 1;} if( ! _temp702){{
void** _temp703=(( void**(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key))
Cyc_Dict_lookup)( _temp692, x); void** _temp704= _temp688( _temp703, y2, 0,
_temp684); if( !(( unsigned int) _temp704)){* _temp699= 0; _npop_handler( 0u);
return;} y=( void**) _check_null( _temp704);}; _pop_handler();} else{ void*
_temp701=( void*) _exn_thrown; void* _temp706= _temp701; _LL708: if( _temp706 ==
Cyc_Dict_Absent){ goto _LL709;} else{ goto _LL710;} _LL710: goto _LL711; _LL709:
y= y2; goto _LL707; _LL711:( void) _throw( _temp706); _LL707:;}}}{ struct
_handler_cons _temp712; _push_handler(& _temp712);{ int _temp714= 0; if( setjmp(
_temp712.handler)){ _temp714= 1;} if( ! _temp714){{ void** _temp715=(( void**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp690, x);
void** _temp716= _temp688( _temp715, y, 0, _temp684); if( _temp716 !=( void**)
_temp715){ if(( unsigned int) _temp716){ Cyc_Interface_check_err( _temp684,(
struct _tagged_arr)({ struct _tagged_arr(* _temp717)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp722= _tag_arr("abstract %s %s is being imported as non-abstract",
sizeof( unsigned char), 49u); struct Cyc_Stdio_String_pa_struct _temp724;
_temp724.tag= Cyc_Stdio_String_pa; _temp724.f1=( struct _tagged_arr) _temp686;{
void* _temp720=( void*)& _temp724; struct Cyc_Stdio_String_pa_struct _temp723;
_temp723.tag= Cyc_Stdio_String_pa; _temp723.f1=( struct _tagged_arr)* x;{ void*
_temp721=( void*)& _temp723; void* _temp718[ 2u]={ _temp720, _temp721}; struct
_tagged_arr _temp719={( void*) _temp718,( void*) _temp718,( void*)( _temp718 + 2u)};
_temp717( _temp722, _temp719);}}}));}* _temp699= 0;}}; _pop_handler();} else{
void* _temp713=( void*) _exn_thrown; void* _temp726= _temp713; _LL728: if(
_temp726 == Cyc_Dict_Absent){ goto _LL729;} else{ goto _LL730;} _LL730: goto
_LL731; _LL729: if(* _temp699){* _temp696=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key, void** data)) Cyc_Dict_insert)(*
_temp696, x, y);} goto _LL727; _LL731:( void) _throw( _temp726); _LL727:;}}}}}
struct Cyc_Core_Opt* Cyc_Interface_compat_merge_dict( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, void**(* merge_f)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_arr*), struct _tagged_arr t,
struct _tagged_arr* msg){ struct _tuple15 _temp732=({ struct _tuple15 _temp734;
_temp734.f1= 1; _temp734.f2= dic1; _temp734.f3= dic1; _temp734.f4= excl;
_temp734.f5= merge_f; _temp734.f6= t; _temp734.f7= msg; _temp734;});(( void(*)(
void(* f)( struct _tuple15*, struct _tagged_arr*, void**), struct _tuple15* env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_compat_merge_dict_f,&
_temp732, dic2); if( _temp732.f1){ return({ struct Cyc_Core_Opt* _temp733=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp733->v=(
void*) _temp732.f2; _temp733;});} else{ return 0;}} struct Cyc_Interface_Ienv*
Cyc_Interface_compat_merge_ienv( struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv*
ie2, struct Cyc_Interface_Ienv* iexcl, struct _tagged_arr* msg){ struct Cyc_Core_Opt*
_temp735=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Structdecl*(* merge_f)(
struct Cyc_Absyn_Structdecl*, struct Cyc_Absyn_Structdecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->structdecls, ie2->structdecls, iexcl->structdecls, Cyc_Tcdecl_merge_structdecl,
_tag_arr("struct", sizeof( unsigned char), 7u), msg); struct Cyc_Core_Opt*
_temp736=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Uniondecl*(* merge_f)( struct
Cyc_Absyn_Uniondecl*, struct Cyc_Absyn_Uniondecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->uniondecls, ie2->uniondecls, iexcl->uniondecls, Cyc_Tcdecl_merge_uniondecl,
_tag_arr("union", sizeof( unsigned char), 6u), msg); struct Cyc_Core_Opt*
_temp737=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Tuniondecl*(* merge_f)(
struct Cyc_Absyn_Tuniondecl*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->tuniondecls, ie2->tuniondecls, iexcl->tuniondecls, Cyc_Tcdecl_merge_tuniondecl,
_tag_arr("tunion", sizeof( unsigned char), 7u), msg); struct Cyc_Core_Opt*
_temp738=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Enumdecl*(* merge_f)( struct
Cyc_Absyn_Enumdecl*, struct Cyc_Absyn_Enumdecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->enumdecls, ie2->enumdecls, iexcl->enumdecls, Cyc_Tcdecl_merge_enumdecl,
_tag_arr("enum", sizeof( unsigned char), 5u), msg); struct Cyc_Core_Opt*
_temp739=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)(
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls, ie2->typedefdecls, iexcl->typedefdecls, Cyc_Tcdecl_merge_typedefdecl,
_tag_arr("typedef", sizeof( unsigned char), 8u), msg); struct Cyc_Core_Opt*
_temp740=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Vardecl*(* merge_f)( struct
Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->vardecls, ie2->vardecls, iexcl->vardecls, Cyc_Tcdecl_merge_vardecl,
_tag_arr("variable", sizeof( unsigned char), 9u), msg); struct Cyc_Core_Opt*
_temp741=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Tcdecl_Xtunionfielddecl*(* merge_f)(
struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_Tcdecl_Xtunionfielddecl*, struct
Cyc_Position_Segment*, struct _tagged_arr*), struct _tagged_arr t, struct
_tagged_arr* msg)) Cyc_Interface_compat_merge_dict)( ie1->xtunionfielddecls, ie2->xtunionfielddecls,
iexcl->xtunionfielddecls, Cyc_Tcdecl_merge_xtunionfielddecl, _tag_arr("xtunionfield",
sizeof( unsigned char), 13u), msg); if(((((( !(( unsigned int) _temp735)? 1: !((
unsigned int) _temp736))? 1: !(( unsigned int) _temp737))? 1: !(( unsigned int)
_temp738))? 1: !(( unsigned int) _temp739))? 1: !(( unsigned int) _temp740))? 1:
!(( unsigned int) _temp741)){ return 0;} else{ return({ struct Cyc_Interface_Ienv*
_temp742=( struct Cyc_Interface_Ienv*) GC_malloc( sizeof( struct Cyc_Interface_Ienv));
_temp742->structdecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp735))->v; _temp742->uniondecls=( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp736))->v; _temp742->tuniondecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp737))->v; _temp742->enumdecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp738))->v;
_temp742->typedefdecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp739))->v; _temp742->vardecls=( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( _temp740))->v; _temp742->xtunionfielddecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp741))->v; _temp742;});}}
struct _tuple16{ int f1; struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3;
struct Cyc_Dict_Dict* f4; struct _tagged_arr* f5; } ; void Cyc_Interface_compat_merge_ns_f(
struct _tuple16* env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie2){
struct _tuple16 _temp745; struct _tagged_arr* _temp746; struct Cyc_Dict_Dict*
_temp748; struct Cyc_Dict_Dict* _temp750; struct Cyc_Dict_Dict* _temp752; struct
Cyc_Dict_Dict** _temp754; int _temp755; int* _temp757; struct _tuple16* _temp743=
env; _temp745=* _temp743; _LL756: _temp755= _temp745.f1; _temp757=&(* _temp743).f1;
goto _LL753; _LL753: _temp752= _temp745.f2; _temp754=&(* _temp743).f2; goto
_LL751; _LL751: _temp750= _temp745.f3; goto _LL749; _LL749: _temp748= _temp745.f4;
goto _LL747; _LL747: _temp746= _temp745.f5; goto _LL744; _LL744: { struct Cyc_Interface_Ienv*
ie1; struct Cyc_Interface_Ienv* iexcl;{ struct _handler_cons _temp758;
_push_handler(& _temp758);{ int _temp760= 0; if( setjmp( _temp758.handler)){
_temp760= 1;} if( ! _temp760){ iexcl=(( struct Cyc_Interface_Ienv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp748, ns);;
_pop_handler();} else{ void* _temp759=( void*) _exn_thrown; void* _temp762=
_temp759; _LL764: if( _temp762 == Cyc_Dict_Absent){ goto _LL765;} else{ goto
_LL766;} _LL766: goto _LL767; _LL765: iexcl= Cyc_Interface_lazy_new_ienv(); goto
_LL763; _LL767:( void) _throw( _temp762); _LL763:;}}}{ struct _handler_cons
_temp768; _push_handler(& _temp768);{ int _temp770= 0; if( setjmp( _temp768.handler)){
_temp770= 1;} if( ! _temp770){ ie1=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp750, ns);{ struct Cyc_Interface_Ienv*
_temp771= Cyc_Interface_compat_merge_ienv( Cyc_Interface_lazy_new_ienv(), ie1,
iexcl, _temp746); if( _temp771 == 0){* _temp757= 0; _npop_handler( 0u); return;}
ie1=( struct Cyc_Interface_Ienv*) _check_null( _temp771);}; _pop_handler();}
else{ void* _temp769=( void*) _exn_thrown; void* _temp773= _temp769; _LL775: if(
_temp773 == Cyc_Dict_Absent){ goto _LL776;} else{ goto _LL777;} _LL777: goto
_LL778; _LL776: ie1= Cyc_Interface_lazy_new_ienv(); goto _LL774; _LL778:( void)
_throw( _temp773); _LL774:;}}}{ struct Cyc_Interface_Ienv* _temp779= Cyc_Interface_compat_merge_ienv(
ie1, ie2, iexcl, _temp746); if( _temp779 == 0){* _temp757= 0;} else{ if(*
_temp757){* _temp754=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(*
_temp754, ns,( struct Cyc_Interface_Ienv*) _check_null( _temp779));}}}}} struct
Cyc_Core_Opt* Cyc_Interface_compat_merge_ns( struct Cyc_Dict_Dict* dic1, struct
Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct _tagged_arr* msg){
struct _tuple16 _temp780=({ struct _tuple16 _temp782; _temp782.f1= 1; _temp782.f2=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct Cyc_List_List*, struct Cyc_List_List*)))
Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp); _temp782.f3= dic1; _temp782.f4= excl;
_temp782.f5= msg; _temp782;});(( void(*)( void(* f)( struct _tuple16*, struct
Cyc_List_List*, struct Cyc_Interface_Ienv*), struct _tuple16* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_compat_merge_ns_f,& _temp780, dic2); if(
_temp780.f1){ return({ struct Cyc_Core_Opt* _temp781=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp781->v=( void*) _temp780.f2;
_temp781;});} else{ return 0;}} struct _tuple17{ int f1; struct Cyc_Dict_Dict*
f2; struct Cyc_Dict_Dict* f3; struct _tagged_arr f4; struct _tagged_arr* f5; } ;
void Cyc_Interface_disj_merge_dict_f( struct _tuple17* env, struct _tagged_arr*
x, void* y){ struct _tuple17 _temp785; struct _tagged_arr* _temp786; struct
_tagged_arr _temp788; struct Cyc_Dict_Dict* _temp790; struct Cyc_Dict_Dict*
_temp792; struct Cyc_Dict_Dict** _temp794; int _temp795; int* _temp797; struct
_tuple17* _temp783= env; _temp785=* _temp783; _LL796: _temp795= _temp785.f1;
_temp797=&(* _temp783).f1; goto _LL793; _LL793: _temp792= _temp785.f2; _temp794=&(*
_temp783).f2; goto _LL791; _LL791: _temp790= _temp785.f3; goto _LL789; _LL789:
_temp788= _temp785.f4; goto _LL787; _LL787: _temp786= _temp785.f5; goto _LL784;
_LL784: if((( int(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_member)(
_temp790, x)){ Cyc_Interface_check_err( _temp786,( struct _tagged_arr)({ struct
_tagged_arr(* _temp798)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp803= _tag_arr("%s %s is exported more than once",
sizeof( unsigned char), 33u); struct Cyc_Stdio_String_pa_struct _temp805;
_temp805.tag= Cyc_Stdio_String_pa; _temp805.f1=( struct _tagged_arr) _temp788;{
void* _temp801=( void*)& _temp805; struct Cyc_Stdio_String_pa_struct _temp804;
_temp804.tag= Cyc_Stdio_String_pa; _temp804.f1=( struct _tagged_arr)* x;{ void*
_temp802=( void*)& _temp804; void* _temp799[ 2u]={ _temp801, _temp802}; struct
_tagged_arr _temp800={( void*) _temp799,( void*) _temp799,( void*)( _temp799 + 2u)};
_temp798( _temp803, _temp800);}}}));* _temp797= 0;} else{ if(* _temp797){*
_temp794=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key, void* data)) Cyc_Dict_insert)(* _temp794, x, y);}}} struct Cyc_Core_Opt*
Cyc_Interface_disj_merge_dict( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct _tagged_arr t, struct _tagged_arr* msg){ struct _tuple17 _temp806=({
struct _tuple17 _temp808; _temp808.f1= 1; _temp808.f2= dic1; _temp808.f3= dic1;
_temp808.f4= t; _temp808.f5= msg; _temp808;});(( void(*)( void(* f)( struct
_tuple17*, struct _tagged_arr*, void*), struct _tuple17* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_disj_merge_dict_f,& _temp806, dic2); if(
_temp806.f1){ return({ struct Cyc_Core_Opt* _temp807=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp807->v=( void*) _temp806.f2;
_temp807;});} else{ return 0;}} struct Cyc_Interface_Ienv* Cyc_Interface_disj_merge_ienv(
struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv* ie2, struct
_tagged_arr* msg){ struct Cyc_Core_Opt* _temp809=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t,
struct _tagged_arr* msg)) Cyc_Interface_disj_merge_dict)( ie1->structdecls, ie2->structdecls,
_tag_arr("struct", sizeof( unsigned char), 7u), msg); struct Cyc_Core_Opt*
_temp810=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_disj_merge_dict)(
ie1->uniondecls, ie2->uniondecls, _tag_arr("union", sizeof( unsigned char), 6u),
msg); struct Cyc_Core_Opt* _temp811=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t, struct _tagged_arr* msg))
Cyc_Interface_disj_merge_dict)( ie1->tuniondecls, ie2->tuniondecls, _tag_arr("[x]tunion",
sizeof( unsigned char), 10u), msg); struct Cyc_Core_Opt* _temp812=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t,
struct _tagged_arr* msg)) Cyc_Interface_disj_merge_dict)( ie1->enumdecls, ie2->enumdecls,
_tag_arr("enum", sizeof( unsigned char), 5u), msg); struct Cyc_Core_Opt*
_temp813=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)(
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls, ie2->typedefdecls,(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp),
Cyc_Tcdecl_merge_typedefdecl, _tag_arr("typedef", sizeof( unsigned char), 8u),
msg); struct Cyc_Core_Opt* _temp814=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t, struct _tagged_arr* msg))
Cyc_Interface_disj_merge_dict)( ie1->vardecls, ie2->vardecls, _tag_arr("variable",
sizeof( unsigned char), 9u), msg); struct Cyc_Core_Opt* _temp815=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t,
struct _tagged_arr* msg)) Cyc_Interface_disj_merge_dict)( ie1->xtunionfielddecls,
ie2->xtunionfielddecls, _tag_arr("xtunionfield", sizeof( unsigned char), 13u),
msg); if(((((( !(( unsigned int) _temp809)? 1: !(( unsigned int) _temp810))? 1:
!(( unsigned int) _temp811))? 1: !(( unsigned int) _temp812))? 1: !((
unsigned int) _temp813))? 1: !(( unsigned int) _temp814))? 1: !(( unsigned int)
_temp815)){ return 0;} else{ return({ struct Cyc_Interface_Ienv* _temp816=(
struct Cyc_Interface_Ienv*) GC_malloc( sizeof( struct Cyc_Interface_Ienv));
_temp816->structdecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp809))->v; _temp816->uniondecls=( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp810))->v; _temp816->tuniondecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp811))->v; _temp816->enumdecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp812))->v;
_temp816->typedefdecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp813))->v; _temp816->vardecls=( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( _temp814))->v; _temp816->xtunionfielddecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp815))->v; _temp816;});}}
struct _tuple18{ int f1; struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3;
struct _tagged_arr* f4; } ; void Cyc_Interface_disj_merge_ns_f( struct _tuple18*
env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie2){ struct _tuple18
_temp819; struct _tagged_arr* _temp820; struct Cyc_Dict_Dict* _temp822; struct
Cyc_Dict_Dict* _temp824; struct Cyc_Dict_Dict** _temp826; int _temp827; int*
_temp829; struct _tuple18* _temp817= env; _temp819=* _temp817; _LL828: _temp827=
_temp819.f1; _temp829=&(* _temp817).f1; goto _LL825; _LL825: _temp824= _temp819.f2;
_temp826=&(* _temp817).f2; goto _LL823; _LL823: _temp822= _temp819.f3; goto
_LL821; _LL821: _temp820= _temp819.f4; goto _LL818; _LL818: { struct Cyc_Interface_Ienv*
ie1;{ struct _handler_cons _temp830; _push_handler(& _temp830);{ int _temp832= 0;
if( setjmp( _temp830.handler)){ _temp832= 1;} if( ! _temp832){ ie1=(( struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp822,
ns);; _pop_handler();} else{ void* _temp831=( void*) _exn_thrown; void* _temp834=
_temp831; _LL836: if( _temp834 == Cyc_Dict_Absent){ goto _LL837;} else{ goto
_LL838;} _LL838: goto _LL839; _LL837: ie1= Cyc_Interface_lazy_new_ienv(); goto
_LL835; _LL839:( void) _throw( _temp834); _LL835:;}}}{ struct Cyc_Interface_Ienv*
_temp840= Cyc_Interface_disj_merge_ienv( ie1, ie2, _temp820); if( _temp840 == 0){*
_temp829= 0;} else{ if(* _temp829){* _temp826=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Interface_Ienv*
data)) Cyc_Dict_insert)(* _temp826, ns,( struct Cyc_Interface_Ienv*) _check_null(
_temp840));}}}}} struct Cyc_Core_Opt* Cyc_Interface_disj_merge_ns( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr* msg){ struct _tuple18
_temp841=({ struct _tuple18 _temp843; _temp843.f1= 1; _temp843.f2= dic1;
_temp843.f3= dic1; _temp843.f4= msg; _temp843;});(( void(*)( void(* f)( struct
_tuple18*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct _tuple18*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_disj_merge_ns_f,&
_temp841, dic2); if( _temp841.f1){ return({ struct Cyc_Core_Opt* _temp842=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp842->v=(
void*) _temp841.f2; _temp842;});} else{ return 0;}} struct Cyc_Interface_I* Cyc_Interface_merge(
struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple3* info){
struct _handler_cons _temp844; _push_handler(& _temp844);{ int _temp846= 0; if(
setjmp( _temp844.handler)){ _temp846= 1;} if( ! _temp846){{ struct _tagged_arr*
msg= 0; if( info != 0){ msg=({ struct _tagged_arr* _temp847=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp847[ 0]=( struct _tagged_arr)({
struct _tagged_arr(* _temp848)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp853= _tag_arr("merging exports of %s and %s,", sizeof(
unsigned char), 30u); struct Cyc_Stdio_String_pa_struct _temp855; _temp855.tag=
Cyc_Stdio_String_pa; _temp855.f1=( struct _tagged_arr)(*(( struct _tuple3*)
_check_null( info))).f1;{ void* _temp851=( void*)& _temp855; struct Cyc_Stdio_String_pa_struct
_temp854; _temp854.tag= Cyc_Stdio_String_pa; _temp854.f1=( struct _tagged_arr)(*((
struct _tuple3*) _check_null( info))).f2;{ void* _temp852=( void*)& _temp854;
void* _temp849[ 2u]={ _temp851, _temp852}; struct _tagged_arr _temp850={( void*)
_temp849,( void*) _temp849,( void*)( _temp849 + 2u)}; _temp848( _temp853,
_temp850);}}}); _temp847;});}{ struct Cyc_Core_Opt* _temp856= Cyc_Interface_disj_merge_ns(
i1->exports, i2->exports, msg); if( _temp856 == 0){ struct Cyc_Interface_I*
_temp857= 0; _npop_handler( 0u); return _temp857;} if( info != 0){ msg=({ struct
_tagged_arr* _temp858=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp858[ 0]=( struct _tagged_arr)({ struct _tagged_arr(*
_temp859)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp864= _tag_arr("merging imports of %s and %s,", sizeof(
unsigned char), 30u); struct Cyc_Stdio_String_pa_struct _temp866; _temp866.tag=
Cyc_Stdio_String_pa; _temp866.f1=( struct _tagged_arr)(*(( struct _tuple3*)
_check_null( info))).f1;{ void* _temp862=( void*)& _temp866; struct Cyc_Stdio_String_pa_struct
_temp865; _temp865.tag= Cyc_Stdio_String_pa; _temp865.f1=( struct _tagged_arr)(*((
struct _tuple3*) _check_null( info))).f2;{ void* _temp863=( void*)& _temp865;
void* _temp860[ 2u]={ _temp862, _temp863}; struct _tagged_arr _temp861={( void*)
_temp860,( void*) _temp860,( void*)( _temp860 + 2u)}; _temp859( _temp864,
_temp861);}}}); _temp858;});}{ struct Cyc_Core_Opt* _temp867= Cyc_Interface_compat_merge_ns(
i1->imports, i2->imports,( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp856))->v, msg); if( _temp867 == 0){ struct Cyc_Interface_I*
_temp868= 0; _npop_handler( 0u); return _temp868;}{ struct Cyc_Interface_I*
_temp870=({ struct Cyc_Interface_I* _temp869=( struct Cyc_Interface_I*)
GC_malloc( sizeof( struct Cyc_Interface_I)); _temp869->imports=( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp867))->v; _temp869->exports=( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp856))->v; _temp869;}); _npop_handler( 0u);
return _temp870;}}}}; _pop_handler();} else{ void* _temp845=( void*) _exn_thrown;
void* _temp872= _temp845; _LL874: if( _temp872 == Cyc_Tcdecl_Incompatible){ goto
_LL875;} else{ goto _LL876;} _LL876: goto _LL877; _LL875: return 0; _LL877:(
void) _throw( _temp872); _LL873:;}}} struct Cyc_Interface_I* Cyc_Interface_merge_list(
struct Cyc_List_List* li, struct Cyc_List_List* linfo){ if( li == 0){ return(
struct Cyc_Interface_I*) Cyc_Interface_empty();}{ struct Cyc_Interface_I* curr_i=(
struct Cyc_Interface_I*)(( struct Cyc_List_List*) _check_null( li))->hd; struct
_tagged_arr* curr_info= linfo != 0?( struct _tagged_arr*)(( struct _tagged_arr*)((
struct Cyc_List_List*) _check_null( linfo))->hd): 0; li=(( struct Cyc_List_List*)
_check_null( li))->tl; if( linfo != 0){ linfo=(( struct Cyc_List_List*)
_check_null( linfo))->tl;} for( 0; li != 0; li=(( struct Cyc_List_List*)
_check_null( li))->tl){ struct Cyc_Interface_I* _temp878= Cyc_Interface_merge(
curr_i,( struct Cyc_Interface_I*)(( struct Cyc_List_List*) _check_null( li))->hd,(
curr_info != 0? linfo != 0: 0)?({ struct _tuple3* _temp888=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp888->f1=*(( struct _tagged_arr*)
_check_null( curr_info)); _temp888->f2=*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( linfo))->hd); _temp888;}): 0); if( _temp878 == 0){ return 0;}
curr_i=( struct Cyc_Interface_I*) _check_null( _temp878); if( curr_info != 0){
curr_info= linfo != 0?({ struct _tagged_arr* _temp879=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp879[ 0]=( struct _tagged_arr)({
struct _tagged_arr(* _temp880)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp885= _tag_arr("%s+%s", sizeof( unsigned char), 6u);
struct Cyc_Stdio_String_pa_struct _temp887; _temp887.tag= Cyc_Stdio_String_pa;
_temp887.f1=( struct _tagged_arr)*(( struct _tagged_arr*) _check_null( curr_info));{
void* _temp883=( void*)& _temp887; struct Cyc_Stdio_String_pa_struct _temp886;
_temp886.tag= Cyc_Stdio_String_pa; _temp886.f1=( struct _tagged_arr)*(( struct
_tagged_arr*)(( struct Cyc_List_List*) _check_null( linfo))->hd);{ void*
_temp884=( void*)& _temp886; void* _temp881[ 2u]={ _temp883, _temp884}; struct
_tagged_arr _temp882={( void*) _temp881,( void*) _temp881,( void*)( _temp881 + 2u)};
_temp880( _temp885, _temp882);}}}); _temp879;}): 0;} if( linfo != 0){ linfo=((
struct Cyc_List_List*) _check_null( linfo))->tl;}} return( struct Cyc_Interface_I*)
curr_i;}} struct Cyc_Interface_I* Cyc_Interface_get_and_merge_list( struct Cyc_Interface_I*(*
get)( void*), struct Cyc_List_List* la, struct Cyc_List_List* linfo){ if( la ==
0){ return( struct Cyc_Interface_I*) Cyc_Interface_empty();}{ struct Cyc_Interface_I*
curr_i= get(( void*)(( struct Cyc_List_List*) _check_null( la))->hd); struct
_tagged_arr* curr_info= linfo != 0?( struct _tagged_arr*)(( struct _tagged_arr*)((
struct Cyc_List_List*) _check_null( linfo))->hd): 0; la=(( struct Cyc_List_List*)
_check_null( la))->tl; if( linfo != 0){ linfo=(( struct Cyc_List_List*)
_check_null( linfo))->tl;} for( 0; la != 0; la=(( struct Cyc_List_List*)
_check_null( la))->tl){ struct Cyc_Interface_I* _temp889= Cyc_Interface_merge(
curr_i, get(( void*)(( struct Cyc_List_List*) _check_null( la))->hd),( curr_info
!= 0? linfo != 0: 0)?({ struct _tuple3* _temp899=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp899->f1=*(( struct _tagged_arr*) _check_null(
curr_info)); _temp899->f2=*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( linfo))->hd); _temp899;}): 0); if( _temp889 == 0){ return 0;}
curr_i=( struct Cyc_Interface_I*) _check_null( _temp889); if( curr_info != 0){
curr_info= linfo != 0?({ struct _tagged_arr* _temp890=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp890[ 0]=( struct _tagged_arr)({
struct _tagged_arr(* _temp891)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp896= _tag_arr("%s+%s", sizeof( unsigned char), 6u);
struct Cyc_Stdio_String_pa_struct _temp898; _temp898.tag= Cyc_Stdio_String_pa;
_temp898.f1=( struct _tagged_arr)*(( struct _tagged_arr*) _check_null( curr_info));{
void* _temp894=( void*)& _temp898; struct Cyc_Stdio_String_pa_struct _temp897;
_temp897.tag= Cyc_Stdio_String_pa; _temp897.f1=( struct _tagged_arr)*(( struct
_tagged_arr*)(( struct Cyc_List_List*) _check_null( linfo))->hd);{ void*
_temp895=( void*)& _temp897; void* _temp892[ 2u]={ _temp894, _temp895}; struct
_tagged_arr _temp893={( void*) _temp892,( void*) _temp892,( void*)( _temp892 + 2u)};
_temp891( _temp896, _temp893);}}}); _temp890;}): 0;} if( linfo != 0){ linfo=((
struct Cyc_List_List*) _check_null( linfo))->tl;}} return( struct Cyc_Interface_I*)
curr_i;}} static struct Cyc_List_List* Cyc_Interface_add_namespace( struct Cyc_List_List*
tds, struct Cyc_List_List* ns){ if( ns == 0){ return tds;} return({ struct Cyc_List_List*
_temp900=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp900->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Namespace_d_struct*
_temp901=( struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp901[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp902; _temp902.tag= Cyc_Absyn_Namespace_d;
_temp902.f1=( struct _tagged_arr*)(( struct Cyc_List_List*) _check_null( ns))->hd;
_temp902.f2= Cyc_Interface_add_namespace( tds,(( struct Cyc_List_List*)
_check_null( ns))->tl); _temp902;}); _temp901;}), 0); _temp900->tl= 0; _temp900;});}
static struct Cyc_List_List* Cyc_Interface_add_structdecl( struct _tagged_arr* x,
struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp903=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp903->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp904=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp904[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp905; _temp905.tag= Cyc_Absyn_Struct_d;
_temp905.f1= d; _temp905;}); _temp904;}), 0); _temp903->tl= tds; _temp903;});}
static struct Cyc_List_List* Cyc_Interface_add_structdecl_header( struct
_tagged_arr* x, struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Structdecl* _temp906=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp906[ 0]=* d; _temp906;}); d->fields=
0; if(( void*) d->sc !=( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)((
void*) Cyc_Absyn_Extern));} return({ struct Cyc_List_List* _temp907=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp907->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp908=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp908[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp909; _temp909.tag= Cyc_Absyn_Struct_d; _temp909.f1=
d; _temp909;}); _temp908;}), 0); _temp907->tl= tds; _temp907;});} static struct
Cyc_List_List* Cyc_Interface_add_uniondecl( struct _tagged_arr* x, struct Cyc_Absyn_Uniondecl*
d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List* _temp910=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp910->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Union_d_struct* _temp911=( struct
Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp911[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp912; _temp912.tag= Cyc_Absyn_Union_d;
_temp912.f1= d; _temp912;}); _temp911;}), 0); _temp910->tl= tds; _temp910;});}
static struct Cyc_List_List* Cyc_Interface_add_uniondecl_header( struct
_tagged_arr* x, struct Cyc_Absyn_Uniondecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Uniondecl* _temp913=( struct Cyc_Absyn_Uniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp913[ 0]=* d; _temp913;}); d->fields=
0; if(( void*) d->sc !=( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)((
void*) Cyc_Absyn_Extern));} return({ struct Cyc_List_List* _temp914=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp914->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Union_d_struct* _temp915=( struct Cyc_Absyn_Union_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct)); _temp915[ 0]=({ struct Cyc_Absyn_Union_d_struct
_temp916; _temp916.tag= Cyc_Absyn_Union_d; _temp916.f1= d; _temp916;}); _temp915;}),
0); _temp914->tl= tds; _temp914;});} static struct Cyc_List_List* Cyc_Interface_add_tuniondecl(
struct _tagged_arr* x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){
return({ struct Cyc_List_List* _temp917=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp917->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Tunion_d_struct* _temp918=( struct Cyc_Absyn_Tunion_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct)); _temp918[ 0]=({ struct
Cyc_Absyn_Tunion_d_struct _temp919; _temp919.tag= Cyc_Absyn_Tunion_d; _temp919.f1=
d; _temp919;}); _temp918;}), 0); _temp917->tl= tds; _temp917;});} static
unsigned char _temp920[ 2u]="_"; static struct _tagged_arr Cyc_Interface_us={
_temp920, _temp920, _temp920 + 2u}; static struct _tagged_arr* Cyc_Interface_us_p=&
Cyc_Interface_us; static struct _tuple10* Cyc_Interface_rewrite_tunionfield_type(
struct _tuple10* x){ return({ struct _tuple10* _temp921=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp921->f1= Cyc_Absyn_empty_tqual();
_temp921->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp922=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp922[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp923; _temp923.tag= Cyc_Absyn_VarType; _temp923.f1=({ struct Cyc_Absyn_Tvar*
_temp924=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp924->name= Cyc_Interface_us_p; _temp924->identity= 0; _temp924->kind= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_BoxKind); _temp924;}); _temp923;}); _temp922;}); _temp921;});}
static struct Cyc_Absyn_Tunionfield* Cyc_Interface_rewrite_tunionfield( struct
Cyc_Absyn_Tunionfield* f){ f=({ struct Cyc_Absyn_Tunionfield* _temp925=( struct
Cyc_Absyn_Tunionfield*) GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield));
_temp925[ 0]=* f; _temp925;}); f->typs=(( struct Cyc_List_List*(*)( struct
_tuple10*(* f)( struct _tuple10*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Interface_rewrite_tunionfield_type,
f->typs); return f;} static struct Cyc_List_List* Cyc_Interface_add_tuniondecl_header(
struct _tagged_arr* x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){
d=({ struct Cyc_Absyn_Tuniondecl* _temp926=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp926[ 0]=* d; _temp926;});
if( d->fields != 0){ d->fields=({ struct Cyc_Core_Opt* _temp927=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp927->v=( void*)(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tunionfield*(* f)( struct Cyc_Absyn_Tunionfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Interface_rewrite_tunionfield,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( d->fields))->v); _temp927;});} if(( void*) d->sc
!=( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern));}
return({ struct Cyc_List_List* _temp928=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp928->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Tunion_d_struct* _temp929=( struct Cyc_Absyn_Tunion_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct)); _temp929[ 0]=({ struct
Cyc_Absyn_Tunion_d_struct _temp930; _temp930.tag= Cyc_Absyn_Tunion_d; _temp930.f1=
d; _temp930;}); _temp929;}), 0); _temp928->tl= tds; _temp928;});} static struct
Cyc_List_List* Cyc_Interface_add_enumdecl( struct _tagged_arr* x, struct Cyc_Absyn_Enumdecl*
d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List* _temp931=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp931->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Enum_d_struct* _temp932=( struct
Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp932[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp933; _temp933.tag= Cyc_Absyn_Enum_d;
_temp933.f1= d; _temp933;}); _temp932;}), 0); _temp931->tl= tds; _temp931;});}
static struct Cyc_List_List* Cyc_Interface_add_enumdecl_header( struct
_tagged_arr* x, struct Cyc_Absyn_Enumdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Enumdecl* _temp934=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp934[ 0]=* d; _temp934;}); d->fields= 0;
if(( void*) d->sc !=( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)(( void*)
Cyc_Absyn_Extern));} return({ struct Cyc_List_List* _temp935=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp935->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Enum_d_struct* _temp936=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp936[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp937; _temp937.tag= Cyc_Absyn_Enum_d; _temp937.f1= d; _temp937;}); _temp936;}),
0); _temp935->tl= tds; _temp935;});} static struct Cyc_List_List* Cyc_Interface_add_typedef(
struct _tagged_arr* x, struct Cyc_Absyn_Typedefdecl* d, struct Cyc_List_List*
tds){ return({ struct Cyc_List_List* _temp938=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp938->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Typedef_d_struct* _temp939=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp939[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp940; _temp940.tag= Cyc_Absyn_Typedef_d; _temp940.f1=
d; _temp940;}); _temp939;}), 0); _temp938->tl= tds; _temp938;});} static struct
Cyc_List_List* Cyc_Interface_add_vardecl( struct _tagged_arr* x, struct Cyc_Absyn_Vardecl*
d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List* _temp941=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp941->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp942=( struct
Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp942[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp943; _temp943.tag= Cyc_Absyn_Var_d;
_temp943.f1= d; _temp943;}); _temp942;}), 0); _temp941->tl= tds; _temp941;});}
static struct Cyc_List_List* Cyc_Interface_add_xtunionfielddecl( struct
_tagged_arr* x, struct Cyc_Tcdecl_Xtunionfielddecl* d, struct Cyc_List_List* tds){
struct Cyc_Tcdecl_Xtunionfielddecl _temp946; struct Cyc_Absyn_Tunionfield*
_temp947; struct Cyc_Absyn_Tuniondecl* _temp949; struct Cyc_Tcdecl_Xtunionfielddecl*
_temp944= d; _temp946=* _temp944; _LL950: _temp949= _temp946.base; goto _LL948;
_LL948: _temp947= _temp946.field; goto _LL945; _LL945: _temp949=({ struct Cyc_Absyn_Tuniondecl*
_temp951=( struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp951[ 0]=* _temp949; _temp951;}); _temp949->fields=({ struct Cyc_Core_Opt*
_temp952=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp952->v=( void*)({ struct Cyc_List_List* _temp953=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp953->hd=( void*) _temp947;
_temp953->tl= 0; _temp953;}); _temp952;});( void*)( _temp949->sc=( void*)(( void*)
Cyc_Absyn_Extern)); return({ struct Cyc_List_List* _temp954=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp954->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp955=( struct Cyc_Absyn_Tunion_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct)); _temp955[ 0]=({ struct
Cyc_Absyn_Tunion_d_struct _temp956; _temp956.tag= Cyc_Absyn_Tunion_d; _temp956.f1=
_temp949; _temp956;}); _temp955;}), 0); _temp954->tl= tds; _temp954;});} static
struct Cyc_List_List* Cyc_Interface_add_xtunionfielddecl_header( struct
_tagged_arr* x, struct Cyc_Tcdecl_Xtunionfielddecl* d, struct Cyc_List_List* tds){
struct Cyc_Tcdecl_Xtunionfielddecl _temp959; struct Cyc_Absyn_Tunionfield*
_temp960; struct Cyc_Absyn_Tuniondecl* _temp962; struct Cyc_Tcdecl_Xtunionfielddecl*
_temp957= d; _temp959=* _temp957; _LL963: _temp962= _temp959.base; goto _LL961;
_LL961: _temp960= _temp959.field; goto _LL958; _LL958: _temp962=({ struct Cyc_Absyn_Tuniondecl*
_temp964=( struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp964[ 0]=* _temp962; _temp964;}); _temp960= Cyc_Interface_rewrite_tunionfield(
_temp960);( void*)( _temp960->sc=( void*)(( void*) Cyc_Absyn_Extern)); _temp962->fields=({
struct Cyc_Core_Opt* _temp965=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp965->v=( void*)({ struct Cyc_List_List* _temp966=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp966->hd=( void*)
_temp960; _temp966->tl= 0; _temp966;}); _temp965;});( void*)( _temp962->sc=(
void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List* _temp967=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp967->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp968=(
struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp968[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp969; _temp969.tag= Cyc_Absyn_Tunion_d;
_temp969.f1= _temp962; _temp969;}); _temp968;}), 0); _temp967->tl= tds; _temp967;});}
static void Cyc_Interface_print_ns_headers( struct Cyc_Stdio___sFILE* f, struct
Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie){ struct Cyc_List_List*
_temp970= 0; _temp970=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)(
struct _tagged_arr*, struct Cyc_Absyn_Structdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_structdecl_header,
ie->structdecls, _temp970); _temp970=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl_header,
ie->uniondecls, _temp970); _temp970=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl_header,
ie->tuniondecls, _temp970); _temp970=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl_header,
ie->enumdecls, _temp970); if( _temp970 != 0){ _temp970=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp970); _temp970= Cyc_Interface_add_namespace(
_temp970, ns); Cyc_Absynpp_decllist2file( _temp970, f);}} static void Cyc_Interface_print_ns_xtunionfielddecl_headers(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp971= 0; _temp971=(( struct Cyc_List_List*(*)(
struct Cyc_List_List*(* f)( struct _tagged_arr*, struct Cyc_Tcdecl_Xtunionfielddecl*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)( Cyc_Interface_add_xtunionfielddecl_header, ie->xtunionfielddecls,
_temp971); if( _temp971 != 0){ _temp971=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp971); _temp971= Cyc_Interface_add_namespace(
_temp971, ns); Cyc_Absynpp_decllist2file( _temp971, f);}} static void Cyc_Interface_print_ns_typedefs(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp972= 0; _temp972=(( struct Cyc_List_List*(*)(
struct Cyc_List_List*(* f)( struct _tagged_arr*, struct Cyc_Absyn_Typedefdecl*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)( Cyc_Interface_add_typedef, ie->typedefdecls, _temp972); if(
_temp972 != 0){ _temp972=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( _temp972); _temp972= Cyc_Interface_add_namespace( _temp972,
ns); Cyc_Absynpp_decllist2file( _temp972, f);}} static void Cyc_Interface_print_ns_decls(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp973= 0; _temp973=(( struct Cyc_List_List*(*)(
struct Cyc_List_List*(* f)( struct _tagged_arr*, struct Cyc_Absyn_Structdecl*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)( Cyc_Interface_add_structdecl, ie->structdecls, _temp973);
_temp973=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct
_tagged_arr*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict*
d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl, ie->uniondecls,
_temp973); _temp973=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)(
struct _tagged_arr*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl,
ie->tuniondecls, _temp973); _temp973=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl,
ie->enumdecls, _temp973); _temp973=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Vardecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_vardecl,
ie->vardecls, _temp973); _temp973=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_xtunionfielddecl,
ie->xtunionfielddecls, _temp973); if( _temp973 != 0){ _temp973=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp973); _temp973= Cyc_Interface_add_namespace(
_temp973, ns); Cyc_Absynpp_decllist2file( _temp973, f);}} void Cyc_Interface_print(
struct Cyc_Interface_I* i, struct Cyc_Stdio___sFILE* f){ Cyc_Absynpp_set_params(&
Cyc_Absynpp_cyci_params_r);({ int(* _temp974)( struct Cyc_Stdio___sFILE*, struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp977= f; struct _tagged_arr _temp978= _tag_arr("/****** needed (headers) ******/\n",
sizeof( unsigned char), 34u); void* _temp975[ 0u]={}; struct _tagged_arr
_temp976={( void*) _temp975,( void*) _temp975,( void*)( _temp975 + 0u)};
_temp974( _temp977, _temp978, _temp976);});(( void(*)( void(* f)( struct Cyc_Stdio___sFILE*,
struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct Cyc_Stdio___sFILE*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_headers,
f, i->imports);({ int(* _temp979)( struct Cyc_Stdio___sFILE*, struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp982=
f; struct _tagged_arr _temp983= _tag_arr("\n/****** provided (headers) ******/\n",
sizeof( unsigned char), 37u); void* _temp980[ 0u]={}; struct _tagged_arr
_temp981={( void*) _temp980,( void*) _temp980,( void*)( _temp980 + 0u)};
_temp979( _temp982, _temp983, _temp981);});(( void(*)( void(* f)( struct Cyc_Stdio___sFILE*,
struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct Cyc_Stdio___sFILE*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_headers,
f, i->exports);({ int(* _temp984)( struct Cyc_Stdio___sFILE*, struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp987=
f; struct _tagged_arr _temp988= _tag_arr("/****** needed (headers of xtunionfielddecls) ******/\n",
sizeof( unsigned char), 55u); void* _temp985[ 0u]={}; struct _tagged_arr
_temp986={( void*) _temp985,( void*) _temp985,( void*)( _temp985 + 0u)};
_temp984( _temp987, _temp988, _temp986);});(( void(*)( void(* f)( struct Cyc_Stdio___sFILE*,
struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct Cyc_Stdio___sFILE*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_xtunionfielddecl_headers,
f, i->imports);({ int(* _temp989)( struct Cyc_Stdio___sFILE*, struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp992=
f; struct _tagged_arr _temp993= _tag_arr("\n/****** provided (headers of xtunionfielddecls) ******/\n",
sizeof( unsigned char), 58u); void* _temp990[ 0u]={}; struct _tagged_arr
_temp991={( void*) _temp990,( void*) _temp990,( void*)( _temp990 + 0u)};
_temp989( _temp992, _temp993, _temp991);});(( void(*)( void(* f)( struct Cyc_Stdio___sFILE*,
struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct Cyc_Stdio___sFILE*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_xtunionfielddecl_headers,
f, i->exports);({ int(* _temp994)( struct Cyc_Stdio___sFILE*, struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp997=
f; struct _tagged_arr _temp998= _tag_arr("\n/****** provided (typedefs) ******/\n",
sizeof( unsigned char), 38u); void* _temp995[ 0u]={}; struct _tagged_arr
_temp996={( void*) _temp995,( void*) _temp995,( void*)( _temp995 + 0u)};
_temp994( _temp997, _temp998, _temp996);});(( void(*)( void(* f)( struct Cyc_Stdio___sFILE*,
struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct Cyc_Stdio___sFILE*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_typedefs,
f, i->exports);({ int(* _temp999)( struct Cyc_Stdio___sFILE*, struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp1002=
f; struct _tagged_arr _temp1003= _tag_arr("\n/****** needed (declarations) ******/\n",
sizeof( unsigned char), 40u); void* _temp1000[ 0u]={}; struct _tagged_arr
_temp1001={( void*) _temp1000,( void*) _temp1000,( void*)( _temp1000 + 0u)};
_temp999( _temp1002, _temp1003, _temp1001);});(( void(*)( void(* f)( struct Cyc_Stdio___sFILE*,
struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct Cyc_Stdio___sFILE*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_decls, f,
i->imports);({ int(* _temp1004)( struct Cyc_Stdio___sFILE*, struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp1007=
f; struct _tagged_arr _temp1008= _tag_arr("\n/****** provided (declarations) ******/\n",
sizeof( unsigned char), 42u); void* _temp1005[ 0u]={}; struct _tagged_arr
_temp1006={( void*) _temp1005,( void*) _temp1005,( void*)( _temp1005 + 0u)};
_temp1004( _temp1007, _temp1008, _temp1006);});(( void(*)( void(* f)( struct Cyc_Stdio___sFILE*,
struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct Cyc_Stdio___sFILE*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_decls, f,
i->exports);} struct Cyc_Interface_I* Cyc_Interface_parse( struct Cyc_Stdio___sFILE*
f){ Cyc_Lex_lex_init();{ struct Cyc_List_List* _temp1009= Cyc_Parse_parse_file(
f); struct Cyc_Tcenv_Tenv* _temp1010= Cyc_Tcenv_tc_init(); Cyc_Tc_tc( _temp1010,
0, _temp1009); return Cyc_Interface_gen_extract( _temp1010->ae, 0);}} void Cyc_Interface_save(
struct Cyc_Interface_I* i, struct Cyc_Stdio___sFILE* f){ Cyc_Interface_print( i,
f);} struct Cyc_Interface_I* Cyc_Interface_load( struct Cyc_Stdio___sFILE* f){
return Cyc_Interface_parse( f);}
