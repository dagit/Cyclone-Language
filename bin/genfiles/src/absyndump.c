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
void* _temp588=( void*) e->r; void* _temp680; unsigned char _temp682; void*
_temp684; void* _temp686; short _temp688; void* _temp690; void* _temp692; int
_temp694; void* _temp696; void* _temp698; int _temp700; void* _temp702; void*
_temp704; long long _temp706; void* _temp708; void* _temp710; struct
_tagged_string _temp712; void* _temp714; void* _temp716; struct _tagged_string
_temp718; struct _tuple0* _temp720; void* _temp722; struct _tuple0* _temp724;
struct Cyc_List_List* _temp726; void* _temp728; struct Cyc_Absyn_Exp* _temp730;
struct Cyc_Core_Opt* _temp732; struct Cyc_Absyn_Exp* _temp734; void* _temp736;
struct Cyc_Absyn_Exp* _temp738; void* _temp740; struct Cyc_Absyn_Exp* _temp742;
void* _temp744; struct Cyc_Absyn_Exp* _temp746; void* _temp748; struct Cyc_Absyn_Exp*
_temp750; struct Cyc_Absyn_Exp* _temp752; struct Cyc_Absyn_Exp* _temp754; struct
Cyc_Absyn_Exp* _temp756; struct Cyc_Absyn_Exp* _temp758; struct Cyc_Absyn_Exp*
_temp760; struct Cyc_List_List* _temp762; struct Cyc_Absyn_Exp* _temp764; struct
Cyc_Absyn_VarargCallInfo* _temp766; struct Cyc_List_List* _temp768; struct Cyc_Absyn_Exp*
_temp770; struct Cyc_Absyn_Exp* _temp772; struct Cyc_Absyn_Exp* _temp774; struct
Cyc_List_List* _temp776; struct Cyc_Absyn_Exp* _temp778; struct Cyc_Absyn_Exp*
_temp780; void* _temp782; struct Cyc_Absyn_Exp* _temp784; struct Cyc_Absyn_Exp*
_temp786; struct Cyc_Absyn_Exp* _temp788; void* _temp790; struct Cyc_Absyn_Exp*
_temp792; struct Cyc_Absyn_Exp* _temp794; struct _tagged_string* _temp796;
struct Cyc_Absyn_Exp* _temp798; struct _tagged_string* _temp800; struct Cyc_Absyn_Exp*
_temp802; struct Cyc_Absyn_Exp* _temp804; struct Cyc_Absyn_Exp* _temp806; struct
Cyc_List_List* _temp808; struct Cyc_List_List* _temp810; struct _tuple1*
_temp812; struct Cyc_List_List* _temp814; struct Cyc_Absyn_Exp* _temp816; struct
Cyc_Absyn_Exp* _temp818; struct Cyc_Absyn_Vardecl* _temp820; struct Cyc_Absyn_Structdecl*
_temp822; struct Cyc_List_List* _temp824; struct Cyc_Core_Opt* _temp826; struct
_tuple0* _temp828; struct Cyc_List_List* _temp830; void* _temp832; struct Cyc_Absyn_Tunionfield*
_temp834; struct Cyc_Absyn_Tuniondecl* _temp836; struct Cyc_List_List* _temp838;
struct Cyc_Core_Opt* _temp840; struct Cyc_Core_Opt* _temp842; struct Cyc_Absyn_Enumfield*
_temp844; struct Cyc_Absyn_Enumdecl* _temp846; struct _tuple0* _temp848; void*
_temp850; struct Cyc_Absyn_Exp* _temp852; struct Cyc_List_List* _temp854; struct
Cyc_Core_Opt* _temp856; struct Cyc_Absyn_Stmt* _temp858; struct Cyc_Absyn_Fndecl*
_temp860; struct Cyc_Absyn_Exp* _temp862; _LL590: if(*(( int*) _temp588) == Cyc_Absyn_Const_e){
_LL681: _temp680=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp588)->f1; if((
unsigned int) _temp680 > 1u?*(( int*) _temp680) == Cyc_Absyn_Char_c: 0){ _LL685:
_temp684=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp680)->f1; goto _LL683;
_LL683: _temp682=(( struct Cyc_Absyn_Char_c_struct*) _temp680)->f2; goto _LL591;}
else{ goto _LL592;}} else{ goto _LL592;} _LL592: if(*(( int*) _temp588) == Cyc_Absyn_Const_e){
_LL687: _temp686=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp588)->f1; if((
unsigned int) _temp686 > 1u?*(( int*) _temp686) == Cyc_Absyn_Short_c: 0){ _LL691:
_temp690=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp686)->f1; goto _LL689;
_LL689: _temp688=(( struct Cyc_Absyn_Short_c_struct*) _temp686)->f2; goto _LL593;}
else{ goto _LL594;}} else{ goto _LL594;} _LL594: if(*(( int*) _temp588) == Cyc_Absyn_Const_e){
_LL693: _temp692=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp588)->f1; if((
unsigned int) _temp692 > 1u?*(( int*) _temp692) == Cyc_Absyn_Int_c: 0){ _LL697:
_temp696=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp692)->f1; if( _temp696
==( void*) Cyc_Absyn_Signed){ goto _LL695;} else{ goto _LL596;} _LL695: _temp694=((
struct Cyc_Absyn_Int_c_struct*) _temp692)->f2; goto _LL595;} else{ goto _LL596;}}
else{ goto _LL596;} _LL596: if(*(( int*) _temp588) == Cyc_Absyn_Const_e){ _LL699:
_temp698=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp588)->f1; if((
unsigned int) _temp698 > 1u?*(( int*) _temp698) == Cyc_Absyn_Int_c: 0){ _LL703:
_temp702=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp698)->f1; if( _temp702
==( void*) Cyc_Absyn_Unsigned){ goto _LL701;} else{ goto _LL598;} _LL701:
_temp700=(( struct Cyc_Absyn_Int_c_struct*) _temp698)->f2; goto _LL597;} else{
goto _LL598;}} else{ goto _LL598;} _LL598: if(*(( int*) _temp588) == Cyc_Absyn_Const_e){
_LL705: _temp704=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp588)->f1; if((
unsigned int) _temp704 > 1u?*(( int*) _temp704) == Cyc_Absyn_LongLong_c: 0){
_LL709: _temp708=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp704)->f1;
goto _LL707; _LL707: _temp706=(( struct Cyc_Absyn_LongLong_c_struct*) _temp704)->f2;
goto _LL599;} else{ goto _LL600;}} else{ goto _LL600;} _LL600: if(*(( int*)
_temp588) == Cyc_Absyn_Const_e){ _LL711: _temp710=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp588)->f1; if(( unsigned int) _temp710 > 1u?*(( int*) _temp710) == Cyc_Absyn_Float_c:
0){ _LL713: _temp712=(( struct Cyc_Absyn_Float_c_struct*) _temp710)->f1; goto
_LL601;} else{ goto _LL602;}} else{ goto _LL602;} _LL602: if(*(( int*) _temp588)
== Cyc_Absyn_Const_e){ _LL715: _temp714=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp588)->f1; if( _temp714 ==( void*) Cyc_Absyn_Null_c){ goto _LL603;} else{
goto _LL604;}} else{ goto _LL604;} _LL604: if(*(( int*) _temp588) == Cyc_Absyn_Const_e){
_LL717: _temp716=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp588)->f1; if((
unsigned int) _temp716 > 1u?*(( int*) _temp716) == Cyc_Absyn_String_c: 0){
_LL719: _temp718=(( struct Cyc_Absyn_String_c_struct*) _temp716)->f1; goto
_LL605;} else{ goto _LL606;}} else{ goto _LL606;} _LL606: if(*(( int*) _temp588)
== Cyc_Absyn_UnknownId_e){ _LL721: _temp720=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp588)->f1; goto _LL607;} else{ goto _LL608;} _LL608: if(*(( int*) _temp588)
== Cyc_Absyn_Var_e){ _LL725: _temp724=(( struct Cyc_Absyn_Var_e_struct*)
_temp588)->f1; goto _LL723; _LL723: _temp722=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp588)->f2; goto _LL609;} else{ goto _LL610;} _LL610: if(*(( int*) _temp588)
== Cyc_Absyn_Primop_e){ _LL729: _temp728=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp588)->f1; goto _LL727; _LL727: _temp726=(( struct Cyc_Absyn_Primop_e_struct*)
_temp588)->f2; goto _LL611;} else{ goto _LL612;} _LL612: if(*(( int*) _temp588)
== Cyc_Absyn_AssignOp_e){ _LL735: _temp734=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp588)->f1; goto _LL733; _LL733: _temp732=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp588)->f2; goto _LL731; _LL731: _temp730=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp588)->f3; goto _LL613;} else{ goto _LL614;} _LL614: if(*(( int*) _temp588)
== Cyc_Absyn_Increment_e){ _LL739: _temp738=(( struct Cyc_Absyn_Increment_e_struct*)
_temp588)->f1; goto _LL737; _LL737: _temp736=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp588)->f2; if( _temp736 ==( void*) Cyc_Absyn_PreInc){ goto _LL615;} else{
goto _LL616;}} else{ goto _LL616;} _LL616: if(*(( int*) _temp588) == Cyc_Absyn_Increment_e){
_LL743: _temp742=(( struct Cyc_Absyn_Increment_e_struct*) _temp588)->f1; goto
_LL741; _LL741: _temp740=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp588)->f2; if( _temp740 ==( void*) Cyc_Absyn_PreDec){ goto _LL617;} else{
goto _LL618;}} else{ goto _LL618;} _LL618: if(*(( int*) _temp588) == Cyc_Absyn_Increment_e){
_LL747: _temp746=(( struct Cyc_Absyn_Increment_e_struct*) _temp588)->f1; goto
_LL745; _LL745: _temp744=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp588)->f2; if( _temp744 ==( void*) Cyc_Absyn_PostInc){ goto _LL619;} else{
goto _LL620;}} else{ goto _LL620;} _LL620: if(*(( int*) _temp588) == Cyc_Absyn_Increment_e){
_LL751: _temp750=(( struct Cyc_Absyn_Increment_e_struct*) _temp588)->f1; goto
_LL749; _LL749: _temp748=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp588)->f2; if( _temp748 ==( void*) Cyc_Absyn_PostDec){ goto _LL621;} else{
goto _LL622;}} else{ goto _LL622;} _LL622: if(*(( int*) _temp588) == Cyc_Absyn_Conditional_e){
_LL757: _temp756=(( struct Cyc_Absyn_Conditional_e_struct*) _temp588)->f1; goto
_LL755; _LL755: _temp754=(( struct Cyc_Absyn_Conditional_e_struct*) _temp588)->f2;
goto _LL753; _LL753: _temp752=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp588)->f3; goto _LL623;} else{ goto _LL624;} _LL624: if(*(( int*) _temp588)
== Cyc_Absyn_SeqExp_e){ _LL761: _temp760=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp588)->f1; goto _LL759; _LL759: _temp758=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp588)->f2; goto _LL625;} else{ goto _LL626;} _LL626: if(*(( int*) _temp588)
== Cyc_Absyn_UnknownCall_e){ _LL765: _temp764=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp588)->f1; goto _LL763; _LL763: _temp762=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp588)->f2; goto _LL627;} else{ goto _LL628;} _LL628: if(*(( int*) _temp588)
== Cyc_Absyn_FnCall_e){ _LL771: _temp770=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp588)->f1; goto _LL769; _LL769: _temp768=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp588)->f2; goto _LL767; _LL767: _temp766=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp588)->f3; goto _LL629;} else{ goto _LL630;} _LL630: if(*(( int*) _temp588)
== Cyc_Absyn_Throw_e){ _LL773: _temp772=(( struct Cyc_Absyn_Throw_e_struct*)
_temp588)->f1; goto _LL631;} else{ goto _LL632;} _LL632: if(*(( int*) _temp588)
== Cyc_Absyn_NoInstantiate_e){ _LL775: _temp774=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp588)->f1; goto _LL633;} else{ goto _LL634;} _LL634: if(*(( int*) _temp588)
== Cyc_Absyn_Instantiate_e){ _LL779: _temp778=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp588)->f1; goto _LL777; _LL777: _temp776=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp588)->f2; goto _LL635;} else{ goto _LL636;} _LL636: if(*(( int*) _temp588)
== Cyc_Absyn_Cast_e){ _LL783: _temp782=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp588)->f1; goto _LL781; _LL781: _temp780=(( struct Cyc_Absyn_Cast_e_struct*)
_temp588)->f2; goto _LL637;} else{ goto _LL638;} _LL638: if(*(( int*) _temp588)
== Cyc_Absyn_Address_e){ _LL785: _temp784=(( struct Cyc_Absyn_Address_e_struct*)
_temp588)->f1; goto _LL639;} else{ goto _LL640;} _LL640: if(*(( int*) _temp588)
== Cyc_Absyn_New_e){ _LL789: _temp788=(( struct Cyc_Absyn_New_e_struct*)
_temp588)->f1; goto _LL787; _LL787: _temp786=(( struct Cyc_Absyn_New_e_struct*)
_temp588)->f2; goto _LL641;} else{ goto _LL642;} _LL642: if(*(( int*) _temp588)
== Cyc_Absyn_Sizeoftyp_e){ _LL791: _temp790=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp588)->f1; goto _LL643;} else{ goto _LL644;} _LL644: if(*(( int*) _temp588)
== Cyc_Absyn_Sizeofexp_e){ _LL793: _temp792=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp588)->f1; goto _LL645;} else{ goto _LL646;} _LL646: if(*(( int*) _temp588)
== Cyc_Absyn_Deref_e){ _LL795: _temp794=(( struct Cyc_Absyn_Deref_e_struct*)
_temp588)->f1; goto _LL647;} else{ goto _LL648;} _LL648: if(*(( int*) _temp588)
== Cyc_Absyn_StructMember_e){ _LL799: _temp798=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp588)->f1; goto _LL797; _LL797: _temp796=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp588)->f2; goto _LL649;} else{ goto _LL650;} _LL650: if(*(( int*) _temp588)
== Cyc_Absyn_StructArrow_e){ _LL803: _temp802=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp588)->f1; goto _LL801; _LL801: _temp800=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp588)->f2; goto _LL651;} else{ goto _LL652;} _LL652: if(*(( int*) _temp588)
== Cyc_Absyn_Subscript_e){ _LL807: _temp806=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp588)->f1; goto _LL805; _LL805: _temp804=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp588)->f2; goto _LL653;} else{ goto _LL654;} _LL654: if(*(( int*) _temp588)
== Cyc_Absyn_Tuple_e){ _LL809: _temp808=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp588)->f1; goto _LL655;} else{ goto _LL656;} _LL656: if(*(( int*) _temp588)
== Cyc_Absyn_CompoundLit_e){ _LL813: _temp812=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp588)->f1; goto _LL811; _LL811: _temp810=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp588)->f2; goto _LL657;} else{ goto _LL658;} _LL658: if(*(( int*) _temp588)
== Cyc_Absyn_Array_e){ _LL815: _temp814=(( struct Cyc_Absyn_Array_e_struct*)
_temp588)->f1; goto _LL659;} else{ goto _LL660;} _LL660: if(*(( int*) _temp588)
== Cyc_Absyn_Comprehension_e){ _LL821: _temp820=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp588)->f1; goto _LL819; _LL819: _temp818=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp588)->f2; goto _LL817; _LL817: _temp816=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp588)->f3; goto _LL661;} else{ goto _LL662;} _LL662: if(*(( int*) _temp588)
== Cyc_Absyn_Struct_e){ _LL829: _temp828=(( struct Cyc_Absyn_Struct_e_struct*)
_temp588)->f1; goto _LL827; _LL827: _temp826=(( struct Cyc_Absyn_Struct_e_struct*)
_temp588)->f2; goto _LL825; _LL825: _temp824=(( struct Cyc_Absyn_Struct_e_struct*)
_temp588)->f3; goto _LL823; _LL823: _temp822=(( struct Cyc_Absyn_Struct_e_struct*)
_temp588)->f4; goto _LL663;} else{ goto _LL664;} _LL664: if(*(( int*) _temp588)
== Cyc_Absyn_AnonStruct_e){ _LL833: _temp832=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp588)->f1; goto _LL831; _LL831: _temp830=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp588)->f2; goto _LL665;} else{ goto _LL666;} _LL666: if(*(( int*) _temp588)
== Cyc_Absyn_Tunion_e){ _LL843: _temp842=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp588)->f1; goto _LL841; _LL841: _temp840=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp588)->f2; goto _LL839; _LL839: _temp838=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp588)->f3; goto _LL837; _LL837: _temp836=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp588)->f4; goto _LL835; _LL835: _temp834=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp588)->f5; goto _LL667;} else{ goto _LL668;} _LL668: if(*(( int*) _temp588)
== Cyc_Absyn_Enum_e){ _LL849: _temp848=(( struct Cyc_Absyn_Enum_e_struct*)
_temp588)->f1; goto _LL847; _LL847: _temp846=(( struct Cyc_Absyn_Enum_e_struct*)
_temp588)->f2; goto _LL845; _LL845: _temp844=(( struct Cyc_Absyn_Enum_e_struct*)
_temp588)->f3; goto _LL669;} else{ goto _LL670;} _LL670: if(*(( int*) _temp588)
== Cyc_Absyn_Malloc_e){ _LL853: _temp852=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp588)->f1; goto _LL851; _LL851: _temp850=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp588)->f2; goto _LL671;} else{ goto _LL672;} _LL672: if(*(( int*) _temp588)
== Cyc_Absyn_UnresolvedMem_e){ _LL857: _temp856=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp588)->f1; goto _LL855; _LL855: _temp854=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp588)->f2; goto _LL673;} else{ goto _LL674;} _LL674: if(*(( int*) _temp588)
== Cyc_Absyn_StmtExp_e){ _LL859: _temp858=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp588)->f1; goto _LL675;} else{ goto _LL676;} _LL676: if(*(( int*) _temp588)
== Cyc_Absyn_Codegen_e){ _LL861: _temp860=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp588)->f1; goto _LL677;} else{ goto _LL678;} _LL678: if(*(( int*) _temp588)
== Cyc_Absyn_Fill_e){ _LL863: _temp862=(( struct Cyc_Absyn_Fill_e_struct*)
_temp588)->f1; goto _LL679;} else{ goto _LL589;} _LL591: Cyc_Absyndump_dump_char((
int)'\''); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_char_escape( _temp682)); Cyc_Absyndump_dump_char((
int)'\''); goto _LL589; _LL593: Cyc_Absyndump_dump(( struct _tagged_string) Cyc_Core_string_of_int((
int) _temp688)); goto _LL589; _LL595: Cyc_Absyndump_dump(( struct _tagged_string)
Cyc_Core_string_of_int( _temp694)); goto _LL589; _LL597: Cyc_Absyndump_dump((
struct _tagged_string) Cyc_Core_string_of_int( _temp700)); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp864=( unsigned char*)"u"; struct _tagged_string _temp865;
_temp865.curr= _temp864; _temp865.base= _temp864; _temp865.last_plus_one=
_temp864 + 2; _temp865;})); goto _LL589; _LL599: Cyc_Absyndump_dump(({
unsigned char* _temp866=( unsigned char*)"<<FIX LONG LONG CONSTANT>>"; struct
_tagged_string _temp867; _temp867.curr= _temp866; _temp867.base= _temp866;
_temp867.last_plus_one= _temp866 + 27; _temp867;})); goto _LL589; _LL601: Cyc_Absyndump_dump(
_temp712); goto _LL589; _LL603: Cyc_Absyndump_dump(({ unsigned char* _temp868=(
unsigned char*)"null"; struct _tagged_string _temp869; _temp869.curr= _temp868;
_temp869.base= _temp868; _temp869.last_plus_one= _temp868 + 5; _temp869;}));
goto _LL589; _LL605: Cyc_Absyndump_dump_char(( int)'"'); Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_string_escape( _temp718)); Cyc_Absyndump_dump_char(( int)'"'); goto
_LL589; _LL607: _temp724= _temp720; goto _LL609; _LL609: Cyc_Absyndump_dumpqvar(
_temp724); goto _LL589; _LL611: { struct _tagged_string _temp870= Cyc_Absynpp_prim2str(
_temp728); goto _LL871; _LL871: if( ! Cyc_Absyn_is_format_prim( _temp728)){
switch((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp726)){ case 1:
_LL872: if( _temp728 ==( void*) Cyc_Absyn_Size){ Cyc_Absyndump_dumpexp_prec(
myprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp726))->hd);
Cyc_Absyndump_dump(({ unsigned char* _temp874=( unsigned char*)".size"; struct
_tagged_string _temp875; _temp875.curr= _temp874; _temp875.base= _temp874;
_temp875.last_plus_one= _temp874 + 6; _temp875;}));} else{ Cyc_Absyndump_dump(
_temp870); Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp726))->hd);} break; case 2: _LL873: Cyc_Absyndump_dumpexp_prec(
myprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp726))->hd);
Cyc_Absyndump_dump( _temp870); Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp726))->tl))->hd); break; default: _LL876:( void) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp878=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp878[ 0]=({ struct Cyc_Core_Failure_struct
_temp879; _temp879.tag= Cyc_Core_Failure; _temp879.f1=({ unsigned char* _temp880=(
unsigned char*)"Absyndump -- Bad number of arguments to primop"; struct
_tagged_string _temp881; _temp881.curr= _temp880; _temp881.base= _temp880;
_temp881.last_plus_one= _temp880 + 47; _temp881;}); _temp879;}); _temp878;}));}}
else{ Cyc_Absyndump_dump( _temp870); Cyc_Absyndump_dump_nospace(({ unsigned char*
_temp882=( unsigned char*)"("; struct _tagged_string _temp883; _temp883.curr=
_temp882; _temp883.base= _temp882; _temp883.last_plus_one= _temp882 + 2;
_temp883;})); Cyc_Absyndump_dumpexps_prec( 20, _temp726); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp884=( unsigned char*)")"; struct _tagged_string _temp885;
_temp885.curr= _temp884; _temp885.base= _temp884; _temp885.last_plus_one=
_temp884 + 2; _temp885;}));} goto _LL589;} _LL613: Cyc_Absyndump_dumpexp_prec(
myprec, _temp734); if( _temp732 != 0){ Cyc_Absyndump_dump( Cyc_Absynpp_prim2str((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp732))->v));} Cyc_Absyndump_dump_nospace(({
unsigned char* _temp886=( unsigned char*)"="; struct _tagged_string _temp887;
_temp887.curr= _temp886; _temp887.base= _temp886; _temp887.last_plus_one=
_temp886 + 2; _temp887;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp730); goto
_LL589; _LL615: Cyc_Absyndump_dump(({ unsigned char* _temp888=( unsigned char*)"++";
struct _tagged_string _temp889; _temp889.curr= _temp888; _temp889.base= _temp888;
_temp889.last_plus_one= _temp888 + 3; _temp889;})); Cyc_Absyndump_dumpexp_prec(
myprec, _temp738); goto _LL589; _LL617: Cyc_Absyndump_dump(({ unsigned char*
_temp890=( unsigned char*)"--"; struct _tagged_string _temp891; _temp891.curr=
_temp890; _temp891.base= _temp890; _temp891.last_plus_one= _temp890 + 3;
_temp891;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp742); goto _LL589; _LL619:
Cyc_Absyndump_dumpexp_prec( myprec, _temp746); Cyc_Absyndump_dump(({
unsigned char* _temp892=( unsigned char*)"++"; struct _tagged_string _temp893;
_temp893.curr= _temp892; _temp893.base= _temp892; _temp893.last_plus_one=
_temp892 + 3; _temp893;})); goto _LL589; _LL621: Cyc_Absyndump_dumpexp_prec(
myprec, _temp750); Cyc_Absyndump_dump(({ unsigned char* _temp894=( unsigned char*)"--";
struct _tagged_string _temp895; _temp895.curr= _temp894; _temp895.base= _temp894;
_temp895.last_plus_one= _temp894 + 3; _temp895;})); goto _LL589; _LL623: Cyc_Absyndump_dumpexp_prec(
myprec, _temp756); Cyc_Absyndump_dump_char(( int)'?'); Cyc_Absyndump_dumpexp_prec(
0, _temp754); Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp752); goto _LL589; _LL625: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumpexp_prec(
myprec, _temp760); Cyc_Absyndump_dump_char(( int)','); Cyc_Absyndump_dumpexp_prec(
myprec, _temp758); Cyc_Absyndump_dump_char(( int)')'); goto _LL589; _LL627:
_temp770= _temp764; _temp768= _temp762; goto _LL629; _LL629: Cyc_Absyndump_dumpexp_prec(
myprec, _temp770); Cyc_Absyndump_dump_nospace(({ unsigned char* _temp896=(
unsigned char*)"("; struct _tagged_string _temp897; _temp897.curr= _temp896;
_temp897.base= _temp896; _temp897.last_plus_one= _temp896 + 2; _temp897;})); Cyc_Absyndump_dumpexps_prec(
20, _temp768); Cyc_Absyndump_dump_nospace(({ unsigned char* _temp898=(
unsigned char*)")"; struct _tagged_string _temp899; _temp899.curr= _temp898;
_temp899.base= _temp898; _temp899.last_plus_one= _temp898 + 2; _temp899;}));
goto _LL589; _LL631: Cyc_Absyndump_dump(({ unsigned char* _temp900=(
unsigned char*)"throw"; struct _tagged_string _temp901; _temp901.curr= _temp900;
_temp901.base= _temp900; _temp901.last_plus_one= _temp900 + 6; _temp901;})); Cyc_Absyndump_dumpexp_prec(
myprec, _temp772); goto _LL589; _LL633: _temp778= _temp774; goto _LL635; _LL635:
Cyc_Absyndump_dumpexp_prec( inprec, _temp778); goto _LL589; _LL637: Cyc_Absyndump_dump_char((
int)'('); Cyc_Absyndump_dumptyp( _temp782); Cyc_Absyndump_dump_char(( int)')');
Cyc_Absyndump_dumpexp_prec( myprec, _temp780); goto _LL589; _LL639: Cyc_Absyndump_dump_char((
int)'&'); Cyc_Absyndump_dumpexp_prec( myprec, _temp784); goto _LL589; _LL641:
Cyc_Absyndump_dump(({ unsigned char* _temp902=( unsigned char*)"new "; struct
_tagged_string _temp903; _temp903.curr= _temp902; _temp903.base= _temp902;
_temp903.last_plus_one= _temp902 + 5; _temp903;})); Cyc_Absyndump_dumpexp_prec(
myprec, _temp786); goto _LL589; _LL643: Cyc_Absyndump_dump(({ unsigned char*
_temp904=( unsigned char*)"sizeof("; struct _tagged_string _temp905; _temp905.curr=
_temp904; _temp905.base= _temp904; _temp905.last_plus_one= _temp904 + 8;
_temp905;})); Cyc_Absyndump_dumptyp( _temp790); Cyc_Absyndump_dump_char(( int)')');
goto _LL589; _LL645: Cyc_Absyndump_dump(({ unsigned char* _temp906=(
unsigned char*)"sizeof("; struct _tagged_string _temp907; _temp907.curr=
_temp906; _temp907.base= _temp906; _temp907.last_plus_one= _temp906 + 8;
_temp907;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp792); Cyc_Absyndump_dump_char((
int)')'); goto _LL589; _LL647: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp794); goto _LL589; _LL649: Cyc_Absyndump_dumpexp_prec( myprec,
_temp798); Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp796); goto _LL589; _LL651: Cyc_Absyndump_dumpexp_prec( myprec, _temp802);
Cyc_Absyndump_dump_nospace(({ unsigned char* _temp908=( unsigned char*)"->";
struct _tagged_string _temp909; _temp909.curr= _temp908; _temp909.base= _temp908;
_temp909.last_plus_one= _temp908 + 3; _temp909;})); Cyc_Absyndump_dump_nospace(*
_temp800); goto _LL589; _LL653: Cyc_Absyndump_dumpexp_prec( myprec, _temp806);
Cyc_Absyndump_dump_char(( int)'['); Cyc_Absyndump_dumpexp( _temp804); Cyc_Absyndump_dump_char((
int)']'); goto _LL589; _LL655: Cyc_Absyndump_dump(({ unsigned char* _temp910=(
unsigned char*)"$("; struct _tagged_string _temp911; _temp911.curr= _temp910;
_temp911.base= _temp910; _temp911.last_plus_one= _temp910 + 3; _temp911;})); Cyc_Absyndump_dumpexps_prec(
20, _temp808); Cyc_Absyndump_dump_char(( int)')'); goto _LL589; _LL657: Cyc_Absyndump_dump_char((
int)'('); Cyc_Absyndump_dumptyp((* _temp812).f3); Cyc_Absyndump_dump_char(( int)')');((
void(*)( void(* f)( struct _tuple8*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp810,({ unsigned char* _temp912=( unsigned char*)"{";
struct _tagged_string _temp913; _temp913.curr= _temp912; _temp913.base= _temp912;
_temp913.last_plus_one= _temp912 + 2; _temp913;}),({ unsigned char* _temp914=(
unsigned char*)"}"; struct _tagged_string _temp915; _temp915.curr= _temp914;
_temp915.base= _temp914; _temp915.last_plus_one= _temp914 + 2; _temp915;}),({
unsigned char* _temp916=( unsigned char*)","; struct _tagged_string _temp917;
_temp917.curr= _temp916; _temp917.base= _temp916; _temp917.last_plus_one=
_temp916 + 2; _temp917;})); goto _LL589; _LL659:(( void(*)( void(* f)( struct
_tuple8*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp814,({ unsigned char* _temp918=( unsigned char*)"{"; struct _tagged_string
_temp919; _temp919.curr= _temp918; _temp919.base= _temp918; _temp919.last_plus_one=
_temp918 + 2; _temp919;}),({ unsigned char* _temp920=( unsigned char*)"}";
struct _tagged_string _temp921; _temp921.curr= _temp920; _temp921.base= _temp920;
_temp921.last_plus_one= _temp920 + 2; _temp921;}),({ unsigned char* _temp922=(
unsigned char*)","; struct _tagged_string _temp923; _temp923.curr= _temp922;
_temp923.base= _temp922; _temp923.last_plus_one= _temp922 + 2; _temp923;}));
goto _LL589; _LL661: Cyc_Absyndump_dump(({ unsigned char* _temp924=(
unsigned char*)"new {for"; struct _tagged_string _temp925; _temp925.curr=
_temp924; _temp925.base= _temp924; _temp925.last_plus_one= _temp924 + 9;
_temp925;})); Cyc_Absyndump_dump_str((* _temp820->name).f2); Cyc_Absyndump_dump_char((
int)'<'); Cyc_Absyndump_dumpexp( _temp818); Cyc_Absyndump_dump_char(( int)':');
Cyc_Absyndump_dumpexp( _temp816); Cyc_Absyndump_dump_char(( int)'}'); goto
_LL589; _LL663: Cyc_Absyndump_dumpqvar( _temp828);(( void(*)( void(* f)( struct
_tuple8*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp824,({ unsigned char* _temp926=( unsigned char*)"{"; struct _tagged_string
_temp927; _temp927.curr= _temp926; _temp927.base= _temp926; _temp927.last_plus_one=
_temp926 + 2; _temp927;}),({ unsigned char* _temp928=( unsigned char*)"}";
struct _tagged_string _temp929; _temp929.curr= _temp928; _temp929.base= _temp928;
_temp929.last_plus_one= _temp928 + 2; _temp929;}),({ unsigned char* _temp930=(
unsigned char*)","; struct _tagged_string _temp931; _temp931.curr= _temp930;
_temp931.base= _temp930; _temp931.last_plus_one= _temp930 + 2; _temp931;}));
goto _LL589; _LL665:(( void(*)( void(* f)( struct _tuple8*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde, _temp830,({ unsigned char*
_temp932=( unsigned char*)"{"; struct _tagged_string _temp933; _temp933.curr=
_temp932; _temp933.base= _temp932; _temp933.last_plus_one= _temp932 + 2;
_temp933;}),({ unsigned char* _temp934=( unsigned char*)"}"; struct
_tagged_string _temp935; _temp935.curr= _temp934; _temp935.base= _temp934;
_temp935.last_plus_one= _temp934 + 2; _temp935;}),({ unsigned char* _temp936=(
unsigned char*)","; struct _tagged_string _temp937; _temp937.curr= _temp936;
_temp937.base= _temp936; _temp937.last_plus_one= _temp936 + 2; _temp937;}));
goto _LL589; _LL667: Cyc_Absyndump_dumpqvar( _temp834->name); if( _temp838 != 0){((
void(*)( void(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpexp, _temp838,({ unsigned char* _temp938=( unsigned char*)"(";
struct _tagged_string _temp939; _temp939.curr= _temp938; _temp939.base= _temp938;
_temp939.last_plus_one= _temp938 + 2; _temp939;}),({ unsigned char* _temp940=(
unsigned char*)")"; struct _tagged_string _temp941; _temp941.curr= _temp940;
_temp941.base= _temp940; _temp941.last_plus_one= _temp940 + 2; _temp941;}),({
unsigned char* _temp942=( unsigned char*)","; struct _tagged_string _temp943;
_temp943.curr= _temp942; _temp943.base= _temp942; _temp943.last_plus_one=
_temp942 + 2; _temp943;}));} goto _LL589; _LL669: Cyc_Absyndump_dumpqvar(
_temp848); goto _LL589; _LL671: if( _temp852 != 0){ Cyc_Absyndump_dump(({
unsigned char* _temp944=( unsigned char*)"rmalloc("; struct _tagged_string
_temp945; _temp945.curr= _temp944; _temp945.base= _temp944; _temp945.last_plus_one=
_temp944 + 9; _temp945;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)
_check_null( _temp852)); Cyc_Absyndump_dump(({ unsigned char* _temp946=(
unsigned char*)","; struct _tagged_string _temp947; _temp947.curr= _temp946;
_temp947.base= _temp946; _temp947.last_plus_one= _temp946 + 2; _temp947;}));}
else{ Cyc_Absyndump_dump(({ unsigned char* _temp948=( unsigned char*)"malloc(";
struct _tagged_string _temp949; _temp949.curr= _temp948; _temp949.base= _temp948;
_temp949.last_plus_one= _temp948 + 8; _temp949;}));} Cyc_Absyndump_dump(({
unsigned char* _temp950=( unsigned char*)"sizeof("; struct _tagged_string
_temp951; _temp951.curr= _temp950; _temp951.base= _temp950; _temp951.last_plus_one=
_temp950 + 8; _temp951;})); Cyc_Absyndump_dumptyp( _temp850); Cyc_Absyndump_dump(({
unsigned char* _temp952=( unsigned char*)"))"; struct _tagged_string _temp953;
_temp953.curr= _temp952; _temp953.base= _temp952; _temp953.last_plus_one=
_temp952 + 3; _temp953;})); goto _LL589; _LL673:(( void(*)( void(* f)( struct
_tuple8*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp854,({ unsigned char* _temp954=( unsigned char*)"{"; struct _tagged_string
_temp955; _temp955.curr= _temp954; _temp955.base= _temp954; _temp955.last_plus_one=
_temp954 + 2; _temp955;}),({ unsigned char* _temp956=( unsigned char*)"}";
struct _tagged_string _temp957; _temp957.curr= _temp956; _temp957.base= _temp956;
_temp957.last_plus_one= _temp956 + 2; _temp957;}),({ unsigned char* _temp958=(
unsigned char*)","; struct _tagged_string _temp959; _temp959.curr= _temp958;
_temp959.base= _temp958; _temp959.last_plus_one= _temp958 + 2; _temp959;}));
goto _LL589; _LL675: Cyc_Absyndump_dump_nospace(({ unsigned char* _temp960=(
unsigned char*)"({"; struct _tagged_string _temp961; _temp961.curr= _temp960;
_temp961.base= _temp960; _temp961.last_plus_one= _temp960 + 3; _temp961;})); Cyc_Absyndump_dumpstmt(
_temp858); Cyc_Absyndump_dump_nospace(({ unsigned char* _temp962=( unsigned char*)"})";
struct _tagged_string _temp963; _temp963.curr= _temp962; _temp963.base= _temp962;
_temp963.last_plus_one= _temp962 + 3; _temp963;})); goto _LL589; _LL677: Cyc_Absyndump_dump(({
unsigned char* _temp964=( unsigned char*)"codegen("; struct _tagged_string
_temp965; _temp965.curr= _temp964; _temp965.base= _temp964; _temp965.last_plus_one=
_temp964 + 9; _temp965;})); Cyc_Absyndump_dumpdecl(({ struct Cyc_Absyn_Decl*
_temp966=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp966->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp967=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp967[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp968; _temp968.tag= Cyc_Absyn_Fn_d;
_temp968.f1= _temp860; _temp968;}); _temp967;})); _temp966->loc= e->loc;
_temp966;})); Cyc_Absyndump_dump(({ unsigned char* _temp969=( unsigned char*)")";
struct _tagged_string _temp970; _temp970.curr= _temp969; _temp970.base= _temp969;
_temp970.last_plus_one= _temp969 + 2; _temp970;})); goto _LL589; _LL679: Cyc_Absyndump_dump(({
unsigned char* _temp971=( unsigned char*)"fill("; struct _tagged_string _temp972;
_temp972.curr= _temp971; _temp972.base= _temp971; _temp972.last_plus_one=
_temp971 + 6; _temp972;})); Cyc_Absyndump_dumpexp( _temp862); Cyc_Absyndump_dump(({
unsigned char* _temp973=( unsigned char*)")"; struct _tagged_string _temp974;
_temp974.curr= _temp973; _temp974.base= _temp973; _temp974.last_plus_one=
_temp973 + 2; _temp974;})); goto _LL589; _LL589:;} if( inprec >= myprec){ Cyc_Absyndump_dump_char((
int)')');}} void Cyc_Absyndump_dumpexp( struct Cyc_Absyn_Exp* e){ Cyc_Absyndump_dumpexp_prec(
0, e);} void Cyc_Absyndump_dumpswitchclauses( struct Cyc_List_List* scs){ for( 0;
scs != 0; scs=(( struct Cyc_List_List*) _check_null( scs))->tl){ struct Cyc_Absyn_Switch_clause*
_temp975=( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
scs))->hd; goto _LL976; _LL976: if( _temp975->where_clause == 0?( void*)(
_temp975->pattern)->r ==( void*) Cyc_Absyn_Wild_p: 0){ Cyc_Absyndump_dump(({
unsigned char* _temp977=( unsigned char*)"default:"; struct _tagged_string
_temp978; _temp978.curr= _temp977; _temp978.base= _temp977; _temp978.last_plus_one=
_temp977 + 9; _temp978;}));} else{ Cyc_Absyndump_dump(({ unsigned char* _temp979=(
unsigned char*)"case"; struct _tagged_string _temp980; _temp980.curr= _temp979;
_temp980.base= _temp979; _temp980.last_plus_one= _temp979 + 5; _temp980;})); Cyc_Absyndump_dumppat(
_temp975->pattern); if( _temp975->where_clause != 0){ Cyc_Absyndump_dump(({
unsigned char* _temp981=( unsigned char*)"&&"; struct _tagged_string _temp982;
_temp982.curr= _temp981; _temp982.base= _temp981; _temp982.last_plus_one=
_temp981 + 3; _temp982;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)
_check_null( _temp975->where_clause));} Cyc_Absyndump_dump_nospace(({
unsigned char* _temp983=( unsigned char*)":"; struct _tagged_string _temp984;
_temp984.curr= _temp983; _temp984.base= _temp983; _temp984.last_plus_one=
_temp983 + 2; _temp984;}));} Cyc_Absyndump_dumpstmt( _temp975->body);}} void Cyc_Absyndump_dumpstmt(
struct Cyc_Absyn_Stmt* s){ void* _temp985=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1031; struct Cyc_Absyn_Stmt* _temp1033; struct Cyc_Absyn_Stmt* _temp1035;
struct Cyc_Absyn_Exp* _temp1037; struct Cyc_Absyn_Exp* _temp1039; struct Cyc_Absyn_Stmt*
_temp1041; struct Cyc_Absyn_Stmt* _temp1043; struct Cyc_Absyn_Exp* _temp1045;
struct Cyc_Absyn_Stmt* _temp1047; struct _tuple2 _temp1049; struct Cyc_Absyn_Stmt*
_temp1051; struct Cyc_Absyn_Exp* _temp1053; struct Cyc_Absyn_Stmt* _temp1055;
struct Cyc_Absyn_Stmt* _temp1057; struct Cyc_Absyn_Stmt* _temp1059; struct
_tagged_string* _temp1061; struct Cyc_Absyn_Stmt* _temp1063; struct _tuple2
_temp1065; struct Cyc_Absyn_Stmt* _temp1067; struct Cyc_Absyn_Exp* _temp1069;
struct _tuple2 _temp1071; struct Cyc_Absyn_Stmt* _temp1073; struct Cyc_Absyn_Exp*
_temp1075; struct Cyc_Absyn_Exp* _temp1077; struct Cyc_List_List* _temp1079;
struct Cyc_Absyn_Exp* _temp1081; struct Cyc_Absyn_Stmt* _temp1083; struct Cyc_Absyn_Decl*
_temp1085; struct Cyc_Absyn_Stmt* _temp1087; struct _tagged_string* _temp1089;
struct _tuple2 _temp1091; struct Cyc_Absyn_Stmt* _temp1093; struct Cyc_Absyn_Exp*
_temp1095; struct Cyc_Absyn_Stmt* _temp1097; struct Cyc_List_List* _temp1099;
struct Cyc_Absyn_Exp* _temp1101; struct Cyc_Absyn_Switch_clause** _temp1103;
struct Cyc_List_List* _temp1105; struct Cyc_Absyn_Switch_clause** _temp1107;
struct Cyc_List_List* _temp1109; struct Cyc_List_List* _temp1111; struct Cyc_Absyn_Stmt*
_temp1113; struct Cyc_Absyn_Stmt* _temp1115; struct Cyc_Absyn_Vardecl* _temp1117;
struct Cyc_Absyn_Tvar* _temp1119; struct Cyc_Absyn_Stmt* _temp1121; struct Cyc_Absyn_Stmt*
_temp1123; _LL987: if( _temp985 ==( void*) Cyc_Absyn_Skip_s){ goto _LL988;}
else{ goto _LL989;} _LL989: if(( unsigned int) _temp985 > 1u?*(( int*) _temp985)
== Cyc_Absyn_Exp_s: 0){ _LL1032: _temp1031=(( struct Cyc_Absyn_Exp_s_struct*)
_temp985)->f1; goto _LL990;} else{ goto _LL991;} _LL991: if(( unsigned int)
_temp985 > 1u?*(( int*) _temp985) == Cyc_Absyn_Seq_s: 0){ _LL1036: _temp1035=((
struct Cyc_Absyn_Seq_s_struct*) _temp985)->f1; goto _LL1034; _LL1034: _temp1033=((
struct Cyc_Absyn_Seq_s_struct*) _temp985)->f2; goto _LL992;} else{ goto _LL993;}
_LL993: if(( unsigned int) _temp985 > 1u?*(( int*) _temp985) == Cyc_Absyn_Return_s:
0){ _LL1038: _temp1037=(( struct Cyc_Absyn_Return_s_struct*) _temp985)->f1; if(
_temp1037 == 0){ goto _LL994;} else{ goto _LL995;}} else{ goto _LL995;} _LL995:
if(( unsigned int) _temp985 > 1u?*(( int*) _temp985) == Cyc_Absyn_Return_s: 0){
_LL1040: _temp1039=(( struct Cyc_Absyn_Return_s_struct*) _temp985)->f1; goto
_LL996;} else{ goto _LL997;} _LL997: if(( unsigned int) _temp985 > 1u?*(( int*)
_temp985) == Cyc_Absyn_IfThenElse_s: 0){ _LL1046: _temp1045=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp985)->f1; goto _LL1044; _LL1044: _temp1043=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp985)->f2; goto _LL1042; _LL1042: _temp1041=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp985)->f3; goto _LL998;} else{ goto _LL999;} _LL999: if(( unsigned int)
_temp985 > 1u?*(( int*) _temp985) == Cyc_Absyn_While_s: 0){ _LL1050: _temp1049=((
struct Cyc_Absyn_While_s_struct*) _temp985)->f1; _LL1054: _temp1053= _temp1049.f1;
goto _LL1052; _LL1052: _temp1051= _temp1049.f2; goto _LL1048; _LL1048: _temp1047=((
struct Cyc_Absyn_While_s_struct*) _temp985)->f2; goto _LL1000;} else{ goto
_LL1001;} _LL1001: if(( unsigned int) _temp985 > 1u?*(( int*) _temp985) == Cyc_Absyn_Break_s:
0){ _LL1056: _temp1055=(( struct Cyc_Absyn_Break_s_struct*) _temp985)->f1; goto
_LL1002;} else{ goto _LL1003;} _LL1003: if(( unsigned int) _temp985 > 1u?*(( int*)
_temp985) == Cyc_Absyn_Continue_s: 0){ _LL1058: _temp1057=(( struct Cyc_Absyn_Continue_s_struct*)
_temp985)->f1; goto _LL1004;} else{ goto _LL1005;} _LL1005: if(( unsigned int)
_temp985 > 1u?*(( int*) _temp985) == Cyc_Absyn_Goto_s: 0){ _LL1062: _temp1061=((
struct Cyc_Absyn_Goto_s_struct*) _temp985)->f1; goto _LL1060; _LL1060: _temp1059=((
struct Cyc_Absyn_Goto_s_struct*) _temp985)->f2; goto _LL1006;} else{ goto
_LL1007;} _LL1007: if(( unsigned int) _temp985 > 1u?*(( int*) _temp985) == Cyc_Absyn_For_s:
0){ _LL1078: _temp1077=(( struct Cyc_Absyn_For_s_struct*) _temp985)->f1; goto
_LL1072; _LL1072: _temp1071=(( struct Cyc_Absyn_For_s_struct*) _temp985)->f2;
_LL1076: _temp1075= _temp1071.f1; goto _LL1074; _LL1074: _temp1073= _temp1071.f2;
goto _LL1066; _LL1066: _temp1065=(( struct Cyc_Absyn_For_s_struct*) _temp985)->f3;
_LL1070: _temp1069= _temp1065.f1; goto _LL1068; _LL1068: _temp1067= _temp1065.f2;
goto _LL1064; _LL1064: _temp1063=(( struct Cyc_Absyn_For_s_struct*) _temp985)->f4;
goto _LL1008;} else{ goto _LL1009;} _LL1009: if(( unsigned int) _temp985 > 1u?*((
int*) _temp985) == Cyc_Absyn_Switch_s: 0){ _LL1082: _temp1081=(( struct Cyc_Absyn_Switch_s_struct*)
_temp985)->f1; goto _LL1080; _LL1080: _temp1079=(( struct Cyc_Absyn_Switch_s_struct*)
_temp985)->f2; goto _LL1010;} else{ goto _LL1011;} _LL1011: if(( unsigned int)
_temp985 > 1u?*(( int*) _temp985) == Cyc_Absyn_Decl_s: 0){ _LL1086: _temp1085=((
struct Cyc_Absyn_Decl_s_struct*) _temp985)->f1; goto _LL1084; _LL1084: _temp1083=((
struct Cyc_Absyn_Decl_s_struct*) _temp985)->f2; goto _LL1012;} else{ goto
_LL1013;} _LL1013: if(( unsigned int) _temp985 > 1u?*(( int*) _temp985) == Cyc_Absyn_Label_s:
0){ _LL1090: _temp1089=(( struct Cyc_Absyn_Label_s_struct*) _temp985)->f1; goto
_LL1088; _LL1088: _temp1087=(( struct Cyc_Absyn_Label_s_struct*) _temp985)->f2;
goto _LL1014;} else{ goto _LL1015;} _LL1015: if(( unsigned int) _temp985 > 1u?*((
int*) _temp985) == Cyc_Absyn_Do_s: 0){ _LL1098: _temp1097=(( struct Cyc_Absyn_Do_s_struct*)
_temp985)->f1; goto _LL1092; _LL1092: _temp1091=(( struct Cyc_Absyn_Do_s_struct*)
_temp985)->f2; _LL1096: _temp1095= _temp1091.f1; goto _LL1094; _LL1094:
_temp1093= _temp1091.f2; goto _LL1016;} else{ goto _LL1017;} _LL1017: if((
unsigned int) _temp985 > 1u?*(( int*) _temp985) == Cyc_Absyn_SwitchC_s: 0){
_LL1102: _temp1101=(( struct Cyc_Absyn_SwitchC_s_struct*) _temp985)->f1; goto
_LL1100; _LL1100: _temp1099=(( struct Cyc_Absyn_SwitchC_s_struct*) _temp985)->f2;
goto _LL1018;} else{ goto _LL1019;} _LL1019: if(( unsigned int) _temp985 > 1u?*((
int*) _temp985) == Cyc_Absyn_Fallthru_s: 0){ _LL1106: _temp1105=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp985)->f1; if( _temp1105 == 0){ goto _LL1104;} else{ goto _LL1021;} _LL1104:
_temp1103=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp985)->f2; goto _LL1020;}
else{ goto _LL1021;} _LL1021: if(( unsigned int) _temp985 > 1u?*(( int*)
_temp985) == Cyc_Absyn_Fallthru_s: 0){ _LL1110: _temp1109=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp985)->f1; goto _LL1108; _LL1108: _temp1107=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp985)->f2; goto _LL1022;} else{ goto _LL1023;} _LL1023: if(( unsigned int)
_temp985 > 1u?*(( int*) _temp985) == Cyc_Absyn_TryCatch_s: 0){ _LL1114:
_temp1113=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp985)->f1; goto _LL1112;
_LL1112: _temp1111=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp985)->f2; goto
_LL1024;} else{ goto _LL1025;} _LL1025: if(( unsigned int) _temp985 > 1u?*(( int*)
_temp985) == Cyc_Absyn_Region_s: 0){ _LL1120: _temp1119=(( struct Cyc_Absyn_Region_s_struct*)
_temp985)->f1; goto _LL1118; _LL1118: _temp1117=(( struct Cyc_Absyn_Region_s_struct*)
_temp985)->f2; goto _LL1116; _LL1116: _temp1115=(( struct Cyc_Absyn_Region_s_struct*)
_temp985)->f3; goto _LL1026;} else{ goto _LL1027;} _LL1027: if(( unsigned int)
_temp985 > 1u?*(( int*) _temp985) == Cyc_Absyn_Cut_s: 0){ _LL1122: _temp1121=((
struct Cyc_Absyn_Cut_s_struct*) _temp985)->f1; goto _LL1028;} else{ goto _LL1029;}
_LL1029: if(( unsigned int) _temp985 > 1u?*(( int*) _temp985) == Cyc_Absyn_Splice_s:
0){ _LL1124: _temp1123=(( struct Cyc_Absyn_Splice_s_struct*) _temp985)->f1; goto
_LL1030;} else{ goto _LL986;} _LL988: Cyc_Absyndump_dump_semi(); goto _LL986;
_LL990: Cyc_Absyndump_dumpexp( _temp1031); Cyc_Absyndump_dump_semi(); goto
_LL986; _LL992: if( Cyc_Absynpp_is_declaration( _temp1035)){ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpstmt( _temp1035); Cyc_Absyndump_dump_char(( int)'}');}
else{ Cyc_Absyndump_dumpstmt( _temp1035);} if( Cyc_Absynpp_is_declaration(
_temp1033)){ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt(
_temp1033); Cyc_Absyndump_dump_char(( int)'}');} else{ Cyc_Absyndump_dumpstmt(
_temp1033);} goto _LL986; _LL994: Cyc_Absyndump_dump(({ unsigned char* _temp1125=(
unsigned char*)"return;"; struct _tagged_string _temp1126; _temp1126.curr=
_temp1125; _temp1126.base= _temp1125; _temp1126.last_plus_one= _temp1125 + 8;
_temp1126;})); goto _LL986; _LL996: Cyc_Absyndump_dump(({ unsigned char*
_temp1127=( unsigned char*)"return"; struct _tagged_string _temp1128; _temp1128.curr=
_temp1127; _temp1128.base= _temp1127; _temp1128.last_plus_one= _temp1127 + 7;
_temp1128;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null(
_temp1039)); Cyc_Absyndump_dump_semi(); goto _LL986; _LL998: Cyc_Absyndump_dump(({
unsigned char* _temp1129=( unsigned char*)"if("; struct _tagged_string _temp1130;
_temp1130.curr= _temp1129; _temp1130.base= _temp1129; _temp1130.last_plus_one=
_temp1129 + 4; _temp1130;})); Cyc_Absyndump_dumpexp( _temp1045); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp1131=( unsigned char*)"){"; struct _tagged_string _temp1132;
_temp1132.curr= _temp1131; _temp1132.base= _temp1131; _temp1132.last_plus_one=
_temp1131 + 3; _temp1132;})); Cyc_Absyndump_dumpstmt( _temp1043); Cyc_Absyndump_dump_char((
int)'}');{ void* _temp1133=( void*) _temp1041->r; _LL1135: if( _temp1133 ==(
void*) Cyc_Absyn_Skip_s){ goto _LL1136;} else{ goto _LL1137;} _LL1137: goto
_LL1138; _LL1136: goto _LL1134; _LL1138: Cyc_Absyndump_dump(({ unsigned char*
_temp1139=( unsigned char*)"else{"; struct _tagged_string _temp1140; _temp1140.curr=
_temp1139; _temp1140.base= _temp1139; _temp1140.last_plus_one= _temp1139 + 6;
_temp1140;})); Cyc_Absyndump_dumpstmt( _temp1041); Cyc_Absyndump_dump_char(( int)'}');
goto _LL1134; _LL1134:;} goto _LL986; _LL1000: Cyc_Absyndump_dump(({
unsigned char* _temp1141=( unsigned char*)"while("; struct _tagged_string
_temp1142; _temp1142.curr= _temp1141; _temp1142.base= _temp1141; _temp1142.last_plus_one=
_temp1141 + 7; _temp1142;})); Cyc_Absyndump_dumpexp( _temp1053); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp1143=( unsigned char*)") {"; struct _tagged_string _temp1144;
_temp1144.curr= _temp1143; _temp1144.base= _temp1143; _temp1144.last_plus_one=
_temp1143 + 4; _temp1144;})); Cyc_Absyndump_dumpstmt( _temp1047); Cyc_Absyndump_dump_char((
int)'}'); goto _LL986; _LL1002: Cyc_Absyndump_dump(({ unsigned char* _temp1145=(
unsigned char*)"break;"; struct _tagged_string _temp1146; _temp1146.curr=
_temp1145; _temp1146.base= _temp1145; _temp1146.last_plus_one= _temp1145 + 7;
_temp1146;})); goto _LL986; _LL1004: Cyc_Absyndump_dump(({ unsigned char*
_temp1147=( unsigned char*)"continue;"; struct _tagged_string _temp1148;
_temp1148.curr= _temp1147; _temp1148.base= _temp1147; _temp1148.last_plus_one=
_temp1147 + 10; _temp1148;})); goto _LL986; _LL1006: Cyc_Absyndump_dump(({
unsigned char* _temp1149=( unsigned char*)"goto"; struct _tagged_string
_temp1150; _temp1150.curr= _temp1149; _temp1150.base= _temp1149; _temp1150.last_plus_one=
_temp1149 + 5; _temp1150;})); Cyc_Absyndump_dump_str( _temp1061); Cyc_Absyndump_dump_semi();
goto _LL986; _LL1008: Cyc_Absyndump_dump(({ unsigned char* _temp1151=(
unsigned char*)"for("; struct _tagged_string _temp1152; _temp1152.curr=
_temp1151; _temp1152.base= _temp1151; _temp1152.last_plus_one= _temp1151 + 5;
_temp1152;})); Cyc_Absyndump_dumpexp( _temp1077); Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpexp(
_temp1075); Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpexp( _temp1069); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp1153=( unsigned char*)"){"; struct _tagged_string _temp1154;
_temp1154.curr= _temp1153; _temp1154.base= _temp1153; _temp1154.last_plus_one=
_temp1153 + 3; _temp1154;})); Cyc_Absyndump_dumpstmt( _temp1063); Cyc_Absyndump_dump_char((
int)'}'); goto _LL986; _LL1010: Cyc_Absyndump_dump(({ unsigned char* _temp1155=(
unsigned char*)"switch("; struct _tagged_string _temp1156; _temp1156.curr=
_temp1155; _temp1156.base= _temp1155; _temp1156.last_plus_one= _temp1155 + 8;
_temp1156;})); Cyc_Absyndump_dumpexp( _temp1081); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp1157=( unsigned char*)"){"; struct _tagged_string _temp1158;
_temp1158.curr= _temp1157; _temp1158.base= _temp1157; _temp1158.last_plus_one=
_temp1157 + 3; _temp1158;})); Cyc_Absyndump_dumpswitchclauses( _temp1079); Cyc_Absyndump_dump_char((
int)'}'); goto _LL986; _LL1012: Cyc_Absyndump_dumpdecl( _temp1085); Cyc_Absyndump_dumpstmt(
_temp1083); goto _LL986; _LL1014: if( Cyc_Absynpp_is_declaration( _temp1087)){
Cyc_Absyndump_dump_str( _temp1089); Cyc_Absyndump_dump_nospace(({ unsigned char*
_temp1159=( unsigned char*)": {"; struct _tagged_string _temp1160; _temp1160.curr=
_temp1159; _temp1160.base= _temp1159; _temp1160.last_plus_one= _temp1159 + 4;
_temp1160;})); Cyc_Absyndump_dumpstmt( _temp1087); Cyc_Absyndump_dump_char(( int)'}');}
else{ Cyc_Absyndump_dump_str( _temp1089); Cyc_Absyndump_dump_char(( int)':');
Cyc_Absyndump_dumpstmt( _temp1087);} goto _LL986; _LL1016: Cyc_Absyndump_dump(({
unsigned char* _temp1161=( unsigned char*)"do {"; struct _tagged_string
_temp1162; _temp1162.curr= _temp1161; _temp1162.base= _temp1161; _temp1162.last_plus_one=
_temp1161 + 5; _temp1162;})); Cyc_Absyndump_dumpstmt( _temp1097); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp1163=( unsigned char*)"} while ("; struct _tagged_string
_temp1164; _temp1164.curr= _temp1163; _temp1164.base= _temp1163; _temp1164.last_plus_one=
_temp1163 + 10; _temp1164;})); Cyc_Absyndump_dumpexp( _temp1095); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp1165=( unsigned char*)");"; struct _tagged_string _temp1166;
_temp1166.curr= _temp1165; _temp1166.base= _temp1165; _temp1166.last_plus_one=
_temp1165 + 3; _temp1166;})); goto _LL986; _LL1018: Cyc_Absyndump_dump(({
unsigned char* _temp1167=( unsigned char*)"switch \"C\" ("; struct
_tagged_string _temp1168; _temp1168.curr= _temp1167; _temp1168.base= _temp1167;
_temp1168.last_plus_one= _temp1167 + 13; _temp1168;})); Cyc_Absyndump_dumpexp(
_temp1101); Cyc_Absyndump_dump_nospace(({ unsigned char* _temp1169=(
unsigned char*)"){"; struct _tagged_string _temp1170; _temp1170.curr= _temp1169;
_temp1170.base= _temp1169; _temp1170.last_plus_one= _temp1169 + 3; _temp1170;}));
for( 0; _temp1099 != 0; _temp1099=(( struct Cyc_List_List*) _check_null(
_temp1099))->tl){ struct Cyc_Absyn_SwitchC_clause _temp1173; struct Cyc_Position_Segment*
_temp1174; struct Cyc_Absyn_Stmt* _temp1176; struct Cyc_Absyn_Exp* _temp1178;
struct Cyc_Absyn_SwitchC_clause* _temp1171=( struct Cyc_Absyn_SwitchC_clause*)((
struct Cyc_List_List*) _check_null( _temp1099))->hd; _temp1173=* _temp1171;
_LL1179: _temp1178= _temp1173.cnst_exp; goto _LL1177; _LL1177: _temp1176=
_temp1173.body; goto _LL1175; _LL1175: _temp1174= _temp1173.loc; goto _LL1172;
_LL1172: if( _temp1178 == 0){ Cyc_Absyndump_dump(({ unsigned char* _temp1180=(
unsigned char*)"default: "; struct _tagged_string _temp1181; _temp1181.curr=
_temp1180; _temp1181.base= _temp1180; _temp1181.last_plus_one= _temp1180 + 10;
_temp1181;}));} else{ Cyc_Absyndump_dump(({ unsigned char* _temp1182=(
unsigned char*)"case "; struct _tagged_string _temp1183; _temp1183.curr=
_temp1182; _temp1183.base= _temp1182; _temp1183.last_plus_one= _temp1182 + 6;
_temp1183;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null(
_temp1178)); Cyc_Absyndump_dump_char(( int)':');} Cyc_Absyndump_dumpstmt(
_temp1176);} Cyc_Absyndump_dump_char(( int)'}'); goto _LL986; _LL1020: Cyc_Absyndump_dump(({
unsigned char* _temp1184=( unsigned char*)"fallthru;"; struct _tagged_string
_temp1185; _temp1185.curr= _temp1184; _temp1185.base= _temp1184; _temp1185.last_plus_one=
_temp1184 + 10; _temp1185;})); goto _LL986; _LL1022: Cyc_Absyndump_dump(({
unsigned char* _temp1186=( unsigned char*)"fallthru("; struct _tagged_string
_temp1187; _temp1187.curr= _temp1186; _temp1187.base= _temp1186; _temp1187.last_plus_one=
_temp1186 + 10; _temp1187;})); Cyc_Absyndump_dumpexps_prec( 20, _temp1109); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp1188=( unsigned char*)");"; struct _tagged_string _temp1189;
_temp1189.curr= _temp1188; _temp1189.base= _temp1188; _temp1189.last_plus_one=
_temp1188 + 3; _temp1189;})); goto _LL986; _LL1024: Cyc_Absyndump_dump(({
unsigned char* _temp1190=( unsigned char*)"try"; struct _tagged_string _temp1191;
_temp1191.curr= _temp1190; _temp1191.base= _temp1190; _temp1191.last_plus_one=
_temp1190 + 4; _temp1191;})); Cyc_Absyndump_dumpstmt( _temp1113); Cyc_Absyndump_dump(({
unsigned char* _temp1192=( unsigned char*)"catch {"; struct _tagged_string
_temp1193; _temp1193.curr= _temp1192; _temp1193.base= _temp1192; _temp1193.last_plus_one=
_temp1192 + 8; _temp1193;})); Cyc_Absyndump_dumpswitchclauses( _temp1111); Cyc_Absyndump_dump_char((
int)'}'); goto _LL986; _LL1026: Cyc_Absyndump_dump(({ unsigned char* _temp1194=(
unsigned char*)"region<"; struct _tagged_string _temp1195; _temp1195.curr=
_temp1194; _temp1195.base= _temp1194; _temp1195.last_plus_one= _temp1194 + 8;
_temp1195;})); Cyc_Absyndump_dumptvar( _temp1119); Cyc_Absyndump_dump(({
unsigned char* _temp1196=( unsigned char*)"> "; struct _tagged_string _temp1197;
_temp1197.curr= _temp1196; _temp1197.base= _temp1196; _temp1197.last_plus_one=
_temp1196 + 3; _temp1197;})); Cyc_Absyndump_dumpqvar( _temp1117->name); Cyc_Absyndump_dump(({
unsigned char* _temp1198=( unsigned char*)"{"; struct _tagged_string _temp1199;
_temp1199.curr= _temp1198; _temp1199.base= _temp1198; _temp1199.last_plus_one=
_temp1198 + 2; _temp1199;})); Cyc_Absyndump_dumpstmt( _temp1115); Cyc_Absyndump_dump(({
unsigned char* _temp1200=( unsigned char*)"}"; struct _tagged_string _temp1201;
_temp1201.curr= _temp1200; _temp1201.base= _temp1200; _temp1201.last_plus_one=
_temp1200 + 2; _temp1201;})); goto _LL986; _LL1028: Cyc_Absyndump_dump(({
unsigned char* _temp1202=( unsigned char*)"cut"; struct _tagged_string _temp1203;
_temp1203.curr= _temp1202; _temp1203.base= _temp1202; _temp1203.last_plus_one=
_temp1202 + 4; _temp1203;})); Cyc_Absyndump_dumpstmt( _temp1121); goto _LL986;
_LL1030: Cyc_Absyndump_dump(({ unsigned char* _temp1204=( unsigned char*)"splice";
struct _tagged_string _temp1205; _temp1205.curr= _temp1204; _temp1205.base=
_temp1204; _temp1205.last_plus_one= _temp1204 + 7; _temp1205;})); Cyc_Absyndump_dumpstmt(
_temp1123); goto _LL986; _LL986:;} struct _tuple9{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Pat* f2; } ; void Cyc_Absyndump_dumpdp( struct _tuple9* dp){((
void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpdesignator,(* dp).f1,({ unsigned char* _temp1206=(
unsigned char*)""; struct _tagged_string _temp1207; _temp1207.curr= _temp1206;
_temp1207.base= _temp1206; _temp1207.last_plus_one= _temp1206 + 1; _temp1207;}),({
unsigned char* _temp1208=( unsigned char*)"="; struct _tagged_string _temp1209;
_temp1209.curr= _temp1208; _temp1209.base= _temp1208; _temp1209.last_plus_one=
_temp1208 + 2; _temp1209;}),({ unsigned char* _temp1210=( unsigned char*)"=";
struct _tagged_string _temp1211; _temp1211.curr= _temp1210; _temp1211.base=
_temp1210; _temp1211.last_plus_one= _temp1210 + 2; _temp1211;})); Cyc_Absyndump_dumppat((*
dp).f2);} void Cyc_Absyndump_dumppat( struct Cyc_Absyn_Pat* p){ void* _temp1212=(
void*) p->r; int _temp1246; void* _temp1248; int _temp1250; void* _temp1252;
unsigned char _temp1254; struct _tagged_string _temp1256; struct Cyc_Absyn_Vardecl*
_temp1258; struct Cyc_List_List* _temp1260; struct Cyc_Absyn_Pat* _temp1262;
struct Cyc_Absyn_Vardecl* _temp1264; struct _tuple0* _temp1266; struct Cyc_List_List*
_temp1268; struct Cyc_List_List* _temp1270; struct _tuple0* _temp1272; struct
Cyc_List_List* _temp1274; struct Cyc_List_List* _temp1276; struct _tuple0*
_temp1278; struct Cyc_List_List* _temp1280; struct Cyc_List_List* _temp1282;
struct Cyc_Core_Opt* _temp1284; struct Cyc_Absyn_Structdecl* _temp1286; struct
Cyc_List_List* _temp1288; struct Cyc_List_List* _temp1290; struct Cyc_Absyn_Tunionfield*
_temp1292; struct Cyc_Absyn_Tuniondecl* _temp1294; struct Cyc_Absyn_Enumfield*
_temp1296; struct Cyc_Absyn_Enumdecl* _temp1298; _LL1214: if( _temp1212 ==( void*)
Cyc_Absyn_Wild_p){ goto _LL1215;} else{ goto _LL1216;} _LL1216: if( _temp1212 ==(
void*) Cyc_Absyn_Null_p){ goto _LL1217;} else{ goto _LL1218;} _LL1218: if((
unsigned int) _temp1212 > 2u?*(( int*) _temp1212) == Cyc_Absyn_Int_p: 0){
_LL1249: _temp1248=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1212)->f1;
if( _temp1248 ==( void*) Cyc_Absyn_Signed){ goto _LL1247;} else{ goto _LL1220;}
_LL1247: _temp1246=(( struct Cyc_Absyn_Int_p_struct*) _temp1212)->f2; goto
_LL1219;} else{ goto _LL1220;} _LL1220: if(( unsigned int) _temp1212 > 2u?*((
int*) _temp1212) == Cyc_Absyn_Int_p: 0){ _LL1253: _temp1252=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1212)->f1; if( _temp1252 ==( void*) Cyc_Absyn_Unsigned){
goto _LL1251;} else{ goto _LL1222;} _LL1251: _temp1250=(( struct Cyc_Absyn_Int_p_struct*)
_temp1212)->f2; goto _LL1221;} else{ goto _LL1222;} _LL1222: if(( unsigned int)
_temp1212 > 2u?*(( int*) _temp1212) == Cyc_Absyn_Char_p: 0){ _LL1255: _temp1254=((
struct Cyc_Absyn_Char_p_struct*) _temp1212)->f1; goto _LL1223;} else{ goto
_LL1224;} _LL1224: if(( unsigned int) _temp1212 > 2u?*(( int*) _temp1212) == Cyc_Absyn_Float_p:
0){ _LL1257: _temp1256=(( struct Cyc_Absyn_Float_p_struct*) _temp1212)->f1; goto
_LL1225;} else{ goto _LL1226;} _LL1226: if(( unsigned int) _temp1212 > 2u?*((
int*) _temp1212) == Cyc_Absyn_Var_p: 0){ _LL1259: _temp1258=(( struct Cyc_Absyn_Var_p_struct*)
_temp1212)->f1; goto _LL1227;} else{ goto _LL1228;} _LL1228: if(( unsigned int)
_temp1212 > 2u?*(( int*) _temp1212) == Cyc_Absyn_Tuple_p: 0){ _LL1261: _temp1260=((
struct Cyc_Absyn_Tuple_p_struct*) _temp1212)->f1; goto _LL1229;} else{ goto
_LL1230;} _LL1230: if(( unsigned int) _temp1212 > 2u?*(( int*) _temp1212) == Cyc_Absyn_Pointer_p:
0){ _LL1263: _temp1262=(( struct Cyc_Absyn_Pointer_p_struct*) _temp1212)->f1;
goto _LL1231;} else{ goto _LL1232;} _LL1232: if(( unsigned int) _temp1212 > 2u?*((
int*) _temp1212) == Cyc_Absyn_Reference_p: 0){ _LL1265: _temp1264=(( struct Cyc_Absyn_Reference_p_struct*)
_temp1212)->f1; goto _LL1233;} else{ goto _LL1234;} _LL1234: if(( unsigned int)
_temp1212 > 2u?*(( int*) _temp1212) == Cyc_Absyn_UnknownId_p: 0){ _LL1267:
_temp1266=(( struct Cyc_Absyn_UnknownId_p_struct*) _temp1212)->f1; goto _LL1235;}
else{ goto _LL1236;} _LL1236: if(( unsigned int) _temp1212 > 2u?*(( int*)
_temp1212) == Cyc_Absyn_UnknownCall_p: 0){ _LL1273: _temp1272=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1212)->f1; goto _LL1271; _LL1271: _temp1270=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1212)->f2; goto _LL1269; _LL1269: _temp1268=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1212)->f3; goto _LL1237;} else{ goto _LL1238;} _LL1238: if(( unsigned int)
_temp1212 > 2u?*(( int*) _temp1212) == Cyc_Absyn_UnknownFields_p: 0){ _LL1279:
_temp1278=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp1212)->f1; goto
_LL1277; _LL1277: _temp1276=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1212)->f2; goto _LL1275; _LL1275: _temp1274=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1212)->f3; goto _LL1239;} else{ goto _LL1240;} _LL1240: if(( unsigned int)
_temp1212 > 2u?*(( int*) _temp1212) == Cyc_Absyn_Struct_p: 0){ _LL1287:
_temp1286=(( struct Cyc_Absyn_Struct_p_struct*) _temp1212)->f1; goto _LL1285;
_LL1285: _temp1284=(( struct Cyc_Absyn_Struct_p_struct*) _temp1212)->f2; goto
_LL1283; _LL1283: _temp1282=(( struct Cyc_Absyn_Struct_p_struct*) _temp1212)->f3;
goto _LL1281; _LL1281: _temp1280=(( struct Cyc_Absyn_Struct_p_struct*) _temp1212)->f4;
goto _LL1241;} else{ goto _LL1242;} _LL1242: if(( unsigned int) _temp1212 > 2u?*((
int*) _temp1212) == Cyc_Absyn_Tunion_p: 0){ _LL1295: _temp1294=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1212)->f1; goto _LL1293; _LL1293: _temp1292=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1212)->f2; goto _LL1291; _LL1291: _temp1290=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1212)->f3; goto _LL1289; _LL1289: _temp1288=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1212)->f4; goto _LL1243;} else{ goto _LL1244;} _LL1244: if(( unsigned int)
_temp1212 > 2u?*(( int*) _temp1212) == Cyc_Absyn_Enum_p: 0){ _LL1299: _temp1298=((
struct Cyc_Absyn_Enum_p_struct*) _temp1212)->f1; goto _LL1297; _LL1297:
_temp1296=(( struct Cyc_Absyn_Enum_p_struct*) _temp1212)->f2; goto _LL1245;}
else{ goto _LL1213;} _LL1215: Cyc_Absyndump_dump_char(( int)'_'); goto _LL1213;
_LL1217: Cyc_Absyndump_dump(({ unsigned char* _temp1300=( unsigned char*)"null";
struct _tagged_string _temp1301; _temp1301.curr= _temp1300; _temp1301.base=
_temp1300; _temp1301.last_plus_one= _temp1300 + 5; _temp1301;})); goto _LL1213;
_LL1219: Cyc_Absyndump_dump(( struct _tagged_string) xprintf("%d", _temp1246));
goto _LL1213; _LL1221: Cyc_Absyndump_dump(( struct _tagged_string) xprintf("%u",(
unsigned int) _temp1250)); goto _LL1213; _LL1223: Cyc_Absyndump_dump(({
unsigned char* _temp1302=( unsigned char*)"'"; struct _tagged_string _temp1303;
_temp1303.curr= _temp1302; _temp1303.base= _temp1302; _temp1303.last_plus_one=
_temp1302 + 2; _temp1303;})); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_char_escape(
_temp1254)); Cyc_Absyndump_dump_nospace(({ unsigned char* _temp1304=(
unsigned char*)"'"; struct _tagged_string _temp1305; _temp1305.curr= _temp1304;
_temp1305.base= _temp1304; _temp1305.last_plus_one= _temp1304 + 2; _temp1305;}));
goto _LL1213; _LL1225: Cyc_Absyndump_dump( _temp1256); goto _LL1213; _LL1227:
Cyc_Absyndump_dumpqvar( _temp1258->name); goto _LL1213; _LL1229:(( void(*)( void(*
f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat,
_temp1260,({ unsigned char* _temp1306=( unsigned char*)"$("; struct
_tagged_string _temp1307; _temp1307.curr= _temp1306; _temp1307.base= _temp1306;
_temp1307.last_plus_one= _temp1306 + 3; _temp1307;}),({ unsigned char* _temp1308=(
unsigned char*)")"; struct _tagged_string _temp1309; _temp1309.curr= _temp1308;
_temp1309.base= _temp1308; _temp1309.last_plus_one= _temp1308 + 2; _temp1309;}),({
unsigned char* _temp1310=( unsigned char*)","; struct _tagged_string _temp1311;
_temp1311.curr= _temp1310; _temp1311.base= _temp1310; _temp1311.last_plus_one=
_temp1310 + 2; _temp1311;})); goto _LL1213; _LL1231: Cyc_Absyndump_dump(({
unsigned char* _temp1312=( unsigned char*)"&"; struct _tagged_string _temp1313;
_temp1313.curr= _temp1312; _temp1313.base= _temp1312; _temp1313.last_plus_one=
_temp1312 + 2; _temp1313;})); Cyc_Absyndump_dumppat( _temp1262); goto _LL1213;
_LL1233: Cyc_Absyndump_dump(({ unsigned char* _temp1314=( unsigned char*)"*";
struct _tagged_string _temp1315; _temp1315.curr= _temp1314; _temp1315.base=
_temp1314; _temp1315.last_plus_one= _temp1314 + 2; _temp1315;})); Cyc_Absyndump_dumpqvar(
_temp1264->name); goto _LL1213; _LL1235: Cyc_Absyndump_dumpqvar( _temp1266);
goto _LL1213; _LL1237: Cyc_Absyndump_dumpqvar( _temp1272); Cyc_Absyndump_dumptvars(
_temp1270);(( void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp1268,({ unsigned char*
_temp1316=( unsigned char*)"("; struct _tagged_string _temp1317; _temp1317.curr=
_temp1316; _temp1317.base= _temp1316; _temp1317.last_plus_one= _temp1316 + 2;
_temp1317;}),({ unsigned char* _temp1318=( unsigned char*)")"; struct
_tagged_string _temp1319; _temp1319.curr= _temp1318; _temp1319.base= _temp1318;
_temp1319.last_plus_one= _temp1318 + 2; _temp1319;}),({ unsigned char* _temp1320=(
unsigned char*)","; struct _tagged_string _temp1321; _temp1321.curr= _temp1320;
_temp1321.base= _temp1320; _temp1321.last_plus_one= _temp1320 + 2; _temp1321;}));
goto _LL1213; _LL1239: Cyc_Absyndump_dumpqvar( _temp1278); Cyc_Absyndump_dumptvars(
_temp1276);(( void(*)( void(* f)( struct _tuple9*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpdp, _temp1274,({ unsigned char*
_temp1322=( unsigned char*)"{"; struct _tagged_string _temp1323; _temp1323.curr=
_temp1322; _temp1323.base= _temp1322; _temp1323.last_plus_one= _temp1322 + 2;
_temp1323;}),({ unsigned char* _temp1324=( unsigned char*)"}"; struct
_tagged_string _temp1325; _temp1325.curr= _temp1324; _temp1325.base= _temp1324;
_temp1325.last_plus_one= _temp1324 + 2; _temp1325;}),({ unsigned char* _temp1326=(
unsigned char*)","; struct _tagged_string _temp1327; _temp1327.curr= _temp1326;
_temp1327.base= _temp1326; _temp1327.last_plus_one= _temp1326 + 2; _temp1327;}));
goto _LL1213; _LL1241: if( _temp1286->name != 0){ Cyc_Absyndump_dumpqvar((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp1286->name))->v);}
Cyc_Absyndump_dumptvars( _temp1282);(( void(*)( void(* f)( struct _tuple9*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpdp,
_temp1280,({ unsigned char* _temp1328=( unsigned char*)"{"; struct
_tagged_string _temp1329; _temp1329.curr= _temp1328; _temp1329.base= _temp1328;
_temp1329.last_plus_one= _temp1328 + 2; _temp1329;}),({ unsigned char* _temp1330=(
unsigned char*)"}"; struct _tagged_string _temp1331; _temp1331.curr= _temp1330;
_temp1331.base= _temp1330; _temp1331.last_plus_one= _temp1330 + 2; _temp1331;}),({
unsigned char* _temp1332=( unsigned char*)","; struct _tagged_string _temp1333;
_temp1333.curr= _temp1332; _temp1333.base= _temp1332; _temp1333.last_plus_one=
_temp1332 + 2; _temp1333;})); goto _LL1213; _LL1243: Cyc_Absyndump_dumpqvar(
_temp1292->name); Cyc_Absyndump_dumptvars( _temp1290); if( _temp1288 != 0){((
void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat, _temp1288,({ unsigned char* _temp1334=( unsigned char*)"(";
struct _tagged_string _temp1335; _temp1335.curr= _temp1334; _temp1335.base=
_temp1334; _temp1335.last_plus_one= _temp1334 + 2; _temp1335;}),({ unsigned char*
_temp1336=( unsigned char*)")"; struct _tagged_string _temp1337; _temp1337.curr=
_temp1336; _temp1337.base= _temp1336; _temp1337.last_plus_one= _temp1336 + 2;
_temp1337;}),({ unsigned char* _temp1338=( unsigned char*)","; struct
_tagged_string _temp1339; _temp1339.curr= _temp1338; _temp1339.base= _temp1338;
_temp1339.last_plus_one= _temp1338 + 2; _temp1339;}));} goto _LL1213; _LL1245:
Cyc_Absyndump_dumpqvar( _temp1296->name); goto _LL1213; _LL1213:;} void Cyc_Absyndump_dumptunionfield(
struct Cyc_Absyn_Tunionfield* ef){ Cyc_Absyndump_dumpqvar( ef->name); if( ef->typs
!= 0){ Cyc_Absyndump_dumpargs( ef->typs);}} void Cyc_Absyndump_dumptunionfields(
struct Cyc_List_List* fields){(( void(*)( void(* f)( struct Cyc_Absyn_Tunionfield*),
struct Cyc_List_List* l, struct _tagged_string sep)) Cyc_Absyndump_dump_sep)(
Cyc_Absyndump_dumptunionfield, fields,({ unsigned char* _temp1340=(
unsigned char*)","; struct _tagged_string _temp1341; _temp1341.curr= _temp1340;
_temp1341.base= _temp1340; _temp1341.last_plus_one= _temp1340 + 2; _temp1341;}));}
void Cyc_Absyndump_dumpenumfield( struct Cyc_Absyn_Enumfield* ef){ Cyc_Absyndump_dumpqvar(
ef->name); if( ef->tag != 0){ Cyc_Absyndump_dump(({ unsigned char* _temp1342=(
unsigned char*)" = "; struct _tagged_string _temp1343; _temp1343.curr= _temp1342;
_temp1343.base= _temp1342; _temp1343.last_plus_one= _temp1342 + 4; _temp1343;}));
Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null( ef->tag));}} void
Cyc_Absyndump_dumpenumfields( struct Cyc_List_List* fields){(( void(*)( void(* f)(
struct Cyc_Absyn_Enumfield*), struct Cyc_List_List* l, struct _tagged_string sep))
Cyc_Absyndump_dump_sep)( Cyc_Absyndump_dumpenumfield, fields,({ unsigned char*
_temp1344=( unsigned char*)","; struct _tagged_string _temp1345; _temp1345.curr=
_temp1344; _temp1345.base= _temp1344; _temp1345.last_plus_one= _temp1344 + 2;
_temp1345;}));} void Cyc_Absyndump_dumpstructfields( struct Cyc_List_List*
fields){ for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null(
fields))->tl){ struct Cyc_Absyn_Structfield _temp1348; struct Cyc_List_List*
_temp1349; struct Cyc_Core_Opt* _temp1351; void* _temp1353; struct Cyc_Absyn_Tqual
_temp1355; struct _tagged_string* _temp1357; struct Cyc_Absyn_Structfield*
_temp1346=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd; _temp1348=* _temp1346; _LL1358: _temp1357= _temp1348.name; goto
_LL1356; _LL1356: _temp1355= _temp1348.tq; goto _LL1354; _LL1354: _temp1353=(
void*) _temp1348.type; goto _LL1352; _LL1352: _temp1351= _temp1348.width; goto
_LL1350; _LL1350: _temp1349= _temp1348.attributes; goto _LL1347; _LL1347:(( void(*)(
struct Cyc_Absyn_Tqual, void*, void(* f)( struct _tagged_string*), struct
_tagged_string*)) Cyc_Absyndump_dumptqtd)( _temp1355, _temp1353, Cyc_Absyndump_dump_str,
_temp1357); Cyc_Absyndump_dumpatts( _temp1349); if( _temp1351 != 0){ Cyc_Absyndump_dump_char((
int)':'); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*)
_check_null( _temp1351))->v);} Cyc_Absyndump_dump_semi();}} void Cyc_Absyndump_dumptypedefname(
struct Cyc_Absyn_Typedefdecl* td){ Cyc_Absyndump_dumpqvar( td->name); Cyc_Absyndump_dumptvars(
td->tvs);} static void Cyc_Absyndump_dump_atts_qvar( struct Cyc_Absyn_Fndecl* fd){
Cyc_Absyndump_dumpatts( fd->attributes); Cyc_Absyndump_dumpqvar( fd->name);}
struct _tuple10{ void* f1; struct _tuple0* f2; } ; static void Cyc_Absyndump_dump_callconv_qvar(
struct _tuple10* pr){{ void* _temp1359=(* pr).f1; _LL1361: if( _temp1359 ==(
void*) Cyc_Absyn_Unused_att){ goto _LL1362;} else{ goto _LL1363;} _LL1363: if(
_temp1359 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL1364;} else{ goto _LL1365;}
_LL1365: if( _temp1359 ==( void*) Cyc_Absyn_Cdecl_att){ goto _LL1366;} else{
goto _LL1367;} _LL1367: if( _temp1359 ==( void*) Cyc_Absyn_Fastcall_att){ goto
_LL1368;} else{ goto _LL1369;} _LL1369: goto _LL1370; _LL1362: goto _LL1360;
_LL1364: Cyc_Absyndump_dump(({ unsigned char* _temp1371=( unsigned char*)"_stdcall";
struct _tagged_string _temp1372; _temp1372.curr= _temp1371; _temp1372.base=
_temp1371; _temp1372.last_plus_one= _temp1371 + 9; _temp1372;})); goto _LL1360;
_LL1366: Cyc_Absyndump_dump(({ unsigned char* _temp1373=( unsigned char*)"_cdecl";
struct _tagged_string _temp1374; _temp1374.curr= _temp1373; _temp1374.base=
_temp1373; _temp1374.last_plus_one= _temp1373 + 7; _temp1374;})); goto _LL1360;
_LL1368: Cyc_Absyndump_dump(({ unsigned char* _temp1375=( unsigned char*)"_fastcall";
struct _tagged_string _temp1376; _temp1376.curr= _temp1375; _temp1376.base=
_temp1375; _temp1376.last_plus_one= _temp1375 + 10; _temp1376;})); goto _LL1360;
_LL1370: goto _LL1360; _LL1360:;} Cyc_Absyndump_dumpqvar((* pr).f2);} static
void Cyc_Absyndump_dump_callconv_fdqvar( struct Cyc_Absyn_Fndecl* fd){ Cyc_Absyndump_dump_callconv(
fd->attributes); Cyc_Absyndump_dumpqvar( fd->name);} static void Cyc_Absyndump_dumpids(
struct Cyc_List_List* vds){ for( 0; vds != 0; vds=(( struct Cyc_List_List*)
_check_null( vds))->tl){ Cyc_Absyndump_dumpqvar((( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( vds))->hd)->name); if((( struct Cyc_List_List*)
_check_null( vds))->tl != 0){ Cyc_Absyndump_dump_char(( int)',');}}} void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl* d){ void* _temp1377=( void*) d->r; struct Cyc_Absyn_Fndecl*
_temp1403; struct Cyc_Absyn_Structdecl* _temp1405; struct Cyc_Absyn_Uniondecl*
_temp1407; struct Cyc_Absyn_Vardecl* _temp1409; struct Cyc_Absyn_Vardecl
_temp1411; struct Cyc_List_List* _temp1412; struct Cyc_Core_Opt* _temp1414;
struct Cyc_Absyn_Exp* _temp1416; void* _temp1418; struct Cyc_Absyn_Tqual
_temp1420; struct _tuple0* _temp1422; void* _temp1424; struct Cyc_Absyn_Tuniondecl*
_temp1426; struct Cyc_Absyn_Tuniondecl _temp1428; int _temp1429; struct Cyc_Core_Opt*
_temp1431; struct Cyc_List_List* _temp1433; struct _tuple0* _temp1435; void*
_temp1437; struct Cyc_Absyn_Enumdecl* _temp1439; struct Cyc_Absyn_Enumdecl
_temp1441; struct Cyc_Core_Opt* _temp1442; struct _tuple0* _temp1444; void*
_temp1446; int _temp1448; struct Cyc_Absyn_Exp* _temp1450; struct Cyc_Core_Opt*
_temp1452; struct Cyc_Core_Opt* _temp1454; struct Cyc_Absyn_Pat* _temp1456;
struct Cyc_List_List* _temp1458; struct Cyc_Absyn_Typedefdecl* _temp1460; struct
Cyc_List_List* _temp1462; struct _tagged_string* _temp1464; struct Cyc_List_List*
_temp1466; struct _tuple0* _temp1468; struct Cyc_List_List* _temp1470; _LL1379:
if(*(( int*) _temp1377) == Cyc_Absyn_Fn_d){ _LL1404: _temp1403=(( struct Cyc_Absyn_Fn_d_struct*)
_temp1377)->f1; goto _LL1380;} else{ goto _LL1381;} _LL1381: if(*(( int*)
_temp1377) == Cyc_Absyn_Struct_d){ _LL1406: _temp1405=(( struct Cyc_Absyn_Struct_d_struct*)
_temp1377)->f1; goto _LL1382;} else{ goto _LL1383;} _LL1383: if(*(( int*)
_temp1377) == Cyc_Absyn_Union_d){ _LL1408: _temp1407=(( struct Cyc_Absyn_Union_d_struct*)
_temp1377)->f1; goto _LL1384;} else{ goto _LL1385;} _LL1385: if(*(( int*)
_temp1377) == Cyc_Absyn_Var_d){ _LL1410: _temp1409=(( struct Cyc_Absyn_Var_d_struct*)
_temp1377)->f1; _temp1411=* _temp1409; _LL1425: _temp1424=( void*) _temp1411.sc;
goto _LL1423; _LL1423: _temp1422= _temp1411.name; goto _LL1421; _LL1421:
_temp1420= _temp1411.tq; goto _LL1419; _LL1419: _temp1418=( void*) _temp1411.type;
goto _LL1417; _LL1417: _temp1416= _temp1411.initializer; goto _LL1415; _LL1415:
_temp1414= _temp1411.rgn; goto _LL1413; _LL1413: _temp1412= _temp1411.attributes;
goto _LL1386;} else{ goto _LL1387;} _LL1387: if(*(( int*) _temp1377) == Cyc_Absyn_Tunion_d){
_LL1427: _temp1426=(( struct Cyc_Absyn_Tunion_d_struct*) _temp1377)->f1;
_temp1428=* _temp1426; _LL1438: _temp1437=( void*) _temp1428.sc; goto _LL1436;
_LL1436: _temp1435= _temp1428.name; goto _LL1434; _LL1434: _temp1433= _temp1428.tvs;
goto _LL1432; _LL1432: _temp1431= _temp1428.fields; goto _LL1430; _LL1430:
_temp1429= _temp1428.is_xtunion; goto _LL1388;} else{ goto _LL1389;} _LL1389:
if(*(( int*) _temp1377) == Cyc_Absyn_Enum_d){ _LL1440: _temp1439=(( struct Cyc_Absyn_Enum_d_struct*)
_temp1377)->f1; _temp1441=* _temp1439; _LL1447: _temp1446=( void*) _temp1441.sc;
goto _LL1445; _LL1445: _temp1444= _temp1441.name; goto _LL1443; _LL1443:
_temp1442= _temp1441.fields; goto _LL1390;} else{ goto _LL1391;} _LL1391: if(*((
int*) _temp1377) == Cyc_Absyn_Let_d){ _LL1457: _temp1456=(( struct Cyc_Absyn_Let_d_struct*)
_temp1377)->f1; goto _LL1455; _LL1455: _temp1454=(( struct Cyc_Absyn_Let_d_struct*)
_temp1377)->f2; goto _LL1453; _LL1453: _temp1452=(( struct Cyc_Absyn_Let_d_struct*)
_temp1377)->f3; goto _LL1451; _LL1451: _temp1450=(( struct Cyc_Absyn_Let_d_struct*)
_temp1377)->f4; goto _LL1449; _LL1449: _temp1448=(( struct Cyc_Absyn_Let_d_struct*)
_temp1377)->f5; goto _LL1392;} else{ goto _LL1393;} _LL1393: if(*(( int*)
_temp1377) == Cyc_Absyn_Letv_d){ _LL1459: _temp1458=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1377)->f1; goto _LL1394;} else{ goto _LL1395;} _LL1395: if(*(( int*)
_temp1377) == Cyc_Absyn_Typedef_d){ _LL1461: _temp1460=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp1377)->f1; goto _LL1396;} else{ goto _LL1397;} _LL1397: if(*(( int*)
_temp1377) == Cyc_Absyn_Namespace_d){ _LL1465: _temp1464=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1377)->f1; goto _LL1463; _LL1463: _temp1462=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1377)->f2; goto _LL1398;} else{ goto _LL1399;} _LL1399: if(*(( int*)
_temp1377) == Cyc_Absyn_Using_d){ _LL1469: _temp1468=(( struct Cyc_Absyn_Using_d_struct*)
_temp1377)->f1; goto _LL1467; _LL1467: _temp1466=(( struct Cyc_Absyn_Using_d_struct*)
_temp1377)->f2; goto _LL1400;} else{ goto _LL1401;} _LL1401: if(*(( int*)
_temp1377) == Cyc_Absyn_ExternC_d){ _LL1471: _temp1470=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp1377)->f1; goto _LL1402;} else{ goto _LL1378;} _LL1380: if( Cyc_Absynpp_to_VC){
Cyc_Absyndump_dumpatts( _temp1403->attributes);} if( _temp1403->is_inline){ if(
Cyc_Absynpp_to_VC){ Cyc_Absyndump_dump(({ unsigned char* _temp1472=(
unsigned char*)"__inline"; struct _tagged_string _temp1473; _temp1473.curr=
_temp1472; _temp1473.base= _temp1472; _temp1473.last_plus_one= _temp1472 + 9;
_temp1473;}));} else{ Cyc_Absyndump_dump(({ unsigned char* _temp1474=(
unsigned char*)"inline"; struct _tagged_string _temp1475; _temp1475.curr=
_temp1474; _temp1475.base= _temp1474; _temp1475.last_plus_one= _temp1474 + 7;
_temp1475;}));}} Cyc_Absyndump_dumpscope(( void*) _temp1403->sc);{ void* t=(
void*)({ struct Cyc_Absyn_FnType_struct* _temp1477=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1477[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1478; _temp1478.tag= Cyc_Absyn_FnType; _temp1478.f1=({ struct Cyc_Absyn_FnInfo
_temp1479; _temp1479.tvars= _temp1403->tvs; _temp1479.effect= _temp1403->effect;
_temp1479.ret_typ=( void*)(( void*) _temp1403->ret_type); _temp1479.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp1403->args); _temp1479.c_varargs=
_temp1403->c_varargs; _temp1479.cyc_varargs= _temp1403->cyc_varargs; _temp1479.rgn_po=
_temp1403->rgn_po; _temp1479.attributes= 0; _temp1479;}); _temp1478;});
_temp1477;});(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct Cyc_Absyn_Fndecl*),
struct Cyc_Absyn_Fndecl*)) Cyc_Absyndump_dumptqtd)(({ struct Cyc_Absyn_Tqual
_temp1476; _temp1476.q_const= 0; _temp1476.q_volatile= 0; _temp1476.q_restrict=
0; _temp1476;}), t, Cyc_Absynpp_to_VC? Cyc_Absyndump_dump_callconv_fdqvar: Cyc_Absyndump_dump_atts_qvar,
_temp1403); Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt(
_temp1403->body); Cyc_Absyndump_dump_char(( int)'}'); goto _LL1378;} _LL1382:
Cyc_Absyndump_dumpscope(( void*) _temp1405->sc); Cyc_Absyndump_dump(({
unsigned char* _temp1480=( unsigned char*)"struct"; struct _tagged_string
_temp1481; _temp1481.curr= _temp1480; _temp1481.base= _temp1480; _temp1481.last_plus_one=
_temp1480 + 7; _temp1481;})); if( _temp1405->name != 0){ Cyc_Absyndump_dumpqvar((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp1405->name))->v);}
Cyc_Absyndump_dumptvars( _temp1405->tvs); if( _temp1405->fields == 0){ Cyc_Absyndump_dump_semi();}
else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstructfields((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1405->fields))->v);
Cyc_Absyndump_dump(({ unsigned char* _temp1482=( unsigned char*)"}"; struct
_tagged_string _temp1483; _temp1483.curr= _temp1482; _temp1483.base= _temp1482;
_temp1483.last_plus_one= _temp1482 + 2; _temp1483;})); Cyc_Absyndump_dumpatts(
_temp1405->attributes); Cyc_Absyndump_dump(({ unsigned char* _temp1484=(
unsigned char*)";"; struct _tagged_string _temp1485; _temp1485.curr= _temp1484;
_temp1485.base= _temp1484; _temp1485.last_plus_one= _temp1484 + 2; _temp1485;}));}
goto _LL1378; _LL1384: Cyc_Absyndump_dumpscope(( void*) _temp1407->sc); Cyc_Absyndump_dump(({
unsigned char* _temp1486=( unsigned char*)"union"; struct _tagged_string
_temp1487; _temp1487.curr= _temp1486; _temp1487.base= _temp1486; _temp1487.last_plus_one=
_temp1486 + 6; _temp1487;})); if( _temp1407->name != 0){ Cyc_Absyndump_dumpqvar((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp1407->name))->v);}
Cyc_Absyndump_dumptvars( _temp1407->tvs); if( _temp1407->fields == 0){ Cyc_Absyndump_dump_semi();}
else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstructfields((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1407->fields))->v);
Cyc_Absyndump_dump(({ unsigned char* _temp1488=( unsigned char*)"}"; struct
_tagged_string _temp1489; _temp1489.curr= _temp1488; _temp1489.base= _temp1488;
_temp1489.last_plus_one= _temp1488 + 2; _temp1489;})); Cyc_Absyndump_dumpatts(
_temp1407->attributes); Cyc_Absyndump_dump(({ unsigned char* _temp1490=(
unsigned char*)";"; struct _tagged_string _temp1491; _temp1491.curr= _temp1490;
_temp1491.base= _temp1490; _temp1491.last_plus_one= _temp1490 + 2; _temp1491;}));}
goto _LL1378; _LL1386: if( Cyc_Absynpp_to_VC){ Cyc_Absyndump_dumpatts( _temp1412);
Cyc_Absyndump_dumpscope( _temp1424);{ struct Cyc_List_List* _temp1494; void*
_temp1496; struct Cyc_Absyn_Tqual _temp1498; struct _tuple4 _temp1492= Cyc_Absynpp_to_tms(
_temp1420, _temp1418); _LL1499: _temp1498= _temp1492.f1; goto _LL1497; _LL1497:
_temp1496= _temp1492.f2; goto _LL1495; _LL1495: _temp1494= _temp1492.f3; goto
_LL1493; _LL1493: Cyc_Absyndump_dumptq( _temp1498);{ void* call_conv=( void*)
Cyc_Absyn_Unused_att;{ struct Cyc_List_List* tms2= _temp1494; for( 0; tms2 != 0;
tms2=(( struct Cyc_List_List*) _check_null( tms2))->tl){ void* _temp1500=( void*)((
struct Cyc_List_List*) _check_null( tms2))->hd; struct Cyc_List_List* _temp1506;
struct Cyc_Position_Segment* _temp1508; _LL1502: if(( unsigned int) _temp1500 >
1u?*(( int*) _temp1500) == Cyc_Absyn_Attributes_mod: 0){ _LL1509: _temp1508=((
struct Cyc_Absyn_Attributes_mod_struct*) _temp1500)->f1; goto _LL1507; _LL1507:
_temp1506=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp1500)->f2; goto
_LL1503;} else{ goto _LL1504;} _LL1504: goto _LL1505; _LL1503: for( 0; _temp1506
!= 0; _temp1506=(( struct Cyc_List_List*) _check_null( _temp1506))->tl){ void*
_temp1510=( void*)(( struct Cyc_List_List*) _check_null( _temp1506))->hd;
_LL1512: if( _temp1510 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL1513;} else{
goto _LL1514;} _LL1514: if( _temp1510 ==( void*) Cyc_Absyn_Cdecl_att){ goto
_LL1515;} else{ goto _LL1516;} _LL1516: if( _temp1510 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL1517;} else{ goto _LL1518;} _LL1518: goto _LL1519; _LL1513: call_conv=(
void*) Cyc_Absyn_Stdcall_att; goto _LL1511; _LL1515: call_conv=( void*) Cyc_Absyn_Cdecl_att;
goto _LL1511; _LL1517: call_conv=( void*) Cyc_Absyn_Fastcall_att; goto _LL1511;
_LL1519: goto _LL1511; _LL1511:;} goto _LL1501; _LL1505: goto _LL1501; _LL1501:;}}
Cyc_Absyndump_dumptq( _temp1498); Cyc_Absyndump_dumpntyp( _temp1496);{ struct
_tuple10 _temp1521=({ struct _tuple10 _temp1520; _temp1520.f1= call_conv;
_temp1520.f2= _temp1422; _temp1520;}); goto _LL1522; _LL1522:(( void(*)( struct
Cyc_List_List* tms, void(* f)( struct _tuple10*), struct _tuple10* a)) Cyc_Absyndump_dumptms)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1494),
Cyc_Absyndump_dump_callconv_qvar,& _temp1521);}}}} else{ Cyc_Absyndump_dumpscope(
_temp1424);(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct _tuple0*),
struct _tuple0*)) Cyc_Absyndump_dumptqtd)( _temp1420, _temp1418, Cyc_Absyndump_dumpqvar,
_temp1422); Cyc_Absyndump_dumpatts( _temp1412);} if( _temp1416 != 0){ Cyc_Absyndump_dump_char((
int)'='); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null( _temp1416));}
Cyc_Absyndump_dump_semi(); goto _LL1378; _LL1388: Cyc_Absyndump_dumpscope(
_temp1437); if( _temp1429){ Cyc_Absyndump_dump(({ unsigned char* _temp1523=(
unsigned char*)"xtunion "; struct _tagged_string _temp1524; _temp1524.curr=
_temp1523; _temp1524.base= _temp1523; _temp1524.last_plus_one= _temp1523 + 9;
_temp1524;}));} else{ Cyc_Absyndump_dump(({ unsigned char* _temp1525=(
unsigned char*)"tunion "; struct _tagged_string _temp1526; _temp1526.curr=
_temp1525; _temp1526.base= _temp1525; _temp1526.last_plus_one= _temp1525 + 8;
_temp1526;}));} Cyc_Absyndump_dumpqvar( _temp1435); Cyc_Absyndump_dumptvars(
_temp1433); if( _temp1431 == 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumptunionfields(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1431))->v); Cyc_Absyndump_dump_nospace(({ unsigned char*
_temp1527=( unsigned char*)"};"; struct _tagged_string _temp1528; _temp1528.curr=
_temp1527; _temp1528.base= _temp1527; _temp1528.last_plus_one= _temp1527 + 3;
_temp1528;}));} goto _LL1378; _LL1390: Cyc_Absyndump_dumpscope( _temp1446); Cyc_Absyndump_dump(({
unsigned char* _temp1529=( unsigned char*)"enum "; struct _tagged_string
_temp1530; _temp1530.curr= _temp1529; _temp1530.base= _temp1529; _temp1530.last_plus_one=
_temp1529 + 6; _temp1530;})); Cyc_Absyndump_dumpqvar( _temp1444); if( _temp1442
== 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpenumfields((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1442))->v); Cyc_Absyndump_dump_nospace(({
unsigned char* _temp1531=( unsigned char*)"};"; struct _tagged_string _temp1532;
_temp1532.curr= _temp1531; _temp1532.base= _temp1531; _temp1532.last_plus_one=
_temp1531 + 3; _temp1532;}));} return; _LL1392: Cyc_Absyndump_dump(({
unsigned char* _temp1533=( unsigned char*)"let"; struct _tagged_string _temp1534;
_temp1534.curr= _temp1533; _temp1534.base= _temp1533; _temp1534.last_plus_one=
_temp1533 + 4; _temp1534;})); Cyc_Absyndump_dumppat( _temp1456); Cyc_Absyndump_dump_char((
int)'='); Cyc_Absyndump_dumpexp( _temp1450); Cyc_Absyndump_dump_semi(); goto
_LL1378; _LL1394: Cyc_Absyndump_dump(({ unsigned char* _temp1535=( unsigned char*)"let ";
struct _tagged_string _temp1536; _temp1536.curr= _temp1535; _temp1536.base=
_temp1535; _temp1536.last_plus_one= _temp1535 + 5; _temp1536;})); Cyc_Absyndump_dumpids(
_temp1458); Cyc_Absyndump_dump_semi(); goto _LL1378; _LL1396: if( ! Cyc_Absynpp_expand_typedefs){
Cyc_Absyndump_dump(({ unsigned char* _temp1537=( unsigned char*)"typedef";
struct _tagged_string _temp1538; _temp1538.curr= _temp1537; _temp1538.base=
_temp1537; _temp1538.last_plus_one= _temp1537 + 8; _temp1538;}));(( void(*)(
struct Cyc_Absyn_Tqual, void*, void(* f)( struct Cyc_Absyn_Typedefdecl*), struct
Cyc_Absyn_Typedefdecl*)) Cyc_Absyndump_dumptqtd)(({ struct Cyc_Absyn_Tqual
_temp1539; _temp1539.q_const= 0; _temp1539.q_volatile= 0; _temp1539.q_restrict=
0; _temp1539;}),( void*) _temp1460->defn, Cyc_Absyndump_dumptypedefname,
_temp1460); Cyc_Absyndump_dump_semi();} goto _LL1378; _LL1398: Cyc_Absyndump_dump(({
unsigned char* _temp1540=( unsigned char*)"namespace"; struct _tagged_string
_temp1541; _temp1541.curr= _temp1540; _temp1541.base= _temp1540; _temp1541.last_plus_one=
_temp1540 + 10; _temp1541;})); Cyc_Absyndump_dump_str( _temp1464); Cyc_Absyndump_dump_char((
int)'{'); for( 0; _temp1462 != 0; _temp1462=(( struct Cyc_List_List*)
_check_null( _temp1462))->tl){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( _temp1462))->hd);} Cyc_Absyndump_dump_char((
int)'}'); goto _LL1378; _LL1400: Cyc_Absyndump_dump(({ unsigned char* _temp1542=(
unsigned char*)"using"; struct _tagged_string _temp1543; _temp1543.curr=
_temp1542; _temp1543.base= _temp1542; _temp1543.last_plus_one= _temp1542 + 6;
_temp1543;})); Cyc_Absyndump_dumpqvar( _temp1468); Cyc_Absyndump_dump_char(( int)'{');
for( 0; _temp1466 != 0; _temp1466=(( struct Cyc_List_List*) _check_null(
_temp1466))->tl){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( _temp1466))->hd);} Cyc_Absyndump_dump_char(( int)'}'); goto _LL1378;
_LL1402: Cyc_Absyndump_dump(({ unsigned char* _temp1544=( unsigned char*)"extern \"C\" {";
struct _tagged_string _temp1545; _temp1545.curr= _temp1544; _temp1545.base=
_temp1544; _temp1545.last_plus_one= _temp1544 + 13; _temp1545;})); for( 0;
_temp1470 != 0; _temp1470=(( struct Cyc_List_List*) _check_null( _temp1470))->tl){
Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( _temp1470))->hd);} Cyc_Absyndump_dump_char(( int)'}'); goto _LL1378;
_LL1378:;} static void Cyc_Absyndump_dump_upperbound( struct Cyc_Absyn_Exp* e){
unsigned int i= Cyc_Evexp_eval_const_uint_exp( e); if( i != 1){ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpexp( e); Cyc_Absyndump_dump_char(( int)'}');}} void
Cyc_Absyndump_dumptms( struct Cyc_List_List* tms, void(* f)( void*), void* a){
if( tms == 0){ f( a); return;}{ void* _temp1546=( void*)(( struct Cyc_List_List*)
_check_null( tms))->hd; struct Cyc_Absyn_Tqual _temp1564; void* _temp1566; void*
_temp1568; struct Cyc_Absyn_Exp* _temp1570; struct Cyc_Absyn_Tqual _temp1572;
void* _temp1574; void* _temp1576; struct Cyc_Absyn_Exp* _temp1578; struct Cyc_Absyn_Tqual
_temp1580; void* _temp1582; void* _temp1584; struct Cyc_Absyn_Tqual _temp1586;
void* _temp1588; struct Cyc_Absyn_Tvar* _temp1590; void* _temp1592; struct Cyc_Absyn_Exp*
_temp1594; struct Cyc_Absyn_Tqual _temp1596; void* _temp1598; struct Cyc_Absyn_Tvar*
_temp1600; void* _temp1602; struct Cyc_Absyn_Exp* _temp1604; struct Cyc_Absyn_Tqual
_temp1606; void* _temp1608; struct Cyc_Absyn_Tvar* _temp1610; void* _temp1612;
struct Cyc_Absyn_Tqual _temp1614; void* _temp1616; void* _temp1618; _LL1548: if((
unsigned int) _temp1546 > 1u?*(( int*) _temp1546) == Cyc_Absyn_Pointer_mod: 0){
_LL1569: _temp1568=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1546)->f1;
if(( unsigned int) _temp1568 > 1u?*(( int*) _temp1568) == Cyc_Absyn_Nullable_ps:
0){ _LL1571: _temp1570=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp1568)->f1;
goto _LL1567;} else{ goto _LL1550;} _LL1567: _temp1566=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1546)->f2; if( _temp1566 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1565;}
else{ goto _LL1550;} _LL1565: _temp1564=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1546)->f3; goto _LL1549;} else{ goto _LL1550;} _LL1550: if(( unsigned int)
_temp1546 > 1u?*(( int*) _temp1546) == Cyc_Absyn_Pointer_mod: 0){ _LL1577:
_temp1576=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1546)->f1; if((
unsigned int) _temp1576 > 1u?*(( int*) _temp1576) == Cyc_Absyn_NonNullable_ps: 0){
_LL1579: _temp1578=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1576)->f1;
goto _LL1575;} else{ goto _LL1552;} _LL1575: _temp1574=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1546)->f2; if( _temp1574 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1573;}
else{ goto _LL1552;} _LL1573: _temp1572=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1546)->f3; goto _LL1551;} else{ goto _LL1552;} _LL1552: if(( unsigned int)
_temp1546 > 1u?*(( int*) _temp1546) == Cyc_Absyn_Pointer_mod: 0){ _LL1585:
_temp1584=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1546)->f1; if(
_temp1584 ==( void*) Cyc_Absyn_TaggedArray_ps){ goto _LL1583;} else{ goto
_LL1554;} _LL1583: _temp1582=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1546)->f2; if( _temp1582 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1581;}
else{ goto _LL1554;} _LL1581: _temp1580=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1546)->f3; goto _LL1553;} else{ goto _LL1554;} _LL1554: if(( unsigned int)
_temp1546 > 1u?*(( int*) _temp1546) == Cyc_Absyn_Pointer_mod: 0){ _LL1593:
_temp1592=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1546)->f1; if((
unsigned int) _temp1592 > 1u?*(( int*) _temp1592) == Cyc_Absyn_Nullable_ps: 0){
_LL1595: _temp1594=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp1592)->f1; goto
_LL1589;} else{ goto _LL1556;} _LL1589: _temp1588=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1546)->f2; if(( unsigned int) _temp1588 > 4u?*(( int*) _temp1588) == Cyc_Absyn_VarType:
0){ _LL1591: _temp1590=(( struct Cyc_Absyn_VarType_struct*) _temp1588)->f1; goto
_LL1587;} else{ goto _LL1556;} _LL1587: _temp1586=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1546)->f3; goto _LL1555;} else{ goto _LL1556;} _LL1556: if(( unsigned int)
_temp1546 > 1u?*(( int*) _temp1546) == Cyc_Absyn_Pointer_mod: 0){ _LL1603:
_temp1602=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1546)->f1; if((
unsigned int) _temp1602 > 1u?*(( int*) _temp1602) == Cyc_Absyn_NonNullable_ps: 0){
_LL1605: _temp1604=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1602)->f1;
goto _LL1599;} else{ goto _LL1558;} _LL1599: _temp1598=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1546)->f2; if(( unsigned int) _temp1598 > 4u?*(( int*) _temp1598) == Cyc_Absyn_VarType:
0){ _LL1601: _temp1600=(( struct Cyc_Absyn_VarType_struct*) _temp1598)->f1; goto
_LL1597;} else{ goto _LL1558;} _LL1597: _temp1596=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1546)->f3; goto _LL1557;} else{ goto _LL1558;} _LL1558: if(( unsigned int)
_temp1546 > 1u?*(( int*) _temp1546) == Cyc_Absyn_Pointer_mod: 0){ _LL1613:
_temp1612=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1546)->f1; if(
_temp1612 ==( void*) Cyc_Absyn_TaggedArray_ps){ goto _LL1609;} else{ goto
_LL1560;} _LL1609: _temp1608=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1546)->f2; if(( unsigned int) _temp1608 > 4u?*(( int*) _temp1608) == Cyc_Absyn_VarType:
0){ _LL1611: _temp1610=(( struct Cyc_Absyn_VarType_struct*) _temp1608)->f1; goto
_LL1607;} else{ goto _LL1560;} _LL1607: _temp1606=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1546)->f3; goto _LL1559;} else{ goto _LL1560;} _LL1560: if(( unsigned int)
_temp1546 > 1u?*(( int*) _temp1546) == Cyc_Absyn_Pointer_mod: 0){ _LL1619:
_temp1618=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1546)->f1; goto
_LL1617; _LL1617: _temp1616=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1546)->f2; goto _LL1615; _LL1615: _temp1614=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1546)->f3; goto _LL1561;} else{ goto _LL1562;} _LL1562: goto _LL1563;
_LL1549: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dump_upperbound(
_temp1570);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f, a);
return; _LL1551: Cyc_Absyndump_dump_char(( int)'@'); Cyc_Absyndump_dump_upperbound(
_temp1578);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f, a);
return; _LL1553: Cyc_Absyndump_dump_char(( int)'?');(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*)
_check_null( tms))->tl, f, a); return; _LL1555: Cyc_Absyndump_dump_char(( int)'*');
Cyc_Absyndump_dump_upperbound( _temp1594); Cyc_Absyndump_dump_str( _temp1590->name);((
void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)(((
struct Cyc_List_List*) _check_null( tms))->tl, f, a); return; _LL1557: Cyc_Absyndump_dump_char((
int)'@'); Cyc_Absyndump_dump_upperbound( _temp1604); Cyc_Absyndump_dump_str(
_temp1600->name);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void*
a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f,
a); return; _LL1559: Cyc_Absyndump_dump_char(( int)'?'); Cyc_Absyndump_dump_str(
_temp1610->name);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void*
a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f,
a); return; _LL1561:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1620=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1620[ 0]=({ struct Cyc_Core_Impossible_struct _temp1621; _temp1621.tag= Cyc_Core_Impossible;
_temp1621.f1=({ unsigned char* _temp1622=( unsigned char*)"dumptms: bad Pointer_mod";
struct _tagged_string _temp1623; _temp1623.curr= _temp1622; _temp1623.base=
_temp1622; _temp1623.last_plus_one= _temp1622 + 25; _temp1623;}); _temp1621;});
_temp1620;})); _LL1563: { int next_is_pointer= 0; if((( struct Cyc_List_List*)
_check_null( tms))->tl != 0){ void* _temp1624=( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->hd; struct Cyc_Absyn_Tqual
_temp1630; void* _temp1632; void* _temp1634; _LL1626: if(( unsigned int)
_temp1624 > 1u?*(( int*) _temp1624) == Cyc_Absyn_Pointer_mod: 0){ _LL1635:
_temp1634=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1624)->f1; goto
_LL1633; _LL1633: _temp1632=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1624)->f2; goto _LL1631; _LL1631: _temp1630=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1624)->f3; goto _LL1627;} else{ goto _LL1628;} _LL1628: goto _LL1629;
_LL1627: next_is_pointer= 1; goto _LL1625; _LL1629: goto _LL1625; _LL1625:;} if(
next_is_pointer){ Cyc_Absyndump_dump_char(( int)'(');}(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*)
_check_null( tms))->tl, f, a); if( next_is_pointer){ Cyc_Absyndump_dump_char((
int)')');}{ void* _temp1636=( void*)(( struct Cyc_List_List*) _check_null( tms))->hd;
struct Cyc_Absyn_Exp* _temp1652; void* _temp1654; struct Cyc_List_List*
_temp1656; struct Cyc_Core_Opt* _temp1658; struct Cyc_Absyn_VarargInfo*
_temp1660; int _temp1662; struct Cyc_List_List* _temp1664; void* _temp1666;
struct Cyc_Position_Segment* _temp1668; struct Cyc_List_List* _temp1670; int
_temp1672; struct Cyc_Position_Segment* _temp1674; struct Cyc_List_List*
_temp1676; struct Cyc_List_List* _temp1678; struct Cyc_Position_Segment*
_temp1680; struct Cyc_Absyn_Tqual _temp1682; void* _temp1684; void* _temp1686;
_LL1638: if( _temp1636 ==( void*) Cyc_Absyn_Carray_mod){ goto _LL1639;} else{
goto _LL1640;} _LL1640: if(( unsigned int) _temp1636 > 1u?*(( int*) _temp1636)
== Cyc_Absyn_ConstArray_mod: 0){ _LL1653: _temp1652=(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp1636)->f1; goto _LL1641;} else{ goto _LL1642;} _LL1642: if(( unsigned int)
_temp1636 > 1u?*(( int*) _temp1636) == Cyc_Absyn_Function_mod: 0){ _LL1655:
_temp1654=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp1636)->f1; if(*((
int*) _temp1654) == Cyc_Absyn_WithTypes){ _LL1665: _temp1664=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1654)->f1; goto _LL1663; _LL1663: _temp1662=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1654)->f2; goto _LL1661; _LL1661: _temp1660=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1654)->f3; goto _LL1659; _LL1659: _temp1658=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1654)->f4; goto _LL1657; _LL1657: _temp1656=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1654)->f5; goto _LL1643;} else{ goto _LL1644;}} else{ goto _LL1644;}
_LL1644: if(( unsigned int) _temp1636 > 1u?*(( int*) _temp1636) == Cyc_Absyn_Function_mod:
0){ _LL1667: _temp1666=( void*)(( struct Cyc_Absyn_Function_mod_struct*)
_temp1636)->f1; if(*(( int*) _temp1666) == Cyc_Absyn_NoTypes){ _LL1671:
_temp1670=(( struct Cyc_Absyn_NoTypes_struct*) _temp1666)->f1; goto _LL1669;
_LL1669: _temp1668=(( struct Cyc_Absyn_NoTypes_struct*) _temp1666)->f2; goto
_LL1645;} else{ goto _LL1646;}} else{ goto _LL1646;} _LL1646: if(( unsigned int)
_temp1636 > 1u?*(( int*) _temp1636) == Cyc_Absyn_TypeParams_mod: 0){ _LL1677:
_temp1676=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp1636)->f1; goto
_LL1675; _LL1675: _temp1674=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1636)->f2; goto _LL1673; _LL1673: _temp1672=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1636)->f3; goto _LL1647;} else{ goto _LL1648;} _LL1648: if(( unsigned int)
_temp1636 > 1u?*(( int*) _temp1636) == Cyc_Absyn_Attributes_mod: 0){ _LL1681:
_temp1680=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp1636)->f1; goto
_LL1679; _LL1679: _temp1678=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp1636)->f2; goto _LL1649;} else{ goto _LL1650;} _LL1650: if(( unsigned int)
_temp1636 > 1u?*(( int*) _temp1636) == Cyc_Absyn_Pointer_mod: 0){ _LL1687:
_temp1686=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1636)->f1; goto
_LL1685; _LL1685: _temp1684=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1636)->f2; goto _LL1683; _LL1683: _temp1682=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1636)->f3; goto _LL1651;} else{ goto _LL1637;} _LL1639: Cyc_Absyndump_dump(({
unsigned char* _temp1688=( unsigned char*)"[]"; struct _tagged_string _temp1689;
_temp1689.curr= _temp1688; _temp1689.base= _temp1688; _temp1689.last_plus_one=
_temp1688 + 3; _temp1689;})); goto _LL1637; _LL1641: Cyc_Absyndump_dump_char((
int)'['); Cyc_Absyndump_dumpexp( _temp1652); Cyc_Absyndump_dump_char(( int)']');
goto _LL1637; _LL1643: Cyc_Absyndump_dumpfunargs( _temp1664, _temp1662,
_temp1660, _temp1658, _temp1656); goto _LL1637; _LL1645:(( void(*)( void(* f)(
struct _tagged_string*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dump_str,
_temp1670,({ unsigned char* _temp1690=( unsigned char*)"("; struct
_tagged_string _temp1691; _temp1691.curr= _temp1690; _temp1691.base= _temp1690;
_temp1691.last_plus_one= _temp1690 + 2; _temp1691;}),({ unsigned char* _temp1692=(
unsigned char*)")"; struct _tagged_string _temp1693; _temp1693.curr= _temp1692;
_temp1693.base= _temp1692; _temp1693.last_plus_one= _temp1692 + 2; _temp1693;}),({
unsigned char* _temp1694=( unsigned char*)","; struct _tagged_string _temp1695;
_temp1695.curr= _temp1694; _temp1695.base= _temp1694; _temp1695.last_plus_one=
_temp1694 + 2; _temp1695;})); goto _LL1637; _LL1647: if( _temp1672){ Cyc_Absyndump_dumpkindedtvars(
_temp1676);} else{ Cyc_Absyndump_dumptvars( _temp1676);} goto _LL1637; _LL1649:
Cyc_Absyndump_dumpatts( _temp1678); goto _LL1637; _LL1651:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp1696=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1696[ 0]=({ struct
Cyc_Core_Impossible_struct _temp1697; _temp1697.tag= Cyc_Core_Impossible;
_temp1697.f1=({ unsigned char* _temp1698=( unsigned char*)"dumptms"; struct
_tagged_string _temp1699; _temp1699.curr= _temp1698; _temp1699.base= _temp1698;
_temp1699.last_plus_one= _temp1698 + 8; _temp1699;}); _temp1697;}); _temp1696;}));
_LL1637:;} return;} _LL1547:;}} void Cyc_Absyndump_dumptqtd( struct Cyc_Absyn_Tqual
tq, void* t, void(* f)( void*), void* a){ struct Cyc_List_List* _temp1702; void*
_temp1704; struct Cyc_Absyn_Tqual _temp1706; struct _tuple4 _temp1700= Cyc_Absynpp_to_tms(
tq, t); _LL1707: _temp1706= _temp1700.f1; goto _LL1705; _LL1705: _temp1704=
_temp1700.f2; goto _LL1703; _LL1703: _temp1702= _temp1700.f3; goto _LL1701;
_LL1701: Cyc_Absyndump_dumptq( _temp1706); Cyc_Absyndump_dumpntyp( _temp1704);((
void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1702),
f, a);} void Cyc_Absyndump_dumpdecllist2file( struct Cyc_List_List* tdl, struct
Cyc_Stdio___sFILE* f){ Cyc_Absyndump_pos= 0;* Cyc_Absyndump_dump_file= f; for( 0;
tdl != 0; tdl=(( struct Cyc_List_List*) _check_null( tdl))->tl){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( tdl))->hd);}}