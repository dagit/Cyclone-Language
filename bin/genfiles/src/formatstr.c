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
_tagged_arr s, int i){ int _temp0= Cyc_String_strlen( s); if( i <  0? 1: i >= 
_temp0){ return 0;}{ struct Cyc_List_List* _temp1= 0; unsigned char c=' '; for(
0; i <  _temp0; i ++){ c=*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), i)); switch( c){ case '+': _LL2: goto _LL3; case '-':
_LL3: goto _LL4; case ' ': _LL4: goto _LL5; case '#': _LL5: goto _LL6; case '0':
_LL6: _temp1=({ struct Cyc_List_List* _temp8=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp8->hd=( void*)(( int) c); _temp8->tl=
_temp1; _temp8;}); continue; default: _LL7: break;} break;} if( i >=  _temp0){
return 0;} _temp1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp1);{ struct Cyc_List_List* _temp10= 0; c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( c == '*'){ _temp10=({
struct Cyc_List_List* _temp11=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp11->hd=( void*)(( int) c); _temp11->tl= _temp10; _temp11;});
i ++;} else{ for( 0; i <  _temp0; i ++){ c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( isdigit(( int) c)){
_temp10=({ struct Cyc_List_List* _temp12=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp12->hd=( void*)(( int) c); _temp12->tl=
_temp10; _temp12;});} else{ break;}}} if( i >=  _temp0){ return 0;} _temp10=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp10);{
struct Cyc_List_List* _temp13= 0; c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( c == '.'){ _temp13=({
struct Cyc_List_List* _temp14=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp14->hd=( void*)(( int) c); _temp14->tl= _temp13; _temp14;});
i ++; if( i >=  _temp0){ return 0;} c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( c == '*'){ _temp13=({
struct Cyc_List_List* _temp15=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp15->hd=( void*)(( int) c); _temp15->tl= _temp13; _temp15;});
i ++;} else{ for( 0; i <  _temp0; i ++){ c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( isdigit(( int) c)){
_temp13=({ struct Cyc_List_List* _temp16=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp16->hd=( void*)(( int) c); _temp16->tl=
_temp13; _temp16;});} else{ break;}}}} if( i >=  _temp0){ return 0;} _temp13=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp13);{
struct Cyc_List_List* _temp17= 0; c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); switch( c){ case 'h':
_LL18: _temp17=({ struct Cyc_List_List* _temp20=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp20->hd=( void*)(( int) c);
_temp20->tl= _temp17; _temp20;}); i ++; if( i >=  _temp0){ return 0;} c=*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i));
if( c == 'h'){ _temp17=({ struct Cyc_List_List* _temp21=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp21->hd=( void*)(( int) c);
_temp21->tl= _temp17; _temp21;}); i ++;} break; case 'l': _LL19: _temp17=({
struct Cyc_List_List* _temp23=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp23->hd=( void*)(( int) c); _temp23->tl= _temp17; _temp23;});
i ++; if( i >=  _temp0){ return 0;} c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( c == 'l'){ _temp17=({
struct Cyc_List_List* _temp24=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp24->hd=( void*)(( int) c); _temp24->tl= _temp17; _temp24;});
i ++;} break; case 'j': _LL22: goto _LL25; case 'z': _LL25: goto _LL26; case 't':
_LL26: goto _LL27; case 'L': _LL27: _temp17=({ struct Cyc_List_List* _temp29=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp29->hd=(
void*)(( int) c); _temp29->tl= _temp17; _temp29;}); i ++; break; default: _LL28:
break;} if( i >=  _temp0){ return 0;} _temp17=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp17); c=*(( const unsigned char*)
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
i +  1; _temp53;}); _temp52;});}}}}} struct Cyc_List_List* Cyc_Formatstr_get_format_typs(
struct Cyc_Tcenv_Tenv* te, struct _tagged_arr s, struct Cyc_Position_Segment*
loc){ int _temp54= Cyc_String_strlen( s); struct Cyc_List_List* _temp55= 0; int
i; for( i= 0; i <  _temp54; i ++){ if(*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)) != '%'){ continue;}{
struct Cyc_Core_Opt* _temp56= Cyc_Formatstr_parse_conversionspecification( s, i
+  1); if( _temp56 ==  0){({ void* _temp57[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("bad format string", sizeof( unsigned char), 18u), _tag_arr( _temp57,
sizeof( void*), 0u));}); return 0;}{ struct _tuple3 _temp60; int _temp61;
unsigned char _temp63; struct Cyc_List_List* _temp65; struct Cyc_List_List*
_temp67; struct Cyc_List_List* _temp69; struct Cyc_List_List* _temp71; struct
_tuple3* _temp58=( struct _tuple3*)(( struct Cyc_Core_Opt*) _check_null( _temp56))->v;
_temp60=* _temp58; _LL72: _temp71= _temp60.f1; goto _LL70; _LL70: _temp69=
_temp60.f2; goto _LL68; _LL68: _temp67= _temp60.f3; goto _LL66; _LL66: _temp65=
_temp60.f4; goto _LL64; _LL64: _temp63= _temp60.f5; goto _LL62; _LL62: _temp61=
_temp60.f6; goto _LL59; _LL59: i= _temp61 -  1;{ struct Cyc_List_List* _temp73=
_temp65; struct Cyc_List_List _temp80; struct Cyc_List_List* _temp81; int
_temp83; _LL75: if( _temp73 ==  0){ goto _LL77;} else{ _temp80=* _temp73; _LL84:
_temp83=( int) _temp80.hd; goto _LL82; _LL82: _temp81= _temp80.tl; if( _temp81
==  0){ goto _LL79;} else{ goto _LL77;}} _LL79: if(( _temp83 == 'j'? 1: _temp83
== 'z')? 1: _temp83 == 't'){ goto _LL76;} else{ goto _LL77;} _LL77: goto _LL78;
_LL76:({ struct Cyc_Stdio_Int_pa_struct _temp86; _temp86.tag= Cyc_Stdio_Int_pa;
_temp86.f1=( int)(( unsigned int) _temp83);{ void* _temp85[ 1u]={& _temp86}; Cyc_Tcutil_terr(
loc, _tag_arr("length modifier '%c' is not supported", sizeof( unsigned char),
38u), _tag_arr( _temp85, sizeof( void*), 1u));}}); return 0; _LL78: goto _LL74;
_LL74:;}{ struct Cyc_List_List* _temp87= _temp69; struct Cyc_List_List _temp94;
struct Cyc_List_List* _temp95; int _temp97; _LL89: if( _temp87 ==  0){ goto
_LL91;} else{ _temp94=* _temp87; _LL98: _temp97=( int) _temp94.hd; goto _LL96;
_LL96: _temp95= _temp94.tl; if( _temp95 ==  0){ goto _LL93;} else{ goto _LL91;}}
_LL93: if( _temp97 == '*'){ goto _LL90;} else{ goto _LL91;} _LL91: goto _LL92;
_LL90: _temp55=({ struct Cyc_List_List* _temp99=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp99->hd=( void*) Cyc_Absyn_sint_t;
_temp99->tl= _temp55; _temp99;}); goto _LL88; _LL92: goto _LL88; _LL88:;}{
struct Cyc_List_List* _temp100= _temp67; struct Cyc_List_List _temp107; struct
Cyc_List_List* _temp108; struct Cyc_List_List _temp110; struct Cyc_List_List*
_temp111; int _temp113; int _temp115; _LL102: if( _temp100 ==  0){ goto _LL104;}
else{ _temp107=* _temp100; _LL116: _temp115=( int) _temp107.hd; goto _LL109;
_LL109: _temp108= _temp107.tl; if( _temp108 ==  0){ goto _LL104;} else{ _temp110=*
_temp108; _LL114: _temp113=( int) _temp110.hd; goto _LL112; _LL112: _temp111=
_temp110.tl; if( _temp111 ==  0){ goto _LL106;} else{ goto _LL104;}}} _LL106:
if( _temp115 == '.'? _temp113 == '*': 0){ goto _LL103;} else{ goto _LL104;}
_LL104: goto _LL105; _LL103: _temp55=({ struct Cyc_List_List* _temp117=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp117->hd=( void*)
Cyc_Absyn_sint_t; _temp117->tl= _temp55; _temp117;}); goto _LL101; _LL105: goto
_LL101; _LL101:;}{ void* t; switch( _temp63){ case 'd': _LL118: goto _LL119;
case 'i': _LL119:{ struct Cyc_List_List* f= _temp71; for( 0; f !=  0; f=((
struct Cyc_List_List*) _check_null( f))->tl){ if(( int)(( struct Cyc_List_List*)
_check_null( f))->hd == '#'){({ struct Cyc_Stdio_Int_pa_struct _temp122;
_temp122.tag= Cyc_Stdio_Int_pa; _temp122.f1=( int)(( unsigned int)(( int)
_temp63));{ void* _temp121[ 1u]={& _temp122}; Cyc_Tcutil_terr( loc, _tag_arr("flag '#' is not valid with %%%c",
sizeof( unsigned char), 32u), _tag_arr( _temp121, sizeof( void*), 1u));}});
return 0;}}}{ struct Cyc_List_List* _temp123= _temp65; struct Cyc_List_List
_temp136; struct Cyc_List_List* _temp137; int _temp139; struct Cyc_List_List
_temp142; struct Cyc_List_List* _temp143; int _temp145; struct Cyc_List_List
_temp148; struct Cyc_List_List* _temp149; struct Cyc_List_List _temp151; struct
Cyc_List_List* _temp152; int _temp154; int _temp156; _LL125: if( _temp123 ==  0){
goto _LL126;} else{ goto _LL127;} _LL127: if( _temp123 ==  0){ goto _LL129;}
else{ _temp136=* _temp123; _LL140: _temp139=( int) _temp136.hd; goto _LL138;
_LL138: _temp137= _temp136.tl; if( _temp137 ==  0){ goto _LL135;} else{ goto
_LL129;}} _LL135: if( _temp139 == 'l'){ goto _LL128;} else{ goto _LL129;} _LL129:
if( _temp123 ==  0){ goto _LL131;} else{ _temp142=* _temp123; _LL146: _temp145=(
int) _temp142.hd; goto _LL144; _LL144: _temp143= _temp142.tl; if( _temp143 ==  0){
goto _LL141;} else{ goto _LL131;}} _LL141: if( _temp145 == 'h'){ goto _LL130;}
else{ goto _LL131;} _LL131: if( _temp123 ==  0){ goto _LL133;} else{ _temp148=*
_temp123; _LL157: _temp156=( int) _temp148.hd; goto _LL150; _LL150: _temp149=
_temp148.tl; if( _temp149 ==  0){ goto _LL133;} else{ _temp151=* _temp149;
_LL155: _temp154=( int) _temp151.hd; goto _LL153; _LL153: _temp152= _temp151.tl;
if( _temp152 ==  0){ goto _LL147;} else{ goto _LL133;}}} _LL147: if( _temp156 == 'h'?
_temp154 == 'h': 0){ goto _LL132;} else{ goto _LL133;} _LL133: goto _LL134;
_LL126: t= Cyc_Absyn_sint_t; goto _LL124; _LL128: t= Cyc_Absyn_slong_t; goto
_LL124; _LL130: t= Cyc_Absyn_sshort_t; goto _LL124; _LL132: t= Cyc_Absyn_schar_t;
goto _LL124; _LL134:({ struct Cyc_Stdio_Int_pa_struct _temp160; _temp160.tag=
Cyc_Stdio_Int_pa; _temp160.f1=( int)(( unsigned int)(( int) _temp63));{ struct
Cyc_Stdio_String_pa_struct _temp159; _temp159.tag= Cyc_Stdio_String_pa; _temp159.f1=(
struct _tagged_arr) Cyc_String_implode( _temp65);{ void* _temp158[ 2u]={&
_temp159,& _temp160}; Cyc_Tcutil_terr( loc, _tag_arr("length modifier '%s' is not allowed with %%%c",
sizeof( unsigned char), 46u), _tag_arr( _temp158, sizeof( void*), 2u));}}});
return 0; _LL124:;} _temp55=({ struct Cyc_List_List* _temp161=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp161->hd=( void*) t; _temp161->tl=
_temp55; _temp161;}); break; case 'u': _LL120:{ struct Cyc_List_List* f= _temp71;
for( 0; f !=  0; f=(( struct Cyc_List_List*) _check_null( f))->tl){ if(( int)((
struct Cyc_List_List*) _check_null( f))->hd == '#'){({ void* _temp163[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("Flag '#' not valid with %%u", sizeof(
unsigned char), 28u), _tag_arr( _temp163, sizeof( void*), 0u));}); return 0;}}}
goto _LL162; case 'o': _LL162: goto _LL164; case 'x': _LL164: goto _LL165; case
'X': _LL165:{ struct Cyc_List_List* _temp167= _temp65; struct Cyc_List_List
_temp180; struct Cyc_List_List* _temp181; int _temp183; struct Cyc_List_List
_temp186; struct Cyc_List_List* _temp187; int _temp189; struct Cyc_List_List
_temp192; struct Cyc_List_List* _temp193; struct Cyc_List_List _temp195; struct
Cyc_List_List* _temp196; int _temp198; int _temp200; _LL169: if( _temp167 ==  0){
goto _LL170;} else{ goto _LL171;} _LL171: if( _temp167 ==  0){ goto _LL173;}
else{ _temp180=* _temp167; _LL184: _temp183=( int) _temp180.hd; goto _LL182;
_LL182: _temp181= _temp180.tl; if( _temp181 ==  0){ goto _LL179;} else{ goto
_LL173;}} _LL179: if( _temp183 == 'l'){ goto _LL172;} else{ goto _LL173;} _LL173:
if( _temp167 ==  0){ goto _LL175;} else{ _temp186=* _temp167; _LL190: _temp189=(
int) _temp186.hd; goto _LL188; _LL188: _temp187= _temp186.tl; if( _temp187 ==  0){
goto _LL185;} else{ goto _LL175;}} _LL185: if( _temp189 == 'h'){ goto _LL174;}
else{ goto _LL175;} _LL175: if( _temp167 ==  0){ goto _LL177;} else{ _temp192=*
_temp167; _LL201: _temp200=( int) _temp192.hd; goto _LL194; _LL194: _temp193=
_temp192.tl; if( _temp193 ==  0){ goto _LL177;} else{ _temp195=* _temp193;
_LL199: _temp198=( int) _temp195.hd; goto _LL197; _LL197: _temp196= _temp195.tl;
if( _temp196 ==  0){ goto _LL191;} else{ goto _LL177;}}} _LL191: if( _temp200 == 'h'?
_temp198 == 'h': 0){ goto _LL176;} else{ goto _LL177;} _LL177: goto _LL178;
_LL170: t= Cyc_Absyn_uint_t; goto _LL168; _LL172: t= Cyc_Absyn_ulong_t; goto
_LL168; _LL174: t= Cyc_Absyn_ushort_t; goto _LL168; _LL176: t= Cyc_Absyn_uchar_t;
goto _LL168; _LL178:({ struct Cyc_Stdio_Int_pa_struct _temp204; _temp204.tag=
Cyc_Stdio_Int_pa; _temp204.f1=( int)(( unsigned int)(( int) _temp63));{ struct
Cyc_Stdio_String_pa_struct _temp203; _temp203.tag= Cyc_Stdio_String_pa; _temp203.f1=(
struct _tagged_arr) Cyc_String_implode( _temp65);{ void* _temp202[ 2u]={&
_temp203,& _temp204}; Cyc_Tcutil_terr( loc, _tag_arr("length modifier '%s' is not allowed with %%%c",
sizeof( unsigned char), 46u), _tag_arr( _temp202, sizeof( void*), 2u));}}});
return 0; _LL168:;} _temp55=({ struct Cyc_List_List* _temp205=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp205->hd=( void*) t; _temp205->tl=
_temp55; _temp205;}); break; case 'f': _LL166: goto _LL206; case 'F': _LL206:
goto _LL207; case 'e': _LL207: goto _LL208; case 'E': _LL208: goto _LL209; case
'g': _LL209: goto _LL210; case 'G': _LL210: goto _LL211; case 'a': _LL211: goto
_LL212; case 'A': _LL212: if( _temp65 !=  0){({ struct Cyc_Stdio_Int_pa_struct
_temp215; _temp215.tag= Cyc_Stdio_Int_pa; _temp215.f1=( int)(( unsigned int)((
int) _temp63));{ void* _temp214[ 1u]={& _temp215}; Cyc_Tcutil_terr( loc,
_tag_arr("length modifiers are not allowed with %%%c", sizeof( unsigned char),
43u), _tag_arr( _temp214, sizeof( void*), 1u));}}); return 0;} _temp55=({ struct
Cyc_List_List* _temp216=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp216->hd=( void*) Cyc_Absyn_double_t; _temp216->tl= _temp55; _temp216;});
break; case 'c': _LL213:{ struct Cyc_List_List* f= _temp71; for( 0; f !=  0; f=((
struct Cyc_List_List*) _check_null( f))->tl){ if(( int)(( struct Cyc_List_List*)
_check_null( f))->hd == '#'? 1:( int)(( struct Cyc_List_List*) _check_null( f))->hd
== '0'){({ struct Cyc_Stdio_Int_pa_struct _temp219; _temp219.tag= Cyc_Stdio_Int_pa;
_temp219.f1=( int)(( unsigned int)(( int)(( struct Cyc_List_List*) _check_null(
f))->hd));{ void* _temp218[ 1u]={& _temp219}; Cyc_Tcutil_terr( loc, _tag_arr("flag '%c' not allowed with %%c",
sizeof( unsigned char), 31u), _tag_arr( _temp218, sizeof( void*), 1u));}});
return 0;}}} if( _temp65 !=  0){({ struct Cyc_Stdio_String_pa_struct _temp221;
_temp221.tag= Cyc_Stdio_String_pa; _temp221.f1=( struct _tagged_arr) Cyc_String_implode(
_temp65);{ void* _temp220[ 1u]={& _temp221}; Cyc_Tcutil_terr( loc, _tag_arr("length modifier '%s' not allowed with %%c",
sizeof( unsigned char), 42u), _tag_arr( _temp220, sizeof( void*), 1u));}});
return 0;} if( _temp67 !=  0){({ struct Cyc_Stdio_String_pa_struct _temp223;
_temp223.tag= Cyc_Stdio_String_pa; _temp223.f1=( struct _tagged_arr) Cyc_String_implode(
_temp67);{ void* _temp222[ 1u]={& _temp223}; Cyc_Tcutil_terr( loc, _tag_arr("precision '%s' not allowed with %%c",
sizeof( unsigned char), 36u), _tag_arr( _temp222, sizeof( void*), 1u));}});
return 0;} _temp55=({ struct Cyc_List_List* _temp224=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp224->hd=( void*) Cyc_Absyn_sint_t;
_temp224->tl= _temp55; _temp224;}); break; case 's': _LL217: if( _temp71 !=  0){({
struct Cyc_Stdio_String_pa_struct _temp227; _temp227.tag= Cyc_Stdio_String_pa;
_temp227.f1=( struct _tagged_arr) Cyc_String_implode( _temp71);{ void* _temp226[
1u]={& _temp227}; Cyc_Tcutil_terr( loc, _tag_arr("flags '%s' not allowed with %%s",
sizeof( unsigned char), 32u), _tag_arr( _temp226, sizeof( void*), 1u));}});
return 0;} if( _temp69 !=  0){({ struct Cyc_Stdio_String_pa_struct _temp229;
_temp229.tag= Cyc_Stdio_String_pa; _temp229.f1=( struct _tagged_arr) Cyc_String_implode(
_temp69);{ void* _temp228[ 1u]={& _temp229}; Cyc_Tcutil_terr( loc, _tag_arr("width '%s' not allowed with %%s",
sizeof( unsigned char), 32u), _tag_arr( _temp228, sizeof( void*), 1u));}});
return 0;} if( _temp67 !=  0){({ struct Cyc_Stdio_String_pa_struct _temp231;
_temp231.tag= Cyc_Stdio_String_pa; _temp231.f1=( struct _tagged_arr) Cyc_String_implode(
_temp67);{ void* _temp230[ 1u]={& _temp231}; Cyc_Tcutil_terr( loc, _tag_arr("precision '%s' not allowed with %%s",
sizeof( unsigned char), 36u), _tag_arr( _temp230, sizeof( void*), 1u));}});
return 0;} if( _temp65 !=  0){({ struct Cyc_Stdio_String_pa_struct _temp233;
_temp233.tag= Cyc_Stdio_String_pa; _temp233.f1=( struct _tagged_arr) Cyc_String_implode(
_temp65);{ void* _temp232[ 1u]={& _temp233}; Cyc_Tcutil_terr( loc, _tag_arr("length modifier '%s' not allowed with %%s",
sizeof( unsigned char), 42u), _tag_arr( _temp232, sizeof( void*), 1u));}});
return 0;} _temp55=({ struct Cyc_List_List* _temp234=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp234->hd=( void*) Cyc_Absyn_const_string_typ(
Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,({ struct Cyc_Core_Opt*
_temp235=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp235->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp235;}))); _temp234->tl=
_temp55; _temp234;}); break; case 'p': _LL225:({ void* _temp237[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("%%p not supported", sizeof( unsigned char), 18u), _tag_arr(
_temp237, sizeof( void*), 0u));}); return 0; case 'n': _LL236:{ struct Cyc_List_List*
f= _temp71; for( 0; f !=  0; f=(( struct Cyc_List_List*) _check_null( f))->tl){
if(( int)(( struct Cyc_List_List*) _check_null( f))->hd == '#'? 1:( int)((
struct Cyc_List_List*) _check_null( f))->hd == '0'){({ struct Cyc_Stdio_Int_pa_struct
_temp240; _temp240.tag= Cyc_Stdio_Int_pa; _temp240.f1=( int)(( unsigned int)((
int)(( struct Cyc_List_List*) _check_null( f))->hd));{ void* _temp239[ 1u]={&
_temp240}; Cyc_Tcutil_terr( loc, _tag_arr("flag '%c' not allowed with %%n",
sizeof( unsigned char), 31u), _tag_arr( _temp239, sizeof( void*), 1u));}});
return 0;}}} if( _temp67 !=  0){({ struct Cyc_Stdio_String_pa_struct _temp242;
_temp242.tag= Cyc_Stdio_String_pa; _temp242.f1=( struct _tagged_arr) Cyc_String_implode(
_temp67);{ void* _temp241[ 1u]={& _temp242}; Cyc_Tcutil_terr( loc, _tag_arr("precision '%s' not allowed with %%n",
sizeof( unsigned char), 36u), _tag_arr( _temp241, sizeof( void*), 1u));}});
return 0;}{ struct Cyc_Core_Opt* _temp243=({ struct Cyc_Core_Opt* _temp282=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp282->v=(
void*) Cyc_Tcenv_lookup_type_vars( te); _temp282;});{ struct Cyc_List_List*
_temp244= _temp65; struct Cyc_List_List _temp257; struct Cyc_List_List* _temp258;
int _temp260; struct Cyc_List_List _temp263; struct Cyc_List_List* _temp264; int
_temp266; struct Cyc_List_List _temp269; struct Cyc_List_List* _temp270; struct
Cyc_List_List _temp272; struct Cyc_List_List* _temp273; int _temp275; int
_temp277; _LL246: if( _temp244 ==  0){ goto _LL247;} else{ goto _LL248;} _LL248:
if( _temp244 ==  0){ goto _LL250;} else{ _temp257=* _temp244; _LL261: _temp260=(
int) _temp257.hd; goto _LL259; _LL259: _temp258= _temp257.tl; if( _temp258 ==  0){
goto _LL256;} else{ goto _LL250;}} _LL256: if( _temp260 == 'l'){ goto _LL249;}
else{ goto _LL250;} _LL250: if( _temp244 ==  0){ goto _LL252;} else{ _temp263=*
_temp244; _LL267: _temp266=( int) _temp263.hd; goto _LL265; _LL265: _temp264=
_temp263.tl; if( _temp264 ==  0){ goto _LL262;} else{ goto _LL252;}} _LL262: if(
_temp266 == 'h'){ goto _LL251;} else{ goto _LL252;} _LL252: if( _temp244 ==  0){
goto _LL254;} else{ _temp269=* _temp244; _LL278: _temp277=( int) _temp269.hd;
goto _LL271; _LL271: _temp270= _temp269.tl; if( _temp270 ==  0){ goto _LL254;}
else{ _temp272=* _temp270; _LL276: _temp275=( int) _temp272.hd; goto _LL274;
_LL274: _temp273= _temp272.tl; if( _temp273 ==  0){ goto _LL268;} else{ goto
_LL254;}}} _LL268: if( _temp277 == 'h'? _temp275 == 'h': 0){ goto _LL253;} else{
goto _LL254;} _LL254: goto _LL255; _LL247: t= Cyc_Absyn_at_typ( Cyc_Absyn_sint_t,
Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp243), Cyc_Absyn_empty_tqual());
goto _LL245; _LL249: t= Cyc_Absyn_at_typ( Cyc_Absyn_ulong_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp243), Cyc_Absyn_empty_tqual());
goto _LL245; _LL251: t= Cyc_Absyn_at_typ( Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp243), Cyc_Absyn_empty_tqual());
goto _LL245; _LL253: t= Cyc_Absyn_at_typ( Cyc_Absyn_schar_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp243), Cyc_Absyn_empty_tqual());
goto _LL245; _LL255:({ struct Cyc_Stdio_String_pa_struct _temp280; _temp280.tag=
Cyc_Stdio_String_pa; _temp280.f1=( struct _tagged_arr) Cyc_String_implode(
_temp65);{ void* _temp279[ 1u]={& _temp280}; Cyc_Tcutil_terr( loc, _tag_arr("length modifier '%s' not allowed with %%n",
sizeof( unsigned char), 42u), _tag_arr( _temp279, sizeof( void*), 1u));}});
return 0; _LL245:;} _temp55=({ struct Cyc_List_List* _temp281=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp281->hd=( void*) t; _temp281->tl=
_temp55; _temp281;}); break;} case '%': _LL238: if( _temp71 !=  0){({ struct Cyc_Stdio_String_pa_struct
_temp285; _temp285.tag= Cyc_Stdio_String_pa; _temp285.f1=( struct _tagged_arr)
Cyc_String_implode( _temp71);{ void* _temp284[ 1u]={& _temp285}; Cyc_Tcutil_terr(
loc, _tag_arr("flags '%s' not allowed with %%%%", sizeof( unsigned char), 33u),
_tag_arr( _temp284, sizeof( void*), 1u));}}); return 0;} if( _temp69 !=  0){({
struct Cyc_Stdio_String_pa_struct _temp287; _temp287.tag= Cyc_Stdio_String_pa;
_temp287.f1=( struct _tagged_arr) Cyc_String_implode( _temp69);{ void* _temp286[
1u]={& _temp287}; Cyc_Tcutil_terr( loc, _tag_arr("width '%s' not allowed with %%%%",
sizeof( unsigned char), 33u), _tag_arr( _temp286, sizeof( void*), 1u));}});
return 0;} if( _temp67 !=  0){({ struct Cyc_Stdio_String_pa_struct _temp289;
_temp289.tag= Cyc_Stdio_String_pa; _temp289.f1=( struct _tagged_arr) Cyc_String_implode(
_temp67);{ void* _temp288[ 1u]={& _temp289}; Cyc_Tcutil_terr( loc, _tag_arr("precision '%s' not allowed with %%%%",
sizeof( unsigned char), 37u), _tag_arr( _temp288, sizeof( void*), 1u));}});
return 0;} if( _temp65 !=  0){({ struct Cyc_Stdio_String_pa_struct _temp291;
_temp291.tag= Cyc_Stdio_String_pa; _temp291.f1=( struct _tagged_arr) Cyc_String_implode(
_temp65);{ void* _temp290[ 1u]={& _temp291}; Cyc_Tcutil_terr( loc, _tag_arr("length modifier '%s' not allowed with %%%%",
sizeof( unsigned char), 43u), _tag_arr( _temp290, sizeof( void*), 1u));}});
return 0;} break; default: _LL283: return 0;}}}}} return Cyc_List_imp_rev(
_temp55);} struct _tuple4{ int f1; struct Cyc_List_List* f2; struct Cyc_List_List*
f3; unsigned char f4; int f5; } ; struct Cyc_Core_Opt* Cyc_Formatstr_parse_inputformat(
struct _tagged_arr s, int i){ int _temp293= Cyc_String_strlen( s); if( i <  0? 1:
i >=  _temp293){ return 0;}{ int _temp294= 0; unsigned char _temp295=*(( const
unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i)); if(
_temp295 == '*'){ _temp294= 1; i ++; if( i >=  _temp293){ return 0;}}{ struct
Cyc_List_List* _temp296= 0; for( 0; i <  _temp293; i ++){ _temp295=*(( const
unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i)); if(
isdigit(( int) _temp295)){ _temp296=({ struct Cyc_List_List* _temp297=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp297->hd=( void*)((
int) _temp295); _temp297->tl= _temp296; _temp297;});} else{ break;}} if( i >= 
_temp293){ return 0;} _temp296=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp296);{ struct Cyc_List_List* _temp298= 0; _temp295=*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i));
switch( _temp295){ case 'h': _LL299: _temp298=({ struct Cyc_List_List* _temp301=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp301->hd=(
void*)(( int) _temp295); _temp301->tl= _temp298; _temp301;}); i ++; if( i >= 
_temp293){ return 0;} _temp295=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( _temp295 == 'h'){
_temp298=({ struct Cyc_List_List* _temp302=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp302->hd=( void*)(( int) _temp295); _temp302->tl=
_temp298; _temp302;}); i ++;} break; case 'l': _LL300: _temp298=({ struct Cyc_List_List*
_temp304=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp304->hd=( void*)(( int) _temp295); _temp304->tl= _temp298; _temp304;}); i
++; if( i >=  _temp293){ return 0;} _temp295=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( _temp295 == 'l'){
_temp298=({ struct Cyc_List_List* _temp305=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp305->hd=( void*)(( int) _temp295); _temp305->tl=
_temp298; _temp305;}); i ++;} break; case 'j': _LL303: goto _LL306; case 'z':
_LL306: goto _LL307; case 't': _LL307: goto _LL308; case 'L': _LL308: _temp298=({
struct Cyc_List_List* _temp310=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp310->hd=( void*)(( int) _temp295); _temp310->tl=
_temp298; _temp310;}); i ++; break; default: _LL309: break;} if( i >=  _temp293){
return 0;} _temp298=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp298); _temp295=*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), i)); switch( _temp295){ case 'd': _LL312: break; case 'i':
_LL313: break; case 'o': _LL314: break; case 'u': _LL315: break; case 'x':
_LL316: break; case 'X': _LL317: break; case 'f': _LL318: break; case 'F':
_LL319: break; case 'e': _LL320: break; case 'E': _LL321: break; case 'g':
_LL322: break; case 'G': _LL323: break; case 'a': _LL324: break; case 'A':
_LL325: break; case 'c': _LL326: break; case 's': _LL327: break; case 'p':
_LL328: break; case 'n': _LL329: break; case '%': _LL330: break; default: _LL331:
return 0;} return({ struct Cyc_Core_Opt* _temp333=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp333->v=( void*)({ struct _tuple4*
_temp334=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp334->f1=
_temp294; _temp334->f2= _temp296; _temp334->f3= _temp298; _temp334->f4= _temp295;
_temp334->f5= i +  1; _temp334;}); _temp333;});}}}} struct Cyc_List_List* Cyc_Formatstr_get_scanf_typs(
struct Cyc_Tcenv_Tenv* te, struct _tagged_arr s, struct Cyc_Position_Segment*
loc){ int _temp335= Cyc_String_strlen( s); struct Cyc_List_List* _temp336= 0;
int i; for( i= 0; i <  _temp335; i ++){ if(*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)) != '%'){ continue;}{
struct Cyc_Core_Opt* _temp337= Cyc_Formatstr_parse_inputformat( s, i +  1); if(
_temp337 ==  0){({ void* _temp338[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bad format string",
sizeof( unsigned char), 18u), _tag_arr( _temp338, sizeof( void*), 0u));});
return 0;}{ struct _tuple4 _temp341; int _temp342; unsigned char _temp344;
struct Cyc_List_List* _temp346; struct Cyc_List_List* _temp348; int _temp350;
struct _tuple4* _temp339=( struct _tuple4*)(( struct Cyc_Core_Opt*) _check_null(
_temp337))->v; _temp341=* _temp339; _LL351: _temp350= _temp341.f1; goto _LL349;
_LL349: _temp348= _temp341.f2; goto _LL347; _LL347: _temp346= _temp341.f3; goto
_LL345; _LL345: _temp344= _temp341.f4; goto _LL343; _LL343: _temp342= _temp341.f5;
goto _LL340; _LL340: i= _temp342 -  1;{ struct Cyc_List_List* _temp352= _temp346;
struct Cyc_List_List _temp359; struct Cyc_List_List* _temp360; int _temp362;
_LL354: if( _temp352 ==  0){ goto _LL356;} else{ _temp359=* _temp352; _LL363:
_temp362=( int) _temp359.hd; goto _LL361; _LL361: _temp360= _temp359.tl; if(
_temp360 ==  0){ goto _LL358;} else{ goto _LL356;}} _LL358: if(( _temp362 == 'j'?
1: _temp362 == 'z')? 1: _temp362 == 't'){ goto _LL355;} else{ goto _LL356;}
_LL356: goto _LL357; _LL355:({ struct Cyc_Stdio_Int_pa_struct _temp365; _temp365.tag=
Cyc_Stdio_Int_pa; _temp365.f1=( int)(( unsigned int) _temp362);{ void* _temp364[
1u]={& _temp365}; Cyc_Tcutil_terr( loc, _tag_arr("length modifier '%c' is not supported",
sizeof( unsigned char), 38u), _tag_arr( _temp364, sizeof( void*), 1u));}});
return 0; _LL357: goto _LL353; _LL353:;} if( _temp350){ continue;}{ void* t;
switch( _temp344){ case 'd': _LL366: goto _LL367; case 'i': _LL367: { struct Cyc_Core_Opt*
_temp369=({ struct Cyc_Core_Opt* _temp409=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp409->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp409;});{ struct Cyc_List_List* _temp370= _temp346; struct Cyc_List_List
_temp383; struct Cyc_List_List* _temp384; int _temp386; struct Cyc_List_List
_temp389; struct Cyc_List_List* _temp390; int _temp392; struct Cyc_List_List
_temp395; struct Cyc_List_List* _temp396; struct Cyc_List_List _temp398; struct
Cyc_List_List* _temp399; int _temp401; int _temp403; _LL372: if( _temp370 ==  0){
goto _LL373;} else{ goto _LL374;} _LL374: if( _temp370 ==  0){ goto _LL376;}
else{ _temp383=* _temp370; _LL387: _temp386=( int) _temp383.hd; goto _LL385;
_LL385: _temp384= _temp383.tl; if( _temp384 ==  0){ goto _LL382;} else{ goto
_LL376;}} _LL382: if( _temp386 == 'l'){ goto _LL375;} else{ goto _LL376;} _LL376:
if( _temp370 ==  0){ goto _LL378;} else{ _temp389=* _temp370; _LL393: _temp392=(
int) _temp389.hd; goto _LL391; _LL391: _temp390= _temp389.tl; if( _temp390 ==  0){
goto _LL388;} else{ goto _LL378;}} _LL388: if( _temp392 == 'h'){ goto _LL377;}
else{ goto _LL378;} _LL378: if( _temp370 ==  0){ goto _LL380;} else{ _temp395=*
_temp370; _LL404: _temp403=( int) _temp395.hd; goto _LL397; _LL397: _temp396=
_temp395.tl; if( _temp396 ==  0){ goto _LL380;} else{ _temp398=* _temp396;
_LL402: _temp401=( int) _temp398.hd; goto _LL400; _LL400: _temp399= _temp398.tl;
if( _temp399 ==  0){ goto _LL394;} else{ goto _LL380;}}} _LL394: if( _temp403 == 'h'?
_temp401 == 'h': 0){ goto _LL379;} else{ goto _LL380;} _LL380: goto _LL381;
_LL373: t= Cyc_Absyn_at_typ( Cyc_Absyn_sint_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp369), Cyc_Absyn_empty_tqual()); goto _LL371; _LL375: t=
Cyc_Absyn_at_typ( Cyc_Absyn_slong_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp369), Cyc_Absyn_empty_tqual()); goto _LL371; _LL377: t=
Cyc_Absyn_at_typ( Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp369), Cyc_Absyn_empty_tqual()); goto _LL371; _LL379: t=
Cyc_Absyn_at_typ( Cyc_Absyn_schar_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp369), Cyc_Absyn_empty_tqual()); goto _LL371; _LL381:({
struct Cyc_Stdio_Int_pa_struct _temp407; _temp407.tag= Cyc_Stdio_Int_pa;
_temp407.f1=( int)(( unsigned int)(( int) _temp344));{ struct Cyc_Stdio_String_pa_struct
_temp406; _temp406.tag= Cyc_Stdio_String_pa; _temp406.f1=( struct _tagged_arr)
Cyc_String_implode( _temp346);{ void* _temp405[ 2u]={& _temp406,& _temp407}; Cyc_Tcutil_terr(
loc, _tag_arr("length modifier '%s' is not allowed with %%%c", sizeof(
unsigned char), 46u), _tag_arr( _temp405, sizeof( void*), 2u));}}}); return 0;
_LL371:;} _temp336=({ struct Cyc_List_List* _temp408=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp408->hd=( void*) t; _temp408->tl=
_temp336; _temp408;}); break;} case 'u': _LL368: goto _LL410; case 'o': _LL410:
goto _LL411; case 'x': _LL411: goto _LL412; case 'X': _LL412: { struct Cyc_Core_Opt*
_temp414=({ struct Cyc_Core_Opt* _temp454=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp454->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp454;});{ struct Cyc_List_List* _temp415= _temp346; struct Cyc_List_List
_temp428; struct Cyc_List_List* _temp429; int _temp431; struct Cyc_List_List
_temp434; struct Cyc_List_List* _temp435; int _temp437; struct Cyc_List_List
_temp440; struct Cyc_List_List* _temp441; struct Cyc_List_List _temp443; struct
Cyc_List_List* _temp444; int _temp446; int _temp448; _LL417: if( _temp415 ==  0){
goto _LL418;} else{ goto _LL419;} _LL419: if( _temp415 ==  0){ goto _LL421;}
else{ _temp428=* _temp415; _LL432: _temp431=( int) _temp428.hd; goto _LL430;
_LL430: _temp429= _temp428.tl; if( _temp429 ==  0){ goto _LL427;} else{ goto
_LL421;}} _LL427: if( _temp431 == 'l'){ goto _LL420;} else{ goto _LL421;} _LL421:
if( _temp415 ==  0){ goto _LL423;} else{ _temp434=* _temp415; _LL438: _temp437=(
int) _temp434.hd; goto _LL436; _LL436: _temp435= _temp434.tl; if( _temp435 ==  0){
goto _LL433;} else{ goto _LL423;}} _LL433: if( _temp437 == 'h'){ goto _LL422;}
else{ goto _LL423;} _LL423: if( _temp415 ==  0){ goto _LL425;} else{ _temp440=*
_temp415; _LL449: _temp448=( int) _temp440.hd; goto _LL442; _LL442: _temp441=
_temp440.tl; if( _temp441 ==  0){ goto _LL425;} else{ _temp443=* _temp441;
_LL447: _temp446=( int) _temp443.hd; goto _LL445; _LL445: _temp444= _temp443.tl;
if( _temp444 ==  0){ goto _LL439;} else{ goto _LL425;}}} _LL439: if( _temp448 == 'h'?
_temp446 == 'h': 0){ goto _LL424;} else{ goto _LL425;} _LL425: goto _LL426;
_LL418: t= Cyc_Absyn_at_typ( Cyc_Absyn_uint_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp414), Cyc_Absyn_empty_tqual()); goto _LL416; _LL420: t=
Cyc_Absyn_at_typ( Cyc_Absyn_ulong_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp414), Cyc_Absyn_empty_tqual()); goto _LL416; _LL422: t=
Cyc_Absyn_at_typ( Cyc_Absyn_ushort_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp414), Cyc_Absyn_empty_tqual()); goto _LL416; _LL424: t=
Cyc_Absyn_at_typ( Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp414), Cyc_Absyn_empty_tqual()); goto _LL416; _LL426:({
struct Cyc_Stdio_Int_pa_struct _temp452; _temp452.tag= Cyc_Stdio_Int_pa;
_temp452.f1=( int)(( unsigned int)(( int) _temp344));{ struct Cyc_Stdio_String_pa_struct
_temp451; _temp451.tag= Cyc_Stdio_String_pa; _temp451.f1=( struct _tagged_arr)
Cyc_String_implode( _temp346);{ void* _temp450[ 2u]={& _temp451,& _temp452}; Cyc_Tcutil_terr(
loc, _tag_arr("length modifier '%s' is not allowed with %%%c", sizeof(
unsigned char), 46u), _tag_arr( _temp450, sizeof( void*), 2u));}}}); return 0;
_LL416:;} _temp336=({ struct Cyc_List_List* _temp453=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp453->hd=( void*) t; _temp453->tl=
_temp336; _temp453;}); break;} case 'f': _LL413: goto _LL455; case 'F': _LL455:
goto _LL456; case 'e': _LL456: goto _LL457; case 'E': _LL457: goto _LL458; case
'g': _LL458: goto _LL459; case 'G': _LL459: goto _LL460; case 'a': _LL460: goto
_LL461; case 'A': _LL461: { struct Cyc_Core_Opt* _temp463=({ struct Cyc_Core_Opt*
_temp482=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp482->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp482;});{ struct Cyc_List_List*
_temp464= _temp346; struct Cyc_List_List _temp473; struct Cyc_List_List*
_temp474; int _temp476; _LL466: if( _temp464 ==  0){ goto _LL467;} else{ goto
_LL468;} _LL468: if( _temp464 ==  0){ goto _LL470;} else{ _temp473=* _temp464;
_LL477: _temp476=( int) _temp473.hd; goto _LL475; _LL475: _temp474= _temp473.tl;
if( _temp474 ==  0){ goto _LL472;} else{ goto _LL470;}} _LL472: if( _temp476 == 'l'){
goto _LL469;} else{ goto _LL470;} _LL470: goto _LL471; _LL467: t= Cyc_Absyn_at_typ(
Cyc_Absyn_float_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp463), Cyc_Absyn_empty_tqual()); goto _LL465; _LL469: t= Cyc_Absyn_at_typ(
Cyc_Absyn_double_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp463), Cyc_Absyn_empty_tqual()); goto _LL465; _LL471:({ struct Cyc_Stdio_Int_pa_struct
_temp480; _temp480.tag= Cyc_Stdio_Int_pa; _temp480.f1=( int)(( unsigned int)((
int) _temp344));{ struct Cyc_Stdio_String_pa_struct _temp479; _temp479.tag= Cyc_Stdio_String_pa;
_temp479.f1=( struct _tagged_arr) Cyc_String_implode( _temp346);{ void* _temp478[
2u]={& _temp479,& _temp480}; Cyc_Tcutil_terr( loc, _tag_arr("length modifier '%s' is not allowed with %%%c",
sizeof( unsigned char), 46u), _tag_arr( _temp478, sizeof( void*), 2u));}}});
return 0; _LL465:;} _temp336=({ struct Cyc_List_List* _temp481=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp481->hd=( void*) t; _temp481->tl=
_temp336; _temp481;}); break;} case 'c': _LL462:({ void* _temp484[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("%%c is not supported", sizeof( unsigned char), 21u), _tag_arr(
_temp484, sizeof( void*), 0u));}); return 0; case 's': _LL483:({ void* _temp486[
0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("%%s is not supported", sizeof(
unsigned char), 21u), _tag_arr( _temp486, sizeof( void*), 0u));}); return 0;
case '[': _LL485:({ void* _temp488[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("%%[ is not supported",
sizeof( unsigned char), 21u), _tag_arr( _temp488, sizeof( void*), 0u));});
return 0; case 'p': _LL487:({ void* _temp490[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("%%p is not supported", sizeof( unsigned char), 21u), _tag_arr(
_temp490, sizeof( void*), 0u));}); return 0; case 'n': _LL489: { struct Cyc_Core_Opt*
_temp492=({ struct Cyc_Core_Opt* _temp531=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp531->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp531;});{ struct Cyc_List_List* _temp493= _temp346; struct Cyc_List_List
_temp506; struct Cyc_List_List* _temp507; int _temp509; struct Cyc_List_List
_temp512; struct Cyc_List_List* _temp513; int _temp515; struct Cyc_List_List
_temp518; struct Cyc_List_List* _temp519; struct Cyc_List_List _temp521; struct
Cyc_List_List* _temp522; int _temp524; int _temp526; _LL495: if( _temp493 ==  0){
goto _LL496;} else{ goto _LL497;} _LL497: if( _temp493 ==  0){ goto _LL499;}
else{ _temp506=* _temp493; _LL510: _temp509=( int) _temp506.hd; goto _LL508;
_LL508: _temp507= _temp506.tl; if( _temp507 ==  0){ goto _LL505;} else{ goto
_LL499;}} _LL505: if( _temp509 == 'l'){ goto _LL498;} else{ goto _LL499;} _LL499:
if( _temp493 ==  0){ goto _LL501;} else{ _temp512=* _temp493; _LL516: _temp515=(
int) _temp512.hd; goto _LL514; _LL514: _temp513= _temp512.tl; if( _temp513 ==  0){
goto _LL511;} else{ goto _LL501;}} _LL511: if( _temp515 == 'h'){ goto _LL500;}
else{ goto _LL501;} _LL501: if( _temp493 ==  0){ goto _LL503;} else{ _temp518=*
_temp493; _LL527: _temp526=( int) _temp518.hd; goto _LL520; _LL520: _temp519=
_temp518.tl; if( _temp519 ==  0){ goto _LL503;} else{ _temp521=* _temp519;
_LL525: _temp524=( int) _temp521.hd; goto _LL523; _LL523: _temp522= _temp521.tl;
if( _temp522 ==  0){ goto _LL517;} else{ goto _LL503;}}} _LL517: if( _temp526 == 'h'?
_temp524 == 'h': 0){ goto _LL502;} else{ goto _LL503;} _LL503: goto _LL504;
_LL496: t= Cyc_Absyn_at_typ( Cyc_Absyn_sint_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp492), Cyc_Absyn_empty_tqual()); goto _LL494; _LL498: t=
Cyc_Absyn_at_typ( Cyc_Absyn_ulong_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp492), Cyc_Absyn_empty_tqual()); goto _LL494; _LL500: t=
Cyc_Absyn_at_typ( Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp492), Cyc_Absyn_empty_tqual()); goto _LL494; _LL502: t=
Cyc_Absyn_at_typ( Cyc_Absyn_schar_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp492), Cyc_Absyn_empty_tqual()); goto _LL494; _LL504:({
struct Cyc_Stdio_String_pa_struct _temp529; _temp529.tag= Cyc_Stdio_String_pa;
_temp529.f1=( struct _tagged_arr) Cyc_String_implode( _temp346);{ void* _temp528[
1u]={& _temp529}; Cyc_Tcutil_terr( loc, _tag_arr("length modifier '%s' not allowed with %%n",
sizeof( unsigned char), 42u), _tag_arr( _temp528, sizeof( void*), 1u));}});
return 0; _LL494:;} _temp336=({ struct Cyc_List_List* _temp530=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp530->hd=( void*) t; _temp530->tl=
_temp336; _temp530;}); break;} case '%': _LL491: if( _temp350){({ void* _temp533[
0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("Assignment suppression (*) is not allowed with %%%%",
sizeof( unsigned char), 52u), _tag_arr( _temp533, sizeof( void*), 0u));});
return 0;} if( _temp348 !=  0){({ struct Cyc_Stdio_String_pa_struct _temp535;
_temp535.tag= Cyc_Stdio_String_pa; _temp535.f1=( struct _tagged_arr) Cyc_String_implode(
_temp348);{ void* _temp534[ 1u]={& _temp535}; Cyc_Tcutil_terr( loc, _tag_arr("width '%s' not allowed with %%%%",
sizeof( unsigned char), 33u), _tag_arr( _temp534, sizeof( void*), 1u));}});
return 0;} if( _temp346 !=  0){({ struct Cyc_Stdio_String_pa_struct _temp537;
_temp537.tag= Cyc_Stdio_String_pa; _temp537.f1=( struct _tagged_arr) Cyc_String_implode(
_temp346);{ void* _temp536[ 1u]={& _temp537}; Cyc_Tcutil_terr( loc, _tag_arr("length modifier '%s' not allowed with %%%%",
sizeof( unsigned char), 43u), _tag_arr( _temp536, sizeof( void*), 1u));}});
return 0;} break; default: _LL532: return 0;}}}}} return Cyc_List_imp_rev(
_temp336);}
