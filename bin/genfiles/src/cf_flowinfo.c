 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_dirent{ int d_ino; int d_off; unsigned short
d_reclen; unsigned char d_type; unsigned char d_name[ 256u]; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern int
Cyc_Core_intcmp( int, int); extern int Cyc_Core_ptrcmp( void**, void**); extern
unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
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
Cyc_List_List* tl; } ; extern unsigned char Cyc_List_List_empty[ 15u]; extern
struct Cyc_List_List* Cyc_List_tl( struct Cyc_List_List* x); extern
unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char Cyc_List_Nth[ 8u];
extern int Cyc_List_mem( int(* compare)( void*, void*), struct Cyc_List_List* l,
void* x); extern int Cyc_List_list_cmp( int(* cmp)( void*, void*), struct Cyc_List_List*
l1, struct Cyc_List_List* l2); struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[
11u]; struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u]; extern
unsigned char Cyc_Dict_Absent[ 11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty(
int(* comp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern void* Cyc_Dict_lookup(
struct Cyc_Dict_Dict* d, void* key); extern void Cyc_Dict_iter_c( void(* f)(
void*, void*, void*), void* env, struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict*
Cyc_Dict_map_c( void*(* f)( void*, void*), void* env, struct Cyc_Dict_Dict* d);
extern struct Cyc_Dict_Dict* Cyc_Dict_intersect_c( void*(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); extern
int Cyc_Dict_forall_c( int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict*
d); extern int Cyc_Dict_forall_intersect( int(* f)( void*, void*, void*), struct
Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); struct Cyc_Stdio___sFILE; extern
unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; static const int Cyc_Stdio_String_pa= 0; struct Cyc_Stdio_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_Int_pa= 1; struct
Cyc_Stdio_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Stdio_Double_pa=
2; struct Cyc_Stdio_Double_pa_struct{ int tag; double f1; } ; static const int
Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{ int tag; short*
f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
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
struct _tagged_arr s2); extern int Cyc_String_zstrptrcmp( struct _tagged_arr*,
struct _tagged_arr*); struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file;
struct _tagged_arr line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static
const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct
Cyc_Position_Error{ struct _tagged_arr source; struct Cyc_Position_Segment* seg;
void* kind; struct _tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[
14u]; struct _tuple0{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar;
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_compress(
void* t); struct Cyc_CfFlowInfo_Place; static const int Cyc_CfFlowInfo_VarRoot=
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
int tag; struct Cyc_Dict_Dict* f1; } ; extern void* Cyc_CfFlowInfo_mkLeafPI(
void* esc, void* il); extern int Cyc_CfFlowInfo_local_root_cmp( void*, void*);
extern int Cyc_CfFlowInfo_field_cmp( void*, void*); extern int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*); extern void* Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(
void* t); extern void* Cyc_CfFlowInfo_lookup_place( struct Cyc_Dict_Dict* d,
struct Cyc_CfFlowInfo_Place* place); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_insert_place(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place, void* pinfo);
extern int Cyc_CfFlowInfo_isAllInit( struct Cyc_Dict_Dict* pinfo_dict, void*
pinfo); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_pointsto( void* pinfo,
struct Cyc_Dict_Dict* d); extern void* Cyc_CfFlowInfo_assign_unknown_dict( void*,
void*); extern void* Cyc_CfFlowInfo_join_flow( void*, void*); extern int Cyc_CfFlowInfo_flow_lessthan_approx(
void* f1, void* f2); static struct Cyc_CfFlowInfo_UnknownIS_struct Cyc_CfFlowInfo_unknownis_unes_noneil={
0u,( void*)(( void*) 1u),( void*)(( void*) 0u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct
Cyc_CfFlowInfo_unknownis_unes_thisil={ 0u,( void*)(( void*) 1u),( void*)(( void*)
1u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct Cyc_CfFlowInfo_unknownis_unes_allil={
0u,( void*)(( void*) 1u),( void*)(( void*) 2u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct
Cyc_CfFlowInfo_unknownis_esc_noneil={ 0u,( void*)(( void*) 0u),( void*)(( void*)
0u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct Cyc_CfFlowInfo_unknownis_esc_thisil={
0u,( void*)(( void*) 0u),( void*)(( void*) 1u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct
Cyc_CfFlowInfo_unknownis_esc_allil={ 0u,( void*)(( void*) 0u),( void*)(( void*)
2u)}; static struct Cyc_CfFlowInfo_LeafPI_struct Cyc_CfFlowInfo_leaf_unesc_none_v={
0u,( void*)(( void*)& Cyc_CfFlowInfo_unknownis_unes_noneil)}; static struct Cyc_CfFlowInfo_LeafPI_struct
Cyc_CfFlowInfo_leaf_unesc_this_v={ 0u,( void*)(( void*)& Cyc_CfFlowInfo_unknownis_unes_thisil)};
static struct Cyc_CfFlowInfo_LeafPI_struct Cyc_CfFlowInfo_leaf_unesc_all_v={ 0u,(
void*)(( void*)& Cyc_CfFlowInfo_unknownis_unes_allil)}; static struct Cyc_CfFlowInfo_LeafPI_struct
Cyc_CfFlowInfo_leaf_esc_none_v={ 0u,( void*)(( void*)& Cyc_CfFlowInfo_unknownis_esc_noneil)};
static struct Cyc_CfFlowInfo_LeafPI_struct Cyc_CfFlowInfo_leaf_esc_this_v={ 0u,(
void*)(( void*)& Cyc_CfFlowInfo_unknownis_esc_thisil)}; static struct Cyc_CfFlowInfo_LeafPI_struct
Cyc_CfFlowInfo_leaf_esc_all_v={ 0u,( void*)(( void*)& Cyc_CfFlowInfo_unknownis_esc_allil)};
void* Cyc_CfFlowInfo_leaf_unesc_none=( void*)& Cyc_CfFlowInfo_leaf_unesc_none_v;
void* Cyc_CfFlowInfo_leaf_unesc_this=( void*)& Cyc_CfFlowInfo_leaf_unesc_this_v;
void* Cyc_CfFlowInfo_leaf_unesc_all=( void*)& Cyc_CfFlowInfo_leaf_unesc_all_v;
void* Cyc_CfFlowInfo_leaf_esc_none=( void*)& Cyc_CfFlowInfo_leaf_esc_none_v;
void* Cyc_CfFlowInfo_leaf_esc_this=( void*)& Cyc_CfFlowInfo_leaf_esc_this_v;
void* Cyc_CfFlowInfo_leaf_esc_all=( void*)& Cyc_CfFlowInfo_leaf_esc_all_v;
struct _tuple3{ void* f1; void* f2; } ; void* Cyc_CfFlowInfo_mkLeafPI( void* esc,
void* il){ struct _tuple3 _temp13=({ struct _tuple3 _temp12; _temp12.f1= esc;
_temp12.f2= il; _temp12;}); void* _temp27; void* _temp29; void* _temp31; void*
_temp33; void* _temp35; void* _temp37; void* _temp39; void* _temp41; void*
_temp43; void* _temp45; void* _temp47; void* _temp49; _LL15: _LL30: _temp29=
_temp13.f1; if( _temp29 ==( void*) Cyc_CfFlowInfo_Unesc){ goto _LL28;} else{
goto _LL17;} _LL28: _temp27= _temp13.f2; if( _temp27 ==( void*) Cyc_CfFlowInfo_NoneIL){
goto _LL16;} else{ goto _LL17;} _LL17: _LL34: _temp33= _temp13.f1; if( _temp33
==( void*) Cyc_CfFlowInfo_Unesc){ goto _LL32;} else{ goto _LL19;} _LL32: _temp31=
_temp13.f2; if( _temp31 ==( void*) Cyc_CfFlowInfo_ThisIL){ goto _LL18;} else{
goto _LL19;} _LL19: _LL38: _temp37= _temp13.f1; if( _temp37 ==( void*) Cyc_CfFlowInfo_Unesc){
goto _LL36;} else{ goto _LL21;} _LL36: _temp35= _temp13.f2; if( _temp35 ==( void*)
Cyc_CfFlowInfo_AllIL){ goto _LL20;} else{ goto _LL21;} _LL21: _LL42: _temp41=
_temp13.f1; if( _temp41 ==( void*) Cyc_CfFlowInfo_Esc){ goto _LL40;} else{ goto
_LL23;} _LL40: _temp39= _temp13.f2; if( _temp39 ==( void*) Cyc_CfFlowInfo_NoneIL){
goto _LL22;} else{ goto _LL23;} _LL23: _LL46: _temp45= _temp13.f1; if( _temp45
==( void*) Cyc_CfFlowInfo_Esc){ goto _LL44;} else{ goto _LL25;} _LL44: _temp43=
_temp13.f2; if( _temp43 ==( void*) Cyc_CfFlowInfo_ThisIL){ goto _LL24;} else{
goto _LL25;} _LL25: _LL50: _temp49= _temp13.f1; if( _temp49 ==( void*) Cyc_CfFlowInfo_Esc){
goto _LL48;} else{ goto _LL14;} _LL48: _temp47= _temp13.f2; if( _temp47 ==( void*)
Cyc_CfFlowInfo_AllIL){ goto _LL26;} else{ goto _LL14;} _LL16: return Cyc_CfFlowInfo_leaf_unesc_none;
_LL18: return Cyc_CfFlowInfo_leaf_unesc_this; _LL20: return Cyc_CfFlowInfo_leaf_unesc_all;
_LL22: return Cyc_CfFlowInfo_leaf_esc_none; _LL24: return Cyc_CfFlowInfo_leaf_esc_this;
_LL26: return Cyc_CfFlowInfo_leaf_esc_all; _LL14:;} int Cyc_CfFlowInfo_local_root_cmp(
void* r1, void* r2){ if( r1 == r2){ return 0;}{ struct _tuple3 _temp52=({ struct
_tuple3 _temp51; _temp51.f1= r1; _temp51.f2= r2; _temp51;}); void* _temp62; void*
_temp64; void* _temp66; void* _temp68; void* _temp70; struct Cyc_Absyn_Vardecl*
_temp72; void* _temp74; struct Cyc_Absyn_Vardecl* _temp76; void* _temp78; struct
Cyc_Absyn_Exp* _temp80; void* _temp82; struct Cyc_Absyn_Exp* _temp84; _LL54:
_LL65: _temp64= _temp52.f1; if(*(( int*) _temp64) == Cyc_CfFlowInfo_VarRoot){
goto _LL63;} else{ goto _LL56;} _LL63: _temp62= _temp52.f2; if(*(( int*) _temp62)
== Cyc_CfFlowInfo_MallocPt){ goto _LL55;} else{ goto _LL56;} _LL56: _LL69:
_temp68= _temp52.f1; if(*(( int*) _temp68) == Cyc_CfFlowInfo_MallocPt){ goto
_LL67;} else{ goto _LL58;} _LL67: _temp66= _temp52.f2; if(*(( int*) _temp66) ==
Cyc_CfFlowInfo_VarRoot){ goto _LL57;} else{ goto _LL58;} _LL58: _LL75: _temp74=
_temp52.f1; if(*(( int*) _temp74) == Cyc_CfFlowInfo_VarRoot){ _LL77: _temp76=((
struct Cyc_CfFlowInfo_VarRoot_struct*) _temp74)->f1; goto _LL71;} else{ goto
_LL60;} _LL71: _temp70= _temp52.f2; if(*(( int*) _temp70) == Cyc_CfFlowInfo_VarRoot){
_LL73: _temp72=(( struct Cyc_CfFlowInfo_VarRoot_struct*) _temp70)->f1; goto
_LL59;} else{ goto _LL60;} _LL60: _LL83: _temp82= _temp52.f1; if(*(( int*)
_temp82) == Cyc_CfFlowInfo_MallocPt){ _LL85: _temp84=(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp82)->f1; goto _LL79;} else{ goto _LL53;} _LL79: _temp78= _temp52.f2; if(*((
int*) _temp78) == Cyc_CfFlowInfo_MallocPt){ _LL81: _temp80=(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp78)->f1; goto _LL61;} else{ goto _LL53;} _LL55: return 1; _LL57: return - 1;
_LL59: return(( int(*)( struct Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*))
Cyc_Core_ptrcmp)( _temp76, _temp72); _LL61: return(( int(*)( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*)) Cyc_Core_ptrcmp)( _temp84, _temp80); _LL53:;}} int Cyc_CfFlowInfo_field_cmp(
void* f1, void* f2){ if( f1 == f2){ return 0;}{ struct _tuple3 _temp87=({ struct
_tuple3 _temp86; _temp86.f1= f1; _temp86.f2= f2; _temp86;}); void* _temp97; void*
_temp99; void* _temp101; void* _temp103; void* _temp105; struct _tagged_arr*
_temp107; void* _temp109; struct _tagged_arr* _temp111; void* _temp113; int
_temp115; void* _temp117; int _temp119; _LL89: _LL100: _temp99= _temp87.f1; if(*((
int*) _temp99) == Cyc_CfFlowInfo_StructF){ goto _LL98;} else{ goto _LL91;} _LL98:
_temp97= _temp87.f2; if(*(( int*) _temp97) == Cyc_CfFlowInfo_TupleF){ goto _LL90;}
else{ goto _LL91;} _LL91: _LL104: _temp103= _temp87.f1; if(*(( int*) _temp103)
== Cyc_CfFlowInfo_TupleF){ goto _LL102;} else{ goto _LL93;} _LL102: _temp101=
_temp87.f2; if(*(( int*) _temp101) == Cyc_CfFlowInfo_StructF){ goto _LL92;}
else{ goto _LL93;} _LL93: _LL110: _temp109= _temp87.f1; if(*(( int*) _temp109)
== Cyc_CfFlowInfo_StructF){ _LL112: _temp111=(( struct Cyc_CfFlowInfo_StructF_struct*)
_temp109)->f1; goto _LL106;} else{ goto _LL95;} _LL106: _temp105= _temp87.f2;
if(*(( int*) _temp105) == Cyc_CfFlowInfo_StructF){ _LL108: _temp107=(( struct
Cyc_CfFlowInfo_StructF_struct*) _temp105)->f1; goto _LL94;} else{ goto _LL95;}
_LL95: _LL118: _temp117= _temp87.f1; if(*(( int*) _temp117) == Cyc_CfFlowInfo_TupleF){
_LL120: _temp119=(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp117)->f1; goto
_LL114;} else{ goto _LL88;} _LL114: _temp113= _temp87.f2; if(*(( int*) _temp113)
== Cyc_CfFlowInfo_TupleF){ _LL116: _temp115=(( struct Cyc_CfFlowInfo_TupleF_struct*)
_temp113)->f1; goto _LL96;} else{ goto _LL88;} _LL90: return 1; _LL92: return -
1; _LL94: return(( int(*)( struct _tagged_arr*, struct _tagged_arr*)) Cyc_Core_ptrcmp)(
_temp111, _temp107); _LL96: return _temp119 - _temp115; _LL88:;}} int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place* p1, struct Cyc_CfFlowInfo_Place* p2){ if( p1 == p2){
return 0;}{ int i= Cyc_CfFlowInfo_local_root_cmp(( void*) p1->root,( void*) p2->root);
if( i != 0){ return i;} return Cyc_List_list_cmp( Cyc_CfFlowInfo_field_cmp, p1->fields,
p2->fields);}} struct _tuple4{ struct Cyc_Absyn_Tqual f1; void* f2; } ; void*
Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( void* t){ void* _temp121= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_TunionFieldInfo _temp133; void* _temp135; struct Cyc_Absyn_Tunionfield*
_temp137; struct Cyc_List_List* _temp139; struct Cyc_List_List* _temp141; struct
Cyc_Absyn_Structdecl** _temp143; struct Cyc_Absyn_Structdecl* _temp145; struct
Cyc_Absyn_Structdecl _temp146; struct Cyc_Core_Opt* _temp147; struct Cyc_Core_Opt
_temp149; struct Cyc_List_List* _temp150; _LL123: if(( unsigned int) _temp121 >
4u?*(( int*) _temp121) == Cyc_Absyn_TunionFieldType: 0){ _LL134: _temp133=((
struct Cyc_Absyn_TunionFieldType_struct*) _temp121)->f1; _LL136: _temp135=( void*)
_temp133.field_info; if(*(( int*) _temp135) == Cyc_Absyn_KnownTunionfield){
_LL138: _temp137=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp135)->f2;
goto _LL124;} else{ goto _LL125;}} else{ goto _LL125;} _LL125: if(( unsigned int)
_temp121 > 4u?*(( int*) _temp121) == Cyc_Absyn_TupleType: 0){ _LL140: _temp139=((
struct Cyc_Absyn_TupleType_struct*) _temp121)->f1; goto _LL126;} else{ goto
_LL127;} _LL127: if(( unsigned int) _temp121 > 4u?*(( int*) _temp121) == Cyc_Absyn_AnonStructType:
0){ _LL142: _temp141=(( struct Cyc_Absyn_AnonStructType_struct*) _temp121)->f1;
goto _LL128;} else{ goto _LL129;} _LL129: if(( unsigned int) _temp121 > 4u?*((
int*) _temp121) == Cyc_Absyn_StructType: 0){ _LL144: _temp143=(( struct Cyc_Absyn_StructType_struct*)
_temp121)->f3; if( _temp143 == 0){ goto _LL131;} else{ _temp145=* _temp143;
_temp146=* _temp145; _LL148: _temp147= _temp146.fields; if( _temp147 == 0){ goto
_LL131;} else{ _temp149=* _temp147; _LL151: _temp150=( struct Cyc_List_List*)
_temp149.v; goto _LL130;}}} else{ goto _LL131;} _LL131: goto _LL132; _LL124: if(
_temp137->typs == 0){ return Cyc_CfFlowInfo_leaf_unesc_none;} _temp139= _temp137->typs;
goto _LL126; _LL126: { struct Cyc_Dict_Dict* _temp152=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( int, int))) Cyc_Dict_empty)( Cyc_Core_intcmp);{ int i= 0; for( 0;
_temp139 != 0;( _temp139=(( struct Cyc_List_List*) _check_null( _temp139))->tl,
++ i)){ struct _tuple4 _temp155; void* _temp156; struct _tuple4* _temp153=(
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp139))->hd; _temp155=*
_temp153; _LL157: _temp156= _temp155.f2; goto _LL154; _LL154: _temp152=(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, int key, void* data)) Cyc_Dict_insert)(
_temp152, i, Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( _temp156));}} return( void*)({
struct Cyc_CfFlowInfo_TuplePI_struct* _temp158=( struct Cyc_CfFlowInfo_TuplePI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct)); _temp158[ 0]=({
struct Cyc_CfFlowInfo_TuplePI_struct _temp159; _temp159.tag= Cyc_CfFlowInfo_TuplePI;
_temp159.f1= _temp152; _temp159;}); _temp158;});} _LL128: _temp150= _temp141;
goto _LL130; _LL130: { struct Cyc_Dict_Dict* d=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
for( 0; _temp150 != 0; _temp150=(( struct Cyc_List_List*) _check_null( _temp150))->tl){
struct Cyc_Absyn_Structfield _temp162; void* _temp163; struct _tagged_arr*
_temp165; struct Cyc_Absyn_Structfield* _temp160=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp150))->hd; _temp162=* _temp160; _LL166:
_temp165= _temp162.name; goto _LL164; _LL164: _temp163=( void*) _temp162.type;
goto _LL161; _LL161: if( Cyc_String_strcmp(* _temp165, _tag_arr("", sizeof(
unsigned char), 1u)) != 0){ d=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, void* data)) Cyc_Dict_insert)( d, _temp165, Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(
_temp163));}} return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct* _temp167=(
struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp167[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp168; _temp168.tag=
Cyc_CfFlowInfo_StructPI; _temp168.f1= d; _temp168;}); _temp167;});} _LL132:
return Cyc_CfFlowInfo_leaf_unesc_none; _LL122:;} void* Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place){ struct Cyc_CfFlowInfo_Place
_temp171; struct Cyc_List_List* _temp172; void* _temp174; struct Cyc_CfFlowInfo_Place*
_temp169= place; _temp171=* _temp169; _LL175: _temp174=( void*) _temp171.root;
goto _LL173; _LL173: _temp172= _temp171.fields; goto _LL170; _LL170: { void*
_temp176= Cyc_Dict_lookup( d, _temp174); for( 0; _temp172 != 0; _temp172=((
struct Cyc_List_List*) _check_null( _temp172))->tl){ struct _tuple3 _temp178=({
struct _tuple3 _temp177; _temp177.f1= _temp176; _temp177.f2=( void*)(( struct
Cyc_List_List*) _check_null( _temp172))->hd; _temp177;}); void* _temp186; int
_temp188; void* _temp190; struct Cyc_Dict_Dict* _temp192; void* _temp194; struct
_tagged_arr* _temp196; void* _temp198; struct Cyc_Dict_Dict* _temp200; _LL180:
_LL191: _temp190= _temp178.f1; if(*(( int*) _temp190) == Cyc_CfFlowInfo_TuplePI){
_LL193: _temp192=(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp190)->f1; goto
_LL187;} else{ goto _LL182;} _LL187: _temp186= _temp178.f2; if(*(( int*)
_temp186) == Cyc_CfFlowInfo_TupleF){ _LL189: _temp188=(( struct Cyc_CfFlowInfo_TupleF_struct*)
_temp186)->f1; goto _LL181;} else{ goto _LL182;} _LL182: _LL199: _temp198=
_temp178.f1; if(*(( int*) _temp198) == Cyc_CfFlowInfo_StructPI){ _LL201:
_temp200=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp198)->f1; goto _LL195;}
else{ goto _LL184;} _LL195: _temp194= _temp178.f2; if(*(( int*) _temp194) == Cyc_CfFlowInfo_StructF){
_LL197: _temp196=(( struct Cyc_CfFlowInfo_StructF_struct*) _temp194)->f1; goto
_LL183;} else{ goto _LL184;} _LL184: goto _LL185; _LL181: _temp176=(( void*(*)(
struct Cyc_Dict_Dict* d, int key)) Cyc_Dict_lookup)( _temp192, _temp188); goto
_LL179; _LL183: _temp176=(( void*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup)( _temp200, _temp196); goto _LL179; _LL185:(
int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp202=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp202[ 0]=({ struct
Cyc_Core_Impossible_struct _temp203; _temp203.tag= Cyc_Core_Impossible; _temp203.f1=
_tag_arr("bad lookup_place", sizeof( unsigned char), 17u); _temp203;}); _temp202;}));
_LL179:;} return _temp176;}} struct _tuple5{ void* f1; struct Cyc_List_List* f2;
} ; void* Cyc_CfFlowInfo_insert_place_rec( void* insert_path, struct Cyc_List_List*
fields, void* pinfo){ if( fields == 0){ return pinfo;}{ struct _tuple5 _temp205=({
struct _tuple5 _temp204; _temp204.f1= insert_path; _temp204.f2= fields; _temp204;});
struct Cyc_List_List* _temp213; struct Cyc_List_List _temp215; struct Cyc_List_List*
_temp216; void* _temp218; int _temp220; void* _temp222; struct Cyc_Dict_Dict*
_temp224; struct Cyc_List_List* _temp226; struct Cyc_List_List _temp228; struct
Cyc_List_List* _temp229; void* _temp231; struct _tagged_arr* _temp233; void*
_temp235; struct Cyc_Dict_Dict* _temp237; _LL207: _LL223: _temp222= _temp205.f1;
if(*(( int*) _temp222) == Cyc_CfFlowInfo_TuplePI){ _LL225: _temp224=(( struct
Cyc_CfFlowInfo_TuplePI_struct*) _temp222)->f1; goto _LL214;} else{ goto _LL209;}
_LL214: _temp213= _temp205.f2; if( _temp213 == 0){ goto _LL209;} else{ _temp215=*
_temp213; _LL219: _temp218=( void*) _temp215.hd; if(*(( int*) _temp218) == Cyc_CfFlowInfo_TupleF){
_LL221: _temp220=(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp218)->f1; goto
_LL217;} else{ goto _LL209;} _LL217: _temp216= _temp215.tl; goto _LL208;} _LL209:
_LL236: _temp235= _temp205.f1; if(*(( int*) _temp235) == Cyc_CfFlowInfo_StructPI){
_LL238: _temp237=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp235)->f1; goto
_LL227;} else{ goto _LL211;} _LL227: _temp226= _temp205.f2; if( _temp226 == 0){
goto _LL211;} else{ _temp228=* _temp226; _LL232: _temp231=( void*) _temp228.hd;
if(*(( int*) _temp231) == Cyc_CfFlowInfo_StructF){ _LL234: _temp233=(( struct
Cyc_CfFlowInfo_StructF_struct*) _temp231)->f1; goto _LL230;} else{ goto _LL211;}
_LL230: _temp229= _temp228.tl; goto _LL210;} _LL211: goto _LL212; _LL208: { void*
_temp239=(( void*(*)( struct Cyc_Dict_Dict* d, int key)) Cyc_Dict_lookup)(
_temp224, _temp220); return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct*
_temp240=( struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp240[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp241; _temp241.tag= Cyc_CfFlowInfo_TuplePI;
_temp241.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, int key, void*
data)) Cyc_Dict_insert)( _temp224, _temp220, Cyc_CfFlowInfo_insert_place_rec(
_temp239, _temp216, pinfo)); _temp241;}); _temp240;});} _LL210: { void* _temp242=((
void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
_temp237, _temp233); return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp243=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp243[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp244; _temp244.tag=
Cyc_CfFlowInfo_StructPI; _temp244.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, void* data)) Cyc_Dict_insert)( _temp237, _temp233,
Cyc_CfFlowInfo_insert_place_rec( _temp242, _temp229, pinfo)); _temp244;});
_temp243;});} _LL212:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp245=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp245[ 0]=({ struct Cyc_Core_Impossible_struct _temp246; _temp246.tag= Cyc_Core_Impossible;
_temp246.f1= _tag_arr("bad insert_place", sizeof( unsigned char), 17u); _temp246;});
_temp245;})); _LL206:;}} struct Cyc_Dict_Dict* Cyc_CfFlowInfo_insert_place(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place, void* pinfo){
struct Cyc_CfFlowInfo_Place _temp249; struct Cyc_List_List* _temp250; void*
_temp252; struct Cyc_CfFlowInfo_Place* _temp247= place; _temp249=* _temp247;
_LL253: _temp252=( void*) _temp249.root; goto _LL251; _LL251: _temp250= _temp249.fields;
goto _LL248; _LL248: return Cyc_Dict_insert( d, _temp252, Cyc_CfFlowInfo_insert_place_rec(
Cyc_Dict_lookup( d, _temp252), _temp250, pinfo));} struct _tuple6{ struct Cyc_Dict_Dict*
f1; struct Cyc_List_List* f2; } ; int Cyc_CfFlowInfo_isAllInit_pinfo( struct
_tuple6* env, void* path); int Cyc_CfFlowInfo_isAllInit_is( struct _tuple6* env,
void* state){ void* _temp254= state; void* _temp262; struct Cyc_CfFlowInfo_Place*
_temp264; _LL256: if(*(( int*) _temp254) == Cyc_CfFlowInfo_UnknownIS){ _LL263:
_temp262=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp254)->f2; if(
_temp262 ==( void*) Cyc_CfFlowInfo_AllIL){ goto _LL257;} else{ goto _LL258;}}
else{ goto _LL258;} _LL258: if(*(( int*) _temp254) == Cyc_CfFlowInfo_UnknownIS){
goto _LL259;} else{ goto _LL260;} _LL260: if(*(( int*) _temp254) == Cyc_CfFlowInfo_MustPointTo){
_LL265: _temp264=(( struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp254)->f1;
goto _LL261;} else{ goto _LL255;} _LL257: return 1; _LL259: return 0; _LL261: {
struct _tuple6 _temp268; struct Cyc_List_List* _temp269; struct Cyc_List_List**
_temp271; struct Cyc_Dict_Dict* _temp272; struct _tuple6* _temp266= env;
_temp268=* _temp266; _LL273: _temp272= _temp268.f1; goto _LL270; _LL270:
_temp269= _temp268.f2; _temp271=&(* _temp266).f2; goto _LL267; _LL267: if((( int(*)(
int(* compare)( struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List* l, struct Cyc_CfFlowInfo_Place* x)) Cyc_List_mem)( Cyc_CfFlowInfo_place_cmp,*
_temp271, _temp264)){ return 1;}* _temp271=({ struct Cyc_List_List* _temp274=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp274->hd=(
void*) _temp264; _temp274->tl=* _temp271; _temp274;}); return Cyc_CfFlowInfo_isAllInit_pinfo(
env, Cyc_CfFlowInfo_lookup_place( _temp272, _temp264));} _LL255:;} int Cyc_CfFlowInfo_isAllInit_f(
struct _tuple6* env, void* a, void* path){ return Cyc_CfFlowInfo_isAllInit_pinfo(
env, path);} int Cyc_CfFlowInfo_isAllInit_pinfo( struct _tuple6* env, void* path){
void* _temp275= path; void* _temp283; struct Cyc_Dict_Dict* _temp285; struct Cyc_Dict_Dict*
_temp287; _LL277: if(*(( int*) _temp275) == Cyc_CfFlowInfo_LeafPI){ _LL284:
_temp283=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp275)->f1; goto
_LL278;} else{ goto _LL279;} _LL279: if(*(( int*) _temp275) == Cyc_CfFlowInfo_TuplePI){
_LL286: _temp285=(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp275)->f1; goto
_LL280;} else{ goto _LL281;} _LL281: if(*(( int*) _temp275) == Cyc_CfFlowInfo_StructPI){
_LL288: _temp287=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp275)->f1; goto
_LL282;} else{ goto _LL276;} _LL278: return Cyc_CfFlowInfo_isAllInit_is( env,
_temp283); _LL280: return(( int(*)( int(* f)( struct _tuple6*, int, void*),
struct _tuple6* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_forall_c)(( int(*)(
struct _tuple6* env, int a, void* path)) Cyc_CfFlowInfo_isAllInit_f, env,
_temp285); _LL282: return(( int(*)( int(* f)( struct _tuple6*, struct
_tagged_arr*, void*), struct _tuple6* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_forall_c)((
int(*)( struct _tuple6* env, struct _tagged_arr* a, void* path)) Cyc_CfFlowInfo_isAllInit_f,
env, _temp287); _LL276:;} int Cyc_CfFlowInfo_isAllInit( struct Cyc_Dict_Dict*
pinfo_dict, void* pinfo){ struct _tuple6 _temp289=({ struct _tuple6 _temp290;
_temp290.f1= pinfo_dict; _temp290.f2= 0; _temp290;}); return Cyc_CfFlowInfo_isAllInit_pinfo(&
_temp289, pinfo);} static void Cyc_CfFlowInfo_add_place( struct _RegionHandle* r,
struct Cyc_List_List** places, struct Cyc_CfFlowInfo_Place* place){ if( !(( int(*)(
int(* compare)( struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List* l, struct Cyc_CfFlowInfo_Place* x)) Cyc_List_mem)( Cyc_CfFlowInfo_place_cmp,*
places, place)){* places=({ struct Cyc_List_List* _temp291=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp291->hd=( void*) place;
_temp291->tl=* places; _temp291;});}} struct _tuple7{ struct Cyc_Dict_Dict* f1;
struct Cyc_List_List** f2; struct _RegionHandle* f3; } ; void* Cyc_CfFlowInfo_pinfo_getplaces_esc(
struct _tuple7* env, void* pinfo){ struct _tuple7 _temp294; struct _RegionHandle*
_temp295; struct Cyc_List_List** _temp297; struct Cyc_Dict_Dict* _temp299;
struct _tuple7* _temp292= env; _temp294=* _temp292; _LL300: _temp299= _temp294.f1;
goto _LL298; _LL298: _temp297= _temp294.f2; goto _LL296; _LL296: _temp295=
_temp294.f3; goto _LL293; _LL293: { void* _temp301= pinfo; void* _temp311;
struct Cyc_CfFlowInfo_Place* _temp313; void* _temp315; void* _temp317; struct
Cyc_Dict_Dict* _temp319; struct Cyc_Dict_Dict* _temp321; _LL303: if(*(( int*)
_temp301) == Cyc_CfFlowInfo_LeafPI){ _LL312: _temp311=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp301)->f1; if(*(( int*) _temp311) == Cyc_CfFlowInfo_MustPointTo){ _LL314:
_temp313=(( struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp311)->f1; goto
_LL304;} else{ goto _LL305;}} else{ goto _LL305;} _LL305: if(*(( int*) _temp301)
== Cyc_CfFlowInfo_LeafPI){ _LL316: _temp315=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp301)->f1; if(*(( int*) _temp315) == Cyc_CfFlowInfo_UnknownIS){ _LL318:
_temp317=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp315)->f2; goto
_LL306;} else{ goto _LL307;}} else{ goto _LL307;} _LL307: if(*(( int*) _temp301)
== Cyc_CfFlowInfo_TuplePI){ _LL320: _temp319=(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp301)->f1; goto _LL308;} else{ goto _LL309;} _LL309: if(*(( int*) _temp301)
== Cyc_CfFlowInfo_StructPI){ _LL322: _temp321=(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp301)->f1; goto _LL310;} else{ goto _LL302;} _LL304: Cyc_CfFlowInfo_add_place(
_temp295, _temp297, _temp313); if( Cyc_CfFlowInfo_isAllInit( _temp299, Cyc_CfFlowInfo_lookup_place(
_temp299, _temp313))){ return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,(
void*) Cyc_CfFlowInfo_AllIL);} return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,(
void*) Cyc_CfFlowInfo_ThisIL); _LL306: return Cyc_CfFlowInfo_mkLeafPI(( void*)
Cyc_CfFlowInfo_Esc, _temp317); _LL308: return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct*
_temp323=( struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp323[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp324; _temp324.tag= Cyc_CfFlowInfo_TuplePI;
_temp324.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct _tuple7*, void*),
struct _tuple7* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_pinfo_getplaces_esc,
env, _temp319); _temp324;}); _temp323;}); _LL310: return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp325=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp325[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp326; _temp326.tag=
Cyc_CfFlowInfo_StructPI; _temp326.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)(
struct _tuple7*, void*), struct _tuple7* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)(
Cyc_CfFlowInfo_pinfo_getplaces_esc, env, _temp321); _temp326;}); _temp325;});
_LL302:;}} struct _tuple8{ struct Cyc_List_List** f1; struct _RegionHandle* f2;
} ; void Cyc_CfFlowInfo_pinfo_getplaces( struct _tuple8* env, void* a, void*
pinfo){ struct _tuple8 _temp329; struct _RegionHandle* _temp330; struct Cyc_List_List**
_temp332; struct _tuple8* _temp327= env; _temp329=* _temp327; _LL333: _temp332=
_temp329.f1; goto _LL331; _LL331: _temp330= _temp329.f2; goto _LL328; _LL328: {
void* _temp334= pinfo; void* _temp344; void* _temp346; struct Cyc_CfFlowInfo_Place*
_temp348; struct Cyc_Dict_Dict* _temp350; struct Cyc_Dict_Dict* _temp352; _LL336:
if(*(( int*) _temp334) == Cyc_CfFlowInfo_LeafPI){ _LL345: _temp344=( void*)((
struct Cyc_CfFlowInfo_LeafPI_struct*) _temp334)->f1; if(*(( int*) _temp344) ==
Cyc_CfFlowInfo_UnknownIS){ goto _LL337;} else{ goto _LL338;}} else{ goto _LL338;}
_LL338: if(*(( int*) _temp334) == Cyc_CfFlowInfo_LeafPI){ _LL347: _temp346=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp334)->f1; if(*(( int*)
_temp346) == Cyc_CfFlowInfo_MustPointTo){ _LL349: _temp348=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp346)->f1; goto _LL339;} else{ goto _LL340;}} else{ goto _LL340;} _LL340:
if(*(( int*) _temp334) == Cyc_CfFlowInfo_TuplePI){ _LL351: _temp350=(( struct
Cyc_CfFlowInfo_TuplePI_struct*) _temp334)->f1; goto _LL341;} else{ goto _LL342;}
_LL342: if(*(( int*) _temp334) == Cyc_CfFlowInfo_StructPI){ _LL353: _temp352=((
struct Cyc_CfFlowInfo_StructPI_struct*) _temp334)->f1; goto _LL343;} else{ goto
_LL335;} _LL337: return; _LL339: Cyc_CfFlowInfo_add_place( _temp330, _temp332,
_temp348); return; _LL341:(( void(*)( void(* f)( struct _tuple8*, int, void*),
struct _tuple8* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)(
struct _tuple8* env, int a, void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces, env,
_temp350); return; _LL343:(( void(*)( void(* f)( struct _tuple8*, struct
_tagged_arr*, void*), struct _tuple8* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)((
void(*)( struct _tuple8* env, struct _tagged_arr* a, void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces,
env, _temp352); return; _LL335:;}} struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_these(
struct _RegionHandle* rgn, struct Cyc_List_List* places, struct Cyc_Dict_Dict* d){
while( places != 0) { struct Cyc_CfFlowInfo_Place* _temp354=( struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*) _check_null( places))->hd; places=(( struct Cyc_List_List*)
_check_null( places))->tl;{ void* place_old_pinfo;{ struct _handler_cons
_temp355; _push_handler(& _temp355);{ int _temp357= 0; if( setjmp( _temp355.handler)){
_temp357= 1;} if( ! _temp357){ place_old_pinfo= Cyc_CfFlowInfo_lookup_place( d,
_temp354);; _pop_handler();} else{ void* _temp356=( void*) _exn_thrown; void*
_temp359= _temp356; _LL361: if( _temp359 == Cyc_Dict_Absent){ goto _LL362;}
else{ goto _LL363;} _LL363: goto _LL364; _LL362: continue; _LL364:( void) _throw(
_temp359); _LL360:;}}}{ struct _tuple7 _temp365=({ struct _tuple7 _temp367;
_temp367.f1= d; _temp367.f2=& places; _temp367.f3= rgn; _temp367;}); void*
_temp366= Cyc_CfFlowInfo_pinfo_getplaces_esc(& _temp365, place_old_pinfo); if(
place_old_pinfo != _temp366){ d= Cyc_CfFlowInfo_insert_place( d, _temp354,
_temp366);}}}} return d;} struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_pointsto(
void* pinfo, struct Cyc_Dict_Dict* d){ struct _RegionHandle _temp368=
_new_region(); struct _RegionHandle* r=& _temp368; _push_region( r);{ struct Cyc_List_List*
_temp369= 0; struct _tuple8 _temp370=({ struct _tuple8 _temp372; _temp372.f1=&
_temp369; _temp372.f2= r; _temp372;});(( void(*)( struct _tuple8* env, int a,
void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces)(& _temp370, 0, pinfo);{ struct Cyc_Dict_Dict*
_temp371= Cyc_CfFlowInfo_escape_these( r, _temp369, d); _npop_handler( 0u);
return _temp371;}}; _pop_region( r);} void* Cyc_CfFlowInfo_assign_unknown_dict(
void* level, void* pinfo){ void* _temp373= pinfo; void* _temp383; void* _temp385;
struct Cyc_Dict_Dict* _temp387; struct Cyc_Dict_Dict* _temp389; _LL375: if(*((
int*) _temp373) == Cyc_CfFlowInfo_LeafPI){ _LL384: _temp383=( void*)(( struct
Cyc_CfFlowInfo_LeafPI_struct*) _temp373)->f1; if(*(( int*) _temp383) == Cyc_CfFlowInfo_UnknownIS){
_LL386: _temp385=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp383)->f1;
if( _temp385 ==( void*) Cyc_CfFlowInfo_Esc){ goto _LL376;} else{ goto _LL377;}}
else{ goto _LL377;}} else{ goto _LL377;} _LL377: if(*(( int*) _temp373) == Cyc_CfFlowInfo_LeafPI){
goto _LL378;} else{ goto _LL379;} _LL379: if(*(( int*) _temp373) == Cyc_CfFlowInfo_TuplePI){
_LL388: _temp387=(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp373)->f1; goto
_LL380;} else{ goto _LL381;} _LL381: if(*(( int*) _temp373) == Cyc_CfFlowInfo_StructPI){
_LL390: _temp389=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp373)->f1; goto
_LL382;} else{ goto _LL374;} _LL376: return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,
level); _LL378: return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Unesc,
level); _LL380: return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct* _temp391=(
struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp391[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp392; _temp392.tag= Cyc_CfFlowInfo_TuplePI;
_temp392.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( void*, void*), void* env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_assign_unknown_dict,
level, _temp387); _temp392;}); _temp391;}); _LL382: return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp393=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp393[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp394; _temp394.tag=
Cyc_CfFlowInfo_StructPI; _temp394.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)(
void*, void*), void* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_assign_unknown_dict,
level, _temp389); _temp394;}); _temp393;}); _LL374:;} struct Cyc_CfFlowInfo_JoinEnv{
struct Cyc_Dict_Dict* d1; struct Cyc_Dict_Dict* d2; struct Cyc_List_List**
places; struct _RegionHandle* rgn; } ; void* Cyc_CfFlowInfo_join_escape( void*
esc1, void* esc2){ struct _tuple3 _temp396=({ struct _tuple3 _temp395; _temp395.f1=
esc1; _temp395.f2= esc2; _temp395;}); void* _temp404; void* _temp406; _LL398:
_LL405: _temp404= _temp396.f1; if( _temp404 ==( void*) Cyc_CfFlowInfo_Esc){ goto
_LL399;} else{ goto _LL400;} _LL400: _LL407: _temp406= _temp396.f2; if( _temp406
==( void*) Cyc_CfFlowInfo_Esc){ goto _LL401;} else{ goto _LL402;} _LL402: goto
_LL403; _LL399: goto _LL401; _LL401: return( void*) Cyc_CfFlowInfo_Esc; _LL403:
return( void*) Cyc_CfFlowInfo_Unesc; _LL397:;} void* Cyc_CfFlowInfo_join_initlevel(
void* il1, void* il2){ struct _tuple3 _temp409=({ struct _tuple3 _temp408;
_temp408.f1= il1; _temp408.f2= il2; _temp408;}); void* _temp421; void* _temp423;
void* _temp425; void* _temp427; _LL411: _LL422: _temp421= _temp409.f1; if(
_temp421 ==( void*) Cyc_CfFlowInfo_NoneIL){ goto _LL412;} else{ goto _LL413;}
_LL413: _LL424: _temp423= _temp409.f2; if( _temp423 ==( void*) Cyc_CfFlowInfo_NoneIL){
goto _LL414;} else{ goto _LL415;} _LL415: _LL426: _temp425= _temp409.f1; if(
_temp425 ==( void*) Cyc_CfFlowInfo_ThisIL){ goto _LL416;} else{ goto _LL417;}
_LL417: _LL428: _temp427= _temp409.f2; if( _temp427 ==( void*) Cyc_CfFlowInfo_ThisIL){
goto _LL418;} else{ goto _LL419;} _LL419: goto _LL420; _LL412: goto _LL414;
_LL414: return( void*) Cyc_CfFlowInfo_NoneIL; _LL416: goto _LL418; _LL418:
return( void*) Cyc_CfFlowInfo_ThisIL; _LL420: return( void*) Cyc_CfFlowInfo_AllIL;
_LL410:;} void* Cyc_CfFlowInfo_join_initstate( struct Cyc_CfFlowInfo_JoinEnv*
env, void* state1, void* state2){ struct _tuple3 _temp430=({ struct _tuple3
_temp429; _temp429.f1= state1; _temp429.f2= state2; _temp429;}); void* _temp440;
void* _temp442; void* _temp444; void* _temp446; void* _temp448; void* _temp450;
void* _temp452; struct Cyc_CfFlowInfo_Place* _temp454; void* _temp456; void*
_temp458; void* _temp460; void* _temp462; void* _temp464; void* _temp466; void*
_temp468; struct Cyc_CfFlowInfo_Place* _temp470; void* _temp472; struct Cyc_CfFlowInfo_Place*
_temp474; void* _temp476; struct Cyc_CfFlowInfo_Place* _temp478; _LL432: _LL447:
_temp446= _temp430.f1; if(*(( int*) _temp446) == Cyc_CfFlowInfo_UnknownIS){
_LL451: _temp450=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp446)->f1;
goto _LL449; _LL449: _temp448=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp446)->f2; goto _LL441;} else{ goto _LL434;} _LL441: _temp440= _temp430.f2;
if(*(( int*) _temp440) == Cyc_CfFlowInfo_UnknownIS){ _LL445: _temp444=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp440)->f1; goto _LL443; _LL443:
_temp442=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp440)->f2; goto
_LL433;} else{ goto _LL434;} _LL434: _LL457: _temp456= _temp430.f1; if(*(( int*)
_temp456) == Cyc_CfFlowInfo_UnknownIS){ _LL461: _temp460=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp456)->f1; goto _LL459; _LL459: _temp458=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp456)->f2; goto _LL453;} else{ goto _LL436;} _LL453: _temp452= _temp430.f2;
if(*(( int*) _temp452) == Cyc_CfFlowInfo_MustPointTo){ _LL455: _temp454=((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp452)->f1; goto _LL435;} else{
goto _LL436;} _LL436: _LL469: _temp468= _temp430.f1; if(*(( int*) _temp468) ==
Cyc_CfFlowInfo_MustPointTo){ _LL471: _temp470=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp468)->f1; goto _LL463;} else{ goto _LL438;} _LL463: _temp462= _temp430.f2;
if(*(( int*) _temp462) == Cyc_CfFlowInfo_UnknownIS){ _LL467: _temp466=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp462)->f1; goto _LL465; _LL465:
_temp464=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp462)->f2; goto
_LL437;} else{ goto _LL438;} _LL438: _LL477: _temp476= _temp430.f1; if(*(( int*)
_temp476) == Cyc_CfFlowInfo_MustPointTo){ _LL479: _temp478=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp476)->f1; goto _LL473;} else{ goto _LL431;} _LL473: _temp472= _temp430.f2;
if(*(( int*) _temp472) == Cyc_CfFlowInfo_MustPointTo){ _LL475: _temp474=((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp472)->f1; goto _LL439;} else{
goto _LL431;} _LL433: return Cyc_CfFlowInfo_mkLeafPI( Cyc_CfFlowInfo_join_escape(
_temp450, _temp444), Cyc_CfFlowInfo_join_initlevel( _temp448, _temp442)); _LL435: {
struct _tuple6 _temp480=({ struct _tuple6 _temp481; _temp481.f1= env->d2;
_temp481.f2= 0; _temp481;}); if( _temp458 ==( void*) Cyc_CfFlowInfo_AllIL? Cyc_CfFlowInfo_isAllInit_pinfo(&
_temp480, Cyc_CfFlowInfo_lookup_place( env->d2, _temp454)): 0){ Cyc_CfFlowInfo_add_place(
env->rgn, env->places, _temp454); return Cyc_CfFlowInfo_mkLeafPI( _temp460,
_temp458);} return Cyc_CfFlowInfo_mkLeafPI( _temp460, Cyc_CfFlowInfo_join_initlevel(
_temp458,( void*) Cyc_CfFlowInfo_ThisIL));} _LL437: { struct _tuple6 _temp482=({
struct _tuple6 _temp483; _temp483.f1= env->d1; _temp483.f2= 0; _temp483;}); if(
_temp464 ==( void*) Cyc_CfFlowInfo_AllIL? Cyc_CfFlowInfo_isAllInit_pinfo(&
_temp482, Cyc_CfFlowInfo_lookup_place( env->d1, _temp470)): 0){ Cyc_CfFlowInfo_add_place(
env->rgn, env->places, _temp470); return Cyc_CfFlowInfo_mkLeafPI( _temp466,
_temp464);} return Cyc_CfFlowInfo_mkLeafPI( _temp466, Cyc_CfFlowInfo_join_initlevel((
void*) Cyc_CfFlowInfo_ThisIL, _temp464));} _LL439: if( Cyc_CfFlowInfo_place_cmp(
_temp478, _temp474) == 0){ return( void*)({ struct Cyc_CfFlowInfo_LeafPI_struct*
_temp484=( struct Cyc_CfFlowInfo_LeafPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_LeafPI_struct));
_temp484[ 0]=({ struct Cyc_CfFlowInfo_LeafPI_struct _temp485; _temp485.tag= Cyc_CfFlowInfo_LeafPI;
_temp485.f1=( void*) state1; _temp485;}); _temp484;});}{ struct _tuple6 _temp486=({
struct _tuple6 _temp489; _temp489.f1= env->d1; _temp489.f2= 0; _temp489;});
struct _tuple6 _temp487=({ struct _tuple6 _temp488; _temp488.f1= env->d2;
_temp488.f2= 0; _temp488;}); if( Cyc_CfFlowInfo_isAllInit_pinfo(& _temp486, Cyc_CfFlowInfo_lookup_place(
env->d1, _temp478))? Cyc_CfFlowInfo_isAllInit_pinfo(& _temp487, Cyc_CfFlowInfo_lookup_place(
env->d2, _temp474)): 0){ Cyc_CfFlowInfo_add_place( env->rgn, env->places,
_temp478); Cyc_CfFlowInfo_add_place( env->rgn, env->places, _temp474); return
Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Unesc,( void*) Cyc_CfFlowInfo_AllIL);}
return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Unesc,( void*) Cyc_CfFlowInfo_ThisIL);}
_LL431:;} void* Cyc_CfFlowInfo_join_pathinfo( struct Cyc_CfFlowInfo_JoinEnv* env,
void* pinfo1, void* pinfo2){ if( pinfo1 == pinfo2){ return pinfo1;}{ struct
_tuple3 _temp491=({ struct _tuple3 _temp490; _temp490.f1= pinfo1; _temp490.f2=
pinfo2; _temp490;}); void* _temp501; void* _temp503; void* _temp505; void*
_temp507; void* _temp509; struct Cyc_Dict_Dict* _temp511; void* _temp513; struct
Cyc_Dict_Dict* _temp515; void* _temp517; struct Cyc_Dict_Dict* _temp519; void*
_temp521; struct Cyc_Dict_Dict* _temp523; _LL493: _LL506: _temp505= _temp491.f1;
if(*(( int*) _temp505) == Cyc_CfFlowInfo_LeafPI){ _LL508: _temp507=( void*)((
struct Cyc_CfFlowInfo_LeafPI_struct*) _temp505)->f1; goto _LL502;} else{ goto
_LL495;} _LL502: _temp501= _temp491.f2; if(*(( int*) _temp501) == Cyc_CfFlowInfo_LeafPI){
_LL504: _temp503=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp501)->f1;
goto _LL494;} else{ goto _LL495;} _LL495: _LL514: _temp513= _temp491.f1; if(*((
int*) _temp513) == Cyc_CfFlowInfo_TuplePI){ _LL516: _temp515=(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp513)->f1; goto _LL510;} else{ goto _LL497;} _LL510: _temp509= _temp491.f2;
if(*(( int*) _temp509) == Cyc_CfFlowInfo_TuplePI){ _LL512: _temp511=(( struct
Cyc_CfFlowInfo_TuplePI_struct*) _temp509)->f1; goto _LL496;} else{ goto _LL497;}
_LL497: _LL522: _temp521= _temp491.f1; if(*(( int*) _temp521) == Cyc_CfFlowInfo_StructPI){
_LL524: _temp523=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp521)->f1; goto
_LL518;} else{ goto _LL499;} _LL518: _temp517= _temp491.f2; if(*(( int*)
_temp517) == Cyc_CfFlowInfo_StructPI){ _LL520: _temp519=(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp517)->f1; goto _LL498;} else{ goto _LL499;} _LL499: goto _LL500; _LL494:
if( _temp507 == _temp503){ return pinfo1;} return Cyc_CfFlowInfo_join_initstate(
env, _temp507, _temp503); _LL496: if( _temp515 == _temp511){ return pinfo1;}
return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct* _temp525=( struct Cyc_CfFlowInfo_TuplePI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct)); _temp525[ 0]=({
struct Cyc_CfFlowInfo_TuplePI_struct _temp526; _temp526.tag= Cyc_CfFlowInfo_TuplePI;
_temp526.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct Cyc_CfFlowInfo_JoinEnv*,
void*, void*), struct Cyc_CfFlowInfo_JoinEnv* env, struct Cyc_Dict_Dict* d1,
struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)( Cyc_CfFlowInfo_join_pathinfo,
env, _temp515, _temp511); _temp526;}); _temp525;}); _LL498: if( _temp523 ==
_temp519){ return pinfo1;} return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp527=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp527[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp528; _temp528.tag=
Cyc_CfFlowInfo_StructPI; _temp528.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)(
struct Cyc_CfFlowInfo_JoinEnv*, void*, void*), struct Cyc_CfFlowInfo_JoinEnv*
env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)(
Cyc_CfFlowInfo_join_pathinfo, env, _temp523, _temp519); _temp528;}); _temp527;});
_LL500:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp529=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp529[ 0]=({ struct Cyc_Core_Impossible_struct _temp530; _temp530.tag= Cyc_Core_Impossible;
_temp530.f1= _tag_arr("join_pathinfo", sizeof( unsigned char), 14u); _temp530;});
_temp529;})); _LL492:;}} void* Cyc_CfFlowInfo_join_flow( void* f1, void* f2){
if( f1 == f2){ return f1;}{ struct _tuple3 _temp532=({ struct _tuple3 _temp531;
_temp531.f1= f1; _temp531.f2= f2; _temp531;}); void* _temp540; void* _temp542;
void* _temp544; struct Cyc_Dict_Dict* _temp546; void* _temp548; struct Cyc_Dict_Dict*
_temp550; _LL534: _LL541: _temp540= _temp532.f1; if( _temp540 ==( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL535;} else{ goto _LL536;} _LL536: _LL543: _temp542= _temp532.f2; if(
_temp542 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL537;} else{ goto _LL538;}
_LL538: _LL549: _temp548= _temp532.f1; if(( unsigned int) _temp548 > 1u?*(( int*)
_temp548) == Cyc_CfFlowInfo_InitsFL: 0){ _LL551: _temp550=(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp548)->f1; goto _LL545;} else{ goto _LL533;} _LL545: _temp544= _temp532.f2;
if(( unsigned int) _temp544 > 1u?*(( int*) _temp544) == Cyc_CfFlowInfo_InitsFL:
0){ _LL547: _temp546=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp544)->f1;
goto _LL539;} else{ goto _LL533;} _LL535: return f2; _LL537: return f1; _LL539:
if( _temp550 == _temp546){ return f1;} if( Cyc_CfFlowInfo_flow_lessthan_approx(
f1, f2)){ return f2;} if( Cyc_CfFlowInfo_flow_lessthan_approx( f2, f1)){ return
f1;}{ struct _RegionHandle _temp552= _new_region(); struct _RegionHandle* r=&
_temp552; _push_region( r);{ struct Cyc_List_List* _temp553= 0; struct Cyc_CfFlowInfo_JoinEnv
_temp554=({ struct Cyc_CfFlowInfo_JoinEnv _temp559; _temp559.d1= _temp550;
_temp559.d2= _temp546; _temp559.places=& _temp553; _temp559.rgn= r; _temp559;});
struct Cyc_Dict_Dict* _temp555=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct
Cyc_CfFlowInfo_JoinEnv*, void*, void*), struct Cyc_CfFlowInfo_JoinEnv* env,
struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)( Cyc_CfFlowInfo_join_pathinfo,&
_temp554, _temp550, _temp546); void* _temp558=( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp556=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp556[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp557; _temp557.tag= Cyc_CfFlowInfo_InitsFL;
_temp557.f1= Cyc_CfFlowInfo_escape_these( r, _temp553, _temp555); _temp557;});
_temp556;}); _npop_handler( 0u); return _temp558;}; _pop_region( r);} _LL533:;}}
int Cyc_CfFlowInfo_initstate_lessthan_approx( void* state1, void* state2){
struct _tuple3 _temp561=({ struct _tuple3 _temp560; _temp560.f1= state1;
_temp560.f2= state2; _temp560;}); void* _temp569; void* _temp571; void* _temp573;
void* _temp575; void* _temp577; void* _temp579; void* _temp581; struct Cyc_CfFlowInfo_Place*
_temp583; void* _temp585; struct Cyc_CfFlowInfo_Place* _temp587; _LL563: _LL576:
_temp575= _temp561.f1; if(*(( int*) _temp575) == Cyc_CfFlowInfo_UnknownIS){
_LL580: _temp579=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp575)->f1;
goto _LL578; _LL578: _temp577=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp575)->f2; goto _LL570;} else{ goto _LL565;} _LL570: _temp569= _temp561.f2;
if(*(( int*) _temp569) == Cyc_CfFlowInfo_UnknownIS){ _LL574: _temp573=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp569)->f1; goto _LL572; _LL572:
_temp571=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp569)->f2; goto
_LL564;} else{ goto _LL565;} _LL565: _LL586: _temp585= _temp561.f1; if(*(( int*)
_temp585) == Cyc_CfFlowInfo_MustPointTo){ _LL588: _temp587=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp585)->f1; goto _LL582;} else{ goto _LL567;} _LL582: _temp581= _temp561.f2;
if(*(( int*) _temp581) == Cyc_CfFlowInfo_MustPointTo){ _LL584: _temp583=((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp581)->f1; goto _LL566;} else{
goto _LL567;} _LL567: goto _LL568; _LL564: return Cyc_CfFlowInfo_join_escape(
_temp579, _temp573) == _temp573? Cyc_CfFlowInfo_join_initlevel( _temp577,
_temp571) == _temp571: 0; _LL566: return Cyc_CfFlowInfo_place_cmp( _temp587,
_temp583) == 0; _LL568: return 0; _LL562:;} int Cyc_CfFlowInfo_pinfo_lessthan_approx(
void* ignore, void* pinfo1, void* pinfo2){ if( pinfo1 == pinfo2){ return 1;}{
struct _tuple3 _temp590=({ struct _tuple3 _temp589; _temp589.f1= pinfo1;
_temp589.f2= pinfo2; _temp589;}); void* _temp600; void* _temp602; void* _temp604;
void* _temp606; void* _temp608; struct Cyc_Dict_Dict* _temp610; void* _temp612;
struct Cyc_Dict_Dict* _temp614; void* _temp616; struct Cyc_Dict_Dict* _temp618;
void* _temp620; struct Cyc_Dict_Dict* _temp622; _LL592: _LL605: _temp604=
_temp590.f1; if(*(( int*) _temp604) == Cyc_CfFlowInfo_LeafPI){ _LL607: _temp606=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp604)->f1; goto _LL601;}
else{ goto _LL594;} _LL601: _temp600= _temp590.f2; if(*(( int*) _temp600) == Cyc_CfFlowInfo_LeafPI){
_LL603: _temp602=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp600)->f1;
goto _LL593;} else{ goto _LL594;} _LL594: _LL613: _temp612= _temp590.f1; if(*((
int*) _temp612) == Cyc_CfFlowInfo_TuplePI){ _LL615: _temp614=(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp612)->f1; goto _LL609;} else{ goto _LL596;} _LL609: _temp608= _temp590.f2;
if(*(( int*) _temp608) == Cyc_CfFlowInfo_TuplePI){ _LL611: _temp610=(( struct
Cyc_CfFlowInfo_TuplePI_struct*) _temp608)->f1; goto _LL595;} else{ goto _LL596;}
_LL596: _LL621: _temp620= _temp590.f1; if(*(( int*) _temp620) == Cyc_CfFlowInfo_StructPI){
_LL623: _temp622=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp620)->f1; goto
_LL617;} else{ goto _LL598;} _LL617: _temp616= _temp590.f2; if(*(( int*)
_temp616) == Cyc_CfFlowInfo_StructPI){ _LL619: _temp618=(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp616)->f1; goto _LL597;} else{ goto _LL598;} _LL598: goto _LL599; _LL593:
return _temp606 == _temp602? 1: Cyc_CfFlowInfo_initstate_lessthan_approx(
_temp606, _temp602); _LL595: return _temp614 == _temp610? 1:(( int(*)( int(* f)(
int, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_forall_intersect)((
int(*)( int ignore, void* pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx,
_temp614, _temp610); _LL597: return _temp622 == _temp618? 1:(( int(*)( int(* f)(
struct _tagged_arr*, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Dict_forall_intersect)(( int(*)( struct _tagged_arr* ignore, void*
pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx, _temp622, _temp618);
_LL599:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp624=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp624[ 0]=({ struct Cyc_Core_Impossible_struct _temp625; _temp625.tag= Cyc_Core_Impossible;
_temp625.f1= _tag_arr("pinfo_lessthan", sizeof( unsigned char), 15u); _temp625;});
_temp624;})); _LL591:;}} int Cyc_CfFlowInfo_flow_lessthan_approx( void* f1, void*
f2){ if( f1 == f2){ return 1;}{ struct _tuple3 _temp627=({ struct _tuple3
_temp626; _temp626.f1= f1; _temp626.f2= f2; _temp626;}); void* _temp635; void*
_temp637; void* _temp639; struct Cyc_Dict_Dict* _temp641; void* _temp643; struct
Cyc_Dict_Dict* _temp645; _LL629: _LL636: _temp635= _temp627.f1; if( _temp635 ==(
void*) Cyc_CfFlowInfo_BottomFL){ goto _LL630;} else{ goto _LL631;} _LL631:
_LL638: _temp637= _temp627.f2; if( _temp637 ==( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL632;} else{ goto _LL633;} _LL633: _LL644: _temp643= _temp627.f1; if((
unsigned int) _temp643 > 1u?*(( int*) _temp643) == Cyc_CfFlowInfo_InitsFL: 0){
_LL646: _temp645=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp643)->f1; goto
_LL640;} else{ goto _LL628;} _LL640: _temp639= _temp627.f2; if(( unsigned int)
_temp639 > 1u?*(( int*) _temp639) == Cyc_CfFlowInfo_InitsFL: 0){ _LL642:
_temp641=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp639)->f1; goto _LL634;}
else{ goto _LL628;} _LL630: return 1; _LL632: return 0; _LL634: if( _temp645 ==
_temp641){ return 1;} return Cyc_Dict_forall_intersect( Cyc_CfFlowInfo_pinfo_lessthan_approx,
_temp645, _temp641); _LL628:;}}