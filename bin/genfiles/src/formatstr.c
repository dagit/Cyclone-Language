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
unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern unsigned char Cyc_List_Nth[ 8u]; extern int
isalnum( int __c); extern int isalpha( int __c); extern int iscntrl( int __c);
extern int isdigit( int __c); extern int isgraph( int __c); extern int islower(
int __c); extern int isprint( int __c); extern int ispunct( int __c); extern int
isspace( int __c); extern int isupper( int __c); extern int isxdigit( int __c);
extern int tolower( int __c); extern int toupper( int __c); extern int isascii(
int __c); extern int toascii( int __c); extern int _tolower( int __c); extern
int _toupper( int __c); struct Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[
19u]; extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
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
int tag; float* f1; } ; extern int Cyc_String_strlen( struct _tagged_arr s);
extern struct _tagged_arr Cyc_String_implode( struct Cyc_List_List* c); struct
Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct _tagged_arr line; int
line_no; int col; } ; extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment;
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
15u]; extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern void* Cyc_Absyn_new_evar(
struct Cyc_Core_Opt* k, struct Cyc_Core_Opt* tenv); extern void* Cyc_Absyn_uchar_t;
extern void* Cyc_Absyn_ushort_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulong_t;
extern void* Cyc_Absyn_schar_t; extern void* Cyc_Absyn_sshort_t; extern void*
Cyc_Absyn_sint_t; extern void* Cyc_Absyn_slong_t; extern void* Cyc_Absyn_float_t;
extern void* Cyc_Absyn_double_t; extern void* Cyc_Absyn_const_string_typ( void*
rgn); extern void* Cyc_Absyn_at_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual
tq); struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict;
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
extern struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars( struct Cyc_Tcenv_Tenv*);
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap);
extern struct Cyc_List_List* Cyc_Formatstr_get_format_typs( struct Cyc_Tcenv_Tenv*,
struct _tagged_arr, struct Cyc_Position_Segment*); extern struct Cyc_List_List*
Cyc_Formatstr_get_scanf_typs( struct Cyc_Tcenv_Tenv*, struct _tagged_arr, struct
Cyc_Position_Segment*); extern struct Cyc_Core_Opt* Cyc_Formatstr_parse_conversionspecification(
struct _tagged_arr s, int i); static struct Cyc_Core_Opt Cyc_Formatstr_rk={(
void*)(( void*) 3u)}; struct _tuple3{ struct Cyc_List_List* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; struct Cyc_List_List* f4; unsigned char f5; int f6;
} ; struct Cyc_Core_Opt* Cyc_Formatstr_parse_conversionspecification( struct
_tagged_arr s, int i){ int _temp0= Cyc_String_strlen( s); if( i < 0? 1: i >=
_temp0){ return 0;}{ struct Cyc_List_List* _temp1= 0; unsigned char c=' '; for(
0; i < _temp0; i ++){ c=*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), i)); switch( c){ case '+': _LL2: goto _LL3; case '-':
_LL3: goto _LL4; case ' ': _LL4: goto _LL5; case '#': _LL5: goto _LL6; case '0':
_LL6: _temp1=({ struct Cyc_List_List* _temp8=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp8->hd=( void*)(( int) c); _temp8->tl=
_temp1; _temp8;}); continue; default: _LL7: break;} break;} if( i >= _temp0){
return 0;} _temp1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp1);{ struct Cyc_List_List* _temp10= 0; c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( c =='*'){ _temp10=({
struct Cyc_List_List* _temp11=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp11->hd=( void*)(( int) c); _temp11->tl= _temp10; _temp11;});
i ++;} else{ for( 0; i < _temp0; i ++){ c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( isdigit(( int) c)){
_temp10=({ struct Cyc_List_List* _temp12=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp12->hd=( void*)(( int) c); _temp12->tl=
_temp10; _temp12;});} else{ break;}}} if( i >= _temp0){ return 0;} _temp10=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp10);{
struct Cyc_List_List* _temp13= 0; c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( c =='.'){ _temp13=({
struct Cyc_List_List* _temp14=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp14->hd=( void*)(( int) c); _temp14->tl= _temp13; _temp14;});
i ++; if( i >= _temp0){ return 0;} c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( c =='*'){ _temp13=({
struct Cyc_List_List* _temp15=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp15->hd=( void*)(( int) c); _temp15->tl= _temp13; _temp15;});
i ++;} else{ for( 0; i < _temp0; i ++){ c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( isdigit(( int) c)){
_temp13=({ struct Cyc_List_List* _temp16=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp16->hd=( void*)(( int) c); _temp16->tl=
_temp13; _temp16;});} else{ break;}}}} if( i >= _temp0){ return 0;} _temp13=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp13);{
struct Cyc_List_List* _temp17= 0; c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); switch( c){ case 'h':
_LL18: _temp17=({ struct Cyc_List_List* _temp20=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp20->hd=( void*)(( int) c);
_temp20->tl= _temp17; _temp20;}); i ++; if( i >= _temp0){ return 0;} c=*(( const
unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i)); if( c
=='h'){ _temp17=({ struct Cyc_List_List* _temp21=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp21->hd=( void*)(( int) c);
_temp21->tl= _temp17; _temp21;}); i ++;} break; case 'l': _LL19: _temp17=({
struct Cyc_List_List* _temp23=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp23->hd=( void*)(( int) c); _temp23->tl= _temp17; _temp23;});
i ++; if( i >= _temp0){ return 0;} c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( c =='l'){ _temp17=({
struct Cyc_List_List* _temp24=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp24->hd=( void*)(( int) c); _temp24->tl= _temp17; _temp24;});
i ++;} break; case 'j': _LL22: goto _LL25; case 'z': _LL25: goto _LL26; case 't':
_LL26: goto _LL27; case 'L': _LL27: _temp17=({ struct Cyc_List_List* _temp29=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp29->hd=(
void*)(( int) c); _temp29->tl= _temp17; _temp29;}); i ++; break; default: _LL28:
break;} if( i >= _temp0){ return 0;} _temp17=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( _temp17); c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); switch( c){ case 'd':
_LL31: break; case 'i': _LL32: break; case 'o': _LL33: break; case 'u': _LL34:
break; case 'x': _LL35: break; case 'X': _LL36: break; case 'f': _LL37: break;
case 'F': _LL38: break; case 'e': _LL39: break; case 'E': _LL40: break; case 'g':
_LL41: break; case 'G': _LL42: break; case 'a': _LL43: break; case 'A': _LL44:
break; case 'c': _LL45: break; case 's': _LL46: break; case 'p': _LL47: break;
case 'n': _LL48: break; case '%': _LL49: break; default: _LL50: return 0;}
return({ struct Cyc_Core_Opt* _temp52=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp52->v=( void*)({ struct _tuple3* _temp53=( struct
_tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp53->f1= _temp1; _temp53->f2=
_temp10; _temp53->f3= _temp13; _temp53->f4= _temp17; _temp53->f5= c; _temp53->f6=
i + 1; _temp53;}); _temp52;});}}}}} struct Cyc_List_List* Cyc_Formatstr_get_format_typs(
struct Cyc_Tcenv_Tenv* te, struct _tagged_arr s, struct Cyc_Position_Segment*
loc){ int _temp54= Cyc_String_strlen( s); struct Cyc_List_List* _temp55= 0; int
i; for( i= 0; i < _temp54; i ++){ if(*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)) !='%'){ continue;}{
struct Cyc_Core_Opt* _temp56= Cyc_Formatstr_parse_conversionspecification( s, i
+ 1); if( _temp56 == 0){({ void(* _temp57)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp60= loc; struct _tagged_arr _temp61= _tag_arr("bad format string", sizeof(
unsigned char), 18u); void* _temp58[ 0u]={}; struct _tagged_arr _temp59={( void*)
_temp58,( void*) _temp58,( void*)( _temp58 + 0u)}; _temp57( _temp60, _temp61,
_temp59);}); return 0;}{ struct _tuple3 _temp64; int _temp65; unsigned char
_temp67; struct Cyc_List_List* _temp69; struct Cyc_List_List* _temp71; struct
Cyc_List_List* _temp73; struct Cyc_List_List* _temp75; struct _tuple3* _temp62=(
struct _tuple3*)(( struct Cyc_Core_Opt*) _check_null( _temp56))->v; _temp64=*
_temp62; _LL76: _temp75= _temp64.f1; goto _LL74; _LL74: _temp73= _temp64.f2;
goto _LL72; _LL72: _temp71= _temp64.f3; goto _LL70; _LL70: _temp69= _temp64.f4;
goto _LL68; _LL68: _temp67= _temp64.f5; goto _LL66; _LL66: _temp65= _temp64.f6;
goto _LL63; _LL63: i= _temp65 - 1;{ struct Cyc_List_List* _temp77= _temp69;
struct Cyc_List_List _temp84; struct Cyc_List_List* _temp85; int _temp87; _LL79:
if( _temp77 == 0){ goto _LL81;} else{ _temp84=* _temp77; _LL88: _temp87=( int)
_temp84.hd; goto _LL86; _LL86: _temp85= _temp84.tl; if( _temp85 == 0){ goto
_LL83;} else{ goto _LL81;}} _LL83: if(( _temp87 =='j'? 1: _temp87 =='z')? 1:
_temp87 =='t'){ goto _LL80;} else{ goto _LL81;} _LL81: goto _LL82; _LL80:({ void(*
_temp89)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp93= loc;
struct _tagged_arr _temp94= _tag_arr("length modifier '%c' is not supported",
sizeof( unsigned char), 38u); struct Cyc_Stdio_Int_pa_struct _temp95; _temp95.tag=
Cyc_Stdio_Int_pa; _temp95.f1=( int)(( unsigned int) _temp87);{ void* _temp92=(
void*)& _temp95; void* _temp90[ 1u]={ _temp92}; struct _tagged_arr _temp91={(
void*) _temp90,( void*) _temp90,( void*)( _temp90 + 1u)}; _temp89( _temp93,
_temp94, _temp91);}}); return 0; _LL82: goto _LL78; _LL78:;}{ struct Cyc_List_List*
_temp96= _temp73; struct Cyc_List_List _temp103; struct Cyc_List_List* _temp104;
int _temp106; _LL98: if( _temp96 == 0){ goto _LL100;} else{ _temp103=* _temp96;
_LL107: _temp106=( int) _temp103.hd; goto _LL105; _LL105: _temp104= _temp103.tl;
if( _temp104 == 0){ goto _LL102;} else{ goto _LL100;}} _LL102: if( _temp106 =='*'){
goto _LL99;} else{ goto _LL100;} _LL100: goto _LL101; _LL99: _temp55=({ struct
Cyc_List_List* _temp108=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp108->hd=( void*) Cyc_Absyn_sint_t; _temp108->tl= _temp55; _temp108;}); goto
_LL97; _LL101: goto _LL97; _LL97:;}{ struct Cyc_List_List* _temp109= _temp71;
struct Cyc_List_List _temp116; struct Cyc_List_List* _temp117; struct Cyc_List_List
_temp119; struct Cyc_List_List* _temp120; int _temp122; int _temp124; _LL111:
if( _temp109 == 0){ goto _LL113;} else{ _temp116=* _temp109; _LL125: _temp124=(
int) _temp116.hd; goto _LL118; _LL118: _temp117= _temp116.tl; if( _temp117 == 0){
goto _LL113;} else{ _temp119=* _temp117; _LL123: _temp122=( int) _temp119.hd;
goto _LL121; _LL121: _temp120= _temp119.tl; if( _temp120 == 0){ goto _LL115;}
else{ goto _LL113;}}} _LL115: if( _temp124 =='.'? _temp122 =='*': 0){ goto
_LL112;} else{ goto _LL113;} _LL113: goto _LL114; _LL112: _temp55=({ struct Cyc_List_List*
_temp126=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp126->hd=( void*) Cyc_Absyn_sint_t; _temp126->tl= _temp55; _temp126;}); goto
_LL110; _LL114: goto _LL110; _LL110:;}{ void* t; switch( _temp67){ case 'd':
_LL127: goto _LL128; case 'i': _LL128:{ struct Cyc_List_List* f= _temp75; for( 0;
f != 0; f=(( struct Cyc_List_List*) _check_null( f))->tl){ if(( int)(( struct
Cyc_List_List*) _check_null( f))->hd =='#'){({ void(* _temp130)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp134= loc; struct _tagged_arr _temp135= _tag_arr("flag '#' is not valid with %%%c",
sizeof( unsigned char), 32u); struct Cyc_Stdio_Int_pa_struct _temp136; _temp136.tag=
Cyc_Stdio_Int_pa; _temp136.f1=( int)(( unsigned int)(( int) _temp67));{ void*
_temp133=( void*)& _temp136; void* _temp131[ 1u]={ _temp133}; struct _tagged_arr
_temp132={( void*) _temp131,( void*) _temp131,( void*)( _temp131 + 1u)};
_temp130( _temp134, _temp135, _temp132);}}); return 0;}}}{ struct Cyc_List_List*
_temp137= _temp69; struct Cyc_List_List _temp150; struct Cyc_List_List* _temp151;
int _temp153; struct Cyc_List_List _temp156; struct Cyc_List_List* _temp157; int
_temp159; struct Cyc_List_List _temp162; struct Cyc_List_List* _temp163; struct
Cyc_List_List _temp165; struct Cyc_List_List* _temp166; int _temp168; int
_temp170; _LL139: if( _temp137 == 0){ goto _LL140;} else{ goto _LL141;} _LL141:
if( _temp137 == 0){ goto _LL143;} else{ _temp150=* _temp137; _LL154: _temp153=(
int) _temp150.hd; goto _LL152; _LL152: _temp151= _temp150.tl; if( _temp151 == 0){
goto _LL149;} else{ goto _LL143;}} _LL149: if( _temp153 =='l'){ goto _LL142;}
else{ goto _LL143;} _LL143: if( _temp137 == 0){ goto _LL145;} else{ _temp156=*
_temp137; _LL160: _temp159=( int) _temp156.hd; goto _LL158; _LL158: _temp157=
_temp156.tl; if( _temp157 == 0){ goto _LL155;} else{ goto _LL145;}} _LL155: if(
_temp159 =='h'){ goto _LL144;} else{ goto _LL145;} _LL145: if( _temp137 == 0){
goto _LL147;} else{ _temp162=* _temp137; _LL171: _temp170=( int) _temp162.hd;
goto _LL164; _LL164: _temp163= _temp162.tl; if( _temp163 == 0){ goto _LL147;}
else{ _temp165=* _temp163; _LL169: _temp168=( int) _temp165.hd; goto _LL167;
_LL167: _temp166= _temp165.tl; if( _temp166 == 0){ goto _LL161;} else{ goto
_LL147;}}} _LL161: if( _temp170 =='h'? _temp168 =='h': 0){ goto _LL146;} else{
goto _LL147;} _LL147: goto _LL148; _LL140: t= Cyc_Absyn_sint_t; goto _LL138;
_LL142: t= Cyc_Absyn_slong_t; goto _LL138; _LL144: t= Cyc_Absyn_sshort_t; goto
_LL138; _LL146: t= Cyc_Absyn_schar_t; goto _LL138; _LL148:({ void(* _temp172)(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp177= loc; struct _tagged_arr
_temp178= _tag_arr("length modifier '%s' is not allowed with %%%c", sizeof(
unsigned char), 46u); struct Cyc_Stdio_String_pa_struct _temp180; _temp180.tag=
Cyc_Stdio_String_pa; _temp180.f1=( struct _tagged_arr) Cyc_String_implode(
_temp69);{ void* _temp175=( void*)& _temp180; struct Cyc_Stdio_Int_pa_struct
_temp179; _temp179.tag= Cyc_Stdio_Int_pa; _temp179.f1=( int)(( unsigned int)((
int) _temp67));{ void* _temp176=( void*)& _temp179; void* _temp173[ 2u]={
_temp175, _temp176}; struct _tagged_arr _temp174={( void*) _temp173,( void*)
_temp173,( void*)( _temp173 + 2u)}; _temp172( _temp177, _temp178, _temp174);}}});
return 0; _LL138:;} _temp55=({ struct Cyc_List_List* _temp181=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp181->hd=( void*) t; _temp181->tl=
_temp55; _temp181;}); break; case 'u': _LL129:{ struct Cyc_List_List* f= _temp75;
for( 0; f != 0; f=(( struct Cyc_List_List*) _check_null( f))->tl){ if(( int)((
struct Cyc_List_List*) _check_null( f))->hd =='#'){({ void(* _temp183)( struct
Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr;
struct Cyc_Position_Segment* _temp186= loc; struct _tagged_arr _temp187=
_tag_arr("Flag '#' not valid with %%u", sizeof( unsigned char), 28u); void*
_temp184[ 0u]={}; struct _tagged_arr _temp185={( void*) _temp184,( void*)
_temp184,( void*)( _temp184 + 0u)}; _temp183( _temp186, _temp187, _temp185);});
return 0;}}} goto _LL182; case 'o': _LL182: goto _LL188; case 'x': _LL188: goto
_LL189; case 'X': _LL189:{ struct Cyc_List_List* _temp191= _temp69; struct Cyc_List_List
_temp204; struct Cyc_List_List* _temp205; int _temp207; struct Cyc_List_List
_temp210; struct Cyc_List_List* _temp211; int _temp213; struct Cyc_List_List
_temp216; struct Cyc_List_List* _temp217; struct Cyc_List_List _temp219; struct
Cyc_List_List* _temp220; int _temp222; int _temp224; _LL193: if( _temp191 == 0){
goto _LL194;} else{ goto _LL195;} _LL195: if( _temp191 == 0){ goto _LL197;}
else{ _temp204=* _temp191; _LL208: _temp207=( int) _temp204.hd; goto _LL206;
_LL206: _temp205= _temp204.tl; if( _temp205 == 0){ goto _LL203;} else{ goto
_LL197;}} _LL203: if( _temp207 =='l'){ goto _LL196;} else{ goto _LL197;} _LL197:
if( _temp191 == 0){ goto _LL199;} else{ _temp210=* _temp191; _LL214: _temp213=(
int) _temp210.hd; goto _LL212; _LL212: _temp211= _temp210.tl; if( _temp211 == 0){
goto _LL209;} else{ goto _LL199;}} _LL209: if( _temp213 =='h'){ goto _LL198;}
else{ goto _LL199;} _LL199: if( _temp191 == 0){ goto _LL201;} else{ _temp216=*
_temp191; _LL225: _temp224=( int) _temp216.hd; goto _LL218; _LL218: _temp217=
_temp216.tl; if( _temp217 == 0){ goto _LL201;} else{ _temp219=* _temp217; _LL223:
_temp222=( int) _temp219.hd; goto _LL221; _LL221: _temp220= _temp219.tl; if(
_temp220 == 0){ goto _LL215;} else{ goto _LL201;}}} _LL215: if( _temp224 =='h'?
_temp222 =='h': 0){ goto _LL200;} else{ goto _LL201;} _LL201: goto _LL202;
_LL194: t= Cyc_Absyn_uint_t; goto _LL192; _LL196: t= Cyc_Absyn_ulong_t; goto
_LL192; _LL198: t= Cyc_Absyn_ushort_t; goto _LL192; _LL200: t= Cyc_Absyn_uchar_t;
goto _LL192; _LL202:({ void(* _temp226)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp231= loc; struct _tagged_arr _temp232= _tag_arr("length modifier '%s' is not allowed with %%%c",
sizeof( unsigned char), 46u); struct Cyc_Stdio_String_pa_struct _temp234;
_temp234.tag= Cyc_Stdio_String_pa; _temp234.f1=( struct _tagged_arr) Cyc_String_implode(
_temp69);{ void* _temp229=( void*)& _temp234; struct Cyc_Stdio_Int_pa_struct
_temp233; _temp233.tag= Cyc_Stdio_Int_pa; _temp233.f1=( int)(( unsigned int)((
int) _temp67));{ void* _temp230=( void*)& _temp233; void* _temp227[ 2u]={
_temp229, _temp230}; struct _tagged_arr _temp228={( void*) _temp227,( void*)
_temp227,( void*)( _temp227 + 2u)}; _temp226( _temp231, _temp232, _temp228);}}});
return 0; _LL192:;} _temp55=({ struct Cyc_List_List* _temp235=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp235->hd=( void*) t; _temp235->tl=
_temp55; _temp235;}); break; case 'f': _LL190: goto _LL236; case 'F': _LL236:
goto _LL237; case 'e': _LL237: goto _LL238; case 'E': _LL238: goto _LL239; case
'g': _LL239: goto _LL240; case 'G': _LL240: goto _LL241; case 'a': _LL241: goto
_LL242; case 'A': _LL242: if( _temp69 != 0){({ void(* _temp244)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp248= loc; struct _tagged_arr _temp249= _tag_arr("length modifiers are not allowed with %%%c",
sizeof( unsigned char), 43u); struct Cyc_Stdio_Int_pa_struct _temp250; _temp250.tag=
Cyc_Stdio_Int_pa; _temp250.f1=( int)(( unsigned int)(( int) _temp67));{ void*
_temp247=( void*)& _temp250; void* _temp245[ 1u]={ _temp247}; struct _tagged_arr
_temp246={( void*) _temp245,( void*) _temp245,( void*)( _temp245 + 1u)};
_temp244( _temp248, _temp249, _temp246);}}); return 0;} _temp55=({ struct Cyc_List_List*
_temp251=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp251->hd=( void*) Cyc_Absyn_double_t; _temp251->tl= _temp55; _temp251;});
break; case 'c': _LL243:{ struct Cyc_List_List* f= _temp75; for( 0; f != 0; f=((
struct Cyc_List_List*) _check_null( f))->tl){ if(( int)(( struct Cyc_List_List*)
_check_null( f))->hd =='#'? 1:( int)(( struct Cyc_List_List*) _check_null( f))->hd
=='0'){({ void(* _temp253)( struct Cyc_Position_Segment*, struct _tagged_arr fmt,
struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp257=
loc; struct _tagged_arr _temp258= _tag_arr("flag '%c' not allowed with %%c",
sizeof( unsigned char), 31u); struct Cyc_Stdio_Int_pa_struct _temp259; _temp259.tag=
Cyc_Stdio_Int_pa; _temp259.f1=( int)(( unsigned int)(( int)(( struct Cyc_List_List*)
_check_null( f))->hd));{ void* _temp256=( void*)& _temp259; void* _temp254[ 1u]={
_temp256}; struct _tagged_arr _temp255={( void*) _temp254,( void*) _temp254,(
void*)( _temp254 + 1u)}; _temp253( _temp257, _temp258, _temp255);}}); return 0;}}}
if( _temp69 != 0){({ void(* _temp260)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp264= loc; struct _tagged_arr _temp265= _tag_arr("length modifier '%s' not allowed with %%c",
sizeof( unsigned char), 42u); struct Cyc_Stdio_String_pa_struct _temp266;
_temp266.tag= Cyc_Stdio_String_pa; _temp266.f1=( struct _tagged_arr) Cyc_String_implode(
_temp69);{ void* _temp263=( void*)& _temp266; void* _temp261[ 1u]={ _temp263};
struct _tagged_arr _temp262={( void*) _temp261,( void*) _temp261,( void*)(
_temp261 + 1u)}; _temp260( _temp264, _temp265, _temp262);}}); return 0;} if(
_temp71 != 0){({ void(* _temp267)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp271= loc; struct _tagged_arr _temp272= _tag_arr("precision '%s' not allowed with %%c",
sizeof( unsigned char), 36u); struct Cyc_Stdio_String_pa_struct _temp273;
_temp273.tag= Cyc_Stdio_String_pa; _temp273.f1=( struct _tagged_arr) Cyc_String_implode(
_temp71);{ void* _temp270=( void*)& _temp273; void* _temp268[ 1u]={ _temp270};
struct _tagged_arr _temp269={( void*) _temp268,( void*) _temp268,( void*)(
_temp268 + 1u)}; _temp267( _temp271, _temp272, _temp269);}}); return 0;} _temp55=({
struct Cyc_List_List* _temp274=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp274->hd=( void*) Cyc_Absyn_sint_t; _temp274->tl=
_temp55; _temp274;}); break; case 's': _LL252: if( _temp75 != 0){({ void(*
_temp276)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp280= loc;
struct _tagged_arr _temp281= _tag_arr("flags '%s' not allowed with %%s", sizeof(
unsigned char), 32u); struct Cyc_Stdio_String_pa_struct _temp282; _temp282.tag=
Cyc_Stdio_String_pa; _temp282.f1=( struct _tagged_arr) Cyc_String_implode(
_temp75);{ void* _temp279=( void*)& _temp282; void* _temp277[ 1u]={ _temp279};
struct _tagged_arr _temp278={( void*) _temp277,( void*) _temp277,( void*)(
_temp277 + 1u)}; _temp276( _temp280, _temp281, _temp278);}}); return 0;} if(
_temp73 != 0){({ void(* _temp283)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp287= loc; struct _tagged_arr _temp288= _tag_arr("width '%s' not allowed with %%s",
sizeof( unsigned char), 32u); struct Cyc_Stdio_String_pa_struct _temp289;
_temp289.tag= Cyc_Stdio_String_pa; _temp289.f1=( struct _tagged_arr) Cyc_String_implode(
_temp73);{ void* _temp286=( void*)& _temp289; void* _temp284[ 1u]={ _temp286};
struct _tagged_arr _temp285={( void*) _temp284,( void*) _temp284,( void*)(
_temp284 + 1u)}; _temp283( _temp287, _temp288, _temp285);}}); return 0;} if(
_temp71 != 0){({ void(* _temp290)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp294= loc; struct _tagged_arr _temp295= _tag_arr("precision '%s' not allowed with %%s",
sizeof( unsigned char), 36u); struct Cyc_Stdio_String_pa_struct _temp296;
_temp296.tag= Cyc_Stdio_String_pa; _temp296.f1=( struct _tagged_arr) Cyc_String_implode(
_temp71);{ void* _temp293=( void*)& _temp296; void* _temp291[ 1u]={ _temp293};
struct _tagged_arr _temp292={( void*) _temp291,( void*) _temp291,( void*)(
_temp291 + 1u)}; _temp290( _temp294, _temp295, _temp292);}}); return 0;} if(
_temp69 != 0){({ void(* _temp297)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp301= loc; struct _tagged_arr _temp302= _tag_arr("length modifier '%s' not allowed with %%s",
sizeof( unsigned char), 42u); struct Cyc_Stdio_String_pa_struct _temp303;
_temp303.tag= Cyc_Stdio_String_pa; _temp303.f1=( struct _tagged_arr) Cyc_String_implode(
_temp69);{ void* _temp300=( void*)& _temp303; void* _temp298[ 1u]={ _temp300};
struct _tagged_arr _temp299={( void*) _temp298,( void*) _temp298,( void*)(
_temp298 + 1u)}; _temp297( _temp301, _temp302, _temp299);}}); return 0;} _temp55=({
struct Cyc_List_List* _temp304=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp304->hd=( void*) Cyc_Absyn_const_string_typ( Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,({ struct Cyc_Core_Opt* _temp305=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp305->v=(
void*) Cyc_Tcenv_lookup_type_vars( te); _temp305;}))); _temp304->tl= _temp55;
_temp304;}); break; case 'p': _LL275:({ void(* _temp307)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp310= loc; struct _tagged_arr _temp311= _tag_arr("%%p not supported",
sizeof( unsigned char), 18u); void* _temp308[ 0u]={}; struct _tagged_arr
_temp309={( void*) _temp308,( void*) _temp308,( void*)( _temp308 + 0u)};
_temp307( _temp310, _temp311, _temp309);}); return 0; case 'n': _LL306:{ struct
Cyc_List_List* f= _temp75; for( 0; f != 0; f=(( struct Cyc_List_List*)
_check_null( f))->tl){ if(( int)(( struct Cyc_List_List*) _check_null( f))->hd
=='#'? 1:( int)(( struct Cyc_List_List*) _check_null( f))->hd =='0'){({ void(*
_temp313)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp317= loc;
struct _tagged_arr _temp318= _tag_arr("flag '%c' not allowed with %%n", sizeof(
unsigned char), 31u); struct Cyc_Stdio_Int_pa_struct _temp319; _temp319.tag= Cyc_Stdio_Int_pa;
_temp319.f1=( int)(( unsigned int)(( int)(( struct Cyc_List_List*) _check_null(
f))->hd));{ void* _temp316=( void*)& _temp319; void* _temp314[ 1u]={ _temp316};
struct _tagged_arr _temp315={( void*) _temp314,( void*) _temp314,( void*)(
_temp314 + 1u)}; _temp313( _temp317, _temp318, _temp315);}}); return 0;}}} if(
_temp71 != 0){({ void(* _temp320)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp324= loc; struct _tagged_arr _temp325= _tag_arr("precision '%s' not allowed with %%n",
sizeof( unsigned char), 36u); struct Cyc_Stdio_String_pa_struct _temp326;
_temp326.tag= Cyc_Stdio_String_pa; _temp326.f1=( struct _tagged_arr) Cyc_String_implode(
_temp71);{ void* _temp323=( void*)& _temp326; void* _temp321[ 1u]={ _temp323};
struct _tagged_arr _temp322={( void*) _temp321,( void*) _temp321,( void*)(
_temp321 + 1u)}; _temp320( _temp324, _temp325, _temp322);}}); return 0;}{ struct
Cyc_Core_Opt* _temp327=({ struct Cyc_Core_Opt* _temp371=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp371->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp371;});{ struct Cyc_List_List* _temp328= _temp69; struct Cyc_List_List
_temp341; struct Cyc_List_List* _temp342; int _temp344; struct Cyc_List_List
_temp347; struct Cyc_List_List* _temp348; int _temp350; struct Cyc_List_List
_temp353; struct Cyc_List_List* _temp354; struct Cyc_List_List _temp356; struct
Cyc_List_List* _temp357; int _temp359; int _temp361; _LL330: if( _temp328 == 0){
goto _LL331;} else{ goto _LL332;} _LL332: if( _temp328 == 0){ goto _LL334;}
else{ _temp341=* _temp328; _LL345: _temp344=( int) _temp341.hd; goto _LL343;
_LL343: _temp342= _temp341.tl; if( _temp342 == 0){ goto _LL340;} else{ goto
_LL334;}} _LL340: if( _temp344 =='l'){ goto _LL333;} else{ goto _LL334;} _LL334:
if( _temp328 == 0){ goto _LL336;} else{ _temp347=* _temp328; _LL351: _temp350=(
int) _temp347.hd; goto _LL349; _LL349: _temp348= _temp347.tl; if( _temp348 == 0){
goto _LL346;} else{ goto _LL336;}} _LL346: if( _temp350 =='h'){ goto _LL335;}
else{ goto _LL336;} _LL336: if( _temp328 == 0){ goto _LL338;} else{ _temp353=*
_temp328; _LL362: _temp361=( int) _temp353.hd; goto _LL355; _LL355: _temp354=
_temp353.tl; if( _temp354 == 0){ goto _LL338;} else{ _temp356=* _temp354; _LL360:
_temp359=( int) _temp356.hd; goto _LL358; _LL358: _temp357= _temp356.tl; if(
_temp357 == 0){ goto _LL352;} else{ goto _LL338;}}} _LL352: if( _temp361 =='h'?
_temp359 =='h': 0){ goto _LL337;} else{ goto _LL338;} _LL338: goto _LL339;
_LL331: t= Cyc_Absyn_at_typ( Cyc_Absyn_sint_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp327), Cyc_Absyn_empty_tqual()); goto _LL329; _LL333: t=
Cyc_Absyn_at_typ( Cyc_Absyn_ulong_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp327), Cyc_Absyn_empty_tqual()); goto _LL329; _LL335: t=
Cyc_Absyn_at_typ( Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp327), Cyc_Absyn_empty_tqual()); goto _LL329; _LL337: t=
Cyc_Absyn_at_typ( Cyc_Absyn_schar_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp327), Cyc_Absyn_empty_tqual()); goto _LL329; _LL339:({
void(* _temp363)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp367= loc;
struct _tagged_arr _temp368= _tag_arr("length modifier '%s' not allowed with %%n",
sizeof( unsigned char), 42u); struct Cyc_Stdio_String_pa_struct _temp369;
_temp369.tag= Cyc_Stdio_String_pa; _temp369.f1=( struct _tagged_arr) Cyc_String_implode(
_temp69);{ void* _temp366=( void*)& _temp369; void* _temp364[ 1u]={ _temp366};
struct _tagged_arr _temp365={( void*) _temp364,( void*) _temp364,( void*)(
_temp364 + 1u)}; _temp363( _temp367, _temp368, _temp365);}}); return 0; _LL329:;}
_temp55=({ struct Cyc_List_List* _temp370=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp370->hd=( void*) t; _temp370->tl= _temp55;
_temp370;}); break;} case '%': _LL312: if( _temp75 != 0){({ void(* _temp373)(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp377= loc; struct _tagged_arr
_temp378= _tag_arr("flags '%s' not allowed with %%%%", sizeof( unsigned char),
33u); struct Cyc_Stdio_String_pa_struct _temp379; _temp379.tag= Cyc_Stdio_String_pa;
_temp379.f1=( struct _tagged_arr) Cyc_String_implode( _temp75);{ void* _temp376=(
void*)& _temp379; void* _temp374[ 1u]={ _temp376}; struct _tagged_arr _temp375={(
void*) _temp374,( void*) _temp374,( void*)( _temp374 + 1u)}; _temp373( _temp377,
_temp378, _temp375);}}); return 0;} if( _temp73 != 0){({ void(* _temp380)(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp384= loc; struct _tagged_arr
_temp385= _tag_arr("width '%s' not allowed with %%%%", sizeof( unsigned char),
33u); struct Cyc_Stdio_String_pa_struct _temp386; _temp386.tag= Cyc_Stdio_String_pa;
_temp386.f1=( struct _tagged_arr) Cyc_String_implode( _temp73);{ void* _temp383=(
void*)& _temp386; void* _temp381[ 1u]={ _temp383}; struct _tagged_arr _temp382={(
void*) _temp381,( void*) _temp381,( void*)( _temp381 + 1u)}; _temp380( _temp384,
_temp385, _temp382);}}); return 0;} if( _temp71 != 0){({ void(* _temp387)(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp391= loc; struct _tagged_arr
_temp392= _tag_arr("precision '%s' not allowed with %%%%", sizeof( unsigned char),
37u); struct Cyc_Stdio_String_pa_struct _temp393; _temp393.tag= Cyc_Stdio_String_pa;
_temp393.f1=( struct _tagged_arr) Cyc_String_implode( _temp71);{ void* _temp390=(
void*)& _temp393; void* _temp388[ 1u]={ _temp390}; struct _tagged_arr _temp389={(
void*) _temp388,( void*) _temp388,( void*)( _temp388 + 1u)}; _temp387( _temp391,
_temp392, _temp389);}}); return 0;} if( _temp69 != 0){({ void(* _temp394)(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp398= loc; struct _tagged_arr
_temp399= _tag_arr("length modifier '%s' not allowed with %%%%", sizeof(
unsigned char), 43u); struct Cyc_Stdio_String_pa_struct _temp400; _temp400.tag=
Cyc_Stdio_String_pa; _temp400.f1=( struct _tagged_arr) Cyc_String_implode(
_temp69);{ void* _temp397=( void*)& _temp400; void* _temp395[ 1u]={ _temp397};
struct _tagged_arr _temp396={( void*) _temp395,( void*) _temp395,( void*)(
_temp395 + 1u)}; _temp394( _temp398, _temp399, _temp396);}}); return 0;} break;
default: _LL372: return 0;}}}}} return Cyc_List_imp_rev( _temp55);} struct
_tuple4{ int f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
unsigned char f4; int f5; } ; struct Cyc_Core_Opt* Cyc_Formatstr_parse_inputformat(
struct _tagged_arr s, int i){ int _temp402= Cyc_String_strlen( s); if( i < 0? 1:
i >= _temp402){ return 0;}{ int _temp403= 0; unsigned char _temp404=*(( const
unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i)); if(
_temp404 =='*'){ _temp403= 1; i ++; if( i >= _temp402){ return 0;}}{ struct Cyc_List_List*
_temp405= 0; for( 0; i < _temp402; i ++){ _temp404=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( isdigit(( int)
_temp404)){ _temp405=({ struct Cyc_List_List* _temp406=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp406->hd=( void*)(( int) _temp404);
_temp406->tl= _temp405; _temp406;});} else{ break;}} if( i >= _temp402){ return
0;} _temp405=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp405);{ struct Cyc_List_List* _temp407= 0; _temp404=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); switch( _temp404){
case 'h': _LL408: _temp407=({ struct Cyc_List_List* _temp410=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp410->hd=( void*)(( int) _temp404);
_temp410->tl= _temp407; _temp410;}); i ++; if( i >= _temp402){ return 0;}
_temp404=*(( const unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), i)); if( _temp404 =='h'){ _temp407=({ struct Cyc_List_List*
_temp411=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp411->hd=( void*)(( int) _temp404); _temp411->tl= _temp407; _temp411;}); i
++;} break; case 'l': _LL409: _temp407=({ struct Cyc_List_List* _temp413=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp413->hd=(
void*)(( int) _temp404); _temp413->tl= _temp407; _temp413;}); i ++; if( i >=
_temp402){ return 0;} _temp404=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( _temp404 =='l'){
_temp407=({ struct Cyc_List_List* _temp414=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp414->hd=( void*)(( int) _temp404); _temp414->tl=
_temp407; _temp414;}); i ++;} break; case 'j': _LL412: goto _LL415; case 'z':
_LL415: goto _LL416; case 't': _LL416: goto _LL417; case 'L': _LL417: _temp407=({
struct Cyc_List_List* _temp419=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp419->hd=( void*)(( int) _temp404); _temp419->tl=
_temp407; _temp419;}); i ++; break; default: _LL418: break;} if( i >= _temp402){
return 0;} _temp407=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp407); _temp404=*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), i)); switch( _temp404){ case 'd': _LL421: break; case 'i':
_LL422: break; case 'o': _LL423: break; case 'u': _LL424: break; case 'x':
_LL425: break; case 'X': _LL426: break; case 'f': _LL427: break; case 'F':
_LL428: break; case 'e': _LL429: break; case 'E': _LL430: break; case 'g':
_LL431: break; case 'G': _LL432: break; case 'a': _LL433: break; case 'A':
_LL434: break; case 'c': _LL435: break; case 's': _LL436: break; case 'p':
_LL437: break; case 'n': _LL438: break; case '%': _LL439: break; default: _LL440:
return 0;} return({ struct Cyc_Core_Opt* _temp442=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp442->v=( void*)({ struct _tuple4*
_temp443=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp443->f1=
_temp403; _temp443->f2= _temp405; _temp443->f3= _temp407; _temp443->f4= _temp404;
_temp443->f5= i + 1; _temp443;}); _temp442;});}}}} struct Cyc_List_List* Cyc_Formatstr_get_scanf_typs(
struct Cyc_Tcenv_Tenv* te, struct _tagged_arr s, struct Cyc_Position_Segment*
loc){ int _temp444= Cyc_String_strlen( s); struct Cyc_List_List* _temp445= 0;
int i; for( i= 0; i < _temp444; i ++){ if(*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)) !='%'){ continue;}{
struct Cyc_Core_Opt* _temp446= Cyc_Formatstr_parse_inputformat( s, i + 1); if(
_temp446 == 0){({ void(* _temp447)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp450= loc; struct _tagged_arr _temp451= _tag_arr("bad format string",
sizeof( unsigned char), 18u); void* _temp448[ 0u]={}; struct _tagged_arr
_temp449={( void*) _temp448,( void*) _temp448,( void*)( _temp448 + 0u)};
_temp447( _temp450, _temp451, _temp449);}); return 0;}{ struct _tuple4 _temp454;
int _temp455; unsigned char _temp457; struct Cyc_List_List* _temp459; struct Cyc_List_List*
_temp461; int _temp463; struct _tuple4* _temp452=( struct _tuple4*)(( struct Cyc_Core_Opt*)
_check_null( _temp446))->v; _temp454=* _temp452; _LL464: _temp463= _temp454.f1;
goto _LL462; _LL462: _temp461= _temp454.f2; goto _LL460; _LL460: _temp459=
_temp454.f3; goto _LL458; _LL458: _temp457= _temp454.f4; goto _LL456; _LL456:
_temp455= _temp454.f5; goto _LL453; _LL453: i= _temp455 - 1;{ struct Cyc_List_List*
_temp465= _temp459; struct Cyc_List_List _temp472; struct Cyc_List_List*
_temp473; int _temp475; _LL467: if( _temp465 == 0){ goto _LL469;} else{ _temp472=*
_temp465; _LL476: _temp475=( int) _temp472.hd; goto _LL474; _LL474: _temp473=
_temp472.tl; if( _temp473 == 0){ goto _LL471;} else{ goto _LL469;}} _LL471: if((
_temp475 =='j'? 1: _temp475 =='z')? 1: _temp475 =='t'){ goto _LL468;} else{ goto
_LL469;} _LL469: goto _LL470; _LL468:({ void(* _temp477)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp481= loc; struct _tagged_arr _temp482= _tag_arr("length modifier '%c' is not supported",
sizeof( unsigned char), 38u); struct Cyc_Stdio_Int_pa_struct _temp483; _temp483.tag=
Cyc_Stdio_Int_pa; _temp483.f1=( int)(( unsigned int) _temp475);{ void* _temp480=(
void*)& _temp483; void* _temp478[ 1u]={ _temp480}; struct _tagged_arr _temp479={(
void*) _temp478,( void*) _temp478,( void*)( _temp478 + 1u)}; _temp477( _temp481,
_temp482, _temp479);}}); return 0; _LL470: goto _LL466; _LL466:;} if( _temp463){
continue;}{ void* t; switch( _temp457){ case 'd': _LL484: goto _LL485; case 'i':
_LL485: { struct Cyc_Core_Opt* _temp487=({ struct Cyc_Core_Opt* _temp533=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp533->v=(
void*) Cyc_Tcenv_lookup_type_vars( te); _temp533;});{ struct Cyc_List_List*
_temp488= _temp459; struct Cyc_List_List _temp501; struct Cyc_List_List*
_temp502; int _temp504; struct Cyc_List_List _temp507; struct Cyc_List_List*
_temp508; int _temp510; struct Cyc_List_List _temp513; struct Cyc_List_List*
_temp514; struct Cyc_List_List _temp516; struct Cyc_List_List* _temp517; int
_temp519; int _temp521; _LL490: if( _temp488 == 0){ goto _LL491;} else{ goto
_LL492;} _LL492: if( _temp488 == 0){ goto _LL494;} else{ _temp501=* _temp488;
_LL505: _temp504=( int) _temp501.hd; goto _LL503; _LL503: _temp502= _temp501.tl;
if( _temp502 == 0){ goto _LL500;} else{ goto _LL494;}} _LL500: if( _temp504 =='l'){
goto _LL493;} else{ goto _LL494;} _LL494: if( _temp488 == 0){ goto _LL496;}
else{ _temp507=* _temp488; _LL511: _temp510=( int) _temp507.hd; goto _LL509;
_LL509: _temp508= _temp507.tl; if( _temp508 == 0){ goto _LL506;} else{ goto
_LL496;}} _LL506: if( _temp510 =='h'){ goto _LL495;} else{ goto _LL496;} _LL496:
if( _temp488 == 0){ goto _LL498;} else{ _temp513=* _temp488; _LL522: _temp521=(
int) _temp513.hd; goto _LL515; _LL515: _temp514= _temp513.tl; if( _temp514 == 0){
goto _LL498;} else{ _temp516=* _temp514; _LL520: _temp519=( int) _temp516.hd;
goto _LL518; _LL518: _temp517= _temp516.tl; if( _temp517 == 0){ goto _LL512;}
else{ goto _LL498;}}} _LL512: if( _temp521 =='h'? _temp519 =='h': 0){ goto
_LL497;} else{ goto _LL498;} _LL498: goto _LL499; _LL491: t= Cyc_Absyn_at_typ(
Cyc_Absyn_sint_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp487), Cyc_Absyn_empty_tqual()); goto _LL489; _LL493: t= Cyc_Absyn_at_typ(
Cyc_Absyn_slong_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp487), Cyc_Absyn_empty_tqual()); goto _LL489; _LL495: t= Cyc_Absyn_at_typ(
Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp487), Cyc_Absyn_empty_tqual()); goto _LL489; _LL497: t= Cyc_Absyn_at_typ(
Cyc_Absyn_schar_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp487), Cyc_Absyn_empty_tqual()); goto _LL489; _LL499:({ void(* _temp523)(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp528= loc; struct _tagged_arr
_temp529= _tag_arr("length modifier '%s' is not allowed with %%%c", sizeof(
unsigned char), 46u); struct Cyc_Stdio_String_pa_struct _temp531; _temp531.tag=
Cyc_Stdio_String_pa; _temp531.f1=( struct _tagged_arr) Cyc_String_implode(
_temp459);{ void* _temp526=( void*)& _temp531; struct Cyc_Stdio_Int_pa_struct
_temp530; _temp530.tag= Cyc_Stdio_Int_pa; _temp530.f1=( int)(( unsigned int)((
int) _temp457));{ void* _temp527=( void*)& _temp530; void* _temp524[ 2u]={
_temp526, _temp527}; struct _tagged_arr _temp525={( void*) _temp524,( void*)
_temp524,( void*)( _temp524 + 2u)}; _temp523( _temp528, _temp529, _temp525);}}});
return 0; _LL489:;} _temp445=({ struct Cyc_List_List* _temp532=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp532->hd=( void*) t; _temp532->tl=
_temp445; _temp532;}); break;} case 'u': _LL486: goto _LL534; case 'o': _LL534:
goto _LL535; case 'x': _LL535: goto _LL536; case 'X': _LL536: { struct Cyc_Core_Opt*
_temp538=({ struct Cyc_Core_Opt* _temp584=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp584->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp584;});{ struct Cyc_List_List* _temp539= _temp459; struct Cyc_List_List
_temp552; struct Cyc_List_List* _temp553; int _temp555; struct Cyc_List_List
_temp558; struct Cyc_List_List* _temp559; int _temp561; struct Cyc_List_List
_temp564; struct Cyc_List_List* _temp565; struct Cyc_List_List _temp567; struct
Cyc_List_List* _temp568; int _temp570; int _temp572; _LL541: if( _temp539 == 0){
goto _LL542;} else{ goto _LL543;} _LL543: if( _temp539 == 0){ goto _LL545;}
else{ _temp552=* _temp539; _LL556: _temp555=( int) _temp552.hd; goto _LL554;
_LL554: _temp553= _temp552.tl; if( _temp553 == 0){ goto _LL551;} else{ goto
_LL545;}} _LL551: if( _temp555 =='l'){ goto _LL544;} else{ goto _LL545;} _LL545:
if( _temp539 == 0){ goto _LL547;} else{ _temp558=* _temp539; _LL562: _temp561=(
int) _temp558.hd; goto _LL560; _LL560: _temp559= _temp558.tl; if( _temp559 == 0){
goto _LL557;} else{ goto _LL547;}} _LL557: if( _temp561 =='h'){ goto _LL546;}
else{ goto _LL547;} _LL547: if( _temp539 == 0){ goto _LL549;} else{ _temp564=*
_temp539; _LL573: _temp572=( int) _temp564.hd; goto _LL566; _LL566: _temp565=
_temp564.tl; if( _temp565 == 0){ goto _LL549;} else{ _temp567=* _temp565; _LL571:
_temp570=( int) _temp567.hd; goto _LL569; _LL569: _temp568= _temp567.tl; if(
_temp568 == 0){ goto _LL563;} else{ goto _LL549;}}} _LL563: if( _temp572 =='h'?
_temp570 =='h': 0){ goto _LL548;} else{ goto _LL549;} _LL549: goto _LL550;
_LL542: t= Cyc_Absyn_at_typ( Cyc_Absyn_uint_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp538), Cyc_Absyn_empty_tqual()); goto _LL540; _LL544: t=
Cyc_Absyn_at_typ( Cyc_Absyn_ulong_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp538), Cyc_Absyn_empty_tqual()); goto _LL540; _LL546: t=
Cyc_Absyn_at_typ( Cyc_Absyn_ushort_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp538), Cyc_Absyn_empty_tqual()); goto _LL540; _LL548: t=
Cyc_Absyn_at_typ( Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp538), Cyc_Absyn_empty_tqual()); goto _LL540; _LL550:({
void(* _temp574)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp579= loc;
struct _tagged_arr _temp580= _tag_arr("length modifier '%s' is not allowed with %%%c",
sizeof( unsigned char), 46u); struct Cyc_Stdio_String_pa_struct _temp582;
_temp582.tag= Cyc_Stdio_String_pa; _temp582.f1=( struct _tagged_arr) Cyc_String_implode(
_temp459);{ void* _temp577=( void*)& _temp582; struct Cyc_Stdio_Int_pa_struct
_temp581; _temp581.tag= Cyc_Stdio_Int_pa; _temp581.f1=( int)(( unsigned int)((
int) _temp457));{ void* _temp578=( void*)& _temp581; void* _temp575[ 2u]={
_temp577, _temp578}; struct _tagged_arr _temp576={( void*) _temp575,( void*)
_temp575,( void*)( _temp575 + 2u)}; _temp574( _temp579, _temp580, _temp576);}}});
return 0; _LL540:;} _temp445=({ struct Cyc_List_List* _temp583=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp583->hd=( void*) t; _temp583->tl=
_temp445; _temp583;}); break;} case 'f': _LL537: goto _LL585; case 'F': _LL585:
goto _LL586; case 'e': _LL586: goto _LL587; case 'E': _LL587: goto _LL588; case
'g': _LL588: goto _LL589; case 'G': _LL589: goto _LL590; case 'a': _LL590: goto
_LL591; case 'A': _LL591: { struct Cyc_Core_Opt* _temp593=({ struct Cyc_Core_Opt*
_temp618=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp618->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp618;});{ struct Cyc_List_List*
_temp594= _temp459; struct Cyc_List_List _temp603; struct Cyc_List_List*
_temp604; int _temp606; _LL596: if( _temp594 == 0){ goto _LL597;} else{ goto
_LL598;} _LL598: if( _temp594 == 0){ goto _LL600;} else{ _temp603=* _temp594;
_LL607: _temp606=( int) _temp603.hd; goto _LL605; _LL605: _temp604= _temp603.tl;
if( _temp604 == 0){ goto _LL602;} else{ goto _LL600;}} _LL602: if( _temp606 =='l'){
goto _LL599;} else{ goto _LL600;} _LL600: goto _LL601; _LL597: t= Cyc_Absyn_at_typ(
Cyc_Absyn_float_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp593), Cyc_Absyn_empty_tqual()); goto _LL595; _LL599: t= Cyc_Absyn_at_typ(
Cyc_Absyn_double_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp593), Cyc_Absyn_empty_tqual()); goto _LL595; _LL601:({ void(* _temp608)(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp613= loc; struct _tagged_arr
_temp614= _tag_arr("length modifier '%s' is not allowed with %%%c", sizeof(
unsigned char), 46u); struct Cyc_Stdio_String_pa_struct _temp616; _temp616.tag=
Cyc_Stdio_String_pa; _temp616.f1=( struct _tagged_arr) Cyc_String_implode(
_temp459);{ void* _temp611=( void*)& _temp616; struct Cyc_Stdio_Int_pa_struct
_temp615; _temp615.tag= Cyc_Stdio_Int_pa; _temp615.f1=( int)(( unsigned int)((
int) _temp457));{ void* _temp612=( void*)& _temp615; void* _temp609[ 2u]={
_temp611, _temp612}; struct _tagged_arr _temp610={( void*) _temp609,( void*)
_temp609,( void*)( _temp609 + 2u)}; _temp608( _temp613, _temp614, _temp610);}}});
return 0; _LL595:;} _temp445=({ struct Cyc_List_List* _temp617=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp617->hd=( void*) t; _temp617->tl=
_temp445; _temp617;}); break;} case 'c': _LL592:({ void(* _temp620)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp623= loc; struct _tagged_arr _temp624= _tag_arr("%%c is not supported",
sizeof( unsigned char), 21u); void* _temp621[ 0u]={}; struct _tagged_arr
_temp622={( void*) _temp621,( void*) _temp621,( void*)( _temp621 + 0u)};
_temp620( _temp623, _temp624, _temp622);}); return 0; case 's': _LL619:({ void(*
_temp626)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp629= loc;
struct _tagged_arr _temp630= _tag_arr("%%s is not supported", sizeof(
unsigned char), 21u); void* _temp627[ 0u]={}; struct _tagged_arr _temp628={(
void*) _temp627,( void*) _temp627,( void*)( _temp627 + 0u)}; _temp626( _temp629,
_temp630, _temp628);}); return 0; case '[': _LL625:({ void(* _temp632)( struct
Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr;
struct Cyc_Position_Segment* _temp635= loc; struct _tagged_arr _temp636=
_tag_arr("%%[ is not supported", sizeof( unsigned char), 21u); void* _temp633[ 0u]={};
struct _tagged_arr _temp634={( void*) _temp633,( void*) _temp633,( void*)(
_temp633 + 0u)}; _temp632( _temp635, _temp636, _temp634);}); return 0; case 'p':
_LL631:({ void(* _temp638)( struct Cyc_Position_Segment*, struct _tagged_arr fmt,
struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp641=
loc; struct _tagged_arr _temp642= _tag_arr("%%p is not supported", sizeof(
unsigned char), 21u); void* _temp639[ 0u]={}; struct _tagged_arr _temp640={(
void*) _temp639,( void*) _temp639,( void*)( _temp639 + 0u)}; _temp638( _temp641,
_temp642, _temp640);}); return 0; case 'n': _LL637: { struct Cyc_Core_Opt*
_temp644=({ struct Cyc_Core_Opt* _temp688=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp688->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp688;});{ struct Cyc_List_List* _temp645= _temp459; struct Cyc_List_List
_temp658; struct Cyc_List_List* _temp659; int _temp661; struct Cyc_List_List
_temp664; struct Cyc_List_List* _temp665; int _temp667; struct Cyc_List_List
_temp670; struct Cyc_List_List* _temp671; struct Cyc_List_List _temp673; struct
Cyc_List_List* _temp674; int _temp676; int _temp678; _LL647: if( _temp645 == 0){
goto _LL648;} else{ goto _LL649;} _LL649: if( _temp645 == 0){ goto _LL651;}
else{ _temp658=* _temp645; _LL662: _temp661=( int) _temp658.hd; goto _LL660;
_LL660: _temp659= _temp658.tl; if( _temp659 == 0){ goto _LL657;} else{ goto
_LL651;}} _LL657: if( _temp661 =='l'){ goto _LL650;} else{ goto _LL651;} _LL651:
if( _temp645 == 0){ goto _LL653;} else{ _temp664=* _temp645; _LL668: _temp667=(
int) _temp664.hd; goto _LL666; _LL666: _temp665= _temp664.tl; if( _temp665 == 0){
goto _LL663;} else{ goto _LL653;}} _LL663: if( _temp667 =='h'){ goto _LL652;}
else{ goto _LL653;} _LL653: if( _temp645 == 0){ goto _LL655;} else{ _temp670=*
_temp645; _LL679: _temp678=( int) _temp670.hd; goto _LL672; _LL672: _temp671=
_temp670.tl; if( _temp671 == 0){ goto _LL655;} else{ _temp673=* _temp671; _LL677:
_temp676=( int) _temp673.hd; goto _LL675; _LL675: _temp674= _temp673.tl; if(
_temp674 == 0){ goto _LL669;} else{ goto _LL655;}}} _LL669: if( _temp678 =='h'?
_temp676 =='h': 0){ goto _LL654;} else{ goto _LL655;} _LL655: goto _LL656;
_LL648: t= Cyc_Absyn_at_typ( Cyc_Absyn_sint_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp644), Cyc_Absyn_empty_tqual()); goto _LL646; _LL650: t=
Cyc_Absyn_at_typ( Cyc_Absyn_ulong_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp644), Cyc_Absyn_empty_tqual()); goto _LL646; _LL652: t=
Cyc_Absyn_at_typ( Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp644), Cyc_Absyn_empty_tqual()); goto _LL646; _LL654: t=
Cyc_Absyn_at_typ( Cyc_Absyn_schar_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp644), Cyc_Absyn_empty_tqual()); goto _LL646; _LL656:({
void(* _temp680)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp684= loc;
struct _tagged_arr _temp685= _tag_arr("length modifier '%s' not allowed with %%n",
sizeof( unsigned char), 42u); struct Cyc_Stdio_String_pa_struct _temp686;
_temp686.tag= Cyc_Stdio_String_pa; _temp686.f1=( struct _tagged_arr) Cyc_String_implode(
_temp459);{ void* _temp683=( void*)& _temp686; void* _temp681[ 1u]={ _temp683};
struct _tagged_arr _temp682={( void*) _temp681,( void*) _temp681,( void*)(
_temp681 + 1u)}; _temp680( _temp684, _temp685, _temp682);}}); return 0; _LL646:;}
_temp445=({ struct Cyc_List_List* _temp687=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp687->hd=( void*) t; _temp687->tl= _temp445;
_temp687;}); break;} case '%': _LL643: if( _temp463){({ void(* _temp690)( struct
Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr;
struct Cyc_Position_Segment* _temp693= loc; struct _tagged_arr _temp694=
_tag_arr("Assignment suppression (*) is not allowed with %%%%", sizeof(
unsigned char), 52u); void* _temp691[ 0u]={}; struct _tagged_arr _temp692={(
void*) _temp691,( void*) _temp691,( void*)( _temp691 + 0u)}; _temp690( _temp693,
_temp694, _temp692);}); return 0;} if( _temp461 != 0){({ void(* _temp695)(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp699= loc; struct _tagged_arr
_temp700= _tag_arr("width '%s' not allowed with %%%%", sizeof( unsigned char),
33u); struct Cyc_Stdio_String_pa_struct _temp701; _temp701.tag= Cyc_Stdio_String_pa;
_temp701.f1=( struct _tagged_arr) Cyc_String_implode( _temp461);{ void* _temp698=(
void*)& _temp701; void* _temp696[ 1u]={ _temp698}; struct _tagged_arr _temp697={(
void*) _temp696,( void*) _temp696,( void*)( _temp696 + 1u)}; _temp695( _temp699,
_temp700, _temp697);}}); return 0;} if( _temp459 != 0){({ void(* _temp702)(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp706= loc; struct _tagged_arr
_temp707= _tag_arr("length modifier '%s' not allowed with %%%%", sizeof(
unsigned char), 43u); struct Cyc_Stdio_String_pa_struct _temp708; _temp708.tag=
Cyc_Stdio_String_pa; _temp708.f1=( struct _tagged_arr) Cyc_String_implode(
_temp459);{ void* _temp705=( void*)& _temp708; void* _temp703[ 1u]={ _temp705};
struct _tagged_arr _temp704={( void*) _temp703,( void*) _temp703,( void*)(
_temp703 + 1u)}; _temp702( _temp706, _temp707, _temp704);}}); return 0;} break;
default: _LL689: return 0;}}}}} return Cyc_List_imp_rev( _temp445);}
