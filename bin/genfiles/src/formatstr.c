 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_string); extern unsigned char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( unsigned char*); struct _tagged_ptr0{ struct _tagged_string*
curr; struct _tagged_string* base; struct _tagged_string* last_plus_one; } ;
extern struct _tagged_ptr0 ntCsl_to_ntsl( unsigned char**); extern int system(
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
_tagged_string f1; } ; extern unsigned int Cyc_String_strlen( struct
_tagged_string s); extern struct _tagged_string Cyc_String_implode( struct Cyc_List_List*
c); struct Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct
_tagged_string line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static
const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct
Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; extern unsigned char Cyc_Position_Nocontext[
14u]; struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct Cyc_Absyn_Tvar;
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
0; struct Cyc_Absyn_Tvar{ struct _tagged_string* name; int* identity; struct Cyc_Absyn_Conref*
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
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Nocommon_att=
6; static const int Cyc_Absyn_Shared_att= 7; static const int Cyc_Absyn_Unused_att=
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
_tagged_string f1; } ; static const int Cyc_Absyn_String_c= 5; struct Cyc_Absyn_String_c_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Null_c= 0;
static const int Cyc_Absyn_Plus= 0; static const int Cyc_Absyn_Times= 1; static
const int Cyc_Absyn_Minus= 2; static const int Cyc_Absyn_Div= 3; static const
int Cyc_Absyn_Mod= 4; static const int Cyc_Absyn_Eq= 5; static const int Cyc_Absyn_Neq=
6; static const int Cyc_Absyn_Gt= 7; static const int Cyc_Absyn_Lt= 8; static
const int Cyc_Absyn_Gte= 9; static const int Cyc_Absyn_Lte= 10; static const int
Cyc_Absyn_Not= 11; static const int Cyc_Absyn_Bitnot= 12; static const int Cyc_Absyn_Bitand=
13; static const int Cyc_Absyn_Bitor= 14; static const int Cyc_Absyn_Bitxor= 15;
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
int tag; void* f1; struct _tagged_string* f2; } ; static const int Cyc_Absyn_Deref_e=
19; struct Cyc_Absyn_Deref_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_StructMember_e= 20; struct Cyc_Absyn_StructMember_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tagged_string* f2; } ; static const
int Cyc_Absyn_StructArrow_e= 21; struct Cyc_Absyn_StructArrow_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct _tagged_string* f2; } ; static const int Cyc_Absyn_Subscript_e=
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
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt*
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
struct _tagged_string* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Do_s=
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
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Tuple_p= 4;
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
attributes; } ; struct Cyc_Absyn_Structfield{ struct _tagged_string* name;
struct Cyc_Absyn_Tqual tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List*
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
int tag; struct _tagged_string* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{ int tag; struct
_tuple0* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
11; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static
const int Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1;
struct Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ;
extern unsigned char Cyc_Absyn_EmptyAnnot[ 15u]; extern struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual(); extern void* Cyc_Absyn_new_evar( struct Cyc_Core_Opt* k,
struct Cyc_Core_Opt* tenv); extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_ushort_t;
extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulong_t; extern void* Cyc_Absyn_schar_t;
extern void* Cyc_Absyn_sshort_t; extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_slong_t;
extern void* Cyc_Absyn_float_t; extern void* Cyc_Absyn_double_t; extern void*
Cyc_Absyn_const_string_typ( void* rgn); extern void* Cyc_Absyn_at_typ( void* t,
void* rgn, struct Cyc_Absyn_Tqual tq); struct Cyc_Set_Set; extern unsigned char
Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[
12u]; extern unsigned char Cyc_Dict_Absent[ 11u]; static const int Cyc_Tcenv_VarRes=
0; struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes=
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
ae; struct Cyc_Core_Opt* le; } ; extern struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*); extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern
void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct _tagged_string);
extern struct Cyc_List_List* Cyc_Formatstr_get_format_typs( struct Cyc_Tcenv_Tenv*,
struct _tagged_string, struct Cyc_Position_Segment*); extern struct Cyc_List_List*
Cyc_Formatstr_get_scanf_typs( struct Cyc_Tcenv_Tenv*, struct _tagged_string,
struct Cyc_Position_Segment*); extern struct Cyc_Core_Opt* Cyc_Formatstr_parse_conversionspecification(
struct _tagged_string s, int i); static struct Cyc_Core_Opt Cyc_Formatstr_rk={(
void*)(( void*) 3u)}; struct _tuple3{ struct Cyc_List_List* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; struct Cyc_List_List* f4; unsigned char f5; int f6;
} ; struct Cyc_Core_Opt* Cyc_Formatstr_parse_conversionspecification( struct
_tagged_string s, int i){ unsigned int _temp0= Cyc_String_strlen( s); goto _LL1;
_LL1: if( i < 0? 1: i >= _temp0){ return 0;}{ struct Cyc_List_List* _temp2= 0;
goto _LL3; _LL3: { unsigned char c=' '; for( 0; i < _temp0; i ++){ c=*(( const
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char), i); switch( c){ case '+':
_LL4: goto _LL5; case '-': _LL5: goto _LL6; case ' ': _LL6: goto _LL7; case '#':
_LL7: goto _LL8; case '0': _LL8: _temp2=({ struct Cyc_List_List* _temp10=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp10->hd=(
void*)(( int) c); _temp10->tl= _temp2; _temp10;}); continue; default: _LL9:
break;} break;} if( i >= _temp0){ return 0;} _temp2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2);{ struct Cyc_List_List*
_temp12= 0; goto _LL13; _LL13: c=*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( unsigned char), i); if( c =='*'){ _temp12=({ struct Cyc_List_List*
_temp14=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp14->hd=( void*)(( int) c); _temp14->tl= _temp12; _temp14;}); i ++;} else{
for( 0; i < _temp0; i ++){ c=*(( const unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char),
i); if( isdigit(( int) c)){ _temp12=({ struct Cyc_List_List* _temp15=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp15->hd=( void*)((
int) c); _temp15->tl= _temp12; _temp15;});} else{ break;}}} if( i >= _temp0){
return 0;} _temp12=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp12);{ struct Cyc_List_List* _temp16= 0; goto _LL17; _LL17: c=*(( const
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char), i); if( c =='.'){ _temp16=({
struct Cyc_List_List* _temp18=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp18->hd=( void*)(( int) c); _temp18->tl= _temp16; _temp18;});
i ++; if( i >= _temp0){ return 0;} c=*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( unsigned char), i); if( c =='*'){ _temp16=({ struct Cyc_List_List*
_temp19=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp19->hd=( void*)(( int) c); _temp19->tl= _temp16; _temp19;}); i ++;} else{
for( 0; i < _temp0; i ++){ c=*(( const unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char),
i); if( isdigit(( int) c)){ _temp16=({ struct Cyc_List_List* _temp20=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp20->hd=( void*)((
int) c); _temp20->tl= _temp16; _temp20;});} else{ break;}}}} if( i >= _temp0){
return 0;} _temp16=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp16);{ struct Cyc_List_List* _temp21= 0; goto _LL22; _LL22: c=*(( const
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char), i); switch( c){ case 'h':
_LL23: _temp21=({ struct Cyc_List_List* _temp25=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp25->hd=( void*)(( int) c);
_temp25->tl= _temp21; _temp25;}); i ++; if( i >= _temp0){ return 0;} c=*(( const
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char), i); if( c =='h'){ _temp21=({
struct Cyc_List_List* _temp26=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp26->hd=( void*)(( int) c); _temp26->tl= _temp21; _temp26;});
i ++;} break; case 'l': _LL24: _temp21=({ struct Cyc_List_List* _temp28=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp28->hd=( void*)((
int) c); _temp28->tl= _temp21; _temp28;}); i ++; if( i >= _temp0){ return 0;} c=*((
const unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char), i); if( c =='l'){ _temp21=({
struct Cyc_List_List* _temp29=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp29->hd=( void*)(( int) c); _temp29->tl= _temp21; _temp29;});
i ++;} break; case 'j': _LL27: goto _LL30; case 'z': _LL30: goto _LL31; case 't':
_LL31: goto _LL32; case 'L': _LL32: _temp21=({ struct Cyc_List_List* _temp34=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp34->hd=(
void*)(( int) c); _temp34->tl= _temp21; _temp34;}); i ++; break; default: _LL33:
break;} if( i >= _temp0){ return 0;} _temp21=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( _temp21); c=*(( const unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( unsigned char), i); switch( c){ case 'd': _LL36: break; case 'i': _LL37:
break; case 'o': _LL38: break; case 'u': _LL39: break; case 'x': _LL40: break;
case 'X': _LL41: break; case 'f': _LL42: break; case 'F': _LL43: break; case 'e':
_LL44: break; case 'E': _LL45: break; case 'g': _LL46: break; case 'G': _LL47:
break; case 'a': _LL48: break; case 'A': _LL49: break; case 'c': _LL50: break;
case 's': _LL51: break; case 'p': _LL52: break; case 'n': _LL53: break; case '%':
_LL54: break; default: _LL55: return 0;} return({ struct Cyc_Core_Opt* _temp57=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp57->v=(
void*)({ struct _tuple3* _temp58=( struct _tuple3*) GC_malloc( sizeof( struct
_tuple3)); _temp58->f1= _temp2; _temp58->f2= _temp12; _temp58->f3= _temp16;
_temp58->f4= _temp21; _temp58->f5= c; _temp58->f6= i + 1; _temp58;}); _temp57;});}}}}}}
struct Cyc_List_List* Cyc_Formatstr_get_format_typs( struct Cyc_Tcenv_Tenv* te,
struct _tagged_string s, struct Cyc_Position_Segment* loc){ unsigned int _temp59=
Cyc_String_strlen( s); goto _LL60; _LL60: { struct Cyc_List_List* _temp61= 0;
goto _LL62; _LL62: { int i; for( i= 0; i < _temp59; i ++){ if(*(( const
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char), i) !='%'){ continue;}{
struct Cyc_Core_Opt* _temp63= Cyc_Formatstr_parse_conversionspecification( s, i
+ 1); goto _LL64; _LL64: if( _temp63 == 0){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp65=( unsigned char*)"bad format string"; struct
_tagged_string _temp66; _temp66.curr= _temp65; _temp66.base= _temp65; _temp66.last_plus_one=
_temp65 + 18; _temp66;})); return 0;}{ struct _tuple3 _temp69; int _temp70;
unsigned char _temp72; struct Cyc_List_List* _temp74; struct Cyc_List_List*
_temp76; struct Cyc_List_List* _temp78; struct Cyc_List_List* _temp80; struct
_tuple3* _temp67=( struct _tuple3*)(( struct Cyc_Core_Opt*) _check_null( _temp63))->v;
_temp69=* _temp67; _LL81: _temp80= _temp69.f1; goto _LL79; _LL79: _temp78=
_temp69.f2; goto _LL77; _LL77: _temp76= _temp69.f3; goto _LL75; _LL75: _temp74=
_temp69.f4; goto _LL73; _LL73: _temp72= _temp69.f5; goto _LL71; _LL71: _temp70=
_temp69.f6; goto _LL68; _LL68: i= _temp70 - 1;{ struct Cyc_List_List* _temp82=
_temp74; struct Cyc_List_List _temp89; struct Cyc_List_List* _temp90; int
_temp92; _LL84: if( _temp82 == 0){ goto _LL86;} else{ _temp89=* _temp82; _LL93:
_temp92=( int) _temp89.hd; goto _LL91; _LL91: _temp90= _temp89.tl; if( _temp90
== 0){ goto _LL88;} else{ goto _LL86;}} _LL88: if(( _temp92 =='j'? 1: _temp92 =='z')?
1: _temp92 =='t'){ goto _LL85;} else{ goto _LL86;} _LL86: goto _LL87; _LL85: Cyc_Tcutil_terr(
loc,( struct _tagged_string) xprintf("length modifier '%c' is not supported",
_temp92)); return 0; _LL87: goto _LL83; _LL83:;}{ struct Cyc_List_List* _temp94=
_temp78; struct Cyc_List_List _temp101; struct Cyc_List_List* _temp102; int
_temp104; _LL96: if( _temp94 == 0){ goto _LL98;} else{ _temp101=* _temp94;
_LL105: _temp104=( int) _temp101.hd; goto _LL103; _LL103: _temp102= _temp101.tl;
if( _temp102 == 0){ goto _LL100;} else{ goto _LL98;}} _LL100: if( _temp104 =='*'){
goto _LL97;} else{ goto _LL98;} _LL98: goto _LL99; _LL97: _temp61=({ struct Cyc_List_List*
_temp106=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp106->hd=( void*) Cyc_Absyn_sint_t; _temp106->tl= _temp61; _temp106;}); goto
_LL95; _LL99: goto _LL95; _LL95:;}{ struct Cyc_List_List* _temp107= _temp76;
struct Cyc_List_List _temp114; struct Cyc_List_List* _temp115; struct Cyc_List_List
_temp117; struct Cyc_List_List* _temp118; int _temp120; int _temp122; _LL109:
if( _temp107 == 0){ goto _LL111;} else{ _temp114=* _temp107; _LL123: _temp122=(
int) _temp114.hd; goto _LL116; _LL116: _temp115= _temp114.tl; if( _temp115 == 0){
goto _LL111;} else{ _temp117=* _temp115; _LL121: _temp120=( int) _temp117.hd;
goto _LL119; _LL119: _temp118= _temp117.tl; if( _temp118 == 0){ goto _LL113;}
else{ goto _LL111;}}} _LL113: if( _temp122 =='.'? _temp120 =='*': 0){ goto
_LL110;} else{ goto _LL111;} _LL111: goto _LL112; _LL110: _temp61=({ struct Cyc_List_List*
_temp124=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp124->hd=( void*) Cyc_Absyn_sint_t; _temp124->tl= _temp61; _temp124;}); goto
_LL108; _LL112: goto _LL108; _LL108:;}{ void* t; switch( _temp72){ case 'd':
_LL125: goto _LL126; case 'i': _LL126:{ struct Cyc_List_List* f= _temp80; for( 0;
f != 0; f=(( struct Cyc_List_List*) _check_null( f))->tl){ if(( int)(( struct
Cyc_List_List*) _check_null( f))->hd =='#'){ Cyc_Tcutil_terr( loc,( struct
_tagged_string) xprintf("flag '#' is not valid with %%%c",( int) _temp72));
return 0;}}}{ struct Cyc_List_List* _temp128= _temp74; struct Cyc_List_List
_temp141; struct Cyc_List_List* _temp142; int _temp144; struct Cyc_List_List
_temp147; struct Cyc_List_List* _temp148; int _temp150; struct Cyc_List_List
_temp153; struct Cyc_List_List* _temp154; struct Cyc_List_List _temp156; struct
Cyc_List_List* _temp157; int _temp159; int _temp161; _LL130: if( _temp128 == 0){
goto _LL131;} else{ goto _LL132;} _LL132: if( _temp128 == 0){ goto _LL134;}
else{ _temp141=* _temp128; _LL145: _temp144=( int) _temp141.hd; goto _LL143;
_LL143: _temp142= _temp141.tl; if( _temp142 == 0){ goto _LL140;} else{ goto
_LL134;}} _LL140: if( _temp144 =='l'){ goto _LL133;} else{ goto _LL134;} _LL134:
if( _temp128 == 0){ goto _LL136;} else{ _temp147=* _temp128; _LL151: _temp150=(
int) _temp147.hd; goto _LL149; _LL149: _temp148= _temp147.tl; if( _temp148 == 0){
goto _LL146;} else{ goto _LL136;}} _LL146: if( _temp150 =='h'){ goto _LL135;}
else{ goto _LL136;} _LL136: if( _temp128 == 0){ goto _LL138;} else{ _temp153=*
_temp128; _LL162: _temp161=( int) _temp153.hd; goto _LL155; _LL155: _temp154=
_temp153.tl; if( _temp154 == 0){ goto _LL138;} else{ _temp156=* _temp154; _LL160:
_temp159=( int) _temp156.hd; goto _LL158; _LL158: _temp157= _temp156.tl; if(
_temp157 == 0){ goto _LL152;} else{ goto _LL138;}}} _LL152: if( _temp161 =='h'?
_temp159 =='h': 0){ goto _LL137;} else{ goto _LL138;} _LL138: goto _LL139;
_LL131: t= Cyc_Absyn_sint_t; goto _LL129; _LL133: t= Cyc_Absyn_slong_t; goto
_LL129; _LL135: t= Cyc_Absyn_sshort_t; goto _LL129; _LL137: t= Cyc_Absyn_schar_t;
goto _LL129; _LL139: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp163=( struct _tagged_string) Cyc_String_implode( _temp74);
int _temp164=( int) _temp72; xprintf("length modifier '%.*s' is not allowed with %%%c",
_temp163.last_plus_one - _temp163.curr, _temp163.curr, _temp164);})); return 0;
_LL129:;} _temp61=({ struct Cyc_List_List* _temp165=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp165->hd=( void*) t; _temp165->tl=
_temp61; _temp165;}); break; case 'u': _LL127:{ struct Cyc_List_List* f= _temp80;
for( 0; f != 0; f=(( struct Cyc_List_List*) _check_null( f))->tl){ if(( int)((
struct Cyc_List_List*) _check_null( f))->hd =='#'){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp167=( unsigned char*)"Flag '#' not valid with %%u"; struct
_tagged_string _temp168; _temp168.curr= _temp167; _temp168.base= _temp167;
_temp168.last_plus_one= _temp167 + 28; _temp168;})); return 0;}}} goto _LL166;
case 'o': _LL166: goto _LL169; case 'x': _LL169: goto _LL170; case 'X': _LL170:{
struct Cyc_List_List* _temp172= _temp74; struct Cyc_List_List _temp185; struct
Cyc_List_List* _temp186; int _temp188; struct Cyc_List_List _temp191; struct Cyc_List_List*
_temp192; int _temp194; struct Cyc_List_List _temp197; struct Cyc_List_List*
_temp198; struct Cyc_List_List _temp200; struct Cyc_List_List* _temp201; int
_temp203; int _temp205; _LL174: if( _temp172 == 0){ goto _LL175;} else{ goto
_LL176;} _LL176: if( _temp172 == 0){ goto _LL178;} else{ _temp185=* _temp172;
_LL189: _temp188=( int) _temp185.hd; goto _LL187; _LL187: _temp186= _temp185.tl;
if( _temp186 == 0){ goto _LL184;} else{ goto _LL178;}} _LL184: if( _temp188 =='l'){
goto _LL177;} else{ goto _LL178;} _LL178: if( _temp172 == 0){ goto _LL180;}
else{ _temp191=* _temp172; _LL195: _temp194=( int) _temp191.hd; goto _LL193;
_LL193: _temp192= _temp191.tl; if( _temp192 == 0){ goto _LL190;} else{ goto
_LL180;}} _LL190: if( _temp194 =='h'){ goto _LL179;} else{ goto _LL180;} _LL180:
if( _temp172 == 0){ goto _LL182;} else{ _temp197=* _temp172; _LL206: _temp205=(
int) _temp197.hd; goto _LL199; _LL199: _temp198= _temp197.tl; if( _temp198 == 0){
goto _LL182;} else{ _temp200=* _temp198; _LL204: _temp203=( int) _temp200.hd;
goto _LL202; _LL202: _temp201= _temp200.tl; if( _temp201 == 0){ goto _LL196;}
else{ goto _LL182;}}} _LL196: if( _temp205 =='h'? _temp203 =='h': 0){ goto
_LL181;} else{ goto _LL182;} _LL182: goto _LL183; _LL175: t= Cyc_Absyn_uint_t;
goto _LL173; _LL177: t= Cyc_Absyn_ulong_t; goto _LL173; _LL179: t= Cyc_Absyn_ushort_t;
goto _LL173; _LL181: t= Cyc_Absyn_uchar_t; goto _LL173; _LL183: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ struct _tagged_string _temp207=( struct
_tagged_string) Cyc_String_implode( _temp74); int _temp208=( int) _temp72;
xprintf("length modifier '%.*s' is not allowed with %%%c", _temp207.last_plus_one
- _temp207.curr, _temp207.curr, _temp208);})); return 0; _LL173:;} _temp61=({
struct Cyc_List_List* _temp209=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp209->hd=( void*) t; _temp209->tl= _temp61; _temp209;});
break; case 'f': _LL171: goto _LL210; case 'F': _LL210: goto _LL211; case 'e':
_LL211: goto _LL212; case 'E': _LL212: goto _LL213; case 'g': _LL213: goto
_LL214; case 'G': _LL214: goto _LL215; case 'a': _LL215: goto _LL216; case 'A':
_LL216: if( _temp74 != 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string) xprintf("length modifiers are not allowed with %%%c",(
int) _temp72)); return 0;} _temp61=({ struct Cyc_List_List* _temp218=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp218->hd=( void*)
Cyc_Absyn_double_t; _temp218->tl= _temp61; _temp218;}); break; case 'c': _LL217:{
struct Cyc_List_List* f= _temp80; for( 0; f != 0; f=(( struct Cyc_List_List*)
_check_null( f))->tl){ if(( int)(( struct Cyc_List_List*) _check_null( f))->hd
=='#'? 1:( int)(( struct Cyc_List_List*) _check_null( f))->hd =='0'){ Cyc_Tcutil_terr(
loc,( struct _tagged_string) xprintf("flag '%c' not allowed with %%c",( int)((
struct Cyc_List_List*) _check_null( f))->hd)); return 0;}}} if( _temp74 != 0){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string _temp220=(
struct _tagged_string) Cyc_String_implode( _temp74); xprintf("length modifier '%.*s' not allowed with %%c",
_temp220.last_plus_one - _temp220.curr, _temp220.curr);})); return 0;} if(
_temp76 != 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp221=( struct _tagged_string) Cyc_String_implode( _temp76);
xprintf("precision '%.*s' not allowed with %%c", _temp221.last_plus_one -
_temp221.curr, _temp221.curr);})); return 0;} _temp61=({ struct Cyc_List_List*
_temp222=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp222->hd=( void*) Cyc_Absyn_sint_t; _temp222->tl= _temp61; _temp222;});
break; case 's': _LL219: if( _temp80 != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp224=( struct _tagged_string) Cyc_String_implode(
_temp80); xprintf("flags '%.*s' not allowed with %%s", _temp224.last_plus_one -
_temp224.curr, _temp224.curr);})); return 0;} if( _temp78 != 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ struct _tagged_string _temp225=( struct
_tagged_string) Cyc_String_implode( _temp78); xprintf("width '%.*s' not allowed with %%s",
_temp225.last_plus_one - _temp225.curr, _temp225.curr);})); return 0;} if(
_temp76 != 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp226=( struct _tagged_string) Cyc_String_implode( _temp76);
xprintf("precision '%.*s' not allowed with %%s", _temp226.last_plus_one -
_temp226.curr, _temp226.curr);})); return 0;} if( _temp74 != 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ struct _tagged_string _temp227=( struct
_tagged_string) Cyc_String_implode( _temp74); xprintf("length modifier '%.*s' not allowed with %%s",
_temp227.last_plus_one - _temp227.curr, _temp227.curr);})); return 0;} _temp61=({
struct Cyc_List_List* _temp228=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp228->hd=( void*) Cyc_Absyn_const_string_typ( Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,({ struct Cyc_Core_Opt* _temp229=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp229->v=(
void*) Cyc_Tcenv_lookup_type_vars( te); _temp229;}))); _temp228->tl= _temp61;
_temp228;}); break; case 'p': _LL223: Cyc_Tcutil_terr( loc,({ unsigned char*
_temp231=( unsigned char*)"%%p not supported"; struct _tagged_string _temp232;
_temp232.curr= _temp231; _temp232.base= _temp231; _temp232.last_plus_one=
_temp231 + 18; _temp232;})); return 0; case 'n': _LL230:{ struct Cyc_List_List*
f= _temp80; for( 0; f != 0; f=(( struct Cyc_List_List*) _check_null( f))->tl){
if(( int)(( struct Cyc_List_List*) _check_null( f))->hd =='#'? 1:( int)(( struct
Cyc_List_List*) _check_null( f))->hd =='0'){ Cyc_Tcutil_terr( loc,( struct
_tagged_string) xprintf("flag '%c' not allowed with %%n",( int)(( struct Cyc_List_List*)
_check_null( f))->hd)); return 0;}}} if( _temp76 != 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp234=( struct _tagged_string)
Cyc_String_implode( _temp76); xprintf("precision '%.*s' not allowed with %%n",
_temp234.last_plus_one - _temp234.curr, _temp234.curr);})); return 0;}{ struct
Cyc_Core_Opt* _temp236=({ struct Cyc_Core_Opt* _temp235=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp235->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp235;}); goto _LL237; _LL237:{ struct Cyc_List_List* _temp238= _temp74;
struct Cyc_List_List _temp251; struct Cyc_List_List* _temp252; int _temp254;
struct Cyc_List_List _temp257; struct Cyc_List_List* _temp258; int _temp260;
struct Cyc_List_List _temp263; struct Cyc_List_List* _temp264; struct Cyc_List_List
_temp266; struct Cyc_List_List* _temp267; int _temp269; int _temp271; _LL240:
if( _temp238 == 0){ goto _LL241;} else{ goto _LL242;} _LL242: if( _temp238 == 0){
goto _LL244;} else{ _temp251=* _temp238; _LL255: _temp254=( int) _temp251.hd;
goto _LL253; _LL253: _temp252= _temp251.tl; if( _temp252 == 0){ goto _LL250;}
else{ goto _LL244;}} _LL250: if( _temp254 =='l'){ goto _LL243;} else{ goto
_LL244;} _LL244: if( _temp238 == 0){ goto _LL246;} else{ _temp257=* _temp238;
_LL261: _temp260=( int) _temp257.hd; goto _LL259; _LL259: _temp258= _temp257.tl;
if( _temp258 == 0){ goto _LL256;} else{ goto _LL246;}} _LL256: if( _temp260 =='h'){
goto _LL245;} else{ goto _LL246;} _LL246: if( _temp238 == 0){ goto _LL248;}
else{ _temp263=* _temp238; _LL272: _temp271=( int) _temp263.hd; goto _LL265;
_LL265: _temp264= _temp263.tl; if( _temp264 == 0){ goto _LL248;} else{ _temp266=*
_temp264; _LL270: _temp269=( int) _temp266.hd; goto _LL268; _LL268: _temp267=
_temp266.tl; if( _temp267 == 0){ goto _LL262;} else{ goto _LL248;}}} _LL262: if(
_temp271 =='h'? _temp269 =='h': 0){ goto _LL247;} else{ goto _LL248;} _LL248:
goto _LL249; _LL241: t= Cyc_Absyn_at_typ( Cyc_Absyn_sint_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp236), Cyc_Absyn_empty_tqual());
goto _LL239; _LL243: t= Cyc_Absyn_at_typ( Cyc_Absyn_ulong_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp236), Cyc_Absyn_empty_tqual());
goto _LL239; _LL245: t= Cyc_Absyn_at_typ( Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp236), Cyc_Absyn_empty_tqual());
goto _LL239; _LL247: t= Cyc_Absyn_at_typ( Cyc_Absyn_schar_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp236), Cyc_Absyn_empty_tqual());
goto _LL239; _LL249: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp273=( struct _tagged_string) Cyc_String_implode( _temp74);
xprintf("length modifier '%.*s' not allowed with %%n", _temp273.last_plus_one -
_temp273.curr, _temp273.curr);})); return 0; _LL239:;} _temp61=({ struct Cyc_List_List*
_temp274=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp274->hd=( void*) t; _temp274->tl= _temp61; _temp274;}); break;} case '%':
_LL233: if( _temp80 != 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({
struct _tagged_string _temp276=( struct _tagged_string) Cyc_String_implode(
_temp80); xprintf("flags '%.*s' not allowed with %%%%", _temp276.last_plus_one -
_temp276.curr, _temp276.curr);})); return 0;} if( _temp78 != 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ struct _tagged_string _temp277=( struct
_tagged_string) Cyc_String_implode( _temp78); xprintf("width '%.*s' not allowed with %%%%",
_temp277.last_plus_one - _temp277.curr, _temp277.curr);})); return 0;} if(
_temp76 != 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp278=( struct _tagged_string) Cyc_String_implode( _temp76);
xprintf("precision '%.*s' not allowed with %%%%", _temp278.last_plus_one -
_temp278.curr, _temp278.curr);})); return 0;} if( _temp74 != 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ struct _tagged_string _temp279=( struct
_tagged_string) Cyc_String_implode( _temp74); xprintf("length modifier '%.*s' not allowed with %%%%",
_temp279.last_plus_one - _temp279.curr, _temp279.curr);})); return 0;} break;
default: _LL275: return 0;}}}}} return(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp61);}}} struct _tuple4{ int f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; unsigned char f4; int f5; } ; struct Cyc_Core_Opt*
Cyc_Formatstr_parse_inputformat( struct _tagged_string s, int i){ unsigned int
_temp281= Cyc_String_strlen( s); goto _LL282; _LL282: if( i < 0? 1: i >=
_temp281){ return 0;}{ int _temp283= 0; goto _LL284; _LL284: { unsigned char
_temp285=*(( const unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char), i); goto
_LL286; _LL286: if( _temp285 =='*'){ _temp283= 1; i ++; if( i >= _temp281){
return 0;}}{ struct Cyc_List_List* _temp287= 0; goto _LL288; _LL288: for( 0; i <
_temp281; i ++){ _temp285=*(( const unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char),
i); if( isdigit(( int) _temp285)){ _temp287=({ struct Cyc_List_List* _temp289=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp289->hd=(
void*)(( int) _temp285); _temp289->tl= _temp287; _temp289;});} else{ break;}}
if( i >= _temp281){ return 0;} _temp287=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp287);{ struct Cyc_List_List* _temp290= 0; goto
_LL291; _LL291: _temp285=*(( const unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char),
i); switch( _temp285){ case 'h': _LL292: _temp290=({ struct Cyc_List_List*
_temp294=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp294->hd=( void*)(( int) _temp285); _temp294->tl= _temp290; _temp294;}); i
++; if( i >= _temp281){ return 0;} _temp285=*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( unsigned char), i); if( _temp285 =='h'){ _temp290=({ struct Cyc_List_List*
_temp295=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp295->hd=( void*)(( int) _temp285); _temp295->tl= _temp290; _temp295;}); i
++;} break; case 'l': _LL293: _temp290=({ struct Cyc_List_List* _temp297=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp297->hd=(
void*)(( int) _temp285); _temp297->tl= _temp290; _temp297;}); i ++; if( i >=
_temp281){ return 0;} _temp285=*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( unsigned char), i); if( _temp285 =='l'){ _temp290=({ struct Cyc_List_List*
_temp298=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp298->hd=( void*)(( int) _temp285); _temp298->tl= _temp290; _temp298;}); i
++;} break; case 'j': _LL296: goto _LL299; case 'z': _LL299: goto _LL300; case 't':
_LL300: goto _LL301; case 'L': _LL301: _temp290=({ struct Cyc_List_List*
_temp303=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp303->hd=( void*)(( int) _temp285); _temp303->tl= _temp290; _temp303;}); i
++; break; default: _LL302: break;} if( i >= _temp281){ return 0;} _temp290=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp290);
_temp285=*(( const unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char), i); switch(
_temp285){ case 'd': _LL305: break; case 'i': _LL306: break; case 'o': _LL307:
break; case 'u': _LL308: break; case 'x': _LL309: break; case 'X': _LL310:
break; case 'f': _LL311: break; case 'F': _LL312: break; case 'e': _LL313:
break; case 'E': _LL314: break; case 'g': _LL315: break; case 'G': _LL316:
break; case 'a': _LL317: break; case 'A': _LL318: break; case 'c': _LL319:
break; case 's': _LL320: break; case 'p': _LL321: break; case 'n': _LL322:
break; case '%': _LL323: break; default: _LL324: return 0;} return({ struct Cyc_Core_Opt*
_temp326=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp326->v=( void*)({ struct _tuple4* _temp327=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp327->f1= _temp283; _temp327->f2= _temp287;
_temp327->f3= _temp290; _temp327->f4= _temp285; _temp327->f5= i + 1; _temp327;});
_temp326;});}}}}} struct Cyc_List_List* Cyc_Formatstr_get_scanf_typs( struct Cyc_Tcenv_Tenv*
te, struct _tagged_string s, struct Cyc_Position_Segment* loc){ unsigned int
_temp328= Cyc_String_strlen( s); goto _LL329; _LL329: { struct Cyc_List_List*
_temp330= 0; goto _LL331; _LL331: { int i; for( i= 0; i < _temp328; i ++){ if(*((
const unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char), i) !='%'){ continue;}{
struct Cyc_Core_Opt* _temp332= Cyc_Formatstr_parse_inputformat( s, i + 1); goto
_LL333; _LL333: if( _temp332 == 0){ Cyc_Tcutil_terr( loc,({ unsigned char*
_temp334=( unsigned char*)"bad format string"; struct _tagged_string _temp335;
_temp335.curr= _temp334; _temp335.base= _temp334; _temp335.last_plus_one=
_temp334 + 18; _temp335;})); return 0;}{ struct _tuple4 _temp338; int _temp339;
unsigned char _temp341; struct Cyc_List_List* _temp343; struct Cyc_List_List*
_temp345; int _temp347; struct _tuple4* _temp336=( struct _tuple4*)(( struct Cyc_Core_Opt*)
_check_null( _temp332))->v; _temp338=* _temp336; _LL348: _temp347= _temp338.f1;
goto _LL346; _LL346: _temp345= _temp338.f2; goto _LL344; _LL344: _temp343=
_temp338.f3; goto _LL342; _LL342: _temp341= _temp338.f4; goto _LL340; _LL340:
_temp339= _temp338.f5; goto _LL337; _LL337: i= _temp339 - 1;{ struct Cyc_List_List*
_temp349= _temp343; struct Cyc_List_List _temp356; struct Cyc_List_List*
_temp357; int _temp359; _LL351: if( _temp349 == 0){ goto _LL353;} else{ _temp356=*
_temp349; _LL360: _temp359=( int) _temp356.hd; goto _LL358; _LL358: _temp357=
_temp356.tl; if( _temp357 == 0){ goto _LL355;} else{ goto _LL353;}} _LL355: if((
_temp359 =='j'? 1: _temp359 =='z')? 1: _temp359 =='t'){ goto _LL352;} else{ goto
_LL353;} _LL353: goto _LL354; _LL352: Cyc_Tcutil_terr( loc,( struct
_tagged_string) xprintf("length modifier '%c' is not supported", _temp359));
return 0; _LL354: goto _LL350; _LL350:;} if( _temp347){ continue;}{ void* t;
switch( _temp341){ case 'd': _LL361: goto _LL362; case 'i': _LL362: { struct Cyc_Core_Opt*
_temp365=({ struct Cyc_Core_Opt* _temp364=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp364->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp364;}); goto _LL366; _LL366:{ struct Cyc_List_List* _temp367= _temp343;
struct Cyc_List_List _temp380; struct Cyc_List_List* _temp381; int _temp383;
struct Cyc_List_List _temp386; struct Cyc_List_List* _temp387; int _temp389;
struct Cyc_List_List _temp392; struct Cyc_List_List* _temp393; struct Cyc_List_List
_temp395; struct Cyc_List_List* _temp396; int _temp398; int _temp400; _LL369:
if( _temp367 == 0){ goto _LL370;} else{ goto _LL371;} _LL371: if( _temp367 == 0){
goto _LL373;} else{ _temp380=* _temp367; _LL384: _temp383=( int) _temp380.hd;
goto _LL382; _LL382: _temp381= _temp380.tl; if( _temp381 == 0){ goto _LL379;}
else{ goto _LL373;}} _LL379: if( _temp383 =='l'){ goto _LL372;} else{ goto
_LL373;} _LL373: if( _temp367 == 0){ goto _LL375;} else{ _temp386=* _temp367;
_LL390: _temp389=( int) _temp386.hd; goto _LL388; _LL388: _temp387= _temp386.tl;
if( _temp387 == 0){ goto _LL385;} else{ goto _LL375;}} _LL385: if( _temp389 =='h'){
goto _LL374;} else{ goto _LL375;} _LL375: if( _temp367 == 0){ goto _LL377;}
else{ _temp392=* _temp367; _LL401: _temp400=( int) _temp392.hd; goto _LL394;
_LL394: _temp393= _temp392.tl; if( _temp393 == 0){ goto _LL377;} else{ _temp395=*
_temp393; _LL399: _temp398=( int) _temp395.hd; goto _LL397; _LL397: _temp396=
_temp395.tl; if( _temp396 == 0){ goto _LL391;} else{ goto _LL377;}}} _LL391: if(
_temp400 =='h'? _temp398 =='h': 0){ goto _LL376;} else{ goto _LL377;} _LL377:
goto _LL378; _LL370: t= Cyc_Absyn_at_typ( Cyc_Absyn_sint_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp365), Cyc_Absyn_empty_tqual());
goto _LL368; _LL372: t= Cyc_Absyn_at_typ( Cyc_Absyn_slong_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp365), Cyc_Absyn_empty_tqual());
goto _LL368; _LL374: t= Cyc_Absyn_at_typ( Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp365), Cyc_Absyn_empty_tqual());
goto _LL368; _LL376: t= Cyc_Absyn_at_typ( Cyc_Absyn_schar_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp365), Cyc_Absyn_empty_tqual());
goto _LL368; _LL378: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp402=( struct _tagged_string) Cyc_String_implode( _temp343);
int _temp403=( int) _temp341; xprintf("length modifier '%.*s' is not allowed with %%%c",
_temp402.last_plus_one - _temp402.curr, _temp402.curr, _temp403);})); return 0;
_LL368:;} _temp330=({ struct Cyc_List_List* _temp404=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp404->hd=( void*) t; _temp404->tl=
_temp330; _temp404;}); break;} case 'u': _LL363: goto _LL405; case 'o': _LL405:
goto _LL406; case 'x': _LL406: goto _LL407; case 'X': _LL407: { struct Cyc_Core_Opt*
_temp410=({ struct Cyc_Core_Opt* _temp409=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp409->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp409;}); goto _LL411; _LL411:{ struct Cyc_List_List* _temp412= _temp343;
struct Cyc_List_List _temp425; struct Cyc_List_List* _temp426; int _temp428;
struct Cyc_List_List _temp431; struct Cyc_List_List* _temp432; int _temp434;
struct Cyc_List_List _temp437; struct Cyc_List_List* _temp438; struct Cyc_List_List
_temp440; struct Cyc_List_List* _temp441; int _temp443; int _temp445; _LL414:
if( _temp412 == 0){ goto _LL415;} else{ goto _LL416;} _LL416: if( _temp412 == 0){
goto _LL418;} else{ _temp425=* _temp412; _LL429: _temp428=( int) _temp425.hd;
goto _LL427; _LL427: _temp426= _temp425.tl; if( _temp426 == 0){ goto _LL424;}
else{ goto _LL418;}} _LL424: if( _temp428 =='l'){ goto _LL417;} else{ goto
_LL418;} _LL418: if( _temp412 == 0){ goto _LL420;} else{ _temp431=* _temp412;
_LL435: _temp434=( int) _temp431.hd; goto _LL433; _LL433: _temp432= _temp431.tl;
if( _temp432 == 0){ goto _LL430;} else{ goto _LL420;}} _LL430: if( _temp434 =='h'){
goto _LL419;} else{ goto _LL420;} _LL420: if( _temp412 == 0){ goto _LL422;}
else{ _temp437=* _temp412; _LL446: _temp445=( int) _temp437.hd; goto _LL439;
_LL439: _temp438= _temp437.tl; if( _temp438 == 0){ goto _LL422;} else{ _temp440=*
_temp438; _LL444: _temp443=( int) _temp440.hd; goto _LL442; _LL442: _temp441=
_temp440.tl; if( _temp441 == 0){ goto _LL436;} else{ goto _LL422;}}} _LL436: if(
_temp445 =='h'? _temp443 =='h': 0){ goto _LL421;} else{ goto _LL422;} _LL422:
goto _LL423; _LL415: t= Cyc_Absyn_at_typ( Cyc_Absyn_uint_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp410), Cyc_Absyn_empty_tqual());
goto _LL413; _LL417: t= Cyc_Absyn_at_typ( Cyc_Absyn_ulong_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp410), Cyc_Absyn_empty_tqual());
goto _LL413; _LL419: t= Cyc_Absyn_at_typ( Cyc_Absyn_ushort_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp410), Cyc_Absyn_empty_tqual());
goto _LL413; _LL421: t= Cyc_Absyn_at_typ( Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp410), Cyc_Absyn_empty_tqual());
goto _LL413; _LL423: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp447=( struct _tagged_string) Cyc_String_implode( _temp343);
int _temp448=( int) _temp341; xprintf("length modifier '%.*s' is not allowed with %%%c",
_temp447.last_plus_one - _temp447.curr, _temp447.curr, _temp448);})); return 0;
_LL413:;} _temp330=({ struct Cyc_List_List* _temp449=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp449->hd=( void*) t; _temp449->tl=
_temp330; _temp449;}); break;} case 'f': _LL408: goto _LL450; case 'F': _LL450:
goto _LL451; case 'e': _LL451: goto _LL452; case 'E': _LL452: goto _LL453; case
'g': _LL453: goto _LL454; case 'G': _LL454: goto _LL455; case 'a': _LL455: goto
_LL456; case 'A': _LL456: { struct Cyc_Core_Opt* _temp459=({ struct Cyc_Core_Opt*
_temp458=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp458->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp458;}); goto _LL460;
_LL460:{ struct Cyc_List_List* _temp461= _temp343; struct Cyc_List_List _temp470;
struct Cyc_List_List* _temp471; int _temp473; _LL463: if( _temp461 == 0){ goto
_LL464;} else{ goto _LL465;} _LL465: if( _temp461 == 0){ goto _LL467;} else{
_temp470=* _temp461; _LL474: _temp473=( int) _temp470.hd; goto _LL472; _LL472:
_temp471= _temp470.tl; if( _temp471 == 0){ goto _LL469;} else{ goto _LL467;}}
_LL469: if( _temp473 =='l'){ goto _LL466;} else{ goto _LL467;} _LL467: goto
_LL468; _LL464: t= Cyc_Absyn_at_typ( Cyc_Absyn_float_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp459), Cyc_Absyn_empty_tqual());
goto _LL462; _LL466: t= Cyc_Absyn_at_typ( Cyc_Absyn_double_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp459), Cyc_Absyn_empty_tqual());
goto _LL462; _LL468: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp475=( struct _tagged_string) Cyc_String_implode( _temp343);
int _temp476=( int) _temp341; xprintf("length modifier '%.*s' is not allowed with %%%c",
_temp475.last_plus_one - _temp475.curr, _temp475.curr, _temp476);})); return 0;
_LL462:;} _temp330=({ struct Cyc_List_List* _temp477=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp477->hd=( void*) t; _temp477->tl=
_temp330; _temp477;}); break;} case 'c': _LL457: Cyc_Tcutil_terr( loc,({
unsigned char* _temp479=( unsigned char*)"%%c is not supported"; struct
_tagged_string _temp480; _temp480.curr= _temp479; _temp480.base= _temp479;
_temp480.last_plus_one= _temp479 + 21; _temp480;})); return 0; case 's': _LL478:
Cyc_Tcutil_terr( loc,({ unsigned char* _temp482=( unsigned char*)"%%s is not supported";
struct _tagged_string _temp483; _temp483.curr= _temp482; _temp483.base= _temp482;
_temp483.last_plus_one= _temp482 + 21; _temp483;})); return 0; case '[': _LL481:
Cyc_Tcutil_terr( loc,({ unsigned char* _temp485=( unsigned char*)"%%[ is not supported";
struct _tagged_string _temp486; _temp486.curr= _temp485; _temp486.base= _temp485;
_temp486.last_plus_one= _temp485 + 21; _temp486;})); return 0; case 'p': _LL484:
Cyc_Tcutil_terr( loc,({ unsigned char* _temp488=( unsigned char*)"%%p is not supported";
struct _tagged_string _temp489; _temp489.curr= _temp488; _temp489.base= _temp488;
_temp489.last_plus_one= _temp488 + 21; _temp489;})); return 0; case 'n': _LL487: {
struct Cyc_Core_Opt* _temp492=({ struct Cyc_Core_Opt* _temp491=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp491->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp491;}); goto _LL493; _LL493:{ struct Cyc_List_List* _temp494= _temp343;
struct Cyc_List_List _temp507; struct Cyc_List_List* _temp508; int _temp510;
struct Cyc_List_List _temp513; struct Cyc_List_List* _temp514; int _temp516;
struct Cyc_List_List _temp519; struct Cyc_List_List* _temp520; struct Cyc_List_List
_temp522; struct Cyc_List_List* _temp523; int _temp525; int _temp527; _LL496:
if( _temp494 == 0){ goto _LL497;} else{ goto _LL498;} _LL498: if( _temp494 == 0){
goto _LL500;} else{ _temp507=* _temp494; _LL511: _temp510=( int) _temp507.hd;
goto _LL509; _LL509: _temp508= _temp507.tl; if( _temp508 == 0){ goto _LL506;}
else{ goto _LL500;}} _LL506: if( _temp510 =='l'){ goto _LL499;} else{ goto
_LL500;} _LL500: if( _temp494 == 0){ goto _LL502;} else{ _temp513=* _temp494;
_LL517: _temp516=( int) _temp513.hd; goto _LL515; _LL515: _temp514= _temp513.tl;
if( _temp514 == 0){ goto _LL512;} else{ goto _LL502;}} _LL512: if( _temp516 =='h'){
goto _LL501;} else{ goto _LL502;} _LL502: if( _temp494 == 0){ goto _LL504;}
else{ _temp519=* _temp494; _LL528: _temp527=( int) _temp519.hd; goto _LL521;
_LL521: _temp520= _temp519.tl; if( _temp520 == 0){ goto _LL504;} else{ _temp522=*
_temp520; _LL526: _temp525=( int) _temp522.hd; goto _LL524; _LL524: _temp523=
_temp522.tl; if( _temp523 == 0){ goto _LL518;} else{ goto _LL504;}}} _LL518: if(
_temp527 =='h'? _temp525 =='h': 0){ goto _LL503;} else{ goto _LL504;} _LL504:
goto _LL505; _LL497: t= Cyc_Absyn_at_typ( Cyc_Absyn_sint_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp492), Cyc_Absyn_empty_tqual());
goto _LL495; _LL499: t= Cyc_Absyn_at_typ( Cyc_Absyn_ulong_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp492), Cyc_Absyn_empty_tqual());
goto _LL495; _LL501: t= Cyc_Absyn_at_typ( Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp492), Cyc_Absyn_empty_tqual());
goto _LL495; _LL503: t= Cyc_Absyn_at_typ( Cyc_Absyn_schar_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp492), Cyc_Absyn_empty_tqual());
goto _LL495; _LL505: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp529=( struct _tagged_string) Cyc_String_implode( _temp343);
xprintf("length modifier '%.*s' not allowed with %%n", _temp529.last_plus_one -
_temp529.curr, _temp529.curr);})); return 0; _LL495:;} _temp330=({ struct Cyc_List_List*
_temp530=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp530->hd=( void*) t; _temp530->tl= _temp330; _temp530;}); break;} case '%':
_LL490: if( _temp347){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp532=(
unsigned char*)"Assignment suppression (*) is not allowed with %%%%"; struct
_tagged_string _temp533; _temp533.curr= _temp532; _temp533.base= _temp532;
_temp533.last_plus_one= _temp532 + 52; _temp533;})); return 0;} if( _temp345 !=
0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string
_temp534=( struct _tagged_string) Cyc_String_implode( _temp345); xprintf("width '%.*s' not allowed with %%%%",
_temp534.last_plus_one - _temp534.curr, _temp534.curr);})); return 0;} if(
_temp343 != 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp535=( struct _tagged_string) Cyc_String_implode( _temp343);
xprintf("length modifier '%.*s' not allowed with %%%%", _temp535.last_plus_one -
_temp535.curr, _temp535.curr);})); return 0;} break; default: _LL531: return 0;}}}}}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp330);}}}