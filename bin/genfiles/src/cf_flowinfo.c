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
Cyc_Absyn_Tqual tq; void* type; struct Cyc_Absyn_Exp* width; struct Cyc_List_List*
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
goto _LL126; _LL126: { struct Cyc_Dict_Dict* d=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( int, int))) Cyc_Dict_empty)( Cyc_Core_intcmp);{ int i= 0; for( 0;
_temp139 != 0;( _temp139=(( struct Cyc_List_List*) _check_null( _temp139))->tl,
++ i)){ struct _tuple4 _temp154; void* _temp155; struct _tuple4* _temp152=(
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp139))->hd; _temp154=*
_temp152; _LL156: _temp155= _temp154.f2; goto _LL153; _LL153: d=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, int key, void* data)) Cyc_Dict_insert)( d, i, Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(
_temp155));}} return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct* _temp157=(
struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp157[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp158; _temp158.tag= Cyc_CfFlowInfo_TuplePI;
_temp158.f1= d; _temp158;}); _temp157;});} _LL128: _temp150= _temp141; goto
_LL130; _LL130: { struct Cyc_Dict_Dict* d=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
for( 0; _temp150 != 0; _temp150=(( struct Cyc_List_List*) _check_null( _temp150))->tl){
struct Cyc_Absyn_Structfield _temp161; void* _temp162; struct _tagged_arr*
_temp164; struct Cyc_Absyn_Structfield* _temp159=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp150))->hd; _temp161=* _temp159; _LL165:
_temp164= _temp161.name; goto _LL163; _LL163: _temp162=( void*) _temp161.type;
goto _LL160; _LL160: if( Cyc_String_strcmp(* _temp164, _tag_arr("", sizeof(
unsigned char), 1u)) != 0){ d=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, void* data)) Cyc_Dict_insert)( d, _temp164, Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(
_temp162));}} return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct* _temp166=(
struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp166[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp167; _temp167.tag=
Cyc_CfFlowInfo_StructPI; _temp167.f1= d; _temp167;}); _temp166;});} _LL132:
return Cyc_CfFlowInfo_leaf_unesc_none; _LL122:;} void* Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place){ struct Cyc_CfFlowInfo_Place
_temp170; struct Cyc_List_List* _temp171; void* _temp173; struct Cyc_CfFlowInfo_Place*
_temp168= place; _temp170=* _temp168; _LL174: _temp173=( void*) _temp170.root;
goto _LL172; _LL172: _temp171= _temp170.fields; goto _LL169; _LL169: { void*
_temp175= Cyc_Dict_lookup( d, _temp173); for( 0; _temp171 != 0; _temp171=((
struct Cyc_List_List*) _check_null( _temp171))->tl){ struct _tuple3 _temp177=({
struct _tuple3 _temp176; _temp176.f1= _temp175; _temp176.f2=( void*)(( struct
Cyc_List_List*) _check_null( _temp171))->hd; _temp176;}); void* _temp185; int
_temp187; void* _temp189; struct Cyc_Dict_Dict* _temp191; void* _temp193; struct
_tagged_arr* _temp195; void* _temp197; struct Cyc_Dict_Dict* _temp199; _LL179:
_LL190: _temp189= _temp177.f1; if(*(( int*) _temp189) == Cyc_CfFlowInfo_TuplePI){
_LL192: _temp191=(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp189)->f1; goto
_LL186;} else{ goto _LL181;} _LL186: _temp185= _temp177.f2; if(*(( int*)
_temp185) == Cyc_CfFlowInfo_TupleF){ _LL188: _temp187=(( struct Cyc_CfFlowInfo_TupleF_struct*)
_temp185)->f1; goto _LL180;} else{ goto _LL181;} _LL181: _LL198: _temp197=
_temp177.f1; if(*(( int*) _temp197) == Cyc_CfFlowInfo_StructPI){ _LL200:
_temp199=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp197)->f1; goto _LL194;}
else{ goto _LL183;} _LL194: _temp193= _temp177.f2; if(*(( int*) _temp193) == Cyc_CfFlowInfo_StructF){
_LL196: _temp195=(( struct Cyc_CfFlowInfo_StructF_struct*) _temp193)->f1; goto
_LL182;} else{ goto _LL183;} _LL183: goto _LL184; _LL180: _temp175=(( void*(*)(
struct Cyc_Dict_Dict* d, int key)) Cyc_Dict_lookup)( _temp191, _temp187); goto
_LL178; _LL182: _temp175=(( void*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* key)) Cyc_Dict_lookup)( _temp199, _temp195); goto _LL178; _LL184:(
int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp201=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp201[ 0]=({ struct
Cyc_Core_Impossible_struct _temp202; _temp202.tag= Cyc_Core_Impossible; _temp202.f1=
_tag_arr("bad lookup_place", sizeof( unsigned char), 17u); _temp202;}); _temp201;}));
_LL178:;} return _temp175;}} struct _tuple5{ void* f1; struct Cyc_List_List* f2;
} ; void* Cyc_CfFlowInfo_insert_place_rec( void* insert_path, struct Cyc_List_List*
fields, void* pinfo){ if( fields == 0){ return pinfo;}{ struct _tuple5 _temp204=({
struct _tuple5 _temp203; _temp203.f1= insert_path; _temp203.f2= fields; _temp203;});
struct Cyc_List_List* _temp212; struct Cyc_List_List _temp214; struct Cyc_List_List*
_temp215; void* _temp217; int _temp219; void* _temp221; struct Cyc_Dict_Dict*
_temp223; struct Cyc_List_List* _temp225; struct Cyc_List_List _temp227; struct
Cyc_List_List* _temp228; void* _temp230; struct _tagged_arr* _temp232; void*
_temp234; struct Cyc_Dict_Dict* _temp236; _LL206: _LL222: _temp221= _temp204.f1;
if(*(( int*) _temp221) == Cyc_CfFlowInfo_TuplePI){ _LL224: _temp223=(( struct
Cyc_CfFlowInfo_TuplePI_struct*) _temp221)->f1; goto _LL213;} else{ goto _LL208;}
_LL213: _temp212= _temp204.f2; if( _temp212 == 0){ goto _LL208;} else{ _temp214=*
_temp212; _LL218: _temp217=( void*) _temp214.hd; if(*(( int*) _temp217) == Cyc_CfFlowInfo_TupleF){
_LL220: _temp219=(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp217)->f1; goto
_LL216;} else{ goto _LL208;} _LL216: _temp215= _temp214.tl; goto _LL207;} _LL208:
_LL235: _temp234= _temp204.f1; if(*(( int*) _temp234) == Cyc_CfFlowInfo_StructPI){
_LL237: _temp236=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp234)->f1; goto
_LL226;} else{ goto _LL210;} _LL226: _temp225= _temp204.f2; if( _temp225 == 0){
goto _LL210;} else{ _temp227=* _temp225; _LL231: _temp230=( void*) _temp227.hd;
if(*(( int*) _temp230) == Cyc_CfFlowInfo_StructF){ _LL233: _temp232=(( struct
Cyc_CfFlowInfo_StructF_struct*) _temp230)->f1; goto _LL229;} else{ goto _LL210;}
_LL229: _temp228= _temp227.tl; goto _LL209;} _LL210: goto _LL211; _LL207: { void*
_temp238=(( void*(*)( struct Cyc_Dict_Dict* d, int key)) Cyc_Dict_lookup)(
_temp223, _temp219); return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct*
_temp239=( struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp239[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp240; _temp240.tag= Cyc_CfFlowInfo_TuplePI;
_temp240.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, int key, void*
data)) Cyc_Dict_insert)( _temp223, _temp219, Cyc_CfFlowInfo_insert_place_rec(
_temp238, _temp215, pinfo)); _temp240;}); _temp239;});} _LL209: { void* _temp241=((
void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup)(
_temp236, _temp232); return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp242=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp242[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp243; _temp243.tag=
Cyc_CfFlowInfo_StructPI; _temp243.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* key, void* data)) Cyc_Dict_insert)( _temp236, _temp232,
Cyc_CfFlowInfo_insert_place_rec( _temp241, _temp228, pinfo)); _temp243;});
_temp242;});} _LL211:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp244=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp244[ 0]=({ struct Cyc_Core_Impossible_struct _temp245; _temp245.tag= Cyc_Core_Impossible;
_temp245.f1= _tag_arr("bad insert_place", sizeof( unsigned char), 17u); _temp245;});
_temp244;})); _LL205:;}} struct Cyc_Dict_Dict* Cyc_CfFlowInfo_insert_place(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place, void* pinfo){
struct Cyc_CfFlowInfo_Place _temp248; struct Cyc_List_List* _temp249; void*
_temp251; struct Cyc_CfFlowInfo_Place* _temp246= place; _temp248=* _temp246;
_LL252: _temp251=( void*) _temp248.root; goto _LL250; _LL250: _temp249= _temp248.fields;
goto _LL247; _LL247: return Cyc_Dict_insert( d, _temp251, Cyc_CfFlowInfo_insert_place_rec(
Cyc_Dict_lookup( d, _temp251), _temp249, pinfo));} struct _tuple6{ struct Cyc_Dict_Dict*
f1; struct Cyc_List_List* f2; } ; int Cyc_CfFlowInfo_isAllInit_pinfo( struct
_tuple6* env, void* path); int Cyc_CfFlowInfo_isAllInit_is( struct _tuple6* env,
void* state){ void* _temp253= state; void* _temp261; struct Cyc_CfFlowInfo_Place*
_temp263; _LL255: if(*(( int*) _temp253) == Cyc_CfFlowInfo_UnknownIS){ _LL262:
_temp261=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp253)->f2; if(
_temp261 ==( void*) Cyc_CfFlowInfo_AllIL){ goto _LL256;} else{ goto _LL257;}}
else{ goto _LL257;} _LL257: if(*(( int*) _temp253) == Cyc_CfFlowInfo_UnknownIS){
goto _LL258;} else{ goto _LL259;} _LL259: if(*(( int*) _temp253) == Cyc_CfFlowInfo_MustPointTo){
_LL264: _temp263=(( struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp253)->f1;
goto _LL260;} else{ goto _LL254;} _LL256: return 1; _LL258: return 0; _LL260: {
struct _tuple6 _temp267; struct Cyc_List_List* _temp268; struct Cyc_List_List**
_temp270; struct Cyc_Dict_Dict* _temp271; struct _tuple6* _temp265= env;
_temp267=* _temp265; _LL272: _temp271= _temp267.f1; goto _LL269; _LL269:
_temp268= _temp267.f2; _temp270=&(* _temp265).f2; goto _LL266; _LL266: if((( int(*)(
int(* compare)( struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List* l, struct Cyc_CfFlowInfo_Place* x)) Cyc_List_mem)( Cyc_CfFlowInfo_place_cmp,*
_temp270, _temp263)){ return 1;}* _temp270=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp273=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp273->hd=( void*) _temp263; _temp273->tl=* _temp270; _temp273;}); return Cyc_CfFlowInfo_isAllInit_pinfo(
env, Cyc_CfFlowInfo_lookup_place( _temp271, _temp263));} _LL254:;} int Cyc_CfFlowInfo_isAllInit_f(
struct _tuple6* env, void* a, void* path){ return Cyc_CfFlowInfo_isAllInit_pinfo(
env, path);} int Cyc_CfFlowInfo_isAllInit_pinfo( struct _tuple6* env, void* path){
void* _temp274= path; void* _temp282; struct Cyc_Dict_Dict* _temp284; struct Cyc_Dict_Dict*
_temp286; _LL276: if(*(( int*) _temp274) == Cyc_CfFlowInfo_LeafPI){ _LL283:
_temp282=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp274)->f1; goto
_LL277;} else{ goto _LL278;} _LL278: if(*(( int*) _temp274) == Cyc_CfFlowInfo_TuplePI){
_LL285: _temp284=(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp274)->f1; goto
_LL279;} else{ goto _LL280;} _LL280: if(*(( int*) _temp274) == Cyc_CfFlowInfo_StructPI){
_LL287: _temp286=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp274)->f1; goto
_LL281;} else{ goto _LL275;} _LL277: return Cyc_CfFlowInfo_isAllInit_is( env,
_temp282); _LL279: return(( int(*)( int(* f)( struct _tuple6*, int, void*),
struct _tuple6* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_forall_c)(( int(*)(
struct _tuple6* env, int a, void* path)) Cyc_CfFlowInfo_isAllInit_f, env,
_temp284); _LL281: return(( int(*)( int(* f)( struct _tuple6*, struct
_tagged_arr*, void*), struct _tuple6* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_forall_c)((
int(*)( struct _tuple6* env, struct _tagged_arr* a, void* path)) Cyc_CfFlowInfo_isAllInit_f,
env, _temp286); _LL275:;} int Cyc_CfFlowInfo_isAllInit( struct Cyc_Dict_Dict*
pinfo_dict, void* pinfo){ struct _tuple6 _temp288=({ struct _tuple6 _temp289;
_temp289.f1= pinfo_dict; _temp289.f2= 0; _temp289;}); return Cyc_CfFlowInfo_isAllInit_pinfo(&
_temp288, pinfo);} static void Cyc_CfFlowInfo_add_place( struct _RegionHandle* r,
struct Cyc_List_List** places, struct Cyc_CfFlowInfo_Place* place){ if( !(( int(*)(
int(* compare)( struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List* l, struct Cyc_CfFlowInfo_Place* x)) Cyc_List_mem)( Cyc_CfFlowInfo_place_cmp,*
places, place)){* places=({ struct Cyc_List_List* _temp290=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp290->hd=( void*) place;
_temp290->tl=* places; _temp290;});}} struct _tuple7{ struct Cyc_Dict_Dict* f1;
struct Cyc_List_List** f2; struct _RegionHandle* f3; } ; void* Cyc_CfFlowInfo_pinfo_getplaces_esc(
struct _tuple7* env, void* pinfo){ struct _tuple7 _temp293; struct _RegionHandle*
_temp294; struct Cyc_List_List** _temp296; struct Cyc_Dict_Dict* _temp298;
struct _tuple7* _temp291= env; _temp293=* _temp291; _LL299: _temp298= _temp293.f1;
goto _LL297; _LL297: _temp296= _temp293.f2; goto _LL295; _LL295: _temp294=
_temp293.f3; goto _LL292; _LL292: { void* _temp300= pinfo; void* _temp310;
struct Cyc_CfFlowInfo_Place* _temp312; void* _temp314; void* _temp316; struct
Cyc_Dict_Dict* _temp318; struct Cyc_Dict_Dict* _temp320; _LL302: if(*(( int*)
_temp300) == Cyc_CfFlowInfo_LeafPI){ _LL311: _temp310=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp300)->f1; if(*(( int*) _temp310) == Cyc_CfFlowInfo_MustPointTo){ _LL313:
_temp312=(( struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp310)->f1; goto
_LL303;} else{ goto _LL304;}} else{ goto _LL304;} _LL304: if(*(( int*) _temp300)
== Cyc_CfFlowInfo_LeafPI){ _LL315: _temp314=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp300)->f1; if(*(( int*) _temp314) == Cyc_CfFlowInfo_UnknownIS){ _LL317:
_temp316=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp314)->f2; goto
_LL305;} else{ goto _LL306;}} else{ goto _LL306;} _LL306: if(*(( int*) _temp300)
== Cyc_CfFlowInfo_TuplePI){ _LL319: _temp318=(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp300)->f1; goto _LL307;} else{ goto _LL308;} _LL308: if(*(( int*) _temp300)
== Cyc_CfFlowInfo_StructPI){ _LL321: _temp320=(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp300)->f1; goto _LL309;} else{ goto _LL301;} _LL303: Cyc_CfFlowInfo_add_place(
_temp294, _temp296, _temp312); if( Cyc_CfFlowInfo_isAllInit( _temp298, Cyc_CfFlowInfo_lookup_place(
_temp298, _temp312))){ return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,(
void*) Cyc_CfFlowInfo_AllIL);} return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,(
void*) Cyc_CfFlowInfo_ThisIL); _LL305: return Cyc_CfFlowInfo_mkLeafPI(( void*)
Cyc_CfFlowInfo_Esc, _temp316); _LL307: return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct*
_temp322=( struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp322[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp323; _temp323.tag= Cyc_CfFlowInfo_TuplePI;
_temp323.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct _tuple7*, void*),
struct _tuple7* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_pinfo_getplaces_esc,
env, _temp318); _temp323;}); _temp322;}); _LL309: return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp324=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp324[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp325; _temp325.tag=
Cyc_CfFlowInfo_StructPI; _temp325.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)(
struct _tuple7*, void*), struct _tuple7* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)(
Cyc_CfFlowInfo_pinfo_getplaces_esc, env, _temp320); _temp325;}); _temp324;});
_LL301:;}} struct _tuple8{ struct Cyc_List_List** f1; struct _RegionHandle* f2;
} ; void Cyc_CfFlowInfo_pinfo_getplaces( struct _tuple8* env, void* a, void*
pinfo){ struct _tuple8 _temp328; struct _RegionHandle* _temp329; struct Cyc_List_List**
_temp331; struct _tuple8* _temp326= env; _temp328=* _temp326; _LL332: _temp331=
_temp328.f1; goto _LL330; _LL330: _temp329= _temp328.f2; goto _LL327; _LL327: {
void* _temp333= pinfo; void* _temp343; void* _temp345; struct Cyc_CfFlowInfo_Place*
_temp347; struct Cyc_Dict_Dict* _temp349; struct Cyc_Dict_Dict* _temp351; _LL335:
if(*(( int*) _temp333) == Cyc_CfFlowInfo_LeafPI){ _LL344: _temp343=( void*)((
struct Cyc_CfFlowInfo_LeafPI_struct*) _temp333)->f1; if(*(( int*) _temp343) ==
Cyc_CfFlowInfo_UnknownIS){ goto _LL336;} else{ goto _LL337;}} else{ goto _LL337;}
_LL337: if(*(( int*) _temp333) == Cyc_CfFlowInfo_LeafPI){ _LL346: _temp345=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp333)->f1; if(*(( int*)
_temp345) == Cyc_CfFlowInfo_MustPointTo){ _LL348: _temp347=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp345)->f1; goto _LL338;} else{ goto _LL339;}} else{ goto _LL339;} _LL339:
if(*(( int*) _temp333) == Cyc_CfFlowInfo_TuplePI){ _LL350: _temp349=(( struct
Cyc_CfFlowInfo_TuplePI_struct*) _temp333)->f1; goto _LL340;} else{ goto _LL341;}
_LL341: if(*(( int*) _temp333) == Cyc_CfFlowInfo_StructPI){ _LL352: _temp351=((
struct Cyc_CfFlowInfo_StructPI_struct*) _temp333)->f1; goto _LL342;} else{ goto
_LL334;} _LL336: return; _LL338: Cyc_CfFlowInfo_add_place( _temp329, _temp331,
_temp347); return; _LL340:(( void(*)( void(* f)( struct _tuple8*, int, void*),
struct _tuple8* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)(
struct _tuple8* env, int a, void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces, env,
_temp349); return; _LL342:(( void(*)( void(* f)( struct _tuple8*, struct
_tagged_arr*, void*), struct _tuple8* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)((
void(*)( struct _tuple8* env, struct _tagged_arr* a, void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces,
env, _temp351); return; _LL334:;}} struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_these(
struct _RegionHandle* rgn, struct Cyc_List_List* places, struct Cyc_Dict_Dict* d){
while( places != 0) { struct Cyc_CfFlowInfo_Place* _temp353=( struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*) _check_null( places))->hd; places=(( struct Cyc_List_List*)
_check_null( places))->tl;{ void* place_old_pinfo;{ struct _handler_cons
_temp354; _push_handler(& _temp354);{ int _temp356= 0; if( setjmp( _temp354.handler)){
_temp356= 1;} if( ! _temp356){ place_old_pinfo= Cyc_CfFlowInfo_lookup_place( d,
_temp353);; _pop_handler();} else{ void* _temp355=( void*) _exn_thrown; void*
_temp358= _temp355; _LL360: if( _temp358 == Cyc_Dict_Absent){ goto _LL361;}
else{ goto _LL362;} _LL362: goto _LL363; _LL361: continue; _LL363:( void) _throw(
_temp358); _LL359:;}}}{ struct _tuple7 _temp364=({ struct _tuple7 _temp366;
_temp366.f1= d; _temp366.f2=& places; _temp366.f3= rgn; _temp366;}); void*
_temp365= Cyc_CfFlowInfo_pinfo_getplaces_esc(& _temp364, place_old_pinfo); if(
place_old_pinfo != _temp365){ d= Cyc_CfFlowInfo_insert_place( d, _temp353,
_temp365);}}}} return d;} struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_pointsto(
void* pinfo, struct Cyc_Dict_Dict* d){ struct _RegionHandle _temp367=
_new_region(); struct _RegionHandle* r=& _temp367; _push_region( r);{ struct Cyc_List_List*
_temp368= 0; struct _tuple8 _temp369=({ struct _tuple8 _temp371; _temp371.f1=&
_temp368; _temp371.f2= r; _temp371;});(( void(*)( struct _tuple8* env, int a,
void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces)(& _temp369, 0, pinfo);{ struct Cyc_Dict_Dict*
_temp370= Cyc_CfFlowInfo_escape_these( r, _temp368, d); _npop_handler( 0u);
return _temp370;}}; _pop_region( r);} void* Cyc_CfFlowInfo_assign_unknown_dict(
void* level, void* pinfo){ void* _temp372= pinfo; void* _temp382; void* _temp384;
struct Cyc_Dict_Dict* _temp386; struct Cyc_Dict_Dict* _temp388; _LL374: if(*((
int*) _temp372) == Cyc_CfFlowInfo_LeafPI){ _LL383: _temp382=( void*)(( struct
Cyc_CfFlowInfo_LeafPI_struct*) _temp372)->f1; if(*(( int*) _temp382) == Cyc_CfFlowInfo_UnknownIS){
_LL385: _temp384=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp382)->f1;
if( _temp384 ==( void*) Cyc_CfFlowInfo_Esc){ goto _LL375;} else{ goto _LL376;}}
else{ goto _LL376;}} else{ goto _LL376;} _LL376: if(*(( int*) _temp372) == Cyc_CfFlowInfo_LeafPI){
goto _LL377;} else{ goto _LL378;} _LL378: if(*(( int*) _temp372) == Cyc_CfFlowInfo_TuplePI){
_LL387: _temp386=(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp372)->f1; goto
_LL379;} else{ goto _LL380;} _LL380: if(*(( int*) _temp372) == Cyc_CfFlowInfo_StructPI){
_LL389: _temp388=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp372)->f1; goto
_LL381;} else{ goto _LL373;} _LL375: return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,
level); _LL377: return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Unesc,
level); _LL379: return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct* _temp390=(
struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp390[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp391; _temp391.tag= Cyc_CfFlowInfo_TuplePI;
_temp391.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( void*, void*), void* env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_assign_unknown_dict,
level, _temp386); _temp391;}); _temp390;}); _LL381: return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp392=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp392[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp393; _temp393.tag=
Cyc_CfFlowInfo_StructPI; _temp393.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)(
void*, void*), void* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_assign_unknown_dict,
level, _temp388); _temp393;}); _temp392;}); _LL373:;} struct Cyc_CfFlowInfo_JoinEnv{
struct Cyc_Dict_Dict* d1; struct Cyc_Dict_Dict* d2; struct Cyc_List_List**
places; struct _RegionHandle* rgn; } ; void* Cyc_CfFlowInfo_join_escape( void*
esc1, void* esc2){ struct _tuple3 _temp395=({ struct _tuple3 _temp394; _temp394.f1=
esc1; _temp394.f2= esc2; _temp394;}); void* _temp403; void* _temp405; _LL397:
_LL404: _temp403= _temp395.f1; if( _temp403 ==( void*) Cyc_CfFlowInfo_Esc){ goto
_LL398;} else{ goto _LL399;} _LL399: _LL406: _temp405= _temp395.f2; if( _temp405
==( void*) Cyc_CfFlowInfo_Esc){ goto _LL400;} else{ goto _LL401;} _LL401: goto
_LL402; _LL398: goto _LL400; _LL400: return( void*) Cyc_CfFlowInfo_Esc; _LL402:
return( void*) Cyc_CfFlowInfo_Unesc; _LL396:;} void* Cyc_CfFlowInfo_join_initlevel(
void* il1, void* il2){ struct _tuple3 _temp408=({ struct _tuple3 _temp407;
_temp407.f1= il1; _temp407.f2= il2; _temp407;}); void* _temp420; void* _temp422;
void* _temp424; void* _temp426; _LL410: _LL421: _temp420= _temp408.f1; if(
_temp420 ==( void*) Cyc_CfFlowInfo_NoneIL){ goto _LL411;} else{ goto _LL412;}
_LL412: _LL423: _temp422= _temp408.f2; if( _temp422 ==( void*) Cyc_CfFlowInfo_NoneIL){
goto _LL413;} else{ goto _LL414;} _LL414: _LL425: _temp424= _temp408.f1; if(
_temp424 ==( void*) Cyc_CfFlowInfo_ThisIL){ goto _LL415;} else{ goto _LL416;}
_LL416: _LL427: _temp426= _temp408.f2; if( _temp426 ==( void*) Cyc_CfFlowInfo_ThisIL){
goto _LL417;} else{ goto _LL418;} _LL418: goto _LL419; _LL411: goto _LL413;
_LL413: return( void*) Cyc_CfFlowInfo_NoneIL; _LL415: goto _LL417; _LL417:
return( void*) Cyc_CfFlowInfo_ThisIL; _LL419: return( void*) Cyc_CfFlowInfo_AllIL;
_LL409:;} void* Cyc_CfFlowInfo_join_initstate( struct Cyc_CfFlowInfo_JoinEnv*
env, void* state1, void* state2){ struct _tuple3 _temp429=({ struct _tuple3
_temp428; _temp428.f1= state1; _temp428.f2= state2; _temp428;}); void* _temp439;
void* _temp441; void* _temp443; void* _temp445; void* _temp447; void* _temp449;
void* _temp451; struct Cyc_CfFlowInfo_Place* _temp453; void* _temp455; void*
_temp457; void* _temp459; void* _temp461; void* _temp463; void* _temp465; void*
_temp467; struct Cyc_CfFlowInfo_Place* _temp469; void* _temp471; struct Cyc_CfFlowInfo_Place*
_temp473; void* _temp475; struct Cyc_CfFlowInfo_Place* _temp477; _LL431: _LL446:
_temp445= _temp429.f1; if(*(( int*) _temp445) == Cyc_CfFlowInfo_UnknownIS){
_LL450: _temp449=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp445)->f1;
goto _LL448; _LL448: _temp447=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp445)->f2; goto _LL440;} else{ goto _LL433;} _LL440: _temp439= _temp429.f2;
if(*(( int*) _temp439) == Cyc_CfFlowInfo_UnknownIS){ _LL444: _temp443=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp439)->f1; goto _LL442; _LL442:
_temp441=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp439)->f2; goto
_LL432;} else{ goto _LL433;} _LL433: _LL456: _temp455= _temp429.f1; if(*(( int*)
_temp455) == Cyc_CfFlowInfo_UnknownIS){ _LL460: _temp459=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp455)->f1; goto _LL458; _LL458: _temp457=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp455)->f2; goto _LL452;} else{ goto _LL435;} _LL452: _temp451= _temp429.f2;
if(*(( int*) _temp451) == Cyc_CfFlowInfo_MustPointTo){ _LL454: _temp453=((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp451)->f1; goto _LL434;} else{
goto _LL435;} _LL435: _LL468: _temp467= _temp429.f1; if(*(( int*) _temp467) ==
Cyc_CfFlowInfo_MustPointTo){ _LL470: _temp469=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp467)->f1; goto _LL462;} else{ goto _LL437;} _LL462: _temp461= _temp429.f2;
if(*(( int*) _temp461) == Cyc_CfFlowInfo_UnknownIS){ _LL466: _temp465=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp461)->f1; goto _LL464; _LL464:
_temp463=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp461)->f2; goto
_LL436;} else{ goto _LL437;} _LL437: _LL476: _temp475= _temp429.f1; if(*(( int*)
_temp475) == Cyc_CfFlowInfo_MustPointTo){ _LL478: _temp477=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp475)->f1; goto _LL472;} else{ goto _LL430;} _LL472: _temp471= _temp429.f2;
if(*(( int*) _temp471) == Cyc_CfFlowInfo_MustPointTo){ _LL474: _temp473=((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp471)->f1; goto _LL438;} else{
goto _LL430;} _LL432: return Cyc_CfFlowInfo_mkLeafPI( Cyc_CfFlowInfo_join_escape(
_temp449, _temp443), Cyc_CfFlowInfo_join_initlevel( _temp447, _temp441)); _LL434: {
struct _tuple6 _temp479=({ struct _tuple6 _temp480; _temp480.f1= env->d2;
_temp480.f2= 0; _temp480;}); if( _temp457 ==( void*) Cyc_CfFlowInfo_AllIL? Cyc_CfFlowInfo_isAllInit_pinfo(&
_temp479, Cyc_CfFlowInfo_lookup_place( env->d2, _temp453)): 0){ Cyc_CfFlowInfo_add_place(
env->rgn, env->places, _temp453); return Cyc_CfFlowInfo_mkLeafPI( _temp459,
_temp457);} return Cyc_CfFlowInfo_mkLeafPI( _temp459, Cyc_CfFlowInfo_join_initlevel(
_temp457,( void*) Cyc_CfFlowInfo_ThisIL));} _LL436: { struct _tuple6 _temp481=({
struct _tuple6 _temp482; _temp482.f1= env->d1; _temp482.f2= 0; _temp482;}); if(
_temp463 ==( void*) Cyc_CfFlowInfo_AllIL? Cyc_CfFlowInfo_isAllInit_pinfo(&
_temp481, Cyc_CfFlowInfo_lookup_place( env->d1, _temp469)): 0){ Cyc_CfFlowInfo_add_place(
env->rgn, env->places, _temp469); return Cyc_CfFlowInfo_mkLeafPI( _temp465,
_temp463);} return Cyc_CfFlowInfo_mkLeafPI( _temp465, Cyc_CfFlowInfo_join_initlevel((
void*) Cyc_CfFlowInfo_ThisIL, _temp463));} _LL438: if( Cyc_CfFlowInfo_place_cmp(
_temp477, _temp473) == 0){ return( void*)({ struct Cyc_CfFlowInfo_LeafPI_struct*
_temp483=( struct Cyc_CfFlowInfo_LeafPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_LeafPI_struct));
_temp483[ 0]=({ struct Cyc_CfFlowInfo_LeafPI_struct _temp484; _temp484.tag= Cyc_CfFlowInfo_LeafPI;
_temp484.f1=( void*) state1; _temp484;}); _temp483;});}{ struct _tuple6 _temp485=({
struct _tuple6 _temp488; _temp488.f1= env->d1; _temp488.f2= 0; _temp488;});
struct _tuple6 _temp486=({ struct _tuple6 _temp487; _temp487.f1= env->d2;
_temp487.f2= 0; _temp487;}); if( Cyc_CfFlowInfo_isAllInit_pinfo(& _temp485, Cyc_CfFlowInfo_lookup_place(
env->d1, _temp477))? Cyc_CfFlowInfo_isAllInit_pinfo(& _temp486, Cyc_CfFlowInfo_lookup_place(
env->d2, _temp473)): 0){ Cyc_CfFlowInfo_add_place( env->rgn, env->places,
_temp477); Cyc_CfFlowInfo_add_place( env->rgn, env->places, _temp473); return
Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Unesc,( void*) Cyc_CfFlowInfo_AllIL);}
return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Unesc,( void*) Cyc_CfFlowInfo_ThisIL);}
_LL430:;} void* Cyc_CfFlowInfo_join_pathinfo( struct Cyc_CfFlowInfo_JoinEnv* env,
void* pinfo1, void* pinfo2){ if( pinfo1 == pinfo2){ return pinfo1;}{ struct
_tuple3 _temp490=({ struct _tuple3 _temp489; _temp489.f1= pinfo1; _temp489.f2=
pinfo2; _temp489;}); void* _temp500; void* _temp502; void* _temp504; void*
_temp506; void* _temp508; struct Cyc_Dict_Dict* _temp510; void* _temp512; struct
Cyc_Dict_Dict* _temp514; void* _temp516; struct Cyc_Dict_Dict* _temp518; void*
_temp520; struct Cyc_Dict_Dict* _temp522; _LL492: _LL505: _temp504= _temp490.f1;
if(*(( int*) _temp504) == Cyc_CfFlowInfo_LeafPI){ _LL507: _temp506=( void*)((
struct Cyc_CfFlowInfo_LeafPI_struct*) _temp504)->f1; goto _LL501;} else{ goto
_LL494;} _LL501: _temp500= _temp490.f2; if(*(( int*) _temp500) == Cyc_CfFlowInfo_LeafPI){
_LL503: _temp502=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp500)->f1;
goto _LL493;} else{ goto _LL494;} _LL494: _LL513: _temp512= _temp490.f1; if(*((
int*) _temp512) == Cyc_CfFlowInfo_TuplePI){ _LL515: _temp514=(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp512)->f1; goto _LL509;} else{ goto _LL496;} _LL509: _temp508= _temp490.f2;
if(*(( int*) _temp508) == Cyc_CfFlowInfo_TuplePI){ _LL511: _temp510=(( struct
Cyc_CfFlowInfo_TuplePI_struct*) _temp508)->f1; goto _LL495;} else{ goto _LL496;}
_LL496: _LL521: _temp520= _temp490.f1; if(*(( int*) _temp520) == Cyc_CfFlowInfo_StructPI){
_LL523: _temp522=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp520)->f1; goto
_LL517;} else{ goto _LL498;} _LL517: _temp516= _temp490.f2; if(*(( int*)
_temp516) == Cyc_CfFlowInfo_StructPI){ _LL519: _temp518=(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp516)->f1; goto _LL497;} else{ goto _LL498;} _LL498: goto _LL499; _LL493:
if( _temp506 == _temp502){ return pinfo1;} return Cyc_CfFlowInfo_join_initstate(
env, _temp506, _temp502); _LL495: if( _temp514 == _temp510){ return pinfo1;}
return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct* _temp524=( struct Cyc_CfFlowInfo_TuplePI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct)); _temp524[ 0]=({
struct Cyc_CfFlowInfo_TuplePI_struct _temp525; _temp525.tag= Cyc_CfFlowInfo_TuplePI;
_temp525.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct Cyc_CfFlowInfo_JoinEnv*,
void*, void*), struct Cyc_CfFlowInfo_JoinEnv* env, struct Cyc_Dict_Dict* d1,
struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)( Cyc_CfFlowInfo_join_pathinfo,
env, _temp514, _temp510); _temp525;}); _temp524;}); _LL497: if( _temp522 ==
_temp518){ return pinfo1;} return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp526=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp526[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp527; _temp527.tag=
Cyc_CfFlowInfo_StructPI; _temp527.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)(
struct Cyc_CfFlowInfo_JoinEnv*, void*, void*), struct Cyc_CfFlowInfo_JoinEnv*
env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)(
Cyc_CfFlowInfo_join_pathinfo, env, _temp522, _temp518); _temp527;}); _temp526;});
_LL499:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp528=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp528[ 0]=({ struct Cyc_Core_Impossible_struct _temp529; _temp529.tag= Cyc_Core_Impossible;
_temp529.f1= _tag_arr("join_pathinfo", sizeof( unsigned char), 14u); _temp529;});
_temp528;})); _LL491:;}} void* Cyc_CfFlowInfo_join_flow( void* f1, void* f2){
if( f1 == f2){ return f1;}{ struct _tuple3 _temp531=({ struct _tuple3 _temp530;
_temp530.f1= f1; _temp530.f2= f2; _temp530;}); void* _temp539; void* _temp541;
void* _temp543; struct Cyc_Dict_Dict* _temp545; void* _temp547; struct Cyc_Dict_Dict*
_temp549; _LL533: _LL540: _temp539= _temp531.f1; if( _temp539 ==( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL534;} else{ goto _LL535;} _LL535: _LL542: _temp541= _temp531.f2; if(
_temp541 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL536;} else{ goto _LL537;}
_LL537: _LL548: _temp547= _temp531.f1; if(( unsigned int) _temp547 > 1u?*(( int*)
_temp547) == Cyc_CfFlowInfo_InitsFL: 0){ _LL550: _temp549=(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp547)->f1; goto _LL544;} else{ goto _LL532;} _LL544: _temp543= _temp531.f2;
if(( unsigned int) _temp543 > 1u?*(( int*) _temp543) == Cyc_CfFlowInfo_InitsFL:
0){ _LL546: _temp545=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp543)->f1;
goto _LL538;} else{ goto _LL532;} _LL534: return f2; _LL536: return f1; _LL538:
if( _temp549 == _temp545){ return f1;} if( Cyc_CfFlowInfo_flow_lessthan_approx(
f1, f2)){ return f2;} if( Cyc_CfFlowInfo_flow_lessthan_approx( f2, f1)){ return
f1;}{ struct _RegionHandle _temp551= _new_region(); struct _RegionHandle* r=&
_temp551; _push_region( r);{ struct Cyc_List_List* _temp552= 0; struct Cyc_CfFlowInfo_JoinEnv
_temp553=({ struct Cyc_CfFlowInfo_JoinEnv _temp558; _temp558.d1= _temp549;
_temp558.d2= _temp545; _temp558.places=& _temp552; _temp558.rgn= r; _temp558;});
struct Cyc_Dict_Dict* _temp554=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct
Cyc_CfFlowInfo_JoinEnv*, void*, void*), struct Cyc_CfFlowInfo_JoinEnv* env,
struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)( Cyc_CfFlowInfo_join_pathinfo,&
_temp553, _temp549, _temp545); void* _temp557=( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp555=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp555[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp556; _temp556.tag= Cyc_CfFlowInfo_InitsFL;
_temp556.f1= Cyc_CfFlowInfo_escape_these( r, _temp552, _temp554); _temp556;});
_temp555;}); _npop_handler( 0u); return _temp557;}; _pop_region( r);} _LL532:;}}
int Cyc_CfFlowInfo_initstate_lessthan_approx( void* state1, void* state2){
struct _tuple3 _temp560=({ struct _tuple3 _temp559; _temp559.f1= state1;
_temp559.f2= state2; _temp559;}); void* _temp568; void* _temp570; void* _temp572;
void* _temp574; void* _temp576; void* _temp578; void* _temp580; struct Cyc_CfFlowInfo_Place*
_temp582; void* _temp584; struct Cyc_CfFlowInfo_Place* _temp586; _LL562: _LL575:
_temp574= _temp560.f1; if(*(( int*) _temp574) == Cyc_CfFlowInfo_UnknownIS){
_LL579: _temp578=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp574)->f1;
goto _LL577; _LL577: _temp576=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp574)->f2; goto _LL569;} else{ goto _LL564;} _LL569: _temp568= _temp560.f2;
if(*(( int*) _temp568) == Cyc_CfFlowInfo_UnknownIS){ _LL573: _temp572=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp568)->f1; goto _LL571; _LL571:
_temp570=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp568)->f2; goto
_LL563;} else{ goto _LL564;} _LL564: _LL585: _temp584= _temp560.f1; if(*(( int*)
_temp584) == Cyc_CfFlowInfo_MustPointTo){ _LL587: _temp586=(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp584)->f1; goto _LL581;} else{ goto _LL566;} _LL581: _temp580= _temp560.f2;
if(*(( int*) _temp580) == Cyc_CfFlowInfo_MustPointTo){ _LL583: _temp582=((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp580)->f1; goto _LL565;} else{
goto _LL566;} _LL566: goto _LL567; _LL563: return Cyc_CfFlowInfo_join_escape(
_temp578, _temp572) == _temp572? Cyc_CfFlowInfo_join_initlevel( _temp576,
_temp570) == _temp570: 0; _LL565: return Cyc_CfFlowInfo_place_cmp( _temp586,
_temp582) == 0; _LL567: return 0; _LL561:;} int Cyc_CfFlowInfo_pinfo_lessthan_approx(
void* ignore, void* pinfo1, void* pinfo2){ if( pinfo1 == pinfo2){ return 1;}{
struct _tuple3 _temp589=({ struct _tuple3 _temp588; _temp588.f1= pinfo1;
_temp588.f2= pinfo2; _temp588;}); void* _temp599; void* _temp601; void* _temp603;
void* _temp605; void* _temp607; struct Cyc_Dict_Dict* _temp609; void* _temp611;
struct Cyc_Dict_Dict* _temp613; void* _temp615; struct Cyc_Dict_Dict* _temp617;
void* _temp619; struct Cyc_Dict_Dict* _temp621; _LL591: _LL604: _temp603=
_temp589.f1; if(*(( int*) _temp603) == Cyc_CfFlowInfo_LeafPI){ _LL606: _temp605=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp603)->f1; goto _LL600;}
else{ goto _LL593;} _LL600: _temp599= _temp589.f2; if(*(( int*) _temp599) == Cyc_CfFlowInfo_LeafPI){
_LL602: _temp601=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp599)->f1;
goto _LL592;} else{ goto _LL593;} _LL593: _LL612: _temp611= _temp589.f1; if(*((
int*) _temp611) == Cyc_CfFlowInfo_TuplePI){ _LL614: _temp613=(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp611)->f1; goto _LL608;} else{ goto _LL595;} _LL608: _temp607= _temp589.f2;
if(*(( int*) _temp607) == Cyc_CfFlowInfo_TuplePI){ _LL610: _temp609=(( struct
Cyc_CfFlowInfo_TuplePI_struct*) _temp607)->f1; goto _LL594;} else{ goto _LL595;}
_LL595: _LL620: _temp619= _temp589.f1; if(*(( int*) _temp619) == Cyc_CfFlowInfo_StructPI){
_LL622: _temp621=(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp619)->f1; goto
_LL616;} else{ goto _LL597;} _LL616: _temp615= _temp589.f2; if(*(( int*)
_temp615) == Cyc_CfFlowInfo_StructPI){ _LL618: _temp617=(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp615)->f1; goto _LL596;} else{ goto _LL597;} _LL597: goto _LL598; _LL592:
return _temp605 == _temp601? 1: Cyc_CfFlowInfo_initstate_lessthan_approx(
_temp605, _temp601); _LL594: return _temp613 == _temp609? 1:(( int(*)( int(* f)(
int, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_forall_intersect)((
int(*)( int ignore, void* pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx,
_temp613, _temp609); _LL596: return _temp621 == _temp617? 1:(( int(*)( int(* f)(
struct _tagged_arr*, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Dict_forall_intersect)(( int(*)( struct _tagged_arr* ignore, void*
pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx, _temp621, _temp617);
_LL598:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp623=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp623[ 0]=({ struct Cyc_Core_Impossible_struct _temp624; _temp624.tag= Cyc_Core_Impossible;
_temp624.f1= _tag_arr("pinfo_lessthan", sizeof( unsigned char), 15u); _temp624;});
_temp623;})); _LL590:;}} int Cyc_CfFlowInfo_flow_lessthan_approx( void* f1, void*
f2){ if( f1 == f2){ return 1;}{ struct _tuple3 _temp626=({ struct _tuple3
_temp625; _temp625.f1= f1; _temp625.f2= f2; _temp625;}); void* _temp634; void*
_temp636; void* _temp638; struct Cyc_Dict_Dict* _temp640; void* _temp642; struct
Cyc_Dict_Dict* _temp644; _LL628: _LL635: _temp634= _temp626.f1; if( _temp634 ==(
void*) Cyc_CfFlowInfo_BottomFL){ goto _LL629;} else{ goto _LL630;} _LL630:
_LL637: _temp636= _temp626.f2; if( _temp636 ==( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL631;} else{ goto _LL632;} _LL632: _LL643: _temp642= _temp626.f1; if((
unsigned int) _temp642 > 1u?*(( int*) _temp642) == Cyc_CfFlowInfo_InitsFL: 0){
_LL645: _temp644=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp642)->f1; goto
_LL639;} else{ goto _LL627;} _LL639: _temp638= _temp626.f2; if(( unsigned int)
_temp638 > 1u?*(( int*) _temp638) == Cyc_CfFlowInfo_InitsFL: 0){ _LL641:
_temp640=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp638)->f1; goto _LL633;}
else{ goto _LL627;} _LL629: return 1; _LL631: return 0; _LL633: if( _temp644 ==
_temp640){ return 1;} return Cyc_Dict_forall_intersect( Cyc_CfFlowInfo_pinfo_lessthan_approx,
_temp644, _temp640); _LL627:;}}
