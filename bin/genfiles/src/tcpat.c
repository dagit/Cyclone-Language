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
Cyc_List_List* tl; } ; extern int Cyc_List_length( struct Cyc_List_List* x);
extern unsigned char Cyc_List_List_empty[ 15u]; extern struct Cyc_List_List* Cyc_List_map(
void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap(
struct _RegionHandle*, void*(* f)( void*), struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_map_c( void*(* f)( void*, void*), void* env,
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap_c( struct
_RegionHandle*, void*(* f)( void*, void*), void* env, struct Cyc_List_List* x);
extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append( struct
Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_rappend(
struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y); extern
unsigned char Cyc_List_Nth[ 8u]; extern struct Cyc_List_List* Cyc_List_rzip(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x,
struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_tabulate_c( int
n, void*(* f)( void*, int), void* env); struct Cyc_Dict_Dict; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
struct Cyc_Set_Set; extern struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*,
void*)); extern struct Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void*
elt); extern int Cyc_Set_cardinality( struct Cyc_Set_Set* s); extern int Cyc_Set_member(
struct Cyc_Set_Set* s, void* elt); extern unsigned char Cyc_Set_Absent[ 11u];
struct Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[ 19u];
extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
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
int tag; float* f1; } ; extern int Cyc_String_strcmp( struct _tagged_arr s1,
struct _tagged_arr s2); extern int Cyc_String_zstrcmp( struct _tagged_arr,
struct _tagged_arr); extern int Cyc_String_zstrptrcmp( struct _tagged_arr*,
struct _tagged_arr*); struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file;
struct _tagged_arr line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static
const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct
Cyc_Position_Error{ struct _tagged_arr source; struct Cyc_Position_Segment* seg;
void* kind; struct _tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[
14u]; struct _tuple1{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar;
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
0; struct Cyc_Absyn_Tvar{ struct _tagged_arr* name; int* identity; struct Cyc_Absyn_Conref*
kind; } ; static const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b=
0; struct Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
struct Cyc_Absyn_PtrInfo{ void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref*
nullable; struct Cyc_Absyn_Tqual tq; struct Cyc_Absyn_Conref* bounds; } ; struct
Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt* name; struct Cyc_Absyn_Tqual tq; void*
type; void* rgn; int inject; } ; struct Cyc_Absyn_FnInfo{ struct Cyc_List_List*
tvars; struct Cyc_Core_Opt* effect; void* ret_typ; struct Cyc_List_List* args;
int c_varargs; struct Cyc_Absyn_VarargInfo* cyc_varargs; struct Cyc_List_List*
rgn_po; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{
struct _tuple1* name; int is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion=
0; struct Cyc_Absyn_UnknownTunion_struct{ int tag; struct Cyc_Absyn_UnknownTunionInfo
f1; } ; static const int Cyc_Absyn_KnownTunion= 1; struct Cyc_Absyn_KnownTunion_struct{
int tag; struct Cyc_Absyn_Tuniondecl* f1; } ; struct Cyc_Absyn_TunionInfo{ void*
tunion_info; struct Cyc_List_List* targs; void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{
struct _tuple1* tunion_name; struct _tuple1* field_name; int is_xtunion; } ;
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
Cyc_Absyn_Tqual tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List*
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
15u]; extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref( void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_new_evar( struct Cyc_Core_Opt* k, struct Cyc_Core_Opt*
tenv); extern void* Cyc_Absyn_wildtyp( struct Cyc_Core_Opt*); extern void* Cyc_Absyn_uchar_t;
extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_float_t;
extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple1* x, void*
t, struct Cyc_Absyn_Exp* init); struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct
Cyc_PP_Doc; struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int qvar_to_Cids:
1; int add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int
rewrite_temp_tvars: 1; int print_all_tvars: 1; int print_all_kinds: 1; int
print_using_stmts: 1; int print_externC_stmts: 1; int print_full_evars: 1; int
use_curr_namespace: 1; struct Cyc_List_List* curr_namespace; } ; extern struct
_tagged_arr Cyc_Absynpp_typ2string( void*); extern struct _tagged_arr Cyc_Absynpp_kind2string(
void*); extern struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple1*);
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
extern void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*);
extern void Cyc_Tcenv_check_rgn_accessible( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
void* rgn); extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos(
struct _tagged_arr fmt, struct _tagged_arr ap); extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap);
extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*, struct _tagged_arr
fmt, struct _tagged_arr ap); extern void* Cyc_Tcutil_compress( void* t); extern
int Cyc_Tcutil_coerceable( void*); extern int Cyc_Tcutil_unify( void*, void*);
extern void* Cyc_Tcutil_rsubstitute( struct _RegionHandle*, struct Cyc_List_List*,
void*); struct _tuple4{ struct Cyc_List_List* f1; struct _RegionHandle* f2; } ;
struct _tuple5{ struct Cyc_Absyn_Tvar* f1; void* f2; } ; extern struct _tuple5*
Cyc_Tcutil_r_make_inst_var( struct _tuple4*, struct Cyc_Absyn_Tvar*); extern
void Cyc_Tcutil_check_unique_vars( struct Cyc_List_List* vs, struct Cyc_Position_Segment*
loc, struct _tagged_arr err_msg); extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields); struct _tuple6{ struct Cyc_List_List* f1;
struct Cyc_List_List* f2; } ; extern struct _tuple6 Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p, void** topt, void** region_opt); extern void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*, struct Cyc_List_List*); struct _tuple7{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Pat* f2; } ; static void Cyc_Tcpat_resolve_pat( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p){ void* _temp0=( void*) p->r; struct _tuple1*
_temp10; struct Cyc_List_List* _temp12; struct Cyc_List_List* _temp14; struct
_tuple1* _temp16; struct Cyc_List_List* _temp18; struct Cyc_List_List* _temp20;
struct _tuple1* _temp22; _LL2: if(( unsigned int) _temp0 > 2u?*(( int*) _temp0)
== Cyc_Absyn_UnknownId_p: 0){ _LL11: _temp10=(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp0)->f1; goto _LL3;} else{ goto _LL4;} _LL4: if(( unsigned int) _temp0 > 2u?*((
int*) _temp0) == Cyc_Absyn_UnknownCall_p: 0){ _LL17: _temp16=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp0)->f1; goto _LL15; _LL15: _temp14=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp0)->f2; goto _LL13; _LL13: _temp12=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp0)->f3; goto _LL5;} else{ goto _LL6;} _LL6: if(( unsigned int) _temp0 > 2u?*((
int*) _temp0) == Cyc_Absyn_UnknownFields_p: 0){ _LL23: _temp22=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp0)->f1; goto _LL21; _LL21: _temp20=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp0)->f2; goto _LL19; _LL19: _temp18=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp0)->f3; goto _LL7;} else{ goto _LL8;} _LL8: goto _LL9; _LL3:{ struct
_handler_cons _temp24; _push_handler(& _temp24);{ int _temp26= 0; if( setjmp(
_temp24.handler)){ _temp26= 1;} if( ! _temp26){{ void* _temp27= Cyc_Tcenv_lookup_ordinary(
te, p->loc, _temp10); struct Cyc_Absyn_Tunionfield* _temp37; struct Cyc_Absyn_Tuniondecl*
_temp39; struct Cyc_Absyn_Enumfield* _temp41; struct Cyc_Absyn_Enumdecl* _temp43;
_LL29: if(*(( int*) _temp27) == Cyc_Tcenv_StructRes){ goto _LL30;} else{ goto
_LL31;} _LL31: if(*(( int*) _temp27) == Cyc_Tcenv_TunionRes){ _LL40: _temp39=((
struct Cyc_Tcenv_TunionRes_struct*) _temp27)->f1; goto _LL38; _LL38: _temp37=((
struct Cyc_Tcenv_TunionRes_struct*) _temp27)->f2; goto _LL32;} else{ goto _LL33;}
_LL33: if(*(( int*) _temp27) == Cyc_Tcenv_EnumRes){ _LL44: _temp43=(( struct Cyc_Tcenv_EnumRes_struct*)
_temp27)->f1; goto _LL42; _LL42: _temp41=(( struct Cyc_Tcenv_EnumRes_struct*)
_temp27)->f2; goto _LL34;} else{ goto _LL35;} _LL35: if(*(( int*) _temp27) ==
Cyc_Tcenv_VarRes){ goto _LL36;} else{ goto _LL28;} _LL30:({ void(* _temp45)(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp48= p->loc; struct
_tagged_arr _temp49= _tag_arr("struct tag used without arguments in pattern",
sizeof( unsigned char), 45u); void* _temp46[ 0u]={}; struct _tagged_arr _temp47={(
void*) _temp46,( void*) _temp46,( void*)( _temp46 + 0u)}; _temp45( _temp48,
_temp49, _temp47);}); _npop_handler( 0u); return; _LL32:( void*)( p->r=( void*)((
void*)({ struct Cyc_Absyn_Tunion_p_struct* _temp50=( struct Cyc_Absyn_Tunion_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct)); _temp50[ 0]=({ struct Cyc_Absyn_Tunion_p_struct
_temp51; _temp51.tag= Cyc_Absyn_Tunion_p; _temp51.f1= _temp39; _temp51.f2=
_temp37; _temp51.f3= 0; _temp51.f4= 0; _temp51;}); _temp50;}))); _npop_handler(
0u); return; _LL34:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_p_struct*
_temp52=( struct Cyc_Absyn_Enum_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_p_struct));
_temp52[ 0]=({ struct Cyc_Absyn_Enum_p_struct _temp53; _temp53.tag= Cyc_Absyn_Enum_p;
_temp53.f1= _temp43; _temp53.f2= _temp41; _temp53;}); _temp52;})));
_npop_handler( 0u); return; _LL36: goto _LL28; _LL28:;}; _pop_handler();} else{
void* _temp25=( void*) _exn_thrown; void* _temp55= _temp25; _LL57: if( _temp55
== Cyc_Dict_Absent){ goto _LL58;} else{ goto _LL59;} _LL59: goto _LL60; _LL58:
goto _LL56; _LL60:( void) _throw( _temp55); _LL56:;}}}{ void* _temp61=(* _temp10).f1;
struct Cyc_List_List* _temp69; _LL63: if( _temp61 ==( void*) Cyc_Absyn_Loc_n){
goto _LL64;} else{ goto _LL65;} _LL65: if(( unsigned int) _temp61 > 1u?*(( int*)
_temp61) == Cyc_Absyn_Rel_n: 0){ _LL70: _temp69=(( struct Cyc_Absyn_Rel_n_struct*)
_temp61)->f1; if( _temp69 == 0){ goto _LL66;} else{ goto _LL67;}} else{ goto
_LL67;} _LL67: goto _LL68; _LL64: goto _LL66; _LL66:(* _temp10).f1=( void*) Cyc_Absyn_Loc_n;(
void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct* _temp71=( struct
Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp71[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp72; _temp72.tag= Cyc_Absyn_Var_p;
_temp72.f1= Cyc_Absyn_new_vardecl( _temp10,( void*) Cyc_Absyn_VoidType, 0);
_temp72;}); _temp71;}))); return; _LL68:({ void(* _temp73)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp76= p->loc; struct _tagged_arr _temp77= _tag_arr("qualified variable in pattern",
sizeof( unsigned char), 30u); void* _temp74[ 0u]={}; struct _tagged_arr _temp75={(
void*) _temp74,( void*) _temp74,( void*)( _temp74 + 0u)}; _temp73( _temp76,
_temp77, _temp75);}); return; _LL62:;} _LL5:{ struct _handler_cons _temp78;
_push_handler(& _temp78);{ int _temp80= 0; if( setjmp( _temp78.handler)){
_temp80= 1;} if( ! _temp80){{ void* _temp81= Cyc_Tcenv_lookup_ordinary( te, p->loc,
_temp16); struct Cyc_Absyn_Structdecl* _temp91; struct Cyc_Absyn_Tunionfield*
_temp93; struct Cyc_Absyn_Tuniondecl* _temp95; _LL83: if(*(( int*) _temp81) ==
Cyc_Tcenv_StructRes){ _LL92: _temp91=(( struct Cyc_Tcenv_StructRes_struct*)
_temp81)->f1; goto _LL84;} else{ goto _LL85;} _LL85: if(*(( int*) _temp81) ==
Cyc_Tcenv_TunionRes){ _LL96: _temp95=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp81)->f1; goto _LL94; _LL94: _temp93=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp81)->f2; goto _LL86;} else{ goto _LL87;} _LL87: if(*(( int*) _temp81) ==
Cyc_Tcenv_EnumRes){ goto _LL88;} else{ goto _LL89;} _LL89: if(*(( int*) _temp81)
== Cyc_Tcenv_VarRes){ goto _LL90;} else{ goto _LL82;} _LL84: { struct Cyc_List_List*
_temp97= 0; for( 0; _temp12 != 0; _temp12=(( struct Cyc_List_List*) _check_null(
_temp12))->tl){ _temp97=({ struct Cyc_List_List* _temp98=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp98->hd=( void*)({ struct _tuple7*
_temp99=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp99->f1= 0;
_temp99->f2=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null(
_temp12))->hd; _temp99;}); _temp98->tl= _temp97; _temp98;});}( void*)( p->r=(
void*)(( void*)({ struct Cyc_Absyn_Struct_p_struct* _temp100=( struct Cyc_Absyn_Struct_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct)); _temp100[ 0]=({ struct
Cyc_Absyn_Struct_p_struct _temp101; _temp101.tag= Cyc_Absyn_Struct_p; _temp101.f1=
_temp91; _temp101.f2= 0; _temp101.f3= _temp14; _temp101.f4=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp97); _temp101;}); _temp100;})));
_npop_handler( 0u); return;} _LL86:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_p_struct*
_temp102=( struct Cyc_Absyn_Tunion_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct));
_temp102[ 0]=({ struct Cyc_Absyn_Tunion_p_struct _temp103; _temp103.tag= Cyc_Absyn_Tunion_p;
_temp103.f1= _temp95; _temp103.f2= _temp93; _temp103.f3= _temp14; _temp103.f4=
_temp12; _temp103;}); _temp102;}))); _npop_handler( 0u); return; _LL88:({ void(*
_temp104)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp108= p->loc;
struct _tagged_arr _temp109= _tag_arr("bad enum constructor %s in pattern",
sizeof( unsigned char), 35u); struct Cyc_Stdio_String_pa_struct _temp110;
_temp110.tag= Cyc_Stdio_String_pa; _temp110.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp16);{ void* _temp107=( void*)& _temp110; void* _temp105[ 1u]={ _temp107};
struct _tagged_arr _temp106={( void*) _temp105,( void*) _temp105,( void*)(
_temp105 + 1u)}; _temp104( _temp108, _temp109, _temp106);}}); _npop_handler( 0u);
return; _LL90: goto _LL82; _LL82:;}; _pop_handler();} else{ void* _temp79=( void*)
_exn_thrown; void* _temp112= _temp79; _LL114: if( _temp112 == Cyc_Dict_Absent){
goto _LL115;} else{ goto _LL116;} _LL116: goto _LL117; _LL115: goto _LL113;
_LL117:( void) _throw( _temp112); _LL113:;}}}({ void(* _temp118)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp122= p->loc; struct _tagged_arr _temp123= _tag_arr("%s is not a constructor in pattern",
sizeof( unsigned char), 35u); struct Cyc_Stdio_String_pa_struct _temp124;
_temp124.tag= Cyc_Stdio_String_pa; _temp124.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp16);{ void* _temp121=( void*)& _temp124; void* _temp119[ 1u]={ _temp121};
struct _tagged_arr _temp120={( void*) _temp119,( void*) _temp119,( void*)(
_temp119 + 1u)}; _temp118( _temp122, _temp123, _temp120);}}); return; _LL7:{
struct _handler_cons _temp125; _push_handler(& _temp125);{ int _temp127= 0; if(
setjmp( _temp125.handler)){ _temp127= 1;} if( ! _temp127){{ void* _temp128= Cyc_Tcenv_lookup_ordinary(
te, p->loc, _temp22); struct Cyc_Absyn_Structdecl* _temp134; _LL130: if(*(( int*)
_temp128) == Cyc_Tcenv_StructRes){ _LL135: _temp134=(( struct Cyc_Tcenv_StructRes_struct*)
_temp128)->f1; goto _LL131;} else{ goto _LL132;} _LL132: goto _LL133; _LL131:(
void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_p_struct* _temp136=(
struct Cyc_Absyn_Struct_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct));
_temp136[ 0]=({ struct Cyc_Absyn_Struct_p_struct _temp137; _temp137.tag= Cyc_Absyn_Struct_p;
_temp137.f1= _temp134; _temp137.f2= 0; _temp137.f3= _temp20; _temp137.f4=
_temp18; _temp137;}); _temp136;}))); _npop_handler( 0u); return; _LL133: goto
_LL129; _LL129:;}; _pop_handler();} else{ void* _temp126=( void*) _exn_thrown;
void* _temp139= _temp126; _LL141: if( _temp139 == Cyc_Dict_Absent){ goto _LL142;}
else{ goto _LL143;} _LL143: goto _LL144; _LL142: goto _LL140; _LL144:( void)
_throw( _temp139); _LL140:;}}}({ void(* _temp145)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp149= p->loc; struct _tagged_arr _temp150= _tag_arr("%s is not a struct name, but pattern has designators",
sizeof( unsigned char), 53u); struct Cyc_Stdio_String_pa_struct _temp151;
_temp151.tag= Cyc_Stdio_String_pa; _temp151.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp22);{ void* _temp148=( void*)& _temp151; void* _temp146[ 1u]={ _temp148};
struct _tagged_arr _temp147={( void*) _temp146,( void*) _temp146,( void*)(
_temp146 + 1u)}; _temp145( _temp149, _temp150, _temp147);}}); return; _LL9:
return; _LL1:;} static struct _tagged_arr* Cyc_Tcpat_get_name( struct Cyc_Absyn_Vardecl*
vd){ return(* vd->name).f2;} static struct Cyc_Core_Opt Cyc_Tcpat_mk={( void*)((
void*) 1u)}; static struct Cyc_Core_Opt Cyc_Tcpat_rk={( void*)(( void*) 3u)};
static struct Cyc_Core_Opt Cyc_Tcpat_ak={( void*)(( void*) 0u)}; static void*
Cyc_Tcpat_any_type( struct Cyc_List_List* s, void** topt){ if( topt != 0){
return*(( void**) _check_null( topt));} return Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcpat_mk,({ struct Cyc_Core_Opt* _temp152=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp152->v=( void*) s; _temp152;}));} static
void* Cyc_Tcpat_num_type( void** topt, void* numt){ if( topt != 0? Cyc_Tcutil_coerceable(*((
void**) _check_null( topt))): 0){ return*(( void**) _check_null( topt));}{ void*
_temp153= Cyc_Tcutil_compress( numt); _LL155: if(( unsigned int) _temp153 > 4u?*((
int*) _temp153) == Cyc_Absyn_EnumType: 0){ goto _LL156;} else{ goto _LL157;}
_LL157: goto _LL158; _LL156: if( topt != 0){ return*(( void**) _check_null( topt));}
goto _LL154; _LL158: goto _LL154; _LL154:;} return numt;} static void Cyc_Tcpat_set_vd(
struct Cyc_Absyn_Vardecl* vd, struct Cyc_List_List** v_result_ptr, void* t){(
void*)( vd->type=( void*) t); vd->tq= Cyc_Absyn_empty_tqual();* v_result_ptr=({
struct Cyc_List_List* _temp159=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp159->hd=( void*) vd; _temp159->tl=* v_result_ptr;
_temp159;});} struct _tuple8{ struct Cyc_Absyn_Tqual f1; void* f2; } ; struct
_tuple9{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Pat* f2; } ; struct
_tuple6 Cyc_Tcpat_tcPatRec( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p,
void** topt, void** region_opt){ Cyc_Tcpat_resolve_pat( te, p);{ void* t; struct
Cyc_List_List* tv_result= 0; struct Cyc_List_List* v_result= 0;{ void* _temp160=(
void*) p->r; struct Cyc_Absyn_Vardecl* _temp194; struct Cyc_Absyn_Vardecl*
_temp196; void* _temp198; void* _temp200; struct Cyc_Absyn_Enumfield* _temp202;
struct Cyc_Absyn_Enumdecl* _temp204; struct Cyc_Absyn_Pat* _temp206; struct Cyc_List_List*
_temp208; struct Cyc_List_List* _temp210; struct Cyc_List_List* _temp212; struct
Cyc_Core_Opt* _temp214; struct Cyc_Core_Opt** _temp216; struct Cyc_Absyn_Structdecl*
_temp217; struct Cyc_List_List* _temp219; struct Cyc_List_List* _temp221; struct
Cyc_Absyn_Tunionfield* _temp223; struct Cyc_Absyn_Tuniondecl* _temp225; _LL162:
if( _temp160 ==( void*) Cyc_Absyn_Wild_p){ goto _LL163;} else{ goto _LL164;}
_LL164: if(( unsigned int) _temp160 > 2u?*(( int*) _temp160) == Cyc_Absyn_Var_p:
0){ _LL195: _temp194=(( struct Cyc_Absyn_Var_p_struct*) _temp160)->f1; goto
_LL165;} else{ goto _LL166;} _LL166: if(( unsigned int) _temp160 > 2u?*(( int*)
_temp160) == Cyc_Absyn_Reference_p: 0){ _LL197: _temp196=(( struct Cyc_Absyn_Reference_p_struct*)
_temp160)->f1; goto _LL167;} else{ goto _LL168;} _LL168: if(( unsigned int)
_temp160 > 2u?*(( int*) _temp160) == Cyc_Absyn_Int_p: 0){ _LL199: _temp198=(
void*)(( struct Cyc_Absyn_Int_p_struct*) _temp160)->f1; if( _temp198 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL169;} else{ goto _LL170;}} else{ goto _LL170;}
_LL170: if(( unsigned int) _temp160 > 2u?*(( int*) _temp160) == Cyc_Absyn_Int_p:
0){ _LL201: _temp200=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp160)->f1;
if( _temp200 ==( void*) Cyc_Absyn_Signed){ goto _LL171;} else{ goto _LL172;}}
else{ goto _LL172;} _LL172: if(( unsigned int) _temp160 > 2u?*(( int*) _temp160)
== Cyc_Absyn_Char_p: 0){ goto _LL173;} else{ goto _LL174;} _LL174: if((
unsigned int) _temp160 > 2u?*(( int*) _temp160) == Cyc_Absyn_Float_p: 0){ goto
_LL175;} else{ goto _LL176;} _LL176: if(( unsigned int) _temp160 > 2u?*(( int*)
_temp160) == Cyc_Absyn_Enum_p: 0){ _LL205: _temp204=(( struct Cyc_Absyn_Enum_p_struct*)
_temp160)->f1; goto _LL203; _LL203: _temp202=(( struct Cyc_Absyn_Enum_p_struct*)
_temp160)->f2; goto _LL177;} else{ goto _LL178;} _LL178: if( _temp160 ==( void*)
Cyc_Absyn_Null_p){ goto _LL179;} else{ goto _LL180;} _LL180: if(( unsigned int)
_temp160 > 2u?*(( int*) _temp160) == Cyc_Absyn_Pointer_p: 0){ _LL207: _temp206=((
struct Cyc_Absyn_Pointer_p_struct*) _temp160)->f1; goto _LL181;} else{ goto
_LL182;} _LL182: if(( unsigned int) _temp160 > 2u?*(( int*) _temp160) == Cyc_Absyn_Tuple_p:
0){ _LL209: _temp208=(( struct Cyc_Absyn_Tuple_p_struct*) _temp160)->f1; goto
_LL183;} else{ goto _LL184;} _LL184: if(( unsigned int) _temp160 > 2u?*(( int*)
_temp160) == Cyc_Absyn_Struct_p: 0){ _LL218: _temp217=(( struct Cyc_Absyn_Struct_p_struct*)
_temp160)->f1; goto _LL215; _LL215: _temp214=(( struct Cyc_Absyn_Struct_p_struct*)
_temp160)->f2; _temp216=&(( struct Cyc_Absyn_Struct_p_struct*) _temp160)->f2;
goto _LL213; _LL213: _temp212=(( struct Cyc_Absyn_Struct_p_struct*) _temp160)->f3;
goto _LL211; _LL211: _temp210=(( struct Cyc_Absyn_Struct_p_struct*) _temp160)->f4;
goto _LL185;} else{ goto _LL186;} _LL186: if(( unsigned int) _temp160 > 2u?*((
int*) _temp160) == Cyc_Absyn_Tunion_p: 0){ _LL226: _temp225=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp160)->f1; goto _LL224; _LL224: _temp223=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp160)->f2; goto _LL222; _LL222: _temp221=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp160)->f3; goto _LL220; _LL220: _temp219=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp160)->f4; goto _LL187;} else{ goto _LL188;} _LL188: if(( unsigned int)
_temp160 > 2u?*(( int*) _temp160) == Cyc_Absyn_UnknownId_p: 0){ goto _LL189;}
else{ goto _LL190;} _LL190: if(( unsigned int) _temp160 > 2u?*(( int*) _temp160)
== Cyc_Absyn_UnknownCall_p: 0){ goto _LL191;} else{ goto _LL192;} _LL192: if((
unsigned int) _temp160 > 2u?*(( int*) _temp160) == Cyc_Absyn_UnknownFields_p: 0){
goto _LL193;} else{ goto _LL161;} _LL163: t= Cyc_Tcpat_any_type( Cyc_Tcenv_lookup_type_vars(
te), topt); goto _LL161; _LL165: t= Cyc_Tcpat_any_type( Cyc_Tcenv_lookup_type_vars(
te), topt); Cyc_Tcpat_set_vd( _temp194,& v_result, t); goto _LL161; _LL167: t=
Cyc_Tcpat_any_type( Cyc_Tcenv_lookup_type_vars( te), topt); if( region_opt == 0){({
void(* _temp227)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp230= p->loc;
struct _tagged_arr _temp231= _tag_arr("* pattern would point into an unknown region",
sizeof( unsigned char), 45u); void* _temp228[ 0u]={}; struct _tagged_arr
_temp229={( void*) _temp228,( void*) _temp228,( void*)( _temp228 + 0u)};
_temp227( _temp230, _temp231, _temp229);}); goto _LL161;} Cyc_Tcpat_set_vd(
_temp196,& v_result,( void*)({ struct Cyc_Absyn_PointerType_struct* _temp232=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp232[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp233; _temp233.tag= Cyc_Absyn_PointerType;
_temp233.f1=({ struct Cyc_Absyn_PtrInfo _temp234; _temp234.elt_typ=( void*) t;
_temp234.rgn_typ=( void*)*(( void**) _check_null( region_opt)); _temp234.nullable=((
struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)( 0); _temp234.tq= Cyc_Absyn_empty_tqual();
_temp234.bounds= Cyc_Absyn_empty_conref(); _temp234;}); _temp233;}); _temp232;}));
goto _LL161; _LL169: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_uint_t); goto _LL161;
_LL171: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_sint_t); goto _LL161; _LL173: t=
Cyc_Tcpat_num_type( topt, Cyc_Absyn_uchar_t); goto _LL161; _LL175: t= Cyc_Tcpat_num_type(
topt, Cyc_Absyn_float_t); goto _LL161; _LL177: t= Cyc_Tcpat_num_type( topt,(
void*)({ struct Cyc_Absyn_EnumType_struct* _temp235=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp235[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp236; _temp236.tag= Cyc_Absyn_EnumType; _temp236.f1=
_temp204->name; _temp236.f2=( struct Cyc_Absyn_Enumdecl*) _temp204; _temp236;});
_temp235;})); goto _LL161; _LL179: if( topt != 0){ void* _temp237= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); _LL239: if(( unsigned int) _temp237 > 4u?*(( int*)
_temp237) == Cyc_Absyn_PointerType: 0){ goto _LL240;} else{ goto _LL241;} _LL241:
goto _LL242; _LL240: t=*(( void**) _check_null( topt)); goto tcpat_end; _LL242:
goto _LL238; _LL238:;}{ struct Cyc_List_List* _temp243= Cyc_Tcenv_lookup_type_vars(
te); t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp244=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp244[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp245; _temp245.tag= Cyc_Absyn_PointerType;
_temp245.f1=({ struct Cyc_Absyn_PtrInfo _temp246; _temp246.elt_typ=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcpat_ak,({ struct Cyc_Core_Opt* _temp248=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp248->v=( void*)
_temp243; _temp248;})); _temp246.rgn_typ=( void*) Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Tcpat_rk,({ struct Cyc_Core_Opt* _temp247=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp247->v=( void*) _temp243;
_temp247;})); _temp246.nullable=(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
1); _temp246.tq= Cyc_Absyn_empty_tqual(); _temp246.bounds= Cyc_Absyn_empty_conref();
_temp246;}); _temp245;}); _temp244;}); goto _LL161;} _LL181: { void* inner_typ=(
void*) Cyc_Absyn_VoidType; void** _temp249= 0; if( topt != 0){ void* _temp250=
Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo
_temp256; void* _temp258; _LL252: if(( unsigned int) _temp250 > 4u?*(( int*)
_temp250) == Cyc_Absyn_PointerType: 0){ _LL257: _temp256=(( struct Cyc_Absyn_PointerType_struct*)
_temp250)->f1; _LL259: _temp258=( void*) _temp256.elt_typ; goto _LL253;} else{
goto _LL254;} _LL254: goto _LL255; _LL253: inner_typ= _temp258; _temp249=( void**)&
inner_typ; goto _LL251; _LL255: goto _LL251; _LL251:;}{ void* ptr_rgn= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcpat_rk,({ struct Cyc_Core_Opt* _temp293=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp293->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp293;})); struct Cyc_List_List* _temp262;
struct Cyc_List_List* _temp264; struct _tuple6 _temp260= Cyc_Tcpat_tcPatRec( te,
_temp206, _temp249,( void**)& ptr_rgn); _LL265: _temp264= _temp260.f1; goto
_LL263; _LL263: _temp262= _temp260.f2; goto _LL261; _LL261: tv_result=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tv_result, _temp264); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( v_result, _temp262); if( _temp249
== 0){ void* _temp266= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp206->topt))->v); struct Cyc_Absyn_TunionFieldInfo _temp272;
struct Cyc_List_List* _temp274; void* _temp276; struct Cyc_Absyn_Tunionfield*
_temp278; struct Cyc_Absyn_Tuniondecl* _temp280; _LL268: if(( unsigned int)
_temp266 > 4u?*(( int*) _temp266) == Cyc_Absyn_TunionFieldType: 0){ _LL273:
_temp272=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp266)->f1; _LL277:
_temp276=( void*) _temp272.field_info; if(*(( int*) _temp276) == Cyc_Absyn_KnownTunionfield){
_LL281: _temp280=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp276)->f1;
goto _LL279; _LL279: _temp278=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp276)->f2; goto _LL275;} else{ goto _LL270;} _LL275: _temp274= _temp272.targs;
goto _LL269;} else{ goto _LL270;} _LL270: goto _LL271; _LL269: t=( void*)({
struct Cyc_Absyn_TunionType_struct* _temp282=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp282[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp283; _temp283.tag= Cyc_Absyn_TunionType;
_temp283.f1=({ struct Cyc_Absyn_TunionInfo _temp284; _temp284.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp285=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp285[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp286; _temp286.tag= Cyc_Absyn_KnownTunion;
_temp286.f1= _temp280; _temp286;}); _temp285;})); _temp284.targs= _temp274;
_temp284.rgn=( void*) ptr_rgn; _temp284;}); _temp283;}); _temp282;}); goto
_LL267; _LL271: t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp287=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp287[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp288; _temp288.tag= Cyc_Absyn_PointerType;
_temp288.f1=({ struct Cyc_Absyn_PtrInfo _temp289; _temp289.elt_typ=( void*)((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp206->topt))->v); _temp289.rgn_typ=(
void*) ptr_rgn; _temp289.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp289.tq= Cyc_Absyn_empty_tqual(); _temp289.bounds= Cyc_Absyn_empty_conref();
_temp289;}); _temp288;}); _temp287;}); goto _LL267; _LL267:;} else{ t=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp290=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp290[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp291; _temp291.tag= Cyc_Absyn_PointerType;
_temp291.f1=({ struct Cyc_Absyn_PtrInfo _temp292; _temp292.elt_typ=( void*)((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp206->topt))->v); _temp292.rgn_typ=(
void*) ptr_rgn; _temp292.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp292.tq= Cyc_Absyn_empty_tqual(); _temp292.bounds= Cyc_Absyn_empty_conref();
_temp292;}); _temp291;}); _temp290;});} goto _LL161;}} _LL183: { struct Cyc_List_List*
pat_ts= 0; struct Cyc_List_List* topt_ts= 0; if( topt != 0){ void* _temp294= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_List_List* _temp300; _LL296: if((
unsigned int) _temp294 > 4u?*(( int*) _temp294) == Cyc_Absyn_TupleType: 0){
_LL301: _temp300=(( struct Cyc_Absyn_TupleType_struct*) _temp294)->f1; goto
_LL297;} else{ goto _LL298;} _LL298: goto _LL299; _LL297: topt_ts= _temp300;
goto _LL295; _LL299: goto _LL295; _LL295:;} for( 0; _temp208 != 0; _temp208=((
struct Cyc_List_List*) _check_null( _temp208))->tl){ void** _temp302= 0; if(
topt_ts != 0){ _temp302=( void**)&(*(( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( topt_ts))->hd)).f2; topt_ts=(( struct Cyc_List_List*) _check_null(
topt_ts))->tl;}{ struct Cyc_List_List* _temp305; struct Cyc_List_List* _temp307;
struct _tuple6 _temp303= Cyc_Tcpat_tcPatRec( te,( struct Cyc_Absyn_Pat*)((
struct Cyc_List_List*) _check_null( _temp208))->hd, _temp302, region_opt);
_LL308: _temp307= _temp303.f1; goto _LL306; _LL306: _temp305= _temp303.f2; goto
_LL304; _LL304: tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp307); v_result=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
v_result, _temp305); pat_ts=({ struct Cyc_List_List* _temp309=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp309->hd=( void*)({ struct
_tuple8* _temp310=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8));
_temp310->f1= Cyc_Absyn_empty_tqual(); _temp310->f2=( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null(
_temp208))->hd)->topt))->v; _temp310;}); _temp309->tl= pat_ts; _temp309;});}} t=(
void*)({ struct Cyc_Absyn_TupleType_struct* _temp311=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp311[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp312; _temp312.tag= Cyc_Absyn_TupleType; _temp312.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( pat_ts);
_temp312;}); _temp311;}); goto _LL161;} _LL185: if( _temp212 != 0){( int) _throw(({
void*(* _temp313)( struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_impos;
struct _tagged_arr _temp316= _tag_arr("tcPat: struct<...> not implemented",
sizeof( unsigned char), 35u); void* _temp314[ 0u]={}; struct _tagged_arr
_temp315={( void*) _temp314,( void*) _temp314,( void*)( _temp314 + 0u)};
_temp313( _temp316, _temp315);}));}{ struct _RegionHandle _temp317= _new_region();
struct _RegionHandle* rgn=& _temp317; _push_region( rgn);{ struct _tuple4
_temp318=({ struct _tuple4 _temp355; _temp355.f1= Cyc_Tcenv_lookup_type_vars( te);
_temp355.f2= rgn; _temp355;}); struct Cyc_List_List* _temp319=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp318, _temp217->tvs); struct Cyc_List_List* _temp320=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp319); t=( void*)({ struct Cyc_Absyn_StructType_struct*
_temp321=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp321[ 0]=({ struct Cyc_Absyn_StructType_struct _temp322; _temp322.tag= Cyc_Absyn_StructType;
_temp322.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp217->name))->v); _temp322.f2= _temp320; _temp322.f3=({ struct
Cyc_Absyn_Structdecl** _temp323=( struct Cyc_Absyn_Structdecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl*)); _temp323[ 0]= _temp217; _temp323;});
_temp322;}); _temp321;}); if( topt != 0){ Cyc_Tcutil_unify( t,*(( void**)
_check_null( topt)));}* _temp216=({ struct Cyc_Core_Opt* _temp324=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp324->v=( void*) _temp320;
_temp324;}); if( _temp217->fields == 0){({ void(* _temp325)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp328= p->loc; struct _tagged_arr _temp329= _tag_arr("can't destructure an abstract struct",
sizeof( unsigned char), 37u); void* _temp326[ 0u]={}; struct _tagged_arr
_temp327={( void*) _temp326,( void*) _temp326,( void*)( _temp326 + 0u)};
_temp325( _temp328, _temp329, _temp327);}); _npop_handler( 0u); goto _LL161;}{
struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)( struct _RegionHandle*
rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_List_List*
fields)) Cyc_Tcutil_resolve_struct_designators)( rgn, p->loc, _temp210,( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp217->fields))->v);
for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){
struct _tuple9 _temp332; struct Cyc_Absyn_Pat* _temp333; struct Cyc_Absyn_Structfield*
_temp335; struct _tuple9* _temp330=( struct _tuple9*)(( struct Cyc_List_List*)
_check_null( fields))->hd; _temp332=* _temp330; _LL336: _temp335= _temp332.f1;
goto _LL334; _LL334: _temp333= _temp332.f2; goto _LL331; _LL331: { void*
_temp337= Cyc_Tcutil_rsubstitute( rgn, _temp319,( void*) _temp335->type); struct
Cyc_List_List* _temp340; struct Cyc_List_List* _temp342; struct _tuple6 _temp338=
Cyc_Tcpat_tcPatRec( te, _temp333,( void**)& _temp337, region_opt); _LL343:
_temp342= _temp338.f1; goto _LL341; _LL341: _temp340= _temp338.f2; goto _LL339;
_LL339: tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp342); v_result=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
v_result, _temp340); if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp333->topt))->v, _temp337)){({ void(* _temp344)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp350= p->loc; struct _tagged_arr _temp351= _tag_arr("field %s of struct pattern expects type %s != %s",
sizeof( unsigned char), 49u); struct Cyc_Stdio_String_pa_struct _temp354;
_temp354.tag= Cyc_Stdio_String_pa; _temp354.f1=( struct _tagged_arr)* _temp335->name;{
void* _temp347=( void*)& _temp354; struct Cyc_Stdio_String_pa_struct _temp353;
_temp353.tag= Cyc_Stdio_String_pa; _temp353.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp337);{ void* _temp348=( void*)& _temp353; struct Cyc_Stdio_String_pa_struct
_temp352; _temp352.tag= Cyc_Stdio_String_pa; _temp352.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp333->topt))->v);{
void* _temp349=( void*)& _temp352; void* _temp345[ 3u]={ _temp347, _temp348,
_temp349}; struct _tagged_arr _temp346={( void*) _temp345,( void*) _temp345,(
void*)( _temp345 + 3u)}; _temp344( _temp350, _temp351, _temp346);}}}});}}}}};
_pop_region( rgn);} goto _LL161; _LL187: if((( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp221) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp223->tvs)){({ void(* _temp356)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp359= p->loc; struct _tagged_arr _temp360= _tag_arr("wrong number of existential type variables in pattern",
sizeof( unsigned char), 54u); void* _temp357[ 0u]={}; struct _tagged_arr
_temp358={( void*) _temp357,( void*) _temp357,( void*)( _temp357 + 0u)};
_temp356( _temp359, _temp360, _temp358);});}{ struct _RegionHandle _temp361=
_new_region(); struct _RegionHandle* rgn=& _temp361; _push_region( rgn);{ struct
Cyc_List_List* _temp362= 0;{ struct Cyc_List_List* t= _temp221; for( 0; t != 0;
t=(( struct Cyc_List_List*) _check_null( t))->tl){ struct Cyc_Absyn_Tvar* tv=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( t))->hd;{ void*
_temp363=( void*)( Cyc_Absyn_compress_conref( tv->kind))->v; void* _temp373;
void* _temp375; _LL365: if(( unsigned int) _temp363 > 1u?*(( int*) _temp363) ==
Cyc_Absyn_Eq_constr: 0){ _LL374: _temp373=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp363)->f1; if( _temp373 ==( void*) Cyc_Absyn_BoxKind){ goto _LL366;} else{
goto _LL367;}} else{ goto _LL367;} _LL367: if(( unsigned int) _temp363 > 1u?*((
int*) _temp363) == Cyc_Absyn_Eq_constr: 0){ _LL376: _temp375=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp363)->f1; goto _LL368;} else{ goto _LL369;}
_LL369: if( _temp363 ==( void*) Cyc_Absyn_No_constr){ goto _LL370;} else{ goto
_LL371;} _LL371: if(( unsigned int) _temp363 > 1u?*(( int*) _temp363) == Cyc_Absyn_Forward_constr:
0){ goto _LL372;} else{ goto _LL364;} _LL366: goto _LL364; _LL368:({ void(*
_temp377)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp383= p->loc;
struct _tagged_arr _temp384= _tag_arr("tunion member %s abstracts type variable %s of kind %s != B",
sizeof( unsigned char), 60u); struct Cyc_Stdio_String_pa_struct _temp387;
_temp387.tag= Cyc_Stdio_String_pa; _temp387.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp225->name);{ void* _temp380=( void*)& _temp387; struct Cyc_Stdio_String_pa_struct
_temp386; _temp386.tag= Cyc_Stdio_String_pa; _temp386.f1=( struct _tagged_arr)*
tv->name;{ void* _temp381=( void*)& _temp386; struct Cyc_Stdio_String_pa_struct
_temp385; _temp385.tag= Cyc_Stdio_String_pa; _temp385.f1=( struct _tagged_arr)
Cyc_Absynpp_kind2string( _temp375);{ void* _temp382=( void*)& _temp385; void*
_temp378[ 3u]={ _temp380, _temp381, _temp382}; struct _tagged_arr _temp379={(
void*) _temp378,( void*) _temp378,( void*)( _temp378 + 3u)}; _temp377( _temp383,
_temp384, _temp379);}}}}); goto _LL364; _LL370:( void*)(( tv->kind)->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp388=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp388[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp389; _temp389.tag= Cyc_Absyn_Eq_constr; _temp389.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp389;}); _temp388;}))); goto _LL364;
_LL372:( int) _throw(({ void*(* _temp390)( struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_impos; struct _tagged_arr _temp393= _tag_arr("forward after compress in conref",
sizeof( unsigned char), 33u); void* _temp391[ 0u]={}; struct _tagged_arr
_temp392={( void*) _temp391,( void*) _temp391,( void*)( _temp391 + 0u)};
_temp390( _temp393, _temp392);})); _LL364:;} _temp362=({ struct Cyc_List_List*
_temp394=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp394->hd=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp395=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp395[ 0]=({ struct Cyc_Absyn_VarType_struct _temp396; _temp396.tag= Cyc_Absyn_VarType;
_temp396.f1= tv; _temp396;}); _temp395;})); _temp394->tl= _temp362; _temp394;});}}
_temp362= Cyc_List_imp_rev( _temp362); tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp221);{ struct Cyc_Tcenv_Tenv* te2= Cyc_Tcenv_add_type_vars( p->loc, te,
_temp221); struct Cyc_List_List* tqts= _temp223->typs; struct Cyc_List_List*
_temp397= Cyc_Tcenv_lookup_type_vars( te); struct _tuple4 _temp398=({ struct
_tuple4 _temp466; _temp466.f1= _temp397; _temp466.f2= rgn; _temp466;}); struct
Cyc_List_List* _temp399=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*), struct _tuple4*
env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp398, _temp225->tvs); struct Cyc_List_List* _temp400=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp223->tvs, _temp362);
struct Cyc_List_List* _temp401=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple5*))
Cyc_Core_snd, _temp399); struct Cyc_List_List* _temp402=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp400); struct Cyc_List_List* _temp403=((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp399, _temp400); if( tqts == 0){
t=( void*)({ struct Cyc_Absyn_TunionType_struct* _temp404=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp404[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp405; _temp405.tag= Cyc_Absyn_TunionType;
_temp405.f1=({ struct Cyc_Absyn_TunionInfo _temp406; _temp406.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp408=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp408[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp409; _temp409.tag= Cyc_Absyn_KnownTunion;
_temp409.f1= _temp225; _temp409;}); _temp408;})); _temp406.targs= _temp401;
_temp406.rgn=( void*) Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcpat_rk,({
struct Cyc_Core_Opt* _temp407=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp407->v=( void*) _temp397; _temp407;})); _temp406;});
_temp405;}); _temp404;});} else{ t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp410=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp410[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp411; _temp411.tag= Cyc_Absyn_TunionFieldType; _temp411.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp412; _temp412.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp413=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp413[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp414; _temp414.tag= Cyc_Absyn_KnownTunionfield; _temp414.f1= _temp225;
_temp414.f2= _temp223; _temp414;}); _temp413;})); _temp412.targs= _temp401;
_temp412;}); _temp411;}); _temp410;});} if( topt != 0? tqts == 0: 0){ void*
_temp415= Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_TunionInfo
_temp423; struct Cyc_List_List* _temp425; _LL417: if(( unsigned int) _temp415 >
4u?*(( int*) _temp415) == Cyc_Absyn_TunionFieldType: 0){ goto _LL418;} else{
goto _LL419;} _LL419: if(( unsigned int) _temp415 > 4u?*(( int*) _temp415) ==
Cyc_Absyn_TunionType: 0){ _LL424: _temp423=(( struct Cyc_Absyn_TunionType_struct*)
_temp415)->f1; _LL426: _temp425= _temp423.targs; goto _LL420;} else{ goto _LL421;}
_LL421: goto _LL422; _LL418: t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp427=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp427[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp428; _temp428.tag= Cyc_Absyn_TunionFieldType; _temp428.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp429; _temp429.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp430=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp430[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp431; _temp431.tag= Cyc_Absyn_KnownTunionfield; _temp431.f1= _temp225;
_temp431.f2= _temp223; _temp431;}); _temp430;})); _temp429.targs= _temp401;
_temp429;}); _temp428;}); _temp427;}); goto _LL416; _LL420: { struct Cyc_List_List*
_temp432= _temp401; for( 0; _temp432 != 0? _temp425 != 0: 0;( _temp432=(( struct
Cyc_List_List*) _check_null( _temp432))->tl, _temp425=(( struct Cyc_List_List*)
_check_null( _temp425))->tl)){ Cyc_Tcutil_unify(( void*)(( struct Cyc_List_List*)
_check_null( _temp432))->hd,( void*)(( struct Cyc_List_List*) _check_null(
_temp425))->hd);} goto _LL416;} _LL422: goto _LL416; _LL416:;} for( 0; _temp219
!= 0? tqts != 0: 0;( _temp219=(( struct Cyc_List_List*) _check_null( _temp219))->tl,
tqts=(( struct Cyc_List_List*) _check_null( tqts))->tl)){ struct Cyc_Absyn_Pat*
_temp433=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null( _temp219))->hd;
void* _temp434= Cyc_Tcutil_rsubstitute( rgn, _temp403,(*(( struct _tuple8*)((
struct Cyc_List_List*) _check_null( tqts))->hd)).f2); struct Cyc_List_List*
_temp437; struct Cyc_List_List* _temp439; struct _tuple6 _temp435= Cyc_Tcpat_tcPatRec(
te2, _temp433,( void**)& _temp434, region_opt); _LL440: _temp439= _temp435.f1;
goto _LL438; _LL438: _temp437= _temp435.f2; goto _LL436; _LL436: tv_result=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tv_result, _temp439); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( v_result, _temp437); if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp433->topt))->v, _temp434)){({
void(* _temp441)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp447=
_temp433->loc; struct _tagged_arr _temp448= _tag_arr("%s expects argument type %s, not %s",
sizeof( unsigned char), 36u); struct Cyc_Stdio_String_pa_struct _temp451;
_temp451.tag= Cyc_Stdio_String_pa; _temp451.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp225->name);{ void* _temp444=( void*)& _temp451; struct Cyc_Stdio_String_pa_struct
_temp450; _temp450.tag= Cyc_Stdio_String_pa; _temp450.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( _temp434);{ void* _temp445=( void*)& _temp450; struct
Cyc_Stdio_String_pa_struct _temp449; _temp449.tag= Cyc_Stdio_String_pa; _temp449.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp433->topt))->v);{ void* _temp446=( void*)& _temp449; void*
_temp442[ 3u]={ _temp444, _temp445, _temp446}; struct _tagged_arr _temp443={(
void*) _temp442,( void*) _temp442,( void*)( _temp442 + 3u)}; _temp441( _temp447,
_temp448, _temp443);}}}});}} if( _temp219 != 0){({ void(* _temp452)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp456= p->loc; struct _tagged_arr _temp457= _tag_arr("too many arguments for tunion constructor %s",
sizeof( unsigned char), 45u); struct Cyc_Stdio_String_pa_struct _temp458;
_temp458.tag= Cyc_Stdio_String_pa; _temp458.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp225->name);{ void* _temp455=( void*)& _temp458; void* _temp453[ 1u]={
_temp455}; struct _tagged_arr _temp454={( void*) _temp453,( void*) _temp453,(
void*)( _temp453 + 1u)}; _temp452( _temp456, _temp457, _temp454);}});} if( tqts
!= 0){({ void(* _temp459)( struct Cyc_Position_Segment*, struct _tagged_arr fmt,
struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp463=
p->loc; struct _tagged_arr _temp464= _tag_arr("too few arguments for tunion constructor %s",
sizeof( unsigned char), 44u); struct Cyc_Stdio_String_pa_struct _temp465;
_temp465.tag= Cyc_Stdio_String_pa; _temp465.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp225->name);{ void* _temp462=( void*)& _temp465; void* _temp460[ 1u]={
_temp462}; struct _tagged_arr _temp461={( void*) _temp460,( void*) _temp460,(
void*)( _temp460 + 1u)}; _temp459( _temp463, _temp464, _temp461);}});}}};
_pop_region( rgn);} goto _LL161; _LL189: goto _LL191; _LL191: goto _LL193;
_LL193: t= Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp467=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp467->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp467;})); goto _LL161; _LL161:;} tcpat_end: p->topt=({ struct Cyc_Core_Opt*
_temp468=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp468->v=( void*) t; _temp468;}); return({ struct _tuple6 _temp469; _temp469.f1=
tv_result; _temp469.f2= v_result; _temp469;});}} struct _tuple6 Cyc_Tcpat_tcPat(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p, void** topt, void**
region_opt){ struct _tuple6 _temp470= Cyc_Tcpat_tcPatRec( te, p, topt,
region_opt);{ struct _RegionHandle _temp471= _new_region(); struct _RegionHandle*
r=& _temp471; _push_region( r); Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tagged_arr*(* f)( struct Cyc_Absyn_Vardecl*),
struct Cyc_List_List* x)) Cyc_List_rmap)( r, Cyc_Tcpat_get_name, _temp470.f2), p->loc,
_tag_arr("pattern contains a repeated variable", sizeof( unsigned char), 37u));;
_pop_region( r);} return _temp470;} void Cyc_Tcpat_check_pat_regions( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p){ void* _temp472=( void*) p->r; struct Cyc_Absyn_Pat*
_temp484; struct Cyc_List_List* _temp486; struct Cyc_List_List* _temp488; struct
Cyc_List_List* _temp490; _LL474: if(( unsigned int) _temp472 > 2u?*(( int*)
_temp472) == Cyc_Absyn_Pointer_p: 0){ _LL485: _temp484=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp472)->f1; goto _LL475;} else{ goto _LL476;} _LL476: if(( unsigned int)
_temp472 > 2u?*(( int*) _temp472) == Cyc_Absyn_Tunion_p: 0){ _LL487: _temp486=((
struct Cyc_Absyn_Tunion_p_struct*) _temp472)->f4; goto _LL477;} else{ goto
_LL478;} _LL478: if(( unsigned int) _temp472 > 2u?*(( int*) _temp472) == Cyc_Absyn_Struct_p:
0){ _LL489: _temp488=(( struct Cyc_Absyn_Struct_p_struct*) _temp472)->f4; goto
_LL479;} else{ goto _LL480;} _LL480: if(( unsigned int) _temp472 > 2u?*(( int*)
_temp472) == Cyc_Absyn_Tuple_p: 0){ _LL491: _temp490=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp472)->f1; goto _LL481;} else{ goto _LL482;} _LL482: goto _LL483; _LL475:
Cyc_Tcpat_check_pat_regions( te, _temp484);{ void* _temp492=( void*)(( struct
Cyc_Core_Opt*) _check_null( p->topt))->v; struct Cyc_Absyn_PtrInfo _temp500;
void* _temp502; struct Cyc_Absyn_TunionInfo _temp504; void* _temp506; _LL494:
if(( unsigned int) _temp492 > 4u?*(( int*) _temp492) == Cyc_Absyn_PointerType: 0){
_LL501: _temp500=(( struct Cyc_Absyn_PointerType_struct*) _temp492)->f1; _LL503:
_temp502=( void*) _temp500.rgn_typ; goto _LL495;} else{ goto _LL496;} _LL496:
if(( unsigned int) _temp492 > 4u?*(( int*) _temp492) == Cyc_Absyn_TunionType: 0){
_LL505: _temp504=(( struct Cyc_Absyn_TunionType_struct*) _temp492)->f1; _LL507:
_temp506=( void*) _temp504.rgn; goto _LL497;} else{ goto _LL498;} _LL498: goto
_LL499; _LL495: _temp506= _temp502; goto _LL497; _LL497: Cyc_Tcenv_check_rgn_accessible(
te, p->loc, _temp506); return; _LL499:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp508=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp508[ 0]=({ struct Cyc_Core_Impossible_struct _temp509; _temp509.tag= Cyc_Core_Impossible;
_temp509.f1= _tag_arr("check_pat_regions: bad pointer type", sizeof(
unsigned char), 36u); _temp509;}); _temp508;})); _LL493:;} _LL477: for( 0;
_temp486 != 0; _temp486=(( struct Cyc_List_List*) _check_null( _temp486))->tl){
Cyc_Tcpat_check_pat_regions( te,( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp486))->hd);}{ void* _temp510=( void*)(( struct Cyc_Core_Opt*)
_check_null( p->topt))->v; struct Cyc_Absyn_TunionInfo _temp518; void* _temp520;
_LL512: if(( unsigned int) _temp510 > 4u?*(( int*) _temp510) == Cyc_Absyn_TunionType:
0){ _LL519: _temp518=(( struct Cyc_Absyn_TunionType_struct*) _temp510)->f1;
_LL521: _temp520=( void*) _temp518.rgn; goto _LL513;} else{ goto _LL514;} _LL514:
if(( unsigned int) _temp510 > 4u?*(( int*) _temp510) == Cyc_Absyn_TunionFieldType:
0){ goto _LL515;} else{ goto _LL516;} _LL516: goto _LL517; _LL513: Cyc_Tcenv_check_rgn_accessible(
te, p->loc, _temp520); return; _LL515: return; _LL517:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp522=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp522[ 0]=({ struct
Cyc_Core_Impossible_struct _temp523; _temp523.tag= Cyc_Core_Impossible; _temp523.f1=
_tag_arr("check_pat_regions: bad tunion type", sizeof( unsigned char), 35u);
_temp523;}); _temp522;})); _LL511:;} _LL479: _temp490=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Pat*(* f)( struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Pat*(*)( struct _tuple7*)) Cyc_Core_snd, _temp488); goto _LL481;
_LL481: for( 0; _temp490 != 0; _temp490=(( struct Cyc_List_List*) _check_null(
_temp490))->tl){ Cyc_Tcpat_check_pat_regions( te,( struct Cyc_Absyn_Pat*)((
struct Cyc_List_List*) _check_null( _temp490))->hd);} return; _LL483: return;
_LL473:;} static const int Cyc_Tcpat_Name_v= 0; struct Cyc_Tcpat_Name_v_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Tcpat_Int_v= 1; struct
Cyc_Tcpat_Int_v_struct{ int tag; int f1; } ; struct Cyc_Tcpat_Con_s{ void* name;
int arity; struct Cyc_Core_Opt* span; } ; static const int Cyc_Tcpat_Any= 0;
static const int Cyc_Tcpat_Con= 0; struct Cyc_Tcpat_Con_struct{ int tag; struct
Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2; } ; static int Cyc_Tcpat_compare_con(
struct Cyc_Tcpat_Con_s* c1, struct Cyc_Tcpat_Con_s* c2){ void* _temp524=( void*)
c1->name; struct _tagged_arr _temp530; int _temp532; _LL526: if(*(( int*)
_temp524) == Cyc_Tcpat_Name_v){ _LL531: _temp530=(( struct Cyc_Tcpat_Name_v_struct*)
_temp524)->f1; goto _LL527;} else{ goto _LL528;} _LL528: if(*(( int*) _temp524)
== Cyc_Tcpat_Int_v){ _LL533: _temp532=(( struct Cyc_Tcpat_Int_v_struct*)
_temp524)->f1; goto _LL529;} else{ goto _LL525;} _LL527: { void* _temp534=( void*)
c2->name; struct _tagged_arr _temp540; _LL536: if(*(( int*) _temp534) == Cyc_Tcpat_Name_v){
_LL541: _temp540=(( struct Cyc_Tcpat_Name_v_struct*) _temp534)->f1; goto _LL537;}
else{ goto _LL538;} _LL538: if(*(( int*) _temp534) == Cyc_Tcpat_Int_v){ goto
_LL539;} else{ goto _LL535;} _LL537: return Cyc_String_zstrcmp( _temp530,
_temp540); _LL539: return - 1; _LL535:;} _LL529: { void* _temp542=( void*) c2->name;
int _temp548; _LL544: if(*(( int*) _temp542) == Cyc_Tcpat_Name_v){ goto _LL545;}
else{ goto _LL546;} _LL546: if(*(( int*) _temp542) == Cyc_Tcpat_Int_v){ _LL549:
_temp548=(( struct Cyc_Tcpat_Int_v_struct*) _temp542)->f1; goto _LL547;} else{
goto _LL543;} _LL545: return 1; _LL547: return _temp532 - _temp548; _LL543:;}
_LL525:;} static struct Cyc_Set_Set* Cyc_Tcpat_empty_con_set(){ return(( struct
Cyc_Set_Set*(*)( int(* comp)( struct Cyc_Tcpat_Con_s*, struct Cyc_Tcpat_Con_s*)))
Cyc_Set_empty)( Cyc_Tcpat_compare_con);} static struct Cyc_Core_Opt Cyc_Tcpat_one_opt={(
void*) 1}; static struct Cyc_Core_Opt Cyc_Tcpat_two_opt={( void*) 2}; static
struct Cyc_Core_Opt* Cyc_Tcpat_one_opt_ptr=( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt;
static unsigned char _temp551[ 5u]="null"; static struct Cyc_Tcpat_Name_v_struct
Cyc_Tcpat_null_name_value={ 0u,{ _temp551, _temp551, _temp551 + 5u}}; static
unsigned char _temp553[ 4u]="ptr"; static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_ptr_name_value={
0u,{ _temp553, _temp553, _temp553 + 4u}}; static unsigned char _temp555[ 6u]="tuple";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={ 0u,{ _temp555,
_temp555, _temp555 + 6u}}; static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_con0={(
void*)(( void*)& Cyc_Tcpat_null_name_value), 0,( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt};
static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_ptr_con0={( void*)(( void*)& Cyc_Tcpat_ptr_name_value),
1,( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt}; static struct Cyc_Tcpat_Con_s Cyc_Tcpat_ptr_con0={(
void*)(( void*)& Cyc_Tcpat_ptr_name_value), 1,( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt};
static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_con=& Cyc_Tcpat_null_con0; static
struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_ptr_con=& Cyc_Tcpat_null_ptr_con0; static
struct Cyc_Tcpat_Con_s* Cyc_Tcpat_ptr_con=& Cyc_Tcpat_ptr_con0; static struct
Cyc_Tcpat_Con_s* Cyc_Tcpat_int_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp556=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp556->name=( void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp557=(
struct Cyc_Tcpat_Int_v_struct*) GC_malloc_atomic( sizeof( struct Cyc_Tcpat_Int_v_struct));
_temp557[ 0]=({ struct Cyc_Tcpat_Int_v_struct _temp558; _temp558.tag= Cyc_Tcpat_Int_v;
_temp558.f1= i; _temp558;}); _temp557;})); _temp556->arity= 0; _temp556->span= 0;
_temp556;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_float_con( struct
_tagged_arr f){ return({ struct Cyc_Tcpat_Con_s* _temp559=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp559->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp560=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp560[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp561; _temp561.tag= Cyc_Tcpat_Name_v; _temp561.f1= f; _temp561;}); _temp560;}));
_temp559->arity= 0; _temp559->span= 0; _temp559;});} static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_char_con( unsigned char c){ return({ struct Cyc_Tcpat_Con_s* _temp562=(
struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp562->name=(
void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp564=( struct Cyc_Tcpat_Int_v_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Tcpat_Int_v_struct)); _temp564[ 0]=({
struct Cyc_Tcpat_Int_v_struct _temp565; _temp565.tag= Cyc_Tcpat_Int_v; _temp565.f1=(
int) c; _temp565;}); _temp564;})); _temp562->arity= 0; _temp562->span=({ struct
Cyc_Core_Opt* _temp563=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct
Cyc_Core_Opt)); _temp563->v=( void*) 256; _temp563;}); _temp562;});} static
struct Cyc_Tcpat_Con_s* Cyc_Tcpat_tuple_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp566=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp566->name=( void*)(( void*)& Cyc_Tcpat_tuple_name_value); _temp566->arity=
i; _temp566->span= Cyc_Tcpat_one_opt_ptr; _temp566;});} static void* Cyc_Tcpat_null_pat(){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp567=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp567[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp568; _temp568.tag= Cyc_Tcpat_Con; _temp568.f1= Cyc_Tcpat_null_con; _temp568.f2=
0; _temp568;}); _temp567;});} static void* Cyc_Tcpat_int_pat( int i){ return(
void*)({ struct Cyc_Tcpat_Con_struct* _temp569=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp569[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp570; _temp570.tag= Cyc_Tcpat_Con; _temp570.f1= Cyc_Tcpat_int_con( i);
_temp570.f2= 0; _temp570;}); _temp569;});} static void* Cyc_Tcpat_char_pat(
unsigned char c){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp571=(
struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp571[ 0]=({ struct Cyc_Tcpat_Con_struct _temp572; _temp572.tag= Cyc_Tcpat_Con;
_temp572.f1= Cyc_Tcpat_char_con( c); _temp572.f2= 0; _temp572;}); _temp571;});}
static void* Cyc_Tcpat_float_pat( struct _tagged_arr f){ return( void*)({ struct
Cyc_Tcpat_Con_struct* _temp573=( struct Cyc_Tcpat_Con_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Con_struct)); _temp573[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp574; _temp574.tag= Cyc_Tcpat_Con; _temp574.f1= Cyc_Tcpat_float_con( f);
_temp574.f2= 0; _temp574;}); _temp573;});} static void* Cyc_Tcpat_null_ptr_pat(
void* p){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp575=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp575[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp576; _temp576.tag= Cyc_Tcpat_Con; _temp576.f1= Cyc_Tcpat_null_ptr_con;
_temp576.f2=({ struct Cyc_List_List* _temp577=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp577->hd=( void*) p; _temp577->tl= 0;
_temp577;}); _temp576;}); _temp575;});} static void* Cyc_Tcpat_ptr_pat( void* p){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp578=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp578[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp579; _temp579.tag= Cyc_Tcpat_Con; _temp579.f1= Cyc_Tcpat_ptr_con; _temp579.f2=({
struct Cyc_List_List* _temp580=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp580->hd=( void*) p; _temp580->tl= 0; _temp580;});
_temp579;}); _temp578;});} static void* Cyc_Tcpat_tuple_pat( struct Cyc_List_List*
ss){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp581=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp581[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp582; _temp582.tag= Cyc_Tcpat_Con; _temp582.f1= Cyc_Tcpat_tuple_con( Cyc_List_length(
ss)); _temp582.f2= ss; _temp582;}); _temp581;});} static void* Cyc_Tcpat_con_pat(
struct _tagged_arr con_name, struct Cyc_Core_Opt* span, struct Cyc_List_List* ps){
struct Cyc_Tcpat_Con_s* c=({ struct Cyc_Tcpat_Con_s* _temp585=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp585->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp586=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp586[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp587; _temp587.tag= Cyc_Tcpat_Name_v; _temp587.f1= con_name; _temp587;});
_temp586;})); _temp585->arity= Cyc_List_length( ps); _temp585->span= span;
_temp585;}); return( void*)({ struct Cyc_Tcpat_Con_struct* _temp583=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp583[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp584; _temp584.tag= Cyc_Tcpat_Con; _temp584.f1= c; _temp584.f2= ps; _temp584;});
_temp583;});} static void* Cyc_Tcpat_compile_pat( struct Cyc_Absyn_Pat* p){ void*
s;{ void* _temp588=( void*) p->r; int _temp622; void* _temp624; unsigned char
_temp626; struct _tagged_arr _temp628; struct Cyc_Absyn_Pat* _temp630; struct
Cyc_List_List* _temp632; struct Cyc_Absyn_Tunionfield* _temp634; struct Cyc_Absyn_Tuniondecl*
_temp636; struct Cyc_List_List* _temp638; struct Cyc_List_List* _temp640; struct
Cyc_List_List* _temp642; struct Cyc_Absyn_Structdecl* _temp644; struct Cyc_Absyn_Enumfield*
_temp646; struct Cyc_Absyn_Enumdecl* _temp648; _LL590: if( _temp588 ==( void*)
Cyc_Absyn_Wild_p){ goto _LL591;} else{ goto _LL592;} _LL592: if(( unsigned int)
_temp588 > 2u?*(( int*) _temp588) == Cyc_Absyn_Var_p: 0){ goto _LL593;} else{
goto _LL594;} _LL594: if( _temp588 ==( void*) Cyc_Absyn_Null_p){ goto _LL595;}
else{ goto _LL596;} _LL596: if(( unsigned int) _temp588 > 2u?*(( int*) _temp588)
== Cyc_Absyn_Int_p: 0){ _LL625: _temp624=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp588)->f1; goto _LL623; _LL623: _temp622=(( struct Cyc_Absyn_Int_p_struct*)
_temp588)->f2; goto _LL597;} else{ goto _LL598;} _LL598: if(( unsigned int)
_temp588 > 2u?*(( int*) _temp588) == Cyc_Absyn_Char_p: 0){ _LL627: _temp626=((
struct Cyc_Absyn_Char_p_struct*) _temp588)->f1; goto _LL599;} else{ goto _LL600;}
_LL600: if(( unsigned int) _temp588 > 2u?*(( int*) _temp588) == Cyc_Absyn_Float_p:
0){ _LL629: _temp628=(( struct Cyc_Absyn_Float_p_struct*) _temp588)->f1; goto
_LL601;} else{ goto _LL602;} _LL602: if(( unsigned int) _temp588 > 2u?*(( int*)
_temp588) == Cyc_Absyn_Reference_p: 0){ goto _LL603;} else{ goto _LL604;} _LL604:
if(( unsigned int) _temp588 > 2u?*(( int*) _temp588) == Cyc_Absyn_Pointer_p: 0){
_LL631: _temp630=(( struct Cyc_Absyn_Pointer_p_struct*) _temp588)->f1; goto
_LL605;} else{ goto _LL606;} _LL606: if(( unsigned int) _temp588 > 2u?*(( int*)
_temp588) == Cyc_Absyn_Tunion_p: 0){ _LL637: _temp636=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp588)->f1; goto _LL635; _LL635: _temp634=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp588)->f2; goto _LL633; _LL633: _temp632=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp588)->f4; if( _temp632 == 0){ goto _LL607;} else{ goto _LL608;}} else{ goto
_LL608;} _LL608: if(( unsigned int) _temp588 > 2u?*(( int*) _temp588) == Cyc_Absyn_Tunion_p:
0){ _LL639: _temp638=(( struct Cyc_Absyn_Tunion_p_struct*) _temp588)->f4; goto
_LL609;} else{ goto _LL610;} _LL610: if(( unsigned int) _temp588 > 2u?*(( int*)
_temp588) == Cyc_Absyn_Tuple_p: 0){ _LL641: _temp640=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp588)->f1; goto _LL611;} else{ goto _LL612;} _LL612: if(( unsigned int)
_temp588 > 2u?*(( int*) _temp588) == Cyc_Absyn_Struct_p: 0){ _LL645: _temp644=((
struct Cyc_Absyn_Struct_p_struct*) _temp588)->f1; goto _LL643; _LL643: _temp642=((
struct Cyc_Absyn_Struct_p_struct*) _temp588)->f4; goto _LL613;} else{ goto
_LL614;} _LL614: if(( unsigned int) _temp588 > 2u?*(( int*) _temp588) == Cyc_Absyn_Enum_p:
0){ _LL649: _temp648=(( struct Cyc_Absyn_Enum_p_struct*) _temp588)->f1; goto
_LL647; _LL647: _temp646=(( struct Cyc_Absyn_Enum_p_struct*) _temp588)->f2; goto
_LL615;} else{ goto _LL616;} _LL616: if(( unsigned int) _temp588 > 2u?*(( int*)
_temp588) == Cyc_Absyn_UnknownId_p: 0){ goto _LL617;} else{ goto _LL618;} _LL618:
if(( unsigned int) _temp588 > 2u?*(( int*) _temp588) == Cyc_Absyn_UnknownCall_p:
0){ goto _LL619;} else{ goto _LL620;} _LL620: if(( unsigned int) _temp588 > 2u?*((
int*) _temp588) == Cyc_Absyn_UnknownFields_p: 0){ goto _LL621;} else{ goto
_LL589;} _LL591: s=( void*) Cyc_Tcpat_Any; goto _LL589; _LL593: s=( void*) Cyc_Tcpat_Any;
goto _LL589; _LL595: s= Cyc_Tcpat_null_pat(); goto _LL589; _LL597: s= Cyc_Tcpat_int_pat(
_temp622); goto _LL589; _LL599: s= Cyc_Tcpat_char_pat( _temp626); goto _LL589;
_LL601: s= Cyc_Tcpat_float_pat( _temp628); goto _LL589; _LL603: s=( void*) Cyc_Tcpat_Any;
goto _LL589; _LL605:{ void* _temp650= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( p->topt))->v); struct Cyc_Absyn_PtrInfo _temp658; struct Cyc_Absyn_Conref*
_temp660; _LL652: if(( unsigned int) _temp650 > 4u?*(( int*) _temp650) == Cyc_Absyn_PointerType:
0){ _LL659: _temp658=(( struct Cyc_Absyn_PointerType_struct*) _temp650)->f1;
_LL661: _temp660= _temp658.nullable; goto _LL653;} else{ goto _LL654;} _LL654:
if(( unsigned int) _temp650 > 4u?*(( int*) _temp650) == Cyc_Absyn_TunionType: 0){
goto _LL655;} else{ goto _LL656;} _LL656: goto _LL657; _LL653: { int is_nullable=
0; int still_working= 1; while( still_working) { void* _temp662=( void*)
_temp660->v; struct Cyc_Absyn_Conref* _temp670; int _temp672; _LL664: if((
unsigned int) _temp662 > 1u?*(( int*) _temp662) == Cyc_Absyn_Forward_constr: 0){
_LL671: _temp670=(( struct Cyc_Absyn_Forward_constr_struct*) _temp662)->f1; goto
_LL665;} else{ goto _LL666;} _LL666: if( _temp662 ==( void*) Cyc_Absyn_No_constr){
goto _LL667;} else{ goto _LL668;} _LL668: if(( unsigned int) _temp662 > 1u?*((
int*) _temp662) == Cyc_Absyn_Eq_constr: 0){ _LL673: _temp672=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp662)->f1; goto _LL669;} else{ goto _LL663;} _LL665:( void*)( _temp660->v=(
void*)(( void*) _temp670->v)); continue; _LL667:( void*)( _temp660->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp674=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp674[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp675; _temp675.tag= Cyc_Absyn_Eq_constr; _temp675.f1=(
void*) 0; _temp675;}); _temp674;}))); is_nullable= 0; still_working= 0; goto
_LL663; _LL669: is_nullable=( int) _temp672; still_working= 0; goto _LL663;
_LL663:;}{ void* ss= Cyc_Tcpat_compile_pat( _temp630); if( is_nullable){ s= Cyc_Tcpat_null_ptr_pat(
ss);} else{ s= Cyc_Tcpat_ptr_pat( ss);} goto _LL651;}} _LL655:{ void* _temp676=(
void*) _temp630->r; struct Cyc_List_List* _temp682; struct Cyc_Absyn_Tunionfield*
_temp684; struct Cyc_Absyn_Tuniondecl* _temp686; _LL678: if(( unsigned int)
_temp676 > 2u?*(( int*) _temp676) == Cyc_Absyn_Tunion_p: 0){ _LL687: _temp686=((
struct Cyc_Absyn_Tunion_p_struct*) _temp676)->f1; goto _LL685; _LL685: _temp684=((
struct Cyc_Absyn_Tunion_p_struct*) _temp676)->f2; goto _LL683; _LL683: _temp682=((
struct Cyc_Absyn_Tunion_p_struct*) _temp676)->f4; goto _LL679;} else{ goto
_LL680;} _LL680: goto _LL681; _LL679: { struct Cyc_Core_Opt* span; if( _temp686->is_xtunion){
span= 0;} else{ span=({ struct Cyc_Core_Opt* _temp688=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp688->v=( void*)(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp686->fields))->v); _temp688;});} s= Cyc_Tcpat_con_pat(*(*
_temp684->name).f2, span,(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat, _temp682)); goto
_LL677;} _LL681: s=({ void*(* _temp689)( struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_impos; struct _tagged_arr _temp692= _tag_arr("non-[x]tunion pattern has tunion type",
sizeof( unsigned char), 38u); void* _temp690[ 0u]={}; struct _tagged_arr
_temp691={( void*) _temp690,( void*) _temp690,( void*)( _temp690 + 0u)};
_temp689( _temp692, _temp691);}); goto _LL677; _LL677:;} goto _LL651; _LL657: s=({
void*(* _temp693)( struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_impos;
struct _tagged_arr _temp696= _tag_arr("pointer pattern does not have pointer type!",
sizeof( unsigned char), 44u); void* _temp694[ 0u]={}; struct _tagged_arr
_temp695={( void*) _temp694,( void*) _temp694,( void*)( _temp694 + 0u)};
_temp693( _temp696, _temp695);}); goto _LL651; _LL651:;} goto _LL589; _LL607: {
struct Cyc_Core_Opt* span;{ void* _temp697= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( p->topt))->v); _LL699: if(( unsigned int)
_temp697 > 4u?*(( int*) _temp697) == Cyc_Absyn_TunionType: 0){ goto _LL700;}
else{ goto _LL701;} _LL701: if(( unsigned int) _temp697 > 4u?*(( int*) _temp697)
== Cyc_Absyn_TunionFieldType: 0){ goto _LL702;} else{ goto _LL703;} _LL703: goto
_LL704; _LL700: if( _temp636->is_xtunion){ span= 0;} else{ span=({ struct Cyc_Core_Opt*
_temp705=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct Cyc_Core_Opt));
_temp705->v=( void*)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp636->fields))->v);
_temp705;});} goto _LL698; _LL702: span=({ struct Cyc_Core_Opt* _temp706=(
struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp706->v=(
void*) 1; _temp706;}); goto _LL698; _LL704: span=({ struct Cyc_Core_Opt*(*
_temp707)( struct _tagged_arr fmt, struct _tagged_arr ap)=( struct Cyc_Core_Opt*(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos; struct
_tagged_arr _temp710= _tag_arr("void tunion pattern has bad type", sizeof(
unsigned char), 33u); void* _temp708[ 0u]={}; struct _tagged_arr _temp709={(
void*) _temp708,( void*) _temp708,( void*)( _temp708 + 0u)}; _temp707( _temp710,
_temp709);}); goto _LL698; _LL698:;} s= Cyc_Tcpat_con_pat(*(* _temp634->name).f2,
span, 0); goto _LL589;} _LL609: _temp640= _temp638; goto _LL611; _LL611: s= Cyc_Tcpat_tuple_pat(((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcpat_compile_pat, _temp640)); goto _LL589; _LL613: {
struct Cyc_List_List* ps= 0;{ struct Cyc_List_List* fields=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp644->fields))->v; for( 0; fields != 0;
fields=(( struct Cyc_List_List*) _check_null( fields))->tl){ int found= Cyc_String_strcmp(*((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( fields))->hd)->name,
_tag_arr("", sizeof( unsigned char), 1u)) == 0;{ struct Cyc_List_List* dlps0=
_temp642; for( 0; ! found? dlps0 != 0: 0; dlps0=(( struct Cyc_List_List*)
_check_null( dlps0))->tl){ struct _tuple7 _temp713; struct Cyc_Absyn_Pat*
_temp714; struct Cyc_List_List* _temp716; struct _tuple7* _temp711=( struct
_tuple7*)(( struct Cyc_List_List*) _check_null( dlps0))->hd; _temp713=* _temp711;
_LL717: _temp716= _temp713.f1; goto _LL715; _LL715: _temp714= _temp713.f2; goto
_LL712; _LL712: { struct Cyc_List_List* _temp718= _temp716; struct Cyc_List_List
_temp724; struct Cyc_List_List* _temp725; void* _temp727; struct _tagged_arr*
_temp729; _LL720: if( _temp718 == 0){ goto _LL722;} else{ _temp724=* _temp718;
_LL728: _temp727=( void*) _temp724.hd; if(*(( int*) _temp727) == Cyc_Absyn_FieldName){
_LL730: _temp729=(( struct Cyc_Absyn_FieldName_struct*) _temp727)->f1; goto
_LL726;} else{ goto _LL722;} _LL726: _temp725= _temp724.tl; if( _temp725 == 0){
goto _LL721;} else{ goto _LL722;}} _LL722: goto _LL723; _LL721: if( Cyc_String_zstrptrcmp(
_temp729,(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd)->name) == 0){ ps=({ struct Cyc_List_List* _temp731=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp731->hd=( void*) Cyc_Tcpat_compile_pat(
_temp714); _temp731->tl= ps; _temp731;}); found= 1;} goto _LL719; _LL723:({ int(*
_temp732)( struct _tagged_arr fmt, struct _tagged_arr ap)=( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos; struct _tagged_arr
_temp735= _tag_arr("bad designator(s)", sizeof( unsigned char), 18u); void*
_temp733[ 0u]={}; struct _tagged_arr _temp734={( void*) _temp733,( void*)
_temp733,( void*)( _temp733 + 0u)}; _temp732( _temp735, _temp734);}); goto
_LL719; _LL719:;}}} if( ! found){({ int(* _temp736)( struct _tagged_arr fmt,
struct _tagged_arr ap)=( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos; struct _tagged_arr _temp739= _tag_arr("bad designator",
sizeof( unsigned char), 15u); void* _temp737[ 0u]={}; struct _tagged_arr
_temp738={( void*) _temp737,( void*) _temp737,( void*)( _temp737 + 0u)};
_temp736( _temp739, _temp738);});}}} s= Cyc_Tcpat_tuple_pat( ps); goto _LL589;}
_LL615: { int span=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp648->fields))->v);
s= Cyc_Tcpat_con_pat(*(* _temp646->name).f2,({ struct Cyc_Core_Opt* _temp740=(
struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp740->v=(
void*) span; _temp740;}), 0); goto _LL589;} _LL617: s=({ void*(* _temp741)(
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_impos; struct
_tagged_arr _temp744= _tag_arr("compile_pat: unknownid", sizeof( unsigned char),
23u); void* _temp742[ 0u]={}; struct _tagged_arr _temp743={( void*) _temp742,(
void*) _temp742,( void*)( _temp742 + 0u)}; _temp741( _temp744, _temp743);});
goto _LL589; _LL619: s=({ void*(* _temp745)( struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_impos; struct _tagged_arr _temp748= _tag_arr("compile_pat: unknowncall",
sizeof( unsigned char), 25u); void* _temp746[ 0u]={}; struct _tagged_arr
_temp747={( void*) _temp746,( void*) _temp746,( void*)( _temp746 + 0u)};
_temp745( _temp748, _temp747);}); goto _LL589; _LL621: s=({ void*(* _temp749)(
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_impos; struct
_tagged_arr _temp752= _tag_arr("compile_pat: unknownfields", sizeof(
unsigned char), 27u); void* _temp750[ 0u]={}; struct _tagged_arr _temp751={(
void*) _temp750,( void*) _temp750,( void*)( _temp750 + 0u)}; _temp749( _temp752,
_temp751);}); goto _LL589; _LL589:;} return s;} static const int Cyc_Tcpat_Pos=
0; struct Cyc_Tcpat_Pos_struct{ int tag; struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Tcpat_Neg= 1; struct Cyc_Tcpat_Neg_struct{ int tag;
struct Cyc_Set_Set* f1; } ; static const int Cyc_Tcpat_Failure= 0; static const
int Cyc_Tcpat_Success= 0; struct Cyc_Tcpat_Success_struct{ int tag; void* f1; }
; static const int Cyc_Tcpat_IfEq= 1; struct Cyc_Tcpat_IfEq_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Tcpat_Con_s* f2; void* f3; void* f4; } ;
struct _tuple10{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; struct Cyc_List_List*
f3; } ; static const int Cyc_Tcpat_Yes= 0; static const int Cyc_Tcpat_No= 1;
static const int Cyc_Tcpat_Maybe= 2; static void* Cyc_Tcpat_add_neg( void* td,
struct Cyc_Tcpat_Con_s* c){ void* _temp753= td; struct Cyc_Set_Set* _temp759;
_LL755: if(*(( int*) _temp753) == Cyc_Tcpat_Neg){ _LL760: _temp759=(( struct Cyc_Tcpat_Neg_struct*)
_temp753)->f1; goto _LL756;} else{ goto _LL757;} _LL757: if(*(( int*) _temp753)
== Cyc_Tcpat_Pos){ goto _LL758;} else{ goto _LL754;} _LL756: if((( int(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)( _temp759,
c)){({ int(* _temp761)( struct _tagged_arr fmt, struct _tagged_arr ap)=( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos; struct
_tagged_arr _temp764= _tag_arr("add_neg called when constructor already in set",
sizeof( unsigned char), 47u); void* _temp762[ 0u]={}; struct _tagged_arr
_temp763={( void*) _temp762,( void*) _temp762,( void*)( _temp762 + 0u)};
_temp761( _temp764, _temp763);});} if( c->span != 0?(( int(*)( struct Cyc_Set_Set*
s)) Cyc_Set_cardinality)( _temp759) + 1 >=( int)(( struct Cyc_Core_Opt*)
_check_null( c->span))->v: 0){({ int(* _temp765)( struct _tagged_arr fmt, struct
_tagged_arr ap)=( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos;
struct _tagged_arr _temp768= _tag_arr("add_neg called when |cs U {c}| >= span(c)",
sizeof( unsigned char), 42u); void* _temp766[ 0u]={}; struct _tagged_arr
_temp767={( void*) _temp766,( void*) _temp766,( void*)( _temp766 + 0u)};
_temp765( _temp768, _temp767);});} return( void*)({ struct Cyc_Tcpat_Neg_struct*
_temp769=( struct Cyc_Tcpat_Neg_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct));
_temp769[ 0]=({ struct Cyc_Tcpat_Neg_struct _temp770; _temp770.tag= Cyc_Tcpat_Neg;
_temp770.f1=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s*
elt)) Cyc_Set_insert)( _temp759, c); _temp770;}); _temp769;}); _LL758: return({
void*(* _temp771)( struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_impos;
struct _tagged_arr _temp774= _tag_arr("add_neg called when td is Positive",
sizeof( unsigned char), 35u); void* _temp772[ 0u]={}; struct _tagged_arr
_temp773={( void*) _temp772,( void*) _temp772,( void*)( _temp772 + 0u)};
_temp771( _temp774, _temp773);}); _LL754:;} static void* Cyc_Tcpat_static_match(
struct Cyc_Tcpat_Con_s* c, void* td){ void* _temp775= td; struct Cyc_Tcpat_Con_s*
_temp781; struct Cyc_Set_Set* _temp783; _LL777: if(*(( int*) _temp775) == Cyc_Tcpat_Pos){
_LL782: _temp781=(( struct Cyc_Tcpat_Pos_struct*) _temp775)->f1; goto _LL778;}
else{ goto _LL779;} _LL779: if(*(( int*) _temp775) == Cyc_Tcpat_Neg){ _LL784:
_temp783=(( struct Cyc_Tcpat_Neg_struct*) _temp775)->f1; goto _LL780;} else{
goto _LL776;} _LL778: if( Cyc_Tcpat_compare_con( c, _temp781) == 0){ return(
void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_No;} _LL780: if((( int(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)( _temp783,
c)){ return( void*) Cyc_Tcpat_No;} else{ if( c->span != 0?( int)(( struct Cyc_Core_Opt*)
_check_null( c->span))->v ==(( int(*)( struct Cyc_Set_Set* s)) Cyc_Set_cardinality)(
_temp783) + 1: 0){ return( void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_Maybe;}}
_LL776:;} struct _tuple11{ struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2;
} ; static struct Cyc_List_List* Cyc_Tcpat_augment( struct Cyc_List_List* ctxt,
void* dsc){ struct Cyc_List_List* _temp785= ctxt; struct Cyc_List_List _temp791;
struct Cyc_List_List* _temp792; struct _tuple11* _temp794; struct _tuple11
_temp796; struct Cyc_List_List* _temp797; struct Cyc_Tcpat_Con_s* _temp799;
_LL787: if( _temp785 == 0){ goto _LL788;} else{ goto _LL789;} _LL789: if(
_temp785 == 0){ goto _LL786;} else{ _temp791=* _temp785; _LL795: _temp794=(
struct _tuple11*) _temp791.hd; _temp796=* _temp794; _LL800: _temp799= _temp796.f1;
goto _LL798; _LL798: _temp797= _temp796.f2; goto _LL793; _LL793: _temp792=
_temp791.tl; goto _LL790;} _LL788: return 0; _LL790: return({ struct Cyc_List_List*
_temp801=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp801->hd=( void*)({ struct _tuple11* _temp802=( struct _tuple11*) GC_malloc(
sizeof( struct _tuple11)); _temp802->f1= _temp799; _temp802->f2=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp803=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp803->hd=( void*) dsc; _temp803->tl= _temp797;
_temp803;}); _temp802;}); _temp801->tl= _temp792; _temp801;}); _LL786:;} static
struct Cyc_List_List* Cyc_Tcpat_norm_context( struct Cyc_List_List* ctxt){
struct Cyc_List_List* _temp804= ctxt; struct Cyc_List_List _temp810; struct Cyc_List_List*
_temp811; struct _tuple11* _temp813; struct _tuple11 _temp815; struct Cyc_List_List*
_temp816; struct Cyc_Tcpat_Con_s* _temp818; _LL806: if( _temp804 == 0){ goto
_LL807;} else{ goto _LL808;} _LL808: if( _temp804 == 0){ goto _LL805;} else{
_temp810=* _temp804; _LL814: _temp813=( struct _tuple11*) _temp810.hd; _temp815=*
_temp813; _LL819: _temp818= _temp815.f1; goto _LL817; _LL817: _temp816= _temp815.f2;
goto _LL812; _LL812: _temp811= _temp810.tl; goto _LL809;} _LL807: return({
struct Cyc_List_List*(* _temp820)( struct _tagged_arr fmt, struct _tagged_arr ap)=(
struct Cyc_List_List*(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos;
struct _tagged_arr _temp823= _tag_arr("norm_context: empty context", sizeof(
unsigned char), 28u); void* _temp821[ 0u]={}; struct _tagged_arr _temp822={(
void*) _temp821,( void*) _temp821,( void*)( _temp821 + 0u)}; _temp820( _temp823,
_temp822);}); _LL809: return Cyc_Tcpat_augment( _temp811,( void*)({ struct Cyc_Tcpat_Pos_struct*
_temp824=( struct Cyc_Tcpat_Pos_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Pos_struct));
_temp824[ 0]=({ struct Cyc_Tcpat_Pos_struct _temp825; _temp825.tag= Cyc_Tcpat_Pos;
_temp825.f1= _temp818; _temp825.f2= Cyc_List_rev( _temp816); _temp825;});
_temp824;})); _LL805:;} static void* Cyc_Tcpat_build_desc( struct Cyc_List_List*
ctxt, void* dsc, struct Cyc_List_List* work){ struct _tuple6 _temp827=({ struct
_tuple6 _temp826; _temp826.f1= ctxt; _temp826.f2= work; _temp826;}); struct Cyc_List_List*
_temp837; struct Cyc_List_List* _temp839; struct Cyc_List_List* _temp841; struct
Cyc_List_List* _temp843; struct Cyc_List_List* _temp845; struct Cyc_List_List
_temp847; struct Cyc_List_List* _temp848; struct _tuple10* _temp850; struct
_tuple10 _temp852; struct Cyc_List_List* _temp853; struct Cyc_List_List*
_temp855; struct Cyc_List_List _temp857; struct Cyc_List_List* _temp858; struct
_tuple11* _temp860; struct _tuple11 _temp862; struct Cyc_List_List* _temp863;
struct Cyc_Tcpat_Con_s* _temp865; _LL829: _LL840: _temp839= _temp827.f1; if(
_temp839 == 0){ goto _LL838;} else{ goto _LL831;} _LL838: _temp837= _temp827.f2;
if( _temp837 == 0){ goto _LL830;} else{ goto _LL831;} _LL831: _LL842: _temp841=
_temp827.f1; if( _temp841 == 0){ goto _LL832;} else{ goto _LL833;} _LL833:
_LL844: _temp843= _temp827.f2; if( _temp843 == 0){ goto _LL834;} else{ goto
_LL835;} _LL835: _LL856: _temp855= _temp827.f1; if( _temp855 == 0){ goto _LL828;}
else{ _temp857=* _temp855; _LL861: _temp860=( struct _tuple11*) _temp857.hd;
_temp862=* _temp860; _LL866: _temp865= _temp862.f1; goto _LL864; _LL864:
_temp863= _temp862.f2; goto _LL859; _LL859: _temp858= _temp857.tl; goto _LL846;}
_LL846: _temp845= _temp827.f2; if( _temp845 == 0){ goto _LL828;} else{ _temp847=*
_temp845; _LL851: _temp850=( struct _tuple10*) _temp847.hd; _temp852=* _temp850;
_LL854: _temp853= _temp852.f3; goto _LL849; _LL849: _temp848= _temp847.tl; goto
_LL836;} _LL830: return dsc; _LL832: return({ void*(* _temp867)( struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_impos; struct _tagged_arr
_temp870= _tag_arr("build_desc: ctxt and work don't match", sizeof(
unsigned char), 38u); void* _temp868[ 0u]={}; struct _tagged_arr _temp869={(
void*) _temp868,( void*) _temp868,( void*)( _temp868 + 0u)}; _temp867( _temp870,
_temp869);}); _LL834: return({ void*(* _temp871)( struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_impos; struct _tagged_arr _temp874= _tag_arr("build_desc: ctxt and work don't match",
sizeof( unsigned char), 38u); void* _temp872[ 0u]={}; struct _tagged_arr
_temp873={( void*) _temp872,( void*) _temp872,( void*)( _temp872 + 0u)};
_temp871( _temp874, _temp873);}); _LL836: { struct Cyc_Tcpat_Pos_struct*
_temp875=({ struct Cyc_Tcpat_Pos_struct* _temp876=( struct Cyc_Tcpat_Pos_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Pos_struct)); _temp876[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp877; _temp877.tag= Cyc_Tcpat_Pos; _temp877.f1= _temp865; _temp877.f2= Cyc_List_append(
Cyc_List_rev( _temp863),({ struct Cyc_List_List* _temp878=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp878->hd=( void*) dsc; _temp878->tl=
_temp853; _temp878;})); _temp877;}); _temp876;}); return Cyc_Tcpat_build_desc(
_temp858,( void*) _temp875, _temp848);} _LL828:;} static void* Cyc_Tcpat_match(
void* p, struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List* ctx, struct
Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List* rules); static
void* Cyc_Tcpat_or_match( void* dsc, struct Cyc_List_List* allmrules){ struct
Cyc_List_List* _temp879= allmrules; struct Cyc_List_List _temp885; struct Cyc_List_List*
_temp886; struct _tuple0* _temp888; struct _tuple0 _temp890; void* _temp891;
void* _temp893; _LL881: if( _temp879 == 0){ goto _LL882;} else{ goto _LL883;}
_LL883: if( _temp879 == 0){ goto _LL880;} else{ _temp885=* _temp879; _LL889:
_temp888=( struct _tuple0*) _temp885.hd; _temp890=* _temp888; _LL894: _temp893=
_temp890.f1; goto _LL892; _LL892: _temp891= _temp890.f2; goto _LL887; _LL887:
_temp886= _temp885.tl; goto _LL884;} _LL882: return( void*) Cyc_Tcpat_Failure;
_LL884: return Cyc_Tcpat_match( _temp893, 0, dsc, 0, 0, _temp891, _temp886);
_LL880:;} static void* Cyc_Tcpat_match_compile( struct Cyc_List_List* allmrules){
return Cyc_Tcpat_or_match(( void*)({ struct Cyc_Tcpat_Neg_struct* _temp895=(
struct Cyc_Tcpat_Neg_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct));
_temp895[ 0]=({ struct Cyc_Tcpat_Neg_struct _temp896; _temp896.tag= Cyc_Tcpat_Neg;
_temp896.f1= Cyc_Tcpat_empty_con_set(); _temp896;}); _temp895;}), allmrules);}
static void* Cyc_Tcpat_and_match( struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules){ struct Cyc_List_List*
_temp897= work; struct Cyc_List_List _temp905; struct Cyc_List_List* _temp906;
struct _tuple10* _temp908; struct _tuple10 _temp910; struct Cyc_List_List*
_temp911; struct Cyc_List_List* _temp913; struct Cyc_List_List* _temp915; struct
Cyc_List_List _temp917; struct Cyc_List_List* _temp918; struct _tuple10*
_temp920; struct _tuple10 _temp922; struct Cyc_List_List* _temp923; struct Cyc_List_List*
_temp925; struct Cyc_List_List* _temp927; _LL899: if( _temp897 == 0){ goto
_LL900;} else{ goto _LL901;} _LL901: if( _temp897 == 0){ goto _LL903;} else{
_temp905=* _temp897; _LL909: _temp908=( struct _tuple10*) _temp905.hd; _temp910=*
_temp908; _LL916: _temp915= _temp910.f1; if( _temp915 == 0){ goto _LL914;} else{
goto _LL903;} _LL914: _temp913= _temp910.f2; if( _temp913 == 0){ goto _LL912;}
else{ goto _LL903;} _LL912: _temp911= _temp910.f3; if( _temp911 == 0){ goto
_LL907;} else{ goto _LL903;} _LL907: _temp906= _temp905.tl; goto _LL902;} _LL903:
if( _temp897 == 0){ goto _LL898;} else{ _temp917=* _temp897; _LL921: _temp920=(
struct _tuple10*) _temp917.hd; _temp922=* _temp920; _LL928: _temp927= _temp922.f1;
goto _LL926; _LL926: _temp925= _temp922.f2; goto _LL924; _LL924: _temp923=
_temp922.f3; goto _LL919; _LL919: _temp918= _temp917.tl; goto _LL904;} _LL900:
return( void*)({ struct Cyc_Tcpat_Success_struct* _temp929=( struct Cyc_Tcpat_Success_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Success_struct)); _temp929[ 0]=({ struct Cyc_Tcpat_Success_struct
_temp930; _temp930.tag= Cyc_Tcpat_Success; _temp930.f1=( void*) right_hand_side;
_temp930;}); _temp929;}); _LL902: return Cyc_Tcpat_and_match( Cyc_Tcpat_norm_context(
ctx), _temp906, right_hand_side, rules); _LL904: if(( _temp927 == 0? 1: _temp925
== 0)? 1: _temp923 == 0){ return({ void*(* _temp931)( struct _tagged_arr fmt,
struct _tagged_arr ap)= Cyc_Tcutil_impos; struct _tagged_arr _temp934= _tag_arr("tcpat:and_match: malformed work frame",
sizeof( unsigned char), 38u); void* _temp932[ 0u]={}; struct _tagged_arr
_temp933={( void*) _temp932,( void*) _temp932,( void*)( _temp932 + 0u)};
_temp931( _temp934, _temp933);});}{ struct Cyc_List_List _temp937; struct Cyc_List_List*
_temp938; void* _temp940; struct Cyc_List_List* _temp935=( struct Cyc_List_List*)
_check_null( _temp927); _temp937=* _temp935; _LL941: _temp940=( void*) _temp937.hd;
goto _LL939; _LL939: _temp938= _temp937.tl; goto _LL936; _LL936: { struct Cyc_List_List
_temp944; struct Cyc_List_List* _temp945; struct Cyc_List_List* _temp947; struct
Cyc_List_List* _temp942=( struct Cyc_List_List*) _check_null( _temp925);
_temp944=* _temp942; _LL948: _temp947=( struct Cyc_List_List*) _temp944.hd; goto
_LL946; _LL946: _temp945= _temp944.tl; goto _LL943; _LL943: { struct Cyc_List_List
_temp951; struct Cyc_List_List* _temp952; void* _temp954; struct Cyc_List_List*
_temp949=( struct Cyc_List_List*) _check_null( _temp923); _temp951=* _temp949;
_LL955: _temp954=( void*) _temp951.hd; goto _LL953; _LL953: _temp952= _temp951.tl;
goto _LL950; _LL950: { struct _tuple10* _temp956=({ struct _tuple10* _temp958=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp958->f1= _temp938;
_temp958->f2= _temp945; _temp958->f3= _temp952; _temp958;}); return Cyc_Tcpat_match(
_temp940, _temp947, _temp954, ctx,({ struct Cyc_List_List* _temp957=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp957->hd=( void*) _temp956;
_temp957->tl= _temp918; _temp957;}), right_hand_side, rules);}}}} _LL898:;}
static struct Cyc_List_List* Cyc_Tcpat_getdargs( struct Cyc_Tcpat_Con_s* pcon,
void* dsc){ void* _temp959= dsc; struct Cyc_Set_Set* _temp965; struct Cyc_List_List*
_temp967; _LL961: if(*(( int*) _temp959) == Cyc_Tcpat_Neg){ _LL966: _temp965=((
struct Cyc_Tcpat_Neg_struct*) _temp959)->f1; goto _LL962;} else{ goto _LL963;}
_LL963: if(*(( int*) _temp959) == Cyc_Tcpat_Pos){ _LL968: _temp967=(( struct Cyc_Tcpat_Pos_struct*)
_temp959)->f2; goto _LL964;} else{ goto _LL960;} _LL962: { void* any=( void*)({
struct Cyc_Tcpat_Neg_struct* _temp971=( struct Cyc_Tcpat_Neg_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Neg_struct)); _temp971[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp972; _temp972.tag= Cyc_Tcpat_Neg; _temp972.f1= Cyc_Tcpat_empty_con_set();
_temp972;}); _temp971;}); struct Cyc_List_List* _temp969= 0;{ int i= 0; for( 0;
i < pcon->arity; ++ i){ _temp969=({ struct Cyc_List_List* _temp970=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp970->hd=( void*) any; _temp970->tl=
_temp969; _temp970;});}} return _temp969;} _LL964: return _temp967; _LL960:;}
static struct Cyc_List_List* Cyc_Tcpat_getoarg( struct Cyc_List_List* obj, int i){
return({ struct Cyc_List_List* _temp973=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp973->hd=( void*)( i + 1); _temp973->tl= obj;
_temp973;});} static struct Cyc_List_List* Cyc_Tcpat_getoargs( struct Cyc_Tcpat_Con_s*
pcon, struct Cyc_List_List* obj){ return(( struct Cyc_List_List*(*)( int n,
struct Cyc_List_List*(* f)( struct Cyc_List_List*, int), struct Cyc_List_List*
env)) Cyc_List_tabulate_c)( pcon->arity, Cyc_Tcpat_getoarg, obj);} static void*
Cyc_Tcpat_match( void* p, struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules){ void* _temp974= p; struct Cyc_List_List* _temp980; struct Cyc_Tcpat_Con_s*
_temp982; _LL976: if( _temp974 ==( void*) Cyc_Tcpat_Any){ goto _LL977;} else{
goto _LL978;} _LL978: if(( unsigned int) _temp974 > 1u?*(( int*) _temp974) ==
Cyc_Tcpat_Con: 0){ _LL983: _temp982=(( struct Cyc_Tcpat_Con_struct*) _temp974)->f1;
goto _LL981; _LL981: _temp980=(( struct Cyc_Tcpat_Con_struct*) _temp974)->f2;
goto _LL979;} else{ goto _LL975;} _LL977: return Cyc_Tcpat_and_match( Cyc_Tcpat_augment(
ctx, dsc), work, right_hand_side, rules); _LL979: { void* _temp984= Cyc_Tcpat_static_match(
_temp982, dsc); _LL986: if( _temp984 ==( void*) Cyc_Tcpat_Yes){ goto _LL987;}
else{ goto _LL988;} _LL988: if( _temp984 ==( void*) Cyc_Tcpat_No){ goto _LL989;}
else{ goto _LL990;} _LL990: if( _temp984 ==( void*) Cyc_Tcpat_Maybe){ goto
_LL991;} else{ goto _LL985;} _LL987: { struct Cyc_List_List* _temp992=({ struct
Cyc_List_List* _temp997=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp997->hd=( void*)({ struct _tuple11* _temp998=( struct _tuple11*) GC_malloc(
sizeof( struct _tuple11)); _temp998->f1= _temp982; _temp998->f2= 0; _temp998;});
_temp997->tl= ctx; _temp997;}); struct _tuple10* _temp993=({ struct _tuple10*
_temp996=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp996->f1=
_temp980; _temp996->f2= Cyc_Tcpat_getoargs( _temp982, obj); _temp996->f3= Cyc_Tcpat_getdargs(
_temp982, dsc); _temp996;}); struct Cyc_List_List* _temp994=({ struct Cyc_List_List*
_temp995=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp995->hd=( void*) _temp993; _temp995->tl= work; _temp995;}); return Cyc_Tcpat_and_match(
_temp992, _temp994, right_hand_side, rules);} _LL989: return Cyc_Tcpat_or_match(
Cyc_Tcpat_build_desc( ctx, dsc, work), rules); _LL991: { struct Cyc_List_List*
_temp999=({ struct Cyc_List_List* _temp1008=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1008->hd=( void*)({ struct _tuple11*
_temp1009=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp1009->f1=
_temp982; _temp1009->f2= 0; _temp1009;}); _temp1008->tl= ctx; _temp1008;});
struct _tuple10* _temp1000=({ struct _tuple10* _temp1007=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1007->f1= _temp980; _temp1007->f2=
Cyc_Tcpat_getoargs( _temp982, obj); _temp1007->f3= Cyc_Tcpat_getdargs( _temp982,
dsc); _temp1007;}); struct Cyc_List_List* _temp1001=({ struct Cyc_List_List*
_temp1006=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1006->hd=( void*) _temp1000; _temp1006->tl= work; _temp1006;}); void*
_temp1002= Cyc_Tcpat_and_match( _temp999, _temp1001, right_hand_side, rules);
void* _temp1003= Cyc_Tcpat_or_match( Cyc_Tcpat_build_desc( ctx, Cyc_Tcpat_add_neg(
dsc, _temp982), work), rules); return( void*)({ struct Cyc_Tcpat_IfEq_struct*
_temp1004=( struct Cyc_Tcpat_IfEq_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_IfEq_struct));
_temp1004[ 0]=({ struct Cyc_Tcpat_IfEq_struct _temp1005; _temp1005.tag= Cyc_Tcpat_IfEq;
_temp1005.f1= obj; _temp1005.f2= _temp982; _temp1005.f3=( void*) _temp1002;
_temp1005.f4=( void*) _temp1003; _temp1005;}); _temp1004;});} _LL985:;} _LL975:;}
static void Cyc_Tcpat_check_exhaust_overlap( void* d, void(* not_exhaust)( void*),
void* env1, void(* rhs_appears)( void*, void*), void* env2){ void* _temp1010= d;
void* _temp1018; void* _temp1020; void* _temp1022; _LL1012: if( _temp1010 ==(
void*) Cyc_Tcpat_Failure){ goto _LL1013;} else{ goto _LL1014;} _LL1014: if((
unsigned int) _temp1010 > 1u?*(( int*) _temp1010) == Cyc_Tcpat_Success: 0){
_LL1019: _temp1018=( void*)(( struct Cyc_Tcpat_Success_struct*) _temp1010)->f1;
goto _LL1015;} else{ goto _LL1016;} _LL1016: if(( unsigned int) _temp1010 > 1u?*((
int*) _temp1010) == Cyc_Tcpat_IfEq: 0){ _LL1023: _temp1022=( void*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1010)->f3; goto _LL1021; _LL1021: _temp1020=( void*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1010)->f4; goto _LL1017;} else{ goto _LL1011;} _LL1013: not_exhaust( env1);
goto _LL1011; _LL1015: rhs_appears( env2, _temp1018); goto _LL1011; _LL1017: Cyc_Tcpat_check_exhaust_overlap(
_temp1022, not_exhaust, env1, rhs_appears, env2); Cyc_Tcpat_check_exhaust_overlap(
_temp1020, not_exhaust, env1, rhs_appears, env2); goto _LL1011; _LL1011:;}
struct _tuple12{ int f1; struct Cyc_Position_Segment* f2; } ; struct _tuple13{
void* f1; struct _tuple12* f2; } ; static struct _tuple13* Cyc_Tcpat_get_match(
int* r, struct Cyc_Absyn_Switch_clause* swc){ void* sp0= Cyc_Tcpat_compile_pat(
swc->pattern); struct _tuple12* rhs=({ struct _tuple12* _temp1029=( struct
_tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp1029->f1= 0; _temp1029->f2=(
swc->pattern)->loc; _temp1029;}); void* sp; if( swc->where_clause != 0){ sp= Cyc_Tcpat_tuple_pat(({
struct Cyc_List_List* _temp1024=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1024->hd=( void*) sp0; _temp1024->tl=({ struct Cyc_List_List*
_temp1025=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1025->hd=( void*) Cyc_Tcpat_int_pat(* r); _temp1025->tl= 0; _temp1025;});
_temp1024;}));* r=* r + 1;} else{ sp= Cyc_Tcpat_tuple_pat(({ struct Cyc_List_List*
_temp1026=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1026->hd=( void*) sp0; _temp1026->tl=({ struct Cyc_List_List* _temp1027=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1027->hd=(
void*)(( void*) Cyc_Tcpat_Any); _temp1027->tl= 0; _temp1027;}); _temp1026;}));}
return({ struct _tuple13* _temp1028=( struct _tuple13*) GC_malloc( sizeof(
struct _tuple13)); _temp1028->f1= sp; _temp1028->f2= rhs; _temp1028;});} static
void Cyc_Tcpat_not_exhaust_err( struct Cyc_Position_Segment* loc){({ void(*
_temp1030)( struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp1033= loc;
struct _tagged_arr _temp1034= _tag_arr("patterns may not be exhaustive", sizeof(
unsigned char), 31u); void* _temp1031[ 0u]={}; struct _tagged_arr _temp1032={(
void*) _temp1031,( void*) _temp1031,( void*)( _temp1031 + 0u)}; _temp1030(
_temp1033, _temp1034, _temp1032);});} static void Cyc_Tcpat_rule_occurs( int
dummy, struct _tuple12* rhs){(* rhs).f1= 1;} void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ int _temp1035= 0;
struct Cyc_List_List* _temp1036=(( struct Cyc_List_List*(*)( struct _tuple13*(*
f)( int*, struct Cyc_Absyn_Switch_clause*), int* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Tcpat_get_match,& _temp1035, swcs); void* _temp1037=(( void*(*)(
struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)( _temp1036);(( void(*)(
void* d, void(* not_exhaust)( struct Cyc_Position_Segment*), struct Cyc_Position_Segment*
env1, void(* rhs_appears)( int, struct _tuple12*), int env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp1037, Cyc_Tcpat_not_exhaust_err, loc, Cyc_Tcpat_rule_occurs, 0); for( 0;
_temp1036 != 0; _temp1036=(( struct Cyc_List_List*) _check_null( _temp1036))->tl){
struct _tuple13 _temp1040; struct _tuple12* _temp1041; struct _tuple12 _temp1043;
struct Cyc_Position_Segment* _temp1044; int _temp1046; struct _tuple13*
_temp1038=( struct _tuple13*)(( struct Cyc_List_List*) _check_null( _temp1036))->hd;
_temp1040=* _temp1038; _LL1042: _temp1041= _temp1040.f2; _temp1043=* _temp1041;
_LL1047: _temp1046= _temp1043.f1; goto _LL1045; _LL1045: _temp1044= _temp1043.f2;
goto _LL1039; _LL1039: if( ! _temp1046){({ void(* _temp1048)( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp1051= _temp1044; struct _tagged_arr _temp1052= _tag_arr("redundant pattern",
sizeof( unsigned char), 18u); void* _temp1049[ 0u]={}; struct _tagged_arr
_temp1050={( void*) _temp1049,( void*) _temp1049,( void*)( _temp1049 + 0u)};
_temp1048( _temp1051, _temp1052, _temp1050);});}}} struct _tuple14{ struct Cyc_Position_Segment*
f1; int f2; } ; static void Cyc_Tcpat_not_exhaust_warn( struct _tuple14* pr){(*
pr).f2= 0;({ void(* _temp1053)( struct Cyc_Position_Segment*, struct _tagged_arr
fmt, struct _tagged_arr ap)= Cyc_Tcutil_warn; struct Cyc_Position_Segment*
_temp1056=(* pr).f1; struct _tagged_arr _temp1057= _tag_arr("pattern not exhaustive",
sizeof( unsigned char), 23u); void* _temp1054[ 0u]={}; struct _tagged_arr
_temp1055={( void*) _temp1054,( void*) _temp1054,( void*)( _temp1054 + 0u)};
_temp1053( _temp1056, _temp1057, _temp1055);});} static void Cyc_Tcpat_dummy_fn(
int i, int j){ return;} struct _tuple15{ void* f1; int f2; } ; int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Pat* p){ struct Cyc_List_List*
_temp1058=({ struct Cyc_List_List* _temp1062=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1062->hd=( void*)({ struct _tuple15*
_temp1063=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp1063->f1=
Cyc_Tcpat_compile_pat( p); _temp1063->f2= 0; _temp1063;}); _temp1062->tl= 0;
_temp1062;}); void* _temp1059=(( void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
_temp1058); struct _tuple14* _temp1060=({ struct _tuple14* _temp1061=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp1061->f1= loc; _temp1061->f2=
1; _temp1061;});(( void(*)( void* d, void(* not_exhaust)( struct _tuple14*),
struct _tuple14* env1, void(* rhs_appears)( int, int), int env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp1059, Cyc_Tcpat_not_exhaust_warn, _temp1060, Cyc_Tcpat_dummy_fn, 0); return(*
_temp1060).f2;} static struct _tuple13* Cyc_Tcpat_get_match2( struct Cyc_Absyn_Switch_clause*
swc){ void* sp0= Cyc_Tcpat_compile_pat( swc->pattern); struct _tuple12* rhs=({
struct _tuple12* _temp1065=( struct _tuple12*) GC_malloc( sizeof( struct
_tuple12)); _temp1065->f1= 0; _temp1065->f2=( swc->pattern)->loc; _temp1065;});
return({ struct _tuple13* _temp1064=( struct _tuple13*) GC_malloc( sizeof(
struct _tuple13)); _temp1064->f1= sp0; _temp1064->f2= rhs; _temp1064;});} static
void Cyc_Tcpat_not_exhaust_err2( struct Cyc_Position_Segment* loc){;} void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ struct Cyc_List_List*
_temp1066=(( struct Cyc_List_List*(*)( struct _tuple13*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_get_match2, swcs); void*
_temp1067=(( void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
_temp1066);(( void(*)( void* d, void(* not_exhaust)( struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env1, void(* rhs_appears)( int, struct _tuple12*),
int env2)) Cyc_Tcpat_check_exhaust_overlap)( _temp1067, Cyc_Tcpat_not_exhaust_err2,
loc, Cyc_Tcpat_rule_occurs, 0); for( 0; _temp1066 != 0; _temp1066=(( struct Cyc_List_List*)
_check_null( _temp1066))->tl){ struct _tuple13 _temp1070; struct _tuple12*
_temp1071; struct _tuple12 _temp1073; struct Cyc_Position_Segment* _temp1074;
int _temp1076; struct _tuple13* _temp1068=( struct _tuple13*)(( struct Cyc_List_List*)
_check_null( _temp1066))->hd; _temp1070=* _temp1068; _LL1072: _temp1071=
_temp1070.f2; _temp1073=* _temp1071; _LL1077: _temp1076= _temp1073.f1; goto
_LL1075; _LL1075: _temp1074= _temp1073.f2; goto _LL1069; _LL1069: if( !
_temp1076){({ void(* _temp1078)( struct Cyc_Position_Segment*, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp1081= _temp1074; struct _tagged_arr _temp1082= _tag_arr("redundant pattern",
sizeof( unsigned char), 18u); void* _temp1079[ 0u]={}; struct _tagged_arr
_temp1080={( void*) _temp1079,( void*) _temp1079,( void*)( _temp1079 + 0u)};
_temp1078( _temp1081, _temp1082, _temp1080);});}}}