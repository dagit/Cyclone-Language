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
struct _tagged_arr fmt, struct _tagged_arr); extern int Cyc_Stdio_vfprintf(
struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt, struct _tagged_arr ap);
static const int Cyc_Stdio_ShortPtr_sa= 0; struct Cyc_Stdio_ShortPtr_sa_struct{
int tag; short* f1; } ; static const int Cyc_Stdio_UShortPtr_sa= 1; struct Cyc_Stdio_UShortPtr_sa_struct{
int tag; unsigned short* f1; } ; static const int Cyc_Stdio_IntPtr_sa= 2; struct
Cyc_Stdio_IntPtr_sa_struct{ int tag; int* f1; } ; static const int Cyc_Stdio_UIntPtr_sa=
3; struct Cyc_Stdio_UIntPtr_sa_struct{ int tag; unsigned int* f1; } ; static
const int Cyc_Stdio_StringPtr_sa= 4; struct Cyc_Stdio_StringPtr_sa_struct{ int
tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_DoublePtr_sa= 5;
struct Cyc_Stdio_DoublePtr_sa_struct{ int tag; double* f1; } ; static const int
Cyc_Stdio_FloatPtr_sa= 6; struct Cyc_Stdio_FloatPtr_sa_struct{ int tag; float*
f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; extern
struct Cyc_List_List* Cyc_List_list( struct _tagged_arr); extern int Cyc_List_length(
struct Cyc_List_List* x); extern unsigned char Cyc_List_List_empty[ 15u]; extern
struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_rmap( struct _RegionHandle*, void*(* f)(
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
struct _tagged_arr fmt, struct _tagged_arr ap); extern void* Cyc_Tcutil_typ_kind(
void* t); extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_unify(
void*, void*); extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields); extern int Cyc_Tcutil_is_tagged_pointer_typ(
void*); extern int Cyc_Tcutil_is_tagged_pointer_typ_elt( void* t, void**
elt_typ_dest); struct _tuple3{ struct Cyc_Absyn_Tqual f1; void* f2; } ; extern
void* Cyc_Tcutil_snd_tqt( struct _tuple3*); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern struct Cyc_List_List* Cyc_Toc_toc( struct Cyc_List_List*
ds); extern struct _tuple0* Cyc_Toc_temp_var(); static struct Cyc_List_List* Cyc_Toc_result_decls=
0; static struct Cyc_List_List* Cyc_Toc_tuple_types= 0; static int Cyc_Toc_tuple_type_counter=
0; static int Cyc_Toc_temp_var_counter= 0; static int Cyc_Toc_fresh_label_counter=
0; unsigned char Cyc_Toc_Toc_Unimplemented[ 22u]="\000\000\000\000Toc_Unimplemented";
unsigned char Cyc_Toc_Toc_Impossible[ 19u]="\000\000\000\000Toc_Impossible";
static void* Cyc_Toc_unimp( struct _tagged_arr fmt, struct _tagged_arr ap){ Cyc_Stdio_vfprintf(
Cyc_Stdio_stderr, fmt, ap);({ int(* _temp0)( struct Cyc_Stdio___sFILE*, struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp3= Cyc_Stdio_stderr; struct _tagged_arr _temp4= _tag_arr("\n", sizeof(
unsigned char), 2u); void* _temp1[ 0u]={}; struct _tagged_arr _temp2={( void*)
_temp1,( void*) _temp1,( void*)( _temp1 + 0u)}; _temp0( _temp3, _temp4, _temp2);});
Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( int) _throw((
void*) Cyc_Toc_Toc_Unimplemented);} static void* Cyc_Toc_toc_impos( struct
_tagged_arr fmt, struct _tagged_arr ap){ Cyc_Stdio_vfprintf( Cyc_Stdio_stderr,
fmt, ap);({ int(* _temp5)( struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp8= Cyc_Stdio_stderr;
struct _tagged_arr _temp9= _tag_arr("\n", sizeof( unsigned char), 2u); void*
_temp6[ 0u]={}; struct _tagged_arr _temp7={( void*) _temp6,( void*) _temp6,(
void*)( _temp6 + 0u)}; _temp5( _temp8, _temp9, _temp7);}); Cyc_Stdio_fflush((
struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( int) _throw(( void*) Cyc_Toc_Toc_Impossible);}
unsigned char Cyc_Toc_Match_error[ 16u]="\000\000\000\000Match_error"; static
unsigned char _temp10[ 5u]="curr"; static struct _tagged_arr Cyc_Toc_curr_string={
_temp10, _temp10, _temp10 + 5u}; static struct _tagged_arr* Cyc_Toc_curr_sp=&
Cyc_Toc_curr_string; static unsigned char _temp11[ 4u]="tag"; static struct
_tagged_arr Cyc_Toc_tag_string={ _temp11, _temp11, _temp11 + 4u}; static struct
_tagged_arr* Cyc_Toc_tag_sp=& Cyc_Toc_tag_string; static unsigned char _temp12[
14u]="_handler_cons"; static struct _tagged_arr Cyc_Toc__handler_cons_string={
_temp12, _temp12, _temp12 + 14u}; static struct _tagged_arr* Cyc_Toc__handler_cons_sp=&
Cyc_Toc__handler_cons_string; static unsigned char _temp13[ 8u]="handler";
static struct _tagged_arr Cyc_Toc_handler_string={ _temp13, _temp13, _temp13 + 8u};
static struct _tagged_arr* Cyc_Toc_handler_sp=& Cyc_Toc_handler_string; static
unsigned char _temp14[ 14u]="_RegionHandle"; static struct _tagged_arr Cyc_Toc__RegionHandle_string={
_temp14, _temp14, _temp14 + 14u}; static struct _tagged_arr* Cyc_Toc__RegionHandle_sp=&
Cyc_Toc__RegionHandle_string; static unsigned char _temp15[ 7u]="_throw"; static
struct _tagged_arr Cyc_Toc__throw_str={ _temp15, _temp15, _temp15 + 7u}; static
struct _tuple0 Cyc_Toc__throw_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__throw_str};
static struct _tuple0* Cyc_Toc__throw_qv=& Cyc_Toc__throw_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__throw_re={ 1u,& Cyc_Toc__throw_pr,( void*)(( void*) 0u)}; static struct
Cyc_Absyn_Exp Cyc_Toc__throw_ev={ 0,( void*)(( void*)& Cyc_Toc__throw_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__throw_e=& Cyc_Toc__throw_ev; static
unsigned char _temp17[ 7u]="setjmp"; static struct _tagged_arr Cyc_Toc_setjmp_str={
_temp17, _temp17, _temp17 + 7u}; static struct _tuple0 Cyc_Toc_setjmp_pr={( void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc_setjmp_str}; static struct _tuple0* Cyc_Toc_setjmp_qv=&
Cyc_Toc_setjmp_pr; static struct Cyc_Absyn_Var_e_struct Cyc_Toc_setjmp_re={ 1u,&
Cyc_Toc_setjmp_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={
0,( void*)(( void*)& Cyc_Toc_setjmp_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc_setjmp_e=&
Cyc_Toc_setjmp_ev; static unsigned char _temp19[ 14u]="_push_handler"; static
struct _tagged_arr Cyc_Toc__push_handler_str={ _temp19, _temp19, _temp19 + 14u};
static struct _tuple0 Cyc_Toc__push_handler_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__push_handler_str}; static struct _tuple0* Cyc_Toc__push_handler_qv=&
Cyc_Toc__push_handler_pr; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_handler_re={
1u,& Cyc_Toc__push_handler_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__push_handler_ev={ 0,( void*)(( void*)& Cyc_Toc__push_handler_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;
static unsigned char _temp21[ 13u]="_pop_handler"; static struct _tagged_arr Cyc_Toc__pop_handler_str={
_temp21, _temp21, _temp21 + 13u}; static struct _tuple0 Cyc_Toc__pop_handler_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__pop_handler_str}; static struct
_tuple0* Cyc_Toc__pop_handler_qv=& Cyc_Toc__pop_handler_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__pop_handler_re={ 1u,& Cyc_Toc__pop_handler_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={ 0,( void*)(( void*)& Cyc_Toc__pop_handler_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;
static unsigned char _temp23[ 12u]="_exn_thrown"; static struct _tagged_arr Cyc_Toc__exn_thrown_str={
_temp23, _temp23, _temp23 + 12u}; static struct _tuple0 Cyc_Toc__exn_thrown_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__exn_thrown_str}; static struct
_tuple0* Cyc_Toc__exn_thrown_qv=& Cyc_Toc__exn_thrown_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__exn_thrown_re={ 1u,& Cyc_Toc__exn_thrown_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={ 0,( void*)(( void*)& Cyc_Toc__exn_thrown_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;
static unsigned char _temp25[ 14u]="_npop_handler"; static struct _tagged_arr
Cyc_Toc__npop_handler_str={ _temp25, _temp25, _temp25 + 14u}; static struct
_tuple0 Cyc_Toc__npop_handler_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__npop_handler_str};
static struct _tuple0* Cyc_Toc__npop_handler_qv=& Cyc_Toc__npop_handler_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__npop_handler_re={ 1u,& Cyc_Toc__npop_handler_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={ 0,(
void*)(( void*)& Cyc_Toc__npop_handler_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__npop_handler_e=&
Cyc_Toc__npop_handler_ev; static unsigned char _temp27[ 12u]="_check_null";
static struct _tagged_arr Cyc_Toc__check_null_str={ _temp27, _temp27, _temp27 +
12u}; static struct _tuple0 Cyc_Toc__check_null_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_null_str}; static struct _tuple0* Cyc_Toc__check_null_qv=& Cyc_Toc__check_null_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_null_re={ 1u,& Cyc_Toc__check_null_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={ 0,(
void*)(( void*)& Cyc_Toc__check_null_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__check_null_e=&
Cyc_Toc__check_null_ev; static unsigned char _temp29[ 28u]="_check_known_subscript_null";
static struct _tagged_arr Cyc_Toc__check_known_subscript_null_str={ _temp29,
_temp29, _temp29 + 28u}; static struct _tuple0 Cyc_Toc__check_known_subscript_null_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_known_subscript_null_str};
static struct _tuple0* Cyc_Toc__check_known_subscript_null_qv=& Cyc_Toc__check_known_subscript_null_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_null_re={ 1u,&
Cyc_Toc__check_known_subscript_null_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__check_known_subscript_null_ev={ 0,( void*)(( void*)& Cyc_Toc__check_known_subscript_null_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static unsigned char _temp31[ 31u]="_check_known_subscript_notnull"; static
struct _tagged_arr Cyc_Toc__check_known_subscript_notnull_str={ _temp31, _temp31,
_temp31 + 31u}; static struct _tuple0 Cyc_Toc__check_known_subscript_notnull_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_known_subscript_notnull_str};
static struct _tuple0* Cyc_Toc__check_known_subscript_notnull_qv=& Cyc_Toc__check_known_subscript_notnull_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_notnull_re={
1u,& Cyc_Toc__check_known_subscript_notnull_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={ 0,( void*)((
void*)& Cyc_Toc__check_known_subscript_notnull_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;
static unsigned char _temp33[ 25u]="_check_unknown_subscript"; static struct
_tagged_arr Cyc_Toc__check_unknown_subscript_str={ _temp33, _temp33, _temp33 +
25u}; static struct _tuple0 Cyc_Toc__check_unknown_subscript_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_unknown_subscript_str}; static struct _tuple0* Cyc_Toc__check_unknown_subscript_qv=&
Cyc_Toc__check_unknown_subscript_pr; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_unknown_subscript_re={
1u,& Cyc_Toc__check_unknown_subscript_pr,( void*)(( void*) 0u)}; static struct
Cyc_Absyn_Exp Cyc_Toc__check_unknown_subscript_ev={ 0,( void*)(( void*)& Cyc_Toc__check_unknown_subscript_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__check_unknown_subscript_e=& Cyc_Toc__check_unknown_subscript_ev;
static unsigned char _temp35[ 12u]="_tagged_arr"; static struct _tagged_arr Cyc_Toc__tagged_arr_str={
_temp35, _temp35, _temp35 + 12u}; static struct _tuple0 Cyc_Toc__tagged_arr_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_str}; static struct
_tuple0* Cyc_Toc__tagged_arr_qv=& Cyc_Toc__tagged_arr_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__tagged_arr_re={ 1u,& Cyc_Toc__tagged_arr_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_ev={ 0,( void*)(( void*)& Cyc_Toc__tagged_arr_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tagged_arr_e=& Cyc_Toc__tagged_arr_ev;
static unsigned char _temp37[ 9u]="_tag_arr"; static struct _tagged_arr Cyc_Toc__tag_arr_str={
_temp37, _temp37, _temp37 + 9u}; static struct _tuple0 Cyc_Toc__tag_arr_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tag_arr_str}; static struct
_tuple0* Cyc_Toc__tag_arr_qv=& Cyc_Toc__tag_arr_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__tag_arr_re={ 1u,& Cyc_Toc__tag_arr_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc__tag_arr_ev={ 0,( void*)(( void*)& Cyc_Toc__tag_arr_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tag_arr_e=& Cyc_Toc__tag_arr_ev;
static unsigned char _temp39[ 14u]="_init_tag_arr"; static struct _tagged_arr
Cyc_Toc__init_tag_arr_str={ _temp39, _temp39, _temp39 + 14u}; static struct
_tuple0 Cyc_Toc__init_tag_arr_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__init_tag_arr_str};
static struct _tuple0* Cyc_Toc__init_tag_arr_qv=& Cyc_Toc__init_tag_arr_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__init_tag_arr_re={ 1u,& Cyc_Toc__init_tag_arr_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__init_tag_arr_ev={ 0,(
void*)(( void*)& Cyc_Toc__init_tag_arr_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__init_tag_arr_e=&
Cyc_Toc__init_tag_arr_ev; static unsigned char _temp41[ 11u]="_untag_arr";
static struct _tagged_arr Cyc_Toc__untag_arr_str={ _temp41, _temp41, _temp41 +
11u}; static struct _tuple0 Cyc_Toc__untag_arr_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__untag_arr_str}; static struct _tuple0* Cyc_Toc__untag_arr_qv=& Cyc_Toc__untag_arr_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_arr_re={ 1u,& Cyc_Toc__untag_arr_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__untag_arr_ev={ 0,(
void*)(( void*)& Cyc_Toc__untag_arr_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__untag_arr_e=&
Cyc_Toc__untag_arr_ev; static unsigned char _temp43[ 14u]="_get_arr_size";
static struct _tagged_arr Cyc_Toc__get_arr_size_str={ _temp43, _temp43, _temp43
+ 14u}; static struct _tuple0 Cyc_Toc__get_arr_size_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__get_arr_size_str}; static struct _tuple0* Cyc_Toc__get_arr_size_qv=&
Cyc_Toc__get_arr_size_pr; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_arr_size_re={
1u,& Cyc_Toc__get_arr_size_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__get_arr_size_ev={ 0,( void*)(( void*)& Cyc_Toc__get_arr_size_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__get_arr_size_e=& Cyc_Toc__get_arr_size_ev;
static unsigned char _temp45[ 17u]="_tagged_arr_plus"; static struct _tagged_arr
Cyc_Toc__tagged_arr_plus_str={ _temp45, _temp45, _temp45 + 17u}; static struct
_tuple0 Cyc_Toc__tagged_arr_plus_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_plus_str};
static struct _tuple0* Cyc_Toc__tagged_arr_plus_qv=& Cyc_Toc__tagged_arr_plus_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_plus_re={ 1u,& Cyc_Toc__tagged_arr_plus_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_plus_ev={
0,( void*)(( void*)& Cyc_Toc__tagged_arr_plus_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__tagged_arr_plus_e=& Cyc_Toc__tagged_arr_plus_ev; static unsigned char
_temp47[ 25u]="_tagged_arr_inplace_plus"; static struct _tagged_arr Cyc_Toc__tagged_arr_inplace_plus_str={
_temp47, _temp47, _temp47 + 25u}; static struct _tuple0 Cyc_Toc__tagged_arr_inplace_plus_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_inplace_plus_str};
static struct _tuple0* Cyc_Toc__tagged_arr_inplace_plus_qv=& Cyc_Toc__tagged_arr_inplace_plus_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_re={ 1u,&
Cyc_Toc__tagged_arr_inplace_plus_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__tagged_arr_inplace_plus_ev={ 0,( void*)(( void*)& Cyc_Toc__tagged_arr_inplace_plus_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tagged_arr_inplace_plus_e=& Cyc_Toc__tagged_arr_inplace_plus_ev;
static unsigned char _temp49[ 30u]="_tagged_arr_inplace_plus_post"; static
struct _tagged_arr Cyc_Toc__tagged_arr_inplace_plus_post_str={ _temp49, _temp49,
_temp49 + 30u}; static struct _tuple0 Cyc_Toc__tagged_arr_inplace_plus_post_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_inplace_plus_post_str};
static struct _tuple0* Cyc_Toc__tagged_arr_inplace_plus_post_qv=& Cyc_Toc__tagged_arr_inplace_plus_post_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_post_re={
1u,& Cyc_Toc__tagged_arr_inplace_plus_post_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_inplace_plus_post_ev={ 0,( void*)((
void*)& Cyc_Toc__tagged_arr_inplace_plus_post_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__tagged_arr_inplace_plus_post_e=& Cyc_Toc__tagged_arr_inplace_plus_post_ev;
static unsigned char _temp51[ 10u]="GC_malloc"; static struct _tagged_arr Cyc_Toc_GC_malloc_str={
_temp51, _temp51, _temp51 + 10u}; static struct _tuple0 Cyc_Toc_GC_malloc_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc_GC_malloc_str}; static struct
_tuple0* Cyc_Toc_GC_malloc_qv=& Cyc_Toc_GC_malloc_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc_GC_malloc_re={ 1u,& Cyc_Toc_GC_malloc_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc_GC_malloc_ev={ 0,( void*)(( void*)& Cyc_Toc_GC_malloc_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc_GC_malloc_e=& Cyc_Toc_GC_malloc_ev;
static unsigned char _temp53[ 17u]="GC_malloc_atomic"; static struct _tagged_arr
Cyc_Toc_GC_malloc_atomic_str={ _temp53, _temp53, _temp53 + 17u}; static struct
_tuple0 Cyc_Toc_GC_malloc_atomic_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc_GC_malloc_atomic_str};
static struct _tuple0* Cyc_Toc_GC_malloc_atomic_qv=& Cyc_Toc_GC_malloc_atomic_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc_GC_malloc_atomic_re={ 1u,& Cyc_Toc_GC_malloc_atomic_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc_GC_malloc_atomic_ev={
0,( void*)(( void*)& Cyc_Toc_GC_malloc_atomic_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc_GC_malloc_atomic_e=& Cyc_Toc_GC_malloc_atomic_ev; static unsigned char
_temp55[ 15u]="_region_malloc"; static struct _tagged_arr Cyc_Toc__region_malloc_str={
_temp55, _temp55, _temp55 + 15u}; static struct _tuple0 Cyc_Toc__region_malloc_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__region_malloc_str}; static struct
_tuple0* Cyc_Toc__region_malloc_qv=& Cyc_Toc__region_malloc_pr; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__region_malloc_re={ 1u,& Cyc_Toc__region_malloc_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={ 0,(
void*)(( void*)& Cyc_Toc__region_malloc_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev; static unsigned char
_temp57[ 12u]="_new_region"; static struct _tagged_arr Cyc_Toc__new_region_str={
_temp57, _temp57, _temp57 + 12u}; static struct _tuple0 Cyc_Toc__new_region_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__new_region_str}; static struct
_tuple0* Cyc_Toc__new_region_qv=& Cyc_Toc__new_region_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__new_region_re={ 1u,& Cyc_Toc__new_region_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={ 0,( void*)(( void*)& Cyc_Toc__new_region_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;
static unsigned char _temp59[ 13u]="_push_region"; static struct _tagged_arr Cyc_Toc__push_region_str={
_temp59, _temp59, _temp59 + 13u}; static struct _tuple0 Cyc_Toc__push_region_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__push_region_str}; static struct
_tuple0* Cyc_Toc__push_region_qv=& Cyc_Toc__push_region_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_region_re={ 1u,& Cyc_Toc__push_region_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={ 0,( void*)(( void*)& Cyc_Toc__push_region_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;
static unsigned char _temp61[ 12u]="_pop_region"; static struct _tagged_arr Cyc_Toc__pop_region_str={
_temp61, _temp61, _temp61 + 12u}; static struct _tuple0 Cyc_Toc__pop_region_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__pop_region_str}; static struct
_tuple0* Cyc_Toc__pop_region_qv=& Cyc_Toc__pop_region_pr; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__pop_region_re={ 1u,& Cyc_Toc__pop_region_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={ 0,( void*)(( void*)& Cyc_Toc__pop_region_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;
static unsigned char _temp63[ 15u]="_sfile_to_file"; static struct _tagged_arr
Cyc_Toc__sfile_to_file_str={ _temp63, _temp63, _temp63 + 15u}; static struct
_tuple0 Cyc_Toc__sfile_to_file_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__sfile_to_file_str};
static struct _tuple0* Cyc_Toc__sfile_to_file_qv=& Cyc_Toc__sfile_to_file_pr;
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__sfile_to_file_re={ 1u,& Cyc_Toc__sfile_to_file_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__sfile_to_file_ev={ 0,(
void*)(( void*)& Cyc_Toc__sfile_to_file_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__sfile_to_file_e=& Cyc_Toc__sfile_to_file_ev; static struct Cyc_Absyn_StructType_struct
Cyc_Toc_tagged_arr_typ_v={ 9u,( struct _tuple0*)& Cyc_Toc__tagged_arr_pr, 0, 0};
static void* Cyc_Toc_tagged_arr_typ=( void*)& Cyc_Toc_tagged_arr_typ_v; static
struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={ 0, 0, 0}; static unsigned char _temp66[ 3u]="f0";
static struct _tagged_arr Cyc_Toc_f0={ _temp66, _temp66, _temp66 + 3u}; static
struct _tagged_arr* Cyc_Toc_field_names_v[ 1u]={& Cyc_Toc_f0}; static struct
_tagged_arr Cyc_Toc_field_names={( void*)(( struct _tagged_arr**) Cyc_Toc_field_names_v),(
void*)(( struct _tagged_arr**) Cyc_Toc_field_names_v),( void*)(( struct
_tagged_arr**) Cyc_Toc_field_names_v) + 1u}; static struct _tagged_arr* Cyc_Toc_fieldname(
int i){ unsigned int fsz= _get_arr_size( Cyc_Toc_field_names, sizeof( struct
_tagged_arr*)); if( i >= fsz){ Cyc_Toc_field_names=({ unsigned int _temp67=(
unsigned int)( i + 1); struct _tagged_arr** _temp68=( struct _tagged_arr**)
GC_malloc( sizeof( struct _tagged_arr*) * _temp67); struct _tagged_arr _temp77=
_tag_arr( _temp68, sizeof( struct _tagged_arr*),( unsigned int)( i + 1));{
unsigned int _temp69= _temp67; unsigned int j; for( j= 0; j < _temp69; j ++){
_temp68[ j]= j < fsz?*(( struct _tagged_arr**) _check_unknown_subscript( Cyc_Toc_field_names,
sizeof( struct _tagged_arr*),( int) j)):({ struct _tagged_arr* _temp70=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp70[ 0]=( struct
_tagged_arr)({ struct _tagged_arr(* _temp71)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp75= _tag_arr("f%d",
sizeof( unsigned char), 4u); struct Cyc_Stdio_Int_pa_struct _temp76; _temp76.tag=
Cyc_Stdio_Int_pa; _temp76.f1=( int) j;{ void* _temp74=( void*)& _temp76; void*
_temp72[ 1u]={ _temp74}; struct _tagged_arr _temp73={( void*) _temp72,( void*)
_temp72,( void*)( _temp72 + 1u)}; _temp71( _temp75, _temp73);}}); _temp70;});}};
_temp77;});} return*(( struct _tagged_arr**) _check_unknown_subscript( Cyc_Toc_field_names,
sizeof( struct _tagged_arr*), i));} struct _tuple4{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; static int Cyc_Toc_is_zero( struct Cyc_Absyn_Exp*
e){ void* _temp78=( void*) e->r; void* _temp104; unsigned char _temp106; void*
_temp108; short _temp110; void* _temp112; int _temp114; void* _temp116;
long long _temp118; void* _temp120; struct Cyc_Absyn_Exp* _temp122; struct Cyc_List_List*
_temp124; struct Cyc_List_List* _temp126; struct Cyc_List_List* _temp128; struct
Cyc_List_List* _temp130; struct Cyc_List_List* _temp132; _LL80: if(*(( int*)
_temp78) == Cyc_Absyn_Const_e){ _LL105: _temp104=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp78)->f1; if(( unsigned int) _temp104 > 1u?*(( int*) _temp104) == Cyc_Absyn_Char_c:
0){ _LL107: _temp106=(( struct Cyc_Absyn_Char_c_struct*) _temp104)->f2; goto
_LL81;} else{ goto _LL82;}} else{ goto _LL82;} _LL82: if(*(( int*) _temp78) ==
Cyc_Absyn_Const_e){ _LL109: _temp108=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp78)->f1; if(( unsigned int) _temp108 > 1u?*(( int*) _temp108) == Cyc_Absyn_Short_c:
0){ _LL111: _temp110=(( struct Cyc_Absyn_Short_c_struct*) _temp108)->f2; goto
_LL83;} else{ goto _LL84;}} else{ goto _LL84;} _LL84: if(*(( int*) _temp78) ==
Cyc_Absyn_Const_e){ _LL113: _temp112=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp78)->f1; if(( unsigned int) _temp112 > 1u?*(( int*) _temp112) == Cyc_Absyn_Int_c:
0){ _LL115: _temp114=(( struct Cyc_Absyn_Int_c_struct*) _temp112)->f2; goto
_LL85;} else{ goto _LL86;}} else{ goto _LL86;} _LL86: if(*(( int*) _temp78) ==
Cyc_Absyn_Const_e){ _LL117: _temp116=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp78)->f1; if(( unsigned int) _temp116 > 1u?*(( int*) _temp116) == Cyc_Absyn_LongLong_c:
0){ _LL119: _temp118=(( struct Cyc_Absyn_LongLong_c_struct*) _temp116)->f2; goto
_LL87;} else{ goto _LL88;}} else{ goto _LL88;} _LL88: if(*(( int*) _temp78) ==
Cyc_Absyn_Const_e){ _LL121: _temp120=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp78)->f1; if( _temp120 ==( void*) Cyc_Absyn_Null_c){ goto _LL89;} else{ goto
_LL90;}} else{ goto _LL90;} _LL90: if(*(( int*) _temp78) == Cyc_Absyn_Cast_e){
_LL123: _temp122=(( struct Cyc_Absyn_Cast_e_struct*) _temp78)->f2; goto _LL91;}
else{ goto _LL92;} _LL92: if(*(( int*) _temp78) == Cyc_Absyn_Tuple_e){ _LL125:
_temp124=(( struct Cyc_Absyn_Tuple_e_struct*) _temp78)->f1; goto _LL93;} else{
goto _LL94;} _LL94: if(*(( int*) _temp78) == Cyc_Absyn_Array_e){ _LL127:
_temp126=(( struct Cyc_Absyn_Array_e_struct*) _temp78)->f1; goto _LL95;} else{
goto _LL96;} _LL96: if(*(( int*) _temp78) == Cyc_Absyn_Struct_e){ _LL129:
_temp128=(( struct Cyc_Absyn_Struct_e_struct*) _temp78)->f3; goto _LL97;} else{
goto _LL98;} _LL98: if(*(( int*) _temp78) == Cyc_Absyn_CompoundLit_e){ _LL131:
_temp130=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp78)->f2; goto _LL99;}
else{ goto _LL100;} _LL100: if(*(( int*) _temp78) == Cyc_Absyn_UnresolvedMem_e){
_LL133: _temp132=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp78)->f2; goto
_LL101;} else{ goto _LL102;} _LL102: goto _LL103; _LL81: return _temp106 =='\000';
_LL83: return _temp110 == 0; _LL85: return _temp114 == 0; _LL87: return _temp118
== 0; _LL89: return 1; _LL91: return Cyc_Toc_is_zero( _temp122); _LL93: return((
int(*)( int(* pred)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_forall)(
Cyc_Toc_is_zero, _temp124); _LL95: _temp128= _temp126; goto _LL97; _LL97:
_temp130= _temp128; goto _LL99; _LL99: _temp132= _temp130; goto _LL101; _LL101:
for( 0; _temp132 != 0; _temp132=(( struct Cyc_List_List*) _check_null( _temp132))->tl){
if( ! Cyc_Toc_is_zero((*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp132))->hd)).f2)){ return 0;}} return 1; _LL103: return 0; _LL79:;} static
int Cyc_Toc_is_nullable( void* t){ void* _temp134= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp140; struct Cyc_Absyn_Conref* _temp142; _LL136:
if(( unsigned int) _temp134 > 4u?*(( int*) _temp134) == Cyc_Absyn_PointerType: 0){
_LL141: _temp140=(( struct Cyc_Absyn_PointerType_struct*) _temp134)->f1; _LL143:
_temp142= _temp140.nullable; goto _LL137;} else{ goto _LL138;} _LL138: goto
_LL139; _LL137: return(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
0, _temp142); _LL139:({ int(* _temp144)( struct _tagged_arr fmt, struct
_tagged_arr ap)=( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos;
struct _tagged_arr _temp147= _tag_arr("is_nullable", sizeof( unsigned char), 12u);
void* _temp145[ 0u]={}; struct _tagged_arr _temp146={( void*) _temp145,( void*)
_temp145,( void*)( _temp145 + 0u)}; _temp144( _temp147, _temp146);}); return 0;
_LL135:;} static struct _tuple0* Cyc_Toc_collapse_qvar_tag( struct _tuple0* x,
struct _tagged_arr tag){ return({ struct _tuple0* _temp148=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp148->f1=(* x).f1; _temp148->f2=({
struct _tagged_arr* _temp149=( struct _tagged_arr*) GC_malloc( sizeof( struct
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
_LL245:( int) _throw(({ void*(* _temp351)( struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_impos; struct _tagged_arr _temp354= _tag_arr("unresolved TunionType",
sizeof( unsigned char), 22u); void* _temp352[ 0u]={}; struct _tagged_arr
_temp353={( void*) _temp352,( void*) _temp352,( void*)( _temp352 + 0u)};
_temp351( _temp354, _temp353);})); _LL247: if( _temp301->typs == 0){ if(
_temp303->is_xtunion){ return Cyc_Absyn_star_typ( Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn,
Cyc_Toc_mt_tq);} else{ return Cyc_Absyn_uint_t;}} else{ return Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag( _temp301->name, _tag_arr("_struct", sizeof(
unsigned char), 8u)));} _LL249:( int) _throw(({ void*(* _temp355)( struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_impos; struct _tagged_arr
_temp358= _tag_arr("unresolved TunionFieldType", sizeof( unsigned char), 27u);
void* _temp356[ 0u]={}; struct _tagged_arr _temp357={( void*) _temp356,( void*)
_temp356,( void*)( _temp356 + 0u)}; _temp355( _temp358, _temp357);})); _LL251:
_temp311= Cyc_Toc_typ_to_c_array( _temp311);{ void* _temp359=( void*)( Cyc_Absyn_compress_conref(
_temp307))->v; void* _temp365; _LL361: if(( unsigned int) _temp359 > 1u?*(( int*)
_temp359) == Cyc_Absyn_Eq_constr: 0){ _LL366: _temp365=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp359)->f1; if( _temp365 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL362;} else{
goto _LL363;}} else{ goto _LL363;} _LL363: goto _LL364; _LL362: return Cyc_Toc_tagged_arr_typ;
_LL364: return Cyc_Absyn_star_typ( _temp311,( void*) Cyc_Absyn_HeapRgn, _temp309);
_LL360:;} _LL253: return t; _LL255: return t; _LL257: return t; _LL259: return(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp367=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp367[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp368; _temp368.tag= Cyc_Absyn_ArrayType; _temp368.f1=(
void*) Cyc_Toc_typ_to_c_array( _temp317); _temp368.f2= _temp315; _temp368.f3=
_temp313; _temp368;}); _temp367;}); _LL261: { struct Cyc_List_List* _temp369= 0;
for( 0; _temp321 != 0; _temp321=(( struct Cyc_List_List*) _check_null( _temp321))->tl){
void* _temp370=( void*)(( struct Cyc_List_List*) _check_null( _temp321))->hd;
_LL372: if( _temp370 ==( void*) Cyc_Absyn_Noreturn_att){ goto _LL373;} else{
goto _LL374;} _LL374: if( _temp370 ==( void*) Cyc_Absyn_Const_att){ goto _LL375;}
else{ goto _LL376;} _LL376: if(( unsigned int) _temp370 > 16u?*(( int*) _temp370)
== Cyc_Absyn_Format_att: 0){ goto _LL377;} else{ goto _LL378;} _LL378: goto
_LL379; _LL373: goto _LL375; _LL375: goto _LL377; _LL377: continue; _LL379:
_temp369=({ struct Cyc_List_List* _temp380=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp380->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( _temp321))->hd); _temp380->tl= _temp369; _temp380;}); goto _LL371;
_LL371:;}{ struct Cyc_List_List* _temp381=(( struct Cyc_List_List*(*)( struct
_tuple1*(* f)( struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_arg_to_c,
_temp327); if( _temp323 != 0){ void* _temp382= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ((
void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp323))->type,( void*)((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp323))->rgn, Cyc_Toc_mt_tq));
struct _tuple1* _temp383=({ struct _tuple1* _temp385=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp385->f1=(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp323))->name; _temp385->f2=(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp323))->tq; _temp385->f3= _temp382; _temp385;}); _temp381=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp381,({ struct Cyc_List_List* _temp384=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp384->hd=( void*) _temp383; _temp384->tl= 0;
_temp384;}));} return( void*)({ struct Cyc_Absyn_FnType_struct* _temp386=(
struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp386[ 0]=({ struct Cyc_Absyn_FnType_struct _temp387; _temp387.tag= Cyc_Absyn_FnType;
_temp387.f1=({ struct Cyc_Absyn_FnInfo _temp388; _temp388.tvars= 0; _temp388.effect=
0; _temp388.ret_typ=( void*) Cyc_Toc_typ_to_c( _temp329); _temp388.args=
_temp381; _temp388.c_varargs= _temp325; _temp388.cyc_varargs= 0; _temp388.rgn_po=
0; _temp388.attributes= _temp369; _temp388;}); _temp387;}); _temp386;});}}
_LL263: _temp331=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct
_tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f, _temp331);{
struct _tagged_arr* _temp389= Cyc_Toc_add_tuple_type( _temp331); return Cyc_Absyn_strct(
_temp389);} _LL265: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp390=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp390[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp391; _temp391.tag= Cyc_Absyn_AnonStructType; _temp391.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_structfield_to_c, _temp333); _temp391;}); _temp390;});
_LL267: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp392=(
struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp392[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp393; _temp393.tag=
Cyc_Absyn_AnonUnionType; _temp393.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp335); _temp393;}); _temp392;}); _LL269: if( _temp337 == 0){({ int(*
_temp394)( struct _tagged_arr fmt, struct _tagged_arr ap)=( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp; struct _tagged_arr
_temp397= _tag_arr("anonymous union", sizeof( unsigned char), 16u); void*
_temp395[ 0u]={}; struct _tagged_arr _temp396={( void*) _temp395,( void*)
_temp395,( void*)( _temp395 + 0u)}; _temp394( _temp397, _temp396);});} return
Cyc_Absyn_unionq_typ(( struct _tuple0*) _check_null( _temp337)); _LL271: if(
_temp339 == 0){({ int(* _temp398)( struct _tagged_arr fmt, struct _tagged_arr ap)=(
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp; struct
_tagged_arr _temp401= _tag_arr("anonymous struct", sizeof( unsigned char), 17u);
void* _temp399[ 0u]={}; struct _tagged_arr _temp400={( void*) _temp399,( void*)
_temp399,( void*)( _temp399 + 0u)}; _temp398( _temp401, _temp400);});} return
Cyc_Absyn_strctq(( struct _tuple0*) _check_null( _temp339)); _LL273: return t;
_LL275: if( _temp343 == 0){ return({ void*(* _temp402)( struct _tagged_arr fmt,
struct _tagged_arr ap)= Cyc_Toc_toc_impos; struct _tagged_arr _temp405= _tag_arr("Toc::typ_to_c: unresolved TypedefType",
sizeof( unsigned char), 38u); void* _temp403[ 0u]={}; struct _tagged_arr
_temp404={( void*) _temp403,( void*) _temp403,( void*)( _temp403 + 0u)};
_temp402( _temp405, _temp404);});} return( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp406=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp406[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp407; _temp407.tag= Cyc_Absyn_TypedefType;
_temp407.f1= _temp347; _temp407.f2= 0; _temp407.f3=({ struct Cyc_Core_Opt*
_temp408=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp408->v=( void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp343))->v); _temp408;}); _temp407;}); _temp406;}); _LL277:
return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp), Cyc_Toc_mt_tq);
_LL279: return({ void*(* _temp409)( struct _tagged_arr fmt, struct _tagged_arr
ap)= Cyc_Toc_toc_impos; struct _tagged_arr _temp412= _tag_arr("Toc::typ_to_c: type translation passed the heap region",
sizeof( unsigned char), 55u); void* _temp410[ 0u]={}; struct _tagged_arr
_temp411={( void*) _temp410,( void*) _temp410,( void*)( _temp410 + 0u)};
_temp409( _temp412, _temp411);}); _LL281: return({ void*(* _temp413)( struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Toc_toc_impos; struct _tagged_arr
_temp416= _tag_arr("Toc::typ_to_c: type translation passed the access eff",
sizeof( unsigned char), 54u); void* _temp414[ 0u]={}; struct _tagged_arr
_temp415={( void*) _temp414,( void*) _temp414,( void*)( _temp414 + 0u)};
_temp413( _temp416, _temp415);}); _LL283: return({ void*(* _temp417)( struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Toc_toc_impos; struct _tagged_arr
_temp420= _tag_arr("Toc::typ_to_c: type translation passed the join eff",
sizeof( unsigned char), 52u); void* _temp418[ 0u]={}; struct _tagged_arr
_temp419={( void*) _temp418,( void*) _temp418,( void*)( _temp418 + 0u)};
_temp417( _temp420, _temp419);}); _LL233:;} static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast(
void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* l){ void*
_temp421= t; struct Cyc_Absyn_Tqual _temp427; void* _temp429; _LL423: if((
unsigned int) _temp421 > 4u?*(( int*) _temp421) == Cyc_Absyn_ArrayType: 0){
_LL430: _temp429=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp421)->f1;
goto _LL428; _LL428: _temp427=(( struct Cyc_Absyn_ArrayType_struct*) _temp421)->f2;
goto _LL424;} else{ goto _LL425;} _LL425: goto _LL426; _LL424: return Cyc_Absyn_cast_exp(
Cyc_Absyn_star_typ( _temp429,( void*) Cyc_Absyn_HeapRgn, _temp427), e, l);
_LL426: return Cyc_Absyn_cast_exp( t, e, l); _LL422:;} static int Cyc_Toc_atomic_typ(
void* t){ void* _temp431= Cyc_Tcutil_compress( t); void* _temp467; struct Cyc_Absyn_Structdecl**
_temp469; struct Cyc_List_List* _temp471; struct Cyc_List_List* _temp473; struct
Cyc_Absyn_TunionFieldInfo _temp475; void* _temp477; struct Cyc_Absyn_Tunionfield*
_temp479; struct Cyc_Absyn_Tuniondecl* _temp481; struct Cyc_List_List* _temp483;
_LL433: if( _temp431 ==( void*) Cyc_Absyn_VoidType){ goto _LL434;} else{ goto
_LL435;} _LL435: if(( unsigned int) _temp431 > 4u?*(( int*) _temp431) == Cyc_Absyn_VarType:
0){ goto _LL436;} else{ goto _LL437;} _LL437: if(( unsigned int) _temp431 > 4u?*((
int*) _temp431) == Cyc_Absyn_IntType: 0){ goto _LL438;} else{ goto _LL439;}
_LL439: if( _temp431 ==( void*) Cyc_Absyn_FloatType){ goto _LL440;} else{ goto
_LL441;} _LL441: if( _temp431 ==( void*) Cyc_Absyn_DoubleType){ goto _LL442;}
else{ goto _LL443;} _LL443: if(( unsigned int) _temp431 > 4u?*(( int*) _temp431)
== Cyc_Absyn_FnType: 0){ goto _LL444;} else{ goto _LL445;} _LL445: if((
unsigned int) _temp431 > 4u?*(( int*) _temp431) == Cyc_Absyn_ArrayType: 0){
_LL468: _temp467=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp431)->f1;
goto _LL446;} else{ goto _LL447;} _LL447: if(( unsigned int) _temp431 > 4u?*((
int*) _temp431) == Cyc_Absyn_StructType: 0){ _LL470: _temp469=(( struct Cyc_Absyn_StructType_struct*)
_temp431)->f3; goto _LL448;} else{ goto _LL449;} _LL449: if(( unsigned int)
_temp431 > 4u?*(( int*) _temp431) == Cyc_Absyn_AnonStructType: 0){ _LL472:
_temp471=(( struct Cyc_Absyn_AnonStructType_struct*) _temp431)->f1; goto _LL450;}
else{ goto _LL451;} _LL451: if(( unsigned int) _temp431 > 4u?*(( int*) _temp431)
== Cyc_Absyn_AnonUnionType: 0){ _LL474: _temp473=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp431)->f1; goto _LL452;} else{ goto _LL453;} _LL453: if(( unsigned int)
_temp431 > 4u?*(( int*) _temp431) == Cyc_Absyn_TunionFieldType: 0){ _LL476:
_temp475=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp431)->f1; _LL478:
_temp477=( void*) _temp475.field_info; if(*(( int*) _temp477) == Cyc_Absyn_KnownTunionfield){
_LL482: _temp481=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp477)->f1;
goto _LL480; _LL480: _temp479=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp477)->f2; goto _LL454;} else{ goto _LL455;}} else{ goto _LL455;} _LL455:
if(( unsigned int) _temp431 > 4u?*(( int*) _temp431) == Cyc_Absyn_TupleType: 0){
_LL484: _temp483=(( struct Cyc_Absyn_TupleType_struct*) _temp431)->f1; goto
_LL456;} else{ goto _LL457;} _LL457: if(( unsigned int) _temp431 > 4u?*(( int*)
_temp431) == Cyc_Absyn_TunionType: 0){ goto _LL458;} else{ goto _LL459;} _LL459:
if(( unsigned int) _temp431 > 4u?*(( int*) _temp431) == Cyc_Absyn_UnionType: 0){
goto _LL460;} else{ goto _LL461;} _LL461: if(( unsigned int) _temp431 > 4u?*((
int*) _temp431) == Cyc_Absyn_PointerType: 0){ goto _LL462;} else{ goto _LL463;}
_LL463: if(( unsigned int) _temp431 > 4u?*(( int*) _temp431) == Cyc_Absyn_RgnHandleType:
0){ goto _LL464;} else{ goto _LL465;} _LL465: goto _LL466; _LL434: return 1;
_LL436: return 0; _LL438: return 1; _LL440: return 1; _LL442: return 1; _LL444:
return 1; _LL446: return Cyc_Toc_atomic_typ( _temp467); _LL448: if( _temp469 ==
0? 1:(*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp469)))->fields == 0){
return 0;}{ struct Cyc_List_List* _temp485=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp469)))->fields))->v;
for( 0; _temp485 != 0; _temp485=(( struct Cyc_List_List*) _check_null( _temp485))->tl){
if( ! Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp485))->hd)->type)){ return 0;}}} return 1; _LL450: _temp473=
_temp471; goto _LL452; _LL452: for( 0; _temp473 != 0; _temp473=(( struct Cyc_List_List*)
_check_null( _temp473))->tl){ if( ! Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp473))->hd)->type)){ return 0;}} return
1; _LL454: _temp483= _temp479->typs; goto _LL456; _LL456: for( 0; _temp483 != 0;
_temp483=(( struct Cyc_List_List*) _check_null( _temp483))->tl){ if( ! Cyc_Toc_atomic_typ((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp483))->hd)).f2)){
return 0;}} return 1; _LL458: return 0; _LL460: return 0; _LL462: return 0;
_LL464: return 0; _LL466:({ int(* _temp486)( struct _tagged_arr fmt, struct
_tagged_arr ap)=( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos;
struct _tagged_arr _temp490= _tag_arr("atomic_typ:  bad type %s", sizeof(
unsigned char), 25u); struct Cyc_Stdio_String_pa_struct _temp491; _temp491.tag=
Cyc_Stdio_String_pa; _temp491.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp489=( void*)& _temp491; void* _temp487[ 1u]={ _temp489}; struct
_tagged_arr _temp488={( void*) _temp487,( void*) _temp487,( void*)( _temp487 + 1u)};
_temp486( _temp490, _temp488);}}); return 0; _LL432:;} static int Cyc_Toc_is_void_star(
void* t){ void* _temp492= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp498; void* _temp500; _LL494: if(( unsigned int) _temp492 > 4u?*(( int*)
_temp492) == Cyc_Absyn_PointerType: 0){ _LL499: _temp498=(( struct Cyc_Absyn_PointerType_struct*)
_temp492)->f1; _LL501: _temp500=( void*) _temp498.elt_typ; goto _LL495;} else{
goto _LL496;} _LL496: goto _LL497; _LL495: { void* _temp502= Cyc_Tcutil_compress(
_temp500); _LL504: if( _temp502 ==( void*) Cyc_Absyn_VoidType){ goto _LL505;}
else{ goto _LL506;} _LL506: goto _LL507; _LL505: return 1; _LL507: return 0;
_LL503:;} _LL497: return 0; _LL493:;} static int Cyc_Toc_is_poly_field( void* t,
struct _tagged_arr* f){ void* _temp508= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp520; struct Cyc_Absyn_Structdecl* _temp522; struct Cyc_List_List* _temp523;
_LL510: if(( unsigned int) _temp508 > 4u?*(( int*) _temp508) == Cyc_Absyn_StructType:
0){ _LL521: _temp520=(( struct Cyc_Absyn_StructType_struct*) _temp508)->f3; if(
_temp520 == 0){ goto _LL512;} else{ _temp522=* _temp520; goto _LL511;}} else{
goto _LL512;} _LL512: if(( unsigned int) _temp508 > 4u?*(( int*) _temp508) ==
Cyc_Absyn_AnonStructType: 0){ _LL524: _temp523=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp508)->f1; goto _LL513;} else{ goto _LL514;} _LL514: if(( unsigned int)
_temp508 > 4u?*(( int*) _temp508) == Cyc_Absyn_AnonUnionType: 0){ goto _LL515;}
else{ goto _LL516;} _LL516: if(( unsigned int) _temp508 > 4u?*(( int*) _temp508)
== Cyc_Absyn_UnionType: 0){ goto _LL517;} else{ goto _LL518;} _LL518: goto
_LL519; _LL511: if( _temp522->fields == 0){( int) _throw(({ void*(* _temp525)(
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Toc_toc_impos; struct
_tagged_arr _temp528= _tag_arr("is_poly_field: struct missing fields", sizeof(
unsigned char), 37u); void* _temp526[ 0u]={}; struct _tagged_arr _temp527={(
void*) _temp526,( void*) _temp526,( void*)( _temp526 + 0u)}; _temp525( _temp528,
_temp527);}));} _temp523=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp522->fields))->v; goto _LL513; _LL513: { struct Cyc_Absyn_Structfield*
_temp529= Cyc_Absyn_lookup_field( _temp523, f); if( _temp529 == 0){({ int(*
_temp530)( struct _tagged_arr fmt, struct _tagged_arr ap)=( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos; struct _tagged_arr
_temp534= _tag_arr("is_poly_field: bad field %s", sizeof( unsigned char), 28u);
struct Cyc_Stdio_String_pa_struct _temp535; _temp535.tag= Cyc_Stdio_String_pa;
_temp535.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Structfield*) _check_null(
_temp529))->name;{ void* _temp533=( void*)& _temp535; void* _temp531[ 1u]={
_temp533}; struct _tagged_arr _temp532={( void*) _temp531,( void*) _temp531,(
void*)( _temp531 + 1u)}; _temp530( _temp534, _temp532);}});} return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp529))->type);} _LL515:
return 0; _LL517: return 0; _LL519:( int) _throw(({ void*(* _temp536)( struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Toc_toc_impos; struct _tagged_arr
_temp540= _tag_arr("is_poly_field: bad type %s", sizeof( unsigned char), 27u);
struct Cyc_Stdio_String_pa_struct _temp541; _temp541.tag= Cyc_Stdio_String_pa;
_temp541.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp539=(
void*)& _temp541; void* _temp537[ 1u]={ _temp539}; struct _tagged_arr _temp538={(
void*) _temp537,( void*) _temp537,( void*)( _temp537 + 1u)}; _temp536( _temp540,
_temp538);}})); _LL509:;} static int Cyc_Toc_is_poly_project( struct Cyc_Absyn_Exp*
e){ void* _temp542=( void*) e->r; struct _tagged_arr* _temp550; struct Cyc_Absyn_Exp*
_temp552; struct _tagged_arr* _temp554; struct Cyc_Absyn_Exp* _temp556; _LL544:
if(*(( int*) _temp542) == Cyc_Absyn_StructMember_e){ _LL553: _temp552=(( struct
Cyc_Absyn_StructMember_e_struct*) _temp542)->f1; goto _LL551; _LL551: _temp550=((
struct Cyc_Absyn_StructMember_e_struct*) _temp542)->f2; goto _LL545;} else{ goto
_LL546;} _LL546: if(*(( int*) _temp542) == Cyc_Absyn_StructArrow_e){ _LL557:
_temp556=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp542)->f1; goto _LL555;
_LL555: _temp554=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp542)->f2; goto
_LL547;} else{ goto _LL548;} _LL548: goto _LL549; _LL545: return Cyc_Toc_is_poly_field((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp552->topt))->v, _temp550);
_LL547: { void* _temp558= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp556->topt))->v); struct Cyc_Absyn_PtrInfo _temp564; void*
_temp566; _LL560: if(( unsigned int) _temp558 > 4u?*(( int*) _temp558) == Cyc_Absyn_PointerType:
0){ _LL565: _temp564=(( struct Cyc_Absyn_PointerType_struct*) _temp558)->f1;
_LL567: _temp566=( void*) _temp564.elt_typ; goto _LL561;} else{ goto _LL562;}
_LL562: goto _LL563; _LL561: return Cyc_Toc_is_poly_field( _temp566, _temp554);
_LL563:({ int(* _temp568)( struct _tagged_arr fmt, struct _tagged_arr ap)=( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos; struct
_tagged_arr _temp572= _tag_arr("is_poly_project:  bad type %s", sizeof(
unsigned char), 30u); struct Cyc_Stdio_String_pa_struct _temp573; _temp573.tag=
Cyc_Stdio_String_pa; _temp573.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp556->topt))->v);{ void*
_temp571=( void*)& _temp573; void* _temp569[ 1u]={ _temp571}; struct _tagged_arr
_temp570={( void*) _temp569,( void*) _temp569,( void*)( _temp569 + 1u)};
_temp568( _temp572, _temp570);}}); return 0; _LL559:;} _LL549: return 0; _LL543:;}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Toc_GC_malloc_e,({ struct Cyc_List_List*
_temp574=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp574->hd=( void*) s; _temp574->tl= 0; _temp574;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_atomic( struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp(
Cyc_Toc_GC_malloc_atomic_e,({ struct Cyc_List_List* _temp575=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp575->hd=( void*) s; _temp575->tl=
0; _temp575;}), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t,
struct Cyc_Absyn_Exp* s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic(
s);} return Cyc_Toc_malloc_ptr( s);} static struct Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp* rgn, struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_malloc_e,({ struct Cyc_List_List*(* _temp576)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp579= rgn; struct Cyc_Absyn_Exp* _temp580= s; struct
Cyc_Absyn_Exp* _temp577[ 2u]={ _temp579, _temp580}; struct _tagged_arr _temp578={(
void*) _temp577,( void*) _temp577,( void*)( _temp577 + 2u)}; _temp576( _temp578);}),
0);} static struct Cyc_Absyn_Exp* Cyc_Toc_fdconv( struct Cyc_Absyn_Exp* e){
return Cyc_Absyn_fncall_exp( Cyc_Toc__sfile_to_file_e,({ struct Cyc_List_List*
_temp581=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp581->hd=( void*) e; _temp581->tl= 0; _temp581;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp( Cyc_Toc__throw_e,({
struct Cyc_List_List* _temp582=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp582->hd=( void*) e; _temp582->tl= 0; _temp582;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_make_toplevel_tagged_arr( void* t, struct
Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp* e){ int is_string= 0;{ void* _temp583=(
void*) e->r; void* _temp589; _LL585: if(*(( int*) _temp583) == Cyc_Absyn_Const_e){
_LL590: _temp589=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp583)->f1; if((
unsigned int) _temp589 > 1u?*(( int*) _temp589) == Cyc_Absyn_String_c: 0){ goto
_LL586;} else{ goto _LL587;}} else{ goto _LL587;} _LL587: goto _LL588; _LL586:
is_string= 1; goto _LL584; _LL588: goto _LL584; _LL584:;}{ struct Cyc_Absyn_Exp*
xexp; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var(); void* vd_typ=( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp594=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp594[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp595; _temp595.tag= Cyc_Absyn_ArrayType; _temp595.f1=(
void*) Cyc_Absyn_uchar_t; _temp595.f2= Cyc_Toc_mt_tq; _temp595.f3=( struct Cyc_Absyn_Exp*)
sz; _temp595;}); _temp594;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp591=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp591->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp592=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp592[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp593; _temp593.tag= Cyc_Absyn_Var_d;
_temp593.f1= vd; _temp593;}); _temp592;}), 0); _temp591->tl= Cyc_Toc_result_decls;
_temp591;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ xexp= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), e, 0);}{ struct Cyc_Absyn_Exp* xplussz= Cyc_Absyn_add_exp(
xexp, sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp( 0,({
struct Cyc_List_List*(* _temp596)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct _tuple4* _temp599=({ struct _tuple4*
_temp604=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp604->f1= 0;
_temp604->f2= xexp; _temp604;}); struct _tuple4* _temp600=({ struct _tuple4*
_temp603=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp603->f1= 0;
_temp603->f2= xexp; _temp603;}); struct _tuple4* _temp601=({ struct _tuple4*
_temp602=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp602->f1= 0;
_temp602->f2= xplussz; _temp602;}); struct _tuple4* _temp597[ 3u]={ _temp599,
_temp600, _temp601}; struct _tagged_arr _temp598={( void*) _temp597,( void*)
_temp597,( void*)( _temp597 + 3u)}; _temp596( _temp598);}), 0); return urm_exp;}}}
struct Cyc_Toc_Env{ struct Cyc_Core_Opt* break_lab; struct Cyc_Core_Opt*
continue_lab; struct Cyc_Core_Opt* fallthru_info; struct Cyc_Dict_Dict* varmap;
int toplevel; } ; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){ return({
struct Cyc_Toc_Env* _temp605=( struct Cyc_Toc_Env*) GC_malloc( sizeof( struct
Cyc_Toc_Env)); _temp605->break_lab= 0; _temp605->continue_lab= 0; _temp605->fallthru_info=
0; _temp605->varmap=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp605->toplevel= 1;
_temp605;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){
return({ struct Cyc_Toc_Env* _temp606=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp606->break_lab= e->break_lab; _temp606->continue_lab=
e->continue_lab; _temp606->fallthru_info= e->fallthru_info; _temp606->varmap= e->varmap;
_temp606->toplevel= e->toplevel; _temp606;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp607=(* x).f1; _LL609:
if(( unsigned int) _temp607 > 1u?*(( int*) _temp607) == Cyc_Absyn_Rel_n: 0){
goto _LL610;} else{ goto _LL611;} _LL611: goto _LL612; _LL610:( int) _throw(({
void*(* _temp613)( struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Toc_toc_impos;
struct _tagged_arr _temp617= _tag_arr("Toc::add_varmap on Rel_n: %s\n", sizeof(
unsigned char), 30u); struct Cyc_Stdio_String_pa_struct _temp618; _temp618.tag=
Cyc_Stdio_String_pa; _temp618.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
x);{ void* _temp616=( void*)& _temp618; void* _temp614[ 1u]={ _temp616}; struct
_tagged_arr _temp615={( void*) _temp614,( void*) _temp614,( void*)( _temp614 + 1u)};
_temp613( _temp617, _temp615);}})); _LL612: goto _LL608; _LL608:;}{ struct Cyc_Toc_Env*
ans= Cyc_Toc_copy_env( e); ans->varmap=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tuple0* key, struct Cyc_Absyn_Exp* data)) Cyc_Dict_insert)( ans->varmap,
x, y); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_loop_env( struct Cyc_Toc_Env*
e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->continue_lab=
0; return ans;} struct _tuple6{ struct _tagged_arr* f1; struct Cyc_List_List* f2;
struct Cyc_Dict_Dict* f3; } ; static struct Cyc_Toc_Env* Cyc_Toc_non_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_arr* break_l, struct _tagged_arr*
fallthru_l, struct Cyc_List_List* fallthru_binders, struct Cyc_Toc_Env*
next_case_env){ struct Cyc_List_List* _temp619= 0; for( 0; fallthru_binders != 0;
fallthru_binders=(( struct Cyc_List_List*) _check_null( fallthru_binders))->tl){
_temp619=({ struct Cyc_List_List* _temp620=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp620->hd=( void*)(( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( fallthru_binders))->hd)->name; _temp620->tl=
_temp619; _temp620;});} _temp619=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp619);{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab=({ struct Cyc_Core_Opt* _temp621=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp621->v=( void*) break_l; _temp621;});
ans->fallthru_info=({ struct Cyc_Core_Opt* _temp622=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp622->v=( void*)({ struct _tuple6*
_temp623=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp623->f1=
fallthru_l; _temp623->f2= _temp619; _temp623->f3= next_case_env->varmap;
_temp623;}); _temp622;}); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_arr* break_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt* _temp624=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp624->v=( void*)
break_l; _temp624;}); ans->fallthru_info= 0; return ans;} static struct Cyc_Toc_Env*
Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env* e, struct _tagged_arr* next_l){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->fallthru_info=({
struct Cyc_Core_Opt* _temp625=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp625->v=( void*)({ struct _tuple6* _temp626=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp626->f1= next_l; _temp626->f2= 0;
_temp626->f3=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct
_tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp626;}); _temp625;});
return ans;} static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e); static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt*
s); static void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp* e){ if( e->topt == 0){({
int(* _temp627)( struct _tagged_arr fmt, struct _tagged_arr ap)=( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos; struct _tagged_arr
_temp630= _tag_arr("Missing type in primop ", sizeof( unsigned char), 24u); void*
_temp628[ 0u]={}; struct _tagged_arr _temp629={( void*) _temp628,( void*)
_temp628,( void*)( _temp628 + 0u)}; _temp627( _temp630, _temp629);});} return
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);}
static void* Cyc_Toc_get_cyc_typ( struct Cyc_Absyn_Exp* e){ if( e->topt == 0){({
int(* _temp631)( struct _tagged_arr fmt, struct _tagged_arr ap)=( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos; struct _tagged_arr
_temp634= _tag_arr("Missing type in primop ", sizeof( unsigned char), 24u); void*
_temp632[ 0u]={}; struct _tagged_arr _temp633={( void*) _temp632,( void*)
_temp632,( void*)( _temp632 + 0u)}; _temp631( _temp634, _temp633);});} return(
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v;} static struct _tuple3*
Cyc_Toc_tup_to_c( struct Cyc_Absyn_Exp* e){ return({ struct _tuple3* _temp635=(
struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp635->f1= Cyc_Toc_mt_tq;
_temp635->f2= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_temp635;});} static struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4*
_temp636=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp636->f1= 0;
_temp636->f2= e; _temp636;});} static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct(
struct Cyc_Toc_Env* nv, struct _tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt*
s, int pointer, struct Cyc_Absyn_Exp* rgnopt, int is_atomic){ struct Cyc_Absyn_Exp*
eo; void* t; if( pointer){ t= Cyc_Absyn_cstar_typ( struct_typ, Cyc_Toc_mt_tq);{
struct Cyc_Absyn_Exp* _temp637= Cyc_Absyn_sizeoftyp_exp( struct_typ, 0); if(
rgnopt == 0){ eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, is_atomic? Cyc_Toc_malloc_atomic(
_temp637): Cyc_Toc_malloc_ptr( _temp637), 0);} else{ struct Cyc_Absyn_Exp* r=(
struct Cyc_Absyn_Exp*) _check_null( rgnopt); Cyc_Toc_exp_to_c( nv, r); eo=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp( r, _temp637),
0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) - 1;{ struct Cyc_List_List* _temp638=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( dles0); for( 0; _temp638 != 0; _temp638=((
struct Cyc_List_List*) _check_null( _temp638))->tl){ struct _tuple4 _temp641;
struct Cyc_Absyn_Exp* _temp642; struct Cyc_List_List* _temp644; struct _tuple4*
_temp639=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp638))->hd;
_temp641=* _temp639; _LL645: _temp644= _temp641.f1; goto _LL643; _LL643:
_temp642= _temp641.f2; goto _LL640; _LL640: { struct Cyc_Absyn_Exp* e_index; if(
_temp644 == 0){ e_index= Cyc_Absyn_signed_int_exp( count --, 0);} else{ if(((
struct Cyc_List_List*) _check_null( _temp644))->tl != 0){({ int(* _temp646)(
struct _tagged_arr fmt, struct _tagged_arr ap)=( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_unimp; struct _tagged_arr _temp649= _tag_arr("multiple designators in array",
sizeof( unsigned char), 30u); void* _temp647[ 0u]={}; struct _tagged_arr
_temp648={( void*) _temp647,( void*) _temp647,( void*)( _temp647 + 0u)};
_temp646( _temp649, _temp648);});}{ void* _temp650=( void*)(( struct Cyc_List_List*)
_check_null( _temp644))->hd; void* _temp651= _temp650; struct Cyc_Absyn_Exp*
_temp657; _LL653: if(*(( int*) _temp651) == Cyc_Absyn_ArrayElement){ _LL658:
_temp657=(( struct Cyc_Absyn_ArrayElement_struct*) _temp651)->f1; goto _LL654;}
else{ goto _LL655;} _LL655: if(*(( int*) _temp651) == Cyc_Absyn_FieldName){ goto
_LL656;} else{ goto _LL652;} _LL654: Cyc_Toc_exp_to_c( nv, _temp657); e_index=
_temp657; goto _LL652; _LL656: e_index=({ struct Cyc_Absyn_Exp*(* _temp659)(
struct _tagged_arr fmt, struct _tagged_arr ap)=( struct Cyc_Absyn_Exp*(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp; struct
_tagged_arr _temp662= _tag_arr("field name designators in array", sizeof(
unsigned char), 32u); void* _temp660[ 0u]={}; struct _tagged_arr _temp661={(
void*) _temp660,( void*) _temp660,( void*)( _temp660 + 0u)}; _temp659( _temp662,
_temp661);}); goto _LL652; _LL652:;}}{ struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp(
lhs, e_index, 0); void* _temp663=( void*) _temp642->r; struct Cyc_List_List*
_temp673; struct Cyc_Absyn_Exp* _temp675; struct Cyc_Absyn_Exp* _temp677; struct
Cyc_Absyn_Vardecl* _temp679; struct Cyc_List_List* _temp681; void* _temp683;
_LL665: if(*(( int*) _temp663) == Cyc_Absyn_Array_e){ _LL674: _temp673=(( struct
Cyc_Absyn_Array_e_struct*) _temp663)->f1; goto _LL666;} else{ goto _LL667;}
_LL667: if(*(( int*) _temp663) == Cyc_Absyn_Comprehension_e){ _LL680: _temp679=((
struct Cyc_Absyn_Comprehension_e_struct*) _temp663)->f1; goto _LL678; _LL678:
_temp677=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp663)->f2; goto _LL676;
_LL676: _temp675=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp663)->f3;
goto _LL668;} else{ goto _LL669;} _LL669: if(*(( int*) _temp663) == Cyc_Absyn_AnonStruct_e){
_LL684: _temp683=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp663)->f1;
goto _LL682; _LL682: _temp681=(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp663)->f2;
goto _LL670;} else{ goto _LL671;} _LL671: goto _LL672; _LL666: s= Cyc_Toc_init_array(
nv, lval, _temp673, s); goto _LL664; _LL668: s= Cyc_Toc_init_comprehension( nv,
lval, _temp679, _temp677, _temp675, s, 0); goto _LL664; _LL670: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp683, _temp681, s); goto _LL664; _LL672: Cyc_Toc_exp_to_c( nv,
_temp642); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
lhs, e_index, 0), _temp642, 0), s, 0); goto _LL664; _LL664:;}}}} return s;}
static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s, int
e1_already_translated){ struct _tuple0* _temp685= vd->name; if( !
e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env* nv2=
Cyc_Toc_add_varmap( nv, _temp685, Cyc_Absyn_varb_exp( _temp685,( void*)({ struct
Cyc_Absyn_Local_b_struct* _temp708=( struct Cyc_Absyn_Local_b_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Local_b_struct)); _temp708[ 0]=({ struct Cyc_Absyn_Local_b_struct
_temp709; _temp709.tag= Cyc_Absyn_Local_b; _temp709.f1= vd; _temp709;});
_temp708;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( _temp685, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp(
_temp685, 0), Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp(
Cyc_Absyn_var_exp( _temp685, 0), 0); struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp(
lhs, Cyc_Absyn_var_exp( _temp685, 0), 0); struct Cyc_Absyn_Stmt* body;{ void*
_temp686=( void*) e2->r; struct Cyc_List_List* _temp696; struct Cyc_Absyn_Exp*
_temp698; struct Cyc_Absyn_Exp* _temp700; struct Cyc_Absyn_Vardecl* _temp702;
struct Cyc_List_List* _temp704; void* _temp706; _LL688: if(*(( int*) _temp686)
== Cyc_Absyn_Array_e){ _LL697: _temp696=(( struct Cyc_Absyn_Array_e_struct*)
_temp686)->f1; goto _LL689;} else{ goto _LL690;} _LL690: if(*(( int*) _temp686)
== Cyc_Absyn_Comprehension_e){ _LL703: _temp702=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp686)->f1; goto _LL701; _LL701: _temp700=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp686)->f2; goto _LL699; _LL699: _temp698=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp686)->f3; goto _LL691;} else{ goto _LL692;} _LL692: if(*(( int*) _temp686)
== Cyc_Absyn_AnonStruct_e){ _LL707: _temp706=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp686)->f1; goto _LL705; _LL705: _temp704=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp686)->f2; goto _LL693;} else{ goto _LL694;} _LL694: goto _LL695; _LL689:
body= Cyc_Toc_init_array( nv2, lval, _temp696, Cyc_Absyn_skip_stmt( 0)); goto
_LL687; _LL691: body= Cyc_Toc_init_comprehension( nv2, lval, _temp702, _temp700,
_temp698, Cyc_Absyn_skip_stmt( 0), 0); goto _LL687; _LL693: body= Cyc_Toc_init_anon_struct(
nv, lval, _temp706, _temp704, Cyc_Absyn_skip_stmt( 0)); goto _LL687; _LL695: Cyc_Toc_exp_to_c(
nv2, e2); body= Cyc_Absyn_assign_stmt( lval, e2, 0); goto _LL687; _LL687:;}{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt( ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt(
_temp685, Cyc_Absyn_uint_t, 0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_anon_struct( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs,
void* struct_type, struct Cyc_List_List* dles, struct Cyc_Absyn_Stmt* s){{
struct Cyc_List_List* _temp710=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( dles); for( 0; _temp710 != 0; _temp710=(( struct Cyc_List_List*)
_check_null( _temp710))->tl){ struct _tuple4 _temp713; struct Cyc_Absyn_Exp*
_temp714; struct Cyc_List_List* _temp716; struct _tuple4* _temp711=( struct
_tuple4*)(( struct Cyc_List_List*) _check_null( _temp710))->hd; _temp713=*
_temp711; _LL717: _temp716= _temp713.f1; goto _LL715; _LL715: _temp714= _temp713.f2;
goto _LL712; _LL712: if( _temp716 == 0){({ int(* _temp718)( struct _tagged_arr
fmt, struct _tagged_arr ap)=( int(*)( struct _tagged_arr fmt, struct _tagged_arr
ap)) Cyc_Toc_toc_impos; struct _tagged_arr _temp721= _tag_arr("empty designator list",
sizeof( unsigned char), 22u); void* _temp719[ 0u]={}; struct _tagged_arr
_temp720={( void*) _temp719,( void*) _temp719,( void*)( _temp719 + 0u)};
_temp718( _temp721, _temp720);});} if((( struct Cyc_List_List*) _check_null(
_temp716))->tl != 0){({ int(* _temp722)( struct _tagged_arr fmt, struct
_tagged_arr ap)=( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos;
struct _tagged_arr _temp725= _tag_arr("too many designators in anonymous struct",
sizeof( unsigned char), 41u); void* _temp723[ 0u]={}; struct _tagged_arr
_temp724={( void*) _temp723,( void*) _temp723,( void*)( _temp723 + 0u)};
_temp722( _temp725, _temp724);});}{ void* _temp726=( void*)(( struct Cyc_List_List*)
_check_null( _temp716))->hd; struct _tagged_arr* _temp732; _LL728: if(*(( int*)
_temp726) == Cyc_Absyn_FieldName){ _LL733: _temp732=(( struct Cyc_Absyn_FieldName_struct*)
_temp726)->f1; goto _LL729;} else{ goto _LL730;} _LL730: goto _LL731; _LL729: {
struct Cyc_Absyn_Exp* lval= Cyc_Absyn_structmember_exp( lhs, _temp732, 0);{ void*
_temp734=( void*) _temp714->r; struct Cyc_List_List* _temp744; struct Cyc_Absyn_Exp*
_temp746; struct Cyc_Absyn_Exp* _temp748; struct Cyc_Absyn_Vardecl* _temp750;
struct Cyc_List_List* _temp752; void* _temp754; _LL736: if(*(( int*) _temp734)
== Cyc_Absyn_Array_e){ _LL745: _temp744=(( struct Cyc_Absyn_Array_e_struct*)
_temp734)->f1; goto _LL737;} else{ goto _LL738;} _LL738: if(*(( int*) _temp734)
== Cyc_Absyn_Comprehension_e){ _LL751: _temp750=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp734)->f1; goto _LL749; _LL749: _temp748=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp734)->f2; goto _LL747; _LL747: _temp746=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp734)->f3; goto _LL739;} else{ goto _LL740;} _LL740: if(*(( int*) _temp734)
== Cyc_Absyn_AnonStruct_e){ _LL755: _temp754=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp734)->f1; goto _LL753; _LL753: _temp752=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp734)->f2; goto _LL741;} else{ goto _LL742;} _LL742: goto _LL743; _LL737: s=
Cyc_Toc_init_array( nv, lval, _temp744, s); goto _LL735; _LL739: s= Cyc_Toc_init_comprehension(
nv, lval, _temp750, _temp748, _temp746, s, 0); goto _LL735; _LL741: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp754, _temp752, s); goto _LL735; _LL743: Cyc_Toc_exp_to_c( nv,
_temp714); if( Cyc_Toc_is_poly_field( struct_type, _temp732)){ _temp714= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp714, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp714, 0), 0), s, 0); goto _LL735; _LL735:;} goto
_LL727;} _LL731:( int) _throw(({ void*(* _temp756)( struct _tagged_arr fmt,
struct _tagged_arr ap)= Cyc_Toc_toc_impos; struct _tagged_arr _temp759= _tag_arr("array designator in struct",
sizeof( unsigned char), 27u); void* _temp757[ 0u]={}; struct _tagged_arr
_temp758={( void*) _temp757,( void*) _temp757,( void*)( _temp757 + 0u)};
_temp756( _temp759, _temp758);})); _LL727:;}}} return s;} static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple( struct Cyc_Toc_Env* nv, int pointer, struct Cyc_Absyn_Exp*
rgnopt, struct Cyc_List_List* es){ struct _RegionHandle _temp760= _new_region();
struct _RegionHandle* r=& _temp760; _push_region( r);{ struct Cyc_List_List*
_temp761=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple3*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_rmap)( r, Cyc_Toc_tup_to_c,
es); struct _tagged_arr* _temp762= Cyc_Toc_add_tuple_type( _temp761); struct
_tuple0* _temp763= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp764= Cyc_Absyn_var_exp(
_temp763, 0); struct Cyc_Absyn_Stmt* _temp765= Cyc_Absyn_exp_stmt( _temp764, 0);
struct Cyc_Absyn_Exp*(* _temp766)( struct Cyc_Absyn_Exp*, struct _tagged_arr*,
struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp;
int is_atomic= 1; struct Cyc_List_List* _temp767=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, es);{ int i=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp767); for( 0; _temp767
!= 0;( _temp767=(( struct Cyc_List_List*) _check_null( _temp767))->tl, -- i)){
struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp767))->hd; struct Cyc_Absyn_Exp* lval= _temp766( _temp764, Cyc_Toc_fieldname(
i), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v): 0;{ void* _temp768=( void*) e->r; struct Cyc_List_List*
_temp776; struct Cyc_Absyn_Exp* _temp778; struct Cyc_Absyn_Exp* _temp780; struct
Cyc_Absyn_Vardecl* _temp782; _LL770: if(*(( int*) _temp768) == Cyc_Absyn_Array_e){
_LL777: _temp776=(( struct Cyc_Absyn_Array_e_struct*) _temp768)->f1; goto _LL771;}
else{ goto _LL772;} _LL772: if(*(( int*) _temp768) == Cyc_Absyn_Comprehension_e){
_LL783: _temp782=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp768)->f1;
goto _LL781; _LL781: _temp780=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp768)->f2; goto _LL779; _LL779: _temp778=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp768)->f3; goto _LL773;} else{ goto _LL774;} _LL774: goto _LL775; _LL771:
_temp765= Cyc_Toc_init_array( nv, lval, _temp776, _temp765); goto _LL769; _LL773:
_temp765= Cyc_Toc_init_comprehension( nv, lval, _temp782, _temp780, _temp778,
_temp765, 0); goto _LL769; _LL775: Cyc_Toc_exp_to_c( nv, e); _temp765= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp766( _temp764, Cyc_Toc_fieldname(
i), 0), e, 0), 0), _temp765, 0); goto _LL769; _LL769:;}}}{ struct Cyc_Absyn_Exp*
_temp784= Cyc_Toc_make_struct( nv, _temp763, Cyc_Absyn_strct( _temp762),
_temp765, pointer, rgnopt, is_atomic); _npop_handler( 0u); return _temp784;}};
_pop_region( r);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env*
nv, void* struct_type, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
dles, struct _tuple0* tdn){ struct _tuple0* _temp785= Cyc_Toc_temp_var(); struct
Cyc_Absyn_Exp* _temp786= Cyc_Absyn_var_exp( _temp785, 0); struct Cyc_Absyn_Stmt*
_temp787= Cyc_Absyn_exp_stmt( _temp786, 0); struct Cyc_Absyn_Exp*(* _temp788)(
struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; int is_atomic= 1;
struct _RegionHandle _temp789= _new_region(); struct _RegionHandle* r=& _temp789;
_push_region( r);{ struct Cyc_List_List* _temp790=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, dles); for(
0; _temp790 != 0; _temp790=(( struct Cyc_List_List*) _check_null( _temp790))->tl){
struct _tuple4 _temp793; struct Cyc_Absyn_Exp* _temp794; struct Cyc_List_List*
_temp796; struct _tuple4* _temp791=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp790))->hd; _temp793=* _temp791; _LL797: _temp796= _temp793.f1;
goto _LL795; _LL795: _temp794= _temp793.f2; goto _LL792; _LL792: is_atomic=
is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp794->topt))->v): 0; if( _temp796 == 0){({ int(* _temp798)( struct
_tagged_arr fmt, struct _tagged_arr ap)=( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos; struct _tagged_arr _temp801= _tag_arr("empty designator list",
sizeof( unsigned char), 22u); void* _temp799[ 0u]={}; struct _tagged_arr
_temp800={( void*) _temp799,( void*) _temp799,( void*)( _temp799 + 0u)};
_temp798( _temp801, _temp800);});} if((( struct Cyc_List_List*) _check_null(
_temp796))->tl != 0){ struct _tuple0* _temp802= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp803= Cyc_Absyn_var_exp( _temp802, 0); for( 0; _temp796 != 0; _temp796=((
struct Cyc_List_List*) _check_null( _temp796))->tl){ void* _temp804=( void*)((
struct Cyc_List_List*) _check_null( _temp796))->hd; struct _tagged_arr* _temp810;
_LL806: if(*(( int*) _temp804) == Cyc_Absyn_FieldName){ _LL811: _temp810=((
struct Cyc_Absyn_FieldName_struct*) _temp804)->f1; goto _LL807;} else{ goto
_LL808;} _LL808: goto _LL809; _LL807: if( Cyc_Toc_is_poly_field( struct_type,
_temp810)){ _temp803= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), _temp803, 0);}
_temp787= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp788(
_temp786, _temp810, 0), _temp803, 0), 0), _temp787, 0); goto _LL805; _LL809:(
int) _throw(({ void*(* _temp812)( struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Toc_toc_impos; struct _tagged_arr _temp815= _tag_arr("array designator in struct",
sizeof( unsigned char), 27u); void* _temp813[ 0u]={}; struct _tagged_arr
_temp814={( void*) _temp813,( void*) _temp813,( void*)( _temp813 + 0u)};
_temp812( _temp815, _temp814);})); _LL805:;} Cyc_Toc_exp_to_c( nv, _temp794);
_temp787= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp803,
_temp794, 0), 0), _temp787, 0);} else{ void* _temp816=( void*)(( struct Cyc_List_List*)
_check_null( _temp796))->hd; struct _tagged_arr* _temp822; _LL818: if(*(( int*)
_temp816) == Cyc_Absyn_FieldName){ _LL823: _temp822=(( struct Cyc_Absyn_FieldName_struct*)
_temp816)->f1; goto _LL819;} else{ goto _LL820;} _LL820: goto _LL821; _LL819: {
struct Cyc_Absyn_Exp* lval= _temp788( _temp786, _temp822, 0);{ void* _temp824=(
void*) _temp794->r; struct Cyc_List_List* _temp834; struct Cyc_Absyn_Exp*
_temp836; struct Cyc_Absyn_Exp* _temp838; struct Cyc_Absyn_Vardecl* _temp840;
struct Cyc_List_List* _temp842; void* _temp844; _LL826: if(*(( int*) _temp824)
== Cyc_Absyn_Array_e){ _LL835: _temp834=(( struct Cyc_Absyn_Array_e_struct*)
_temp824)->f1; goto _LL827;} else{ goto _LL828;} _LL828: if(*(( int*) _temp824)
== Cyc_Absyn_Comprehension_e){ _LL841: _temp840=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp824)->f1; goto _LL839; _LL839: _temp838=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp824)->f2; goto _LL837; _LL837: _temp836=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp824)->f3; goto _LL829;} else{ goto _LL830;} _LL830: if(*(( int*) _temp824)
== Cyc_Absyn_AnonStruct_e){ _LL845: _temp844=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp824)->f1; goto _LL843; _LL843: _temp842=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp824)->f2; goto _LL831;} else{ goto _LL832;} _LL832: goto _LL833; _LL827:
_temp787= Cyc_Toc_init_array( nv, lval, _temp834, _temp787); goto _LL825; _LL829:
_temp787= Cyc_Toc_init_comprehension( nv, lval, _temp840, _temp838, _temp836,
_temp787, 0); goto _LL825; _LL831: _temp787= Cyc_Toc_init_anon_struct( nv, lval,
_temp844, _temp842, _temp787); goto _LL825; _LL833: Cyc_Toc_exp_to_c( nv,
_temp794); if( Cyc_Toc_is_poly_field( struct_type, _temp822)){ _temp794= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp794, 0);} _temp787= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp794, 0), 0), _temp787, 0); goto _LL825; _LL825:;}
goto _LL817;} _LL821:( int) _throw(({ void*(* _temp846)( struct _tagged_arr fmt,
struct _tagged_arr ap)= Cyc_Toc_toc_impos; struct _tagged_arr _temp849= _tag_arr("array designator in struct",
sizeof( unsigned char), 27u); void* _temp847[ 0u]={}; struct _tagged_arr
_temp848={( void*) _temp847,( void*) _temp847,( void*)( _temp847 + 0u)};
_temp846( _temp849, _temp848);})); _LL817:;}}}{ struct Cyc_Absyn_Exp* _temp850=
Cyc_Toc_make_struct( nv, _temp785, Cyc_Absyn_strctq( tdn), _temp787, pointer,
rgnopt, is_atomic); _npop_handler( 0u); return _temp850;}; _pop_region( r);}
struct _tuple7{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ; static
struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp* el, struct
_tuple7* pr){ return Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2, 0);} static
struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp* e1, int
ignore){ return Cyc_Absyn_address_exp( e1, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue(
struct Cyc_Absyn_Exp* e1, void* incr){ return Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Increment_e_struct* _temp851=( struct Cyc_Absyn_Increment_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp851[ 0]=({ struct
Cyc_Absyn_Increment_e_struct _temp852; _temp852.tag= Cyc_Absyn_Increment_e;
_temp852.f1= e1; _temp852.f2=( void*) incr; _temp852;}); _temp851;}), 0);}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env);
static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp853=( void*) e1->r; struct Cyc_Absyn_Stmt* _temp863; struct Cyc_Absyn_Exp*
_temp865; void* _temp867; struct _tagged_arr* _temp869; struct Cyc_Absyn_Exp*
_temp871; _LL855: if(*(( int*) _temp853) == Cyc_Absyn_StmtExp_e){ _LL864:
_temp863=(( struct Cyc_Absyn_StmtExp_e_struct*) _temp853)->f1; goto _LL856;}
else{ goto _LL857;} _LL857: if(*(( int*) _temp853) == Cyc_Absyn_Cast_e){ _LL868:
_temp867=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp853)->f1; goto _LL866;
_LL866: _temp865=(( struct Cyc_Absyn_Cast_e_struct*) _temp853)->f2; goto _LL858;}
else{ goto _LL859;} _LL859: if(*(( int*) _temp853) == Cyc_Absyn_StructMember_e){
_LL872: _temp871=(( struct Cyc_Absyn_StructMember_e_struct*) _temp853)->f1; goto
_LL870; _LL870: _temp869=(( struct Cyc_Absyn_StructMember_e_struct*) _temp853)->f2;
goto _LL860;} else{ goto _LL861;} _LL861: goto _LL862; _LL856: Cyc_Toc_lvalue_assign_stmt(
_temp863, fs, f, f_env); goto _LL854; _LL858: Cyc_Toc_lvalue_assign( _temp865,
fs, f, f_env); goto _LL854; _LL860:( void*)( e1->r=( void*)(( void*) _temp871->r));
Cyc_Toc_lvalue_assign( e1,({ struct Cyc_List_List* _temp873=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp873->hd=( void*) _temp869;
_temp873->tl= fs; _temp873;}), f, f_env); goto _LL854; _LL862: if( ! Cyc_Absyn_is_lvalue(
e1)){({ int(* _temp874)( struct _tagged_arr fmt, struct _tagged_arr ap)=( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos; struct
_tagged_arr _temp878= _tag_arr("lvalue_assign: could not find lvalue in `%s'",
sizeof( unsigned char), 45u); struct Cyc_Stdio_String_pa_struct _temp879;
_temp879.tag= Cyc_Stdio_String_pa; _temp879.f1=( struct _tagged_arr) Cyc_Absynpp_exp2string(
e1);{ void* _temp877=( void*)& _temp879; void* _temp875[ 1u]={ _temp877}; struct
_tagged_arr _temp876={( void*) _temp875,( void*) _temp875,( void*)( _temp875 + 1u)};
_temp874( _temp878, _temp876);}});}{ struct Cyc_Absyn_Exp* e1_copy= Cyc_Absyn_copy_exp(
e1); for( 0; fs != 0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){
e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( fs))->hd, e1_copy->loc);}( void*)( e1->r=( void*)(( void*)( f(
e1_copy, f_env))->r)); goto _LL854;} _LL854:;} static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env){ void* _temp880=( void*) s->r;
struct Cyc_Absyn_Exp* _temp890; struct Cyc_Absyn_Stmt* _temp892; struct Cyc_Absyn_Decl*
_temp894; struct Cyc_Absyn_Stmt* _temp896; _LL882: if(( unsigned int) _temp880 >
1u?*(( int*) _temp880) == Cyc_Absyn_Exp_s: 0){ _LL891: _temp890=(( struct Cyc_Absyn_Exp_s_struct*)
_temp880)->f1; goto _LL883;} else{ goto _LL884;} _LL884: if(( unsigned int)
_temp880 > 1u?*(( int*) _temp880) == Cyc_Absyn_Decl_s: 0){ _LL895: _temp894=((
struct Cyc_Absyn_Decl_s_struct*) _temp880)->f1; goto _LL893; _LL893: _temp892=((
struct Cyc_Absyn_Decl_s_struct*) _temp880)->f2; goto _LL885;} else{ goto _LL886;}
_LL886: if(( unsigned int) _temp880 > 1u?*(( int*) _temp880) == Cyc_Absyn_Seq_s:
0){ _LL897: _temp896=(( struct Cyc_Absyn_Seq_s_struct*) _temp880)->f2; goto
_LL887;} else{ goto _LL888;} _LL888: goto _LL889; _LL883: Cyc_Toc_lvalue_assign(
_temp890, fs, f, f_env); goto _LL881; _LL885: Cyc_Toc_lvalue_assign_stmt(
_temp892, fs, f, f_env); goto _LL881; _LL887: Cyc_Toc_lvalue_assign_stmt(
_temp896, fs, f, f_env); goto _LL881; _LL889:({ int(* _temp898)( struct
_tagged_arr fmt, struct _tagged_arr ap)=( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos; struct _tagged_arr _temp902= _tag_arr("lvalue_assign_stmt: %s",
sizeof( unsigned char), 23u); struct Cyc_Stdio_String_pa_struct _temp903;
_temp903.tag= Cyc_Stdio_String_pa; _temp903.f1=( struct _tagged_arr) Cyc_Absynpp_stmt2string(
s);{ void* _temp901=( void*)& _temp903; void* _temp899[ 1u]={ _temp901}; struct
_tagged_arr _temp900={( void*) _temp899,( void*) _temp899,( void*)( _temp899 + 1u)};
_temp898( _temp902, _temp900);}}); goto _LL881; _LL881:;} static struct Cyc_List_List*
Cyc_Toc_rmap_2c( struct _RegionHandle* r2, void*(* f)( void*, void*), void* env,
struct Cyc_List_List* x){ struct Cyc_List_List* result; struct Cyc_List_List*
prev; if( x == 0){ return 0;} result=({ struct Cyc_List_List* _temp904=( struct
Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List)); _temp904->hd=(
void*) f(( void*)(( struct Cyc_List_List*) _check_null( x))->hd, env); _temp904->tl=
0; _temp904;}); prev= result; for( x=(( struct Cyc_List_List*) _check_null( x))->tl;
x != 0; x=(( struct Cyc_List_List*) _check_null( x))->tl){(( struct Cyc_List_List*)
_check_null( prev))->tl=({ struct Cyc_List_List* _temp905=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp905->hd=( void*) f((
void*)(( struct Cyc_List_List*) _check_null( x))->hd, env); _temp905->tl= 0;
_temp905;}); prev=(( struct Cyc_List_List*) _check_null( prev))->tl;} return
result;} static struct Cyc_List_List* Cyc_Toc_map_2c( void*(* f)( void*, void*),
void* env, struct Cyc_List_List* x){ return Cyc_Toc_rmap_2c( Cyc_Core_heap_region,
f, env, x);} static struct _tuple4* Cyc_Toc_make_dle( struct Cyc_Absyn_Exp* e){
return({ struct _tuple4* _temp906=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp906->f1= 0; _temp906->f2= e; _temp906;});} static struct Cyc_Absyn_PtrInfo
Cyc_Toc_get_ptr_type( void* t){ void* _temp907= Cyc_Tcutil_compress( t); struct
Cyc_Absyn_PtrInfo _temp913; _LL909: if(( unsigned int) _temp907 > 4u?*(( int*)
_temp907) == Cyc_Absyn_PointerType: 0){ _LL914: _temp913=(( struct Cyc_Absyn_PointerType_struct*)
_temp907)->f1; goto _LL910;} else{ goto _LL911;} _LL911: goto _LL912; _LL910:
return _temp913; _LL912:( int) _throw(({ void*(* _temp915)( struct _tagged_arr
fmt, struct _tagged_arr ap)= Cyc_Tcutil_impos; struct _tagged_arr _temp918=
_tag_arr("get_ptr_typ:  not a pointer!", sizeof( unsigned char), 29u); void*
_temp916[ 0u]={}; struct _tagged_arr _temp917={( void*) _temp916,( void*)
_temp916,( void*)( _temp916 + 0u)}; _temp915( _temp918, _temp917);})); _LL908:;}
struct _tuple8{ void* f1; void* f2; } ; struct _tuple9{ struct _tuple0* f1; void*
f2; struct Cyc_Absyn_Exp* f3; } ; struct _tuple10{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Exp* e){ void* _temp919=( void*) e->r; if( e->topt == 0){({
int(* _temp920)( struct _tagged_arr fmt, struct _tagged_arr ap)=( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos; struct _tagged_arr
_temp924= _tag_arr("exp_to_c: no type for %s", sizeof( unsigned char), 25u);
struct Cyc_Stdio_String_pa_struct _temp925; _temp925.tag= Cyc_Stdio_String_pa;
_temp925.f1=( struct _tagged_arr) Cyc_Absynpp_exp2string( e);{ void* _temp923=(
void*)& _temp925; void* _temp921[ 1u]={ _temp923}; struct _tagged_arr _temp922={(
void*) _temp921,( void*) _temp921,( void*)( _temp921 + 1u)}; _temp920( _temp924,
_temp922);}}); return;}{ void* old_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; void* _temp926= _temp919; void* _temp1006; void*
_temp1008; struct _tuple0* _temp1010; struct _tuple0* _temp1012; struct Cyc_List_List*
_temp1014; void* _temp1016; void* _temp1018; struct Cyc_Absyn_Exp* _temp1020;
struct Cyc_Absyn_Exp* _temp1022; struct Cyc_Core_Opt* _temp1024; struct Cyc_Absyn_Exp*
_temp1026; struct Cyc_Absyn_Exp* _temp1028; struct Cyc_Absyn_Exp* _temp1030;
struct Cyc_Absyn_Exp* _temp1032; struct Cyc_Absyn_Exp* _temp1034; struct Cyc_Absyn_Exp*
_temp1036; struct Cyc_List_List* _temp1038; struct Cyc_Absyn_Exp* _temp1040;
struct Cyc_Absyn_VarargCallInfo* _temp1042; struct Cyc_List_List* _temp1044;
struct Cyc_Absyn_Exp* _temp1046; struct Cyc_Absyn_VarargCallInfo* _temp1048;
struct Cyc_Absyn_VarargCallInfo _temp1050; struct Cyc_Absyn_VarargInfo*
_temp1051; struct Cyc_List_List* _temp1053; int _temp1055; struct Cyc_List_List*
_temp1057; struct Cyc_Absyn_Exp* _temp1059; struct Cyc_Absyn_Exp* _temp1061;
struct Cyc_Absyn_Exp* _temp1063; struct Cyc_List_List* _temp1065; struct Cyc_Absyn_Exp*
_temp1067; struct Cyc_Absyn_Exp* _temp1069; void* _temp1071; void** _temp1073;
struct Cyc_Absyn_Exp* _temp1074; struct Cyc_Absyn_Exp* _temp1076; struct Cyc_Absyn_Exp*
_temp1078; struct Cyc_Absyn_Exp* _temp1080; void* _temp1082; struct _tagged_arr*
_temp1084; void* _temp1086; struct Cyc_Absyn_Exp* _temp1088; struct _tagged_arr*
_temp1090; struct Cyc_Absyn_Exp* _temp1092; struct _tagged_arr* _temp1094;
struct Cyc_Absyn_Exp* _temp1096; struct Cyc_Absyn_Exp* _temp1098; struct Cyc_Absyn_Exp*
_temp1100; struct Cyc_List_List* _temp1102; struct Cyc_List_List* _temp1104;
struct Cyc_Absyn_Exp* _temp1106; struct Cyc_Absyn_Exp* _temp1108; struct Cyc_Absyn_Vardecl*
_temp1110; struct Cyc_Absyn_Structdecl* _temp1112; struct Cyc_List_List*
_temp1114; struct Cyc_Core_Opt* _temp1116; struct _tuple0* _temp1118; struct Cyc_List_List*
_temp1120; void* _temp1122; struct Cyc_Absyn_Tunionfield* _temp1124; struct Cyc_Absyn_Tuniondecl*
_temp1126; struct Cyc_List_List* _temp1128; struct Cyc_Absyn_Tunionfield*
_temp1130; struct Cyc_Absyn_Tuniondecl* _temp1132; struct Cyc_List_List*
_temp1134; void* _temp1136; struct Cyc_Absyn_Exp* _temp1138; struct Cyc_Absyn_Stmt*
_temp1140; struct Cyc_Absyn_Fndecl* _temp1142; _LL928: if(*(( int*) _temp926) ==
Cyc_Absyn_Const_e){ _LL1007: _temp1006=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp926)->f1; if( _temp1006 ==( void*) Cyc_Absyn_Null_c){ goto _LL929;} else{
goto _LL930;}} else{ goto _LL930;} _LL930: if(*(( int*) _temp926) == Cyc_Absyn_Const_e){
goto _LL931;} else{ goto _LL932;} _LL932: if(*(( int*) _temp926) == Cyc_Absyn_Var_e){
_LL1011: _temp1010=(( struct Cyc_Absyn_Var_e_struct*) _temp926)->f1; goto
_LL1009; _LL1009: _temp1008=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp926)->f2;
goto _LL933;} else{ goto _LL934;} _LL934: if(*(( int*) _temp926) == Cyc_Absyn_UnknownId_e){
_LL1013: _temp1012=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp926)->f1; goto
_LL935;} else{ goto _LL936;} _LL936: if(*(( int*) _temp926) == Cyc_Absyn_Primop_e){
_LL1017: _temp1016=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp926)->f1;
goto _LL1015; _LL1015: _temp1014=(( struct Cyc_Absyn_Primop_e_struct*) _temp926)->f2;
goto _LL937;} else{ goto _LL938;} _LL938: if(*(( int*) _temp926) == Cyc_Absyn_Increment_e){
_LL1021: _temp1020=(( struct Cyc_Absyn_Increment_e_struct*) _temp926)->f1; goto
_LL1019; _LL1019: _temp1018=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp926)->f2; goto _LL939;} else{ goto _LL940;} _LL940: if(*(( int*) _temp926)
== Cyc_Absyn_AssignOp_e){ _LL1027: _temp1026=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp926)->f1; goto _LL1025; _LL1025: _temp1024=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp926)->f2; goto _LL1023; _LL1023: _temp1022=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp926)->f3; goto _LL941;} else{ goto _LL942;} _LL942: if(*(( int*) _temp926)
== Cyc_Absyn_Conditional_e){ _LL1033: _temp1032=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp926)->f1; goto _LL1031; _LL1031: _temp1030=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp926)->f2; goto _LL1029; _LL1029: _temp1028=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp926)->f3; goto _LL943;} else{ goto _LL944;} _LL944: if(*(( int*) _temp926)
== Cyc_Absyn_SeqExp_e){ _LL1037: _temp1036=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp926)->f1; goto _LL1035; _LL1035: _temp1034=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp926)->f2; goto _LL945;} else{ goto _LL946;} _LL946: if(*(( int*) _temp926)
== Cyc_Absyn_UnknownCall_e){ _LL1041: _temp1040=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp926)->f1; goto _LL1039; _LL1039: _temp1038=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp926)->f2; goto _LL947;} else{ goto _LL948;} _LL948: if(*(( int*) _temp926)
== Cyc_Absyn_FnCall_e){ _LL1047: _temp1046=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp926)->f1; goto _LL1045; _LL1045: _temp1044=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp926)->f2; goto _LL1043; _LL1043: _temp1042=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp926)->f3; if( _temp1042 == 0){ goto _LL949;} else{ goto _LL950;}} else{
goto _LL950;} _LL950: if(*(( int*) _temp926) == Cyc_Absyn_FnCall_e){ _LL1060:
_temp1059=(( struct Cyc_Absyn_FnCall_e_struct*) _temp926)->f1; goto _LL1058;
_LL1058: _temp1057=(( struct Cyc_Absyn_FnCall_e_struct*) _temp926)->f2; goto
_LL1049; _LL1049: _temp1048=(( struct Cyc_Absyn_FnCall_e_struct*) _temp926)->f3;
if( _temp1048 == 0){ goto _LL952;} else{ _temp1050=* _temp1048; _LL1056:
_temp1055= _temp1050.num_varargs; goto _LL1054; _LL1054: _temp1053= _temp1050.injectors;
goto _LL1052; _LL1052: _temp1051= _temp1050.vai; goto _LL951;}} else{ goto
_LL952;} _LL952: if(*(( int*) _temp926) == Cyc_Absyn_Throw_e){ _LL1062:
_temp1061=(( struct Cyc_Absyn_Throw_e_struct*) _temp926)->f1; goto _LL953;}
else{ goto _LL954;} _LL954: if(*(( int*) _temp926) == Cyc_Absyn_NoInstantiate_e){
_LL1064: _temp1063=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp926)->f1;
goto _LL955;} else{ goto _LL956;} _LL956: if(*(( int*) _temp926) == Cyc_Absyn_Instantiate_e){
_LL1068: _temp1067=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp926)->f1;
goto _LL1066; _LL1066: _temp1065=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp926)->f2; goto _LL957;} else{ goto _LL958;} _LL958: if(*(( int*) _temp926)
== Cyc_Absyn_Cast_e){ _LL1072: _temp1071=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp926)->f1; _temp1073=&(( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp926)->f1);
goto _LL1070; _LL1070: _temp1069=(( struct Cyc_Absyn_Cast_e_struct*) _temp926)->f2;
goto _LL959;} else{ goto _LL960;} _LL960: if(*(( int*) _temp926) == Cyc_Absyn_Address_e){
_LL1075: _temp1074=(( struct Cyc_Absyn_Address_e_struct*) _temp926)->f1; goto
_LL961;} else{ goto _LL962;} _LL962: if(*(( int*) _temp926) == Cyc_Absyn_New_e){
_LL1079: _temp1078=(( struct Cyc_Absyn_New_e_struct*) _temp926)->f1; goto
_LL1077; _LL1077: _temp1076=(( struct Cyc_Absyn_New_e_struct*) _temp926)->f2;
goto _LL963;} else{ goto _LL964;} _LL964: if(*(( int*) _temp926) == Cyc_Absyn_Sizeofexp_e){
_LL1081: _temp1080=(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp926)->f1; goto
_LL965;} else{ goto _LL966;} _LL966: if(*(( int*) _temp926) == Cyc_Absyn_Sizeoftyp_e){
_LL1083: _temp1082=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp926)->f1;
goto _LL967;} else{ goto _LL968;} _LL968: if(*(( int*) _temp926) == Cyc_Absyn_Offsetof_e){
_LL1087: _temp1086=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp926)->f1;
goto _LL1085; _LL1085: _temp1084=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp926)->f2; goto _LL969;} else{ goto _LL970;} _LL970: if(*(( int*) _temp926)
== Cyc_Absyn_Deref_e){ _LL1089: _temp1088=(( struct Cyc_Absyn_Deref_e_struct*)
_temp926)->f1; goto _LL971;} else{ goto _LL972;} _LL972: if(*(( int*) _temp926)
== Cyc_Absyn_StructMember_e){ _LL1093: _temp1092=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp926)->f1; goto _LL1091; _LL1091: _temp1090=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp926)->f2; goto _LL973;} else{ goto _LL974;} _LL974: if(*(( int*) _temp926)
== Cyc_Absyn_StructArrow_e){ _LL1097: _temp1096=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp926)->f1; goto _LL1095; _LL1095: _temp1094=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp926)->f2; goto _LL975;} else{ goto _LL976;} _LL976: if(*(( int*) _temp926)
== Cyc_Absyn_Subscript_e){ _LL1101: _temp1100=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp926)->f1; goto _LL1099; _LL1099: _temp1098=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp926)->f2; goto _LL977;} else{ goto _LL978;} _LL978: if(*(( int*) _temp926)
== Cyc_Absyn_Tuple_e){ _LL1103: _temp1102=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp926)->f1; goto _LL979;} else{ goto _LL980;} _LL980: if(*(( int*) _temp926)
== Cyc_Absyn_Array_e){ _LL1105: _temp1104=(( struct Cyc_Absyn_Array_e_struct*)
_temp926)->f1; goto _LL981;} else{ goto _LL982;} _LL982: if(*(( int*) _temp926)
== Cyc_Absyn_Comprehension_e){ _LL1111: _temp1110=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp926)->f1; goto _LL1109; _LL1109: _temp1108=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp926)->f2; goto _LL1107; _LL1107: _temp1106=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp926)->f3; goto _LL983;} else{ goto _LL984;} _LL984: if(*(( int*) _temp926)
== Cyc_Absyn_Struct_e){ _LL1119: _temp1118=(( struct Cyc_Absyn_Struct_e_struct*)
_temp926)->f1; goto _LL1117; _LL1117: _temp1116=(( struct Cyc_Absyn_Struct_e_struct*)
_temp926)->f2; goto _LL1115; _LL1115: _temp1114=(( struct Cyc_Absyn_Struct_e_struct*)
_temp926)->f3; goto _LL1113; _LL1113: _temp1112=(( struct Cyc_Absyn_Struct_e_struct*)
_temp926)->f4; goto _LL985;} else{ goto _LL986;} _LL986: if(*(( int*) _temp926)
== Cyc_Absyn_AnonStruct_e){ _LL1123: _temp1122=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp926)->f1; goto _LL1121; _LL1121: _temp1120=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp926)->f2; goto _LL987;} else{ goto _LL988;} _LL988: if(*(( int*) _temp926)
== Cyc_Absyn_Tunion_e){ _LL1129: _temp1128=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp926)->f3; if( _temp1128 == 0){ goto _LL1127;} else{ goto _LL990;} _LL1127:
_temp1126=(( struct Cyc_Absyn_Tunion_e_struct*) _temp926)->f4; goto _LL1125;
_LL1125: _temp1124=(( struct Cyc_Absyn_Tunion_e_struct*) _temp926)->f5; goto
_LL989;} else{ goto _LL990;} _LL990: if(*(( int*) _temp926) == Cyc_Absyn_Tunion_e){
_LL1135: _temp1134=(( struct Cyc_Absyn_Tunion_e_struct*) _temp926)->f3; goto
_LL1133; _LL1133: _temp1132=(( struct Cyc_Absyn_Tunion_e_struct*) _temp926)->f4;
goto _LL1131; _LL1131: _temp1130=(( struct Cyc_Absyn_Tunion_e_struct*) _temp926)->f5;
goto _LL991;} else{ goto _LL992;} _LL992: if(*(( int*) _temp926) == Cyc_Absyn_Enum_e){
goto _LL993;} else{ goto _LL994;} _LL994: if(*(( int*) _temp926) == Cyc_Absyn_Malloc_e){
_LL1139: _temp1138=(( struct Cyc_Absyn_Malloc_e_struct*) _temp926)->f1; goto
_LL1137; _LL1137: _temp1136=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp926)->f2; goto _LL995;} else{ goto _LL996;} _LL996: if(*(( int*) _temp926)
== Cyc_Absyn_StmtExp_e){ _LL1141: _temp1140=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp926)->f1; goto _LL997;} else{ goto _LL998;} _LL998: if(*(( int*) _temp926)
== Cyc_Absyn_UnresolvedMem_e){ goto _LL999;} else{ goto _LL1000;} _LL1000: if(*((
int*) _temp926) == Cyc_Absyn_CompoundLit_e){ goto _LL1001;} else{ goto _LL1002;}
_LL1002: if(*(( int*) _temp926) == Cyc_Absyn_Codegen_e){ _LL1143: _temp1142=((
struct Cyc_Absyn_Codegen_e_struct*) _temp926)->f1; goto _LL1003;} else{ goto
_LL1004;} _LL1004: if(*(( int*) _temp926) == Cyc_Absyn_Fill_e){ goto _LL1005;}
else{ goto _LL927;} _LL929: { struct Cyc_Absyn_Exp* _temp1144= Cyc_Absyn_uint_exp(
0, 0); if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){ if( nv->toplevel){( void*)(
e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr( old_typ, _temp1144,
_temp1144))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_List_List*(* _temp1145)( struct _tagged_arr)=(
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp*
_temp1148= _temp1144; struct Cyc_Absyn_Exp* _temp1149= _temp1144; struct Cyc_Absyn_Exp*
_temp1150= _temp1144; struct Cyc_Absyn_Exp* _temp1146[ 3u]={ _temp1148,
_temp1149, _temp1150}; struct _tagged_arr _temp1147={( void*) _temp1146,( void*)
_temp1146,( void*)( _temp1146 + 3u)}; _temp1145( _temp1147);}), 0))->r));}}
else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp( 0, 0))->r));}
goto _LL927;} _LL931: goto _LL927; _LL933:{ struct _handler_cons _temp1151;
_push_handler(& _temp1151);{ int _temp1153= 0; if( setjmp( _temp1151.handler)){
_temp1153= 1;} if( ! _temp1153){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( nv->varmap,
_temp1010))->r));; _pop_handler();} else{ void* _temp1152=( void*) _exn_thrown;
void* _temp1155= _temp1152; _LL1157: if( _temp1155 == Cyc_Dict_Absent){ goto
_LL1158;} else{ goto _LL1159;} _LL1159: goto _LL1160; _LL1158:({ int(* _temp1161)(
struct _tagged_arr fmt, struct _tagged_arr ap)=( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos; struct _tagged_arr _temp1165=
_tag_arr("Can't find %s in exp_to_c, Var\n", sizeof( unsigned char), 32u);
struct Cyc_Stdio_String_pa_struct _temp1166; _temp1166.tag= Cyc_Stdio_String_pa;
_temp1166.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp1010);{ void*
_temp1164=( void*)& _temp1166; void* _temp1162[ 1u]={ _temp1164}; struct
_tagged_arr _temp1163={( void*) _temp1162,( void*) _temp1162,( void*)( _temp1162
+ 1u)}; _temp1161( _temp1165, _temp1163);}}); return; _LL1160:( void) _throw(
_temp1155); _LL1156:;}}} goto _LL927; _LL935:({ int(* _temp1167)( struct
_tagged_arr fmt, struct _tagged_arr ap)=( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos; struct _tagged_arr _temp1170= _tag_arr("unknownid",
sizeof( unsigned char), 10u); void* _temp1168[ 0u]={}; struct _tagged_arr
_temp1169={( void*) _temp1168,( void*) _temp1168,( void*)( _temp1168 + 0u)};
_temp1167( _temp1170, _temp1169);}); goto _LL927; _LL937: { struct Cyc_List_List*
_temp1171=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ, _temp1014); struct
Cyc_List_List* _temp1172=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ, _temp1014);(( void(*)(
void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1014);{
void* _temp1173= _temp1016; _LL1175: if( _temp1173 ==( void*) Cyc_Absyn_Size){
goto _LL1176;} else{ goto _LL1177;} _LL1177: if( _temp1173 ==( void*) Cyc_Absyn_Plus){
goto _LL1178;} else{ goto _LL1179;} _LL1179: if( _temp1173 ==( void*) Cyc_Absyn_Minus){
goto _LL1180;} else{ goto _LL1181;} _LL1181: if( _temp1173 ==( void*) Cyc_Absyn_Eq){
goto _LL1182;} else{ goto _LL1183;} _LL1183: if( _temp1173 ==( void*) Cyc_Absyn_Neq){
goto _LL1184;} else{ goto _LL1185;} _LL1185: if( _temp1173 ==( void*) Cyc_Absyn_Gt){
goto _LL1186;} else{ goto _LL1187;} _LL1187: if( _temp1173 ==( void*) Cyc_Absyn_Gte){
goto _LL1188;} else{ goto _LL1189;} _LL1189: if( _temp1173 ==( void*) Cyc_Absyn_Lt){
goto _LL1190;} else{ goto _LL1191;} _LL1191: if( _temp1173 ==( void*) Cyc_Absyn_Lte){
goto _LL1192;} else{ goto _LL1193;} _LL1193: goto _LL1194; _LL1176: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1014))->hd;{
void* _temp1195=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v;
struct Cyc_Absyn_Exp* _temp1203; struct Cyc_Absyn_PtrInfo _temp1205; struct Cyc_Absyn_Conref*
_temp1207; void* _temp1209; _LL1197: if(( unsigned int) _temp1195 > 4u?*(( int*)
_temp1195) == Cyc_Absyn_ArrayType: 0){ _LL1204: _temp1203=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1195)->f3; goto _LL1198;} else{ goto _LL1199;} _LL1199: if(( unsigned int)
_temp1195 > 4u?*(( int*) _temp1195) == Cyc_Absyn_PointerType: 0){ _LL1206:
_temp1205=(( struct Cyc_Absyn_PointerType_struct*) _temp1195)->f1; _LL1210:
_temp1209=( void*) _temp1205.elt_typ; goto _LL1208; _LL1208: _temp1207=
_temp1205.bounds; goto _LL1200;} else{ goto _LL1201;} _LL1201: goto _LL1202;
_LL1198:( void*)( e->r=( void*)(( void*)(( struct Cyc_Absyn_Exp*) _check_null(
_temp1203))->r)); goto _LL1196; _LL1200:{ void* _temp1211= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1207); struct Cyc_Absyn_Exp* _temp1217; _LL1213: if(
_temp1211 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1214;} else{ goto _LL1215;}
_LL1215: if(( unsigned int) _temp1211 > 1u?*(( int*) _temp1211) == Cyc_Absyn_Upper_b:
0){ _LL1218: _temp1217=(( struct Cyc_Absyn_Upper_b_struct*) _temp1211)->f1; goto
_LL1216;} else{ goto _LL1212;} _LL1214:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__get_arr_size_e,({ struct Cyc_List_List*(* _temp1219)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1222=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1014))->hd; struct Cyc_Absyn_Exp* _temp1223= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( _temp1209), 0); struct Cyc_Absyn_Exp* _temp1220[ 2u]={
_temp1222, _temp1223}; struct _tagged_arr _temp1221={( void*) _temp1220,( void*)
_temp1220,( void*)( _temp1220 + 2u)}; _temp1219( _temp1221);}), 0))->r)); goto
_LL1212; _LL1216:( void*)( e->r=( void*)(( void*) _temp1217->r)); goto _LL1212;
_LL1212:;} goto _LL1196; _LL1202:({ int(* _temp1224)( struct _tagged_arr fmt,
struct _tagged_arr ap)=( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos; struct _tagged_arr _temp1229= _tag_arr("size primop applied to non-array %s (%s)",
sizeof( unsigned char), 41u); struct Cyc_Stdio_String_pa_struct _temp1231;
_temp1231.tag= Cyc_Stdio_String_pa; _temp1231.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_List_List*) _check_null( _temp1172))->hd);{ void* _temp1227=(
void*)& _temp1231; struct Cyc_Stdio_String_pa_struct _temp1230; _temp1230.tag=
Cyc_Stdio_String_pa; _temp1230.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v);{ void* _temp1228=(
void*)& _temp1230; void* _temp1225[ 2u]={ _temp1227, _temp1228}; struct
_tagged_arr _temp1226={( void*) _temp1225,( void*) _temp1225,( void*)( _temp1225
+ 2u)}; _temp1224( _temp1229, _temp1226);}}}); return; _LL1196:;} goto _LL1174;}
_LL1178: { void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt((
void*)(( struct Cyc_List_List*) _check_null( _temp1171))->hd,& elt_typ)){ struct
Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp1014))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1014))->tl))->hd;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__tagged_arr_plus_e,({
struct Cyc_List_List*(* _temp1232)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1235= e1; struct
Cyc_Absyn_Exp* _temp1236= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0);
struct Cyc_Absyn_Exp* _temp1237= e2; struct Cyc_Absyn_Exp* _temp1233[ 3u]={
_temp1235, _temp1236, _temp1237}; struct _tagged_arr _temp1234={( void*)
_temp1233,( void*) _temp1233,( void*)( _temp1233 + 3u)}; _temp1232( _temp1234);}),
0))->r));} goto _LL1174;} _LL1180: { void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt(( void*)(( struct Cyc_List_List*)
_check_null( _temp1171))->hd,& elt_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1014))->hd; struct Cyc_Absyn_Exp* e2=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1014))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(( void*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1171))->tl))->hd)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0),
Cyc_Toc_curr_sp, 0))->r));( void*)( e->r=( void*)(( void*)( Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Div, Cyc_Absyn_copy_exp( e), Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(
elt_typ), 0), 0))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_plus_e,({ struct Cyc_List_List*(* _temp1238)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1241= e1; struct Cyc_Absyn_Exp* _temp1242= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( elt_typ), 0); struct Cyc_Absyn_Exp* _temp1243= Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Minus, e2, 0); struct Cyc_Absyn_Exp* _temp1239[ 3u]={ _temp1241,
_temp1242, _temp1243}; struct _tagged_arr _temp1240={( void*) _temp1239,( void*)
_temp1239,( void*)( _temp1239 + 3u)}; _temp1238( _temp1240);}), 0))->r));}} goto
_LL1174;} _LL1182: goto _LL1184; _LL1184: goto _LL1186; _LL1186: goto _LL1188;
_LL1188: goto _LL1190; _LL1190: goto _LL1192; _LL1192: { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1014))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1014))->tl))->hd; void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp1171))->hd; void* t2=(
void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1171))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){(
void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1174;} _LL1194: goto
_LL1174; _LL1174:;} goto _LL927;} _LL939: { void* e2_cyc_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1020->topt))->v; Cyc_Toc_exp_to_c( nv,
_temp1020);{ void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){ struct Cyc_Absyn_Exp* _temp1244=( _temp1018 ==( void*) Cyc_Absyn_PostInc?
1: _temp1018 ==( void*) Cyc_Absyn_PostDec)? Cyc_Toc__tagged_arr_inplace_plus_post_e:
Cyc_Toc__tagged_arr_inplace_plus_e; int _temp1245=( _temp1018 ==( void*) Cyc_Absyn_PreInc?
1: _temp1018 ==( void*) Cyc_Absyn_PostInc)? 1: - 1;( void*)( e->r=( void*)((
void*)( Cyc_Absyn_fncall_exp( _temp1244,({ struct Cyc_List_List*(* _temp1246)(
struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1249= Cyc_Absyn_address_exp( _temp1020, 0); struct
Cyc_Absyn_Exp* _temp1250= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0);
struct Cyc_Absyn_Exp* _temp1251= Cyc_Absyn_signed_int_exp( _temp1245, 0); struct
Cyc_Absyn_Exp* _temp1247[ 3u]={ _temp1249, _temp1250, _temp1251}; struct
_tagged_arr _temp1248={( void*) _temp1247,( void*) _temp1247,( void*)( _temp1247
+ 3u)}; _temp1246( _temp1248);}), 0))->r));} if( elt_typ ==( void*) Cyc_Absyn_VoidType?
! Cyc_Absyn_is_lvalue( _temp1020): 0){ Cyc_Toc_lvalue_assign( _temp1020, 0, Cyc_Toc_incr_lvalue,
_temp1018);( void*)( e->r=( void*)(( void*) _temp1020->r));} goto _LL927;}}
_LL941: { int e1_poly= Cyc_Toc_is_poly_project( _temp1026); void* e1_old_typ=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1026->topt))->v; void*
e2_old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1022->topt))->v;
Cyc_Toc_exp_to_c( nv, _temp1026); Cyc_Toc_exp_to_c( nv, _temp1022);{ int done= 0;
if( _temp1024 != 0){ void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){ struct Cyc_Absyn_Exp* change;{ void* _temp1252=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1024))->v; _LL1254: if( _temp1252 ==(
void*) Cyc_Absyn_Plus){ goto _LL1255;} else{ goto _LL1256;} _LL1256: if(
_temp1252 ==( void*) Cyc_Absyn_Minus){ goto _LL1257;} else{ goto _LL1258;}
_LL1258: goto _LL1259; _LL1255: change= _temp1022; goto _LL1253; _LL1257: change=
Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Minus, _temp1022, 0); goto _LL1253;
_LL1259:( int) _throw(({ void*(* _temp1260)( struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_impos; struct _tagged_arr _temp1263= _tag_arr("bad t ? pointer arithmetic",
sizeof( unsigned char), 27u); void* _temp1261[ 0u]={}; struct _tagged_arr
_temp1262={( void*) _temp1261,( void*) _temp1261,( void*)( _temp1261 + 0u)};
_temp1260( _temp1263, _temp1262);})); goto _LL1253; _LL1253:;} done= 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__tagged_arr_inplace_plus_e,({
struct Cyc_List_List*(* _temp1264)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1267= Cyc_Absyn_address_exp(
_temp1026, 0); struct Cyc_Absyn_Exp* _temp1268= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(
elt_typ), 0); struct Cyc_Absyn_Exp* _temp1269= change; struct Cyc_Absyn_Exp*
_temp1265[ 3u]={ _temp1267, _temp1268, _temp1269}; struct _tagged_arr _temp1266={(
void*) _temp1265,( void*) _temp1265,( void*)( _temp1265 + 3u)}; _temp1264(
_temp1266);}), 0))->r));}} if( ! done){ if( e1_poly){( void*)( _temp1022->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp((
void*) _temp1022->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue( _temp1026)){((
void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Absyn_Exp*, struct _tuple7*), struct _tuple7* f_env)) Cyc_Toc_lvalue_assign)(
_temp1026, 0, Cyc_Toc_assignop_lvalue,({ struct _tuple7* _temp1270=( struct
_tuple7*) GC_malloc( sizeof( struct _tuple7) * 1); _temp1270[ 0]=({ struct
_tuple7 _temp1271; _temp1271.f1= _temp1024; _temp1271.f2= _temp1022; _temp1271;});
_temp1270;}));( void*)( e->r=( void*)(( void*) _temp1026->r));}} goto _LL927;}}
_LL943: Cyc_Toc_exp_to_c( nv, _temp1032); Cyc_Toc_exp_to_c( nv, _temp1030); Cyc_Toc_exp_to_c(
nv, _temp1028); goto _LL927; _LL945: Cyc_Toc_exp_to_c( nv, _temp1036); Cyc_Toc_exp_to_c(
nv, _temp1034); goto _LL927; _LL947: _temp1046= _temp1040; _temp1044= _temp1038;
goto _LL949; _LL949: Cyc_Toc_exp_to_c( nv, _temp1046);(( void(*)( void(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1044); goto _LL927; _LL951:{
struct _RegionHandle _temp1272= _new_region(); struct _RegionHandle* r=&
_temp1272; _push_region( r);{ void* _temp1273=( void*) _temp1051->type; void*
_temp1274= Cyc_Toc_typ_to_c( _temp1273); void* _temp1275= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1059->topt))->v); struct Cyc_Absyn_Exp*
_temp1276= Cyc_Absyn_uint_exp(( unsigned int) _temp1055, 0); void* arr_type= Cyc_Toc_typ_to_c((
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp1325=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1325[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1326; _temp1326.tag= Cyc_Absyn_ArrayType;
_temp1326.f1=( void*) _temp1273; _temp1326.f2= Cyc_Toc_mt_tq; _temp1326.f3=(
struct Cyc_Absyn_Exp*) _temp1276; _temp1326;}); _temp1325;})); struct _tuple0*
_temp1277= Cyc_Toc_temp_var(); struct _tuple0* _temp1278= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* _temp1279= Cyc_Absyn_var_exp( _temp1278, 0); struct
_tuple0* _temp1280= Cyc_Toc_temp_var(); int _temp1281=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp1057); int _temp1282= _temp1281 - _temp1055; struct
_tagged_arr vs=({ unsigned int _temp1321=( unsigned int) _temp1055; struct
_tuple0** _temp1322=( struct _tuple0**) _region_malloc( r, sizeof( struct
_tuple0*) * _temp1321); struct _tagged_arr _temp1324= _tag_arr( _temp1322,
sizeof( struct _tuple0*),( unsigned int) _temp1055);{ unsigned int _temp1323=
_temp1321; unsigned int i; for( i= 0; i < _temp1323; i ++){ _temp1322[ i]= Cyc_Toc_temp_var();}};
_temp1324;}); struct _tagged_arr xs=({ unsigned int _temp1317=( unsigned int)
_temp1282; struct _tuple0** _temp1318=( struct _tuple0**) _region_malloc( r,
sizeof( struct _tuple0*) * _temp1317); struct _tagged_arr _temp1320= _tag_arr(
_temp1318, sizeof( struct _tuple0*),( unsigned int) _temp1282);{ unsigned int
_temp1319= _temp1317; unsigned int i; for( i= 0; i < _temp1319; i ++){ _temp1318[
i]= Cyc_Toc_temp_var();}}; _temp1320;}); struct Cyc_List_List* xexps=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple0*, struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_Toc_map_2c)( Cyc_Absyn_var_exp,(
struct Cyc_Position_Segment*) 0,(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, struct _tagged_arr arr)) Cyc_List_rfrom_array)( r, xs));
xexps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( xexps,({ struct Cyc_List_List* _temp1283=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1283->hd=( void*) Cyc_Absyn_var_exp(
_temp1280, 0); _temp1283->tl= 0; _temp1283;}));{ struct Cyc_Absyn_Stmt*
_temp1284= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(
_temp1277, 0), xexps, 0), 0); struct Cyc_Absyn_Exp* _temp1285= Cyc_Absyn_add_exp(
_temp1279, _temp1276, 0); struct Cyc_List_List* _temp1286=({ struct Cyc_List_List*(*
_temp1308)( struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list; struct _tuple4* _temp1311=({ struct _tuple4* _temp1316=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1316->f1= 0; _temp1316->f2=
Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), _temp1279, 0); _temp1316;});
struct _tuple4* _temp1312=({ struct _tuple4* _temp1315=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp1315->f1= 0; _temp1315->f2= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp1279, 0); _temp1315;}); struct _tuple4*
_temp1313=({ struct _tuple4* _temp1314=( struct _tuple4*) GC_malloc( sizeof(
struct _tuple4)); _temp1314->f1= 0; _temp1314->f2= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1285, 0); _temp1314;}); struct _tuple4* _temp1309[ 3u]={ _temp1311,
_temp1312, _temp1313}; struct _tagged_arr _temp1310={( void*) _temp1309,( void*)
_temp1309,( void*)( _temp1309 + 3u)}; _temp1308( _temp1310);}); _temp1284= Cyc_Absyn_declare_stmt(
_temp1280, Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ( _temp1273,( void*) _temp1051->rgn,
Cyc_Toc_mt_tq)),( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp( 0,
_temp1286, 0), _temp1284, 0);{ struct Cyc_List_List* _temp1287=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct Cyc_Absyn_Exp*(* f)( struct _tuple0*, struct
Cyc_Position_Segment*), struct Cyc_Position_Segment* env, struct Cyc_List_List*
x)) Cyc_Toc_rmap_2c)( r, Cyc_Absyn_var_exp, 0,(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct _tagged_arr arr)) Cyc_List_rfrom_array)( r, vs));
struct Cyc_List_List* _temp1288=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_make_dle,
_temp1287); _temp1284= Cyc_Absyn_declare_stmt( _temp1278, arr_type,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_unresolvedmem_exp( 0, _temp1288, 0), _temp1284, 0);{ struct Cyc_List_List*
_temp1289=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( r, _temp1057); struct Cyc_List_List* _temp1290=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, _temp1053);{
int i= _temp1055 - 1; for( 0; i >= 0;( i --, _temp1289=(( struct Cyc_List_List*)
_check_null( _temp1289))->tl)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1289))->hd; void* arg_type=( void*)((
struct Cyc_Core_Opt*) _check_null( arg->topt))->v; Cyc_Toc_exp_to_c( nv, arg);
if( _temp1051->inject){ void* _temp1293; struct Cyc_Position_Segment* _temp1295;
struct Cyc_List_List* _temp1297; struct Cyc_List_List* _temp1299; struct _tuple0*
_temp1301; struct Cyc_Absyn_Tunionfield _temp1291=*(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1290))->hd); _LL1302: _temp1301=
_temp1291.name; goto _LL1300; _LL1300: _temp1299= _temp1291.tvs; goto _LL1298;
_LL1298: _temp1297= _temp1291.typs; goto _LL1296; _LL1296: _temp1295= _temp1291.loc;
goto _LL1294; _LL1294: _temp1293=( void*) _temp1291.sc; goto _LL1292; _LL1292:
_temp1290=(( struct Cyc_List_List*) _check_null( _temp1290))->tl;{ struct
_tuple0* _temp1303= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp1304= Cyc_Absyn_var_exp(
_temp1303, 0); void* _temp1305= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp1301, _tag_arr("_struct", sizeof( unsigned char), 8u))); _temp1284= Cyc_Absyn_declare_stmt(*((
struct _tuple0**) _check_unknown_subscript( vs, sizeof( struct _tuple0*), i)),
_temp1274,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( _temp1274, Cyc_Absyn_address_exp(
_temp1304, 0), 0), _temp1284, 0);{ void* field_typ= Cyc_Toc_typ_to_c((*(( struct
_tuple3*)(( struct Cyc_List_List*) _check_null( _temp1297))->hd)).f2); if( Cyc_Toc_is_void_star(
field_typ)){ arg= Cyc_Absyn_cast_exp( field_typ, arg, 0);}{ struct Cyc_Absyn_Stmt*
_temp1306= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1304, Cyc_Toc_fieldname(
1), 0), arg, 0); _temp1284= Cyc_Absyn_seq_stmt( _temp1306, _temp1284, 0);{
struct Cyc_Absyn_Stmt* _temp1307= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
_temp1304, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( _temp1301, 0), 0); _temp1284=
Cyc_Absyn_seq_stmt( _temp1307, _temp1284, 0); _temp1284= Cyc_Absyn_declare_stmt(
_temp1303, _temp1305, 0, _temp1284, 0);}}}}} else{ _temp1284= Cyc_Absyn_declare_stmt(*((
struct _tuple0**) _check_unknown_subscript( vs, sizeof( struct _tuple0*), i)),
_temp1274,( struct Cyc_Absyn_Exp*) arg, _temp1284, 0);}}}{ int i= _temp1282 - 1;
for( 0; i >= 0;( i --, _temp1289=(( struct Cyc_List_List*) _check_null(
_temp1289))->tl)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1289))->hd; void* argtype= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v); Cyc_Toc_exp_to_c(
nv, arg); _temp1284= Cyc_Absyn_declare_stmt(*(( struct _tuple0**)
_check_unknown_subscript( xs, sizeof( struct _tuple0*), i)), argtype,( struct
Cyc_Absyn_Exp*) arg, _temp1284, 0);}} Cyc_Toc_exp_to_c( nv, _temp1059);
_temp1284= Cyc_Absyn_declare_stmt( _temp1277, _temp1275,( struct Cyc_Absyn_Exp*)
_temp1059, _temp1284, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1284, 0))->r));}}}}; _pop_region( r);} goto _LL927; _LL953: Cyc_Toc_exp_to_c(
nv, _temp1061);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(
old_typ), Cyc_Toc_newthrow_exp( _temp1061), 0))->r)); goto _LL927; _LL955: Cyc_Toc_exp_to_c(
nv, _temp1063); goto _LL927; _LL957: Cyc_Toc_exp_to_c( nv, _temp1067); for( 0;
_temp1065 != 0; _temp1065=(( struct Cyc_List_List*) _check_null( _temp1065))->tl){
void* k= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*) _check_null(
_temp1065))->hd); if( k !=( void*) Cyc_Absyn_EffKind? k !=( void*) Cyc_Absyn_RgnKind:
0){{ void* _temp1327= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*)
_check_null( _temp1065))->hd); _LL1329: if(( unsigned int) _temp1327 > 4u?*((
int*) _temp1327) == Cyc_Absyn_VarType: 0){ goto _LL1330;} else{ goto _LL1331;}
_LL1331: if(( unsigned int) _temp1327 > 4u?*(( int*) _temp1327) == Cyc_Absyn_TunionType:
0){ goto _LL1332;} else{ goto _LL1333;} _LL1333: goto _LL1334; _LL1330:
continue; _LL1332: continue; _LL1334:( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp1067, 0))->r)); goto _LL1328; _LL1328:;} break;}} goto _LL927; _LL959: {
void* old_t2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1069->topt))->v;
void* new_typ=* _temp1073;* _temp1073= Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c(
nv, _temp1069);{ struct _tuple8 _temp1336=({ struct _tuple8 _temp1335; _temp1335.f1=
Cyc_Tcutil_compress( old_t2); _temp1335.f2= Cyc_Tcutil_compress( new_typ);
_temp1335;}); void* _temp1344; struct Cyc_Absyn_PtrInfo _temp1346; void*
_temp1348; struct Cyc_Absyn_PtrInfo _temp1350; void* _temp1352; void* _temp1354;
struct Cyc_Absyn_PtrInfo _temp1356; _LL1338: _LL1349: _temp1348= _temp1336.f1;
if(( unsigned int) _temp1348 > 4u?*(( int*) _temp1348) == Cyc_Absyn_PointerType:
0){ _LL1351: _temp1350=(( struct Cyc_Absyn_PointerType_struct*) _temp1348)->f1;
goto _LL1345;} else{ goto _LL1340;} _LL1345: _temp1344= _temp1336.f2; if((
unsigned int) _temp1344 > 4u?*(( int*) _temp1344) == Cyc_Absyn_PointerType: 0){
_LL1347: _temp1346=(( struct Cyc_Absyn_PointerType_struct*) _temp1344)->f1; goto
_LL1339;} else{ goto _LL1340;} _LL1340: _LL1355: _temp1354= _temp1336.f1; if((
unsigned int) _temp1354 > 4u?*(( int*) _temp1354) == Cyc_Absyn_PointerType: 0){
_LL1357: _temp1356=(( struct Cyc_Absyn_PointerType_struct*) _temp1354)->f1; goto
_LL1353;} else{ goto _LL1342;} _LL1353: _temp1352= _temp1336.f2; if((
unsigned int) _temp1352 > 4u?*(( int*) _temp1352) == Cyc_Absyn_IntType: 0){ goto
_LL1341;} else{ goto _LL1342;} _LL1342: goto _LL1343; _LL1339: { int _temp1358=((
int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0, _temp1350.nullable);
int _temp1359=(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
0, _temp1346.nullable); void* _temp1360= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1350.bounds); void* _temp1361= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1346.bounds);{ struct _tuple8 _temp1363=({ struct _tuple8 _temp1362;
_temp1362.f1= _temp1360; _temp1362.f2= _temp1361; _temp1362;}); void* _temp1373;
void* _temp1375; void* _temp1377; void* _temp1379; struct Cyc_Absyn_Exp*
_temp1381; void* _temp1383; struct Cyc_Absyn_Exp* _temp1385; void* _temp1387;
_LL1365: _LL1376: _temp1375= _temp1363.f1; if(( unsigned int) _temp1375 > 1u?*((
int*) _temp1375) == Cyc_Absyn_Upper_b: 0){ goto _LL1374;} else{ goto _LL1367;}
_LL1374: _temp1373= _temp1363.f2; if(( unsigned int) _temp1373 > 1u?*(( int*)
_temp1373) == Cyc_Absyn_Upper_b: 0){ goto _LL1366;} else{ goto _LL1367;} _LL1367:
_LL1380: _temp1379= _temp1363.f1; if(( unsigned int) _temp1379 > 1u?*(( int*)
_temp1379) == Cyc_Absyn_Upper_b: 0){ _LL1382: _temp1381=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1379)->f1; goto _LL1378;} else{ goto _LL1369;} _LL1378: _temp1377=
_temp1363.f2; if( _temp1377 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1368;}
else{ goto _LL1369;} _LL1369: _LL1388: _temp1387= _temp1363.f1; if( _temp1387 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL1384;} else{ goto _LL1371;} _LL1384:
_temp1383= _temp1363.f2; if(( unsigned int) _temp1383 > 1u?*(( int*) _temp1383)
== Cyc_Absyn_Upper_b: 0){ _LL1386: _temp1385=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1383)->f1; goto _LL1370;} else{ goto _LL1371;} _LL1371: goto _LL1372;
_LL1366: if( _temp1358? ! _temp1359: 0){ if( nv->toplevel){({ int(* _temp1389)(
struct _tagged_arr fmt, struct _tagged_arr ap)=( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_unimp; struct _tagged_arr _temp1392= _tag_arr("can't do null-check conversion at top-level",
sizeof( unsigned char), 44u); void* _temp1390[ 0u]={}; struct _tagged_arr
_temp1391={( void*) _temp1390,( void*) _temp1390,( void*)( _temp1390 + 0u)};
_temp1389( _temp1392, _temp1391);});}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(*
_temp1073, Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({ struct Cyc_List_List*
_temp1393=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1393->hd=( void*) _temp1069; _temp1393->tl= 0; _temp1393;}), 0), 0))->r));}
goto _LL1364; _LL1368: if( nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr(
old_t2, _temp1381, _temp1069))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_List_List*(* _temp1394)( struct _tagged_arr)=(
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp*
_temp1397= _temp1069; struct Cyc_Absyn_Exp* _temp1398= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(( void*) _temp1346.elt_typ), 0); struct Cyc_Absyn_Exp*
_temp1399= _temp1381; struct Cyc_Absyn_Exp* _temp1395[ 3u]={ _temp1397,
_temp1398, _temp1399}; struct _tagged_arr _temp1396={( void*) _temp1395,( void*)
_temp1395,( void*)( _temp1395 + 3u)}; _temp1394( _temp1396);}), 0))->r));} goto
_LL1364; _LL1370: if( nv->toplevel){({ int(* _temp1400)( struct _tagged_arr fmt,
struct _tagged_arr ap)=( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_unimp; struct _tagged_arr _temp1403= _tag_arr("can't coerce t? to t* or t@ at the top-level",
sizeof( unsigned char), 45u); void* _temp1401[ 0u]={}; struct _tagged_arr
_temp1402={( void*) _temp1401,( void*) _temp1401,( void*)( _temp1401 + 0u)};
_temp1400( _temp1403, _temp1402);});}{ struct Cyc_Absyn_Exp* _temp1404= Cyc_Absyn_fncall_exp(
Cyc_Toc__untag_arr_e,({ struct Cyc_List_List*(* _temp1406)( struct _tagged_arr)=(
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp*
_temp1409= _temp1069; struct Cyc_Absyn_Exp* _temp1410= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(( void*) _temp1350.elt_typ), 0); struct Cyc_Absyn_Exp*
_temp1411= _temp1385; struct Cyc_Absyn_Exp* _temp1407[ 3u]={ _temp1409,
_temp1410, _temp1411}; struct _tagged_arr _temp1408={( void*) _temp1407,( void*)
_temp1407,( void*)( _temp1407 + 3u)}; _temp1406( _temp1408);}), 0); if(
_temp1359){( void*)( _temp1404->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({
struct Cyc_List_List* _temp1405=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1405->hd=( void*) Cyc_Absyn_copy_exp( _temp1404);
_temp1405->tl= 0; _temp1405;}), 0))->r));}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(*
_temp1073, _temp1404, 0))->r)); goto _LL1364;} _LL1372: goto _LL1364; _LL1364:;}
goto _LL1337;} _LL1341:{ void* _temp1412= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1356.bounds); _LL1414: if( _temp1412 ==( void*) Cyc_Absyn_Unknown_b){ goto
_LL1415;} else{ goto _LL1416;} _LL1416: goto _LL1417; _LL1415:( void*)(
_temp1069->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) _temp1069->r, _temp1069->loc), Cyc_Toc_curr_sp, 0))->r)); goto _LL1413;
_LL1417: goto _LL1413; _LL1413:;} goto _LL1337; _LL1343: goto _LL1337; _LL1337:;}
goto _LL927;} _LL961:{ void* _temp1418=( void*) _temp1074->r; struct Cyc_List_List*
_temp1426; struct _tuple0* _temp1428; struct Cyc_List_List* _temp1430; _LL1420:
if(*(( int*) _temp1418) == Cyc_Absyn_Struct_e){ _LL1429: _temp1428=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1418)->f1; goto _LL1427; _LL1427: _temp1426=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1418)->f3; goto _LL1421;} else{ goto _LL1422;} _LL1422: if(*(( int*)
_temp1418) == Cyc_Absyn_Tuple_e){ _LL1431: _temp1430=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1418)->f1; goto _LL1423;} else{ goto _LL1424;} _LL1424: goto _LL1425;
_LL1421: if( nv->toplevel){({ int(* _temp1432)( struct _tagged_arr fmt, struct
_tagged_arr ap)=( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp;
struct _tagged_arr _temp1436= _tag_arr("%s: & on non-identifiers at the top-level",
sizeof( unsigned char), 42u); struct Cyc_Stdio_String_pa_struct _temp1437;
_temp1437.tag= Cyc_Stdio_String_pa; _temp1437.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp1074->loc);{ void* _temp1435=( void*)& _temp1437; void* _temp1433[ 1u]={
_temp1435}; struct _tagged_arr _temp1434={( void*) _temp1433,( void*) _temp1433,(
void*)( _temp1433 + 1u)}; _temp1432( _temp1436, _temp1434);}});}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1074->topt))->v, 1, 0, _temp1426, _temp1428))->r)); goto
_LL1419; _LL1423: if( nv->toplevel){({ int(* _temp1438)( struct _tagged_arr fmt,
struct _tagged_arr ap)=( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_unimp; struct _tagged_arr _temp1442= _tag_arr("%s: & on non-identifiers at the top-level",
sizeof( unsigned char), 42u); struct Cyc_Stdio_String_pa_struct _temp1443;
_temp1443.tag= Cyc_Stdio_String_pa; _temp1443.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp1074->loc);{ void* _temp1441=( void*)& _temp1443; void* _temp1439[ 1u]={
_temp1441}; struct _tagged_arr _temp1440={( void*) _temp1439,( void*) _temp1439,(
void*)( _temp1439 + 1u)}; _temp1438( _temp1442, _temp1440);}});}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1430))->r)); goto _LL1419;
_LL1425: Cyc_Toc_exp_to_c( nv, _temp1074); if( ! Cyc_Absyn_is_lvalue( _temp1074)){((
void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1074,
0, Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp1074, 0))->r));} goto _LL1419; _LL1419:;} goto _LL927; _LL963: if( nv->toplevel){({
int(* _temp1444)( struct _tagged_arr fmt, struct _tagged_arr ap)=( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp; struct
_tagged_arr _temp1448= _tag_arr("%s: new at top-level", sizeof( unsigned char),
21u); struct Cyc_Stdio_String_pa_struct _temp1449; _temp1449.tag= Cyc_Stdio_String_pa;
_temp1449.f1=( struct _tagged_arr) Cyc_Position_string_of_segment( _temp1076->loc);{
void* _temp1447=( void*)& _temp1449; void* _temp1445[ 1u]={ _temp1447}; struct
_tagged_arr _temp1446={( void*) _temp1445,( void*) _temp1445,( void*)( _temp1445
+ 1u)}; _temp1444( _temp1448, _temp1446);}});}{ void* _temp1450=( void*)
_temp1076->r; struct Cyc_List_List* _temp1462; struct Cyc_Absyn_Exp* _temp1464;
struct Cyc_Absyn_Exp* _temp1466; struct Cyc_Absyn_Vardecl* _temp1468; struct Cyc_List_List*
_temp1470; struct _tuple0* _temp1472; struct Cyc_List_List* _temp1474; _LL1452:
if(*(( int*) _temp1450) == Cyc_Absyn_Array_e){ _LL1463: _temp1462=(( struct Cyc_Absyn_Array_e_struct*)
_temp1450)->f1; goto _LL1453;} else{ goto _LL1454;} _LL1454: if(*(( int*)
_temp1450) == Cyc_Absyn_Comprehension_e){ _LL1469: _temp1468=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1450)->f1; goto _LL1467; _LL1467: _temp1466=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1450)->f2; goto _LL1465; _LL1465: _temp1464=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1450)->f3; goto _LL1455;} else{ goto _LL1456;} _LL1456: if(*(( int*)
_temp1450) == Cyc_Absyn_Struct_e){ _LL1473: _temp1472=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1450)->f1; goto _LL1471; _LL1471: _temp1470=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1450)->f3; goto _LL1457;} else{ goto _LL1458;} _LL1458: if(*(( int*)
_temp1450) == Cyc_Absyn_Tuple_e){ _LL1475: _temp1474=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1450)->f1; goto _LL1459;} else{ goto _LL1460;} _LL1460: goto _LL1461;
_LL1453: { struct _tuple0* _temp1476= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp1477= Cyc_Absyn_var_exp( _temp1476, 0); struct Cyc_Absyn_Stmt* _temp1478=
Cyc_Toc_init_array( nv, _temp1477, _temp1462, Cyc_Absyn_exp_stmt( _temp1477, 0));
void* old_elt_typ;{ void* _temp1479= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1485; struct Cyc_Absyn_Tqual _temp1487; void* _temp1489; _LL1481: if((
unsigned int) _temp1479 > 4u?*(( int*) _temp1479) == Cyc_Absyn_PointerType: 0){
_LL1486: _temp1485=(( struct Cyc_Absyn_PointerType_struct*) _temp1479)->f1;
_LL1490: _temp1489=( void*) _temp1485.elt_typ; goto _LL1488; _LL1488: _temp1487=
_temp1485.tq; goto _LL1482;} else{ goto _LL1483;} _LL1483: goto _LL1484; _LL1482:
old_elt_typ= _temp1489; goto _LL1480; _LL1484: old_elt_typ=({ void*(* _temp1491)(
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Toc_toc_impos; struct
_tagged_arr _temp1494= _tag_arr("exp_to_c:new array expression doesn't have ptr type",
sizeof( unsigned char), 52u); void* _temp1492[ 0u]={}; struct _tagged_arr
_temp1493={( void*) _temp1492,( void*) _temp1492,( void*)( _temp1492 + 0u)};
_temp1491( _temp1494, _temp1493);}); goto _LL1480; _LL1480:;}{ void* elt_typ=
Cyc_Toc_typ_to_c( old_elt_typ); void* _temp1495= Cyc_Absyn_cstar_typ( elt_typ,
Cyc_Toc_mt_tq); struct Cyc_Absyn_Exp* _temp1496= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_signed_int_exp((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1462), 0), 0); struct Cyc_Absyn_Exp* e1; if( _temp1078 == 0){ e1= Cyc_Toc_malloc_exp(
old_elt_typ, _temp1496);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp1078); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r,
_temp1496);}{ struct Cyc_Absyn_Exp* _temp1497= Cyc_Absyn_cast_exp( _temp1495, e1,
0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1476, _temp1495,( struct Cyc_Absyn_Exp*) _temp1497, _temp1478, 0), 0))->r));
goto _LL1451;}}} _LL1455: { int is_tagged_ptr= 0;{ void* _temp1498= Cyc_Tcutil_compress(
old_typ); struct Cyc_Absyn_PtrInfo _temp1504; struct Cyc_Absyn_Conref* _temp1506;
struct Cyc_Absyn_Tqual _temp1508; void* _temp1510; _LL1500: if(( unsigned int)
_temp1498 > 4u?*(( int*) _temp1498) == Cyc_Absyn_PointerType: 0){ _LL1505:
_temp1504=(( struct Cyc_Absyn_PointerType_struct*) _temp1498)->f1; _LL1511:
_temp1510=( void*) _temp1504.elt_typ; goto _LL1509; _LL1509: _temp1508=
_temp1504.tq; goto _LL1507; _LL1507: _temp1506= _temp1504.bounds; goto _LL1501;}
else{ goto _LL1502;} _LL1502: goto _LL1503; _LL1501: is_tagged_ptr= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1506) ==( void*) Cyc_Absyn_Unknown_b; goto _LL1499;
_LL1503:({ int(* _temp1512)( struct _tagged_arr fmt, struct _tagged_arr ap)=(
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos;
struct _tagged_arr _temp1515= _tag_arr("exp_to_c: comprehension not an array type",
sizeof( unsigned char), 42u); void* _temp1513[ 0u]={}; struct _tagged_arr
_temp1514={( void*) _temp1513,( void*) _temp1513,( void*)( _temp1513 + 0u)};
_temp1512( _temp1515, _temp1514);}); goto _LL1499; _LL1499:;}{ struct _tuple0*
max= Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var(); void*
old_elt_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1464->topt))->v;
void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c( nv, _temp1466);{ struct Cyc_Absyn_Stmt*
s= Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp( a, 0), _temp1468, Cyc_Absyn_var_exp(
max, 0), _temp1464, Cyc_Absyn_skip_stmt( 0), 1);{ struct _RegionHandle _temp1516=
_new_region(); struct _RegionHandle* r=& _temp1516; _push_region( r);{ struct
Cyc_List_List* decls=({ struct Cyc_List_List* _temp1539=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1539->hd=( void*)({
struct _tuple9* _temp1540=( struct _tuple9*) _region_malloc( r, sizeof( struct
_tuple9)); _temp1540->f1= max; _temp1540->f2= Cyc_Absyn_uint_t; _temp1540->f3=(
struct Cyc_Absyn_Exp*) _temp1466; _temp1540;}); _temp1539->tl= 0; _temp1539;});
struct Cyc_Absyn_Exp* ai; if( _temp1078 == 0){ ai= Cyc_Toc_malloc_exp(
old_elt_typ, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp1078); Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp( r,
Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));}{ struct Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp( ptr_typ, ai, 0);
decls=({ struct Cyc_List_List* _temp1517=( struct Cyc_List_List*) _region_malloc(
r, sizeof( struct Cyc_List_List)); _temp1517->hd=( void*)({ struct _tuple9*
_temp1518=( struct _tuple9*) _region_malloc( r, sizeof( struct _tuple9));
_temp1518->f1= a; _temp1518->f2= ptr_typ; _temp1518->f3=( struct Cyc_Absyn_Exp*)
ainit; _temp1518;}); _temp1517->tl= decls; _temp1517;}); if( is_tagged_ptr){
struct _tuple0* _temp1519= Cyc_Toc_temp_var(); void* _temp1520= Cyc_Toc_typ_to_c(
old_typ); struct Cyc_Absyn_Exp* _temp1521= Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({
struct Cyc_List_List*(* _temp1524)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1527= Cyc_Absyn_var_exp(
a, 0); struct Cyc_Absyn_Exp* _temp1528= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);
struct Cyc_Absyn_Exp* _temp1529= _temp1466; struct Cyc_Absyn_Exp* _temp1525[ 3u]={
_temp1527, _temp1528, _temp1529}; struct _tagged_arr _temp1526={( void*)
_temp1525,( void*) _temp1525,( void*)( _temp1525 + 3u)}; _temp1524( _temp1526);}),
0); decls=({ struct Cyc_List_List* _temp1522=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1522->hd=( void*)({
struct _tuple9* _temp1523=( struct _tuple9*) _region_malloc( r, sizeof( struct
_tuple9)); _temp1523->f1= _temp1519; _temp1523->f2= _temp1520; _temp1523->f3=(
struct Cyc_Absyn_Exp*) _temp1521; _temp1523;}); _temp1522->tl= decls; _temp1522;});
s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1519, 0),
0), 0);} else{ s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
a, 0), 0), 0);}{ struct Cyc_List_List* _temp1530= decls; for( 0; _temp1530 != 0;
_temp1530=(( struct Cyc_List_List*) _check_null( _temp1530))->tl){ struct Cyc_Absyn_Exp*
_temp1533; void* _temp1535; struct _tuple0* _temp1537; struct _tuple9 _temp1531=*((
struct _tuple9*)(( struct Cyc_List_List*) _check_null( _temp1530))->hd); _LL1538:
_temp1537= _temp1531.f1; goto _LL1536; _LL1536: _temp1535= _temp1531.f2; goto
_LL1534; _LL1534: _temp1533= _temp1531.f3; goto _LL1532; _LL1532: s= Cyc_Absyn_declare_stmt(
_temp1537, _temp1535, _temp1533, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));}}; _pop_region( r);} goto _LL1451;}}} _LL1457:( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1076->topt))->v, 1, _temp1078, _temp1470, _temp1472))->r)); goto _LL1451;
_LL1459:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, _temp1078,
_temp1474))->r)); goto _LL1451; _LL1461: { void* old_elt_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1076->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); struct Cyc_Absyn_Exp* mexp= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);
if( _temp1078 == 0){ mexp= Cyc_Toc_malloc_exp( old_elt_typ, mexp);} else{ struct
Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( _temp1078); Cyc_Toc_exp_to_c(
nv, r); mexp= Cyc_Toc_rmalloc_exp( r, mexp);} mexp= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq), mexp, 0);{ int done= 0;{ void* _temp1541=( void*)
_temp1076->r; struct Cyc_Absyn_Exp* _temp1547; void* _temp1549; _LL1543: if(*((
int*) _temp1541) == Cyc_Absyn_Cast_e){ _LL1550: _temp1549=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1541)->f1; goto _LL1548; _LL1548: _temp1547=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1541)->f2; goto _LL1544;} else{ goto _LL1545;} _LL1545: goto _LL1546;
_LL1544:{ struct _tuple8 _temp1552=({ struct _tuple8 _temp1551; _temp1551.f1=
Cyc_Tcutil_compress( _temp1549); _temp1551.f2= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1547->topt))->v); _temp1551;}); void*
_temp1558; struct Cyc_Absyn_PtrInfo _temp1560; struct Cyc_Absyn_Conref*
_temp1562; void* _temp1564; struct Cyc_Absyn_PtrInfo _temp1566; struct Cyc_Absyn_Conref*
_temp1568; void* _temp1570; _LL1554: _LL1565: _temp1564= _temp1552.f1; if((
unsigned int) _temp1564 > 4u?*(( int*) _temp1564) == Cyc_Absyn_PointerType: 0){
_LL1567: _temp1566=(( struct Cyc_Absyn_PointerType_struct*) _temp1564)->f1;
_LL1571: _temp1570=( void*) _temp1566.elt_typ; goto _LL1569; _LL1569: _temp1568=
_temp1566.bounds; goto _LL1559;} else{ goto _LL1556;} _LL1559: _temp1558=
_temp1552.f2; if(( unsigned int) _temp1558 > 4u?*(( int*) _temp1558) == Cyc_Absyn_PointerType:
0){ _LL1561: _temp1560=(( struct Cyc_Absyn_PointerType_struct*) _temp1558)->f1;
_LL1563: _temp1562= _temp1560.bounds; goto _LL1555;} else{ goto _LL1556;}
_LL1556: goto _LL1557; _LL1555:{ struct _tuple8 _temp1573=({ struct _tuple8
_temp1572; _temp1572.f1= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1568);
_temp1572.f2= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1562); _temp1572;});
void* _temp1579; struct Cyc_Absyn_Exp* _temp1581; void* _temp1583; _LL1575:
_LL1584: _temp1583= _temp1573.f1; if( _temp1583 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1580;} else{ goto _LL1577;} _LL1580: _temp1579= _temp1573.f2; if((
unsigned int) _temp1579 > 1u?*(( int*) _temp1579) == Cyc_Absyn_Upper_b: 0){
_LL1582: _temp1581=(( struct Cyc_Absyn_Upper_b_struct*) _temp1579)->f1; goto
_LL1576;} else{ goto _LL1577;} _LL1577: goto _LL1578; _LL1576: done= 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__init_tag_arr_e,({ struct
Cyc_List_List*(* _temp1585)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1588= mexp;
struct Cyc_Absyn_Exp* _temp1589= _temp1547; struct Cyc_Absyn_Exp* _temp1590= Cyc_Absyn_sizeoftyp_exp(
_temp1570, 0); struct Cyc_Absyn_Exp* _temp1591= _temp1581; struct Cyc_Absyn_Exp*
_temp1586[ 4u]={ _temp1588, _temp1589, _temp1590, _temp1591}; struct _tagged_arr
_temp1587={( void*) _temp1586,( void*) _temp1586,( void*)( _temp1586 + 4u)};
_temp1585( _temp1587);}), 0))->r)); goto _LL1574; _LL1578: goto _LL1574; _LL1574:;}
goto _LL1553; _LL1557: goto _LL1553; _LL1553:;} goto _LL1542; _LL1546: goto
_LL1542; _LL1542:;} if( ! done){ struct _tuple0* _temp1592= Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt* _temp1593= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
_temp1592, 0), 0); struct Cyc_Absyn_Exp* _temp1594= Cyc_Absyn_signed_int_exp( 0,
0); Cyc_Toc_exp_to_c( nv, _temp1076); _temp1593= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( _temp1592, 0), _temp1594, 0),
_temp1076, 0), _temp1593, 0);{ void* _temp1595= Cyc_Absyn_cstar_typ( elt_typ,
Cyc_Toc_mt_tq);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1592, _temp1595,( struct Cyc_Absyn_Exp*) mexp, _temp1593, 0), 0))->r));}}
goto _LL1451;}} _LL1451:;} goto _LL927; _LL965: Cyc_Toc_exp_to_c( nv, _temp1080);
goto _LL927; _LL967:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1596=( struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1596[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1597; _temp1597.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1597.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1082);
_temp1597;}); _temp1596;}))); goto _LL927; _LL969:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Offsetof_e_struct* _temp1598=( struct Cyc_Absyn_Offsetof_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp1598[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp1599; _temp1599.tag= Cyc_Absyn_Offsetof_e;
_temp1599.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1086); _temp1599.f2=
_temp1084; _temp1599;}); _temp1598;}))); goto _LL927; _LL971: { void* _temp1600=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1088->topt))->v);{
void* _temp1601= _temp1600; struct Cyc_Absyn_PtrInfo _temp1607; struct Cyc_Absyn_Conref*
_temp1609; struct Cyc_Absyn_Tqual _temp1611; struct Cyc_Absyn_Conref* _temp1613;
void* _temp1615; void* _temp1617; _LL1603: if(( unsigned int) _temp1601 > 4u?*((
int*) _temp1601) == Cyc_Absyn_PointerType: 0){ _LL1608: _temp1607=(( struct Cyc_Absyn_PointerType_struct*)
_temp1601)->f1; _LL1618: _temp1617=( void*) _temp1607.elt_typ; goto _LL1616;
_LL1616: _temp1615=( void*) _temp1607.rgn_typ; goto _LL1614; _LL1614: _temp1613=
_temp1607.nullable; goto _LL1612; _LL1612: _temp1611= _temp1607.tq; goto _LL1610;
_LL1610: _temp1609= _temp1607.bounds; goto _LL1604;} else{ goto _LL1605;}
_LL1605: goto _LL1606; _LL1604:{ void* _temp1619= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1609); _LL1621: if(( unsigned int) _temp1619 > 1u?*(( int*) _temp1619) ==
Cyc_Absyn_Upper_b: 0){ goto _LL1622;} else{ goto _LL1623;} _LL1623: if(
_temp1619 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1624;} else{ goto _LL1620;}
_LL1622: Cyc_Toc_exp_to_c( nv, _temp1088); if( Cyc_Toc_is_nullable( _temp1600)){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1088->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1625=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1625->hd=( void*) _temp1088;
_temp1625->tl= 0; _temp1625;}), 0), 0), 0))->r));} goto _LL1620; _LL1624: {
struct Cyc_Absyn_Exp* _temp1626= Cyc_Absyn_uint_exp( 0, 0); _temp1626->topt=({
struct Cyc_Core_Opt* _temp1627=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1627->v=( void*) Cyc_Absyn_uint_t; _temp1627;});( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_subscript_exp( _temp1088, _temp1626, 0))->r)); Cyc_Toc_exp_to_c(
nv, e); goto _LL1620;} _LL1620:;} goto _LL1602; _LL1606:({ int(* _temp1628)(
struct _tagged_arr fmt, struct _tagged_arr ap)=( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos; struct _tagged_arr _temp1631=
_tag_arr("exp_to_c: Deref: non-pointer", sizeof( unsigned char), 29u); void*
_temp1629[ 0u]={}; struct _tagged_arr _temp1630={( void*) _temp1629,( void*)
_temp1629,( void*)( _temp1629 + 0u)}; _temp1628( _temp1631, _temp1630);}); goto
_LL1602; _LL1602:;} goto _LL927;} _LL973: Cyc_Toc_exp_to_c( nv, _temp1092); if(
Cyc_Toc_is_poly_project( e)){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));} goto _LL927; _LL975: { void*
e1typ= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1096->topt))->v); Cyc_Toc_exp_to_c( nv, _temp1096);{ int is_poly= Cyc_Toc_is_poly_project(
e); struct Cyc_Absyn_Conref* _temp1634; struct Cyc_Absyn_Tqual _temp1636; struct
Cyc_Absyn_Conref* _temp1638; void* _temp1640; void* _temp1642; struct Cyc_Absyn_PtrInfo
_temp1632= Cyc_Toc_get_ptr_type( e1typ); _LL1643: _temp1642=( void*) _temp1632.elt_typ;
goto _LL1641; _LL1641: _temp1640=( void*) _temp1632.rgn_typ; goto _LL1639;
_LL1639: _temp1638= _temp1632.nullable; goto _LL1637; _LL1637: _temp1636=
_temp1632.tq; goto _LL1635; _LL1635: _temp1634= _temp1632.bounds; goto _LL1633;
_LL1633:{ void* _temp1644= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1634);
struct Cyc_Absyn_Exp* _temp1650; _LL1646: if(( unsigned int) _temp1644 > 1u?*((
int*) _temp1644) == Cyc_Absyn_Upper_b: 0){ _LL1651: _temp1650=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1644)->f1; goto _LL1647;} else{ goto _LL1648;} _LL1648: if( _temp1644 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL1649;} else{ goto _LL1645;} _LL1647: if(
Cyc_Evexp_eval_const_uint_exp( _temp1650) < 1){({ int(* _temp1652)( struct
_tagged_arr fmt, struct _tagged_arr ap)=( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos; struct _tagged_arr _temp1655= _tag_arr("exp_to_c:  StructArrow_e on pointer of size 0",
sizeof( unsigned char), 46u); void* _temp1653[ 0u]={}; struct _tagged_arr
_temp1654={( void*) _temp1653,( void*) _temp1653,( void*)( _temp1653 + 0u)};
_temp1652( _temp1655, _temp1654);});}{ int possibly_null=(( int(*)( int, struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0, _temp1638); if( Cyc_Toc_is_nullable((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1096->topt))->v)){( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1096->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1656=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1656->hd=( void*) _temp1096;
_temp1656->tl= 0; _temp1656;}), 0), 0), _temp1094, 0))->r));} goto _LL1645;}
_LL1649: { void* ta1= Cyc_Toc_typ_to_c_array( _temp1642);( void*)( _temp1096->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( ta1, _temp1636), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_unknown_subscript_e,({ struct Cyc_List_List*(* _temp1657)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1660= Cyc_Absyn_copy_exp( _temp1096); struct Cyc_Absyn_Exp*
_temp1661= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp* _temp1662=
Cyc_Absyn_uint_exp( 0, 0); struct Cyc_Absyn_Exp* _temp1658[ 3u]={ _temp1660,
_temp1661, _temp1662}; struct _tagged_arr _temp1659={( void*) _temp1658,( void*)
_temp1658,( void*)( _temp1658 + 3u)}; _temp1657( _temp1659);}), 0), 0))->r));
goto _LL1645;} _LL1645:;} if( is_poly){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));} goto _LL927;}} _LL977: { void*
_temp1663= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1100->topt))->v); Cyc_Toc_exp_to_c( nv, _temp1100); Cyc_Toc_exp_to_c( nv,
_temp1098);{ void* _temp1664= _temp1663; struct Cyc_List_List* _temp1672; struct
Cyc_Absyn_PtrInfo _temp1674; struct Cyc_Absyn_Conref* _temp1676; struct Cyc_Absyn_Tqual
_temp1678; struct Cyc_Absyn_Conref* _temp1680; void* _temp1682; void* _temp1684;
_LL1666: if(( unsigned int) _temp1664 > 4u?*(( int*) _temp1664) == Cyc_Absyn_TupleType:
0){ _LL1673: _temp1672=(( struct Cyc_Absyn_TupleType_struct*) _temp1664)->f1;
goto _LL1667;} else{ goto _LL1668;} _LL1668: if(( unsigned int) _temp1664 > 4u?*((
int*) _temp1664) == Cyc_Absyn_PointerType: 0){ _LL1675: _temp1674=(( struct Cyc_Absyn_PointerType_struct*)
_temp1664)->f1; _LL1685: _temp1684=( void*) _temp1674.elt_typ; goto _LL1683;
_LL1683: _temp1682=( void*) _temp1674.rgn_typ; goto _LL1681; _LL1681: _temp1680=
_temp1674.nullable; goto _LL1679; _LL1679: _temp1678= _temp1674.tq; goto _LL1677;
_LL1677: _temp1676= _temp1674.bounds; goto _LL1669;} else{ goto _LL1670;}
_LL1670: goto _LL1671; _LL1667: { unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1098) + 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
_temp1100, Cyc_Toc_fieldname(( int) i), 0))->r)); goto _LL1665;} _LL1669:{ void*
_temp1686= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1676); struct Cyc_Absyn_Exp*
_temp1692; _LL1688: if(( unsigned int) _temp1686 > 1u?*(( int*) _temp1686) ==
Cyc_Absyn_Upper_b: 0){ _LL1693: _temp1692=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1686)->f1; goto _LL1689;} else{ goto _LL1690;} _LL1690: if( _temp1686 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL1691;} else{ goto _LL1687;} _LL1689:
_temp1692= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp( _temp1692), 0);{
int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
0, _temp1680); void* ta1= Cyc_Toc_typ_to_c( _temp1684); void* ta2= Cyc_Absyn_cstar_typ(
ta1, _temp1678); if( possibly_null){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp(
Cyc_Absyn_cast_exp( ta2, Cyc_Absyn_fncall_exp( Cyc_Toc__check_known_subscript_null_e,({
struct Cyc_List_List*(* _temp1694)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1697= _temp1100;
struct Cyc_Absyn_Exp* _temp1698= _temp1692; struct Cyc_Absyn_Exp* _temp1699= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); struct Cyc_Absyn_Exp* _temp1700= _temp1098; struct Cyc_Absyn_Exp*
_temp1695[ 4u]={ _temp1697, _temp1698, _temp1699, _temp1700}; struct _tagged_arr
_temp1696={( void*) _temp1695,( void*) _temp1695,( void*)( _temp1695 + 4u)};
_temp1694( _temp1696);}), 0), 0), 0))->r));} else{( void*)( _temp1098->r=( void*)((
void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__check_known_subscript_notnull_e,({ struct
Cyc_List_List*(* _temp1701)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1704= _temp1692;
struct Cyc_Absyn_Exp* _temp1705= Cyc_Absyn_copy_exp( _temp1098); struct Cyc_Absyn_Exp*
_temp1702[ 2u]={ _temp1704, _temp1705}; struct _tagged_arr _temp1703={( void*)
_temp1702,( void*) _temp1702,( void*)( _temp1702 + 2u)}; _temp1701( _temp1703);}),
0))->r));} goto _LL1687;} _LL1691: { void* ta1= Cyc_Toc_typ_to_c_array(
_temp1684);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( ta1, _temp1678), Cyc_Absyn_fncall_exp( Cyc_Toc__check_unknown_subscript_e,({
struct Cyc_List_List*(* _temp1706)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1709= _temp1100;
struct Cyc_Absyn_Exp* _temp1710= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp*
_temp1711= _temp1098; struct Cyc_Absyn_Exp* _temp1707[ 3u]={ _temp1709,
_temp1710, _temp1711}; struct _tagged_arr _temp1708={( void*) _temp1707,( void*)
_temp1707,( void*)( _temp1707 + 3u)}; _temp1706( _temp1708);}), 0), 0), 0))->r));
goto _LL1687;} _LL1687:;} goto _LL1665; _LL1671:({ int(* _temp1712)( struct
_tagged_arr fmt, struct _tagged_arr ap)=( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos; struct _tagged_arr _temp1715= _tag_arr("exp_to_c: Subscript on non-tuple/array/tuple ptr",
sizeof( unsigned char), 49u); void* _temp1713[ 0u]={}; struct _tagged_arr
_temp1714={( void*) _temp1713,( void*) _temp1713,( void*)( _temp1713 + 0u)};
_temp1712( _temp1715, _temp1714);}); goto _LL1665; _LL1665:;} goto _LL927;}
_LL979: if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple(
nv, 0, 0, _temp1102))->r));} else{ struct Cyc_List_List* _temp1716=(( struct Cyc_List_List*(*)(
struct _tuple3*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Toc_tup_to_c, _temp1102); struct _tagged_arr* _temp1717= Cyc_Toc_add_tuple_type(
_temp1716); struct Cyc_List_List* dles= 0;{ int i= 1; for( 0; _temp1102 != 0;(
_temp1102=(( struct Cyc_List_List*) _check_null( _temp1102))->tl, i ++)){ Cyc_Toc_exp_to_c(
nv,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1102))->hd);
dles=({ struct Cyc_List_List* _temp1718=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1718->hd=( void*)({ struct _tuple4*
_temp1719=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1719->f1=
0; _temp1719->f2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp1102))->hd; _temp1719;}); _temp1718->tl= dles; _temp1718;});}} dles=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, dles, 0))->r));}
goto _LL927; _LL981:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1104, 0))->r));{ struct Cyc_List_List* _temp1720= _temp1104; for( 0;
_temp1720 != 0; _temp1720=(( struct Cyc_List_List*) _check_null( _temp1720))->tl){
struct _tuple4 _temp1723; struct Cyc_Absyn_Exp* _temp1724; struct _tuple4*
_temp1721=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1720))->hd;
_temp1723=* _temp1721; _LL1725: _temp1724= _temp1723.f2; goto _LL1722; _LL1722:
Cyc_Toc_exp_to_c( nv, _temp1724);}} goto _LL927; _LL983: { unsigned int
_temp1726= Cyc_Evexp_eval_const_uint_exp( _temp1108); Cyc_Toc_exp_to_c( nv,
_temp1106);{ struct Cyc_List_List* es= 0; if( ! Cyc_Toc_is_zero( _temp1106)){
unsigned int i= 0; for( 0; i < _temp1726; i ++){ es=({ struct Cyc_List_List*
_temp1727=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1727->hd=( void*)({ struct _tuple4* _temp1728=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1728->f1= 0; _temp1728->f2= _temp1106; _temp1728;});
_temp1727->tl= es; _temp1727;});}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, es, 0))->r)); goto _LL927;}} _LL985: if( ! nv->toplevel){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1114, _temp1118))->r));}
else{ if( _temp1112 == 0){({ int(* _temp1729)( struct _tagged_arr fmt, struct
_tagged_arr ap)=( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos;
struct _tagged_arr _temp1732= _tag_arr("Struct_e: missing structdecl pointer",
sizeof( unsigned char), 37u); void* _temp1730[ 0u]={}; struct _tagged_arr
_temp1731={( void*) _temp1730,( void*) _temp1730,( void*)( _temp1730 + 0u)};
_temp1729( _temp1732, _temp1731);});}{ struct Cyc_Absyn_Structdecl* sd2=( struct
Cyc_Absyn_Structdecl*) _check_null( _temp1112); struct _RegionHandle _temp1733=
_new_region(); struct _RegionHandle* rgn=& _temp1733; _push_region( rgn);{
struct Cyc_List_List* _temp1734=(( struct Cyc_List_List*(*)( struct
_RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List* des,
struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn, e->loc,
_temp1114,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd2->fields))->v);
struct Cyc_List_List* _temp1735= 0;{ struct Cyc_List_List* _temp1736=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd2->fields))->v; for( 0;
_temp1736 != 0; _temp1736=(( struct Cyc_List_List*) _check_null( _temp1736))->tl){
struct Cyc_List_List* _temp1737= _temp1734; for( 0; _temp1737 != 0; _temp1737=((
struct Cyc_List_List*) _check_null( _temp1737))->tl){ if((*(( struct _tuple10*)((
struct Cyc_List_List*) _check_null( _temp1737))->hd)).f1 ==( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp1736))->hd){ struct _tuple10 _temp1740;
struct Cyc_Absyn_Exp* _temp1741; struct Cyc_Absyn_Structfield* _temp1743; struct
_tuple10* _temp1738=( struct _tuple10*)(( struct Cyc_List_List*) _check_null(
_temp1737))->hd; _temp1740=* _temp1738; _LL1744: _temp1743= _temp1740.f1; goto
_LL1742; _LL1742: _temp1741= _temp1740.f2; goto _LL1739; _LL1739: { void*
_temp1745=( void*) _temp1743->type; Cyc_Toc_exp_to_c( nv, _temp1741); if( Cyc_Toc_is_void_star(
_temp1745)){( void*)( _temp1741->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1741->r, 0), 0))->r));} _temp1735=({ struct Cyc_List_List*
_temp1746=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1746->hd=( void*)({ struct _tuple4* _temp1747=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1747->f1= 0; _temp1747->f2= _temp1741; _temp1747;});
_temp1746->tl= _temp1735; _temp1746;}); break;}}}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp1735), 0))->r));}; _pop_region( rgn);}} goto _LL927;
_LL987: { struct Cyc_List_List* fs;{ void* _temp1748= Cyc_Tcutil_compress(
_temp1122); struct Cyc_List_List* _temp1754; _LL1750: if(( unsigned int)
_temp1748 > 4u?*(( int*) _temp1748) == Cyc_Absyn_AnonStructType: 0){ _LL1755:
_temp1754=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1748)->f1; goto
_LL1751;} else{ goto _LL1752;} _LL1752: goto _LL1753; _LL1751: fs= _temp1754;
goto _LL1749; _LL1753: fs=({ struct Cyc_List_List*(* _temp1756)( struct
_tagged_arr fmt, struct _tagged_arr ap)=( struct Cyc_List_List*(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos; struct _tagged_arr
_temp1760= _tag_arr("anon struct has type %s", sizeof( unsigned char), 24u);
struct Cyc_Stdio_String_pa_struct _temp1761; _temp1761.tag= Cyc_Stdio_String_pa;
_temp1761.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( _temp1122);{ void*
_temp1759=( void*)& _temp1761; void* _temp1757[ 1u]={ _temp1759}; struct
_tagged_arr _temp1758={( void*) _temp1757,( void*) _temp1757,( void*)( _temp1757
+ 1u)}; _temp1756( _temp1760, _temp1758);}}); goto _LL1749; _LL1749:;}{ struct
_RegionHandle _temp1762= _new_region(); struct _RegionHandle* rgn=& _temp1762;
_push_region( rgn);{ struct Cyc_List_List* _temp1763=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp1120, fs); for( 0; _temp1763 != 0; _temp1763=(( struct Cyc_List_List*)
_check_null( _temp1763))->tl){ struct _tuple10 _temp1766; struct Cyc_Absyn_Exp*
_temp1767; struct Cyc_Absyn_Structfield* _temp1769; struct _tuple10* _temp1764=(
struct _tuple10*)(( struct Cyc_List_List*) _check_null( _temp1763))->hd;
_temp1766=* _temp1764; _LL1770: _temp1769= _temp1766.f1; goto _LL1768; _LL1768:
_temp1767= _temp1766.f2; goto _LL1765; _LL1765: { void* _temp1771=( void*)
_temp1769->type; Cyc_Toc_exp_to_c( nv, _temp1767); if( Cyc_Toc_is_void_star(
_temp1771)){( void*)( _temp1767->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1767->r, 0), 0))->r));}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0, _temp1120, 0))->r));}; _pop_region( rgn);}
goto _LL927;} _LL989: { struct _tuple0* qv= _temp1124->name; if( _temp1126->is_xtunion?
1: ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));}
else{ int tag_count= 0; struct Cyc_List_List* _temp1772= _temp1126->fields == 0?
0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1126->fields))->v;
while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp1772))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1772))->hd)->typs == 0){ tag_count ++;}
_temp1772=(( struct Cyc_List_List*) _check_null( _temp1772))->tl;}( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));} goto
_LL927;} _LL991: { struct _tuple0* _temp1773= _temp1130->name; struct Cyc_List_List*
_temp1774= _temp1130->typs; struct _tuple0* _temp1775= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* _temp1776= Cyc_Absyn_var_exp( _temp1775, 0); void*
_temp1777= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1773, _tag_arr("_struct",
sizeof( unsigned char), 8u))); if( nv->toplevel){ struct Cyc_Absyn_Exp* tag_exp;
if( _temp1132->is_xtunion){ tag_exp= Cyc_Absyn_var_exp( _temp1773, 0);} else{
int tag_count= 0; struct Cyc_List_List* _temp1778= _temp1132->fields == 0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1132->fields))->v;
while( Cyc_Absyn_qvar_cmp( _temp1773,(( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp1778))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1778))->hd)->typs != 0){ tag_count ++;}
_temp1778=(( struct Cyc_List_List*) _check_null( _temp1778))->tl;} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* _temp1779= 0; for( 0;
_temp1134 != 0;( _temp1134=(( struct Cyc_List_List*) _check_null( _temp1134))->tl,
_temp1774=(( struct Cyc_List_List*) _check_null( _temp1774))->tl)){ struct Cyc_Absyn_Exp*
cur_e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1134))->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1774))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);} _temp1779=({
struct Cyc_List_List* _temp1780=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1780->hd=( void*)({ struct _tuple4* _temp1781=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1781->f1= 0;
_temp1781->f2= cur_e; _temp1781;}); _temp1780->tl= _temp1779; _temp1780;});}
_temp1779=({ struct Cyc_List_List* _temp1782=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1782->hd=( void*)({ struct _tuple4*
_temp1783=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1783->f1=
0; _temp1783->f2= tag_exp; _temp1783;}); _temp1782->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1779); _temp1782;});( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp1784=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1784->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp1773, _tag_arr("_struct",
sizeof( unsigned char), 8u)); _temp1784;}), _temp1779, 0))->r));}} else{ struct
Cyc_List_List* _temp1785=({ struct Cyc_List_List* _temp1791=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1791->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp1776, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp(
_temp1773, 0), 0); _temp1791->tl= 0; _temp1791;});{ int i= 1; for( 0; _temp1134
!= 0;((( _temp1134=(( struct Cyc_List_List*) _check_null( _temp1134))->tl, i ++)),
_temp1774=(( struct Cyc_List_List*) _check_null( _temp1774))->tl)){ struct Cyc_Absyn_Exp*
cur_e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1134))->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1774))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt*
_temp1786= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1776, Cyc_Toc_fieldname(
i), 0), cur_e, 0); _temp1785=({ struct Cyc_List_List* _temp1787=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1787->hd=( void*) _temp1786;
_temp1787->tl= _temp1785; _temp1787;});}}}{ struct Cyc_Absyn_Stmt* _temp1788=
Cyc_Absyn_exp_stmt( _temp1776, 0); struct Cyc_Absyn_Stmt* _temp1789= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct
Cyc_List_List* _temp1790=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1790->hd=( void*) _temp1788; _temp1790->tl= _temp1785; _temp1790;})), 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1775, _temp1777, 0, _temp1789, 0), 0))->r));}} goto _LL927;} _LL993: goto
_LL927; _LL995: { void* t_c= Cyc_Toc_typ_to_c( _temp1136); if( _temp1138 != 0){
struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*) _check_null( _temp1138); Cyc_Toc_exp_to_c(
nv, rgn);( void*)( e->r=( void*)(( void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp(
_temp1136, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));} goto _LL927;} _LL997: Cyc_Toc_stmt_to_c(
nv, _temp1140); goto _LL927; _LL999:({ int(* _temp1792)( struct _tagged_arr fmt,
struct _tagged_arr ap)=( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos; struct _tagged_arr _temp1795= _tag_arr("UnresolvedMem",
sizeof( unsigned char), 14u); void* _temp1793[ 0u]={}; struct _tagged_arr
_temp1794={( void*) _temp1793,( void*) _temp1793,( void*)( _temp1793 + 0u)};
_temp1792( _temp1795, _temp1794);}); goto _LL927; _LL1001:({ int(* _temp1796)(
struct _tagged_arr fmt, struct _tagged_arr ap)=( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_unimp; struct _tagged_arr _temp1799= _tag_arr("compoundlit",
sizeof( unsigned char), 12u); void* _temp1797[ 0u]={}; struct _tagged_arr
_temp1798={( void*) _temp1797,( void*) _temp1797,( void*)( _temp1797 + 0u)};
_temp1796( _temp1799, _temp1798);}); goto _LL927; _LL1003:({ int(* _temp1800)(
struct _tagged_arr fmt, struct _tagged_arr ap)=( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_unimp; struct _tagged_arr _temp1803= _tag_arr("codegen",
sizeof( unsigned char), 8u); void* _temp1801[ 0u]={}; struct _tagged_arr
_temp1802={( void*) _temp1801,( void*) _temp1801,( void*)( _temp1801 + 0u)};
_temp1800( _temp1803, _temp1802);}); goto _LL927; _LL1005:({ int(* _temp1804)(
struct _tagged_arr fmt, struct _tagged_arr ap)=( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_unimp; struct _tagged_arr _temp1807= _tag_arr("fill",
sizeof( unsigned char), 5u); void* _temp1805[ 0u]={}; struct _tagged_arr
_temp1806={( void*) _temp1805,( void*) _temp1805,( void*)( _temp1805 + 0u)};
_temp1804( _temp1807, _temp1806);}); goto _LL927; _LL927:;}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_eq_goto_stmt( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
struct _tagged_arr* succ_lab, struct _tagged_arr* fail_lab){ return Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0);} struct _tuple11{ struct Cyc_Toc_Env* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Stmt* f3; } ; struct _tuple12{ struct _tuple0* f1; void* f2;
} ; struct _tuple13{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ;
static struct _tuple11 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env* nv, struct
_RegionHandle* rgn, void* t, struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path,
struct Cyc_Absyn_Pat* p, struct _tagged_arr* succ_lab, struct _tagged_arr*
fail_lab, struct Cyc_List_List* decls){ struct Cyc_Absyn_Stmt* s;{ void*
_temp1808=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp1844; struct Cyc_Absyn_Vardecl
_temp1846; struct _tuple0* _temp1847; struct Cyc_Absyn_Vardecl* _temp1849; int
_temp1851; void* _temp1853; unsigned char _temp1855; struct _tagged_arr
_temp1857; struct Cyc_Absyn_Enumfield* _temp1859; struct Cyc_Absyn_Enumdecl*
_temp1861; struct Cyc_List_List* _temp1863; struct Cyc_List_List* _temp1865;
struct Cyc_Absyn_Tunionfield* _temp1867; struct Cyc_Absyn_Tuniondecl* _temp1869;
struct Cyc_Absyn_Pat* _temp1872; struct Cyc_Absyn_Pat _temp1874; void* _temp1875;
struct Cyc_List_List* _temp1877; struct Cyc_List_List* _temp1879; struct Cyc_Absyn_Tunionfield*
_temp1881; struct Cyc_Absyn_Tuniondecl* _temp1883; struct Cyc_List_List*
_temp1885; struct Cyc_List_List* _temp1887; struct Cyc_List_List* _temp1889;
struct Cyc_List_List* _temp1891; struct Cyc_Core_Opt* _temp1893; struct Cyc_Absyn_Structdecl*
_temp1895; struct Cyc_Absyn_Pat* _temp1897; _LL1810: if(( unsigned int)
_temp1808 > 2u?*(( int*) _temp1808) == Cyc_Absyn_Var_p: 0){ _LL1845: _temp1844=((
struct Cyc_Absyn_Var_p_struct*) _temp1808)->f1; _temp1846=* _temp1844; _LL1848:
_temp1847= _temp1846.name; goto _LL1811;} else{ goto _LL1812;} _LL1812: if(
_temp1808 ==( void*) Cyc_Absyn_Wild_p){ goto _LL1813;} else{ goto _LL1814;}
_LL1814: if(( unsigned int) _temp1808 > 2u?*(( int*) _temp1808) == Cyc_Absyn_Reference_p:
0){ _LL1850: _temp1849=(( struct Cyc_Absyn_Reference_p_struct*) _temp1808)->f1;
goto _LL1815;} else{ goto _LL1816;} _LL1816: if( _temp1808 ==( void*) Cyc_Absyn_Null_p){
goto _LL1817;} else{ goto _LL1818;} _LL1818: if(( unsigned int) _temp1808 > 2u?*((
int*) _temp1808) == Cyc_Absyn_Int_p: 0){ _LL1854: _temp1853=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1808)->f1; goto _LL1852; _LL1852: _temp1851=((
struct Cyc_Absyn_Int_p_struct*) _temp1808)->f2; goto _LL1819;} else{ goto
_LL1820;} _LL1820: if(( unsigned int) _temp1808 > 2u?*(( int*) _temp1808) == Cyc_Absyn_Char_p:
0){ _LL1856: _temp1855=(( struct Cyc_Absyn_Char_p_struct*) _temp1808)->f1; goto
_LL1821;} else{ goto _LL1822;} _LL1822: if(( unsigned int) _temp1808 > 2u?*((
int*) _temp1808) == Cyc_Absyn_Float_p: 0){ _LL1858: _temp1857=(( struct Cyc_Absyn_Float_p_struct*)
_temp1808)->f1; goto _LL1823;} else{ goto _LL1824;} _LL1824: if(( unsigned int)
_temp1808 > 2u?*(( int*) _temp1808) == Cyc_Absyn_Enum_p: 0){ _LL1862: _temp1861=((
struct Cyc_Absyn_Enum_p_struct*) _temp1808)->f1; goto _LL1860; _LL1860:
_temp1859=(( struct Cyc_Absyn_Enum_p_struct*) _temp1808)->f2; goto _LL1825;}
else{ goto _LL1826;} _LL1826: if(( unsigned int) _temp1808 > 2u?*(( int*)
_temp1808) == Cyc_Absyn_Tunion_p: 0){ _LL1870: _temp1869=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1808)->f1; goto _LL1868; _LL1868: _temp1867=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1808)->f2; goto _LL1866; _LL1866: _temp1865=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1808)->f3; goto _LL1864; _LL1864: _temp1863=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1808)->f4; if( _temp1863 == 0){ goto _LL1827;} else{ goto _LL1828;}} else{
goto _LL1828;} _LL1828: if(( unsigned int) _temp1808 > 2u?*(( int*) _temp1808)
== Cyc_Absyn_Pointer_p: 0){ _LL1873: _temp1872=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1808)->f1; _temp1874=* _temp1872; _LL1876: _temp1875=( void*) _temp1874.r;
if(( unsigned int) _temp1875 > 2u?*(( int*) _temp1875) == Cyc_Absyn_Tunion_p: 0){
_LL1884: _temp1883=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1875)->f1; goto
_LL1882; _LL1882: _temp1881=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1875)->f2;
goto _LL1880; _LL1880: _temp1879=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1875)->f3;
goto _LL1878; _LL1878: _temp1877=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1875)->f4;
goto _LL1871;} else{ goto _LL1830;}} else{ goto _LL1830;} _LL1871: if( _temp1877
!= 0){ goto _LL1829;} else{ goto _LL1830;} _LL1830: if(( unsigned int) _temp1808
> 2u?*(( int*) _temp1808) == Cyc_Absyn_Tunion_p: 0){ _LL1886: _temp1885=((
struct Cyc_Absyn_Tunion_p_struct*) _temp1808)->f4; goto _LL1831;} else{ goto
_LL1832;} _LL1832: if(( unsigned int) _temp1808 > 2u?*(( int*) _temp1808) == Cyc_Absyn_Tuple_p:
0){ _LL1888: _temp1887=(( struct Cyc_Absyn_Tuple_p_struct*) _temp1808)->f1; goto
_LL1833;} else{ goto _LL1834;} _LL1834: if(( unsigned int) _temp1808 > 2u?*((
int*) _temp1808) == Cyc_Absyn_Struct_p: 0){ _LL1896: _temp1895=(( struct Cyc_Absyn_Struct_p_struct*)
_temp1808)->f1; goto _LL1894; _LL1894: _temp1893=(( struct Cyc_Absyn_Struct_p_struct*)
_temp1808)->f2; goto _LL1892; _LL1892: _temp1891=(( struct Cyc_Absyn_Struct_p_struct*)
_temp1808)->f3; goto _LL1890; _LL1890: _temp1889=(( struct Cyc_Absyn_Struct_p_struct*)
_temp1808)->f4; goto _LL1835;} else{ goto _LL1836;} _LL1836: if(( unsigned int)
_temp1808 > 2u?*(( int*) _temp1808) == Cyc_Absyn_Pointer_p: 0){ _LL1898:
_temp1897=(( struct Cyc_Absyn_Pointer_p_struct*) _temp1808)->f1; goto _LL1837;}
else{ goto _LL1838;} _LL1838: if(( unsigned int) _temp1808 > 2u?*(( int*)
_temp1808) == Cyc_Absyn_UnknownId_p: 0){ goto _LL1839;} else{ goto _LL1840;}
_LL1840: if(( unsigned int) _temp1808 > 2u?*(( int*) _temp1808) == Cyc_Absyn_UnknownCall_p:
0){ goto _LL1841;} else{ goto _LL1842;} _LL1842: if(( unsigned int) _temp1808 >
2u?*(( int*) _temp1808) == Cyc_Absyn_UnknownFields_p: 0){ goto _LL1843;} else{
goto _LL1809;} _LL1811: nv= Cyc_Toc_add_varmap( nv, _temp1847, r); goto _LL1813;
_LL1813: s= Cyc_Absyn_goto_stmt( succ_lab, 0); goto _LL1809; _LL1815: { struct
_tuple0* _temp1899= Cyc_Toc_temp_var(); decls=({ struct Cyc_List_List* _temp1900=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1900->hd=( void*)({ struct _tuple12* _temp1901=( struct _tuple12*)
_region_malloc( rgn, sizeof( struct _tuple12)); _temp1901->f1= _temp1899;
_temp1901->f2= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq);
_temp1901;}); _temp1900->tl= decls; _temp1900;}); nv= Cyc_Toc_add_varmap( nv,
_temp1849->name, Cyc_Absyn_var_exp( _temp1899, 0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp1899, 0), Cyc_Absyn_address_exp( path, 0), 0), Cyc_Absyn_goto_stmt(
succ_lab, 0), 0); goto _LL1809;} _LL1817: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp(
0, 0), succ_lab, fail_lab); goto _LL1809; _LL1819: s= Cyc_Toc_if_eq_goto_stmt( r,
Cyc_Absyn_int_exp( _temp1853, _temp1851, 0), succ_lab, fail_lab); goto _LL1809;
_LL1821: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp( _temp1855, 0),
succ_lab, fail_lab); goto _LL1809; _LL1823: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp(
_temp1857, 0), succ_lab, fail_lab); goto _LL1809; _LL1825: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp1902=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp1902[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp1903; _temp1903.tag= Cyc_Absyn_Enum_e;
_temp1903.f1= _temp1859->name; _temp1903.f2=( struct Cyc_Absyn_Enumdecl*)
_temp1861; _temp1903.f3=( struct Cyc_Absyn_Enumfield*) _temp1859; _temp1903;});
_temp1902;}), 0), succ_lab, fail_lab); goto _LL1809; _LL1827: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp1867->name, 0); if( ! _temp1869->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL1809;} _LL1829: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1877);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp1881->name, _tag_arr("_struct",
sizeof( unsigned char), 8u)); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0); struct
Cyc_List_List* _temp1904=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1881->typs); struct Cyc_List_List*
_temp1905=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp1877); for( 0; _temp1905 != 0;((( _temp1905=((
struct Cyc_List_List*) _check_null( _temp1905))->tl, _temp1904=(( struct Cyc_List_List*)
_check_null( _temp1904))->tl)), cnt --)){ struct Cyc_Absyn_Pat* _temp1906=(
struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null( _temp1905))->hd;
if(( void*) _temp1906->r ==( void*) Cyc_Absyn_Wild_p){ continue;}{ void*
_temp1907=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1904))->hd)).f2;
struct _tuple0* _temp1908= Cyc_Toc_temp_var(); void* _temp1909=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1906->topt))->v; void* _temp1910= Cyc_Toc_typ_to_c(
_temp1909); struct _tagged_arr* _temp1911= Cyc_Toc_fresh_label(); struct Cyc_Absyn_Exp*
_temp1912= Cyc_Absyn_structarrow_exp( rcast, Cyc_Toc_fieldname( cnt), 0); if(
Cyc_Toc_is_void_star( Cyc_Toc_typ_to_c( _temp1907))){ _temp1912= Cyc_Absyn_cast_exp(
_temp1910, _temp1912, 0);} decls=({ struct Cyc_List_List* _temp1913=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1913->hd=( void*)({
struct _tuple12* _temp1914=( struct _tuple12*) _region_malloc( rgn, sizeof(
struct _tuple12)); _temp1914->f1= _temp1908; _temp1914->f2= _temp1910; _temp1914;});
_temp1913->tl= decls; _temp1913;});{ struct _tuple11 _temp1915= Cyc_Toc_xlate_pat(
nv, rgn, _temp1909, Cyc_Absyn_var_exp( _temp1908, 0), _temp1912, _temp1906,
succ_lab, fail_lab, decls); nv= _temp1915.f1; decls= _temp1915.f2;{ struct Cyc_Absyn_Stmt*
_temp1916= _temp1915.f3; struct Cyc_Absyn_Stmt* _temp1917= Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp1908, 0), _temp1912, 0); struct Cyc_Absyn_Stmt*
_temp1918= Cyc_Absyn_seq_stmt( _temp1917, _temp1916, 0); ss=({ struct Cyc_List_List*
_temp1919=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1919->hd=( void*) Cyc_Absyn_label_stmt( _temp1911, _temp1918, 0); _temp1919->tl=
ss; _temp1919;}); succ_lab= _temp1911;}}}} if( ss == 0){ ss=({ struct Cyc_List_List*
_temp1920=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1920->hd=( void*) Cyc_Absyn_goto_stmt( succ_lab, 0); _temp1920->tl= 0;
_temp1920;});}{ struct Cyc_Absyn_Exp* test_exp; if( _temp1883->is_xtunion){
struct Cyc_Absyn_Exp* e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(),
Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0);
struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp1881->name, 0); test_exp= Cyc_Absyn_eq_exp(
e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* _temp1921=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1883->fields))->v; for( 0; _temp1921 != 0; _temp1921=(( struct
Cyc_List_List*) _check_null( _temp1921))->tl){ struct Cyc_Absyn_Tunionfield*
_temp1922=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp1921))->hd; if( _temp1922->typs == 0){ max_tag ++;}}}{ struct Cyc_Absyn_Exp*
max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_t, r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp(
_temp1881->name, 0); test_exp= max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp(
e1, e, 0), 0): Cyc_Absyn_eq_exp( e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt(
test_exp, Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);
goto _LL1809;}} _LL1831: _temp1887= _temp1885; goto _LL1833; _LL1833: { struct
Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1887);{ struct Cyc_List_List* _temp1923=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1887); for(
0; _temp1923 != 0;( _temp1923=(( struct Cyc_List_List*) _check_null( _temp1923))->tl,
cnt --)){ struct Cyc_Absyn_Pat* _temp1924=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp1923))->hd; if(( void*) _temp1924->r ==( void*) Cyc_Absyn_Wild_p){
continue;}{ struct _tuple0* _temp1925= Cyc_Toc_temp_var(); void* _temp1926=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1924->topt))->v; struct
_tagged_arr* _temp1927= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1928=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1928->hd=( void*)({ struct _tuple12* _temp1929=( struct _tuple12*)
_region_malloc( rgn, sizeof( struct _tuple12)); _temp1929->f1= _temp1925;
_temp1929->f2= Cyc_Toc_typ_to_c( _temp1926); _temp1929;}); _temp1928->tl= decls;
_temp1928;});{ struct _tuple11 _temp1930= Cyc_Toc_xlate_pat( nv, rgn, _temp1926,
Cyc_Absyn_var_exp( _temp1925, 0), Cyc_Absyn_structmember_exp( path, Cyc_Toc_fieldname(
cnt), 0), _temp1924, succ_lab, fail_lab, decls); nv= _temp1930.f1; decls=
_temp1930.f2;{ struct Cyc_Absyn_Stmt* _temp1931= _temp1930.f3; struct Cyc_Absyn_Stmt*
_temp1932= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp1925, 0), Cyc_Absyn_structmember_exp( r, Cyc_Toc_fieldname( cnt), 0), 0),
_temp1931, 0); ss=({ struct Cyc_List_List* _temp1933=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1933->hd=( void*) Cyc_Absyn_label_stmt(
_temp1927, _temp1932, 0); _temp1933->tl= ss; _temp1933;}); succ_lab= _temp1927;}}}}}
if( ss == 0){ ss=({ struct Cyc_List_List* _temp1934=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1934->hd=( void*) Cyc_Absyn_goto_stmt(
succ_lab, 0); _temp1934->tl= 0; _temp1934;});} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL1809;} _LL1835: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
_temp1935=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp1889); for( 0; _temp1935 != 0; _temp1935=(( struct
Cyc_List_List*) _check_null( _temp1935))->tl){ struct _tuple13* _temp1936=(
struct _tuple13*)(( struct Cyc_List_List*) _check_null( _temp1935))->hd; struct
Cyc_Absyn_Pat* _temp1937=(* _temp1936).f2; if(( void*) _temp1937->r ==( void*)
Cyc_Absyn_Wild_p){ continue;}{ struct _tagged_arr* f;{ void* _temp1938=( void*)((
struct Cyc_List_List*) _check_null((* _temp1936).f1))->hd; struct _tagged_arr*
_temp1944; _LL1940: if(*(( int*) _temp1938) == Cyc_Absyn_FieldName){ _LL1945:
_temp1944=(( struct Cyc_Absyn_FieldName_struct*) _temp1938)->f1; goto _LL1941;}
else{ goto _LL1942;} _LL1942: goto _LL1943; _LL1941: f= _temp1944; goto _LL1939;
_LL1943:( int) _throw(( void*) Cyc_Toc_Match_error); _LL1939:;}{ struct _tuple0*
_temp1946= Cyc_Toc_temp_var(); void* _temp1947=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1937->topt))->v; void* _temp1948= Cyc_Toc_typ_to_c( _temp1947);
struct _tagged_arr* _temp1949= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1950=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1950->hd=( void*)({ struct _tuple12* _temp1951=( struct _tuple12*)
_region_malloc( rgn, sizeof( struct _tuple12)); _temp1951->f1= _temp1946;
_temp1951->f2= _temp1948; _temp1951;}); _temp1950->tl= decls; _temp1950;});{
struct _tuple11 _temp1952= Cyc_Toc_xlate_pat( nv, rgn, _temp1947, Cyc_Absyn_var_exp(
_temp1946, 0), Cyc_Absyn_structmember_exp( path, f, 0), _temp1937, succ_lab,
fail_lab, decls); nv= _temp1952.f1; decls= _temp1952.f2;{ struct Cyc_Absyn_Exp*
_temp1953= Cyc_Absyn_structmember_exp( r, f, 0); if( Cyc_Toc_is_void_star(( void*)((
struct Cyc_Absyn_Structfield*) _check_null( Cyc_Absyn_lookup_field(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1895->fields))->v, f)))->type)){
_temp1953= Cyc_Absyn_cast_exp( _temp1948, _temp1953, 0);}{ struct Cyc_Absyn_Stmt*
_temp1954= _temp1952.f3; struct Cyc_Absyn_Stmt* _temp1955= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1946, 0), _temp1953, 0),
_temp1954, 0); ss=({ struct Cyc_List_List* _temp1956=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1956->hd=( void*) Cyc_Absyn_label_stmt(
_temp1949, _temp1955, 0); _temp1956->tl= ss; _temp1956;}); succ_lab= _temp1949;}}}}}}}
if( ss == 0){ ss=({ struct Cyc_List_List* _temp1957=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1957->hd=( void*) Cyc_Absyn_goto_stmt(
succ_lab, 0); _temp1957->tl= 0; _temp1957;});} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL1809;} _LL1837: { struct _tuple0* _temp1958= Cyc_Toc_temp_var(); void*
_temp1959=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1897->topt))->v;
decls=({ struct Cyc_List_List* _temp1960=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp1960->hd=( void*)({ struct _tuple12*
_temp1961=( struct _tuple12*) _region_malloc( rgn, sizeof( struct _tuple12));
_temp1961->f1= _temp1958; _temp1961->f2= Cyc_Toc_typ_to_c( _temp1959); _temp1961;});
_temp1960->tl= decls; _temp1960;});{ struct _tuple11 _temp1962= Cyc_Toc_xlate_pat(
nv, rgn, _temp1959, Cyc_Absyn_var_exp( _temp1958, 0), Cyc_Absyn_deref_exp( path,
0), _temp1897, succ_lab, fail_lab, decls); nv= _temp1962.f1; decls= _temp1962.f2;{
struct Cyc_Absyn_Stmt* _temp1963= _temp1962.f3; struct Cyc_Absyn_Stmt* _temp1964=
Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1958, 0), Cyc_Absyn_deref_exp(
r, 0), 0), _temp1963, 0); if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp1964, 0);} else{ s= _temp1964;} goto _LL1809;}}} _LL1839: s=({
struct Cyc_Absyn_Stmt*(* _temp1965)( struct _tagged_arr fmt, struct _tagged_arr
ap)=( struct Cyc_Absyn_Stmt*(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos; struct _tagged_arr _temp1968= _tag_arr("unknownid", sizeof(
unsigned char), 10u); void* _temp1966[ 0u]={}; struct _tagged_arr _temp1967={(
void*) _temp1966,( void*) _temp1966,( void*)( _temp1966 + 0u)}; _temp1965(
_temp1968, _temp1967);}); goto _LL1809; _LL1841: s=({ struct Cyc_Absyn_Stmt*(*
_temp1969)( struct _tagged_arr fmt, struct _tagged_arr ap)=( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos; struct
_tagged_arr _temp1972= _tag_arr("unknowncall", sizeof( unsigned char), 12u);
void* _temp1970[ 0u]={}; struct _tagged_arr _temp1971={( void*) _temp1970,( void*)
_temp1970,( void*)( _temp1970 + 0u)}; _temp1969( _temp1972, _temp1971);}); goto
_LL1809; _LL1843: s=({ struct Cyc_Absyn_Stmt*(* _temp1973)( struct _tagged_arr
fmt, struct _tagged_arr ap)=( struct Cyc_Absyn_Stmt*(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos; struct _tagged_arr _temp1976=
_tag_arr("unknownfields", sizeof( unsigned char), 14u); void* _temp1974[ 0u]={};
struct _tagged_arr _temp1975={( void*) _temp1974,( void*) _temp1974,( void*)(
_temp1974 + 0u)}; _temp1973( _temp1976, _temp1975);}); goto _LL1809; _LL1809:;}
return({ struct _tuple11 _temp1977; _temp1977.f1= nv; _temp1977.f2= decls;
_temp1977.f3= s; _temp1977;});} struct _tuple14{ struct _tagged_arr* f1; struct
_tagged_arr* f2; struct Cyc_Absyn_Switch_clause* f3; } ; static struct _tuple14*
Cyc_Toc_gen_label( struct _RegionHandle* r, struct Cyc_Absyn_Switch_clause* sc){
return({ struct _tuple14* _temp1978=( struct _tuple14*) _region_malloc( r,
sizeof( struct _tuple14)); _temp1978->f1= Cyc_Toc_fresh_label(); _temp1978->f2=
Cyc_Toc_fresh_label(); _temp1978->f3= sc; _temp1978;});} static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* scs){ Cyc_Toc_exp_to_c( nv, e);{ void* _temp1979=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; int leave_as_switch;{ void*
_temp1980= Cyc_Tcutil_compress( _temp1979); _LL1982: if(( unsigned int)
_temp1980 > 4u?*(( int*) _temp1980) == Cyc_Absyn_IntType: 0){ goto _LL1983;}
else{ goto _LL1984;} _LL1984: goto _LL1985; _LL1983: leave_as_switch= 1; goto
_LL1981; _LL1985: leave_as_switch= 0; goto _LL1981; _LL1981:;}{ struct Cyc_List_List*
_temp1986= scs; for( 0; _temp1986 != 0; _temp1986=(( struct Cyc_List_List*)
_check_null( _temp1986))->tl){ if(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp1986))->hd)->pat_vars))->v != 0? 1:(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp1986))->hd)->where_clause != 0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_arr* next_l=
Cyc_Toc_fresh_label();{ struct Cyc_List_List* _temp1987= scs; for( 0; _temp1987
!= 0; _temp1987=(( struct Cyc_List_List*) _check_null( _temp1987))->tl){ struct
Cyc_Absyn_Stmt* _temp1988=(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp1987))->hd)->body;(( struct Cyc_Absyn_Switch_clause*)(( struct
Cyc_List_List*) _check_null( _temp1987))->hd)->body= Cyc_Absyn_label_stmt(
next_l, _temp1988, 0); next_l= Cyc_Toc_fresh_label(); Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env(
nv, next_l), _temp1988);}} return;}{ struct _tuple0* v= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp( v, 0); struct Cyc_Absyn_Exp* path=
Cyc_Absyn_var_exp( v, 0); struct _tagged_arr* end_l= Cyc_Toc_fresh_label();
struct _RegionHandle _temp1989= _new_region(); struct _RegionHandle* rgn=&
_temp1989; _push_region( rgn);{ struct Cyc_List_List* lscs=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple14*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Toc_gen_label,
rgn, scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0;
struct Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp1990= lscs; for( 0;
_temp1990 != 0; _temp1990=(( struct Cyc_List_List*) _check_null( _temp1990))->tl){
struct Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null( _temp1990))->hd)).f3; struct _tagged_arr* fail_lab=(( struct Cyc_List_List*)
_check_null( _temp1990))->tl == 0? end_l:(*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1990))->tl))->hd)).f1;
struct _tagged_arr* succ_lab=(*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null( _temp1990))->hd)).f2; if( sc->where_clause == 0){ struct _tuple11
_temp1991= Cyc_Toc_xlate_pat( nv, rgn, _temp1979, r, path, sc->pattern, succ_lab,
fail_lab, decls); nvs=({ struct Cyc_List_List* _temp1992=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1992->hd=( void*)
_temp1991.f1; _temp1992->tl= nvs; _temp1992;}); decls= _temp1991.f2; test_stmts=({
struct Cyc_List_List* _temp1993=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp1993->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( _temp1990))->hd)).f1,
_temp1991.f3, 0); _temp1993->tl= test_stmts; _temp1993;});} else{ struct Cyc_Absyn_Exp*
_temp1994=( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause); struct
_tagged_arr* _temp1995= Cyc_Toc_fresh_label(); struct _tuple11 _temp1996= Cyc_Toc_xlate_pat(
nv, rgn, _temp1979, r, path, sc->pattern, _temp1995, fail_lab, decls); Cyc_Toc_exp_to_c(
_temp1996.f1, _temp1994); nvs=({ struct Cyc_List_List* _temp1997=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1997->hd=( void*)
_temp1996.f1; _temp1997->tl= nvs; _temp1997;}); decls= _temp1996.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp1994, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp1995, s2,
0); test_stmts=({ struct Cyc_List_List* _temp1998=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1998->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( _temp1990))->hd)).f1,
Cyc_Absyn_seq_stmt( _temp1996.f3, s3, 0), 0); _temp1998->tl= test_stmts;
_temp1998;});}}}}{ struct Cyc_Absyn_Stmt* _temp1999= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
test_stmts), 0); nvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt( 0); if( lscs != 0){
for( 0;(( struct Cyc_List_List*) _check_null( lscs))->tl != 0;( lscs=(( struct
Cyc_List_List*) _check_null( lscs))->tl, nvs=(( struct Cyc_List_List*)
_check_null( nvs))->tl)){ struct Cyc_Toc_Env* _temp2000=( struct Cyc_Toc_Env*)((
struct Cyc_List_List*) _check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env( _temp2000, end_l,(*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( lscs))->tl))->hd)).f2,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null(((*(( struct _tuple14*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f3)->pat_vars))->v,(
struct Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( nvs))->tl))->hd), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0),
0);}{ struct Cyc_Toc_Env* _temp2001=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*(( struct _tuple14*)((
struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body; Cyc_Toc_stmt_to_c(
Cyc_Toc_last_switchclause_env( _temp2001, end_l), s); clauses= Cyc_Absyn_seq_stmt(
clauses, Cyc_Absyn_label_stmt((*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null( lscs))->hd)).f2, s, 0), 0);}}{ struct Cyc_Absyn_Stmt* res= Cyc_Absyn_seq_stmt(
_temp1999, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt( end_l, Cyc_Absyn_skip_stmt(
0), 0), 0), 0); for( decls; decls != 0; decls=(( struct Cyc_List_List*)
_check_null( decls))->tl){ struct _tuple12 _temp2004; void* _temp2005; struct
_tuple0* _temp2007; struct _tuple12* _temp2002=( struct _tuple12*)(( struct Cyc_List_List*)
_check_null( decls))->hd; _temp2004=* _temp2002; _LL2008: _temp2007= _temp2004.f1;
goto _LL2006; _LL2006: _temp2005= _temp2004.f2; goto _LL2003; _LL2003: res= Cyc_Absyn_declare_stmt(
_temp2007, _temp2005, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}; _pop_region( rgn);}}} static
struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat*
p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt*
s); static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv, struct Cyc_Toc_Env*
init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt* s); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__npop_handler_e,({ struct Cyc_List_List* _temp2009=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp2009[ 0]=({ struct Cyc_List_List
_temp2010; _temp2010.hd=( void*) Cyc_Absyn_uint_exp(( unsigned int)( n - 1), 0);
_temp2010.tl= 0; _temp2010;}); _temp2009;}), 0), 0);} void Cyc_Toc_do_npop_before(
int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt(( void*) s->r,
0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* s){ while( 1) { void* _temp2011=( void*) s->r; struct Cyc_Absyn_Exp*
_temp2053; struct Cyc_Absyn_Stmt* _temp2055; struct Cyc_Absyn_Stmt* _temp2057;
struct Cyc_Absyn_Exp* _temp2059; struct Cyc_Absyn_Stmt* _temp2061; struct Cyc_Absyn_Stmt*
_temp2063; struct Cyc_Absyn_Exp* _temp2065; struct Cyc_Absyn_Stmt* _temp2067;
struct _tuple2 _temp2069; struct Cyc_Absyn_Exp* _temp2071; struct Cyc_Absyn_Stmt*
_temp2073; struct Cyc_Absyn_Stmt* _temp2075; struct Cyc_Absyn_Stmt* _temp2077;
struct Cyc_Absyn_Stmt* _temp2079; struct _tuple2 _temp2081; struct Cyc_Absyn_Exp*
_temp2083; struct _tuple2 _temp2085; struct Cyc_Absyn_Exp* _temp2087; struct Cyc_Absyn_Exp*
_temp2089; struct Cyc_List_List* _temp2091; struct Cyc_Absyn_Exp* _temp2093;
struct Cyc_Absyn_Switch_clause** _temp2095; struct Cyc_List_List* _temp2097;
struct Cyc_Absyn_Stmt* _temp2099; struct Cyc_Absyn_Decl* _temp2101; struct Cyc_Absyn_Stmt*
_temp2103; struct _tagged_arr* _temp2105; struct _tuple2 _temp2107; struct Cyc_Absyn_Exp*
_temp2109; struct Cyc_Absyn_Stmt* _temp2111; struct Cyc_List_List* _temp2113;
struct Cyc_Absyn_Stmt* _temp2115; struct Cyc_Absyn_Stmt* _temp2117; struct Cyc_Absyn_Vardecl*
_temp2119; struct Cyc_Absyn_Tvar* _temp2121; _LL2013: if( _temp2011 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL2014;} else{ goto _LL2015;} _LL2015: if((
unsigned int) _temp2011 > 1u?*(( int*) _temp2011) == Cyc_Absyn_Exp_s: 0){
_LL2054: _temp2053=(( struct Cyc_Absyn_Exp_s_struct*) _temp2011)->f1; goto
_LL2016;} else{ goto _LL2017;} _LL2017: if(( unsigned int) _temp2011 > 1u?*((
int*) _temp2011) == Cyc_Absyn_Seq_s: 0){ _LL2058: _temp2057=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2011)->f1; goto _LL2056; _LL2056: _temp2055=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2011)->f2; goto _LL2018;} else{ goto _LL2019;} _LL2019: if(( unsigned int)
_temp2011 > 1u?*(( int*) _temp2011) == Cyc_Absyn_Return_s: 0){ _LL2060:
_temp2059=(( struct Cyc_Absyn_Return_s_struct*) _temp2011)->f1; goto _LL2020;}
else{ goto _LL2021;} _LL2021: if(( unsigned int) _temp2011 > 1u?*(( int*)
_temp2011) == Cyc_Absyn_IfThenElse_s: 0){ _LL2066: _temp2065=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2011)->f1; goto _LL2064; _LL2064: _temp2063=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2011)->f2; goto _LL2062; _LL2062: _temp2061=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2011)->f3; goto _LL2022;} else{ goto _LL2023;} _LL2023: if(( unsigned int)
_temp2011 > 1u?*(( int*) _temp2011) == Cyc_Absyn_While_s: 0){ _LL2070: _temp2069=((
struct Cyc_Absyn_While_s_struct*) _temp2011)->f1; _LL2072: _temp2071= _temp2069.f1;
goto _LL2068; _LL2068: _temp2067=(( struct Cyc_Absyn_While_s_struct*) _temp2011)->f2;
goto _LL2024;} else{ goto _LL2025;} _LL2025: if(( unsigned int) _temp2011 > 1u?*((
int*) _temp2011) == Cyc_Absyn_Break_s: 0){ _LL2074: _temp2073=(( struct Cyc_Absyn_Break_s_struct*)
_temp2011)->f1; goto _LL2026;} else{ goto _LL2027;} _LL2027: if(( unsigned int)
_temp2011 > 1u?*(( int*) _temp2011) == Cyc_Absyn_Continue_s: 0){ _LL2076:
_temp2075=(( struct Cyc_Absyn_Continue_s_struct*) _temp2011)->f1; goto _LL2028;}
else{ goto _LL2029;} _LL2029: if(( unsigned int) _temp2011 > 1u?*(( int*)
_temp2011) == Cyc_Absyn_Goto_s: 0){ _LL2078: _temp2077=(( struct Cyc_Absyn_Goto_s_struct*)
_temp2011)->f2; goto _LL2030;} else{ goto _LL2031;} _LL2031: if(( unsigned int)
_temp2011 > 1u?*(( int*) _temp2011) == Cyc_Absyn_For_s: 0){ _LL2090: _temp2089=((
struct Cyc_Absyn_For_s_struct*) _temp2011)->f1; goto _LL2086; _LL2086: _temp2085=((
struct Cyc_Absyn_For_s_struct*) _temp2011)->f2; _LL2088: _temp2087= _temp2085.f1;
goto _LL2082; _LL2082: _temp2081=(( struct Cyc_Absyn_For_s_struct*) _temp2011)->f3;
_LL2084: _temp2083= _temp2081.f1; goto _LL2080; _LL2080: _temp2079=(( struct Cyc_Absyn_For_s_struct*)
_temp2011)->f4; goto _LL2032;} else{ goto _LL2033;} _LL2033: if(( unsigned int)
_temp2011 > 1u?*(( int*) _temp2011) == Cyc_Absyn_Switch_s: 0){ _LL2094:
_temp2093=(( struct Cyc_Absyn_Switch_s_struct*) _temp2011)->f1; goto _LL2092;
_LL2092: _temp2091=(( struct Cyc_Absyn_Switch_s_struct*) _temp2011)->f2; goto
_LL2034;} else{ goto _LL2035;} _LL2035: if(( unsigned int) _temp2011 > 1u?*((
int*) _temp2011) == Cyc_Absyn_Fallthru_s: 0){ _LL2098: _temp2097=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2011)->f1; goto _LL2096; _LL2096: _temp2095=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2011)->f2; goto _LL2036;} else{ goto _LL2037;} _LL2037: if(( unsigned int)
_temp2011 > 1u?*(( int*) _temp2011) == Cyc_Absyn_Decl_s: 0){ _LL2102: _temp2101=((
struct Cyc_Absyn_Decl_s_struct*) _temp2011)->f1; goto _LL2100; _LL2100:
_temp2099=(( struct Cyc_Absyn_Decl_s_struct*) _temp2011)->f2; goto _LL2038;}
else{ goto _LL2039;} _LL2039: if(( unsigned int) _temp2011 > 1u?*(( int*)
_temp2011) == Cyc_Absyn_Label_s: 0){ _LL2106: _temp2105=(( struct Cyc_Absyn_Label_s_struct*)
_temp2011)->f1; goto _LL2104; _LL2104: _temp2103=(( struct Cyc_Absyn_Label_s_struct*)
_temp2011)->f2; goto _LL2040;} else{ goto _LL2041;} _LL2041: if(( unsigned int)
_temp2011 > 1u?*(( int*) _temp2011) == Cyc_Absyn_Do_s: 0){ _LL2112: _temp2111=((
struct Cyc_Absyn_Do_s_struct*) _temp2011)->f1; goto _LL2108; _LL2108: _temp2107=((
struct Cyc_Absyn_Do_s_struct*) _temp2011)->f2; _LL2110: _temp2109= _temp2107.f1;
goto _LL2042;} else{ goto _LL2043;} _LL2043: if(( unsigned int) _temp2011 > 1u?*((
int*) _temp2011) == Cyc_Absyn_TryCatch_s: 0){ _LL2116: _temp2115=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2011)->f1; goto _LL2114; _LL2114: _temp2113=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2011)->f2; goto _LL2044;} else{ goto _LL2045;} _LL2045: if(( unsigned int)
_temp2011 > 1u?*(( int*) _temp2011) == Cyc_Absyn_Region_s: 0){ _LL2122:
_temp2121=(( struct Cyc_Absyn_Region_s_struct*) _temp2011)->f1; goto _LL2120;
_LL2120: _temp2119=(( struct Cyc_Absyn_Region_s_struct*) _temp2011)->f2; goto
_LL2118; _LL2118: _temp2117=(( struct Cyc_Absyn_Region_s_struct*) _temp2011)->f3;
goto _LL2046;} else{ goto _LL2047;} _LL2047: if(( unsigned int) _temp2011 > 1u?*((
int*) _temp2011) == Cyc_Absyn_SwitchC_s: 0){ goto _LL2048;} else{ goto _LL2049;}
_LL2049: if(( unsigned int) _temp2011 > 1u?*(( int*) _temp2011) == Cyc_Absyn_Cut_s:
0){ goto _LL2050;} else{ goto _LL2051;} _LL2051: if(( unsigned int) _temp2011 >
1u?*(( int*) _temp2011) == Cyc_Absyn_Splice_s: 0){ goto _LL2052;} else{ goto
_LL2012;} _LL2014: return; _LL2016: Cyc_Toc_exp_to_c( nv, _temp2053); return;
_LL2018: Cyc_Toc_stmt_to_c( nv, _temp2057); s= _temp2055; continue; _LL2020: {
struct Cyc_Core_Opt* topt= 0; if( _temp2059 != 0){ topt=({ struct Cyc_Core_Opt*
_temp2123=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2123->v=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*) _check_null( _temp2059))->topt))->v);
_temp2123;}); Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null(
_temp2059));} if( s->try_depth > 0){ if( topt != 0){ struct _tuple0* _temp2124=
Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* _temp2125= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( _temp2124, 0), 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( _temp2124,( void*)(( struct Cyc_Core_Opt*)
_check_null( topt))->v, _temp2059, Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler(
s->try_depth), _temp2125, 0), 0))->r));} else{ Cyc_Toc_do_npop_before( s->try_depth,
s);}} return;} _LL2022: Cyc_Toc_exp_to_c( nv, _temp2065); Cyc_Toc_stmt_to_c( nv,
_temp2063); s= _temp2061; continue; _LL2024: Cyc_Toc_exp_to_c( nv, _temp2071);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2067); return; _LL2026: if( nv->break_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( nv->break_lab))->v, 0))->r));}{ int
dest_depth= _temp2073 == 0? 0:(( struct Cyc_Absyn_Stmt*) _check_null( _temp2073))->try_depth;
Cyc_Toc_do_npop_before( s->try_depth - dest_depth, s); return;} _LL2028: if( nv->continue_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( nv->continue_lab))->v, 0))->r));} _temp2077=
_temp2075; goto _LL2030; _LL2030: Cyc_Toc_do_npop_before( s->try_depth -((
struct Cyc_Absyn_Stmt*) _check_null( _temp2077))->try_depth, s); return; _LL2032:
Cyc_Toc_exp_to_c( nv, _temp2089); Cyc_Toc_exp_to_c( nv, _temp2087); Cyc_Toc_exp_to_c(
nv, _temp2083); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2079); return;
_LL2034: Cyc_Toc_xlate_switch( nv, s, _temp2093, _temp2091); return; _LL2036:
if( nv->fallthru_info == 0){( int) _throw(({ void*(* _temp2126)( struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Toc_toc_impos; struct _tagged_arr
_temp2129= _tag_arr("fallthru in unexpected place", sizeof( unsigned char), 29u);
void* _temp2127[ 0u]={}; struct _tagged_arr _temp2128={( void*) _temp2127,( void*)
_temp2127,( void*)( _temp2127 + 0u)}; _temp2126( _temp2129, _temp2128);}));}{
struct _tuple6 _temp2132; struct Cyc_Dict_Dict* _temp2133; struct Cyc_List_List*
_temp2135; struct _tagged_arr* _temp2137; struct _tuple6* _temp2130=( struct
_tuple6*)(( struct Cyc_Core_Opt*) _check_null( nv->fallthru_info))->v; _temp2132=*
_temp2130; _LL2138: _temp2137= _temp2132.f1; goto _LL2136; _LL2136: _temp2135=
_temp2132.f2; goto _LL2134; _LL2134: _temp2133= _temp2132.f3; goto _LL2131;
_LL2131: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt( _temp2137, 0); Cyc_Toc_do_npop_before(
s->try_depth -((*(( struct Cyc_Absyn_Switch_clause**) _check_null( _temp2095)))->body)->try_depth,
s2);{ struct Cyc_List_List* _temp2139=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp2135); struct Cyc_List_List* _temp2140=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( _temp2097); for( 0; _temp2139 != 0;(
_temp2139=(( struct Cyc_List_List*) _check_null( _temp2139))->tl, _temp2140=((
struct Cyc_List_List*) _check_null( _temp2140))->tl)){ Cyc_Toc_exp_to_c( nv,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp2140))->hd);
s2= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( _temp2133,(
struct _tuple0*)(( struct Cyc_List_List*) _check_null( _temp2139))->hd),( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp2140))->hd, 0), s2, 0);}(
void*)( s->r=( void*)(( void*) s2->r)); return;}}} _LL2038:{ void* _temp2141=(
void*) _temp2101->r; struct Cyc_Absyn_Vardecl* _temp2151; int _temp2153; struct
Cyc_Absyn_Exp* _temp2155; struct Cyc_Core_Opt* _temp2157; struct Cyc_Absyn_Pat*
_temp2159; struct Cyc_List_List* _temp2161; _LL2143: if(*(( int*) _temp2141) ==
Cyc_Absyn_Var_d){ _LL2152: _temp2151=(( struct Cyc_Absyn_Var_d_struct*)
_temp2141)->f1; goto _LL2144;} else{ goto _LL2145;} _LL2145: if(*(( int*)
_temp2141) == Cyc_Absyn_Let_d){ _LL2160: _temp2159=(( struct Cyc_Absyn_Let_d_struct*)
_temp2141)->f1; goto _LL2158; _LL2158: _temp2157=(( struct Cyc_Absyn_Let_d_struct*)
_temp2141)->f3; goto _LL2156; _LL2156: _temp2155=(( struct Cyc_Absyn_Let_d_struct*)
_temp2141)->f4; goto _LL2154; _LL2154: _temp2153=(( struct Cyc_Absyn_Let_d_struct*)
_temp2141)->f5; goto _LL2146;} else{ goto _LL2147;} _LL2147: if(*(( int*)
_temp2141) == Cyc_Absyn_Letv_d){ _LL2162: _temp2161=(( struct Cyc_Absyn_Letv_d_struct*)
_temp2141)->f1; goto _LL2148;} else{ goto _LL2149;} _LL2149: goto _LL2150;
_LL2144: { struct Cyc_Toc_Env* _temp2163= Cyc_Toc_add_varmap( nv, _temp2151->name,
Cyc_Absyn_varb_exp( _temp2151->name,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp2164=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp2164[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp2165; _temp2165.tag= Cyc_Absyn_Local_b;
_temp2165.f1= _temp2151; _temp2165;}); _temp2164;}), 0)); Cyc_Toc_local_decl_to_c(
_temp2163, _temp2163, _temp2151, _temp2099); goto _LL2142;} _LL2146:{ void*
_temp2166=( void*) _temp2159->r; struct Cyc_Absyn_Vardecl* _temp2172; _LL2168:
if(( unsigned int) _temp2166 > 2u?*(( int*) _temp2166) == Cyc_Absyn_Var_p: 0){
_LL2173: _temp2172=(( struct Cyc_Absyn_Var_p_struct*) _temp2166)->f1; goto
_LL2169;} else{ goto _LL2170;} _LL2170: goto _LL2171; _LL2169: { struct _tuple0*
old_name= _temp2172->name; struct _tuple0* new_name= Cyc_Toc_temp_var();
_temp2172->name= new_name; _temp2172->initializer=( struct Cyc_Absyn_Exp*)
_temp2155;( void*)( _temp2101->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2174=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2174[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2175; _temp2175.tag= Cyc_Absyn_Var_d;
_temp2175.f1= _temp2172; _temp2175;}); _temp2174;})));{ struct Cyc_Toc_Env*
_temp2176= Cyc_Toc_add_varmap( nv, old_name, Cyc_Absyn_varb_exp( new_name,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2177=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2177[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2178; _temp2178.tag= Cyc_Absyn_Local_b; _temp2178.f1=
_temp2172; _temp2178;}); _temp2177;}), 0)); Cyc_Toc_local_decl_to_c( _temp2176,
nv, _temp2172, _temp2099); goto _LL2167;}} _LL2171:( void*)( s->r=( void*)((
void*)( Cyc_Toc_letdecl_to_c( nv, _temp2159, _temp2157, _temp2155, _temp2153,
_temp2099))->r)); goto _LL2167; _LL2167:;} goto _LL2142; _LL2148: { struct Cyc_List_List*
_temp2179=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2161); if( _temp2179 == 0){({ int(* _temp2180)( struct _tagged_arr fmt,
struct _tagged_arr ap)=( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos; struct _tagged_arr _temp2183= _tag_arr("empty Letv_d", sizeof(
unsigned char), 13u); void* _temp2181[ 0u]={}; struct _tagged_arr _temp2182={(
void*) _temp2181,( void*) _temp2181,( void*)( _temp2181 + 0u)}; _temp2180(
_temp2183, _temp2182);});}( void*)( _temp2101->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2184=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2184[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2185; _temp2185.tag= Cyc_Absyn_Var_d;
_temp2185.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp2179))->hd; _temp2185;}); _temp2184;}))); _temp2179=(( struct Cyc_List_List*)
_check_null( _temp2179))->tl; for( 0; _temp2179 != 0; _temp2179=(( struct Cyc_List_List*)
_check_null( _temp2179))->tl){ struct Cyc_Absyn_Decl* _temp2186= Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp2187=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2187[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2188; _temp2188.tag= Cyc_Absyn_Var_d; _temp2188.f1=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp2179))->hd; _temp2188;}); _temp2187;}),
0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_decl_stmt( _temp2186, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));} Cyc_Toc_stmt_to_c( nv, s); goto _LL2142;} _LL2150:({
int(* _temp2189)( struct _tagged_arr fmt, struct _tagged_arr ap)=( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos; struct
_tagged_arr _temp2192= _tag_arr("bad nested declaration within function",
sizeof( unsigned char), 39u); void* _temp2190[ 0u]={}; struct _tagged_arr
_temp2191={( void*) _temp2190,( void*) _temp2190,( void*)( _temp2190 + 0u)};
_temp2189( _temp2192, _temp2191);}); goto _LL2142; _LL2142:;} return; _LL2040: s=
_temp2103; continue; _LL2042: Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv),
_temp2111); Cyc_Toc_exp_to_c( nv, _temp2109); return; _LL2044: { struct _tuple0*
h_var= Cyc_Toc_temp_var(); struct _tuple0* e_var= Cyc_Toc_temp_var(); struct
_tuple0* was_thrown_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp= Cyc_Absyn_var_exp(
h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp( e_var, 0); struct Cyc_Absyn_Exp*
was_thrown_exp= Cyc_Absyn_var_exp( was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp); void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ);
void* was_thrown_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_sint_t); e_exp->topt=({ struct
Cyc_Core_Opt* _temp2193=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2193->v=( void*) e_typ; _temp2193;}); nv= Cyc_Toc_add_varmap( nv, e_var,
e_exp); Cyc_Toc_stmt_to_c( nv, _temp2115);{ struct Cyc_Absyn_Stmt* _temp2194=
Cyc_Absyn_seq_stmt( _temp2115, Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__pop_handler_e,
0, 0), 0), 0); struct _tuple0* _temp2195= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp2196= Cyc_Absyn_var_exp( _temp2195, 0); struct Cyc_Absyn_Vardecl* _temp2197=
Cyc_Absyn_new_vardecl( _temp2195, Cyc_Absyn_exn_typ, 0); _temp2196->topt=({
struct Cyc_Core_Opt* _temp2198=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2198->v=( void*) Cyc_Absyn_exn_typ; _temp2198;});{ struct
Cyc_Absyn_Pat* _temp2199=({ struct Cyc_Absyn_Pat* _temp2215=( struct Cyc_Absyn_Pat*)
GC_malloc( sizeof( struct Cyc_Absyn_Pat)); _temp2215->r=( void*)(( void*)({
struct Cyc_Absyn_Var_p_struct* _temp2217=( struct Cyc_Absyn_Var_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct)); _temp2217[ 0]=({ struct Cyc_Absyn_Var_p_struct
_temp2218; _temp2218.tag= Cyc_Absyn_Var_p; _temp2218.f1= _temp2197; _temp2218;});
_temp2217;})); _temp2215->topt=({ struct Cyc_Core_Opt* _temp2216=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2216->v=( void*) Cyc_Absyn_exn_typ;
_temp2216;}); _temp2215->loc= 0; _temp2215;}); struct Cyc_Absyn_Exp* _temp2200=
Cyc_Absyn_throw_exp( _temp2196, 0); _temp2200->topt=({ struct Cyc_Core_Opt*
_temp2201=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2201->v=( void*)(( void*) Cyc_Absyn_VoidType); _temp2201;});{ struct Cyc_Absyn_Stmt*
_temp2202= Cyc_Absyn_exp_stmt( _temp2200, 0); struct Cyc_Absyn_Switch_clause*
_temp2203=({ struct Cyc_Absyn_Switch_clause* _temp2212=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2212->pattern=
_temp2199; _temp2212->pat_vars=({ struct Cyc_Core_Opt* _temp2213=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2213->v=( void*)({ struct Cyc_List_List*
_temp2214=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2214->hd=( void*) _temp2197; _temp2214->tl= 0; _temp2214;}); _temp2213;});
_temp2212->where_clause= 0; _temp2212->body= _temp2202; _temp2212->loc= 0;
_temp2212;}); struct Cyc_Absyn_Stmt* _temp2204= Cyc_Absyn_switch_stmt( e_exp,((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp2113,({ struct Cyc_List_List* _temp2211=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2211->hd=( void*) _temp2203; _temp2211->tl=
0; _temp2211;})), 0); Cyc_Toc_stmt_to_c( nv, _temp2204);{ struct Cyc_Absyn_Exp*
_temp2205= Cyc_Absyn_fncall_exp( Cyc_Toc_setjmp_e,({ struct Cyc_List_List*
_temp2210=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2210->hd=( void*) Cyc_Absyn_structmember_exp( h_exp, Cyc_Toc_handler_sp, 0);
_temp2210->tl= 0; _temp2210;}), 0); struct Cyc_Absyn_Stmt* _temp2206= Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( Cyc_Toc__push_handler_e,({ struct Cyc_List_List* _temp2209=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2209->hd=(
void*) Cyc_Absyn_address_exp( h_exp, 0); _temp2209->tl= 0; _temp2209;}), 0), 0);
struct Cyc_Absyn_Exp* _temp2207= Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, 0,
0); struct Cyc_Absyn_Exp* _temp2208= Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed,
1, 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt( h_var, h_typ, 0,
Cyc_Absyn_seq_stmt( _temp2206, Cyc_Absyn_declare_stmt( was_thrown_var,
was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp2207, Cyc_Absyn_seq_stmt( Cyc_Absyn_ifthenelse_stmt(
_temp2205, Cyc_Absyn_assign_stmt( was_thrown_exp, _temp2208, 0), Cyc_Absyn_skip_stmt(
0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Not,
was_thrown_exp, 0), _temp2194, Cyc_Absyn_declare_stmt( e_var, e_typ,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_cast_exp( e_typ, Cyc_Toc__exn_thrown_e, 0), _temp2204, 0), 0), 0), 0),
0), 0))->r)); return;}}}}} _LL2046: { void* rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp);
void* rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq);
struct _tuple0* rh_var= Cyc_Toc_temp_var(); struct _tuple0* x_var= _temp2119->name;
struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp( rh_var, 0); struct Cyc_Absyn_Exp*
x_exp= Cyc_Absyn_var_exp( x_var, 0); Cyc_Toc_stmt_to_c( Cyc_Toc_add_varmap( nv,
x_var, x_exp), _temp2117);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_fncall_exp( Cyc_Toc__new_region_e,
0, 0), Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp( rh_exp, 0), Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__push_region_e,({ struct Cyc_List_List*(* _temp2219)( struct _tagged_arr)=(
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp*
_temp2222= x_exp; struct Cyc_Absyn_Exp* _temp2220[ 1u]={ _temp2222}; struct
_tagged_arr _temp2221={( void*) _temp2220,( void*) _temp2220,( void*)( _temp2220
+ 1u)}; _temp2219( _temp2221);}), 0), 0), Cyc_Absyn_seq_stmt( _temp2117, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( Cyc_Toc__pop_region_e,({ struct Cyc_List_List*(* _temp2223)(
struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp2226= x_exp; struct Cyc_Absyn_Exp* _temp2224[ 1u]={
_temp2226}; struct _tagged_arr _temp2225={( void*) _temp2224,( void*) _temp2224,(
void*)( _temp2224 + 1u)}; _temp2223( _temp2225);}), 0), 0), 0), 0), 0), 0))->r));
return;} _LL2048:({ int(* _temp2227)( struct _tagged_arr fmt, struct _tagged_arr
ap)=( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos;
struct _tagged_arr _temp2230= _tag_arr("switchC_s", sizeof( unsigned char), 10u);
void* _temp2228[ 0u]={}; struct _tagged_arr _temp2229={( void*) _temp2228,( void*)
_temp2228,( void*)( _temp2228 + 0u)}; _temp2227( _temp2230, _temp2229);});
return; _LL2050:({ int(* _temp2231)( struct _tagged_arr fmt, struct _tagged_arr
ap)=( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp;
struct _tagged_arr _temp2234= _tag_arr("cut", sizeof( unsigned char), 4u); void*
_temp2232[ 0u]={}; struct _tagged_arr _temp2233={( void*) _temp2232,( void*)
_temp2232,( void*)( _temp2232 + 0u)}; _temp2231( _temp2234, _temp2233);});
return; _LL2052:({ int(* _temp2235)( struct _tagged_arr fmt, struct _tagged_arr
ap)=( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp;
struct _tagged_arr _temp2238= _tag_arr("splice", sizeof( unsigned char), 7u);
void* _temp2236[ 0u]={}; struct _tagged_arr _temp2237={( void*) _temp2236,( void*)
_temp2236,( void*)( _temp2236 + 0u)}; _temp2235( _temp2238, _temp2237);});
return; _LL2012:;}} struct _tuple15{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual
f2; void* f3; } ; static void Cyc_Toc_fndecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Fndecl* f){ f->tvs= 0; f->effect= 0; f->rgn_po= 0;( void*)( f->ret_type=(
void*) Cyc_Toc_typ_to_c(( void*) f->ret_type));{ struct Cyc_List_List* _temp2239=
f->args; for( 0; _temp2239 != 0; _temp2239=(( struct Cyc_List_List*) _check_null(
_temp2239))->tl){(*(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
_temp2239))->hd)).f3= Cyc_Toc_typ_to_c((*(( struct _tuple15*)(( struct Cyc_List_List*)
_check_null( _temp2239))->hd)).f3);{ struct _tuple0* _temp2240=({ struct _tuple0*
_temp2241=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2241->f1=(
void*) Cyc_Absyn_Loc_n; _temp2241->f2=(*(( struct _tuple15*)(( struct Cyc_List_List*)
_check_null( _temp2239))->hd)).f1; _temp2241;}); nv= Cyc_Toc_add_varmap( nv,
_temp2240, Cyc_Absyn_var_exp( _temp2240, 0));}}} if(( unsigned int) f->cyc_varargs?((
struct Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs))->name != 0: 0){ int
_temp2244; void* _temp2246; void* _temp2248; struct Cyc_Absyn_Tqual _temp2250;
struct Cyc_Core_Opt* _temp2252; struct Cyc_Absyn_VarargInfo _temp2242=*(( struct
Cyc_Absyn_VarargInfo*) _check_null( f->cyc_varargs)); _LL2253: _temp2252=
_temp2242.name; goto _LL2251; _LL2251: _temp2250= _temp2242.tq; goto _LL2249;
_LL2249: _temp2248=( void*) _temp2242.type; goto _LL2247; _LL2247: _temp2246=(
void*) _temp2242.rgn; goto _LL2245; _LL2245: _temp2244= _temp2242.inject; goto
_LL2243; _LL2243: { void* _temp2254= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(
_temp2248, _temp2246, _temp2250)); struct _tuple0* _temp2255=({ struct _tuple0*
_temp2258=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2258->f1=(
void*) Cyc_Absyn_Loc_n; _temp2258->f2=( struct _tagged_arr*)(( struct Cyc_Core_Opt*)
_check_null( _temp2252))->v; _temp2258;}); f->args=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( f->args,({
struct Cyc_List_List* _temp2256=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2256->hd=( void*)({ struct _tuple15* _temp2257=(
struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp2257->f1=( struct
_tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( _temp2252))->v; _temp2257->f2=
_temp2250; _temp2257->f3= _temp2254; _temp2257;}); _temp2256->tl= 0; _temp2256;}));
nv= Cyc_Toc_add_varmap( nv, _temp2255, Cyc_Absyn_var_exp( _temp2255, 0)); f->cyc_varargs=
0;}}{ struct Cyc_List_List* _temp2259=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( f->param_vardecls))->v; for( 0; _temp2259 != 0; _temp2259=(( struct
Cyc_List_List*) _check_null( _temp2259))->tl){( void*)((( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp2259))->hd)->type=( void*) Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp2259))->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel( nv), f->body);}
static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2260= s; _LL2262: if(
_temp2260 ==( void*) Cyc_Absyn_Abstract){ goto _LL2263;} else{ goto _LL2264;}
_LL2264: if( _temp2260 ==( void*) Cyc_Absyn_ExternC){ goto _LL2265;} else{ goto
_LL2266;} _LL2266: goto _LL2267; _LL2263: return( void*) Cyc_Absyn_Public;
_LL2265: return( void*) Cyc_Absyn_Extern; _LL2267: return s; _LL2261:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp2268=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; if( Cyc_Toc_structs_so_far == 0){
Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp2269=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2269->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2269;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp2270=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(*
_temp2268).f2); if( _temp2270 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2268).f2, s));} else{ if(((
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Core_Opt*) _check_null( _temp2270))->v)->fields
== 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(* _temp2268).f2,
s)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)((
void*) Cyc_Absyn_Public)); s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp2271=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2271->v=(
void*) _temp2268; _temp2271;}); if( s->fields != 0){ if( seen_defn_before){ s->fields=
0;} else{ struct Cyc_List_List* _temp2272=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; for( 0; _temp2272 != 0; _temp2272=(( struct Cyc_List_List*)
_check_null( _temp2272))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2272))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2272))->hd)->type));}}}}} struct Cyc_Core_Opt* Cyc_Toc_unions_so_far= 0;
static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl* u){ struct
_tuple0* _temp2273=( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( u->name))->v;
if( Cyc_Toc_unions_so_far == 0){ Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt*
_temp2274=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2274->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2274;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp2275=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp2273).f2); if( _temp2275 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2273).f2, u));} else{ if(((
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Core_Opt*) _check_null( _temp2275))->v)->fields
== 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(* _temp2273).f2,
u)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)( u->sc=( void*)((
void*) Cyc_Absyn_Public)); u->tvs= 0; u->name=({ struct Cyc_Core_Opt* _temp2276=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2276->v=(
void*) _temp2273; _temp2276;}); if( u->fields != 0){ if( seen_defn_before){ u->fields=
0;} else{ struct Cyc_List_List* _temp2277=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( u->fields))->v; for( 0; _temp2277 != 0; _temp2277=(( struct Cyc_List_List*)
_check_null( _temp2277))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2277))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2277))->hd)->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp2278=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2278->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2278;});}{ struct
_tuple0* _temp2279= tud->name; if( tud->fields == 0){ return;} if((( int(*)(
struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)(( struct Cyc_Set_Set*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v, _temp2279)){
return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v=(
void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tuple0* elt))
Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp2279));{ int tag_count= 0; int box_count= 0; struct Cyc_List_List*
_temp2280=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( tud->fields))->v;
for( 0; _temp2280 != 0; _temp2280=(( struct Cyc_List_List*) _check_null(
_temp2280))->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2280))->hd; if( f->typs == 0){ struct
Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp( tag_count
++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name, Cyc_Absyn_sint_t,
eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2281; _temp2281.q_const= 1;
_temp2281.q_volatile= 0; _temp2281.q_restrict= 0; _temp2281;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2282=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2282->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2283=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2283[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2284; _temp2284.tag= Cyc_Absyn_Var_d; _temp2284.f1= vd; _temp2284;});
_temp2283;}), 0); _temp2282->tl= Cyc_Toc_result_decls; _temp2282;});} else{
struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2285; _temp2285.q_const=
1; _temp2285.q_volatile= 0; _temp2285.q_restrict= 0; _temp2285;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2286=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2286->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2287=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2287[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2288; _temp2288.tag= Cyc_Absyn_Var_d; _temp2288.f1= vd; _temp2288;});
_temp2287;}), 0); _temp2286->tl= Cyc_Toc_result_decls; _temp2286;});{ struct Cyc_List_List*
_temp2289= 0; int i= 1;{ struct Cyc_List_List* _temp2290= f->typs; for( 0;
_temp2290 != 0;( _temp2290=(( struct Cyc_List_List*) _check_null( _temp2290))->tl,
i ++)){ struct _tagged_arr* _temp2291= Cyc_Toc_fieldname( i); struct Cyc_Absyn_Structfield*
_temp2292=({ struct Cyc_Absyn_Structfield* _temp2294=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2294->name= _temp2291;
_temp2294->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null(
_temp2290))->hd)).f1; _temp2294->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2290))->hd)).f2);
_temp2294->width= 0; _temp2294->attributes= 0; _temp2294;}); _temp2289=({ struct
Cyc_List_List* _temp2293=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2293->hd=( void*) _temp2292; _temp2293->tl= _temp2289; _temp2293;});}}
_temp2289=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp2289); _temp2289=({ struct Cyc_List_List* _temp2295=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2295->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2296=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2296->name= Cyc_Toc_tag_sp; _temp2296->tq= Cyc_Toc_mt_tq; _temp2296->type=(
void*) Cyc_Absyn_sint_t; _temp2296->width= 0; _temp2296->attributes= 0;
_temp2296;}); _temp2295->tl= _temp2289; _temp2295;});{ struct Cyc_Absyn_Structdecl*
_temp2297=({ struct Cyc_Absyn_Structdecl* _temp2301=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2301->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2301->name=({ struct Cyc_Core_Opt* _temp2303=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2303->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp2303;}); _temp2301->tvs= 0; _temp2301->fields=({ struct Cyc_Core_Opt*
_temp2302=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2302->v=( void*) _temp2289; _temp2302;}); _temp2301->attributes= 0;
_temp2301;}); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2298=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2298->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2299=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2299[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2300; _temp2300.tag= Cyc_Absyn_Struct_d;
_temp2300.f1= _temp2297; _temp2300;}); _temp2299;}), 0); _temp2298->tl= Cyc_Toc_result_decls;
_temp2298;});}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far == 0){ Cyc_Toc_xtunion_fields_so_far=({ struct Cyc_Core_Opt*
_temp2304=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2304->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp2304;});} if( xd->fields
== 0){ return;}{ struct _tuple0* _temp2305= xd->name; struct Cyc_List_List*
_temp2306=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( xd->fields))->v;
for( 0; _temp2306 != 0; _temp2306=(( struct Cyc_List_List*) _check_null(
_temp2306))->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2306))->hd; struct _tagged_arr* fn=(* f->name).f2;
struct Cyc_Absyn_Exp* _temp2307= Cyc_Absyn_uint_exp( _get_arr_size(* fn, sizeof(
unsigned char)) + 4, 0); struct Cyc_Absyn_ArrayType_struct* _temp2308=({ struct
Cyc_Absyn_ArrayType_struct* _temp2378=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2378[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2379; _temp2379.tag= Cyc_Absyn_ArrayType;
_temp2379.f1=( void*) Cyc_Absyn_uchar_t; _temp2379.f2= Cyc_Toc_mt_tq; _temp2379.f3=(
struct Cyc_Absyn_Exp*) _temp2307; _temp2379;}); _temp2378;}); struct Cyc_Core_Opt*
_temp2309=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tuple0*
key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct Cyc_Core_Opt
_temp2317; int _temp2318; _LL2311: if( _temp2309 == 0){ goto _LL2312;} else{
goto _LL2313;} _LL2313: if( _temp2309 == 0){ goto _LL2315;} else{ _temp2317=*
_temp2309; _LL2319: _temp2318=( int) _temp2317.v; if( _temp2318 == 0){ goto
_LL2314;} else{ goto _LL2315;}} _LL2315: goto _LL2316; _LL2312: { struct Cyc_Absyn_Exp*
initopt= 0; if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){ initopt=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_string_exp(( struct _tagged_arr)({ struct _tagged_arr(* _temp2320)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp2328= _tag_arr("%c%c%c%c%s", sizeof( unsigned char), 11u);
struct Cyc_Stdio_Int_pa_struct _temp2333; _temp2333.tag= Cyc_Stdio_Int_pa;
_temp2333.f1=( int)(( unsigned int) 0);{ void* _temp2323=( void*)& _temp2333;
struct Cyc_Stdio_Int_pa_struct _temp2332; _temp2332.tag= Cyc_Stdio_Int_pa;
_temp2332.f1=( int)(( unsigned int) 0);{ void* _temp2324=( void*)& _temp2332;
struct Cyc_Stdio_Int_pa_struct _temp2331; _temp2331.tag= Cyc_Stdio_Int_pa;
_temp2331.f1=( int)(( unsigned int) 0);{ void* _temp2325=( void*)& _temp2331;
struct Cyc_Stdio_Int_pa_struct _temp2330; _temp2330.tag= Cyc_Stdio_Int_pa;
_temp2330.f1=( int)(( unsigned int) 0);{ void* _temp2326=( void*)& _temp2330;
struct Cyc_Stdio_String_pa_struct _temp2329; _temp2329.tag= Cyc_Stdio_String_pa;
_temp2329.f1=( struct _tagged_arr)* fn;{ void* _temp2327=( void*)& _temp2329;
void* _temp2321[ 5u]={ _temp2323, _temp2324, _temp2325, _temp2326, _temp2327};
struct _tagged_arr _temp2322={( void*) _temp2321,( void*) _temp2321,( void*)(
_temp2321 + 5u)}; _temp2320( _temp2328, _temp2322);}}}}}}), 0);}{ struct Cyc_Absyn_Vardecl*
_temp2334= Cyc_Absyn_new_vardecl( f->name,( void*) _temp2308, initopt);( void*)(
_temp2334->sc=( void*)(( void*) f->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2335=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2335->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2336=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2336[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2337; _temp2337.tag= Cyc_Absyn_Var_d;
_temp2337.f1= _temp2334; _temp2337;}); _temp2336;}), 0); _temp2335->tl= Cyc_Toc_result_decls;
_temp2335;});( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key,
int data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc !=( void*)
Cyc_Absyn_Extern)); if( f->typs != 0){ struct Cyc_List_List* fields= 0; int i= 1;{
struct Cyc_List_List* _temp2338= f->typs; for( 0; _temp2338 != 0;( _temp2338=((
struct Cyc_List_List*) _check_null( _temp2338))->tl, i ++)){ struct _tagged_arr*
_temp2339=({ struct _tagged_arr* _temp2343=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr)); _temp2343[ 0]=( struct _tagged_arr)({ struct
_tagged_arr(* _temp2344)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp2348= _tag_arr("f%d", sizeof( unsigned char), 4u);
struct Cyc_Stdio_Int_pa_struct _temp2349; _temp2349.tag= Cyc_Stdio_Int_pa;
_temp2349.f1=( int)(( unsigned int) i);{ void* _temp2347=( void*)& _temp2349;
void* _temp2345[ 1u]={ _temp2347}; struct _tagged_arr _temp2346={( void*)
_temp2345,( void*) _temp2345,( void*)( _temp2345 + 1u)}; _temp2344( _temp2348,
_temp2346);}}); _temp2343;}); struct Cyc_Absyn_Structfield* _temp2340=({ struct
Cyc_Absyn_Structfield* _temp2342=( struct Cyc_Absyn_Structfield*) GC_malloc(
sizeof( struct Cyc_Absyn_Structfield)); _temp2342->name= _temp2339; _temp2342->tq=(*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2338))->hd)).f1;
_temp2342->type=( void*) Cyc_Toc_typ_to_c_array((*(( struct _tuple3*)(( struct
Cyc_List_List*) _check_null( _temp2338))->hd)).f2); _temp2342->width= 0;
_temp2342->attributes= 0; _temp2342;}); fields=({ struct Cyc_List_List*
_temp2341=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2341->hd=( void*) _temp2340; _temp2341->tl= fields; _temp2341;});}} fields=({
struct Cyc_List_List* _temp2350=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2350->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2351=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2351->name= Cyc_Toc_tag_sp; _temp2351->tq= Cyc_Toc_mt_tq; _temp2351->type=(
void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t, Cyc_Toc_mt_tq); _temp2351->width=
0; _temp2351->attributes= 0; _temp2351;}); _temp2350->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields); _temp2350;});{ struct Cyc_Absyn_Structdecl*
_temp2352=({ struct Cyc_Absyn_Structdecl* _temp2356=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2356->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2356->name=({ struct Cyc_Core_Opt* _temp2358=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2358->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp2358;}); _temp2356->tvs= 0; _temp2356->fields=({ struct Cyc_Core_Opt*
_temp2357=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2357->v=( void*) fields; _temp2357;}); _temp2356->attributes= 0; _temp2356;});
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2353=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2353->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2354=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2354[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2355; _temp2355.tag= Cyc_Absyn_Struct_d;
_temp2355.f1= _temp2352; _temp2355;}); _temp2354;}), 0); _temp2353->tl= Cyc_Toc_result_decls;
_temp2353;});}} goto _LL2310;}} _LL2314: if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2359= Cyc_Absyn_string_exp(( struct _tagged_arr)({
struct _tagged_arr(* _temp2364)( struct _tagged_arr fmt, struct _tagged_arr)=
Cyc_Stdio_aprintf; struct _tagged_arr _temp2372= _tag_arr("%c%c%c%c%s", sizeof(
unsigned char), 11u); struct Cyc_Stdio_Int_pa_struct _temp2377; _temp2377.tag=
Cyc_Stdio_Int_pa; _temp2377.f1=( int)(( unsigned int) 0);{ void* _temp2367=(
void*)& _temp2377; struct Cyc_Stdio_Int_pa_struct _temp2376; _temp2376.tag= Cyc_Stdio_Int_pa;
_temp2376.f1=( int)(( unsigned int) 0);{ void* _temp2368=( void*)& _temp2376;
struct Cyc_Stdio_Int_pa_struct _temp2375; _temp2375.tag= Cyc_Stdio_Int_pa;
_temp2375.f1=( int)(( unsigned int) 0);{ void* _temp2369=( void*)& _temp2375;
struct Cyc_Stdio_Int_pa_struct _temp2374; _temp2374.tag= Cyc_Stdio_Int_pa;
_temp2374.f1=( int)(( unsigned int) 0);{ void* _temp2370=( void*)& _temp2374;
struct Cyc_Stdio_String_pa_struct _temp2373; _temp2373.tag= Cyc_Stdio_String_pa;
_temp2373.f1=( struct _tagged_arr)* fn;{ void* _temp2371=( void*)& _temp2373;
void* _temp2365[ 5u]={ _temp2367, _temp2368, _temp2369, _temp2370, _temp2371};
struct _tagged_arr _temp2366={( void*) _temp2365,( void*) _temp2365,( void*)(
_temp2365 + 5u)}; _temp2364( _temp2372, _temp2366);}}}}}}), 0); struct Cyc_Absyn_Vardecl*
_temp2360= Cyc_Absyn_new_vardecl( f->name,( void*) _temp2308,( struct Cyc_Absyn_Exp*)
_temp2359);( void*)( _temp2360->sc=( void*)(( void*) f->sc)); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2361=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2361->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2362=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2362[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2363; _temp2363.tag= Cyc_Absyn_Var_d; _temp2363.f1= _temp2360; _temp2363;});
_temp2362;}), 0); _temp2361->tl= Cyc_Toc_result_decls; _temp2361;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));} goto _LL2310; _LL2316: goto
_LL2310; _LL2310:;}}} static void Cyc_Toc_enumdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2380= ed->name;( void*)( ed->sc=(
void*)(( void*) Cyc_Absyn_Public)); if( ed->fields != 0){ struct Cyc_List_List*
_temp2381=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ed->fields))->v;
for( 0; _temp2381 != 0; _temp2381=(( struct Cyc_List_List*) _check_null(
_temp2381))->tl){ if((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2381))->hd)->tag != 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2381))->hd)->tag));(( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_List_List*) _check_null( _temp2381))->hd)->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
i, 0);}}}} static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv,
struct Cyc_Toc_Env* init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt*
s){( void*)( vd->type=( void*) Cyc_Toc_typ_to_c_array(( void*) vd->type)); Cyc_Toc_stmt_to_c(
body_nv, s); if( vd->initializer != 0){ struct Cyc_Absyn_Exp* init=( struct Cyc_Absyn_Exp*)
_check_null( vd->initializer); void* _temp2382=( void*) init->r; struct Cyc_Absyn_Exp*
_temp2388; struct Cyc_Absyn_Exp* _temp2390; struct Cyc_Absyn_Vardecl* _temp2392;
_LL2384: if(*(( int*) _temp2382) == Cyc_Absyn_Comprehension_e){ _LL2393:
_temp2392=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp2382)->f1; goto
_LL2391; _LL2391: _temp2390=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2382)->f2; goto _LL2389; _LL2389: _temp2388=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2382)->f3; goto _LL2385;} else{ goto _LL2386;} _LL2386: goto _LL2387;
_LL2385: vd->initializer= 0;( void*)( s->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
init_nv, Cyc_Absyn_var_exp( vd->name, 0), _temp2392, _temp2390, _temp2388, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r)); goto _LL2383; _LL2387: if(( void*) vd->sc ==( void*)
Cyc_Absyn_Static){ struct Cyc_Toc_Env* _temp2394= Cyc_Toc_copy_env( init_nv);
_temp2394->toplevel= 1; Cyc_Toc_exp_to_c( _temp2394, init);} else{ Cyc_Toc_exp_to_c(
init_nv, init);} goto _LL2383; _LL2383:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt,
struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)((
struct Cyc_Core_Opt*) _check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct
_tuple0* x= Cyc_Toc_temp_var(); struct _tagged_arr* succ_lab= Cyc_Toc_fresh_label();
struct _tagged_arr* fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();{ struct
_RegionHandle _temp2395= _new_region(); struct _RegionHandle* prgn=& _temp2395;
_push_region( prgn);{ struct Cyc_Absyn_Stmt* _temp2398; struct Cyc_List_List*
_temp2400; struct Cyc_Toc_Env* _temp2402; struct _tuple11 _temp2396= Cyc_Toc_xlate_pat(
nv, prgn, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab,
fail_lab, 0); _LL2403: _temp2402= _temp2396.f1; goto _LL2401; _LL2401: _temp2400=
_temp2396.f2; goto _LL2399; _LL2399: _temp2398= _temp2396.f3; goto _LL2397;
_LL2397: Cyc_Toc_stmt_to_c( _temp2402, s);{ struct Cyc_Absyn_Stmt* _temp2404=
Cyc_Absyn_label_stmt( succ_lab, s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt(
x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2398, _temp2404, 0), 0);} else{ struct Cyc_Absyn_Exp* _temp2405= Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp( 0)); struct Cyc_Absyn_Stmt* _temp2406= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( _temp2405, 0), 0); s= Cyc_Absyn_declare_stmt( x,
Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2398,
Cyc_Absyn_seq_stmt( _temp2406, _temp2404, 0), 0), 0);} for( 0; _temp2400 != 0;
_temp2400=(( struct Cyc_List_List*) _check_null( _temp2400))->tl){ struct
_tuple12 _temp2409; void* _temp2410; struct _tuple0* _temp2412; struct _tuple12*
_temp2407=( struct _tuple12*)(( struct Cyc_List_List*) _check_null( _temp2400))->hd;
_temp2409=* _temp2407; _LL2413: _temp2412= _temp2409.f1; goto _LL2411; _LL2411:
_temp2410= _temp2409.f2; goto _LL2408; _LL2408: s= Cyc_Absyn_declare_stmt(
_temp2412, _temp2410, 0, s, 0);}}}; _pop_region( prgn);} return s;}} static
struct Cyc_Toc_Env* Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv, struct Cyc_List_List*
ds, int top){ for( 0; ds != 0; ds=(( struct Cyc_List_List*) _check_null( ds))->tl){
if( ! nv->toplevel){({ int(* _temp2414)( struct _tagged_arr fmt, struct
_tagged_arr ap)=( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos;
struct _tagged_arr _temp2417= _tag_arr("decls_to_c: not at toplevel!", sizeof(
unsigned char), 29u); void* _temp2415[ 0u]={}; struct _tagged_arr _temp2416={(
void*) _temp2415,( void*) _temp2415,( void*)( _temp2415 + 0u)}; _temp2414(
_temp2417, _temp2416);});}{ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( ds))->hd; void* _temp2418=( void*) d->r;
struct Cyc_Absyn_Vardecl* _temp2444; struct Cyc_Absyn_Fndecl* _temp2446; struct
Cyc_Absyn_Structdecl* _temp2448; struct Cyc_Absyn_Uniondecl* _temp2450; struct
Cyc_Absyn_Tuniondecl* _temp2452; struct Cyc_Absyn_Enumdecl* _temp2454; struct
Cyc_Absyn_Typedefdecl* _temp2456; struct Cyc_List_List* _temp2458; struct Cyc_List_List*
_temp2460; struct Cyc_List_List* _temp2462; _LL2420: if(*(( int*) _temp2418) ==
Cyc_Absyn_Var_d){ _LL2445: _temp2444=(( struct Cyc_Absyn_Var_d_struct*)
_temp2418)->f1; goto _LL2421;} else{ goto _LL2422;} _LL2422: if(*(( int*)
_temp2418) == Cyc_Absyn_Fn_d){ _LL2447: _temp2446=(( struct Cyc_Absyn_Fn_d_struct*)
_temp2418)->f1; goto _LL2423;} else{ goto _LL2424;} _LL2424: if(*(( int*)
_temp2418) == Cyc_Absyn_Let_d){ goto _LL2425;} else{ goto _LL2426;} _LL2426: if(*((
int*) _temp2418) == Cyc_Absyn_Letv_d){ goto _LL2427;} else{ goto _LL2428;}
_LL2428: if(*(( int*) _temp2418) == Cyc_Absyn_Struct_d){ _LL2449: _temp2448=((
struct Cyc_Absyn_Struct_d_struct*) _temp2418)->f1; goto _LL2429;} else{ goto
_LL2430;} _LL2430: if(*(( int*) _temp2418) == Cyc_Absyn_Union_d){ _LL2451:
_temp2450=(( struct Cyc_Absyn_Union_d_struct*) _temp2418)->f1; goto _LL2431;}
else{ goto _LL2432;} _LL2432: if(*(( int*) _temp2418) == Cyc_Absyn_Tunion_d){
_LL2453: _temp2452=(( struct Cyc_Absyn_Tunion_d_struct*) _temp2418)->f1; goto
_LL2433;} else{ goto _LL2434;} _LL2434: if(*(( int*) _temp2418) == Cyc_Absyn_Enum_d){
_LL2455: _temp2454=(( struct Cyc_Absyn_Enum_d_struct*) _temp2418)->f1; goto
_LL2435;} else{ goto _LL2436;} _LL2436: if(*(( int*) _temp2418) == Cyc_Absyn_Typedef_d){
_LL2457: _temp2456=(( struct Cyc_Absyn_Typedef_d_struct*) _temp2418)->f1; goto
_LL2437;} else{ goto _LL2438;} _LL2438: if(*(( int*) _temp2418) == Cyc_Absyn_Namespace_d){
_LL2459: _temp2458=(( struct Cyc_Absyn_Namespace_d_struct*) _temp2418)->f2; goto
_LL2439;} else{ goto _LL2440;} _LL2440: if(*(( int*) _temp2418) == Cyc_Absyn_Using_d){
_LL2461: _temp2460=(( struct Cyc_Absyn_Using_d_struct*) _temp2418)->f2; goto
_LL2441;} else{ goto _LL2442;} _LL2442: if(*(( int*) _temp2418) == Cyc_Absyn_ExternC_d){
_LL2463: _temp2462=(( struct Cyc_Absyn_ExternC_d_struct*) _temp2418)->f1; goto
_LL2443;} else{ goto _LL2419;} _LL2421: { struct _tuple0* _temp2464= _temp2444->name;
if(( void*) _temp2444->sc ==( void*) Cyc_Absyn_ExternC){ _temp2464=({ struct
_tuple0* _temp2465=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2465->f1= Cyc_Absyn_rel_ns_null; _temp2465->f2=(* _temp2464).f2; _temp2465;});}
if( _temp2444->initializer != 0){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)
_check_null( _temp2444->initializer));} nv= Cyc_Toc_add_varmap( nv, _temp2444->name,
Cyc_Absyn_varb_exp( _temp2464,( void*)({ struct Cyc_Absyn_Global_b_struct*
_temp2466=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp2466[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp2467; _temp2467.tag= Cyc_Absyn_Global_b;
_temp2467.f1= _temp2444; _temp2467;}); _temp2466;}), 0)); _temp2444->name=
_temp2464;( void*)( _temp2444->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2444->sc));(
void*)( _temp2444->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2444->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2468=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2468->hd=( void*) d; _temp2468->tl=
Cyc_Toc_result_decls; _temp2468;}); goto _LL2419;} _LL2423: { struct _tuple0*
_temp2469= _temp2446->name; if(( void*) _temp2446->sc ==( void*) Cyc_Absyn_ExternC){
_temp2469=({ struct _tuple0* _temp2470=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp2470->f1= Cyc_Absyn_rel_ns_null; _temp2470->f2=(*
_temp2469).f2; _temp2470;});} nv= Cyc_Toc_add_varmap( nv, _temp2446->name, Cyc_Absyn_var_exp(
_temp2469, 0)); _temp2446->name= _temp2469; Cyc_Toc_fndecl_to_c( nv, _temp2446);
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2471=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2471->hd=( void*) d; _temp2471->tl=
Cyc_Toc_result_decls; _temp2471;}); goto _LL2419;} _LL2425: goto _LL2427;
_LL2427:({ int(* _temp2472)( struct _tagged_arr fmt, struct _tagged_arr ap)=(
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos;
struct _tagged_arr _temp2475= _tag_arr("letdecl at toplevel", sizeof(
unsigned char), 20u); void* _temp2473[ 0u]={}; struct _tagged_arr _temp2474={(
void*) _temp2473,( void*) _temp2473,( void*)( _temp2473 + 0u)}; _temp2472(
_temp2475, _temp2474);}); goto _LL2419; _LL2429: Cyc_Toc_structdecl_to_c(
_temp2448); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2476=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2476->hd=( void*) d; _temp2476->tl=
Cyc_Toc_result_decls; _temp2476;}); goto _LL2419; _LL2431: Cyc_Toc_uniondecl_to_c(
_temp2450); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2477=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2477->hd=( void*) d; _temp2477->tl=
Cyc_Toc_result_decls; _temp2477;}); goto _LL2419; _LL2433: if( _temp2452->is_xtunion){
Cyc_Toc_xtuniondecl_to_c( _temp2452);} else{ Cyc_Toc_tuniondecl_to_c( _temp2452);}
goto _LL2419; _LL2435: Cyc_Toc_enumdecl_to_c( nv, _temp2454); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2478=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2478->hd=( void*) d; _temp2478->tl= Cyc_Toc_result_decls;
_temp2478;}); goto _LL2419; _LL2437: _temp2456->name= _temp2456->name; _temp2456->tvs=
0;( void*)( _temp2456->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2456->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2479=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2479->hd=( void*) d; _temp2479->tl=
Cyc_Toc_result_decls; _temp2479;}); goto _LL2419; _LL2439: _temp2460= _temp2458;
goto _LL2441; _LL2441: _temp2462= _temp2460; goto _LL2443; _LL2443: nv= Cyc_Toc_decls_to_c(
nv, _temp2462, top); goto _LL2419; _LL2419:;}} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_temp_var_counter= 0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far=
0; Cyc_Toc_tunions_so_far= 0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List*
Cyc_Toc_toc( struct Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(),
ds, 1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}
