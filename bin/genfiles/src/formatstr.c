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
isalnum( int __c)  __attribute__(( cdecl )) ; extern int isalpha( int __c)
 __attribute__(( cdecl )) ; extern int iscntrl( int __c)  __attribute__(( cdecl
)) ; extern int isdigit( int __c)  __attribute__(( cdecl )) ; extern int isgraph(
int __c)  __attribute__(( cdecl )) ; extern int islower( int __c)
 __attribute__(( cdecl )) ; extern int isprint( int __c)  __attribute__(( cdecl
)) ; extern int ispunct( int __c)  __attribute__(( cdecl )) ; extern int isspace(
int __c)  __attribute__(( cdecl )) ; extern int isupper( int __c)
 __attribute__(( cdecl )) ; extern int isxdigit( int __c)  __attribute__(( cdecl
)) ; extern int tolower( int __c)  __attribute__(( cdecl )) ; extern int toupper(
int __c)  __attribute__(( cdecl )) ; extern int isascii( int __c)
 __attribute__(( cdecl )) ; extern int toascii( int __c)  __attribute__(( cdecl
)) ; extern int _tolower( int __c)  __attribute__(( cdecl )) ; extern int
_toupper( int __c)  __attribute__(( cdecl )) ; struct Cyc_Stdio___sFILE; extern
unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; static const int Cyc_Stdio_String_pa= 0; struct Cyc_Stdio_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_Int_pa= 1; struct
Cyc_Stdio_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Stdio_Double_pa=
2; struct Cyc_Stdio_Double_pa_struct{ int tag; double f1; } ; static const int
Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{ int tag; short*
f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
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
struct Cyc_Position_Segment*, struct _tagged_arr); extern struct Cyc_List_List*
Cyc_Formatstr_get_format_typs( struct Cyc_Tcenv_Tenv*, struct _tagged_arr,
struct Cyc_Position_Segment*); extern struct Cyc_List_List* Cyc_Formatstr_get_scanf_typs(
struct Cyc_Tcenv_Tenv*, struct _tagged_arr, struct Cyc_Position_Segment*);
extern struct Cyc_Core_Opt* Cyc_Formatstr_parse_conversionspecification( struct
_tagged_arr s, int i); static struct Cyc_Core_Opt Cyc_Formatstr_rk={( void*)((
void*) 3u)}; struct _tuple3{ struct Cyc_List_List* f1; struct Cyc_List_List* f2;
struct Cyc_List_List* f3; struct Cyc_List_List* f4; unsigned char f5; int f6; }
; struct Cyc_Core_Opt* Cyc_Formatstr_parse_conversionspecification( struct
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
+ 1); if( _temp56 == 0){ Cyc_Tcutil_terr( loc, _tag_arr("bad format string",
sizeof( unsigned char), 18u)); return 0;}{ struct _tuple3 _temp59; int _temp60;
unsigned char _temp62; struct Cyc_List_List* _temp64; struct Cyc_List_List*
_temp66; struct Cyc_List_List* _temp68; struct Cyc_List_List* _temp70; struct
_tuple3* _temp57=( struct _tuple3*)(( struct Cyc_Core_Opt*) _check_null( _temp56))->v;
_temp59=* _temp57; _LL71: _temp70= _temp59.f1; goto _LL69; _LL69: _temp68=
_temp59.f2; goto _LL67; _LL67: _temp66= _temp59.f3; goto _LL65; _LL65: _temp64=
_temp59.f4; goto _LL63; _LL63: _temp62= _temp59.f5; goto _LL61; _LL61: _temp60=
_temp59.f6; goto _LL58; _LL58: i= _temp60 - 1;{ struct Cyc_List_List* _temp72=
_temp64; struct Cyc_List_List _temp79; struct Cyc_List_List* _temp80; int
_temp82; _LL74: if( _temp72 == 0){ goto _LL76;} else{ _temp79=* _temp72; _LL83:
_temp82=( int) _temp79.hd; goto _LL81; _LL81: _temp80= _temp79.tl; if( _temp80
== 0){ goto _LL78;} else{ goto _LL76;}} _LL78: if(( _temp82 =='j'? 1: _temp82 =='z')?
1: _temp82 =='t'){ goto _LL75;} else{ goto _LL76;} _LL76: goto _LL77; _LL75: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp84)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp88=
_tag_arr("length modifier '%c' is not supported", sizeof( unsigned char), 38u);
struct Cyc_Stdio_Int_pa_struct _temp89; _temp89.tag= Cyc_Stdio_Int_pa; _temp89.f1=(
int)(( unsigned int) _temp82);{ void* _temp87=( void*)& _temp89; void* _temp85[
1u]={ _temp87}; struct _tagged_arr _temp86={( void*) _temp85,( void*) _temp85,(
void*)( _temp85 + 1u)}; _temp84( _temp88, _temp86);}})); return 0; _LL77: goto
_LL73; _LL73:;}{ struct Cyc_List_List* _temp90= _temp68; struct Cyc_List_List
_temp97; struct Cyc_List_List* _temp98; int _temp100; _LL92: if( _temp90 == 0){
goto _LL94;} else{ _temp97=* _temp90; _LL101: _temp100=( int) _temp97.hd; goto
_LL99; _LL99: _temp98= _temp97.tl; if( _temp98 == 0){ goto _LL96;} else{ goto
_LL94;}} _LL96: if( _temp100 =='*'){ goto _LL93;} else{ goto _LL94;} _LL94: goto
_LL95; _LL93: _temp55=({ struct Cyc_List_List* _temp102=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp102->hd=( void*) Cyc_Absyn_sint_t;
_temp102->tl= _temp55; _temp102;}); goto _LL91; _LL95: goto _LL91; _LL91:;}{
struct Cyc_List_List* _temp103= _temp66; struct Cyc_List_List _temp110; struct
Cyc_List_List* _temp111; struct Cyc_List_List _temp113; struct Cyc_List_List*
_temp114; int _temp116; int _temp118; _LL105: if( _temp103 == 0){ goto _LL107;}
else{ _temp110=* _temp103; _LL119: _temp118=( int) _temp110.hd; goto _LL112;
_LL112: _temp111= _temp110.tl; if( _temp111 == 0){ goto _LL107;} else{ _temp113=*
_temp111; _LL117: _temp116=( int) _temp113.hd; goto _LL115; _LL115: _temp114=
_temp113.tl; if( _temp114 == 0){ goto _LL109;} else{ goto _LL107;}}} _LL109: if(
_temp118 =='.'? _temp116 =='*': 0){ goto _LL106;} else{ goto _LL107;} _LL107:
goto _LL108; _LL106: _temp55=({ struct Cyc_List_List* _temp120=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp120->hd=( void*) Cyc_Absyn_sint_t;
_temp120->tl= _temp55; _temp120;}); goto _LL104; _LL108: goto _LL104; _LL104:;}{
void* t; switch( _temp62){ case 'd': _LL121: goto _LL122; case 'i': _LL122:{
struct Cyc_List_List* f= _temp70; for( 0; f != 0; f=(( struct Cyc_List_List*)
_check_null( f))->tl){ if(( int)(( struct Cyc_List_List*) _check_null( f))->hd
=='#'){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp124)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp128= _tag_arr("flag '#' is not valid with %%%c", sizeof(
unsigned char), 32u); struct Cyc_Stdio_Int_pa_struct _temp129; _temp129.tag= Cyc_Stdio_Int_pa;
_temp129.f1=( int)(( unsigned int)(( int) _temp62));{ void* _temp127=( void*)&
_temp129; void* _temp125[ 1u]={ _temp127}; struct _tagged_arr _temp126={( void*)
_temp125,( void*) _temp125,( void*)( _temp125 + 1u)}; _temp124( _temp128,
_temp126);}})); return 0;}}}{ struct Cyc_List_List* _temp130= _temp64; struct
Cyc_List_List _temp143; struct Cyc_List_List* _temp144; int _temp146; struct Cyc_List_List
_temp149; struct Cyc_List_List* _temp150; int _temp152; struct Cyc_List_List
_temp155; struct Cyc_List_List* _temp156; struct Cyc_List_List _temp158; struct
Cyc_List_List* _temp159; int _temp161; int _temp163; _LL132: if( _temp130 == 0){
goto _LL133;} else{ goto _LL134;} _LL134: if( _temp130 == 0){ goto _LL136;}
else{ _temp143=* _temp130; _LL147: _temp146=( int) _temp143.hd; goto _LL145;
_LL145: _temp144= _temp143.tl; if( _temp144 == 0){ goto _LL142;} else{ goto
_LL136;}} _LL142: if( _temp146 =='l'){ goto _LL135;} else{ goto _LL136;} _LL136:
if( _temp130 == 0){ goto _LL138;} else{ _temp149=* _temp130; _LL153: _temp152=(
int) _temp149.hd; goto _LL151; _LL151: _temp150= _temp149.tl; if( _temp150 == 0){
goto _LL148;} else{ goto _LL138;}} _LL148: if( _temp152 =='h'){ goto _LL137;}
else{ goto _LL138;} _LL138: if( _temp130 == 0){ goto _LL140;} else{ _temp155=*
_temp130; _LL164: _temp163=( int) _temp155.hd; goto _LL157; _LL157: _temp156=
_temp155.tl; if( _temp156 == 0){ goto _LL140;} else{ _temp158=* _temp156; _LL162:
_temp161=( int) _temp158.hd; goto _LL160; _LL160: _temp159= _temp158.tl; if(
_temp159 == 0){ goto _LL154;} else{ goto _LL140;}}} _LL154: if( _temp163 =='h'?
_temp161 =='h': 0){ goto _LL139;} else{ goto _LL140;} _LL140: goto _LL141;
_LL133: t= Cyc_Absyn_sint_t; goto _LL131; _LL135: t= Cyc_Absyn_slong_t; goto
_LL131; _LL137: t= Cyc_Absyn_sshort_t; goto _LL131; _LL139: t= Cyc_Absyn_schar_t;
goto _LL131; _LL141: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp165)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp170= _tag_arr("length modifier '%s' is not allowed with %%%c",
sizeof( unsigned char), 46u); struct Cyc_Stdio_String_pa_struct _temp172;
_temp172.tag= Cyc_Stdio_String_pa; _temp172.f1=( struct _tagged_arr) Cyc_String_implode(
_temp64);{ void* _temp168=( void*)& _temp172; struct Cyc_Stdio_Int_pa_struct
_temp171; _temp171.tag= Cyc_Stdio_Int_pa; _temp171.f1=( int)(( unsigned int)((
int) _temp62));{ void* _temp169=( void*)& _temp171; void* _temp166[ 2u]={
_temp168, _temp169}; struct _tagged_arr _temp167={( void*) _temp166,( void*)
_temp166,( void*)( _temp166 + 2u)}; _temp165( _temp170, _temp167);}}})); return
0; _LL131:;} _temp55=({ struct Cyc_List_List* _temp173=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp173->hd=( void*) t; _temp173->tl=
_temp55; _temp173;}); break; case 'u': _LL123:{ struct Cyc_List_List* f= _temp70;
for( 0; f != 0; f=(( struct Cyc_List_List*) _check_null( f))->tl){ if(( int)((
struct Cyc_List_List*) _check_null( f))->hd =='#'){ Cyc_Tcutil_terr( loc,
_tag_arr("Flag '#' not valid with %%u", sizeof( unsigned char), 28u)); return 0;}}}
goto _LL174; case 'o': _LL174: goto _LL175; case 'x': _LL175: goto _LL176; case
'X': _LL176:{ struct Cyc_List_List* _temp178= _temp64; struct Cyc_List_List
_temp191; struct Cyc_List_List* _temp192; int _temp194; struct Cyc_List_List
_temp197; struct Cyc_List_List* _temp198; int _temp200; struct Cyc_List_List
_temp203; struct Cyc_List_List* _temp204; struct Cyc_List_List _temp206; struct
Cyc_List_List* _temp207; int _temp209; int _temp211; _LL180: if( _temp178 == 0){
goto _LL181;} else{ goto _LL182;} _LL182: if( _temp178 == 0){ goto _LL184;}
else{ _temp191=* _temp178; _LL195: _temp194=( int) _temp191.hd; goto _LL193;
_LL193: _temp192= _temp191.tl; if( _temp192 == 0){ goto _LL190;} else{ goto
_LL184;}} _LL190: if( _temp194 =='l'){ goto _LL183;} else{ goto _LL184;} _LL184:
if( _temp178 == 0){ goto _LL186;} else{ _temp197=* _temp178; _LL201: _temp200=(
int) _temp197.hd; goto _LL199; _LL199: _temp198= _temp197.tl; if( _temp198 == 0){
goto _LL196;} else{ goto _LL186;}} _LL196: if( _temp200 =='h'){ goto _LL185;}
else{ goto _LL186;} _LL186: if( _temp178 == 0){ goto _LL188;} else{ _temp203=*
_temp178; _LL212: _temp211=( int) _temp203.hd; goto _LL205; _LL205: _temp204=
_temp203.tl; if( _temp204 == 0){ goto _LL188;} else{ _temp206=* _temp204; _LL210:
_temp209=( int) _temp206.hd; goto _LL208; _LL208: _temp207= _temp206.tl; if(
_temp207 == 0){ goto _LL202;} else{ goto _LL188;}}} _LL202: if( _temp211 =='h'?
_temp209 =='h': 0){ goto _LL187;} else{ goto _LL188;} _LL188: goto _LL189;
_LL181: t= Cyc_Absyn_uint_t; goto _LL179; _LL183: t= Cyc_Absyn_ulong_t; goto
_LL179; _LL185: t= Cyc_Absyn_ushort_t; goto _LL179; _LL187: t= Cyc_Absyn_uchar_t;
goto _LL179; _LL189: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp213)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp218= _tag_arr("length modifier '%s' is not allowed with %%%c",
sizeof( unsigned char), 46u); struct Cyc_Stdio_String_pa_struct _temp220;
_temp220.tag= Cyc_Stdio_String_pa; _temp220.f1=( struct _tagged_arr) Cyc_String_implode(
_temp64);{ void* _temp216=( void*)& _temp220; struct Cyc_Stdio_Int_pa_struct
_temp219; _temp219.tag= Cyc_Stdio_Int_pa; _temp219.f1=( int)(( unsigned int)((
int) _temp62));{ void* _temp217=( void*)& _temp219; void* _temp214[ 2u]={
_temp216, _temp217}; struct _tagged_arr _temp215={( void*) _temp214,( void*)
_temp214,( void*)( _temp214 + 2u)}; _temp213( _temp218, _temp215);}}})); return
0; _LL179:;} _temp55=({ struct Cyc_List_List* _temp221=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp221->hd=( void*) t; _temp221->tl=
_temp55; _temp221;}); break; case 'f': _LL177: goto _LL222; case 'F': _LL222:
goto _LL223; case 'e': _LL223: goto _LL224; case 'E': _LL224: goto _LL225; case
'g': _LL225: goto _LL226; case 'G': _LL226: goto _LL227; case 'a': _LL227: goto
_LL228; case 'A': _LL228: if( _temp64 != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp230)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp234= _tag_arr("length modifiers are not allowed with %%%c",
sizeof( unsigned char), 43u); struct Cyc_Stdio_Int_pa_struct _temp235; _temp235.tag=
Cyc_Stdio_Int_pa; _temp235.f1=( int)(( unsigned int)(( int) _temp62));{ void*
_temp233=( void*)& _temp235; void* _temp231[ 1u]={ _temp233}; struct _tagged_arr
_temp232={( void*) _temp231,( void*) _temp231,( void*)( _temp231 + 1u)};
_temp230( _temp234, _temp232);}})); return 0;} _temp55=({ struct Cyc_List_List*
_temp236=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp236->hd=( void*) Cyc_Absyn_double_t; _temp236->tl= _temp55; _temp236;});
break; case 'c': _LL229:{ struct Cyc_List_List* f= _temp70; for( 0; f != 0; f=((
struct Cyc_List_List*) _check_null( f))->tl){ if(( int)(( struct Cyc_List_List*)
_check_null( f))->hd =='#'? 1:( int)(( struct Cyc_List_List*) _check_null( f))->hd
=='0'){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp238)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp242= _tag_arr("flag '%c' not allowed with %%c", sizeof(
unsigned char), 31u); struct Cyc_Stdio_Int_pa_struct _temp243; _temp243.tag= Cyc_Stdio_Int_pa;
_temp243.f1=( int)(( unsigned int)(( int)(( struct Cyc_List_List*) _check_null(
f))->hd));{ void* _temp241=( void*)& _temp243; void* _temp239[ 1u]={ _temp241};
struct _tagged_arr _temp240={( void*) _temp239,( void*) _temp239,( void*)(
_temp239 + 1u)}; _temp238( _temp242, _temp240);}})); return 0;}}} if( _temp64 !=
0){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp244)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp248= _tag_arr("length modifier '%s' not allowed with %%c",
sizeof( unsigned char), 42u); struct Cyc_Stdio_String_pa_struct _temp249;
_temp249.tag= Cyc_Stdio_String_pa; _temp249.f1=( struct _tagged_arr) Cyc_String_implode(
_temp64);{ void* _temp247=( void*)& _temp249; void* _temp245[ 1u]={ _temp247};
struct _tagged_arr _temp246={( void*) _temp245,( void*) _temp245,( void*)(
_temp245 + 1u)}; _temp244( _temp248, _temp246);}})); return 0;} if( _temp66 != 0){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp250)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp254= _tag_arr("precision '%s' not allowed with %%c", sizeof(
unsigned char), 36u); struct Cyc_Stdio_String_pa_struct _temp255; _temp255.tag=
Cyc_Stdio_String_pa; _temp255.f1=( struct _tagged_arr) Cyc_String_implode(
_temp66);{ void* _temp253=( void*)& _temp255; void* _temp251[ 1u]={ _temp253};
struct _tagged_arr _temp252={( void*) _temp251,( void*) _temp251,( void*)(
_temp251 + 1u)}; _temp250( _temp254, _temp252);}})); return 0;} _temp55=({
struct Cyc_List_List* _temp256=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp256->hd=( void*) Cyc_Absyn_sint_t; _temp256->tl=
_temp55; _temp256;}); break; case 's': _LL237: if( _temp70 != 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp258)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp262=
_tag_arr("flags '%s' not allowed with %%s", sizeof( unsigned char), 32u); struct
Cyc_Stdio_String_pa_struct _temp263; _temp263.tag= Cyc_Stdio_String_pa; _temp263.f1=(
struct _tagged_arr) Cyc_String_implode( _temp70);{ void* _temp261=( void*)&
_temp263; void* _temp259[ 1u]={ _temp261}; struct _tagged_arr _temp260={( void*)
_temp259,( void*) _temp259,( void*)( _temp259 + 1u)}; _temp258( _temp262,
_temp260);}})); return 0;} if( _temp68 != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp264)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp268= _tag_arr("width '%s' not allowed with %%s",
sizeof( unsigned char), 32u); struct Cyc_Stdio_String_pa_struct _temp269;
_temp269.tag= Cyc_Stdio_String_pa; _temp269.f1=( struct _tagged_arr) Cyc_String_implode(
_temp68);{ void* _temp267=( void*)& _temp269; void* _temp265[ 1u]={ _temp267};
struct _tagged_arr _temp266={( void*) _temp265,( void*) _temp265,( void*)(
_temp265 + 1u)}; _temp264( _temp268, _temp266);}})); return 0;} if( _temp66 != 0){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp270)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp274= _tag_arr("precision '%s' not allowed with %%s", sizeof(
unsigned char), 36u); struct Cyc_Stdio_String_pa_struct _temp275; _temp275.tag=
Cyc_Stdio_String_pa; _temp275.f1=( struct _tagged_arr) Cyc_String_implode(
_temp66);{ void* _temp273=( void*)& _temp275; void* _temp271[ 1u]={ _temp273};
struct _tagged_arr _temp272={( void*) _temp271,( void*) _temp271,( void*)(
_temp271 + 1u)}; _temp270( _temp274, _temp272);}})); return 0;} if( _temp64 != 0){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp276)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp280= _tag_arr("length modifier '%s' not allowed with %%s",
sizeof( unsigned char), 42u); struct Cyc_Stdio_String_pa_struct _temp281;
_temp281.tag= Cyc_Stdio_String_pa; _temp281.f1=( struct _tagged_arr) Cyc_String_implode(
_temp64);{ void* _temp279=( void*)& _temp281; void* _temp277[ 1u]={ _temp279};
struct _tagged_arr _temp278={( void*) _temp277,( void*) _temp277,( void*)(
_temp277 + 1u)}; _temp276( _temp280, _temp278);}})); return 0;} _temp55=({
struct Cyc_List_List* _temp282=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp282->hd=( void*) Cyc_Absyn_const_string_typ( Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,({ struct Cyc_Core_Opt* _temp283=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp283->v=(
void*) Cyc_Tcenv_lookup_type_vars( te); _temp283;}))); _temp282->tl= _temp55;
_temp282;}); break; case 'p': _LL257: Cyc_Tcutil_terr( loc, _tag_arr("%%p not supported",
sizeof( unsigned char), 18u)); return 0; case 'n': _LL284:{ struct Cyc_List_List*
f= _temp70; for( 0; f != 0; f=(( struct Cyc_List_List*) _check_null( f))->tl){
if(( int)(( struct Cyc_List_List*) _check_null( f))->hd =='#'? 1:( int)(( struct
Cyc_List_List*) _check_null( f))->hd =='0'){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp286)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp290= _tag_arr("flag '%c' not allowed with %%n",
sizeof( unsigned char), 31u); struct Cyc_Stdio_Int_pa_struct _temp291; _temp291.tag=
Cyc_Stdio_Int_pa; _temp291.f1=( int)(( unsigned int)(( int)(( struct Cyc_List_List*)
_check_null( f))->hd));{ void* _temp289=( void*)& _temp291; void* _temp287[ 1u]={
_temp289}; struct _tagged_arr _temp288={( void*) _temp287,( void*) _temp287,(
void*)( _temp287 + 1u)}; _temp286( _temp290, _temp288);}})); return 0;}}} if(
_temp66 != 0){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp292)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp296= _tag_arr("precision '%s' not allowed with %%n",
sizeof( unsigned char), 36u); struct Cyc_Stdio_String_pa_struct _temp297;
_temp297.tag= Cyc_Stdio_String_pa; _temp297.f1=( struct _tagged_arr) Cyc_String_implode(
_temp66);{ void* _temp295=( void*)& _temp297; void* _temp293[ 1u]={ _temp295};
struct _tagged_arr _temp294={( void*) _temp293,( void*) _temp293,( void*)(
_temp293 + 1u)}; _temp292( _temp296, _temp294);}})); return 0;}{ struct Cyc_Core_Opt*
_temp298=({ struct Cyc_Core_Opt* _temp341=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp341->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp341;});{ struct Cyc_List_List* _temp299= _temp64; struct Cyc_List_List
_temp312; struct Cyc_List_List* _temp313; int _temp315; struct Cyc_List_List
_temp318; struct Cyc_List_List* _temp319; int _temp321; struct Cyc_List_List
_temp324; struct Cyc_List_List* _temp325; struct Cyc_List_List _temp327; struct
Cyc_List_List* _temp328; int _temp330; int _temp332; _LL301: if( _temp299 == 0){
goto _LL302;} else{ goto _LL303;} _LL303: if( _temp299 == 0){ goto _LL305;}
else{ _temp312=* _temp299; _LL316: _temp315=( int) _temp312.hd; goto _LL314;
_LL314: _temp313= _temp312.tl; if( _temp313 == 0){ goto _LL311;} else{ goto
_LL305;}} _LL311: if( _temp315 =='l'){ goto _LL304;} else{ goto _LL305;} _LL305:
if( _temp299 == 0){ goto _LL307;} else{ _temp318=* _temp299; _LL322: _temp321=(
int) _temp318.hd; goto _LL320; _LL320: _temp319= _temp318.tl; if( _temp319 == 0){
goto _LL317;} else{ goto _LL307;}} _LL317: if( _temp321 =='h'){ goto _LL306;}
else{ goto _LL307;} _LL307: if( _temp299 == 0){ goto _LL309;} else{ _temp324=*
_temp299; _LL333: _temp332=( int) _temp324.hd; goto _LL326; _LL326: _temp325=
_temp324.tl; if( _temp325 == 0){ goto _LL309;} else{ _temp327=* _temp325; _LL331:
_temp330=( int) _temp327.hd; goto _LL329; _LL329: _temp328= _temp327.tl; if(
_temp328 == 0){ goto _LL323;} else{ goto _LL309;}}} _LL323: if( _temp332 =='h'?
_temp330 =='h': 0){ goto _LL308;} else{ goto _LL309;} _LL309: goto _LL310;
_LL302: t= Cyc_Absyn_at_typ( Cyc_Absyn_sint_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp298), Cyc_Absyn_empty_tqual()); goto _LL300; _LL304: t=
Cyc_Absyn_at_typ( Cyc_Absyn_ulong_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp298), Cyc_Absyn_empty_tqual()); goto _LL300; _LL306: t=
Cyc_Absyn_at_typ( Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp298), Cyc_Absyn_empty_tqual()); goto _LL300; _LL308: t=
Cyc_Absyn_at_typ( Cyc_Absyn_schar_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp298), Cyc_Absyn_empty_tqual()); goto _LL300; _LL310: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp334)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp338=
_tag_arr("length modifier '%s' not allowed with %%n", sizeof( unsigned char), 42u);
struct Cyc_Stdio_String_pa_struct _temp339; _temp339.tag= Cyc_Stdio_String_pa;
_temp339.f1=( struct _tagged_arr) Cyc_String_implode( _temp64);{ void* _temp337=(
void*)& _temp339; void* _temp335[ 1u]={ _temp337}; struct _tagged_arr _temp336={(
void*) _temp335,( void*) _temp335,( void*)( _temp335 + 1u)}; _temp334( _temp338,
_temp336);}})); return 0; _LL300:;} _temp55=({ struct Cyc_List_List* _temp340=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp340->hd=(
void*) t; _temp340->tl= _temp55; _temp340;}); break;} case '%': _LL285: if(
_temp70 != 0){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp343)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp347= _tag_arr("flags '%s' not allowed with %%%%",
sizeof( unsigned char), 33u); struct Cyc_Stdio_String_pa_struct _temp348;
_temp348.tag= Cyc_Stdio_String_pa; _temp348.f1=( struct _tagged_arr) Cyc_String_implode(
_temp70);{ void* _temp346=( void*)& _temp348; void* _temp344[ 1u]={ _temp346};
struct _tagged_arr _temp345={( void*) _temp344,( void*) _temp344,( void*)(
_temp344 + 1u)}; _temp343( _temp347, _temp345);}})); return 0;} if( _temp68 != 0){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp349)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp353= _tag_arr("width '%s' not allowed with %%%%", sizeof(
unsigned char), 33u); struct Cyc_Stdio_String_pa_struct _temp354; _temp354.tag=
Cyc_Stdio_String_pa; _temp354.f1=( struct _tagged_arr) Cyc_String_implode(
_temp68);{ void* _temp352=( void*)& _temp354; void* _temp350[ 1u]={ _temp352};
struct _tagged_arr _temp351={( void*) _temp350,( void*) _temp350,( void*)(
_temp350 + 1u)}; _temp349( _temp353, _temp351);}})); return 0;} if( _temp66 != 0){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp355)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp359= _tag_arr("precision '%s' not allowed with %%%%", sizeof(
unsigned char), 37u); struct Cyc_Stdio_String_pa_struct _temp360; _temp360.tag=
Cyc_Stdio_String_pa; _temp360.f1=( struct _tagged_arr) Cyc_String_implode(
_temp66);{ void* _temp358=( void*)& _temp360; void* _temp356[ 1u]={ _temp358};
struct _tagged_arr _temp357={( void*) _temp356,( void*) _temp356,( void*)(
_temp356 + 1u)}; _temp355( _temp359, _temp357);}})); return 0;} if( _temp64 != 0){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp361)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp365= _tag_arr("length modifier '%s' not allowed with %%%%",
sizeof( unsigned char), 43u); struct Cyc_Stdio_String_pa_struct _temp366;
_temp366.tag= Cyc_Stdio_String_pa; _temp366.f1=( struct _tagged_arr) Cyc_String_implode(
_temp64);{ void* _temp364=( void*)& _temp366; void* _temp362[ 1u]={ _temp364};
struct _tagged_arr _temp363={( void*) _temp362,( void*) _temp362,( void*)(
_temp362 + 1u)}; _temp361( _temp365, _temp363);}})); return 0;} break; default:
_LL342: return 0;}}}}} return Cyc_List_imp_rev( _temp55);} struct _tuple4{ int
f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; unsigned char f4; int f5;
} ; struct Cyc_Core_Opt* Cyc_Formatstr_parse_inputformat( struct _tagged_arr s,
int i){ int _temp368= Cyc_String_strlen( s); if( i < 0? 1: i >= _temp368){
return 0;}{ int _temp369= 0; unsigned char _temp370=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( _temp370 =='*'){
_temp369= 1; i ++; if( i >= _temp368){ return 0;}}{ struct Cyc_List_List*
_temp371= 0; for( 0; i < _temp368; i ++){ _temp370=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( isdigit(( int)
_temp370)){ _temp371=({ struct Cyc_List_List* _temp372=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp372->hd=( void*)(( int) _temp370);
_temp372->tl= _temp371; _temp372;});} else{ break;}} if( i >= _temp368){ return
0;} _temp371=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp371);{ struct Cyc_List_List* _temp373= 0; _temp370=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); switch( _temp370){
case 'h': _LL374: _temp373=({ struct Cyc_List_List* _temp376=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp376->hd=( void*)(( int) _temp370);
_temp376->tl= _temp373; _temp376;}); i ++; if( i >= _temp368){ return 0;}
_temp370=*(( const unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), i)); if( _temp370 =='h'){ _temp373=({ struct Cyc_List_List*
_temp377=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp377->hd=( void*)(( int) _temp370); _temp377->tl= _temp373; _temp377;}); i
++;} break; case 'l': _LL375: _temp373=({ struct Cyc_List_List* _temp379=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp379->hd=(
void*)(( int) _temp370); _temp379->tl= _temp373; _temp379;}); i ++; if( i >=
_temp368){ return 0;} _temp370=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( _temp370 =='l'){
_temp373=({ struct Cyc_List_List* _temp380=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp380->hd=( void*)(( int) _temp370); _temp380->tl=
_temp373; _temp380;}); i ++;} break; case 'j': _LL378: goto _LL381; case 'z':
_LL381: goto _LL382; case 't': _LL382: goto _LL383; case 'L': _LL383: _temp373=({
struct Cyc_List_List* _temp385=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp385->hd=( void*)(( int) _temp370); _temp385->tl=
_temp373; _temp385;}); i ++; break; default: _LL384: break;} if( i >= _temp368){
return 0;} _temp373=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp373); _temp370=*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), i)); switch( _temp370){ case 'd': _LL387: break; case 'i':
_LL388: break; case 'o': _LL389: break; case 'u': _LL390: break; case 'x':
_LL391: break; case 'X': _LL392: break; case 'f': _LL393: break; case 'F':
_LL394: break; case 'e': _LL395: break; case 'E': _LL396: break; case 'g':
_LL397: break; case 'G': _LL398: break; case 'a': _LL399: break; case 'A':
_LL400: break; case 'c': _LL401: break; case 's': _LL402: break; case 'p':
_LL403: break; case 'n': _LL404: break; case '%': _LL405: break; default: _LL406:
return 0;} return({ struct Cyc_Core_Opt* _temp408=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp408->v=( void*)({ struct _tuple4*
_temp409=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp409->f1=
_temp369; _temp409->f2= _temp371; _temp409->f3= _temp373; _temp409->f4= _temp370;
_temp409->f5= i + 1; _temp409;}); _temp408;});}}}} struct Cyc_List_List* Cyc_Formatstr_get_scanf_typs(
struct Cyc_Tcenv_Tenv* te, struct _tagged_arr s, struct Cyc_Position_Segment*
loc){ int _temp410= Cyc_String_strlen( s); struct Cyc_List_List* _temp411= 0;
int i; for( i= 0; i < _temp410; i ++){ if(*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)) !='%'){ continue;}{
struct Cyc_Core_Opt* _temp412= Cyc_Formatstr_parse_inputformat( s, i + 1); if(
_temp412 == 0){ Cyc_Tcutil_terr( loc, _tag_arr("bad format string", sizeof(
unsigned char), 18u)); return 0;}{ struct _tuple4 _temp415; int _temp416;
unsigned char _temp418; struct Cyc_List_List* _temp420; struct Cyc_List_List*
_temp422; int _temp424; struct _tuple4* _temp413=( struct _tuple4*)(( struct Cyc_Core_Opt*)
_check_null( _temp412))->v; _temp415=* _temp413; _LL425: _temp424= _temp415.f1;
goto _LL423; _LL423: _temp422= _temp415.f2; goto _LL421; _LL421: _temp420=
_temp415.f3; goto _LL419; _LL419: _temp418= _temp415.f4; goto _LL417; _LL417:
_temp416= _temp415.f5; goto _LL414; _LL414: i= _temp416 - 1;{ struct Cyc_List_List*
_temp426= _temp420; struct Cyc_List_List _temp433; struct Cyc_List_List*
_temp434; int _temp436; _LL428: if( _temp426 == 0){ goto _LL430;} else{ _temp433=*
_temp426; _LL437: _temp436=( int) _temp433.hd; goto _LL435; _LL435: _temp434=
_temp433.tl; if( _temp434 == 0){ goto _LL432;} else{ goto _LL430;}} _LL432: if((
_temp436 =='j'? 1: _temp436 =='z')? 1: _temp436 =='t'){ goto _LL429;} else{ goto
_LL430;} _LL430: goto _LL431; _LL429: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr(* _temp438)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp442= _tag_arr("length modifier '%c' is not supported",
sizeof( unsigned char), 38u); struct Cyc_Stdio_Int_pa_struct _temp443; _temp443.tag=
Cyc_Stdio_Int_pa; _temp443.f1=( int)(( unsigned int) _temp436);{ void* _temp441=(
void*)& _temp443; void* _temp439[ 1u]={ _temp441}; struct _tagged_arr _temp440={(
void*) _temp439,( void*) _temp439,( void*)( _temp439 + 1u)}; _temp438( _temp442,
_temp440);}})); return 0; _LL431: goto _LL427; _LL427:;} if( _temp424){
continue;}{ void* t; switch( _temp418){ case 'd': _LL444: goto _LL445; case 'i':
_LL445: { struct Cyc_Core_Opt* _temp447=({ struct Cyc_Core_Opt* _temp492=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp492->v=(
void*) Cyc_Tcenv_lookup_type_vars( te); _temp492;});{ struct Cyc_List_List*
_temp448= _temp420; struct Cyc_List_List _temp461; struct Cyc_List_List*
_temp462; int _temp464; struct Cyc_List_List _temp467; struct Cyc_List_List*
_temp468; int _temp470; struct Cyc_List_List _temp473; struct Cyc_List_List*
_temp474; struct Cyc_List_List _temp476; struct Cyc_List_List* _temp477; int
_temp479; int _temp481; _LL450: if( _temp448 == 0){ goto _LL451;} else{ goto
_LL452;} _LL452: if( _temp448 == 0){ goto _LL454;} else{ _temp461=* _temp448;
_LL465: _temp464=( int) _temp461.hd; goto _LL463; _LL463: _temp462= _temp461.tl;
if( _temp462 == 0){ goto _LL460;} else{ goto _LL454;}} _LL460: if( _temp464 =='l'){
goto _LL453;} else{ goto _LL454;} _LL454: if( _temp448 == 0){ goto _LL456;}
else{ _temp467=* _temp448; _LL471: _temp470=( int) _temp467.hd; goto _LL469;
_LL469: _temp468= _temp467.tl; if( _temp468 == 0){ goto _LL466;} else{ goto
_LL456;}} _LL466: if( _temp470 =='h'){ goto _LL455;} else{ goto _LL456;} _LL456:
if( _temp448 == 0){ goto _LL458;} else{ _temp473=* _temp448; _LL482: _temp481=(
int) _temp473.hd; goto _LL475; _LL475: _temp474= _temp473.tl; if( _temp474 == 0){
goto _LL458;} else{ _temp476=* _temp474; _LL480: _temp479=( int) _temp476.hd;
goto _LL478; _LL478: _temp477= _temp476.tl; if( _temp477 == 0){ goto _LL472;}
else{ goto _LL458;}}} _LL472: if( _temp481 =='h'? _temp479 =='h': 0){ goto
_LL457;} else{ goto _LL458;} _LL458: goto _LL459; _LL451: t= Cyc_Absyn_at_typ(
Cyc_Absyn_sint_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp447), Cyc_Absyn_empty_tqual()); goto _LL449; _LL453: t= Cyc_Absyn_at_typ(
Cyc_Absyn_slong_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp447), Cyc_Absyn_empty_tqual()); goto _LL449; _LL455: t= Cyc_Absyn_at_typ(
Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp447), Cyc_Absyn_empty_tqual()); goto _LL449; _LL457: t= Cyc_Absyn_at_typ(
Cyc_Absyn_schar_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp447), Cyc_Absyn_empty_tqual()); goto _LL449; _LL459: Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp483)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp488= _tag_arr("length modifier '%s' is not allowed with %%%c",
sizeof( unsigned char), 46u); struct Cyc_Stdio_String_pa_struct _temp490;
_temp490.tag= Cyc_Stdio_String_pa; _temp490.f1=( struct _tagged_arr) Cyc_String_implode(
_temp420);{ void* _temp486=( void*)& _temp490; struct Cyc_Stdio_Int_pa_struct
_temp489; _temp489.tag= Cyc_Stdio_Int_pa; _temp489.f1=( int)(( unsigned int)((
int) _temp418));{ void* _temp487=( void*)& _temp489; void* _temp484[ 2u]={
_temp486, _temp487}; struct _tagged_arr _temp485={( void*) _temp484,( void*)
_temp484,( void*)( _temp484 + 2u)}; _temp483( _temp488, _temp485);}}})); return
0; _LL449:;} _temp411=({ struct Cyc_List_List* _temp491=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp491->hd=( void*) t; _temp491->tl=
_temp411; _temp491;}); break;} case 'u': _LL446: goto _LL493; case 'o': _LL493:
goto _LL494; case 'x': _LL494: goto _LL495; case 'X': _LL495: { struct Cyc_Core_Opt*
_temp497=({ struct Cyc_Core_Opt* _temp542=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp542->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp542;});{ struct Cyc_List_List* _temp498= _temp420; struct Cyc_List_List
_temp511; struct Cyc_List_List* _temp512; int _temp514; struct Cyc_List_List
_temp517; struct Cyc_List_List* _temp518; int _temp520; struct Cyc_List_List
_temp523; struct Cyc_List_List* _temp524; struct Cyc_List_List _temp526; struct
Cyc_List_List* _temp527; int _temp529; int _temp531; _LL500: if( _temp498 == 0){
goto _LL501;} else{ goto _LL502;} _LL502: if( _temp498 == 0){ goto _LL504;}
else{ _temp511=* _temp498; _LL515: _temp514=( int) _temp511.hd; goto _LL513;
_LL513: _temp512= _temp511.tl; if( _temp512 == 0){ goto _LL510;} else{ goto
_LL504;}} _LL510: if( _temp514 =='l'){ goto _LL503;} else{ goto _LL504;} _LL504:
if( _temp498 == 0){ goto _LL506;} else{ _temp517=* _temp498; _LL521: _temp520=(
int) _temp517.hd; goto _LL519; _LL519: _temp518= _temp517.tl; if( _temp518 == 0){
goto _LL516;} else{ goto _LL506;}} _LL516: if( _temp520 =='h'){ goto _LL505;}
else{ goto _LL506;} _LL506: if( _temp498 == 0){ goto _LL508;} else{ _temp523=*
_temp498; _LL532: _temp531=( int) _temp523.hd; goto _LL525; _LL525: _temp524=
_temp523.tl; if( _temp524 == 0){ goto _LL508;} else{ _temp526=* _temp524; _LL530:
_temp529=( int) _temp526.hd; goto _LL528; _LL528: _temp527= _temp526.tl; if(
_temp527 == 0){ goto _LL522;} else{ goto _LL508;}}} _LL522: if( _temp531 =='h'?
_temp529 =='h': 0){ goto _LL507;} else{ goto _LL508;} _LL508: goto _LL509;
_LL501: t= Cyc_Absyn_at_typ( Cyc_Absyn_uint_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp497), Cyc_Absyn_empty_tqual()); goto _LL499; _LL503: t=
Cyc_Absyn_at_typ( Cyc_Absyn_ulong_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp497), Cyc_Absyn_empty_tqual()); goto _LL499; _LL505: t=
Cyc_Absyn_at_typ( Cyc_Absyn_ushort_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp497), Cyc_Absyn_empty_tqual()); goto _LL499; _LL507: t=
Cyc_Absyn_at_typ( Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp497), Cyc_Absyn_empty_tqual()); goto _LL499; _LL509: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp533)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp538=
_tag_arr("length modifier '%s' is not allowed with %%%c", sizeof( unsigned char),
46u); struct Cyc_Stdio_String_pa_struct _temp540; _temp540.tag= Cyc_Stdio_String_pa;
_temp540.f1=( struct _tagged_arr) Cyc_String_implode( _temp420);{ void* _temp536=(
void*)& _temp540; struct Cyc_Stdio_Int_pa_struct _temp539; _temp539.tag= Cyc_Stdio_Int_pa;
_temp539.f1=( int)(( unsigned int)(( int) _temp418));{ void* _temp537=( void*)&
_temp539; void* _temp534[ 2u]={ _temp536, _temp537}; struct _tagged_arr _temp535={(
void*) _temp534,( void*) _temp534,( void*)( _temp534 + 2u)}; _temp533( _temp538,
_temp535);}}})); return 0; _LL499:;} _temp411=({ struct Cyc_List_List* _temp541=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp541->hd=(
void*) t; _temp541->tl= _temp411; _temp541;}); break;} case 'f': _LL496: goto
_LL543; case 'F': _LL543: goto _LL544; case 'e': _LL544: goto _LL545; case 'E':
_LL545: goto _LL546; case 'g': _LL546: goto _LL547; case 'G': _LL547: goto
_LL548; case 'a': _LL548: goto _LL549; case 'A': _LL549: { struct Cyc_Core_Opt*
_temp551=({ struct Cyc_Core_Opt* _temp575=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp575->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp575;});{ struct Cyc_List_List* _temp552= _temp420; struct Cyc_List_List
_temp561; struct Cyc_List_List* _temp562; int _temp564; _LL554: if( _temp552 ==
0){ goto _LL555;} else{ goto _LL556;} _LL556: if( _temp552 == 0){ goto _LL558;}
else{ _temp561=* _temp552; _LL565: _temp564=( int) _temp561.hd; goto _LL563;
_LL563: _temp562= _temp561.tl; if( _temp562 == 0){ goto _LL560;} else{ goto
_LL558;}} _LL560: if( _temp564 =='l'){ goto _LL557;} else{ goto _LL558;} _LL558:
goto _LL559; _LL555: t= Cyc_Absyn_at_typ( Cyc_Absyn_float_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp551), Cyc_Absyn_empty_tqual());
goto _LL553; _LL557: t= Cyc_Absyn_at_typ( Cyc_Absyn_double_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp551), Cyc_Absyn_empty_tqual());
goto _LL553; _LL559: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp566)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp571= _tag_arr("length modifier '%s' is not allowed with %%%c",
sizeof( unsigned char), 46u); struct Cyc_Stdio_String_pa_struct _temp573;
_temp573.tag= Cyc_Stdio_String_pa; _temp573.f1=( struct _tagged_arr) Cyc_String_implode(
_temp420);{ void* _temp569=( void*)& _temp573; struct Cyc_Stdio_Int_pa_struct
_temp572; _temp572.tag= Cyc_Stdio_Int_pa; _temp572.f1=( int)(( unsigned int)((
int) _temp418));{ void* _temp570=( void*)& _temp572; void* _temp567[ 2u]={
_temp569, _temp570}; struct _tagged_arr _temp568={( void*) _temp567,( void*)
_temp567,( void*)( _temp567 + 2u)}; _temp566( _temp571, _temp568);}}})); return
0; _LL553:;} _temp411=({ struct Cyc_List_List* _temp574=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp574->hd=( void*) t; _temp574->tl=
_temp411; _temp574;}); break;} case 'c': _LL550: Cyc_Tcutil_terr( loc, _tag_arr("%%c is not supported",
sizeof( unsigned char), 21u)); return 0; case 's': _LL576: Cyc_Tcutil_terr( loc,
_tag_arr("%%s is not supported", sizeof( unsigned char), 21u)); return 0; case '[':
_LL577: Cyc_Tcutil_terr( loc, _tag_arr("%%[ is not supported", sizeof(
unsigned char), 21u)); return 0; case 'p': _LL578: Cyc_Tcutil_terr( loc,
_tag_arr("%%p is not supported", sizeof( unsigned char), 21u)); return 0; case 'n':
_LL579: { struct Cyc_Core_Opt* _temp581=({ struct Cyc_Core_Opt* _temp624=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp624->v=(
void*) Cyc_Tcenv_lookup_type_vars( te); _temp624;});{ struct Cyc_List_List*
_temp582= _temp420; struct Cyc_List_List _temp595; struct Cyc_List_List*
_temp596; int _temp598; struct Cyc_List_List _temp601; struct Cyc_List_List*
_temp602; int _temp604; struct Cyc_List_List _temp607; struct Cyc_List_List*
_temp608; struct Cyc_List_List _temp610; struct Cyc_List_List* _temp611; int
_temp613; int _temp615; _LL584: if( _temp582 == 0){ goto _LL585;} else{ goto
_LL586;} _LL586: if( _temp582 == 0){ goto _LL588;} else{ _temp595=* _temp582;
_LL599: _temp598=( int) _temp595.hd; goto _LL597; _LL597: _temp596= _temp595.tl;
if( _temp596 == 0){ goto _LL594;} else{ goto _LL588;}} _LL594: if( _temp598 =='l'){
goto _LL587;} else{ goto _LL588;} _LL588: if( _temp582 == 0){ goto _LL590;}
else{ _temp601=* _temp582; _LL605: _temp604=( int) _temp601.hd; goto _LL603;
_LL603: _temp602= _temp601.tl; if( _temp602 == 0){ goto _LL600;} else{ goto
_LL590;}} _LL600: if( _temp604 =='h'){ goto _LL589;} else{ goto _LL590;} _LL590:
if( _temp582 == 0){ goto _LL592;} else{ _temp607=* _temp582; _LL616: _temp615=(
int) _temp607.hd; goto _LL609; _LL609: _temp608= _temp607.tl; if( _temp608 == 0){
goto _LL592;} else{ _temp610=* _temp608; _LL614: _temp613=( int) _temp610.hd;
goto _LL612; _LL612: _temp611= _temp610.tl; if( _temp611 == 0){ goto _LL606;}
else{ goto _LL592;}}} _LL606: if( _temp615 =='h'? _temp613 =='h': 0){ goto
_LL591;} else{ goto _LL592;} _LL592: goto _LL593; _LL585: t= Cyc_Absyn_at_typ(
Cyc_Absyn_sint_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp581), Cyc_Absyn_empty_tqual()); goto _LL583; _LL587: t= Cyc_Absyn_at_typ(
Cyc_Absyn_ulong_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp581), Cyc_Absyn_empty_tqual()); goto _LL583; _LL589: t= Cyc_Absyn_at_typ(
Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp581), Cyc_Absyn_empty_tqual()); goto _LL583; _LL591: t= Cyc_Absyn_at_typ(
Cyc_Absyn_schar_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp581), Cyc_Absyn_empty_tqual()); goto _LL583; _LL593: Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp617)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp621= _tag_arr("length modifier '%s' not allowed with %%n",
sizeof( unsigned char), 42u); struct Cyc_Stdio_String_pa_struct _temp622;
_temp622.tag= Cyc_Stdio_String_pa; _temp622.f1=( struct _tagged_arr) Cyc_String_implode(
_temp420);{ void* _temp620=( void*)& _temp622; void* _temp618[ 1u]={ _temp620};
struct _tagged_arr _temp619={( void*) _temp618,( void*) _temp618,( void*)(
_temp618 + 1u)}; _temp617( _temp621, _temp619);}})); return 0; _LL583:;}
_temp411=({ struct Cyc_List_List* _temp623=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp623->hd=( void*) t; _temp623->tl= _temp411;
_temp623;}); break;} case '%': _LL580: if( _temp424){ Cyc_Tcutil_terr( loc,
_tag_arr("Assignment suppression (*) is not allowed with %%%%", sizeof(
unsigned char), 52u)); return 0;} if( _temp422 != 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp626)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp630= _tag_arr("width '%s' not allowed with %%%%",
sizeof( unsigned char), 33u); struct Cyc_Stdio_String_pa_struct _temp631;
_temp631.tag= Cyc_Stdio_String_pa; _temp631.f1=( struct _tagged_arr) Cyc_String_implode(
_temp422);{ void* _temp629=( void*)& _temp631; void* _temp627[ 1u]={ _temp629};
struct _tagged_arr _temp628={( void*) _temp627,( void*) _temp627,( void*)(
_temp627 + 1u)}; _temp626( _temp630, _temp628);}})); return 0;} if( _temp420 !=
0){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp632)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp636= _tag_arr("length modifier '%s' not allowed with %%%%",
sizeof( unsigned char), 43u); struct Cyc_Stdio_String_pa_struct _temp637;
_temp637.tag= Cyc_Stdio_String_pa; _temp637.f1=( struct _tagged_arr) Cyc_String_implode(
_temp420);{ void* _temp635=( void*)& _temp637; void* _temp633[ 1u]={ _temp635};
struct _tagged_arr _temp634={( void*) _temp633,( void*) _temp633,( void*)(
_temp633 + 1u)}; _temp632( _temp636, _temp634);}})); return 0;} break; default:
_LL625: return 0;}}}}} return Cyc_List_imp_rev( _temp411);}