 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern struct _tagged_string
Cyc_Core_string_of_int( int); extern unsigned char* string_to_Cstring( struct
_tagged_string); extern unsigned char* underlying_Cstring( struct _tagged_string);
extern struct _tagged_string Cstring_to_string( unsigned char*); struct
_tagged_ptr0{ struct _tagged_string* curr; struct _tagged_string* base; struct
_tagged_string* last_plus_one; } ; extern struct _tagged_ptr0 ntCsl_to_ntsl(
unsigned char**); extern int system( unsigned char*); extern int* __errno();
struct Cyc_Stdio___sFILE; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stdout;
extern int Cyc_Stdio_fputc( int, struct Cyc_Stdio___sFILE*); extern
unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
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
extern unsigned char Cyc_Absyn_EmptyAnnot[ 15u]; extern struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x); extern int Cyc_Absyn_is_format_prim(
void* p); extern struct _tagged_string Cyc_Absyn_attribute2string( void*);
struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{
int expand_typedefs: 1; int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1;
int decls_first: 1; int rewrite_temp_tvars: 1; int print_all_tvars: 1; int
print_all_kinds: 1; int print_using_stmts: 1; int print_externC_stmts: 1; int
print_full_evars: 1; int use_curr_namespace: 1; struct Cyc_List_List*
curr_namespace; } ; extern void Cyc_Absynpp_set_params( struct Cyc_Absynpp_Params*
fs); extern struct _tagged_string* Cyc_Absynpp_cyc_stringptr; extern int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*); extern struct _tagged_string Cyc_Absynpp_char_escape(
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
void* t); static int Cyc_Absyndump_expand_typedefs; static int Cyc_Absyndump_qvar_to_Cids;
static int Cyc_Absyndump_add_cyc_prefix; static int Cyc_Absyndump_to_VC; void
Cyc_Absyndump_set_params( struct Cyc_Absynpp_Params* fs){ Cyc_Absyndump_expand_typedefs=
fs->expand_typedefs; Cyc_Absyndump_qvar_to_Cids= fs->qvar_to_Cids; Cyc_Absyndump_add_cyc_prefix=
fs->add_cyc_prefix; Cyc_Absyndump_to_VC= fs->to_VC; Cyc_Absynpp_set_params( fs);}
void Cyc_Absyndump_dumptyp( void*); void Cyc_Absyndump_dumpntyp( void* t); void
Cyc_Absyndump_dumpexp( struct Cyc_Absyn_Exp*); void Cyc_Absyndump_dumpexp_prec(
int, struct Cyc_Absyn_Exp*); void Cyc_Absyndump_dumppat( struct Cyc_Absyn_Pat*);
void Cyc_Absyndump_dumpstmt( struct Cyc_Absyn_Stmt*); void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl*); void Cyc_Absyndump_dumptms( struct Cyc_List_List* tms,
void(* f)( void*), void* a); void Cyc_Absyndump_dumptqtd( struct Cyc_Absyn_Tqual,
void*, void(* f)( void*), void*); void Cyc_Absyndump_dumpstructfields( struct
Cyc_List_List* fields); struct Cyc_Stdio___sFILE** Cyc_Absyndump_dump_file=& Cyc_Stdio_stdout;
void Cyc_Absyndump_ignore( void* x){ return;} static unsigned int Cyc_Absyndump_pos=
0; void Cyc_Absyndump_dump( struct _tagged_string s){ int sz=( int)({ struct
_tagged_string _temp0= s;( unsigned int)( _temp0.last_plus_one - _temp0.curr);});
if( !(( int)*(( const unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char), sz - 1))){
-- sz;} Cyc_Absyndump_pos += sz + 1; if( Cyc_Absyndump_pos > 80){ Cyc_Absyndump_pos=(
unsigned int) sz; Cyc_Stdio_fputc(( int)'\n',* Cyc_Absyndump_dump_file);} else{
Cyc_Stdio_fputc(( int)' ',* Cyc_Absyndump_dump_file);} Cyc_Stdio_file_string_write(*
Cyc_Absyndump_dump_file, s, 0, sz);} void Cyc_Absyndump_dump_nospace( struct
_tagged_string s){ int sz=( int)({ struct _tagged_string _temp1= s;(
unsigned int)( _temp1.last_plus_one - _temp1.curr);}); if( !(( int)*(( const
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
Cyc_List_List* _temp12; struct Cyc_List_List* _temp14; _LL6: if( _temp4 ==( void*)
Cyc_Absyn_Loc_n){ goto _LL7;} else{ goto _LL8;} _LL8: if(( unsigned int) _temp4
> 1u?*(( int*) _temp4) == Cyc_Absyn_Rel_n: 0){ _LL13: _temp12=(( struct Cyc_Absyn_Rel_n_struct*)
_temp4)->f1; goto _LL9;} else{ goto _LL10;} _LL10: if(( unsigned int) _temp4 > 1u?*((
int*) _temp4) == Cyc_Absyn_Abs_n: 0){ _LL15: _temp14=(( struct Cyc_Absyn_Abs_n_struct*)
_temp4)->f1; goto _LL11;} else{ goto _LL5;} _LL7: _temp12= 0; goto _LL9; _LL9:
_temp2= _temp12; goto _LL5; _LL11: _temp2=( Cyc_Absyndump_qvar_to_Cids? Cyc_Absyndump_add_cyc_prefix:
0)?({ struct Cyc_List_List* _temp16=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp16->hd=( void*) Cyc_Absynpp_cyc_stringptr; _temp16->tl=
_temp14; _temp16;}): _temp14; goto _LL5; _LL5:;} if( _temp2 != 0){ Cyc_Absyndump_dump_str((
struct _tagged_string*)(( struct Cyc_List_List*) _check_null( _temp2))->hd);
for( _temp2=(( struct Cyc_List_List*) _check_null( _temp2))->tl; _temp2 != 0;
_temp2=(( struct Cyc_List_List*) _check_null( _temp2))->tl){ if( Cyc_Absyndump_qvar_to_Cids){
Cyc_Absyndump_dump_char(( int)'_');} else{ Cyc_Absyndump_dump_nospace(({
unsigned char* _temp17=( unsigned char*)"::"; struct _tagged_string _temp18;
_temp18.curr= _temp17; _temp18.base= _temp17; _temp18.last_plus_one= _temp17 + 3;
_temp18;}));} Cyc_Absyndump_dump_nospace(*(( struct _tagged_string*)(( struct
Cyc_List_List*) _check_null( _temp2))->hd));} if( Cyc_Absyndump_qvar_to_Cids){
Cyc_Absyndump_dump_nospace(({ unsigned char* _temp19=( unsigned char*)"_";
struct _tagged_string _temp20; _temp20.curr= _temp19; _temp20.base= _temp19;
_temp20.last_plus_one= _temp19 + 2; _temp20;}));} else{ Cyc_Absyndump_dump_nospace(({
unsigned char* _temp21=( unsigned char*)"::"; struct _tagged_string _temp22;
_temp22.curr= _temp21; _temp22.base= _temp21; _temp22.last_plus_one= _temp21 + 3;
_temp22;}));} Cyc_Absyndump_dump_nospace(*(* v).f2);} else{ Cyc_Absyndump_dump_str((*
v).f2);}} void Cyc_Absyndump_dumptq( struct Cyc_Absyn_Tqual tq){ if( tq.q_restrict){
Cyc_Absyndump_dump(({ unsigned char* _temp23=( unsigned char*)"restrict"; struct
_tagged_string _temp24; _temp24.curr= _temp23; _temp24.base= _temp23; _temp24.last_plus_one=
_temp23 + 9; _temp24;}));} if( tq.q_volatile){ Cyc_Absyndump_dump(({
unsigned char* _temp25=( unsigned char*)"volatile"; struct _tagged_string
_temp26; _temp26.curr= _temp25; _temp26.base= _temp25; _temp26.last_plus_one=
_temp25 + 9; _temp26;}));} if( tq.q_const){ Cyc_Absyndump_dump(({ unsigned char*
_temp27=( unsigned char*)"const"; struct _tagged_string _temp28; _temp28.curr=
_temp27; _temp28.base= _temp27; _temp28.last_plus_one= _temp27 + 6; _temp28;}));}}
void Cyc_Absyndump_dumpscope( void* sc){ void* _temp29= sc; _LL31: if( _temp29
==( void*) Cyc_Absyn_Static){ goto _LL32;} else{ goto _LL33;} _LL33: if( _temp29
==( void*) Cyc_Absyn_Public){ goto _LL34;} else{ goto _LL35;} _LL35: if( _temp29
==( void*) Cyc_Absyn_Extern){ goto _LL36;} else{ goto _LL37;} _LL37: if( _temp29
==( void*) Cyc_Absyn_ExternC){ goto _LL38;} else{ goto _LL39;} _LL39: if(
_temp29 ==( void*) Cyc_Absyn_Abstract){ goto _LL40;} else{ goto _LL30;} _LL32:
Cyc_Absyndump_dump(({ unsigned char* _temp41=( unsigned char*)"static"; struct
_tagged_string _temp42; _temp42.curr= _temp41; _temp42.base= _temp41; _temp42.last_plus_one=
_temp41 + 7; _temp42;})); return; _LL34: return; _LL36: Cyc_Absyndump_dump(({
unsigned char* _temp43=( unsigned char*)"extern"; struct _tagged_string _temp44;
_temp44.curr= _temp43; _temp44.base= _temp43; _temp44.last_plus_one= _temp43 + 7;
_temp44;})); return; _LL38: Cyc_Absyndump_dump(({ unsigned char* _temp45=(
unsigned char*)"extern \"C\""; struct _tagged_string _temp46; _temp46.curr=
_temp45; _temp46.base= _temp45; _temp46.last_plus_one= _temp45 + 11; _temp46;}));
return; _LL40: Cyc_Absyndump_dump(({ unsigned char* _temp47=( unsigned char*)"abstract";
struct _tagged_string _temp48; _temp48.curr= _temp47; _temp48.base= _temp47;
_temp48.last_plus_one= _temp47 + 9; _temp48;})); return; _LL30:;} void Cyc_Absyndump_dumpkind(
void* k){ void* _temp49= k; _LL51: if( _temp49 ==( void*) Cyc_Absyn_AnyKind){
goto _LL52;} else{ goto _LL53;} _LL53: if( _temp49 ==( void*) Cyc_Absyn_MemKind){
goto _LL54;} else{ goto _LL55;} _LL55: if( _temp49 ==( void*) Cyc_Absyn_BoxKind){
goto _LL56;} else{ goto _LL57;} _LL57: if( _temp49 ==( void*) Cyc_Absyn_RgnKind){
goto _LL58;} else{ goto _LL59;} _LL59: if( _temp49 ==( void*) Cyc_Absyn_EffKind){
goto _LL60;} else{ goto _LL50;} _LL52: Cyc_Absyndump_dump(({ unsigned char*
_temp61=( unsigned char*)"A"; struct _tagged_string _temp62; _temp62.curr=
_temp61; _temp62.base= _temp61; _temp62.last_plus_one= _temp61 + 2; _temp62;}));
return; _LL54: Cyc_Absyndump_dump(({ unsigned char* _temp63=( unsigned char*)"M";
struct _tagged_string _temp64; _temp64.curr= _temp63; _temp64.base= _temp63;
_temp64.last_plus_one= _temp63 + 2; _temp64;})); return; _LL56: Cyc_Absyndump_dump(({
unsigned char* _temp65=( unsigned char*)"B"; struct _tagged_string _temp66;
_temp66.curr= _temp65; _temp66.base= _temp65; _temp66.last_plus_one= _temp65 + 2;
_temp66;})); return; _LL58: Cyc_Absyndump_dump(({ unsigned char* _temp67=(
unsigned char*)"R"; struct _tagged_string _temp68; _temp68.curr= _temp67;
_temp68.base= _temp67; _temp68.last_plus_one= _temp67 + 2; _temp68;})); return;
_LL60: Cyc_Absyndump_dump(({ unsigned char* _temp69=( unsigned char*)"E"; struct
_tagged_string _temp70; _temp70.curr= _temp69; _temp70.base= _temp69; _temp70.last_plus_one=
_temp69 + 2; _temp70;})); return; _LL50:;} void Cyc_Absyndump_dumptps( struct
Cyc_List_List* ts){(( void(*)( void(* f)( void*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumptyp, ts,({ unsigned char* _temp71=(
unsigned char*)"<"; struct _tagged_string _temp72; _temp72.curr= _temp71;
_temp72.base= _temp71; _temp72.last_plus_one= _temp71 + 2; _temp72;}),({
unsigned char* _temp73=( unsigned char*)">"; struct _tagged_string _temp74;
_temp74.curr= _temp73; _temp74.base= _temp73; _temp74.last_plus_one= _temp73 + 2;
_temp74;}),({ unsigned char* _temp75=( unsigned char*)","; struct _tagged_string
_temp76; _temp76.curr= _temp75; _temp76.base= _temp75; _temp76.last_plus_one=
_temp75 + 2; _temp76;}));} void Cyc_Absyndump_dumptvar( struct Cyc_Absyn_Tvar*
tv){ Cyc_Absyndump_dump_str( tv->name);} void Cyc_Absyndump_dumpkindedtvar(
struct Cyc_Absyn_Tvar* tv){ Cyc_Absyndump_dump_str( tv->name);{ void* _temp77=(
void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
tv->kind))->v; void* _temp85; void* _temp87; _LL79: if(( unsigned int) _temp77 >
1u?*(( int*) _temp77) == Cyc_Absyn_Eq_constr: 0){ _LL86: _temp85=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp77)->f1; if( _temp85 ==( void*) Cyc_Absyn_BoxKind){
goto _LL80;} else{ goto _LL81;}} else{ goto _LL81;} _LL81: if(( unsigned int)
_temp77 > 1u?*(( int*) _temp77) == Cyc_Absyn_Eq_constr: 0){ _LL88: _temp87=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp77)->f1; goto _LL82;} else{
goto _LL83;} _LL83: goto _LL84; _LL80: goto _LL78; _LL82: Cyc_Absyndump_dump(({
unsigned char* _temp89=( unsigned char*)"::"; struct _tagged_string _temp90;
_temp90.curr= _temp89; _temp90.base= _temp89; _temp90.last_plus_one= _temp89 + 3;
_temp90;})); Cyc_Absyndump_dumpkind( _temp87); goto _LL78; _LL84: Cyc_Absyndump_dump(({
unsigned char* _temp91=( unsigned char*)"::?"; struct _tagged_string _temp92;
_temp92.curr= _temp91; _temp92.base= _temp91; _temp92.last_plus_one= _temp91 + 4;
_temp92;})); goto _LL78; _LL78:;}} void Cyc_Absyndump_dumptvars( struct Cyc_List_List*
tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumptvar, tvs,({ unsigned char*
_temp93=( unsigned char*)"<"; struct _tagged_string _temp94; _temp94.curr=
_temp93; _temp94.base= _temp93; _temp94.last_plus_one= _temp93 + 2; _temp94;}),({
unsigned char* _temp95=( unsigned char*)">"; struct _tagged_string _temp96;
_temp96.curr= _temp95; _temp96.base= _temp95; _temp96.last_plus_one= _temp95 + 2;
_temp96;}),({ unsigned char* _temp97=( unsigned char*)","; struct _tagged_string
_temp98; _temp98.curr= _temp97; _temp98.base= _temp97; _temp98.last_plus_one=
_temp97 + 2; _temp98;}));} void Cyc_Absyndump_dumpkindedtvars( struct Cyc_List_List*
tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumpkindedtvar, tvs,({ unsigned char*
_temp99=( unsigned char*)"<"; struct _tagged_string _temp100; _temp100.curr=
_temp99; _temp100.base= _temp99; _temp100.last_plus_one= _temp99 + 2; _temp100;}),({
unsigned char* _temp101=( unsigned char*)">"; struct _tagged_string _temp102;
_temp102.curr= _temp101; _temp102.base= _temp101; _temp102.last_plus_one=
_temp101 + 2; _temp102;}),({ unsigned char* _temp103=( unsigned char*)",";
struct _tagged_string _temp104; _temp104.curr= _temp103; _temp104.base= _temp103;
_temp104.last_plus_one= _temp103 + 2; _temp104;}));} struct _tuple5{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; void Cyc_Absyndump_dumparg( struct _tuple5* pr){(( void(*)(
struct Cyc_Absyn_Tqual, void*, void(* f)( int), int)) Cyc_Absyndump_dumptqtd)((*
pr).f1,(* pr).f2,( void(*)( int x)) Cyc_Absyndump_ignore, 0);} void Cyc_Absyndump_dumpargs(
struct Cyc_List_List* ts){(( void(*)( void(* f)( struct _tuple5*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumparg, ts,({ unsigned char* _temp105=(
unsigned char*)"("; struct _tagged_string _temp106; _temp106.curr= _temp105;
_temp106.base= _temp105; _temp106.last_plus_one= _temp105 + 2; _temp106;}),({
unsigned char* _temp107=( unsigned char*)")"; struct _tagged_string _temp108;
_temp108.curr= _temp107; _temp108.base= _temp107; _temp108.last_plus_one=
_temp107 + 2; _temp108;}),({ unsigned char* _temp109=( unsigned char*)",";
struct _tagged_string _temp110; _temp110.curr= _temp109; _temp110.base= _temp109;
_temp110.last_plus_one= _temp109 + 2; _temp110;}));} void Cyc_Absyndump_dump_callconv(
struct Cyc_List_List* atts){ for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp111=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; _LL113: if( _temp111 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL114;} else{ goto _LL115;} _LL115: if( _temp111 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL116;} else{ goto _LL117;} _LL117: if( _temp111 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL118;} else{ goto _LL119;} _LL119: goto _LL120; _LL114: Cyc_Absyndump_dump(({
unsigned char* _temp121=( unsigned char*)"_stdcall"; struct _tagged_string
_temp122; _temp122.curr= _temp121; _temp122.base= _temp121; _temp122.last_plus_one=
_temp121 + 9; _temp122;})); return; _LL116: Cyc_Absyndump_dump(({ unsigned char*
_temp123=( unsigned char*)"_cdecl"; struct _tagged_string _temp124; _temp124.curr=
_temp123; _temp124.base= _temp123; _temp124.last_plus_one= _temp123 + 7;
_temp124;})); return; _LL118: Cyc_Absyndump_dump(({ unsigned char* _temp125=(
unsigned char*)"_fastcall"; struct _tagged_string _temp126; _temp126.curr=
_temp125; _temp126.base= _temp125; _temp126.last_plus_one= _temp125 + 10;
_temp126;})); return; _LL120: goto _LL112; _LL112:;}} void Cyc_Absyndump_dump_noncallconv(
struct Cyc_List_List* atts){ int hasatt= 0;{ struct Cyc_List_List* atts2= atts;
for( 0; atts2 != 0; atts2=(( struct Cyc_List_List*) _check_null( atts2))->tl){
void* _temp127=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd; _LL129:
if( _temp127 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL130;} else{ goto _LL131;}
_LL131: if( _temp127 ==( void*) Cyc_Absyn_Cdecl_att){ goto _LL132;} else{ goto
_LL133;} _LL133: if( _temp127 ==( void*) Cyc_Absyn_Fastcall_att){ goto _LL134;}
else{ goto _LL135;} _LL135: goto _LL136; _LL130: goto _LL128; _LL132: goto
_LL128; _LL134: goto _LL128; _LL136: hasatt= 1; goto _LL128; _LL128:;}} if( !
hasatt){ return;} Cyc_Absyndump_dump(({ unsigned char* _temp137=( unsigned char*)"__declspec(";
struct _tagged_string _temp138; _temp138.curr= _temp137; _temp138.base= _temp137;
_temp138.last_plus_one= _temp137 + 12; _temp138;})); for( 0; atts != 0; atts=((
struct Cyc_List_List*) _check_null( atts))->tl){ void* _temp139=( void*)((
struct Cyc_List_List*) _check_null( atts))->hd; _LL141: if( _temp139 ==( void*)
Cyc_Absyn_Stdcall_att){ goto _LL142;} else{ goto _LL143;} _LL143: if( _temp139
==( void*) Cyc_Absyn_Cdecl_att){ goto _LL144;} else{ goto _LL145;} _LL145: if(
_temp139 ==( void*) Cyc_Absyn_Fastcall_att){ goto _LL146;} else{ goto _LL147;}
_LL147: goto _LL148; _LL142: goto _LL140; _LL144: goto _LL140; _LL146: goto
_LL140; _LL148: Cyc_Absyndump_dump( Cyc_Absyn_attribute2string(( void*)(( struct
Cyc_List_List*) _check_null( atts))->hd)); goto _LL140; _LL140:;} Cyc_Absyndump_dump_char((
int)')');} void Cyc_Absyndump_dumpatts( struct Cyc_List_List* atts){ if( atts ==
0){ return;} if( Cyc_Absyndump_to_VC){ Cyc_Absyndump_dump_noncallconv( atts);
return;} Cyc_Absyndump_dump(({ unsigned char* _temp149=( unsigned char*)" __attribute__((";
struct _tagged_string _temp150; _temp150.curr= _temp149; _temp150.base= _temp149;
_temp150.last_plus_one= _temp149 + 17; _temp150;})); for( 0; atts != 0; atts=((
struct Cyc_List_List*) _check_null( atts))->tl){ Cyc_Absyndump_dump( Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd)); if((( struct Cyc_List_List*)
_check_null( atts))->tl != 0){ Cyc_Absyndump_dump(({ unsigned char* _temp151=(
unsigned char*)","; struct _tagged_string _temp152; _temp152.curr= _temp151;
_temp152.base= _temp151; _temp152.last_plus_one= _temp151 + 2; _temp152;}));}}
Cyc_Absyndump_dump(({ unsigned char* _temp153=( unsigned char*)")) "; struct
_tagged_string _temp154; _temp154.curr= _temp153; _temp154.base= _temp153;
_temp154.last_plus_one= _temp153 + 4; _temp154;}));} int Cyc_Absyndump_next_is_pointer(
struct Cyc_List_List* tms){ if( tms == 0){ return 0;}{ void* _temp155=( void*)((
struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Tqual _temp161;
void* _temp163; void* _temp165; _LL157: if(( unsigned int) _temp155 > 1u?*(( int*)
_temp155) == Cyc_Absyn_Pointer_mod: 0){ _LL166: _temp165=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp155)->f1; goto _LL164; _LL164: _temp163=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp155)->f2; goto _LL162; _LL162: _temp161=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp155)->f3; goto _LL158;} else{ goto _LL159;} _LL159: goto _LL160; _LL158:
return 1; _LL160: return 0; _LL156:;}} static void Cyc_Absyndump_dumprgn( void*
t){ void* _temp167= Cyc_Tcutil_compress( t); _LL169: if( _temp167 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL170;} else{ goto _LL171;} _LL171: goto _LL172; _LL170: Cyc_Absyndump_dump(({
unsigned char* _temp173=( unsigned char*)"`H"; struct _tagged_string _temp174;
_temp174.curr= _temp173; _temp174.base= _temp173; _temp174.last_plus_one=
_temp173 + 3; _temp174;})); goto _LL168; _LL172: Cyc_Absyndump_dumpntyp( t);
goto _LL168; _LL168:;} struct _tuple6{ struct Cyc_List_List* f1; struct Cyc_List_List*
f2; } ; static struct _tuple6 Cyc_Absyndump_effects_split( void* t){ struct Cyc_List_List*
regions= 0; struct Cyc_List_List* effects= 0;{ void* _temp175= Cyc_Tcutil_compress(
t); void* _temp183; struct Cyc_List_List* _temp185; _LL177: if(( unsigned int)
_temp175 > 4u?*(( int*) _temp175) == Cyc_Absyn_AccessEff: 0){ _LL184: _temp183=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp175)->f1; goto _LL178;} else{
goto _LL179;} _LL179: if(( unsigned int) _temp175 > 4u?*(( int*) _temp175) ==
Cyc_Absyn_JoinEff: 0){ _LL186: _temp185=(( struct Cyc_Absyn_JoinEff_struct*)
_temp175)->f1; goto _LL180;} else{ goto _LL181;} _LL181: goto _LL182; _LL178:
regions=({ struct Cyc_List_List* _temp187=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp187->hd=( void*) _temp183; _temp187->tl=
regions; _temp187;}); goto _LL176; _LL180: for( 0; _temp185 != 0; _temp185=((
struct Cyc_List_List*) _check_null( _temp185))->tl){ struct Cyc_List_List*
_temp190; struct Cyc_List_List* _temp192; struct _tuple6 _temp188= Cyc_Absyndump_effects_split((
void*)(( struct Cyc_List_List*) _check_null( _temp185))->hd); _LL193: _temp192=
_temp188.f1; goto _LL191; _LL191: _temp190= _temp188.f2; goto _LL189; _LL189:
regions=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( _temp192, regions); effects=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp190, effects);} goto _LL176; _LL182: effects=({ struct Cyc_List_List*
_temp194=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp194->hd=( void*) t; _temp194->tl= effects; _temp194;}); goto _LL176; _LL176:;}
return({ struct _tuple6 _temp195; _temp195.f1= regions; _temp195.f2= effects;
_temp195;});} static void Cyc_Absyndump_dumpeff( void* t){ struct Cyc_List_List*
_temp198; struct Cyc_List_List* _temp200; struct _tuple6 _temp196= Cyc_Absyndump_effects_split(
t); _LL201: _temp200= _temp196.f1; goto _LL199; _LL199: _temp198= _temp196.f2;
goto _LL197; _LL197: _temp200=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp200); _temp198=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( _temp198); for( 0; _temp198 != 0; _temp198=((
struct Cyc_List_List*) _check_null( _temp198))->tl){ Cyc_Absyndump_dumpntyp((
void*)(( struct Cyc_List_List*) _check_null( _temp198))->hd); Cyc_Absyndump_dump_char((
int)'+');} Cyc_Absyndump_dump_char(( int)'{'); for( 0; _temp200 != 0; _temp200=((
struct Cyc_List_List*) _check_null( _temp200))->tl){ Cyc_Absyndump_dumprgn((
void*)(( struct Cyc_List_List*) _check_null( _temp200))->hd); if((( struct Cyc_List_List*)
_check_null( _temp200))->tl != 0){ Cyc_Absyndump_dump_char(( int)',');}} Cyc_Absyndump_dump_char((
int)'}');} void Cyc_Absyndump_dumpntyp( void* t){ void* _temp202= t; struct Cyc_Absyn_Exp*
_temp272; struct Cyc_Absyn_Tqual _temp274; void* _temp276; struct Cyc_Absyn_FnInfo
_temp278; struct Cyc_Absyn_PtrInfo _temp280; struct Cyc_Absyn_Tvar* _temp282;
struct Cyc_Core_Opt* _temp284; int _temp286; struct Cyc_Core_Opt* _temp288;
struct Cyc_Core_Opt* _temp290; struct Cyc_Core_Opt* _temp292; int _temp294;
struct Cyc_Core_Opt* _temp296; struct Cyc_Core_Opt _temp298; void* _temp299;
struct Cyc_Core_Opt* _temp301; struct Cyc_Absyn_TunionInfo _temp303; void*
_temp305; struct Cyc_List_List* _temp307; void* _temp309; struct Cyc_Absyn_TunionFieldInfo
_temp311; struct Cyc_List_List* _temp313; void* _temp315; struct Cyc_Absyn_Enumdecl*
_temp317; struct _tuple0* _temp319; void* _temp321; void* _temp323; void*
_temp325; void* _temp327; void* _temp329; void* _temp331; void* _temp333; void*
_temp335; void* _temp337; void* _temp339; void* _temp341; void* _temp343; void*
_temp346; void* _temp348; void* _temp350; void* _temp352; void* _temp355; void*
_temp357; void* _temp359; void* _temp361; struct Cyc_List_List* _temp363; struct
Cyc_Absyn_Structdecl** _temp365; struct Cyc_List_List* _temp367; struct _tuple0*
_temp369; struct Cyc_Absyn_Structdecl** _temp371; struct Cyc_List_List* _temp373;
struct _tuple0* _temp375; struct Cyc_Absyn_Uniondecl** _temp377; struct Cyc_List_List*
_temp379; struct _tuple0* _temp381; struct Cyc_Absyn_Uniondecl** _temp383;
struct Cyc_List_List* _temp385; struct _tuple0* _temp387; struct Cyc_List_List*
_temp389; struct Cyc_List_List* _temp391; struct Cyc_Core_Opt* _temp393; struct
Cyc_List_List* _temp395; struct _tuple0* _temp397; void* _temp399; void*
_temp401; struct Cyc_List_List* _temp403; _LL204: if(( unsigned int) _temp202 >
4u?*(( int*) _temp202) == Cyc_Absyn_ArrayType: 0){ _LL277: _temp276=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp202)->f1; goto _LL275; _LL275: _temp274=((
struct Cyc_Absyn_ArrayType_struct*) _temp202)->f2; goto _LL273; _LL273: _temp272=((
struct Cyc_Absyn_ArrayType_struct*) _temp202)->f3; goto _LL205;} else{ goto
_LL206;} _LL206: if(( unsigned int) _temp202 > 4u?*(( int*) _temp202) == Cyc_Absyn_FnType:
0){ _LL279: _temp278=(( struct Cyc_Absyn_FnType_struct*) _temp202)->f1; goto
_LL207;} else{ goto _LL208;} _LL208: if(( unsigned int) _temp202 > 4u?*(( int*)
_temp202) == Cyc_Absyn_PointerType: 0){ _LL281: _temp280=(( struct Cyc_Absyn_PointerType_struct*)
_temp202)->f1; goto _LL209;} else{ goto _LL210;} _LL210: if( _temp202 ==( void*)
Cyc_Absyn_VoidType){ goto _LL211;} else{ goto _LL212;} _LL212: if(( unsigned int)
_temp202 > 4u?*(( int*) _temp202) == Cyc_Absyn_VarType: 0){ _LL283: _temp282=((
struct Cyc_Absyn_VarType_struct*) _temp202)->f1; goto _LL213;} else{ goto _LL214;}
_LL214: if(( unsigned int) _temp202 > 4u?*(( int*) _temp202) == Cyc_Absyn_Evar:
0){ _LL291: _temp290=(( struct Cyc_Absyn_Evar_struct*) _temp202)->f1; goto
_LL289; _LL289: _temp288=(( struct Cyc_Absyn_Evar_struct*) _temp202)->f2; if(
_temp288 == 0){ goto _LL287;} else{ goto _LL216;} _LL287: _temp286=(( struct Cyc_Absyn_Evar_struct*)
_temp202)->f3; goto _LL285; _LL285: _temp284=(( struct Cyc_Absyn_Evar_struct*)
_temp202)->f4; goto _LL215;} else{ goto _LL216;} _LL216: if(( unsigned int)
_temp202 > 4u?*(( int*) _temp202) == Cyc_Absyn_Evar: 0){ _LL302: _temp301=((
struct Cyc_Absyn_Evar_struct*) _temp202)->f1; goto _LL297; _LL297: _temp296=((
struct Cyc_Absyn_Evar_struct*) _temp202)->f2; if( _temp296 == 0){ goto _LL218;}
else{ _temp298=* _temp296; _LL300: _temp299=( void*) _temp298.v; goto _LL295;}
_LL295: _temp294=(( struct Cyc_Absyn_Evar_struct*) _temp202)->f3; goto _LL293;
_LL293: _temp292=(( struct Cyc_Absyn_Evar_struct*) _temp202)->f4; goto _LL217;}
else{ goto _LL218;} _LL218: if(( unsigned int) _temp202 > 4u?*(( int*) _temp202)
== Cyc_Absyn_TunionType: 0){ _LL304: _temp303=(( struct Cyc_Absyn_TunionType_struct*)
_temp202)->f1; _LL310: _temp309=( void*) _temp303.tunion_info; goto _LL308;
_LL308: _temp307= _temp303.targs; goto _LL306; _LL306: _temp305=( void*)
_temp303.rgn; goto _LL219;} else{ goto _LL220;} _LL220: if(( unsigned int)
_temp202 > 4u?*(( int*) _temp202) == Cyc_Absyn_TunionFieldType: 0){ _LL312:
_temp311=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp202)->f1; _LL316:
_temp315=( void*) _temp311.field_info; goto _LL314; _LL314: _temp313= _temp311.targs;
goto _LL221;} else{ goto _LL222;} _LL222: if(( unsigned int) _temp202 > 4u?*((
int*) _temp202) == Cyc_Absyn_EnumType: 0){ _LL320: _temp319=(( struct Cyc_Absyn_EnumType_struct*)
_temp202)->f1; goto _LL318; _LL318: _temp317=(( struct Cyc_Absyn_EnumType_struct*)
_temp202)->f2; goto _LL223;} else{ goto _LL224;} _LL224: if(( unsigned int)
_temp202 > 4u?*(( int*) _temp202) == Cyc_Absyn_IntType: 0){ _LL324: _temp323=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp202)->f1; if( _temp323 ==( void*)
Cyc_Absyn_Signed){ goto _LL322;} else{ goto _LL226;} _LL322: _temp321=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp202)->f2; if( _temp321 ==( void*) Cyc_Absyn_B4){
goto _LL225;} else{ goto _LL226;}} else{ goto _LL226;} _LL226: if(( unsigned int)
_temp202 > 4u?*(( int*) _temp202) == Cyc_Absyn_IntType: 0){ _LL328: _temp327=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp202)->f1; if( _temp327 ==( void*)
Cyc_Absyn_Signed){ goto _LL326;} else{ goto _LL228;} _LL326: _temp325=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp202)->f2; if( _temp325 ==( void*) Cyc_Absyn_B1){
goto _LL227;} else{ goto _LL228;}} else{ goto _LL228;} _LL228: if(( unsigned int)
_temp202 > 4u?*(( int*) _temp202) == Cyc_Absyn_IntType: 0){ _LL332: _temp331=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp202)->f1; if( _temp331 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL330;} else{ goto _LL230;} _LL330: _temp329=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp202)->f2; if( _temp329 ==( void*) Cyc_Absyn_B1){
goto _LL229;} else{ goto _LL230;}} else{ goto _LL230;} _LL230: if(( unsigned int)
_temp202 > 4u?*(( int*) _temp202) == Cyc_Absyn_IntType: 0){ _LL336: _temp335=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp202)->f1; if( _temp335 ==( void*)
Cyc_Absyn_Signed){ goto _LL334;} else{ goto _LL232;} _LL334: _temp333=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp202)->f2; if( _temp333 ==( void*) Cyc_Absyn_B2){
goto _LL231;} else{ goto _LL232;}} else{ goto _LL232;} _LL232: if(( unsigned int)
_temp202 > 4u?*(( int*) _temp202) == Cyc_Absyn_IntType: 0){ _LL340: _temp339=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp202)->f1; if( _temp339 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL338;} else{ goto _LL234;} _LL338: _temp337=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp202)->f2; if( _temp337 ==( void*) Cyc_Absyn_B2){
goto _LL233;} else{ goto _LL234;}} else{ goto _LL234;} _LL234: if(( unsigned int)
_temp202 > 4u?*(( int*) _temp202) == Cyc_Absyn_IntType: 0){ _LL344: _temp343=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp202)->f1; if( _temp343 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL342;} else{ goto _LL236;} _LL342: _temp341=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp202)->f2; if( _temp341 ==( void*) Cyc_Absyn_B4){
goto _LL235;} else{ goto _LL236;}} else{ goto _LL236;} _LL236: if(( unsigned int)
_temp202 > 4u?*(( int*) _temp202) == Cyc_Absyn_IntType: 0){ _LL349: _temp348=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp202)->f1; if( _temp348 ==( void*)
Cyc_Absyn_Signed){ goto _LL347;} else{ goto _LL238;} _LL347: _temp346=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp202)->f2; if( _temp346 ==( void*) Cyc_Absyn_B8){
goto _LL345;} else{ goto _LL238;}} else{ goto _LL238;} _LL345: if( Cyc_Absyndump_to_VC){
goto _LL237;} else{ goto _LL238;} _LL238: if(( unsigned int) _temp202 > 4u?*((
int*) _temp202) == Cyc_Absyn_IntType: 0){ _LL353: _temp352=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp202)->f1; if( _temp352 ==( void*) Cyc_Absyn_Signed){ goto _LL351;} else{
goto _LL240;} _LL351: _temp350=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp202)->f2; if( _temp350 ==( void*) Cyc_Absyn_B8){ goto _LL239;} else{ goto
_LL240;}} else{ goto _LL240;} _LL240: if(( unsigned int) _temp202 > 4u?*(( int*)
_temp202) == Cyc_Absyn_IntType: 0){ _LL358: _temp357=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp202)->f1; if( _temp357 ==( void*) Cyc_Absyn_Unsigned){ goto _LL356;} else{
goto _LL242;} _LL356: _temp355=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp202)->f2; if( _temp355 ==( void*) Cyc_Absyn_B8){ goto _LL354;} else{ goto
_LL242;}} else{ goto _LL242;} _LL354: if( Cyc_Absyndump_to_VC){ goto _LL241;}
else{ goto _LL242;} _LL242: if(( unsigned int) _temp202 > 4u?*(( int*) _temp202)
== Cyc_Absyn_IntType: 0){ _LL362: _temp361=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp202)->f1; if( _temp361 ==( void*) Cyc_Absyn_Unsigned){ goto _LL360;} else{
goto _LL244;} _LL360: _temp359=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp202)->f2; if( _temp359 ==( void*) Cyc_Absyn_B8){ goto _LL243;} else{ goto
_LL244;}} else{ goto _LL244;} _LL244: if( _temp202 ==( void*) Cyc_Absyn_FloatType){
goto _LL245;} else{ goto _LL246;} _LL246: if( _temp202 ==( void*) Cyc_Absyn_DoubleType){
goto _LL247;} else{ goto _LL248;} _LL248: if(( unsigned int) _temp202 > 4u?*((
int*) _temp202) == Cyc_Absyn_TupleType: 0){ _LL364: _temp363=(( struct Cyc_Absyn_TupleType_struct*)
_temp202)->f1; goto _LL249;} else{ goto _LL250;} _LL250: if(( unsigned int)
_temp202 > 4u?*(( int*) _temp202) == Cyc_Absyn_StructType: 0){ _LL370: _temp369=((
struct Cyc_Absyn_StructType_struct*) _temp202)->f1; if( _temp369 == 0){ goto
_LL368;} else{ goto _LL252;} _LL368: _temp367=(( struct Cyc_Absyn_StructType_struct*)
_temp202)->f2; goto _LL366; _LL366: _temp365=(( struct Cyc_Absyn_StructType_struct*)
_temp202)->f3; goto _LL251;} else{ goto _LL252;} _LL252: if(( unsigned int)
_temp202 > 4u?*(( int*) _temp202) == Cyc_Absyn_StructType: 0){ _LL376: _temp375=((
struct Cyc_Absyn_StructType_struct*) _temp202)->f1; goto _LL374; _LL374:
_temp373=(( struct Cyc_Absyn_StructType_struct*) _temp202)->f2; goto _LL372;
_LL372: _temp371=(( struct Cyc_Absyn_StructType_struct*) _temp202)->f3; goto
_LL253;} else{ goto _LL254;} _LL254: if(( unsigned int) _temp202 > 4u?*(( int*)
_temp202) == Cyc_Absyn_UnionType: 0){ _LL382: _temp381=(( struct Cyc_Absyn_UnionType_struct*)
_temp202)->f1; if( _temp381 == 0){ goto _LL380;} else{ goto _LL256;} _LL380:
_temp379=(( struct Cyc_Absyn_UnionType_struct*) _temp202)->f2; goto _LL378;
_LL378: _temp377=(( struct Cyc_Absyn_UnionType_struct*) _temp202)->f3; goto
_LL255;} else{ goto _LL256;} _LL256: if(( unsigned int) _temp202 > 4u?*(( int*)
_temp202) == Cyc_Absyn_UnionType: 0){ _LL388: _temp387=(( struct Cyc_Absyn_UnionType_struct*)
_temp202)->f1; goto _LL386; _LL386: _temp385=(( struct Cyc_Absyn_UnionType_struct*)
_temp202)->f2; goto _LL384; _LL384: _temp383=(( struct Cyc_Absyn_UnionType_struct*)
_temp202)->f3; goto _LL257;} else{ goto _LL258;} _LL258: if(( unsigned int)
_temp202 > 4u?*(( int*) _temp202) == Cyc_Absyn_AnonStructType: 0){ _LL390:
_temp389=(( struct Cyc_Absyn_AnonStructType_struct*) _temp202)->f1; goto _LL259;}
else{ goto _LL260;} _LL260: if(( unsigned int) _temp202 > 4u?*(( int*) _temp202)
== Cyc_Absyn_AnonUnionType: 0){ _LL392: _temp391=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp202)->f1; goto _LL261;} else{ goto _LL262;} _LL262: if(( unsigned int)
_temp202 > 4u?*(( int*) _temp202) == Cyc_Absyn_TypedefType: 0){ _LL398: _temp397=((
struct Cyc_Absyn_TypedefType_struct*) _temp202)->f1; goto _LL396; _LL396:
_temp395=(( struct Cyc_Absyn_TypedefType_struct*) _temp202)->f2; goto _LL394;
_LL394: _temp393=(( struct Cyc_Absyn_TypedefType_struct*) _temp202)->f3; goto
_LL263;} else{ goto _LL264;} _LL264: if(( unsigned int) _temp202 > 4u?*(( int*)
_temp202) == Cyc_Absyn_RgnHandleType: 0){ _LL400: _temp399=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp202)->f1; goto _LL265;} else{ goto _LL266;} _LL266: if( _temp202 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL267;} else{ goto _LL268;} _LL268: if(( unsigned int)
_temp202 > 4u?*(( int*) _temp202) == Cyc_Absyn_AccessEff: 0){ _LL402: _temp401=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp202)->f1; goto _LL269;} else{
goto _LL270;} _LL270: if(( unsigned int) _temp202 > 4u?*(( int*) _temp202) ==
Cyc_Absyn_JoinEff: 0){ _LL404: _temp403=(( struct Cyc_Absyn_JoinEff_struct*)
_temp202)->f1; goto _LL271;} else{ goto _LL203;} _LL205: return; _LL207: return;
_LL209: return; _LL211: Cyc_Absyndump_dump(({ unsigned char* _temp405=(
unsigned char*)"void"; struct _tagged_string _temp406; _temp406.curr= _temp405;
_temp406.base= _temp405; _temp406.last_plus_one= _temp405 + 5; _temp406;}));
return; _LL213: Cyc_Absyndump_dump_str( _temp282->name); return; _LL215: Cyc_Absyndump_dump(({
unsigned char* _temp407=( unsigned char*)"%"; struct _tagged_string _temp408;
_temp408.curr= _temp407; _temp408.base= _temp407; _temp408.last_plus_one=
_temp407 + 2; _temp408;})); if( _temp290 == 0){ Cyc_Absyndump_dump(({
unsigned char* _temp409=( unsigned char*)"?"; struct _tagged_string _temp410;
_temp410.curr= _temp409; _temp410.base= _temp409; _temp410.last_plus_one=
_temp409 + 2; _temp410;}));} else{ Cyc_Absyndump_dumpkind(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp290))->v);} Cyc_Absyndump_dump(( struct _tagged_string)
xprintf("(%d)", _temp286)); return; _LL217: Cyc_Absyndump_dumpntyp( _temp299);
return; _LL219:{ void* _temp411= _temp309; struct Cyc_Absyn_UnknownTunionInfo
_temp417; int _temp419; struct _tuple0* _temp421; struct Cyc_Absyn_Tuniondecl*
_temp423; struct Cyc_Absyn_Tuniondecl _temp425; int _temp426; struct Cyc_Core_Opt*
_temp428; struct Cyc_List_List* _temp430; struct _tuple0* _temp432; void*
_temp434; _LL413: if(*(( int*) _temp411) == Cyc_Absyn_UnknownTunion){ _LL418:
_temp417=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp411)->f1; _LL422:
_temp421= _temp417.name; goto _LL420; _LL420: _temp419= _temp417.is_xtunion;
goto _LL414;} else{ goto _LL415;} _LL415: if(*(( int*) _temp411) == Cyc_Absyn_KnownTunion){
_LL424: _temp423=(( struct Cyc_Absyn_KnownTunion_struct*) _temp411)->f1;
_temp425=* _temp423; _LL435: _temp434=( void*) _temp425.sc; goto _LL433; _LL433:
_temp432= _temp425.name; goto _LL431; _LL431: _temp430= _temp425.tvs; goto
_LL429; _LL429: _temp428= _temp425.fields; goto _LL427; _LL427: _temp426=
_temp425.is_xtunion; goto _LL416;} else{ goto _LL412;} _LL414: _temp432=
_temp421; _temp426= _temp419; goto _LL416; _LL416: if( _temp426){ Cyc_Absyndump_dump(({
unsigned char* _temp436=( unsigned char*)"xtunion "; struct _tagged_string
_temp437; _temp437.curr= _temp436; _temp437.base= _temp436; _temp437.last_plus_one=
_temp436 + 9; _temp437;}));} else{ Cyc_Absyndump_dump(({ unsigned char* _temp438=(
unsigned char*)"tunion "; struct _tagged_string _temp439; _temp439.curr=
_temp438; _temp439.base= _temp438; _temp439.last_plus_one= _temp438 + 8;
_temp439;}));}{ void* _temp440= Cyc_Tcutil_compress( _temp305); _LL442: if(
_temp440 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL443;} else{ goto _LL444;} _LL444:
goto _LL445; _LL443: goto _LL441; _LL445: Cyc_Absyndump_dumptyp( _temp305); Cyc_Absyndump_dump(({
unsigned char* _temp446=( unsigned char*)" "; struct _tagged_string _temp447;
_temp447.curr= _temp446; _temp447.base= _temp446; _temp447.last_plus_one=
_temp446 + 2; _temp447;})); goto _LL441; _LL441:;} Cyc_Absyndump_dumpqvar(
_temp432); Cyc_Absyndump_dumptps( _temp307); goto _LL412; _LL412:;} goto _LL203;
_LL221:{ void* _temp448= _temp315; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp454; int _temp456; struct _tuple0* _temp458; struct _tuple0* _temp460;
struct Cyc_Absyn_Tunionfield* _temp462; struct Cyc_Absyn_Tunionfield _temp464;
void* _temp465; struct Cyc_Position_Segment* _temp467; struct Cyc_List_List*
_temp469; struct Cyc_List_List* _temp471; struct _tuple0* _temp473; struct Cyc_Absyn_Tuniondecl*
_temp475; struct Cyc_Absyn_Tuniondecl _temp477; int _temp478; struct Cyc_Core_Opt*
_temp480; struct Cyc_List_List* _temp482; struct _tuple0* _temp484; void*
_temp486; _LL450: if(*(( int*) _temp448) == Cyc_Absyn_UnknownTunionfield){
_LL455: _temp454=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp448)->f1;
_LL461: _temp460= _temp454.tunion_name; goto _LL459; _LL459: _temp458= _temp454.field_name;
goto _LL457; _LL457: _temp456= _temp454.is_xtunion; goto _LL451;} else{ goto
_LL452;} _LL452: if(*(( int*) _temp448) == Cyc_Absyn_KnownTunionfield){ _LL476:
_temp475=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp448)->f1; _temp477=*
_temp475; _LL487: _temp486=( void*) _temp477.sc; goto _LL485; _LL485: _temp484=
_temp477.name; goto _LL483; _LL483: _temp482= _temp477.tvs; goto _LL481; _LL481:
_temp480= _temp477.fields; goto _LL479; _LL479: _temp478= _temp477.is_xtunion;
goto _LL463; _LL463: _temp462=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp448)->f2; _temp464=* _temp462; _LL474: _temp473= _temp464.name; goto _LL472;
_LL472: _temp471= _temp464.tvs; goto _LL470; _LL470: _temp469= _temp464.typs;
goto _LL468; _LL468: _temp467= _temp464.loc; goto _LL466; _LL466: _temp465=(
void*) _temp464.sc; goto _LL453;} else{ goto _LL449;} _LL451: _temp484= _temp460;
_temp478= _temp456; _temp473= _temp458; goto _LL453; _LL453: if( _temp478){ Cyc_Absyndump_dump(({
unsigned char* _temp488=( unsigned char*)"xtunion "; struct _tagged_string
_temp489; _temp489.curr= _temp488; _temp489.base= _temp488; _temp489.last_plus_one=
_temp488 + 9; _temp489;}));} else{ Cyc_Absyndump_dump(({ unsigned char* _temp490=(
unsigned char*)"tunion "; struct _tagged_string _temp491; _temp491.curr=
_temp490; _temp491.base= _temp490; _temp491.last_plus_one= _temp490 + 8;
_temp491;}));} Cyc_Absyndump_dumpqvar( _temp484); Cyc_Absyndump_dump(({
unsigned char* _temp492=( unsigned char*)"."; struct _tagged_string _temp493;
_temp493.curr= _temp492; _temp493.base= _temp492; _temp493.last_plus_one=
_temp492 + 2; _temp493;})); Cyc_Absyndump_dumpqvar( _temp473); Cyc_Absyndump_dumptps(
_temp313); goto _LL449; _LL449:;} goto _LL203; _LL223: Cyc_Absyndump_dump(({
unsigned char* _temp494=( unsigned char*)"enum "; struct _tagged_string _temp495;
_temp495.curr= _temp494; _temp495.base= _temp494; _temp495.last_plus_one=
_temp494 + 6; _temp495;})); Cyc_Absyndump_dumpqvar( _temp319); return; _LL225:
Cyc_Absyndump_dump(({ unsigned char* _temp496=( unsigned char*)"int"; struct
_tagged_string _temp497; _temp497.curr= _temp496; _temp497.base= _temp496;
_temp497.last_plus_one= _temp496 + 4; _temp497;})); return; _LL227: Cyc_Absyndump_dump(({
unsigned char* _temp498=( unsigned char*)"signed char"; struct _tagged_string
_temp499; _temp499.curr= _temp498; _temp499.base= _temp498; _temp499.last_plus_one=
_temp498 + 12; _temp499;})); return; _LL229: Cyc_Absyndump_dump(({ unsigned char*
_temp500=( unsigned char*)"unsigned char"; struct _tagged_string _temp501;
_temp501.curr= _temp500; _temp501.base= _temp500; _temp501.last_plus_one=
_temp500 + 14; _temp501;})); return; _LL231: Cyc_Absyndump_dump(({ unsigned char*
_temp502=( unsigned char*)"short"; struct _tagged_string _temp503; _temp503.curr=
_temp502; _temp503.base= _temp502; _temp503.last_plus_one= _temp502 + 6;
_temp503;})); return; _LL233: Cyc_Absyndump_dump(({ unsigned char* _temp504=(
unsigned char*)"unsigned short"; struct _tagged_string _temp505; _temp505.curr=
_temp504; _temp505.base= _temp504; _temp505.last_plus_one= _temp504 + 15;
_temp505;})); return; _LL235: Cyc_Absyndump_dump(({ unsigned char* _temp506=(
unsigned char*)"unsigned int"; struct _tagged_string _temp507; _temp507.curr=
_temp506; _temp507.base= _temp506; _temp507.last_plus_one= _temp506 + 13;
_temp507;})); return; _LL237: Cyc_Absyndump_dump(({ unsigned char* _temp508=(
unsigned char*)"__int64"; struct _tagged_string _temp509; _temp509.curr=
_temp508; _temp509.base= _temp508; _temp509.last_plus_one= _temp508 + 8;
_temp509;})); return; _LL239: Cyc_Absyndump_dump(({ unsigned char* _temp510=(
unsigned char*)"long long"; struct _tagged_string _temp511; _temp511.curr=
_temp510; _temp511.base= _temp510; _temp511.last_plus_one= _temp510 + 10;
_temp511;})); return; _LL241: Cyc_Absyndump_dump(({ unsigned char* _temp512=(
unsigned char*)"unsigned __int64"; struct _tagged_string _temp513; _temp513.curr=
_temp512; _temp513.base= _temp512; _temp513.last_plus_one= _temp512 + 17;
_temp513;})); return; _LL243: Cyc_Absyndump_dump(({ unsigned char* _temp514=(
unsigned char*)"unsigned long long"; struct _tagged_string _temp515; _temp515.curr=
_temp514; _temp515.base= _temp514; _temp515.last_plus_one= _temp514 + 19;
_temp515;})); return; _LL245: Cyc_Absyndump_dump(({ unsigned char* _temp516=(
unsigned char*)"float"; struct _tagged_string _temp517; _temp517.curr= _temp516;
_temp517.base= _temp516; _temp517.last_plus_one= _temp516 + 6; _temp517;}));
return; _LL247: Cyc_Absyndump_dump(({ unsigned char* _temp518=( unsigned char*)"double";
struct _tagged_string _temp519; _temp519.curr= _temp518; _temp519.base= _temp518;
_temp519.last_plus_one= _temp518 + 7; _temp519;})); return; _LL249: Cyc_Absyndump_dump_char((
int)'$'); Cyc_Absyndump_dumpargs( _temp363); return; _LL251: Cyc_Absyndump_dump(({
unsigned char* _temp520=( unsigned char*)"struct"; struct _tagged_string
_temp521; _temp521.curr= _temp520; _temp521.base= _temp520; _temp521.last_plus_one=
_temp520 + 7; _temp521;})); Cyc_Absyndump_dumptps( _temp367); return; _LL253:
Cyc_Absyndump_dump(({ unsigned char* _temp522=( unsigned char*)"struct"; struct
_tagged_string _temp523; _temp523.curr= _temp522; _temp523.base= _temp522;
_temp523.last_plus_one= _temp522 + 7; _temp523;})); Cyc_Absyndump_dumpqvar((
struct _tuple0*) _check_null( _temp375)); Cyc_Absyndump_dumptps( _temp373);
return; _LL255: Cyc_Absyndump_dump(({ unsigned char* _temp524=( unsigned char*)"union";
struct _tagged_string _temp525; _temp525.curr= _temp524; _temp525.base= _temp524;
_temp525.last_plus_one= _temp524 + 6; _temp525;})); Cyc_Absyndump_dumptps(
_temp379); return; _LL257: Cyc_Absyndump_dump(({ unsigned char* _temp526=(
unsigned char*)"union"; struct _tagged_string _temp527; _temp527.curr= _temp526;
_temp527.base= _temp526; _temp527.last_plus_one= _temp526 + 6; _temp527;})); Cyc_Absyndump_dumpqvar((
struct _tuple0*) _check_null( _temp387)); Cyc_Absyndump_dumptps( _temp385);
return; _LL259: Cyc_Absyndump_dump(({ unsigned char* _temp528=( unsigned char*)"struct {";
struct _tagged_string _temp529; _temp529.curr= _temp528; _temp529.base= _temp528;
_temp529.last_plus_one= _temp528 + 9; _temp529;})); Cyc_Absyndump_dumpstructfields(
_temp389); Cyc_Absyndump_dump(({ unsigned char* _temp530=( unsigned char*)"}";
struct _tagged_string _temp531; _temp531.curr= _temp530; _temp531.base= _temp530;
_temp531.last_plus_one= _temp530 + 2; _temp531;})); return; _LL261: Cyc_Absyndump_dump(({
unsigned char* _temp532=( unsigned char*)"union {"; struct _tagged_string
_temp533; _temp533.curr= _temp532; _temp533.base= _temp532; _temp533.last_plus_one=
_temp532 + 8; _temp533;})); Cyc_Absyndump_dumpstructfields( _temp391); Cyc_Absyndump_dump(({
unsigned char* _temp534=( unsigned char*)"}"; struct _tagged_string _temp535;
_temp535.curr= _temp534; _temp535.base= _temp534; _temp535.last_plus_one=
_temp534 + 2; _temp535;})); return; _LL263:( Cyc_Absyndump_dumpqvar( _temp397),
Cyc_Absyndump_dumptps( _temp395)); return; _LL265: Cyc_Absyndump_dumprgn(
_temp399); return; _LL267: return; _LL269: return; _LL271: return; _LL203:;}
void Cyc_Absyndump_dumpvaropt( struct Cyc_Core_Opt* vo){ if( vo != 0){ Cyc_Absyndump_dump_str((
struct _tagged_string*)(( struct Cyc_Core_Opt*) _check_null( vo))->v);}} void
Cyc_Absyndump_dumpfunarg( struct _tuple1* t){(( void(*)( struct Cyc_Absyn_Tqual,
void*, void(* f)( struct Cyc_Core_Opt*), struct Cyc_Core_Opt*)) Cyc_Absyndump_dumptqtd)((*
t).f2,(* t).f3, Cyc_Absyndump_dumpvaropt,(* t).f1);} struct _tuple7{ void* f1;
void* f2; } ; void Cyc_Absyndump_dump_rgncmp( struct _tuple7* cmp){ struct
_tuple7 _temp538; void* _temp539; void* _temp541; struct _tuple7* _temp536= cmp;
_temp538=* _temp536; _LL542: _temp541= _temp538.f1; goto _LL540; _LL540:
_temp539= _temp538.f2; goto _LL537; _LL537: Cyc_Absyndump_dumptyp( _temp541);
Cyc_Absyndump_dump_char(( int)'<'); Cyc_Absyndump_dumptyp( _temp539);} void Cyc_Absyndump_dump_rgnpo(
struct Cyc_List_List* rgn_po){(( void(*)( void(* f)( struct _tuple7*), struct
Cyc_List_List* l, struct _tagged_string sep)) Cyc_Absyndump_dump_sep)( Cyc_Absyndump_dump_rgncmp,
rgn_po,({ unsigned char* _temp543=( unsigned char*)","; struct _tagged_string
_temp544; _temp544.curr= _temp543; _temp544.base= _temp543; _temp544.last_plus_one=
_temp543 + 2; _temp544;}));} void Cyc_Absyndump_dumpfunargs( struct Cyc_List_List*
args, int c_varargs, struct Cyc_Absyn_VarargInfo* cyc_varargs, struct Cyc_Core_Opt*
effopt, struct Cyc_List_List* rgn_po){ Cyc_Absyndump_dump_char(( int)'('); for(
0; args != 0; args=(( struct Cyc_List_List*) _check_null( args))->tl){ Cyc_Absyndump_dumpfunarg((
struct _tuple1*)(( struct Cyc_List_List*) _check_null( args))->hd); if((((
struct Cyc_List_List*) _check_null( args))->tl != 0? 1: c_varargs)? 1:
cyc_varargs != 0){ Cyc_Absyndump_dump_char(( int)',');}} if( c_varargs){ Cyc_Absyndump_dump(({
unsigned char* _temp545=( unsigned char*)"..."; struct _tagged_string _temp546;
_temp546.curr= _temp545; _temp546.base= _temp545; _temp546.last_plus_one=
_temp545 + 4; _temp546;}));} else{ if( cyc_varargs != 0){ struct _tuple1*
_temp548=({ struct _tuple1* _temp547=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp547->f1=(( struct Cyc_Absyn_VarargInfo*) _check_null(
cyc_varargs))->name; _temp547->f2=(( struct Cyc_Absyn_VarargInfo*) _check_null(
cyc_varargs))->tq; _temp547->f3=( void*)(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->type; _temp547;}); goto _LL549; _LL549: Cyc_Absyndump_dump(({
unsigned char* _temp550=( unsigned char*)"..."; struct _tagged_string _temp551;
_temp551.curr= _temp550; _temp551.base= _temp550; _temp551.last_plus_one=
_temp550 + 4; _temp551;})); Cyc_Absyndump_dumptyp(( void*)(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->rgn); if((( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->inject){ Cyc_Absyndump_dump(({ unsigned char*
_temp552=( unsigned char*)" inject "; struct _tagged_string _temp553; _temp553.curr=
_temp552; _temp553.base= _temp552; _temp553.last_plus_one= _temp552 + 9;
_temp553;}));} Cyc_Absyndump_dumpfunarg( _temp548);}} if( effopt != 0){ Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpeff(( void*)(( struct Cyc_Core_Opt*) _check_null( effopt))->v);}
if( rgn_po != 0){ Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dump_rgnpo(
rgn_po);} Cyc_Absyndump_dump_char(( int)')');} void Cyc_Absyndump_dumptyp( void*
t){(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( int), int)) Cyc_Absyndump_dumptqtd)(({
struct Cyc_Absyn_Tqual _temp554; _temp554.q_const= 0; _temp554.q_volatile= 0;
_temp554.q_restrict= 0; _temp554;}), t,( void(*)( int x)) Cyc_Absyndump_ignore,
0);} void Cyc_Absyndump_dumpdesignator( void* d){ void* _temp555= d; struct Cyc_Absyn_Exp*
_temp561; struct _tagged_string* _temp563; _LL557: if(*(( int*) _temp555) == Cyc_Absyn_ArrayElement){
_LL562: _temp561=(( struct Cyc_Absyn_ArrayElement_struct*) _temp555)->f1; goto
_LL558;} else{ goto _LL559;} _LL559: if(*(( int*) _temp555) == Cyc_Absyn_FieldName){
_LL564: _temp563=(( struct Cyc_Absyn_FieldName_struct*) _temp555)->f1; goto
_LL560;} else{ goto _LL556;} _LL558: Cyc_Absyndump_dump(({ unsigned char*
_temp565=( unsigned char*)".["; struct _tagged_string _temp566; _temp566.curr=
_temp565; _temp566.base= _temp565; _temp566.last_plus_one= _temp565 + 3;
_temp566;})); Cyc_Absyndump_dumpexp( _temp561); Cyc_Absyndump_dump_char(( int)']');
goto _LL556; _LL560: Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp563); goto _LL556; _LL556:;} struct _tuple8{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; void Cyc_Absyndump_dumpde( struct _tuple8* de){((
void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpdesignator,(* de).f1,({ unsigned char* _temp567=(
unsigned char*)""; struct _tagged_string _temp568; _temp568.curr= _temp567;
_temp568.base= _temp567; _temp568.last_plus_one= _temp567 + 1; _temp568;}),({
unsigned char* _temp569=( unsigned char*)"="; struct _tagged_string _temp570;
_temp570.curr= _temp569; _temp570.base= _temp569; _temp570.last_plus_one=
_temp569 + 2; _temp570;}),({ unsigned char* _temp571=( unsigned char*)"=";
struct _tagged_string _temp572; _temp572.curr= _temp571; _temp572.base= _temp571;
_temp572.last_plus_one= _temp571 + 2; _temp572;})); Cyc_Absyndump_dumpexp((* de).f2);}
void Cyc_Absyndump_dumpexps_prec( int inprec, struct Cyc_List_List* es){(( void(*)(
void(* f)( int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group_c)(
Cyc_Absyndump_dumpexp_prec, inprec, es,({ unsigned char* _temp573=(
unsigned char*)""; struct _tagged_string _temp574; _temp574.curr= _temp573;
_temp574.base= _temp573; _temp574.last_plus_one= _temp573 + 1; _temp574;}),({
unsigned char* _temp575=( unsigned char*)""; struct _tagged_string _temp576;
_temp576.curr= _temp575; _temp576.base= _temp575; _temp576.last_plus_one=
_temp575 + 1; _temp576;}),({ unsigned char* _temp577=( unsigned char*)",";
struct _tagged_string _temp578; _temp578.curr= _temp577; _temp578.base= _temp577;
_temp578.last_plus_one= _temp577 + 2; _temp578;}));} void Cyc_Absyndump_dumpexp_prec(
int inprec, struct Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec( e); if(
inprec >= myprec){ Cyc_Absyndump_dump_nospace(({ unsigned char* _temp579=(
unsigned char*)"("; struct _tagged_string _temp580; _temp580.curr= _temp579;
_temp580.base= _temp579; _temp580.last_plus_one= _temp579 + 2; _temp580;}));}{
void* _temp581=( void*) e->r; void* _temp675; unsigned char _temp677; void*
_temp679; void* _temp681; short _temp683; void* _temp685; void* _temp687; int
_temp689; void* _temp691; void* _temp693; int _temp695; void* _temp697; void*
_temp699; long long _temp701; void* _temp703; void* _temp705; struct
_tagged_string _temp707; void* _temp709; void* _temp711; struct _tagged_string
_temp713; struct _tuple0* _temp715; void* _temp717; struct _tuple0* _temp719;
struct Cyc_List_List* _temp721; void* _temp723; struct Cyc_Absyn_Exp* _temp725;
struct Cyc_Core_Opt* _temp727; struct Cyc_Absyn_Exp* _temp729; void* _temp731;
struct Cyc_Absyn_Exp* _temp733; void* _temp735; struct Cyc_Absyn_Exp* _temp737;
void* _temp739; struct Cyc_Absyn_Exp* _temp741; void* _temp743; struct Cyc_Absyn_Exp*
_temp745; struct Cyc_Absyn_Exp* _temp747; struct Cyc_Absyn_Exp* _temp749; struct
Cyc_Absyn_Exp* _temp751; struct Cyc_Absyn_Exp* _temp753; struct Cyc_Absyn_Exp*
_temp755; struct Cyc_List_List* _temp757; struct Cyc_Absyn_Exp* _temp759; struct
Cyc_Absyn_VarargCallInfo* _temp761; struct Cyc_List_List* _temp763; struct Cyc_Absyn_Exp*
_temp765; struct Cyc_Absyn_Exp* _temp767; struct Cyc_Absyn_Exp* _temp769; struct
Cyc_List_List* _temp771; struct Cyc_Absyn_Exp* _temp773; struct Cyc_Absyn_Exp*
_temp775; void* _temp777; struct Cyc_Absyn_Exp* _temp779; struct Cyc_Absyn_Exp*
_temp781; struct Cyc_Absyn_Exp* _temp783; void* _temp785; struct Cyc_Absyn_Exp*
_temp787; struct _tagged_string* _temp789; void* _temp791; struct Cyc_Absyn_Exp*
_temp793; struct _tagged_string* _temp795; struct Cyc_Absyn_Exp* _temp797;
struct _tagged_string* _temp799; struct Cyc_Absyn_Exp* _temp801; struct Cyc_Absyn_Exp*
_temp803; struct Cyc_Absyn_Exp* _temp805; struct Cyc_List_List* _temp807; struct
Cyc_List_List* _temp809; struct _tuple1* _temp811; struct Cyc_List_List*
_temp813; struct Cyc_Absyn_Exp* _temp815; struct Cyc_Absyn_Exp* _temp817; struct
Cyc_Absyn_Vardecl* _temp819; struct Cyc_Absyn_Structdecl* _temp821; struct Cyc_List_List*
_temp823; struct Cyc_Core_Opt* _temp825; struct _tuple0* _temp827; struct Cyc_List_List*
_temp829; void* _temp831; struct Cyc_Absyn_Tunionfield* _temp833; struct Cyc_Absyn_Tuniondecl*
_temp835; struct Cyc_List_List* _temp837; struct Cyc_Core_Opt* _temp839; struct
Cyc_Core_Opt* _temp841; struct Cyc_Absyn_Enumfield* _temp843; struct Cyc_Absyn_Enumdecl*
_temp845; struct _tuple0* _temp847; void* _temp849; struct Cyc_Absyn_Exp*
_temp851; struct Cyc_List_List* _temp853; struct Cyc_Core_Opt* _temp855; struct
Cyc_Absyn_Stmt* _temp857; struct Cyc_Absyn_Fndecl* _temp859; struct Cyc_Absyn_Exp*
_temp861; _LL583: if(*(( int*) _temp581) == Cyc_Absyn_Const_e){ _LL676: _temp675=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp581)->f1; if(( unsigned int)
_temp675 > 1u?*(( int*) _temp675) == Cyc_Absyn_Char_c: 0){ _LL680: _temp679=(
void*)(( struct Cyc_Absyn_Char_c_struct*) _temp675)->f1; goto _LL678; _LL678:
_temp677=(( struct Cyc_Absyn_Char_c_struct*) _temp675)->f2; goto _LL584;} else{
goto _LL585;}} else{ goto _LL585;} _LL585: if(*(( int*) _temp581) == Cyc_Absyn_Const_e){
_LL682: _temp681=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp581)->f1; if((
unsigned int) _temp681 > 1u?*(( int*) _temp681) == Cyc_Absyn_Short_c: 0){ _LL686:
_temp685=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp681)->f1; goto _LL684;
_LL684: _temp683=(( struct Cyc_Absyn_Short_c_struct*) _temp681)->f2; goto _LL586;}
else{ goto _LL587;}} else{ goto _LL587;} _LL587: if(*(( int*) _temp581) == Cyc_Absyn_Const_e){
_LL688: _temp687=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp581)->f1; if((
unsigned int) _temp687 > 1u?*(( int*) _temp687) == Cyc_Absyn_Int_c: 0){ _LL692:
_temp691=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp687)->f1; if( _temp691
==( void*) Cyc_Absyn_Signed){ goto _LL690;} else{ goto _LL589;} _LL690: _temp689=((
struct Cyc_Absyn_Int_c_struct*) _temp687)->f2; goto _LL588;} else{ goto _LL589;}}
else{ goto _LL589;} _LL589: if(*(( int*) _temp581) == Cyc_Absyn_Const_e){ _LL694:
_temp693=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp581)->f1; if((
unsigned int) _temp693 > 1u?*(( int*) _temp693) == Cyc_Absyn_Int_c: 0){ _LL698:
_temp697=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp693)->f1; if( _temp697
==( void*) Cyc_Absyn_Unsigned){ goto _LL696;} else{ goto _LL591;} _LL696:
_temp695=(( struct Cyc_Absyn_Int_c_struct*) _temp693)->f2; goto _LL590;} else{
goto _LL591;}} else{ goto _LL591;} _LL591: if(*(( int*) _temp581) == Cyc_Absyn_Const_e){
_LL700: _temp699=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp581)->f1; if((
unsigned int) _temp699 > 1u?*(( int*) _temp699) == Cyc_Absyn_LongLong_c: 0){
_LL704: _temp703=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp699)->f1;
goto _LL702; _LL702: _temp701=(( struct Cyc_Absyn_LongLong_c_struct*) _temp699)->f2;
goto _LL592;} else{ goto _LL593;}} else{ goto _LL593;} _LL593: if(*(( int*)
_temp581) == Cyc_Absyn_Const_e){ _LL706: _temp705=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp581)->f1; if(( unsigned int) _temp705 > 1u?*(( int*) _temp705) == Cyc_Absyn_Float_c:
0){ _LL708: _temp707=(( struct Cyc_Absyn_Float_c_struct*) _temp705)->f1; goto
_LL594;} else{ goto _LL595;}} else{ goto _LL595;} _LL595: if(*(( int*) _temp581)
== Cyc_Absyn_Const_e){ _LL710: _temp709=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp581)->f1; if( _temp709 ==( void*) Cyc_Absyn_Null_c){ goto _LL596;} else{
goto _LL597;}} else{ goto _LL597;} _LL597: if(*(( int*) _temp581) == Cyc_Absyn_Const_e){
_LL712: _temp711=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp581)->f1; if((
unsigned int) _temp711 > 1u?*(( int*) _temp711) == Cyc_Absyn_String_c: 0){
_LL714: _temp713=(( struct Cyc_Absyn_String_c_struct*) _temp711)->f1; goto
_LL598;} else{ goto _LL599;}} else{ goto _LL599;} _LL599: if(*(( int*) _temp581)
== Cyc_Absyn_UnknownId_e){ _LL716: _temp715=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp581)->f1; goto _LL600;} else{ goto _LL601;} _LL601: if(*(( int*) _temp581)
== Cyc_Absyn_Var_e){ _LL720: _temp719=(( struct Cyc_Absyn_Var_e_struct*)
_temp581)->f1; goto _LL718; _LL718: _temp717=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp581)->f2; goto _LL602;} else{ goto _LL603;} _LL603: if(*(( int*) _temp581)
== Cyc_Absyn_Primop_e){ _LL724: _temp723=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp581)->f1; goto _LL722; _LL722: _temp721=(( struct Cyc_Absyn_Primop_e_struct*)
_temp581)->f2; goto _LL604;} else{ goto _LL605;} _LL605: if(*(( int*) _temp581)
== Cyc_Absyn_AssignOp_e){ _LL730: _temp729=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp581)->f1; goto _LL728; _LL728: _temp727=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp581)->f2; goto _LL726; _LL726: _temp725=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp581)->f3; goto _LL606;} else{ goto _LL607;} _LL607: if(*(( int*) _temp581)
== Cyc_Absyn_Increment_e){ _LL734: _temp733=(( struct Cyc_Absyn_Increment_e_struct*)
_temp581)->f1; goto _LL732; _LL732: _temp731=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp581)->f2; if( _temp731 ==( void*) Cyc_Absyn_PreInc){ goto _LL608;} else{
goto _LL609;}} else{ goto _LL609;} _LL609: if(*(( int*) _temp581) == Cyc_Absyn_Increment_e){
_LL738: _temp737=(( struct Cyc_Absyn_Increment_e_struct*) _temp581)->f1; goto
_LL736; _LL736: _temp735=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp581)->f2; if( _temp735 ==( void*) Cyc_Absyn_PreDec){ goto _LL610;} else{
goto _LL611;}} else{ goto _LL611;} _LL611: if(*(( int*) _temp581) == Cyc_Absyn_Increment_e){
_LL742: _temp741=(( struct Cyc_Absyn_Increment_e_struct*) _temp581)->f1; goto
_LL740; _LL740: _temp739=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp581)->f2; if( _temp739 ==( void*) Cyc_Absyn_PostInc){ goto _LL612;} else{
goto _LL613;}} else{ goto _LL613;} _LL613: if(*(( int*) _temp581) == Cyc_Absyn_Increment_e){
_LL746: _temp745=(( struct Cyc_Absyn_Increment_e_struct*) _temp581)->f1; goto
_LL744; _LL744: _temp743=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp581)->f2; if( _temp743 ==( void*) Cyc_Absyn_PostDec){ goto _LL614;} else{
goto _LL615;}} else{ goto _LL615;} _LL615: if(*(( int*) _temp581) == Cyc_Absyn_Conditional_e){
_LL752: _temp751=(( struct Cyc_Absyn_Conditional_e_struct*) _temp581)->f1; goto
_LL750; _LL750: _temp749=(( struct Cyc_Absyn_Conditional_e_struct*) _temp581)->f2;
goto _LL748; _LL748: _temp747=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp581)->f3; goto _LL616;} else{ goto _LL617;} _LL617: if(*(( int*) _temp581)
== Cyc_Absyn_SeqExp_e){ _LL756: _temp755=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp581)->f1; goto _LL754; _LL754: _temp753=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp581)->f2; goto _LL618;} else{ goto _LL619;} _LL619: if(*(( int*) _temp581)
== Cyc_Absyn_UnknownCall_e){ _LL760: _temp759=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp581)->f1; goto _LL758; _LL758: _temp757=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp581)->f2; goto _LL620;} else{ goto _LL621;} _LL621: if(*(( int*) _temp581)
== Cyc_Absyn_FnCall_e){ _LL766: _temp765=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp581)->f1; goto _LL764; _LL764: _temp763=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp581)->f2; goto _LL762; _LL762: _temp761=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp581)->f3; goto _LL622;} else{ goto _LL623;} _LL623: if(*(( int*) _temp581)
== Cyc_Absyn_Throw_e){ _LL768: _temp767=(( struct Cyc_Absyn_Throw_e_struct*)
_temp581)->f1; goto _LL624;} else{ goto _LL625;} _LL625: if(*(( int*) _temp581)
== Cyc_Absyn_NoInstantiate_e){ _LL770: _temp769=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp581)->f1; goto _LL626;} else{ goto _LL627;} _LL627: if(*(( int*) _temp581)
== Cyc_Absyn_Instantiate_e){ _LL774: _temp773=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp581)->f1; goto _LL772; _LL772: _temp771=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp581)->f2; goto _LL628;} else{ goto _LL629;} _LL629: if(*(( int*) _temp581)
== Cyc_Absyn_Cast_e){ _LL778: _temp777=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp581)->f1; goto _LL776; _LL776: _temp775=(( struct Cyc_Absyn_Cast_e_struct*)
_temp581)->f2; goto _LL630;} else{ goto _LL631;} _LL631: if(*(( int*) _temp581)
== Cyc_Absyn_Address_e){ _LL780: _temp779=(( struct Cyc_Absyn_Address_e_struct*)
_temp581)->f1; goto _LL632;} else{ goto _LL633;} _LL633: if(*(( int*) _temp581)
== Cyc_Absyn_New_e){ _LL784: _temp783=(( struct Cyc_Absyn_New_e_struct*)
_temp581)->f1; goto _LL782; _LL782: _temp781=(( struct Cyc_Absyn_New_e_struct*)
_temp581)->f2; goto _LL634;} else{ goto _LL635;} _LL635: if(*(( int*) _temp581)
== Cyc_Absyn_Sizeoftyp_e){ _LL786: _temp785=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp581)->f1; goto _LL636;} else{ goto _LL637;} _LL637: if(*(( int*) _temp581)
== Cyc_Absyn_Sizeofexp_e){ _LL788: _temp787=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp581)->f1; goto _LL638;} else{ goto _LL639;} _LL639: if(*(( int*) _temp581)
== Cyc_Absyn_Offsetof_e){ _LL792: _temp791=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp581)->f1; goto _LL790; _LL790: _temp789=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp581)->f2; goto _LL640;} else{ goto _LL641;} _LL641: if(*(( int*) _temp581)
== Cyc_Absyn_Deref_e){ _LL794: _temp793=(( struct Cyc_Absyn_Deref_e_struct*)
_temp581)->f1; goto _LL642;} else{ goto _LL643;} _LL643: if(*(( int*) _temp581)
== Cyc_Absyn_StructMember_e){ _LL798: _temp797=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp581)->f1; goto _LL796; _LL796: _temp795=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp581)->f2; goto _LL644;} else{ goto _LL645;} _LL645: if(*(( int*) _temp581)
== Cyc_Absyn_StructArrow_e){ _LL802: _temp801=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp581)->f1; goto _LL800; _LL800: _temp799=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp581)->f2; goto _LL646;} else{ goto _LL647;} _LL647: if(*(( int*) _temp581)
== Cyc_Absyn_Subscript_e){ _LL806: _temp805=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp581)->f1; goto _LL804; _LL804: _temp803=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp581)->f2; goto _LL648;} else{ goto _LL649;} _LL649: if(*(( int*) _temp581)
== Cyc_Absyn_Tuple_e){ _LL808: _temp807=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp581)->f1; goto _LL650;} else{ goto _LL651;} _LL651: if(*(( int*) _temp581)
== Cyc_Absyn_CompoundLit_e){ _LL812: _temp811=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp581)->f1; goto _LL810; _LL810: _temp809=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp581)->f2; goto _LL652;} else{ goto _LL653;} _LL653: if(*(( int*) _temp581)
== Cyc_Absyn_Array_e){ _LL814: _temp813=(( struct Cyc_Absyn_Array_e_struct*)
_temp581)->f1; goto _LL654;} else{ goto _LL655;} _LL655: if(*(( int*) _temp581)
== Cyc_Absyn_Comprehension_e){ _LL820: _temp819=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp581)->f1; goto _LL818; _LL818: _temp817=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp581)->f2; goto _LL816; _LL816: _temp815=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp581)->f3; goto _LL656;} else{ goto _LL657;} _LL657: if(*(( int*) _temp581)
== Cyc_Absyn_Struct_e){ _LL828: _temp827=(( struct Cyc_Absyn_Struct_e_struct*)
_temp581)->f1; goto _LL826; _LL826: _temp825=(( struct Cyc_Absyn_Struct_e_struct*)
_temp581)->f2; goto _LL824; _LL824: _temp823=(( struct Cyc_Absyn_Struct_e_struct*)
_temp581)->f3; goto _LL822; _LL822: _temp821=(( struct Cyc_Absyn_Struct_e_struct*)
_temp581)->f4; goto _LL658;} else{ goto _LL659;} _LL659: if(*(( int*) _temp581)
== Cyc_Absyn_AnonStruct_e){ _LL832: _temp831=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp581)->f1; goto _LL830; _LL830: _temp829=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp581)->f2; goto _LL660;} else{ goto _LL661;} _LL661: if(*(( int*) _temp581)
== Cyc_Absyn_Tunion_e){ _LL842: _temp841=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp581)->f1; goto _LL840; _LL840: _temp839=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp581)->f2; goto _LL838; _LL838: _temp837=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp581)->f3; goto _LL836; _LL836: _temp835=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp581)->f4; goto _LL834; _LL834: _temp833=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp581)->f5; goto _LL662;} else{ goto _LL663;} _LL663: if(*(( int*) _temp581)
== Cyc_Absyn_Enum_e){ _LL848: _temp847=(( struct Cyc_Absyn_Enum_e_struct*)
_temp581)->f1; goto _LL846; _LL846: _temp845=(( struct Cyc_Absyn_Enum_e_struct*)
_temp581)->f2; goto _LL844; _LL844: _temp843=(( struct Cyc_Absyn_Enum_e_struct*)
_temp581)->f3; goto _LL664;} else{ goto _LL665;} _LL665: if(*(( int*) _temp581)
== Cyc_Absyn_Malloc_e){ _LL852: _temp851=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp581)->f1; goto _LL850; _LL850: _temp849=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp581)->f2; goto _LL666;} else{ goto _LL667;} _LL667: if(*(( int*) _temp581)
== Cyc_Absyn_UnresolvedMem_e){ _LL856: _temp855=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp581)->f1; goto _LL854; _LL854: _temp853=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp581)->f2; goto _LL668;} else{ goto _LL669;} _LL669: if(*(( int*) _temp581)
== Cyc_Absyn_StmtExp_e){ _LL858: _temp857=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp581)->f1; goto _LL670;} else{ goto _LL671;} _LL671: if(*(( int*) _temp581)
== Cyc_Absyn_Codegen_e){ _LL860: _temp859=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp581)->f1; goto _LL672;} else{ goto _LL673;} _LL673: if(*(( int*) _temp581)
== Cyc_Absyn_Fill_e){ _LL862: _temp861=(( struct Cyc_Absyn_Fill_e_struct*)
_temp581)->f1; goto _LL674;} else{ goto _LL582;} _LL584: Cyc_Absyndump_dump_char((
int)'\''); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_char_escape( _temp677)); Cyc_Absyndump_dump_char((
int)'\''); goto _LL582; _LL586: Cyc_Absyndump_dump(( struct _tagged_string) Cyc_Core_string_of_int((
int) _temp683)); goto _LL582; _LL588: Cyc_Absyndump_dump(( struct _tagged_string)
Cyc_Core_string_of_int( _temp689)); goto _LL582; _LL590: Cyc_Absyndump_dump((
struct _tagged_string) Cyc_Core_string_of_int( _temp695)); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp863=( unsigned char*)"u"; struct _tagged_string _temp864;
_temp864.curr= _temp863; _temp864.base= _temp863; _temp864.last_plus_one=
_temp863 + 2; _temp864;})); goto _LL582; _LL592: Cyc_Absyndump_dump(({
unsigned char* _temp865=( unsigned char*)"<<FIX LONG LONG CONSTANT>>"; struct
_tagged_string _temp866; _temp866.curr= _temp865; _temp866.base= _temp865;
_temp866.last_plus_one= _temp865 + 27; _temp866;})); goto _LL582; _LL594: Cyc_Absyndump_dump(
_temp707); goto _LL582; _LL596: Cyc_Absyndump_dump(({ unsigned char* _temp867=(
unsigned char*)"null"; struct _tagged_string _temp868; _temp868.curr= _temp867;
_temp868.base= _temp867; _temp868.last_plus_one= _temp867 + 5; _temp868;}));
goto _LL582; _LL598: Cyc_Absyndump_dump_char(( int)'"'); Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_string_escape( _temp713)); Cyc_Absyndump_dump_char(( int)'"'); goto
_LL582; _LL600: _temp719= _temp715; goto _LL602; _LL602: Cyc_Absyndump_dumpqvar(
_temp719); goto _LL582; _LL604: { struct _tagged_string _temp869= Cyc_Absynpp_prim2str(
_temp723); goto _LL870; _LL870: if( ! Cyc_Absyn_is_format_prim( _temp723)){
switch((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp721)){ case 1:
_LL871: if( _temp723 ==( void*) Cyc_Absyn_Size){ Cyc_Absyndump_dumpexp_prec(
myprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp721))->hd);
Cyc_Absyndump_dump(({ unsigned char* _temp873=( unsigned char*)".size"; struct
_tagged_string _temp874; _temp874.curr= _temp873; _temp874.base= _temp873;
_temp874.last_plus_one= _temp873 + 6; _temp874;}));} else{ Cyc_Absyndump_dump(
_temp869); Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp721))->hd);} break; case 2: _LL872: Cyc_Absyndump_dumpexp_prec(
myprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp721))->hd);
Cyc_Absyndump_dump( _temp869); Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp721))->tl))->hd); break; default: _LL875:( void) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp877=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp877[ 0]=({ struct Cyc_Core_Failure_struct
_temp878; _temp878.tag= Cyc_Core_Failure; _temp878.f1=({ unsigned char* _temp879=(
unsigned char*)"Absyndump -- Bad number of arguments to primop"; struct
_tagged_string _temp880; _temp880.curr= _temp879; _temp880.base= _temp879;
_temp880.last_plus_one= _temp879 + 47; _temp880;}); _temp878;}); _temp877;}));}}
else{ Cyc_Absyndump_dump( _temp869); Cyc_Absyndump_dump_nospace(({ unsigned char*
_temp881=( unsigned char*)"("; struct _tagged_string _temp882; _temp882.curr=
_temp881; _temp882.base= _temp881; _temp882.last_plus_one= _temp881 + 2;
_temp882;})); Cyc_Absyndump_dumpexps_prec( 20, _temp721); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp883=( unsigned char*)")"; struct _tagged_string _temp884;
_temp884.curr= _temp883; _temp884.base= _temp883; _temp884.last_plus_one=
_temp883 + 2; _temp884;}));} goto _LL582;} _LL606: Cyc_Absyndump_dumpexp_prec(
myprec, _temp729); if( _temp727 != 0){ Cyc_Absyndump_dump( Cyc_Absynpp_prim2str((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp727))->v));} Cyc_Absyndump_dump_nospace(({
unsigned char* _temp885=( unsigned char*)"="; struct _tagged_string _temp886;
_temp886.curr= _temp885; _temp886.base= _temp885; _temp886.last_plus_one=
_temp885 + 2; _temp886;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp725); goto
_LL582; _LL608: Cyc_Absyndump_dump(({ unsigned char* _temp887=( unsigned char*)"++";
struct _tagged_string _temp888; _temp888.curr= _temp887; _temp888.base= _temp887;
_temp888.last_plus_one= _temp887 + 3; _temp888;})); Cyc_Absyndump_dumpexp_prec(
myprec, _temp733); goto _LL582; _LL610: Cyc_Absyndump_dump(({ unsigned char*
_temp889=( unsigned char*)"--"; struct _tagged_string _temp890; _temp890.curr=
_temp889; _temp890.base= _temp889; _temp890.last_plus_one= _temp889 + 3;
_temp890;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp737); goto _LL582; _LL612:
Cyc_Absyndump_dumpexp_prec( myprec, _temp741); Cyc_Absyndump_dump(({
unsigned char* _temp891=( unsigned char*)"++"; struct _tagged_string _temp892;
_temp892.curr= _temp891; _temp892.base= _temp891; _temp892.last_plus_one=
_temp891 + 3; _temp892;})); goto _LL582; _LL614: Cyc_Absyndump_dumpexp_prec(
myprec, _temp745); Cyc_Absyndump_dump(({ unsigned char* _temp893=( unsigned char*)"--";
struct _tagged_string _temp894; _temp894.curr= _temp893; _temp894.base= _temp893;
_temp894.last_plus_one= _temp893 + 3; _temp894;})); goto _LL582; _LL616: Cyc_Absyndump_dumpexp_prec(
myprec, _temp751); Cyc_Absyndump_dump_char(( int)'?'); Cyc_Absyndump_dumpexp_prec(
0, _temp749); Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp747); goto _LL582; _LL618: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumpexp_prec(
myprec, _temp755); Cyc_Absyndump_dump_char(( int)','); Cyc_Absyndump_dumpexp_prec(
myprec, _temp753); Cyc_Absyndump_dump_char(( int)')'); goto _LL582; _LL620:
_temp765= _temp759; _temp763= _temp757; goto _LL622; _LL622: Cyc_Absyndump_dumpexp_prec(
myprec, _temp765); Cyc_Absyndump_dump_nospace(({ unsigned char* _temp895=(
unsigned char*)"("; struct _tagged_string _temp896; _temp896.curr= _temp895;
_temp896.base= _temp895; _temp896.last_plus_one= _temp895 + 2; _temp896;})); Cyc_Absyndump_dumpexps_prec(
20, _temp763); Cyc_Absyndump_dump_nospace(({ unsigned char* _temp897=(
unsigned char*)")"; struct _tagged_string _temp898; _temp898.curr= _temp897;
_temp898.base= _temp897; _temp898.last_plus_one= _temp897 + 2; _temp898;}));
goto _LL582; _LL624: Cyc_Absyndump_dump(({ unsigned char* _temp899=(
unsigned char*)"throw"; struct _tagged_string _temp900; _temp900.curr= _temp899;
_temp900.base= _temp899; _temp900.last_plus_one= _temp899 + 6; _temp900;})); Cyc_Absyndump_dumpexp_prec(
myprec, _temp767); goto _LL582; _LL626: _temp773= _temp769; goto _LL628; _LL628:
Cyc_Absyndump_dumpexp_prec( inprec, _temp773); goto _LL582; _LL630: Cyc_Absyndump_dump_char((
int)'('); Cyc_Absyndump_dumptyp( _temp777); Cyc_Absyndump_dump_char(( int)')');
Cyc_Absyndump_dumpexp_prec( myprec, _temp775); goto _LL582; _LL632: Cyc_Absyndump_dump_char((
int)'&'); Cyc_Absyndump_dumpexp_prec( myprec, _temp779); goto _LL582; _LL634:
Cyc_Absyndump_dump(({ unsigned char* _temp901=( unsigned char*)"new "; struct
_tagged_string _temp902; _temp902.curr= _temp901; _temp902.base= _temp901;
_temp902.last_plus_one= _temp901 + 5; _temp902;})); Cyc_Absyndump_dumpexp_prec(
myprec, _temp781); goto _LL582; _LL636: Cyc_Absyndump_dump(({ unsigned char*
_temp903=( unsigned char*)"sizeof("; struct _tagged_string _temp904; _temp904.curr=
_temp903; _temp904.base= _temp903; _temp904.last_plus_one= _temp903 + 8;
_temp904;})); Cyc_Absyndump_dumptyp( _temp785); Cyc_Absyndump_dump_char(( int)')');
goto _LL582; _LL638: Cyc_Absyndump_dump(({ unsigned char* _temp905=(
unsigned char*)"sizeof("; struct _tagged_string _temp906; _temp906.curr=
_temp905; _temp906.base= _temp905; _temp906.last_plus_one= _temp905 + 8;
_temp906;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp787); Cyc_Absyndump_dump_char((
int)')'); goto _LL582; _LL640: Cyc_Absyndump_dump(({ unsigned char* _temp907=(
unsigned char*)"offsetof("; struct _tagged_string _temp908; _temp908.curr=
_temp907; _temp908.base= _temp907; _temp908.last_plus_one= _temp907 + 10;
_temp908;})); Cyc_Absyndump_dumptyp( _temp791); Cyc_Absyndump_dump_char(( int)',');
Cyc_Absyndump_dump_nospace(* _temp789); Cyc_Absyndump_dump_char(( int)')'); goto
_LL582; _LL642: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp793); goto _LL582; _LL644: Cyc_Absyndump_dumpexp_prec( myprec,
_temp797); Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp795); goto _LL582; _LL646: Cyc_Absyndump_dumpexp_prec( myprec, _temp801);
Cyc_Absyndump_dump_nospace(({ unsigned char* _temp909=( unsigned char*)"->";
struct _tagged_string _temp910; _temp910.curr= _temp909; _temp910.base= _temp909;
_temp910.last_plus_one= _temp909 + 3; _temp910;})); Cyc_Absyndump_dump_nospace(*
_temp799); goto _LL582; _LL648: Cyc_Absyndump_dumpexp_prec( myprec, _temp805);
Cyc_Absyndump_dump_char(( int)'['); Cyc_Absyndump_dumpexp( _temp803); Cyc_Absyndump_dump_char((
int)']'); goto _LL582; _LL650: Cyc_Absyndump_dump(({ unsigned char* _temp911=(
unsigned char*)"$("; struct _tagged_string _temp912; _temp912.curr= _temp911;
_temp912.base= _temp911; _temp912.last_plus_one= _temp911 + 3; _temp912;})); Cyc_Absyndump_dumpexps_prec(
20, _temp807); Cyc_Absyndump_dump_char(( int)')'); goto _LL582; _LL652: Cyc_Absyndump_dump_char((
int)'('); Cyc_Absyndump_dumptyp((* _temp811).f3); Cyc_Absyndump_dump_char(( int)')');((
void(*)( void(* f)( struct _tuple8*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp809,({ unsigned char* _temp913=( unsigned char*)"{";
struct _tagged_string _temp914; _temp914.curr= _temp913; _temp914.base= _temp913;
_temp914.last_plus_one= _temp913 + 2; _temp914;}),({ unsigned char* _temp915=(
unsigned char*)"}"; struct _tagged_string _temp916; _temp916.curr= _temp915;
_temp916.base= _temp915; _temp916.last_plus_one= _temp915 + 2; _temp916;}),({
unsigned char* _temp917=( unsigned char*)","; struct _tagged_string _temp918;
_temp918.curr= _temp917; _temp918.base= _temp917; _temp918.last_plus_one=
_temp917 + 2; _temp918;})); goto _LL582; _LL654:(( void(*)( void(* f)( struct
_tuple8*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp813,({ unsigned char* _temp919=( unsigned char*)"{"; struct _tagged_string
_temp920; _temp920.curr= _temp919; _temp920.base= _temp919; _temp920.last_plus_one=
_temp919 + 2; _temp920;}),({ unsigned char* _temp921=( unsigned char*)"}";
struct _tagged_string _temp922; _temp922.curr= _temp921; _temp922.base= _temp921;
_temp922.last_plus_one= _temp921 + 2; _temp922;}),({ unsigned char* _temp923=(
unsigned char*)","; struct _tagged_string _temp924; _temp924.curr= _temp923;
_temp924.base= _temp923; _temp924.last_plus_one= _temp923 + 2; _temp924;}));
goto _LL582; _LL656: Cyc_Absyndump_dump(({ unsigned char* _temp925=(
unsigned char*)"new {for"; struct _tagged_string _temp926; _temp926.curr=
_temp925; _temp926.base= _temp925; _temp926.last_plus_one= _temp925 + 9;
_temp926;})); Cyc_Absyndump_dump_str((* _temp819->name).f2); Cyc_Absyndump_dump_char((
int)'<'); Cyc_Absyndump_dumpexp( _temp817); Cyc_Absyndump_dump_char(( int)':');
Cyc_Absyndump_dumpexp( _temp815); Cyc_Absyndump_dump_char(( int)'}'); goto
_LL582; _LL658: Cyc_Absyndump_dumpqvar( _temp827);(( void(*)( void(* f)( struct
_tuple8*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp823,({ unsigned char* _temp927=( unsigned char*)"{"; struct _tagged_string
_temp928; _temp928.curr= _temp927; _temp928.base= _temp927; _temp928.last_plus_one=
_temp927 + 2; _temp928;}),({ unsigned char* _temp929=( unsigned char*)"}";
struct _tagged_string _temp930; _temp930.curr= _temp929; _temp930.base= _temp929;
_temp930.last_plus_one= _temp929 + 2; _temp930;}),({ unsigned char* _temp931=(
unsigned char*)","; struct _tagged_string _temp932; _temp932.curr= _temp931;
_temp932.base= _temp931; _temp932.last_plus_one= _temp931 + 2; _temp932;}));
goto _LL582; _LL660:(( void(*)( void(* f)( struct _tuple8*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde, _temp829,({ unsigned char*
_temp933=( unsigned char*)"{"; struct _tagged_string _temp934; _temp934.curr=
_temp933; _temp934.base= _temp933; _temp934.last_plus_one= _temp933 + 2;
_temp934;}),({ unsigned char* _temp935=( unsigned char*)"}"; struct
_tagged_string _temp936; _temp936.curr= _temp935; _temp936.base= _temp935;
_temp936.last_plus_one= _temp935 + 2; _temp936;}),({ unsigned char* _temp937=(
unsigned char*)","; struct _tagged_string _temp938; _temp938.curr= _temp937;
_temp938.base= _temp937; _temp938.last_plus_one= _temp937 + 2; _temp938;}));
goto _LL582; _LL662: Cyc_Absyndump_dumpqvar( _temp833->name); if( _temp837 != 0){((
void(*)( void(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpexp, _temp837,({ unsigned char* _temp939=( unsigned char*)"(";
struct _tagged_string _temp940; _temp940.curr= _temp939; _temp940.base= _temp939;
_temp940.last_plus_one= _temp939 + 2; _temp940;}),({ unsigned char* _temp941=(
unsigned char*)")"; struct _tagged_string _temp942; _temp942.curr= _temp941;
_temp942.base= _temp941; _temp942.last_plus_one= _temp941 + 2; _temp942;}),({
unsigned char* _temp943=( unsigned char*)","; struct _tagged_string _temp944;
_temp944.curr= _temp943; _temp944.base= _temp943; _temp944.last_plus_one=
_temp943 + 2; _temp944;}));} goto _LL582; _LL664: Cyc_Absyndump_dumpqvar(
_temp847); goto _LL582; _LL666: if( _temp851 != 0){ Cyc_Absyndump_dump(({
unsigned char* _temp945=( unsigned char*)"rmalloc("; struct _tagged_string
_temp946; _temp946.curr= _temp945; _temp946.base= _temp945; _temp946.last_plus_one=
_temp945 + 9; _temp946;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)
_check_null( _temp851)); Cyc_Absyndump_dump(({ unsigned char* _temp947=(
unsigned char*)","; struct _tagged_string _temp948; _temp948.curr= _temp947;
_temp948.base= _temp947; _temp948.last_plus_one= _temp947 + 2; _temp948;}));}
else{ Cyc_Absyndump_dump(({ unsigned char* _temp949=( unsigned char*)"malloc(";
struct _tagged_string _temp950; _temp950.curr= _temp949; _temp950.base= _temp949;
_temp950.last_plus_one= _temp949 + 8; _temp950;}));} Cyc_Absyndump_dump(({
unsigned char* _temp951=( unsigned char*)"sizeof("; struct _tagged_string
_temp952; _temp952.curr= _temp951; _temp952.base= _temp951; _temp952.last_plus_one=
_temp951 + 8; _temp952;})); Cyc_Absyndump_dumptyp( _temp849); Cyc_Absyndump_dump(({
unsigned char* _temp953=( unsigned char*)"))"; struct _tagged_string _temp954;
_temp954.curr= _temp953; _temp954.base= _temp953; _temp954.last_plus_one=
_temp953 + 3; _temp954;})); goto _LL582; _LL668:(( void(*)( void(* f)( struct
_tuple8*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp853,({ unsigned char* _temp955=( unsigned char*)"{"; struct _tagged_string
_temp956; _temp956.curr= _temp955; _temp956.base= _temp955; _temp956.last_plus_one=
_temp955 + 2; _temp956;}),({ unsigned char* _temp957=( unsigned char*)"}";
struct _tagged_string _temp958; _temp958.curr= _temp957; _temp958.base= _temp957;
_temp958.last_plus_one= _temp957 + 2; _temp958;}),({ unsigned char* _temp959=(
unsigned char*)","; struct _tagged_string _temp960; _temp960.curr= _temp959;
_temp960.base= _temp959; _temp960.last_plus_one= _temp959 + 2; _temp960;}));
goto _LL582; _LL670: Cyc_Absyndump_dump_nospace(({ unsigned char* _temp961=(
unsigned char*)"({"; struct _tagged_string _temp962; _temp962.curr= _temp961;
_temp962.base= _temp961; _temp962.last_plus_one= _temp961 + 3; _temp962;})); Cyc_Absyndump_dumpstmt(
_temp857); Cyc_Absyndump_dump_nospace(({ unsigned char* _temp963=( unsigned char*)"})";
struct _tagged_string _temp964; _temp964.curr= _temp963; _temp964.base= _temp963;
_temp964.last_plus_one= _temp963 + 3; _temp964;})); goto _LL582; _LL672: Cyc_Absyndump_dump(({
unsigned char* _temp965=( unsigned char*)"codegen("; struct _tagged_string
_temp966; _temp966.curr= _temp965; _temp966.base= _temp965; _temp966.last_plus_one=
_temp965 + 9; _temp966;})); Cyc_Absyndump_dumpdecl(({ struct Cyc_Absyn_Decl*
_temp967=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp967->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp968=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp968[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp969; _temp969.tag= Cyc_Absyn_Fn_d;
_temp969.f1= _temp859; _temp969;}); _temp968;})); _temp967->loc= e->loc;
_temp967;})); Cyc_Absyndump_dump(({ unsigned char* _temp970=( unsigned char*)")";
struct _tagged_string _temp971; _temp971.curr= _temp970; _temp971.base= _temp970;
_temp971.last_plus_one= _temp970 + 2; _temp971;})); goto _LL582; _LL674: Cyc_Absyndump_dump(({
unsigned char* _temp972=( unsigned char*)"fill("; struct _tagged_string _temp973;
_temp973.curr= _temp972; _temp973.base= _temp972; _temp973.last_plus_one=
_temp972 + 6; _temp973;})); Cyc_Absyndump_dumpexp( _temp861); Cyc_Absyndump_dump(({
unsigned char* _temp974=( unsigned char*)")"; struct _tagged_string _temp975;
_temp975.curr= _temp974; _temp975.base= _temp974; _temp975.last_plus_one=
_temp974 + 2; _temp975;})); goto _LL582; _LL582:;} if( inprec >= myprec){ Cyc_Absyndump_dump_char((
int)')');}} void Cyc_Absyndump_dumpexp( struct Cyc_Absyn_Exp* e){ Cyc_Absyndump_dumpexp_prec(
0, e);} void Cyc_Absyndump_dumpswitchclauses( struct Cyc_List_List* scs){ for( 0;
scs != 0; scs=(( struct Cyc_List_List*) _check_null( scs))->tl){ struct Cyc_Absyn_Switch_clause*
_temp976=( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
scs))->hd; goto _LL977; _LL977: if( _temp976->where_clause == 0?( void*)(
_temp976->pattern)->r ==( void*) Cyc_Absyn_Wild_p: 0){ Cyc_Absyndump_dump(({
unsigned char* _temp978=( unsigned char*)"default:"; struct _tagged_string
_temp979; _temp979.curr= _temp978; _temp979.base= _temp978; _temp979.last_plus_one=
_temp978 + 9; _temp979;}));} else{ Cyc_Absyndump_dump(({ unsigned char* _temp980=(
unsigned char*)"case"; struct _tagged_string _temp981; _temp981.curr= _temp980;
_temp981.base= _temp980; _temp981.last_plus_one= _temp980 + 5; _temp981;})); Cyc_Absyndump_dumppat(
_temp976->pattern); if( _temp976->where_clause != 0){ Cyc_Absyndump_dump(({
unsigned char* _temp982=( unsigned char*)"&&"; struct _tagged_string _temp983;
_temp983.curr= _temp982; _temp983.base= _temp982; _temp983.last_plus_one=
_temp982 + 3; _temp983;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)
_check_null( _temp976->where_clause));} Cyc_Absyndump_dump_nospace(({
unsigned char* _temp984=( unsigned char*)":"; struct _tagged_string _temp985;
_temp985.curr= _temp984; _temp985.base= _temp984; _temp985.last_plus_one=
_temp984 + 2; _temp985;}));} Cyc_Absyndump_dumpstmt( _temp976->body);}} void Cyc_Absyndump_dumpstmt(
struct Cyc_Absyn_Stmt* s){ void* _temp986=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1032; struct Cyc_Absyn_Stmt* _temp1034; struct Cyc_Absyn_Stmt* _temp1036;
struct Cyc_Absyn_Exp* _temp1038; struct Cyc_Absyn_Exp* _temp1040; struct Cyc_Absyn_Stmt*
_temp1042; struct Cyc_Absyn_Stmt* _temp1044; struct Cyc_Absyn_Exp* _temp1046;
struct Cyc_Absyn_Stmt* _temp1048; struct _tuple2 _temp1050; struct Cyc_Absyn_Stmt*
_temp1052; struct Cyc_Absyn_Exp* _temp1054; struct Cyc_Absyn_Stmt* _temp1056;
struct Cyc_Absyn_Stmt* _temp1058; struct Cyc_Absyn_Stmt* _temp1060; struct
_tagged_string* _temp1062; struct Cyc_Absyn_Stmt* _temp1064; struct _tuple2
_temp1066; struct Cyc_Absyn_Stmt* _temp1068; struct Cyc_Absyn_Exp* _temp1070;
struct _tuple2 _temp1072; struct Cyc_Absyn_Stmt* _temp1074; struct Cyc_Absyn_Exp*
_temp1076; struct Cyc_Absyn_Exp* _temp1078; struct Cyc_List_List* _temp1080;
struct Cyc_Absyn_Exp* _temp1082; struct Cyc_Absyn_Stmt* _temp1084; struct Cyc_Absyn_Decl*
_temp1086; struct Cyc_Absyn_Stmt* _temp1088; struct _tagged_string* _temp1090;
struct _tuple2 _temp1092; struct Cyc_Absyn_Stmt* _temp1094; struct Cyc_Absyn_Exp*
_temp1096; struct Cyc_Absyn_Stmt* _temp1098; struct Cyc_List_List* _temp1100;
struct Cyc_Absyn_Exp* _temp1102; struct Cyc_Absyn_Switch_clause** _temp1104;
struct Cyc_List_List* _temp1106; struct Cyc_Absyn_Switch_clause** _temp1108;
struct Cyc_List_List* _temp1110; struct Cyc_List_List* _temp1112; struct Cyc_Absyn_Stmt*
_temp1114; struct Cyc_Absyn_Stmt* _temp1116; struct Cyc_Absyn_Vardecl* _temp1118;
struct Cyc_Absyn_Tvar* _temp1120; struct Cyc_Absyn_Stmt* _temp1122; struct Cyc_Absyn_Stmt*
_temp1124; _LL988: if( _temp986 ==( void*) Cyc_Absyn_Skip_s){ goto _LL989;}
else{ goto _LL990;} _LL990: if(( unsigned int) _temp986 > 1u?*(( int*) _temp986)
== Cyc_Absyn_Exp_s: 0){ _LL1033: _temp1032=(( struct Cyc_Absyn_Exp_s_struct*)
_temp986)->f1; goto _LL991;} else{ goto _LL992;} _LL992: if(( unsigned int)
_temp986 > 1u?*(( int*) _temp986) == Cyc_Absyn_Seq_s: 0){ _LL1037: _temp1036=((
struct Cyc_Absyn_Seq_s_struct*) _temp986)->f1; goto _LL1035; _LL1035: _temp1034=((
struct Cyc_Absyn_Seq_s_struct*) _temp986)->f2; goto _LL993;} else{ goto _LL994;}
_LL994: if(( unsigned int) _temp986 > 1u?*(( int*) _temp986) == Cyc_Absyn_Return_s:
0){ _LL1039: _temp1038=(( struct Cyc_Absyn_Return_s_struct*) _temp986)->f1; if(
_temp1038 == 0){ goto _LL995;} else{ goto _LL996;}} else{ goto _LL996;} _LL996:
if(( unsigned int) _temp986 > 1u?*(( int*) _temp986) == Cyc_Absyn_Return_s: 0){
_LL1041: _temp1040=(( struct Cyc_Absyn_Return_s_struct*) _temp986)->f1; goto
_LL997;} else{ goto _LL998;} _LL998: if(( unsigned int) _temp986 > 1u?*(( int*)
_temp986) == Cyc_Absyn_IfThenElse_s: 0){ _LL1047: _temp1046=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp986)->f1; goto _LL1045; _LL1045: _temp1044=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp986)->f2; goto _LL1043; _LL1043: _temp1042=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp986)->f3; goto _LL999;} else{ goto _LL1000;} _LL1000: if(( unsigned int)
_temp986 > 1u?*(( int*) _temp986) == Cyc_Absyn_While_s: 0){ _LL1051: _temp1050=((
struct Cyc_Absyn_While_s_struct*) _temp986)->f1; _LL1055: _temp1054= _temp1050.f1;
goto _LL1053; _LL1053: _temp1052= _temp1050.f2; goto _LL1049; _LL1049: _temp1048=((
struct Cyc_Absyn_While_s_struct*) _temp986)->f2; goto _LL1001;} else{ goto
_LL1002;} _LL1002: if(( unsigned int) _temp986 > 1u?*(( int*) _temp986) == Cyc_Absyn_Break_s:
0){ _LL1057: _temp1056=(( struct Cyc_Absyn_Break_s_struct*) _temp986)->f1; goto
_LL1003;} else{ goto _LL1004;} _LL1004: if(( unsigned int) _temp986 > 1u?*(( int*)
_temp986) == Cyc_Absyn_Continue_s: 0){ _LL1059: _temp1058=(( struct Cyc_Absyn_Continue_s_struct*)
_temp986)->f1; goto _LL1005;} else{ goto _LL1006;} _LL1006: if(( unsigned int)
_temp986 > 1u?*(( int*) _temp986) == Cyc_Absyn_Goto_s: 0){ _LL1063: _temp1062=((
struct Cyc_Absyn_Goto_s_struct*) _temp986)->f1; goto _LL1061; _LL1061: _temp1060=((
struct Cyc_Absyn_Goto_s_struct*) _temp986)->f2; goto _LL1007;} else{ goto
_LL1008;} _LL1008: if(( unsigned int) _temp986 > 1u?*(( int*) _temp986) == Cyc_Absyn_For_s:
0){ _LL1079: _temp1078=(( struct Cyc_Absyn_For_s_struct*) _temp986)->f1; goto
_LL1073; _LL1073: _temp1072=(( struct Cyc_Absyn_For_s_struct*) _temp986)->f2;
_LL1077: _temp1076= _temp1072.f1; goto _LL1075; _LL1075: _temp1074= _temp1072.f2;
goto _LL1067; _LL1067: _temp1066=(( struct Cyc_Absyn_For_s_struct*) _temp986)->f3;
_LL1071: _temp1070= _temp1066.f1; goto _LL1069; _LL1069: _temp1068= _temp1066.f2;
goto _LL1065; _LL1065: _temp1064=(( struct Cyc_Absyn_For_s_struct*) _temp986)->f4;
goto _LL1009;} else{ goto _LL1010;} _LL1010: if(( unsigned int) _temp986 > 1u?*((
int*) _temp986) == Cyc_Absyn_Switch_s: 0){ _LL1083: _temp1082=(( struct Cyc_Absyn_Switch_s_struct*)
_temp986)->f1; goto _LL1081; _LL1081: _temp1080=(( struct Cyc_Absyn_Switch_s_struct*)
_temp986)->f2; goto _LL1011;} else{ goto _LL1012;} _LL1012: if(( unsigned int)
_temp986 > 1u?*(( int*) _temp986) == Cyc_Absyn_Decl_s: 0){ _LL1087: _temp1086=((
struct Cyc_Absyn_Decl_s_struct*) _temp986)->f1; goto _LL1085; _LL1085: _temp1084=((
struct Cyc_Absyn_Decl_s_struct*) _temp986)->f2; goto _LL1013;} else{ goto
_LL1014;} _LL1014: if(( unsigned int) _temp986 > 1u?*(( int*) _temp986) == Cyc_Absyn_Label_s:
0){ _LL1091: _temp1090=(( struct Cyc_Absyn_Label_s_struct*) _temp986)->f1; goto
_LL1089; _LL1089: _temp1088=(( struct Cyc_Absyn_Label_s_struct*) _temp986)->f2;
goto _LL1015;} else{ goto _LL1016;} _LL1016: if(( unsigned int) _temp986 > 1u?*((
int*) _temp986) == Cyc_Absyn_Do_s: 0){ _LL1099: _temp1098=(( struct Cyc_Absyn_Do_s_struct*)
_temp986)->f1; goto _LL1093; _LL1093: _temp1092=(( struct Cyc_Absyn_Do_s_struct*)
_temp986)->f2; _LL1097: _temp1096= _temp1092.f1; goto _LL1095; _LL1095:
_temp1094= _temp1092.f2; goto _LL1017;} else{ goto _LL1018;} _LL1018: if((
unsigned int) _temp986 > 1u?*(( int*) _temp986) == Cyc_Absyn_SwitchC_s: 0){
_LL1103: _temp1102=(( struct Cyc_Absyn_SwitchC_s_struct*) _temp986)->f1; goto
_LL1101; _LL1101: _temp1100=(( struct Cyc_Absyn_SwitchC_s_struct*) _temp986)->f2;
goto _LL1019;} else{ goto _LL1020;} _LL1020: if(( unsigned int) _temp986 > 1u?*((
int*) _temp986) == Cyc_Absyn_Fallthru_s: 0){ _LL1107: _temp1106=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp986)->f1; if( _temp1106 == 0){ goto _LL1105;} else{ goto _LL1022;} _LL1105:
_temp1104=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp986)->f2; goto _LL1021;}
else{ goto _LL1022;} _LL1022: if(( unsigned int) _temp986 > 1u?*(( int*)
_temp986) == Cyc_Absyn_Fallthru_s: 0){ _LL1111: _temp1110=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp986)->f1; goto _LL1109; _LL1109: _temp1108=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp986)->f2; goto _LL1023;} else{ goto _LL1024;} _LL1024: if(( unsigned int)
_temp986 > 1u?*(( int*) _temp986) == Cyc_Absyn_TryCatch_s: 0){ _LL1115:
_temp1114=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp986)->f1; goto _LL1113;
_LL1113: _temp1112=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp986)->f2; goto
_LL1025;} else{ goto _LL1026;} _LL1026: if(( unsigned int) _temp986 > 1u?*(( int*)
_temp986) == Cyc_Absyn_Region_s: 0){ _LL1121: _temp1120=(( struct Cyc_Absyn_Region_s_struct*)
_temp986)->f1; goto _LL1119; _LL1119: _temp1118=(( struct Cyc_Absyn_Region_s_struct*)
_temp986)->f2; goto _LL1117; _LL1117: _temp1116=(( struct Cyc_Absyn_Region_s_struct*)
_temp986)->f3; goto _LL1027;} else{ goto _LL1028;} _LL1028: if(( unsigned int)
_temp986 > 1u?*(( int*) _temp986) == Cyc_Absyn_Cut_s: 0){ _LL1123: _temp1122=((
struct Cyc_Absyn_Cut_s_struct*) _temp986)->f1; goto _LL1029;} else{ goto _LL1030;}
_LL1030: if(( unsigned int) _temp986 > 1u?*(( int*) _temp986) == Cyc_Absyn_Splice_s:
0){ _LL1125: _temp1124=(( struct Cyc_Absyn_Splice_s_struct*) _temp986)->f1; goto
_LL1031;} else{ goto _LL987;} _LL989: Cyc_Absyndump_dump_semi(); goto _LL987;
_LL991: Cyc_Absyndump_dumpexp( _temp1032); Cyc_Absyndump_dump_semi(); goto
_LL987; _LL993: if( Cyc_Absynpp_is_declaration( _temp1036)){ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpstmt( _temp1036); Cyc_Absyndump_dump_char(( int)'}');}
else{ Cyc_Absyndump_dumpstmt( _temp1036);} if( Cyc_Absynpp_is_declaration(
_temp1034)){ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt(
_temp1034); Cyc_Absyndump_dump_char(( int)'}');} else{ Cyc_Absyndump_dumpstmt(
_temp1034);} goto _LL987; _LL995: Cyc_Absyndump_dump(({ unsigned char* _temp1126=(
unsigned char*)"return;"; struct _tagged_string _temp1127; _temp1127.curr=
_temp1126; _temp1127.base= _temp1126; _temp1127.last_plus_one= _temp1126 + 8;
_temp1127;})); goto _LL987; _LL997: Cyc_Absyndump_dump(({ unsigned char*
_temp1128=( unsigned char*)"return"; struct _tagged_string _temp1129; _temp1129.curr=
_temp1128; _temp1129.base= _temp1128; _temp1129.last_plus_one= _temp1128 + 7;
_temp1129;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null(
_temp1040)); Cyc_Absyndump_dump_semi(); goto _LL987; _LL999: Cyc_Absyndump_dump(({
unsigned char* _temp1130=( unsigned char*)"if("; struct _tagged_string _temp1131;
_temp1131.curr= _temp1130; _temp1131.base= _temp1130; _temp1131.last_plus_one=
_temp1130 + 4; _temp1131;})); Cyc_Absyndump_dumpexp( _temp1046); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp1132=( unsigned char*)"){"; struct _tagged_string _temp1133;
_temp1133.curr= _temp1132; _temp1133.base= _temp1132; _temp1133.last_plus_one=
_temp1132 + 3; _temp1133;})); Cyc_Absyndump_dumpstmt( _temp1044); Cyc_Absyndump_dump_char((
int)'}');{ void* _temp1134=( void*) _temp1042->r; _LL1136: if( _temp1134 ==(
void*) Cyc_Absyn_Skip_s){ goto _LL1137;} else{ goto _LL1138;} _LL1138: goto
_LL1139; _LL1137: goto _LL1135; _LL1139: Cyc_Absyndump_dump(({ unsigned char*
_temp1140=( unsigned char*)"else{"; struct _tagged_string _temp1141; _temp1141.curr=
_temp1140; _temp1141.base= _temp1140; _temp1141.last_plus_one= _temp1140 + 6;
_temp1141;})); Cyc_Absyndump_dumpstmt( _temp1042); Cyc_Absyndump_dump_char(( int)'}');
goto _LL1135; _LL1135:;} goto _LL987; _LL1001: Cyc_Absyndump_dump(({
unsigned char* _temp1142=( unsigned char*)"while("; struct _tagged_string
_temp1143; _temp1143.curr= _temp1142; _temp1143.base= _temp1142; _temp1143.last_plus_one=
_temp1142 + 7; _temp1143;})); Cyc_Absyndump_dumpexp( _temp1054); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp1144=( unsigned char*)") {"; struct _tagged_string _temp1145;
_temp1145.curr= _temp1144; _temp1145.base= _temp1144; _temp1145.last_plus_one=
_temp1144 + 4; _temp1145;})); Cyc_Absyndump_dumpstmt( _temp1048); Cyc_Absyndump_dump_char((
int)'}'); goto _LL987; _LL1003: Cyc_Absyndump_dump(({ unsigned char* _temp1146=(
unsigned char*)"break;"; struct _tagged_string _temp1147; _temp1147.curr=
_temp1146; _temp1147.base= _temp1146; _temp1147.last_plus_one= _temp1146 + 7;
_temp1147;})); goto _LL987; _LL1005: Cyc_Absyndump_dump(({ unsigned char*
_temp1148=( unsigned char*)"continue;"; struct _tagged_string _temp1149;
_temp1149.curr= _temp1148; _temp1149.base= _temp1148; _temp1149.last_plus_one=
_temp1148 + 10; _temp1149;})); goto _LL987; _LL1007: Cyc_Absyndump_dump(({
unsigned char* _temp1150=( unsigned char*)"goto"; struct _tagged_string
_temp1151; _temp1151.curr= _temp1150; _temp1151.base= _temp1150; _temp1151.last_plus_one=
_temp1150 + 5; _temp1151;})); Cyc_Absyndump_dump_str( _temp1062); Cyc_Absyndump_dump_semi();
goto _LL987; _LL1009: Cyc_Absyndump_dump(({ unsigned char* _temp1152=(
unsigned char*)"for("; struct _tagged_string _temp1153; _temp1153.curr=
_temp1152; _temp1153.base= _temp1152; _temp1153.last_plus_one= _temp1152 + 5;
_temp1153;})); Cyc_Absyndump_dumpexp( _temp1078); Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpexp(
_temp1076); Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpexp( _temp1070); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp1154=( unsigned char*)"){"; struct _tagged_string _temp1155;
_temp1155.curr= _temp1154; _temp1155.base= _temp1154; _temp1155.last_plus_one=
_temp1154 + 3; _temp1155;})); Cyc_Absyndump_dumpstmt( _temp1064); Cyc_Absyndump_dump_char((
int)'}'); goto _LL987; _LL1011: Cyc_Absyndump_dump(({ unsigned char* _temp1156=(
unsigned char*)"switch("; struct _tagged_string _temp1157; _temp1157.curr=
_temp1156; _temp1157.base= _temp1156; _temp1157.last_plus_one= _temp1156 + 8;
_temp1157;})); Cyc_Absyndump_dumpexp( _temp1082); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp1158=( unsigned char*)"){"; struct _tagged_string _temp1159;
_temp1159.curr= _temp1158; _temp1159.base= _temp1158; _temp1159.last_plus_one=
_temp1158 + 3; _temp1159;})); Cyc_Absyndump_dumpswitchclauses( _temp1080); Cyc_Absyndump_dump_char((
int)'}'); goto _LL987; _LL1013: Cyc_Absyndump_dumpdecl( _temp1086); Cyc_Absyndump_dumpstmt(
_temp1084); goto _LL987; _LL1015: if( Cyc_Absynpp_is_declaration( _temp1088)){
Cyc_Absyndump_dump_str( _temp1090); Cyc_Absyndump_dump_nospace(({ unsigned char*
_temp1160=( unsigned char*)": {"; struct _tagged_string _temp1161; _temp1161.curr=
_temp1160; _temp1161.base= _temp1160; _temp1161.last_plus_one= _temp1160 + 4;
_temp1161;})); Cyc_Absyndump_dumpstmt( _temp1088); Cyc_Absyndump_dump_char(( int)'}');}
else{ Cyc_Absyndump_dump_str( _temp1090); Cyc_Absyndump_dump_char(( int)':');
Cyc_Absyndump_dumpstmt( _temp1088);} goto _LL987; _LL1017: Cyc_Absyndump_dump(({
unsigned char* _temp1162=( unsigned char*)"do {"; struct _tagged_string
_temp1163; _temp1163.curr= _temp1162; _temp1163.base= _temp1162; _temp1163.last_plus_one=
_temp1162 + 5; _temp1163;})); Cyc_Absyndump_dumpstmt( _temp1098); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp1164=( unsigned char*)"} while ("; struct _tagged_string
_temp1165; _temp1165.curr= _temp1164; _temp1165.base= _temp1164; _temp1165.last_plus_one=
_temp1164 + 10; _temp1165;})); Cyc_Absyndump_dumpexp( _temp1096); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp1166=( unsigned char*)");"; struct _tagged_string _temp1167;
_temp1167.curr= _temp1166; _temp1167.base= _temp1166; _temp1167.last_plus_one=
_temp1166 + 3; _temp1167;})); goto _LL987; _LL1019: Cyc_Absyndump_dump(({
unsigned char* _temp1168=( unsigned char*)"switch \"C\" ("; struct
_tagged_string _temp1169; _temp1169.curr= _temp1168; _temp1169.base= _temp1168;
_temp1169.last_plus_one= _temp1168 + 13; _temp1169;})); Cyc_Absyndump_dumpexp(
_temp1102); Cyc_Absyndump_dump_nospace(({ unsigned char* _temp1170=(
unsigned char*)"){"; struct _tagged_string _temp1171; _temp1171.curr= _temp1170;
_temp1171.base= _temp1170; _temp1171.last_plus_one= _temp1170 + 3; _temp1171;}));
for( 0; _temp1100 != 0; _temp1100=(( struct Cyc_List_List*) _check_null(
_temp1100))->tl){ struct Cyc_Absyn_SwitchC_clause _temp1174; struct Cyc_Position_Segment*
_temp1175; struct Cyc_Absyn_Stmt* _temp1177; struct Cyc_Absyn_Exp* _temp1179;
struct Cyc_Absyn_SwitchC_clause* _temp1172=( struct Cyc_Absyn_SwitchC_clause*)((
struct Cyc_List_List*) _check_null( _temp1100))->hd; _temp1174=* _temp1172;
_LL1180: _temp1179= _temp1174.cnst_exp; goto _LL1178; _LL1178: _temp1177=
_temp1174.body; goto _LL1176; _LL1176: _temp1175= _temp1174.loc; goto _LL1173;
_LL1173: if( _temp1179 == 0){ Cyc_Absyndump_dump(({ unsigned char* _temp1181=(
unsigned char*)"default: "; struct _tagged_string _temp1182; _temp1182.curr=
_temp1181; _temp1182.base= _temp1181; _temp1182.last_plus_one= _temp1181 + 10;
_temp1182;}));} else{ Cyc_Absyndump_dump(({ unsigned char* _temp1183=(
unsigned char*)"case "; struct _tagged_string _temp1184; _temp1184.curr=
_temp1183; _temp1184.base= _temp1183; _temp1184.last_plus_one= _temp1183 + 6;
_temp1184;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null(
_temp1179)); Cyc_Absyndump_dump_char(( int)':');} Cyc_Absyndump_dumpstmt(
_temp1177);} Cyc_Absyndump_dump_char(( int)'}'); goto _LL987; _LL1021: Cyc_Absyndump_dump(({
unsigned char* _temp1185=( unsigned char*)"fallthru;"; struct _tagged_string
_temp1186; _temp1186.curr= _temp1185; _temp1186.base= _temp1185; _temp1186.last_plus_one=
_temp1185 + 10; _temp1186;})); goto _LL987; _LL1023: Cyc_Absyndump_dump(({
unsigned char* _temp1187=( unsigned char*)"fallthru("; struct _tagged_string
_temp1188; _temp1188.curr= _temp1187; _temp1188.base= _temp1187; _temp1188.last_plus_one=
_temp1187 + 10; _temp1188;})); Cyc_Absyndump_dumpexps_prec( 20, _temp1110); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp1189=( unsigned char*)");"; struct _tagged_string _temp1190;
_temp1190.curr= _temp1189; _temp1190.base= _temp1189; _temp1190.last_plus_one=
_temp1189 + 3; _temp1190;})); goto _LL987; _LL1025: Cyc_Absyndump_dump(({
unsigned char* _temp1191=( unsigned char*)"try"; struct _tagged_string _temp1192;
_temp1192.curr= _temp1191; _temp1192.base= _temp1191; _temp1192.last_plus_one=
_temp1191 + 4; _temp1192;})); Cyc_Absyndump_dumpstmt( _temp1114); Cyc_Absyndump_dump(({
unsigned char* _temp1193=( unsigned char*)"catch {"; struct _tagged_string
_temp1194; _temp1194.curr= _temp1193; _temp1194.base= _temp1193; _temp1194.last_plus_one=
_temp1193 + 8; _temp1194;})); Cyc_Absyndump_dumpswitchclauses( _temp1112); Cyc_Absyndump_dump_char((
int)'}'); goto _LL987; _LL1027: Cyc_Absyndump_dump(({ unsigned char* _temp1195=(
unsigned char*)"region<"; struct _tagged_string _temp1196; _temp1196.curr=
_temp1195; _temp1196.base= _temp1195; _temp1196.last_plus_one= _temp1195 + 8;
_temp1196;})); Cyc_Absyndump_dumptvar( _temp1120); Cyc_Absyndump_dump(({
unsigned char* _temp1197=( unsigned char*)"> "; struct _tagged_string _temp1198;
_temp1198.curr= _temp1197; _temp1198.base= _temp1197; _temp1198.last_plus_one=
_temp1197 + 3; _temp1198;})); Cyc_Absyndump_dumpqvar( _temp1118->name); Cyc_Absyndump_dump(({
unsigned char* _temp1199=( unsigned char*)"{"; struct _tagged_string _temp1200;
_temp1200.curr= _temp1199; _temp1200.base= _temp1199; _temp1200.last_plus_one=
_temp1199 + 2; _temp1200;})); Cyc_Absyndump_dumpstmt( _temp1116); Cyc_Absyndump_dump(({
unsigned char* _temp1201=( unsigned char*)"}"; struct _tagged_string _temp1202;
_temp1202.curr= _temp1201; _temp1202.base= _temp1201; _temp1202.last_plus_one=
_temp1201 + 2; _temp1202;})); goto _LL987; _LL1029: Cyc_Absyndump_dump(({
unsigned char* _temp1203=( unsigned char*)"cut"; struct _tagged_string _temp1204;
_temp1204.curr= _temp1203; _temp1204.base= _temp1203; _temp1204.last_plus_one=
_temp1203 + 4; _temp1204;})); Cyc_Absyndump_dumpstmt( _temp1122); goto _LL987;
_LL1031: Cyc_Absyndump_dump(({ unsigned char* _temp1205=( unsigned char*)"splice";
struct _tagged_string _temp1206; _temp1206.curr= _temp1205; _temp1206.base=
_temp1205; _temp1206.last_plus_one= _temp1205 + 7; _temp1206;})); Cyc_Absyndump_dumpstmt(
_temp1124); goto _LL987; _LL987:;} struct _tuple9{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Pat* f2; } ; void Cyc_Absyndump_dumpdp( struct _tuple9* dp){((
void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpdesignator,(* dp).f1,({ unsigned char* _temp1207=(
unsigned char*)""; struct _tagged_string _temp1208; _temp1208.curr= _temp1207;
_temp1208.base= _temp1207; _temp1208.last_plus_one= _temp1207 + 1; _temp1208;}),({
unsigned char* _temp1209=( unsigned char*)"="; struct _tagged_string _temp1210;
_temp1210.curr= _temp1209; _temp1210.base= _temp1209; _temp1210.last_plus_one=
_temp1209 + 2; _temp1210;}),({ unsigned char* _temp1211=( unsigned char*)"=";
struct _tagged_string _temp1212; _temp1212.curr= _temp1211; _temp1212.base=
_temp1211; _temp1212.last_plus_one= _temp1211 + 2; _temp1212;})); Cyc_Absyndump_dumppat((*
dp).f2);} void Cyc_Absyndump_dumppat( struct Cyc_Absyn_Pat* p){ void* _temp1213=(
void*) p->r; int _temp1247; void* _temp1249; int _temp1251; void* _temp1253;
unsigned char _temp1255; struct _tagged_string _temp1257; struct Cyc_Absyn_Vardecl*
_temp1259; struct Cyc_List_List* _temp1261; struct Cyc_Absyn_Pat* _temp1263;
struct Cyc_Absyn_Vardecl* _temp1265; struct _tuple0* _temp1267; struct Cyc_List_List*
_temp1269; struct Cyc_List_List* _temp1271; struct _tuple0* _temp1273; struct
Cyc_List_List* _temp1275; struct Cyc_List_List* _temp1277; struct _tuple0*
_temp1279; struct Cyc_List_List* _temp1281; struct Cyc_List_List* _temp1283;
struct Cyc_Core_Opt* _temp1285; struct Cyc_Absyn_Structdecl* _temp1287; struct
Cyc_List_List* _temp1289; struct Cyc_List_List* _temp1291; struct Cyc_Absyn_Tunionfield*
_temp1293; struct Cyc_Absyn_Tuniondecl* _temp1295; struct Cyc_Absyn_Enumfield*
_temp1297; struct Cyc_Absyn_Enumdecl* _temp1299; _LL1215: if( _temp1213 ==( void*)
Cyc_Absyn_Wild_p){ goto _LL1216;} else{ goto _LL1217;} _LL1217: if( _temp1213 ==(
void*) Cyc_Absyn_Null_p){ goto _LL1218;} else{ goto _LL1219;} _LL1219: if((
unsigned int) _temp1213 > 2u?*(( int*) _temp1213) == Cyc_Absyn_Int_p: 0){
_LL1250: _temp1249=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1213)->f1;
if( _temp1249 ==( void*) Cyc_Absyn_Signed){ goto _LL1248;} else{ goto _LL1221;}
_LL1248: _temp1247=(( struct Cyc_Absyn_Int_p_struct*) _temp1213)->f2; goto
_LL1220;} else{ goto _LL1221;} _LL1221: if(( unsigned int) _temp1213 > 2u?*((
int*) _temp1213) == Cyc_Absyn_Int_p: 0){ _LL1254: _temp1253=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1213)->f1; if( _temp1253 ==( void*) Cyc_Absyn_Unsigned){
goto _LL1252;} else{ goto _LL1223;} _LL1252: _temp1251=(( struct Cyc_Absyn_Int_p_struct*)
_temp1213)->f2; goto _LL1222;} else{ goto _LL1223;} _LL1223: if(( unsigned int)
_temp1213 > 2u?*(( int*) _temp1213) == Cyc_Absyn_Char_p: 0){ _LL1256: _temp1255=((
struct Cyc_Absyn_Char_p_struct*) _temp1213)->f1; goto _LL1224;} else{ goto
_LL1225;} _LL1225: if(( unsigned int) _temp1213 > 2u?*(( int*) _temp1213) == Cyc_Absyn_Float_p:
0){ _LL1258: _temp1257=(( struct Cyc_Absyn_Float_p_struct*) _temp1213)->f1; goto
_LL1226;} else{ goto _LL1227;} _LL1227: if(( unsigned int) _temp1213 > 2u?*((
int*) _temp1213) == Cyc_Absyn_Var_p: 0){ _LL1260: _temp1259=(( struct Cyc_Absyn_Var_p_struct*)
_temp1213)->f1; goto _LL1228;} else{ goto _LL1229;} _LL1229: if(( unsigned int)
_temp1213 > 2u?*(( int*) _temp1213) == Cyc_Absyn_Tuple_p: 0){ _LL1262: _temp1261=((
struct Cyc_Absyn_Tuple_p_struct*) _temp1213)->f1; goto _LL1230;} else{ goto
_LL1231;} _LL1231: if(( unsigned int) _temp1213 > 2u?*(( int*) _temp1213) == Cyc_Absyn_Pointer_p:
0){ _LL1264: _temp1263=(( struct Cyc_Absyn_Pointer_p_struct*) _temp1213)->f1;
goto _LL1232;} else{ goto _LL1233;} _LL1233: if(( unsigned int) _temp1213 > 2u?*((
int*) _temp1213) == Cyc_Absyn_Reference_p: 0){ _LL1266: _temp1265=(( struct Cyc_Absyn_Reference_p_struct*)
_temp1213)->f1; goto _LL1234;} else{ goto _LL1235;} _LL1235: if(( unsigned int)
_temp1213 > 2u?*(( int*) _temp1213) == Cyc_Absyn_UnknownId_p: 0){ _LL1268:
_temp1267=(( struct Cyc_Absyn_UnknownId_p_struct*) _temp1213)->f1; goto _LL1236;}
else{ goto _LL1237;} _LL1237: if(( unsigned int) _temp1213 > 2u?*(( int*)
_temp1213) == Cyc_Absyn_UnknownCall_p: 0){ _LL1274: _temp1273=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1213)->f1; goto _LL1272; _LL1272: _temp1271=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1213)->f2; goto _LL1270; _LL1270: _temp1269=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1213)->f3; goto _LL1238;} else{ goto _LL1239;} _LL1239: if(( unsigned int)
_temp1213 > 2u?*(( int*) _temp1213) == Cyc_Absyn_UnknownFields_p: 0){ _LL1280:
_temp1279=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp1213)->f1; goto
_LL1278; _LL1278: _temp1277=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1213)->f2; goto _LL1276; _LL1276: _temp1275=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1213)->f3; goto _LL1240;} else{ goto _LL1241;} _LL1241: if(( unsigned int)
_temp1213 > 2u?*(( int*) _temp1213) == Cyc_Absyn_Struct_p: 0){ _LL1288:
_temp1287=(( struct Cyc_Absyn_Struct_p_struct*) _temp1213)->f1; goto _LL1286;
_LL1286: _temp1285=(( struct Cyc_Absyn_Struct_p_struct*) _temp1213)->f2; goto
_LL1284; _LL1284: _temp1283=(( struct Cyc_Absyn_Struct_p_struct*) _temp1213)->f3;
goto _LL1282; _LL1282: _temp1281=(( struct Cyc_Absyn_Struct_p_struct*) _temp1213)->f4;
goto _LL1242;} else{ goto _LL1243;} _LL1243: if(( unsigned int) _temp1213 > 2u?*((
int*) _temp1213) == Cyc_Absyn_Tunion_p: 0){ _LL1296: _temp1295=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1213)->f1; goto _LL1294; _LL1294: _temp1293=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1213)->f2; goto _LL1292; _LL1292: _temp1291=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1213)->f3; goto _LL1290; _LL1290: _temp1289=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1213)->f4; goto _LL1244;} else{ goto _LL1245;} _LL1245: if(( unsigned int)
_temp1213 > 2u?*(( int*) _temp1213) == Cyc_Absyn_Enum_p: 0){ _LL1300: _temp1299=((
struct Cyc_Absyn_Enum_p_struct*) _temp1213)->f1; goto _LL1298; _LL1298:
_temp1297=(( struct Cyc_Absyn_Enum_p_struct*) _temp1213)->f2; goto _LL1246;}
else{ goto _LL1214;} _LL1216: Cyc_Absyndump_dump_char(( int)'_'); goto _LL1214;
_LL1218: Cyc_Absyndump_dump(({ unsigned char* _temp1301=( unsigned char*)"null";
struct _tagged_string _temp1302; _temp1302.curr= _temp1301; _temp1302.base=
_temp1301; _temp1302.last_plus_one= _temp1301 + 5; _temp1302;})); goto _LL1214;
_LL1220: Cyc_Absyndump_dump(( struct _tagged_string) xprintf("%d", _temp1247));
goto _LL1214; _LL1222: Cyc_Absyndump_dump(( struct _tagged_string) xprintf("%u",(
unsigned int) _temp1251)); goto _LL1214; _LL1224: Cyc_Absyndump_dump(({
unsigned char* _temp1303=( unsigned char*)"'"; struct _tagged_string _temp1304;
_temp1304.curr= _temp1303; _temp1304.base= _temp1303; _temp1304.last_plus_one=
_temp1303 + 2; _temp1304;})); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_char_escape(
_temp1255)); Cyc_Absyndump_dump_nospace(({ unsigned char* _temp1305=(
unsigned char*)"'"; struct _tagged_string _temp1306; _temp1306.curr= _temp1305;
_temp1306.base= _temp1305; _temp1306.last_plus_one= _temp1305 + 2; _temp1306;}));
goto _LL1214; _LL1226: Cyc_Absyndump_dump( _temp1257); goto _LL1214; _LL1228:
Cyc_Absyndump_dumpqvar( _temp1259->name); goto _LL1214; _LL1230:(( void(*)( void(*
f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat,
_temp1261,({ unsigned char* _temp1307=( unsigned char*)"$("; struct
_tagged_string _temp1308; _temp1308.curr= _temp1307; _temp1308.base= _temp1307;
_temp1308.last_plus_one= _temp1307 + 3; _temp1308;}),({ unsigned char* _temp1309=(
unsigned char*)")"; struct _tagged_string _temp1310; _temp1310.curr= _temp1309;
_temp1310.base= _temp1309; _temp1310.last_plus_one= _temp1309 + 2; _temp1310;}),({
unsigned char* _temp1311=( unsigned char*)","; struct _tagged_string _temp1312;
_temp1312.curr= _temp1311; _temp1312.base= _temp1311; _temp1312.last_plus_one=
_temp1311 + 2; _temp1312;})); goto _LL1214; _LL1232: Cyc_Absyndump_dump(({
unsigned char* _temp1313=( unsigned char*)"&"; struct _tagged_string _temp1314;
_temp1314.curr= _temp1313; _temp1314.base= _temp1313; _temp1314.last_plus_one=
_temp1313 + 2; _temp1314;})); Cyc_Absyndump_dumppat( _temp1263); goto _LL1214;
_LL1234: Cyc_Absyndump_dump(({ unsigned char* _temp1315=( unsigned char*)"*";
struct _tagged_string _temp1316; _temp1316.curr= _temp1315; _temp1316.base=
_temp1315; _temp1316.last_plus_one= _temp1315 + 2; _temp1316;})); Cyc_Absyndump_dumpqvar(
_temp1265->name); goto _LL1214; _LL1236: Cyc_Absyndump_dumpqvar( _temp1267);
goto _LL1214; _LL1238: Cyc_Absyndump_dumpqvar( _temp1273); Cyc_Absyndump_dumptvars(
_temp1271);(( void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp1269,({ unsigned char*
_temp1317=( unsigned char*)"("; struct _tagged_string _temp1318; _temp1318.curr=
_temp1317; _temp1318.base= _temp1317; _temp1318.last_plus_one= _temp1317 + 2;
_temp1318;}),({ unsigned char* _temp1319=( unsigned char*)")"; struct
_tagged_string _temp1320; _temp1320.curr= _temp1319; _temp1320.base= _temp1319;
_temp1320.last_plus_one= _temp1319 + 2; _temp1320;}),({ unsigned char* _temp1321=(
unsigned char*)","; struct _tagged_string _temp1322; _temp1322.curr= _temp1321;
_temp1322.base= _temp1321; _temp1322.last_plus_one= _temp1321 + 2; _temp1322;}));
goto _LL1214; _LL1240: Cyc_Absyndump_dumpqvar( _temp1279); Cyc_Absyndump_dumptvars(
_temp1277);(( void(*)( void(* f)( struct _tuple9*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpdp, _temp1275,({ unsigned char*
_temp1323=( unsigned char*)"{"; struct _tagged_string _temp1324; _temp1324.curr=
_temp1323; _temp1324.base= _temp1323; _temp1324.last_plus_one= _temp1323 + 2;
_temp1324;}),({ unsigned char* _temp1325=( unsigned char*)"}"; struct
_tagged_string _temp1326; _temp1326.curr= _temp1325; _temp1326.base= _temp1325;
_temp1326.last_plus_one= _temp1325 + 2; _temp1326;}),({ unsigned char* _temp1327=(
unsigned char*)","; struct _tagged_string _temp1328; _temp1328.curr= _temp1327;
_temp1328.base= _temp1327; _temp1328.last_plus_one= _temp1327 + 2; _temp1328;}));
goto _LL1214; _LL1242: if( _temp1287->name != 0){ Cyc_Absyndump_dumpqvar((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp1287->name))->v);}
Cyc_Absyndump_dumptvars( _temp1283);(( void(*)( void(* f)( struct _tuple9*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpdp,
_temp1281,({ unsigned char* _temp1329=( unsigned char*)"{"; struct
_tagged_string _temp1330; _temp1330.curr= _temp1329; _temp1330.base= _temp1329;
_temp1330.last_plus_one= _temp1329 + 2; _temp1330;}),({ unsigned char* _temp1331=(
unsigned char*)"}"; struct _tagged_string _temp1332; _temp1332.curr= _temp1331;
_temp1332.base= _temp1331; _temp1332.last_plus_one= _temp1331 + 2; _temp1332;}),({
unsigned char* _temp1333=( unsigned char*)","; struct _tagged_string _temp1334;
_temp1334.curr= _temp1333; _temp1334.base= _temp1333; _temp1334.last_plus_one=
_temp1333 + 2; _temp1334;})); goto _LL1214; _LL1244: Cyc_Absyndump_dumpqvar(
_temp1293->name); Cyc_Absyndump_dumptvars( _temp1291); if( _temp1289 != 0){((
void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat, _temp1289,({ unsigned char* _temp1335=( unsigned char*)"(";
struct _tagged_string _temp1336; _temp1336.curr= _temp1335; _temp1336.base=
_temp1335; _temp1336.last_plus_one= _temp1335 + 2; _temp1336;}),({ unsigned char*
_temp1337=( unsigned char*)")"; struct _tagged_string _temp1338; _temp1338.curr=
_temp1337; _temp1338.base= _temp1337; _temp1338.last_plus_one= _temp1337 + 2;
_temp1338;}),({ unsigned char* _temp1339=( unsigned char*)","; struct
_tagged_string _temp1340; _temp1340.curr= _temp1339; _temp1340.base= _temp1339;
_temp1340.last_plus_one= _temp1339 + 2; _temp1340;}));} goto _LL1214; _LL1246:
Cyc_Absyndump_dumpqvar( _temp1297->name); goto _LL1214; _LL1214:;} void Cyc_Absyndump_dumptunionfield(
struct Cyc_Absyn_Tunionfield* ef){ Cyc_Absyndump_dumpqvar( ef->name); if( ef->typs
!= 0){ Cyc_Absyndump_dumpargs( ef->typs);}} void Cyc_Absyndump_dumptunionfields(
struct Cyc_List_List* fields){(( void(*)( void(* f)( struct Cyc_Absyn_Tunionfield*),
struct Cyc_List_List* l, struct _tagged_string sep)) Cyc_Absyndump_dump_sep)(
Cyc_Absyndump_dumptunionfield, fields,({ unsigned char* _temp1341=(
unsigned char*)","; struct _tagged_string _temp1342; _temp1342.curr= _temp1341;
_temp1342.base= _temp1341; _temp1342.last_plus_one= _temp1341 + 2; _temp1342;}));}
void Cyc_Absyndump_dumpenumfield( struct Cyc_Absyn_Enumfield* ef){ Cyc_Absyndump_dumpqvar(
ef->name); if( ef->tag != 0){ Cyc_Absyndump_dump(({ unsigned char* _temp1343=(
unsigned char*)" = "; struct _tagged_string _temp1344; _temp1344.curr= _temp1343;
_temp1344.base= _temp1343; _temp1344.last_plus_one= _temp1343 + 4; _temp1344;}));
Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null( ef->tag));}} void
Cyc_Absyndump_dumpenumfields( struct Cyc_List_List* fields){(( void(*)( void(* f)(
struct Cyc_Absyn_Enumfield*), struct Cyc_List_List* l, struct _tagged_string sep))
Cyc_Absyndump_dump_sep)( Cyc_Absyndump_dumpenumfield, fields,({ unsigned char*
_temp1345=( unsigned char*)","; struct _tagged_string _temp1346; _temp1346.curr=
_temp1345; _temp1346.base= _temp1345; _temp1346.last_plus_one= _temp1345 + 2;
_temp1346;}));} void Cyc_Absyndump_dumpstructfields( struct Cyc_List_List*
fields){ for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null(
fields))->tl){ struct Cyc_Absyn_Structfield _temp1349; struct Cyc_List_List*
_temp1350; struct Cyc_Core_Opt* _temp1352; void* _temp1354; struct Cyc_Absyn_Tqual
_temp1356; struct _tagged_string* _temp1358; struct Cyc_Absyn_Structfield*
_temp1347=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd; _temp1349=* _temp1347; _LL1359: _temp1358= _temp1349.name; goto
_LL1357; _LL1357: _temp1356= _temp1349.tq; goto _LL1355; _LL1355: _temp1354=(
void*) _temp1349.type; goto _LL1353; _LL1353: _temp1352= _temp1349.width; goto
_LL1351; _LL1351: _temp1350= _temp1349.attributes; goto _LL1348; _LL1348:(( void(*)(
struct Cyc_Absyn_Tqual, void*, void(* f)( struct _tagged_string*), struct
_tagged_string*)) Cyc_Absyndump_dumptqtd)( _temp1356, _temp1354, Cyc_Absyndump_dump_str,
_temp1358); Cyc_Absyndump_dumpatts( _temp1350); if( _temp1352 != 0){ Cyc_Absyndump_dump_char((
int)':'); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*)
_check_null( _temp1352))->v);} Cyc_Absyndump_dump_semi();}} void Cyc_Absyndump_dumptypedefname(
struct Cyc_Absyn_Typedefdecl* td){ Cyc_Absyndump_dumpqvar( td->name); Cyc_Absyndump_dumptvars(
td->tvs);} static void Cyc_Absyndump_dump_atts_qvar( struct Cyc_Absyn_Fndecl* fd){
Cyc_Absyndump_dumpatts( fd->attributes); Cyc_Absyndump_dumpqvar( fd->name);}
struct _tuple10{ void* f1; struct _tuple0* f2; } ; static void Cyc_Absyndump_dump_callconv_qvar(
struct _tuple10* pr){{ void* _temp1360=(* pr).f1; _LL1362: if( _temp1360 ==(
void*) Cyc_Absyn_Unused_att){ goto _LL1363;} else{ goto _LL1364;} _LL1364: if(
_temp1360 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL1365;} else{ goto _LL1366;}
_LL1366: if( _temp1360 ==( void*) Cyc_Absyn_Cdecl_att){ goto _LL1367;} else{
goto _LL1368;} _LL1368: if( _temp1360 ==( void*) Cyc_Absyn_Fastcall_att){ goto
_LL1369;} else{ goto _LL1370;} _LL1370: goto _LL1371; _LL1363: goto _LL1361;
_LL1365: Cyc_Absyndump_dump(({ unsigned char* _temp1372=( unsigned char*)"_stdcall";
struct _tagged_string _temp1373; _temp1373.curr= _temp1372; _temp1373.base=
_temp1372; _temp1373.last_plus_one= _temp1372 + 9; _temp1373;})); goto _LL1361;
_LL1367: Cyc_Absyndump_dump(({ unsigned char* _temp1374=( unsigned char*)"_cdecl";
struct _tagged_string _temp1375; _temp1375.curr= _temp1374; _temp1375.base=
_temp1374; _temp1375.last_plus_one= _temp1374 + 7; _temp1375;})); goto _LL1361;
_LL1369: Cyc_Absyndump_dump(({ unsigned char* _temp1376=( unsigned char*)"_fastcall";
struct _tagged_string _temp1377; _temp1377.curr= _temp1376; _temp1377.base=
_temp1376; _temp1377.last_plus_one= _temp1376 + 10; _temp1377;})); goto _LL1361;
_LL1371: goto _LL1361; _LL1361:;} Cyc_Absyndump_dumpqvar((* pr).f2);} static
void Cyc_Absyndump_dump_callconv_fdqvar( struct Cyc_Absyn_Fndecl* fd){ Cyc_Absyndump_dump_callconv(
fd->attributes); Cyc_Absyndump_dumpqvar( fd->name);} static void Cyc_Absyndump_dumpids(
struct Cyc_List_List* vds){ for( 0; vds != 0; vds=(( struct Cyc_List_List*)
_check_null( vds))->tl){ Cyc_Absyndump_dumpqvar((( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( vds))->hd)->name); if((( struct Cyc_List_List*)
_check_null( vds))->tl != 0){ Cyc_Absyndump_dump_char(( int)',');}}} void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl* d){ void* _temp1378=( void*) d->r; struct Cyc_Absyn_Fndecl*
_temp1404; struct Cyc_Absyn_Structdecl* _temp1406; struct Cyc_Absyn_Uniondecl*
_temp1408; struct Cyc_Absyn_Vardecl* _temp1410; struct Cyc_Absyn_Vardecl
_temp1412; struct Cyc_List_List* _temp1413; struct Cyc_Core_Opt* _temp1415;
struct Cyc_Absyn_Exp* _temp1417; void* _temp1419; struct Cyc_Absyn_Tqual
_temp1421; struct _tuple0* _temp1423; void* _temp1425; struct Cyc_Absyn_Tuniondecl*
_temp1427; struct Cyc_Absyn_Tuniondecl _temp1429; int _temp1430; struct Cyc_Core_Opt*
_temp1432; struct Cyc_List_List* _temp1434; struct _tuple0* _temp1436; void*
_temp1438; struct Cyc_Absyn_Enumdecl* _temp1440; struct Cyc_Absyn_Enumdecl
_temp1442; struct Cyc_Core_Opt* _temp1443; struct _tuple0* _temp1445; void*
_temp1447; int _temp1449; struct Cyc_Absyn_Exp* _temp1451; struct Cyc_Core_Opt*
_temp1453; struct Cyc_Core_Opt* _temp1455; struct Cyc_Absyn_Pat* _temp1457;
struct Cyc_List_List* _temp1459; struct Cyc_Absyn_Typedefdecl* _temp1461; struct
Cyc_List_List* _temp1463; struct _tagged_string* _temp1465; struct Cyc_List_List*
_temp1467; struct _tuple0* _temp1469; struct Cyc_List_List* _temp1471; _LL1380:
if(*(( int*) _temp1378) == Cyc_Absyn_Fn_d){ _LL1405: _temp1404=(( struct Cyc_Absyn_Fn_d_struct*)
_temp1378)->f1; goto _LL1381;} else{ goto _LL1382;} _LL1382: if(*(( int*)
_temp1378) == Cyc_Absyn_Struct_d){ _LL1407: _temp1406=(( struct Cyc_Absyn_Struct_d_struct*)
_temp1378)->f1; goto _LL1383;} else{ goto _LL1384;} _LL1384: if(*(( int*)
_temp1378) == Cyc_Absyn_Union_d){ _LL1409: _temp1408=(( struct Cyc_Absyn_Union_d_struct*)
_temp1378)->f1; goto _LL1385;} else{ goto _LL1386;} _LL1386: if(*(( int*)
_temp1378) == Cyc_Absyn_Var_d){ _LL1411: _temp1410=(( struct Cyc_Absyn_Var_d_struct*)
_temp1378)->f1; _temp1412=* _temp1410; _LL1426: _temp1425=( void*) _temp1412.sc;
goto _LL1424; _LL1424: _temp1423= _temp1412.name; goto _LL1422; _LL1422:
_temp1421= _temp1412.tq; goto _LL1420; _LL1420: _temp1419=( void*) _temp1412.type;
goto _LL1418; _LL1418: _temp1417= _temp1412.initializer; goto _LL1416; _LL1416:
_temp1415= _temp1412.rgn; goto _LL1414; _LL1414: _temp1413= _temp1412.attributes;
goto _LL1387;} else{ goto _LL1388;} _LL1388: if(*(( int*) _temp1378) == Cyc_Absyn_Tunion_d){
_LL1428: _temp1427=(( struct Cyc_Absyn_Tunion_d_struct*) _temp1378)->f1;
_temp1429=* _temp1427; _LL1439: _temp1438=( void*) _temp1429.sc; goto _LL1437;
_LL1437: _temp1436= _temp1429.name; goto _LL1435; _LL1435: _temp1434= _temp1429.tvs;
goto _LL1433; _LL1433: _temp1432= _temp1429.fields; goto _LL1431; _LL1431:
_temp1430= _temp1429.is_xtunion; goto _LL1389;} else{ goto _LL1390;} _LL1390:
if(*(( int*) _temp1378) == Cyc_Absyn_Enum_d){ _LL1441: _temp1440=(( struct Cyc_Absyn_Enum_d_struct*)
_temp1378)->f1; _temp1442=* _temp1440; _LL1448: _temp1447=( void*) _temp1442.sc;
goto _LL1446; _LL1446: _temp1445= _temp1442.name; goto _LL1444; _LL1444:
_temp1443= _temp1442.fields; goto _LL1391;} else{ goto _LL1392;} _LL1392: if(*((
int*) _temp1378) == Cyc_Absyn_Let_d){ _LL1458: _temp1457=(( struct Cyc_Absyn_Let_d_struct*)
_temp1378)->f1; goto _LL1456; _LL1456: _temp1455=(( struct Cyc_Absyn_Let_d_struct*)
_temp1378)->f2; goto _LL1454; _LL1454: _temp1453=(( struct Cyc_Absyn_Let_d_struct*)
_temp1378)->f3; goto _LL1452; _LL1452: _temp1451=(( struct Cyc_Absyn_Let_d_struct*)
_temp1378)->f4; goto _LL1450; _LL1450: _temp1449=(( struct Cyc_Absyn_Let_d_struct*)
_temp1378)->f5; goto _LL1393;} else{ goto _LL1394;} _LL1394: if(*(( int*)
_temp1378) == Cyc_Absyn_Letv_d){ _LL1460: _temp1459=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1378)->f1; goto _LL1395;} else{ goto _LL1396;} _LL1396: if(*(( int*)
_temp1378) == Cyc_Absyn_Typedef_d){ _LL1462: _temp1461=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp1378)->f1; goto _LL1397;} else{ goto _LL1398;} _LL1398: if(*(( int*)
_temp1378) == Cyc_Absyn_Namespace_d){ _LL1466: _temp1465=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1378)->f1; goto _LL1464; _LL1464: _temp1463=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1378)->f2; goto _LL1399;} else{ goto _LL1400;} _LL1400: if(*(( int*)
_temp1378) == Cyc_Absyn_Using_d){ _LL1470: _temp1469=(( struct Cyc_Absyn_Using_d_struct*)
_temp1378)->f1; goto _LL1468; _LL1468: _temp1467=(( struct Cyc_Absyn_Using_d_struct*)
_temp1378)->f2; goto _LL1401;} else{ goto _LL1402;} _LL1402: if(*(( int*)
_temp1378) == Cyc_Absyn_ExternC_d){ _LL1472: _temp1471=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp1378)->f1; goto _LL1403;} else{ goto _LL1379;} _LL1381: if( Cyc_Absyndump_to_VC){
Cyc_Absyndump_dumpatts( _temp1404->attributes);} if( _temp1404->is_inline){ if(
Cyc_Absyndump_to_VC){ Cyc_Absyndump_dump(({ unsigned char* _temp1473=(
unsigned char*)"__inline"; struct _tagged_string _temp1474; _temp1474.curr=
_temp1473; _temp1474.base= _temp1473; _temp1474.last_plus_one= _temp1473 + 9;
_temp1474;}));} else{ Cyc_Absyndump_dump(({ unsigned char* _temp1475=(
unsigned char*)"inline"; struct _tagged_string _temp1476; _temp1476.curr=
_temp1475; _temp1476.base= _temp1475; _temp1476.last_plus_one= _temp1475 + 7;
_temp1476;}));}} Cyc_Absyndump_dumpscope(( void*) _temp1404->sc);{ void* t=(
void*)({ struct Cyc_Absyn_FnType_struct* _temp1478=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1478[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1479; _temp1479.tag= Cyc_Absyn_FnType; _temp1479.f1=({ struct Cyc_Absyn_FnInfo
_temp1480; _temp1480.tvars= _temp1404->tvs; _temp1480.effect= _temp1404->effect;
_temp1480.ret_typ=( void*)(( void*) _temp1404->ret_type); _temp1480.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp1404->args); _temp1480.c_varargs=
_temp1404->c_varargs; _temp1480.cyc_varargs= _temp1404->cyc_varargs; _temp1480.rgn_po=
_temp1404->rgn_po; _temp1480.attributes= 0; _temp1480;}); _temp1479;});
_temp1478;});(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct Cyc_Absyn_Fndecl*),
struct Cyc_Absyn_Fndecl*)) Cyc_Absyndump_dumptqtd)(({ struct Cyc_Absyn_Tqual
_temp1477; _temp1477.q_const= 0; _temp1477.q_volatile= 0; _temp1477.q_restrict=
0; _temp1477;}), t, Cyc_Absyndump_to_VC? Cyc_Absyndump_dump_callconv_fdqvar: Cyc_Absyndump_dump_atts_qvar,
_temp1404); Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt(
_temp1404->body); Cyc_Absyndump_dump_char(( int)'}'); goto _LL1379;} _LL1383:
Cyc_Absyndump_dumpscope(( void*) _temp1406->sc); Cyc_Absyndump_dump(({
unsigned char* _temp1481=( unsigned char*)"struct"; struct _tagged_string
_temp1482; _temp1482.curr= _temp1481; _temp1482.base= _temp1481; _temp1482.last_plus_one=
_temp1481 + 7; _temp1482;})); if( _temp1406->name != 0){ Cyc_Absyndump_dumpqvar((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp1406->name))->v);}
Cyc_Absyndump_dumptvars( _temp1406->tvs); if( _temp1406->fields == 0){ Cyc_Absyndump_dump_semi();}
else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstructfields((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1406->fields))->v);
Cyc_Absyndump_dump(({ unsigned char* _temp1483=( unsigned char*)"}"; struct
_tagged_string _temp1484; _temp1484.curr= _temp1483; _temp1484.base= _temp1483;
_temp1484.last_plus_one= _temp1483 + 2; _temp1484;})); Cyc_Absyndump_dumpatts(
_temp1406->attributes); Cyc_Absyndump_dump(({ unsigned char* _temp1485=(
unsigned char*)";"; struct _tagged_string _temp1486; _temp1486.curr= _temp1485;
_temp1486.base= _temp1485; _temp1486.last_plus_one= _temp1485 + 2; _temp1486;}));}
goto _LL1379; _LL1385: Cyc_Absyndump_dumpscope(( void*) _temp1408->sc); Cyc_Absyndump_dump(({
unsigned char* _temp1487=( unsigned char*)"union"; struct _tagged_string
_temp1488; _temp1488.curr= _temp1487; _temp1488.base= _temp1487; _temp1488.last_plus_one=
_temp1487 + 6; _temp1488;})); if( _temp1408->name != 0){ Cyc_Absyndump_dumpqvar((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp1408->name))->v);}
Cyc_Absyndump_dumptvars( _temp1408->tvs); if( _temp1408->fields == 0){ Cyc_Absyndump_dump_semi();}
else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstructfields((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1408->fields))->v);
Cyc_Absyndump_dump(({ unsigned char* _temp1489=( unsigned char*)"}"; struct
_tagged_string _temp1490; _temp1490.curr= _temp1489; _temp1490.base= _temp1489;
_temp1490.last_plus_one= _temp1489 + 2; _temp1490;})); Cyc_Absyndump_dumpatts(
_temp1408->attributes); Cyc_Absyndump_dump(({ unsigned char* _temp1491=(
unsigned char*)";"; struct _tagged_string _temp1492; _temp1492.curr= _temp1491;
_temp1492.base= _temp1491; _temp1492.last_plus_one= _temp1491 + 2; _temp1492;}));}
goto _LL1379; _LL1387: if( Cyc_Absyndump_to_VC){ Cyc_Absyndump_dumpatts(
_temp1413); Cyc_Absyndump_dumpscope( _temp1425);{ struct Cyc_List_List*
_temp1495; void* _temp1497; struct Cyc_Absyn_Tqual _temp1499; struct _tuple4
_temp1493= Cyc_Absynpp_to_tms( _temp1421, _temp1419); _LL1500: _temp1499=
_temp1493.f1; goto _LL1498; _LL1498: _temp1497= _temp1493.f2; goto _LL1496;
_LL1496: _temp1495= _temp1493.f3; goto _LL1494; _LL1494: Cyc_Absyndump_dumptq(
_temp1499);{ void* call_conv=( void*) Cyc_Absyn_Unused_att;{ struct Cyc_List_List*
tms2= _temp1495; for( 0; tms2 != 0; tms2=(( struct Cyc_List_List*) _check_null(
tms2))->tl){ void* _temp1501=( void*)(( struct Cyc_List_List*) _check_null( tms2))->hd;
struct Cyc_List_List* _temp1507; struct Cyc_Position_Segment* _temp1509; _LL1503:
if(( unsigned int) _temp1501 > 1u?*(( int*) _temp1501) == Cyc_Absyn_Attributes_mod:
0){ _LL1510: _temp1509=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp1501)->f1;
goto _LL1508; _LL1508: _temp1507=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp1501)->f2; goto _LL1504;} else{ goto _LL1505;} _LL1505: goto _LL1506;
_LL1504: for( 0; _temp1507 != 0; _temp1507=(( struct Cyc_List_List*) _check_null(
_temp1507))->tl){ void* _temp1511=( void*)(( struct Cyc_List_List*) _check_null(
_temp1507))->hd; _LL1513: if( _temp1511 ==( void*) Cyc_Absyn_Stdcall_att){ goto
_LL1514;} else{ goto _LL1515;} _LL1515: if( _temp1511 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL1516;} else{ goto _LL1517;} _LL1517: if( _temp1511 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL1518;} else{ goto _LL1519;} _LL1519: goto _LL1520; _LL1514: call_conv=(
void*) Cyc_Absyn_Stdcall_att; goto _LL1512; _LL1516: call_conv=( void*) Cyc_Absyn_Cdecl_att;
goto _LL1512; _LL1518: call_conv=( void*) Cyc_Absyn_Fastcall_att; goto _LL1512;
_LL1520: goto _LL1512; _LL1512:;} goto _LL1502; _LL1506: goto _LL1502; _LL1502:;}}
Cyc_Absyndump_dumptq( _temp1499); Cyc_Absyndump_dumpntyp( _temp1497);{ struct
_tuple10 _temp1522=({ struct _tuple10 _temp1521; _temp1521.f1= call_conv;
_temp1521.f2= _temp1423; _temp1521;}); goto _LL1523; _LL1523:(( void(*)( struct
Cyc_List_List* tms, void(* f)( struct _tuple10*), struct _tuple10* a)) Cyc_Absyndump_dumptms)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1495),
Cyc_Absyndump_dump_callconv_qvar,& _temp1522);}}}} else{ Cyc_Absyndump_dumpscope(
_temp1425);(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct _tuple0*),
struct _tuple0*)) Cyc_Absyndump_dumptqtd)( _temp1421, _temp1419, Cyc_Absyndump_dumpqvar,
_temp1423); Cyc_Absyndump_dumpatts( _temp1413);} if( _temp1417 != 0){ Cyc_Absyndump_dump_char((
int)'='); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null( _temp1417));}
Cyc_Absyndump_dump_semi(); goto _LL1379; _LL1389: Cyc_Absyndump_dumpscope(
_temp1438); if( _temp1430){ Cyc_Absyndump_dump(({ unsigned char* _temp1524=(
unsigned char*)"xtunion "; struct _tagged_string _temp1525; _temp1525.curr=
_temp1524; _temp1525.base= _temp1524; _temp1525.last_plus_one= _temp1524 + 9;
_temp1525;}));} else{ Cyc_Absyndump_dump(({ unsigned char* _temp1526=(
unsigned char*)"tunion "; struct _tagged_string _temp1527; _temp1527.curr=
_temp1526; _temp1527.base= _temp1526; _temp1527.last_plus_one= _temp1526 + 8;
_temp1527;}));} Cyc_Absyndump_dumpqvar( _temp1436); Cyc_Absyndump_dumptvars(
_temp1434); if( _temp1432 == 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumptunionfields(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1432))->v); Cyc_Absyndump_dump_nospace(({ unsigned char*
_temp1528=( unsigned char*)"};"; struct _tagged_string _temp1529; _temp1529.curr=
_temp1528; _temp1529.base= _temp1528; _temp1529.last_plus_one= _temp1528 + 3;
_temp1529;}));} goto _LL1379; _LL1391: Cyc_Absyndump_dumpscope( _temp1447); Cyc_Absyndump_dump(({
unsigned char* _temp1530=( unsigned char*)"enum "; struct _tagged_string
_temp1531; _temp1531.curr= _temp1530; _temp1531.base= _temp1530; _temp1531.last_plus_one=
_temp1530 + 6; _temp1531;})); Cyc_Absyndump_dumpqvar( _temp1445); if( _temp1443
== 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpenumfields((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1443))->v); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp1532=( unsigned char*)"};"; struct _tagged_string _temp1533;
_temp1533.curr= _temp1532; _temp1533.base= _temp1532; _temp1533.last_plus_one=
_temp1532 + 3; _temp1533;}));} return; _LL1393: Cyc_Absyndump_dump(({
unsigned char* _temp1534=( unsigned char*)"let"; struct _tagged_string _temp1535;
_temp1535.curr= _temp1534; _temp1535.base= _temp1534; _temp1535.last_plus_one=
_temp1534 + 4; _temp1535;})); Cyc_Absyndump_dumppat( _temp1457); Cyc_Absyndump_dump_char((
int)'='); Cyc_Absyndump_dumpexp( _temp1451); Cyc_Absyndump_dump_semi(); goto
_LL1379; _LL1395: Cyc_Absyndump_dump(({ unsigned char* _temp1536=( unsigned char*)"let ";
struct _tagged_string _temp1537; _temp1537.curr= _temp1536; _temp1537.base=
_temp1536; _temp1537.last_plus_one= _temp1536 + 5; _temp1537;})); Cyc_Absyndump_dumpids(
_temp1459); Cyc_Absyndump_dump_semi(); goto _LL1379; _LL1397: if( ! Cyc_Absyndump_expand_typedefs){
Cyc_Absyndump_dump(({ unsigned char* _temp1538=( unsigned char*)"typedef";
struct _tagged_string _temp1539; _temp1539.curr= _temp1538; _temp1539.base=
_temp1538; _temp1539.last_plus_one= _temp1538 + 8; _temp1539;}));(( void(*)(
struct Cyc_Absyn_Tqual, void*, void(* f)( struct Cyc_Absyn_Typedefdecl*), struct
Cyc_Absyn_Typedefdecl*)) Cyc_Absyndump_dumptqtd)(({ struct Cyc_Absyn_Tqual
_temp1540; _temp1540.q_const= 0; _temp1540.q_volatile= 0; _temp1540.q_restrict=
0; _temp1540;}),( void*) _temp1461->defn, Cyc_Absyndump_dumptypedefname,
_temp1461); Cyc_Absyndump_dump_semi();} goto _LL1379; _LL1399: Cyc_Absyndump_dump(({
unsigned char* _temp1541=( unsigned char*)"namespace"; struct _tagged_string
_temp1542; _temp1542.curr= _temp1541; _temp1542.base= _temp1541; _temp1542.last_plus_one=
_temp1541 + 10; _temp1542;})); Cyc_Absyndump_dump_str( _temp1465); Cyc_Absyndump_dump_char((
int)'{'); for( 0; _temp1463 != 0; _temp1463=(( struct Cyc_List_List*)
_check_null( _temp1463))->tl){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( _temp1463))->hd);} Cyc_Absyndump_dump_char((
int)'}'); goto _LL1379; _LL1401: Cyc_Absyndump_dump(({ unsigned char* _temp1543=(
unsigned char*)"using"; struct _tagged_string _temp1544; _temp1544.curr=
_temp1543; _temp1544.base= _temp1543; _temp1544.last_plus_one= _temp1543 + 6;
_temp1544;})); Cyc_Absyndump_dumpqvar( _temp1469); Cyc_Absyndump_dump_char(( int)'{');
for( 0; _temp1467 != 0; _temp1467=(( struct Cyc_List_List*) _check_null(
_temp1467))->tl){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( _temp1467))->hd);} Cyc_Absyndump_dump_char(( int)'}'); goto _LL1379;
_LL1403: Cyc_Absyndump_dump(({ unsigned char* _temp1545=( unsigned char*)"extern \"C\" {";
struct _tagged_string _temp1546; _temp1546.curr= _temp1545; _temp1546.base=
_temp1545; _temp1546.last_plus_one= _temp1545 + 13; _temp1546;})); for( 0;
_temp1471 != 0; _temp1471=(( struct Cyc_List_List*) _check_null( _temp1471))->tl){
Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( _temp1471))->hd);} Cyc_Absyndump_dump_char(( int)'}'); goto _LL1379;
_LL1379:;} static void Cyc_Absyndump_dump_upperbound( struct Cyc_Absyn_Exp* e){
unsigned int i= Cyc_Evexp_eval_const_uint_exp( e); if( i != 1){ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpexp( e); Cyc_Absyndump_dump_char(( int)'}');}} void
Cyc_Absyndump_dumptms( struct Cyc_List_List* tms, void(* f)( void*), void* a){
if( tms == 0){ f( a); return;}{ void* _temp1547=( void*)(( struct Cyc_List_List*)
_check_null( tms))->hd; struct Cyc_Absyn_Tqual _temp1565; void* _temp1567; void*
_temp1569; struct Cyc_Absyn_Exp* _temp1571; struct Cyc_Absyn_Tqual _temp1573;
void* _temp1575; void* _temp1577; struct Cyc_Absyn_Exp* _temp1579; struct Cyc_Absyn_Tqual
_temp1581; void* _temp1583; void* _temp1585; struct Cyc_Absyn_Tqual _temp1587;
void* _temp1589; struct Cyc_Absyn_Tvar* _temp1591; void* _temp1593; struct Cyc_Absyn_Exp*
_temp1595; struct Cyc_Absyn_Tqual _temp1597; void* _temp1599; struct Cyc_Absyn_Tvar*
_temp1601; void* _temp1603; struct Cyc_Absyn_Exp* _temp1605; struct Cyc_Absyn_Tqual
_temp1607; void* _temp1609; struct Cyc_Absyn_Tvar* _temp1611; void* _temp1613;
struct Cyc_Absyn_Tqual _temp1615; void* _temp1617; void* _temp1619; _LL1549: if((
unsigned int) _temp1547 > 1u?*(( int*) _temp1547) == Cyc_Absyn_Pointer_mod: 0){
_LL1570: _temp1569=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1547)->f1;
if(( unsigned int) _temp1569 > 1u?*(( int*) _temp1569) == Cyc_Absyn_Nullable_ps:
0){ _LL1572: _temp1571=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp1569)->f1;
goto _LL1568;} else{ goto _LL1551;} _LL1568: _temp1567=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1547)->f2; if( _temp1567 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1566;}
else{ goto _LL1551;} _LL1566: _temp1565=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1547)->f3; goto _LL1550;} else{ goto _LL1551;} _LL1551: if(( unsigned int)
_temp1547 > 1u?*(( int*) _temp1547) == Cyc_Absyn_Pointer_mod: 0){ _LL1578:
_temp1577=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1547)->f1; if((
unsigned int) _temp1577 > 1u?*(( int*) _temp1577) == Cyc_Absyn_NonNullable_ps: 0){
_LL1580: _temp1579=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1577)->f1;
goto _LL1576;} else{ goto _LL1553;} _LL1576: _temp1575=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1547)->f2; if( _temp1575 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1574;}
else{ goto _LL1553;} _LL1574: _temp1573=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1547)->f3; goto _LL1552;} else{ goto _LL1553;} _LL1553: if(( unsigned int)
_temp1547 > 1u?*(( int*) _temp1547) == Cyc_Absyn_Pointer_mod: 0){ _LL1586:
_temp1585=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1547)->f1; if(
_temp1585 ==( void*) Cyc_Absyn_TaggedArray_ps){ goto _LL1584;} else{ goto
_LL1555;} _LL1584: _temp1583=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1547)->f2; if( _temp1583 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1582;}
else{ goto _LL1555;} _LL1582: _temp1581=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1547)->f3; goto _LL1554;} else{ goto _LL1555;} _LL1555: if(( unsigned int)
_temp1547 > 1u?*(( int*) _temp1547) == Cyc_Absyn_Pointer_mod: 0){ _LL1594:
_temp1593=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1547)->f1; if((
unsigned int) _temp1593 > 1u?*(( int*) _temp1593) == Cyc_Absyn_Nullable_ps: 0){
_LL1596: _temp1595=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp1593)->f1; goto
_LL1590;} else{ goto _LL1557;} _LL1590: _temp1589=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1547)->f2; if(( unsigned int) _temp1589 > 4u?*(( int*) _temp1589) == Cyc_Absyn_VarType:
0){ _LL1592: _temp1591=(( struct Cyc_Absyn_VarType_struct*) _temp1589)->f1; goto
_LL1588;} else{ goto _LL1557;} _LL1588: _temp1587=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1547)->f3; goto _LL1556;} else{ goto _LL1557;} _LL1557: if(( unsigned int)
_temp1547 > 1u?*(( int*) _temp1547) == Cyc_Absyn_Pointer_mod: 0){ _LL1604:
_temp1603=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1547)->f1; if((
unsigned int) _temp1603 > 1u?*(( int*) _temp1603) == Cyc_Absyn_NonNullable_ps: 0){
_LL1606: _temp1605=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1603)->f1;
goto _LL1600;} else{ goto _LL1559;} _LL1600: _temp1599=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1547)->f2; if(( unsigned int) _temp1599 > 4u?*(( int*) _temp1599) == Cyc_Absyn_VarType:
0){ _LL1602: _temp1601=(( struct Cyc_Absyn_VarType_struct*) _temp1599)->f1; goto
_LL1598;} else{ goto _LL1559;} _LL1598: _temp1597=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1547)->f3; goto _LL1558;} else{ goto _LL1559;} _LL1559: if(( unsigned int)
_temp1547 > 1u?*(( int*) _temp1547) == Cyc_Absyn_Pointer_mod: 0){ _LL1614:
_temp1613=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1547)->f1; if(
_temp1613 ==( void*) Cyc_Absyn_TaggedArray_ps){ goto _LL1610;} else{ goto
_LL1561;} _LL1610: _temp1609=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1547)->f2; if(( unsigned int) _temp1609 > 4u?*(( int*) _temp1609) == Cyc_Absyn_VarType:
0){ _LL1612: _temp1611=(( struct Cyc_Absyn_VarType_struct*) _temp1609)->f1; goto
_LL1608;} else{ goto _LL1561;} _LL1608: _temp1607=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1547)->f3; goto _LL1560;} else{ goto _LL1561;} _LL1561: if(( unsigned int)
_temp1547 > 1u?*(( int*) _temp1547) == Cyc_Absyn_Pointer_mod: 0){ _LL1620:
_temp1619=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1547)->f1; goto
_LL1618; _LL1618: _temp1617=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1547)->f2; goto _LL1616; _LL1616: _temp1615=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1547)->f3; goto _LL1562;} else{ goto _LL1563;} _LL1563: goto _LL1564;
_LL1550: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dump_upperbound(
_temp1571);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f, a);
return; _LL1552: Cyc_Absyndump_dump_char(( int)'@'); Cyc_Absyndump_dump_upperbound(
_temp1579);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f, a);
return; _LL1554: Cyc_Absyndump_dump_char(( int)'?');(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*)
_check_null( tms))->tl, f, a); return; _LL1556: Cyc_Absyndump_dump_char(( int)'*');
Cyc_Absyndump_dump_upperbound( _temp1595); Cyc_Absyndump_dump_str( _temp1591->name);((
void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)(((
struct Cyc_List_List*) _check_null( tms))->tl, f, a); return; _LL1558: Cyc_Absyndump_dump_char((
int)'@'); Cyc_Absyndump_dump_upperbound( _temp1605); Cyc_Absyndump_dump_str(
_temp1601->name);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void*
a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f,
a); return; _LL1560: Cyc_Absyndump_dump_char(( int)'?'); Cyc_Absyndump_dump_str(
_temp1611->name);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void*
a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f,
a); return; _LL1562:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1621=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1621[ 0]=({ struct Cyc_Core_Impossible_struct _temp1622; _temp1622.tag= Cyc_Core_Impossible;
_temp1622.f1=({ unsigned char* _temp1623=( unsigned char*)"dumptms: bad Pointer_mod";
struct _tagged_string _temp1624; _temp1624.curr= _temp1623; _temp1624.base=
_temp1623; _temp1624.last_plus_one= _temp1623 + 25; _temp1624;}); _temp1622;});
_temp1621;})); _LL1564: { int next_is_pointer= 0; if((( struct Cyc_List_List*)
_check_null( tms))->tl != 0){ void* _temp1625=( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->hd; struct Cyc_Absyn_Tqual
_temp1631; void* _temp1633; void* _temp1635; _LL1627: if(( unsigned int)
_temp1625 > 1u?*(( int*) _temp1625) == Cyc_Absyn_Pointer_mod: 0){ _LL1636:
_temp1635=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1625)->f1; goto
_LL1634; _LL1634: _temp1633=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1625)->f2; goto _LL1632; _LL1632: _temp1631=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1625)->f3; goto _LL1628;} else{ goto _LL1629;} _LL1629: goto _LL1630;
_LL1628: next_is_pointer= 1; goto _LL1626; _LL1630: goto _LL1626; _LL1626:;} if(
next_is_pointer){ Cyc_Absyndump_dump_char(( int)'(');}(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*)
_check_null( tms))->tl, f, a); if( next_is_pointer){ Cyc_Absyndump_dump_char((
int)')');}{ void* _temp1637=( void*)(( struct Cyc_List_List*) _check_null( tms))->hd;
struct Cyc_Absyn_Exp* _temp1653; void* _temp1655; struct Cyc_List_List*
_temp1657; struct Cyc_Core_Opt* _temp1659; struct Cyc_Absyn_VarargInfo*
_temp1661; int _temp1663; struct Cyc_List_List* _temp1665; void* _temp1667;
struct Cyc_Position_Segment* _temp1669; struct Cyc_List_List* _temp1671; int
_temp1673; struct Cyc_Position_Segment* _temp1675; struct Cyc_List_List*
_temp1677; struct Cyc_List_List* _temp1679; struct Cyc_Position_Segment*
_temp1681; struct Cyc_Absyn_Tqual _temp1683; void* _temp1685; void* _temp1687;
_LL1639: if( _temp1637 ==( void*) Cyc_Absyn_Carray_mod){ goto _LL1640;} else{
goto _LL1641;} _LL1641: if(( unsigned int) _temp1637 > 1u?*(( int*) _temp1637)
== Cyc_Absyn_ConstArray_mod: 0){ _LL1654: _temp1653=(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp1637)->f1; goto _LL1642;} else{ goto _LL1643;} _LL1643: if(( unsigned int)
_temp1637 > 1u?*(( int*) _temp1637) == Cyc_Absyn_Function_mod: 0){ _LL1656:
_temp1655=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp1637)->f1; if(*((
int*) _temp1655) == Cyc_Absyn_WithTypes){ _LL1666: _temp1665=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1655)->f1; goto _LL1664; _LL1664: _temp1663=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1655)->f2; goto _LL1662; _LL1662: _temp1661=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1655)->f3; goto _LL1660; _LL1660: _temp1659=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1655)->f4; goto _LL1658; _LL1658: _temp1657=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1655)->f5; goto _LL1644;} else{ goto _LL1645;}} else{ goto _LL1645;}
_LL1645: if(( unsigned int) _temp1637 > 1u?*(( int*) _temp1637) == Cyc_Absyn_Function_mod:
0){ _LL1668: _temp1667=( void*)(( struct Cyc_Absyn_Function_mod_struct*)
_temp1637)->f1; if(*(( int*) _temp1667) == Cyc_Absyn_NoTypes){ _LL1672:
_temp1671=(( struct Cyc_Absyn_NoTypes_struct*) _temp1667)->f1; goto _LL1670;
_LL1670: _temp1669=(( struct Cyc_Absyn_NoTypes_struct*) _temp1667)->f2; goto
_LL1646;} else{ goto _LL1647;}} else{ goto _LL1647;} _LL1647: if(( unsigned int)
_temp1637 > 1u?*(( int*) _temp1637) == Cyc_Absyn_TypeParams_mod: 0){ _LL1678:
_temp1677=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp1637)->f1; goto
_LL1676; _LL1676: _temp1675=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1637)->f2; goto _LL1674; _LL1674: _temp1673=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1637)->f3; goto _LL1648;} else{ goto _LL1649;} _LL1649: if(( unsigned int)
_temp1637 > 1u?*(( int*) _temp1637) == Cyc_Absyn_Attributes_mod: 0){ _LL1682:
_temp1681=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp1637)->f1; goto
_LL1680; _LL1680: _temp1679=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp1637)->f2; goto _LL1650;} else{ goto _LL1651;} _LL1651: if(( unsigned int)
_temp1637 > 1u?*(( int*) _temp1637) == Cyc_Absyn_Pointer_mod: 0){ _LL1688:
_temp1687=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1637)->f1; goto
_LL1686; _LL1686: _temp1685=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1637)->f2; goto _LL1684; _LL1684: _temp1683=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1637)->f3; goto _LL1652;} else{ goto _LL1638;} _LL1640: Cyc_Absyndump_dump(({
unsigned char* _temp1689=( unsigned char*)"[]"; struct _tagged_string _temp1690;
_temp1690.curr= _temp1689; _temp1690.base= _temp1689; _temp1690.last_plus_one=
_temp1689 + 3; _temp1690;})); goto _LL1638; _LL1642: Cyc_Absyndump_dump_char((
int)'['); Cyc_Absyndump_dumpexp( _temp1653); Cyc_Absyndump_dump_char(( int)']');
goto _LL1638; _LL1644: Cyc_Absyndump_dumpfunargs( _temp1665, _temp1663,
_temp1661, _temp1659, _temp1657); goto _LL1638; _LL1646:(( void(*)( void(* f)(
struct _tagged_string*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dump_str,
_temp1671,({ unsigned char* _temp1691=( unsigned char*)"("; struct
_tagged_string _temp1692; _temp1692.curr= _temp1691; _temp1692.base= _temp1691;
_temp1692.last_plus_one= _temp1691 + 2; _temp1692;}),({ unsigned char* _temp1693=(
unsigned char*)")"; struct _tagged_string _temp1694; _temp1694.curr= _temp1693;
_temp1694.base= _temp1693; _temp1694.last_plus_one= _temp1693 + 2; _temp1694;}),({
unsigned char* _temp1695=( unsigned char*)","; struct _tagged_string _temp1696;
_temp1696.curr= _temp1695; _temp1696.base= _temp1695; _temp1696.last_plus_one=
_temp1695 + 2; _temp1696;})); goto _LL1638; _LL1648: if( _temp1673){ Cyc_Absyndump_dumpkindedtvars(
_temp1677);} else{ Cyc_Absyndump_dumptvars( _temp1677);} goto _LL1638; _LL1650:
Cyc_Absyndump_dumpatts( _temp1679); goto _LL1638; _LL1652:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp1697=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1697[ 0]=({ struct
Cyc_Core_Impossible_struct _temp1698; _temp1698.tag= Cyc_Core_Impossible;
_temp1698.f1=({ unsigned char* _temp1699=( unsigned char*)"dumptms"; struct
_tagged_string _temp1700; _temp1700.curr= _temp1699; _temp1700.base= _temp1699;
_temp1700.last_plus_one= _temp1699 + 8; _temp1700;}); _temp1698;}); _temp1697;}));
_LL1638:;} return;} _LL1548:;}} void Cyc_Absyndump_dumptqtd( struct Cyc_Absyn_Tqual
tq, void* t, void(* f)( void*), void* a){ struct Cyc_List_List* _temp1703; void*
_temp1705; struct Cyc_Absyn_Tqual _temp1707; struct _tuple4 _temp1701= Cyc_Absynpp_to_tms(
tq, t); _LL1708: _temp1707= _temp1701.f1; goto _LL1706; _LL1706: _temp1705=
_temp1701.f2; goto _LL1704; _LL1704: _temp1703= _temp1701.f3; goto _LL1702;
_LL1702: Cyc_Absyndump_dumptq( _temp1707); Cyc_Absyndump_dumpntyp( _temp1705);((
void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1703),
f, a);} void Cyc_Absyndump_dumpdecllist2file( struct Cyc_List_List* tdl, struct
Cyc_Stdio___sFILE* f){ Cyc_Absyndump_pos= 0;* Cyc_Absyndump_dump_file= f; for( 0;
tdl != 0; tdl=(( struct Cyc_List_List*) _check_null( tdl))->tl){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( tdl))->hd);}}