 struct Cyc_timespec{ unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{
struct Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; extern void
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
_tagged_arr f1; } ; extern int Cyc_String_strlen( struct _tagged_arr s); extern
struct _tagged_arr Cyc_String_implode( struct Cyc_List_List* c); struct Cyc_Lineno_Pos{
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
_tagged_arr s, int i){ int _temp0= Cyc_String_strlen( s); goto _LL1; _LL1: if( i
< 0? 1: i >= _temp0){ return 0;}{ struct Cyc_List_List* _temp2= 0; goto _LL3;
_LL3: { unsigned char c=' '; for( 0; i < _temp0; i ++){ c=*(( const
unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i));
switch( c){ case '+': _LL4: goto _LL5; case '-': _LL5: goto _LL6; case ' ': _LL6:
goto _LL7; case '#': _LL7: goto _LL8; case '0': _LL8: _temp2=({ struct Cyc_List_List*
_temp10=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp10->hd=( void*)(( int) c); _temp10->tl= _temp2; _temp10;}); continue;
default: _LL9: break;} break;} if( i >= _temp0){ return 0;} _temp2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2);{ struct Cyc_List_List*
_temp12= 0; goto _LL13; _LL13: c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( c =='*'){ _temp12=({
struct Cyc_List_List* _temp14=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp14->hd=( void*)(( int) c); _temp14->tl= _temp12; _temp14;});
i ++;} else{ for( 0; i < _temp0; i ++){ c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( isdigit(( int) c)){
_temp12=({ struct Cyc_List_List* _temp15=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp15->hd=( void*)(( int) c); _temp15->tl=
_temp12; _temp15;});} else{ break;}}} if( i >= _temp0){ return 0;} _temp12=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp12);{
struct Cyc_List_List* _temp16= 0; goto _LL17; _LL17: c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( c =='.'){ _temp16=({
struct Cyc_List_List* _temp18=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp18->hd=( void*)(( int) c); _temp18->tl= _temp16; _temp18;});
i ++; if( i >= _temp0){ return 0;} c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( c =='*'){ _temp16=({
struct Cyc_List_List* _temp19=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp19->hd=( void*)(( int) c); _temp19->tl= _temp16; _temp19;});
i ++;} else{ for( 0; i < _temp0; i ++){ c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( isdigit(( int) c)){
_temp16=({ struct Cyc_List_List* _temp20=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp20->hd=( void*)(( int) c); _temp20->tl=
_temp16; _temp20;});} else{ break;}}}} if( i >= _temp0){ return 0;} _temp16=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp16);{
struct Cyc_List_List* _temp21= 0; goto _LL22; _LL22: c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); switch( c){ case 'h':
_LL23: _temp21=({ struct Cyc_List_List* _temp25=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp25->hd=( void*)(( int) c);
_temp25->tl= _temp21; _temp25;}); i ++; if( i >= _temp0){ return 0;} c=*(( const
unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i)); if( c
=='h'){ _temp21=({ struct Cyc_List_List* _temp26=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp26->hd=( void*)(( int) c);
_temp26->tl= _temp21; _temp26;}); i ++;} break; case 'l': _LL24: _temp21=({
struct Cyc_List_List* _temp28=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp28->hd=( void*)(( int) c); _temp28->tl= _temp21; _temp28;});
i ++; if( i >= _temp0){ return 0;} c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( c =='l'){ _temp21=({
struct Cyc_List_List* _temp29=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp29->hd=( void*)(( int) c); _temp29->tl= _temp21; _temp29;});
i ++;} break; case 'j': _LL27: goto _LL30; case 'z': _LL30: goto _LL31; case 't':
_LL31: goto _LL32; case 'L': _LL32: _temp21=({ struct Cyc_List_List* _temp34=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp34->hd=(
void*)(( int) c); _temp34->tl= _temp21; _temp34;}); i ++; break; default: _LL33:
break;} if( i >= _temp0){ return 0;} _temp21=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( _temp21); c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); switch( c){ case 'd':
_LL36: break; case 'i': _LL37: break; case 'o': _LL38: break; case 'u': _LL39:
break; case 'x': _LL40: break; case 'X': _LL41: break; case 'f': _LL42: break;
case 'F': _LL43: break; case 'e': _LL44: break; case 'E': _LL45: break; case 'g':
_LL46: break; case 'G': _LL47: break; case 'a': _LL48: break; case 'A': _LL49:
break; case 'c': _LL50: break; case 's': _LL51: break; case 'p': _LL52: break;
case 'n': _LL53: break; case '%': _LL54: break; default: _LL55: return 0;}
return({ struct Cyc_Core_Opt* _temp57=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp57->v=( void*)({ struct _tuple3* _temp58=( struct
_tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp58->f1= _temp2; _temp58->f2=
_temp12; _temp58->f3= _temp16; _temp58->f4= _temp21; _temp58->f5= c; _temp58->f6=
i + 1; _temp58;}); _temp57;});}}}}}} struct Cyc_List_List* Cyc_Formatstr_get_format_typs(
struct Cyc_Tcenv_Tenv* te, struct _tagged_arr s, struct Cyc_Position_Segment*
loc){ int _temp59= Cyc_String_strlen( s); goto _LL60; _LL60: { struct Cyc_List_List*
_temp61= 0; goto _LL62; _LL62: { int i; for( i= 0; i < _temp59; i ++){ if(*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i))
!='%'){ continue;}{ struct Cyc_Core_Opt* _temp63= Cyc_Formatstr_parse_conversionspecification(
s, i + 1); goto _LL64; _LL64: if( _temp63 == 0){ Cyc_Tcutil_terr( loc, _tag_arr("bad format string",
sizeof( unsigned char), 18u)); return 0;}{ struct _tuple3 _temp67; int _temp68;
unsigned char _temp70; struct Cyc_List_List* _temp72; struct Cyc_List_List*
_temp74; struct Cyc_List_List* _temp76; struct Cyc_List_List* _temp78; struct
_tuple3* _temp65=( struct _tuple3*)(( struct Cyc_Core_Opt*) _check_null( _temp63))->v;
_temp67=* _temp65; _LL79: _temp78= _temp67.f1; goto _LL77; _LL77: _temp76=
_temp67.f2; goto _LL75; _LL75: _temp74= _temp67.f3; goto _LL73; _LL73: _temp72=
_temp67.f4; goto _LL71; _LL71: _temp70= _temp67.f5; goto _LL69; _LL69: _temp68=
_temp67.f6; goto _LL66; _LL66: i= _temp68 - 1;{ struct Cyc_List_List* _temp80=
_temp72; struct Cyc_List_List _temp87; struct Cyc_List_List* _temp88; int
_temp90; _LL82: if( _temp80 == 0){ goto _LL84;} else{ _temp87=* _temp80; _LL91:
_temp90=( int) _temp87.hd; goto _LL89; _LL89: _temp88= _temp87.tl; if( _temp88
== 0){ goto _LL86;} else{ goto _LL84;}} _LL86: if(( _temp90 =='j'? 1: _temp90 =='z')?
1: _temp90 =='t'){ goto _LL83;} else{ goto _LL84;} _LL84: goto _LL85; _LL83: Cyc_Tcutil_terr(
loc,( struct _tagged_arr) xprintf("length modifier '%c' is not supported",
_temp90)); return 0; _LL85: goto _LL81; _LL81:;}{ struct Cyc_List_List* _temp92=
_temp76; struct Cyc_List_List _temp99; struct Cyc_List_List* _temp100; int
_temp102; _LL94: if( _temp92 == 0){ goto _LL96;} else{ _temp99=* _temp92; _LL103:
_temp102=( int) _temp99.hd; goto _LL101; _LL101: _temp100= _temp99.tl; if(
_temp100 == 0){ goto _LL98;} else{ goto _LL96;}} _LL98: if( _temp102 =='*'){
goto _LL95;} else{ goto _LL96;} _LL96: goto _LL97; _LL95: _temp61=({ struct Cyc_List_List*
_temp104=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp104->hd=( void*) Cyc_Absyn_sint_t; _temp104->tl= _temp61; _temp104;}); goto
_LL93; _LL97: goto _LL93; _LL93:;}{ struct Cyc_List_List* _temp105= _temp74;
struct Cyc_List_List _temp112; struct Cyc_List_List* _temp113; struct Cyc_List_List
_temp115; struct Cyc_List_List* _temp116; int _temp118; int _temp120; _LL107:
if( _temp105 == 0){ goto _LL109;} else{ _temp112=* _temp105; _LL121: _temp120=(
int) _temp112.hd; goto _LL114; _LL114: _temp113= _temp112.tl; if( _temp113 == 0){
goto _LL109;} else{ _temp115=* _temp113; _LL119: _temp118=( int) _temp115.hd;
goto _LL117; _LL117: _temp116= _temp115.tl; if( _temp116 == 0){ goto _LL111;}
else{ goto _LL109;}}} _LL111: if( _temp120 =='.'? _temp118 =='*': 0){ goto
_LL108;} else{ goto _LL109;} _LL109: goto _LL110; _LL108: _temp61=({ struct Cyc_List_List*
_temp122=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp122->hd=( void*) Cyc_Absyn_sint_t; _temp122->tl= _temp61; _temp122;}); goto
_LL106; _LL110: goto _LL106; _LL106:;}{ void* t; switch( _temp70){ case 'd':
_LL123: goto _LL124; case 'i': _LL124:{ struct Cyc_List_List* f= _temp78; for( 0;
f != 0; f=(( struct Cyc_List_List*) _check_null( f))->tl){ if(( int)(( struct
Cyc_List_List*) _check_null( f))->hd =='#'){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr) xprintf("flag '#' is not valid with %%%c",( int) _temp70)); return
0;}}}{ struct Cyc_List_List* _temp126= _temp72; struct Cyc_List_List _temp139;
struct Cyc_List_List* _temp140; int _temp142; struct Cyc_List_List _temp145;
struct Cyc_List_List* _temp146; int _temp148; struct Cyc_List_List _temp151;
struct Cyc_List_List* _temp152; struct Cyc_List_List _temp154; struct Cyc_List_List*
_temp155; int _temp157; int _temp159; _LL128: if( _temp126 == 0){ goto _LL129;}
else{ goto _LL130;} _LL130: if( _temp126 == 0){ goto _LL132;} else{ _temp139=*
_temp126; _LL143: _temp142=( int) _temp139.hd; goto _LL141; _LL141: _temp140=
_temp139.tl; if( _temp140 == 0){ goto _LL138;} else{ goto _LL132;}} _LL138: if(
_temp142 =='l'){ goto _LL131;} else{ goto _LL132;} _LL132: if( _temp126 == 0){
goto _LL134;} else{ _temp145=* _temp126; _LL149: _temp148=( int) _temp145.hd;
goto _LL147; _LL147: _temp146= _temp145.tl; if( _temp146 == 0){ goto _LL144;}
else{ goto _LL134;}} _LL144: if( _temp148 =='h'){ goto _LL133;} else{ goto
_LL134;} _LL134: if( _temp126 == 0){ goto _LL136;} else{ _temp151=* _temp126;
_LL160: _temp159=( int) _temp151.hd; goto _LL153; _LL153: _temp152= _temp151.tl;
if( _temp152 == 0){ goto _LL136;} else{ _temp154=* _temp152; _LL158: _temp157=(
int) _temp154.hd; goto _LL156; _LL156: _temp155= _temp154.tl; if( _temp155 == 0){
goto _LL150;} else{ goto _LL136;}}} _LL150: if( _temp159 =='h'? _temp157 =='h':
0){ goto _LL135;} else{ goto _LL136;} _LL136: goto _LL137; _LL129: t= Cyc_Absyn_sint_t;
goto _LL127; _LL131: t= Cyc_Absyn_slong_t; goto _LL127; _LL133: t= Cyc_Absyn_sshort_t;
goto _LL127; _LL135: t= Cyc_Absyn_schar_t; goto _LL127; _LL137: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp161=( struct _tagged_arr)
Cyc_String_implode( _temp72); int _temp162=( int) _temp70; xprintf("length modifier '%.*s' is not allowed with %%%c",
_get_arr_size( _temp161, 1u), _temp161.curr, _temp162);})); return 0; _LL127:;}
_temp61=({ struct Cyc_List_List* _temp163=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp163->hd=( void*) t; _temp163->tl= _temp61;
_temp163;}); break; case 'u': _LL125:{ struct Cyc_List_List* f= _temp78; for( 0;
f != 0; f=(( struct Cyc_List_List*) _check_null( f))->tl){ if(( int)(( struct
Cyc_List_List*) _check_null( f))->hd =='#'){ Cyc_Tcutil_terr( loc, _tag_arr("Flag '#' not valid with %%u",
sizeof( unsigned char), 28u)); return 0;}}} goto _LL164; case 'o': _LL164: goto
_LL165; case 'x': _LL165: goto _LL166; case 'X': _LL166:{ struct Cyc_List_List*
_temp168= _temp72; struct Cyc_List_List _temp181; struct Cyc_List_List* _temp182;
int _temp184; struct Cyc_List_List _temp187; struct Cyc_List_List* _temp188; int
_temp190; struct Cyc_List_List _temp193; struct Cyc_List_List* _temp194; struct
Cyc_List_List _temp196; struct Cyc_List_List* _temp197; int _temp199; int
_temp201; _LL170: if( _temp168 == 0){ goto _LL171;} else{ goto _LL172;} _LL172:
if( _temp168 == 0){ goto _LL174;} else{ _temp181=* _temp168; _LL185: _temp184=(
int) _temp181.hd; goto _LL183; _LL183: _temp182= _temp181.tl; if( _temp182 == 0){
goto _LL180;} else{ goto _LL174;}} _LL180: if( _temp184 =='l'){ goto _LL173;}
else{ goto _LL174;} _LL174: if( _temp168 == 0){ goto _LL176;} else{ _temp187=*
_temp168; _LL191: _temp190=( int) _temp187.hd; goto _LL189; _LL189: _temp188=
_temp187.tl; if( _temp188 == 0){ goto _LL186;} else{ goto _LL176;}} _LL186: if(
_temp190 =='h'){ goto _LL175;} else{ goto _LL176;} _LL176: if( _temp168 == 0){
goto _LL178;} else{ _temp193=* _temp168; _LL202: _temp201=( int) _temp193.hd;
goto _LL195; _LL195: _temp194= _temp193.tl; if( _temp194 == 0){ goto _LL178;}
else{ _temp196=* _temp194; _LL200: _temp199=( int) _temp196.hd; goto _LL198;
_LL198: _temp197= _temp196.tl; if( _temp197 == 0){ goto _LL192;} else{ goto
_LL178;}}} _LL192: if( _temp201 =='h'? _temp199 =='h': 0){ goto _LL177;} else{
goto _LL178;} _LL178: goto _LL179; _LL171: t= Cyc_Absyn_uint_t; goto _LL169;
_LL173: t= Cyc_Absyn_ulong_t; goto _LL169; _LL175: t= Cyc_Absyn_ushort_t; goto
_LL169; _LL177: t= Cyc_Absyn_uchar_t; goto _LL169; _LL179: Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp203=( struct _tagged_arr) Cyc_String_implode(
_temp72); int _temp204=( int) _temp70; xprintf("length modifier '%.*s' is not allowed with %%%c",
_get_arr_size( _temp203, 1u), _temp203.curr, _temp204);})); return 0; _LL169:;}
_temp61=({ struct Cyc_List_List* _temp205=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp205->hd=( void*) t; _temp205->tl= _temp61;
_temp205;}); break; case 'f': _LL167: goto _LL206; case 'F': _LL206: goto _LL207;
case 'e': _LL207: goto _LL208; case 'E': _LL208: goto _LL209; case 'g': _LL209:
goto _LL210; case 'G': _LL210: goto _LL211; case 'a': _LL211: goto _LL212; case
'A': _LL212: if( _temp72 != 0){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)
xprintf("length modifiers are not allowed with %%%c",( int) _temp70)); return 0;}
_temp61=({ struct Cyc_List_List* _temp214=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp214->hd=( void*) Cyc_Absyn_double_t;
_temp214->tl= _temp61; _temp214;}); break; case 'c': _LL213:{ struct Cyc_List_List*
f= _temp78; for( 0; f != 0; f=(( struct Cyc_List_List*) _check_null( f))->tl){
if(( int)(( struct Cyc_List_List*) _check_null( f))->hd =='#'? 1:( int)(( struct
Cyc_List_List*) _check_null( f))->hd =='0'){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr) xprintf("flag '%c' not allowed with %%c",( int)(( struct Cyc_List_List*)
_check_null( f))->hd)); return 0;}}} if( _temp72 != 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp216=( struct _tagged_arr) Cyc_String_implode(
_temp72); xprintf("length modifier '%.*s' not allowed with %%c", _get_arr_size(
_temp216, 1u), _temp216.curr);})); return 0;} if( _temp74 != 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp217=( struct _tagged_arr)
Cyc_String_implode( _temp74); xprintf("precision '%.*s' not allowed with %%c",
_get_arr_size( _temp217, 1u), _temp217.curr);})); return 0;} _temp61=({ struct
Cyc_List_List* _temp218=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp218->hd=( void*) Cyc_Absyn_sint_t; _temp218->tl= _temp61; _temp218;});
break; case 's': _LL215: if( _temp78 != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp220=( struct _tagged_arr) Cyc_String_implode(
_temp78); xprintf("flags '%.*s' not allowed with %%s", _get_arr_size( _temp220,
1u), _temp220.curr);})); return 0;} if( _temp76 != 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp221=( struct _tagged_arr) Cyc_String_implode(
_temp76); xprintf("width '%.*s' not allowed with %%s", _get_arr_size( _temp221,
1u), _temp221.curr);})); return 0;} if( _temp74 != 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp222=( struct _tagged_arr) Cyc_String_implode(
_temp74); xprintf("precision '%.*s' not allowed with %%s", _get_arr_size(
_temp222, 1u), _temp222.curr);})); return 0;} if( _temp72 != 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp223=( struct _tagged_arr)
Cyc_String_implode( _temp72); xprintf("length modifier '%.*s' not allowed with %%s",
_get_arr_size( _temp223, 1u), _temp223.curr);})); return 0;} _temp61=({ struct
Cyc_List_List* _temp224=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp224->hd=( void*) Cyc_Absyn_const_string_typ( Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Formatstr_rk,({ struct Cyc_Core_Opt* _temp225=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp225->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp225;}))); _temp224->tl= _temp61; _temp224;}); break; case 'p': _LL219:
Cyc_Tcutil_terr( loc, _tag_arr("%%p not supported", sizeof( unsigned char), 18u));
return 0; case 'n': _LL226:{ struct Cyc_List_List* f= _temp78; for( 0; f != 0; f=((
struct Cyc_List_List*) _check_null( f))->tl){ if(( int)(( struct Cyc_List_List*)
_check_null( f))->hd =='#'? 1:( int)(( struct Cyc_List_List*) _check_null( f))->hd
=='0'){ Cyc_Tcutil_terr( loc,( struct _tagged_arr) xprintf("flag '%c' not allowed with %%n",(
int)(( struct Cyc_List_List*) _check_null( f))->hd)); return 0;}}} if( _temp74
!= 0){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp228=(
struct _tagged_arr) Cyc_String_implode( _temp74); xprintf("precision '%.*s' not allowed with %%n",
_get_arr_size( _temp228, 1u), _temp228.curr);})); return 0;}{ struct Cyc_Core_Opt*
_temp230=({ struct Cyc_Core_Opt* _temp229=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp229->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp229;}); goto _LL231; _LL231:{ struct Cyc_List_List* _temp232= _temp72;
struct Cyc_List_List _temp245; struct Cyc_List_List* _temp246; int _temp248;
struct Cyc_List_List _temp251; struct Cyc_List_List* _temp252; int _temp254;
struct Cyc_List_List _temp257; struct Cyc_List_List* _temp258; struct Cyc_List_List
_temp260; struct Cyc_List_List* _temp261; int _temp263; int _temp265; _LL234:
if( _temp232 == 0){ goto _LL235;} else{ goto _LL236;} _LL236: if( _temp232 == 0){
goto _LL238;} else{ _temp245=* _temp232; _LL249: _temp248=( int) _temp245.hd;
goto _LL247; _LL247: _temp246= _temp245.tl; if( _temp246 == 0){ goto _LL244;}
else{ goto _LL238;}} _LL244: if( _temp248 =='l'){ goto _LL237;} else{ goto
_LL238;} _LL238: if( _temp232 == 0){ goto _LL240;} else{ _temp251=* _temp232;
_LL255: _temp254=( int) _temp251.hd; goto _LL253; _LL253: _temp252= _temp251.tl;
if( _temp252 == 0){ goto _LL250;} else{ goto _LL240;}} _LL250: if( _temp254 =='h'){
goto _LL239;} else{ goto _LL240;} _LL240: if( _temp232 == 0){ goto _LL242;}
else{ _temp257=* _temp232; _LL266: _temp265=( int) _temp257.hd; goto _LL259;
_LL259: _temp258= _temp257.tl; if( _temp258 == 0){ goto _LL242;} else{ _temp260=*
_temp258; _LL264: _temp263=( int) _temp260.hd; goto _LL262; _LL262: _temp261=
_temp260.tl; if( _temp261 == 0){ goto _LL256;} else{ goto _LL242;}}} _LL256: if(
_temp265 =='h'? _temp263 =='h': 0){ goto _LL241;} else{ goto _LL242;} _LL242:
goto _LL243; _LL235: t= Cyc_Absyn_at_typ( Cyc_Absyn_sint_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp230), Cyc_Absyn_empty_tqual());
goto _LL233; _LL237: t= Cyc_Absyn_at_typ( Cyc_Absyn_ulong_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp230), Cyc_Absyn_empty_tqual());
goto _LL233; _LL239: t= Cyc_Absyn_at_typ( Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp230), Cyc_Absyn_empty_tqual());
goto _LL233; _LL241: t= Cyc_Absyn_at_typ( Cyc_Absyn_schar_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp230), Cyc_Absyn_empty_tqual());
goto _LL233; _LL243: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp267=( struct _tagged_arr) Cyc_String_implode( _temp72); xprintf("length modifier '%.*s' not allowed with %%n",
_get_arr_size( _temp267, 1u), _temp267.curr);})); return 0; _LL233:;} _temp61=({
struct Cyc_List_List* _temp268=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp268->hd=( void*) t; _temp268->tl= _temp61; _temp268;});
break;} case '%': _LL227: if( _temp78 != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp270=( struct _tagged_arr) Cyc_String_implode(
_temp78); xprintf("flags '%.*s' not allowed with %%%%", _get_arr_size( _temp270,
1u), _temp270.curr);})); return 0;} if( _temp76 != 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp271=( struct _tagged_arr) Cyc_String_implode(
_temp76); xprintf("width '%.*s' not allowed with %%%%", _get_arr_size( _temp271,
1u), _temp271.curr);})); return 0;} if( _temp74 != 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp272=( struct _tagged_arr) Cyc_String_implode(
_temp74); xprintf("precision '%.*s' not allowed with %%%%", _get_arr_size(
_temp272, 1u), _temp272.curr);})); return 0;} if( _temp72 != 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp273=( struct _tagged_arr)
Cyc_String_implode( _temp72); xprintf("length modifier '%.*s' not allowed with %%%%",
_get_arr_size( _temp273, 1u), _temp273.curr);})); return 0;} break; default:
_LL269: return 0;}}}}} return(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp61);}}} struct _tuple4{ int f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; unsigned char f4; int f5; } ; struct Cyc_Core_Opt*
Cyc_Formatstr_parse_inputformat( struct _tagged_arr s, int i){ int _temp275= Cyc_String_strlen(
s); goto _LL276; _LL276: if( i < 0? 1: i >= _temp275){ return 0;}{ int _temp277=
0; goto _LL278; _LL278: { unsigned char _temp279=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); goto _LL280; _LL280:
if( _temp279 =='*'){ _temp277= 1; i ++; if( i >= _temp275){ return 0;}}{ struct
Cyc_List_List* _temp281= 0; goto _LL282; _LL282: for( 0; i < _temp275; i ++){
_temp279=*(( const unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), i)); if( isdigit(( int) _temp279)){ _temp281=({ struct Cyc_List_List*
_temp283=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp283->hd=( void*)(( int) _temp279); _temp283->tl= _temp281; _temp283;});}
else{ break;}} if( i >= _temp275){ return 0;} _temp281=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp281);{ struct Cyc_List_List*
_temp284= 0; goto _LL285; _LL285: _temp279=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); switch( _temp279){
case 'h': _LL286: _temp284=({ struct Cyc_List_List* _temp288=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp288->hd=( void*)(( int) _temp279);
_temp288->tl= _temp284; _temp288;}); i ++; if( i >= _temp275){ return 0;}
_temp279=*(( const unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), i)); if( _temp279 =='h'){ _temp284=({ struct Cyc_List_List*
_temp289=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp289->hd=( void*)(( int) _temp279); _temp289->tl= _temp284; _temp289;}); i
++;} break; case 'l': _LL287: _temp284=({ struct Cyc_List_List* _temp291=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp291->hd=(
void*)(( int) _temp279); _temp291->tl= _temp284; _temp291;}); i ++; if( i >=
_temp275){ return 0;} _temp279=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if( _temp279 =='l'){
_temp284=({ struct Cyc_List_List* _temp292=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp292->hd=( void*)(( int) _temp279); _temp292->tl=
_temp284; _temp292;}); i ++;} break; case 'j': _LL290: goto _LL293; case 'z':
_LL293: goto _LL294; case 't': _LL294: goto _LL295; case 'L': _LL295: _temp284=({
struct Cyc_List_List* _temp297=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp297->hd=( void*)(( int) _temp279); _temp297->tl=
_temp284; _temp297;}); i ++; break; default: _LL296: break;} if( i >= _temp275){
return 0;} _temp284=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp284); _temp279=*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), i)); switch( _temp279){ case 'd': _LL299: break; case 'i':
_LL300: break; case 'o': _LL301: break; case 'u': _LL302: break; case 'x':
_LL303: break; case 'X': _LL304: break; case 'f': _LL305: break; case 'F':
_LL306: break; case 'e': _LL307: break; case 'E': _LL308: break; case 'g':
_LL309: break; case 'G': _LL310: break; case 'a': _LL311: break; case 'A':
_LL312: break; case 'c': _LL313: break; case 's': _LL314: break; case 'p':
_LL315: break; case 'n': _LL316: break; case '%': _LL317: break; default: _LL318:
return 0;} return({ struct Cyc_Core_Opt* _temp320=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp320->v=( void*)({ struct _tuple4*
_temp321=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp321->f1=
_temp277; _temp321->f2= _temp281; _temp321->f3= _temp284; _temp321->f4= _temp279;
_temp321->f5= i + 1; _temp321;}); _temp320;});}}}}} struct Cyc_List_List* Cyc_Formatstr_get_scanf_typs(
struct Cyc_Tcenv_Tenv* te, struct _tagged_arr s, struct Cyc_Position_Segment*
loc){ int _temp322= Cyc_String_strlen( s); goto _LL323; _LL323: { struct Cyc_List_List*
_temp324= 0; goto _LL325; _LL325: { int i; for( i= 0; i < _temp322; i ++){ if(*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i))
!='%'){ continue;}{ struct Cyc_Core_Opt* _temp326= Cyc_Formatstr_parse_inputformat(
s, i + 1); goto _LL327; _LL327: if( _temp326 == 0){ Cyc_Tcutil_terr( loc,
_tag_arr("bad format string", sizeof( unsigned char), 18u)); return 0;}{ struct
_tuple4 _temp330; int _temp331; unsigned char _temp333; struct Cyc_List_List*
_temp335; struct Cyc_List_List* _temp337; int _temp339; struct _tuple4* _temp328=(
struct _tuple4*)(( struct Cyc_Core_Opt*) _check_null( _temp326))->v; _temp330=*
_temp328; _LL340: _temp339= _temp330.f1; goto _LL338; _LL338: _temp337= _temp330.f2;
goto _LL336; _LL336: _temp335= _temp330.f3; goto _LL334; _LL334: _temp333=
_temp330.f4; goto _LL332; _LL332: _temp331= _temp330.f5; goto _LL329; _LL329: i=
_temp331 - 1;{ struct Cyc_List_List* _temp341= _temp335; struct Cyc_List_List
_temp348; struct Cyc_List_List* _temp349; int _temp351; _LL343: if( _temp341 ==
0){ goto _LL345;} else{ _temp348=* _temp341; _LL352: _temp351=( int) _temp348.hd;
goto _LL350; _LL350: _temp349= _temp348.tl; if( _temp349 == 0){ goto _LL347;}
else{ goto _LL345;}} _LL347: if(( _temp351 =='j'? 1: _temp351 =='z')? 1:
_temp351 =='t'){ goto _LL344;} else{ goto _LL345;} _LL345: goto _LL346; _LL344:
Cyc_Tcutil_terr( loc,( struct _tagged_arr) xprintf("length modifier '%c' is not supported",
_temp351)); return 0; _LL346: goto _LL342; _LL342:;} if( _temp339){ continue;}{
void* t; switch( _temp333){ case 'd': _LL353: goto _LL354; case 'i': _LL354: {
struct Cyc_Core_Opt* _temp357=({ struct Cyc_Core_Opt* _temp356=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp356->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp356;}); goto _LL358; _LL358:{ struct Cyc_List_List* _temp359= _temp335;
struct Cyc_List_List _temp372; struct Cyc_List_List* _temp373; int _temp375;
struct Cyc_List_List _temp378; struct Cyc_List_List* _temp379; int _temp381;
struct Cyc_List_List _temp384; struct Cyc_List_List* _temp385; struct Cyc_List_List
_temp387; struct Cyc_List_List* _temp388; int _temp390; int _temp392; _LL361:
if( _temp359 == 0){ goto _LL362;} else{ goto _LL363;} _LL363: if( _temp359 == 0){
goto _LL365;} else{ _temp372=* _temp359; _LL376: _temp375=( int) _temp372.hd;
goto _LL374; _LL374: _temp373= _temp372.tl; if( _temp373 == 0){ goto _LL371;}
else{ goto _LL365;}} _LL371: if( _temp375 =='l'){ goto _LL364;} else{ goto
_LL365;} _LL365: if( _temp359 == 0){ goto _LL367;} else{ _temp378=* _temp359;
_LL382: _temp381=( int) _temp378.hd; goto _LL380; _LL380: _temp379= _temp378.tl;
if( _temp379 == 0){ goto _LL377;} else{ goto _LL367;}} _LL377: if( _temp381 =='h'){
goto _LL366;} else{ goto _LL367;} _LL367: if( _temp359 == 0){ goto _LL369;}
else{ _temp384=* _temp359; _LL393: _temp392=( int) _temp384.hd; goto _LL386;
_LL386: _temp385= _temp384.tl; if( _temp385 == 0){ goto _LL369;} else{ _temp387=*
_temp385; _LL391: _temp390=( int) _temp387.hd; goto _LL389; _LL389: _temp388=
_temp387.tl; if( _temp388 == 0){ goto _LL383;} else{ goto _LL369;}}} _LL383: if(
_temp392 =='h'? _temp390 =='h': 0){ goto _LL368;} else{ goto _LL369;} _LL369:
goto _LL370; _LL362: t= Cyc_Absyn_at_typ( Cyc_Absyn_sint_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp357), Cyc_Absyn_empty_tqual());
goto _LL360; _LL364: t= Cyc_Absyn_at_typ( Cyc_Absyn_slong_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp357), Cyc_Absyn_empty_tqual());
goto _LL360; _LL366: t= Cyc_Absyn_at_typ( Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp357), Cyc_Absyn_empty_tqual());
goto _LL360; _LL368: t= Cyc_Absyn_at_typ( Cyc_Absyn_schar_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp357), Cyc_Absyn_empty_tqual());
goto _LL360; _LL370: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp394=( struct _tagged_arr) Cyc_String_implode( _temp335); int
_temp395=( int) _temp333; xprintf("length modifier '%.*s' is not allowed with %%%c",
_get_arr_size( _temp394, 1u), _temp394.curr, _temp395);})); return 0; _LL360:;}
_temp324=({ struct Cyc_List_List* _temp396=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp396->hd=( void*) t; _temp396->tl= _temp324;
_temp396;}); break;} case 'u': _LL355: goto _LL397; case 'o': _LL397: goto
_LL398; case 'x': _LL398: goto _LL399; case 'X': _LL399: { struct Cyc_Core_Opt*
_temp402=({ struct Cyc_Core_Opt* _temp401=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp401->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp401;}); goto _LL403; _LL403:{ struct Cyc_List_List* _temp404= _temp335;
struct Cyc_List_List _temp417; struct Cyc_List_List* _temp418; int _temp420;
struct Cyc_List_List _temp423; struct Cyc_List_List* _temp424; int _temp426;
struct Cyc_List_List _temp429; struct Cyc_List_List* _temp430; struct Cyc_List_List
_temp432; struct Cyc_List_List* _temp433; int _temp435; int _temp437; _LL406:
if( _temp404 == 0){ goto _LL407;} else{ goto _LL408;} _LL408: if( _temp404 == 0){
goto _LL410;} else{ _temp417=* _temp404; _LL421: _temp420=( int) _temp417.hd;
goto _LL419; _LL419: _temp418= _temp417.tl; if( _temp418 == 0){ goto _LL416;}
else{ goto _LL410;}} _LL416: if( _temp420 =='l'){ goto _LL409;} else{ goto
_LL410;} _LL410: if( _temp404 == 0){ goto _LL412;} else{ _temp423=* _temp404;
_LL427: _temp426=( int) _temp423.hd; goto _LL425; _LL425: _temp424= _temp423.tl;
if( _temp424 == 0){ goto _LL422;} else{ goto _LL412;}} _LL422: if( _temp426 =='h'){
goto _LL411;} else{ goto _LL412;} _LL412: if( _temp404 == 0){ goto _LL414;}
else{ _temp429=* _temp404; _LL438: _temp437=( int) _temp429.hd; goto _LL431;
_LL431: _temp430= _temp429.tl; if( _temp430 == 0){ goto _LL414;} else{ _temp432=*
_temp430; _LL436: _temp435=( int) _temp432.hd; goto _LL434; _LL434: _temp433=
_temp432.tl; if( _temp433 == 0){ goto _LL428;} else{ goto _LL414;}}} _LL428: if(
_temp437 =='h'? _temp435 =='h': 0){ goto _LL413;} else{ goto _LL414;} _LL414:
goto _LL415; _LL407: t= Cyc_Absyn_at_typ( Cyc_Absyn_uint_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp402), Cyc_Absyn_empty_tqual());
goto _LL405; _LL409: t= Cyc_Absyn_at_typ( Cyc_Absyn_ulong_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp402), Cyc_Absyn_empty_tqual());
goto _LL405; _LL411: t= Cyc_Absyn_at_typ( Cyc_Absyn_ushort_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp402), Cyc_Absyn_empty_tqual());
goto _LL405; _LL413: t= Cyc_Absyn_at_typ( Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp402), Cyc_Absyn_empty_tqual());
goto _LL405; _LL415: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp439=( struct _tagged_arr) Cyc_String_implode( _temp335); int
_temp440=( int) _temp333; xprintf("length modifier '%.*s' is not allowed with %%%c",
_get_arr_size( _temp439, 1u), _temp439.curr, _temp440);})); return 0; _LL405:;}
_temp324=({ struct Cyc_List_List* _temp441=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp441->hd=( void*) t; _temp441->tl= _temp324;
_temp441;}); break;} case 'f': _LL400: goto _LL442; case 'F': _LL442: goto
_LL443; case 'e': _LL443: goto _LL444; case 'E': _LL444: goto _LL445; case 'g':
_LL445: goto _LL446; case 'G': _LL446: goto _LL447; case 'a': _LL447: goto
_LL448; case 'A': _LL448: { struct Cyc_Core_Opt* _temp451=({ struct Cyc_Core_Opt*
_temp450=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp450->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp450;}); goto _LL452;
_LL452:{ struct Cyc_List_List* _temp453= _temp335; struct Cyc_List_List _temp462;
struct Cyc_List_List* _temp463; int _temp465; _LL455: if( _temp453 == 0){ goto
_LL456;} else{ goto _LL457;} _LL457: if( _temp453 == 0){ goto _LL459;} else{
_temp462=* _temp453; _LL466: _temp465=( int) _temp462.hd; goto _LL464; _LL464:
_temp463= _temp462.tl; if( _temp463 == 0){ goto _LL461;} else{ goto _LL459;}}
_LL461: if( _temp465 =='l'){ goto _LL458;} else{ goto _LL459;} _LL459: goto
_LL460; _LL456: t= Cyc_Absyn_at_typ( Cyc_Absyn_float_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp451), Cyc_Absyn_empty_tqual());
goto _LL454; _LL458: t= Cyc_Absyn_at_typ( Cyc_Absyn_double_t, Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk, _temp451), Cyc_Absyn_empty_tqual());
goto _LL454; _LL460: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp467=( struct _tagged_arr) Cyc_String_implode( _temp335); int
_temp468=( int) _temp333; xprintf("length modifier '%.*s' is not allowed with %%%c",
_get_arr_size( _temp467, 1u), _temp467.curr, _temp468);})); return 0; _LL454:;}
_temp324=({ struct Cyc_List_List* _temp469=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp469->hd=( void*) t; _temp469->tl= _temp324;
_temp469;}); break;} case 'c': _LL449: Cyc_Tcutil_terr( loc, _tag_arr("%%c is not supported",
sizeof( unsigned char), 21u)); return 0; case 's': _LL470: Cyc_Tcutil_terr( loc,
_tag_arr("%%s is not supported", sizeof( unsigned char), 21u)); return 0; case '[':
_LL471: Cyc_Tcutil_terr( loc, _tag_arr("%%[ is not supported", sizeof(
unsigned char), 21u)); return 0; case 'p': _LL472: Cyc_Tcutil_terr( loc,
_tag_arr("%%p is not supported", sizeof( unsigned char), 21u)); return 0; case 'n':
_LL473: { struct Cyc_Core_Opt* _temp476=({ struct Cyc_Core_Opt* _temp475=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp475->v=(
void*) Cyc_Tcenv_lookup_type_vars( te); _temp475;}); goto _LL477; _LL477:{
struct Cyc_List_List* _temp478= _temp335; struct Cyc_List_List _temp491; struct
Cyc_List_List* _temp492; int _temp494; struct Cyc_List_List _temp497; struct Cyc_List_List*
_temp498; int _temp500; struct Cyc_List_List _temp503; struct Cyc_List_List*
_temp504; struct Cyc_List_List _temp506; struct Cyc_List_List* _temp507; int
_temp509; int _temp511; _LL480: if( _temp478 == 0){ goto _LL481;} else{ goto
_LL482;} _LL482: if( _temp478 == 0){ goto _LL484;} else{ _temp491=* _temp478;
_LL495: _temp494=( int) _temp491.hd; goto _LL493; _LL493: _temp492= _temp491.tl;
if( _temp492 == 0){ goto _LL490;} else{ goto _LL484;}} _LL490: if( _temp494 =='l'){
goto _LL483;} else{ goto _LL484;} _LL484: if( _temp478 == 0){ goto _LL486;}
else{ _temp497=* _temp478; _LL501: _temp500=( int) _temp497.hd; goto _LL499;
_LL499: _temp498= _temp497.tl; if( _temp498 == 0){ goto _LL496;} else{ goto
_LL486;}} _LL496: if( _temp500 =='h'){ goto _LL485;} else{ goto _LL486;} _LL486:
if( _temp478 == 0){ goto _LL488;} else{ _temp503=* _temp478; _LL512: _temp511=(
int) _temp503.hd; goto _LL505; _LL505: _temp504= _temp503.tl; if( _temp504 == 0){
goto _LL488;} else{ _temp506=* _temp504; _LL510: _temp509=( int) _temp506.hd;
goto _LL508; _LL508: _temp507= _temp506.tl; if( _temp507 == 0){ goto _LL502;}
else{ goto _LL488;}}} _LL502: if( _temp511 =='h'? _temp509 =='h': 0){ goto
_LL487;} else{ goto _LL488;} _LL488: goto _LL489; _LL481: t= Cyc_Absyn_at_typ(
Cyc_Absyn_sint_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp476), Cyc_Absyn_empty_tqual()); goto _LL479; _LL483: t= Cyc_Absyn_at_typ(
Cyc_Absyn_ulong_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp476), Cyc_Absyn_empty_tqual()); goto _LL479; _LL485: t= Cyc_Absyn_at_typ(
Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp476), Cyc_Absyn_empty_tqual()); goto _LL479; _LL487: t= Cyc_Absyn_at_typ(
Cyc_Absyn_schar_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_temp476), Cyc_Absyn_empty_tqual()); goto _LL479; _LL489: Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp513=( struct _tagged_arr) Cyc_String_implode(
_temp335); xprintf("length modifier '%.*s' not allowed with %%n", _get_arr_size(
_temp513, 1u), _temp513.curr);})); return 0; _LL479:;} _temp324=({ struct Cyc_List_List*
_temp514=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp514->hd=( void*) t; _temp514->tl= _temp324; _temp514;}); break;} case '%':
_LL474: if( _temp339){ Cyc_Tcutil_terr( loc, _tag_arr("Assignment suppression (*) is not allowed with %%%%",
sizeof( unsigned char), 52u)); return 0;} if( _temp337 != 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp516=( struct _tagged_arr)
Cyc_String_implode( _temp337); xprintf("width '%.*s' not allowed with %%%%",
_get_arr_size( _temp516, 1u), _temp516.curr);})); return 0;} if( _temp335 != 0){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp517=(
struct _tagged_arr) Cyc_String_implode( _temp335); xprintf("length modifier '%.*s' not allowed with %%%%",
_get_arr_size( _temp517, 1u), _temp517.curr);})); return 0;} break; default:
_LL515: return 0;}}}}} return(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp324);}}}