 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_dirent{ int d_ino; int d_off; unsigned short
d_reclen; unsigned char d_type; unsigned char d_name[ 256u]; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern
unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern struct _RegionHandle* Cyc_Core_heap_region;
extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); extern int system( unsigned char*); extern int*
__errno(); struct Cyc_Stdio___sFILE; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr;
extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*); extern unsigned char
Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[ 18u];
struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; static const int Cyc_Stdio_String_pa= 0; struct Cyc_Stdio_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_Int_pa= 1; struct
Cyc_Stdio_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Stdio_Double_pa=
2; struct Cyc_Stdio_Double_pa_struct{ int tag; double f1; } ; static const int
Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{ int tag; short*
f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
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
} ; extern struct Cyc_List_List* Cyc_List_list( struct _tagged_arr); extern int
Cyc_List_length( struct Cyc_List_List* x); extern unsigned char Cyc_List_List_empty[
15u]; extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_rmap( struct _RegionHandle*, void*(* f)(
void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap_c(
struct _RegionHandle*, void*(* f)( void*, void*), void* env, struct Cyc_List_List*
x); extern unsigned char Cyc_List_List_mismatch[ 18u]; extern void Cyc_List_iter_c(
void(* f)( void*, void*), void* env, struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rrev(
struct _RegionHandle*, struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List*
Cyc_List_imp_append( struct Cyc_List_List* x, struct Cyc_List_List* y); extern
unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_List_forall( int(* pred)( void*),
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rfrom_array(
struct _RegionHandle* r2, struct _tagged_arr arr); extern int Cyc_String_zstrptrcmp(
struct _tagged_arr*, struct _tagged_arr*); extern struct _tagged_arr Cyc_String_strconcat(
struct _tagged_arr, struct _tagged_arr); struct Cyc_Set_Set; extern struct Cyc_Set_Set*
Cyc_Set_empty( int(* comp)( void*, void*)); extern struct Cyc_Set_Set* Cyc_Set_insert(
struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_member( struct Cyc_Set_Set*
s, void* elt); extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[
11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* comp)( void*, void*));
extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key,
void* data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key);
extern struct Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void*
key); struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct _tagged_arr
line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[ 9u];
struct Cyc_Position_Segment; extern struct _tagged_arr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*); static const int Cyc_Position_Lex= 0; static
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
15u]; extern int Cyc_Absyn_qvar_cmp( struct _tuple0*, struct _tuple0*); extern
struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value; extern void* Cyc_Absyn_rel_ns_null;
extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_conref_def( void*, struct Cyc_Absyn_Conref* x);
extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t;
extern void* Cyc_Absyn_exn_typ; extern void* Cyc_Absyn_bounds_one; extern void*
Cyc_Absyn_star_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern void*
Cyc_Absyn_cstar_typ( void* t, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_tagged_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_void_star_typ();
extern void* Cyc_Absyn_strct( struct _tagged_arr* name); extern void* Cyc_Absyn_strctq(
struct _tuple0* name); extern void* Cyc_Absyn_unionq_typ( struct _tuple0* name);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void*, int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_char_exp( unsigned char c, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp( struct _tagged_arr f, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp(
struct _tagged_arr s, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_var_exp( struct _tuple0*, struct Cyc_Position_Segment*); extern struct
Cyc_Absyn_Exp* Cyc_Absyn_varb_exp( struct _tuple0*, void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim1_exp( void*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim2_exp(
void*, struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_add_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_eq_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_lt_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Core_Opt*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_assign_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_and_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_fncall_exp( struct Cyc_Absyn_Exp*, struct
Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_address_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeoftyp_exp(
void* t, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_structmember_exp( struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_structarrow_exp(
struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_subscript_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_stmt_exp( struct Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_match_exn_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_unresolvedmem_exp( struct Cyc_Core_Opt*,
struct Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt( void* s, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt( struct Cyc_Position_Segment* loc); extern
struct Cyc_Absyn_Stmt* Cyc_Absyn_exp_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmt( struct Cyc_Absyn_Stmt*
s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmts( struct Cyc_List_List*, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_return_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_for_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3,
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_switch_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_List_List*, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_declare_stmt( struct _tuple0*,
void*, struct Cyc_Absyn_Exp* init, struct Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_label_stmt( struct _tagged_arr* v,
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_goto_stmt( struct _tagged_arr* lab, struct Cyc_Position_Segment* loc);
extern struct Cyc_Absyn_Stmt* Cyc_Absyn_assign_stmt( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Decl*
Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple0* x, void* t, struct Cyc_Absyn_Exp*
init); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_static_vardecl( struct _tuple0*
x, void* t, struct Cyc_Absyn_Exp* init); extern int Cyc_Absyn_is_lvalue( struct
Cyc_Absyn_Exp*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_field(
struct Cyc_List_List*, struct _tagged_arr*); struct Cyc_PP_Ppstate; struct Cyc_PP_Out;
struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int
qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int
rewrite_temp_tvars: 1; int print_all_tvars: 1; int print_all_kinds: 1; int
print_using_stmts: 1; int print_externC_stmts: 1; int print_full_evars: 1; int
use_curr_namespace: 1; struct Cyc_List_List* curr_namespace; } ; extern struct
_tagged_arr Cyc_Absynpp_typ2string( void*); extern struct _tagged_arr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*); extern struct _tagged_arr Cyc_Absynpp_stmt2string(
struct Cyc_Absyn_Stmt*); extern struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple0*); static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{
int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos(
struct _tagged_arr); extern void* Cyc_Tcutil_typ_kind( void* t); extern void*
Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_unify( void*, void*);
extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators( struct
_RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List* des,
struct Cyc_List_List* fields); extern int Cyc_Tcutil_is_tagged_pointer_typ( void*);
extern int Cyc_Tcutil_is_tagged_pointer_typ_elt( void* t, void** elt_typ_dest);
struct _tuple3{ struct Cyc_Absyn_Tqual f1; void* f2; } ; extern void* Cyc_Tcutil_snd_tqt(
struct _tuple3*); extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp*
e); extern struct Cyc_List_List* Cyc_Toc_toc( struct Cyc_List_List* ds); extern
struct _tuple0* Cyc_Toc_temp_var(); static struct Cyc_List_List* Cyc_Toc_result_decls=
0; static struct Cyc_List_List* Cyc_Toc_tuple_types= 0; static int Cyc_Toc_tuple_type_counter=
0; static int Cyc_Toc_temp_var_counter= 0; static int Cyc_Toc_fresh_label_counter=
0; unsigned char Cyc_Toc_Toc_Unimplemented[ 22u]="\000\000\000\000Toc_Unimplemented";
unsigned char Cyc_Toc_Toc_Impossible[ 19u]="\000\000\000\000Toc_Impossible";
static void* Cyc_Toc_unimp( struct _tagged_arr s){({ int(* _temp0)( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp4= Cyc_Stdio_stderr; struct _tagged_arr _temp5= _tag_arr("%s\n", sizeof(
unsigned char), 4u); struct Cyc_Stdio_String_pa_struct _temp6; _temp6.tag= Cyc_Stdio_String_pa;
_temp6.f1=( struct _tagged_arr) s;{ void* _temp3=( void*)& _temp6; void* _temp1[
1u]={ _temp3}; struct _tagged_arr _temp2={( void*) _temp1,( void*) _temp1,( void*)(
_temp1 + 1u)}; _temp0( _temp4, _temp5, _temp2);}}); Cyc_Stdio_fflush(( struct
Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( int) _throw(( void*) Cyc_Toc_Toc_Unimplemented);}
static void* Cyc_Toc_toc_impos( struct _tagged_arr s){({ int(* _temp7)( struct
Cyc_Stdio___sFILE*, struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf;
struct Cyc_Stdio___sFILE* _temp11= Cyc_Stdio_stderr; struct _tagged_arr _temp12=
_tag_arr("%s\n", sizeof( unsigned char), 4u); struct Cyc_Stdio_String_pa_struct
_temp13; _temp13.tag= Cyc_Stdio_String_pa; _temp13.f1=( struct _tagged_arr) s;{
void* _temp10=( void*)& _temp13; void* _temp8[ 1u]={ _temp10}; struct
_tagged_arr _temp9={( void*) _temp8,( void*) _temp8,( void*)( _temp8 + 1u)};
_temp7( _temp11, _temp12, _temp9);}}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr);( int) _throw(( void*) Cyc_Toc_Toc_Impossible);} unsigned char
Cyc_Toc_Match_error[ 16u]="\000\000\000\000Match_error"; static unsigned char
_temp14[ 5u]="curr"; static struct _tagged_arr Cyc_Toc_curr_string={ _temp14,
_temp14, _temp14 + 5u}; static struct _tagged_arr* Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;
static unsigned char _temp15[ 4u]="tag"; static struct _tagged_arr Cyc_Toc_tag_string={
_temp15, _temp15, _temp15 + 4u}; static struct _tagged_arr* Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;
static unsigned char _temp16[ 14u]="_handler_cons"; static struct _tagged_arr
Cyc_Toc__handler_cons_string={ _temp16, _temp16, _temp16 + 14u}; static struct
_tagged_arr* Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string; static
unsigned char _temp17[ 8u]="handler"; static struct _tagged_arr Cyc_Toc_handler_string={
_temp17, _temp17, _temp17 + 8u}; static struct _tagged_arr* Cyc_Toc_handler_sp=&
Cyc_Toc_handler_string; static unsigned char _temp18[ 14u]="_RegionHandle";
static struct _tagged_arr Cyc_Toc__RegionHandle_string={ _temp18, _temp18,
_temp18 + 14u}; static struct _tagged_arr* Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;
static unsigned char _temp19[ 7u]="_throw"; static struct _tagged_arr Cyc_Toc__throw_str={
_temp19, _temp19, _temp19 + 7u}; static struct _tuple0 Cyc_Toc__throw_pr={( void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__throw_str}; static struct _tuple0* Cyc_Toc__throw_qv=&
Cyc_Toc__throw_pr; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_re={ 1u,&
Cyc_Toc__throw_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={
0,( void*)(( void*)& Cyc_Toc__throw_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__throw_e=&
Cyc_Toc__throw_ev; static unsigned char _temp21[ 7u]="setjmp"; static struct
_tagged_arr Cyc_Toc_setjmp_str={ _temp21, _temp21, _temp21 + 7u}; static struct
_tuple0 Cyc_Toc_setjmp_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc_setjmp_str};
static struct _tuple0* Cyc_Toc_setjmp_qv=& Cyc_Toc_setjmp_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc_setjmp_re={ 1u,& Cyc_Toc_setjmp_pr,( void*)(( void*) 0u)}; static struct
Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={ 0,( void*)(( void*)& Cyc_Toc_setjmp_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev; static
unsigned char _temp23[ 14u]="_push_handler"; static struct _tagged_arr Cyc_Toc__push_handler_str={
_temp23, _temp23, _temp23 + 14u}; static struct _tuple0 Cyc_Toc__push_handler_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__push_handler_str}; static struct
_tuple0* Cyc_Toc__push_handler_qv=& Cyc_Toc__push_handler_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_handler_re={ 1u,& Cyc_Toc__push_handler_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={ 0,( void*)(( void*)& Cyc_Toc__push_handler_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;
static unsigned char _temp25[ 13u]="_pop_handler"; static struct _tagged_arr Cyc_Toc__pop_handler_str={
_temp25, _temp25, _temp25 + 13u}; static struct _tuple0 Cyc_Toc__pop_handler_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__pop_handler_str}; static struct
_tuple0* Cyc_Toc__pop_handler_qv=& Cyc_Toc__pop_handler_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__pop_handler_re={ 1u,& Cyc_Toc__pop_handler_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={ 0,( void*)(( void*)& Cyc_Toc__pop_handler_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;
static unsigned char _temp27[ 12u]="_exn_thrown"; static struct _tagged_arr Cyc_Toc__exn_thrown_str={
_temp27, _temp27, _temp27 + 12u}; static struct _tuple0 Cyc_Toc__exn_thrown_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__exn_thrown_str}; static struct
_tuple0* Cyc_Toc__exn_thrown_qv=& Cyc_Toc__exn_thrown_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__exn_thrown_re={ 1u,& Cyc_Toc__exn_thrown_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={ 0,( void*)(( void*)& Cyc_Toc__exn_thrown_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;
static unsigned char _temp29[ 14u]="_npop_handler"; static struct _tagged_arr
Cyc_Toc__npop_handler_str={ _temp29, _temp29, _temp29 + 14u}; static struct
_tuple0 Cyc_Toc__npop_handler_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__npop_handler_str};
static struct _tuple0* Cyc_Toc__npop_handler_qv=& Cyc_Toc__npop_handler_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__npop_handler_re={ 1u,& Cyc_Toc__npop_handler_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={ 0,(
void*)(( void*)& Cyc_Toc__npop_handler_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__npop_handler_e=&
Cyc_Toc__npop_handler_ev; static unsigned char _temp31[ 12u]="_check_null";
static struct _tagged_arr Cyc_Toc__check_null_str={ _temp31, _temp31, _temp31 +
12u}; static struct _tuple0 Cyc_Toc__check_null_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_null_str}; static struct _tuple0* Cyc_Toc__check_null_qv=& Cyc_Toc__check_null_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_null_re={ 1u,& Cyc_Toc__check_null_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={ 0,(
void*)(( void*)& Cyc_Toc__check_null_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__check_null_e=&
Cyc_Toc__check_null_ev; static unsigned char _temp33[ 28u]="_check_known_subscript_null";
static struct _tagged_arr Cyc_Toc__check_known_subscript_null_str={ _temp33,
_temp33, _temp33 + 28u}; static struct _tuple0 Cyc_Toc__check_known_subscript_null_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_known_subscript_null_str};
static struct _tuple0* Cyc_Toc__check_known_subscript_null_qv=& Cyc_Toc__check_known_subscript_null_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_null_re={ 1u,&
Cyc_Toc__check_known_subscript_null_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__check_known_subscript_null_ev={ 0,( void*)(( void*)& Cyc_Toc__check_known_subscript_null_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static unsigned char _temp35[ 31u]="_check_known_subscript_notnull"; static
struct _tagged_arr Cyc_Toc__check_known_subscript_notnull_str={ _temp35, _temp35,
_temp35 + 31u}; static struct _tuple0 Cyc_Toc__check_known_subscript_notnull_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_known_subscript_notnull_str};
static struct _tuple0* Cyc_Toc__check_known_subscript_notnull_qv=& Cyc_Toc__check_known_subscript_notnull_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_notnull_re={
1u,& Cyc_Toc__check_known_subscript_notnull_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={ 0,( void*)((
void*)& Cyc_Toc__check_known_subscript_notnull_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;
static unsigned char _temp37[ 25u]="_check_unknown_subscript"; static struct
_tagged_arr Cyc_Toc__check_unknown_subscript_str={ _temp37, _temp37, _temp37 +
25u}; static struct _tuple0 Cyc_Toc__check_unknown_subscript_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_unknown_subscript_str}; static struct _tuple0* Cyc_Toc__check_unknown_subscript_qv=&
Cyc_Toc__check_unknown_subscript_pr; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_unknown_subscript_re={
1u,& Cyc_Toc__check_unknown_subscript_pr,( void*)(( void*) 0u)}; static struct
Cyc_Absyn_Exp Cyc_Toc__check_unknown_subscript_ev={ 0,( void*)(( void*)& Cyc_Toc__check_unknown_subscript_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__check_unknown_subscript_e=& Cyc_Toc__check_unknown_subscript_ev;
static unsigned char _temp39[ 12u]="_tagged_arr"; static struct _tagged_arr Cyc_Toc__tagged_arr_str={
_temp39, _temp39, _temp39 + 12u}; static struct _tuple0 Cyc_Toc__tagged_arr_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_str}; static struct
_tuple0* Cyc_Toc__tagged_arr_qv=& Cyc_Toc__tagged_arr_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__tagged_arr_re={ 1u,& Cyc_Toc__tagged_arr_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_ev={ 0,( void*)(( void*)& Cyc_Toc__tagged_arr_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tagged_arr_e=& Cyc_Toc__tagged_arr_ev;
static unsigned char _temp41[ 9u]="_tag_arr"; static struct _tagged_arr Cyc_Toc__tag_arr_str={
_temp41, _temp41, _temp41 + 9u}; static struct _tuple0 Cyc_Toc__tag_arr_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tag_arr_str}; static struct
_tuple0* Cyc_Toc__tag_arr_qv=& Cyc_Toc__tag_arr_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__tag_arr_re={ 1u,& Cyc_Toc__tag_arr_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc__tag_arr_ev={ 0,( void*)(( void*)& Cyc_Toc__tag_arr_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tag_arr_e=& Cyc_Toc__tag_arr_ev;
static unsigned char _temp43[ 14u]="_init_tag_arr"; static struct _tagged_arr
Cyc_Toc__init_tag_arr_str={ _temp43, _temp43, _temp43 + 14u}; static struct
_tuple0 Cyc_Toc__init_tag_arr_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__init_tag_arr_str};
static struct _tuple0* Cyc_Toc__init_tag_arr_qv=& Cyc_Toc__init_tag_arr_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__init_tag_arr_re={ 1u,& Cyc_Toc__init_tag_arr_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__init_tag_arr_ev={ 0,(
void*)(( void*)& Cyc_Toc__init_tag_arr_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__init_tag_arr_e=&
Cyc_Toc__init_tag_arr_ev; static unsigned char _temp45[ 11u]="_untag_arr";
static struct _tagged_arr Cyc_Toc__untag_arr_str={ _temp45, _temp45, _temp45 +
11u}; static struct _tuple0 Cyc_Toc__untag_arr_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__untag_arr_str}; static struct _tuple0* Cyc_Toc__untag_arr_qv=& Cyc_Toc__untag_arr_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_arr_re={ 1u,& Cyc_Toc__untag_arr_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__untag_arr_ev={ 0,(
void*)(( void*)& Cyc_Toc__untag_arr_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__untag_arr_e=&
Cyc_Toc__untag_arr_ev; static unsigned char _temp47[ 14u]="_get_arr_size";
static struct _tagged_arr Cyc_Toc__get_arr_size_str={ _temp47, _temp47, _temp47
+ 14u}; static struct _tuple0 Cyc_Toc__get_arr_size_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__get_arr_size_str}; static struct _tuple0* Cyc_Toc__get_arr_size_qv=&
Cyc_Toc__get_arr_size_pr; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_arr_size_re={
1u,& Cyc_Toc__get_arr_size_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__get_arr_size_ev={ 0,( void*)(( void*)& Cyc_Toc__get_arr_size_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__get_arr_size_e=& Cyc_Toc__get_arr_size_ev;
static unsigned char _temp49[ 17u]="_tagged_arr_plus"; static struct _tagged_arr
Cyc_Toc__tagged_arr_plus_str={ _temp49, _temp49, _temp49 + 17u}; static struct
_tuple0 Cyc_Toc__tagged_arr_plus_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_plus_str};
static struct _tuple0* Cyc_Toc__tagged_arr_plus_qv=& Cyc_Toc__tagged_arr_plus_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_plus_re={ 1u,& Cyc_Toc__tagged_arr_plus_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_plus_ev={
0,( void*)(( void*)& Cyc_Toc__tagged_arr_plus_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__tagged_arr_plus_e=& Cyc_Toc__tagged_arr_plus_ev; static unsigned char
_temp51[ 25u]="_tagged_arr_inplace_plus"; static struct _tagged_arr Cyc_Toc__tagged_arr_inplace_plus_str={
_temp51, _temp51, _temp51 + 25u}; static struct _tuple0 Cyc_Toc__tagged_arr_inplace_plus_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_inplace_plus_str};
static struct _tuple0* Cyc_Toc__tagged_arr_inplace_plus_qv=& Cyc_Toc__tagged_arr_inplace_plus_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_re={ 1u,&
Cyc_Toc__tagged_arr_inplace_plus_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__tagged_arr_inplace_plus_ev={ 0,( void*)(( void*)& Cyc_Toc__tagged_arr_inplace_plus_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tagged_arr_inplace_plus_e=& Cyc_Toc__tagged_arr_inplace_plus_ev;
static unsigned char _temp53[ 30u]="_tagged_arr_inplace_plus_post"; static
struct _tagged_arr Cyc_Toc__tagged_arr_inplace_plus_post_str={ _temp53, _temp53,
_temp53 + 30u}; static struct _tuple0 Cyc_Toc__tagged_arr_inplace_plus_post_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_inplace_plus_post_str};
static struct _tuple0* Cyc_Toc__tagged_arr_inplace_plus_post_qv=& Cyc_Toc__tagged_arr_inplace_plus_post_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_post_re={
1u,& Cyc_Toc__tagged_arr_inplace_plus_post_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_inplace_plus_post_ev={ 0,( void*)((
void*)& Cyc_Toc__tagged_arr_inplace_plus_post_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__tagged_arr_inplace_plus_post_e=& Cyc_Toc__tagged_arr_inplace_plus_post_ev;
static unsigned char _temp55[ 10u]="GC_malloc"; static struct _tagged_arr Cyc_Toc_GC_malloc_str={
_temp55, _temp55, _temp55 + 10u}; static struct _tuple0 Cyc_Toc_GC_malloc_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc_GC_malloc_str}; static struct
_tuple0* Cyc_Toc_GC_malloc_qv=& Cyc_Toc_GC_malloc_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc_GC_malloc_re={ 1u,& Cyc_Toc_GC_malloc_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc_GC_malloc_ev={ 0,( void*)(( void*)& Cyc_Toc_GC_malloc_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc_GC_malloc_e=& Cyc_Toc_GC_malloc_ev;
static unsigned char _temp57[ 17u]="GC_malloc_atomic"; static struct _tagged_arr
Cyc_Toc_GC_malloc_atomic_str={ _temp57, _temp57, _temp57 + 17u}; static struct
_tuple0 Cyc_Toc_GC_malloc_atomic_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc_GC_malloc_atomic_str};
static struct _tuple0* Cyc_Toc_GC_malloc_atomic_qv=& Cyc_Toc_GC_malloc_atomic_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc_GC_malloc_atomic_re={ 1u,& Cyc_Toc_GC_malloc_atomic_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc_GC_malloc_atomic_ev={
0,( void*)(( void*)& Cyc_Toc_GC_malloc_atomic_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc_GC_malloc_atomic_e=& Cyc_Toc_GC_malloc_atomic_ev; static unsigned char
_temp59[ 15u]="_region_malloc"; static struct _tagged_arr Cyc_Toc__region_malloc_str={
_temp59, _temp59, _temp59 + 15u}; static struct _tuple0 Cyc_Toc__region_malloc_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__region_malloc_str}; static struct
_tuple0* Cyc_Toc__region_malloc_qv=& Cyc_Toc__region_malloc_pr; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__region_malloc_re={ 1u,& Cyc_Toc__region_malloc_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={ 0,(
void*)(( void*)& Cyc_Toc__region_malloc_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev; static unsigned char
_temp61[ 12u]="_new_region"; static struct _tagged_arr Cyc_Toc__new_region_str={
_temp61, _temp61, _temp61 + 12u}; static struct _tuple0 Cyc_Toc__new_region_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__new_region_str}; static struct
_tuple0* Cyc_Toc__new_region_qv=& Cyc_Toc__new_region_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__new_region_re={ 1u,& Cyc_Toc__new_region_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={ 0,( void*)(( void*)& Cyc_Toc__new_region_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;
static unsigned char _temp63[ 13u]="_push_region"; static struct _tagged_arr Cyc_Toc__push_region_str={
_temp63, _temp63, _temp63 + 13u}; static struct _tuple0 Cyc_Toc__push_region_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__push_region_str}; static struct
_tuple0* Cyc_Toc__push_region_qv=& Cyc_Toc__push_region_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_region_re={ 1u,& Cyc_Toc__push_region_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={ 0,( void*)(( void*)& Cyc_Toc__push_region_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;
static unsigned char _temp65[ 12u]="_pop_region"; static struct _tagged_arr Cyc_Toc__pop_region_str={
_temp65, _temp65, _temp65 + 12u}; static struct _tuple0 Cyc_Toc__pop_region_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__pop_region_str}; static struct
_tuple0* Cyc_Toc__pop_region_qv=& Cyc_Toc__pop_region_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__pop_region_re={ 1u,& Cyc_Toc__pop_region_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={ 0,( void*)(( void*)& Cyc_Toc__pop_region_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;
static unsigned char _temp67[ 15u]="_sfile_to_file"; static struct _tagged_arr
Cyc_Toc__sfile_to_file_str={ _temp67, _temp67, _temp67 + 15u}; static struct
_tuple0 Cyc_Toc__sfile_to_file_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__sfile_to_file_str};
static struct _tuple0* Cyc_Toc__sfile_to_file_qv=& Cyc_Toc__sfile_to_file_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__sfile_to_file_re={ 1u,& Cyc_Toc__sfile_to_file_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__sfile_to_file_ev={ 0,(
void*)(( void*)& Cyc_Toc__sfile_to_file_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__sfile_to_file_e=& Cyc_Toc__sfile_to_file_ev; static struct Cyc_Absyn_StructType_struct
Cyc_Toc_tagged_arr_typ_v={ 9u,( struct _tuple0*)& Cyc_Toc__tagged_arr_pr, 0, 0};
static void* Cyc_Toc_tagged_arr_typ=( void*)& Cyc_Toc_tagged_arr_typ_v; static
struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={ 0, 0, 0}; static unsigned char _temp70[ 3u]="f0";
static struct _tagged_arr Cyc_Toc_f0={ _temp70, _temp70, _temp70 + 3u}; static
struct _tagged_arr* Cyc_Toc_field_names_v[ 1u]={& Cyc_Toc_f0}; static struct
_tagged_arr Cyc_Toc_field_names={( void*)(( struct _tagged_arr**) Cyc_Toc_field_names_v),(
void*)(( struct _tagged_arr**) Cyc_Toc_field_names_v),( void*)(( struct
_tagged_arr**) Cyc_Toc_field_names_v) + 1u}; static struct _tagged_arr* Cyc_Toc_fieldname(
int i){ unsigned int fsz= _get_arr_size( Cyc_Toc_field_names, sizeof( struct
_tagged_arr*)); if( i >= fsz){ Cyc_Toc_field_names=({ unsigned int _temp71=(
unsigned int)( i + 1); struct _tagged_arr** _temp72=( struct _tagged_arr**)
GC_malloc( sizeof( struct _tagged_arr*) * _temp71); struct _tagged_arr _temp81=
_tag_arr( _temp72, sizeof( struct _tagged_arr*),( unsigned int)( i + 1));{
unsigned int _temp73= _temp71; unsigned int j; for( j= 0; j < _temp73; j ++){
_temp72[ j]= j < fsz?*(( struct _tagged_arr**) _check_unknown_subscript( Cyc_Toc_field_names,
sizeof( struct _tagged_arr*),( int) j)):({ struct _tagged_arr* _temp74=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp74[ 0]=( struct
_tagged_arr)({ struct _tagged_arr(* _temp75)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp79= _tag_arr("f%d",
sizeof( unsigned char), 4u); struct Cyc_Stdio_Int_pa_struct _temp80; _temp80.tag=
Cyc_Stdio_Int_pa; _temp80.f1=( int) j;{ void* _temp78=( void*)& _temp80; void*
_temp76[ 1u]={ _temp78}; struct _tagged_arr _temp77={( void*) _temp76,( void*)
_temp76,( void*)( _temp76 + 1u)}; _temp75( _temp79, _temp77);}}); _temp74;});}};
_temp81;});} return*(( struct _tagged_arr**) _check_unknown_subscript( Cyc_Toc_field_names,
sizeof( struct _tagged_arr*), i));} struct _tuple4{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; static int Cyc_Toc_is_zero( struct Cyc_Absyn_Exp*
e){ void* _temp82=( void*) e->r; void* _temp108; unsigned char _temp110; void*
_temp112; short _temp114; void* _temp116; int _temp118; void* _temp120;
long long _temp122; void* _temp124; struct Cyc_Absyn_Exp* _temp126; struct Cyc_List_List*
_temp128; struct Cyc_List_List* _temp130; struct Cyc_List_List* _temp132; struct
Cyc_List_List* _temp134; struct Cyc_List_List* _temp136; _LL84: if(*(( int*)
_temp82) == Cyc_Absyn_Const_e){ _LL109: _temp108=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp82)->f1; if(( unsigned int) _temp108 > 1u?*(( int*) _temp108) == Cyc_Absyn_Char_c:
0){ _LL111: _temp110=(( struct Cyc_Absyn_Char_c_struct*) _temp108)->f2; goto
_LL85;} else{ goto _LL86;}} else{ goto _LL86;} _LL86: if(*(( int*) _temp82) ==
Cyc_Absyn_Const_e){ _LL113: _temp112=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp82)->f1; if(( unsigned int) _temp112 > 1u?*(( int*) _temp112) == Cyc_Absyn_Short_c:
0){ _LL115: _temp114=(( struct Cyc_Absyn_Short_c_struct*) _temp112)->f2; goto
_LL87;} else{ goto _LL88;}} else{ goto _LL88;} _LL88: if(*(( int*) _temp82) ==
Cyc_Absyn_Const_e){ _LL117: _temp116=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp82)->f1; if(( unsigned int) _temp116 > 1u?*(( int*) _temp116) == Cyc_Absyn_Int_c:
0){ _LL119: _temp118=(( struct Cyc_Absyn_Int_c_struct*) _temp116)->f2; goto
_LL89;} else{ goto _LL90;}} else{ goto _LL90;} _LL90: if(*(( int*) _temp82) ==
Cyc_Absyn_Const_e){ _LL121: _temp120=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp82)->f1; if(( unsigned int) _temp120 > 1u?*(( int*) _temp120) == Cyc_Absyn_LongLong_c:
0){ _LL123: _temp122=(( struct Cyc_Absyn_LongLong_c_struct*) _temp120)->f2; goto
_LL91;} else{ goto _LL92;}} else{ goto _LL92;} _LL92: if(*(( int*) _temp82) ==
Cyc_Absyn_Const_e){ _LL125: _temp124=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp82)->f1; if( _temp124 ==( void*) Cyc_Absyn_Null_c){ goto _LL93;} else{ goto
_LL94;}} else{ goto _LL94;} _LL94: if(*(( int*) _temp82) == Cyc_Absyn_Cast_e){
_LL127: _temp126=(( struct Cyc_Absyn_Cast_e_struct*) _temp82)->f2; goto _LL95;}
else{ goto _LL96;} _LL96: if(*(( int*) _temp82) == Cyc_Absyn_Tuple_e){ _LL129:
_temp128=(( struct Cyc_Absyn_Tuple_e_struct*) _temp82)->f1; goto _LL97;} else{
goto _LL98;} _LL98: if(*(( int*) _temp82) == Cyc_Absyn_Array_e){ _LL131:
_temp130=(( struct Cyc_Absyn_Array_e_struct*) _temp82)->f1; goto _LL99;} else{
goto _LL100;} _LL100: if(*(( int*) _temp82) == Cyc_Absyn_Struct_e){ _LL133:
_temp132=(( struct Cyc_Absyn_Struct_e_struct*) _temp82)->f3; goto _LL101;} else{
goto _LL102;} _LL102: if(*(( int*) _temp82) == Cyc_Absyn_CompoundLit_e){ _LL135:
_temp134=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp82)->f2; goto _LL103;}
else{ goto _LL104;} _LL104: if(*(( int*) _temp82) == Cyc_Absyn_UnresolvedMem_e){
_LL137: _temp136=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp82)->f2; goto
_LL105;} else{ goto _LL106;} _LL106: goto _LL107; _LL85: return _temp110 =='\000';
_LL87: return _temp114 == 0; _LL89: return _temp118 == 0; _LL91: return _temp122
== 0; _LL93: return 1; _LL95: return Cyc_Toc_is_zero( _temp126); _LL97: return((
int(*)( int(* pred)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_forall)(
Cyc_Toc_is_zero, _temp128); _LL99: _temp132= _temp130; goto _LL101; _LL101:
_temp134= _temp132; goto _LL103; _LL103: _temp136= _temp134; goto _LL105; _LL105:
for( 0; _temp136 != 0; _temp136=(( struct Cyc_List_List*) _check_null( _temp136))->tl){
if( ! Cyc_Toc_is_zero((*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp136))->hd)).f2)){ return 0;}} return 1; _LL107: return 0; _LL83:;} static
int Cyc_Toc_is_nullable( void* t){ void* _temp138= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp144; struct Cyc_Absyn_Conref* _temp146; _LL140:
if(( unsigned int) _temp138 > 4u?*(( int*) _temp138) == Cyc_Absyn_PointerType: 0){
_LL145: _temp144=(( struct Cyc_Absyn_PointerType_struct*) _temp138)->f1; _LL147:
_temp146= _temp144.nullable; goto _LL141;} else{ goto _LL142;} _LL142: goto
_LL143; _LL141: return(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
0, _temp146); _LL143:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("is_nullable", sizeof( unsigned char), 12u)); return 0; _LL139:;}
static struct _tuple0* Cyc_Toc_collapse_qvar_tag( struct _tuple0* x, struct
_tagged_arr tag){ return({ struct _tuple0* _temp148=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp148->f1=(* x).f1; _temp148->f2=({ struct
_tagged_arr* _temp149=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp149[ 0]=( struct _tagged_arr) Cyc_String_strconcat(*(* x).f2,
tag); _temp149;}); _temp148;});} struct _tuple5{ struct _tagged_arr* f1; struct
Cyc_List_List* f2; } ; static struct _tagged_arr* Cyc_Toc_add_tuple_type( struct
Cyc_List_List* tqs0){{ struct Cyc_List_List* _temp150= Cyc_Toc_tuple_types; for(
0; _temp150 != 0; _temp150=(( struct Cyc_List_List*) _check_null( _temp150))->tl){
struct _tuple5 _temp153; struct Cyc_List_List* _temp154; struct _tagged_arr*
_temp156; struct _tuple5* _temp151=( struct _tuple5*)(( struct Cyc_List_List*)
_check_null( _temp150))->hd; _temp153=* _temp151; _LL157: _temp156= _temp153.f1;
goto _LL155; _LL155: _temp154= _temp153.f2; goto _LL152; _LL152: { struct Cyc_List_List*
_temp158= tqs0; for( 0; _temp158 != 0? _temp154 != 0: 0;( _temp158=(( struct Cyc_List_List*)
_check_null( _temp158))->tl, _temp154=(( struct Cyc_List_List*) _check_null(
_temp154))->tl)){ if( ! Cyc_Tcutil_unify((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp158))->hd)).f2,( void*)(( struct Cyc_List_List*) _check_null(
_temp154))->hd)){ break;}} if( _temp158 == 0? _temp154 == 0: 0){ return _temp156;}}}}{
struct _tagged_arr* x=({ struct _tagged_arr* _temp172=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp172[ 0]=( struct _tagged_arr)({
struct _tagged_arr(* _temp173)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp177= _tag_arr("_tuple%d", sizeof( unsigned char), 9u);
struct Cyc_Stdio_Int_pa_struct _temp178; _temp178.tag= Cyc_Stdio_Int_pa;
_temp178.f1=( int)(( unsigned int) Cyc_Toc_tuple_type_counter ++);{ void*
_temp176=( void*)& _temp178; void* _temp174[ 1u]={ _temp176}; struct _tagged_arr
_temp175={( void*) _temp174,( void*) _temp174,( void*)( _temp174 + 1u)};
_temp173( _temp177, _temp175);}}); _temp172;}); struct Cyc_List_List* ts=((
struct Cyc_List_List*(*)( void*(* f)( struct _tuple3*), struct Cyc_List_List* x))
Cyc_List_map)( Cyc_Tcutil_snd_tqt, tqs0); struct Cyc_List_List* _temp159= 0;
struct Cyc_List_List* ts2= ts;{ int i= 1; for( 0; ts2 != 0;( ts2=(( struct Cyc_List_List*)
_check_null( ts2))->tl, i ++)){ _temp159=({ struct Cyc_List_List* _temp160=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp160->hd=(
void*)({ struct Cyc_Absyn_Structfield* _temp161=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp161->name= Cyc_Toc_fieldname(
i); _temp161->tq= Cyc_Toc_mt_tq; _temp161->type=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( ts2))->hd); _temp161->width= 0; _temp161->attributes= 0; _temp161;});
_temp160->tl= _temp159; _temp160;});}} _temp159=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp159);{ struct Cyc_Absyn_Structdecl*
_temp162=({ struct Cyc_Absyn_Structdecl* _temp168=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp168->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp168->name=({ struct Cyc_Core_Opt* _temp170=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp170->v=( void*)({ struct _tuple0*
_temp171=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp171->f1=
Cyc_Absyn_rel_ns_null; _temp171->f2= x; _temp171;}); _temp170;}); _temp168->tvs=
0; _temp168->fields=({ struct Cyc_Core_Opt* _temp169=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp169->v=( void*) _temp159;
_temp169;}); _temp168->attributes= 0; _temp168;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp163=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp163->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp164=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp164[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp165; _temp165.tag= Cyc_Absyn_Struct_d; _temp165.f1=
_temp162; _temp165;}); _temp164;}), 0); _temp163->tl= Cyc_Toc_result_decls;
_temp163;}); Cyc_Toc_tuple_types=({ struct Cyc_List_List* _temp166=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp166->hd=( void*)({ struct
_tuple5* _temp167=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp167->f1= x; _temp167->f2= ts; _temp167;}); _temp166->tl= Cyc_Toc_tuple_types;
_temp166;}); return x;}}} struct _tuple0* Cyc_Toc_temp_var(){ return({ struct
_tuple0* _temp179=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp179->f1=( void*) Cyc_Absyn_Loc_n; _temp179->f2=({ struct _tagged_arr*
_temp180=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr));
_temp180[ 0]=( struct _tagged_arr)({ struct _tagged_arr(* _temp181)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp185= _tag_arr("_temp%d", sizeof( unsigned char), 8u); struct Cyc_Stdio_Int_pa_struct
_temp186; _temp186.tag= Cyc_Stdio_Int_pa; _temp186.f1=( int)(( unsigned int) Cyc_Toc_temp_var_counter
++);{ void* _temp184=( void*)& _temp186; void* _temp182[ 1u]={ _temp184}; struct
_tagged_arr _temp183={( void*) _temp182,( void*) _temp182,( void*)( _temp182 + 1u)};
_temp181( _temp185, _temp183);}}); _temp180;}); _temp179;});} static struct
_tagged_arr* Cyc_Toc_fresh_label(){ return({ struct _tagged_arr* _temp187=(
struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp187[ 0]=(
struct _tagged_arr)({ struct _tagged_arr(* _temp188)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp192= _tag_arr("_LL%d",
sizeof( unsigned char), 6u); struct Cyc_Stdio_Int_pa_struct _temp193; _temp193.tag=
Cyc_Stdio_Int_pa; _temp193.f1=( int)(( unsigned int) Cyc_Toc_temp_var_counter ++);{
void* _temp191=( void*)& _temp193; void* _temp189[ 1u]={ _temp191}; struct
_tagged_arr _temp190={( void*) _temp189,( void*) _temp189,( void*)( _temp189 + 1u)};
_temp188( _temp192, _temp190);}}); _temp187;});} static void* Cyc_Toc_typ_to_c(
void* t); static struct _tuple1* Cyc_Toc_arg_to_c( struct _tuple1* a){ void*
_temp196; struct Cyc_Absyn_Tqual _temp198; struct Cyc_Core_Opt* _temp200; struct
_tuple1 _temp194=* a; _LL201: _temp200= _temp194.f1; goto _LL199; _LL199:
_temp198= _temp194.f2; goto _LL197; _LL197: _temp196= _temp194.f3; goto _LL195;
_LL195: return({ struct _tuple1* _temp202=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp202->f1= _temp200; _temp202->f2= _temp198; _temp202->f3=
Cyc_Toc_typ_to_c( _temp196); _temp202;});} static struct _tuple3* Cyc_Toc_typ_to_c_f(
struct _tuple3* x){ void* _temp205; struct Cyc_Absyn_Tqual _temp207; struct
_tuple3 _temp203=* x; _LL208: _temp207= _temp203.f1; goto _LL206; _LL206:
_temp205= _temp203.f2; goto _LL204; _LL204: return({ struct _tuple3* _temp209=(
struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp209->f1= _temp207;
_temp209->f2= Cyc_Toc_typ_to_c( _temp205); _temp209;});} static void* Cyc_Toc_typ_to_c_array(
void* t){ void* _temp210= t; struct Cyc_Absyn_Exp* _temp218; struct Cyc_Absyn_Tqual
_temp220; void* _temp222; struct Cyc_Core_Opt* _temp224; struct Cyc_Core_Opt
_temp226; void* _temp227; _LL212: if(( unsigned int) _temp210 > 4u?*(( int*)
_temp210) == Cyc_Absyn_ArrayType: 0){ _LL223: _temp222=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp210)->f1; goto _LL221; _LL221: _temp220=(( struct Cyc_Absyn_ArrayType_struct*)
_temp210)->f2; goto _LL219; _LL219: _temp218=(( struct Cyc_Absyn_ArrayType_struct*)
_temp210)->f3; goto _LL213;} else{ goto _LL214;} _LL214: if(( unsigned int)
_temp210 > 4u?*(( int*) _temp210) == Cyc_Absyn_Evar: 0){ _LL225: _temp224=((
struct Cyc_Absyn_Evar_struct*) _temp210)->f2; if( _temp224 == 0){ goto _LL216;}
else{ _temp226=* _temp224; _LL228: _temp227=( void*) _temp226.v; goto _LL215;}}
else{ goto _LL216;} _LL216: goto _LL217; _LL213: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp229=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp229[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp230; _temp230.tag= Cyc_Absyn_ArrayType;
_temp230.f1=( void*) Cyc_Toc_typ_to_c_array( _temp222); _temp230.f2= _temp220;
_temp230.f3= _temp218; _temp230;}); _temp229;}); _LL215: return Cyc_Toc_typ_to_c_array(
_temp227); _LL217: return Cyc_Toc_typ_to_c( t); _LL211:;} static struct Cyc_Absyn_Structfield*
Cyc_Toc_structfield_to_c( struct Cyc_Absyn_Structfield* f){ return({ struct Cyc_Absyn_Structfield*
_temp231=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp231->name= f->name; _temp231->tq= f->tq; _temp231->type=( void*) Cyc_Toc_typ_to_c((
void*) f->type); _temp231->width= f->width; _temp231->attributes= f->attributes;
_temp231;});} static void* Cyc_Toc_typ_to_c( void* t){ void* _temp232= t; struct
Cyc_Core_Opt* _temp284; struct Cyc_Core_Opt* _temp286; struct Cyc_Core_Opt
_temp288; void* _temp289; struct Cyc_Absyn_TunionInfo _temp291; void* _temp293;
struct Cyc_Absyn_Tuniondecl* _temp295; struct Cyc_Absyn_TunionFieldInfo _temp297;
void* _temp299; struct Cyc_Absyn_Tunionfield* _temp301; struct Cyc_Absyn_Tuniondecl*
_temp303; struct Cyc_Absyn_PtrInfo _temp305; struct Cyc_Absyn_Conref* _temp307;
struct Cyc_Absyn_Tqual _temp309; void* _temp311; struct Cyc_Absyn_Exp* _temp313;
struct Cyc_Absyn_Tqual _temp315; void* _temp317; struct Cyc_Absyn_FnInfo
_temp319; struct Cyc_List_List* _temp321; struct Cyc_Absyn_VarargInfo* _temp323;
int _temp325; struct Cyc_List_List* _temp327; void* _temp329; struct Cyc_List_List*
_temp331; struct Cyc_List_List* _temp333; struct Cyc_List_List* _temp335; struct
_tuple0* _temp337; struct _tuple0* _temp339; struct _tuple0* _temp341; struct
Cyc_Core_Opt* _temp343; struct Cyc_List_List* _temp345; struct _tuple0* _temp347;
void* _temp349; _LL234: if( _temp232 ==( void*) Cyc_Absyn_VoidType){ goto _LL235;}
else{ goto _LL236;} _LL236: if(( unsigned int) _temp232 > 4u?*(( int*) _temp232)
== Cyc_Absyn_Evar: 0){ _LL285: _temp284=(( struct Cyc_Absyn_Evar_struct*)
_temp232)->f2; if( _temp284 == 0){ goto _LL237;} else{ goto _LL238;}} else{ goto
_LL238;} _LL238: if(( unsigned int) _temp232 > 4u?*(( int*) _temp232) == Cyc_Absyn_Evar:
0){ _LL287: _temp286=(( struct Cyc_Absyn_Evar_struct*) _temp232)->f2; if(
_temp286 == 0){ goto _LL240;} else{ _temp288=* _temp286; _LL290: _temp289=( void*)
_temp288.v; goto _LL239;}} else{ goto _LL240;} _LL240: if(( unsigned int)
_temp232 > 4u?*(( int*) _temp232) == Cyc_Absyn_VarType: 0){ goto _LL241;} else{
goto _LL242;} _LL242: if(( unsigned int) _temp232 > 4u?*(( int*) _temp232) ==
Cyc_Absyn_TunionType: 0){ _LL292: _temp291=(( struct Cyc_Absyn_TunionType_struct*)
_temp232)->f1; _LL294: _temp293=( void*) _temp291.tunion_info; if(*(( int*)
_temp293) == Cyc_Absyn_KnownTunion){ _LL296: _temp295=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp293)->f1; goto _LL243;} else{ goto _LL244;}} else{ goto _LL244;} _LL244:
if(( unsigned int) _temp232 > 4u?*(( int*) _temp232) == Cyc_Absyn_TunionType: 0){
goto _LL245;} else{ goto _LL246;} _LL246: if(( unsigned int) _temp232 > 4u?*((
int*) _temp232) == Cyc_Absyn_TunionFieldType: 0){ _LL298: _temp297=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp232)->f1; _LL300: _temp299=( void*) _temp297.field_info; if(*(( int*)
_temp299) == Cyc_Absyn_KnownTunionfield){ _LL304: _temp303=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp299)->f1; goto _LL302; _LL302: _temp301=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp299)->f2; goto _LL247;} else{ goto _LL248;}} else{ goto _LL248;} _LL248:
if(( unsigned int) _temp232 > 4u?*(( int*) _temp232) == Cyc_Absyn_TunionFieldType:
0){ goto _LL249;} else{ goto _LL250;} _LL250: if(( unsigned int) _temp232 > 4u?*((
int*) _temp232) == Cyc_Absyn_PointerType: 0){ _LL306: _temp305=(( struct Cyc_Absyn_PointerType_struct*)
_temp232)->f1; _LL312: _temp311=( void*) _temp305.elt_typ; goto _LL310; _LL310:
_temp309= _temp305.tq; goto _LL308; _LL308: _temp307= _temp305.bounds; goto
_LL251;} else{ goto _LL252;} _LL252: if(( unsigned int) _temp232 > 4u?*(( int*)
_temp232) == Cyc_Absyn_IntType: 0){ goto _LL253;} else{ goto _LL254;} _LL254:
if( _temp232 ==( void*) Cyc_Absyn_FloatType){ goto _LL255;} else{ goto _LL256;}
_LL256: if( _temp232 ==( void*) Cyc_Absyn_DoubleType){ goto _LL257;} else{ goto
_LL258;} _LL258: if(( unsigned int) _temp232 > 4u?*(( int*) _temp232) == Cyc_Absyn_ArrayType:
0){ _LL318: _temp317=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp232)->f1;
goto _LL316; _LL316: _temp315=(( struct Cyc_Absyn_ArrayType_struct*) _temp232)->f2;
goto _LL314; _LL314: _temp313=(( struct Cyc_Absyn_ArrayType_struct*) _temp232)->f3;
goto _LL259;} else{ goto _LL260;} _LL260: if(( unsigned int) _temp232 > 4u?*((
int*) _temp232) == Cyc_Absyn_FnType: 0){ _LL320: _temp319=(( struct Cyc_Absyn_FnType_struct*)
_temp232)->f1; _LL330: _temp329=( void*) _temp319.ret_typ; goto _LL328; _LL328:
_temp327= _temp319.args; goto _LL326; _LL326: _temp325= _temp319.c_varargs; goto
_LL324; _LL324: _temp323= _temp319.cyc_varargs; goto _LL322; _LL322: _temp321=
_temp319.attributes; goto _LL261;} else{ goto _LL262;} _LL262: if(( unsigned int)
_temp232 > 4u?*(( int*) _temp232) == Cyc_Absyn_TupleType: 0){ _LL332: _temp331=((
struct Cyc_Absyn_TupleType_struct*) _temp232)->f1; goto _LL263;} else{ goto
_LL264;} _LL264: if(( unsigned int) _temp232 > 4u?*(( int*) _temp232) == Cyc_Absyn_AnonStructType:
0){ _LL334: _temp333=(( struct Cyc_Absyn_AnonStructType_struct*) _temp232)->f1;
goto _LL265;} else{ goto _LL266;} _LL266: if(( unsigned int) _temp232 > 4u?*((
int*) _temp232) == Cyc_Absyn_AnonUnionType: 0){ _LL336: _temp335=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp232)->f1; goto _LL267;} else{ goto _LL268;} _LL268: if(( unsigned int)
_temp232 > 4u?*(( int*) _temp232) == Cyc_Absyn_UnionType: 0){ _LL338: _temp337=((
struct Cyc_Absyn_UnionType_struct*) _temp232)->f1; goto _LL269;} else{ goto
_LL270;} _LL270: if(( unsigned int) _temp232 > 4u?*(( int*) _temp232) == Cyc_Absyn_StructType:
0){ _LL340: _temp339=(( struct Cyc_Absyn_StructType_struct*) _temp232)->f1; goto
_LL271;} else{ goto _LL272;} _LL272: if(( unsigned int) _temp232 > 4u?*(( int*)
_temp232) == Cyc_Absyn_EnumType: 0){ _LL342: _temp341=(( struct Cyc_Absyn_EnumType_struct*)
_temp232)->f1; goto _LL273;} else{ goto _LL274;} _LL274: if(( unsigned int)
_temp232 > 4u?*(( int*) _temp232) == Cyc_Absyn_TypedefType: 0){ _LL348: _temp347=((
struct Cyc_Absyn_TypedefType_struct*) _temp232)->f1; goto _LL346; _LL346:
_temp345=(( struct Cyc_Absyn_TypedefType_struct*) _temp232)->f2; goto _LL344;
_LL344: _temp343=(( struct Cyc_Absyn_TypedefType_struct*) _temp232)->f3; goto
_LL275;} else{ goto _LL276;} _LL276: if(( unsigned int) _temp232 > 4u?*(( int*)
_temp232) == Cyc_Absyn_RgnHandleType: 0){ _LL350: _temp349=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp232)->f1; goto _LL277;} else{ goto _LL278;} _LL278: if( _temp232 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL279;} else{ goto _LL280;} _LL280: if(( unsigned int)
_temp232 > 4u?*(( int*) _temp232) == Cyc_Absyn_AccessEff: 0){ goto _LL281;}
else{ goto _LL282;} _LL282: if(( unsigned int) _temp232 > 4u?*(( int*) _temp232)
== Cyc_Absyn_JoinEff: 0){ goto _LL283;} else{ goto _LL233;} _LL235: return t;
_LL237: return Cyc_Absyn_sint_t; _LL239: return Cyc_Toc_typ_to_c( _temp289);
_LL241: return Cyc_Absyn_void_star_typ(); _LL243: return Cyc_Absyn_void_star_typ();
_LL245:( int) _throw( Cyc_Tcutil_impos( _tag_arr("unresolved TunionType",
sizeof( unsigned char), 22u))); _LL247: if( _temp301->typs == 0){ if( _temp303->is_xtunion){
return Cyc_Absyn_star_typ( Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);}
else{ return Cyc_Absyn_uint_t;}} else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp301->name, _tag_arr("_struct", sizeof( unsigned char), 8u)));} _LL249:( int)
_throw( Cyc_Tcutil_impos( _tag_arr("unresolved TunionFieldType", sizeof(
unsigned char), 27u))); _LL251: _temp311= Cyc_Toc_typ_to_c_array( _temp311);{
void* _temp351=( void*)( Cyc_Absyn_compress_conref( _temp307))->v; void*
_temp357; _LL353: if(( unsigned int) _temp351 > 1u?*(( int*) _temp351) == Cyc_Absyn_Eq_constr:
0){ _LL358: _temp357=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp351)->f1;
if( _temp357 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL354;} else{ goto _LL355;}}
else{ goto _LL355;} _LL355: goto _LL356; _LL354: return Cyc_Toc_tagged_arr_typ;
_LL356: return Cyc_Absyn_star_typ( _temp311,( void*) Cyc_Absyn_HeapRgn, _temp309);
_LL352:;} _LL253: return t; _LL255: return t; _LL257: return t; _LL259: return(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp359=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp359[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp360; _temp360.tag= Cyc_Absyn_ArrayType; _temp360.f1=(
void*) Cyc_Toc_typ_to_c_array( _temp317); _temp360.f2= _temp315; _temp360.f3=
_temp313; _temp360;}); _temp359;}); _LL261: { struct Cyc_List_List* _temp361= 0;
for( 0; _temp321 != 0; _temp321=(( struct Cyc_List_List*) _check_null( _temp321))->tl){
void* _temp362=( void*)(( struct Cyc_List_List*) _check_null( _temp321))->hd;
_LL364: if( _temp362 ==( void*) Cyc_Absyn_Noreturn_att){ goto _LL365;} else{
goto _LL366;} _LL366: if( _temp362 ==( void*) Cyc_Absyn_Const_att){ goto _LL367;}
else{ goto _LL368;} _LL368: if(( unsigned int) _temp362 > 16u?*(( int*) _temp362)
== Cyc_Absyn_Format_att: 0){ goto _LL369;} else{ goto _LL370;} _LL370: goto
_LL371; _LL365: goto _LL367; _LL367: goto _LL369; _LL369: continue; _LL371:
_temp361=({ struct Cyc_List_List* _temp372=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp372->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( _temp321))->hd); _temp372->tl= _temp361; _temp372;}); goto _LL363;
_LL363:;}{ struct Cyc_List_List* _temp373=(( struct Cyc_List_List*(*)( struct
_tuple1*(* f)( struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_arg_to_c,
_temp327); if( _temp323 != 0){ void* _temp374= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ((
void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp323))->type,( void*)((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp323))->rgn, Cyc_Toc_mt_tq));
struct _tuple1* _temp375=({ struct _tuple1* _temp377=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp377->f1=(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp323))->name; _temp377->f2=(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp323))->tq; _temp377->f3= _temp374; _temp377;}); _temp373=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp373,({ struct Cyc_List_List* _temp376=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp376->hd=( void*) _temp375; _temp376->tl= 0;
_temp376;}));} return( void*)({ struct Cyc_Absyn_FnType_struct* _temp378=(
struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp378[ 0]=({ struct Cyc_Absyn_FnType_struct _temp379; _temp379.tag= Cyc_Absyn_FnType;
_temp379.f1=({ struct Cyc_Absyn_FnInfo _temp380; _temp380.tvars= 0; _temp380.effect=
0; _temp380.ret_typ=( void*) Cyc_Toc_typ_to_c( _temp329); _temp380.args=
_temp373; _temp380.c_varargs= _temp325; _temp380.cyc_varargs= 0; _temp380.rgn_po=
0; _temp380.attributes= _temp361; _temp380;}); _temp379;}); _temp378;});}}
_LL263: _temp331=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct
_tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f, _temp331);{
struct _tagged_arr* _temp381= Cyc_Toc_add_tuple_type( _temp331); return Cyc_Absyn_strct(
_temp381);} _LL265: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp382=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp382[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp383; _temp383.tag= Cyc_Absyn_AnonStructType; _temp383.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_structfield_to_c, _temp333); _temp383;}); _temp382;});
_LL267: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp384=(
struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp384[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp385; _temp385.tag=
Cyc_Absyn_AnonUnionType; _temp385.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp335); _temp385;}); _temp384;}); _LL269: if( _temp337 == 0){(( int(*)(
struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("anonymous union", sizeof(
unsigned char), 16u));} return Cyc_Absyn_unionq_typ(( struct _tuple0*)
_check_null( _temp337)); _LL271: if( _temp339 == 0){(( int(*)( struct
_tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("anonymous struct", sizeof(
unsigned char), 17u));} return Cyc_Absyn_strctq(( struct _tuple0*) _check_null(
_temp339)); _LL273: return t; _LL275: if( _temp343 == 0){ return Cyc_Toc_toc_impos(
_tag_arr("Toc::typ_to_c: unresolved TypedefType", sizeof( unsigned char), 38u));}
return( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp386=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp386[ 0]=({ struct
Cyc_Absyn_TypedefType_struct _temp387; _temp387.tag= Cyc_Absyn_TypedefType;
_temp387.f1= _temp347; _temp387.f2= 0; _temp387.f3=({ struct Cyc_Core_Opt*
_temp388=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp388->v=( void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp343))->v); _temp388;}); _temp387;}); _temp386;}); _LL277:
return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp), Cyc_Toc_mt_tq);
_LL279: return Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the heap region",
sizeof( unsigned char), 55u)); _LL281: return Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the access eff",
sizeof( unsigned char), 54u)); _LL283: return Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the join eff",
sizeof( unsigned char), 52u)); _LL233:;} static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast(
void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* l){ void*
_temp389= t; struct Cyc_Absyn_Tqual _temp395; void* _temp397; _LL391: if((
unsigned int) _temp389 > 4u?*(( int*) _temp389) == Cyc_Absyn_ArrayType: 0){
_LL398: _temp397=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp389)->f1;
goto _LL396; _LL396: _temp395=(( struct Cyc_Absyn_ArrayType_struct*) _temp389)->f2;
goto _LL392;} else{ goto _LL393;} _LL393: goto _LL394; _LL392: return Cyc_Absyn_cast_exp(
Cyc_Absyn_star_typ( _temp397,( void*) Cyc_Absyn_HeapRgn, _temp395), e, l);
_LL394: return Cyc_Absyn_cast_exp( t, e, l); _LL390:;} static int Cyc_Toc_atomic_typ(
void* t){ void* _temp399= Cyc_Tcutil_compress( t); void* _temp435; struct Cyc_Absyn_Structdecl**
_temp437; struct Cyc_List_List* _temp439; struct Cyc_List_List* _temp441; struct
Cyc_Absyn_TunionFieldInfo _temp443; void* _temp445; struct Cyc_Absyn_Tunionfield*
_temp447; struct Cyc_Absyn_Tuniondecl* _temp449; struct Cyc_List_List* _temp451;
_LL401: if( _temp399 ==( void*) Cyc_Absyn_VoidType){ goto _LL402;} else{ goto
_LL403;} _LL403: if(( unsigned int) _temp399 > 4u?*(( int*) _temp399) == Cyc_Absyn_VarType:
0){ goto _LL404;} else{ goto _LL405;} _LL405: if(( unsigned int) _temp399 > 4u?*((
int*) _temp399) == Cyc_Absyn_IntType: 0){ goto _LL406;} else{ goto _LL407;}
_LL407: if( _temp399 ==( void*) Cyc_Absyn_FloatType){ goto _LL408;} else{ goto
_LL409;} _LL409: if( _temp399 ==( void*) Cyc_Absyn_DoubleType){ goto _LL410;}
else{ goto _LL411;} _LL411: if(( unsigned int) _temp399 > 4u?*(( int*) _temp399)
== Cyc_Absyn_FnType: 0){ goto _LL412;} else{ goto _LL413;} _LL413: if((
unsigned int) _temp399 > 4u?*(( int*) _temp399) == Cyc_Absyn_ArrayType: 0){
_LL436: _temp435=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp399)->f1;
goto _LL414;} else{ goto _LL415;} _LL415: if(( unsigned int) _temp399 > 4u?*((
int*) _temp399) == Cyc_Absyn_StructType: 0){ _LL438: _temp437=(( struct Cyc_Absyn_StructType_struct*)
_temp399)->f3; goto _LL416;} else{ goto _LL417;} _LL417: if(( unsigned int)
_temp399 > 4u?*(( int*) _temp399) == Cyc_Absyn_AnonStructType: 0){ _LL440:
_temp439=(( struct Cyc_Absyn_AnonStructType_struct*) _temp399)->f1; goto _LL418;}
else{ goto _LL419;} _LL419: if(( unsigned int) _temp399 > 4u?*(( int*) _temp399)
== Cyc_Absyn_AnonUnionType: 0){ _LL442: _temp441=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp399)->f1; goto _LL420;} else{ goto _LL421;} _LL421: if(( unsigned int)
_temp399 > 4u?*(( int*) _temp399) == Cyc_Absyn_TunionFieldType: 0){ _LL444:
_temp443=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp399)->f1; _LL446:
_temp445=( void*) _temp443.field_info; if(*(( int*) _temp445) == Cyc_Absyn_KnownTunionfield){
_LL450: _temp449=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp445)->f1;
goto _LL448; _LL448: _temp447=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp445)->f2; goto _LL422;} else{ goto _LL423;}} else{ goto _LL423;} _LL423:
if(( unsigned int) _temp399 > 4u?*(( int*) _temp399) == Cyc_Absyn_TupleType: 0){
_LL452: _temp451=(( struct Cyc_Absyn_TupleType_struct*) _temp399)->f1; goto
_LL424;} else{ goto _LL425;} _LL425: if(( unsigned int) _temp399 > 4u?*(( int*)
_temp399) == Cyc_Absyn_TunionType: 0){ goto _LL426;} else{ goto _LL427;} _LL427:
if(( unsigned int) _temp399 > 4u?*(( int*) _temp399) == Cyc_Absyn_UnionType: 0){
goto _LL428;} else{ goto _LL429;} _LL429: if(( unsigned int) _temp399 > 4u?*((
int*) _temp399) == Cyc_Absyn_PointerType: 0){ goto _LL430;} else{ goto _LL431;}
_LL431: if(( unsigned int) _temp399 > 4u?*(( int*) _temp399) == Cyc_Absyn_RgnHandleType:
0){ goto _LL432;} else{ goto _LL433;} _LL433: goto _LL434; _LL402: return 1;
_LL404: return 0; _LL406: return 1; _LL408: return 1; _LL410: return 1; _LL412:
return 1; _LL414: return Cyc_Toc_atomic_typ( _temp435); _LL416: if( _temp437 ==
0? 1:(*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp437)))->fields == 0){
return 0;}{ struct Cyc_List_List* _temp453=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp437)))->fields))->v;
for( 0; _temp453 != 0; _temp453=(( struct Cyc_List_List*) _check_null( _temp453))->tl){
if( ! Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp453))->hd)->type)){ return 0;}}} return 1; _LL418: _temp441=
_temp439; goto _LL420; _LL420: for( 0; _temp441 != 0; _temp441=(( struct Cyc_List_List*)
_check_null( _temp441))->tl){ if( ! Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp441))->hd)->type)){ return 0;}} return
1; _LL422: _temp451= _temp447->typs; goto _LL424; _LL424: for( 0; _temp451 != 0;
_temp451=(( struct Cyc_List_List*) _check_null( _temp451))->tl){ if( ! Cyc_Toc_atomic_typ((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp451))->hd)).f2)){
return 0;}} return 1; _LL426: return 0; _LL428: return 0; _LL430: return 0;
_LL432: return 0; _LL434:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr(* _temp454)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp458= _tag_arr("atomic_typ:  bad type %s",
sizeof( unsigned char), 25u); struct Cyc_Stdio_String_pa_struct _temp459;
_temp459.tag= Cyc_Stdio_String_pa; _temp459.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp457=( void*)& _temp459; void* _temp455[ 1u]={ _temp457}; struct
_tagged_arr _temp456={( void*) _temp455,( void*) _temp455,( void*)( _temp455 + 1u)};
_temp454( _temp458, _temp456);}})); return 0; _LL400:;} static int Cyc_Toc_is_void_star(
void* t){ void* _temp460= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp466; void* _temp468; _LL462: if(( unsigned int) _temp460 > 4u?*(( int*)
_temp460) == Cyc_Absyn_PointerType: 0){ _LL467: _temp466=(( struct Cyc_Absyn_PointerType_struct*)
_temp460)->f1; _LL469: _temp468=( void*) _temp466.elt_typ; goto _LL463;} else{
goto _LL464;} _LL464: goto _LL465; _LL463: { void* _temp470= Cyc_Tcutil_compress(
_temp468); _LL472: if( _temp470 ==( void*) Cyc_Absyn_VoidType){ goto _LL473;}
else{ goto _LL474;} _LL474: goto _LL475; _LL473: return 1; _LL475: return 0;
_LL471:;} _LL465: return 0; _LL461:;} static int Cyc_Toc_is_poly_field( void* t,
struct _tagged_arr* f){ void* _temp476= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp488; struct Cyc_Absyn_Structdecl* _temp490; struct Cyc_List_List* _temp491;
_LL478: if(( unsigned int) _temp476 > 4u?*(( int*) _temp476) == Cyc_Absyn_StructType:
0){ _LL489: _temp488=(( struct Cyc_Absyn_StructType_struct*) _temp476)->f3; if(
_temp488 == 0){ goto _LL480;} else{ _temp490=* _temp488; goto _LL479;}} else{
goto _LL480;} _LL480: if(( unsigned int) _temp476 > 4u?*(( int*) _temp476) ==
Cyc_Absyn_AnonStructType: 0){ _LL492: _temp491=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp476)->f1; goto _LL481;} else{ goto _LL482;} _LL482: if(( unsigned int)
_temp476 > 4u?*(( int*) _temp476) == Cyc_Absyn_AnonUnionType: 0){ goto _LL483;}
else{ goto _LL484;} _LL484: if(( unsigned int) _temp476 > 4u?*(( int*) _temp476)
== Cyc_Absyn_UnionType: 0){ goto _LL485;} else{ goto _LL486;} _LL486: goto
_LL487; _LL479: if( _temp490->fields == 0){( int) _throw( Cyc_Toc_toc_impos(
_tag_arr("is_poly_field: struct missing fields", sizeof( unsigned char), 37u)));}
_temp491=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp490->fields))->v;
goto _LL481; _LL481: { struct Cyc_Absyn_Structfield* _temp493= Cyc_Absyn_lookup_field(
_temp491, f); if( _temp493 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr(* _temp494)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp498= _tag_arr("is_poly_field: bad field %s",
sizeof( unsigned char), 28u); struct Cyc_Stdio_String_pa_struct _temp499;
_temp499.tag= Cyc_Stdio_String_pa; _temp499.f1=( struct _tagged_arr)*(( struct
Cyc_Absyn_Structfield*) _check_null( _temp493))->name;{ void* _temp497=( void*)&
_temp499; void* _temp495[ 1u]={ _temp497}; struct _tagged_arr _temp496={( void*)
_temp495,( void*) _temp495,( void*)( _temp495 + 1u)}; _temp494( _temp498,
_temp496);}}));} return Cyc_Toc_is_void_star(( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp493))->type);} _LL483: return 0; _LL485: return 0; _LL487:(
int) _throw( Cyc_Toc_toc_impos(( struct _tagged_arr)({ struct _tagged_arr(*
_temp500)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp504= _tag_arr("is_poly_field: bad type %s", sizeof(
unsigned char), 27u); struct Cyc_Stdio_String_pa_struct _temp505; _temp505.tag=
Cyc_Stdio_String_pa; _temp505.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp503=( void*)& _temp505; void* _temp501[ 1u]={ _temp503}; struct
_tagged_arr _temp502={( void*) _temp501,( void*) _temp501,( void*)( _temp501 + 1u)};
_temp500( _temp504, _temp502);}}))); _LL477:;} static int Cyc_Toc_is_poly_project(
struct Cyc_Absyn_Exp* e){ void* _temp506=( void*) e->r; struct _tagged_arr*
_temp514; struct Cyc_Absyn_Exp* _temp516; struct _tagged_arr* _temp518; struct
Cyc_Absyn_Exp* _temp520; _LL508: if(*(( int*) _temp506) == Cyc_Absyn_StructMember_e){
_LL517: _temp516=(( struct Cyc_Absyn_StructMember_e_struct*) _temp506)->f1; goto
_LL515; _LL515: _temp514=(( struct Cyc_Absyn_StructMember_e_struct*) _temp506)->f2;
goto _LL509;} else{ goto _LL510;} _LL510: if(*(( int*) _temp506) == Cyc_Absyn_StructArrow_e){
_LL521: _temp520=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp506)->f1; goto
_LL519; _LL519: _temp518=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp506)->f2;
goto _LL511;} else{ goto _LL512;} _LL512: goto _LL513; _LL509: return Cyc_Toc_is_poly_field((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp516->topt))->v, _temp514);
_LL511: { void* _temp522= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp520->topt))->v); struct Cyc_Absyn_PtrInfo _temp528; void*
_temp530; _LL524: if(( unsigned int) _temp522 > 4u?*(( int*) _temp522) == Cyc_Absyn_PointerType:
0){ _LL529: _temp528=(( struct Cyc_Absyn_PointerType_struct*) _temp522)->f1;
_LL531: _temp530=( void*) _temp528.elt_typ; goto _LL525;} else{ goto _LL526;}
_LL526: goto _LL527; _LL525: return Cyc_Toc_is_poly_field( _temp530, _temp518);
_LL527:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({
struct _tagged_arr(* _temp532)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp536= _tag_arr("is_poly_project:  bad type %s", sizeof(
unsigned char), 30u); struct Cyc_Stdio_String_pa_struct _temp537; _temp537.tag=
Cyc_Stdio_String_pa; _temp537.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp520->topt))->v);{ void*
_temp535=( void*)& _temp537; void* _temp533[ 1u]={ _temp535}; struct _tagged_arr
_temp534={( void*) _temp533,( void*) _temp533,( void*)( _temp533 + 1u)};
_temp532( _temp536, _temp534);}})); return 0; _LL523:;} _LL513: return 0; _LL507:;}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Toc_GC_malloc_e,({ struct Cyc_List_List*
_temp538=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp538->hd=( void*) s; _temp538->tl= 0; _temp538;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_atomic( struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp(
Cyc_Toc_GC_malloc_atomic_e,({ struct Cyc_List_List* _temp539=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp539->hd=( void*) s; _temp539->tl=
0; _temp539;}), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t,
struct Cyc_Absyn_Exp* s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic(
s);} return Cyc_Toc_malloc_ptr( s);} static struct Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp* rgn, struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_malloc_e,({ struct Cyc_List_List*(* _temp540)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp543= rgn; struct Cyc_Absyn_Exp* _temp544= s; struct
Cyc_Absyn_Exp* _temp541[ 2u]={ _temp543, _temp544}; struct _tagged_arr _temp542={(
void*) _temp541,( void*) _temp541,( void*)( _temp541 + 2u)}; _temp540( _temp542);}),
0);} static struct Cyc_Absyn_Exp* Cyc_Toc_fdconv( struct Cyc_Absyn_Exp* e){
return Cyc_Absyn_fncall_exp( Cyc_Toc__sfile_to_file_e,({ struct Cyc_List_List*
_temp545=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp545->hd=( void*) e; _temp545->tl= 0; _temp545;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp( Cyc_Toc__throw_e,({
struct Cyc_List_List* _temp546=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp546->hd=( void*) e; _temp546->tl= 0; _temp546;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_make_toplevel_tagged_arr( void* t, struct
Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp* e){ int is_string= 0;{ void* _temp547=(
void*) e->r; void* _temp553; _LL549: if(*(( int*) _temp547) == Cyc_Absyn_Const_e){
_LL554: _temp553=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp547)->f1; if((
unsigned int) _temp553 > 1u?*(( int*) _temp553) == Cyc_Absyn_String_c: 0){ goto
_LL550;} else{ goto _LL551;}} else{ goto _LL551;} _LL551: goto _LL552; _LL550:
is_string= 1; goto _LL548; _LL552: goto _LL548; _LL548:;}{ struct Cyc_Absyn_Exp*
xexp; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var(); void* vd_typ=( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp558=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp558[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp559; _temp559.tag= Cyc_Absyn_ArrayType; _temp559.f1=(
void*) Cyc_Absyn_uchar_t; _temp559.f2= Cyc_Toc_mt_tq; _temp559.f3=( struct Cyc_Absyn_Exp*)
sz; _temp559;}); _temp558;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp555=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp555->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp556=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp556[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp557; _temp557.tag= Cyc_Absyn_Var_d;
_temp557.f1= vd; _temp557;}); _temp556;}), 0); _temp555->tl= Cyc_Toc_result_decls;
_temp555;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ xexp= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), e, 0);}{ struct Cyc_Absyn_Exp* xplussz= Cyc_Absyn_add_exp(
xexp, sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp( 0,({
struct Cyc_List_List*(* _temp560)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct _tuple4* _temp563=({ struct _tuple4*
_temp568=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp568->f1= 0;
_temp568->f2= xexp; _temp568;}); struct _tuple4* _temp564=({ struct _tuple4*
_temp567=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp567->f1= 0;
_temp567->f2= xexp; _temp567;}); struct _tuple4* _temp565=({ struct _tuple4*
_temp566=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp566->f1= 0;
_temp566->f2= xplussz; _temp566;}); struct _tuple4* _temp561[ 3u]={ _temp563,
_temp564, _temp565}; struct _tagged_arr _temp562={( void*) _temp561,( void*)
_temp561,( void*)( _temp561 + 3u)}; _temp560( _temp562);}), 0); return urm_exp;}}}
struct Cyc_Toc_Env{ struct Cyc_Core_Opt* break_lab; struct Cyc_Core_Opt*
continue_lab; struct Cyc_Core_Opt* fallthru_info; struct Cyc_Dict_Dict* varmap;
int toplevel; } ; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){ return({
struct Cyc_Toc_Env* _temp569=( struct Cyc_Toc_Env*) GC_malloc( sizeof( struct
Cyc_Toc_Env)); _temp569->break_lab= 0; _temp569->continue_lab= 0; _temp569->fallthru_info=
0; _temp569->varmap=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp569->toplevel= 1;
_temp569;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){
return({ struct Cyc_Toc_Env* _temp570=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp570->break_lab= e->break_lab; _temp570->continue_lab=
e->continue_lab; _temp570->fallthru_info= e->fallthru_info; _temp570->varmap= e->varmap;
_temp570->toplevel= e->toplevel; _temp570;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp571=(* x).f1; _LL573:
if(( unsigned int) _temp571 > 1u?*(( int*) _temp571) == Cyc_Absyn_Rel_n: 0){
goto _LL574;} else{ goto _LL575;} _LL575: goto _LL576; _LL574:( int) _throw( Cyc_Toc_toc_impos((
struct _tagged_arr)({ struct _tagged_arr(* _temp577)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp581= _tag_arr("Toc::add_varmap on Rel_n: %s\n",
sizeof( unsigned char), 30u); struct Cyc_Stdio_String_pa_struct _temp582;
_temp582.tag= Cyc_Stdio_String_pa; _temp582.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
x);{ void* _temp580=( void*)& _temp582; void* _temp578[ 1u]={ _temp580}; struct
_tagged_arr _temp579={( void*) _temp578,( void*) _temp578,( void*)( _temp578 + 1u)};
_temp577( _temp581, _temp579);}}))); _LL576: goto _LL572; _LL572:;}{ struct Cyc_Toc_Env*
ans= Cyc_Toc_copy_env( e); ans->varmap=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tuple0* key, struct Cyc_Absyn_Exp* data)) Cyc_Dict_insert)( ans->varmap,
x, y); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_loop_env( struct Cyc_Toc_Env*
e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->continue_lab=
0; return ans;} struct _tuple6{ struct _tagged_arr* f1; struct Cyc_List_List* f2;
struct Cyc_Dict_Dict* f3; } ; static struct Cyc_Toc_Env* Cyc_Toc_non_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_arr* break_l, struct _tagged_arr*
fallthru_l, struct Cyc_List_List* fallthru_binders, struct Cyc_Toc_Env*
next_case_env){ struct Cyc_List_List* _temp583= 0; for( 0; fallthru_binders != 0;
fallthru_binders=(( struct Cyc_List_List*) _check_null( fallthru_binders))->tl){
_temp583=({ struct Cyc_List_List* _temp584=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp584->hd=( void*)(( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( fallthru_binders))->hd)->name; _temp584->tl=
_temp583; _temp584;});} _temp583=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp583);{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab=({ struct Cyc_Core_Opt* _temp585=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp585->v=( void*) break_l; _temp585;});
ans->fallthru_info=({ struct Cyc_Core_Opt* _temp586=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp586->v=( void*)({ struct _tuple6*
_temp587=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp587->f1=
fallthru_l; _temp587->f2= _temp583; _temp587->f3= next_case_env->varmap;
_temp587;}); _temp586;}); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_arr* break_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt* _temp588=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp588->v=( void*)
break_l; _temp588;}); ans->fallthru_info= 0; return ans;} static struct Cyc_Toc_Env*
Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env* e, struct _tagged_arr* next_l){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->fallthru_info=({
struct Cyc_Core_Opt* _temp589=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp589->v=( void*)({ struct _tuple6* _temp590=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp590->f1= next_l; _temp590->f2= 0;
_temp590->f3=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct
_tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp590;}); _temp589;});
return ans;} static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e); static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt*
s); static void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp* e){ if( e->topt == 0){((
int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("Missing type in primop ",
sizeof( unsigned char), 24u));} return Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v);} static void* Cyc_Toc_get_cyc_typ( struct Cyc_Absyn_Exp*
e){ if( e->topt == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("Missing type in primop ", sizeof( unsigned char), 24u));} return( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v;} static struct _tuple3* Cyc_Toc_tup_to_c(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple3* _temp591=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp591->f1= Cyc_Toc_mt_tq; _temp591->f2=
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_temp591;});} static struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4*
_temp592=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp592->f1= 0;
_temp592->f2= e; _temp592;});} static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct(
struct Cyc_Toc_Env* nv, struct _tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt*
s, int pointer, struct Cyc_Absyn_Exp* rgnopt, int is_atomic){ struct Cyc_Absyn_Exp*
eo; void* t; if( pointer){ t= Cyc_Absyn_cstar_typ( struct_typ, Cyc_Toc_mt_tq);{
struct Cyc_Absyn_Exp* _temp593= Cyc_Absyn_sizeoftyp_exp( struct_typ, 0); if(
rgnopt == 0){ eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, is_atomic? Cyc_Toc_malloc_atomic(
_temp593): Cyc_Toc_malloc_ptr( _temp593), 0);} else{ struct Cyc_Absyn_Exp* r=(
struct Cyc_Absyn_Exp*) _check_null( rgnopt); Cyc_Toc_exp_to_c( nv, r); eo=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp( r, _temp593),
0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) - 1;{ struct Cyc_List_List* _temp594=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( dles0); for( 0; _temp594 != 0; _temp594=((
struct Cyc_List_List*) _check_null( _temp594))->tl){ struct _tuple4 _temp597;
struct Cyc_Absyn_Exp* _temp598; struct Cyc_List_List* _temp600; struct _tuple4*
_temp595=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp594))->hd;
_temp597=* _temp595; _LL601: _temp600= _temp597.f1; goto _LL599; _LL599:
_temp598= _temp597.f2; goto _LL596; _LL596: { struct Cyc_Absyn_Exp* e_index; if(
_temp600 == 0){ e_index= Cyc_Absyn_signed_int_exp( count --, 0);} else{ if(((
struct Cyc_List_List*) _check_null( _temp600))->tl != 0){(( int(*)( struct
_tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("multiple designators in array",
sizeof( unsigned char), 30u));}{ void* _temp602=( void*)(( struct Cyc_List_List*)
_check_null( _temp600))->hd; void* _temp603= _temp602; struct Cyc_Absyn_Exp*
_temp609; _LL605: if(*(( int*) _temp603) == Cyc_Absyn_ArrayElement){ _LL610:
_temp609=(( struct Cyc_Absyn_ArrayElement_struct*) _temp603)->f1; goto _LL606;}
else{ goto _LL607;} _LL607: if(*(( int*) _temp603) == Cyc_Absyn_FieldName){ goto
_LL608;} else{ goto _LL604;} _LL606: Cyc_Toc_exp_to_c( nv, _temp609); e_index=
_temp609; goto _LL604; _LL608: e_index=(( struct Cyc_Absyn_Exp*(*)( struct
_tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("field name designators in array",
sizeof( unsigned char), 32u)); goto _LL604; _LL604:;}}{ struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, e_index, 0); void* _temp611=( void*)
_temp598->r; struct Cyc_List_List* _temp621; struct Cyc_Absyn_Exp* _temp623;
struct Cyc_Absyn_Exp* _temp625; struct Cyc_Absyn_Vardecl* _temp627; struct Cyc_List_List*
_temp629; void* _temp631; _LL613: if(*(( int*) _temp611) == Cyc_Absyn_Array_e){
_LL622: _temp621=(( struct Cyc_Absyn_Array_e_struct*) _temp611)->f1; goto _LL614;}
else{ goto _LL615;} _LL615: if(*(( int*) _temp611) == Cyc_Absyn_Comprehension_e){
_LL628: _temp627=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp611)->f1;
goto _LL626; _LL626: _temp625=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp611)->f2; goto _LL624; _LL624: _temp623=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp611)->f3; goto _LL616;} else{ goto _LL617;} _LL617: if(*(( int*) _temp611)
== Cyc_Absyn_AnonStruct_e){ _LL632: _temp631=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp611)->f1; goto _LL630; _LL630: _temp629=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp611)->f2; goto _LL618;} else{ goto _LL619;} _LL619: goto _LL620; _LL614: s=
Cyc_Toc_init_array( nv, lval, _temp621, s); goto _LL612; _LL616: s= Cyc_Toc_init_comprehension(
nv, lval, _temp627, _temp625, _temp623, s, 0); goto _LL612; _LL618: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp631, _temp629, s); goto _LL612; _LL620: Cyc_Toc_exp_to_c( nv,
_temp598); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
lhs, e_index, 0), _temp598, 0), s, 0); goto _LL612; _LL612:;}}}} return s;}
static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s, int
e1_already_translated){ struct _tuple0* _temp633= vd->name; if( !
e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env* nv2=
Cyc_Toc_add_varmap( nv, _temp633, Cyc_Absyn_varb_exp( _temp633,( void*)({ struct
Cyc_Absyn_Local_b_struct* _temp656=( struct Cyc_Absyn_Local_b_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Local_b_struct)); _temp656[ 0]=({ struct Cyc_Absyn_Local_b_struct
_temp657; _temp657.tag= Cyc_Absyn_Local_b; _temp657.f1= vd; _temp657;});
_temp656;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( _temp633, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp(
_temp633, 0), Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp(
Cyc_Absyn_var_exp( _temp633, 0), 0); struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp(
lhs, Cyc_Absyn_var_exp( _temp633, 0), 0); struct Cyc_Absyn_Stmt* body;{ void*
_temp634=( void*) e2->r; struct Cyc_List_List* _temp644; struct Cyc_Absyn_Exp*
_temp646; struct Cyc_Absyn_Exp* _temp648; struct Cyc_Absyn_Vardecl* _temp650;
struct Cyc_List_List* _temp652; void* _temp654; _LL636: if(*(( int*) _temp634)
== Cyc_Absyn_Array_e){ _LL645: _temp644=(( struct Cyc_Absyn_Array_e_struct*)
_temp634)->f1; goto _LL637;} else{ goto _LL638;} _LL638: if(*(( int*) _temp634)
== Cyc_Absyn_Comprehension_e){ _LL651: _temp650=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp634)->f1; goto _LL649; _LL649: _temp648=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp634)->f2; goto _LL647; _LL647: _temp646=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp634)->f3; goto _LL639;} else{ goto _LL640;} _LL640: if(*(( int*) _temp634)
== Cyc_Absyn_AnonStruct_e){ _LL655: _temp654=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp634)->f1; goto _LL653; _LL653: _temp652=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp634)->f2; goto _LL641;} else{ goto _LL642;} _LL642: goto _LL643; _LL637:
body= Cyc_Toc_init_array( nv2, lval, _temp644, Cyc_Absyn_skip_stmt( 0)); goto
_LL635; _LL639: body= Cyc_Toc_init_comprehension( nv2, lval, _temp650, _temp648,
_temp646, Cyc_Absyn_skip_stmt( 0), 0); goto _LL635; _LL641: body= Cyc_Toc_init_anon_struct(
nv, lval, _temp654, _temp652, Cyc_Absyn_skip_stmt( 0)); goto _LL635; _LL643: Cyc_Toc_exp_to_c(
nv2, e2); body= Cyc_Absyn_assign_stmt( lval, e2, 0); goto _LL635; _LL635:;}{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt( ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt(
_temp633, Cyc_Absyn_uint_t, 0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_anon_struct( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs,
void* struct_type, struct Cyc_List_List* dles, struct Cyc_Absyn_Stmt* s){{
struct Cyc_List_List* _temp658=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( dles); for( 0; _temp658 != 0; _temp658=(( struct Cyc_List_List*)
_check_null( _temp658))->tl){ struct _tuple4 _temp661; struct Cyc_Absyn_Exp*
_temp662; struct Cyc_List_List* _temp664; struct _tuple4* _temp659=( struct
_tuple4*)(( struct Cyc_List_List*) _check_null( _temp658))->hd; _temp661=*
_temp659; _LL665: _temp664= _temp661.f1; goto _LL663; _LL663: _temp662= _temp661.f2;
goto _LL660; _LL660: if( _temp664 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("empty designator list", sizeof( unsigned char), 22u));} if((( struct
Cyc_List_List*) _check_null( _temp664))->tl != 0){(( int(*)( struct _tagged_arr
s)) Cyc_Toc_toc_impos)( _tag_arr("too many designators in anonymous struct",
sizeof( unsigned char), 41u));}{ void* _temp666=( void*)(( struct Cyc_List_List*)
_check_null( _temp664))->hd; struct _tagged_arr* _temp672; _LL668: if(*(( int*)
_temp666) == Cyc_Absyn_FieldName){ _LL673: _temp672=(( struct Cyc_Absyn_FieldName_struct*)
_temp666)->f1; goto _LL669;} else{ goto _LL670;} _LL670: goto _LL671; _LL669: {
struct Cyc_Absyn_Exp* lval= Cyc_Absyn_structmember_exp( lhs, _temp672, 0);{ void*
_temp674=( void*) _temp662->r; struct Cyc_List_List* _temp684; struct Cyc_Absyn_Exp*
_temp686; struct Cyc_Absyn_Exp* _temp688; struct Cyc_Absyn_Vardecl* _temp690;
struct Cyc_List_List* _temp692; void* _temp694; _LL676: if(*(( int*) _temp674)
== Cyc_Absyn_Array_e){ _LL685: _temp684=(( struct Cyc_Absyn_Array_e_struct*)
_temp674)->f1; goto _LL677;} else{ goto _LL678;} _LL678: if(*(( int*) _temp674)
== Cyc_Absyn_Comprehension_e){ _LL691: _temp690=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp674)->f1; goto _LL689; _LL689: _temp688=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp674)->f2; goto _LL687; _LL687: _temp686=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp674)->f3; goto _LL679;} else{ goto _LL680;} _LL680: if(*(( int*) _temp674)
== Cyc_Absyn_AnonStruct_e){ _LL695: _temp694=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp674)->f1; goto _LL693; _LL693: _temp692=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp674)->f2; goto _LL681;} else{ goto _LL682;} _LL682: goto _LL683; _LL677: s=
Cyc_Toc_init_array( nv, lval, _temp684, s); goto _LL675; _LL679: s= Cyc_Toc_init_comprehension(
nv, lval, _temp690, _temp688, _temp686, s, 0); goto _LL675; _LL681: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp694, _temp692, s); goto _LL675; _LL683: Cyc_Toc_exp_to_c( nv,
_temp662); if( Cyc_Toc_is_poly_field( struct_type, _temp672)){ _temp662= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp662, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp662, 0), 0), s, 0); goto _LL675; _LL675:;} goto
_LL667;} _LL671:( int) _throw( Cyc_Toc_toc_impos( _tag_arr("array designator in struct",
sizeof( unsigned char), 27u))); _LL667:;}}} return s;} static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple( struct Cyc_Toc_Env* nv, int pointer, struct Cyc_Absyn_Exp*
rgnopt, struct Cyc_List_List* es){ struct _RegionHandle _temp696= _new_region();
struct _RegionHandle* r=& _temp696; _push_region( r);{ struct Cyc_List_List*
_temp697=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple3*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_rmap)( r, Cyc_Toc_tup_to_c,
es); struct _tagged_arr* _temp698= Cyc_Toc_add_tuple_type( _temp697); struct
_tuple0* _temp699= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp700= Cyc_Absyn_var_exp(
_temp699, 0); struct Cyc_Absyn_Stmt* _temp701= Cyc_Absyn_exp_stmt( _temp700, 0);
struct Cyc_Absyn_Exp*(* _temp702)( struct Cyc_Absyn_Exp*, struct _tagged_arr*,
struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp;
int is_atomic= 1; struct Cyc_List_List* _temp703=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, es);{ int i=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp703); for( 0; _temp703
!= 0;( _temp703=(( struct Cyc_List_List*) _check_null( _temp703))->tl, -- i)){
struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp703))->hd; struct Cyc_Absyn_Exp* lval= _temp702( _temp700, Cyc_Toc_fieldname(
i), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v): 0;{ void* _temp704=( void*) e->r; struct Cyc_List_List*
_temp712; struct Cyc_Absyn_Exp* _temp714; struct Cyc_Absyn_Exp* _temp716; struct
Cyc_Absyn_Vardecl* _temp718; _LL706: if(*(( int*) _temp704) == Cyc_Absyn_Array_e){
_LL713: _temp712=(( struct Cyc_Absyn_Array_e_struct*) _temp704)->f1; goto _LL707;}
else{ goto _LL708;} _LL708: if(*(( int*) _temp704) == Cyc_Absyn_Comprehension_e){
_LL719: _temp718=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp704)->f1;
goto _LL717; _LL717: _temp716=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp704)->f2; goto _LL715; _LL715: _temp714=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp704)->f3; goto _LL709;} else{ goto _LL710;} _LL710: goto _LL711; _LL707:
_temp701= Cyc_Toc_init_array( nv, lval, _temp712, _temp701); goto _LL705; _LL709:
_temp701= Cyc_Toc_init_comprehension( nv, lval, _temp718, _temp716, _temp714,
_temp701, 0); goto _LL705; _LL711: Cyc_Toc_exp_to_c( nv, e); _temp701= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp702( _temp700, Cyc_Toc_fieldname(
i), 0), e, 0), 0), _temp701, 0); goto _LL705; _LL705:;}}}{ struct Cyc_Absyn_Exp*
_temp720= Cyc_Toc_make_struct( nv, _temp699, Cyc_Absyn_strct( _temp698),
_temp701, pointer, rgnopt, is_atomic); _npop_handler( 0u); return _temp720;}};
_pop_region( r);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env*
nv, void* struct_type, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
dles, struct _tuple0* tdn){ struct _tuple0* _temp721= Cyc_Toc_temp_var(); struct
Cyc_Absyn_Exp* _temp722= Cyc_Absyn_var_exp( _temp721, 0); struct Cyc_Absyn_Stmt*
_temp723= Cyc_Absyn_exp_stmt( _temp722, 0); struct Cyc_Absyn_Exp*(* _temp724)(
struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; int is_atomic= 1;
struct _RegionHandle _temp725= _new_region(); struct _RegionHandle* r=& _temp725;
_push_region( r);{ struct Cyc_List_List* _temp726=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, dles); for(
0; _temp726 != 0; _temp726=(( struct Cyc_List_List*) _check_null( _temp726))->tl){
struct _tuple4 _temp729; struct Cyc_Absyn_Exp* _temp730; struct Cyc_List_List*
_temp732; struct _tuple4* _temp727=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp726))->hd; _temp729=* _temp727; _LL733: _temp732= _temp729.f1;
goto _LL731; _LL731: _temp730= _temp729.f2; goto _LL728; _LL728: is_atomic=
is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp730->topt))->v): 0; if( _temp732 == 0){(( int(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("empty designator list", sizeof( unsigned char), 22u));}
if((( struct Cyc_List_List*) _check_null( _temp732))->tl != 0){ struct _tuple0*
_temp734= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp735= Cyc_Absyn_var_exp(
_temp734, 0); for( 0; _temp732 != 0; _temp732=(( struct Cyc_List_List*)
_check_null( _temp732))->tl){ void* _temp736=( void*)(( struct Cyc_List_List*)
_check_null( _temp732))->hd; struct _tagged_arr* _temp742; _LL738: if(*(( int*)
_temp736) == Cyc_Absyn_FieldName){ _LL743: _temp742=(( struct Cyc_Absyn_FieldName_struct*)
_temp736)->f1; goto _LL739;} else{ goto _LL740;} _LL740: goto _LL741; _LL739:
if( Cyc_Toc_is_poly_field( struct_type, _temp742)){ _temp735= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp735, 0);} _temp723= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( _temp724( _temp722, _temp742, 0), _temp735, 0), 0),
_temp723, 0); goto _LL737; _LL741:( int) _throw( Cyc_Toc_toc_impos( _tag_arr("array designator in struct",
sizeof( unsigned char), 27u))); _LL737:;} Cyc_Toc_exp_to_c( nv, _temp730);
_temp723= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp735,
_temp730, 0), 0), _temp723, 0);} else{ void* _temp744=( void*)(( struct Cyc_List_List*)
_check_null( _temp732))->hd; struct _tagged_arr* _temp750; _LL746: if(*(( int*)
_temp744) == Cyc_Absyn_FieldName){ _LL751: _temp750=(( struct Cyc_Absyn_FieldName_struct*)
_temp744)->f1; goto _LL747;} else{ goto _LL748;} _LL748: goto _LL749; _LL747: {
struct Cyc_Absyn_Exp* lval= _temp724( _temp722, _temp750, 0);{ void* _temp752=(
void*) _temp730->r; struct Cyc_List_List* _temp762; struct Cyc_Absyn_Exp*
_temp764; struct Cyc_Absyn_Exp* _temp766; struct Cyc_Absyn_Vardecl* _temp768;
struct Cyc_List_List* _temp770; void* _temp772; _LL754: if(*(( int*) _temp752)
== Cyc_Absyn_Array_e){ _LL763: _temp762=(( struct Cyc_Absyn_Array_e_struct*)
_temp752)->f1; goto _LL755;} else{ goto _LL756;} _LL756: if(*(( int*) _temp752)
== Cyc_Absyn_Comprehension_e){ _LL769: _temp768=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp752)->f1; goto _LL767; _LL767: _temp766=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp752)->f2; goto _LL765; _LL765: _temp764=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp752)->f3; goto _LL757;} else{ goto _LL758;} _LL758: if(*(( int*) _temp752)
== Cyc_Absyn_AnonStruct_e){ _LL773: _temp772=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp752)->f1; goto _LL771; _LL771: _temp770=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp752)->f2; goto _LL759;} else{ goto _LL760;} _LL760: goto _LL761; _LL755:
_temp723= Cyc_Toc_init_array( nv, lval, _temp762, _temp723); goto _LL753; _LL757:
_temp723= Cyc_Toc_init_comprehension( nv, lval, _temp768, _temp766, _temp764,
_temp723, 0); goto _LL753; _LL759: _temp723= Cyc_Toc_init_anon_struct( nv, lval,
_temp772, _temp770, _temp723); goto _LL753; _LL761: Cyc_Toc_exp_to_c( nv,
_temp730); if( Cyc_Toc_is_poly_field( struct_type, _temp750)){ _temp730= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp730, 0);} _temp723= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp730, 0), 0), _temp723, 0); goto _LL753; _LL753:;}
goto _LL745;} _LL749:( int) _throw( Cyc_Toc_toc_impos( _tag_arr("array designator in struct",
sizeof( unsigned char), 27u))); _LL745:;}}}{ struct Cyc_Absyn_Exp* _temp774= Cyc_Toc_make_struct(
nv, _temp721, Cyc_Absyn_strctq( tdn), _temp723, pointer, rgnopt, is_atomic);
_npop_handler( 0u); return _temp774;}; _pop_region( r);} struct _tuple7{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue(
struct Cyc_Absyn_Exp* el, struct _tuple7* pr){ return Cyc_Absyn_assignop_exp( el,(*
pr).f1,(* pr).f2, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue(
struct Cyc_Absyn_Exp* e1, int ignore){ return Cyc_Absyn_address_exp( e1, 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp* e1, void*
incr){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Increment_e_struct*
_temp775=( struct Cyc_Absyn_Increment_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct));
_temp775[ 0]=({ struct Cyc_Absyn_Increment_e_struct _temp776; _temp776.tag= Cyc_Absyn_Increment_e;
_temp776.f1= e1; _temp776.f2=( void*) incr; _temp776;}); _temp775;}), 0);}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env);
static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp777=( void*) e1->r; struct Cyc_Absyn_Stmt* _temp787; struct Cyc_Absyn_Exp*
_temp789; void* _temp791; struct _tagged_arr* _temp793; struct Cyc_Absyn_Exp*
_temp795; _LL779: if(*(( int*) _temp777) == Cyc_Absyn_StmtExp_e){ _LL788:
_temp787=(( struct Cyc_Absyn_StmtExp_e_struct*) _temp777)->f1; goto _LL780;}
else{ goto _LL781;} _LL781: if(*(( int*) _temp777) == Cyc_Absyn_Cast_e){ _LL792:
_temp791=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp777)->f1; goto _LL790;
_LL790: _temp789=(( struct Cyc_Absyn_Cast_e_struct*) _temp777)->f2; goto _LL782;}
else{ goto _LL783;} _LL783: if(*(( int*) _temp777) == Cyc_Absyn_StructMember_e){
_LL796: _temp795=(( struct Cyc_Absyn_StructMember_e_struct*) _temp777)->f1; goto
_LL794; _LL794: _temp793=(( struct Cyc_Absyn_StructMember_e_struct*) _temp777)->f2;
goto _LL784;} else{ goto _LL785;} _LL785: goto _LL786; _LL780: Cyc_Toc_lvalue_assign_stmt(
_temp787, fs, f, f_env); goto _LL778; _LL782: Cyc_Toc_lvalue_assign( _temp789,
fs, f, f_env); goto _LL778; _LL784:( void*)( e1->r=( void*)(( void*) _temp795->r));
Cyc_Toc_lvalue_assign( e1,({ struct Cyc_List_List* _temp797=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp797->hd=( void*) _temp793;
_temp797->tl= fs; _temp797;}), f, f_env); goto _LL778; _LL786: if( ! Cyc_Absyn_is_lvalue(
e1)){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({
struct _tagged_arr(* _temp798)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp802= _tag_arr("lvalue_assign: could not find lvalue in `%s'",
sizeof( unsigned char), 45u); struct Cyc_Stdio_String_pa_struct _temp803;
_temp803.tag= Cyc_Stdio_String_pa; _temp803.f1=( struct _tagged_arr) Cyc_Absynpp_exp2string(
e1);{ void* _temp801=( void*)& _temp803; void* _temp799[ 1u]={ _temp801}; struct
_tagged_arr _temp800={( void*) _temp799,( void*) _temp799,( void*)( _temp799 + 1u)};
_temp798( _temp802, _temp800);}}));}{ struct Cyc_Absyn_Exp* e1_copy= Cyc_Absyn_copy_exp(
e1); for( 0; fs != 0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){
e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( fs))->hd, e1_copy->loc);}( void*)( e1->r=( void*)(( void*)( f(
e1_copy, f_env))->r)); goto _LL778;} _LL778:;} static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env){ void* _temp804=( void*) s->r;
struct Cyc_Absyn_Exp* _temp814; struct Cyc_Absyn_Stmt* _temp816; struct Cyc_Absyn_Decl*
_temp818; struct Cyc_Absyn_Stmt* _temp820; _LL806: if(( unsigned int) _temp804 >
1u?*(( int*) _temp804) == Cyc_Absyn_Exp_s: 0){ _LL815: _temp814=(( struct Cyc_Absyn_Exp_s_struct*)
_temp804)->f1; goto _LL807;} else{ goto _LL808;} _LL808: if(( unsigned int)
_temp804 > 1u?*(( int*) _temp804) == Cyc_Absyn_Decl_s: 0){ _LL819: _temp818=((
struct Cyc_Absyn_Decl_s_struct*) _temp804)->f1; goto _LL817; _LL817: _temp816=((
struct Cyc_Absyn_Decl_s_struct*) _temp804)->f2; goto _LL809;} else{ goto _LL810;}
_LL810: if(( unsigned int) _temp804 > 1u?*(( int*) _temp804) == Cyc_Absyn_Seq_s:
0){ _LL821: _temp820=(( struct Cyc_Absyn_Seq_s_struct*) _temp804)->f2; goto
_LL811;} else{ goto _LL812;} _LL812: goto _LL813; _LL807: Cyc_Toc_lvalue_assign(
_temp814, fs, f, f_env); goto _LL805; _LL809: Cyc_Toc_lvalue_assign_stmt(
_temp816, fs, f, f_env); goto _LL805; _LL811: Cyc_Toc_lvalue_assign_stmt(
_temp820, fs, f, f_env); goto _LL805; _LL813:(( int(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct _tagged_arr(* _temp822)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp826= _tag_arr("lvalue_assign_stmt: %s", sizeof( unsigned char),
23u); struct Cyc_Stdio_String_pa_struct _temp827; _temp827.tag= Cyc_Stdio_String_pa;
_temp827.f1=( struct _tagged_arr) Cyc_Absynpp_stmt2string( s);{ void* _temp825=(
void*)& _temp827; void* _temp823[ 1u]={ _temp825}; struct _tagged_arr _temp824={(
void*) _temp823,( void*) _temp823,( void*)( _temp823 + 1u)}; _temp822( _temp826,
_temp824);}})); goto _LL805; _LL805:;} static struct Cyc_List_List* Cyc_Toc_rmap_2c(
struct _RegionHandle* r2, void*(* f)( void*, void*), void* env, struct Cyc_List_List*
x){ struct Cyc_List_List* result; struct Cyc_List_List* prev; if( x == 0){
return 0;} result=({ struct Cyc_List_List* _temp828=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp828->hd=( void*) f((
void*)(( struct Cyc_List_List*) _check_null( x))->hd, env); _temp828->tl= 0;
_temp828;}); prev= result; for( x=(( struct Cyc_List_List*) _check_null( x))->tl;
x != 0; x=(( struct Cyc_List_List*) _check_null( x))->tl){(( struct Cyc_List_List*)
_check_null( prev))->tl=({ struct Cyc_List_List* _temp829=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp829->hd=( void*) f((
void*)(( struct Cyc_List_List*) _check_null( x))->hd, env); _temp829->tl= 0;
_temp829;}); prev=(( struct Cyc_List_List*) _check_null( prev))->tl;} return
result;} static struct Cyc_List_List* Cyc_Toc_map_2c( void*(* f)( void*, void*),
void* env, struct Cyc_List_List* x){ return Cyc_Toc_rmap_2c( Cyc_Core_heap_region,
f, env, x);} static struct _tuple4* Cyc_Toc_make_dle( struct Cyc_Absyn_Exp* e){
return({ struct _tuple4* _temp830=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp830->f1= 0; _temp830->f2= e; _temp830;});} static struct Cyc_Absyn_PtrInfo
Cyc_Toc_get_ptr_type( void* t){ void* _temp831= Cyc_Tcutil_compress( t); struct
Cyc_Absyn_PtrInfo _temp837; _LL833: if(( unsigned int) _temp831 > 4u?*(( int*)
_temp831) == Cyc_Absyn_PointerType: 0){ _LL838: _temp837=(( struct Cyc_Absyn_PointerType_struct*)
_temp831)->f1; goto _LL834;} else{ goto _LL835;} _LL835: goto _LL836; _LL834:
return _temp837; _LL836:( int) _throw( Cyc_Tcutil_impos( _tag_arr("get_ptr_typ:  not a pointer!",
sizeof( unsigned char), 29u))); _LL832:;} struct _tuple8{ void* f1; void* f2; }
; struct _tuple9{ struct _tuple0* f1; void* f2; struct Cyc_Absyn_Exp* f3; } ;
struct _tuple10{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Exp* f2; } ;
static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e){
void* _temp839=( void*) e->r; if( e->topt == 0){(( int(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct _tagged_arr(* _temp840)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp844= _tag_arr("exp_to_c: no type for %s", sizeof( unsigned char),
25u); struct Cyc_Stdio_String_pa_struct _temp845; _temp845.tag= Cyc_Stdio_String_pa;
_temp845.f1=( struct _tagged_arr) Cyc_Absynpp_exp2string( e);{ void* _temp843=(
void*)& _temp845; void* _temp841[ 1u]={ _temp843}; struct _tagged_arr _temp842={(
void*) _temp841,( void*) _temp841,( void*)( _temp841 + 1u)}; _temp840( _temp844,
_temp842);}})); return;}{ void* old_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; void* _temp846= _temp839; void* _temp926; void*
_temp928; struct _tuple0* _temp930; struct _tuple0* _temp932; struct Cyc_List_List*
_temp934; void* _temp936; void* _temp938; struct Cyc_Absyn_Exp* _temp940; struct
Cyc_Absyn_Exp* _temp942; struct Cyc_Core_Opt* _temp944; struct Cyc_Absyn_Exp*
_temp946; struct Cyc_Absyn_Exp* _temp948; struct Cyc_Absyn_Exp* _temp950; struct
Cyc_Absyn_Exp* _temp952; struct Cyc_Absyn_Exp* _temp954; struct Cyc_Absyn_Exp*
_temp956; struct Cyc_List_List* _temp958; struct Cyc_Absyn_Exp* _temp960; struct
Cyc_Absyn_VarargCallInfo* _temp962; struct Cyc_List_List* _temp964; struct Cyc_Absyn_Exp*
_temp966; struct Cyc_Absyn_VarargCallInfo* _temp968; struct Cyc_Absyn_VarargCallInfo
_temp970; struct Cyc_Absyn_VarargInfo* _temp971; struct Cyc_List_List* _temp973;
int _temp975; struct Cyc_List_List* _temp977; struct Cyc_Absyn_Exp* _temp979;
struct Cyc_Absyn_Exp* _temp981; struct Cyc_Absyn_Exp* _temp983; struct Cyc_List_List*
_temp985; struct Cyc_Absyn_Exp* _temp987; struct Cyc_Absyn_Exp* _temp989; void*
_temp991; void** _temp993; struct Cyc_Absyn_Exp* _temp994; struct Cyc_Absyn_Exp*
_temp996; struct Cyc_Absyn_Exp* _temp998; struct Cyc_Absyn_Exp* _temp1000; void*
_temp1002; struct _tagged_arr* _temp1004; void* _temp1006; struct Cyc_Absyn_Exp*
_temp1008; struct _tagged_arr* _temp1010; struct Cyc_Absyn_Exp* _temp1012;
struct _tagged_arr* _temp1014; struct Cyc_Absyn_Exp* _temp1016; struct Cyc_Absyn_Exp*
_temp1018; struct Cyc_Absyn_Exp* _temp1020; struct Cyc_List_List* _temp1022;
struct Cyc_List_List* _temp1024; struct Cyc_Absyn_Exp* _temp1026; struct Cyc_Absyn_Exp*
_temp1028; struct Cyc_Absyn_Vardecl* _temp1030; struct Cyc_Absyn_Structdecl*
_temp1032; struct Cyc_List_List* _temp1034; struct Cyc_Core_Opt* _temp1036;
struct _tuple0* _temp1038; struct Cyc_List_List* _temp1040; void* _temp1042;
struct Cyc_Absyn_Tunionfield* _temp1044; struct Cyc_Absyn_Tuniondecl* _temp1046;
struct Cyc_List_List* _temp1048; struct Cyc_Absyn_Tunionfield* _temp1050; struct
Cyc_Absyn_Tuniondecl* _temp1052; struct Cyc_List_List* _temp1054; void*
_temp1056; struct Cyc_Absyn_Exp* _temp1058; struct Cyc_Absyn_Stmt* _temp1060;
struct Cyc_Absyn_Fndecl* _temp1062; _LL848: if(*(( int*) _temp846) == Cyc_Absyn_Const_e){
_LL927: _temp926=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp846)->f1; if(
_temp926 ==( void*) Cyc_Absyn_Null_c){ goto _LL849;} else{ goto _LL850;}} else{
goto _LL850;} _LL850: if(*(( int*) _temp846) == Cyc_Absyn_Const_e){ goto _LL851;}
else{ goto _LL852;} _LL852: if(*(( int*) _temp846) == Cyc_Absyn_Var_e){ _LL931:
_temp930=(( struct Cyc_Absyn_Var_e_struct*) _temp846)->f1; goto _LL929; _LL929:
_temp928=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp846)->f2; goto _LL853;}
else{ goto _LL854;} _LL854: if(*(( int*) _temp846) == Cyc_Absyn_UnknownId_e){
_LL933: _temp932=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp846)->f1; goto
_LL855;} else{ goto _LL856;} _LL856: if(*(( int*) _temp846) == Cyc_Absyn_Primop_e){
_LL937: _temp936=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp846)->f1;
goto _LL935; _LL935: _temp934=(( struct Cyc_Absyn_Primop_e_struct*) _temp846)->f2;
goto _LL857;} else{ goto _LL858;} _LL858: if(*(( int*) _temp846) == Cyc_Absyn_Increment_e){
_LL941: _temp940=(( struct Cyc_Absyn_Increment_e_struct*) _temp846)->f1; goto
_LL939; _LL939: _temp938=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp846)->f2; goto _LL859;} else{ goto _LL860;} _LL860: if(*(( int*) _temp846)
== Cyc_Absyn_AssignOp_e){ _LL947: _temp946=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp846)->f1; goto _LL945; _LL945: _temp944=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp846)->f2; goto _LL943; _LL943: _temp942=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp846)->f3; goto _LL861;} else{ goto _LL862;} _LL862: if(*(( int*) _temp846)
== Cyc_Absyn_Conditional_e){ _LL953: _temp952=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp846)->f1; goto _LL951; _LL951: _temp950=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp846)->f2; goto _LL949; _LL949: _temp948=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp846)->f3; goto _LL863;} else{ goto _LL864;} _LL864: if(*(( int*) _temp846)
== Cyc_Absyn_SeqExp_e){ _LL957: _temp956=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp846)->f1; goto _LL955; _LL955: _temp954=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp846)->f2; goto _LL865;} else{ goto _LL866;} _LL866: if(*(( int*) _temp846)
== Cyc_Absyn_UnknownCall_e){ _LL961: _temp960=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp846)->f1; goto _LL959; _LL959: _temp958=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp846)->f2; goto _LL867;} else{ goto _LL868;} _LL868: if(*(( int*) _temp846)
== Cyc_Absyn_FnCall_e){ _LL967: _temp966=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp846)->f1; goto _LL965; _LL965: _temp964=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp846)->f2; goto _LL963; _LL963: _temp962=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp846)->f3; if( _temp962 == 0){ goto _LL869;} else{ goto _LL870;}} else{ goto
_LL870;} _LL870: if(*(( int*) _temp846) == Cyc_Absyn_FnCall_e){ _LL980: _temp979=((
struct Cyc_Absyn_FnCall_e_struct*) _temp846)->f1; goto _LL978; _LL978: _temp977=((
struct Cyc_Absyn_FnCall_e_struct*) _temp846)->f2; goto _LL969; _LL969: _temp968=((
struct Cyc_Absyn_FnCall_e_struct*) _temp846)->f3; if( _temp968 == 0){ goto
_LL872;} else{ _temp970=* _temp968; _LL976: _temp975= _temp970.num_varargs; goto
_LL974; _LL974: _temp973= _temp970.injectors; goto _LL972; _LL972: _temp971=
_temp970.vai; goto _LL871;}} else{ goto _LL872;} _LL872: if(*(( int*) _temp846)
== Cyc_Absyn_Throw_e){ _LL982: _temp981=(( struct Cyc_Absyn_Throw_e_struct*)
_temp846)->f1; goto _LL873;} else{ goto _LL874;} _LL874: if(*(( int*) _temp846)
== Cyc_Absyn_NoInstantiate_e){ _LL984: _temp983=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp846)->f1; goto _LL875;} else{ goto _LL876;} _LL876: if(*(( int*) _temp846)
== Cyc_Absyn_Instantiate_e){ _LL988: _temp987=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp846)->f1; goto _LL986; _LL986: _temp985=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp846)->f2; goto _LL877;} else{ goto _LL878;} _LL878: if(*(( int*) _temp846)
== Cyc_Absyn_Cast_e){ _LL992: _temp991=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp846)->f1; _temp993=&(( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp846)->f1);
goto _LL990; _LL990: _temp989=(( struct Cyc_Absyn_Cast_e_struct*) _temp846)->f2;
goto _LL879;} else{ goto _LL880;} _LL880: if(*(( int*) _temp846) == Cyc_Absyn_Address_e){
_LL995: _temp994=(( struct Cyc_Absyn_Address_e_struct*) _temp846)->f1; goto
_LL881;} else{ goto _LL882;} _LL882: if(*(( int*) _temp846) == Cyc_Absyn_New_e){
_LL999: _temp998=(( struct Cyc_Absyn_New_e_struct*) _temp846)->f1; goto _LL997;
_LL997: _temp996=(( struct Cyc_Absyn_New_e_struct*) _temp846)->f2; goto _LL883;}
else{ goto _LL884;} _LL884: if(*(( int*) _temp846) == Cyc_Absyn_Sizeofexp_e){
_LL1001: _temp1000=(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp846)->f1; goto
_LL885;} else{ goto _LL886;} _LL886: if(*(( int*) _temp846) == Cyc_Absyn_Sizeoftyp_e){
_LL1003: _temp1002=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp846)->f1;
goto _LL887;} else{ goto _LL888;} _LL888: if(*(( int*) _temp846) == Cyc_Absyn_Offsetof_e){
_LL1007: _temp1006=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp846)->f1;
goto _LL1005; _LL1005: _temp1004=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp846)->f2; goto _LL889;} else{ goto _LL890;} _LL890: if(*(( int*) _temp846)
== Cyc_Absyn_Deref_e){ _LL1009: _temp1008=(( struct Cyc_Absyn_Deref_e_struct*)
_temp846)->f1; goto _LL891;} else{ goto _LL892;} _LL892: if(*(( int*) _temp846)
== Cyc_Absyn_StructMember_e){ _LL1013: _temp1012=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp846)->f1; goto _LL1011; _LL1011: _temp1010=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp846)->f2; goto _LL893;} else{ goto _LL894;} _LL894: if(*(( int*) _temp846)
== Cyc_Absyn_StructArrow_e){ _LL1017: _temp1016=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp846)->f1; goto _LL1015; _LL1015: _temp1014=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp846)->f2; goto _LL895;} else{ goto _LL896;} _LL896: if(*(( int*) _temp846)
== Cyc_Absyn_Subscript_e){ _LL1021: _temp1020=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp846)->f1; goto _LL1019; _LL1019: _temp1018=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp846)->f2; goto _LL897;} else{ goto _LL898;} _LL898: if(*(( int*) _temp846)
== Cyc_Absyn_Tuple_e){ _LL1023: _temp1022=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp846)->f1; goto _LL899;} else{ goto _LL900;} _LL900: if(*(( int*) _temp846)
== Cyc_Absyn_Array_e){ _LL1025: _temp1024=(( struct Cyc_Absyn_Array_e_struct*)
_temp846)->f1; goto _LL901;} else{ goto _LL902;} _LL902: if(*(( int*) _temp846)
== Cyc_Absyn_Comprehension_e){ _LL1031: _temp1030=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp846)->f1; goto _LL1029; _LL1029: _temp1028=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp846)->f2; goto _LL1027; _LL1027: _temp1026=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp846)->f3; goto _LL903;} else{ goto _LL904;} _LL904: if(*(( int*) _temp846)
== Cyc_Absyn_Struct_e){ _LL1039: _temp1038=(( struct Cyc_Absyn_Struct_e_struct*)
_temp846)->f1; goto _LL1037; _LL1037: _temp1036=(( struct Cyc_Absyn_Struct_e_struct*)
_temp846)->f2; goto _LL1035; _LL1035: _temp1034=(( struct Cyc_Absyn_Struct_e_struct*)
_temp846)->f3; goto _LL1033; _LL1033: _temp1032=(( struct Cyc_Absyn_Struct_e_struct*)
_temp846)->f4; goto _LL905;} else{ goto _LL906;} _LL906: if(*(( int*) _temp846)
== Cyc_Absyn_AnonStruct_e){ _LL1043: _temp1042=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp846)->f1; goto _LL1041; _LL1041: _temp1040=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp846)->f2; goto _LL907;} else{ goto _LL908;} _LL908: if(*(( int*) _temp846)
== Cyc_Absyn_Tunion_e){ _LL1049: _temp1048=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp846)->f3; if( _temp1048 == 0){ goto _LL1047;} else{ goto _LL910;} _LL1047:
_temp1046=(( struct Cyc_Absyn_Tunion_e_struct*) _temp846)->f4; goto _LL1045;
_LL1045: _temp1044=(( struct Cyc_Absyn_Tunion_e_struct*) _temp846)->f5; goto
_LL909;} else{ goto _LL910;} _LL910: if(*(( int*) _temp846) == Cyc_Absyn_Tunion_e){
_LL1055: _temp1054=(( struct Cyc_Absyn_Tunion_e_struct*) _temp846)->f3; goto
_LL1053; _LL1053: _temp1052=(( struct Cyc_Absyn_Tunion_e_struct*) _temp846)->f4;
goto _LL1051; _LL1051: _temp1050=(( struct Cyc_Absyn_Tunion_e_struct*) _temp846)->f5;
goto _LL911;} else{ goto _LL912;} _LL912: if(*(( int*) _temp846) == Cyc_Absyn_Enum_e){
goto _LL913;} else{ goto _LL914;} _LL914: if(*(( int*) _temp846) == Cyc_Absyn_Malloc_e){
_LL1059: _temp1058=(( struct Cyc_Absyn_Malloc_e_struct*) _temp846)->f1; goto
_LL1057; _LL1057: _temp1056=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp846)->f2; goto _LL915;} else{ goto _LL916;} _LL916: if(*(( int*) _temp846)
== Cyc_Absyn_StmtExp_e){ _LL1061: _temp1060=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp846)->f1; goto _LL917;} else{ goto _LL918;} _LL918: if(*(( int*) _temp846)
== Cyc_Absyn_UnresolvedMem_e){ goto _LL919;} else{ goto _LL920;} _LL920: if(*((
int*) _temp846) == Cyc_Absyn_CompoundLit_e){ goto _LL921;} else{ goto _LL922;}
_LL922: if(*(( int*) _temp846) == Cyc_Absyn_Codegen_e){ _LL1063: _temp1062=((
struct Cyc_Absyn_Codegen_e_struct*) _temp846)->f1; goto _LL923;} else{ goto
_LL924;} _LL924: if(*(( int*) _temp846) == Cyc_Absyn_Fill_e){ goto _LL925;}
else{ goto _LL847;} _LL849: { struct Cyc_Absyn_Exp* _temp1064= Cyc_Absyn_uint_exp(
0, 0); if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){ if( nv->toplevel){( void*)(
e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr( old_typ, _temp1064,
_temp1064))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_List_List*(* _temp1065)( struct _tagged_arr)=(
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp*
_temp1068= _temp1064; struct Cyc_Absyn_Exp* _temp1069= _temp1064; struct Cyc_Absyn_Exp*
_temp1070= _temp1064; struct Cyc_Absyn_Exp* _temp1066[ 3u]={ _temp1068,
_temp1069, _temp1070}; struct _tagged_arr _temp1067={( void*) _temp1066,( void*)
_temp1066,( void*)( _temp1066 + 3u)}; _temp1065( _temp1067);}), 0))->r));}}
else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp( 0, 0))->r));}
goto _LL847;} _LL851: goto _LL847; _LL853:{ struct _handler_cons _temp1071;
_push_handler(& _temp1071);{ int _temp1073= 0; if( setjmp( _temp1071.handler)){
_temp1073= 1;} if( ! _temp1073){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( nv->varmap,
_temp930))->r));; _pop_handler();} else{ void* _temp1072=( void*) _exn_thrown;
void* _temp1075= _temp1072; _LL1077: if( _temp1075 == Cyc_Dict_Absent){ goto
_LL1078;} else{ goto _LL1079;} _LL1079: goto _LL1080; _LL1078:(( int(*)( struct
_tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct _tagged_arr(*
_temp1081)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1085= _tag_arr("Can't find %s in exp_to_c, Var\n",
sizeof( unsigned char), 32u); struct Cyc_Stdio_String_pa_struct _temp1086;
_temp1086.tag= Cyc_Stdio_String_pa; _temp1086.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp930);{ void* _temp1084=( void*)& _temp1086; void* _temp1082[ 1u]={
_temp1084}; struct _tagged_arr _temp1083={( void*) _temp1082,( void*) _temp1082,(
void*)( _temp1082 + 1u)}; _temp1081( _temp1085, _temp1083);}})); return; _LL1080:(
void) _throw( _temp1075); _LL1076:;}}} goto _LL847; _LL855:(( int(*)( struct
_tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknownid", sizeof( unsigned char),
10u)); goto _LL847; _LL857: { struct Cyc_List_List* _temp1087=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ,
_temp934); struct Cyc_List_List* _temp1088=(( struct Cyc_List_List*(*)( void*(*
f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ,
_temp934);(( void(*)( void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*),
struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c,
nv, _temp934);{ void* _temp1089= _temp936; _LL1091: if( _temp1089 ==( void*) Cyc_Absyn_Size){
goto _LL1092;} else{ goto _LL1093;} _LL1093: if( _temp1089 ==( void*) Cyc_Absyn_Plus){
goto _LL1094;} else{ goto _LL1095;} _LL1095: if( _temp1089 ==( void*) Cyc_Absyn_Minus){
goto _LL1096;} else{ goto _LL1097;} _LL1097: if( _temp1089 ==( void*) Cyc_Absyn_Eq){
goto _LL1098;} else{ goto _LL1099;} _LL1099: if( _temp1089 ==( void*) Cyc_Absyn_Neq){
goto _LL1100;} else{ goto _LL1101;} _LL1101: if( _temp1089 ==( void*) Cyc_Absyn_Gt){
goto _LL1102;} else{ goto _LL1103;} _LL1103: if( _temp1089 ==( void*) Cyc_Absyn_Gte){
goto _LL1104;} else{ goto _LL1105;} _LL1105: if( _temp1089 ==( void*) Cyc_Absyn_Lt){
goto _LL1106;} else{ goto _LL1107;} _LL1107: if( _temp1089 ==( void*) Cyc_Absyn_Lte){
goto _LL1108;} else{ goto _LL1109;} _LL1109: goto _LL1110; _LL1092: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp934))->hd;{
void* _temp1111=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v;
struct Cyc_Absyn_Exp* _temp1119; struct Cyc_Absyn_PtrInfo _temp1121; struct Cyc_Absyn_Conref*
_temp1123; void* _temp1125; _LL1113: if(( unsigned int) _temp1111 > 4u?*(( int*)
_temp1111) == Cyc_Absyn_ArrayType: 0){ _LL1120: _temp1119=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1111)->f3; goto _LL1114;} else{ goto _LL1115;} _LL1115: if(( unsigned int)
_temp1111 > 4u?*(( int*) _temp1111) == Cyc_Absyn_PointerType: 0){ _LL1122:
_temp1121=(( struct Cyc_Absyn_PointerType_struct*) _temp1111)->f1; _LL1126:
_temp1125=( void*) _temp1121.elt_typ; goto _LL1124; _LL1124: _temp1123=
_temp1121.bounds; goto _LL1116;} else{ goto _LL1117;} _LL1117: goto _LL1118;
_LL1114:( void*)( e->r=( void*)(( void*)(( struct Cyc_Absyn_Exp*) _check_null(
_temp1119))->r)); goto _LL1112; _LL1116:{ void* _temp1127= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1123); struct Cyc_Absyn_Exp* _temp1133; _LL1129: if(
_temp1127 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1130;} else{ goto _LL1131;}
_LL1131: if(( unsigned int) _temp1127 > 1u?*(( int*) _temp1127) == Cyc_Absyn_Upper_b:
0){ _LL1134: _temp1133=(( struct Cyc_Absyn_Upper_b_struct*) _temp1127)->f1; goto
_LL1132;} else{ goto _LL1128;} _LL1130:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__get_arr_size_e,({ struct Cyc_List_List*(* _temp1135)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1138=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp934))->hd; struct Cyc_Absyn_Exp* _temp1139= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( _temp1125), 0); struct Cyc_Absyn_Exp* _temp1136[ 2u]={
_temp1138, _temp1139}; struct _tagged_arr _temp1137={( void*) _temp1136,( void*)
_temp1136,( void*)( _temp1136 + 2u)}; _temp1135( _temp1137);}), 0))->r)); goto
_LL1128; _LL1132:( void*)( e->r=( void*)(( void*) _temp1133->r)); goto _LL1128;
_LL1128:;} goto _LL1112; _LL1118:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr(* _temp1140)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp1145= _tag_arr("size primop applied to non-array %s (%s)",
sizeof( unsigned char), 41u); struct Cyc_Stdio_String_pa_struct _temp1147;
_temp1147.tag= Cyc_Stdio_String_pa; _temp1147.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_List_List*) _check_null( _temp1088))->hd);{ void* _temp1143=(
void*)& _temp1147; struct Cyc_Stdio_String_pa_struct _temp1146; _temp1146.tag=
Cyc_Stdio_String_pa; _temp1146.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v);{ void* _temp1144=(
void*)& _temp1146; void* _temp1141[ 2u]={ _temp1143, _temp1144}; struct
_tagged_arr _temp1142={( void*) _temp1141,( void*) _temp1141,( void*)( _temp1141
+ 2u)}; _temp1140( _temp1145, _temp1142);}}})); return; _LL1112:;} goto _LL1090;}
_LL1094: { void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt((
void*)(( struct Cyc_List_List*) _check_null( _temp1087))->hd,& elt_typ)){ struct
Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp934))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp934))->tl))->hd;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__tagged_arr_plus_e,({
struct Cyc_List_List*(* _temp1148)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1151= e1; struct
Cyc_Absyn_Exp* _temp1152= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0);
struct Cyc_Absyn_Exp* _temp1153= e2; struct Cyc_Absyn_Exp* _temp1149[ 3u]={
_temp1151, _temp1152, _temp1153}; struct _tagged_arr _temp1150={( void*)
_temp1149,( void*) _temp1149,( void*)( _temp1149 + 3u)}; _temp1148( _temp1150);}),
0))->r));} goto _LL1090;} _LL1096: { void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt(( void*)(( struct Cyc_List_List*)
_check_null( _temp1087))->hd,& elt_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp934))->hd; struct Cyc_Absyn_Exp* e2=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp934))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(( void*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1087))->tl))->hd)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0),
Cyc_Toc_curr_sp, 0))->r));( void*)( e->r=( void*)(( void*)( Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Div, Cyc_Absyn_copy_exp( e), Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(
elt_typ), 0), 0))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_plus_e,({ struct Cyc_List_List*(* _temp1154)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1157= e1; struct Cyc_Absyn_Exp* _temp1158= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( elt_typ), 0); struct Cyc_Absyn_Exp* _temp1159= Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Minus, e2, 0); struct Cyc_Absyn_Exp* _temp1155[ 3u]={ _temp1157,
_temp1158, _temp1159}; struct _tagged_arr _temp1156={( void*) _temp1155,( void*)
_temp1155,( void*)( _temp1155 + 3u)}; _temp1154( _temp1156);}), 0))->r));}} goto
_LL1090;} _LL1098: goto _LL1100; _LL1100: goto _LL1102; _LL1102: goto _LL1104;
_LL1104: goto _LL1106; _LL1106: goto _LL1108; _LL1108: { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp934))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp934))->tl))->hd; void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp1087))->hd; void* t2=(
void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1087))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){(
void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1090;} _LL1110: goto
_LL1090; _LL1090:;} goto _LL847;} _LL859: { void* e2_cyc_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp940->topt))->v; Cyc_Toc_exp_to_c( nv, _temp940);{
void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){ struct Cyc_Absyn_Exp* _temp1160=( _temp938 ==( void*) Cyc_Absyn_PostInc?
1: _temp938 ==( void*) Cyc_Absyn_PostDec)? Cyc_Toc__tagged_arr_inplace_plus_post_e:
Cyc_Toc__tagged_arr_inplace_plus_e; int _temp1161=( _temp938 ==( void*) Cyc_Absyn_PreInc?
1: _temp938 ==( void*) Cyc_Absyn_PostInc)? 1: - 1;( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_fncall_exp( _temp1160,({ struct Cyc_List_List*(* _temp1162)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1165= Cyc_Absyn_address_exp( _temp940, 0); struct Cyc_Absyn_Exp*
_temp1166= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0); struct Cyc_Absyn_Exp*
_temp1167= Cyc_Absyn_signed_int_exp( _temp1161, 0); struct Cyc_Absyn_Exp*
_temp1163[ 3u]={ _temp1165, _temp1166, _temp1167}; struct _tagged_arr _temp1164={(
void*) _temp1163,( void*) _temp1163,( void*)( _temp1163 + 3u)}; _temp1162(
_temp1164);}), 0))->r));} if( elt_typ ==( void*) Cyc_Absyn_VoidType? ! Cyc_Absyn_is_lvalue(
_temp940): 0){ Cyc_Toc_lvalue_assign( _temp940, 0, Cyc_Toc_incr_lvalue, _temp938);(
void*)( e->r=( void*)(( void*) _temp940->r));} goto _LL847;}} _LL861: { int
e1_poly= Cyc_Toc_is_poly_project( _temp946); void* e1_old_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp946->topt))->v; void* e2_old_typ=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp942->topt))->v; Cyc_Toc_exp_to_c( nv,
_temp946); Cyc_Toc_exp_to_c( nv, _temp942);{ int done= 0; if( _temp944 != 0){
void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){ struct Cyc_Absyn_Exp* change;{ void* _temp1168=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp944))->v; _LL1170: if( _temp1168 ==(
void*) Cyc_Absyn_Plus){ goto _LL1171;} else{ goto _LL1172;} _LL1172: if(
_temp1168 ==( void*) Cyc_Absyn_Minus){ goto _LL1173;} else{ goto _LL1174;}
_LL1174: goto _LL1175; _LL1171: change= _temp942; goto _LL1169; _LL1173: change=
Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Minus, _temp942, 0); goto _LL1169;
_LL1175:( int) _throw( Cyc_Tcutil_impos( _tag_arr("bad t ? pointer arithmetic",
sizeof( unsigned char), 27u))); goto _LL1169; _LL1169:;} done= 1;( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__tagged_arr_inplace_plus_e,({
struct Cyc_List_List*(* _temp1176)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1179= Cyc_Absyn_address_exp(
_temp946, 0); struct Cyc_Absyn_Exp* _temp1180= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(
elt_typ), 0); struct Cyc_Absyn_Exp* _temp1181= change; struct Cyc_Absyn_Exp*
_temp1177[ 3u]={ _temp1179, _temp1180, _temp1181}; struct _tagged_arr _temp1178={(
void*) _temp1177,( void*) _temp1177,( void*)( _temp1177 + 3u)}; _temp1176(
_temp1178);}), 0))->r));}} if( ! done){ if( e1_poly){( void*)( _temp942->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp((
void*) _temp942->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue( _temp946)){(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, struct _tuple7*), struct _tuple7* f_env)) Cyc_Toc_lvalue_assign)(
_temp946, 0, Cyc_Toc_assignop_lvalue,({ struct _tuple7* _temp1182=( struct
_tuple7*) GC_malloc( sizeof( struct _tuple7) * 1); _temp1182[ 0]=({ struct
_tuple7 _temp1183; _temp1183.f1= _temp944; _temp1183.f2= _temp942; _temp1183;});
_temp1182;}));( void*)( e->r=( void*)(( void*) _temp946->r));}} goto _LL847;}}
_LL863: Cyc_Toc_exp_to_c( nv, _temp952); Cyc_Toc_exp_to_c( nv, _temp950); Cyc_Toc_exp_to_c(
nv, _temp948); goto _LL847; _LL865: Cyc_Toc_exp_to_c( nv, _temp956); Cyc_Toc_exp_to_c(
nv, _temp954); goto _LL847; _LL867: _temp966= _temp960; _temp964= _temp958; goto
_LL869; _LL869: Cyc_Toc_exp_to_c( nv, _temp966);(( void(*)( void(* f)( struct
Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp964); goto _LL847; _LL871:{
struct _RegionHandle _temp1184= _new_region(); struct _RegionHandle* r=&
_temp1184; _push_region( r);{ void* _temp1185=( void*) _temp971->type; void*
_temp1186= Cyc_Toc_typ_to_c( _temp1185); void* _temp1187= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp979->topt))->v); struct Cyc_Absyn_Exp*
_temp1188= Cyc_Absyn_uint_exp(( unsigned int) _temp975, 0); void* arr_type= Cyc_Toc_typ_to_c((
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp1237=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1237[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1238; _temp1238.tag= Cyc_Absyn_ArrayType;
_temp1238.f1=( void*) _temp1185; _temp1238.f2= Cyc_Toc_mt_tq; _temp1238.f3=(
struct Cyc_Absyn_Exp*) _temp1188; _temp1238;}); _temp1237;})); struct _tuple0*
_temp1189= Cyc_Toc_temp_var(); struct _tuple0* _temp1190= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* _temp1191= Cyc_Absyn_var_exp( _temp1190, 0); struct
_tuple0* _temp1192= Cyc_Toc_temp_var(); int _temp1193=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp977); int _temp1194= _temp1193 - _temp975; struct
_tagged_arr vs=({ unsigned int _temp1233=( unsigned int) _temp975; struct
_tuple0** _temp1234=( struct _tuple0**) _region_malloc( r, sizeof( struct
_tuple0*) * _temp1233); struct _tagged_arr _temp1236= _tag_arr( _temp1234,
sizeof( struct _tuple0*),( unsigned int) _temp975);{ unsigned int _temp1235=
_temp1233; unsigned int i; for( i= 0; i < _temp1235; i ++){ _temp1234[ i]= Cyc_Toc_temp_var();}};
_temp1236;}); struct _tagged_arr xs=({ unsigned int _temp1229=( unsigned int)
_temp1194; struct _tuple0** _temp1230=( struct _tuple0**) _region_malloc( r,
sizeof( struct _tuple0*) * _temp1229); struct _tagged_arr _temp1232= _tag_arr(
_temp1230, sizeof( struct _tuple0*),( unsigned int) _temp1194);{ unsigned int
_temp1231= _temp1229; unsigned int i; for( i= 0; i < _temp1231; i ++){ _temp1230[
i]= Cyc_Toc_temp_var();}}; _temp1232;}); struct Cyc_List_List* xexps=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple0*, struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_Toc_map_2c)( Cyc_Absyn_var_exp,(
struct Cyc_Position_Segment*) 0,(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, struct _tagged_arr arr)) Cyc_List_rfrom_array)( r, xs));
xexps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( xexps,({ struct Cyc_List_List* _temp1195=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1195->hd=( void*) Cyc_Absyn_var_exp(
_temp1192, 0); _temp1195->tl= 0; _temp1195;}));{ struct Cyc_Absyn_Stmt*
_temp1196= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(
_temp1189, 0), xexps, 0), 0); struct Cyc_Absyn_Exp* _temp1197= Cyc_Absyn_add_exp(
_temp1191, _temp1188, 0); struct Cyc_List_List* _temp1198=({ struct Cyc_List_List*(*
_temp1220)( struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list; struct _tuple4* _temp1223=({ struct _tuple4* _temp1228=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1228->f1= 0; _temp1228->f2=
Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), _temp1191, 0); _temp1228;});
struct _tuple4* _temp1224=({ struct _tuple4* _temp1227=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp1227->f1= 0; _temp1227->f2= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp1191, 0); _temp1227;}); struct _tuple4*
_temp1225=({ struct _tuple4* _temp1226=( struct _tuple4*) GC_malloc( sizeof(
struct _tuple4)); _temp1226->f1= 0; _temp1226->f2= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1197, 0); _temp1226;}); struct _tuple4* _temp1221[ 3u]={ _temp1223,
_temp1224, _temp1225}; struct _tagged_arr _temp1222={( void*) _temp1221,( void*)
_temp1221,( void*)( _temp1221 + 3u)}; _temp1220( _temp1222);}); _temp1196= Cyc_Absyn_declare_stmt(
_temp1192, Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ( _temp1185,( void*) _temp971->rgn,
Cyc_Toc_mt_tq)),( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp( 0,
_temp1198, 0), _temp1196, 0);{ struct Cyc_List_List* _temp1199=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct Cyc_Absyn_Exp*(* f)( struct _tuple0*, struct
Cyc_Position_Segment*), struct Cyc_Position_Segment* env, struct Cyc_List_List*
x)) Cyc_Toc_rmap_2c)( r, Cyc_Absyn_var_exp, 0,(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct _tagged_arr arr)) Cyc_List_rfrom_array)( r, vs));
struct Cyc_List_List* _temp1200=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_make_dle,
_temp1199); _temp1196= Cyc_Absyn_declare_stmt( _temp1190, arr_type,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_unresolvedmem_exp( 0, _temp1200, 0), _temp1196, 0);{ struct Cyc_List_List*
_temp1201=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( r, _temp977); struct Cyc_List_List* _temp1202=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, _temp973);{
int i= _temp975 - 1; for( 0; i >= 0;( i --, _temp1201=(( struct Cyc_List_List*)
_check_null( _temp1201))->tl)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1201))->hd; void* arg_type=( void*)((
struct Cyc_Core_Opt*) _check_null( arg->topt))->v; Cyc_Toc_exp_to_c( nv, arg);
if( _temp971->inject){ void* _temp1205; struct Cyc_Position_Segment* _temp1207;
struct Cyc_List_List* _temp1209; struct Cyc_List_List* _temp1211; struct _tuple0*
_temp1213; struct Cyc_Absyn_Tunionfield _temp1203=*(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1202))->hd); _LL1214: _temp1213=
_temp1203.name; goto _LL1212; _LL1212: _temp1211= _temp1203.tvs; goto _LL1210;
_LL1210: _temp1209= _temp1203.typs; goto _LL1208; _LL1208: _temp1207= _temp1203.loc;
goto _LL1206; _LL1206: _temp1205=( void*) _temp1203.sc; goto _LL1204; _LL1204:
_temp1202=(( struct Cyc_List_List*) _check_null( _temp1202))->tl;{ struct
_tuple0* _temp1215= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp1216= Cyc_Absyn_var_exp(
_temp1215, 0); void* _temp1217= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp1213, _tag_arr("_struct", sizeof( unsigned char), 8u))); _temp1196= Cyc_Absyn_declare_stmt(*((
struct _tuple0**) _check_unknown_subscript( vs, sizeof( struct _tuple0*), i)),
_temp1186,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( _temp1186, Cyc_Absyn_address_exp(
_temp1216, 0), 0), _temp1196, 0);{ void* field_typ= Cyc_Toc_typ_to_c((*(( struct
_tuple3*)(( struct Cyc_List_List*) _check_null( _temp1209))->hd)).f2); if( Cyc_Toc_is_void_star(
field_typ)){ arg= Cyc_Absyn_cast_exp( field_typ, arg, 0);}{ struct Cyc_Absyn_Stmt*
_temp1218= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1216, Cyc_Toc_fieldname(
1), 0), arg, 0); _temp1196= Cyc_Absyn_seq_stmt( _temp1218, _temp1196, 0);{
struct Cyc_Absyn_Stmt* _temp1219= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
_temp1216, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( _temp1213, 0), 0); _temp1196=
Cyc_Absyn_seq_stmt( _temp1219, _temp1196, 0); _temp1196= Cyc_Absyn_declare_stmt(
_temp1215, _temp1217, 0, _temp1196, 0);}}}}} else{ _temp1196= Cyc_Absyn_declare_stmt(*((
struct _tuple0**) _check_unknown_subscript( vs, sizeof( struct _tuple0*), i)),
_temp1186,( struct Cyc_Absyn_Exp*) arg, _temp1196, 0);}}}{ int i= _temp1194 - 1;
for( 0; i >= 0;( i --, _temp1201=(( struct Cyc_List_List*) _check_null(
_temp1201))->tl)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1201))->hd; void* argtype= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v); Cyc_Toc_exp_to_c(
nv, arg); _temp1196= Cyc_Absyn_declare_stmt(*(( struct _tuple0**)
_check_unknown_subscript( xs, sizeof( struct _tuple0*), i)), argtype,( struct
Cyc_Absyn_Exp*) arg, _temp1196, 0);}} Cyc_Toc_exp_to_c( nv, _temp979); _temp1196=
Cyc_Absyn_declare_stmt( _temp1189, _temp1187,( struct Cyc_Absyn_Exp*) _temp979,
_temp1196, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( _temp1196, 0))->r));}}}};
_pop_region( r);} goto _LL847; _LL873: Cyc_Toc_exp_to_c( nv, _temp981);( void*)(
e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c( old_typ),
Cyc_Toc_newthrow_exp( _temp981), 0))->r)); goto _LL847; _LL875: Cyc_Toc_exp_to_c(
nv, _temp983); goto _LL847; _LL877: Cyc_Toc_exp_to_c( nv, _temp987); for( 0;
_temp985 != 0; _temp985=(( struct Cyc_List_List*) _check_null( _temp985))->tl){
void* k= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*) _check_null(
_temp985))->hd); if( k !=( void*) Cyc_Absyn_EffKind? k !=( void*) Cyc_Absyn_RgnKind:
0){{ void* _temp1239= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*)
_check_null( _temp985))->hd); _LL1241: if(( unsigned int) _temp1239 > 4u?*(( int*)
_temp1239) == Cyc_Absyn_VarType: 0){ goto _LL1242;} else{ goto _LL1243;} _LL1243:
if(( unsigned int) _temp1239 > 4u?*(( int*) _temp1239) == Cyc_Absyn_TunionType:
0){ goto _LL1244;} else{ goto _LL1245;} _LL1245: goto _LL1246; _LL1242:
continue; _LL1244: continue; _LL1246:( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp987, 0))->r)); goto _LL1240; _LL1240:;} break;}} goto _LL847; _LL879: {
void* old_t2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp989->topt))->v;
void* new_typ=* _temp993;* _temp993= Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c(
nv, _temp989);{ struct _tuple8 _temp1248=({ struct _tuple8 _temp1247; _temp1247.f1=
Cyc_Tcutil_compress( old_t2); _temp1247.f2= Cyc_Tcutil_compress( new_typ);
_temp1247;}); void* _temp1256; struct Cyc_Absyn_PtrInfo _temp1258; void*
_temp1260; struct Cyc_Absyn_PtrInfo _temp1262; void* _temp1264; void* _temp1266;
struct Cyc_Absyn_PtrInfo _temp1268; _LL1250: _LL1261: _temp1260= _temp1248.f1;
if(( unsigned int) _temp1260 > 4u?*(( int*) _temp1260) == Cyc_Absyn_PointerType:
0){ _LL1263: _temp1262=(( struct Cyc_Absyn_PointerType_struct*) _temp1260)->f1;
goto _LL1257;} else{ goto _LL1252;} _LL1257: _temp1256= _temp1248.f2; if((
unsigned int) _temp1256 > 4u?*(( int*) _temp1256) == Cyc_Absyn_PointerType: 0){
_LL1259: _temp1258=(( struct Cyc_Absyn_PointerType_struct*) _temp1256)->f1; goto
_LL1251;} else{ goto _LL1252;} _LL1252: _LL1267: _temp1266= _temp1248.f1; if((
unsigned int) _temp1266 > 4u?*(( int*) _temp1266) == Cyc_Absyn_PointerType: 0){
_LL1269: _temp1268=(( struct Cyc_Absyn_PointerType_struct*) _temp1266)->f1; goto
_LL1265;} else{ goto _LL1254;} _LL1265: _temp1264= _temp1248.f2; if((
unsigned int) _temp1264 > 4u?*(( int*) _temp1264) == Cyc_Absyn_IntType: 0){ goto
_LL1253;} else{ goto _LL1254;} _LL1254: goto _LL1255; _LL1251: { int _temp1270=((
int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0, _temp1262.nullable);
int _temp1271=(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
0, _temp1258.nullable); void* _temp1272= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1262.bounds); void* _temp1273= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1258.bounds);{ struct _tuple8 _temp1275=({ struct _tuple8 _temp1274;
_temp1274.f1= _temp1272; _temp1274.f2= _temp1273; _temp1274;}); void* _temp1285;
void* _temp1287; void* _temp1289; void* _temp1291; struct Cyc_Absyn_Exp*
_temp1293; void* _temp1295; struct Cyc_Absyn_Exp* _temp1297; void* _temp1299;
_LL1277: _LL1288: _temp1287= _temp1275.f1; if(( unsigned int) _temp1287 > 1u?*((
int*) _temp1287) == Cyc_Absyn_Upper_b: 0){ goto _LL1286;} else{ goto _LL1279;}
_LL1286: _temp1285= _temp1275.f2; if(( unsigned int) _temp1285 > 1u?*(( int*)
_temp1285) == Cyc_Absyn_Upper_b: 0){ goto _LL1278;} else{ goto _LL1279;} _LL1279:
_LL1292: _temp1291= _temp1275.f1; if(( unsigned int) _temp1291 > 1u?*(( int*)
_temp1291) == Cyc_Absyn_Upper_b: 0){ _LL1294: _temp1293=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1291)->f1; goto _LL1290;} else{ goto _LL1281;} _LL1290: _temp1289=
_temp1275.f2; if( _temp1289 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1280;}
else{ goto _LL1281;} _LL1281: _LL1300: _temp1299= _temp1275.f1; if( _temp1299 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL1296;} else{ goto _LL1283;} _LL1296:
_temp1295= _temp1275.f2; if(( unsigned int) _temp1295 > 1u?*(( int*) _temp1295)
== Cyc_Absyn_Upper_b: 0){ _LL1298: _temp1297=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1295)->f1; goto _LL1282;} else{ goto _LL1283;} _LL1283: goto _LL1284;
_LL1278: if( _temp1270? ! _temp1271: 0){ if( nv->toplevel){(( int(*)( struct
_tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("can't do null-check conversion at top-level",
sizeof( unsigned char), 44u));}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(*
_temp993, Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({ struct Cyc_List_List*
_temp1301=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1301->hd=( void*) _temp989; _temp1301->tl= 0; _temp1301;}), 0), 0))->r));}
goto _LL1276; _LL1280: if( nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr(
old_t2, _temp1293, _temp989))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_List_List*(* _temp1302)( struct _tagged_arr)=(
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp*
_temp1305= _temp989; struct Cyc_Absyn_Exp* _temp1306= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(( void*) _temp1258.elt_typ), 0); struct Cyc_Absyn_Exp*
_temp1307= _temp1293; struct Cyc_Absyn_Exp* _temp1303[ 3u]={ _temp1305,
_temp1306, _temp1307}; struct _tagged_arr _temp1304={( void*) _temp1303,( void*)
_temp1303,( void*)( _temp1303 + 3u)}; _temp1302( _temp1304);}), 0))->r));} goto
_LL1276; _LL1282: if( nv->toplevel){(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(
_tag_arr("can't coerce t? to t* or t@ at the top-level", sizeof( unsigned char),
45u));}{ struct Cyc_Absyn_Exp* _temp1308= Cyc_Absyn_fncall_exp( Cyc_Toc__untag_arr_e,({
struct Cyc_List_List*(* _temp1310)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1313= _temp989;
struct Cyc_Absyn_Exp* _temp1314= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c((
void*) _temp1262.elt_typ), 0); struct Cyc_Absyn_Exp* _temp1315= _temp1297;
struct Cyc_Absyn_Exp* _temp1311[ 3u]={ _temp1313, _temp1314, _temp1315}; struct
_tagged_arr _temp1312={( void*) _temp1311,( void*) _temp1311,( void*)( _temp1311
+ 3u)}; _temp1310( _temp1312);}), 0); if( _temp1271){( void*)( _temp1308->r=(
void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({ struct Cyc_List_List*
_temp1309=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1309->hd=( void*) Cyc_Absyn_copy_exp( _temp1308); _temp1309->tl= 0;
_temp1309;}), 0))->r));}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(*
_temp993, _temp1308, 0))->r)); goto _LL1276;} _LL1284: goto _LL1276; _LL1276:;}
goto _LL1249;} _LL1253:{ void* _temp1316= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1268.bounds); _LL1318: if( _temp1316 ==( void*) Cyc_Absyn_Unknown_b){ goto
_LL1319;} else{ goto _LL1320;} _LL1320: goto _LL1321; _LL1319:( void*)( _temp989->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) _temp989->r,
_temp989->loc), Cyc_Toc_curr_sp, 0))->r)); goto _LL1317; _LL1321: goto _LL1317;
_LL1317:;} goto _LL1249; _LL1255: goto _LL1249; _LL1249:;} goto _LL847;} _LL881:{
void* _temp1322=( void*) _temp994->r; struct Cyc_List_List* _temp1330; struct
_tuple0* _temp1332; struct Cyc_List_List* _temp1334; _LL1324: if(*(( int*)
_temp1322) == Cyc_Absyn_Struct_e){ _LL1333: _temp1332=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1322)->f1; goto _LL1331; _LL1331: _temp1330=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1322)->f3; goto _LL1325;} else{ goto _LL1326;} _LL1326: if(*(( int*)
_temp1322) == Cyc_Absyn_Tuple_e){ _LL1335: _temp1334=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1322)->f1; goto _LL1327;} else{ goto _LL1328;} _LL1328: goto _LL1329;
_LL1325: if( nv->toplevel){(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)((
struct _tagged_arr)({ struct _tagged_arr(* _temp1336)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp1340= _tag_arr("%s: & on non-identifiers at the top-level",
sizeof( unsigned char), 42u); struct Cyc_Stdio_String_pa_struct _temp1341;
_temp1341.tag= Cyc_Stdio_String_pa; _temp1341.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp994->loc);{ void* _temp1339=( void*)& _temp1341; void* _temp1337[ 1u]={
_temp1339}; struct _tagged_arr _temp1338={( void*) _temp1337,( void*) _temp1337,(
void*)( _temp1337 + 1u)}; _temp1336( _temp1340, _temp1338);}}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp994->topt))->v, 1, 0, _temp1330, _temp1332))->r)); goto
_LL1323; _LL1327: if( nv->toplevel){(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)((
struct _tagged_arr)({ struct _tagged_arr(* _temp1342)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp1346= _tag_arr("%s: & on non-identifiers at the top-level",
sizeof( unsigned char), 42u); struct Cyc_Stdio_String_pa_struct _temp1347;
_temp1347.tag= Cyc_Stdio_String_pa; _temp1347.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp994->loc);{ void* _temp1345=( void*)& _temp1347; void* _temp1343[ 1u]={
_temp1345}; struct _tagged_arr _temp1344={( void*) _temp1343,( void*) _temp1343,(
void*)( _temp1343 + 1u)}; _temp1342( _temp1346, _temp1344);}}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1334))->r)); goto _LL1323;
_LL1329: Cyc_Toc_exp_to_c( nv, _temp994); if( ! Cyc_Absyn_is_lvalue( _temp994)){((
void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)( _temp994, 0,
Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp994, 0))->r));} goto _LL1323; _LL1323:;} goto _LL847; _LL883: if( nv->toplevel){((
int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(( struct _tagged_arr)({ struct
_tagged_arr(* _temp1348)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1352= _tag_arr("%s: new at top-level", sizeof(
unsigned char), 21u); struct Cyc_Stdio_String_pa_struct _temp1353; _temp1353.tag=
Cyc_Stdio_String_pa; _temp1353.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp996->loc);{ void* _temp1351=( void*)& _temp1353; void* _temp1349[ 1u]={
_temp1351}; struct _tagged_arr _temp1350={( void*) _temp1349,( void*) _temp1349,(
void*)( _temp1349 + 1u)}; _temp1348( _temp1352, _temp1350);}}));}{ void*
_temp1354=( void*) _temp996->r; struct Cyc_List_List* _temp1366; struct Cyc_Absyn_Exp*
_temp1368; struct Cyc_Absyn_Exp* _temp1370; struct Cyc_Absyn_Vardecl* _temp1372;
struct Cyc_List_List* _temp1374; struct _tuple0* _temp1376; struct Cyc_List_List*
_temp1378; _LL1356: if(*(( int*) _temp1354) == Cyc_Absyn_Array_e){ _LL1367:
_temp1366=(( struct Cyc_Absyn_Array_e_struct*) _temp1354)->f1; goto _LL1357;}
else{ goto _LL1358;} _LL1358: if(*(( int*) _temp1354) == Cyc_Absyn_Comprehension_e){
_LL1373: _temp1372=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1354)->f1;
goto _LL1371; _LL1371: _temp1370=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1354)->f2; goto _LL1369; _LL1369: _temp1368=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1354)->f3; goto _LL1359;} else{ goto _LL1360;} _LL1360: if(*(( int*)
_temp1354) == Cyc_Absyn_Struct_e){ _LL1377: _temp1376=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1354)->f1; goto _LL1375; _LL1375: _temp1374=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1354)->f3; goto _LL1361;} else{ goto _LL1362;} _LL1362: if(*(( int*)
_temp1354) == Cyc_Absyn_Tuple_e){ _LL1379: _temp1378=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1354)->f1; goto _LL1363;} else{ goto _LL1364;} _LL1364: goto _LL1365;
_LL1357: { struct _tuple0* _temp1380= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp1381= Cyc_Absyn_var_exp( _temp1380, 0); struct Cyc_Absyn_Stmt* _temp1382=
Cyc_Toc_init_array( nv, _temp1381, _temp1366, Cyc_Absyn_exp_stmt( _temp1381, 0));
void* old_elt_typ;{ void* _temp1383= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1389; struct Cyc_Absyn_Tqual _temp1391; void* _temp1393; _LL1385: if((
unsigned int) _temp1383 > 4u?*(( int*) _temp1383) == Cyc_Absyn_PointerType: 0){
_LL1390: _temp1389=(( struct Cyc_Absyn_PointerType_struct*) _temp1383)->f1;
_LL1394: _temp1393=( void*) _temp1389.elt_typ; goto _LL1392; _LL1392: _temp1391=
_temp1389.tq; goto _LL1386;} else{ goto _LL1387;} _LL1387: goto _LL1388; _LL1386:
old_elt_typ= _temp1393; goto _LL1384; _LL1388: old_elt_typ= Cyc_Toc_toc_impos(
_tag_arr("exp_to_c:new array expression doesn't have ptr type", sizeof(
unsigned char), 52u)); goto _LL1384; _LL1384:;}{ void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); void* _temp1395= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp* _temp1396= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_signed_int_exp((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1366), 0), 0); struct Cyc_Absyn_Exp* e1; if( _temp998 == 0){ e1= Cyc_Toc_malloc_exp(
old_elt_typ, _temp1396);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp998); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r,
_temp1396);}{ struct Cyc_Absyn_Exp* _temp1397= Cyc_Absyn_cast_exp( _temp1395, e1,
0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1380, _temp1395,( struct Cyc_Absyn_Exp*) _temp1397, _temp1382, 0), 0))->r));
goto _LL1355;}}} _LL1359: { int is_tagged_ptr= 0;{ void* _temp1398= Cyc_Tcutil_compress(
old_typ); struct Cyc_Absyn_PtrInfo _temp1404; struct Cyc_Absyn_Conref* _temp1406;
struct Cyc_Absyn_Tqual _temp1408; void* _temp1410; _LL1400: if(( unsigned int)
_temp1398 > 4u?*(( int*) _temp1398) == Cyc_Absyn_PointerType: 0){ _LL1405:
_temp1404=(( struct Cyc_Absyn_PointerType_struct*) _temp1398)->f1; _LL1411:
_temp1410=( void*) _temp1404.elt_typ; goto _LL1409; _LL1409: _temp1408=
_temp1404.tq; goto _LL1407; _LL1407: _temp1406= _temp1404.bounds; goto _LL1401;}
else{ goto _LL1402;} _LL1402: goto _LL1403; _LL1401: is_tagged_ptr= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1406) ==( void*) Cyc_Absyn_Unknown_b; goto _LL1399;
_LL1403:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: comprehension not an array type",
sizeof( unsigned char), 42u)); goto _LL1399; _LL1399:;}{ struct _tuple0* max=
Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var(); void* old_elt_typ=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1368->topt))->v; void* elt_typ=
Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c( nv, _temp1370);{ struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( a, 0), _temp1372, Cyc_Absyn_var_exp( max, 0), _temp1368,
Cyc_Absyn_skip_stmt( 0), 1);{ struct _RegionHandle _temp1412= _new_region();
struct _RegionHandle* r=& _temp1412; _push_region( r);{ struct Cyc_List_List*
decls=({ struct Cyc_List_List* _temp1435=( struct Cyc_List_List*) _region_malloc(
r, sizeof( struct Cyc_List_List)); _temp1435->hd=( void*)({ struct _tuple9*
_temp1436=( struct _tuple9*) _region_malloc( r, sizeof( struct _tuple9));
_temp1436->f1= max; _temp1436->f2= Cyc_Absyn_uint_t; _temp1436->f3=( struct Cyc_Absyn_Exp*)
_temp1370; _temp1436;}); _temp1435->tl= 0; _temp1435;}); struct Cyc_Absyn_Exp*
ai; if( _temp998 == 0){ ai= Cyc_Toc_malloc_exp( old_elt_typ, Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0));} else{
struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( _temp998); Cyc_Toc_exp_to_c(
nv, r); ai= Cyc_Toc_rmalloc_exp( r, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0));}{ struct Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp(
ptr_typ, ai, 0); decls=({ struct Cyc_List_List* _temp1413=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1413->hd=( void*)({
struct _tuple9* _temp1414=( struct _tuple9*) _region_malloc( r, sizeof( struct
_tuple9)); _temp1414->f1= a; _temp1414->f2= ptr_typ; _temp1414->f3=( struct Cyc_Absyn_Exp*)
ainit; _temp1414;}); _temp1413->tl= decls; _temp1413;}); if( is_tagged_ptr){
struct _tuple0* _temp1415= Cyc_Toc_temp_var(); void* _temp1416= Cyc_Toc_typ_to_c(
old_typ); struct Cyc_Absyn_Exp* _temp1417= Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({
struct Cyc_List_List*(* _temp1420)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1423= Cyc_Absyn_var_exp(
a, 0); struct Cyc_Absyn_Exp* _temp1424= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);
struct Cyc_Absyn_Exp* _temp1425= _temp1370; struct Cyc_Absyn_Exp* _temp1421[ 3u]={
_temp1423, _temp1424, _temp1425}; struct _tagged_arr _temp1422={( void*)
_temp1421,( void*) _temp1421,( void*)( _temp1421 + 3u)}; _temp1420( _temp1422);}),
0); decls=({ struct Cyc_List_List* _temp1418=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1418->hd=( void*)({
struct _tuple9* _temp1419=( struct _tuple9*) _region_malloc( r, sizeof( struct
_tuple9)); _temp1419->f1= _temp1415; _temp1419->f2= _temp1416; _temp1419->f3=(
struct Cyc_Absyn_Exp*) _temp1417; _temp1419;}); _temp1418->tl= decls; _temp1418;});
s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1415, 0),
0), 0);} else{ s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
a, 0), 0), 0);}{ struct Cyc_List_List* _temp1426= decls; for( 0; _temp1426 != 0;
_temp1426=(( struct Cyc_List_List*) _check_null( _temp1426))->tl){ struct Cyc_Absyn_Exp*
_temp1429; void* _temp1431; struct _tuple0* _temp1433; struct _tuple9 _temp1427=*((
struct _tuple9*)(( struct Cyc_List_List*) _check_null( _temp1426))->hd); _LL1434:
_temp1433= _temp1427.f1; goto _LL1432; _LL1432: _temp1431= _temp1427.f2; goto
_LL1430; _LL1430: _temp1429= _temp1427.f3; goto _LL1428; _LL1428: s= Cyc_Absyn_declare_stmt(
_temp1433, _temp1431, _temp1429, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));}}; _pop_region( r);} goto _LL1355;}}} _LL1361:( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp996->topt))->v, 1, _temp998, _temp1374, _temp1376))->r)); goto _LL1355;
_LL1363:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, _temp998,
_temp1378))->r)); goto _LL1355; _LL1365: { void* old_elt_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp996->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); struct Cyc_Absyn_Exp* mexp= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);
if( _temp998 == 0){ mexp= Cyc_Toc_malloc_exp( old_elt_typ, mexp);} else{ struct
Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( _temp998); Cyc_Toc_exp_to_c(
nv, r); mexp= Cyc_Toc_rmalloc_exp( r, mexp);} mexp= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq), mexp, 0);{ int done= 0;{ void* _temp1437=( void*)
_temp996->r; struct Cyc_Absyn_Exp* _temp1443; void* _temp1445; _LL1439: if(*((
int*) _temp1437) == Cyc_Absyn_Cast_e){ _LL1446: _temp1445=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1437)->f1; goto _LL1444; _LL1444: _temp1443=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1437)->f2; goto _LL1440;} else{ goto _LL1441;} _LL1441: goto _LL1442;
_LL1440:{ struct _tuple8 _temp1448=({ struct _tuple8 _temp1447; _temp1447.f1=
Cyc_Tcutil_compress( _temp1445); _temp1447.f2= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1443->topt))->v); _temp1447;}); void*
_temp1454; struct Cyc_Absyn_PtrInfo _temp1456; struct Cyc_Absyn_Conref*
_temp1458; void* _temp1460; struct Cyc_Absyn_PtrInfo _temp1462; struct Cyc_Absyn_Conref*
_temp1464; void* _temp1466; _LL1450: _LL1461: _temp1460= _temp1448.f1; if((
unsigned int) _temp1460 > 4u?*(( int*) _temp1460) == Cyc_Absyn_PointerType: 0){
_LL1463: _temp1462=(( struct Cyc_Absyn_PointerType_struct*) _temp1460)->f1;
_LL1467: _temp1466=( void*) _temp1462.elt_typ; goto _LL1465; _LL1465: _temp1464=
_temp1462.bounds; goto _LL1455;} else{ goto _LL1452;} _LL1455: _temp1454=
_temp1448.f2; if(( unsigned int) _temp1454 > 4u?*(( int*) _temp1454) == Cyc_Absyn_PointerType:
0){ _LL1457: _temp1456=(( struct Cyc_Absyn_PointerType_struct*) _temp1454)->f1;
_LL1459: _temp1458= _temp1456.bounds; goto _LL1451;} else{ goto _LL1452;}
_LL1452: goto _LL1453; _LL1451:{ struct _tuple8 _temp1469=({ struct _tuple8
_temp1468; _temp1468.f1= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1464);
_temp1468.f2= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1458); _temp1468;});
void* _temp1475; struct Cyc_Absyn_Exp* _temp1477; void* _temp1479; _LL1471:
_LL1480: _temp1479= _temp1469.f1; if( _temp1479 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1476;} else{ goto _LL1473;} _LL1476: _temp1475= _temp1469.f2; if((
unsigned int) _temp1475 > 1u?*(( int*) _temp1475) == Cyc_Absyn_Upper_b: 0){
_LL1478: _temp1477=(( struct Cyc_Absyn_Upper_b_struct*) _temp1475)->f1; goto
_LL1472;} else{ goto _LL1473;} _LL1473: goto _LL1474; _LL1472: done= 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__init_tag_arr_e,({ struct
Cyc_List_List*(* _temp1481)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1484= mexp;
struct Cyc_Absyn_Exp* _temp1485= _temp1443; struct Cyc_Absyn_Exp* _temp1486= Cyc_Absyn_sizeoftyp_exp(
_temp1466, 0); struct Cyc_Absyn_Exp* _temp1487= _temp1477; struct Cyc_Absyn_Exp*
_temp1482[ 4u]={ _temp1484, _temp1485, _temp1486, _temp1487}; struct _tagged_arr
_temp1483={( void*) _temp1482,( void*) _temp1482,( void*)( _temp1482 + 4u)};
_temp1481( _temp1483);}), 0))->r)); goto _LL1470; _LL1474: goto _LL1470; _LL1470:;}
goto _LL1449; _LL1453: goto _LL1449; _LL1449:;} goto _LL1438; _LL1442: goto
_LL1438; _LL1438:;} if( ! done){ struct _tuple0* _temp1488= Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt* _temp1489= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
_temp1488, 0), 0); struct Cyc_Absyn_Exp* _temp1490= Cyc_Absyn_signed_int_exp( 0,
0); Cyc_Toc_exp_to_c( nv, _temp996); _temp1489= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( _temp1488, 0), _temp1490, 0),
_temp996, 0), _temp1489, 0);{ void* _temp1491= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1488, _temp1491,( struct Cyc_Absyn_Exp*) mexp, _temp1489, 0), 0))->r));}}
goto _LL1355;}} _LL1355:;} goto _LL847; _LL885: Cyc_Toc_exp_to_c( nv, _temp1000);
goto _LL847; _LL887:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1492=( struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1492[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1493; _temp1493.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1493.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1002);
_temp1493;}); _temp1492;}))); goto _LL847; _LL889:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Offsetof_e_struct* _temp1494=( struct Cyc_Absyn_Offsetof_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp1494[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp1495; _temp1495.tag= Cyc_Absyn_Offsetof_e;
_temp1495.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1006); _temp1495.f2=
_temp1004; _temp1495;}); _temp1494;}))); goto _LL847; _LL891: { void* _temp1496=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1008->topt))->v);{
void* _temp1497= _temp1496; struct Cyc_Absyn_PtrInfo _temp1503; struct Cyc_Absyn_Conref*
_temp1505; struct Cyc_Absyn_Tqual _temp1507; struct Cyc_Absyn_Conref* _temp1509;
void* _temp1511; void* _temp1513; _LL1499: if(( unsigned int) _temp1497 > 4u?*((
int*) _temp1497) == Cyc_Absyn_PointerType: 0){ _LL1504: _temp1503=(( struct Cyc_Absyn_PointerType_struct*)
_temp1497)->f1; _LL1514: _temp1513=( void*) _temp1503.elt_typ; goto _LL1512;
_LL1512: _temp1511=( void*) _temp1503.rgn_typ; goto _LL1510; _LL1510: _temp1509=
_temp1503.nullable; goto _LL1508; _LL1508: _temp1507= _temp1503.tq; goto _LL1506;
_LL1506: _temp1505= _temp1503.bounds; goto _LL1500;} else{ goto _LL1501;}
_LL1501: goto _LL1502; _LL1500:{ void* _temp1515= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1505); _LL1517: if(( unsigned int) _temp1515 > 1u?*(( int*) _temp1515) ==
Cyc_Absyn_Upper_b: 0){ goto _LL1518;} else{ goto _LL1519;} _LL1519: if(
_temp1515 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1520;} else{ goto _LL1516;}
_LL1518: Cyc_Toc_exp_to_c( nv, _temp1008); if( Cyc_Toc_is_nullable( _temp1496)){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1008->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1521=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1521->hd=( void*) _temp1008;
_temp1521->tl= 0; _temp1521;}), 0), 0), 0))->r));} goto _LL1516; _LL1520: {
struct Cyc_Absyn_Exp* _temp1522= Cyc_Absyn_uint_exp( 0, 0); _temp1522->topt=({
struct Cyc_Core_Opt* _temp1523=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1523->v=( void*) Cyc_Absyn_uint_t; _temp1523;});( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_subscript_exp( _temp1008, _temp1522, 0))->r)); Cyc_Toc_exp_to_c(
nv, e); goto _LL1516;} _LL1516:;} goto _LL1498; _LL1502:(( int(*)( struct
_tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: Deref: non-pointer",
sizeof( unsigned char), 29u)); goto _LL1498; _LL1498:;} goto _LL847;} _LL893:
Cyc_Toc_exp_to_c( nv, _temp1012); if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r, 0),
0))->r));} goto _LL847; _LL895: { void* e1typ= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1016->topt))->v); Cyc_Toc_exp_to_c( nv,
_temp1016);{ int is_poly= Cyc_Toc_is_poly_project( e); struct Cyc_Absyn_Conref*
_temp1526; struct Cyc_Absyn_Tqual _temp1528; struct Cyc_Absyn_Conref* _temp1530;
void* _temp1532; void* _temp1534; struct Cyc_Absyn_PtrInfo _temp1524= Cyc_Toc_get_ptr_type(
e1typ); _LL1535: _temp1534=( void*) _temp1524.elt_typ; goto _LL1533; _LL1533:
_temp1532=( void*) _temp1524.rgn_typ; goto _LL1531; _LL1531: _temp1530=
_temp1524.nullable; goto _LL1529; _LL1529: _temp1528= _temp1524.tq; goto _LL1527;
_LL1527: _temp1526= _temp1524.bounds; goto _LL1525; _LL1525:{ void* _temp1536=
Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1526); struct Cyc_Absyn_Exp*
_temp1542; _LL1538: if(( unsigned int) _temp1536 > 1u?*(( int*) _temp1536) ==
Cyc_Absyn_Upper_b: 0){ _LL1543: _temp1542=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1536)->f1; goto _LL1539;} else{ goto _LL1540;} _LL1540: if( _temp1536 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL1541;} else{ goto _LL1537;} _LL1539: if(
Cyc_Evexp_eval_const_uint_exp( _temp1542) < 1){(( int(*)( struct _tagged_arr))
Cyc_Tcutil_impos)( _tag_arr("exp_to_c:  StructArrow_e on pointer of size 0",
sizeof( unsigned char), 46u));}{ int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( 0, _temp1530); if( Cyc_Toc_is_nullable(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1016->topt))->v)){( void*)( e->r=( void*)((
void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1016->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1544=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1544->hd=( void*) _temp1016;
_temp1544->tl= 0; _temp1544;}), 0), 0), _temp1014, 0))->r));} goto _LL1537;}
_LL1541: { void* ta1= Cyc_Toc_typ_to_c_array( _temp1534);( void*)( _temp1016->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( ta1, _temp1528), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_unknown_subscript_e,({ struct Cyc_List_List*(* _temp1545)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1548= Cyc_Absyn_copy_exp( _temp1016); struct Cyc_Absyn_Exp*
_temp1549= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp* _temp1550=
Cyc_Absyn_uint_exp( 0, 0); struct Cyc_Absyn_Exp* _temp1546[ 3u]={ _temp1548,
_temp1549, _temp1550}; struct _tagged_arr _temp1547={( void*) _temp1546,( void*)
_temp1546,( void*)( _temp1546 + 3u)}; _temp1545( _temp1547);}), 0), 0))->r));
goto _LL1537;} _LL1537:;} if( is_poly){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));} goto _LL847;}} _LL897: { void*
_temp1551= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1020->topt))->v); Cyc_Toc_exp_to_c( nv, _temp1020); Cyc_Toc_exp_to_c( nv,
_temp1018);{ void* _temp1552= _temp1551; struct Cyc_List_List* _temp1560; struct
Cyc_Absyn_PtrInfo _temp1562; struct Cyc_Absyn_Conref* _temp1564; struct Cyc_Absyn_Tqual
_temp1566; struct Cyc_Absyn_Conref* _temp1568; void* _temp1570; void* _temp1572;
_LL1554: if(( unsigned int) _temp1552 > 4u?*(( int*) _temp1552) == Cyc_Absyn_TupleType:
0){ _LL1561: _temp1560=(( struct Cyc_Absyn_TupleType_struct*) _temp1552)->f1;
goto _LL1555;} else{ goto _LL1556;} _LL1556: if(( unsigned int) _temp1552 > 4u?*((
int*) _temp1552) == Cyc_Absyn_PointerType: 0){ _LL1563: _temp1562=(( struct Cyc_Absyn_PointerType_struct*)
_temp1552)->f1; _LL1573: _temp1572=( void*) _temp1562.elt_typ; goto _LL1571;
_LL1571: _temp1570=( void*) _temp1562.rgn_typ; goto _LL1569; _LL1569: _temp1568=
_temp1562.nullable; goto _LL1567; _LL1567: _temp1566= _temp1562.tq; goto _LL1565;
_LL1565: _temp1564= _temp1562.bounds; goto _LL1557;} else{ goto _LL1558;}
_LL1558: goto _LL1559; _LL1555: { unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1018) + 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
_temp1020, Cyc_Toc_fieldname(( int) i), 0))->r)); goto _LL1553;} _LL1557:{ void*
_temp1574= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1564); struct Cyc_Absyn_Exp*
_temp1580; _LL1576: if(( unsigned int) _temp1574 > 1u?*(( int*) _temp1574) ==
Cyc_Absyn_Upper_b: 0){ _LL1581: _temp1580=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1574)->f1; goto _LL1577;} else{ goto _LL1578;} _LL1578: if( _temp1574 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL1579;} else{ goto _LL1575;} _LL1577:
_temp1580= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp( _temp1580), 0);{
int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
0, _temp1568); void* ta1= Cyc_Toc_typ_to_c( _temp1572); void* ta2= Cyc_Absyn_cstar_typ(
ta1, _temp1566); if( possibly_null){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp(
Cyc_Absyn_cast_exp( ta2, Cyc_Absyn_fncall_exp( Cyc_Toc__check_known_subscript_null_e,({
struct Cyc_List_List*(* _temp1582)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1585= _temp1020;
struct Cyc_Absyn_Exp* _temp1586= _temp1580; struct Cyc_Absyn_Exp* _temp1587= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); struct Cyc_Absyn_Exp* _temp1588= _temp1018; struct Cyc_Absyn_Exp*
_temp1583[ 4u]={ _temp1585, _temp1586, _temp1587, _temp1588}; struct _tagged_arr
_temp1584={( void*) _temp1583,( void*) _temp1583,( void*)( _temp1583 + 4u)};
_temp1582( _temp1584);}), 0), 0), 0))->r));} else{( void*)( _temp1018->r=( void*)((
void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__check_known_subscript_notnull_e,({ struct
Cyc_List_List*(* _temp1589)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1592= _temp1580;
struct Cyc_Absyn_Exp* _temp1593= Cyc_Absyn_copy_exp( _temp1018); struct Cyc_Absyn_Exp*
_temp1590[ 2u]={ _temp1592, _temp1593}; struct _tagged_arr _temp1591={( void*)
_temp1590,( void*) _temp1590,( void*)( _temp1590 + 2u)}; _temp1589( _temp1591);}),
0))->r));} goto _LL1575;} _LL1579: { void* ta1= Cyc_Toc_typ_to_c_array(
_temp1572);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( ta1, _temp1566), Cyc_Absyn_fncall_exp( Cyc_Toc__check_unknown_subscript_e,({
struct Cyc_List_List*(* _temp1594)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1597= _temp1020;
struct Cyc_Absyn_Exp* _temp1598= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp*
_temp1599= _temp1018; struct Cyc_Absyn_Exp* _temp1595[ 3u]={ _temp1597,
_temp1598, _temp1599}; struct _tagged_arr _temp1596={( void*) _temp1595,( void*)
_temp1595,( void*)( _temp1595 + 3u)}; _temp1594( _temp1596);}), 0), 0), 0))->r));
goto _LL1575;} _LL1575:;} goto _LL1553; _LL1559:(( int(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: Subscript on non-tuple/array/tuple ptr",
sizeof( unsigned char), 49u)); goto _LL1553; _LL1553:;} goto _LL847;} _LL899:
if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 0,
0, _temp1022))->r));} else{ struct Cyc_List_List* _temp1600=(( struct Cyc_List_List*(*)(
struct _tuple3*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Toc_tup_to_c, _temp1022); struct _tagged_arr* _temp1601= Cyc_Toc_add_tuple_type(
_temp1600); struct Cyc_List_List* dles= 0;{ int i= 1; for( 0; _temp1022 != 0;(
_temp1022=(( struct Cyc_List_List*) _check_null( _temp1022))->tl, i ++)){ Cyc_Toc_exp_to_c(
nv,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1022))->hd);
dles=({ struct Cyc_List_List* _temp1602=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1602->hd=( void*)({ struct _tuple4*
_temp1603=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1603->f1=
0; _temp1603->f2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp1022))->hd; _temp1603;}); _temp1602->tl= dles; _temp1602;});}} dles=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, dles, 0))->r));}
goto _LL847; _LL901:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1024, 0))->r));{ struct Cyc_List_List* _temp1604= _temp1024; for( 0;
_temp1604 != 0; _temp1604=(( struct Cyc_List_List*) _check_null( _temp1604))->tl){
struct _tuple4 _temp1607; struct Cyc_Absyn_Exp* _temp1608; struct _tuple4*
_temp1605=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1604))->hd;
_temp1607=* _temp1605; _LL1609: _temp1608= _temp1607.f2; goto _LL1606; _LL1606:
Cyc_Toc_exp_to_c( nv, _temp1608);}} goto _LL847; _LL903: { unsigned int
_temp1610= Cyc_Evexp_eval_const_uint_exp( _temp1028); Cyc_Toc_exp_to_c( nv,
_temp1026);{ struct Cyc_List_List* es= 0; if( ! Cyc_Toc_is_zero( _temp1026)){
unsigned int i= 0; for( 0; i < _temp1610; i ++){ es=({ struct Cyc_List_List*
_temp1611=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1611->hd=( void*)({ struct _tuple4* _temp1612=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1612->f1= 0; _temp1612->f2= _temp1026; _temp1612;});
_temp1611->tl= es; _temp1611;});}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, es, 0))->r)); goto _LL847;}} _LL905: if( ! nv->toplevel){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1034, _temp1038))->r));}
else{ if( _temp1032 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("Struct_e: missing structdecl pointer", sizeof( unsigned char), 37u));}{
struct Cyc_Absyn_Structdecl* sd2=( struct Cyc_Absyn_Structdecl*) _check_null(
_temp1032); struct _RegionHandle _temp1613= _new_region(); struct _RegionHandle*
rgn=& _temp1613; _push_region( rgn);{ struct Cyc_List_List* _temp1614=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp1034,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( sd2->fields))->v); struct Cyc_List_List* _temp1615= 0;{ struct Cyc_List_List*
_temp1616=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd2->fields))->v;
for( 0; _temp1616 != 0; _temp1616=(( struct Cyc_List_List*) _check_null(
_temp1616))->tl){ struct Cyc_List_List* _temp1617= _temp1614; for( 0; _temp1617
!= 0; _temp1617=(( struct Cyc_List_List*) _check_null( _temp1617))->tl){ if((*((
struct _tuple10*)(( struct Cyc_List_List*) _check_null( _temp1617))->hd)).f1 ==(
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp1616))->hd){
struct _tuple10 _temp1620; struct Cyc_Absyn_Exp* _temp1621; struct Cyc_Absyn_Structfield*
_temp1623; struct _tuple10* _temp1618=( struct _tuple10*)(( struct Cyc_List_List*)
_check_null( _temp1617))->hd; _temp1620=* _temp1618; _LL1624: _temp1623=
_temp1620.f1; goto _LL1622; _LL1622: _temp1621= _temp1620.f2; goto _LL1619;
_LL1619: { void* _temp1625=( void*) _temp1623->type; Cyc_Toc_exp_to_c( nv,
_temp1621); if( Cyc_Toc_is_void_star( _temp1625)){( void*)( _temp1621->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp1621->r, 0), 0))->r));} _temp1615=({ struct Cyc_List_List* _temp1626=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1626->hd=(
void*)({ struct _tuple4* _temp1627=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp1627->f1= 0; _temp1627->f2= _temp1621; _temp1627;}); _temp1626->tl=
_temp1615; _temp1626;}); break;}}}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0,(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp1615), 0))->r));}; _pop_region( rgn);}} goto _LL847; _LL907: { struct Cyc_List_List*
fs;{ void* _temp1628= Cyc_Tcutil_compress( _temp1042); struct Cyc_List_List*
_temp1634; _LL1630: if(( unsigned int) _temp1628 > 4u?*(( int*) _temp1628) ==
Cyc_Absyn_AnonStructType: 0){ _LL1635: _temp1634=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp1628)->f1; goto _LL1631;} else{ goto _LL1632;} _LL1632: goto _LL1633;
_LL1631: fs= _temp1634; goto _LL1629; _LL1633: fs=(( struct Cyc_List_List*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct
_tagged_arr(* _temp1636)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1640= _tag_arr("anon struct has type %s", sizeof(
unsigned char), 24u); struct Cyc_Stdio_String_pa_struct _temp1641; _temp1641.tag=
Cyc_Stdio_String_pa; _temp1641.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp1042);{ void* _temp1639=( void*)& _temp1641; void* _temp1637[ 1u]={
_temp1639}; struct _tagged_arr _temp1638={( void*) _temp1637,( void*) _temp1637,(
void*)( _temp1637 + 1u)}; _temp1636( _temp1640, _temp1638);}})); goto _LL1629;
_LL1629:;}{ struct _RegionHandle _temp1642= _new_region(); struct _RegionHandle*
rgn=& _temp1642; _push_region( rgn);{ struct Cyc_List_List* _temp1643=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp1040, fs); for( 0; _temp1643 != 0; _temp1643=(( struct Cyc_List_List*)
_check_null( _temp1643))->tl){ struct _tuple10 _temp1646; struct Cyc_Absyn_Exp*
_temp1647; struct Cyc_Absyn_Structfield* _temp1649; struct _tuple10* _temp1644=(
struct _tuple10*)(( struct Cyc_List_List*) _check_null( _temp1643))->hd;
_temp1646=* _temp1644; _LL1650: _temp1649= _temp1646.f1; goto _LL1648; _LL1648:
_temp1647= _temp1646.f2; goto _LL1645; _LL1645: { void* _temp1651=( void*)
_temp1649->type; Cyc_Toc_exp_to_c( nv, _temp1647); if( Cyc_Toc_is_void_star(
_temp1651)){( void*)( _temp1647->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1647->r, 0), 0))->r));}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0, _temp1040, 0))->r));}; _pop_region( rgn);}
goto _LL847;} _LL909: { struct _tuple0* qv= _temp1044->name; if( _temp1046->is_xtunion?
1: ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));}
else{ int tag_count= 0; struct Cyc_List_List* _temp1652= _temp1046->fields == 0?
0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1046->fields))->v;
while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp1652))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1652))->hd)->typs == 0){ tag_count ++;}
_temp1652=(( struct Cyc_List_List*) _check_null( _temp1652))->tl;}( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));} goto
_LL847;} _LL911: { struct _tuple0* _temp1653= _temp1050->name; struct Cyc_List_List*
_temp1654= _temp1050->typs; struct _tuple0* _temp1655= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* _temp1656= Cyc_Absyn_var_exp( _temp1655, 0); void*
_temp1657= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1653, _tag_arr("_struct",
sizeof( unsigned char), 8u))); if( nv->toplevel){ struct Cyc_Absyn_Exp* tag_exp;
if( _temp1052->is_xtunion){ tag_exp= Cyc_Absyn_var_exp( _temp1653, 0);} else{
int tag_count= 0; struct Cyc_List_List* _temp1658= _temp1052->fields == 0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1052->fields))->v;
while( Cyc_Absyn_qvar_cmp( _temp1653,(( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp1658))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1658))->hd)->typs != 0){ tag_count ++;}
_temp1658=(( struct Cyc_List_List*) _check_null( _temp1658))->tl;} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* _temp1659= 0; for( 0;
_temp1054 != 0;( _temp1054=(( struct Cyc_List_List*) _check_null( _temp1054))->tl,
_temp1654=(( struct Cyc_List_List*) _check_null( _temp1654))->tl)){ struct Cyc_Absyn_Exp*
cur_e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1054))->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1654))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);} _temp1659=({
struct Cyc_List_List* _temp1660=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1660->hd=( void*)({ struct _tuple4* _temp1661=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1661->f1= 0;
_temp1661->f2= cur_e; _temp1661;}); _temp1660->tl= _temp1659; _temp1660;});}
_temp1659=({ struct Cyc_List_List* _temp1662=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1662->hd=( void*)({ struct _tuple4*
_temp1663=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1663->f1=
0; _temp1663->f2= tag_exp; _temp1663;}); _temp1662->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1659); _temp1662;});( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp1664=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1664->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp1653, _tag_arr("_struct",
sizeof( unsigned char), 8u)); _temp1664;}), _temp1659, 0))->r));}} else{ struct
Cyc_List_List* _temp1665=({ struct Cyc_List_List* _temp1671=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1671->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp1656, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp(
_temp1653, 0), 0); _temp1671->tl= 0; _temp1671;});{ int i= 1; for( 0; _temp1054
!= 0;((( _temp1054=(( struct Cyc_List_List*) _check_null( _temp1054))->tl, i ++)),
_temp1654=(( struct Cyc_List_List*) _check_null( _temp1654))->tl)){ struct Cyc_Absyn_Exp*
cur_e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1054))->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1654))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt*
_temp1666= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1656, Cyc_Toc_fieldname(
i), 0), cur_e, 0); _temp1665=({ struct Cyc_List_List* _temp1667=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1667->hd=( void*) _temp1666;
_temp1667->tl= _temp1665; _temp1667;});}}}{ struct Cyc_Absyn_Stmt* _temp1668=
Cyc_Absyn_exp_stmt( _temp1656, 0); struct Cyc_Absyn_Stmt* _temp1669= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct
Cyc_List_List* _temp1670=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1670->hd=( void*) _temp1668; _temp1670->tl= _temp1665; _temp1670;})), 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1655, _temp1657, 0, _temp1669, 0), 0))->r));}} goto _LL847;} _LL913: goto
_LL847; _LL915: { void* t_c= Cyc_Toc_typ_to_c( _temp1056); if( _temp1058 != 0){
struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*) _check_null( _temp1058); Cyc_Toc_exp_to_c(
nv, rgn);( void*)( e->r=( void*)(( void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp(
_temp1056, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));} goto _LL847;} _LL917: Cyc_Toc_stmt_to_c(
nv, _temp1060); goto _LL847; _LL919:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("UnresolvedMem", sizeof( unsigned char), 14u)); goto _LL847; _LL921:((
int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("compoundlit", sizeof(
unsigned char), 12u)); goto _LL847; _LL923:(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(
_tag_arr("codegen", sizeof( unsigned char), 8u)); goto _LL847; _LL925:(( int(*)(
struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("fill", sizeof( unsigned char),
5u)); goto _LL847; _LL847:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_if_eq_goto_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct _tagged_arr* succ_lab,
struct _tagged_arr* fail_lab){ return Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp(
e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt( fail_lab, 0),
0);} struct _tuple11{ struct Cyc_Toc_Env* f1; struct Cyc_List_List* f2; struct
Cyc_Absyn_Stmt* f3; } ; struct _tuple12{ struct _tuple0* f1; void* f2; } ;
struct _tuple13{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; static
struct _tuple11 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env* nv, struct _RegionHandle*
rgn, void* t, struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path, struct Cyc_Absyn_Pat*
p, struct _tagged_arr* succ_lab, struct _tagged_arr* fail_lab, struct Cyc_List_List*
decls){ struct Cyc_Absyn_Stmt* s;{ void* _temp1672=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp1708; struct Cyc_Absyn_Vardecl _temp1710; struct _tuple0* _temp1711; struct
Cyc_Absyn_Vardecl* _temp1713; int _temp1715; void* _temp1717; unsigned char
_temp1719; struct _tagged_arr _temp1721; struct Cyc_Absyn_Enumfield* _temp1723;
struct Cyc_Absyn_Enumdecl* _temp1725; struct Cyc_List_List* _temp1727; struct
Cyc_List_List* _temp1729; struct Cyc_Absyn_Tunionfield* _temp1731; struct Cyc_Absyn_Tuniondecl*
_temp1733; struct Cyc_Absyn_Pat* _temp1736; struct Cyc_Absyn_Pat _temp1738; void*
_temp1739; struct Cyc_List_List* _temp1741; struct Cyc_List_List* _temp1743;
struct Cyc_Absyn_Tunionfield* _temp1745; struct Cyc_Absyn_Tuniondecl* _temp1747;
struct Cyc_List_List* _temp1749; struct Cyc_List_List* _temp1751; struct Cyc_List_List*
_temp1753; struct Cyc_List_List* _temp1755; struct Cyc_Core_Opt* _temp1757;
struct Cyc_Absyn_Structdecl* _temp1759; struct Cyc_Absyn_Pat* _temp1761; _LL1674:
if(( unsigned int) _temp1672 > 2u?*(( int*) _temp1672) == Cyc_Absyn_Var_p: 0){
_LL1709: _temp1708=(( struct Cyc_Absyn_Var_p_struct*) _temp1672)->f1; _temp1710=*
_temp1708; _LL1712: _temp1711= _temp1710.name; goto _LL1675;} else{ goto _LL1676;}
_LL1676: if( _temp1672 ==( void*) Cyc_Absyn_Wild_p){ goto _LL1677;} else{ goto
_LL1678;} _LL1678: if(( unsigned int) _temp1672 > 2u?*(( int*) _temp1672) == Cyc_Absyn_Reference_p:
0){ _LL1714: _temp1713=(( struct Cyc_Absyn_Reference_p_struct*) _temp1672)->f1;
goto _LL1679;} else{ goto _LL1680;} _LL1680: if( _temp1672 ==( void*) Cyc_Absyn_Null_p){
goto _LL1681;} else{ goto _LL1682;} _LL1682: if(( unsigned int) _temp1672 > 2u?*((
int*) _temp1672) == Cyc_Absyn_Int_p: 0){ _LL1718: _temp1717=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1672)->f1; goto _LL1716; _LL1716: _temp1715=((
struct Cyc_Absyn_Int_p_struct*) _temp1672)->f2; goto _LL1683;} else{ goto
_LL1684;} _LL1684: if(( unsigned int) _temp1672 > 2u?*(( int*) _temp1672) == Cyc_Absyn_Char_p:
0){ _LL1720: _temp1719=(( struct Cyc_Absyn_Char_p_struct*) _temp1672)->f1; goto
_LL1685;} else{ goto _LL1686;} _LL1686: if(( unsigned int) _temp1672 > 2u?*((
int*) _temp1672) == Cyc_Absyn_Float_p: 0){ _LL1722: _temp1721=(( struct Cyc_Absyn_Float_p_struct*)
_temp1672)->f1; goto _LL1687;} else{ goto _LL1688;} _LL1688: if(( unsigned int)
_temp1672 > 2u?*(( int*) _temp1672) == Cyc_Absyn_Enum_p: 0){ _LL1726: _temp1725=((
struct Cyc_Absyn_Enum_p_struct*) _temp1672)->f1; goto _LL1724; _LL1724:
_temp1723=(( struct Cyc_Absyn_Enum_p_struct*) _temp1672)->f2; goto _LL1689;}
else{ goto _LL1690;} _LL1690: if(( unsigned int) _temp1672 > 2u?*(( int*)
_temp1672) == Cyc_Absyn_Tunion_p: 0){ _LL1734: _temp1733=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1672)->f1; goto _LL1732; _LL1732: _temp1731=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1672)->f2; goto _LL1730; _LL1730: _temp1729=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1672)->f3; goto _LL1728; _LL1728: _temp1727=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1672)->f4; if( _temp1727 == 0){ goto _LL1691;} else{ goto _LL1692;}} else{
goto _LL1692;} _LL1692: if(( unsigned int) _temp1672 > 2u?*(( int*) _temp1672)
== Cyc_Absyn_Pointer_p: 0){ _LL1737: _temp1736=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1672)->f1; _temp1738=* _temp1736; _LL1740: _temp1739=( void*) _temp1738.r;
if(( unsigned int) _temp1739 > 2u?*(( int*) _temp1739) == Cyc_Absyn_Tunion_p: 0){
_LL1748: _temp1747=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1739)->f1; goto
_LL1746; _LL1746: _temp1745=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1739)->f2;
goto _LL1744; _LL1744: _temp1743=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1739)->f3;
goto _LL1742; _LL1742: _temp1741=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1739)->f4;
goto _LL1735;} else{ goto _LL1694;}} else{ goto _LL1694;} _LL1735: if( _temp1741
!= 0){ goto _LL1693;} else{ goto _LL1694;} _LL1694: if(( unsigned int) _temp1672
> 2u?*(( int*) _temp1672) == Cyc_Absyn_Tunion_p: 0){ _LL1750: _temp1749=((
struct Cyc_Absyn_Tunion_p_struct*) _temp1672)->f4; goto _LL1695;} else{ goto
_LL1696;} _LL1696: if(( unsigned int) _temp1672 > 2u?*(( int*) _temp1672) == Cyc_Absyn_Tuple_p:
0){ _LL1752: _temp1751=(( struct Cyc_Absyn_Tuple_p_struct*) _temp1672)->f1; goto
_LL1697;} else{ goto _LL1698;} _LL1698: if(( unsigned int) _temp1672 > 2u?*((
int*) _temp1672) == Cyc_Absyn_Struct_p: 0){ _LL1760: _temp1759=(( struct Cyc_Absyn_Struct_p_struct*)
_temp1672)->f1; goto _LL1758; _LL1758: _temp1757=(( struct Cyc_Absyn_Struct_p_struct*)
_temp1672)->f2; goto _LL1756; _LL1756: _temp1755=(( struct Cyc_Absyn_Struct_p_struct*)
_temp1672)->f3; goto _LL1754; _LL1754: _temp1753=(( struct Cyc_Absyn_Struct_p_struct*)
_temp1672)->f4; goto _LL1699;} else{ goto _LL1700;} _LL1700: if(( unsigned int)
_temp1672 > 2u?*(( int*) _temp1672) == Cyc_Absyn_Pointer_p: 0){ _LL1762:
_temp1761=(( struct Cyc_Absyn_Pointer_p_struct*) _temp1672)->f1; goto _LL1701;}
else{ goto _LL1702;} _LL1702: if(( unsigned int) _temp1672 > 2u?*(( int*)
_temp1672) == Cyc_Absyn_UnknownId_p: 0){ goto _LL1703;} else{ goto _LL1704;}
_LL1704: if(( unsigned int) _temp1672 > 2u?*(( int*) _temp1672) == Cyc_Absyn_UnknownCall_p:
0){ goto _LL1705;} else{ goto _LL1706;} _LL1706: if(( unsigned int) _temp1672 >
2u?*(( int*) _temp1672) == Cyc_Absyn_UnknownFields_p: 0){ goto _LL1707;} else{
goto _LL1673;} _LL1675: nv= Cyc_Toc_add_varmap( nv, _temp1711, r); goto _LL1677;
_LL1677: s= Cyc_Absyn_goto_stmt( succ_lab, 0); goto _LL1673; _LL1679: { struct
_tuple0* _temp1763= Cyc_Toc_temp_var(); decls=({ struct Cyc_List_List* _temp1764=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1764->hd=( void*)({ struct _tuple12* _temp1765=( struct _tuple12*)
_region_malloc( rgn, sizeof( struct _tuple12)); _temp1765->f1= _temp1763;
_temp1765->f2= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq);
_temp1765;}); _temp1764->tl= decls; _temp1764;}); nv= Cyc_Toc_add_varmap( nv,
_temp1713->name, Cyc_Absyn_var_exp( _temp1763, 0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp1763, 0), Cyc_Absyn_address_exp( path, 0), 0), Cyc_Absyn_goto_stmt(
succ_lab, 0), 0); goto _LL1673;} _LL1681: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp(
0, 0), succ_lab, fail_lab); goto _LL1673; _LL1683: s= Cyc_Toc_if_eq_goto_stmt( r,
Cyc_Absyn_int_exp( _temp1717, _temp1715, 0), succ_lab, fail_lab); goto _LL1673;
_LL1685: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp( _temp1719, 0),
succ_lab, fail_lab); goto _LL1673; _LL1687: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp(
_temp1721, 0), succ_lab, fail_lab); goto _LL1673; _LL1689: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp1766=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp1766[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp1767; _temp1767.tag= Cyc_Absyn_Enum_e;
_temp1767.f1= _temp1723->name; _temp1767.f2=( struct Cyc_Absyn_Enumdecl*)
_temp1725; _temp1767.f3=( struct Cyc_Absyn_Enumfield*) _temp1723; _temp1767;});
_temp1766;}), 0), succ_lab, fail_lab); goto _LL1673; _LL1691: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp1731->name, 0); if( ! _temp1733->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL1673;} _LL1693: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1741);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp1745->name, _tag_arr("_struct",
sizeof( unsigned char), 8u)); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0); struct
Cyc_List_List* _temp1768=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1745->typs); struct Cyc_List_List*
_temp1769=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp1741); for( 0; _temp1769 != 0;((( _temp1769=((
struct Cyc_List_List*) _check_null( _temp1769))->tl, _temp1768=(( struct Cyc_List_List*)
_check_null( _temp1768))->tl)), cnt --)){ struct Cyc_Absyn_Pat* _temp1770=(
struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null( _temp1769))->hd;
if(( void*) _temp1770->r ==( void*) Cyc_Absyn_Wild_p){ continue;}{ void*
_temp1771=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1768))->hd)).f2;
struct _tuple0* _temp1772= Cyc_Toc_temp_var(); void* _temp1773=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1770->topt))->v; void* _temp1774= Cyc_Toc_typ_to_c(
_temp1773); struct _tagged_arr* _temp1775= Cyc_Toc_fresh_label(); struct Cyc_Absyn_Exp*
_temp1776= Cyc_Absyn_structarrow_exp( rcast, Cyc_Toc_fieldname( cnt), 0); if(
Cyc_Toc_is_void_star( Cyc_Toc_typ_to_c( _temp1771))){ _temp1776= Cyc_Absyn_cast_exp(
_temp1774, _temp1776, 0);} decls=({ struct Cyc_List_List* _temp1777=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1777->hd=( void*)({
struct _tuple12* _temp1778=( struct _tuple12*) _region_malloc( rgn, sizeof(
struct _tuple12)); _temp1778->f1= _temp1772; _temp1778->f2= _temp1774; _temp1778;});
_temp1777->tl= decls; _temp1777;});{ struct _tuple11 _temp1779= Cyc_Toc_xlate_pat(
nv, rgn, _temp1773, Cyc_Absyn_var_exp( _temp1772, 0), _temp1776, _temp1770,
succ_lab, fail_lab, decls); nv= _temp1779.f1; decls= _temp1779.f2;{ struct Cyc_Absyn_Stmt*
_temp1780= _temp1779.f3; struct Cyc_Absyn_Stmt* _temp1781= Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp1772, 0), _temp1776, 0); struct Cyc_Absyn_Stmt*
_temp1782= Cyc_Absyn_seq_stmt( _temp1781, _temp1780, 0); ss=({ struct Cyc_List_List*
_temp1783=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1783->hd=( void*) Cyc_Absyn_label_stmt( _temp1775, _temp1782, 0); _temp1783->tl=
ss; _temp1783;}); succ_lab= _temp1775;}}}} if( ss == 0){ ss=({ struct Cyc_List_List*
_temp1784=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1784->hd=( void*) Cyc_Absyn_goto_stmt( succ_lab, 0); _temp1784->tl= 0;
_temp1784;});}{ struct Cyc_Absyn_Exp* test_exp; if( _temp1747->is_xtunion){
struct Cyc_Absyn_Exp* e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(),
Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0);
struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp1745->name, 0); test_exp= Cyc_Absyn_eq_exp(
e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* _temp1785=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1747->fields))->v; for( 0; _temp1785 != 0; _temp1785=(( struct
Cyc_List_List*) _check_null( _temp1785))->tl){ struct Cyc_Absyn_Tunionfield*
_temp1786=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp1785))->hd; if( _temp1786->typs == 0){ max_tag ++;}}}{ struct Cyc_Absyn_Exp*
max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_t, r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp(
_temp1745->name, 0); test_exp= max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp(
e1, e, 0), 0): Cyc_Absyn_eq_exp( e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt(
test_exp, Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);
goto _LL1673;}} _LL1695: _temp1751= _temp1749; goto _LL1697; _LL1697: { struct
Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1751);{ struct Cyc_List_List* _temp1787=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1751); for(
0; _temp1787 != 0;( _temp1787=(( struct Cyc_List_List*) _check_null( _temp1787))->tl,
cnt --)){ struct Cyc_Absyn_Pat* _temp1788=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp1787))->hd; if(( void*) _temp1788->r ==( void*) Cyc_Absyn_Wild_p){
continue;}{ struct _tuple0* _temp1789= Cyc_Toc_temp_var(); void* _temp1790=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1788->topt))->v; struct
_tagged_arr* _temp1791= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1792=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1792->hd=( void*)({ struct _tuple12* _temp1793=( struct _tuple12*)
_region_malloc( rgn, sizeof( struct _tuple12)); _temp1793->f1= _temp1789;
_temp1793->f2= Cyc_Toc_typ_to_c( _temp1790); _temp1793;}); _temp1792->tl= decls;
_temp1792;});{ struct _tuple11 _temp1794= Cyc_Toc_xlate_pat( nv, rgn, _temp1790,
Cyc_Absyn_var_exp( _temp1789, 0), Cyc_Absyn_structmember_exp( path, Cyc_Toc_fieldname(
cnt), 0), _temp1788, succ_lab, fail_lab, decls); nv= _temp1794.f1; decls=
_temp1794.f2;{ struct Cyc_Absyn_Stmt* _temp1795= _temp1794.f3; struct Cyc_Absyn_Stmt*
_temp1796= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp1789, 0), Cyc_Absyn_structmember_exp( r, Cyc_Toc_fieldname( cnt), 0), 0),
_temp1795, 0); ss=({ struct Cyc_List_List* _temp1797=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1797->hd=( void*) Cyc_Absyn_label_stmt(
_temp1791, _temp1796, 0); _temp1797->tl= ss; _temp1797;}); succ_lab= _temp1791;}}}}}
if( ss == 0){ ss=({ struct Cyc_List_List* _temp1798=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1798->hd=( void*) Cyc_Absyn_goto_stmt(
succ_lab, 0); _temp1798->tl= 0; _temp1798;});} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL1673;} _LL1699: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
_temp1799=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp1753); for( 0; _temp1799 != 0; _temp1799=(( struct
Cyc_List_List*) _check_null( _temp1799))->tl){ struct _tuple13* _temp1800=(
struct _tuple13*)(( struct Cyc_List_List*) _check_null( _temp1799))->hd; struct
Cyc_Absyn_Pat* _temp1801=(* _temp1800).f2; if(( void*) _temp1801->r ==( void*)
Cyc_Absyn_Wild_p){ continue;}{ struct _tagged_arr* f;{ void* _temp1802=( void*)((
struct Cyc_List_List*) _check_null((* _temp1800).f1))->hd; struct _tagged_arr*
_temp1808; _LL1804: if(*(( int*) _temp1802) == Cyc_Absyn_FieldName){ _LL1809:
_temp1808=(( struct Cyc_Absyn_FieldName_struct*) _temp1802)->f1; goto _LL1805;}
else{ goto _LL1806;} _LL1806: goto _LL1807; _LL1805: f= _temp1808; goto _LL1803;
_LL1807:( int) _throw(( void*) Cyc_Toc_Match_error); _LL1803:;}{ struct _tuple0*
_temp1810= Cyc_Toc_temp_var(); void* _temp1811=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1801->topt))->v; void* _temp1812= Cyc_Toc_typ_to_c( _temp1811);
struct _tagged_arr* _temp1813= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1814=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1814->hd=( void*)({ struct _tuple12* _temp1815=( struct _tuple12*)
_region_malloc( rgn, sizeof( struct _tuple12)); _temp1815->f1= _temp1810;
_temp1815->f2= _temp1812; _temp1815;}); _temp1814->tl= decls; _temp1814;});{
struct _tuple11 _temp1816= Cyc_Toc_xlate_pat( nv, rgn, _temp1811, Cyc_Absyn_var_exp(
_temp1810, 0), Cyc_Absyn_structmember_exp( path, f, 0), _temp1801, succ_lab,
fail_lab, decls); nv= _temp1816.f1; decls= _temp1816.f2;{ struct Cyc_Absyn_Exp*
_temp1817= Cyc_Absyn_structmember_exp( r, f, 0); if( Cyc_Toc_is_void_star(( void*)((
struct Cyc_Absyn_Structfield*) _check_null( Cyc_Absyn_lookup_field(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1759->fields))->v, f)))->type)){
_temp1817= Cyc_Absyn_cast_exp( _temp1812, _temp1817, 0);}{ struct Cyc_Absyn_Stmt*
_temp1818= _temp1816.f3; struct Cyc_Absyn_Stmt* _temp1819= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1810, 0), _temp1817, 0),
_temp1818, 0); ss=({ struct Cyc_List_List* _temp1820=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1820->hd=( void*) Cyc_Absyn_label_stmt(
_temp1813, _temp1819, 0); _temp1820->tl= ss; _temp1820;}); succ_lab= _temp1813;}}}}}}}
if( ss == 0){ ss=({ struct Cyc_List_List* _temp1821=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1821->hd=( void*) Cyc_Absyn_goto_stmt(
succ_lab, 0); _temp1821->tl= 0; _temp1821;});} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL1673;} _LL1701: { struct _tuple0* _temp1822= Cyc_Toc_temp_var(); void*
_temp1823=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1761->topt))->v;
decls=({ struct Cyc_List_List* _temp1824=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp1824->hd=( void*)({ struct _tuple12*
_temp1825=( struct _tuple12*) _region_malloc( rgn, sizeof( struct _tuple12));
_temp1825->f1= _temp1822; _temp1825->f2= Cyc_Toc_typ_to_c( _temp1823); _temp1825;});
_temp1824->tl= decls; _temp1824;});{ struct _tuple11 _temp1826= Cyc_Toc_xlate_pat(
nv, rgn, _temp1823, Cyc_Absyn_var_exp( _temp1822, 0), Cyc_Absyn_deref_exp( path,
0), _temp1761, succ_lab, fail_lab, decls); nv= _temp1826.f1; decls= _temp1826.f2;{
struct Cyc_Absyn_Stmt* _temp1827= _temp1826.f3; struct Cyc_Absyn_Stmt* _temp1828=
Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1822, 0), Cyc_Absyn_deref_exp(
r, 0), 0), _temp1827, 0); if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp1828, 0);} else{ s= _temp1828;} goto _LL1673;}}} _LL1703: s=((
struct Cyc_Absyn_Stmt*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknownid",
sizeof( unsigned char), 10u)); goto _LL1673; _LL1705: s=(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknowncall", sizeof(
unsigned char), 12u)); goto _LL1673; _LL1707: s=(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknownfields", sizeof(
unsigned char), 14u)); goto _LL1673; _LL1673:;} return({ struct _tuple11
_temp1829; _temp1829.f1= nv; _temp1829.f2= decls; _temp1829.f3= s; _temp1829;});}
struct _tuple14{ struct _tagged_arr* f1; struct _tagged_arr* f2; struct Cyc_Absyn_Switch_clause*
f3; } ; static struct _tuple14* Cyc_Toc_gen_label( struct _RegionHandle* r,
struct Cyc_Absyn_Switch_clause* sc){ return({ struct _tuple14* _temp1830=(
struct _tuple14*) _region_malloc( r, sizeof( struct _tuple14)); _temp1830->f1=
Cyc_Toc_fresh_label(); _temp1830->f2= Cyc_Toc_fresh_label(); _temp1830->f3= sc;
_temp1830;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* scs){
Cyc_Toc_exp_to_c( nv, e);{ void* _temp1831=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; int leave_as_switch;{ void* _temp1832= Cyc_Tcutil_compress(
_temp1831); _LL1834: if(( unsigned int) _temp1832 > 4u?*(( int*) _temp1832) ==
Cyc_Absyn_IntType: 0){ goto _LL1835;} else{ goto _LL1836;} _LL1836: goto _LL1837;
_LL1835: leave_as_switch= 1; goto _LL1833; _LL1837: leave_as_switch= 0; goto
_LL1833; _LL1833:;}{ struct Cyc_List_List* _temp1838= scs; for( 0; _temp1838 !=
0; _temp1838=(( struct Cyc_List_List*) _check_null( _temp1838))->tl){ if((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp1838))->hd)->pat_vars))->v != 0? 1:((
struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( _temp1838))->hd)->where_clause
!= 0){ leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_arr*
next_l= Cyc_Toc_fresh_label();{ struct Cyc_List_List* _temp1839= scs; for( 0;
_temp1839 != 0; _temp1839=(( struct Cyc_List_List*) _check_null( _temp1839))->tl){
struct Cyc_Absyn_Stmt* _temp1840=(( struct Cyc_Absyn_Switch_clause*)(( struct
Cyc_List_List*) _check_null( _temp1839))->hd)->body;(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp1839))->hd)->body= Cyc_Absyn_label_stmt(
next_l, _temp1840, 0); next_l= Cyc_Toc_fresh_label(); Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env(
nv, next_l), _temp1840);}} return;}{ struct _tuple0* v= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp( v, 0); struct Cyc_Absyn_Exp* path=
Cyc_Absyn_var_exp( v, 0); struct _tagged_arr* end_l= Cyc_Toc_fresh_label();
struct _RegionHandle _temp1841= _new_region(); struct _RegionHandle* rgn=&
_temp1841; _push_region( rgn);{ struct Cyc_List_List* lscs=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple14*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Toc_gen_label,
rgn, scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0;
struct Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp1842= lscs; for( 0;
_temp1842 != 0; _temp1842=(( struct Cyc_List_List*) _check_null( _temp1842))->tl){
struct Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null( _temp1842))->hd)).f3; struct _tagged_arr* fail_lab=(( struct Cyc_List_List*)
_check_null( _temp1842))->tl == 0? end_l:(*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1842))->tl))->hd)).f1;
struct _tagged_arr* succ_lab=(*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null( _temp1842))->hd)).f2; if( sc->where_clause == 0){ struct _tuple11
_temp1843= Cyc_Toc_xlate_pat( nv, rgn, _temp1831, r, path, sc->pattern, succ_lab,
fail_lab, decls); nvs=({ struct Cyc_List_List* _temp1844=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1844->hd=( void*)
_temp1843.f1; _temp1844->tl= nvs; _temp1844;}); decls= _temp1843.f2; test_stmts=({
struct Cyc_List_List* _temp1845=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp1845->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( _temp1842))->hd)).f1,
_temp1843.f3, 0); _temp1845->tl= test_stmts; _temp1845;});} else{ struct Cyc_Absyn_Exp*
_temp1846=( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause); struct
_tagged_arr* _temp1847= Cyc_Toc_fresh_label(); struct _tuple11 _temp1848= Cyc_Toc_xlate_pat(
nv, rgn, _temp1831, r, path, sc->pattern, _temp1847, fail_lab, decls); Cyc_Toc_exp_to_c(
_temp1848.f1, _temp1846); nvs=({ struct Cyc_List_List* _temp1849=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1849->hd=( void*)
_temp1848.f1; _temp1849->tl= nvs; _temp1849;}); decls= _temp1848.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp1846, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp1847, s2,
0); test_stmts=({ struct Cyc_List_List* _temp1850=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1850->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( _temp1842))->hd)).f1,
Cyc_Absyn_seq_stmt( _temp1848.f3, s3, 0), 0); _temp1850->tl= test_stmts;
_temp1850;});}}}}{ struct Cyc_Absyn_Stmt* _temp1851= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
test_stmts), 0); nvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt( 0); if( lscs != 0){
for( 0;(( struct Cyc_List_List*) _check_null( lscs))->tl != 0;( lscs=(( struct
Cyc_List_List*) _check_null( lscs))->tl, nvs=(( struct Cyc_List_List*)
_check_null( nvs))->tl)){ struct Cyc_Toc_Env* _temp1852=( struct Cyc_Toc_Env*)((
struct Cyc_List_List*) _check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env( _temp1852, end_l,(*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( lscs))->tl))->hd)).f2,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null(((*(( struct _tuple14*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f3)->pat_vars))->v,(
struct Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( nvs))->tl))->hd), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0),
0);}{ struct Cyc_Toc_Env* _temp1853=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*(( struct _tuple14*)((
struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body; Cyc_Toc_stmt_to_c(
Cyc_Toc_last_switchclause_env( _temp1853, end_l), s); clauses= Cyc_Absyn_seq_stmt(
clauses, Cyc_Absyn_label_stmt((*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null( lscs))->hd)).f2, s, 0), 0);}}{ struct Cyc_Absyn_Stmt* res= Cyc_Absyn_seq_stmt(
_temp1851, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt( end_l, Cyc_Absyn_skip_stmt(
0), 0), 0), 0); for( decls; decls != 0; decls=(( struct Cyc_List_List*)
_check_null( decls))->tl){ struct _tuple12 _temp1856; void* _temp1857; struct
_tuple0* _temp1859; struct _tuple12* _temp1854=( struct _tuple12*)(( struct Cyc_List_List*)
_check_null( decls))->hd; _temp1856=* _temp1854; _LL1860: _temp1859= _temp1856.f1;
goto _LL1858; _LL1858: _temp1857= _temp1856.f2; goto _LL1855; _LL1855: res= Cyc_Absyn_declare_stmt(
_temp1859, _temp1857, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}; _pop_region( rgn);}}} static
struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat*
p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt*
s); static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv, struct Cyc_Toc_Env*
init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt* s); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__npop_handler_e,({ struct Cyc_List_List* _temp1861=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1861[ 0]=({ struct Cyc_List_List
_temp1862; _temp1862.hd=( void*) Cyc_Absyn_uint_exp(( unsigned int)( n - 1), 0);
_temp1862.tl= 0; _temp1862;}); _temp1861;}), 0), 0);} void Cyc_Toc_do_npop_before(
int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt(( void*) s->r,
0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* s){ while( 1) { void* _temp1863=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1905; struct Cyc_Absyn_Stmt* _temp1907; struct Cyc_Absyn_Stmt* _temp1909;
struct Cyc_Absyn_Exp* _temp1911; struct Cyc_Absyn_Stmt* _temp1913; struct Cyc_Absyn_Stmt*
_temp1915; struct Cyc_Absyn_Exp* _temp1917; struct Cyc_Absyn_Stmt* _temp1919;
struct _tuple2 _temp1921; struct Cyc_Absyn_Exp* _temp1923; struct Cyc_Absyn_Stmt*
_temp1925; struct Cyc_Absyn_Stmt* _temp1927; struct Cyc_Absyn_Stmt* _temp1929;
struct Cyc_Absyn_Stmt* _temp1931; struct _tuple2 _temp1933; struct Cyc_Absyn_Exp*
_temp1935; struct _tuple2 _temp1937; struct Cyc_Absyn_Exp* _temp1939; struct Cyc_Absyn_Exp*
_temp1941; struct Cyc_List_List* _temp1943; struct Cyc_Absyn_Exp* _temp1945;
struct Cyc_Absyn_Switch_clause** _temp1947; struct Cyc_List_List* _temp1949;
struct Cyc_Absyn_Stmt* _temp1951; struct Cyc_Absyn_Decl* _temp1953; struct Cyc_Absyn_Stmt*
_temp1955; struct _tagged_arr* _temp1957; struct _tuple2 _temp1959; struct Cyc_Absyn_Exp*
_temp1961; struct Cyc_Absyn_Stmt* _temp1963; struct Cyc_List_List* _temp1965;
struct Cyc_Absyn_Stmt* _temp1967; struct Cyc_Absyn_Stmt* _temp1969; struct Cyc_Absyn_Vardecl*
_temp1971; struct Cyc_Absyn_Tvar* _temp1973; _LL1865: if( _temp1863 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL1866;} else{ goto _LL1867;} _LL1867: if((
unsigned int) _temp1863 > 1u?*(( int*) _temp1863) == Cyc_Absyn_Exp_s: 0){
_LL1906: _temp1905=(( struct Cyc_Absyn_Exp_s_struct*) _temp1863)->f1; goto
_LL1868;} else{ goto _LL1869;} _LL1869: if(( unsigned int) _temp1863 > 1u?*((
int*) _temp1863) == Cyc_Absyn_Seq_s: 0){ _LL1910: _temp1909=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1863)->f1; goto _LL1908; _LL1908: _temp1907=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1863)->f2; goto _LL1870;} else{ goto _LL1871;} _LL1871: if(( unsigned int)
_temp1863 > 1u?*(( int*) _temp1863) == Cyc_Absyn_Return_s: 0){ _LL1912:
_temp1911=(( struct Cyc_Absyn_Return_s_struct*) _temp1863)->f1; goto _LL1872;}
else{ goto _LL1873;} _LL1873: if(( unsigned int) _temp1863 > 1u?*(( int*)
_temp1863) == Cyc_Absyn_IfThenElse_s: 0){ _LL1918: _temp1917=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1863)->f1; goto _LL1916; _LL1916: _temp1915=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1863)->f2; goto _LL1914; _LL1914: _temp1913=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1863)->f3; goto _LL1874;} else{ goto _LL1875;} _LL1875: if(( unsigned int)
_temp1863 > 1u?*(( int*) _temp1863) == Cyc_Absyn_While_s: 0){ _LL1922: _temp1921=((
struct Cyc_Absyn_While_s_struct*) _temp1863)->f1; _LL1924: _temp1923= _temp1921.f1;
goto _LL1920; _LL1920: _temp1919=(( struct Cyc_Absyn_While_s_struct*) _temp1863)->f2;
goto _LL1876;} else{ goto _LL1877;} _LL1877: if(( unsigned int) _temp1863 > 1u?*((
int*) _temp1863) == Cyc_Absyn_Break_s: 0){ _LL1926: _temp1925=(( struct Cyc_Absyn_Break_s_struct*)
_temp1863)->f1; goto _LL1878;} else{ goto _LL1879;} _LL1879: if(( unsigned int)
_temp1863 > 1u?*(( int*) _temp1863) == Cyc_Absyn_Continue_s: 0){ _LL1928:
_temp1927=(( struct Cyc_Absyn_Continue_s_struct*) _temp1863)->f1; goto _LL1880;}
else{ goto _LL1881;} _LL1881: if(( unsigned int) _temp1863 > 1u?*(( int*)
_temp1863) == Cyc_Absyn_Goto_s: 0){ _LL1930: _temp1929=(( struct Cyc_Absyn_Goto_s_struct*)
_temp1863)->f2; goto _LL1882;} else{ goto _LL1883;} _LL1883: if(( unsigned int)
_temp1863 > 1u?*(( int*) _temp1863) == Cyc_Absyn_For_s: 0){ _LL1942: _temp1941=((
struct Cyc_Absyn_For_s_struct*) _temp1863)->f1; goto _LL1938; _LL1938: _temp1937=((
struct Cyc_Absyn_For_s_struct*) _temp1863)->f2; _LL1940: _temp1939= _temp1937.f1;
goto _LL1934; _LL1934: _temp1933=(( struct Cyc_Absyn_For_s_struct*) _temp1863)->f3;
_LL1936: _temp1935= _temp1933.f1; goto _LL1932; _LL1932: _temp1931=(( struct Cyc_Absyn_For_s_struct*)
_temp1863)->f4; goto _LL1884;} else{ goto _LL1885;} _LL1885: if(( unsigned int)
_temp1863 > 1u?*(( int*) _temp1863) == Cyc_Absyn_Switch_s: 0){ _LL1946:
_temp1945=(( struct Cyc_Absyn_Switch_s_struct*) _temp1863)->f1; goto _LL1944;
_LL1944: _temp1943=(( struct Cyc_Absyn_Switch_s_struct*) _temp1863)->f2; goto
_LL1886;} else{ goto _LL1887;} _LL1887: if(( unsigned int) _temp1863 > 1u?*((
int*) _temp1863) == Cyc_Absyn_Fallthru_s: 0){ _LL1950: _temp1949=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1863)->f1; goto _LL1948; _LL1948: _temp1947=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1863)->f2; goto _LL1888;} else{ goto _LL1889;} _LL1889: if(( unsigned int)
_temp1863 > 1u?*(( int*) _temp1863) == Cyc_Absyn_Decl_s: 0){ _LL1954: _temp1953=((
struct Cyc_Absyn_Decl_s_struct*) _temp1863)->f1; goto _LL1952; _LL1952:
_temp1951=(( struct Cyc_Absyn_Decl_s_struct*) _temp1863)->f2; goto _LL1890;}
else{ goto _LL1891;} _LL1891: if(( unsigned int) _temp1863 > 1u?*(( int*)
_temp1863) == Cyc_Absyn_Label_s: 0){ _LL1958: _temp1957=(( struct Cyc_Absyn_Label_s_struct*)
_temp1863)->f1; goto _LL1956; _LL1956: _temp1955=(( struct Cyc_Absyn_Label_s_struct*)
_temp1863)->f2; goto _LL1892;} else{ goto _LL1893;} _LL1893: if(( unsigned int)
_temp1863 > 1u?*(( int*) _temp1863) == Cyc_Absyn_Do_s: 0){ _LL1964: _temp1963=((
struct Cyc_Absyn_Do_s_struct*) _temp1863)->f1; goto _LL1960; _LL1960: _temp1959=((
struct Cyc_Absyn_Do_s_struct*) _temp1863)->f2; _LL1962: _temp1961= _temp1959.f1;
goto _LL1894;} else{ goto _LL1895;} _LL1895: if(( unsigned int) _temp1863 > 1u?*((
int*) _temp1863) == Cyc_Absyn_TryCatch_s: 0){ _LL1968: _temp1967=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1863)->f1; goto _LL1966; _LL1966: _temp1965=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1863)->f2; goto _LL1896;} else{ goto _LL1897;} _LL1897: if(( unsigned int)
_temp1863 > 1u?*(( int*) _temp1863) == Cyc_Absyn_Region_s: 0){ _LL1974:
_temp1973=(( struct Cyc_Absyn_Region_s_struct*) _temp1863)->f1; goto _LL1972;
_LL1972: _temp1971=(( struct Cyc_Absyn_Region_s_struct*) _temp1863)->f2; goto
_LL1970; _LL1970: _temp1969=(( struct Cyc_Absyn_Region_s_struct*) _temp1863)->f3;
goto _LL1898;} else{ goto _LL1899;} _LL1899: if(( unsigned int) _temp1863 > 1u?*((
int*) _temp1863) == Cyc_Absyn_SwitchC_s: 0){ goto _LL1900;} else{ goto _LL1901;}
_LL1901: if(( unsigned int) _temp1863 > 1u?*(( int*) _temp1863) == Cyc_Absyn_Cut_s:
0){ goto _LL1902;} else{ goto _LL1903;} _LL1903: if(( unsigned int) _temp1863 >
1u?*(( int*) _temp1863) == Cyc_Absyn_Splice_s: 0){ goto _LL1904;} else{ goto
_LL1864;} _LL1866: return; _LL1868: Cyc_Toc_exp_to_c( nv, _temp1905); return;
_LL1870: Cyc_Toc_stmt_to_c( nv, _temp1909); s= _temp1907; continue; _LL1872: {
struct Cyc_Core_Opt* topt= 0; if( _temp1911 != 0){ topt=({ struct Cyc_Core_Opt*
_temp1975=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1975->v=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*) _check_null( _temp1911))->topt))->v);
_temp1975;}); Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null(
_temp1911));} if( s->try_depth > 0){ if( topt != 0){ struct _tuple0* _temp1976=
Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* _temp1977= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( _temp1976, 0), 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( _temp1976,( void*)(( struct Cyc_Core_Opt*)
_check_null( topt))->v, _temp1911, Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler(
s->try_depth), _temp1977, 0), 0))->r));} else{ Cyc_Toc_do_npop_before( s->try_depth,
s);}} return;} _LL1874: Cyc_Toc_exp_to_c( nv, _temp1917); Cyc_Toc_stmt_to_c( nv,
_temp1915); s= _temp1913; continue; _LL1876: Cyc_Toc_exp_to_c( nv, _temp1923);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1919); return; _LL1878: if( nv->break_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( nv->break_lab))->v, 0))->r));}{ int
dest_depth= _temp1925 == 0? 0:(( struct Cyc_Absyn_Stmt*) _check_null( _temp1925))->try_depth;
Cyc_Toc_do_npop_before( s->try_depth - dest_depth, s); return;} _LL1880: if( nv->continue_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( nv->continue_lab))->v, 0))->r));} _temp1929=
_temp1927; goto _LL1882; _LL1882: Cyc_Toc_do_npop_before( s->try_depth -((
struct Cyc_Absyn_Stmt*) _check_null( _temp1929))->try_depth, s); return; _LL1884:
Cyc_Toc_exp_to_c( nv, _temp1941); Cyc_Toc_exp_to_c( nv, _temp1939); Cyc_Toc_exp_to_c(
nv, _temp1935); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1931); return;
_LL1886: Cyc_Toc_xlate_switch( nv, s, _temp1945, _temp1943); return; _LL1888:
if( nv->fallthru_info == 0){( int) _throw( Cyc_Toc_toc_impos( _tag_arr("fallthru in unexpected place",
sizeof( unsigned char), 29u)));}{ struct _tuple6 _temp1980; struct Cyc_Dict_Dict*
_temp1981; struct Cyc_List_List* _temp1983; struct _tagged_arr* _temp1985;
struct _tuple6* _temp1978=( struct _tuple6*)(( struct Cyc_Core_Opt*) _check_null(
nv->fallthru_info))->v; _temp1980=* _temp1978; _LL1986: _temp1985= _temp1980.f1;
goto _LL1984; _LL1984: _temp1983= _temp1980.f2; goto _LL1982; _LL1982: _temp1981=
_temp1980.f3; goto _LL1979; _LL1979: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt(
_temp1985, 0); Cyc_Toc_do_npop_before( s->try_depth -((*(( struct Cyc_Absyn_Switch_clause**)
_check_null( _temp1947)))->body)->try_depth, s2);{ struct Cyc_List_List*
_temp1987=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1983); struct Cyc_List_List* _temp1988=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( _temp1949); for( 0; _temp1987 != 0;( _temp1987=((
struct Cyc_List_List*) _check_null( _temp1987))->tl, _temp1988=(( struct Cyc_List_List*)
_check_null( _temp1988))->tl)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1988))->hd); s2= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key)) Cyc_Dict_lookup)( _temp1981,( struct _tuple0*)(( struct
Cyc_List_List*) _check_null( _temp1987))->hd),( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1988))->hd, 0), s2, 0);}( void*)( s->r=( void*)((
void*) s2->r)); return;}}} _LL1890:{ void* _temp1989=( void*) _temp1953->r;
struct Cyc_Absyn_Vardecl* _temp1999; int _temp2001; struct Cyc_Absyn_Exp*
_temp2003; struct Cyc_Core_Opt* _temp2005; struct Cyc_Absyn_Pat* _temp2007;
struct Cyc_List_List* _temp2009; _LL1991: if(*(( int*) _temp1989) == Cyc_Absyn_Var_d){
_LL2000: _temp1999=(( struct Cyc_Absyn_Var_d_struct*) _temp1989)->f1; goto
_LL1992;} else{ goto _LL1993;} _LL1993: if(*(( int*) _temp1989) == Cyc_Absyn_Let_d){
_LL2008: _temp2007=(( struct Cyc_Absyn_Let_d_struct*) _temp1989)->f1; goto
_LL2006; _LL2006: _temp2005=(( struct Cyc_Absyn_Let_d_struct*) _temp1989)->f3;
goto _LL2004; _LL2004: _temp2003=(( struct Cyc_Absyn_Let_d_struct*) _temp1989)->f4;
goto _LL2002; _LL2002: _temp2001=(( struct Cyc_Absyn_Let_d_struct*) _temp1989)->f5;
goto _LL1994;} else{ goto _LL1995;} _LL1995: if(*(( int*) _temp1989) == Cyc_Absyn_Letv_d){
_LL2010: _temp2009=(( struct Cyc_Absyn_Letv_d_struct*) _temp1989)->f1; goto
_LL1996;} else{ goto _LL1997;} _LL1997: goto _LL1998; _LL1992: { struct Cyc_Toc_Env*
_temp2011= Cyc_Toc_add_varmap( nv, _temp1999->name, Cyc_Absyn_varb_exp(
_temp1999->name,( void*)({ struct Cyc_Absyn_Local_b_struct* _temp2012=( struct
Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp2012[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp2013; _temp2013.tag= Cyc_Absyn_Local_b;
_temp2013.f1= _temp1999; _temp2013;}); _temp2012;}), 0)); Cyc_Toc_local_decl_to_c(
_temp2011, _temp2011, _temp1999, _temp1951); goto _LL1990;} _LL1994:{ void*
_temp2014=( void*) _temp2007->r; struct Cyc_Absyn_Vardecl* _temp2020; _LL2016:
if(( unsigned int) _temp2014 > 2u?*(( int*) _temp2014) == Cyc_Absyn_Var_p: 0){
_LL2021: _temp2020=(( struct Cyc_Absyn_Var_p_struct*) _temp2014)->f1; goto
_LL2017;} else{ goto _LL2018;} _LL2018: goto _LL2019; _LL2017: { struct _tuple0*
old_name= _temp2020->name; struct _tuple0* new_name= Cyc_Toc_temp_var();
_temp2020->name= new_name; _temp2020->initializer=( struct Cyc_Absyn_Exp*)
_temp2003;( void*)( _temp1953->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2022=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2022[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2023; _temp2023.tag= Cyc_Absyn_Var_d;
_temp2023.f1= _temp2020; _temp2023;}); _temp2022;})));{ struct Cyc_Toc_Env*
_temp2024= Cyc_Toc_add_varmap( nv, old_name, Cyc_Absyn_varb_exp( new_name,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2025=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2025[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2026; _temp2026.tag= Cyc_Absyn_Local_b; _temp2026.f1=
_temp2020; _temp2026;}); _temp2025;}), 0)); Cyc_Toc_local_decl_to_c( _temp2024,
nv, _temp2020, _temp1951); goto _LL2015;}} _LL2019:( void*)( s->r=( void*)((
void*)( Cyc_Toc_letdecl_to_c( nv, _temp2007, _temp2005, _temp2003, _temp2001,
_temp1951))->r)); goto _LL2015; _LL2015:;} goto _LL1990; _LL1996: { struct Cyc_List_List*
_temp2027=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2009); if( _temp2027 == 0){(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("empty Letv_d", sizeof( unsigned char), 13u));}( void*)( _temp1953->r=(
void*)(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2028=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2028[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2029; _temp2029.tag= Cyc_Absyn_Var_d; _temp2029.f1=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp2027))->hd; _temp2029;}); _temp2028;})));
_temp2027=(( struct Cyc_List_List*) _check_null( _temp2027))->tl; for( 0;
_temp2027 != 0; _temp2027=(( struct Cyc_List_List*) _check_null( _temp2027))->tl){
struct Cyc_Absyn_Decl* _temp2030= Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2031=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2031[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2032; _temp2032.tag= Cyc_Absyn_Var_d;
_temp2032.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp2027))->hd; _temp2032;}); _temp2031;}), 0);( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_decl_stmt( _temp2030, Cyc_Absyn_new_stmt(( void*) s->r, 0), 0))->r));}
Cyc_Toc_stmt_to_c( nv, s); goto _LL1990;} _LL1998:(( int(*)( struct _tagged_arr))
Cyc_Tcutil_impos)( _tag_arr("bad nested declaration within function", sizeof(
unsigned char), 39u)); goto _LL1990; _LL1990:;} return; _LL1892: s= _temp1955;
continue; _LL1894: Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1963); Cyc_Toc_exp_to_c(
nv, _temp1961); return; _LL1896: { struct _tuple0* h_var= Cyc_Toc_temp_var();
struct _tuple0* e_var= Cyc_Toc_temp_var(); struct _tuple0* was_thrown_var= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* h_exp= Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp*
e_exp= Cyc_Absyn_var_exp( e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); e_exp->topt=({ struct Cyc_Core_Opt* _temp2033=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2033->v=( void*) e_typ; _temp2033;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp1967);{
struct Cyc_Absyn_Stmt* _temp2034= Cyc_Absyn_seq_stmt( _temp1967, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( Cyc_Toc__pop_handler_e, 0, 0), 0), 0); struct _tuple0*
_temp2035= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp2036= Cyc_Absyn_var_exp(
_temp2035, 0); struct Cyc_Absyn_Vardecl* _temp2037= Cyc_Absyn_new_vardecl(
_temp2035, Cyc_Absyn_exn_typ, 0); _temp2036->topt=({ struct Cyc_Core_Opt*
_temp2038=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2038->v=( void*) Cyc_Absyn_exn_typ; _temp2038;});{ struct Cyc_Absyn_Pat*
_temp2039=({ struct Cyc_Absyn_Pat* _temp2055=( struct Cyc_Absyn_Pat*) GC_malloc(
sizeof( struct Cyc_Absyn_Pat)); _temp2055->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp2057=( struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2057[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2058; _temp2058.tag= Cyc_Absyn_Var_p;
_temp2058.f1= _temp2037; _temp2058;}); _temp2057;})); _temp2055->topt=({ struct
Cyc_Core_Opt* _temp2056=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2056->v=( void*) Cyc_Absyn_exn_typ; _temp2056;}); _temp2055->loc= 0;
_temp2055;}); struct Cyc_Absyn_Exp* _temp2040= Cyc_Absyn_throw_exp( _temp2036, 0);
_temp2040->topt=({ struct Cyc_Core_Opt* _temp2041=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2041->v=( void*)(( void*) Cyc_Absyn_VoidType);
_temp2041;});{ struct Cyc_Absyn_Stmt* _temp2042= Cyc_Absyn_exp_stmt( _temp2040,
0); struct Cyc_Absyn_Switch_clause* _temp2043=({ struct Cyc_Absyn_Switch_clause*
_temp2052=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2052->pattern= _temp2039; _temp2052->pat_vars=({ struct Cyc_Core_Opt*
_temp2053=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2053->v=( void*)({ struct Cyc_List_List* _temp2054=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2054->hd=( void*) _temp2037;
_temp2054->tl= 0; _temp2054;}); _temp2053;}); _temp2052->where_clause= 0;
_temp2052->body= _temp2042; _temp2052->loc= 0; _temp2052;}); struct Cyc_Absyn_Stmt*
_temp2044= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp1965,({ struct Cyc_List_List*
_temp2051=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2051->hd=( void*) _temp2043; _temp2051->tl= 0; _temp2051;})), 0); Cyc_Toc_stmt_to_c(
nv, _temp2044);{ struct Cyc_Absyn_Exp* _temp2045= Cyc_Absyn_fncall_exp( Cyc_Toc_setjmp_e,({
struct Cyc_List_List* _temp2050=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2050->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp2050->tl= 0; _temp2050;}), 0); struct Cyc_Absyn_Stmt*
_temp2046= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__push_handler_e,({
struct Cyc_List_List* _temp2049=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2049->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0);
_temp2049->tl= 0; _temp2049;}), 0), 0); struct Cyc_Absyn_Exp* _temp2047= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 0, 0); struct Cyc_Absyn_Exp* _temp2048= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 1, 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp2046, Cyc_Absyn_declare_stmt(
was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp2047, Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt( _temp2045, Cyc_Absyn_assign_stmt( was_thrown_exp,
_temp2048, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp2034, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, Cyc_Toc__exn_thrown_e,
0), _temp2044, 0), 0), 0), 0), 0), 0))->r)); return;}}}}} _LL1898: { void*
rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); struct
_tuple0* rh_var= Cyc_Toc_temp_var(); struct _tuple0* x_var= _temp1971->name;
struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp( rh_var, 0); struct Cyc_Absyn_Exp*
x_exp= Cyc_Absyn_var_exp( x_var, 0); Cyc_Toc_stmt_to_c( Cyc_Toc_add_varmap( nv,
x_var, x_exp), _temp1969);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_fncall_exp( Cyc_Toc__new_region_e,
0, 0), Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp( rh_exp, 0), Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__push_region_e,({ struct Cyc_List_List*(* _temp2059)( struct _tagged_arr)=(
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp*
_temp2062= x_exp; struct Cyc_Absyn_Exp* _temp2060[ 1u]={ _temp2062}; struct
_tagged_arr _temp2061={( void*) _temp2060,( void*) _temp2060,( void*)( _temp2060
+ 1u)}; _temp2059( _temp2061);}), 0), 0), Cyc_Absyn_seq_stmt( _temp1969, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( Cyc_Toc__pop_region_e,({ struct Cyc_List_List*(* _temp2063)(
struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp2066= x_exp; struct Cyc_Absyn_Exp* _temp2064[ 1u]={
_temp2066}; struct _tagged_arr _temp2065={( void*) _temp2064,( void*) _temp2064,(
void*)( _temp2064 + 1u)}; _temp2063( _temp2065);}), 0), 0), 0), 0), 0), 0))->r));
return;} _LL1900:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("switchC_s",
sizeof( unsigned char), 10u)); return; _LL1902:(( int(*)( struct _tagged_arr s))
Cyc_Toc_unimp)( _tag_arr("cut", sizeof( unsigned char), 4u)); return; _LL1904:((
int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("splice", sizeof(
unsigned char), 7u)); return; _LL1864:;}} struct _tuple15{ struct _tagged_arr*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl* f){ f->tvs= 0; f->effect= 0; f->rgn_po=
0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c(( void*) f->ret_type));{
struct Cyc_List_List* _temp2067= f->args; for( 0; _temp2067 != 0; _temp2067=((
struct Cyc_List_List*) _check_null( _temp2067))->tl){(*(( struct _tuple15*)((
struct Cyc_List_List*) _check_null( _temp2067))->hd)).f3= Cyc_Toc_typ_to_c((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( _temp2067))->hd)).f3);{
struct _tuple0* _temp2068=({ struct _tuple0* _temp2069=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp2069->f1=( void*) Cyc_Absyn_Loc_n;
_temp2069->f2=(*(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
_temp2067))->hd)).f1; _temp2069;}); nv= Cyc_Toc_add_varmap( nv, _temp2068, Cyc_Absyn_var_exp(
_temp2068, 0));}}} if(( unsigned int) f->cyc_varargs?(( struct Cyc_Absyn_VarargInfo*)
_check_null( f->cyc_varargs))->name != 0: 0){ int _temp2072; void* _temp2074;
void* _temp2076; struct Cyc_Absyn_Tqual _temp2078; struct Cyc_Core_Opt*
_temp2080; struct Cyc_Absyn_VarargInfo _temp2070=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( f->cyc_varargs)); _LL2081: _temp2080= _temp2070.name; goto _LL2079;
_LL2079: _temp2078= _temp2070.tq; goto _LL2077; _LL2077: _temp2076=( void*)
_temp2070.type; goto _LL2075; _LL2075: _temp2074=( void*) _temp2070.rgn; goto
_LL2073; _LL2073: _temp2072= _temp2070.inject; goto _LL2071; _LL2071: { void*
_temp2082= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ( _temp2076, _temp2074,
_temp2078)); struct _tuple0* _temp2083=({ struct _tuple0* _temp2086=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2086->f1=( void*) Cyc_Absyn_Loc_n;
_temp2086->f2=( struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null(
_temp2080))->v; _temp2086;}); f->args=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( f->args,({ struct Cyc_List_List*
_temp2084=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2084->hd=( void*)({ struct _tuple15* _temp2085=( struct _tuple15*)
GC_malloc( sizeof( struct _tuple15)); _temp2085->f1=( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( _temp2080))->v; _temp2085->f2= _temp2078;
_temp2085->f3= _temp2082; _temp2085;}); _temp2084->tl= 0; _temp2084;})); nv= Cyc_Toc_add_varmap(
nv, _temp2083, Cyc_Absyn_var_exp( _temp2083, 0)); f->cyc_varargs= 0;}}{ struct
Cyc_List_List* _temp2087=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( f->param_vardecls))->v; for( 0; _temp2087 != 0; _temp2087=(( struct
Cyc_List_List*) _check_null( _temp2087))->tl){( void*)((( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp2087))->hd)->type=( void*) Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp2087))->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel( nv), f->body);}
static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2088= s; _LL2090: if(
_temp2088 ==( void*) Cyc_Absyn_Abstract){ goto _LL2091;} else{ goto _LL2092;}
_LL2092: if( _temp2088 ==( void*) Cyc_Absyn_ExternC){ goto _LL2093;} else{ goto
_LL2094;} _LL2094: goto _LL2095; _LL2091: return( void*) Cyc_Absyn_Public;
_LL2093: return( void*) Cyc_Absyn_Extern; _LL2095: return s; _LL2089:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp2096=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; if( Cyc_Toc_structs_so_far == 0){
Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp2097=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2097->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2097;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp2098=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(*
_temp2096).f2); if( _temp2098 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2096).f2, s));} else{ if(((
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Core_Opt*) _check_null( _temp2098))->v)->fields
== 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(* _temp2096).f2,
s)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)((
void*) Cyc_Absyn_Public)); s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp2099=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2099->v=(
void*) _temp2096; _temp2099;}); if( s->fields != 0){ if( seen_defn_before){ s->fields=
0;} else{ struct Cyc_List_List* _temp2100=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; for( 0; _temp2100 != 0; _temp2100=(( struct Cyc_List_List*)
_check_null( _temp2100))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2100))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2100))->hd)->type));}}}}} struct Cyc_Core_Opt* Cyc_Toc_unions_so_far= 0;
static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl* u){ struct
_tuple0* _temp2101=( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( u->name))->v;
if( Cyc_Toc_unions_so_far == 0){ Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt*
_temp2102=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2102->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2102;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp2103=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp2101).f2); if( _temp2103 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2101).f2, u));} else{ if(((
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Core_Opt*) _check_null( _temp2103))->v)->fields
== 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(* _temp2101).f2,
u)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)( u->sc=( void*)((
void*) Cyc_Absyn_Public)); u->tvs= 0; u->name=({ struct Cyc_Core_Opt* _temp2104=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2104->v=(
void*) _temp2101; _temp2104;}); if( u->fields != 0){ if( seen_defn_before){ u->fields=
0;} else{ struct Cyc_List_List* _temp2105=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( u->fields))->v; for( 0; _temp2105 != 0; _temp2105=(( struct Cyc_List_List*)
_check_null( _temp2105))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2105))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2105))->hd)->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp2106=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2106->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2106;});}{ struct
_tuple0* _temp2107= tud->name; if( tud->fields == 0){ return;} if((( int(*)(
struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)(( struct Cyc_Set_Set*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v, _temp2107)){
return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v=(
void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tuple0* elt))
Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp2107));{ int tag_count= 0; int box_count= 0; struct Cyc_List_List*
_temp2108=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( tud->fields))->v;
for( 0; _temp2108 != 0; _temp2108=(( struct Cyc_List_List*) _check_null(
_temp2108))->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2108))->hd; if( f->typs == 0){ struct
Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp( tag_count
++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name, Cyc_Absyn_sint_t,
eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2109; _temp2109.q_const= 1;
_temp2109.q_volatile= 0; _temp2109.q_restrict= 0; _temp2109;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2110=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2110->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2111=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2111[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2112; _temp2112.tag= Cyc_Absyn_Var_d; _temp2112.f1= vd; _temp2112;});
_temp2111;}), 0); _temp2110->tl= Cyc_Toc_result_decls; _temp2110;});} else{
struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2113; _temp2113.q_const=
1; _temp2113.q_volatile= 0; _temp2113.q_restrict= 0; _temp2113;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2114=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2114->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2115=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2115[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2116; _temp2116.tag= Cyc_Absyn_Var_d; _temp2116.f1= vd; _temp2116;});
_temp2115;}), 0); _temp2114->tl= Cyc_Toc_result_decls; _temp2114;});{ struct Cyc_List_List*
_temp2117= 0; int i= 1;{ struct Cyc_List_List* _temp2118= f->typs; for( 0;
_temp2118 != 0;( _temp2118=(( struct Cyc_List_List*) _check_null( _temp2118))->tl,
i ++)){ struct _tagged_arr* _temp2119= Cyc_Toc_fieldname( i); struct Cyc_Absyn_Structfield*
_temp2120=({ struct Cyc_Absyn_Structfield* _temp2122=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2122->name= _temp2119;
_temp2122->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null(
_temp2118))->hd)).f1; _temp2122->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2118))->hd)).f2);
_temp2122->width= 0; _temp2122->attributes= 0; _temp2122;}); _temp2117=({ struct
Cyc_List_List* _temp2121=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2121->hd=( void*) _temp2120; _temp2121->tl= _temp2117; _temp2121;});}}
_temp2117=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp2117); _temp2117=({ struct Cyc_List_List* _temp2123=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2123->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2124=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2124->name= Cyc_Toc_tag_sp; _temp2124->tq= Cyc_Toc_mt_tq; _temp2124->type=(
void*) Cyc_Absyn_sint_t; _temp2124->width= 0; _temp2124->attributes= 0;
_temp2124;}); _temp2123->tl= _temp2117; _temp2123;});{ struct Cyc_Absyn_Structdecl*
_temp2125=({ struct Cyc_Absyn_Structdecl* _temp2129=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2129->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2129->name=({ struct Cyc_Core_Opt* _temp2131=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2131->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp2131;}); _temp2129->tvs= 0; _temp2129->fields=({ struct Cyc_Core_Opt*
_temp2130=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2130->v=( void*) _temp2117; _temp2130;}); _temp2129->attributes= 0;
_temp2129;}); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2126=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2126->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2127=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2127[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2128; _temp2128.tag= Cyc_Absyn_Struct_d;
_temp2128.f1= _temp2125; _temp2128;}); _temp2127;}), 0); _temp2126->tl= Cyc_Toc_result_decls;
_temp2126;});}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far == 0){ Cyc_Toc_xtunion_fields_so_far=({ struct Cyc_Core_Opt*
_temp2132=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2132->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp2132;});} if( xd->fields
== 0){ return;}{ struct _tuple0* _temp2133= xd->name; struct Cyc_List_List*
_temp2134=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( xd->fields))->v;
for( 0; _temp2134 != 0; _temp2134=(( struct Cyc_List_List*) _check_null(
_temp2134))->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2134))->hd; struct _tagged_arr* fn=(* f->name).f2;
struct Cyc_Absyn_Exp* _temp2135= Cyc_Absyn_uint_exp( _get_arr_size(* fn, sizeof(
unsigned char)) + 4, 0); struct Cyc_Absyn_ArrayType_struct* _temp2136=({ struct
Cyc_Absyn_ArrayType_struct* _temp2206=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2206[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2207; _temp2207.tag= Cyc_Absyn_ArrayType;
_temp2207.f1=( void*) Cyc_Absyn_uchar_t; _temp2207.f2= Cyc_Toc_mt_tq; _temp2207.f3=(
struct Cyc_Absyn_Exp*) _temp2135; _temp2207;}); _temp2206;}); struct Cyc_Core_Opt*
_temp2137=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tuple0*
key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct Cyc_Core_Opt
_temp2145; int _temp2146; _LL2139: if( _temp2137 == 0){ goto _LL2140;} else{
goto _LL2141;} _LL2141: if( _temp2137 == 0){ goto _LL2143;} else{ _temp2145=*
_temp2137; _LL2147: _temp2146=( int) _temp2145.v; if( _temp2146 == 0){ goto
_LL2142;} else{ goto _LL2143;}} _LL2143: goto _LL2144; _LL2140: { struct Cyc_Absyn_Exp*
initopt= 0; if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){ initopt=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_string_exp(( struct _tagged_arr)({ struct _tagged_arr(* _temp2148)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp2156= _tag_arr("%c%c%c%c%s", sizeof( unsigned char), 11u);
struct Cyc_Stdio_Int_pa_struct _temp2161; _temp2161.tag= Cyc_Stdio_Int_pa;
_temp2161.f1=( int)(( unsigned int) 0);{ void* _temp2151=( void*)& _temp2161;
struct Cyc_Stdio_Int_pa_struct _temp2160; _temp2160.tag= Cyc_Stdio_Int_pa;
_temp2160.f1=( int)(( unsigned int) 0);{ void* _temp2152=( void*)& _temp2160;
struct Cyc_Stdio_Int_pa_struct _temp2159; _temp2159.tag= Cyc_Stdio_Int_pa;
_temp2159.f1=( int)(( unsigned int) 0);{ void* _temp2153=( void*)& _temp2159;
struct Cyc_Stdio_Int_pa_struct _temp2158; _temp2158.tag= Cyc_Stdio_Int_pa;
_temp2158.f1=( int)(( unsigned int) 0);{ void* _temp2154=( void*)& _temp2158;
struct Cyc_Stdio_String_pa_struct _temp2157; _temp2157.tag= Cyc_Stdio_String_pa;
_temp2157.f1=( struct _tagged_arr)* fn;{ void* _temp2155=( void*)& _temp2157;
void* _temp2149[ 5u]={ _temp2151, _temp2152, _temp2153, _temp2154, _temp2155};
struct _tagged_arr _temp2150={( void*) _temp2149,( void*) _temp2149,( void*)(
_temp2149 + 5u)}; _temp2148( _temp2156, _temp2150);}}}}}}), 0);}{ struct Cyc_Absyn_Vardecl*
_temp2162= Cyc_Absyn_new_vardecl( f->name,( void*) _temp2136, initopt);( void*)(
_temp2162->sc=( void*)(( void*) f->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2163=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2163->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2164=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2164[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2165; _temp2165.tag= Cyc_Absyn_Var_d;
_temp2165.f1= _temp2162; _temp2165;}); _temp2164;}), 0); _temp2163->tl= Cyc_Toc_result_decls;
_temp2163;});( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key,
int data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc !=( void*)
Cyc_Absyn_Extern)); if( f->typs != 0){ struct Cyc_List_List* fields= 0; int i= 1;{
struct Cyc_List_List* _temp2166= f->typs; for( 0; _temp2166 != 0;( _temp2166=((
struct Cyc_List_List*) _check_null( _temp2166))->tl, i ++)){ struct _tagged_arr*
_temp2167=({ struct _tagged_arr* _temp2171=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr)); _temp2171[ 0]=( struct _tagged_arr)({ struct
_tagged_arr(* _temp2172)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp2176= _tag_arr("f%d", sizeof( unsigned char), 4u);
struct Cyc_Stdio_Int_pa_struct _temp2177; _temp2177.tag= Cyc_Stdio_Int_pa;
_temp2177.f1=( int)(( unsigned int) i);{ void* _temp2175=( void*)& _temp2177;
void* _temp2173[ 1u]={ _temp2175}; struct _tagged_arr _temp2174={( void*)
_temp2173,( void*) _temp2173,( void*)( _temp2173 + 1u)}; _temp2172( _temp2176,
_temp2174);}}); _temp2171;}); struct Cyc_Absyn_Structfield* _temp2168=({ struct
Cyc_Absyn_Structfield* _temp2170=( struct Cyc_Absyn_Structfield*) GC_malloc(
sizeof( struct Cyc_Absyn_Structfield)); _temp2170->name= _temp2167; _temp2170->tq=(*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2166))->hd)).f1;
_temp2170->type=( void*) Cyc_Toc_typ_to_c_array((*(( struct _tuple3*)(( struct
Cyc_List_List*) _check_null( _temp2166))->hd)).f2); _temp2170->width= 0;
_temp2170->attributes= 0; _temp2170;}); fields=({ struct Cyc_List_List*
_temp2169=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2169->hd=( void*) _temp2168; _temp2169->tl= fields; _temp2169;});}} fields=({
struct Cyc_List_List* _temp2178=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2178->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2179=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2179->name= Cyc_Toc_tag_sp; _temp2179->tq= Cyc_Toc_mt_tq; _temp2179->type=(
void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t, Cyc_Toc_mt_tq); _temp2179->width=
0; _temp2179->attributes= 0; _temp2179;}); _temp2178->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields); _temp2178;});{ struct Cyc_Absyn_Structdecl*
_temp2180=({ struct Cyc_Absyn_Structdecl* _temp2184=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2184->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2184->name=({ struct Cyc_Core_Opt* _temp2186=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2186->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp2186;}); _temp2184->tvs= 0; _temp2184->fields=({ struct Cyc_Core_Opt*
_temp2185=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2185->v=( void*) fields; _temp2185;}); _temp2184->attributes= 0; _temp2184;});
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2181=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2181->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2182=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2182[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2183; _temp2183.tag= Cyc_Absyn_Struct_d;
_temp2183.f1= _temp2180; _temp2183;}); _temp2182;}), 0); _temp2181->tl= Cyc_Toc_result_decls;
_temp2181;});}} goto _LL2138;}} _LL2142: if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2187= Cyc_Absyn_string_exp(( struct _tagged_arr)({
struct _tagged_arr(* _temp2192)( struct _tagged_arr fmt, struct _tagged_arr)=
Cyc_Stdio_aprintf; struct _tagged_arr _temp2200= _tag_arr("%c%c%c%c%s", sizeof(
unsigned char), 11u); struct Cyc_Stdio_Int_pa_struct _temp2205; _temp2205.tag=
Cyc_Stdio_Int_pa; _temp2205.f1=( int)(( unsigned int) 0);{ void* _temp2195=(
void*)& _temp2205; struct Cyc_Stdio_Int_pa_struct _temp2204; _temp2204.tag= Cyc_Stdio_Int_pa;
_temp2204.f1=( int)(( unsigned int) 0);{ void* _temp2196=( void*)& _temp2204;
struct Cyc_Stdio_Int_pa_struct _temp2203; _temp2203.tag= Cyc_Stdio_Int_pa;
_temp2203.f1=( int)(( unsigned int) 0);{ void* _temp2197=( void*)& _temp2203;
struct Cyc_Stdio_Int_pa_struct _temp2202; _temp2202.tag= Cyc_Stdio_Int_pa;
_temp2202.f1=( int)(( unsigned int) 0);{ void* _temp2198=( void*)& _temp2202;
struct Cyc_Stdio_String_pa_struct _temp2201; _temp2201.tag= Cyc_Stdio_String_pa;
_temp2201.f1=( struct _tagged_arr)* fn;{ void* _temp2199=( void*)& _temp2201;
void* _temp2193[ 5u]={ _temp2195, _temp2196, _temp2197, _temp2198, _temp2199};
struct _tagged_arr _temp2194={( void*) _temp2193,( void*) _temp2193,( void*)(
_temp2193 + 5u)}; _temp2192( _temp2200, _temp2194);}}}}}}), 0); struct Cyc_Absyn_Vardecl*
_temp2188= Cyc_Absyn_new_vardecl( f->name,( void*) _temp2136,( struct Cyc_Absyn_Exp*)
_temp2187);( void*)( _temp2188->sc=( void*)(( void*) f->sc)); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2189=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2189->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2190=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2190[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2191; _temp2191.tag= Cyc_Absyn_Var_d; _temp2191.f1= _temp2188; _temp2191;});
_temp2190;}), 0); _temp2189->tl= Cyc_Toc_result_decls; _temp2189;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));} goto _LL2138; _LL2144: goto
_LL2138; _LL2138:;}}} static void Cyc_Toc_enumdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2208= ed->name;( void*)( ed->sc=(
void*)(( void*) Cyc_Absyn_Public)); if( ed->fields != 0){ struct Cyc_List_List*
_temp2209=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ed->fields))->v;
for( 0; _temp2209 != 0; _temp2209=(( struct Cyc_List_List*) _check_null(
_temp2209))->tl){ if((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2209))->hd)->tag != 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2209))->hd)->tag));(( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_List_List*) _check_null( _temp2209))->hd)->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
i, 0);}}}} static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv,
struct Cyc_Toc_Env* init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt*
s){( void*)( vd->type=( void*) Cyc_Toc_typ_to_c_array(( void*) vd->type)); Cyc_Toc_stmt_to_c(
body_nv, s); if( vd->initializer != 0){ struct Cyc_Absyn_Exp* init=( struct Cyc_Absyn_Exp*)
_check_null( vd->initializer); void* _temp2210=( void*) init->r; struct Cyc_Absyn_Exp*
_temp2216; struct Cyc_Absyn_Exp* _temp2218; struct Cyc_Absyn_Vardecl* _temp2220;
_LL2212: if(*(( int*) _temp2210) == Cyc_Absyn_Comprehension_e){ _LL2221:
_temp2220=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp2210)->f1; goto
_LL2219; _LL2219: _temp2218=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2210)->f2; goto _LL2217; _LL2217: _temp2216=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2210)->f3; goto _LL2213;} else{ goto _LL2214;} _LL2214: goto _LL2215;
_LL2213: vd->initializer= 0;( void*)( s->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
init_nv, Cyc_Absyn_var_exp( vd->name, 0), _temp2220, _temp2218, _temp2216, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r)); goto _LL2211; _LL2215: Cyc_Toc_exp_to_c( init_nv,
init); goto _LL2211; _LL2211:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt,
struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)((
struct Cyc_Core_Opt*) _check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct
_tuple0* x= Cyc_Toc_temp_var(); struct _tagged_arr* succ_lab= Cyc_Toc_fresh_label();
struct _tagged_arr* fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();{ struct
_RegionHandle _temp2222= _new_region(); struct _RegionHandle* prgn=& _temp2222;
_push_region( prgn);{ struct Cyc_Absyn_Stmt* _temp2225; struct Cyc_List_List*
_temp2227; struct Cyc_Toc_Env* _temp2229; struct _tuple11 _temp2223= Cyc_Toc_xlate_pat(
nv, prgn, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab,
fail_lab, 0); _LL2230: _temp2229= _temp2223.f1; goto _LL2228; _LL2228: _temp2227=
_temp2223.f2; goto _LL2226; _LL2226: _temp2225= _temp2223.f3; goto _LL2224;
_LL2224: Cyc_Toc_stmt_to_c( _temp2229, s);{ struct Cyc_Absyn_Stmt* _temp2231=
Cyc_Absyn_label_stmt( succ_lab, s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt(
x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2225, _temp2231, 0), 0);} else{ struct Cyc_Absyn_Exp* _temp2232= Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp( 0)); struct Cyc_Absyn_Stmt* _temp2233= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( _temp2232, 0), 0); s= Cyc_Absyn_declare_stmt( x,
Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2225,
Cyc_Absyn_seq_stmt( _temp2233, _temp2231, 0), 0), 0);} for( 0; _temp2227 != 0;
_temp2227=(( struct Cyc_List_List*) _check_null( _temp2227))->tl){ struct
_tuple12 _temp2236; void* _temp2237; struct _tuple0* _temp2239; struct _tuple12*
_temp2234=( struct _tuple12*)(( struct Cyc_List_List*) _check_null( _temp2227))->hd;
_temp2236=* _temp2234; _LL2240: _temp2239= _temp2236.f1; goto _LL2238; _LL2238:
_temp2237= _temp2236.f2; goto _LL2235; _LL2235: s= Cyc_Absyn_declare_stmt(
_temp2239, _temp2237, 0, s, 0);}}}; _pop_region( prgn);} return s;}} static
struct Cyc_Toc_Env* Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv, struct Cyc_List_List*
ds, int top){ for( 0; ds != 0; ds=(( struct Cyc_List_List*) _check_null( ds))->tl){
if( ! nv->toplevel){(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("decls_to_c: not at toplevel!",
sizeof( unsigned char), 29u));}{ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( ds))->hd; void* _temp2241=( void*) d->r;
struct Cyc_Absyn_Vardecl* _temp2267; struct Cyc_Absyn_Fndecl* _temp2269; struct
Cyc_Absyn_Structdecl* _temp2271; struct Cyc_Absyn_Uniondecl* _temp2273; struct
Cyc_Absyn_Tuniondecl* _temp2275; struct Cyc_Absyn_Enumdecl* _temp2277; struct
Cyc_Absyn_Typedefdecl* _temp2279; struct Cyc_List_List* _temp2281; struct Cyc_List_List*
_temp2283; struct Cyc_List_List* _temp2285; _LL2243: if(*(( int*) _temp2241) ==
Cyc_Absyn_Var_d){ _LL2268: _temp2267=(( struct Cyc_Absyn_Var_d_struct*)
_temp2241)->f1; goto _LL2244;} else{ goto _LL2245;} _LL2245: if(*(( int*)
_temp2241) == Cyc_Absyn_Fn_d){ _LL2270: _temp2269=(( struct Cyc_Absyn_Fn_d_struct*)
_temp2241)->f1; goto _LL2246;} else{ goto _LL2247;} _LL2247: if(*(( int*)
_temp2241) == Cyc_Absyn_Let_d){ goto _LL2248;} else{ goto _LL2249;} _LL2249: if(*((
int*) _temp2241) == Cyc_Absyn_Letv_d){ goto _LL2250;} else{ goto _LL2251;}
_LL2251: if(*(( int*) _temp2241) == Cyc_Absyn_Struct_d){ _LL2272: _temp2271=((
struct Cyc_Absyn_Struct_d_struct*) _temp2241)->f1; goto _LL2252;} else{ goto
_LL2253;} _LL2253: if(*(( int*) _temp2241) == Cyc_Absyn_Union_d){ _LL2274:
_temp2273=(( struct Cyc_Absyn_Union_d_struct*) _temp2241)->f1; goto _LL2254;}
else{ goto _LL2255;} _LL2255: if(*(( int*) _temp2241) == Cyc_Absyn_Tunion_d){
_LL2276: _temp2275=(( struct Cyc_Absyn_Tunion_d_struct*) _temp2241)->f1; goto
_LL2256;} else{ goto _LL2257;} _LL2257: if(*(( int*) _temp2241) == Cyc_Absyn_Enum_d){
_LL2278: _temp2277=(( struct Cyc_Absyn_Enum_d_struct*) _temp2241)->f1; goto
_LL2258;} else{ goto _LL2259;} _LL2259: if(*(( int*) _temp2241) == Cyc_Absyn_Typedef_d){
_LL2280: _temp2279=(( struct Cyc_Absyn_Typedef_d_struct*) _temp2241)->f1; goto
_LL2260;} else{ goto _LL2261;} _LL2261: if(*(( int*) _temp2241) == Cyc_Absyn_Namespace_d){
_LL2282: _temp2281=(( struct Cyc_Absyn_Namespace_d_struct*) _temp2241)->f2; goto
_LL2262;} else{ goto _LL2263;} _LL2263: if(*(( int*) _temp2241) == Cyc_Absyn_Using_d){
_LL2284: _temp2283=(( struct Cyc_Absyn_Using_d_struct*) _temp2241)->f2; goto
_LL2264;} else{ goto _LL2265;} _LL2265: if(*(( int*) _temp2241) == Cyc_Absyn_ExternC_d){
_LL2286: _temp2285=(( struct Cyc_Absyn_ExternC_d_struct*) _temp2241)->f1; goto
_LL2266;} else{ goto _LL2242;} _LL2244: { struct _tuple0* _temp2287= _temp2267->name;
if(( void*) _temp2267->sc ==( void*) Cyc_Absyn_ExternC){ _temp2287=({ struct
_tuple0* _temp2288=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2288->f1= Cyc_Absyn_rel_ns_null; _temp2288->f2=(* _temp2287).f2; _temp2288;});}
if( _temp2267->initializer != 0){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)
_check_null( _temp2267->initializer));} nv= Cyc_Toc_add_varmap( nv, _temp2267->name,
Cyc_Absyn_varb_exp( _temp2287,( void*)({ struct Cyc_Absyn_Global_b_struct*
_temp2289=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp2289[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp2290; _temp2290.tag= Cyc_Absyn_Global_b;
_temp2290.f1= _temp2267; _temp2290;}); _temp2289;}), 0)); _temp2267->name=
_temp2287;( void*)( _temp2267->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2267->sc));(
void*)( _temp2267->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2267->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2291=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2291->hd=( void*) d; _temp2291->tl=
Cyc_Toc_result_decls; _temp2291;}); goto _LL2242;} _LL2246: { struct _tuple0*
_temp2292= _temp2269->name; if(( void*) _temp2269->sc ==( void*) Cyc_Absyn_ExternC){
_temp2292=({ struct _tuple0* _temp2293=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp2293->f1= Cyc_Absyn_rel_ns_null; _temp2293->f2=(*
_temp2292).f2; _temp2293;});} nv= Cyc_Toc_add_varmap( nv, _temp2269->name, Cyc_Absyn_var_exp(
_temp2292, 0)); _temp2269->name= _temp2292; Cyc_Toc_fndecl_to_c( nv, _temp2269);
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2294=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2294->hd=( void*) d; _temp2294->tl=
Cyc_Toc_result_decls; _temp2294;}); goto _LL2242;} _LL2248: goto _LL2250;
_LL2250:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("letdecl at toplevel",
sizeof( unsigned char), 20u)); goto _LL2242; _LL2252: Cyc_Toc_structdecl_to_c(
_temp2271); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2295=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2295->hd=( void*) d; _temp2295->tl=
Cyc_Toc_result_decls; _temp2295;}); goto _LL2242; _LL2254: Cyc_Toc_uniondecl_to_c(
_temp2273); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2296=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2296->hd=( void*) d; _temp2296->tl=
Cyc_Toc_result_decls; _temp2296;}); goto _LL2242; _LL2256: if( _temp2275->is_xtunion){
Cyc_Toc_xtuniondecl_to_c( _temp2275);} else{ Cyc_Toc_tuniondecl_to_c( _temp2275);}
goto _LL2242; _LL2258: Cyc_Toc_enumdecl_to_c( nv, _temp2277); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2297=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2297->hd=( void*) d; _temp2297->tl= Cyc_Toc_result_decls;
_temp2297;}); goto _LL2242; _LL2260: _temp2279->name= _temp2279->name; _temp2279->tvs=
0;( void*)( _temp2279->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2279->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2298=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2298->hd=( void*) d; _temp2298->tl=
Cyc_Toc_result_decls; _temp2298;}); goto _LL2242; _LL2262: _temp2283= _temp2281;
goto _LL2264; _LL2264: _temp2285= _temp2283; goto _LL2266; _LL2266: nv= Cyc_Toc_decls_to_c(
nv, _temp2285, top); goto _LL2242; _LL2242:;}} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_temp_var_counter= 0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far=
0; Cyc_Toc_tunions_so_far= 0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List*
Cyc_Toc_toc( struct Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(),
ds, 1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}