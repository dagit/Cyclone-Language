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
struct _tagged_arr); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_arr); extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*,
struct _tagged_arr); extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_coerceable(
void*); extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_rsubstitute(
struct _RegionHandle*, struct Cyc_List_List*, void*); struct _tuple4{ struct Cyc_List_List*
f1; struct _RegionHandle* f2; } ; struct _tuple5{ struct Cyc_Absyn_Tvar* f1;
void* f2; } ; extern struct _tuple5* Cyc_Tcutil_r_make_inst_var( struct _tuple4*,
struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_check_unique_vars( struct Cyc_List_List*
vs, struct Cyc_Position_Segment* loc, struct _tagged_arr err_msg); extern struct
Cyc_List_List* Cyc_Tcutil_resolve_struct_designators( struct _RegionHandle* rgn,
struct Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_List_List*
fields); struct _tuple6{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ;
extern struct _tuple6 Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat*
p, void** topt, void** region_opt); extern void Cyc_Tcpat_check_pat_regions(
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
Cyc_Tcenv_VarRes){ goto _LL36;} else{ goto _LL28;} _LL30: Cyc_Tcutil_terr( p->loc,
_tag_arr("struct tag used without arguments in pattern", sizeof( unsigned char),
45u)); _npop_handler( 0u); return; _LL32:( void*)( p->r=( void*)(( void*)({
struct Cyc_Absyn_Tunion_p_struct* _temp45=( struct Cyc_Absyn_Tunion_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct)); _temp45[ 0]=({ struct Cyc_Absyn_Tunion_p_struct
_temp46; _temp46.tag= Cyc_Absyn_Tunion_p; _temp46.f1= _temp39; _temp46.f2=
_temp37; _temp46.f3= 0; _temp46.f4= 0; _temp46;}); _temp45;}))); _npop_handler(
0u); return; _LL34:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_p_struct*
_temp47=( struct Cyc_Absyn_Enum_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_p_struct));
_temp47[ 0]=({ struct Cyc_Absyn_Enum_p_struct _temp48; _temp48.tag= Cyc_Absyn_Enum_p;
_temp48.f1= _temp43; _temp48.f2= _temp41; _temp48;}); _temp47;})));
_npop_handler( 0u); return; _LL36: goto _LL28; _LL28:;}; _pop_handler();} else{
void* _temp25=( void*) _exn_thrown; void* _temp50= _temp25; _LL52: if( _temp50
== Cyc_Dict_Absent){ goto _LL53;} else{ goto _LL54;} _LL54: goto _LL55; _LL53:
goto _LL51; _LL55:( void) _throw( _temp50); _LL51:;}}}{ void* _temp56=(* _temp10).f1;
struct Cyc_List_List* _temp64; _LL58: if( _temp56 ==( void*) Cyc_Absyn_Loc_n){
goto _LL59;} else{ goto _LL60;} _LL60: if(( unsigned int) _temp56 > 1u?*(( int*)
_temp56) == Cyc_Absyn_Rel_n: 0){ _LL65: _temp64=(( struct Cyc_Absyn_Rel_n_struct*)
_temp56)->f1; if( _temp64 == 0){ goto _LL61;} else{ goto _LL62;}} else{ goto
_LL62;} _LL62: goto _LL63; _LL59: goto _LL61; _LL61:(* _temp10).f1=( void*) Cyc_Absyn_Loc_n;(
void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct* _temp66=( struct
Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp66[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp67; _temp67.tag= Cyc_Absyn_Var_p;
_temp67.f1= Cyc_Absyn_new_vardecl( _temp10,( void*) Cyc_Absyn_VoidType, 0);
_temp67;}); _temp66;}))); return; _LL63: Cyc_Tcutil_terr( p->loc, _tag_arr("qualified variable in pattern",
sizeof( unsigned char), 30u)); return; _LL57:;} _LL5:{ struct _handler_cons
_temp68; _push_handler(& _temp68);{ int _temp70= 0; if( setjmp( _temp68.handler)){
_temp70= 1;} if( ! _temp70){{ void* _temp71= Cyc_Tcenv_lookup_ordinary( te, p->loc,
_temp16); struct Cyc_Absyn_Structdecl* _temp81; struct Cyc_Absyn_Tunionfield*
_temp83; struct Cyc_Absyn_Tuniondecl* _temp85; _LL73: if(*(( int*) _temp71) ==
Cyc_Tcenv_StructRes){ _LL82: _temp81=(( struct Cyc_Tcenv_StructRes_struct*)
_temp71)->f1; goto _LL74;} else{ goto _LL75;} _LL75: if(*(( int*) _temp71) ==
Cyc_Tcenv_TunionRes){ _LL86: _temp85=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp71)->f1; goto _LL84; _LL84: _temp83=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp71)->f2; goto _LL76;} else{ goto _LL77;} _LL77: if(*(( int*) _temp71) ==
Cyc_Tcenv_EnumRes){ goto _LL78;} else{ goto _LL79;} _LL79: if(*(( int*) _temp71)
== Cyc_Tcenv_VarRes){ goto _LL80;} else{ goto _LL72;} _LL74: { struct Cyc_List_List*
_temp87= 0; for( 0; _temp12 != 0; _temp12=(( struct Cyc_List_List*) _check_null(
_temp12))->tl){ _temp87=({ struct Cyc_List_List* _temp88=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp88->hd=( void*)({ struct _tuple7*
_temp89=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp89->f1= 0;
_temp89->f2=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null(
_temp12))->hd; _temp89;}); _temp88->tl= _temp87; _temp88;});}( void*)( p->r=(
void*)(( void*)({ struct Cyc_Absyn_Struct_p_struct* _temp90=( struct Cyc_Absyn_Struct_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct)); _temp90[ 0]=({ struct Cyc_Absyn_Struct_p_struct
_temp91; _temp91.tag= Cyc_Absyn_Struct_p; _temp91.f1= _temp81; _temp91.f2= 0;
_temp91.f3= _temp14; _temp91.f4=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp87); _temp91;}); _temp90;}))); _npop_handler( 0u);
return;} _LL76:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_p_struct*
_temp92=( struct Cyc_Absyn_Tunion_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct));
_temp92[ 0]=({ struct Cyc_Absyn_Tunion_p_struct _temp93; _temp93.tag= Cyc_Absyn_Tunion_p;
_temp93.f1= _temp85; _temp93.f2= _temp83; _temp93.f3= _temp14; _temp93.f4=
_temp12; _temp93;}); _temp92;}))); _npop_handler( 0u); return; _LL78: Cyc_Tcutil_terr(
p->loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp94)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp98=
_tag_arr("bad enum constructor %s in pattern", sizeof( unsigned char), 35u);
struct Cyc_Stdio_String_pa_struct _temp99; _temp99.tag= Cyc_Stdio_String_pa;
_temp99.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp16);{ void*
_temp97=( void*)& _temp99; void* _temp95[ 1u]={ _temp97}; struct _tagged_arr
_temp96={( void*) _temp95,( void*) _temp95,( void*)( _temp95 + 1u)}; _temp94(
_temp98, _temp96);}})); _npop_handler( 0u); return; _LL80: goto _LL72; _LL72:;};
_pop_handler();} else{ void* _temp69=( void*) _exn_thrown; void* _temp101=
_temp69; _LL103: if( _temp101 == Cyc_Dict_Absent){ goto _LL104;} else{ goto
_LL105;} _LL105: goto _LL106; _LL104: goto _LL102; _LL106:( void) _throw(
_temp101); _LL102:;}}} Cyc_Tcutil_terr( p->loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp107)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp111= _tag_arr("%s is not a constructor in pattern",
sizeof( unsigned char), 35u); struct Cyc_Stdio_String_pa_struct _temp112;
_temp112.tag= Cyc_Stdio_String_pa; _temp112.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp16);{ void* _temp110=( void*)& _temp112; void* _temp108[ 1u]={ _temp110};
struct _tagged_arr _temp109={( void*) _temp108,( void*) _temp108,( void*)(
_temp108 + 1u)}; _temp107( _temp111, _temp109);}})); return; _LL7:{ struct
_handler_cons _temp113; _push_handler(& _temp113);{ int _temp115= 0; if( setjmp(
_temp113.handler)){ _temp115= 1;} if( ! _temp115){{ void* _temp116= Cyc_Tcenv_lookup_ordinary(
te, p->loc, _temp22); struct Cyc_Absyn_Structdecl* _temp122; _LL118: if(*(( int*)
_temp116) == Cyc_Tcenv_StructRes){ _LL123: _temp122=(( struct Cyc_Tcenv_StructRes_struct*)
_temp116)->f1; goto _LL119;} else{ goto _LL120;} _LL120: goto _LL121; _LL119:(
void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_p_struct* _temp124=(
struct Cyc_Absyn_Struct_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct));
_temp124[ 0]=({ struct Cyc_Absyn_Struct_p_struct _temp125; _temp125.tag= Cyc_Absyn_Struct_p;
_temp125.f1= _temp122; _temp125.f2= 0; _temp125.f3= _temp20; _temp125.f4=
_temp18; _temp125;}); _temp124;}))); _npop_handler( 0u); return; _LL121: goto
_LL117; _LL117:;}; _pop_handler();} else{ void* _temp114=( void*) _exn_thrown;
void* _temp127= _temp114; _LL129: if( _temp127 == Cyc_Dict_Absent){ goto _LL130;}
else{ goto _LL131;} _LL131: goto _LL132; _LL130: goto _LL128; _LL132:( void)
_throw( _temp127); _LL128:;}}} Cyc_Tcutil_terr( p->loc,( struct _tagged_arr)({
struct _tagged_arr(* _temp133)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp137= _tag_arr("%s is not a struct name, but pattern has designators",
sizeof( unsigned char), 53u); struct Cyc_Stdio_String_pa_struct _temp138;
_temp138.tag= Cyc_Stdio_String_pa; _temp138.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp22);{ void* _temp136=( void*)& _temp138; void* _temp134[ 1u]={ _temp136};
struct _tagged_arr _temp135={( void*) _temp134,( void*) _temp134,( void*)(
_temp134 + 1u)}; _temp133( _temp137, _temp135);}})); return; _LL9: return; _LL1:;}
static struct _tagged_arr* Cyc_Tcpat_get_name( struct Cyc_Absyn_Vardecl* vd){
return(* vd->name).f2;} static struct Cyc_Core_Opt Cyc_Tcpat_mk={( void*)(( void*)
1u)}; static struct Cyc_Core_Opt Cyc_Tcpat_rk={( void*)(( void*) 3u)}; static
struct Cyc_Core_Opt Cyc_Tcpat_ak={( void*)(( void*) 0u)}; static void* Cyc_Tcpat_any_type(
struct Cyc_List_List* s, void** topt){ if( topt != 0){ return*(( void**)
_check_null( topt));} return Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcpat_mk,({
struct Cyc_Core_Opt* _temp139=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp139->v=( void*) s; _temp139;}));} static void* Cyc_Tcpat_num_type(
void** topt, void* numt){ if( topt != 0? Cyc_Tcutil_coerceable(*(( void**)
_check_null( topt))): 0){ return*(( void**) _check_null( topt));}{ void*
_temp140= Cyc_Tcutil_compress( numt); _LL142: if(( unsigned int) _temp140 > 4u?*((
int*) _temp140) == Cyc_Absyn_EnumType: 0){ goto _LL143;} else{ goto _LL144;}
_LL144: goto _LL145; _LL143: if( topt != 0){ return*(( void**) _check_null( topt));}
goto _LL141; _LL145: goto _LL141; _LL141:;} return numt;} static void Cyc_Tcpat_set_vd(
struct Cyc_Absyn_Vardecl* vd, struct Cyc_List_List** v_result_ptr, void* t){(
void*)( vd->type=( void*) t); vd->tq= Cyc_Absyn_empty_tqual();* v_result_ptr=({
struct Cyc_List_List* _temp146=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp146->hd=( void*) vd; _temp146->tl=* v_result_ptr;
_temp146;});} struct _tuple8{ struct Cyc_Absyn_Tqual f1; void* f2; } ; struct
_tuple9{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Pat* f2; } ; struct
_tuple6 Cyc_Tcpat_tcPatRec( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p,
void** topt, void** region_opt){ Cyc_Tcpat_resolve_pat( te, p);{ void* t; struct
Cyc_List_List* tv_result= 0; struct Cyc_List_List* v_result= 0;{ void* _temp147=(
void*) p->r; struct Cyc_Absyn_Vardecl* _temp181; struct Cyc_Absyn_Vardecl*
_temp183; void* _temp185; void* _temp187; struct Cyc_Absyn_Enumfield* _temp189;
struct Cyc_Absyn_Enumdecl* _temp191; struct Cyc_Absyn_Pat* _temp193; struct Cyc_List_List*
_temp195; struct Cyc_List_List* _temp197; struct Cyc_List_List* _temp199; struct
Cyc_Core_Opt* _temp201; struct Cyc_Core_Opt** _temp203; struct Cyc_Absyn_Structdecl*
_temp204; struct Cyc_List_List* _temp206; struct Cyc_List_List* _temp208; struct
Cyc_Absyn_Tunionfield* _temp210; struct Cyc_Absyn_Tuniondecl* _temp212; _LL149:
if( _temp147 ==( void*) Cyc_Absyn_Wild_p){ goto _LL150;} else{ goto _LL151;}
_LL151: if(( unsigned int) _temp147 > 2u?*(( int*) _temp147) == Cyc_Absyn_Var_p:
0){ _LL182: _temp181=(( struct Cyc_Absyn_Var_p_struct*) _temp147)->f1; goto
_LL152;} else{ goto _LL153;} _LL153: if(( unsigned int) _temp147 > 2u?*(( int*)
_temp147) == Cyc_Absyn_Reference_p: 0){ _LL184: _temp183=(( struct Cyc_Absyn_Reference_p_struct*)
_temp147)->f1; goto _LL154;} else{ goto _LL155;} _LL155: if(( unsigned int)
_temp147 > 2u?*(( int*) _temp147) == Cyc_Absyn_Int_p: 0){ _LL186: _temp185=(
void*)(( struct Cyc_Absyn_Int_p_struct*) _temp147)->f1; if( _temp185 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL156;} else{ goto _LL157;}} else{ goto _LL157;}
_LL157: if(( unsigned int) _temp147 > 2u?*(( int*) _temp147) == Cyc_Absyn_Int_p:
0){ _LL188: _temp187=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp147)->f1;
if( _temp187 ==( void*) Cyc_Absyn_Signed){ goto _LL158;} else{ goto _LL159;}}
else{ goto _LL159;} _LL159: if(( unsigned int) _temp147 > 2u?*(( int*) _temp147)
== Cyc_Absyn_Char_p: 0){ goto _LL160;} else{ goto _LL161;} _LL161: if((
unsigned int) _temp147 > 2u?*(( int*) _temp147) == Cyc_Absyn_Float_p: 0){ goto
_LL162;} else{ goto _LL163;} _LL163: if(( unsigned int) _temp147 > 2u?*(( int*)
_temp147) == Cyc_Absyn_Enum_p: 0){ _LL192: _temp191=(( struct Cyc_Absyn_Enum_p_struct*)
_temp147)->f1; goto _LL190; _LL190: _temp189=(( struct Cyc_Absyn_Enum_p_struct*)
_temp147)->f2; goto _LL164;} else{ goto _LL165;} _LL165: if( _temp147 ==( void*)
Cyc_Absyn_Null_p){ goto _LL166;} else{ goto _LL167;} _LL167: if(( unsigned int)
_temp147 > 2u?*(( int*) _temp147) == Cyc_Absyn_Pointer_p: 0){ _LL194: _temp193=((
struct Cyc_Absyn_Pointer_p_struct*) _temp147)->f1; goto _LL168;} else{ goto
_LL169;} _LL169: if(( unsigned int) _temp147 > 2u?*(( int*) _temp147) == Cyc_Absyn_Tuple_p:
0){ _LL196: _temp195=(( struct Cyc_Absyn_Tuple_p_struct*) _temp147)->f1; goto
_LL170;} else{ goto _LL171;} _LL171: if(( unsigned int) _temp147 > 2u?*(( int*)
_temp147) == Cyc_Absyn_Struct_p: 0){ _LL205: _temp204=(( struct Cyc_Absyn_Struct_p_struct*)
_temp147)->f1; goto _LL202; _LL202: _temp201=(( struct Cyc_Absyn_Struct_p_struct*)
_temp147)->f2; _temp203=&(( struct Cyc_Absyn_Struct_p_struct*) _temp147)->f2;
goto _LL200; _LL200: _temp199=(( struct Cyc_Absyn_Struct_p_struct*) _temp147)->f3;
goto _LL198; _LL198: _temp197=(( struct Cyc_Absyn_Struct_p_struct*) _temp147)->f4;
goto _LL172;} else{ goto _LL173;} _LL173: if(( unsigned int) _temp147 > 2u?*((
int*) _temp147) == Cyc_Absyn_Tunion_p: 0){ _LL213: _temp212=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp147)->f1; goto _LL211; _LL211: _temp210=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp147)->f2; goto _LL209; _LL209: _temp208=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp147)->f3; goto _LL207; _LL207: _temp206=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp147)->f4; goto _LL174;} else{ goto _LL175;} _LL175: if(( unsigned int)
_temp147 > 2u?*(( int*) _temp147) == Cyc_Absyn_UnknownId_p: 0){ goto _LL176;}
else{ goto _LL177;} _LL177: if(( unsigned int) _temp147 > 2u?*(( int*) _temp147)
== Cyc_Absyn_UnknownCall_p: 0){ goto _LL178;} else{ goto _LL179;} _LL179: if((
unsigned int) _temp147 > 2u?*(( int*) _temp147) == Cyc_Absyn_UnknownFields_p: 0){
goto _LL180;} else{ goto _LL148;} _LL150: t= Cyc_Tcpat_any_type( Cyc_Tcenv_lookup_type_vars(
te), topt); goto _LL148; _LL152: t= Cyc_Tcpat_any_type( Cyc_Tcenv_lookup_type_vars(
te), topt); Cyc_Tcpat_set_vd( _temp181,& v_result, t); goto _LL148; _LL154: t=
Cyc_Tcpat_any_type( Cyc_Tcenv_lookup_type_vars( te), topt); if( region_opt == 0){
Cyc_Tcutil_terr( p->loc, _tag_arr("* pattern would point into an unknown region",
sizeof( unsigned char), 45u)); goto _LL148;} Cyc_Tcpat_set_vd( _temp183,&
v_result,( void*)({ struct Cyc_Absyn_PointerType_struct* _temp214=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp214[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp215; _temp215.tag= Cyc_Absyn_PointerType;
_temp215.f1=({ struct Cyc_Absyn_PtrInfo _temp216; _temp216.elt_typ=( void*) t;
_temp216.rgn_typ=( void*)*(( void**) _check_null( region_opt)); _temp216.nullable=((
struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)( 0); _temp216.tq= Cyc_Absyn_empty_tqual();
_temp216.bounds= Cyc_Absyn_empty_conref(); _temp216;}); _temp215;}); _temp214;}));
goto _LL148; _LL156: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_uint_t); goto _LL148;
_LL158: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_sint_t); goto _LL148; _LL160: t=
Cyc_Tcpat_num_type( topt, Cyc_Absyn_uchar_t); goto _LL148; _LL162: t= Cyc_Tcpat_num_type(
topt, Cyc_Absyn_float_t); goto _LL148; _LL164: t= Cyc_Tcpat_num_type( topt,(
void*)({ struct Cyc_Absyn_EnumType_struct* _temp217=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp217[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp218; _temp218.tag= Cyc_Absyn_EnumType; _temp218.f1=
_temp191->name; _temp218.f2=( struct Cyc_Absyn_Enumdecl*) _temp191; _temp218;});
_temp217;})); goto _LL148; _LL166: if( topt != 0){ void* _temp219= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); _LL221: if(( unsigned int) _temp219 > 4u?*(( int*)
_temp219) == Cyc_Absyn_PointerType: 0){ goto _LL222;} else{ goto _LL223;} _LL223:
goto _LL224; _LL222: t=*(( void**) _check_null( topt)); goto tcpat_end; _LL224:
goto _LL220; _LL220:;}{ struct Cyc_List_List* _temp225= Cyc_Tcenv_lookup_type_vars(
te); t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp226=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp226[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp227; _temp227.tag= Cyc_Absyn_PointerType;
_temp227.f1=({ struct Cyc_Absyn_PtrInfo _temp228; _temp228.elt_typ=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcpat_ak,({ struct Cyc_Core_Opt* _temp230=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp230->v=( void*)
_temp225; _temp230;})); _temp228.rgn_typ=( void*) Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Tcpat_rk,({ struct Cyc_Core_Opt* _temp229=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp229->v=( void*) _temp225;
_temp229;})); _temp228.nullable=(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
1); _temp228.tq= Cyc_Absyn_empty_tqual(); _temp228.bounds= Cyc_Absyn_empty_conref();
_temp228;}); _temp227;}); _temp226;}); goto _LL148;} _LL168: { void* inner_typ=(
void*) Cyc_Absyn_VoidType; void** _temp231= 0; if( topt != 0){ void* _temp232=
Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo
_temp238; void* _temp240; _LL234: if(( unsigned int) _temp232 > 4u?*(( int*)
_temp232) == Cyc_Absyn_PointerType: 0){ _LL239: _temp238=(( struct Cyc_Absyn_PointerType_struct*)
_temp232)->f1; _LL241: _temp240=( void*) _temp238.elt_typ; goto _LL235;} else{
goto _LL236;} _LL236: goto _LL237; _LL235: inner_typ= _temp240; _temp231=( void**)&
inner_typ; goto _LL233; _LL237: goto _LL233; _LL233:;}{ void* ptr_rgn= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcpat_rk,({ struct Cyc_Core_Opt* _temp275=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp275->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp275;})); struct Cyc_List_List* _temp244;
struct Cyc_List_List* _temp246; struct _tuple6 _temp242= Cyc_Tcpat_tcPatRec( te,
_temp193, _temp231,( void**)& ptr_rgn); _LL247: _temp246= _temp242.f1; goto
_LL245; _LL245: _temp244= _temp242.f2; goto _LL243; _LL243: tv_result=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tv_result, _temp246); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( v_result, _temp244); if( _temp231
== 0){ void* _temp248= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp193->topt))->v); struct Cyc_Absyn_TunionFieldInfo _temp254;
struct Cyc_List_List* _temp256; void* _temp258; struct Cyc_Absyn_Tunionfield*
_temp260; struct Cyc_Absyn_Tuniondecl* _temp262; _LL250: if(( unsigned int)
_temp248 > 4u?*(( int*) _temp248) == Cyc_Absyn_TunionFieldType: 0){ _LL255:
_temp254=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp248)->f1; _LL259:
_temp258=( void*) _temp254.field_info; if(*(( int*) _temp258) == Cyc_Absyn_KnownTunionfield){
_LL263: _temp262=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp258)->f1;
goto _LL261; _LL261: _temp260=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp258)->f2; goto _LL257;} else{ goto _LL252;} _LL257: _temp256= _temp254.targs;
goto _LL251;} else{ goto _LL252;} _LL252: goto _LL253; _LL251: t=( void*)({
struct Cyc_Absyn_TunionType_struct* _temp264=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp264[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp265; _temp265.tag= Cyc_Absyn_TunionType;
_temp265.f1=({ struct Cyc_Absyn_TunionInfo _temp266; _temp266.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp267=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp267[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp268; _temp268.tag= Cyc_Absyn_KnownTunion;
_temp268.f1= _temp262; _temp268;}); _temp267;})); _temp266.targs= _temp256;
_temp266.rgn=( void*) ptr_rgn; _temp266;}); _temp265;}); _temp264;}); goto
_LL249; _LL253: t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp269=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp269[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp270; _temp270.tag= Cyc_Absyn_PointerType;
_temp270.f1=({ struct Cyc_Absyn_PtrInfo _temp271; _temp271.elt_typ=( void*)((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp193->topt))->v); _temp271.rgn_typ=(
void*) ptr_rgn; _temp271.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp271.tq= Cyc_Absyn_empty_tqual(); _temp271.bounds= Cyc_Absyn_empty_conref();
_temp271;}); _temp270;}); _temp269;}); goto _LL249; _LL249:;} else{ t=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp272=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp272[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp273; _temp273.tag= Cyc_Absyn_PointerType;
_temp273.f1=({ struct Cyc_Absyn_PtrInfo _temp274; _temp274.elt_typ=( void*)((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp193->topt))->v); _temp274.rgn_typ=(
void*) ptr_rgn; _temp274.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp274.tq= Cyc_Absyn_empty_tqual(); _temp274.bounds= Cyc_Absyn_empty_conref();
_temp274;}); _temp273;}); _temp272;});} goto _LL148;}} _LL170: { struct Cyc_List_List*
pat_ts= 0; struct Cyc_List_List* topt_ts= 0; if( topt != 0){ void* _temp276= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_List_List* _temp282; _LL278: if((
unsigned int) _temp276 > 4u?*(( int*) _temp276) == Cyc_Absyn_TupleType: 0){
_LL283: _temp282=(( struct Cyc_Absyn_TupleType_struct*) _temp276)->f1; goto
_LL279;} else{ goto _LL280;} _LL280: goto _LL281; _LL279: topt_ts= _temp282;
goto _LL277; _LL281: goto _LL277; _LL277:;} for( 0; _temp195 != 0; _temp195=((
struct Cyc_List_List*) _check_null( _temp195))->tl){ void** _temp284= 0; if(
topt_ts != 0){ _temp284=( void**)&(*(( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( topt_ts))->hd)).f2; topt_ts=(( struct Cyc_List_List*) _check_null(
topt_ts))->tl;}{ struct Cyc_List_List* _temp287; struct Cyc_List_List* _temp289;
struct _tuple6 _temp285= Cyc_Tcpat_tcPatRec( te,( struct Cyc_Absyn_Pat*)((
struct Cyc_List_List*) _check_null( _temp195))->hd, _temp284, region_opt);
_LL290: _temp289= _temp285.f1; goto _LL288; _LL288: _temp287= _temp285.f2; goto
_LL286; _LL286: tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp289); v_result=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
v_result, _temp287); pat_ts=({ struct Cyc_List_List* _temp291=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp291->hd=( void*)({ struct
_tuple8* _temp292=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8));
_temp292->f1= Cyc_Absyn_empty_tqual(); _temp292->f2=( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null(
_temp195))->hd)->topt))->v; _temp292;}); _temp291->tl= pat_ts; _temp291;});}} t=(
void*)({ struct Cyc_Absyn_TupleType_struct* _temp293=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp293[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp294; _temp294.tag= Cyc_Absyn_TupleType; _temp294.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( pat_ts);
_temp294;}); _temp293;}); goto _LL148;} _LL172: if( _temp199 != 0){( int) _throw(
Cyc_Tcutil_impos( _tag_arr("tcPat: struct<...> not implemented", sizeof(
unsigned char), 35u)));}{ struct _RegionHandle _temp295= _new_region(); struct
_RegionHandle* rgn=& _temp295; _push_region( rgn);{ struct _tuple4 _temp296=({
struct _tuple4 _temp327; _temp327.f1= Cyc_Tcenv_lookup_type_vars( te); _temp327.f2=
rgn; _temp327;}); struct Cyc_List_List* _temp297=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp296, _temp204->tvs); struct Cyc_List_List* _temp298=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp297); t=( void*)({ struct Cyc_Absyn_StructType_struct*
_temp299=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp299[ 0]=({ struct Cyc_Absyn_StructType_struct _temp300; _temp300.tag= Cyc_Absyn_StructType;
_temp300.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp204->name))->v); _temp300.f2= _temp298; _temp300.f3=({ struct
Cyc_Absyn_Structdecl** _temp301=( struct Cyc_Absyn_Structdecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl*)); _temp301[ 0]= _temp204; _temp301;});
_temp300;}); _temp299;}); if( topt != 0){ Cyc_Tcutil_unify( t,*(( void**)
_check_null( topt)));}* _temp203=({ struct Cyc_Core_Opt* _temp302=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp302->v=( void*) _temp298;
_temp302;}); if( _temp204->fields == 0){ Cyc_Tcutil_terr( p->loc, _tag_arr("can't destructure an abstract struct",
sizeof( unsigned char), 37u)); _npop_handler( 0u); goto _LL148;}{ struct Cyc_List_List*
fields=(( struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, p->loc, _temp197,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp204->fields))->v); for( 0; fields != 0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ struct _tuple9 _temp305; struct Cyc_Absyn_Pat*
_temp306; struct Cyc_Absyn_Structfield* _temp308; struct _tuple9* _temp303=(
struct _tuple9*)(( struct Cyc_List_List*) _check_null( fields))->hd; _temp305=*
_temp303; _LL309: _temp308= _temp305.f1; goto _LL307; _LL307: _temp306= _temp305.f2;
goto _LL304; _LL304: { void* _temp310= Cyc_Tcutil_rsubstitute( rgn, _temp297,(
void*) _temp308->type); struct Cyc_List_List* _temp313; struct Cyc_List_List*
_temp315; struct _tuple6 _temp311= Cyc_Tcpat_tcPatRec( te, _temp306,( void**)&
_temp310, region_opt); _LL316: _temp315= _temp311.f1; goto _LL314; _LL314:
_temp313= _temp311.f2; goto _LL312; _LL312: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp315); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp313); if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp306->topt))->v, _temp310)){ Cyc_Tcutil_terr(
p->loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp317)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp323= _tag_arr("field %s of struct pattern expects type %s != %s", sizeof(
unsigned char), 49u); struct Cyc_Stdio_String_pa_struct _temp326; _temp326.tag=
Cyc_Stdio_String_pa; _temp326.f1=( struct _tagged_arr)* _temp308->name;{ void*
_temp320=( void*)& _temp326; struct Cyc_Stdio_String_pa_struct _temp325;
_temp325.tag= Cyc_Stdio_String_pa; _temp325.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp310);{ void* _temp321=( void*)& _temp325; struct Cyc_Stdio_String_pa_struct
_temp324; _temp324.tag= Cyc_Stdio_String_pa; _temp324.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp306->topt))->v);{
void* _temp322=( void*)& _temp324; void* _temp318[ 3u]={ _temp320, _temp321,
_temp322}; struct _tagged_arr _temp319={( void*) _temp318,( void*) _temp318,(
void*)( _temp318 + 3u)}; _temp317( _temp323, _temp319);}}}}));}}}}}; _pop_region(
rgn);} goto _LL148; _LL174: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp208) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp210->tvs)){
Cyc_Tcutil_terr( p->loc, _tag_arr("wrong number of existential type variables in pattern",
sizeof( unsigned char), 54u));}{ struct _RegionHandle _temp328= _new_region();
struct _RegionHandle* rgn=& _temp328; _push_region( rgn);{ struct Cyc_List_List*
_temp329= 0;{ struct Cyc_List_List* t= _temp208; for( 0; t != 0; t=(( struct Cyc_List_List*)
_check_null( t))->tl){ struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( t))->hd;{ void* _temp330=( void*)( Cyc_Absyn_compress_conref(
tv->kind))->v; void* _temp340; void* _temp342; _LL332: if(( unsigned int)
_temp330 > 1u?*(( int*) _temp330) == Cyc_Absyn_Eq_constr: 0){ _LL341: _temp340=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp330)->f1; if( _temp340 ==(
void*) Cyc_Absyn_BoxKind){ goto _LL333;} else{ goto _LL334;}} else{ goto _LL334;}
_LL334: if(( unsigned int) _temp330 > 1u?*(( int*) _temp330) == Cyc_Absyn_Eq_constr:
0){ _LL343: _temp342=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp330)->f1;
goto _LL335;} else{ goto _LL336;} _LL336: if( _temp330 ==( void*) Cyc_Absyn_No_constr){
goto _LL337;} else{ goto _LL338;} _LL338: if(( unsigned int) _temp330 > 1u?*((
int*) _temp330) == Cyc_Absyn_Forward_constr: 0){ goto _LL339;} else{ goto _LL331;}
_LL333: goto _LL331; _LL335: Cyc_Tcutil_terr( p->loc,( struct _tagged_arr)({
struct _tagged_arr(* _temp344)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp350= _tag_arr("tunion member %s abstracts type variable %s of kind %s != B",
sizeof( unsigned char), 60u); struct Cyc_Stdio_String_pa_struct _temp353;
_temp353.tag= Cyc_Stdio_String_pa; _temp353.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp212->name);{ void* _temp347=( void*)& _temp353; struct Cyc_Stdio_String_pa_struct
_temp352; _temp352.tag= Cyc_Stdio_String_pa; _temp352.f1=( struct _tagged_arr)*
tv->name;{ void* _temp348=( void*)& _temp352; struct Cyc_Stdio_String_pa_struct
_temp351; _temp351.tag= Cyc_Stdio_String_pa; _temp351.f1=( struct _tagged_arr)
Cyc_Absynpp_kind2string( _temp342);{ void* _temp349=( void*)& _temp351; void*
_temp345[ 3u]={ _temp347, _temp348, _temp349}; struct _tagged_arr _temp346={(
void*) _temp345,( void*) _temp345,( void*)( _temp345 + 3u)}; _temp344( _temp350,
_temp346);}}}})); goto _LL331; _LL337:( void*)(( tv->kind)->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_constr_struct* _temp354=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp354[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp355; _temp355.tag= Cyc_Absyn_Eq_constr; _temp355.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp355;}); _temp354;}))); goto _LL331;
_LL339:( int) _throw( Cyc_Tcutil_impos( _tag_arr("forward after compress in conref",
sizeof( unsigned char), 33u))); _LL331:;} _temp329=({ struct Cyc_List_List*
_temp356=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp356->hd=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp357=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp357[ 0]=({ struct Cyc_Absyn_VarType_struct _temp358; _temp358.tag= Cyc_Absyn_VarType;
_temp358.f1= tv; _temp358;}); _temp357;})); _temp356->tl= _temp329; _temp356;});}}
_temp329= Cyc_List_imp_rev( _temp329); tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp208);{ struct Cyc_Tcenv_Tenv* te2= Cyc_Tcenv_add_type_vars( p->loc, te,
_temp208); struct Cyc_List_List* tqts= _temp210->typs; struct Cyc_List_List*
_temp359= Cyc_Tcenv_lookup_type_vars( te); struct _tuple4 _temp360=({ struct
_tuple4 _temp425; _temp425.f1= _temp359; _temp425.f2= rgn; _temp425;}); struct
Cyc_List_List* _temp361=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*), struct _tuple4*
env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp360, _temp212->tvs); struct Cyc_List_List* _temp362=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp210->tvs, _temp329);
struct Cyc_List_List* _temp363=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple5*))
Cyc_Core_snd, _temp361); struct Cyc_List_List* _temp364=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp362); struct Cyc_List_List* _temp365=((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp361, _temp362); if( tqts == 0){
t=( void*)({ struct Cyc_Absyn_TunionType_struct* _temp366=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp366[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp367; _temp367.tag= Cyc_Absyn_TunionType;
_temp367.f1=({ struct Cyc_Absyn_TunionInfo _temp368; _temp368.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp370=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp370[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp371; _temp371.tag= Cyc_Absyn_KnownTunion;
_temp371.f1= _temp212; _temp371;}); _temp370;})); _temp368.targs= _temp363;
_temp368.rgn=( void*) Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcpat_rk,({
struct Cyc_Core_Opt* _temp369=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp369->v=( void*) _temp359; _temp369;})); _temp368;});
_temp367;}); _temp366;});} else{ t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp372=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp372[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp373; _temp373.tag= Cyc_Absyn_TunionFieldType; _temp373.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp374; _temp374.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp375=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp375[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp376; _temp376.tag= Cyc_Absyn_KnownTunionfield; _temp376.f1= _temp212;
_temp376.f2= _temp210; _temp376;}); _temp375;})); _temp374.targs= _temp363;
_temp374;}); _temp373;}); _temp372;});} if( topt != 0? tqts == 0: 0){ void*
_temp377= Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_TunionInfo
_temp385; struct Cyc_List_List* _temp387; _LL379: if(( unsigned int) _temp377 >
4u?*(( int*) _temp377) == Cyc_Absyn_TunionFieldType: 0){ goto _LL380;} else{
goto _LL381;} _LL381: if(( unsigned int) _temp377 > 4u?*(( int*) _temp377) ==
Cyc_Absyn_TunionType: 0){ _LL386: _temp385=(( struct Cyc_Absyn_TunionType_struct*)
_temp377)->f1; _LL388: _temp387= _temp385.targs; goto _LL382;} else{ goto _LL383;}
_LL383: goto _LL384; _LL380: t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp389=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp389[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp390; _temp390.tag= Cyc_Absyn_TunionFieldType; _temp390.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp391; _temp391.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp392=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp392[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp393; _temp393.tag= Cyc_Absyn_KnownTunionfield; _temp393.f1= _temp212;
_temp393.f2= _temp210; _temp393;}); _temp392;})); _temp391.targs= _temp363;
_temp391;}); _temp390;}); _temp389;}); goto _LL378; _LL382: { struct Cyc_List_List*
_temp394= _temp363; for( 0; _temp394 != 0? _temp387 != 0: 0;( _temp394=(( struct
Cyc_List_List*) _check_null( _temp394))->tl, _temp387=(( struct Cyc_List_List*)
_check_null( _temp387))->tl)){ Cyc_Tcutil_unify(( void*)(( struct Cyc_List_List*)
_check_null( _temp394))->hd,( void*)(( struct Cyc_List_List*) _check_null(
_temp387))->hd);} goto _LL378;} _LL384: goto _LL378; _LL378:;} for( 0; _temp206
!= 0? tqts != 0: 0;( _temp206=(( struct Cyc_List_List*) _check_null( _temp206))->tl,
tqts=(( struct Cyc_List_List*) _check_null( tqts))->tl)){ struct Cyc_Absyn_Pat*
_temp395=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null( _temp206))->hd;
void* _temp396= Cyc_Tcutil_rsubstitute( rgn, _temp365,(*(( struct _tuple8*)((
struct Cyc_List_List*) _check_null( tqts))->hd)).f2); struct Cyc_List_List*
_temp399; struct Cyc_List_List* _temp401; struct _tuple6 _temp397= Cyc_Tcpat_tcPatRec(
te2, _temp395,( void**)& _temp396, region_opt); _LL402: _temp401= _temp397.f1;
goto _LL400; _LL400: _temp399= _temp397.f2; goto _LL398; _LL398: tv_result=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tv_result, _temp401); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( v_result, _temp399); if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp395->topt))->v, _temp396)){ Cyc_Tcutil_terr(
_temp395->loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp403)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp409= _tag_arr("%s expects argument type %s, not %s", sizeof( unsigned char),
36u); struct Cyc_Stdio_String_pa_struct _temp412; _temp412.tag= Cyc_Stdio_String_pa;
_temp412.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp212->name);{
void* _temp406=( void*)& _temp412; struct Cyc_Stdio_String_pa_struct _temp411;
_temp411.tag= Cyc_Stdio_String_pa; _temp411.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp396);{ void* _temp407=( void*)& _temp411; struct Cyc_Stdio_String_pa_struct
_temp410; _temp410.tag= Cyc_Stdio_String_pa; _temp410.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp395->topt))->v);{
void* _temp408=( void*)& _temp410; void* _temp404[ 3u]={ _temp406, _temp407,
_temp408}; struct _tagged_arr _temp405={( void*) _temp404,( void*) _temp404,(
void*)( _temp404 + 3u)}; _temp403( _temp409, _temp405);}}}}));}} if( _temp206 !=
0){ Cyc_Tcutil_terr( p->loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp413)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp417= _tag_arr("too many arguments for tunion constructor %s",
sizeof( unsigned char), 45u); struct Cyc_Stdio_String_pa_struct _temp418;
_temp418.tag= Cyc_Stdio_String_pa; _temp418.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp212->name);{ void* _temp416=( void*)& _temp418; void* _temp414[ 1u]={
_temp416}; struct _tagged_arr _temp415={( void*) _temp414,( void*) _temp414,(
void*)( _temp414 + 1u)}; _temp413( _temp417, _temp415);}}));} if( tqts != 0){
Cyc_Tcutil_terr( p->loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp419)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp423= _tag_arr("too few arguments for tunion constructor %s",
sizeof( unsigned char), 44u); struct Cyc_Stdio_String_pa_struct _temp424;
_temp424.tag= Cyc_Stdio_String_pa; _temp424.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp212->name);{ void* _temp422=( void*)& _temp424; void* _temp420[ 1u]={
_temp422}; struct _tagged_arr _temp421={( void*) _temp420,( void*) _temp420,(
void*)( _temp420 + 1u)}; _temp419( _temp423, _temp421);}}));}}}; _pop_region(
rgn);} goto _LL148; _LL176: goto _LL178; _LL178: goto _LL180; _LL180: t= Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt* _temp426=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp426->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp426;}));
goto _LL148; _LL148:;} tcpat_end: p->topt=({ struct Cyc_Core_Opt* _temp427=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp427->v=(
void*) t; _temp427;}); return({ struct _tuple6 _temp428; _temp428.f1= tv_result;
_temp428.f2= v_result; _temp428;});}} struct _tuple6 Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p, void** topt, void** region_opt){ struct _tuple6
_temp429= Cyc_Tcpat_tcPatRec( te, p, topt, region_opt);{ struct _RegionHandle
_temp430= _new_region(); struct _RegionHandle* r=& _temp430; _push_region( r);
Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tagged_arr*(* f)( struct Cyc_Absyn_Vardecl*), struct Cyc_List_List* x))
Cyc_List_rmap)( r, Cyc_Tcpat_get_name, _temp429.f2), p->loc, _tag_arr("pattern contains a repeated variable",
sizeof( unsigned char), 37u));; _pop_region( r);} return _temp429;} void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p){ void* _temp431=( void*) p->r;
struct Cyc_Absyn_Pat* _temp443; struct Cyc_List_List* _temp445; struct Cyc_List_List*
_temp447; struct Cyc_List_List* _temp449; _LL433: if(( unsigned int) _temp431 >
2u?*(( int*) _temp431) == Cyc_Absyn_Pointer_p: 0){ _LL444: _temp443=(( struct
Cyc_Absyn_Pointer_p_struct*) _temp431)->f1; goto _LL434;} else{ goto _LL435;}
_LL435: if(( unsigned int) _temp431 > 2u?*(( int*) _temp431) == Cyc_Absyn_Tunion_p:
0){ _LL446: _temp445=(( struct Cyc_Absyn_Tunion_p_struct*) _temp431)->f4; goto
_LL436;} else{ goto _LL437;} _LL437: if(( unsigned int) _temp431 > 2u?*(( int*)
_temp431) == Cyc_Absyn_Struct_p: 0){ _LL448: _temp447=(( struct Cyc_Absyn_Struct_p_struct*)
_temp431)->f4; goto _LL438;} else{ goto _LL439;} _LL439: if(( unsigned int)
_temp431 > 2u?*(( int*) _temp431) == Cyc_Absyn_Tuple_p: 0){ _LL450: _temp449=((
struct Cyc_Absyn_Tuple_p_struct*) _temp431)->f1; goto _LL440;} else{ goto _LL441;}
_LL441: goto _LL442; _LL434: Cyc_Tcpat_check_pat_regions( te, _temp443);{ void*
_temp451=( void*)(( struct Cyc_Core_Opt*) _check_null( p->topt))->v; struct Cyc_Absyn_PtrInfo
_temp459; void* _temp461; struct Cyc_Absyn_TunionInfo _temp463; void* _temp465;
_LL453: if(( unsigned int) _temp451 > 4u?*(( int*) _temp451) == Cyc_Absyn_PointerType:
0){ _LL460: _temp459=(( struct Cyc_Absyn_PointerType_struct*) _temp451)->f1;
_LL462: _temp461=( void*) _temp459.rgn_typ; goto _LL454;} else{ goto _LL455;}
_LL455: if(( unsigned int) _temp451 > 4u?*(( int*) _temp451) == Cyc_Absyn_TunionType:
0){ _LL464: _temp463=(( struct Cyc_Absyn_TunionType_struct*) _temp451)->f1;
_LL466: _temp465=( void*) _temp463.rgn; goto _LL456;} else{ goto _LL457;} _LL457:
goto _LL458; _LL454: _temp465= _temp461; goto _LL456; _LL456: Cyc_Tcenv_check_rgn_accessible(
te, p->loc, _temp465); return; _LL458:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp467=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp467[ 0]=({ struct Cyc_Core_Impossible_struct _temp468; _temp468.tag= Cyc_Core_Impossible;
_temp468.f1= _tag_arr("check_pat_regions: bad pointer type", sizeof(
unsigned char), 36u); _temp468;}); _temp467;})); _LL452:;} _LL436: for( 0;
_temp445 != 0; _temp445=(( struct Cyc_List_List*) _check_null( _temp445))->tl){
Cyc_Tcpat_check_pat_regions( te,( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp445))->hd);}{ void* _temp469=( void*)(( struct Cyc_Core_Opt*)
_check_null( p->topt))->v; struct Cyc_Absyn_TunionInfo _temp477; void* _temp479;
_LL471: if(( unsigned int) _temp469 > 4u?*(( int*) _temp469) == Cyc_Absyn_TunionType:
0){ _LL478: _temp477=(( struct Cyc_Absyn_TunionType_struct*) _temp469)->f1;
_LL480: _temp479=( void*) _temp477.rgn; goto _LL472;} else{ goto _LL473;} _LL473:
if(( unsigned int) _temp469 > 4u?*(( int*) _temp469) == Cyc_Absyn_TunionFieldType:
0){ goto _LL474;} else{ goto _LL475;} _LL475: goto _LL476; _LL472: Cyc_Tcenv_check_rgn_accessible(
te, p->loc, _temp479); return; _LL474: return; _LL476:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp481=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp481[ 0]=({ struct
Cyc_Core_Impossible_struct _temp482; _temp482.tag= Cyc_Core_Impossible; _temp482.f1=
_tag_arr("check_pat_regions: bad tunion type", sizeof( unsigned char), 35u);
_temp482;}); _temp481;})); _LL470:;} _LL438: _temp449=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Pat*(* f)( struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Pat*(*)( struct _tuple7*)) Cyc_Core_snd, _temp447); goto _LL440;
_LL440: for( 0; _temp449 != 0; _temp449=(( struct Cyc_List_List*) _check_null(
_temp449))->tl){ Cyc_Tcpat_check_pat_regions( te,( struct Cyc_Absyn_Pat*)((
struct Cyc_List_List*) _check_null( _temp449))->hd);} return; _LL442: return;
_LL432:;} static const int Cyc_Tcpat_Name_v= 0; struct Cyc_Tcpat_Name_v_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Tcpat_Int_v= 1; struct
Cyc_Tcpat_Int_v_struct{ int tag; int f1; } ; struct Cyc_Tcpat_Con_s{ void* name;
int arity; struct Cyc_Core_Opt* span; } ; static const int Cyc_Tcpat_Any= 0;
static const int Cyc_Tcpat_Con= 0; struct Cyc_Tcpat_Con_struct{ int tag; struct
Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2; } ; static int Cyc_Tcpat_compare_con(
struct Cyc_Tcpat_Con_s* c1, struct Cyc_Tcpat_Con_s* c2){ void* _temp483=( void*)
c1->name; struct _tagged_arr _temp489; int _temp491; _LL485: if(*(( int*)
_temp483) == Cyc_Tcpat_Name_v){ _LL490: _temp489=(( struct Cyc_Tcpat_Name_v_struct*)
_temp483)->f1; goto _LL486;} else{ goto _LL487;} _LL487: if(*(( int*) _temp483)
== Cyc_Tcpat_Int_v){ _LL492: _temp491=(( struct Cyc_Tcpat_Int_v_struct*)
_temp483)->f1; goto _LL488;} else{ goto _LL484;} _LL486: { void* _temp493=( void*)
c2->name; struct _tagged_arr _temp499; _LL495: if(*(( int*) _temp493) == Cyc_Tcpat_Name_v){
_LL500: _temp499=(( struct Cyc_Tcpat_Name_v_struct*) _temp493)->f1; goto _LL496;}
else{ goto _LL497;} _LL497: if(*(( int*) _temp493) == Cyc_Tcpat_Int_v){ goto
_LL498;} else{ goto _LL494;} _LL496: return Cyc_String_zstrcmp( _temp489,
_temp499); _LL498: return - 1; _LL494:;} _LL488: { void* _temp501=( void*) c2->name;
int _temp507; _LL503: if(*(( int*) _temp501) == Cyc_Tcpat_Name_v){ goto _LL504;}
else{ goto _LL505;} _LL505: if(*(( int*) _temp501) == Cyc_Tcpat_Int_v){ _LL508:
_temp507=(( struct Cyc_Tcpat_Int_v_struct*) _temp501)->f1; goto _LL506;} else{
goto _LL502;} _LL504: return 1; _LL506: return _temp491 - _temp507; _LL502:;}
_LL484:;} static struct Cyc_Set_Set* Cyc_Tcpat_empty_con_set(){ return(( struct
Cyc_Set_Set*(*)( int(* comp)( struct Cyc_Tcpat_Con_s*, struct Cyc_Tcpat_Con_s*)))
Cyc_Set_empty)( Cyc_Tcpat_compare_con);} static struct Cyc_Core_Opt Cyc_Tcpat_one_opt={(
void*) 1}; static struct Cyc_Core_Opt Cyc_Tcpat_two_opt={( void*) 2}; static
struct Cyc_Core_Opt* Cyc_Tcpat_one_opt_ptr=( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt;
static unsigned char _temp510[ 5u]="null"; static struct Cyc_Tcpat_Name_v_struct
Cyc_Tcpat_null_name_value={ 0u,{ _temp510, _temp510, _temp510 + 5u}}; static
unsigned char _temp512[ 4u]="ptr"; static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_ptr_name_value={
0u,{ _temp512, _temp512, _temp512 + 4u}}; static unsigned char _temp514[ 6u]="tuple";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={ 0u,{ _temp514,
_temp514, _temp514 + 6u}}; static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_con0={(
void*)(( void*)& Cyc_Tcpat_null_name_value), 0,( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt};
static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_ptr_con0={( void*)(( void*)& Cyc_Tcpat_ptr_name_value),
1,( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt}; static struct Cyc_Tcpat_Con_s Cyc_Tcpat_ptr_con0={(
void*)(( void*)& Cyc_Tcpat_ptr_name_value), 1,( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt};
static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_con=& Cyc_Tcpat_null_con0; static
struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_ptr_con=& Cyc_Tcpat_null_ptr_con0; static
struct Cyc_Tcpat_Con_s* Cyc_Tcpat_ptr_con=& Cyc_Tcpat_ptr_con0; static struct
Cyc_Tcpat_Con_s* Cyc_Tcpat_int_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp515=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp515->name=( void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp516=(
struct Cyc_Tcpat_Int_v_struct*) GC_malloc_atomic( sizeof( struct Cyc_Tcpat_Int_v_struct));
_temp516[ 0]=({ struct Cyc_Tcpat_Int_v_struct _temp517; _temp517.tag= Cyc_Tcpat_Int_v;
_temp517.f1= i; _temp517;}); _temp516;})); _temp515->arity= 0; _temp515->span= 0;
_temp515;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_float_con( struct
_tagged_arr f){ return({ struct Cyc_Tcpat_Con_s* _temp518=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp518->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp519=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp519[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp520; _temp520.tag= Cyc_Tcpat_Name_v; _temp520.f1= f; _temp520;}); _temp519;}));
_temp518->arity= 0; _temp518->span= 0; _temp518;});} static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_char_con( unsigned char c){ return({ struct Cyc_Tcpat_Con_s* _temp521=(
struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp521->name=(
void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp523=( struct Cyc_Tcpat_Int_v_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Tcpat_Int_v_struct)); _temp523[ 0]=({
struct Cyc_Tcpat_Int_v_struct _temp524; _temp524.tag= Cyc_Tcpat_Int_v; _temp524.f1=(
int) c; _temp524;}); _temp523;})); _temp521->arity= 0; _temp521->span=({ struct
Cyc_Core_Opt* _temp522=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct
Cyc_Core_Opt)); _temp522->v=( void*) 256; _temp522;}); _temp521;});} static
struct Cyc_Tcpat_Con_s* Cyc_Tcpat_tuple_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp525=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp525->name=( void*)(( void*)& Cyc_Tcpat_tuple_name_value); _temp525->arity=
i; _temp525->span= Cyc_Tcpat_one_opt_ptr; _temp525;});} static void* Cyc_Tcpat_null_pat(){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp526=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp526[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp527; _temp527.tag= Cyc_Tcpat_Con; _temp527.f1= Cyc_Tcpat_null_con; _temp527.f2=
0; _temp527;}); _temp526;});} static void* Cyc_Tcpat_int_pat( int i){ return(
void*)({ struct Cyc_Tcpat_Con_struct* _temp528=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp528[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp529; _temp529.tag= Cyc_Tcpat_Con; _temp529.f1= Cyc_Tcpat_int_con( i);
_temp529.f2= 0; _temp529;}); _temp528;});} static void* Cyc_Tcpat_char_pat(
unsigned char c){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp530=(
struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp530[ 0]=({ struct Cyc_Tcpat_Con_struct _temp531; _temp531.tag= Cyc_Tcpat_Con;
_temp531.f1= Cyc_Tcpat_char_con( c); _temp531.f2= 0; _temp531;}); _temp530;});}
static void* Cyc_Tcpat_float_pat( struct _tagged_arr f){ return( void*)({ struct
Cyc_Tcpat_Con_struct* _temp532=( struct Cyc_Tcpat_Con_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Con_struct)); _temp532[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp533; _temp533.tag= Cyc_Tcpat_Con; _temp533.f1= Cyc_Tcpat_float_con( f);
_temp533.f2= 0; _temp533;}); _temp532;});} static void* Cyc_Tcpat_null_ptr_pat(
void* p){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp534=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp534[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp535; _temp535.tag= Cyc_Tcpat_Con; _temp535.f1= Cyc_Tcpat_null_ptr_con;
_temp535.f2=({ struct Cyc_List_List* _temp536=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp536->hd=( void*) p; _temp536->tl= 0;
_temp536;}); _temp535;}); _temp534;});} static void* Cyc_Tcpat_ptr_pat( void* p){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp537=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp537[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp538; _temp538.tag= Cyc_Tcpat_Con; _temp538.f1= Cyc_Tcpat_ptr_con; _temp538.f2=({
struct Cyc_List_List* _temp539=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp539->hd=( void*) p; _temp539->tl= 0; _temp539;});
_temp538;}); _temp537;});} static void* Cyc_Tcpat_tuple_pat( struct Cyc_List_List*
ss){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp540=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp540[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp541; _temp541.tag= Cyc_Tcpat_Con; _temp541.f1= Cyc_Tcpat_tuple_con( Cyc_List_length(
ss)); _temp541.f2= ss; _temp541;}); _temp540;});} static void* Cyc_Tcpat_con_pat(
struct _tagged_arr con_name, struct Cyc_Core_Opt* span, struct Cyc_List_List* ps){
struct Cyc_Tcpat_Con_s* c=({ struct Cyc_Tcpat_Con_s* _temp544=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp544->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp545=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp545[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp546; _temp546.tag= Cyc_Tcpat_Name_v; _temp546.f1= con_name; _temp546;});
_temp545;})); _temp544->arity= Cyc_List_length( ps); _temp544->span= span;
_temp544;}); return( void*)({ struct Cyc_Tcpat_Con_struct* _temp542=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp542[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp543; _temp543.tag= Cyc_Tcpat_Con; _temp543.f1= c; _temp543.f2= ps; _temp543;});
_temp542;});} static void* Cyc_Tcpat_compile_pat( struct Cyc_Absyn_Pat* p){ void*
s;{ void* _temp547=( void*) p->r; int _temp581; void* _temp583; unsigned char
_temp585; struct _tagged_arr _temp587; struct Cyc_Absyn_Pat* _temp589; struct
Cyc_List_List* _temp591; struct Cyc_Absyn_Tunionfield* _temp593; struct Cyc_Absyn_Tuniondecl*
_temp595; struct Cyc_List_List* _temp597; struct Cyc_List_List* _temp599; struct
Cyc_List_List* _temp601; struct Cyc_Absyn_Structdecl* _temp603; struct Cyc_Absyn_Enumfield*
_temp605; struct Cyc_Absyn_Enumdecl* _temp607; _LL549: if( _temp547 ==( void*)
Cyc_Absyn_Wild_p){ goto _LL550;} else{ goto _LL551;} _LL551: if(( unsigned int)
_temp547 > 2u?*(( int*) _temp547) == Cyc_Absyn_Var_p: 0){ goto _LL552;} else{
goto _LL553;} _LL553: if( _temp547 ==( void*) Cyc_Absyn_Null_p){ goto _LL554;}
else{ goto _LL555;} _LL555: if(( unsigned int) _temp547 > 2u?*(( int*) _temp547)
== Cyc_Absyn_Int_p: 0){ _LL584: _temp583=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp547)->f1; goto _LL582; _LL582: _temp581=(( struct Cyc_Absyn_Int_p_struct*)
_temp547)->f2; goto _LL556;} else{ goto _LL557;} _LL557: if(( unsigned int)
_temp547 > 2u?*(( int*) _temp547) == Cyc_Absyn_Char_p: 0){ _LL586: _temp585=((
struct Cyc_Absyn_Char_p_struct*) _temp547)->f1; goto _LL558;} else{ goto _LL559;}
_LL559: if(( unsigned int) _temp547 > 2u?*(( int*) _temp547) == Cyc_Absyn_Float_p:
0){ _LL588: _temp587=(( struct Cyc_Absyn_Float_p_struct*) _temp547)->f1; goto
_LL560;} else{ goto _LL561;} _LL561: if(( unsigned int) _temp547 > 2u?*(( int*)
_temp547) == Cyc_Absyn_Reference_p: 0){ goto _LL562;} else{ goto _LL563;} _LL563:
if(( unsigned int) _temp547 > 2u?*(( int*) _temp547) == Cyc_Absyn_Pointer_p: 0){
_LL590: _temp589=(( struct Cyc_Absyn_Pointer_p_struct*) _temp547)->f1; goto
_LL564;} else{ goto _LL565;} _LL565: if(( unsigned int) _temp547 > 2u?*(( int*)
_temp547) == Cyc_Absyn_Tunion_p: 0){ _LL596: _temp595=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp547)->f1; goto _LL594; _LL594: _temp593=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp547)->f2; goto _LL592; _LL592: _temp591=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp547)->f4; if( _temp591 == 0){ goto _LL566;} else{ goto _LL567;}} else{ goto
_LL567;} _LL567: if(( unsigned int) _temp547 > 2u?*(( int*) _temp547) == Cyc_Absyn_Tunion_p:
0){ _LL598: _temp597=(( struct Cyc_Absyn_Tunion_p_struct*) _temp547)->f4; goto
_LL568;} else{ goto _LL569;} _LL569: if(( unsigned int) _temp547 > 2u?*(( int*)
_temp547) == Cyc_Absyn_Tuple_p: 0){ _LL600: _temp599=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp547)->f1; goto _LL570;} else{ goto _LL571;} _LL571: if(( unsigned int)
_temp547 > 2u?*(( int*) _temp547) == Cyc_Absyn_Struct_p: 0){ _LL604: _temp603=((
struct Cyc_Absyn_Struct_p_struct*) _temp547)->f1; goto _LL602; _LL602: _temp601=((
struct Cyc_Absyn_Struct_p_struct*) _temp547)->f4; goto _LL572;} else{ goto
_LL573;} _LL573: if(( unsigned int) _temp547 > 2u?*(( int*) _temp547) == Cyc_Absyn_Enum_p:
0){ _LL608: _temp607=(( struct Cyc_Absyn_Enum_p_struct*) _temp547)->f1; goto
_LL606; _LL606: _temp605=(( struct Cyc_Absyn_Enum_p_struct*) _temp547)->f2; goto
_LL574;} else{ goto _LL575;} _LL575: if(( unsigned int) _temp547 > 2u?*(( int*)
_temp547) == Cyc_Absyn_UnknownId_p: 0){ goto _LL576;} else{ goto _LL577;} _LL577:
if(( unsigned int) _temp547 > 2u?*(( int*) _temp547) == Cyc_Absyn_UnknownCall_p:
0){ goto _LL578;} else{ goto _LL579;} _LL579: if(( unsigned int) _temp547 > 2u?*((
int*) _temp547) == Cyc_Absyn_UnknownFields_p: 0){ goto _LL580;} else{ goto
_LL548;} _LL550: s=( void*) Cyc_Tcpat_Any; goto _LL548; _LL552: s=( void*) Cyc_Tcpat_Any;
goto _LL548; _LL554: s= Cyc_Tcpat_null_pat(); goto _LL548; _LL556: s= Cyc_Tcpat_int_pat(
_temp581); goto _LL548; _LL558: s= Cyc_Tcpat_char_pat( _temp585); goto _LL548;
_LL560: s= Cyc_Tcpat_float_pat( _temp587); goto _LL548; _LL562: s=( void*) Cyc_Tcpat_Any;
goto _LL548; _LL564:{ void* _temp609= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( p->topt))->v); struct Cyc_Absyn_PtrInfo _temp617; struct Cyc_Absyn_Conref*
_temp619; _LL611: if(( unsigned int) _temp609 > 4u?*(( int*) _temp609) == Cyc_Absyn_PointerType:
0){ _LL618: _temp617=(( struct Cyc_Absyn_PointerType_struct*) _temp609)->f1;
_LL620: _temp619= _temp617.nullable; goto _LL612;} else{ goto _LL613;} _LL613:
if(( unsigned int) _temp609 > 4u?*(( int*) _temp609) == Cyc_Absyn_TunionType: 0){
goto _LL614;} else{ goto _LL615;} _LL615: goto _LL616; _LL612: { int is_nullable=
0; int still_working= 1; while( still_working) { void* _temp621=( void*)
_temp619->v; struct Cyc_Absyn_Conref* _temp629; int _temp631; _LL623: if((
unsigned int) _temp621 > 1u?*(( int*) _temp621) == Cyc_Absyn_Forward_constr: 0){
_LL630: _temp629=(( struct Cyc_Absyn_Forward_constr_struct*) _temp621)->f1; goto
_LL624;} else{ goto _LL625;} _LL625: if( _temp621 ==( void*) Cyc_Absyn_No_constr){
goto _LL626;} else{ goto _LL627;} _LL627: if(( unsigned int) _temp621 > 1u?*((
int*) _temp621) == Cyc_Absyn_Eq_constr: 0){ _LL632: _temp631=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp621)->f1; goto _LL628;} else{ goto _LL622;} _LL624:( void*)( _temp619->v=(
void*)(( void*) _temp629->v)); continue; _LL626:( void*)( _temp619->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp633=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp633[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp634; _temp634.tag= Cyc_Absyn_Eq_constr; _temp634.f1=(
void*) 0; _temp634;}); _temp633;}))); is_nullable= 0; still_working= 0; goto
_LL622; _LL628: is_nullable=( int) _temp631; still_working= 0; goto _LL622;
_LL622:;}{ void* ss= Cyc_Tcpat_compile_pat( _temp589); if( is_nullable){ s= Cyc_Tcpat_null_ptr_pat(
ss);} else{ s= Cyc_Tcpat_ptr_pat( ss);} goto _LL610;}} _LL614:{ void* _temp635=(
void*) _temp589->r; struct Cyc_List_List* _temp641; struct Cyc_Absyn_Tunionfield*
_temp643; struct Cyc_Absyn_Tuniondecl* _temp645; _LL637: if(( unsigned int)
_temp635 > 2u?*(( int*) _temp635) == Cyc_Absyn_Tunion_p: 0){ _LL646: _temp645=((
struct Cyc_Absyn_Tunion_p_struct*) _temp635)->f1; goto _LL644; _LL644: _temp643=((
struct Cyc_Absyn_Tunion_p_struct*) _temp635)->f2; goto _LL642; _LL642: _temp641=((
struct Cyc_Absyn_Tunion_p_struct*) _temp635)->f4; goto _LL638;} else{ goto
_LL639;} _LL639: goto _LL640; _LL638: { struct Cyc_Core_Opt* span; if( _temp645->is_xtunion){
span= 0;} else{ span=({ struct Cyc_Core_Opt* _temp647=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp647->v=( void*)(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp645->fields))->v); _temp647;});} s= Cyc_Tcpat_con_pat(*(*
_temp643->name).f2, span,(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat, _temp641)); goto
_LL636;} _LL640: s= Cyc_Tcutil_impos( _tag_arr("non-[x]tunion pattern has tunion type",
sizeof( unsigned char), 38u)); goto _LL636; _LL636:;} goto _LL610; _LL616: s=
Cyc_Tcutil_impos( _tag_arr("pointer pattern does not have pointer type!",
sizeof( unsigned char), 44u)); goto _LL610; _LL610:;} goto _LL548; _LL566: {
struct Cyc_Core_Opt* span;{ void* _temp648= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( p->topt))->v); _LL650: if(( unsigned int)
_temp648 > 4u?*(( int*) _temp648) == Cyc_Absyn_TunionType: 0){ goto _LL651;}
else{ goto _LL652;} _LL652: if(( unsigned int) _temp648 > 4u?*(( int*) _temp648)
== Cyc_Absyn_TunionFieldType: 0){ goto _LL653;} else{ goto _LL654;} _LL654: goto
_LL655; _LL651: if( _temp595->is_xtunion){ span= 0;} else{ span=({ struct Cyc_Core_Opt*
_temp656=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct Cyc_Core_Opt));
_temp656->v=( void*)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp595->fields))->v);
_temp656;});} goto _LL649; _LL653: span=({ struct Cyc_Core_Opt* _temp657=(
struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp657->v=(
void*) 1; _temp657;}); goto _LL649; _LL655: span=(( struct Cyc_Core_Opt*(*)(
struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("void tunion pattern has bad type",
sizeof( unsigned char), 33u)); goto _LL649; _LL649:;} s= Cyc_Tcpat_con_pat(*(*
_temp593->name).f2, span, 0); goto _LL548;} _LL568: _temp599= _temp597; goto
_LL570; _LL570: s= Cyc_Tcpat_tuple_pat((( struct Cyc_List_List*(*)( void*(* f)(
struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp599)); goto _LL548; _LL572: { struct Cyc_List_List* ps= 0;{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp603->fields))->v;
for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){
int found= Cyc_String_strcmp(*(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fields))->hd)->name, _tag_arr("", sizeof( unsigned char), 1u)) == 0;{
struct Cyc_List_List* dlps0= _temp601; for( 0; ! found? dlps0 != 0: 0; dlps0=((
struct Cyc_List_List*) _check_null( dlps0))->tl){ struct _tuple7 _temp660;
struct Cyc_Absyn_Pat* _temp661; struct Cyc_List_List* _temp663; struct _tuple7*
_temp658=( struct _tuple7*)(( struct Cyc_List_List*) _check_null( dlps0))->hd;
_temp660=* _temp658; _LL664: _temp663= _temp660.f1; goto _LL662; _LL662:
_temp661= _temp660.f2; goto _LL659; _LL659: { struct Cyc_List_List* _temp665=
_temp663; struct Cyc_List_List _temp671; struct Cyc_List_List* _temp672; void*
_temp674; struct _tagged_arr* _temp676; _LL667: if( _temp665 == 0){ goto _LL669;}
else{ _temp671=* _temp665; _LL675: _temp674=( void*) _temp671.hd; if(*(( int*)
_temp674) == Cyc_Absyn_FieldName){ _LL677: _temp676=(( struct Cyc_Absyn_FieldName_struct*)
_temp674)->f1; goto _LL673;} else{ goto _LL669;} _LL673: _temp672= _temp671.tl;
if( _temp672 == 0){ goto _LL668;} else{ goto _LL669;}} _LL669: goto _LL670;
_LL668: if( Cyc_String_zstrptrcmp( _temp676,(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( fields))->hd)->name) == 0){ ps=({ struct Cyc_List_List*
_temp678=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp678->hd=( void*) Cyc_Tcpat_compile_pat( _temp661); _temp678->tl= ps;
_temp678;}); found= 1;} goto _LL666; _LL670:(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("bad designator(s)", sizeof( unsigned char), 18u)); goto _LL666; _LL666:;}}}
if( ! found){(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("bad designator",
sizeof( unsigned char), 15u));}}} s= Cyc_Tcpat_tuple_pat( ps); goto _LL548;}
_LL574: { int span=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp607->fields))->v);
s= Cyc_Tcpat_con_pat(*(* _temp605->name).f2,({ struct Cyc_Core_Opt* _temp679=(
struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp679->v=(
void*) span; _temp679;}), 0); goto _LL548;} _LL576: s= Cyc_Tcutil_impos(
_tag_arr("compile_pat: unknownid", sizeof( unsigned char), 23u)); goto _LL548;
_LL578: s= Cyc_Tcutil_impos( _tag_arr("compile_pat: unknowncall", sizeof(
unsigned char), 25u)); goto _LL548; _LL580: s= Cyc_Tcutil_impos( _tag_arr("compile_pat: unknownfields",
sizeof( unsigned char), 27u)); goto _LL548; _LL548:;} return s;} static const
int Cyc_Tcpat_Pos= 0; struct Cyc_Tcpat_Pos_struct{ int tag; struct Cyc_Tcpat_Con_s*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Tcpat_Neg= 1; struct Cyc_Tcpat_Neg_struct{
int tag; struct Cyc_Set_Set* f1; } ; static const int Cyc_Tcpat_Failure= 0;
static const int Cyc_Tcpat_Success= 0; struct Cyc_Tcpat_Success_struct{ int tag;
void* f1; } ; static const int Cyc_Tcpat_IfEq= 1; struct Cyc_Tcpat_IfEq_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Tcpat_Con_s* f2; void* f3; void*
f4; } ; struct _tuple10{ struct Cyc_List_List* f1; struct Cyc_List_List* f2;
struct Cyc_List_List* f3; } ; static const int Cyc_Tcpat_Yes= 0; static const
int Cyc_Tcpat_No= 1; static const int Cyc_Tcpat_Maybe= 2; static void* Cyc_Tcpat_add_neg(
void* td, struct Cyc_Tcpat_Con_s* c){ void* _temp680= td; struct Cyc_Set_Set*
_temp686; _LL682: if(*(( int*) _temp680) == Cyc_Tcpat_Neg){ _LL687: _temp686=((
struct Cyc_Tcpat_Neg_struct*) _temp680)->f1; goto _LL683;} else{ goto _LL684;}
_LL684: if(*(( int*) _temp680) == Cyc_Tcpat_Pos){ goto _LL685;} else{ goto
_LL681;} _LL683: if((( int(*)( struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s*
elt)) Cyc_Set_member)( _temp686, c)){(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("add_neg called when constructor already in set", sizeof( unsigned char),
47u));} if( c->span != 0?(( int(*)( struct Cyc_Set_Set* s)) Cyc_Set_cardinality)(
_temp686) + 1 >=( int)(( struct Cyc_Core_Opt*) _check_null( c->span))->v: 0){((
int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("add_neg called when |cs U {c}| >= span(c)",
sizeof( unsigned char), 42u));} return( void*)({ struct Cyc_Tcpat_Neg_struct*
_temp688=( struct Cyc_Tcpat_Neg_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct));
_temp688[ 0]=({ struct Cyc_Tcpat_Neg_struct _temp689; _temp689.tag= Cyc_Tcpat_Neg;
_temp689.f1=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s*
elt)) Cyc_Set_insert)( _temp686, c); _temp689;}); _temp688;}); _LL685: return
Cyc_Tcutil_impos( _tag_arr("add_neg called when td is Positive", sizeof(
unsigned char), 35u)); _LL681:;} static void* Cyc_Tcpat_static_match( struct Cyc_Tcpat_Con_s*
c, void* td){ void* _temp690= td; struct Cyc_Tcpat_Con_s* _temp696; struct Cyc_Set_Set*
_temp698; _LL692: if(*(( int*) _temp690) == Cyc_Tcpat_Pos){ _LL697: _temp696=((
struct Cyc_Tcpat_Pos_struct*) _temp690)->f1; goto _LL693;} else{ goto _LL694;}
_LL694: if(*(( int*) _temp690) == Cyc_Tcpat_Neg){ _LL699: _temp698=(( struct Cyc_Tcpat_Neg_struct*)
_temp690)->f1; goto _LL695;} else{ goto _LL691;} _LL693: if( Cyc_Tcpat_compare_con(
c, _temp696) == 0){ return( void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_No;}
_LL695: if((( int(*)( struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)(
_temp698, c)){ return( void*) Cyc_Tcpat_No;} else{ if( c->span != 0?( int)((
struct Cyc_Core_Opt*) _check_null( c->span))->v ==(( int(*)( struct Cyc_Set_Set*
s)) Cyc_Set_cardinality)( _temp698) + 1: 0){ return( void*) Cyc_Tcpat_Yes;}
else{ return( void*) Cyc_Tcpat_Maybe;}} _LL691:;} struct _tuple11{ struct Cyc_Tcpat_Con_s*
f1; struct Cyc_List_List* f2; } ; static struct Cyc_List_List* Cyc_Tcpat_augment(
struct Cyc_List_List* ctxt, void* dsc){ struct Cyc_List_List* _temp700= ctxt;
struct Cyc_List_List _temp706; struct Cyc_List_List* _temp707; struct _tuple11*
_temp709; struct _tuple11 _temp711; struct Cyc_List_List* _temp712; struct Cyc_Tcpat_Con_s*
_temp714; _LL702: if( _temp700 == 0){ goto _LL703;} else{ goto _LL704;} _LL704:
if( _temp700 == 0){ goto _LL701;} else{ _temp706=* _temp700; _LL710: _temp709=(
struct _tuple11*) _temp706.hd; _temp711=* _temp709; _LL715: _temp714= _temp711.f1;
goto _LL713; _LL713: _temp712= _temp711.f2; goto _LL708; _LL708: _temp707=
_temp706.tl; goto _LL705;} _LL703: return 0; _LL705: return({ struct Cyc_List_List*
_temp716=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp716->hd=( void*)({ struct _tuple11* _temp717=( struct _tuple11*) GC_malloc(
sizeof( struct _tuple11)); _temp717->f1= _temp714; _temp717->f2=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp718=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp718->hd=( void*) dsc; _temp718->tl= _temp712;
_temp718;}); _temp717;}); _temp716->tl= _temp707; _temp716;}); _LL701:;} static
struct Cyc_List_List* Cyc_Tcpat_norm_context( struct Cyc_List_List* ctxt){
struct Cyc_List_List* _temp719= ctxt; struct Cyc_List_List _temp725; struct Cyc_List_List*
_temp726; struct _tuple11* _temp728; struct _tuple11 _temp730; struct Cyc_List_List*
_temp731; struct Cyc_Tcpat_Con_s* _temp733; _LL721: if( _temp719 == 0){ goto
_LL722;} else{ goto _LL723;} _LL723: if( _temp719 == 0){ goto _LL720;} else{
_temp725=* _temp719; _LL729: _temp728=( struct _tuple11*) _temp725.hd; _temp730=*
_temp728; _LL734: _temp733= _temp730.f1; goto _LL732; _LL732: _temp731= _temp730.f2;
goto _LL727; _LL727: _temp726= _temp725.tl; goto _LL724;} _LL722: return((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("norm_context: empty context",
sizeof( unsigned char), 28u)); _LL724: return Cyc_Tcpat_augment( _temp726,( void*)({
struct Cyc_Tcpat_Pos_struct* _temp735=( struct Cyc_Tcpat_Pos_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Pos_struct)); _temp735[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp736; _temp736.tag= Cyc_Tcpat_Pos; _temp736.f1= _temp733; _temp736.f2= Cyc_List_rev(
_temp731); _temp736;}); _temp735;})); _LL720:;} static void* Cyc_Tcpat_build_desc(
struct Cyc_List_List* ctxt, void* dsc, struct Cyc_List_List* work){ struct
_tuple6 _temp738=({ struct _tuple6 _temp737; _temp737.f1= ctxt; _temp737.f2=
work; _temp737;}); struct Cyc_List_List* _temp748; struct Cyc_List_List*
_temp750; struct Cyc_List_List* _temp752; struct Cyc_List_List* _temp754; struct
Cyc_List_List* _temp756; struct Cyc_List_List _temp758; struct Cyc_List_List*
_temp759; struct _tuple10* _temp761; struct _tuple10 _temp763; struct Cyc_List_List*
_temp764; struct Cyc_List_List* _temp766; struct Cyc_List_List _temp768; struct
Cyc_List_List* _temp769; struct _tuple11* _temp771; struct _tuple11 _temp773;
struct Cyc_List_List* _temp774; struct Cyc_Tcpat_Con_s* _temp776; _LL740: _LL751:
_temp750= _temp738.f1; if( _temp750 == 0){ goto _LL749;} else{ goto _LL742;}
_LL749: _temp748= _temp738.f2; if( _temp748 == 0){ goto _LL741;} else{ goto
_LL742;} _LL742: _LL753: _temp752= _temp738.f1; if( _temp752 == 0){ goto _LL743;}
else{ goto _LL744;} _LL744: _LL755: _temp754= _temp738.f2; if( _temp754 == 0){
goto _LL745;} else{ goto _LL746;} _LL746: _LL767: _temp766= _temp738.f1; if(
_temp766 == 0){ goto _LL739;} else{ _temp768=* _temp766; _LL772: _temp771=(
struct _tuple11*) _temp768.hd; _temp773=* _temp771; _LL777: _temp776= _temp773.f1;
goto _LL775; _LL775: _temp774= _temp773.f2; goto _LL770; _LL770: _temp769=
_temp768.tl; goto _LL757;} _LL757: _temp756= _temp738.f2; if( _temp756 == 0){
goto _LL739;} else{ _temp758=* _temp756; _LL762: _temp761=( struct _tuple10*)
_temp758.hd; _temp763=* _temp761; _LL765: _temp764= _temp763.f3; goto _LL760;
_LL760: _temp759= _temp758.tl; goto _LL747;} _LL741: return dsc; _LL743: return
Cyc_Tcutil_impos( _tag_arr("build_desc: ctxt and work don't match", sizeof(
unsigned char), 38u)); _LL745: return Cyc_Tcutil_impos( _tag_arr("build_desc: ctxt and work don't match",
sizeof( unsigned char), 38u)); _LL747: { struct Cyc_Tcpat_Pos_struct* _temp778=({
struct Cyc_Tcpat_Pos_struct* _temp779=( struct Cyc_Tcpat_Pos_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Pos_struct)); _temp779[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp780; _temp780.tag= Cyc_Tcpat_Pos; _temp780.f1= _temp776; _temp780.f2= Cyc_List_append(
Cyc_List_rev( _temp774),({ struct Cyc_List_List* _temp781=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp781->hd=( void*) dsc; _temp781->tl=
_temp764; _temp781;})); _temp780;}); _temp779;}); return Cyc_Tcpat_build_desc(
_temp769,( void*) _temp778, _temp759);} _LL739:;} static void* Cyc_Tcpat_match(
void* p, struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List* ctx, struct
Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List* rules); static
void* Cyc_Tcpat_or_match( void* dsc, struct Cyc_List_List* allmrules){ struct
Cyc_List_List* _temp782= allmrules; struct Cyc_List_List _temp788; struct Cyc_List_List*
_temp789; struct _tuple0* _temp791; struct _tuple0 _temp793; void* _temp794;
void* _temp796; _LL784: if( _temp782 == 0){ goto _LL785;} else{ goto _LL786;}
_LL786: if( _temp782 == 0){ goto _LL783;} else{ _temp788=* _temp782; _LL792:
_temp791=( struct _tuple0*) _temp788.hd; _temp793=* _temp791; _LL797: _temp796=
_temp793.f1; goto _LL795; _LL795: _temp794= _temp793.f2; goto _LL790; _LL790:
_temp789= _temp788.tl; goto _LL787;} _LL785: return( void*) Cyc_Tcpat_Failure;
_LL787: return Cyc_Tcpat_match( _temp796, 0, dsc, 0, 0, _temp794, _temp789);
_LL783:;} static void* Cyc_Tcpat_match_compile( struct Cyc_List_List* allmrules){
return Cyc_Tcpat_or_match(( void*)({ struct Cyc_Tcpat_Neg_struct* _temp798=(
struct Cyc_Tcpat_Neg_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct));
_temp798[ 0]=({ struct Cyc_Tcpat_Neg_struct _temp799; _temp799.tag= Cyc_Tcpat_Neg;
_temp799.f1= Cyc_Tcpat_empty_con_set(); _temp799;}); _temp798;}), allmrules);}
static void* Cyc_Tcpat_and_match( struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules){ struct Cyc_List_List*
_temp800= work; struct Cyc_List_List _temp808; struct Cyc_List_List* _temp809;
struct _tuple10* _temp811; struct _tuple10 _temp813; struct Cyc_List_List*
_temp814; struct Cyc_List_List* _temp816; struct Cyc_List_List* _temp818; struct
Cyc_List_List _temp820; struct Cyc_List_List* _temp821; struct _tuple10*
_temp823; struct _tuple10 _temp825; struct Cyc_List_List* _temp826; struct Cyc_List_List*
_temp828; struct Cyc_List_List* _temp830; _LL802: if( _temp800 == 0){ goto
_LL803;} else{ goto _LL804;} _LL804: if( _temp800 == 0){ goto _LL806;} else{
_temp808=* _temp800; _LL812: _temp811=( struct _tuple10*) _temp808.hd; _temp813=*
_temp811; _LL819: _temp818= _temp813.f1; if( _temp818 == 0){ goto _LL817;} else{
goto _LL806;} _LL817: _temp816= _temp813.f2; if( _temp816 == 0){ goto _LL815;}
else{ goto _LL806;} _LL815: _temp814= _temp813.f3; if( _temp814 == 0){ goto
_LL810;} else{ goto _LL806;} _LL810: _temp809= _temp808.tl; goto _LL805;} _LL806:
if( _temp800 == 0){ goto _LL801;} else{ _temp820=* _temp800; _LL824: _temp823=(
struct _tuple10*) _temp820.hd; _temp825=* _temp823; _LL831: _temp830= _temp825.f1;
goto _LL829; _LL829: _temp828= _temp825.f2; goto _LL827; _LL827: _temp826=
_temp825.f3; goto _LL822; _LL822: _temp821= _temp820.tl; goto _LL807;} _LL803:
return( void*)({ struct Cyc_Tcpat_Success_struct* _temp832=( struct Cyc_Tcpat_Success_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Success_struct)); _temp832[ 0]=({ struct Cyc_Tcpat_Success_struct
_temp833; _temp833.tag= Cyc_Tcpat_Success; _temp833.f1=( void*) right_hand_side;
_temp833;}); _temp832;}); _LL805: return Cyc_Tcpat_and_match( Cyc_Tcpat_norm_context(
ctx), _temp809, right_hand_side, rules); _LL807: if(( _temp830 == 0? 1: _temp828
== 0)? 1: _temp826 == 0){ return Cyc_Tcutil_impos( _tag_arr("tcpat:and_match: malformed work frame",
sizeof( unsigned char), 38u));}{ struct Cyc_List_List _temp836; struct Cyc_List_List*
_temp837; void* _temp839; struct Cyc_List_List* _temp834=( struct Cyc_List_List*)
_check_null( _temp830); _temp836=* _temp834; _LL840: _temp839=( void*) _temp836.hd;
goto _LL838; _LL838: _temp837= _temp836.tl; goto _LL835; _LL835: { struct Cyc_List_List
_temp843; struct Cyc_List_List* _temp844; struct Cyc_List_List* _temp846; struct
Cyc_List_List* _temp841=( struct Cyc_List_List*) _check_null( _temp828);
_temp843=* _temp841; _LL847: _temp846=( struct Cyc_List_List*) _temp843.hd; goto
_LL845; _LL845: _temp844= _temp843.tl; goto _LL842; _LL842: { struct Cyc_List_List
_temp850; struct Cyc_List_List* _temp851; void* _temp853; struct Cyc_List_List*
_temp848=( struct Cyc_List_List*) _check_null( _temp826); _temp850=* _temp848;
_LL854: _temp853=( void*) _temp850.hd; goto _LL852; _LL852: _temp851= _temp850.tl;
goto _LL849; _LL849: { struct _tuple10* _temp855=({ struct _tuple10* _temp857=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp857->f1= _temp837;
_temp857->f2= _temp844; _temp857->f3= _temp851; _temp857;}); return Cyc_Tcpat_match(
_temp839, _temp846, _temp853, ctx,({ struct Cyc_List_List* _temp856=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp856->hd=( void*) _temp855;
_temp856->tl= _temp821; _temp856;}), right_hand_side, rules);}}}} _LL801:;}
static struct Cyc_List_List* Cyc_Tcpat_getdargs( struct Cyc_Tcpat_Con_s* pcon,
void* dsc){ void* _temp858= dsc; struct Cyc_Set_Set* _temp864; struct Cyc_List_List*
_temp866; _LL860: if(*(( int*) _temp858) == Cyc_Tcpat_Neg){ _LL865: _temp864=((
struct Cyc_Tcpat_Neg_struct*) _temp858)->f1; goto _LL861;} else{ goto _LL862;}
_LL862: if(*(( int*) _temp858) == Cyc_Tcpat_Pos){ _LL867: _temp866=(( struct Cyc_Tcpat_Pos_struct*)
_temp858)->f2; goto _LL863;} else{ goto _LL859;} _LL861: { void* any=( void*)({
struct Cyc_Tcpat_Neg_struct* _temp870=( struct Cyc_Tcpat_Neg_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Neg_struct)); _temp870[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp871; _temp871.tag= Cyc_Tcpat_Neg; _temp871.f1= Cyc_Tcpat_empty_con_set();
_temp871;}); _temp870;}); struct Cyc_List_List* _temp868= 0;{ int i= 0; for( 0;
i < pcon->arity; ++ i){ _temp868=({ struct Cyc_List_List* _temp869=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp869->hd=( void*) any; _temp869->tl=
_temp868; _temp869;});}} return _temp868;} _LL863: return _temp866; _LL859:;}
static struct Cyc_List_List* Cyc_Tcpat_getoarg( struct Cyc_List_List* obj, int i){
return({ struct Cyc_List_List* _temp872=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp872->hd=( void*)( i + 1); _temp872->tl= obj;
_temp872;});} static struct Cyc_List_List* Cyc_Tcpat_getoargs( struct Cyc_Tcpat_Con_s*
pcon, struct Cyc_List_List* obj){ return(( struct Cyc_List_List*(*)( int n,
struct Cyc_List_List*(* f)( struct Cyc_List_List*, int), struct Cyc_List_List*
env)) Cyc_List_tabulate_c)( pcon->arity, Cyc_Tcpat_getoarg, obj);} static void*
Cyc_Tcpat_match( void* p, struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules){ void* _temp873= p; struct Cyc_List_List* _temp879; struct Cyc_Tcpat_Con_s*
_temp881; _LL875: if( _temp873 ==( void*) Cyc_Tcpat_Any){ goto _LL876;} else{
goto _LL877;} _LL877: if(( unsigned int) _temp873 > 1u?*(( int*) _temp873) ==
Cyc_Tcpat_Con: 0){ _LL882: _temp881=(( struct Cyc_Tcpat_Con_struct*) _temp873)->f1;
goto _LL880; _LL880: _temp879=(( struct Cyc_Tcpat_Con_struct*) _temp873)->f2;
goto _LL878;} else{ goto _LL874;} _LL876: return Cyc_Tcpat_and_match( Cyc_Tcpat_augment(
ctx, dsc), work, right_hand_side, rules); _LL878: { void* _temp883= Cyc_Tcpat_static_match(
_temp881, dsc); _LL885: if( _temp883 ==( void*) Cyc_Tcpat_Yes){ goto _LL886;}
else{ goto _LL887;} _LL887: if( _temp883 ==( void*) Cyc_Tcpat_No){ goto _LL888;}
else{ goto _LL889;} _LL889: if( _temp883 ==( void*) Cyc_Tcpat_Maybe){ goto
_LL890;} else{ goto _LL884;} _LL886: { struct Cyc_List_List* _temp891=({ struct
Cyc_List_List* _temp896=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp896->hd=( void*)({ struct _tuple11* _temp897=( struct _tuple11*) GC_malloc(
sizeof( struct _tuple11)); _temp897->f1= _temp881; _temp897->f2= 0; _temp897;});
_temp896->tl= ctx; _temp896;}); struct _tuple10* _temp892=({ struct _tuple10*
_temp895=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp895->f1=
_temp879; _temp895->f2= Cyc_Tcpat_getoargs( _temp881, obj); _temp895->f3= Cyc_Tcpat_getdargs(
_temp881, dsc); _temp895;}); struct Cyc_List_List* _temp893=({ struct Cyc_List_List*
_temp894=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp894->hd=( void*) _temp892; _temp894->tl= work; _temp894;}); return Cyc_Tcpat_and_match(
_temp891, _temp893, right_hand_side, rules);} _LL888: return Cyc_Tcpat_or_match(
Cyc_Tcpat_build_desc( ctx, dsc, work), rules); _LL890: { struct Cyc_List_List*
_temp898=({ struct Cyc_List_List* _temp907=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp907->hd=( void*)({ struct _tuple11*
_temp908=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp908->f1=
_temp881; _temp908->f2= 0; _temp908;}); _temp907->tl= ctx; _temp907;}); struct
_tuple10* _temp899=({ struct _tuple10* _temp906=( struct _tuple10*) GC_malloc(
sizeof( struct _tuple10)); _temp906->f1= _temp879; _temp906->f2= Cyc_Tcpat_getoargs(
_temp881, obj); _temp906->f3= Cyc_Tcpat_getdargs( _temp881, dsc); _temp906;});
struct Cyc_List_List* _temp900=({ struct Cyc_List_List* _temp905=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp905->hd=( void*) _temp899;
_temp905->tl= work; _temp905;}); void* _temp901= Cyc_Tcpat_and_match( _temp898,
_temp900, right_hand_side, rules); void* _temp902= Cyc_Tcpat_or_match( Cyc_Tcpat_build_desc(
ctx, Cyc_Tcpat_add_neg( dsc, _temp881), work), rules); return( void*)({ struct
Cyc_Tcpat_IfEq_struct* _temp903=( struct Cyc_Tcpat_IfEq_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_IfEq_struct)); _temp903[ 0]=({ struct Cyc_Tcpat_IfEq_struct
_temp904; _temp904.tag= Cyc_Tcpat_IfEq; _temp904.f1= obj; _temp904.f2= _temp881;
_temp904.f3=( void*) _temp901; _temp904.f4=( void*) _temp902; _temp904;});
_temp903;});} _LL884:;} _LL874:;} static void Cyc_Tcpat_check_exhaust_overlap(
void* d, void(* not_exhaust)( void*), void* env1, void(* rhs_appears)( void*,
void*), void* env2){ void* _temp909= d; void* _temp917; void* _temp919; void*
_temp921; _LL911: if( _temp909 ==( void*) Cyc_Tcpat_Failure){ goto _LL912;}
else{ goto _LL913;} _LL913: if(( unsigned int) _temp909 > 1u?*(( int*) _temp909)
== Cyc_Tcpat_Success: 0){ _LL918: _temp917=( void*)(( struct Cyc_Tcpat_Success_struct*)
_temp909)->f1; goto _LL914;} else{ goto _LL915;} _LL915: if(( unsigned int)
_temp909 > 1u?*(( int*) _temp909) == Cyc_Tcpat_IfEq: 0){ _LL922: _temp921=( void*)((
struct Cyc_Tcpat_IfEq_struct*) _temp909)->f3; goto _LL920; _LL920: _temp919=(
void*)(( struct Cyc_Tcpat_IfEq_struct*) _temp909)->f4; goto _LL916;} else{ goto
_LL910;} _LL912: not_exhaust( env1); goto _LL910; _LL914: rhs_appears( env2,
_temp917); goto _LL910; _LL916: Cyc_Tcpat_check_exhaust_overlap( _temp921,
not_exhaust, env1, rhs_appears, env2); Cyc_Tcpat_check_exhaust_overlap( _temp919,
not_exhaust, env1, rhs_appears, env2); goto _LL910; _LL910:;} struct _tuple12{
int f1; struct Cyc_Position_Segment* f2; } ; struct _tuple13{ void* f1; struct
_tuple12* f2; } ; static struct _tuple13* Cyc_Tcpat_get_match( int* r, struct
Cyc_Absyn_Switch_clause* swc){ void* sp0= Cyc_Tcpat_compile_pat( swc->pattern);
struct _tuple12* rhs=({ struct _tuple12* _temp928=( struct _tuple12*) GC_malloc(
sizeof( struct _tuple12)); _temp928->f1= 0; _temp928->f2=( swc->pattern)->loc;
_temp928;}); void* sp; if( swc->where_clause != 0){ sp= Cyc_Tcpat_tuple_pat(({
struct Cyc_List_List* _temp923=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp923->hd=( void*) sp0; _temp923->tl=({ struct Cyc_List_List*
_temp924=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp924->hd=( void*) Cyc_Tcpat_int_pat(* r); _temp924->tl= 0; _temp924;});
_temp923;}));* r=* r + 1;} else{ sp= Cyc_Tcpat_tuple_pat(({ struct Cyc_List_List*
_temp925=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp925->hd=( void*) sp0; _temp925->tl=({ struct Cyc_List_List* _temp926=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp926->hd=(
void*)(( void*) Cyc_Tcpat_Any); _temp926->tl= 0; _temp926;}); _temp925;}));}
return({ struct _tuple13* _temp927=( struct _tuple13*) GC_malloc( sizeof( struct
_tuple13)); _temp927->f1= sp; _temp927->f2= rhs; _temp927;});} static void Cyc_Tcpat_not_exhaust_err(
struct Cyc_Position_Segment* loc){ Cyc_Tcutil_terr( loc, _tag_arr("patterns may not be exhaustive",
sizeof( unsigned char), 31u));} static void Cyc_Tcpat_rule_occurs( int dummy,
struct _tuple12* rhs){(* rhs).f1= 1;} void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ int _temp929= 0;
struct Cyc_List_List* _temp930=(( struct Cyc_List_List*(*)( struct _tuple13*(* f)(
int*, struct Cyc_Absyn_Switch_clause*), int* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Tcpat_get_match,& _temp929, swcs); void* _temp931=(( void*(*)( struct Cyc_List_List*
allmrules)) Cyc_Tcpat_match_compile)( _temp930);(( void(*)( void* d, void(*
not_exhaust)( struct Cyc_Position_Segment*), struct Cyc_Position_Segment* env1,
void(* rhs_appears)( int, struct _tuple12*), int env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp931, Cyc_Tcpat_not_exhaust_err, loc, Cyc_Tcpat_rule_occurs, 0); for( 0;
_temp930 != 0; _temp930=(( struct Cyc_List_List*) _check_null( _temp930))->tl){
struct _tuple13 _temp934; struct _tuple12* _temp935; struct _tuple12 _temp937;
struct Cyc_Position_Segment* _temp938; int _temp940; struct _tuple13* _temp932=(
struct _tuple13*)(( struct Cyc_List_List*) _check_null( _temp930))->hd; _temp934=*
_temp932; _LL936: _temp935= _temp934.f2; _temp937=* _temp935; _LL941: _temp940=
_temp937.f1; goto _LL939; _LL939: _temp938= _temp937.f2; goto _LL933; _LL933:
if( ! _temp940){ Cyc_Tcutil_terr( _temp938, _tag_arr("redundant pattern",
sizeof( unsigned char), 18u));}}} struct _tuple14{ struct Cyc_Position_Segment*
f1; int f2; } ; static void Cyc_Tcpat_not_exhaust_warn( struct _tuple14* pr){(*
pr).f2= 0; Cyc_Tcutil_warn((* pr).f1, _tag_arr("pattern not exhaustive", sizeof(
unsigned char), 23u));} static void Cyc_Tcpat_dummy_fn( int i, int j){ return;}
struct _tuple15{ void* f1; int f2; } ; int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Pat* p){ struct Cyc_List_List*
_temp942=({ struct Cyc_List_List* _temp946=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp946->hd=( void*)({ struct _tuple15*
_temp947=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp947->f1=
Cyc_Tcpat_compile_pat( p); _temp947->f2= 0; _temp947;}); _temp946->tl= 0;
_temp946;}); void* _temp943=(( void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
_temp942); struct _tuple14* _temp944=({ struct _tuple14* _temp945=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp945->f1= loc; _temp945->f2=
1; _temp945;});(( void(*)( void* d, void(* not_exhaust)( struct _tuple14*),
struct _tuple14* env1, void(* rhs_appears)( int, int), int env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp943, Cyc_Tcpat_not_exhaust_warn, _temp944, Cyc_Tcpat_dummy_fn, 0); return(*
_temp944).f2;} static struct _tuple13* Cyc_Tcpat_get_match2( struct Cyc_Absyn_Switch_clause*
swc){ void* sp0= Cyc_Tcpat_compile_pat( swc->pattern); struct _tuple12* rhs=({
struct _tuple12* _temp949=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12));
_temp949->f1= 0; _temp949->f2=( swc->pattern)->loc; _temp949;}); return({ struct
_tuple13* _temp948=( struct _tuple13*) GC_malloc( sizeof( struct _tuple13));
_temp948->f1= sp0; _temp948->f2= rhs; _temp948;});} static void Cyc_Tcpat_not_exhaust_err2(
struct Cyc_Position_Segment* loc){;} void Cyc_Tcpat_check_catch_overlap( struct
Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ struct Cyc_List_List*
_temp950=(( struct Cyc_List_List*(*)( struct _tuple13*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_get_match2, swcs); void*
_temp951=(( void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
_temp950);(( void(*)( void* d, void(* not_exhaust)( struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env1, void(* rhs_appears)( int, struct _tuple12*),
int env2)) Cyc_Tcpat_check_exhaust_overlap)( _temp951, Cyc_Tcpat_not_exhaust_err2,
loc, Cyc_Tcpat_rule_occurs, 0); for( 0; _temp950 != 0; _temp950=(( struct Cyc_List_List*)
_check_null( _temp950))->tl){ struct _tuple13 _temp954; struct _tuple12*
_temp955; struct _tuple12 _temp957; struct Cyc_Position_Segment* _temp958; int
_temp960; struct _tuple13* _temp952=( struct _tuple13*)(( struct Cyc_List_List*)
_check_null( _temp950))->hd; _temp954=* _temp952; _LL956: _temp955= _temp954.f2;
_temp957=* _temp955; _LL961: _temp960= _temp957.f1; goto _LL959; _LL959:
_temp958= _temp957.f2; goto _LL953; _LL953: if( ! _temp960){ Cyc_Tcutil_terr(
_temp958, _tag_arr("redundant pattern", sizeof( unsigned char), 18u));}}}