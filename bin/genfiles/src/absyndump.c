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
unsigned char* _temp24=( unsigned char*)"::"; struct _tagged_string _temp25;
_temp25.curr= _temp24; _temp25.base= _temp24; _temp25.last_plus_one= _temp24 + 3;
_temp25;}));} Cyc_Absyndump_dump_nospace(*(( struct _tagged_string*)(( struct
Cyc_List_List*) _check_null( _temp2))->hd));} if( Cyc_Absynpp_qvar_to_Cids){ Cyc_Absyndump_dump_nospace(({
unsigned char* _temp26=( unsigned char*)"_"; struct _tagged_string _temp27;
_temp27.curr= _temp26; _temp27.base= _temp26; _temp27.last_plus_one= _temp26 + 2;
_temp27;}));} else{ Cyc_Absyndump_dump_nospace(({ unsigned char* _temp28=(
unsigned char*)"::"; struct _tagged_string _temp29; _temp29.curr= _temp28;
_temp29.base= _temp28; _temp29.last_plus_one= _temp28 + 3; _temp29;}));} Cyc_Absyndump_dump_nospace(*(*
v).f2);} void Cyc_Absyndump_dumptq( struct Cyc_Absyn_Tqual tq){ if( tq.q_restrict){
Cyc_Absyndump_dump(({ unsigned char* _temp30=( unsigned char*)"restrict"; struct
_tagged_string _temp31; _temp31.curr= _temp30; _temp31.base= _temp30; _temp31.last_plus_one=
_temp30 + 9; _temp31;}));} if( tq.q_volatile){ Cyc_Absyndump_dump(({
unsigned char* _temp32=( unsigned char*)"volatile"; struct _tagged_string
_temp33; _temp33.curr= _temp32; _temp33.base= _temp32; _temp33.last_plus_one=
_temp32 + 9; _temp33;}));} if( tq.q_const){ Cyc_Absyndump_dump(({ unsigned char*
_temp34=( unsigned char*)"const"; struct _tagged_string _temp35; _temp35.curr=
_temp34; _temp35.base= _temp34; _temp35.last_plus_one= _temp34 + 6; _temp35;}));}}
void Cyc_Absyndump_dumpscope( void* sc){ void* _temp36= sc; _LL38: if( _temp36
==( void*) Cyc_Absyn_Static){ goto _LL39;} else{ goto _LL40;} _LL40: if( _temp36
==( void*) Cyc_Absyn_Public){ goto _LL41;} else{ goto _LL42;} _LL42: if( _temp36
==( void*) Cyc_Absyn_Extern){ goto _LL43;} else{ goto _LL44;} _LL44: if( _temp36
==( void*) Cyc_Absyn_ExternC){ goto _LL45;} else{ goto _LL46;} _LL46: if(
_temp36 ==( void*) Cyc_Absyn_Abstract){ goto _LL47;} else{ goto _LL37;} _LL39:
Cyc_Absyndump_dump(({ unsigned char* _temp48=( unsigned char*)"static"; struct
_tagged_string _temp49; _temp49.curr= _temp48; _temp49.base= _temp48; _temp49.last_plus_one=
_temp48 + 7; _temp49;})); return; _LL41: return; _LL43: Cyc_Absyndump_dump(({
unsigned char* _temp50=( unsigned char*)"extern"; struct _tagged_string _temp51;
_temp51.curr= _temp50; _temp51.base= _temp50; _temp51.last_plus_one= _temp50 + 7;
_temp51;})); return; _LL45: Cyc_Absyndump_dump(({ unsigned char* _temp52=(
unsigned char*)"extern \"C\""; struct _tagged_string _temp53; _temp53.curr=
_temp52; _temp53.base= _temp52; _temp53.last_plus_one= _temp52 + 11; _temp53;}));
return; _LL47: Cyc_Absyndump_dump(({ unsigned char* _temp54=( unsigned char*)"abstract";
struct _tagged_string _temp55; _temp55.curr= _temp54; _temp55.base= _temp54;
_temp55.last_plus_one= _temp54 + 9; _temp55;})); return; _LL37:;} void Cyc_Absyndump_dumpkind(
void* k){ void* _temp56= k; _LL58: if( _temp56 ==( void*) Cyc_Absyn_AnyKind){
goto _LL59;} else{ goto _LL60;} _LL60: if( _temp56 ==( void*) Cyc_Absyn_MemKind){
goto _LL61;} else{ goto _LL62;} _LL62: if( _temp56 ==( void*) Cyc_Absyn_BoxKind){
goto _LL63;} else{ goto _LL64;} _LL64: if( _temp56 ==( void*) Cyc_Absyn_RgnKind){
goto _LL65;} else{ goto _LL66;} _LL66: if( _temp56 ==( void*) Cyc_Absyn_EffKind){
goto _LL67;} else{ goto _LL57;} _LL59: Cyc_Absyndump_dump(({ unsigned char*
_temp68=( unsigned char*)"A"; struct _tagged_string _temp69; _temp69.curr=
_temp68; _temp69.base= _temp68; _temp69.last_plus_one= _temp68 + 2; _temp69;}));
return; _LL61: Cyc_Absyndump_dump(({ unsigned char* _temp70=( unsigned char*)"M";
struct _tagged_string _temp71; _temp71.curr= _temp70; _temp71.base= _temp70;
_temp71.last_plus_one= _temp70 + 2; _temp71;})); return; _LL63: Cyc_Absyndump_dump(({
unsigned char* _temp72=( unsigned char*)"B"; struct _tagged_string _temp73;
_temp73.curr= _temp72; _temp73.base= _temp72; _temp73.last_plus_one= _temp72 + 2;
_temp73;})); return; _LL65: Cyc_Absyndump_dump(({ unsigned char* _temp74=(
unsigned char*)"R"; struct _tagged_string _temp75; _temp75.curr= _temp74;
_temp75.base= _temp74; _temp75.last_plus_one= _temp74 + 2; _temp75;})); return;
_LL67: Cyc_Absyndump_dump(({ unsigned char* _temp76=( unsigned char*)"E"; struct
_tagged_string _temp77; _temp77.curr= _temp76; _temp77.base= _temp76; _temp77.last_plus_one=
_temp76 + 2; _temp77;})); return; _LL57:;} void Cyc_Absyndump_dumptps( struct
Cyc_List_List* ts){(( void(*)( void(* f)( void*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumptyp, ts,({ unsigned char* _temp78=(
unsigned char*)"<"; struct _tagged_string _temp79; _temp79.curr= _temp78;
_temp79.base= _temp78; _temp79.last_plus_one= _temp78 + 2; _temp79;}),({
unsigned char* _temp80=( unsigned char*)">"; struct _tagged_string _temp81;
_temp81.curr= _temp80; _temp81.base= _temp80; _temp81.last_plus_one= _temp80 + 2;
_temp81;}),({ unsigned char* _temp82=( unsigned char*)","; struct _tagged_string
_temp83; _temp83.curr= _temp82; _temp83.base= _temp82; _temp83.last_plus_one=
_temp82 + 2; _temp83;}));} void Cyc_Absyndump_dumptvar( struct Cyc_Absyn_Tvar*
tv){ Cyc_Absyndump_dump_str( tv->name);} void Cyc_Absyndump_dumpkindedtvar(
struct Cyc_Absyn_Tvar* tv){ Cyc_Absyndump_dump_str( tv->name);{ void* _temp84=(
void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
tv->kind))->v; void* _temp92; void* _temp94; _LL86: if(( unsigned int) _temp84 >
1u?*(( int*) _temp84) == Cyc_Absyn_Eq_constr: 0){ _LL93: _temp92=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp84)->f1; if( _temp92 ==( void*) Cyc_Absyn_BoxKind){
goto _LL87;} else{ goto _LL88;}} else{ goto _LL88;} _LL88: if(( unsigned int)
_temp84 > 1u?*(( int*) _temp84) == Cyc_Absyn_Eq_constr: 0){ _LL95: _temp94=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp84)->f1; goto _LL89;} else{
goto _LL90;} _LL90: goto _LL91; _LL87: goto _LL85; _LL89: Cyc_Absyndump_dump(({
unsigned char* _temp96=( unsigned char*)"::"; struct _tagged_string _temp97;
_temp97.curr= _temp96; _temp97.base= _temp96; _temp97.last_plus_one= _temp96 + 3;
_temp97;})); Cyc_Absyndump_dumpkind( _temp94); goto _LL85; _LL91: Cyc_Absyndump_dump(({
unsigned char* _temp98=( unsigned char*)"::?"; struct _tagged_string _temp99;
_temp99.curr= _temp98; _temp99.base= _temp98; _temp99.last_plus_one= _temp98 + 4;
_temp99;})); goto _LL85; _LL85:;}} void Cyc_Absyndump_dumptvars( struct Cyc_List_List*
tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumptvar, tvs,({ unsigned char*
_temp100=( unsigned char*)"<"; struct _tagged_string _temp101; _temp101.curr=
_temp100; _temp101.base= _temp100; _temp101.last_plus_one= _temp100 + 2;
_temp101;}),({ unsigned char* _temp102=( unsigned char*)">"; struct
_tagged_string _temp103; _temp103.curr= _temp102; _temp103.base= _temp102;
_temp103.last_plus_one= _temp102 + 2; _temp103;}),({ unsigned char* _temp104=(
unsigned char*)","; struct _tagged_string _temp105; _temp105.curr= _temp104;
_temp105.base= _temp104; _temp105.last_plus_one= _temp104 + 2; _temp105;}));}
void Cyc_Absyndump_dumpkindedtvars( struct Cyc_List_List* tvs){(( void(*)( void(*
f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct _tagged_string
start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpkindedtvar, tvs,({ unsigned char* _temp106=( unsigned char*)"<";
struct _tagged_string _temp107; _temp107.curr= _temp106; _temp107.base= _temp106;
_temp107.last_plus_one= _temp106 + 2; _temp107;}),({ unsigned char* _temp108=(
unsigned char*)">"; struct _tagged_string _temp109; _temp109.curr= _temp108;
_temp109.base= _temp108; _temp109.last_plus_one= _temp108 + 2; _temp109;}),({
unsigned char* _temp110=( unsigned char*)","; struct _tagged_string _temp111;
_temp111.curr= _temp110; _temp111.base= _temp110; _temp111.last_plus_one=
_temp110 + 2; _temp111;}));} struct _tuple5{ struct Cyc_Absyn_Tqual f1; void* f2;
} ; void Cyc_Absyndump_dumparg( struct _tuple5* pr){(( void(*)( struct Cyc_Absyn_Tqual,
void*, void(* f)( int), int)) Cyc_Absyndump_dumptqtd)((* pr).f1,(* pr).f2,( void(*)(
int x)) Cyc_Absyndump_ignore, 0);} void Cyc_Absyndump_dumpargs( struct Cyc_List_List*
ts){(( void(*)( void(* f)( struct _tuple5*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumparg, ts,({ unsigned char* _temp112=( unsigned char*)"(";
struct _tagged_string _temp113; _temp113.curr= _temp112; _temp113.base= _temp112;
_temp113.last_plus_one= _temp112 + 2; _temp113;}),({ unsigned char* _temp114=(
unsigned char*)")"; struct _tagged_string _temp115; _temp115.curr= _temp114;
_temp115.base= _temp114; _temp115.last_plus_one= _temp114 + 2; _temp115;}),({
unsigned char* _temp116=( unsigned char*)","; struct _tagged_string _temp117;
_temp117.curr= _temp116; _temp117.base= _temp116; _temp117.last_plus_one=
_temp116 + 2; _temp117;}));} void Cyc_Absyndump_dump_callconv( struct Cyc_List_List*
atts){ for( 0; atts != 0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){
void* _temp118=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd; _LL120:
if( _temp118 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL121;} else{ goto _LL122;}
_LL122: if( _temp118 ==( void*) Cyc_Absyn_Cdecl_att){ goto _LL123;} else{ goto
_LL124;} _LL124: if( _temp118 ==( void*) Cyc_Absyn_Fastcall_att){ goto _LL125;}
else{ goto _LL126;} _LL126: goto _LL127; _LL121: Cyc_Absyndump_dump(({
unsigned char* _temp128=( unsigned char*)"_stdcall"; struct _tagged_string
_temp129; _temp129.curr= _temp128; _temp129.base= _temp128; _temp129.last_plus_one=
_temp128 + 9; _temp129;})); return; _LL123: Cyc_Absyndump_dump(({ unsigned char*
_temp130=( unsigned char*)"_cdecl"; struct _tagged_string _temp131; _temp131.curr=
_temp130; _temp131.base= _temp130; _temp131.last_plus_one= _temp130 + 7;
_temp131;})); return; _LL125: Cyc_Absyndump_dump(({ unsigned char* _temp132=(
unsigned char*)"_fastcall"; struct _tagged_string _temp133; _temp133.curr=
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
hasatt){ return;} Cyc_Absyndump_dump(({ unsigned char* _temp144=( unsigned char*)"__declspec(";
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
return;} Cyc_Absyndump_dump(({ unsigned char* _temp156=( unsigned char*)" __attribute__((";
struct _tagged_string _temp157; _temp157.curr= _temp156; _temp157.base= _temp156;
_temp157.last_plus_one= _temp156 + 17; _temp157;})); for( 0; atts != 0; atts=((
struct Cyc_List_List*) _check_null( atts))->tl){ Cyc_Absyndump_dump( Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd)); if((( struct Cyc_List_List*)
_check_null( atts))->tl != 0){ Cyc_Absyndump_dump(({ unsigned char* _temp158=(
unsigned char*)","; struct _tagged_string _temp159; _temp159.curr= _temp158;
_temp159.base= _temp158; _temp159.last_plus_one= _temp158 + 2; _temp159;}));}}
Cyc_Absyndump_dump(({ unsigned char* _temp160=( unsigned char*)")) "; struct
_tagged_string _temp161; _temp161.curr= _temp160; _temp161.base= _temp160;
_temp161.last_plus_one= _temp160 + 4; _temp161;}));} int Cyc_Absyndump_next_is_pointer(
struct Cyc_List_List* tms){ if( tms == 0){ return 0;}{ void* _temp162=( void*)((
struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Tqual _temp168;
void* _temp170; void* _temp172; _LL164: if(( unsigned int) _temp162 > 1u?*(( int*)
_temp162) == Cyc_Absyn_Pointer_mod: 0){ _LL173: _temp172=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp162)->f1; goto _LL171; _LL171: _temp170=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp162)->f2; goto _LL169; _LL169: _temp168=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp162)->f3; goto _LL165;} else{ goto _LL166;} _LL166: goto _LL167; _LL165:
return 1; _LL167: return 0; _LL163:;}} static void Cyc_Absyndump_dumprgn( void*
t){ void* _temp174= Cyc_Tcutil_compress( t); _LL176: if( _temp174 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL177;} else{ goto _LL178;} _LL178: goto _LL179; _LL177: Cyc_Absyndump_dump(({
unsigned char* _temp180=( unsigned char*)"`H"; struct _tagged_string _temp181;
_temp181.curr= _temp180; _temp181.base= _temp180; _temp181.last_plus_one=
_temp180 + 3; _temp181;})); goto _LL175; _LL179: Cyc_Absyndump_dumpntyp( t);
goto _LL175; _LL175:;} struct _tuple6{ struct Cyc_List_List* f1; struct Cyc_List_List*
f2; } ; static struct _tuple6 Cyc_Absyndump_effects_split( void* t){ struct Cyc_List_List*
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
_LL216: return; _LL218: Cyc_Absyndump_dump(({ unsigned char* _temp412=(
unsigned char*)"void"; struct _tagged_string _temp413; _temp413.curr= _temp412;
_temp413.base= _temp412; _temp413.last_plus_one= _temp412 + 5; _temp413;}));
return; _LL220: Cyc_Absyndump_dump_str( _temp289->name); return; _LL222: Cyc_Absyndump_dump(({
unsigned char* _temp414=( unsigned char*)"%"; struct _tagged_string _temp415;
_temp415.curr= _temp414; _temp415.base= _temp414; _temp415.last_plus_one=
_temp414 + 2; _temp415;})); if( _temp297 == 0){ Cyc_Absyndump_dump(({
unsigned char* _temp416=( unsigned char*)"?"; struct _tagged_string _temp417;
_temp417.curr= _temp416; _temp417.base= _temp416; _temp417.last_plus_one=
_temp416 + 2; _temp417;}));} else{ Cyc_Absyndump_dumpkind(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp297))->v);} Cyc_Absyndump_dump(( struct _tagged_string)
xprintf("(%d)", _temp293)); return; _LL224: Cyc_Absyndump_dumpntyp( _temp306);
return; _LL226:{ void* _temp418= _temp316; struct Cyc_Absyn_UnknownTunionInfo
_temp424; int _temp426; struct _tuple0* _temp428; struct Cyc_Absyn_Tuniondecl*
_temp430; struct Cyc_Absyn_Tuniondecl _temp432; int _temp433; struct Cyc_Core_Opt*
_temp435; struct Cyc_List_List* _temp437; struct _tuple0* _temp439; void*
_temp441; _LL420: if(*(( int*) _temp418) == Cyc_Absyn_UnknownTunion){ _LL425:
_temp424=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp418)->f1; _LL429:
_temp428= _temp424.name; goto _LL427; _LL427: _temp426= _temp424.is_xtunion;
goto _LL421;} else{ goto _LL422;} _LL422: if(*(( int*) _temp418) == Cyc_Absyn_KnownTunion){
_LL431: _temp430=(( struct Cyc_Absyn_KnownTunion_struct*) _temp418)->f1;
_temp432=* _temp430; _LL442: _temp441=( void*) _temp432.sc; goto _LL440; _LL440:
_temp439= _temp432.name; goto _LL438; _LL438: _temp437= _temp432.tvs; goto
_LL436; _LL436: _temp435= _temp432.fields; goto _LL434; _LL434: _temp433=
_temp432.is_xtunion; goto _LL423;} else{ goto _LL419;} _LL421: _temp439=
_temp428; _temp433= _temp426; goto _LL423; _LL423: if( _temp433){ Cyc_Absyndump_dump(({
unsigned char* _temp443=( unsigned char*)"xtunion "; struct _tagged_string
_temp444; _temp444.curr= _temp443; _temp444.base= _temp443; _temp444.last_plus_one=
_temp443 + 9; _temp444;}));} else{ Cyc_Absyndump_dump(({ unsigned char* _temp445=(
unsigned char*)"tunion "; struct _tagged_string _temp446; _temp446.curr=
_temp445; _temp446.base= _temp445; _temp446.last_plus_one= _temp445 + 8;
_temp446;}));}{ void* _temp447= Cyc_Tcutil_compress( _temp312); _LL449: if(
_temp447 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL450;} else{ goto _LL451;} _LL451:
goto _LL452; _LL450: goto _LL448; _LL452: Cyc_Absyndump_dumptyp( _temp312); Cyc_Absyndump_dump(({
unsigned char* _temp453=( unsigned char*)" "; struct _tagged_string _temp454;
_temp454.curr= _temp453; _temp454.base= _temp453; _temp454.last_plus_one=
_temp453 + 2; _temp454;})); goto _LL448; _LL448:;} Cyc_Absyndump_dumpqvar(
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
unsigned char* _temp495=( unsigned char*)"xtunion "; struct _tagged_string
_temp496; _temp496.curr= _temp495; _temp496.base= _temp495; _temp496.last_plus_one=
_temp495 + 9; _temp496;}));} else{ Cyc_Absyndump_dump(({ unsigned char* _temp497=(
unsigned char*)"tunion "; struct _tagged_string _temp498; _temp498.curr=
_temp497; _temp498.base= _temp497; _temp498.last_plus_one= _temp497 + 8;
_temp498;}));} Cyc_Absyndump_dumpqvar( _temp491); Cyc_Absyndump_dump(({
unsigned char* _temp499=( unsigned char*)"."; struct _tagged_string _temp500;
_temp500.curr= _temp499; _temp500.base= _temp499; _temp500.last_plus_one=
_temp499 + 2; _temp500;})); Cyc_Absyndump_dumpqvar( _temp480); Cyc_Absyndump_dumptps(
_temp320); goto _LL456; _LL456:;} goto _LL210; _LL230: Cyc_Absyndump_dump(({
unsigned char* _temp501=( unsigned char*)"enum "; struct _tagged_string _temp502;
_temp502.curr= _temp501; _temp502.base= _temp501; _temp502.last_plus_one=
_temp501 + 6; _temp502;})); Cyc_Absyndump_dumpqvar( _temp326); return; _LL232:
Cyc_Absyndump_dump(({ unsigned char* _temp503=( unsigned char*)"int"; struct
_tagged_string _temp504; _temp504.curr= _temp503; _temp504.base= _temp503;
_temp504.last_plus_one= _temp503 + 4; _temp504;})); return; _LL234: Cyc_Absyndump_dump(({
unsigned char* _temp505=( unsigned char*)"signed char"; struct _tagged_string
_temp506; _temp506.curr= _temp505; _temp506.base= _temp505; _temp506.last_plus_one=
_temp505 + 12; _temp506;})); return; _LL236: Cyc_Absyndump_dump(({ unsigned char*
_temp507=( unsigned char*)"unsigned char"; struct _tagged_string _temp508;
_temp508.curr= _temp507; _temp508.base= _temp507; _temp508.last_plus_one=
_temp507 + 14; _temp508;})); return; _LL238: Cyc_Absyndump_dump(({ unsigned char*
_temp509=( unsigned char*)"short"; struct _tagged_string _temp510; _temp510.curr=
_temp509; _temp510.base= _temp509; _temp510.last_plus_one= _temp509 + 6;
_temp510;})); return; _LL240: Cyc_Absyndump_dump(({ unsigned char* _temp511=(
unsigned char*)"unsigned short"; struct _tagged_string _temp512; _temp512.curr=
_temp511; _temp512.base= _temp511; _temp512.last_plus_one= _temp511 + 15;
_temp512;})); return; _LL242: Cyc_Absyndump_dump(({ unsigned char* _temp513=(
unsigned char*)"unsigned int"; struct _tagged_string _temp514; _temp514.curr=
_temp513; _temp514.base= _temp513; _temp514.last_plus_one= _temp513 + 13;
_temp514;})); return; _LL244: Cyc_Absyndump_dump(({ unsigned char* _temp515=(
unsigned char*)"__int64"; struct _tagged_string _temp516; _temp516.curr=
_temp515; _temp516.base= _temp515; _temp516.last_plus_one= _temp515 + 8;
_temp516;})); return; _LL246: Cyc_Absyndump_dump(({ unsigned char* _temp517=(
unsigned char*)"long long"; struct _tagged_string _temp518; _temp518.curr=
_temp517; _temp518.base= _temp517; _temp518.last_plus_one= _temp517 + 10;
_temp518;})); return; _LL248: Cyc_Absyndump_dump(({ unsigned char* _temp519=(
unsigned char*)"unsigned __int64"; struct _tagged_string _temp520; _temp520.curr=
_temp519; _temp520.base= _temp519; _temp520.last_plus_one= _temp519 + 17;
_temp520;})); return; _LL250: Cyc_Absyndump_dump(({ unsigned char* _temp521=(
unsigned char*)"unsigned long long"; struct _tagged_string _temp522; _temp522.curr=
_temp521; _temp522.base= _temp521; _temp522.last_plus_one= _temp521 + 19;
_temp522;})); return; _LL252: Cyc_Absyndump_dump(({ unsigned char* _temp523=(
unsigned char*)"float"; struct _tagged_string _temp524; _temp524.curr= _temp523;
_temp524.base= _temp523; _temp524.last_plus_one= _temp523 + 6; _temp524;}));
return; _LL254: Cyc_Absyndump_dump(({ unsigned char* _temp525=( unsigned char*)"double";
struct _tagged_string _temp526; _temp526.curr= _temp525; _temp526.base= _temp525;
_temp526.last_plus_one= _temp525 + 7; _temp526;})); return; _LL256: Cyc_Absyndump_dump_char((
int)'$'); Cyc_Absyndump_dumpargs( _temp370); return; _LL258: Cyc_Absyndump_dump(({
unsigned char* _temp527=( unsigned char*)"struct"; struct _tagged_string
_temp528; _temp528.curr= _temp527; _temp528.base= _temp527; _temp528.last_plus_one=
_temp527 + 7; _temp528;})); Cyc_Absyndump_dumptps( _temp374); return; _LL260:
Cyc_Absyndump_dump(({ unsigned char* _temp529=( unsigned char*)"struct"; struct
_tagged_string _temp530; _temp530.curr= _temp529; _temp530.base= _temp529;
_temp530.last_plus_one= _temp529 + 7; _temp530;})); Cyc_Absyndump_dumpqvar((
struct _tuple0*) _check_null( _temp382)); Cyc_Absyndump_dumptps( _temp380);
return; _LL262: Cyc_Absyndump_dump(({ unsigned char* _temp531=( unsigned char*)"union";
struct _tagged_string _temp532; _temp532.curr= _temp531; _temp532.base= _temp531;
_temp532.last_plus_one= _temp531 + 6; _temp532;})); Cyc_Absyndump_dumptps(
_temp386); return; _LL264: Cyc_Absyndump_dump(({ unsigned char* _temp533=(
unsigned char*)"union"; struct _tagged_string _temp534; _temp534.curr= _temp533;
_temp534.base= _temp533; _temp534.last_plus_one= _temp533 + 6; _temp534;})); Cyc_Absyndump_dumpqvar((
struct _tuple0*) _check_null( _temp394)); Cyc_Absyndump_dumptps( _temp392);
return; _LL266: Cyc_Absyndump_dump(({ unsigned char* _temp535=( unsigned char*)"struct {";
struct _tagged_string _temp536; _temp536.curr= _temp535; _temp536.base= _temp535;
_temp536.last_plus_one= _temp535 + 9; _temp536;})); Cyc_Absyndump_dumpstructfields(
_temp396); Cyc_Absyndump_dump(({ unsigned char* _temp537=( unsigned char*)"}";
struct _tagged_string _temp538; _temp538.curr= _temp537; _temp538.base= _temp537;
_temp538.last_plus_one= _temp537 + 2; _temp538;})); return; _LL268: Cyc_Absyndump_dump(({
unsigned char* _temp539=( unsigned char*)"union {"; struct _tagged_string
_temp540; _temp540.curr= _temp539; _temp540.base= _temp539; _temp540.last_plus_one=
_temp539 + 8; _temp540;})); Cyc_Absyndump_dumpstructfields( _temp398); Cyc_Absyndump_dump(({
unsigned char* _temp541=( unsigned char*)"}"; struct _tagged_string _temp542;
_temp542.curr= _temp541; _temp542.base= _temp541; _temp542.last_plus_one=
_temp541 + 2; _temp542;})); return; _LL270:( Cyc_Absyndump_dumpqvar( _temp404),
Cyc_Absyndump_dumptps( _temp402)); return; _LL272: Cyc_Absyndump_dumprgn(
_temp406); return; _LL274: return; _LL276: return; _LL278: return; _LL210:;}
void Cyc_Absyndump_dumpvaropt( struct Cyc_Core_Opt* vo){ if( vo != 0){ Cyc_Absyndump_dump_str((
struct _tagged_string*)(( struct Cyc_Core_Opt*) _check_null( vo))->v);}} void
Cyc_Absyndump_dumpfunarg( struct _tuple1* t){(( void(*)( struct Cyc_Absyn_Tqual,
void*, void(* f)( struct Cyc_Core_Opt*), struct Cyc_Core_Opt*)) Cyc_Absyndump_dumptqtd)((*
t).f2,(* t).f3, Cyc_Absyndump_dumpvaropt,(* t).f1);} struct _tuple7{ void* f1;
void* f2; } ; void Cyc_Absyndump_dump_rgncmp( struct _tuple7* cmp){ struct
_tuple7 _temp545; void* _temp546; void* _temp548; struct _tuple7* _temp543= cmp;
_temp545=* _temp543; _LL549: _temp548= _temp545.f1; goto _LL547; _LL547:
_temp546= _temp545.f2; goto _LL544; _LL544: Cyc_Absyndump_dumptyp( _temp548);
Cyc_Absyndump_dump_char(( int)'<'); Cyc_Absyndump_dumptyp( _temp546);} void Cyc_Absyndump_dump_rgnpo(
struct Cyc_List_List* rgn_po){(( void(*)( void(* f)( struct _tuple7*), struct
Cyc_List_List* l, struct _tagged_string sep)) Cyc_Absyndump_dump_sep)( Cyc_Absyndump_dump_rgncmp,
rgn_po,({ unsigned char* _temp550=( unsigned char*)","; struct _tagged_string
_temp551; _temp551.curr= _temp550; _temp551.base= _temp550; _temp551.last_plus_one=
_temp550 + 2; _temp551;}));} void Cyc_Absyndump_dumpfunargs( struct Cyc_List_List*
args, int c_varargs, struct Cyc_Absyn_VarargInfo* cyc_varargs, struct Cyc_Core_Opt*
effopt, struct Cyc_List_List* rgn_po){ Cyc_Absyndump_dump_char(( int)'('); for(
0; args != 0; args=(( struct Cyc_List_List*) _check_null( args))->tl){ Cyc_Absyndump_dumpfunarg((
struct _tuple1*)(( struct Cyc_List_List*) _check_null( args))->hd); if((((
struct Cyc_List_List*) _check_null( args))->tl != 0? 1: c_varargs)? 1:
cyc_varargs != 0){ Cyc_Absyndump_dump_char(( int)',');}} if( c_varargs){ Cyc_Absyndump_dump(({
unsigned char* _temp552=( unsigned char*)"..."; struct _tagged_string _temp553;
_temp553.curr= _temp552; _temp553.base= _temp552; _temp553.last_plus_one=
_temp552 + 4; _temp553;}));} else{ if( cyc_varargs != 0){ struct _tuple1*
_temp555=({ struct _tuple1* _temp554=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp554->f1=(( struct Cyc_Absyn_VarargInfo*) _check_null(
cyc_varargs))->name; _temp554->f2=(( struct Cyc_Absyn_VarargInfo*) _check_null(
cyc_varargs))->tq; _temp554->f3=( void*)(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->type; _temp554;}); goto _LL556; _LL556: Cyc_Absyndump_dump(({
unsigned char* _temp557=( unsigned char*)"..."; struct _tagged_string _temp558;
_temp558.curr= _temp557; _temp558.base= _temp557; _temp558.last_plus_one=
_temp557 + 4; _temp558;})); Cyc_Absyndump_dumptyp(( void*)(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->rgn); if((( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->inject){ Cyc_Absyndump_dump(({ unsigned char*
_temp559=( unsigned char*)" inject "; struct _tagged_string _temp560; _temp560.curr=
_temp559; _temp560.base= _temp559; _temp560.last_plus_one= _temp559 + 9;
_temp560;}));} Cyc_Absyndump_dumpfunarg( _temp555);}} if( effopt != 0){ Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpeff(( void*)(( struct Cyc_Core_Opt*) _check_null( effopt))->v);}
if( rgn_po != 0){ Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dump_rgnpo(
rgn_po);} Cyc_Absyndump_dump_char(( int)')');} void Cyc_Absyndump_dumptyp( void*
t){(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( int), int)) Cyc_Absyndump_dumptqtd)(({
struct Cyc_Absyn_Tqual _temp561; _temp561.q_const= 0; _temp561.q_volatile= 0;
_temp561.q_restrict= 0; _temp561;}), t,( void(*)( int x)) Cyc_Absyndump_ignore,
0);} void Cyc_Absyndump_dumpdesignator( void* d){ void* _temp562= d; struct Cyc_Absyn_Exp*
_temp568; struct _tagged_string* _temp570; _LL564: if(*(( int*) _temp562) == Cyc_Absyn_ArrayElement){
_LL569: _temp568=(( struct Cyc_Absyn_ArrayElement_struct*) _temp562)->f1; goto
_LL565;} else{ goto _LL566;} _LL566: if(*(( int*) _temp562) == Cyc_Absyn_FieldName){
_LL571: _temp570=(( struct Cyc_Absyn_FieldName_struct*) _temp562)->f1; goto
_LL567;} else{ goto _LL563;} _LL565: Cyc_Absyndump_dump(({ unsigned char*
_temp572=( unsigned char*)".["; struct _tagged_string _temp573; _temp573.curr=
_temp572; _temp573.base= _temp572; _temp573.last_plus_one= _temp572 + 3;
_temp573;})); Cyc_Absyndump_dumpexp( _temp568); Cyc_Absyndump_dump_char(( int)']');
goto _LL563; _LL567: Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp570); goto _LL563; _LL563:;} struct _tuple8{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; void Cyc_Absyndump_dumpde( struct _tuple8* de){((
void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpdesignator,(* de).f1,({ unsigned char* _temp574=(
unsigned char*)""; struct _tagged_string _temp575; _temp575.curr= _temp574;
_temp575.base= _temp574; _temp575.last_plus_one= _temp574 + 1; _temp575;}),({
unsigned char* _temp576=( unsigned char*)"="; struct _tagged_string _temp577;
_temp577.curr= _temp576; _temp577.base= _temp576; _temp577.last_plus_one=
_temp576 + 2; _temp577;}),({ unsigned char* _temp578=( unsigned char*)"=";
struct _tagged_string _temp579; _temp579.curr= _temp578; _temp579.base= _temp578;
_temp579.last_plus_one= _temp578 + 2; _temp579;})); Cyc_Absyndump_dumpexp((* de).f2);}
void Cyc_Absyndump_dumpexps_prec( int inprec, struct Cyc_List_List* es){(( void(*)(
void(* f)( int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group_c)(
Cyc_Absyndump_dumpexp_prec, inprec, es,({ unsigned char* _temp580=(
unsigned char*)""; struct _tagged_string _temp581; _temp581.curr= _temp580;
_temp581.base= _temp580; _temp581.last_plus_one= _temp580 + 1; _temp581;}),({
unsigned char* _temp582=( unsigned char*)""; struct _tagged_string _temp583;
_temp583.curr= _temp582; _temp583.base= _temp582; _temp583.last_plus_one=
_temp582 + 1; _temp583;}),({ unsigned char* _temp584=( unsigned char*)",";
struct _tagged_string _temp585; _temp585.curr= _temp584; _temp585.base= _temp584;
_temp585.last_plus_one= _temp584 + 2; _temp585;}));} void Cyc_Absyndump_dumpexp_prec(
int inprec, struct Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec( e); if(
inprec >= myprec){ Cyc_Absyndump_dump_nospace(({ unsigned char* _temp586=(
unsigned char*)"("; struct _tagged_string _temp587; _temp587.curr= _temp586;
_temp587.base= _temp586; _temp587.last_plus_one= _temp586 + 2; _temp587;}));}{
void* _temp588=( void*) e->r; void* _temp682; unsigned char _temp684; void*
_temp686; void* _temp688; short _temp690; void* _temp692; void* _temp694; int
_temp696; void* _temp698; void* _temp700; int _temp702; void* _temp704; void*
_temp706; long long _temp708; void* _temp710; void* _temp712; struct
_tagged_string _temp714; void* _temp716; void* _temp718; struct _tagged_string
_temp720; struct _tuple0* _temp722; void* _temp724; struct _tuple0* _temp726;
struct Cyc_List_List* _temp728; void* _temp730; struct Cyc_Absyn_Exp* _temp732;
struct Cyc_Core_Opt* _temp734; struct Cyc_Absyn_Exp* _temp736; void* _temp738;
struct Cyc_Absyn_Exp* _temp740; void* _temp742; struct Cyc_Absyn_Exp* _temp744;
void* _temp746; struct Cyc_Absyn_Exp* _temp748; void* _temp750; struct Cyc_Absyn_Exp*
_temp752; struct Cyc_Absyn_Exp* _temp754; struct Cyc_Absyn_Exp* _temp756; struct
Cyc_Absyn_Exp* _temp758; struct Cyc_Absyn_Exp* _temp760; struct Cyc_Absyn_Exp*
_temp762; struct Cyc_List_List* _temp764; struct Cyc_Absyn_Exp* _temp766; struct
Cyc_Absyn_VarargCallInfo* _temp768; struct Cyc_List_List* _temp770; struct Cyc_Absyn_Exp*
_temp772; struct Cyc_Absyn_Exp* _temp774; struct Cyc_Absyn_Exp* _temp776; struct
Cyc_List_List* _temp778; struct Cyc_Absyn_Exp* _temp780; struct Cyc_Absyn_Exp*
_temp782; void* _temp784; struct Cyc_Absyn_Exp* _temp786; struct Cyc_Absyn_Exp*
_temp788; struct Cyc_Absyn_Exp* _temp790; void* _temp792; struct Cyc_Absyn_Exp*
_temp794; struct _tagged_string* _temp796; void* _temp798; struct Cyc_Absyn_Exp*
_temp800; struct _tagged_string* _temp802; struct Cyc_Absyn_Exp* _temp804;
struct _tagged_string* _temp806; struct Cyc_Absyn_Exp* _temp808; struct Cyc_Absyn_Exp*
_temp810; struct Cyc_Absyn_Exp* _temp812; struct Cyc_List_List* _temp814; struct
Cyc_List_List* _temp816; struct _tuple1* _temp818; struct Cyc_List_List*
_temp820; struct Cyc_Absyn_Exp* _temp822; struct Cyc_Absyn_Exp* _temp824; struct
Cyc_Absyn_Vardecl* _temp826; struct Cyc_Absyn_Structdecl* _temp828; struct Cyc_List_List*
_temp830; struct Cyc_Core_Opt* _temp832; struct _tuple0* _temp834; struct Cyc_List_List*
_temp836; void* _temp838; struct Cyc_Absyn_Tunionfield* _temp840; struct Cyc_Absyn_Tuniondecl*
_temp842; struct Cyc_List_List* _temp844; struct Cyc_Core_Opt* _temp846; struct
Cyc_Core_Opt* _temp848; struct Cyc_Absyn_Enumfield* _temp850; struct Cyc_Absyn_Enumdecl*
_temp852; struct _tuple0* _temp854; void* _temp856; struct Cyc_Absyn_Exp*
_temp858; struct Cyc_List_List* _temp860; struct Cyc_Core_Opt* _temp862; struct
Cyc_Absyn_Stmt* _temp864; struct Cyc_Absyn_Fndecl* _temp866; struct Cyc_Absyn_Exp*
_temp868; _LL590: if(*(( int*) _temp588) == Cyc_Absyn_Const_e){ _LL683: _temp682=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp588)->f1; if(( unsigned int)
_temp682 > 1u?*(( int*) _temp682) == Cyc_Absyn_Char_c: 0){ _LL687: _temp686=(
void*)(( struct Cyc_Absyn_Char_c_struct*) _temp682)->f1; goto _LL685; _LL685:
_temp684=(( struct Cyc_Absyn_Char_c_struct*) _temp682)->f2; goto _LL591;} else{
goto _LL592;}} else{ goto _LL592;} _LL592: if(*(( int*) _temp588) == Cyc_Absyn_Const_e){
_LL689: _temp688=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp588)->f1; if((
unsigned int) _temp688 > 1u?*(( int*) _temp688) == Cyc_Absyn_Short_c: 0){ _LL693:
_temp692=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp688)->f1; goto _LL691;
_LL691: _temp690=(( struct Cyc_Absyn_Short_c_struct*) _temp688)->f2; goto _LL593;}
else{ goto _LL594;}} else{ goto _LL594;} _LL594: if(*(( int*) _temp588) == Cyc_Absyn_Const_e){
_LL695: _temp694=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp588)->f1; if((
unsigned int) _temp694 > 1u?*(( int*) _temp694) == Cyc_Absyn_Int_c: 0){ _LL699:
_temp698=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp694)->f1; if( _temp698
==( void*) Cyc_Absyn_Signed){ goto _LL697;} else{ goto _LL596;} _LL697: _temp696=((
struct Cyc_Absyn_Int_c_struct*) _temp694)->f2; goto _LL595;} else{ goto _LL596;}}
else{ goto _LL596;} _LL596: if(*(( int*) _temp588) == Cyc_Absyn_Const_e){ _LL701:
_temp700=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp588)->f1; if((
unsigned int) _temp700 > 1u?*(( int*) _temp700) == Cyc_Absyn_Int_c: 0){ _LL705:
_temp704=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp700)->f1; if( _temp704
==( void*) Cyc_Absyn_Unsigned){ goto _LL703;} else{ goto _LL598;} _LL703:
_temp702=(( struct Cyc_Absyn_Int_c_struct*) _temp700)->f2; goto _LL597;} else{
goto _LL598;}} else{ goto _LL598;} _LL598: if(*(( int*) _temp588) == Cyc_Absyn_Const_e){
_LL707: _temp706=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp588)->f1; if((
unsigned int) _temp706 > 1u?*(( int*) _temp706) == Cyc_Absyn_LongLong_c: 0){
_LL711: _temp710=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp706)->f1;
goto _LL709; _LL709: _temp708=(( struct Cyc_Absyn_LongLong_c_struct*) _temp706)->f2;
goto _LL599;} else{ goto _LL600;}} else{ goto _LL600;} _LL600: if(*(( int*)
_temp588) == Cyc_Absyn_Const_e){ _LL713: _temp712=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp588)->f1; if(( unsigned int) _temp712 > 1u?*(( int*) _temp712) == Cyc_Absyn_Float_c:
0){ _LL715: _temp714=(( struct Cyc_Absyn_Float_c_struct*) _temp712)->f1; goto
_LL601;} else{ goto _LL602;}} else{ goto _LL602;} _LL602: if(*(( int*) _temp588)
== Cyc_Absyn_Const_e){ _LL717: _temp716=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp588)->f1; if( _temp716 ==( void*) Cyc_Absyn_Null_c){ goto _LL603;} else{
goto _LL604;}} else{ goto _LL604;} _LL604: if(*(( int*) _temp588) == Cyc_Absyn_Const_e){
_LL719: _temp718=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp588)->f1; if((
unsigned int) _temp718 > 1u?*(( int*) _temp718) == Cyc_Absyn_String_c: 0){
_LL721: _temp720=(( struct Cyc_Absyn_String_c_struct*) _temp718)->f1; goto
_LL605;} else{ goto _LL606;}} else{ goto _LL606;} _LL606: if(*(( int*) _temp588)
== Cyc_Absyn_UnknownId_e){ _LL723: _temp722=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp588)->f1; goto _LL607;} else{ goto _LL608;} _LL608: if(*(( int*) _temp588)
== Cyc_Absyn_Var_e){ _LL727: _temp726=(( struct Cyc_Absyn_Var_e_struct*)
_temp588)->f1; goto _LL725; _LL725: _temp724=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp588)->f2; goto _LL609;} else{ goto _LL610;} _LL610: if(*(( int*) _temp588)
== Cyc_Absyn_Primop_e){ _LL731: _temp730=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp588)->f1; goto _LL729; _LL729: _temp728=(( struct Cyc_Absyn_Primop_e_struct*)
_temp588)->f2; goto _LL611;} else{ goto _LL612;} _LL612: if(*(( int*) _temp588)
== Cyc_Absyn_AssignOp_e){ _LL737: _temp736=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp588)->f1; goto _LL735; _LL735: _temp734=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp588)->f2; goto _LL733; _LL733: _temp732=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp588)->f3; goto _LL613;} else{ goto _LL614;} _LL614: if(*(( int*) _temp588)
== Cyc_Absyn_Increment_e){ _LL741: _temp740=(( struct Cyc_Absyn_Increment_e_struct*)
_temp588)->f1; goto _LL739; _LL739: _temp738=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp588)->f2; if( _temp738 ==( void*) Cyc_Absyn_PreInc){ goto _LL615;} else{
goto _LL616;}} else{ goto _LL616;} _LL616: if(*(( int*) _temp588) == Cyc_Absyn_Increment_e){
_LL745: _temp744=(( struct Cyc_Absyn_Increment_e_struct*) _temp588)->f1; goto
_LL743; _LL743: _temp742=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp588)->f2; if( _temp742 ==( void*) Cyc_Absyn_PreDec){ goto _LL617;} else{
goto _LL618;}} else{ goto _LL618;} _LL618: if(*(( int*) _temp588) == Cyc_Absyn_Increment_e){
_LL749: _temp748=(( struct Cyc_Absyn_Increment_e_struct*) _temp588)->f1; goto
_LL747; _LL747: _temp746=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp588)->f2; if( _temp746 ==( void*) Cyc_Absyn_PostInc){ goto _LL619;} else{
goto _LL620;}} else{ goto _LL620;} _LL620: if(*(( int*) _temp588) == Cyc_Absyn_Increment_e){
_LL753: _temp752=(( struct Cyc_Absyn_Increment_e_struct*) _temp588)->f1; goto
_LL751; _LL751: _temp750=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp588)->f2; if( _temp750 ==( void*) Cyc_Absyn_PostDec){ goto _LL621;} else{
goto _LL622;}} else{ goto _LL622;} _LL622: if(*(( int*) _temp588) == Cyc_Absyn_Conditional_e){
_LL759: _temp758=(( struct Cyc_Absyn_Conditional_e_struct*) _temp588)->f1; goto
_LL757; _LL757: _temp756=(( struct Cyc_Absyn_Conditional_e_struct*) _temp588)->f2;
goto _LL755; _LL755: _temp754=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp588)->f3; goto _LL623;} else{ goto _LL624;} _LL624: if(*(( int*) _temp588)
== Cyc_Absyn_SeqExp_e){ _LL763: _temp762=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp588)->f1; goto _LL761; _LL761: _temp760=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp588)->f2; goto _LL625;} else{ goto _LL626;} _LL626: if(*(( int*) _temp588)
== Cyc_Absyn_UnknownCall_e){ _LL767: _temp766=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp588)->f1; goto _LL765; _LL765: _temp764=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp588)->f2; goto _LL627;} else{ goto _LL628;} _LL628: if(*(( int*) _temp588)
== Cyc_Absyn_FnCall_e){ _LL773: _temp772=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp588)->f1; goto _LL771; _LL771: _temp770=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp588)->f2; goto _LL769; _LL769: _temp768=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp588)->f3; goto _LL629;} else{ goto _LL630;} _LL630: if(*(( int*) _temp588)
== Cyc_Absyn_Throw_e){ _LL775: _temp774=(( struct Cyc_Absyn_Throw_e_struct*)
_temp588)->f1; goto _LL631;} else{ goto _LL632;} _LL632: if(*(( int*) _temp588)
== Cyc_Absyn_NoInstantiate_e){ _LL777: _temp776=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp588)->f1; goto _LL633;} else{ goto _LL634;} _LL634: if(*(( int*) _temp588)
== Cyc_Absyn_Instantiate_e){ _LL781: _temp780=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp588)->f1; goto _LL779; _LL779: _temp778=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp588)->f2; goto _LL635;} else{ goto _LL636;} _LL636: if(*(( int*) _temp588)
== Cyc_Absyn_Cast_e){ _LL785: _temp784=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp588)->f1; goto _LL783; _LL783: _temp782=(( struct Cyc_Absyn_Cast_e_struct*)
_temp588)->f2; goto _LL637;} else{ goto _LL638;} _LL638: if(*(( int*) _temp588)
== Cyc_Absyn_Address_e){ _LL787: _temp786=(( struct Cyc_Absyn_Address_e_struct*)
_temp588)->f1; goto _LL639;} else{ goto _LL640;} _LL640: if(*(( int*) _temp588)
== Cyc_Absyn_New_e){ _LL791: _temp790=(( struct Cyc_Absyn_New_e_struct*)
_temp588)->f1; goto _LL789; _LL789: _temp788=(( struct Cyc_Absyn_New_e_struct*)
_temp588)->f2; goto _LL641;} else{ goto _LL642;} _LL642: if(*(( int*) _temp588)
== Cyc_Absyn_Sizeoftyp_e){ _LL793: _temp792=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp588)->f1; goto _LL643;} else{ goto _LL644;} _LL644: if(*(( int*) _temp588)
== Cyc_Absyn_Sizeofexp_e){ _LL795: _temp794=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp588)->f1; goto _LL645;} else{ goto _LL646;} _LL646: if(*(( int*) _temp588)
== Cyc_Absyn_Offsetof_e){ _LL799: _temp798=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp588)->f1; goto _LL797; _LL797: _temp796=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp588)->f2; goto _LL647;} else{ goto _LL648;} _LL648: if(*(( int*) _temp588)
== Cyc_Absyn_Deref_e){ _LL801: _temp800=(( struct Cyc_Absyn_Deref_e_struct*)
_temp588)->f1; goto _LL649;} else{ goto _LL650;} _LL650: if(*(( int*) _temp588)
== Cyc_Absyn_StructMember_e){ _LL805: _temp804=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp588)->f1; goto _LL803; _LL803: _temp802=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp588)->f2; goto _LL651;} else{ goto _LL652;} _LL652: if(*(( int*) _temp588)
== Cyc_Absyn_StructArrow_e){ _LL809: _temp808=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp588)->f1; goto _LL807; _LL807: _temp806=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp588)->f2; goto _LL653;} else{ goto _LL654;} _LL654: if(*(( int*) _temp588)
== Cyc_Absyn_Subscript_e){ _LL813: _temp812=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp588)->f1; goto _LL811; _LL811: _temp810=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp588)->f2; goto _LL655;} else{ goto _LL656;} _LL656: if(*(( int*) _temp588)
== Cyc_Absyn_Tuple_e){ _LL815: _temp814=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp588)->f1; goto _LL657;} else{ goto _LL658;} _LL658: if(*(( int*) _temp588)
== Cyc_Absyn_CompoundLit_e){ _LL819: _temp818=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp588)->f1; goto _LL817; _LL817: _temp816=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp588)->f2; goto _LL659;} else{ goto _LL660;} _LL660: if(*(( int*) _temp588)
== Cyc_Absyn_Array_e){ _LL821: _temp820=(( struct Cyc_Absyn_Array_e_struct*)
_temp588)->f1; goto _LL661;} else{ goto _LL662;} _LL662: if(*(( int*) _temp588)
== Cyc_Absyn_Comprehension_e){ _LL827: _temp826=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp588)->f1; goto _LL825; _LL825: _temp824=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp588)->f2; goto _LL823; _LL823: _temp822=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp588)->f3; goto _LL663;} else{ goto _LL664;} _LL664: if(*(( int*) _temp588)
== Cyc_Absyn_Struct_e){ _LL835: _temp834=(( struct Cyc_Absyn_Struct_e_struct*)
_temp588)->f1; goto _LL833; _LL833: _temp832=(( struct Cyc_Absyn_Struct_e_struct*)
_temp588)->f2; goto _LL831; _LL831: _temp830=(( struct Cyc_Absyn_Struct_e_struct*)
_temp588)->f3; goto _LL829; _LL829: _temp828=(( struct Cyc_Absyn_Struct_e_struct*)
_temp588)->f4; goto _LL665;} else{ goto _LL666;} _LL666: if(*(( int*) _temp588)
== Cyc_Absyn_AnonStruct_e){ _LL839: _temp838=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp588)->f1; goto _LL837; _LL837: _temp836=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp588)->f2; goto _LL667;} else{ goto _LL668;} _LL668: if(*(( int*) _temp588)
== Cyc_Absyn_Tunion_e){ _LL849: _temp848=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp588)->f1; goto _LL847; _LL847: _temp846=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp588)->f2; goto _LL845; _LL845: _temp844=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp588)->f3; goto _LL843; _LL843: _temp842=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp588)->f4; goto _LL841; _LL841: _temp840=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp588)->f5; goto _LL669;} else{ goto _LL670;} _LL670: if(*(( int*) _temp588)
== Cyc_Absyn_Enum_e){ _LL855: _temp854=(( struct Cyc_Absyn_Enum_e_struct*)
_temp588)->f1; goto _LL853; _LL853: _temp852=(( struct Cyc_Absyn_Enum_e_struct*)
_temp588)->f2; goto _LL851; _LL851: _temp850=(( struct Cyc_Absyn_Enum_e_struct*)
_temp588)->f3; goto _LL671;} else{ goto _LL672;} _LL672: if(*(( int*) _temp588)
== Cyc_Absyn_Malloc_e){ _LL859: _temp858=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp588)->f1; goto _LL857; _LL857: _temp856=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp588)->f2; goto _LL673;} else{ goto _LL674;} _LL674: if(*(( int*) _temp588)
== Cyc_Absyn_UnresolvedMem_e){ _LL863: _temp862=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp588)->f1; goto _LL861; _LL861: _temp860=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp588)->f2; goto _LL675;} else{ goto _LL676;} _LL676: if(*(( int*) _temp588)
== Cyc_Absyn_StmtExp_e){ _LL865: _temp864=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp588)->f1; goto _LL677;} else{ goto _LL678;} _LL678: if(*(( int*) _temp588)
== Cyc_Absyn_Codegen_e){ _LL867: _temp866=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp588)->f1; goto _LL679;} else{ goto _LL680;} _LL680: if(*(( int*) _temp588)
== Cyc_Absyn_Fill_e){ _LL869: _temp868=(( struct Cyc_Absyn_Fill_e_struct*)
_temp588)->f1; goto _LL681;} else{ goto _LL589;} _LL591: Cyc_Absyndump_dump_char((
int)'\''); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_char_escape( _temp684)); Cyc_Absyndump_dump_char((
int)'\''); goto _LL589; _LL593: Cyc_Absyndump_dump(( struct _tagged_string) Cyc_Core_string_of_int((
int) _temp690)); goto _LL589; _LL595: Cyc_Absyndump_dump(( struct _tagged_string)
Cyc_Core_string_of_int( _temp696)); goto _LL589; _LL597: Cyc_Absyndump_dump((
struct _tagged_string) Cyc_Core_string_of_int( _temp702)); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp870=( unsigned char*)"u"; struct _tagged_string _temp871;
_temp871.curr= _temp870; _temp871.base= _temp870; _temp871.last_plus_one=
_temp870 + 2; _temp871;})); goto _LL589; _LL599: Cyc_Absyndump_dump(({
unsigned char* _temp872=( unsigned char*)"<<FIX LONG LONG CONSTANT>>"; struct
_tagged_string _temp873; _temp873.curr= _temp872; _temp873.base= _temp872;
_temp873.last_plus_one= _temp872 + 27; _temp873;})); goto _LL589; _LL601: Cyc_Absyndump_dump(
_temp714); goto _LL589; _LL603: Cyc_Absyndump_dump(({ unsigned char* _temp874=(
unsigned char*)"null"; struct _tagged_string _temp875; _temp875.curr= _temp874;
_temp875.base= _temp874; _temp875.last_plus_one= _temp874 + 5; _temp875;}));
goto _LL589; _LL605: Cyc_Absyndump_dump_char(( int)'"'); Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_string_escape( _temp720)); Cyc_Absyndump_dump_char(( int)'"'); goto
_LL589; _LL607: _temp726= _temp722; goto _LL609; _LL609: Cyc_Absyndump_dumpqvar(
_temp726); goto _LL589; _LL611: { struct _tagged_string _temp876= Cyc_Absynpp_prim2str(
_temp730); goto _LL877; _LL877: if( ! Cyc_Absyn_is_format_prim( _temp730)){
switch((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp728)){ case 1:
_LL878: if( _temp730 ==( void*) Cyc_Absyn_Size){ Cyc_Absyndump_dumpexp_prec(
myprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp728))->hd);
Cyc_Absyndump_dump(({ unsigned char* _temp880=( unsigned char*)".size"; struct
_tagged_string _temp881; _temp881.curr= _temp880; _temp881.base= _temp880;
_temp881.last_plus_one= _temp880 + 6; _temp881;}));} else{ Cyc_Absyndump_dump(
_temp876); Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp728))->hd);} break; case 2: _LL879: Cyc_Absyndump_dumpexp_prec(
myprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp728))->hd);
Cyc_Absyndump_dump( _temp876); Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp728))->tl))->hd); break; default: _LL882:( void) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp884=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp884[ 0]=({ struct Cyc_Core_Failure_struct
_temp885; _temp885.tag= Cyc_Core_Failure; _temp885.f1=({ unsigned char* _temp886=(
unsigned char*)"Absyndump -- Bad number of arguments to primop"; struct
_tagged_string _temp887; _temp887.curr= _temp886; _temp887.base= _temp886;
_temp887.last_plus_one= _temp886 + 47; _temp887;}); _temp885;}); _temp884;}));}}
else{ Cyc_Absyndump_dump( _temp876); Cyc_Absyndump_dump_nospace(({ unsigned char*
_temp888=( unsigned char*)"("; struct _tagged_string _temp889; _temp889.curr=
_temp888; _temp889.base= _temp888; _temp889.last_plus_one= _temp888 + 2;
_temp889;})); Cyc_Absyndump_dumpexps_prec( 20, _temp728); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp890=( unsigned char*)")"; struct _tagged_string _temp891;
_temp891.curr= _temp890; _temp891.base= _temp890; _temp891.last_plus_one=
_temp890 + 2; _temp891;}));} goto _LL589;} _LL613: Cyc_Absyndump_dumpexp_prec(
myprec, _temp736); if( _temp734 != 0){ Cyc_Absyndump_dump( Cyc_Absynpp_prim2str((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp734))->v));} Cyc_Absyndump_dump_nospace(({
unsigned char* _temp892=( unsigned char*)"="; struct _tagged_string _temp893;
_temp893.curr= _temp892; _temp893.base= _temp892; _temp893.last_plus_one=
_temp892 + 2; _temp893;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp732); goto
_LL589; _LL615: Cyc_Absyndump_dump(({ unsigned char* _temp894=( unsigned char*)"++";
struct _tagged_string _temp895; _temp895.curr= _temp894; _temp895.base= _temp894;
_temp895.last_plus_one= _temp894 + 3; _temp895;})); Cyc_Absyndump_dumpexp_prec(
myprec, _temp740); goto _LL589; _LL617: Cyc_Absyndump_dump(({ unsigned char*
_temp896=( unsigned char*)"--"; struct _tagged_string _temp897; _temp897.curr=
_temp896; _temp897.base= _temp896; _temp897.last_plus_one= _temp896 + 3;
_temp897;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp744); goto _LL589; _LL619:
Cyc_Absyndump_dumpexp_prec( myprec, _temp748); Cyc_Absyndump_dump(({
unsigned char* _temp898=( unsigned char*)"++"; struct _tagged_string _temp899;
_temp899.curr= _temp898; _temp899.base= _temp898; _temp899.last_plus_one=
_temp898 + 3; _temp899;})); goto _LL589; _LL621: Cyc_Absyndump_dumpexp_prec(
myprec, _temp752); Cyc_Absyndump_dump(({ unsigned char* _temp900=( unsigned char*)"--";
struct _tagged_string _temp901; _temp901.curr= _temp900; _temp901.base= _temp900;
_temp901.last_plus_one= _temp900 + 3; _temp901;})); goto _LL589; _LL623: Cyc_Absyndump_dumpexp_prec(
myprec, _temp758); Cyc_Absyndump_dump_char(( int)'?'); Cyc_Absyndump_dumpexp_prec(
0, _temp756); Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp754); goto _LL589; _LL625: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumpexp_prec(
myprec, _temp762); Cyc_Absyndump_dump_char(( int)','); Cyc_Absyndump_dumpexp_prec(
myprec, _temp760); Cyc_Absyndump_dump_char(( int)')'); goto _LL589; _LL627:
_temp772= _temp766; _temp770= _temp764; goto _LL629; _LL629: Cyc_Absyndump_dumpexp_prec(
myprec, _temp772); Cyc_Absyndump_dump_nospace(({ unsigned char* _temp902=(
unsigned char*)"("; struct _tagged_string _temp903; _temp903.curr= _temp902;
_temp903.base= _temp902; _temp903.last_plus_one= _temp902 + 2; _temp903;})); Cyc_Absyndump_dumpexps_prec(
20, _temp770); Cyc_Absyndump_dump_nospace(({ unsigned char* _temp904=(
unsigned char*)")"; struct _tagged_string _temp905; _temp905.curr= _temp904;
_temp905.base= _temp904; _temp905.last_plus_one= _temp904 + 2; _temp905;}));
goto _LL589; _LL631: Cyc_Absyndump_dump(({ unsigned char* _temp906=(
unsigned char*)"throw"; struct _tagged_string _temp907; _temp907.curr= _temp906;
_temp907.base= _temp906; _temp907.last_plus_one= _temp906 + 6; _temp907;})); Cyc_Absyndump_dumpexp_prec(
myprec, _temp774); goto _LL589; _LL633: _temp780= _temp776; goto _LL635; _LL635:
Cyc_Absyndump_dumpexp_prec( inprec, _temp780); goto _LL589; _LL637: Cyc_Absyndump_dump_char((
int)'('); Cyc_Absyndump_dumptyp( _temp784); Cyc_Absyndump_dump_char(( int)')');
Cyc_Absyndump_dumpexp_prec( myprec, _temp782); goto _LL589; _LL639: Cyc_Absyndump_dump_char((
int)'&'); Cyc_Absyndump_dumpexp_prec( myprec, _temp786); goto _LL589; _LL641:
Cyc_Absyndump_dump(({ unsigned char* _temp908=( unsigned char*)"new "; struct
_tagged_string _temp909; _temp909.curr= _temp908; _temp909.base= _temp908;
_temp909.last_plus_one= _temp908 + 5; _temp909;})); Cyc_Absyndump_dumpexp_prec(
myprec, _temp788); goto _LL589; _LL643: Cyc_Absyndump_dump(({ unsigned char*
_temp910=( unsigned char*)"sizeof("; struct _tagged_string _temp911; _temp911.curr=
_temp910; _temp911.base= _temp910; _temp911.last_plus_one= _temp910 + 8;
_temp911;})); Cyc_Absyndump_dumptyp( _temp792); Cyc_Absyndump_dump_char(( int)')');
goto _LL589; _LL645: Cyc_Absyndump_dump(({ unsigned char* _temp912=(
unsigned char*)"sizeof("; struct _tagged_string _temp913; _temp913.curr=
_temp912; _temp913.base= _temp912; _temp913.last_plus_one= _temp912 + 8;
_temp913;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp794); Cyc_Absyndump_dump_char((
int)')'); goto _LL589; _LL647: Cyc_Absyndump_dump(({ unsigned char* _temp914=(
unsigned char*)"offsetof("; struct _tagged_string _temp915; _temp915.curr=
_temp914; _temp915.base= _temp914; _temp915.last_plus_one= _temp914 + 10;
_temp915;})); Cyc_Absyndump_dumptyp( _temp798); Cyc_Absyndump_dump_char(( int)',');
Cyc_Absyndump_dump_nospace(* _temp796); Cyc_Absyndump_dump_char(( int)')'); goto
_LL589; _LL649: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp800); goto _LL589; _LL651: Cyc_Absyndump_dumpexp_prec( myprec,
_temp804); Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp802); goto _LL589; _LL653: Cyc_Absyndump_dumpexp_prec( myprec, _temp808);
Cyc_Absyndump_dump_nospace(({ unsigned char* _temp916=( unsigned char*)"->";
struct _tagged_string _temp917; _temp917.curr= _temp916; _temp917.base= _temp916;
_temp917.last_plus_one= _temp916 + 3; _temp917;})); Cyc_Absyndump_dump_nospace(*
_temp806); goto _LL589; _LL655: Cyc_Absyndump_dumpexp_prec( myprec, _temp812);
Cyc_Absyndump_dump_char(( int)'['); Cyc_Absyndump_dumpexp( _temp810); Cyc_Absyndump_dump_char((
int)']'); goto _LL589; _LL657: Cyc_Absyndump_dump(({ unsigned char* _temp918=(
unsigned char*)"$("; struct _tagged_string _temp919; _temp919.curr= _temp918;
_temp919.base= _temp918; _temp919.last_plus_one= _temp918 + 3; _temp919;})); Cyc_Absyndump_dumpexps_prec(
20, _temp814); Cyc_Absyndump_dump_char(( int)')'); goto _LL589; _LL659: Cyc_Absyndump_dump_char((
int)'('); Cyc_Absyndump_dumptyp((* _temp818).f3); Cyc_Absyndump_dump_char(( int)')');((
void(*)( void(* f)( struct _tuple8*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp816,({ unsigned char* _temp920=( unsigned char*)"{";
struct _tagged_string _temp921; _temp921.curr= _temp920; _temp921.base= _temp920;
_temp921.last_plus_one= _temp920 + 2; _temp921;}),({ unsigned char* _temp922=(
unsigned char*)"}"; struct _tagged_string _temp923; _temp923.curr= _temp922;
_temp923.base= _temp922; _temp923.last_plus_one= _temp922 + 2; _temp923;}),({
unsigned char* _temp924=( unsigned char*)","; struct _tagged_string _temp925;
_temp925.curr= _temp924; _temp925.base= _temp924; _temp925.last_plus_one=
_temp924 + 2; _temp925;})); goto _LL589; _LL661:(( void(*)( void(* f)( struct
_tuple8*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp820,({ unsigned char* _temp926=( unsigned char*)"{"; struct _tagged_string
_temp927; _temp927.curr= _temp926; _temp927.base= _temp926; _temp927.last_plus_one=
_temp926 + 2; _temp927;}),({ unsigned char* _temp928=( unsigned char*)"}";
struct _tagged_string _temp929; _temp929.curr= _temp928; _temp929.base= _temp928;
_temp929.last_plus_one= _temp928 + 2; _temp929;}),({ unsigned char* _temp930=(
unsigned char*)","; struct _tagged_string _temp931; _temp931.curr= _temp930;
_temp931.base= _temp930; _temp931.last_plus_one= _temp930 + 2; _temp931;}));
goto _LL589; _LL663: Cyc_Absyndump_dump(({ unsigned char* _temp932=(
unsigned char*)"new {for"; struct _tagged_string _temp933; _temp933.curr=
_temp932; _temp933.base= _temp932; _temp933.last_plus_one= _temp932 + 9;
_temp933;})); Cyc_Absyndump_dump_str((* _temp826->name).f2); Cyc_Absyndump_dump_char((
int)'<'); Cyc_Absyndump_dumpexp( _temp824); Cyc_Absyndump_dump_char(( int)':');
Cyc_Absyndump_dumpexp( _temp822); Cyc_Absyndump_dump_char(( int)'}'); goto
_LL589; _LL665: Cyc_Absyndump_dumpqvar( _temp834);(( void(*)( void(* f)( struct
_tuple8*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp830,({ unsigned char* _temp934=( unsigned char*)"{"; struct _tagged_string
_temp935; _temp935.curr= _temp934; _temp935.base= _temp934; _temp935.last_plus_one=
_temp934 + 2; _temp935;}),({ unsigned char* _temp936=( unsigned char*)"}";
struct _tagged_string _temp937; _temp937.curr= _temp936; _temp937.base= _temp936;
_temp937.last_plus_one= _temp936 + 2; _temp937;}),({ unsigned char* _temp938=(
unsigned char*)","; struct _tagged_string _temp939; _temp939.curr= _temp938;
_temp939.base= _temp938; _temp939.last_plus_one= _temp938 + 2; _temp939;}));
goto _LL589; _LL667:(( void(*)( void(* f)( struct _tuple8*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde, _temp836,({ unsigned char*
_temp940=( unsigned char*)"{"; struct _tagged_string _temp941; _temp941.curr=
_temp940; _temp941.base= _temp940; _temp941.last_plus_one= _temp940 + 2;
_temp941;}),({ unsigned char* _temp942=( unsigned char*)"}"; struct
_tagged_string _temp943; _temp943.curr= _temp942; _temp943.base= _temp942;
_temp943.last_plus_one= _temp942 + 2; _temp943;}),({ unsigned char* _temp944=(
unsigned char*)","; struct _tagged_string _temp945; _temp945.curr= _temp944;
_temp945.base= _temp944; _temp945.last_plus_one= _temp944 + 2; _temp945;}));
goto _LL589; _LL669: Cyc_Absyndump_dumpqvar( _temp840->name); if( _temp844 != 0){((
void(*)( void(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpexp, _temp844,({ unsigned char* _temp946=( unsigned char*)"(";
struct _tagged_string _temp947; _temp947.curr= _temp946; _temp947.base= _temp946;
_temp947.last_plus_one= _temp946 + 2; _temp947;}),({ unsigned char* _temp948=(
unsigned char*)")"; struct _tagged_string _temp949; _temp949.curr= _temp948;
_temp949.base= _temp948; _temp949.last_plus_one= _temp948 + 2; _temp949;}),({
unsigned char* _temp950=( unsigned char*)","; struct _tagged_string _temp951;
_temp951.curr= _temp950; _temp951.base= _temp950; _temp951.last_plus_one=
_temp950 + 2; _temp951;}));} goto _LL589; _LL671: Cyc_Absyndump_dumpqvar(
_temp854); goto _LL589; _LL673: if( _temp858 != 0){ Cyc_Absyndump_dump(({
unsigned char* _temp952=( unsigned char*)"rmalloc("; struct _tagged_string
_temp953; _temp953.curr= _temp952; _temp953.base= _temp952; _temp953.last_plus_one=
_temp952 + 9; _temp953;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)
_check_null( _temp858)); Cyc_Absyndump_dump(({ unsigned char* _temp954=(
unsigned char*)","; struct _tagged_string _temp955; _temp955.curr= _temp954;
_temp955.base= _temp954; _temp955.last_plus_one= _temp954 + 2; _temp955;}));}
else{ Cyc_Absyndump_dump(({ unsigned char* _temp956=( unsigned char*)"malloc(";
struct _tagged_string _temp957; _temp957.curr= _temp956; _temp957.base= _temp956;
_temp957.last_plus_one= _temp956 + 8; _temp957;}));} Cyc_Absyndump_dump(({
unsigned char* _temp958=( unsigned char*)"sizeof("; struct _tagged_string
_temp959; _temp959.curr= _temp958; _temp959.base= _temp958; _temp959.last_plus_one=
_temp958 + 8; _temp959;})); Cyc_Absyndump_dumptyp( _temp856); Cyc_Absyndump_dump(({
unsigned char* _temp960=( unsigned char*)"))"; struct _tagged_string _temp961;
_temp961.curr= _temp960; _temp961.base= _temp960; _temp961.last_plus_one=
_temp960 + 3; _temp961;})); goto _LL589; _LL675:(( void(*)( void(* f)( struct
_tuple8*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp860,({ unsigned char* _temp962=( unsigned char*)"{"; struct _tagged_string
_temp963; _temp963.curr= _temp962; _temp963.base= _temp962; _temp963.last_plus_one=
_temp962 + 2; _temp963;}),({ unsigned char* _temp964=( unsigned char*)"}";
struct _tagged_string _temp965; _temp965.curr= _temp964; _temp965.base= _temp964;
_temp965.last_plus_one= _temp964 + 2; _temp965;}),({ unsigned char* _temp966=(
unsigned char*)","; struct _tagged_string _temp967; _temp967.curr= _temp966;
_temp967.base= _temp966; _temp967.last_plus_one= _temp966 + 2; _temp967;}));
goto _LL589; _LL677: Cyc_Absyndump_dump_nospace(({ unsigned char* _temp968=(
unsigned char*)"({"; struct _tagged_string _temp969; _temp969.curr= _temp968;
_temp969.base= _temp968; _temp969.last_plus_one= _temp968 + 3; _temp969;})); Cyc_Absyndump_dumpstmt(
_temp864); Cyc_Absyndump_dump_nospace(({ unsigned char* _temp970=( unsigned char*)"})";
struct _tagged_string _temp971; _temp971.curr= _temp970; _temp971.base= _temp970;
_temp971.last_plus_one= _temp970 + 3; _temp971;})); goto _LL589; _LL679: Cyc_Absyndump_dump(({
unsigned char* _temp972=( unsigned char*)"codegen("; struct _tagged_string
_temp973; _temp973.curr= _temp972; _temp973.base= _temp972; _temp973.last_plus_one=
_temp972 + 9; _temp973;})); Cyc_Absyndump_dumpdecl(({ struct Cyc_Absyn_Decl*
_temp974=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp974->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp975=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp975[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp976; _temp976.tag= Cyc_Absyn_Fn_d;
_temp976.f1= _temp866; _temp976;}); _temp975;})); _temp974->loc= e->loc;
_temp974;})); Cyc_Absyndump_dump(({ unsigned char* _temp977=( unsigned char*)")";
struct _tagged_string _temp978; _temp978.curr= _temp977; _temp978.base= _temp977;
_temp978.last_plus_one= _temp977 + 2; _temp978;})); goto _LL589; _LL681: Cyc_Absyndump_dump(({
unsigned char* _temp979=( unsigned char*)"fill("; struct _tagged_string _temp980;
_temp980.curr= _temp979; _temp980.base= _temp979; _temp980.last_plus_one=
_temp979 + 6; _temp980;})); Cyc_Absyndump_dumpexp( _temp868); Cyc_Absyndump_dump(({
unsigned char* _temp981=( unsigned char*)")"; struct _tagged_string _temp982;
_temp982.curr= _temp981; _temp982.base= _temp981; _temp982.last_plus_one=
_temp981 + 2; _temp982;})); goto _LL589; _LL589:;} if( inprec >= myprec){ Cyc_Absyndump_dump_char((
int)')');}} void Cyc_Absyndump_dumpexp( struct Cyc_Absyn_Exp* e){ Cyc_Absyndump_dumpexp_prec(
0, e);} void Cyc_Absyndump_dumpswitchclauses( struct Cyc_List_List* scs){ for( 0;
scs != 0; scs=(( struct Cyc_List_List*) _check_null( scs))->tl){ struct Cyc_Absyn_Switch_clause*
_temp983=( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
scs))->hd; goto _LL984; _LL984: if( _temp983->where_clause == 0?( void*)(
_temp983->pattern)->r ==( void*) Cyc_Absyn_Wild_p: 0){ Cyc_Absyndump_dump(({
unsigned char* _temp985=( unsigned char*)"default:"; struct _tagged_string
_temp986; _temp986.curr= _temp985; _temp986.base= _temp985; _temp986.last_plus_one=
_temp985 + 9; _temp986;}));} else{ Cyc_Absyndump_dump(({ unsigned char* _temp987=(
unsigned char*)"case"; struct _tagged_string _temp988; _temp988.curr= _temp987;
_temp988.base= _temp987; _temp988.last_plus_one= _temp987 + 5; _temp988;})); Cyc_Absyndump_dumppat(
_temp983->pattern); if( _temp983->where_clause != 0){ Cyc_Absyndump_dump(({
unsigned char* _temp989=( unsigned char*)"&&"; struct _tagged_string _temp990;
_temp990.curr= _temp989; _temp990.base= _temp989; _temp990.last_plus_one=
_temp989 + 3; _temp990;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)
_check_null( _temp983->where_clause));} Cyc_Absyndump_dump_nospace(({
unsigned char* _temp991=( unsigned char*)":"; struct _tagged_string _temp992;
_temp992.curr= _temp991; _temp992.base= _temp991; _temp992.last_plus_one=
_temp991 + 2; _temp992;}));} Cyc_Absyndump_dumpstmt( _temp983->body);}} void Cyc_Absyndump_dumpstmt(
struct Cyc_Absyn_Stmt* s){ void* _temp993=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1039; struct Cyc_Absyn_Stmt* _temp1041; struct Cyc_Absyn_Stmt* _temp1043;
struct Cyc_Absyn_Exp* _temp1045; struct Cyc_Absyn_Exp* _temp1047; struct Cyc_Absyn_Stmt*
_temp1049; struct Cyc_Absyn_Stmt* _temp1051; struct Cyc_Absyn_Exp* _temp1053;
struct Cyc_Absyn_Stmt* _temp1055; struct _tuple2 _temp1057; struct Cyc_Absyn_Stmt*
_temp1059; struct Cyc_Absyn_Exp* _temp1061; struct Cyc_Absyn_Stmt* _temp1063;
struct Cyc_Absyn_Stmt* _temp1065; struct Cyc_Absyn_Stmt* _temp1067; struct
_tagged_string* _temp1069; struct Cyc_Absyn_Stmt* _temp1071; struct _tuple2
_temp1073; struct Cyc_Absyn_Stmt* _temp1075; struct Cyc_Absyn_Exp* _temp1077;
struct _tuple2 _temp1079; struct Cyc_Absyn_Stmt* _temp1081; struct Cyc_Absyn_Exp*
_temp1083; struct Cyc_Absyn_Exp* _temp1085; struct Cyc_List_List* _temp1087;
struct Cyc_Absyn_Exp* _temp1089; struct Cyc_Absyn_Stmt* _temp1091; struct Cyc_Absyn_Decl*
_temp1093; struct Cyc_Absyn_Stmt* _temp1095; struct _tagged_string* _temp1097;
struct _tuple2 _temp1099; struct Cyc_Absyn_Stmt* _temp1101; struct Cyc_Absyn_Exp*
_temp1103; struct Cyc_Absyn_Stmt* _temp1105; struct Cyc_List_List* _temp1107;
struct Cyc_Absyn_Exp* _temp1109; struct Cyc_Absyn_Switch_clause** _temp1111;
struct Cyc_List_List* _temp1113; struct Cyc_Absyn_Switch_clause** _temp1115;
struct Cyc_List_List* _temp1117; struct Cyc_List_List* _temp1119; struct Cyc_Absyn_Stmt*
_temp1121; struct Cyc_Absyn_Stmt* _temp1123; struct Cyc_Absyn_Vardecl* _temp1125;
struct Cyc_Absyn_Tvar* _temp1127; struct Cyc_Absyn_Stmt* _temp1129; struct Cyc_Absyn_Stmt*
_temp1131; _LL995: if( _temp993 ==( void*) Cyc_Absyn_Skip_s){ goto _LL996;}
else{ goto _LL997;} _LL997: if(( unsigned int) _temp993 > 1u?*(( int*) _temp993)
== Cyc_Absyn_Exp_s: 0){ _LL1040: _temp1039=(( struct Cyc_Absyn_Exp_s_struct*)
_temp993)->f1; goto _LL998;} else{ goto _LL999;} _LL999: if(( unsigned int)
_temp993 > 1u?*(( int*) _temp993) == Cyc_Absyn_Seq_s: 0){ _LL1044: _temp1043=((
struct Cyc_Absyn_Seq_s_struct*) _temp993)->f1; goto _LL1042; _LL1042: _temp1041=((
struct Cyc_Absyn_Seq_s_struct*) _temp993)->f2; goto _LL1000;} else{ goto _LL1001;}
_LL1001: if(( unsigned int) _temp993 > 1u?*(( int*) _temp993) == Cyc_Absyn_Return_s:
0){ _LL1046: _temp1045=(( struct Cyc_Absyn_Return_s_struct*) _temp993)->f1; if(
_temp1045 == 0){ goto _LL1002;} else{ goto _LL1003;}} else{ goto _LL1003;}
_LL1003: if(( unsigned int) _temp993 > 1u?*(( int*) _temp993) == Cyc_Absyn_Return_s:
0){ _LL1048: _temp1047=(( struct Cyc_Absyn_Return_s_struct*) _temp993)->f1; goto
_LL1004;} else{ goto _LL1005;} _LL1005: if(( unsigned int) _temp993 > 1u?*(( int*)
_temp993) == Cyc_Absyn_IfThenElse_s: 0){ _LL1054: _temp1053=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp993)->f1; goto _LL1052; _LL1052: _temp1051=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp993)->f2; goto _LL1050; _LL1050: _temp1049=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp993)->f3; goto _LL1006;} else{ goto _LL1007;} _LL1007: if(( unsigned int)
_temp993 > 1u?*(( int*) _temp993) == Cyc_Absyn_While_s: 0){ _LL1058: _temp1057=((
struct Cyc_Absyn_While_s_struct*) _temp993)->f1; _LL1062: _temp1061= _temp1057.f1;
goto _LL1060; _LL1060: _temp1059= _temp1057.f2; goto _LL1056; _LL1056: _temp1055=((
struct Cyc_Absyn_While_s_struct*) _temp993)->f2; goto _LL1008;} else{ goto
_LL1009;} _LL1009: if(( unsigned int) _temp993 > 1u?*(( int*) _temp993) == Cyc_Absyn_Break_s:
0){ _LL1064: _temp1063=(( struct Cyc_Absyn_Break_s_struct*) _temp993)->f1; goto
_LL1010;} else{ goto _LL1011;} _LL1011: if(( unsigned int) _temp993 > 1u?*(( int*)
_temp993) == Cyc_Absyn_Continue_s: 0){ _LL1066: _temp1065=(( struct Cyc_Absyn_Continue_s_struct*)
_temp993)->f1; goto _LL1012;} else{ goto _LL1013;} _LL1013: if(( unsigned int)
_temp993 > 1u?*(( int*) _temp993) == Cyc_Absyn_Goto_s: 0){ _LL1070: _temp1069=((
struct Cyc_Absyn_Goto_s_struct*) _temp993)->f1; goto _LL1068; _LL1068: _temp1067=((
struct Cyc_Absyn_Goto_s_struct*) _temp993)->f2; goto _LL1014;} else{ goto
_LL1015;} _LL1015: if(( unsigned int) _temp993 > 1u?*(( int*) _temp993) == Cyc_Absyn_For_s:
0){ _LL1086: _temp1085=(( struct Cyc_Absyn_For_s_struct*) _temp993)->f1; goto
_LL1080; _LL1080: _temp1079=(( struct Cyc_Absyn_For_s_struct*) _temp993)->f2;
_LL1084: _temp1083= _temp1079.f1; goto _LL1082; _LL1082: _temp1081= _temp1079.f2;
goto _LL1074; _LL1074: _temp1073=(( struct Cyc_Absyn_For_s_struct*) _temp993)->f3;
_LL1078: _temp1077= _temp1073.f1; goto _LL1076; _LL1076: _temp1075= _temp1073.f2;
goto _LL1072; _LL1072: _temp1071=(( struct Cyc_Absyn_For_s_struct*) _temp993)->f4;
goto _LL1016;} else{ goto _LL1017;} _LL1017: if(( unsigned int) _temp993 > 1u?*((
int*) _temp993) == Cyc_Absyn_Switch_s: 0){ _LL1090: _temp1089=(( struct Cyc_Absyn_Switch_s_struct*)
_temp993)->f1; goto _LL1088; _LL1088: _temp1087=(( struct Cyc_Absyn_Switch_s_struct*)
_temp993)->f2; goto _LL1018;} else{ goto _LL1019;} _LL1019: if(( unsigned int)
_temp993 > 1u?*(( int*) _temp993) == Cyc_Absyn_Decl_s: 0){ _LL1094: _temp1093=((
struct Cyc_Absyn_Decl_s_struct*) _temp993)->f1; goto _LL1092; _LL1092: _temp1091=((
struct Cyc_Absyn_Decl_s_struct*) _temp993)->f2; goto _LL1020;} else{ goto
_LL1021;} _LL1021: if(( unsigned int) _temp993 > 1u?*(( int*) _temp993) == Cyc_Absyn_Label_s:
0){ _LL1098: _temp1097=(( struct Cyc_Absyn_Label_s_struct*) _temp993)->f1; goto
_LL1096; _LL1096: _temp1095=(( struct Cyc_Absyn_Label_s_struct*) _temp993)->f2;
goto _LL1022;} else{ goto _LL1023;} _LL1023: if(( unsigned int) _temp993 > 1u?*((
int*) _temp993) == Cyc_Absyn_Do_s: 0){ _LL1106: _temp1105=(( struct Cyc_Absyn_Do_s_struct*)
_temp993)->f1; goto _LL1100; _LL1100: _temp1099=(( struct Cyc_Absyn_Do_s_struct*)
_temp993)->f2; _LL1104: _temp1103= _temp1099.f1; goto _LL1102; _LL1102:
_temp1101= _temp1099.f2; goto _LL1024;} else{ goto _LL1025;} _LL1025: if((
unsigned int) _temp993 > 1u?*(( int*) _temp993) == Cyc_Absyn_SwitchC_s: 0){
_LL1110: _temp1109=(( struct Cyc_Absyn_SwitchC_s_struct*) _temp993)->f1; goto
_LL1108; _LL1108: _temp1107=(( struct Cyc_Absyn_SwitchC_s_struct*) _temp993)->f2;
goto _LL1026;} else{ goto _LL1027;} _LL1027: if(( unsigned int) _temp993 > 1u?*((
int*) _temp993) == Cyc_Absyn_Fallthru_s: 0){ _LL1114: _temp1113=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp993)->f1; if( _temp1113 == 0){ goto _LL1112;} else{ goto _LL1029;} _LL1112:
_temp1111=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp993)->f2; goto _LL1028;}
else{ goto _LL1029;} _LL1029: if(( unsigned int) _temp993 > 1u?*(( int*)
_temp993) == Cyc_Absyn_Fallthru_s: 0){ _LL1118: _temp1117=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp993)->f1; goto _LL1116; _LL1116: _temp1115=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp993)->f2; goto _LL1030;} else{ goto _LL1031;} _LL1031: if(( unsigned int)
_temp993 > 1u?*(( int*) _temp993) == Cyc_Absyn_TryCatch_s: 0){ _LL1122:
_temp1121=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp993)->f1; goto _LL1120;
_LL1120: _temp1119=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp993)->f2; goto
_LL1032;} else{ goto _LL1033;} _LL1033: if(( unsigned int) _temp993 > 1u?*(( int*)
_temp993) == Cyc_Absyn_Region_s: 0){ _LL1128: _temp1127=(( struct Cyc_Absyn_Region_s_struct*)
_temp993)->f1; goto _LL1126; _LL1126: _temp1125=(( struct Cyc_Absyn_Region_s_struct*)
_temp993)->f2; goto _LL1124; _LL1124: _temp1123=(( struct Cyc_Absyn_Region_s_struct*)
_temp993)->f3; goto _LL1034;} else{ goto _LL1035;} _LL1035: if(( unsigned int)
_temp993 > 1u?*(( int*) _temp993) == Cyc_Absyn_Cut_s: 0){ _LL1130: _temp1129=((
struct Cyc_Absyn_Cut_s_struct*) _temp993)->f1; goto _LL1036;} else{ goto _LL1037;}
_LL1037: if(( unsigned int) _temp993 > 1u?*(( int*) _temp993) == Cyc_Absyn_Splice_s:
0){ _LL1132: _temp1131=(( struct Cyc_Absyn_Splice_s_struct*) _temp993)->f1; goto
_LL1038;} else{ goto _LL994;} _LL996: Cyc_Absyndump_dump_semi(); goto _LL994;
_LL998: Cyc_Absyndump_dumpexp( _temp1039); Cyc_Absyndump_dump_semi(); goto
_LL994; _LL1000: if( Cyc_Absynpp_is_declaration( _temp1043)){ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpstmt( _temp1043); Cyc_Absyndump_dump_char(( int)'}');}
else{ Cyc_Absyndump_dumpstmt( _temp1043);} if( Cyc_Absynpp_is_declaration(
_temp1041)){ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt(
_temp1041); Cyc_Absyndump_dump_char(( int)'}');} else{ Cyc_Absyndump_dumpstmt(
_temp1041);} goto _LL994; _LL1002: Cyc_Absyndump_dump(({ unsigned char*
_temp1133=( unsigned char*)"return;"; struct _tagged_string _temp1134; _temp1134.curr=
_temp1133; _temp1134.base= _temp1133; _temp1134.last_plus_one= _temp1133 + 8;
_temp1134;})); goto _LL994; _LL1004: Cyc_Absyndump_dump(({ unsigned char*
_temp1135=( unsigned char*)"return"; struct _tagged_string _temp1136; _temp1136.curr=
_temp1135; _temp1136.base= _temp1135; _temp1136.last_plus_one= _temp1135 + 7;
_temp1136;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null(
_temp1047)); Cyc_Absyndump_dump_semi(); goto _LL994; _LL1006: Cyc_Absyndump_dump(({
unsigned char* _temp1137=( unsigned char*)"if("; struct _tagged_string _temp1138;
_temp1138.curr= _temp1137; _temp1138.base= _temp1137; _temp1138.last_plus_one=
_temp1137 + 4; _temp1138;})); Cyc_Absyndump_dumpexp( _temp1053); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp1139=( unsigned char*)"){"; struct _tagged_string _temp1140;
_temp1140.curr= _temp1139; _temp1140.base= _temp1139; _temp1140.last_plus_one=
_temp1139 + 3; _temp1140;})); Cyc_Absyndump_dumpstmt( _temp1051); Cyc_Absyndump_dump_char((
int)'}');{ void* _temp1141=( void*) _temp1049->r; _LL1143: if( _temp1141 ==(
void*) Cyc_Absyn_Skip_s){ goto _LL1144;} else{ goto _LL1145;} _LL1145: goto
_LL1146; _LL1144: goto _LL1142; _LL1146: Cyc_Absyndump_dump(({ unsigned char*
_temp1147=( unsigned char*)"else{"; struct _tagged_string _temp1148; _temp1148.curr=
_temp1147; _temp1148.base= _temp1147; _temp1148.last_plus_one= _temp1147 + 6;
_temp1148;})); Cyc_Absyndump_dumpstmt( _temp1049); Cyc_Absyndump_dump_char(( int)'}');
goto _LL1142; _LL1142:;} goto _LL994; _LL1008: Cyc_Absyndump_dump(({
unsigned char* _temp1149=( unsigned char*)"while("; struct _tagged_string
_temp1150; _temp1150.curr= _temp1149; _temp1150.base= _temp1149; _temp1150.last_plus_one=
_temp1149 + 7; _temp1150;})); Cyc_Absyndump_dumpexp( _temp1061); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp1151=( unsigned char*)") {"; struct _tagged_string _temp1152;
_temp1152.curr= _temp1151; _temp1152.base= _temp1151; _temp1152.last_plus_one=
_temp1151 + 4; _temp1152;})); Cyc_Absyndump_dumpstmt( _temp1055); Cyc_Absyndump_dump_char((
int)'}'); goto _LL994; _LL1010: Cyc_Absyndump_dump(({ unsigned char* _temp1153=(
unsigned char*)"break;"; struct _tagged_string _temp1154; _temp1154.curr=
_temp1153; _temp1154.base= _temp1153; _temp1154.last_plus_one= _temp1153 + 7;
_temp1154;})); goto _LL994; _LL1012: Cyc_Absyndump_dump(({ unsigned char*
_temp1155=( unsigned char*)"continue;"; struct _tagged_string _temp1156;
_temp1156.curr= _temp1155; _temp1156.base= _temp1155; _temp1156.last_plus_one=
_temp1155 + 10; _temp1156;})); goto _LL994; _LL1014: Cyc_Absyndump_dump(({
unsigned char* _temp1157=( unsigned char*)"goto"; struct _tagged_string
_temp1158; _temp1158.curr= _temp1157; _temp1158.base= _temp1157; _temp1158.last_plus_one=
_temp1157 + 5; _temp1158;})); Cyc_Absyndump_dump_str( _temp1069); Cyc_Absyndump_dump_semi();
goto _LL994; _LL1016: Cyc_Absyndump_dump(({ unsigned char* _temp1159=(
unsigned char*)"for("; struct _tagged_string _temp1160; _temp1160.curr=
_temp1159; _temp1160.base= _temp1159; _temp1160.last_plus_one= _temp1159 + 5;
_temp1160;})); Cyc_Absyndump_dumpexp( _temp1085); Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpexp(
_temp1083); Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpexp( _temp1077); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp1161=( unsigned char*)"){"; struct _tagged_string _temp1162;
_temp1162.curr= _temp1161; _temp1162.base= _temp1161; _temp1162.last_plus_one=
_temp1161 + 3; _temp1162;})); Cyc_Absyndump_dumpstmt( _temp1071); Cyc_Absyndump_dump_char((
int)'}'); goto _LL994; _LL1018: Cyc_Absyndump_dump(({ unsigned char* _temp1163=(
unsigned char*)"switch("; struct _tagged_string _temp1164; _temp1164.curr=
_temp1163; _temp1164.base= _temp1163; _temp1164.last_plus_one= _temp1163 + 8;
_temp1164;})); Cyc_Absyndump_dumpexp( _temp1089); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp1165=( unsigned char*)"){"; struct _tagged_string _temp1166;
_temp1166.curr= _temp1165; _temp1166.base= _temp1165; _temp1166.last_plus_one=
_temp1165 + 3; _temp1166;})); Cyc_Absyndump_dumpswitchclauses( _temp1087); Cyc_Absyndump_dump_char((
int)'}'); goto _LL994; _LL1020: Cyc_Absyndump_dumpdecl( _temp1093); Cyc_Absyndump_dumpstmt(
_temp1091); goto _LL994; _LL1022: if( Cyc_Absynpp_is_declaration( _temp1095)){
Cyc_Absyndump_dump_str( _temp1097); Cyc_Absyndump_dump_nospace(({ unsigned char*
_temp1167=( unsigned char*)": {"; struct _tagged_string _temp1168; _temp1168.curr=
_temp1167; _temp1168.base= _temp1167; _temp1168.last_plus_one= _temp1167 + 4;
_temp1168;})); Cyc_Absyndump_dumpstmt( _temp1095); Cyc_Absyndump_dump_char(( int)'}');}
else{ Cyc_Absyndump_dump_str( _temp1097); Cyc_Absyndump_dump_char(( int)':');
Cyc_Absyndump_dumpstmt( _temp1095);} goto _LL994; _LL1024: Cyc_Absyndump_dump(({
unsigned char* _temp1169=( unsigned char*)"do {"; struct _tagged_string
_temp1170; _temp1170.curr= _temp1169; _temp1170.base= _temp1169; _temp1170.last_plus_one=
_temp1169 + 5; _temp1170;})); Cyc_Absyndump_dumpstmt( _temp1105); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp1171=( unsigned char*)"} while ("; struct _tagged_string
_temp1172; _temp1172.curr= _temp1171; _temp1172.base= _temp1171; _temp1172.last_plus_one=
_temp1171 + 10; _temp1172;})); Cyc_Absyndump_dumpexp( _temp1103); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp1173=( unsigned char*)");"; struct _tagged_string _temp1174;
_temp1174.curr= _temp1173; _temp1174.base= _temp1173; _temp1174.last_plus_one=
_temp1173 + 3; _temp1174;})); goto _LL994; _LL1026: Cyc_Absyndump_dump(({
unsigned char* _temp1175=( unsigned char*)"switch \"C\" ("; struct
_tagged_string _temp1176; _temp1176.curr= _temp1175; _temp1176.base= _temp1175;
_temp1176.last_plus_one= _temp1175 + 13; _temp1176;})); Cyc_Absyndump_dumpexp(
_temp1109); Cyc_Absyndump_dump_nospace(({ unsigned char* _temp1177=(
unsigned char*)"){"; struct _tagged_string _temp1178; _temp1178.curr= _temp1177;
_temp1178.base= _temp1177; _temp1178.last_plus_one= _temp1177 + 3; _temp1178;}));
for( 0; _temp1107 != 0; _temp1107=(( struct Cyc_List_List*) _check_null(
_temp1107))->tl){ struct Cyc_Absyn_SwitchC_clause _temp1181; struct Cyc_Position_Segment*
_temp1182; struct Cyc_Absyn_Stmt* _temp1184; struct Cyc_Absyn_Exp* _temp1186;
struct Cyc_Absyn_SwitchC_clause* _temp1179=( struct Cyc_Absyn_SwitchC_clause*)((
struct Cyc_List_List*) _check_null( _temp1107))->hd; _temp1181=* _temp1179;
_LL1187: _temp1186= _temp1181.cnst_exp; goto _LL1185; _LL1185: _temp1184=
_temp1181.body; goto _LL1183; _LL1183: _temp1182= _temp1181.loc; goto _LL1180;
_LL1180: if( _temp1186 == 0){ Cyc_Absyndump_dump(({ unsigned char* _temp1188=(
unsigned char*)"default: "; struct _tagged_string _temp1189; _temp1189.curr=
_temp1188; _temp1189.base= _temp1188; _temp1189.last_plus_one= _temp1188 + 10;
_temp1189;}));} else{ Cyc_Absyndump_dump(({ unsigned char* _temp1190=(
unsigned char*)"case "; struct _tagged_string _temp1191; _temp1191.curr=
_temp1190; _temp1191.base= _temp1190; _temp1191.last_plus_one= _temp1190 + 6;
_temp1191;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null(
_temp1186)); Cyc_Absyndump_dump_char(( int)':');} Cyc_Absyndump_dumpstmt(
_temp1184);} Cyc_Absyndump_dump_char(( int)'}'); goto _LL994; _LL1028: Cyc_Absyndump_dump(({
unsigned char* _temp1192=( unsigned char*)"fallthru;"; struct _tagged_string
_temp1193; _temp1193.curr= _temp1192; _temp1193.base= _temp1192; _temp1193.last_plus_one=
_temp1192 + 10; _temp1193;})); goto _LL994; _LL1030: Cyc_Absyndump_dump(({
unsigned char* _temp1194=( unsigned char*)"fallthru("; struct _tagged_string
_temp1195; _temp1195.curr= _temp1194; _temp1195.base= _temp1194; _temp1195.last_plus_one=
_temp1194 + 10; _temp1195;})); Cyc_Absyndump_dumpexps_prec( 20, _temp1117); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp1196=( unsigned char*)");"; struct _tagged_string _temp1197;
_temp1197.curr= _temp1196; _temp1197.base= _temp1196; _temp1197.last_plus_one=
_temp1196 + 3; _temp1197;})); goto _LL994; _LL1032: Cyc_Absyndump_dump(({
unsigned char* _temp1198=( unsigned char*)"try"; struct _tagged_string _temp1199;
_temp1199.curr= _temp1198; _temp1199.base= _temp1198; _temp1199.last_plus_one=
_temp1198 + 4; _temp1199;})); Cyc_Absyndump_dumpstmt( _temp1121); Cyc_Absyndump_dump(({
unsigned char* _temp1200=( unsigned char*)"catch {"; struct _tagged_string
_temp1201; _temp1201.curr= _temp1200; _temp1201.base= _temp1200; _temp1201.last_plus_one=
_temp1200 + 8; _temp1201;})); Cyc_Absyndump_dumpswitchclauses( _temp1119); Cyc_Absyndump_dump_char((
int)'}'); goto _LL994; _LL1034: Cyc_Absyndump_dump(({ unsigned char* _temp1202=(
unsigned char*)"region<"; struct _tagged_string _temp1203; _temp1203.curr=
_temp1202; _temp1203.base= _temp1202; _temp1203.last_plus_one= _temp1202 + 8;
_temp1203;})); Cyc_Absyndump_dumptvar( _temp1127); Cyc_Absyndump_dump(({
unsigned char* _temp1204=( unsigned char*)"> "; struct _tagged_string _temp1205;
_temp1205.curr= _temp1204; _temp1205.base= _temp1204; _temp1205.last_plus_one=
_temp1204 + 3; _temp1205;})); Cyc_Absyndump_dumpqvar( _temp1125->name); Cyc_Absyndump_dump(({
unsigned char* _temp1206=( unsigned char*)"{"; struct _tagged_string _temp1207;
_temp1207.curr= _temp1206; _temp1207.base= _temp1206; _temp1207.last_plus_one=
_temp1206 + 2; _temp1207;})); Cyc_Absyndump_dumpstmt( _temp1123); Cyc_Absyndump_dump(({
unsigned char* _temp1208=( unsigned char*)"}"; struct _tagged_string _temp1209;
_temp1209.curr= _temp1208; _temp1209.base= _temp1208; _temp1209.last_plus_one=
_temp1208 + 2; _temp1209;})); goto _LL994; _LL1036: Cyc_Absyndump_dump(({
unsigned char* _temp1210=( unsigned char*)"cut"; struct _tagged_string _temp1211;
_temp1211.curr= _temp1210; _temp1211.base= _temp1210; _temp1211.last_plus_one=
_temp1210 + 4; _temp1211;})); Cyc_Absyndump_dumpstmt( _temp1129); goto _LL994;
_LL1038: Cyc_Absyndump_dump(({ unsigned char* _temp1212=( unsigned char*)"splice";
struct _tagged_string _temp1213; _temp1213.curr= _temp1212; _temp1213.base=
_temp1212; _temp1213.last_plus_one= _temp1212 + 7; _temp1213;})); Cyc_Absyndump_dumpstmt(
_temp1131); goto _LL994; _LL994:;} struct _tuple9{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Pat* f2; } ; void Cyc_Absyndump_dumpdp( struct _tuple9* dp){((
void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpdesignator,(* dp).f1,({ unsigned char* _temp1214=(
unsigned char*)""; struct _tagged_string _temp1215; _temp1215.curr= _temp1214;
_temp1215.base= _temp1214; _temp1215.last_plus_one= _temp1214 + 1; _temp1215;}),({
unsigned char* _temp1216=( unsigned char*)"="; struct _tagged_string _temp1217;
_temp1217.curr= _temp1216; _temp1217.base= _temp1216; _temp1217.last_plus_one=
_temp1216 + 2; _temp1217;}),({ unsigned char* _temp1218=( unsigned char*)"=";
struct _tagged_string _temp1219; _temp1219.curr= _temp1218; _temp1219.base=
_temp1218; _temp1219.last_plus_one= _temp1218 + 2; _temp1219;})); Cyc_Absyndump_dumppat((*
dp).f2);} void Cyc_Absyndump_dumppat( struct Cyc_Absyn_Pat* p){ void* _temp1220=(
void*) p->r; int _temp1254; void* _temp1256; int _temp1258; void* _temp1260;
unsigned char _temp1262; struct _tagged_string _temp1264; struct Cyc_Absyn_Vardecl*
_temp1266; struct Cyc_List_List* _temp1268; struct Cyc_Absyn_Pat* _temp1270;
struct Cyc_Absyn_Vardecl* _temp1272; struct _tuple0* _temp1274; struct Cyc_List_List*
_temp1276; struct Cyc_List_List* _temp1278; struct _tuple0* _temp1280; struct
Cyc_List_List* _temp1282; struct Cyc_List_List* _temp1284; struct _tuple0*
_temp1286; struct Cyc_List_List* _temp1288; struct Cyc_List_List* _temp1290;
struct Cyc_Core_Opt* _temp1292; struct Cyc_Absyn_Structdecl* _temp1294; struct
Cyc_List_List* _temp1296; struct Cyc_List_List* _temp1298; struct Cyc_Absyn_Tunionfield*
_temp1300; struct Cyc_Absyn_Tuniondecl* _temp1302; struct Cyc_Absyn_Enumfield*
_temp1304; struct Cyc_Absyn_Enumdecl* _temp1306; _LL1222: if( _temp1220 ==( void*)
Cyc_Absyn_Wild_p){ goto _LL1223;} else{ goto _LL1224;} _LL1224: if( _temp1220 ==(
void*) Cyc_Absyn_Null_p){ goto _LL1225;} else{ goto _LL1226;} _LL1226: if((
unsigned int) _temp1220 > 2u?*(( int*) _temp1220) == Cyc_Absyn_Int_p: 0){
_LL1257: _temp1256=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1220)->f1;
if( _temp1256 ==( void*) Cyc_Absyn_Signed){ goto _LL1255;} else{ goto _LL1228;}
_LL1255: _temp1254=(( struct Cyc_Absyn_Int_p_struct*) _temp1220)->f2; goto
_LL1227;} else{ goto _LL1228;} _LL1228: if(( unsigned int) _temp1220 > 2u?*((
int*) _temp1220) == Cyc_Absyn_Int_p: 0){ _LL1261: _temp1260=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1220)->f1; if( _temp1260 ==( void*) Cyc_Absyn_Unsigned){
goto _LL1259;} else{ goto _LL1230;} _LL1259: _temp1258=(( struct Cyc_Absyn_Int_p_struct*)
_temp1220)->f2; goto _LL1229;} else{ goto _LL1230;} _LL1230: if(( unsigned int)
_temp1220 > 2u?*(( int*) _temp1220) == Cyc_Absyn_Char_p: 0){ _LL1263: _temp1262=((
struct Cyc_Absyn_Char_p_struct*) _temp1220)->f1; goto _LL1231;} else{ goto
_LL1232;} _LL1232: if(( unsigned int) _temp1220 > 2u?*(( int*) _temp1220) == Cyc_Absyn_Float_p:
0){ _LL1265: _temp1264=(( struct Cyc_Absyn_Float_p_struct*) _temp1220)->f1; goto
_LL1233;} else{ goto _LL1234;} _LL1234: if(( unsigned int) _temp1220 > 2u?*((
int*) _temp1220) == Cyc_Absyn_Var_p: 0){ _LL1267: _temp1266=(( struct Cyc_Absyn_Var_p_struct*)
_temp1220)->f1; goto _LL1235;} else{ goto _LL1236;} _LL1236: if(( unsigned int)
_temp1220 > 2u?*(( int*) _temp1220) == Cyc_Absyn_Tuple_p: 0){ _LL1269: _temp1268=((
struct Cyc_Absyn_Tuple_p_struct*) _temp1220)->f1; goto _LL1237;} else{ goto
_LL1238;} _LL1238: if(( unsigned int) _temp1220 > 2u?*(( int*) _temp1220) == Cyc_Absyn_Pointer_p:
0){ _LL1271: _temp1270=(( struct Cyc_Absyn_Pointer_p_struct*) _temp1220)->f1;
goto _LL1239;} else{ goto _LL1240;} _LL1240: if(( unsigned int) _temp1220 > 2u?*((
int*) _temp1220) == Cyc_Absyn_Reference_p: 0){ _LL1273: _temp1272=(( struct Cyc_Absyn_Reference_p_struct*)
_temp1220)->f1; goto _LL1241;} else{ goto _LL1242;} _LL1242: if(( unsigned int)
_temp1220 > 2u?*(( int*) _temp1220) == Cyc_Absyn_UnknownId_p: 0){ _LL1275:
_temp1274=(( struct Cyc_Absyn_UnknownId_p_struct*) _temp1220)->f1; goto _LL1243;}
else{ goto _LL1244;} _LL1244: if(( unsigned int) _temp1220 > 2u?*(( int*)
_temp1220) == Cyc_Absyn_UnknownCall_p: 0){ _LL1281: _temp1280=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1220)->f1; goto _LL1279; _LL1279: _temp1278=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1220)->f2; goto _LL1277; _LL1277: _temp1276=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1220)->f3; goto _LL1245;} else{ goto _LL1246;} _LL1246: if(( unsigned int)
_temp1220 > 2u?*(( int*) _temp1220) == Cyc_Absyn_UnknownFields_p: 0){ _LL1287:
_temp1286=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp1220)->f1; goto
_LL1285; _LL1285: _temp1284=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1220)->f2; goto _LL1283; _LL1283: _temp1282=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1220)->f3; goto _LL1247;} else{ goto _LL1248;} _LL1248: if(( unsigned int)
_temp1220 > 2u?*(( int*) _temp1220) == Cyc_Absyn_Struct_p: 0){ _LL1295:
_temp1294=(( struct Cyc_Absyn_Struct_p_struct*) _temp1220)->f1; goto _LL1293;
_LL1293: _temp1292=(( struct Cyc_Absyn_Struct_p_struct*) _temp1220)->f2; goto
_LL1291; _LL1291: _temp1290=(( struct Cyc_Absyn_Struct_p_struct*) _temp1220)->f3;
goto _LL1289; _LL1289: _temp1288=(( struct Cyc_Absyn_Struct_p_struct*) _temp1220)->f4;
goto _LL1249;} else{ goto _LL1250;} _LL1250: if(( unsigned int) _temp1220 > 2u?*((
int*) _temp1220) == Cyc_Absyn_Tunion_p: 0){ _LL1303: _temp1302=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1220)->f1; goto _LL1301; _LL1301: _temp1300=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1220)->f2; goto _LL1299; _LL1299: _temp1298=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1220)->f3; goto _LL1297; _LL1297: _temp1296=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1220)->f4; goto _LL1251;} else{ goto _LL1252;} _LL1252: if(( unsigned int)
_temp1220 > 2u?*(( int*) _temp1220) == Cyc_Absyn_Enum_p: 0){ _LL1307: _temp1306=((
struct Cyc_Absyn_Enum_p_struct*) _temp1220)->f1; goto _LL1305; _LL1305:
_temp1304=(( struct Cyc_Absyn_Enum_p_struct*) _temp1220)->f2; goto _LL1253;}
else{ goto _LL1221;} _LL1223: Cyc_Absyndump_dump_char(( int)'_'); goto _LL1221;
_LL1225: Cyc_Absyndump_dump(({ unsigned char* _temp1308=( unsigned char*)"null";
struct _tagged_string _temp1309; _temp1309.curr= _temp1308; _temp1309.base=
_temp1308; _temp1309.last_plus_one= _temp1308 + 5; _temp1309;})); goto _LL1221;
_LL1227: Cyc_Absyndump_dump(( struct _tagged_string) xprintf("%d", _temp1254));
goto _LL1221; _LL1229: Cyc_Absyndump_dump(( struct _tagged_string) xprintf("%u",(
unsigned int) _temp1258)); goto _LL1221; _LL1231: Cyc_Absyndump_dump(({
unsigned char* _temp1310=( unsigned char*)"'"; struct _tagged_string _temp1311;
_temp1311.curr= _temp1310; _temp1311.base= _temp1310; _temp1311.last_plus_one=
_temp1310 + 2; _temp1311;})); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_char_escape(
_temp1262)); Cyc_Absyndump_dump_nospace(({ unsigned char* _temp1312=(
unsigned char*)"'"; struct _tagged_string _temp1313; _temp1313.curr= _temp1312;
_temp1313.base= _temp1312; _temp1313.last_plus_one= _temp1312 + 2; _temp1313;}));
goto _LL1221; _LL1233: Cyc_Absyndump_dump( _temp1264); goto _LL1221; _LL1235:
Cyc_Absyndump_dumpqvar( _temp1266->name); goto _LL1221; _LL1237:(( void(*)( void(*
f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat,
_temp1268,({ unsigned char* _temp1314=( unsigned char*)"$("; struct
_tagged_string _temp1315; _temp1315.curr= _temp1314; _temp1315.base= _temp1314;
_temp1315.last_plus_one= _temp1314 + 3; _temp1315;}),({ unsigned char* _temp1316=(
unsigned char*)")"; struct _tagged_string _temp1317; _temp1317.curr= _temp1316;
_temp1317.base= _temp1316; _temp1317.last_plus_one= _temp1316 + 2; _temp1317;}),({
unsigned char* _temp1318=( unsigned char*)","; struct _tagged_string _temp1319;
_temp1319.curr= _temp1318; _temp1319.base= _temp1318; _temp1319.last_plus_one=
_temp1318 + 2; _temp1319;})); goto _LL1221; _LL1239: Cyc_Absyndump_dump(({
unsigned char* _temp1320=( unsigned char*)"&"; struct _tagged_string _temp1321;
_temp1321.curr= _temp1320; _temp1321.base= _temp1320; _temp1321.last_plus_one=
_temp1320 + 2; _temp1321;})); Cyc_Absyndump_dumppat( _temp1270); goto _LL1221;
_LL1241: Cyc_Absyndump_dump(({ unsigned char* _temp1322=( unsigned char*)"*";
struct _tagged_string _temp1323; _temp1323.curr= _temp1322; _temp1323.base=
_temp1322; _temp1323.last_plus_one= _temp1322 + 2; _temp1323;})); Cyc_Absyndump_dumpqvar(
_temp1272->name); goto _LL1221; _LL1243: Cyc_Absyndump_dumpqvar( _temp1274);
goto _LL1221; _LL1245: Cyc_Absyndump_dumpqvar( _temp1280); Cyc_Absyndump_dumptvars(
_temp1278);(( void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp1276,({ unsigned char*
_temp1324=( unsigned char*)"("; struct _tagged_string _temp1325; _temp1325.curr=
_temp1324; _temp1325.base= _temp1324; _temp1325.last_plus_one= _temp1324 + 2;
_temp1325;}),({ unsigned char* _temp1326=( unsigned char*)")"; struct
_tagged_string _temp1327; _temp1327.curr= _temp1326; _temp1327.base= _temp1326;
_temp1327.last_plus_one= _temp1326 + 2; _temp1327;}),({ unsigned char* _temp1328=(
unsigned char*)","; struct _tagged_string _temp1329; _temp1329.curr= _temp1328;
_temp1329.base= _temp1328; _temp1329.last_plus_one= _temp1328 + 2; _temp1329;}));
goto _LL1221; _LL1247: Cyc_Absyndump_dumpqvar( _temp1286); Cyc_Absyndump_dumptvars(
_temp1284);(( void(*)( void(* f)( struct _tuple9*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpdp, _temp1282,({ unsigned char*
_temp1330=( unsigned char*)"{"; struct _tagged_string _temp1331; _temp1331.curr=
_temp1330; _temp1331.base= _temp1330; _temp1331.last_plus_one= _temp1330 + 2;
_temp1331;}),({ unsigned char* _temp1332=( unsigned char*)"}"; struct
_tagged_string _temp1333; _temp1333.curr= _temp1332; _temp1333.base= _temp1332;
_temp1333.last_plus_one= _temp1332 + 2; _temp1333;}),({ unsigned char* _temp1334=(
unsigned char*)","; struct _tagged_string _temp1335; _temp1335.curr= _temp1334;
_temp1335.base= _temp1334; _temp1335.last_plus_one= _temp1334 + 2; _temp1335;}));
goto _LL1221; _LL1249: if( _temp1294->name != 0){ Cyc_Absyndump_dumpqvar((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp1294->name))->v);}
Cyc_Absyndump_dumptvars( _temp1290);(( void(*)( void(* f)( struct _tuple9*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpdp,
_temp1288,({ unsigned char* _temp1336=( unsigned char*)"{"; struct
_tagged_string _temp1337; _temp1337.curr= _temp1336; _temp1337.base= _temp1336;
_temp1337.last_plus_one= _temp1336 + 2; _temp1337;}),({ unsigned char* _temp1338=(
unsigned char*)"}"; struct _tagged_string _temp1339; _temp1339.curr= _temp1338;
_temp1339.base= _temp1338; _temp1339.last_plus_one= _temp1338 + 2; _temp1339;}),({
unsigned char* _temp1340=( unsigned char*)","; struct _tagged_string _temp1341;
_temp1341.curr= _temp1340; _temp1341.base= _temp1340; _temp1341.last_plus_one=
_temp1340 + 2; _temp1341;})); goto _LL1221; _LL1251: Cyc_Absyndump_dumpqvar(
_temp1300->name); Cyc_Absyndump_dumptvars( _temp1298); if( _temp1296 != 0){((
void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat, _temp1296,({ unsigned char* _temp1342=( unsigned char*)"(";
struct _tagged_string _temp1343; _temp1343.curr= _temp1342; _temp1343.base=
_temp1342; _temp1343.last_plus_one= _temp1342 + 2; _temp1343;}),({ unsigned char*
_temp1344=( unsigned char*)")"; struct _tagged_string _temp1345; _temp1345.curr=
_temp1344; _temp1345.base= _temp1344; _temp1345.last_plus_one= _temp1344 + 2;
_temp1345;}),({ unsigned char* _temp1346=( unsigned char*)","; struct
_tagged_string _temp1347; _temp1347.curr= _temp1346; _temp1347.base= _temp1346;
_temp1347.last_plus_one= _temp1346 + 2; _temp1347;}));} goto _LL1221; _LL1253:
Cyc_Absyndump_dumpqvar( _temp1304->name); goto _LL1221; _LL1221:;} void Cyc_Absyndump_dumptunionfield(
struct Cyc_Absyn_Tunionfield* ef){ Cyc_Absyndump_dumpqvar( ef->name); if( ef->typs
!= 0){ Cyc_Absyndump_dumpargs( ef->typs);}} void Cyc_Absyndump_dumptunionfields(
struct Cyc_List_List* fields){(( void(*)( void(* f)( struct Cyc_Absyn_Tunionfield*),
struct Cyc_List_List* l, struct _tagged_string sep)) Cyc_Absyndump_dump_sep)(
Cyc_Absyndump_dumptunionfield, fields,({ unsigned char* _temp1348=(
unsigned char*)","; struct _tagged_string _temp1349; _temp1349.curr= _temp1348;
_temp1349.base= _temp1348; _temp1349.last_plus_one= _temp1348 + 2; _temp1349;}));}
void Cyc_Absyndump_dumpenumfield( struct Cyc_Absyn_Enumfield* ef){ Cyc_Absyndump_dumpqvar(
ef->name); if( ef->tag != 0){ Cyc_Absyndump_dump(({ unsigned char* _temp1350=(
unsigned char*)" = "; struct _tagged_string _temp1351; _temp1351.curr= _temp1350;
_temp1351.base= _temp1350; _temp1351.last_plus_one= _temp1350 + 4; _temp1351;}));
Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null( ef->tag));}} void
Cyc_Absyndump_dumpenumfields( struct Cyc_List_List* fields){(( void(*)( void(* f)(
struct Cyc_Absyn_Enumfield*), struct Cyc_List_List* l, struct _tagged_string sep))
Cyc_Absyndump_dump_sep)( Cyc_Absyndump_dumpenumfield, fields,({ unsigned char*
_temp1352=( unsigned char*)","; struct _tagged_string _temp1353; _temp1353.curr=
_temp1352; _temp1353.base= _temp1352; _temp1353.last_plus_one= _temp1352 + 2;
_temp1353;}));} void Cyc_Absyndump_dumpstructfields( struct Cyc_List_List*
fields){ for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null(
fields))->tl){ struct Cyc_Absyn_Structfield _temp1356; struct Cyc_List_List*
_temp1357; struct Cyc_Core_Opt* _temp1359; void* _temp1361; struct Cyc_Absyn_Tqual
_temp1363; struct _tagged_string* _temp1365; struct Cyc_Absyn_Structfield*
_temp1354=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd; _temp1356=* _temp1354; _LL1366: _temp1365= _temp1356.name; goto
_LL1364; _LL1364: _temp1363= _temp1356.tq; goto _LL1362; _LL1362: _temp1361=(
void*) _temp1356.type; goto _LL1360; _LL1360: _temp1359= _temp1356.width; goto
_LL1358; _LL1358: _temp1357= _temp1356.attributes; goto _LL1355; _LL1355:(( void(*)(
struct Cyc_Absyn_Tqual, void*, void(* f)( struct _tagged_string*), struct
_tagged_string*)) Cyc_Absyndump_dumptqtd)( _temp1363, _temp1361, Cyc_Absyndump_dump_str,
_temp1365); Cyc_Absyndump_dumpatts( _temp1357); if( _temp1359 != 0){ Cyc_Absyndump_dump_char((
int)':'); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*)
_check_null( _temp1359))->v);} Cyc_Absyndump_dump_semi();}} void Cyc_Absyndump_dumptypedefname(
struct Cyc_Absyn_Typedefdecl* td){ Cyc_Absyndump_dumpqvar( td->name); Cyc_Absyndump_dumptvars(
td->tvs);} static void Cyc_Absyndump_dump_atts_qvar( struct Cyc_Absyn_Fndecl* fd){
Cyc_Absyndump_dumpatts( fd->attributes); Cyc_Absyndump_dumpqvar( fd->name);}
struct _tuple10{ void* f1; struct _tuple0* f2; } ; static void Cyc_Absyndump_dump_callconv_qvar(
struct _tuple10* pr){{ void* _temp1367=(* pr).f1; _LL1369: if( _temp1367 ==(
void*) Cyc_Absyn_Unused_att){ goto _LL1370;} else{ goto _LL1371;} _LL1371: if(
_temp1367 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL1372;} else{ goto _LL1373;}
_LL1373: if( _temp1367 ==( void*) Cyc_Absyn_Cdecl_att){ goto _LL1374;} else{
goto _LL1375;} _LL1375: if( _temp1367 ==( void*) Cyc_Absyn_Fastcall_att){ goto
_LL1376;} else{ goto _LL1377;} _LL1377: goto _LL1378; _LL1370: goto _LL1368;
_LL1372: Cyc_Absyndump_dump(({ unsigned char* _temp1379=( unsigned char*)"_stdcall";
struct _tagged_string _temp1380; _temp1380.curr= _temp1379; _temp1380.base=
_temp1379; _temp1380.last_plus_one= _temp1379 + 9; _temp1380;})); goto _LL1368;
_LL1374: Cyc_Absyndump_dump(({ unsigned char* _temp1381=( unsigned char*)"_cdecl";
struct _tagged_string _temp1382; _temp1382.curr= _temp1381; _temp1382.base=
_temp1381; _temp1382.last_plus_one= _temp1381 + 7; _temp1382;})); goto _LL1368;
_LL1376: Cyc_Absyndump_dump(({ unsigned char* _temp1383=( unsigned char*)"_fastcall";
struct _tagged_string _temp1384; _temp1384.curr= _temp1383; _temp1384.base=
_temp1383; _temp1384.last_plus_one= _temp1383 + 10; _temp1384;})); goto _LL1368;
_LL1378: goto _LL1368; _LL1368:;} Cyc_Absyndump_dumpqvar((* pr).f2);} static
void Cyc_Absyndump_dump_callconv_fdqvar( struct Cyc_Absyn_Fndecl* fd){ Cyc_Absyndump_dump_callconv(
fd->attributes); Cyc_Absyndump_dumpqvar( fd->name);} static void Cyc_Absyndump_dumpids(
struct Cyc_List_List* vds){ for( 0; vds != 0; vds=(( struct Cyc_List_List*)
_check_null( vds))->tl){ Cyc_Absyndump_dumpqvar((( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( vds))->hd)->name); if((( struct Cyc_List_List*)
_check_null( vds))->tl != 0){ Cyc_Absyndump_dump_char(( int)',');}}} void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl* d){ void* _temp1385=( void*) d->r; struct Cyc_Absyn_Fndecl*
_temp1411; struct Cyc_Absyn_Structdecl* _temp1413; struct Cyc_Absyn_Uniondecl*
_temp1415; struct Cyc_Absyn_Vardecl* _temp1417; struct Cyc_Absyn_Vardecl
_temp1419; struct Cyc_List_List* _temp1420; struct Cyc_Core_Opt* _temp1422;
struct Cyc_Absyn_Exp* _temp1424; void* _temp1426; struct Cyc_Absyn_Tqual
_temp1428; struct _tuple0* _temp1430; void* _temp1432; struct Cyc_Absyn_Tuniondecl*
_temp1434; struct Cyc_Absyn_Tuniondecl _temp1436; int _temp1437; struct Cyc_Core_Opt*
_temp1439; struct Cyc_List_List* _temp1441; struct _tuple0* _temp1443; void*
_temp1445; struct Cyc_Absyn_Enumdecl* _temp1447; struct Cyc_Absyn_Enumdecl
_temp1449; struct Cyc_Core_Opt* _temp1450; struct _tuple0* _temp1452; void*
_temp1454; int _temp1456; struct Cyc_Absyn_Exp* _temp1458; struct Cyc_Core_Opt*
_temp1460; struct Cyc_Core_Opt* _temp1462; struct Cyc_Absyn_Pat* _temp1464;
struct Cyc_List_List* _temp1466; struct Cyc_Absyn_Typedefdecl* _temp1468; struct
Cyc_List_List* _temp1470; struct _tagged_string* _temp1472; struct Cyc_List_List*
_temp1474; struct _tuple0* _temp1476; struct Cyc_List_List* _temp1478; _LL1387:
if(*(( int*) _temp1385) == Cyc_Absyn_Fn_d){ _LL1412: _temp1411=(( struct Cyc_Absyn_Fn_d_struct*)
_temp1385)->f1; goto _LL1388;} else{ goto _LL1389;} _LL1389: if(*(( int*)
_temp1385) == Cyc_Absyn_Struct_d){ _LL1414: _temp1413=(( struct Cyc_Absyn_Struct_d_struct*)
_temp1385)->f1; goto _LL1390;} else{ goto _LL1391;} _LL1391: if(*(( int*)
_temp1385) == Cyc_Absyn_Union_d){ _LL1416: _temp1415=(( struct Cyc_Absyn_Union_d_struct*)
_temp1385)->f1; goto _LL1392;} else{ goto _LL1393;} _LL1393: if(*(( int*)
_temp1385) == Cyc_Absyn_Var_d){ _LL1418: _temp1417=(( struct Cyc_Absyn_Var_d_struct*)
_temp1385)->f1; _temp1419=* _temp1417; _LL1433: _temp1432=( void*) _temp1419.sc;
goto _LL1431; _LL1431: _temp1430= _temp1419.name; goto _LL1429; _LL1429:
_temp1428= _temp1419.tq; goto _LL1427; _LL1427: _temp1426=( void*) _temp1419.type;
goto _LL1425; _LL1425: _temp1424= _temp1419.initializer; goto _LL1423; _LL1423:
_temp1422= _temp1419.rgn; goto _LL1421; _LL1421: _temp1420= _temp1419.attributes;
goto _LL1394;} else{ goto _LL1395;} _LL1395: if(*(( int*) _temp1385) == Cyc_Absyn_Tunion_d){
_LL1435: _temp1434=(( struct Cyc_Absyn_Tunion_d_struct*) _temp1385)->f1;
_temp1436=* _temp1434; _LL1446: _temp1445=( void*) _temp1436.sc; goto _LL1444;
_LL1444: _temp1443= _temp1436.name; goto _LL1442; _LL1442: _temp1441= _temp1436.tvs;
goto _LL1440; _LL1440: _temp1439= _temp1436.fields; goto _LL1438; _LL1438:
_temp1437= _temp1436.is_xtunion; goto _LL1396;} else{ goto _LL1397;} _LL1397:
if(*(( int*) _temp1385) == Cyc_Absyn_Enum_d){ _LL1448: _temp1447=(( struct Cyc_Absyn_Enum_d_struct*)
_temp1385)->f1; _temp1449=* _temp1447; _LL1455: _temp1454=( void*) _temp1449.sc;
goto _LL1453; _LL1453: _temp1452= _temp1449.name; goto _LL1451; _LL1451:
_temp1450= _temp1449.fields; goto _LL1398;} else{ goto _LL1399;} _LL1399: if(*((
int*) _temp1385) == Cyc_Absyn_Let_d){ _LL1465: _temp1464=(( struct Cyc_Absyn_Let_d_struct*)
_temp1385)->f1; goto _LL1463; _LL1463: _temp1462=(( struct Cyc_Absyn_Let_d_struct*)
_temp1385)->f2; goto _LL1461; _LL1461: _temp1460=(( struct Cyc_Absyn_Let_d_struct*)
_temp1385)->f3; goto _LL1459; _LL1459: _temp1458=(( struct Cyc_Absyn_Let_d_struct*)
_temp1385)->f4; goto _LL1457; _LL1457: _temp1456=(( struct Cyc_Absyn_Let_d_struct*)
_temp1385)->f5; goto _LL1400;} else{ goto _LL1401;} _LL1401: if(*(( int*)
_temp1385) == Cyc_Absyn_Letv_d){ _LL1467: _temp1466=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1385)->f1; goto _LL1402;} else{ goto _LL1403;} _LL1403: if(*(( int*)
_temp1385) == Cyc_Absyn_Typedef_d){ _LL1469: _temp1468=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp1385)->f1; goto _LL1404;} else{ goto _LL1405;} _LL1405: if(*(( int*)
_temp1385) == Cyc_Absyn_Namespace_d){ _LL1473: _temp1472=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1385)->f1; goto _LL1471; _LL1471: _temp1470=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1385)->f2; goto _LL1406;} else{ goto _LL1407;} _LL1407: if(*(( int*)
_temp1385) == Cyc_Absyn_Using_d){ _LL1477: _temp1476=(( struct Cyc_Absyn_Using_d_struct*)
_temp1385)->f1; goto _LL1475; _LL1475: _temp1474=(( struct Cyc_Absyn_Using_d_struct*)
_temp1385)->f2; goto _LL1408;} else{ goto _LL1409;} _LL1409: if(*(( int*)
_temp1385) == Cyc_Absyn_ExternC_d){ _LL1479: _temp1478=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp1385)->f1; goto _LL1410;} else{ goto _LL1386;} _LL1388: if( Cyc_Absynpp_to_VC){
Cyc_Absyndump_dumpatts( _temp1411->attributes);} if( _temp1411->is_inline){ if(
Cyc_Absynpp_to_VC){ Cyc_Absyndump_dump(({ unsigned char* _temp1480=(
unsigned char*)"__inline"; struct _tagged_string _temp1481; _temp1481.curr=
_temp1480; _temp1481.base= _temp1480; _temp1481.last_plus_one= _temp1480 + 9;
_temp1481;}));} else{ Cyc_Absyndump_dump(({ unsigned char* _temp1482=(
unsigned char*)"inline"; struct _tagged_string _temp1483; _temp1483.curr=
_temp1482; _temp1483.base= _temp1482; _temp1483.last_plus_one= _temp1482 + 7;
_temp1483;}));}} Cyc_Absyndump_dumpscope(( void*) _temp1411->sc);{ void* t=(
void*)({ struct Cyc_Absyn_FnType_struct* _temp1485=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1485[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1486; _temp1486.tag= Cyc_Absyn_FnType; _temp1486.f1=({ struct Cyc_Absyn_FnInfo
_temp1487; _temp1487.tvars= _temp1411->tvs; _temp1487.effect= _temp1411->effect;
_temp1487.ret_typ=( void*)(( void*) _temp1411->ret_type); _temp1487.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp1411->args); _temp1487.c_varargs=
_temp1411->c_varargs; _temp1487.cyc_varargs= _temp1411->cyc_varargs; _temp1487.rgn_po=
_temp1411->rgn_po; _temp1487.attributes= 0; _temp1487;}); _temp1486;});
_temp1485;});(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct Cyc_Absyn_Fndecl*),
struct Cyc_Absyn_Fndecl*)) Cyc_Absyndump_dumptqtd)(({ struct Cyc_Absyn_Tqual
_temp1484; _temp1484.q_const= 0; _temp1484.q_volatile= 0; _temp1484.q_restrict=
0; _temp1484;}), t, Cyc_Absynpp_to_VC? Cyc_Absyndump_dump_callconv_fdqvar: Cyc_Absyndump_dump_atts_qvar,
_temp1411); Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt(
_temp1411->body); Cyc_Absyndump_dump_char(( int)'}'); goto _LL1386;} _LL1390:
Cyc_Absyndump_dumpscope(( void*) _temp1413->sc); Cyc_Absyndump_dump(({
unsigned char* _temp1488=( unsigned char*)"struct"; struct _tagged_string
_temp1489; _temp1489.curr= _temp1488; _temp1489.base= _temp1488; _temp1489.last_plus_one=
_temp1488 + 7; _temp1489;})); if( _temp1413->name != 0){ Cyc_Absyndump_dumpqvar((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp1413->name))->v);}
Cyc_Absyndump_dumptvars( _temp1413->tvs); if( _temp1413->fields == 0){ Cyc_Absyndump_dump_semi();}
else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstructfields((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1413->fields))->v);
Cyc_Absyndump_dump(({ unsigned char* _temp1490=( unsigned char*)"}"; struct
_tagged_string _temp1491; _temp1491.curr= _temp1490; _temp1491.base= _temp1490;
_temp1491.last_plus_one= _temp1490 + 2; _temp1491;})); Cyc_Absyndump_dumpatts(
_temp1413->attributes); Cyc_Absyndump_dump(({ unsigned char* _temp1492=(
unsigned char*)";"; struct _tagged_string _temp1493; _temp1493.curr= _temp1492;
_temp1493.base= _temp1492; _temp1493.last_plus_one= _temp1492 + 2; _temp1493;}));}
goto _LL1386; _LL1392: Cyc_Absyndump_dumpscope(( void*) _temp1415->sc); Cyc_Absyndump_dump(({
unsigned char* _temp1494=( unsigned char*)"union"; struct _tagged_string
_temp1495; _temp1495.curr= _temp1494; _temp1495.base= _temp1494; _temp1495.last_plus_one=
_temp1494 + 6; _temp1495;})); if( _temp1415->name != 0){ Cyc_Absyndump_dumpqvar((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp1415->name))->v);}
Cyc_Absyndump_dumptvars( _temp1415->tvs); if( _temp1415->fields == 0){ Cyc_Absyndump_dump_semi();}
else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstructfields((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1415->fields))->v);
Cyc_Absyndump_dump(({ unsigned char* _temp1496=( unsigned char*)"}"; struct
_tagged_string _temp1497; _temp1497.curr= _temp1496; _temp1497.base= _temp1496;
_temp1497.last_plus_one= _temp1496 + 2; _temp1497;})); Cyc_Absyndump_dumpatts(
_temp1415->attributes); Cyc_Absyndump_dump(({ unsigned char* _temp1498=(
unsigned char*)";"; struct _tagged_string _temp1499; _temp1499.curr= _temp1498;
_temp1499.base= _temp1498; _temp1499.last_plus_one= _temp1498 + 2; _temp1499;}));}
goto _LL1386; _LL1394: if( Cyc_Absynpp_to_VC){ Cyc_Absyndump_dumpatts( _temp1420);
Cyc_Absyndump_dumpscope( _temp1432);{ struct Cyc_List_List* _temp1502; void*
_temp1504; struct Cyc_Absyn_Tqual _temp1506; struct _tuple4 _temp1500= Cyc_Absynpp_to_tms(
_temp1428, _temp1426); _LL1507: _temp1506= _temp1500.f1; goto _LL1505; _LL1505:
_temp1504= _temp1500.f2; goto _LL1503; _LL1503: _temp1502= _temp1500.f3; goto
_LL1501; _LL1501: Cyc_Absyndump_dumptq( _temp1506);{ void* call_conv=( void*)
Cyc_Absyn_Unused_att;{ struct Cyc_List_List* tms2= _temp1502; for( 0; tms2 != 0;
tms2=(( struct Cyc_List_List*) _check_null( tms2))->tl){ void* _temp1508=( void*)((
struct Cyc_List_List*) _check_null( tms2))->hd; struct Cyc_List_List* _temp1514;
struct Cyc_Position_Segment* _temp1516; _LL1510: if(( unsigned int) _temp1508 >
1u?*(( int*) _temp1508) == Cyc_Absyn_Attributes_mod: 0){ _LL1517: _temp1516=((
struct Cyc_Absyn_Attributes_mod_struct*) _temp1508)->f1; goto _LL1515; _LL1515:
_temp1514=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp1508)->f2; goto
_LL1511;} else{ goto _LL1512;} _LL1512: goto _LL1513; _LL1511: for( 0; _temp1514
!= 0; _temp1514=(( struct Cyc_List_List*) _check_null( _temp1514))->tl){ void*
_temp1518=( void*)(( struct Cyc_List_List*) _check_null( _temp1514))->hd;
_LL1520: if( _temp1518 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL1521;} else{
goto _LL1522;} _LL1522: if( _temp1518 ==( void*) Cyc_Absyn_Cdecl_att){ goto
_LL1523;} else{ goto _LL1524;} _LL1524: if( _temp1518 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL1525;} else{ goto _LL1526;} _LL1526: goto _LL1527; _LL1521: call_conv=(
void*) Cyc_Absyn_Stdcall_att; goto _LL1519; _LL1523: call_conv=( void*) Cyc_Absyn_Cdecl_att;
goto _LL1519; _LL1525: call_conv=( void*) Cyc_Absyn_Fastcall_att; goto _LL1519;
_LL1527: goto _LL1519; _LL1519:;} goto _LL1509; _LL1513: goto _LL1509; _LL1509:;}}
Cyc_Absyndump_dumptq( _temp1506); Cyc_Absyndump_dumpntyp( _temp1504);{ struct
_tuple10 _temp1529=({ struct _tuple10 _temp1528; _temp1528.f1= call_conv;
_temp1528.f2= _temp1430; _temp1528;}); goto _LL1530; _LL1530:(( void(*)( struct
Cyc_List_List* tms, void(* f)( struct _tuple10*), struct _tuple10* a)) Cyc_Absyndump_dumptms)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1502),
Cyc_Absyndump_dump_callconv_qvar,& _temp1529);}}}} else{ Cyc_Absyndump_dumpscope(
_temp1432);(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct _tuple0*),
struct _tuple0*)) Cyc_Absyndump_dumptqtd)( _temp1428, _temp1426, Cyc_Absyndump_dumpqvar,
_temp1430); Cyc_Absyndump_dumpatts( _temp1420);} if( _temp1424 != 0){ Cyc_Absyndump_dump_char((
int)'='); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null( _temp1424));}
Cyc_Absyndump_dump_semi(); goto _LL1386; _LL1396: Cyc_Absyndump_dumpscope(
_temp1445); if( _temp1437){ Cyc_Absyndump_dump(({ unsigned char* _temp1531=(
unsigned char*)"xtunion "; struct _tagged_string _temp1532; _temp1532.curr=
_temp1531; _temp1532.base= _temp1531; _temp1532.last_plus_one= _temp1531 + 9;
_temp1532;}));} else{ Cyc_Absyndump_dump(({ unsigned char* _temp1533=(
unsigned char*)"tunion "; struct _tagged_string _temp1534; _temp1534.curr=
_temp1533; _temp1534.base= _temp1533; _temp1534.last_plus_one= _temp1533 + 8;
_temp1534;}));} Cyc_Absyndump_dumpqvar( _temp1443); Cyc_Absyndump_dumptvars(
_temp1441); if( _temp1439 == 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumptunionfields(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1439))->v); Cyc_Absyndump_dump_nospace(({ unsigned char*
_temp1535=( unsigned char*)"};"; struct _tagged_string _temp1536; _temp1536.curr=
_temp1535; _temp1536.base= _temp1535; _temp1536.last_plus_one= _temp1535 + 3;
_temp1536;}));} goto _LL1386; _LL1398: Cyc_Absyndump_dumpscope( _temp1454); Cyc_Absyndump_dump(({
unsigned char* _temp1537=( unsigned char*)"enum "; struct _tagged_string
_temp1538; _temp1538.curr= _temp1537; _temp1538.base= _temp1537; _temp1538.last_plus_one=
_temp1537 + 6; _temp1538;})); Cyc_Absyndump_dumpqvar( _temp1452); if( _temp1450
== 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpenumfields((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1450))->v); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp1539=( unsigned char*)"};"; struct _tagged_string _temp1540;
_temp1540.curr= _temp1539; _temp1540.base= _temp1539; _temp1540.last_plus_one=
_temp1539 + 3; _temp1540;}));} return; _LL1400: Cyc_Absyndump_dump(({
unsigned char* _temp1541=( unsigned char*)"let"; struct _tagged_string _temp1542;
_temp1542.curr= _temp1541; _temp1542.base= _temp1541; _temp1542.last_plus_one=
_temp1541 + 4; _temp1542;})); Cyc_Absyndump_dumppat( _temp1464); Cyc_Absyndump_dump_char((
int)'='); Cyc_Absyndump_dumpexp( _temp1458); Cyc_Absyndump_dump_semi(); goto
_LL1386; _LL1402: Cyc_Absyndump_dump(({ unsigned char* _temp1543=( unsigned char*)"let ";
struct _tagged_string _temp1544; _temp1544.curr= _temp1543; _temp1544.base=
_temp1543; _temp1544.last_plus_one= _temp1543 + 5; _temp1544;})); Cyc_Absyndump_dumpids(
_temp1466); Cyc_Absyndump_dump_semi(); goto _LL1386; _LL1404: if( ! Cyc_Absynpp_expand_typedefs){
Cyc_Absyndump_dump(({ unsigned char* _temp1545=( unsigned char*)"typedef";
struct _tagged_string _temp1546; _temp1546.curr= _temp1545; _temp1546.base=
_temp1545; _temp1546.last_plus_one= _temp1545 + 8; _temp1546;}));(( void(*)(
struct Cyc_Absyn_Tqual, void*, void(* f)( struct Cyc_Absyn_Typedefdecl*), struct
Cyc_Absyn_Typedefdecl*)) Cyc_Absyndump_dumptqtd)(({ struct Cyc_Absyn_Tqual
_temp1547; _temp1547.q_const= 0; _temp1547.q_volatile= 0; _temp1547.q_restrict=
0; _temp1547;}),( void*) _temp1468->defn, Cyc_Absyndump_dumptypedefname,
_temp1468); Cyc_Absyndump_dump_semi();} goto _LL1386; _LL1406: Cyc_Absyndump_dump(({
unsigned char* _temp1548=( unsigned char*)"namespace"; struct _tagged_string
_temp1549; _temp1549.curr= _temp1548; _temp1549.base= _temp1548; _temp1549.last_plus_one=
_temp1548 + 10; _temp1549;})); Cyc_Absyndump_dump_str( _temp1472); Cyc_Absyndump_dump_char((
int)'{'); for( 0; _temp1470 != 0; _temp1470=(( struct Cyc_List_List*)
_check_null( _temp1470))->tl){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( _temp1470))->hd);} Cyc_Absyndump_dump_char((
int)'}'); goto _LL1386; _LL1408: Cyc_Absyndump_dump(({ unsigned char* _temp1550=(
unsigned char*)"using"; struct _tagged_string _temp1551; _temp1551.curr=
_temp1550; _temp1551.base= _temp1550; _temp1551.last_plus_one= _temp1550 + 6;
_temp1551;})); Cyc_Absyndump_dumpqvar( _temp1476); Cyc_Absyndump_dump_char(( int)'{');
for( 0; _temp1474 != 0; _temp1474=(( struct Cyc_List_List*) _check_null(
_temp1474))->tl){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( _temp1474))->hd);} Cyc_Absyndump_dump_char(( int)'}'); goto _LL1386;
_LL1410: Cyc_Absyndump_dump(({ unsigned char* _temp1552=( unsigned char*)"extern \"C\" {";
struct _tagged_string _temp1553; _temp1553.curr= _temp1552; _temp1553.base=
_temp1552; _temp1553.last_plus_one= _temp1552 + 13; _temp1553;})); for( 0;
_temp1478 != 0; _temp1478=(( struct Cyc_List_List*) _check_null( _temp1478))->tl){
Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( _temp1478))->hd);} Cyc_Absyndump_dump_char(( int)'}'); goto _LL1386;
_LL1386:;} static void Cyc_Absyndump_dump_upperbound( struct Cyc_Absyn_Exp* e){
unsigned int i= Cyc_Evexp_eval_const_uint_exp( e); if( i != 1){ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpexp( e); Cyc_Absyndump_dump_char(( int)'}');}} void
Cyc_Absyndump_dumptms( struct Cyc_List_List* tms, void(* f)( void*), void* a){
if( tms == 0){ f( a); return;}{ void* _temp1554=( void*)(( struct Cyc_List_List*)
_check_null( tms))->hd; struct Cyc_Absyn_Tqual _temp1572; void* _temp1574; void*
_temp1576; struct Cyc_Absyn_Exp* _temp1578; struct Cyc_Absyn_Tqual _temp1580;
void* _temp1582; void* _temp1584; struct Cyc_Absyn_Exp* _temp1586; struct Cyc_Absyn_Tqual
_temp1588; void* _temp1590; void* _temp1592; struct Cyc_Absyn_Tqual _temp1594;
void* _temp1596; struct Cyc_Absyn_Tvar* _temp1598; void* _temp1600; struct Cyc_Absyn_Exp*
_temp1602; struct Cyc_Absyn_Tqual _temp1604; void* _temp1606; struct Cyc_Absyn_Tvar*
_temp1608; void* _temp1610; struct Cyc_Absyn_Exp* _temp1612; struct Cyc_Absyn_Tqual
_temp1614; void* _temp1616; struct Cyc_Absyn_Tvar* _temp1618; void* _temp1620;
struct Cyc_Absyn_Tqual _temp1622; void* _temp1624; void* _temp1626; _LL1556: if((
unsigned int) _temp1554 > 1u?*(( int*) _temp1554) == Cyc_Absyn_Pointer_mod: 0){
_LL1577: _temp1576=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1554)->f1;
if(( unsigned int) _temp1576 > 1u?*(( int*) _temp1576) == Cyc_Absyn_Nullable_ps:
0){ _LL1579: _temp1578=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp1576)->f1;
goto _LL1575;} else{ goto _LL1558;} _LL1575: _temp1574=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1554)->f2; if( _temp1574 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1573;}
else{ goto _LL1558;} _LL1573: _temp1572=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1554)->f3; goto _LL1557;} else{ goto _LL1558;} _LL1558: if(( unsigned int)
_temp1554 > 1u?*(( int*) _temp1554) == Cyc_Absyn_Pointer_mod: 0){ _LL1585:
_temp1584=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1554)->f1; if((
unsigned int) _temp1584 > 1u?*(( int*) _temp1584) == Cyc_Absyn_NonNullable_ps: 0){
_LL1587: _temp1586=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1584)->f1;
goto _LL1583;} else{ goto _LL1560;} _LL1583: _temp1582=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1554)->f2; if( _temp1582 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1581;}
else{ goto _LL1560;} _LL1581: _temp1580=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1554)->f3; goto _LL1559;} else{ goto _LL1560;} _LL1560: if(( unsigned int)
_temp1554 > 1u?*(( int*) _temp1554) == Cyc_Absyn_Pointer_mod: 0){ _LL1593:
_temp1592=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1554)->f1; if(
_temp1592 ==( void*) Cyc_Absyn_TaggedArray_ps){ goto _LL1591;} else{ goto
_LL1562;} _LL1591: _temp1590=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1554)->f2; if( _temp1590 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1589;}
else{ goto _LL1562;} _LL1589: _temp1588=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1554)->f3; goto _LL1561;} else{ goto _LL1562;} _LL1562: if(( unsigned int)
_temp1554 > 1u?*(( int*) _temp1554) == Cyc_Absyn_Pointer_mod: 0){ _LL1601:
_temp1600=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1554)->f1; if((
unsigned int) _temp1600 > 1u?*(( int*) _temp1600) == Cyc_Absyn_Nullable_ps: 0){
_LL1603: _temp1602=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp1600)->f1; goto
_LL1597;} else{ goto _LL1564;} _LL1597: _temp1596=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1554)->f2; if(( unsigned int) _temp1596 > 4u?*(( int*) _temp1596) == Cyc_Absyn_VarType:
0){ _LL1599: _temp1598=(( struct Cyc_Absyn_VarType_struct*) _temp1596)->f1; goto
_LL1595;} else{ goto _LL1564;} _LL1595: _temp1594=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1554)->f3; goto _LL1563;} else{ goto _LL1564;} _LL1564: if(( unsigned int)
_temp1554 > 1u?*(( int*) _temp1554) == Cyc_Absyn_Pointer_mod: 0){ _LL1611:
_temp1610=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1554)->f1; if((
unsigned int) _temp1610 > 1u?*(( int*) _temp1610) == Cyc_Absyn_NonNullable_ps: 0){
_LL1613: _temp1612=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1610)->f1;
goto _LL1607;} else{ goto _LL1566;} _LL1607: _temp1606=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1554)->f2; if(( unsigned int) _temp1606 > 4u?*(( int*) _temp1606) == Cyc_Absyn_VarType:
0){ _LL1609: _temp1608=(( struct Cyc_Absyn_VarType_struct*) _temp1606)->f1; goto
_LL1605;} else{ goto _LL1566;} _LL1605: _temp1604=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1554)->f3; goto _LL1565;} else{ goto _LL1566;} _LL1566: if(( unsigned int)
_temp1554 > 1u?*(( int*) _temp1554) == Cyc_Absyn_Pointer_mod: 0){ _LL1621:
_temp1620=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1554)->f1; if(
_temp1620 ==( void*) Cyc_Absyn_TaggedArray_ps){ goto _LL1617;} else{ goto
_LL1568;} _LL1617: _temp1616=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1554)->f2; if(( unsigned int) _temp1616 > 4u?*(( int*) _temp1616) == Cyc_Absyn_VarType:
0){ _LL1619: _temp1618=(( struct Cyc_Absyn_VarType_struct*) _temp1616)->f1; goto
_LL1615;} else{ goto _LL1568;} _LL1615: _temp1614=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1554)->f3; goto _LL1567;} else{ goto _LL1568;} _LL1568: if(( unsigned int)
_temp1554 > 1u?*(( int*) _temp1554) == Cyc_Absyn_Pointer_mod: 0){ _LL1627:
_temp1626=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1554)->f1; goto
_LL1625; _LL1625: _temp1624=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1554)->f2; goto _LL1623; _LL1623: _temp1622=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1554)->f3; goto _LL1569;} else{ goto _LL1570;} _LL1570: goto _LL1571;
_LL1557: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dump_upperbound(
_temp1578);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f, a);
return; _LL1559: Cyc_Absyndump_dump_char(( int)'@'); Cyc_Absyndump_dump_upperbound(
_temp1586);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f, a);
return; _LL1561: Cyc_Absyndump_dump_char(( int)'?');(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*)
_check_null( tms))->tl, f, a); return; _LL1563: Cyc_Absyndump_dump_char(( int)'*');
Cyc_Absyndump_dump_upperbound( _temp1602); Cyc_Absyndump_dump_str( _temp1598->name);((
void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)(((
struct Cyc_List_List*) _check_null( tms))->tl, f, a); return; _LL1565: Cyc_Absyndump_dump_char((
int)'@'); Cyc_Absyndump_dump_upperbound( _temp1612); Cyc_Absyndump_dump_str(
_temp1608->name);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void*
a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f,
a); return; _LL1567: Cyc_Absyndump_dump_char(( int)'?'); Cyc_Absyndump_dump_str(
_temp1618->name);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void*
a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f,
a); return; _LL1569:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1628=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1628[ 0]=({ struct Cyc_Core_Impossible_struct _temp1629; _temp1629.tag= Cyc_Core_Impossible;
_temp1629.f1=({ unsigned char* _temp1630=( unsigned char*)"dumptms: bad Pointer_mod";
struct _tagged_string _temp1631; _temp1631.curr= _temp1630; _temp1631.base=
_temp1630; _temp1631.last_plus_one= _temp1630 + 25; _temp1631;}); _temp1629;});
_temp1628;})); _LL1571: { int next_is_pointer= 0; if((( struct Cyc_List_List*)
_check_null( tms))->tl != 0){ void* _temp1632=( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->hd; struct Cyc_Absyn_Tqual
_temp1638; void* _temp1640; void* _temp1642; _LL1634: if(( unsigned int)
_temp1632 > 1u?*(( int*) _temp1632) == Cyc_Absyn_Pointer_mod: 0){ _LL1643:
_temp1642=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1632)->f1; goto
_LL1641; _LL1641: _temp1640=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1632)->f2; goto _LL1639; _LL1639: _temp1638=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1632)->f3; goto _LL1635;} else{ goto _LL1636;} _LL1636: goto _LL1637;
_LL1635: next_is_pointer= 1; goto _LL1633; _LL1637: goto _LL1633; _LL1633:;} if(
next_is_pointer){ Cyc_Absyndump_dump_char(( int)'(');}(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*)
_check_null( tms))->tl, f, a); if( next_is_pointer){ Cyc_Absyndump_dump_char((
int)')');}{ void* _temp1644=( void*)(( struct Cyc_List_List*) _check_null( tms))->hd;
struct Cyc_Absyn_Exp* _temp1660; void* _temp1662; struct Cyc_List_List*
_temp1664; struct Cyc_Core_Opt* _temp1666; struct Cyc_Absyn_VarargInfo*
_temp1668; int _temp1670; struct Cyc_List_List* _temp1672; void* _temp1674;
struct Cyc_Position_Segment* _temp1676; struct Cyc_List_List* _temp1678; int
_temp1680; struct Cyc_Position_Segment* _temp1682; struct Cyc_List_List*
_temp1684; struct Cyc_List_List* _temp1686; struct Cyc_Position_Segment*
_temp1688; struct Cyc_Absyn_Tqual _temp1690; void* _temp1692; void* _temp1694;
_LL1646: if( _temp1644 ==( void*) Cyc_Absyn_Carray_mod){ goto _LL1647;} else{
goto _LL1648;} _LL1648: if(( unsigned int) _temp1644 > 1u?*(( int*) _temp1644)
== Cyc_Absyn_ConstArray_mod: 0){ _LL1661: _temp1660=(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp1644)->f1; goto _LL1649;} else{ goto _LL1650;} _LL1650: if(( unsigned int)
_temp1644 > 1u?*(( int*) _temp1644) == Cyc_Absyn_Function_mod: 0){ _LL1663:
_temp1662=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp1644)->f1; if(*((
int*) _temp1662) == Cyc_Absyn_WithTypes){ _LL1673: _temp1672=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1662)->f1; goto _LL1671; _LL1671: _temp1670=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1662)->f2; goto _LL1669; _LL1669: _temp1668=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1662)->f3; goto _LL1667; _LL1667: _temp1666=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1662)->f4; goto _LL1665; _LL1665: _temp1664=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1662)->f5; goto _LL1651;} else{ goto _LL1652;}} else{ goto _LL1652;}
_LL1652: if(( unsigned int) _temp1644 > 1u?*(( int*) _temp1644) == Cyc_Absyn_Function_mod:
0){ _LL1675: _temp1674=( void*)(( struct Cyc_Absyn_Function_mod_struct*)
_temp1644)->f1; if(*(( int*) _temp1674) == Cyc_Absyn_NoTypes){ _LL1679:
_temp1678=(( struct Cyc_Absyn_NoTypes_struct*) _temp1674)->f1; goto _LL1677;
_LL1677: _temp1676=(( struct Cyc_Absyn_NoTypes_struct*) _temp1674)->f2; goto
_LL1653;} else{ goto _LL1654;}} else{ goto _LL1654;} _LL1654: if(( unsigned int)
_temp1644 > 1u?*(( int*) _temp1644) == Cyc_Absyn_TypeParams_mod: 0){ _LL1685:
_temp1684=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp1644)->f1; goto
_LL1683; _LL1683: _temp1682=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1644)->f2; goto _LL1681; _LL1681: _temp1680=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1644)->f3; goto _LL1655;} else{ goto _LL1656;} _LL1656: if(( unsigned int)
_temp1644 > 1u?*(( int*) _temp1644) == Cyc_Absyn_Attributes_mod: 0){ _LL1689:
_temp1688=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp1644)->f1; goto
_LL1687; _LL1687: _temp1686=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp1644)->f2; goto _LL1657;} else{ goto _LL1658;} _LL1658: if(( unsigned int)
_temp1644 > 1u?*(( int*) _temp1644) == Cyc_Absyn_Pointer_mod: 0){ _LL1695:
_temp1694=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1644)->f1; goto
_LL1693; _LL1693: _temp1692=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1644)->f2; goto _LL1691; _LL1691: _temp1690=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1644)->f3; goto _LL1659;} else{ goto _LL1645;} _LL1647: Cyc_Absyndump_dump(({
unsigned char* _temp1696=( unsigned char*)"[]"; struct _tagged_string _temp1697;
_temp1697.curr= _temp1696; _temp1697.base= _temp1696; _temp1697.last_plus_one=
_temp1696 + 3; _temp1697;})); goto _LL1645; _LL1649: Cyc_Absyndump_dump_char((
int)'['); Cyc_Absyndump_dumpexp( _temp1660); Cyc_Absyndump_dump_char(( int)']');
goto _LL1645; _LL1651: Cyc_Absyndump_dumpfunargs( _temp1672, _temp1670,
_temp1668, _temp1666, _temp1664); goto _LL1645; _LL1653:(( void(*)( void(* f)(
struct _tagged_string*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dump_str,
_temp1678,({ unsigned char* _temp1698=( unsigned char*)"("; struct
_tagged_string _temp1699; _temp1699.curr= _temp1698; _temp1699.base= _temp1698;
_temp1699.last_plus_one= _temp1698 + 2; _temp1699;}),({ unsigned char* _temp1700=(
unsigned char*)")"; struct _tagged_string _temp1701; _temp1701.curr= _temp1700;
_temp1701.base= _temp1700; _temp1701.last_plus_one= _temp1700 + 2; _temp1701;}),({
unsigned char* _temp1702=( unsigned char*)","; struct _tagged_string _temp1703;
_temp1703.curr= _temp1702; _temp1703.base= _temp1702; _temp1703.last_plus_one=
_temp1702 + 2; _temp1703;})); goto _LL1645; _LL1655: if( _temp1680){ Cyc_Absyndump_dumpkindedtvars(
_temp1684);} else{ Cyc_Absyndump_dumptvars( _temp1684);} goto _LL1645; _LL1657:
Cyc_Absyndump_dumpatts( _temp1686); goto _LL1645; _LL1659:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp1704=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1704[ 0]=({ struct
Cyc_Core_Impossible_struct _temp1705; _temp1705.tag= Cyc_Core_Impossible;
_temp1705.f1=({ unsigned char* _temp1706=( unsigned char*)"dumptms"; struct
_tagged_string _temp1707; _temp1707.curr= _temp1706; _temp1707.base= _temp1706;
_temp1707.last_plus_one= _temp1706 + 8; _temp1707;}); _temp1705;}); _temp1704;}));
_LL1645:;} return;} _LL1555:;}} void Cyc_Absyndump_dumptqtd( struct Cyc_Absyn_Tqual
tq, void* t, void(* f)( void*), void* a){ struct Cyc_List_List* _temp1710; void*
_temp1712; struct Cyc_Absyn_Tqual _temp1714; struct _tuple4 _temp1708= Cyc_Absynpp_to_tms(
tq, t); _LL1715: _temp1714= _temp1708.f1; goto _LL1713; _LL1713: _temp1712=
_temp1708.f2; goto _LL1711; _LL1711: _temp1710= _temp1708.f3; goto _LL1709;
_LL1709: Cyc_Absyndump_dumptq( _temp1714); Cyc_Absyndump_dumpntyp( _temp1712);((
void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1710),
f, a);} void Cyc_Absyndump_dumpdecllist2file( struct Cyc_List_List* tdl, struct
Cyc_Stdio___sFILE* f){ Cyc_Absyndump_pos= 0;* Cyc_Absyndump_dump_file= f; for( 0;
tdl != 0; tdl=(( struct Cyc_List_List*) _check_null( tdl))->tl){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( tdl))->hd);}}