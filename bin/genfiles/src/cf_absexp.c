 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_dirent{ int d_ino; int d_off; unsigned short
d_reclen; unsigned char d_type; unsigned char d_name[ 256u]; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern
struct _tagged_arr Cyc_Core_new_string( int); extern int Cyc_Core_intcmp( int,
int); extern int Cyc_Core_ptrcmp( void**, void**); extern unsigned char Cyc_Core_InvalidArg[
15u]; struct Cyc_Core_InvalidArg_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
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
int tag; unsigned int* f1; } ; extern struct _tagged_arr Cyc_Stdio_aprintf(
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
} ; extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_rappend( struct
_RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y); extern
unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_List_memq( struct Cyc_List_List*
l, void* x); extern int Cyc_String_zstrptrcmp( struct _tagged_arr*, struct
_tagged_arr*); struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u];
extern unsigned char Cyc_Dict_Absent[ 11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty(
int(* comp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern void* Cyc_Dict_lookup(
struct Cyc_Dict_Dict* d, void* key); extern int Cyc_Dict_lookup_bool( struct Cyc_Dict_Dict*
d, void* key, void** ans_place); extern void* Cyc_Dict_fold2_c( void*(* f)( void*,
void*, void*, void*, void*), void* inner_env, struct Cyc_Dict_Dict* d1, struct
Cyc_Dict_Dict* d2, void* accum); struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[
11u]; struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct _tagged_arr
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
static const int Cyc_Absyn_TaggedArray_ps= 0; static const int Cyc_Absyn_Printf_ft=
0; static const int Cyc_Absyn_Scanf_ft= 1; static const int Cyc_Absyn_Regparm_att=
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
Cyc_Absyn_Format_att= 3; struct Cyc_Absyn_Format_att_struct{ int tag; void* f1;
int f2; int f3; } ; static const int Cyc_Absyn_Carray_mod= 0; static const int
Cyc_Absyn_ConstArray_mod= 0; struct Cyc_Absyn_ConstArray_mod_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Pointer_mod= 1; struct
Cyc_Absyn_Pointer_mod_struct{ int tag; void* f1; void* f2; struct Cyc_Absyn_Tqual
f3; } ; static const int Cyc_Absyn_Function_mod= 2; struct Cyc_Absyn_Function_mod_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypeParams_mod= 3; struct Cyc_Absyn_TypeParams_mod_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; int f3; } ;
static const int Cyc_Absyn_Attributes_mod= 4; struct Cyc_Absyn_Attributes_mod_struct{
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
19; static const int Cyc_Absyn_PreInc= 0; static const int Cyc_Absyn_PostInc= 1;
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
15u]; static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap);
extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*, struct _tagged_arr
fmt, struct _tagged_arr ap); extern int Cyc_Tcutil_bits_only( void* t); struct
Cyc_CfFlowInfo_Place; static const int Cyc_CfFlowInfo_VarRoot= 0; struct Cyc_CfFlowInfo_VarRoot_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_CfFlowInfo_MallocPt=
1; struct Cyc_CfFlowInfo_MallocPt_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_CfFlowInfo_StructF= 0; struct Cyc_CfFlowInfo_StructF_struct{
int tag; struct _tagged_arr* f1; } ; static const int Cyc_CfFlowInfo_TupleF= 1;
struct Cyc_CfFlowInfo_TupleF_struct{ int tag; int f1; } ; struct Cyc_CfFlowInfo_Place{
void* root; struct Cyc_List_List* fields; } ; static const int Cyc_CfFlowInfo_Esc=
0; static const int Cyc_CfFlowInfo_Unesc= 1; static const int Cyc_CfFlowInfo_NoneIL=
0; static const int Cyc_CfFlowInfo_ThisIL= 1; static const int Cyc_CfFlowInfo_AllIL=
2; static const int Cyc_CfFlowInfo_UnknownIS= 0; struct Cyc_CfFlowInfo_UnknownIS_struct{
int tag; void* f1; void* f2; } ; static const int Cyc_CfFlowInfo_MustPointTo= 1;
struct Cyc_CfFlowInfo_MustPointTo_struct{ int tag; struct Cyc_CfFlowInfo_Place*
f1; } ; static const int Cyc_CfFlowInfo_LeafPI= 0; struct Cyc_CfFlowInfo_LeafPI_struct{
int tag; void* f1; } ; static const int Cyc_CfFlowInfo_TuplePI= 1; struct Cyc_CfFlowInfo_TuplePI_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_StructPI=
2; struct Cyc_CfFlowInfo_StructPI_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
static const int Cyc_CfFlowInfo_BottomFL= 0; static const int Cyc_CfFlowInfo_InitsFL=
0; struct Cyc_CfFlowInfo_InitsFL_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
extern void* Cyc_CfFlowInfo_mkLeafPI( void* esc, void* il); extern void* Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place); extern struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_insert_place( struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place*
place, void* pinfo); extern int Cyc_CfFlowInfo_isAllInit( struct Cyc_Dict_Dict*
pinfo_dict, void* pinfo); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_pointsto(
void* pinfo, struct Cyc_Dict_Dict* d); extern void* Cyc_CfFlowInfo_assign_unknown_dict(
void*, void*); extern void* Cyc_CfFlowInfo_join_flow( void*, void*); extern int
Cyc_CfFlowInfo_flow_lessthan_approx( void* f1, void* f2); struct Cyc_NewControlFlow_AnalEnv{
struct Cyc_Dict_Dict* roots; int in_try; void* tryflow; } ; extern void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv* env, void* new_flow); extern void* Cyc_NewControlFlow_cf_analyze_stmt(
struct Cyc_NewControlFlow_AnalEnv*, struct Cyc_Absyn_Stmt*, void*); extern
unsigned char Cyc_CfAbsexp_BadAbsexp[ 14u]; static const int Cyc_CfAbsexp_OrderedG=
0; static const int Cyc_CfAbsexp_UnorderedG= 1; static const int Cyc_CfAbsexp_OneofG=
2; extern void Cyc_CfAbsexp_check_absexp( void*); extern struct _tagged_arr Cyc_CfAbsexp_absexp2string(
void* ae, int depth); extern void* Cyc_CfAbsexp_mkAnyOrderG(); extern void* Cyc_CfAbsexp_mkUnknownOp();
extern void* Cyc_CfAbsexp_mkAddressOp( void* ao); extern void* Cyc_CfAbsexp_mkLocalOp(
struct Cyc_Absyn_Vardecl* vd); extern void* Cyc_CfAbsexp_mkMemberOp( void* ao,
void* f); extern void* Cyc_CfAbsexp_mkMallocOp( struct Cyc_Absyn_Exp* e); extern
void* Cyc_CfAbsexp_mkDerefOp( void* ao); extern void* Cyc_CfAbsexp_mkBottomAE();
extern void* Cyc_CfAbsexp_mkSkipAE(); extern void* Cyc_CfAbsexp_mkUseAE( void*
ao); extern void* Cyc_CfAbsexp_mkAssignAE( void* l, void* r); extern void* Cyc_CfAbsexp_mkMallocAE(
struct Cyc_Absyn_Exp* e); extern void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt*
s); extern void* Cyc_CfAbsexp_mkGroupAE( void* g, void* ae1, void* ae2); extern
void* Cyc_CfAbsexp_mkGroupAE_l( void* g, struct Cyc_List_List* ael); extern int
Cyc_CfAbsexp_isUnknownOp( void*); extern void* Cyc_CfAbsexp_eval_absexp( struct
Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Position_Segment* loc, void* ae,
void* in_flow); unsigned char Cyc_CfAbsexp_BadAbsexp[ 14u]="\000\000\000\000BadAbsexp";
void* Cyc_CfAbsexp_mkAnyOrderG(){ return( void*) Cyc_CfAbsexp_OrderedG;} static
const int Cyc_CfAbsexp_BottomAE= 0; static const int Cyc_CfAbsexp_SkipAE= 1;
static const int Cyc_CfAbsexp_UseAE= 0; struct Cyc_CfAbsexp_UseAE_struct{ int
tag; void* f1; } ; static const int Cyc_CfAbsexp_AssignAE= 1; struct Cyc_CfAbsexp_AssignAE_struct{
int tag; void* f1; void* f2; } ; static const int Cyc_CfAbsexp_MallocAE= 2;
struct Cyc_CfAbsexp_MallocAE_struct{ int tag; void* f1; int f2; } ; static const
int Cyc_CfAbsexp_StmtAE= 3; struct Cyc_CfAbsexp_StmtAE_struct{ int tag; struct
Cyc_Absyn_Stmt* f1; } ; static const int Cyc_CfAbsexp_GroupAE= 4; struct Cyc_CfAbsexp_GroupAE_struct{
int tag; void* f1; struct Cyc_List_List* f2; } ; static const int Cyc_CfAbsexp_UnknownOp=
0; static const int Cyc_CfAbsexp_AddressOp= 0; struct Cyc_CfAbsexp_AddressOp_struct{
int tag; void* f1; } ; static const int Cyc_CfAbsexp_DerefOp= 1; struct Cyc_CfAbsexp_DerefOp_struct{
int tag; void* f1; } ; static const int Cyc_CfAbsexp_MemberOp= 2; struct Cyc_CfAbsexp_MemberOp_struct{
int tag; void* f1; void* f2; } ; static const int Cyc_CfAbsexp_LocalOp= 3;
struct Cyc_CfAbsexp_LocalOp_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_CfAbsexp_MallocOp= 4; struct Cyc_CfAbsexp_MallocOp_struct{
int tag; struct Cyc_Absyn_Exp* f1; int f2; } ; static void Cyc_CfAbsexp_ok_address_arg(
void* ao){ void* _temp0= ao; _LL2: if(( unsigned int) _temp0 > 1u?*(( int*)
_temp0) == Cyc_CfAbsexp_LocalOp: 0){ goto _LL3;} else{ goto _LL4;} _LL4: if((
unsigned int) _temp0 > 1u?*(( int*) _temp0) == Cyc_CfAbsexp_MemberOp: 0){ goto
_LL5;} else{ goto _LL6;} _LL6: if(( unsigned int) _temp0 > 1u?*(( int*) _temp0)
== Cyc_CfAbsexp_MallocOp: 0){ goto _LL7;} else{ goto _LL8;} _LL8: goto _LL9;
_LL3: return; _LL5: return; _LL7: return; _LL9:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp);
_LL1:;} static void Cyc_CfAbsexp_ok_member_arg( void* ao){ void* _temp10= ao;
_LL12: if(( unsigned int) _temp10 > 1u?*(( int*) _temp10) == Cyc_CfAbsexp_LocalOp:
0){ goto _LL13;} else{ goto _LL14;} _LL14: if(( unsigned int) _temp10 > 1u?*((
int*) _temp10) == Cyc_CfAbsexp_MemberOp: 0){ goto _LL15;} else{ goto _LL16;}
_LL16: if(( unsigned int) _temp10 > 1u?*(( int*) _temp10) == Cyc_CfAbsexp_MallocOp:
0){ goto _LL17;} else{ goto _LL18;} _LL18: if(( unsigned int) _temp10 > 1u?*((
int*) _temp10) == Cyc_CfAbsexp_DerefOp: 0){ goto _LL19;} else{ goto _LL20;}
_LL20: goto _LL21; _LL13: return; _LL15: return; _LL17: return; _LL19: return;
_LL21:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL11:;} static void Cyc_CfAbsexp_ok_deref_arg(
void* ao){ void* _temp22= ao; _LL24: if(( unsigned int) _temp22 > 1u?*(( int*)
_temp22) == Cyc_CfAbsexp_LocalOp: 0){ goto _LL25;} else{ goto _LL26;} _LL26: if((
unsigned int) _temp22 > 1u?*(( int*) _temp22) == Cyc_CfAbsexp_MemberOp: 0){ goto
_LL27;} else{ goto _LL28;} _LL28: if(( unsigned int) _temp22 > 1u?*(( int*)
_temp22) == Cyc_CfAbsexp_DerefOp: 0){ goto _LL29;} else{ goto _LL30;} _LL30:
goto _LL31; _LL25: return; _LL27: return; _LL29: return; _LL31:( int) _throw((
void*) Cyc_CfAbsexp_BadAbsexp); _LL23:;} static void Cyc_CfAbsexp_ok_lvalue(
void* ao){ void* _temp32= ao; _LL34: if( _temp32 ==( void*) Cyc_CfAbsexp_UnknownOp){
goto _LL35;} else{ goto _LL36;} _LL36: if(( unsigned int) _temp32 > 1u?*(( int*)
_temp32) == Cyc_CfAbsexp_AddressOp: 0){ goto _LL37;} else{ goto _LL38;} _LL38:
goto _LL39; _LL35: goto _LL37; _LL37:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp);
_LL39: return; _LL33:;} static int Cyc_CfAbsexp_num_mallocpts= 0; static struct
Cyc_Dict_Dict** Cyc_CfAbsexp_mallocpt_dict= 0; int Cyc_CfAbsexp_mallocpt_int(
struct Cyc_Absyn_Exp* e){ if( Cyc_CfAbsexp_mallocpt_dict == 0){ struct Cyc_Dict_Dict*
d=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*)))
Cyc_Dict_empty)(( int(*)( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*)) Cyc_Core_ptrcmp);
Cyc_CfAbsexp_mallocpt_dict=({ struct Cyc_Dict_Dict** _temp40=( struct Cyc_Dict_Dict**)
GC_malloc( sizeof( struct Cyc_Dict_Dict*) * 1); _temp40[ 0]= d; _temp40;});}{
int i= 0; if( !(( int(*)( struct Cyc_Dict_Dict* d, struct Cyc_Absyn_Exp* key,
int* ans_place)) Cyc_Dict_lookup_bool)(*(( struct Cyc_Dict_Dict**) _check_null(
Cyc_CfAbsexp_mallocpt_dict)), e,& i)){*(( struct Cyc_Dict_Dict**) _check_null(
Cyc_CfAbsexp_mallocpt_dict))=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_Absyn_Exp* key, int data)) Cyc_Dict_insert)(*(( struct Cyc_Dict_Dict**)
_check_null( Cyc_CfAbsexp_mallocpt_dict)), e,( i= ++ Cyc_CfAbsexp_num_mallocpts));}
return i;}} void* Cyc_CfAbsexp_mkUnknownOp(){ return( void*) Cyc_CfAbsexp_UnknownOp;}
void* Cyc_CfAbsexp_mkAddressOp( void* ao){ Cyc_CfAbsexp_ok_address_arg( ao);
return( void*)({ struct Cyc_CfAbsexp_AddressOp_struct* _temp41=( struct Cyc_CfAbsexp_AddressOp_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_AddressOp_struct)); _temp41[ 0]=({ struct
Cyc_CfAbsexp_AddressOp_struct _temp42; _temp42.tag= Cyc_CfAbsexp_AddressOp;
_temp42.f1=( void*) ao; _temp42;}); _temp41;});} void* Cyc_CfAbsexp_mkLocalOp(
struct Cyc_Absyn_Vardecl* vd){ return( void*)({ struct Cyc_CfAbsexp_LocalOp_struct*
_temp43=( struct Cyc_CfAbsexp_LocalOp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_LocalOp_struct));
_temp43[ 0]=({ struct Cyc_CfAbsexp_LocalOp_struct _temp44; _temp44.tag= Cyc_CfAbsexp_LocalOp;
_temp44.f1= vd; _temp44;}); _temp43;});} void* Cyc_CfAbsexp_mkMemberOp( void* ao,
void* f){ Cyc_CfAbsexp_ok_member_arg( ao); return( void*)({ struct Cyc_CfAbsexp_MemberOp_struct*
_temp45=( struct Cyc_CfAbsexp_MemberOp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_MemberOp_struct));
_temp45[ 0]=({ struct Cyc_CfAbsexp_MemberOp_struct _temp46; _temp46.tag= Cyc_CfAbsexp_MemberOp;
_temp46.f1=( void*) ao; _temp46.f2=( void*) f; _temp46;}); _temp45;});} void*
Cyc_CfAbsexp_mkMallocOp( struct Cyc_Absyn_Exp* e){ return( void*)({ struct Cyc_CfAbsexp_MallocOp_struct*
_temp47=( struct Cyc_CfAbsexp_MallocOp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_MallocOp_struct));
_temp47[ 0]=({ struct Cyc_CfAbsexp_MallocOp_struct _temp48; _temp48.tag= Cyc_CfAbsexp_MallocOp;
_temp48.f1= e; _temp48.f2= Cyc_CfAbsexp_mallocpt_int( e); _temp48;}); _temp47;});}
void* Cyc_CfAbsexp_mkDerefOp( void* ao){ Cyc_CfAbsexp_ok_deref_arg( ao); return(
void*)({ struct Cyc_CfAbsexp_DerefOp_struct* _temp49=( struct Cyc_CfAbsexp_DerefOp_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_DerefOp_struct)); _temp49[ 0]=({ struct
Cyc_CfAbsexp_DerefOp_struct _temp50; _temp50.tag= Cyc_CfAbsexp_DerefOp; _temp50.f1=(
void*) ao; _temp50;}); _temp49;});} void* Cyc_CfAbsexp_mkBottomAE(){ return(
void*) Cyc_CfAbsexp_BottomAE;} void* Cyc_CfAbsexp_mkSkipAE(){ return( void*) Cyc_CfAbsexp_SkipAE;}
void* Cyc_CfAbsexp_mkMallocAE( struct Cyc_Absyn_Exp* e){ return( void*)({ struct
Cyc_CfAbsexp_MallocAE_struct* _temp51=( struct Cyc_CfAbsexp_MallocAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_MallocAE_struct)); _temp51[ 0]=({ struct
Cyc_CfAbsexp_MallocAE_struct _temp52; _temp52.tag= Cyc_CfAbsexp_MallocAE;
_temp52.f1=( void*)(( void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp53=(
struct Cyc_CfFlowInfo_MallocPt_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp53[ 0]=({ struct Cyc_CfFlowInfo_MallocPt_struct _temp54; _temp54.tag= Cyc_CfFlowInfo_MallocPt;
_temp54.f1= e; _temp54;}); _temp53;})); _temp52.f2= Cyc_CfAbsexp_mallocpt_int( e);
_temp52;}); _temp51;});} void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt* s){
return( void*)({ struct Cyc_CfAbsexp_StmtAE_struct* _temp55=( struct Cyc_CfAbsexp_StmtAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_StmtAE_struct)); _temp55[ 0]=({ struct
Cyc_CfAbsexp_StmtAE_struct _temp56; _temp56.tag= Cyc_CfAbsexp_StmtAE; _temp56.f1=
s; _temp56;}); _temp55;});} void* Cyc_CfAbsexp_mkUseAE( void* ao){ void* _temp57=
ao; _LL59: if( _temp57 ==( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL60;} else{
goto _LL61;} _LL61: goto _LL62; _LL60: return( void*) Cyc_CfAbsexp_SkipAE; _LL62:
return( void*)({ struct Cyc_CfAbsexp_UseAE_struct* _temp63=( struct Cyc_CfAbsexp_UseAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_UseAE_struct)); _temp63[ 0]=({ struct Cyc_CfAbsexp_UseAE_struct
_temp64; _temp64.tag= Cyc_CfAbsexp_UseAE; _temp64.f1=( void*) ao; _temp64;});
_temp63;}); _LL58:;} struct _tuple3{ void* f1; void* f2; } ; void* Cyc_CfAbsexp_mkAssignAE(
void* l, void* r){ struct _tuple3 _temp66=({ struct _tuple3 _temp65; _temp65.f1=
l; _temp65.f2= r; _temp65;}); void* _temp74; void* _temp76; void* _temp78; _LL68:
_LL77: _temp76= _temp66.f1; if( _temp76 ==( void*) Cyc_CfAbsexp_UnknownOp){ goto
_LL75;} else{ goto _LL70;} _LL75: _temp74= _temp66.f2; if( _temp74 ==( void*)
Cyc_CfAbsexp_UnknownOp){ goto _LL69;} else{ goto _LL70;} _LL70: _LL79: _temp78=
_temp66.f1; if( _temp78 ==( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL71;} else{
goto _LL72;} _LL72: goto _LL73; _LL69: return( void*) Cyc_CfAbsexp_SkipAE; _LL71:
return Cyc_CfAbsexp_mkUseAE( r); _LL73: Cyc_CfAbsexp_ok_lvalue( l); return( void*)({
struct Cyc_CfAbsexp_AssignAE_struct* _temp80=( struct Cyc_CfAbsexp_AssignAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_AssignAE_struct)); _temp80[ 0]=({ struct
Cyc_CfAbsexp_AssignAE_struct _temp81; _temp81.tag= Cyc_CfAbsexp_AssignAE;
_temp81.f1=( void*) l; _temp81.f2=( void*) r; _temp81;}); _temp80;}); _LL67:;}
struct _tuple4{ void* f1; void* f2; void* f3; } ; void* Cyc_CfAbsexp_mkGroupAE(
void* g, void* ae1, void* ae2){ struct _tuple4 _temp83=({ struct _tuple4 _temp82;
_temp82.f1= g; _temp82.f2= ae1; _temp82.f3= ae2; _temp82;}); void* _temp106;
void* _temp109; void* _temp111; void* _temp113; void* _temp115; void* _temp117;
void* _temp119; void* _temp121; void* _temp124; struct Cyc_List_List* _temp126;
void* _temp128; void* _temp130; struct Cyc_List_List* _temp132; void* _temp134;
void* _temp136; void* _temp139; struct Cyc_List_List* _temp141; void* _temp143;
void* _temp145; void* _temp147; struct Cyc_List_List* _temp149; void* _temp151;
void* _temp153; void* _temp156; struct Cyc_List_List* _temp158; void* _temp160;
void* _temp162; _LL85: _LL107: _temp106= _temp83.f3; if( _temp106 ==( void*) Cyc_CfAbsexp_SkipAE){
goto _LL105;} else{ goto _LL87;} _LL105: if( g !=( void*) Cyc_CfAbsexp_OneofG){
goto _LL86;} else{ goto _LL87;} _LL87: _LL110: _temp109= _temp83.f2; if(
_temp109 ==( void*) Cyc_CfAbsexp_SkipAE){ goto _LL108;} else{ goto _LL89;}
_LL108: if( g !=( void*) Cyc_CfAbsexp_OneofG){ goto _LL88;} else{ goto _LL89;}
_LL89: _LL114: _temp113= _temp83.f1; if( _temp113 ==( void*) Cyc_CfAbsexp_OneofG){
goto _LL112;} else{ goto _LL91;} _LL112: _temp111= _temp83.f3; if( _temp111 ==(
void*) Cyc_CfAbsexp_BottomAE){ goto _LL90;} else{ goto _LL91;} _LL91: _LL118:
_temp117= _temp83.f1; if( _temp117 ==( void*) Cyc_CfAbsexp_OneofG){ goto _LL116;}
else{ goto _LL93;} _LL116: _temp115= _temp83.f2; if( _temp115 ==( void*) Cyc_CfAbsexp_BottomAE){
goto _LL92;} else{ goto _LL93;} _LL93: _LL122: _temp121= _temp83.f1; if(
_temp121 ==( void*) Cyc_CfAbsexp_OrderedG){ goto _LL120;} else{ goto _LL95;}
_LL120: _temp119= _temp83.f2; if( _temp119 ==( void*) Cyc_CfAbsexp_BottomAE){
goto _LL94;} else{ goto _LL95;} _LL95: _LL137: _temp136= _temp83.f1; goto _LL131;
_LL131: _temp130= _temp83.f2; if(( unsigned int) _temp130 > 2u?*(( int*)
_temp130) == Cyc_CfAbsexp_GroupAE: 0){ _LL135: _temp134=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp130)->f1; goto _LL133; _LL133: _temp132=(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp130)->f2; goto _LL125;} else{ goto _LL97;} _LL125: _temp124= _temp83.f3;
if(( unsigned int) _temp124 > 2u?*(( int*) _temp124) == Cyc_CfAbsexp_GroupAE: 0){
_LL129: _temp128=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp124)->f1;
goto _LL127; _LL127: _temp126=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp124)->f2;
goto _LL123;} else{ goto _LL97;} _LL123: if( _temp136 == _temp134? _temp136 ==
_temp128: 0){ goto _LL96;} else{ goto _LL97;} _LL97: _LL146: _temp145= _temp83.f1;
goto _LL140; _LL140: _temp139= _temp83.f2; if(( unsigned int) _temp139 > 2u?*((
int*) _temp139) == Cyc_CfAbsexp_GroupAE: 0){ _LL144: _temp143=( void*)(( struct
Cyc_CfAbsexp_GroupAE_struct*) _temp139)->f1; goto _LL142; _LL142: _temp141=((
struct Cyc_CfAbsexp_GroupAE_struct*) _temp139)->f2; goto _LL138;} else{ goto
_LL99;} _LL138: if( _temp145 == _temp143? _temp145 !=( void*) Cyc_CfAbsexp_OrderedG:
0){ goto _LL98;} else{ goto _LL99;} _LL99: _LL154: _temp153= _temp83.f1; if(
_temp153 ==( void*) Cyc_CfAbsexp_OrderedG){ goto _LL148;} else{ goto _LL101;}
_LL148: _temp147= _temp83.f2; if(( unsigned int) _temp147 > 2u?*(( int*)
_temp147) == Cyc_CfAbsexp_GroupAE: 0){ _LL152: _temp151=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp147)->f1; if( _temp151 ==( void*) Cyc_CfAbsexp_OrderedG){ goto _LL150;}
else{ goto _LL101;} _LL150: _temp149=(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp147)->f2; goto _LL100;} else{ goto _LL101;} _LL101: _LL163: _temp162=
_temp83.f1; goto _LL157; _LL157: _temp156= _temp83.f3; if(( unsigned int)
_temp156 > 2u?*(( int*) _temp156) == Cyc_CfAbsexp_GroupAE: 0){ _LL161: _temp160=(
void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp156)->f1; goto _LL159; _LL159:
_temp158=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp156)->f2; goto _LL155;}
else{ goto _LL103;} _LL155: if( _temp162 == _temp160){ goto _LL102;} else{ goto
_LL103;} _LL103: goto _LL104; _LL86: return ae1; _LL88: return ae2; _LL90:
return ae1; _LL92: return ae2; _LL94: return( void*) Cyc_CfAbsexp_BottomAE;
_LL96: return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct* _temp164=( struct
Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp164[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp165; _temp165.tag= Cyc_CfAbsexp_GroupAE;
_temp165.f1=( void*) _temp136; _temp165.f2= Cyc_List_append( _temp132, _temp126);
_temp165;}); _temp164;}); _LL98: return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp166=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp166[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp167; _temp167.tag= Cyc_CfAbsexp_GroupAE;
_temp167.f1=( void*) _temp145; _temp167.f2=({ struct Cyc_List_List* _temp168=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp168[
0]=({ struct Cyc_List_List _temp169; _temp169.hd=( void*) ae2; _temp169.tl=
_temp141; _temp169;}); _temp168;}); _temp167;}); _temp166;}); _LL100: return(
void*)({ struct Cyc_CfAbsexp_GroupAE_struct* _temp170=( struct Cyc_CfAbsexp_GroupAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct)); _temp170[ 0]=({ struct
Cyc_CfAbsexp_GroupAE_struct _temp171; _temp171.tag= Cyc_CfAbsexp_GroupAE;
_temp171.f1=( void*)(( void*) Cyc_CfAbsexp_OrderedG); _temp171.f2= Cyc_List_append(
_temp149,({ struct Cyc_List_List* _temp172=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List) * 1); _temp172[ 0]=({ struct Cyc_List_List
_temp173; _temp173.hd=( void*) ae2; _temp173.tl= 0; _temp173;}); _temp172;}));
_temp171;}); _temp170;}); _LL102: return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp174=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp174[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp175; _temp175.tag= Cyc_CfAbsexp_GroupAE;
_temp175.f1=( void*) _temp162; _temp175.f2=({ struct Cyc_List_List* _temp176=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp176[
0]=({ struct Cyc_List_List _temp177; _temp177.hd=( void*) ae1; _temp177.tl=
_temp158; _temp177;}); _temp176;}); _temp175;}); _temp174;}); _LL104: return(
void*)({ struct Cyc_CfAbsexp_GroupAE_struct* _temp178=( struct Cyc_CfAbsexp_GroupAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct)); _temp178[ 0]=({ struct
Cyc_CfAbsexp_GroupAE_struct _temp179; _temp179.tag= Cyc_CfAbsexp_GroupAE;
_temp179.f1=( void*) g; _temp179.f2=({ struct Cyc_List_List* _temp180=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp180[ 0]=({
struct Cyc_List_List _temp181; _temp181.hd=( void*) ae1; _temp181.tl=({ struct
Cyc_List_List* _temp182=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)
* 1); _temp182[ 0]=({ struct Cyc_List_List _temp183; _temp183.hd=( void*) ae2;
_temp183.tl= 0; _temp183;}); _temp182;}); _temp181;}); _temp180;}); _temp179;});
_temp178;}); _LL84:;} void* Cyc_CfAbsexp_mkGroupAE_l( void* g, struct Cyc_List_List*
ael){ if( ael == 0){ return( void*) Cyc_CfAbsexp_SkipAE;} if((( struct Cyc_List_List*)
_check_null( ael))->tl == 0){ return( void*)(( struct Cyc_List_List*)
_check_null( ael))->hd;} return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp184=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp184[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp185; _temp185.tag= Cyc_CfAbsexp_GroupAE;
_temp185.f1=( void*) g; _temp185.f2= ael; _temp185;}); _temp184;});} int Cyc_CfAbsexp_isUnknownOp(
void* ao){ return ao ==( void*) Cyc_CfAbsexp_UnknownOp;} struct _tagged_arr Cyc_CfAbsexp_absop2string(
void* ao){ void* _temp186= ao; void* _temp200; struct Cyc_Absyn_Vardecl*
_temp202; void* _temp204; void* _temp206; int _temp208; struct Cyc_Absyn_Exp*
_temp210; void* _temp212; _LL188: if( _temp186 ==( void*) Cyc_CfAbsexp_UnknownOp){
goto _LL189;} else{ goto _LL190;} _LL190: if(( unsigned int) _temp186 > 1u?*((
int*) _temp186) == Cyc_CfAbsexp_AddressOp: 0){ _LL201: _temp200=( void*)((
struct Cyc_CfAbsexp_AddressOp_struct*) _temp186)->f1; goto _LL191;} else{ goto
_LL192;} _LL192: if(( unsigned int) _temp186 > 1u?*(( int*) _temp186) == Cyc_CfAbsexp_LocalOp:
0){ _LL203: _temp202=(( struct Cyc_CfAbsexp_LocalOp_struct*) _temp186)->f1; goto
_LL193;} else{ goto _LL194;} _LL194: if(( unsigned int) _temp186 > 1u?*(( int*)
_temp186) == Cyc_CfAbsexp_MemberOp: 0){ _LL207: _temp206=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp186)->f1; goto _LL205; _LL205: _temp204=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp186)->f2; goto _LL195;} else{ goto _LL196;} _LL196: if(( unsigned int)
_temp186 > 1u?*(( int*) _temp186) == Cyc_CfAbsexp_MallocOp: 0){ _LL211: _temp210=((
struct Cyc_CfAbsexp_MallocOp_struct*) _temp186)->f1; goto _LL209; _LL209:
_temp208=(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp186)->f2; goto _LL197;}
else{ goto _LL198;} _LL198: if(( unsigned int) _temp186 > 1u?*(( int*) _temp186)
== Cyc_CfAbsexp_DerefOp: 0){ _LL213: _temp212=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*)
_temp186)->f1; goto _LL199;} else{ goto _LL187;} _LL189: return _tag_arr("Unknown",
sizeof( unsigned char), 8u); _LL191: return( struct _tagged_arr)({ struct
_tagged_arr(* _temp214)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp218= _tag_arr("& %s", sizeof( unsigned char), 5u);
struct Cyc_Stdio_String_pa_struct _temp219; _temp219.tag= Cyc_Stdio_String_pa;
_temp219.f1=( struct _tagged_arr) Cyc_CfAbsexp_absop2string( _temp200);{ void*
_temp217=( void*)& _temp219; void* _temp215[ 1u]={ _temp217}; struct _tagged_arr
_temp216={( void*) _temp215,( void*) _temp215,( void*)( _temp215 + 1u)};
_temp214( _temp218, _temp216);}}); _LL193: return*(* _temp202->name).f2; _LL195: {
struct _tagged_arr fs;{ void* _temp220= _temp204; struct _tagged_arr* _temp226;
int _temp228; _LL222: if(*(( int*) _temp220) == Cyc_CfFlowInfo_StructF){ _LL227:
_temp226=(( struct Cyc_CfFlowInfo_StructF_struct*) _temp220)->f1; goto _LL223;}
else{ goto _LL224;} _LL224: if(*(( int*) _temp220) == Cyc_CfFlowInfo_TupleF){
_LL229: _temp228=(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp220)->f1; goto
_LL225;} else{ goto _LL221;} _LL223: fs=* _temp226; goto _LL221; _LL225: fs=(
struct _tagged_arr)({ struct _tagged_arr(* _temp230)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp234= _tag_arr("%d",
sizeof( unsigned char), 3u); struct Cyc_Stdio_Int_pa_struct _temp235; _temp235.tag=
Cyc_Stdio_Int_pa; _temp235.f1=( int)(( unsigned int) _temp228);{ void* _temp233=(
void*)& _temp235; void* _temp231[ 1u]={ _temp233}; struct _tagged_arr _temp232={(
void*) _temp231,( void*) _temp231,( void*)( _temp231 + 1u)}; _temp230( _temp234,
_temp232);}}); goto _LL221; _LL221:;} return( struct _tagged_arr)({ struct
_tagged_arr(* _temp236)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp241= _tag_arr("%s.%s", sizeof( unsigned char), 6u);
struct Cyc_Stdio_String_pa_struct _temp243; _temp243.tag= Cyc_Stdio_String_pa;
_temp243.f1=( struct _tagged_arr) Cyc_CfAbsexp_absop2string( _temp206);{ void*
_temp239=( void*)& _temp243; struct Cyc_Stdio_String_pa_struct _temp242;
_temp242.tag= Cyc_Stdio_String_pa; _temp242.f1=( struct _tagged_arr) fs;{ void*
_temp240=( void*)& _temp242; void* _temp237[ 2u]={ _temp239, _temp240}; struct
_tagged_arr _temp238={( void*) _temp237,( void*) _temp237,( void*)( _temp237 + 2u)};
_temp236( _temp241, _temp238);}}});} _LL197: return( struct _tagged_arr)({
struct _tagged_arr(* _temp244)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp248= _tag_arr("<mpt%d>", sizeof( unsigned char), 8u);
struct Cyc_Stdio_Int_pa_struct _temp249; _temp249.tag= Cyc_Stdio_Int_pa;
_temp249.f1=( int)(( unsigned int) _temp208);{ void* _temp247=( void*)& _temp249;
void* _temp245[ 1u]={ _temp247}; struct _tagged_arr _temp246={( void*) _temp245,(
void*) _temp245,( void*)( _temp245 + 1u)}; _temp244( _temp248, _temp246);}});
_LL199: return( struct _tagged_arr)({ struct _tagged_arr(* _temp250)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp254= _tag_arr("(*%s)", sizeof( unsigned char), 6u); struct Cyc_Stdio_String_pa_struct
_temp255; _temp255.tag= Cyc_Stdio_String_pa; _temp255.f1=( struct _tagged_arr)
Cyc_CfAbsexp_absop2string( _temp212);{ void* _temp253=( void*)& _temp255; void*
_temp251[ 1u]={ _temp253}; struct _tagged_arr _temp252={( void*) _temp251,( void*)
_temp251,( void*)( _temp251 + 1u)}; _temp250( _temp254, _temp252);}}); _LL187:;}
struct _tagged_arr Cyc_CfAbsexp_absexp2string( void* ae, int depth){ struct
_tagged_arr ans= Cyc_Core_new_string( depth);{ int i= 0; for( 0; i <
_get_arr_size( ans, sizeof( unsigned char)); ++ i){*(( unsigned char*)
_check_unknown_subscript( ans, sizeof( unsigned char), i))=' ';}}{ void*
_temp256= ae; void* _temp274; int _temp276; void* _temp278; struct Cyc_Absyn_Exp*
_temp280; void* _temp282; void* _temp284; struct Cyc_List_List* _temp286; void*
_temp288; struct Cyc_Absyn_Stmt* _temp290; _LL258: if( _temp256 ==( void*) Cyc_CfAbsexp_SkipAE){
goto _LL259;} else{ goto _LL260;} _LL260: if(( unsigned int) _temp256 > 2u?*((
int*) _temp256) == Cyc_CfAbsexp_UseAE: 0){ _LL275: _temp274=( void*)(( struct
Cyc_CfAbsexp_UseAE_struct*) _temp256)->f1; goto _LL261;} else{ goto _LL262;}
_LL262: if(( unsigned int) _temp256 > 2u?*(( int*) _temp256) == Cyc_CfAbsexp_MallocAE:
0){ _LL279: _temp278=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp256)->f1;
if(*(( int*) _temp278) == Cyc_CfFlowInfo_MallocPt){ _LL281: _temp280=(( struct
Cyc_CfFlowInfo_MallocPt_struct*) _temp278)->f1; goto _LL277;} else{ goto _LL264;}
_LL277: _temp276=(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp256)->f2; goto
_LL263;} else{ goto _LL264;} _LL264: if(( unsigned int) _temp256 > 2u?*(( int*)
_temp256) == Cyc_CfAbsexp_AssignAE: 0){ _LL285: _temp284=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp256)->f1; goto _LL283; _LL283: _temp282=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp256)->f2; goto _LL265;} else{ goto _LL266;} _LL266: if(( unsigned int)
_temp256 > 2u?*(( int*) _temp256) == Cyc_CfAbsexp_GroupAE: 0){ _LL289: _temp288=(
void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp256)->f1; goto _LL287; _LL287:
_temp286=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp256)->f2; goto _LL267;}
else{ goto _LL268;} _LL268: if(( unsigned int) _temp256 > 2u?*(( int*) _temp256)
== Cyc_CfAbsexp_StmtAE: 0){ _LL291: _temp290=(( struct Cyc_CfAbsexp_StmtAE_struct*)
_temp256)->f1; goto _LL269;} else{ goto _LL270;} _LL270: if( _temp256 ==( void*)
Cyc_CfAbsexp_BottomAE){ goto _LL271;} else{ goto _LL272;} _LL272: goto _LL273;
_LL259: ans=({ struct _tagged_arr(* _temp292)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp296= _tag_arr("%sskip",
sizeof( unsigned char), 7u); struct Cyc_Stdio_String_pa_struct _temp297;
_temp297.tag= Cyc_Stdio_String_pa; _temp297.f1=( struct _tagged_arr) ans;{ void*
_temp295=( void*)& _temp297; void* _temp293[ 1u]={ _temp295}; struct _tagged_arr
_temp294={( void*) _temp293,( void*) _temp293,( void*)( _temp293 + 1u)};
_temp292( _temp296, _temp294);}}); goto _LL257; _LL261: ans=({ struct
_tagged_arr(* _temp298)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp303= _tag_arr("%suse %s", sizeof( unsigned char), 9u);
struct Cyc_Stdio_String_pa_struct _temp305; _temp305.tag= Cyc_Stdio_String_pa;
_temp305.f1=( struct _tagged_arr) ans;{ void* _temp301=( void*)& _temp305;
struct Cyc_Stdio_String_pa_struct _temp304; _temp304.tag= Cyc_Stdio_String_pa;
_temp304.f1=( struct _tagged_arr) Cyc_CfAbsexp_absop2string( _temp274);{ void*
_temp302=( void*)& _temp304; void* _temp299[ 2u]={ _temp301, _temp302}; struct
_tagged_arr _temp300={( void*) _temp299,( void*) _temp299,( void*)( _temp299 + 2u)};
_temp298( _temp303, _temp300);}}}); goto _LL257; _LL263: ans=({ struct
_tagged_arr(* _temp306)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp311= _tag_arr("%smalloc %i", sizeof( unsigned char), 12u);
struct Cyc_Stdio_String_pa_struct _temp313; _temp313.tag= Cyc_Stdio_String_pa;
_temp313.f1=( struct _tagged_arr) ans;{ void* _temp309=( void*)& _temp313;
struct Cyc_Stdio_Int_pa_struct _temp312; _temp312.tag= Cyc_Stdio_Int_pa;
_temp312.f1=( int)(( unsigned int) _temp276);{ void* _temp310=( void*)& _temp312;
void* _temp307[ 2u]={ _temp309, _temp310}; struct _tagged_arr _temp308={( void*)
_temp307,( void*) _temp307,( void*)( _temp307 + 2u)}; _temp306( _temp311,
_temp308);}}}); goto _LL257; _LL265: ans=({ struct _tagged_arr(* _temp314)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp320= _tag_arr("%sassign %s %s", sizeof( unsigned char), 15u);
struct Cyc_Stdio_String_pa_struct _temp323; _temp323.tag= Cyc_Stdio_String_pa;
_temp323.f1=( struct _tagged_arr) ans;{ void* _temp317=( void*)& _temp323;
struct Cyc_Stdio_String_pa_struct _temp322; _temp322.tag= Cyc_Stdio_String_pa;
_temp322.f1=( struct _tagged_arr) Cyc_CfAbsexp_absop2string( _temp284);{ void*
_temp318=( void*)& _temp322; struct Cyc_Stdio_String_pa_struct _temp321;
_temp321.tag= Cyc_Stdio_String_pa; _temp321.f1=( struct _tagged_arr) Cyc_CfAbsexp_absop2string(
_temp282);{ void* _temp319=( void*)& _temp321; void* _temp315[ 3u]={ _temp317,
_temp318, _temp319}; struct _tagged_arr _temp316={( void*) _temp315,( void*)
_temp315,( void*)( _temp315 + 3u)}; _temp314( _temp320, _temp316);}}}}); goto
_LL257; _LL267:{ void* _temp324= _temp288; _LL326: if( _temp324 ==( void*) Cyc_CfAbsexp_OrderedG){
goto _LL327;} else{ goto _LL328;} _LL328: if( _temp324 ==( void*) Cyc_CfAbsexp_UnorderedG){
goto _LL329;} else{ goto _LL330;} _LL330: if( _temp324 ==( void*) Cyc_CfAbsexp_OneofG){
goto _LL331;} else{ goto _LL325;} _LL327: ans=({ struct _tagged_arr(* _temp332)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp336= _tag_arr("%sordered", sizeof( unsigned char), 10u); struct
Cyc_Stdio_String_pa_struct _temp337; _temp337.tag= Cyc_Stdio_String_pa; _temp337.f1=(
struct _tagged_arr) ans;{ void* _temp335=( void*)& _temp337; void* _temp333[ 1u]={
_temp335}; struct _tagged_arr _temp334={( void*) _temp333,( void*) _temp333,(
void*)( _temp333 + 1u)}; _temp332( _temp336, _temp334);}}); goto _LL325; _LL329:
ans=({ struct _tagged_arr(* _temp338)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp342= _tag_arr("%sunordered",
sizeof( unsigned char), 12u); struct Cyc_Stdio_String_pa_struct _temp343;
_temp343.tag= Cyc_Stdio_String_pa; _temp343.f1=( struct _tagged_arr) ans;{ void*
_temp341=( void*)& _temp343; void* _temp339[ 1u]={ _temp341}; struct _tagged_arr
_temp340={( void*) _temp339,( void*) _temp339,( void*)( _temp339 + 1u)};
_temp338( _temp342, _temp340);}}); goto _LL325; _LL331: ans=({ struct
_tagged_arr(* _temp344)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp348= _tag_arr("%soneof", sizeof( unsigned char), 8u);
struct Cyc_Stdio_String_pa_struct _temp349; _temp349.tag= Cyc_Stdio_String_pa;
_temp349.f1=( struct _tagged_arr) ans;{ void* _temp347=( void*)& _temp349; void*
_temp345[ 1u]={ _temp347}; struct _tagged_arr _temp346={( void*) _temp345,( void*)
_temp345,( void*)( _temp345 + 1u)}; _temp344( _temp348, _temp346);}}); goto
_LL325; _LL325:;} for( 0; _temp286 != 0; _temp286=(( struct Cyc_List_List*)
_check_null( _temp286))->tl){ ans=({ struct _tagged_arr(* _temp350)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp355= _tag_arr("%s\n%s", sizeof( unsigned char), 6u); struct Cyc_Stdio_String_pa_struct
_temp357; _temp357.tag= Cyc_Stdio_String_pa; _temp357.f1=( struct _tagged_arr)
ans;{ void* _temp353=( void*)& _temp357; struct Cyc_Stdio_String_pa_struct
_temp356; _temp356.tag= Cyc_Stdio_String_pa; _temp356.f1=( struct _tagged_arr)
Cyc_CfAbsexp_absexp2string(( void*)(( struct Cyc_List_List*) _check_null(
_temp286))->hd, depth + 1);{ void* _temp354=( void*)& _temp356; void* _temp351[
2u]={ _temp353, _temp354}; struct _tagged_arr _temp352={( void*) _temp351,( void*)
_temp351,( void*)( _temp351 + 2u)}; _temp350( _temp355, _temp352);}}});} goto
_LL257; _LL269: ans=({ struct _tagged_arr(* _temp358)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp362= _tag_arr("%s<<stmt>>",
sizeof( unsigned char), 11u); struct Cyc_Stdio_String_pa_struct _temp363;
_temp363.tag= Cyc_Stdio_String_pa; _temp363.f1=( struct _tagged_arr) ans;{ void*
_temp361=( void*)& _temp363; void* _temp359[ 1u]={ _temp361}; struct _tagged_arr
_temp360={( void*) _temp359,( void*) _temp359,( void*)( _temp359 + 1u)};
_temp358( _temp362, _temp360);}}); goto _LL257; _LL271: ans=({ struct
_tagged_arr(* _temp364)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp368= _tag_arr("%sbottom", sizeof( unsigned char), 9u);
struct Cyc_Stdio_String_pa_struct _temp369; _temp369.tag= Cyc_Stdio_String_pa;
_temp369.f1=( struct _tagged_arr) ans;{ void* _temp367=( void*)& _temp369; void*
_temp365[ 1u]={ _temp367}; struct _tagged_arr _temp366={( void*) _temp365,( void*)
_temp365,( void*)( _temp365 + 1u)}; _temp364( _temp368, _temp366);}}); goto
_LL257; _LL273:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL257:;} return(
struct _tagged_arr) ans;} static void Cyc_CfAbsexp_check_absop( void* ao){ void*
_temp370= ao; void* _temp384; void* _temp386; void* _temp388; _LL372: if(
_temp370 ==( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL373;} else{ goto _LL374;}
_LL374: if(( unsigned int) _temp370 > 1u?*(( int*) _temp370) == Cyc_CfAbsexp_AddressOp:
0){ _LL385: _temp384=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*) _temp370)->f1;
goto _LL375;} else{ goto _LL376;} _LL376: if(( unsigned int) _temp370 > 1u?*((
int*) _temp370) == Cyc_CfAbsexp_DerefOp: 0){ _LL387: _temp386=( void*)(( struct
Cyc_CfAbsexp_DerefOp_struct*) _temp370)->f1; goto _LL377;} else{ goto _LL378;}
_LL378: if(( unsigned int) _temp370 > 1u?*(( int*) _temp370) == Cyc_CfAbsexp_MemberOp:
0){ _LL389: _temp388=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp370)->f1;
goto _LL379;} else{ goto _LL380;} _LL380: if(( unsigned int) _temp370 > 1u?*((
int*) _temp370) == Cyc_CfAbsexp_LocalOp: 0){ goto _LL381;} else{ goto _LL382;}
_LL382: if(( unsigned int) _temp370 > 1u?*(( int*) _temp370) == Cyc_CfAbsexp_MallocOp:
0){ goto _LL383;} else{ goto _LL371;} _LL373: return; _LL375: Cyc_CfAbsexp_check_absop(
_temp384); Cyc_CfAbsexp_ok_address_arg( _temp384); return; _LL377: Cyc_CfAbsexp_check_absop(
_temp386); Cyc_CfAbsexp_ok_deref_arg( _temp386); return; _LL379: Cyc_CfAbsexp_check_absop(
_temp388); Cyc_CfAbsexp_ok_member_arg( _temp388); return; _LL381: return; _LL383:
return; _LL371:;} void Cyc_CfAbsexp_check_absexp( void* ae){ void* _temp390= ae;
void* _temp408; struct Cyc_Absyn_Stmt* _temp410; struct Cyc_List_List* _temp412;
void* _temp414; void* _temp416; void* _temp418; _LL392: if( _temp390 ==( void*)
Cyc_CfAbsexp_BottomAE){ goto _LL393;} else{ goto _LL394;} _LL394: if( _temp390
==( void*) Cyc_CfAbsexp_SkipAE){ goto _LL395;} else{ goto _LL396;} _LL396: if((
unsigned int) _temp390 > 2u?*(( int*) _temp390) == Cyc_CfAbsexp_MallocAE: 0){
_LL409: _temp408=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp390)->f1;
if(*(( int*) _temp408) == Cyc_CfFlowInfo_VarRoot){ goto _LL397;} else{ goto
_LL398;}} else{ goto _LL398;} _LL398: if(( unsigned int) _temp390 > 2u?*(( int*)
_temp390) == Cyc_CfAbsexp_MallocAE: 0){ goto _LL399;} else{ goto _LL400;} _LL400:
if(( unsigned int) _temp390 > 2u?*(( int*) _temp390) == Cyc_CfAbsexp_StmtAE: 0){
_LL411: _temp410=(( struct Cyc_CfAbsexp_StmtAE_struct*) _temp390)->f1; goto
_LL401;} else{ goto _LL402;} _LL402: if(( unsigned int) _temp390 > 2u?*(( int*)
_temp390) == Cyc_CfAbsexp_GroupAE: 0){ _LL413: _temp412=(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp390)->f2; goto _LL403;} else{ goto _LL404;} _LL404: if(( unsigned int)
_temp390 > 2u?*(( int*) _temp390) == Cyc_CfAbsexp_UseAE: 0){ _LL415: _temp414=(
void*)(( struct Cyc_CfAbsexp_UseAE_struct*) _temp390)->f1; goto _LL405;} else{
goto _LL406;} _LL406: if(( unsigned int) _temp390 > 2u?*(( int*) _temp390) ==
Cyc_CfAbsexp_AssignAE: 0){ _LL419: _temp418=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp390)->f1; goto _LL417; _LL417: _temp416=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp390)->f2; goto _LL407;} else{ goto _LL391;} _LL393: return; _LL395: return;
_LL397:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL399: return; _LL401:
return; _LL403: for( 0; _temp412 != 0; _temp412=(( struct Cyc_List_List*)
_check_null( _temp412))->tl){ Cyc_CfAbsexp_check_absexp(( void*)(( struct Cyc_List_List*)
_check_null( _temp412))->hd);} return; _LL405: Cyc_CfAbsexp_check_absop(
_temp414); return; _LL407: Cyc_CfAbsexp_check_absop( _temp418); Cyc_CfAbsexp_check_absop(
_temp416); Cyc_CfAbsexp_ok_lvalue( _temp418); return; _LL391:;} void* Cyc_CfAbsexp_eval_absop_r(
struct Cyc_Dict_Dict* pinfo_dict, void* ao); void* Cyc_CfAbsexp_eval_absop_l(
struct Cyc_Dict_Dict* pinfo_dict, void* ao){ void* _temp420= ao; struct Cyc_Absyn_Vardecl*
_temp434; struct Cyc_Absyn_Exp* _temp436; void* _temp438; void* _temp440; void*
_temp442; _LL422: if(( unsigned int) _temp420 > 1u?*(( int*) _temp420) == Cyc_CfAbsexp_LocalOp:
0){ _LL435: _temp434=(( struct Cyc_CfAbsexp_LocalOp_struct*) _temp420)->f1; goto
_LL423;} else{ goto _LL424;} _LL424: if(( unsigned int) _temp420 > 1u?*(( int*)
_temp420) == Cyc_CfAbsexp_MallocOp: 0){ _LL437: _temp436=(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp420)->f1; goto _LL425;} else{ goto _LL426;} _LL426: if(( unsigned int)
_temp420 > 1u?*(( int*) _temp420) == Cyc_CfAbsexp_MemberOp: 0){ _LL441: _temp440=(
void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp420)->f1; goto _LL439;
_LL439: _temp438=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp420)->f2;
goto _LL427;} else{ goto _LL428;} _LL428: if(( unsigned int) _temp420 > 1u?*((
int*) _temp420) == Cyc_CfAbsexp_DerefOp: 0){ _LL443: _temp442=( void*)(( struct
Cyc_CfAbsexp_DerefOp_struct*) _temp420)->f1; goto _LL429;} else{ goto _LL430;}
_LL430: if( _temp420 ==( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL431;} else{
goto _LL432;} _LL432: if(( unsigned int) _temp420 > 1u?*(( int*) _temp420) ==
Cyc_CfAbsexp_AddressOp: 0){ goto _LL433;} else{ goto _LL421;} _LL423: return(
void*)({ struct Cyc_CfFlowInfo_MustPointTo_struct* _temp444=( struct Cyc_CfFlowInfo_MustPointTo_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MustPointTo_struct)); _temp444[ 0]=({
struct Cyc_CfFlowInfo_MustPointTo_struct _temp445; _temp445.tag= Cyc_CfFlowInfo_MustPointTo;
_temp445.f1=({ struct Cyc_CfFlowInfo_Place* _temp446=( struct Cyc_CfFlowInfo_Place*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_Place)); _temp446->root=( void*)(( void*)({
struct Cyc_CfFlowInfo_VarRoot_struct* _temp447=( struct Cyc_CfFlowInfo_VarRoot_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp447[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp448; _temp448.tag= Cyc_CfFlowInfo_VarRoot;
_temp448.f1= _temp434; _temp448;}); _temp447;})); _temp446->fields= 0; _temp446;});
_temp445;}); _temp444;}); _LL425: return( void*)({ struct Cyc_CfFlowInfo_MustPointTo_struct*
_temp449=( struct Cyc_CfFlowInfo_MustPointTo_struct*) GC_malloc( sizeof( struct
Cyc_CfFlowInfo_MustPointTo_struct)); _temp449[ 0]=({ struct Cyc_CfFlowInfo_MustPointTo_struct
_temp450; _temp450.tag= Cyc_CfFlowInfo_MustPointTo; _temp450.f1=({ struct Cyc_CfFlowInfo_Place*
_temp451=( struct Cyc_CfFlowInfo_Place*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_Place));
_temp451->root=( void*)(( void*)({ struct Cyc_CfFlowInfo_MallocPt_struct*
_temp452=( struct Cyc_CfFlowInfo_MallocPt_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp452[ 0]=({ struct Cyc_CfFlowInfo_MallocPt_struct _temp453; _temp453.tag=
Cyc_CfFlowInfo_MallocPt; _temp453.f1= _temp436; _temp453;}); _temp452;}));
_temp451->fields= 0; _temp451;}); _temp450;}); _temp449;}); _LL427: { void*
_temp454= Cyc_CfAbsexp_eval_absop_l( pinfo_dict, _temp440); void* _temp455=
_temp454; struct Cyc_CfFlowInfo_Place* _temp461; struct Cyc_CfFlowInfo_Place
_temp463; struct Cyc_List_List* _temp464; void* _temp466; _LL457: if(*(( int*)
_temp455) == Cyc_CfFlowInfo_UnknownIS){ goto _LL458;} else{ goto _LL459;} _LL459:
if(*(( int*) _temp455) == Cyc_CfFlowInfo_MustPointTo){ _LL462: _temp461=((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp455)->f1; _temp463=* _temp461;
_LL467: _temp466=( void*) _temp463.root; goto _LL465; _LL465: _temp464= _temp463.fields;
goto _LL460;} else{ goto _LL456;} _LL458: return _temp454; _LL460: return( void*)({
struct Cyc_CfFlowInfo_MustPointTo_struct* _temp468=( struct Cyc_CfFlowInfo_MustPointTo_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MustPointTo_struct)); _temp468[ 0]=({
struct Cyc_CfFlowInfo_MustPointTo_struct _temp469; _temp469.tag= Cyc_CfFlowInfo_MustPointTo;
_temp469.f1=({ struct Cyc_CfFlowInfo_Place* _temp470=( struct Cyc_CfFlowInfo_Place*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_Place)); _temp470->root=( void*)
_temp466; _temp470->fields= Cyc_List_append( _temp464,({ struct Cyc_List_List*
_temp471=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp471->hd=( void*) _temp438; _temp471->tl= 0; _temp471;})); _temp470;});
_temp469;}); _temp468;}); _LL456:;} _LL429: { void* _temp472= Cyc_CfAbsexp_eval_absop_r(
pinfo_dict, _temp442); void* _temp473= _temp472; void* _temp481; _LL475: if(*((
int*) _temp473) == Cyc_CfFlowInfo_LeafPI){ _LL482: _temp481=( void*)(( struct
Cyc_CfFlowInfo_LeafPI_struct*) _temp473)->f1; goto _LL476;} else{ goto _LL477;}
_LL477: if(*(( int*) _temp473) == Cyc_CfFlowInfo_TuplePI){ goto _LL478;} else{
goto _LL479;} _LL479: if(*(( int*) _temp473) == Cyc_CfFlowInfo_StructPI){ goto
_LL480;} else{ goto _LL474;} _LL476: return _temp481; _LL478: goto _LL480;
_LL480:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL474:;} _LL431: goto
_LL433; _LL433:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL421:;} void*
Cyc_CfAbsexp_eval_absop_r( struct Cyc_Dict_Dict* pinfo_dict, void* ao){ void*
_temp483= ao; struct Cyc_Absyn_Vardecl* _temp497; struct Cyc_Absyn_Exp* _temp499;
void* _temp501; void* _temp503; void* _temp505; void* _temp507; _LL485: if((
unsigned int) _temp483 > 1u?*(( int*) _temp483) == Cyc_CfAbsexp_LocalOp: 0){
_LL498: _temp497=(( struct Cyc_CfAbsexp_LocalOp_struct*) _temp483)->f1; goto
_LL486;} else{ goto _LL487;} _LL487: if(( unsigned int) _temp483 > 1u?*(( int*)
_temp483) == Cyc_CfAbsexp_MallocOp: 0){ _LL500: _temp499=(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp483)->f1; goto _LL488;} else{ goto _LL489;} _LL489: if(( unsigned int)
_temp483 > 1u?*(( int*) _temp483) == Cyc_CfAbsexp_MemberOp: 0){ _LL504: _temp503=(
void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp483)->f1; goto _LL502;
_LL502: _temp501=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp483)->f2;
goto _LL490;} else{ goto _LL491;} _LL491: if(( unsigned int) _temp483 > 1u?*((
int*) _temp483) == Cyc_CfAbsexp_DerefOp: 0){ _LL506: _temp505=( void*)(( struct
Cyc_CfAbsexp_DerefOp_struct*) _temp483)->f1; goto _LL492;} else{ goto _LL493;}
_LL493: if( _temp483 ==( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL494;} else{
goto _LL495;} _LL495: if(( unsigned int) _temp483 > 1u?*(( int*) _temp483) ==
Cyc_CfAbsexp_AddressOp: 0){ _LL508: _temp507=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*)
_temp483)->f1; goto _LL496;} else{ goto _LL484;} _LL486: return Cyc_Dict_lookup(
pinfo_dict,( void*)({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp509=( struct
Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp509[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp510; _temp510.tag= Cyc_CfFlowInfo_VarRoot;
_temp510.f1= _temp497; _temp510;}); _temp509;})); _LL488: return Cyc_Dict_lookup(
pinfo_dict,( void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp511=( struct
Cyc_CfFlowInfo_MallocPt_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp511[ 0]=({ struct Cyc_CfFlowInfo_MallocPt_struct _temp512; _temp512.tag=
Cyc_CfFlowInfo_MallocPt; _temp512.f1= _temp499; _temp512;}); _temp511;}));
_LL490: { void* _temp513= Cyc_CfAbsexp_eval_absop_r( pinfo_dict, _temp503);
struct _tuple3 _temp515=({ struct _tuple3 _temp514; _temp514.f1= _temp513;
_temp514.f2= _temp501; _temp514;}); void* _temp525; void* _temp527; void*
_temp529; int _temp531; void* _temp533; struct Cyc_Dict_Dict* _temp535; void*
_temp537; struct _tagged_arr* _temp539; void* _temp541; struct Cyc_Dict_Dict*
_temp543; _LL517: _LL526: _temp525= _temp515.f1; if(*(( int*) _temp525) == Cyc_CfFlowInfo_LeafPI){
_LL528: _temp527=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp525)->f1;
if(*(( int*) _temp527) == Cyc_CfFlowInfo_UnknownIS){ goto _LL518;} else{ goto
_LL519;}} else{ goto _LL519;} _LL519: _LL534: _temp533= _temp515.f1; if(*(( int*)
_temp533) == Cyc_CfFlowInfo_TuplePI){ _LL536: _temp535=(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp533)->f1; goto _LL530;} else{ goto _LL521;} _LL530: _temp529= _temp515.f2;
if(*(( int*) _temp529) == Cyc_CfFlowInfo_TupleF){ _LL532: _temp531=(( struct Cyc_CfFlowInfo_TupleF_struct*)
_temp529)->f1; goto _LL520;} else{ goto _LL521;} _LL521: _LL542: _temp541=
_temp515.f1; if(*(( int*) _temp541) == Cyc_CfFlowInfo_StructPI){ _LL544:
_temp543=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp541)->f1; goto _LL538;}
else{ goto _LL523;} _LL538: _temp537= _temp515.f2; if(*(( int*) _temp537) == Cyc_CfFlowInfo_StructF){
_LL540: _temp539=(( struct Cyc_CfFlowInfo_StructF_struct*) _temp537)->f1; goto
_LL522;} else{ goto _LL523;} _LL523: goto _LL524; _LL518: return _temp513;
_LL520: return(( void*(*)( struct Cyc_Dict_Dict* d, int key)) Cyc_Dict_lookup)(
_temp535, _temp531); _LL522: return(( void*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup)( _temp543, _temp539); _LL524:( int) _throw((
void*) Cyc_CfAbsexp_BadAbsexp); _LL516:;} _LL492: { void* _temp545= Cyc_CfAbsexp_eval_absop_r(
pinfo_dict, _temp505); void* _temp546= _temp545; void* _temp556; void* _temp558;
void* _temp560; void* _temp562; void* _temp564; struct Cyc_CfFlowInfo_Place*
_temp566; _LL548: if(*(( int*) _temp546) == Cyc_CfFlowInfo_LeafPI){ _LL557:
_temp556=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp546)->f1; if(*((
int*) _temp556) == Cyc_CfFlowInfo_UnknownIS){ _LL561: _temp560=( void*)(( struct
Cyc_CfFlowInfo_UnknownIS_struct*) _temp556)->f1; goto _LL559; _LL559: _temp558=(
void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp556)->f2; if( _temp558 ==(
void*) Cyc_CfFlowInfo_ThisIL){ goto _LL549;} else{ goto _LL550;}} else{ goto
_LL550;}} else{ goto _LL550;} _LL550: if(*(( int*) _temp546) == Cyc_CfFlowInfo_LeafPI){
_LL563: _temp562=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp546)->f1;
if(*(( int*) _temp562) == Cyc_CfFlowInfo_UnknownIS){ goto _LL551;} else{ goto
_LL552;}} else{ goto _LL552;} _LL552: if(*(( int*) _temp546) == Cyc_CfFlowInfo_LeafPI){
_LL565: _temp564=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp546)->f1;
if(*(( int*) _temp564) == Cyc_CfFlowInfo_MustPointTo){ _LL567: _temp566=((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp564)->f1; goto _LL553;} else{
goto _LL554;}} else{ goto _LL554;} _LL554: goto _LL555; _LL549: return Cyc_CfFlowInfo_mkLeafPI((
void*) Cyc_CfFlowInfo_Esc,( void*) Cyc_CfFlowInfo_NoneIL); _LL551: return
_temp545; _LL553: return Cyc_CfFlowInfo_lookup_place( pinfo_dict, _temp566);
_LL555:( int) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL547:;} _LL494: return
Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,( void*) Cyc_CfFlowInfo_AllIL);
_LL496: return( void*)({ struct Cyc_CfFlowInfo_LeafPI_struct* _temp568=( struct
Cyc_CfFlowInfo_LeafPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_LeafPI_struct));
_temp568[ 0]=({ struct Cyc_CfFlowInfo_LeafPI_struct _temp569; _temp569.tag= Cyc_CfFlowInfo_LeafPI;
_temp569.f1=( void*) Cyc_CfAbsexp_eval_absop_l( pinfo_dict, _temp507); _temp569;});
_temp568;}); _LL484:;} void* Cyc_CfAbsexp_assign_escape( struct Cyc_List_List**
escaping_states, void* old_pinfo, void* new_pinfo); struct Cyc_Dict_Dict* Cyc_CfAbsexp_assign_escape_f(
struct Cyc_List_List** escaping_states, void* key, void* b1, void* b2, struct
Cyc_Dict_Dict* accum){ return Cyc_Dict_insert( accum, key, Cyc_CfAbsexp_assign_escape(
escaping_states, b1, b2));} void* Cyc_CfAbsexp_assign_escape( struct Cyc_List_List**
escaping_states, void* old_pinfo, void* new_pinfo){ struct _tuple3 _temp571=({
struct _tuple3 _temp570; _temp570.f1= old_pinfo; _temp570.f2= new_pinfo;
_temp570;}); void* _temp589; void* _temp591; void* _temp593; void* _temp595;
void* _temp597; void* _temp599; void* _temp601; void* _temp603; void* _temp605;
void* _temp607; void* _temp609; void* _temp611; void* _temp613; void* _temp615;
void* _temp617; void* _temp619; void* _temp621; void* _temp623; void* _temp625;
void* _temp627; void* _temp629; void* _temp631; void* _temp633; struct Cyc_Dict_Dict*
_temp635; void* _temp637; struct Cyc_Dict_Dict* _temp639; void* _temp641; struct
Cyc_Dict_Dict* _temp643; void* _temp645; struct Cyc_Dict_Dict* _temp647; _LL573:
_LL596: _temp595= _temp571.f1; if(*(( int*) _temp595) == Cyc_CfFlowInfo_LeafPI){
_LL598: _temp597=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp595)->f1;
if(*(( int*) _temp597) == Cyc_CfFlowInfo_MustPointTo){ goto _LL590;} else{ goto
_LL575;}} else{ goto _LL575;} _LL590: _temp589= _temp571.f2; if(*(( int*)
_temp589) == Cyc_CfFlowInfo_LeafPI){ _LL592: _temp591=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp589)->f1; if(*(( int*) _temp591) == Cyc_CfFlowInfo_UnknownIS){ _LL594:
_temp593=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp591)->f2; goto
_LL574;} else{ goto _LL575;}} else{ goto _LL575;} _LL575: _LL606: _temp605=
_temp571.f1; if(*(( int*) _temp605) == Cyc_CfFlowInfo_LeafPI){ _LL608: _temp607=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp605)->f1; if(*(( int*)
_temp607) == Cyc_CfFlowInfo_UnknownIS){ _LL610: _temp609=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp607)->f1; if( _temp609 ==( void*) Cyc_CfFlowInfo_Unesc){ goto _LL600;}
else{ goto _LL577;}} else{ goto _LL577;}} else{ goto _LL577;} _LL600: _temp599=
_temp571.f2; if(*(( int*) _temp599) == Cyc_CfFlowInfo_LeafPI){ _LL602: _temp601=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp599)->f1; if(*(( int*)
_temp601) == Cyc_CfFlowInfo_UnknownIS){ _LL604: _temp603=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp601)->f2; goto _LL576;} else{ goto _LL577;}} else{ goto _LL577;} _LL577:
_LL614: _temp613= _temp571.f1; if(*(( int*) _temp613) == Cyc_CfFlowInfo_LeafPI){
_LL616: _temp615=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp613)->f1;
if(*(( int*) _temp615) == Cyc_CfFlowInfo_MustPointTo){ goto _LL612;} else{ goto
_LL579;}} else{ goto _LL579;} _LL612: _temp611= _temp571.f2; if(*(( int*)
_temp611) == Cyc_CfFlowInfo_LeafPI){ goto _LL578;} else{ goto _LL579;} _LL579:
_LL620: _temp619= _temp571.f1; if(*(( int*) _temp619) == Cyc_CfFlowInfo_LeafPI){
_LL622: _temp621=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp619)->f1;
if(*(( int*) _temp621) == Cyc_CfFlowInfo_UnknownIS){ _LL624: _temp623=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp621)->f1; if( _temp623 ==( void*)
Cyc_CfFlowInfo_Unesc){ goto _LL618;} else{ goto _LL581;}} else{ goto _LL581;}}
else{ goto _LL581;} _LL618: _temp617= _temp571.f2; if(*(( int*) _temp617) == Cyc_CfFlowInfo_LeafPI){
goto _LL580;} else{ goto _LL581;} _LL581: _LL628: _temp627= _temp571.f1; if(*((
int*) _temp627) == Cyc_CfFlowInfo_LeafPI){ _LL630: _temp629=( void*)(( struct
Cyc_CfFlowInfo_LeafPI_struct*) _temp627)->f1; if(*(( int*) _temp629) == Cyc_CfFlowInfo_UnknownIS){
_LL632: _temp631=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp629)->f1;
if( _temp631 ==( void*) Cyc_CfFlowInfo_Esc){ goto _LL626;} else{ goto _LL583;}}
else{ goto _LL583;}} else{ goto _LL583;} _LL626: _temp625= _temp571.f2; if(*((
int*) _temp625) == Cyc_CfFlowInfo_LeafPI){ goto _LL582;} else{ goto _LL583;}
_LL583: _LL638: _temp637= _temp571.f1; if(*(( int*) _temp637) == Cyc_CfFlowInfo_TuplePI){
_LL640: _temp639=(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp637)->f1; goto
_LL634;} else{ goto _LL585;} _LL634: _temp633= _temp571.f2; if(*(( int*)
_temp633) == Cyc_CfFlowInfo_TuplePI){ _LL636: _temp635=(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp633)->f1; goto _LL584;} else{ goto _LL585;} _LL585: _LL646: _temp645=
_temp571.f1; if(*(( int*) _temp645) == Cyc_CfFlowInfo_StructPI){ _LL648:
_temp647=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp645)->f1; goto _LL642;}
else{ goto _LL587;} _LL642: _temp641= _temp571.f2; if(*(( int*) _temp641) == Cyc_CfFlowInfo_StructPI){
_LL644: _temp643=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp641)->f1; goto
_LL586;} else{ goto _LL587;} _LL587: goto _LL588; _LL574: _temp603= _temp593;
goto _LL576; _LL576: return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Unesc,
_temp603); _LL578: goto _LL580; _LL580: return new_pinfo; _LL582:*
escaping_states=({ struct Cyc_List_List* _temp649=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp649->hd=( void*) new_pinfo;
_temp649->tl=* escaping_states; _temp649;}); return Cyc_CfFlowInfo_mkLeafPI((
void*) Cyc_CfFlowInfo_Esc,( void*) Cyc_CfFlowInfo_AllIL); _LL584: return( void*)({
struct Cyc_CfFlowInfo_TuplePI_struct* _temp650=( struct Cyc_CfFlowInfo_TuplePI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct)); _temp650[ 0]=({
struct Cyc_CfFlowInfo_TuplePI_struct _temp651; _temp651.tag= Cyc_CfFlowInfo_TuplePI;
_temp651.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*(* f)( struct Cyc_List_List**,
int, void*, void*, struct Cyc_Dict_Dict*), struct Cyc_List_List** inner_env,
struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2, struct Cyc_Dict_Dict* accum))
Cyc_Dict_fold2_c)(( struct Cyc_Dict_Dict*(*)( struct Cyc_List_List**
escaping_states, int key, void* b1, void* b2, struct Cyc_Dict_Dict* accum)) Cyc_CfAbsexp_assign_escape_f,
escaping_states, _temp639, _temp635,(( struct Cyc_Dict_Dict*(*)( int(* comp)(
int, int))) Cyc_Dict_empty)( Cyc_Core_intcmp)); _temp651;}); _temp650;}); _LL586:
return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct* _temp652=( struct Cyc_CfFlowInfo_StructPI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct)); _temp652[ 0]=({
struct Cyc_CfFlowInfo_StructPI_struct _temp653; _temp653.tag= Cyc_CfFlowInfo_StructPI;
_temp653.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*(* f)( struct Cyc_List_List**,
struct _tagged_arr*, void*, void*, struct Cyc_Dict_Dict*), struct Cyc_List_List**
inner_env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2, struct Cyc_Dict_Dict*
accum)) Cyc_Dict_fold2_c)(( struct Cyc_Dict_Dict*(*)( struct Cyc_List_List**
escaping_states, struct _tagged_arr* key, void* b1, void* b2, struct Cyc_Dict_Dict*
accum)) Cyc_CfAbsexp_assign_escape_f, escaping_states, _temp647, _temp643,((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_String_zstrptrcmp)); _temp653;}); _temp652;}); _LL588:( int)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp654=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp654[ 0]=({ struct
Cyc_Core_Impossible_struct _temp655; _temp655.tag= Cyc_Core_Impossible; _temp655.f1=
_tag_arr("bad pinfos in assign_escape", sizeof( unsigned char), 28u); _temp655;});
_temp654;})); _LL572:;} int Cyc_CfAbsexp_is_ok_malloc_assign( void* lval, struct
Cyc_List_List* ok_mallocs){ void* _temp656= lval; struct Cyc_Absyn_Exp* _temp664;
void* _temp666; _LL658: if(( unsigned int) _temp656 > 1u?*(( int*) _temp656) ==
Cyc_CfAbsexp_MallocOp: 0){ _LL665: _temp664=(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp656)->f1; goto _LL659;} else{ goto _LL660;} _LL660: if(( unsigned int)
_temp656 > 1u?*(( int*) _temp656) == Cyc_CfAbsexp_MemberOp: 0){ _LL667: _temp666=(
void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp656)->f1; goto _LL661;}
else{ goto _LL662;} _LL662: goto _LL663; _LL659: return(( int(*)( struct Cyc_List_List*
l, struct Cyc_Absyn_Exp* x)) Cyc_List_memq)( ok_mallocs, _temp664); _LL661:
return Cyc_CfAbsexp_is_ok_malloc_assign( _temp666, ok_mallocs); _LL663: return 0;
_LL657:;} struct _tuple5{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; }
; static struct _tuple5 Cyc_CfAbsexp_unordered_mallocs( struct _RegionHandle*
rgn, void* ae, struct Cyc_List_List* ok_mallocs){ struct Cyc_List_List* _temp668=
0; struct Cyc_List_List* _temp669= 0;{ void* _temp670= ae; void* _temp692; void*
_temp694; void* _temp696; struct Cyc_Absyn_Exp* _temp698; struct Cyc_Absyn_Stmt*
_temp700; struct Cyc_List_List* _temp702; void* _temp704; struct Cyc_List_List*
_temp706; void* _temp708; struct Cyc_List_List* _temp710; void* _temp712; _LL672:
if( _temp670 ==( void*) Cyc_CfAbsexp_BottomAE){ goto _LL673;} else{ goto _LL674;}
_LL674: if( _temp670 ==( void*) Cyc_CfAbsexp_SkipAE){ goto _LL675;} else{ goto
_LL676;} _LL676: if(( unsigned int) _temp670 > 2u?*(( int*) _temp670) == Cyc_CfAbsexp_UseAE:
0){ goto _LL677;} else{ goto _LL678;} _LL678: if(( unsigned int) _temp670 > 2u?*((
int*) _temp670) == Cyc_CfAbsexp_AssignAE: 0){ _LL695: _temp694=( void*)(( struct
Cyc_CfAbsexp_AssignAE_struct*) _temp670)->f1; goto _LL693; _LL693: _temp692=(
void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp670)->f2; goto _LL679;}
else{ goto _LL680;} _LL680: if(( unsigned int) _temp670 > 2u?*(( int*) _temp670)
== Cyc_CfAbsexp_MallocAE: 0){ _LL697: _temp696=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp670)->f1; if(*(( int*) _temp696) == Cyc_CfFlowInfo_MallocPt){ _LL699:
_temp698=(( struct Cyc_CfFlowInfo_MallocPt_struct*) _temp696)->f1; goto _LL681;}
else{ goto _LL682;}} else{ goto _LL682;} _LL682: if(( unsigned int) _temp670 > 2u?*((
int*) _temp670) == Cyc_CfAbsexp_MallocAE: 0){ goto _LL683;} else{ goto _LL684;}
_LL684: if(( unsigned int) _temp670 > 2u?*(( int*) _temp670) == Cyc_CfAbsexp_StmtAE:
0){ _LL701: _temp700=(( struct Cyc_CfAbsexp_StmtAE_struct*) _temp670)->f1; goto
_LL685;} else{ goto _LL686;} _LL686: if(( unsigned int) _temp670 > 2u?*(( int*)
_temp670) == Cyc_CfAbsexp_GroupAE: 0){ _LL705: _temp704=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp670)->f1; if( _temp704 ==( void*) Cyc_CfAbsexp_OrderedG){ goto _LL703;}
else{ goto _LL688;} _LL703: _temp702=(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp670)->f2; goto _LL687;} else{ goto _LL688;} _LL688: if(( unsigned int)
_temp670 > 2u?*(( int*) _temp670) == Cyc_CfAbsexp_GroupAE: 0){ _LL709: _temp708=(
void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp670)->f1; if( _temp708 ==(
void*) Cyc_CfAbsexp_OneofG){ goto _LL707;} else{ goto _LL690;} _LL707: _temp706=((
struct Cyc_CfAbsexp_GroupAE_struct*) _temp670)->f2; goto _LL689;} else{ goto
_LL690;} _LL690: if(( unsigned int) _temp670 > 2u?*(( int*) _temp670) == Cyc_CfAbsexp_GroupAE:
0){ _LL713: _temp712=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp670)->f1;
if( _temp712 ==( void*) Cyc_CfAbsexp_UnorderedG){ goto _LL711;} else{ goto
_LL671;} _LL711: _temp710=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp670)->f2;
goto _LL691;} else{ goto _LL671;} _LL673: goto _LL671; _LL675: goto _LL671;
_LL677: goto _LL671; _LL679: if( Cyc_CfAbsexp_is_ok_malloc_assign( _temp694,
ok_mallocs)){ _temp668=({ struct Cyc_List_List* _temp714=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp714->hd=( void*) ae;
_temp714->tl= _temp668; _temp714;});} goto _LL671; _LL681: _temp669=({ struct
Cyc_List_List* _temp715=( struct Cyc_List_List*) _region_malloc( rgn, sizeof(
struct Cyc_List_List)); _temp715->hd=( void*) _temp698; _temp715->tl= _temp669;
_temp715;}); goto _LL671; _LL683:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp716=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp716[ 0]=({ struct Cyc_Core_Impossible_struct _temp717; _temp717.tag= Cyc_Core_Impossible;
_temp717.f1= _tag_arr("local variable in MallocAE", sizeof( unsigned char), 27u);
_temp717;}); _temp716;})); _LL685: goto _LL671; _LL687: for( 0; _temp702 != 0;
_temp702=(( struct Cyc_List_List*) _check_null( _temp702))->tl){ struct Cyc_List_List*
_temp720; struct Cyc_List_List* _temp722; struct _tuple5 _temp718= Cyc_CfAbsexp_unordered_mallocs(
rgn,( void*)(( struct Cyc_List_List*) _check_null( _temp702))->hd,(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rappend)(
rgn, _temp669, ok_mallocs)); _LL723: _temp722= _temp718.f1; goto _LL721; _LL721:
_temp720= _temp718.f2; goto _LL719; _LL719: _temp668= Cyc_List_rappend( rgn,
_temp722, _temp668); _temp669=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rappend)( rgn,
_temp720, _temp669);} goto _LL671; _LL689: goto _LL671; _LL691: for( 0; _temp710
!= 0; _temp710=(( struct Cyc_List_List*) _check_null( _temp710))->tl){ struct
Cyc_List_List* _temp726; struct Cyc_List_List* _temp728; struct _tuple5 _temp724=
Cyc_CfAbsexp_unordered_mallocs( rgn,( void*)(( struct Cyc_List_List*)
_check_null( _temp710))->hd, ok_mallocs); _LL729: _temp728= _temp724.f1; goto
_LL727; _LL727: _temp726= _temp724.f2; goto _LL725; _LL725: _temp668= Cyc_List_rappend(
rgn, _temp728, _temp668); _temp669=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rappend)(
rgn, _temp726, _temp669);} goto _LL671; _LL671:;} return({ struct _tuple5
_temp730; _temp730.f1= _temp668; _temp730.f2= _temp669; _temp730;});} void* Cyc_CfAbsexp_eval_absexp(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Position_Segment* loc, void*
ae, void* in_flow){ struct Cyc_Dict_Dict* pinfo_dict;{ void* _temp731= in_flow;
struct Cyc_Dict_Dict* _temp737; _LL733: if( _temp731 ==( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL734;} else{ goto _LL735;} _LL735: if(( unsigned int) _temp731 > 1u?*((
int*) _temp731) == Cyc_CfFlowInfo_InitsFL: 0){ _LL738: _temp737=(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp731)->f1; goto _LL736;} else{ goto _LL732;} _LL734: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL736: pinfo_dict= _temp737; goto _LL732; _LL732:;}{ void* _temp739= ae; void*
_temp759; void* _temp761; void* _temp763; void* _temp765; struct Cyc_Absyn_Stmt*
_temp767; struct Cyc_List_List* _temp769; void* _temp771; struct Cyc_List_List*
_temp773; void* _temp775; struct Cyc_List_List* _temp777; void* _temp779; _LL741:
if(( unsigned int) _temp739 > 2u?*(( int*) _temp739) == Cyc_CfAbsexp_AssignAE: 0){
_LL762: _temp761=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp739)->f1;
goto _LL760; _LL760: _temp759=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp739)->f2; goto _LL742;} else{ goto _LL743;} _LL743: if(( unsigned int)
_temp739 > 2u?*(( int*) _temp739) == Cyc_CfAbsexp_UseAE: 0){ _LL764: _temp763=(
void*)(( struct Cyc_CfAbsexp_UseAE_struct*) _temp739)->f1; goto _LL744;} else{
goto _LL745;} _LL745: if(( unsigned int) _temp739 > 2u?*(( int*) _temp739) ==
Cyc_CfAbsexp_MallocAE: 0){ _LL766: _temp765=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp739)->f1; goto _LL746;} else{ goto _LL747;} _LL747: if( _temp739 ==( void*)
Cyc_CfAbsexp_BottomAE){ goto _LL748;} else{ goto _LL749;} _LL749: if( _temp739
==( void*) Cyc_CfAbsexp_SkipAE){ goto _LL750;} else{ goto _LL751;} _LL751: if((
unsigned int) _temp739 > 2u?*(( int*) _temp739) == Cyc_CfAbsexp_StmtAE: 0){
_LL768: _temp767=(( struct Cyc_CfAbsexp_StmtAE_struct*) _temp739)->f1; goto
_LL752;} else{ goto _LL753;} _LL753: if(( unsigned int) _temp739 > 2u?*(( int*)
_temp739) == Cyc_CfAbsexp_GroupAE: 0){ _LL772: _temp771=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp739)->f1; if( _temp771 ==( void*) Cyc_CfAbsexp_OrderedG){ goto _LL770;}
else{ goto _LL755;} _LL770: _temp769=(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp739)->f2; goto _LL754;} else{ goto _LL755;} _LL755: if(( unsigned int)
_temp739 > 2u?*(( int*) _temp739) == Cyc_CfAbsexp_GroupAE: 0){ _LL776: _temp775=(
void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp739)->f1; if( _temp775 ==(
void*) Cyc_CfAbsexp_OneofG){ goto _LL774;} else{ goto _LL757;} _LL774: _temp773=((
struct Cyc_CfAbsexp_GroupAE_struct*) _temp739)->f2; goto _LL756;} else{ goto
_LL757;} _LL757: if(( unsigned int) _temp739 > 2u?*(( int*) _temp739) == Cyc_CfAbsexp_GroupAE:
0){ _LL780: _temp779=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp739)->f1;
if( _temp779 ==( void*) Cyc_CfAbsexp_UnorderedG){ goto _LL778;} else{ goto
_LL740;} _LL778: _temp777=(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp739)->f2;
goto _LL758;} else{ goto _LL740;} _LL742:{ void* _temp781= Cyc_CfAbsexp_eval_absop_l(
pinfo_dict, _temp761); struct Cyc_CfFlowInfo_Place* _temp787; _LL783: if(*(( int*)
_temp781) == Cyc_CfFlowInfo_UnknownIS){ goto _LL784;} else{ goto _LL785;} _LL785:
if(*(( int*) _temp781) == Cyc_CfFlowInfo_MustPointTo){ _LL788: _temp787=((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp781)->f1; goto _LL786;} else{
goto _LL782;} _LL784: goto _LL782; _LL786: { void* _temp789= Cyc_CfFlowInfo_lookup_place(
pinfo_dict, _temp787); void* _temp790= Cyc_CfAbsexp_eval_absop_r( pinfo_dict,
_temp759);{ struct _tuple3 _temp792=({ struct _tuple3 _temp791; _temp791.f1=
_temp789; _temp791.f2= _temp790; _temp791;}); void* _temp800; void* _temp802;
void* _temp804; void* _temp806; void* _temp808; void* _temp810; void* _temp812;
void* _temp814; _LL794: _LL807: _temp806= _temp792.f1; if(*(( int*) _temp806) ==
Cyc_CfFlowInfo_TuplePI){ goto _LL801;} else{ goto _LL796;} _LL801: _temp800=
_temp792.f2; if(*(( int*) _temp800) == Cyc_CfFlowInfo_LeafPI){ _LL803: _temp802=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp800)->f1; if(*(( int*)
_temp802) == Cyc_CfFlowInfo_UnknownIS){ _LL805: _temp804=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp802)->f2; goto _LL795;} else{ goto _LL796;}} else{ goto _LL796;} _LL796:
_LL815: _temp814= _temp792.f1; if(*(( int*) _temp814) == Cyc_CfFlowInfo_StructPI){
goto _LL809;} else{ goto _LL798;} _LL809: _temp808= _temp792.f2; if(*(( int*)
_temp808) == Cyc_CfFlowInfo_LeafPI){ _LL811: _temp810=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp808)->f1; if(*(( int*) _temp810) == Cyc_CfFlowInfo_UnknownIS){ _LL813:
_temp812=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp810)->f2; goto
_LL797;} else{ goto _LL798;}} else{ goto _LL798;} _LL798: goto _LL799; _LL795:
_temp812= _temp804; goto _LL797; _LL797: _temp790= Cyc_CfFlowInfo_assign_unknown_dict(
_temp812, _temp789); goto _LL793; _LL799: goto _LL793; _LL793:;}{ struct Cyc_List_List*
_temp816= 0; void* _temp817= Cyc_CfAbsexp_assign_escape(& _temp816, _temp789,
_temp790); for( 0; _temp816 != 0; _temp816=(( struct Cyc_List_List*) _check_null(
_temp816))->tl){ void* _temp818=( void*)(( struct Cyc_List_List*) _check_null(
_temp816))->hd; if( ! Cyc_CfFlowInfo_isAllInit( pinfo_dict, _temp818)){({ void(*
_temp819)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp822= loc;
struct _tagged_arr _temp823= _tag_arr("uninitialized value assigned to escaped place",
sizeof( unsigned char), 46u); void* _temp820[ 0u]={}; struct _tagged_arr
_temp821={( void*) _temp820,( void*) _temp820,( void*)( _temp820 + 0u)};
_temp819( _temp822, _temp823, _temp821);}); return( void*) Cyc_CfFlowInfo_BottomFL;}
pinfo_dict= Cyc_CfFlowInfo_escape_pointsto( _temp818, pinfo_dict);} pinfo_dict=
Cyc_CfFlowInfo_insert_place( pinfo_dict, _temp787, _temp817);{ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp824=({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp825=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp825[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp826; _temp826.tag= Cyc_CfFlowInfo_InitsFL;
_temp826.f1= pinfo_dict; _temp826;}); _temp825;}); Cyc_NewControlFlow_update_tryflow(
env,( void*) _temp824); return( void*) _temp824;}}} _LL782:;} _temp763= _temp759;
goto _LL744; _LL744: { void* _temp827= Cyc_CfAbsexp_eval_absop_r( pinfo_dict,
_temp763); if( ! Cyc_CfFlowInfo_isAllInit( pinfo_dict, _temp827)){ void*
_temp828= _temp763; struct Cyc_Absyn_Vardecl* _temp834; _LL830: if((
unsigned int) _temp828 > 1u?*(( int*) _temp828) == Cyc_CfAbsexp_LocalOp: 0){
_LL835: _temp834=(( struct Cyc_CfAbsexp_LocalOp_struct*) _temp828)->f1; goto
_LL831;} else{ goto _LL832;} _LL832: goto _LL833; _LL831: if( Cyc_Tcutil_bits_only((
void*) _temp834->type)){({ void(* _temp836)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_warn; struct Cyc_Position_Segment*
_temp840= loc; struct _tagged_arr _temp841= _tag_arr("pointer-free %s may not be fully initialized",
sizeof( unsigned char), 45u); struct Cyc_Stdio_String_pa_struct _temp842;
_temp842.tag= Cyc_Stdio_String_pa; _temp842.f1=( struct _tagged_arr)*(* _temp834->name).f2;{
void* _temp839=( void*)& _temp842; void* _temp837[ 1u]={ _temp839}; struct
_tagged_arr _temp838={( void*) _temp837,( void*) _temp837,( void*)( _temp837 + 1u)};
_temp836( _temp840, _temp841, _temp838);}}); goto _LL829;} goto _LL833; _LL833:({
void(* _temp843)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp847= loc;
struct _tagged_arr _temp848= _tag_arr("%s must be fully initialized to be used here",
sizeof( unsigned char), 45u); struct Cyc_Stdio_String_pa_struct _temp849;
_temp849.tag= Cyc_Stdio_String_pa; _temp849.f1=( struct _tagged_arr) Cyc_CfAbsexp_absop2string(
_temp763);{ void* _temp846=( void*)& _temp849; void* _temp844[ 1u]={ _temp846};
struct _tagged_arr _temp845={( void*) _temp844,( void*) _temp844,( void*)(
_temp844 + 1u)}; _temp843( _temp847, _temp848, _temp845);}}); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL829:;}{ struct Cyc_Dict_Dict* _temp850= Cyc_CfFlowInfo_escape_pointsto(
_temp827, pinfo_dict); if( _temp850 == pinfo_dict){ return in_flow;}{ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp851=({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp852=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp852[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp853; _temp853.tag= Cyc_CfFlowInfo_InitsFL;
_temp853.f1= pinfo_dict; _temp853;}); _temp852;}); Cyc_NewControlFlow_update_tryflow(
env,( void*) _temp851); return( void*) _temp851;}}} _LL746: return( void*)({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp854=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp854[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp855; _temp855.tag= Cyc_CfFlowInfo_InitsFL;
_temp855.f1= Cyc_Dict_insert( pinfo_dict, _temp765, Cyc_Dict_lookup( env->roots,
_temp765)); _temp855;}); _temp854;}); _LL748: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL750: return in_flow; _LL752: return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp767, in_flow); _LL754: for( 0; _temp769 != 0; _temp769=(( struct Cyc_List_List*)
_check_null( _temp769))->tl){ in_flow= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)((
struct Cyc_List_List*) _check_null( _temp769))->hd, in_flow);} return in_flow;
_LL756: { void* out_flow=( void*) Cyc_CfFlowInfo_BottomFL; for( 0; _temp773 != 0;
_temp773=(( struct Cyc_List_List*) _check_null( _temp773))->tl){ out_flow= Cyc_CfFlowInfo_join_flow(
out_flow, Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct Cyc_List_List*)
_check_null( _temp773))->hd, in_flow));} return out_flow;} _LL758: { void*
_temp856= in_flow; do { in_flow= _temp856;{ struct Cyc_List_List* ael2= _temp777;
for( 0; ael2 != 0; ael2=(( struct Cyc_List_List*) _check_null( ael2))->tl){
_temp856= Cyc_CfFlowInfo_join_flow( _temp856, Cyc_CfAbsexp_eval_absexp( env, loc,(
void*)(( struct Cyc_List_List*) _check_null( ael2))->hd, _temp856));}}} while (
! Cyc_CfFlowInfo_flow_lessthan_approx( _temp856, in_flow));{ struct Cyc_Dict_Dict*
pinfo_dict;{ void* _temp857= in_flow; struct Cyc_Dict_Dict* _temp863; _LL859:
if( _temp857 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL860;} else{ goto
_LL861;} _LL861: if(( unsigned int) _temp857 > 1u?*(( int*) _temp857) == Cyc_CfFlowInfo_InitsFL:
0){ _LL864: _temp863=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp857)->f1;
goto _LL862;} else{ goto _LL858;} _LL860: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL862: pinfo_dict= _temp863; goto _LL858; _LL858:;}{ struct _RegionHandle
_temp865= _new_region(); struct _RegionHandle* spine_rgn=& _temp865;
_push_region( spine_rgn);{ struct Cyc_List_List* _temp868; struct Cyc_List_List*
_temp870; struct _tuple5 _temp866= Cyc_CfAbsexp_unordered_mallocs( spine_rgn, ae,
0); _LL871: _temp870= _temp866.f1; goto _LL869; _LL869: _temp868= _temp866.f2;
goto _LL867; _LL867: for( 0; _temp868 != 0; _temp868=(( struct Cyc_List_List*)
_check_null( _temp868))->tl){ struct Cyc_CfFlowInfo_MallocPt_struct* _temp872=({
struct Cyc_CfFlowInfo_MallocPt_struct* _temp873=( struct Cyc_CfFlowInfo_MallocPt_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp873[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp874; _temp874.tag= Cyc_CfFlowInfo_MallocPt;
_temp874.f1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp868))->hd; _temp874;}); _temp873;}); pinfo_dict= Cyc_Dict_insert(
pinfo_dict,( void*) _temp872, Cyc_Dict_lookup( env->roots,( void*) _temp872));}
_temp856=( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp875=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp875[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp876; _temp876.tag= Cyc_CfFlowInfo_InitsFL;
_temp876.f1= pinfo_dict; _temp876;}); _temp875;}); for( 0; _temp870 != 0;
_temp870=(( struct Cyc_List_List*) _check_null( _temp870))->tl){ _temp856= Cyc_CfAbsexp_eval_absexp(
env, loc,( void*)(( struct Cyc_List_List*) _check_null( _temp870))->hd, _temp856);}};
_pop_region( spine_rgn);} return _temp856;}} _LL740:;}}
