 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 8u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern struct _tagged_string
Cyc_Core_string_of_int( int); extern unsigned char* string_to_Cstring( struct
_tagged_string); extern unsigned char* underlying_Cstring( struct _tagged_string);
extern struct _tagged_string Cstring_to_string( unsigned char*); extern int
system( unsigned char*); struct Cyc_Stdio___sFILE; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_stdout; extern int Cyc_Stdio_fputc( int, struct Cyc_Stdio___sFILE*);
extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_string f1; } ; extern int Cyc_Stdio_file_string_write( struct Cyc_Stdio___sFILE*
fd, struct _tagged_string src, int src_offset, int max_count); struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; extern int Cyc_List_length( struct Cyc_List_List*
x); extern unsigned char Cyc_List_List_empty[ 15u]; extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern unsigned char
Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[
8u]; struct Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct
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
attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{ struct _tuple0* name; int
is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion= 0; struct Cyc_Absyn_UnknownTunion_struct{
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
f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt* f4; } ; static
const int Cyc_Absyn_NonNullable_ps= 0; struct Cyc_Absyn_NonNullable_ps_struct{
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
static const int Cyc_Absyn_Deref_e= 18; struct Cyc_Absyn_Deref_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_StructMember_e= 19;
struct Cyc_Absyn_StructMember_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct _tagged_string* f2; } ; static const int Cyc_Absyn_StructArrow_e= 20;
struct Cyc_Absyn_StructArrow_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tagged_string* f2; } ; static const int Cyc_Absyn_Subscript_e= 21; struct Cyc_Absyn_Subscript_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; static const
int Cyc_Absyn_Tuple_e= 22; struct Cyc_Absyn_Tuple_e_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_CompoundLit_e= 23; struct _tuple1{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; struct Cyc_Absyn_CompoundLit_e_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e=
24; struct Cyc_Absyn_Array_e_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_Comprehension_e= 25; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Struct_e= 26; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_AnonStruct_e= 27;
struct Cyc_Absyn_AnonStruct_e_struct{ int tag; void* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Absyn_Tunion_e= 28; struct Cyc_Absyn_Tunion_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ;
static const int Cyc_Absyn_Enum_e= 29; struct Cyc_Absyn_Enum_e_struct{ int tag;
struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_Malloc_e= 30; struct Cyc_Absyn_Malloc_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_UnresolvedMem_e=
31; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 32; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 33; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 34; struct Cyc_Absyn_Fill_e_struct{
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
cyc_varargs; struct Cyc_Absyn_Stmt* body; struct Cyc_Core_Opt* cached_typ;
struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List* attributes; } ;
struct Cyc_Absyn_Structfield{ struct _tagged_string* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List* attributes; }
; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Tunionfield{ struct _tuple0* name; struct Cyc_List_List* tvs; struct
Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ; struct Cyc_Absyn_Tuniondecl{
void* sc; struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{ struct _tuple0* name;
struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Enumdecl{
void* sc; struct _tuple0* name; struct Cyc_Core_Opt* fields; } ; struct Cyc_Absyn_Typedefdecl{
struct _tuple0* name; struct Cyc_List_List* tvs; void* defn; } ; static const
int Cyc_Absyn_Var_d= 0; struct Cyc_Absyn_Var_d_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; } ; static const int Cyc_Absyn_Fn_d= 1; struct Cyc_Absyn_Fn_d_struct{ int
tag; struct Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Let_d= 2;
struct Cyc_Absyn_Let_d_struct{ int tag; struct Cyc_Absyn_Pat* f1; struct Cyc_Core_Opt*
f2; struct Cyc_Core_Opt* f3; struct Cyc_Absyn_Exp* f4; int f5; } ; static const
int Cyc_Absyn_Letv_d= 3; struct Cyc_Absyn_Letv_d_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_Struct_d= 4; struct Cyc_Absyn_Struct_d_struct{
int tag; struct Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Absyn_Union_d=
5; struct Cyc_Absyn_Union_d_struct{ int tag; struct Cyc_Absyn_Uniondecl* f1; } ;
static const int Cyc_Absyn_Tunion_d= 6; struct Cyc_Absyn_Tunion_d_struct{ int
tag; struct Cyc_Absyn_Tuniondecl* f1; } ; static const int Cyc_Absyn_Enum_d= 7;
struct Cyc_Absyn_Enum_d_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; } ;
static const int Cyc_Absyn_Typedef_d= 8; struct Cyc_Absyn_Typedef_d_struct{ int
tag; struct Cyc_Absyn_Typedefdecl* f1; } ; static const int Cyc_Absyn_Namespace_d=
9; struct Cyc_Absyn_Namespace_d_struct{ int tag; struct _tagged_string* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
11; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static
const int Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1;
struct Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ;
extern unsigned char Cyc_Absyn_EmptyAnnot[ 15u]; extern struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x); extern int Cyc_Absyn_is_format_prim(
void* p); extern struct _tagged_string Cyc_Absyn_attribute2string( void*);
struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc; extern int Cyc_Absynpp_expand_typedefs;
extern int Cyc_Absynpp_qvar_to_Cids; extern int Cyc_Absynpp_to_VC; extern int
Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp*); extern struct _tagged_string Cyc_Absynpp_char_escape(
unsigned char); extern struct _tagged_string Cyc_Absynpp_string_escape( struct
_tagged_string); extern struct _tagged_string Cyc_Absynpp_prim2str( void* p);
extern int Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s); struct _tuple3{
struct _tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; extern
struct _tuple1* Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg); struct _tuple4{
struct Cyc_Absyn_Tqual f1; void* f2; struct Cyc_List_List* f3; } ; extern struct
_tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual tq, void* t); extern
unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); struct Cyc_Set_Set;
extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int tag;
void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_compress(
void* t); void Cyc_Absyndump_dumptyp( void*); void Cyc_Absyndump_dumpntyp( void*
t); void Cyc_Absyndump_dumpexp( struct Cyc_Absyn_Exp*); void Cyc_Absyndump_dumpexp_prec(
int, struct Cyc_Absyn_Exp*); void Cyc_Absyndump_dumppat( struct Cyc_Absyn_Pat*);
void Cyc_Absyndump_dumpstmt( struct Cyc_Absyn_Stmt*); void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl*); void Cyc_Absyndump_dumptms( struct Cyc_List_List* tms,
void(* f)( void*), void* a); void Cyc_Absyndump_dumptqtd( struct Cyc_Absyn_Tqual,
void*, void(* f)( void*), void*); void Cyc_Absyndump_dumpstructfields( struct
Cyc_List_List* fields); struct Cyc_Stdio___sFILE** Cyc_Absyndump_dump_file=& Cyc_Stdio_stdout;
void Cyc_Absyndump_ignore( void* x){ return;} static unsigned int Cyc_Absyndump_pos=
0; void Cyc_Absyndump_dump( struct _tagged_string s){ int sz=( int)({ struct
_tagged_string _temp0= s;( unsigned int)( _temp0.last_plus_one - _temp0.curr);});
if( !(( int)*(( unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char), sz - 1))){
-- sz;} Cyc_Absyndump_pos += sz + 1; if( Cyc_Absyndump_pos > 80){ Cyc_Absyndump_pos=(
unsigned int) sz; Cyc_Stdio_fputc(( int)'\n',* Cyc_Absyndump_dump_file);} else{
Cyc_Stdio_fputc(( int)' ',* Cyc_Absyndump_dump_file);} Cyc_Stdio_file_string_write(*
Cyc_Absyndump_dump_file, s, 0, sz);} void Cyc_Absyndump_dump_nospace( struct
_tagged_string s){ int sz=( int)({ struct _tagged_string _temp1= s;(
unsigned int)( _temp1.last_plus_one - _temp1.curr);}); if( !(( int)*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char), sz - 1))){ -- sz;} Cyc_Absyndump_pos
+= sz; Cyc_Stdio_file_string_write(* Cyc_Absyndump_dump_file, s, 0, sz);} void
Cyc_Absyndump_dump_char( int c){ ++ Cyc_Absyndump_pos; Cyc_Stdio_fputc( c,* Cyc_Absyndump_dump_file);}
void Cyc_Absyndump_dump_str( struct _tagged_string* s){ Cyc_Absyndump_dump(* s);}
void Cyc_Absyndump_dump_semi(){ Cyc_Absyndump_dump_char(( int)';');} void Cyc_Absyndump_dump_sep(
void(* f)( void*), struct Cyc_List_List* l, struct _tagged_string sep){ if( l ==
0){ return;} for( 0;(( struct Cyc_List_List*) _check_null( l))->tl != 0; l=((
struct Cyc_List_List*) _check_null( l))->tl){ f(( void*)(( struct Cyc_List_List*)
_check_null( l))->hd); Cyc_Absyndump_dump_nospace( sep);} f(( void*)(( struct
Cyc_List_List*) _check_null( l))->hd);} void Cyc_Absyndump_dump_sep_c( void(* f)(
void*, void*), void* env, struct Cyc_List_List* l, struct _tagged_string sep){
if( l == 0){ return;} for( 0;(( struct Cyc_List_List*) _check_null( l))->tl != 0;
l=(( struct Cyc_List_List*) _check_null( l))->tl){ f( env,( void*)(( struct Cyc_List_List*)
_check_null( l))->hd); Cyc_Absyndump_dump_nospace( sep);} f( env,( void*)((
struct Cyc_List_List*) _check_null( l))->hd);} void Cyc_Absyndump_group( void(*
f)( void*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep){ Cyc_Absyndump_dump_nospace(
start);(( void(*)( void(* f)( void*), struct Cyc_List_List* l, struct
_tagged_string sep)) Cyc_Absyndump_dump_sep)( f, l, sep); Cyc_Absyndump_dump_nospace(
end);} void Cyc_Absyndump_group_c( void(* f)( void*, void*), void* env, struct
Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end, struct
_tagged_string sep){ Cyc_Absyndump_dump_nospace( start);(( void(*)( void(* f)(
void*, void*), void* env, struct Cyc_List_List* l, struct _tagged_string sep))
Cyc_Absyndump_dump_sep_c)( f, env, l, sep); Cyc_Absyndump_dump_nospace( end);}
void Cyc_Absyndump_egroup( void(* f)( void*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep){ if(
l != 0){(( void(*)( void(* f)( void*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
f, l, start, end, sep);}} void Cyc_Absyndump_dumpqvar( struct _tuple0* v){
struct Cyc_List_List* _temp2= 0; goto _LL3; _LL3:{ void* _temp4=(* v).f1; struct
Cyc_List_List* _temp16; struct Cyc_List_List* _temp18; struct Cyc_List_List*
_temp20; struct Cyc_List_List* _temp22; _LL6: if( _temp4 ==( void*) Cyc_Absyn_Loc_n){
goto _LL7;} else{ goto _LL8;} _LL8: if(( unsigned int) _temp4 > 1u?*(( int*)
_temp4) == Cyc_Absyn_Rel_n: 0){ _LL17: _temp16=(( struct Cyc_Absyn_Rel_n_struct*)
_temp4)->f1; if( _temp16 == 0){ goto _LL9;} else{ goto _LL10;}} else{ goto _LL10;}
_LL10: if(( unsigned int) _temp4 > 1u?*(( int*) _temp4) == Cyc_Absyn_Abs_n: 0){
_LL19: _temp18=(( struct Cyc_Absyn_Abs_n_struct*) _temp4)->f1; if( _temp18 == 0){
goto _LL11;} else{ goto _LL12;}} else{ goto _LL12;} _LL12: if(( unsigned int)
_temp4 > 1u?*(( int*) _temp4) == Cyc_Absyn_Rel_n: 0){ _LL21: _temp20=(( struct
Cyc_Absyn_Rel_n_struct*) _temp4)->f1; goto _LL13;} else{ goto _LL14;} _LL14: if((
unsigned int) _temp4 > 1u?*(( int*) _temp4) == Cyc_Absyn_Abs_n: 0){ _LL23:
_temp22=(( struct Cyc_Absyn_Abs_n_struct*) _temp4)->f1; goto _LL15;} else{ goto
_LL5;} _LL7: goto _LL9; _LL9: goto _LL11; _LL11: Cyc_Absyndump_dump_str((* v).f2);
return; _LL13: _temp22= _temp20; goto _LL15; _LL15: _temp2= _temp22; goto _LL5;
_LL5:;} Cyc_Absyndump_dump_str(( struct _tagged_string*)(( struct Cyc_List_List*)
_check_null( _temp2))->hd); for( _temp2=(( struct Cyc_List_List*) _check_null(
_temp2))->tl; _temp2 != 0; _temp2=(( struct Cyc_List_List*) _check_null( _temp2))->tl){
if( Cyc_Absynpp_qvar_to_Cids){ Cyc_Absyndump_dump_char(( int)'_');} else{ Cyc_Absyndump_dump_nospace(({
unsigned char* _temp24="::"; struct _tagged_string _temp25; _temp25.curr=
_temp24; _temp25.base= _temp24; _temp25.last_plus_one= _temp24 + 3; _temp25;}));}
Cyc_Absyndump_dump_nospace(*(( struct _tagged_string*)(( struct Cyc_List_List*)
_check_null( _temp2))->hd));} if( Cyc_Absynpp_qvar_to_Cids){ Cyc_Absyndump_dump_nospace(({
unsigned char* _temp26="_"; struct _tagged_string _temp27; _temp27.curr= _temp26;
_temp27.base= _temp26; _temp27.last_plus_one= _temp26 + 2; _temp27;}));} else{
Cyc_Absyndump_dump_nospace(({ unsigned char* _temp28="::"; struct _tagged_string
_temp29; _temp29.curr= _temp28; _temp29.base= _temp28; _temp29.last_plus_one=
_temp28 + 3; _temp29;}));} Cyc_Absyndump_dump_nospace(*(* v).f2);} void Cyc_Absyndump_dumptq(
struct Cyc_Absyn_Tqual tq){ if( tq.q_restrict){ Cyc_Absyndump_dump(({
unsigned char* _temp30="restrict"; struct _tagged_string _temp31; _temp31.curr=
_temp30; _temp31.base= _temp30; _temp31.last_plus_one= _temp30 + 9; _temp31;}));}
if( tq.q_volatile){ Cyc_Absyndump_dump(({ unsigned char* _temp32="volatile";
struct _tagged_string _temp33; _temp33.curr= _temp32; _temp33.base= _temp32;
_temp33.last_plus_one= _temp32 + 9; _temp33;}));} if( tq.q_const){ Cyc_Absyndump_dump(({
unsigned char* _temp34="const"; struct _tagged_string _temp35; _temp35.curr=
_temp34; _temp35.base= _temp34; _temp35.last_plus_one= _temp34 + 6; _temp35;}));}}
void Cyc_Absyndump_dumpscope( void* sc){ void* _temp36= sc; _LL38: if( _temp36
==( void*) Cyc_Absyn_Static){ goto _LL39;} else{ goto _LL40;} _LL40: if( _temp36
==( void*) Cyc_Absyn_Public){ goto _LL41;} else{ goto _LL42;} _LL42: if( _temp36
==( void*) Cyc_Absyn_Extern){ goto _LL43;} else{ goto _LL44;} _LL44: if( _temp36
==( void*) Cyc_Absyn_ExternC){ goto _LL45;} else{ goto _LL46;} _LL46: if(
_temp36 ==( void*) Cyc_Absyn_Abstract){ goto _LL47;} else{ goto _LL37;} _LL39:
Cyc_Absyndump_dump(({ unsigned char* _temp48="static"; struct _tagged_string
_temp49; _temp49.curr= _temp48; _temp49.base= _temp48; _temp49.last_plus_one=
_temp48 + 7; _temp49;})); return; _LL41: return; _LL43: Cyc_Absyndump_dump(({
unsigned char* _temp50="extern"; struct _tagged_string _temp51; _temp51.curr=
_temp50; _temp51.base= _temp50; _temp51.last_plus_one= _temp50 + 7; _temp51;}));
return; _LL45: Cyc_Absyndump_dump(({ unsigned char* _temp52="extern \"C\"";
struct _tagged_string _temp53; _temp53.curr= _temp52; _temp53.base= _temp52;
_temp53.last_plus_one= _temp52 + 11; _temp53;})); return; _LL47: Cyc_Absyndump_dump(({
unsigned char* _temp54="abstract"; struct _tagged_string _temp55; _temp55.curr=
_temp54; _temp55.base= _temp54; _temp55.last_plus_one= _temp54 + 9; _temp55;}));
return; _LL37:;} void Cyc_Absyndump_dumpkind( void* k){ void* _temp56= k; _LL58:
if( _temp56 ==( void*) Cyc_Absyn_AnyKind){ goto _LL59;} else{ goto _LL60;} _LL60:
if( _temp56 ==( void*) Cyc_Absyn_MemKind){ goto _LL61;} else{ goto _LL62;} _LL62:
if( _temp56 ==( void*) Cyc_Absyn_BoxKind){ goto _LL63;} else{ goto _LL64;} _LL64:
if( _temp56 ==( void*) Cyc_Absyn_RgnKind){ goto _LL65;} else{ goto _LL66;} _LL66:
if( _temp56 ==( void*) Cyc_Absyn_EffKind){ goto _LL67;} else{ goto _LL57;} _LL59:
Cyc_Absyndump_dump(({ unsigned char* _temp68="A"; struct _tagged_string _temp69;
_temp69.curr= _temp68; _temp69.base= _temp68; _temp69.last_plus_one= _temp68 + 2;
_temp69;})); return; _LL61: Cyc_Absyndump_dump(({ unsigned char* _temp70="M";
struct _tagged_string _temp71; _temp71.curr= _temp70; _temp71.base= _temp70;
_temp71.last_plus_one= _temp70 + 2; _temp71;})); return; _LL63: Cyc_Absyndump_dump(({
unsigned char* _temp72="B"; struct _tagged_string _temp73; _temp73.curr= _temp72;
_temp73.base= _temp72; _temp73.last_plus_one= _temp72 + 2; _temp73;})); return;
_LL65: Cyc_Absyndump_dump(({ unsigned char* _temp74="R"; struct _tagged_string
_temp75; _temp75.curr= _temp74; _temp75.base= _temp74; _temp75.last_plus_one=
_temp74 + 2; _temp75;})); return; _LL67: Cyc_Absyndump_dump(({ unsigned char*
_temp76="E"; struct _tagged_string _temp77; _temp77.curr= _temp76; _temp77.base=
_temp76; _temp77.last_plus_one= _temp76 + 2; _temp77;})); return; _LL57:;} void
Cyc_Absyndump_dumptps( struct Cyc_List_List* ts){(( void(*)( void(* f)( void*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumptyp, ts,({
unsigned char* _temp78="<"; struct _tagged_string _temp79; _temp79.curr= _temp78;
_temp79.base= _temp78; _temp79.last_plus_one= _temp78 + 2; _temp79;}),({
unsigned char* _temp80=">"; struct _tagged_string _temp81; _temp81.curr= _temp80;
_temp81.base= _temp80; _temp81.last_plus_one= _temp80 + 2; _temp81;}),({
unsigned char* _temp82=","; struct _tagged_string _temp83; _temp83.curr= _temp82;
_temp83.base= _temp82; _temp83.last_plus_one= _temp82 + 2; _temp83;}));} void
Cyc_Absyndump_dumptvar( struct Cyc_Absyn_Tvar* tv){ Cyc_Absyndump_dump_str( tv->name);}
void Cyc_Absyndump_dumpkindedtvar( struct Cyc_Absyn_Tvar* tv){ Cyc_Absyndump_dump_str(
tv->name);{ void* _temp84=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( tv->kind))->v; void* _temp92; void* _temp94;
_LL86: if(( unsigned int) _temp84 > 1u?*(( int*) _temp84) == Cyc_Absyn_Eq_constr:
0){ _LL93: _temp92=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp84)->f1;
if( _temp92 ==( void*) Cyc_Absyn_BoxKind){ goto _LL87;} else{ goto _LL88;}}
else{ goto _LL88;} _LL88: if(( unsigned int) _temp84 > 1u?*(( int*) _temp84) ==
Cyc_Absyn_Eq_constr: 0){ _LL95: _temp94=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp84)->f1; goto _LL89;} else{ goto _LL90;} _LL90: goto _LL91; _LL87: goto
_LL85; _LL89: Cyc_Absyndump_dump(({ unsigned char* _temp96="::"; struct
_tagged_string _temp97; _temp97.curr= _temp96; _temp97.base= _temp96; _temp97.last_plus_one=
_temp96 + 3; _temp97;})); Cyc_Absyndump_dumpkind( _temp94); goto _LL85; _LL91:
Cyc_Absyndump_dump(({ unsigned char* _temp98="::?"; struct _tagged_string
_temp99; _temp99.curr= _temp98; _temp99.base= _temp98; _temp99.last_plus_one=
_temp98 + 4; _temp99;})); goto _LL85; _LL85:;}} void Cyc_Absyndump_dumptvars(
struct Cyc_List_List* tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumptvar, tvs,({
unsigned char* _temp100="<"; struct _tagged_string _temp101; _temp101.curr=
_temp100; _temp101.base= _temp100; _temp101.last_plus_one= _temp100 + 2;
_temp101;}),({ unsigned char* _temp102=">"; struct _tagged_string _temp103;
_temp103.curr= _temp102; _temp103.base= _temp102; _temp103.last_plus_one=
_temp102 + 2; _temp103;}),({ unsigned char* _temp104=","; struct _tagged_string
_temp105; _temp105.curr= _temp104; _temp105.base= _temp104; _temp105.last_plus_one=
_temp104 + 2; _temp105;}));} void Cyc_Absyndump_dumpkindedtvars( struct Cyc_List_List*
tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumpkindedtvar, tvs,({ unsigned char*
_temp106="<"; struct _tagged_string _temp107; _temp107.curr= _temp106; _temp107.base=
_temp106; _temp107.last_plus_one= _temp106 + 2; _temp107;}),({ unsigned char*
_temp108=">"; struct _tagged_string _temp109; _temp109.curr= _temp108; _temp109.base=
_temp108; _temp109.last_plus_one= _temp108 + 2; _temp109;}),({ unsigned char*
_temp110=","; struct _tagged_string _temp111; _temp111.curr= _temp110; _temp111.base=
_temp110; _temp111.last_plus_one= _temp110 + 2; _temp111;}));} struct _tuple5{
struct Cyc_Absyn_Tqual f1; void* f2; } ; void Cyc_Absyndump_dumparg( struct
_tuple5* pr){(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( int), int))
Cyc_Absyndump_dumptqtd)((* pr).f1,(* pr).f2,( void(*)( int x)) Cyc_Absyndump_ignore,
0);} void Cyc_Absyndump_dumpargs( struct Cyc_List_List* ts){(( void(*)( void(* f)(
struct _tuple5*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumparg,
ts,({ unsigned char* _temp112="("; struct _tagged_string _temp113; _temp113.curr=
_temp112; _temp113.base= _temp112; _temp113.last_plus_one= _temp112 + 2;
_temp113;}),({ unsigned char* _temp114=")"; struct _tagged_string _temp115;
_temp115.curr= _temp114; _temp115.base= _temp114; _temp115.last_plus_one=
_temp114 + 2; _temp115;}),({ unsigned char* _temp116=","; struct _tagged_string
_temp117; _temp117.curr= _temp116; _temp117.base= _temp116; _temp117.last_plus_one=
_temp116 + 2; _temp117;}));} void Cyc_Absyndump_dump_callconv( struct Cyc_List_List*
atts){ for( 0; atts != 0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){
void* _temp118=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd; _LL120:
if( _temp118 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL121;} else{ goto _LL122;}
_LL122: if( _temp118 ==( void*) Cyc_Absyn_Cdecl_att){ goto _LL123;} else{ goto
_LL124;} _LL124: if( _temp118 ==( void*) Cyc_Absyn_Fastcall_att){ goto _LL125;}
else{ goto _LL126;} _LL126: goto _LL127; _LL121: Cyc_Absyndump_dump(({
unsigned char* _temp128="_stdcall"; struct _tagged_string _temp129; _temp129.curr=
_temp128; _temp129.base= _temp128; _temp129.last_plus_one= _temp128 + 9;
_temp129;})); return; _LL123: Cyc_Absyndump_dump(({ unsigned char* _temp130="_cdecl";
struct _tagged_string _temp131; _temp131.curr= _temp130; _temp131.base= _temp130;
_temp131.last_plus_one= _temp130 + 7; _temp131;})); return; _LL125: Cyc_Absyndump_dump(({
unsigned char* _temp132="_fastcall"; struct _tagged_string _temp133; _temp133.curr=
_temp132; _temp133.base= _temp132; _temp133.last_plus_one= _temp132 + 10;
_temp133;})); return; _LL127: goto _LL119; _LL119:;}} void Cyc_Absyndump_dump_noncallconv(
struct Cyc_List_List* atts){ int hasatt= 0;{ struct Cyc_List_List* atts2= atts;
for( 0; atts2 != 0; atts2=(( struct Cyc_List_List*) _check_null( atts2))->tl){
void* _temp134=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd; _LL136:
if( _temp134 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL137;} else{ goto _LL138;}
_LL138: if( _temp134 ==( void*) Cyc_Absyn_Cdecl_att){ goto _LL139;} else{ goto
_LL140;} _LL140: if( _temp134 ==( void*) Cyc_Absyn_Fastcall_att){ goto _LL141;}
else{ goto _LL142;} _LL142: goto _LL143; _LL137: goto _LL135; _LL139: goto
_LL135; _LL141: goto _LL135; _LL143: hasatt= 1; goto _LL135; _LL135:;}} if( !
hasatt){ return;} Cyc_Absyndump_dump(({ unsigned char* _temp144="__declspec(";
struct _tagged_string _temp145; _temp145.curr= _temp144; _temp145.base= _temp144;
_temp145.last_plus_one= _temp144 + 12; _temp145;})); for( 0; atts != 0; atts=((
struct Cyc_List_List*) _check_null( atts))->tl){ void* _temp146=( void*)((
struct Cyc_List_List*) _check_null( atts))->hd; _LL148: if( _temp146 ==( void*)
Cyc_Absyn_Stdcall_att){ goto _LL149;} else{ goto _LL150;} _LL150: if( _temp146
==( void*) Cyc_Absyn_Cdecl_att){ goto _LL151;} else{ goto _LL152;} _LL152: if(
_temp146 ==( void*) Cyc_Absyn_Fastcall_att){ goto _LL153;} else{ goto _LL154;}
_LL154: goto _LL155; _LL149: goto _LL147; _LL151: goto _LL147; _LL153: goto
_LL147; _LL155: Cyc_Absyndump_dump( Cyc_Absyn_attribute2string(( void*)(( struct
Cyc_List_List*) _check_null( atts))->hd)); goto _LL147; _LL147:;} Cyc_Absyndump_dump_char((
int)')');} void Cyc_Absyndump_dumpatts( struct Cyc_List_List* atts){ if( atts ==
0){ return;} if( Cyc_Absynpp_to_VC){ Cyc_Absyndump_dump_noncallconv( atts);
return;} Cyc_Absyndump_dump(({ unsigned char* _temp156=" __attribute__((";
struct _tagged_string _temp157; _temp157.curr= _temp156; _temp157.base= _temp156;
_temp157.last_plus_one= _temp156 + 17; _temp157;})); for( 0; atts != 0; atts=((
struct Cyc_List_List*) _check_null( atts))->tl){ Cyc_Absyndump_dump( Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd)); if((( struct Cyc_List_List*)
_check_null( atts))->tl != 0){ Cyc_Absyndump_dump(({ unsigned char* _temp158=",";
struct _tagged_string _temp159; _temp159.curr= _temp158; _temp159.base= _temp158;
_temp159.last_plus_one= _temp158 + 2; _temp159;}));}} Cyc_Absyndump_dump(({
unsigned char* _temp160=")) "; struct _tagged_string _temp161; _temp161.curr=
_temp160; _temp161.base= _temp160; _temp161.last_plus_one= _temp160 + 4;
_temp161;}));} int Cyc_Absyndump_next_is_pointer( struct Cyc_List_List* tms){
if( tms == 0){ return 0;}{ void* _temp162=( void*)(( struct Cyc_List_List*)
_check_null( tms))->hd; struct Cyc_Absyn_Tqual _temp168; void* _temp170; void*
_temp172; _LL164: if(( unsigned int) _temp162 > 1u?*(( int*) _temp162) == Cyc_Absyn_Pointer_mod:
0){ _LL173: _temp172=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp162)->f1;
goto _LL171; _LL171: _temp170=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp162)->f2; goto _LL169; _LL169: _temp168=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp162)->f3; goto _LL165;} else{ goto _LL166;} _LL166: goto _LL167; _LL165:
return 1; _LL167: return 0; _LL163:;}} static void Cyc_Absyndump_dumprgn( void*
t){ void* _temp174= Cyc_Tcutil_compress( t); _LL176: if( _temp174 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL177;} else{ goto _LL178;} _LL178: goto _LL179; _LL177: Cyc_Absyndump_dump(({
unsigned char* _temp180="`H"; struct _tagged_string _temp181; _temp181.curr=
_temp180; _temp181.base= _temp180; _temp181.last_plus_one= _temp180 + 3;
_temp181;})); goto _LL175; _LL179: Cyc_Absyndump_dumpntyp( t); goto _LL175;
_LL175:;} struct _tuple6{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; }
; static struct _tuple6 Cyc_Absyndump_effects_split( void* t){ struct Cyc_List_List*
regions= 0; struct Cyc_List_List* effects= 0;{ void* _temp182= Cyc_Tcutil_compress(
t); void* _temp190; struct Cyc_List_List* _temp192; _LL184: if(( unsigned int)
_temp182 > 4u?*(( int*) _temp182) == Cyc_Absyn_AccessEff: 0){ _LL191: _temp190=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp182)->f1; goto _LL185;} else{
goto _LL186;} _LL186: if(( unsigned int) _temp182 > 4u?*(( int*) _temp182) ==
Cyc_Absyn_JoinEff: 0){ _LL193: _temp192=(( struct Cyc_Absyn_JoinEff_struct*)
_temp182)->f1; goto _LL187;} else{ goto _LL188;} _LL188: goto _LL189; _LL185:
regions=({ struct Cyc_List_List* _temp194=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp194->hd=( void*) _temp190; _temp194->tl=
regions; _temp194;}); goto _LL183; _LL187: for( 0; _temp192 != 0; _temp192=((
struct Cyc_List_List*) _check_null( _temp192))->tl){ struct Cyc_List_List*
_temp197; struct Cyc_List_List* _temp199; struct _tuple6 _temp195= Cyc_Absyndump_effects_split((
void*)(( struct Cyc_List_List*) _check_null( _temp192))->hd); _LL200: _temp199=
_temp195.f1; goto _LL198; _LL198: _temp197= _temp195.f2; goto _LL196; _LL196:
regions=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( _temp199, regions); effects=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp197, effects);} goto _LL183; _LL189: effects=({ struct Cyc_List_List*
_temp201=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp201->hd=( void*) t; _temp201->tl= effects; _temp201;}); goto _LL183; _LL183:;}
return({ struct _tuple6 _temp202; _temp202.f1= regions; _temp202.f2= effects;
_temp202;});} static void Cyc_Absyndump_dumpeff( void* t){ struct Cyc_List_List*
_temp205; struct Cyc_List_List* _temp207; struct _tuple6 _temp203= Cyc_Absyndump_effects_split(
t); _LL208: _temp207= _temp203.f1; goto _LL206; _LL206: _temp205= _temp203.f2;
goto _LL204; _LL204: _temp207=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp207); _temp205=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( _temp205); for( 0; _temp205 != 0; _temp205=((
struct Cyc_List_List*) _check_null( _temp205))->tl){ Cyc_Absyndump_dumpntyp((
void*)(( struct Cyc_List_List*) _check_null( _temp205))->hd); Cyc_Absyndump_dump_char((
int)'+');} Cyc_Absyndump_dump_char(( int)'{'); for( 0; _temp207 != 0; _temp207=((
struct Cyc_List_List*) _check_null( _temp207))->tl){ Cyc_Absyndump_dumprgn((
void*)(( struct Cyc_List_List*) _check_null( _temp207))->hd); if((( struct Cyc_List_List*)
_check_null( _temp207))->tl != 0){ Cyc_Absyndump_dump_char(( int)',');}} Cyc_Absyndump_dump_char((
int)'}');} void Cyc_Absyndump_dumpntyp( void* t){ void* _temp209= t; struct Cyc_Absyn_Exp*
_temp279; struct Cyc_Absyn_Tqual _temp281; void* _temp283; struct Cyc_Absyn_FnInfo
_temp285; struct Cyc_Absyn_PtrInfo _temp287; struct Cyc_Absyn_Tvar* _temp289;
struct Cyc_Core_Opt* _temp291; int _temp293; struct Cyc_Core_Opt* _temp295;
struct Cyc_Core_Opt* _temp297; struct Cyc_Core_Opt* _temp299; int _temp301;
struct Cyc_Core_Opt* _temp303; struct Cyc_Core_Opt _temp305; void* _temp306;
struct Cyc_Core_Opt* _temp308; struct Cyc_Absyn_TunionInfo _temp310; void*
_temp312; struct Cyc_List_List* _temp314; void* _temp316; struct Cyc_Absyn_TunionFieldInfo
_temp318; struct Cyc_List_List* _temp320; void* _temp322; struct Cyc_Absyn_Enumdecl*
_temp324; struct _tuple0* _temp326; void* _temp328; void* _temp330; void*
_temp332; void* _temp334; void* _temp336; void* _temp338; void* _temp340; void*
_temp342; void* _temp344; void* _temp346; void* _temp348; void* _temp350; void*
_temp353; void* _temp355; void* _temp357; void* _temp359; void* _temp362; void*
_temp364; void* _temp366; void* _temp368; struct Cyc_List_List* _temp370; struct
Cyc_Absyn_Structdecl** _temp372; struct Cyc_List_List* _temp374; struct _tuple0*
_temp376; struct Cyc_Absyn_Structdecl** _temp378; struct Cyc_List_List* _temp380;
struct _tuple0* _temp382; struct Cyc_Absyn_Uniondecl** _temp384; struct Cyc_List_List*
_temp386; struct _tuple0* _temp388; struct Cyc_Absyn_Uniondecl** _temp390;
struct Cyc_List_List* _temp392; struct _tuple0* _temp394; struct Cyc_List_List*
_temp396; struct Cyc_List_List* _temp398; struct Cyc_Core_Opt* _temp400; struct
Cyc_List_List* _temp402; struct _tuple0* _temp404; void* _temp406; void*
_temp408; struct Cyc_List_List* _temp410; _LL211: if(( unsigned int) _temp209 >
4u?*(( int*) _temp209) == Cyc_Absyn_ArrayType: 0){ _LL284: _temp283=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp209)->f1; goto _LL282; _LL282: _temp281=((
struct Cyc_Absyn_ArrayType_struct*) _temp209)->f2; goto _LL280; _LL280: _temp279=((
struct Cyc_Absyn_ArrayType_struct*) _temp209)->f3; goto _LL212;} else{ goto
_LL213;} _LL213: if(( unsigned int) _temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_FnType:
0){ _LL286: _temp285=(( struct Cyc_Absyn_FnType_struct*) _temp209)->f1; goto
_LL214;} else{ goto _LL215;} _LL215: if(( unsigned int) _temp209 > 4u?*(( int*)
_temp209) == Cyc_Absyn_PointerType: 0){ _LL288: _temp287=(( struct Cyc_Absyn_PointerType_struct*)
_temp209)->f1; goto _LL216;} else{ goto _LL217;} _LL217: if( _temp209 ==( void*)
Cyc_Absyn_VoidType){ goto _LL218;} else{ goto _LL219;} _LL219: if(( unsigned int)
_temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_VarType: 0){ _LL290: _temp289=((
struct Cyc_Absyn_VarType_struct*) _temp209)->f1; goto _LL220;} else{ goto _LL221;}
_LL221: if(( unsigned int) _temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_Evar:
0){ _LL298: _temp297=(( struct Cyc_Absyn_Evar_struct*) _temp209)->f1; goto
_LL296; _LL296: _temp295=(( struct Cyc_Absyn_Evar_struct*) _temp209)->f2; if(
_temp295 == 0){ goto _LL294;} else{ goto _LL223;} _LL294: _temp293=(( struct Cyc_Absyn_Evar_struct*)
_temp209)->f3; goto _LL292; _LL292: _temp291=(( struct Cyc_Absyn_Evar_struct*)
_temp209)->f4; goto _LL222;} else{ goto _LL223;} _LL223: if(( unsigned int)
_temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_Evar: 0){ _LL309: _temp308=((
struct Cyc_Absyn_Evar_struct*) _temp209)->f1; goto _LL304; _LL304: _temp303=((
struct Cyc_Absyn_Evar_struct*) _temp209)->f2; if( _temp303 == 0){ goto _LL225;}
else{ _temp305=* _temp303; _LL307: _temp306=( void*) _temp305.v; goto _LL302;}
_LL302: _temp301=(( struct Cyc_Absyn_Evar_struct*) _temp209)->f3; goto _LL300;
_LL300: _temp299=(( struct Cyc_Absyn_Evar_struct*) _temp209)->f4; goto _LL224;}
else{ goto _LL225;} _LL225: if(( unsigned int) _temp209 > 4u?*(( int*) _temp209)
== Cyc_Absyn_TunionType: 0){ _LL311: _temp310=(( struct Cyc_Absyn_TunionType_struct*)
_temp209)->f1; _LL317: _temp316=( void*) _temp310.tunion_info; goto _LL315;
_LL315: _temp314= _temp310.targs; goto _LL313; _LL313: _temp312=( void*)
_temp310.rgn; goto _LL226;} else{ goto _LL227;} _LL227: if(( unsigned int)
_temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_TunionFieldType: 0){ _LL319:
_temp318=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp209)->f1; _LL323:
_temp322=( void*) _temp318.field_info; goto _LL321; _LL321: _temp320= _temp318.targs;
goto _LL228;} else{ goto _LL229;} _LL229: if(( unsigned int) _temp209 > 4u?*((
int*) _temp209) == Cyc_Absyn_EnumType: 0){ _LL327: _temp326=(( struct Cyc_Absyn_EnumType_struct*)
_temp209)->f1; goto _LL325; _LL325: _temp324=(( struct Cyc_Absyn_EnumType_struct*)
_temp209)->f2; goto _LL230;} else{ goto _LL231;} _LL231: if(( unsigned int)
_temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_IntType: 0){ _LL331: _temp330=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp209)->f1; if( _temp330 ==( void*)
Cyc_Absyn_Signed){ goto _LL329;} else{ goto _LL233;} _LL329: _temp328=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp209)->f2; if( _temp328 ==( void*) Cyc_Absyn_B4){
goto _LL232;} else{ goto _LL233;}} else{ goto _LL233;} _LL233: if(( unsigned int)
_temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_IntType: 0){ _LL335: _temp334=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp209)->f1; if( _temp334 ==( void*)
Cyc_Absyn_Signed){ goto _LL333;} else{ goto _LL235;} _LL333: _temp332=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp209)->f2; if( _temp332 ==( void*) Cyc_Absyn_B1){
goto _LL234;} else{ goto _LL235;}} else{ goto _LL235;} _LL235: if(( unsigned int)
_temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_IntType: 0){ _LL339: _temp338=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp209)->f1; if( _temp338 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL337;} else{ goto _LL237;} _LL337: _temp336=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp209)->f2; if( _temp336 ==( void*) Cyc_Absyn_B1){
goto _LL236;} else{ goto _LL237;}} else{ goto _LL237;} _LL237: if(( unsigned int)
_temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_IntType: 0){ _LL343: _temp342=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp209)->f1; if( _temp342 ==( void*)
Cyc_Absyn_Signed){ goto _LL341;} else{ goto _LL239;} _LL341: _temp340=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp209)->f2; if( _temp340 ==( void*) Cyc_Absyn_B2){
goto _LL238;} else{ goto _LL239;}} else{ goto _LL239;} _LL239: if(( unsigned int)
_temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_IntType: 0){ _LL347: _temp346=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp209)->f1; if( _temp346 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL345;} else{ goto _LL241;} _LL345: _temp344=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp209)->f2; if( _temp344 ==( void*) Cyc_Absyn_B2){
goto _LL240;} else{ goto _LL241;}} else{ goto _LL241;} _LL241: if(( unsigned int)
_temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_IntType: 0){ _LL351: _temp350=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp209)->f1; if( _temp350 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL349;} else{ goto _LL243;} _LL349: _temp348=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp209)->f2; if( _temp348 ==( void*) Cyc_Absyn_B4){
goto _LL242;} else{ goto _LL243;}} else{ goto _LL243;} _LL243: if(( unsigned int)
_temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_IntType: 0){ _LL356: _temp355=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp209)->f1; if( _temp355 ==( void*)
Cyc_Absyn_Signed){ goto _LL354;} else{ goto _LL245;} _LL354: _temp353=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp209)->f2; if( _temp353 ==( void*) Cyc_Absyn_B8){
goto _LL352;} else{ goto _LL245;}} else{ goto _LL245;} _LL352: if( Cyc_Absynpp_to_VC){
goto _LL244;} else{ goto _LL245;} _LL245: if(( unsigned int) _temp209 > 4u?*((
int*) _temp209) == Cyc_Absyn_IntType: 0){ _LL360: _temp359=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp209)->f1; if( _temp359 ==( void*) Cyc_Absyn_Signed){ goto _LL358;} else{
goto _LL247;} _LL358: _temp357=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp209)->f2; if( _temp357 ==( void*) Cyc_Absyn_B8){ goto _LL246;} else{ goto
_LL247;}} else{ goto _LL247;} _LL247: if(( unsigned int) _temp209 > 4u?*(( int*)
_temp209) == Cyc_Absyn_IntType: 0){ _LL365: _temp364=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp209)->f1; if( _temp364 ==( void*) Cyc_Absyn_Unsigned){ goto _LL363;} else{
goto _LL249;} _LL363: _temp362=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp209)->f2; if( _temp362 ==( void*) Cyc_Absyn_B8){ goto _LL361;} else{ goto
_LL249;}} else{ goto _LL249;} _LL361: if( Cyc_Absynpp_to_VC){ goto _LL248;}
else{ goto _LL249;} _LL249: if(( unsigned int) _temp209 > 4u?*(( int*) _temp209)
== Cyc_Absyn_IntType: 0){ _LL369: _temp368=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp209)->f1; if( _temp368 ==( void*) Cyc_Absyn_Unsigned){ goto _LL367;} else{
goto _LL251;} _LL367: _temp366=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp209)->f2; if( _temp366 ==( void*) Cyc_Absyn_B8){ goto _LL250;} else{ goto
_LL251;}} else{ goto _LL251;} _LL251: if( _temp209 ==( void*) Cyc_Absyn_FloatType){
goto _LL252;} else{ goto _LL253;} _LL253: if( _temp209 ==( void*) Cyc_Absyn_DoubleType){
goto _LL254;} else{ goto _LL255;} _LL255: if(( unsigned int) _temp209 > 4u?*((
int*) _temp209) == Cyc_Absyn_TupleType: 0){ _LL371: _temp370=(( struct Cyc_Absyn_TupleType_struct*)
_temp209)->f1; goto _LL256;} else{ goto _LL257;} _LL257: if(( unsigned int)
_temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_StructType: 0){ _LL377: _temp376=((
struct Cyc_Absyn_StructType_struct*) _temp209)->f1; if( _temp376 == 0){ goto
_LL375;} else{ goto _LL259;} _LL375: _temp374=(( struct Cyc_Absyn_StructType_struct*)
_temp209)->f2; goto _LL373; _LL373: _temp372=(( struct Cyc_Absyn_StructType_struct*)
_temp209)->f3; goto _LL258;} else{ goto _LL259;} _LL259: if(( unsigned int)
_temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_StructType: 0){ _LL383: _temp382=((
struct Cyc_Absyn_StructType_struct*) _temp209)->f1; goto _LL381; _LL381:
_temp380=(( struct Cyc_Absyn_StructType_struct*) _temp209)->f2; goto _LL379;
_LL379: _temp378=(( struct Cyc_Absyn_StructType_struct*) _temp209)->f3; goto
_LL260;} else{ goto _LL261;} _LL261: if(( unsigned int) _temp209 > 4u?*(( int*)
_temp209) == Cyc_Absyn_UnionType: 0){ _LL389: _temp388=(( struct Cyc_Absyn_UnionType_struct*)
_temp209)->f1; if( _temp388 == 0){ goto _LL387;} else{ goto _LL263;} _LL387:
_temp386=(( struct Cyc_Absyn_UnionType_struct*) _temp209)->f2; goto _LL385;
_LL385: _temp384=(( struct Cyc_Absyn_UnionType_struct*) _temp209)->f3; goto
_LL262;} else{ goto _LL263;} _LL263: if(( unsigned int) _temp209 > 4u?*(( int*)
_temp209) == Cyc_Absyn_UnionType: 0){ _LL395: _temp394=(( struct Cyc_Absyn_UnionType_struct*)
_temp209)->f1; goto _LL393; _LL393: _temp392=(( struct Cyc_Absyn_UnionType_struct*)
_temp209)->f2; goto _LL391; _LL391: _temp390=(( struct Cyc_Absyn_UnionType_struct*)
_temp209)->f3; goto _LL264;} else{ goto _LL265;} _LL265: if(( unsigned int)
_temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_AnonStructType: 0){ _LL397:
_temp396=(( struct Cyc_Absyn_AnonStructType_struct*) _temp209)->f1; goto _LL266;}
else{ goto _LL267;} _LL267: if(( unsigned int) _temp209 > 4u?*(( int*) _temp209)
== Cyc_Absyn_AnonUnionType: 0){ _LL399: _temp398=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp209)->f1; goto _LL268;} else{ goto _LL269;} _LL269: if(( unsigned int)
_temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_TypedefType: 0){ _LL405: _temp404=((
struct Cyc_Absyn_TypedefType_struct*) _temp209)->f1; goto _LL403; _LL403:
_temp402=(( struct Cyc_Absyn_TypedefType_struct*) _temp209)->f2; goto _LL401;
_LL401: _temp400=(( struct Cyc_Absyn_TypedefType_struct*) _temp209)->f3; goto
_LL270;} else{ goto _LL271;} _LL271: if(( unsigned int) _temp209 > 4u?*(( int*)
_temp209) == Cyc_Absyn_RgnHandleType: 0){ _LL407: _temp406=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp209)->f1; goto _LL272;} else{ goto _LL273;} _LL273: if( _temp209 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL274;} else{ goto _LL275;} _LL275: if(( unsigned int)
_temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_AccessEff: 0){ _LL409: _temp408=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp209)->f1; goto _LL276;} else{
goto _LL277;} _LL277: if(( unsigned int) _temp209 > 4u?*(( int*) _temp209) ==
Cyc_Absyn_JoinEff: 0){ _LL411: _temp410=(( struct Cyc_Absyn_JoinEff_struct*)
_temp209)->f1; goto _LL278;} else{ goto _LL210;} _LL212: return; _LL214: return;
_LL216: return; _LL218: Cyc_Absyndump_dump(({ unsigned char* _temp412="void";
struct _tagged_string _temp413; _temp413.curr= _temp412; _temp413.base= _temp412;
_temp413.last_plus_one= _temp412 + 5; _temp413;})); return; _LL220: Cyc_Absyndump_dump_str(
_temp289->name); return; _LL222: Cyc_Absyndump_dump(({ unsigned char* _temp414="%";
struct _tagged_string _temp415; _temp415.curr= _temp414; _temp415.base= _temp414;
_temp415.last_plus_one= _temp414 + 2; _temp415;})); if( _temp297 == 0){ Cyc_Absyndump_dump(({
unsigned char* _temp416="?"; struct _tagged_string _temp417; _temp417.curr=
_temp416; _temp417.base= _temp416; _temp417.last_plus_one= _temp416 + 2;
_temp417;}));} else{ Cyc_Absyndump_dumpkind(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp297))->v);} Cyc_Absyndump_dump( xprintf("(%d)", _temp293));
return; _LL224: Cyc_Absyndump_dumpntyp( _temp306); return; _LL226:{ void*
_temp418= _temp316; struct Cyc_Absyn_UnknownTunionInfo _temp424; int _temp426;
struct _tuple0* _temp428; struct Cyc_Absyn_Tuniondecl* _temp430; struct Cyc_Absyn_Tuniondecl
_temp432; int _temp433; struct Cyc_Core_Opt* _temp435; struct Cyc_List_List*
_temp437; struct _tuple0* _temp439; void* _temp441; _LL420: if(*(( int*)
_temp418) == Cyc_Absyn_UnknownTunion){ _LL425: _temp424=(( struct Cyc_Absyn_UnknownTunion_struct*)
_temp418)->f1; _LL429: _temp428= _temp424.name; goto _LL427; _LL427: _temp426=
_temp424.is_xtunion; goto _LL421;} else{ goto _LL422;} _LL422: if(*(( int*)
_temp418) == Cyc_Absyn_KnownTunion){ _LL431: _temp430=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp418)->f1; _temp432=* _temp430; _LL442: _temp441=( void*) _temp432.sc; goto
_LL440; _LL440: _temp439= _temp432.name; goto _LL438; _LL438: _temp437= _temp432.tvs;
goto _LL436; _LL436: _temp435= _temp432.fields; goto _LL434; _LL434: _temp433=
_temp432.is_xtunion; goto _LL423;} else{ goto _LL419;} _LL421: _temp439=
_temp428; _temp433= _temp426; goto _LL423; _LL423: if( _temp433){ Cyc_Absyndump_dump(({
unsigned char* _temp443="xtunion "; struct _tagged_string _temp444; _temp444.curr=
_temp443; _temp444.base= _temp443; _temp444.last_plus_one= _temp443 + 9;
_temp444;}));} else{ Cyc_Absyndump_dump(({ unsigned char* _temp445="tunion ";
struct _tagged_string _temp446; _temp446.curr= _temp445; _temp446.base= _temp445;
_temp446.last_plus_one= _temp445 + 8; _temp446;}));}{ void* _temp447= Cyc_Tcutil_compress(
_temp312); _LL449: if( _temp447 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL450;}
else{ goto _LL451;} _LL451: goto _LL452; _LL450: goto _LL448; _LL452: Cyc_Absyndump_dumptyp(
_temp312); Cyc_Absyndump_dump(({ unsigned char* _temp453=" "; struct
_tagged_string _temp454; _temp454.curr= _temp453; _temp454.base= _temp453;
_temp454.last_plus_one= _temp453 + 2; _temp454;})); goto _LL448; _LL448:;} Cyc_Absyndump_dumpqvar(
_temp439); Cyc_Absyndump_dumptps( _temp314); goto _LL419; _LL419:;} goto _LL210;
_LL228:{ void* _temp455= _temp322; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp461; int _temp463; struct _tuple0* _temp465; struct _tuple0* _temp467;
struct Cyc_Absyn_Tunionfield* _temp469; struct Cyc_Absyn_Tunionfield _temp471;
void* _temp472; struct Cyc_Position_Segment* _temp474; struct Cyc_List_List*
_temp476; struct Cyc_List_List* _temp478; struct _tuple0* _temp480; struct Cyc_Absyn_Tuniondecl*
_temp482; struct Cyc_Absyn_Tuniondecl _temp484; int _temp485; struct Cyc_Core_Opt*
_temp487; struct Cyc_List_List* _temp489; struct _tuple0* _temp491; void*
_temp493; _LL457: if(*(( int*) _temp455) == Cyc_Absyn_UnknownTunionfield){
_LL462: _temp461=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp455)->f1;
_LL468: _temp467= _temp461.tunion_name; goto _LL466; _LL466: _temp465= _temp461.field_name;
goto _LL464; _LL464: _temp463= _temp461.is_xtunion; goto _LL458;} else{ goto
_LL459;} _LL459: if(*(( int*) _temp455) == Cyc_Absyn_KnownTunionfield){ _LL483:
_temp482=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp455)->f1; _temp484=*
_temp482; _LL494: _temp493=( void*) _temp484.sc; goto _LL492; _LL492: _temp491=
_temp484.name; goto _LL490; _LL490: _temp489= _temp484.tvs; goto _LL488; _LL488:
_temp487= _temp484.fields; goto _LL486; _LL486: _temp485= _temp484.is_xtunion;
goto _LL470; _LL470: _temp469=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp455)->f2; _temp471=* _temp469; _LL481: _temp480= _temp471.name; goto _LL479;
_LL479: _temp478= _temp471.tvs; goto _LL477; _LL477: _temp476= _temp471.typs;
goto _LL475; _LL475: _temp474= _temp471.loc; goto _LL473; _LL473: _temp472=(
void*) _temp471.sc; goto _LL460;} else{ goto _LL456;} _LL458: _temp491= _temp467;
_temp485= _temp463; _temp480= _temp465; goto _LL460; _LL460: if( _temp485){ Cyc_Absyndump_dump(({
unsigned char* _temp495="xtunion "; struct _tagged_string _temp496; _temp496.curr=
_temp495; _temp496.base= _temp495; _temp496.last_plus_one= _temp495 + 9;
_temp496;}));} else{ Cyc_Absyndump_dump(({ unsigned char* _temp497="tunion ";
struct _tagged_string _temp498; _temp498.curr= _temp497; _temp498.base= _temp497;
_temp498.last_plus_one= _temp497 + 8; _temp498;}));} Cyc_Absyndump_dumpqvar(
_temp491); Cyc_Absyndump_dump(({ unsigned char* _temp499="."; struct
_tagged_string _temp500; _temp500.curr= _temp499; _temp500.base= _temp499;
_temp500.last_plus_one= _temp499 + 2; _temp500;})); Cyc_Absyndump_dumpqvar(
_temp480); Cyc_Absyndump_dumptps( _temp320); goto _LL456; _LL456:;} goto _LL210;
_LL230: Cyc_Absyndump_dump(({ unsigned char* _temp501="enum "; struct
_tagged_string _temp502; _temp502.curr= _temp501; _temp502.base= _temp501;
_temp502.last_plus_one= _temp501 + 6; _temp502;})); Cyc_Absyndump_dumpqvar(
_temp326); return; _LL232: Cyc_Absyndump_dump(({ unsigned char* _temp503="int";
struct _tagged_string _temp504; _temp504.curr= _temp503; _temp504.base= _temp503;
_temp504.last_plus_one= _temp503 + 4; _temp504;})); return; _LL234: Cyc_Absyndump_dump(({
unsigned char* _temp505="signed char"; struct _tagged_string _temp506; _temp506.curr=
_temp505; _temp506.base= _temp505; _temp506.last_plus_one= _temp505 + 12;
_temp506;})); return; _LL236: Cyc_Absyndump_dump(({ unsigned char* _temp507="unsigned char";
struct _tagged_string _temp508; _temp508.curr= _temp507; _temp508.base= _temp507;
_temp508.last_plus_one= _temp507 + 14; _temp508;})); return; _LL238: Cyc_Absyndump_dump(({
unsigned char* _temp509="short"; struct _tagged_string _temp510; _temp510.curr=
_temp509; _temp510.base= _temp509; _temp510.last_plus_one= _temp509 + 6;
_temp510;})); return; _LL240: Cyc_Absyndump_dump(({ unsigned char* _temp511="unsigned short";
struct _tagged_string _temp512; _temp512.curr= _temp511; _temp512.base= _temp511;
_temp512.last_plus_one= _temp511 + 15; _temp512;})); return; _LL242: Cyc_Absyndump_dump(({
unsigned char* _temp513="unsigned int"; struct _tagged_string _temp514; _temp514.curr=
_temp513; _temp514.base= _temp513; _temp514.last_plus_one= _temp513 + 13;
_temp514;})); return; _LL244: Cyc_Absyndump_dump(({ unsigned char* _temp515="__int64";
struct _tagged_string _temp516; _temp516.curr= _temp515; _temp516.base= _temp515;
_temp516.last_plus_one= _temp515 + 8; _temp516;})); return; _LL246: Cyc_Absyndump_dump(({
unsigned char* _temp517="long long"; struct _tagged_string _temp518; _temp518.curr=
_temp517; _temp518.base= _temp517; _temp518.last_plus_one= _temp517 + 10;
_temp518;})); return; _LL248: Cyc_Absyndump_dump(({ unsigned char* _temp519="unsigned __int64";
struct _tagged_string _temp520; _temp520.curr= _temp519; _temp520.base= _temp519;
_temp520.last_plus_one= _temp519 + 17; _temp520;})); return; _LL250: Cyc_Absyndump_dump(({
unsigned char* _temp521="unsigned long long"; struct _tagged_string _temp522;
_temp522.curr= _temp521; _temp522.base= _temp521; _temp522.last_plus_one=
_temp521 + 19; _temp522;})); return; _LL252: Cyc_Absyndump_dump(({ unsigned char*
_temp523="float"; struct _tagged_string _temp524; _temp524.curr= _temp523;
_temp524.base= _temp523; _temp524.last_plus_one= _temp523 + 6; _temp524;}));
return; _LL254: Cyc_Absyndump_dump(({ unsigned char* _temp525="double"; struct
_tagged_string _temp526; _temp526.curr= _temp525; _temp526.base= _temp525;
_temp526.last_plus_one= _temp525 + 7; _temp526;})); return; _LL256: Cyc_Absyndump_dump_char((
int)'$'); Cyc_Absyndump_dumpargs( _temp370); return; _LL258: Cyc_Absyndump_dump(({
unsigned char* _temp527="struct"; struct _tagged_string _temp528; _temp528.curr=
_temp527; _temp528.base= _temp527; _temp528.last_plus_one= _temp527 + 7;
_temp528;})); Cyc_Absyndump_dumptps( _temp374); return; _LL260: Cyc_Absyndump_dump(({
unsigned char* _temp529="struct"; struct _tagged_string _temp530; _temp530.curr=
_temp529; _temp530.base= _temp529; _temp530.last_plus_one= _temp529 + 7;
_temp530;})); Cyc_Absyndump_dumpqvar(( struct _tuple0*) _check_null( _temp382));
Cyc_Absyndump_dumptps( _temp380); return; _LL262: Cyc_Absyndump_dump(({
unsigned char* _temp531="union"; struct _tagged_string _temp532; _temp532.curr=
_temp531; _temp532.base= _temp531; _temp532.last_plus_one= _temp531 + 6;
_temp532;})); Cyc_Absyndump_dumptps( _temp386); return; _LL264: Cyc_Absyndump_dump(({
unsigned char* _temp533="union"; struct _tagged_string _temp534; _temp534.curr=
_temp533; _temp534.base= _temp533; _temp534.last_plus_one= _temp533 + 6;
_temp534;})); Cyc_Absyndump_dumpqvar(( struct _tuple0*) _check_null( _temp394));
Cyc_Absyndump_dumptps( _temp392); return; _LL266: Cyc_Absyndump_dump(({
unsigned char* _temp535="struct {"; struct _tagged_string _temp536; _temp536.curr=
_temp535; _temp536.base= _temp535; _temp536.last_plus_one= _temp535 + 9;
_temp536;})); Cyc_Absyndump_dumpstructfields( _temp396); Cyc_Absyndump_dump(({
unsigned char* _temp537="}"; struct _tagged_string _temp538; _temp538.curr=
_temp537; _temp538.base= _temp537; _temp538.last_plus_one= _temp537 + 2;
_temp538;})); return; _LL268: Cyc_Absyndump_dump(({ unsigned char* _temp539="union {";
struct _tagged_string _temp540; _temp540.curr= _temp539; _temp540.base= _temp539;
_temp540.last_plus_one= _temp539 + 8; _temp540;})); Cyc_Absyndump_dumpstructfields(
_temp398); Cyc_Absyndump_dump(({ unsigned char* _temp541="}"; struct
_tagged_string _temp542; _temp542.curr= _temp541; _temp542.base= _temp541;
_temp542.last_plus_one= _temp541 + 2; _temp542;})); return; _LL270:( Cyc_Absyndump_dumpqvar(
_temp404), Cyc_Absyndump_dumptps( _temp402)); return; _LL272: Cyc_Absyndump_dumprgn(
_temp406); return; _LL274: return; _LL276: return; _LL278: return; _LL210:;}
void Cyc_Absyndump_dumpvaropt( struct Cyc_Core_Opt* vo){ if( vo != 0){ Cyc_Absyndump_dump_str((
struct _tagged_string*)(( struct Cyc_Core_Opt*) _check_null( vo))->v);}} void
Cyc_Absyndump_dumpfunarg( struct _tuple1* t){(( void(*)( struct Cyc_Absyn_Tqual,
void*, void(* f)( struct Cyc_Core_Opt*), struct Cyc_Core_Opt*)) Cyc_Absyndump_dumptqtd)((*
t).f2,(* t).f3, Cyc_Absyndump_dumpvaropt,(* t).f1);} void Cyc_Absyndump_dumpfunargs(
struct Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo*
cyc_varargs, struct Cyc_Core_Opt* effopt){ Cyc_Absyndump_dump_char(( int)'(');
for( 0; args != 0; args=(( struct Cyc_List_List*) _check_null( args))->tl){ Cyc_Absyndump_dumpfunarg((
struct _tuple1*)(( struct Cyc_List_List*) _check_null( args))->hd); if((((
struct Cyc_List_List*) _check_null( args))->tl != 0? 1: c_varargs)? 1:
cyc_varargs != 0){ Cyc_Absyndump_dump_char(( int)',');}} if( c_varargs){ Cyc_Absyndump_dump(({
unsigned char* _temp543="..."; struct _tagged_string _temp544; _temp544.curr=
_temp543; _temp544.base= _temp543; _temp544.last_plus_one= _temp543 + 4;
_temp544;}));} else{ if( cyc_varargs != 0){ struct _tuple1* _temp546=({ struct
_tuple1* _temp545=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp545->f1=(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->name;
_temp545->f2=(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->tq;
_temp545->f3=( void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->type;
_temp545;}); goto _LL547; _LL547: Cyc_Absyndump_dump(({ unsigned char* _temp548="...";
struct _tagged_string _temp549; _temp549.curr= _temp548; _temp549.base= _temp548;
_temp549.last_plus_one= _temp548 + 4; _temp549;})); Cyc_Absyndump_dumptyp(( void*)((
struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->rgn); if((( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->inject){ Cyc_Absyndump_dump(({ unsigned char*
_temp550=" inject "; struct _tagged_string _temp551; _temp551.curr= _temp550;
_temp551.base= _temp550; _temp551.last_plus_one= _temp550 + 9; _temp551;}));}
Cyc_Absyndump_dumpfunarg( _temp546);}} if( effopt != 0){ Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpeff(( void*)(( struct Cyc_Core_Opt*) _check_null( effopt))->v);}
Cyc_Absyndump_dump_char(( int)')');} void Cyc_Absyndump_dumptyp( void* t){((
void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( int), int)) Cyc_Absyndump_dumptqtd)(({
struct Cyc_Absyn_Tqual _temp552; _temp552.q_const= 0; _temp552.q_volatile= 0;
_temp552.q_restrict= 0; _temp552;}), t,( void(*)( int x)) Cyc_Absyndump_ignore,
0);} void Cyc_Absyndump_dumpdesignator( void* d){ void* _temp553= d; struct Cyc_Absyn_Exp*
_temp559; struct _tagged_string* _temp561; _LL555: if(*(( int*) _temp553) == Cyc_Absyn_ArrayElement){
_LL560: _temp559=(( struct Cyc_Absyn_ArrayElement_struct*) _temp553)->f1; goto
_LL556;} else{ goto _LL557;} _LL557: if(*(( int*) _temp553) == Cyc_Absyn_FieldName){
_LL562: _temp561=(( struct Cyc_Absyn_FieldName_struct*) _temp553)->f1; goto
_LL558;} else{ goto _LL554;} _LL556: Cyc_Absyndump_dump(({ unsigned char*
_temp563=".["; struct _tagged_string _temp564; _temp564.curr= _temp563; _temp564.base=
_temp563; _temp564.last_plus_one= _temp563 + 3; _temp564;})); Cyc_Absyndump_dumpexp(
_temp559); Cyc_Absyndump_dump_char(( int)']'); goto _LL554; _LL558: Cyc_Absyndump_dump_char((
int)'.'); Cyc_Absyndump_dump_nospace(* _temp561); goto _LL554; _LL554:;} struct
_tuple7{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; void Cyc_Absyndump_dumpde(
struct _tuple7* de){(( void(*)( void(* f)( void*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumpdesignator,(* de).f1,({
unsigned char* _temp565=""; struct _tagged_string _temp566; _temp566.curr=
_temp565; _temp566.base= _temp565; _temp566.last_plus_one= _temp565 + 1;
_temp566;}),({ unsigned char* _temp567="="; struct _tagged_string _temp568;
_temp568.curr= _temp567; _temp568.base= _temp567; _temp568.last_plus_one=
_temp567 + 2; _temp568;}),({ unsigned char* _temp569="="; struct _tagged_string
_temp570; _temp570.curr= _temp569; _temp570.base= _temp569; _temp570.last_plus_one=
_temp569 + 2; _temp570;})); Cyc_Absyndump_dumpexp((* de).f2);} void Cyc_Absyndump_dumpexps_prec(
int inprec, struct Cyc_List_List* es){(( void(*)( void(* f)( int, struct Cyc_Absyn_Exp*),
int env, struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group_c)( Cyc_Absyndump_dumpexp_prec,
inprec, es,({ unsigned char* _temp571=""; struct _tagged_string _temp572;
_temp572.curr= _temp571; _temp572.base= _temp571; _temp572.last_plus_one=
_temp571 + 1; _temp572;}),({ unsigned char* _temp573=""; struct _tagged_string
_temp574; _temp574.curr= _temp573; _temp574.base= _temp573; _temp574.last_plus_one=
_temp573 + 1; _temp574;}),({ unsigned char* _temp575=","; struct _tagged_string
_temp576; _temp576.curr= _temp575; _temp576.base= _temp575; _temp576.last_plus_one=
_temp575 + 2; _temp576;}));} void Cyc_Absyndump_dumpexp_prec( int inprec, struct
Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec( e); if( inprec >= myprec){
Cyc_Absyndump_dump_nospace(({ unsigned char* _temp577="("; struct _tagged_string
_temp578; _temp578.curr= _temp577; _temp578.base= _temp577; _temp578.last_plus_one=
_temp577 + 2; _temp578;}));}{ void* _temp579=( void*) e->r; void* _temp671;
unsigned char _temp673; void* _temp675; void* _temp677; short _temp679; void*
_temp681; void* _temp683; int _temp685; void* _temp687; void* _temp689; int
_temp691; void* _temp693; void* _temp695; long long _temp697; void* _temp699;
void* _temp701; struct _tagged_string _temp703; void* _temp705; void* _temp707;
struct _tagged_string _temp709; struct _tuple0* _temp711; void* _temp713; struct
_tuple0* _temp715; struct Cyc_List_List* _temp717; void* _temp719; struct Cyc_Absyn_Exp*
_temp721; struct Cyc_Core_Opt* _temp723; struct Cyc_Absyn_Exp* _temp725; void*
_temp727; struct Cyc_Absyn_Exp* _temp729; void* _temp731; struct Cyc_Absyn_Exp*
_temp733; void* _temp735; struct Cyc_Absyn_Exp* _temp737; void* _temp739; struct
Cyc_Absyn_Exp* _temp741; struct Cyc_Absyn_Exp* _temp743; struct Cyc_Absyn_Exp*
_temp745; struct Cyc_Absyn_Exp* _temp747; struct Cyc_Absyn_Exp* _temp749; struct
Cyc_Absyn_Exp* _temp751; struct Cyc_List_List* _temp753; struct Cyc_Absyn_Exp*
_temp755; struct Cyc_Absyn_VarargCallInfo* _temp757; struct Cyc_List_List*
_temp759; struct Cyc_Absyn_Exp* _temp761; struct Cyc_Absyn_Exp* _temp763; struct
Cyc_Absyn_Exp* _temp765; struct Cyc_List_List* _temp767; struct Cyc_Absyn_Exp*
_temp769; struct Cyc_Absyn_Exp* _temp771; void* _temp773; struct Cyc_Absyn_Exp*
_temp775; struct Cyc_Absyn_Exp* _temp777; struct Cyc_Absyn_Exp* _temp779; void*
_temp781; struct Cyc_Absyn_Exp* _temp783; struct Cyc_Absyn_Exp* _temp785; struct
_tagged_string* _temp787; struct Cyc_Absyn_Exp* _temp789; struct _tagged_string*
_temp791; struct Cyc_Absyn_Exp* _temp793; struct Cyc_Absyn_Exp* _temp795; struct
Cyc_Absyn_Exp* _temp797; struct Cyc_List_List* _temp799; struct Cyc_List_List*
_temp801; struct _tuple1* _temp803; struct Cyc_List_List* _temp805; struct Cyc_Absyn_Exp*
_temp807; struct Cyc_Absyn_Exp* _temp809; struct Cyc_Absyn_Vardecl* _temp811;
struct Cyc_Absyn_Structdecl* _temp813; struct Cyc_List_List* _temp815; struct
Cyc_Core_Opt* _temp817; struct _tuple0* _temp819; struct Cyc_List_List* _temp821;
void* _temp823; struct Cyc_Absyn_Tunionfield* _temp825; struct Cyc_Absyn_Tuniondecl*
_temp827; struct Cyc_List_List* _temp829; struct Cyc_Core_Opt* _temp831; struct
Cyc_Core_Opt* _temp833; struct Cyc_Absyn_Enumfield* _temp835; struct Cyc_Absyn_Enumdecl*
_temp837; struct _tuple0* _temp839; void* _temp841; struct Cyc_Absyn_Exp*
_temp843; struct Cyc_List_List* _temp845; struct Cyc_Core_Opt* _temp847; struct
Cyc_Absyn_Stmt* _temp849; struct Cyc_Absyn_Fndecl* _temp851; struct Cyc_Absyn_Exp*
_temp853; _LL581: if(*(( int*) _temp579) == Cyc_Absyn_Const_e){ _LL672: _temp671=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp579)->f1; if(( unsigned int)
_temp671 > 1u?*(( int*) _temp671) == Cyc_Absyn_Char_c: 0){ _LL676: _temp675=(
void*)(( struct Cyc_Absyn_Char_c_struct*) _temp671)->f1; goto _LL674; _LL674:
_temp673=(( struct Cyc_Absyn_Char_c_struct*) _temp671)->f2; goto _LL582;} else{
goto _LL583;}} else{ goto _LL583;} _LL583: if(*(( int*) _temp579) == Cyc_Absyn_Const_e){
_LL678: _temp677=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp579)->f1; if((
unsigned int) _temp677 > 1u?*(( int*) _temp677) == Cyc_Absyn_Short_c: 0){ _LL682:
_temp681=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp677)->f1; goto _LL680;
_LL680: _temp679=(( struct Cyc_Absyn_Short_c_struct*) _temp677)->f2; goto _LL584;}
else{ goto _LL585;}} else{ goto _LL585;} _LL585: if(*(( int*) _temp579) == Cyc_Absyn_Const_e){
_LL684: _temp683=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp579)->f1; if((
unsigned int) _temp683 > 1u?*(( int*) _temp683) == Cyc_Absyn_Int_c: 0){ _LL688:
_temp687=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp683)->f1; if( _temp687
==( void*) Cyc_Absyn_Signed){ goto _LL686;} else{ goto _LL587;} _LL686: _temp685=((
struct Cyc_Absyn_Int_c_struct*) _temp683)->f2; goto _LL586;} else{ goto _LL587;}}
else{ goto _LL587;} _LL587: if(*(( int*) _temp579) == Cyc_Absyn_Const_e){ _LL690:
_temp689=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp579)->f1; if((
unsigned int) _temp689 > 1u?*(( int*) _temp689) == Cyc_Absyn_Int_c: 0){ _LL694:
_temp693=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp689)->f1; if( _temp693
==( void*) Cyc_Absyn_Unsigned){ goto _LL692;} else{ goto _LL589;} _LL692:
_temp691=(( struct Cyc_Absyn_Int_c_struct*) _temp689)->f2; goto _LL588;} else{
goto _LL589;}} else{ goto _LL589;} _LL589: if(*(( int*) _temp579) == Cyc_Absyn_Const_e){
_LL696: _temp695=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp579)->f1; if((
unsigned int) _temp695 > 1u?*(( int*) _temp695) == Cyc_Absyn_LongLong_c: 0){
_LL700: _temp699=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp695)->f1;
goto _LL698; _LL698: _temp697=(( struct Cyc_Absyn_LongLong_c_struct*) _temp695)->f2;
goto _LL590;} else{ goto _LL591;}} else{ goto _LL591;} _LL591: if(*(( int*)
_temp579) == Cyc_Absyn_Const_e){ _LL702: _temp701=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp579)->f1; if(( unsigned int) _temp701 > 1u?*(( int*) _temp701) == Cyc_Absyn_Float_c:
0){ _LL704: _temp703=(( struct Cyc_Absyn_Float_c_struct*) _temp701)->f1; goto
_LL592;} else{ goto _LL593;}} else{ goto _LL593;} _LL593: if(*(( int*) _temp579)
== Cyc_Absyn_Const_e){ _LL706: _temp705=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp579)->f1; if( _temp705 ==( void*) Cyc_Absyn_Null_c){ goto _LL594;} else{
goto _LL595;}} else{ goto _LL595;} _LL595: if(*(( int*) _temp579) == Cyc_Absyn_Const_e){
_LL708: _temp707=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp579)->f1; if((
unsigned int) _temp707 > 1u?*(( int*) _temp707) == Cyc_Absyn_String_c: 0){
_LL710: _temp709=(( struct Cyc_Absyn_String_c_struct*) _temp707)->f1; goto
_LL596;} else{ goto _LL597;}} else{ goto _LL597;} _LL597: if(*(( int*) _temp579)
== Cyc_Absyn_UnknownId_e){ _LL712: _temp711=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp579)->f1; goto _LL598;} else{ goto _LL599;} _LL599: if(*(( int*) _temp579)
== Cyc_Absyn_Var_e){ _LL716: _temp715=(( struct Cyc_Absyn_Var_e_struct*)
_temp579)->f1; goto _LL714; _LL714: _temp713=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp579)->f2; goto _LL600;} else{ goto _LL601;} _LL601: if(*(( int*) _temp579)
== Cyc_Absyn_Primop_e){ _LL720: _temp719=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp579)->f1; goto _LL718; _LL718: _temp717=(( struct Cyc_Absyn_Primop_e_struct*)
_temp579)->f2; goto _LL602;} else{ goto _LL603;} _LL603: if(*(( int*) _temp579)
== Cyc_Absyn_AssignOp_e){ _LL726: _temp725=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp579)->f1; goto _LL724; _LL724: _temp723=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp579)->f2; goto _LL722; _LL722: _temp721=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp579)->f3; goto _LL604;} else{ goto _LL605;} _LL605: if(*(( int*) _temp579)
== Cyc_Absyn_Increment_e){ _LL730: _temp729=(( struct Cyc_Absyn_Increment_e_struct*)
_temp579)->f1; goto _LL728; _LL728: _temp727=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp579)->f2; if( _temp727 ==( void*) Cyc_Absyn_PreInc){ goto _LL606;} else{
goto _LL607;}} else{ goto _LL607;} _LL607: if(*(( int*) _temp579) == Cyc_Absyn_Increment_e){
_LL734: _temp733=(( struct Cyc_Absyn_Increment_e_struct*) _temp579)->f1; goto
_LL732; _LL732: _temp731=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp579)->f2; if( _temp731 ==( void*) Cyc_Absyn_PreDec){ goto _LL608;} else{
goto _LL609;}} else{ goto _LL609;} _LL609: if(*(( int*) _temp579) == Cyc_Absyn_Increment_e){
_LL738: _temp737=(( struct Cyc_Absyn_Increment_e_struct*) _temp579)->f1; goto
_LL736; _LL736: _temp735=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp579)->f2; if( _temp735 ==( void*) Cyc_Absyn_PostInc){ goto _LL610;} else{
goto _LL611;}} else{ goto _LL611;} _LL611: if(*(( int*) _temp579) == Cyc_Absyn_Increment_e){
_LL742: _temp741=(( struct Cyc_Absyn_Increment_e_struct*) _temp579)->f1; goto
_LL740; _LL740: _temp739=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp579)->f2; if( _temp739 ==( void*) Cyc_Absyn_PostDec){ goto _LL612;} else{
goto _LL613;}} else{ goto _LL613;} _LL613: if(*(( int*) _temp579) == Cyc_Absyn_Conditional_e){
_LL748: _temp747=(( struct Cyc_Absyn_Conditional_e_struct*) _temp579)->f1; goto
_LL746; _LL746: _temp745=(( struct Cyc_Absyn_Conditional_e_struct*) _temp579)->f2;
goto _LL744; _LL744: _temp743=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp579)->f3; goto _LL614;} else{ goto _LL615;} _LL615: if(*(( int*) _temp579)
== Cyc_Absyn_SeqExp_e){ _LL752: _temp751=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp579)->f1; goto _LL750; _LL750: _temp749=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp579)->f2; goto _LL616;} else{ goto _LL617;} _LL617: if(*(( int*) _temp579)
== Cyc_Absyn_UnknownCall_e){ _LL756: _temp755=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp579)->f1; goto _LL754; _LL754: _temp753=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp579)->f2; goto _LL618;} else{ goto _LL619;} _LL619: if(*(( int*) _temp579)
== Cyc_Absyn_FnCall_e){ _LL762: _temp761=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp579)->f1; goto _LL760; _LL760: _temp759=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp579)->f2; goto _LL758; _LL758: _temp757=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp579)->f3; goto _LL620;} else{ goto _LL621;} _LL621: if(*(( int*) _temp579)
== Cyc_Absyn_Throw_e){ _LL764: _temp763=(( struct Cyc_Absyn_Throw_e_struct*)
_temp579)->f1; goto _LL622;} else{ goto _LL623;} _LL623: if(*(( int*) _temp579)
== Cyc_Absyn_NoInstantiate_e){ _LL766: _temp765=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp579)->f1; goto _LL624;} else{ goto _LL625;} _LL625: if(*(( int*) _temp579)
== Cyc_Absyn_Instantiate_e){ _LL770: _temp769=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp579)->f1; goto _LL768; _LL768: _temp767=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp579)->f2; goto _LL626;} else{ goto _LL627;} _LL627: if(*(( int*) _temp579)
== Cyc_Absyn_Cast_e){ _LL774: _temp773=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp579)->f1; goto _LL772; _LL772: _temp771=(( struct Cyc_Absyn_Cast_e_struct*)
_temp579)->f2; goto _LL628;} else{ goto _LL629;} _LL629: if(*(( int*) _temp579)
== Cyc_Absyn_Address_e){ _LL776: _temp775=(( struct Cyc_Absyn_Address_e_struct*)
_temp579)->f1; goto _LL630;} else{ goto _LL631;} _LL631: if(*(( int*) _temp579)
== Cyc_Absyn_New_e){ _LL780: _temp779=(( struct Cyc_Absyn_New_e_struct*)
_temp579)->f1; goto _LL778; _LL778: _temp777=(( struct Cyc_Absyn_New_e_struct*)
_temp579)->f2; goto _LL632;} else{ goto _LL633;} _LL633: if(*(( int*) _temp579)
== Cyc_Absyn_Sizeoftyp_e){ _LL782: _temp781=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp579)->f1; goto _LL634;} else{ goto _LL635;} _LL635: if(*(( int*) _temp579)
== Cyc_Absyn_Sizeofexp_e){ _LL784: _temp783=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp579)->f1; goto _LL636;} else{ goto _LL637;} _LL637: if(*(( int*) _temp579)
== Cyc_Absyn_Deref_e){ _LL786: _temp785=(( struct Cyc_Absyn_Deref_e_struct*)
_temp579)->f1; goto _LL638;} else{ goto _LL639;} _LL639: if(*(( int*) _temp579)
== Cyc_Absyn_StructMember_e){ _LL790: _temp789=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp579)->f1; goto _LL788; _LL788: _temp787=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp579)->f2; goto _LL640;} else{ goto _LL641;} _LL641: if(*(( int*) _temp579)
== Cyc_Absyn_StructArrow_e){ _LL794: _temp793=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp579)->f1; goto _LL792; _LL792: _temp791=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp579)->f2; goto _LL642;} else{ goto _LL643;} _LL643: if(*(( int*) _temp579)
== Cyc_Absyn_Subscript_e){ _LL798: _temp797=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp579)->f1; goto _LL796; _LL796: _temp795=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp579)->f2; goto _LL644;} else{ goto _LL645;} _LL645: if(*(( int*) _temp579)
== Cyc_Absyn_Tuple_e){ _LL800: _temp799=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp579)->f1; goto _LL646;} else{ goto _LL647;} _LL647: if(*(( int*) _temp579)
== Cyc_Absyn_CompoundLit_e){ _LL804: _temp803=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp579)->f1; goto _LL802; _LL802: _temp801=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp579)->f2; goto _LL648;} else{ goto _LL649;} _LL649: if(*(( int*) _temp579)
== Cyc_Absyn_Array_e){ _LL806: _temp805=(( struct Cyc_Absyn_Array_e_struct*)
_temp579)->f1; goto _LL650;} else{ goto _LL651;} _LL651: if(*(( int*) _temp579)
== Cyc_Absyn_Comprehension_e){ _LL812: _temp811=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp579)->f1; goto _LL810; _LL810: _temp809=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp579)->f2; goto _LL808; _LL808: _temp807=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp579)->f3; goto _LL652;} else{ goto _LL653;} _LL653: if(*(( int*) _temp579)
== Cyc_Absyn_Struct_e){ _LL820: _temp819=(( struct Cyc_Absyn_Struct_e_struct*)
_temp579)->f1; goto _LL818; _LL818: _temp817=(( struct Cyc_Absyn_Struct_e_struct*)
_temp579)->f2; goto _LL816; _LL816: _temp815=(( struct Cyc_Absyn_Struct_e_struct*)
_temp579)->f3; goto _LL814; _LL814: _temp813=(( struct Cyc_Absyn_Struct_e_struct*)
_temp579)->f4; goto _LL654;} else{ goto _LL655;} _LL655: if(*(( int*) _temp579)
== Cyc_Absyn_AnonStruct_e){ _LL824: _temp823=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp579)->f1; goto _LL822; _LL822: _temp821=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp579)->f2; goto _LL656;} else{ goto _LL657;} _LL657: if(*(( int*) _temp579)
== Cyc_Absyn_Tunion_e){ _LL834: _temp833=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp579)->f1; goto _LL832; _LL832: _temp831=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp579)->f2; goto _LL830; _LL830: _temp829=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp579)->f3; goto _LL828; _LL828: _temp827=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp579)->f4; goto _LL826; _LL826: _temp825=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp579)->f5; goto _LL658;} else{ goto _LL659;} _LL659: if(*(( int*) _temp579)
== Cyc_Absyn_Enum_e){ _LL840: _temp839=(( struct Cyc_Absyn_Enum_e_struct*)
_temp579)->f1; goto _LL838; _LL838: _temp837=(( struct Cyc_Absyn_Enum_e_struct*)
_temp579)->f2; goto _LL836; _LL836: _temp835=(( struct Cyc_Absyn_Enum_e_struct*)
_temp579)->f3; goto _LL660;} else{ goto _LL661;} _LL661: if(*(( int*) _temp579)
== Cyc_Absyn_Malloc_e){ _LL844: _temp843=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp579)->f1; goto _LL842; _LL842: _temp841=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp579)->f2; goto _LL662;} else{ goto _LL663;} _LL663: if(*(( int*) _temp579)
== Cyc_Absyn_UnresolvedMem_e){ _LL848: _temp847=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp579)->f1; goto _LL846; _LL846: _temp845=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp579)->f2; goto _LL664;} else{ goto _LL665;} _LL665: if(*(( int*) _temp579)
== Cyc_Absyn_StmtExp_e){ _LL850: _temp849=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp579)->f1; goto _LL666;} else{ goto _LL667;} _LL667: if(*(( int*) _temp579)
== Cyc_Absyn_Codegen_e){ _LL852: _temp851=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp579)->f1; goto _LL668;} else{ goto _LL669;} _LL669: if(*(( int*) _temp579)
== Cyc_Absyn_Fill_e){ _LL854: _temp853=(( struct Cyc_Absyn_Fill_e_struct*)
_temp579)->f1; goto _LL670;} else{ goto _LL580;} _LL582: Cyc_Absyndump_dump_char((
int)'\''); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_char_escape( _temp673)); Cyc_Absyndump_dump_char((
int)'\''); goto _LL580; _LL584: Cyc_Absyndump_dump( Cyc_Core_string_of_int(( int)
_temp679)); goto _LL580; _LL586: Cyc_Absyndump_dump( Cyc_Core_string_of_int(
_temp685)); goto _LL580; _LL588: Cyc_Absyndump_dump( Cyc_Core_string_of_int(
_temp691)); Cyc_Absyndump_dump_nospace(({ unsigned char* _temp855="u"; struct
_tagged_string _temp856; _temp856.curr= _temp855; _temp856.base= _temp855;
_temp856.last_plus_one= _temp855 + 2; _temp856;})); goto _LL580; _LL590: Cyc_Absyndump_dump(({
unsigned char* _temp857="<<FIX LONG LONG CONSTANT>>"; struct _tagged_string
_temp858; _temp858.curr= _temp857; _temp858.base= _temp857; _temp858.last_plus_one=
_temp857 + 27; _temp858;})); goto _LL580; _LL592: Cyc_Absyndump_dump( _temp703);
goto _LL580; _LL594: Cyc_Absyndump_dump(({ unsigned char* _temp859="null";
struct _tagged_string _temp860; _temp860.curr= _temp859; _temp860.base= _temp859;
_temp860.last_plus_one= _temp859 + 5; _temp860;})); goto _LL580; _LL596: Cyc_Absyndump_dump_char((
int)'"'); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_string_escape( _temp709)); Cyc_Absyndump_dump_char((
int)'"'); goto _LL580; _LL598: _temp715= _temp711; goto _LL600; _LL600: Cyc_Absyndump_dumpqvar(
_temp715); goto _LL580; _LL602: { struct _tagged_string pstr= Cyc_Absynpp_prim2str(
_temp719); if( ! Cyc_Absyn_is_format_prim( _temp719)){ switch((( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp717)){ case 1: _LL861: if( _temp719 ==(
void*) Cyc_Absyn_Size){ Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp717))->hd); Cyc_Absyndump_dump(({
unsigned char* _temp863=".size"; struct _tagged_string _temp864; _temp864.curr=
_temp863; _temp864.base= _temp863; _temp864.last_plus_one= _temp863 + 6;
_temp864;}));} else{ Cyc_Absyndump_dump( pstr); Cyc_Absyndump_dumpexp_prec(
myprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp717))->hd);}
break; case 2: _LL862: Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp717))->hd); Cyc_Absyndump_dump( pstr);
Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp717))->tl))->hd); break;
default: _LL865:( void) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp867=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp867[ 0]=({ struct Cyc_Core_Failure_struct _temp868; _temp868.tag= Cyc_Core_Failure;
_temp868.f1=({ unsigned char* _temp869="Absyndump -- Bad number of arguments to primop";
struct _tagged_string _temp870; _temp870.curr= _temp869; _temp870.base= _temp869;
_temp870.last_plus_one= _temp869 + 47; _temp870;}); _temp868;}); _temp867;}));}}
else{ Cyc_Absyndump_dump( pstr); Cyc_Absyndump_dump_nospace(({ unsigned char*
_temp871="("; struct _tagged_string _temp872; _temp872.curr= _temp871; _temp872.base=
_temp871; _temp872.last_plus_one= _temp871 + 2; _temp872;})); Cyc_Absyndump_dumpexps_prec(
20, _temp717); Cyc_Absyndump_dump_nospace(({ unsigned char* _temp873=")"; struct
_tagged_string _temp874; _temp874.curr= _temp873; _temp874.base= _temp873;
_temp874.last_plus_one= _temp873 + 2; _temp874;}));} goto _LL580;} _LL604: Cyc_Absyndump_dumpexp_prec(
myprec, _temp725); if( _temp723 != 0){ Cyc_Absyndump_dump( Cyc_Absynpp_prim2str((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp723))->v));} Cyc_Absyndump_dump_nospace(({
unsigned char* _temp875="="; struct _tagged_string _temp876; _temp876.curr=
_temp875; _temp876.base= _temp875; _temp876.last_plus_one= _temp875 + 2;
_temp876;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp721); goto _LL580; _LL606:
Cyc_Absyndump_dump(({ unsigned char* _temp877="++"; struct _tagged_string
_temp878; _temp878.curr= _temp877; _temp878.base= _temp877; _temp878.last_plus_one=
_temp877 + 3; _temp878;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp729); goto
_LL580; _LL608: Cyc_Absyndump_dump(({ unsigned char* _temp879="--"; struct
_tagged_string _temp880; _temp880.curr= _temp879; _temp880.base= _temp879;
_temp880.last_plus_one= _temp879 + 3; _temp880;})); Cyc_Absyndump_dumpexp_prec(
myprec, _temp733); goto _LL580; _LL610: Cyc_Absyndump_dumpexp_prec( myprec,
_temp737); Cyc_Absyndump_dump(({ unsigned char* _temp881="++"; struct
_tagged_string _temp882; _temp882.curr= _temp881; _temp882.base= _temp881;
_temp882.last_plus_one= _temp881 + 3; _temp882;})); goto _LL580; _LL612: Cyc_Absyndump_dumpexp_prec(
myprec, _temp741); Cyc_Absyndump_dump(({ unsigned char* _temp883="--"; struct
_tagged_string _temp884; _temp884.curr= _temp883; _temp884.base= _temp883;
_temp884.last_plus_one= _temp883 + 3; _temp884;})); goto _LL580; _LL614: Cyc_Absyndump_dumpexp_prec(
myprec, _temp747); Cyc_Absyndump_dump_char(( int)'?'); Cyc_Absyndump_dumpexp_prec(
0, _temp745); Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp743); goto _LL580; _LL616: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumpexp_prec(
myprec, _temp751); Cyc_Absyndump_dump_char(( int)','); Cyc_Absyndump_dumpexp_prec(
myprec, _temp749); Cyc_Absyndump_dump_char(( int)')'); goto _LL580; _LL618:
_temp761= _temp755; _temp759= _temp753; goto _LL620; _LL620: Cyc_Absyndump_dumpexp_prec(
myprec, _temp761); Cyc_Absyndump_dump_nospace(({ unsigned char* _temp885="(";
struct _tagged_string _temp886; _temp886.curr= _temp885; _temp886.base= _temp885;
_temp886.last_plus_one= _temp885 + 2; _temp886;})); Cyc_Absyndump_dumpexps_prec(
20, _temp759); Cyc_Absyndump_dump_nospace(({ unsigned char* _temp887=")"; struct
_tagged_string _temp888; _temp888.curr= _temp887; _temp888.base= _temp887;
_temp888.last_plus_one= _temp887 + 2; _temp888;})); goto _LL580; _LL622: Cyc_Absyndump_dump(({
unsigned char* _temp889="throw"; struct _tagged_string _temp890; _temp890.curr=
_temp889; _temp890.base= _temp889; _temp890.last_plus_one= _temp889 + 6;
_temp890;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp763); goto _LL580; _LL624:
_temp769= _temp765; goto _LL626; _LL626: Cyc_Absyndump_dumpexp_prec( inprec,
_temp769); goto _LL580; _LL628: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumptyp(
_temp773); Cyc_Absyndump_dump_char(( int)')'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp771); goto _LL580; _LL630: Cyc_Absyndump_dump_char(( int)'&'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp775); goto _LL580; _LL632: Cyc_Absyndump_dump(({ unsigned char*
_temp891="new "; struct _tagged_string _temp892; _temp892.curr= _temp891;
_temp892.base= _temp891; _temp892.last_plus_one= _temp891 + 5; _temp892;})); Cyc_Absyndump_dumpexp_prec(
myprec, _temp777); goto _LL580; _LL634: Cyc_Absyndump_dump(({ unsigned char*
_temp893="sizeof("; struct _tagged_string _temp894; _temp894.curr= _temp893;
_temp894.base= _temp893; _temp894.last_plus_one= _temp893 + 8; _temp894;})); Cyc_Absyndump_dumptyp(
_temp781); Cyc_Absyndump_dump_char(( int)')'); goto _LL580; _LL636: Cyc_Absyndump_dump(({
unsigned char* _temp895="sizeof("; struct _tagged_string _temp896; _temp896.curr=
_temp895; _temp896.base= _temp895; _temp896.last_plus_one= _temp895 + 8;
_temp896;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp783); Cyc_Absyndump_dump_char((
int)')'); goto _LL580; _LL638: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp785); goto _LL580; _LL640: Cyc_Absyndump_dumpexp_prec( myprec,
_temp789); Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp787); goto _LL580; _LL642: Cyc_Absyndump_dumpexp_prec( myprec, _temp793);
Cyc_Absyndump_dump_nospace(({ unsigned char* _temp897="->"; struct
_tagged_string _temp898; _temp898.curr= _temp897; _temp898.base= _temp897;
_temp898.last_plus_one= _temp897 + 3; _temp898;})); Cyc_Absyndump_dump_nospace(*
_temp791); goto _LL580; _LL644: Cyc_Absyndump_dumpexp_prec( myprec, _temp797);
Cyc_Absyndump_dump_char(( int)'['); Cyc_Absyndump_dumpexp( _temp795); Cyc_Absyndump_dump_char((
int)']'); goto _LL580; _LL646: Cyc_Absyndump_dump(({ unsigned char* _temp899="$(";
struct _tagged_string _temp900; _temp900.curr= _temp899; _temp900.base= _temp899;
_temp900.last_plus_one= _temp899 + 3; _temp900;})); Cyc_Absyndump_dumpexps_prec(
20, _temp799); Cyc_Absyndump_dump_char(( int)')'); goto _LL580; _LL648: Cyc_Absyndump_dump_char((
int)'('); Cyc_Absyndump_dumptyp((* _temp803).f3); Cyc_Absyndump_dump_char(( int)')');((
void(*)( void(* f)( struct _tuple7*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp801,({ unsigned char* _temp901="{"; struct
_tagged_string _temp902; _temp902.curr= _temp901; _temp902.base= _temp901;
_temp902.last_plus_one= _temp901 + 2; _temp902;}),({ unsigned char* _temp903="}";
struct _tagged_string _temp904; _temp904.curr= _temp903; _temp904.base= _temp903;
_temp904.last_plus_one= _temp903 + 2; _temp904;}),({ unsigned char* _temp905=",";
struct _tagged_string _temp906; _temp906.curr= _temp905; _temp906.base= _temp905;
_temp906.last_plus_one= _temp905 + 2; _temp906;})); goto _LL580; _LL650:(( void(*)(
void(* f)( struct _tuple7*), struct Cyc_List_List* l, struct _tagged_string
start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp805,({ unsigned char* _temp907="{"; struct
_tagged_string _temp908; _temp908.curr= _temp907; _temp908.base= _temp907;
_temp908.last_plus_one= _temp907 + 2; _temp908;}),({ unsigned char* _temp909="}";
struct _tagged_string _temp910; _temp910.curr= _temp909; _temp910.base= _temp909;
_temp910.last_plus_one= _temp909 + 2; _temp910;}),({ unsigned char* _temp911=",";
struct _tagged_string _temp912; _temp912.curr= _temp911; _temp912.base= _temp911;
_temp912.last_plus_one= _temp911 + 2; _temp912;})); goto _LL580; _LL652: Cyc_Absyndump_dump(({
unsigned char* _temp913="new {for"; struct _tagged_string _temp914; _temp914.curr=
_temp913; _temp914.base= _temp913; _temp914.last_plus_one= _temp913 + 9;
_temp914;})); Cyc_Absyndump_dump_str((* _temp811->name).f2); Cyc_Absyndump_dump_char((
int)'<'); Cyc_Absyndump_dumpexp( _temp809); Cyc_Absyndump_dump_char(( int)':');
Cyc_Absyndump_dumpexp( _temp807); Cyc_Absyndump_dump_char(( int)'}'); goto
_LL580; _LL654: Cyc_Absyndump_dumpqvar( _temp819);(( void(*)( void(* f)( struct
_tuple7*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp815,({ unsigned char* _temp915="{"; struct _tagged_string _temp916;
_temp916.curr= _temp915; _temp916.base= _temp915; _temp916.last_plus_one=
_temp915 + 2; _temp916;}),({ unsigned char* _temp917="}"; struct _tagged_string
_temp918; _temp918.curr= _temp917; _temp918.base= _temp917; _temp918.last_plus_one=
_temp917 + 2; _temp918;}),({ unsigned char* _temp919=","; struct _tagged_string
_temp920; _temp920.curr= _temp919; _temp920.base= _temp919; _temp920.last_plus_one=
_temp919 + 2; _temp920;})); goto _LL580; _LL656:(( void(*)( void(* f)( struct
_tuple7*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp821,({ unsigned char* _temp921="{"; struct _tagged_string _temp922;
_temp922.curr= _temp921; _temp922.base= _temp921; _temp922.last_plus_one=
_temp921 + 2; _temp922;}),({ unsigned char* _temp923="}"; struct _tagged_string
_temp924; _temp924.curr= _temp923; _temp924.base= _temp923; _temp924.last_plus_one=
_temp923 + 2; _temp924;}),({ unsigned char* _temp925=","; struct _tagged_string
_temp926; _temp926.curr= _temp925; _temp926.base= _temp925; _temp926.last_plus_one=
_temp925 + 2; _temp926;})); goto _LL580; _LL658: Cyc_Absyndump_dumpqvar(
_temp825->name); if( _temp829 != 0){(( void(*)( void(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpexp,
_temp829,({ unsigned char* _temp927="("; struct _tagged_string _temp928;
_temp928.curr= _temp927; _temp928.base= _temp927; _temp928.last_plus_one=
_temp927 + 2; _temp928;}),({ unsigned char* _temp929=")"; struct _tagged_string
_temp930; _temp930.curr= _temp929; _temp930.base= _temp929; _temp930.last_plus_one=
_temp929 + 2; _temp930;}),({ unsigned char* _temp931=","; struct _tagged_string
_temp932; _temp932.curr= _temp931; _temp932.base= _temp931; _temp932.last_plus_one=
_temp931 + 2; _temp932;}));} goto _LL580; _LL660: Cyc_Absyndump_dumpqvar(
_temp839); goto _LL580; _LL662: if( _temp843 != 0){ Cyc_Absyndump_dump(({
unsigned char* _temp933="rmalloc("; struct _tagged_string _temp934; _temp934.curr=
_temp933; _temp934.base= _temp933; _temp934.last_plus_one= _temp933 + 9;
_temp934;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null(
_temp843)); Cyc_Absyndump_dump(({ unsigned char* _temp935=","; struct
_tagged_string _temp936; _temp936.curr= _temp935; _temp936.base= _temp935;
_temp936.last_plus_one= _temp935 + 2; _temp936;}));} else{ Cyc_Absyndump_dump(({
unsigned char* _temp937="malloc("; struct _tagged_string _temp938; _temp938.curr=
_temp937; _temp938.base= _temp937; _temp938.last_plus_one= _temp937 + 8;
_temp938;}));} Cyc_Absyndump_dump(({ unsigned char* _temp939="sizeof("; struct
_tagged_string _temp940; _temp940.curr= _temp939; _temp940.base= _temp939;
_temp940.last_plus_one= _temp939 + 8; _temp940;})); Cyc_Absyndump_dumptyp(
_temp841); Cyc_Absyndump_dump(({ unsigned char* _temp941="))"; struct
_tagged_string _temp942; _temp942.curr= _temp941; _temp942.base= _temp941;
_temp942.last_plus_one= _temp941 + 3; _temp942;})); goto _LL580; _LL664:(( void(*)(
void(* f)( struct _tuple7*), struct Cyc_List_List* l, struct _tagged_string
start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp845,({ unsigned char* _temp943="{"; struct
_tagged_string _temp944; _temp944.curr= _temp943; _temp944.base= _temp943;
_temp944.last_plus_one= _temp943 + 2; _temp944;}),({ unsigned char* _temp945="}";
struct _tagged_string _temp946; _temp946.curr= _temp945; _temp946.base= _temp945;
_temp946.last_plus_one= _temp945 + 2; _temp946;}),({ unsigned char* _temp947=",";
struct _tagged_string _temp948; _temp948.curr= _temp947; _temp948.base= _temp947;
_temp948.last_plus_one= _temp947 + 2; _temp948;})); goto _LL580; _LL666: Cyc_Absyndump_dump_nospace(({
unsigned char* _temp949="({"; struct _tagged_string _temp950; _temp950.curr=
_temp949; _temp950.base= _temp949; _temp950.last_plus_one= _temp949 + 3;
_temp950;})); Cyc_Absyndump_dumpstmt( _temp849); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp951="})"; struct _tagged_string _temp952; _temp952.curr=
_temp951; _temp952.base= _temp951; _temp952.last_plus_one= _temp951 + 3;
_temp952;})); goto _LL580; _LL668: Cyc_Absyndump_dump(({ unsigned char* _temp953="codegen(";
struct _tagged_string _temp954; _temp954.curr= _temp953; _temp954.base= _temp953;
_temp954.last_plus_one= _temp953 + 9; _temp954;})); Cyc_Absyndump_dumpdecl(({
struct Cyc_Absyn_Decl* _temp955=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof(
struct Cyc_Absyn_Decl)); _temp955->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp956=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp956[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp957; _temp957.tag= Cyc_Absyn_Fn_d;
_temp957.f1= _temp851; _temp957;}); _temp956;})); _temp955->loc= e->loc;
_temp955;})); Cyc_Absyndump_dump(({ unsigned char* _temp958=")"; struct
_tagged_string _temp959; _temp959.curr= _temp958; _temp959.base= _temp958;
_temp959.last_plus_one= _temp958 + 2; _temp959;})); goto _LL580; _LL670: Cyc_Absyndump_dump(({
unsigned char* _temp960="fill("; struct _tagged_string _temp961; _temp961.curr=
_temp960; _temp961.base= _temp960; _temp961.last_plus_one= _temp960 + 6;
_temp961;})); Cyc_Absyndump_dumpexp( _temp853); Cyc_Absyndump_dump(({
unsigned char* _temp962=")"; struct _tagged_string _temp963; _temp963.curr=
_temp962; _temp963.base= _temp962; _temp963.last_plus_one= _temp962 + 2;
_temp963;})); goto _LL580; _LL580:;} if( inprec >= myprec){ Cyc_Absyndump_dump_char((
int)')');}} void Cyc_Absyndump_dumpexp( struct Cyc_Absyn_Exp* e){ Cyc_Absyndump_dumpexp_prec(
0, e);} void Cyc_Absyndump_dumpswitchclauses( struct Cyc_List_List* scs){ for( 0;
scs != 0; scs=(( struct Cyc_List_List*) _check_null( scs))->tl){ struct Cyc_Absyn_Switch_clause*
_temp964=( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
scs))->hd; goto _LL965; _LL965: if( _temp964->where_clause == 0?( void*)(
_temp964->pattern)->r ==( void*) Cyc_Absyn_Wild_p: 0){ Cyc_Absyndump_dump(({
unsigned char* _temp966="default:"; struct _tagged_string _temp967; _temp967.curr=
_temp966; _temp967.base= _temp966; _temp967.last_plus_one= _temp966 + 9;
_temp967;}));} else{ Cyc_Absyndump_dump(({ unsigned char* _temp968="case";
struct _tagged_string _temp969; _temp969.curr= _temp968; _temp969.base= _temp968;
_temp969.last_plus_one= _temp968 + 5; _temp969;})); Cyc_Absyndump_dumppat(
_temp964->pattern); if( _temp964->where_clause != 0){ Cyc_Absyndump_dump(({
unsigned char* _temp970="&&"; struct _tagged_string _temp971; _temp971.curr=
_temp970; _temp971.base= _temp970; _temp971.last_plus_one= _temp970 + 3;
_temp971;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null(
_temp964->where_clause));} Cyc_Absyndump_dump_nospace(({ unsigned char* _temp972=":";
struct _tagged_string _temp973; _temp973.curr= _temp972; _temp973.base= _temp972;
_temp973.last_plus_one= _temp972 + 2; _temp973;}));} Cyc_Absyndump_dumpstmt(
_temp964->body);}} void Cyc_Absyndump_dumpstmt( struct Cyc_Absyn_Stmt* s){ void*
_temp974=( void*) s->r; struct Cyc_Absyn_Exp* _temp1020; struct Cyc_Absyn_Stmt*
_temp1022; struct Cyc_Absyn_Stmt* _temp1024; struct Cyc_Absyn_Exp* _temp1026;
struct Cyc_Absyn_Exp* _temp1028; struct Cyc_Absyn_Stmt* _temp1030; struct Cyc_Absyn_Stmt*
_temp1032; struct Cyc_Absyn_Exp* _temp1034; struct Cyc_Absyn_Stmt* _temp1036;
struct _tuple2 _temp1038; struct Cyc_Absyn_Stmt* _temp1040; struct Cyc_Absyn_Exp*
_temp1042; struct Cyc_Absyn_Stmt* _temp1044; struct Cyc_Absyn_Stmt* _temp1046;
struct Cyc_Absyn_Stmt* _temp1048; struct _tagged_string* _temp1050; struct Cyc_Absyn_Stmt*
_temp1052; struct _tuple2 _temp1054; struct Cyc_Absyn_Stmt* _temp1056; struct
Cyc_Absyn_Exp* _temp1058; struct _tuple2 _temp1060; struct Cyc_Absyn_Stmt*
_temp1062; struct Cyc_Absyn_Exp* _temp1064; struct Cyc_Absyn_Exp* _temp1066;
struct Cyc_List_List* _temp1068; struct Cyc_Absyn_Exp* _temp1070; struct Cyc_Absyn_Stmt*
_temp1072; struct Cyc_Absyn_Decl* _temp1074; struct Cyc_Absyn_Stmt* _temp1076;
struct _tagged_string* _temp1078; struct _tuple2 _temp1080; struct Cyc_Absyn_Stmt*
_temp1082; struct Cyc_Absyn_Exp* _temp1084; struct Cyc_Absyn_Stmt* _temp1086;
struct Cyc_List_List* _temp1088; struct Cyc_Absyn_Exp* _temp1090; struct Cyc_Absyn_Switch_clause**
_temp1092; struct Cyc_List_List* _temp1094; struct Cyc_Absyn_Switch_clause**
_temp1096; struct Cyc_List_List* _temp1098; struct Cyc_List_List* _temp1100;
struct Cyc_Absyn_Stmt* _temp1102; struct Cyc_Absyn_Stmt* _temp1104; struct Cyc_Absyn_Vardecl*
_temp1106; struct Cyc_Absyn_Tvar* _temp1108; struct Cyc_Absyn_Stmt* _temp1110;
struct Cyc_Absyn_Stmt* _temp1112; _LL976: if( _temp974 ==( void*) Cyc_Absyn_Skip_s){
goto _LL977;} else{ goto _LL978;} _LL978: if(( unsigned int) _temp974 > 1u?*((
int*) _temp974) == Cyc_Absyn_Exp_s: 0){ _LL1021: _temp1020=(( struct Cyc_Absyn_Exp_s_struct*)
_temp974)->f1; goto _LL979;} else{ goto _LL980;} _LL980: if(( unsigned int)
_temp974 > 1u?*(( int*) _temp974) == Cyc_Absyn_Seq_s: 0){ _LL1025: _temp1024=((
struct Cyc_Absyn_Seq_s_struct*) _temp974)->f1; goto _LL1023; _LL1023: _temp1022=((
struct Cyc_Absyn_Seq_s_struct*) _temp974)->f2; goto _LL981;} else{ goto _LL982;}
_LL982: if(( unsigned int) _temp974 > 1u?*(( int*) _temp974) == Cyc_Absyn_Return_s:
0){ _LL1027: _temp1026=(( struct Cyc_Absyn_Return_s_struct*) _temp974)->f1; if(
_temp1026 == 0){ goto _LL983;} else{ goto _LL984;}} else{ goto _LL984;} _LL984:
if(( unsigned int) _temp974 > 1u?*(( int*) _temp974) == Cyc_Absyn_Return_s: 0){
_LL1029: _temp1028=(( struct Cyc_Absyn_Return_s_struct*) _temp974)->f1; goto
_LL985;} else{ goto _LL986;} _LL986: if(( unsigned int) _temp974 > 1u?*(( int*)
_temp974) == Cyc_Absyn_IfThenElse_s: 0){ _LL1035: _temp1034=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp974)->f1; goto _LL1033; _LL1033: _temp1032=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp974)->f2; goto _LL1031; _LL1031: _temp1030=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp974)->f3; goto _LL987;} else{ goto _LL988;} _LL988: if(( unsigned int)
_temp974 > 1u?*(( int*) _temp974) == Cyc_Absyn_While_s: 0){ _LL1039: _temp1038=((
struct Cyc_Absyn_While_s_struct*) _temp974)->f1; _LL1043: _temp1042= _temp1038.f1;
goto _LL1041; _LL1041: _temp1040= _temp1038.f2; goto _LL1037; _LL1037: _temp1036=((
struct Cyc_Absyn_While_s_struct*) _temp974)->f2; goto _LL989;} else{ goto _LL990;}
_LL990: if(( unsigned int) _temp974 > 1u?*(( int*) _temp974) == Cyc_Absyn_Break_s:
0){ _LL1045: _temp1044=(( struct Cyc_Absyn_Break_s_struct*) _temp974)->f1; goto
_LL991;} else{ goto _LL992;} _LL992: if(( unsigned int) _temp974 > 1u?*(( int*)
_temp974) == Cyc_Absyn_Continue_s: 0){ _LL1047: _temp1046=(( struct Cyc_Absyn_Continue_s_struct*)
_temp974)->f1; goto _LL993;} else{ goto _LL994;} _LL994: if(( unsigned int)
_temp974 > 1u?*(( int*) _temp974) == Cyc_Absyn_Goto_s: 0){ _LL1051: _temp1050=((
struct Cyc_Absyn_Goto_s_struct*) _temp974)->f1; goto _LL1049; _LL1049: _temp1048=((
struct Cyc_Absyn_Goto_s_struct*) _temp974)->f2; goto _LL995;} else{ goto _LL996;}
_LL996: if(( unsigned int) _temp974 > 1u?*(( int*) _temp974) == Cyc_Absyn_For_s:
0){ _LL1067: _temp1066=(( struct Cyc_Absyn_For_s_struct*) _temp974)->f1; goto
_LL1061; _LL1061: _temp1060=(( struct Cyc_Absyn_For_s_struct*) _temp974)->f2;
_LL1065: _temp1064= _temp1060.f1; goto _LL1063; _LL1063: _temp1062= _temp1060.f2;
goto _LL1055; _LL1055: _temp1054=(( struct Cyc_Absyn_For_s_struct*) _temp974)->f3;
_LL1059: _temp1058= _temp1054.f1; goto _LL1057; _LL1057: _temp1056= _temp1054.f2;
goto _LL1053; _LL1053: _temp1052=(( struct Cyc_Absyn_For_s_struct*) _temp974)->f4;
goto _LL997;} else{ goto _LL998;} _LL998: if(( unsigned int) _temp974 > 1u?*((
int*) _temp974) == Cyc_Absyn_Switch_s: 0){ _LL1071: _temp1070=(( struct Cyc_Absyn_Switch_s_struct*)
_temp974)->f1; goto _LL1069; _LL1069: _temp1068=(( struct Cyc_Absyn_Switch_s_struct*)
_temp974)->f2; goto _LL999;} else{ goto _LL1000;} _LL1000: if(( unsigned int)
_temp974 > 1u?*(( int*) _temp974) == Cyc_Absyn_Decl_s: 0){ _LL1075: _temp1074=((
struct Cyc_Absyn_Decl_s_struct*) _temp974)->f1; goto _LL1073; _LL1073: _temp1072=((
struct Cyc_Absyn_Decl_s_struct*) _temp974)->f2; goto _LL1001;} else{ goto
_LL1002;} _LL1002: if(( unsigned int) _temp974 > 1u?*(( int*) _temp974) == Cyc_Absyn_Label_s:
0){ _LL1079: _temp1078=(( struct Cyc_Absyn_Label_s_struct*) _temp974)->f1; goto
_LL1077; _LL1077: _temp1076=(( struct Cyc_Absyn_Label_s_struct*) _temp974)->f2;
goto _LL1003;} else{ goto _LL1004;} _LL1004: if(( unsigned int) _temp974 > 1u?*((
int*) _temp974) == Cyc_Absyn_Do_s: 0){ _LL1087: _temp1086=(( struct Cyc_Absyn_Do_s_struct*)
_temp974)->f1; goto _LL1081; _LL1081: _temp1080=(( struct Cyc_Absyn_Do_s_struct*)
_temp974)->f2; _LL1085: _temp1084= _temp1080.f1; goto _LL1083; _LL1083:
_temp1082= _temp1080.f2; goto _LL1005;} else{ goto _LL1006;} _LL1006: if((
unsigned int) _temp974 > 1u?*(( int*) _temp974) == Cyc_Absyn_SwitchC_s: 0){
_LL1091: _temp1090=(( struct Cyc_Absyn_SwitchC_s_struct*) _temp974)->f1; goto
_LL1089; _LL1089: _temp1088=(( struct Cyc_Absyn_SwitchC_s_struct*) _temp974)->f2;
goto _LL1007;} else{ goto _LL1008;} _LL1008: if(( unsigned int) _temp974 > 1u?*((
int*) _temp974) == Cyc_Absyn_Fallthru_s: 0){ _LL1095: _temp1094=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp974)->f1; if( _temp1094 == 0){ goto _LL1093;} else{ goto _LL1010;} _LL1093:
_temp1092=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp974)->f2; goto _LL1009;}
else{ goto _LL1010;} _LL1010: if(( unsigned int) _temp974 > 1u?*(( int*)
_temp974) == Cyc_Absyn_Fallthru_s: 0){ _LL1099: _temp1098=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp974)->f1; goto _LL1097; _LL1097: _temp1096=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp974)->f2; goto _LL1011;} else{ goto _LL1012;} _LL1012: if(( unsigned int)
_temp974 > 1u?*(( int*) _temp974) == Cyc_Absyn_TryCatch_s: 0){ _LL1103:
_temp1102=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp974)->f1; goto _LL1101;
_LL1101: _temp1100=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp974)->f2; goto
_LL1013;} else{ goto _LL1014;} _LL1014: if(( unsigned int) _temp974 > 1u?*(( int*)
_temp974) == Cyc_Absyn_Region_s: 0){ _LL1109: _temp1108=(( struct Cyc_Absyn_Region_s_struct*)
_temp974)->f1; goto _LL1107; _LL1107: _temp1106=(( struct Cyc_Absyn_Region_s_struct*)
_temp974)->f2; goto _LL1105; _LL1105: _temp1104=(( struct Cyc_Absyn_Region_s_struct*)
_temp974)->f3; goto _LL1015;} else{ goto _LL1016;} _LL1016: if(( unsigned int)
_temp974 > 1u?*(( int*) _temp974) == Cyc_Absyn_Cut_s: 0){ _LL1111: _temp1110=((
struct Cyc_Absyn_Cut_s_struct*) _temp974)->f1; goto _LL1017;} else{ goto _LL1018;}
_LL1018: if(( unsigned int) _temp974 > 1u?*(( int*) _temp974) == Cyc_Absyn_Splice_s:
0){ _LL1113: _temp1112=(( struct Cyc_Absyn_Splice_s_struct*) _temp974)->f1; goto
_LL1019;} else{ goto _LL975;} _LL977: Cyc_Absyndump_dump_semi(); goto _LL975;
_LL979: Cyc_Absyndump_dumpexp( _temp1020); Cyc_Absyndump_dump_semi(); goto
_LL975; _LL981: if( Cyc_Absynpp_is_declaration( _temp1024)){ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpstmt( _temp1024); Cyc_Absyndump_dump_char(( int)'}');}
else{ Cyc_Absyndump_dumpstmt( _temp1024);} if( Cyc_Absynpp_is_declaration(
_temp1022)){ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt(
_temp1022); Cyc_Absyndump_dump_char(( int)'}');} else{ Cyc_Absyndump_dumpstmt(
_temp1022);} goto _LL975; _LL983: Cyc_Absyndump_dump(({ unsigned char* _temp1114="return;";
struct _tagged_string _temp1115; _temp1115.curr= _temp1114; _temp1115.base=
_temp1114; _temp1115.last_plus_one= _temp1114 + 8; _temp1115;})); goto _LL975;
_LL985: Cyc_Absyndump_dump(({ unsigned char* _temp1116="return"; struct
_tagged_string _temp1117; _temp1117.curr= _temp1116; _temp1117.base= _temp1116;
_temp1117.last_plus_one= _temp1116 + 7; _temp1117;})); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*) _check_null( _temp1028)); Cyc_Absyndump_dump_semi(); goto
_LL975; _LL987: Cyc_Absyndump_dump(({ unsigned char* _temp1118="if("; struct
_tagged_string _temp1119; _temp1119.curr= _temp1118; _temp1119.base= _temp1118;
_temp1119.last_plus_one= _temp1118 + 4; _temp1119;})); Cyc_Absyndump_dumpexp(
_temp1034); Cyc_Absyndump_dump_nospace(({ unsigned char* _temp1120="){"; struct
_tagged_string _temp1121; _temp1121.curr= _temp1120; _temp1121.base= _temp1120;
_temp1121.last_plus_one= _temp1120 + 3; _temp1121;})); Cyc_Absyndump_dumpstmt(
_temp1032); Cyc_Absyndump_dump_char(( int)'}');{ void* _temp1122=( void*)
_temp1030->r; _LL1124: if( _temp1122 ==( void*) Cyc_Absyn_Skip_s){ goto _LL1125;}
else{ goto _LL1126;} _LL1126: goto _LL1127; _LL1125: goto _LL1123; _LL1127: Cyc_Absyndump_dump(({
unsigned char* _temp1128="else{"; struct _tagged_string _temp1129; _temp1129.curr=
_temp1128; _temp1129.base= _temp1128; _temp1129.last_plus_one= _temp1128 + 6;
_temp1129;})); Cyc_Absyndump_dumpstmt( _temp1030); Cyc_Absyndump_dump_char(( int)'}');
goto _LL1123; _LL1123:;} goto _LL975; _LL989: Cyc_Absyndump_dump(({
unsigned char* _temp1130="while("; struct _tagged_string _temp1131; _temp1131.curr=
_temp1130; _temp1131.base= _temp1130; _temp1131.last_plus_one= _temp1130 + 7;
_temp1131;})); Cyc_Absyndump_dumpexp( _temp1042); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp1132=") {"; struct _tagged_string _temp1133; _temp1133.curr=
_temp1132; _temp1133.base= _temp1132; _temp1133.last_plus_one= _temp1132 + 4;
_temp1133;})); Cyc_Absyndump_dumpstmt( _temp1036); Cyc_Absyndump_dump_char(( int)'}');
goto _LL975; _LL991: Cyc_Absyndump_dump(({ unsigned char* _temp1134="break;";
struct _tagged_string _temp1135; _temp1135.curr= _temp1134; _temp1135.base=
_temp1134; _temp1135.last_plus_one= _temp1134 + 7; _temp1135;})); goto _LL975;
_LL993: Cyc_Absyndump_dump(({ unsigned char* _temp1136="continue;"; struct
_tagged_string _temp1137; _temp1137.curr= _temp1136; _temp1137.base= _temp1136;
_temp1137.last_plus_one= _temp1136 + 10; _temp1137;})); goto _LL975; _LL995: Cyc_Absyndump_dump(({
unsigned char* _temp1138="goto"; struct _tagged_string _temp1139; _temp1139.curr=
_temp1138; _temp1139.base= _temp1138; _temp1139.last_plus_one= _temp1138 + 5;
_temp1139;})); Cyc_Absyndump_dump_str( _temp1050); Cyc_Absyndump_dump_semi();
goto _LL975; _LL997: Cyc_Absyndump_dump(({ unsigned char* _temp1140="for(";
struct _tagged_string _temp1141; _temp1141.curr= _temp1140; _temp1141.base=
_temp1140; _temp1141.last_plus_one= _temp1140 + 5; _temp1141;})); Cyc_Absyndump_dumpexp(
_temp1066); Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpexp( _temp1064); Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpexp( _temp1058); Cyc_Absyndump_dump_nospace(({ unsigned char*
_temp1142="){"; struct _tagged_string _temp1143; _temp1143.curr= _temp1142;
_temp1143.base= _temp1142; _temp1143.last_plus_one= _temp1142 + 3; _temp1143;}));
Cyc_Absyndump_dumpstmt( _temp1052); Cyc_Absyndump_dump_char(( int)'}'); goto
_LL975; _LL999: Cyc_Absyndump_dump(({ unsigned char* _temp1144="switch("; struct
_tagged_string _temp1145; _temp1145.curr= _temp1144; _temp1145.base= _temp1144;
_temp1145.last_plus_one= _temp1144 + 8; _temp1145;})); Cyc_Absyndump_dumpexp(
_temp1070); Cyc_Absyndump_dump_nospace(({ unsigned char* _temp1146="){"; struct
_tagged_string _temp1147; _temp1147.curr= _temp1146; _temp1147.base= _temp1146;
_temp1147.last_plus_one= _temp1146 + 3; _temp1147;})); Cyc_Absyndump_dumpswitchclauses(
_temp1068); Cyc_Absyndump_dump_char(( int)'}'); goto _LL975; _LL1001: Cyc_Absyndump_dumpdecl(
_temp1074); Cyc_Absyndump_dumpstmt( _temp1072); goto _LL975; _LL1003: if( Cyc_Absynpp_is_declaration(
_temp1076)){ Cyc_Absyndump_dump_str( _temp1078); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp1148=": {"; struct _tagged_string _temp1149; _temp1149.curr=
_temp1148; _temp1149.base= _temp1148; _temp1149.last_plus_one= _temp1148 + 4;
_temp1149;})); Cyc_Absyndump_dumpstmt( _temp1076); Cyc_Absyndump_dump_char(( int)'}');}
else{ Cyc_Absyndump_dump_str( _temp1078); Cyc_Absyndump_dump_char(( int)':');
Cyc_Absyndump_dumpstmt( _temp1076);} goto _LL975; _LL1005: Cyc_Absyndump_dump(({
unsigned char* _temp1150="do {"; struct _tagged_string _temp1151; _temp1151.curr=
_temp1150; _temp1151.base= _temp1150; _temp1151.last_plus_one= _temp1150 + 5;
_temp1151;})); Cyc_Absyndump_dumpstmt( _temp1086); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp1152="} while ("; struct _tagged_string _temp1153; _temp1153.curr=
_temp1152; _temp1153.base= _temp1152; _temp1153.last_plus_one= _temp1152 + 10;
_temp1153;})); Cyc_Absyndump_dumpexp( _temp1084); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp1154=");"; struct _tagged_string _temp1155; _temp1155.curr=
_temp1154; _temp1155.base= _temp1154; _temp1155.last_plus_one= _temp1154 + 3;
_temp1155;})); goto _LL975; _LL1007: Cyc_Absyndump_dump(({ unsigned char*
_temp1156="switch \"C\" ("; struct _tagged_string _temp1157; _temp1157.curr=
_temp1156; _temp1157.base= _temp1156; _temp1157.last_plus_one= _temp1156 + 13;
_temp1157;})); Cyc_Absyndump_dumpexp( _temp1090); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp1158="){"; struct _tagged_string _temp1159; _temp1159.curr=
_temp1158; _temp1159.base= _temp1158; _temp1159.last_plus_one= _temp1158 + 3;
_temp1159;})); for( 0; _temp1088 != 0; _temp1088=(( struct Cyc_List_List*)
_check_null( _temp1088))->tl){ struct Cyc_Absyn_SwitchC_clause _temp1162; struct
Cyc_Position_Segment* _temp1163; struct Cyc_Absyn_Stmt* _temp1165; struct Cyc_Absyn_Exp*
_temp1167; struct Cyc_Absyn_SwitchC_clause* _temp1160=( struct Cyc_Absyn_SwitchC_clause*)((
struct Cyc_List_List*) _check_null( _temp1088))->hd; _temp1162=* _temp1160;
_LL1168: _temp1167= _temp1162.cnst_exp; goto _LL1166; _LL1166: _temp1165=
_temp1162.body; goto _LL1164; _LL1164: _temp1163= _temp1162.loc; goto _LL1161;
_LL1161: if( _temp1167 == 0){ Cyc_Absyndump_dump(({ unsigned char* _temp1169="default: ";
struct _tagged_string _temp1170; _temp1170.curr= _temp1169; _temp1170.base=
_temp1169; _temp1170.last_plus_one= _temp1169 + 10; _temp1170;}));} else{ Cyc_Absyndump_dump(({
unsigned char* _temp1171="case "; struct _tagged_string _temp1172; _temp1172.curr=
_temp1171; _temp1172.base= _temp1171; _temp1172.last_plus_one= _temp1171 + 6;
_temp1172;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null(
_temp1167)); Cyc_Absyndump_dump_char(( int)':');} Cyc_Absyndump_dumpstmt(
_temp1165);} Cyc_Absyndump_dump_char(( int)'}'); goto _LL975; _LL1009: Cyc_Absyndump_dump(({
unsigned char* _temp1173="fallthru;"; struct _tagged_string _temp1174; _temp1174.curr=
_temp1173; _temp1174.base= _temp1173; _temp1174.last_plus_one= _temp1173 + 10;
_temp1174;})); goto _LL975; _LL1011: Cyc_Absyndump_dump(({ unsigned char*
_temp1175="fallthru("; struct _tagged_string _temp1176; _temp1176.curr=
_temp1175; _temp1176.base= _temp1175; _temp1176.last_plus_one= _temp1175 + 10;
_temp1176;})); Cyc_Absyndump_dumpexps_prec( 20, _temp1098); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp1177=");"; struct _tagged_string _temp1178; _temp1178.curr=
_temp1177; _temp1178.base= _temp1177; _temp1178.last_plus_one= _temp1177 + 3;
_temp1178;})); goto _LL975; _LL1013: Cyc_Absyndump_dump(({ unsigned char*
_temp1179="try"; struct _tagged_string _temp1180; _temp1180.curr= _temp1179;
_temp1180.base= _temp1179; _temp1180.last_plus_one= _temp1179 + 4; _temp1180;}));
Cyc_Absyndump_dumpstmt( _temp1102); Cyc_Absyndump_dump(({ unsigned char*
_temp1181="catch {"; struct _tagged_string _temp1182; _temp1182.curr= _temp1181;
_temp1182.base= _temp1181; _temp1182.last_plus_one= _temp1181 + 8; _temp1182;}));
Cyc_Absyndump_dumpswitchclauses( _temp1100); Cyc_Absyndump_dump_char(( int)'}');
goto _LL975; _LL1015: Cyc_Absyndump_dump(({ unsigned char* _temp1183="region<";
struct _tagged_string _temp1184; _temp1184.curr= _temp1183; _temp1184.base=
_temp1183; _temp1184.last_plus_one= _temp1183 + 8; _temp1184;})); Cyc_Absyndump_dumptvar(
_temp1108); Cyc_Absyndump_dump(({ unsigned char* _temp1185="> "; struct
_tagged_string _temp1186; _temp1186.curr= _temp1185; _temp1186.base= _temp1185;
_temp1186.last_plus_one= _temp1185 + 3; _temp1186;})); Cyc_Absyndump_dumpqvar(
_temp1106->name); Cyc_Absyndump_dump(({ unsigned char* _temp1187="{"; struct
_tagged_string _temp1188; _temp1188.curr= _temp1187; _temp1188.base= _temp1187;
_temp1188.last_plus_one= _temp1187 + 2; _temp1188;})); Cyc_Absyndump_dumpstmt(
_temp1104); Cyc_Absyndump_dump(({ unsigned char* _temp1189="}"; struct
_tagged_string _temp1190; _temp1190.curr= _temp1189; _temp1190.base= _temp1189;
_temp1190.last_plus_one= _temp1189 + 2; _temp1190;})); goto _LL975; _LL1017: Cyc_Absyndump_dump(({
unsigned char* _temp1191="cut"; struct _tagged_string _temp1192; _temp1192.curr=
_temp1191; _temp1192.base= _temp1191; _temp1192.last_plus_one= _temp1191 + 4;
_temp1192;})); Cyc_Absyndump_dumpstmt( _temp1110); goto _LL975; _LL1019: Cyc_Absyndump_dump(({
unsigned char* _temp1193="splice"; struct _tagged_string _temp1194; _temp1194.curr=
_temp1193; _temp1194.base= _temp1193; _temp1194.last_plus_one= _temp1193 + 7;
_temp1194;})); Cyc_Absyndump_dumpstmt( _temp1112); goto _LL975; _LL975:;} struct
_tuple8{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; void Cyc_Absyndump_dumpdp(
struct _tuple8* dp){(( void(*)( void(* f)( void*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumpdesignator,(* dp).f1,({
unsigned char* _temp1195=""; struct _tagged_string _temp1196; _temp1196.curr=
_temp1195; _temp1196.base= _temp1195; _temp1196.last_plus_one= _temp1195 + 1;
_temp1196;}),({ unsigned char* _temp1197="="; struct _tagged_string _temp1198;
_temp1198.curr= _temp1197; _temp1198.base= _temp1197; _temp1198.last_plus_one=
_temp1197 + 2; _temp1198;}),({ unsigned char* _temp1199="="; struct
_tagged_string _temp1200; _temp1200.curr= _temp1199; _temp1200.base= _temp1199;
_temp1200.last_plus_one= _temp1199 + 2; _temp1200;})); Cyc_Absyndump_dumppat((*
dp).f2);} void Cyc_Absyndump_dumppat( struct Cyc_Absyn_Pat* p){ void* _temp1201=(
void*) p->r; int _temp1235; void* _temp1237; int _temp1239; void* _temp1241;
unsigned char _temp1243; struct _tagged_string _temp1245; struct Cyc_Absyn_Vardecl*
_temp1247; struct Cyc_List_List* _temp1249; struct Cyc_Absyn_Pat* _temp1251;
struct Cyc_Absyn_Vardecl* _temp1253; struct _tuple0* _temp1255; struct Cyc_List_List*
_temp1257; struct Cyc_List_List* _temp1259; struct _tuple0* _temp1261; struct
Cyc_List_List* _temp1263; struct Cyc_List_List* _temp1265; struct _tuple0*
_temp1267; struct Cyc_List_List* _temp1269; struct Cyc_List_List* _temp1271;
struct Cyc_Core_Opt* _temp1273; struct Cyc_Absyn_Structdecl* _temp1275; struct
Cyc_List_List* _temp1277; struct Cyc_List_List* _temp1279; struct Cyc_Absyn_Tunionfield*
_temp1281; struct Cyc_Absyn_Tuniondecl* _temp1283; struct Cyc_Absyn_Enumfield*
_temp1285; struct Cyc_Absyn_Enumdecl* _temp1287; _LL1203: if( _temp1201 ==( void*)
Cyc_Absyn_Wild_p){ goto _LL1204;} else{ goto _LL1205;} _LL1205: if( _temp1201 ==(
void*) Cyc_Absyn_Null_p){ goto _LL1206;} else{ goto _LL1207;} _LL1207: if((
unsigned int) _temp1201 > 2u?*(( int*) _temp1201) == Cyc_Absyn_Int_p: 0){
_LL1238: _temp1237=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1201)->f1;
if( _temp1237 ==( void*) Cyc_Absyn_Signed){ goto _LL1236;} else{ goto _LL1209;}
_LL1236: _temp1235=(( struct Cyc_Absyn_Int_p_struct*) _temp1201)->f2; goto
_LL1208;} else{ goto _LL1209;} _LL1209: if(( unsigned int) _temp1201 > 2u?*((
int*) _temp1201) == Cyc_Absyn_Int_p: 0){ _LL1242: _temp1241=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1201)->f1; if( _temp1241 ==( void*) Cyc_Absyn_Unsigned){
goto _LL1240;} else{ goto _LL1211;} _LL1240: _temp1239=(( struct Cyc_Absyn_Int_p_struct*)
_temp1201)->f2; goto _LL1210;} else{ goto _LL1211;} _LL1211: if(( unsigned int)
_temp1201 > 2u?*(( int*) _temp1201) == Cyc_Absyn_Char_p: 0){ _LL1244: _temp1243=((
struct Cyc_Absyn_Char_p_struct*) _temp1201)->f1; goto _LL1212;} else{ goto
_LL1213;} _LL1213: if(( unsigned int) _temp1201 > 2u?*(( int*) _temp1201) == Cyc_Absyn_Float_p:
0){ _LL1246: _temp1245=(( struct Cyc_Absyn_Float_p_struct*) _temp1201)->f1; goto
_LL1214;} else{ goto _LL1215;} _LL1215: if(( unsigned int) _temp1201 > 2u?*((
int*) _temp1201) == Cyc_Absyn_Var_p: 0){ _LL1248: _temp1247=(( struct Cyc_Absyn_Var_p_struct*)
_temp1201)->f1; goto _LL1216;} else{ goto _LL1217;} _LL1217: if(( unsigned int)
_temp1201 > 2u?*(( int*) _temp1201) == Cyc_Absyn_Tuple_p: 0){ _LL1250: _temp1249=((
struct Cyc_Absyn_Tuple_p_struct*) _temp1201)->f1; goto _LL1218;} else{ goto
_LL1219;} _LL1219: if(( unsigned int) _temp1201 > 2u?*(( int*) _temp1201) == Cyc_Absyn_Pointer_p:
0){ _LL1252: _temp1251=(( struct Cyc_Absyn_Pointer_p_struct*) _temp1201)->f1;
goto _LL1220;} else{ goto _LL1221;} _LL1221: if(( unsigned int) _temp1201 > 2u?*((
int*) _temp1201) == Cyc_Absyn_Reference_p: 0){ _LL1254: _temp1253=(( struct Cyc_Absyn_Reference_p_struct*)
_temp1201)->f1; goto _LL1222;} else{ goto _LL1223;} _LL1223: if(( unsigned int)
_temp1201 > 2u?*(( int*) _temp1201) == Cyc_Absyn_UnknownId_p: 0){ _LL1256:
_temp1255=(( struct Cyc_Absyn_UnknownId_p_struct*) _temp1201)->f1; goto _LL1224;}
else{ goto _LL1225;} _LL1225: if(( unsigned int) _temp1201 > 2u?*(( int*)
_temp1201) == Cyc_Absyn_UnknownCall_p: 0){ _LL1262: _temp1261=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1201)->f1; goto _LL1260; _LL1260: _temp1259=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1201)->f2; goto _LL1258; _LL1258: _temp1257=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1201)->f3; goto _LL1226;} else{ goto _LL1227;} _LL1227: if(( unsigned int)
_temp1201 > 2u?*(( int*) _temp1201) == Cyc_Absyn_UnknownFields_p: 0){ _LL1268:
_temp1267=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp1201)->f1; goto
_LL1266; _LL1266: _temp1265=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1201)->f2; goto _LL1264; _LL1264: _temp1263=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1201)->f3; goto _LL1228;} else{ goto _LL1229;} _LL1229: if(( unsigned int)
_temp1201 > 2u?*(( int*) _temp1201) == Cyc_Absyn_Struct_p: 0){ _LL1276:
_temp1275=(( struct Cyc_Absyn_Struct_p_struct*) _temp1201)->f1; goto _LL1274;
_LL1274: _temp1273=(( struct Cyc_Absyn_Struct_p_struct*) _temp1201)->f2; goto
_LL1272; _LL1272: _temp1271=(( struct Cyc_Absyn_Struct_p_struct*) _temp1201)->f3;
goto _LL1270; _LL1270: _temp1269=(( struct Cyc_Absyn_Struct_p_struct*) _temp1201)->f4;
goto _LL1230;} else{ goto _LL1231;} _LL1231: if(( unsigned int) _temp1201 > 2u?*((
int*) _temp1201) == Cyc_Absyn_Tunion_p: 0){ _LL1284: _temp1283=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1201)->f1; goto _LL1282; _LL1282: _temp1281=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1201)->f2; goto _LL1280; _LL1280: _temp1279=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1201)->f3; goto _LL1278; _LL1278: _temp1277=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1201)->f4; goto _LL1232;} else{ goto _LL1233;} _LL1233: if(( unsigned int)
_temp1201 > 2u?*(( int*) _temp1201) == Cyc_Absyn_Enum_p: 0){ _LL1288: _temp1287=((
struct Cyc_Absyn_Enum_p_struct*) _temp1201)->f1; goto _LL1286; _LL1286:
_temp1285=(( struct Cyc_Absyn_Enum_p_struct*) _temp1201)->f2; goto _LL1234;}
else{ goto _LL1202;} _LL1204: Cyc_Absyndump_dump_char(( int)'_'); goto _LL1202;
_LL1206: Cyc_Absyndump_dump(({ unsigned char* _temp1289="null"; struct
_tagged_string _temp1290; _temp1290.curr= _temp1289; _temp1290.base= _temp1289;
_temp1290.last_plus_one= _temp1289 + 5; _temp1290;})); goto _LL1202; _LL1208:
Cyc_Absyndump_dump( xprintf("%d", _temp1235)); goto _LL1202; _LL1210: Cyc_Absyndump_dump(
xprintf("%u",( unsigned int) _temp1239)); goto _LL1202; _LL1212: Cyc_Absyndump_dump(({
unsigned char* _temp1291="'"; struct _tagged_string _temp1292; _temp1292.curr=
_temp1291; _temp1292.base= _temp1291; _temp1292.last_plus_one= _temp1291 + 2;
_temp1292;})); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_char_escape( _temp1243));
Cyc_Absyndump_dump_nospace(({ unsigned char* _temp1293="'"; struct
_tagged_string _temp1294; _temp1294.curr= _temp1293; _temp1294.base= _temp1293;
_temp1294.last_plus_one= _temp1293 + 2; _temp1294;})); goto _LL1202; _LL1214:
Cyc_Absyndump_dump( _temp1245); goto _LL1202; _LL1216: Cyc_Absyndump_dumpqvar(
_temp1247->name); goto _LL1202; _LL1218:(( void(*)( void(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat,
_temp1249,({ unsigned char* _temp1295="$("; struct _tagged_string _temp1296;
_temp1296.curr= _temp1295; _temp1296.base= _temp1295; _temp1296.last_plus_one=
_temp1295 + 3; _temp1296;}),({ unsigned char* _temp1297=")"; struct
_tagged_string _temp1298; _temp1298.curr= _temp1297; _temp1298.base= _temp1297;
_temp1298.last_plus_one= _temp1297 + 2; _temp1298;}),({ unsigned char* _temp1299=",";
struct _tagged_string _temp1300; _temp1300.curr= _temp1299; _temp1300.base=
_temp1299; _temp1300.last_plus_one= _temp1299 + 2; _temp1300;})); goto _LL1202;
_LL1220: Cyc_Absyndump_dump(({ unsigned char* _temp1301="&"; struct
_tagged_string _temp1302; _temp1302.curr= _temp1301; _temp1302.base= _temp1301;
_temp1302.last_plus_one= _temp1301 + 2; _temp1302;})); Cyc_Absyndump_dumppat(
_temp1251); goto _LL1202; _LL1222: Cyc_Absyndump_dump(({ unsigned char*
_temp1303="*"; struct _tagged_string _temp1304; _temp1304.curr= _temp1303;
_temp1304.base= _temp1303; _temp1304.last_plus_one= _temp1303 + 2; _temp1304;}));
Cyc_Absyndump_dumpqvar( _temp1253->name); goto _LL1202; _LL1224: Cyc_Absyndump_dumpqvar(
_temp1255); goto _LL1202; _LL1226: Cyc_Absyndump_dumpqvar( _temp1261); Cyc_Absyndump_dumptvars(
_temp1259);(( void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp1257,({ unsigned char*
_temp1305="("; struct _tagged_string _temp1306; _temp1306.curr= _temp1305;
_temp1306.base= _temp1305; _temp1306.last_plus_one= _temp1305 + 2; _temp1306;}),({
unsigned char* _temp1307=")"; struct _tagged_string _temp1308; _temp1308.curr=
_temp1307; _temp1308.base= _temp1307; _temp1308.last_plus_one= _temp1307 + 2;
_temp1308;}),({ unsigned char* _temp1309=","; struct _tagged_string _temp1310;
_temp1310.curr= _temp1309; _temp1310.base= _temp1309; _temp1310.last_plus_one=
_temp1309 + 2; _temp1310;})); goto _LL1202; _LL1228: Cyc_Absyndump_dumpqvar(
_temp1267); Cyc_Absyndump_dumptvars( _temp1265);(( void(*)( void(* f)( struct
_tuple8*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpdp,
_temp1263,({ unsigned char* _temp1311="{"; struct _tagged_string _temp1312;
_temp1312.curr= _temp1311; _temp1312.base= _temp1311; _temp1312.last_plus_one=
_temp1311 + 2; _temp1312;}),({ unsigned char* _temp1313="}"; struct
_tagged_string _temp1314; _temp1314.curr= _temp1313; _temp1314.base= _temp1313;
_temp1314.last_plus_one= _temp1313 + 2; _temp1314;}),({ unsigned char* _temp1315=",";
struct _tagged_string _temp1316; _temp1316.curr= _temp1315; _temp1316.base=
_temp1315; _temp1316.last_plus_one= _temp1315 + 2; _temp1316;})); goto _LL1202;
_LL1230: if( _temp1275->name != 0){ Cyc_Absyndump_dumpqvar(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( _temp1275->name))->v);} Cyc_Absyndump_dumptvars(
_temp1271);(( void(*)( void(* f)( struct _tuple8*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpdp, _temp1269,({ unsigned char*
_temp1317="{"; struct _tagged_string _temp1318; _temp1318.curr= _temp1317;
_temp1318.base= _temp1317; _temp1318.last_plus_one= _temp1317 + 2; _temp1318;}),({
unsigned char* _temp1319="}"; struct _tagged_string _temp1320; _temp1320.curr=
_temp1319; _temp1320.base= _temp1319; _temp1320.last_plus_one= _temp1319 + 2;
_temp1320;}),({ unsigned char* _temp1321=","; struct _tagged_string _temp1322;
_temp1322.curr= _temp1321; _temp1322.base= _temp1321; _temp1322.last_plus_one=
_temp1321 + 2; _temp1322;})); goto _LL1202; _LL1232: Cyc_Absyndump_dumpqvar(
_temp1281->name); Cyc_Absyndump_dumptvars( _temp1279); if( _temp1277 != 0){((
void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat, _temp1277,({ unsigned char* _temp1323="("; struct
_tagged_string _temp1324; _temp1324.curr= _temp1323; _temp1324.base= _temp1323;
_temp1324.last_plus_one= _temp1323 + 2; _temp1324;}),({ unsigned char* _temp1325=")";
struct _tagged_string _temp1326; _temp1326.curr= _temp1325; _temp1326.base=
_temp1325; _temp1326.last_plus_one= _temp1325 + 2; _temp1326;}),({ unsigned char*
_temp1327=","; struct _tagged_string _temp1328; _temp1328.curr= _temp1327;
_temp1328.base= _temp1327; _temp1328.last_plus_one= _temp1327 + 2; _temp1328;}));}
goto _LL1202; _LL1234: Cyc_Absyndump_dumpqvar( _temp1285->name); goto _LL1202;
_LL1202:;} void Cyc_Absyndump_dumptunionfield( struct Cyc_Absyn_Tunionfield* ef){
Cyc_Absyndump_dumpqvar( ef->name); if( ef->typs != 0){ Cyc_Absyndump_dumpargs(
ef->typs);}} void Cyc_Absyndump_dumptunionfields( struct Cyc_List_List* fields){((
void(*)( void(* f)( struct Cyc_Absyn_Tunionfield*), struct Cyc_List_List* l,
struct _tagged_string sep)) Cyc_Absyndump_dump_sep)( Cyc_Absyndump_dumptunionfield,
fields,({ unsigned char* _temp1329=","; struct _tagged_string _temp1330;
_temp1330.curr= _temp1329; _temp1330.base= _temp1329; _temp1330.last_plus_one=
_temp1329 + 2; _temp1330;}));} void Cyc_Absyndump_dumpenumfield( struct Cyc_Absyn_Enumfield*
ef){ Cyc_Absyndump_dumpqvar( ef->name); if( ef->tag != 0){ Cyc_Absyndump_dump(({
unsigned char* _temp1331=" = "; struct _tagged_string _temp1332; _temp1332.curr=
_temp1331; _temp1332.base= _temp1331; _temp1332.last_plus_one= _temp1331 + 4;
_temp1332;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null( ef->tag));}}
void Cyc_Absyndump_dumpenumfields( struct Cyc_List_List* fields){(( void(*)(
void(* f)( struct Cyc_Absyn_Enumfield*), struct Cyc_List_List* l, struct
_tagged_string sep)) Cyc_Absyndump_dump_sep)( Cyc_Absyndump_dumpenumfield,
fields,({ unsigned char* _temp1333=","; struct _tagged_string _temp1334;
_temp1334.curr= _temp1333; _temp1334.base= _temp1333; _temp1334.last_plus_one=
_temp1333 + 2; _temp1334;}));} void Cyc_Absyndump_dumpstructfields( struct Cyc_List_List*
fields){ for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null(
fields))->tl){ struct Cyc_Absyn_Structfield _temp1337; struct Cyc_List_List*
_temp1338; struct Cyc_Core_Opt* _temp1340; void* _temp1342; struct Cyc_Absyn_Tqual
_temp1344; struct _tagged_string* _temp1346; struct Cyc_Absyn_Structfield*
_temp1335=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd; _temp1337=* _temp1335; _LL1347: _temp1346= _temp1337.name; goto
_LL1345; _LL1345: _temp1344= _temp1337.tq; goto _LL1343; _LL1343: _temp1342=(
void*) _temp1337.type; goto _LL1341; _LL1341: _temp1340= _temp1337.width; goto
_LL1339; _LL1339: _temp1338= _temp1337.attributes; goto _LL1336; _LL1336:(( void(*)(
struct Cyc_Absyn_Tqual, void*, void(* f)( struct _tagged_string*), struct
_tagged_string*)) Cyc_Absyndump_dumptqtd)( _temp1344, _temp1342, Cyc_Absyndump_dump_str,
_temp1346); Cyc_Absyndump_dumpatts( _temp1338); if( _temp1340 != 0){ Cyc_Absyndump_dump_char((
int)':'); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*)
_check_null( _temp1340))->v);} Cyc_Absyndump_dump_semi();}} void Cyc_Absyndump_dumptypedefname(
struct Cyc_Absyn_Typedefdecl* td){ Cyc_Absyndump_dumpqvar( td->name); Cyc_Absyndump_dumptvars(
td->tvs);} static void Cyc_Absyndump_dump_atts_qvar( struct Cyc_Absyn_Fndecl* fd){
Cyc_Absyndump_dumpatts( fd->attributes); Cyc_Absyndump_dumpqvar( fd->name);}
struct _tuple9{ void* f1; struct _tuple0* f2; } ; static void Cyc_Absyndump_dump_callconv_qvar(
struct _tuple9* pr){{ void* _temp1348=(* pr).f1; _LL1350: if( _temp1348 ==( void*)
Cyc_Absyn_Unused_att){ goto _LL1351;} else{ goto _LL1352;} _LL1352: if(
_temp1348 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL1353;} else{ goto _LL1354;}
_LL1354: if( _temp1348 ==( void*) Cyc_Absyn_Cdecl_att){ goto _LL1355;} else{
goto _LL1356;} _LL1356: if( _temp1348 ==( void*) Cyc_Absyn_Fastcall_att){ goto
_LL1357;} else{ goto _LL1358;} _LL1358: goto _LL1359; _LL1351: goto _LL1349;
_LL1353: Cyc_Absyndump_dump(({ unsigned char* _temp1360="_stdcall"; struct
_tagged_string _temp1361; _temp1361.curr= _temp1360; _temp1361.base= _temp1360;
_temp1361.last_plus_one= _temp1360 + 9; _temp1361;})); goto _LL1349; _LL1355:
Cyc_Absyndump_dump(({ unsigned char* _temp1362="_cdecl"; struct _tagged_string
_temp1363; _temp1363.curr= _temp1362; _temp1363.base= _temp1362; _temp1363.last_plus_one=
_temp1362 + 7; _temp1363;})); goto _LL1349; _LL1357: Cyc_Absyndump_dump(({
unsigned char* _temp1364="_fastcall"; struct _tagged_string _temp1365; _temp1365.curr=
_temp1364; _temp1365.base= _temp1364; _temp1365.last_plus_one= _temp1364 + 10;
_temp1365;})); goto _LL1349; _LL1359: goto _LL1349; _LL1349:;} Cyc_Absyndump_dumpqvar((*
pr).f2);} static void Cyc_Absyndump_dump_callconv_fdqvar( struct Cyc_Absyn_Fndecl*
fd){ Cyc_Absyndump_dump_callconv( fd->attributes); Cyc_Absyndump_dumpqvar( fd->name);}
static void Cyc_Absyndump_dumpids( struct Cyc_List_List* vds){ for( 0; vds != 0;
vds=(( struct Cyc_List_List*) _check_null( vds))->tl){ Cyc_Absyndump_dumpqvar(((
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( vds))->hd)->name);
if((( struct Cyc_List_List*) _check_null( vds))->tl != 0){ Cyc_Absyndump_dump_char((
int)',');}}} void Cyc_Absyndump_dumpdecl( struct Cyc_Absyn_Decl* d){ void*
_temp1366=( void*) d->r; struct Cyc_Absyn_Fndecl* _temp1392; struct Cyc_Absyn_Structdecl*
_temp1394; struct Cyc_Absyn_Uniondecl* _temp1396; struct Cyc_Absyn_Vardecl*
_temp1398; struct Cyc_Absyn_Vardecl _temp1400; struct Cyc_List_List* _temp1401;
struct Cyc_Core_Opt* _temp1403; struct Cyc_Absyn_Exp* _temp1405; void* _temp1407;
struct Cyc_Absyn_Tqual _temp1409; struct _tuple0* _temp1411; void* _temp1413;
struct Cyc_Absyn_Tuniondecl* _temp1415; struct Cyc_Absyn_Tuniondecl _temp1417;
int _temp1418; struct Cyc_Core_Opt* _temp1420; struct Cyc_List_List* _temp1422;
struct _tuple0* _temp1424; void* _temp1426; struct Cyc_Absyn_Enumdecl* _temp1428;
struct Cyc_Absyn_Enumdecl _temp1430; struct Cyc_Core_Opt* _temp1431; struct
_tuple0* _temp1433; void* _temp1435; int _temp1437; struct Cyc_Absyn_Exp*
_temp1439; struct Cyc_Core_Opt* _temp1441; struct Cyc_Core_Opt* _temp1443;
struct Cyc_Absyn_Pat* _temp1445; struct Cyc_List_List* _temp1447; struct Cyc_Absyn_Typedefdecl*
_temp1449; struct Cyc_List_List* _temp1451; struct _tagged_string* _temp1453;
struct Cyc_List_List* _temp1455; struct _tuple0* _temp1457; struct Cyc_List_List*
_temp1459; _LL1368: if(*(( int*) _temp1366) == Cyc_Absyn_Fn_d){ _LL1393:
_temp1392=(( struct Cyc_Absyn_Fn_d_struct*) _temp1366)->f1; goto _LL1369;} else{
goto _LL1370;} _LL1370: if(*(( int*) _temp1366) == Cyc_Absyn_Struct_d){ _LL1395:
_temp1394=(( struct Cyc_Absyn_Struct_d_struct*) _temp1366)->f1; goto _LL1371;}
else{ goto _LL1372;} _LL1372: if(*(( int*) _temp1366) == Cyc_Absyn_Union_d){
_LL1397: _temp1396=(( struct Cyc_Absyn_Union_d_struct*) _temp1366)->f1; goto
_LL1373;} else{ goto _LL1374;} _LL1374: if(*(( int*) _temp1366) == Cyc_Absyn_Var_d){
_LL1399: _temp1398=(( struct Cyc_Absyn_Var_d_struct*) _temp1366)->f1; _temp1400=*
_temp1398; _LL1414: _temp1413=( void*) _temp1400.sc; goto _LL1412; _LL1412:
_temp1411= _temp1400.name; goto _LL1410; _LL1410: _temp1409= _temp1400.tq; goto
_LL1408; _LL1408: _temp1407=( void*) _temp1400.type; goto _LL1406; _LL1406:
_temp1405= _temp1400.initializer; goto _LL1404; _LL1404: _temp1403= _temp1400.rgn;
goto _LL1402; _LL1402: _temp1401= _temp1400.attributes; goto _LL1375;} else{
goto _LL1376;} _LL1376: if(*(( int*) _temp1366) == Cyc_Absyn_Tunion_d){ _LL1416:
_temp1415=(( struct Cyc_Absyn_Tunion_d_struct*) _temp1366)->f1; _temp1417=*
_temp1415; _LL1427: _temp1426=( void*) _temp1417.sc; goto _LL1425; _LL1425:
_temp1424= _temp1417.name; goto _LL1423; _LL1423: _temp1422= _temp1417.tvs; goto
_LL1421; _LL1421: _temp1420= _temp1417.fields; goto _LL1419; _LL1419: _temp1418=
_temp1417.is_xtunion; goto _LL1377;} else{ goto _LL1378;} _LL1378: if(*(( int*)
_temp1366) == Cyc_Absyn_Enum_d){ _LL1429: _temp1428=(( struct Cyc_Absyn_Enum_d_struct*)
_temp1366)->f1; _temp1430=* _temp1428; _LL1436: _temp1435=( void*) _temp1430.sc;
goto _LL1434; _LL1434: _temp1433= _temp1430.name; goto _LL1432; _LL1432:
_temp1431= _temp1430.fields; goto _LL1379;} else{ goto _LL1380;} _LL1380: if(*((
int*) _temp1366) == Cyc_Absyn_Let_d){ _LL1446: _temp1445=(( struct Cyc_Absyn_Let_d_struct*)
_temp1366)->f1; goto _LL1444; _LL1444: _temp1443=(( struct Cyc_Absyn_Let_d_struct*)
_temp1366)->f2; goto _LL1442; _LL1442: _temp1441=(( struct Cyc_Absyn_Let_d_struct*)
_temp1366)->f3; goto _LL1440; _LL1440: _temp1439=(( struct Cyc_Absyn_Let_d_struct*)
_temp1366)->f4; goto _LL1438; _LL1438: _temp1437=(( struct Cyc_Absyn_Let_d_struct*)
_temp1366)->f5; goto _LL1381;} else{ goto _LL1382;} _LL1382: if(*(( int*)
_temp1366) == Cyc_Absyn_Letv_d){ _LL1448: _temp1447=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1366)->f1; goto _LL1383;} else{ goto _LL1384;} _LL1384: if(*(( int*)
_temp1366) == Cyc_Absyn_Typedef_d){ _LL1450: _temp1449=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp1366)->f1; goto _LL1385;} else{ goto _LL1386;} _LL1386: if(*(( int*)
_temp1366) == Cyc_Absyn_Namespace_d){ _LL1454: _temp1453=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1366)->f1; goto _LL1452; _LL1452: _temp1451=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1366)->f2; goto _LL1387;} else{ goto _LL1388;} _LL1388: if(*(( int*)
_temp1366) == Cyc_Absyn_Using_d){ _LL1458: _temp1457=(( struct Cyc_Absyn_Using_d_struct*)
_temp1366)->f1; goto _LL1456; _LL1456: _temp1455=(( struct Cyc_Absyn_Using_d_struct*)
_temp1366)->f2; goto _LL1389;} else{ goto _LL1390;} _LL1390: if(*(( int*)
_temp1366) == Cyc_Absyn_ExternC_d){ _LL1460: _temp1459=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp1366)->f1; goto _LL1391;} else{ goto _LL1367;} _LL1369: if( Cyc_Absynpp_to_VC){
Cyc_Absyndump_dumpatts( _temp1392->attributes);} if( _temp1392->is_inline){ if(
Cyc_Absynpp_to_VC){ Cyc_Absyndump_dump(({ unsigned char* _temp1461="__inline";
struct _tagged_string _temp1462; _temp1462.curr= _temp1461; _temp1462.base=
_temp1461; _temp1462.last_plus_one= _temp1461 + 9; _temp1462;}));} else{ Cyc_Absyndump_dump(({
unsigned char* _temp1463="inline"; struct _tagged_string _temp1464; _temp1464.curr=
_temp1463; _temp1464.base= _temp1463; _temp1464.last_plus_one= _temp1463 + 7;
_temp1464;}));}} Cyc_Absyndump_dumpscope(( void*) _temp1392->sc);{ void* t=(
void*)({ struct Cyc_Absyn_FnType_struct* _temp1466=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1466[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1467; _temp1467.tag= Cyc_Absyn_FnType; _temp1467.f1=({ struct Cyc_Absyn_FnInfo
_temp1468; _temp1468.tvars= _temp1392->tvs; _temp1468.effect= _temp1392->effect;
_temp1468.ret_typ=( void*)(( void*) _temp1392->ret_type); _temp1468.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp1392->args); _temp1468.c_varargs=
_temp1392->c_varargs; _temp1468.cyc_varargs= _temp1392->cyc_varargs; _temp1468.attributes=
0; _temp1468;}); _temp1467;}); _temp1466;});(( void(*)( struct Cyc_Absyn_Tqual,
void*, void(* f)( struct Cyc_Absyn_Fndecl*), struct Cyc_Absyn_Fndecl*)) Cyc_Absyndump_dumptqtd)(({
struct Cyc_Absyn_Tqual _temp1465; _temp1465.q_const= 0; _temp1465.q_volatile= 0;
_temp1465.q_restrict= 0; _temp1465;}), t, Cyc_Absynpp_to_VC? Cyc_Absyndump_dump_callconv_fdqvar:
Cyc_Absyndump_dump_atts_qvar, _temp1392); Cyc_Absyndump_dump_char(( int)'{');
Cyc_Absyndump_dumpstmt( _temp1392->body); Cyc_Absyndump_dump_char(( int)'}');
goto _LL1367;} _LL1371: Cyc_Absyndump_dumpscope(( void*) _temp1394->sc); Cyc_Absyndump_dump(({
unsigned char* _temp1469="struct"; struct _tagged_string _temp1470; _temp1470.curr=
_temp1469; _temp1470.base= _temp1469; _temp1470.last_plus_one= _temp1469 + 7;
_temp1470;})); if( _temp1394->name != 0){ Cyc_Absyndump_dumpqvar(( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp1394->name))->v);} Cyc_Absyndump_dumptvars(
_temp1394->tvs); if( _temp1394->fields == 0){ Cyc_Absyndump_dump_semi();} else{
Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstructfields(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1394->fields))->v); Cyc_Absyndump_dump(({
unsigned char* _temp1471="}"; struct _tagged_string _temp1472; _temp1472.curr=
_temp1471; _temp1472.base= _temp1471; _temp1472.last_plus_one= _temp1471 + 2;
_temp1472;})); Cyc_Absyndump_dumpatts( _temp1394->attributes); Cyc_Absyndump_dump(({
unsigned char* _temp1473=";"; struct _tagged_string _temp1474; _temp1474.curr=
_temp1473; _temp1474.base= _temp1473; _temp1474.last_plus_one= _temp1473 + 2;
_temp1474;}));} goto _LL1367; _LL1373: Cyc_Absyndump_dumpscope(( void*)
_temp1396->sc); Cyc_Absyndump_dump(({ unsigned char* _temp1475="union"; struct
_tagged_string _temp1476; _temp1476.curr= _temp1475; _temp1476.base= _temp1475;
_temp1476.last_plus_one= _temp1475 + 6; _temp1476;})); if( _temp1396->name != 0){
Cyc_Absyndump_dumpqvar(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp1396->name))->v);} Cyc_Absyndump_dumptvars( _temp1396->tvs); if( _temp1396->fields
== 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstructfields((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1396->fields))->v);
Cyc_Absyndump_dump(({ unsigned char* _temp1477="}"; struct _tagged_string
_temp1478; _temp1478.curr= _temp1477; _temp1478.base= _temp1477; _temp1478.last_plus_one=
_temp1477 + 2; _temp1478;})); Cyc_Absyndump_dumpatts( _temp1396->attributes);
Cyc_Absyndump_dump(({ unsigned char* _temp1479=";"; struct _tagged_string
_temp1480; _temp1480.curr= _temp1479; _temp1480.base= _temp1479; _temp1480.last_plus_one=
_temp1479 + 2; _temp1480;}));} goto _LL1367; _LL1375: if( Cyc_Absynpp_to_VC){
Cyc_Absyndump_dumpatts( _temp1401); Cyc_Absyndump_dumpscope( _temp1413);{ struct
Cyc_List_List* _temp1483; void* _temp1485; struct Cyc_Absyn_Tqual _temp1487;
struct _tuple4 _temp1481= Cyc_Absynpp_to_tms( _temp1409, _temp1407); _LL1488:
_temp1487= _temp1481.f1; goto _LL1486; _LL1486: _temp1485= _temp1481.f2; goto
_LL1484; _LL1484: _temp1483= _temp1481.f3; goto _LL1482; _LL1482: Cyc_Absyndump_dumptq(
_temp1487);{ void* call_conv=( void*) Cyc_Absyn_Unused_att;{ struct Cyc_List_List*
tms2= _temp1483; for( 0; tms2 != 0; tms2=(( struct Cyc_List_List*) _check_null(
tms2))->tl){ void* _temp1489=( void*)(( struct Cyc_List_List*) _check_null( tms2))->hd;
struct Cyc_List_List* _temp1495; struct Cyc_Position_Segment* _temp1497; _LL1491:
if(( unsigned int) _temp1489 > 1u?*(( int*) _temp1489) == Cyc_Absyn_Attributes_mod:
0){ _LL1498: _temp1497=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp1489)->f1;
goto _LL1496; _LL1496: _temp1495=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp1489)->f2; goto _LL1492;} else{ goto _LL1493;} _LL1493: goto _LL1494;
_LL1492: for( 0; _temp1495 != 0; _temp1495=(( struct Cyc_List_List*) _check_null(
_temp1495))->tl){ void* _temp1499=( void*)(( struct Cyc_List_List*) _check_null(
_temp1495))->hd; _LL1501: if( _temp1499 ==( void*) Cyc_Absyn_Stdcall_att){ goto
_LL1502;} else{ goto _LL1503;} _LL1503: if( _temp1499 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL1504;} else{ goto _LL1505;} _LL1505: if( _temp1499 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL1506;} else{ goto _LL1507;} _LL1507: goto _LL1508; _LL1502: call_conv=(
void*) Cyc_Absyn_Stdcall_att; goto _LL1500; _LL1504: call_conv=( void*) Cyc_Absyn_Cdecl_att;
goto _LL1500; _LL1506: call_conv=( void*) Cyc_Absyn_Fastcall_att; goto _LL1500;
_LL1508: goto _LL1500; _LL1500:;} goto _LL1490; _LL1494: goto _LL1490; _LL1490:;}}
Cyc_Absyndump_dumptq( _temp1487); Cyc_Absyndump_dumpntyp( _temp1485);{ struct
_tuple9 _temp1510=({ struct _tuple9 _temp1509; _temp1509.f1= call_conv;
_temp1509.f2= _temp1411; _temp1509;}); goto _LL1511; _LL1511:(( void(*)( struct
Cyc_List_List* tms, void(* f)( struct _tuple9*), struct _tuple9* a)) Cyc_Absyndump_dumptms)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1483),
Cyc_Absyndump_dump_callconv_qvar,& _temp1510);}}}} else{ Cyc_Absyndump_dumpscope(
_temp1413);(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct _tuple0*),
struct _tuple0*)) Cyc_Absyndump_dumptqtd)( _temp1409, _temp1407, Cyc_Absyndump_dumpqvar,
_temp1411); Cyc_Absyndump_dumpatts( _temp1401);} if( _temp1405 != 0){ Cyc_Absyndump_dump_char((
int)'='); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null( _temp1405));}
Cyc_Absyndump_dump_semi(); goto _LL1367; _LL1377: Cyc_Absyndump_dumpscope(
_temp1426); if( _temp1418){ Cyc_Absyndump_dump(({ unsigned char* _temp1512="xtunion ";
struct _tagged_string _temp1513; _temp1513.curr= _temp1512; _temp1513.base=
_temp1512; _temp1513.last_plus_one= _temp1512 + 9; _temp1513;}));} else{ Cyc_Absyndump_dump(({
unsigned char* _temp1514="tunion "; struct _tagged_string _temp1515; _temp1515.curr=
_temp1514; _temp1515.base= _temp1514; _temp1515.last_plus_one= _temp1514 + 8;
_temp1515;}));} Cyc_Absyndump_dumpqvar( _temp1424); Cyc_Absyndump_dumptvars(
_temp1422); if( _temp1420 == 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumptunionfields(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1420))->v); Cyc_Absyndump_dump_nospace(({ unsigned char*
_temp1516="};"; struct _tagged_string _temp1517; _temp1517.curr= _temp1516;
_temp1517.base= _temp1516; _temp1517.last_plus_one= _temp1516 + 3; _temp1517;}));}
goto _LL1367; _LL1379: Cyc_Absyndump_dumpscope( _temp1435); Cyc_Absyndump_dump(({
unsigned char* _temp1518="enum "; struct _tagged_string _temp1519; _temp1519.curr=
_temp1518; _temp1519.base= _temp1518; _temp1519.last_plus_one= _temp1518 + 6;
_temp1519;})); Cyc_Absyndump_dumpqvar( _temp1433); if( _temp1431 == 0){ Cyc_Absyndump_dump_semi();}
else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpenumfields(( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1431))->v); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp1520="};"; struct _tagged_string _temp1521; _temp1521.curr=
_temp1520; _temp1521.base= _temp1520; _temp1521.last_plus_one= _temp1520 + 3;
_temp1521;}));} return; _LL1381: Cyc_Absyndump_dump(({ unsigned char* _temp1522="let";
struct _tagged_string _temp1523; _temp1523.curr= _temp1522; _temp1523.base=
_temp1522; _temp1523.last_plus_one= _temp1522 + 4; _temp1523;})); Cyc_Absyndump_dumppat(
_temp1445); Cyc_Absyndump_dump_char(( int)'='); Cyc_Absyndump_dumpexp( _temp1439);
Cyc_Absyndump_dump_semi(); goto _LL1367; _LL1383: Cyc_Absyndump_dump(({
unsigned char* _temp1524="let "; struct _tagged_string _temp1525; _temp1525.curr=
_temp1524; _temp1525.base= _temp1524; _temp1525.last_plus_one= _temp1524 + 5;
_temp1525;})); Cyc_Absyndump_dumpids( _temp1447); Cyc_Absyndump_dump_semi();
goto _LL1367; _LL1385: if( ! Cyc_Absynpp_expand_typedefs){ Cyc_Absyndump_dump(({
unsigned char* _temp1526="typedef"; struct _tagged_string _temp1527; _temp1527.curr=
_temp1526; _temp1527.base= _temp1526; _temp1527.last_plus_one= _temp1526 + 8;
_temp1527;}));(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct Cyc_Absyn_Typedefdecl*),
struct Cyc_Absyn_Typedefdecl*)) Cyc_Absyndump_dumptqtd)(({ struct Cyc_Absyn_Tqual
_temp1528; _temp1528.q_const= 0; _temp1528.q_volatile= 0; _temp1528.q_restrict=
0; _temp1528;}),( void*) _temp1449->defn, Cyc_Absyndump_dumptypedefname,
_temp1449); Cyc_Absyndump_dump_semi();} goto _LL1367; _LL1387: Cyc_Absyndump_dump(({
unsigned char* _temp1529="namespace"; struct _tagged_string _temp1530; _temp1530.curr=
_temp1529; _temp1530.base= _temp1529; _temp1530.last_plus_one= _temp1529 + 10;
_temp1530;})); Cyc_Absyndump_dump_str( _temp1453); Cyc_Absyndump_dump_char(( int)'{');
for( 0; _temp1451 != 0; _temp1451=(( struct Cyc_List_List*) _check_null(
_temp1451))->tl){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( _temp1451))->hd);} Cyc_Absyndump_dump_char(( int)'}'); goto _LL1367;
_LL1389: Cyc_Absyndump_dump(({ unsigned char* _temp1531="using"; struct
_tagged_string _temp1532; _temp1532.curr= _temp1531; _temp1532.base= _temp1531;
_temp1532.last_plus_one= _temp1531 + 6; _temp1532;})); Cyc_Absyndump_dumpqvar(
_temp1457); Cyc_Absyndump_dump_char(( int)'{'); for( 0; _temp1455 != 0;
_temp1455=(( struct Cyc_List_List*) _check_null( _temp1455))->tl){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( _temp1455))->hd);}
Cyc_Absyndump_dump_char(( int)'}'); goto _LL1367; _LL1391: Cyc_Absyndump_dump(({
unsigned char* _temp1533="extern \"C\" {"; struct _tagged_string _temp1534;
_temp1534.curr= _temp1533; _temp1534.base= _temp1533; _temp1534.last_plus_one=
_temp1533 + 13; _temp1534;})); for( 0; _temp1459 != 0; _temp1459=(( struct Cyc_List_List*)
_check_null( _temp1459))->tl){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( _temp1459))->hd);} Cyc_Absyndump_dump_char((
int)'}'); goto _LL1367; _LL1367:;} static void Cyc_Absyndump_dump_upperbound(
struct Cyc_Absyn_Exp* e){ unsigned int i= Cyc_Evexp_eval_const_uint_exp( e); if(
i != 1){ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpexp( e); Cyc_Absyndump_dump_char((
int)'}');}} void Cyc_Absyndump_dumptms( struct Cyc_List_List* tms, void(* f)(
void*), void* a){ if( tms == 0){ f( a); return;}{ void* _temp1535=( void*)((
struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Tqual _temp1553;
void* _temp1555; void* _temp1557; struct Cyc_Absyn_Exp* _temp1559; struct Cyc_Absyn_Tqual
_temp1561; void* _temp1563; void* _temp1565; struct Cyc_Absyn_Exp* _temp1567;
struct Cyc_Absyn_Tqual _temp1569; void* _temp1571; void* _temp1573; struct Cyc_Absyn_Tqual
_temp1575; void* _temp1577; struct Cyc_Absyn_Tvar* _temp1579; void* _temp1581;
struct Cyc_Absyn_Exp* _temp1583; struct Cyc_Absyn_Tqual _temp1585; void*
_temp1587; struct Cyc_Absyn_Tvar* _temp1589; void* _temp1591; struct Cyc_Absyn_Exp*
_temp1593; struct Cyc_Absyn_Tqual _temp1595; void* _temp1597; struct Cyc_Absyn_Tvar*
_temp1599; void* _temp1601; struct Cyc_Absyn_Tqual _temp1603; void* _temp1605;
void* _temp1607; _LL1537: if(( unsigned int) _temp1535 > 1u?*(( int*) _temp1535)
== Cyc_Absyn_Pointer_mod: 0){ _LL1558: _temp1557=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1535)->f1; if(( unsigned int) _temp1557 > 1u?*(( int*) _temp1557) == Cyc_Absyn_Nullable_ps:
0){ _LL1560: _temp1559=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp1557)->f1;
goto _LL1556;} else{ goto _LL1539;} _LL1556: _temp1555=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1535)->f2; if( _temp1555 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1554;}
else{ goto _LL1539;} _LL1554: _temp1553=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1535)->f3; goto _LL1538;} else{ goto _LL1539;} _LL1539: if(( unsigned int)
_temp1535 > 1u?*(( int*) _temp1535) == Cyc_Absyn_Pointer_mod: 0){ _LL1566:
_temp1565=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1535)->f1; if((
unsigned int) _temp1565 > 1u?*(( int*) _temp1565) == Cyc_Absyn_NonNullable_ps: 0){
_LL1568: _temp1567=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1565)->f1;
goto _LL1564;} else{ goto _LL1541;} _LL1564: _temp1563=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1535)->f2; if( _temp1563 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1562;}
else{ goto _LL1541;} _LL1562: _temp1561=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1535)->f3; goto _LL1540;} else{ goto _LL1541;} _LL1541: if(( unsigned int)
_temp1535 > 1u?*(( int*) _temp1535) == Cyc_Absyn_Pointer_mod: 0){ _LL1574:
_temp1573=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1535)->f1; if(
_temp1573 ==( void*) Cyc_Absyn_TaggedArray_ps){ goto _LL1572;} else{ goto
_LL1543;} _LL1572: _temp1571=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1535)->f2; if( _temp1571 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1570;}
else{ goto _LL1543;} _LL1570: _temp1569=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1535)->f3; goto _LL1542;} else{ goto _LL1543;} _LL1543: if(( unsigned int)
_temp1535 > 1u?*(( int*) _temp1535) == Cyc_Absyn_Pointer_mod: 0){ _LL1582:
_temp1581=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1535)->f1; if((
unsigned int) _temp1581 > 1u?*(( int*) _temp1581) == Cyc_Absyn_Nullable_ps: 0){
_LL1584: _temp1583=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp1581)->f1; goto
_LL1578;} else{ goto _LL1545;} _LL1578: _temp1577=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1535)->f2; if(( unsigned int) _temp1577 > 4u?*(( int*) _temp1577) == Cyc_Absyn_VarType:
0){ _LL1580: _temp1579=(( struct Cyc_Absyn_VarType_struct*) _temp1577)->f1; goto
_LL1576;} else{ goto _LL1545;} _LL1576: _temp1575=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1535)->f3; goto _LL1544;} else{ goto _LL1545;} _LL1545: if(( unsigned int)
_temp1535 > 1u?*(( int*) _temp1535) == Cyc_Absyn_Pointer_mod: 0){ _LL1592:
_temp1591=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1535)->f1; if((
unsigned int) _temp1591 > 1u?*(( int*) _temp1591) == Cyc_Absyn_NonNullable_ps: 0){
_LL1594: _temp1593=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1591)->f1;
goto _LL1588;} else{ goto _LL1547;} _LL1588: _temp1587=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1535)->f2; if(( unsigned int) _temp1587 > 4u?*(( int*) _temp1587) == Cyc_Absyn_VarType:
0){ _LL1590: _temp1589=(( struct Cyc_Absyn_VarType_struct*) _temp1587)->f1; goto
_LL1586;} else{ goto _LL1547;} _LL1586: _temp1585=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1535)->f3; goto _LL1546;} else{ goto _LL1547;} _LL1547: if(( unsigned int)
_temp1535 > 1u?*(( int*) _temp1535) == Cyc_Absyn_Pointer_mod: 0){ _LL1602:
_temp1601=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1535)->f1; if(
_temp1601 ==( void*) Cyc_Absyn_TaggedArray_ps){ goto _LL1598;} else{ goto
_LL1549;} _LL1598: _temp1597=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1535)->f2; if(( unsigned int) _temp1597 > 4u?*(( int*) _temp1597) == Cyc_Absyn_VarType:
0){ _LL1600: _temp1599=(( struct Cyc_Absyn_VarType_struct*) _temp1597)->f1; goto
_LL1596;} else{ goto _LL1549;} _LL1596: _temp1595=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1535)->f3; goto _LL1548;} else{ goto _LL1549;} _LL1549: if(( unsigned int)
_temp1535 > 1u?*(( int*) _temp1535) == Cyc_Absyn_Pointer_mod: 0){ _LL1608:
_temp1607=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1535)->f1; goto
_LL1606; _LL1606: _temp1605=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1535)->f2; goto _LL1604; _LL1604: _temp1603=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1535)->f3; goto _LL1550;} else{ goto _LL1551;} _LL1551: goto _LL1552;
_LL1538: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dump_upperbound(
_temp1559);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f, a);
return; _LL1540: Cyc_Absyndump_dump_char(( int)'@'); Cyc_Absyndump_dump_upperbound(
_temp1567);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f, a);
return; _LL1542: Cyc_Absyndump_dump_char(( int)'?');(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*)
_check_null( tms))->tl, f, a); return; _LL1544: Cyc_Absyndump_dump_char(( int)'*');
Cyc_Absyndump_dump_upperbound( _temp1583); Cyc_Absyndump_dump_str( _temp1579->name);((
void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)(((
struct Cyc_List_List*) _check_null( tms))->tl, f, a); return; _LL1546: Cyc_Absyndump_dump_char((
int)'@'); Cyc_Absyndump_dump_upperbound( _temp1593); Cyc_Absyndump_dump_str(
_temp1589->name);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void*
a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f,
a); return; _LL1548: Cyc_Absyndump_dump_char(( int)'?'); Cyc_Absyndump_dump_str(
_temp1599->name);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void*
a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f,
a); return; _LL1550:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1609=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1609[ 0]=({ struct Cyc_Core_Impossible_struct _temp1610; _temp1610.tag= Cyc_Core_Impossible;
_temp1610.f1=({ unsigned char* _temp1611="dumptms: bad Pointer_mod"; struct
_tagged_string _temp1612; _temp1612.curr= _temp1611; _temp1612.base= _temp1611;
_temp1612.last_plus_one= _temp1611 + 25; _temp1612;}); _temp1610;}); _temp1609;}));
_LL1552: { int next_is_pointer= 0; if((( struct Cyc_List_List*) _check_null( tms))->tl
!= 0){ void* _temp1613=( void*)(( struct Cyc_List_List*) _check_null((( struct
Cyc_List_List*) _check_null( tms))->tl))->hd; struct Cyc_Absyn_Tqual _temp1619;
void* _temp1621; void* _temp1623; _LL1615: if(( unsigned int) _temp1613 > 1u?*((
int*) _temp1613) == Cyc_Absyn_Pointer_mod: 0){ _LL1624: _temp1623=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1613)->f1; goto _LL1622; _LL1622:
_temp1621=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1613)->f2; goto
_LL1620; _LL1620: _temp1619=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1613)->f3;
goto _LL1616;} else{ goto _LL1617;} _LL1617: goto _LL1618; _LL1616:
next_is_pointer= 1; goto _LL1614; _LL1618: goto _LL1614; _LL1614:;} if(
next_is_pointer){ Cyc_Absyndump_dump_char(( int)'(');}(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*)
_check_null( tms))->tl, f, a); if( next_is_pointer){ Cyc_Absyndump_dump_char((
int)')');}{ void* _temp1625=( void*)(( struct Cyc_List_List*) _check_null( tms))->hd;
struct Cyc_Absyn_Exp* _temp1641; void* _temp1643; struct Cyc_Core_Opt* _temp1645;
struct Cyc_Absyn_VarargInfo* _temp1647; int _temp1649; struct Cyc_List_List*
_temp1651; void* _temp1653; struct Cyc_Position_Segment* _temp1655; struct Cyc_List_List*
_temp1657; int _temp1659; struct Cyc_Position_Segment* _temp1661; struct Cyc_List_List*
_temp1663; struct Cyc_List_List* _temp1665; struct Cyc_Position_Segment*
_temp1667; struct Cyc_Absyn_Tqual _temp1669; void* _temp1671; void* _temp1673;
_LL1627: if( _temp1625 ==( void*) Cyc_Absyn_Carray_mod){ goto _LL1628;} else{
goto _LL1629;} _LL1629: if(( unsigned int) _temp1625 > 1u?*(( int*) _temp1625)
== Cyc_Absyn_ConstArray_mod: 0){ _LL1642: _temp1641=(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp1625)->f1; goto _LL1630;} else{ goto _LL1631;} _LL1631: if(( unsigned int)
_temp1625 > 1u?*(( int*) _temp1625) == Cyc_Absyn_Function_mod: 0){ _LL1644:
_temp1643=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp1625)->f1; if(*((
int*) _temp1643) == Cyc_Absyn_WithTypes){ _LL1652: _temp1651=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1643)->f1; goto _LL1650; _LL1650: _temp1649=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1643)->f2; goto _LL1648; _LL1648: _temp1647=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1643)->f3; goto _LL1646; _LL1646: _temp1645=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1643)->f4; goto _LL1632;} else{ goto _LL1633;}} else{ goto _LL1633;}
_LL1633: if(( unsigned int) _temp1625 > 1u?*(( int*) _temp1625) == Cyc_Absyn_Function_mod:
0){ _LL1654: _temp1653=( void*)(( struct Cyc_Absyn_Function_mod_struct*)
_temp1625)->f1; if(*(( int*) _temp1653) == Cyc_Absyn_NoTypes){ _LL1658:
_temp1657=(( struct Cyc_Absyn_NoTypes_struct*) _temp1653)->f1; goto _LL1656;
_LL1656: _temp1655=(( struct Cyc_Absyn_NoTypes_struct*) _temp1653)->f2; goto
_LL1634;} else{ goto _LL1635;}} else{ goto _LL1635;} _LL1635: if(( unsigned int)
_temp1625 > 1u?*(( int*) _temp1625) == Cyc_Absyn_TypeParams_mod: 0){ _LL1664:
_temp1663=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp1625)->f1; goto
_LL1662; _LL1662: _temp1661=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1625)->f2; goto _LL1660; _LL1660: _temp1659=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1625)->f3; goto _LL1636;} else{ goto _LL1637;} _LL1637: if(( unsigned int)
_temp1625 > 1u?*(( int*) _temp1625) == Cyc_Absyn_Attributes_mod: 0){ _LL1668:
_temp1667=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp1625)->f1; goto
_LL1666; _LL1666: _temp1665=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp1625)->f2; goto _LL1638;} else{ goto _LL1639;} _LL1639: if(( unsigned int)
_temp1625 > 1u?*(( int*) _temp1625) == Cyc_Absyn_Pointer_mod: 0){ _LL1674:
_temp1673=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1625)->f1; goto
_LL1672; _LL1672: _temp1671=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1625)->f2; goto _LL1670; _LL1670: _temp1669=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1625)->f3; goto _LL1640;} else{ goto _LL1626;} _LL1628: Cyc_Absyndump_dump(({
unsigned char* _temp1675="[]"; struct _tagged_string _temp1676; _temp1676.curr=
_temp1675; _temp1676.base= _temp1675; _temp1676.last_plus_one= _temp1675 + 3;
_temp1676;})); goto _LL1626; _LL1630: Cyc_Absyndump_dump_char(( int)'['); Cyc_Absyndump_dumpexp(
_temp1641); Cyc_Absyndump_dump_char(( int)']'); goto _LL1626; _LL1632: Cyc_Absyndump_dumpfunargs(
_temp1651, _temp1649, _temp1647, _temp1645); goto _LL1626; _LL1634:(( void(*)(
void(* f)( struct _tagged_string*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dump_str, _temp1657,({ unsigned char* _temp1677="("; struct
_tagged_string _temp1678; _temp1678.curr= _temp1677; _temp1678.base= _temp1677;
_temp1678.last_plus_one= _temp1677 + 2; _temp1678;}),({ unsigned char* _temp1679=")";
struct _tagged_string _temp1680; _temp1680.curr= _temp1679; _temp1680.base=
_temp1679; _temp1680.last_plus_one= _temp1679 + 2; _temp1680;}),({ unsigned char*
_temp1681=","; struct _tagged_string _temp1682; _temp1682.curr= _temp1681;
_temp1682.base= _temp1681; _temp1682.last_plus_one= _temp1681 + 2; _temp1682;}));
goto _LL1626; _LL1636: if( _temp1659){ Cyc_Absyndump_dumpkindedtvars( _temp1663);}
else{ Cyc_Absyndump_dumptvars( _temp1663);} goto _LL1626; _LL1638: Cyc_Absyndump_dumpatts(
_temp1665); goto _LL1626; _LL1640:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1683=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1683[ 0]=({ struct Cyc_Core_Impossible_struct _temp1684; _temp1684.tag= Cyc_Core_Impossible;
_temp1684.f1=({ unsigned char* _temp1685="dumptms"; struct _tagged_string
_temp1686; _temp1686.curr= _temp1685; _temp1686.base= _temp1685; _temp1686.last_plus_one=
_temp1685 + 8; _temp1686;}); _temp1684;}); _temp1683;})); _LL1626:;} return;}
_LL1536:;}} void Cyc_Absyndump_dumptqtd( struct Cyc_Absyn_Tqual tq, void* t,
void(* f)( void*), void* a){ struct Cyc_List_List* _temp1689; void* _temp1691;
struct Cyc_Absyn_Tqual _temp1693; struct _tuple4 _temp1687= Cyc_Absynpp_to_tms(
tq, t); _LL1694: _temp1693= _temp1687.f1; goto _LL1692; _LL1692: _temp1691=
_temp1687.f2; goto _LL1690; _LL1690: _temp1689= _temp1687.f3; goto _LL1688;
_LL1688: Cyc_Absyndump_dumptq( _temp1693); Cyc_Absyndump_dumpntyp( _temp1691);((
void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1689),
f, a);} void Cyc_Absyndump_dumpdecllist2file( struct Cyc_List_List* tdl, struct
Cyc_Stdio___sFILE* f){ Cyc_Absyndump_pos= 0;* Cyc_Absyndump_dump_file= f; for( 0;
tdl != 0; tdl=(( struct Cyc_List_List*) _check_null( tdl))->tl){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( tdl))->hd);}}