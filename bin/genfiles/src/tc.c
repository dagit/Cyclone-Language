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
unsigned char* tag; struct _tagged_arr f1; } ; extern int Cyc_String_strcmp(
struct _tagged_arr s1, struct _tagged_arr s2); extern int Cyc_String_strptrcmp(
struct _tagged_arr* s1, struct _tagged_arr* s2); extern int Cyc_String_zstrptrcmp(
struct _tagged_arr*, struct _tagged_arr*); struct Cyc_Lineno_Pos{ struct
_tagged_arr logical_file; struct _tagged_arr line; int line_no; int col; } ;
extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment; static
const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1; static
const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct _tagged_arr
source; struct Cyc_Position_Segment* seg; void* kind; struct _tagged_arr desc; }
; extern unsigned char Cyc_Position_Nocontext[ 14u]; struct _tuple0{ void* f1;
struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual;
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
_tagged_arr)({ struct _tagged_arr _temp100= Cyc_Absyn_attribute2string(( void*)((
struct Cyc_List_List*) _check_null( atts))->hd); struct _tagged_arr _temp101=
Cyc_Absynpp_qvar2string( vd->name); xprintf("bad attribute %.*s for variable %.*s",
_get_arr_size( _temp100, 1u), _temp100.curr, _get_arr_size( _temp101, 1u),
_temp101.curr);})); goto _LL81; _LL81:;} if( vd->initializer == 0){ if(
check_var_init? ! Cyc_Tcutil_supports_default( t): 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp102= Cyc_Absynpp_qvar2string( vd->name);
struct _tagged_arr _temp103= Cyc_Absynpp_typ2string( t); xprintf("initializer required for variable %.*s of type %.*s",
_get_arr_size( _temp102, 1u), _temp102.curr, _get_arr_size( _temp103, 1u),
_temp103.curr);}));}} else{ struct Cyc_Absyn_Exp* _temp104=( struct Cyc_Absyn_Exp*)
_check_null( vd->initializer); void* _temp105= Cyc_Tcexp_tcExpInitializer( te,(
void**)& t, _temp104); if( ! Cyc_Tcutil_coerce_assign( te, _temp104, t)){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp106= Cyc_Absynpp_qvar2string(
vd->name); struct _tagged_arr _temp107= Cyc_Absynpp_typ2string( t); struct
_tagged_arr _temp108= Cyc_Absynpp_typ2string( _temp105); xprintf("%.*s is declared with type %.*s but initialized with type %.*s",
_get_arr_size( _temp106, 1u), _temp106.curr, _get_arr_size( _temp107, 1u),
_temp107.curr, _get_arr_size( _temp108, 1u), _temp108.curr);}));} if( ! Cyc_Tcutil_is_const_exp(
te, _temp104)){ Cyc_Tcutil_terr( loc, _tag_arr("initializer is not a constant expression",
sizeof( unsigned char), 41u));}}} else{ for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp109=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; _LL111: if(( unsigned int) _temp109 > 16u?*(( int*)
_temp109) == Cyc_Absyn_Regparm_att: 0){ goto _LL112;} else{ goto _LL113;} _LL113:
if( _temp109 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL114;} else{ goto _LL115;}
_LL115: if( _temp109 ==( void*) Cyc_Absyn_Cdecl_att){ goto _LL116;} else{ goto
_LL117;} _LL117: if( _temp109 ==( void*) Cyc_Absyn_Fastcall_att){ goto _LL118;}
else{ goto _LL119;} _LL119: if( _temp109 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL120;} else{ goto _LL121;} _LL121: if( _temp109 ==( void*) Cyc_Absyn_Const_att){
goto _LL122;} else{ goto _LL123;} _LL123: if(( unsigned int) _temp109 > 16u?*((
int*) _temp109) == Cyc_Absyn_Aligned_att: 0){ goto _LL124;} else{ goto _LL125;}
_LL125: if( _temp109 ==( void*) Cyc_Absyn_Packed_att){ goto _LL126;} else{ goto
_LL127;} _LL127: goto _LL128; _LL112: goto _LL114; _LL114: goto _LL116; _LL116:
goto _LL118; _LL118: goto _LL120; _LL120: goto _LL122; _LL122:(( int(*)( struct
_tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("tcVardecl: fn type attributes in function var decl",
sizeof( unsigned char), 51u)); goto _LL110; _LL124: goto _LL126; _LL126: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp129= Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd); xprintf("bad attribute %.*s in function declaration",
_get_arr_size( _temp129, 1u), _temp129.curr);})); goto _LL110; _LL128: continue;
_LL110:;}}}{ struct _handler_cons _temp130; _push_handler(& _temp130);{ int
_temp132= 0; if( setjmp( _temp130.handler)){ _temp132= 1;} if( ! _temp132){{
struct _tuple3* _temp133=(( struct _tuple3*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup)( ge->ordinaries, v); void* _temp134=(*
_temp133).f1; void* _temp144; _LL136: if(*(( int*) _temp134) == Cyc_Tcenv_VarRes){
_LL145: _temp144=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp134)->f1; goto
_LL137;} else{ goto _LL138;} _LL138: if(*(( int*) _temp134) == Cyc_Tcenv_StructRes){
goto _LL139;} else{ goto _LL140;} _LL140: if(*(( int*) _temp134) == Cyc_Tcenv_TunionRes){
goto _LL141;} else{ goto _LL142;} _LL142: if(*(( int*) _temp134) == Cyc_Tcenv_EnumRes){
goto _LL143;} else{ goto _LL135;} _LL137: { struct Cyc_Absyn_Global_b_struct*
_temp146=({ struct Cyc_Absyn_Global_b_struct* _temp151=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp151[ 0]=({ struct
Cyc_Absyn_Global_b_struct _temp152; _temp152.tag= Cyc_Absyn_Global_b; _temp152.f1=
vd; _temp152;}); _temp151;}); void* _temp147= Cyc_Tcdecl_merge_binding( _temp144,(
void*) _temp146, loc, Cyc_Tc_tc_msg); if( _temp147 ==( void*) Cyc_Absyn_Unresolved_b){
goto _LL135;} if( _temp147 == _temp144?(* _temp133).f2: 0){ goto _LL135;} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple3*
_temp148=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp148->f1=(
void*)({ struct Cyc_Tcenv_VarRes_struct* _temp149=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp149[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp150; _temp150.tag= Cyc_Tcenv_VarRes; _temp150.f1=( void*) _temp147;
_temp150;}); _temp149;}); _temp148->f2= 1; _temp148;})); goto _LL135;} _LL139:
Cyc_Tcutil_warn( loc, _tag_arr("variable declaration shadows previous struct declaration",
sizeof( unsigned char), 57u)); goto _LL135; _LL141: Cyc_Tcutil_warn( loc,
_tag_arr("variable declaration shadows previous [x]tunion constructor", sizeof(
unsigned char), 60u)); goto _LL135; _LL143: Cyc_Tcutil_warn( loc, _tag_arr("variable declaration shadows previous enum tag",
sizeof( unsigned char), 47u)); goto _LL135; _LL135:;}; _pop_handler();} else{
void* _temp131=( void*) _exn_thrown; void* _temp154= _temp131; _LL156: if(
_temp154 == Cyc_Dict_Absent){ goto _LL157;} else{ goto _LL158;} _LL158: goto
_LL159; _LL157: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,
v,({ struct _tuple3* _temp160=( struct _tuple3*) GC_malloc( sizeof( struct
_tuple3)); _temp160->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp161=(
struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp161[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp162; _temp162.tag= Cyc_Tcenv_VarRes;
_temp162.f1=( void*)(( void*)({ struct Cyc_Absyn_Global_b_struct* _temp163=(
struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp163[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp164; _temp164.tag= Cyc_Absyn_Global_b;
_temp164.f1= vd; _temp164;}); _temp163;})); _temp162;}); _temp161;}); _temp160->f2=
0; _temp160;})); goto _LL155; _LL159:( void) _throw( _temp154); _LL155:;}}}}
static void Cyc_Tc_tcFndecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl* fd){ struct
_tagged_arr* v=(* fd->name).f2; void* sc=( void*) fd->sc;{ void* _temp165=(* fd->name).f1;
struct Cyc_List_List* _temp173; struct Cyc_List_List* _temp175; _LL167: if((
unsigned int) _temp165 > 1u?*(( int*) _temp165) == Cyc_Absyn_Rel_n: 0){ _LL174:
_temp173=(( struct Cyc_Absyn_Rel_n_struct*) _temp165)->f1; if( _temp173 == 0){
goto _LL168;} else{ goto _LL169;}} else{ goto _LL169;} _LL169: if(( unsigned int)
_temp165 > 1u?*(( int*) _temp165) == Cyc_Absyn_Abs_n: 0){ _LL176: _temp175=((
struct Cyc_Absyn_Abs_n_struct*) _temp165)->f1; goto _LL170;} else{ goto _LL171;}
_LL171: goto _LL172; _LL168: goto _LL166; _LL170:( int) _throw(( void*)({ struct
Cyc_Core_Impossible_struct* _temp177=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp177[ 0]=({ struct
Cyc_Core_Impossible_struct _temp178; _temp178.tag= Cyc_Core_Impossible; _temp178.f1=
_tag_arr("tc: Abs_n in tcFndecl", sizeof( unsigned char), 22u); _temp178;});
_temp177;})); _LL172: Cyc_Tcutil_terr( loc, _tag_arr("qualified declarations are not implemented",
sizeof( unsigned char), 43u)); return; _LL166:;}(* fd->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp179=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp179[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp180; _temp180.tag= Cyc_Absyn_Abs_n; _temp180.f1= te->ns; _temp180;});
_temp179;}); Cyc_Tcutil_check_fndecl_valid_type( loc, te, fd);{ void* t= Cyc_Tcutil_fndecl2typ(
fd);{ struct Cyc_List_List* atts= fd->attributes; for( 0; atts != 0; atts=((
struct Cyc_List_List*) _check_null( atts))->tl){ void* _temp181=( void*)((
struct Cyc_List_List*) _check_null( atts))->hd; _LL183: if( _temp181 ==( void*)
Cyc_Absyn_Packed_att){ goto _LL184;} else{ goto _LL185;} _LL185: if((
unsigned int) _temp181 > 16u?*(( int*) _temp181) == Cyc_Absyn_Aligned_att: 0){
goto _LL186;} else{ goto _LL187;} _LL187: goto _LL188; _LL184: goto _LL186;
_LL186: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp189=
Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*) _check_null( atts))->hd);
xprintf("bad attribute %.*s for function", _get_arr_size( _temp189, 1u),
_temp189.curr);})); goto _LL182; _LL188: goto _LL182; _LL182:;}}{ struct
_handler_cons _temp190; _push_handler(& _temp190);{ int _temp192= 0; if( setjmp(
_temp190.handler)){ _temp192= 1;} if( ! _temp192){{ struct _tuple3* _temp193=((
struct _tuple3*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
ge->ordinaries, v); void* _temp194=(* _temp193).f1; void* _temp204; _LL196: if(*((
int*) _temp194) == Cyc_Tcenv_VarRes){ _LL205: _temp204=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp194)->f1; goto _LL197;} else{ goto _LL198;} _LL198: if(*(( int*) _temp194)
== Cyc_Tcenv_StructRes){ goto _LL199;} else{ goto _LL200;} _LL200: if(*(( int*)
_temp194) == Cyc_Tcenv_TunionRes){ goto _LL201;} else{ goto _LL202;} _LL202: if(*((
int*) _temp194) == Cyc_Tcenv_EnumRes){ goto _LL203;} else{ goto _LL195;} _LL197: {
struct Cyc_Absyn_Funname_b_struct* _temp206=({ struct Cyc_Absyn_Funname_b_struct*
_temp211=( struct Cyc_Absyn_Funname_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct));
_temp211[ 0]=({ struct Cyc_Absyn_Funname_b_struct _temp212; _temp212.tag= Cyc_Absyn_Funname_b;
_temp212.f1= fd; _temp212;}); _temp211;}); void* _temp207= Cyc_Tcdecl_merge_binding(
_temp204,( void*) _temp206, loc, Cyc_Tc_tc_msg); if( _temp207 ==( void*) Cyc_Absyn_Unresolved_b){
goto _LL195;} if( _temp207 == _temp204?(* _temp193).f2: 0){ goto _LL195;} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple3*
_temp208=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp208->f1=(
void*)({ struct Cyc_Tcenv_VarRes_struct* _temp209=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp209[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp210; _temp210.tag= Cyc_Tcenv_VarRes; _temp210.f1=( void*) _temp207;
_temp210;}); _temp209;}); _temp208->f2= 1; _temp208;})); goto _LL195;} _LL199:
Cyc_Tcutil_warn( loc, _tag_arr("function declaration shadows previous struct declaration",
sizeof( unsigned char), 57u)); goto _LL195; _LL201: Cyc_Tcutil_warn( loc,
_tag_arr("function declaration shadows previous [x]tunion constructor", sizeof(
unsigned char), 60u)); goto _LL195; _LL203: Cyc_Tcutil_warn( loc, _tag_arr("function declaration shadows previous enum tag",
sizeof( unsigned char), 47u)); goto _LL195; _LL195:;}; _pop_handler();} else{
void* _temp191=( void*) _exn_thrown; void* _temp214= _temp191; _LL216: if(
_temp214 == Cyc_Dict_Absent){ goto _LL217;} else{ goto _LL218;} _LL218: goto
_LL219; _LL217: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,
v,({ struct _tuple3* _temp220=( struct _tuple3*) GC_malloc( sizeof( struct
_tuple3)); _temp220->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp221=(
struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp221[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp222; _temp222.tag= Cyc_Tcenv_VarRes;
_temp222.f1=( void*)(( void*)({ struct Cyc_Absyn_Funname_b_struct* _temp223=(
struct Cyc_Absyn_Funname_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct));
_temp223[ 0]=({ struct Cyc_Absyn_Funname_b_struct _temp224; _temp224.tag= Cyc_Absyn_Funname_b;
_temp224.f1= fd; _temp224;}); _temp223;})); _temp222;}); _temp221;}); _temp220->f2=
0; _temp220;})); goto _LL215; _LL219:( void) _throw( _temp214); _LL215:;}}} te->le=({
struct Cyc_Core_Opt* _temp225=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp225->v=( void*)(( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp226=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp226[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp227; _temp227.tag= Cyc_Tcenv_Outermost;
_temp227.f1=( void*) Cyc_Tcenv_new_fenv( loc, fd); _temp227;}); _temp226;}));
_temp225;}); Cyc_Tcstmt_tcStmt( te, fd->body, 0); if( ! Cyc_Tcenv_all_labels_resolved(
te)){ Cyc_Tcutil_terr( loc, _tag_arr("function has goto statements to undefined labels",
sizeof( unsigned char), 49u));} te->le= 0;}} static void Cyc_Tc_tcTypedefdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Typedefdecl* td){ struct _tagged_arr* v=(* td->name).f2;{
void* _temp228=(* td->name).f1; struct Cyc_List_List* _temp236; struct Cyc_List_List*
_temp238; _LL230: if(( unsigned int) _temp228 > 1u?*(( int*) _temp228) == Cyc_Absyn_Rel_n:
0){ _LL237: _temp236=(( struct Cyc_Absyn_Rel_n_struct*) _temp228)->f1; if(
_temp236 == 0){ goto _LL231;} else{ goto _LL232;}} else{ goto _LL232;} _LL232:
if(( unsigned int) _temp228 > 1u?*(( int*) _temp228) == Cyc_Absyn_Abs_n: 0){
_LL239: _temp238=(( struct Cyc_Absyn_Abs_n_struct*) _temp228)->f1; if( _temp238
== 0){ goto _LL233;} else{ goto _LL234;}} else{ goto _LL234;} _LL234: goto
_LL235; _LL231: goto _LL229; _LL233: goto _LL229; _LL235: Cyc_Tcutil_terr( loc,
_tag_arr("qualified declarations are not implemented", sizeof( unsigned char),
43u)); return; _LL229:;} if((( int(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_member)( ge->typedefs, v)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp240=* v; xprintf("redeclaration of typedef %.*s",
_get_arr_size( _temp240, 1u), _temp240.curr);})); return;}(* td->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp241=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp241[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp242; _temp242.tag= Cyc_Absyn_Abs_n; _temp242.f1= te->ns; _temp242;});
_temp241;}); Cyc_Tcutil_check_unique_tvars( loc, td->tvs); Cyc_Tcutil_add_tvar_identities(
td->tvs); Cyc_Tcutil_check_type( loc, te, td->tvs,( void*) Cyc_Absyn_AnyKind,(
void*) td->defn); ge->typedefs=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Typedefdecl* data)) Cyc_Dict_insert)(
ge->typedefs, v, td);} static void Cyc_Tc_tcStructFields( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct
_tagged_arr obj, struct Cyc_List_List* fields, struct Cyc_List_List* tvs){
struct _RegionHandle _temp243= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp243; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0;
struct Cyc_List_List* _temp244= fields; for( 0; _temp244 != 0; _temp244=((
struct Cyc_List_List*) _check_null( _temp244))->tl){ struct Cyc_Absyn_Structfield
_temp247; struct Cyc_List_List* _temp248; struct Cyc_Core_Opt* _temp250; void*
_temp252; struct Cyc_Absyn_Tqual _temp254; struct _tagged_arr* _temp256; struct
Cyc_Absyn_Structfield* _temp245=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp244))->hd; _temp247=* _temp245; _LL257: _temp256= _temp247.name;
goto _LL255; _LL255: _temp254= _temp247.tq; goto _LL253; _LL253: _temp252=( void*)
_temp247.type; goto _LL251; _LL251: _temp250= _temp247.width; goto _LL249;
_LL249: _temp248= _temp247.attributes; goto _LL246; _LL246: if((( int(*)( int(*
compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l,
struct _tagged_arr* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,
_temp256)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr
_temp258=* _temp256; struct _tagged_arr _temp259= obj; xprintf("duplicate field %.*s in %.*s",
_get_arr_size( _temp258, 1u), _temp258.curr, _get_arr_size( _temp259, 1u),
_temp259.curr);}));} if( Cyc_String_strcmp(* _temp256, _tag_arr("", sizeof(
unsigned char), 1u)) != 0){ prev_fields=({ struct Cyc_List_List* _temp260=(
struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp260->hd=( void*) _temp256; _temp260->tl= prev_fields; _temp260;});} Cyc_Tcutil_check_type(
loc, te, tvs,( void*) Cyc_Absyn_MemKind, _temp252); if( _temp250 != 0){
unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp250))->v)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp261=* _temp256; xprintf("bitfield %.*s does not have constant width",
_get_arr_size( _temp261, 1u), _temp261.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp250))->v);}{
void* _temp262= Cyc_Tcutil_compress( _temp252); void* _temp268; _LL264: if((
unsigned int) _temp262 > 4u?*(( int*) _temp262) == Cyc_Absyn_IntType: 0){ _LL269:
_temp268=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp262)->f2; goto _LL265;}
else{ goto _LL266;} _LL266: goto _LL267; _LL265:{ void* _temp270= _temp268;
_LL272: if( _temp270 ==( void*) Cyc_Absyn_B1){ goto _LL273;} else{ goto _LL274;}
_LL274: if( _temp270 ==( void*) Cyc_Absyn_B2){ goto _LL275;} else{ goto _LL276;}
_LL276: if( _temp270 ==( void*) Cyc_Absyn_B4){ goto _LL277;} else{ goto _LL278;}
_LL278: if( _temp270 ==( void*) Cyc_Absyn_B8){ goto _LL279;} else{ goto _LL271;}
_LL273: if( w > 8){ Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u));} goto _LL271; _LL275: if( w > 16){ Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u));} goto
_LL271; _LL277: if( w > 32){ Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u));} goto _LL271; _LL279: if( w > 64){ Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u));} goto
_LL271; _LL271:;} goto _LL263; _LL267: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr _temp280=* _temp256; struct _tagged_arr _temp281= Cyc_Absynpp_typ2string(
_temp252); xprintf("bitfield %.*s must have integral type but has type %.*s",
_get_arr_size( _temp280, 1u), _temp280.curr, _get_arr_size( _temp281, 1u),
_temp281.curr);})); goto _LL263; _LL263:;}}}}; _pop_region( uprev_rgn);} static
void Cyc_Tc_tcStructOrUniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct _tagged_arr obj, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl*
sd){ struct _tagged_arr* _temp282=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v)).f2; if( sd->name == 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp283= obj; xprintf("anonymous %.*ss are not allowed at top level",
_get_arr_size( _temp283, 1u), _temp283.curr);})); return;}{ struct Cyc_List_List*
atts= sd->attributes; for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp284=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; _LL286: if( _temp284 ==( void*) Cyc_Absyn_Packed_att){
goto _LL287;} else{ goto _LL288;} _LL288: if(( unsigned int) _temp284 > 16u?*((
int*) _temp284) == Cyc_Absyn_Aligned_att: 0){ goto _LL289;} else{ goto _LL290;}
_LL290: goto _LL291; _LL287: continue; _LL289: continue; _LL291: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp292= Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd); struct _tagged_arr
_temp293= obj; struct _tagged_arr _temp294=* _temp282; xprintf("bad attribute %.*s in %.*s %.*s definition",
_get_arr_size( _temp292, 1u), _temp292.curr, _get_arr_size( _temp293, 1u),
_temp293.curr, _get_arr_size( _temp294, 1u), _temp294.curr);})); goto _LL285;
_LL285:;}}{ struct Cyc_List_List* tvs= sd->tvs; for( 0; tvs != 0; tvs=(( struct
Cyc_List_List*) _check_null( tvs))->tl){ struct Cyc_Absyn_Conref* _temp295= Cyc_Absyn_compress_conref(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
void* _temp296=( void*) _temp295->v; void* _temp304; _LL298: if( _temp296 ==(
void*) Cyc_Absyn_No_constr){ goto _LL299;} else{ goto _LL300;} _LL300: if((
unsigned int) _temp296 > 1u?*(( int*) _temp296) == Cyc_Absyn_Eq_constr: 0){
_LL305: _temp304=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp296)->f1;
if( _temp304 ==( void*) Cyc_Absyn_MemKind){ goto _LL301;} else{ goto _LL302;}}
else{ goto _LL302;} _LL302: goto _LL303; _LL299:( void*)( _temp295->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp306=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp306[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp307; _temp307.tag= Cyc_Absyn_Eq_constr; _temp307.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp307;}); _temp306;}))); continue; _LL301:
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp308= obj;
struct _tagged_arr _temp309=* _temp282; struct _tagged_arr _temp310=*(( struct
Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->name; xprintf("%.*s %.*s attempts to abstract type variable %.*s of kind M",
_get_arr_size( _temp308, 1u), _temp308.curr, _get_arr_size( _temp309, 1u),
_temp309.curr, _get_arr_size( _temp310, 1u), _temp310.curr);})); continue;
_LL303: continue; _LL297:;}}{ void* _temp311=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v)).f1; struct Cyc_List_List* _temp319; struct Cyc_List_List*
_temp321; _LL313: if(( unsigned int) _temp311 > 1u?*(( int*) _temp311) == Cyc_Absyn_Rel_n:
0){ _LL320: _temp319=(( struct Cyc_Absyn_Rel_n_struct*) _temp311)->f1; if(
_temp319 == 0){ goto _LL314;} else{ goto _LL315;}} else{ goto _LL315;} _LL315:
if(( unsigned int) _temp311 > 1u?*(( int*) _temp311) == Cyc_Absyn_Abs_n: 0){
_LL322: _temp321=(( struct Cyc_Absyn_Abs_n_struct*) _temp311)->f1; if( _temp321
== 0){ goto _LL316;} else{ goto _LL317;}} else{ goto _LL317;} _LL317: goto
_LL318; _LL314: goto _LL312; _LL316: goto _LL312; _LL318: Cyc_Tcutil_terr( loc,
_tag_arr("qualified declarations are not implemented", sizeof( unsigned char),
43u)); return; _LL312:;}(*(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v)).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp323=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp323[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp324; _temp324.tag= Cyc_Absyn_Abs_n;
_temp324.f1= te->ns; _temp324;}); _temp323;}); Cyc_Tcutil_check_unique_tvars(
loc, sd->tvs); Cyc_Tcutil_add_tvar_identities( sd->tvs);} struct _tuple4{ struct
Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ; static void Cyc_Tc_tcStructdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Structdecl* sd){ struct _tagged_arr* _temp325=(*(( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)).f2; const
unsigned char* _temp326="struct"; Cyc_Tc_tcStructOrUniondecl( te, ge, _tag_arr(
_temp326, sizeof( unsigned char), 7u), loc, sd);{ struct Cyc_List_List* _temp327=
sd->tvs;{ struct _tuple4 _temp329=({ struct _tuple4 _temp328; _temp328.f1= sd->fields;
_temp328.f2=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup_opt)( ge->structdecls, _temp325); _temp328;});
struct Cyc_Core_Opt* _temp339; struct Cyc_Core_Opt* _temp341; struct Cyc_Core_Opt*
_temp343; struct Cyc_Core_Opt* _temp345; struct Cyc_Core_Opt _temp347; struct
Cyc_List_List* _temp348; struct Cyc_Core_Opt* _temp350; struct Cyc_Core_Opt
_temp352; struct Cyc_Absyn_Structdecl** _temp353; struct Cyc_Core_Opt* _temp355;
struct Cyc_Core_Opt _temp357; struct Cyc_List_List* _temp358; struct Cyc_Core_Opt*
_temp360; struct Cyc_Core_Opt _temp362; struct Cyc_Absyn_Structdecl** _temp363;
struct Cyc_Core_Opt* _temp365; _LL331: _LL342: _temp341= _temp329.f1; if(
_temp341 == 0){ goto _LL340;} else{ goto _LL333;} _LL340: _temp339= _temp329.f2;
if( _temp339 == 0){ goto _LL332;} else{ goto _LL333;} _LL333: _LL346: _temp345=
_temp329.f1; if( _temp345 == 0){ goto _LL335;} else{ _temp347=* _temp345; _LL349:
_temp348=( struct Cyc_List_List*) _temp347.v; goto _LL344;} _LL344: _temp343=
_temp329.f2; if( _temp343 == 0){ goto _LL334;} else{ goto _LL335;} _LL335:
_LL356: _temp355= _temp329.f1; if( _temp355 == 0){ goto _LL337;} else{ _temp357=*
_temp355; _LL359: _temp358=( struct Cyc_List_List*) _temp357.v; goto _LL351;}
_LL351: _temp350= _temp329.f2; if( _temp350 == 0){ goto _LL337;} else{ _temp352=*
_temp350; _LL354: _temp353=( struct Cyc_Absyn_Structdecl**) _temp352.v; goto
_LL336;} _LL337: _LL366: _temp365= _temp329.f1; if( _temp365 == 0){ goto _LL361;}
else{ goto _LL330;} _LL361: _temp360= _temp329.f2; if( _temp360 == 0){ goto
_LL330;} else{ _temp362=* _temp360; _LL364: _temp363=( struct Cyc_Absyn_Structdecl**)
_temp362.v; goto _LL338;} _LL332: ge->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl**
data)) Cyc_Dict_insert)( ge->structdecls, _temp325,({ struct Cyc_Absyn_Structdecl**
_temp367=( struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp367[ 0]= sd; _temp367;})); goto _LL330; _LL334: { struct Cyc_Absyn_Structdecl**
_temp368=({ struct Cyc_Absyn_Structdecl** _temp369=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp369[ 0]=({ struct Cyc_Absyn_Structdecl*
_temp370=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp370->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp370->name= sd->name;
_temp370->tvs= _temp327; _temp370->fields= 0; _temp370->attributes= 0; _temp370;});
_temp369;}); ge->structdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl** data)) Cyc_Dict_insert)(
ge->structdecls, _temp325, _temp368); Cyc_Tc_tcStructFields( te, ge, loc,
_tag_arr( _temp326, sizeof( unsigned char), 7u), _temp348, _temp327);* _temp368=
sd; goto _LL330;} _LL336: { struct Cyc_Absyn_Structdecl* _temp371=* _temp353;*
_temp353=({ struct Cyc_Absyn_Structdecl* _temp372=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp372->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp372->name= sd->name; _temp372->tvs= _temp327; _temp372->fields=
0; _temp372->attributes= 0; _temp372;}); Cyc_Tc_tcStructFields( te, ge, loc,
_tag_arr( _temp326, sizeof( unsigned char), 7u), _temp358, _temp327);* _temp353=
_temp371; _temp363= _temp353; goto _LL338;} _LL338: { struct Cyc_Absyn_Structdecl*
_temp373= Cyc_Tcdecl_merge_structdecl(* _temp363, sd, loc, Cyc_Tc_tc_msg); if(
_temp373 == 0){ return;} else{* _temp363=( struct Cyc_Absyn_Structdecl*)
_check_null( _temp373); sd=( struct Cyc_Absyn_Structdecl*) _check_null( _temp373);
goto _LL330;}} _LL330:;} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,
_temp325,({ struct _tuple3* _temp374=( struct _tuple3*) GC_malloc( sizeof(
struct _tuple3)); _temp374->f1=( void*)({ struct Cyc_Tcenv_StructRes_struct*
_temp375=( struct Cyc_Tcenv_StructRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_StructRes_struct));
_temp375[ 0]=({ struct Cyc_Tcenv_StructRes_struct _temp376; _temp376.tag= Cyc_Tcenv_StructRes;
_temp376.f1= sd; _temp376;}); _temp375;}); _temp374->f2= 1; _temp374;}));}}
static void Cyc_Tc_tcUniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl* ud){ struct
_tagged_arr* _temp377=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
ud->name))->v)).f2; const unsigned char* _temp378="union"; Cyc_Tc_tcStructOrUniondecl(
te, ge, _tag_arr( _temp378, sizeof( unsigned char), 6u), loc,( struct Cyc_Absyn_Structdecl*)
ud);{ struct Cyc_List_List* _temp379= ud->tvs; struct _tuple4 _temp381=({ struct
_tuple4 _temp380; _temp380.f1= ud->fields; _temp380.f2=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)( ge->uniondecls,
_temp377); _temp380;}); struct Cyc_Core_Opt* _temp391; struct Cyc_Core_Opt*
_temp393; struct Cyc_Core_Opt* _temp395; struct Cyc_Core_Opt* _temp397; struct
Cyc_Core_Opt _temp399; struct Cyc_List_List* _temp400; struct Cyc_Core_Opt*
_temp402; struct Cyc_Core_Opt _temp404; struct Cyc_Absyn_Uniondecl** _temp405;
struct Cyc_Core_Opt* _temp407; struct Cyc_Core_Opt _temp409; struct Cyc_List_List*
_temp410; struct Cyc_Core_Opt* _temp412; struct Cyc_Core_Opt _temp414; struct
Cyc_Absyn_Uniondecl** _temp415; struct Cyc_Core_Opt* _temp417; _LL383: _LL394:
_temp393= _temp381.f1; if( _temp393 == 0){ goto _LL392;} else{ goto _LL385;}
_LL392: _temp391= _temp381.f2; if( _temp391 == 0){ goto _LL384;} else{ goto
_LL385;} _LL385: _LL398: _temp397= _temp381.f1; if( _temp397 == 0){ goto _LL387;}
else{ _temp399=* _temp397; _LL401: _temp400=( struct Cyc_List_List*) _temp399.v;
goto _LL396;} _LL396: _temp395= _temp381.f2; if( _temp395 == 0){ goto _LL386;}
else{ goto _LL387;} _LL387: _LL408: _temp407= _temp381.f1; if( _temp407 == 0){
goto _LL389;} else{ _temp409=* _temp407; _LL411: _temp410=( struct Cyc_List_List*)
_temp409.v; goto _LL403;} _LL403: _temp402= _temp381.f2; if( _temp402 == 0){
goto _LL389;} else{ _temp404=* _temp402; _LL406: _temp405=( struct Cyc_Absyn_Uniondecl**)
_temp404.v; goto _LL388;} _LL389: _LL418: _temp417= _temp381.f1; if( _temp417 ==
0){ goto _LL413;} else{ goto _LL382;} _LL413: _temp412= _temp381.f2; if(
_temp412 == 0){ goto _LL382;} else{ _temp414=* _temp412; _LL416: _temp415=(
struct Cyc_Absyn_Uniondecl**) _temp414.v; goto _LL390;} _LL384: ge->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)( ge->uniondecls, _temp377,({
struct Cyc_Absyn_Uniondecl** _temp419=( struct Cyc_Absyn_Uniondecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl*)); _temp419[ 0]= ud; _temp419;})); goto
_LL382; _LL386: { struct Cyc_Absyn_Uniondecl** _temp420=({ struct Cyc_Absyn_Uniondecl**
_temp424=( struct Cyc_Absyn_Uniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl*));
_temp424[ 0]=({ struct Cyc_Absyn_Uniondecl* _temp425=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp425->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp425->name= ud->name; _temp425->tvs= _temp379; _temp425->fields=
0; _temp425->attributes= ud->attributes; _temp425;}); _temp424;}); ge->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)( ge->uniondecls, _temp377,
_temp420); Cyc_Tc_tcStructFields( te, ge, loc, _tag_arr( _temp378, sizeof(
unsigned char), 6u), _temp400, _temp379);{ struct Cyc_List_List* f= _temp400;
for( 0; f != 0; f=(( struct Cyc_List_List*) _check_null( f))->tl){ if( ! Cyc_Tcutil_bits_only((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( f))->hd)->type)){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp421=*((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( f))->hd)->name;
struct _tagged_arr _temp422=* _temp377; struct _tagged_arr _temp423= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( f))->hd)->type);
xprintf("field %.*s of union %.*s has type %.*s which is not `bits-only'",
_get_arr_size( _temp421, 1u), _temp421.curr, _get_arr_size( _temp422, 1u),
_temp422.curr, _get_arr_size( _temp423, 1u), _temp423.curr);}));}}}* _temp420=
ud; goto _LL382;} _LL388: { struct Cyc_Absyn_Uniondecl* _temp426=* _temp405;*
_temp405=({ struct Cyc_Absyn_Uniondecl* _temp427=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp427->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp427->name= ud->name; _temp427->tvs= _temp379; _temp427->fields=
0; _temp427->attributes= ud->attributes; _temp427;}); Cyc_Tc_tcStructFields( te,
ge, loc, _tag_arr( _temp378, sizeof( unsigned char), 6u), _temp410, _temp379);*
_temp405= _temp426; _temp415= _temp405; goto _LL390;} _LL390: { struct Cyc_Absyn_Uniondecl*
_temp428= Cyc_Tcdecl_merge_uniondecl(* _temp415, ud, loc, Cyc_Tc_tc_msg); if(
_temp428 == 0){ return;} else{* _temp415=( struct Cyc_Absyn_Uniondecl*)
_check_null( _temp428); goto _LL382;}} _LL382:;}} struct _tuple5{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; static struct Cyc_List_List* Cyc_Tc_tcTunionFields( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct
_tagged_arr obj, int is_xtunion, struct _tuple0* name, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs, struct Cyc_Absyn_Tuniondecl* tudres){{ struct
Cyc_List_List* _temp429= fields; for( 0; _temp429 != 0; _temp429=(( struct Cyc_List_List*)
_check_null( _temp429))->tl){ struct Cyc_Absyn_Tunionfield* _temp430=( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp429))->hd;{
struct Cyc_List_List* tvs= _temp430->tvs; for( 0; tvs != 0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){ struct Cyc_Absyn_Tvar* _temp431=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd; struct Cyc_Absyn_Conref* _temp432=
Cyc_Absyn_compress_conref( _temp431->kind); void* _temp433=( void*) _temp432->v;
void* _temp441; _LL435: if( _temp433 ==( void*) Cyc_Absyn_No_constr){ goto
_LL436;} else{ goto _LL437;} _LL437: if(( unsigned int) _temp433 > 1u?*(( int*)
_temp433) == Cyc_Absyn_Eq_constr: 0){ _LL442: _temp441=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp433)->f1; if( _temp441 ==( void*) Cyc_Absyn_MemKind){ goto _LL438;} else{
goto _LL439;}} else{ goto _LL439;} _LL439: goto _LL440; _LL436:( void*)(
_temp432->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp443=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp443[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp444; _temp444.tag= Cyc_Absyn_Eq_constr;
_temp444.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp444;}); _temp443;})));
goto _LL434; _LL438: Cyc_Tcutil_terr( _temp430->loc,( struct _tagged_arr)({
struct _tagged_arr _temp445=*(* _temp430->name).f2; xprintf("field %.*s abstracts type variable of kind M",
_get_arr_size( _temp445, 1u), _temp445.curr);})); goto _LL434; _LL440: goto
_LL434; _LL434:;}}{ struct Cyc_List_List* alltvs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tvs,
_temp430->tvs); Cyc_Tcutil_check_unique_tvars( loc, alltvs); Cyc_Tcutil_add_tvar_identities(
_temp430->tvs);{ struct Cyc_List_List* typs= _temp430->typs; for( 0; typs != 0;
typs=(( struct Cyc_List_List*) _check_null( typs))->tl){ Cyc_Tcutil_check_type(
_temp430->loc, te, alltvs,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple5*)((
struct Cyc_List_List*) _check_null( typs))->hd)).f2);}}{ void* _temp446=(*
_temp430->name).f1; struct Cyc_List_List* _temp456; _LL448: if(( unsigned int)
_temp446 > 1u?*(( int*) _temp446) == Cyc_Absyn_Rel_n: 0){ _LL457: _temp456=((
struct Cyc_Absyn_Rel_n_struct*) _temp446)->f1; if( _temp456 == 0){ goto _LL449;}
else{ goto _LL450;}} else{ goto _LL450;} _LL450: if(( unsigned int) _temp446 > 1u?*((
int*) _temp446) == Cyc_Absyn_Rel_n: 0){ goto _LL451;} else{ goto _LL452;} _LL452:
if(( unsigned int) _temp446 > 1u?*(( int*) _temp446) == Cyc_Absyn_Abs_n: 0){
goto _LL453;} else{ goto _LL454;} _LL454: if( _temp446 ==( void*) Cyc_Absyn_Loc_n){
goto _LL455;} else{ goto _LL447;} _LL449: if( is_xtunion){(* _temp430->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp458=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp458[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp459; _temp459.tag= Cyc_Absyn_Abs_n; _temp459.f1= te->ns; _temp459;});
_temp458;});} else{(* _temp430->name).f1=(* name).f1;} goto _LL447; _LL451: Cyc_Tcutil_terr(
_temp430->loc, _tag_arr("qualified declarations are not allowed", sizeof(
unsigned char), 39u)); goto _LL447; _LL453: goto _LL447; _LL455:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp460=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp460[ 0]=({ struct
Cyc_Core_Impossible_struct _temp461; _temp461.tag= Cyc_Core_Impossible; _temp461.f1=
_tag_arr("tcTunionFields: Loc_n", sizeof( unsigned char), 22u); _temp461;});
_temp460;})); goto _LL447; _LL447:;}}}}{ struct Cyc_List_List* fields2; if(
is_xtunion){ int _temp462= 1; struct Cyc_List_List* _temp463= Cyc_Tcdecl_sort_xtunion_fields(
fields,& _temp462,(* name).f2, loc, Cyc_Tc_tc_msg); if( _temp462){ fields2=
_temp463;} else{ fields2= 0;}} else{ struct _RegionHandle _temp464= _new_region();
struct _RegionHandle* uprev_rgn=& _temp464; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0;{ struct Cyc_List_List* fs= fields; for( 0; fs !=
0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){ struct Cyc_Absyn_Tunionfield*
_temp465=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fs))->hd; if((( int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*),
struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp,
prev_fields,(* _temp465->name).f2)){ Cyc_Tcutil_terr( _temp465->loc,( struct
_tagged_arr)({ struct _tagged_arr _temp466=*(* _temp465->name).f2; struct
_tagged_arr _temp467= obj; xprintf("duplicate field name %.*s in %.*s",
_get_arr_size( _temp466, 1u), _temp466.curr, _get_arr_size( _temp467, 1u),
_temp467.curr);}));} else{ prev_fields=({ struct Cyc_List_List* _temp468=(
struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp468->hd=( void*)(* _temp465->name).f2; _temp468->tl= prev_fields; _temp468;});}
if(( void*) _temp465->sc !=( void*) Cyc_Absyn_Public){ Cyc_Tcutil_warn( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp469=*(* _temp465->name).f2;
xprintf("ignoring scope of field %.*s", _get_arr_size( _temp469, 1u), _temp469.curr);}));(
void*)( _temp465->sc=( void*)(( void*) Cyc_Absyn_Public));}}} fields2= fields;};
_pop_region( uprev_rgn);}{ struct Cyc_List_List* _temp470= fields; for( 0;
_temp470 != 0; _temp470=(( struct Cyc_List_List*) _check_null( _temp470))->tl){
struct Cyc_Absyn_Tunionfield* _temp471=( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp470))->hd; ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries,(* _temp471->name).f2,({ struct _tuple3* _temp472=( struct
_tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp472->f1=( void*)({ struct
Cyc_Tcenv_TunionRes_struct* _temp473=( struct Cyc_Tcenv_TunionRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct)); _temp473[ 0]=({ struct
Cyc_Tcenv_TunionRes_struct _temp474; _temp474.tag= Cyc_Tcenv_TunionRes; _temp474.f1=
tudres; _temp474.f2= _temp471; _temp474;}); _temp473;}); _temp472->f2= 1;
_temp472;}));}} return fields2;}} static void Cyc_Tc_tcTuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl*
tud){ struct _tagged_arr* v=(* tud->name).f2; struct _tagged_arr obj; if( tud->is_xtunion){
obj= _tag_arr("xtunion", sizeof( unsigned char), 8u);} else{ obj= _tag_arr("tunion",
sizeof( unsigned char), 7u);}{ struct Cyc_List_List* tvs= tud->tvs;{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 != 0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
struct Cyc_Absyn_Conref* c= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->kind); void* _temp475=( void*)
c->v; void* _temp483; _LL477: if( _temp475 ==( void*) Cyc_Absyn_No_constr){ goto
_LL478;} else{ goto _LL479;} _LL479: if(( unsigned int) _temp475 > 1u?*(( int*)
_temp475) == Cyc_Absyn_Eq_constr: 0){ _LL484: _temp483=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp475)->f1; if( _temp483 ==( void*) Cyc_Absyn_MemKind){ goto _LL480;} else{
goto _LL481;}} else{ goto _LL481;} _LL481: goto _LL482; _LL478:( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp485=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp485[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp486; _temp486.tag= Cyc_Absyn_Eq_constr; _temp486.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp486;}); _temp485;}))); goto _LL476;
_LL480: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp487=
obj; struct _tagged_arr _temp488=* v; struct _tagged_arr _temp489=*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->name; xprintf("%.*s %.*s attempts to abstract type variable %.*s of kind M",
_get_arr_size( _temp487, 1u), _temp487.curr, _get_arr_size( _temp488, 1u),
_temp488.curr, _get_arr_size( _temp489, 1u), _temp489.curr);})); goto _LL476;
_LL482: goto _LL476; _LL476:;}} Cyc_Tcutil_check_unique_tvars( loc, tvs); Cyc_Tcutil_add_tvar_identities(
tvs);{ struct Cyc_Core_Opt* tud_opt; if( tud->is_xtunion){{ struct _handler_cons
_temp490; _push_handler(& _temp490);{ int _temp492= 0; if( setjmp( _temp490.handler)){
_temp492= 1;} if( ! _temp492){ tud_opt= Cyc_Tcenv_lookup_xtuniondecl( te, loc,
tud->name);; _pop_handler();} else{ void* _temp491=( void*) _exn_thrown; void*
_temp494= _temp491; _LL496: if( _temp494 == Cyc_Dict_Absent){ goto _LL497;}
else{ goto _LL498;} _LL498: goto _LL499; _LL497: Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp500= Cyc_Absynpp_qvar2string( tud->name);
xprintf("qualified xtunion declaration %.*s is not an existing xtunion",
_get_arr_size( _temp500, 1u), _temp500.curr);})); return; _LL499:( void) _throw(
_temp494); _LL495:;}}} if( tud_opt != 0){ tud->name=(*(( struct Cyc_Absyn_Tuniondecl**)((
struct Cyc_Core_Opt*) _check_null( tud_opt))->v))->name;} else{(* tud->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp501=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp501[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp502; _temp502.tag= Cyc_Absyn_Abs_n; _temp502.f1= te->ns; _temp502;});
_temp501;});}} else{{ void* _temp503=(* tud->name).f1; struct Cyc_List_List*
_temp511; _LL505: if(( unsigned int) _temp503 > 1u?*(( int*) _temp503) == Cyc_Absyn_Rel_n:
0){ _LL512: _temp511=(( struct Cyc_Absyn_Rel_n_struct*) _temp503)->f1; if(
_temp511 == 0){ goto _LL506;} else{ goto _LL507;}} else{ goto _LL507;} _LL507:
if(( unsigned int) _temp503 > 1u?*(( int*) _temp503) == Cyc_Absyn_Abs_n: 0){
goto _LL508;} else{ goto _LL509;} _LL509: goto _LL510; _LL506:(* tud->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp513=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp513[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp514; _temp514.tag= Cyc_Absyn_Abs_n; _temp514.f1= te->ns; _temp514;});
_temp513;}); goto _LL504; _LL508: goto _LL510; _LL510: Cyc_Tcutil_terr( loc,
_tag_arr("qualified declarations are not implemented", sizeof( unsigned char),
43u)); return; _LL504:;} tud_opt=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)( ge->tuniondecls, v);}{ struct
_tuple4 _temp516=({ struct _tuple4 _temp515; _temp515.f1= tud->fields; _temp515.f2=
tud_opt; _temp515;}); struct Cyc_Core_Opt* _temp526; struct Cyc_Core_Opt*
_temp528; struct Cyc_Core_Opt* _temp530; struct Cyc_Core_Opt* _temp532; struct
Cyc_Core_Opt _temp534; struct Cyc_List_List* _temp535; struct Cyc_List_List**
_temp537; struct Cyc_Core_Opt* _temp538; struct Cyc_Core_Opt _temp540; struct
Cyc_Absyn_Tuniondecl** _temp541; struct Cyc_Core_Opt* _temp543; struct Cyc_Core_Opt
_temp545; struct Cyc_List_List* _temp546; struct Cyc_List_List** _temp548;
struct Cyc_Core_Opt* _temp549; struct Cyc_Core_Opt _temp551; struct Cyc_Absyn_Tuniondecl**
_temp552; struct Cyc_Core_Opt* _temp554; _LL518: _LL529: _temp528= _temp516.f1;
if( _temp528 == 0){ goto _LL527;} else{ goto _LL520;} _LL527: _temp526= _temp516.f2;
if( _temp526 == 0){ goto _LL519;} else{ goto _LL520;} _LL520: _LL533: _temp532=
_temp516.f1; if( _temp532 == 0){ goto _LL522;} else{ _temp534=* _temp532; _LL536:
_temp535=( struct Cyc_List_List*) _temp534.v; _temp537=&(* _temp516.f1).v; goto
_LL531;} _LL531: _temp530= _temp516.f2; if( _temp530 == 0){ goto _LL521;} else{
goto _LL522;} _LL522: _LL544: _temp543= _temp516.f1; if( _temp543 == 0){ goto
_LL524;} else{ _temp545=* _temp543; _LL547: _temp546=( struct Cyc_List_List*)
_temp545.v; _temp548=&(* _temp516.f1).v; goto _LL539;} _LL539: _temp538=
_temp516.f2; if( _temp538 == 0){ goto _LL524;} else{ _temp540=* _temp538; _LL542:
_temp541=( struct Cyc_Absyn_Tuniondecl**) _temp540.v; goto _LL523;} _LL524:
_LL555: _temp554= _temp516.f1; if( _temp554 == 0){ goto _LL550;} else{ goto
_LL517;} _LL550: _temp549= _temp516.f2; if( _temp549 == 0){ goto _LL517;} else{
_temp551=* _temp549; _LL553: _temp552=( struct Cyc_Absyn_Tuniondecl**) _temp551.v;
goto _LL525;} _LL519: ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)(
ge->tuniondecls, v,({ struct Cyc_Absyn_Tuniondecl** _temp556=( struct Cyc_Absyn_Tuniondecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp556[ 0]= tud; _temp556;}));
goto _LL517; _LL521: { struct Cyc_Absyn_Tuniondecl** _temp557=({ struct Cyc_Absyn_Tuniondecl**
_temp558=( struct Cyc_Absyn_Tuniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*));
_temp558[ 0]=({ struct Cyc_Absyn_Tuniondecl* _temp559=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp559->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp559->name= tud->name; _temp559->tvs= tvs; _temp559->fields=
0; _temp559->is_xtunion= tud->is_xtunion; _temp559;}); _temp558;}); ge->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)( ge->tuniondecls, v,
_temp557);* _temp537= Cyc_Tc_tcTunionFields( te, ge, loc, obj, tud->is_xtunion,
tud->name,* _temp537, tvs, tud);* _temp557= tud; goto _LL517;} _LL523: { struct
Cyc_Absyn_Tuniondecl* _temp560=* _temp541;* _temp541=({ struct Cyc_Absyn_Tuniondecl*
_temp561=( struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp561->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp561->name= tud->name;
_temp561->tvs= tvs; _temp561->fields= 0; _temp561->is_xtunion= tud->is_xtunion;
_temp561;});* _temp548= Cyc_Tc_tcTunionFields( te, ge, loc, obj, tud->is_xtunion,
tud->name,* _temp548, tvs, tud);* _temp541= _temp560; _temp552= _temp541; goto
_LL525;} _LL525: { struct Cyc_Absyn_Tuniondecl* _temp562= Cyc_Tcdecl_merge_tuniondecl(*
_temp552, tud, loc, Cyc_Tc_tc_msg); if( _temp562 == 0){ return;} else{* _temp552=(
struct Cyc_Absyn_Tuniondecl*) _check_null( _temp562); goto _LL517;}} _LL517:;}}}}
static void Cyc_Tc_tcEnumdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Enumdecl* ed){ struct
_tagged_arr* v=(* ed->name).f2;{ void* _temp563=(* ed->name).f1; struct Cyc_List_List*
_temp571; struct Cyc_List_List* _temp573; _LL565: if(( unsigned int) _temp563 >
1u?*(( int*) _temp563) == Cyc_Absyn_Rel_n: 0){ _LL572: _temp571=(( struct Cyc_Absyn_Rel_n_struct*)
_temp563)->f1; if( _temp571 == 0){ goto _LL566;} else{ goto _LL567;}} else{ goto
_LL567;} _LL567: if(( unsigned int) _temp563 > 1u?*(( int*) _temp563) == Cyc_Absyn_Abs_n:
0){ _LL574: _temp573=(( struct Cyc_Absyn_Abs_n_struct*) _temp563)->f1; if(
_temp573 == 0){ goto _LL568;} else{ goto _LL569;}} else{ goto _LL569;} _LL569:
goto _LL570; _LL566: goto _LL564; _LL568: goto _LL564; _LL570: Cyc_Tcutil_terr(
loc, _tag_arr("qualified declarations are not implemented", sizeof(
unsigned char), 43u)); return; _LL564:;}(* ed->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp575=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp575[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp576; _temp576.tag= Cyc_Absyn_Abs_n;
_temp576.f1= te->ns; _temp576;}); _temp575;}); if( ed->fields != 0){ struct
_RegionHandle _temp577= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp577; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0;
unsigned int tag_count= 0; struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( ed->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ struct Cyc_Absyn_Enumfield*
_temp578=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( fs))->hd;
if((( int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*), struct
Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp,
prev_fields,(* _temp578->name).f2)){ Cyc_Tcutil_terr( _temp578->loc,( struct
_tagged_arr)({ struct _tagged_arr _temp579=*(* _temp578->name).f2; xprintf("duplicate field name %.*s",
_get_arr_size( _temp579, 1u), _temp579.curr);}));} else{ prev_fields=({ struct
Cyc_List_List* _temp580=( struct Cyc_List_List*) _region_malloc( uprev_rgn,
sizeof( struct Cyc_List_List)); _temp580->hd=( void*)(* _temp578->name).f2;
_temp580->tl= prev_fields; _temp580;});} if( _temp578->tag == 0){ _temp578->tag=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( tag_count, _temp578->loc);} else{ if(
! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*) _check_null( _temp578->tag))){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp581=* v;
struct _tagged_arr _temp582=*(* _temp578->name).f2; xprintf("enum %.*s, field %.*s: expression is not constant",
_get_arr_size( _temp581, 1u), _temp581.curr, _get_arr_size( _temp582, 1u),
_temp582.curr);}));}}{ unsigned int t1= Cyc_Evexp_eval_const_uint_exp(( struct
Cyc_Absyn_Exp*) _check_null( _temp578->tag)); tag_count= t1 + 1;(* _temp578->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp583=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp583[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp584; _temp584.tag= Cyc_Absyn_Abs_n; _temp584.f1= te->ns; _temp584;});
_temp583;});}}}; _pop_region( uprev_rgn);}{ struct _handler_cons _temp585;
_push_handler(& _temp585);{ int _temp587= 0; if( setjmp( _temp585.handler)){
_temp587= 1;} if( ! _temp587){{ struct Cyc_Absyn_Enumdecl** _temp588=(( struct
Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
ge->enumdecls, v); struct Cyc_Absyn_Enumdecl* _temp589= Cyc_Tcdecl_merge_enumdecl(*
_temp588, ed, loc, Cyc_Tc_tc_msg); if( _temp589 == 0){ _npop_handler( 0u);
return;}* _temp588=( struct Cyc_Absyn_Enumdecl*) _check_null( _temp589);};
_pop_handler();} else{ void* _temp586=( void*) _exn_thrown; void* _temp591=
_temp586; _LL593: if( _temp591 == Cyc_Dict_Absent){ goto _LL594;} else{ goto
_LL595;} _LL595: goto _LL596; _LL594: { struct Cyc_Absyn_Enumdecl** _temp597=({
struct Cyc_Absyn_Enumdecl** _temp598=( struct Cyc_Absyn_Enumdecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl*)); _temp598[ 0]= ed; _temp598;}); ge->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Enumdecl** data)) Cyc_Dict_insert)( ge->enumdecls, v, _temp597);
goto _LL592;} _LL596:( void) _throw( _temp591); _LL592:;}}} if( ed->fields != 0){
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ed->fields))->v; for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ struct Cyc_Absyn_Enumfield* _temp599=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_List_List*) _check_null( fs))->hd; ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries,(* _temp599->name).f2,({ struct _tuple3* _temp600=( struct
_tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp600->f1=( void*)({ struct
Cyc_Tcenv_EnumRes_struct* _temp601=( struct Cyc_Tcenv_EnumRes_struct*) GC_malloc(
sizeof( struct Cyc_Tcenv_EnumRes_struct)); _temp601[ 0]=({ struct Cyc_Tcenv_EnumRes_struct
_temp602; _temp602.tag= Cyc_Tcenv_EnumRes; _temp602.f1= ed; _temp602.f2=
_temp599; _temp602;}); _temp601;}); _temp600->f2= 1; _temp600;}));}}} static int
Cyc_Tc_okay_externC( struct Cyc_Position_Segment* loc, void* sc){ void* _temp603=
sc; _LL605: if( _temp603 ==( void*) Cyc_Absyn_Static){ goto _LL606;} else{ goto
_LL607;} _LL607: if( _temp603 ==( void*) Cyc_Absyn_Abstract){ goto _LL608;}
else{ goto _LL609;} _LL609: if( _temp603 ==( void*) Cyc_Absyn_Public){ goto
_LL610;} else{ goto _LL611;} _LL611: if( _temp603 ==( void*) Cyc_Absyn_Extern){
goto _LL612;} else{ goto _LL613;} _LL613: if( _temp603 ==( void*) Cyc_Absyn_ExternC){
goto _LL614;} else{ goto _LL604;} _LL606: Cyc_Tcutil_warn( loc, _tag_arr("static declaration nested within extern \"C\"",
sizeof( unsigned char), 44u)); return 0; _LL608: Cyc_Tcutil_warn( loc, _tag_arr("abstract declaration nested within extern \"C\"",
sizeof( unsigned char), 46u)); return 0; _LL610: return 1; _LL612: return 1;
_LL614: Cyc_Tcutil_warn( loc, _tag_arr("nested extern \"C\" declaration",
sizeof( unsigned char), 30u)); return 1; _LL604:;} static void Cyc_Tc_tc_decls(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds0, int in_externC, int
check_var_init){ struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, te->ns);
struct Cyc_List_List* _temp615= ds0; for( 0; _temp615 != 0; _temp615=(( struct
Cyc_List_List*) _check_null( _temp615))->tl){ struct Cyc_Absyn_Decl* d=( struct
Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( _temp615))->hd; struct
Cyc_Position_Segment* loc= d->loc; void* _temp616=( void*) d->r; struct Cyc_Absyn_Vardecl*
_temp642; struct Cyc_Absyn_Fndecl* _temp644; struct Cyc_Absyn_Typedefdecl*
_temp646; struct Cyc_Absyn_Structdecl* _temp648; struct Cyc_Absyn_Uniondecl*
_temp650; struct Cyc_Absyn_Tuniondecl* _temp652; struct Cyc_Absyn_Enumdecl*
_temp654; struct Cyc_List_List* _temp656; struct _tagged_arr* _temp658; struct
Cyc_List_List* _temp660; struct _tuple0* _temp662; struct _tuple0 _temp664;
struct _tagged_arr* _temp665; void* _temp667; struct Cyc_List_List* _temp669;
_LL618: if(*(( int*) _temp616) == Cyc_Absyn_Let_d){ goto _LL619;} else{ goto
_LL620;} _LL620: if(*(( int*) _temp616) == Cyc_Absyn_Letv_d){ goto _LL621;}
else{ goto _LL622;} _LL622: if(*(( int*) _temp616) == Cyc_Absyn_Var_d){ _LL643:
_temp642=(( struct Cyc_Absyn_Var_d_struct*) _temp616)->f1; goto _LL623;} else{
goto _LL624;} _LL624: if(*(( int*) _temp616) == Cyc_Absyn_Fn_d){ _LL645:
_temp644=(( struct Cyc_Absyn_Fn_d_struct*) _temp616)->f1; goto _LL625;} else{
goto _LL626;} _LL626: if(*(( int*) _temp616) == Cyc_Absyn_Typedef_d){ _LL647:
_temp646=(( struct Cyc_Absyn_Typedef_d_struct*) _temp616)->f1; goto _LL627;}
else{ goto _LL628;} _LL628: if(*(( int*) _temp616) == Cyc_Absyn_Struct_d){
_LL649: _temp648=(( struct Cyc_Absyn_Struct_d_struct*) _temp616)->f1; goto
_LL629;} else{ goto _LL630;} _LL630: if(*(( int*) _temp616) == Cyc_Absyn_Union_d){
_LL651: _temp650=(( struct Cyc_Absyn_Union_d_struct*) _temp616)->f1; goto _LL631;}
else{ goto _LL632;} _LL632: if(*(( int*) _temp616) == Cyc_Absyn_Tunion_d){
_LL653: _temp652=(( struct Cyc_Absyn_Tunion_d_struct*) _temp616)->f1; goto
_LL633;} else{ goto _LL634;} _LL634: if(*(( int*) _temp616) == Cyc_Absyn_Enum_d){
_LL655: _temp654=(( struct Cyc_Absyn_Enum_d_struct*) _temp616)->f1; goto _LL635;}
else{ goto _LL636;} _LL636: if(*(( int*) _temp616) == Cyc_Absyn_Namespace_d){
_LL659: _temp658=(( struct Cyc_Absyn_Namespace_d_struct*) _temp616)->f1; goto
_LL657; _LL657: _temp656=(( struct Cyc_Absyn_Namespace_d_struct*) _temp616)->f2;
goto _LL637;} else{ goto _LL638;} _LL638: if(*(( int*) _temp616) == Cyc_Absyn_Using_d){
_LL663: _temp662=(( struct Cyc_Absyn_Using_d_struct*) _temp616)->f1; _temp664=*
_temp662; _LL668: _temp667= _temp664.f1; goto _LL666; _LL666: _temp665= _temp664.f2;
goto _LL661; _LL661: _temp660=(( struct Cyc_Absyn_Using_d_struct*) _temp616)->f2;
goto _LL639;} else{ goto _LL640;} _LL640: if(*(( int*) _temp616) == Cyc_Absyn_ExternC_d){
_LL670: _temp669=(( struct Cyc_Absyn_ExternC_d_struct*) _temp616)->f1; goto
_LL641;} else{ goto _LL617;} _LL619: Cyc_Tcutil_terr( loc, _tag_arr("top level let-declarations are not implemented",
sizeof( unsigned char), 47u)); goto _LL617; _LL621: Cyc_Tcutil_terr( loc,
_tag_arr("top level let-declarations are not implemented", sizeof( unsigned char),
47u)); goto _LL617; _LL623: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*)
_temp642->sc): 0){( void*)( _temp642->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcVardecl( te, ge, loc, _temp642, check_var_init); goto _LL617; _LL625:
if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp644->sc): 0){( void*)(
_temp644->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcFndecl( te, ge,
loc, _temp644); goto _LL617; _LL627: Cyc_Tc_tcTypedefdecl( te, ge, loc, _temp646);
goto _LL617; _LL629: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*)
_temp648->sc): 0){( void*)( _temp648->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcStructdecl( te, ge, loc, _temp648); goto _LL617; _LL631: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp650->sc): 0){( void*)( _temp650->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcUniondecl( te, ge, loc, _temp650);
goto _LL617; _LL633: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*)
_temp652->sc): 0){( void*)( _temp652->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcTuniondecl( te, ge, loc, _temp652); goto _LL617; _LL635: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp654->sc): 0){( void*)( _temp654->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcEnumdecl( te, ge, loc, _temp654);
goto _LL617; _LL637: { struct Cyc_List_List* _temp671= te->ns; struct Cyc_List_List*
_temp672=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp671,({ struct Cyc_List_List* _temp673=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp673->hd=( void*) _temp658;
_temp673->tl= 0; _temp673;})); if( !(( int(*)( struct Cyc_Set_Set* s, struct
_tagged_arr* elt)) Cyc_Set_member)( ge->namespaces, _temp658)){ ge->namespaces=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)(
ge->namespaces, _temp658); te->ae=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key, struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)( te->ae,
_temp672, Cyc_Tcenv_empty_genv());} te->ns= _temp672; Cyc_Tc_tc_decls( te,
_temp656, in_externC, check_var_init); te->ns= _temp671; goto _LL617;} _LL639: {
struct _tagged_arr* first; struct Cyc_List_List* rest;{ void* _temp674= _temp667;
struct Cyc_List_List* _temp686; struct Cyc_List_List* _temp688; struct Cyc_List_List*
_temp690; struct Cyc_List_List _temp692; struct Cyc_List_List* _temp693; struct
_tagged_arr* _temp695; struct Cyc_List_List* _temp697; struct Cyc_List_List
_temp699; struct Cyc_List_List* _temp700; struct _tagged_arr* _temp702; _LL676:
if( _temp674 ==( void*) Cyc_Absyn_Loc_n){ goto _LL677;} else{ goto _LL678;}
_LL678: if(( unsigned int) _temp674 > 1u?*(( int*) _temp674) == Cyc_Absyn_Rel_n:
0){ _LL687: _temp686=(( struct Cyc_Absyn_Rel_n_struct*) _temp674)->f1; if(
_temp686 == 0){ goto _LL679;} else{ goto _LL680;}} else{ goto _LL680;} _LL680:
if(( unsigned int) _temp674 > 1u?*(( int*) _temp674) == Cyc_Absyn_Abs_n: 0){
_LL689: _temp688=(( struct Cyc_Absyn_Abs_n_struct*) _temp674)->f1; if( _temp688
== 0){ goto _LL681;} else{ goto _LL682;}} else{ goto _LL682;} _LL682: if((
unsigned int) _temp674 > 1u?*(( int*) _temp674) == Cyc_Absyn_Rel_n: 0){ _LL691:
_temp690=(( struct Cyc_Absyn_Rel_n_struct*) _temp674)->f1; if( _temp690 == 0){
goto _LL684;} else{ _temp692=* _temp690; _LL696: _temp695=( struct _tagged_arr*)
_temp692.hd; goto _LL694; _LL694: _temp693= _temp692.tl; goto _LL683;}} else{
goto _LL684;} _LL684: if(( unsigned int) _temp674 > 1u?*(( int*) _temp674) ==
Cyc_Absyn_Abs_n: 0){ _LL698: _temp697=(( struct Cyc_Absyn_Abs_n_struct*)
_temp674)->f1; if( _temp697 == 0){ goto _LL675;} else{ _temp699=* _temp697;
_LL703: _temp702=( struct _tagged_arr*) _temp699.hd; goto _LL701; _LL701:
_temp700= _temp699.tl; goto _LL685;}} else{ goto _LL675;} _LL677: goto _LL679;
_LL679: goto _LL681; _LL681: first= _temp665; rest= 0; goto _LL675; _LL683:
_temp702= _temp695; _temp700= _temp693; goto _LL685; _LL685: first= _temp702;
rest=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp700,({ struct Cyc_List_List* _temp704=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp704->hd=( void*) _temp665;
_temp704->tl= 0; _temp704;})); goto _LL675; _LL675:;}{ struct Cyc_List_List*
_temp705= Cyc_Tcenv_resolve_namespace( te, loc, first, rest); ge->availables=({
struct Cyc_List_List* _temp706=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp706->hd=( void*) _temp705; _temp706->tl= ge->availables;
_temp706;}); Cyc_Tc_tc_decls( te, _temp660, in_externC, check_var_init); ge->availables=((
struct Cyc_List_List*) _check_null( ge->availables))->tl; goto _LL617;}} _LL641:
Cyc_Tc_tc_decls( te, _temp669, 1, check_var_init); goto _LL617; _LL617:;}} void
Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int check_var_init, struct Cyc_List_List*
ds){ Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r); Cyc_Tc_tc_decls( te, ds,
0, check_var_init);} static int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env,
struct Cyc_Absyn_Decl* d); static struct Cyc_List_List* Cyc_Tc_treeshake_f(
struct Cyc_Dict_Dict* env, struct Cyc_List_List* ds){ return(( struct Cyc_List_List*(*)(
int(* f)( struct Cyc_Dict_Dict*, struct Cyc_Absyn_Decl*), struct Cyc_Dict_Dict*
env, struct Cyc_List_List* l)) Cyc_List_filter_c)( Cyc_Tc_vardecl_needed, env,
ds);} struct _tuple6{ struct Cyc_Tcenv_Genv* f1; struct Cyc_Set_Set* f2; } ;
static int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl*
d){ void* _temp707=( void*) d->r; struct Cyc_Absyn_Vardecl* _temp717; struct Cyc_List_List*
_temp719; struct Cyc_List_List** _temp721; struct Cyc_List_List* _temp722;
struct Cyc_List_List** _temp724; _LL709: if(*(( int*) _temp707) == Cyc_Absyn_Var_d){
_LL718: _temp717=(( struct Cyc_Absyn_Var_d_struct*) _temp707)->f1; goto _LL710;}
else{ goto _LL711;} _LL711: if(*(( int*) _temp707) == Cyc_Absyn_Using_d){ _LL720:
_temp719=(( struct Cyc_Absyn_Using_d_struct*) _temp707)->f2; _temp721=&(( struct
Cyc_Absyn_Using_d_struct*) _temp707)->f2; goto _LL712;} else{ goto _LL713;}
_LL713: if(*(( int*) _temp707) == Cyc_Absyn_Namespace_d){ _LL723: _temp722=((
struct Cyc_Absyn_Namespace_d_struct*) _temp707)->f2; _temp724=&(( struct Cyc_Absyn_Namespace_d_struct*)
_temp707)->f2; goto _LL714;} else{ goto _LL715;} _LL715: goto _LL716; _LL710:
if(( void*) _temp717->sc !=( void*) Cyc_Absyn_Extern){ return 1;}{ struct
_tuple0 _temp727; struct _tagged_arr* _temp728; void* _temp730; struct _tuple0*
_temp725= _temp717->name; _temp727=* _temp725; _LL731: _temp730= _temp727.f1;
goto _LL729; _LL729: _temp728= _temp727.f2; goto _LL726; _LL726: { struct Cyc_List_List*
ns;{ void* _temp732= _temp730; struct Cyc_List_List* _temp740; struct Cyc_List_List*
_temp742; _LL734: if( _temp732 ==( void*) Cyc_Absyn_Loc_n){ goto _LL735;} else{
goto _LL736;} _LL736: if(( unsigned int) _temp732 > 1u?*(( int*) _temp732) ==
Cyc_Absyn_Rel_n: 0){ _LL741: _temp740=(( struct Cyc_Absyn_Rel_n_struct*)
_temp732)->f1; goto _LL737;} else{ goto _LL738;} _LL738: if(( unsigned int)
_temp732 > 1u?*(( int*) _temp732) == Cyc_Absyn_Abs_n: 0){ _LL743: _temp742=((
struct Cyc_Absyn_Abs_n_struct*) _temp732)->f1; goto _LL739;} else{ goto _LL733;}
_LL735: ns= 0; goto _LL733; _LL737: ns= _temp740; goto _LL733; _LL739: ns=
_temp742; goto _LL733; _LL733:;}{ struct _tuple6* _temp744=(( struct _tuple6*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( env, ns);
struct Cyc_Tcenv_Genv* _temp745=(* _temp744).f1; int _temp746=(*(( struct
_tuple3*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
_temp745->ordinaries, _temp728)).f2; if( ! _temp746){(* _temp744).f2=(( struct
Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)((*
_temp744).f2, _temp728);} return _temp746;}}} _LL712: _temp724= _temp721; goto
_LL714; _LL714:* _temp724= Cyc_Tc_treeshake_f( env,* _temp724); return 1; _LL716:
return 1; _LL708:;} static int Cyc_Tc_treeshake_remove_f( struct Cyc_Set_Set*
set, struct _tagged_arr* x, struct _tuple3* y){ return !(( int(*)( struct Cyc_Set_Set*
s, struct _tagged_arr* elt)) Cyc_Set_member)( set, x);} static void Cyc_Tc_treeshake_remove(
struct Cyc_List_List* ns, struct _tuple6* nsenv){ struct Cyc_Tcenv_Genv*
_temp747=(* nsenv).f1; struct Cyc_Set_Set* _temp748=(* nsenv).f2; _temp747->ordinaries=((
struct Cyc_Dict_Dict*(*)( int(* f)( struct Cyc_Set_Set*, struct _tagged_arr*,
struct _tuple3*), struct Cyc_Set_Set* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_filter_c)(
Cyc_Tc_treeshake_remove_f, _temp748, _temp747->ordinaries);} static struct
_tuple6* Cyc_Tc_treeshake_make_env_f( struct Cyc_Tcenv_Genv* ge){ return({
struct _tuple6* _temp749=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp749->f1= ge; _temp749->f2=(( struct Cyc_Set_Set*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Set_empty)( Cyc_String_strptrcmp);
_temp749;});} struct Cyc_List_List* Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv* te,
struct Cyc_List_List* ds){ struct Cyc_Dict_Dict* _temp750=(( struct Cyc_Dict_Dict*(*)(
struct _tuple6*(* f)( struct Cyc_Tcenv_Genv*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)(
Cyc_Tc_treeshake_make_env_f, te->ae); struct Cyc_List_List* _temp751= Cyc_Tc_treeshake_f(
_temp750, ds);(( void(*)( void(* f)( struct Cyc_List_List*, struct _tuple6*),
struct Cyc_Dict_Dict* d)) Cyc_Dict_iter)( Cyc_Tc_treeshake_remove, _temp750);
return _temp751;}