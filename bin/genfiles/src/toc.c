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
struct Cyc_List_List* x); extern int Cyc_String_zstrptrcmp( struct _tagged_arr*,
struct _tagged_arr*); extern struct _tagged_arr Cyc_String_strconcat( struct
_tagged_arr, struct _tagged_arr); struct Cyc_Set_Set; extern struct Cyc_Set_Set*
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
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_add_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_times_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_divide_exp(
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
Cyc_Stdio_stderr, fmt, ap);({ void* _temp0[ 0u]={}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("\n", sizeof( unsigned char), 2u), _tag_arr( _temp0, sizeof( void*), 0u));});
Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( int) _throw((
void*) Cyc_Toc_Toc_Unimplemented);} static void* Cyc_Toc_toc_impos( struct
_tagged_arr fmt, struct _tagged_arr ap){ Cyc_Stdio_vfprintf( Cyc_Stdio_stderr,
fmt, ap);({ void* _temp1[ 0u]={}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr, _tag_arr("\n",
sizeof( unsigned char), 2u), _tag_arr( _temp1, sizeof( void*), 0u));}); Cyc_Stdio_fflush((
struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( int) _throw(( void*) Cyc_Toc_Toc_Impossible);}
unsigned char Cyc_Toc_Match_error[ 16u]="\000\000\000\000Match_error"; static
unsigned char _temp2[ 5u]="curr"; static struct _tagged_arr Cyc_Toc_curr_string={
_temp2, _temp2, _temp2 + 5u}; static struct _tagged_arr* Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;
static unsigned char _temp3[ 4u]="tag"; static struct _tagged_arr Cyc_Toc_tag_string={
_temp3, _temp3, _temp3 + 4u}; static struct _tagged_arr* Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;
static unsigned char _temp4[ 14u]="_handler_cons"; static struct _tagged_arr Cyc_Toc__handler_cons_string={
_temp4, _temp4, _temp4 + 14u}; static struct _tagged_arr* Cyc_Toc__handler_cons_sp=&
Cyc_Toc__handler_cons_string; static unsigned char _temp5[ 8u]="handler"; static
struct _tagged_arr Cyc_Toc_handler_string={ _temp5, _temp5, _temp5 + 8u}; static
struct _tagged_arr* Cyc_Toc_handler_sp=& Cyc_Toc_handler_string; static
unsigned char _temp6[ 14u]="_RegionHandle"; static struct _tagged_arr Cyc_Toc__RegionHandle_string={
_temp6, _temp6, _temp6 + 14u}; static struct _tagged_arr* Cyc_Toc__RegionHandle_sp=&
Cyc_Toc__RegionHandle_string; static unsigned char _temp7[ 7u]="_throw"; static
struct _tagged_arr Cyc_Toc__throw_str={ _temp7, _temp7, _temp7 + 7u}; static
struct _tuple0 Cyc_Toc__throw_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__throw_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_re={ 1u,& Cyc_Toc__throw_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={ 0,( void*)((
void*)& Cyc_Toc__throw_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__throw_e=&
Cyc_Toc__throw_ev; static unsigned char _temp9[ 7u]="setjmp"; static struct
_tagged_arr Cyc_Toc_setjmp_str={ _temp9, _temp9, _temp9 + 7u}; static struct
_tuple0 Cyc_Toc_setjmp_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc_setjmp_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc_setjmp_re={ 1u,& Cyc_Toc_setjmp_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={ 0,( void*)((
void*)& Cyc_Toc_setjmp_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc_setjmp_e=&
Cyc_Toc_setjmp_ev; static unsigned char _temp11[ 14u]="_push_handler"; static
struct _tagged_arr Cyc_Toc__push_handler_str={ _temp11, _temp11, _temp11 + 14u};
static struct _tuple0 Cyc_Toc__push_handler_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__push_handler_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_handler_re={
1u,& Cyc_Toc__push_handler_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__push_handler_ev={ 0,( void*)(( void*)& Cyc_Toc__push_handler_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;
static unsigned char _temp13[ 13u]="_pop_handler"; static struct _tagged_arr Cyc_Toc__pop_handler_str={
_temp13, _temp13, _temp13 + 13u}; static struct _tuple0 Cyc_Toc__pop_handler_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__pop_handler_str}; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={ 1u,& Cyc_Toc__pop_handler_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={ 0,(
void*)(( void*)& Cyc_Toc__pop_handler_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__pop_handler_e=&
Cyc_Toc__pop_handler_ev; static unsigned char _temp15[ 12u]="_exn_thrown";
static struct _tagged_arr Cyc_Toc__exn_thrown_str={ _temp15, _temp15, _temp15 +
12u}; static struct _tuple0 Cyc_Toc__exn_thrown_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__exn_thrown_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__exn_thrown_re={
1u,& Cyc_Toc__exn_thrown_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__exn_thrown_ev={ 0,( void*)(( void*)& Cyc_Toc__exn_thrown_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;
static unsigned char _temp17[ 14u]="_npop_handler"; static struct _tagged_arr
Cyc_Toc__npop_handler_str={ _temp17, _temp17, _temp17 + 14u}; static struct
_tuple0 Cyc_Toc__npop_handler_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__npop_handler_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__npop_handler_re={ 1u,& Cyc_Toc__npop_handler_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={ 0,(
void*)(( void*)& Cyc_Toc__npop_handler_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__npop_handler_e=&
Cyc_Toc__npop_handler_ev; static unsigned char _temp19[ 12u]="_check_null";
static struct _tagged_arr Cyc_Toc__check_null_str={ _temp19, _temp19, _temp19 +
12u}; static struct _tuple0 Cyc_Toc__check_null_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_null_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_null_re={
1u,& Cyc_Toc__check_null_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__check_null_ev={ 0,( void*)(( void*)& Cyc_Toc__check_null_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;
static unsigned char _temp21[ 28u]="_check_known_subscript_null"; static struct
_tagged_arr Cyc_Toc__check_known_subscript_null_str={ _temp21, _temp21, _temp21
+ 28u}; static struct _tuple0 Cyc_Toc__check_known_subscript_null_pr={( void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_known_subscript_null_str}; static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_null_re={ 1u,& Cyc_Toc__check_known_subscript_null_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,( void*)(( void*)& Cyc_Toc__check_known_subscript_null_re), 0}; static struct
Cyc_Absyn_Exp* Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static unsigned char _temp23[ 31u]="_check_known_subscript_notnull"; static
struct _tagged_arr Cyc_Toc__check_known_subscript_notnull_str={ _temp23, _temp23,
_temp23 + 31u}; static struct _tuple0 Cyc_Toc__check_known_subscript_notnull_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_known_subscript_notnull_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_notnull_re={
1u,& Cyc_Toc__check_known_subscript_notnull_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={ 0,( void*)((
void*)& Cyc_Toc__check_known_subscript_notnull_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;
static unsigned char _temp25[ 25u]="_check_unknown_subscript"; static struct
_tagged_arr Cyc_Toc__check_unknown_subscript_str={ _temp25, _temp25, _temp25 +
25u}; static struct _tuple0 Cyc_Toc__check_unknown_subscript_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_unknown_subscript_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_unknown_subscript_re={
1u,& Cyc_Toc__check_unknown_subscript_pr,( void*)(( void*) 0u)}; static struct
Cyc_Absyn_Exp Cyc_Toc__check_unknown_subscript_ev={ 0,( void*)(( void*)& Cyc_Toc__check_unknown_subscript_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__check_unknown_subscript_e=& Cyc_Toc__check_unknown_subscript_ev;
static unsigned char _temp27[ 12u]="_tagged_arr"; static struct _tagged_arr Cyc_Toc__tagged_arr_str={
_temp27, _temp27, _temp27 + 12u}; static struct _tuple0 Cyc_Toc__tagged_arr_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_str}; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_re={ 1u,& Cyc_Toc__tagged_arr_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_ev={ 0,(
void*)(( void*)& Cyc_Toc__tagged_arr_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tagged_arr_e=&
Cyc_Toc__tagged_arr_ev; static unsigned char _temp29[ 9u]="_tag_arr"; static
struct _tagged_arr Cyc_Toc__tag_arr_str={ _temp29, _temp29, _temp29 + 9u};
static struct _tuple0 Cyc_Toc__tag_arr_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__tag_arr_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_arr_re={
1u,& Cyc_Toc__tag_arr_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__tag_arr_ev={
0,( void*)(( void*)& Cyc_Toc__tag_arr_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tag_arr_e=&
Cyc_Toc__tag_arr_ev; static unsigned char _temp31[ 14u]="_init_tag_arr"; static
struct _tagged_arr Cyc_Toc__init_tag_arr_str={ _temp31, _temp31, _temp31 + 14u};
static struct _tuple0 Cyc_Toc__init_tag_arr_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__init_tag_arr_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__init_tag_arr_re={
1u,& Cyc_Toc__init_tag_arr_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__init_tag_arr_ev={ 0,( void*)(( void*)& Cyc_Toc__init_tag_arr_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__init_tag_arr_e=& Cyc_Toc__init_tag_arr_ev;
static unsigned char _temp33[ 11u]="_untag_arr"; static struct _tagged_arr Cyc_Toc__untag_arr_str={
_temp33, _temp33, _temp33 + 11u}; static struct _tuple0 Cyc_Toc__untag_arr_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__untag_arr_str}; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__untag_arr_re={ 1u,& Cyc_Toc__untag_arr_pr,( void*)(( void*) 0u)};
static struct Cyc_Absyn_Exp Cyc_Toc__untag_arr_ev={ 0,( void*)(( void*)& Cyc_Toc__untag_arr_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__untag_arr_e=& Cyc_Toc__untag_arr_ev;
static unsigned char _temp35[ 14u]="_get_arr_size"; static struct _tagged_arr
Cyc_Toc__get_arr_size_str={ _temp35, _temp35, _temp35 + 14u}; static struct
_tuple0 Cyc_Toc__get_arr_size_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__get_arr_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_arr_size_re={ 1u,& Cyc_Toc__get_arr_size_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__get_arr_size_ev={ 0,(
void*)(( void*)& Cyc_Toc__get_arr_size_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__get_arr_size_e=&
Cyc_Toc__get_arr_size_ev; static unsigned char _temp37[ 17u]="_tagged_arr_plus";
static struct _tagged_arr Cyc_Toc__tagged_arr_plus_str={ _temp37, _temp37,
_temp37 + 17u}; static struct _tuple0 Cyc_Toc__tagged_arr_plus_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__tagged_arr_plus_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_plus_re={
1u,& Cyc_Toc__tagged_arr_plus_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__tagged_arr_plus_ev={ 0,( void*)(( void*)& Cyc_Toc__tagged_arr_plus_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tagged_arr_plus_e=& Cyc_Toc__tagged_arr_plus_ev;
static unsigned char _temp39[ 25u]="_tagged_arr_inplace_plus"; static struct
_tagged_arr Cyc_Toc__tagged_arr_inplace_plus_str={ _temp39, _temp39, _temp39 +
25u}; static struct _tuple0 Cyc_Toc__tagged_arr_inplace_plus_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__tagged_arr_inplace_plus_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_re={
1u,& Cyc_Toc__tagged_arr_inplace_plus_pr,( void*)(( void*) 0u)}; static struct
Cyc_Absyn_Exp Cyc_Toc__tagged_arr_inplace_plus_ev={ 0,( void*)(( void*)& Cyc_Toc__tagged_arr_inplace_plus_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc__tagged_arr_inplace_plus_e=& Cyc_Toc__tagged_arr_inplace_plus_ev;
static unsigned char _temp41[ 30u]="_tagged_arr_inplace_plus_post"; static
struct _tagged_arr Cyc_Toc__tagged_arr_inplace_plus_post_str={ _temp41, _temp41,
_temp41 + 30u}; static struct _tuple0 Cyc_Toc__tagged_arr_inplace_plus_post_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_inplace_plus_post_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_post_re={
1u,& Cyc_Toc__tagged_arr_inplace_plus_post_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_inplace_plus_post_ev={ 0,( void*)((
void*)& Cyc_Toc__tagged_arr_inplace_plus_post_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__tagged_arr_inplace_plus_post_e=& Cyc_Toc__tagged_arr_inplace_plus_post_ev;
static unsigned char _temp43[ 10u]="GC_malloc"; static struct _tagged_arr Cyc_Toc_GC_malloc_str={
_temp43, _temp43, _temp43 + 10u}; static struct _tuple0 Cyc_Toc_GC_malloc_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc_GC_malloc_str}; static struct Cyc_Absyn_Var_e_struct
Cyc_Toc_GC_malloc_re={ 1u,& Cyc_Toc_GC_malloc_pr,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp Cyc_Toc_GC_malloc_ev={ 0,( void*)(( void*)& Cyc_Toc_GC_malloc_re),
0}; static struct Cyc_Absyn_Exp* Cyc_Toc_GC_malloc_e=& Cyc_Toc_GC_malloc_ev;
static unsigned char _temp45[ 17u]="GC_malloc_atomic"; static struct _tagged_arr
Cyc_Toc_GC_malloc_atomic_str={ _temp45, _temp45, _temp45 + 17u}; static struct
_tuple0 Cyc_Toc_GC_malloc_atomic_pr={( void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc_GC_malloc_atomic_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc_GC_malloc_atomic_re={ 1u,& Cyc_Toc_GC_malloc_atomic_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc_GC_malloc_atomic_ev={
0,( void*)(( void*)& Cyc_Toc_GC_malloc_atomic_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc_GC_malloc_atomic_e=& Cyc_Toc_GC_malloc_atomic_ev; static unsigned char
_temp47[ 15u]="_region_malloc"; static struct _tagged_arr Cyc_Toc__region_malloc_str={
_temp47, _temp47, _temp47 + 15u}; static struct _tuple0 Cyc_Toc__region_malloc_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__region_malloc_str}; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__region_malloc_re={ 1u,& Cyc_Toc__region_malloc_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={ 0,(
void*)(( void*)& Cyc_Toc__region_malloc_re), 0}; static struct Cyc_Absyn_Exp*
Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev; static unsigned char
_temp49[ 12u]="_new_region"; static struct _tagged_arr Cyc_Toc__new_region_str={
_temp49, _temp49, _temp49 + 12u}; static struct _tuple0 Cyc_Toc__new_region_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__new_region_str}; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={ 1u,& Cyc_Toc__new_region_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={ 0,(
void*)(( void*)& Cyc_Toc__new_region_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__new_region_e=&
Cyc_Toc__new_region_ev; static unsigned char _temp51[ 13u]="_push_region";
static struct _tagged_arr Cyc_Toc__push_region_str={ _temp51, _temp51, _temp51 +
13u}; static struct _tuple0 Cyc_Toc__push_region_pr={( void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__push_region_str}; static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_region_re={
1u,& Cyc_Toc__push_region_pr,( void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp
Cyc_Toc__push_region_ev={ 0,( void*)(( void*)& Cyc_Toc__push_region_re), 0};
static struct Cyc_Absyn_Exp* Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;
static unsigned char _temp53[ 12u]="_pop_region"; static struct _tagged_arr Cyc_Toc__pop_region_str={
_temp53, _temp53, _temp53 + 12u}; static struct _tuple0 Cyc_Toc__pop_region_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__pop_region_str}; static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={ 1u,& Cyc_Toc__pop_region_pr,(
void*)(( void*) 0u)}; static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={ 0,(
void*)(( void*)& Cyc_Toc__pop_region_re), 0}; static struct Cyc_Absyn_Exp* Cyc_Toc__pop_region_e=&
Cyc_Toc__pop_region_ev; static struct Cyc_Absyn_StructType_struct Cyc_Toc_tagged_arr_typ_v={
9u,( struct _tuple0*)& Cyc_Toc__tagged_arr_pr, 0, 0}; static void* Cyc_Toc_tagged_arr_typ=(
void*)& Cyc_Toc_tagged_arr_typ_v; static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={
0, 0, 0}; static unsigned char _temp56[ 3u]="f0"; static struct _tagged_arr Cyc_Toc_f0={
_temp56, _temp56, _temp56 + 3u}; static struct _tagged_arr* Cyc_Toc_field_names_v[
1u]={& Cyc_Toc_f0}; static struct _tagged_arr Cyc_Toc_field_names={( void*)((
struct _tagged_arr**) Cyc_Toc_field_names_v),( void*)(( struct _tagged_arr**)
Cyc_Toc_field_names_v),( void*)(( struct _tagged_arr**) Cyc_Toc_field_names_v) +
1u}; static struct _tagged_arr* Cyc_Toc_fieldname( int i){ unsigned int fsz=
_get_arr_size( Cyc_Toc_field_names, sizeof( struct _tagged_arr*)); if( i >= fsz){
Cyc_Toc_field_names=({ unsigned int _temp57=( unsigned int)( i + 1); struct
_tagged_arr** _temp58=( struct _tagged_arr**) GC_malloc( sizeof( struct
_tagged_arr*) * _temp57); struct _tagged_arr _temp63= _tag_arr( _temp58, sizeof(
struct _tagged_arr*),( unsigned int)( i + 1));{ unsigned int _temp59= _temp57;
unsigned int j; for( j= 0; j < _temp59; j ++){ _temp58[ j]= j < fsz?*(( struct
_tagged_arr**) _check_unknown_subscript( Cyc_Toc_field_names, sizeof( struct
_tagged_arr*),( int) j)):({ struct _tagged_arr* _temp60=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp60[ 0]=( struct _tagged_arr)({
struct Cyc_Stdio_Int_pa_struct _temp62; _temp62.tag= Cyc_Stdio_Int_pa; _temp62.f1=(
int) j;{ void* _temp61[ 1u]={& _temp62}; Cyc_Stdio_aprintf( _tag_arr("f%d",
sizeof( unsigned char), 4u), _tag_arr( _temp61, sizeof( void*), 1u));}});
_temp60;});}}; _temp63;});} return*(( struct _tagged_arr**)
_check_unknown_subscript( Cyc_Toc_field_names, sizeof( struct _tagged_arr*), i));}
struct _tuple4{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; static
int Cyc_Toc_is_zero( struct Cyc_Absyn_Exp* e){ void* _temp64=( void*) e->r; void*
_temp90; unsigned char _temp92; void* _temp94; short _temp96; void* _temp98; int
_temp100; void* _temp102; long long _temp104; void* _temp106; struct Cyc_Absyn_Exp*
_temp108; struct Cyc_List_List* _temp110; struct Cyc_List_List* _temp112; struct
Cyc_List_List* _temp114; struct Cyc_List_List* _temp116; struct Cyc_List_List*
_temp118; _LL66: if(*(( int*) _temp64) == Cyc_Absyn_Const_e){ _LL91: _temp90=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp64)->f1; if(( unsigned int)
_temp90 > 1u?*(( int*) _temp90) == Cyc_Absyn_Char_c: 0){ _LL93: _temp92=((
struct Cyc_Absyn_Char_c_struct*) _temp90)->f2; goto _LL67;} else{ goto _LL68;}}
else{ goto _LL68;} _LL68: if(*(( int*) _temp64) == Cyc_Absyn_Const_e){ _LL95:
_temp94=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp64)->f1; if((
unsigned int) _temp94 > 1u?*(( int*) _temp94) == Cyc_Absyn_Short_c: 0){ _LL97:
_temp96=(( struct Cyc_Absyn_Short_c_struct*) _temp94)->f2; goto _LL69;} else{
goto _LL70;}} else{ goto _LL70;} _LL70: if(*(( int*) _temp64) == Cyc_Absyn_Const_e){
_LL99: _temp98=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp64)->f1; if((
unsigned int) _temp98 > 1u?*(( int*) _temp98) == Cyc_Absyn_Int_c: 0){ _LL101:
_temp100=(( struct Cyc_Absyn_Int_c_struct*) _temp98)->f2; goto _LL71;} else{
goto _LL72;}} else{ goto _LL72;} _LL72: if(*(( int*) _temp64) == Cyc_Absyn_Const_e){
_LL103: _temp102=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp64)->f1; if((
unsigned int) _temp102 > 1u?*(( int*) _temp102) == Cyc_Absyn_LongLong_c: 0){
_LL105: _temp104=(( struct Cyc_Absyn_LongLong_c_struct*) _temp102)->f2; goto
_LL73;} else{ goto _LL74;}} else{ goto _LL74;} _LL74: if(*(( int*) _temp64) ==
Cyc_Absyn_Const_e){ _LL107: _temp106=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp64)->f1; if( _temp106 ==( void*) Cyc_Absyn_Null_c){ goto _LL75;} else{ goto
_LL76;}} else{ goto _LL76;} _LL76: if(*(( int*) _temp64) == Cyc_Absyn_Cast_e){
_LL109: _temp108=(( struct Cyc_Absyn_Cast_e_struct*) _temp64)->f2; goto _LL77;}
else{ goto _LL78;} _LL78: if(*(( int*) _temp64) == Cyc_Absyn_Tuple_e){ _LL111:
_temp110=(( struct Cyc_Absyn_Tuple_e_struct*) _temp64)->f1; goto _LL79;} else{
goto _LL80;} _LL80: if(*(( int*) _temp64) == Cyc_Absyn_Array_e){ _LL113:
_temp112=(( struct Cyc_Absyn_Array_e_struct*) _temp64)->f1; goto _LL81;} else{
goto _LL82;} _LL82: if(*(( int*) _temp64) == Cyc_Absyn_Struct_e){ _LL115:
_temp114=(( struct Cyc_Absyn_Struct_e_struct*) _temp64)->f3; goto _LL83;} else{
goto _LL84;} _LL84: if(*(( int*) _temp64) == Cyc_Absyn_CompoundLit_e){ _LL117:
_temp116=(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp64)->f2; goto _LL85;}
else{ goto _LL86;} _LL86: if(*(( int*) _temp64) == Cyc_Absyn_UnresolvedMem_e){
_LL119: _temp118=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp64)->f2; goto
_LL87;} else{ goto _LL88;} _LL88: goto _LL89; _LL67: return _temp92 =='\000';
_LL69: return _temp96 == 0; _LL71: return _temp100 == 0; _LL73: return _temp104
== 0; _LL75: return 1; _LL77: return Cyc_Toc_is_zero( _temp108); _LL79: return((
int(*)( int(* pred)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_forall)(
Cyc_Toc_is_zero, _temp110); _LL81: _temp114= _temp112; goto _LL83; _LL83:
_temp116= _temp114; goto _LL85; _LL85: _temp118= _temp116; goto _LL87; _LL87:
for( 0; _temp118 != 0; _temp118=(( struct Cyc_List_List*) _check_null( _temp118))->tl){
if( ! Cyc_Toc_is_zero((*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp118))->hd)).f2)){ return 0;}} return 1; _LL89: return 0; _LL65:;} static
int Cyc_Toc_is_nullable( void* t){ void* _temp120= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp126; struct Cyc_Absyn_Conref* _temp128; _LL122:
if(( unsigned int) _temp120 > 4u?*(( int*) _temp120) == Cyc_Absyn_PointerType: 0){
_LL127: _temp126=(( struct Cyc_Absyn_PointerType_struct*) _temp120)->f1; _LL129:
_temp128= _temp126.nullable; goto _LL123;} else{ goto _LL124;} _LL124: goto
_LL125; _LL123: return(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
0, _temp128); _LL125:({ void* _temp130[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("is_nullable", sizeof(
unsigned char), 12u), _tag_arr( _temp130, sizeof( void*), 0u));}); return 0;
_LL121:;} static struct _tuple0* Cyc_Toc_collapse_qvar_tag( struct _tuple0* x,
struct _tagged_arr tag){ return({ struct _tuple0* _temp131=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp131->f1=(* x).f1; _temp131->f2=({
struct _tagged_arr* _temp132=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp132[ 0]=( struct _tagged_arr) Cyc_String_strconcat(*(* x).f2,
tag); _temp132;}); _temp131;});} struct _tuple5{ struct _tagged_arr* f1; struct
Cyc_List_List* f2; } ; static struct _tagged_arr* Cyc_Toc_add_tuple_type( struct
Cyc_List_List* tqs0){{ struct Cyc_List_List* _temp133= Cyc_Toc_tuple_types; for(
0; _temp133 != 0; _temp133=(( struct Cyc_List_List*) _check_null( _temp133))->tl){
struct _tuple5 _temp136; struct Cyc_List_List* _temp137; struct _tagged_arr*
_temp139; struct _tuple5* _temp134=( struct _tuple5*)(( struct Cyc_List_List*)
_check_null( _temp133))->hd; _temp136=* _temp134; _LL140: _temp139= _temp136.f1;
goto _LL138; _LL138: _temp137= _temp136.f2; goto _LL135; _LL135: { struct Cyc_List_List*
_temp141= tqs0; for( 0; _temp141 != 0? _temp137 != 0: 0;( _temp141=(( struct Cyc_List_List*)
_check_null( _temp141))->tl, _temp137=(( struct Cyc_List_List*) _check_null(
_temp137))->tl)){ if( ! Cyc_Tcutil_unify((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp141))->hd)).f2,( void*)(( struct Cyc_List_List*) _check_null(
_temp137))->hd)){ break;}} if( _temp141 == 0? _temp137 == 0: 0){ return _temp139;}}}}{
struct _tagged_arr* x=({ struct _tagged_arr* _temp155=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp155[ 0]=( struct _tagged_arr)({
struct Cyc_Stdio_Int_pa_struct _temp157; _temp157.tag= Cyc_Stdio_Int_pa;
_temp157.f1=( int)(( unsigned int) Cyc_Toc_tuple_type_counter ++);{ void*
_temp156[ 1u]={& _temp157}; Cyc_Stdio_aprintf( _tag_arr("_tuple%d", sizeof(
unsigned char), 9u), _tag_arr( _temp156, sizeof( void*), 1u));}}); _temp155;});
struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple3*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_snd_tqt, tqs0); struct Cyc_List_List*
_temp142= 0; struct Cyc_List_List* ts2= ts;{ int i= 1; for( 0; ts2 != 0;( ts2=((
struct Cyc_List_List*) _check_null( ts2))->tl, i ++)){ _temp142=({ struct Cyc_List_List*
_temp143=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp143->hd=( void*)({ struct Cyc_Absyn_Structfield* _temp144=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp144->name= Cyc_Toc_fieldname(
i); _temp144->tq= Cyc_Toc_mt_tq; _temp144->type=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( ts2))->hd); _temp144->width= 0; _temp144->attributes= 0; _temp144;});
_temp143->tl= _temp142; _temp143;});}} _temp142=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp142);{ struct Cyc_Absyn_Structdecl*
_temp145=({ struct Cyc_Absyn_Structdecl* _temp151=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp151->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp151->name=({ struct Cyc_Core_Opt* _temp153=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp153->v=( void*)({ struct _tuple0*
_temp154=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp154->f1=
Cyc_Absyn_rel_ns_null; _temp154->f2= x; _temp154;}); _temp153;}); _temp151->tvs=
0; _temp151->fields=({ struct Cyc_Core_Opt* _temp152=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp152->v=( void*) _temp142;
_temp152;}); _temp151->attributes= 0; _temp151;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp146=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp146->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp147=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp147[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp148; _temp148.tag= Cyc_Absyn_Struct_d; _temp148.f1=
_temp145; _temp148;}); _temp147;}), 0); _temp146->tl= Cyc_Toc_result_decls;
_temp146;}); Cyc_Toc_tuple_types=({ struct Cyc_List_List* _temp149=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp149->hd=( void*)({ struct
_tuple5* _temp150=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp150->f1= x; _temp150->f2= ts; _temp150;}); _temp149->tl= Cyc_Toc_tuple_types;
_temp149;}); return x;}}} struct _tuple0* Cyc_Toc_temp_var(){ return({ struct
_tuple0* _temp158=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp158->f1=( void*) Cyc_Absyn_Loc_n; _temp158->f2=({ struct _tagged_arr*
_temp159=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr));
_temp159[ 0]=( struct _tagged_arr)({ struct Cyc_Stdio_Int_pa_struct _temp161;
_temp161.tag= Cyc_Stdio_Int_pa; _temp161.f1=( int)(( unsigned int) Cyc_Toc_temp_var_counter
++);{ void* _temp160[ 1u]={& _temp161}; Cyc_Stdio_aprintf( _tag_arr("_temp%d",
sizeof( unsigned char), 8u), _tag_arr( _temp160, sizeof( void*), 1u));}});
_temp159;}); _temp158;});} static struct _tagged_arr* Cyc_Toc_fresh_label(){
return({ struct _tagged_arr* _temp162=( struct _tagged_arr*) GC_malloc( sizeof(
struct _tagged_arr)); _temp162[ 0]=( struct _tagged_arr)({ struct Cyc_Stdio_Int_pa_struct
_temp164; _temp164.tag= Cyc_Stdio_Int_pa; _temp164.f1=( int)(( unsigned int) Cyc_Toc_temp_var_counter
++);{ void* _temp163[ 1u]={& _temp164}; Cyc_Stdio_aprintf( _tag_arr("_LL%d",
sizeof( unsigned char), 6u), _tag_arr( _temp163, sizeof( void*), 1u));}});
_temp162;});} static void* Cyc_Toc_typ_to_c( void* t); static struct _tuple1*
Cyc_Toc_arg_to_c( struct _tuple1* a){ void* _temp167; struct Cyc_Absyn_Tqual
_temp169; struct Cyc_Core_Opt* _temp171; struct _tuple1 _temp165=* a; _LL172:
_temp171= _temp165.f1; goto _LL170; _LL170: _temp169= _temp165.f2; goto _LL168;
_LL168: _temp167= _temp165.f3; goto _LL166; _LL166: return({ struct _tuple1*
_temp173=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp173->f1=
_temp171; _temp173->f2= _temp169; _temp173->f3= Cyc_Toc_typ_to_c( _temp167);
_temp173;});} static struct _tuple3* Cyc_Toc_typ_to_c_f( struct _tuple3* x){
void* _temp176; struct Cyc_Absyn_Tqual _temp178; struct _tuple3 _temp174=* x;
_LL179: _temp178= _temp174.f1; goto _LL177; _LL177: _temp176= _temp174.f2; goto
_LL175; _LL175: return({ struct _tuple3* _temp180=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp180->f1= _temp178; _temp180->f2= Cyc_Toc_typ_to_c(
_temp176); _temp180;});} static void* Cyc_Toc_typ_to_c_array( void* t){ void*
_temp181= t; struct Cyc_Absyn_Exp* _temp189; struct Cyc_Absyn_Tqual _temp191;
void* _temp193; struct Cyc_Core_Opt* _temp195; struct Cyc_Core_Opt _temp197;
void* _temp198; _LL183: if(( unsigned int) _temp181 > 4u?*(( int*) _temp181) ==
Cyc_Absyn_ArrayType: 0){ _LL194: _temp193=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp181)->f1; goto _LL192; _LL192: _temp191=(( struct Cyc_Absyn_ArrayType_struct*)
_temp181)->f2; goto _LL190; _LL190: _temp189=(( struct Cyc_Absyn_ArrayType_struct*)
_temp181)->f3; goto _LL184;} else{ goto _LL185;} _LL185: if(( unsigned int)
_temp181 > 4u?*(( int*) _temp181) == Cyc_Absyn_Evar: 0){ _LL196: _temp195=((
struct Cyc_Absyn_Evar_struct*) _temp181)->f2; if( _temp195 == 0){ goto _LL187;}
else{ _temp197=* _temp195; _LL199: _temp198=( void*) _temp197.v; goto _LL186;}}
else{ goto _LL187;} _LL187: goto _LL188; _LL184: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp200=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp200[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp201; _temp201.tag= Cyc_Absyn_ArrayType;
_temp201.f1=( void*) Cyc_Toc_typ_to_c_array( _temp193); _temp201.f2= _temp191;
_temp201.f3= _temp189; _temp201;}); _temp200;}); _LL186: return Cyc_Toc_typ_to_c_array(
_temp198); _LL188: return Cyc_Toc_typ_to_c( t); _LL182:;} static struct Cyc_Absyn_Structfield*
Cyc_Toc_structfield_to_c( struct Cyc_Absyn_Structfield* f){ return({ struct Cyc_Absyn_Structfield*
_temp202=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp202->name= f->name; _temp202->tq= f->tq; _temp202->type=( void*) Cyc_Toc_typ_to_c((
void*) f->type); _temp202->width= f->width; _temp202->attributes= f->attributes;
_temp202;});} static void* Cyc_Toc_typ_to_c( void* t){ void* _temp203= t; struct
Cyc_Core_Opt* _temp255; struct Cyc_Core_Opt* _temp257; struct Cyc_Core_Opt
_temp259; void* _temp260; struct Cyc_Absyn_TunionInfo _temp262; void* _temp264;
struct Cyc_Absyn_Tuniondecl* _temp266; struct Cyc_Absyn_TunionFieldInfo _temp268;
void* _temp270; struct Cyc_Absyn_Tunionfield* _temp272; struct Cyc_Absyn_Tuniondecl*
_temp274; struct Cyc_Absyn_PtrInfo _temp276; struct Cyc_Absyn_Conref* _temp278;
struct Cyc_Absyn_Tqual _temp280; void* _temp282; struct Cyc_Absyn_Exp* _temp284;
struct Cyc_Absyn_Tqual _temp286; void* _temp288; struct Cyc_Absyn_FnInfo
_temp290; struct Cyc_List_List* _temp292; struct Cyc_Absyn_VarargInfo* _temp294;
int _temp296; struct Cyc_List_List* _temp298; void* _temp300; struct Cyc_List_List*
_temp302; struct Cyc_List_List* _temp304; struct Cyc_List_List* _temp306; struct
_tuple0* _temp308; struct _tuple0* _temp310; struct _tuple0* _temp312; struct
Cyc_Core_Opt* _temp314; struct Cyc_List_List* _temp316; struct _tuple0* _temp318;
void* _temp320; _LL205: if( _temp203 ==( void*) Cyc_Absyn_VoidType){ goto _LL206;}
else{ goto _LL207;} _LL207: if(( unsigned int) _temp203 > 4u?*(( int*) _temp203)
== Cyc_Absyn_Evar: 0){ _LL256: _temp255=(( struct Cyc_Absyn_Evar_struct*)
_temp203)->f2; if( _temp255 == 0){ goto _LL208;} else{ goto _LL209;}} else{ goto
_LL209;} _LL209: if(( unsigned int) _temp203 > 4u?*(( int*) _temp203) == Cyc_Absyn_Evar:
0){ _LL258: _temp257=(( struct Cyc_Absyn_Evar_struct*) _temp203)->f2; if(
_temp257 == 0){ goto _LL211;} else{ _temp259=* _temp257; _LL261: _temp260=( void*)
_temp259.v; goto _LL210;}} else{ goto _LL211;} _LL211: if(( unsigned int)
_temp203 > 4u?*(( int*) _temp203) == Cyc_Absyn_VarType: 0){ goto _LL212;} else{
goto _LL213;} _LL213: if(( unsigned int) _temp203 > 4u?*(( int*) _temp203) ==
Cyc_Absyn_TunionType: 0){ _LL263: _temp262=(( struct Cyc_Absyn_TunionType_struct*)
_temp203)->f1; _LL265: _temp264=( void*) _temp262.tunion_info; if(*(( int*)
_temp264) == Cyc_Absyn_KnownTunion){ _LL267: _temp266=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp264)->f1; goto _LL214;} else{ goto _LL215;}} else{ goto _LL215;} _LL215:
if(( unsigned int) _temp203 > 4u?*(( int*) _temp203) == Cyc_Absyn_TunionType: 0){
goto _LL216;} else{ goto _LL217;} _LL217: if(( unsigned int) _temp203 > 4u?*((
int*) _temp203) == Cyc_Absyn_TunionFieldType: 0){ _LL269: _temp268=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp203)->f1; _LL271: _temp270=( void*) _temp268.field_info; if(*(( int*)
_temp270) == Cyc_Absyn_KnownTunionfield){ _LL275: _temp274=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp270)->f1; goto _LL273; _LL273: _temp272=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp270)->f2; goto _LL218;} else{ goto _LL219;}} else{ goto _LL219;} _LL219:
if(( unsigned int) _temp203 > 4u?*(( int*) _temp203) == Cyc_Absyn_TunionFieldType:
0){ goto _LL220;} else{ goto _LL221;} _LL221: if(( unsigned int) _temp203 > 4u?*((
int*) _temp203) == Cyc_Absyn_PointerType: 0){ _LL277: _temp276=(( struct Cyc_Absyn_PointerType_struct*)
_temp203)->f1; _LL283: _temp282=( void*) _temp276.elt_typ; goto _LL281; _LL281:
_temp280= _temp276.tq; goto _LL279; _LL279: _temp278= _temp276.bounds; goto
_LL222;} else{ goto _LL223;} _LL223: if(( unsigned int) _temp203 > 4u?*(( int*)
_temp203) == Cyc_Absyn_IntType: 0){ goto _LL224;} else{ goto _LL225;} _LL225:
if( _temp203 ==( void*) Cyc_Absyn_FloatType){ goto _LL226;} else{ goto _LL227;}
_LL227: if( _temp203 ==( void*) Cyc_Absyn_DoubleType){ goto _LL228;} else{ goto
_LL229;} _LL229: if(( unsigned int) _temp203 > 4u?*(( int*) _temp203) == Cyc_Absyn_ArrayType:
0){ _LL289: _temp288=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp203)->f1;
goto _LL287; _LL287: _temp286=(( struct Cyc_Absyn_ArrayType_struct*) _temp203)->f2;
goto _LL285; _LL285: _temp284=(( struct Cyc_Absyn_ArrayType_struct*) _temp203)->f3;
goto _LL230;} else{ goto _LL231;} _LL231: if(( unsigned int) _temp203 > 4u?*((
int*) _temp203) == Cyc_Absyn_FnType: 0){ _LL291: _temp290=(( struct Cyc_Absyn_FnType_struct*)
_temp203)->f1; _LL301: _temp300=( void*) _temp290.ret_typ; goto _LL299; _LL299:
_temp298= _temp290.args; goto _LL297; _LL297: _temp296= _temp290.c_varargs; goto
_LL295; _LL295: _temp294= _temp290.cyc_varargs; goto _LL293; _LL293: _temp292=
_temp290.attributes; goto _LL232;} else{ goto _LL233;} _LL233: if(( unsigned int)
_temp203 > 4u?*(( int*) _temp203) == Cyc_Absyn_TupleType: 0){ _LL303: _temp302=((
struct Cyc_Absyn_TupleType_struct*) _temp203)->f1; goto _LL234;} else{ goto
_LL235;} _LL235: if(( unsigned int) _temp203 > 4u?*(( int*) _temp203) == Cyc_Absyn_AnonStructType:
0){ _LL305: _temp304=(( struct Cyc_Absyn_AnonStructType_struct*) _temp203)->f1;
goto _LL236;} else{ goto _LL237;} _LL237: if(( unsigned int) _temp203 > 4u?*((
int*) _temp203) == Cyc_Absyn_AnonUnionType: 0){ _LL307: _temp306=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp203)->f1; goto _LL238;} else{ goto _LL239;} _LL239: if(( unsigned int)
_temp203 > 4u?*(( int*) _temp203) == Cyc_Absyn_UnionType: 0){ _LL309: _temp308=((
struct Cyc_Absyn_UnionType_struct*) _temp203)->f1; goto _LL240;} else{ goto
_LL241;} _LL241: if(( unsigned int) _temp203 > 4u?*(( int*) _temp203) == Cyc_Absyn_StructType:
0){ _LL311: _temp310=(( struct Cyc_Absyn_StructType_struct*) _temp203)->f1; goto
_LL242;} else{ goto _LL243;} _LL243: if(( unsigned int) _temp203 > 4u?*(( int*)
_temp203) == Cyc_Absyn_EnumType: 0){ _LL313: _temp312=(( struct Cyc_Absyn_EnumType_struct*)
_temp203)->f1; goto _LL244;} else{ goto _LL245;} _LL245: if(( unsigned int)
_temp203 > 4u?*(( int*) _temp203) == Cyc_Absyn_TypedefType: 0){ _LL319: _temp318=((
struct Cyc_Absyn_TypedefType_struct*) _temp203)->f1; goto _LL317; _LL317:
_temp316=(( struct Cyc_Absyn_TypedefType_struct*) _temp203)->f2; goto _LL315;
_LL315: _temp314=(( struct Cyc_Absyn_TypedefType_struct*) _temp203)->f3; goto
_LL246;} else{ goto _LL247;} _LL247: if(( unsigned int) _temp203 > 4u?*(( int*)
_temp203) == Cyc_Absyn_RgnHandleType: 0){ _LL321: _temp320=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp203)->f1; goto _LL248;} else{ goto _LL249;} _LL249: if( _temp203 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL250;} else{ goto _LL251;} _LL251: if(( unsigned int)
_temp203 > 4u?*(( int*) _temp203) == Cyc_Absyn_AccessEff: 0){ goto _LL252;}
else{ goto _LL253;} _LL253: if(( unsigned int) _temp203 > 4u?*(( int*) _temp203)
== Cyc_Absyn_JoinEff: 0){ goto _LL254;} else{ goto _LL204;} _LL206: return t;
_LL208: return Cyc_Absyn_sint_t; _LL210: return Cyc_Toc_typ_to_c( _temp260);
_LL212: return Cyc_Absyn_void_star_typ(); _LL214: return Cyc_Absyn_void_star_typ();
_LL216:( int) _throw(({ void* _temp322[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("unresolved TunionType",
sizeof( unsigned char), 22u), _tag_arr( _temp322, sizeof( void*), 0u));}));
_LL218: if( _temp272->typs == 0){ if( _temp274->is_xtunion){ return Cyc_Absyn_star_typ(
Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);} else{ return Cyc_Absyn_uint_t;}}
else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp272->name,
_tag_arr("_struct", sizeof( unsigned char), 8u)));} _LL220:( int) _throw(({ void*
_temp323[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("unresolved TunionFieldType",
sizeof( unsigned char), 27u), _tag_arr( _temp323, sizeof( void*), 0u));}));
_LL222: _temp282= Cyc_Toc_typ_to_c_array( _temp282);{ void* _temp324=( void*)(
Cyc_Absyn_compress_conref( _temp278))->v; void* _temp330; _LL326: if((
unsigned int) _temp324 > 1u?*(( int*) _temp324) == Cyc_Absyn_Eq_constr: 0){
_LL331: _temp330=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp324)->f1;
if( _temp330 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL327;} else{ goto _LL328;}}
else{ goto _LL328;} _LL328: goto _LL329; _LL327: return Cyc_Toc_tagged_arr_typ;
_LL329: return Cyc_Absyn_star_typ( _temp282,( void*) Cyc_Absyn_HeapRgn, _temp280);
_LL325:;} _LL224: return t; _LL226: return t; _LL228: return t; _LL230: return(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp332=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp332[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp333; _temp333.tag= Cyc_Absyn_ArrayType; _temp333.f1=(
void*) Cyc_Toc_typ_to_c_array( _temp288); _temp333.f2= _temp286; _temp333.f3=
_temp284; _temp333;}); _temp332;}); _LL232: { struct Cyc_List_List* _temp334= 0;
for( 0; _temp292 != 0; _temp292=(( struct Cyc_List_List*) _check_null( _temp292))->tl){
void* _temp335=( void*)(( struct Cyc_List_List*) _check_null( _temp292))->hd;
_LL337: if( _temp335 ==( void*) Cyc_Absyn_Noreturn_att){ goto _LL338;} else{
goto _LL339;} _LL339: if( _temp335 ==( void*) Cyc_Absyn_Const_att){ goto _LL340;}
else{ goto _LL341;} _LL341: if(( unsigned int) _temp335 > 16u?*(( int*) _temp335)
== Cyc_Absyn_Format_att: 0){ goto _LL342;} else{ goto _LL343;} _LL343: goto
_LL344; _LL338: goto _LL340; _LL340: goto _LL342; _LL342: continue; _LL344:
_temp334=({ struct Cyc_List_List* _temp345=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp345->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( _temp292))->hd); _temp345->tl= _temp334; _temp345;}); goto _LL336;
_LL336:;}{ struct Cyc_List_List* _temp346=(( struct Cyc_List_List*(*)( struct
_tuple1*(* f)( struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_arg_to_c,
_temp298); if( _temp294 != 0){ void* _temp347= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ((
void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp294))->type,( void*)((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp294))->rgn, Cyc_Toc_mt_tq));
struct _tuple1* _temp348=({ struct _tuple1* _temp350=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp350->f1=(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp294))->name; _temp350->f2=(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp294))->tq; _temp350->f3= _temp347; _temp350;}); _temp346=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp346,({ struct Cyc_List_List* _temp349=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp349->hd=( void*) _temp348; _temp349->tl= 0;
_temp349;}));} return( void*)({ struct Cyc_Absyn_FnType_struct* _temp351=(
struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp351[ 0]=({ struct Cyc_Absyn_FnType_struct _temp352; _temp352.tag= Cyc_Absyn_FnType;
_temp352.f1=({ struct Cyc_Absyn_FnInfo _temp353; _temp353.tvars= 0; _temp353.effect=
0; _temp353.ret_typ=( void*) Cyc_Toc_typ_to_c( _temp300); _temp353.args=
_temp346; _temp353.c_varargs= _temp296; _temp353.cyc_varargs= 0; _temp353.rgn_po=
0; _temp353.attributes= _temp334; _temp353;}); _temp352;}); _temp351;});}}
_LL234: _temp302=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct
_tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f, _temp302);{
struct _tagged_arr* _temp354= Cyc_Toc_add_tuple_type( _temp302); return Cyc_Absyn_strct(
_temp354);} _LL236: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp355=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp355[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp356; _temp356.tag= Cyc_Absyn_AnonStructType; _temp356.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_structfield_to_c, _temp304); _temp356;}); _temp355;});
_LL238: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp357=(
struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp357[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp358; _temp358.tag=
Cyc_Absyn_AnonUnionType; _temp358.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp306); _temp358;}); _temp357;}); _LL240: if( _temp308 == 0){({ void*
_temp359[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("anonymous union", sizeof( unsigned char), 16u), _tag_arr( _temp359,
sizeof( void*), 0u));});} return Cyc_Absyn_unionq_typ(( struct _tuple0*)
_check_null( _temp308)); _LL242: if( _temp310 == 0){({ void* _temp360[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("anonymous struct",
sizeof( unsigned char), 17u), _tag_arr( _temp360, sizeof( void*), 0u));});}
return Cyc_Absyn_strctq(( struct _tuple0*) _check_null( _temp310)); _LL244:
return t; _LL246: if( _temp314 == 0){ return({ void* _temp361[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("Toc::typ_to_c: unresolved TypedefType", sizeof( unsigned char), 38u),
_tag_arr( _temp361, sizeof( void*), 0u));});} return( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp362=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp362[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp363; _temp363.tag= Cyc_Absyn_TypedefType;
_temp363.f1= _temp318; _temp363.f2= 0; _temp363.f3=({ struct Cyc_Core_Opt*
_temp364=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp364->v=( void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp314))->v); _temp364;}); _temp363;}); _temp362;}); _LL248:
return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp), Cyc_Toc_mt_tq);
_LL250: return({ void* _temp365[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: passed the heap region",
sizeof( unsigned char), 38u), _tag_arr( _temp365, sizeof( void*), 0u));});
_LL252: return({ void* _temp366[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: passed the access eff",
sizeof( unsigned char), 37u), _tag_arr( _temp366, sizeof( void*), 0u));});
_LL254: return({ void* _temp367[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: passed the join eff",
sizeof( unsigned char), 35u), _tag_arr( _temp367, sizeof( void*), 0u));});
_LL204:;} static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast( void* t,
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* l){ void* _temp368= t;
struct Cyc_Absyn_Tqual _temp374; void* _temp376; _LL370: if(( unsigned int)
_temp368 > 4u?*(( int*) _temp368) == Cyc_Absyn_ArrayType: 0){ _LL377: _temp376=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp368)->f1; goto _LL375; _LL375:
_temp374=(( struct Cyc_Absyn_ArrayType_struct*) _temp368)->f2; goto _LL371;}
else{ goto _LL372;} _LL372: goto _LL373; _LL371: return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ(
_temp376,( void*) Cyc_Absyn_HeapRgn, _temp374), e, l); _LL373: return Cyc_Absyn_cast_exp(
t, e, l); _LL369:;} static int Cyc_Toc_atomic_typ( void* t){ void* _temp378= Cyc_Tcutil_compress(
t); void* _temp416; struct Cyc_Absyn_Structdecl** _temp418; struct Cyc_List_List*
_temp420; struct Cyc_List_List* _temp422; struct Cyc_Absyn_TunionFieldInfo
_temp424; void* _temp426; struct Cyc_Absyn_Tunionfield* _temp428; struct Cyc_Absyn_Tuniondecl*
_temp430; struct Cyc_List_List* _temp432; _LL380: if( _temp378 ==( void*) Cyc_Absyn_VoidType){
goto _LL381;} else{ goto _LL382;} _LL382: if(( unsigned int) _temp378 > 4u?*((
int*) _temp378) == Cyc_Absyn_VarType: 0){ goto _LL383;} else{ goto _LL384;}
_LL384: if(( unsigned int) _temp378 > 4u?*(( int*) _temp378) == Cyc_Absyn_IntType:
0){ goto _LL385;} else{ goto _LL386;} _LL386: if(( unsigned int) _temp378 > 4u?*((
int*) _temp378) == Cyc_Absyn_EnumType: 0){ goto _LL387;} else{ goto _LL388;}
_LL388: if( _temp378 ==( void*) Cyc_Absyn_FloatType){ goto _LL389;} else{ goto
_LL390;} _LL390: if( _temp378 ==( void*) Cyc_Absyn_DoubleType){ goto _LL391;}
else{ goto _LL392;} _LL392: if(( unsigned int) _temp378 > 4u?*(( int*) _temp378)
== Cyc_Absyn_FnType: 0){ goto _LL393;} else{ goto _LL394;} _LL394: if((
unsigned int) _temp378 > 4u?*(( int*) _temp378) == Cyc_Absyn_ArrayType: 0){
_LL417: _temp416=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp378)->f1;
goto _LL395;} else{ goto _LL396;} _LL396: if(( unsigned int) _temp378 > 4u?*((
int*) _temp378) == Cyc_Absyn_StructType: 0){ _LL419: _temp418=(( struct Cyc_Absyn_StructType_struct*)
_temp378)->f3; goto _LL397;} else{ goto _LL398;} _LL398: if(( unsigned int)
_temp378 > 4u?*(( int*) _temp378) == Cyc_Absyn_AnonStructType: 0){ _LL421:
_temp420=(( struct Cyc_Absyn_AnonStructType_struct*) _temp378)->f1; goto _LL399;}
else{ goto _LL400;} _LL400: if(( unsigned int) _temp378 > 4u?*(( int*) _temp378)
== Cyc_Absyn_AnonUnionType: 0){ _LL423: _temp422=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp378)->f1; goto _LL401;} else{ goto _LL402;} _LL402: if(( unsigned int)
_temp378 > 4u?*(( int*) _temp378) == Cyc_Absyn_TunionFieldType: 0){ _LL425:
_temp424=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp378)->f1; _LL427:
_temp426=( void*) _temp424.field_info; if(*(( int*) _temp426) == Cyc_Absyn_KnownTunionfield){
_LL431: _temp430=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp426)->f1;
goto _LL429; _LL429: _temp428=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp426)->f2; goto _LL403;} else{ goto _LL404;}} else{ goto _LL404;} _LL404:
if(( unsigned int) _temp378 > 4u?*(( int*) _temp378) == Cyc_Absyn_TupleType: 0){
_LL433: _temp432=(( struct Cyc_Absyn_TupleType_struct*) _temp378)->f1; goto
_LL405;} else{ goto _LL406;} _LL406: if(( unsigned int) _temp378 > 4u?*(( int*)
_temp378) == Cyc_Absyn_TunionType: 0){ goto _LL407;} else{ goto _LL408;} _LL408:
if(( unsigned int) _temp378 > 4u?*(( int*) _temp378) == Cyc_Absyn_UnionType: 0){
goto _LL409;} else{ goto _LL410;} _LL410: if(( unsigned int) _temp378 > 4u?*((
int*) _temp378) == Cyc_Absyn_PointerType: 0){ goto _LL411;} else{ goto _LL412;}
_LL412: if(( unsigned int) _temp378 > 4u?*(( int*) _temp378) == Cyc_Absyn_RgnHandleType:
0){ goto _LL413;} else{ goto _LL414;} _LL414: goto _LL415; _LL381: return 1;
_LL383: return 0; _LL385: return 1; _LL387: return 1; _LL389: return 1; _LL391:
return 1; _LL393: return 1; _LL395: return Cyc_Toc_atomic_typ( _temp416); _LL397:
if( _temp418 == 0? 1:(*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp418)))->fields
== 0){ return 0;}{ struct Cyc_List_List* _temp434=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp418)))->fields))->v; for( 0; _temp434 != 0; _temp434=(( struct
Cyc_List_List*) _check_null( _temp434))->tl){ if( ! Cyc_Toc_atomic_typ(( void*)((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp434))->hd)->type)){
return 0;}}} return 1; _LL399: _temp422= _temp420; goto _LL401; _LL401: for( 0;
_temp422 != 0; _temp422=(( struct Cyc_List_List*) _check_null( _temp422))->tl){
if( ! Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp422))->hd)->type)){ return 0;}} return 1; _LL403: _temp432=
_temp428->typs; goto _LL405; _LL405: for( 0; _temp432 != 0; _temp432=(( struct
Cyc_List_List*) _check_null( _temp432))->tl){ if( ! Cyc_Toc_atomic_typ((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp432))->hd)).f2)){
return 0;}} return 1; _LL407: return 0; _LL409: return 0; _LL411: return 0;
_LL413: return 0; _LL415:({ struct Cyc_Stdio_String_pa_struct _temp436; _temp436.tag=
Cyc_Stdio_String_pa; _temp436.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp435[ 1u]={& _temp436};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("atomic_typ:  bad type %s",
sizeof( unsigned char), 25u), _tag_arr( _temp435, sizeof( void*), 1u));}});
return 0; _LL379:;} static int Cyc_Toc_is_void_star( void* t){ void* _temp437=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp443; void* _temp445;
_LL439: if(( unsigned int) _temp437 > 4u?*(( int*) _temp437) == Cyc_Absyn_PointerType:
0){ _LL444: _temp443=(( struct Cyc_Absyn_PointerType_struct*) _temp437)->f1;
_LL446: _temp445=( void*) _temp443.elt_typ; goto _LL440;} else{ goto _LL441;}
_LL441: goto _LL442; _LL440: { void* _temp447= Cyc_Tcutil_compress( _temp445);
_LL449: if( _temp447 ==( void*) Cyc_Absyn_VoidType){ goto _LL450;} else{ goto
_LL451;} _LL451: goto _LL452; _LL450: return 1; _LL452: return 0; _LL448:;}
_LL442: return 0; _LL438:;} static int Cyc_Toc_is_poly_field( void* t, struct
_tagged_arr* f){ void* _temp453= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp465; struct Cyc_Absyn_Structdecl* _temp467; struct Cyc_List_List* _temp468;
_LL455: if(( unsigned int) _temp453 > 4u?*(( int*) _temp453) == Cyc_Absyn_StructType:
0){ _LL466: _temp465=(( struct Cyc_Absyn_StructType_struct*) _temp453)->f3; if(
_temp465 == 0){ goto _LL457;} else{ _temp467=* _temp465; goto _LL456;}} else{
goto _LL457;} _LL457: if(( unsigned int) _temp453 > 4u?*(( int*) _temp453) ==
Cyc_Absyn_AnonStructType: 0){ _LL469: _temp468=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp453)->f1; goto _LL458;} else{ goto _LL459;} _LL459: if(( unsigned int)
_temp453 > 4u?*(( int*) _temp453) == Cyc_Absyn_AnonUnionType: 0){ goto _LL460;}
else{ goto _LL461;} _LL461: if(( unsigned int) _temp453 > 4u?*(( int*) _temp453)
== Cyc_Absyn_UnionType: 0){ goto _LL462;} else{ goto _LL463;} _LL463: goto
_LL464; _LL456: if( _temp467->fields == 0){( int) _throw(({ void* _temp470[ 0u]={};
Cyc_Toc_toc_impos( _tag_arr("is_poly_field: struct missing fields", sizeof(
unsigned char), 37u), _tag_arr( _temp470, sizeof( void*), 0u));}));} _temp468=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp467->fields))->v;
goto _LL458; _LL458: { struct Cyc_Absyn_Structfield* _temp471= Cyc_Absyn_lookup_field(
_temp468, f); if( _temp471 == 0){({ struct Cyc_Stdio_String_pa_struct _temp473;
_temp473.tag= Cyc_Stdio_String_pa; _temp473.f1=( struct _tagged_arr)*(( struct
Cyc_Absyn_Structfield*) _check_null( _temp471))->name;{ void* _temp472[ 1u]={&
_temp473};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("is_poly_field: bad field %s", sizeof( unsigned char), 28u), _tag_arr(
_temp472, sizeof( void*), 1u));}});} return Cyc_Toc_is_void_star(( void*)((
struct Cyc_Absyn_Structfield*) _check_null( _temp471))->type);} _LL460: return 0;
_LL462: return 0; _LL464:( int) _throw(({ struct Cyc_Stdio_String_pa_struct
_temp475; _temp475.tag= Cyc_Stdio_String_pa; _temp475.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp474[ 1u]={& _temp475}; Cyc_Toc_toc_impos(
_tag_arr("is_poly_field: bad type %s", sizeof( unsigned char), 27u), _tag_arr(
_temp474, sizeof( void*), 1u));}})); _LL454:;} static int Cyc_Toc_is_poly_project(
struct Cyc_Absyn_Exp* e){ void* _temp476=( void*) e->r; struct _tagged_arr*
_temp484; struct Cyc_Absyn_Exp* _temp486; struct _tagged_arr* _temp488; struct
Cyc_Absyn_Exp* _temp490; _LL478: if(*(( int*) _temp476) == Cyc_Absyn_StructMember_e){
_LL487: _temp486=(( struct Cyc_Absyn_StructMember_e_struct*) _temp476)->f1; goto
_LL485; _LL485: _temp484=(( struct Cyc_Absyn_StructMember_e_struct*) _temp476)->f2;
goto _LL479;} else{ goto _LL480;} _LL480: if(*(( int*) _temp476) == Cyc_Absyn_StructArrow_e){
_LL491: _temp490=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp476)->f1; goto
_LL489; _LL489: _temp488=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp476)->f2;
goto _LL481;} else{ goto _LL482;} _LL482: goto _LL483; _LL479: return Cyc_Toc_is_poly_field((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp486->topt))->v, _temp484);
_LL481: { void* _temp492= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp490->topt))->v); struct Cyc_Absyn_PtrInfo _temp498; void*
_temp500; _LL494: if(( unsigned int) _temp492 > 4u?*(( int*) _temp492) == Cyc_Absyn_PointerType:
0){ _LL499: _temp498=(( struct Cyc_Absyn_PointerType_struct*) _temp492)->f1;
_LL501: _temp500=( void*) _temp498.elt_typ; goto _LL495;} else{ goto _LL496;}
_LL496: goto _LL497; _LL495: return Cyc_Toc_is_poly_field( _temp500, _temp488);
_LL497:({ struct Cyc_Stdio_String_pa_struct _temp503; _temp503.tag= Cyc_Stdio_String_pa;
_temp503.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp490->topt))->v);{ void* _temp502[ 1u]={& _temp503};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("is_poly_project:  bad type %s",
sizeof( unsigned char), 30u), _tag_arr( _temp502, sizeof( void*), 1u));}});
return 0; _LL493:;} _LL483: return 0; _LL477:;} static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Toc_GC_malloc_e,({
struct Cyc_List_List* _temp504=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp504->hd=( void*) s; _temp504->tl= 0; _temp504;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Toc_GC_malloc_atomic_e,({ struct Cyc_List_List*
_temp505=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp505->hd=( void*) s; _temp505->tl= 0; _temp505;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp* s){ if( Cyc_Toc_atomic_typ( t)){
return Cyc_Toc_malloc_atomic( s);} return Cyc_Toc_malloc_ptr( s);} static struct
Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp( struct Cyc_Absyn_Exp* rgn, struct Cyc_Absyn_Exp*
s){ return Cyc_Absyn_fncall_exp( Cyc_Toc__region_malloc_e,({ struct Cyc_Absyn_Exp*
_temp506[ 2u]; _temp506[ 1u]= s; _temp506[ 0u]= rgn;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp506, sizeof( struct Cyc_Absyn_Exp*),
2u));}), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp*
e){ return Cyc_Absyn_fncall_exp( Cyc_Toc__throw_e,({ struct Cyc_List_List*
_temp507=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp507->hd=( void*) e; _temp507->tl= 0; _temp507;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_make_toplevel_tagged_arr( void* t, struct Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp*
e){ int is_string= 0;{ void* _temp508=( void*) e->r; void* _temp514; _LL510: if(*((
int*) _temp508) == Cyc_Absyn_Const_e){ _LL515: _temp514=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp508)->f1; if(( unsigned int) _temp514 > 1u?*(( int*) _temp514) == Cyc_Absyn_String_c:
0){ goto _LL511;} else{ goto _LL512;}} else{ goto _LL512;} _LL512: goto _LL513;
_LL511: is_string= 1; goto _LL509; _LL513: goto _LL509; _LL509:;}{ struct Cyc_Absyn_Exp*
xexp; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var(); void* vd_typ=( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp519=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp519[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp520; _temp520.tag= Cyc_Absyn_ArrayType; _temp520.f1=(
void*) Cyc_Absyn_uchar_t; _temp520.f2= Cyc_Toc_mt_tq; _temp520.f3=( struct Cyc_Absyn_Exp*)
sz; _temp520;}); _temp519;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp516=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp516->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp517=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp517[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp518; _temp518.tag= Cyc_Absyn_Var_d;
_temp518.f1= vd; _temp518;}); _temp517;}), 0); _temp516->tl= Cyc_Toc_result_decls;
_temp516;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ xexp= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), e, 0);}{ struct Cyc_Absyn_Exp* xplussz= Cyc_Absyn_add_exp(
xexp, sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp( 0,({
struct _tuple4* _temp521[ 3u]; _temp521[ 2u]=({ struct _tuple4* _temp524=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp524->f1= 0; _temp524->f2=
xplussz; _temp524;}); _temp521[ 1u]=({ struct _tuple4* _temp523=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp523->f1= 0; _temp523->f2= xexp;
_temp523;}); _temp521[ 0u]=({ struct _tuple4* _temp522=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp522->f1= 0; _temp522->f2= xexp;
_temp522;});(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp521, sizeof( struct _tuple4*), 3u));}), 0); return urm_exp;}}}
struct Cyc_Toc_Env{ struct Cyc_Core_Opt* break_lab; struct Cyc_Core_Opt*
continue_lab; struct Cyc_Core_Opt* fallthru_info; struct Cyc_Dict_Dict* varmap;
int toplevel; } ; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){ return({
struct Cyc_Toc_Env* _temp525=( struct Cyc_Toc_Env*) GC_malloc( sizeof( struct
Cyc_Toc_Env)); _temp525->break_lab= 0; _temp525->continue_lab= 0; _temp525->fallthru_info=
0; _temp525->varmap=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp525->toplevel= 1;
_temp525;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){
return({ struct Cyc_Toc_Env* _temp526=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp526->break_lab= e->break_lab; _temp526->continue_lab=
e->continue_lab; _temp526->fallthru_info= e->fallthru_info; _temp526->varmap= e->varmap;
_temp526->toplevel= e->toplevel; _temp526;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp527=(* x).f1; _LL529:
if(( unsigned int) _temp527 > 1u?*(( int*) _temp527) == Cyc_Absyn_Rel_n: 0){
goto _LL530;} else{ goto _LL531;} _LL531: goto _LL532; _LL530:( int) _throw(({
struct Cyc_Stdio_String_pa_struct _temp534; _temp534.tag= Cyc_Stdio_String_pa;
_temp534.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( x);{ void* _temp533[
1u]={& _temp534}; Cyc_Toc_toc_impos( _tag_arr("Toc::add_varmap on Rel_n: %s\n",
sizeof( unsigned char), 30u), _tag_arr( _temp533, sizeof( void*), 1u));}}));
_LL532: goto _LL528; _LL528:;}{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->varmap=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0*
key, struct Cyc_Absyn_Exp* data)) Cyc_Dict_insert)( ans->varmap, x, y); return
ans;}} static struct Cyc_Toc_Env* Cyc_Toc_loop_env( struct Cyc_Toc_Env* e){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->continue_lab=
0; return ans;} struct _tuple6{ struct _tagged_arr* f1; struct Cyc_List_List* f2;
struct Cyc_Dict_Dict* f3; } ; static struct Cyc_Toc_Env* Cyc_Toc_non_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_arr* break_l, struct _tagged_arr*
fallthru_l, struct Cyc_List_List* fallthru_binders, struct Cyc_Toc_Env*
next_case_env){ struct Cyc_List_List* _temp535= 0; for( 0; fallthru_binders != 0;
fallthru_binders=(( struct Cyc_List_List*) _check_null( fallthru_binders))->tl){
_temp535=({ struct Cyc_List_List* _temp536=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp536->hd=( void*)(( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( fallthru_binders))->hd)->name; _temp536->tl=
_temp535; _temp536;});} _temp535=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp535);{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab=({ struct Cyc_Core_Opt* _temp537=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp537->v=( void*) break_l; _temp537;});
ans->fallthru_info=({ struct Cyc_Core_Opt* _temp538=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp538->v=( void*)({ struct _tuple6*
_temp539=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp539->f1=
fallthru_l; _temp539->f2= _temp535; _temp539->f3= next_case_env->varmap;
_temp539;}); _temp538;}); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_arr* break_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt* _temp540=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp540->v=( void*)
break_l; _temp540;}); ans->fallthru_info= 0; return ans;} static struct Cyc_Toc_Env*
Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env* e, struct _tagged_arr* next_l){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->fallthru_info=({
struct Cyc_Core_Opt* _temp541=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp541->v=( void*)({ struct _tuple6* _temp542=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp542->f1= next_l; _temp542->f2= 0;
_temp542->f3=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct
_tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp542;}); _temp541;});
return ans;} static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e); static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt*
s); static void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp* e){ if( e->topt == 0){({
void* _temp543[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("Missing type in primop ", sizeof( unsigned char),
24u), _tag_arr( _temp543, sizeof( void*), 0u));});} return Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);} static void* Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp* e){ if( e->topt == 0){({ void* _temp544[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("Missing type in primop ",
sizeof( unsigned char), 24u), _tag_arr( _temp544, sizeof( void*), 0u));});}
return( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v;} static struct
_tuple3* Cyc_Toc_tup_to_c( struct Cyc_Absyn_Exp* e){ return({ struct _tuple3*
_temp545=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp545->f1=
Cyc_Toc_mt_tq; _temp545->f2= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); _temp545;});} static struct _tuple4* Cyc_Toc_add_designator(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e);
return({ struct _tuple4* _temp546=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp546->f1= 0; _temp546->f2= e; _temp546;});} static struct Cyc_Absyn_Exp*
Cyc_Toc_make_struct( struct Cyc_Toc_Env* nv, struct _tuple0* x, void* struct_typ,
struct Cyc_Absyn_Stmt* s, int pointer, struct Cyc_Absyn_Exp* rgnopt, int
is_atomic){ struct Cyc_Absyn_Exp* eo; void* t; if( pointer){ t= Cyc_Absyn_cstar_typ(
struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp* _temp547= Cyc_Absyn_sizeoftyp_exp(
struct_typ, 0); if( rgnopt == 0){ eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp(
t, is_atomic? Cyc_Toc_malloc_atomic( _temp547): Cyc_Toc_malloc_ptr( _temp547), 0);}
else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( rgnopt); Cyc_Toc_exp_to_c(
nv, r); eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp(
r, _temp547), 0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) - 1;{ struct Cyc_List_List* _temp548=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( dles0); for( 0; _temp548 != 0; _temp548=((
struct Cyc_List_List*) _check_null( _temp548))->tl){ struct _tuple4 _temp551;
struct Cyc_Absyn_Exp* _temp552; struct Cyc_List_List* _temp554; struct _tuple4*
_temp549=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp548))->hd;
_temp551=* _temp549; _LL555: _temp554= _temp551.f1; goto _LL553; _LL553:
_temp552= _temp551.f2; goto _LL550; _LL550: { struct Cyc_Absyn_Exp* e_index; if(
_temp554 == 0){ e_index= Cyc_Absyn_signed_int_exp( count --, 0);} else{ if(((
struct Cyc_List_List*) _check_null( _temp554))->tl != 0){({ void* _temp556[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("multiple designators in array",
sizeof( unsigned char), 30u), _tag_arr( _temp556, sizeof( void*), 0u));});}{
void* _temp557=( void*)(( struct Cyc_List_List*) _check_null( _temp554))->hd;
void* _temp558= _temp557; struct Cyc_Absyn_Exp* _temp564; _LL560: if(*(( int*)
_temp558) == Cyc_Absyn_ArrayElement){ _LL565: _temp564=(( struct Cyc_Absyn_ArrayElement_struct*)
_temp558)->f1; goto _LL561;} else{ goto _LL562;} _LL562: if(*(( int*) _temp558)
== Cyc_Absyn_FieldName){ goto _LL563;} else{ goto _LL559;} _LL561: Cyc_Toc_exp_to_c(
nv, _temp564); e_index= _temp564; goto _LL559; _LL563: e_index=({ void* _temp566[
0u]={};(( struct Cyc_Absyn_Exp*(*)( struct _tagged_arr fmt, struct _tagged_arr
ap)) Cyc_Toc_unimp)( _tag_arr("field name designators in array", sizeof(
unsigned char), 32u), _tag_arr( _temp566, sizeof( void*), 0u));}); goto _LL559;
_LL559:;}}{ struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp( lhs, e_index, 0);
void* _temp567=( void*) _temp552->r; struct Cyc_List_List* _temp577; struct Cyc_Absyn_Exp*
_temp579; struct Cyc_Absyn_Exp* _temp581; struct Cyc_Absyn_Vardecl* _temp583;
struct Cyc_List_List* _temp585; void* _temp587; _LL569: if(*(( int*) _temp567)
== Cyc_Absyn_Array_e){ _LL578: _temp577=(( struct Cyc_Absyn_Array_e_struct*)
_temp567)->f1; goto _LL570;} else{ goto _LL571;} _LL571: if(*(( int*) _temp567)
== Cyc_Absyn_Comprehension_e){ _LL584: _temp583=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp567)->f1; goto _LL582; _LL582: _temp581=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp567)->f2; goto _LL580; _LL580: _temp579=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp567)->f3; goto _LL572;} else{ goto _LL573;} _LL573: if(*(( int*) _temp567)
== Cyc_Absyn_AnonStruct_e){ _LL588: _temp587=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp567)->f1; goto _LL586; _LL586: _temp585=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp567)->f2; goto _LL574;} else{ goto _LL575;} _LL575: goto _LL576; _LL570: s=
Cyc_Toc_init_array( nv, lval, _temp577, s); goto _LL568; _LL572: s= Cyc_Toc_init_comprehension(
nv, lval, _temp583, _temp581, _temp579, s, 0); goto _LL568; _LL574: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp587, _temp585, s); goto _LL568; _LL576: Cyc_Toc_exp_to_c( nv,
_temp552); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
lhs, e_index, 0), _temp552, 0), s, 0); goto _LL568; _LL568:;}}}} return s;}
static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s, int
e1_already_translated){ struct _tuple0* _temp589= vd->name; if( !
e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env* nv2=
Cyc_Toc_add_varmap( nv, _temp589, Cyc_Absyn_varb_exp( _temp589,( void*)({ struct
Cyc_Absyn_Local_b_struct* _temp612=( struct Cyc_Absyn_Local_b_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Local_b_struct)); _temp612[ 0]=({ struct Cyc_Absyn_Local_b_struct
_temp613; _temp613.tag= Cyc_Absyn_Local_b; _temp613.f1= vd; _temp613;});
_temp612;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( _temp589, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp(
_temp589, 0), Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp(
Cyc_Absyn_var_exp( _temp589, 0), 0); struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp(
lhs, Cyc_Absyn_var_exp( _temp589, 0), 0); struct Cyc_Absyn_Stmt* body;{ void*
_temp590=( void*) e2->r; struct Cyc_List_List* _temp600; struct Cyc_Absyn_Exp*
_temp602; struct Cyc_Absyn_Exp* _temp604; struct Cyc_Absyn_Vardecl* _temp606;
struct Cyc_List_List* _temp608; void* _temp610; _LL592: if(*(( int*) _temp590)
== Cyc_Absyn_Array_e){ _LL601: _temp600=(( struct Cyc_Absyn_Array_e_struct*)
_temp590)->f1; goto _LL593;} else{ goto _LL594;} _LL594: if(*(( int*) _temp590)
== Cyc_Absyn_Comprehension_e){ _LL607: _temp606=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp590)->f1; goto _LL605; _LL605: _temp604=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp590)->f2; goto _LL603; _LL603: _temp602=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp590)->f3; goto _LL595;} else{ goto _LL596;} _LL596: if(*(( int*) _temp590)
== Cyc_Absyn_AnonStruct_e){ _LL611: _temp610=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp590)->f1; goto _LL609; _LL609: _temp608=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp590)->f2; goto _LL597;} else{ goto _LL598;} _LL598: goto _LL599; _LL593:
body= Cyc_Toc_init_array( nv2, lval, _temp600, Cyc_Absyn_skip_stmt( 0)); goto
_LL591; _LL595: body= Cyc_Toc_init_comprehension( nv2, lval, _temp606, _temp604,
_temp602, Cyc_Absyn_skip_stmt( 0), 0); goto _LL591; _LL597: body= Cyc_Toc_init_anon_struct(
nv, lval, _temp610, _temp608, Cyc_Absyn_skip_stmt( 0)); goto _LL591; _LL599: Cyc_Toc_exp_to_c(
nv2, e2); body= Cyc_Absyn_assign_stmt( lval, e2, 0); goto _LL591; _LL591:;}{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt( ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt(
_temp589, Cyc_Absyn_uint_t, 0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_anon_struct( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs,
void* struct_type, struct Cyc_List_List* dles, struct Cyc_Absyn_Stmt* s){{
struct Cyc_List_List* _temp614=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( dles); for( 0; _temp614 != 0; _temp614=(( struct Cyc_List_List*)
_check_null( _temp614))->tl){ struct _tuple4 _temp617; struct Cyc_Absyn_Exp*
_temp618; struct Cyc_List_List* _temp620; struct _tuple4* _temp615=( struct
_tuple4*)(( struct Cyc_List_List*) _check_null( _temp614))->hd; _temp617=*
_temp615; _LL621: _temp620= _temp617.f1; goto _LL619; _LL619: _temp618= _temp617.f2;
goto _LL616; _LL616: if( _temp620 == 0){({ void* _temp622[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("empty designator list",
sizeof( unsigned char), 22u), _tag_arr( _temp622, sizeof( void*), 0u));});} if(((
struct Cyc_List_List*) _check_null( _temp620))->tl != 0){({ void* _temp623[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("too many designators in anonymous struct", sizeof( unsigned char), 41u),
_tag_arr( _temp623, sizeof( void*), 0u));});}{ void* _temp624=( void*)(( struct
Cyc_List_List*) _check_null( _temp620))->hd; struct _tagged_arr* _temp630;
_LL626: if(*(( int*) _temp624) == Cyc_Absyn_FieldName){ _LL631: _temp630=((
struct Cyc_Absyn_FieldName_struct*) _temp624)->f1; goto _LL627;} else{ goto
_LL628;} _LL628: goto _LL629; _LL627: { struct Cyc_Absyn_Exp* lval= Cyc_Absyn_structmember_exp(
lhs, _temp630, 0);{ void* _temp632=( void*) _temp618->r; struct Cyc_List_List*
_temp642; struct Cyc_Absyn_Exp* _temp644; struct Cyc_Absyn_Exp* _temp646; struct
Cyc_Absyn_Vardecl* _temp648; struct Cyc_List_List* _temp650; void* _temp652;
_LL634: if(*(( int*) _temp632) == Cyc_Absyn_Array_e){ _LL643: _temp642=(( struct
Cyc_Absyn_Array_e_struct*) _temp632)->f1; goto _LL635;} else{ goto _LL636;}
_LL636: if(*(( int*) _temp632) == Cyc_Absyn_Comprehension_e){ _LL649: _temp648=((
struct Cyc_Absyn_Comprehension_e_struct*) _temp632)->f1; goto _LL647; _LL647:
_temp646=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp632)->f2; goto _LL645;
_LL645: _temp644=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp632)->f3;
goto _LL637;} else{ goto _LL638;} _LL638: if(*(( int*) _temp632) == Cyc_Absyn_AnonStruct_e){
_LL653: _temp652=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp632)->f1;
goto _LL651; _LL651: _temp650=(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp632)->f2;
goto _LL639;} else{ goto _LL640;} _LL640: goto _LL641; _LL635: s= Cyc_Toc_init_array(
nv, lval, _temp642, s); goto _LL633; _LL637: s= Cyc_Toc_init_comprehension( nv,
lval, _temp648, _temp646, _temp644, s, 0); goto _LL633; _LL639: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp652, _temp650, s); goto _LL633; _LL641: Cyc_Toc_exp_to_c( nv,
_temp618); if( Cyc_Toc_is_poly_field( struct_type, _temp630)){ _temp618= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp618, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp618, 0), 0), s, 0); goto _LL633; _LL633:;} goto
_LL625;} _LL629:( int) _throw(({ void* _temp654[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u), _tag_arr(
_temp654, sizeof( void*), 0u));})); _LL625:;}}} return s;} static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple( struct Cyc_Toc_Env* nv, int pointer, struct Cyc_Absyn_Exp*
rgnopt, struct Cyc_List_List* es){ struct _RegionHandle _temp655= _new_region();
struct _RegionHandle* r=& _temp655; _push_region( r);{ struct Cyc_List_List*
_temp656=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple3*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_rmap)( r, Cyc_Toc_tup_to_c,
es); struct _tagged_arr* _temp657= Cyc_Toc_add_tuple_type( _temp656); struct
_tuple0* _temp658= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp659= Cyc_Absyn_var_exp(
_temp658, 0); struct Cyc_Absyn_Stmt* _temp660= Cyc_Absyn_exp_stmt( _temp659, 0);
struct Cyc_Absyn_Exp*(* _temp661)( struct Cyc_Absyn_Exp*, struct _tagged_arr*,
struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp;
int is_atomic= 1; struct Cyc_List_List* _temp662=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, es);{ int i=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp662); for( 0; _temp662
!= 0;( _temp662=(( struct Cyc_List_List*) _check_null( _temp662))->tl, -- i)){
struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp662))->hd; struct Cyc_Absyn_Exp* lval= _temp661( _temp659, Cyc_Toc_fieldname(
i), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v): 0;{ void* _temp663=( void*) e->r; struct Cyc_List_List*
_temp671; struct Cyc_Absyn_Exp* _temp673; struct Cyc_Absyn_Exp* _temp675; struct
Cyc_Absyn_Vardecl* _temp677; _LL665: if(*(( int*) _temp663) == Cyc_Absyn_Array_e){
_LL672: _temp671=(( struct Cyc_Absyn_Array_e_struct*) _temp663)->f1; goto _LL666;}
else{ goto _LL667;} _LL667: if(*(( int*) _temp663) == Cyc_Absyn_Comprehension_e){
_LL678: _temp677=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp663)->f1;
goto _LL676; _LL676: _temp675=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp663)->f2; goto _LL674; _LL674: _temp673=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp663)->f3; goto _LL668;} else{ goto _LL669;} _LL669: goto _LL670; _LL666:
_temp660= Cyc_Toc_init_array( nv, lval, _temp671, _temp660); goto _LL664; _LL668:
_temp660= Cyc_Toc_init_comprehension( nv, lval, _temp677, _temp675, _temp673,
_temp660, 0); goto _LL664; _LL670: Cyc_Toc_exp_to_c( nv, e); _temp660= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp661( _temp659, Cyc_Toc_fieldname(
i), 0), e, 0), 0), _temp660, 0); goto _LL664; _LL664:;}}}{ struct Cyc_Absyn_Exp*
_temp679= Cyc_Toc_make_struct( nv, _temp658, Cyc_Absyn_strct( _temp657),
_temp660, pointer, rgnopt, is_atomic); _npop_handler( 0u); return _temp679;}};
_pop_region( r);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env*
nv, void* struct_type, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
dles, struct _tuple0* tdn){ struct _tuple0* _temp680= Cyc_Toc_temp_var(); struct
Cyc_Absyn_Exp* _temp681= Cyc_Absyn_var_exp( _temp680, 0); struct Cyc_Absyn_Stmt*
_temp682= Cyc_Absyn_exp_stmt( _temp681, 0); struct Cyc_Absyn_Exp*(* _temp683)(
struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; int is_atomic= 1;
struct _RegionHandle _temp684= _new_region(); struct _RegionHandle* r=& _temp684;
_push_region( r);{ struct Cyc_List_List* _temp685=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, dles); for(
0; _temp685 != 0; _temp685=(( struct Cyc_List_List*) _check_null( _temp685))->tl){
struct _tuple4 _temp688; struct Cyc_Absyn_Exp* _temp689; struct Cyc_List_List*
_temp691; struct _tuple4* _temp686=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp685))->hd; _temp688=* _temp686; _LL692: _temp691= _temp688.f1;
goto _LL690; _LL690: _temp689= _temp688.f2; goto _LL687; _LL687: is_atomic=
is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp689->topt))->v): 0; if( _temp691 == 0){({ void* _temp693[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("empty designator list",
sizeof( unsigned char), 22u), _tag_arr( _temp693, sizeof( void*), 0u));});} if(((
struct Cyc_List_List*) _check_null( _temp691))->tl != 0){ struct _tuple0*
_temp694= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp695= Cyc_Absyn_var_exp(
_temp694, 0); for( 0; _temp691 != 0; _temp691=(( struct Cyc_List_List*)
_check_null( _temp691))->tl){ void* _temp696=( void*)(( struct Cyc_List_List*)
_check_null( _temp691))->hd; struct _tagged_arr* _temp702; _LL698: if(*(( int*)
_temp696) == Cyc_Absyn_FieldName){ _LL703: _temp702=(( struct Cyc_Absyn_FieldName_struct*)
_temp696)->f1; goto _LL699;} else{ goto _LL700;} _LL700: goto _LL701; _LL699:
if( Cyc_Toc_is_poly_field( struct_type, _temp702)){ _temp695= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp695, 0);} _temp682= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( _temp683( _temp681, _temp702, 0), _temp695, 0), 0),
_temp682, 0); goto _LL697; _LL701:( int) _throw(({ void* _temp704[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u), _tag_arr(
_temp704, sizeof( void*), 0u));})); _LL697:;} Cyc_Toc_exp_to_c( nv, _temp689);
_temp682= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp695,
_temp689, 0), 0), _temp682, 0);} else{ void* _temp705=( void*)(( struct Cyc_List_List*)
_check_null( _temp691))->hd; struct _tagged_arr* _temp711; _LL707: if(*(( int*)
_temp705) == Cyc_Absyn_FieldName){ _LL712: _temp711=(( struct Cyc_Absyn_FieldName_struct*)
_temp705)->f1; goto _LL708;} else{ goto _LL709;} _LL709: goto _LL710; _LL708: {
struct Cyc_Absyn_Exp* lval= _temp683( _temp681, _temp711, 0);{ void* _temp713=(
void*) _temp689->r; struct Cyc_List_List* _temp723; struct Cyc_Absyn_Exp*
_temp725; struct Cyc_Absyn_Exp* _temp727; struct Cyc_Absyn_Vardecl* _temp729;
struct Cyc_List_List* _temp731; void* _temp733; _LL715: if(*(( int*) _temp713)
== Cyc_Absyn_Array_e){ _LL724: _temp723=(( struct Cyc_Absyn_Array_e_struct*)
_temp713)->f1; goto _LL716;} else{ goto _LL717;} _LL717: if(*(( int*) _temp713)
== Cyc_Absyn_Comprehension_e){ _LL730: _temp729=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp713)->f1; goto _LL728; _LL728: _temp727=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp713)->f2; goto _LL726; _LL726: _temp725=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp713)->f3; goto _LL718;} else{ goto _LL719;} _LL719: if(*(( int*) _temp713)
== Cyc_Absyn_AnonStruct_e){ _LL734: _temp733=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp713)->f1; goto _LL732; _LL732: _temp731=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp713)->f2; goto _LL720;} else{ goto _LL721;} _LL721: goto _LL722; _LL716:
_temp682= Cyc_Toc_init_array( nv, lval, _temp723, _temp682); goto _LL714; _LL718:
_temp682= Cyc_Toc_init_comprehension( nv, lval, _temp729, _temp727, _temp725,
_temp682, 0); goto _LL714; _LL720: _temp682= Cyc_Toc_init_anon_struct( nv, lval,
_temp733, _temp731, _temp682); goto _LL714; _LL722: Cyc_Toc_exp_to_c( nv,
_temp689); if( Cyc_Toc_is_poly_field( struct_type, _temp711)){ _temp689= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp689, 0);} _temp682= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp689, 0), 0), _temp682, 0); goto _LL714; _LL714:;}
goto _LL706;} _LL710:( int) _throw(({ void* _temp735[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u), _tag_arr(
_temp735, sizeof( void*), 0u));})); _LL706:;}}}{ struct Cyc_Absyn_Exp* _temp736=
Cyc_Toc_make_struct( nv, _temp680, Cyc_Absyn_strctq( tdn), _temp682, pointer,
rgnopt, is_atomic); _npop_handler( 0u); return _temp736;}; _pop_region( r);}
struct _tuple7{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ; static
struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp* el, struct
_tuple7* pr){ return Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2, 0);} static
struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp* e1, int
ignore){ return Cyc_Absyn_address_exp( e1, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue(
struct Cyc_Absyn_Exp* e1, void* incr){ return Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Increment_e_struct* _temp737=( struct Cyc_Absyn_Increment_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp737[ 0]=({ struct
Cyc_Absyn_Increment_e_struct _temp738; _temp738.tag= Cyc_Absyn_Increment_e;
_temp738.f1= e1; _temp738.f2=( void*) incr; _temp738;}); _temp737;}), 0);}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env);
static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp739=( void*) e1->r; struct Cyc_Absyn_Stmt* _temp749; struct Cyc_Absyn_Exp*
_temp751; void* _temp753; struct _tagged_arr* _temp755; struct Cyc_Absyn_Exp*
_temp757; _LL741: if(*(( int*) _temp739) == Cyc_Absyn_StmtExp_e){ _LL750:
_temp749=(( struct Cyc_Absyn_StmtExp_e_struct*) _temp739)->f1; goto _LL742;}
else{ goto _LL743;} _LL743: if(*(( int*) _temp739) == Cyc_Absyn_Cast_e){ _LL754:
_temp753=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp739)->f1; goto _LL752;
_LL752: _temp751=(( struct Cyc_Absyn_Cast_e_struct*) _temp739)->f2; goto _LL744;}
else{ goto _LL745;} _LL745: if(*(( int*) _temp739) == Cyc_Absyn_StructMember_e){
_LL758: _temp757=(( struct Cyc_Absyn_StructMember_e_struct*) _temp739)->f1; goto
_LL756; _LL756: _temp755=(( struct Cyc_Absyn_StructMember_e_struct*) _temp739)->f2;
goto _LL746;} else{ goto _LL747;} _LL747: goto _LL748; _LL742: Cyc_Toc_lvalue_assign_stmt(
_temp749, fs, f, f_env); goto _LL740; _LL744: Cyc_Toc_lvalue_assign( _temp751,
fs, f, f_env); goto _LL740; _LL746:( void*)( e1->r=( void*)(( void*) _temp757->r));
Cyc_Toc_lvalue_assign( e1,({ struct Cyc_List_List* _temp759=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp759->hd=( void*) _temp755;
_temp759->tl= fs; _temp759;}), f, f_env); goto _LL740; _LL748: if( ! Cyc_Absyn_is_lvalue(
e1)){({ struct Cyc_Stdio_String_pa_struct _temp761; _temp761.tag= Cyc_Stdio_String_pa;
_temp761.f1=( struct _tagged_arr) Cyc_Absynpp_exp2string( e1);{ void* _temp760[
1u]={& _temp761};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("lvalue_assign: could not find lvalue in `%s'", sizeof( unsigned char),
45u), _tag_arr( _temp760, sizeof( void*), 1u));}});}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct
_tagged_arr*)(( struct Cyc_List_List*) _check_null( fs))->hd, e1_copy->loc);}(
void*)( e1->r=( void*)(( void*)( f( e1_copy, f_env))->r)); goto _LL740;} _LL740:;}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp762=( void*) s->r; struct Cyc_Absyn_Exp* _temp772; struct Cyc_Absyn_Stmt*
_temp774; struct Cyc_Absyn_Decl* _temp776; struct Cyc_Absyn_Stmt* _temp778;
_LL764: if(( unsigned int) _temp762 > 1u?*(( int*) _temp762) == Cyc_Absyn_Exp_s:
0){ _LL773: _temp772=(( struct Cyc_Absyn_Exp_s_struct*) _temp762)->f1; goto
_LL765;} else{ goto _LL766;} _LL766: if(( unsigned int) _temp762 > 1u?*(( int*)
_temp762) == Cyc_Absyn_Decl_s: 0){ _LL777: _temp776=(( struct Cyc_Absyn_Decl_s_struct*)
_temp762)->f1; goto _LL775; _LL775: _temp774=(( struct Cyc_Absyn_Decl_s_struct*)
_temp762)->f2; goto _LL767;} else{ goto _LL768;} _LL768: if(( unsigned int)
_temp762 > 1u?*(( int*) _temp762) == Cyc_Absyn_Seq_s: 0){ _LL779: _temp778=((
struct Cyc_Absyn_Seq_s_struct*) _temp762)->f2; goto _LL769;} else{ goto _LL770;}
_LL770: goto _LL771; _LL765: Cyc_Toc_lvalue_assign( _temp772, fs, f, f_env);
goto _LL763; _LL767: Cyc_Toc_lvalue_assign_stmt( _temp774, fs, f, f_env); goto
_LL763; _LL769: Cyc_Toc_lvalue_assign_stmt( _temp778, fs, f, f_env); goto _LL763;
_LL771:({ struct Cyc_Stdio_String_pa_struct _temp781; _temp781.tag= Cyc_Stdio_String_pa;
_temp781.f1=( struct _tagged_arr) Cyc_Absynpp_stmt2string( s);{ void* _temp780[
1u]={& _temp781};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("lvalue_assign_stmt: %s", sizeof( unsigned char), 23u), _tag_arr(
_temp780, sizeof( void*), 1u));}}); goto _LL763; _LL763:;} static struct Cyc_List_List*
Cyc_Toc_rmap_2c( struct _RegionHandle* r2, void*(* f)( void*, void*), void* env,
struct Cyc_List_List* x){ struct Cyc_List_List* result; struct Cyc_List_List*
prev; if( x == 0){ return 0;} result=({ struct Cyc_List_List* _temp782=( struct
Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List)); _temp782->hd=(
void*) f(( void*)(( struct Cyc_List_List*) _check_null( x))->hd, env); _temp782->tl=
0; _temp782;}); prev= result; for( x=(( struct Cyc_List_List*) _check_null( x))->tl;
x != 0; x=(( struct Cyc_List_List*) _check_null( x))->tl){(( struct Cyc_List_List*)
_check_null( prev))->tl=({ struct Cyc_List_List* _temp783=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp783->hd=( void*) f((
void*)(( struct Cyc_List_List*) _check_null( x))->hd, env); _temp783->tl= 0;
_temp783;}); prev=(( struct Cyc_List_List*) _check_null( prev))->tl;} return
result;} static struct Cyc_List_List* Cyc_Toc_map_2c( void*(* f)( void*, void*),
void* env, struct Cyc_List_List* x){ return Cyc_Toc_rmap_2c( Cyc_Core_heap_region,
f, env, x);} static struct _tuple4* Cyc_Toc_make_dle( struct Cyc_Absyn_Exp* e){
return({ struct _tuple4* _temp784=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp784->f1= 0; _temp784->f2= e; _temp784;});} static struct Cyc_Absyn_PtrInfo
Cyc_Toc_get_ptr_type( void* t){ void* _temp785= Cyc_Tcutil_compress( t); struct
Cyc_Absyn_PtrInfo _temp791; _LL787: if(( unsigned int) _temp785 > 4u?*(( int*)
_temp785) == Cyc_Absyn_PointerType: 0){ _LL792: _temp791=(( struct Cyc_Absyn_PointerType_struct*)
_temp785)->f1; goto _LL788;} else{ goto _LL789;} _LL789: goto _LL790; _LL788:
return _temp791; _LL790:( int) _throw(({ void* _temp793[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("get_ptr_typ: not a pointer!", sizeof( unsigned char), 28u), _tag_arr(
_temp793, sizeof( void*), 0u));})); _LL786:;} struct _tuple8{ void* f1; void* f2;
} ; struct _tuple9{ struct _tuple0* f1; void* f2; struct Cyc_Absyn_Exp* f3; } ;
struct _tuple10{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Exp* f2; } ;
static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e){
void* _temp794=( void*) e->r; if( e->topt == 0){({ struct Cyc_Stdio_String_pa_struct
_temp796; _temp796.tag= Cyc_Stdio_String_pa; _temp796.f1=( struct _tagged_arr)
Cyc_Absynpp_exp2string( e);{ void* _temp795[ 1u]={& _temp796};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: no type for %s",
sizeof( unsigned char), 25u), _tag_arr( _temp795, sizeof( void*), 1u));}});
return;}{ void* old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v;
void* _temp797= _temp794; void* _temp877; void* _temp879; struct _tuple0*
_temp881; struct _tuple0* _temp883; struct Cyc_List_List* _temp885; void*
_temp887; void* _temp889; struct Cyc_Absyn_Exp* _temp891; struct Cyc_Absyn_Exp*
_temp893; struct Cyc_Core_Opt* _temp895; struct Cyc_Absyn_Exp* _temp897; struct
Cyc_Absyn_Exp* _temp899; struct Cyc_Absyn_Exp* _temp901; struct Cyc_Absyn_Exp*
_temp903; struct Cyc_Absyn_Exp* _temp905; struct Cyc_Absyn_Exp* _temp907; struct
Cyc_List_List* _temp909; struct Cyc_Absyn_Exp* _temp911; struct Cyc_Absyn_VarargCallInfo*
_temp913; struct Cyc_List_List* _temp915; struct Cyc_Absyn_Exp* _temp917; struct
Cyc_Absyn_VarargCallInfo* _temp919; struct Cyc_Absyn_VarargCallInfo _temp921;
struct Cyc_Absyn_VarargInfo* _temp922; struct Cyc_List_List* _temp924; int
_temp926; struct Cyc_List_List* _temp928; struct Cyc_Absyn_Exp* _temp930; struct
Cyc_Absyn_Exp* _temp932; struct Cyc_Absyn_Exp* _temp934; struct Cyc_List_List*
_temp936; struct Cyc_Absyn_Exp* _temp938; struct Cyc_Absyn_Exp* _temp940; void*
_temp942; void** _temp944; struct Cyc_Absyn_Exp* _temp945; struct Cyc_Absyn_Exp*
_temp947; struct Cyc_Absyn_Exp* _temp949; struct Cyc_Absyn_Exp* _temp951; void*
_temp953; struct _tagged_arr* _temp955; void* _temp957; struct Cyc_Absyn_Exp*
_temp959; struct _tagged_arr* _temp961; struct Cyc_Absyn_Exp* _temp963; struct
_tagged_arr* _temp965; struct Cyc_Absyn_Exp* _temp967; struct Cyc_Absyn_Exp*
_temp969; struct Cyc_Absyn_Exp* _temp971; struct Cyc_List_List* _temp973; struct
Cyc_List_List* _temp975; struct Cyc_Absyn_Exp* _temp977; struct Cyc_Absyn_Exp*
_temp979; struct Cyc_Absyn_Vardecl* _temp981; struct Cyc_Absyn_Structdecl*
_temp983; struct Cyc_List_List* _temp985; struct Cyc_Core_Opt* _temp987; struct
_tuple0* _temp989; struct Cyc_List_List* _temp991; void* _temp993; struct Cyc_Absyn_Tunionfield*
_temp995; struct Cyc_Absyn_Tuniondecl* _temp997; struct Cyc_List_List* _temp999;
struct Cyc_Absyn_Tunionfield* _temp1001; struct Cyc_Absyn_Tuniondecl* _temp1003;
struct Cyc_List_List* _temp1005; void* _temp1007; struct Cyc_Absyn_Exp*
_temp1009; struct Cyc_Absyn_Stmt* _temp1011; struct Cyc_Absyn_Fndecl* _temp1013;
_LL799: if(*(( int*) _temp797) == Cyc_Absyn_Const_e){ _LL878: _temp877=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp797)->f1; if( _temp877 ==( void*) Cyc_Absyn_Null_c){
goto _LL800;} else{ goto _LL801;}} else{ goto _LL801;} _LL801: if(*(( int*)
_temp797) == Cyc_Absyn_Const_e){ goto _LL802;} else{ goto _LL803;} _LL803: if(*((
int*) _temp797) == Cyc_Absyn_Var_e){ _LL882: _temp881=(( struct Cyc_Absyn_Var_e_struct*)
_temp797)->f1; goto _LL880; _LL880: _temp879=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp797)->f2; goto _LL804;} else{ goto _LL805;} _LL805: if(*(( int*) _temp797)
== Cyc_Absyn_UnknownId_e){ _LL884: _temp883=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp797)->f1; goto _LL806;} else{ goto _LL807;} _LL807: if(*(( int*) _temp797)
== Cyc_Absyn_Primop_e){ _LL888: _temp887=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp797)->f1; goto _LL886; _LL886: _temp885=(( struct Cyc_Absyn_Primop_e_struct*)
_temp797)->f2; goto _LL808;} else{ goto _LL809;} _LL809: if(*(( int*) _temp797)
== Cyc_Absyn_Increment_e){ _LL892: _temp891=(( struct Cyc_Absyn_Increment_e_struct*)
_temp797)->f1; goto _LL890; _LL890: _temp889=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp797)->f2; goto _LL810;} else{ goto _LL811;} _LL811: if(*(( int*) _temp797)
== Cyc_Absyn_AssignOp_e){ _LL898: _temp897=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp797)->f1; goto _LL896; _LL896: _temp895=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp797)->f2; goto _LL894; _LL894: _temp893=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp797)->f3; goto _LL812;} else{ goto _LL813;} _LL813: if(*(( int*) _temp797)
== Cyc_Absyn_Conditional_e){ _LL904: _temp903=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp797)->f1; goto _LL902; _LL902: _temp901=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp797)->f2; goto _LL900; _LL900: _temp899=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp797)->f3; goto _LL814;} else{ goto _LL815;} _LL815: if(*(( int*) _temp797)
== Cyc_Absyn_SeqExp_e){ _LL908: _temp907=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp797)->f1; goto _LL906; _LL906: _temp905=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp797)->f2; goto _LL816;} else{ goto _LL817;} _LL817: if(*(( int*) _temp797)
== Cyc_Absyn_UnknownCall_e){ _LL912: _temp911=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp797)->f1; goto _LL910; _LL910: _temp909=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp797)->f2; goto _LL818;} else{ goto _LL819;} _LL819: if(*(( int*) _temp797)
== Cyc_Absyn_FnCall_e){ _LL918: _temp917=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp797)->f1; goto _LL916; _LL916: _temp915=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp797)->f2; goto _LL914; _LL914: _temp913=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp797)->f3; if( _temp913 == 0){ goto _LL820;} else{ goto _LL821;}} else{ goto
_LL821;} _LL821: if(*(( int*) _temp797) == Cyc_Absyn_FnCall_e){ _LL931: _temp930=((
struct Cyc_Absyn_FnCall_e_struct*) _temp797)->f1; goto _LL929; _LL929: _temp928=((
struct Cyc_Absyn_FnCall_e_struct*) _temp797)->f2; goto _LL920; _LL920: _temp919=((
struct Cyc_Absyn_FnCall_e_struct*) _temp797)->f3; if( _temp919 == 0){ goto
_LL823;} else{ _temp921=* _temp919; _LL927: _temp926= _temp921.num_varargs; goto
_LL925; _LL925: _temp924= _temp921.injectors; goto _LL923; _LL923: _temp922=
_temp921.vai; goto _LL822;}} else{ goto _LL823;} _LL823: if(*(( int*) _temp797)
== Cyc_Absyn_Throw_e){ _LL933: _temp932=(( struct Cyc_Absyn_Throw_e_struct*)
_temp797)->f1; goto _LL824;} else{ goto _LL825;} _LL825: if(*(( int*) _temp797)
== Cyc_Absyn_NoInstantiate_e){ _LL935: _temp934=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp797)->f1; goto _LL826;} else{ goto _LL827;} _LL827: if(*(( int*) _temp797)
== Cyc_Absyn_Instantiate_e){ _LL939: _temp938=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp797)->f1; goto _LL937; _LL937: _temp936=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp797)->f2; goto _LL828;} else{ goto _LL829;} _LL829: if(*(( int*) _temp797)
== Cyc_Absyn_Cast_e){ _LL943: _temp942=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp797)->f1; _temp944=&(( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp797)->f1);
goto _LL941; _LL941: _temp940=(( struct Cyc_Absyn_Cast_e_struct*) _temp797)->f2;
goto _LL830;} else{ goto _LL831;} _LL831: if(*(( int*) _temp797) == Cyc_Absyn_Address_e){
_LL946: _temp945=(( struct Cyc_Absyn_Address_e_struct*) _temp797)->f1; goto
_LL832;} else{ goto _LL833;} _LL833: if(*(( int*) _temp797) == Cyc_Absyn_New_e){
_LL950: _temp949=(( struct Cyc_Absyn_New_e_struct*) _temp797)->f1; goto _LL948;
_LL948: _temp947=(( struct Cyc_Absyn_New_e_struct*) _temp797)->f2; goto _LL834;}
else{ goto _LL835;} _LL835: if(*(( int*) _temp797) == Cyc_Absyn_Sizeofexp_e){
_LL952: _temp951=(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp797)->f1; goto
_LL836;} else{ goto _LL837;} _LL837: if(*(( int*) _temp797) == Cyc_Absyn_Sizeoftyp_e){
_LL954: _temp953=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp797)->f1;
goto _LL838;} else{ goto _LL839;} _LL839: if(*(( int*) _temp797) == Cyc_Absyn_Offsetof_e){
_LL958: _temp957=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp797)->f1;
goto _LL956; _LL956: _temp955=(( struct Cyc_Absyn_Offsetof_e_struct*) _temp797)->f2;
goto _LL840;} else{ goto _LL841;} _LL841: if(*(( int*) _temp797) == Cyc_Absyn_Deref_e){
_LL960: _temp959=(( struct Cyc_Absyn_Deref_e_struct*) _temp797)->f1; goto _LL842;}
else{ goto _LL843;} _LL843: if(*(( int*) _temp797) == Cyc_Absyn_StructMember_e){
_LL964: _temp963=(( struct Cyc_Absyn_StructMember_e_struct*) _temp797)->f1; goto
_LL962; _LL962: _temp961=(( struct Cyc_Absyn_StructMember_e_struct*) _temp797)->f2;
goto _LL844;} else{ goto _LL845;} _LL845: if(*(( int*) _temp797) == Cyc_Absyn_StructArrow_e){
_LL968: _temp967=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp797)->f1; goto
_LL966; _LL966: _temp965=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp797)->f2;
goto _LL846;} else{ goto _LL847;} _LL847: if(*(( int*) _temp797) == Cyc_Absyn_Subscript_e){
_LL972: _temp971=(( struct Cyc_Absyn_Subscript_e_struct*) _temp797)->f1; goto
_LL970; _LL970: _temp969=(( struct Cyc_Absyn_Subscript_e_struct*) _temp797)->f2;
goto _LL848;} else{ goto _LL849;} _LL849: if(*(( int*) _temp797) == Cyc_Absyn_Tuple_e){
_LL974: _temp973=(( struct Cyc_Absyn_Tuple_e_struct*) _temp797)->f1; goto _LL850;}
else{ goto _LL851;} _LL851: if(*(( int*) _temp797) == Cyc_Absyn_Array_e){ _LL976:
_temp975=(( struct Cyc_Absyn_Array_e_struct*) _temp797)->f1; goto _LL852;} else{
goto _LL853;} _LL853: if(*(( int*) _temp797) == Cyc_Absyn_Comprehension_e){
_LL982: _temp981=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp797)->f1;
goto _LL980; _LL980: _temp979=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp797)->f2; goto _LL978; _LL978: _temp977=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp797)->f3; goto _LL854;} else{ goto _LL855;} _LL855: if(*(( int*) _temp797)
== Cyc_Absyn_Struct_e){ _LL990: _temp989=(( struct Cyc_Absyn_Struct_e_struct*)
_temp797)->f1; goto _LL988; _LL988: _temp987=(( struct Cyc_Absyn_Struct_e_struct*)
_temp797)->f2; goto _LL986; _LL986: _temp985=(( struct Cyc_Absyn_Struct_e_struct*)
_temp797)->f3; goto _LL984; _LL984: _temp983=(( struct Cyc_Absyn_Struct_e_struct*)
_temp797)->f4; goto _LL856;} else{ goto _LL857;} _LL857: if(*(( int*) _temp797)
== Cyc_Absyn_AnonStruct_e){ _LL994: _temp993=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp797)->f1; goto _LL992; _LL992: _temp991=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp797)->f2; goto _LL858;} else{ goto _LL859;} _LL859: if(*(( int*) _temp797)
== Cyc_Absyn_Tunion_e){ _LL1000: _temp999=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp797)->f3; if( _temp999 == 0){ goto _LL998;} else{ goto _LL861;} _LL998:
_temp997=(( struct Cyc_Absyn_Tunion_e_struct*) _temp797)->f4; goto _LL996;
_LL996: _temp995=(( struct Cyc_Absyn_Tunion_e_struct*) _temp797)->f5; goto
_LL860;} else{ goto _LL861;} _LL861: if(*(( int*) _temp797) == Cyc_Absyn_Tunion_e){
_LL1006: _temp1005=(( struct Cyc_Absyn_Tunion_e_struct*) _temp797)->f3; goto
_LL1004; _LL1004: _temp1003=(( struct Cyc_Absyn_Tunion_e_struct*) _temp797)->f4;
goto _LL1002; _LL1002: _temp1001=(( struct Cyc_Absyn_Tunion_e_struct*) _temp797)->f5;
goto _LL862;} else{ goto _LL863;} _LL863: if(*(( int*) _temp797) == Cyc_Absyn_Enum_e){
goto _LL864;} else{ goto _LL865;} _LL865: if(*(( int*) _temp797) == Cyc_Absyn_Malloc_e){
_LL1010: _temp1009=(( struct Cyc_Absyn_Malloc_e_struct*) _temp797)->f1; goto
_LL1008; _LL1008: _temp1007=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp797)->f2; goto _LL866;} else{ goto _LL867;} _LL867: if(*(( int*) _temp797)
== Cyc_Absyn_StmtExp_e){ _LL1012: _temp1011=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp797)->f1; goto _LL868;} else{ goto _LL869;} _LL869: if(*(( int*) _temp797)
== Cyc_Absyn_UnresolvedMem_e){ goto _LL870;} else{ goto _LL871;} _LL871: if(*((
int*) _temp797) == Cyc_Absyn_CompoundLit_e){ goto _LL872;} else{ goto _LL873;}
_LL873: if(*(( int*) _temp797) == Cyc_Absyn_Codegen_e){ _LL1014: _temp1013=((
struct Cyc_Absyn_Codegen_e_struct*) _temp797)->f1; goto _LL874;} else{ goto
_LL875;} _LL875: if(*(( int*) _temp797) == Cyc_Absyn_Fill_e){ goto _LL876;}
else{ goto _LL798;} _LL800: { struct Cyc_Absyn_Exp* _temp1015= Cyc_Absyn_uint_exp(
0, 0); if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){ if( nv->toplevel){( void*)(
e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr( old_typ, _temp1015,
_temp1015))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1016[ 3u]; _temp1016[ 2u]=
_temp1015; _temp1016[ 1u]= _temp1015; _temp1016[ 0u]= _temp1015;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1016, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0))->r));}} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp(
0, 0))->r));} goto _LL798;} _LL802: goto _LL798; _LL804:{ struct _handler_cons
_temp1017; _push_handler(& _temp1017);{ int _temp1019= 0; if( setjmp( _temp1017.handler)){
_temp1019= 1;} if( ! _temp1019){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( nv->varmap,
_temp881))->r));; _pop_handler();} else{ void* _temp1018=( void*) _exn_thrown;
void* _temp1021= _temp1018; _LL1023: if( _temp1021 == Cyc_Dict_Absent){ goto
_LL1024;} else{ goto _LL1025;} _LL1025: goto _LL1026; _LL1024:({ struct Cyc_Stdio_String_pa_struct
_temp1028; _temp1028.tag= Cyc_Stdio_String_pa; _temp1028.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp881);{ void* _temp1027[ 1u]={& _temp1028};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("Can't find %s in exp_to_c, Var\n",
sizeof( unsigned char), 32u), _tag_arr( _temp1027, sizeof( void*), 1u));}});
return; _LL1026:( void) _throw( _temp1021); _LL1022:;}}} goto _LL798; _LL806:({
void* _temp1029[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("unknownid", sizeof( unsigned char), 10u), _tag_arr(
_temp1029, sizeof( void*), 0u));}); goto _LL798; _LL808: { struct Cyc_List_List*
_temp1030=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ, _temp885);(( void(*)(
void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp885);{
void* _temp1031= _temp887; _LL1033: if( _temp1031 ==( void*) Cyc_Absyn_Size){
goto _LL1034;} else{ goto _LL1035;} _LL1035: if( _temp1031 ==( void*) Cyc_Absyn_Plus){
goto _LL1036;} else{ goto _LL1037;} _LL1037: if( _temp1031 ==( void*) Cyc_Absyn_Minus){
goto _LL1038;} else{ goto _LL1039;} _LL1039: if( _temp1031 ==( void*) Cyc_Absyn_Eq){
goto _LL1040;} else{ goto _LL1041;} _LL1041: if( _temp1031 ==( void*) Cyc_Absyn_Neq){
goto _LL1042;} else{ goto _LL1043;} _LL1043: if( _temp1031 ==( void*) Cyc_Absyn_Gt){
goto _LL1044;} else{ goto _LL1045;} _LL1045: if( _temp1031 ==( void*) Cyc_Absyn_Gte){
goto _LL1046;} else{ goto _LL1047;} _LL1047: if( _temp1031 ==( void*) Cyc_Absyn_Lt){
goto _LL1048;} else{ goto _LL1049;} _LL1049: if( _temp1031 ==( void*) Cyc_Absyn_Lte){
goto _LL1050;} else{ goto _LL1051;} _LL1051: goto _LL1052; _LL1034: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp885))->hd;{
void* _temp1053= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( arg->topt))->v); struct Cyc_Absyn_Exp* _temp1061; struct Cyc_Absyn_PtrInfo
_temp1063; struct Cyc_Absyn_Conref* _temp1065; void* _temp1067; _LL1055: if((
unsigned int) _temp1053 > 4u?*(( int*) _temp1053) == Cyc_Absyn_ArrayType: 0){
_LL1062: _temp1061=(( struct Cyc_Absyn_ArrayType_struct*) _temp1053)->f3; goto
_LL1056;} else{ goto _LL1057;} _LL1057: if(( unsigned int) _temp1053 > 4u?*((
int*) _temp1053) == Cyc_Absyn_PointerType: 0){ _LL1064: _temp1063=(( struct Cyc_Absyn_PointerType_struct*)
_temp1053)->f1; _LL1068: _temp1067=( void*) _temp1063.elt_typ; goto _LL1066;
_LL1066: _temp1065= _temp1063.bounds; goto _LL1058;} else{ goto _LL1059;}
_LL1059: goto _LL1060; _LL1056:( void*)( e->r=( void*)(( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1061))->r)); goto _LL1054; _LL1058:{ void* _temp1069= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1065); struct Cyc_Absyn_Exp* _temp1075; _LL1071: if(
_temp1069 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1072;} else{ goto _LL1073;}
_LL1073: if(( unsigned int) _temp1069 > 1u?*(( int*) _temp1069) == Cyc_Absyn_Upper_b:
0){ _LL1076: _temp1075=(( struct Cyc_Absyn_Upper_b_struct*) _temp1069)->f1; goto
_LL1074;} else{ goto _LL1070;} _LL1072:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__get_arr_size_e,({ struct Cyc_Absyn_Exp* _temp1077[ 2u]; _temp1077[ 1u]=
Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( _temp1067), 0); _temp1077[ 0u]=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp885))->hd;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1077, sizeof( struct Cyc_Absyn_Exp*), 2u));}), 0))->r)); goto _LL1070;
_LL1074:( void*)( e->r=( void*)(( void*) _temp1075->r)); goto _LL1070; _LL1070:;}
goto _LL1054; _LL1060:({ struct Cyc_Stdio_String_pa_struct _temp1080; _temp1080.tag=
Cyc_Stdio_String_pa; _temp1080.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v);{ struct Cyc_Stdio_String_pa_struct
_temp1079; _temp1079.tag= Cyc_Stdio_String_pa; _temp1079.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v);{
void* _temp1078[ 2u]={& _temp1079,& _temp1080};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("size primop applied to non-array %s (%s)",
sizeof( unsigned char), 41u), _tag_arr( _temp1078, sizeof( void*), 2u));}}});
return; _LL1054:;} goto _LL1032;} _LL1036: { void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt(( void*)(( struct Cyc_List_List*)
_check_null( _temp1030))->hd,& elt_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp885))->hd; struct Cyc_Absyn_Exp* e2=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp885))->tl))->hd;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_plus_e,({ struct Cyc_Absyn_Exp* _temp1081[ 3u]; _temp1081[ 2u]=
e2; _temp1081[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0);
_temp1081[ 0u]= e1;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1081, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));} goto
_LL1032;} _LL1038: { void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt((
void*)(( struct Cyc_List_List*) _check_null( _temp1030))->hd,& elt_typ)){ struct
Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp885))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp885))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ((
void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1030))->tl))->hd)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0),
Cyc_Toc_curr_sp, 0))->r));( void*)( e->r=( void*)(( void*)( Cyc_Absyn_divide_exp(
Cyc_Absyn_copy_exp( e), Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0),
0))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__tagged_arr_plus_e,({
struct Cyc_Absyn_Exp* _temp1082[ 3u]; _temp1082[ 2u]= Cyc_Absyn_prim1_exp(( void*)
Cyc_Absyn_Minus, e2, 0); _temp1082[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(
elt_typ), 0); _temp1082[ 0u]= e1;(( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list)( _tag_arr( _temp1082, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));}}
goto _LL1032;} _LL1040: goto _LL1042; _LL1042: goto _LL1044; _LL1044: goto
_LL1046; _LL1046: goto _LL1048; _LL1048: goto _LL1050; _LL1050: { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp885))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp885))->tl))->hd; void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp1030))->hd; void* t2=(
void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1030))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){(
void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1032;} _LL1052: goto
_LL1032; _LL1032:;} goto _LL798;} _LL810: { void* e2_cyc_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp891->topt))->v; Cyc_Toc_exp_to_c( nv, _temp891);{
void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){ struct Cyc_Absyn_Exp* _temp1083=( _temp889 ==( void*) Cyc_Absyn_PostInc?
1: _temp889 ==( void*) Cyc_Absyn_PostDec)? Cyc_Toc__tagged_arr_inplace_plus_post_e:
Cyc_Toc__tagged_arr_inplace_plus_e; int _temp1084=( _temp889 ==( void*) Cyc_Absyn_PreInc?
1: _temp889 ==( void*) Cyc_Absyn_PostInc)? 1: - 1;( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_fncall_exp( _temp1083,({ struct Cyc_Absyn_Exp* _temp1085[ 3u];
_temp1085[ 2u]= Cyc_Absyn_signed_int_exp( _temp1084, 0); _temp1085[ 1u]= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( elt_typ), 0); _temp1085[ 0u]= Cyc_Absyn_address_exp( _temp891,
0);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1085, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));} if( elt_typ ==(
void*) Cyc_Absyn_VoidType? ! Cyc_Absyn_is_lvalue( _temp891): 0){ Cyc_Toc_lvalue_assign(
_temp891, 0, Cyc_Toc_incr_lvalue, _temp889);( void*)( e->r=( void*)(( void*)
_temp891->r));} goto _LL798;}} _LL812: { int e1_poly= Cyc_Toc_is_poly_project(
_temp897); void* e1_old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp897->topt))->v; void* e2_old_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp893->topt))->v; Cyc_Toc_exp_to_c( nv, _temp897); Cyc_Toc_exp_to_c(
nv, _temp893);{ int done= 0; if( _temp895 != 0){ void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt( old_typ,& elt_typ)){ struct Cyc_Absyn_Exp*
change;{ void* _temp1086=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp895))->v;
_LL1088: if( _temp1086 ==( void*) Cyc_Absyn_Plus){ goto _LL1089;} else{ goto
_LL1090;} _LL1090: if( _temp1086 ==( void*) Cyc_Absyn_Minus){ goto _LL1091;}
else{ goto _LL1092;} _LL1092: goto _LL1093; _LL1089: change= _temp893; goto
_LL1087; _LL1091: change= Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Minus, _temp893,
0); goto _LL1087; _LL1093:( int) _throw(({ void* _temp1094[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("bad t ? pointer arithmetic", sizeof( unsigned char), 27u), _tag_arr(
_temp1094, sizeof( void*), 0u));})); goto _LL1087; _LL1087:;} done= 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__tagged_arr_inplace_plus_e,({
struct Cyc_Absyn_Exp* _temp1095[ 3u]; _temp1095[ 2u]= change; _temp1095[ 1u]=
Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0); _temp1095[ 0u]= Cyc_Absyn_address_exp(
_temp897, 0);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1095, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));}} if( !
done){ if( e1_poly){( void*)( _temp893->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*) _temp893->r, 0), 0))->r));}
if( ! Cyc_Absyn_is_lvalue( _temp897)){(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
struct _tuple7*), struct _tuple7* f_env)) Cyc_Toc_lvalue_assign)( _temp897, 0,
Cyc_Toc_assignop_lvalue,({ struct _tuple7* _temp1096=( struct _tuple7*)
GC_malloc( sizeof( struct _tuple7) * 1); _temp1096[ 0]=({ struct _tuple7
_temp1097; _temp1097.f1= _temp895; _temp1097.f2= _temp893; _temp1097;});
_temp1096;}));( void*)( e->r=( void*)(( void*) _temp897->r));}} goto _LL798;}}
_LL814: Cyc_Toc_exp_to_c( nv, _temp903); Cyc_Toc_exp_to_c( nv, _temp901); Cyc_Toc_exp_to_c(
nv, _temp899); goto _LL798; _LL816: Cyc_Toc_exp_to_c( nv, _temp907); Cyc_Toc_exp_to_c(
nv, _temp905); goto _LL798; _LL818: _temp917= _temp911; _temp915= _temp909; goto
_LL820; _LL820: Cyc_Toc_exp_to_c( nv, _temp917);(( void(*)( void(* f)( struct
Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp915); goto _LL798; _LL822:{
struct _RegionHandle _temp1098= _new_region(); struct _RegionHandle* r=&
_temp1098; _push_region( r);{ struct _tuple0* argv= Cyc_Toc_temp_var(); struct
Cyc_Absyn_Exp* argvexp= Cyc_Absyn_var_exp( argv, 0); struct Cyc_Absyn_Exp*
num_varargs_exp= Cyc_Absyn_uint_exp(( unsigned int) _temp926, 0); void* cva_type=
Cyc_Toc_typ_to_c(( void*) _temp922->type); void* arr_type=( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1115=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1115[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1116; _temp1116.tag= Cyc_Absyn_ArrayType;
_temp1116.f1=( void*) cva_type; _temp1116.f2= Cyc_Toc_mt_tq; _temp1116.f3=(
struct Cyc_Absyn_Exp*) num_varargs_exp; _temp1116;}); _temp1115;}); int num_args=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp928); int num_normargs=
num_args - _temp926; struct Cyc_List_List* new_args= 0;{ int i= 0; for( 0; i <
num_normargs;( ++ i, _temp928=(( struct Cyc_List_List*) _check_null( _temp928))->tl)){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp928))->hd); new_args=({ struct Cyc_List_List* _temp1099=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1099->hd=(
void*)(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp928))->hd);
_temp1099->tl= new_args; _temp1099;});}} new_args=({ struct Cyc_List_List*
_temp1100=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1100->hd=( void*) Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp*
_temp1101[ 3u]; _temp1101[ 2u]= num_varargs_exp; _temp1101[ 1u]= Cyc_Absyn_sizeoftyp_exp(
cva_type, 0); _temp1101[ 0u]= argvexp;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1101, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0); _temp1100->tl= new_args; _temp1100;}); new_args=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( new_args); Cyc_Toc_exp_to_c( nv,
_temp930);{ struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
_temp930, new_args, 0), 0); if( _temp922->inject){ struct _tagged_arr vs=({
unsigned int _temp1111=( unsigned int) _temp926; struct _tuple0** _temp1112=(
struct _tuple0**) _region_malloc( r, sizeof( struct _tuple0*) * _temp1111);
struct _tagged_arr _temp1114= _tag_arr( _temp1112, sizeof( struct _tuple0*),(
unsigned int) _temp926);{ unsigned int _temp1113= _temp1111; unsigned int i;
for( i= 0; i < _temp1113; i ++){ _temp1112[ i]= Cyc_Toc_temp_var();}}; _temp1114;});
struct Cyc_List_List* _temp1102= 0;{ int i= _temp926 - 1; for( 0; i >= 0; -- i){
_temp1102=({ struct Cyc_List_List* _temp1103=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1103->hd=( void*) Cyc_Toc_make_dle( Cyc_Absyn_address_exp(
Cyc_Absyn_var_exp(*(( struct _tuple0**) _check_unknown_subscript( vs, sizeof(
struct _tuple0*), i)), 0), 0)); _temp1103->tl= _temp1102; _temp1103;});}} s= Cyc_Absyn_declare_stmt(
argv, arr_type,( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp( 0,
_temp1102, 0), s, 0);{ int i= 0; for( 0; _temp928 != 0;((( _temp928=(( struct
Cyc_List_List*) _check_null( _temp928))->tl, _temp924=(( struct Cyc_List_List*)
_check_null( _temp924))->tl)), ++ i)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp928))->hd; void* arg_type=( void*)((
struct Cyc_Core_Opt*) _check_null( arg->topt))->v; struct _tuple0* var=*((
struct _tuple0**) _check_unknown_subscript( vs, sizeof( struct _tuple0*), i));
struct Cyc_Absyn_Exp* varexp= Cyc_Absyn_var_exp( var, 0); struct Cyc_Absyn_Tunionfield
_temp1106; struct Cyc_List_List* _temp1107; struct _tuple0* _temp1109; struct
Cyc_Absyn_Tunionfield* _temp1104=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp924))->hd; _temp1106=* _temp1104; _LL1110: _temp1109=
_temp1106.name; goto _LL1108; _LL1108: _temp1107= _temp1106.typs; goto _LL1105;
_LL1105: { void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1107))->hd)).f2); Cyc_Toc_exp_to_c( nv, arg); if( Cyc_Toc_is_void_star(
field_typ)){ arg= Cyc_Absyn_cast_exp( field_typ, arg, 0);} s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( varexp, Cyc_Toc_fieldname( 1),
0), arg, 0), s, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
varexp, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( _temp1109, 0), 0), s, 0); s= Cyc_Absyn_declare_stmt(
var, Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1109, _tag_arr("_struct",
sizeof( unsigned char), 8u))), 0, s, 0);}}}} else{{ int i= 0; for( 0; _temp928
!= 0;( _temp928=(( struct Cyc_List_List*) _check_null( _temp928))->tl, ++ i)){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp928))->hd); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
argvexp, Cyc_Absyn_uint_exp(( unsigned int) i, 0), 0),( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp928))->hd, 0), s, 0);}} s= Cyc_Absyn_declare_stmt(
argv, arr_type, 0, s, 0);}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));}}; _pop_region( r);} goto _LL798; _LL824: Cyc_Toc_exp_to_c( nv,
_temp932);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(
old_typ), Cyc_Toc_newthrow_exp( _temp932), 0))->r)); goto _LL798; _LL826: Cyc_Toc_exp_to_c(
nv, _temp934); goto _LL798; _LL828: Cyc_Toc_exp_to_c( nv, _temp938); for( 0;
_temp936 != 0; _temp936=(( struct Cyc_List_List*) _check_null( _temp936))->tl){
void* k= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*) _check_null(
_temp936))->hd); if( k !=( void*) Cyc_Absyn_EffKind? k !=( void*) Cyc_Absyn_RgnKind:
0){{ void* _temp1117= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*)
_check_null( _temp936))->hd); _LL1119: if(( unsigned int) _temp1117 > 4u?*(( int*)
_temp1117) == Cyc_Absyn_VarType: 0){ goto _LL1120;} else{ goto _LL1121;} _LL1121:
if(( unsigned int) _temp1117 > 4u?*(( int*) _temp1117) == Cyc_Absyn_TunionType:
0){ goto _LL1122;} else{ goto _LL1123;} _LL1123: goto _LL1124; _LL1120:
continue; _LL1122: continue; _LL1124:( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp938, 0))->r)); goto _LL1118; _LL1118:;} break;}} goto _LL798; _LL830: {
void* old_t2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp940->topt))->v;
void* new_typ=* _temp944;* _temp944= Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c(
nv, _temp940);{ struct _tuple8 _temp1126=({ struct _tuple8 _temp1125; _temp1125.f1=
Cyc_Tcutil_compress( old_t2); _temp1125.f2= Cyc_Tcutil_compress( new_typ);
_temp1125;}); void* _temp1134; struct Cyc_Absyn_PtrInfo _temp1136; void*
_temp1138; struct Cyc_Absyn_PtrInfo _temp1140; void* _temp1142; void* _temp1144;
struct Cyc_Absyn_PtrInfo _temp1146; _LL1128: _LL1139: _temp1138= _temp1126.f1;
if(( unsigned int) _temp1138 > 4u?*(( int*) _temp1138) == Cyc_Absyn_PointerType:
0){ _LL1141: _temp1140=(( struct Cyc_Absyn_PointerType_struct*) _temp1138)->f1;
goto _LL1135;} else{ goto _LL1130;} _LL1135: _temp1134= _temp1126.f2; if((
unsigned int) _temp1134 > 4u?*(( int*) _temp1134) == Cyc_Absyn_PointerType: 0){
_LL1137: _temp1136=(( struct Cyc_Absyn_PointerType_struct*) _temp1134)->f1; goto
_LL1129;} else{ goto _LL1130;} _LL1130: _LL1145: _temp1144= _temp1126.f1; if((
unsigned int) _temp1144 > 4u?*(( int*) _temp1144) == Cyc_Absyn_PointerType: 0){
_LL1147: _temp1146=(( struct Cyc_Absyn_PointerType_struct*) _temp1144)->f1; goto
_LL1143;} else{ goto _LL1132;} _LL1143: _temp1142= _temp1126.f2; if((
unsigned int) _temp1142 > 4u?*(( int*) _temp1142) == Cyc_Absyn_IntType: 0){ goto
_LL1131;} else{ goto _LL1132;} _LL1132: goto _LL1133; _LL1129: { int _temp1148=((
int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0, _temp1140.nullable);
int _temp1149=(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
0, _temp1136.nullable); void* _temp1150= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1140.bounds); void* _temp1151= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1136.bounds);{ struct _tuple8 _temp1153=({ struct _tuple8 _temp1152;
_temp1152.f1= _temp1150; _temp1152.f2= _temp1151; _temp1152;}); void* _temp1163;
void* _temp1165; void* _temp1167; void* _temp1169; struct Cyc_Absyn_Exp*
_temp1171; void* _temp1173; struct Cyc_Absyn_Exp* _temp1175; void* _temp1177;
_LL1155: _LL1166: _temp1165= _temp1153.f1; if(( unsigned int) _temp1165 > 1u?*((
int*) _temp1165) == Cyc_Absyn_Upper_b: 0){ goto _LL1164;} else{ goto _LL1157;}
_LL1164: _temp1163= _temp1153.f2; if(( unsigned int) _temp1163 > 1u?*(( int*)
_temp1163) == Cyc_Absyn_Upper_b: 0){ goto _LL1156;} else{ goto _LL1157;} _LL1157:
_LL1170: _temp1169= _temp1153.f1; if(( unsigned int) _temp1169 > 1u?*(( int*)
_temp1169) == Cyc_Absyn_Upper_b: 0){ _LL1172: _temp1171=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1169)->f1; goto _LL1168;} else{ goto _LL1159;} _LL1168: _temp1167=
_temp1153.f2; if( _temp1167 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1158;}
else{ goto _LL1159;} _LL1159: _LL1178: _temp1177= _temp1153.f1; if( _temp1177 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL1174;} else{ goto _LL1161;} _LL1174:
_temp1173= _temp1153.f2; if(( unsigned int) _temp1173 > 1u?*(( int*) _temp1173)
== Cyc_Absyn_Upper_b: 0){ _LL1176: _temp1175=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1173)->f1; goto _LL1160;} else{ goto _LL1161;} _LL1161: goto _LL1162;
_LL1156: if( _temp1148? ! _temp1149: 0){ if( nv->toplevel){({ void* _temp1179[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("can't do null-check conversion at top-level",
sizeof( unsigned char), 44u), _tag_arr( _temp1179, sizeof( void*), 0u));});}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(* _temp944, Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1180=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1180->hd=( void*) _temp940;
_temp1180->tl= 0; _temp1180;}), 0), 0))->r));} goto _LL1154; _LL1158: if( nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr( old_t2,
_temp1171, _temp940))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1181[ 3u]; _temp1181[ 2u]=
_temp1171; _temp1181[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(( void*)
_temp1136.elt_typ), 0); _temp1181[ 0u]= _temp940;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1181, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0))->r));} goto _LL1154; _LL1160: if( nv->toplevel){({ void* _temp1182[
0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("can't coerce t? to t* or t@ at the top-level", sizeof( unsigned char),
45u), _tag_arr( _temp1182, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Exp*
_temp1183= Cyc_Absyn_fncall_exp( Cyc_Toc__untag_arr_e,({ struct Cyc_Absyn_Exp*
_temp1185[ 3u]; _temp1185[ 2u]= _temp1175; _temp1185[ 1u]= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(( void*) _temp1140.elt_typ), 0); _temp1185[ 0u]= _temp940;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1185, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0); if( _temp1149){( void*)(
_temp1183->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({
struct Cyc_List_List* _temp1184=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1184->hd=( void*) Cyc_Absyn_copy_exp( _temp1183);
_temp1184->tl= 0; _temp1184;}), 0))->r));}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(*
_temp944, _temp1183, 0))->r)); goto _LL1154;} _LL1162: goto _LL1154; _LL1154:;}
goto _LL1127;} _LL1131:{ void* _temp1186= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1146.bounds); _LL1188: if( _temp1186 ==( void*) Cyc_Absyn_Unknown_b){ goto
_LL1189;} else{ goto _LL1190;} _LL1190: goto _LL1191; _LL1189:( void*)( _temp940->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) _temp940->r,
_temp940->loc), Cyc_Toc_curr_sp, 0))->r)); goto _LL1187; _LL1191: goto _LL1187;
_LL1187:;} goto _LL1127; _LL1133: goto _LL1127; _LL1127:;} goto _LL798;} _LL832:{
void* _temp1192=( void*) _temp945->r; struct Cyc_List_List* _temp1200; struct
_tuple0* _temp1202; struct Cyc_List_List* _temp1204; _LL1194: if(*(( int*)
_temp1192) == Cyc_Absyn_Struct_e){ _LL1203: _temp1202=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1192)->f1; goto _LL1201; _LL1201: _temp1200=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1192)->f3; goto _LL1195;} else{ goto _LL1196;} _LL1196: if(*(( int*)
_temp1192) == Cyc_Absyn_Tuple_e){ _LL1205: _temp1204=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1192)->f1; goto _LL1197;} else{ goto _LL1198;} _LL1198: goto _LL1199;
_LL1195: if( nv->toplevel){({ struct Cyc_Stdio_String_pa_struct _temp1207;
_temp1207.tag= Cyc_Stdio_String_pa; _temp1207.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp945->loc);{ void* _temp1206[ 1u]={& _temp1207};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("%s: & on non-identifiers at the top-level",
sizeof( unsigned char), 42u), _tag_arr( _temp1206, sizeof( void*), 1u));}});}(
void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp945->topt))->v, 1, 0, _temp1200, _temp1202))->r)); goto
_LL1193; _LL1197: if( nv->toplevel){({ struct Cyc_Stdio_String_pa_struct
_temp1209; _temp1209.tag= Cyc_Stdio_String_pa; _temp1209.f1=( struct _tagged_arr)
Cyc_Position_string_of_segment( _temp945->loc);{ void* _temp1208[ 1u]={&
_temp1209};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("%s: & on non-identifiers at the top-level", sizeof( unsigned char), 42u),
_tag_arr( _temp1208, sizeof( void*), 1u));}});}( void*)( e->r=( void*)(( void*)(
Cyc_Toc_init_tuple( nv, 1, 0, _temp1204))->r)); goto _LL1193; _LL1199: Cyc_Toc_exp_to_c(
nv, _temp945); if( ! Cyc_Absyn_is_lvalue( _temp945)){(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
int), int f_env)) Cyc_Toc_lvalue_assign)( _temp945, 0, Cyc_Toc_address_lvalue, 1);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v), _temp945, 0))->r));} goto
_LL1193; _LL1193:;} goto _LL798; _LL834: if( nv->toplevel){({ struct Cyc_Stdio_String_pa_struct
_temp1211; _temp1211.tag= Cyc_Stdio_String_pa; _temp1211.f1=( struct _tagged_arr)
Cyc_Position_string_of_segment( _temp947->loc);{ void* _temp1210[ 1u]={&
_temp1211};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("%s: new at top-level", sizeof( unsigned char), 21u), _tag_arr(
_temp1210, sizeof( void*), 1u));}});}{ void* _temp1212=( void*) _temp947->r;
struct Cyc_List_List* _temp1224; struct Cyc_Absyn_Exp* _temp1226; struct Cyc_Absyn_Exp*
_temp1228; struct Cyc_Absyn_Vardecl* _temp1230; struct Cyc_List_List* _temp1232;
struct _tuple0* _temp1234; struct Cyc_List_List* _temp1236; _LL1214: if(*(( int*)
_temp1212) == Cyc_Absyn_Array_e){ _LL1225: _temp1224=(( struct Cyc_Absyn_Array_e_struct*)
_temp1212)->f1; goto _LL1215;} else{ goto _LL1216;} _LL1216: if(*(( int*)
_temp1212) == Cyc_Absyn_Comprehension_e){ _LL1231: _temp1230=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1212)->f1; goto _LL1229; _LL1229: _temp1228=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1212)->f2; goto _LL1227; _LL1227: _temp1226=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1212)->f3; goto _LL1217;} else{ goto _LL1218;} _LL1218: if(*(( int*)
_temp1212) == Cyc_Absyn_Struct_e){ _LL1235: _temp1234=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1212)->f1; goto _LL1233; _LL1233: _temp1232=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1212)->f3; goto _LL1219;} else{ goto _LL1220;} _LL1220: if(*(( int*)
_temp1212) == Cyc_Absyn_Tuple_e){ _LL1237: _temp1236=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1212)->f1; goto _LL1221;} else{ goto _LL1222;} _LL1222: goto _LL1223;
_LL1215: { struct _tuple0* _temp1238= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp1239= Cyc_Absyn_var_exp( _temp1238, 0); struct Cyc_Absyn_Stmt* _temp1240=
Cyc_Toc_init_array( nv, _temp1239, _temp1224, Cyc_Absyn_exp_stmt( _temp1239, 0));
void* old_elt_typ;{ void* _temp1241= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1247; struct Cyc_Absyn_Tqual _temp1249; void* _temp1251; _LL1243: if((
unsigned int) _temp1241 > 4u?*(( int*) _temp1241) == Cyc_Absyn_PointerType: 0){
_LL1248: _temp1247=(( struct Cyc_Absyn_PointerType_struct*) _temp1241)->f1;
_LL1252: _temp1251=( void*) _temp1247.elt_typ; goto _LL1250; _LL1250: _temp1249=
_temp1247.tq; goto _LL1244;} else{ goto _LL1245;} _LL1245: goto _LL1246; _LL1244:
old_elt_typ= _temp1251; goto _LL1242; _LL1246: old_elt_typ=({ void* _temp1253[ 0u]={};
Cyc_Toc_toc_impos( _tag_arr("exp_to_c:new array expression doesn't have ptr type",
sizeof( unsigned char), 52u), _tag_arr( _temp1253, sizeof( void*), 0u));}); goto
_LL1242; _LL1242:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void*
_temp1254= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); struct Cyc_Absyn_Exp*
_temp1255= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_signed_int_exp(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1224), 0), 0); struct
Cyc_Absyn_Exp* e1; if( _temp949 == 0){ e1= Cyc_Toc_malloc_exp( old_elt_typ,
_temp1255);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null(
_temp949); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r, _temp1255);}{
struct Cyc_Absyn_Exp* _temp1256= Cyc_Absyn_cast_exp( _temp1254, e1, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1238,
_temp1254,( struct Cyc_Absyn_Exp*) _temp1256, _temp1240, 0), 0))->r)); goto
_LL1213;}}} _LL1217: { int is_tagged_ptr= 0;{ void* _temp1257= Cyc_Tcutil_compress(
old_typ); struct Cyc_Absyn_PtrInfo _temp1263; struct Cyc_Absyn_Conref* _temp1265;
struct Cyc_Absyn_Tqual _temp1267; void* _temp1269; _LL1259: if(( unsigned int)
_temp1257 > 4u?*(( int*) _temp1257) == Cyc_Absyn_PointerType: 0){ _LL1264:
_temp1263=(( struct Cyc_Absyn_PointerType_struct*) _temp1257)->f1; _LL1270:
_temp1269=( void*) _temp1263.elt_typ; goto _LL1268; _LL1268: _temp1267=
_temp1263.tq; goto _LL1266; _LL1266: _temp1265= _temp1263.bounds; goto _LL1260;}
else{ goto _LL1261;} _LL1261: goto _LL1262; _LL1260: is_tagged_ptr= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1265) ==( void*) Cyc_Absyn_Unknown_b; goto _LL1258;
_LL1262:({ void* _temp1271[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: comprehension not an array type",
sizeof( unsigned char), 42u), _tag_arr( _temp1271, sizeof( void*), 0u));}); goto
_LL1258; _LL1258:;}{ struct _tuple0* max= Cyc_Toc_temp_var(); struct _tuple0* a=
Cyc_Toc_temp_var(); void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1226->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ);
void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c(
nv, _temp1228);{ struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp(
a, 0), _temp1230, Cyc_Absyn_var_exp( max, 0), _temp1226, Cyc_Absyn_skip_stmt( 0),
1);{ struct _RegionHandle _temp1272= _new_region(); struct _RegionHandle* r=&
_temp1272; _push_region( r);{ struct Cyc_List_List* decls=({ struct Cyc_List_List*
_temp1290=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp1290->hd=( void*)({ struct _tuple9* _temp1291=( struct _tuple9*)
_region_malloc( r, sizeof( struct _tuple9)); _temp1291->f1= max; _temp1291->f2=
Cyc_Absyn_uint_t; _temp1291->f3=( struct Cyc_Absyn_Exp*) _temp1228; _temp1291;});
_temp1290->tl= 0; _temp1290;}); struct Cyc_Absyn_Exp* ai; if( _temp949 == 0){ ai=
Cyc_Toc_malloc_exp( old_elt_typ, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0));} else{ struct Cyc_Absyn_Exp* r=(
struct Cyc_Absyn_Exp*) _check_null( _temp949); Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp(
r, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));}{ struct Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp( ptr_typ, ai, 0);
decls=({ struct Cyc_List_List* _temp1273=( struct Cyc_List_List*) _region_malloc(
r, sizeof( struct Cyc_List_List)); _temp1273->hd=( void*)({ struct _tuple9*
_temp1274=( struct _tuple9*) _region_malloc( r, sizeof( struct _tuple9));
_temp1274->f1= a; _temp1274->f2= ptr_typ; _temp1274->f3=( struct Cyc_Absyn_Exp*)
ainit; _temp1274;}); _temp1273->tl= decls; _temp1273;}); if( is_tagged_ptr){
struct _tuple0* _temp1275= Cyc_Toc_temp_var(); void* _temp1276= Cyc_Toc_typ_to_c(
old_typ); struct Cyc_Absyn_Exp* _temp1277= Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp* _temp1280[ 3u]; _temp1280[ 2u]= _temp1228; _temp1280[ 1u]=
Cyc_Absyn_sizeoftyp_exp( elt_typ, 0); _temp1280[ 0u]= Cyc_Absyn_var_exp( a, 0);((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1280, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0); decls=({ struct Cyc_List_List*
_temp1278=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp1278->hd=( void*)({ struct _tuple9* _temp1279=( struct _tuple9*)
_region_malloc( r, sizeof( struct _tuple9)); _temp1279->f1= _temp1275; _temp1279->f2=
_temp1276; _temp1279->f3=( struct Cyc_Absyn_Exp*) _temp1277; _temp1279;});
_temp1278->tl= decls; _temp1278;}); s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( _temp1275, 0), 0), 0);} else{ s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( a, 0), 0), 0);}{ struct Cyc_List_List* _temp1281= decls; for(
0; _temp1281 != 0; _temp1281=(( struct Cyc_List_List*) _check_null( _temp1281))->tl){
struct Cyc_Absyn_Exp* _temp1284; void* _temp1286; struct _tuple0* _temp1288;
struct _tuple9 _temp1282=*(( struct _tuple9*)(( struct Cyc_List_List*)
_check_null( _temp1281))->hd); _LL1289: _temp1288= _temp1282.f1; goto _LL1287;
_LL1287: _temp1286= _temp1282.f2; goto _LL1285; _LL1285: _temp1284= _temp1282.f3;
goto _LL1283; _LL1283: s= Cyc_Absyn_declare_stmt( _temp1288, _temp1286,
_temp1284, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}};
_pop_region( r);} goto _LL1213;}}} _LL1219:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp947->topt))->v, 1,
_temp949, _temp1232, _temp1234))->r)); goto _LL1213; _LL1221:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, _temp949, _temp1236))->r)); goto
_LL1213; _LL1223: { void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp947->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ);
struct Cyc_Absyn_Exp* mexp= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0); if( _temp949
== 0){ mexp= Cyc_Toc_malloc_exp( old_elt_typ, mexp);} else{ struct Cyc_Absyn_Exp*
r=( struct Cyc_Absyn_Exp*) _check_null( _temp949); Cyc_Toc_exp_to_c( nv, r);
mexp= Cyc_Toc_rmalloc_exp( r, mexp);} mexp= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq), mexp, 0);{ int done= 0;{ void* _temp1292=( void*)
_temp947->r; struct Cyc_Absyn_Exp* _temp1298; void* _temp1300; _LL1294: if(*((
int*) _temp1292) == Cyc_Absyn_Cast_e){ _LL1301: _temp1300=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1292)->f1; goto _LL1299; _LL1299: _temp1298=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1292)->f2; goto _LL1295;} else{ goto _LL1296;} _LL1296: goto _LL1297;
_LL1295:{ struct _tuple8 _temp1303=({ struct _tuple8 _temp1302; _temp1302.f1=
Cyc_Tcutil_compress( _temp1300); _temp1302.f2= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1298->topt))->v); _temp1302;}); void*
_temp1309; struct Cyc_Absyn_PtrInfo _temp1311; struct Cyc_Absyn_Conref*
_temp1313; void* _temp1315; struct Cyc_Absyn_PtrInfo _temp1317; struct Cyc_Absyn_Conref*
_temp1319; void* _temp1321; _LL1305: _LL1316: _temp1315= _temp1303.f1; if((
unsigned int) _temp1315 > 4u?*(( int*) _temp1315) == Cyc_Absyn_PointerType: 0){
_LL1318: _temp1317=(( struct Cyc_Absyn_PointerType_struct*) _temp1315)->f1;
_LL1322: _temp1321=( void*) _temp1317.elt_typ; goto _LL1320; _LL1320: _temp1319=
_temp1317.bounds; goto _LL1310;} else{ goto _LL1307;} _LL1310: _temp1309=
_temp1303.f2; if(( unsigned int) _temp1309 > 4u?*(( int*) _temp1309) == Cyc_Absyn_PointerType:
0){ _LL1312: _temp1311=(( struct Cyc_Absyn_PointerType_struct*) _temp1309)->f1;
_LL1314: _temp1313= _temp1311.bounds; goto _LL1306;} else{ goto _LL1307;}
_LL1307: goto _LL1308; _LL1306:{ struct _tuple8 _temp1324=({ struct _tuple8
_temp1323; _temp1323.f1= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1319);
_temp1323.f2= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1313); _temp1323;});
void* _temp1330; struct Cyc_Absyn_Exp* _temp1332; void* _temp1334; _LL1326:
_LL1335: _temp1334= _temp1324.f1; if( _temp1334 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1331;} else{ goto _LL1328;} _LL1331: _temp1330= _temp1324.f2; if((
unsigned int) _temp1330 > 1u?*(( int*) _temp1330) == Cyc_Absyn_Upper_b: 0){
_LL1333: _temp1332=(( struct Cyc_Absyn_Upper_b_struct*) _temp1330)->f1; goto
_LL1327;} else{ goto _LL1328;} _LL1328: goto _LL1329; _LL1327: done= 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__init_tag_arr_e,({ struct
Cyc_Absyn_Exp* _temp1336[ 4u]; _temp1336[ 3u]= _temp1332; _temp1336[ 2u]= Cyc_Absyn_sizeoftyp_exp(
_temp1321, 0); _temp1336[ 1u]= _temp1298; _temp1336[ 0u]= mexp;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1336, sizeof( struct Cyc_Absyn_Exp*),
4u));}), 0))->r)); goto _LL1325; _LL1329: goto _LL1325; _LL1325:;} goto _LL1304;
_LL1308: goto _LL1304; _LL1304:;} goto _LL1293; _LL1297: goto _LL1293; _LL1293:;}
if( ! done){ struct _tuple0* _temp1337= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt*
_temp1338= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1337, 0), 0); struct Cyc_Absyn_Exp*
_temp1339= Cyc_Absyn_signed_int_exp( 0, 0); Cyc_Toc_exp_to_c( nv, _temp947);
_temp1338= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( _temp1337, 0), _temp1339, 0), _temp947, 0), _temp1338, 0);{
void* _temp1340= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1337,
_temp1340,( struct Cyc_Absyn_Exp*) mexp, _temp1338, 0), 0))->r));}} goto _LL1213;}}
_LL1213:;} goto _LL798; _LL836: Cyc_Toc_exp_to_c( nv, _temp951); goto _LL798;
_LL838:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1341=( struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1341[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1342; _temp1342.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1342.f1=( void*) Cyc_Toc_typ_to_c_array( _temp953);
_temp1342;}); _temp1341;}))); goto _LL798; _LL840:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Offsetof_e_struct* _temp1343=( struct Cyc_Absyn_Offsetof_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp1343[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp1344; _temp1344.tag= Cyc_Absyn_Offsetof_e;
_temp1344.f1=( void*) Cyc_Toc_typ_to_c_array( _temp957); _temp1344.f2= _temp955;
_temp1344;}); _temp1343;}))); goto _LL798; _LL842: { void* _temp1345= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp959->topt))->v);{ void*
_temp1346= _temp1345; struct Cyc_Absyn_PtrInfo _temp1352; struct Cyc_Absyn_Conref*
_temp1354; struct Cyc_Absyn_Tqual _temp1356; struct Cyc_Absyn_Conref* _temp1358;
void* _temp1360; void* _temp1362; _LL1348: if(( unsigned int) _temp1346 > 4u?*((
int*) _temp1346) == Cyc_Absyn_PointerType: 0){ _LL1353: _temp1352=(( struct Cyc_Absyn_PointerType_struct*)
_temp1346)->f1; _LL1363: _temp1362=( void*) _temp1352.elt_typ; goto _LL1361;
_LL1361: _temp1360=( void*) _temp1352.rgn_typ; goto _LL1359; _LL1359: _temp1358=
_temp1352.nullable; goto _LL1357; _LL1357: _temp1356= _temp1352.tq; goto _LL1355;
_LL1355: _temp1354= _temp1352.bounds; goto _LL1349;} else{ goto _LL1350;}
_LL1350: goto _LL1351; _LL1349:{ void* _temp1364= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1354); _LL1366: if(( unsigned int) _temp1364 > 1u?*(( int*) _temp1364) ==
Cyc_Absyn_Upper_b: 0){ goto _LL1367;} else{ goto _LL1368;} _LL1368: if(
_temp1364 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1369;} else{ goto _LL1365;}
_LL1367: Cyc_Toc_exp_to_c( nv, _temp959); if( Cyc_Toc_is_nullable( _temp1345)){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp959->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1370=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1370->hd=( void*) _temp959;
_temp1370->tl= 0; _temp1370;}), 0), 0), 0))->r));} goto _LL1365; _LL1369: {
struct Cyc_Absyn_Exp* _temp1371= Cyc_Absyn_uint_exp( 0, 0); _temp1371->topt=({
struct Cyc_Core_Opt* _temp1372=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1372->v=( void*) Cyc_Absyn_uint_t; _temp1372;});( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_subscript_exp( _temp959, _temp1371, 0))->r)); Cyc_Toc_exp_to_c(
nv, e); goto _LL1365;} _LL1365:;} goto _LL1347; _LL1351:({ void* _temp1373[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: Deref: non-pointer", sizeof( unsigned char), 29u), _tag_arr(
_temp1373, sizeof( void*), 0u));}); goto _LL1347; _LL1347:;} goto _LL798;}
_LL844: Cyc_Toc_exp_to_c( nv, _temp963); if( Cyc_Toc_is_poly_project( e)){( void*)(
e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r,
0), 0))->r));} goto _LL798; _LL846: { void* e1typ= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp967->topt))->v); Cyc_Toc_exp_to_c( nv,
_temp967);{ int is_poly= Cyc_Toc_is_poly_project( e); struct Cyc_Absyn_Conref*
_temp1376; struct Cyc_Absyn_Tqual _temp1378; struct Cyc_Absyn_Conref* _temp1380;
void* _temp1382; void* _temp1384; struct Cyc_Absyn_PtrInfo _temp1374= Cyc_Toc_get_ptr_type(
e1typ); _LL1385: _temp1384=( void*) _temp1374.elt_typ; goto _LL1383; _LL1383:
_temp1382=( void*) _temp1374.rgn_typ; goto _LL1381; _LL1381: _temp1380=
_temp1374.nullable; goto _LL1379; _LL1379: _temp1378= _temp1374.tq; goto _LL1377;
_LL1377: _temp1376= _temp1374.bounds; goto _LL1375; _LL1375:{ void* _temp1386=
Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1376); struct Cyc_Absyn_Exp*
_temp1392; _LL1388: if(( unsigned int) _temp1386 > 1u?*(( int*) _temp1386) ==
Cyc_Absyn_Upper_b: 0){ _LL1393: _temp1392=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1386)->f1; goto _LL1389;} else{ goto _LL1390;} _LL1390: if( _temp1386 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL1391;} else{ goto _LL1387;} _LL1389: if(
Cyc_Evexp_eval_const_uint_exp( _temp1392) < 1){({ void* _temp1394[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("exp_to_c:  StructArrow_e on pointer of size 0",
sizeof( unsigned char), 46u), _tag_arr( _temp1394, sizeof( void*), 0u));});}{
int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
0, _temp1380); if( Cyc_Toc_is_nullable(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp967->topt))->v)){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structarrow_exp(
Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp967->topt))->v), Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({
struct Cyc_List_List* _temp1395=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1395->hd=( void*) _temp967; _temp1395->tl= 0;
_temp1395;}), 0), 0), _temp965, 0))->r));} goto _LL1387;} _LL1391: { void* ta1=
Cyc_Toc_typ_to_c_array( _temp1384);( void*)( _temp967->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( ta1, _temp1378), Cyc_Absyn_fncall_exp( Cyc_Toc__check_unknown_subscript_e,({
struct Cyc_Absyn_Exp* _temp1396[ 3u]; _temp1396[ 2u]= Cyc_Absyn_uint_exp( 0, 0);
_temp1396[ 1u]= Cyc_Absyn_sizeoftyp_exp( ta1, 0); _temp1396[ 0u]= Cyc_Absyn_copy_exp(
_temp967);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1396, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0), 0))->r)); goto
_LL1387;} _LL1387:;} if( is_poly){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));} goto _LL798;}} _LL848: { void*
_temp1397= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp971->topt))->v); Cyc_Toc_exp_to_c( nv, _temp971); Cyc_Toc_exp_to_c( nv,
_temp969);{ void* _temp1398= _temp1397; struct Cyc_List_List* _temp1406; struct
Cyc_Absyn_PtrInfo _temp1408; struct Cyc_Absyn_Conref* _temp1410; struct Cyc_Absyn_Tqual
_temp1412; struct Cyc_Absyn_Conref* _temp1414; void* _temp1416; void* _temp1418;
_LL1400: if(( unsigned int) _temp1398 > 4u?*(( int*) _temp1398) == Cyc_Absyn_TupleType:
0){ _LL1407: _temp1406=(( struct Cyc_Absyn_TupleType_struct*) _temp1398)->f1;
goto _LL1401;} else{ goto _LL1402;} _LL1402: if(( unsigned int) _temp1398 > 4u?*((
int*) _temp1398) == Cyc_Absyn_PointerType: 0){ _LL1409: _temp1408=(( struct Cyc_Absyn_PointerType_struct*)
_temp1398)->f1; _LL1419: _temp1418=( void*) _temp1408.elt_typ; goto _LL1417;
_LL1417: _temp1416=( void*) _temp1408.rgn_typ; goto _LL1415; _LL1415: _temp1414=
_temp1408.nullable; goto _LL1413; _LL1413: _temp1412= _temp1408.tq; goto _LL1411;
_LL1411: _temp1410= _temp1408.bounds; goto _LL1403;} else{ goto _LL1404;}
_LL1404: goto _LL1405; _LL1401: { unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp969) + 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
_temp971, Cyc_Toc_fieldname(( int) i), 0))->r)); goto _LL1399;} _LL1403:{ void*
_temp1420= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1410); struct Cyc_Absyn_Exp*
_temp1426; _LL1422: if(( unsigned int) _temp1420 > 1u?*(( int*) _temp1420) ==
Cyc_Absyn_Upper_b: 0){ _LL1427: _temp1426=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1420)->f1; goto _LL1423;} else{ goto _LL1424;} _LL1424: if( _temp1420 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL1425;} else{ goto _LL1421;} _LL1423:
_temp1426= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp( _temp1426), 0);{
int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
0, _temp1414); void* ta1= Cyc_Toc_typ_to_c( _temp1418); void* ta2= Cyc_Absyn_cstar_typ(
ta1, _temp1412); if( possibly_null){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp(
Cyc_Absyn_cast_exp( ta2, Cyc_Absyn_fncall_exp( Cyc_Toc__check_known_subscript_null_e,({
struct Cyc_Absyn_Exp* _temp1428[ 4u]; _temp1428[ 3u]= _temp969; _temp1428[ 2u]=
Cyc_Absyn_sizeoftyp_exp( ta1, 0); _temp1428[ 1u]= _temp1426; _temp1428[ 0u]=
_temp971;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1428, sizeof( struct Cyc_Absyn_Exp*), 4u));}), 0), 0), 0))->r));}
else{( void*)( _temp969->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__check_known_subscript_notnull_e,({
struct Cyc_Absyn_Exp* _temp1429[ 2u]; _temp1429[ 1u]= Cyc_Absyn_copy_exp(
_temp969); _temp1429[ 0u]= _temp1426;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1429, sizeof( struct Cyc_Absyn_Exp*),
2u));}), 0))->r));} goto _LL1421;} _LL1425: { void* ta1= Cyc_Toc_typ_to_c_array(
_temp1418);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( ta1, _temp1412), Cyc_Absyn_fncall_exp( Cyc_Toc__check_unknown_subscript_e,({
struct Cyc_Absyn_Exp* _temp1430[ 3u]; _temp1430[ 2u]= _temp969; _temp1430[ 1u]=
Cyc_Absyn_sizeoftyp_exp( ta1, 0); _temp1430[ 0u]= _temp971;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1430, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0), 0), 0))->r)); goto _LL1421;} _LL1421:;} goto _LL1399; _LL1405:({
void* _temp1431[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: Subscript on non-tuple/array/tuple ptr",
sizeof( unsigned char), 49u), _tag_arr( _temp1431, sizeof( void*), 0u));}); goto
_LL1399; _LL1399:;} goto _LL798;} _LL850: if( ! nv->toplevel){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 0, 0, _temp973))->r));} else{ struct
Cyc_List_List* _temp1432=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c,
_temp973); struct _tagged_arr* _temp1433= Cyc_Toc_add_tuple_type( _temp1432);
struct Cyc_List_List* dles= 0;{ int i= 1; for( 0; _temp973 != 0;( _temp973=((
struct Cyc_List_List*) _check_null( _temp973))->tl, i ++)){ Cyc_Toc_exp_to_c( nv,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp973))->hd);
dles=({ struct Cyc_List_List* _temp1434=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1434->hd=( void*)({ struct _tuple4*
_temp1435=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1435->f1=
0; _temp1435->f2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp973))->hd; _temp1435;}); _temp1434->tl= dles; _temp1434;});}} dles=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, dles, 0))->r));}
goto _LL798; _LL852:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp975, 0))->r));{ struct Cyc_List_List* _temp1436= _temp975; for( 0;
_temp1436 != 0; _temp1436=(( struct Cyc_List_List*) _check_null( _temp1436))->tl){
struct _tuple4 _temp1439; struct Cyc_Absyn_Exp* _temp1440; struct _tuple4*
_temp1437=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1436))->hd;
_temp1439=* _temp1437; _LL1441: _temp1440= _temp1439.f2; goto _LL1438; _LL1438:
Cyc_Toc_exp_to_c( nv, _temp1440);}} goto _LL798; _LL854: { unsigned int
_temp1442= Cyc_Evexp_eval_const_uint_exp( _temp979); Cyc_Toc_exp_to_c( nv,
_temp977);{ struct Cyc_List_List* es= 0; if( ! Cyc_Toc_is_zero( _temp977)){
unsigned int i= 0; for( 0; i < _temp1442; i ++){ es=({ struct Cyc_List_List*
_temp1443=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1443->hd=( void*)({ struct _tuple4* _temp1444=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1444->f1= 0; _temp1444->f2= _temp977; _temp1444;});
_temp1443->tl= es; _temp1443;});}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, es, 0))->r)); goto _LL798;}} _LL856: if( ! nv->toplevel){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp985, _temp989))->r));}
else{ if( _temp983 == 0){({ void* _temp1445[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("Struct_e: missing structdecl pointer",
sizeof( unsigned char), 37u), _tag_arr( _temp1445, sizeof( void*), 0u));});}{
struct Cyc_Absyn_Structdecl* sd2=( struct Cyc_Absyn_Structdecl*) _check_null(
_temp983); struct _RegionHandle _temp1446= _new_region(); struct _RegionHandle*
rgn=& _temp1446; _push_region( rgn);{ struct Cyc_List_List* _temp1447=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp985,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( sd2->fields))->v); struct Cyc_List_List* _temp1448= 0;{ struct Cyc_List_List*
_temp1449=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd2->fields))->v;
for( 0; _temp1449 != 0; _temp1449=(( struct Cyc_List_List*) _check_null(
_temp1449))->tl){ struct Cyc_List_List* _temp1450= _temp1447; for( 0; _temp1450
!= 0; _temp1450=(( struct Cyc_List_List*) _check_null( _temp1450))->tl){ if((*((
struct _tuple10*)(( struct Cyc_List_List*) _check_null( _temp1450))->hd)).f1 ==(
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp1449))->hd){
struct _tuple10 _temp1453; struct Cyc_Absyn_Exp* _temp1454; struct Cyc_Absyn_Structfield*
_temp1456; struct _tuple10* _temp1451=( struct _tuple10*)(( struct Cyc_List_List*)
_check_null( _temp1450))->hd; _temp1453=* _temp1451; _LL1457: _temp1456=
_temp1453.f1; goto _LL1455; _LL1455: _temp1454= _temp1453.f2; goto _LL1452;
_LL1452: { void* _temp1458=( void*) _temp1456->type; Cyc_Toc_exp_to_c( nv,
_temp1454); if( Cyc_Toc_is_void_star( _temp1458)){( void*)( _temp1454->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp1454->r, 0), 0))->r));} _temp1448=({ struct Cyc_List_List* _temp1459=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1459->hd=(
void*)({ struct _tuple4* _temp1460=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp1460->f1= 0; _temp1460->f2= _temp1454; _temp1460;}); _temp1459->tl=
_temp1448; _temp1459;}); break;}}}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0,(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp1448), 0))->r));}; _pop_region( rgn);}} goto _LL798; _LL858: { struct Cyc_List_List*
fs;{ void* _temp1461= Cyc_Tcutil_compress( _temp993); struct Cyc_List_List*
_temp1467; _LL1463: if(( unsigned int) _temp1461 > 4u?*(( int*) _temp1461) ==
Cyc_Absyn_AnonStructType: 0){ _LL1468: _temp1467=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp1461)->f1; goto _LL1464;} else{ goto _LL1465;} _LL1465: goto _LL1466;
_LL1464: fs= _temp1467; goto _LL1462; _LL1466: fs=({ struct Cyc_Stdio_String_pa_struct
_temp1470; _temp1470.tag= Cyc_Stdio_String_pa; _temp1470.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( _temp993);{ void* _temp1469[ 1u]={& _temp1470};(( struct
Cyc_List_List*(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("anon struct has type %s", sizeof( unsigned char), 24u), _tag_arr(
_temp1469, sizeof( void*), 1u));}}); goto _LL1462; _LL1462:;}{ struct
_RegionHandle _temp1471= _new_region(); struct _RegionHandle* rgn=& _temp1471;
_push_region( rgn);{ struct Cyc_List_List* _temp1472=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp991, fs); for( 0; _temp1472 != 0; _temp1472=(( struct Cyc_List_List*)
_check_null( _temp1472))->tl){ struct _tuple10 _temp1475; struct Cyc_Absyn_Exp*
_temp1476; struct Cyc_Absyn_Structfield* _temp1478; struct _tuple10* _temp1473=(
struct _tuple10*)(( struct Cyc_List_List*) _check_null( _temp1472))->hd;
_temp1475=* _temp1473; _LL1479: _temp1478= _temp1475.f1; goto _LL1477; _LL1477:
_temp1476= _temp1475.f2; goto _LL1474; _LL1474: { void* _temp1480=( void*)
_temp1478->type; Cyc_Toc_exp_to_c( nv, _temp1476); if( Cyc_Toc_is_void_star(
_temp1480)){( void*)( _temp1476->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1476->r, 0), 0))->r));}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0, _temp991, 0))->r));}; _pop_region( rgn);}
goto _LL798;} _LL860: { struct _tuple0* qv= _temp995->name; if( _temp997->is_xtunion?
1: ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));}
else{ int tag_count= 0; struct Cyc_List_List* _temp1481= _temp997->fields == 0?
0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp997->fields))->v;
while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp1481))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1481))->hd)->typs == 0){ tag_count ++;}
_temp1481=(( struct Cyc_List_List*) _check_null( _temp1481))->tl;}( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));} goto
_LL798;} _LL862: { struct _tuple0* _temp1482= _temp1001->name; struct Cyc_List_List*
_temp1483= _temp1001->typs; struct _tuple0* _temp1484= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* _temp1485= Cyc_Absyn_var_exp( _temp1484, 0); void*
_temp1486= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1482, _tag_arr("_struct",
sizeof( unsigned char), 8u))); if( nv->toplevel){ struct Cyc_Absyn_Exp* tag_exp;
if( _temp1003->is_xtunion){ tag_exp= Cyc_Absyn_var_exp( _temp1482, 0);} else{
int tag_count= 0; struct Cyc_List_List* _temp1487= _temp1003->fields == 0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1003->fields))->v;
while( Cyc_Absyn_qvar_cmp( _temp1482,(( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp1487))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1487))->hd)->typs != 0){ tag_count ++;}
_temp1487=(( struct Cyc_List_List*) _check_null( _temp1487))->tl;} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* _temp1488= 0; for( 0;
_temp1005 != 0;( _temp1005=(( struct Cyc_List_List*) _check_null( _temp1005))->tl,
_temp1483=(( struct Cyc_List_List*) _check_null( _temp1483))->tl)){ struct Cyc_Absyn_Exp*
cur_e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1005))->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1483))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);} _temp1488=({
struct Cyc_List_List* _temp1489=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1489->hd=( void*)({ struct _tuple4* _temp1490=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1490->f1= 0;
_temp1490->f2= cur_e; _temp1490;}); _temp1489->tl= _temp1488; _temp1489;});}
_temp1488=({ struct Cyc_List_List* _temp1491=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1491->hd=( void*)({ struct _tuple4*
_temp1492=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1492->f1=
0; _temp1492->f2= tag_exp; _temp1492;}); _temp1491->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1488); _temp1491;});( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp1493=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1493->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp1482, _tag_arr("_struct",
sizeof( unsigned char), 8u)); _temp1493;}), _temp1488, 0))->r));}} else{ struct
Cyc_List_List* _temp1494=({ struct Cyc_List_List* _temp1500=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1500->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp1485, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp(
_temp1482, 0), 0); _temp1500->tl= 0; _temp1500;});{ int i= 1; for( 0; _temp1005
!= 0;((( _temp1005=(( struct Cyc_List_List*) _check_null( _temp1005))->tl, i ++)),
_temp1483=(( struct Cyc_List_List*) _check_null( _temp1483))->tl)){ struct Cyc_Absyn_Exp*
cur_e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1005))->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1483))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt*
_temp1495= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1485, Cyc_Toc_fieldname(
i), 0), cur_e, 0); _temp1494=({ struct Cyc_List_List* _temp1496=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1496->hd=( void*) _temp1495;
_temp1496->tl= _temp1494; _temp1496;});}}}{ struct Cyc_Absyn_Stmt* _temp1497=
Cyc_Absyn_exp_stmt( _temp1485, 0); struct Cyc_Absyn_Stmt* _temp1498= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct
Cyc_List_List* _temp1499=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1499->hd=( void*) _temp1497; _temp1499->tl= _temp1494; _temp1499;})), 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1484, _temp1486, 0, _temp1498, 0), 0))->r));}} goto _LL798;} _LL864: goto
_LL798; _LL866: { void* t_c= Cyc_Toc_typ_to_c( _temp1007); if( _temp1009 != 0){
struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*) _check_null( _temp1009); Cyc_Toc_exp_to_c(
nv, rgn);( void*)( e->r=( void*)(( void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp(
_temp1007, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));} goto _LL798;} _LL868: Cyc_Toc_stmt_to_c(
nv, _temp1011); goto _LL798; _LL870:({ void* _temp1501[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("UnresolvedMem",
sizeof( unsigned char), 14u), _tag_arr( _temp1501, sizeof( void*), 0u));}); goto
_LL798; _LL872:({ void* _temp1502[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("compoundlit", sizeof(
unsigned char), 12u), _tag_arr( _temp1502, sizeof( void*), 0u));}); goto _LL798;
_LL874:({ void* _temp1503[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("codegen", sizeof( unsigned char), 8u),
_tag_arr( _temp1503, sizeof( void*), 0u));}); goto _LL798; _LL876:({ void*
_temp1504[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("fill", sizeof( unsigned char), 5u), _tag_arr( _temp1504, sizeof( void*),
0u));}); goto _LL798; _LL798:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_if_eq_goto_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct _tagged_arr* succ_lab,
struct _tagged_arr* fail_lab){ return Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp(
e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt( fail_lab, 0),
0);} struct _tuple11{ struct Cyc_Toc_Env* f1; struct Cyc_List_List* f2; struct
Cyc_Absyn_Stmt* f3; } ; struct _tuple12{ struct _tuple0* f1; void* f2; } ;
struct _tuple13{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; static
struct _tuple11 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env* nv, struct _RegionHandle*
rgn, void* t, struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path, struct Cyc_Absyn_Pat*
p, struct _tagged_arr* succ_lab, struct _tagged_arr* fail_lab, struct Cyc_List_List*
decls){ struct Cyc_Absyn_Stmt* s;{ void* _temp1505=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp1541; struct Cyc_Absyn_Vardecl _temp1543; struct _tuple0* _temp1544; struct
Cyc_Absyn_Vardecl* _temp1546; int _temp1548; void* _temp1550; unsigned char
_temp1552; struct _tagged_arr _temp1554; struct Cyc_Absyn_Enumfield* _temp1556;
struct Cyc_Absyn_Enumdecl* _temp1558; struct Cyc_List_List* _temp1560; struct
Cyc_List_List* _temp1562; struct Cyc_Absyn_Tunionfield* _temp1564; struct Cyc_Absyn_Tuniondecl*
_temp1566; struct Cyc_Absyn_Pat* _temp1569; struct Cyc_Absyn_Pat _temp1571; void*
_temp1572; struct Cyc_List_List* _temp1574; struct Cyc_List_List* _temp1576;
struct Cyc_Absyn_Tunionfield* _temp1578; struct Cyc_Absyn_Tuniondecl* _temp1580;
struct Cyc_List_List* _temp1582; struct Cyc_List_List* _temp1584; struct Cyc_List_List*
_temp1586; struct Cyc_List_List* _temp1588; struct Cyc_Core_Opt* _temp1590;
struct Cyc_Absyn_Structdecl* _temp1592; struct Cyc_Absyn_Pat* _temp1594; _LL1507:
if(( unsigned int) _temp1505 > 2u?*(( int*) _temp1505) == Cyc_Absyn_Var_p: 0){
_LL1542: _temp1541=(( struct Cyc_Absyn_Var_p_struct*) _temp1505)->f1; _temp1543=*
_temp1541; _LL1545: _temp1544= _temp1543.name; goto _LL1508;} else{ goto _LL1509;}
_LL1509: if( _temp1505 ==( void*) Cyc_Absyn_Wild_p){ goto _LL1510;} else{ goto
_LL1511;} _LL1511: if(( unsigned int) _temp1505 > 2u?*(( int*) _temp1505) == Cyc_Absyn_Reference_p:
0){ _LL1547: _temp1546=(( struct Cyc_Absyn_Reference_p_struct*) _temp1505)->f1;
goto _LL1512;} else{ goto _LL1513;} _LL1513: if( _temp1505 ==( void*) Cyc_Absyn_Null_p){
goto _LL1514;} else{ goto _LL1515;} _LL1515: if(( unsigned int) _temp1505 > 2u?*((
int*) _temp1505) == Cyc_Absyn_Int_p: 0){ _LL1551: _temp1550=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1505)->f1; goto _LL1549; _LL1549: _temp1548=((
struct Cyc_Absyn_Int_p_struct*) _temp1505)->f2; goto _LL1516;} else{ goto
_LL1517;} _LL1517: if(( unsigned int) _temp1505 > 2u?*(( int*) _temp1505) == Cyc_Absyn_Char_p:
0){ _LL1553: _temp1552=(( struct Cyc_Absyn_Char_p_struct*) _temp1505)->f1; goto
_LL1518;} else{ goto _LL1519;} _LL1519: if(( unsigned int) _temp1505 > 2u?*((
int*) _temp1505) == Cyc_Absyn_Float_p: 0){ _LL1555: _temp1554=(( struct Cyc_Absyn_Float_p_struct*)
_temp1505)->f1; goto _LL1520;} else{ goto _LL1521;} _LL1521: if(( unsigned int)
_temp1505 > 2u?*(( int*) _temp1505) == Cyc_Absyn_Enum_p: 0){ _LL1559: _temp1558=((
struct Cyc_Absyn_Enum_p_struct*) _temp1505)->f1; goto _LL1557; _LL1557:
_temp1556=(( struct Cyc_Absyn_Enum_p_struct*) _temp1505)->f2; goto _LL1522;}
else{ goto _LL1523;} _LL1523: if(( unsigned int) _temp1505 > 2u?*(( int*)
_temp1505) == Cyc_Absyn_Tunion_p: 0){ _LL1567: _temp1566=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1505)->f1; goto _LL1565; _LL1565: _temp1564=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1505)->f2; goto _LL1563; _LL1563: _temp1562=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1505)->f3; goto _LL1561; _LL1561: _temp1560=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1505)->f4; if( _temp1560 == 0){ goto _LL1524;} else{ goto _LL1525;}} else{
goto _LL1525;} _LL1525: if(( unsigned int) _temp1505 > 2u?*(( int*) _temp1505)
== Cyc_Absyn_Pointer_p: 0){ _LL1570: _temp1569=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1505)->f1; _temp1571=* _temp1569; _LL1573: _temp1572=( void*) _temp1571.r;
if(( unsigned int) _temp1572 > 2u?*(( int*) _temp1572) == Cyc_Absyn_Tunion_p: 0){
_LL1581: _temp1580=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1572)->f1; goto
_LL1579; _LL1579: _temp1578=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1572)->f2;
goto _LL1577; _LL1577: _temp1576=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1572)->f3;
goto _LL1575; _LL1575: _temp1574=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1572)->f4;
goto _LL1568;} else{ goto _LL1527;}} else{ goto _LL1527;} _LL1568: if( _temp1574
!= 0){ goto _LL1526;} else{ goto _LL1527;} _LL1527: if(( unsigned int) _temp1505
> 2u?*(( int*) _temp1505) == Cyc_Absyn_Tunion_p: 0){ _LL1583: _temp1582=((
struct Cyc_Absyn_Tunion_p_struct*) _temp1505)->f4; goto _LL1528;} else{ goto
_LL1529;} _LL1529: if(( unsigned int) _temp1505 > 2u?*(( int*) _temp1505) == Cyc_Absyn_Tuple_p:
0){ _LL1585: _temp1584=(( struct Cyc_Absyn_Tuple_p_struct*) _temp1505)->f1; goto
_LL1530;} else{ goto _LL1531;} _LL1531: if(( unsigned int) _temp1505 > 2u?*((
int*) _temp1505) == Cyc_Absyn_Struct_p: 0){ _LL1593: _temp1592=(( struct Cyc_Absyn_Struct_p_struct*)
_temp1505)->f1; goto _LL1591; _LL1591: _temp1590=(( struct Cyc_Absyn_Struct_p_struct*)
_temp1505)->f2; goto _LL1589; _LL1589: _temp1588=(( struct Cyc_Absyn_Struct_p_struct*)
_temp1505)->f3; goto _LL1587; _LL1587: _temp1586=(( struct Cyc_Absyn_Struct_p_struct*)
_temp1505)->f4; goto _LL1532;} else{ goto _LL1533;} _LL1533: if(( unsigned int)
_temp1505 > 2u?*(( int*) _temp1505) == Cyc_Absyn_Pointer_p: 0){ _LL1595:
_temp1594=(( struct Cyc_Absyn_Pointer_p_struct*) _temp1505)->f1; goto _LL1534;}
else{ goto _LL1535;} _LL1535: if(( unsigned int) _temp1505 > 2u?*(( int*)
_temp1505) == Cyc_Absyn_UnknownId_p: 0){ goto _LL1536;} else{ goto _LL1537;}
_LL1537: if(( unsigned int) _temp1505 > 2u?*(( int*) _temp1505) == Cyc_Absyn_UnknownCall_p:
0){ goto _LL1538;} else{ goto _LL1539;} _LL1539: if(( unsigned int) _temp1505 >
2u?*(( int*) _temp1505) == Cyc_Absyn_UnknownFields_p: 0){ goto _LL1540;} else{
goto _LL1506;} _LL1508: nv= Cyc_Toc_add_varmap( nv, _temp1544, r); goto _LL1510;
_LL1510: s= Cyc_Absyn_goto_stmt( succ_lab, 0); goto _LL1506; _LL1512: { struct
_tuple0* _temp1596= Cyc_Toc_temp_var(); decls=({ struct Cyc_List_List* _temp1597=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1597->hd=( void*)({ struct _tuple12* _temp1598=( struct _tuple12*)
_region_malloc( rgn, sizeof( struct _tuple12)); _temp1598->f1= _temp1596;
_temp1598->f2= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq);
_temp1598;}); _temp1597->tl= decls; _temp1597;}); nv= Cyc_Toc_add_varmap( nv,
_temp1546->name, Cyc_Absyn_var_exp( _temp1596, 0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp1596, 0), Cyc_Absyn_address_exp( path, 0), 0), Cyc_Absyn_goto_stmt(
succ_lab, 0), 0); goto _LL1506;} _LL1514: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp(
0, 0), succ_lab, fail_lab); goto _LL1506; _LL1516: s= Cyc_Toc_if_eq_goto_stmt( r,
Cyc_Absyn_int_exp( _temp1550, _temp1548, 0), succ_lab, fail_lab); goto _LL1506;
_LL1518: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp( _temp1552, 0),
succ_lab, fail_lab); goto _LL1506; _LL1520: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp(
_temp1554, 0), succ_lab, fail_lab); goto _LL1506; _LL1522: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp1599=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp1599[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp1600; _temp1600.tag= Cyc_Absyn_Enum_e;
_temp1600.f1= _temp1556->name; _temp1600.f2=( struct Cyc_Absyn_Enumdecl*)
_temp1558; _temp1600.f3=( struct Cyc_Absyn_Enumfield*) _temp1556; _temp1600;});
_temp1599;}), 0), succ_lab, fail_lab); goto _LL1506; _LL1524: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp1564->name, 0); if( ! _temp1566->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL1506;} _LL1526: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1574);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp1578->name, _tag_arr("_struct",
sizeof( unsigned char), 8u)); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0); struct
Cyc_List_List* _temp1601=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1578->typs); struct Cyc_List_List*
_temp1602=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp1574); for( 0; _temp1602 != 0;((( _temp1602=((
struct Cyc_List_List*) _check_null( _temp1602))->tl, _temp1601=(( struct Cyc_List_List*)
_check_null( _temp1601))->tl)), cnt --)){ struct Cyc_Absyn_Pat* _temp1603=(
struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null( _temp1602))->hd;
if(( void*) _temp1603->r ==( void*) Cyc_Absyn_Wild_p){ continue;}{ void*
_temp1604=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1601))->hd)).f2;
struct _tuple0* _temp1605= Cyc_Toc_temp_var(); void* _temp1606=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1603->topt))->v; void* _temp1607= Cyc_Toc_typ_to_c(
_temp1606); struct _tagged_arr* _temp1608= Cyc_Toc_fresh_label(); struct Cyc_Absyn_Exp*
_temp1609= Cyc_Absyn_structarrow_exp( rcast, Cyc_Toc_fieldname( cnt), 0); if(
Cyc_Toc_is_void_star( Cyc_Toc_typ_to_c( _temp1604))){ _temp1609= Cyc_Absyn_cast_exp(
_temp1607, _temp1609, 0);} decls=({ struct Cyc_List_List* _temp1610=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1610->hd=( void*)({
struct _tuple12* _temp1611=( struct _tuple12*) _region_malloc( rgn, sizeof(
struct _tuple12)); _temp1611->f1= _temp1605; _temp1611->f2= _temp1607; _temp1611;});
_temp1610->tl= decls; _temp1610;});{ struct _tuple11 _temp1612= Cyc_Toc_xlate_pat(
nv, rgn, _temp1606, Cyc_Absyn_var_exp( _temp1605, 0), _temp1609, _temp1603,
succ_lab, fail_lab, decls); nv= _temp1612.f1; decls= _temp1612.f2;{ struct Cyc_Absyn_Stmt*
_temp1613= _temp1612.f3; struct Cyc_Absyn_Stmt* _temp1614= Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp1605, 0), _temp1609, 0); struct Cyc_Absyn_Stmt*
_temp1615= Cyc_Absyn_seq_stmt( _temp1614, _temp1613, 0); ss=({ struct Cyc_List_List*
_temp1616=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1616->hd=( void*) Cyc_Absyn_label_stmt( _temp1608, _temp1615, 0); _temp1616->tl=
ss; _temp1616;}); succ_lab= _temp1608;}}}} if( ss == 0){ ss=({ struct Cyc_List_List*
_temp1617=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1617->hd=( void*) Cyc_Absyn_goto_stmt( succ_lab, 0); _temp1617->tl= 0;
_temp1617;});}{ struct Cyc_Absyn_Exp* test_exp; if( _temp1580->is_xtunion){
struct Cyc_Absyn_Exp* e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(),
Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0);
struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp1578->name, 0); test_exp= Cyc_Absyn_eq_exp(
e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* _temp1618=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1580->fields))->v; for( 0; _temp1618 != 0; _temp1618=(( struct
Cyc_List_List*) _check_null( _temp1618))->tl){ struct Cyc_Absyn_Tunionfield*
_temp1619=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp1618))->hd; if( _temp1619->typs == 0){ max_tag ++;}}}{ struct Cyc_Absyn_Exp*
max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_t, r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp(
_temp1578->name, 0); test_exp= max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp(
e1, e, 0), 0): Cyc_Absyn_eq_exp( e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt(
test_exp, Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);
goto _LL1506;}} _LL1528: _temp1584= _temp1582; goto _LL1530; _LL1530: { struct
Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1584);{ struct Cyc_List_List* _temp1620=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1584); for(
0; _temp1620 != 0;( _temp1620=(( struct Cyc_List_List*) _check_null( _temp1620))->tl,
cnt --)){ struct Cyc_Absyn_Pat* _temp1621=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp1620))->hd; if(( void*) _temp1621->r ==( void*) Cyc_Absyn_Wild_p){
continue;}{ struct _tuple0* _temp1622= Cyc_Toc_temp_var(); void* _temp1623=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1621->topt))->v; struct
_tagged_arr* _temp1624= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1625=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1625->hd=( void*)({ struct _tuple12* _temp1626=( struct _tuple12*)
_region_malloc( rgn, sizeof( struct _tuple12)); _temp1626->f1= _temp1622;
_temp1626->f2= Cyc_Toc_typ_to_c( _temp1623); _temp1626;}); _temp1625->tl= decls;
_temp1625;});{ struct _tuple11 _temp1627= Cyc_Toc_xlate_pat( nv, rgn, _temp1623,
Cyc_Absyn_var_exp( _temp1622, 0), Cyc_Absyn_structmember_exp( path, Cyc_Toc_fieldname(
cnt), 0), _temp1621, succ_lab, fail_lab, decls); nv= _temp1627.f1; decls=
_temp1627.f2;{ struct Cyc_Absyn_Stmt* _temp1628= _temp1627.f3; struct Cyc_Absyn_Stmt*
_temp1629= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp1622, 0), Cyc_Absyn_structmember_exp( r, Cyc_Toc_fieldname( cnt), 0), 0),
_temp1628, 0); ss=({ struct Cyc_List_List* _temp1630=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1630->hd=( void*) Cyc_Absyn_label_stmt(
_temp1624, _temp1629, 0); _temp1630->tl= ss; _temp1630;}); succ_lab= _temp1624;}}}}}
if( ss == 0){ ss=({ struct Cyc_List_List* _temp1631=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1631->hd=( void*) Cyc_Absyn_goto_stmt(
succ_lab, 0); _temp1631->tl= 0; _temp1631;});} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL1506;} _LL1532: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
_temp1632=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp1586); for( 0; _temp1632 != 0; _temp1632=(( struct
Cyc_List_List*) _check_null( _temp1632))->tl){ struct _tuple13* _temp1633=(
struct _tuple13*)(( struct Cyc_List_List*) _check_null( _temp1632))->hd; struct
Cyc_Absyn_Pat* _temp1634=(* _temp1633).f2; if(( void*) _temp1634->r ==( void*)
Cyc_Absyn_Wild_p){ continue;}{ struct _tagged_arr* f;{ void* _temp1635=( void*)((
struct Cyc_List_List*) _check_null((* _temp1633).f1))->hd; struct _tagged_arr*
_temp1641; _LL1637: if(*(( int*) _temp1635) == Cyc_Absyn_FieldName){ _LL1642:
_temp1641=(( struct Cyc_Absyn_FieldName_struct*) _temp1635)->f1; goto _LL1638;}
else{ goto _LL1639;} _LL1639: goto _LL1640; _LL1638: f= _temp1641; goto _LL1636;
_LL1640:( int) _throw(( void*) Cyc_Toc_Match_error); _LL1636:;}{ struct _tuple0*
_temp1643= Cyc_Toc_temp_var(); void* _temp1644=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1634->topt))->v; void* _temp1645= Cyc_Toc_typ_to_c( _temp1644);
struct _tagged_arr* _temp1646= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1647=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1647->hd=( void*)({ struct _tuple12* _temp1648=( struct _tuple12*)
_region_malloc( rgn, sizeof( struct _tuple12)); _temp1648->f1= _temp1643;
_temp1648->f2= _temp1645; _temp1648;}); _temp1647->tl= decls; _temp1647;});{
struct _tuple11 _temp1649= Cyc_Toc_xlate_pat( nv, rgn, _temp1644, Cyc_Absyn_var_exp(
_temp1643, 0), Cyc_Absyn_structmember_exp( path, f, 0), _temp1634, succ_lab,
fail_lab, decls); nv= _temp1649.f1; decls= _temp1649.f2;{ struct Cyc_Absyn_Exp*
_temp1650= Cyc_Absyn_structmember_exp( r, f, 0); if( Cyc_Toc_is_void_star(( void*)((
struct Cyc_Absyn_Structfield*) _check_null( Cyc_Absyn_lookup_field(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1592->fields))->v, f)))->type)){
_temp1650= Cyc_Absyn_cast_exp( _temp1645, _temp1650, 0);}{ struct Cyc_Absyn_Stmt*
_temp1651= _temp1649.f3; struct Cyc_Absyn_Stmt* _temp1652= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1643, 0), _temp1650, 0),
_temp1651, 0); ss=({ struct Cyc_List_List* _temp1653=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1653->hd=( void*) Cyc_Absyn_label_stmt(
_temp1646, _temp1652, 0); _temp1653->tl= ss; _temp1653;}); succ_lab= _temp1646;}}}}}}}
if( ss == 0){ ss=({ struct Cyc_List_List* _temp1654=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1654->hd=( void*) Cyc_Absyn_goto_stmt(
succ_lab, 0); _temp1654->tl= 0; _temp1654;});} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL1506;} _LL1534: { struct _tuple0* _temp1655= Cyc_Toc_temp_var(); void*
_temp1656=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1594->topt))->v;
decls=({ struct Cyc_List_List* _temp1657=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp1657->hd=( void*)({ struct _tuple12*
_temp1658=( struct _tuple12*) _region_malloc( rgn, sizeof( struct _tuple12));
_temp1658->f1= _temp1655; _temp1658->f2= Cyc_Toc_typ_to_c( _temp1656); _temp1658;});
_temp1657->tl= decls; _temp1657;});{ struct _tuple11 _temp1659= Cyc_Toc_xlate_pat(
nv, rgn, _temp1656, Cyc_Absyn_var_exp( _temp1655, 0), Cyc_Absyn_deref_exp( path,
0), _temp1594, succ_lab, fail_lab, decls); nv= _temp1659.f1; decls= _temp1659.f2;{
struct Cyc_Absyn_Stmt* _temp1660= _temp1659.f3; struct Cyc_Absyn_Stmt* _temp1661=
Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1655, 0), Cyc_Absyn_deref_exp(
r, 0), 0), _temp1660, 0); if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp1661, 0);} else{ s= _temp1661;} goto _LL1506;}}} _LL1536: s=({
void* _temp1662[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknownid", sizeof(
unsigned char), 10u), _tag_arr( _temp1662, sizeof( void*), 0u));}); goto _LL1506;
_LL1538: s=({ void* _temp1663[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknowncall",
sizeof( unsigned char), 12u), _tag_arr( _temp1663, sizeof( void*), 0u));}); goto
_LL1506; _LL1540: s=({ void* _temp1664[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknownfields",
sizeof( unsigned char), 14u), _tag_arr( _temp1664, sizeof( void*), 0u));}); goto
_LL1506; _LL1506:;} return({ struct _tuple11 _temp1665; _temp1665.f1= nv;
_temp1665.f2= decls; _temp1665.f3= s; _temp1665;});} struct _tuple14{ struct
_tagged_arr* f1; struct _tagged_arr* f2; struct Cyc_Absyn_Switch_clause* f3; } ;
static struct _tuple14* Cyc_Toc_gen_label( struct _RegionHandle* r, struct Cyc_Absyn_Switch_clause*
sc){ return({ struct _tuple14* _temp1666=( struct _tuple14*) _region_malloc( r,
sizeof( struct _tuple14)); _temp1666->f1= Cyc_Toc_fresh_label(); _temp1666->f2=
Cyc_Toc_fresh_label(); _temp1666->f3= sc; _temp1666;});} static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* scs){ Cyc_Toc_exp_to_c( nv, e);{ void* _temp1667=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; int leave_as_switch;{ void*
_temp1668= Cyc_Tcutil_compress( _temp1667); _LL1670: if(( unsigned int)
_temp1668 > 4u?*(( int*) _temp1668) == Cyc_Absyn_IntType: 0){ goto _LL1671;}
else{ goto _LL1672;} _LL1672: goto _LL1673; _LL1671: leave_as_switch= 1; goto
_LL1669; _LL1673: leave_as_switch= 0; goto _LL1669; _LL1669:;}{ struct Cyc_List_List*
_temp1674= scs; for( 0; _temp1674 != 0; _temp1674=(( struct Cyc_List_List*)
_check_null( _temp1674))->tl){ if(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp1674))->hd)->pat_vars))->v != 0? 1:(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp1674))->hd)->where_clause != 0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_arr* next_l=
Cyc_Toc_fresh_label();{ struct Cyc_List_List* _temp1675= scs; for( 0; _temp1675
!= 0; _temp1675=(( struct Cyc_List_List*) _check_null( _temp1675))->tl){ struct
Cyc_Absyn_Stmt* _temp1676=(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp1675))->hd)->body;(( struct Cyc_Absyn_Switch_clause*)(( struct
Cyc_List_List*) _check_null( _temp1675))->hd)->body= Cyc_Absyn_label_stmt(
next_l, _temp1676, 0); next_l= Cyc_Toc_fresh_label(); Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env(
nv, next_l), _temp1676);}} return;}{ struct _tuple0* v= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp( v, 0); struct Cyc_Absyn_Exp* path=
Cyc_Absyn_var_exp( v, 0); struct _tagged_arr* end_l= Cyc_Toc_fresh_label();
struct _RegionHandle _temp1677= _new_region(); struct _RegionHandle* rgn=&
_temp1677; _push_region( rgn);{ struct Cyc_List_List* lscs=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple14*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Toc_gen_label,
rgn, scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0;
struct Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp1678= lscs; for( 0;
_temp1678 != 0; _temp1678=(( struct Cyc_List_List*) _check_null( _temp1678))->tl){
struct Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null( _temp1678))->hd)).f3; struct _tagged_arr* fail_lab=(( struct Cyc_List_List*)
_check_null( _temp1678))->tl == 0? end_l:(*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1678))->tl))->hd)).f1;
struct _tagged_arr* succ_lab=(*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null( _temp1678))->hd)).f2; if( sc->where_clause == 0){ struct _tuple11
_temp1679= Cyc_Toc_xlate_pat( nv, rgn, _temp1667, r, path, sc->pattern, succ_lab,
fail_lab, decls); nvs=({ struct Cyc_List_List* _temp1680=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1680->hd=( void*)
_temp1679.f1; _temp1680->tl= nvs; _temp1680;}); decls= _temp1679.f2; test_stmts=({
struct Cyc_List_List* _temp1681=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp1681->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( _temp1678))->hd)).f1,
_temp1679.f3, 0); _temp1681->tl= test_stmts; _temp1681;});} else{ struct Cyc_Absyn_Exp*
_temp1682=( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause); struct
_tagged_arr* _temp1683= Cyc_Toc_fresh_label(); struct _tuple11 _temp1684= Cyc_Toc_xlate_pat(
nv, rgn, _temp1667, r, path, sc->pattern, _temp1683, fail_lab, decls); Cyc_Toc_exp_to_c(
_temp1684.f1, _temp1682); nvs=({ struct Cyc_List_List* _temp1685=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1685->hd=( void*)
_temp1684.f1; _temp1685->tl= nvs; _temp1685;}); decls= _temp1684.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp1682, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp1683, s2,
0); test_stmts=({ struct Cyc_List_List* _temp1686=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1686->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( _temp1678))->hd)).f1,
Cyc_Absyn_seq_stmt( _temp1684.f3, s3, 0), 0); _temp1686->tl= test_stmts;
_temp1686;});}}}}{ struct Cyc_Absyn_Stmt* _temp1687= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
test_stmts), 0); nvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt( 0); if( lscs != 0){
for( 0;(( struct Cyc_List_List*) _check_null( lscs))->tl != 0;( lscs=(( struct
Cyc_List_List*) _check_null( lscs))->tl, nvs=(( struct Cyc_List_List*)
_check_null( nvs))->tl)){ struct Cyc_Toc_Env* _temp1688=( struct Cyc_Toc_Env*)((
struct Cyc_List_List*) _check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env( _temp1688, end_l,(*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( lscs))->tl))->hd)).f2,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null(((*(( struct _tuple14*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f3)->pat_vars))->v,(
struct Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( nvs))->tl))->hd), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0),
0);}{ struct Cyc_Toc_Env* _temp1689=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*(( struct _tuple14*)((
struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body; Cyc_Toc_stmt_to_c(
Cyc_Toc_last_switchclause_env( _temp1689, end_l), s); clauses= Cyc_Absyn_seq_stmt(
clauses, Cyc_Absyn_label_stmt((*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null( lscs))->hd)).f2, s, 0), 0);}}{ struct Cyc_Absyn_Stmt* res= Cyc_Absyn_seq_stmt(
_temp1687, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt( end_l, Cyc_Absyn_skip_stmt(
0), 0), 0), 0); for( decls; decls != 0; decls=(( struct Cyc_List_List*)
_check_null( decls))->tl){ struct _tuple12 _temp1692; void* _temp1693; struct
_tuple0* _temp1695; struct _tuple12* _temp1690=( struct _tuple12*)(( struct Cyc_List_List*)
_check_null( decls))->hd; _temp1692=* _temp1690; _LL1696: _temp1695= _temp1692.f1;
goto _LL1694; _LL1694: _temp1693= _temp1692.f2; goto _LL1691; _LL1691: res= Cyc_Absyn_declare_stmt(
_temp1695, _temp1693, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}; _pop_region( rgn);}}} static
struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat*
p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt*
s); static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv, struct Cyc_Toc_Env*
init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt* s); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__npop_handler_e,({ struct Cyc_List_List* _temp1697=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1697[ 0]=({ struct Cyc_List_List
_temp1698; _temp1698.hd=( void*) Cyc_Absyn_uint_exp(( unsigned int)( n - 1), 0);
_temp1698.tl= 0; _temp1698;}); _temp1697;}), 0), 0);} void Cyc_Toc_do_npop_before(
int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt(( void*) s->r,
0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* s){ while( 1) { void* _temp1699=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1741; struct Cyc_Absyn_Stmt* _temp1743; struct Cyc_Absyn_Stmt* _temp1745;
struct Cyc_Absyn_Exp* _temp1747; struct Cyc_Absyn_Stmt* _temp1749; struct Cyc_Absyn_Stmt*
_temp1751; struct Cyc_Absyn_Exp* _temp1753; struct Cyc_Absyn_Stmt* _temp1755;
struct _tuple2 _temp1757; struct Cyc_Absyn_Exp* _temp1759; struct Cyc_Absyn_Stmt*
_temp1761; struct Cyc_Absyn_Stmt* _temp1763; struct Cyc_Absyn_Stmt* _temp1765;
struct Cyc_Absyn_Stmt* _temp1767; struct _tuple2 _temp1769; struct Cyc_Absyn_Exp*
_temp1771; struct _tuple2 _temp1773; struct Cyc_Absyn_Exp* _temp1775; struct Cyc_Absyn_Exp*
_temp1777; struct Cyc_List_List* _temp1779; struct Cyc_Absyn_Exp* _temp1781;
struct Cyc_Absyn_Switch_clause** _temp1783; struct Cyc_List_List* _temp1785;
struct Cyc_Absyn_Stmt* _temp1787; struct Cyc_Absyn_Decl* _temp1789; struct Cyc_Absyn_Stmt*
_temp1791; struct _tagged_arr* _temp1793; struct _tuple2 _temp1795; struct Cyc_Absyn_Exp*
_temp1797; struct Cyc_Absyn_Stmt* _temp1799; struct Cyc_List_List* _temp1801;
struct Cyc_Absyn_Stmt* _temp1803; struct Cyc_Absyn_Stmt* _temp1805; struct Cyc_Absyn_Vardecl*
_temp1807; struct Cyc_Absyn_Tvar* _temp1809; _LL1701: if( _temp1699 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL1702;} else{ goto _LL1703;} _LL1703: if((
unsigned int) _temp1699 > 1u?*(( int*) _temp1699) == Cyc_Absyn_Exp_s: 0){
_LL1742: _temp1741=(( struct Cyc_Absyn_Exp_s_struct*) _temp1699)->f1; goto
_LL1704;} else{ goto _LL1705;} _LL1705: if(( unsigned int) _temp1699 > 1u?*((
int*) _temp1699) == Cyc_Absyn_Seq_s: 0){ _LL1746: _temp1745=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1699)->f1; goto _LL1744; _LL1744: _temp1743=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1699)->f2; goto _LL1706;} else{ goto _LL1707;} _LL1707: if(( unsigned int)
_temp1699 > 1u?*(( int*) _temp1699) == Cyc_Absyn_Return_s: 0){ _LL1748:
_temp1747=(( struct Cyc_Absyn_Return_s_struct*) _temp1699)->f1; goto _LL1708;}
else{ goto _LL1709;} _LL1709: if(( unsigned int) _temp1699 > 1u?*(( int*)
_temp1699) == Cyc_Absyn_IfThenElse_s: 0){ _LL1754: _temp1753=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1699)->f1; goto _LL1752; _LL1752: _temp1751=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1699)->f2; goto _LL1750; _LL1750: _temp1749=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1699)->f3; goto _LL1710;} else{ goto _LL1711;} _LL1711: if(( unsigned int)
_temp1699 > 1u?*(( int*) _temp1699) == Cyc_Absyn_While_s: 0){ _LL1758: _temp1757=((
struct Cyc_Absyn_While_s_struct*) _temp1699)->f1; _LL1760: _temp1759= _temp1757.f1;
goto _LL1756; _LL1756: _temp1755=(( struct Cyc_Absyn_While_s_struct*) _temp1699)->f2;
goto _LL1712;} else{ goto _LL1713;} _LL1713: if(( unsigned int) _temp1699 > 1u?*((
int*) _temp1699) == Cyc_Absyn_Break_s: 0){ _LL1762: _temp1761=(( struct Cyc_Absyn_Break_s_struct*)
_temp1699)->f1; goto _LL1714;} else{ goto _LL1715;} _LL1715: if(( unsigned int)
_temp1699 > 1u?*(( int*) _temp1699) == Cyc_Absyn_Continue_s: 0){ _LL1764:
_temp1763=(( struct Cyc_Absyn_Continue_s_struct*) _temp1699)->f1; goto _LL1716;}
else{ goto _LL1717;} _LL1717: if(( unsigned int) _temp1699 > 1u?*(( int*)
_temp1699) == Cyc_Absyn_Goto_s: 0){ _LL1766: _temp1765=(( struct Cyc_Absyn_Goto_s_struct*)
_temp1699)->f2; goto _LL1718;} else{ goto _LL1719;} _LL1719: if(( unsigned int)
_temp1699 > 1u?*(( int*) _temp1699) == Cyc_Absyn_For_s: 0){ _LL1778: _temp1777=((
struct Cyc_Absyn_For_s_struct*) _temp1699)->f1; goto _LL1774; _LL1774: _temp1773=((
struct Cyc_Absyn_For_s_struct*) _temp1699)->f2; _LL1776: _temp1775= _temp1773.f1;
goto _LL1770; _LL1770: _temp1769=(( struct Cyc_Absyn_For_s_struct*) _temp1699)->f3;
_LL1772: _temp1771= _temp1769.f1; goto _LL1768; _LL1768: _temp1767=(( struct Cyc_Absyn_For_s_struct*)
_temp1699)->f4; goto _LL1720;} else{ goto _LL1721;} _LL1721: if(( unsigned int)
_temp1699 > 1u?*(( int*) _temp1699) == Cyc_Absyn_Switch_s: 0){ _LL1782:
_temp1781=(( struct Cyc_Absyn_Switch_s_struct*) _temp1699)->f1; goto _LL1780;
_LL1780: _temp1779=(( struct Cyc_Absyn_Switch_s_struct*) _temp1699)->f2; goto
_LL1722;} else{ goto _LL1723;} _LL1723: if(( unsigned int) _temp1699 > 1u?*((
int*) _temp1699) == Cyc_Absyn_Fallthru_s: 0){ _LL1786: _temp1785=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1699)->f1; goto _LL1784; _LL1784: _temp1783=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1699)->f2; goto _LL1724;} else{ goto _LL1725;} _LL1725: if(( unsigned int)
_temp1699 > 1u?*(( int*) _temp1699) == Cyc_Absyn_Decl_s: 0){ _LL1790: _temp1789=((
struct Cyc_Absyn_Decl_s_struct*) _temp1699)->f1; goto _LL1788; _LL1788:
_temp1787=(( struct Cyc_Absyn_Decl_s_struct*) _temp1699)->f2; goto _LL1726;}
else{ goto _LL1727;} _LL1727: if(( unsigned int) _temp1699 > 1u?*(( int*)
_temp1699) == Cyc_Absyn_Label_s: 0){ _LL1794: _temp1793=(( struct Cyc_Absyn_Label_s_struct*)
_temp1699)->f1; goto _LL1792; _LL1792: _temp1791=(( struct Cyc_Absyn_Label_s_struct*)
_temp1699)->f2; goto _LL1728;} else{ goto _LL1729;} _LL1729: if(( unsigned int)
_temp1699 > 1u?*(( int*) _temp1699) == Cyc_Absyn_Do_s: 0){ _LL1800: _temp1799=((
struct Cyc_Absyn_Do_s_struct*) _temp1699)->f1; goto _LL1796; _LL1796: _temp1795=((
struct Cyc_Absyn_Do_s_struct*) _temp1699)->f2; _LL1798: _temp1797= _temp1795.f1;
goto _LL1730;} else{ goto _LL1731;} _LL1731: if(( unsigned int) _temp1699 > 1u?*((
int*) _temp1699) == Cyc_Absyn_TryCatch_s: 0){ _LL1804: _temp1803=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1699)->f1; goto _LL1802; _LL1802: _temp1801=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1699)->f2; goto _LL1732;} else{ goto _LL1733;} _LL1733: if(( unsigned int)
_temp1699 > 1u?*(( int*) _temp1699) == Cyc_Absyn_Region_s: 0){ _LL1810:
_temp1809=(( struct Cyc_Absyn_Region_s_struct*) _temp1699)->f1; goto _LL1808;
_LL1808: _temp1807=(( struct Cyc_Absyn_Region_s_struct*) _temp1699)->f2; goto
_LL1806; _LL1806: _temp1805=(( struct Cyc_Absyn_Region_s_struct*) _temp1699)->f3;
goto _LL1734;} else{ goto _LL1735;} _LL1735: if(( unsigned int) _temp1699 > 1u?*((
int*) _temp1699) == Cyc_Absyn_SwitchC_s: 0){ goto _LL1736;} else{ goto _LL1737;}
_LL1737: if(( unsigned int) _temp1699 > 1u?*(( int*) _temp1699) == Cyc_Absyn_Cut_s:
0){ goto _LL1738;} else{ goto _LL1739;} _LL1739: if(( unsigned int) _temp1699 >
1u?*(( int*) _temp1699) == Cyc_Absyn_Splice_s: 0){ goto _LL1740;} else{ goto
_LL1700;} _LL1702: return; _LL1704: Cyc_Toc_exp_to_c( nv, _temp1741); return;
_LL1706: Cyc_Toc_stmt_to_c( nv, _temp1745); s= _temp1743; continue; _LL1708: {
struct Cyc_Core_Opt* topt= 0; if( _temp1747 != 0){ topt=({ struct Cyc_Core_Opt*
_temp1811=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1811->v=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*) _check_null( _temp1747))->topt))->v);
_temp1811;}); Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null(
_temp1747));} if( s->try_depth > 0){ if( topt != 0){ struct _tuple0* _temp1812=
Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* _temp1813= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( _temp1812, 0), 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( _temp1812,( void*)(( struct Cyc_Core_Opt*)
_check_null( topt))->v, _temp1747, Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler(
s->try_depth), _temp1813, 0), 0))->r));} else{ Cyc_Toc_do_npop_before( s->try_depth,
s);}} return;} _LL1710: Cyc_Toc_exp_to_c( nv, _temp1753); Cyc_Toc_stmt_to_c( nv,
_temp1751); s= _temp1749; continue; _LL1712: Cyc_Toc_exp_to_c( nv, _temp1759);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1755); return; _LL1714: if( nv->break_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( nv->break_lab))->v, 0))->r));}{ int
dest_depth= _temp1761 == 0? 0:(( struct Cyc_Absyn_Stmt*) _check_null( _temp1761))->try_depth;
Cyc_Toc_do_npop_before( s->try_depth - dest_depth, s); return;} _LL1716: if( nv->continue_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( nv->continue_lab))->v, 0))->r));} _temp1765=
_temp1763; goto _LL1718; _LL1718: Cyc_Toc_do_npop_before( s->try_depth -((
struct Cyc_Absyn_Stmt*) _check_null( _temp1765))->try_depth, s); return; _LL1720:
Cyc_Toc_exp_to_c( nv, _temp1777); Cyc_Toc_exp_to_c( nv, _temp1775); Cyc_Toc_exp_to_c(
nv, _temp1771); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1767); return;
_LL1722: Cyc_Toc_xlate_switch( nv, s, _temp1781, _temp1779); return; _LL1724:
if( nv->fallthru_info == 0){( int) _throw(({ void* _temp1814[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("fallthru in unexpected place", sizeof( unsigned char), 29u), _tag_arr(
_temp1814, sizeof( void*), 0u));}));}{ struct _tuple6 _temp1817; struct Cyc_Dict_Dict*
_temp1818; struct Cyc_List_List* _temp1820; struct _tagged_arr* _temp1822;
struct _tuple6* _temp1815=( struct _tuple6*)(( struct Cyc_Core_Opt*) _check_null(
nv->fallthru_info))->v; _temp1817=* _temp1815; _LL1823: _temp1822= _temp1817.f1;
goto _LL1821; _LL1821: _temp1820= _temp1817.f2; goto _LL1819; _LL1819: _temp1818=
_temp1817.f3; goto _LL1816; _LL1816: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt(
_temp1822, 0); Cyc_Toc_do_npop_before( s->try_depth -((*(( struct Cyc_Absyn_Switch_clause**)
_check_null( _temp1783)))->body)->try_depth, s2);{ struct Cyc_List_List*
_temp1824=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1820); struct Cyc_List_List* _temp1825=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( _temp1785); for( 0; _temp1824 != 0;( _temp1824=((
struct Cyc_List_List*) _check_null( _temp1824))->tl, _temp1825=(( struct Cyc_List_List*)
_check_null( _temp1825))->tl)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1825))->hd); s2= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key)) Cyc_Dict_lookup)( _temp1818,( struct _tuple0*)(( struct
Cyc_List_List*) _check_null( _temp1824))->hd),( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1825))->hd, 0), s2, 0);}( void*)( s->r=( void*)((
void*) s2->r)); return;}}} _LL1726:{ void* _temp1826=( void*) _temp1789->r;
struct Cyc_Absyn_Vardecl* _temp1836; int _temp1838; struct Cyc_Absyn_Exp*
_temp1840; struct Cyc_Core_Opt* _temp1842; struct Cyc_Absyn_Pat* _temp1844;
struct Cyc_List_List* _temp1846; _LL1828: if(*(( int*) _temp1826) == Cyc_Absyn_Var_d){
_LL1837: _temp1836=(( struct Cyc_Absyn_Var_d_struct*) _temp1826)->f1; goto
_LL1829;} else{ goto _LL1830;} _LL1830: if(*(( int*) _temp1826) == Cyc_Absyn_Let_d){
_LL1845: _temp1844=(( struct Cyc_Absyn_Let_d_struct*) _temp1826)->f1; goto
_LL1843; _LL1843: _temp1842=(( struct Cyc_Absyn_Let_d_struct*) _temp1826)->f3;
goto _LL1841; _LL1841: _temp1840=(( struct Cyc_Absyn_Let_d_struct*) _temp1826)->f4;
goto _LL1839; _LL1839: _temp1838=(( struct Cyc_Absyn_Let_d_struct*) _temp1826)->f5;
goto _LL1831;} else{ goto _LL1832;} _LL1832: if(*(( int*) _temp1826) == Cyc_Absyn_Letv_d){
_LL1847: _temp1846=(( struct Cyc_Absyn_Letv_d_struct*) _temp1826)->f1; goto
_LL1833;} else{ goto _LL1834;} _LL1834: goto _LL1835; _LL1829: { struct Cyc_Toc_Env*
_temp1848= Cyc_Toc_add_varmap( nv, _temp1836->name, Cyc_Absyn_varb_exp(
_temp1836->name,( void*)({ struct Cyc_Absyn_Local_b_struct* _temp1849=( struct
Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp1849[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp1850; _temp1850.tag= Cyc_Absyn_Local_b;
_temp1850.f1= _temp1836; _temp1850;}); _temp1849;}), 0)); Cyc_Toc_local_decl_to_c(
_temp1848, _temp1848, _temp1836, _temp1787); goto _LL1827;} _LL1831:{ void*
_temp1851=( void*) _temp1844->r; struct Cyc_Absyn_Vardecl* _temp1857; _LL1853:
if(( unsigned int) _temp1851 > 2u?*(( int*) _temp1851) == Cyc_Absyn_Var_p: 0){
_LL1858: _temp1857=(( struct Cyc_Absyn_Var_p_struct*) _temp1851)->f1; goto
_LL1854;} else{ goto _LL1855;} _LL1855: goto _LL1856; _LL1854: { struct _tuple0*
old_name= _temp1857->name; struct _tuple0* new_name= Cyc_Toc_temp_var();
_temp1857->name= new_name; _temp1857->initializer=( struct Cyc_Absyn_Exp*)
_temp1840;( void*)( _temp1789->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp1859=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1859[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp1860; _temp1860.tag= Cyc_Absyn_Var_d;
_temp1860.f1= _temp1857; _temp1860;}); _temp1859;})));{ struct Cyc_Toc_Env*
_temp1861= Cyc_Toc_add_varmap( nv, old_name, Cyc_Absyn_varb_exp( new_name,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp1862=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp1862[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp1863; _temp1863.tag= Cyc_Absyn_Local_b; _temp1863.f1=
_temp1857; _temp1863;}); _temp1862;}), 0)); Cyc_Toc_local_decl_to_c( _temp1861,
nv, _temp1857, _temp1787); goto _LL1852;}} _LL1856:( void*)( s->r=( void*)((
void*)( Cyc_Toc_letdecl_to_c( nv, _temp1844, _temp1842, _temp1840, _temp1838,
_temp1787))->r)); goto _LL1852; _LL1852:;} goto _LL1827; _LL1833: { struct Cyc_List_List*
_temp1864=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1846); if( _temp1864 == 0){({ void* _temp1865[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("empty Letv_d",
sizeof( unsigned char), 13u), _tag_arr( _temp1865, sizeof( void*), 0u));});}(
void*)( _temp1789->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp1866=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1866[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp1867; _temp1867.tag= Cyc_Absyn_Var_d;
_temp1867.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp1864))->hd; _temp1867;}); _temp1866;}))); _temp1864=(( struct Cyc_List_List*)
_check_null( _temp1864))->tl; for( 0; _temp1864 != 0; _temp1864=(( struct Cyc_List_List*)
_check_null( _temp1864))->tl){ struct Cyc_Absyn_Decl* _temp1868= Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp1869=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp1869[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp1870; _temp1870.tag= Cyc_Absyn_Var_d; _temp1870.f1=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp1864))->hd; _temp1870;}); _temp1869;}),
0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_decl_stmt( _temp1868, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));} Cyc_Toc_stmt_to_c( nv, s); goto _LL1827;} _LL1835:({
void* _temp1871[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("bad nested declaration within function", sizeof(
unsigned char), 39u), _tag_arr( _temp1871, sizeof( void*), 0u));}); goto _LL1827;
_LL1827:;} return; _LL1728: s= _temp1791; continue; _LL1730: Cyc_Toc_stmt_to_c(
Cyc_Toc_loop_env( nv), _temp1799); Cyc_Toc_exp_to_c( nv, _temp1797); return;
_LL1732: { struct _tuple0* h_var= Cyc_Toc_temp_var(); struct _tuple0* e_var= Cyc_Toc_temp_var();
struct _tuple0* was_thrown_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp=
Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp(
e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); e_exp->topt=({ struct Cyc_Core_Opt* _temp1872=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1872->v=( void*) e_typ; _temp1872;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp1803);{
struct Cyc_Absyn_Stmt* _temp1873= Cyc_Absyn_seq_stmt( _temp1803, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( Cyc_Toc__pop_handler_e, 0, 0), 0), 0); struct _tuple0*
_temp1874= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp1875= Cyc_Absyn_var_exp(
_temp1874, 0); struct Cyc_Absyn_Vardecl* _temp1876= Cyc_Absyn_new_vardecl(
_temp1874, Cyc_Absyn_exn_typ, 0); _temp1875->topt=({ struct Cyc_Core_Opt*
_temp1877=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1877->v=( void*) Cyc_Absyn_exn_typ; _temp1877;});{ struct Cyc_Absyn_Pat*
_temp1878=({ struct Cyc_Absyn_Pat* _temp1894=( struct Cyc_Absyn_Pat*) GC_malloc(
sizeof( struct Cyc_Absyn_Pat)); _temp1894->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp1896=( struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp1896[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp1897; _temp1897.tag= Cyc_Absyn_Var_p;
_temp1897.f1= _temp1876; _temp1897;}); _temp1896;})); _temp1894->topt=({ struct
Cyc_Core_Opt* _temp1895=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1895->v=( void*) Cyc_Absyn_exn_typ; _temp1895;}); _temp1894->loc= 0;
_temp1894;}); struct Cyc_Absyn_Exp* _temp1879= Cyc_Absyn_throw_exp( _temp1875, 0);
_temp1879->topt=({ struct Cyc_Core_Opt* _temp1880=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1880->v=( void*)(( void*) Cyc_Absyn_VoidType);
_temp1880;});{ struct Cyc_Absyn_Stmt* _temp1881= Cyc_Absyn_exp_stmt( _temp1879,
0); struct Cyc_Absyn_Switch_clause* _temp1882=({ struct Cyc_Absyn_Switch_clause*
_temp1891=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp1891->pattern= _temp1878; _temp1891->pat_vars=({ struct Cyc_Core_Opt*
_temp1892=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1892->v=( void*)({ struct Cyc_List_List* _temp1893=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1893->hd=( void*) _temp1876;
_temp1893->tl= 0; _temp1893;}); _temp1892;}); _temp1891->where_clause= 0;
_temp1891->body= _temp1881; _temp1891->loc= 0; _temp1891;}); struct Cyc_Absyn_Stmt*
_temp1883= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp1801,({ struct Cyc_List_List*
_temp1890=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1890->hd=( void*) _temp1882; _temp1890->tl= 0; _temp1890;})), 0); Cyc_Toc_stmt_to_c(
nv, _temp1883);{ struct Cyc_Absyn_Exp* _temp1884= Cyc_Absyn_fncall_exp( Cyc_Toc_setjmp_e,({
struct Cyc_List_List* _temp1889=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1889->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp1889->tl= 0; _temp1889;}), 0); struct Cyc_Absyn_Stmt*
_temp1885= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__push_handler_e,({
struct Cyc_List_List* _temp1888=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1888->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0);
_temp1888->tl= 0; _temp1888;}), 0), 0); struct Cyc_Absyn_Exp* _temp1886= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 0, 0); struct Cyc_Absyn_Exp* _temp1887= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 1, 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp1885, Cyc_Absyn_declare_stmt(
was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp1886, Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt( _temp1884, Cyc_Absyn_assign_stmt( was_thrown_exp,
_temp1887, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp1873, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, Cyc_Toc__exn_thrown_e,
0), _temp1883, 0), 0), 0), 0), 0), 0))->r)); return;}}}}} _LL1734: { void*
rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); struct
_tuple0* rh_var= Cyc_Toc_temp_var(); struct _tuple0* x_var= _temp1807->name;
struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp( rh_var, 0); struct Cyc_Absyn_Exp*
x_exp= Cyc_Absyn_var_exp( x_var, 0); Cyc_Toc_stmt_to_c( Cyc_Toc_add_varmap( nv,
x_var, x_exp), _temp1805);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_fncall_exp( Cyc_Toc__new_region_e,
0, 0), Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp( rh_exp, 0), Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__push_region_e,({ struct Cyc_Absyn_Exp* _temp1898[ 1u]; _temp1898[ 0u]=
x_exp;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1898, sizeof( struct Cyc_Absyn_Exp*), 1u));}), 0), 0), Cyc_Absyn_seq_stmt(
_temp1805, Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__pop_region_e,({
struct Cyc_Absyn_Exp* _temp1899[ 1u]; _temp1899[ 0u]= x_exp;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1899, sizeof( struct Cyc_Absyn_Exp*),
1u));}), 0), 0), 0), 0), 0), 0))->r)); return;} _LL1736:({ void* _temp1900[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("switchC_s", sizeof( unsigned char), 10u), _tag_arr( _temp1900, sizeof(
void*), 0u));}); return; _LL1738:({ void* _temp1901[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("cut", sizeof(
unsigned char), 4u), _tag_arr( _temp1901, sizeof( void*), 0u));}); return;
_LL1740:({ void* _temp1902[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("splice", sizeof( unsigned char), 7u),
_tag_arr( _temp1902, sizeof( void*), 0u));}); return; _LL1700:;}} struct
_tuple15{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ;
static void Cyc_Toc_fndecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl*
f){ f->tvs= 0; f->effect= 0; f->rgn_po= 0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c((
void*) f->ret_type));{ struct Cyc_List_List* _temp1903= f->args; for( 0;
_temp1903 != 0; _temp1903=(( struct Cyc_List_List*) _check_null( _temp1903))->tl){(*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( _temp1903))->hd)).f3=
Cyc_Toc_typ_to_c((*(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
_temp1903))->hd)).f3);{ struct _tuple0* _temp1904=({ struct _tuple0* _temp1905=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp1905->f1=( void*) Cyc_Absyn_Loc_n;
_temp1905->f2=(*(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
_temp1903))->hd)).f1; _temp1905;}); nv= Cyc_Toc_add_varmap( nv, _temp1904, Cyc_Absyn_var_exp(
_temp1904, 0));}}} if(( unsigned int) f->cyc_varargs?(( struct Cyc_Absyn_VarargInfo*)
_check_null( f->cyc_varargs))->name != 0: 0){ int _temp1908; void* _temp1910;
void* _temp1912; struct Cyc_Absyn_Tqual _temp1914; struct Cyc_Core_Opt*
_temp1916; struct Cyc_Absyn_VarargInfo _temp1906=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( f->cyc_varargs)); _LL1917: _temp1916= _temp1906.name; goto _LL1915;
_LL1915: _temp1914= _temp1906.tq; goto _LL1913; _LL1913: _temp1912=( void*)
_temp1906.type; goto _LL1911; _LL1911: _temp1910=( void*) _temp1906.rgn; goto
_LL1909; _LL1909: _temp1908= _temp1906.inject; goto _LL1907; _LL1907: { void*
_temp1918= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ( _temp1912, _temp1910,
_temp1914)); struct _tuple0* _temp1919=({ struct _tuple0* _temp1922=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp1922->f1=( void*) Cyc_Absyn_Loc_n;
_temp1922->f2=( struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null(
_temp1916))->v; _temp1922;}); f->args=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( f->args,({ struct Cyc_List_List*
_temp1920=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1920->hd=( void*)({ struct _tuple15* _temp1921=( struct _tuple15*)
GC_malloc( sizeof( struct _tuple15)); _temp1921->f1=( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( _temp1916))->v; _temp1921->f2= _temp1914;
_temp1921->f3= _temp1918; _temp1921;}); _temp1920->tl= 0; _temp1920;})); nv= Cyc_Toc_add_varmap(
nv, _temp1919, Cyc_Absyn_var_exp( _temp1919, 0)); f->cyc_varargs= 0;}}{ struct
Cyc_List_List* _temp1923=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( f->param_vardecls))->v; for( 0; _temp1923 != 0; _temp1923=(( struct
Cyc_List_List*) _check_null( _temp1923))->tl){( void*)((( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp1923))->hd)->type=( void*) Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp1923))->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel( nv), f->body);}
static void* Cyc_Toc_scope_to_c( void* s){ void* _temp1924= s; _LL1926: if(
_temp1924 ==( void*) Cyc_Absyn_Abstract){ goto _LL1927;} else{ goto _LL1928;}
_LL1928: if( _temp1924 ==( void*) Cyc_Absyn_ExternC){ goto _LL1929;} else{ goto
_LL1930;} _LL1930: goto _LL1931; _LL1927: return( void*) Cyc_Absyn_Public;
_LL1929: return( void*) Cyc_Absyn_Extern; _LL1931: return s; _LL1925:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp1932=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; if( Cyc_Toc_structs_so_far == 0){
Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp1933=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1933->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp1933;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp1934=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(*
_temp1932).f2); if( _temp1934 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp1932).f2, s));} else{ if(((
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Core_Opt*) _check_null( _temp1934))->v)->fields
== 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(* _temp1932).f2,
s)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)((
void*) Cyc_Absyn_Public)); s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp1935=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1935->v=(
void*) _temp1932; _temp1935;}); if( s->fields != 0){ if( seen_defn_before){ s->fields=
0;} else{ struct Cyc_List_List* _temp1936=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; for( 0; _temp1936 != 0; _temp1936=(( struct Cyc_List_List*)
_check_null( _temp1936))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp1936))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1936))->hd)->type));}}}}} struct Cyc_Core_Opt* Cyc_Toc_unions_so_far= 0;
static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl* u){ struct
_tuple0* _temp1937=( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( u->name))->v;
if( Cyc_Toc_unions_so_far == 0){ Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt*
_temp1938=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1938->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp1938;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp1939=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp1937).f2); if( _temp1939 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp1937).f2, u));} else{ if(((
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Core_Opt*) _check_null( _temp1939))->v)->fields
== 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(* _temp1937).f2,
u)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)( u->sc=( void*)((
void*) Cyc_Absyn_Public)); u->tvs= 0; u->name=({ struct Cyc_Core_Opt* _temp1940=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1940->v=(
void*) _temp1937; _temp1940;}); if( u->fields != 0){ if( seen_defn_before){ u->fields=
0;} else{ struct Cyc_List_List* _temp1941=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( u->fields))->v; for( 0; _temp1941 != 0; _temp1941=(( struct Cyc_List_List*)
_check_null( _temp1941))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp1941))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1941))->hd)->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp1942=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1942->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp1942;});}{ struct
_tuple0* _temp1943= tud->name; if( tud->fields == 0){ return;} if((( int(*)(
struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)(( struct Cyc_Set_Set*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v, _temp1943)){
return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v=(
void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tuple0* elt))
Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp1943));{ int tag_count= 0; int box_count= 0; struct Cyc_List_List*
_temp1944=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( tud->fields))->v;
for( 0; _temp1944 != 0; _temp1944=(( struct Cyc_List_List*) _check_null(
_temp1944))->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1944))->hd; if( f->typs == 0){ struct
Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp( tag_count
++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name, Cyc_Absyn_sint_t,
eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp1945; _temp1945.q_const= 1;
_temp1945.q_volatile= 0; _temp1945.q_restrict= 0; _temp1945;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp1946=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1946->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp1947=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp1947[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp1948; _temp1948.tag= Cyc_Absyn_Var_d; _temp1948.f1= vd; _temp1948;});
_temp1947;}), 0); _temp1946->tl= Cyc_Toc_result_decls; _temp1946;});} else{
struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp1949; _temp1949.q_const=
1; _temp1949.q_volatile= 0; _temp1949.q_restrict= 0; _temp1949;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp1950=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1950->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp1951=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp1951[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp1952; _temp1952.tag= Cyc_Absyn_Var_d; _temp1952.f1= vd; _temp1952;});
_temp1951;}), 0); _temp1950->tl= Cyc_Toc_result_decls; _temp1950;});{ struct Cyc_List_List*
_temp1953= 0; int i= 1;{ struct Cyc_List_List* _temp1954= f->typs; for( 0;
_temp1954 != 0;( _temp1954=(( struct Cyc_List_List*) _check_null( _temp1954))->tl,
i ++)){ struct _tagged_arr* _temp1955= Cyc_Toc_fieldname( i); struct Cyc_Absyn_Structfield*
_temp1956=({ struct Cyc_Absyn_Structfield* _temp1958=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp1958->name= _temp1955;
_temp1958->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null(
_temp1954))->hd)).f1; _temp1958->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1954))->hd)).f2);
_temp1958->width= 0; _temp1958->attributes= 0; _temp1958;}); _temp1953=({ struct
Cyc_List_List* _temp1957=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1957->hd=( void*) _temp1956; _temp1957->tl= _temp1953; _temp1957;});}}
_temp1953=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp1953); _temp1953=({ struct Cyc_List_List* _temp1959=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1959->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp1960=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp1960->name= Cyc_Toc_tag_sp; _temp1960->tq= Cyc_Toc_mt_tq; _temp1960->type=(
void*) Cyc_Absyn_sint_t; _temp1960->width= 0; _temp1960->attributes= 0;
_temp1960;}); _temp1959->tl= _temp1953; _temp1959;});{ struct Cyc_Absyn_Structdecl*
_temp1961=({ struct Cyc_Absyn_Structdecl* _temp1965=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp1965->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp1965->name=({ struct Cyc_Core_Opt* _temp1967=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1967->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp1967;}); _temp1965->tvs= 0; _temp1965->fields=({ struct Cyc_Core_Opt*
_temp1966=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1966->v=( void*) _temp1953; _temp1966;}); _temp1965->attributes= 0;
_temp1965;}); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp1962=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1962->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp1963=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp1963[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp1964; _temp1964.tag= Cyc_Absyn_Struct_d;
_temp1964.f1= _temp1961; _temp1964;}); _temp1963;}), 0); _temp1962->tl= Cyc_Toc_result_decls;
_temp1962;});}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far == 0){ Cyc_Toc_xtunion_fields_so_far=({ struct Cyc_Core_Opt*
_temp1968=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1968->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp1968;});} if( xd->fields
== 0){ return;}{ struct _tuple0* _temp1969= xd->name; struct Cyc_List_List*
_temp1970=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( xd->fields))->v;
for( 0; _temp1970 != 0; _temp1970=(( struct Cyc_List_List*) _check_null(
_temp1970))->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1970))->hd; struct _tagged_arr* fn=(* f->name).f2;
struct Cyc_Absyn_Exp* _temp1971= Cyc_Absyn_uint_exp( _get_arr_size(* fn, sizeof(
unsigned char)) + 4, 0); struct Cyc_Absyn_ArrayType_struct* _temp1972=({ struct
Cyc_Absyn_ArrayType_struct* _temp2022=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2022[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2023; _temp2023.tag= Cyc_Absyn_ArrayType;
_temp2023.f1=( void*) Cyc_Absyn_uchar_t; _temp2023.f2= Cyc_Toc_mt_tq; _temp2023.f3=(
struct Cyc_Absyn_Exp*) _temp1971; _temp2023;}); _temp2022;}); struct Cyc_Core_Opt*
_temp1973=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tuple0*
key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct Cyc_Core_Opt
_temp1981; int _temp1982; _LL1975: if( _temp1973 == 0){ goto _LL1976;} else{
goto _LL1977;} _LL1977: if( _temp1973 == 0){ goto _LL1979;} else{ _temp1981=*
_temp1973; _LL1983: _temp1982=( int) _temp1981.v; if( _temp1982 == 0){ goto
_LL1978;} else{ goto _LL1979;}} _LL1979: goto _LL1980; _LL1976: { struct Cyc_Absyn_Exp*
initopt= 0; if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){ initopt=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_string_exp(( struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct
_temp1989; _temp1989.tag= Cyc_Stdio_String_pa; _temp1989.f1=( struct _tagged_arr)*
fn;{ struct Cyc_Stdio_Int_pa_struct _temp1988; _temp1988.tag= Cyc_Stdio_Int_pa;
_temp1988.f1=( int)(( unsigned int) 0);{ struct Cyc_Stdio_Int_pa_struct
_temp1987; _temp1987.tag= Cyc_Stdio_Int_pa; _temp1987.f1=( int)(( unsigned int)
0);{ struct Cyc_Stdio_Int_pa_struct _temp1986; _temp1986.tag= Cyc_Stdio_Int_pa;
_temp1986.f1=( int)(( unsigned int) 0);{ struct Cyc_Stdio_Int_pa_struct
_temp1985; _temp1985.tag= Cyc_Stdio_Int_pa; _temp1985.f1=( int)(( unsigned int)
0);{ void* _temp1984[ 5u]={& _temp1985,& _temp1986,& _temp1987,& _temp1988,&
_temp1989}; Cyc_Stdio_aprintf( _tag_arr("%c%c%c%c%s", sizeof( unsigned char), 11u),
_tag_arr( _temp1984, sizeof( void*), 5u));}}}}}}), 0);}{ struct Cyc_Absyn_Vardecl*
_temp1990= Cyc_Absyn_new_vardecl( f->name,( void*) _temp1972, initopt);( void*)(
_temp1990->sc=( void*)(( void*) f->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp1991=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1991->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp1992=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1992[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp1993; _temp1993.tag= Cyc_Absyn_Var_d;
_temp1993.f1= _temp1990; _temp1993;}); _temp1992;}), 0); _temp1991->tl= Cyc_Toc_result_decls;
_temp1991;});( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key,
int data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc !=( void*)
Cyc_Absyn_Extern)); if( f->typs != 0){ struct Cyc_List_List* fields= 0; int i= 1;{
struct Cyc_List_List* _temp1994= f->typs; for( 0; _temp1994 != 0;( _temp1994=((
struct Cyc_List_List*) _check_null( _temp1994))->tl, i ++)){ struct _tagged_arr*
_temp1995=({ struct _tagged_arr* _temp1999=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr)); _temp1999[ 0]=( struct _tagged_arr)({ struct Cyc_Stdio_Int_pa_struct
_temp2001; _temp2001.tag= Cyc_Stdio_Int_pa; _temp2001.f1=( int)(( unsigned int)
i);{ void* _temp2000[ 1u]={& _temp2001}; Cyc_Stdio_aprintf( _tag_arr("f%d",
sizeof( unsigned char), 4u), _tag_arr( _temp2000, sizeof( void*), 1u));}});
_temp1999;}); struct Cyc_Absyn_Structfield* _temp1996=({ struct Cyc_Absyn_Structfield*
_temp1998=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp1998->name= _temp1995; _temp1998->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1994))->hd)).f1; _temp1998->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1994))->hd)).f2);
_temp1998->width= 0; _temp1998->attributes= 0; _temp1998;}); fields=({ struct
Cyc_List_List* _temp1997=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1997->hd=( void*) _temp1996; _temp1997->tl= fields; _temp1997;});}} fields=({
struct Cyc_List_List* _temp2002=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2002->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2003=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2003->name= Cyc_Toc_tag_sp; _temp2003->tq= Cyc_Toc_mt_tq; _temp2003->type=(
void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t, Cyc_Toc_mt_tq); _temp2003->width=
0; _temp2003->attributes= 0; _temp2003;}); _temp2002->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields); _temp2002;});{ struct Cyc_Absyn_Structdecl*
_temp2004=({ struct Cyc_Absyn_Structdecl* _temp2008=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2008->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2008->name=({ struct Cyc_Core_Opt* _temp2010=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2010->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp2010;}); _temp2008->tvs= 0; _temp2008->fields=({ struct Cyc_Core_Opt*
_temp2009=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2009->v=( void*) fields; _temp2009;}); _temp2008->attributes= 0; _temp2008;});
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2005=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2005->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2006=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2006[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2007; _temp2007.tag= Cyc_Absyn_Struct_d;
_temp2007.f1= _temp2004; _temp2007;}); _temp2006;}), 0); _temp2005->tl= Cyc_Toc_result_decls;
_temp2005;});}} goto _LL1974;}} _LL1978: if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2011= Cyc_Absyn_string_exp(( struct _tagged_arr)({
struct Cyc_Stdio_String_pa_struct _temp2021; _temp2021.tag= Cyc_Stdio_String_pa;
_temp2021.f1=( struct _tagged_arr)* fn;{ struct Cyc_Stdio_Int_pa_struct
_temp2020; _temp2020.tag= Cyc_Stdio_Int_pa; _temp2020.f1=( int)(( unsigned int)
0);{ struct Cyc_Stdio_Int_pa_struct _temp2019; _temp2019.tag= Cyc_Stdio_Int_pa;
_temp2019.f1=( int)(( unsigned int) 0);{ struct Cyc_Stdio_Int_pa_struct
_temp2018; _temp2018.tag= Cyc_Stdio_Int_pa; _temp2018.f1=( int)(( unsigned int)
0);{ struct Cyc_Stdio_Int_pa_struct _temp2017; _temp2017.tag= Cyc_Stdio_Int_pa;
_temp2017.f1=( int)(( unsigned int) 0);{ void* _temp2016[ 5u]={& _temp2017,&
_temp2018,& _temp2019,& _temp2020,& _temp2021}; Cyc_Stdio_aprintf( _tag_arr("%c%c%c%c%s",
sizeof( unsigned char), 11u), _tag_arr( _temp2016, sizeof( void*), 5u));}}}}}}),
0); struct Cyc_Absyn_Vardecl* _temp2012= Cyc_Absyn_new_vardecl( f->name,( void*)
_temp1972,( struct Cyc_Absyn_Exp*) _temp2011);( void*)( _temp2012->sc=( void*)((
void*) f->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2013=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2013->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2014=( struct
Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2014[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2015; _temp2015.tag= Cyc_Absyn_Var_d;
_temp2015.f1= _temp2012; _temp2015;}); _temp2014;}), 0); _temp2013->tl= Cyc_Toc_result_decls;
_temp2013;});( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key,
int data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));} goto _LL1974;
_LL1980: goto _LL1974; _LL1974:;}}} static void Cyc_Toc_enumdecl_to_c( struct
Cyc_Toc_Env* nv, struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2024= ed->name;(
void*)( ed->sc=( void*)(( void*) Cyc_Absyn_Public)); if( ed->fields != 0){
struct Cyc_List_List* _temp2025=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ed->fields))->v; for( 0; _temp2025 != 0; _temp2025=(( struct Cyc_List_List*)
_check_null( _temp2025))->tl){ if((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2025))->hd)->tag != 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2025))->hd)->tag));(( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_List_List*) _check_null( _temp2025))->hd)->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
i, 0);}}}} static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv,
struct Cyc_Toc_Env* init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt*
s){( void*)( vd->type=( void*) Cyc_Toc_typ_to_c_array(( void*) vd->type)); Cyc_Toc_stmt_to_c(
body_nv, s); if( vd->initializer != 0){ struct Cyc_Absyn_Exp* init=( struct Cyc_Absyn_Exp*)
_check_null( vd->initializer); void* _temp2026=( void*) init->r; struct Cyc_Absyn_Exp*
_temp2032; struct Cyc_Absyn_Exp* _temp2034; struct Cyc_Absyn_Vardecl* _temp2036;
_LL2028: if(*(( int*) _temp2026) == Cyc_Absyn_Comprehension_e){ _LL2037:
_temp2036=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp2026)->f1; goto
_LL2035; _LL2035: _temp2034=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2026)->f2; goto _LL2033; _LL2033: _temp2032=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2026)->f3; goto _LL2029;} else{ goto _LL2030;} _LL2030: goto _LL2031;
_LL2029: vd->initializer= 0;( void*)( s->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
init_nv, Cyc_Absyn_var_exp( vd->name, 0), _temp2036, _temp2034, _temp2032, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r)); goto _LL2027; _LL2031: if(( void*) vd->sc ==( void*)
Cyc_Absyn_Static){ struct Cyc_Toc_Env* _temp2038= Cyc_Toc_copy_env( init_nv);
_temp2038->toplevel= 1; Cyc_Toc_exp_to_c( _temp2038, init);} else{ Cyc_Toc_exp_to_c(
init_nv, init);} goto _LL2027; _LL2027:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt,
struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)((
struct Cyc_Core_Opt*) _check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct
_tuple0* x= Cyc_Toc_temp_var(); struct _tagged_arr* succ_lab= Cyc_Toc_fresh_label();
struct _tagged_arr* fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();{ struct
_RegionHandle _temp2039= _new_region(); struct _RegionHandle* prgn=& _temp2039;
_push_region( prgn);{ struct Cyc_Absyn_Stmt* _temp2042; struct Cyc_List_List*
_temp2044; struct Cyc_Toc_Env* _temp2046; struct _tuple11 _temp2040= Cyc_Toc_xlate_pat(
nv, prgn, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab,
fail_lab, 0); _LL2047: _temp2046= _temp2040.f1; goto _LL2045; _LL2045: _temp2044=
_temp2040.f2; goto _LL2043; _LL2043: _temp2042= _temp2040.f3; goto _LL2041;
_LL2041: Cyc_Toc_stmt_to_c( _temp2046, s);{ struct Cyc_Absyn_Stmt* _temp2048=
Cyc_Absyn_label_stmt( succ_lab, s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt(
x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2042, _temp2048, 0), 0);} else{ struct Cyc_Absyn_Exp* _temp2049= Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp( 0)); struct Cyc_Absyn_Stmt* _temp2050= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( _temp2049, 0), 0); s= Cyc_Absyn_declare_stmt( x,
Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2042,
Cyc_Absyn_seq_stmt( _temp2050, _temp2048, 0), 0), 0);} for( 0; _temp2044 != 0;
_temp2044=(( struct Cyc_List_List*) _check_null( _temp2044))->tl){ struct
_tuple12 _temp2053; void* _temp2054; struct _tuple0* _temp2056; struct _tuple12*
_temp2051=( struct _tuple12*)(( struct Cyc_List_List*) _check_null( _temp2044))->hd;
_temp2053=* _temp2051; _LL2057: _temp2056= _temp2053.f1; goto _LL2055; _LL2055:
_temp2054= _temp2053.f2; goto _LL2052; _LL2052: s= Cyc_Absyn_declare_stmt(
_temp2056, _temp2054, 0, s, 0);}}}; _pop_region( prgn);} return s;}} static
struct Cyc_Toc_Env* Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv, struct Cyc_List_List*
ds, int top){ for( 0; ds != 0; ds=(( struct Cyc_List_List*) _check_null( ds))->tl){
if( ! nv->toplevel){({ void* _temp2058[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("decls_to_c: not at toplevel!",
sizeof( unsigned char), 29u), _tag_arr( _temp2058, sizeof( void*), 0u));});}{
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( ds))->hd; void* _temp2059=( void*) d->r; struct Cyc_Absyn_Vardecl*
_temp2085; struct Cyc_Absyn_Fndecl* _temp2087; struct Cyc_Absyn_Structdecl*
_temp2089; struct Cyc_Absyn_Uniondecl* _temp2091; struct Cyc_Absyn_Tuniondecl*
_temp2093; struct Cyc_Absyn_Enumdecl* _temp2095; struct Cyc_Absyn_Typedefdecl*
_temp2097; struct Cyc_List_List* _temp2099; struct Cyc_List_List* _temp2101;
struct Cyc_List_List* _temp2103; _LL2061: if(*(( int*) _temp2059) == Cyc_Absyn_Var_d){
_LL2086: _temp2085=(( struct Cyc_Absyn_Var_d_struct*) _temp2059)->f1; goto
_LL2062;} else{ goto _LL2063;} _LL2063: if(*(( int*) _temp2059) == Cyc_Absyn_Fn_d){
_LL2088: _temp2087=(( struct Cyc_Absyn_Fn_d_struct*) _temp2059)->f1; goto
_LL2064;} else{ goto _LL2065;} _LL2065: if(*(( int*) _temp2059) == Cyc_Absyn_Let_d){
goto _LL2066;} else{ goto _LL2067;} _LL2067: if(*(( int*) _temp2059) == Cyc_Absyn_Letv_d){
goto _LL2068;} else{ goto _LL2069;} _LL2069: if(*(( int*) _temp2059) == Cyc_Absyn_Struct_d){
_LL2090: _temp2089=(( struct Cyc_Absyn_Struct_d_struct*) _temp2059)->f1; goto
_LL2070;} else{ goto _LL2071;} _LL2071: if(*(( int*) _temp2059) == Cyc_Absyn_Union_d){
_LL2092: _temp2091=(( struct Cyc_Absyn_Union_d_struct*) _temp2059)->f1; goto
_LL2072;} else{ goto _LL2073;} _LL2073: if(*(( int*) _temp2059) == Cyc_Absyn_Tunion_d){
_LL2094: _temp2093=(( struct Cyc_Absyn_Tunion_d_struct*) _temp2059)->f1; goto
_LL2074;} else{ goto _LL2075;} _LL2075: if(*(( int*) _temp2059) == Cyc_Absyn_Enum_d){
_LL2096: _temp2095=(( struct Cyc_Absyn_Enum_d_struct*) _temp2059)->f1; goto
_LL2076;} else{ goto _LL2077;} _LL2077: if(*(( int*) _temp2059) == Cyc_Absyn_Typedef_d){
_LL2098: _temp2097=(( struct Cyc_Absyn_Typedef_d_struct*) _temp2059)->f1; goto
_LL2078;} else{ goto _LL2079;} _LL2079: if(*(( int*) _temp2059) == Cyc_Absyn_Namespace_d){
_LL2100: _temp2099=(( struct Cyc_Absyn_Namespace_d_struct*) _temp2059)->f2; goto
_LL2080;} else{ goto _LL2081;} _LL2081: if(*(( int*) _temp2059) == Cyc_Absyn_Using_d){
_LL2102: _temp2101=(( struct Cyc_Absyn_Using_d_struct*) _temp2059)->f2; goto
_LL2082;} else{ goto _LL2083;} _LL2083: if(*(( int*) _temp2059) == Cyc_Absyn_ExternC_d){
_LL2104: _temp2103=(( struct Cyc_Absyn_ExternC_d_struct*) _temp2059)->f1; goto
_LL2084;} else{ goto _LL2060;} _LL2062: { struct _tuple0* _temp2105= _temp2085->name;
if(( void*) _temp2085->sc ==( void*) Cyc_Absyn_ExternC){ _temp2105=({ struct
_tuple0* _temp2106=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2106->f1= Cyc_Absyn_rel_ns_null; _temp2106->f2=(* _temp2105).f2; _temp2106;});}
if( _temp2085->initializer != 0){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)
_check_null( _temp2085->initializer));} nv= Cyc_Toc_add_varmap( nv, _temp2085->name,
Cyc_Absyn_varb_exp( _temp2105,( void*)({ struct Cyc_Absyn_Global_b_struct*
_temp2107=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp2107[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp2108; _temp2108.tag= Cyc_Absyn_Global_b;
_temp2108.f1= _temp2085; _temp2108;}); _temp2107;}), 0)); _temp2085->name=
_temp2105;( void*)( _temp2085->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2085->sc));(
void*)( _temp2085->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2085->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2109=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2109->hd=( void*) d; _temp2109->tl=
Cyc_Toc_result_decls; _temp2109;}); goto _LL2060;} _LL2064: { struct _tuple0*
_temp2110= _temp2087->name; if(( void*) _temp2087->sc ==( void*) Cyc_Absyn_ExternC){
_temp2110=({ struct _tuple0* _temp2111=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp2111->f1= Cyc_Absyn_rel_ns_null; _temp2111->f2=(*
_temp2110).f2; _temp2111;});} nv= Cyc_Toc_add_varmap( nv, _temp2087->name, Cyc_Absyn_var_exp(
_temp2110, 0)); _temp2087->name= _temp2110; Cyc_Toc_fndecl_to_c( nv, _temp2087);
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2112=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2112->hd=( void*) d; _temp2112->tl=
Cyc_Toc_result_decls; _temp2112;}); goto _LL2060;} _LL2066: goto _LL2068;
_LL2068:({ void* _temp2113[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("letdecl at toplevel", sizeof(
unsigned char), 20u), _tag_arr( _temp2113, sizeof( void*), 0u));}); goto _LL2060;
_LL2070: Cyc_Toc_structdecl_to_c( _temp2089); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2114=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2114->hd=( void*) d; _temp2114->tl= Cyc_Toc_result_decls; _temp2114;});
goto _LL2060; _LL2072: Cyc_Toc_uniondecl_to_c( _temp2091); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2115=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2115->hd=( void*) d; _temp2115->tl= Cyc_Toc_result_decls;
_temp2115;}); goto _LL2060; _LL2074: if( _temp2093->is_xtunion){ Cyc_Toc_xtuniondecl_to_c(
_temp2093);} else{ Cyc_Toc_tuniondecl_to_c( _temp2093);} goto _LL2060; _LL2076:
Cyc_Toc_enumdecl_to_c( nv, _temp2095); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2116=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2116->hd=( void*) d; _temp2116->tl= Cyc_Toc_result_decls; _temp2116;});
goto _LL2060; _LL2078: _temp2097->name= _temp2097->name; _temp2097->tvs= 0;(
void*)( _temp2097->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2097->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2117=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2117->hd=( void*) d; _temp2117->tl=
Cyc_Toc_result_decls; _temp2117;}); goto _LL2060; _LL2080: _temp2101= _temp2099;
goto _LL2082; _LL2082: _temp2103= _temp2101; goto _LL2084; _LL2084: nv= Cyc_Toc_decls_to_c(
nv, _temp2103, top); goto _LL2060; _LL2060:;}} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_temp_var_counter= 0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far=
0; Cyc_Toc_tunions_so_far= 0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List*
Cyc_Toc_toc( struct Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(),
ds, 1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}
