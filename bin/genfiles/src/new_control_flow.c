 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_dirent{ int d_ino; int d_off; unsigned short
d_reclen; unsigned char d_type; unsigned char d_name[ 256u]; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; struct
_tuple0{ void* f1; void* f2; } ; extern void* Cyc_Core_snd( struct _tuple0*);
extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
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
Cyc_List_List* tl; } ; extern struct Cyc_List_List* Cyc_List_list( struct
_tagged_arr); extern unsigned char Cyc_List_List_empty[ 15u]; extern struct Cyc_List_List*
Cyc_List_tl( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map(
void*(* f)( void*), struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x);
extern unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Dict_Dict; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* comp)( void*, void*)); extern
struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key, void*
data); extern struct Cyc_Dict_Dict* Cyc_Dict_insert_new( struct Cyc_Dict_Dict* d,
void* key, void* data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d,
void* key); struct Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[
19u]; extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_String_pa=
0; struct Cyc_Stdio_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_Int_pa= 1; struct Cyc_Stdio_Int_pa_struct{ int tag;
unsigned int f1; } ; static const int Cyc_Stdio_Double_pa= 2; struct Cyc_Stdio_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Stdio_ShortPtr_sa= 0; struct
Cyc_Stdio_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int Cyc_Stdio_UShortPtr_sa=
1; struct Cyc_Stdio_UShortPtr_sa_struct{ int tag; unsigned short* f1; } ; static
const int Cyc_Stdio_IntPtr_sa= 2; struct Cyc_Stdio_IntPtr_sa_struct{ int tag;
int* f1; } ; static const int Cyc_Stdio_UIntPtr_sa= 3; struct Cyc_Stdio_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Stdio_StringPtr_sa= 4;
struct Cyc_Stdio_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_DoublePtr_sa= 5; struct Cyc_Stdio_DoublePtr_sa_struct{ int
tag; double* f1; } ; static const int Cyc_Stdio_FloatPtr_sa= 6; struct Cyc_Stdio_FloatPtr_sa_struct{
int tag; float* f1; } ; extern int Cyc_String_zstrptrcmp( struct _tagged_arr*,
struct _tagged_arr*); extern void* Cyc_Assert_AssertFail( struct _tagged_arr msg);
struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct _tagged_arr line;
int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[ 9u]; struct
Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse=
1; static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; struct
_tuple1{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual;
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
struct Cyc_Absyn_UnknownTunionInfo{ struct _tuple1* name; int is_xtunion; } ;
static const int Cyc_Absyn_UnknownTunion= 0; struct Cyc_Absyn_UnknownTunion_struct{
int tag; struct Cyc_Absyn_UnknownTunionInfo f1; } ; static const int Cyc_Absyn_KnownTunion=
1; struct Cyc_Absyn_KnownTunion_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
} ; struct Cyc_Absyn_TunionInfo{ void* tunion_info; struct Cyc_List_List* targs;
void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{ struct _tuple1*
tunion_name; struct _tuple1* field_name; int is_xtunion; } ; static const int
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
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; static const int Cyc_Absyn_UnionType= 10; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; static const int Cyc_Absyn_AnonStructType= 11; struct Cyc_Absyn_AnonStructType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType=
12; struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_EnumType= 13; struct Cyc_Absyn_EnumType_struct{ int
tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int Cyc_Absyn_RgnHandleType=
14; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 15; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
static const int Cyc_Absyn_HeapRgn= 3; static const int Cyc_Absyn_AccessEff= 16;
struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_JoinEff=
17; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_RgnsEff= 18; struct Cyc_Absyn_RgnsEff_struct{ int tag;
void* f1; } ; static const int Cyc_Absyn_NoTypes= 0; struct Cyc_Absyn_NoTypes_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; static
const int Cyc_Absyn_WithTypes= 1; struct Cyc_Absyn_WithTypes_struct{ int tag;
struct Cyc_List_List* f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt*
f4; struct Cyc_List_List* f5; } ; static const int Cyc_Absyn_NonNullable_ps= 0;
struct Cyc_Absyn_NonNullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_Nullable_ps= 1; struct Cyc_Absyn_Nullable_ps_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_TaggedArray_ps=
0; static const int Cyc_Absyn_Printf_ft= 0; static const int Cyc_Absyn_Scanf_ft=
1; static const int Cyc_Absyn_Regparm_att= 0; struct Cyc_Absyn_Regparm_att_struct{
int tag; int f1; } ; static const int Cyc_Absyn_Stdcall_att= 0; static const int
Cyc_Absyn_Cdecl_att= 1; static const int Cyc_Absyn_Fastcall_att= 2; static const
int Cyc_Absyn_Noreturn_att= 3; static const int Cyc_Absyn_Const_att= 4; static
const int Cyc_Absyn_Aligned_att= 1; struct Cyc_Absyn_Aligned_att_struct{ int tag;
int f1; } ; static const int Cyc_Absyn_Packed_att= 5; static const int Cyc_Absyn_Section_att=
2; struct Cyc_Absyn_Section_att_struct{ int tag; struct _tagged_arr f1; } ;
static const int Cyc_Absyn_Nocommon_att= 6; static const int Cyc_Absyn_Shared_att=
7; static const int Cyc_Absyn_Unused_att= 8; static const int Cyc_Absyn_Weak_att=
9; static const int Cyc_Absyn_Dllimport_att= 10; static const int Cyc_Absyn_Dllexport_att=
11; static const int Cyc_Absyn_No_instrument_function_att= 12; static const int
Cyc_Absyn_Constructor_att= 13; static const int Cyc_Absyn_Destructor_att= 14;
static const int Cyc_Absyn_No_check_memory_usage_att= 15; static const int Cyc_Absyn_Format_att=
3; struct Cyc_Absyn_Format_att_struct{ int tag; void* f1; int f2; int f3; } ;
static const int Cyc_Absyn_Carray_mod= 0; static const int Cyc_Absyn_ConstArray_mod=
0; struct Cyc_Absyn_ConstArray_mod_struct{ int tag; struct Cyc_Absyn_Exp* f1; }
; static const int Cyc_Absyn_Pointer_mod= 1; struct Cyc_Absyn_Pointer_mod_struct{
int tag; void* f1; void* f2; struct Cyc_Absyn_Tqual f3; } ; static const int Cyc_Absyn_Function_mod=
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
19; static const int Cyc_Absyn_PreInc= 0; static const int Cyc_Absyn_PostInc= 1;
static const int Cyc_Absyn_PreDec= 2; static const int Cyc_Absyn_PostDec= 3;
struct Cyc_Absyn_VarargCallInfo{ int num_varargs; struct Cyc_List_List*
injectors; struct Cyc_Absyn_VarargInfo* vai; } ; static const int Cyc_Absyn_Const_e=
0; struct Cyc_Absyn_Const_e_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Var_e=
1; struct Cyc_Absyn_Var_e_struct{ int tag; struct _tuple1* f1; void* f2; } ;
static const int Cyc_Absyn_UnknownId_e= 2; struct Cyc_Absyn_UnknownId_e_struct{
int tag; struct _tuple1* f1; } ; static const int Cyc_Absyn_Primop_e= 3; struct
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
24; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple2* f1; struct
Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e= 25; struct Cyc_Absyn_Array_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Comprehension_e=
26; struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Struct_e=
27; struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple1* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4; } ; static const
int Cyc_Absyn_AnonStruct_e= 28; struct Cyc_Absyn_AnonStruct_e_struct{ int tag;
void* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Tunion_e= 29;
struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_e= 30; struct Cyc_Absyn_Enum_e_struct{
int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
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
f3; } ; static const int Cyc_Absyn_While_s= 4; struct _tuple3{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple3 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_arr* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_For_s= 8; struct Cyc_Absyn_For_s_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct _tuple3 f2; struct _tuple3 f3; struct Cyc_Absyn_Stmt*
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
_tuple3 f2; } ; static const int Cyc_Absyn_TryCatch_s= 17; struct Cyc_Absyn_TryCatch_s_struct{
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
struct Cyc_Absyn_UnknownId_p_struct{ int tag; struct _tuple1* f1; } ; static
const int Cyc_Absyn_UnknownCall_p= 11; struct Cyc_Absyn_UnknownCall_p_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; static const int Cyc_Absyn_UnknownFields_p= 12; struct Cyc_Absyn_UnknownFields_p_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
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
void* sc; struct _tuple1* name; struct Cyc_Absyn_Tqual tq; void* type; struct
Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int is_inline; struct _tuple1*
name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt* effect; void* ret_type;
struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
cyc_varargs; struct Cyc_List_List* rgn_po; struct Cyc_Absyn_Stmt* body; struct
Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structfield{ struct _tagged_arr* name; struct
Cyc_Absyn_Tqual tq; void* type; struct Cyc_Absyn_Exp* width; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name;
struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name;
struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Tunionfield{ struct _tuple1* name; struct Cyc_List_List*
tvs; struct Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ;
struct Cyc_Absyn_Tuniondecl{ void* sc; struct _tuple1* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{
struct _tuple1* name; struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc; struct _tuple1* name; struct Cyc_Core_Opt*
fields; } ; struct Cyc_Absyn_Typedefdecl{ struct _tuple1* name; struct Cyc_List_List*
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
Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{ int tag; struct _tuple1*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d= 11;
struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ; struct
Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static const int
Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int tag; struct
Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1; struct Cyc_Absyn_FieldName_struct{
int tag; struct _tagged_arr* f1; } ; extern unsigned char Cyc_Absyn_EmptyAnnot[
15u]; extern struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_false_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_structmember_exp( struct Cyc_Absyn_Exp*,
struct _tagged_arr*, struct Cyc_Position_Segment*); struct Cyc_Set_Set; extern
unsigned char Cyc_Set_Absent[ 11u]; static const int Cyc_Tcenv_VarRes= 0; struct
Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes=
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
ae; struct Cyc_Core_Opt* le; } ; extern unsigned char Cyc_Tcutil_TypeErr[ 12u];
extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct _tagged_arr
fmt, struct _tagged_arr ap); extern void* Cyc_Tcutil_compress( void* t); extern
int Cyc_Tcutil_is_tagged_pointer_typ( void*); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); struct Cyc_CfFlowInfo_Place; static const int Cyc_CfFlowInfo_VarRoot=
0; struct Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
} ; static const int Cyc_CfFlowInfo_MallocPt= 1; struct Cyc_CfFlowInfo_MallocPt_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_CfFlowInfo_StructF=
0; struct Cyc_CfFlowInfo_StructF_struct{ int tag; struct _tagged_arr* f1; } ;
static const int Cyc_CfFlowInfo_TupleF= 1; struct Cyc_CfFlowInfo_TupleF_struct{
int tag; int f1; } ; struct Cyc_CfFlowInfo_Place{ void* root; struct Cyc_List_List*
fields; } ; static const int Cyc_CfFlowInfo_Esc= 0; static const int Cyc_CfFlowInfo_Unesc=
1; static const int Cyc_CfFlowInfo_NoneIL= 0; static const int Cyc_CfFlowInfo_ThisIL=
1; static const int Cyc_CfFlowInfo_AllIL= 2; static const int Cyc_CfFlowInfo_UnknownIS=
0; struct Cyc_CfFlowInfo_UnknownIS_struct{ int tag; void* f1; void* f2; } ;
static const int Cyc_CfFlowInfo_MustPointTo= 1; struct Cyc_CfFlowInfo_MustPointTo_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1; } ; static const int Cyc_CfFlowInfo_LeafPI=
0; struct Cyc_CfFlowInfo_LeafPI_struct{ int tag; void* f1; } ; static const int
Cyc_CfFlowInfo_TuplePI= 1; struct Cyc_CfFlowInfo_TuplePI_struct{ int tag; struct
Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_StructPI= 2; struct Cyc_CfFlowInfo_StructPI_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_BottomFL=
0; static const int Cyc_CfFlowInfo_InitsFL= 0; struct Cyc_CfFlowInfo_InitsFL_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; extern int Cyc_CfFlowInfo_local_root_cmp(
void*, void*); extern void* Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( void* t);
extern void* Cyc_CfFlowInfo_assign_unknown_dict( void*, void*); extern void* Cyc_CfFlowInfo_join_flow(
void*, void*); extern int Cyc_CfFlowInfo_flow_lessthan_approx( void* f1, void*
f2); extern void Cyc_NewControlFlow_set_encloser( struct Cyc_Absyn_Stmt*
enclosee, struct Cyc_Absyn_Stmt* encloser); struct Cyc_NewControlFlow_AnalEnv{
struct Cyc_Dict_Dict* roots; int in_try; void* tryflow; } ; extern void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv* env, void* new_flow); extern void* Cyc_NewControlFlow_cf_analyze_stmt(
struct Cyc_NewControlFlow_AnalEnv*, struct Cyc_Absyn_Stmt*, void*); extern void
Cyc_NewControlFlow_cf_check( struct Cyc_List_List* ds); extern unsigned char Cyc_CfAbsexp_BadAbsexp[
14u]; static const int Cyc_CfAbsexp_OrderedG= 0; static const int Cyc_CfAbsexp_UnorderedG=
1; static const int Cyc_CfAbsexp_OneofG= 2; extern void* Cyc_CfAbsexp_mkAnyOrderG();
extern void* Cyc_CfAbsexp_mkUnknownOp(); extern void* Cyc_CfAbsexp_mkAddressOp(
void* ao); extern void* Cyc_CfAbsexp_mkLocalOp( struct Cyc_Absyn_Vardecl* vd);
extern void* Cyc_CfAbsexp_mkMemberOp( void* ao, void* f); extern void* Cyc_CfAbsexp_mkMallocOp(
struct Cyc_Absyn_Exp* e); extern void* Cyc_CfAbsexp_mkDerefOp( void* ao); extern
void* Cyc_CfAbsexp_mkBottomAE(); extern void* Cyc_CfAbsexp_mkSkipAE(); extern
void* Cyc_CfAbsexp_mkAssignAE( void* l, void* r); extern void* Cyc_CfAbsexp_mkMallocAE(
struct Cyc_Absyn_Exp* e); extern void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt*
s); extern void* Cyc_CfAbsexp_mkGroupAE( void* g, void* ae1, void* ae2); extern
void* Cyc_CfAbsexp_mkGroupAE_l( void* g, struct Cyc_List_List* ael); extern int
Cyc_CfAbsexp_isUnknownOp( void*); extern void* Cyc_CfAbsexp_eval_absexp( struct
Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Position_Segment* loc, void* ae,
void* in_flow); struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int qvar_to_Cids: 1; int
add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int rewrite_temp_tvars: 1;
int print_all_tvars: 1; int print_all_kinds: 1; int print_using_stmts: 1; int
print_externC_stmts: 1; int print_full_evars: 1; int use_curr_namespace: 1;
struct Cyc_List_List* curr_namespace; } ; struct Cyc_NewControlFlow_CFStmtAnnot{
struct Cyc_Absyn_Stmt* encloser; int visited; void* flow; struct Cyc_List_List*
absexps; } ; unsigned char Cyc_NewControlFlow_CFAnnot[ 12u]="\000\000\000\000CFAnnot";
struct Cyc_NewControlFlow_CFAnnot_struct{ unsigned char* tag; struct Cyc_NewControlFlow_CFStmtAnnot
f1; } ; void Cyc_NewControlFlow_set_encloser( struct Cyc_Absyn_Stmt* enclosee,
struct Cyc_Absyn_Stmt* encloser){( void*)( enclosee->annot=( void*)(( void*)({
struct Cyc_NewControlFlow_CFAnnot_struct* _temp0=( struct Cyc_NewControlFlow_CFAnnot_struct*)
GC_malloc( sizeof( struct Cyc_NewControlFlow_CFAnnot_struct)); _temp0[ 0]=({
struct Cyc_NewControlFlow_CFAnnot_struct _temp1; _temp1.tag= Cyc_NewControlFlow_CFAnnot;
_temp1.f1=({ struct Cyc_NewControlFlow_CFStmtAnnot _temp2; _temp2.encloser=
encloser; _temp2.visited= 0; _temp2.flow=( void*)(( void*) Cyc_CfFlowInfo_BottomFL);
_temp2.absexps= 0; _temp2;}); _temp1;}); _temp0;})));} static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_get_stmt_annot( struct Cyc_Absyn_Stmt* s){ void* _temp3=(
void*) s->annot; struct Cyc_NewControlFlow_CFStmtAnnot _temp9; struct Cyc_NewControlFlow_CFStmtAnnot*
_temp11; _LL5: if(*(( void**) _temp3) == Cyc_NewControlFlow_CFAnnot){ _LL10:
_temp9=(( struct Cyc_NewControlFlow_CFAnnot_struct*) _temp3)->f1; _temp11=&((
struct Cyc_NewControlFlow_CFAnnot_struct*) _temp3)->f1; goto _LL6;} else{ goto
_LL7;} _LL7: goto _LL8; _LL6: return _temp11; _LL8:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp12=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp12[ 0]=({ struct
Cyc_Core_Impossible_struct _temp13; _temp13.tag= Cyc_Core_Impossible; _temp13.f1=
_tag_arr("ControlFlow -- wrong stmt annotation", sizeof( unsigned char), 37u);
_temp13;}); _temp12;})); _LL4:;} void Cyc_NewControlFlow_cf_set_absexps( struct
Cyc_Absyn_Stmt* s, struct Cyc_List_List* ael){( Cyc_NewControlFlow_get_stmt_annot(
s))->absexps= ael;} struct Cyc_NewControlFlow_SharedAbsEnv{ struct Cyc_Dict_Dict*
all_roots; struct Cyc_List_List* comprehension_vars; } ; struct Cyc_NewControlFlow_AbsEnv{
struct Cyc_NewControlFlow_SharedAbsEnv* shared; struct Cyc_List_List* lhsides;
struct Cyc_List_List* access_path; } ; struct Cyc_NewControlFlow_AbsSynOne{ void*
inner_exp; void* assigns; } ; struct Cyc_NewControlFlow_AbsSyn{ struct Cyc_NewControlFlow_AbsSynOne
when_true; struct Cyc_NewControlFlow_AbsSynOne when_false; struct Cyc_List_List*
lvalues; } ; void Cyc_NewControlFlow_add_var_root( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Vardecl* vd){( env.shared)->all_roots= Cyc_Dict_insert_new((
env.shared)->all_roots,( void*)({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp14=(
struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp14[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp15; _temp15.tag= Cyc_CfFlowInfo_VarRoot;
_temp15.f1= vd; _temp15;}); _temp14;}), Cyc_CfFlowInfo_typ_to_unesc_none_pinfo((
void*) vd->type));} void Cyc_NewControlFlow_add_malloc_root( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e, void* t){( env.shared)->all_roots= Cyc_Dict_insert_new((
env.shared)->all_roots,( void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp16=(
struct Cyc_CfFlowInfo_MallocPt_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp16[ 0]=({ struct Cyc_CfFlowInfo_MallocPt_struct _temp17; _temp17.tag= Cyc_CfFlowInfo_MallocPt;
_temp17.f1= e; _temp17;}); _temp16;}), Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( t));}
struct Cyc_NewControlFlow_AbsEnv Cyc_NewControlFlow_change_lhs( struct Cyc_NewControlFlow_AbsEnv
src, struct Cyc_List_List* new_lhs){ return({ struct Cyc_NewControlFlow_AbsEnv
_temp18; _temp18.shared= src.shared; _temp18.lhsides= new_lhs; _temp18.access_path=
src.access_path; _temp18;});} static struct Cyc_List_List* Cyc_NewControlFlow_unknown_list=
0; static struct Cyc_List_List* Cyc_NewControlFlow_use_it_list(){ if( Cyc_NewControlFlow_unknown_list
== 0){ Cyc_NewControlFlow_unknown_list=({ struct Cyc_List_List* _temp19=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp19->hd=( void*)
Cyc_CfAbsexp_mkUnknownOp(); _temp19->tl= 0; _temp19;});} return Cyc_NewControlFlow_unknown_list;}
struct Cyc_NewControlFlow_AbsEnv Cyc_NewControlFlow_use_it_env( struct Cyc_NewControlFlow_AbsEnv
src){ struct Cyc_NewControlFlow_AbsEnv _temp20= Cyc_NewControlFlow_change_lhs(
src, Cyc_NewControlFlow_use_it_list()); _temp20.access_path= 0; return _temp20;}
struct Cyc_NewControlFlow_AbsSynOne Cyc_NewControlFlow_meet_absexp( struct Cyc_NewControlFlow_AbsSyn
syn){ struct Cyc_NewControlFlow_AbsSynOne _temp23; void* _temp25; void* _temp27;
struct Cyc_NewControlFlow_AbsSynOne _temp29; void* _temp31; void* _temp33;
struct Cyc_NewControlFlow_AbsSyn _temp21= syn; _LL30: _temp29= _temp21.when_true;
_LL34: _temp33=( void*) _temp29.inner_exp; goto _LL32; _LL32: _temp31=( void*)
_temp29.assigns; goto _LL24; _LL24: _temp23= _temp21.when_false; _LL28: _temp27=(
void*) _temp23.inner_exp; goto _LL26; _LL26: _temp25=( void*) _temp23.assigns;
goto _LL22; _LL22: { void* _temp35= _temp33 == _temp27? _temp33: Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OneofG, _temp33, _temp27); void* _temp36= _temp31 == _temp25?
_temp31: Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG, _temp31, _temp25);
return({ struct Cyc_NewControlFlow_AbsSynOne _temp37; _temp37.inner_exp=( void*)
_temp35; _temp37.assigns=( void*) _temp36; _temp37;});}} struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_mkSyn_tf( void* ae, struct Cyc_List_List* lvals, void*
assigns){ struct Cyc_NewControlFlow_AbsSynOne _temp38=({ struct Cyc_NewControlFlow_AbsSynOne
_temp40; _temp40.inner_exp=( void*) ae; _temp40.assigns=( void*) assigns;
_temp40;}); return({ struct Cyc_NewControlFlow_AbsSyn _temp39; _temp39.when_true=
_temp38; _temp39.when_false= _temp38; _temp39.lvalues= lvals; _temp39;});} void*
Cyc_NewControlFlow_make_assigns( struct Cyc_NewControlFlow_AbsEnv env, void* ae){
if( env.lhsides == 0){ return Cyc_CfAbsexp_mkSkipAE();} if((( struct Cyc_List_List*)
_check_null( env.lhsides))->tl == 0){ return Cyc_CfAbsexp_mkAssignAE(( void*)((
struct Cyc_List_List*) _check_null( env.lhsides))->hd, ae);}{ struct Cyc_List_List*
_temp41= 0;{ struct Cyc_List_List* lhsides= env.lhsides; for( 0; lhsides != 0;
lhsides=(( struct Cyc_List_List*) _check_null( lhsides))->tl){ _temp41=({ struct
Cyc_List_List* _temp42=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp42->hd=( void*) Cyc_CfAbsexp_mkAssignAE(( void*)(( struct Cyc_List_List*)
_check_null( lhsides))->hd, ae); _temp42->tl= _temp41; _temp42;});}} return Cyc_CfAbsexp_mkGroupAE_l((
void*) Cyc_CfAbsexp_OneofG, _temp41);}} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e); void Cyc_NewControlFlow_cf_prepass(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Stmt* s); struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_use_exp_unordered( struct Cyc_NewControlFlow_AbsEnv env,
struct Cyc_List_List* es, struct Cyc_List_List* lhs){ struct Cyc_NewControlFlow_AbsEnv
elt_env= Cyc_NewControlFlow_use_it_env( env); void* es_exp= Cyc_CfAbsexp_mkSkipAE();
void* uses_exp= Cyc_CfAbsexp_mkSkipAE(); for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ struct Cyc_NewControlFlow_AbsSyn _temp43= Cyc_NewControlFlow_abstract_exp(
elt_env,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);
void* _temp46; void* _temp48; struct Cyc_NewControlFlow_AbsSynOne _temp44= Cyc_NewControlFlow_meet_absexp(
_temp43); _LL49: _temp48=( void*) _temp44.inner_exp; goto _LL47; _LL47: _temp46=(
void*) _temp44.assigns; goto _LL45; _LL45: es_exp= Cyc_CfAbsexp_mkGroupAE(( void*)
Cyc_CfAbsexp_UnorderedG, _temp48, es_exp); uses_exp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
_temp46, uses_exp);} return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, es_exp, uses_exp), lhs, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_descend_path(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* path_e, struct Cyc_Absyn_Exp*
inner_e){ env.access_path=({ struct Cyc_List_List* _temp50=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp50->hd=( void*) path_e; _temp50->tl=
env.access_path; _temp50;});{ struct Cyc_NewControlFlow_AbsSyn _temp51= Cyc_NewControlFlow_abstract_exp(
env, inner_e); if( _temp51.lvalues == 0){ _temp51.lvalues= Cyc_NewControlFlow_use_it_list();}
return _temp51;}} void* Cyc_NewControlFlow_destruct_path( struct Cyc_NewControlFlow_AbsEnv
env, void* absop){{ struct Cyc_List_List* path= env.access_path; for( 0; path !=
0; path=(( struct Cyc_List_List*) _check_null( path))->tl){ void* _temp52=( void*)((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( path))->hd)->r;
struct _tagged_arr* _temp66; struct _tagged_arr* _temp68; struct Cyc_Absyn_Exp*
_temp70; _LL54: if(*(( int*) _temp52) == Cyc_Absyn_Address_e){ goto _LL55;}
else{ goto _LL56;} _LL56: if(*(( int*) _temp52) == Cyc_Absyn_Deref_e){ goto
_LL57;} else{ goto _LL58;} _LL58: if(*(( int*) _temp52) == Cyc_Absyn_StructArrow_e){
_LL67: _temp66=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp52)->f2; goto
_LL59;} else{ goto _LL60;} _LL60: if(*(( int*) _temp52) == Cyc_Absyn_StructMember_e){
_LL69: _temp68=(( struct Cyc_Absyn_StructMember_e_struct*) _temp52)->f2; goto
_LL61;} else{ goto _LL62;} _LL62: if(*(( int*) _temp52) == Cyc_Absyn_Subscript_e){
_LL71: _temp70=(( struct Cyc_Absyn_Subscript_e_struct*) _temp52)->f2; goto _LL63;}
else{ goto _LL64;} _LL64: goto _LL65; _LL55:(( struct Cyc_List_List*)
_check_null( path))->tl == 0? 0:(( int(*)( struct _tagged_arr msg)) Cyc_Assert_AssertFail)(
_tag_arr("new_control_flow.cyc:238 path->tl == null", sizeof( unsigned char), 42u));
return Cyc_CfAbsexp_mkAddressOp( absop); _LL57: absop= Cyc_CfAbsexp_mkDerefOp(
absop); goto _LL53; _LL59: absop= Cyc_CfAbsexp_mkDerefOp( absop); _temp68=
_temp66; goto _LL61; _LL61: absop= Cyc_CfAbsexp_mkMemberOp( absop,( void*)({
struct Cyc_CfFlowInfo_StructF_struct* _temp72=( struct Cyc_CfFlowInfo_StructF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct)); _temp72[ 0]=({ struct
Cyc_CfFlowInfo_StructF_struct _temp73; _temp73.tag= Cyc_CfFlowInfo_StructF;
_temp73.f1= _temp68; _temp73;}); _temp72;})); goto _LL53; _LL63: { unsigned int
_temp74= Cyc_Evexp_eval_const_uint_exp( _temp70); absop= Cyc_CfAbsexp_mkMemberOp(
absop,( void*)({ struct Cyc_CfFlowInfo_TupleF_struct* _temp75=( struct Cyc_CfFlowInfo_TupleF_struct*)
GC_malloc_atomic( sizeof( struct Cyc_CfFlowInfo_TupleF_struct)); _temp75[ 0]=({
struct Cyc_CfFlowInfo_TupleF_struct _temp76; _temp76.tag= Cyc_CfFlowInfo_TupleF;
_temp76.f1=( int) _temp74; _temp76;}); _temp75;})); goto _LL53;} _LL65:( int)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp77=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp77[ 0]=({ struct
Cyc_Core_Impossible_struct _temp78; _temp78.tag= Cyc_Core_Impossible; _temp78.f1=
_tag_arr("abstract_exp: unexpected acces path element", sizeof( unsigned char),
44u); _temp78;}); _temp77;})); _LL53:;}} return absop;} struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_abstract_malloc( struct Cyc_NewControlFlow_AbsEnv env, struct
Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp* rgn_exp, struct Cyc_Absyn_Exp*
init_exp){ void* _temp79= Cyc_CfAbsexp_mkMallocOp( outer_e); void* _temp80= Cyc_CfAbsexp_mkSkipAE();
void* _temp81= Cyc_CfAbsexp_mkSkipAE(); if( init_exp != 0){ struct Cyc_NewControlFlow_AbsEnv
_temp82= Cyc_NewControlFlow_change_lhs( env,({ struct Cyc_List_List* _temp89=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp89->hd=(
void*) _temp79; _temp89->tl= 0; _temp89;})); _temp82.access_path= 0;{ void*
_temp85; void* _temp87; struct Cyc_NewControlFlow_AbsSynOne _temp83= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( _temp82,( struct Cyc_Absyn_Exp*) _check_null(
init_exp))); _LL88: _temp87=( void*) _temp83.inner_exp; goto _LL86; _LL86:
_temp85=( void*) _temp83.assigns; goto _LL84; _LL84: _temp80= _temp87; _temp81=
_temp85;}} if( rgn_exp != 0){ struct Cyc_NewControlFlow_AbsSyn _temp90= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env),( struct Cyc_Absyn_Exp*) _check_null(
rgn_exp)); void* _temp93; void* _temp95; struct Cyc_NewControlFlow_AbsSynOne
_temp91= Cyc_NewControlFlow_meet_absexp( _temp90); _LL96: _temp95=( void*)
_temp91.inner_exp; goto _LL94; _LL94: _temp93=( void*) _temp91.assigns; goto
_LL92; _LL92: _temp80= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp80, _temp95); _temp81= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
_temp81, _temp93);}{ void* _temp97= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkMallocAE( outer_e), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp80, _temp81)); struct Cyc_NewControlFlow_AbsSyn _temp98= Cyc_NewControlFlow_mkSyn_tf(
_temp97, 0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkAddressOp(
_temp79))); if( env.access_path != 0){ void* _temp99=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( env.access_path))->hd)->r; struct
_tagged_arr* _temp107; struct Cyc_Absyn_Exp* _temp109; _LL101: if(*(( int*)
_temp99) == Cyc_Absyn_Deref_e){ goto _LL102;} else{ goto _LL103;} _LL103: if(*((
int*) _temp99) == Cyc_Absyn_StructArrow_e){ _LL110: _temp109=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp99)->f1; goto _LL108; _LL108: _temp107=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp99)->f2; goto _LL104;} else{ goto _LL105;} _LL105: goto _LL106; _LL102: env.access_path=((
struct Cyc_List_List*) _check_null( env.access_path))->tl; _temp98.lvalues=({
struct Cyc_List_List* _temp111=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp111->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, _temp79); _temp111->tl= 0; _temp111;}); goto _LL100; _LL104: env.access_path=({
struct Cyc_List_List* _temp112=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp112->hd=( void*) Cyc_Absyn_structmember_exp(
_temp109, _temp107, 0); _temp112->tl=(( struct Cyc_List_List*) _check_null( env.access_path))->tl;
_temp112;}); _temp98.lvalues=({ struct Cyc_List_List* _temp113=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp113->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, _temp79); _temp113->tl= 0; _temp113;}); goto _LL100; _LL106:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp114=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp114[ 0]=({ struct
Cyc_Core_Impossible_struct _temp115; _temp115.tag= Cyc_Core_Impossible; _temp115.f1=
_tag_arr("bad malloc access path", sizeof( unsigned char), 23u); _temp115;});
_temp114;})); _LL100:;} return _temp98;}} struct _tuple4{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Exp* f2; } ; struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ void* _temp116=(
void*) e->r; struct Cyc_Absyn_Exp* _temp206; struct Cyc_Absyn_Exp* _temp208;
void* _temp210; int _temp212; void* _temp214; void* _temp216; int _temp218; void*
_temp220; void* _temp222; void* _temp224; void* _temp226; struct Cyc_Absyn_Vardecl*
_temp228; void* _temp230; struct Cyc_Absyn_Vardecl* _temp232; void* _temp234;
struct Cyc_Absyn_Vardecl* _temp236; struct Cyc_Absyn_Exp* _temp238; struct Cyc_Core_Opt*
_temp240; struct Cyc_Absyn_Exp* _temp242; struct Cyc_Absyn_Exp* _temp244; struct
Cyc_Absyn_Exp* _temp246; struct Cyc_List_List* _temp248; struct Cyc_Absyn_Exp*
_temp250; struct Cyc_List_List* _temp252; struct Cyc_Absyn_Exp* _temp254; struct
Cyc_Absyn_Exp* _temp256; struct Cyc_Absyn_Exp* _temp258; struct Cyc_Absyn_Exp*
_temp260; struct Cyc_Absyn_Exp* _temp262; struct Cyc_Absyn_Exp* _temp264; void*
_temp266; struct Cyc_Absyn_Exp* _temp268; struct Cyc_Absyn_Exp* _temp270; struct
Cyc_Absyn_Exp* _temp272; struct Cyc_Absyn_Exp* _temp274; struct Cyc_List_List*
_temp276; struct Cyc_List_List* _temp278; struct Cyc_List_List* _temp280; struct
Cyc_List_List* _temp282; struct Cyc_List_List* _temp284; struct Cyc_Absyn_Exp*
_temp286; struct Cyc_Absyn_Exp* _temp288; struct Cyc_Absyn_Exp* _temp290; struct
Cyc_Absyn_Exp* _temp292; struct Cyc_Absyn_Exp* _temp294; struct Cyc_Absyn_Stmt*
_temp296; struct Cyc_Absyn_Exp* _temp298; struct Cyc_Absyn_Exp* _temp300; struct
Cyc_Absyn_Exp* _temp302; struct Cyc_Absyn_Exp* _temp304; struct Cyc_Absyn_Vardecl*
_temp306; void* _temp308; _LL118: if(*(( int*) _temp116) == Cyc_Absyn_NoInstantiate_e){
_LL207: _temp206=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp116)->f1;
goto _LL119;} else{ goto _LL120;} _LL120: if(*(( int*) _temp116) == Cyc_Absyn_Instantiate_e){
_LL209: _temp208=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp116)->f1; goto
_LL121;} else{ goto _LL122;} _LL122: if(*(( int*) _temp116) == Cyc_Absyn_Const_e){
_LL211: _temp210=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp116)->f1; if((
unsigned int) _temp210 > 1u?*(( int*) _temp210) == Cyc_Absyn_Int_c: 0){ _LL215:
_temp214=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp210)->f1; if( _temp214
==( void*) Cyc_Absyn_Signed){ goto _LL213;} else{ goto _LL124;} _LL213: _temp212=((
struct Cyc_Absyn_Int_c_struct*) _temp210)->f2; if( _temp212 == 0){ goto _LL123;}
else{ goto _LL124;}} else{ goto _LL124;}} else{ goto _LL124;} _LL124: if(*(( int*)
_temp116) == Cyc_Absyn_Const_e){ _LL217: _temp216=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp116)->f1; if(( unsigned int) _temp216 > 1u?*(( int*) _temp216) == Cyc_Absyn_Int_c:
0){ _LL221: _temp220=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp216)->f1;
if( _temp220 ==( void*) Cyc_Absyn_Signed){ goto _LL219;} else{ goto _LL126;}
_LL219: _temp218=(( struct Cyc_Absyn_Int_c_struct*) _temp216)->f2; if( _temp218
== 1){ goto _LL125;} else{ goto _LL126;}} else{ goto _LL126;}} else{ goto _LL126;}
_LL126: if(*(( int*) _temp116) == Cyc_Absyn_Sizeofexp_e){ goto _LL127;} else{
goto _LL128;} _LL128: if(*(( int*) _temp116) == Cyc_Absyn_Sizeoftyp_e){ goto
_LL129;} else{ goto _LL130;} _LL130: if(*(( int*) _temp116) == Cyc_Absyn_Offsetof_e){
goto _LL131;} else{ goto _LL132;} _LL132: if(*(( int*) _temp116) == Cyc_Absyn_Const_e){
goto _LL133;} else{ goto _LL134;} _LL134: if(*(( int*) _temp116) == Cyc_Absyn_Enum_e){
goto _LL135;} else{ goto _LL136;} _LL136: if(*(( int*) _temp116) == Cyc_Absyn_Var_e){
_LL223: _temp222=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp116)->f2; if((
unsigned int) _temp222 > 1u?*(( int*) _temp222) == Cyc_Absyn_Funname_b: 0){ goto
_LL137;} else{ goto _LL138;}} else{ goto _LL138;} _LL138: if(*(( int*) _temp116)
== Cyc_Absyn_Var_e){ _LL225: _temp224=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp116)->f2; if(( unsigned int) _temp224 > 1u?*(( int*) _temp224) == Cyc_Absyn_Global_b:
0){ goto _LL139;} else{ goto _LL140;}} else{ goto _LL140;} _LL140: if(*(( int*)
_temp116) == Cyc_Absyn_Var_e){ _LL227: _temp226=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp116)->f2; if(( unsigned int) _temp226 > 1u?*(( int*) _temp226) == Cyc_Absyn_Param_b:
0){ _LL229: _temp228=(( struct Cyc_Absyn_Param_b_struct*) _temp226)->f1; goto
_LL141;} else{ goto _LL142;}} else{ goto _LL142;} _LL142: if(*(( int*) _temp116)
== Cyc_Absyn_Var_e){ _LL231: _temp230=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp116)->f2; if(( unsigned int) _temp230 > 1u?*(( int*) _temp230) == Cyc_Absyn_Local_b:
0){ _LL233: _temp232=(( struct Cyc_Absyn_Local_b_struct*) _temp230)->f1; goto
_LL143;} else{ goto _LL144;}} else{ goto _LL144;} _LL144: if(*(( int*) _temp116)
== Cyc_Absyn_Var_e){ _LL235: _temp234=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp116)->f2; if(( unsigned int) _temp234 > 1u?*(( int*) _temp234) == Cyc_Absyn_Pat_b:
0){ _LL237: _temp236=(( struct Cyc_Absyn_Pat_b_struct*) _temp234)->f1; goto
_LL145;} else{ goto _LL146;}} else{ goto _LL146;} _LL146: if(*(( int*) _temp116)
== Cyc_Absyn_AssignOp_e){ _LL243: _temp242=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp116)->f1; goto _LL241; _LL241: _temp240=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp116)->f2; if( _temp240 == 0){ goto _LL239;} else{ goto _LL148;} _LL239:
_temp238=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp116)->f3; goto _LL147;}
else{ goto _LL148;} _LL148: if(*(( int*) _temp116) == Cyc_Absyn_AssignOp_e){
_LL247: _temp246=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp116)->f1; goto
_LL245; _LL245: _temp244=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp116)->f3;
goto _LL149;} else{ goto _LL150;} _LL150: if(*(( int*) _temp116) == Cyc_Absyn_Primop_e){
_LL249: _temp248=(( struct Cyc_Absyn_Primop_e_struct*) _temp116)->f2; goto
_LL151;} else{ goto _LL152;} _LL152: if(*(( int*) _temp116) == Cyc_Absyn_Cast_e){
_LL251: _temp250=(( struct Cyc_Absyn_Cast_e_struct*) _temp116)->f2; goto _LL153;}
else{ goto _LL154;} _LL154: if(*(( int*) _temp116) == Cyc_Absyn_FnCall_e){
_LL255: _temp254=(( struct Cyc_Absyn_FnCall_e_struct*) _temp116)->f1; goto
_LL253; _LL253: _temp252=(( struct Cyc_Absyn_FnCall_e_struct*) _temp116)->f2;
goto _LL155;} else{ goto _LL156;} _LL156: if(*(( int*) _temp116) == Cyc_Absyn_Subscript_e){
_LL259: _temp258=(( struct Cyc_Absyn_Subscript_e_struct*) _temp116)->f1; goto
_LL257; _LL257: _temp256=(( struct Cyc_Absyn_Subscript_e_struct*) _temp116)->f2;
goto _LL157;} else{ goto _LL158;} _LL158: if(*(( int*) _temp116) == Cyc_Absyn_Address_e){
_LL261: _temp260=(( struct Cyc_Absyn_Address_e_struct*) _temp116)->f1; goto
_LL159;} else{ goto _LL160;} _LL160: if(*(( int*) _temp116) == Cyc_Absyn_New_e){
_LL265: _temp264=(( struct Cyc_Absyn_New_e_struct*) _temp116)->f1; goto _LL263;
_LL263: _temp262=(( struct Cyc_Absyn_New_e_struct*) _temp116)->f2; goto _LL161;}
else{ goto _LL162;} _LL162: if(*(( int*) _temp116) == Cyc_Absyn_Malloc_e){
_LL269: _temp268=(( struct Cyc_Absyn_Malloc_e_struct*) _temp116)->f1; goto
_LL267; _LL267: _temp266=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp116)->f2;
goto _LL163;} else{ goto _LL164;} _LL164: if(*(( int*) _temp116) == Cyc_Absyn_Deref_e){
_LL271: _temp270=(( struct Cyc_Absyn_Deref_e_struct*) _temp116)->f1; goto _LL165;}
else{ goto _LL166;} _LL166: if(*(( int*) _temp116) == Cyc_Absyn_StructArrow_e){
_LL273: _temp272=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp116)->f1; goto
_LL167;} else{ goto _LL168;} _LL168: if(*(( int*) _temp116) == Cyc_Absyn_StructMember_e){
_LL275: _temp274=(( struct Cyc_Absyn_StructMember_e_struct*) _temp116)->f1; goto
_LL169;} else{ goto _LL170;} _LL170: if(*(( int*) _temp116) == Cyc_Absyn_Tunion_e){
_LL277: _temp276=(( struct Cyc_Absyn_Tunion_e_struct*) _temp116)->f3; goto
_LL171;} else{ goto _LL172;} _LL172: if(*(( int*) _temp116) == Cyc_Absyn_Tuple_e){
_LL279: _temp278=(( struct Cyc_Absyn_Tuple_e_struct*) _temp116)->f1; goto _LL173;}
else{ goto _LL174;} _LL174: if(*(( int*) _temp116) == Cyc_Absyn_AnonStruct_e){
_LL281: _temp280=(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp116)->f2; goto
_LL175;} else{ goto _LL176;} _LL176: if(*(( int*) _temp116) == Cyc_Absyn_Struct_e){
_LL283: _temp282=(( struct Cyc_Absyn_Struct_e_struct*) _temp116)->f3; goto
_LL177;} else{ goto _LL178;} _LL178: if(*(( int*) _temp116) == Cyc_Absyn_Array_e){
_LL285: _temp284=(( struct Cyc_Absyn_Array_e_struct*) _temp116)->f1; goto _LL179;}
else{ goto _LL180;} _LL180: if(*(( int*) _temp116) == Cyc_Absyn_Increment_e){
_LL287: _temp286=(( struct Cyc_Absyn_Increment_e_struct*) _temp116)->f1; goto
_LL181;} else{ goto _LL182;} _LL182: if(*(( int*) _temp116) == Cyc_Absyn_Throw_e){
_LL289: _temp288=(( struct Cyc_Absyn_Throw_e_struct*) _temp116)->f1; goto _LL183;}
else{ goto _LL184;} _LL184: if(*(( int*) _temp116) == Cyc_Absyn_Conditional_e){
_LL295: _temp294=(( struct Cyc_Absyn_Conditional_e_struct*) _temp116)->f1; goto
_LL293; _LL293: _temp292=(( struct Cyc_Absyn_Conditional_e_struct*) _temp116)->f2;
goto _LL291; _LL291: _temp290=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp116)->f3; goto _LL185;} else{ goto _LL186;} _LL186: if(*(( int*) _temp116)
== Cyc_Absyn_StmtExp_e){ _LL297: _temp296=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp116)->f1; goto _LL187;} else{ goto _LL188;} _LL188: if(*(( int*) _temp116)
== Cyc_Absyn_SeqExp_e){ _LL301: _temp300=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp116)->f1; goto _LL299; _LL299: _temp298=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp116)->f2; goto _LL189;} else{ goto _LL190;} _LL190: if(*(( int*) _temp116)
== Cyc_Absyn_Comprehension_e){ _LL307: _temp306=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp116)->f1; goto _LL305; _LL305: _temp304=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp116)->f2; goto _LL303; _LL303: _temp302=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp116)->f3; goto _LL191;} else{ goto _LL192;} _LL192: if(*(( int*) _temp116)
== Cyc_Absyn_Var_e){ _LL309: _temp308=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp116)->f2; if( _temp308 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL193;}
else{ goto _LL194;}} else{ goto _LL194;} _LL194: if(*(( int*) _temp116) == Cyc_Absyn_UnknownId_e){
goto _LL195;} else{ goto _LL196;} _LL196: if(*(( int*) _temp116) == Cyc_Absyn_UnknownCall_e){
goto _LL197;} else{ goto _LL198;} _LL198: if(*(( int*) _temp116) == Cyc_Absyn_UnresolvedMem_e){
goto _LL199;} else{ goto _LL200;} _LL200: if(*(( int*) _temp116) == Cyc_Absyn_CompoundLit_e){
goto _LL201;} else{ goto _LL202;} _LL202: if(*(( int*) _temp116) == Cyc_Absyn_Codegen_e){
goto _LL203;} else{ goto _LL204;} _LL204: if(*(( int*) _temp116) == Cyc_Absyn_Fill_e){
goto _LL205;} else{ goto _LL117;} _LL119: _temp208= _temp206; goto _LL121;
_LL121: return Cyc_NewControlFlow_abstract_exp( env, _temp208); _LL123: env.access_path
== 0? 0:(( int(*)( struct _tagged_arr msg)) Cyc_Assert_AssertFail)( _tag_arr("new_control_flow.cyc:303 env.access_path==null",
sizeof( unsigned char), 47u)); return({ struct Cyc_NewControlFlow_AbsSyn
_temp310; _temp310.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp312;
_temp312.inner_exp=( void*) Cyc_CfAbsexp_mkBottomAE(); _temp312.assigns=( void*)
Cyc_CfAbsexp_mkBottomAE(); _temp312;}); _temp310.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp311; _temp311.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp311.assigns=(
void*) Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
_temp311;}); _temp310.lvalues= 0; _temp310;}); _LL125: env.access_path == 0? 0:((
int(*)( struct _tagged_arr msg)) Cyc_Assert_AssertFail)( _tag_arr("new_control_flow.cyc:308 env.access_path==null",
sizeof( unsigned char), 47u)); return({ struct Cyc_NewControlFlow_AbsSyn
_temp313; _temp313.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp315;
_temp315.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp315.assigns=( void*)
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()); _temp315;});
_temp313.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp314; _temp314.inner_exp=(
void*) Cyc_CfAbsexp_mkBottomAE(); _temp314.assigns=( void*) Cyc_CfAbsexp_mkBottomAE();
_temp314;}); _temp313.lvalues= 0; _temp313;}); _LL127: goto _LL129; _LL129: goto
_LL131; _LL131: goto _LL133; _LL133: goto _LL135; _LL135: goto _LL137; _LL137:
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp())); _LL139: return Cyc_NewControlFlow_mkSyn_tf(
Cyc_CfAbsexp_mkSkipAE(), Cyc_NewControlFlow_use_it_list(), Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp())); _LL141: _temp232= _temp228; goto _LL143;
_LL143: _temp236= _temp232; goto _LL145; _LL145: { void* absop= Cyc_NewControlFlow_destruct_path(
env, Cyc_CfAbsexp_mkLocalOp( _temp236)); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),({
struct Cyc_List_List* _temp316=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp316->hd=( void*) absop; _temp316->tl= 0; _temp316;}),
Cyc_NewControlFlow_make_assigns( env, absop));} _LL147: { struct Cyc_NewControlFlow_AbsSyn
_temp317= Cyc_NewControlFlow_abstract_exp( env, _temp242); struct Cyc_List_List*
_temp318= _temp317.lvalues == 0? Cyc_NewControlFlow_use_it_list(): _temp317.lvalues;
struct Cyc_NewControlFlow_AbsSyn _temp319= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs(
env, _temp318), _temp238); void* _temp322; void* _temp324; struct Cyc_NewControlFlow_AbsSynOne
_temp320= Cyc_NewControlFlow_meet_absexp( _temp317); _LL325: _temp324=( void*)
_temp320.inner_exp; goto _LL323; _LL323: _temp322=( void*) _temp320.assigns;
goto _LL321; _LL321: { void* _temp328; void* _temp330; struct Cyc_NewControlFlow_AbsSynOne
_temp326= Cyc_NewControlFlow_meet_absexp( _temp319); _LL331: _temp330=( void*)
_temp326.inner_exp; goto _LL329; _LL329: _temp328=( void*) _temp326.assigns;
goto _LL327; _LL327: { void* _temp332= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG, _temp324, _temp330),
_temp328); return Cyc_NewControlFlow_mkSyn_tf( _temp332, 0, _temp322);}}} _LL149:
return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp333=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp333->hd=( void*) _temp246; _temp333->tl=({ struct Cyc_List_List* _temp334=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp334->hd=(
void*) _temp244; _temp334->tl= 0; _temp334;}); _temp333;}), 0); _LL151: return
Cyc_NewControlFlow_use_exp_unordered( env, _temp248, 0); _LL153: { int both_leaf=
1;{ void* _temp335=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp250->topt))->v;
_LL337: if(( unsigned int) _temp335 > 4u?*(( int*) _temp335) == Cyc_Absyn_AnonStructType:
0){ goto _LL338;} else{ goto _LL339;} _LL339: if(( unsigned int) _temp335 > 4u?*((
int*) _temp335) == Cyc_Absyn_StructType: 0){ goto _LL340;} else{ goto _LL341;}
_LL341: if(( unsigned int) _temp335 > 4u?*(( int*) _temp335) == Cyc_Absyn_TupleType:
0){ goto _LL342;} else{ goto _LL343;} _LL343: goto _LL344; _LL338: goto _LL340;
_LL340: goto _LL342; _LL342: both_leaf= 0; goto _LL336; _LL344: goto _LL336;
_LL336:;}{ void* _temp345=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v;
_LL347: if(( unsigned int) _temp345 > 4u?*(( int*) _temp345) == Cyc_Absyn_AnonStructType:
0){ goto _LL348;} else{ goto _LL349;} _LL349: if(( unsigned int) _temp345 > 4u?*((
int*) _temp345) == Cyc_Absyn_StructType: 0){ goto _LL350;} else{ goto _LL351;}
_LL351: if(( unsigned int) _temp345 > 4u?*(( int*) _temp345) == Cyc_Absyn_TupleType:
0){ goto _LL352;} else{ goto _LL353;} _LL353: goto _LL354; _LL348: goto _LL350;
_LL350: goto _LL352; _LL352: both_leaf= 0; goto _LL346; _LL354: goto _LL346;
_LL346:;} if( both_leaf){ return Cyc_NewControlFlow_abstract_exp( env, _temp250);}
return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp355=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp355->hd=( void*) _temp250; _temp355->tl= 0; _temp355;}), env.lhsides);}
_LL155: return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp356=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp356->hd=( void*) _temp254; _temp356->tl= _temp252; _temp356;}), 0); _LL157: {
void* _temp357= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp258->topt))->v); _LL359: if(( unsigned int) _temp357 > 4u?*(( int*)
_temp357) == Cyc_Absyn_TupleType: 0){ goto _LL360;} else{ goto _LL361;} _LL361:
goto _LL362; _LL360: return Cyc_NewControlFlow_descend_path( env, e, _temp258);
_LL362: return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp363=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp363->hd=( void*) _temp258; _temp363->tl=({ struct Cyc_List_List* _temp364=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp364->hd=(
void*) _temp256; _temp364->tl= 0; _temp364;}); _temp363;}), Cyc_NewControlFlow_use_it_list());
_LL358:;} _LL159: { int is_malloc= 0;{ void* _temp365=( void*) _temp260->r;
_LL367: if(*(( int*) _temp365) == Cyc_Absyn_Struct_e){ goto _LL368;} else{ goto
_LL369;} _LL369: if(*(( int*) _temp365) == Cyc_Absyn_Tuple_e){ goto _LL370;}
else{ goto _LL371;} _LL371: goto _LL372; _LL368: goto _LL370; _LL370: is_malloc=
1; goto _LL366; _LL372: goto _LL366; _LL366:;} if( is_malloc){ _temp264= 0;
_temp262= _temp260; goto _LL161;}{ struct Cyc_List_List* _temp373= env.access_path;
struct Cyc_List_List _temp385; struct Cyc_List_List* _temp386; struct Cyc_Absyn_Exp*
_temp388; struct Cyc_Absyn_Exp _temp390; void* _temp391; struct Cyc_Absyn_Exp*
_temp393; struct Cyc_List_List _temp395; struct Cyc_List_List* _temp396; struct
Cyc_Absyn_Exp* _temp398; struct Cyc_Absyn_Exp _temp400; void* _temp401; struct
Cyc_List_List _temp403; struct Cyc_List_List* _temp404; struct Cyc_Absyn_Exp*
_temp406; struct Cyc_Absyn_Exp _temp408; void* _temp409; struct _tagged_arr*
_temp411; struct Cyc_Absyn_Exp* _temp413; _LL375: if( _temp373 == 0){ goto
_LL376;} else{ goto _LL377;} _LL377: if( _temp373 == 0){ goto _LL379;} else{
_temp385=* _temp373; _LL389: _temp388=( struct Cyc_Absyn_Exp*) _temp385.hd;
_temp390=* _temp388; _LL392: _temp391=( void*) _temp390.r; if(*(( int*) _temp391)
== Cyc_Absyn_Subscript_e){ _LL394: _temp393=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp391)->f2; goto _LL387;} else{ goto _LL379;} _LL387: _temp386= _temp385.tl;
goto _LL378;} _LL379: if( _temp373 == 0){ goto _LL381;} else{ _temp395=*
_temp373; _LL399: _temp398=( struct Cyc_Absyn_Exp*) _temp395.hd; _temp400=*
_temp398; _LL402: _temp401=( void*) _temp400.r; if(*(( int*) _temp401) == Cyc_Absyn_Deref_e){
goto _LL397;} else{ goto _LL381;} _LL397: _temp396= _temp395.tl; goto _LL380;}
_LL381: if( _temp373 == 0){ goto _LL383;} else{ _temp403=* _temp373; _LL407:
_temp406=( struct Cyc_Absyn_Exp*) _temp403.hd; _temp408=* _temp406; _LL410:
_temp409=( void*) _temp408.r; if(*(( int*) _temp409) == Cyc_Absyn_StructArrow_e){
_LL414: _temp413=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp409)->f1; goto
_LL412; _LL412: _temp411=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp409)->f2;
goto _LL405;} else{ goto _LL383;} _LL405: _temp404= _temp403.tl; goto _LL382;}
_LL383: goto _LL384; _LL376: env.access_path=({ struct Cyc_List_List* _temp415=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp415->hd=(
void*) e; _temp415->tl= env.access_path; _temp415;}); goto _LL374; _LL378: Cyc_Evexp_eval_const_uint_exp(
_temp393) == 0? 0:(( int(*)( struct _tagged_arr msg)) Cyc_Assert_AssertFail)(
_tag_arr("new_control_flow.cyc:432 Evexp::eval_const_uint_exp(e2) == 0", sizeof(
unsigned char), 61u)); _temp396= _temp386; goto _LL380; _LL380: env.access_path=
_temp396; goto _LL374; _LL382: env.access_path=({ struct Cyc_List_List* _temp416=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp416->hd=(
void*) Cyc_Absyn_structmember_exp( _temp413, _temp411, 0); _temp416->tl=
_temp404; _temp416;}); goto _LL374; _LL384:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp417=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp417[ 0]=({ struct Cyc_Core_Impossible_struct _temp418; _temp418.tag= Cyc_Core_Impossible;
_temp418.f1= _tag_arr("abstract_exp: bad access path for &", sizeof(
unsigned char), 36u); _temp418;}); _temp417;})); _LL374:;} return Cyc_NewControlFlow_abstract_exp(
env, _temp260);} _LL161: Cyc_NewControlFlow_add_malloc_root( env, e,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp262->topt))->v); return Cyc_NewControlFlow_abstract_malloc(
env, e, _temp264,( struct Cyc_Absyn_Exp*) _temp262); _LL163: Cyc_NewControlFlow_add_malloc_root(
env, e, _temp266); return Cyc_NewControlFlow_abstract_malloc( env, e, _temp268,
0); _LL165: if( Cyc_Tcutil_is_tagged_pointer_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp270->topt))->v)){ return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp419=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp419->hd=( void*) _temp270; _temp419->tl= 0;
_temp419;}), Cyc_NewControlFlow_use_it_list());}{ struct Cyc_List_List* _temp420=
env.access_path; struct Cyc_List_List _temp426; struct Cyc_List_List* _temp427;
struct Cyc_Absyn_Exp* _temp429; struct Cyc_Absyn_Exp _temp431; void* _temp432;
_LL422: if( _temp420 == 0){ goto _LL424;} else{ _temp426=* _temp420; _LL430:
_temp429=( struct Cyc_Absyn_Exp*) _temp426.hd; _temp431=* _temp429; _LL433:
_temp432=( void*) _temp431.r; if(*(( int*) _temp432) == Cyc_Absyn_Address_e){
goto _LL428;} else{ goto _LL424;} _LL428: _temp427= _temp426.tl; goto _LL423;}
_LL424: goto _LL425; _LL423: env.access_path= _temp427;{ struct Cyc_NewControlFlow_AbsSyn
_temp434= Cyc_NewControlFlow_abstract_exp( env, _temp270); if( _temp434.lvalues
== 0){ _temp434.lvalues= Cyc_NewControlFlow_use_it_list();} return _temp434;}
_LL425: return Cyc_NewControlFlow_descend_path( env, e, _temp270); _LL421:;}
_LL167: if( Cyc_Tcutil_is_tagged_pointer_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp272->topt))->v)){ return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp435=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp435->hd=( void*) _temp272; _temp435->tl= 0;
_temp435;}), Cyc_NewControlFlow_use_it_list());}{ void* _temp436= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp272->topt))->v); struct Cyc_Absyn_PtrInfo
_temp442; void* _temp444; _LL438: if(( unsigned int) _temp436 > 4u?*(( int*)
_temp436) == Cyc_Absyn_PointerType: 0){ _LL443: _temp442=(( struct Cyc_Absyn_PointerType_struct*)
_temp436)->f1; _LL445: _temp444=( void*) _temp442.elt_typ; goto _LL439;} else{
goto _LL440;} _LL440: goto _LL441; _LL439:{ void* _temp446= Cyc_Tcutil_compress(
_temp444); _LL448: if(( unsigned int) _temp446 > 4u?*(( int*) _temp446) == Cyc_Absyn_AnonStructType:
0){ goto _LL449;} else{ goto _LL450;} _LL450: if(( unsigned int) _temp446 > 4u?*((
int*) _temp446) == Cyc_Absyn_StructType: 0){ goto _LL451;} else{ goto _LL452;}
_LL452: if(( unsigned int) _temp446 > 4u?*(( int*) _temp446) == Cyc_Absyn_AnonUnionType:
0){ goto _LL453;} else{ goto _LL454;} _LL454: if(( unsigned int) _temp446 > 4u?*((
int*) _temp446) == Cyc_Absyn_UnionType: 0){ goto _LL455;} else{ goto _LL456;}
_LL456: goto _LL457; _LL449: goto _LL451; _LL451: return Cyc_NewControlFlow_descend_path(
env, e, _temp272); _LL453: goto _LL455; _LL455: { struct Cyc_NewControlFlow_AbsSyn
_temp458= Cyc_NewControlFlow_abstract_exp( env, _temp272); _temp458.lvalues= Cyc_NewControlFlow_use_it_list();
return _temp458;} _LL457: goto _LL447; _LL447:;} goto _LL441; _LL441:( int)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp459=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp459[ 0]=({ struct
Cyc_Core_Impossible_struct _temp460; _temp460.tag= Cyc_Core_Impossible; _temp460.f1=
_tag_arr("NewControlFlow: bad type in StructArrow", sizeof( unsigned char), 40u);
_temp460;}); _temp459;})); _LL437:;} _LL169: { void* _temp461= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp274->topt))->v); _LL463: if((
unsigned int) _temp461 > 4u?*(( int*) _temp461) == Cyc_Absyn_AnonStructType: 0){
goto _LL464;} else{ goto _LL465;} _LL465: if(( unsigned int) _temp461 > 4u?*((
int*) _temp461) == Cyc_Absyn_StructType: 0){ goto _LL466;} else{ goto _LL467;}
_LL467: if(( unsigned int) _temp461 > 4u?*(( int*) _temp461) == Cyc_Absyn_AnonUnionType:
0){ goto _LL468;} else{ goto _LL469;} _LL469: if(( unsigned int) _temp461 > 4u?*((
int*) _temp461) == Cyc_Absyn_UnionType: 0){ goto _LL470;} else{ goto _LL471;}
_LL471: goto _LL472; _LL464: goto _LL466; _LL466: return Cyc_NewControlFlow_descend_path(
env, e, _temp274); _LL468: goto _LL470; _LL470: { struct Cyc_NewControlFlow_AbsSyn
_temp473= Cyc_NewControlFlow_abstract_exp( env, _temp274); _temp473.lvalues= Cyc_NewControlFlow_use_it_list();
return _temp473;} _LL472:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp474=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp474[ 0]=({ struct Cyc_Core_Impossible_struct _temp475; _temp475.tag= Cyc_Core_Impossible;
_temp475.f1= _tag_arr("NewControlFlow: bad type in StructMember", sizeof(
unsigned char), 41u); _temp475;}); _temp474;})); _LL462:;} _LL171: env.access_path
== 0? 0:(( int(*)( struct _tagged_arr msg)) Cyc_Assert_AssertFail)( _tag_arr("new_control_flow.cyc:506 env.access_path == null",
sizeof( unsigned char), 49u)); if( _temp276 == 0){ return Cyc_NewControlFlow_mkSyn_tf(
Cyc_CfAbsexp_mkSkipAE(), Cyc_NewControlFlow_use_it_list(), Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} _temp278= _temp276; goto _LL173; _LL173: {
struct Cyc_List_List* _temp476= env.access_path; struct Cyc_List_List _temp484;
struct Cyc_List_List* _temp485; struct Cyc_Absyn_Exp* _temp487; struct Cyc_Absyn_Exp
_temp489; void* _temp490; struct Cyc_Absyn_Exp* _temp492; _LL478: if( _temp476
== 0){ goto _LL479;} else{ goto _LL480;} _LL480: if( _temp476 == 0){ goto _LL482;}
else{ _temp484=* _temp476; _LL488: _temp487=( struct Cyc_Absyn_Exp*) _temp484.hd;
_temp489=* _temp487; _LL491: _temp490=( void*) _temp489.r; if(*(( int*) _temp490)
== Cyc_Absyn_Subscript_e){ _LL493: _temp492=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp490)->f2; goto _LL486;} else{ goto _LL482;} _LL486: _temp485= _temp484.tl;
goto _LL481;} _LL482: goto _LL483; _LL479: { void* _temp494= Cyc_CfAbsexp_mkSkipAE();
void* _temp495= Cyc_CfAbsexp_mkSkipAE();{ int j= 0; for( 0; _temp278 != 0;(
_temp278=(( struct Cyc_List_List*) _check_null( _temp278))->tl, ++ j)){ struct
Cyc_List_List* _temp496= 0;{ struct Cyc_List_List* ls= env.lhsides; for( 0; ls
!= 0; ls=(( struct Cyc_List_List*) _check_null( ls))->tl){ if( Cyc_CfAbsexp_isUnknownOp((
void*)(( struct Cyc_List_List*) _check_null( ls))->hd)){ _temp496=({ struct Cyc_List_List*
_temp497=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp497->hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp497->tl= _temp496;
_temp497;});} else{ _temp496=({ struct Cyc_List_List* _temp498=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp498->hd=( void*) Cyc_CfAbsexp_mkMemberOp((
void*)(( struct Cyc_List_List*) _check_null( ls))->hd,( void*)({ struct Cyc_CfFlowInfo_TupleF_struct*
_temp499=( struct Cyc_CfFlowInfo_TupleF_struct*) GC_malloc_atomic( sizeof(
struct Cyc_CfFlowInfo_TupleF_struct)); _temp499[ 0]=({ struct Cyc_CfFlowInfo_TupleF_struct
_temp500; _temp500.tag= Cyc_CfFlowInfo_TupleF; _temp500.f1= j; _temp500;});
_temp499;})); _temp498->tl= _temp496; _temp498;});}}}{ struct Cyc_NewControlFlow_AbsSyn
_temp501= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs( env,
_temp496),( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp278))->hd); void* _temp504; void* _temp506; struct Cyc_NewControlFlow_AbsSynOne
_temp502= Cyc_NewControlFlow_meet_absexp( _temp501); _LL507: _temp506=( void*)
_temp502.inner_exp; goto _LL505; _LL505: _temp504=( void*) _temp502.assigns;
goto _LL503; _LL503: _temp494= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp494, _temp506); _temp495= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
_temp495, _temp504);}}} return Cyc_NewControlFlow_mkSyn_tf( _temp494, 0,
_temp495);} _LL481: { int i=( int) Cyc_Evexp_eval_const_uint_exp( _temp492);
struct Cyc_NewControlFlow_AbsEnv _temp508= Cyc_NewControlFlow_change_lhs( env, 0);
_temp508.access_path= 0; env.access_path= _temp485;{ void* _temp509= Cyc_CfAbsexp_mkSkipAE();
void* _temp510= Cyc_CfAbsexp_mkSkipAE();{ int j= 0; for( 0; _temp278 != 0;(
_temp278=(( struct Cyc_List_List*) _check_null( _temp278))->tl, ++ j)){ if( i ==
j){ void* _temp513; void* _temp515; struct Cyc_NewControlFlow_AbsSynOne _temp511=
Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp278))->hd)); _LL516:
_temp515=( void*) _temp511.inner_exp; goto _LL514; _LL514: _temp513=( void*)
_temp511.assigns; goto _LL512; _LL512: _temp510= _temp513; _temp509= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, _temp509, _temp515);} else{ void* _temp519; void*
_temp521; struct Cyc_NewControlFlow_AbsSynOne _temp517= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( _temp508,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp278))->hd)); _LL522: _temp521=( void*) _temp517.inner_exp;
goto _LL520; _LL520: _temp519=( void*) _temp517.assigns; goto _LL518; _LL518:
_temp509= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG, _temp509,
_temp521);}}} return Cyc_NewControlFlow_mkSyn_tf( _temp509,({ struct Cyc_List_List*
_temp523=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp523->hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp523->tl= 0; _temp523;}),
_temp510);}} _LL483:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp524=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp524[ 0]=({ struct Cyc_Core_Impossible_struct _temp525; _temp525.tag= Cyc_Core_Impossible;
_temp525.f1= _tag_arr("abstract_exp: unexpected access path to Tuple", sizeof(
unsigned char), 46u); _temp525;}); _temp524;})); _LL477:;} _LL175: _temp282=
_temp280; goto _LL177; _LL177: { struct Cyc_List_List* _temp526= env.access_path;
struct Cyc_List_List _temp534; struct Cyc_List_List* _temp535; struct Cyc_Absyn_Exp*
_temp537; struct Cyc_Absyn_Exp _temp539; void* _temp540; struct _tagged_arr*
_temp542; _LL528: if( _temp526 == 0){ goto _LL529;} else{ goto _LL530;} _LL530:
if( _temp526 == 0){ goto _LL532;} else{ _temp534=* _temp526; _LL538: _temp537=(
struct Cyc_Absyn_Exp*) _temp534.hd; _temp539=* _temp537; _LL541: _temp540=( void*)
_temp539.r; if(*(( int*) _temp540) == Cyc_Absyn_StructMember_e){ _LL543:
_temp542=(( struct Cyc_Absyn_StructMember_e_struct*) _temp540)->f2; goto _LL536;}
else{ goto _LL532;} _LL536: _temp535= _temp534.tl; goto _LL531;} _LL532: goto
_LL533; _LL529: { void* _temp544= Cyc_CfAbsexp_mkSkipAE(); void* _temp545= Cyc_CfAbsexp_mkSkipAE();
for( 0; _temp282 != 0; _temp282=(( struct Cyc_List_List*) _check_null( _temp282))->tl){
struct Cyc_List_List* _temp546= 0;{ struct Cyc_List_List* ls= env.lhsides; for(
0; ls != 0; ls=(( struct Cyc_List_List*) _check_null( ls))->tl){ if( Cyc_CfAbsexp_isUnknownOp((
void*)(( struct Cyc_List_List*) _check_null( ls))->hd)){ _temp546=({ struct Cyc_List_List*
_temp547=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp547->hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp547->tl= _temp546;
_temp547;});} else{ struct Cyc_List_List* ds=(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp282))->hd)).f1; for( 0; ds != 0; ds=(( struct Cyc_List_List*)
_check_null( ds))->tl){ void* _temp548=( void*)(( struct Cyc_List_List*)
_check_null( ds))->hd; struct _tagged_arr* _temp554; _LL550: if(*(( int*)
_temp548) == Cyc_Absyn_ArrayElement){ goto _LL551;} else{ goto _LL552;} _LL552:
if(*(( int*) _temp548) == Cyc_Absyn_FieldName){ _LL555: _temp554=(( struct Cyc_Absyn_FieldName_struct*)
_temp548)->f1; goto _LL553;} else{ goto _LL549;} _LL551:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp556=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp556[ 0]=({ struct
Cyc_Core_Impossible_struct _temp557; _temp557.tag= Cyc_Core_Impossible; _temp557.f1=
_tag_arr("bad struct designator", sizeof( unsigned char), 22u); _temp557;});
_temp556;})); _LL553: _temp546=({ struct Cyc_List_List* _temp558=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp558->hd=( void*) Cyc_CfAbsexp_mkMemberOp((
void*)(( struct Cyc_List_List*) _check_null( ls))->hd,( void*)({ struct Cyc_CfFlowInfo_StructF_struct*
_temp559=( struct Cyc_CfFlowInfo_StructF_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct));
_temp559[ 0]=({ struct Cyc_CfFlowInfo_StructF_struct _temp560; _temp560.tag= Cyc_CfFlowInfo_StructF;
_temp560.f1= _temp554; _temp560;}); _temp559;})); _temp558->tl= _temp546;
_temp558;}); goto _LL549; _LL549:;}}}}{ void** _temp561=( void**)(( void*)&((
struct Cyc_Core_Opt*) _check_null(((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp282))->hd)).f2)->topt))->v); struct Cyc_NewControlFlow_AbsSyn
_temp562= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs( env,
_temp546),(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp282))->hd)).f2);
void* _temp565; void* _temp567; struct Cyc_NewControlFlow_AbsSynOne _temp563=
Cyc_NewControlFlow_meet_absexp( _temp562); _LL568: _temp567=( void*) _temp563.inner_exp;
goto _LL566; _LL566: _temp565=( void*) _temp563.assigns; goto _LL564; _LL564:
_temp544= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG, _temp544,
_temp567); _temp545= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
_temp545, _temp565);}} return Cyc_NewControlFlow_mkSyn_tf( _temp544, 0, _temp545);}
_LL531: { struct Cyc_NewControlFlow_AbsEnv _temp569= Cyc_NewControlFlow_change_lhs(
env, 0); _temp569.access_path= 0; env.access_path= _temp535;{ void* _temp570=
Cyc_CfAbsexp_mkSkipAE(); void* _temp571= Cyc_CfAbsexp_mkSkipAE(); for( 0;
_temp282 != 0; _temp282=(( struct Cyc_List_List*) _check_null( _temp282))->tl){
int used= 0;{ struct Cyc_List_List* ds=(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp282))->hd)).f1; for( 0; ds != 0; ds=(( struct Cyc_List_List*)
_check_null( ds))->tl){ void* _temp572=( void*)(( struct Cyc_List_List*)
_check_null( ds))->hd; struct _tagged_arr* _temp578; _LL574: if(*(( int*)
_temp572) == Cyc_Absyn_ArrayElement){ goto _LL575;} else{ goto _LL576;} _LL576:
if(*(( int*) _temp572) == Cyc_Absyn_FieldName){ _LL579: _temp578=(( struct Cyc_Absyn_FieldName_struct*)
_temp572)->f1; goto _LL577;} else{ goto _LL573;} _LL575:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp580=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp580[ 0]=({ struct
Cyc_Core_Impossible_struct _temp581; _temp581.tag= Cyc_Core_Impossible; _temp581.f1=
_tag_arr("bad struct designator", sizeof( unsigned char), 22u); _temp581;});
_temp580;})); _LL577: if( Cyc_String_zstrptrcmp( _temp542, _temp578) == 0){ used=
1;} goto _LL573; _LL573:;}} if( used){ void* _temp584; void* _temp586; struct
Cyc_NewControlFlow_AbsSynOne _temp582= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
env,(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp282))->hd)).f2));
_LL587: _temp586=( void*) _temp582.inner_exp; goto _LL585; _LL585: _temp584=(
void*) _temp582.assigns; goto _LL583; _LL583: _temp571= _temp584; _temp570= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, _temp570, _temp586);} else{ void* _temp590; void*
_temp592; struct Cyc_NewControlFlow_AbsSynOne _temp588= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( _temp569,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp282))->hd)).f2)); _LL593: _temp592=( void*) _temp588.inner_exp;
goto _LL591; _LL591: _temp590=( void*) _temp588.assigns; goto _LL589; _LL589:
_temp570= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG, _temp570,
_temp592);}} return Cyc_NewControlFlow_mkSyn_tf( _temp570, 0, _temp571);}}
_LL533:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp594=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp594[ 0]=({ struct Cyc_Core_Impossible_struct _temp595; _temp595.tag= Cyc_Core_Impossible;
_temp595.f1= _tag_arr("abstract_exp: unexpected access path to Struct", sizeof(
unsigned char), 47u); _temp595;}); _temp594;})); _LL527:;} _LL179: return Cyc_NewControlFlow_use_exp_unordered(
env,(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)( struct
_tuple4*)) Cyc_Core_snd, _temp284), 0); _LL181: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp596=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp596->hd=( void*) _temp286; _temp596->tl= 0;
_temp596;}), 0); _LL183: { struct Cyc_NewControlFlow_AbsSyn _temp597= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp288); return Cyc_NewControlFlow_mkSyn_tf(
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)( Cyc_NewControlFlow_meet_absexp(
_temp597)).inner_exp, Cyc_CfAbsexp_mkBottomAE()), 0, Cyc_CfAbsexp_mkSkipAE());}
_LL185: { struct Cyc_NewControlFlow_AbsSynOne _temp600; struct Cyc_NewControlFlow_AbsSynOne
_temp602; struct Cyc_NewControlFlow_AbsSyn _temp598= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp294); _LL603: _temp602= _temp598.when_true;
goto _LL601; _LL601: _temp600= _temp598.when_false; goto _LL599; _LL599: {
struct Cyc_NewControlFlow_AbsSynOne _temp606; struct Cyc_NewControlFlow_AbsSynOne
_temp608; struct Cyc_NewControlFlow_AbsSyn _temp604= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp292); _LL609: _temp608= _temp604.when_true;
goto _LL607; _LL607: _temp606= _temp604.when_false; goto _LL605; _LL605: {
struct Cyc_NewControlFlow_AbsSynOne _temp612; struct Cyc_NewControlFlow_AbsSynOne
_temp614; struct Cyc_NewControlFlow_AbsSyn _temp610= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp290); _LL615: _temp614= _temp610.when_true;
goto _LL613; _LL613: _temp612= _temp610.when_false; goto _LL611; _LL611: { void*
_temp616= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp602.inner_exp,( void*) _temp602.assigns), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp608.inner_exp,( void*) _temp608.assigns)),
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp600.inner_exp,( void*) _temp600.assigns),
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp614.inner_exp,(
void*) _temp614.assigns))); void* _temp617= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp602.inner_exp,( void*) _temp602.assigns),
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp606.inner_exp,(
void*) _temp606.assigns)), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp600.inner_exp,(
void*) _temp600.assigns), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp612.inner_exp,( void*) _temp612.assigns))); return({ struct Cyc_NewControlFlow_AbsSyn
_temp618; _temp618.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp620;
_temp620.inner_exp=( void*) _temp616; _temp620.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp620;}); _temp618.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp619; _temp619.inner_exp=( void*) _temp617; _temp619.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp619;}); _temp618.lvalues= 0; _temp618;});}}}}
_LL187: { struct Cyc_NewControlFlow_AbsEnv _temp621= Cyc_NewControlFlow_change_lhs(
env, 0); _temp621.access_path= 0; Cyc_NewControlFlow_cf_prepass( _temp621,
_temp296);{ struct Cyc_Absyn_Stmt* _temp622= _temp296; while( 1) { void*
_temp623=( void*) _temp622->r; struct Cyc_Absyn_Stmt* _temp633; struct Cyc_Absyn_Stmt*
_temp635; struct Cyc_Absyn_Stmt* _temp637; struct Cyc_Absyn_Decl* _temp639;
struct Cyc_Absyn_Exp* _temp641; _LL625: if(( unsigned int) _temp623 > 1u?*(( int*)
_temp623) == Cyc_Absyn_Seq_s: 0){ _LL636: _temp635=(( struct Cyc_Absyn_Seq_s_struct*)
_temp623)->f1; goto _LL634; _LL634: _temp633=(( struct Cyc_Absyn_Seq_s_struct*)
_temp623)->f2; goto _LL626;} else{ goto _LL627;} _LL627: if(( unsigned int)
_temp623 > 1u?*(( int*) _temp623) == Cyc_Absyn_Decl_s: 0){ _LL640: _temp639=((
struct Cyc_Absyn_Decl_s_struct*) _temp623)->f1; goto _LL638; _LL638: _temp637=((
struct Cyc_Absyn_Decl_s_struct*) _temp623)->f2; goto _LL628;} else{ goto _LL629;}
_LL629: if(( unsigned int) _temp623 > 1u?*(( int*) _temp623) == Cyc_Absyn_Exp_s:
0){ _LL642: _temp641=(( struct Cyc_Absyn_Exp_s_struct*) _temp623)->f1; goto
_LL630;} else{ goto _LL631;} _LL631: goto _LL632; _LL626: _temp622= _temp633;
continue; _LL628: _temp622= _temp637; continue; _LL630: { struct Cyc_NewControlFlow_AbsSyn
_temp643= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp641); void* _temp646; void* _temp648; struct Cyc_NewControlFlow_AbsSynOne
_temp644= Cyc_NewControlFlow_meet_absexp( _temp643); _LL649: _temp648=( void*)
_temp644.inner_exp; goto _LL647; _LL647: _temp646=( void*) _temp644.assigns;
goto _LL645; _LL645:( void*)((( struct Cyc_List_List*) _check_null(( Cyc_NewControlFlow_get_stmt_annot(
_temp622))->absexps))->hd=( void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp648, _temp646)); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkStmtAE(
_temp296), 0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()));}
_LL632:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp650=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp650[ 0]=({ struct Cyc_Core_Impossible_struct _temp651; _temp651.tag= Cyc_Core_Impossible;
_temp651.f1= _tag_arr("abstract_exp: ill-formed StmtExp", sizeof( unsigned char),
33u); _temp651;}); _temp650;})); _LL624:;}}} _LL189: { struct Cyc_NewControlFlow_AbsEnv
_temp652= Cyc_NewControlFlow_change_lhs( env, 0); _temp652.access_path= 0;{
struct Cyc_NewControlFlow_AbsSyn _temp653= Cyc_NewControlFlow_abstract_exp(
_temp652, _temp300); struct Cyc_NewControlFlow_AbsSyn _temp654= Cyc_NewControlFlow_abstract_exp(
env, _temp298); struct Cyc_NewControlFlow_AbsSynOne _temp655= Cyc_NewControlFlow_meet_absexp(
_temp653); return({ struct Cyc_NewControlFlow_AbsSyn _temp656; _temp656.when_true=({
struct Cyc_NewControlFlow_AbsSynOne _temp658; _temp658.inner_exp=( void*) Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp655.inner_exp,( void*)( _temp654.when_true).inner_exp);
_temp658.assigns=( void*)(( void*)( _temp654.when_true).assigns); _temp658;});
_temp656.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp657; _temp657.inner_exp=(
void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp655.inner_exp,(
void*)( _temp654.when_false).inner_exp); _temp657.assigns=( void*)(( void*)(
_temp654.when_false).assigns); _temp657;}); _temp656.lvalues= 0; _temp656;});}}
_LL191: env.access_path == 0? 0:(( int(*)( struct _tagged_arr msg)) Cyc_Assert_AssertFail)(
_tag_arr("new_control_flow.cyc:726 env.access_path==null", sizeof( unsigned char),
47u)); Cyc_NewControlFlow_add_var_root( env, _temp306);( env.shared)->comprehension_vars=({
struct Cyc_List_List* _temp659=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp659->hd=( void*) _temp306; _temp659->tl=( env.shared)->comprehension_vars;
_temp659;});{ struct Cyc_NewControlFlow_AbsEnv _temp660= Cyc_NewControlFlow_use_it_env(
env); void* _temp663; void* _temp665; struct Cyc_NewControlFlow_AbsSynOne
_temp661= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
_temp660, _temp304)); _LL666: _temp665=( void*) _temp661.inner_exp; goto _LL664;
_LL664: _temp663=( void*) _temp661.assigns; goto _LL662; _LL662: { void*
_temp669; void* _temp671; struct Cyc_NewControlFlow_AbsSynOne _temp667= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( _temp660, _temp302)); _LL672: _temp671=( void*)
_temp667.inner_exp; goto _LL670; _LL670: _temp669=( void*) _temp667.assigns;
goto _LL668; _LL668: { void* _temp673= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp665, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp663, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkAssignAE( Cyc_CfAbsexp_mkLocalOp(
_temp306), Cyc_CfAbsexp_mkUnknownOp()), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp671, _temp669), Cyc_CfAbsexp_mkSkipAE()))));
return Cyc_NewControlFlow_mkSyn_tf( _temp673, 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));}}} _LL193: goto _LL195; _LL195: goto _LL197;
_LL197: goto _LL199; _LL199: goto _LL201; _LL201: goto _LL203; _LL203: goto
_LL205; _LL205:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp674=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp674[ 0]=({ struct Cyc_Core_Impossible_struct _temp675; _temp675.tag= Cyc_Core_Impossible;
_temp675.f1= _tag_arr("abstract_exp, unexpected exp form", sizeof( unsigned char),
34u); _temp675;}); _temp674;})); _LL117:;} void* Cyc_NewControlFlow_abstract_exp_top(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ void* _temp678;
void* _temp680; struct Cyc_NewControlFlow_AbsSynOne _temp676= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( env, e)); _LL681: _temp680=( void*) _temp676.inner_exp;
goto _LL679; _LL679: _temp678=( void*) _temp676.assigns; goto _LL677; _LL677:
return Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp680, _temp678);}
struct _tuple0 Cyc_NewControlFlow_abstract_guard( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e){ struct Cyc_NewControlFlow_AbsSyn _temp682= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), e); return({ struct _tuple0 _temp683;
_temp683.f1= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)(
_temp682.when_true).inner_exp,( void*)( _temp682.when_true).assigns); _temp683.f2=
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)( _temp682.when_false).inner_exp,(
void*)( _temp682.when_false).assigns); _temp683;});} struct Cyc_List_List* Cyc_NewControlFlow_cf_prepass_scs(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_List_List* scs){ struct Cyc_List_List*
_temp684= 0; for( 0; scs != 0; scs=(( struct Cyc_List_List*) _check_null( scs))->tl){
struct Cyc_Absyn_Switch_clause _temp687; struct Cyc_Absyn_Stmt* _temp688; struct
Cyc_Absyn_Exp* _temp690; struct Cyc_Core_Opt* _temp692; struct Cyc_Absyn_Switch_clause*
_temp685=( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
scs))->hd; _temp687=* _temp685; _LL693: _temp692= _temp687.pat_vars; goto _LL691;
_LL691: _temp690= _temp687.where_clause; goto _LL689; _LL689: _temp688= _temp687.body;
goto _LL686; _LL686: if( _temp692 == 0){( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp694=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp694[ 0]=({ struct Cyc_Core_Impossible_struct _temp695; _temp695.tag= Cyc_Core_Impossible;
_temp695.f1= _tag_arr("switch clause vds not set", sizeof( unsigned char), 26u);
_temp695;}); _temp694;}));}{ struct Cyc_List_List* vds=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp692))->v; for( 0; vds != 0; vds=((
struct Cyc_List_List*) _check_null( vds))->tl){ Cyc_NewControlFlow_add_var_root(
env,( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( vds))->hd);}}
if( _temp690 != 0){ void* _temp698; void* _temp700; struct _tuple0 _temp696= Cyc_NewControlFlow_abstract_guard(
env,( struct Cyc_Absyn_Exp*) _check_null( _temp690)); _LL701: _temp700= _temp696.f1;
goto _LL699; _LL699: _temp698= _temp696.f2; goto _LL697; _LL697: _temp684=({
struct Cyc_List_List* _temp702=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp702->hd=( void*) _temp700; _temp702->tl=({ struct
Cyc_List_List* _temp703=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp703->hd=( void*) _temp698; _temp703->tl= _temp684; _temp703;}); _temp702;});}
Cyc_NewControlFlow_cf_prepass( env, _temp688);} return Cyc_List_imp_rev(
_temp684);} void Cyc_NewControlFlow_cf_prepass( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Stmt* s){ env.lhsides == 0? 0:(( int(*)( struct
_tagged_arr msg)) Cyc_Assert_AssertFail)( _tag_arr("new_control_flow.cyc:778 env.lhsides == null",
sizeof( unsigned char), 45u)); env.access_path == 0? 0:(( int(*)( struct
_tagged_arr msg)) Cyc_Assert_AssertFail)( _tag_arr("new_control_flow.cyc:779 env.access_path == null",
sizeof( unsigned char), 49u));{ void* _temp704=( void*) s->r; struct Cyc_Absyn_Exp*
_temp756; struct Cyc_Absyn_Stmt* _temp758; struct Cyc_Absyn_Stmt* _temp760;
struct Cyc_Absyn_Exp* _temp762; struct Cyc_Absyn_Exp* _temp764; struct Cyc_Absyn_Stmt*
_temp766; struct Cyc_Absyn_Stmt* _temp768; struct Cyc_Absyn_Exp* _temp770;
struct _tuple3 _temp772; struct Cyc_Absyn_Stmt* _temp774; struct Cyc_Absyn_Exp*
_temp776; struct Cyc_Absyn_Stmt* _temp778; struct Cyc_Absyn_Stmt* _temp780;
struct _tuple3 _temp782; struct Cyc_Absyn_Stmt* _temp784; struct Cyc_Absyn_Exp*
_temp786; struct Cyc_Absyn_Stmt* _temp788; struct Cyc_Absyn_Stmt* _temp790;
struct Cyc_Absyn_Stmt* _temp792; struct _tuple3 _temp794; struct Cyc_Absyn_Stmt*
_temp796; struct Cyc_Absyn_Exp* _temp798; struct _tuple3 _temp800; struct Cyc_Absyn_Stmt*
_temp802; struct Cyc_Absyn_Exp* _temp804; struct Cyc_Absyn_Exp* _temp806; struct
Cyc_List_List* _temp808; struct Cyc_Absyn_Exp* _temp810; struct Cyc_List_List*
_temp812; struct Cyc_Absyn_Stmt* _temp814; struct Cyc_Absyn_Decl* _temp816;
struct Cyc_Absyn_Decl _temp818; void* _temp819; struct Cyc_Absyn_Vardecl*
_temp821; struct Cyc_Absyn_Stmt* _temp823; struct Cyc_Absyn_Decl* _temp825;
struct Cyc_Absyn_Decl _temp827; void* _temp828; struct Cyc_Absyn_Exp* _temp830;
struct Cyc_Core_Opt* _temp832; struct Cyc_Core_Opt _temp834; struct Cyc_List_List*
_temp835; struct Cyc_Absyn_Stmt* _temp837; struct Cyc_Absyn_Decl* _temp839;
struct Cyc_Absyn_Decl _temp841; void* _temp842; struct Cyc_List_List* _temp844;
struct Cyc_Absyn_Stmt* _temp846; struct Cyc_List_List* _temp848; struct Cyc_Absyn_Stmt*
_temp850; struct Cyc_Absyn_Stmt* _temp852; struct Cyc_Absyn_Vardecl* _temp854;
struct Cyc_Absyn_Tvar* _temp856; _LL706: if( _temp704 ==( void*) Cyc_Absyn_Skip_s){
goto _LL707;} else{ goto _LL708;} _LL708: if(( unsigned int) _temp704 > 1u?*((
int*) _temp704) == Cyc_Absyn_Exp_s: 0){ _LL757: _temp756=(( struct Cyc_Absyn_Exp_s_struct*)
_temp704)->f1; goto _LL709;} else{ goto _LL710;} _LL710: if(( unsigned int)
_temp704 > 1u?*(( int*) _temp704) == Cyc_Absyn_Seq_s: 0){ _LL761: _temp760=((
struct Cyc_Absyn_Seq_s_struct*) _temp704)->f1; goto _LL759; _LL759: _temp758=((
struct Cyc_Absyn_Seq_s_struct*) _temp704)->f2; goto _LL711;} else{ goto _LL712;}
_LL712: if(( unsigned int) _temp704 > 1u?*(( int*) _temp704) == Cyc_Absyn_Return_s:
0){ _LL763: _temp762=(( struct Cyc_Absyn_Return_s_struct*) _temp704)->f1; if(
_temp762 == 0){ goto _LL713;} else{ goto _LL714;}} else{ goto _LL714;} _LL714:
if(( unsigned int) _temp704 > 1u?*(( int*) _temp704) == Cyc_Absyn_Return_s: 0){
_LL765: _temp764=(( struct Cyc_Absyn_Return_s_struct*) _temp704)->f1; goto
_LL715;} else{ goto _LL716;} _LL716: if(( unsigned int) _temp704 > 1u?*(( int*)
_temp704) == Cyc_Absyn_IfThenElse_s: 0){ _LL771: _temp770=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp704)->f1; goto _LL769; _LL769: _temp768=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp704)->f2; goto _LL767; _LL767: _temp766=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp704)->f3; goto _LL717;} else{ goto _LL718;} _LL718: if(( unsigned int)
_temp704 > 1u?*(( int*) _temp704) == Cyc_Absyn_Do_s: 0){ _LL779: _temp778=((
struct Cyc_Absyn_Do_s_struct*) _temp704)->f1; goto _LL773; _LL773: _temp772=((
struct Cyc_Absyn_Do_s_struct*) _temp704)->f2; _LL777: _temp776= _temp772.f1;
goto _LL775; _LL775: _temp774= _temp772.f2; goto _LL719;} else{ goto _LL720;}
_LL720: if(( unsigned int) _temp704 > 1u?*(( int*) _temp704) == Cyc_Absyn_While_s:
0){ _LL783: _temp782=(( struct Cyc_Absyn_While_s_struct*) _temp704)->f1; _LL787:
_temp786= _temp782.f1; goto _LL785; _LL785: _temp784= _temp782.f2; goto _LL781;
_LL781: _temp780=(( struct Cyc_Absyn_While_s_struct*) _temp704)->f2; goto _LL721;}
else{ goto _LL722;} _LL722: if(( unsigned int) _temp704 > 1u?*(( int*) _temp704)
== Cyc_Absyn_Break_s: 0){ goto _LL723;} else{ goto _LL724;} _LL724: if((
unsigned int) _temp704 > 1u?*(( int*) _temp704) == Cyc_Absyn_Continue_s: 0){
goto _LL725;} else{ goto _LL726;} _LL726: if(( unsigned int) _temp704 > 1u?*((
int*) _temp704) == Cyc_Absyn_Goto_s: 0){ _LL789: _temp788=(( struct Cyc_Absyn_Goto_s_struct*)
_temp704)->f2; if( _temp788 == 0){ goto _LL727;} else{ goto _LL728;}} else{ goto
_LL728;} _LL728: if(( unsigned int) _temp704 > 1u?*(( int*) _temp704) == Cyc_Absyn_Goto_s:
0){ _LL791: _temp790=(( struct Cyc_Absyn_Goto_s_struct*) _temp704)->f2; goto
_LL729;} else{ goto _LL730;} _LL730: if(( unsigned int) _temp704 > 1u?*(( int*)
_temp704) == Cyc_Absyn_For_s: 0){ _LL807: _temp806=(( struct Cyc_Absyn_For_s_struct*)
_temp704)->f1; goto _LL801; _LL801: _temp800=(( struct Cyc_Absyn_For_s_struct*)
_temp704)->f2; _LL805: _temp804= _temp800.f1; goto _LL803; _LL803: _temp802=
_temp800.f2; goto _LL795; _LL795: _temp794=(( struct Cyc_Absyn_For_s_struct*)
_temp704)->f3; _LL799: _temp798= _temp794.f1; goto _LL797; _LL797: _temp796=
_temp794.f2; goto _LL793; _LL793: _temp792=(( struct Cyc_Absyn_For_s_struct*)
_temp704)->f4; goto _LL731;} else{ goto _LL732;} _LL732: if(( unsigned int)
_temp704 > 1u?*(( int*) _temp704) == Cyc_Absyn_Switch_s: 0){ _LL811: _temp810=((
struct Cyc_Absyn_Switch_s_struct*) _temp704)->f1; goto _LL809; _LL809: _temp808=((
struct Cyc_Absyn_Switch_s_struct*) _temp704)->f2; goto _LL733;} else{ goto
_LL734;} _LL734: if(( unsigned int) _temp704 > 1u?*(( int*) _temp704) == Cyc_Absyn_Fallthru_s:
0){ _LL813: _temp812=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp704)->f1; goto
_LL735;} else{ goto _LL736;} _LL736: if(( unsigned int) _temp704 > 1u?*(( int*)
_temp704) == Cyc_Absyn_Decl_s: 0){ _LL817: _temp816=(( struct Cyc_Absyn_Decl_s_struct*)
_temp704)->f1; _temp818=* _temp816; _LL820: _temp819=( void*) _temp818.r; if(*((
int*) _temp819) == Cyc_Absyn_Var_d){ _LL822: _temp821=(( struct Cyc_Absyn_Var_d_struct*)
_temp819)->f1; goto _LL815;} else{ goto _LL738;} _LL815: _temp814=(( struct Cyc_Absyn_Decl_s_struct*)
_temp704)->f2; goto _LL737;} else{ goto _LL738;} _LL738: if(( unsigned int)
_temp704 > 1u?*(( int*) _temp704) == Cyc_Absyn_Decl_s: 0){ _LL826: _temp825=((
struct Cyc_Absyn_Decl_s_struct*) _temp704)->f1; _temp827=* _temp825; _LL829:
_temp828=( void*) _temp827.r; if(*(( int*) _temp828) == Cyc_Absyn_Let_d){ _LL833:
_temp832=(( struct Cyc_Absyn_Let_d_struct*) _temp828)->f2; if( _temp832 == 0){
goto _LL740;} else{ _temp834=* _temp832; _LL836: _temp835=( struct Cyc_List_List*)
_temp834.v; goto _LL831;} _LL831: _temp830=(( struct Cyc_Absyn_Let_d_struct*)
_temp828)->f4; goto _LL824;} else{ goto _LL740;} _LL824: _temp823=(( struct Cyc_Absyn_Decl_s_struct*)
_temp704)->f2; goto _LL739;} else{ goto _LL740;} _LL740: if(( unsigned int)
_temp704 > 1u?*(( int*) _temp704) == Cyc_Absyn_Decl_s: 0){ _LL840: _temp839=((
struct Cyc_Absyn_Decl_s_struct*) _temp704)->f1; _temp841=* _temp839; _LL843:
_temp842=( void*) _temp841.r; if(*(( int*) _temp842) == Cyc_Absyn_Letv_d){
_LL845: _temp844=(( struct Cyc_Absyn_Letv_d_struct*) _temp842)->f1; goto _LL838;}
else{ goto _LL742;} _LL838: _temp837=(( struct Cyc_Absyn_Decl_s_struct*)
_temp704)->f2; goto _LL741;} else{ goto _LL742;} _LL742: if(( unsigned int)
_temp704 > 1u?*(( int*) _temp704) == Cyc_Absyn_Label_s: 0){ _LL847: _temp846=((
struct Cyc_Absyn_Label_s_struct*) _temp704)->f2; goto _LL743;} else{ goto _LL744;}
_LL744: if(( unsigned int) _temp704 > 1u?*(( int*) _temp704) == Cyc_Absyn_TryCatch_s:
0){ _LL851: _temp850=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp704)->f1; goto
_LL849; _LL849: _temp848=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp704)->f2;
goto _LL745;} else{ goto _LL746;} _LL746: if(( unsigned int) _temp704 > 1u?*((
int*) _temp704) == Cyc_Absyn_Region_s: 0){ _LL857: _temp856=(( struct Cyc_Absyn_Region_s_struct*)
_temp704)->f1; goto _LL855; _LL855: _temp854=(( struct Cyc_Absyn_Region_s_struct*)
_temp704)->f2; goto _LL853; _LL853: _temp852=(( struct Cyc_Absyn_Region_s_struct*)
_temp704)->f3; goto _LL747;} else{ goto _LL748;} _LL748: if(( unsigned int)
_temp704 > 1u?*(( int*) _temp704) == Cyc_Absyn_Decl_s: 0){ goto _LL749;} else{
goto _LL750;} _LL750: if(( unsigned int) _temp704 > 1u?*(( int*) _temp704) ==
Cyc_Absyn_SwitchC_s: 0){ goto _LL751;} else{ goto _LL752;} _LL752: if((
unsigned int) _temp704 > 1u?*(( int*) _temp704) == Cyc_Absyn_Cut_s: 0){ goto
_LL753;} else{ goto _LL754;} _LL754: if(( unsigned int) _temp704 > 1u?*(( int*)
_temp704) == Cyc_Absyn_Splice_s: 0){ goto _LL755;} else{ goto _LL705;} _LL707:
return; _LL709: { void* _temp858= Cyc_NewControlFlow_abstract_exp_top( env,
_temp756); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp859=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp859->hd=( void*) _temp858; _temp859->tl= 0; _temp859;})); return;} _LL711:
Cyc_NewControlFlow_cf_prepass( env, _temp760); Cyc_NewControlFlow_cf_prepass(
env, _temp758); return; _LL713: return; _LL715: { void* _temp860= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env),( struct Cyc_Absyn_Exp*) _check_null(
_temp764)); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp861=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp861->hd=( void*) _temp860; _temp861->tl= 0; _temp861;})); return;} _LL717: {
void* _temp864; void* _temp866; struct _tuple0 _temp862= Cyc_NewControlFlow_abstract_guard(
env, _temp770); _LL867: _temp866= _temp862.f1; goto _LL865; _LL865: _temp864=
_temp862.f2; goto _LL863; _LL863: Cyc_NewControlFlow_cf_set_absexps( s,({ struct
Cyc_List_List* _temp868=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp868->hd=( void*) _temp866; _temp868->tl=({ struct Cyc_List_List* _temp869=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp869->hd=(
void*) _temp864; _temp869->tl= 0; _temp869;}); _temp868;})); Cyc_NewControlFlow_cf_prepass(
env, _temp768); Cyc_NewControlFlow_cf_prepass( env, _temp766); return;} _LL719:
_temp786= _temp776; _temp784= _temp774; _temp780= _temp778; goto _LL721; _LL721: {
void* _temp872; void* _temp874; struct _tuple0 _temp870= Cyc_NewControlFlow_abstract_guard(
env, _temp786); _LL875: _temp874= _temp870.f1; goto _LL873; _LL873: _temp872=
_temp870.f2; goto _LL871; _LL871: Cyc_NewControlFlow_cf_set_absexps( s,({ struct
Cyc_List_List* _temp876=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp876->hd=( void*) _temp874; _temp876->tl=({ struct Cyc_List_List* _temp877=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp877->hd=(
void*) _temp872; _temp877->tl= 0; _temp877;}); _temp876;})); Cyc_NewControlFlow_cf_prepass(
env, _temp780); return;} _LL723: return; _LL725: return; _LL727:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp878=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp878[ 0]=({ struct
Cyc_Core_Impossible_struct _temp879; _temp879.tag= Cyc_Core_Impossible; _temp879.f1=
_tag_arr("cf_prepass: goto w/o destination", sizeof( unsigned char), 33u);
_temp879;}); _temp878;})); _LL729: { struct Cyc_Absyn_Stmt* _temp880=( Cyc_NewControlFlow_get_stmt_annot(
s))->encloser; struct Cyc_Absyn_Stmt* _temp881=( Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*) _check_null( _temp790)))->encloser; while( _temp881 !=
_temp880) { struct Cyc_Absyn_Stmt* _temp882=( Cyc_NewControlFlow_get_stmt_annot(
_temp880))->encloser; if( _temp882 == _temp880){({ void* _temp883[ 0u]={}; Cyc_Tcutil_terr(
s->loc, _tag_arr("goto enters local scope or exception handler", sizeof(
unsigned char), 45u), _tag_arr( _temp883, sizeof( void*), 0u));}); break;}
_temp880= _temp882;} return;} _LL731: { void* _temp884= Cyc_NewControlFlow_abstract_exp_top(
env, _temp806); void* _temp887; void* _temp889; struct _tuple0 _temp885= Cyc_NewControlFlow_abstract_guard(
env, _temp804); _LL890: _temp889= _temp885.f1; goto _LL888; _LL888: _temp887=
_temp885.f2; goto _LL886; _LL886: { void* _temp891= Cyc_NewControlFlow_abstract_exp_top(
env, _temp798); Cyc_NewControlFlow_cf_set_absexps( s,({ void* _temp892[ 4u];
_temp892[ 3u]= _temp891; _temp892[ 2u]= _temp887; _temp892[ 1u]= _temp889;
_temp892[ 0u]= _temp884; Cyc_List_list( _tag_arr( _temp892, sizeof( void*), 4u));}));
Cyc_NewControlFlow_cf_prepass( env, _temp792); return;}} _LL733: { void*
_temp893= Cyc_NewControlFlow_abstract_exp_top( Cyc_NewControlFlow_use_it_env(
env), _temp810); struct Cyc_List_List* _temp894= Cyc_NewControlFlow_cf_prepass_scs(
env, _temp808); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp895=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp895->hd=( void*) _temp893; _temp895->tl= _temp894; _temp895;})); return;}
_LL735: { void* _temp898; void* _temp900; struct Cyc_NewControlFlow_AbsSynOne
_temp896= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_use_exp_unordered(
env, _temp812, 0)); _LL901: _temp900=( void*) _temp896.inner_exp; goto _LL899;
_LL899: _temp898=( void*) _temp896.assigns; goto _LL897; _LL897: { void*
_temp902= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp900,
_temp898); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp903=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp903->hd=( void*) _temp902; _temp903->tl= 0; _temp903;})); return;}} _LL737:
Cyc_NewControlFlow_add_var_root( env, _temp821); if( _temp821->initializer != 0){
struct Cyc_NewControlFlow_AbsEnv _temp904= Cyc_NewControlFlow_change_lhs( env,({
struct Cyc_List_List* _temp907=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp907->hd=( void*) Cyc_CfAbsexp_mkLocalOp( _temp821);
_temp907->tl= 0; _temp907;})); void* _temp905= Cyc_NewControlFlow_abstract_exp_top(
_temp904,( struct Cyc_Absyn_Exp*) _check_null( _temp821->initializer)); Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp906=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp906->hd=( void*) _temp905; _temp906->tl= 0;
_temp906;}));} Cyc_NewControlFlow_cf_prepass( env, _temp814); return; _LL739:
for( 0; _temp835 != 0; _temp835=(( struct Cyc_List_List*) _check_null( _temp835))->tl){
Cyc_NewControlFlow_add_var_root( env,( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( _temp835))->hd);}{ void* _temp908= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env), _temp830); Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp909=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp909->hd=( void*) _temp908; _temp909->tl= 0;
_temp909;})); Cyc_NewControlFlow_cf_prepass( env, _temp823); return;} _LL741:
for( 0; _temp844 != 0; _temp844=(( struct Cyc_List_List*) _check_null( _temp844))->tl){
Cyc_NewControlFlow_add_var_root( env,( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( _temp844))->hd);} Cyc_NewControlFlow_cf_prepass( env, _temp837);
return; _LL743: Cyc_NewControlFlow_cf_prepass( env, _temp846); return; _LL745:
Cyc_NewControlFlow_cf_prepass( env, _temp850);{ struct Cyc_List_List* _temp910=
Cyc_NewControlFlow_cf_prepass_scs( env, _temp848); Cyc_NewControlFlow_cf_set_absexps(
s, _temp910); return;} _LL747: Cyc_NewControlFlow_add_var_root( env, _temp854);
Cyc_NewControlFlow_cf_prepass( env, _temp852); return; _LL749: goto _LL751;
_LL751: goto _LL753; _LL753: goto _LL755; _LL755:( int) _throw(( void*)({ struct
Cyc_Core_Impossible_struct* _temp911=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp911[ 0]=({ struct
Cyc_Core_Impossible_struct _temp912; _temp912.tag= Cyc_Core_Impossible; _temp912.f1=
_tag_arr("cf_prepass: bad stmt form", sizeof( unsigned char), 26u); _temp912;});
_temp911;})); _LL705:;}} static int Cyc_NewControlFlow_iterate_cf_check= 0;
static int Cyc_NewControlFlow_iteration_num= 0; struct Cyc_NewControlFlow_AnalEnv;
void Cyc_NewControlFlow_update_tryflow( struct Cyc_NewControlFlow_AnalEnv* env,
void* new_flow){ if( env->in_try){( void*)( env->tryflow=( void*) Cyc_CfFlowInfo_join_flow(
new_flow,( void*) env->tryflow));}} static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_pre_stmt_check( void* in_flow, struct Cyc_Absyn_Stmt* s){
struct Cyc_NewControlFlow_CFStmtAnnot* _temp913= Cyc_NewControlFlow_get_stmt_annot(
s);( void*)( _temp913->flow=( void*) Cyc_CfFlowInfo_join_flow( in_flow,( void*)
_temp913->flow)); ++ _temp913->visited; _temp913->visited == Cyc_NewControlFlow_iteration_num?
0:(( int(*)( struct _tagged_arr msg)) Cyc_Assert_AssertFail)( _tag_arr("new_control_flow.cyc:914 annot->visited == iteration_num",
sizeof( unsigned char), 57u)); return _temp913;} static void Cyc_NewControlFlow_update_flow(
struct Cyc_Absyn_Stmt* s, void* flow){ struct Cyc_NewControlFlow_CFStmtAnnot*
_temp914= Cyc_NewControlFlow_get_stmt_annot( s); void* _temp915= Cyc_CfFlowInfo_join_flow(
flow,( void*) _temp914->flow); if( ! Cyc_CfFlowInfo_flow_lessthan_approx(
_temp915,( void*) _temp914->flow)){( void*)( _temp914->flow=( void*) _temp915);
if( _temp914->visited == Cyc_NewControlFlow_iteration_num){ Cyc_NewControlFlow_iterate_cf_check=
1;}}} static void* Cyc_NewControlFlow_add_init_vars_flow( struct Cyc_NewControlFlow_AnalEnv*
env, void* flow, struct Cyc_List_List* vds){ void* _temp916= flow; struct Cyc_Dict_Dict*
_temp922; _LL918: if( _temp916 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL919;}
else{ goto _LL920;} _LL920: if(( unsigned int) _temp916 > 1u?*(( int*) _temp916)
== Cyc_CfFlowInfo_InitsFL: 0){ _LL923: _temp922=(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp916)->f1; goto _LL921;} else{ goto _LL917;} _LL919: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL921: for( 0; vds != 0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){
struct Cyc_CfFlowInfo_VarRoot_struct* _temp924=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp926=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp926[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp927; _temp927.tag= Cyc_CfFlowInfo_VarRoot;
_temp927.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
vds))->hd; _temp927;}); _temp926;}); void* _temp925= Cyc_CfFlowInfo_assign_unknown_dict((
void*) Cyc_CfFlowInfo_AllIL, Cyc_Dict_lookup( env->roots,( void*) _temp924));
_temp922= Cyc_Dict_insert( _temp922,( void*) _temp924, _temp925);} return( void*)({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp928=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp928[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp929; _temp929.tag= Cyc_CfFlowInfo_InitsFL;
_temp929.f1= _temp922; _temp929;}); _temp928;}); _LL917:;} static void* Cyc_NewControlFlow_add_vardecls_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp930= flow; struct Cyc_Dict_Dict* _temp936; _LL932: if( _temp930 ==(
void*) Cyc_CfFlowInfo_BottomFL){ goto _LL933;} else{ goto _LL934;} _LL934: if((
unsigned int) _temp930 > 1u?*(( int*) _temp930) == Cyc_CfFlowInfo_InitsFL: 0){
_LL937: _temp936=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp930)->f1; goto
_LL935;} else{ goto _LL931;} _LL933: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL935: for( 0; vds != 0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){
struct Cyc_CfFlowInfo_VarRoot_struct* _temp938=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp939=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp939[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp940; _temp940.tag= Cyc_CfFlowInfo_VarRoot;
_temp940.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
vds))->hd; _temp940;}); _temp939;}); _temp936= Cyc_Dict_insert( _temp936,( void*)
_temp938, Cyc_Dict_lookup( env->roots,( void*) _temp938));} return( void*)({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp941=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp941[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp942; _temp942.tag= Cyc_CfFlowInfo_InitsFL;
_temp942.f1= _temp936; _temp942;}); _temp941;}); _LL931:;} static struct _tuple0
Cyc_NewControlFlow_cf_evalguard( struct Cyc_NewControlFlow_AnalEnv* env, struct
Cyc_Position_Segment* loc, struct Cyc_List_List* aes, void* in_flow){ if(( void*)((
struct Cyc_List_List*) _check_null( aes))->hd ==( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( aes))->tl))->hd){ void*
_temp943= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct Cyc_List_List*)
_check_null( aes))->hd, in_flow); return({ struct _tuple0 _temp944; _temp944.f1=
_temp943; _temp944.f2= _temp943; _temp944;});} return({ struct _tuple0 _temp945;
_temp945.f1= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct Cyc_List_List*)
_check_null( aes))->hd, in_flow); _temp945.f2= Cyc_CfAbsexp_eval_absexp( env,
loc,( void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( aes))->tl))->hd, in_flow); _temp945;});} static void Cyc_NewControlFlow_cf_analyze_switch_clauses(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_List_List* scs, struct Cyc_List_List*
aes, void* in_flow){ for( 0; scs != 0; scs=(( struct Cyc_List_List*) _check_null(
scs))->tl){ struct Cyc_Absyn_Switch_clause _temp948; struct Cyc_Absyn_Stmt*
_temp949; struct Cyc_Absyn_Exp* _temp951; struct Cyc_Core_Opt* _temp953; struct
Cyc_Absyn_Switch_clause* _temp946=( struct Cyc_Absyn_Switch_clause*)(( struct
Cyc_List_List*) _check_null( scs))->hd; _temp948=* _temp946; _LL954: _temp953=
_temp948.pat_vars; goto _LL952; _LL952: _temp951= _temp948.where_clause; goto
_LL950; _LL950: _temp949= _temp948.body; goto _LL947; _LL947: { void* _temp955=
Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp953))->v); _temp955= Cyc_NewControlFlow_add_init_vars_flow(
env, _temp955,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp953))->v);{ void* body_outflow; if( _temp951 != 0){ void* _temp958; void*
_temp960; struct _tuple0 _temp956= Cyc_NewControlFlow_cf_evalguard( env,((
struct Cyc_Absyn_Exp*) _check_null( _temp951))->loc, aes, _temp955); _LL961:
_temp960= _temp956.f1; goto _LL959; _LL959: _temp958= _temp956.f2; goto _LL957;
_LL957: aes=(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( aes))->tl))->tl; in_flow= _temp958; body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp949, _temp960);} else{ body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp949, _temp955);}{ void* _temp962= body_outflow; _LL964: if( _temp962
==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL965;} else{ goto _LL966;} _LL966:
goto _LL967; _LL965: goto _LL963; _LL967:({ void* _temp968[ 0u]={}; Cyc_Tcutil_terr(
_temp949->loc, _tag_arr("switch clause may implicitly fallthru", sizeof(
unsigned char), 38u), _tag_arr( _temp968, sizeof( void*), 0u));}); goto _LL963;
_LL963:;}}}}} void* Cyc_NewControlFlow_cf_analyze_stmt( struct Cyc_NewControlFlow_AnalEnv*
env, struct Cyc_Absyn_Stmt* s, void* in_flow){ struct Cyc_NewControlFlow_CFStmtAnnot*
_temp969= Cyc_NewControlFlow_pre_stmt_check( in_flow, s); in_flow=( void*)
_temp969->flow;{ struct Cyc_List_List* _temp970= _temp969->absexps; void*
_temp971=( void*) s->r; struct Cyc_Absyn_Exp* _temp1017; struct Cyc_Absyn_Exp*
_temp1019; struct Cyc_Absyn_Exp* _temp1021; struct Cyc_Absyn_Stmt* _temp1023;
struct Cyc_Absyn_Stmt* _temp1025; struct Cyc_Absyn_Stmt* _temp1027; struct Cyc_Absyn_Stmt*
_temp1029; struct Cyc_Absyn_Exp* _temp1031; struct Cyc_Absyn_Stmt* _temp1033;
struct _tuple3 _temp1035; struct Cyc_Absyn_Stmt* _temp1037; struct Cyc_Absyn_Exp*
_temp1039; struct _tuple3 _temp1041; struct Cyc_Absyn_Stmt* _temp1043; struct
Cyc_Absyn_Exp* _temp1045; struct Cyc_Absyn_Stmt* _temp1047; struct Cyc_Absyn_Stmt*
_temp1049; struct _tuple3 _temp1051; struct Cyc_Absyn_Stmt* _temp1053; struct
Cyc_Absyn_Exp* _temp1055; struct _tuple3 _temp1057; struct Cyc_Absyn_Stmt*
_temp1059; struct Cyc_Absyn_Exp* _temp1061; struct Cyc_Absyn_Exp* _temp1063;
struct Cyc_Absyn_Stmt* _temp1065; struct Cyc_Absyn_Switch_clause** _temp1067;
struct Cyc_Absyn_Switch_clause* _temp1069; struct Cyc_List_List* _temp1070;
struct Cyc_Absyn_Stmt* _temp1072; struct Cyc_Absyn_Stmt* _temp1074; struct Cyc_Absyn_Stmt*
_temp1076; struct Cyc_List_List* _temp1078; struct Cyc_Absyn_Exp* _temp1080;
struct Cyc_List_List* _temp1082; struct Cyc_Absyn_Stmt* _temp1084; struct Cyc_Absyn_Stmt*
_temp1086; struct Cyc_Absyn_Decl* _temp1088; struct Cyc_Absyn_Decl _temp1090;
void* _temp1091; struct Cyc_Absyn_Vardecl* _temp1093; struct Cyc_Absyn_Stmt*
_temp1095; struct Cyc_Absyn_Decl* _temp1097; struct Cyc_Absyn_Decl _temp1099;
void* _temp1100; struct Cyc_Absyn_Exp* _temp1102; struct Cyc_Core_Opt* _temp1104;
struct Cyc_Core_Opt _temp1106; struct Cyc_List_List* _temp1107; struct Cyc_Absyn_Stmt*
_temp1109; struct Cyc_Absyn_Decl* _temp1111; struct Cyc_Absyn_Decl _temp1113;
void* _temp1114; struct Cyc_List_List* _temp1116; struct Cyc_Absyn_Stmt*
_temp1118; struct Cyc_Absyn_Stmt* _temp1120; struct Cyc_Absyn_Vardecl* _temp1122;
struct Cyc_Absyn_Tvar* _temp1124; _LL973: if( _temp971 ==( void*) Cyc_Absyn_Skip_s){
goto _LL974;} else{ goto _LL975;} _LL975: if(( unsigned int) _temp971 > 1u?*((
int*) _temp971) == Cyc_Absyn_Return_s: 0){ _LL1018: _temp1017=(( struct Cyc_Absyn_Return_s_struct*)
_temp971)->f1; if( _temp1017 == 0){ goto _LL976;} else{ goto _LL977;}} else{
goto _LL977;} _LL977: if(( unsigned int) _temp971 > 1u?*(( int*) _temp971) ==
Cyc_Absyn_Return_s: 0){ _LL1020: _temp1019=(( struct Cyc_Absyn_Return_s_struct*)
_temp971)->f1; goto _LL978;} else{ goto _LL979;} _LL979: if(( unsigned int)
_temp971 > 1u?*(( int*) _temp971) == Cyc_Absyn_Exp_s: 0){ _LL1022: _temp1021=((
struct Cyc_Absyn_Exp_s_struct*) _temp971)->f1; goto _LL980;} else{ goto _LL981;}
_LL981: if(( unsigned int) _temp971 > 1u?*(( int*) _temp971) == Cyc_Absyn_Seq_s:
0){ _LL1026: _temp1025=(( struct Cyc_Absyn_Seq_s_struct*) _temp971)->f1; goto
_LL1024; _LL1024: _temp1023=(( struct Cyc_Absyn_Seq_s_struct*) _temp971)->f2;
goto _LL982;} else{ goto _LL983;} _LL983: if(( unsigned int) _temp971 > 1u?*((
int*) _temp971) == Cyc_Absyn_IfThenElse_s: 0){ _LL1032: _temp1031=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp971)->f1; goto _LL1030; _LL1030: _temp1029=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp971)->f2; goto _LL1028; _LL1028: _temp1027=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp971)->f3; goto _LL984;} else{ goto _LL985;} _LL985: if(( unsigned int)
_temp971 > 1u?*(( int*) _temp971) == Cyc_Absyn_While_s: 0){ _LL1036: _temp1035=((
struct Cyc_Absyn_While_s_struct*) _temp971)->f1; _LL1040: _temp1039= _temp1035.f1;
goto _LL1038; _LL1038: _temp1037= _temp1035.f2; goto _LL1034; _LL1034: _temp1033=((
struct Cyc_Absyn_While_s_struct*) _temp971)->f2; goto _LL986;} else{ goto _LL987;}
_LL987: if(( unsigned int) _temp971 > 1u?*(( int*) _temp971) == Cyc_Absyn_Do_s:
0){ _LL1048: _temp1047=(( struct Cyc_Absyn_Do_s_struct*) _temp971)->f1; goto
_LL1042; _LL1042: _temp1041=(( struct Cyc_Absyn_Do_s_struct*) _temp971)->f2;
_LL1046: _temp1045= _temp1041.f1; goto _LL1044; _LL1044: _temp1043= _temp1041.f2;
goto _LL988;} else{ goto _LL989;} _LL989: if(( unsigned int) _temp971 > 1u?*((
int*) _temp971) == Cyc_Absyn_For_s: 0){ _LL1064: _temp1063=(( struct Cyc_Absyn_For_s_struct*)
_temp971)->f1; goto _LL1058; _LL1058: _temp1057=(( struct Cyc_Absyn_For_s_struct*)
_temp971)->f2; _LL1062: _temp1061= _temp1057.f1; goto _LL1060; _LL1060:
_temp1059= _temp1057.f2; goto _LL1052; _LL1052: _temp1051=(( struct Cyc_Absyn_For_s_struct*)
_temp971)->f3; _LL1056: _temp1055= _temp1051.f1; goto _LL1054; _LL1054:
_temp1053= _temp1051.f2; goto _LL1050; _LL1050: _temp1049=(( struct Cyc_Absyn_For_s_struct*)
_temp971)->f4; goto _LL990;} else{ goto _LL991;} _LL991: if(( unsigned int)
_temp971 > 1u?*(( int*) _temp971) == Cyc_Absyn_Break_s: 0){ _LL1066: _temp1065=((
struct Cyc_Absyn_Break_s_struct*) _temp971)->f1; if( _temp1065 == 0){ goto
_LL992;} else{ goto _LL993;}} else{ goto _LL993;} _LL993: if(( unsigned int)
_temp971 > 1u?*(( int*) _temp971) == Cyc_Absyn_Fallthru_s: 0){ _LL1071:
_temp1070=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp971)->f1; goto _LL1068;
_LL1068: _temp1067=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp971)->f2; if(
_temp1067 == 0){ goto _LL995;} else{ _temp1069=* _temp1067; goto _LL994;}} else{
goto _LL995;} _LL995: if(( unsigned int) _temp971 > 1u?*(( int*) _temp971) ==
Cyc_Absyn_Break_s: 0){ _LL1073: _temp1072=(( struct Cyc_Absyn_Break_s_struct*)
_temp971)->f1; goto _LL996;} else{ goto _LL997;} _LL997: if(( unsigned int)
_temp971 > 1u?*(( int*) _temp971) == Cyc_Absyn_Continue_s: 0){ _LL1075:
_temp1074=(( struct Cyc_Absyn_Continue_s_struct*) _temp971)->f1; goto _LL998;}
else{ goto _LL999;} _LL999: if(( unsigned int) _temp971 > 1u?*(( int*) _temp971)
== Cyc_Absyn_Goto_s: 0){ _LL1077: _temp1076=(( struct Cyc_Absyn_Goto_s_struct*)
_temp971)->f2; goto _LL1000;} else{ goto _LL1001;} _LL1001: if(( unsigned int)
_temp971 > 1u?*(( int*) _temp971) == Cyc_Absyn_Switch_s: 0){ _LL1081: _temp1080=((
struct Cyc_Absyn_Switch_s_struct*) _temp971)->f1; goto _LL1079; _LL1079:
_temp1078=(( struct Cyc_Absyn_Switch_s_struct*) _temp971)->f2; goto _LL1002;}
else{ goto _LL1003;} _LL1003: if(( unsigned int) _temp971 > 1u?*(( int*)
_temp971) == Cyc_Absyn_TryCatch_s: 0){ _LL1085: _temp1084=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp971)->f1; goto _LL1083; _LL1083: _temp1082=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp971)->f2; goto _LL1004;} else{ goto _LL1005;} _LL1005: if(( unsigned int)
_temp971 > 1u?*(( int*) _temp971) == Cyc_Absyn_Decl_s: 0){ _LL1089: _temp1088=((
struct Cyc_Absyn_Decl_s_struct*) _temp971)->f1; _temp1090=* _temp1088; _LL1092:
_temp1091=( void*) _temp1090.r; if(*(( int*) _temp1091) == Cyc_Absyn_Var_d){
_LL1094: _temp1093=(( struct Cyc_Absyn_Var_d_struct*) _temp1091)->f1; goto
_LL1087;} else{ goto _LL1007;} _LL1087: _temp1086=(( struct Cyc_Absyn_Decl_s_struct*)
_temp971)->f2; goto _LL1006;} else{ goto _LL1007;} _LL1007: if(( unsigned int)
_temp971 > 1u?*(( int*) _temp971) == Cyc_Absyn_Decl_s: 0){ _LL1098: _temp1097=((
struct Cyc_Absyn_Decl_s_struct*) _temp971)->f1; _temp1099=* _temp1097; _LL1101:
_temp1100=( void*) _temp1099.r; if(*(( int*) _temp1100) == Cyc_Absyn_Let_d){
_LL1105: _temp1104=(( struct Cyc_Absyn_Let_d_struct*) _temp1100)->f2; if(
_temp1104 == 0){ goto _LL1009;} else{ _temp1106=* _temp1104; _LL1108: _temp1107=(
struct Cyc_List_List*) _temp1106.v; goto _LL1103;} _LL1103: _temp1102=(( struct
Cyc_Absyn_Let_d_struct*) _temp1100)->f4; goto _LL1096;} else{ goto _LL1009;}
_LL1096: _temp1095=(( struct Cyc_Absyn_Decl_s_struct*) _temp971)->f2; goto
_LL1008;} else{ goto _LL1009;} _LL1009: if(( unsigned int) _temp971 > 1u?*(( int*)
_temp971) == Cyc_Absyn_Decl_s: 0){ _LL1112: _temp1111=(( struct Cyc_Absyn_Decl_s_struct*)
_temp971)->f1; _temp1113=* _temp1111; _LL1115: _temp1114=( void*) _temp1113.r;
if(*(( int*) _temp1114) == Cyc_Absyn_Letv_d){ _LL1117: _temp1116=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1114)->f1; goto _LL1110;} else{ goto _LL1011;} _LL1110: _temp1109=(( struct
Cyc_Absyn_Decl_s_struct*) _temp971)->f2; goto _LL1010;} else{ goto _LL1011;}
_LL1011: if(( unsigned int) _temp971 > 1u?*(( int*) _temp971) == Cyc_Absyn_Label_s:
0){ _LL1119: _temp1118=(( struct Cyc_Absyn_Label_s_struct*) _temp971)->f2; goto
_LL1012;} else{ goto _LL1013;} _LL1013: if(( unsigned int) _temp971 > 1u?*(( int*)
_temp971) == Cyc_Absyn_Region_s: 0){ _LL1125: _temp1124=(( struct Cyc_Absyn_Region_s_struct*)
_temp971)->f1; goto _LL1123; _LL1123: _temp1122=(( struct Cyc_Absyn_Region_s_struct*)
_temp971)->f2; goto _LL1121; _LL1121: _temp1120=(( struct Cyc_Absyn_Region_s_struct*)
_temp971)->f3; goto _LL1014;} else{ goto _LL1015;} _LL1015: goto _LL1016; _LL974:
return in_flow; _LL976: return( void*) Cyc_CfFlowInfo_BottomFL; _LL978: Cyc_CfAbsexp_eval_absexp(
env,(( struct Cyc_Absyn_Exp*) _check_null( _temp1019))->loc,( void*)(( struct
Cyc_List_List*) _check_null( _temp970))->hd, in_flow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL980: return Cyc_CfAbsexp_eval_absexp( env, _temp1021->loc,( void*)(( struct
Cyc_List_List*) _check_null( _temp970))->hd, in_flow); _LL982: return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1023, Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1025, in_flow));
_LL984: { void* _temp1128; void* _temp1130; struct _tuple0 _temp1126= Cyc_NewControlFlow_cf_evalguard(
env, _temp1031->loc, _temp970, in_flow); _LL1131: _temp1130= _temp1126.f1; goto
_LL1129; _LL1129: _temp1128= _temp1126.f2; goto _LL1127; _LL1127: return Cyc_CfFlowInfo_join_flow(
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1029, _temp1130), Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1027, _temp1128));} _LL986: { void* _temp1132=( void*)( Cyc_NewControlFlow_pre_stmt_check(
in_flow, _temp1037))->flow; void* _temp1135; void* _temp1137; struct _tuple0
_temp1133= Cyc_NewControlFlow_cf_evalguard( env, _temp1039->loc, _temp970,
_temp1132); _LL1138: _temp1137= _temp1133.f1; goto _LL1136; _LL1136: _temp1135=
_temp1133.f2; goto _LL1134; _LL1134: { void* _temp1139= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1033, _temp1137); Cyc_NewControlFlow_update_flow( _temp1037, _temp1139);
return _temp1135;}} _LL988: { void* _temp1140= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1047, in_flow); void* _temp1141=( void*)( Cyc_NewControlFlow_pre_stmt_check(
_temp1140, _temp1043))->flow; void* _temp1144; void* _temp1146; struct _tuple0
_temp1142= Cyc_NewControlFlow_cf_evalguard( env, _temp1045->loc, _temp970,
_temp1141); _LL1147: _temp1146= _temp1142.f1; goto _LL1145; _LL1145: _temp1144=
_temp1142.f2; goto _LL1143; _LL1143: Cyc_NewControlFlow_update_flow( _temp1047,
_temp1146); return _temp1144;} _LL990: { void* _temp1148= Cyc_CfAbsexp_eval_absexp(
env, _temp1063->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp970))->hd,
in_flow); void* _temp1149=( void*)( Cyc_NewControlFlow_pre_stmt_check( _temp1148,
_temp1059))->flow; void* _temp1152; void* _temp1154; struct _tuple0 _temp1150=
Cyc_NewControlFlow_cf_evalguard( env, _temp1061->loc,(( struct Cyc_List_List*)
_check_null( _temp970))->tl, _temp1149); _LL1155: _temp1154= _temp1150.f1; goto
_LL1153; _LL1153: _temp1152= _temp1150.f2; goto _LL1151; _LL1151: { void*
_temp1156= Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1049, _temp1154); void*
_temp1157=( void*)( Cyc_NewControlFlow_pre_stmt_check( _temp1156, _temp1053))->flow;
void* _temp1158= Cyc_CfAbsexp_eval_absexp( env, _temp1055->loc,( void*)(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp970))->tl))->tl))->tl))->hd,
_temp1157); Cyc_NewControlFlow_update_flow( _temp1059, _temp1158); return
_temp1152;}} _LL992: return( void*) Cyc_CfFlowInfo_BottomFL; _LL994: in_flow=
Cyc_CfAbsexp_eval_absexp( env, s->loc,( void*)(( struct Cyc_List_List*)
_check_null( _temp970))->hd, in_flow);{ struct Cyc_List_List* _temp1159=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1069->pat_vars))->v;
in_flow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow, _temp1159); in_flow=
Cyc_NewControlFlow_add_init_vars_flow( env, in_flow, _temp1159); _temp1072=(
struct Cyc_Absyn_Stmt*) _temp1069->body; goto _LL996;} _LL996: _temp1074=
_temp1072; goto _LL998; _LL998: _temp1076= _temp1074; goto _LL1000; _LL1000: Cyc_NewControlFlow_update_flow((
struct Cyc_Absyn_Stmt*) _check_null( _temp1076), in_flow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1002: in_flow= Cyc_CfAbsexp_eval_absexp( env, _temp1080->loc,( void*)((
struct Cyc_List_List*) _check_null( _temp970))->hd, in_flow); Cyc_NewControlFlow_cf_analyze_switch_clauses(
env, _temp1078,(( struct Cyc_List_List*) _check_null( _temp970))->tl, in_flow);
return( void*) Cyc_CfFlowInfo_BottomFL; _LL1004: { struct Cyc_NewControlFlow_AnalEnv
_temp1162; void* _temp1163; int _temp1165; struct Cyc_NewControlFlow_AnalEnv*
_temp1160= env; _temp1162=* _temp1160; _LL1166: _temp1165= _temp1162.in_try;
goto _LL1164; _LL1164: _temp1163=( void*) _temp1162.tryflow; goto _LL1161;
_LL1161: env->in_try= 1;( void*)( env->tryflow=( void*) in_flow);{ void*
_temp1167= Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1084, in_flow); void*
_temp1168=( void*) env->tryflow; env->in_try= _temp1165;( void*)( env->tryflow=(
void*) _temp1163); Cyc_NewControlFlow_update_tryflow( env, _temp1168); Cyc_NewControlFlow_cf_analyze_switch_clauses(
env, _temp1082, _temp970, _temp1168); return _temp1167;}} _LL1006: { struct Cyc_List_List
_temp1169=({ struct Cyc_List_List _temp1172; _temp1172.hd=( void*) _temp1093;
_temp1172.tl= 0; _temp1172;}); void* _temp1170= Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow,( struct Cyc_List_List*)& _temp1169); struct Cyc_Absyn_Exp*
_temp1171= _temp1093->initializer; if( _temp1171 != 0){ _temp1170= Cyc_CfAbsexp_eval_absexp(
env,(( struct Cyc_Absyn_Exp*) _check_null( _temp1171))->loc,( void*)(( struct
Cyc_List_List*) _check_null( _temp970))->hd, _temp1170);} return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1086, _temp1170);} _LL1008: { void* _temp1173= Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow, _temp1107); _temp1173= Cyc_CfAbsexp_eval_absexp( env, _temp1102->loc,(
void*)(( struct Cyc_List_List*) _check_null( _temp970))->hd, _temp1173);
_temp1173= Cyc_NewControlFlow_add_init_vars_flow( env, _temp1173, _temp1107);
return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1095, _temp1173);} _LL1010: {
void* _temp1174= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow, _temp1116);
return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1109, _temp1174);} _LL1012:
return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1118, in_flow); _LL1014: {
struct Cyc_List_List* _temp1175=({ struct Cyc_List_List* _temp1177=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1177->hd=( void*) _temp1122;
_temp1177->tl= 0; _temp1177;}); void* _temp1176= Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow, _temp1175); _temp1176= Cyc_NewControlFlow_add_init_vars_flow( env,
_temp1176, _temp1175); return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1120,
_temp1176);} _LL1016:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1178=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1178[ 0]=({ struct Cyc_Core_Impossible_struct _temp1179; _temp1179.tag= Cyc_Core_Impossible;
_temp1179.f1= _tag_arr("cf_analyze_stmt -- bad stmt syntax or unimplemented stmt form",
sizeof( unsigned char), 62u); _temp1179;}); _temp1178;})); _LL972:;}} void Cyc_NewControlFlow_cf_postpass(
struct Cyc_Absyn_Stmt* s){( void*)( s->annot=( void*)(( void*) Cyc_Absyn_EmptyAnnot));{
void* _temp1180=( void*) s->r; struct Cyc_Absyn_Stmt* _temp1204; struct Cyc_Absyn_Stmt*
_temp1206; struct Cyc_Absyn_Stmt* _temp1208; struct Cyc_Absyn_Stmt* _temp1210;
struct Cyc_Absyn_Stmt* _temp1212; struct _tuple3 _temp1214; struct Cyc_Absyn_Stmt*
_temp1216; struct _tuple3 _temp1218; struct Cyc_Absyn_Stmt* _temp1220; struct
Cyc_Absyn_Stmt* _temp1222; struct _tuple3 _temp1224; struct Cyc_Absyn_Stmt*
_temp1226; struct _tuple3 _temp1228; struct Cyc_Absyn_Stmt* _temp1230; struct
Cyc_Absyn_Stmt* _temp1232; struct Cyc_Absyn_Stmt* _temp1234; struct Cyc_Absyn_Stmt*
_temp1236; struct Cyc_Absyn_Stmt* _temp1238; struct Cyc_List_List* _temp1240;
struct Cyc_Absyn_Stmt* _temp1242; struct Cyc_List_List* _temp1244; _LL1182: if((
unsigned int) _temp1180 > 1u?*(( int*) _temp1180) == Cyc_Absyn_Seq_s: 0){
_LL1207: _temp1206=(( struct Cyc_Absyn_Seq_s_struct*) _temp1180)->f1; goto
_LL1205; _LL1205: _temp1204=(( struct Cyc_Absyn_Seq_s_struct*) _temp1180)->f2;
goto _LL1183;} else{ goto _LL1184;} _LL1184: if(( unsigned int) _temp1180 > 1u?*((
int*) _temp1180) == Cyc_Absyn_IfThenElse_s: 0){ _LL1211: _temp1210=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1180)->f2; goto _LL1209; _LL1209: _temp1208=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1180)->f3; goto _LL1185;} else{ goto _LL1186;} _LL1186: if(( unsigned int)
_temp1180 > 1u?*(( int*) _temp1180) == Cyc_Absyn_For_s: 0){ _LL1219: _temp1218=((
struct Cyc_Absyn_For_s_struct*) _temp1180)->f2; _LL1221: _temp1220= _temp1218.f2;
goto _LL1215; _LL1215: _temp1214=(( struct Cyc_Absyn_For_s_struct*) _temp1180)->f3;
_LL1217: _temp1216= _temp1214.f2; goto _LL1213; _LL1213: _temp1212=(( struct Cyc_Absyn_For_s_struct*)
_temp1180)->f4; goto _LL1187;} else{ goto _LL1188;} _LL1188: if(( unsigned int)
_temp1180 > 1u?*(( int*) _temp1180) == Cyc_Absyn_While_s: 0){ _LL1225: _temp1224=((
struct Cyc_Absyn_While_s_struct*) _temp1180)->f1; _LL1227: _temp1226= _temp1224.f2;
goto _LL1223; _LL1223: _temp1222=(( struct Cyc_Absyn_While_s_struct*) _temp1180)->f2;
goto _LL1189;} else{ goto _LL1190;} _LL1190: if(( unsigned int) _temp1180 > 1u?*((
int*) _temp1180) == Cyc_Absyn_Do_s: 0){ _LL1233: _temp1232=(( struct Cyc_Absyn_Do_s_struct*)
_temp1180)->f1; goto _LL1229; _LL1229: _temp1228=(( struct Cyc_Absyn_Do_s_struct*)
_temp1180)->f2; _LL1231: _temp1230= _temp1228.f2; goto _LL1191;} else{ goto
_LL1192;} _LL1192: if(( unsigned int) _temp1180 > 1u?*(( int*) _temp1180) == Cyc_Absyn_Region_s:
0){ _LL1235: _temp1234=(( struct Cyc_Absyn_Region_s_struct*) _temp1180)->f3;
goto _LL1193;} else{ goto _LL1194;} _LL1194: if(( unsigned int) _temp1180 > 1u?*((
int*) _temp1180) == Cyc_Absyn_Label_s: 0){ _LL1237: _temp1236=(( struct Cyc_Absyn_Label_s_struct*)
_temp1180)->f2; goto _LL1195;} else{ goto _LL1196;} _LL1196: if(( unsigned int)
_temp1180 > 1u?*(( int*) _temp1180) == Cyc_Absyn_Decl_s: 0){ _LL1239: _temp1238=((
struct Cyc_Absyn_Decl_s_struct*) _temp1180)->f2; goto _LL1197;} else{ goto
_LL1198;} _LL1198: if(( unsigned int) _temp1180 > 1u?*(( int*) _temp1180) == Cyc_Absyn_TryCatch_s:
0){ _LL1243: _temp1242=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp1180)->f1;
goto _LL1241; _LL1241: _temp1240=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1180)->f2; goto _LL1199;} else{ goto _LL1200;} _LL1200: if(( unsigned int)
_temp1180 > 1u?*(( int*) _temp1180) == Cyc_Absyn_Switch_s: 0){ _LL1245:
_temp1244=(( struct Cyc_Absyn_Switch_s_struct*) _temp1180)->f2; goto _LL1201;}
else{ goto _LL1202;} _LL1202: goto _LL1203; _LL1183: _temp1210= _temp1206;
_temp1208= _temp1204; goto _LL1185; _LL1185: Cyc_NewControlFlow_cf_postpass(
_temp1210); Cyc_NewControlFlow_cf_postpass( _temp1208); return; _LL1187: Cyc_NewControlFlow_cf_postpass(
_temp1216); _temp1226= _temp1212; _temp1222= _temp1220; goto _LL1189; _LL1189:
_temp1232= _temp1222; _temp1230= _temp1226; goto _LL1191; _LL1191: Cyc_NewControlFlow_cf_postpass(
_temp1230); _temp1234= _temp1232; goto _LL1193; _LL1193: _temp1236= _temp1234;
goto _LL1195; _LL1195: _temp1238= _temp1236; goto _LL1197; _LL1197: Cyc_NewControlFlow_cf_postpass(
_temp1238); return; _LL1199: Cyc_NewControlFlow_cf_postpass( _temp1242);
_temp1244= _temp1240; goto _LL1201; _LL1201: for( 0; _temp1244 != 0; _temp1244=((
struct Cyc_List_List*) _check_null( _temp1244))->tl){ Cyc_NewControlFlow_cf_postpass(((
struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( _temp1244))->hd)->body);}
return; _LL1203: return; _LL1181:;}} void Cyc_NewControlFlow_cf_check_fun(
struct Cyc_Absyn_Fndecl* fd){ struct Cyc_NewControlFlow_AbsEnv absenv=({ struct
Cyc_NewControlFlow_AbsEnv _temp1267; _temp1267.shared=({ struct Cyc_NewControlFlow_SharedAbsEnv*
_temp1268=( struct Cyc_NewControlFlow_SharedAbsEnv*) GC_malloc( sizeof( struct
Cyc_NewControlFlow_SharedAbsEnv)); _temp1268->all_roots= Cyc_Dict_empty( Cyc_CfFlowInfo_local_root_cmp);
_temp1268->comprehension_vars= 0; _temp1268;}); _temp1267.lhsides= 0; _temp1267.access_path=
0; _temp1267;}); struct Cyc_List_List* _temp1246=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( fd->param_vardecls))->v;{ struct Cyc_List_List*
vds= _temp1246; for( 0; vds != 0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){
Cyc_NewControlFlow_add_var_root( absenv,( struct Cyc_Absyn_Vardecl*)(( struct
Cyc_List_List*) _check_null( vds))->hd);}} Cyc_NewControlFlow_cf_prepass( absenv,
fd->body);{ struct Cyc_NewControlFlow_SharedAbsEnv* _temp1249; struct Cyc_NewControlFlow_SharedAbsEnv
_temp1251; struct Cyc_List_List* _temp1252; struct Cyc_Dict_Dict* _temp1254;
struct Cyc_NewControlFlow_AbsEnv _temp1247= absenv; _LL1250: _temp1249=
_temp1247.shared; _temp1251=* _temp1249; _LL1255: _temp1254= _temp1251.all_roots;
goto _LL1253; _LL1253: _temp1252= _temp1251.comprehension_vars; goto _LL1248;
_LL1248: { struct Cyc_NewControlFlow_AnalEnv* _temp1256=({ struct Cyc_NewControlFlow_AnalEnv*
_temp1266=( struct Cyc_NewControlFlow_AnalEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_AnalEnv));
_temp1266->roots= _temp1254; _temp1266->in_try= 0; _temp1266->tryflow=( void*)((
void*) Cyc_CfFlowInfo_BottomFL); _temp1266;}); void* in_flow=( void*)({ struct
Cyc_CfFlowInfo_InitsFL_struct* _temp1264=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1264[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp1265; _temp1265.tag= Cyc_CfFlowInfo_InitsFL;
_temp1265.f1= Cyc_Dict_empty( Cyc_CfFlowInfo_local_root_cmp); _temp1265;});
_temp1264;}); in_flow= Cyc_NewControlFlow_add_init_vars_flow( _temp1256, in_flow,
_temp1246); in_flow= Cyc_NewControlFlow_add_vardecls_flow( _temp1256, in_flow,
_temp1252);{ void* out_flow= in_flow; Cyc_NewControlFlow_iteration_num= 0; Cyc_NewControlFlow_iterate_cf_check=
1; while( Cyc_NewControlFlow_iterate_cf_check) { ++ Cyc_NewControlFlow_iteration_num;
Cyc_NewControlFlow_iterate_cf_check= 0; out_flow= Cyc_NewControlFlow_cf_analyze_stmt(
_temp1256, fd->body, in_flow);}{ void* _temp1257= out_flow; _LL1259: if(
_temp1257 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1260;} else{ goto _LL1261;}
_LL1261: goto _LL1262; _LL1260: goto _LL1258; _LL1262: if(( void*) fd->ret_type
!=( void*) Cyc_Absyn_VoidType){({ void* _temp1263[ 0u]={}; Cyc_Tcutil_terr(( fd->body)->loc,
_tag_arr("function may complete without returning a value", sizeof(
unsigned char), 48u), _tag_arr( _temp1263, sizeof( void*), 0u));});} goto
_LL1258; _LL1258:;} Cyc_NewControlFlow_cf_postpass( fd->body);}}}} void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds){ for( 0; ds != 0; ds=(( struct Cyc_List_List*)
_check_null( ds))->tl){ void* _temp1269=( void*)(( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( ds))->hd)->r; struct Cyc_Absyn_Fndecl*
_temp1281; struct Cyc_List_List* _temp1283; struct Cyc_List_List* _temp1285;
struct Cyc_List_List* _temp1287; _LL1271: if(*(( int*) _temp1269) == Cyc_Absyn_Fn_d){
_LL1282: _temp1281=(( struct Cyc_Absyn_Fn_d_struct*) _temp1269)->f1; goto
_LL1272;} else{ goto _LL1273;} _LL1273: if(*(( int*) _temp1269) == Cyc_Absyn_ExternC_d){
_LL1284: _temp1283=(( struct Cyc_Absyn_ExternC_d_struct*) _temp1269)->f1; goto
_LL1274;} else{ goto _LL1275;} _LL1275: if(*(( int*) _temp1269) == Cyc_Absyn_Using_d){
_LL1286: _temp1285=(( struct Cyc_Absyn_Using_d_struct*) _temp1269)->f2; goto
_LL1276;} else{ goto _LL1277;} _LL1277: if(*(( int*) _temp1269) == Cyc_Absyn_Namespace_d){
_LL1288: _temp1287=(( struct Cyc_Absyn_Namespace_d_struct*) _temp1269)->f2; goto
_LL1278;} else{ goto _LL1279;} _LL1279: goto _LL1280; _LL1272: Cyc_NewControlFlow_cf_check_fun(
_temp1281); goto _LL1270; _LL1274: _temp1285= _temp1283; goto _LL1276; _LL1276:
_temp1287= _temp1285; goto _LL1278; _LL1278: Cyc_NewControlFlow_cf_check(
_temp1287); goto _LL1270; _LL1280: goto _LL1270; _LL1270:;}}
