 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_dirent{ int d_ino; int d_off; unsigned short
d_reclen; unsigned char d_type; unsigned char d_name[ 256u]; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern void*
Cyc_Core_identity( void*); extern unsigned char Cyc_Core_InvalidArg[ 15u];
struct Cyc_Core_InvalidArg_struct{ unsigned char* tag; struct _tagged_arr f1; }
; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); extern int system( unsigned char*); extern int*
__errno(); struct Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[
19u]; extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_String_pa=
0; struct Cyc_Stdio_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_Int_pa= 1; struct Cyc_Stdio_Int_pa_struct{ int tag;
unsigned int f1; } ; static const int Cyc_Stdio_Double_pa= 2; struct Cyc_Stdio_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_Stdio_fprintf( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); extern struct _tagged_arr Cyc_Stdio_aprintf(
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_Stdio_ShortPtr_sa=
0; struct Cyc_Stdio_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_Stdio_UShortPtr_sa= 1; struct Cyc_Stdio_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_Stdio_IntPtr_sa= 2; struct Cyc_Stdio_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_Stdio_UIntPtr_sa= 3; struct Cyc_Stdio_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Stdio_StringPtr_sa= 4;
struct Cyc_Stdio_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_DoublePtr_sa= 5; struct Cyc_Stdio_DoublePtr_sa_struct{ int
tag; double* f1; } ; static const int Cyc_Stdio_FloatPtr_sa= 6; struct Cyc_Stdio_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; extern unsigned char Cyc_List_List_empty[ 15u]; extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern unsigned char
Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Lineno_Pos{
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
21; static const int Cyc_Absyn_Aprintf= 22; static const int Cyc_Absyn_Scanf= 23;
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
15u]; extern int Cyc_Absyn_qvar_cmp( struct _tuple0*, struct _tuple0*); extern
int Cyc_Absyn_varlist_cmp( struct Cyc_List_List*, struct Cyc_List_List*); extern
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref( void* x); extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t;
extern void* Cyc_Absyn_sint_t; extern struct _tuple0* Cyc_Absyn_exn_name; extern
struct Cyc_Absyn_Tuniondecl* Cyc_Absyn_exn_tud; extern struct _tuple0* Cyc_Absyn_null_pointer_exn_name;
extern struct _tuple0* Cyc_Absyn_match_exn_name; extern struct Cyc_Absyn_Tunionfield*
Cyc_Absyn_null_pointer_exn_tuf; extern struct Cyc_Absyn_Tunionfield* Cyc_Absyn_match_exn_tuf;
extern void* Cyc_Absyn_tagged_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual tq);
extern struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple0* x,
void* t, struct Cyc_Absyn_Exp* init); extern void* Cyc_Absyn_function_typ(
struct Cyc_List_List* tvs, struct Cyc_Core_Opt* eff_typ, void* ret_typ, struct
Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo* cyc_varargs,
struct Cyc_List_List* rgn_po, struct Cyc_List_List*); struct Cyc_Set_Set; extern
unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; extern unsigned char
Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u]; extern
struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* comp)( void*, void*)); extern int
Cyc_Dict_member( struct Cyc_Dict_Dict* d, void* key); extern struct Cyc_Dict_Dict*
Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key, void* data); extern void*
Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key); extern void* Cyc_Dict_fold(
void*(* f)( void*, void*, void*), struct Cyc_Dict_Dict* d, void* accum); extern
void Cyc_Dict_iter_c( void(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict*
d); extern struct Cyc_Dict_Dict* Cyc_Dict_map( void*(* f)( void*), struct Cyc_Dict_Dict*
d); static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int
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
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init(); struct Cyc_Interface_I;
extern struct Cyc_Interface_I* Cyc_Interface_empty(); extern struct Cyc_Interface_I*
Cyc_Interface_final(); extern struct Cyc_Interface_I* Cyc_Interface_extract(
struct Cyc_Dict_Dict* ae); struct _tuple3{ struct _tagged_arr f1; struct
_tagged_arr f2; } ; extern int Cyc_Interface_is_subinterface( struct Cyc_Interface_I*
i1, struct Cyc_Interface_I* i2, struct _tuple3* info); extern struct Cyc_Interface_I*
Cyc_Interface_merge( struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2,
struct _tuple3* info); extern struct Cyc_Interface_I* Cyc_Interface_merge_list(
struct Cyc_List_List* li, struct Cyc_List_List* linfo); extern struct Cyc_Interface_I*
Cyc_Interface_get_and_merge_list( struct Cyc_Interface_I*(* get)( void*), struct
Cyc_List_List* la, struct Cyc_List_List* linfo); extern void Cyc_Interface_print(
struct Cyc_Interface_I*, struct Cyc_Stdio___sFILE*); extern struct Cyc_Interface_I*
Cyc_Interface_parse( struct Cyc_Stdio___sFILE*); extern void Cyc_Interface_save(
struct Cyc_Interface_I*, struct Cyc_Stdio___sFILE*); extern struct Cyc_Interface_I*
Cyc_Interface_load( struct Cyc_Stdio___sFILE*); struct Cyc_PP_Ppstate; struct
Cyc_PP_Out; struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{ int expand_typedefs: 1;
int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1;
int rewrite_temp_tvars: 1; int print_all_tvars: 1; int print_all_kinds: 1; int
print_using_stmts: 1; int print_externC_stmts: 1; int print_full_evars: 1; int
use_curr_namespace: 1; struct Cyc_List_List* curr_namespace; } ; extern void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params* fs); extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;
extern void Cyc_Absynpp_decllist2file( struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE*
f); extern struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple0*); extern
void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int var_default_init, struct Cyc_List_List*
ds); extern unsigned char Cyc_Lexing_Error[ 10u]; struct Cyc_Lexing_Error_struct{
unsigned char* tag; struct _tagged_arr f1; } ; struct Cyc_Lexing_lexbuf{ void(*
refill_buff)( struct Cyc_Lexing_lexbuf*); void* refill_state; struct _tagged_arr
lex_buffer; int lex_buffer_len; int lex_abs_pos; int lex_start_pos; int
lex_curr_pos; int lex_last_pos; int lex_last_action; int lex_eof_reached; } ;
struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_arr,
int, void*); void* read_fun_state; } ; struct Cyc_Lexing_lex_tables{ struct
_tagged_arr lex_base; struct _tagged_arr lex_backtrk; struct _tagged_arr
lex_default; struct _tagged_arr lex_trans; struct _tagged_arr lex_check; } ;
extern struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE* f);
struct Cyc_Declaration_spec; struct Cyc_Declarator; struct Cyc_Abstractdeclarator;
extern unsigned char Cyc_AbstractDeclarator_tok[ 27u]; struct Cyc_AbstractDeclarator_tok_struct{
unsigned char* tag; struct Cyc_Abstractdeclarator* f1; } ; extern unsigned char
Cyc_AttributeList_tok[ 22u]; struct Cyc_AttributeList_tok_struct{ unsigned char*
tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_Attribute_tok[ 18u];
struct Cyc_Attribute_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_BlockItem_tok[ 18u]; struct Cyc_BlockItem_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Bool_tok[ 13u];
struct Cyc_Bool_tok_struct{ unsigned char* tag; int f1; } ; extern unsigned char
Cyc_Char_tok[ 13u]; struct Cyc_Char_tok_struct{ unsigned char* tag;
unsigned char f1; } ; extern unsigned char Cyc_DeclList_tok[ 17u]; struct Cyc_DeclList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_DeclSpec_tok[
17u]; struct Cyc_DeclSpec_tok_struct{ unsigned char* tag; struct Cyc_Declaration_spec*
f1; } ; extern unsigned char Cyc_DeclaratorExpoptList_tok[ 29u]; struct Cyc_DeclaratorExpoptList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_DeclaratorExpopt_tok[
25u]; struct _tuple4{ struct Cyc_Declarator* f1; struct Cyc_Core_Opt* f2; } ;
struct Cyc_DeclaratorExpopt_tok_struct{ unsigned char* tag; struct _tuple4* f1;
} ; extern unsigned char Cyc_Declarator_tok[ 19u]; struct Cyc_Declarator_tok_struct{
unsigned char* tag; struct Cyc_Declarator* f1; } ; extern unsigned char Cyc_DesignatorList_tok[
23u]; struct Cyc_DesignatorList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Designator_tok[ 19u]; struct Cyc_Designator_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_EnumfieldList_tok[ 22u];
struct Cyc_EnumfieldList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Enumfield_tok[ 18u]; struct Cyc_Enumfield_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Enumfield* f1; } ; extern unsigned char Cyc_ExpList_tok[
16u]; struct Cyc_ExpList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Exp_tok[ 12u]; struct Cyc_Exp_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Exp* f1; } ; extern unsigned char Cyc_FieldPatternList_tok[
25u]; struct Cyc_FieldPatternList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_FieldPattern_tok[ 21u]; struct _tuple5{ struct
Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; struct Cyc_FieldPattern_tok_struct{
unsigned char* tag; struct _tuple5* f1; } ; extern unsigned char Cyc_FnDecl_tok[
15u]; struct Cyc_FnDecl_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Fndecl*
f1; } ; extern unsigned char Cyc_IdList_tok[ 15u]; struct Cyc_IdList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_InitDeclList_tok[
21u]; struct Cyc_InitDeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_InitDecl_tok[ 17u]; struct _tuple6{ struct Cyc_Declarator*
f1; struct Cyc_Absyn_Exp* f2; } ; struct Cyc_InitDecl_tok_struct{ unsigned char*
tag; struct _tuple6* f1; } ; extern unsigned char Cyc_InitializerList_tok[ 24u];
struct Cyc_InitializerList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Int_tok[ 12u]; struct _tuple7{ void* f1; int f2;
} ; struct Cyc_Int_tok_struct{ unsigned char* tag; struct _tuple7* f1; } ;
extern unsigned char Cyc_Kind_tok[ 13u]; struct Cyc_Kind_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Okay_tok[ 13u];
extern unsigned char Cyc_ParamDeclListBool_tok[ 26u]; struct _tuple8{ struct Cyc_List_List*
f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt* f4; struct Cyc_List_List*
f5; } ; struct Cyc_ParamDeclListBool_tok_struct{ unsigned char* tag; struct
_tuple8* f1; } ; extern unsigned char Cyc_ParamDeclList_tok[ 22u]; struct Cyc_ParamDeclList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_ParamDecl_tok[
18u]; struct Cyc_ParamDecl_tok_struct{ unsigned char* tag; struct _tuple1* f1; }
; extern unsigned char Cyc_PatternList_tok[ 20u]; struct Cyc_PatternList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_Pattern_tok[
16u]; struct Cyc_Pattern_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Pat*
f1; } ; extern unsigned char Cyc_Pointer_Sort_tok[ 21u]; struct Cyc_Pointer_Sort_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Primop_tok[ 15u];
struct Cyc_Primop_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_Primopopt_tok[ 18u]; struct Cyc_Primopopt_tok_struct{
unsigned char* tag; struct Cyc_Core_Opt* f1; } ; extern unsigned char Cyc_QualId_tok[
15u]; struct Cyc_QualId_tok_struct{ unsigned char* tag; struct _tuple0* f1; } ;
extern unsigned char Cyc_QualSpecList_tok[ 21u]; struct _tuple9{ struct Cyc_Absyn_Tqual
f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; } ; struct Cyc_QualSpecList_tok_struct{
unsigned char* tag; struct _tuple9* f1; } ; extern unsigned char Cyc_Rgn_tok[ 12u];
struct Cyc_Rgn_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_Rgnorder_tok[ 17u]; struct Cyc_Rgnorder_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_Scope_tok[
14u]; struct Cyc_Scope_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_Short_tok[ 14u]; struct Cyc_Short_tok_struct{ unsigned char*
tag; short f1; } ; extern unsigned char Cyc_Stmt_tok[ 13u]; struct Cyc_Stmt_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Stmt* f1; } ; extern unsigned char Cyc_StorageClass_tok[
21u]; struct Cyc_StorageClass_tok_struct{ unsigned char* tag; void* f1; } ;
extern unsigned char Cyc_String_tok[ 15u]; struct Cyc_String_tok_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Stringopt_tok[
18u]; struct Cyc_Stringopt_tok_struct{ unsigned char* tag; struct Cyc_Core_Opt*
f1; } ; extern unsigned char Cyc_StructFieldDeclListList_tok[ 32u]; struct Cyc_StructFieldDeclListList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_StructFieldDeclList_tok[
28u]; struct Cyc_StructFieldDeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_StructOrUnion_tok[ 22u]; struct Cyc_StructOrUnion_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_SwitchCClauseList_tok[
26u]; struct Cyc_SwitchCClauseList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_SwitchClauseList_tok[ 25u]; struct Cyc_SwitchClauseList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_TunionFieldList_tok[
24u]; struct Cyc_TunionFieldList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_TunionField_tok[ 20u]; struct Cyc_TunionField_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Tunionfield* f1; } ; extern unsigned char
Cyc_TypeList_tok[ 17u]; struct Cyc_TypeList_tok_struct{ unsigned char* tag;
struct Cyc_List_List* f1; } ; extern unsigned char Cyc_TypeModifierList_tok[ 25u];
struct Cyc_TypeModifierList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_TypeOpt_tok[ 16u]; struct Cyc_TypeOpt_tok_struct{
unsigned char* tag; struct Cyc_Core_Opt* f1; } ; extern unsigned char Cyc_TypeQual_tok[
17u]; struct Cyc_TypeQual_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Tqual
f1; } ; extern unsigned char Cyc_TypeSpecifier_tok[ 22u]; struct Cyc_TypeSpecifier_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Type_tok[ 13u];
struct Cyc_Type_tok_struct{ unsigned char* tag; void* f1; } ; struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
} ; extern int Cyc_String_strptrcmp( struct _tagged_arr* s1, struct _tagged_arr*
s2); extern int Cyc_String_zstrptrcmp( struct _tagged_arr*, struct _tagged_arr*);
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_arr); extern void* Cyc_Tcutil_compress(
void* t); extern int Cyc_Tcutil_is_function_type( void* t); extern unsigned char
Cyc_Tcdecl_Incompatible[ 17u]; struct Cyc_Tcdecl_Xtunionfielddecl{ struct Cyc_Absyn_Tuniondecl*
base; struct Cyc_Absyn_Tunionfield* field; } ; extern void Cyc_Tcdecl_merr(
struct Cyc_Position_Segment* loc, struct _tagged_arr* msg0, struct _tagged_arr
msg1); extern struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_structdecl( struct
Cyc_Absyn_Structdecl* d0, struct Cyc_Absyn_Structdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Uniondecl* Cyc_Tcdecl_merge_uniondecl(
struct Cyc_Absyn_Uniondecl* d0, struct Cyc_Absyn_Uniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_merge_tuniondecl(
struct Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Enumdecl* Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl* d0, struct Cyc_Absyn_Enumdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Vardecl* Cyc_Tcdecl_merge_vardecl(
struct Cyc_Absyn_Vardecl* d0, struct Cyc_Absyn_Vardecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Typedefdecl* Cyc_Tcdecl_merge_typedefdecl(
struct Cyc_Absyn_Typedefdecl* d0, struct Cyc_Absyn_Typedefdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Tcdecl_Xtunionfielddecl* Cyc_Tcdecl_merge_xtunionfielddecl(
struct Cyc_Tcdecl_Xtunionfielddecl* d0, struct Cyc_Tcdecl_Xtunionfielddecl* d1,
struct Cyc_Position_Segment* loc, struct _tagged_arr* msg); extern void Cyc_Lex_lex_init();
struct Cyc_Interface_Ienv{ struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict*
uniondecls; struct Cyc_Dict_Dict* tuniondecls; struct Cyc_Dict_Dict* enumdecls;
struct Cyc_Dict_Dict* typedefdecls; struct Cyc_Dict_Dict* vardecls; struct Cyc_Dict_Dict*
xtunionfielddecls; } ; struct Cyc_Interface_I{ struct Cyc_Dict_Dict* imports;
struct Cyc_Dict_Dict* exports; } ; static struct Cyc_Interface_Ienv* Cyc_Interface_new_ienv(){
return({ struct Cyc_Interface_Ienv* _temp0=( struct Cyc_Interface_Ienv*)
GC_malloc( sizeof( struct Cyc_Interface_Ienv)); _temp0->structdecls=(( struct
Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp0->uniondecls=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp0->tuniondecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp0->enumdecls=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp0->typedefdecls=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp0->vardecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*,
struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp0->xtunionfielddecls=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp0;});} static struct Cyc_Interface_Ienv*
Cyc_Interface_lazy_new_ienv(){ static struct Cyc_Interface_Ienv* ie= 0; if( ie
== 0){ ie=( struct Cyc_Interface_Ienv*) Cyc_Interface_new_ienv();} return(
struct Cyc_Interface_Ienv*) _check_null( ie);} static struct Cyc_Interface_Ienv*
Cyc_Interface_skel2i_f( void* ignored){ return Cyc_Interface_new_ienv();} static
struct Cyc_Interface_I* Cyc_Interface_skel2i( struct Cyc_Dict_Dict* skel){
return({ struct Cyc_Interface_I* _temp1=( struct Cyc_Interface_I*) GC_malloc(
sizeof( struct Cyc_Interface_I)); _temp1->imports=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Interface_Ienv*(* f)( void*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)(
Cyc_Interface_skel2i_f, skel); _temp1->exports=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Interface_Ienv*(* f)( void*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)(
Cyc_Interface_skel2i_f, skel); _temp1;});} struct Cyc_Interface_I* Cyc_Interface_empty(){
return({ struct Cyc_Interface_I* _temp2=( struct Cyc_Interface_I*) GC_malloc(
sizeof( struct Cyc_Interface_I)); _temp2->imports=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)(
Cyc_Absyn_varlist_cmp); _temp2->exports=(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp);
_temp2;});} struct Cyc_Interface_I* Cyc_Interface_final(){ struct Cyc_Interface_I*
_temp3= Cyc_Interface_empty(); struct Cyc_Interface_Ienv* _temp4= Cyc_Interface_new_ienv();
struct Cyc_Absyn_Tuniondecl* _temp5=({ struct Cyc_Absyn_Tuniondecl* _temp23=(
struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp23[ 0]=* Cyc_Absyn_exn_tud; _temp23;});( void*)( _temp5->sc=( void*)(( void*)
Cyc_Absyn_Public)); _temp4->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl* data)) Cyc_Dict_insert)(
_temp4->tuniondecls,(* Cyc_Absyn_exn_name).f2, _temp5);{ struct Cyc_Absyn_Tunionfield*
_temp6=({ struct Cyc_Absyn_Tunionfield* _temp22=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp22[ 0]=* Cyc_Absyn_null_pointer_exn_tuf;
_temp22;});( void*)( _temp6->sc=( void*)(( void*) Cyc_Absyn_Public));{ struct
Cyc_Tcdecl_Xtunionfielddecl* _temp7=({ struct Cyc_Tcdecl_Xtunionfielddecl*
_temp21=( struct Cyc_Tcdecl_Xtunionfielddecl*) GC_malloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl));
_temp21->base= _temp5; _temp21->field= _temp6; _temp21;}); _temp4->xtunionfielddecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Tcdecl_Xtunionfielddecl* data)) Cyc_Dict_insert)( _temp4->xtunionfielddecls,(*
Cyc_Absyn_null_pointer_exn_name).f2, _temp7);{ struct Cyc_Absyn_Tunionfield*
_temp8=({ struct Cyc_Absyn_Tunionfield* _temp20=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp20[ 0]=* Cyc_Absyn_match_exn_tuf;
_temp20;});( void*)( _temp8->sc=( void*)(( void*) Cyc_Absyn_Public));{ struct
Cyc_Tcdecl_Xtunionfielddecl* _temp9=({ struct Cyc_Tcdecl_Xtunionfielddecl*
_temp19=( struct Cyc_Tcdecl_Xtunionfielddecl*) GC_malloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl));
_temp19->base= _temp5; _temp19->field= _temp8; _temp19;}); _temp4->xtunionfielddecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Tcdecl_Xtunionfielddecl* data)) Cyc_Dict_insert)( _temp4->xtunionfielddecls,(*
Cyc_Absyn_match_exn_name).f2, _temp9); _temp3->exports=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Interface_Ienv*
data)) Cyc_Dict_insert)( _temp3->exports, 0, _temp4);{ struct Cyc_Interface_Ienv*
_temp10= Cyc_Interface_new_ienv(); struct _tuple0* qmain=({ struct _tuple0*
_temp16=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp16->f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp17=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp17[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp18; _temp18.tag= Cyc_Absyn_Abs_n; _temp18.f1= 0; _temp18;}); _temp17;});
_temp16->f2= _init_tag_arr(( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)),"main", sizeof( unsigned char), 5u); _temp16;}); struct Cyc_Absyn_Vardecl*
_temp11= Cyc_Absyn_new_vardecl( qmain, Cyc_Absyn_function_typ( 0, 0, Cyc_Absyn_sint_t,({
struct Cyc_List_List* _temp12=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp12->hd=( void*)({ struct _tuple1* _temp15=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp15->f1= 0; _temp15->f2= Cyc_Absyn_empty_tqual();
_temp15->f3= Cyc_Absyn_uint_t; _temp15;}); _temp12->tl=({ struct Cyc_List_List*
_temp13=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp13->hd=( void*)({ struct _tuple1* _temp14=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp14->f1= 0; _temp14->f2= Cyc_Absyn_empty_tqual();
_temp14->f3= Cyc_Absyn_tagged_typ( Cyc_Absyn_tagged_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); _temp14;}); _temp13->tl= 0; _temp13;}); _temp12;}), 0,
0, 0, 0), 0);( void*)( _temp11->sc=( void*)(( void*) Cyc_Absyn_Extern)); _temp10->vardecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Vardecl* data)) Cyc_Dict_insert)( _temp10->vardecls,(* qmain).f2,
_temp11); _temp3->imports=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(
_temp3->exports, 0, _temp10); return _temp3;}}}}}} static void Cyc_Interface_err(
struct _tagged_arr msg){ Cyc_Tcutil_terr( 0, msg);} static void Cyc_Interface_fields_err(
struct _tagged_arr sc, struct _tagged_arr t, struct _tuple0* n){ Cyc_Interface_err((
struct _tagged_arr)({ struct _tagged_arr(* _temp24)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp30= _tag_arr("fields of %s %s %s have never been defined",
sizeof( unsigned char), 43u); struct Cyc_Stdio_String_pa_struct _temp33; _temp33.tag=
Cyc_Stdio_String_pa; _temp33.f1=( struct _tagged_arr) sc;{ void* _temp27=( void*)&
_temp33; struct Cyc_Stdio_String_pa_struct _temp32; _temp32.tag= Cyc_Stdio_String_pa;
_temp32.f1=( struct _tagged_arr) t;{ void* _temp28=( void*)& _temp32; struct Cyc_Stdio_String_pa_struct
_temp31; _temp31.tag= Cyc_Stdio_String_pa; _temp31.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
n);{ void* _temp29=( void*)& _temp31; void* _temp25[ 3u]={ _temp27, _temp28,
_temp29}; struct _tagged_arr _temp26={( void*) _temp25,( void*) _temp25,( void*)(
_temp25 + 3u)}; _temp24( _temp30, _temp26);}}}}));} static void Cyc_Interface_body_err(
struct _tagged_arr sc, struct _tuple0* n){ Cyc_Interface_err(( struct
_tagged_arr)({ struct _tagged_arr(* _temp34)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp39= _tag_arr("the body of %s function %s has never been defined",
sizeof( unsigned char), 50u); struct Cyc_Stdio_String_pa_struct _temp41; _temp41.tag=
Cyc_Stdio_String_pa; _temp41.f1=( struct _tagged_arr) sc;{ void* _temp37=( void*)&
_temp41; struct Cyc_Stdio_String_pa_struct _temp40; _temp40.tag= Cyc_Stdio_String_pa;
_temp40.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( n);{ void* _temp38=(
void*)& _temp40; void* _temp35[ 2u]={ _temp37, _temp38}; struct _tagged_arr
_temp36={( void*) _temp35,( void*) _temp35,( void*)( _temp35 + 2u)}; _temp34(
_temp39, _temp36);}}}));} static void Cyc_Interface_static_err( struct
_tagged_arr obj1, struct _tuple0* name1, struct _tagged_arr obj2, struct _tuple0*
name2){ if( obj1.curr !=(( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ Cyc_Interface_err((
struct _tagged_arr)({ struct _tagged_arr(* _temp42)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp49= _tag_arr("declaration of %s %s relies on static %s %s",
sizeof( unsigned char), 44u); struct Cyc_Stdio_String_pa_struct _temp53; _temp53.tag=
Cyc_Stdio_String_pa; _temp53.f1=( struct _tagged_arr) obj1;{ void* _temp45=(
void*)& _temp53; struct Cyc_Stdio_String_pa_struct _temp52; _temp52.tag= Cyc_Stdio_String_pa;
_temp52.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( name1);{ void* _temp46=(
void*)& _temp52; struct Cyc_Stdio_String_pa_struct _temp51; _temp51.tag= Cyc_Stdio_String_pa;
_temp51.f1=( struct _tagged_arr) obj2;{ void* _temp47=( void*)& _temp51; struct
Cyc_Stdio_String_pa_struct _temp50; _temp50.tag= Cyc_Stdio_String_pa; _temp50.f1=(
struct _tagged_arr) Cyc_Absynpp_qvar2string( name2);{ void* _temp48=( void*)&
_temp50; void* _temp43[ 4u]={ _temp45, _temp46, _temp47, _temp48}; struct
_tagged_arr _temp44={( void*) _temp43,( void*) _temp43,( void*)( _temp43 + 4u)};
_temp42( _temp49, _temp44);}}}}}));}} static void Cyc_Interface_abstract_err(
struct _tagged_arr obj1, struct _tuple0* name1, struct _tagged_arr obj2, struct
_tuple0* name2){ if( obj1.curr !=(( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){
Cyc_Interface_err(( struct _tagged_arr)({ struct _tagged_arr(* _temp54)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp61= _tag_arr("declaration of %s %s relies on fields of abstract %s %s",
sizeof( unsigned char), 56u); struct Cyc_Stdio_String_pa_struct _temp65; _temp65.tag=
Cyc_Stdio_String_pa; _temp65.f1=( struct _tagged_arr) obj1;{ void* _temp57=(
void*)& _temp65; struct Cyc_Stdio_String_pa_struct _temp64; _temp64.tag= Cyc_Stdio_String_pa;
_temp64.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( name1);{ void* _temp58=(
void*)& _temp64; struct Cyc_Stdio_String_pa_struct _temp63; _temp63.tag= Cyc_Stdio_String_pa;
_temp63.f1=( struct _tagged_arr) obj2;{ void* _temp59=( void*)& _temp63; struct
Cyc_Stdio_String_pa_struct _temp62; _temp62.tag= Cyc_Stdio_String_pa; _temp62.f1=(
struct _tagged_arr) Cyc_Absynpp_qvar2string( name2);{ void* _temp60=( void*)&
_temp62; void* _temp55[ 4u]={ _temp57, _temp58, _temp59, _temp60}; struct
_tagged_arr _temp56={( void*) _temp55,( void*) _temp55,( void*)( _temp55 + 4u)};
_temp54( _temp61, _temp56);}}}}}));}} struct Cyc_Interface_Seen{ struct Cyc_Dict_Dict*
structs; struct Cyc_Dict_Dict* unions; struct Cyc_Dict_Dict* tunions; } ; static
struct Cyc_Interface_Seen* Cyc_Interface_new_seen(){ return({ struct Cyc_Interface_Seen*
_temp66=( struct Cyc_Interface_Seen*) GC_malloc( sizeof( struct Cyc_Interface_Seen));
_temp66->structs=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp66->unions=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp); _temp66->tunions=(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp66;});} static int Cyc_Interface_check_public_type( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0*
name, void* t); static int Cyc_Interface_check_public_type_list( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0*
name, void*(* f)( void*), struct Cyc_List_List* l){ int _temp67= 1; for( 0; l !=
0; l=(( struct Cyc_List_List*) _check_null( l))->tl){ if( ! Cyc_Interface_check_public_type(
ae, seen, obj, name, f(( void*)(( struct Cyc_List_List*) _check_null( l))->hd))){
_temp67= 0;}} return _temp67;} static int Cyc_Interface_check_public_structdecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Structdecl*
d){ struct _handler_cons _temp68; _push_handler(& _temp68);{ int _temp70= 0; if(
setjmp( _temp68.handler)){ _temp70= 1;} if( ! _temp70){{ int _temp71=(( int(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->structs,(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v);
_npop_handler( 0u); return _temp71;}; _pop_handler();} else{ void* _temp69=(
void*) _exn_thrown; void* _temp73= _temp69; _LL75: if( _temp73 == Cyc_Dict_Absent){
goto _LL76;} else{ goto _LL77;} _LL77: goto _LL78; _LL76: { int _temp79= 1; seen->structs=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)( seen->structs,( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( d->name))->v, _temp79); if( d->fields != 0){ struct Cyc_List_List*
_temp80=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( d->fields))->v;
for( 0; _temp80 != 0; _temp80=(( struct Cyc_List_List*) _check_null( _temp80))->tl){
if( ! Cyc_Interface_check_public_type( ae, seen, _tag_arr("struct", sizeof(
unsigned char), 7u),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v,(
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp80))->hd)->type)){ _temp79= 0;}}} seen->structs=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->structs,(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v, _temp79);
return _temp79;} _LL78:( void) _throw( _temp73); _LL74:;}}} static int Cyc_Interface_check_public_uniondecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Uniondecl*
d){ struct _handler_cons _temp81; _push_handler(& _temp81);{ int _temp83= 0; if(
setjmp( _temp81.handler)){ _temp83= 1;} if( ! _temp83){{ int _temp84=(( int(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->unions,(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v);
_npop_handler( 0u); return _temp84;}; _pop_handler();} else{ void* _temp82=(
void*) _exn_thrown; void* _temp86= _temp82; _LL88: if( _temp86 == Cyc_Dict_Absent){
goto _LL89;} else{ goto _LL90;} _LL90: goto _LL91; _LL89: { int _temp92= 1; seen->unions=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)( seen->unions,( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( d->name))->v, _temp92); if( d->fields != 0){ struct Cyc_List_List*
_temp93=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( d->fields))->v;
for( 0; _temp93 != 0; _temp93=(( struct Cyc_List_List*) _check_null( _temp93))->tl){
if( ! Cyc_Interface_check_public_type( ae, seen, _tag_arr("union", sizeof(
unsigned char), 6u),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v,(
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp93))->hd)->type)){ _temp92= 0;}}} seen->unions=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->unions,(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v, _temp92);
return _temp92;} _LL91:( void) _throw( _temp86); _LL87:;}}} struct _tuple10{
struct Cyc_Absyn_Tqual f1; void* f2; } ; static void* Cyc_Interface_get_type1(
struct _tuple10* x){ return(* x).f2;} static int Cyc_Interface_check_public_tuniondecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Tuniondecl*
d){ struct _handler_cons _temp94; _push_handler(& _temp94);{ int _temp96= 0; if(
setjmp( _temp94.handler)){ _temp96= 1;} if( ! _temp96){{ int _temp97=(( int(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->tunions,
d->name); _npop_handler( 0u); return _temp97;}; _pop_handler();} else{ void*
_temp95=( void*) _exn_thrown; void* _temp99= _temp95; _LL101: if( _temp99 == Cyc_Dict_Absent){
goto _LL102;} else{ goto _LL103;} _LL103: goto _LL104; _LL102: { int _temp105= 1;
seen->tunions=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tuple0* key, int data)) Cyc_Dict_insert)( seen->tunions, d->name, _temp105);
if( d->fields != 0){ struct Cyc_List_List* _temp106=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( d->fields))->v; for( 0; _temp106 != 0;
_temp106=(( struct Cyc_List_List*) _check_null( _temp106))->tl){ if( !(( int(*)(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr
obj, struct _tuple0* name, void*(* f)( struct _tuple10*), struct Cyc_List_List*
l)) Cyc_Interface_check_public_type_list)( ae, seen, _tag_arr("[x]tunion",
sizeof( unsigned char), 10u), d->name, Cyc_Interface_get_type1,(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp106))->hd)->typs)){ _temp105= 0;}}}
seen->tunions=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tuple0* key, int data)) Cyc_Dict_insert)( seen->tunions, d->name, _temp105);
return _temp105;} _LL104:( void) _throw( _temp99); _LL100:;}}} static int Cyc_Interface_check_public_enumdecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Enumdecl*
d){ return 1;} static int Cyc_Interface_check_public_typedefdecl( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Typedefdecl* d){ return
Cyc_Interface_check_public_type( ae, seen,( struct _tagged_arr) _tag_arr( 0u, 0u,
0u), d->name,( void*) d->defn);} static int Cyc_Interface_check_public_vardecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Vardecl*
d){ return Cyc_Interface_check_public_type( ae, seen, _tag_arr("variable",
sizeof( unsigned char), 9u), d->name,( void*) d->type);} static void* Cyc_Interface_get_type2(
struct _tuple1* x){ return(* x).f3;} static struct Cyc_List_List* Cyc_Interface_get_abs_ns(
void* ns){ void* _temp107= ns; struct Cyc_List_List* _temp113; _LL109: if((
unsigned int) _temp107 > 1u?*(( int*) _temp107) == Cyc_Absyn_Abs_n: 0){ _LL114:
_temp113=(( struct Cyc_Absyn_Abs_n_struct*) _temp107)->f1; goto _LL110;} else{
goto _LL111;} _LL111: goto _LL112; _LL110: return _temp113; _LL112:( int) _throw((
void*)({ struct Cyc_Core_InvalidArg_struct* _temp115=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp115[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp116; _temp116.tag= Cyc_Core_InvalidArg; _temp116.f1=
_tag_arr("get_abs_ns", sizeof( unsigned char), 11u); _temp116;}); _temp115;}));
_LL108:;} static int Cyc_Interface_check_public_type( struct Cyc_Dict_Dict* ae,
struct Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0* name,
void* t){ void* _temp117= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp141; void* _temp143; void* _temp145; struct Cyc_Core_Opt* _temp147; struct
Cyc_Core_Opt _temp149; void* _temp150; struct Cyc_Absyn_FnInfo _temp152; struct
Cyc_Absyn_VarargInfo* _temp154; struct Cyc_List_List* _temp156; void* _temp158;
struct Cyc_List_List* _temp160; struct Cyc_Absyn_Structdecl** _temp162; struct
Cyc_Absyn_Structdecl* _temp164; struct Cyc_List_List* _temp165; struct Cyc_Absyn_Uniondecl**
_temp167; struct Cyc_Absyn_Uniondecl* _temp169; struct Cyc_List_List* _temp170;
struct _tuple0* _temp172; struct Cyc_Absyn_TunionInfo _temp174; struct Cyc_List_List*
_temp176; void* _temp178; struct Cyc_Absyn_Tuniondecl* _temp180; struct Cyc_Absyn_TunionFieldInfo
_temp182; struct Cyc_List_List* _temp184; void* _temp186; struct Cyc_Absyn_Tunionfield*
_temp188; struct Cyc_Absyn_Tuniondecl* _temp190; _LL119: if(( unsigned int)
_temp117 > 4u?*(( int*) _temp117) == Cyc_Absyn_PointerType: 0){ _LL142: _temp141=((
struct Cyc_Absyn_PointerType_struct*) _temp117)->f1; _LL144: _temp143=( void*)
_temp141.elt_typ; goto _LL120;} else{ goto _LL121;} _LL121: if(( unsigned int)
_temp117 > 4u?*(( int*) _temp117) == Cyc_Absyn_ArrayType: 0){ _LL146: _temp145=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp117)->f1; goto _LL122;} else{
goto _LL123;} _LL123: if(( unsigned int) _temp117 > 4u?*(( int*) _temp117) ==
Cyc_Absyn_TypedefType: 0){ _LL148: _temp147=(( struct Cyc_Absyn_TypedefType_struct*)
_temp117)->f3; if( _temp147 == 0){ goto _LL125;} else{ _temp149=* _temp147;
_LL151: _temp150=( void*) _temp149.v; goto _LL124;}} else{ goto _LL125;} _LL125:
if(( unsigned int) _temp117 > 4u?*(( int*) _temp117) == Cyc_Absyn_FnType: 0){
_LL153: _temp152=(( struct Cyc_Absyn_FnType_struct*) _temp117)->f1; _LL159:
_temp158=( void*) _temp152.ret_typ; goto _LL157; _LL157: _temp156= _temp152.args;
goto _LL155; _LL155: _temp154= _temp152.cyc_varargs; goto _LL126;} else{ goto
_LL127;} _LL127: if(( unsigned int) _temp117 > 4u?*(( int*) _temp117) == Cyc_Absyn_TupleType:
0){ _LL161: _temp160=(( struct Cyc_Absyn_TupleType_struct*) _temp117)->f1; goto
_LL128;} else{ goto _LL129;} _LL129: if(( unsigned int) _temp117 > 4u?*(( int*)
_temp117) == Cyc_Absyn_StructType: 0){ _LL166: _temp165=(( struct Cyc_Absyn_StructType_struct*)
_temp117)->f2; goto _LL163; _LL163: _temp162=(( struct Cyc_Absyn_StructType_struct*)
_temp117)->f3; if( _temp162 == 0){ goto _LL131;} else{ _temp164=* _temp162; goto
_LL130;}} else{ goto _LL131;} _LL131: if(( unsigned int) _temp117 > 4u?*(( int*)
_temp117) == Cyc_Absyn_UnionType: 0){ _LL171: _temp170=(( struct Cyc_Absyn_UnionType_struct*)
_temp117)->f2; goto _LL168; _LL168: _temp167=(( struct Cyc_Absyn_UnionType_struct*)
_temp117)->f3; if( _temp167 == 0){ goto _LL133;} else{ _temp169=* _temp167; goto
_LL132;}} else{ goto _LL133;} _LL133: if(( unsigned int) _temp117 > 4u?*(( int*)
_temp117) == Cyc_Absyn_EnumType: 0){ _LL173: _temp172=(( struct Cyc_Absyn_EnumType_struct*)
_temp117)->f1; goto _LL134;} else{ goto _LL135;} _LL135: if(( unsigned int)
_temp117 > 4u?*(( int*) _temp117) == Cyc_Absyn_TunionType: 0){ _LL175: _temp174=((
struct Cyc_Absyn_TunionType_struct*) _temp117)->f1; _LL179: _temp178=( void*)
_temp174.tunion_info; if(*(( int*) _temp178) == Cyc_Absyn_KnownTunion){ _LL181:
_temp180=(( struct Cyc_Absyn_KnownTunion_struct*) _temp178)->f1; goto _LL177;}
else{ goto _LL137;} _LL177: _temp176= _temp174.targs; goto _LL136;} else{ goto
_LL137;} _LL137: if(( unsigned int) _temp117 > 4u?*(( int*) _temp117) == Cyc_Absyn_TunionFieldType:
0){ _LL183: _temp182=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp117)->f1;
_LL187: _temp186=( void*) _temp182.field_info; if(*(( int*) _temp186) == Cyc_Absyn_KnownTunionfield){
_LL191: _temp190=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp186)->f1;
goto _LL189; _LL189: _temp188=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp186)->f2; goto _LL185;} else{ goto _LL139;} _LL185: _temp184= _temp182.targs;
goto _LL138;} else{ goto _LL139;} _LL139: goto _LL140; _LL120: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp143); _LL122: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp145); _LL124: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp150); _LL126: { int b=(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0*
name, void*(* f)( struct _tuple1*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name, Cyc_Interface_get_type2, _temp156)? Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp158): 0; if( _temp154 != 0){ void* _temp194; struct
Cyc_Absyn_VarargInfo _temp192=*(( struct Cyc_Absyn_VarargInfo*) _check_null(
_temp154)); _LL195: _temp194=( void*) _temp192.type; goto _LL193; _LL193: b= Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp194);} return b;} _LL128: return(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_arr obj, struct _tuple0*
name, void*(* f)( struct _tuple10*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name, Cyc_Interface_get_type1, _temp160); _LL130: if(( void*)
_temp164->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err( obj, name,
_tag_arr("struct", sizeof( unsigned char), 7u),( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp164->name))->v); return 0;} return Cyc_Interface_check_public_type_list(
ae, seen, obj, name, Cyc_Core_identity, _temp165)? Cyc_Interface_check_public_structdecl(
ae, seen, _temp164): 0; _LL132: if(( void*) _temp169->sc ==( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, name, _tag_arr("union", sizeof( unsigned char), 6u),(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp169->name))->v);
return 0;} return Cyc_Interface_check_public_type_list( ae, seen, obj, name, Cyc_Core_identity,
_temp170)? Cyc_Interface_check_public_uniondecl( ae, seen, _temp169): 0; _LL134: {
struct _tuple0 _temp198; struct _tagged_arr* _temp199; void* _temp201; struct
_tuple0* _temp196= _temp172; _temp198=* _temp196; _LL202: _temp201= _temp198.f1;
goto _LL200; _LL200: _temp199= _temp198.f2; goto _LL197; _LL197: { struct Cyc_List_List*
_temp203= Cyc_Interface_get_abs_ns( _temp201); struct Cyc_Absyn_Enumdecl* ed;{
struct _handler_cons _temp204; _push_handler(& _temp204);{ int _temp206= 0; if(
setjmp( _temp204.handler)){ _temp206= 1;} if( ! _temp206){{ struct Cyc_Tcenv_Genv*
_temp207=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( ae, _temp203); ed=*(( struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp207->enumdecls,
_temp199);}; _pop_handler();} else{ void* _temp205=( void*) _exn_thrown; void*
_temp209= _temp205; _LL211: if( _temp209 == Cyc_Dict_Absent){ goto _LL212;}
else{ goto _LL213;} _LL213: goto _LL214; _LL212:( int) _throw(( void*)({ struct
Cyc_Core_InvalidArg_struct* _temp215=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp215[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp216; _temp216.tag= Cyc_Core_InvalidArg; _temp216.f1=(
struct _tagged_arr)({ struct _tagged_arr(* _temp217)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp221= _tag_arr("check_public_type (can't find enum %s)",
sizeof( unsigned char), 39u); struct Cyc_Stdio_String_pa_struct _temp222;
_temp222.tag= Cyc_Stdio_String_pa; _temp222.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp172);{ void* _temp220=( void*)& _temp222; void* _temp218[ 1u]={ _temp220};
struct _tagged_arr _temp219={( void*) _temp218,( void*) _temp218,( void*)(
_temp218 + 1u)}; _temp217( _temp221, _temp219);}}); _temp216;}); _temp215;}));
_LL214:( void) _throw( _temp209); _LL210:;}}} if(( void*) ed->sc ==( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, _temp172, _tag_arr("enum", sizeof( unsigned char),
5u), ed->name); return 0;} return 1;}} _LL136: { struct _tuple0 _temp225; struct
_tagged_arr* _temp226; void* _temp228; struct _tuple0* _temp223= _temp180->name;
_temp225=* _temp223; _LL229: _temp228= _temp225.f1; goto _LL227; _LL227:
_temp226= _temp225.f2; goto _LL224; _LL224: { struct Cyc_List_List* _temp230=
Cyc_Interface_get_abs_ns( _temp228); struct Cyc_Absyn_Tuniondecl* tud;{ struct
_handler_cons _temp231; _push_handler(& _temp231);{ int _temp233= 0; if( setjmp(
_temp231.handler)){ _temp233= 1;} if( ! _temp233){{ struct Cyc_Tcenv_Genv*
_temp234=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( ae, _temp230); tud=*(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp234->tuniondecls,
_temp226);}; _pop_handler();} else{ void* _temp232=( void*) _exn_thrown; void*
_temp236= _temp232; _LL238: if( _temp236 == Cyc_Dict_Absent){ goto _LL239;}
else{ goto _LL240;} _LL240: goto _LL241; _LL239:( int) _throw(( void*)({ struct
Cyc_Core_InvalidArg_struct* _temp242=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp242[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp243; _temp243.tag= Cyc_Core_InvalidArg; _temp243.f1=(
struct _tagged_arr)({ struct _tagged_arr(* _temp244)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp248= _tag_arr("check_public_type (can't find [x]tunion %s)",
sizeof( unsigned char), 44u); struct Cyc_Stdio_String_pa_struct _temp249;
_temp249.tag= Cyc_Stdio_String_pa; _temp249.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp180->name);{ void* _temp247=( void*)& _temp249; void* _temp245[ 1u]={
_temp247}; struct _tagged_arr _temp246={( void*) _temp245,( void*) _temp245,(
void*)( _temp245 + 1u)}; _temp244( _temp248, _temp246);}}); _temp243;});
_temp242;})); _LL241:( void) _throw( _temp236); _LL237:;}}} if(( void*) tud->sc
==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err( obj, name, _tag_arr("[x]tunion",
sizeof( unsigned char), 10u), tud->name); return 0;} return Cyc_Interface_check_public_type_list(
ae, seen, obj, name, Cyc_Core_identity, _temp176)? Cyc_Interface_check_public_tuniondecl(
ae, seen, tud): 0;}} _LL138: { struct _tuple0 _temp252; struct _tagged_arr*
_temp253; void* _temp255; struct _tuple0* _temp250= _temp190->name; _temp252=*
_temp250; _LL256: _temp255= _temp252.f1; goto _LL254; _LL254: _temp253= _temp252.f2;
goto _LL251; _LL251: { struct Cyc_List_List* _temp257= Cyc_Interface_get_abs_ns(
_temp255); struct Cyc_Absyn_Tuniondecl* tud;{ struct _handler_cons _temp258;
_push_handler(& _temp258);{ int _temp260= 0; if( setjmp( _temp258.handler)){
_temp260= 1;} if( ! _temp260){{ struct Cyc_Tcenv_Genv* _temp261=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( ae,
_temp257); tud=*(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp261->tuniondecls, _temp253);};
_pop_handler();} else{ void* _temp259=( void*) _exn_thrown; void* _temp263=
_temp259; _LL265: if( _temp263 == Cyc_Dict_Absent){ goto _LL266;} else{ goto
_LL267;} _LL267: goto _LL268; _LL266:( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp269=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp269[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp270; _temp270.tag= Cyc_Core_InvalidArg;
_temp270.f1=( struct _tagged_arr)({ struct _tagged_arr(* _temp271)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp275= _tag_arr("check_public_type (can't find [x]tunion %s and search its fields)",
sizeof( unsigned char), 66u); struct Cyc_Stdio_String_pa_struct _temp276;
_temp276.tag= Cyc_Stdio_String_pa; _temp276.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp190->name);{ void* _temp274=( void*)& _temp276; void* _temp272[ 1u]={
_temp274}; struct _tagged_arr _temp273={( void*) _temp272,( void*) _temp272,(
void*)( _temp272 + 1u)}; _temp271( _temp275, _temp273);}}); _temp270;});
_temp269;})); _LL268:( void) _throw( _temp263); _LL264:;}}} if( tud->fields == 0){(
int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp277=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp277[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp278; _temp278.tag= Cyc_Core_InvalidArg; _temp278.f1=(
struct _tagged_arr)({ struct _tagged_arr(* _temp279)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp283= _tag_arr("check_public_type ([x]tunion %s has no fields)",
sizeof( unsigned char), 47u); struct Cyc_Stdio_String_pa_struct _temp284;
_temp284.tag= Cyc_Stdio_String_pa; _temp284.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
tud->name);{ void* _temp282=( void*)& _temp284; void* _temp280[ 1u]={ _temp282};
struct _tagged_arr _temp281={( void*) _temp280,( void*) _temp280,( void*)(
_temp280 + 1u)}; _temp279( _temp283, _temp281);}}); _temp278;}); _temp277;}));}{
struct Cyc_Absyn_Tunionfield* tuf1= 0;{ struct Cyc_List_List* _temp285=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( tud->fields))->v; for( 0;
_temp285 != 0; _temp285=(( struct Cyc_List_List*) _check_null( _temp285))->tl){
if( Cyc_String_strptrcmp((* _temp188->name).f2,(*(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp285))->hd)->name).f2) == 0){ tuf1=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp285))->hd); break;}}} if( tuf1 == 0){( int) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp286=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp286[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp287; _temp287.tag= Cyc_Core_InvalidArg; _temp287.f1=(
struct _tagged_arr)({ struct _tagged_arr(* _temp288)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp292= _tag_arr("check_public_type (can't find [x]tunionfield %s)",
sizeof( unsigned char), 49u); struct Cyc_Stdio_String_pa_struct _temp293;
_temp293.tag= Cyc_Stdio_String_pa; _temp293.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp188->name);{ void* _temp291=( void*)& _temp293; void* _temp289[ 1u]={
_temp291}; struct _tagged_arr _temp290={( void*) _temp289,( void*) _temp289,(
void*)( _temp289 + 1u)}; _temp288( _temp292, _temp290);}}); _temp287;});
_temp286;}));}{ struct Cyc_Absyn_Tunionfield* tuf=( struct Cyc_Absyn_Tunionfield*)
_check_null( tuf1); if(( void*) tud->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err(
obj, name, _tag_arr("[x]tunion", sizeof( unsigned char), 10u), tud->name);
return 0;} if(( void*) tud->sc ==( void*) Cyc_Absyn_Abstract){ Cyc_Interface_abstract_err(
obj, name, _tag_arr("[x]tunion", sizeof( unsigned char), 10u), tud->name);
return 0;} if(( void*) tuf->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err(
obj, name,( struct _tagged_arr)({ struct _tagged_arr(* _temp294)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp298= _tag_arr("field %s of", sizeof( unsigned char), 12u); struct Cyc_Stdio_String_pa_struct
_temp299; _temp299.tag= Cyc_Stdio_String_pa; _temp299.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( tuf->name);{ void* _temp297=( void*)& _temp299; void*
_temp295[ 1u]={ _temp297}; struct _tagged_arr _temp296={( void*) _temp295,( void*)
_temp295,( void*)( _temp295 + 1u)}; _temp294( _temp298, _temp296);}}), tud->name);
return 0;} return Cyc_Interface_check_public_type_list( ae, seen, obj, name, Cyc_Core_identity,
_temp184)? Cyc_Interface_check_public_tuniondecl( ae, seen, tud): 0;}}}} _LL140:
return 1; _LL118:;} struct _tuple11{ struct Cyc_Interface_Ienv* f1; struct Cyc_Interface_Ienv*
f2; int f3; struct Cyc_Dict_Dict* f4; struct Cyc_Interface_Seen* f5; struct Cyc_Interface_I*
f6; } ; static void Cyc_Interface_extract_structdecl( struct _tuple11* env,
struct _tagged_arr* x, struct Cyc_Absyn_Structdecl** dp){ struct _tuple11
_temp302; struct Cyc_Interface_Seen* _temp303; struct Cyc_Dict_Dict* _temp305;
int _temp307; struct Cyc_Interface_Ienv* _temp309; struct Cyc_Interface_Ienv*
_temp311; struct _tuple11* _temp300= env; _temp302=* _temp300; _LL312: _temp311=
_temp302.f1; goto _LL310; _LL310: _temp309= _temp302.f2; goto _LL308; _LL308:
_temp307= _temp302.f3; goto _LL306; _LL306: _temp305= _temp302.f4; goto _LL304;
_LL304: _temp303= _temp302.f5; goto _LL301; _LL301: { struct Cyc_Absyn_Structdecl*
_temp313=* dp; void* _temp314=( void*) _temp313->sc; _LL316: if( _temp314 ==(
void*) Cyc_Absyn_Static){ goto _LL317;} else{ goto _LL318;} _LL318: if( _temp314
==( void*) Cyc_Absyn_Abstract){ goto _LL319;} else{ goto _LL320;} _LL320: if(
_temp314 ==( void*) Cyc_Absyn_Public){ goto _LL321;} else{ goto _LL322;} _LL322:
if( _temp314 ==( void*) Cyc_Absyn_ExternC){ goto _LL323;} else{ goto _LL324;}
_LL324: if( _temp314 ==( void*) Cyc_Absyn_Extern){ goto _LL325;} else{ goto
_LL315;} _LL317: if( _temp307? _temp313->fields == 0: 0){ Cyc_Interface_fields_err(
_tag_arr("static", sizeof( unsigned char), 7u), _tag_arr("struct", sizeof(
unsigned char), 7u),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp313->name))->v);} goto _LL315; _LL319: if( _temp313->fields == 0){ if(
_temp307){ Cyc_Interface_fields_err( _tag_arr("abstract", sizeof( unsigned char),
9u), _tag_arr("struct", sizeof( unsigned char), 7u),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp313->name))->v);}} else{ _temp313=({ struct Cyc_Absyn_Structdecl*
_temp326=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp326[ 0]=* _temp313; _temp326;}); _temp313->fields= 0;} if( Cyc_Interface_check_public_structdecl(
_temp305, _temp303, _temp313)){ _temp309->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)( _temp309->structdecls, x, _temp313);} goto _LL315;
_LL321: if( _temp313->fields == 0){ Cyc_Interface_fields_err( _tag_arr("public",
sizeof( unsigned char), 7u), _tag_arr("struct", sizeof( unsigned char), 7u),(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp313->name))->v);
_temp313=({ struct Cyc_Absyn_Structdecl* _temp327=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp327[ 0]=* _temp313;
_temp327;});( void*)( _temp313->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if(
Cyc_Interface_check_public_structdecl( _temp305, _temp303, _temp313)){ _temp309->structdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)( _temp309->structdecls, x,
_temp313);} goto _LL315; _LL323: goto _LL325; _LL325: if( Cyc_Interface_check_public_structdecl(
_temp305, _temp303, _temp313)){ _temp311->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)( _temp311->structdecls, x, _temp313);} goto _LL315;
_LL315:;}} static void Cyc_Interface_extract_uniondecl( struct _tuple11* env,
struct _tagged_arr* x, struct Cyc_Absyn_Uniondecl** dp){ struct _tuple11
_temp330; struct Cyc_Interface_Seen* _temp331; struct Cyc_Dict_Dict* _temp333;
int _temp335; struct Cyc_Interface_Ienv* _temp337; struct Cyc_Interface_Ienv*
_temp339; struct _tuple11* _temp328= env; _temp330=* _temp328; _LL340: _temp339=
_temp330.f1; goto _LL338; _LL338: _temp337= _temp330.f2; goto _LL336; _LL336:
_temp335= _temp330.f3; goto _LL334; _LL334: _temp333= _temp330.f4; goto _LL332;
_LL332: _temp331= _temp330.f5; goto _LL329; _LL329: { struct Cyc_Absyn_Uniondecl*
_temp341=* dp; void* _temp342=( void*) _temp341->sc; _LL344: if( _temp342 ==(
void*) Cyc_Absyn_Static){ goto _LL345;} else{ goto _LL346;} _LL346: if( _temp342
==( void*) Cyc_Absyn_Abstract){ goto _LL347;} else{ goto _LL348;} _LL348: if(
_temp342 ==( void*) Cyc_Absyn_Public){ goto _LL349;} else{ goto _LL350;} _LL350:
if( _temp342 ==( void*) Cyc_Absyn_ExternC){ goto _LL351;} else{ goto _LL352;}
_LL352: if( _temp342 ==( void*) Cyc_Absyn_Extern){ goto _LL353;} else{ goto
_LL343;} _LL345: if( _temp335? _temp341->fields == 0: 0){ Cyc_Interface_fields_err(
_tag_arr("static", sizeof( unsigned char), 7u), _tag_arr("union", sizeof(
unsigned char), 6u),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp341->name))->v);} goto _LL343; _LL347: if( _temp341->fields == 0){ if(
_temp335){ Cyc_Interface_fields_err( _tag_arr("abstract", sizeof( unsigned char),
9u), _tag_arr("union", sizeof( unsigned char), 6u),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp341->name))->v);}} else{ _temp341=({ struct Cyc_Absyn_Uniondecl*
_temp354=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp354[ 0]=* _temp341; _temp354;}); _temp341->fields= 0;} if( Cyc_Interface_check_public_uniondecl(
_temp333, _temp331, _temp341)){ _temp337->uniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)( _temp337->uniondecls, x, _temp341);} goto _LL343;
_LL349: if( _temp341->fields == 0){ Cyc_Interface_fields_err( _tag_arr("public",
sizeof( unsigned char), 7u), _tag_arr("union", sizeof( unsigned char), 6u),(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp341->name))->v);
_temp341=({ struct Cyc_Absyn_Uniondecl* _temp355=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp355[ 0]=* _temp341;
_temp355;});( void*)( _temp341->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if(
Cyc_Interface_check_public_uniondecl( _temp333, _temp331, _temp341)){ _temp337->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)( _temp337->uniondecls, x,
_temp341);} goto _LL343; _LL351: goto _LL353; _LL353: if( Cyc_Interface_check_public_uniondecl(
_temp333, _temp331, _temp341)){ _temp339->uniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)( _temp339->uniondecls, x, _temp341);} goto _LL343;
_LL343:;}} static void Cyc_Interface_extract_enumdecl( struct _tuple11* env,
struct _tagged_arr* x, struct Cyc_Absyn_Enumdecl** dp){ struct _tuple11 _temp358;
struct Cyc_Interface_Seen* _temp359; struct Cyc_Dict_Dict* _temp361; int
_temp363; struct Cyc_Interface_Ienv* _temp365; struct Cyc_Interface_Ienv*
_temp367; struct _tuple11* _temp356= env; _temp358=* _temp356; _LL368: _temp367=
_temp358.f1; goto _LL366; _LL366: _temp365= _temp358.f2; goto _LL364; _LL364:
_temp363= _temp358.f3; goto _LL362; _LL362: _temp361= _temp358.f4; goto _LL360;
_LL360: _temp359= _temp358.f5; goto _LL357; _LL357: { struct Cyc_Absyn_Enumdecl*
_temp369=* dp; void* _temp370=( void*) _temp369->sc; _LL372: if( _temp370 ==(
void*) Cyc_Absyn_Static){ goto _LL373;} else{ goto _LL374;} _LL374: if( _temp370
==( void*) Cyc_Absyn_Abstract){ goto _LL375;} else{ goto _LL376;} _LL376: if(
_temp370 ==( void*) Cyc_Absyn_Public){ goto _LL377;} else{ goto _LL378;} _LL378:
if( _temp370 ==( void*) Cyc_Absyn_ExternC){ goto _LL379;} else{ goto _LL380;}
_LL380: if( _temp370 ==( void*) Cyc_Absyn_Extern){ goto _LL381;} else{ goto
_LL371;} _LL373: if( _temp363? _temp369->fields == 0: 0){ Cyc_Interface_fields_err(
_tag_arr("static", sizeof( unsigned char), 7u), _tag_arr("enum", sizeof(
unsigned char), 5u), _temp369->name);} goto _LL371; _LL375: if( _temp369->fields
== 0){ if( _temp363){ Cyc_Interface_fields_err( _tag_arr("abstract", sizeof(
unsigned char), 9u), _tag_arr("enum", sizeof( unsigned char), 5u), _temp369->name);}}
else{ _temp369=({ struct Cyc_Absyn_Enumdecl* _temp382=( struct Cyc_Absyn_Enumdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl)); _temp382[ 0]=* _temp369;
_temp382;}); _temp369->fields= 0;} if( Cyc_Interface_check_public_enumdecl(
_temp361, _temp359, _temp369)){ _temp365->enumdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Enumdecl*
data)) Cyc_Dict_insert)( _temp365->enumdecls, x, _temp369);} goto _LL371; _LL377:
if( _temp369->fields == 0){ Cyc_Interface_fields_err( _tag_arr("public", sizeof(
unsigned char), 7u), _tag_arr("enum", sizeof( unsigned char), 5u), _temp369->name);
_temp369=({ struct Cyc_Absyn_Enumdecl* _temp383=( struct Cyc_Absyn_Enumdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl)); _temp383[ 0]=* _temp369;
_temp383;});( void*)( _temp369->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if(
Cyc_Interface_check_public_enumdecl( _temp361, _temp359, _temp369)){ _temp365->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key,
struct Cyc_Absyn_Enumdecl* data)) Cyc_Dict_insert)( _temp365->enumdecls, x,
_temp369);} goto _LL371; _LL379: goto _LL381; _LL381: if( Cyc_Interface_check_public_enumdecl(
_temp361, _temp359, _temp369)){ _temp367->enumdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Enumdecl*
data)) Cyc_Dict_insert)( _temp367->enumdecls, x, _temp369);} goto _LL371; _LL371:;}}
static void Cyc_Interface_extract_xtunionfielddecl( struct Cyc_Interface_I* i,
struct Cyc_Absyn_Tuniondecl* d, struct Cyc_Absyn_Tunionfield* f){ struct Cyc_List_List*
_temp384= 0;{ void* _temp385=(* f->name).f1; struct Cyc_List_List* _temp393;
struct Cyc_List_List* _temp395; _LL387: if(( unsigned int) _temp385 > 1u?*(( int*)
_temp385) == Cyc_Absyn_Rel_n: 0){ _LL394: _temp393=(( struct Cyc_Absyn_Rel_n_struct*)
_temp385)->f1; goto _LL388;} else{ goto _LL389;} _LL389: if(( unsigned int)
_temp385 > 1u?*(( int*) _temp385) == Cyc_Absyn_Abs_n: 0){ _LL396: _temp395=((
struct Cyc_Absyn_Abs_n_struct*) _temp385)->f1; goto _LL390;} else{ goto _LL391;}
_LL391: goto _LL392; _LL388: _temp395= _temp393; goto _LL390; _LL390: _temp384=
_temp395; goto _LL386; _LL392: goto _LL386; _LL386:;}{ struct Cyc_Dict_Dict**
dict;{ void* _temp397=( void*) f->sc; _LL399: if( _temp397 ==( void*) Cyc_Absyn_Static){
goto _LL400;} else{ goto _LL401;} _LL401: if( _temp397 ==( void*) Cyc_Absyn_Extern){
goto _LL402;} else{ goto _LL403;} _LL403: if( _temp397 ==( void*) Cyc_Absyn_Public){
goto _LL404;} else{ goto _LL405;} _LL405: goto _LL406; _LL400: return; _LL402:
dict=& i->imports; goto _LL398; _LL404: dict=& i->exports; goto _LL398; _LL406:(
int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp407=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp407[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp408; _temp408.tag= Cyc_Core_InvalidArg; _temp408.f1=
_tag_arr("add_xtunionfielddecl", sizeof( unsigned char), 21u); _temp408;});
_temp407;})); _LL398:;}{ struct Cyc_Interface_Ienv* env;{ struct _handler_cons
_temp409; _push_handler(& _temp409);{ int _temp411= 0; if( setjmp( _temp409.handler)){
_temp411= 1;} if( ! _temp411){ env=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(* dict, _temp384);; _pop_handler();}
else{ void* _temp410=( void*) _exn_thrown; void* _temp413= _temp410; _LL415: if(
_temp413 == Cyc_Dict_Absent){ goto _LL416;} else{ goto _LL417;} _LL417: goto
_LL418; _LL416: env= Cyc_Interface_new_ienv();* dict=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Interface_Ienv*
data)) Cyc_Dict_insert)(* dict, _temp384, env); goto _LL414; _LL418:( void)
_throw( _temp413); _LL414:;}}} env->xtunionfielddecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Tcdecl_Xtunionfielddecl*
data)) Cyc_Dict_insert)( env->xtunionfielddecls,(* f->name).f2,({ struct Cyc_Tcdecl_Xtunionfielddecl*
_temp419=( struct Cyc_Tcdecl_Xtunionfielddecl*) GC_malloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl));
_temp419->base= d; _temp419->field= f; _temp419;}));}}} static void Cyc_Interface_extract_tuniondecl(
struct _tuple11* env, struct _tagged_arr* x, struct Cyc_Absyn_Tuniondecl** dp){
struct _tuple11 _temp422; struct Cyc_Interface_I* _temp423; struct Cyc_Interface_Seen*
_temp425; struct Cyc_Dict_Dict* _temp427; int _temp429; struct Cyc_Interface_Ienv*
_temp431; struct Cyc_Interface_Ienv* _temp433; struct _tuple11* _temp420= env;
_temp422=* _temp420; _LL434: _temp433= _temp422.f1; goto _LL432; _LL432:
_temp431= _temp422.f2; goto _LL430; _LL430: _temp429= _temp422.f3; goto _LL428;
_LL428: _temp427= _temp422.f4; goto _LL426; _LL426: _temp425= _temp422.f5; goto
_LL424; _LL424: _temp423= _temp422.f6; goto _LL421; _LL421: { struct Cyc_Absyn_Tuniondecl*
_temp435=* dp; void* _temp436=( void*) _temp435->sc; _LL438: if( _temp436 ==(
void*) Cyc_Absyn_Static){ goto _LL439;} else{ goto _LL440;} _LL440: if( _temp436
==( void*) Cyc_Absyn_Abstract){ goto _LL441;} else{ goto _LL442;} _LL442: if(
_temp436 ==( void*) Cyc_Absyn_Public){ goto _LL443;} else{ goto _LL444;} _LL444:
if( _temp436 ==( void*) Cyc_Absyn_ExternC){ goto _LL445;} else{ goto _LL446;}
_LL446: if( _temp436 ==( void*) Cyc_Absyn_Extern){ goto _LL447;} else{ goto
_LL437;} _LL439: if(( ! _temp435->is_xtunion? _temp429: 0)? _temp435->fields ==
0: 0){ Cyc_Interface_fields_err( _tag_arr("static", sizeof( unsigned char), 7u),
_tag_arr("tunion", sizeof( unsigned char), 7u), _temp435->name);} goto _LL437;
_LL441: if( _temp435->fields == 0){ if( ! _temp435->is_xtunion? _temp429: 0){
Cyc_Interface_fields_err( _tag_arr("abstract", sizeof( unsigned char), 9u),
_tag_arr("tunion", sizeof( unsigned char), 7u), _temp435->name);}} else{
_temp435=({ struct Cyc_Absyn_Tuniondecl* _temp448=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp448[ 0]=* _temp435;
_temp448;}); _temp435->fields= 0;} if( Cyc_Interface_check_public_tuniondecl(
_temp427, _temp425, _temp435)){ _temp431->tuniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl*
data)) Cyc_Dict_insert)( _temp431->tuniondecls, x, _temp435);} goto _LL437;
_LL443: _temp435=({ struct Cyc_Absyn_Tuniondecl* _temp449=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp449[ 0]=* _temp435;
_temp449;}); if( ! _temp435->is_xtunion? _temp435->fields == 0: 0){ Cyc_Interface_fields_err(
_tag_arr("public", sizeof( unsigned char), 7u), _tag_arr("tunion", sizeof(
unsigned char), 7u), _temp435->name);( void*)( _temp435->sc=( void*)(( void*)
Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_tuniondecl( _temp427,
_temp425, _temp435)){ if( _temp435->is_xtunion? _temp435->fields != 0: 0){
struct Cyc_List_List* _temp450=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp435->fields))->v; _temp435->fields= 0;{ struct Cyc_List_List*
_temp451= _temp450; for( 0; _temp451 != 0; _temp451=(( struct Cyc_List_List*)
_check_null( _temp451))->tl){ Cyc_Interface_extract_xtunionfielddecl( _temp423,
_temp435,( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp451))->hd);}}} _temp431->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl* data))
Cyc_Dict_insert)( _temp431->tuniondecls, x, _temp435);} goto _LL437; _LL445:(
int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp452=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp452[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp453; _temp453.tag= Cyc_Core_InvalidArg; _temp453.f1=
_tag_arr("extract_tuniondecl", sizeof( unsigned char), 19u); _temp453;});
_temp452;})); _LL447: if( Cyc_Interface_check_public_tuniondecl( _temp427,
_temp425, _temp435)){ if( _temp435->is_xtunion? _temp435->fields != 0: 0){
_temp435=({ struct Cyc_Absyn_Tuniondecl* _temp454=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp454[ 0]=* _temp435;
_temp454;});{ struct Cyc_List_List* _temp455=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( _temp435->fields))->v; _temp435->fields= 0;{ struct
Cyc_List_List* _temp456= _temp455; for( 0; _temp456 != 0; _temp456=(( struct Cyc_List_List*)
_check_null( _temp456))->tl){ Cyc_Interface_extract_xtunionfielddecl( _temp423,
_temp435,( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp456))->hd);}}}} _temp433->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Tuniondecl* data))
Cyc_Dict_insert)( _temp433->tuniondecls, x, _temp435);} goto _LL437; _LL437:;}}
static void Cyc_Interface_extract_typedef( struct _tuple11* env, struct
_tagged_arr* x, struct Cyc_Absyn_Typedefdecl* d){ struct _tuple11 _temp459;
struct Cyc_Interface_Seen* _temp460; struct Cyc_Dict_Dict* _temp462; struct Cyc_Interface_Ienv*
_temp464; struct _tuple11* _temp457= env; _temp459=* _temp457; _LL465: _temp464=
_temp459.f2; goto _LL463; _LL463: _temp462= _temp459.f4; goto _LL461; _LL461:
_temp460= _temp459.f5; goto _LL458; _LL458: if( Cyc_Interface_check_public_typedefdecl(
_temp462, _temp460, d)){ _temp464->typedefdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Typedefdecl*
data)) Cyc_Dict_insert)( _temp464->typedefdecls, x, d);}} static void Cyc_Interface_extract_ordinarie(
struct _tuple11* env, struct _tagged_arr* x, struct _tuple7* v){ struct _tuple11
_temp468; struct Cyc_Interface_Seen* _temp469; struct Cyc_Dict_Dict* _temp471;
int _temp473; struct Cyc_Interface_Ienv* _temp475; struct Cyc_Interface_Ienv*
_temp477; struct _tuple11* _temp466= env; _temp468=* _temp466; _LL478: _temp477=
_temp468.f1; goto _LL476; _LL476: _temp475= _temp468.f2; goto _LL474; _LL474:
_temp473= _temp468.f3; goto _LL472; _LL472: _temp471= _temp468.f4; goto _LL470;
_LL470: _temp469= _temp468.f5; goto _LL467; _LL467: { void* _temp479=(* v).f1;
void* _temp480= _temp479; void* _temp486; _LL482: if(*(( int*) _temp480) == Cyc_Tcenv_VarRes){
_LL487: _temp486=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp480)->f1; goto
_LL483;} else{ goto _LL484;} _LL484: goto _LL485; _LL483:{ void* _temp488=
_temp486; struct Cyc_Absyn_Fndecl* _temp496; struct Cyc_Absyn_Vardecl* _temp498;
_LL490: if(( unsigned int) _temp488 > 1u?*(( int*) _temp488) == Cyc_Absyn_Funname_b:
0){ _LL497: _temp496=(( struct Cyc_Absyn_Funname_b_struct*) _temp488)->f1; goto
_LL491;} else{ goto _LL492;} _LL492: if(( unsigned int) _temp488 > 1u?*(( int*)
_temp488) == Cyc_Absyn_Global_b: 0){ _LL499: _temp498=(( struct Cyc_Absyn_Global_b_struct*)
_temp488)->f1; goto _LL493;} else{ goto _LL494;} _LL494: goto _LL495; _LL491: {
struct Cyc_Absyn_Vardecl* _temp500=({ struct Cyc_Absyn_Vardecl* _temp501=(
struct Cyc_Absyn_Vardecl*) GC_malloc( sizeof( struct Cyc_Absyn_Vardecl));
_temp501->sc=( void*)(( void*) _temp496->sc); _temp501->name= _temp496->name;
_temp501->tq=({ struct Cyc_Absyn_Tqual _temp502; _temp502.q_const= 0; _temp502.q_volatile=
0; _temp502.q_restrict= 0; _temp502;}); _temp501->type=( void*)(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp496->cached_typ))->v); _temp501->initializer=
0; _temp501->rgn= 0; _temp501->attributes= 0; _temp501;}); _temp473= 0; _temp498=
_temp500; goto _LL493;} _LL493: if( _temp498->initializer != 0){ _temp498=({
struct Cyc_Absyn_Vardecl* _temp503=( struct Cyc_Absyn_Vardecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Vardecl)); _temp503[ 0]=* _temp498; _temp503;});
_temp498->initializer= 0;}{ void* _temp504=( void*) _temp498->sc; _LL506: if(
_temp504 ==( void*) Cyc_Absyn_Static){ goto _LL507;} else{ goto _LL508;} _LL508:
if( _temp504 ==( void*) Cyc_Absyn_Abstract){ goto _LL509;} else{ goto _LL510;}
_LL510: if( _temp504 ==( void*) Cyc_Absyn_Public){ goto _LL511;} else{ goto
_LL512;} _LL512: if( _temp504 ==( void*) Cyc_Absyn_ExternC){ goto _LL513;} else{
goto _LL514;} _LL514: if( _temp504 ==( void*) Cyc_Absyn_Extern){ goto _LL515;}
else{ goto _LL505;} _LL507: if( _temp473? Cyc_Tcutil_is_function_type(( void*)
_temp498->type): 0){ Cyc_Interface_body_err( _tag_arr("static", sizeof(
unsigned char), 7u), _temp498->name);} goto _LL505; _LL509:( int) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp516=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp516[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp517; _temp517.tag= Cyc_Core_InvalidArg; _temp517.f1=
_tag_arr("extract_ordinarie", sizeof( unsigned char), 18u); _temp517;});
_temp516;})); _LL511: if( _temp473? Cyc_Tcutil_is_function_type(( void*)
_temp498->type): 0){ Cyc_Interface_body_err( _tag_arr("public", sizeof(
unsigned char), 7u), _temp498->name);} if( Cyc_Interface_check_public_vardecl(
_temp471, _temp469, _temp498)){ _temp475->vardecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Vardecl* data))
Cyc_Dict_insert)( _temp475->vardecls, x, _temp498);} goto _LL505; _LL513: goto
_LL515; _LL515: if( Cyc_Interface_check_public_vardecl( _temp471, _temp469,
_temp498)){ _temp477->vardecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, struct Cyc_Absyn_Vardecl* data)) Cyc_Dict_insert)(
_temp477->vardecls, x, _temp498);} goto _LL505; _LL505:;} goto _LL489; _LL495:
goto _LL489; _LL489:;} goto _LL481; _LL485: goto _LL481; _LL481:;}} struct
_tuple12{ struct Cyc_Interface_I* f1; int f2; struct Cyc_Dict_Dict* f3; struct
Cyc_Interface_Seen* f4; } ; static void Cyc_Interface_extract_f( struct _tuple12*
env_f, struct Cyc_List_List* ns, struct Cyc_Tcenv_Genv* ge){ struct _tuple12
_temp520; struct Cyc_Interface_Seen* _temp521; struct Cyc_Dict_Dict* _temp523;
int _temp525; struct Cyc_Interface_I* _temp527; struct _tuple12* _temp518= env_f;
_temp520=* _temp518; _LL528: _temp527= _temp520.f1; goto _LL526; _LL526:
_temp525= _temp520.f2; goto _LL524; _LL524: _temp523= _temp520.f3; goto _LL522;
_LL522: _temp521= _temp520.f4; goto _LL519; _LL519: { struct _tuple11 _temp529=({
struct _tuple11 _temp530; _temp530.f1=(( struct Cyc_Interface_Ienv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp527->imports,
ns); _temp530.f2=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp527->exports, ns); _temp530.f3=
_temp525; _temp530.f4= _temp523; _temp530.f5= _temp521; _temp530.f6= _temp527;
_temp530;});(( void(*)( void(* f)( struct _tuple11*, struct _tagged_arr*, struct
Cyc_Absyn_Structdecl**), struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(
Cyc_Interface_extract_structdecl,& _temp529, ge->structdecls);(( void(*)( void(*
f)( struct _tuple11*, struct _tagged_arr*, struct Cyc_Absyn_Uniondecl**), struct
_tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_uniondecl,&
_temp529, ge->uniondecls);(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_arr*, struct Cyc_Absyn_Tuniondecl**), struct _tuple11* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_tuniondecl,& _temp529, ge->tuniondecls);((
void(*)( void(* f)( struct _tuple11*, struct _tagged_arr*, struct Cyc_Absyn_Enumdecl**),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_enumdecl,&
_temp529, ge->enumdecls);(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_arr*, struct Cyc_Absyn_Typedefdecl*), struct _tuple11* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_typedef,& _temp529, ge->typedefs);((
void(*)( void(* f)( struct _tuple11*, struct _tagged_arr*, struct _tuple7*),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_ordinarie,&
_temp529, ge->ordinaries);}} static struct Cyc_Interface_I* Cyc_Interface_gen_extract(
struct Cyc_Dict_Dict* ae, int check_complete_defs){ struct _tuple12 _temp531=({
struct _tuple12 _temp532; _temp532.f1=(( struct Cyc_Interface_I*(*)( struct Cyc_Dict_Dict*
skel)) Cyc_Interface_skel2i)( ae); _temp532.f2= check_complete_defs; _temp532.f3=
ae; _temp532.f4= Cyc_Interface_new_seen(); _temp532;});(( void(*)( void(* f)(
struct _tuple12*, struct Cyc_List_List*, struct Cyc_Tcenv_Genv*), struct
_tuple12* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_f,&
_temp531, ae); return _temp531.f1;} struct Cyc_Interface_I* Cyc_Interface_extract(
struct Cyc_Dict_Dict* ae){ return Cyc_Interface_gen_extract( ae, 1);} inline
static void Cyc_Interface_check_err( struct _tagged_arr* msg1, struct
_tagged_arr msg2){ Cyc_Tcdecl_merr( 0, msg1, msg2);} struct _tuple13{ int f1;
struct Cyc_Dict_Dict* f2; int(* f3)( void*, void*, struct _tagged_arr*); struct
_tagged_arr f4; struct _tagged_arr* f5; } ; static void Cyc_Interface_incl_dict_f(
struct _tuple13* env, struct _tagged_arr* x, void* y1){ struct _tuple13 _temp535;
struct _tagged_arr* _temp536; struct _tagged_arr _temp538; int(* _temp540)( void*,
void*, struct _tagged_arr*); struct Cyc_Dict_Dict* _temp542; int _temp544; int*
_temp546; struct _tuple13* _temp533= env; _temp535=* _temp533; _LL545: _temp544=
_temp535.f1; _temp546=&(* _temp533).f1; goto _LL543; _LL543: _temp542= _temp535.f2;
goto _LL541; _LL541: _temp540= _temp535.f3; goto _LL539; _LL539: _temp538=
_temp535.f4; goto _LL537; _LL537: _temp536= _temp535.f5; goto _LL534; _LL534: {
struct _handler_cons _temp547; _push_handler(& _temp547);{ int _temp549= 0; if(
setjmp( _temp547.handler)){ _temp549= 1;} if( ! _temp549){{ void* _temp550=((
void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
_temp542, x); if( ! _temp540( y1, _temp550, _temp536)){* _temp546= 0;}};
_pop_handler();} else{ void* _temp548=( void*) _exn_thrown; void* _temp552=
_temp548; _LL554: if( _temp552 == Cyc_Dict_Absent){ goto _LL555;} else{ goto
_LL556;} _LL556: goto _LL557; _LL555: Cyc_Interface_check_err( _temp536,( struct
_tagged_arr)({ struct _tagged_arr(* _temp558)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp563= _tag_arr("%s %s is missing",
sizeof( unsigned char), 17u); struct Cyc_Stdio_String_pa_struct _temp565;
_temp565.tag= Cyc_Stdio_String_pa; _temp565.f1=( struct _tagged_arr) _temp538;{
void* _temp561=( void*)& _temp565; struct Cyc_Stdio_String_pa_struct _temp564;
_temp564.tag= Cyc_Stdio_String_pa; _temp564.f1=( struct _tagged_arr)* x;{ void*
_temp562=( void*)& _temp564; void* _temp559[ 2u]={ _temp561, _temp562}; struct
_tagged_arr _temp560={( void*) _temp559,( void*) _temp559,( void*)( _temp559 + 2u)};
_temp558( _temp563, _temp560);}}}));* _temp546= 0; goto _LL553; _LL557:( void)
_throw( _temp552); _LL553:;}}}} static int Cyc_Interface_incl_dict( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( void*, void*, struct
_tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg){ struct _tuple13
_temp566=({ struct _tuple13 _temp567; _temp567.f1= 1; _temp567.f2= dic2;
_temp567.f3= incl_f; _temp567.f4= t; _temp567.f5= msg; _temp567;});(( void(*)(
void(* f)( struct _tuple13*, struct _tagged_arr*, void*), struct _tuple13* env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_incl_dict_f,& _temp566,
dic1); return _temp566.f1;} static int Cyc_Interface_incl_structdecl( struct Cyc_Absyn_Structdecl*
d0, struct Cyc_Absyn_Structdecl* d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Structdecl*
_temp568= Cyc_Tcdecl_merge_structdecl( d0, d1, 0, msg); if( _temp568 == 0){
return 0;} if(( struct Cyc_Absyn_Structdecl*) d0 != _temp568){ Cyc_Interface_check_err(
msg,( struct _tagged_arr)({ struct _tagged_arr(* _temp569)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp573=
_tag_arr("declaration of struct %s discloses too much information", sizeof(
unsigned char), 56u); struct Cyc_Stdio_String_pa_struct _temp574; _temp574.tag=
Cyc_Stdio_String_pa; _temp574.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d1->name))->v);{ void*
_temp572=( void*)& _temp574; void* _temp570[ 1u]={ _temp572}; struct _tagged_arr
_temp571={( void*) _temp570,( void*) _temp570,( void*)( _temp570 + 1u)};
_temp569( _temp573, _temp571);}})); return 0;} return 1;} static int Cyc_Interface_incl_uniondecl(
struct Cyc_Absyn_Uniondecl* d0, struct Cyc_Absyn_Uniondecl* d1, struct
_tagged_arr* msg){ struct Cyc_Absyn_Uniondecl* _temp575= Cyc_Tcdecl_merge_uniondecl(
d0, d1, 0, msg); if( _temp575 == 0){ return 0;} if(( struct Cyc_Absyn_Uniondecl*)
d0 != _temp575){ Cyc_Interface_check_err( msg,( struct _tagged_arr)({ struct
_tagged_arr(* _temp576)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp580= _tag_arr("declaration of union %s discloses too much information",
sizeof( unsigned char), 55u); struct Cyc_Stdio_String_pa_struct _temp581;
_temp581.tag= Cyc_Stdio_String_pa; _temp581.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d1->name))->v);{ void*
_temp579=( void*)& _temp581; void* _temp577[ 1u]={ _temp579}; struct _tagged_arr
_temp578={( void*) _temp577,( void*) _temp577,( void*)( _temp577 + 1u)};
_temp576( _temp580, _temp578);}})); return 0;} return 1;} static int Cyc_Interface_incl_tuniondecl(
struct Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl* d1, struct
_tagged_arr* msg){ struct Cyc_Absyn_Tuniondecl* _temp582= Cyc_Tcdecl_merge_tuniondecl(
d0, d1, 0, msg); if( _temp582 == 0){ return 0;} if(( struct Cyc_Absyn_Tuniondecl*)
d0 != _temp582){ Cyc_Interface_check_err( msg,( struct _tagged_arr)({ struct
_tagged_arr(* _temp583)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp587= _tag_arr("declaration of tunion %s discloses too much information",
sizeof( unsigned char), 56u); struct Cyc_Stdio_String_pa_struct _temp588;
_temp588.tag= Cyc_Stdio_String_pa; _temp588.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
d1->name);{ void* _temp586=( void*)& _temp588; void* _temp584[ 1u]={ _temp586};
struct _tagged_arr _temp585={( void*) _temp584,( void*) _temp584,( void*)(
_temp584 + 1u)}; _temp583( _temp587, _temp585);}})); return 0;} return 1;}
static int Cyc_Interface_incl_enumdecl( struct Cyc_Absyn_Enumdecl* d0, struct
Cyc_Absyn_Enumdecl* d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Enumdecl*
_temp589= Cyc_Tcdecl_merge_enumdecl( d0, d1, 0, msg); if( _temp589 == 0){ return
0;} if(( struct Cyc_Absyn_Enumdecl*) d0 != _temp589){ Cyc_Interface_check_err(
msg,( struct _tagged_arr)({ struct _tagged_arr(* _temp590)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp594=
_tag_arr("declaration of enum %s discloses too much information", sizeof(
unsigned char), 54u); struct Cyc_Stdio_String_pa_struct _temp595; _temp595.tag=
Cyc_Stdio_String_pa; _temp595.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
d1->name);{ void* _temp593=( void*)& _temp595; void* _temp591[ 1u]={ _temp593};
struct _tagged_arr _temp592={( void*) _temp591,( void*) _temp591,( void*)(
_temp591 + 1u)}; _temp590( _temp594, _temp592);}})); return 0;} return 1;}
static int Cyc_Interface_incl_vardecl( struct Cyc_Absyn_Vardecl* d0, struct Cyc_Absyn_Vardecl*
d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Vardecl* _temp596= Cyc_Tcdecl_merge_vardecl(
d0, d1, 0, msg); if( _temp596 == 0){ return 0;} if(( struct Cyc_Absyn_Vardecl*)
d0 != _temp596){ Cyc_Interface_check_err( msg,( struct _tagged_arr)({ struct
_tagged_arr(* _temp597)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp601= _tag_arr("declaration of variable %s discloses too much information",
sizeof( unsigned char), 58u); struct Cyc_Stdio_String_pa_struct _temp602;
_temp602.tag= Cyc_Stdio_String_pa; _temp602.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
d1->name);{ void* _temp600=( void*)& _temp602; void* _temp598[ 1u]={ _temp600};
struct _tagged_arr _temp599={( void*) _temp598,( void*) _temp598,( void*)(
_temp598 + 1u)}; _temp597( _temp601, _temp599);}})); return 0;} return 1;}
static int Cyc_Interface_incl_typedefdecl( struct Cyc_Absyn_Typedefdecl* d0,
struct Cyc_Absyn_Typedefdecl* d1, struct _tagged_arr* msg){ struct Cyc_Absyn_Typedefdecl*
_temp603= Cyc_Tcdecl_merge_typedefdecl( d0, d1, 0, msg); if( _temp603 == 0){
return 0;} if(( struct Cyc_Absyn_Typedefdecl*) d0 != _temp603){ Cyc_Interface_check_err(
msg,( struct _tagged_arr)({ struct _tagged_arr(* _temp604)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp608=
_tag_arr("declaration of typedef %s discloses too much information", sizeof(
unsigned char), 57u); struct Cyc_Stdio_String_pa_struct _temp609; _temp609.tag=
Cyc_Stdio_String_pa; _temp609.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
d1->name);{ void* _temp607=( void*)& _temp609; void* _temp605[ 1u]={ _temp607};
struct _tagged_arr _temp606={( void*) _temp605,( void*) _temp605,( void*)(
_temp605 + 1u)}; _temp604( _temp608, _temp606);}})); return 0;} return 1;}
static int Cyc_Interface_incl_xtunionfielddecl( struct Cyc_Tcdecl_Xtunionfielddecl*
d0, struct Cyc_Tcdecl_Xtunionfielddecl* d1, struct _tagged_arr* msg){ struct Cyc_Tcdecl_Xtunionfielddecl*
_temp610= Cyc_Tcdecl_merge_xtunionfielddecl( d0, d1, 0, msg); if( _temp610 == 0){
return 0;} if(( struct Cyc_Tcdecl_Xtunionfielddecl*) d0 != _temp610){ Cyc_Interface_check_err(
msg,( struct _tagged_arr)({ struct _tagged_arr(* _temp611)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp615=
_tag_arr("declaration of xtunionfield %s discloses too much information",
sizeof( unsigned char), 62u); struct Cyc_Stdio_String_pa_struct _temp616;
_temp616.tag= Cyc_Stdio_String_pa; _temp616.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
d1->field)->name);{ void* _temp614=( void*)& _temp616; void* _temp612[ 1u]={
_temp614}; struct _tagged_arr _temp613={( void*) _temp612,( void*) _temp612,(
void*)( _temp612 + 1u)}; _temp611( _temp615, _temp613);}})); return 0;} return 1;}
struct Cyc_Core_Opt* Cyc_Interface_compat_merge_dict( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, void**(* merge_f)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_arr*), struct _tagged_arr t,
struct _tagged_arr* msg); static int Cyc_Interface_incl_ienv( struct Cyc_Interface_Ienv*
ie1, struct Cyc_Interface_Ienv* ie2, struct _tagged_arr* msg){ int _temp617=((
int(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)(
struct Cyc_Absyn_Structdecl*, struct Cyc_Absyn_Structdecl*, struct _tagged_arr*),
struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_incl_dict)( ie1->structdecls,
ie2->structdecls, Cyc_Interface_incl_structdecl, _tag_arr("struct", sizeof(
unsigned char), 7u), msg); int _temp618=(( int(*)( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Uniondecl*, struct
Cyc_Absyn_Uniondecl*, struct _tagged_arr*), struct _tagged_arr t, struct
_tagged_arr* msg)) Cyc_Interface_incl_dict)( ie1->uniondecls, ie2->uniondecls,
Cyc_Interface_incl_uniondecl, _tag_arr("union", sizeof( unsigned char), 6u), msg);
int _temp619=(( int(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2,
int(* incl_f)( struct Cyc_Absyn_Tuniondecl*, struct Cyc_Absyn_Tuniondecl*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_incl_dict)(
ie1->tuniondecls, ie2->tuniondecls, Cyc_Interface_incl_tuniondecl, _tag_arr("tunion",
sizeof( unsigned char), 7u), msg); int _temp620=(( int(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Enumdecl*,
struct Cyc_Absyn_Enumdecl*, struct _tagged_arr*), struct _tagged_arr t, struct
_tagged_arr* msg)) Cyc_Interface_incl_dict)( ie1->enumdecls, ie2->enumdecls, Cyc_Interface_incl_enumdecl,
_tag_arr("enum", sizeof( unsigned char), 5u), msg); int _temp621=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)( struct Cyc_Absyn_Typedefdecl*,
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*, struct _tagged_arr*),
struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls, ie2->typedefdecls,(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_strptrcmp),
Cyc_Tcdecl_merge_typedefdecl, _tag_arr("typedef", sizeof( unsigned char), 8u),
msg) != 0; int _temp622=(( int(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, int(* incl_f)( struct Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_incl_dict)(
ie1->vardecls, ie2->vardecls, Cyc_Interface_incl_vardecl, _tag_arr("variable",
sizeof( unsigned char), 9u), msg); int _temp623=(( int(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Tcdecl_Xtunionfielddecl*,
struct Cyc_Tcdecl_Xtunionfielddecl*, struct _tagged_arr*), struct _tagged_arr t,
struct _tagged_arr* msg)) Cyc_Interface_incl_dict)( ie1->xtunionfielddecls, ie2->xtunionfielddecls,
Cyc_Interface_incl_xtunionfielddecl, _tag_arr("xtunionfield", sizeof(
unsigned char), 13u), msg); return((((( _temp617? _temp618: 0)? _temp619: 0)?
_temp620: 0)? _temp621: 0)? _temp622: 0)? _temp623: 0;} struct _tuple14{ int f1;
struct Cyc_Dict_Dict* f2; struct _tagged_arr* f3; } ; static void Cyc_Interface_incl_ns_f(
struct _tuple14* env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie1){
struct _tuple14 _temp626; struct _tagged_arr* _temp627; struct Cyc_Dict_Dict*
_temp629; int _temp631; int* _temp633; struct _tuple14* _temp624= env; _temp626=*
_temp624; _LL632: _temp631= _temp626.f1; _temp633=&(* _temp624).f1; goto _LL630;
_LL630: _temp629= _temp626.f2; goto _LL628; _LL628: _temp627= _temp626.f3; goto
_LL625; _LL625: { struct Cyc_Interface_Ienv* ie2;{ struct _handler_cons _temp634;
_push_handler(& _temp634);{ int _temp636= 0; if( setjmp( _temp634.handler)){
_temp636= 1;} if( ! _temp636){ ie2=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp629, ns);; _pop_handler();}
else{ void* _temp635=( void*) _exn_thrown; void* _temp638= _temp635; _LL640: if(
_temp638 == Cyc_Dict_Absent){ goto _LL641;} else{ goto _LL642;} _LL642: goto
_LL643; _LL641: ie2= Cyc_Interface_lazy_new_ienv(); goto _LL639; _LL643:( void)
_throw( _temp638); _LL639:;}}} if( ! Cyc_Interface_incl_ienv( ie1, ie2, _temp627)){*
_temp633= 0;}}} static int Cyc_Interface_incl_ns( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct _tagged_arr* msg){ struct _tuple14 _temp644=({
struct _tuple14 _temp645; _temp645.f1= 1; _temp645.f2= dic2; _temp645.f3= msg;
_temp645;});(( void(*)( void(* f)( struct _tuple14*, struct Cyc_List_List*,
struct Cyc_Interface_Ienv*), struct _tuple14* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(
Cyc_Interface_incl_ns_f,& _temp644, dic1); return _temp644.f1;} int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple3* info){
struct _handler_cons _temp646; _push_handler(& _temp646);{ int _temp648= 0; if(
setjmp( _temp646.handler)){ _temp648= 1;} if( ! _temp648){{ int _temp649= 1;
struct _tagged_arr* msg= 0; if( info != 0){ msg=({ struct _tagged_arr* _temp650=(
struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp650[ 0]=(
struct _tagged_arr)({ struct _tagged_arr(* _temp651)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp656= _tag_arr("checking inclusion of %s exports into %s exports,",
sizeof( unsigned char), 50u); struct Cyc_Stdio_String_pa_struct _temp658;
_temp658.tag= Cyc_Stdio_String_pa; _temp658.f1=( struct _tagged_arr)(*(( struct
_tuple3*) _check_null( info))).f1;{ void* _temp654=( void*)& _temp658; struct
Cyc_Stdio_String_pa_struct _temp657; _temp657.tag= Cyc_Stdio_String_pa; _temp657.f1=(
struct _tagged_arr)(*(( struct _tuple3*) _check_null( info))).f2;{ void*
_temp655=( void*)& _temp657; void* _temp652[ 2u]={ _temp654, _temp655}; struct
_tagged_arr _temp653={( void*) _temp652,( void*) _temp652,( void*)( _temp652 + 2u)};
_temp651( _temp656, _temp653);}}}); _temp650;});} if( ! Cyc_Interface_incl_ns(
i1->exports, i2->exports, msg)){ _temp649= 0;} if( info != 0){ msg=({ struct
_tagged_arr* _temp659=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp659[ 0]=( struct _tagged_arr)({ struct _tagged_arr(*
_temp660)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp665= _tag_arr("checking inclusion of %s imports into %s imports,",
sizeof( unsigned char), 50u); struct Cyc_Stdio_String_pa_struct _temp667;
_temp667.tag= Cyc_Stdio_String_pa; _temp667.f1=( struct _tagged_arr)(*(( struct
_tuple3*) _check_null( info))).f2;{ void* _temp663=( void*)& _temp667; struct
Cyc_Stdio_String_pa_struct _temp666; _temp666.tag= Cyc_Stdio_String_pa; _temp666.f1=(
struct _tagged_arr)(*(( struct _tuple3*) _check_null( info))).f1;{ void*
_temp664=( void*)& _temp666; void* _temp661[ 2u]={ _temp663, _temp664}; struct
_tagged_arr _temp662={( void*) _temp661,( void*) _temp661,( void*)( _temp661 + 2u)};
_temp660( _temp665, _temp662);}}}); _temp659;});} if( ! Cyc_Interface_incl_ns(
i2->imports, i1->imports, msg)){ _temp649= 0;}{ int _temp668= _temp649;
_npop_handler( 0u); return _temp668;}}; _pop_handler();} else{ void* _temp647=(
void*) _exn_thrown; void* _temp670= _temp647; _LL672: if( _temp670 == Cyc_Tcdecl_Incompatible){
goto _LL673;} else{ goto _LL674;} _LL674: goto _LL675; _LL673: return 0; _LL675:(
void) _throw( _temp670); _LL671:;}}} struct _tuple15{ int f1; struct Cyc_Dict_Dict*
f2; struct Cyc_Dict_Dict* f3; struct Cyc_Dict_Dict* f4; void**(* f5)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_arr*); struct _tagged_arr
f6; struct _tagged_arr* f7; } ; void Cyc_Interface_compat_merge_dict_f( struct
_tuple15* env, struct _tagged_arr* x, void** y2){ struct _tuple15 _temp678;
struct _tagged_arr* _temp679; struct _tagged_arr _temp681; void**(* _temp683)(
void**, void**, struct Cyc_Position_Segment*, struct _tagged_arr*); struct Cyc_Dict_Dict*
_temp685; struct Cyc_Dict_Dict* _temp687; struct Cyc_Dict_Dict* _temp689; struct
Cyc_Dict_Dict** _temp691; int _temp692; int* _temp694; struct _tuple15* _temp676=
env; _temp678=* _temp676; _LL693: _temp692= _temp678.f1; _temp694=&(* _temp676).f1;
goto _LL690; _LL690: _temp689= _temp678.f2; _temp691=&(* _temp676).f2; goto
_LL688; _LL688: _temp687= _temp678.f3; goto _LL686; _LL686: _temp685= _temp678.f4;
goto _LL684; _LL684: _temp683= _temp678.f5; goto _LL682; _LL682: _temp681=
_temp678.f6; goto _LL680; _LL680: _temp679= _temp678.f7; goto _LL677; _LL677: {
void** y;{ struct _handler_cons _temp695; _push_handler(& _temp695);{ int
_temp697= 0; if( setjmp( _temp695.handler)){ _temp697= 1;} if( ! _temp697){{
void** _temp698=(( void**(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key))
Cyc_Dict_lookup)( _temp687, x); void** _temp699= _temp683( _temp698, y2, 0,
_temp679); if( !(( unsigned int) _temp699)){* _temp694= 0; _npop_handler( 0u);
return;} y=( void**) _check_null( _temp699);}; _pop_handler();} else{ void*
_temp696=( void*) _exn_thrown; void* _temp701= _temp696; _LL703: if( _temp701 ==
Cyc_Dict_Absent){ goto _LL704;} else{ goto _LL705;} _LL705: goto _LL706; _LL704:
y= y2; goto _LL702; _LL706:( void) _throw( _temp701); _LL702:;}}}{ struct
_handler_cons _temp707; _push_handler(& _temp707);{ int _temp709= 0; if( setjmp(
_temp707.handler)){ _temp709= 1;} if( ! _temp709){{ void** _temp710=(( void**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)( _temp685, x);
void** _temp711= _temp683( _temp710, y, 0, _temp679); if( _temp711 !=( void**)
_temp710){ if(( unsigned int) _temp711){ Cyc_Interface_check_err( _temp679,(
struct _tagged_arr)({ struct _tagged_arr(* _temp712)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp717= _tag_arr("abstract %s %s is being imported as non-abstract",
sizeof( unsigned char), 49u); struct Cyc_Stdio_String_pa_struct _temp719;
_temp719.tag= Cyc_Stdio_String_pa; _temp719.f1=( struct _tagged_arr) _temp681;{
void* _temp715=( void*)& _temp719; struct Cyc_Stdio_String_pa_struct _temp718;
_temp718.tag= Cyc_Stdio_String_pa; _temp718.f1=( struct _tagged_arr)* x;{ void*
_temp716=( void*)& _temp718; void* _temp713[ 2u]={ _temp715, _temp716}; struct
_tagged_arr _temp714={( void*) _temp713,( void*) _temp713,( void*)( _temp713 + 2u)};
_temp712( _temp717, _temp714);}}}));}* _temp694= 0;}}; _pop_handler();} else{
void* _temp708=( void*) _exn_thrown; void* _temp721= _temp708; _LL723: if(
_temp721 == Cyc_Dict_Absent){ goto _LL724;} else{ goto _LL725;} _LL725: goto
_LL726; _LL724: if(* _temp694){* _temp691=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* key, void** data)) Cyc_Dict_insert)(*
_temp691, x, y);} goto _LL722; _LL726:( void) _throw( _temp721); _LL722:;}}}}}
struct Cyc_Core_Opt* Cyc_Interface_compat_merge_dict( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, void**(* merge_f)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_arr*), struct _tagged_arr t,
struct _tagged_arr* msg){ struct _tuple15 _temp727=({ struct _tuple15 _temp729;
_temp729.f1= 1; _temp729.f2= dic1; _temp729.f3= dic1; _temp729.f4= excl;
_temp729.f5= merge_f; _temp729.f6= t; _temp729.f7= msg; _temp729;});(( void(*)(
void(* f)( struct _tuple15*, struct _tagged_arr*, void**), struct _tuple15* env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_compat_merge_dict_f,&
_temp727, dic2); if( _temp727.f1){ return({ struct Cyc_Core_Opt* _temp728=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp728->v=(
void*) _temp727.f2; _temp728;});} else{ return 0;}} struct Cyc_Interface_Ienv*
Cyc_Interface_compat_merge_ienv( struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv*
ie2, struct Cyc_Interface_Ienv* iexcl, struct _tagged_arr* msg){ struct Cyc_Core_Opt*
_temp730=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Structdecl*(* merge_f)(
struct Cyc_Absyn_Structdecl*, struct Cyc_Absyn_Structdecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->structdecls, ie2->structdecls, iexcl->structdecls, Cyc_Tcdecl_merge_structdecl,
_tag_arr("struct", sizeof( unsigned char), 7u), msg); struct Cyc_Core_Opt*
_temp731=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Uniondecl*(* merge_f)( struct
Cyc_Absyn_Uniondecl*, struct Cyc_Absyn_Uniondecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->uniondecls, ie2->uniondecls, iexcl->uniondecls, Cyc_Tcdecl_merge_uniondecl,
_tag_arr("union", sizeof( unsigned char), 6u), msg); struct Cyc_Core_Opt*
_temp732=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Tuniondecl*(* merge_f)(
struct Cyc_Absyn_Tuniondecl*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->tuniondecls, ie2->tuniondecls, iexcl->tuniondecls, Cyc_Tcdecl_merge_tuniondecl,
_tag_arr("tunion", sizeof( unsigned char), 7u), msg); struct Cyc_Core_Opt*
_temp733=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Enumdecl*(* merge_f)( struct
Cyc_Absyn_Enumdecl*, struct Cyc_Absyn_Enumdecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->enumdecls, ie2->enumdecls, iexcl->enumdecls, Cyc_Tcdecl_merge_enumdecl,
_tag_arr("enum", sizeof( unsigned char), 5u), msg); struct Cyc_Core_Opt*
_temp734=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)(
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls, ie2->typedefdecls, iexcl->typedefdecls, Cyc_Tcdecl_merge_typedefdecl,
_tag_arr("typedef", sizeof( unsigned char), 8u), msg); struct Cyc_Core_Opt*
_temp735=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Vardecl*(* merge_f)( struct
Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->vardecls, ie2->vardecls, iexcl->vardecls, Cyc_Tcdecl_merge_vardecl,
_tag_arr("variable", sizeof( unsigned char), 9u), msg); struct Cyc_Core_Opt*
_temp736=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Tcdecl_Xtunionfielddecl*(* merge_f)(
struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_Tcdecl_Xtunionfielddecl*, struct
Cyc_Position_Segment*, struct _tagged_arr*), struct _tagged_arr t, struct
_tagged_arr* msg)) Cyc_Interface_compat_merge_dict)( ie1->xtunionfielddecls, ie2->xtunionfielddecls,
iexcl->xtunionfielddecls, Cyc_Tcdecl_merge_xtunionfielddecl, _tag_arr("xtunionfield",
sizeof( unsigned char), 13u), msg); if(((((( !(( unsigned int) _temp730)? 1: !((
unsigned int) _temp731))? 1: !(( unsigned int) _temp732))? 1: !(( unsigned int)
_temp733))? 1: !(( unsigned int) _temp734))? 1: !(( unsigned int) _temp735))? 1:
!(( unsigned int) _temp736)){ return 0;} else{ return({ struct Cyc_Interface_Ienv*
_temp737=( struct Cyc_Interface_Ienv*) GC_malloc( sizeof( struct Cyc_Interface_Ienv));
_temp737->structdecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp730))->v; _temp737->uniondecls=( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp731))->v; _temp737->tuniondecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp732))->v; _temp737->enumdecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp733))->v;
_temp737->typedefdecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp734))->v; _temp737->vardecls=( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( _temp735))->v; _temp737->xtunionfielddecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp736))->v; _temp737;});}}
struct _tuple16{ int f1; struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3;
struct Cyc_Dict_Dict* f4; struct _tagged_arr* f5; } ; void Cyc_Interface_compat_merge_ns_f(
struct _tuple16* env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie2){
struct _tuple16 _temp740; struct _tagged_arr* _temp741; struct Cyc_Dict_Dict*
_temp743; struct Cyc_Dict_Dict* _temp745; struct Cyc_Dict_Dict* _temp747; struct
Cyc_Dict_Dict** _temp749; int _temp750; int* _temp752; struct _tuple16* _temp738=
env; _temp740=* _temp738; _LL751: _temp750= _temp740.f1; _temp752=&(* _temp738).f1;
goto _LL748; _LL748: _temp747= _temp740.f2; _temp749=&(* _temp738).f2; goto
_LL746; _LL746: _temp745= _temp740.f3; goto _LL744; _LL744: _temp743= _temp740.f4;
goto _LL742; _LL742: _temp741= _temp740.f5; goto _LL739; _LL739: { struct Cyc_Interface_Ienv*
ie1; struct Cyc_Interface_Ienv* iexcl;{ struct _handler_cons _temp753;
_push_handler(& _temp753);{ int _temp755= 0; if( setjmp( _temp753.handler)){
_temp755= 1;} if( ! _temp755){ iexcl=(( struct Cyc_Interface_Ienv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp743, ns);;
_pop_handler();} else{ void* _temp754=( void*) _exn_thrown; void* _temp757=
_temp754; _LL759: if( _temp757 == Cyc_Dict_Absent){ goto _LL760;} else{ goto
_LL761;} _LL761: goto _LL762; _LL760: iexcl= Cyc_Interface_lazy_new_ienv(); goto
_LL758; _LL762:( void) _throw( _temp757); _LL758:;}}}{ struct _handler_cons
_temp763; _push_handler(& _temp763);{ int _temp765= 0; if( setjmp( _temp763.handler)){
_temp765= 1;} if( ! _temp765){ ie1=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp745, ns);{ struct Cyc_Interface_Ienv*
_temp766= Cyc_Interface_compat_merge_ienv( Cyc_Interface_lazy_new_ienv(), ie1,
iexcl, _temp741); if( _temp766 == 0){* _temp752= 0; _npop_handler( 0u); return;}
ie1=( struct Cyc_Interface_Ienv*) _check_null( _temp766);}; _pop_handler();}
else{ void* _temp764=( void*) _exn_thrown; void* _temp768= _temp764; _LL770: if(
_temp768 == Cyc_Dict_Absent){ goto _LL771;} else{ goto _LL772;} _LL772: goto
_LL773; _LL771: ie1= Cyc_Interface_lazy_new_ienv(); goto _LL769; _LL773:( void)
_throw( _temp768); _LL769:;}}}{ struct Cyc_Interface_Ienv* _temp774= Cyc_Interface_compat_merge_ienv(
ie1, ie2, iexcl, _temp741); if( _temp774 == 0){* _temp752= 0;} else{ if(*
_temp752){* _temp749=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(*
_temp749, ns,( struct Cyc_Interface_Ienv*) _check_null( _temp774));}}}}} struct
Cyc_Core_Opt* Cyc_Interface_compat_merge_ns( struct Cyc_Dict_Dict* dic1, struct
Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct _tagged_arr* msg){
struct _tuple16 _temp775=({ struct _tuple16 _temp777; _temp777.f1= 1; _temp777.f2=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct Cyc_List_List*, struct Cyc_List_List*)))
Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp); _temp777.f3= dic1; _temp777.f4= excl;
_temp777.f5= msg; _temp777;});(( void(*)( void(* f)( struct _tuple16*, struct
Cyc_List_List*, struct Cyc_Interface_Ienv*), struct _tuple16* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_compat_merge_ns_f,& _temp775, dic2); if(
_temp775.f1){ return({ struct Cyc_Core_Opt* _temp776=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp776->v=( void*) _temp775.f2;
_temp776;});} else{ return 0;}} struct _tuple17{ int f1; struct Cyc_Dict_Dict*
f2; struct Cyc_Dict_Dict* f3; struct _tagged_arr f4; struct _tagged_arr* f5; } ;
void Cyc_Interface_disj_merge_dict_f( struct _tuple17* env, struct _tagged_arr*
x, void* y){ struct _tuple17 _temp780; struct _tagged_arr* _temp781; struct
_tagged_arr _temp783; struct Cyc_Dict_Dict* _temp785; struct Cyc_Dict_Dict*
_temp787; struct Cyc_Dict_Dict** _temp789; int _temp790; int* _temp792; struct
_tuple17* _temp778= env; _temp780=* _temp778; _LL791: _temp790= _temp780.f1;
_temp792=&(* _temp778).f1; goto _LL788; _LL788: _temp787= _temp780.f2; _temp789=&(*
_temp778).f2; goto _LL786; _LL786: _temp785= _temp780.f3; goto _LL784; _LL784:
_temp783= _temp780.f4; goto _LL782; _LL782: _temp781= _temp780.f5; goto _LL779;
_LL779: if((( int(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_member)(
_temp785, x)){ Cyc_Interface_check_err( _temp781,( struct _tagged_arr)({ struct
_tagged_arr(* _temp793)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp798= _tag_arr("%s %s is exported more than once",
sizeof( unsigned char), 33u); struct Cyc_Stdio_String_pa_struct _temp800;
_temp800.tag= Cyc_Stdio_String_pa; _temp800.f1=( struct _tagged_arr) _temp783;{
void* _temp796=( void*)& _temp800; struct Cyc_Stdio_String_pa_struct _temp799;
_temp799.tag= Cyc_Stdio_String_pa; _temp799.f1=( struct _tagged_arr)* x;{ void*
_temp797=( void*)& _temp799; void* _temp794[ 2u]={ _temp796, _temp797}; struct
_tagged_arr _temp795={( void*) _temp794,( void*) _temp794,( void*)( _temp794 + 2u)};
_temp793( _temp798, _temp795);}}}));* _temp792= 0;} else{ if(* _temp792){*
_temp789=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key, void* data)) Cyc_Dict_insert)(* _temp789, x, y);}}} struct Cyc_Core_Opt*
Cyc_Interface_disj_merge_dict( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct _tagged_arr t, struct _tagged_arr* msg){ struct _tuple17 _temp801=({
struct _tuple17 _temp803; _temp803.f1= 1; _temp803.f2= dic1; _temp803.f3= dic1;
_temp803.f4= t; _temp803.f5= msg; _temp803;});(( void(*)( void(* f)( struct
_tuple17*, struct _tagged_arr*, void*), struct _tuple17* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_disj_merge_dict_f,& _temp801, dic2); if(
_temp801.f1){ return({ struct Cyc_Core_Opt* _temp802=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp802->v=( void*) _temp801.f2;
_temp802;});} else{ return 0;}} struct Cyc_Interface_Ienv* Cyc_Interface_disj_merge_ienv(
struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv* ie2, struct
_tagged_arr* msg){ struct Cyc_Core_Opt* _temp804=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t,
struct _tagged_arr* msg)) Cyc_Interface_disj_merge_dict)( ie1->structdecls, ie2->structdecls,
_tag_arr("struct", sizeof( unsigned char), 7u), msg); struct Cyc_Core_Opt*
_temp805=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_disj_merge_dict)(
ie1->uniondecls, ie2->uniondecls, _tag_arr("union", sizeof( unsigned char), 6u),
msg); struct Cyc_Core_Opt* _temp806=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t, struct _tagged_arr* msg))
Cyc_Interface_disj_merge_dict)( ie1->tuniondecls, ie2->tuniondecls, _tag_arr("[x]tunion",
sizeof( unsigned char), 10u), msg); struct Cyc_Core_Opt* _temp807=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t,
struct _tagged_arr* msg)) Cyc_Interface_disj_merge_dict)( ie1->enumdecls, ie2->enumdecls,
_tag_arr("enum", sizeof( unsigned char), 5u), msg); struct Cyc_Core_Opt*
_temp808=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)(
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*,
struct _tagged_arr*), struct _tagged_arr t, struct _tagged_arr* msg)) Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls, ie2->typedefdecls,(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp),
Cyc_Tcdecl_merge_typedefdecl, _tag_arr("typedef", sizeof( unsigned char), 8u),
msg); struct Cyc_Core_Opt* _temp809=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t, struct _tagged_arr* msg))
Cyc_Interface_disj_merge_dict)( ie1->vardecls, ie2->vardecls, _tag_arr("variable",
sizeof( unsigned char), 9u), msg); struct Cyc_Core_Opt* _temp810=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr t,
struct _tagged_arr* msg)) Cyc_Interface_disj_merge_dict)( ie1->xtunionfielddecls,
ie2->xtunionfielddecls, _tag_arr("xtunionfield", sizeof( unsigned char), 13u),
msg); if(((((( !(( unsigned int) _temp804)? 1: !(( unsigned int) _temp805))? 1:
!(( unsigned int) _temp806))? 1: !(( unsigned int) _temp807))? 1: !((
unsigned int) _temp808))? 1: !(( unsigned int) _temp809))? 1: !(( unsigned int)
_temp810)){ return 0;} else{ return({ struct Cyc_Interface_Ienv* _temp811=(
struct Cyc_Interface_Ienv*) GC_malloc( sizeof( struct Cyc_Interface_Ienv));
_temp811->structdecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp804))->v; _temp811->uniondecls=( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp805))->v; _temp811->tuniondecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp806))->v; _temp811->enumdecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp807))->v;
_temp811->typedefdecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp808))->v; _temp811->vardecls=( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( _temp809))->v; _temp811->xtunionfielddecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp810))->v; _temp811;});}}
struct _tuple18{ int f1; struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3;
struct _tagged_arr* f4; } ; void Cyc_Interface_disj_merge_ns_f( struct _tuple18*
env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie2){ struct _tuple18
_temp814; struct _tagged_arr* _temp815; struct Cyc_Dict_Dict* _temp817; struct
Cyc_Dict_Dict* _temp819; struct Cyc_Dict_Dict** _temp821; int _temp822; int*
_temp824; struct _tuple18* _temp812= env; _temp814=* _temp812; _LL823: _temp822=
_temp814.f1; _temp824=&(* _temp812).f1; goto _LL820; _LL820: _temp819= _temp814.f2;
_temp821=&(* _temp812).f2; goto _LL818; _LL818: _temp817= _temp814.f3; goto
_LL816; _LL816: _temp815= _temp814.f4; goto _LL813; _LL813: { struct Cyc_Interface_Ienv*
ie1;{ struct _handler_cons _temp825; _push_handler(& _temp825);{ int _temp827= 0;
if( setjmp( _temp825.handler)){ _temp827= 1;} if( ! _temp827){ ie1=(( struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp817,
ns);; _pop_handler();} else{ void* _temp826=( void*) _exn_thrown; void* _temp829=
_temp826; _LL831: if( _temp829 == Cyc_Dict_Absent){ goto _LL832;} else{ goto
_LL833;} _LL833: goto _LL834; _LL832: ie1= Cyc_Interface_lazy_new_ienv(); goto
_LL830; _LL834:( void) _throw( _temp829); _LL830:;}}}{ struct Cyc_Interface_Ienv*
_temp835= Cyc_Interface_disj_merge_ienv( ie1, ie2, _temp815); if( _temp835 == 0){*
_temp824= 0;} else{ if(* _temp824){* _temp821=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Interface_Ienv*
data)) Cyc_Dict_insert)(* _temp821, ns,( struct Cyc_Interface_Ienv*) _check_null(
_temp835));}}}}} struct Cyc_Core_Opt* Cyc_Interface_disj_merge_ns( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_arr* msg){ struct _tuple18
_temp836=({ struct _tuple18 _temp838; _temp838.f1= 1; _temp838.f2= dic1;
_temp838.f3= dic1; _temp838.f4= msg; _temp838;});(( void(*)( void(* f)( struct
_tuple18*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct _tuple18*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_disj_merge_ns_f,&
_temp836, dic2); if( _temp836.f1){ return({ struct Cyc_Core_Opt* _temp837=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp837->v=(
void*) _temp836.f2; _temp837;});} else{ return 0;}} struct Cyc_Interface_I* Cyc_Interface_merge(
struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple3* info){
struct _handler_cons _temp839; _push_handler(& _temp839);{ int _temp841= 0; if(
setjmp( _temp839.handler)){ _temp841= 1;} if( ! _temp841){{ struct _tagged_arr*
msg= 0; if( info != 0){ msg=({ struct _tagged_arr* _temp842=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp842[ 0]=( struct _tagged_arr)({
struct _tagged_arr(* _temp843)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp848= _tag_arr("merging exports of %s and %s,", sizeof(
unsigned char), 30u); struct Cyc_Stdio_String_pa_struct _temp850; _temp850.tag=
Cyc_Stdio_String_pa; _temp850.f1=( struct _tagged_arr)(*(( struct _tuple3*)
_check_null( info))).f1;{ void* _temp846=( void*)& _temp850; struct Cyc_Stdio_String_pa_struct
_temp849; _temp849.tag= Cyc_Stdio_String_pa; _temp849.f1=( struct _tagged_arr)(*((
struct _tuple3*) _check_null( info))).f2;{ void* _temp847=( void*)& _temp849;
void* _temp844[ 2u]={ _temp846, _temp847}; struct _tagged_arr _temp845={( void*)
_temp844,( void*) _temp844,( void*)( _temp844 + 2u)}; _temp843( _temp848,
_temp845);}}}); _temp842;});}{ struct Cyc_Core_Opt* _temp851= Cyc_Interface_disj_merge_ns(
i1->exports, i2->exports, msg); if( _temp851 == 0){ struct Cyc_Interface_I*
_temp852= 0; _npop_handler( 0u); return _temp852;} if( info != 0){ msg=({ struct
_tagged_arr* _temp853=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp853[ 0]=( struct _tagged_arr)({ struct _tagged_arr(*
_temp854)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp859= _tag_arr("merging imports of %s and %s,", sizeof(
unsigned char), 30u); struct Cyc_Stdio_String_pa_struct _temp861; _temp861.tag=
Cyc_Stdio_String_pa; _temp861.f1=( struct _tagged_arr)(*(( struct _tuple3*)
_check_null( info))).f1;{ void* _temp857=( void*)& _temp861; struct Cyc_Stdio_String_pa_struct
_temp860; _temp860.tag= Cyc_Stdio_String_pa; _temp860.f1=( struct _tagged_arr)(*((
struct _tuple3*) _check_null( info))).f2;{ void* _temp858=( void*)& _temp860;
void* _temp855[ 2u]={ _temp857, _temp858}; struct _tagged_arr _temp856={( void*)
_temp855,( void*) _temp855,( void*)( _temp855 + 2u)}; _temp854( _temp859,
_temp856);}}}); _temp853;});}{ struct Cyc_Core_Opt* _temp862= Cyc_Interface_compat_merge_ns(
i1->imports, i2->imports,( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp851))->v, msg); if( _temp862 == 0){ struct Cyc_Interface_I*
_temp863= 0; _npop_handler( 0u); return _temp863;}{ struct Cyc_Interface_I*
_temp865=({ struct Cyc_Interface_I* _temp864=( struct Cyc_Interface_I*)
GC_malloc( sizeof( struct Cyc_Interface_I)); _temp864->imports=( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp862))->v; _temp864->exports=( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp851))->v; _temp864;}); _npop_handler( 0u);
return _temp865;}}}}; _pop_handler();} else{ void* _temp840=( void*) _exn_thrown;
void* _temp867= _temp840; _LL869: if( _temp867 == Cyc_Tcdecl_Incompatible){ goto
_LL870;} else{ goto _LL871;} _LL871: goto _LL872; _LL870: return 0; _LL872:(
void) _throw( _temp867); _LL868:;}}} struct Cyc_Interface_I* Cyc_Interface_merge_list(
struct Cyc_List_List* li, struct Cyc_List_List* linfo){ if( li == 0){ return(
struct Cyc_Interface_I*) Cyc_Interface_empty();}{ struct Cyc_Interface_I* curr_i=(
struct Cyc_Interface_I*)(( struct Cyc_List_List*) _check_null( li))->hd; struct
_tagged_arr* curr_info= linfo != 0?( struct _tagged_arr*)(( struct _tagged_arr*)((
struct Cyc_List_List*) _check_null( linfo))->hd): 0; li=(( struct Cyc_List_List*)
_check_null( li))->tl; if( linfo != 0){ linfo=(( struct Cyc_List_List*)
_check_null( linfo))->tl;} for( 0; li != 0; li=(( struct Cyc_List_List*)
_check_null( li))->tl){ struct Cyc_Interface_I* _temp873= Cyc_Interface_merge(
curr_i,( struct Cyc_Interface_I*)(( struct Cyc_List_List*) _check_null( li))->hd,(
curr_info != 0? linfo != 0: 0)?({ struct _tuple3* _temp883=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp883->f1=*(( struct _tagged_arr*)
_check_null( curr_info)); _temp883->f2=*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( linfo))->hd); _temp883;}): 0); if( _temp873 == 0){ return 0;}
curr_i=( struct Cyc_Interface_I*) _check_null( _temp873); if( curr_info != 0){
curr_info= linfo != 0?({ struct _tagged_arr* _temp874=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp874[ 0]=( struct _tagged_arr)({
struct _tagged_arr(* _temp875)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp880= _tag_arr("%s+%s", sizeof( unsigned char), 6u);
struct Cyc_Stdio_String_pa_struct _temp882; _temp882.tag= Cyc_Stdio_String_pa;
_temp882.f1=( struct _tagged_arr)*(( struct _tagged_arr*) _check_null( curr_info));{
void* _temp878=( void*)& _temp882; struct Cyc_Stdio_String_pa_struct _temp881;
_temp881.tag= Cyc_Stdio_String_pa; _temp881.f1=( struct _tagged_arr)*(( struct
_tagged_arr*)(( struct Cyc_List_List*) _check_null( linfo))->hd);{ void*
_temp879=( void*)& _temp881; void* _temp876[ 2u]={ _temp878, _temp879}; struct
_tagged_arr _temp877={( void*) _temp876,( void*) _temp876,( void*)( _temp876 + 2u)};
_temp875( _temp880, _temp877);}}}); _temp874;}): 0;} if( linfo != 0){ linfo=((
struct Cyc_List_List*) _check_null( linfo))->tl;}} return( struct Cyc_Interface_I*)
curr_i;}} struct Cyc_Interface_I* Cyc_Interface_get_and_merge_list( struct Cyc_Interface_I*(*
get)( void*), struct Cyc_List_List* la, struct Cyc_List_List* linfo){ if( la ==
0){ return( struct Cyc_Interface_I*) Cyc_Interface_empty();}{ struct Cyc_Interface_I*
curr_i= get(( void*)(( struct Cyc_List_List*) _check_null( la))->hd); struct
_tagged_arr* curr_info= linfo != 0?( struct _tagged_arr*)(( struct _tagged_arr*)((
struct Cyc_List_List*) _check_null( linfo))->hd): 0; la=(( struct Cyc_List_List*)
_check_null( la))->tl; if( linfo != 0){ linfo=(( struct Cyc_List_List*)
_check_null( linfo))->tl;} for( 0; la != 0; la=(( struct Cyc_List_List*)
_check_null( la))->tl){ struct Cyc_Interface_I* _temp884= Cyc_Interface_merge(
curr_i, get(( void*)(( struct Cyc_List_List*) _check_null( la))->hd),( curr_info
!= 0? linfo != 0: 0)?({ struct _tuple3* _temp894=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp894->f1=*(( struct _tagged_arr*) _check_null(
curr_info)); _temp894->f2=*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( linfo))->hd); _temp894;}): 0); if( _temp884 == 0){ return 0;}
curr_i=( struct Cyc_Interface_I*) _check_null( _temp884); if( curr_info != 0){
curr_info= linfo != 0?({ struct _tagged_arr* _temp885=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp885[ 0]=( struct _tagged_arr)({
struct _tagged_arr(* _temp886)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp891= _tag_arr("%s+%s", sizeof( unsigned char), 6u);
struct Cyc_Stdio_String_pa_struct _temp893; _temp893.tag= Cyc_Stdio_String_pa;
_temp893.f1=( struct _tagged_arr)*(( struct _tagged_arr*) _check_null( curr_info));{
void* _temp889=( void*)& _temp893; struct Cyc_Stdio_String_pa_struct _temp892;
_temp892.tag= Cyc_Stdio_String_pa; _temp892.f1=( struct _tagged_arr)*(( struct
_tagged_arr*)(( struct Cyc_List_List*) _check_null( linfo))->hd);{ void*
_temp890=( void*)& _temp892; void* _temp887[ 2u]={ _temp889, _temp890}; struct
_tagged_arr _temp888={( void*) _temp887,( void*) _temp887,( void*)( _temp887 + 2u)};
_temp886( _temp891, _temp888);}}}); _temp885;}): 0;} if( linfo != 0){ linfo=((
struct Cyc_List_List*) _check_null( linfo))->tl;}} return( struct Cyc_Interface_I*)
curr_i;}} static struct Cyc_List_List* Cyc_Interface_add_namespace( struct Cyc_List_List*
tds, struct Cyc_List_List* ns){ if( ns == 0){ return tds;} return({ struct Cyc_List_List*
_temp895=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp895->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Namespace_d_struct*
_temp896=( struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp896[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp897; _temp897.tag= Cyc_Absyn_Namespace_d;
_temp897.f1=( struct _tagged_arr*)(( struct Cyc_List_List*) _check_null( ns))->hd;
_temp897.f2= Cyc_Interface_add_namespace( tds,(( struct Cyc_List_List*)
_check_null( ns))->tl); _temp897;}); _temp896;}), 0); _temp895->tl= 0; _temp895;});}
static struct Cyc_List_List* Cyc_Interface_add_structdecl( struct _tagged_arr* x,
struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp898=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp898->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp899=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp899[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp900; _temp900.tag= Cyc_Absyn_Struct_d;
_temp900.f1= d; _temp900;}); _temp899;}), 0); _temp898->tl= tds; _temp898;});}
static struct Cyc_List_List* Cyc_Interface_add_structdecl_header( struct
_tagged_arr* x, struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Structdecl* _temp901=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp901[ 0]=* d; _temp901;}); d->fields=
0; if(( void*) d->sc !=( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)((
void*) Cyc_Absyn_Extern));} return({ struct Cyc_List_List* _temp902=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp902->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp903=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp903[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp904; _temp904.tag= Cyc_Absyn_Struct_d; _temp904.f1=
d; _temp904;}); _temp903;}), 0); _temp902->tl= tds; _temp902;});} static struct
Cyc_List_List* Cyc_Interface_add_uniondecl( struct _tagged_arr* x, struct Cyc_Absyn_Uniondecl*
d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List* _temp905=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp905->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Union_d_struct* _temp906=( struct
Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp906[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp907; _temp907.tag= Cyc_Absyn_Union_d;
_temp907.f1= d; _temp907;}); _temp906;}), 0); _temp905->tl= tds; _temp905;});}
static struct Cyc_List_List* Cyc_Interface_add_uniondecl_header( struct
_tagged_arr* x, struct Cyc_Absyn_Uniondecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Uniondecl* _temp908=( struct Cyc_Absyn_Uniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp908[ 0]=* d; _temp908;}); d->fields=
0; if(( void*) d->sc !=( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)((
void*) Cyc_Absyn_Extern));} return({ struct Cyc_List_List* _temp909=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp909->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Union_d_struct* _temp910=( struct Cyc_Absyn_Union_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct)); _temp910[ 0]=({ struct Cyc_Absyn_Union_d_struct
_temp911; _temp911.tag= Cyc_Absyn_Union_d; _temp911.f1= d; _temp911;}); _temp910;}),
0); _temp909->tl= tds; _temp909;});} static struct Cyc_List_List* Cyc_Interface_add_tuniondecl(
struct _tagged_arr* x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){
return({ struct Cyc_List_List* _temp912=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp912->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Tunion_d_struct* _temp913=( struct Cyc_Absyn_Tunion_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct)); _temp913[ 0]=({ struct
Cyc_Absyn_Tunion_d_struct _temp914; _temp914.tag= Cyc_Absyn_Tunion_d; _temp914.f1=
d; _temp914;}); _temp913;}), 0); _temp912->tl= tds; _temp912;});} static
unsigned char _temp915[ 2u]="_"; static struct _tagged_arr Cyc_Interface_us={
_temp915, _temp915, _temp915 + 2u}; static struct _tagged_arr* Cyc_Interface_us_p=&
Cyc_Interface_us; static struct _tuple10* Cyc_Interface_rewrite_tunionfield_type(
struct _tuple10* x){ return({ struct _tuple10* _temp916=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp916->f1= Cyc_Absyn_empty_tqual();
_temp916->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp917=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp917[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp918; _temp918.tag= Cyc_Absyn_VarType; _temp918.f1=({ struct Cyc_Absyn_Tvar*
_temp919=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp919->name= Cyc_Interface_us_p; _temp919->identity= 0; _temp919->kind= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_BoxKind); _temp919;}); _temp918;}); _temp917;}); _temp916;});}
static struct Cyc_Absyn_Tunionfield* Cyc_Interface_rewrite_tunionfield( struct
Cyc_Absyn_Tunionfield* f){ f=({ struct Cyc_Absyn_Tunionfield* _temp920=( struct
Cyc_Absyn_Tunionfield*) GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield));
_temp920[ 0]=* f; _temp920;}); f->typs=(( struct Cyc_List_List*(*)( struct
_tuple10*(* f)( struct _tuple10*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Interface_rewrite_tunionfield_type,
f->typs); return f;} static struct Cyc_List_List* Cyc_Interface_add_tuniondecl_header(
struct _tagged_arr* x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){
d=({ struct Cyc_Absyn_Tuniondecl* _temp921=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp921[ 0]=* d; _temp921;});
if( d->fields != 0){ d->fields=({ struct Cyc_Core_Opt* _temp922=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp922->v=( void*)(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tunionfield*(* f)( struct Cyc_Absyn_Tunionfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Interface_rewrite_tunionfield,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( d->fields))->v); _temp922;});} if(( void*) d->sc
!=( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern));}
return({ struct Cyc_List_List* _temp923=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp923->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Tunion_d_struct* _temp924=( struct Cyc_Absyn_Tunion_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct)); _temp924[ 0]=({ struct
Cyc_Absyn_Tunion_d_struct _temp925; _temp925.tag= Cyc_Absyn_Tunion_d; _temp925.f1=
d; _temp925;}); _temp924;}), 0); _temp923->tl= tds; _temp923;});} static struct
Cyc_List_List* Cyc_Interface_add_enumdecl( struct _tagged_arr* x, struct Cyc_Absyn_Enumdecl*
d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List* _temp926=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp926->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Enum_d_struct* _temp927=( struct
Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp927[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp928; _temp928.tag= Cyc_Absyn_Enum_d;
_temp928.f1= d; _temp928;}); _temp927;}), 0); _temp926->tl= tds; _temp926;});}
static struct Cyc_List_List* Cyc_Interface_add_enumdecl_header( struct
_tagged_arr* x, struct Cyc_Absyn_Enumdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Enumdecl* _temp929=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp929[ 0]=* d; _temp929;}); d->fields= 0;
if(( void*) d->sc !=( void*) Cyc_Absyn_ExternC){( void*)( d->sc=( void*)(( void*)
Cyc_Absyn_Extern));} return({ struct Cyc_List_List* _temp930=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp930->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Enum_d_struct* _temp931=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp931[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp932; _temp932.tag= Cyc_Absyn_Enum_d; _temp932.f1= d; _temp932;}); _temp931;}),
0); _temp930->tl= tds; _temp930;});} static struct Cyc_List_List* Cyc_Interface_add_typedef(
struct _tagged_arr* x, struct Cyc_Absyn_Typedefdecl* d, struct Cyc_List_List*
tds){ return({ struct Cyc_List_List* _temp933=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp933->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Typedef_d_struct* _temp934=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp934[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp935; _temp935.tag= Cyc_Absyn_Typedef_d; _temp935.f1=
d; _temp935;}); _temp934;}), 0); _temp933->tl= tds; _temp933;});} static struct
Cyc_List_List* Cyc_Interface_add_vardecl( struct _tagged_arr* x, struct Cyc_Absyn_Vardecl*
d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List* _temp936=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp936->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp937=( struct
Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp937[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp938; _temp938.tag= Cyc_Absyn_Var_d;
_temp938.f1= d; _temp938;}); _temp937;}), 0); _temp936->tl= tds; _temp936;});}
static struct Cyc_List_List* Cyc_Interface_add_xtunionfielddecl( struct
_tagged_arr* x, struct Cyc_Tcdecl_Xtunionfielddecl* d, struct Cyc_List_List* tds){
struct Cyc_Tcdecl_Xtunionfielddecl _temp941; struct Cyc_Absyn_Tunionfield*
_temp942; struct Cyc_Absyn_Tuniondecl* _temp944; struct Cyc_Tcdecl_Xtunionfielddecl*
_temp939= d; _temp941=* _temp939; _LL945: _temp944= _temp941.base; goto _LL943;
_LL943: _temp942= _temp941.field; goto _LL940; _LL940: _temp944=({ struct Cyc_Absyn_Tuniondecl*
_temp946=( struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp946[ 0]=* _temp944; _temp946;}); _temp944->fields=({ struct Cyc_Core_Opt*
_temp947=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp947->v=( void*)({ struct Cyc_List_List* _temp948=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp948->hd=( void*) _temp942;
_temp948->tl= 0; _temp948;}); _temp947;});( void*)( _temp944->sc=( void*)(( void*)
Cyc_Absyn_Extern)); return({ struct Cyc_List_List* _temp949=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp949->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp950=( struct Cyc_Absyn_Tunion_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct)); _temp950[ 0]=({ struct
Cyc_Absyn_Tunion_d_struct _temp951; _temp951.tag= Cyc_Absyn_Tunion_d; _temp951.f1=
_temp944; _temp951;}); _temp950;}), 0); _temp949->tl= tds; _temp949;});} static
struct Cyc_List_List* Cyc_Interface_add_xtunionfielddecl_header( struct
_tagged_arr* x, struct Cyc_Tcdecl_Xtunionfielddecl* d, struct Cyc_List_List* tds){
struct Cyc_Tcdecl_Xtunionfielddecl _temp954; struct Cyc_Absyn_Tunionfield*
_temp955; struct Cyc_Absyn_Tuniondecl* _temp957; struct Cyc_Tcdecl_Xtunionfielddecl*
_temp952= d; _temp954=* _temp952; _LL958: _temp957= _temp954.base; goto _LL956;
_LL956: _temp955= _temp954.field; goto _LL953; _LL953: _temp957=({ struct Cyc_Absyn_Tuniondecl*
_temp959=( struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp959[ 0]=* _temp957; _temp959;}); _temp955= Cyc_Interface_rewrite_tunionfield(
_temp955);( void*)( _temp955->sc=( void*)(( void*) Cyc_Absyn_Extern)); _temp957->fields=({
struct Cyc_Core_Opt* _temp960=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp960->v=( void*)({ struct Cyc_List_List* _temp961=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp961->hd=( void*)
_temp955; _temp961->tl= 0; _temp961;}); _temp960;});( void*)( _temp957->sc=(
void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List* _temp962=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp962->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp963=(
struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp963[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp964; _temp964.tag= Cyc_Absyn_Tunion_d;
_temp964.f1= _temp957; _temp964;}); _temp963;}), 0); _temp962->tl= tds; _temp962;});}
static void Cyc_Interface_print_ns_headers( struct Cyc_Stdio___sFILE* f, struct
Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie){ struct Cyc_List_List*
_temp965= 0; _temp965=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)(
struct _tagged_arr*, struct Cyc_Absyn_Structdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_structdecl_header,
ie->structdecls, _temp965); _temp965=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl_header,
ie->uniondecls, _temp965); _temp965=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl_header,
ie->tuniondecls, _temp965); _temp965=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl_header,
ie->enumdecls, _temp965); if( _temp965 != 0){ _temp965=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp965); _temp965= Cyc_Interface_add_namespace(
_temp965, ns); Cyc_Absynpp_decllist2file( _temp965, f);}} static void Cyc_Interface_print_ns_xtunionfielddecl_headers(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp966= 0; _temp966=(( struct Cyc_List_List*(*)(
struct Cyc_List_List*(* f)( struct _tagged_arr*, struct Cyc_Tcdecl_Xtunionfielddecl*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)( Cyc_Interface_add_xtunionfielddecl_header, ie->xtunionfielddecls,
_temp966); if( _temp966 != 0){ _temp966=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp966); _temp966= Cyc_Interface_add_namespace(
_temp966, ns); Cyc_Absynpp_decllist2file( _temp966, f);}} static void Cyc_Interface_print_ns_typedefs(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp967= 0; _temp967=(( struct Cyc_List_List*(*)(
struct Cyc_List_List*(* f)( struct _tagged_arr*, struct Cyc_Absyn_Typedefdecl*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)( Cyc_Interface_add_typedef, ie->typedefdecls, _temp967); if(
_temp967 != 0){ _temp967=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( _temp967); _temp967= Cyc_Interface_add_namespace( _temp967,
ns); Cyc_Absynpp_decllist2file( _temp967, f);}} static void Cyc_Interface_print_ns_decls(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp968= 0; _temp968=(( struct Cyc_List_List*(*)(
struct Cyc_List_List*(* f)( struct _tagged_arr*, struct Cyc_Absyn_Structdecl*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)( Cyc_Interface_add_structdecl, ie->structdecls, _temp968);
_temp968=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct
_tagged_arr*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict*
d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl, ie->uniondecls,
_temp968); _temp968=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)(
struct _tagged_arr*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl,
ie->tuniondecls, _temp968); _temp968=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl,
ie->enumdecls, _temp968); _temp968=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Absyn_Vardecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_vardecl,
ie->vardecls, _temp968); _temp968=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_arr*, struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_xtunionfielddecl,
ie->xtunionfielddecls, _temp968); if( _temp968 != 0){ _temp968=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp968); _temp968= Cyc_Interface_add_namespace(
_temp968, ns); Cyc_Absynpp_decllist2file( _temp968, f);}} void Cyc_Interface_print(
struct Cyc_Interface_I* i, struct Cyc_Stdio___sFILE* f){ Cyc_Absynpp_set_params(&
Cyc_Absynpp_cyci_params_r);({ int(* _temp969)( struct Cyc_Stdio___sFILE*, struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp972= f; struct _tagged_arr _temp973= _tag_arr("/****** needed (headers) ******/\n",
sizeof( unsigned char), 34u); void* _temp970[ 0u]={}; struct _tagged_arr
_temp971={( void*) _temp970,( void*) _temp970,( void*)( _temp970 + 0u)};
_temp969( _temp972, _temp973, _temp971);});(( void(*)( void(* f)( struct Cyc_Stdio___sFILE*,
struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct Cyc_Stdio___sFILE*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_headers,
f, i->imports);({ int(* _temp974)( struct Cyc_Stdio___sFILE*, struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp977=
f; struct _tagged_arr _temp978= _tag_arr("\n/****** provided (headers) ******/\n",
sizeof( unsigned char), 37u); void* _temp975[ 0u]={}; struct _tagged_arr
_temp976={( void*) _temp975,( void*) _temp975,( void*)( _temp975 + 0u)};
_temp974( _temp977, _temp978, _temp976);});(( void(*)( void(* f)( struct Cyc_Stdio___sFILE*,
struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct Cyc_Stdio___sFILE*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_headers,
f, i->exports);({ int(* _temp979)( struct Cyc_Stdio___sFILE*, struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp982=
f; struct _tagged_arr _temp983= _tag_arr("/****** needed (headers of xtunionfielddecls) ******/\n",
sizeof( unsigned char), 55u); void* _temp980[ 0u]={}; struct _tagged_arr
_temp981={( void*) _temp980,( void*) _temp980,( void*)( _temp980 + 0u)};
_temp979( _temp982, _temp983, _temp981);});(( void(*)( void(* f)( struct Cyc_Stdio___sFILE*,
struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct Cyc_Stdio___sFILE*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_xtunionfielddecl_headers,
f, i->imports);({ int(* _temp984)( struct Cyc_Stdio___sFILE*, struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp987=
f; struct _tagged_arr _temp988= _tag_arr("\n/****** provided (headers of xtunionfielddecls) ******/\n",
sizeof( unsigned char), 58u); void* _temp985[ 0u]={}; struct _tagged_arr
_temp986={( void*) _temp985,( void*) _temp985,( void*)( _temp985 + 0u)};
_temp984( _temp987, _temp988, _temp986);});(( void(*)( void(* f)( struct Cyc_Stdio___sFILE*,
struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct Cyc_Stdio___sFILE*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_xtunionfielddecl_headers,
f, i->exports);({ int(* _temp989)( struct Cyc_Stdio___sFILE*, struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp992=
f; struct _tagged_arr _temp993= _tag_arr("\n/****** provided (typedefs) ******/\n",
sizeof( unsigned char), 38u); void* _temp990[ 0u]={}; struct _tagged_arr
_temp991={( void*) _temp990,( void*) _temp990,( void*)( _temp990 + 0u)};
_temp989( _temp992, _temp993, _temp991);});(( void(*)( void(* f)( struct Cyc_Stdio___sFILE*,
struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct Cyc_Stdio___sFILE*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_typedefs,
f, i->exports);({ int(* _temp994)( struct Cyc_Stdio___sFILE*, struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp997=
f; struct _tagged_arr _temp998= _tag_arr("\n/****** needed (declarations) ******/\n",
sizeof( unsigned char), 40u); void* _temp995[ 0u]={}; struct _tagged_arr
_temp996={( void*) _temp995,( void*) _temp995,( void*)( _temp995 + 0u)};
_temp994( _temp997, _temp998, _temp996);});(( void(*)( void(* f)( struct Cyc_Stdio___sFILE*,
struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct Cyc_Stdio___sFILE*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_decls, f,
i->imports);({ int(* _temp999)( struct Cyc_Stdio___sFILE*, struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp1002=
f; struct _tagged_arr _temp1003= _tag_arr("\n/****** provided (declarations) ******/\n",
sizeof( unsigned char), 42u); void* _temp1000[ 0u]={}; struct _tagged_arr
_temp1001={( void*) _temp1000,( void*) _temp1000,( void*)( _temp1000 + 0u)};
_temp999( _temp1002, _temp1003, _temp1001);});(( void(*)( void(* f)( struct Cyc_Stdio___sFILE*,
struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct Cyc_Stdio___sFILE*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_decls, f,
i->exports);} struct Cyc_Interface_I* Cyc_Interface_parse( struct Cyc_Stdio___sFILE*
f){ Cyc_Lex_lex_init();{ struct Cyc_List_List* _temp1004= Cyc_Parse_parse_file(
f); struct Cyc_Tcenv_Tenv* _temp1005= Cyc_Tcenv_tc_init(); Cyc_Tc_tc( _temp1005,
0, _temp1004); return Cyc_Interface_gen_extract( _temp1005->ae, 0);}} void Cyc_Interface_save(
struct Cyc_Interface_I* i, struct Cyc_Stdio___sFILE* f){ Cyc_Interface_print( i,
f);} struct Cyc_Interface_I* Cyc_Interface_load( struct Cyc_Stdio___sFILE* f){
return Cyc_Interface_parse( f);}