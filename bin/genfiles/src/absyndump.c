 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern struct _tagged_arr Cyc_Core_string_of_int(
int); extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); extern int system( unsigned char*); extern int*
__errno(); struct Cyc_Stdio___sFILE; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stdout;
extern int Cyc_Stdio_fputc( int, struct Cyc_Stdio___sFILE*); extern
unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; extern int Cyc_Stdio_file_string_write( struct Cyc_Stdio___sFILE*
fd, struct _tagged_arr src, int src_offset, int max_count); struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; extern int Cyc_List_length( struct Cyc_List_List*
x); extern unsigned char Cyc_List_List_empty[ 15u]; extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern unsigned char
Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[
8u]; struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct _tagged_arr
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
15u]; extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern int Cyc_Absyn_is_format_prim( void* p); extern struct _tagged_arr Cyc_Absyn_attribute2string(
void*); struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{
int expand_typedefs: 1; int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1;
int decls_first: 1; int rewrite_temp_tvars: 1; int print_all_tvars: 1; int
print_all_kinds: 1; int print_using_stmts: 1; int print_externC_stmts: 1; int
print_full_evars: 1; int use_curr_namespace: 1; struct Cyc_List_List*
curr_namespace; } ; extern void Cyc_Absynpp_set_params( struct Cyc_Absynpp_Params*
fs); extern struct _tagged_arr* Cyc_Absynpp_cyc_stringptr; extern int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*); extern struct _tagged_arr Cyc_Absynpp_char_escape(
unsigned char); extern struct _tagged_arr Cyc_Absynpp_string_escape( struct
_tagged_arr); extern struct _tagged_arr Cyc_Absynpp_prim2str( void* p); extern
int Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s); struct _tuple3{
struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; extern struct
_tuple1* Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg); struct _tuple4{ struct
Cyc_Absyn_Tqual f1; void* f2; struct Cyc_List_List* f3; } ; extern struct
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
0; void Cyc_Absyndump_dump( struct _tagged_arr s){ int sz=( int) _get_arr_size(
s, sizeof( unsigned char)); if( !(( int)*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), sz - 1)))){ -- sz;} Cyc_Absyndump_pos
+= sz + 1; if( Cyc_Absyndump_pos > 80){ Cyc_Absyndump_pos=( unsigned int) sz;
Cyc_Stdio_fputc(( int)'\n',* Cyc_Absyndump_dump_file);} else{ Cyc_Stdio_fputc((
int)' ',* Cyc_Absyndump_dump_file);} Cyc_Stdio_file_string_write(* Cyc_Absyndump_dump_file,
s, 0, sz);} void Cyc_Absyndump_dump_nospace( struct _tagged_arr s){ int sz=( int)
_get_arr_size( s, sizeof( unsigned char)); if( !(( int)*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), sz - 1)))){ -- sz;} Cyc_Absyndump_pos
+= sz; Cyc_Stdio_file_string_write(* Cyc_Absyndump_dump_file, s, 0, sz);} void
Cyc_Absyndump_dump_char( int c){ ++ Cyc_Absyndump_pos; Cyc_Stdio_fputc( c,* Cyc_Absyndump_dump_file);}
void Cyc_Absyndump_dump_str( struct _tagged_arr* s){ Cyc_Absyndump_dump(* s);}
void Cyc_Absyndump_dump_semi(){ Cyc_Absyndump_dump_char(( int)';');} void Cyc_Absyndump_dump_sep(
void(* f)( void*), struct Cyc_List_List* l, struct _tagged_arr sep){ if( l == 0){
return;} for( 0;(( struct Cyc_List_List*) _check_null( l))->tl != 0; l=(( struct
Cyc_List_List*) _check_null( l))->tl){ f(( void*)(( struct Cyc_List_List*)
_check_null( l))->hd); Cyc_Absyndump_dump_nospace( sep);} f(( void*)(( struct
Cyc_List_List*) _check_null( l))->hd);} void Cyc_Absyndump_dump_sep_c( void(* f)(
void*, void*), void* env, struct Cyc_List_List* l, struct _tagged_arr sep){ if(
l == 0){ return;} for( 0;(( struct Cyc_List_List*) _check_null( l))->tl != 0; l=((
struct Cyc_List_List*) _check_null( l))->tl){ f( env,( void*)(( struct Cyc_List_List*)
_check_null( l))->hd); Cyc_Absyndump_dump_nospace( sep);} f( env,( void*)((
struct Cyc_List_List*) _check_null( l))->hd);} void Cyc_Absyndump_group( void(*
f)( void*), struct Cyc_List_List* l, struct _tagged_arr start, struct
_tagged_arr end, struct _tagged_arr sep){ Cyc_Absyndump_dump_nospace( start);((
void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_arr sep))
Cyc_Absyndump_dump_sep)( f, l, sep); Cyc_Absyndump_dump_nospace( end);} void Cyc_Absyndump_group_c(
void(* f)( void*, void*), void* env, struct Cyc_List_List* l, struct _tagged_arr
start, struct _tagged_arr end, struct _tagged_arr sep){ Cyc_Absyndump_dump_nospace(
start);(( void(*)( void(* f)( void*, void*), void* env, struct Cyc_List_List* l,
struct _tagged_arr sep)) Cyc_Absyndump_dump_sep_c)( f, env, l, sep); Cyc_Absyndump_dump_nospace(
end);} void Cyc_Absyndump_egroup( void(* f)( void*), struct Cyc_List_List* l,
struct _tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep){ if( l
!= 0){(( void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_arr
start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)( f,
l, start, end, sep);}} void Cyc_Absyndump_dumpqvar( struct _tuple0* v){ struct
Cyc_List_List* _temp0= 0; goto _LL1; _LL1:{ void* _temp2=(* v).f1; struct Cyc_List_List*
_temp10; struct Cyc_List_List* _temp12; _LL4: if( _temp2 ==( void*) Cyc_Absyn_Loc_n){
goto _LL5;} else{ goto _LL6;} _LL6: if(( unsigned int) _temp2 > 1u?*(( int*)
_temp2) == Cyc_Absyn_Rel_n: 0){ _LL11: _temp10=(( struct Cyc_Absyn_Rel_n_struct*)
_temp2)->f1; goto _LL7;} else{ goto _LL8;} _LL8: if(( unsigned int) _temp2 > 1u?*((
int*) _temp2) == Cyc_Absyn_Abs_n: 0){ _LL13: _temp12=(( struct Cyc_Absyn_Abs_n_struct*)
_temp2)->f1; goto _LL9;} else{ goto _LL3;} _LL5: _temp10= 0; goto _LL7; _LL7:
_temp0= _temp10; goto _LL3; _LL9: _temp0=( Cyc_Absyndump_qvar_to_Cids? Cyc_Absyndump_add_cyc_prefix:
0)?({ struct Cyc_List_List* _temp14=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp14->hd=( void*) Cyc_Absynpp_cyc_stringptr; _temp14->tl=
_temp12; _temp14;}): _temp12; goto _LL3; _LL3:;} if( _temp0 != 0){ Cyc_Absyndump_dump_str((
struct _tagged_arr*)(( struct Cyc_List_List*) _check_null( _temp0))->hd); for(
_temp0=(( struct Cyc_List_List*) _check_null( _temp0))->tl; _temp0 != 0; _temp0=((
struct Cyc_List_List*) _check_null( _temp0))->tl){ if( Cyc_Absyndump_qvar_to_Cids){
Cyc_Absyndump_dump_char(( int)'_');} else{ Cyc_Absyndump_dump_nospace( _tag_arr("::",
sizeof( unsigned char), 3u));} Cyc_Absyndump_dump_nospace(*(( struct _tagged_arr*)((
struct Cyc_List_List*) _check_null( _temp0))->hd));} if( Cyc_Absyndump_qvar_to_Cids){
Cyc_Absyndump_dump_nospace( _tag_arr("_", sizeof( unsigned char), 2u));} else{
Cyc_Absyndump_dump_nospace( _tag_arr("::", sizeof( unsigned char), 3u));} Cyc_Absyndump_dump_nospace(*(*
v).f2);} else{ Cyc_Absyndump_dump_str((* v).f2);}} void Cyc_Absyndump_dumptq(
struct Cyc_Absyn_Tqual tq){ if( tq.q_restrict){ Cyc_Absyndump_dump( _tag_arr("restrict",
sizeof( unsigned char), 9u));} if( tq.q_volatile){ Cyc_Absyndump_dump( _tag_arr("volatile",
sizeof( unsigned char), 9u));} if( tq.q_const){ Cyc_Absyndump_dump( _tag_arr("const",
sizeof( unsigned char), 6u));}} void Cyc_Absyndump_dumpscope( void* sc){ void*
_temp15= sc; _LL17: if( _temp15 ==( void*) Cyc_Absyn_Static){ goto _LL18;} else{
goto _LL19;} _LL19: if( _temp15 ==( void*) Cyc_Absyn_Public){ goto _LL20;} else{
goto _LL21;} _LL21: if( _temp15 ==( void*) Cyc_Absyn_Extern){ goto _LL22;} else{
goto _LL23;} _LL23: if( _temp15 ==( void*) Cyc_Absyn_ExternC){ goto _LL24;}
else{ goto _LL25;} _LL25: if( _temp15 ==( void*) Cyc_Absyn_Abstract){ goto _LL26;}
else{ goto _LL16;} _LL18: Cyc_Absyndump_dump( _tag_arr("static", sizeof(
unsigned char), 7u)); return; _LL20: return; _LL22: Cyc_Absyndump_dump( _tag_arr("extern",
sizeof( unsigned char), 7u)); return; _LL24: Cyc_Absyndump_dump( _tag_arr("extern \"C\"",
sizeof( unsigned char), 11u)); return; _LL26: Cyc_Absyndump_dump( _tag_arr("abstract",
sizeof( unsigned char), 9u)); return; _LL16:;} void Cyc_Absyndump_dumpkind( void*
k){ void* _temp27= k; _LL29: if( _temp27 ==( void*) Cyc_Absyn_AnyKind){ goto
_LL30;} else{ goto _LL31;} _LL31: if( _temp27 ==( void*) Cyc_Absyn_MemKind){
goto _LL32;} else{ goto _LL33;} _LL33: if( _temp27 ==( void*) Cyc_Absyn_BoxKind){
goto _LL34;} else{ goto _LL35;} _LL35: if( _temp27 ==( void*) Cyc_Absyn_RgnKind){
goto _LL36;} else{ goto _LL37;} _LL37: if( _temp27 ==( void*) Cyc_Absyn_EffKind){
goto _LL38;} else{ goto _LL28;} _LL30: Cyc_Absyndump_dump( _tag_arr("A", sizeof(
unsigned char), 2u)); return; _LL32: Cyc_Absyndump_dump( _tag_arr("M", sizeof(
unsigned char), 2u)); return; _LL34: Cyc_Absyndump_dump( _tag_arr("B", sizeof(
unsigned char), 2u)); return; _LL36: Cyc_Absyndump_dump( _tag_arr("R", sizeof(
unsigned char), 2u)); return; _LL38: Cyc_Absyndump_dump( _tag_arr("E", sizeof(
unsigned char), 2u)); return; _LL28:;} void Cyc_Absyndump_dumptps( struct Cyc_List_List*
ts){(( void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_arr
start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumptyp, ts, _tag_arr("<", sizeof( unsigned char), 2u), _tag_arr(">",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u));} void
Cyc_Absyndump_dumptvar( struct Cyc_Absyn_Tvar* tv){ Cyc_Absyndump_dump_str( tv->name);}
void Cyc_Absyndump_dumpkindedtvar( struct Cyc_Absyn_Tvar* tv){ Cyc_Absyndump_dump_str(
tv->name);{ void* _temp39=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( tv->kind))->v; void* _temp47; void* _temp49;
_LL41: if(( unsigned int) _temp39 > 1u?*(( int*) _temp39) == Cyc_Absyn_Eq_constr:
0){ _LL48: _temp47=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp39)->f1;
if( _temp47 ==( void*) Cyc_Absyn_BoxKind){ goto _LL42;} else{ goto _LL43;}}
else{ goto _LL43;} _LL43: if(( unsigned int) _temp39 > 1u?*(( int*) _temp39) ==
Cyc_Absyn_Eq_constr: 0){ _LL50: _temp49=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp39)->f1; goto _LL44;} else{ goto _LL45;} _LL45: goto _LL46; _LL42: goto
_LL40; _LL44: Cyc_Absyndump_dump( _tag_arr("::", sizeof( unsigned char), 3u));
Cyc_Absyndump_dumpkind( _temp49); goto _LL40; _LL46: Cyc_Absyndump_dump(
_tag_arr("::?", sizeof( unsigned char), 4u)); goto _LL40; _LL40:;}} void Cyc_Absyndump_dumptvars(
struct Cyc_List_List* tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr end,
struct _tagged_arr sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumptvar, tvs,
_tag_arr("<", sizeof( unsigned char), 2u), _tag_arr(">", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u));} void Cyc_Absyndump_dumpkindedtvars(
struct Cyc_List_List* tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr end,
struct _tagged_arr sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumpkindedtvar,
tvs, _tag_arr("<", sizeof( unsigned char), 2u), _tag_arr(">", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u));} struct _tuple5{
struct Cyc_Absyn_Tqual f1; void* f2; } ; void Cyc_Absyndump_dumparg( struct
_tuple5* pr){(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( int), int))
Cyc_Absyndump_dumptqtd)((* pr).f1,(* pr).f2,( void(*)( int x)) Cyc_Absyndump_ignore,
0);} void Cyc_Absyndump_dumpargs( struct Cyc_List_List* ts){(( void(*)( void(* f)(
struct _tuple5*), struct Cyc_List_List* l, struct _tagged_arr start, struct
_tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumparg,
ts, _tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u));} void Cyc_Absyndump_dump_callconv(
struct Cyc_List_List* atts){ for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp51=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; _LL53: if( _temp51 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL54;} else{ goto _LL55;} _LL55: if( _temp51 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL56;} else{ goto _LL57;} _LL57: if( _temp51 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL58;} else{ goto _LL59;} _LL59: goto _LL60; _LL54: Cyc_Absyndump_dump(
_tag_arr("_stdcall", sizeof( unsigned char), 9u)); return; _LL56: Cyc_Absyndump_dump(
_tag_arr("_cdecl", sizeof( unsigned char), 7u)); return; _LL58: Cyc_Absyndump_dump(
_tag_arr("_fastcall", sizeof( unsigned char), 10u)); return; _LL60: goto _LL52;
_LL52:;}} void Cyc_Absyndump_dump_noncallconv( struct Cyc_List_List* atts){ int
hasatt= 0;{ struct Cyc_List_List* atts2= atts; for( 0; atts2 != 0; atts2=((
struct Cyc_List_List*) _check_null( atts2))->tl){ void* _temp61=( void*)((
struct Cyc_List_List*) _check_null( atts))->hd; _LL63: if( _temp61 ==( void*)
Cyc_Absyn_Stdcall_att){ goto _LL64;} else{ goto _LL65;} _LL65: if( _temp61 ==(
void*) Cyc_Absyn_Cdecl_att){ goto _LL66;} else{ goto _LL67;} _LL67: if( _temp61
==( void*) Cyc_Absyn_Fastcall_att){ goto _LL68;} else{ goto _LL69;} _LL69: goto
_LL70; _LL64: goto _LL62; _LL66: goto _LL62; _LL68: goto _LL62; _LL70: hasatt= 1;
goto _LL62; _LL62:;}} if( ! hasatt){ return;} Cyc_Absyndump_dump( _tag_arr("__declspec(",
sizeof( unsigned char), 12u)); for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp71=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; _LL73: if( _temp71 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL74;} else{ goto _LL75;} _LL75: if( _temp71 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL76;} else{ goto _LL77;} _LL77: if( _temp71 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL78;} else{ goto _LL79;} _LL79: goto _LL80; _LL74: goto _LL72; _LL76:
goto _LL72; _LL78: goto _LL72; _LL80: Cyc_Absyndump_dump( Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd)); goto _LL72; _LL72:;}
Cyc_Absyndump_dump_char(( int)')');} void Cyc_Absyndump_dumpatts( struct Cyc_List_List*
atts){ if( atts == 0){ return;} if( Cyc_Absyndump_to_VC){ Cyc_Absyndump_dump_noncallconv(
atts); return;} Cyc_Absyndump_dump( _tag_arr(" __attribute__((", sizeof(
unsigned char), 17u)); for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ Cyc_Absyndump_dump( Cyc_Absyn_attribute2string(( void*)((
struct Cyc_List_List*) _check_null( atts))->hd)); if((( struct Cyc_List_List*)
_check_null( atts))->tl != 0){ Cyc_Absyndump_dump( _tag_arr(",", sizeof(
unsigned char), 2u));}} Cyc_Absyndump_dump( _tag_arr(")) ", sizeof(
unsigned char), 4u));} int Cyc_Absyndump_next_is_pointer( struct Cyc_List_List*
tms){ if( tms == 0){ return 0;}{ void* _temp81=( void*)(( struct Cyc_List_List*)
_check_null( tms))->hd; struct Cyc_Absyn_Tqual _temp87; void* _temp89; void*
_temp91; _LL83: if(( unsigned int) _temp81 > 1u?*(( int*) _temp81) == Cyc_Absyn_Pointer_mod:
0){ _LL92: _temp91=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp81)->f1;
goto _LL90; _LL90: _temp89=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp81)->f2; goto _LL88; _LL88: _temp87=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp81)->f3; goto _LL84;} else{ goto _LL85;} _LL85: goto _LL86; _LL84: return 1;
_LL86: return 0; _LL82:;}} static void Cyc_Absyndump_dumprgn( void* t){ void*
_temp93= Cyc_Tcutil_compress( t); _LL95: if( _temp93 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL96;} else{ goto _LL97;} _LL97: goto _LL98; _LL96: Cyc_Absyndump_dump(
_tag_arr("`H", sizeof( unsigned char), 3u)); goto _LL94; _LL98: Cyc_Absyndump_dumpntyp(
t); goto _LL94; _LL94:;} struct _tuple6{ struct Cyc_List_List* f1; struct Cyc_List_List*
f2; } ; static struct _tuple6 Cyc_Absyndump_effects_split( void* t){ struct Cyc_List_List*
regions= 0; struct Cyc_List_List* effects= 0;{ void* _temp99= Cyc_Tcutil_compress(
t); void* _temp107; struct Cyc_List_List* _temp109; _LL101: if(( unsigned int)
_temp99 > 4u?*(( int*) _temp99) == Cyc_Absyn_AccessEff: 0){ _LL108: _temp107=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp99)->f1; goto _LL102;} else{
goto _LL103;} _LL103: if(( unsigned int) _temp99 > 4u?*(( int*) _temp99) == Cyc_Absyn_JoinEff:
0){ _LL110: _temp109=(( struct Cyc_Absyn_JoinEff_struct*) _temp99)->f1; goto
_LL104;} else{ goto _LL105;} _LL105: goto _LL106; _LL102: regions=({ struct Cyc_List_List*
_temp111=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp111->hd=( void*) _temp107; _temp111->tl= regions; _temp111;}); goto _LL100;
_LL104: for( 0; _temp109 != 0; _temp109=(( struct Cyc_List_List*) _check_null(
_temp109))->tl){ struct Cyc_List_List* _temp114; struct Cyc_List_List* _temp116;
struct _tuple6 _temp112= Cyc_Absyndump_effects_split(( void*)(( struct Cyc_List_List*)
_check_null( _temp109))->hd); _LL117: _temp116= _temp112.f1; goto _LL115; _LL115:
_temp114= _temp112.f2; goto _LL113; _LL113: regions=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp116, regions); effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( _temp114, effects);} goto _LL100;
_LL106: effects=({ struct Cyc_List_List* _temp118=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp118->hd=( void*) t; _temp118->tl=
effects; _temp118;}); goto _LL100; _LL100:;} return({ struct _tuple6 _temp119;
_temp119.f1= regions; _temp119.f2= effects; _temp119;});} static void Cyc_Absyndump_dumpeff(
void* t){ struct Cyc_List_List* _temp122; struct Cyc_List_List* _temp124; struct
_tuple6 _temp120= Cyc_Absyndump_effects_split( t); _LL125: _temp124= _temp120.f1;
goto _LL123; _LL123: _temp122= _temp120.f2; goto _LL121; _LL121: _temp124=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp124);
_temp122=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp122); for( 0; _temp122 != 0; _temp122=(( struct Cyc_List_List*) _check_null(
_temp122))->tl){ Cyc_Absyndump_dumpntyp(( void*)(( struct Cyc_List_List*)
_check_null( _temp122))->hd); Cyc_Absyndump_dump_char(( int)'+');} Cyc_Absyndump_dump_char((
int)'{'); for( 0; _temp124 != 0; _temp124=(( struct Cyc_List_List*) _check_null(
_temp124))->tl){ Cyc_Absyndump_dumprgn(( void*)(( struct Cyc_List_List*)
_check_null( _temp124))->hd); if((( struct Cyc_List_List*) _check_null( _temp124))->tl
!= 0){ Cyc_Absyndump_dump_char(( int)',');}} Cyc_Absyndump_dump_char(( int)'}');}
void Cyc_Absyndump_dumpntyp( void* t){ void* _temp126= t; struct Cyc_Absyn_Exp*
_temp196; struct Cyc_Absyn_Tqual _temp198; void* _temp200; struct Cyc_Absyn_FnInfo
_temp202; struct Cyc_Absyn_PtrInfo _temp204; struct Cyc_Absyn_Tvar* _temp206;
struct Cyc_Core_Opt* _temp208; int _temp210; struct Cyc_Core_Opt* _temp212;
struct Cyc_Core_Opt* _temp214; struct Cyc_Core_Opt* _temp216; int _temp218;
struct Cyc_Core_Opt* _temp220; struct Cyc_Core_Opt _temp222; void* _temp223;
struct Cyc_Core_Opt* _temp225; struct Cyc_Absyn_TunionInfo _temp227; void*
_temp229; struct Cyc_List_List* _temp231; void* _temp233; struct Cyc_Absyn_TunionFieldInfo
_temp235; struct Cyc_List_List* _temp237; void* _temp239; struct Cyc_Absyn_Enumdecl*
_temp241; struct _tuple0* _temp243; void* _temp245; void* _temp247; void*
_temp249; void* _temp251; void* _temp253; void* _temp255; void* _temp257; void*
_temp259; void* _temp261; void* _temp263; void* _temp265; void* _temp267; void*
_temp270; void* _temp272; void* _temp274; void* _temp276; void* _temp279; void*
_temp281; void* _temp283; void* _temp285; struct Cyc_List_List* _temp287; struct
Cyc_Absyn_Structdecl** _temp289; struct Cyc_List_List* _temp291; struct _tuple0*
_temp293; struct Cyc_Absyn_Structdecl** _temp295; struct Cyc_List_List* _temp297;
struct _tuple0* _temp299; struct Cyc_Absyn_Uniondecl** _temp301; struct Cyc_List_List*
_temp303; struct _tuple0* _temp305; struct Cyc_Absyn_Uniondecl** _temp307;
struct Cyc_List_List* _temp309; struct _tuple0* _temp311; struct Cyc_List_List*
_temp313; struct Cyc_List_List* _temp315; struct Cyc_Core_Opt* _temp317; struct
Cyc_List_List* _temp319; struct _tuple0* _temp321; void* _temp323; void*
_temp325; struct Cyc_List_List* _temp327; _LL128: if(( unsigned int) _temp126 >
4u?*(( int*) _temp126) == Cyc_Absyn_ArrayType: 0){ _LL201: _temp200=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp126)->f1; goto _LL199; _LL199: _temp198=((
struct Cyc_Absyn_ArrayType_struct*) _temp126)->f2; goto _LL197; _LL197: _temp196=((
struct Cyc_Absyn_ArrayType_struct*) _temp126)->f3; goto _LL129;} else{ goto
_LL130;} _LL130: if(( unsigned int) _temp126 > 4u?*(( int*) _temp126) == Cyc_Absyn_FnType:
0){ _LL203: _temp202=(( struct Cyc_Absyn_FnType_struct*) _temp126)->f1; goto
_LL131;} else{ goto _LL132;} _LL132: if(( unsigned int) _temp126 > 4u?*(( int*)
_temp126) == Cyc_Absyn_PointerType: 0){ _LL205: _temp204=(( struct Cyc_Absyn_PointerType_struct*)
_temp126)->f1; goto _LL133;} else{ goto _LL134;} _LL134: if( _temp126 ==( void*)
Cyc_Absyn_VoidType){ goto _LL135;} else{ goto _LL136;} _LL136: if(( unsigned int)
_temp126 > 4u?*(( int*) _temp126) == Cyc_Absyn_VarType: 0){ _LL207: _temp206=((
struct Cyc_Absyn_VarType_struct*) _temp126)->f1; goto _LL137;} else{ goto _LL138;}
_LL138: if(( unsigned int) _temp126 > 4u?*(( int*) _temp126) == Cyc_Absyn_Evar:
0){ _LL215: _temp214=(( struct Cyc_Absyn_Evar_struct*) _temp126)->f1; goto
_LL213; _LL213: _temp212=(( struct Cyc_Absyn_Evar_struct*) _temp126)->f2; if(
_temp212 == 0){ goto _LL211;} else{ goto _LL140;} _LL211: _temp210=(( struct Cyc_Absyn_Evar_struct*)
_temp126)->f3; goto _LL209; _LL209: _temp208=(( struct Cyc_Absyn_Evar_struct*)
_temp126)->f4; goto _LL139;} else{ goto _LL140;} _LL140: if(( unsigned int)
_temp126 > 4u?*(( int*) _temp126) == Cyc_Absyn_Evar: 0){ _LL226: _temp225=((
struct Cyc_Absyn_Evar_struct*) _temp126)->f1; goto _LL221; _LL221: _temp220=((
struct Cyc_Absyn_Evar_struct*) _temp126)->f2; if( _temp220 == 0){ goto _LL142;}
else{ _temp222=* _temp220; _LL224: _temp223=( void*) _temp222.v; goto _LL219;}
_LL219: _temp218=(( struct Cyc_Absyn_Evar_struct*) _temp126)->f3; goto _LL217;
_LL217: _temp216=(( struct Cyc_Absyn_Evar_struct*) _temp126)->f4; goto _LL141;}
else{ goto _LL142;} _LL142: if(( unsigned int) _temp126 > 4u?*(( int*) _temp126)
== Cyc_Absyn_TunionType: 0){ _LL228: _temp227=(( struct Cyc_Absyn_TunionType_struct*)
_temp126)->f1; _LL234: _temp233=( void*) _temp227.tunion_info; goto _LL232;
_LL232: _temp231= _temp227.targs; goto _LL230; _LL230: _temp229=( void*)
_temp227.rgn; goto _LL143;} else{ goto _LL144;} _LL144: if(( unsigned int)
_temp126 > 4u?*(( int*) _temp126) == Cyc_Absyn_TunionFieldType: 0){ _LL236:
_temp235=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp126)->f1; _LL240:
_temp239=( void*) _temp235.field_info; goto _LL238; _LL238: _temp237= _temp235.targs;
goto _LL145;} else{ goto _LL146;} _LL146: if(( unsigned int) _temp126 > 4u?*((
int*) _temp126) == Cyc_Absyn_EnumType: 0){ _LL244: _temp243=(( struct Cyc_Absyn_EnumType_struct*)
_temp126)->f1; goto _LL242; _LL242: _temp241=(( struct Cyc_Absyn_EnumType_struct*)
_temp126)->f2; goto _LL147;} else{ goto _LL148;} _LL148: if(( unsigned int)
_temp126 > 4u?*(( int*) _temp126) == Cyc_Absyn_IntType: 0){ _LL248: _temp247=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp126)->f1; if( _temp247 ==( void*)
Cyc_Absyn_Signed){ goto _LL246;} else{ goto _LL150;} _LL246: _temp245=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp126)->f2; if( _temp245 ==( void*) Cyc_Absyn_B4){
goto _LL149;} else{ goto _LL150;}} else{ goto _LL150;} _LL150: if(( unsigned int)
_temp126 > 4u?*(( int*) _temp126) == Cyc_Absyn_IntType: 0){ _LL252: _temp251=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp126)->f1; if( _temp251 ==( void*)
Cyc_Absyn_Signed){ goto _LL250;} else{ goto _LL152;} _LL250: _temp249=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp126)->f2; if( _temp249 ==( void*) Cyc_Absyn_B1){
goto _LL151;} else{ goto _LL152;}} else{ goto _LL152;} _LL152: if(( unsigned int)
_temp126 > 4u?*(( int*) _temp126) == Cyc_Absyn_IntType: 0){ _LL256: _temp255=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp126)->f1; if( _temp255 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL254;} else{ goto _LL154;} _LL254: _temp253=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp126)->f2; if( _temp253 ==( void*) Cyc_Absyn_B1){
goto _LL153;} else{ goto _LL154;}} else{ goto _LL154;} _LL154: if(( unsigned int)
_temp126 > 4u?*(( int*) _temp126) == Cyc_Absyn_IntType: 0){ _LL260: _temp259=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp126)->f1; if( _temp259 ==( void*)
Cyc_Absyn_Signed){ goto _LL258;} else{ goto _LL156;} _LL258: _temp257=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp126)->f2; if( _temp257 ==( void*) Cyc_Absyn_B2){
goto _LL155;} else{ goto _LL156;}} else{ goto _LL156;} _LL156: if(( unsigned int)
_temp126 > 4u?*(( int*) _temp126) == Cyc_Absyn_IntType: 0){ _LL264: _temp263=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp126)->f1; if( _temp263 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL262;} else{ goto _LL158;} _LL262: _temp261=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp126)->f2; if( _temp261 ==( void*) Cyc_Absyn_B2){
goto _LL157;} else{ goto _LL158;}} else{ goto _LL158;} _LL158: if(( unsigned int)
_temp126 > 4u?*(( int*) _temp126) == Cyc_Absyn_IntType: 0){ _LL268: _temp267=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp126)->f1; if( _temp267 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL266;} else{ goto _LL160;} _LL266: _temp265=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp126)->f2; if( _temp265 ==( void*) Cyc_Absyn_B4){
goto _LL159;} else{ goto _LL160;}} else{ goto _LL160;} _LL160: if(( unsigned int)
_temp126 > 4u?*(( int*) _temp126) == Cyc_Absyn_IntType: 0){ _LL273: _temp272=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp126)->f1; if( _temp272 ==( void*)
Cyc_Absyn_Signed){ goto _LL271;} else{ goto _LL162;} _LL271: _temp270=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp126)->f2; if( _temp270 ==( void*) Cyc_Absyn_B8){
goto _LL269;} else{ goto _LL162;}} else{ goto _LL162;} _LL269: if( Cyc_Absyndump_to_VC){
goto _LL161;} else{ goto _LL162;} _LL162: if(( unsigned int) _temp126 > 4u?*((
int*) _temp126) == Cyc_Absyn_IntType: 0){ _LL277: _temp276=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp126)->f1; if( _temp276 ==( void*) Cyc_Absyn_Signed){ goto _LL275;} else{
goto _LL164;} _LL275: _temp274=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp126)->f2; if( _temp274 ==( void*) Cyc_Absyn_B8){ goto _LL163;} else{ goto
_LL164;}} else{ goto _LL164;} _LL164: if(( unsigned int) _temp126 > 4u?*(( int*)
_temp126) == Cyc_Absyn_IntType: 0){ _LL282: _temp281=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp126)->f1; if( _temp281 ==( void*) Cyc_Absyn_Unsigned){ goto _LL280;} else{
goto _LL166;} _LL280: _temp279=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp126)->f2; if( _temp279 ==( void*) Cyc_Absyn_B8){ goto _LL278;} else{ goto
_LL166;}} else{ goto _LL166;} _LL278: if( Cyc_Absyndump_to_VC){ goto _LL165;}
else{ goto _LL166;} _LL166: if(( unsigned int) _temp126 > 4u?*(( int*) _temp126)
== Cyc_Absyn_IntType: 0){ _LL286: _temp285=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp126)->f1; if( _temp285 ==( void*) Cyc_Absyn_Unsigned){ goto _LL284;} else{
goto _LL168;} _LL284: _temp283=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp126)->f2; if( _temp283 ==( void*) Cyc_Absyn_B8){ goto _LL167;} else{ goto
_LL168;}} else{ goto _LL168;} _LL168: if( _temp126 ==( void*) Cyc_Absyn_FloatType){
goto _LL169;} else{ goto _LL170;} _LL170: if( _temp126 ==( void*) Cyc_Absyn_DoubleType){
goto _LL171;} else{ goto _LL172;} _LL172: if(( unsigned int) _temp126 > 4u?*((
int*) _temp126) == Cyc_Absyn_TupleType: 0){ _LL288: _temp287=(( struct Cyc_Absyn_TupleType_struct*)
_temp126)->f1; goto _LL173;} else{ goto _LL174;} _LL174: if(( unsigned int)
_temp126 > 4u?*(( int*) _temp126) == Cyc_Absyn_StructType: 0){ _LL294: _temp293=((
struct Cyc_Absyn_StructType_struct*) _temp126)->f1; if( _temp293 == 0){ goto
_LL292;} else{ goto _LL176;} _LL292: _temp291=(( struct Cyc_Absyn_StructType_struct*)
_temp126)->f2; goto _LL290; _LL290: _temp289=(( struct Cyc_Absyn_StructType_struct*)
_temp126)->f3; goto _LL175;} else{ goto _LL176;} _LL176: if(( unsigned int)
_temp126 > 4u?*(( int*) _temp126) == Cyc_Absyn_StructType: 0){ _LL300: _temp299=((
struct Cyc_Absyn_StructType_struct*) _temp126)->f1; goto _LL298; _LL298:
_temp297=(( struct Cyc_Absyn_StructType_struct*) _temp126)->f2; goto _LL296;
_LL296: _temp295=(( struct Cyc_Absyn_StructType_struct*) _temp126)->f3; goto
_LL177;} else{ goto _LL178;} _LL178: if(( unsigned int) _temp126 > 4u?*(( int*)
_temp126) == Cyc_Absyn_UnionType: 0){ _LL306: _temp305=(( struct Cyc_Absyn_UnionType_struct*)
_temp126)->f1; if( _temp305 == 0){ goto _LL304;} else{ goto _LL180;} _LL304:
_temp303=(( struct Cyc_Absyn_UnionType_struct*) _temp126)->f2; goto _LL302;
_LL302: _temp301=(( struct Cyc_Absyn_UnionType_struct*) _temp126)->f3; goto
_LL179;} else{ goto _LL180;} _LL180: if(( unsigned int) _temp126 > 4u?*(( int*)
_temp126) == Cyc_Absyn_UnionType: 0){ _LL312: _temp311=(( struct Cyc_Absyn_UnionType_struct*)
_temp126)->f1; goto _LL310; _LL310: _temp309=(( struct Cyc_Absyn_UnionType_struct*)
_temp126)->f2; goto _LL308; _LL308: _temp307=(( struct Cyc_Absyn_UnionType_struct*)
_temp126)->f3; goto _LL181;} else{ goto _LL182;} _LL182: if(( unsigned int)
_temp126 > 4u?*(( int*) _temp126) == Cyc_Absyn_AnonStructType: 0){ _LL314:
_temp313=(( struct Cyc_Absyn_AnonStructType_struct*) _temp126)->f1; goto _LL183;}
else{ goto _LL184;} _LL184: if(( unsigned int) _temp126 > 4u?*(( int*) _temp126)
== Cyc_Absyn_AnonUnionType: 0){ _LL316: _temp315=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp126)->f1; goto _LL185;} else{ goto _LL186;} _LL186: if(( unsigned int)
_temp126 > 4u?*(( int*) _temp126) == Cyc_Absyn_TypedefType: 0){ _LL322: _temp321=((
struct Cyc_Absyn_TypedefType_struct*) _temp126)->f1; goto _LL320; _LL320:
_temp319=(( struct Cyc_Absyn_TypedefType_struct*) _temp126)->f2; goto _LL318;
_LL318: _temp317=(( struct Cyc_Absyn_TypedefType_struct*) _temp126)->f3; goto
_LL187;} else{ goto _LL188;} _LL188: if(( unsigned int) _temp126 > 4u?*(( int*)
_temp126) == Cyc_Absyn_RgnHandleType: 0){ _LL324: _temp323=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp126)->f1; goto _LL189;} else{ goto _LL190;} _LL190: if( _temp126 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL191;} else{ goto _LL192;} _LL192: if(( unsigned int)
_temp126 > 4u?*(( int*) _temp126) == Cyc_Absyn_AccessEff: 0){ _LL326: _temp325=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp126)->f1; goto _LL193;} else{
goto _LL194;} _LL194: if(( unsigned int) _temp126 > 4u?*(( int*) _temp126) ==
Cyc_Absyn_JoinEff: 0){ _LL328: _temp327=(( struct Cyc_Absyn_JoinEff_struct*)
_temp126)->f1; goto _LL195;} else{ goto _LL127;} _LL129: return; _LL131: return;
_LL133: return; _LL135: Cyc_Absyndump_dump( _tag_arr("void", sizeof(
unsigned char), 5u)); return; _LL137: Cyc_Absyndump_dump_str( _temp206->name);
return; _LL139: Cyc_Absyndump_dump( _tag_arr("%", sizeof( unsigned char), 2u));
if( _temp214 == 0){ Cyc_Absyndump_dump( _tag_arr("?", sizeof( unsigned char), 2u));}
else{ Cyc_Absyndump_dumpkind(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp214))->v);} Cyc_Absyndump_dump(( struct _tagged_arr) xprintf("(%d)",
_temp210)); return; _LL141: Cyc_Absyndump_dumpntyp( _temp223); return; _LL143:{
void* _temp329= _temp233; struct Cyc_Absyn_UnknownTunionInfo _temp335; int
_temp337; struct _tuple0* _temp339; struct Cyc_Absyn_Tuniondecl* _temp341;
struct Cyc_Absyn_Tuniondecl _temp343; int _temp344; struct Cyc_Core_Opt*
_temp346; struct Cyc_List_List* _temp348; struct _tuple0* _temp350; void*
_temp352; _LL331: if(*(( int*) _temp329) == Cyc_Absyn_UnknownTunion){ _LL336:
_temp335=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp329)->f1; _LL340:
_temp339= _temp335.name; goto _LL338; _LL338: _temp337= _temp335.is_xtunion;
goto _LL332;} else{ goto _LL333;} _LL333: if(*(( int*) _temp329) == Cyc_Absyn_KnownTunion){
_LL342: _temp341=(( struct Cyc_Absyn_KnownTunion_struct*) _temp329)->f1;
_temp343=* _temp341; _LL353: _temp352=( void*) _temp343.sc; goto _LL351; _LL351:
_temp350= _temp343.name; goto _LL349; _LL349: _temp348= _temp343.tvs; goto
_LL347; _LL347: _temp346= _temp343.fields; goto _LL345; _LL345: _temp344=
_temp343.is_xtunion; goto _LL334;} else{ goto _LL330;} _LL332: _temp350=
_temp339; _temp344= _temp337; goto _LL334; _LL334: if( _temp344){ Cyc_Absyndump_dump(
_tag_arr("xtunion ", sizeof( unsigned char), 9u));} else{ Cyc_Absyndump_dump(
_tag_arr("tunion ", sizeof( unsigned char), 8u));}{ void* _temp354= Cyc_Tcutil_compress(
_temp229); _LL356: if( _temp354 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL357;}
else{ goto _LL358;} _LL358: goto _LL359; _LL357: goto _LL355; _LL359: Cyc_Absyndump_dumptyp(
_temp229); Cyc_Absyndump_dump( _tag_arr(" ", sizeof( unsigned char), 2u)); goto
_LL355; _LL355:;} Cyc_Absyndump_dumpqvar( _temp350); Cyc_Absyndump_dumptps(
_temp231); goto _LL330; _LL330:;} goto _LL127; _LL145:{ void* _temp360= _temp239;
struct Cyc_Absyn_UnknownTunionFieldInfo _temp366; int _temp368; struct _tuple0*
_temp370; struct _tuple0* _temp372; struct Cyc_Absyn_Tunionfield* _temp374;
struct Cyc_Absyn_Tunionfield _temp376; void* _temp377; struct Cyc_Position_Segment*
_temp379; struct Cyc_List_List* _temp381; struct Cyc_List_List* _temp383; struct
_tuple0* _temp385; struct Cyc_Absyn_Tuniondecl* _temp387; struct Cyc_Absyn_Tuniondecl
_temp389; int _temp390; struct Cyc_Core_Opt* _temp392; struct Cyc_List_List*
_temp394; struct _tuple0* _temp396; void* _temp398; _LL362: if(*(( int*)
_temp360) == Cyc_Absyn_UnknownTunionfield){ _LL367: _temp366=(( struct Cyc_Absyn_UnknownTunionfield_struct*)
_temp360)->f1; _LL373: _temp372= _temp366.tunion_name; goto _LL371; _LL371:
_temp370= _temp366.field_name; goto _LL369; _LL369: _temp368= _temp366.is_xtunion;
goto _LL363;} else{ goto _LL364;} _LL364: if(*(( int*) _temp360) == Cyc_Absyn_KnownTunionfield){
_LL388: _temp387=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp360)->f1;
_temp389=* _temp387; _LL399: _temp398=( void*) _temp389.sc; goto _LL397; _LL397:
_temp396= _temp389.name; goto _LL395; _LL395: _temp394= _temp389.tvs; goto
_LL393; _LL393: _temp392= _temp389.fields; goto _LL391; _LL391: _temp390=
_temp389.is_xtunion; goto _LL375; _LL375: _temp374=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp360)->f2; _temp376=* _temp374; _LL386: _temp385= _temp376.name; goto _LL384;
_LL384: _temp383= _temp376.tvs; goto _LL382; _LL382: _temp381= _temp376.typs;
goto _LL380; _LL380: _temp379= _temp376.loc; goto _LL378; _LL378: _temp377=(
void*) _temp376.sc; goto _LL365;} else{ goto _LL361;} _LL363: _temp396= _temp372;
_temp390= _temp368; _temp385= _temp370; goto _LL365; _LL365: if( _temp390){ Cyc_Absyndump_dump(
_tag_arr("xtunion ", sizeof( unsigned char), 9u));} else{ Cyc_Absyndump_dump(
_tag_arr("tunion ", sizeof( unsigned char), 8u));} Cyc_Absyndump_dumpqvar(
_temp396); Cyc_Absyndump_dump( _tag_arr(".", sizeof( unsigned char), 2u)); Cyc_Absyndump_dumpqvar(
_temp385); Cyc_Absyndump_dumptps( _temp237); goto _LL361; _LL361:;} goto _LL127;
_LL147: Cyc_Absyndump_dump( _tag_arr("enum ", sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpqvar(
_temp243); return; _LL149: Cyc_Absyndump_dump( _tag_arr("int", sizeof(
unsigned char), 4u)); return; _LL151: Cyc_Absyndump_dump( _tag_arr("signed char",
sizeof( unsigned char), 12u)); return; _LL153: Cyc_Absyndump_dump( _tag_arr("unsigned char",
sizeof( unsigned char), 14u)); return; _LL155: Cyc_Absyndump_dump( _tag_arr("short",
sizeof( unsigned char), 6u)); return; _LL157: Cyc_Absyndump_dump( _tag_arr("unsigned short",
sizeof( unsigned char), 15u)); return; _LL159: Cyc_Absyndump_dump( _tag_arr("unsigned int",
sizeof( unsigned char), 13u)); return; _LL161: Cyc_Absyndump_dump( _tag_arr("__int64",
sizeof( unsigned char), 8u)); return; _LL163: Cyc_Absyndump_dump( _tag_arr("long long",
sizeof( unsigned char), 10u)); return; _LL165: Cyc_Absyndump_dump( _tag_arr("unsigned __int64",
sizeof( unsigned char), 17u)); return; _LL167: Cyc_Absyndump_dump( _tag_arr("unsigned long long",
sizeof( unsigned char), 19u)); return; _LL169: Cyc_Absyndump_dump( _tag_arr("float",
sizeof( unsigned char), 6u)); return; _LL171: Cyc_Absyndump_dump( _tag_arr("double",
sizeof( unsigned char), 7u)); return; _LL173: Cyc_Absyndump_dump_char(( int)'$');
Cyc_Absyndump_dumpargs( _temp287); return; _LL175: Cyc_Absyndump_dump( _tag_arr("struct",
sizeof( unsigned char), 7u)); Cyc_Absyndump_dumptps( _temp291); return; _LL177:
Cyc_Absyndump_dump( _tag_arr("struct", sizeof( unsigned char), 7u)); Cyc_Absyndump_dumpqvar((
struct _tuple0*) _check_null( _temp299)); Cyc_Absyndump_dumptps( _temp297);
return; _LL179: Cyc_Absyndump_dump( _tag_arr("union", sizeof( unsigned char), 6u));
Cyc_Absyndump_dumptps( _temp303); return; _LL181: Cyc_Absyndump_dump( _tag_arr("union",
sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpqvar(( struct _tuple0*)
_check_null( _temp311)); Cyc_Absyndump_dumptps( _temp309); return; _LL183: Cyc_Absyndump_dump(
_tag_arr("struct {", sizeof( unsigned char), 9u)); Cyc_Absyndump_dumpstructfields(
_temp313); Cyc_Absyndump_dump( _tag_arr("}", sizeof( unsigned char), 2u));
return; _LL185: Cyc_Absyndump_dump( _tag_arr("union {", sizeof( unsigned char),
8u)); Cyc_Absyndump_dumpstructfields( _temp315); Cyc_Absyndump_dump( _tag_arr("}",
sizeof( unsigned char), 2u)); return; _LL187:( Cyc_Absyndump_dumpqvar( _temp321),
Cyc_Absyndump_dumptps( _temp319)); return; _LL189: Cyc_Absyndump_dumprgn(
_temp323); return; _LL191: return; _LL193: return; _LL195: return; _LL127:;}
void Cyc_Absyndump_dumpvaropt( struct Cyc_Core_Opt* vo){ if( vo != 0){ Cyc_Absyndump_dump_str((
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( vo))->v);}} void Cyc_Absyndump_dumpfunarg(
struct _tuple1* t){(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct
Cyc_Core_Opt*), struct Cyc_Core_Opt*)) Cyc_Absyndump_dumptqtd)((* t).f2,(* t).f3,
Cyc_Absyndump_dumpvaropt,(* t).f1);} struct _tuple7{ void* f1; void* f2; } ;
void Cyc_Absyndump_dump_rgncmp( struct _tuple7* cmp){ struct _tuple7 _temp402;
void* _temp403; void* _temp405; struct _tuple7* _temp400= cmp; _temp402=*
_temp400; _LL406: _temp405= _temp402.f1; goto _LL404; _LL404: _temp403= _temp402.f2;
goto _LL401; _LL401: Cyc_Absyndump_dumptyp( _temp405); Cyc_Absyndump_dump_char((
int)'<'); Cyc_Absyndump_dumptyp( _temp403);} void Cyc_Absyndump_dump_rgnpo(
struct Cyc_List_List* rgn_po){(( void(*)( void(* f)( struct _tuple7*), struct
Cyc_List_List* l, struct _tagged_arr sep)) Cyc_Absyndump_dump_sep)( Cyc_Absyndump_dump_rgncmp,
rgn_po, _tag_arr(",", sizeof( unsigned char), 2u));} void Cyc_Absyndump_dumpfunargs(
struct Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo*
cyc_varargs, struct Cyc_Core_Opt* effopt, struct Cyc_List_List* rgn_po){ Cyc_Absyndump_dump_char((
int)'('); for( 0; args != 0; args=(( struct Cyc_List_List*) _check_null( args))->tl){
Cyc_Absyndump_dumpfunarg(( struct _tuple1*)(( struct Cyc_List_List*) _check_null(
args))->hd); if(((( struct Cyc_List_List*) _check_null( args))->tl != 0? 1:
c_varargs)? 1: cyc_varargs != 0){ Cyc_Absyndump_dump_char(( int)',');}} if(
c_varargs){ Cyc_Absyndump_dump( _tag_arr("...", sizeof( unsigned char), 4u));}
else{ if( cyc_varargs != 0){ struct _tuple1* _temp408=({ struct _tuple1*
_temp407=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp407->f1=((
struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->name; _temp407->f2=((
struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->tq; _temp407->f3=(
void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->type;
_temp407;}); goto _LL409; _LL409: Cyc_Absyndump_dump( _tag_arr("...", sizeof(
unsigned char), 4u)); Cyc_Absyndump_dumptyp(( void*)(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->rgn); if((( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->inject){ Cyc_Absyndump_dump( _tag_arr(" inject ",
sizeof( unsigned char), 9u));} Cyc_Absyndump_dumpfunarg( _temp408);}} if( effopt
!= 0){ Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpeff(( void*)(( struct Cyc_Core_Opt*)
_check_null( effopt))->v);} if( rgn_po != 0){ Cyc_Absyndump_dump_char(( int)':');
Cyc_Absyndump_dump_rgnpo( rgn_po);} Cyc_Absyndump_dump_char(( int)')');} void
Cyc_Absyndump_dumptyp( void* t){(( void(*)( struct Cyc_Absyn_Tqual, void*, void(*
f)( int), int)) Cyc_Absyndump_dumptqtd)(({ struct Cyc_Absyn_Tqual _temp410;
_temp410.q_const= 0; _temp410.q_volatile= 0; _temp410.q_restrict= 0; _temp410;}),
t,( void(*)( int x)) Cyc_Absyndump_ignore, 0);} void Cyc_Absyndump_dumpdesignator(
void* d){ void* _temp411= d; struct Cyc_Absyn_Exp* _temp417; struct _tagged_arr*
_temp419; _LL413: if(*(( int*) _temp411) == Cyc_Absyn_ArrayElement){ _LL418:
_temp417=(( struct Cyc_Absyn_ArrayElement_struct*) _temp411)->f1; goto _LL414;}
else{ goto _LL415;} _LL415: if(*(( int*) _temp411) == Cyc_Absyn_FieldName){
_LL420: _temp419=(( struct Cyc_Absyn_FieldName_struct*) _temp411)->f1; goto
_LL416;} else{ goto _LL412;} _LL414: Cyc_Absyndump_dump( _tag_arr(".[", sizeof(
unsigned char), 3u)); Cyc_Absyndump_dumpexp( _temp417); Cyc_Absyndump_dump_char((
int)']'); goto _LL412; _LL416: Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp419); goto _LL412; _LL412:;} struct _tuple8{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; void Cyc_Absyndump_dumpde( struct _tuple8* de){((
void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_arr start,
struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumpdesignator,(*
de).f1, _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("=", sizeof(
unsigned char), 2u), _tag_arr("=", sizeof( unsigned char), 2u)); Cyc_Absyndump_dumpexp((*
de).f2);} void Cyc_Absyndump_dumpexps_prec( int inprec, struct Cyc_List_List* es){((
void(*)( void(* f)( int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List*
l, struct _tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep))
Cyc_Absyndump_group_c)( Cyc_Absyndump_dumpexp_prec, inprec, es, _tag_arr("",
sizeof( unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u), _tag_arr(",",
sizeof( unsigned char), 2u));} void Cyc_Absyndump_dumpexp_prec( int inprec,
struct Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec( e); if( inprec >=
myprec){ Cyc_Absyndump_dump_nospace( _tag_arr("(", sizeof( unsigned char), 2u));}{
void* _temp421=( void*) e->r; void* _temp515; unsigned char _temp517; void*
_temp519; void* _temp521; short _temp523; void* _temp525; void* _temp527; int
_temp529; void* _temp531; void* _temp533; int _temp535; void* _temp537; void*
_temp539; long long _temp541; void* _temp543; void* _temp545; struct _tagged_arr
_temp547; void* _temp549; void* _temp551; struct _tagged_arr _temp553; struct
_tuple0* _temp555; void* _temp557; struct _tuple0* _temp559; struct Cyc_List_List*
_temp561; void* _temp563; struct Cyc_Absyn_Exp* _temp565; struct Cyc_Core_Opt*
_temp567; struct Cyc_Absyn_Exp* _temp569; void* _temp571; struct Cyc_Absyn_Exp*
_temp573; void* _temp575; struct Cyc_Absyn_Exp* _temp577; void* _temp579; struct
Cyc_Absyn_Exp* _temp581; void* _temp583; struct Cyc_Absyn_Exp* _temp585; struct
Cyc_Absyn_Exp* _temp587; struct Cyc_Absyn_Exp* _temp589; struct Cyc_Absyn_Exp*
_temp591; struct Cyc_Absyn_Exp* _temp593; struct Cyc_Absyn_Exp* _temp595; struct
Cyc_List_List* _temp597; struct Cyc_Absyn_Exp* _temp599; struct Cyc_Absyn_VarargCallInfo*
_temp601; struct Cyc_List_List* _temp603; struct Cyc_Absyn_Exp* _temp605; struct
Cyc_Absyn_Exp* _temp607; struct Cyc_Absyn_Exp* _temp609; struct Cyc_List_List*
_temp611; struct Cyc_Absyn_Exp* _temp613; struct Cyc_Absyn_Exp* _temp615; void*
_temp617; struct Cyc_Absyn_Exp* _temp619; struct Cyc_Absyn_Exp* _temp621; struct
Cyc_Absyn_Exp* _temp623; void* _temp625; struct Cyc_Absyn_Exp* _temp627; struct
_tagged_arr* _temp629; void* _temp631; struct Cyc_Absyn_Exp* _temp633; struct
_tagged_arr* _temp635; struct Cyc_Absyn_Exp* _temp637; struct _tagged_arr*
_temp639; struct Cyc_Absyn_Exp* _temp641; struct Cyc_Absyn_Exp* _temp643; struct
Cyc_Absyn_Exp* _temp645; struct Cyc_List_List* _temp647; struct Cyc_List_List*
_temp649; struct _tuple1* _temp651; struct Cyc_List_List* _temp653; struct Cyc_Absyn_Exp*
_temp655; struct Cyc_Absyn_Exp* _temp657; struct Cyc_Absyn_Vardecl* _temp659;
struct Cyc_Absyn_Structdecl* _temp661; struct Cyc_List_List* _temp663; struct
Cyc_Core_Opt* _temp665; struct _tuple0* _temp667; struct Cyc_List_List* _temp669;
void* _temp671; struct Cyc_Absyn_Tunionfield* _temp673; struct Cyc_Absyn_Tuniondecl*
_temp675; struct Cyc_List_List* _temp677; struct Cyc_Core_Opt* _temp679; struct
Cyc_Core_Opt* _temp681; struct Cyc_Absyn_Enumfield* _temp683; struct Cyc_Absyn_Enumdecl*
_temp685; struct _tuple0* _temp687; void* _temp689; struct Cyc_Absyn_Exp*
_temp691; struct Cyc_List_List* _temp693; struct Cyc_Core_Opt* _temp695; struct
Cyc_Absyn_Stmt* _temp697; struct Cyc_Absyn_Fndecl* _temp699; struct Cyc_Absyn_Exp*
_temp701; _LL423: if(*(( int*) _temp421) == Cyc_Absyn_Const_e){ _LL516: _temp515=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp421)->f1; if(( unsigned int)
_temp515 > 1u?*(( int*) _temp515) == Cyc_Absyn_Char_c: 0){ _LL520: _temp519=(
void*)(( struct Cyc_Absyn_Char_c_struct*) _temp515)->f1; goto _LL518; _LL518:
_temp517=(( struct Cyc_Absyn_Char_c_struct*) _temp515)->f2; goto _LL424;} else{
goto _LL425;}} else{ goto _LL425;} _LL425: if(*(( int*) _temp421) == Cyc_Absyn_Const_e){
_LL522: _temp521=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp421)->f1; if((
unsigned int) _temp521 > 1u?*(( int*) _temp521) == Cyc_Absyn_Short_c: 0){ _LL526:
_temp525=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp521)->f1; goto _LL524;
_LL524: _temp523=(( struct Cyc_Absyn_Short_c_struct*) _temp521)->f2; goto _LL426;}
else{ goto _LL427;}} else{ goto _LL427;} _LL427: if(*(( int*) _temp421) == Cyc_Absyn_Const_e){
_LL528: _temp527=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp421)->f1; if((
unsigned int) _temp527 > 1u?*(( int*) _temp527) == Cyc_Absyn_Int_c: 0){ _LL532:
_temp531=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp527)->f1; if( _temp531
==( void*) Cyc_Absyn_Signed){ goto _LL530;} else{ goto _LL429;} _LL530: _temp529=((
struct Cyc_Absyn_Int_c_struct*) _temp527)->f2; goto _LL428;} else{ goto _LL429;}}
else{ goto _LL429;} _LL429: if(*(( int*) _temp421) == Cyc_Absyn_Const_e){ _LL534:
_temp533=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp421)->f1; if((
unsigned int) _temp533 > 1u?*(( int*) _temp533) == Cyc_Absyn_Int_c: 0){ _LL538:
_temp537=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp533)->f1; if( _temp537
==( void*) Cyc_Absyn_Unsigned){ goto _LL536;} else{ goto _LL431;} _LL536:
_temp535=(( struct Cyc_Absyn_Int_c_struct*) _temp533)->f2; goto _LL430;} else{
goto _LL431;}} else{ goto _LL431;} _LL431: if(*(( int*) _temp421) == Cyc_Absyn_Const_e){
_LL540: _temp539=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp421)->f1; if((
unsigned int) _temp539 > 1u?*(( int*) _temp539) == Cyc_Absyn_LongLong_c: 0){
_LL544: _temp543=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp539)->f1;
goto _LL542; _LL542: _temp541=(( struct Cyc_Absyn_LongLong_c_struct*) _temp539)->f2;
goto _LL432;} else{ goto _LL433;}} else{ goto _LL433;} _LL433: if(*(( int*)
_temp421) == Cyc_Absyn_Const_e){ _LL546: _temp545=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp421)->f1; if(( unsigned int) _temp545 > 1u?*(( int*) _temp545) == Cyc_Absyn_Float_c:
0){ _LL548: _temp547=(( struct Cyc_Absyn_Float_c_struct*) _temp545)->f1; goto
_LL434;} else{ goto _LL435;}} else{ goto _LL435;} _LL435: if(*(( int*) _temp421)
== Cyc_Absyn_Const_e){ _LL550: _temp549=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp421)->f1; if( _temp549 ==( void*) Cyc_Absyn_Null_c){ goto _LL436;} else{
goto _LL437;}} else{ goto _LL437;} _LL437: if(*(( int*) _temp421) == Cyc_Absyn_Const_e){
_LL552: _temp551=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp421)->f1; if((
unsigned int) _temp551 > 1u?*(( int*) _temp551) == Cyc_Absyn_String_c: 0){
_LL554: _temp553=(( struct Cyc_Absyn_String_c_struct*) _temp551)->f1; goto
_LL438;} else{ goto _LL439;}} else{ goto _LL439;} _LL439: if(*(( int*) _temp421)
== Cyc_Absyn_UnknownId_e){ _LL556: _temp555=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp421)->f1; goto _LL440;} else{ goto _LL441;} _LL441: if(*(( int*) _temp421)
== Cyc_Absyn_Var_e){ _LL560: _temp559=(( struct Cyc_Absyn_Var_e_struct*)
_temp421)->f1; goto _LL558; _LL558: _temp557=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp421)->f2; goto _LL442;} else{ goto _LL443;} _LL443: if(*(( int*) _temp421)
== Cyc_Absyn_Primop_e){ _LL564: _temp563=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp421)->f1; goto _LL562; _LL562: _temp561=(( struct Cyc_Absyn_Primop_e_struct*)
_temp421)->f2; goto _LL444;} else{ goto _LL445;} _LL445: if(*(( int*) _temp421)
== Cyc_Absyn_AssignOp_e){ _LL570: _temp569=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp421)->f1; goto _LL568; _LL568: _temp567=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp421)->f2; goto _LL566; _LL566: _temp565=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp421)->f3; goto _LL446;} else{ goto _LL447;} _LL447: if(*(( int*) _temp421)
== Cyc_Absyn_Increment_e){ _LL574: _temp573=(( struct Cyc_Absyn_Increment_e_struct*)
_temp421)->f1; goto _LL572; _LL572: _temp571=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp421)->f2; if( _temp571 ==( void*) Cyc_Absyn_PreInc){ goto _LL448;} else{
goto _LL449;}} else{ goto _LL449;} _LL449: if(*(( int*) _temp421) == Cyc_Absyn_Increment_e){
_LL578: _temp577=(( struct Cyc_Absyn_Increment_e_struct*) _temp421)->f1; goto
_LL576; _LL576: _temp575=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp421)->f2; if( _temp575 ==( void*) Cyc_Absyn_PreDec){ goto _LL450;} else{
goto _LL451;}} else{ goto _LL451;} _LL451: if(*(( int*) _temp421) == Cyc_Absyn_Increment_e){
_LL582: _temp581=(( struct Cyc_Absyn_Increment_e_struct*) _temp421)->f1; goto
_LL580; _LL580: _temp579=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp421)->f2; if( _temp579 ==( void*) Cyc_Absyn_PostInc){ goto _LL452;} else{
goto _LL453;}} else{ goto _LL453;} _LL453: if(*(( int*) _temp421) == Cyc_Absyn_Increment_e){
_LL586: _temp585=(( struct Cyc_Absyn_Increment_e_struct*) _temp421)->f1; goto
_LL584; _LL584: _temp583=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp421)->f2; if( _temp583 ==( void*) Cyc_Absyn_PostDec){ goto _LL454;} else{
goto _LL455;}} else{ goto _LL455;} _LL455: if(*(( int*) _temp421) == Cyc_Absyn_Conditional_e){
_LL592: _temp591=(( struct Cyc_Absyn_Conditional_e_struct*) _temp421)->f1; goto
_LL590; _LL590: _temp589=(( struct Cyc_Absyn_Conditional_e_struct*) _temp421)->f2;
goto _LL588; _LL588: _temp587=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp421)->f3; goto _LL456;} else{ goto _LL457;} _LL457: if(*(( int*) _temp421)
== Cyc_Absyn_SeqExp_e){ _LL596: _temp595=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp421)->f1; goto _LL594; _LL594: _temp593=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp421)->f2; goto _LL458;} else{ goto _LL459;} _LL459: if(*(( int*) _temp421)
== Cyc_Absyn_UnknownCall_e){ _LL600: _temp599=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp421)->f1; goto _LL598; _LL598: _temp597=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp421)->f2; goto _LL460;} else{ goto _LL461;} _LL461: if(*(( int*) _temp421)
== Cyc_Absyn_FnCall_e){ _LL606: _temp605=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp421)->f1; goto _LL604; _LL604: _temp603=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp421)->f2; goto _LL602; _LL602: _temp601=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp421)->f3; goto _LL462;} else{ goto _LL463;} _LL463: if(*(( int*) _temp421)
== Cyc_Absyn_Throw_e){ _LL608: _temp607=(( struct Cyc_Absyn_Throw_e_struct*)
_temp421)->f1; goto _LL464;} else{ goto _LL465;} _LL465: if(*(( int*) _temp421)
== Cyc_Absyn_NoInstantiate_e){ _LL610: _temp609=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp421)->f1; goto _LL466;} else{ goto _LL467;} _LL467: if(*(( int*) _temp421)
== Cyc_Absyn_Instantiate_e){ _LL614: _temp613=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp421)->f1; goto _LL612; _LL612: _temp611=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp421)->f2; goto _LL468;} else{ goto _LL469;} _LL469: if(*(( int*) _temp421)
== Cyc_Absyn_Cast_e){ _LL618: _temp617=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp421)->f1; goto _LL616; _LL616: _temp615=(( struct Cyc_Absyn_Cast_e_struct*)
_temp421)->f2; goto _LL470;} else{ goto _LL471;} _LL471: if(*(( int*) _temp421)
== Cyc_Absyn_Address_e){ _LL620: _temp619=(( struct Cyc_Absyn_Address_e_struct*)
_temp421)->f1; goto _LL472;} else{ goto _LL473;} _LL473: if(*(( int*) _temp421)
== Cyc_Absyn_New_e){ _LL624: _temp623=(( struct Cyc_Absyn_New_e_struct*)
_temp421)->f1; goto _LL622; _LL622: _temp621=(( struct Cyc_Absyn_New_e_struct*)
_temp421)->f2; goto _LL474;} else{ goto _LL475;} _LL475: if(*(( int*) _temp421)
== Cyc_Absyn_Sizeoftyp_e){ _LL626: _temp625=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp421)->f1; goto _LL476;} else{ goto _LL477;} _LL477: if(*(( int*) _temp421)
== Cyc_Absyn_Sizeofexp_e){ _LL628: _temp627=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp421)->f1; goto _LL478;} else{ goto _LL479;} _LL479: if(*(( int*) _temp421)
== Cyc_Absyn_Offsetof_e){ _LL632: _temp631=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp421)->f1; goto _LL630; _LL630: _temp629=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp421)->f2; goto _LL480;} else{ goto _LL481;} _LL481: if(*(( int*) _temp421)
== Cyc_Absyn_Deref_e){ _LL634: _temp633=(( struct Cyc_Absyn_Deref_e_struct*)
_temp421)->f1; goto _LL482;} else{ goto _LL483;} _LL483: if(*(( int*) _temp421)
== Cyc_Absyn_StructMember_e){ _LL638: _temp637=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp421)->f1; goto _LL636; _LL636: _temp635=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp421)->f2; goto _LL484;} else{ goto _LL485;} _LL485: if(*(( int*) _temp421)
== Cyc_Absyn_StructArrow_e){ _LL642: _temp641=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp421)->f1; goto _LL640; _LL640: _temp639=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp421)->f2; goto _LL486;} else{ goto _LL487;} _LL487: if(*(( int*) _temp421)
== Cyc_Absyn_Subscript_e){ _LL646: _temp645=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp421)->f1; goto _LL644; _LL644: _temp643=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp421)->f2; goto _LL488;} else{ goto _LL489;} _LL489: if(*(( int*) _temp421)
== Cyc_Absyn_Tuple_e){ _LL648: _temp647=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp421)->f1; goto _LL490;} else{ goto _LL491;} _LL491: if(*(( int*) _temp421)
== Cyc_Absyn_CompoundLit_e){ _LL652: _temp651=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp421)->f1; goto _LL650; _LL650: _temp649=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp421)->f2; goto _LL492;} else{ goto _LL493;} _LL493: if(*(( int*) _temp421)
== Cyc_Absyn_Array_e){ _LL654: _temp653=(( struct Cyc_Absyn_Array_e_struct*)
_temp421)->f1; goto _LL494;} else{ goto _LL495;} _LL495: if(*(( int*) _temp421)
== Cyc_Absyn_Comprehension_e){ _LL660: _temp659=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp421)->f1; goto _LL658; _LL658: _temp657=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp421)->f2; goto _LL656; _LL656: _temp655=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp421)->f3; goto _LL496;} else{ goto _LL497;} _LL497: if(*(( int*) _temp421)
== Cyc_Absyn_Struct_e){ _LL668: _temp667=(( struct Cyc_Absyn_Struct_e_struct*)
_temp421)->f1; goto _LL666; _LL666: _temp665=(( struct Cyc_Absyn_Struct_e_struct*)
_temp421)->f2; goto _LL664; _LL664: _temp663=(( struct Cyc_Absyn_Struct_e_struct*)
_temp421)->f3; goto _LL662; _LL662: _temp661=(( struct Cyc_Absyn_Struct_e_struct*)
_temp421)->f4; goto _LL498;} else{ goto _LL499;} _LL499: if(*(( int*) _temp421)
== Cyc_Absyn_AnonStruct_e){ _LL672: _temp671=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp421)->f1; goto _LL670; _LL670: _temp669=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp421)->f2; goto _LL500;} else{ goto _LL501;} _LL501: if(*(( int*) _temp421)
== Cyc_Absyn_Tunion_e){ _LL682: _temp681=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp421)->f1; goto _LL680; _LL680: _temp679=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp421)->f2; goto _LL678; _LL678: _temp677=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp421)->f3; goto _LL676; _LL676: _temp675=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp421)->f4; goto _LL674; _LL674: _temp673=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp421)->f5; goto _LL502;} else{ goto _LL503;} _LL503: if(*(( int*) _temp421)
== Cyc_Absyn_Enum_e){ _LL688: _temp687=(( struct Cyc_Absyn_Enum_e_struct*)
_temp421)->f1; goto _LL686; _LL686: _temp685=(( struct Cyc_Absyn_Enum_e_struct*)
_temp421)->f2; goto _LL684; _LL684: _temp683=(( struct Cyc_Absyn_Enum_e_struct*)
_temp421)->f3; goto _LL504;} else{ goto _LL505;} _LL505: if(*(( int*) _temp421)
== Cyc_Absyn_Malloc_e){ _LL692: _temp691=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp421)->f1; goto _LL690; _LL690: _temp689=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp421)->f2; goto _LL506;} else{ goto _LL507;} _LL507: if(*(( int*) _temp421)
== Cyc_Absyn_UnresolvedMem_e){ _LL696: _temp695=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp421)->f1; goto _LL694; _LL694: _temp693=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp421)->f2; goto _LL508;} else{ goto _LL509;} _LL509: if(*(( int*) _temp421)
== Cyc_Absyn_StmtExp_e){ _LL698: _temp697=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp421)->f1; goto _LL510;} else{ goto _LL511;} _LL511: if(*(( int*) _temp421)
== Cyc_Absyn_Codegen_e){ _LL700: _temp699=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp421)->f1; goto _LL512;} else{ goto _LL513;} _LL513: if(*(( int*) _temp421)
== Cyc_Absyn_Fill_e){ _LL702: _temp701=(( struct Cyc_Absyn_Fill_e_struct*)
_temp421)->f1; goto _LL514;} else{ goto _LL422;} _LL424: Cyc_Absyndump_dump_char((
int)'\''); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_char_escape( _temp517)); Cyc_Absyndump_dump_char((
int)'\''); goto _LL422; _LL426: Cyc_Absyndump_dump(( struct _tagged_arr) Cyc_Core_string_of_int((
int) _temp523)); goto _LL422; _LL428: Cyc_Absyndump_dump(( struct _tagged_arr)
Cyc_Core_string_of_int( _temp529)); goto _LL422; _LL430: Cyc_Absyndump_dump((
struct _tagged_arr) Cyc_Core_string_of_int( _temp535)); Cyc_Absyndump_dump_nospace(
_tag_arr("u", sizeof( unsigned char), 2u)); goto _LL422; _LL432: Cyc_Absyndump_dump(
_tag_arr("<<FIX LONG LONG CONSTANT>>", sizeof( unsigned char), 27u)); goto
_LL422; _LL434: Cyc_Absyndump_dump( _temp547); goto _LL422; _LL436: Cyc_Absyndump_dump(
_tag_arr("null", sizeof( unsigned char), 5u)); goto _LL422; _LL438: Cyc_Absyndump_dump_char((
int)'"'); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_string_escape( _temp553)); Cyc_Absyndump_dump_char((
int)'"'); goto _LL422; _LL440: _temp559= _temp555; goto _LL442; _LL442: Cyc_Absyndump_dumpqvar(
_temp559); goto _LL422; _LL444: { struct _tagged_arr _temp703= Cyc_Absynpp_prim2str(
_temp563); goto _LL704; _LL704: if( ! Cyc_Absyn_is_format_prim( _temp563)){
switch((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp561)){ case 1:
_LL705: if( _temp563 ==( void*) Cyc_Absyn_Size){ Cyc_Absyndump_dumpexp_prec(
myprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp561))->hd);
Cyc_Absyndump_dump( _tag_arr(".size", sizeof( unsigned char), 6u));} else{ Cyc_Absyndump_dump(
_temp703); Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp561))->hd);} break; case 2: _LL706: Cyc_Absyndump_dumpexp_prec(
myprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp561))->hd);
Cyc_Absyndump_dump( _temp703); Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp561))->tl))->hd); break; default: _LL707:( int) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp709=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp709[ 0]=({ struct Cyc_Core_Failure_struct
_temp710; _temp710.tag= Cyc_Core_Failure; _temp710.f1= _tag_arr("Absyndump -- Bad number of arguments to primop",
sizeof( unsigned char), 47u); _temp710;}); _temp709;}));}} else{ Cyc_Absyndump_dump(
_temp703); Cyc_Absyndump_dump_nospace( _tag_arr("(", sizeof( unsigned char), 2u));
Cyc_Absyndump_dumpexps_prec( 20, _temp561); Cyc_Absyndump_dump_nospace( _tag_arr(")",
sizeof( unsigned char), 2u));} goto _LL422;} _LL446: Cyc_Absyndump_dumpexp_prec(
myprec, _temp569); if( _temp567 != 0){ Cyc_Absyndump_dump( Cyc_Absynpp_prim2str((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp567))->v));} Cyc_Absyndump_dump_nospace(
_tag_arr("=", sizeof( unsigned char), 2u)); Cyc_Absyndump_dumpexp_prec( myprec,
_temp565); goto _LL422; _LL448: Cyc_Absyndump_dump( _tag_arr("++", sizeof(
unsigned char), 3u)); Cyc_Absyndump_dumpexp_prec( myprec, _temp573); goto _LL422;
_LL450: Cyc_Absyndump_dump( _tag_arr("--", sizeof( unsigned char), 3u)); Cyc_Absyndump_dumpexp_prec(
myprec, _temp577); goto _LL422; _LL452: Cyc_Absyndump_dumpexp_prec( myprec,
_temp581); Cyc_Absyndump_dump( _tag_arr("++", sizeof( unsigned char), 3u)); goto
_LL422; _LL454: Cyc_Absyndump_dumpexp_prec( myprec, _temp585); Cyc_Absyndump_dump(
_tag_arr("--", sizeof( unsigned char), 3u)); goto _LL422; _LL456: Cyc_Absyndump_dumpexp_prec(
myprec, _temp591); Cyc_Absyndump_dump_char(( int)'?'); Cyc_Absyndump_dumpexp_prec(
0, _temp589); Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp587); goto _LL422; _LL458: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumpexp_prec(
myprec, _temp595); Cyc_Absyndump_dump_char(( int)','); Cyc_Absyndump_dumpexp_prec(
myprec, _temp593); Cyc_Absyndump_dump_char(( int)')'); goto _LL422; _LL460:
_temp605= _temp599; _temp603= _temp597; goto _LL462; _LL462: Cyc_Absyndump_dumpexp_prec(
myprec, _temp605); Cyc_Absyndump_dump_nospace( _tag_arr("(", sizeof(
unsigned char), 2u)); Cyc_Absyndump_dumpexps_prec( 20, _temp603); Cyc_Absyndump_dump_nospace(
_tag_arr(")", sizeof( unsigned char), 2u)); goto _LL422; _LL464: Cyc_Absyndump_dump(
_tag_arr("throw", sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpexp_prec(
myprec, _temp607); goto _LL422; _LL466: _temp613= _temp609; goto _LL468; _LL468:
Cyc_Absyndump_dumpexp_prec( inprec, _temp613); goto _LL422; _LL470: Cyc_Absyndump_dump_char((
int)'('); Cyc_Absyndump_dumptyp( _temp617); Cyc_Absyndump_dump_char(( int)')');
Cyc_Absyndump_dumpexp_prec( myprec, _temp615); goto _LL422; _LL472: Cyc_Absyndump_dump_char((
int)'&'); Cyc_Absyndump_dumpexp_prec( myprec, _temp619); goto _LL422; _LL474:
Cyc_Absyndump_dump( _tag_arr("new ", sizeof( unsigned char), 5u)); Cyc_Absyndump_dumpexp_prec(
myprec, _temp621); goto _LL422; _LL476: Cyc_Absyndump_dump( _tag_arr("sizeof(",
sizeof( unsigned char), 8u)); Cyc_Absyndump_dumptyp( _temp625); Cyc_Absyndump_dump_char((
int)')'); goto _LL422; _LL478: Cyc_Absyndump_dump( _tag_arr("sizeof(", sizeof(
unsigned char), 8u)); Cyc_Absyndump_dumpexp_prec( myprec, _temp627); Cyc_Absyndump_dump_char((
int)')'); goto _LL422; _LL480: Cyc_Absyndump_dump( _tag_arr("offsetof(", sizeof(
unsigned char), 10u)); Cyc_Absyndump_dumptyp( _temp631); Cyc_Absyndump_dump_char((
int)','); Cyc_Absyndump_dump_nospace(* _temp629); Cyc_Absyndump_dump_char(( int)')');
goto _LL422; _LL482: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp633); goto _LL422; _LL484: Cyc_Absyndump_dumpexp_prec( myprec,
_temp637); Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp635); goto _LL422; _LL486: Cyc_Absyndump_dumpexp_prec( myprec, _temp641);
Cyc_Absyndump_dump_nospace( _tag_arr("->", sizeof( unsigned char), 3u)); Cyc_Absyndump_dump_nospace(*
_temp639); goto _LL422; _LL488: Cyc_Absyndump_dumpexp_prec( myprec, _temp645);
Cyc_Absyndump_dump_char(( int)'['); Cyc_Absyndump_dumpexp( _temp643); Cyc_Absyndump_dump_char((
int)']'); goto _LL422; _LL490: Cyc_Absyndump_dump( _tag_arr("$(", sizeof(
unsigned char), 3u)); Cyc_Absyndump_dumpexps_prec( 20, _temp647); Cyc_Absyndump_dump_char((
int)')'); goto _LL422; _LL492: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumptyp((*
_temp651).f3); Cyc_Absyndump_dump_char(( int)')');(( void(*)( void(* f)( struct
_tuple8*), struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr
end, struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp649, _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u)); goto _LL422;
_LL494:(( void(*)( void(* f)( struct _tuple8*), struct Cyc_List_List* l, struct
_tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp653, _tag_arr("{", sizeof( unsigned char), 2u),
_tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u)); goto _LL422; _LL496: Cyc_Absyndump_dump( _tag_arr("new {for", sizeof(
unsigned char), 9u)); Cyc_Absyndump_dump_str((* _temp659->name).f2); Cyc_Absyndump_dump_char((
int)'<'); Cyc_Absyndump_dumpexp( _temp657); Cyc_Absyndump_dump_char(( int)':');
Cyc_Absyndump_dumpexp( _temp655); Cyc_Absyndump_dump_char(( int)'}'); goto
_LL422; _LL498: Cyc_Absyndump_dumpqvar( _temp667);(( void(*)( void(* f)( struct
_tuple8*), struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr
end, struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp663, _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u)); goto _LL422;
_LL500:(( void(*)( void(* f)( struct _tuple8*), struct Cyc_List_List* l, struct
_tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp669, _tag_arr("{", sizeof( unsigned char), 2u),
_tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u)); goto _LL422; _LL502: Cyc_Absyndump_dumpqvar( _temp673->name); if( _temp677
!= 0){(( void(*)( void(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* l,
struct _tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpexp, _temp677, _tag_arr("(", sizeof( unsigned char), 2u),
_tag_arr(")", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u));} goto _LL422; _LL504: Cyc_Absyndump_dumpqvar( _temp687); goto _LL422;
_LL506: if( _temp691 != 0){ Cyc_Absyndump_dump( _tag_arr("rmalloc(", sizeof(
unsigned char), 9u)); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null(
_temp691)); Cyc_Absyndump_dump( _tag_arr(",", sizeof( unsigned char), 2u));}
else{ Cyc_Absyndump_dump( _tag_arr("malloc(", sizeof( unsigned char), 8u));} Cyc_Absyndump_dump(
_tag_arr("sizeof(", sizeof( unsigned char), 8u)); Cyc_Absyndump_dumptyp(
_temp689); Cyc_Absyndump_dump( _tag_arr("))", sizeof( unsigned char), 3u)); goto
_LL422; _LL508:(( void(*)( void(* f)( struct _tuple8*), struct Cyc_List_List* l,
struct _tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp693, _tag_arr("{", sizeof( unsigned char), 2u),
_tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u)); goto _LL422; _LL510: Cyc_Absyndump_dump_nospace( _tag_arr("({", sizeof(
unsigned char), 3u)); Cyc_Absyndump_dumpstmt( _temp697); Cyc_Absyndump_dump_nospace(
_tag_arr("})", sizeof( unsigned char), 3u)); goto _LL422; _LL512: Cyc_Absyndump_dump(
_tag_arr("codegen(", sizeof( unsigned char), 9u)); Cyc_Absyndump_dumpdecl(({
struct Cyc_Absyn_Decl* _temp711=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof(
struct Cyc_Absyn_Decl)); _temp711->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp712=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp712[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp713; _temp713.tag= Cyc_Absyn_Fn_d;
_temp713.f1= _temp699; _temp713;}); _temp712;})); _temp711->loc= e->loc;
_temp711;})); Cyc_Absyndump_dump( _tag_arr(")", sizeof( unsigned char), 2u));
goto _LL422; _LL514: Cyc_Absyndump_dump( _tag_arr("fill(", sizeof( unsigned char),
6u)); Cyc_Absyndump_dumpexp( _temp701); Cyc_Absyndump_dump( _tag_arr(")",
sizeof( unsigned char), 2u)); goto _LL422; _LL422:;} if( inprec >= myprec){ Cyc_Absyndump_dump_char((
int)')');}} void Cyc_Absyndump_dumpexp( struct Cyc_Absyn_Exp* e){ Cyc_Absyndump_dumpexp_prec(
0, e);} void Cyc_Absyndump_dumpswitchclauses( struct Cyc_List_List* scs){ for( 0;
scs != 0; scs=(( struct Cyc_List_List*) _check_null( scs))->tl){ struct Cyc_Absyn_Switch_clause*
_temp714=( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
scs))->hd; goto _LL715; _LL715: if( _temp714->where_clause == 0?( void*)(
_temp714->pattern)->r ==( void*) Cyc_Absyn_Wild_p: 0){ Cyc_Absyndump_dump(
_tag_arr("default:", sizeof( unsigned char), 9u));} else{ Cyc_Absyndump_dump(
_tag_arr("case", sizeof( unsigned char), 5u)); Cyc_Absyndump_dumppat( _temp714->pattern);
if( _temp714->where_clause != 0){ Cyc_Absyndump_dump( _tag_arr("&&", sizeof(
unsigned char), 3u)); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null(
_temp714->where_clause));} Cyc_Absyndump_dump_nospace( _tag_arr(":", sizeof(
unsigned char), 2u));} Cyc_Absyndump_dumpstmt( _temp714->body);}} void Cyc_Absyndump_dumpstmt(
struct Cyc_Absyn_Stmt* s){ void* _temp716=( void*) s->r; struct Cyc_Absyn_Exp*
_temp762; struct Cyc_Absyn_Stmt* _temp764; struct Cyc_Absyn_Stmt* _temp766;
struct Cyc_Absyn_Exp* _temp768; struct Cyc_Absyn_Exp* _temp770; struct Cyc_Absyn_Stmt*
_temp772; struct Cyc_Absyn_Stmt* _temp774; struct Cyc_Absyn_Exp* _temp776;
struct Cyc_Absyn_Stmt* _temp778; struct _tuple2 _temp780; struct Cyc_Absyn_Stmt*
_temp782; struct Cyc_Absyn_Exp* _temp784; struct Cyc_Absyn_Stmt* _temp786;
struct Cyc_Absyn_Stmt* _temp788; struct Cyc_Absyn_Stmt* _temp790; struct
_tagged_arr* _temp792; struct Cyc_Absyn_Stmt* _temp794; struct _tuple2 _temp796;
struct Cyc_Absyn_Stmt* _temp798; struct Cyc_Absyn_Exp* _temp800; struct _tuple2
_temp802; struct Cyc_Absyn_Stmt* _temp804; struct Cyc_Absyn_Exp* _temp806;
struct Cyc_Absyn_Exp* _temp808; struct Cyc_List_List* _temp810; struct Cyc_Absyn_Exp*
_temp812; struct Cyc_Absyn_Stmt* _temp814; struct Cyc_Absyn_Decl* _temp816;
struct Cyc_Absyn_Stmt* _temp818; struct _tagged_arr* _temp820; struct _tuple2
_temp822; struct Cyc_Absyn_Stmt* _temp824; struct Cyc_Absyn_Exp* _temp826;
struct Cyc_Absyn_Stmt* _temp828; struct Cyc_List_List* _temp830; struct Cyc_Absyn_Exp*
_temp832; struct Cyc_Absyn_Switch_clause** _temp834; struct Cyc_List_List*
_temp836; struct Cyc_Absyn_Switch_clause** _temp838; struct Cyc_List_List*
_temp840; struct Cyc_List_List* _temp842; struct Cyc_Absyn_Stmt* _temp844;
struct Cyc_Absyn_Stmt* _temp846; struct Cyc_Absyn_Vardecl* _temp848; struct Cyc_Absyn_Tvar*
_temp850; struct Cyc_Absyn_Stmt* _temp852; struct Cyc_Absyn_Stmt* _temp854;
_LL718: if( _temp716 ==( void*) Cyc_Absyn_Skip_s){ goto _LL719;} else{ goto
_LL720;} _LL720: if(( unsigned int) _temp716 > 1u?*(( int*) _temp716) == Cyc_Absyn_Exp_s:
0){ _LL763: _temp762=(( struct Cyc_Absyn_Exp_s_struct*) _temp716)->f1; goto
_LL721;} else{ goto _LL722;} _LL722: if(( unsigned int) _temp716 > 1u?*(( int*)
_temp716) == Cyc_Absyn_Seq_s: 0){ _LL767: _temp766=(( struct Cyc_Absyn_Seq_s_struct*)
_temp716)->f1; goto _LL765; _LL765: _temp764=(( struct Cyc_Absyn_Seq_s_struct*)
_temp716)->f2; goto _LL723;} else{ goto _LL724;} _LL724: if(( unsigned int)
_temp716 > 1u?*(( int*) _temp716) == Cyc_Absyn_Return_s: 0){ _LL769: _temp768=((
struct Cyc_Absyn_Return_s_struct*) _temp716)->f1; if( _temp768 == 0){ goto
_LL725;} else{ goto _LL726;}} else{ goto _LL726;} _LL726: if(( unsigned int)
_temp716 > 1u?*(( int*) _temp716) == Cyc_Absyn_Return_s: 0){ _LL771: _temp770=((
struct Cyc_Absyn_Return_s_struct*) _temp716)->f1; goto _LL727;} else{ goto
_LL728;} _LL728: if(( unsigned int) _temp716 > 1u?*(( int*) _temp716) == Cyc_Absyn_IfThenElse_s:
0){ _LL777: _temp776=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp716)->f1;
goto _LL775; _LL775: _temp774=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp716)->f2;
goto _LL773; _LL773: _temp772=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp716)->f3;
goto _LL729;} else{ goto _LL730;} _LL730: if(( unsigned int) _temp716 > 1u?*((
int*) _temp716) == Cyc_Absyn_While_s: 0){ _LL781: _temp780=(( struct Cyc_Absyn_While_s_struct*)
_temp716)->f1; _LL785: _temp784= _temp780.f1; goto _LL783; _LL783: _temp782=
_temp780.f2; goto _LL779; _LL779: _temp778=(( struct Cyc_Absyn_While_s_struct*)
_temp716)->f2; goto _LL731;} else{ goto _LL732;} _LL732: if(( unsigned int)
_temp716 > 1u?*(( int*) _temp716) == Cyc_Absyn_Break_s: 0){ _LL787: _temp786=((
struct Cyc_Absyn_Break_s_struct*) _temp716)->f1; goto _LL733;} else{ goto _LL734;}
_LL734: if(( unsigned int) _temp716 > 1u?*(( int*) _temp716) == Cyc_Absyn_Continue_s:
0){ _LL789: _temp788=(( struct Cyc_Absyn_Continue_s_struct*) _temp716)->f1; goto
_LL735;} else{ goto _LL736;} _LL736: if(( unsigned int) _temp716 > 1u?*(( int*)
_temp716) == Cyc_Absyn_Goto_s: 0){ _LL793: _temp792=(( struct Cyc_Absyn_Goto_s_struct*)
_temp716)->f1; goto _LL791; _LL791: _temp790=(( struct Cyc_Absyn_Goto_s_struct*)
_temp716)->f2; goto _LL737;} else{ goto _LL738;} _LL738: if(( unsigned int)
_temp716 > 1u?*(( int*) _temp716) == Cyc_Absyn_For_s: 0){ _LL809: _temp808=((
struct Cyc_Absyn_For_s_struct*) _temp716)->f1; goto _LL803; _LL803: _temp802=((
struct Cyc_Absyn_For_s_struct*) _temp716)->f2; _LL807: _temp806= _temp802.f1;
goto _LL805; _LL805: _temp804= _temp802.f2; goto _LL797; _LL797: _temp796=((
struct Cyc_Absyn_For_s_struct*) _temp716)->f3; _LL801: _temp800= _temp796.f1;
goto _LL799; _LL799: _temp798= _temp796.f2; goto _LL795; _LL795: _temp794=((
struct Cyc_Absyn_For_s_struct*) _temp716)->f4; goto _LL739;} else{ goto _LL740;}
_LL740: if(( unsigned int) _temp716 > 1u?*(( int*) _temp716) == Cyc_Absyn_Switch_s:
0){ _LL813: _temp812=(( struct Cyc_Absyn_Switch_s_struct*) _temp716)->f1; goto
_LL811; _LL811: _temp810=(( struct Cyc_Absyn_Switch_s_struct*) _temp716)->f2;
goto _LL741;} else{ goto _LL742;} _LL742: if(( unsigned int) _temp716 > 1u?*((
int*) _temp716) == Cyc_Absyn_Decl_s: 0){ _LL817: _temp816=(( struct Cyc_Absyn_Decl_s_struct*)
_temp716)->f1; goto _LL815; _LL815: _temp814=(( struct Cyc_Absyn_Decl_s_struct*)
_temp716)->f2; goto _LL743;} else{ goto _LL744;} _LL744: if(( unsigned int)
_temp716 > 1u?*(( int*) _temp716) == Cyc_Absyn_Label_s: 0){ _LL821: _temp820=((
struct Cyc_Absyn_Label_s_struct*) _temp716)->f1; goto _LL819; _LL819: _temp818=((
struct Cyc_Absyn_Label_s_struct*) _temp716)->f2; goto _LL745;} else{ goto _LL746;}
_LL746: if(( unsigned int) _temp716 > 1u?*(( int*) _temp716) == Cyc_Absyn_Do_s:
0){ _LL829: _temp828=(( struct Cyc_Absyn_Do_s_struct*) _temp716)->f1; goto
_LL823; _LL823: _temp822=(( struct Cyc_Absyn_Do_s_struct*) _temp716)->f2; _LL827:
_temp826= _temp822.f1; goto _LL825; _LL825: _temp824= _temp822.f2; goto _LL747;}
else{ goto _LL748;} _LL748: if(( unsigned int) _temp716 > 1u?*(( int*) _temp716)
== Cyc_Absyn_SwitchC_s: 0){ _LL833: _temp832=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp716)->f1; goto _LL831; _LL831: _temp830=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp716)->f2; goto _LL749;} else{ goto _LL750;} _LL750: if(( unsigned int)
_temp716 > 1u?*(( int*) _temp716) == Cyc_Absyn_Fallthru_s: 0){ _LL837: _temp836=((
struct Cyc_Absyn_Fallthru_s_struct*) _temp716)->f1; if( _temp836 == 0){ goto
_LL835;} else{ goto _LL752;} _LL835: _temp834=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp716)->f2; goto _LL751;} else{ goto _LL752;} _LL752: if(( unsigned int)
_temp716 > 1u?*(( int*) _temp716) == Cyc_Absyn_Fallthru_s: 0){ _LL841: _temp840=((
struct Cyc_Absyn_Fallthru_s_struct*) _temp716)->f1; goto _LL839; _LL839:
_temp838=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp716)->f2; goto _LL753;}
else{ goto _LL754;} _LL754: if(( unsigned int) _temp716 > 1u?*(( int*) _temp716)
== Cyc_Absyn_TryCatch_s: 0){ _LL845: _temp844=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp716)->f1; goto _LL843; _LL843: _temp842=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp716)->f2; goto _LL755;} else{ goto _LL756;} _LL756: if(( unsigned int)
_temp716 > 1u?*(( int*) _temp716) == Cyc_Absyn_Region_s: 0){ _LL851: _temp850=((
struct Cyc_Absyn_Region_s_struct*) _temp716)->f1; goto _LL849; _LL849: _temp848=((
struct Cyc_Absyn_Region_s_struct*) _temp716)->f2; goto _LL847; _LL847: _temp846=((
struct Cyc_Absyn_Region_s_struct*) _temp716)->f3; goto _LL757;} else{ goto
_LL758;} _LL758: if(( unsigned int) _temp716 > 1u?*(( int*) _temp716) == Cyc_Absyn_Cut_s:
0){ _LL853: _temp852=(( struct Cyc_Absyn_Cut_s_struct*) _temp716)->f1; goto
_LL759;} else{ goto _LL760;} _LL760: if(( unsigned int) _temp716 > 1u?*(( int*)
_temp716) == Cyc_Absyn_Splice_s: 0){ _LL855: _temp854=(( struct Cyc_Absyn_Splice_s_struct*)
_temp716)->f1; goto _LL761;} else{ goto _LL717;} _LL719: Cyc_Absyndump_dump_semi();
goto _LL717; _LL721: Cyc_Absyndump_dumpexp( _temp762); Cyc_Absyndump_dump_semi();
goto _LL717; _LL723: if( Cyc_Absynpp_is_declaration( _temp766)){ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpstmt( _temp766); Cyc_Absyndump_dump_char(( int)'}');}
else{ Cyc_Absyndump_dumpstmt( _temp766);} if( Cyc_Absynpp_is_declaration(
_temp764)){ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt( _temp764);
Cyc_Absyndump_dump_char(( int)'}');} else{ Cyc_Absyndump_dumpstmt( _temp764);}
goto _LL717; _LL725: Cyc_Absyndump_dump( _tag_arr("return;", sizeof(
unsigned char), 8u)); goto _LL717; _LL727: Cyc_Absyndump_dump( _tag_arr("return",
sizeof( unsigned char), 7u)); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)
_check_null( _temp770)); Cyc_Absyndump_dump_semi(); goto _LL717; _LL729: Cyc_Absyndump_dump(
_tag_arr("if(", sizeof( unsigned char), 4u)); Cyc_Absyndump_dumpexp( _temp776);
Cyc_Absyndump_dump_nospace( _tag_arr("){", sizeof( unsigned char), 3u)); Cyc_Absyndump_dumpstmt(
_temp774); Cyc_Absyndump_dump_char(( int)'}');{ void* _temp856=( void*) _temp772->r;
_LL858: if( _temp856 ==( void*) Cyc_Absyn_Skip_s){ goto _LL859;} else{ goto
_LL860;} _LL860: goto _LL861; _LL859: goto _LL857; _LL861: Cyc_Absyndump_dump(
_tag_arr("else{", sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpstmt( _temp772);
Cyc_Absyndump_dump_char(( int)'}'); goto _LL857; _LL857:;} goto _LL717; _LL731:
Cyc_Absyndump_dump( _tag_arr("while(", sizeof( unsigned char), 7u)); Cyc_Absyndump_dumpexp(
_temp784); Cyc_Absyndump_dump_nospace( _tag_arr(") {", sizeof( unsigned char), 4u));
Cyc_Absyndump_dumpstmt( _temp778); Cyc_Absyndump_dump_char(( int)'}'); goto
_LL717; _LL733: Cyc_Absyndump_dump( _tag_arr("break;", sizeof( unsigned char), 7u));
goto _LL717; _LL735: Cyc_Absyndump_dump( _tag_arr("continue;", sizeof(
unsigned char), 10u)); goto _LL717; _LL737: Cyc_Absyndump_dump( _tag_arr("goto",
sizeof( unsigned char), 5u)); Cyc_Absyndump_dump_str( _temp792); Cyc_Absyndump_dump_semi();
goto _LL717; _LL739: Cyc_Absyndump_dump( _tag_arr("for(", sizeof( unsigned char),
5u)); Cyc_Absyndump_dumpexp( _temp808); Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpexp(
_temp806); Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpexp( _temp800); Cyc_Absyndump_dump_nospace(
_tag_arr("){", sizeof( unsigned char), 3u)); Cyc_Absyndump_dumpstmt( _temp794);
Cyc_Absyndump_dump_char(( int)'}'); goto _LL717; _LL741: Cyc_Absyndump_dump(
_tag_arr("switch(", sizeof( unsigned char), 8u)); Cyc_Absyndump_dumpexp(
_temp812); Cyc_Absyndump_dump_nospace( _tag_arr("){", sizeof( unsigned char), 3u));
Cyc_Absyndump_dumpswitchclauses( _temp810); Cyc_Absyndump_dump_char(( int)'}');
goto _LL717; _LL743: Cyc_Absyndump_dumpdecl( _temp816); Cyc_Absyndump_dumpstmt(
_temp814); goto _LL717; _LL745: if( Cyc_Absynpp_is_declaration( _temp818)){ Cyc_Absyndump_dump_str(
_temp820); Cyc_Absyndump_dump_nospace( _tag_arr(": {", sizeof( unsigned char), 4u));
Cyc_Absyndump_dumpstmt( _temp818); Cyc_Absyndump_dump_char(( int)'}');} else{
Cyc_Absyndump_dump_str( _temp820); Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpstmt(
_temp818);} goto _LL717; _LL747: Cyc_Absyndump_dump( _tag_arr("do {", sizeof(
unsigned char), 5u)); Cyc_Absyndump_dumpstmt( _temp828); Cyc_Absyndump_dump_nospace(
_tag_arr("} while (", sizeof( unsigned char), 10u)); Cyc_Absyndump_dumpexp(
_temp826); Cyc_Absyndump_dump_nospace( _tag_arr(");", sizeof( unsigned char), 3u));
goto _LL717; _LL749: Cyc_Absyndump_dump( _tag_arr("switch \"C\" (", sizeof(
unsigned char), 13u)); Cyc_Absyndump_dumpexp( _temp832); Cyc_Absyndump_dump_nospace(
_tag_arr("){", sizeof( unsigned char), 3u)); for( 0; _temp830 != 0; _temp830=((
struct Cyc_List_List*) _check_null( _temp830))->tl){ struct Cyc_Absyn_SwitchC_clause
_temp864; struct Cyc_Position_Segment* _temp865; struct Cyc_Absyn_Stmt* _temp867;
struct Cyc_Absyn_Exp* _temp869; struct Cyc_Absyn_SwitchC_clause* _temp862=(
struct Cyc_Absyn_SwitchC_clause*)(( struct Cyc_List_List*) _check_null( _temp830))->hd;
_temp864=* _temp862; _LL870: _temp869= _temp864.cnst_exp; goto _LL868; _LL868:
_temp867= _temp864.body; goto _LL866; _LL866: _temp865= _temp864.loc; goto
_LL863; _LL863: if( _temp869 == 0){ Cyc_Absyndump_dump( _tag_arr("default: ",
sizeof( unsigned char), 10u));} else{ Cyc_Absyndump_dump( _tag_arr("case ",
sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)
_check_null( _temp869)); Cyc_Absyndump_dump_char(( int)':');} Cyc_Absyndump_dumpstmt(
_temp867);} Cyc_Absyndump_dump_char(( int)'}'); goto _LL717; _LL751: Cyc_Absyndump_dump(
_tag_arr("fallthru;", sizeof( unsigned char), 10u)); goto _LL717; _LL753: Cyc_Absyndump_dump(
_tag_arr("fallthru(", sizeof( unsigned char), 10u)); Cyc_Absyndump_dumpexps_prec(
20, _temp840); Cyc_Absyndump_dump_nospace( _tag_arr(");", sizeof( unsigned char),
3u)); goto _LL717; _LL755: Cyc_Absyndump_dump( _tag_arr("try", sizeof(
unsigned char), 4u)); Cyc_Absyndump_dumpstmt( _temp844); Cyc_Absyndump_dump(
_tag_arr("catch {", sizeof( unsigned char), 8u)); Cyc_Absyndump_dumpswitchclauses(
_temp842); Cyc_Absyndump_dump_char(( int)'}'); goto _LL717; _LL757: Cyc_Absyndump_dump(
_tag_arr("region<", sizeof( unsigned char), 8u)); Cyc_Absyndump_dumptvar(
_temp850); Cyc_Absyndump_dump( _tag_arr("> ", sizeof( unsigned char), 3u)); Cyc_Absyndump_dumpqvar(
_temp848->name); Cyc_Absyndump_dump( _tag_arr("{", sizeof( unsigned char), 2u));
Cyc_Absyndump_dumpstmt( _temp846); Cyc_Absyndump_dump( _tag_arr("}", sizeof(
unsigned char), 2u)); goto _LL717; _LL759: Cyc_Absyndump_dump( _tag_arr("cut",
sizeof( unsigned char), 4u)); Cyc_Absyndump_dumpstmt( _temp852); goto _LL717;
_LL761: Cyc_Absyndump_dump( _tag_arr("splice", sizeof( unsigned char), 7u)); Cyc_Absyndump_dumpstmt(
_temp854); goto _LL717; _LL717:;} struct _tuple9{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Pat* f2; } ; void Cyc_Absyndump_dumpdp( struct _tuple9* dp){((
void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_arr start,
struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumpdesignator,(*
dp).f1, _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("=", sizeof(
unsigned char), 2u), _tag_arr("=", sizeof( unsigned char), 2u)); Cyc_Absyndump_dumppat((*
dp).f2);} void Cyc_Absyndump_dumppat( struct Cyc_Absyn_Pat* p){ void* _temp871=(
void*) p->r; int _temp905; void* _temp907; int _temp909; void* _temp911;
unsigned char _temp913; struct _tagged_arr _temp915; struct Cyc_Absyn_Vardecl*
_temp917; struct Cyc_List_List* _temp919; struct Cyc_Absyn_Pat* _temp921; struct
Cyc_Absyn_Vardecl* _temp923; struct _tuple0* _temp925; struct Cyc_List_List*
_temp927; struct Cyc_List_List* _temp929; struct _tuple0* _temp931; struct Cyc_List_List*
_temp933; struct Cyc_List_List* _temp935; struct _tuple0* _temp937; struct Cyc_List_List*
_temp939; struct Cyc_List_List* _temp941; struct Cyc_Core_Opt* _temp943; struct
Cyc_Absyn_Structdecl* _temp945; struct Cyc_List_List* _temp947; struct Cyc_List_List*
_temp949; struct Cyc_Absyn_Tunionfield* _temp951; struct Cyc_Absyn_Tuniondecl*
_temp953; struct Cyc_Absyn_Enumfield* _temp955; struct Cyc_Absyn_Enumdecl*
_temp957; _LL873: if( _temp871 ==( void*) Cyc_Absyn_Wild_p){ goto _LL874;} else{
goto _LL875;} _LL875: if( _temp871 ==( void*) Cyc_Absyn_Null_p){ goto _LL876;}
else{ goto _LL877;} _LL877: if(( unsigned int) _temp871 > 2u?*(( int*) _temp871)
== Cyc_Absyn_Int_p: 0){ _LL908: _temp907=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp871)->f1; if( _temp907 ==( void*) Cyc_Absyn_Signed){ goto _LL906;} else{
goto _LL879;} _LL906: _temp905=(( struct Cyc_Absyn_Int_p_struct*) _temp871)->f2;
goto _LL878;} else{ goto _LL879;} _LL879: if(( unsigned int) _temp871 > 2u?*((
int*) _temp871) == Cyc_Absyn_Int_p: 0){ _LL912: _temp911=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp871)->f1; if( _temp911 ==( void*) Cyc_Absyn_Unsigned){ goto _LL910;} else{
goto _LL881;} _LL910: _temp909=(( struct Cyc_Absyn_Int_p_struct*) _temp871)->f2;
goto _LL880;} else{ goto _LL881;} _LL881: if(( unsigned int) _temp871 > 2u?*((
int*) _temp871) == Cyc_Absyn_Char_p: 0){ _LL914: _temp913=(( struct Cyc_Absyn_Char_p_struct*)
_temp871)->f1; goto _LL882;} else{ goto _LL883;} _LL883: if(( unsigned int)
_temp871 > 2u?*(( int*) _temp871) == Cyc_Absyn_Float_p: 0){ _LL916: _temp915=((
struct Cyc_Absyn_Float_p_struct*) _temp871)->f1; goto _LL884;} else{ goto _LL885;}
_LL885: if(( unsigned int) _temp871 > 2u?*(( int*) _temp871) == Cyc_Absyn_Var_p:
0){ _LL918: _temp917=(( struct Cyc_Absyn_Var_p_struct*) _temp871)->f1; goto
_LL886;} else{ goto _LL887;} _LL887: if(( unsigned int) _temp871 > 2u?*(( int*)
_temp871) == Cyc_Absyn_Tuple_p: 0){ _LL920: _temp919=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp871)->f1; goto _LL888;} else{ goto _LL889;} _LL889: if(( unsigned int)
_temp871 > 2u?*(( int*) _temp871) == Cyc_Absyn_Pointer_p: 0){ _LL922: _temp921=((
struct Cyc_Absyn_Pointer_p_struct*) _temp871)->f1; goto _LL890;} else{ goto
_LL891;} _LL891: if(( unsigned int) _temp871 > 2u?*(( int*) _temp871) == Cyc_Absyn_Reference_p:
0){ _LL924: _temp923=(( struct Cyc_Absyn_Reference_p_struct*) _temp871)->f1;
goto _LL892;} else{ goto _LL893;} _LL893: if(( unsigned int) _temp871 > 2u?*((
int*) _temp871) == Cyc_Absyn_UnknownId_p: 0){ _LL926: _temp925=(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp871)->f1; goto _LL894;} else{ goto _LL895;} _LL895: if(( unsigned int)
_temp871 > 2u?*(( int*) _temp871) == Cyc_Absyn_UnknownCall_p: 0){ _LL932:
_temp931=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp871)->f1; goto _LL930;
_LL930: _temp929=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp871)->f2; goto
_LL928; _LL928: _temp927=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp871)->f3;
goto _LL896;} else{ goto _LL897;} _LL897: if(( unsigned int) _temp871 > 2u?*((
int*) _temp871) == Cyc_Absyn_UnknownFields_p: 0){ _LL938: _temp937=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp871)->f1; goto _LL936; _LL936: _temp935=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp871)->f2; goto _LL934; _LL934: _temp933=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp871)->f3; goto _LL898;} else{ goto _LL899;} _LL899: if(( unsigned int)
_temp871 > 2u?*(( int*) _temp871) == Cyc_Absyn_Struct_p: 0){ _LL946: _temp945=((
struct Cyc_Absyn_Struct_p_struct*) _temp871)->f1; goto _LL944; _LL944: _temp943=((
struct Cyc_Absyn_Struct_p_struct*) _temp871)->f2; goto _LL942; _LL942: _temp941=((
struct Cyc_Absyn_Struct_p_struct*) _temp871)->f3; goto _LL940; _LL940: _temp939=((
struct Cyc_Absyn_Struct_p_struct*) _temp871)->f4; goto _LL900;} else{ goto
_LL901;} _LL901: if(( unsigned int) _temp871 > 2u?*(( int*) _temp871) == Cyc_Absyn_Tunion_p:
0){ _LL954: _temp953=(( struct Cyc_Absyn_Tunion_p_struct*) _temp871)->f1; goto
_LL952; _LL952: _temp951=(( struct Cyc_Absyn_Tunion_p_struct*) _temp871)->f2;
goto _LL950; _LL950: _temp949=(( struct Cyc_Absyn_Tunion_p_struct*) _temp871)->f3;
goto _LL948; _LL948: _temp947=(( struct Cyc_Absyn_Tunion_p_struct*) _temp871)->f4;
goto _LL902;} else{ goto _LL903;} _LL903: if(( unsigned int) _temp871 > 2u?*((
int*) _temp871) == Cyc_Absyn_Enum_p: 0){ _LL958: _temp957=(( struct Cyc_Absyn_Enum_p_struct*)
_temp871)->f1; goto _LL956; _LL956: _temp955=(( struct Cyc_Absyn_Enum_p_struct*)
_temp871)->f2; goto _LL904;} else{ goto _LL872;} _LL874: Cyc_Absyndump_dump_char((
int)'_'); goto _LL872; _LL876: Cyc_Absyndump_dump( _tag_arr("null", sizeof(
unsigned char), 5u)); goto _LL872; _LL878: Cyc_Absyndump_dump(( struct
_tagged_arr) xprintf("%d", _temp905)); goto _LL872; _LL880: Cyc_Absyndump_dump((
struct _tagged_arr) xprintf("%u",( unsigned int) _temp909)); goto _LL872; _LL882:
Cyc_Absyndump_dump( _tag_arr("'", sizeof( unsigned char), 2u)); Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_char_escape( _temp913)); Cyc_Absyndump_dump_nospace( _tag_arr("'",
sizeof( unsigned char), 2u)); goto _LL872; _LL884: Cyc_Absyndump_dump( _temp915);
goto _LL872; _LL886: Cyc_Absyndump_dumpqvar( _temp917->name); goto _LL872;
_LL888:(( void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* l,
struct _tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat, _temp919, _tag_arr("$(", sizeof( unsigned char), 3u),
_tag_arr(")", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u)); goto _LL872; _LL890: Cyc_Absyndump_dump( _tag_arr("&", sizeof(
unsigned char), 2u)); Cyc_Absyndump_dumppat( _temp921); goto _LL872; _LL892: Cyc_Absyndump_dump(
_tag_arr("*", sizeof( unsigned char), 2u)); Cyc_Absyndump_dumpqvar( _temp923->name);
goto _LL872; _LL894: Cyc_Absyndump_dumpqvar( _temp925); goto _LL872; _LL896: Cyc_Absyndump_dumpqvar(
_temp931); Cyc_Absyndump_dumptvars( _temp929);(( void(*)( void(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr end,
struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp927,
_tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u)); goto _LL872; _LL898: Cyc_Absyndump_dumpqvar(
_temp937); Cyc_Absyndump_dumptvars( _temp935);(( void(*)( void(* f)( struct
_tuple9*), struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr
end, struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpdp,
_temp933, _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u)); goto _LL872;
_LL900: if( _temp945->name != 0){ Cyc_Absyndump_dumpqvar(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( _temp945->name))->v);} Cyc_Absyndump_dumptvars(
_temp941);(( void(*)( void(* f)( struct _tuple9*), struct Cyc_List_List* l,
struct _tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpdp, _temp939, _tag_arr("{", sizeof( unsigned char), 2u),
_tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u)); goto _LL872; _LL902: Cyc_Absyndump_dumpqvar( _temp951->name); Cyc_Absyndump_dumptvars(
_temp949); if( _temp947 != 0){(( void(*)( void(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr end,
struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp947,
_tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u));} goto _LL872; _LL904: Cyc_Absyndump_dumpqvar(
_temp955->name); goto _LL872; _LL872:;} void Cyc_Absyndump_dumptunionfield(
struct Cyc_Absyn_Tunionfield* ef){ Cyc_Absyndump_dumpqvar( ef->name); if( ef->typs
!= 0){ Cyc_Absyndump_dumpargs( ef->typs);}} void Cyc_Absyndump_dumptunionfields(
struct Cyc_List_List* fields){(( void(*)( void(* f)( struct Cyc_Absyn_Tunionfield*),
struct Cyc_List_List* l, struct _tagged_arr sep)) Cyc_Absyndump_dump_sep)( Cyc_Absyndump_dumptunionfield,
fields, _tag_arr(",", sizeof( unsigned char), 2u));} void Cyc_Absyndump_dumpenumfield(
struct Cyc_Absyn_Enumfield* ef){ Cyc_Absyndump_dumpqvar( ef->name); if( ef->tag
!= 0){ Cyc_Absyndump_dump( _tag_arr(" = ", sizeof( unsigned char), 4u)); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*) _check_null( ef->tag));}} void Cyc_Absyndump_dumpenumfields(
struct Cyc_List_List* fields){(( void(*)( void(* f)( struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List* l, struct _tagged_arr sep)) Cyc_Absyndump_dump_sep)( Cyc_Absyndump_dumpenumfield,
fields, _tag_arr(",", sizeof( unsigned char), 2u));} void Cyc_Absyndump_dumpstructfields(
struct Cyc_List_List* fields){ for( 0; fields != 0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ struct Cyc_Absyn_Structfield _temp961; struct Cyc_List_List*
_temp962; struct Cyc_Core_Opt* _temp964; void* _temp966; struct Cyc_Absyn_Tqual
_temp968; struct _tagged_arr* _temp970; struct Cyc_Absyn_Structfield* _temp959=(
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( fields))->hd;
_temp961=* _temp959; _LL971: _temp970= _temp961.name; goto _LL969; _LL969:
_temp968= _temp961.tq; goto _LL967; _LL967: _temp966=( void*) _temp961.type;
goto _LL965; _LL965: _temp964= _temp961.width; goto _LL963; _LL963: _temp962=
_temp961.attributes; goto _LL960; _LL960:(( void(*)( struct Cyc_Absyn_Tqual,
void*, void(* f)( struct _tagged_arr*), struct _tagged_arr*)) Cyc_Absyndump_dumptqtd)(
_temp968, _temp966, Cyc_Absyndump_dump_str, _temp970); Cyc_Absyndump_dumpatts(
_temp962); if( _temp964 != 0){ Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp964))->v);} Cyc_Absyndump_dump_semi();}}
void Cyc_Absyndump_dumptypedefname( struct Cyc_Absyn_Typedefdecl* td){ Cyc_Absyndump_dumpqvar(
td->name); Cyc_Absyndump_dumptvars( td->tvs);} static void Cyc_Absyndump_dump_atts_qvar(
struct Cyc_Absyn_Fndecl* fd){ Cyc_Absyndump_dumpatts( fd->attributes); Cyc_Absyndump_dumpqvar(
fd->name);} struct _tuple10{ void* f1; struct _tuple0* f2; } ; static void Cyc_Absyndump_dump_callconv_qvar(
struct _tuple10* pr){{ void* _temp972=(* pr).f1; _LL974: if( _temp972 ==( void*)
Cyc_Absyn_Unused_att){ goto _LL975;} else{ goto _LL976;} _LL976: if( _temp972 ==(
void*) Cyc_Absyn_Stdcall_att){ goto _LL977;} else{ goto _LL978;} _LL978: if(
_temp972 ==( void*) Cyc_Absyn_Cdecl_att){ goto _LL979;} else{ goto _LL980;}
_LL980: if( _temp972 ==( void*) Cyc_Absyn_Fastcall_att){ goto _LL981;} else{
goto _LL982;} _LL982: goto _LL983; _LL975: goto _LL973; _LL977: Cyc_Absyndump_dump(
_tag_arr("_stdcall", sizeof( unsigned char), 9u)); goto _LL973; _LL979: Cyc_Absyndump_dump(
_tag_arr("_cdecl", sizeof( unsigned char), 7u)); goto _LL973; _LL981: Cyc_Absyndump_dump(
_tag_arr("_fastcall", sizeof( unsigned char), 10u)); goto _LL973; _LL983: goto
_LL973; _LL973:;} Cyc_Absyndump_dumpqvar((* pr).f2);} static void Cyc_Absyndump_dump_callconv_fdqvar(
struct Cyc_Absyn_Fndecl* fd){ Cyc_Absyndump_dump_callconv( fd->attributes); Cyc_Absyndump_dumpqvar(
fd->name);} static void Cyc_Absyndump_dumpids( struct Cyc_List_List* vds){ for(
0; vds != 0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){ Cyc_Absyndump_dumpqvar(((
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( vds))->hd)->name);
if((( struct Cyc_List_List*) _check_null( vds))->tl != 0){ Cyc_Absyndump_dump_char((
int)',');}}} void Cyc_Absyndump_dumpdecl( struct Cyc_Absyn_Decl* d){ void*
_temp984=( void*) d->r; struct Cyc_Absyn_Fndecl* _temp1010; struct Cyc_Absyn_Structdecl*
_temp1012; struct Cyc_Absyn_Uniondecl* _temp1014; struct Cyc_Absyn_Vardecl*
_temp1016; struct Cyc_Absyn_Vardecl _temp1018; struct Cyc_List_List* _temp1019;
struct Cyc_Core_Opt* _temp1021; struct Cyc_Absyn_Exp* _temp1023; void* _temp1025;
struct Cyc_Absyn_Tqual _temp1027; struct _tuple0* _temp1029; void* _temp1031;
struct Cyc_Absyn_Tuniondecl* _temp1033; struct Cyc_Absyn_Tuniondecl _temp1035;
int _temp1036; struct Cyc_Core_Opt* _temp1038; struct Cyc_List_List* _temp1040;
struct _tuple0* _temp1042; void* _temp1044; struct Cyc_Absyn_Enumdecl* _temp1046;
struct Cyc_Absyn_Enumdecl _temp1048; struct Cyc_Core_Opt* _temp1049; struct
_tuple0* _temp1051; void* _temp1053; int _temp1055; struct Cyc_Absyn_Exp*
_temp1057; struct Cyc_Core_Opt* _temp1059; struct Cyc_Core_Opt* _temp1061;
struct Cyc_Absyn_Pat* _temp1063; struct Cyc_List_List* _temp1065; struct Cyc_Absyn_Typedefdecl*
_temp1067; struct Cyc_List_List* _temp1069; struct _tagged_arr* _temp1071;
struct Cyc_List_List* _temp1073; struct _tuple0* _temp1075; struct Cyc_List_List*
_temp1077; _LL986: if(*(( int*) _temp984) == Cyc_Absyn_Fn_d){ _LL1011: _temp1010=((
struct Cyc_Absyn_Fn_d_struct*) _temp984)->f1; goto _LL987;} else{ goto _LL988;}
_LL988: if(*(( int*) _temp984) == Cyc_Absyn_Struct_d){ _LL1013: _temp1012=((
struct Cyc_Absyn_Struct_d_struct*) _temp984)->f1; goto _LL989;} else{ goto
_LL990;} _LL990: if(*(( int*) _temp984) == Cyc_Absyn_Union_d){ _LL1015:
_temp1014=(( struct Cyc_Absyn_Union_d_struct*) _temp984)->f1; goto _LL991;}
else{ goto _LL992;} _LL992: if(*(( int*) _temp984) == Cyc_Absyn_Var_d){ _LL1017:
_temp1016=(( struct Cyc_Absyn_Var_d_struct*) _temp984)->f1; _temp1018=*
_temp1016; _LL1032: _temp1031=( void*) _temp1018.sc; goto _LL1030; _LL1030:
_temp1029= _temp1018.name; goto _LL1028; _LL1028: _temp1027= _temp1018.tq; goto
_LL1026; _LL1026: _temp1025=( void*) _temp1018.type; goto _LL1024; _LL1024:
_temp1023= _temp1018.initializer; goto _LL1022; _LL1022: _temp1021= _temp1018.rgn;
goto _LL1020; _LL1020: _temp1019= _temp1018.attributes; goto _LL993;} else{ goto
_LL994;} _LL994: if(*(( int*) _temp984) == Cyc_Absyn_Tunion_d){ _LL1034:
_temp1033=(( struct Cyc_Absyn_Tunion_d_struct*) _temp984)->f1; _temp1035=*
_temp1033; _LL1045: _temp1044=( void*) _temp1035.sc; goto _LL1043; _LL1043:
_temp1042= _temp1035.name; goto _LL1041; _LL1041: _temp1040= _temp1035.tvs; goto
_LL1039; _LL1039: _temp1038= _temp1035.fields; goto _LL1037; _LL1037: _temp1036=
_temp1035.is_xtunion; goto _LL995;} else{ goto _LL996;} _LL996: if(*(( int*)
_temp984) == Cyc_Absyn_Enum_d){ _LL1047: _temp1046=(( struct Cyc_Absyn_Enum_d_struct*)
_temp984)->f1; _temp1048=* _temp1046; _LL1054: _temp1053=( void*) _temp1048.sc;
goto _LL1052; _LL1052: _temp1051= _temp1048.name; goto _LL1050; _LL1050:
_temp1049= _temp1048.fields; goto _LL997;} else{ goto _LL998;} _LL998: if(*((
int*) _temp984) == Cyc_Absyn_Let_d){ _LL1064: _temp1063=(( struct Cyc_Absyn_Let_d_struct*)
_temp984)->f1; goto _LL1062; _LL1062: _temp1061=(( struct Cyc_Absyn_Let_d_struct*)
_temp984)->f2; goto _LL1060; _LL1060: _temp1059=(( struct Cyc_Absyn_Let_d_struct*)
_temp984)->f3; goto _LL1058; _LL1058: _temp1057=(( struct Cyc_Absyn_Let_d_struct*)
_temp984)->f4; goto _LL1056; _LL1056: _temp1055=(( struct Cyc_Absyn_Let_d_struct*)
_temp984)->f5; goto _LL999;} else{ goto _LL1000;} _LL1000: if(*(( int*) _temp984)
== Cyc_Absyn_Letv_d){ _LL1066: _temp1065=(( struct Cyc_Absyn_Letv_d_struct*)
_temp984)->f1; goto _LL1001;} else{ goto _LL1002;} _LL1002: if(*(( int*)
_temp984) == Cyc_Absyn_Typedef_d){ _LL1068: _temp1067=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp984)->f1; goto _LL1003;} else{ goto _LL1004;} _LL1004: if(*(( int*)
_temp984) == Cyc_Absyn_Namespace_d){ _LL1072: _temp1071=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp984)->f1; goto _LL1070; _LL1070: _temp1069=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp984)->f2; goto _LL1005;} else{ goto _LL1006;} _LL1006: if(*(( int*)
_temp984) == Cyc_Absyn_Using_d){ _LL1076: _temp1075=(( struct Cyc_Absyn_Using_d_struct*)
_temp984)->f1; goto _LL1074; _LL1074: _temp1073=(( struct Cyc_Absyn_Using_d_struct*)
_temp984)->f2; goto _LL1007;} else{ goto _LL1008;} _LL1008: if(*(( int*)
_temp984) == Cyc_Absyn_ExternC_d){ _LL1078: _temp1077=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp984)->f1; goto _LL1009;} else{ goto _LL985;} _LL987: if( Cyc_Absyndump_to_VC){
Cyc_Absyndump_dumpatts( _temp1010->attributes);} if( _temp1010->is_inline){ if(
Cyc_Absyndump_to_VC){ Cyc_Absyndump_dump( _tag_arr("__inline", sizeof(
unsigned char), 9u));} else{ Cyc_Absyndump_dump( _tag_arr("inline", sizeof(
unsigned char), 7u));}} Cyc_Absyndump_dumpscope(( void*) _temp1010->sc);{ void*
t=( void*)({ struct Cyc_Absyn_FnType_struct* _temp1080=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1080[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1081; _temp1081.tag= Cyc_Absyn_FnType; _temp1081.f1=({ struct Cyc_Absyn_FnInfo
_temp1082; _temp1082.tvars= _temp1010->tvs; _temp1082.effect= _temp1010->effect;
_temp1082.ret_typ=( void*)(( void*) _temp1010->ret_type); _temp1082.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp1010->args); _temp1082.c_varargs=
_temp1010->c_varargs; _temp1082.cyc_varargs= _temp1010->cyc_varargs; _temp1082.rgn_po=
_temp1010->rgn_po; _temp1082.attributes= 0; _temp1082;}); _temp1081;});
_temp1080;});(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct Cyc_Absyn_Fndecl*),
struct Cyc_Absyn_Fndecl*)) Cyc_Absyndump_dumptqtd)(({ struct Cyc_Absyn_Tqual
_temp1079; _temp1079.q_const= 0; _temp1079.q_volatile= 0; _temp1079.q_restrict=
0; _temp1079;}), t, Cyc_Absyndump_to_VC? Cyc_Absyndump_dump_callconv_fdqvar: Cyc_Absyndump_dump_atts_qvar,
_temp1010); Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt(
_temp1010->body); Cyc_Absyndump_dump_char(( int)'}'); goto _LL985;} _LL989: Cyc_Absyndump_dumpscope((
void*) _temp1012->sc); Cyc_Absyndump_dump( _tag_arr("struct", sizeof(
unsigned char), 7u)); if( _temp1012->name != 0){ Cyc_Absyndump_dumpqvar(( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp1012->name))->v);} Cyc_Absyndump_dumptvars(
_temp1012->tvs); if( _temp1012->fields == 0){ Cyc_Absyndump_dump_semi();} else{
Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstructfields(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1012->fields))->v); Cyc_Absyndump_dump(
_tag_arr("}", sizeof( unsigned char), 2u)); Cyc_Absyndump_dumpatts( _temp1012->attributes);
Cyc_Absyndump_dump( _tag_arr(";", sizeof( unsigned char), 2u));} goto _LL985;
_LL991: Cyc_Absyndump_dumpscope(( void*) _temp1014->sc); Cyc_Absyndump_dump(
_tag_arr("union", sizeof( unsigned char), 6u)); if( _temp1014->name != 0){ Cyc_Absyndump_dumpqvar((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp1014->name))->v);}
Cyc_Absyndump_dumptvars( _temp1014->tvs); if( _temp1014->fields == 0){ Cyc_Absyndump_dump_semi();}
else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstructfields((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1014->fields))->v);
Cyc_Absyndump_dump( _tag_arr("}", sizeof( unsigned char), 2u)); Cyc_Absyndump_dumpatts(
_temp1014->attributes); Cyc_Absyndump_dump( _tag_arr(";", sizeof( unsigned char),
2u));} goto _LL985; _LL993: if( Cyc_Absyndump_to_VC){ Cyc_Absyndump_dumpatts(
_temp1019); Cyc_Absyndump_dumpscope( _temp1031);{ struct Cyc_List_List*
_temp1085; void* _temp1087; struct Cyc_Absyn_Tqual _temp1089; struct _tuple4
_temp1083= Cyc_Absynpp_to_tms( _temp1027, _temp1025); _LL1090: _temp1089=
_temp1083.f1; goto _LL1088; _LL1088: _temp1087= _temp1083.f2; goto _LL1086;
_LL1086: _temp1085= _temp1083.f3; goto _LL1084; _LL1084: Cyc_Absyndump_dumptq(
_temp1089);{ void* call_conv=( void*) Cyc_Absyn_Unused_att;{ struct Cyc_List_List*
tms2= _temp1085; for( 0; tms2 != 0; tms2=(( struct Cyc_List_List*) _check_null(
tms2))->tl){ void* _temp1091=( void*)(( struct Cyc_List_List*) _check_null( tms2))->hd;
struct Cyc_List_List* _temp1097; struct Cyc_Position_Segment* _temp1099; _LL1093:
if(( unsigned int) _temp1091 > 1u?*(( int*) _temp1091) == Cyc_Absyn_Attributes_mod:
0){ _LL1100: _temp1099=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp1091)->f1;
goto _LL1098; _LL1098: _temp1097=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp1091)->f2; goto _LL1094;} else{ goto _LL1095;} _LL1095: goto _LL1096;
_LL1094: for( 0; _temp1097 != 0; _temp1097=(( struct Cyc_List_List*) _check_null(
_temp1097))->tl){ void* _temp1101=( void*)(( struct Cyc_List_List*) _check_null(
_temp1097))->hd; _LL1103: if( _temp1101 ==( void*) Cyc_Absyn_Stdcall_att){ goto
_LL1104;} else{ goto _LL1105;} _LL1105: if( _temp1101 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL1106;} else{ goto _LL1107;} _LL1107: if( _temp1101 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL1108;} else{ goto _LL1109;} _LL1109: goto _LL1110; _LL1104: call_conv=(
void*) Cyc_Absyn_Stdcall_att; goto _LL1102; _LL1106: call_conv=( void*) Cyc_Absyn_Cdecl_att;
goto _LL1102; _LL1108: call_conv=( void*) Cyc_Absyn_Fastcall_att; goto _LL1102;
_LL1110: goto _LL1102; _LL1102:;} goto _LL1092; _LL1096: goto _LL1092; _LL1092:;}}
Cyc_Absyndump_dumptq( _temp1089); Cyc_Absyndump_dumpntyp( _temp1087);{ struct
_tuple10 _temp1112=({ struct _tuple10 _temp1111; _temp1111.f1= call_conv;
_temp1111.f2= _temp1029; _temp1111;}); goto _LL1113; _LL1113:(( void(*)( struct
Cyc_List_List* tms, void(* f)( struct _tuple10*), struct _tuple10* a)) Cyc_Absyndump_dumptms)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1085),
Cyc_Absyndump_dump_callconv_qvar,& _temp1112);}}}} else{ Cyc_Absyndump_dumpscope(
_temp1031);(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct _tuple0*),
struct _tuple0*)) Cyc_Absyndump_dumptqtd)( _temp1027, _temp1025, Cyc_Absyndump_dumpqvar,
_temp1029); Cyc_Absyndump_dumpatts( _temp1019);} if( _temp1023 != 0){ Cyc_Absyndump_dump_char((
int)'='); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null( _temp1023));}
Cyc_Absyndump_dump_semi(); goto _LL985; _LL995: Cyc_Absyndump_dumpscope(
_temp1044); if( _temp1036){ Cyc_Absyndump_dump( _tag_arr("xtunion ", sizeof(
unsigned char), 9u));} else{ Cyc_Absyndump_dump( _tag_arr("tunion ", sizeof(
unsigned char), 8u));} Cyc_Absyndump_dumpqvar( _temp1042); Cyc_Absyndump_dumptvars(
_temp1040); if( _temp1038 == 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumptunionfields(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1038))->v); Cyc_Absyndump_dump_nospace( _tag_arr("};", sizeof(
unsigned char), 3u));} goto _LL985; _LL997: Cyc_Absyndump_dumpscope( _temp1053);
Cyc_Absyndump_dump( _tag_arr("enum ", sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpqvar(
_temp1051); if( _temp1049 == 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpenumfields(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1049))->v); Cyc_Absyndump_dump_nospace( _tag_arr("};", sizeof(
unsigned char), 3u));} return; _LL999: Cyc_Absyndump_dump( _tag_arr("let",
sizeof( unsigned char), 4u)); Cyc_Absyndump_dumppat( _temp1063); Cyc_Absyndump_dump_char((
int)'='); Cyc_Absyndump_dumpexp( _temp1057); Cyc_Absyndump_dump_semi(); goto
_LL985; _LL1001: Cyc_Absyndump_dump( _tag_arr("let ", sizeof( unsigned char), 5u));
Cyc_Absyndump_dumpids( _temp1065); Cyc_Absyndump_dump_semi(); goto _LL985;
_LL1003: if( ! Cyc_Absyndump_expand_typedefs){ Cyc_Absyndump_dump( _tag_arr("typedef",
sizeof( unsigned char), 8u));(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)(
struct Cyc_Absyn_Typedefdecl*), struct Cyc_Absyn_Typedefdecl*)) Cyc_Absyndump_dumptqtd)(({
struct Cyc_Absyn_Tqual _temp1114; _temp1114.q_const= 0; _temp1114.q_volatile= 0;
_temp1114.q_restrict= 0; _temp1114;}),( void*) _temp1067->defn, Cyc_Absyndump_dumptypedefname,
_temp1067); Cyc_Absyndump_dump_semi();} goto _LL985; _LL1005: Cyc_Absyndump_dump(
_tag_arr("namespace", sizeof( unsigned char), 10u)); Cyc_Absyndump_dump_str(
_temp1071); Cyc_Absyndump_dump_char(( int)'{'); for( 0; _temp1069 != 0;
_temp1069=(( struct Cyc_List_List*) _check_null( _temp1069))->tl){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( _temp1069))->hd);}
Cyc_Absyndump_dump_char(( int)'}'); goto _LL985; _LL1007: Cyc_Absyndump_dump(
_tag_arr("using", sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpqvar(
_temp1075); Cyc_Absyndump_dump_char(( int)'{'); for( 0; _temp1073 != 0;
_temp1073=(( struct Cyc_List_List*) _check_null( _temp1073))->tl){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( _temp1073))->hd);}
Cyc_Absyndump_dump_char(( int)'}'); goto _LL985; _LL1009: Cyc_Absyndump_dump(
_tag_arr("extern \"C\" {", sizeof( unsigned char), 13u)); for( 0; _temp1077 != 0;
_temp1077=(( struct Cyc_List_List*) _check_null( _temp1077))->tl){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( _temp1077))->hd);}
Cyc_Absyndump_dump_char(( int)'}'); goto _LL985; _LL985:;} static void Cyc_Absyndump_dump_upperbound(
struct Cyc_Absyn_Exp* e){ unsigned int i= Cyc_Evexp_eval_const_uint_exp( e); if(
i != 1){ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpexp( e); Cyc_Absyndump_dump_char((
int)'}');}} void Cyc_Absyndump_dumptms( struct Cyc_List_List* tms, void(* f)(
void*), void* a){ if( tms == 0){ f( a); return;}{ void* _temp1115=( void*)((
struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Tqual _temp1133;
void* _temp1135; void* _temp1137; struct Cyc_Absyn_Exp* _temp1139; struct Cyc_Absyn_Tqual
_temp1141; void* _temp1143; void* _temp1145; struct Cyc_Absyn_Exp* _temp1147;
struct Cyc_Absyn_Tqual _temp1149; void* _temp1151; void* _temp1153; struct Cyc_Absyn_Tqual
_temp1155; void* _temp1157; struct Cyc_Absyn_Tvar* _temp1159; void* _temp1161;
struct Cyc_Absyn_Exp* _temp1163; struct Cyc_Absyn_Tqual _temp1165; void*
_temp1167; struct Cyc_Absyn_Tvar* _temp1169; void* _temp1171; struct Cyc_Absyn_Exp*
_temp1173; struct Cyc_Absyn_Tqual _temp1175; void* _temp1177; struct Cyc_Absyn_Tvar*
_temp1179; void* _temp1181; struct Cyc_Absyn_Tqual _temp1183; void* _temp1185;
void* _temp1187; _LL1117: if(( unsigned int) _temp1115 > 1u?*(( int*) _temp1115)
== Cyc_Absyn_Pointer_mod: 0){ _LL1138: _temp1137=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1115)->f1; if(( unsigned int) _temp1137 > 1u?*(( int*) _temp1137) == Cyc_Absyn_Nullable_ps:
0){ _LL1140: _temp1139=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp1137)->f1;
goto _LL1136;} else{ goto _LL1119;} _LL1136: _temp1135=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1115)->f2; if( _temp1135 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1134;}
else{ goto _LL1119;} _LL1134: _temp1133=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1115)->f3; goto _LL1118;} else{ goto _LL1119;} _LL1119: if(( unsigned int)
_temp1115 > 1u?*(( int*) _temp1115) == Cyc_Absyn_Pointer_mod: 0){ _LL1146:
_temp1145=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1115)->f1; if((
unsigned int) _temp1145 > 1u?*(( int*) _temp1145) == Cyc_Absyn_NonNullable_ps: 0){
_LL1148: _temp1147=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1145)->f1;
goto _LL1144;} else{ goto _LL1121;} _LL1144: _temp1143=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1115)->f2; if( _temp1143 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1142;}
else{ goto _LL1121;} _LL1142: _temp1141=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1115)->f3; goto _LL1120;} else{ goto _LL1121;} _LL1121: if(( unsigned int)
_temp1115 > 1u?*(( int*) _temp1115) == Cyc_Absyn_Pointer_mod: 0){ _LL1154:
_temp1153=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1115)->f1; if(
_temp1153 ==( void*) Cyc_Absyn_TaggedArray_ps){ goto _LL1152;} else{ goto
_LL1123;} _LL1152: _temp1151=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1115)->f2; if( _temp1151 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1150;}
else{ goto _LL1123;} _LL1150: _temp1149=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1115)->f3; goto _LL1122;} else{ goto _LL1123;} _LL1123: if(( unsigned int)
_temp1115 > 1u?*(( int*) _temp1115) == Cyc_Absyn_Pointer_mod: 0){ _LL1162:
_temp1161=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1115)->f1; if((
unsigned int) _temp1161 > 1u?*(( int*) _temp1161) == Cyc_Absyn_Nullable_ps: 0){
_LL1164: _temp1163=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp1161)->f1; goto
_LL1158;} else{ goto _LL1125;} _LL1158: _temp1157=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1115)->f2; if(( unsigned int) _temp1157 > 4u?*(( int*) _temp1157) == Cyc_Absyn_VarType:
0){ _LL1160: _temp1159=(( struct Cyc_Absyn_VarType_struct*) _temp1157)->f1; goto
_LL1156;} else{ goto _LL1125;} _LL1156: _temp1155=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1115)->f3; goto _LL1124;} else{ goto _LL1125;} _LL1125: if(( unsigned int)
_temp1115 > 1u?*(( int*) _temp1115) == Cyc_Absyn_Pointer_mod: 0){ _LL1172:
_temp1171=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1115)->f1; if((
unsigned int) _temp1171 > 1u?*(( int*) _temp1171) == Cyc_Absyn_NonNullable_ps: 0){
_LL1174: _temp1173=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1171)->f1;
goto _LL1168;} else{ goto _LL1127;} _LL1168: _temp1167=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1115)->f2; if(( unsigned int) _temp1167 > 4u?*(( int*) _temp1167) == Cyc_Absyn_VarType:
0){ _LL1170: _temp1169=(( struct Cyc_Absyn_VarType_struct*) _temp1167)->f1; goto
_LL1166;} else{ goto _LL1127;} _LL1166: _temp1165=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1115)->f3; goto _LL1126;} else{ goto _LL1127;} _LL1127: if(( unsigned int)
_temp1115 > 1u?*(( int*) _temp1115) == Cyc_Absyn_Pointer_mod: 0){ _LL1182:
_temp1181=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1115)->f1; if(
_temp1181 ==( void*) Cyc_Absyn_TaggedArray_ps){ goto _LL1178;} else{ goto
_LL1129;} _LL1178: _temp1177=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1115)->f2; if(( unsigned int) _temp1177 > 4u?*(( int*) _temp1177) == Cyc_Absyn_VarType:
0){ _LL1180: _temp1179=(( struct Cyc_Absyn_VarType_struct*) _temp1177)->f1; goto
_LL1176;} else{ goto _LL1129;} _LL1176: _temp1175=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1115)->f3; goto _LL1128;} else{ goto _LL1129;} _LL1129: if(( unsigned int)
_temp1115 > 1u?*(( int*) _temp1115) == Cyc_Absyn_Pointer_mod: 0){ _LL1188:
_temp1187=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1115)->f1; goto
_LL1186; _LL1186: _temp1185=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1115)->f2; goto _LL1184; _LL1184: _temp1183=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1115)->f3; goto _LL1130;} else{ goto _LL1131;} _LL1131: goto _LL1132;
_LL1118: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dump_upperbound(
_temp1139);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f, a);
return; _LL1120: Cyc_Absyndump_dump_char(( int)'@'); Cyc_Absyndump_dump_upperbound(
_temp1147);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f, a);
return; _LL1122: Cyc_Absyndump_dump_char(( int)'?');(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*)
_check_null( tms))->tl, f, a); return; _LL1124: Cyc_Absyndump_dump_char(( int)'*');
Cyc_Absyndump_dump_upperbound( _temp1163); Cyc_Absyndump_dump_str( _temp1159->name);((
void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)(((
struct Cyc_List_List*) _check_null( tms))->tl, f, a); return; _LL1126: Cyc_Absyndump_dump_char((
int)'@'); Cyc_Absyndump_dump_upperbound( _temp1173); Cyc_Absyndump_dump_str(
_temp1169->name);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void*
a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f,
a); return; _LL1128: Cyc_Absyndump_dump_char(( int)'?'); Cyc_Absyndump_dump_str(
_temp1179->name);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void*
a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f,
a); return; _LL1130:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1189=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1189[ 0]=({ struct Cyc_Core_Impossible_struct _temp1190; _temp1190.tag= Cyc_Core_Impossible;
_temp1190.f1= _tag_arr("dumptms: bad Pointer_mod", sizeof( unsigned char), 25u);
_temp1190;}); _temp1189;})); _LL1132: { int next_is_pointer= 0; if((( struct Cyc_List_List*)
_check_null( tms))->tl != 0){ void* _temp1191=( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->hd; struct Cyc_Absyn_Tqual
_temp1197; void* _temp1199; void* _temp1201; _LL1193: if(( unsigned int)
_temp1191 > 1u?*(( int*) _temp1191) == Cyc_Absyn_Pointer_mod: 0){ _LL1202:
_temp1201=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1191)->f1; goto
_LL1200; _LL1200: _temp1199=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1191)->f2; goto _LL1198; _LL1198: _temp1197=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1191)->f3; goto _LL1194;} else{ goto _LL1195;} _LL1195: goto _LL1196;
_LL1194: next_is_pointer= 1; goto _LL1192; _LL1196: goto _LL1192; _LL1192:;} if(
next_is_pointer){ Cyc_Absyndump_dump_char(( int)'(');}(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*)
_check_null( tms))->tl, f, a); if( next_is_pointer){ Cyc_Absyndump_dump_char((
int)')');}{ void* _temp1203=( void*)(( struct Cyc_List_List*) _check_null( tms))->hd;
struct Cyc_Absyn_Exp* _temp1219; void* _temp1221; struct Cyc_List_List*
_temp1223; struct Cyc_Core_Opt* _temp1225; struct Cyc_Absyn_VarargInfo*
_temp1227; int _temp1229; struct Cyc_List_List* _temp1231; void* _temp1233;
struct Cyc_Position_Segment* _temp1235; struct Cyc_List_List* _temp1237; int
_temp1239; struct Cyc_Position_Segment* _temp1241; struct Cyc_List_List*
_temp1243; struct Cyc_List_List* _temp1245; struct Cyc_Position_Segment*
_temp1247; struct Cyc_Absyn_Tqual _temp1249; void* _temp1251; void* _temp1253;
_LL1205: if( _temp1203 ==( void*) Cyc_Absyn_Carray_mod){ goto _LL1206;} else{
goto _LL1207;} _LL1207: if(( unsigned int) _temp1203 > 1u?*(( int*) _temp1203)
== Cyc_Absyn_ConstArray_mod: 0){ _LL1220: _temp1219=(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp1203)->f1; goto _LL1208;} else{ goto _LL1209;} _LL1209: if(( unsigned int)
_temp1203 > 1u?*(( int*) _temp1203) == Cyc_Absyn_Function_mod: 0){ _LL1222:
_temp1221=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp1203)->f1; if(*((
int*) _temp1221) == Cyc_Absyn_WithTypes){ _LL1232: _temp1231=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1221)->f1; goto _LL1230; _LL1230: _temp1229=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1221)->f2; goto _LL1228; _LL1228: _temp1227=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1221)->f3; goto _LL1226; _LL1226: _temp1225=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1221)->f4; goto _LL1224; _LL1224: _temp1223=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1221)->f5; goto _LL1210;} else{ goto _LL1211;}} else{ goto _LL1211;}
_LL1211: if(( unsigned int) _temp1203 > 1u?*(( int*) _temp1203) == Cyc_Absyn_Function_mod:
0){ _LL1234: _temp1233=( void*)(( struct Cyc_Absyn_Function_mod_struct*)
_temp1203)->f1; if(*(( int*) _temp1233) == Cyc_Absyn_NoTypes){ _LL1238:
_temp1237=(( struct Cyc_Absyn_NoTypes_struct*) _temp1233)->f1; goto _LL1236;
_LL1236: _temp1235=(( struct Cyc_Absyn_NoTypes_struct*) _temp1233)->f2; goto
_LL1212;} else{ goto _LL1213;}} else{ goto _LL1213;} _LL1213: if(( unsigned int)
_temp1203 > 1u?*(( int*) _temp1203) == Cyc_Absyn_TypeParams_mod: 0){ _LL1244:
_temp1243=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp1203)->f1; goto
_LL1242; _LL1242: _temp1241=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1203)->f2; goto _LL1240; _LL1240: _temp1239=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1203)->f3; goto _LL1214;} else{ goto _LL1215;} _LL1215: if(( unsigned int)
_temp1203 > 1u?*(( int*) _temp1203) == Cyc_Absyn_Attributes_mod: 0){ _LL1248:
_temp1247=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp1203)->f1; goto
_LL1246; _LL1246: _temp1245=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp1203)->f2; goto _LL1216;} else{ goto _LL1217;} _LL1217: if(( unsigned int)
_temp1203 > 1u?*(( int*) _temp1203) == Cyc_Absyn_Pointer_mod: 0){ _LL1254:
_temp1253=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1203)->f1; goto
_LL1252; _LL1252: _temp1251=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1203)->f2; goto _LL1250; _LL1250: _temp1249=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1203)->f3; goto _LL1218;} else{ goto _LL1204;} _LL1206: Cyc_Absyndump_dump(
_tag_arr("[]", sizeof( unsigned char), 3u)); goto _LL1204; _LL1208: Cyc_Absyndump_dump_char((
int)'['); Cyc_Absyndump_dumpexp( _temp1219); Cyc_Absyndump_dump_char(( int)']');
goto _LL1204; _LL1210: Cyc_Absyndump_dumpfunargs( _temp1231, _temp1229,
_temp1227, _temp1225, _temp1223); goto _LL1204; _LL1212:(( void(*)( void(* f)(
struct _tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr start, struct
_tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dump_str,
_temp1237, _tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u)); goto _LL1204;
_LL1214: if( _temp1239){ Cyc_Absyndump_dumpkindedtvars( _temp1243);} else{ Cyc_Absyndump_dumptvars(
_temp1243);} goto _LL1204; _LL1216: Cyc_Absyndump_dumpatts( _temp1245); goto
_LL1204; _LL1218:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1255=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1255[ 0]=({ struct Cyc_Core_Impossible_struct _temp1256; _temp1256.tag= Cyc_Core_Impossible;
_temp1256.f1= _tag_arr("dumptms", sizeof( unsigned char), 8u); _temp1256;});
_temp1255;})); _LL1204:;} return;} _LL1116:;}} void Cyc_Absyndump_dumptqtd(
struct Cyc_Absyn_Tqual tq, void* t, void(* f)( void*), void* a){ struct Cyc_List_List*
_temp1259; void* _temp1261; struct Cyc_Absyn_Tqual _temp1263; struct _tuple4
_temp1257= Cyc_Absynpp_to_tms( tq, t); _LL1264: _temp1263= _temp1257.f1; goto
_LL1262; _LL1262: _temp1261= _temp1257.f2; goto _LL1260; _LL1260: _temp1259=
_temp1257.f3; goto _LL1258; _LL1258: Cyc_Absyndump_dumptq( _temp1263); Cyc_Absyndump_dumpntyp(
_temp1261);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)((( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( _temp1259), f, a);} void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE* f){ Cyc_Absyndump_pos= 0;*
Cyc_Absyndump_dump_file= f; for( 0; tdl != 0; tdl=(( struct Cyc_List_List*)
_check_null( tdl))->tl){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( tdl))->hd);}}