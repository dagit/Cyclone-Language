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
unsigned char* tag; struct _tagged_arr f1; } ; extern int Cyc_String_strlen(
struct _tagged_arr s); extern struct _tagged_arr Cyc_String_implode( struct Cyc_List_List*
c); struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct _tagged_arr
line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[ 9u];
struct Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static const
int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{
struct _tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
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
loc,( struct _tagged_arr) xprintf("length modifier '%c' is not supported",
_temp82)); return 0; _LL77: goto _LL73; _LL73:;}{ struct Cyc_List_List* _temp84=
_temp68; struct Cyc_List_List _temp91; struct Cyc_List_List* _temp92; int
_temp94; _LL86: if( _temp84 == 0){ goto _LL88;} else{ _temp91=* _temp84; _LL95:
_temp94=( int) _temp91.hd; goto _LL93; _LL93: _temp92= _temp91.tl; if( _temp92
== 0){ goto _LL90;} else{ goto _LL88;}} _LL90: if( _temp94 =='*'){ goto _LL87;}
else{ goto _LL88;} _LL88: goto _LL89; _LL87: _temp55=({ struct Cyc_List_List*
_temp96=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp96->hd=( void*) Cyc_Absyn_sint_t; _temp96->tl= _temp55; _temp96;}); goto
_LL85; _LL89: goto _LL85; _LL85:;}{ struct Cyc_List_List* _temp97= _temp66;
struct Cyc_List_List _temp104; struct Cyc_List_List* _temp105; struct Cyc_List_List
_temp107; struct Cyc_List_List* _temp108; int _temp110; int _temp112; _LL99: if(
_temp97 == 0){ goto _LL101;} else{ _temp104=* _temp97; _LL113: _temp112=( int)
_temp104.hd; goto _LL106; _LL106: _temp105= _temp104.tl; if( _temp105 == 0){
goto _LL101;} else{ _temp107=* _temp105; _LL111: _temp110=( int) _temp107.hd;
goto _LL109; _LL109: _temp108= _temp107.tl; if( _temp108 == 0){ goto _LL103;}
else{ goto _LL101;}}} _LL103: if( _temp112 =='.'? _temp110 =='*': 0){ goto
_LL100;} else{ goto _LL101;} _LL101: goto _LL102; _LL100: _temp55=({ struct Cyc_List_List*
_temp114=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp114->hd=( void*) Cyc_Absyn_sint_t; _temp114->tl= _temp55; _temp114;}); goto
_LL98; _LL102: goto _LL98; _LL98:;}{ void* t; switch( _temp62){ case 'd': _LL115:
goto _LL116; case 'i': _LL116:{ struct Cyc_List_List* f= _temp70; for( 0; f != 0;
f=(( struct Cyc_List_List*) _check_null( f))->tl){ if(( int)(( struct Cyc_List_List*)
_check_null( f))->hd =='#'){ Cyc_Tcutil_terr( loc,( struct _tagged_arr) xprintf("flag '#' is not valid with %%%c",(
int) _temp62)); return 0;}}}{ struct Cyc_List_List* _temp118= _temp64; struct
Cyc_List_List _temp131; struct Cyc_List_List* _temp132; int _temp134; struct Cyc_List_List
_temp137; struct Cyc_List_List* _temp138; int _temp140; struct Cyc_List_List
_temp143; struct Cyc_List_List* _temp144; struct Cyc_List_List _temp146; struct
Cyc_List_List* _temp147; int _temp149; int _temp151; _LL120: if( _temp118 == 0){
goto _LL121;} else{ goto _LL122;} _LL122: if( _temp118 == 0){ goto _LL124;}
else{ _temp131=* _temp118; _LL135: _temp134=( int) _temp131.hd; goto _LL133;
_LL133: _temp132= _temp131.tl; if( _temp132 == 0){ goto _LL130;} else{ goto
_LL124;}} _LL130: if( _temp134 =='l'){ goto _LL123;} else{ goto _LL124;} _LL124:
if( _temp118 == 0){ goto _LL126;} else{ _temp137=* _temp118; _LL141: _temp140=(
int) _temp137.hd; goto _LL139; _LL139: _temp138= _temp137.tl; if( _temp138 == 0){
goto _LL136;} else{ goto _LL126;}} _LL136: if( _temp140 =='h'){ goto _LL125;}
else{ goto _LL126;} _LL126: if( _temp118 == 0){ goto _LL128;} else{ _temp143=*
_temp118; _LL152: _temp151=( int) _temp143.hd; goto _LL145; _LL145: _temp144=
_temp143.tl; if( _temp144 == 0){ goto _LL128;} else{ _temp146=* _temp144; _LL150:
_temp149=( int) _temp146.hd; goto _LL148; _LL148: _temp147= _temp146.tl; if(
_temp147 == 0){ goto _LL142;} else{ goto _LL128;}}} _LL142: if( _temp151 =='h'?
_temp149 =='h': 0){ goto _LL127;} else{ goto _LL128;} _LL128: goto _LL129;
_LL121: t= Cyc_Absyn_sint_t; goto _LL119; _LL123: t= Cyc_Absyn_slong_t; goto
_LL119; _LL125: t= Cyc_Absyn_sshort_t; goto _LL119; _LL127: t= Cyc_Absyn_schar_t;
goto _LL119; _LL129: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp153=( struct _tagged_arr) Cyc_String_implode( _temp64); int
_temp154=( int) _temp62; xprintf("length modifier '%.*s' is not allowed with %%%c",
_get_arr_size( _temp153, 1u), _temp153.curr, _temp154);})); return 0; _LL119:;}
_temp55=({ struct Cyc_List_List* _temp155=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp155->hd=( void*) t; _temp155->tl= _temp55;
_temp155;}); break; case 'u': _LL117:{ struct Cyc_List_List* f= _temp70; for( 0;
f != 0; f=(( struct Cyc_List_List*) _check_null( f))->tl){ if(( int)(( struct
Cyc_List_List*) _check_null( f))->hd =='#'){ Cyc_Tcutil_terr( loc, _tag_arr("Flag '#' not valid with %%u",
sizeof( unsigned char), 28u)); return 0;}}} goto _LL156; case 'o': _LL156: goto
_LL157; case 'x': _LL157: goto _LL158; case 'X': _LL158:{ struct Cyc_List_List*
_temp160= _temp64; struct Cyc_List_List _temp173; struct Cyc_List_List* _temp174;
int _temp176; struct Cyc_List_List _temp179; struct Cyc_List_List* _temp180; int
_temp182; struct Cyc_List_List _temp185; struct Cyc_List_List* _temp186; struct
Cyc_List_List _temp188; struct Cyc_List_List* _temp189; int _temp191; int
_temp193; _LL162: if( _temp160 == 0){ goto _LL163;} else{ goto _LL164;} _LL164:
if( _temp160 == 0){ goto _LL166;} else{ _temp173=* _temp160; _LL177: _temp176=(
int) _temp173.hd; goto _LL175; _LL175: _temp174= _temp173.tl; if( _temp174 == 0){
goto _LL172;} else{ goto _LL166;}} _LL172: if( _temp176 =='l'){ goto _LL165;}
else{ goto _LL166;} _LL166: if( _temp160 == 0){ goto _LL168;} else{ _temp179=*
_temp160; _LL183: _temp182=( int) _temp179.hd; goto _LL181; _LL181: _temp180=
_temp179.tl; if( _temp180 == 0){ goto _LL178;} else{ goto _LL168;}} _LL178: if(
_temp182 =='h'){ goto _LL167;} else{ goto _LL168;} _LL168: if( _temp160 == 0){
goto _LL170;} else{ _temp185=* _temp160; _LL194: _temp193=( int) _temp185.hd;
goto _LL187; _LL187: _temp186= _temp185.tl; if( _temp186 == 0){ goto _LL170;}
else{ _temp188=* _temp186; _LL192: _temp191=( int) _temp188.hd; goto _LL190;
_LL190: _temp189= _temp188.tl; if( _temp189 == 0){ goto _LL184;} else{ goto
_LL170;}}} _LL184: if( _temp193 =='h'? _temp191 =='h': 0){ goto _LL169;} else{
goto _LL170;} _LL170: goto _LL171; _LL163: t= Cyc_Absyn_uint_t; goto _LL161;
_LL165: t= Cyc_Absyn_ulong_t; goto _LL161; _LL167: t= Cyc_Absyn_ushort_t; goto
_LL161; _LL169: t= Cyc_Absyn_uchar_t; goto _LL161; _LL171: Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp195=( struct _tagged_arr) Cyc_String_implode(
_temp64); int _temp196=( int) _temp62; xprintf("length modifier '%.*s' is not allowed with %%%c",
_get_arr_size( _temp195, 1u), _temp195.curr, _temp196);})); return 0; _LL161:;}
_temp55=({ struct Cyc_List_List* _temp197=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp197->hd=( void*) t; _temp197->tl= _temp55;
_temp197;}); break; case 'f': _LL159: goto _LL198; case 'F': _LL198: goto _LL199;
case 'e': _LL199: goto _LL200; case 'E': _LL200: goto _LL201; case 'g': _LL201:
goto _LL202; case 'G': _LL202: goto _LL203; case 'a': _LL203: goto _LL204; case
'A': _LL204: if( _temp64 != 0){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)
xprintf("length modifiers are not allowed with %%%c",( int) _temp62)); return 0;}
_temp55=({ struct Cyc_List_List* _temp206=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp206->hd=( void*) Cyc_Absyn_double_t;
_temp206->tl= _temp55; _temp206;}); break; case 'c': _LL205:{ struct Cyc_List_List*
f= _temp70; for( 0; f != 0; f=(( struct Cyc_List_List*) _check_null( f))->tl){
if(( int)(( struct Cyc_List_List*) _check_null( f))->hd =='#'? 1:( int)(( struct
Cyc_List_List*) _check_null( f))->hd =='0'){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr) xprintf("flag '%c' not allowed with %%c",( int)(( struct Cyc_List_List*)
_check_null( f))->hd)); return 0;}}} if( _temp64 != 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp208=( struct _tagged_arr) Cyc_String_implode(
_temp64); xprintf("length modifier '%.*s' not allowed with %%c", _get_arr_size(
_temp208, 1u), _temp208.curr);})); return 0;} if( _temp66 != 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp209=( struct _tagged_arr)
Cyc_String_implode( _temp66); xprintf("precision '%.*s' not allowed with %%c",
_get_arr_size( _temp209, 1u), _temp209.curr);})); return 0;} _temp55=({ struct
Cyc_List_List* _temp210=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp210->hd=( void*) Cyc_Absyn_sint_t; _temp210->tl= _temp55; _temp210;});
break; case 's': _LL207: if( _temp70 != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp212=( struct _tagged_arr) Cyc_String_implode(
_temp70); xprintf("flags '%.*s' not allowed with %%s", _get_arr_size( _temp212,
1u), _temp212.curr);})); return 0;} if( _temp68 != 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp213=( struct _tagged_arr) Cyc_String_implode(
_temp68); xprintf("width '%.*s' not allowed with %%s", _get_arr_size( _temp213,
1u), _temp213.curr);})); return 0;} if( _temp66 != 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp214=( struct _tagged_arr) Cyc_String_implode(
_temp66); xprintf("precision '%.*s' not allowed with %%s", _get_arr_size(
_temp214, 1u), _temp214.curr);})); return 0;} if( _temp64 != 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp215=( struct _tagged_arr)
Cyc_String_implode( _temp64); xprintf("length modifier '%.*s' not allowed with %%s",
_get_arr_size( _temp215, 1u), _temp215.curr);})); return 0;} _temp55=({ struct
Cyc_List_List* _temp216=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp216->hd=( void*) Cyc_Absyn_const_string_typ( Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Formatstr_rk,({ struct Cyc_Core_Opt* _temp217=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp217->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp217;}))); _temp216->tl= _temp55; _temp216;}); break; case 'p': _LL211:
Cyc_Tcutil_terr( loc, _tag_arr("%%p not supported", sizeof( unsigned char), 18u));
return 0; case 'n': _LL218:{ struct Cyc_List_List* f= _temp70; for( 0; f != 0; f=((
struct Cyc_List_List*) _check_null( f))->tl){ if(( int)(( struct Cyc_List_List*)
_check_null( f))->hd =='#'? 1:( int)(( struct Cyc_List_List*) _check_null( f))->hd
=='0'){ Cyc_Tcutil_terr( loc,( struct _tagged_arr) xprintf("flag '%c' not allowed with %%n",(
int)(( struct Cyc_List_List*) _check_null( f))->hd)); return 0;}}} if( _temp66
!= 0){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp220=(
struct _tagged_arr) Cyc_String_implode( _temp66); xprintf("precision '%.*s' not allowed with %%n",
_get_arr_size( _temp220, 1u), _temp220.curr);})); return 0;}{ struct Cyc_Core_Opt*
_temp221=({ struct Cyc_Core_Opt* _temp259=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp259->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp259;});{ struct Cyc_List_List* _temp222= _temp64; struct Cyc_List_List
_temp235; struct Cyc_List_List* _temp236; int _temp238; struct Cyc_List_List
_temp241; struct Cyc_List_List* _temp242; int _temp244; struct Cyc_List_List
_temp247; struct Cyc_List_List* _temp248; struct Cyc_List_List _temp250; struct
Cyc_List_List* _temp251; int _temp253; int _temp255; _LL224: if( _temp222 == 0){
goto _LL225;} else{ goto _LL226;} _LL226: if( _temp222 == 0){ goto _LL228;}
else{ _temp235=* _temp222; _LL239: _temp238=( int) _temp235.hd; goto _LL237;
_LL237: _temp236= _temp235.tl; if( _temp236 == 0){ goto _LL234;} else{ goto
_LL228;}} _LL234: if( _temp238 =='l'){ goto _LL227;} else{ goto _LL228;} _LL228:
if( _temp222 == 0){ goto _LL230;} else{ _temp241=* _temp222; _LL245: _temp244=(
int) _temp241.hd; goto _LL243; _LL243: _temp242= _temp241.tl; if( _temp242 == 0){
goto _LL240;} else{ goto _LL230;}} _LL240: if( _temp244 =='h'){ goto _LL229;}
else{ goto _LL230;} _LL230: if( _temp222 == 0){ goto _LL232;} else{ _temp247=*
_temp222; _LL256: _temp255=( int) _temp247.hd; goto _LL249; _LL249: _temp248=
_temp247.tl; if( _temp248 == 0){ goto _LL232;} else{ _temp250=* _temp248; _LL254:
_temp253=( int) _temp250.hd; goto _LL252; _LL252: _temp251= _temp250.tl; if(
_temp251 == 0){ goto _LL246;} else{ goto _LL232;}}} _LL246: if( _temp255 =='h'?
_temp253 =='h': 0){ goto _LL231;} else{ goto _LL232;} _LL232: goto _LL233;
_LL225: t= Cyc_Absyn_at_typ( Cyc_Absyn_sint_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp221), Cyc_Absyn_empty_tqual()); goto _LL223; _LL227: t=
Cyc_Absyn_at_typ( Cyc_Absyn_ulong_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp221), Cyc_Absyn_empty_tqual()); goto _LL223; _LL229: t=
Cyc_Absyn_at_typ( Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp221), Cyc_Absyn_empty_tqual()); goto _LL223; _LL231: t=
Cyc_Absyn_at_typ( Cyc_Absyn_schar_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp221), Cyc_Absyn_empty_tqual()); goto _LL223; _LL233: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp257=( struct _tagged_arr)
Cyc_String_implode( _temp64); xprintf("length modifier '%.*s' not allowed with %%n",
_get_arr_size( _temp257, 1u), _temp257.curr);})); return 0; _LL223:;} _temp55=({
struct Cyc_List_List* _temp258=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp258->hd=( void*) t; _temp258->tl= _temp55; _temp258;});
break;} case '%': _LL219: if( _temp70 != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp261=( struct _tagged_arr) Cyc_String_implode(
_temp70); xprintf("flags '%.*s' not allowed with %%%%", _get_arr_size( _temp261,
1u), _temp261.curr);})); return 0;} if( _temp68 != 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp262=( struct _tagged_arr) Cyc_String_implode(
_temp68); xprintf("width '%.*s' not allowed with %%%%", _get_arr_size( _temp262,
1u), _temp262.curr);})); return 0;} if( _temp66 != 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp263=( struct _tagged_arr) Cyc_String_implode(
_temp66); xprintf("precision '%.*s' not allowed with %%%%", _get_arr_size(
_temp263, 1u), _temp263.curr);})); return 0;} if( _temp64 != 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp264=( struct _tagged_arr)
Cyc_String_implode( _temp64); xprintf("length modifier '%.*s' not allowed with %%%%",
_get_arr_size( _temp264, 1u), _temp264.curr);})); return 0;} break; default:
_LL260: return 0;}}}}} return(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp55);} struct _tuple4{ int f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; unsigned char f4; int f5; } ; struct Cyc_Core_Opt*
Cyc_Formatstr_parse_inputformat( struct _tagged_arr s, int i){ int _temp266= Cyc_String_strlen(
s); if( i < 0? 1: i >= _temp266){ return 0;}{ int _temp267= 0; unsigned char
_temp268=*(( const unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), i)); if( _temp268 =='*'){ _temp267= 1; i ++; if( i >= _temp266){
return 0;}}{ struct Cyc_List_List* _temp269= 0; for( 0; i < _temp266; i ++){
_temp268=*(( const unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), i)); if( isdigit(( int) _temp268)){ _temp269=({ struct Cyc_List_List*
_temp270=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp270->hd=( void*)(( int) _temp268); _temp270->tl= _temp269; _temp270;});}
else{ break;}} if( i >= _temp266){ return 0;} _temp269=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp269);{ struct Cyc_List_List*
_temp271= 0; _temp268=*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), i)); switch( _temp268){ case 'h': _LL272: _temp271=({
struct Cyc_List_List* _temp274=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp274->hd=( void*)(( int) _temp268); _temp274->tl=
_temp271; _temp274;}); i ++; if( i >= _temp266){ return 0;} _temp268=*(( const
unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i)); if(
_temp268 =='h'){ _temp271=({ struct Cyc_List_List* _temp275=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp275->hd=( void*)(( int) _temp268);
_temp275->tl= _temp271; _temp275;}); i ++;} break; case 'l': _LL273: _temp271=({
struct Cyc_List_List* _temp277=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp277->hd=( void*)(( int) _temp268); _temp277->tl=
_temp271; _temp277;}); i ++; if( i >= _temp266){ return 0;} _temp268=*(( const
unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i)); if(
_temp268 =='l'){ _temp271=({ struct Cyc_List_List* _temp278=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp278->hd=( void*)(( int) _temp268);
_temp278->tl= _temp271; _temp278;}); i ++;} break; case 'j': _LL276: goto _LL279;
case 'z': _LL279: goto _LL280; case 't': _LL280: goto _LL281; case 'L': _LL281:
_temp271=({ struct Cyc_List_List* _temp283=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp283->hd=( void*)(( int) _temp268); _temp283->tl=
_temp271; _temp283;}); i ++; break; default: _LL282: break;} if( i >= _temp266){
return 0;} _temp271=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp271); _temp268=*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), i)); switch( _temp268){ case 'd': _LL285: break; case 'i':
_LL286: break; case 'o': _LL287: break; case 'u': _LL288: break; case 'x':
_LL289: break; case 'X': _LL290: break; case 'f': _LL291: break; case 'F':
_LL292: break; case 'e': _LL293: break; case 'E': _LL294: break; case 'g':
_LL295: break; case 'G': _LL296: break; case 'a': _LL297: break; case 'A':
_LL298: break; case 'c': _LL299: break; case 's': _LL300: break; case 'p':
_LL301: break; case 'n': _LL302: break; case '%': _LL303: break; default: _LL304:
return 0;} return({ struct Cyc_Core_Opt* _temp306=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp306->v=( void*)({ struct _tuple4*
_temp307=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp307->f1=
_temp267; _temp307->f2= _temp269; _temp307->f3= _temp271; _temp307->f4= _temp268;
_temp307->f5= i + 1; _temp307;}); _temp306;});}}}} struct Cyc_List_List* Cyc_Formatstr_get_scanf_typs(
struct Cyc_Tcenv_Tenv* te, struct _tagged_arr s, struct Cyc_Position_Segment*
loc){ int _temp308= Cyc_String_strlen( s); struct Cyc_List_List* _temp309= 0;
int i; for( i= 0; i < _temp308; i ++){ if(*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)) !='%'){ continue;}{
struct Cyc_Core_Opt* _temp310= Cyc_Formatstr_parse_inputformat( s, i + 1); if(
_temp310 == 0){ Cyc_Tcutil_terr( loc, _tag_arr("bad format string", sizeof(
unsigned char), 18u)); return 0;}{ struct _tuple4 _temp313; int _temp314;
unsigned char _temp316; struct Cyc_List_List* _temp318; struct Cyc_List_List*
_temp320; int _temp322; struct _tuple4* _temp311=( struct _tuple4*)(( struct Cyc_Core_Opt*)
_check_null( _temp310))->v; _temp313=* _temp311; _LL323: _temp322= _temp313.f1;
goto _LL321; _LL321: _temp320= _temp313.f2; goto _LL319; _LL319: _temp318=
_temp313.f3; goto _LL317; _LL317: _temp316= _temp313.f4; goto _LL315; _LL315:
_temp314= _temp313.f5; goto _LL312; _LL312: i= _temp314 - 1;{ struct Cyc_List_List*
_temp324= _temp318; struct Cyc_List_List _temp331; struct Cyc_List_List*
_temp332; int _temp334; _LL326: if( _temp324 == 0){ goto _LL328;} else{ _temp331=*
_temp324; _LL335: _temp334=( int) _temp331.hd; goto _LL333; _LL333: _temp332=
_temp331.tl; if( _temp332 == 0){ goto _LL330;} else{ goto _LL328;}} _LL330: if((
_temp334 =='j'? 1: _temp334 =='z')? 1: _temp334 =='t'){ goto _LL327;} else{ goto
_LL328;} _LL328: goto _LL329; _LL327: Cyc_Tcutil_terr( loc,( struct _tagged_arr)
xprintf("length modifier '%c' is not supported", _temp334)); return 0; _LL329:
goto _LL325; _LL325:;} if( _temp322){ continue;}{ void* t; switch( _temp316){
case 'd': _LL336: goto _LL337; case 'i': _LL337: { struct Cyc_Core_Opt* _temp339=({
struct Cyc_Core_Opt* _temp378=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp378->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp378;});{
struct Cyc_List_List* _temp340= _temp318; struct Cyc_List_List _temp353; struct
Cyc_List_List* _temp354; int _temp356; struct Cyc_List_List _temp359; struct Cyc_List_List*
_temp360; int _temp362; struct Cyc_List_List _temp365; struct Cyc_List_List*
_temp366; struct Cyc_List_List _temp368; struct Cyc_List_List* _temp369; int
_temp371; int _temp373; _LL342: if( _temp340 == 0){ goto _LL343;} else{ goto
_LL344;} _LL344: if( _temp340 == 0){ goto _LL346;} else{ _temp353=* _temp340;
_LL357: _temp356=( int) _temp353.hd; goto _LL355; _LL355: _temp354= _temp353.tl;
if( _temp354 == 0){ goto _LL352;} else{ goto _LL346;}} _LL352: if( _temp356 =='l'){
goto _LL345;} else{ goto _LL346;} _LL346: if( _temp340 == 0){ goto _LL348;}
else{ _temp359=* _temp340; _LL363: _temp362=( int) _temp359.hd; goto _LL361;
_LL361: _temp360= _temp359.tl; if( _temp360 == 0){ goto _LL358;} else{ goto
_LL348;}} _LL358: if( _temp362 =='h'){ goto _LL347;} else{ goto _LL348;} _LL348:
if( _temp340 == 0){ goto _LL350;} else{ _temp365=* _temp340; _LL374: _temp373=(
int) _temp365.hd; goto _LL367; _LL367: _temp366= _temp365.tl; if( _temp366 == 0){
goto _LL350;} else{ _temp368=* _temp366; _LL372: _temp371=( int) _temp368.hd;
goto _LL370; _LL370: _temp369= _temp368.tl; if( _temp369 == 0){ goto _LL364;}
else{ goto _LL350;}}} _LL364: if( _temp373 =='h'? _temp371 =='h': 0){ goto
_LL349;} else{ goto _LL350;} _LL350: goto _LL351; _LL343: t= Cyc_Absyn_at_typ(
Cyc_Absyn_sint_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp339), Cyc_Absyn_empty_tqual()); goto _LL341; _LL345: t= Cyc_Absyn_at_typ(
Cyc_Absyn_slong_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp339), Cyc_Absyn_empty_tqual()); goto _LL341; _LL347: t= Cyc_Absyn_at_typ(
Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp339), Cyc_Absyn_empty_tqual()); goto _LL341; _LL349: t= Cyc_Absyn_at_typ(
Cyc_Absyn_schar_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp339), Cyc_Absyn_empty_tqual()); goto _LL341; _LL351: Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp375=( struct _tagged_arr) Cyc_String_implode(
_temp318); int _temp376=( int) _temp316; xprintf("length modifier '%.*s' is not allowed with %%%c",
_get_arr_size( _temp375, 1u), _temp375.curr, _temp376);})); return 0; _LL341:;}
_temp309=({ struct Cyc_List_List* _temp377=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp377->hd=( void*) t; _temp377->tl= _temp309;
_temp377;}); break;} case 'u': _LL338: goto _LL379; case 'o': _LL379: goto
_LL380; case 'x': _LL380: goto _LL381; case 'X': _LL381: { struct Cyc_Core_Opt*
_temp383=({ struct Cyc_Core_Opt* _temp422=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp422->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp422;});{ struct Cyc_List_List* _temp384= _temp318; struct Cyc_List_List
_temp397; struct Cyc_List_List* _temp398; int _temp400; struct Cyc_List_List
_temp403; struct Cyc_List_List* _temp404; int _temp406; struct Cyc_List_List
_temp409; struct Cyc_List_List* _temp410; struct Cyc_List_List _temp412; struct
Cyc_List_List* _temp413; int _temp415; int _temp417; _LL386: if( _temp384 == 0){
goto _LL387;} else{ goto _LL388;} _LL388: if( _temp384 == 0){ goto _LL390;}
else{ _temp397=* _temp384; _LL401: _temp400=( int) _temp397.hd; goto _LL399;
_LL399: _temp398= _temp397.tl; if( _temp398 == 0){ goto _LL396;} else{ goto
_LL390;}} _LL396: if( _temp400 =='l'){ goto _LL389;} else{ goto _LL390;} _LL390:
if( _temp384 == 0){ goto _LL392;} else{ _temp403=* _temp384; _LL407: _temp406=(
int) _temp403.hd; goto _LL405; _LL405: _temp404= _temp403.tl; if( _temp404 == 0){
goto _LL402;} else{ goto _LL392;}} _LL402: if( _temp406 =='h'){ goto _LL391;}
else{ goto _LL392;} _LL392: if( _temp384 == 0){ goto _LL394;} else{ _temp409=*
_temp384; _LL418: _temp417=( int) _temp409.hd; goto _LL411; _LL411: _temp410=
_temp409.tl; if( _temp410 == 0){ goto _LL394;} else{ _temp412=* _temp410; _LL416:
_temp415=( int) _temp412.hd; goto _LL414; _LL414: _temp413= _temp412.tl; if(
_temp413 == 0){ goto _LL408;} else{ goto _LL394;}}} _LL408: if( _temp417 =='h'?
_temp415 =='h': 0){ goto _LL393;} else{ goto _LL394;} _LL394: goto _LL395;
_LL387: t= Cyc_Absyn_at_typ( Cyc_Absyn_uint_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp383), Cyc_Absyn_empty_tqual()); goto _LL385; _LL389: t=
Cyc_Absyn_at_typ( Cyc_Absyn_ulong_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp383), Cyc_Absyn_empty_tqual()); goto _LL385; _LL391: t=
Cyc_Absyn_at_typ( Cyc_Absyn_ushort_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp383), Cyc_Absyn_empty_tqual()); goto _LL385; _LL393: t=
Cyc_Absyn_at_typ( Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk, _temp383), Cyc_Absyn_empty_tqual()); goto _LL385; _LL395: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp419=( struct _tagged_arr)
Cyc_String_implode( _temp318); int _temp420=( int) _temp316; xprintf("length modifier '%.*s' is not allowed with %%%c",
_get_arr_size( _temp419, 1u), _temp419.curr, _temp420);})); return 0; _LL385:;}
_temp309=({ struct Cyc_List_List* _temp421=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp421->hd=( void*) t; _temp421->tl= _temp309;
_temp421;}); break;} case 'f': _LL382: goto _LL423; case 'F': _LL423: goto
_LL424; case 'e': _LL424: goto _LL425; case 'E': _LL425: goto _LL426; case 'g':
_LL426: goto _LL427; case 'G': _LL427: goto _LL428; case 'a': _LL428: goto
_LL429; case 'A': _LL429: { struct Cyc_Core_Opt* _temp431=({ struct Cyc_Core_Opt*
_temp449=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp449->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp449;});{ struct Cyc_List_List*
_temp432= _temp318; struct Cyc_List_List _temp441; struct Cyc_List_List*
_temp442; int _temp444; _LL434: if( _temp432 == 0){ goto _LL435;} else{ goto
_LL436;} _LL436: if( _temp432 == 0){ goto _LL438;} else{ _temp441=* _temp432;
_LL445: _temp444=( int) _temp441.hd; goto _LL443; _LL443: _temp442= _temp441.tl;
if( _temp442 == 0){ goto _LL440;} else{ goto _LL438;}} _LL440: if( _temp444 =='l'){
goto _LL437;} else{ goto _LL438;} _LL438: goto _LL439; _LL435: t= Cyc_Absyn_at_typ(
Cyc_Absyn_float_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp431), Cyc_Absyn_empty_tqual()); goto _LL433; _LL437: t= Cyc_Absyn_at_typ(
Cyc_Absyn_double_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp431), Cyc_Absyn_empty_tqual()); goto _LL433; _LL439: Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp446=( struct _tagged_arr) Cyc_String_implode(
_temp318); int _temp447=( int) _temp316; xprintf("length modifier '%.*s' is not allowed with %%%c",
_get_arr_size( _temp446, 1u), _temp446.curr, _temp447);})); return 0; _LL433:;}
_temp309=({ struct Cyc_List_List* _temp448=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp448->hd=( void*) t; _temp448->tl= _temp309;
_temp448;}); break;} case 'c': _LL430: Cyc_Tcutil_terr( loc, _tag_arr("%%c is not supported",
sizeof( unsigned char), 21u)); return 0; case 's': _LL450: Cyc_Tcutil_terr( loc,
_tag_arr("%%s is not supported", sizeof( unsigned char), 21u)); return 0; case '[':
_LL451: Cyc_Tcutil_terr( loc, _tag_arr("%%[ is not supported", sizeof(
unsigned char), 21u)); return 0; case 'p': _LL452: Cyc_Tcutil_terr( loc,
_tag_arr("%%p is not supported", sizeof( unsigned char), 21u)); return 0; case 'n':
_LL453: { struct Cyc_Core_Opt* _temp455=({ struct Cyc_Core_Opt* _temp493=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp493->v=(
void*) Cyc_Tcenv_lookup_type_vars( te); _temp493;});{ struct Cyc_List_List*
_temp456= _temp318; struct Cyc_List_List _temp469; struct Cyc_List_List*
_temp470; int _temp472; struct Cyc_List_List _temp475; struct Cyc_List_List*
_temp476; int _temp478; struct Cyc_List_List _temp481; struct Cyc_List_List*
_temp482; struct Cyc_List_List _temp484; struct Cyc_List_List* _temp485; int
_temp487; int _temp489; _LL458: if( _temp456 == 0){ goto _LL459;} else{ goto
_LL460;} _LL460: if( _temp456 == 0){ goto _LL462;} else{ _temp469=* _temp456;
_LL473: _temp472=( int) _temp469.hd; goto _LL471; _LL471: _temp470= _temp469.tl;
if( _temp470 == 0){ goto _LL468;} else{ goto _LL462;}} _LL468: if( _temp472 =='l'){
goto _LL461;} else{ goto _LL462;} _LL462: if( _temp456 == 0){ goto _LL464;}
else{ _temp475=* _temp456; _LL479: _temp478=( int) _temp475.hd; goto _LL477;
_LL477: _temp476= _temp475.tl; if( _temp476 == 0){ goto _LL474;} else{ goto
_LL464;}} _LL474: if( _temp478 =='h'){ goto _LL463;} else{ goto _LL464;} _LL464:
if( _temp456 == 0){ goto _LL466;} else{ _temp481=* _temp456; _LL490: _temp489=(
int) _temp481.hd; goto _LL483; _LL483: _temp482= _temp481.tl; if( _temp482 == 0){
goto _LL466;} else{ _temp484=* _temp482; _LL488: _temp487=( int) _temp484.hd;
goto _LL486; _LL486: _temp485= _temp484.tl; if( _temp485 == 0){ goto _LL480;}
else{ goto _LL466;}}} _LL480: if( _temp489 =='h'? _temp487 =='h': 0){ goto
_LL465;} else{ goto _LL466;} _LL466: goto _LL467; _LL459: t= Cyc_Absyn_at_typ(
Cyc_Absyn_sint_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp455), Cyc_Absyn_empty_tqual()); goto _LL457; _LL461: t= Cyc_Absyn_at_typ(
Cyc_Absyn_ulong_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp455), Cyc_Absyn_empty_tqual()); goto _LL457; _LL463: t= Cyc_Absyn_at_typ(
Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp455), Cyc_Absyn_empty_tqual()); goto _LL457; _LL465: t= Cyc_Absyn_at_typ(
Cyc_Absyn_schar_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp455), Cyc_Absyn_empty_tqual()); goto _LL457; _LL467: Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp491=( struct _tagged_arr) Cyc_String_implode(
_temp318); xprintf("length modifier '%.*s' not allowed with %%n", _get_arr_size(
_temp491, 1u), _temp491.curr);})); return 0; _LL457:;} _temp309=({ struct Cyc_List_List*
_temp492=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp492->hd=( void*) t; _temp492->tl= _temp309; _temp492;}); break;} case '%':
_LL454: if( _temp322){ Cyc_Tcutil_terr( loc, _tag_arr("Assignment suppression (*) is not allowed with %%%%",
sizeof( unsigned char), 52u)); return 0;} if( _temp320 != 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp495=( struct _tagged_arr)
Cyc_String_implode( _temp320); xprintf("width '%.*s' not allowed with %%%%",
_get_arr_size( _temp495, 1u), _temp495.curr);})); return 0;} if( _temp318 != 0){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp496=(
struct _tagged_arr) Cyc_String_implode( _temp318); xprintf("length modifier '%.*s' not allowed with %%%%",
_get_arr_size( _temp496, 1u), _temp496.curr);})); return 0;} break; default:
_LL494: return 0;}}}}} return(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp309);}