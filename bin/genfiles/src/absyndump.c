 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
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
wrap_Cstring_as_string( unsigned char*, int); extern struct _tagged_arr
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
Cyc_List_List* _temp0= 0;{ void* _temp1=(* v).f1; struct Cyc_List_List* _temp9;
struct Cyc_List_List* _temp11; _LL3: if( _temp1 ==( void*) Cyc_Absyn_Loc_n){
goto _LL4;} else{ goto _LL5;} _LL5: if(( unsigned int) _temp1 > 1u?*(( int*)
_temp1) == Cyc_Absyn_Rel_n: 0){ _LL10: _temp9=(( struct Cyc_Absyn_Rel_n_struct*)
_temp1)->f1; goto _LL6;} else{ goto _LL7;} _LL7: if(( unsigned int) _temp1 > 1u?*((
int*) _temp1) == Cyc_Absyn_Abs_n: 0){ _LL12: _temp11=(( struct Cyc_Absyn_Abs_n_struct*)
_temp1)->f1; goto _LL8;} else{ goto _LL2;} _LL4: _temp9= 0; goto _LL6; _LL6:
_temp0= _temp9; goto _LL2; _LL8: _temp0=( Cyc_Absyndump_qvar_to_Cids? Cyc_Absyndump_add_cyc_prefix:
0)?({ struct Cyc_List_List* _temp13=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp13->hd=( void*) Cyc_Absynpp_cyc_stringptr; _temp13->tl=
_temp11; _temp13;}): _temp11; goto _LL2; _LL2:;} if( _temp0 != 0){ Cyc_Absyndump_dump_str((
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
_temp14= sc; _LL16: if( _temp14 ==( void*) Cyc_Absyn_Static){ goto _LL17;} else{
goto _LL18;} _LL18: if( _temp14 ==( void*) Cyc_Absyn_Public){ goto _LL19;} else{
goto _LL20;} _LL20: if( _temp14 ==( void*) Cyc_Absyn_Extern){ goto _LL21;} else{
goto _LL22;} _LL22: if( _temp14 ==( void*) Cyc_Absyn_ExternC){ goto _LL23;}
else{ goto _LL24;} _LL24: if( _temp14 ==( void*) Cyc_Absyn_Abstract){ goto _LL25;}
else{ goto _LL15;} _LL17: Cyc_Absyndump_dump( _tag_arr("static", sizeof(
unsigned char), 7u)); return; _LL19: return; _LL21: Cyc_Absyndump_dump( _tag_arr("extern",
sizeof( unsigned char), 7u)); return; _LL23: Cyc_Absyndump_dump( _tag_arr("extern \"C\"",
sizeof( unsigned char), 11u)); return; _LL25: Cyc_Absyndump_dump( _tag_arr("abstract",
sizeof( unsigned char), 9u)); return; _LL15:;} void Cyc_Absyndump_dumpkind( void*
k){ void* _temp26= k; _LL28: if( _temp26 ==( void*) Cyc_Absyn_AnyKind){ goto
_LL29;} else{ goto _LL30;} _LL30: if( _temp26 ==( void*) Cyc_Absyn_MemKind){
goto _LL31;} else{ goto _LL32;} _LL32: if( _temp26 ==( void*) Cyc_Absyn_BoxKind){
goto _LL33;} else{ goto _LL34;} _LL34: if( _temp26 ==( void*) Cyc_Absyn_RgnKind){
goto _LL35;} else{ goto _LL36;} _LL36: if( _temp26 ==( void*) Cyc_Absyn_EffKind){
goto _LL37;} else{ goto _LL27;} _LL29: Cyc_Absyndump_dump( _tag_arr("A", sizeof(
unsigned char), 2u)); return; _LL31: Cyc_Absyndump_dump( _tag_arr("M", sizeof(
unsigned char), 2u)); return; _LL33: Cyc_Absyndump_dump( _tag_arr("B", sizeof(
unsigned char), 2u)); return; _LL35: Cyc_Absyndump_dump( _tag_arr("R", sizeof(
unsigned char), 2u)); return; _LL37: Cyc_Absyndump_dump( _tag_arr("E", sizeof(
unsigned char), 2u)); return; _LL27:;} void Cyc_Absyndump_dumptps( struct Cyc_List_List*
ts){(( void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_arr
start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumptyp, ts, _tag_arr("<", sizeof( unsigned char), 2u), _tag_arr(">",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u));} void
Cyc_Absyndump_dumptvar( struct Cyc_Absyn_Tvar* tv){ Cyc_Absyndump_dump_str( tv->name);}
void Cyc_Absyndump_dumpkindedtvar( struct Cyc_Absyn_Tvar* tv){ Cyc_Absyndump_dump_str(
tv->name);{ void* _temp38=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( tv->kind))->v; void* _temp46; void* _temp48;
_LL40: if(( unsigned int) _temp38 > 1u?*(( int*) _temp38) == Cyc_Absyn_Eq_constr:
0){ _LL47: _temp46=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp38)->f1;
if( _temp46 ==( void*) Cyc_Absyn_BoxKind){ goto _LL41;} else{ goto _LL42;}}
else{ goto _LL42;} _LL42: if(( unsigned int) _temp38 > 1u?*(( int*) _temp38) ==
Cyc_Absyn_Eq_constr: 0){ _LL49: _temp48=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp38)->f1; goto _LL43;} else{ goto _LL44;} _LL44: goto _LL45; _LL41: goto
_LL39; _LL43: Cyc_Absyndump_dump( _tag_arr("::", sizeof( unsigned char), 3u));
Cyc_Absyndump_dumpkind( _temp48); goto _LL39; _LL45: Cyc_Absyndump_dump(
_tag_arr("::?", sizeof( unsigned char), 4u)); goto _LL39; _LL39:;}} void Cyc_Absyndump_dumptvars(
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
_check_null( atts))->tl){ void* _temp50=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; _LL52: if( _temp50 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL53;} else{ goto _LL54;} _LL54: if( _temp50 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL55;} else{ goto _LL56;} _LL56: if( _temp50 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL57;} else{ goto _LL58;} _LL58: goto _LL59; _LL53: Cyc_Absyndump_dump(
_tag_arr("_stdcall", sizeof( unsigned char), 9u)); return; _LL55: Cyc_Absyndump_dump(
_tag_arr("_cdecl", sizeof( unsigned char), 7u)); return; _LL57: Cyc_Absyndump_dump(
_tag_arr("_fastcall", sizeof( unsigned char), 10u)); return; _LL59: goto _LL51;
_LL51:;}} void Cyc_Absyndump_dump_noncallconv( struct Cyc_List_List* atts){ int
hasatt= 0;{ struct Cyc_List_List* atts2= atts; for( 0; atts2 != 0; atts2=((
struct Cyc_List_List*) _check_null( atts2))->tl){ void* _temp60=( void*)((
struct Cyc_List_List*) _check_null( atts))->hd; _LL62: if( _temp60 ==( void*)
Cyc_Absyn_Stdcall_att){ goto _LL63;} else{ goto _LL64;} _LL64: if( _temp60 ==(
void*) Cyc_Absyn_Cdecl_att){ goto _LL65;} else{ goto _LL66;} _LL66: if( _temp60
==( void*) Cyc_Absyn_Fastcall_att){ goto _LL67;} else{ goto _LL68;} _LL68: goto
_LL69; _LL63: goto _LL61; _LL65: goto _LL61; _LL67: goto _LL61; _LL69: hasatt= 1;
goto _LL61; _LL61:;}} if( ! hasatt){ return;} Cyc_Absyndump_dump( _tag_arr("__declspec(",
sizeof( unsigned char), 12u)); for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp70=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; _LL72: if( _temp70 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL73;} else{ goto _LL74;} _LL74: if( _temp70 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL75;} else{ goto _LL76;} _LL76: if( _temp70 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL77;} else{ goto _LL78;} _LL78: goto _LL79; _LL73: goto _LL71; _LL75:
goto _LL71; _LL77: goto _LL71; _LL79: Cyc_Absyndump_dump( Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd)); goto _LL71; _LL71:;}
Cyc_Absyndump_dump_char(( int)')');} void Cyc_Absyndump_dumpatts( struct Cyc_List_List*
atts){ if( atts == 0){ return;} if( Cyc_Absyndump_to_VC){ Cyc_Absyndump_dump_noncallconv(
atts); return;} Cyc_Absyndump_dump( _tag_arr(" __attribute__((", sizeof(
unsigned char), 17u)); for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ Cyc_Absyndump_dump( Cyc_Absyn_attribute2string(( void*)((
struct Cyc_List_List*) _check_null( atts))->hd)); if((( struct Cyc_List_List*)
_check_null( atts))->tl != 0){ Cyc_Absyndump_dump( _tag_arr(",", sizeof(
unsigned char), 2u));}} Cyc_Absyndump_dump( _tag_arr(")) ", sizeof(
unsigned char), 4u));} int Cyc_Absyndump_next_is_pointer( struct Cyc_List_List*
tms){ if( tms == 0){ return 0;}{ void* _temp80=( void*)(( struct Cyc_List_List*)
_check_null( tms))->hd; struct Cyc_Absyn_Tqual _temp86; void* _temp88; void*
_temp90; _LL82: if(( unsigned int) _temp80 > 1u?*(( int*) _temp80) == Cyc_Absyn_Pointer_mod:
0){ _LL91: _temp90=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp80)->f1;
goto _LL89; _LL89: _temp88=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp80)->f2; goto _LL87; _LL87: _temp86=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp80)->f3; goto _LL83;} else{ goto _LL84;} _LL84: goto _LL85; _LL83: return 1;
_LL85: return 0; _LL81:;}} static void Cyc_Absyndump_dumprgn( void* t){ void*
_temp92= Cyc_Tcutil_compress( t); _LL94: if( _temp92 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL95;} else{ goto _LL96;} _LL96: goto _LL97; _LL95: Cyc_Absyndump_dump(
_tag_arr("`H", sizeof( unsigned char), 3u)); goto _LL93; _LL97: Cyc_Absyndump_dumpntyp(
t); goto _LL93; _LL93:;} struct _tuple6{ struct Cyc_List_List* f1; struct Cyc_List_List*
f2; } ; static struct _tuple6 Cyc_Absyndump_effects_split( void* t){ struct Cyc_List_List*
regions= 0; struct Cyc_List_List* effects= 0;{ void* _temp98= Cyc_Tcutil_compress(
t); void* _temp106; struct Cyc_List_List* _temp108; _LL100: if(( unsigned int)
_temp98 > 4u?*(( int*) _temp98) == Cyc_Absyn_AccessEff: 0){ _LL107: _temp106=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp98)->f1; goto _LL101;} else{
goto _LL102;} _LL102: if(( unsigned int) _temp98 > 4u?*(( int*) _temp98) == Cyc_Absyn_JoinEff:
0){ _LL109: _temp108=(( struct Cyc_Absyn_JoinEff_struct*) _temp98)->f1; goto
_LL103;} else{ goto _LL104;} _LL104: goto _LL105; _LL101: regions=({ struct Cyc_List_List*
_temp110=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp110->hd=( void*) _temp106; _temp110->tl= regions; _temp110;}); goto _LL99;
_LL103: for( 0; _temp108 != 0; _temp108=(( struct Cyc_List_List*) _check_null(
_temp108))->tl){ struct Cyc_List_List* _temp113; struct Cyc_List_List* _temp115;
struct _tuple6 _temp111= Cyc_Absyndump_effects_split(( void*)(( struct Cyc_List_List*)
_check_null( _temp108))->hd); _LL116: _temp115= _temp111.f1; goto _LL114; _LL114:
_temp113= _temp111.f2; goto _LL112; _LL112: regions=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp115, regions); effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( _temp113, effects);} goto _LL99;
_LL105: effects=({ struct Cyc_List_List* _temp117=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp117->hd=( void*) t; _temp117->tl=
effects; _temp117;}); goto _LL99; _LL99:;} return({ struct _tuple6 _temp118;
_temp118.f1= regions; _temp118.f2= effects; _temp118;});} static void Cyc_Absyndump_dumpeff(
void* t){ struct Cyc_List_List* _temp121; struct Cyc_List_List* _temp123; struct
_tuple6 _temp119= Cyc_Absyndump_effects_split( t); _LL124: _temp123= _temp119.f1;
goto _LL122; _LL122: _temp121= _temp119.f2; goto _LL120; _LL120: _temp123=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp123);
_temp121=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp121); for( 0; _temp121 != 0; _temp121=(( struct Cyc_List_List*) _check_null(
_temp121))->tl){ Cyc_Absyndump_dumpntyp(( void*)(( struct Cyc_List_List*)
_check_null( _temp121))->hd); Cyc_Absyndump_dump_char(( int)'+');} Cyc_Absyndump_dump_char((
int)'{'); for( 0; _temp123 != 0; _temp123=(( struct Cyc_List_List*) _check_null(
_temp123))->tl){ Cyc_Absyndump_dumprgn(( void*)(( struct Cyc_List_List*)
_check_null( _temp123))->hd); if((( struct Cyc_List_List*) _check_null( _temp123))->tl
!= 0){ Cyc_Absyndump_dump_char(( int)',');}} Cyc_Absyndump_dump_char(( int)'}');}
void Cyc_Absyndump_dumpntyp( void* t){ void* _temp125= t; struct Cyc_Absyn_Exp*
_temp195; struct Cyc_Absyn_Tqual _temp197; void* _temp199; struct Cyc_Absyn_FnInfo
_temp201; struct Cyc_Absyn_PtrInfo _temp203; struct Cyc_Absyn_Tvar* _temp205;
struct Cyc_Core_Opt* _temp207; int _temp209; struct Cyc_Core_Opt* _temp211;
struct Cyc_Core_Opt* _temp213; struct Cyc_Core_Opt* _temp215; int _temp217;
struct Cyc_Core_Opt* _temp219; struct Cyc_Core_Opt _temp221; void* _temp222;
struct Cyc_Core_Opt* _temp224; struct Cyc_Absyn_TunionInfo _temp226; void*
_temp228; struct Cyc_List_List* _temp230; void* _temp232; struct Cyc_Absyn_TunionFieldInfo
_temp234; struct Cyc_List_List* _temp236; void* _temp238; struct Cyc_Absyn_Enumdecl*
_temp240; struct _tuple0* _temp242; void* _temp244; void* _temp246; void*
_temp248; void* _temp250; void* _temp252; void* _temp254; void* _temp256; void*
_temp258; void* _temp260; void* _temp262; void* _temp264; void* _temp266; void*
_temp269; void* _temp271; void* _temp273; void* _temp275; void* _temp278; void*
_temp280; void* _temp282; void* _temp284; struct Cyc_List_List* _temp286; struct
Cyc_Absyn_Structdecl** _temp288; struct Cyc_List_List* _temp290; struct _tuple0*
_temp292; struct Cyc_Absyn_Structdecl** _temp294; struct Cyc_List_List* _temp296;
struct _tuple0* _temp298; struct Cyc_Absyn_Uniondecl** _temp300; struct Cyc_List_List*
_temp302; struct _tuple0* _temp304; struct Cyc_Absyn_Uniondecl** _temp306;
struct Cyc_List_List* _temp308; struct _tuple0* _temp310; struct Cyc_List_List*
_temp312; struct Cyc_List_List* _temp314; struct Cyc_Core_Opt* _temp316; struct
Cyc_List_List* _temp318; struct _tuple0* _temp320; void* _temp322; void*
_temp324; struct Cyc_List_List* _temp326; _LL127: if(( unsigned int) _temp125 >
4u?*(( int*) _temp125) == Cyc_Absyn_ArrayType: 0){ _LL200: _temp199=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp125)->f1; goto _LL198; _LL198: _temp197=((
struct Cyc_Absyn_ArrayType_struct*) _temp125)->f2; goto _LL196; _LL196: _temp195=((
struct Cyc_Absyn_ArrayType_struct*) _temp125)->f3; goto _LL128;} else{ goto
_LL129;} _LL129: if(( unsigned int) _temp125 > 4u?*(( int*) _temp125) == Cyc_Absyn_FnType:
0){ _LL202: _temp201=(( struct Cyc_Absyn_FnType_struct*) _temp125)->f1; goto
_LL130;} else{ goto _LL131;} _LL131: if(( unsigned int) _temp125 > 4u?*(( int*)
_temp125) == Cyc_Absyn_PointerType: 0){ _LL204: _temp203=(( struct Cyc_Absyn_PointerType_struct*)
_temp125)->f1; goto _LL132;} else{ goto _LL133;} _LL133: if( _temp125 ==( void*)
Cyc_Absyn_VoidType){ goto _LL134;} else{ goto _LL135;} _LL135: if(( unsigned int)
_temp125 > 4u?*(( int*) _temp125) == Cyc_Absyn_VarType: 0){ _LL206: _temp205=((
struct Cyc_Absyn_VarType_struct*) _temp125)->f1; goto _LL136;} else{ goto _LL137;}
_LL137: if(( unsigned int) _temp125 > 4u?*(( int*) _temp125) == Cyc_Absyn_Evar:
0){ _LL214: _temp213=(( struct Cyc_Absyn_Evar_struct*) _temp125)->f1; goto
_LL212; _LL212: _temp211=(( struct Cyc_Absyn_Evar_struct*) _temp125)->f2; if(
_temp211 == 0){ goto _LL210;} else{ goto _LL139;} _LL210: _temp209=(( struct Cyc_Absyn_Evar_struct*)
_temp125)->f3; goto _LL208; _LL208: _temp207=(( struct Cyc_Absyn_Evar_struct*)
_temp125)->f4; goto _LL138;} else{ goto _LL139;} _LL139: if(( unsigned int)
_temp125 > 4u?*(( int*) _temp125) == Cyc_Absyn_Evar: 0){ _LL225: _temp224=((
struct Cyc_Absyn_Evar_struct*) _temp125)->f1; goto _LL220; _LL220: _temp219=((
struct Cyc_Absyn_Evar_struct*) _temp125)->f2; if( _temp219 == 0){ goto _LL141;}
else{ _temp221=* _temp219; _LL223: _temp222=( void*) _temp221.v; goto _LL218;}
_LL218: _temp217=(( struct Cyc_Absyn_Evar_struct*) _temp125)->f3; goto _LL216;
_LL216: _temp215=(( struct Cyc_Absyn_Evar_struct*) _temp125)->f4; goto _LL140;}
else{ goto _LL141;} _LL141: if(( unsigned int) _temp125 > 4u?*(( int*) _temp125)
== Cyc_Absyn_TunionType: 0){ _LL227: _temp226=(( struct Cyc_Absyn_TunionType_struct*)
_temp125)->f1; _LL233: _temp232=( void*) _temp226.tunion_info; goto _LL231;
_LL231: _temp230= _temp226.targs; goto _LL229; _LL229: _temp228=( void*)
_temp226.rgn; goto _LL142;} else{ goto _LL143;} _LL143: if(( unsigned int)
_temp125 > 4u?*(( int*) _temp125) == Cyc_Absyn_TunionFieldType: 0){ _LL235:
_temp234=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp125)->f1; _LL239:
_temp238=( void*) _temp234.field_info; goto _LL237; _LL237: _temp236= _temp234.targs;
goto _LL144;} else{ goto _LL145;} _LL145: if(( unsigned int) _temp125 > 4u?*((
int*) _temp125) == Cyc_Absyn_EnumType: 0){ _LL243: _temp242=(( struct Cyc_Absyn_EnumType_struct*)
_temp125)->f1; goto _LL241; _LL241: _temp240=(( struct Cyc_Absyn_EnumType_struct*)
_temp125)->f2; goto _LL146;} else{ goto _LL147;} _LL147: if(( unsigned int)
_temp125 > 4u?*(( int*) _temp125) == Cyc_Absyn_IntType: 0){ _LL247: _temp246=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp125)->f1; if( _temp246 ==( void*)
Cyc_Absyn_Signed){ goto _LL245;} else{ goto _LL149;} _LL245: _temp244=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp125)->f2; if( _temp244 ==( void*) Cyc_Absyn_B4){
goto _LL148;} else{ goto _LL149;}} else{ goto _LL149;} _LL149: if(( unsigned int)
_temp125 > 4u?*(( int*) _temp125) == Cyc_Absyn_IntType: 0){ _LL251: _temp250=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp125)->f1; if( _temp250 ==( void*)
Cyc_Absyn_Signed){ goto _LL249;} else{ goto _LL151;} _LL249: _temp248=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp125)->f2; if( _temp248 ==( void*) Cyc_Absyn_B1){
goto _LL150;} else{ goto _LL151;}} else{ goto _LL151;} _LL151: if(( unsigned int)
_temp125 > 4u?*(( int*) _temp125) == Cyc_Absyn_IntType: 0){ _LL255: _temp254=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp125)->f1; if( _temp254 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL253;} else{ goto _LL153;} _LL253: _temp252=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp125)->f2; if( _temp252 ==( void*) Cyc_Absyn_B1){
goto _LL152;} else{ goto _LL153;}} else{ goto _LL153;} _LL153: if(( unsigned int)
_temp125 > 4u?*(( int*) _temp125) == Cyc_Absyn_IntType: 0){ _LL259: _temp258=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp125)->f1; if( _temp258 ==( void*)
Cyc_Absyn_Signed){ goto _LL257;} else{ goto _LL155;} _LL257: _temp256=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp125)->f2; if( _temp256 ==( void*) Cyc_Absyn_B2){
goto _LL154;} else{ goto _LL155;}} else{ goto _LL155;} _LL155: if(( unsigned int)
_temp125 > 4u?*(( int*) _temp125) == Cyc_Absyn_IntType: 0){ _LL263: _temp262=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp125)->f1; if( _temp262 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL261;} else{ goto _LL157;} _LL261: _temp260=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp125)->f2; if( _temp260 ==( void*) Cyc_Absyn_B2){
goto _LL156;} else{ goto _LL157;}} else{ goto _LL157;} _LL157: if(( unsigned int)
_temp125 > 4u?*(( int*) _temp125) == Cyc_Absyn_IntType: 0){ _LL267: _temp266=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp125)->f1; if( _temp266 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL265;} else{ goto _LL159;} _LL265: _temp264=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp125)->f2; if( _temp264 ==( void*) Cyc_Absyn_B4){
goto _LL158;} else{ goto _LL159;}} else{ goto _LL159;} _LL159: if(( unsigned int)
_temp125 > 4u?*(( int*) _temp125) == Cyc_Absyn_IntType: 0){ _LL272: _temp271=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp125)->f1; if( _temp271 ==( void*)
Cyc_Absyn_Signed){ goto _LL270;} else{ goto _LL161;} _LL270: _temp269=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp125)->f2; if( _temp269 ==( void*) Cyc_Absyn_B8){
goto _LL268;} else{ goto _LL161;}} else{ goto _LL161;} _LL268: if( Cyc_Absyndump_to_VC){
goto _LL160;} else{ goto _LL161;} _LL161: if(( unsigned int) _temp125 > 4u?*((
int*) _temp125) == Cyc_Absyn_IntType: 0){ _LL276: _temp275=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp125)->f1; if( _temp275 ==( void*) Cyc_Absyn_Signed){ goto _LL274;} else{
goto _LL163;} _LL274: _temp273=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp125)->f2; if( _temp273 ==( void*) Cyc_Absyn_B8){ goto _LL162;} else{ goto
_LL163;}} else{ goto _LL163;} _LL163: if(( unsigned int) _temp125 > 4u?*(( int*)
_temp125) == Cyc_Absyn_IntType: 0){ _LL281: _temp280=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp125)->f1; if( _temp280 ==( void*) Cyc_Absyn_Unsigned){ goto _LL279;} else{
goto _LL165;} _LL279: _temp278=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp125)->f2; if( _temp278 ==( void*) Cyc_Absyn_B8){ goto _LL277;} else{ goto
_LL165;}} else{ goto _LL165;} _LL277: if( Cyc_Absyndump_to_VC){ goto _LL164;}
else{ goto _LL165;} _LL165: if(( unsigned int) _temp125 > 4u?*(( int*) _temp125)
== Cyc_Absyn_IntType: 0){ _LL285: _temp284=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp125)->f1; if( _temp284 ==( void*) Cyc_Absyn_Unsigned){ goto _LL283;} else{
goto _LL167;} _LL283: _temp282=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp125)->f2; if( _temp282 ==( void*) Cyc_Absyn_B8){ goto _LL166;} else{ goto
_LL167;}} else{ goto _LL167;} _LL167: if( _temp125 ==( void*) Cyc_Absyn_FloatType){
goto _LL168;} else{ goto _LL169;} _LL169: if( _temp125 ==( void*) Cyc_Absyn_DoubleType){
goto _LL170;} else{ goto _LL171;} _LL171: if(( unsigned int) _temp125 > 4u?*((
int*) _temp125) == Cyc_Absyn_TupleType: 0){ _LL287: _temp286=(( struct Cyc_Absyn_TupleType_struct*)
_temp125)->f1; goto _LL172;} else{ goto _LL173;} _LL173: if(( unsigned int)
_temp125 > 4u?*(( int*) _temp125) == Cyc_Absyn_StructType: 0){ _LL293: _temp292=((
struct Cyc_Absyn_StructType_struct*) _temp125)->f1; if( _temp292 == 0){ goto
_LL291;} else{ goto _LL175;} _LL291: _temp290=(( struct Cyc_Absyn_StructType_struct*)
_temp125)->f2; goto _LL289; _LL289: _temp288=(( struct Cyc_Absyn_StructType_struct*)
_temp125)->f3; goto _LL174;} else{ goto _LL175;} _LL175: if(( unsigned int)
_temp125 > 4u?*(( int*) _temp125) == Cyc_Absyn_StructType: 0){ _LL299: _temp298=((
struct Cyc_Absyn_StructType_struct*) _temp125)->f1; goto _LL297; _LL297:
_temp296=(( struct Cyc_Absyn_StructType_struct*) _temp125)->f2; goto _LL295;
_LL295: _temp294=(( struct Cyc_Absyn_StructType_struct*) _temp125)->f3; goto
_LL176;} else{ goto _LL177;} _LL177: if(( unsigned int) _temp125 > 4u?*(( int*)
_temp125) == Cyc_Absyn_UnionType: 0){ _LL305: _temp304=(( struct Cyc_Absyn_UnionType_struct*)
_temp125)->f1; if( _temp304 == 0){ goto _LL303;} else{ goto _LL179;} _LL303:
_temp302=(( struct Cyc_Absyn_UnionType_struct*) _temp125)->f2; goto _LL301;
_LL301: _temp300=(( struct Cyc_Absyn_UnionType_struct*) _temp125)->f3; goto
_LL178;} else{ goto _LL179;} _LL179: if(( unsigned int) _temp125 > 4u?*(( int*)
_temp125) == Cyc_Absyn_UnionType: 0){ _LL311: _temp310=(( struct Cyc_Absyn_UnionType_struct*)
_temp125)->f1; goto _LL309; _LL309: _temp308=(( struct Cyc_Absyn_UnionType_struct*)
_temp125)->f2; goto _LL307; _LL307: _temp306=(( struct Cyc_Absyn_UnionType_struct*)
_temp125)->f3; goto _LL180;} else{ goto _LL181;} _LL181: if(( unsigned int)
_temp125 > 4u?*(( int*) _temp125) == Cyc_Absyn_AnonStructType: 0){ _LL313:
_temp312=(( struct Cyc_Absyn_AnonStructType_struct*) _temp125)->f1; goto _LL182;}
else{ goto _LL183;} _LL183: if(( unsigned int) _temp125 > 4u?*(( int*) _temp125)
== Cyc_Absyn_AnonUnionType: 0){ _LL315: _temp314=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp125)->f1; goto _LL184;} else{ goto _LL185;} _LL185: if(( unsigned int)
_temp125 > 4u?*(( int*) _temp125) == Cyc_Absyn_TypedefType: 0){ _LL321: _temp320=((
struct Cyc_Absyn_TypedefType_struct*) _temp125)->f1; goto _LL319; _LL319:
_temp318=(( struct Cyc_Absyn_TypedefType_struct*) _temp125)->f2; goto _LL317;
_LL317: _temp316=(( struct Cyc_Absyn_TypedefType_struct*) _temp125)->f3; goto
_LL186;} else{ goto _LL187;} _LL187: if(( unsigned int) _temp125 > 4u?*(( int*)
_temp125) == Cyc_Absyn_RgnHandleType: 0){ _LL323: _temp322=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp125)->f1; goto _LL188;} else{ goto _LL189;} _LL189: if( _temp125 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL190;} else{ goto _LL191;} _LL191: if(( unsigned int)
_temp125 > 4u?*(( int*) _temp125) == Cyc_Absyn_AccessEff: 0){ _LL325: _temp324=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp125)->f1; goto _LL192;} else{
goto _LL193;} _LL193: if(( unsigned int) _temp125 > 4u?*(( int*) _temp125) ==
Cyc_Absyn_JoinEff: 0){ _LL327: _temp326=(( struct Cyc_Absyn_JoinEff_struct*)
_temp125)->f1; goto _LL194;} else{ goto _LL126;} _LL128: return; _LL130: return;
_LL132: return; _LL134: Cyc_Absyndump_dump( _tag_arr("void", sizeof(
unsigned char), 5u)); return; _LL136: Cyc_Absyndump_dump_str( _temp205->name);
return; _LL138: Cyc_Absyndump_dump( _tag_arr("%", sizeof( unsigned char), 2u));
if( _temp213 == 0){ Cyc_Absyndump_dump( _tag_arr("?", sizeof( unsigned char), 2u));}
else{ Cyc_Absyndump_dumpkind(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp213))->v);} Cyc_Absyndump_dump(( struct _tagged_arr) xprintf("(%d)",
_temp209)); return; _LL140: Cyc_Absyndump_dumpntyp( _temp222); return; _LL142:{
void* _temp328= _temp232; struct Cyc_Absyn_UnknownTunionInfo _temp334; int
_temp336; struct _tuple0* _temp338; struct Cyc_Absyn_Tuniondecl* _temp340;
struct Cyc_Absyn_Tuniondecl _temp342; int _temp343; struct Cyc_Core_Opt*
_temp345; struct Cyc_List_List* _temp347; struct _tuple0* _temp349; void*
_temp351; _LL330: if(*(( int*) _temp328) == Cyc_Absyn_UnknownTunion){ _LL335:
_temp334=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp328)->f1; _LL339:
_temp338= _temp334.name; goto _LL337; _LL337: _temp336= _temp334.is_xtunion;
goto _LL331;} else{ goto _LL332;} _LL332: if(*(( int*) _temp328) == Cyc_Absyn_KnownTunion){
_LL341: _temp340=(( struct Cyc_Absyn_KnownTunion_struct*) _temp328)->f1;
_temp342=* _temp340; _LL352: _temp351=( void*) _temp342.sc; goto _LL350; _LL350:
_temp349= _temp342.name; goto _LL348; _LL348: _temp347= _temp342.tvs; goto
_LL346; _LL346: _temp345= _temp342.fields; goto _LL344; _LL344: _temp343=
_temp342.is_xtunion; goto _LL333;} else{ goto _LL329;} _LL331: _temp349=
_temp338; _temp343= _temp336; goto _LL333; _LL333: if( _temp343){ Cyc_Absyndump_dump(
_tag_arr("xtunion ", sizeof( unsigned char), 9u));} else{ Cyc_Absyndump_dump(
_tag_arr("tunion ", sizeof( unsigned char), 8u));}{ void* _temp353= Cyc_Tcutil_compress(
_temp228); _LL355: if( _temp353 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL356;}
else{ goto _LL357;} _LL357: goto _LL358; _LL356: goto _LL354; _LL358: Cyc_Absyndump_dumptyp(
_temp228); Cyc_Absyndump_dump( _tag_arr(" ", sizeof( unsigned char), 2u)); goto
_LL354; _LL354:;} Cyc_Absyndump_dumpqvar( _temp349); Cyc_Absyndump_dumptps(
_temp230); goto _LL329; _LL329:;} goto _LL126; _LL144:{ void* _temp359= _temp238;
struct Cyc_Absyn_UnknownTunionFieldInfo _temp365; int _temp367; struct _tuple0*
_temp369; struct _tuple0* _temp371; struct Cyc_Absyn_Tunionfield* _temp373;
struct Cyc_Absyn_Tunionfield _temp375; void* _temp376; struct Cyc_Position_Segment*
_temp378; struct Cyc_List_List* _temp380; struct Cyc_List_List* _temp382; struct
_tuple0* _temp384; struct Cyc_Absyn_Tuniondecl* _temp386; struct Cyc_Absyn_Tuniondecl
_temp388; int _temp389; struct Cyc_Core_Opt* _temp391; struct Cyc_List_List*
_temp393; struct _tuple0* _temp395; void* _temp397; _LL361: if(*(( int*)
_temp359) == Cyc_Absyn_UnknownTunionfield){ _LL366: _temp365=(( struct Cyc_Absyn_UnknownTunionfield_struct*)
_temp359)->f1; _LL372: _temp371= _temp365.tunion_name; goto _LL370; _LL370:
_temp369= _temp365.field_name; goto _LL368; _LL368: _temp367= _temp365.is_xtunion;
goto _LL362;} else{ goto _LL363;} _LL363: if(*(( int*) _temp359) == Cyc_Absyn_KnownTunionfield){
_LL387: _temp386=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp359)->f1;
_temp388=* _temp386; _LL398: _temp397=( void*) _temp388.sc; goto _LL396; _LL396:
_temp395= _temp388.name; goto _LL394; _LL394: _temp393= _temp388.tvs; goto
_LL392; _LL392: _temp391= _temp388.fields; goto _LL390; _LL390: _temp389=
_temp388.is_xtunion; goto _LL374; _LL374: _temp373=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp359)->f2; _temp375=* _temp373; _LL385: _temp384= _temp375.name; goto _LL383;
_LL383: _temp382= _temp375.tvs; goto _LL381; _LL381: _temp380= _temp375.typs;
goto _LL379; _LL379: _temp378= _temp375.loc; goto _LL377; _LL377: _temp376=(
void*) _temp375.sc; goto _LL364;} else{ goto _LL360;} _LL362: _temp395= _temp371;
_temp389= _temp367; _temp384= _temp369; goto _LL364; _LL364: if( _temp389){ Cyc_Absyndump_dump(
_tag_arr("xtunion ", sizeof( unsigned char), 9u));} else{ Cyc_Absyndump_dump(
_tag_arr("tunion ", sizeof( unsigned char), 8u));} Cyc_Absyndump_dumpqvar(
_temp395); Cyc_Absyndump_dump( _tag_arr(".", sizeof( unsigned char), 2u)); Cyc_Absyndump_dumpqvar(
_temp384); Cyc_Absyndump_dumptps( _temp236); goto _LL360; _LL360:;} goto _LL126;
_LL146: Cyc_Absyndump_dump( _tag_arr("enum ", sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpqvar(
_temp242); return; _LL148: Cyc_Absyndump_dump( _tag_arr("int", sizeof(
unsigned char), 4u)); return; _LL150: Cyc_Absyndump_dump( _tag_arr("signed char",
sizeof( unsigned char), 12u)); return; _LL152: Cyc_Absyndump_dump( _tag_arr("unsigned char",
sizeof( unsigned char), 14u)); return; _LL154: Cyc_Absyndump_dump( _tag_arr("short",
sizeof( unsigned char), 6u)); return; _LL156: Cyc_Absyndump_dump( _tag_arr("unsigned short",
sizeof( unsigned char), 15u)); return; _LL158: Cyc_Absyndump_dump( _tag_arr("unsigned int",
sizeof( unsigned char), 13u)); return; _LL160: Cyc_Absyndump_dump( _tag_arr("__int64",
sizeof( unsigned char), 8u)); return; _LL162: Cyc_Absyndump_dump( _tag_arr("long long",
sizeof( unsigned char), 10u)); return; _LL164: Cyc_Absyndump_dump( _tag_arr("unsigned __int64",
sizeof( unsigned char), 17u)); return; _LL166: Cyc_Absyndump_dump( _tag_arr("unsigned long long",
sizeof( unsigned char), 19u)); return; _LL168: Cyc_Absyndump_dump( _tag_arr("float",
sizeof( unsigned char), 6u)); return; _LL170: Cyc_Absyndump_dump( _tag_arr("double",
sizeof( unsigned char), 7u)); return; _LL172: Cyc_Absyndump_dump_char(( int)'$');
Cyc_Absyndump_dumpargs( _temp286); return; _LL174: Cyc_Absyndump_dump( _tag_arr("struct",
sizeof( unsigned char), 7u)); Cyc_Absyndump_dumptps( _temp290); return; _LL176:
Cyc_Absyndump_dump( _tag_arr("struct", sizeof( unsigned char), 7u)); Cyc_Absyndump_dumpqvar((
struct _tuple0*) _check_null( _temp298)); Cyc_Absyndump_dumptps( _temp296);
return; _LL178: Cyc_Absyndump_dump( _tag_arr("union", sizeof( unsigned char), 6u));
Cyc_Absyndump_dumptps( _temp302); return; _LL180: Cyc_Absyndump_dump( _tag_arr("union",
sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpqvar(( struct _tuple0*)
_check_null( _temp310)); Cyc_Absyndump_dumptps( _temp308); return; _LL182: Cyc_Absyndump_dump(
_tag_arr("struct {", sizeof( unsigned char), 9u)); Cyc_Absyndump_dumpstructfields(
_temp312); Cyc_Absyndump_dump( _tag_arr("}", sizeof( unsigned char), 2u));
return; _LL184: Cyc_Absyndump_dump( _tag_arr("union {", sizeof( unsigned char),
8u)); Cyc_Absyndump_dumpstructfields( _temp314); Cyc_Absyndump_dump( _tag_arr("}",
sizeof( unsigned char), 2u)); return; _LL186:( Cyc_Absyndump_dumpqvar( _temp320),
Cyc_Absyndump_dumptps( _temp318)); return; _LL188: Cyc_Absyndump_dumprgn(
_temp322); return; _LL190: return; _LL192: return; _LL194: return; _LL126:;}
void Cyc_Absyndump_dumpvaropt( struct Cyc_Core_Opt* vo){ if( vo != 0){ Cyc_Absyndump_dump_str((
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( vo))->v);}} void Cyc_Absyndump_dumpfunarg(
struct _tuple1* t){(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct
Cyc_Core_Opt*), struct Cyc_Core_Opt*)) Cyc_Absyndump_dumptqtd)((* t).f2,(* t).f3,
Cyc_Absyndump_dumpvaropt,(* t).f1);} struct _tuple7{ void* f1; void* f2; } ;
void Cyc_Absyndump_dump_rgncmp( struct _tuple7* cmp){ struct _tuple7 _temp401;
void* _temp402; void* _temp404; struct _tuple7* _temp399= cmp; _temp401=*
_temp399; _LL405: _temp404= _temp401.f1; goto _LL403; _LL403: _temp402= _temp401.f2;
goto _LL400; _LL400: Cyc_Absyndump_dumptyp( _temp404); Cyc_Absyndump_dump_char((
int)'<'); Cyc_Absyndump_dumptyp( _temp402);} void Cyc_Absyndump_dump_rgnpo(
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
else{ if( cyc_varargs != 0){ struct _tuple1* _temp406=({ struct _tuple1*
_temp407=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp407->f1=((
struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->name; _temp407->f2=((
struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->tq; _temp407->f3=(
void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->type;
_temp407;}); Cyc_Absyndump_dump( _tag_arr("...", sizeof( unsigned char), 4u));
Cyc_Absyndump_dumptyp(( void*)(( struct Cyc_Absyn_VarargInfo*) _check_null(
cyc_varargs))->rgn); if((( struct Cyc_Absyn_VarargInfo*) _check_null(
cyc_varargs))->inject){ Cyc_Absyndump_dump( _tag_arr(" inject ", sizeof(
unsigned char), 9u));} Cyc_Absyndump_dumpfunarg( _temp406);}} if( effopt != 0){
Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpeff(( void*)(( struct Cyc_Core_Opt*)
_check_null( effopt))->v);} if( rgn_po != 0){ Cyc_Absyndump_dump_char(( int)':');
Cyc_Absyndump_dump_rgnpo( rgn_po);} Cyc_Absyndump_dump_char(( int)')');} void
Cyc_Absyndump_dumptyp( void* t){(( void(*)( struct Cyc_Absyn_Tqual, void*, void(*
f)( int), int)) Cyc_Absyndump_dumptqtd)(({ struct Cyc_Absyn_Tqual _temp408;
_temp408.q_const= 0; _temp408.q_volatile= 0; _temp408.q_restrict= 0; _temp408;}),
t,( void(*)( int x)) Cyc_Absyndump_ignore, 0);} void Cyc_Absyndump_dumpdesignator(
void* d){ void* _temp409= d; struct Cyc_Absyn_Exp* _temp415; struct _tagged_arr*
_temp417; _LL411: if(*(( int*) _temp409) == Cyc_Absyn_ArrayElement){ _LL416:
_temp415=(( struct Cyc_Absyn_ArrayElement_struct*) _temp409)->f1; goto _LL412;}
else{ goto _LL413;} _LL413: if(*(( int*) _temp409) == Cyc_Absyn_FieldName){
_LL418: _temp417=(( struct Cyc_Absyn_FieldName_struct*) _temp409)->f1; goto
_LL414;} else{ goto _LL410;} _LL412: Cyc_Absyndump_dump( _tag_arr(".[", sizeof(
unsigned char), 3u)); Cyc_Absyndump_dumpexp( _temp415); Cyc_Absyndump_dump_char((
int)']'); goto _LL410; _LL414: Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp417); goto _LL410; _LL410:;} struct _tuple8{ struct Cyc_List_List* f1;
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
void* _temp419=( void*) e->r; void* _temp513; unsigned char _temp515; void*
_temp517; void* _temp519; short _temp521; void* _temp523; void* _temp525; int
_temp527; void* _temp529; void* _temp531; int _temp533; void* _temp535; void*
_temp537; long long _temp539; void* _temp541; void* _temp543; struct _tagged_arr
_temp545; void* _temp547; void* _temp549; struct _tagged_arr _temp551; struct
_tuple0* _temp553; void* _temp555; struct _tuple0* _temp557; struct Cyc_List_List*
_temp559; void* _temp561; struct Cyc_Absyn_Exp* _temp563; struct Cyc_Core_Opt*
_temp565; struct Cyc_Absyn_Exp* _temp567; void* _temp569; struct Cyc_Absyn_Exp*
_temp571; void* _temp573; struct Cyc_Absyn_Exp* _temp575; void* _temp577; struct
Cyc_Absyn_Exp* _temp579; void* _temp581; struct Cyc_Absyn_Exp* _temp583; struct
Cyc_Absyn_Exp* _temp585; struct Cyc_Absyn_Exp* _temp587; struct Cyc_Absyn_Exp*
_temp589; struct Cyc_Absyn_Exp* _temp591; struct Cyc_Absyn_Exp* _temp593; struct
Cyc_List_List* _temp595; struct Cyc_Absyn_Exp* _temp597; struct Cyc_Absyn_VarargCallInfo*
_temp599; struct Cyc_List_List* _temp601; struct Cyc_Absyn_Exp* _temp603; struct
Cyc_Absyn_Exp* _temp605; struct Cyc_Absyn_Exp* _temp607; struct Cyc_List_List*
_temp609; struct Cyc_Absyn_Exp* _temp611; struct Cyc_Absyn_Exp* _temp613; void*
_temp615; struct Cyc_Absyn_Exp* _temp617; struct Cyc_Absyn_Exp* _temp619; struct
Cyc_Absyn_Exp* _temp621; void* _temp623; struct Cyc_Absyn_Exp* _temp625; struct
_tagged_arr* _temp627; void* _temp629; struct Cyc_Absyn_Exp* _temp631; struct
_tagged_arr* _temp633; struct Cyc_Absyn_Exp* _temp635; struct _tagged_arr*
_temp637; struct Cyc_Absyn_Exp* _temp639; struct Cyc_Absyn_Exp* _temp641; struct
Cyc_Absyn_Exp* _temp643; struct Cyc_List_List* _temp645; struct Cyc_List_List*
_temp647; struct _tuple1* _temp649; struct Cyc_List_List* _temp651; struct Cyc_Absyn_Exp*
_temp653; struct Cyc_Absyn_Exp* _temp655; struct Cyc_Absyn_Vardecl* _temp657;
struct Cyc_Absyn_Structdecl* _temp659; struct Cyc_List_List* _temp661; struct
Cyc_Core_Opt* _temp663; struct _tuple0* _temp665; struct Cyc_List_List* _temp667;
void* _temp669; struct Cyc_Absyn_Tunionfield* _temp671; struct Cyc_Absyn_Tuniondecl*
_temp673; struct Cyc_List_List* _temp675; struct Cyc_Core_Opt* _temp677; struct
Cyc_Core_Opt* _temp679; struct Cyc_Absyn_Enumfield* _temp681; struct Cyc_Absyn_Enumdecl*
_temp683; struct _tuple0* _temp685; void* _temp687; struct Cyc_Absyn_Exp*
_temp689; struct Cyc_List_List* _temp691; struct Cyc_Core_Opt* _temp693; struct
Cyc_Absyn_Stmt* _temp695; struct Cyc_Absyn_Fndecl* _temp697; struct Cyc_Absyn_Exp*
_temp699; _LL421: if(*(( int*) _temp419) == Cyc_Absyn_Const_e){ _LL514: _temp513=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp419)->f1; if(( unsigned int)
_temp513 > 1u?*(( int*) _temp513) == Cyc_Absyn_Char_c: 0){ _LL518: _temp517=(
void*)(( struct Cyc_Absyn_Char_c_struct*) _temp513)->f1; goto _LL516; _LL516:
_temp515=(( struct Cyc_Absyn_Char_c_struct*) _temp513)->f2; goto _LL422;} else{
goto _LL423;}} else{ goto _LL423;} _LL423: if(*(( int*) _temp419) == Cyc_Absyn_Const_e){
_LL520: _temp519=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp419)->f1; if((
unsigned int) _temp519 > 1u?*(( int*) _temp519) == Cyc_Absyn_Short_c: 0){ _LL524:
_temp523=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp519)->f1; goto _LL522;
_LL522: _temp521=(( struct Cyc_Absyn_Short_c_struct*) _temp519)->f2; goto _LL424;}
else{ goto _LL425;}} else{ goto _LL425;} _LL425: if(*(( int*) _temp419) == Cyc_Absyn_Const_e){
_LL526: _temp525=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp419)->f1; if((
unsigned int) _temp525 > 1u?*(( int*) _temp525) == Cyc_Absyn_Int_c: 0){ _LL530:
_temp529=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp525)->f1; if( _temp529
==( void*) Cyc_Absyn_Signed){ goto _LL528;} else{ goto _LL427;} _LL528: _temp527=((
struct Cyc_Absyn_Int_c_struct*) _temp525)->f2; goto _LL426;} else{ goto _LL427;}}
else{ goto _LL427;} _LL427: if(*(( int*) _temp419) == Cyc_Absyn_Const_e){ _LL532:
_temp531=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp419)->f1; if((
unsigned int) _temp531 > 1u?*(( int*) _temp531) == Cyc_Absyn_Int_c: 0){ _LL536:
_temp535=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp531)->f1; if( _temp535
==( void*) Cyc_Absyn_Unsigned){ goto _LL534;} else{ goto _LL429;} _LL534:
_temp533=(( struct Cyc_Absyn_Int_c_struct*) _temp531)->f2; goto _LL428;} else{
goto _LL429;}} else{ goto _LL429;} _LL429: if(*(( int*) _temp419) == Cyc_Absyn_Const_e){
_LL538: _temp537=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp419)->f1; if((
unsigned int) _temp537 > 1u?*(( int*) _temp537) == Cyc_Absyn_LongLong_c: 0){
_LL542: _temp541=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp537)->f1;
goto _LL540; _LL540: _temp539=(( struct Cyc_Absyn_LongLong_c_struct*) _temp537)->f2;
goto _LL430;} else{ goto _LL431;}} else{ goto _LL431;} _LL431: if(*(( int*)
_temp419) == Cyc_Absyn_Const_e){ _LL544: _temp543=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp419)->f1; if(( unsigned int) _temp543 > 1u?*(( int*) _temp543) == Cyc_Absyn_Float_c:
0){ _LL546: _temp545=(( struct Cyc_Absyn_Float_c_struct*) _temp543)->f1; goto
_LL432;} else{ goto _LL433;}} else{ goto _LL433;} _LL433: if(*(( int*) _temp419)
== Cyc_Absyn_Const_e){ _LL548: _temp547=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp419)->f1; if( _temp547 ==( void*) Cyc_Absyn_Null_c){ goto _LL434;} else{
goto _LL435;}} else{ goto _LL435;} _LL435: if(*(( int*) _temp419) == Cyc_Absyn_Const_e){
_LL550: _temp549=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp419)->f1; if((
unsigned int) _temp549 > 1u?*(( int*) _temp549) == Cyc_Absyn_String_c: 0){
_LL552: _temp551=(( struct Cyc_Absyn_String_c_struct*) _temp549)->f1; goto
_LL436;} else{ goto _LL437;}} else{ goto _LL437;} _LL437: if(*(( int*) _temp419)
== Cyc_Absyn_UnknownId_e){ _LL554: _temp553=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp419)->f1; goto _LL438;} else{ goto _LL439;} _LL439: if(*(( int*) _temp419)
== Cyc_Absyn_Var_e){ _LL558: _temp557=(( struct Cyc_Absyn_Var_e_struct*)
_temp419)->f1; goto _LL556; _LL556: _temp555=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp419)->f2; goto _LL440;} else{ goto _LL441;} _LL441: if(*(( int*) _temp419)
== Cyc_Absyn_Primop_e){ _LL562: _temp561=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp419)->f1; goto _LL560; _LL560: _temp559=(( struct Cyc_Absyn_Primop_e_struct*)
_temp419)->f2; goto _LL442;} else{ goto _LL443;} _LL443: if(*(( int*) _temp419)
== Cyc_Absyn_AssignOp_e){ _LL568: _temp567=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp419)->f1; goto _LL566; _LL566: _temp565=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp419)->f2; goto _LL564; _LL564: _temp563=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp419)->f3; goto _LL444;} else{ goto _LL445;} _LL445: if(*(( int*) _temp419)
== Cyc_Absyn_Increment_e){ _LL572: _temp571=(( struct Cyc_Absyn_Increment_e_struct*)
_temp419)->f1; goto _LL570; _LL570: _temp569=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp419)->f2; if( _temp569 ==( void*) Cyc_Absyn_PreInc){ goto _LL446;} else{
goto _LL447;}} else{ goto _LL447;} _LL447: if(*(( int*) _temp419) == Cyc_Absyn_Increment_e){
_LL576: _temp575=(( struct Cyc_Absyn_Increment_e_struct*) _temp419)->f1; goto
_LL574; _LL574: _temp573=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp419)->f2; if( _temp573 ==( void*) Cyc_Absyn_PreDec){ goto _LL448;} else{
goto _LL449;}} else{ goto _LL449;} _LL449: if(*(( int*) _temp419) == Cyc_Absyn_Increment_e){
_LL580: _temp579=(( struct Cyc_Absyn_Increment_e_struct*) _temp419)->f1; goto
_LL578; _LL578: _temp577=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp419)->f2; if( _temp577 ==( void*) Cyc_Absyn_PostInc){ goto _LL450;} else{
goto _LL451;}} else{ goto _LL451;} _LL451: if(*(( int*) _temp419) == Cyc_Absyn_Increment_e){
_LL584: _temp583=(( struct Cyc_Absyn_Increment_e_struct*) _temp419)->f1; goto
_LL582; _LL582: _temp581=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp419)->f2; if( _temp581 ==( void*) Cyc_Absyn_PostDec){ goto _LL452;} else{
goto _LL453;}} else{ goto _LL453;} _LL453: if(*(( int*) _temp419) == Cyc_Absyn_Conditional_e){
_LL590: _temp589=(( struct Cyc_Absyn_Conditional_e_struct*) _temp419)->f1; goto
_LL588; _LL588: _temp587=(( struct Cyc_Absyn_Conditional_e_struct*) _temp419)->f2;
goto _LL586; _LL586: _temp585=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp419)->f3; goto _LL454;} else{ goto _LL455;} _LL455: if(*(( int*) _temp419)
== Cyc_Absyn_SeqExp_e){ _LL594: _temp593=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp419)->f1; goto _LL592; _LL592: _temp591=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp419)->f2; goto _LL456;} else{ goto _LL457;} _LL457: if(*(( int*) _temp419)
== Cyc_Absyn_UnknownCall_e){ _LL598: _temp597=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp419)->f1; goto _LL596; _LL596: _temp595=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp419)->f2; goto _LL458;} else{ goto _LL459;} _LL459: if(*(( int*) _temp419)
== Cyc_Absyn_FnCall_e){ _LL604: _temp603=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp419)->f1; goto _LL602; _LL602: _temp601=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp419)->f2; goto _LL600; _LL600: _temp599=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp419)->f3; goto _LL460;} else{ goto _LL461;} _LL461: if(*(( int*) _temp419)
== Cyc_Absyn_Throw_e){ _LL606: _temp605=(( struct Cyc_Absyn_Throw_e_struct*)
_temp419)->f1; goto _LL462;} else{ goto _LL463;} _LL463: if(*(( int*) _temp419)
== Cyc_Absyn_NoInstantiate_e){ _LL608: _temp607=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp419)->f1; goto _LL464;} else{ goto _LL465;} _LL465: if(*(( int*) _temp419)
== Cyc_Absyn_Instantiate_e){ _LL612: _temp611=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp419)->f1; goto _LL610; _LL610: _temp609=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp419)->f2; goto _LL466;} else{ goto _LL467;} _LL467: if(*(( int*) _temp419)
== Cyc_Absyn_Cast_e){ _LL616: _temp615=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp419)->f1; goto _LL614; _LL614: _temp613=(( struct Cyc_Absyn_Cast_e_struct*)
_temp419)->f2; goto _LL468;} else{ goto _LL469;} _LL469: if(*(( int*) _temp419)
== Cyc_Absyn_Address_e){ _LL618: _temp617=(( struct Cyc_Absyn_Address_e_struct*)
_temp419)->f1; goto _LL470;} else{ goto _LL471;} _LL471: if(*(( int*) _temp419)
== Cyc_Absyn_New_e){ _LL622: _temp621=(( struct Cyc_Absyn_New_e_struct*)
_temp419)->f1; goto _LL620; _LL620: _temp619=(( struct Cyc_Absyn_New_e_struct*)
_temp419)->f2; goto _LL472;} else{ goto _LL473;} _LL473: if(*(( int*) _temp419)
== Cyc_Absyn_Sizeoftyp_e){ _LL624: _temp623=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp419)->f1; goto _LL474;} else{ goto _LL475;} _LL475: if(*(( int*) _temp419)
== Cyc_Absyn_Sizeofexp_e){ _LL626: _temp625=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp419)->f1; goto _LL476;} else{ goto _LL477;} _LL477: if(*(( int*) _temp419)
== Cyc_Absyn_Offsetof_e){ _LL630: _temp629=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp419)->f1; goto _LL628; _LL628: _temp627=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp419)->f2; goto _LL478;} else{ goto _LL479;} _LL479: if(*(( int*) _temp419)
== Cyc_Absyn_Deref_e){ _LL632: _temp631=(( struct Cyc_Absyn_Deref_e_struct*)
_temp419)->f1; goto _LL480;} else{ goto _LL481;} _LL481: if(*(( int*) _temp419)
== Cyc_Absyn_StructMember_e){ _LL636: _temp635=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp419)->f1; goto _LL634; _LL634: _temp633=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp419)->f2; goto _LL482;} else{ goto _LL483;} _LL483: if(*(( int*) _temp419)
== Cyc_Absyn_StructArrow_e){ _LL640: _temp639=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp419)->f1; goto _LL638; _LL638: _temp637=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp419)->f2; goto _LL484;} else{ goto _LL485;} _LL485: if(*(( int*) _temp419)
== Cyc_Absyn_Subscript_e){ _LL644: _temp643=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp419)->f1; goto _LL642; _LL642: _temp641=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp419)->f2; goto _LL486;} else{ goto _LL487;} _LL487: if(*(( int*) _temp419)
== Cyc_Absyn_Tuple_e){ _LL646: _temp645=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp419)->f1; goto _LL488;} else{ goto _LL489;} _LL489: if(*(( int*) _temp419)
== Cyc_Absyn_CompoundLit_e){ _LL650: _temp649=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp419)->f1; goto _LL648; _LL648: _temp647=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp419)->f2; goto _LL490;} else{ goto _LL491;} _LL491: if(*(( int*) _temp419)
== Cyc_Absyn_Array_e){ _LL652: _temp651=(( struct Cyc_Absyn_Array_e_struct*)
_temp419)->f1; goto _LL492;} else{ goto _LL493;} _LL493: if(*(( int*) _temp419)
== Cyc_Absyn_Comprehension_e){ _LL658: _temp657=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp419)->f1; goto _LL656; _LL656: _temp655=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp419)->f2; goto _LL654; _LL654: _temp653=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp419)->f3; goto _LL494;} else{ goto _LL495;} _LL495: if(*(( int*) _temp419)
== Cyc_Absyn_Struct_e){ _LL666: _temp665=(( struct Cyc_Absyn_Struct_e_struct*)
_temp419)->f1; goto _LL664; _LL664: _temp663=(( struct Cyc_Absyn_Struct_e_struct*)
_temp419)->f2; goto _LL662; _LL662: _temp661=(( struct Cyc_Absyn_Struct_e_struct*)
_temp419)->f3; goto _LL660; _LL660: _temp659=(( struct Cyc_Absyn_Struct_e_struct*)
_temp419)->f4; goto _LL496;} else{ goto _LL497;} _LL497: if(*(( int*) _temp419)
== Cyc_Absyn_AnonStruct_e){ _LL670: _temp669=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp419)->f1; goto _LL668; _LL668: _temp667=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp419)->f2; goto _LL498;} else{ goto _LL499;} _LL499: if(*(( int*) _temp419)
== Cyc_Absyn_Tunion_e){ _LL680: _temp679=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp419)->f1; goto _LL678; _LL678: _temp677=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp419)->f2; goto _LL676; _LL676: _temp675=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp419)->f3; goto _LL674; _LL674: _temp673=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp419)->f4; goto _LL672; _LL672: _temp671=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp419)->f5; goto _LL500;} else{ goto _LL501;} _LL501: if(*(( int*) _temp419)
== Cyc_Absyn_Enum_e){ _LL686: _temp685=(( struct Cyc_Absyn_Enum_e_struct*)
_temp419)->f1; goto _LL684; _LL684: _temp683=(( struct Cyc_Absyn_Enum_e_struct*)
_temp419)->f2; goto _LL682; _LL682: _temp681=(( struct Cyc_Absyn_Enum_e_struct*)
_temp419)->f3; goto _LL502;} else{ goto _LL503;} _LL503: if(*(( int*) _temp419)
== Cyc_Absyn_Malloc_e){ _LL690: _temp689=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp419)->f1; goto _LL688; _LL688: _temp687=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp419)->f2; goto _LL504;} else{ goto _LL505;} _LL505: if(*(( int*) _temp419)
== Cyc_Absyn_UnresolvedMem_e){ _LL694: _temp693=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp419)->f1; goto _LL692; _LL692: _temp691=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp419)->f2; goto _LL506;} else{ goto _LL507;} _LL507: if(*(( int*) _temp419)
== Cyc_Absyn_StmtExp_e){ _LL696: _temp695=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp419)->f1; goto _LL508;} else{ goto _LL509;} _LL509: if(*(( int*) _temp419)
== Cyc_Absyn_Codegen_e){ _LL698: _temp697=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp419)->f1; goto _LL510;} else{ goto _LL511;} _LL511: if(*(( int*) _temp419)
== Cyc_Absyn_Fill_e){ _LL700: _temp699=(( struct Cyc_Absyn_Fill_e_struct*)
_temp419)->f1; goto _LL512;} else{ goto _LL420;} _LL422: Cyc_Absyndump_dump_char((
int)'\''); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_char_escape( _temp515)); Cyc_Absyndump_dump_char((
int)'\''); goto _LL420; _LL424: Cyc_Absyndump_dump(( struct _tagged_arr) Cyc_Core_string_of_int((
int) _temp521)); goto _LL420; _LL426: Cyc_Absyndump_dump(( struct _tagged_arr)
Cyc_Core_string_of_int( _temp527)); goto _LL420; _LL428: Cyc_Absyndump_dump((
struct _tagged_arr) Cyc_Core_string_of_int( _temp533)); Cyc_Absyndump_dump_nospace(
_tag_arr("u", sizeof( unsigned char), 2u)); goto _LL420; _LL430: Cyc_Absyndump_dump(
_tag_arr("<<FIX LONG LONG CONSTANT>>", sizeof( unsigned char), 27u)); goto
_LL420; _LL432: Cyc_Absyndump_dump( _temp545); goto _LL420; _LL434: Cyc_Absyndump_dump(
_tag_arr("null", sizeof( unsigned char), 5u)); goto _LL420; _LL436: Cyc_Absyndump_dump_char((
int)'"'); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_string_escape( _temp551)); Cyc_Absyndump_dump_char((
int)'"'); goto _LL420; _LL438: _temp557= _temp553; goto _LL440; _LL440: Cyc_Absyndump_dumpqvar(
_temp557); goto _LL420; _LL442: { struct _tagged_arr _temp701= Cyc_Absynpp_prim2str(
_temp561); if( ! Cyc_Absyn_is_format_prim( _temp561)){ switch((( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp559)){ case 1: _LL702: if( _temp561 ==(
void*) Cyc_Absyn_Size){ Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp559))->hd); Cyc_Absyndump_dump(
_tag_arr(".size", sizeof( unsigned char), 6u));} else{ Cyc_Absyndump_dump(
_temp701); Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp559))->hd);} break; case 2: _LL703: Cyc_Absyndump_dumpexp_prec(
myprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp559))->hd);
Cyc_Absyndump_dump( _temp701); Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp559))->tl))->hd); break; default: _LL704:( int) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp706=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp706[ 0]=({ struct Cyc_Core_Failure_struct
_temp707; _temp707.tag= Cyc_Core_Failure; _temp707.f1= _tag_arr("Absyndump -- Bad number of arguments to primop",
sizeof( unsigned char), 47u); _temp707;}); _temp706;}));}} else{ Cyc_Absyndump_dump(
_temp701); Cyc_Absyndump_dump_nospace( _tag_arr("(", sizeof( unsigned char), 2u));
Cyc_Absyndump_dumpexps_prec( 20, _temp559); Cyc_Absyndump_dump_nospace( _tag_arr(")",
sizeof( unsigned char), 2u));} goto _LL420;} _LL444: Cyc_Absyndump_dumpexp_prec(
myprec, _temp567); if( _temp565 != 0){ Cyc_Absyndump_dump( Cyc_Absynpp_prim2str((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp565))->v));} Cyc_Absyndump_dump_nospace(
_tag_arr("=", sizeof( unsigned char), 2u)); Cyc_Absyndump_dumpexp_prec( myprec,
_temp563); goto _LL420; _LL446: Cyc_Absyndump_dump( _tag_arr("++", sizeof(
unsigned char), 3u)); Cyc_Absyndump_dumpexp_prec( myprec, _temp571); goto _LL420;
_LL448: Cyc_Absyndump_dump( _tag_arr("--", sizeof( unsigned char), 3u)); Cyc_Absyndump_dumpexp_prec(
myprec, _temp575); goto _LL420; _LL450: Cyc_Absyndump_dumpexp_prec( myprec,
_temp579); Cyc_Absyndump_dump( _tag_arr("++", sizeof( unsigned char), 3u)); goto
_LL420; _LL452: Cyc_Absyndump_dumpexp_prec( myprec, _temp583); Cyc_Absyndump_dump(
_tag_arr("--", sizeof( unsigned char), 3u)); goto _LL420; _LL454: Cyc_Absyndump_dumpexp_prec(
myprec, _temp589); Cyc_Absyndump_dump_char(( int)'?'); Cyc_Absyndump_dumpexp_prec(
0, _temp587); Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp585); goto _LL420; _LL456: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumpexp_prec(
myprec, _temp593); Cyc_Absyndump_dump_char(( int)','); Cyc_Absyndump_dumpexp_prec(
myprec, _temp591); Cyc_Absyndump_dump_char(( int)')'); goto _LL420; _LL458:
_temp603= _temp597; _temp601= _temp595; goto _LL460; _LL460: Cyc_Absyndump_dumpexp_prec(
myprec, _temp603); Cyc_Absyndump_dump_nospace( _tag_arr("(", sizeof(
unsigned char), 2u)); Cyc_Absyndump_dumpexps_prec( 20, _temp601); Cyc_Absyndump_dump_nospace(
_tag_arr(")", sizeof( unsigned char), 2u)); goto _LL420; _LL462: Cyc_Absyndump_dump(
_tag_arr("throw", sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpexp_prec(
myprec, _temp605); goto _LL420; _LL464: _temp611= _temp607; goto _LL466; _LL466:
Cyc_Absyndump_dumpexp_prec( inprec, _temp611); goto _LL420; _LL468: Cyc_Absyndump_dump_char((
int)'('); Cyc_Absyndump_dumptyp( _temp615); Cyc_Absyndump_dump_char(( int)')');
Cyc_Absyndump_dumpexp_prec( myprec, _temp613); goto _LL420; _LL470: Cyc_Absyndump_dump_char((
int)'&'); Cyc_Absyndump_dumpexp_prec( myprec, _temp617); goto _LL420; _LL472:
Cyc_Absyndump_dump( _tag_arr("new ", sizeof( unsigned char), 5u)); Cyc_Absyndump_dumpexp_prec(
myprec, _temp619); goto _LL420; _LL474: Cyc_Absyndump_dump( _tag_arr("sizeof(",
sizeof( unsigned char), 8u)); Cyc_Absyndump_dumptyp( _temp623); Cyc_Absyndump_dump_char((
int)')'); goto _LL420; _LL476: Cyc_Absyndump_dump( _tag_arr("sizeof(", sizeof(
unsigned char), 8u)); Cyc_Absyndump_dumpexp_prec( myprec, _temp625); Cyc_Absyndump_dump_char((
int)')'); goto _LL420; _LL478: Cyc_Absyndump_dump( _tag_arr("offsetof(", sizeof(
unsigned char), 10u)); Cyc_Absyndump_dumptyp( _temp629); Cyc_Absyndump_dump_char((
int)','); Cyc_Absyndump_dump_nospace(* _temp627); Cyc_Absyndump_dump_char(( int)')');
goto _LL420; _LL480: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp631); goto _LL420; _LL482: Cyc_Absyndump_dumpexp_prec( myprec,
_temp635); Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp633); goto _LL420; _LL484: Cyc_Absyndump_dumpexp_prec( myprec, _temp639);
Cyc_Absyndump_dump_nospace( _tag_arr("->", sizeof( unsigned char), 3u)); Cyc_Absyndump_dump_nospace(*
_temp637); goto _LL420; _LL486: Cyc_Absyndump_dumpexp_prec( myprec, _temp643);
Cyc_Absyndump_dump_char(( int)'['); Cyc_Absyndump_dumpexp( _temp641); Cyc_Absyndump_dump_char((
int)']'); goto _LL420; _LL488: Cyc_Absyndump_dump( _tag_arr("$(", sizeof(
unsigned char), 3u)); Cyc_Absyndump_dumpexps_prec( 20, _temp645); Cyc_Absyndump_dump_char((
int)')'); goto _LL420; _LL490: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumptyp((*
_temp649).f3); Cyc_Absyndump_dump_char(( int)')');(( void(*)( void(* f)( struct
_tuple8*), struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr
end, struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp647, _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u)); goto _LL420;
_LL492:(( void(*)( void(* f)( struct _tuple8*), struct Cyc_List_List* l, struct
_tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp651, _tag_arr("{", sizeof( unsigned char), 2u),
_tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u)); goto _LL420; _LL494: Cyc_Absyndump_dump( _tag_arr("new {for", sizeof(
unsigned char), 9u)); Cyc_Absyndump_dump_str((* _temp657->name).f2); Cyc_Absyndump_dump_char((
int)'<'); Cyc_Absyndump_dumpexp( _temp655); Cyc_Absyndump_dump_char(( int)':');
Cyc_Absyndump_dumpexp( _temp653); Cyc_Absyndump_dump_char(( int)'}'); goto
_LL420; _LL496: Cyc_Absyndump_dumpqvar( _temp665);(( void(*)( void(* f)( struct
_tuple8*), struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr
end, struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp661, _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u)); goto _LL420;
_LL498:(( void(*)( void(* f)( struct _tuple8*), struct Cyc_List_List* l, struct
_tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp667, _tag_arr("{", sizeof( unsigned char), 2u),
_tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u)); goto _LL420; _LL500: Cyc_Absyndump_dumpqvar( _temp671->name); if( _temp675
!= 0){(( void(*)( void(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* l,
struct _tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpexp, _temp675, _tag_arr("(", sizeof( unsigned char), 2u),
_tag_arr(")", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u));} goto _LL420; _LL502: Cyc_Absyndump_dumpqvar( _temp685); goto _LL420;
_LL504: if( _temp689 != 0){ Cyc_Absyndump_dump( _tag_arr("rmalloc(", sizeof(
unsigned char), 9u)); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null(
_temp689)); Cyc_Absyndump_dump( _tag_arr(",", sizeof( unsigned char), 2u));}
else{ Cyc_Absyndump_dump( _tag_arr("malloc(", sizeof( unsigned char), 8u));} Cyc_Absyndump_dump(
_tag_arr("sizeof(", sizeof( unsigned char), 8u)); Cyc_Absyndump_dumptyp(
_temp687); Cyc_Absyndump_dump( _tag_arr("))", sizeof( unsigned char), 3u)); goto
_LL420; _LL506:(( void(*)( void(* f)( struct _tuple8*), struct Cyc_List_List* l,
struct _tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp691, _tag_arr("{", sizeof( unsigned char), 2u),
_tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u)); goto _LL420; _LL508: Cyc_Absyndump_dump_nospace( _tag_arr("({", sizeof(
unsigned char), 3u)); Cyc_Absyndump_dumpstmt( _temp695); Cyc_Absyndump_dump_nospace(
_tag_arr("})", sizeof( unsigned char), 3u)); goto _LL420; _LL510: Cyc_Absyndump_dump(
_tag_arr("codegen(", sizeof( unsigned char), 9u)); Cyc_Absyndump_dumpdecl(({
struct Cyc_Absyn_Decl* _temp708=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof(
struct Cyc_Absyn_Decl)); _temp708->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp709=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp709[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp710; _temp710.tag= Cyc_Absyn_Fn_d;
_temp710.f1= _temp697; _temp710;}); _temp709;})); _temp708->loc= e->loc;
_temp708;})); Cyc_Absyndump_dump( _tag_arr(")", sizeof( unsigned char), 2u));
goto _LL420; _LL512: Cyc_Absyndump_dump( _tag_arr("fill(", sizeof( unsigned char),
6u)); Cyc_Absyndump_dumpexp( _temp699); Cyc_Absyndump_dump( _tag_arr(")",
sizeof( unsigned char), 2u)); goto _LL420; _LL420:;} if( inprec >= myprec){ Cyc_Absyndump_dump_char((
int)')');}} void Cyc_Absyndump_dumpexp( struct Cyc_Absyn_Exp* e){ Cyc_Absyndump_dumpexp_prec(
0, e);} void Cyc_Absyndump_dumpswitchclauses( struct Cyc_List_List* scs){ for( 0;
scs != 0; scs=(( struct Cyc_List_List*) _check_null( scs))->tl){ struct Cyc_Absyn_Switch_clause*
_temp711=( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
scs))->hd; if( _temp711->where_clause == 0?( void*)( _temp711->pattern)->r ==(
void*) Cyc_Absyn_Wild_p: 0){ Cyc_Absyndump_dump( _tag_arr("default:", sizeof(
unsigned char), 9u));} else{ Cyc_Absyndump_dump( _tag_arr("case", sizeof(
unsigned char), 5u)); Cyc_Absyndump_dumppat( _temp711->pattern); if( _temp711->where_clause
!= 0){ Cyc_Absyndump_dump( _tag_arr("&&", sizeof( unsigned char), 3u)); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*) _check_null( _temp711->where_clause));} Cyc_Absyndump_dump_nospace(
_tag_arr(":", sizeof( unsigned char), 2u));} Cyc_Absyndump_dumpstmt( _temp711->body);}}
void Cyc_Absyndump_dumpstmt( struct Cyc_Absyn_Stmt* s){ void* _temp712=( void*)
s->r; struct Cyc_Absyn_Exp* _temp758; struct Cyc_Absyn_Stmt* _temp760; struct
Cyc_Absyn_Stmt* _temp762; struct Cyc_Absyn_Exp* _temp764; struct Cyc_Absyn_Exp*
_temp766; struct Cyc_Absyn_Stmt* _temp768; struct Cyc_Absyn_Stmt* _temp770;
struct Cyc_Absyn_Exp* _temp772; struct Cyc_Absyn_Stmt* _temp774; struct _tuple2
_temp776; struct Cyc_Absyn_Stmt* _temp778; struct Cyc_Absyn_Exp* _temp780;
struct Cyc_Absyn_Stmt* _temp782; struct Cyc_Absyn_Stmt* _temp784; struct Cyc_Absyn_Stmt*
_temp786; struct _tagged_arr* _temp788; struct Cyc_Absyn_Stmt* _temp790; struct
_tuple2 _temp792; struct Cyc_Absyn_Stmt* _temp794; struct Cyc_Absyn_Exp*
_temp796; struct _tuple2 _temp798; struct Cyc_Absyn_Stmt* _temp800; struct Cyc_Absyn_Exp*
_temp802; struct Cyc_Absyn_Exp* _temp804; struct Cyc_List_List* _temp806; struct
Cyc_Absyn_Exp* _temp808; struct Cyc_Absyn_Stmt* _temp810; struct Cyc_Absyn_Decl*
_temp812; struct Cyc_Absyn_Stmt* _temp814; struct _tagged_arr* _temp816; struct
_tuple2 _temp818; struct Cyc_Absyn_Stmt* _temp820; struct Cyc_Absyn_Exp*
_temp822; struct Cyc_Absyn_Stmt* _temp824; struct Cyc_List_List* _temp826;
struct Cyc_Absyn_Exp* _temp828; struct Cyc_Absyn_Switch_clause** _temp830;
struct Cyc_List_List* _temp832; struct Cyc_Absyn_Switch_clause** _temp834;
struct Cyc_List_List* _temp836; struct Cyc_List_List* _temp838; struct Cyc_Absyn_Stmt*
_temp840; struct Cyc_Absyn_Stmt* _temp842; struct Cyc_Absyn_Vardecl* _temp844;
struct Cyc_Absyn_Tvar* _temp846; struct Cyc_Absyn_Stmt* _temp848; struct Cyc_Absyn_Stmt*
_temp850; _LL714: if( _temp712 ==( void*) Cyc_Absyn_Skip_s){ goto _LL715;} else{
goto _LL716;} _LL716: if(( unsigned int) _temp712 > 1u?*(( int*) _temp712) ==
Cyc_Absyn_Exp_s: 0){ _LL759: _temp758=(( struct Cyc_Absyn_Exp_s_struct*)
_temp712)->f1; goto _LL717;} else{ goto _LL718;} _LL718: if(( unsigned int)
_temp712 > 1u?*(( int*) _temp712) == Cyc_Absyn_Seq_s: 0){ _LL763: _temp762=((
struct Cyc_Absyn_Seq_s_struct*) _temp712)->f1; goto _LL761; _LL761: _temp760=((
struct Cyc_Absyn_Seq_s_struct*) _temp712)->f2; goto _LL719;} else{ goto _LL720;}
_LL720: if(( unsigned int) _temp712 > 1u?*(( int*) _temp712) == Cyc_Absyn_Return_s:
0){ _LL765: _temp764=(( struct Cyc_Absyn_Return_s_struct*) _temp712)->f1; if(
_temp764 == 0){ goto _LL721;} else{ goto _LL722;}} else{ goto _LL722;} _LL722:
if(( unsigned int) _temp712 > 1u?*(( int*) _temp712) == Cyc_Absyn_Return_s: 0){
_LL767: _temp766=(( struct Cyc_Absyn_Return_s_struct*) _temp712)->f1; goto
_LL723;} else{ goto _LL724;} _LL724: if(( unsigned int) _temp712 > 1u?*(( int*)
_temp712) == Cyc_Absyn_IfThenElse_s: 0){ _LL773: _temp772=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp712)->f1; goto _LL771; _LL771: _temp770=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp712)->f2; goto _LL769; _LL769: _temp768=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp712)->f3; goto _LL725;} else{ goto _LL726;} _LL726: if(( unsigned int)
_temp712 > 1u?*(( int*) _temp712) == Cyc_Absyn_While_s: 0){ _LL777: _temp776=((
struct Cyc_Absyn_While_s_struct*) _temp712)->f1; _LL781: _temp780= _temp776.f1;
goto _LL779; _LL779: _temp778= _temp776.f2; goto _LL775; _LL775: _temp774=((
struct Cyc_Absyn_While_s_struct*) _temp712)->f2; goto _LL727;} else{ goto _LL728;}
_LL728: if(( unsigned int) _temp712 > 1u?*(( int*) _temp712) == Cyc_Absyn_Break_s:
0){ _LL783: _temp782=(( struct Cyc_Absyn_Break_s_struct*) _temp712)->f1; goto
_LL729;} else{ goto _LL730;} _LL730: if(( unsigned int) _temp712 > 1u?*(( int*)
_temp712) == Cyc_Absyn_Continue_s: 0){ _LL785: _temp784=(( struct Cyc_Absyn_Continue_s_struct*)
_temp712)->f1; goto _LL731;} else{ goto _LL732;} _LL732: if(( unsigned int)
_temp712 > 1u?*(( int*) _temp712) == Cyc_Absyn_Goto_s: 0){ _LL789: _temp788=((
struct Cyc_Absyn_Goto_s_struct*) _temp712)->f1; goto _LL787; _LL787: _temp786=((
struct Cyc_Absyn_Goto_s_struct*) _temp712)->f2; goto _LL733;} else{ goto _LL734;}
_LL734: if(( unsigned int) _temp712 > 1u?*(( int*) _temp712) == Cyc_Absyn_For_s:
0){ _LL805: _temp804=(( struct Cyc_Absyn_For_s_struct*) _temp712)->f1; goto
_LL799; _LL799: _temp798=(( struct Cyc_Absyn_For_s_struct*) _temp712)->f2;
_LL803: _temp802= _temp798.f1; goto _LL801; _LL801: _temp800= _temp798.f2; goto
_LL793; _LL793: _temp792=(( struct Cyc_Absyn_For_s_struct*) _temp712)->f3;
_LL797: _temp796= _temp792.f1; goto _LL795; _LL795: _temp794= _temp792.f2; goto
_LL791; _LL791: _temp790=(( struct Cyc_Absyn_For_s_struct*) _temp712)->f4; goto
_LL735;} else{ goto _LL736;} _LL736: if(( unsigned int) _temp712 > 1u?*(( int*)
_temp712) == Cyc_Absyn_Switch_s: 0){ _LL809: _temp808=(( struct Cyc_Absyn_Switch_s_struct*)
_temp712)->f1; goto _LL807; _LL807: _temp806=(( struct Cyc_Absyn_Switch_s_struct*)
_temp712)->f2; goto _LL737;} else{ goto _LL738;} _LL738: if(( unsigned int)
_temp712 > 1u?*(( int*) _temp712) == Cyc_Absyn_Decl_s: 0){ _LL813: _temp812=((
struct Cyc_Absyn_Decl_s_struct*) _temp712)->f1; goto _LL811; _LL811: _temp810=((
struct Cyc_Absyn_Decl_s_struct*) _temp712)->f2; goto _LL739;} else{ goto _LL740;}
_LL740: if(( unsigned int) _temp712 > 1u?*(( int*) _temp712) == Cyc_Absyn_Label_s:
0){ _LL817: _temp816=(( struct Cyc_Absyn_Label_s_struct*) _temp712)->f1; goto
_LL815; _LL815: _temp814=(( struct Cyc_Absyn_Label_s_struct*) _temp712)->f2;
goto _LL741;} else{ goto _LL742;} _LL742: if(( unsigned int) _temp712 > 1u?*((
int*) _temp712) == Cyc_Absyn_Do_s: 0){ _LL825: _temp824=(( struct Cyc_Absyn_Do_s_struct*)
_temp712)->f1; goto _LL819; _LL819: _temp818=(( struct Cyc_Absyn_Do_s_struct*)
_temp712)->f2; _LL823: _temp822= _temp818.f1; goto _LL821; _LL821: _temp820=
_temp818.f2; goto _LL743;} else{ goto _LL744;} _LL744: if(( unsigned int)
_temp712 > 1u?*(( int*) _temp712) == Cyc_Absyn_SwitchC_s: 0){ _LL829: _temp828=((
struct Cyc_Absyn_SwitchC_s_struct*) _temp712)->f1; goto _LL827; _LL827: _temp826=((
struct Cyc_Absyn_SwitchC_s_struct*) _temp712)->f2; goto _LL745;} else{ goto
_LL746;} _LL746: if(( unsigned int) _temp712 > 1u?*(( int*) _temp712) == Cyc_Absyn_Fallthru_s:
0){ _LL833: _temp832=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp712)->f1; if(
_temp832 == 0){ goto _LL831;} else{ goto _LL748;} _LL831: _temp830=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp712)->f2; goto _LL747;} else{ goto _LL748;} _LL748: if(( unsigned int)
_temp712 > 1u?*(( int*) _temp712) == Cyc_Absyn_Fallthru_s: 0){ _LL837: _temp836=((
struct Cyc_Absyn_Fallthru_s_struct*) _temp712)->f1; goto _LL835; _LL835:
_temp834=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp712)->f2; goto _LL749;}
else{ goto _LL750;} _LL750: if(( unsigned int) _temp712 > 1u?*(( int*) _temp712)
== Cyc_Absyn_TryCatch_s: 0){ _LL841: _temp840=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp712)->f1; goto _LL839; _LL839: _temp838=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp712)->f2; goto _LL751;} else{ goto _LL752;} _LL752: if(( unsigned int)
_temp712 > 1u?*(( int*) _temp712) == Cyc_Absyn_Region_s: 0){ _LL847: _temp846=((
struct Cyc_Absyn_Region_s_struct*) _temp712)->f1; goto _LL845; _LL845: _temp844=((
struct Cyc_Absyn_Region_s_struct*) _temp712)->f2; goto _LL843; _LL843: _temp842=((
struct Cyc_Absyn_Region_s_struct*) _temp712)->f3; goto _LL753;} else{ goto
_LL754;} _LL754: if(( unsigned int) _temp712 > 1u?*(( int*) _temp712) == Cyc_Absyn_Cut_s:
0){ _LL849: _temp848=(( struct Cyc_Absyn_Cut_s_struct*) _temp712)->f1; goto
_LL755;} else{ goto _LL756;} _LL756: if(( unsigned int) _temp712 > 1u?*(( int*)
_temp712) == Cyc_Absyn_Splice_s: 0){ _LL851: _temp850=(( struct Cyc_Absyn_Splice_s_struct*)
_temp712)->f1; goto _LL757;} else{ goto _LL713;} _LL715: Cyc_Absyndump_dump_semi();
goto _LL713; _LL717: Cyc_Absyndump_dumpexp( _temp758); Cyc_Absyndump_dump_semi();
goto _LL713; _LL719: if( Cyc_Absynpp_is_declaration( _temp762)){ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpstmt( _temp762); Cyc_Absyndump_dump_char(( int)'}');}
else{ Cyc_Absyndump_dumpstmt( _temp762);} if( Cyc_Absynpp_is_declaration(
_temp760)){ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt( _temp760);
Cyc_Absyndump_dump_char(( int)'}');} else{ Cyc_Absyndump_dumpstmt( _temp760);}
goto _LL713; _LL721: Cyc_Absyndump_dump( _tag_arr("return;", sizeof(
unsigned char), 8u)); goto _LL713; _LL723: Cyc_Absyndump_dump( _tag_arr("return",
sizeof( unsigned char), 7u)); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)
_check_null( _temp766)); Cyc_Absyndump_dump_semi(); goto _LL713; _LL725: Cyc_Absyndump_dump(
_tag_arr("if(", sizeof( unsigned char), 4u)); Cyc_Absyndump_dumpexp( _temp772);
Cyc_Absyndump_dump_nospace( _tag_arr("){", sizeof( unsigned char), 3u)); Cyc_Absyndump_dumpstmt(
_temp770); Cyc_Absyndump_dump_char(( int)'}');{ void* _temp852=( void*) _temp768->r;
_LL854: if( _temp852 ==( void*) Cyc_Absyn_Skip_s){ goto _LL855;} else{ goto
_LL856;} _LL856: goto _LL857; _LL855: goto _LL853; _LL857: Cyc_Absyndump_dump(
_tag_arr("else{", sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpstmt( _temp768);
Cyc_Absyndump_dump_char(( int)'}'); goto _LL853; _LL853:;} goto _LL713; _LL727:
Cyc_Absyndump_dump( _tag_arr("while(", sizeof( unsigned char), 7u)); Cyc_Absyndump_dumpexp(
_temp780); Cyc_Absyndump_dump_nospace( _tag_arr(") {", sizeof( unsigned char), 4u));
Cyc_Absyndump_dumpstmt( _temp774); Cyc_Absyndump_dump_char(( int)'}'); goto
_LL713; _LL729: Cyc_Absyndump_dump( _tag_arr("break;", sizeof( unsigned char), 7u));
goto _LL713; _LL731: Cyc_Absyndump_dump( _tag_arr("continue;", sizeof(
unsigned char), 10u)); goto _LL713; _LL733: Cyc_Absyndump_dump( _tag_arr("goto",
sizeof( unsigned char), 5u)); Cyc_Absyndump_dump_str( _temp788); Cyc_Absyndump_dump_semi();
goto _LL713; _LL735: Cyc_Absyndump_dump( _tag_arr("for(", sizeof( unsigned char),
5u)); Cyc_Absyndump_dumpexp( _temp804); Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpexp(
_temp802); Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpexp( _temp796); Cyc_Absyndump_dump_nospace(
_tag_arr("){", sizeof( unsigned char), 3u)); Cyc_Absyndump_dumpstmt( _temp790);
Cyc_Absyndump_dump_char(( int)'}'); goto _LL713; _LL737: Cyc_Absyndump_dump(
_tag_arr("switch(", sizeof( unsigned char), 8u)); Cyc_Absyndump_dumpexp(
_temp808); Cyc_Absyndump_dump_nospace( _tag_arr("){", sizeof( unsigned char), 3u));
Cyc_Absyndump_dumpswitchclauses( _temp806); Cyc_Absyndump_dump_char(( int)'}');
goto _LL713; _LL739: Cyc_Absyndump_dumpdecl( _temp812); Cyc_Absyndump_dumpstmt(
_temp810); goto _LL713; _LL741: if( Cyc_Absynpp_is_declaration( _temp814)){ Cyc_Absyndump_dump_str(
_temp816); Cyc_Absyndump_dump_nospace( _tag_arr(": {", sizeof( unsigned char), 4u));
Cyc_Absyndump_dumpstmt( _temp814); Cyc_Absyndump_dump_char(( int)'}');} else{
Cyc_Absyndump_dump_str( _temp816); Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpstmt(
_temp814);} goto _LL713; _LL743: Cyc_Absyndump_dump( _tag_arr("do {", sizeof(
unsigned char), 5u)); Cyc_Absyndump_dumpstmt( _temp824); Cyc_Absyndump_dump_nospace(
_tag_arr("} while (", sizeof( unsigned char), 10u)); Cyc_Absyndump_dumpexp(
_temp822); Cyc_Absyndump_dump_nospace( _tag_arr(");", sizeof( unsigned char), 3u));
goto _LL713; _LL745: Cyc_Absyndump_dump( _tag_arr("switch \"C\" (", sizeof(
unsigned char), 13u)); Cyc_Absyndump_dumpexp( _temp828); Cyc_Absyndump_dump_nospace(
_tag_arr("){", sizeof( unsigned char), 3u)); for( 0; _temp826 != 0; _temp826=((
struct Cyc_List_List*) _check_null( _temp826))->tl){ struct Cyc_Absyn_SwitchC_clause
_temp860; struct Cyc_Position_Segment* _temp861; struct Cyc_Absyn_Stmt* _temp863;
struct Cyc_Absyn_Exp* _temp865; struct Cyc_Absyn_SwitchC_clause* _temp858=(
struct Cyc_Absyn_SwitchC_clause*)(( struct Cyc_List_List*) _check_null( _temp826))->hd;
_temp860=* _temp858; _LL866: _temp865= _temp860.cnst_exp; goto _LL864; _LL864:
_temp863= _temp860.body; goto _LL862; _LL862: _temp861= _temp860.loc; goto
_LL859; _LL859: if( _temp865 == 0){ Cyc_Absyndump_dump( _tag_arr("default: ",
sizeof( unsigned char), 10u));} else{ Cyc_Absyndump_dump( _tag_arr("case ",
sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)
_check_null( _temp865)); Cyc_Absyndump_dump_char(( int)':');} Cyc_Absyndump_dumpstmt(
_temp863);} Cyc_Absyndump_dump_char(( int)'}'); goto _LL713; _LL747: Cyc_Absyndump_dump(
_tag_arr("fallthru;", sizeof( unsigned char), 10u)); goto _LL713; _LL749: Cyc_Absyndump_dump(
_tag_arr("fallthru(", sizeof( unsigned char), 10u)); Cyc_Absyndump_dumpexps_prec(
20, _temp836); Cyc_Absyndump_dump_nospace( _tag_arr(");", sizeof( unsigned char),
3u)); goto _LL713; _LL751: Cyc_Absyndump_dump( _tag_arr("try", sizeof(
unsigned char), 4u)); Cyc_Absyndump_dumpstmt( _temp840); Cyc_Absyndump_dump(
_tag_arr("catch {", sizeof( unsigned char), 8u)); Cyc_Absyndump_dumpswitchclauses(
_temp838); Cyc_Absyndump_dump_char(( int)'}'); goto _LL713; _LL753: Cyc_Absyndump_dump(
_tag_arr("region<", sizeof( unsigned char), 8u)); Cyc_Absyndump_dumptvar(
_temp846); Cyc_Absyndump_dump( _tag_arr("> ", sizeof( unsigned char), 3u)); Cyc_Absyndump_dumpqvar(
_temp844->name); Cyc_Absyndump_dump( _tag_arr("{", sizeof( unsigned char), 2u));
Cyc_Absyndump_dumpstmt( _temp842); Cyc_Absyndump_dump( _tag_arr("}", sizeof(
unsigned char), 2u)); goto _LL713; _LL755: Cyc_Absyndump_dump( _tag_arr("cut",
sizeof( unsigned char), 4u)); Cyc_Absyndump_dumpstmt( _temp848); goto _LL713;
_LL757: Cyc_Absyndump_dump( _tag_arr("splice", sizeof( unsigned char), 7u)); Cyc_Absyndump_dumpstmt(
_temp850); goto _LL713; _LL713:;} struct _tuple9{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Pat* f2; } ; void Cyc_Absyndump_dumpdp( struct _tuple9* dp){((
void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_arr start,
struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumpdesignator,(*
dp).f1, _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("=", sizeof(
unsigned char), 2u), _tag_arr("=", sizeof( unsigned char), 2u)); Cyc_Absyndump_dumppat((*
dp).f2);} void Cyc_Absyndump_dumppat( struct Cyc_Absyn_Pat* p){ void* _temp867=(
void*) p->r; int _temp901; void* _temp903; int _temp905; void* _temp907;
unsigned char _temp909; struct _tagged_arr _temp911; struct Cyc_Absyn_Vardecl*
_temp913; struct Cyc_List_List* _temp915; struct Cyc_Absyn_Pat* _temp917; struct
Cyc_Absyn_Vardecl* _temp919; struct _tuple0* _temp921; struct Cyc_List_List*
_temp923; struct Cyc_List_List* _temp925; struct _tuple0* _temp927; struct Cyc_List_List*
_temp929; struct Cyc_List_List* _temp931; struct _tuple0* _temp933; struct Cyc_List_List*
_temp935; struct Cyc_List_List* _temp937; struct Cyc_Core_Opt* _temp939; struct
Cyc_Absyn_Structdecl* _temp941; struct Cyc_List_List* _temp943; struct Cyc_List_List*
_temp945; struct Cyc_Absyn_Tunionfield* _temp947; struct Cyc_Absyn_Tuniondecl*
_temp949; struct Cyc_Absyn_Enumfield* _temp951; struct Cyc_Absyn_Enumdecl*
_temp953; _LL869: if( _temp867 ==( void*) Cyc_Absyn_Wild_p){ goto _LL870;} else{
goto _LL871;} _LL871: if( _temp867 ==( void*) Cyc_Absyn_Null_p){ goto _LL872;}
else{ goto _LL873;} _LL873: if(( unsigned int) _temp867 > 2u?*(( int*) _temp867)
== Cyc_Absyn_Int_p: 0){ _LL904: _temp903=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp867)->f1; if( _temp903 ==( void*) Cyc_Absyn_Signed){ goto _LL902;} else{
goto _LL875;} _LL902: _temp901=(( struct Cyc_Absyn_Int_p_struct*) _temp867)->f2;
goto _LL874;} else{ goto _LL875;} _LL875: if(( unsigned int) _temp867 > 2u?*((
int*) _temp867) == Cyc_Absyn_Int_p: 0){ _LL908: _temp907=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp867)->f1; if( _temp907 ==( void*) Cyc_Absyn_Unsigned){ goto _LL906;} else{
goto _LL877;} _LL906: _temp905=(( struct Cyc_Absyn_Int_p_struct*) _temp867)->f2;
goto _LL876;} else{ goto _LL877;} _LL877: if(( unsigned int) _temp867 > 2u?*((
int*) _temp867) == Cyc_Absyn_Char_p: 0){ _LL910: _temp909=(( struct Cyc_Absyn_Char_p_struct*)
_temp867)->f1; goto _LL878;} else{ goto _LL879;} _LL879: if(( unsigned int)
_temp867 > 2u?*(( int*) _temp867) == Cyc_Absyn_Float_p: 0){ _LL912: _temp911=((
struct Cyc_Absyn_Float_p_struct*) _temp867)->f1; goto _LL880;} else{ goto _LL881;}
_LL881: if(( unsigned int) _temp867 > 2u?*(( int*) _temp867) == Cyc_Absyn_Var_p:
0){ _LL914: _temp913=(( struct Cyc_Absyn_Var_p_struct*) _temp867)->f1; goto
_LL882;} else{ goto _LL883;} _LL883: if(( unsigned int) _temp867 > 2u?*(( int*)
_temp867) == Cyc_Absyn_Tuple_p: 0){ _LL916: _temp915=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp867)->f1; goto _LL884;} else{ goto _LL885;} _LL885: if(( unsigned int)
_temp867 > 2u?*(( int*) _temp867) == Cyc_Absyn_Pointer_p: 0){ _LL918: _temp917=((
struct Cyc_Absyn_Pointer_p_struct*) _temp867)->f1; goto _LL886;} else{ goto
_LL887;} _LL887: if(( unsigned int) _temp867 > 2u?*(( int*) _temp867) == Cyc_Absyn_Reference_p:
0){ _LL920: _temp919=(( struct Cyc_Absyn_Reference_p_struct*) _temp867)->f1;
goto _LL888;} else{ goto _LL889;} _LL889: if(( unsigned int) _temp867 > 2u?*((
int*) _temp867) == Cyc_Absyn_UnknownId_p: 0){ _LL922: _temp921=(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp867)->f1; goto _LL890;} else{ goto _LL891;} _LL891: if(( unsigned int)
_temp867 > 2u?*(( int*) _temp867) == Cyc_Absyn_UnknownCall_p: 0){ _LL928:
_temp927=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp867)->f1; goto _LL926;
_LL926: _temp925=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp867)->f2; goto
_LL924; _LL924: _temp923=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp867)->f3;
goto _LL892;} else{ goto _LL893;} _LL893: if(( unsigned int) _temp867 > 2u?*((
int*) _temp867) == Cyc_Absyn_UnknownFields_p: 0){ _LL934: _temp933=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp867)->f1; goto _LL932; _LL932: _temp931=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp867)->f2; goto _LL930; _LL930: _temp929=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp867)->f3; goto _LL894;} else{ goto _LL895;} _LL895: if(( unsigned int)
_temp867 > 2u?*(( int*) _temp867) == Cyc_Absyn_Struct_p: 0){ _LL942: _temp941=((
struct Cyc_Absyn_Struct_p_struct*) _temp867)->f1; goto _LL940; _LL940: _temp939=((
struct Cyc_Absyn_Struct_p_struct*) _temp867)->f2; goto _LL938; _LL938: _temp937=((
struct Cyc_Absyn_Struct_p_struct*) _temp867)->f3; goto _LL936; _LL936: _temp935=((
struct Cyc_Absyn_Struct_p_struct*) _temp867)->f4; goto _LL896;} else{ goto
_LL897;} _LL897: if(( unsigned int) _temp867 > 2u?*(( int*) _temp867) == Cyc_Absyn_Tunion_p:
0){ _LL950: _temp949=(( struct Cyc_Absyn_Tunion_p_struct*) _temp867)->f1; goto
_LL948; _LL948: _temp947=(( struct Cyc_Absyn_Tunion_p_struct*) _temp867)->f2;
goto _LL946; _LL946: _temp945=(( struct Cyc_Absyn_Tunion_p_struct*) _temp867)->f3;
goto _LL944; _LL944: _temp943=(( struct Cyc_Absyn_Tunion_p_struct*) _temp867)->f4;
goto _LL898;} else{ goto _LL899;} _LL899: if(( unsigned int) _temp867 > 2u?*((
int*) _temp867) == Cyc_Absyn_Enum_p: 0){ _LL954: _temp953=(( struct Cyc_Absyn_Enum_p_struct*)
_temp867)->f1; goto _LL952; _LL952: _temp951=(( struct Cyc_Absyn_Enum_p_struct*)
_temp867)->f2; goto _LL900;} else{ goto _LL868;} _LL870: Cyc_Absyndump_dump_char((
int)'_'); goto _LL868; _LL872: Cyc_Absyndump_dump( _tag_arr("null", sizeof(
unsigned char), 5u)); goto _LL868; _LL874: Cyc_Absyndump_dump(( struct
_tagged_arr) xprintf("%d", _temp901)); goto _LL868; _LL876: Cyc_Absyndump_dump((
struct _tagged_arr) xprintf("%u",( unsigned int) _temp905)); goto _LL868; _LL878:
Cyc_Absyndump_dump( _tag_arr("'", sizeof( unsigned char), 2u)); Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_char_escape( _temp909)); Cyc_Absyndump_dump_nospace( _tag_arr("'",
sizeof( unsigned char), 2u)); goto _LL868; _LL880: Cyc_Absyndump_dump( _temp911);
goto _LL868; _LL882: Cyc_Absyndump_dumpqvar( _temp913->name); goto _LL868;
_LL884:(( void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* l,
struct _tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat, _temp915, _tag_arr("$(", sizeof( unsigned char), 3u),
_tag_arr(")", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u)); goto _LL868; _LL886: Cyc_Absyndump_dump( _tag_arr("&", sizeof(
unsigned char), 2u)); Cyc_Absyndump_dumppat( _temp917); goto _LL868; _LL888: Cyc_Absyndump_dump(
_tag_arr("*", sizeof( unsigned char), 2u)); Cyc_Absyndump_dumpqvar( _temp919->name);
goto _LL868; _LL890: Cyc_Absyndump_dumpqvar( _temp921); goto _LL868; _LL892: Cyc_Absyndump_dumpqvar(
_temp927); Cyc_Absyndump_dumptvars( _temp925);(( void(*)( void(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr end,
struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp923,
_tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u)); goto _LL868; _LL894: Cyc_Absyndump_dumpqvar(
_temp933); Cyc_Absyndump_dumptvars( _temp931);(( void(*)( void(* f)( struct
_tuple9*), struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr
end, struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpdp,
_temp929, _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u)); goto _LL868;
_LL896: if( _temp941->name != 0){ Cyc_Absyndump_dumpqvar(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( _temp941->name))->v);} Cyc_Absyndump_dumptvars(
_temp937);(( void(*)( void(* f)( struct _tuple9*), struct Cyc_List_List* l,
struct _tagged_arr start, struct _tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpdp, _temp935, _tag_arr("{", sizeof( unsigned char), 2u),
_tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char),
2u)); goto _LL868; _LL898: Cyc_Absyndump_dumpqvar( _temp947->name); Cyc_Absyndump_dumptvars(
_temp945); if( _temp943 != 0){(( void(*)( void(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* l, struct _tagged_arr start, struct _tagged_arr end,
struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp943,
_tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u));} goto _LL868; _LL900: Cyc_Absyndump_dumpqvar(
_temp951->name); goto _LL868; _LL868:;} void Cyc_Absyndump_dumptunionfield(
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
_check_null( fields))->tl){ struct Cyc_Absyn_Structfield _temp957; struct Cyc_List_List*
_temp958; struct Cyc_Core_Opt* _temp960; void* _temp962; struct Cyc_Absyn_Tqual
_temp964; struct _tagged_arr* _temp966; struct Cyc_Absyn_Structfield* _temp955=(
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( fields))->hd;
_temp957=* _temp955; _LL967: _temp966= _temp957.name; goto _LL965; _LL965:
_temp964= _temp957.tq; goto _LL963; _LL963: _temp962=( void*) _temp957.type;
goto _LL961; _LL961: _temp960= _temp957.width; goto _LL959; _LL959: _temp958=
_temp957.attributes; goto _LL956; _LL956:(( void(*)( struct Cyc_Absyn_Tqual,
void*, void(* f)( struct _tagged_arr*), struct _tagged_arr*)) Cyc_Absyndump_dumptqtd)(
_temp964, _temp962, Cyc_Absyndump_dump_str, _temp966); Cyc_Absyndump_dumpatts(
_temp958); if( _temp960 != 0){ Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp960))->v);} Cyc_Absyndump_dump_semi();}}
void Cyc_Absyndump_dumptypedefname( struct Cyc_Absyn_Typedefdecl* td){ Cyc_Absyndump_dumpqvar(
td->name); Cyc_Absyndump_dumptvars( td->tvs);} static void Cyc_Absyndump_dump_atts_qvar(
struct Cyc_Absyn_Fndecl* fd){ Cyc_Absyndump_dumpatts( fd->attributes); Cyc_Absyndump_dumpqvar(
fd->name);} struct _tuple10{ void* f1; struct _tuple0* f2; } ; static void Cyc_Absyndump_dump_callconv_qvar(
struct _tuple10* pr){{ void* _temp968=(* pr).f1; _LL970: if( _temp968 ==( void*)
Cyc_Absyn_Unused_att){ goto _LL971;} else{ goto _LL972;} _LL972: if( _temp968 ==(
void*) Cyc_Absyn_Stdcall_att){ goto _LL973;} else{ goto _LL974;} _LL974: if(
_temp968 ==( void*) Cyc_Absyn_Cdecl_att){ goto _LL975;} else{ goto _LL976;}
_LL976: if( _temp968 ==( void*) Cyc_Absyn_Fastcall_att){ goto _LL977;} else{
goto _LL978;} _LL978: goto _LL979; _LL971: goto _LL969; _LL973: Cyc_Absyndump_dump(
_tag_arr("_stdcall", sizeof( unsigned char), 9u)); goto _LL969; _LL975: Cyc_Absyndump_dump(
_tag_arr("_cdecl", sizeof( unsigned char), 7u)); goto _LL969; _LL977: Cyc_Absyndump_dump(
_tag_arr("_fastcall", sizeof( unsigned char), 10u)); goto _LL969; _LL979: goto
_LL969; _LL969:;} Cyc_Absyndump_dumpqvar((* pr).f2);} static void Cyc_Absyndump_dump_callconv_fdqvar(
struct Cyc_Absyn_Fndecl* fd){ Cyc_Absyndump_dump_callconv( fd->attributes); Cyc_Absyndump_dumpqvar(
fd->name);} static void Cyc_Absyndump_dumpids( struct Cyc_List_List* vds){ for(
0; vds != 0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){ Cyc_Absyndump_dumpqvar(((
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( vds))->hd)->name);
if((( struct Cyc_List_List*) _check_null( vds))->tl != 0){ Cyc_Absyndump_dump_char((
int)',');}}} void Cyc_Absyndump_dumpdecl( struct Cyc_Absyn_Decl* d){ void*
_temp980=( void*) d->r; struct Cyc_Absyn_Fndecl* _temp1006; struct Cyc_Absyn_Structdecl*
_temp1008; struct Cyc_Absyn_Uniondecl* _temp1010; struct Cyc_Absyn_Vardecl*
_temp1012; struct Cyc_Absyn_Vardecl _temp1014; struct Cyc_List_List* _temp1015;
struct Cyc_Core_Opt* _temp1017; struct Cyc_Absyn_Exp* _temp1019; void* _temp1021;
struct Cyc_Absyn_Tqual _temp1023; struct _tuple0* _temp1025; void* _temp1027;
struct Cyc_Absyn_Tuniondecl* _temp1029; struct Cyc_Absyn_Tuniondecl _temp1031;
int _temp1032; struct Cyc_Core_Opt* _temp1034; struct Cyc_List_List* _temp1036;
struct _tuple0* _temp1038; void* _temp1040; struct Cyc_Absyn_Enumdecl* _temp1042;
struct Cyc_Absyn_Enumdecl _temp1044; struct Cyc_Core_Opt* _temp1045; struct
_tuple0* _temp1047; void* _temp1049; int _temp1051; struct Cyc_Absyn_Exp*
_temp1053; struct Cyc_Core_Opt* _temp1055; struct Cyc_Core_Opt* _temp1057;
struct Cyc_Absyn_Pat* _temp1059; struct Cyc_List_List* _temp1061; struct Cyc_Absyn_Typedefdecl*
_temp1063; struct Cyc_List_List* _temp1065; struct _tagged_arr* _temp1067;
struct Cyc_List_List* _temp1069; struct _tuple0* _temp1071; struct Cyc_List_List*
_temp1073; _LL982: if(*(( int*) _temp980) == Cyc_Absyn_Fn_d){ _LL1007: _temp1006=((
struct Cyc_Absyn_Fn_d_struct*) _temp980)->f1; goto _LL983;} else{ goto _LL984;}
_LL984: if(*(( int*) _temp980) == Cyc_Absyn_Struct_d){ _LL1009: _temp1008=((
struct Cyc_Absyn_Struct_d_struct*) _temp980)->f1; goto _LL985;} else{ goto
_LL986;} _LL986: if(*(( int*) _temp980) == Cyc_Absyn_Union_d){ _LL1011:
_temp1010=(( struct Cyc_Absyn_Union_d_struct*) _temp980)->f1; goto _LL987;}
else{ goto _LL988;} _LL988: if(*(( int*) _temp980) == Cyc_Absyn_Var_d){ _LL1013:
_temp1012=(( struct Cyc_Absyn_Var_d_struct*) _temp980)->f1; _temp1014=*
_temp1012; _LL1028: _temp1027=( void*) _temp1014.sc; goto _LL1026; _LL1026:
_temp1025= _temp1014.name; goto _LL1024; _LL1024: _temp1023= _temp1014.tq; goto
_LL1022; _LL1022: _temp1021=( void*) _temp1014.type; goto _LL1020; _LL1020:
_temp1019= _temp1014.initializer; goto _LL1018; _LL1018: _temp1017= _temp1014.rgn;
goto _LL1016; _LL1016: _temp1015= _temp1014.attributes; goto _LL989;} else{ goto
_LL990;} _LL990: if(*(( int*) _temp980) == Cyc_Absyn_Tunion_d){ _LL1030:
_temp1029=(( struct Cyc_Absyn_Tunion_d_struct*) _temp980)->f1; _temp1031=*
_temp1029; _LL1041: _temp1040=( void*) _temp1031.sc; goto _LL1039; _LL1039:
_temp1038= _temp1031.name; goto _LL1037; _LL1037: _temp1036= _temp1031.tvs; goto
_LL1035; _LL1035: _temp1034= _temp1031.fields; goto _LL1033; _LL1033: _temp1032=
_temp1031.is_xtunion; goto _LL991;} else{ goto _LL992;} _LL992: if(*(( int*)
_temp980) == Cyc_Absyn_Enum_d){ _LL1043: _temp1042=(( struct Cyc_Absyn_Enum_d_struct*)
_temp980)->f1; _temp1044=* _temp1042; _LL1050: _temp1049=( void*) _temp1044.sc;
goto _LL1048; _LL1048: _temp1047= _temp1044.name; goto _LL1046; _LL1046:
_temp1045= _temp1044.fields; goto _LL993;} else{ goto _LL994;} _LL994: if(*((
int*) _temp980) == Cyc_Absyn_Let_d){ _LL1060: _temp1059=(( struct Cyc_Absyn_Let_d_struct*)
_temp980)->f1; goto _LL1058; _LL1058: _temp1057=(( struct Cyc_Absyn_Let_d_struct*)
_temp980)->f2; goto _LL1056; _LL1056: _temp1055=(( struct Cyc_Absyn_Let_d_struct*)
_temp980)->f3; goto _LL1054; _LL1054: _temp1053=(( struct Cyc_Absyn_Let_d_struct*)
_temp980)->f4; goto _LL1052; _LL1052: _temp1051=(( struct Cyc_Absyn_Let_d_struct*)
_temp980)->f5; goto _LL995;} else{ goto _LL996;} _LL996: if(*(( int*) _temp980)
== Cyc_Absyn_Letv_d){ _LL1062: _temp1061=(( struct Cyc_Absyn_Letv_d_struct*)
_temp980)->f1; goto _LL997;} else{ goto _LL998;} _LL998: if(*(( int*) _temp980)
== Cyc_Absyn_Typedef_d){ _LL1064: _temp1063=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp980)->f1; goto _LL999;} else{ goto _LL1000;} _LL1000: if(*(( int*) _temp980)
== Cyc_Absyn_Namespace_d){ _LL1068: _temp1067=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp980)->f1; goto _LL1066; _LL1066: _temp1065=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp980)->f2; goto _LL1001;} else{ goto _LL1002;} _LL1002: if(*(( int*)
_temp980) == Cyc_Absyn_Using_d){ _LL1072: _temp1071=(( struct Cyc_Absyn_Using_d_struct*)
_temp980)->f1; goto _LL1070; _LL1070: _temp1069=(( struct Cyc_Absyn_Using_d_struct*)
_temp980)->f2; goto _LL1003;} else{ goto _LL1004;} _LL1004: if(*(( int*)
_temp980) == Cyc_Absyn_ExternC_d){ _LL1074: _temp1073=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp980)->f1; goto _LL1005;} else{ goto _LL981;} _LL983: if( Cyc_Absyndump_to_VC){
Cyc_Absyndump_dumpatts( _temp1006->attributes);} if( _temp1006->is_inline){ if(
Cyc_Absyndump_to_VC){ Cyc_Absyndump_dump( _tag_arr("__inline", sizeof(
unsigned char), 9u));} else{ Cyc_Absyndump_dump( _tag_arr("inline", sizeof(
unsigned char), 7u));}} Cyc_Absyndump_dumpscope(( void*) _temp1006->sc);{ void*
t=( void*)({ struct Cyc_Absyn_FnType_struct* _temp1076=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1076[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1077; _temp1077.tag= Cyc_Absyn_FnType; _temp1077.f1=({ struct Cyc_Absyn_FnInfo
_temp1078; _temp1078.tvars= _temp1006->tvs; _temp1078.effect= _temp1006->effect;
_temp1078.ret_typ=( void*)(( void*) _temp1006->ret_type); _temp1078.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp1006->args); _temp1078.c_varargs=
_temp1006->c_varargs; _temp1078.cyc_varargs= _temp1006->cyc_varargs; _temp1078.rgn_po=
_temp1006->rgn_po; _temp1078.attributes= 0; _temp1078;}); _temp1077;});
_temp1076;});(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct Cyc_Absyn_Fndecl*),
struct Cyc_Absyn_Fndecl*)) Cyc_Absyndump_dumptqtd)(({ struct Cyc_Absyn_Tqual
_temp1075; _temp1075.q_const= 0; _temp1075.q_volatile= 0; _temp1075.q_restrict=
0; _temp1075;}), t, Cyc_Absyndump_to_VC? Cyc_Absyndump_dump_callconv_fdqvar: Cyc_Absyndump_dump_atts_qvar,
_temp1006); Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt(
_temp1006->body); Cyc_Absyndump_dump_char(( int)'}'); goto _LL981;} _LL985: Cyc_Absyndump_dumpscope((
void*) _temp1008->sc); Cyc_Absyndump_dump( _tag_arr("struct", sizeof(
unsigned char), 7u)); if( _temp1008->name != 0){ Cyc_Absyndump_dumpqvar(( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp1008->name))->v);} Cyc_Absyndump_dumptvars(
_temp1008->tvs); if( _temp1008->fields == 0){ Cyc_Absyndump_dump_semi();} else{
Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstructfields(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1008->fields))->v); Cyc_Absyndump_dump(
_tag_arr("}", sizeof( unsigned char), 2u)); Cyc_Absyndump_dumpatts( _temp1008->attributes);
Cyc_Absyndump_dump( _tag_arr(";", sizeof( unsigned char), 2u));} goto _LL981;
_LL987: Cyc_Absyndump_dumpscope(( void*) _temp1010->sc); Cyc_Absyndump_dump(
_tag_arr("union", sizeof( unsigned char), 6u)); if( _temp1010->name != 0){ Cyc_Absyndump_dumpqvar((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp1010->name))->v);}
Cyc_Absyndump_dumptvars( _temp1010->tvs); if( _temp1010->fields == 0){ Cyc_Absyndump_dump_semi();}
else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstructfields((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1010->fields))->v);
Cyc_Absyndump_dump( _tag_arr("}", sizeof( unsigned char), 2u)); Cyc_Absyndump_dumpatts(
_temp1010->attributes); Cyc_Absyndump_dump( _tag_arr(";", sizeof( unsigned char),
2u));} goto _LL981; _LL989: if( Cyc_Absyndump_to_VC){ Cyc_Absyndump_dumpatts(
_temp1015); Cyc_Absyndump_dumpscope( _temp1027);{ struct Cyc_List_List*
_temp1081; void* _temp1083; struct Cyc_Absyn_Tqual _temp1085; struct _tuple4
_temp1079= Cyc_Absynpp_to_tms( _temp1023, _temp1021); _LL1086: _temp1085=
_temp1079.f1; goto _LL1084; _LL1084: _temp1083= _temp1079.f2; goto _LL1082;
_LL1082: _temp1081= _temp1079.f3; goto _LL1080; _LL1080: { void* call_conv=(
void*) Cyc_Absyn_Unused_att;{ struct Cyc_List_List* tms2= _temp1081; for( 0;
tms2 != 0; tms2=(( struct Cyc_List_List*) _check_null( tms2))->tl){ void*
_temp1087=( void*)(( struct Cyc_List_List*) _check_null( tms2))->hd; struct Cyc_List_List*
_temp1093; struct Cyc_Position_Segment* _temp1095; _LL1089: if(( unsigned int)
_temp1087 > 1u?*(( int*) _temp1087) == Cyc_Absyn_Attributes_mod: 0){ _LL1096:
_temp1095=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp1087)->f1; goto
_LL1094; _LL1094: _temp1093=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp1087)->f2; goto _LL1090;} else{ goto _LL1091;} _LL1091: goto _LL1092;
_LL1090: for( 0; _temp1093 != 0; _temp1093=(( struct Cyc_List_List*) _check_null(
_temp1093))->tl){ void* _temp1097=( void*)(( struct Cyc_List_List*) _check_null(
_temp1093))->hd; _LL1099: if( _temp1097 ==( void*) Cyc_Absyn_Stdcall_att){ goto
_LL1100;} else{ goto _LL1101;} _LL1101: if( _temp1097 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL1102;} else{ goto _LL1103;} _LL1103: if( _temp1097 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL1104;} else{ goto _LL1105;} _LL1105: goto _LL1106; _LL1100: call_conv=(
void*) Cyc_Absyn_Stdcall_att; goto _LL1098; _LL1102: call_conv=( void*) Cyc_Absyn_Cdecl_att;
goto _LL1098; _LL1104: call_conv=( void*) Cyc_Absyn_Fastcall_att; goto _LL1098;
_LL1106: goto _LL1098; _LL1098:;} goto _LL1088; _LL1092: goto _LL1088; _LL1088:;}}
Cyc_Absyndump_dumptq( _temp1085); Cyc_Absyndump_dumpntyp( _temp1083);{ struct
_tuple10 _temp1107=({ struct _tuple10 _temp1108; _temp1108.f1= call_conv;
_temp1108.f2= _temp1025; _temp1108;});(( void(*)( struct Cyc_List_List* tms,
void(* f)( struct _tuple10*), struct _tuple10* a)) Cyc_Absyndump_dumptms)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1081),
Cyc_Absyndump_dump_callconv_qvar,& _temp1107);}}}} else{ Cyc_Absyndump_dumpscope(
_temp1027);(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct _tuple0*),
struct _tuple0*)) Cyc_Absyndump_dumptqtd)( _temp1023, _temp1021, Cyc_Absyndump_dumpqvar,
_temp1025); Cyc_Absyndump_dumpatts( _temp1015);} if( _temp1019 != 0){ Cyc_Absyndump_dump_char((
int)'='); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null( _temp1019));}
Cyc_Absyndump_dump_semi(); goto _LL981; _LL991: Cyc_Absyndump_dumpscope(
_temp1040); if( _temp1032){ Cyc_Absyndump_dump( _tag_arr("xtunion ", sizeof(
unsigned char), 9u));} else{ Cyc_Absyndump_dump( _tag_arr("tunion ", sizeof(
unsigned char), 8u));} Cyc_Absyndump_dumpqvar( _temp1038); Cyc_Absyndump_dumptvars(
_temp1036); if( _temp1034 == 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumptunionfields(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1034))->v); Cyc_Absyndump_dump_nospace( _tag_arr("};", sizeof(
unsigned char), 3u));} goto _LL981; _LL993: Cyc_Absyndump_dumpscope( _temp1049);
Cyc_Absyndump_dump( _tag_arr("enum ", sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpqvar(
_temp1047); if( _temp1045 == 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpenumfields(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1045))->v); Cyc_Absyndump_dump_nospace( _tag_arr("};", sizeof(
unsigned char), 3u));} return; _LL995: Cyc_Absyndump_dump( _tag_arr("let",
sizeof( unsigned char), 4u)); Cyc_Absyndump_dumppat( _temp1059); Cyc_Absyndump_dump_char((
int)'='); Cyc_Absyndump_dumpexp( _temp1053); Cyc_Absyndump_dump_semi(); goto
_LL981; _LL997: Cyc_Absyndump_dump( _tag_arr("let ", sizeof( unsigned char), 5u));
Cyc_Absyndump_dumpids( _temp1061); Cyc_Absyndump_dump_semi(); goto _LL981;
_LL999: if( ! Cyc_Absyndump_expand_typedefs){ Cyc_Absyndump_dump( _tag_arr("typedef",
sizeof( unsigned char), 8u));(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)(
struct Cyc_Absyn_Typedefdecl*), struct Cyc_Absyn_Typedefdecl*)) Cyc_Absyndump_dumptqtd)(({
struct Cyc_Absyn_Tqual _temp1109; _temp1109.q_const= 0; _temp1109.q_volatile= 0;
_temp1109.q_restrict= 0; _temp1109;}),( void*) _temp1063->defn, Cyc_Absyndump_dumptypedefname,
_temp1063); Cyc_Absyndump_dump_semi();} goto _LL981; _LL1001: Cyc_Absyndump_dump(
_tag_arr("namespace", sizeof( unsigned char), 10u)); Cyc_Absyndump_dump_str(
_temp1067); Cyc_Absyndump_dump_char(( int)'{'); for( 0; _temp1065 != 0;
_temp1065=(( struct Cyc_List_List*) _check_null( _temp1065))->tl){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( _temp1065))->hd);}
Cyc_Absyndump_dump_char(( int)'}'); goto _LL981; _LL1003: Cyc_Absyndump_dump(
_tag_arr("using", sizeof( unsigned char), 6u)); Cyc_Absyndump_dumpqvar(
_temp1071); Cyc_Absyndump_dump_char(( int)'{'); for( 0; _temp1069 != 0;
_temp1069=(( struct Cyc_List_List*) _check_null( _temp1069))->tl){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( _temp1069))->hd);}
Cyc_Absyndump_dump_char(( int)'}'); goto _LL981; _LL1005: Cyc_Absyndump_dump(
_tag_arr("extern \"C\" {", sizeof( unsigned char), 13u)); for( 0; _temp1073 != 0;
_temp1073=(( struct Cyc_List_List*) _check_null( _temp1073))->tl){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( _temp1073))->hd);}
Cyc_Absyndump_dump_char(( int)'}'); goto _LL981; _LL981:;} static void Cyc_Absyndump_dump_upperbound(
struct Cyc_Absyn_Exp* e){ unsigned int i= Cyc_Evexp_eval_const_uint_exp( e); if(
i != 1){ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpexp( e); Cyc_Absyndump_dump_char((
int)'}');}} void Cyc_Absyndump_dumptms( struct Cyc_List_List* tms, void(* f)(
void*), void* a){ if( tms == 0){ f( a); return;}{ void* _temp1110=( void*)((
struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Tqual _temp1128;
void* _temp1130; void* _temp1132; struct Cyc_Absyn_Exp* _temp1134; struct Cyc_Absyn_Tqual
_temp1136; void* _temp1138; void* _temp1140; struct Cyc_Absyn_Exp* _temp1142;
struct Cyc_Absyn_Tqual _temp1144; void* _temp1146; void* _temp1148; struct Cyc_Absyn_Tqual
_temp1150; void* _temp1152; struct Cyc_Absyn_Tvar* _temp1154; void* _temp1156;
struct Cyc_Absyn_Exp* _temp1158; struct Cyc_Absyn_Tqual _temp1160; void*
_temp1162; struct Cyc_Absyn_Tvar* _temp1164; void* _temp1166; struct Cyc_Absyn_Exp*
_temp1168; struct Cyc_Absyn_Tqual _temp1170; void* _temp1172; struct Cyc_Absyn_Tvar*
_temp1174; void* _temp1176; struct Cyc_Absyn_Tqual _temp1178; void* _temp1180;
void* _temp1182; _LL1112: if(( unsigned int) _temp1110 > 1u?*(( int*) _temp1110)
== Cyc_Absyn_Pointer_mod: 0){ _LL1133: _temp1132=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1110)->f1; if(( unsigned int) _temp1132 > 1u?*(( int*) _temp1132) == Cyc_Absyn_Nullable_ps:
0){ _LL1135: _temp1134=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp1132)->f1;
goto _LL1131;} else{ goto _LL1114;} _LL1131: _temp1130=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1110)->f2; if( _temp1130 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1129;}
else{ goto _LL1114;} _LL1129: _temp1128=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1110)->f3; goto _LL1113;} else{ goto _LL1114;} _LL1114: if(( unsigned int)
_temp1110 > 1u?*(( int*) _temp1110) == Cyc_Absyn_Pointer_mod: 0){ _LL1141:
_temp1140=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1110)->f1; if((
unsigned int) _temp1140 > 1u?*(( int*) _temp1140) == Cyc_Absyn_NonNullable_ps: 0){
_LL1143: _temp1142=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1140)->f1;
goto _LL1139;} else{ goto _LL1116;} _LL1139: _temp1138=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1110)->f2; if( _temp1138 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1137;}
else{ goto _LL1116;} _LL1137: _temp1136=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1110)->f3; goto _LL1115;} else{ goto _LL1116;} _LL1116: if(( unsigned int)
_temp1110 > 1u?*(( int*) _temp1110) == Cyc_Absyn_Pointer_mod: 0){ _LL1149:
_temp1148=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1110)->f1; if(
_temp1148 ==( void*) Cyc_Absyn_TaggedArray_ps){ goto _LL1147;} else{ goto
_LL1118;} _LL1147: _temp1146=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1110)->f2; if( _temp1146 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1145;}
else{ goto _LL1118;} _LL1145: _temp1144=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1110)->f3; goto _LL1117;} else{ goto _LL1118;} _LL1118: if(( unsigned int)
_temp1110 > 1u?*(( int*) _temp1110) == Cyc_Absyn_Pointer_mod: 0){ _LL1157:
_temp1156=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1110)->f1; if((
unsigned int) _temp1156 > 1u?*(( int*) _temp1156) == Cyc_Absyn_Nullable_ps: 0){
_LL1159: _temp1158=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp1156)->f1; goto
_LL1153;} else{ goto _LL1120;} _LL1153: _temp1152=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1110)->f2; if(( unsigned int) _temp1152 > 4u?*(( int*) _temp1152) == Cyc_Absyn_VarType:
0){ _LL1155: _temp1154=(( struct Cyc_Absyn_VarType_struct*) _temp1152)->f1; goto
_LL1151;} else{ goto _LL1120;} _LL1151: _temp1150=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1110)->f3; goto _LL1119;} else{ goto _LL1120;} _LL1120: if(( unsigned int)
_temp1110 > 1u?*(( int*) _temp1110) == Cyc_Absyn_Pointer_mod: 0){ _LL1167:
_temp1166=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1110)->f1; if((
unsigned int) _temp1166 > 1u?*(( int*) _temp1166) == Cyc_Absyn_NonNullable_ps: 0){
_LL1169: _temp1168=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1166)->f1;
goto _LL1163;} else{ goto _LL1122;} _LL1163: _temp1162=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1110)->f2; if(( unsigned int) _temp1162 > 4u?*(( int*) _temp1162) == Cyc_Absyn_VarType:
0){ _LL1165: _temp1164=(( struct Cyc_Absyn_VarType_struct*) _temp1162)->f1; goto
_LL1161;} else{ goto _LL1122;} _LL1161: _temp1160=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1110)->f3; goto _LL1121;} else{ goto _LL1122;} _LL1122: if(( unsigned int)
_temp1110 > 1u?*(( int*) _temp1110) == Cyc_Absyn_Pointer_mod: 0){ _LL1177:
_temp1176=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1110)->f1; if(
_temp1176 ==( void*) Cyc_Absyn_TaggedArray_ps){ goto _LL1173;} else{ goto
_LL1124;} _LL1173: _temp1172=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1110)->f2; if(( unsigned int) _temp1172 > 4u?*(( int*) _temp1172) == Cyc_Absyn_VarType:
0){ _LL1175: _temp1174=(( struct Cyc_Absyn_VarType_struct*) _temp1172)->f1; goto
_LL1171;} else{ goto _LL1124;} _LL1171: _temp1170=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1110)->f3; goto _LL1123;} else{ goto _LL1124;} _LL1124: if(( unsigned int)
_temp1110 > 1u?*(( int*) _temp1110) == Cyc_Absyn_Pointer_mod: 0){ _LL1183:
_temp1182=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1110)->f1; goto
_LL1181; _LL1181: _temp1180=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1110)->f2; goto _LL1179; _LL1179: _temp1178=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1110)->f3; goto _LL1125;} else{ goto _LL1126;} _LL1126: goto _LL1127;
_LL1113: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dump_upperbound(
_temp1134);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f, a);
return; _LL1115: Cyc_Absyndump_dump_char(( int)'@'); Cyc_Absyndump_dump_upperbound(
_temp1142);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f, a);
return; _LL1117: Cyc_Absyndump_dump_char(( int)'?');(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*)
_check_null( tms))->tl, f, a); return; _LL1119: Cyc_Absyndump_dump_char(( int)'*');
Cyc_Absyndump_dump_upperbound( _temp1158); Cyc_Absyndump_dump_str( _temp1154->name);((
void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)(((
struct Cyc_List_List*) _check_null( tms))->tl, f, a); return; _LL1121: Cyc_Absyndump_dump_char((
int)'@'); Cyc_Absyndump_dump_upperbound( _temp1168); Cyc_Absyndump_dump_str(
_temp1164->name);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void*
a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f,
a); return; _LL1123: Cyc_Absyndump_dump_char(( int)'?'); Cyc_Absyndump_dump_str(
_temp1174->name);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void*
a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f,
a); return; _LL1125:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1184=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1184[ 0]=({ struct Cyc_Core_Impossible_struct _temp1185; _temp1185.tag= Cyc_Core_Impossible;
_temp1185.f1= _tag_arr("dumptms: bad Pointer_mod", sizeof( unsigned char), 25u);
_temp1185;}); _temp1184;})); _LL1127: { int next_is_pointer= 0; if((( struct Cyc_List_List*)
_check_null( tms))->tl != 0){ void* _temp1186=( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->hd; struct Cyc_Absyn_Tqual
_temp1192; void* _temp1194; void* _temp1196; _LL1188: if(( unsigned int)
_temp1186 > 1u?*(( int*) _temp1186) == Cyc_Absyn_Pointer_mod: 0){ _LL1197:
_temp1196=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1186)->f1; goto
_LL1195; _LL1195: _temp1194=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1186)->f2; goto _LL1193; _LL1193: _temp1192=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1186)->f3; goto _LL1189;} else{ goto _LL1190;} _LL1190: goto _LL1191;
_LL1189: next_is_pointer= 1; goto _LL1187; _LL1191: goto _LL1187; _LL1187:;} if(
next_is_pointer){ Cyc_Absyndump_dump_char(( int)'(');}(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*)
_check_null( tms))->tl, f, a); if( next_is_pointer){ Cyc_Absyndump_dump_char((
int)')');}{ void* _temp1198=( void*)(( struct Cyc_List_List*) _check_null( tms))->hd;
struct Cyc_Absyn_Exp* _temp1214; void* _temp1216; struct Cyc_List_List*
_temp1218; struct Cyc_Core_Opt* _temp1220; struct Cyc_Absyn_VarargInfo*
_temp1222; int _temp1224; struct Cyc_List_List* _temp1226; void* _temp1228;
struct Cyc_Position_Segment* _temp1230; struct Cyc_List_List* _temp1232; int
_temp1234; struct Cyc_Position_Segment* _temp1236; struct Cyc_List_List*
_temp1238; struct Cyc_List_List* _temp1240; struct Cyc_Position_Segment*
_temp1242; struct Cyc_Absyn_Tqual _temp1244; void* _temp1246; void* _temp1248;
_LL1200: if( _temp1198 ==( void*) Cyc_Absyn_Carray_mod){ goto _LL1201;} else{
goto _LL1202;} _LL1202: if(( unsigned int) _temp1198 > 1u?*(( int*) _temp1198)
== Cyc_Absyn_ConstArray_mod: 0){ _LL1215: _temp1214=(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp1198)->f1; goto _LL1203;} else{ goto _LL1204;} _LL1204: if(( unsigned int)
_temp1198 > 1u?*(( int*) _temp1198) == Cyc_Absyn_Function_mod: 0){ _LL1217:
_temp1216=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp1198)->f1; if(*((
int*) _temp1216) == Cyc_Absyn_WithTypes){ _LL1227: _temp1226=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1216)->f1; goto _LL1225; _LL1225: _temp1224=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1216)->f2; goto _LL1223; _LL1223: _temp1222=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1216)->f3; goto _LL1221; _LL1221: _temp1220=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1216)->f4; goto _LL1219; _LL1219: _temp1218=(( struct Cyc_Absyn_WithTypes_struct*)
_temp1216)->f5; goto _LL1205;} else{ goto _LL1206;}} else{ goto _LL1206;}
_LL1206: if(( unsigned int) _temp1198 > 1u?*(( int*) _temp1198) == Cyc_Absyn_Function_mod:
0){ _LL1229: _temp1228=( void*)(( struct Cyc_Absyn_Function_mod_struct*)
_temp1198)->f1; if(*(( int*) _temp1228) == Cyc_Absyn_NoTypes){ _LL1233:
_temp1232=(( struct Cyc_Absyn_NoTypes_struct*) _temp1228)->f1; goto _LL1231;
_LL1231: _temp1230=(( struct Cyc_Absyn_NoTypes_struct*) _temp1228)->f2; goto
_LL1207;} else{ goto _LL1208;}} else{ goto _LL1208;} _LL1208: if(( unsigned int)
_temp1198 > 1u?*(( int*) _temp1198) == Cyc_Absyn_TypeParams_mod: 0){ _LL1239:
_temp1238=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp1198)->f1; goto
_LL1237; _LL1237: _temp1236=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1198)->f2; goto _LL1235; _LL1235: _temp1234=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1198)->f3; goto _LL1209;} else{ goto _LL1210;} _LL1210: if(( unsigned int)
_temp1198 > 1u?*(( int*) _temp1198) == Cyc_Absyn_Attributes_mod: 0){ _LL1243:
_temp1242=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp1198)->f1; goto
_LL1241; _LL1241: _temp1240=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp1198)->f2; goto _LL1211;} else{ goto _LL1212;} _LL1212: if(( unsigned int)
_temp1198 > 1u?*(( int*) _temp1198) == Cyc_Absyn_Pointer_mod: 0){ _LL1249:
_temp1248=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1198)->f1; goto
_LL1247; _LL1247: _temp1246=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1198)->f2; goto _LL1245; _LL1245: _temp1244=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1198)->f3; goto _LL1213;} else{ goto _LL1199;} _LL1201: Cyc_Absyndump_dump(
_tag_arr("[]", sizeof( unsigned char), 3u)); goto _LL1199; _LL1203: Cyc_Absyndump_dump_char((
int)'['); Cyc_Absyndump_dumpexp( _temp1214); Cyc_Absyndump_dump_char(( int)']');
goto _LL1199; _LL1205: Cyc_Absyndump_dumpfunargs( _temp1226, _temp1224,
_temp1222, _temp1220, _temp1218); goto _LL1199; _LL1207:(( void(*)( void(* f)(
struct _tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr start, struct
_tagged_arr end, struct _tagged_arr sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dump_str,
_temp1232, _tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof(
unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u)); goto _LL1199;
_LL1209: if( _temp1234){ Cyc_Absyndump_dumpkindedtvars( _temp1238);} else{ Cyc_Absyndump_dumptvars(
_temp1238);} goto _LL1199; _LL1211: Cyc_Absyndump_dumpatts( _temp1240); goto
_LL1199; _LL1213:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1250=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1250[ 0]=({ struct Cyc_Core_Impossible_struct _temp1251; _temp1251.tag= Cyc_Core_Impossible;
_temp1251.f1= _tag_arr("dumptms", sizeof( unsigned char), 8u); _temp1251;});
_temp1250;})); _LL1199:;} return;} _LL1111:;}} void Cyc_Absyndump_dumptqtd(
struct Cyc_Absyn_Tqual tq, void* t, void(* f)( void*), void* a){ struct Cyc_List_List*
_temp1254; void* _temp1256; struct Cyc_Absyn_Tqual _temp1258; struct _tuple4
_temp1252= Cyc_Absynpp_to_tms( tq, t); _LL1259: _temp1258= _temp1252.f1; goto
_LL1257; _LL1257: _temp1256= _temp1252.f2; goto _LL1255; _LL1255: _temp1254=
_temp1252.f3; goto _LL1253; _LL1253: Cyc_Absyndump_dumptq( _temp1258); Cyc_Absyndump_dumpntyp(
_temp1256);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)((( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( _temp1254), f, a);} void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE* f){ Cyc_Absyndump_pos= 0;*
Cyc_Absyndump_dump_file= f; for( 0; tdl != 0; tdl=(( struct Cyc_List_List*)
_check_null( tdl))->tl){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( tdl))->hd);}}