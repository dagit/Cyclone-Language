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
Cyc_Core_Opt* _temp257; struct Cyc_Core_Opt* _temp259; struct Cyc_Core_Opt
_temp261; void* _temp262; struct Cyc_Absyn_TunionInfo _temp264; void* _temp266;
struct Cyc_Absyn_Tuniondecl* _temp268; struct Cyc_Absyn_TunionFieldInfo _temp270;
void* _temp272; struct Cyc_Absyn_Tunionfield* _temp274; struct Cyc_Absyn_Tuniondecl*
_temp276; struct Cyc_Absyn_PtrInfo _temp278; struct Cyc_Absyn_Conref* _temp280;
struct Cyc_Absyn_Tqual _temp282; void* _temp284; struct Cyc_Absyn_Exp* _temp286;
struct Cyc_Absyn_Tqual _temp288; void* _temp290; struct Cyc_Absyn_FnInfo
_temp292; struct Cyc_List_List* _temp294; struct Cyc_Absyn_VarargInfo* _temp296;
int _temp298; struct Cyc_List_List* _temp300; void* _temp302; struct Cyc_List_List*
_temp304; struct Cyc_List_List* _temp306; struct Cyc_List_List* _temp308; struct
_tuple0* _temp310; struct _tuple0* _temp312; struct _tuple0* _temp314; struct
Cyc_Core_Opt* _temp316; struct Cyc_List_List* _temp318; struct _tuple0* _temp320;
void* _temp322; _LL205: if( _temp203 ==( void*) Cyc_Absyn_VoidType){ goto _LL206;}
else{ goto _LL207;} _LL207: if(( unsigned int) _temp203 > 4u?*(( int*) _temp203)
== Cyc_Absyn_Evar: 0){ _LL258: _temp257=(( struct Cyc_Absyn_Evar_struct*)
_temp203)->f2; if( _temp257 == 0){ goto _LL208;} else{ goto _LL209;}} else{ goto
_LL209;} _LL209: if(( unsigned int) _temp203 > 4u?*(( int*) _temp203) == Cyc_Absyn_Evar:
0){ _LL260: _temp259=(( struct Cyc_Absyn_Evar_struct*) _temp203)->f2; if(
_temp259 == 0){ goto _LL211;} else{ _temp261=* _temp259; _LL263: _temp262=( void*)
_temp261.v; goto _LL210;}} else{ goto _LL211;} _LL211: if(( unsigned int)
_temp203 > 4u?*(( int*) _temp203) == Cyc_Absyn_VarType: 0){ goto _LL212;} else{
goto _LL213;} _LL213: if(( unsigned int) _temp203 > 4u?*(( int*) _temp203) ==
Cyc_Absyn_TunionType: 0){ _LL265: _temp264=(( struct Cyc_Absyn_TunionType_struct*)
_temp203)->f1; _LL267: _temp266=( void*) _temp264.tunion_info; if(*(( int*)
_temp266) == Cyc_Absyn_KnownTunion){ _LL269: _temp268=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp266)->f1; goto _LL214;} else{ goto _LL215;}} else{ goto _LL215;} _LL215:
if(( unsigned int) _temp203 > 4u?*(( int*) _temp203) == Cyc_Absyn_TunionType: 0){
goto _LL216;} else{ goto _LL217;} _LL217: if(( unsigned int) _temp203 > 4u?*((
int*) _temp203) == Cyc_Absyn_TunionFieldType: 0){ _LL271: _temp270=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp203)->f1; _LL273: _temp272=( void*) _temp270.field_info; if(*(( int*)
_temp272) == Cyc_Absyn_KnownTunionfield){ _LL277: _temp276=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp272)->f1; goto _LL275; _LL275: _temp274=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp272)->f2; goto _LL218;} else{ goto _LL219;}} else{ goto _LL219;} _LL219:
if(( unsigned int) _temp203 > 4u?*(( int*) _temp203) == Cyc_Absyn_TunionFieldType:
0){ goto _LL220;} else{ goto _LL221;} _LL221: if(( unsigned int) _temp203 > 4u?*((
int*) _temp203) == Cyc_Absyn_PointerType: 0){ _LL279: _temp278=(( struct Cyc_Absyn_PointerType_struct*)
_temp203)->f1; _LL285: _temp284=( void*) _temp278.elt_typ; goto _LL283; _LL283:
_temp282= _temp278.tq; goto _LL281; _LL281: _temp280= _temp278.bounds; goto
_LL222;} else{ goto _LL223;} _LL223: if(( unsigned int) _temp203 > 4u?*(( int*)
_temp203) == Cyc_Absyn_IntType: 0){ goto _LL224;} else{ goto _LL225;} _LL225:
if( _temp203 ==( void*) Cyc_Absyn_FloatType){ goto _LL226;} else{ goto _LL227;}
_LL227: if( _temp203 ==( void*) Cyc_Absyn_DoubleType){ goto _LL228;} else{ goto
_LL229;} _LL229: if(( unsigned int) _temp203 > 4u?*(( int*) _temp203) == Cyc_Absyn_ArrayType:
0){ _LL291: _temp290=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp203)->f1;
goto _LL289; _LL289: _temp288=(( struct Cyc_Absyn_ArrayType_struct*) _temp203)->f2;
goto _LL287; _LL287: _temp286=(( struct Cyc_Absyn_ArrayType_struct*) _temp203)->f3;
goto _LL230;} else{ goto _LL231;} _LL231: if(( unsigned int) _temp203 > 4u?*((
int*) _temp203) == Cyc_Absyn_FnType: 0){ _LL293: _temp292=(( struct Cyc_Absyn_FnType_struct*)
_temp203)->f1; _LL303: _temp302=( void*) _temp292.ret_typ; goto _LL301; _LL301:
_temp300= _temp292.args; goto _LL299; _LL299: _temp298= _temp292.c_varargs; goto
_LL297; _LL297: _temp296= _temp292.cyc_varargs; goto _LL295; _LL295: _temp294=
_temp292.attributes; goto _LL232;} else{ goto _LL233;} _LL233: if(( unsigned int)
_temp203 > 4u?*(( int*) _temp203) == Cyc_Absyn_TupleType: 0){ _LL305: _temp304=((
struct Cyc_Absyn_TupleType_struct*) _temp203)->f1; goto _LL234;} else{ goto
_LL235;} _LL235: if(( unsigned int) _temp203 > 4u?*(( int*) _temp203) == Cyc_Absyn_AnonStructType:
0){ _LL307: _temp306=(( struct Cyc_Absyn_AnonStructType_struct*) _temp203)->f1;
goto _LL236;} else{ goto _LL237;} _LL237: if(( unsigned int) _temp203 > 4u?*((
int*) _temp203) == Cyc_Absyn_AnonUnionType: 0){ _LL309: _temp308=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp203)->f1; goto _LL238;} else{ goto _LL239;} _LL239: if(( unsigned int)
_temp203 > 4u?*(( int*) _temp203) == Cyc_Absyn_UnionType: 0){ _LL311: _temp310=((
struct Cyc_Absyn_UnionType_struct*) _temp203)->f1; goto _LL240;} else{ goto
_LL241;} _LL241: if(( unsigned int) _temp203 > 4u?*(( int*) _temp203) == Cyc_Absyn_StructType:
0){ _LL313: _temp312=(( struct Cyc_Absyn_StructType_struct*) _temp203)->f1; goto
_LL242;} else{ goto _LL243;} _LL243: if(( unsigned int) _temp203 > 4u?*(( int*)
_temp203) == Cyc_Absyn_EnumType: 0){ _LL315: _temp314=(( struct Cyc_Absyn_EnumType_struct*)
_temp203)->f1; goto _LL244;} else{ goto _LL245;} _LL245: if(( unsigned int)
_temp203 > 4u?*(( int*) _temp203) == Cyc_Absyn_TypedefType: 0){ _LL321: _temp320=((
struct Cyc_Absyn_TypedefType_struct*) _temp203)->f1; goto _LL319; _LL319:
_temp318=(( struct Cyc_Absyn_TypedefType_struct*) _temp203)->f2; goto _LL317;
_LL317: _temp316=(( struct Cyc_Absyn_TypedefType_struct*) _temp203)->f3; goto
_LL246;} else{ goto _LL247;} _LL247: if(( unsigned int) _temp203 > 4u?*(( int*)
_temp203) == Cyc_Absyn_RgnHandleType: 0){ _LL323: _temp322=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp203)->f1; goto _LL248;} else{ goto _LL249;} _LL249: if( _temp203 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL250;} else{ goto _LL251;} _LL251: if(( unsigned int)
_temp203 > 4u?*(( int*) _temp203) == Cyc_Absyn_AccessEff: 0){ goto _LL252;}
else{ goto _LL253;} _LL253: if(( unsigned int) _temp203 > 4u?*(( int*) _temp203)
== Cyc_Absyn_JoinEff: 0){ goto _LL254;} else{ goto _LL255;} _LL255: if((
unsigned int) _temp203 > 4u?*(( int*) _temp203) == Cyc_Absyn_RgnsEff: 0){ goto
_LL256;} else{ goto _LL204;} _LL206: return t; _LL208: return Cyc_Absyn_sint_t;
_LL210: return Cyc_Toc_typ_to_c( _temp262); _LL212: return Cyc_Absyn_void_star_typ();
_LL214: return Cyc_Absyn_void_star_typ(); _LL216:( int) _throw(({ void* _temp324[
0u]={}; Cyc_Tcutil_impos( _tag_arr("unresolved TunionType", sizeof(
unsigned char), 22u), _tag_arr( _temp324, sizeof( void*), 0u));})); _LL218: if(
_temp274->typs == 0){ if( _temp276->is_xtunion){ return Cyc_Absyn_star_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq);} else{ return Cyc_Absyn_uint_t;}}
else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp274->name,
_tag_arr("_struct", sizeof( unsigned char), 8u)));} _LL220:( int) _throw(({ void*
_temp325[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("unresolved TunionFieldType",
sizeof( unsigned char), 27u), _tag_arr( _temp325, sizeof( void*), 0u));}));
_LL222: _temp284= Cyc_Toc_typ_to_c_array( _temp284);{ void* _temp326=( void*)(
Cyc_Absyn_compress_conref( _temp280))->v; void* _temp332; _LL328: if((
unsigned int) _temp326 > 1u?*(( int*) _temp326) == Cyc_Absyn_Eq_constr: 0){
_LL333: _temp332=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp326)->f1;
if( _temp332 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL329;} else{ goto _LL330;}}
else{ goto _LL330;} _LL330: goto _LL331; _LL329: return Cyc_Toc_tagged_arr_typ;
_LL331: return Cyc_Absyn_star_typ( _temp284,( void*) Cyc_Absyn_HeapRgn, _temp282);
_LL327:;} _LL224: return t; _LL226: return t; _LL228: return t; _LL230: return(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp334=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp334[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp335; _temp335.tag= Cyc_Absyn_ArrayType; _temp335.f1=(
void*) Cyc_Toc_typ_to_c_array( _temp290); _temp335.f2= _temp288; _temp335.f3=
_temp286; _temp335;}); _temp334;}); _LL232: { struct Cyc_List_List* _temp336= 0;
for( 0; _temp294 != 0; _temp294=(( struct Cyc_List_List*) _check_null( _temp294))->tl){
void* _temp337=( void*)(( struct Cyc_List_List*) _check_null( _temp294))->hd;
_LL339: if( _temp337 ==( void*) Cyc_Absyn_Noreturn_att){ goto _LL340;} else{
goto _LL341;} _LL341: if( _temp337 ==( void*) Cyc_Absyn_Const_att){ goto _LL342;}
else{ goto _LL343;} _LL343: if(( unsigned int) _temp337 > 16u?*(( int*) _temp337)
== Cyc_Absyn_Format_att: 0){ goto _LL344;} else{ goto _LL345;} _LL345: goto
_LL346; _LL340: goto _LL342; _LL342: goto _LL344; _LL344: continue; _LL346:
_temp336=({ struct Cyc_List_List* _temp347=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp347->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( _temp294))->hd); _temp347->tl= _temp336; _temp347;}); goto _LL338;
_LL338:;}{ struct Cyc_List_List* _temp348=(( struct Cyc_List_List*(*)( struct
_tuple1*(* f)( struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_arg_to_c,
_temp300); if( _temp296 != 0){ void* _temp349= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ((
void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp296))->type,( void*)((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp296))->rgn, Cyc_Toc_mt_tq));
struct _tuple1* _temp350=({ struct _tuple1* _temp352=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp352->f1=(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp296))->name; _temp352->f2=(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp296))->tq; _temp352->f3= _temp349; _temp352;}); _temp348=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp348,({ struct Cyc_List_List* _temp351=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp351->hd=( void*) _temp350; _temp351->tl= 0;
_temp351;}));} return( void*)({ struct Cyc_Absyn_FnType_struct* _temp353=(
struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp353[ 0]=({ struct Cyc_Absyn_FnType_struct _temp354; _temp354.tag= Cyc_Absyn_FnType;
_temp354.f1=({ struct Cyc_Absyn_FnInfo _temp355; _temp355.tvars= 0; _temp355.effect=
0; _temp355.ret_typ=( void*) Cyc_Toc_typ_to_c( _temp302); _temp355.args=
_temp348; _temp355.c_varargs= _temp298; _temp355.cyc_varargs= 0; _temp355.rgn_po=
0; _temp355.attributes= _temp336; _temp355;}); _temp354;}); _temp353;});}}
_LL234: _temp304=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct
_tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f, _temp304);{
struct _tagged_arr* _temp356= Cyc_Toc_add_tuple_type( _temp304); return Cyc_Absyn_strct(
_temp356);} _LL236: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp357=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp357[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp358; _temp358.tag= Cyc_Absyn_AnonStructType; _temp358.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_structfield_to_c, _temp306); _temp358;}); _temp357;});
_LL238: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp359=(
struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp359[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp360; _temp360.tag=
Cyc_Absyn_AnonUnionType; _temp360.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp308); _temp360;}); _temp359;}); _LL240: if( _temp310 == 0){({ void*
_temp361[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("anonymous union", sizeof( unsigned char), 16u), _tag_arr( _temp361,
sizeof( void*), 0u));});} return Cyc_Absyn_unionq_typ(( struct _tuple0*)
_check_null( _temp310)); _LL242: if( _temp312 == 0){({ void* _temp362[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("anonymous struct",
sizeof( unsigned char), 17u), _tag_arr( _temp362, sizeof( void*), 0u));});}
return Cyc_Absyn_strctq(( struct _tuple0*) _check_null( _temp312)); _LL244:
return t; _LL246: if( _temp316 == 0){ return({ void* _temp363[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("Toc::typ_to_c: unresolved TypedefType", sizeof( unsigned char), 38u),
_tag_arr( _temp363, sizeof( void*), 0u));});} return( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp364=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp364[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp365; _temp365.tag= Cyc_Absyn_TypedefType;
_temp365.f1= _temp320; _temp365.f2= 0; _temp365.f3=({ struct Cyc_Core_Opt*
_temp366=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp366->v=( void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp316))->v); _temp366;}); _temp365;}); _temp364;}); _LL248:
return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp), Cyc_Toc_mt_tq);
_LL250: return({ void* _temp367[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the heap region",
sizeof( unsigned char), 55u), _tag_arr( _temp367, sizeof( void*), 0u));});
_LL252: return({ void* _temp368[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the access eff",
sizeof( unsigned char), 54u), _tag_arr( _temp368, sizeof( void*), 0u));});
_LL254: return({ void* _temp369[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the join eff",
sizeof( unsigned char), 52u), _tag_arr( _temp369, sizeof( void*), 0u));});
_LL256: return({ void* _temp370[ 0u]={}; Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the regions eff",
sizeof( unsigned char), 55u), _tag_arr( _temp370, sizeof( void*), 0u));});
_LL204:;} static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast( void* t,
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* l){ void* _temp371= t;
struct Cyc_Absyn_Tqual _temp377; void* _temp379; _LL373: if(( unsigned int)
_temp371 > 4u?*(( int*) _temp371) == Cyc_Absyn_ArrayType: 0){ _LL380: _temp379=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp371)->f1; goto _LL378; _LL378:
_temp377=(( struct Cyc_Absyn_ArrayType_struct*) _temp371)->f2; goto _LL374;}
else{ goto _LL375;} _LL375: goto _LL376; _LL374: return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ(
_temp379,( void*) Cyc_Absyn_HeapRgn, _temp377), e, l); _LL376: return Cyc_Absyn_cast_exp(
t, e, l); _LL372:;} static int Cyc_Toc_atomic_typ( void* t){ void* _temp381= Cyc_Tcutil_compress(
t); void* _temp419; struct Cyc_Absyn_Structdecl** _temp421; struct Cyc_List_List*
_temp423; struct Cyc_List_List* _temp425; struct Cyc_Absyn_TunionFieldInfo
_temp427; void* _temp429; struct Cyc_Absyn_Tunionfield* _temp431; struct Cyc_Absyn_Tuniondecl*
_temp433; struct Cyc_List_List* _temp435; _LL383: if( _temp381 ==( void*) Cyc_Absyn_VoidType){
goto _LL384;} else{ goto _LL385;} _LL385: if(( unsigned int) _temp381 > 4u?*((
int*) _temp381) == Cyc_Absyn_VarType: 0){ goto _LL386;} else{ goto _LL387;}
_LL387: if(( unsigned int) _temp381 > 4u?*(( int*) _temp381) == Cyc_Absyn_IntType:
0){ goto _LL388;} else{ goto _LL389;} _LL389: if(( unsigned int) _temp381 > 4u?*((
int*) _temp381) == Cyc_Absyn_EnumType: 0){ goto _LL390;} else{ goto _LL391;}
_LL391: if( _temp381 ==( void*) Cyc_Absyn_FloatType){ goto _LL392;} else{ goto
_LL393;} _LL393: if( _temp381 ==( void*) Cyc_Absyn_DoubleType){ goto _LL394;}
else{ goto _LL395;} _LL395: if(( unsigned int) _temp381 > 4u?*(( int*) _temp381)
== Cyc_Absyn_FnType: 0){ goto _LL396;} else{ goto _LL397;} _LL397: if((
unsigned int) _temp381 > 4u?*(( int*) _temp381) == Cyc_Absyn_ArrayType: 0){
_LL420: _temp419=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp381)->f1;
goto _LL398;} else{ goto _LL399;} _LL399: if(( unsigned int) _temp381 > 4u?*((
int*) _temp381) == Cyc_Absyn_StructType: 0){ _LL422: _temp421=(( struct Cyc_Absyn_StructType_struct*)
_temp381)->f3; goto _LL400;} else{ goto _LL401;} _LL401: if(( unsigned int)
_temp381 > 4u?*(( int*) _temp381) == Cyc_Absyn_AnonStructType: 0){ _LL424:
_temp423=(( struct Cyc_Absyn_AnonStructType_struct*) _temp381)->f1; goto _LL402;}
else{ goto _LL403;} _LL403: if(( unsigned int) _temp381 > 4u?*(( int*) _temp381)
== Cyc_Absyn_AnonUnionType: 0){ _LL426: _temp425=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp381)->f1; goto _LL404;} else{ goto _LL405;} _LL405: if(( unsigned int)
_temp381 > 4u?*(( int*) _temp381) == Cyc_Absyn_TunionFieldType: 0){ _LL428:
_temp427=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp381)->f1; _LL430:
_temp429=( void*) _temp427.field_info; if(*(( int*) _temp429) == Cyc_Absyn_KnownTunionfield){
_LL434: _temp433=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp429)->f1;
goto _LL432; _LL432: _temp431=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp429)->f2; goto _LL406;} else{ goto _LL407;}} else{ goto _LL407;} _LL407:
if(( unsigned int) _temp381 > 4u?*(( int*) _temp381) == Cyc_Absyn_TupleType: 0){
_LL436: _temp435=(( struct Cyc_Absyn_TupleType_struct*) _temp381)->f1; goto
_LL408;} else{ goto _LL409;} _LL409: if(( unsigned int) _temp381 > 4u?*(( int*)
_temp381) == Cyc_Absyn_TunionType: 0){ goto _LL410;} else{ goto _LL411;} _LL411:
if(( unsigned int) _temp381 > 4u?*(( int*) _temp381) == Cyc_Absyn_UnionType: 0){
goto _LL412;} else{ goto _LL413;} _LL413: if(( unsigned int) _temp381 > 4u?*((
int*) _temp381) == Cyc_Absyn_PointerType: 0){ goto _LL414;} else{ goto _LL415;}
_LL415: if(( unsigned int) _temp381 > 4u?*(( int*) _temp381) == Cyc_Absyn_RgnHandleType:
0){ goto _LL416;} else{ goto _LL417;} _LL417: goto _LL418; _LL384: return 1;
_LL386: return 0; _LL388: return 1; _LL390: return 1; _LL392: return 1; _LL394:
return 1; _LL396: return 1; _LL398: return Cyc_Toc_atomic_typ( _temp419); _LL400:
if( _temp421 == 0? 1:(*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp421)))->fields
== 0){ return 0;}{ struct Cyc_List_List* _temp437=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp421)))->fields))->v; for( 0; _temp437 != 0; _temp437=(( struct
Cyc_List_List*) _check_null( _temp437))->tl){ if( ! Cyc_Toc_atomic_typ(( void*)((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp437))->hd)->type)){
return 0;}}} return 1; _LL402: _temp425= _temp423; goto _LL404; _LL404: for( 0;
_temp425 != 0; _temp425=(( struct Cyc_List_List*) _check_null( _temp425))->tl){
if( ! Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp425))->hd)->type)){ return 0;}} return 1; _LL406: _temp435=
_temp431->typs; goto _LL408; _LL408: for( 0; _temp435 != 0; _temp435=(( struct
Cyc_List_List*) _check_null( _temp435))->tl){ if( ! Cyc_Toc_atomic_typ((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp435))->hd)).f2)){
return 0;}} return 1; _LL410: return 0; _LL412: return 0; _LL414: return 0;
_LL416: return 0; _LL418:({ struct Cyc_Stdio_String_pa_struct _temp439; _temp439.tag=
Cyc_Stdio_String_pa; _temp439.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp438[ 1u]={& _temp439};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("atomic_typ:  bad type %s",
sizeof( unsigned char), 25u), _tag_arr( _temp438, sizeof( void*), 1u));}});
return 0; _LL382:;} static int Cyc_Toc_is_void_star( void* t){ void* _temp440=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp446; void* _temp448;
_LL442: if(( unsigned int) _temp440 > 4u?*(( int*) _temp440) == Cyc_Absyn_PointerType:
0){ _LL447: _temp446=(( struct Cyc_Absyn_PointerType_struct*) _temp440)->f1;
_LL449: _temp448=( void*) _temp446.elt_typ; goto _LL443;} else{ goto _LL444;}
_LL444: goto _LL445; _LL443: { void* _temp450= Cyc_Tcutil_compress( _temp448);
_LL452: if( _temp450 ==( void*) Cyc_Absyn_VoidType){ goto _LL453;} else{ goto
_LL454;} _LL454: goto _LL455; _LL453: return 1; _LL455: return 0; _LL451:;}
_LL445: return 0; _LL441:;} static int Cyc_Toc_is_poly_field( void* t, struct
_tagged_arr* f){ void* _temp456= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp468; struct Cyc_Absyn_Structdecl* _temp470; struct Cyc_List_List* _temp471;
_LL458: if(( unsigned int) _temp456 > 4u?*(( int*) _temp456) == Cyc_Absyn_StructType:
0){ _LL469: _temp468=(( struct Cyc_Absyn_StructType_struct*) _temp456)->f3; if(
_temp468 == 0){ goto _LL460;} else{ _temp470=* _temp468; goto _LL459;}} else{
goto _LL460;} _LL460: if(( unsigned int) _temp456 > 4u?*(( int*) _temp456) ==
Cyc_Absyn_AnonStructType: 0){ _LL472: _temp471=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp456)->f1; goto _LL461;} else{ goto _LL462;} _LL462: if(( unsigned int)
_temp456 > 4u?*(( int*) _temp456) == Cyc_Absyn_AnonUnionType: 0){ goto _LL463;}
else{ goto _LL464;} _LL464: if(( unsigned int) _temp456 > 4u?*(( int*) _temp456)
== Cyc_Absyn_UnionType: 0){ goto _LL465;} else{ goto _LL466;} _LL466: goto
_LL467; _LL459: if( _temp470->fields == 0){( int) _throw(({ void* _temp473[ 0u]={};
Cyc_Toc_toc_impos( _tag_arr("is_poly_field: struct missing fields", sizeof(
unsigned char), 37u), _tag_arr( _temp473, sizeof( void*), 0u));}));} _temp471=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp470->fields))->v;
goto _LL461; _LL461: { struct Cyc_Absyn_Structfield* _temp474= Cyc_Absyn_lookup_field(
_temp471, f); if( _temp474 == 0){({ struct Cyc_Stdio_String_pa_struct _temp476;
_temp476.tag= Cyc_Stdio_String_pa; _temp476.f1=( struct _tagged_arr)*(( struct
Cyc_Absyn_Structfield*) _check_null( _temp474))->name;{ void* _temp475[ 1u]={&
_temp476};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("is_poly_field: bad field %s", sizeof( unsigned char), 28u), _tag_arr(
_temp475, sizeof( void*), 1u));}});} return Cyc_Toc_is_void_star(( void*)((
struct Cyc_Absyn_Structfield*) _check_null( _temp474))->type);} _LL463: return 0;
_LL465: return 0; _LL467:( int) _throw(({ struct Cyc_Stdio_String_pa_struct
_temp478; _temp478.tag= Cyc_Stdio_String_pa; _temp478.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp477[ 1u]={& _temp478}; Cyc_Toc_toc_impos(
_tag_arr("is_poly_field: bad type %s", sizeof( unsigned char), 27u), _tag_arr(
_temp477, sizeof( void*), 1u));}})); _LL457:;} static int Cyc_Toc_is_poly_project(
struct Cyc_Absyn_Exp* e){ void* _temp479=( void*) e->r; struct _tagged_arr*
_temp487; struct Cyc_Absyn_Exp* _temp489; struct _tagged_arr* _temp491; struct
Cyc_Absyn_Exp* _temp493; _LL481: if(*(( int*) _temp479) == Cyc_Absyn_StructMember_e){
_LL490: _temp489=(( struct Cyc_Absyn_StructMember_e_struct*) _temp479)->f1; goto
_LL488; _LL488: _temp487=(( struct Cyc_Absyn_StructMember_e_struct*) _temp479)->f2;
goto _LL482;} else{ goto _LL483;} _LL483: if(*(( int*) _temp479) == Cyc_Absyn_StructArrow_e){
_LL494: _temp493=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp479)->f1; goto
_LL492; _LL492: _temp491=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp479)->f2;
goto _LL484;} else{ goto _LL485;} _LL485: goto _LL486; _LL482: return Cyc_Toc_is_poly_field((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp489->topt))->v, _temp487);
_LL484: { void* _temp495= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp493->topt))->v); struct Cyc_Absyn_PtrInfo _temp501; void*
_temp503; _LL497: if(( unsigned int) _temp495 > 4u?*(( int*) _temp495) == Cyc_Absyn_PointerType:
0){ _LL502: _temp501=(( struct Cyc_Absyn_PointerType_struct*) _temp495)->f1;
_LL504: _temp503=( void*) _temp501.elt_typ; goto _LL498;} else{ goto _LL499;}
_LL499: goto _LL500; _LL498: return Cyc_Toc_is_poly_field( _temp503, _temp491);
_LL500:({ struct Cyc_Stdio_String_pa_struct _temp506; _temp506.tag= Cyc_Stdio_String_pa;
_temp506.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp493->topt))->v);{ void* _temp505[ 1u]={& _temp506};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("is_poly_project:  bad type %s",
sizeof( unsigned char), 30u), _tag_arr( _temp505, sizeof( void*), 1u));}});
return 0; _LL496:;} _LL486: return 0; _LL480:;} static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Toc_GC_malloc_e,({
struct Cyc_List_List* _temp507=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp507->hd=( void*) s; _temp507->tl= 0; _temp507;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Toc_GC_malloc_atomic_e,({ struct Cyc_List_List*
_temp508=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp508->hd=( void*) s; _temp508->tl= 0; _temp508;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp* s){ if( Cyc_Toc_atomic_typ( t)){
return Cyc_Toc_malloc_atomic( s);} return Cyc_Toc_malloc_ptr( s);} static struct
Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp( struct Cyc_Absyn_Exp* rgn, struct Cyc_Absyn_Exp*
s){ return Cyc_Absyn_fncall_exp( Cyc_Toc__region_malloc_e,({ struct Cyc_Absyn_Exp*
_temp509[ 2u]; _temp509[ 1u]= s; _temp509[ 0u]= rgn;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp509, sizeof( struct Cyc_Absyn_Exp*),
2u));}), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp*
e){ return Cyc_Absyn_fncall_exp( Cyc_Toc__throw_e,({ struct Cyc_List_List*
_temp510=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp510->hd=( void*) e; _temp510->tl= 0; _temp510;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_make_toplevel_tagged_arr( void* t, struct Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp*
e){ int is_string= 0;{ void* _temp511=( void*) e->r; void* _temp517; _LL513: if(*((
int*) _temp511) == Cyc_Absyn_Const_e){ _LL518: _temp517=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp511)->f1; if(( unsigned int) _temp517 > 1u?*(( int*) _temp517) == Cyc_Absyn_String_c:
0){ goto _LL514;} else{ goto _LL515;}} else{ goto _LL515;} _LL515: goto _LL516;
_LL514: is_string= 1; goto _LL512; _LL516: goto _LL512; _LL512:;}{ struct Cyc_Absyn_Exp*
xexp; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var(); void* vd_typ=( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp522=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp522[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp523; _temp523.tag= Cyc_Absyn_ArrayType; _temp523.f1=(
void*) Cyc_Absyn_uchar_t; _temp523.f2= Cyc_Toc_mt_tq; _temp523.f3=( struct Cyc_Absyn_Exp*)
sz; _temp523;}); _temp522;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp519=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp519->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp520=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp520[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp521; _temp521.tag= Cyc_Absyn_Var_d;
_temp521.f1= vd; _temp521;}); _temp520;}), 0); _temp519->tl= Cyc_Toc_result_decls;
_temp519;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ xexp= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), e, 0);}{ struct Cyc_Absyn_Exp* xplussz= Cyc_Absyn_add_exp(
xexp, sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp( 0,({
struct _tuple4* _temp524[ 3u]; _temp524[ 2u]=({ struct _tuple4* _temp527=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp527->f1= 0; _temp527->f2=
xplussz; _temp527;}); _temp524[ 1u]=({ struct _tuple4* _temp526=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp526->f1= 0; _temp526->f2= xexp;
_temp526;}); _temp524[ 0u]=({ struct _tuple4* _temp525=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp525->f1= 0; _temp525->f2= xexp;
_temp525;});(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp524, sizeof( struct _tuple4*), 3u));}), 0); return urm_exp;}}}
struct Cyc_Toc_Env{ struct Cyc_Core_Opt* break_lab; struct Cyc_Core_Opt*
continue_lab; struct Cyc_Core_Opt* fallthru_info; struct Cyc_Dict_Dict* varmap;
int toplevel; } ; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){ return({
struct Cyc_Toc_Env* _temp528=( struct Cyc_Toc_Env*) GC_malloc( sizeof( struct
Cyc_Toc_Env)); _temp528->break_lab= 0; _temp528->continue_lab= 0; _temp528->fallthru_info=
0; _temp528->varmap=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp528->toplevel= 1;
_temp528;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){
return({ struct Cyc_Toc_Env* _temp529=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp529->break_lab= e->break_lab; _temp529->continue_lab=
e->continue_lab; _temp529->fallthru_info= e->fallthru_info; _temp529->varmap= e->varmap;
_temp529->toplevel= e->toplevel; _temp529;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp530=(* x).f1; _LL532:
if(( unsigned int) _temp530 > 1u?*(( int*) _temp530) == Cyc_Absyn_Rel_n: 0){
goto _LL533;} else{ goto _LL534;} _LL534: goto _LL535; _LL533:( int) _throw(({
struct Cyc_Stdio_String_pa_struct _temp537; _temp537.tag= Cyc_Stdio_String_pa;
_temp537.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( x);{ void* _temp536[
1u]={& _temp537}; Cyc_Toc_toc_impos( _tag_arr("Toc::add_varmap on Rel_n: %s\n",
sizeof( unsigned char), 30u), _tag_arr( _temp536, sizeof( void*), 1u));}}));
_LL535: goto _LL531; _LL531:;}{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->varmap=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0*
key, struct Cyc_Absyn_Exp* data)) Cyc_Dict_insert)( ans->varmap, x, y); return
ans;}} static struct Cyc_Toc_Env* Cyc_Toc_loop_env( struct Cyc_Toc_Env* e){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->continue_lab=
0; return ans;} struct _tuple6{ struct _tagged_arr* f1; struct Cyc_List_List* f2;
struct Cyc_Dict_Dict* f3; } ; static struct Cyc_Toc_Env* Cyc_Toc_non_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_arr* break_l, struct _tagged_arr*
fallthru_l, struct Cyc_List_List* fallthru_binders, struct Cyc_Toc_Env*
next_case_env){ struct Cyc_List_List* _temp538= 0; for( 0; fallthru_binders != 0;
fallthru_binders=(( struct Cyc_List_List*) _check_null( fallthru_binders))->tl){
_temp538=({ struct Cyc_List_List* _temp539=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp539->hd=( void*)(( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( fallthru_binders))->hd)->name; _temp539->tl=
_temp538; _temp539;});} _temp538=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp538);{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab=({ struct Cyc_Core_Opt* _temp540=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp540->v=( void*) break_l; _temp540;});
ans->fallthru_info=({ struct Cyc_Core_Opt* _temp541=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp541->v=( void*)({ struct _tuple6*
_temp542=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp542->f1=
fallthru_l; _temp542->f2= _temp538; _temp542->f3= next_case_env->varmap;
_temp542;}); _temp541;}); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_arr* break_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt* _temp543=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp543->v=( void*)
break_l; _temp543;}); ans->fallthru_info= 0; return ans;} static struct Cyc_Toc_Env*
Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env* e, struct _tagged_arr* next_l){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->fallthru_info=({
struct Cyc_Core_Opt* _temp544=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp544->v=( void*)({ struct _tuple6* _temp545=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp545->f1= next_l; _temp545->f2= 0;
_temp545->f3=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct
_tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp545;}); _temp544;});
return ans;} static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e); static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt*
s); static void* Cyc_Toc_get_c_typ( struct Cyc_Absyn_Exp* e){ if( e->topt == 0){({
void* _temp546[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("Missing type in primop ", sizeof( unsigned char),
24u), _tag_arr( _temp546, sizeof( void*), 0u));});} return Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);} static void* Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp* e){ if( e->topt == 0){({ void* _temp547[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("Missing type in primop ",
sizeof( unsigned char), 24u), _tag_arr( _temp547, sizeof( void*), 0u));});}
return( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v;} static struct
_tuple3* Cyc_Toc_tup_to_c( struct Cyc_Absyn_Exp* e){ return({ struct _tuple3*
_temp548=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp548->f1=
Cyc_Toc_mt_tq; _temp548->f2= Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); _temp548;});} static struct _tuple4* Cyc_Toc_add_designator(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e);
return({ struct _tuple4* _temp549=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp549->f1= 0; _temp549->f2= e; _temp549;});} static struct Cyc_Absyn_Exp*
Cyc_Toc_make_struct( struct Cyc_Toc_Env* nv, struct _tuple0* x, void* struct_typ,
struct Cyc_Absyn_Stmt* s, int pointer, struct Cyc_Absyn_Exp* rgnopt, int
is_atomic){ struct Cyc_Absyn_Exp* eo; void* t; if( pointer){ t= Cyc_Absyn_cstar_typ(
struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp* _temp550= Cyc_Absyn_sizeoftyp_exp(
struct_typ, 0); if( rgnopt == 0){ eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp(
t, is_atomic? Cyc_Toc_malloc_atomic( _temp550): Cyc_Toc_malloc_ptr( _temp550), 0);}
else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( rgnopt); Cyc_Toc_exp_to_c(
nv, r); eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp(
r, _temp550), 0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) - 1;{ struct Cyc_List_List* _temp551=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( dles0); for( 0; _temp551 != 0; _temp551=((
struct Cyc_List_List*) _check_null( _temp551))->tl){ struct _tuple4 _temp554;
struct Cyc_Absyn_Exp* _temp555; struct Cyc_List_List* _temp557; struct _tuple4*
_temp552=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp551))->hd;
_temp554=* _temp552; _LL558: _temp557= _temp554.f1; goto _LL556; _LL556:
_temp555= _temp554.f2; goto _LL553; _LL553: { struct Cyc_Absyn_Exp* e_index; if(
_temp557 == 0){ e_index= Cyc_Absyn_signed_int_exp( count --, 0);} else{ if(((
struct Cyc_List_List*) _check_null( _temp557))->tl != 0){({ void* _temp559[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("multiple designators in array",
sizeof( unsigned char), 30u), _tag_arr( _temp559, sizeof( void*), 0u));});}{
void* _temp560=( void*)(( struct Cyc_List_List*) _check_null( _temp557))->hd;
void* _temp561= _temp560; struct Cyc_Absyn_Exp* _temp567; _LL563: if(*(( int*)
_temp561) == Cyc_Absyn_ArrayElement){ _LL568: _temp567=(( struct Cyc_Absyn_ArrayElement_struct*)
_temp561)->f1; goto _LL564;} else{ goto _LL565;} _LL565: if(*(( int*) _temp561)
== Cyc_Absyn_FieldName){ goto _LL566;} else{ goto _LL562;} _LL564: Cyc_Toc_exp_to_c(
nv, _temp567); e_index= _temp567; goto _LL562; _LL566: e_index=({ void* _temp569[
0u]={};(( struct Cyc_Absyn_Exp*(*)( struct _tagged_arr fmt, struct _tagged_arr
ap)) Cyc_Toc_unimp)( _tag_arr("field name designators in array", sizeof(
unsigned char), 32u), _tag_arr( _temp569, sizeof( void*), 0u));}); goto _LL562;
_LL562:;}}{ struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp( lhs, e_index, 0);
void* _temp570=( void*) _temp555->r; struct Cyc_List_List* _temp580; struct Cyc_Absyn_Exp*
_temp582; struct Cyc_Absyn_Exp* _temp584; struct Cyc_Absyn_Vardecl* _temp586;
struct Cyc_List_List* _temp588; void* _temp590; _LL572: if(*(( int*) _temp570)
== Cyc_Absyn_Array_e){ _LL581: _temp580=(( struct Cyc_Absyn_Array_e_struct*)
_temp570)->f1; goto _LL573;} else{ goto _LL574;} _LL574: if(*(( int*) _temp570)
== Cyc_Absyn_Comprehension_e){ _LL587: _temp586=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp570)->f1; goto _LL585; _LL585: _temp584=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp570)->f2; goto _LL583; _LL583: _temp582=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp570)->f3; goto _LL575;} else{ goto _LL576;} _LL576: if(*(( int*) _temp570)
== Cyc_Absyn_AnonStruct_e){ _LL591: _temp590=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp570)->f1; goto _LL589; _LL589: _temp588=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp570)->f2; goto _LL577;} else{ goto _LL578;} _LL578: goto _LL579; _LL573: s=
Cyc_Toc_init_array( nv, lval, _temp580, s); goto _LL571; _LL575: s= Cyc_Toc_init_comprehension(
nv, lval, _temp586, _temp584, _temp582, s, 0); goto _LL571; _LL577: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp590, _temp588, s); goto _LL571; _LL579: Cyc_Toc_exp_to_c( nv,
_temp555); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
lhs, e_index, 0), _temp555, 0), s, 0); goto _LL571; _LL571:;}}}} return s;}
static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s, int
e1_already_translated){ struct _tuple0* _temp592= vd->name; if( !
e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env* nv2=
Cyc_Toc_add_varmap( nv, _temp592, Cyc_Absyn_varb_exp( _temp592,( void*)({ struct
Cyc_Absyn_Local_b_struct* _temp615=( struct Cyc_Absyn_Local_b_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Local_b_struct)); _temp615[ 0]=({ struct Cyc_Absyn_Local_b_struct
_temp616; _temp616.tag= Cyc_Absyn_Local_b; _temp616.f1= vd; _temp616;});
_temp615;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( _temp592, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp(
_temp592, 0), Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp(
Cyc_Absyn_var_exp( _temp592, 0), 0); struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp(
lhs, Cyc_Absyn_var_exp( _temp592, 0), 0); struct Cyc_Absyn_Stmt* body;{ void*
_temp593=( void*) e2->r; struct Cyc_List_List* _temp603; struct Cyc_Absyn_Exp*
_temp605; struct Cyc_Absyn_Exp* _temp607; struct Cyc_Absyn_Vardecl* _temp609;
struct Cyc_List_List* _temp611; void* _temp613; _LL595: if(*(( int*) _temp593)
== Cyc_Absyn_Array_e){ _LL604: _temp603=(( struct Cyc_Absyn_Array_e_struct*)
_temp593)->f1; goto _LL596;} else{ goto _LL597;} _LL597: if(*(( int*) _temp593)
== Cyc_Absyn_Comprehension_e){ _LL610: _temp609=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp593)->f1; goto _LL608; _LL608: _temp607=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp593)->f2; goto _LL606; _LL606: _temp605=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp593)->f3; goto _LL598;} else{ goto _LL599;} _LL599: if(*(( int*) _temp593)
== Cyc_Absyn_AnonStruct_e){ _LL614: _temp613=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp593)->f1; goto _LL612; _LL612: _temp611=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp593)->f2; goto _LL600;} else{ goto _LL601;} _LL601: goto _LL602; _LL596:
body= Cyc_Toc_init_array( nv2, lval, _temp603, Cyc_Absyn_skip_stmt( 0)); goto
_LL594; _LL598: body= Cyc_Toc_init_comprehension( nv2, lval, _temp609, _temp607,
_temp605, Cyc_Absyn_skip_stmt( 0), 0); goto _LL594; _LL600: body= Cyc_Toc_init_anon_struct(
nv, lval, _temp613, _temp611, Cyc_Absyn_skip_stmt( 0)); goto _LL594; _LL602: Cyc_Toc_exp_to_c(
nv2, e2); body= Cyc_Absyn_assign_stmt( lval, e2, 0); goto _LL594; _LL594:;}{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt( ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt(
_temp592, Cyc_Absyn_uint_t, 0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_anon_struct( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs,
void* struct_type, struct Cyc_List_List* dles, struct Cyc_Absyn_Stmt* s){{
struct Cyc_List_List* _temp617=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( dles); for( 0; _temp617 != 0; _temp617=(( struct Cyc_List_List*)
_check_null( _temp617))->tl){ struct _tuple4 _temp620; struct Cyc_Absyn_Exp*
_temp621; struct Cyc_List_List* _temp623; struct _tuple4* _temp618=( struct
_tuple4*)(( struct Cyc_List_List*) _check_null( _temp617))->hd; _temp620=*
_temp618; _LL624: _temp623= _temp620.f1; goto _LL622; _LL622: _temp621= _temp620.f2;
goto _LL619; _LL619: if( _temp623 == 0){({ void* _temp625[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("empty designator list",
sizeof( unsigned char), 22u), _tag_arr( _temp625, sizeof( void*), 0u));});} if(((
struct Cyc_List_List*) _check_null( _temp623))->tl != 0){({ void* _temp626[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("too many designators in anonymous struct", sizeof( unsigned char), 41u),
_tag_arr( _temp626, sizeof( void*), 0u));});}{ void* _temp627=( void*)(( struct
Cyc_List_List*) _check_null( _temp623))->hd; struct _tagged_arr* _temp633;
_LL629: if(*(( int*) _temp627) == Cyc_Absyn_FieldName){ _LL634: _temp633=((
struct Cyc_Absyn_FieldName_struct*) _temp627)->f1; goto _LL630;} else{ goto
_LL631;} _LL631: goto _LL632; _LL630: { struct Cyc_Absyn_Exp* lval= Cyc_Absyn_structmember_exp(
lhs, _temp633, 0);{ void* _temp635=( void*) _temp621->r; struct Cyc_List_List*
_temp645; struct Cyc_Absyn_Exp* _temp647; struct Cyc_Absyn_Exp* _temp649; struct
Cyc_Absyn_Vardecl* _temp651; struct Cyc_List_List* _temp653; void* _temp655;
_LL637: if(*(( int*) _temp635) == Cyc_Absyn_Array_e){ _LL646: _temp645=(( struct
Cyc_Absyn_Array_e_struct*) _temp635)->f1; goto _LL638;} else{ goto _LL639;}
_LL639: if(*(( int*) _temp635) == Cyc_Absyn_Comprehension_e){ _LL652: _temp651=((
struct Cyc_Absyn_Comprehension_e_struct*) _temp635)->f1; goto _LL650; _LL650:
_temp649=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp635)->f2; goto _LL648;
_LL648: _temp647=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp635)->f3;
goto _LL640;} else{ goto _LL641;} _LL641: if(*(( int*) _temp635) == Cyc_Absyn_AnonStruct_e){
_LL656: _temp655=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp635)->f1;
goto _LL654; _LL654: _temp653=(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp635)->f2;
goto _LL642;} else{ goto _LL643;} _LL643: goto _LL644; _LL638: s= Cyc_Toc_init_array(
nv, lval, _temp645, s); goto _LL636; _LL640: s= Cyc_Toc_init_comprehension( nv,
lval, _temp651, _temp649, _temp647, s, 0); goto _LL636; _LL642: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp655, _temp653, s); goto _LL636; _LL644: Cyc_Toc_exp_to_c( nv,
_temp621); if( Cyc_Toc_is_poly_field( struct_type, _temp633)){ _temp621= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp621, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp621, 0), 0), s, 0); goto _LL636; _LL636:;} goto
_LL628;} _LL632:( int) _throw(({ void* _temp657[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u), _tag_arr(
_temp657, sizeof( void*), 0u));})); _LL628:;}}} return s;} static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple( struct Cyc_Toc_Env* nv, int pointer, struct Cyc_Absyn_Exp*
rgnopt, struct Cyc_List_List* es){ struct _RegionHandle _temp658= _new_region();
struct _RegionHandle* r=& _temp658; _push_region( r);{ struct Cyc_List_List*
_temp659=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple3*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_rmap)( r, Cyc_Toc_tup_to_c,
es); struct _tagged_arr* _temp660= Cyc_Toc_add_tuple_type( _temp659); struct
_tuple0* _temp661= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp662= Cyc_Absyn_var_exp(
_temp661, 0); struct Cyc_Absyn_Stmt* _temp663= Cyc_Absyn_exp_stmt( _temp662, 0);
struct Cyc_Absyn_Exp*(* _temp664)( struct Cyc_Absyn_Exp*, struct _tagged_arr*,
struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp;
int is_atomic= 1; struct Cyc_List_List* _temp665=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, es);{ int i=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp665); for( 0; _temp665
!= 0;( _temp665=(( struct Cyc_List_List*) _check_null( _temp665))->tl, -- i)){
struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp665))->hd; struct Cyc_Absyn_Exp* lval= _temp664( _temp662, Cyc_Toc_fieldname(
i), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v): 0;{ void* _temp666=( void*) e->r; struct Cyc_List_List*
_temp674; struct Cyc_Absyn_Exp* _temp676; struct Cyc_Absyn_Exp* _temp678; struct
Cyc_Absyn_Vardecl* _temp680; _LL668: if(*(( int*) _temp666) == Cyc_Absyn_Array_e){
_LL675: _temp674=(( struct Cyc_Absyn_Array_e_struct*) _temp666)->f1; goto _LL669;}
else{ goto _LL670;} _LL670: if(*(( int*) _temp666) == Cyc_Absyn_Comprehension_e){
_LL681: _temp680=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp666)->f1;
goto _LL679; _LL679: _temp678=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp666)->f2; goto _LL677; _LL677: _temp676=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp666)->f3; goto _LL671;} else{ goto _LL672;} _LL672: goto _LL673; _LL669:
_temp663= Cyc_Toc_init_array( nv, lval, _temp674, _temp663); goto _LL667; _LL671:
_temp663= Cyc_Toc_init_comprehension( nv, lval, _temp680, _temp678, _temp676,
_temp663, 0); goto _LL667; _LL673: Cyc_Toc_exp_to_c( nv, e); _temp663= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp664( _temp662, Cyc_Toc_fieldname(
i), 0), e, 0), 0), _temp663, 0); goto _LL667; _LL667:;}}}{ struct Cyc_Absyn_Exp*
_temp682= Cyc_Toc_make_struct( nv, _temp661, Cyc_Absyn_strct( _temp660),
_temp663, pointer, rgnopt, is_atomic); _npop_handler( 0u); return _temp682;}};
_pop_region( r);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env*
nv, void* struct_type, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
dles, struct _tuple0* tdn){ struct _tuple0* _temp683= Cyc_Toc_temp_var(); struct
Cyc_Absyn_Exp* _temp684= Cyc_Absyn_var_exp( _temp683, 0); struct Cyc_Absyn_Stmt*
_temp685= Cyc_Absyn_exp_stmt( _temp684, 0); struct Cyc_Absyn_Exp*(* _temp686)(
struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; int is_atomic= 1;
struct _RegionHandle _temp687= _new_region(); struct _RegionHandle* r=& _temp687;
_push_region( r);{ struct Cyc_List_List* _temp688=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, dles); for(
0; _temp688 != 0; _temp688=(( struct Cyc_List_List*) _check_null( _temp688))->tl){
struct _tuple4 _temp691; struct Cyc_Absyn_Exp* _temp692; struct Cyc_List_List*
_temp694; struct _tuple4* _temp689=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp688))->hd; _temp691=* _temp689; _LL695: _temp694= _temp691.f1;
goto _LL693; _LL693: _temp692= _temp691.f2; goto _LL690; _LL690: is_atomic=
is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp692->topt))->v): 0; if( _temp694 == 0){({ void* _temp696[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("empty designator list",
sizeof( unsigned char), 22u), _tag_arr( _temp696, sizeof( void*), 0u));});} if(((
struct Cyc_List_List*) _check_null( _temp694))->tl != 0){ struct _tuple0*
_temp697= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp698= Cyc_Absyn_var_exp(
_temp697, 0); for( 0; _temp694 != 0; _temp694=(( struct Cyc_List_List*)
_check_null( _temp694))->tl){ void* _temp699=( void*)(( struct Cyc_List_List*)
_check_null( _temp694))->hd; struct _tagged_arr* _temp705; _LL701: if(*(( int*)
_temp699) == Cyc_Absyn_FieldName){ _LL706: _temp705=(( struct Cyc_Absyn_FieldName_struct*)
_temp699)->f1; goto _LL702;} else{ goto _LL703;} _LL703: goto _LL704; _LL702:
if( Cyc_Toc_is_poly_field( struct_type, _temp705)){ _temp698= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp698, 0);} _temp685= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( _temp686( _temp684, _temp705, 0), _temp698, 0), 0),
_temp685, 0); goto _LL700; _LL704:( int) _throw(({ void* _temp707[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u), _tag_arr(
_temp707, sizeof( void*), 0u));})); _LL700:;} Cyc_Toc_exp_to_c( nv, _temp692);
_temp685= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp698,
_temp692, 0), 0), _temp685, 0);} else{ void* _temp708=( void*)(( struct Cyc_List_List*)
_check_null( _temp694))->hd; struct _tagged_arr* _temp714; _LL710: if(*(( int*)
_temp708) == Cyc_Absyn_FieldName){ _LL715: _temp714=(( struct Cyc_Absyn_FieldName_struct*)
_temp708)->f1; goto _LL711;} else{ goto _LL712;} _LL712: goto _LL713; _LL711: {
struct Cyc_Absyn_Exp* lval= _temp686( _temp684, _temp714, 0);{ void* _temp716=(
void*) _temp692->r; struct Cyc_List_List* _temp726; struct Cyc_Absyn_Exp*
_temp728; struct Cyc_Absyn_Exp* _temp730; struct Cyc_Absyn_Vardecl* _temp732;
struct Cyc_List_List* _temp734; void* _temp736; _LL718: if(*(( int*) _temp716)
== Cyc_Absyn_Array_e){ _LL727: _temp726=(( struct Cyc_Absyn_Array_e_struct*)
_temp716)->f1; goto _LL719;} else{ goto _LL720;} _LL720: if(*(( int*) _temp716)
== Cyc_Absyn_Comprehension_e){ _LL733: _temp732=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp716)->f1; goto _LL731; _LL731: _temp730=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp716)->f2; goto _LL729; _LL729: _temp728=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp716)->f3; goto _LL721;} else{ goto _LL722;} _LL722: if(*(( int*) _temp716)
== Cyc_Absyn_AnonStruct_e){ _LL737: _temp736=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp716)->f1; goto _LL735; _LL735: _temp734=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp716)->f2; goto _LL723;} else{ goto _LL724;} _LL724: goto _LL725; _LL719:
_temp685= Cyc_Toc_init_array( nv, lval, _temp726, _temp685); goto _LL717; _LL721:
_temp685= Cyc_Toc_init_comprehension( nv, lval, _temp732, _temp730, _temp728,
_temp685, 0); goto _LL717; _LL723: _temp685= Cyc_Toc_init_anon_struct( nv, lval,
_temp736, _temp734, _temp685); goto _LL717; _LL725: Cyc_Toc_exp_to_c( nv,
_temp692); if( Cyc_Toc_is_poly_field( struct_type, _temp714)){ _temp692= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp692, 0);} _temp685= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp692, 0), 0), _temp685, 0); goto _LL717; _LL717:;}
goto _LL709;} _LL713:( int) _throw(({ void* _temp738[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("array designator in struct", sizeof( unsigned char), 27u), _tag_arr(
_temp738, sizeof( void*), 0u));})); _LL709:;}}}{ struct Cyc_Absyn_Exp* _temp739=
Cyc_Toc_make_struct( nv, _temp683, Cyc_Absyn_strctq( tdn), _temp685, pointer,
rgnopt, is_atomic); _npop_handler( 0u); return _temp739;}; _pop_region( r);}
struct _tuple7{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ; static
struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp* el, struct
_tuple7* pr){ return Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2, 0);} static
struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp* e1, int
ignore){ return Cyc_Absyn_address_exp( e1, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue(
struct Cyc_Absyn_Exp* e1, void* incr){ return Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Increment_e_struct* _temp740=( struct Cyc_Absyn_Increment_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp740[ 0]=({ struct
Cyc_Absyn_Increment_e_struct _temp741; _temp741.tag= Cyc_Absyn_Increment_e;
_temp741.f1= e1; _temp741.f2=( void*) incr; _temp741;}); _temp740;}), 0);}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env);
static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp742=( void*) e1->r; struct Cyc_Absyn_Stmt* _temp752; struct Cyc_Absyn_Exp*
_temp754; void* _temp756; struct _tagged_arr* _temp758; struct Cyc_Absyn_Exp*
_temp760; _LL744: if(*(( int*) _temp742) == Cyc_Absyn_StmtExp_e){ _LL753:
_temp752=(( struct Cyc_Absyn_StmtExp_e_struct*) _temp742)->f1; goto _LL745;}
else{ goto _LL746;} _LL746: if(*(( int*) _temp742) == Cyc_Absyn_Cast_e){ _LL757:
_temp756=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp742)->f1; goto _LL755;
_LL755: _temp754=(( struct Cyc_Absyn_Cast_e_struct*) _temp742)->f2; goto _LL747;}
else{ goto _LL748;} _LL748: if(*(( int*) _temp742) == Cyc_Absyn_StructMember_e){
_LL761: _temp760=(( struct Cyc_Absyn_StructMember_e_struct*) _temp742)->f1; goto
_LL759; _LL759: _temp758=(( struct Cyc_Absyn_StructMember_e_struct*) _temp742)->f2;
goto _LL749;} else{ goto _LL750;} _LL750: goto _LL751; _LL745: Cyc_Toc_lvalue_assign_stmt(
_temp752, fs, f, f_env); goto _LL743; _LL747: Cyc_Toc_lvalue_assign( _temp754,
fs, f, f_env); goto _LL743; _LL749:( void*)( e1->r=( void*)(( void*) _temp760->r));
Cyc_Toc_lvalue_assign( e1,( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp762=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp762->hd=( void*) _temp758; _temp762->tl= fs; _temp762;}), f, f_env); goto
_LL743; _LL751: if( ! Cyc_Absyn_is_lvalue( e1)){({ struct Cyc_Stdio_String_pa_struct
_temp764; _temp764.tag= Cyc_Stdio_String_pa; _temp764.f1=( struct _tagged_arr)
Cyc_Absynpp_exp2string( e1);{ void* _temp763[ 1u]={& _temp764};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("lvalue_assign: could not find lvalue in `%s'",
sizeof( unsigned char), 45u), _tag_arr( _temp763, sizeof( void*), 1u));}});}{
struct Cyc_Absyn_Exp* e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ e1_copy= Cyc_Absyn_structmember_exp(
e1_copy,( struct _tagged_arr*)(( struct Cyc_List_List*) _check_null( fs))->hd,
e1_copy->loc);}( void*)( e1->r=( void*)(( void*)( f( e1_copy, f_env))->r)); goto
_LL743;} _LL743:;} static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env){ void* _temp765=( void*) s->r; struct Cyc_Absyn_Exp*
_temp775; struct Cyc_Absyn_Stmt* _temp777; struct Cyc_Absyn_Decl* _temp779;
struct Cyc_Absyn_Stmt* _temp781; _LL767: if(( unsigned int) _temp765 > 1u?*((
int*) _temp765) == Cyc_Absyn_Exp_s: 0){ _LL776: _temp775=(( struct Cyc_Absyn_Exp_s_struct*)
_temp765)->f1; goto _LL768;} else{ goto _LL769;} _LL769: if(( unsigned int)
_temp765 > 1u?*(( int*) _temp765) == Cyc_Absyn_Decl_s: 0){ _LL780: _temp779=((
struct Cyc_Absyn_Decl_s_struct*) _temp765)->f1; goto _LL778; _LL778: _temp777=((
struct Cyc_Absyn_Decl_s_struct*) _temp765)->f2; goto _LL770;} else{ goto _LL771;}
_LL771: if(( unsigned int) _temp765 > 1u?*(( int*) _temp765) == Cyc_Absyn_Seq_s:
0){ _LL782: _temp781=(( struct Cyc_Absyn_Seq_s_struct*) _temp765)->f2; goto
_LL772;} else{ goto _LL773;} _LL773: goto _LL774; _LL768: Cyc_Toc_lvalue_assign(
_temp775, fs, f, f_env); goto _LL766; _LL770: Cyc_Toc_lvalue_assign_stmt(
_temp777, fs, f, f_env); goto _LL766; _LL772: Cyc_Toc_lvalue_assign_stmt(
_temp781, fs, f, f_env); goto _LL766; _LL774:({ struct Cyc_Stdio_String_pa_struct
_temp784; _temp784.tag= Cyc_Stdio_String_pa; _temp784.f1=( struct _tagged_arr)
Cyc_Absynpp_stmt2string( s);{ void* _temp783[ 1u]={& _temp784};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("lvalue_assign_stmt: %s",
sizeof( unsigned char), 23u), _tag_arr( _temp783, sizeof( void*), 1u));}}); goto
_LL766; _LL766:;} static struct Cyc_List_List* Cyc_Toc_rmap_2c( struct
_RegionHandle* r2, void*(* f)( void*, void*), void* env, struct Cyc_List_List* x){
struct Cyc_List_List* result; struct Cyc_List_List* prev; if( x == 0){ return 0;}
result=({ struct Cyc_List_List* _temp785=( struct Cyc_List_List*) _region_malloc(
r2, sizeof( struct Cyc_List_List)); _temp785->hd=( void*) f(( void*)(( struct
Cyc_List_List*) _check_null( x))->hd, env); _temp785->tl= 0; _temp785;}); prev=
result; for( x=(( struct Cyc_List_List*) _check_null( x))->tl; x != 0; x=((
struct Cyc_List_List*) _check_null( x))->tl){(( struct Cyc_List_List*)
_check_null( prev))->tl=({ struct Cyc_List_List* _temp786=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp786->hd=( void*) f((
void*)(( struct Cyc_List_List*) _check_null( x))->hd, env); _temp786->tl= 0;
_temp786;}); prev=(( struct Cyc_List_List*) _check_null( prev))->tl;} return
result;} static struct Cyc_List_List* Cyc_Toc_map_2c( void*(* f)( void*, void*),
void* env, struct Cyc_List_List* x){ return Cyc_Toc_rmap_2c( Cyc_Core_heap_region,
f, env, x);} static struct _tuple4* Cyc_Toc_make_dle( struct Cyc_Absyn_Exp* e){
return({ struct _tuple4* _temp787=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp787->f1= 0; _temp787->f2= e; _temp787;});} static struct Cyc_Absyn_PtrInfo
Cyc_Toc_get_ptr_type( void* t){ void* _temp788= Cyc_Tcutil_compress( t); struct
Cyc_Absyn_PtrInfo _temp794; _LL790: if(( unsigned int) _temp788 > 4u?*(( int*)
_temp788) == Cyc_Absyn_PointerType: 0){ _LL795: _temp794=(( struct Cyc_Absyn_PointerType_struct*)
_temp788)->f1; goto _LL791;} else{ goto _LL792;} _LL792: goto _LL793; _LL791:
return _temp794; _LL793:( int) _throw(({ void* _temp796[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("get_ptr_typ: not a pointer!", sizeof( unsigned char), 28u), _tag_arr(
_temp796, sizeof( void*), 0u));})); _LL789:;} struct _tuple8{ void* f1; void* f2;
} ; struct _tuple9{ struct _tuple0* f1; void* f2; struct Cyc_Absyn_Exp* f3; } ;
struct _tuple10{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Exp* f2; } ;
static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e){
void* _temp797=( void*) e->r; if( e->topt == 0){({ struct Cyc_Stdio_String_pa_struct
_temp799; _temp799.tag= Cyc_Stdio_String_pa; _temp799.f1=( struct _tagged_arr)
Cyc_Absynpp_exp2string( e);{ void* _temp798[ 1u]={& _temp799};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: no type for %s",
sizeof( unsigned char), 25u), _tag_arr( _temp798, sizeof( void*), 1u));}});
return;}{ void* old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v;
void* _temp800= _temp797; void* _temp880; void* _temp882; struct _tuple0*
_temp884; struct _tuple0* _temp886; struct Cyc_List_List* _temp888; void*
_temp890; void* _temp892; struct Cyc_Absyn_Exp* _temp894; struct Cyc_Absyn_Exp*
_temp896; struct Cyc_Core_Opt* _temp898; struct Cyc_Absyn_Exp* _temp900; struct
Cyc_Absyn_Exp* _temp902; struct Cyc_Absyn_Exp* _temp904; struct Cyc_Absyn_Exp*
_temp906; struct Cyc_Absyn_Exp* _temp908; struct Cyc_Absyn_Exp* _temp910; struct
Cyc_List_List* _temp912; struct Cyc_Absyn_Exp* _temp914; struct Cyc_Absyn_VarargCallInfo*
_temp916; struct Cyc_List_List* _temp918; struct Cyc_Absyn_Exp* _temp920; struct
Cyc_Absyn_VarargCallInfo* _temp922; struct Cyc_Absyn_VarargCallInfo _temp924;
struct Cyc_Absyn_VarargInfo* _temp925; struct Cyc_List_List* _temp927; int
_temp929; struct Cyc_List_List* _temp931; struct Cyc_Absyn_Exp* _temp933; struct
Cyc_Absyn_Exp* _temp935; struct Cyc_Absyn_Exp* _temp937; struct Cyc_List_List*
_temp939; struct Cyc_Absyn_Exp* _temp941; struct Cyc_Absyn_Exp* _temp943; void*
_temp945; void** _temp947; struct Cyc_Absyn_Exp* _temp948; struct Cyc_Absyn_Exp*
_temp950; struct Cyc_Absyn_Exp* _temp952; struct Cyc_Absyn_Exp* _temp954; void*
_temp956; struct _tagged_arr* _temp958; void* _temp960; struct Cyc_Absyn_Exp*
_temp962; struct _tagged_arr* _temp964; struct Cyc_Absyn_Exp* _temp966; struct
_tagged_arr* _temp968; struct Cyc_Absyn_Exp* _temp970; struct Cyc_Absyn_Exp*
_temp972; struct Cyc_Absyn_Exp* _temp974; struct Cyc_List_List* _temp976; struct
Cyc_List_List* _temp978; struct Cyc_Absyn_Exp* _temp980; struct Cyc_Absyn_Exp*
_temp982; struct Cyc_Absyn_Vardecl* _temp984; struct Cyc_Absyn_Structdecl*
_temp986; struct Cyc_List_List* _temp988; struct Cyc_Core_Opt* _temp990; struct
_tuple0* _temp992; struct Cyc_List_List* _temp994; void* _temp996; struct Cyc_Absyn_Tunionfield*
_temp998; struct Cyc_Absyn_Tuniondecl* _temp1000; struct Cyc_List_List*
_temp1002; struct Cyc_Absyn_Tunionfield* _temp1004; struct Cyc_Absyn_Tuniondecl*
_temp1006; struct Cyc_List_List* _temp1008; void* _temp1010; struct Cyc_Absyn_Exp*
_temp1012; struct Cyc_Absyn_Stmt* _temp1014; struct Cyc_Absyn_Fndecl* _temp1016;
_LL802: if(*(( int*) _temp800) == Cyc_Absyn_Const_e){ _LL881: _temp880=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp800)->f1; if( _temp880 ==( void*) Cyc_Absyn_Null_c){
goto _LL803;} else{ goto _LL804;}} else{ goto _LL804;} _LL804: if(*(( int*)
_temp800) == Cyc_Absyn_Const_e){ goto _LL805;} else{ goto _LL806;} _LL806: if(*((
int*) _temp800) == Cyc_Absyn_Var_e){ _LL885: _temp884=(( struct Cyc_Absyn_Var_e_struct*)
_temp800)->f1; goto _LL883; _LL883: _temp882=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp800)->f2; goto _LL807;} else{ goto _LL808;} _LL808: if(*(( int*) _temp800)
== Cyc_Absyn_UnknownId_e){ _LL887: _temp886=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp800)->f1; goto _LL809;} else{ goto _LL810;} _LL810: if(*(( int*) _temp800)
== Cyc_Absyn_Primop_e){ _LL891: _temp890=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp800)->f1; goto _LL889; _LL889: _temp888=(( struct Cyc_Absyn_Primop_e_struct*)
_temp800)->f2; goto _LL811;} else{ goto _LL812;} _LL812: if(*(( int*) _temp800)
== Cyc_Absyn_Increment_e){ _LL895: _temp894=(( struct Cyc_Absyn_Increment_e_struct*)
_temp800)->f1; goto _LL893; _LL893: _temp892=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp800)->f2; goto _LL813;} else{ goto _LL814;} _LL814: if(*(( int*) _temp800)
== Cyc_Absyn_AssignOp_e){ _LL901: _temp900=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp800)->f1; goto _LL899; _LL899: _temp898=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp800)->f2; goto _LL897; _LL897: _temp896=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp800)->f3; goto _LL815;} else{ goto _LL816;} _LL816: if(*(( int*) _temp800)
== Cyc_Absyn_Conditional_e){ _LL907: _temp906=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp800)->f1; goto _LL905; _LL905: _temp904=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp800)->f2; goto _LL903; _LL903: _temp902=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp800)->f3; goto _LL817;} else{ goto _LL818;} _LL818: if(*(( int*) _temp800)
== Cyc_Absyn_SeqExp_e){ _LL911: _temp910=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp800)->f1; goto _LL909; _LL909: _temp908=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp800)->f2; goto _LL819;} else{ goto _LL820;} _LL820: if(*(( int*) _temp800)
== Cyc_Absyn_UnknownCall_e){ _LL915: _temp914=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp800)->f1; goto _LL913; _LL913: _temp912=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp800)->f2; goto _LL821;} else{ goto _LL822;} _LL822: if(*(( int*) _temp800)
== Cyc_Absyn_FnCall_e){ _LL921: _temp920=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp800)->f1; goto _LL919; _LL919: _temp918=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp800)->f2; goto _LL917; _LL917: _temp916=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp800)->f3; if( _temp916 == 0){ goto _LL823;} else{ goto _LL824;}} else{ goto
_LL824;} _LL824: if(*(( int*) _temp800) == Cyc_Absyn_FnCall_e){ _LL934: _temp933=((
struct Cyc_Absyn_FnCall_e_struct*) _temp800)->f1; goto _LL932; _LL932: _temp931=((
struct Cyc_Absyn_FnCall_e_struct*) _temp800)->f2; goto _LL923; _LL923: _temp922=((
struct Cyc_Absyn_FnCall_e_struct*) _temp800)->f3; if( _temp922 == 0){ goto
_LL826;} else{ _temp924=* _temp922; _LL930: _temp929= _temp924.num_varargs; goto
_LL928; _LL928: _temp927= _temp924.injectors; goto _LL926; _LL926: _temp925=
_temp924.vai; goto _LL825;}} else{ goto _LL826;} _LL826: if(*(( int*) _temp800)
== Cyc_Absyn_Throw_e){ _LL936: _temp935=(( struct Cyc_Absyn_Throw_e_struct*)
_temp800)->f1; goto _LL827;} else{ goto _LL828;} _LL828: if(*(( int*) _temp800)
== Cyc_Absyn_NoInstantiate_e){ _LL938: _temp937=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp800)->f1; goto _LL829;} else{ goto _LL830;} _LL830: if(*(( int*) _temp800)
== Cyc_Absyn_Instantiate_e){ _LL942: _temp941=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp800)->f1; goto _LL940; _LL940: _temp939=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp800)->f2; goto _LL831;} else{ goto _LL832;} _LL832: if(*(( int*) _temp800)
== Cyc_Absyn_Cast_e){ _LL946: _temp945=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp800)->f1; _temp947=&(( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp800)->f1);
goto _LL944; _LL944: _temp943=(( struct Cyc_Absyn_Cast_e_struct*) _temp800)->f2;
goto _LL833;} else{ goto _LL834;} _LL834: if(*(( int*) _temp800) == Cyc_Absyn_Address_e){
_LL949: _temp948=(( struct Cyc_Absyn_Address_e_struct*) _temp800)->f1; goto
_LL835;} else{ goto _LL836;} _LL836: if(*(( int*) _temp800) == Cyc_Absyn_New_e){
_LL953: _temp952=(( struct Cyc_Absyn_New_e_struct*) _temp800)->f1; goto _LL951;
_LL951: _temp950=(( struct Cyc_Absyn_New_e_struct*) _temp800)->f2; goto _LL837;}
else{ goto _LL838;} _LL838: if(*(( int*) _temp800) == Cyc_Absyn_Sizeofexp_e){
_LL955: _temp954=(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp800)->f1; goto
_LL839;} else{ goto _LL840;} _LL840: if(*(( int*) _temp800) == Cyc_Absyn_Sizeoftyp_e){
_LL957: _temp956=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp800)->f1;
goto _LL841;} else{ goto _LL842;} _LL842: if(*(( int*) _temp800) == Cyc_Absyn_Offsetof_e){
_LL961: _temp960=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp800)->f1;
goto _LL959; _LL959: _temp958=(( struct Cyc_Absyn_Offsetof_e_struct*) _temp800)->f2;
goto _LL843;} else{ goto _LL844;} _LL844: if(*(( int*) _temp800) == Cyc_Absyn_Deref_e){
_LL963: _temp962=(( struct Cyc_Absyn_Deref_e_struct*) _temp800)->f1; goto _LL845;}
else{ goto _LL846;} _LL846: if(*(( int*) _temp800) == Cyc_Absyn_StructMember_e){
_LL967: _temp966=(( struct Cyc_Absyn_StructMember_e_struct*) _temp800)->f1; goto
_LL965; _LL965: _temp964=(( struct Cyc_Absyn_StructMember_e_struct*) _temp800)->f2;
goto _LL847;} else{ goto _LL848;} _LL848: if(*(( int*) _temp800) == Cyc_Absyn_StructArrow_e){
_LL971: _temp970=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp800)->f1; goto
_LL969; _LL969: _temp968=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp800)->f2;
goto _LL849;} else{ goto _LL850;} _LL850: if(*(( int*) _temp800) == Cyc_Absyn_Subscript_e){
_LL975: _temp974=(( struct Cyc_Absyn_Subscript_e_struct*) _temp800)->f1; goto
_LL973; _LL973: _temp972=(( struct Cyc_Absyn_Subscript_e_struct*) _temp800)->f2;
goto _LL851;} else{ goto _LL852;} _LL852: if(*(( int*) _temp800) == Cyc_Absyn_Tuple_e){
_LL977: _temp976=(( struct Cyc_Absyn_Tuple_e_struct*) _temp800)->f1; goto _LL853;}
else{ goto _LL854;} _LL854: if(*(( int*) _temp800) == Cyc_Absyn_Array_e){ _LL979:
_temp978=(( struct Cyc_Absyn_Array_e_struct*) _temp800)->f1; goto _LL855;} else{
goto _LL856;} _LL856: if(*(( int*) _temp800) == Cyc_Absyn_Comprehension_e){
_LL985: _temp984=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp800)->f1;
goto _LL983; _LL983: _temp982=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp800)->f2; goto _LL981; _LL981: _temp980=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp800)->f3; goto _LL857;} else{ goto _LL858;} _LL858: if(*(( int*) _temp800)
== Cyc_Absyn_Struct_e){ _LL993: _temp992=(( struct Cyc_Absyn_Struct_e_struct*)
_temp800)->f1; goto _LL991; _LL991: _temp990=(( struct Cyc_Absyn_Struct_e_struct*)
_temp800)->f2; goto _LL989; _LL989: _temp988=(( struct Cyc_Absyn_Struct_e_struct*)
_temp800)->f3; goto _LL987; _LL987: _temp986=(( struct Cyc_Absyn_Struct_e_struct*)
_temp800)->f4; goto _LL859;} else{ goto _LL860;} _LL860: if(*(( int*) _temp800)
== Cyc_Absyn_AnonStruct_e){ _LL997: _temp996=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp800)->f1; goto _LL995; _LL995: _temp994=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp800)->f2; goto _LL861;} else{ goto _LL862;} _LL862: if(*(( int*) _temp800)
== Cyc_Absyn_Tunion_e){ _LL1003: _temp1002=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp800)->f3; if( _temp1002 == 0){ goto _LL1001;} else{ goto _LL864;} _LL1001:
_temp1000=(( struct Cyc_Absyn_Tunion_e_struct*) _temp800)->f4; goto _LL999;
_LL999: _temp998=(( struct Cyc_Absyn_Tunion_e_struct*) _temp800)->f5; goto
_LL863;} else{ goto _LL864;} _LL864: if(*(( int*) _temp800) == Cyc_Absyn_Tunion_e){
_LL1009: _temp1008=(( struct Cyc_Absyn_Tunion_e_struct*) _temp800)->f3; goto
_LL1007; _LL1007: _temp1006=(( struct Cyc_Absyn_Tunion_e_struct*) _temp800)->f4;
goto _LL1005; _LL1005: _temp1004=(( struct Cyc_Absyn_Tunion_e_struct*) _temp800)->f5;
goto _LL865;} else{ goto _LL866;} _LL866: if(*(( int*) _temp800) == Cyc_Absyn_Enum_e){
goto _LL867;} else{ goto _LL868;} _LL868: if(*(( int*) _temp800) == Cyc_Absyn_Malloc_e){
_LL1013: _temp1012=(( struct Cyc_Absyn_Malloc_e_struct*) _temp800)->f1; goto
_LL1011; _LL1011: _temp1010=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp800)->f2; goto _LL869;} else{ goto _LL870;} _LL870: if(*(( int*) _temp800)
== Cyc_Absyn_StmtExp_e){ _LL1015: _temp1014=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp800)->f1; goto _LL871;} else{ goto _LL872;} _LL872: if(*(( int*) _temp800)
== Cyc_Absyn_UnresolvedMem_e){ goto _LL873;} else{ goto _LL874;} _LL874: if(*((
int*) _temp800) == Cyc_Absyn_CompoundLit_e){ goto _LL875;} else{ goto _LL876;}
_LL876: if(*(( int*) _temp800) == Cyc_Absyn_Codegen_e){ _LL1017: _temp1016=((
struct Cyc_Absyn_Codegen_e_struct*) _temp800)->f1; goto _LL877;} else{ goto
_LL878;} _LL878: if(*(( int*) _temp800) == Cyc_Absyn_Fill_e){ goto _LL879;}
else{ goto _LL801;} _LL803: { struct Cyc_Absyn_Exp* _temp1018= Cyc_Absyn_uint_exp(
0, 0); if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){ if( nv->toplevel){( void*)(
e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr( old_typ, _temp1018,
_temp1018))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1019[ 3u]; _temp1019[ 2u]=
_temp1018; _temp1019[ 1u]= _temp1018; _temp1019[ 0u]= _temp1018;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1019, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0))->r));}} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp(
0, 0))->r));} goto _LL801;} _LL805: goto _LL801; _LL807:{ struct _handler_cons
_temp1020; _push_handler(& _temp1020);{ int _temp1022= 0; if( setjmp( _temp1020.handler)){
_temp1022= 1;} if( ! _temp1022){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( nv->varmap,
_temp884))->r));; _pop_handler();} else{ void* _temp1021=( void*) _exn_thrown;
void* _temp1024= _temp1021; _LL1026: if( _temp1024 == Cyc_Dict_Absent){ goto
_LL1027;} else{ goto _LL1028;} _LL1028: goto _LL1029; _LL1027:({ struct Cyc_Stdio_String_pa_struct
_temp1031; _temp1031.tag= Cyc_Stdio_String_pa; _temp1031.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp884);{ void* _temp1030[ 1u]={& _temp1031};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("Can't find %s in exp_to_c, Var\n",
sizeof( unsigned char), 32u), _tag_arr( _temp1030, sizeof( void*), 1u));}});
return; _LL1029:( void) _throw( _temp1024); _LL1025:;}}} goto _LL801; _LL809:({
void* _temp1032[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("unknownid", sizeof( unsigned char), 10u), _tag_arr(
_temp1032, sizeof( void*), 0u));}); goto _LL801; _LL811: { struct Cyc_List_List*
_temp1033=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ, _temp888);(( void(*)(
void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env,
struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp888);{
void* _temp1034= _temp890; _LL1036: if( _temp1034 ==( void*) Cyc_Absyn_Size){
goto _LL1037;} else{ goto _LL1038;} _LL1038: if( _temp1034 ==( void*) Cyc_Absyn_Plus){
goto _LL1039;} else{ goto _LL1040;} _LL1040: if( _temp1034 ==( void*) Cyc_Absyn_Minus){
goto _LL1041;} else{ goto _LL1042;} _LL1042: if( _temp1034 ==( void*) Cyc_Absyn_Eq){
goto _LL1043;} else{ goto _LL1044;} _LL1044: if( _temp1034 ==( void*) Cyc_Absyn_Neq){
goto _LL1045;} else{ goto _LL1046;} _LL1046: if( _temp1034 ==( void*) Cyc_Absyn_Gt){
goto _LL1047;} else{ goto _LL1048;} _LL1048: if( _temp1034 ==( void*) Cyc_Absyn_Gte){
goto _LL1049;} else{ goto _LL1050;} _LL1050: if( _temp1034 ==( void*) Cyc_Absyn_Lt){
goto _LL1051;} else{ goto _LL1052;} _LL1052: if( _temp1034 ==( void*) Cyc_Absyn_Lte){
goto _LL1053;} else{ goto _LL1054;} _LL1054: goto _LL1055; _LL1037: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp888))->hd;{
void* _temp1056= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( arg->topt))->v); struct Cyc_Absyn_Exp* _temp1064; struct Cyc_Absyn_PtrInfo
_temp1066; struct Cyc_Absyn_Conref* _temp1068; void* _temp1070; _LL1058: if((
unsigned int) _temp1056 > 4u?*(( int*) _temp1056) == Cyc_Absyn_ArrayType: 0){
_LL1065: _temp1064=(( struct Cyc_Absyn_ArrayType_struct*) _temp1056)->f3; goto
_LL1059;} else{ goto _LL1060;} _LL1060: if(( unsigned int) _temp1056 > 4u?*((
int*) _temp1056) == Cyc_Absyn_PointerType: 0){ _LL1067: _temp1066=(( struct Cyc_Absyn_PointerType_struct*)
_temp1056)->f1; _LL1071: _temp1070=( void*) _temp1066.elt_typ; goto _LL1069;
_LL1069: _temp1068= _temp1066.bounds; goto _LL1061;} else{ goto _LL1062;}
_LL1062: goto _LL1063; _LL1059:( void*)( e->r=( void*)(( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1064))->r)); goto _LL1057; _LL1061:{ void* _temp1072= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1068); struct Cyc_Absyn_Exp* _temp1078; _LL1074: if(
_temp1072 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1075;} else{ goto _LL1076;}
_LL1076: if(( unsigned int) _temp1072 > 1u?*(( int*) _temp1072) == Cyc_Absyn_Upper_b:
0){ _LL1079: _temp1078=(( struct Cyc_Absyn_Upper_b_struct*) _temp1072)->f1; goto
_LL1077;} else{ goto _LL1073;} _LL1075:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__get_arr_size_e,({ struct Cyc_Absyn_Exp* _temp1080[ 2u]; _temp1080[ 1u]=
Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( _temp1070), 0); _temp1080[ 0u]=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp888))->hd;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1080, sizeof( struct Cyc_Absyn_Exp*), 2u));}), 0))->r)); goto _LL1073;
_LL1077:( void*)( e->r=( void*)(( void*) _temp1078->r)); goto _LL1073; _LL1073:;}
goto _LL1057; _LL1063:({ struct Cyc_Stdio_String_pa_struct _temp1083; _temp1083.tag=
Cyc_Stdio_String_pa; _temp1083.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v);{ struct Cyc_Stdio_String_pa_struct
_temp1082; _temp1082.tag= Cyc_Stdio_String_pa; _temp1082.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v);{
void* _temp1081[ 2u]={& _temp1082,& _temp1083};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("size primop applied to non-array %s (%s)",
sizeof( unsigned char), 41u), _tag_arr( _temp1081, sizeof( void*), 2u));}}});
return; _LL1057:;} goto _LL1035;} _LL1039: { void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt(( void*)(( struct Cyc_List_List*)
_check_null( _temp1033))->hd,& elt_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp888))->hd; struct Cyc_Absyn_Exp* e2=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp888))->tl))->hd;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_plus_e,({ struct Cyc_Absyn_Exp* _temp1084[ 3u]; _temp1084[ 2u]=
e2; _temp1084[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0);
_temp1084[ 0u]= e1;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1084, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));} goto
_LL1035;} _LL1041: { void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt((
void*)(( struct Cyc_List_List*) _check_null( _temp1033))->hd,& elt_typ)){ struct
Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp888))->hd; struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp888))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ((
void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1033))->tl))->hd)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0),
Cyc_Toc_curr_sp, 0))->r));( void*)( e->r=( void*)(( void*)( Cyc_Absyn_divide_exp(
Cyc_Absyn_copy_exp( e), Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0),
0))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__tagged_arr_plus_e,({
struct Cyc_Absyn_Exp* _temp1085[ 3u]; _temp1085[ 2u]= Cyc_Absyn_prim1_exp(( void*)
Cyc_Absyn_Minus, e2, 0); _temp1085[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(
elt_typ), 0); _temp1085[ 0u]= e1;(( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list)( _tag_arr( _temp1085, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));}}
goto _LL1035;} _LL1043: goto _LL1045; _LL1045: goto _LL1047; _LL1047: goto
_LL1049; _LL1049: goto _LL1051; _LL1051: goto _LL1053; _LL1053: { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp888))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp888))->tl))->hd; void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp1033))->hd; void* t2=(
void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1033))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){(
void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1035;} _LL1055: goto
_LL1035; _LL1035:;} goto _LL801;} _LL813: { void* e2_cyc_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp894->topt))->v; Cyc_Toc_exp_to_c( nv, _temp894);{
void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){ struct Cyc_Absyn_Exp* _temp1086=( _temp892 ==( void*) Cyc_Absyn_PostInc?
1: _temp892 ==( void*) Cyc_Absyn_PostDec)? Cyc_Toc__tagged_arr_inplace_plus_post_e:
Cyc_Toc__tagged_arr_inplace_plus_e; int _temp1087=( _temp892 ==( void*) Cyc_Absyn_PreInc?
1: _temp892 ==( void*) Cyc_Absyn_PostInc)? 1: - 1;( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_fncall_exp( _temp1086,({ struct Cyc_Absyn_Exp* _temp1088[ 3u];
_temp1088[ 2u]= Cyc_Absyn_signed_int_exp( _temp1087, 0); _temp1088[ 1u]= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( elt_typ), 0); _temp1088[ 0u]= Cyc_Absyn_address_exp( _temp894,
0);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1088, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));} if( elt_typ ==(
void*) Cyc_Absyn_VoidType? ! Cyc_Absyn_is_lvalue( _temp894): 0){ Cyc_Toc_lvalue_assign(
_temp894, 0, Cyc_Toc_incr_lvalue, _temp892);( void*)( e->r=( void*)(( void*)
_temp894->r));} goto _LL801;}} _LL815: { int e1_poly= Cyc_Toc_is_poly_project(
_temp900); void* e1_old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp900->topt))->v; void* e2_old_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp896->topt))->v; Cyc_Toc_exp_to_c( nv, _temp900); Cyc_Toc_exp_to_c(
nv, _temp896);{ int done= 0; if( _temp898 != 0){ void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt( old_typ,& elt_typ)){ struct Cyc_Absyn_Exp*
change;{ void* _temp1089=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp898))->v;
_LL1091: if( _temp1089 ==( void*) Cyc_Absyn_Plus){ goto _LL1092;} else{ goto
_LL1093;} _LL1093: if( _temp1089 ==( void*) Cyc_Absyn_Minus){ goto _LL1094;}
else{ goto _LL1095;} _LL1095: goto _LL1096; _LL1092: change= _temp896; goto
_LL1090; _LL1094: change= Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Minus, _temp896,
0); goto _LL1090; _LL1096:( int) _throw(({ void* _temp1097[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("bad t ? pointer arithmetic", sizeof( unsigned char), 27u), _tag_arr(
_temp1097, sizeof( void*), 0u));})); goto _LL1090; _LL1090:;} done= 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__tagged_arr_inplace_plus_e,({
struct Cyc_Absyn_Exp* _temp1098[ 3u]; _temp1098[ 2u]= change; _temp1098[ 1u]=
Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0); _temp1098[ 0u]= Cyc_Absyn_address_exp(
_temp900, 0);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1098, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0))->r));}} if( !
done){ if( e1_poly){( void*)( _temp896->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*) _temp896->r, 0), 0))->r));}
if( ! Cyc_Absyn_is_lvalue( _temp900)){(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
struct _tuple7*), struct _tuple7* f_env)) Cyc_Toc_lvalue_assign)( _temp900, 0,
Cyc_Toc_assignop_lvalue,({ struct _tuple7* _temp1099=( struct _tuple7*)
GC_malloc( sizeof( struct _tuple7) * 1); _temp1099[ 0]=({ struct _tuple7
_temp1100; _temp1100.f1= _temp898; _temp1100.f2= _temp896; _temp1100;});
_temp1099;}));( void*)( e->r=( void*)(( void*) _temp900->r));}} goto _LL801;}}
_LL817: Cyc_Toc_exp_to_c( nv, _temp906); Cyc_Toc_exp_to_c( nv, _temp904); Cyc_Toc_exp_to_c(
nv, _temp902); goto _LL801; _LL819: Cyc_Toc_exp_to_c( nv, _temp910); Cyc_Toc_exp_to_c(
nv, _temp908); goto _LL801; _LL821: _temp920= _temp914; _temp918= _temp912; goto
_LL823; _LL823: Cyc_Toc_exp_to_c( nv, _temp920);(( void(*)( void(* f)( struct
Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp918); goto _LL801; _LL825:{
struct _RegionHandle _temp1101= _new_region(); struct _RegionHandle* r=&
_temp1101; _push_region( r);{ struct _tuple0* argv= Cyc_Toc_temp_var(); struct
Cyc_Absyn_Exp* argvexp= Cyc_Absyn_var_exp( argv, 0); struct Cyc_Absyn_Exp*
num_varargs_exp= Cyc_Absyn_uint_exp(( unsigned int) _temp929, 0); void* cva_type=
Cyc_Toc_typ_to_c(( void*) _temp925->type); void* arr_type=( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1118=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1118[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1119; _temp1119.tag= Cyc_Absyn_ArrayType;
_temp1119.f1=( void*) cva_type; _temp1119.f2= Cyc_Toc_mt_tq; _temp1119.f3=(
struct Cyc_Absyn_Exp*) num_varargs_exp; _temp1119;}); _temp1118;}); int num_args=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp931); int num_normargs=
num_args - _temp929; struct Cyc_List_List* new_args= 0;{ int i= 0; for( 0; i <
num_normargs;( ++ i, _temp931=(( struct Cyc_List_List*) _check_null( _temp931))->tl)){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp931))->hd); new_args=({ struct Cyc_List_List* _temp1102=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1102->hd=(
void*)(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp931))->hd);
_temp1102->tl= new_args; _temp1102;});}} new_args=({ struct Cyc_List_List*
_temp1103=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1103->hd=( void*) Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp*
_temp1104[ 3u]; _temp1104[ 2u]= num_varargs_exp; _temp1104[ 1u]= Cyc_Absyn_sizeoftyp_exp(
cva_type, 0); _temp1104[ 0u]= argvexp;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1104, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0); _temp1103->tl= new_args; _temp1103;}); new_args=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( new_args); Cyc_Toc_exp_to_c( nv,
_temp933);{ struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
_temp933, new_args, 0), 0); if( _temp925->inject){ struct _tagged_arr vs=({
unsigned int _temp1114=( unsigned int) _temp929; struct _tuple0** _temp1115=(
struct _tuple0**) _region_malloc( r, sizeof( struct _tuple0*) * _temp1114);
struct _tagged_arr _temp1117= _tag_arr( _temp1115, sizeof( struct _tuple0*),(
unsigned int) _temp929);{ unsigned int _temp1116= _temp1114; unsigned int i;
for( i= 0; i < _temp1116; i ++){ _temp1115[ i]= Cyc_Toc_temp_var();}}; _temp1117;});
struct Cyc_List_List* _temp1105= 0;{ int i= _temp929 - 1; for( 0; i >= 0; -- i){
_temp1105=({ struct Cyc_List_List* _temp1106=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1106->hd=( void*) Cyc_Toc_make_dle( Cyc_Absyn_address_exp(
Cyc_Absyn_var_exp(*(( struct _tuple0**) _check_unknown_subscript( vs, sizeof(
struct _tuple0*), i)), 0), 0)); _temp1106->tl= _temp1105; _temp1106;});}} s= Cyc_Absyn_declare_stmt(
argv, arr_type,( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp( 0,
_temp1105, 0), s, 0);{ int i= 0; for( 0; _temp931 != 0;((( _temp931=(( struct
Cyc_List_List*) _check_null( _temp931))->tl, _temp927=(( struct Cyc_List_List*)
_check_null( _temp927))->tl)), ++ i)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp931))->hd; void* arg_type=( void*)((
struct Cyc_Core_Opt*) _check_null( arg->topt))->v; struct _tuple0* var=*((
struct _tuple0**) _check_unknown_subscript( vs, sizeof( struct _tuple0*), i));
struct Cyc_Absyn_Exp* varexp= Cyc_Absyn_var_exp( var, 0); struct Cyc_Absyn_Tunionfield
_temp1109; struct Cyc_List_List* _temp1110; struct _tuple0* _temp1112; struct
Cyc_Absyn_Tunionfield* _temp1107=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp927))->hd; _temp1109=* _temp1107; _LL1113: _temp1112=
_temp1109.name; goto _LL1111; _LL1111: _temp1110= _temp1109.typs; goto _LL1108;
_LL1108: { void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1110))->hd)).f2); Cyc_Toc_exp_to_c( nv, arg); if( Cyc_Toc_is_void_star(
field_typ)){ arg= Cyc_Absyn_cast_exp( field_typ, arg, 0);} s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( varexp, Cyc_Toc_fieldname( 1),
0), arg, 0), s, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
varexp, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( _temp1112, 0), 0), s, 0); s= Cyc_Absyn_declare_stmt(
var, Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1112, _tag_arr("_struct",
sizeof( unsigned char), 8u))), 0, s, 0);}}}} else{{ int i= 0; for( 0; _temp931
!= 0;( _temp931=(( struct Cyc_List_List*) _check_null( _temp931))->tl, ++ i)){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp931))->hd); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
argvexp, Cyc_Absyn_uint_exp(( unsigned int) i, 0), 0),( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp931))->hd, 0), s, 0);}} s= Cyc_Absyn_declare_stmt(
argv, arr_type, 0, s, 0);}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));}}; _pop_region( r);} goto _LL801; _LL827: Cyc_Toc_exp_to_c( nv,
_temp935);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(
old_typ), Cyc_Toc_newthrow_exp( _temp935), 0))->r)); goto _LL801; _LL829: Cyc_Toc_exp_to_c(
nv, _temp937); goto _LL801; _LL831: Cyc_Toc_exp_to_c( nv, _temp941); for( 0;
_temp939 != 0; _temp939=(( struct Cyc_List_List*) _check_null( _temp939))->tl){
void* k= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*) _check_null(
_temp939))->hd); if( k !=( void*) Cyc_Absyn_EffKind? k !=( void*) Cyc_Absyn_RgnKind:
0){{ void* _temp1120= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*)
_check_null( _temp939))->hd); _LL1122: if(( unsigned int) _temp1120 > 4u?*(( int*)
_temp1120) == Cyc_Absyn_VarType: 0){ goto _LL1123;} else{ goto _LL1124;} _LL1124:
if(( unsigned int) _temp1120 > 4u?*(( int*) _temp1120) == Cyc_Absyn_TunionType:
0){ goto _LL1125;} else{ goto _LL1126;} _LL1126: goto _LL1127; _LL1123:
continue; _LL1125: continue; _LL1127:( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp941, 0))->r)); goto _LL1121; _LL1121:;} break;}} goto _LL801; _LL833: {
void* old_t2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp943->topt))->v;
void* new_typ=* _temp947;* _temp947= Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c(
nv, _temp943);{ struct _tuple8 _temp1129=({ struct _tuple8 _temp1128; _temp1128.f1=
Cyc_Tcutil_compress( old_t2); _temp1128.f2= Cyc_Tcutil_compress( new_typ);
_temp1128;}); void* _temp1137; struct Cyc_Absyn_PtrInfo _temp1139; void*
_temp1141; struct Cyc_Absyn_PtrInfo _temp1143; void* _temp1145; void* _temp1147;
struct Cyc_Absyn_PtrInfo _temp1149; _LL1131: _LL1142: _temp1141= _temp1129.f1;
if(( unsigned int) _temp1141 > 4u?*(( int*) _temp1141) == Cyc_Absyn_PointerType:
0){ _LL1144: _temp1143=(( struct Cyc_Absyn_PointerType_struct*) _temp1141)->f1;
goto _LL1138;} else{ goto _LL1133;} _LL1138: _temp1137= _temp1129.f2; if((
unsigned int) _temp1137 > 4u?*(( int*) _temp1137) == Cyc_Absyn_PointerType: 0){
_LL1140: _temp1139=(( struct Cyc_Absyn_PointerType_struct*) _temp1137)->f1; goto
_LL1132;} else{ goto _LL1133;} _LL1133: _LL1148: _temp1147= _temp1129.f1; if((
unsigned int) _temp1147 > 4u?*(( int*) _temp1147) == Cyc_Absyn_PointerType: 0){
_LL1150: _temp1149=(( struct Cyc_Absyn_PointerType_struct*) _temp1147)->f1; goto
_LL1146;} else{ goto _LL1135;} _LL1146: _temp1145= _temp1129.f2; if((
unsigned int) _temp1145 > 4u?*(( int*) _temp1145) == Cyc_Absyn_IntType: 0){ goto
_LL1134;} else{ goto _LL1135;} _LL1135: goto _LL1136; _LL1132: { int _temp1151=((
int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0, _temp1143.nullable);
int _temp1152=(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
0, _temp1139.nullable); void* _temp1153= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1143.bounds); void* _temp1154= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1139.bounds);{ struct _tuple8 _temp1156=({ struct _tuple8 _temp1155;
_temp1155.f1= _temp1153; _temp1155.f2= _temp1154; _temp1155;}); void* _temp1166;
void* _temp1168; void* _temp1170; void* _temp1172; struct Cyc_Absyn_Exp*
_temp1174; void* _temp1176; struct Cyc_Absyn_Exp* _temp1178; void* _temp1180;
_LL1158: _LL1169: _temp1168= _temp1156.f1; if(( unsigned int) _temp1168 > 1u?*((
int*) _temp1168) == Cyc_Absyn_Upper_b: 0){ goto _LL1167;} else{ goto _LL1160;}
_LL1167: _temp1166= _temp1156.f2; if(( unsigned int) _temp1166 > 1u?*(( int*)
_temp1166) == Cyc_Absyn_Upper_b: 0){ goto _LL1159;} else{ goto _LL1160;} _LL1160:
_LL1173: _temp1172= _temp1156.f1; if(( unsigned int) _temp1172 > 1u?*(( int*)
_temp1172) == Cyc_Absyn_Upper_b: 0){ _LL1175: _temp1174=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1172)->f1; goto _LL1171;} else{ goto _LL1162;} _LL1171: _temp1170=
_temp1156.f2; if( _temp1170 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1161;}
else{ goto _LL1162;} _LL1162: _LL1181: _temp1180= _temp1156.f1; if( _temp1180 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL1177;} else{ goto _LL1164;} _LL1177:
_temp1176= _temp1156.f2; if(( unsigned int) _temp1176 > 1u?*(( int*) _temp1176)
== Cyc_Absyn_Upper_b: 0){ _LL1179: _temp1178=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1176)->f1; goto _LL1163;} else{ goto _LL1164;} _LL1164: goto _LL1165;
_LL1159: if( _temp1151? ! _temp1152: 0){ if( nv->toplevel){({ void* _temp1182[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("can't do null-check conversion at top-level",
sizeof( unsigned char), 44u), _tag_arr( _temp1182, sizeof( void*), 0u));});}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(* _temp947, Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1183=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1183->hd=( void*) _temp943;
_temp1183->tl= 0; _temp1183;}), 0), 0))->r));} goto _LL1157; _LL1161: if( nv->toplevel){(
void*)( e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr( old_t2,
_temp1174, _temp943))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_Absyn_Exp* _temp1184[ 3u]; _temp1184[ 2u]=
_temp1174; _temp1184[ 1u]= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(( void*)
_temp1139.elt_typ), 0); _temp1184[ 0u]= _temp943;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1184, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0))->r));} goto _LL1157; _LL1163: if( nv->toplevel){({ void* _temp1185[
0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("can't coerce t? to t* or t@ at the top-level", sizeof( unsigned char),
45u), _tag_arr( _temp1185, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Exp*
_temp1186= Cyc_Absyn_fncall_exp( Cyc_Toc__untag_arr_e,({ struct Cyc_Absyn_Exp*
_temp1188[ 3u]; _temp1188[ 2u]= _temp1178; _temp1188[ 1u]= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(( void*) _temp1143.elt_typ), 0); _temp1188[ 0u]= _temp943;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1188, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0); if( _temp1152){( void*)(
_temp1186->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({
struct Cyc_List_List* _temp1187=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1187->hd=( void*) Cyc_Absyn_copy_exp( _temp1186);
_temp1187->tl= 0; _temp1187;}), 0))->r));}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(*
_temp947, _temp1186, 0))->r)); goto _LL1157;} _LL1165: goto _LL1157; _LL1157:;}
goto _LL1130;} _LL1134:{ void* _temp1189= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1149.bounds); _LL1191: if( _temp1189 ==( void*) Cyc_Absyn_Unknown_b){ goto
_LL1192;} else{ goto _LL1193;} _LL1193: goto _LL1194; _LL1192:( void*)( _temp943->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) _temp943->r,
_temp943->loc), Cyc_Toc_curr_sp, 0))->r)); goto _LL1190; _LL1194: goto _LL1190;
_LL1190:;} goto _LL1130; _LL1136: goto _LL1130; _LL1130:;} goto _LL801;} _LL835:{
void* _temp1195=( void*) _temp948->r; struct Cyc_List_List* _temp1203; struct
_tuple0* _temp1205; struct Cyc_List_List* _temp1207; _LL1197: if(*(( int*)
_temp1195) == Cyc_Absyn_Struct_e){ _LL1206: _temp1205=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1195)->f1; goto _LL1204; _LL1204: _temp1203=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1195)->f3; goto _LL1198;} else{ goto _LL1199;} _LL1199: if(*(( int*)
_temp1195) == Cyc_Absyn_Tuple_e){ _LL1208: _temp1207=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1195)->f1; goto _LL1200;} else{ goto _LL1201;} _LL1201: goto _LL1202;
_LL1198: if( nv->toplevel){({ struct Cyc_Stdio_String_pa_struct _temp1210;
_temp1210.tag= Cyc_Stdio_String_pa; _temp1210.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp948->loc);{ void* _temp1209[ 1u]={& _temp1210};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("%s: & on non-identifiers at the top-level",
sizeof( unsigned char), 42u), _tag_arr( _temp1209, sizeof( void*), 1u));}});}(
void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp948->topt))->v, 1, 0, _temp1203, _temp1205))->r)); goto
_LL1196; _LL1200: if( nv->toplevel){({ struct Cyc_Stdio_String_pa_struct
_temp1212; _temp1212.tag= Cyc_Stdio_String_pa; _temp1212.f1=( struct _tagged_arr)
Cyc_Position_string_of_segment( _temp948->loc);{ void* _temp1211[ 1u]={&
_temp1212};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("%s: & on non-identifiers at the top-level", sizeof( unsigned char), 42u),
_tag_arr( _temp1211, sizeof( void*), 1u));}});}( void*)( e->r=( void*)(( void*)(
Cyc_Toc_init_tuple( nv, 1, 0, _temp1207))->r)); goto _LL1196; _LL1202: Cyc_Toc_exp_to_c(
nv, _temp948); if( ! Cyc_Absyn_is_lvalue( _temp948)){(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
int), int f_env)) Cyc_Toc_lvalue_assign)( _temp948, 0, Cyc_Toc_address_lvalue, 1);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v), _temp948, 0))->r));} goto
_LL1196; _LL1196:;} goto _LL801; _LL837: if( nv->toplevel){({ struct Cyc_Stdio_String_pa_struct
_temp1214; _temp1214.tag= Cyc_Stdio_String_pa; _temp1214.f1=( struct _tagged_arr)
Cyc_Position_string_of_segment( _temp950->loc);{ void* _temp1213[ 1u]={&
_temp1214};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("%s: new at top-level", sizeof( unsigned char), 21u), _tag_arr(
_temp1213, sizeof( void*), 1u));}});}{ void* _temp1215=( void*) _temp950->r;
struct Cyc_List_List* _temp1227; struct Cyc_Absyn_Exp* _temp1229; struct Cyc_Absyn_Exp*
_temp1231; struct Cyc_Absyn_Vardecl* _temp1233; struct Cyc_List_List* _temp1235;
struct _tuple0* _temp1237; struct Cyc_List_List* _temp1239; _LL1217: if(*(( int*)
_temp1215) == Cyc_Absyn_Array_e){ _LL1228: _temp1227=(( struct Cyc_Absyn_Array_e_struct*)
_temp1215)->f1; goto _LL1218;} else{ goto _LL1219;} _LL1219: if(*(( int*)
_temp1215) == Cyc_Absyn_Comprehension_e){ _LL1234: _temp1233=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1215)->f1; goto _LL1232; _LL1232: _temp1231=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1215)->f2; goto _LL1230; _LL1230: _temp1229=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1215)->f3; goto _LL1220;} else{ goto _LL1221;} _LL1221: if(*(( int*)
_temp1215) == Cyc_Absyn_Struct_e){ _LL1238: _temp1237=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1215)->f1; goto _LL1236; _LL1236: _temp1235=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1215)->f3; goto _LL1222;} else{ goto _LL1223;} _LL1223: if(*(( int*)
_temp1215) == Cyc_Absyn_Tuple_e){ _LL1240: _temp1239=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1215)->f1; goto _LL1224;} else{ goto _LL1225;} _LL1225: goto _LL1226;
_LL1218: { struct _tuple0* _temp1241= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp1242= Cyc_Absyn_var_exp( _temp1241, 0); struct Cyc_Absyn_Stmt* _temp1243=
Cyc_Toc_init_array( nv, _temp1242, _temp1227, Cyc_Absyn_exp_stmt( _temp1242, 0));
void* old_elt_typ;{ void* _temp1244= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1250; struct Cyc_Absyn_Tqual _temp1252; void* _temp1254; _LL1246: if((
unsigned int) _temp1244 > 4u?*(( int*) _temp1244) == Cyc_Absyn_PointerType: 0){
_LL1251: _temp1250=(( struct Cyc_Absyn_PointerType_struct*) _temp1244)->f1;
_LL1255: _temp1254=( void*) _temp1250.elt_typ; goto _LL1253; _LL1253: _temp1252=
_temp1250.tq; goto _LL1247;} else{ goto _LL1248;} _LL1248: goto _LL1249; _LL1247:
old_elt_typ= _temp1254; goto _LL1245; _LL1249: old_elt_typ=({ void* _temp1256[ 0u]={};
Cyc_Toc_toc_impos( _tag_arr("exp_to_c:new array expression doesn't have ptr type",
sizeof( unsigned char), 52u), _tag_arr( _temp1256, sizeof( void*), 0u));}); goto
_LL1245; _LL1245:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void*
_temp1257= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); struct Cyc_Absyn_Exp*
_temp1258= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_signed_int_exp(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1227), 0), 0); struct
Cyc_Absyn_Exp* e1; if( _temp952 == 0){ e1= Cyc_Toc_malloc_exp( old_elt_typ,
_temp1258);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null(
_temp952); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r, _temp1258);}{
struct Cyc_Absyn_Exp* _temp1259= Cyc_Absyn_cast_exp( _temp1257, e1, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1241,
_temp1257,( struct Cyc_Absyn_Exp*) _temp1259, _temp1243, 0), 0))->r)); goto
_LL1216;}}} _LL1220: { int is_tagged_ptr= 0;{ void* _temp1260= Cyc_Tcutil_compress(
old_typ); struct Cyc_Absyn_PtrInfo _temp1266; struct Cyc_Absyn_Conref* _temp1268;
struct Cyc_Absyn_Tqual _temp1270; void* _temp1272; _LL1262: if(( unsigned int)
_temp1260 > 4u?*(( int*) _temp1260) == Cyc_Absyn_PointerType: 0){ _LL1267:
_temp1266=(( struct Cyc_Absyn_PointerType_struct*) _temp1260)->f1; _LL1273:
_temp1272=( void*) _temp1266.elt_typ; goto _LL1271; _LL1271: _temp1270=
_temp1266.tq; goto _LL1269; _LL1269: _temp1268= _temp1266.bounds; goto _LL1263;}
else{ goto _LL1264;} _LL1264: goto _LL1265; _LL1263: is_tagged_ptr= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1268) ==( void*) Cyc_Absyn_Unknown_b; goto _LL1261;
_LL1265:({ void* _temp1274[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: comprehension not an array type",
sizeof( unsigned char), 42u), _tag_arr( _temp1274, sizeof( void*), 0u));}); goto
_LL1261; _LL1261:;}{ struct _tuple0* max= Cyc_Toc_temp_var(); struct _tuple0* a=
Cyc_Toc_temp_var(); void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1229->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ);
void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c(
nv, _temp1231);{ struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp(
a, 0), _temp1233, Cyc_Absyn_var_exp( max, 0), _temp1229, Cyc_Absyn_skip_stmt( 0),
1);{ struct _RegionHandle _temp1275= _new_region(); struct _RegionHandle* r=&
_temp1275; _push_region( r);{ struct Cyc_List_List* decls=({ struct Cyc_List_List*
_temp1293=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp1293->hd=( void*)({ struct _tuple9* _temp1294=( struct _tuple9*)
_region_malloc( r, sizeof( struct _tuple9)); _temp1294->f1= max; _temp1294->f2=
Cyc_Absyn_uint_t; _temp1294->f3=( struct Cyc_Absyn_Exp*) _temp1231; _temp1294;});
_temp1293->tl= 0; _temp1293;}); struct Cyc_Absyn_Exp* ai; if( _temp952 == 0){ ai=
Cyc_Toc_malloc_exp( old_elt_typ, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0));} else{ struct Cyc_Absyn_Exp* r=(
struct Cyc_Absyn_Exp*) _check_null( _temp952); Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp(
r, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));}{ struct Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp( ptr_typ, ai, 0);
decls=({ struct Cyc_List_List* _temp1276=( struct Cyc_List_List*) _region_malloc(
r, sizeof( struct Cyc_List_List)); _temp1276->hd=( void*)({ struct _tuple9*
_temp1277=( struct _tuple9*) _region_malloc( r, sizeof( struct _tuple9));
_temp1277->f1= a; _temp1277->f2= ptr_typ; _temp1277->f3=( struct Cyc_Absyn_Exp*)
ainit; _temp1277;}); _temp1276->tl= decls; _temp1276;}); if( is_tagged_ptr){
struct _tuple0* _temp1278= Cyc_Toc_temp_var(); void* _temp1279= Cyc_Toc_typ_to_c(
old_typ); struct Cyc_Absyn_Exp* _temp1280= Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp* _temp1283[ 3u]; _temp1283[ 2u]= _temp1231; _temp1283[ 1u]=
Cyc_Absyn_sizeoftyp_exp( elt_typ, 0); _temp1283[ 0u]= Cyc_Absyn_var_exp( a, 0);((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1283, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0); decls=({ struct Cyc_List_List*
_temp1281=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp1281->hd=( void*)({ struct _tuple9* _temp1282=( struct _tuple9*)
_region_malloc( r, sizeof( struct _tuple9)); _temp1282->f1= _temp1278; _temp1282->f2=
_temp1279; _temp1282->f3=( struct Cyc_Absyn_Exp*) _temp1280; _temp1282;});
_temp1281->tl= decls; _temp1281;}); s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( _temp1278, 0), 0), 0);} else{ s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( a, 0), 0), 0);}{ struct Cyc_List_List* _temp1284= decls; for(
0; _temp1284 != 0; _temp1284=(( struct Cyc_List_List*) _check_null( _temp1284))->tl){
struct Cyc_Absyn_Exp* _temp1287; void* _temp1289; struct _tuple0* _temp1291;
struct _tuple9 _temp1285=*(( struct _tuple9*)(( struct Cyc_List_List*)
_check_null( _temp1284))->hd); _LL1292: _temp1291= _temp1285.f1; goto _LL1290;
_LL1290: _temp1289= _temp1285.f2; goto _LL1288; _LL1288: _temp1287= _temp1285.f3;
goto _LL1286; _LL1286: s= Cyc_Absyn_declare_stmt( _temp1291, _temp1289,
_temp1287, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}};
_pop_region( r);} goto _LL1216;}}} _LL1222:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp950->topt))->v, 1,
_temp952, _temp1235, _temp1237))->r)); goto _LL1216; _LL1224:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, _temp952, _temp1239))->r)); goto
_LL1216; _LL1226: { void* old_elt_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp950->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ);
struct Cyc_Absyn_Exp* mexp= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0); if( _temp952
== 0){ mexp= Cyc_Toc_malloc_exp( old_elt_typ, mexp);} else{ struct Cyc_Absyn_Exp*
r=( struct Cyc_Absyn_Exp*) _check_null( _temp952); Cyc_Toc_exp_to_c( nv, r);
mexp= Cyc_Toc_rmalloc_exp( r, mexp);} mexp= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq), mexp, 0);{ int done= 0;{ void* _temp1295=( void*)
_temp950->r; struct Cyc_Absyn_Exp* _temp1301; void* _temp1303; _LL1297: if(*((
int*) _temp1295) == Cyc_Absyn_Cast_e){ _LL1304: _temp1303=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1295)->f1; goto _LL1302; _LL1302: _temp1301=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1295)->f2; goto _LL1298;} else{ goto _LL1299;} _LL1299: goto _LL1300;
_LL1298:{ struct _tuple8 _temp1306=({ struct _tuple8 _temp1305; _temp1305.f1=
Cyc_Tcutil_compress( _temp1303); _temp1305.f2= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1301->topt))->v); _temp1305;}); void*
_temp1312; struct Cyc_Absyn_PtrInfo _temp1314; struct Cyc_Absyn_Conref*
_temp1316; void* _temp1318; struct Cyc_Absyn_PtrInfo _temp1320; struct Cyc_Absyn_Conref*
_temp1322; void* _temp1324; _LL1308: _LL1319: _temp1318= _temp1306.f1; if((
unsigned int) _temp1318 > 4u?*(( int*) _temp1318) == Cyc_Absyn_PointerType: 0){
_LL1321: _temp1320=(( struct Cyc_Absyn_PointerType_struct*) _temp1318)->f1;
_LL1325: _temp1324=( void*) _temp1320.elt_typ; goto _LL1323; _LL1323: _temp1322=
_temp1320.bounds; goto _LL1313;} else{ goto _LL1310;} _LL1313: _temp1312=
_temp1306.f2; if(( unsigned int) _temp1312 > 4u?*(( int*) _temp1312) == Cyc_Absyn_PointerType:
0){ _LL1315: _temp1314=(( struct Cyc_Absyn_PointerType_struct*) _temp1312)->f1;
_LL1317: _temp1316= _temp1314.bounds; goto _LL1309;} else{ goto _LL1310;}
_LL1310: goto _LL1311; _LL1309:{ struct _tuple8 _temp1327=({ struct _tuple8
_temp1326; _temp1326.f1= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1322);
_temp1326.f2= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1316); _temp1326;});
void* _temp1333; struct Cyc_Absyn_Exp* _temp1335; void* _temp1337; _LL1329:
_LL1338: _temp1337= _temp1327.f1; if( _temp1337 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1334;} else{ goto _LL1331;} _LL1334: _temp1333= _temp1327.f2; if((
unsigned int) _temp1333 > 1u?*(( int*) _temp1333) == Cyc_Absyn_Upper_b: 0){
_LL1336: _temp1335=(( struct Cyc_Absyn_Upper_b_struct*) _temp1333)->f1; goto
_LL1330;} else{ goto _LL1331;} _LL1331: goto _LL1332; _LL1330: done= 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__init_tag_arr_e,({ struct
Cyc_Absyn_Exp* _temp1339[ 4u]; _temp1339[ 3u]= _temp1335; _temp1339[ 2u]= Cyc_Absyn_sizeoftyp_exp(
_temp1324, 0); _temp1339[ 1u]= _temp1301; _temp1339[ 0u]= mexp;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1339, sizeof( struct Cyc_Absyn_Exp*),
4u));}), 0))->r)); goto _LL1328; _LL1332: goto _LL1328; _LL1328:;} goto _LL1307;
_LL1311: goto _LL1307; _LL1307:;} goto _LL1296; _LL1300: goto _LL1296; _LL1296:;}
if( ! done){ struct _tuple0* _temp1340= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt*
_temp1341= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1340, 0), 0); struct Cyc_Absyn_Exp*
_temp1342= Cyc_Absyn_signed_int_exp( 0, 0); Cyc_Toc_exp_to_c( nv, _temp950);
_temp1341= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( _temp1340, 0), _temp1342, 0), _temp950, 0), _temp1341, 0);{
void* _temp1343= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( _temp1340,
_temp1343,( struct Cyc_Absyn_Exp*) mexp, _temp1341, 0), 0))->r));}} goto _LL1216;}}
_LL1216:;} goto _LL801; _LL839: Cyc_Toc_exp_to_c( nv, _temp954); goto _LL801;
_LL841:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1344=( struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1344[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1345; _temp1345.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1345.f1=( void*) Cyc_Toc_typ_to_c_array( _temp956);
_temp1345;}); _temp1344;}))); goto _LL801; _LL843:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Offsetof_e_struct* _temp1346=( struct Cyc_Absyn_Offsetof_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp1346[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp1347; _temp1347.tag= Cyc_Absyn_Offsetof_e;
_temp1347.f1=( void*) Cyc_Toc_typ_to_c_array( _temp960); _temp1347.f2= _temp958;
_temp1347;}); _temp1346;}))); goto _LL801; _LL845: { void* _temp1348= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp962->topt))->v);{ void*
_temp1349= _temp1348; struct Cyc_Absyn_PtrInfo _temp1355; struct Cyc_Absyn_Conref*
_temp1357; struct Cyc_Absyn_Tqual _temp1359; struct Cyc_Absyn_Conref* _temp1361;
void* _temp1363; void* _temp1365; _LL1351: if(( unsigned int) _temp1349 > 4u?*((
int*) _temp1349) == Cyc_Absyn_PointerType: 0){ _LL1356: _temp1355=(( struct Cyc_Absyn_PointerType_struct*)
_temp1349)->f1; _LL1366: _temp1365=( void*) _temp1355.elt_typ; goto _LL1364;
_LL1364: _temp1363=( void*) _temp1355.rgn_typ; goto _LL1362; _LL1362: _temp1361=
_temp1355.nullable; goto _LL1360; _LL1360: _temp1359= _temp1355.tq; goto _LL1358;
_LL1358: _temp1357= _temp1355.bounds; goto _LL1352;} else{ goto _LL1353;}
_LL1353: goto _LL1354; _LL1352:{ void* _temp1367= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1357); _LL1369: if(( unsigned int) _temp1367 > 1u?*(( int*) _temp1367) ==
Cyc_Absyn_Upper_b: 0){ goto _LL1370;} else{ goto _LL1371;} _LL1371: if(
_temp1367 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1372;} else{ goto _LL1368;}
_LL1370: Cyc_Toc_exp_to_c( nv, _temp962); if( Cyc_Toc_is_nullable( _temp1348)){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp962->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1373=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1373->hd=( void*) _temp962;
_temp1373->tl= 0; _temp1373;}), 0), 0), 0))->r));} goto _LL1368; _LL1372: {
struct Cyc_Absyn_Exp* _temp1374= Cyc_Absyn_uint_exp( 0, 0); _temp1374->topt=({
struct Cyc_Core_Opt* _temp1375=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1375->v=( void*) Cyc_Absyn_uint_t; _temp1375;});( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_subscript_exp( _temp962, _temp1374, 0))->r)); Cyc_Toc_exp_to_c(
nv, e); goto _LL1368;} _LL1368:;} goto _LL1350; _LL1354:({ void* _temp1376[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("exp_to_c: Deref: non-pointer", sizeof( unsigned char), 29u), _tag_arr(
_temp1376, sizeof( void*), 0u));}); goto _LL1350; _LL1350:;} goto _LL801;}
_LL847: Cyc_Toc_exp_to_c( nv, _temp966); if( Cyc_Toc_is_poly_project( e)){( void*)(
e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r,
0), 0))->r));} goto _LL801; _LL849: { void* e1typ= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp970->topt))->v); Cyc_Toc_exp_to_c( nv,
_temp970);{ int is_poly= Cyc_Toc_is_poly_project( e); struct Cyc_Absyn_Conref*
_temp1379; struct Cyc_Absyn_Tqual _temp1381; struct Cyc_Absyn_Conref* _temp1383;
void* _temp1385; void* _temp1387; struct Cyc_Absyn_PtrInfo _temp1377= Cyc_Toc_get_ptr_type(
e1typ); _LL1388: _temp1387=( void*) _temp1377.elt_typ; goto _LL1386; _LL1386:
_temp1385=( void*) _temp1377.rgn_typ; goto _LL1384; _LL1384: _temp1383=
_temp1377.nullable; goto _LL1382; _LL1382: _temp1381= _temp1377.tq; goto _LL1380;
_LL1380: _temp1379= _temp1377.bounds; goto _LL1378; _LL1378:{ void* _temp1389=
Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1379); struct Cyc_Absyn_Exp*
_temp1395; _LL1391: if(( unsigned int) _temp1389 > 1u?*(( int*) _temp1389) ==
Cyc_Absyn_Upper_b: 0){ _LL1396: _temp1395=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1389)->f1; goto _LL1392;} else{ goto _LL1393;} _LL1393: if( _temp1389 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL1394;} else{ goto _LL1390;} _LL1392: if(
Cyc_Evexp_eval_const_uint_exp( _temp1395) < 1){({ void* _temp1397[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("exp_to_c:  StructArrow_e on pointer of size 0",
sizeof( unsigned char), 46u), _tag_arr( _temp1397, sizeof( void*), 0u));});}{
int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
0, _temp1383); if( Cyc_Toc_is_nullable(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp970->topt))->v)){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structarrow_exp(
Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp970->topt))->v), Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({
struct Cyc_List_List* _temp1398=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1398->hd=( void*) _temp970; _temp1398->tl= 0;
_temp1398;}), 0), 0), _temp968, 0))->r));} goto _LL1390;} _LL1394: { void* ta1=
Cyc_Toc_typ_to_c_array( _temp1387);( void*)( _temp970->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( ta1, _temp1381), Cyc_Absyn_fncall_exp( Cyc_Toc__check_unknown_subscript_e,({
struct Cyc_Absyn_Exp* _temp1399[ 3u]; _temp1399[ 2u]= Cyc_Absyn_uint_exp( 0, 0);
_temp1399[ 1u]= Cyc_Absyn_sizeoftyp_exp( ta1, 0); _temp1399[ 0u]= Cyc_Absyn_copy_exp(
_temp970);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1399, sizeof( struct Cyc_Absyn_Exp*), 3u));}), 0), 0))->r)); goto
_LL1390;} _LL1390:;} if( is_poly){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));} goto _LL801;}} _LL851: { void*
_temp1400= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp974->topt))->v); Cyc_Toc_exp_to_c( nv, _temp974); Cyc_Toc_exp_to_c( nv,
_temp972);{ void* _temp1401= _temp1400; struct Cyc_List_List* _temp1409; struct
Cyc_Absyn_PtrInfo _temp1411; struct Cyc_Absyn_Conref* _temp1413; struct Cyc_Absyn_Tqual
_temp1415; struct Cyc_Absyn_Conref* _temp1417; void* _temp1419; void* _temp1421;
_LL1403: if(( unsigned int) _temp1401 > 4u?*(( int*) _temp1401) == Cyc_Absyn_TupleType:
0){ _LL1410: _temp1409=(( struct Cyc_Absyn_TupleType_struct*) _temp1401)->f1;
goto _LL1404;} else{ goto _LL1405;} _LL1405: if(( unsigned int) _temp1401 > 4u?*((
int*) _temp1401) == Cyc_Absyn_PointerType: 0){ _LL1412: _temp1411=(( struct Cyc_Absyn_PointerType_struct*)
_temp1401)->f1; _LL1422: _temp1421=( void*) _temp1411.elt_typ; goto _LL1420;
_LL1420: _temp1419=( void*) _temp1411.rgn_typ; goto _LL1418; _LL1418: _temp1417=
_temp1411.nullable; goto _LL1416; _LL1416: _temp1415= _temp1411.tq; goto _LL1414;
_LL1414: _temp1413= _temp1411.bounds; goto _LL1406;} else{ goto _LL1407;}
_LL1407: goto _LL1408; _LL1404: { unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp972) + 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
_temp974, Cyc_Toc_fieldname(( int) i), 0))->r)); goto _LL1402;} _LL1406:{ void*
_temp1423= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1413); struct Cyc_Absyn_Exp*
_temp1429; _LL1425: if(( unsigned int) _temp1423 > 1u?*(( int*) _temp1423) ==
Cyc_Absyn_Upper_b: 0){ _LL1430: _temp1429=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1423)->f1; goto _LL1426;} else{ goto _LL1427;} _LL1427: if( _temp1423 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL1428;} else{ goto _LL1424;} _LL1426:
_temp1429= Cyc_Absyn_uint_exp( Cyc_Evexp_eval_const_uint_exp( _temp1429), 0);{
int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
0, _temp1417); void* ta1= Cyc_Toc_typ_to_c( _temp1421); void* ta2= Cyc_Absyn_cstar_typ(
ta1, _temp1415); if( possibly_null){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp(
Cyc_Absyn_cast_exp( ta2, Cyc_Absyn_fncall_exp( Cyc_Toc__check_known_subscript_null_e,({
struct Cyc_Absyn_Exp* _temp1431[ 4u]; _temp1431[ 3u]= _temp972; _temp1431[ 2u]=
Cyc_Absyn_sizeoftyp_exp( ta1, 0); _temp1431[ 1u]= _temp1429; _temp1431[ 0u]=
_temp974;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1431, sizeof( struct Cyc_Absyn_Exp*), 4u));}), 0), 0), 0))->r));}
else{( void*)( _temp972->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__check_known_subscript_notnull_e,({
struct Cyc_Absyn_Exp* _temp1432[ 2u]; _temp1432[ 1u]= Cyc_Absyn_copy_exp(
_temp972); _temp1432[ 0u]= _temp1429;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp1432, sizeof( struct Cyc_Absyn_Exp*),
2u));}), 0))->r));} goto _LL1424;} _LL1428: { void* ta1= Cyc_Toc_typ_to_c_array(
_temp1421);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( ta1, _temp1415), Cyc_Absyn_fncall_exp( Cyc_Toc__check_unknown_subscript_e,({
struct Cyc_Absyn_Exp* _temp1433[ 3u]; _temp1433[ 2u]= _temp972; _temp1433[ 1u]=
Cyc_Absyn_sizeoftyp_exp( ta1, 0); _temp1433[ 0u]= _temp974;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1433, sizeof( struct Cyc_Absyn_Exp*),
3u));}), 0), 0), 0))->r)); goto _LL1424;} _LL1424:;} goto _LL1402; _LL1408:({
void* _temp1434[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: Subscript on non-tuple/array/tuple ptr",
sizeof( unsigned char), 49u), _tag_arr( _temp1434, sizeof( void*), 0u));}); goto
_LL1402; _LL1402:;} goto _LL801;} _LL853: if( ! nv->toplevel){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 0, 0, _temp976))->r));} else{ struct
Cyc_List_List* _temp1435=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c,
_temp976); struct _tagged_arr* _temp1436= Cyc_Toc_add_tuple_type( _temp1435);
struct Cyc_List_List* dles= 0;{ int i= 1; for( 0; _temp976 != 0;( _temp976=((
struct Cyc_List_List*) _check_null( _temp976))->tl, i ++)){ Cyc_Toc_exp_to_c( nv,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp976))->hd);
dles=({ struct Cyc_List_List* _temp1437=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1437->hd=( void*)({ struct _tuple4*
_temp1438=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1438->f1=
0; _temp1438->f2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp976))->hd; _temp1438;}); _temp1437->tl= dles; _temp1437;});}} dles=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, dles, 0))->r));}
goto _LL801; _LL855:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp978, 0))->r));{ struct Cyc_List_List* _temp1439= _temp978; for( 0;
_temp1439 != 0; _temp1439=(( struct Cyc_List_List*) _check_null( _temp1439))->tl){
struct _tuple4 _temp1442; struct Cyc_Absyn_Exp* _temp1443; struct _tuple4*
_temp1440=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1439))->hd;
_temp1442=* _temp1440; _LL1444: _temp1443= _temp1442.f2; goto _LL1441; _LL1441:
Cyc_Toc_exp_to_c( nv, _temp1443);}} goto _LL801; _LL857: { unsigned int
_temp1445= Cyc_Evexp_eval_const_uint_exp( _temp982); Cyc_Toc_exp_to_c( nv,
_temp980);{ struct Cyc_List_List* es= 0; if( ! Cyc_Toc_is_zero( _temp980)){
unsigned int i= 0; for( 0; i < _temp1445; i ++){ es=({ struct Cyc_List_List*
_temp1446=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1446->hd=( void*)({ struct _tuple4* _temp1447=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1447->f1= 0; _temp1447->f2= _temp980; _temp1447;});
_temp1446->tl= es; _temp1446;});}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, es, 0))->r)); goto _LL801;}} _LL859: if( ! nv->toplevel){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp988, _temp992))->r));}
else{ if( _temp986 == 0){({ void* _temp1448[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("Struct_e: missing structdecl pointer",
sizeof( unsigned char), 37u), _tag_arr( _temp1448, sizeof( void*), 0u));});}{
struct Cyc_Absyn_Structdecl* sd2=( struct Cyc_Absyn_Structdecl*) _check_null(
_temp986); struct _RegionHandle _temp1449= _new_region(); struct _RegionHandle*
rgn=& _temp1449; _push_region( rgn);{ struct Cyc_List_List* _temp1450=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp988,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( sd2->fields))->v); struct Cyc_List_List* _temp1451= 0;{ struct Cyc_List_List*
_temp1452=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd2->fields))->v;
for( 0; _temp1452 != 0; _temp1452=(( struct Cyc_List_List*) _check_null(
_temp1452))->tl){ struct Cyc_List_List* _temp1453= _temp1450; for( 0; _temp1453
!= 0; _temp1453=(( struct Cyc_List_List*) _check_null( _temp1453))->tl){ if((*((
struct _tuple10*)(( struct Cyc_List_List*) _check_null( _temp1453))->hd)).f1 ==(
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp1452))->hd){
struct _tuple10 _temp1456; struct Cyc_Absyn_Exp* _temp1457; struct Cyc_Absyn_Structfield*
_temp1459; struct _tuple10* _temp1454=( struct _tuple10*)(( struct Cyc_List_List*)
_check_null( _temp1453))->hd; _temp1456=* _temp1454; _LL1460: _temp1459=
_temp1456.f1; goto _LL1458; _LL1458: _temp1457= _temp1456.f2; goto _LL1455;
_LL1455: { void* _temp1461=( void*) _temp1459->type; Cyc_Toc_exp_to_c( nv,
_temp1457); if( Cyc_Toc_is_void_star( _temp1461)){( void*)( _temp1457->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp1457->r, 0), 0))->r));} _temp1451=({ struct Cyc_List_List* _temp1462=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1462->hd=(
void*)({ struct _tuple4* _temp1463=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp1463->f1= 0; _temp1463->f2= _temp1457; _temp1463;}); _temp1462->tl=
_temp1451; _temp1462;}); break;}}}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0,(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp1451), 0))->r));}; _pop_region( rgn);}} goto _LL801; _LL861: { struct Cyc_List_List*
fs;{ void* _temp1464= Cyc_Tcutil_compress( _temp996); struct Cyc_List_List*
_temp1470; _LL1466: if(( unsigned int) _temp1464 > 4u?*(( int*) _temp1464) ==
Cyc_Absyn_AnonStructType: 0){ _LL1471: _temp1470=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp1464)->f1; goto _LL1467;} else{ goto _LL1468;} _LL1468: goto _LL1469;
_LL1467: fs= _temp1470; goto _LL1465; _LL1469: fs=({ struct Cyc_Stdio_String_pa_struct
_temp1473; _temp1473.tag= Cyc_Stdio_String_pa; _temp1473.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( _temp996);{ void* _temp1472[ 1u]={& _temp1473};(( struct
Cyc_List_List*(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("anon struct has type %s", sizeof( unsigned char), 24u), _tag_arr(
_temp1472, sizeof( void*), 1u));}}); goto _LL1465; _LL1465:;}{ struct
_RegionHandle _temp1474= _new_region(); struct _RegionHandle* rgn=& _temp1474;
_push_region( rgn);{ struct Cyc_List_List* _temp1475=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
e->loc, _temp994, fs); for( 0; _temp1475 != 0; _temp1475=(( struct Cyc_List_List*)
_check_null( _temp1475))->tl){ struct _tuple10 _temp1478; struct Cyc_Absyn_Exp*
_temp1479; struct Cyc_Absyn_Structfield* _temp1481; struct _tuple10* _temp1476=(
struct _tuple10*)(( struct Cyc_List_List*) _check_null( _temp1475))->hd;
_temp1478=* _temp1476; _LL1482: _temp1481= _temp1478.f1; goto _LL1480; _LL1480:
_temp1479= _temp1478.f2; goto _LL1477; _LL1477: { void* _temp1483=( void*)
_temp1481->type; Cyc_Toc_exp_to_c( nv, _temp1479); if( Cyc_Toc_is_void_star(
_temp1483)){( void*)( _temp1479->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1479->r, 0), 0))->r));}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0, _temp994, 0))->r));}; _pop_region( rgn);}
goto _LL801;} _LL863: { struct _tuple0* qv= _temp998->name; if( _temp1000->is_xtunion?
1: ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));}
else{ int tag_count= 0; struct Cyc_List_List* _temp1484= _temp1000->fields == 0?
0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1000->fields))->v;
while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp1484))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1484))->hd)->typs == 0){ tag_count ++;}
_temp1484=(( struct Cyc_List_List*) _check_null( _temp1484))->tl;}( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));} goto
_LL801;} _LL865: { struct _tuple0* _temp1485= _temp1004->name; struct Cyc_List_List*
_temp1486= _temp1004->typs; struct _tuple0* _temp1487= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* _temp1488= Cyc_Absyn_var_exp( _temp1487, 0); void*
_temp1489= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1485, _tag_arr("_struct",
sizeof( unsigned char), 8u))); if( nv->toplevel){ struct Cyc_Absyn_Exp* tag_exp;
if( _temp1006->is_xtunion){ tag_exp= Cyc_Absyn_var_exp( _temp1485, 0);} else{
int tag_count= 0; struct Cyc_List_List* _temp1490= _temp1006->fields == 0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1006->fields))->v;
while( Cyc_Absyn_qvar_cmp( _temp1485,(( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp1490))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1490))->hd)->typs != 0){ tag_count ++;}
_temp1490=(( struct Cyc_List_List*) _check_null( _temp1490))->tl;} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* dles= 0; for( 0; _temp1008
!= 0;( _temp1008=(( struct Cyc_List_List*) _check_null( _temp1008))->tl,
_temp1486=(( struct Cyc_List_List*) _check_null( _temp1486))->tl)){ struct Cyc_Absyn_Exp*
cur_e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1008))->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1486))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);} dles=({ struct
Cyc_List_List* _temp1491=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1491->hd=( void*)({ struct _tuple4* _temp1492=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1492->f1= 0; _temp1492->f2= cur_e; _temp1492;});
_temp1491->tl= dles; _temp1491;});} dles=({ struct Cyc_List_List* _temp1493=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1493->hd=(
void*)({ struct _tuple4* _temp1494=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp1494->f1= 0; _temp1494->f2= tag_exp; _temp1494;}); _temp1493->tl=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);
_temp1493;});( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({
struct Cyc_Core_Opt* _temp1495=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1495->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp1485,
_tag_arr("_struct", sizeof( unsigned char), 8u)); _temp1495;}), dles, 0))->r));}}
else{ struct Cyc_List_List* _temp1496=({ struct Cyc_List_List* _temp1502=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1502->hd=(
void*) Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1488, Cyc_Toc_tag_sp,
0), Cyc_Absyn_var_exp( _temp1485, 0), 0); _temp1502->tl= 0; _temp1502;});{ int i=
1; for( 0; _temp1008 != 0;((( _temp1008=(( struct Cyc_List_List*) _check_null(
_temp1008))->tl, i ++)), _temp1486=(( struct Cyc_List_List*) _check_null(
_temp1486))->tl)){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1008))->hd; void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1486))->hd)).f2);
Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt* _temp1497= Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp1488, Cyc_Toc_fieldname( i), 0), cur_e, 0);
_temp1496=({ struct Cyc_List_List* _temp1498=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1498->hd=( void*) _temp1497; _temp1498->tl=
_temp1496; _temp1498;});}}}{ struct Cyc_Absyn_Stmt* _temp1499= Cyc_Absyn_exp_stmt(
_temp1488, 0); struct Cyc_Absyn_Stmt* _temp1500= Cyc_Absyn_seq_stmts((( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct Cyc_List_List*
_temp1501=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1501->hd=( void*) _temp1499; _temp1501->tl= _temp1496; _temp1501;})), 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1487, _temp1489, 0, _temp1500, 0), 0))->r));}} goto _LL801;} _LL867: goto
_LL801; _LL869: { void* t_c= Cyc_Toc_typ_to_c( _temp1010); if( _temp1012 != 0){
struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*) _check_null( _temp1012); Cyc_Toc_exp_to_c(
nv, rgn);( void*)( e->r=( void*)(( void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp(
_temp1010, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));} goto _LL801;} _LL871: Cyc_Toc_stmt_to_c(
nv, _temp1014); goto _LL801; _LL873:({ void* _temp1503[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("UnresolvedMem",
sizeof( unsigned char), 14u), _tag_arr( _temp1503, sizeof( void*), 0u));}); goto
_LL801; _LL875:({ void* _temp1504[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("compoundlit", sizeof(
unsigned char), 12u), _tag_arr( _temp1504, sizeof( void*), 0u));}); goto _LL801;
_LL877:({ void* _temp1505[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("codegen", sizeof( unsigned char), 8u),
_tag_arr( _temp1505, sizeof( void*), 0u));}); goto _LL801; _LL879:({ void*
_temp1506[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)(
_tag_arr("fill", sizeof( unsigned char), 5u), _tag_arr( _temp1506, sizeof( void*),
0u));}); goto _LL801; _LL801:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_if_eq_goto_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct _tagged_arr* succ_lab,
struct _tagged_arr* fail_lab){ return Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp(
e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt( fail_lab, 0),
0);} struct _tuple11{ struct Cyc_Toc_Env* f1; struct Cyc_List_List* f2; struct
Cyc_Absyn_Stmt* f3; } ; struct _tuple12{ struct _tuple0* f1; void* f2; } ;
struct _tuple13{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; static
struct _tuple11 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env* nv, struct _RegionHandle*
rgn, void* t, struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path, struct Cyc_Absyn_Pat*
p, struct _tagged_arr* succ_lab, struct _tagged_arr* fail_lab, struct Cyc_List_List*
decls){ struct Cyc_Absyn_Stmt* s;{ void* _temp1507=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp1543; struct Cyc_Absyn_Vardecl _temp1545; struct _tuple0* _temp1546; struct
Cyc_Absyn_Vardecl* _temp1548; int _temp1550; void* _temp1552; unsigned char
_temp1554; struct _tagged_arr _temp1556; struct Cyc_Absyn_Enumfield* _temp1558;
struct Cyc_Absyn_Enumdecl* _temp1560; struct Cyc_List_List* _temp1562; struct
Cyc_List_List* _temp1564; struct Cyc_Absyn_Tunionfield* _temp1566; struct Cyc_Absyn_Tuniondecl*
_temp1568; struct Cyc_Absyn_Pat* _temp1571; struct Cyc_Absyn_Pat _temp1573; void*
_temp1574; struct Cyc_List_List* _temp1576; struct Cyc_List_List* _temp1578;
struct Cyc_Absyn_Tunionfield* _temp1580; struct Cyc_Absyn_Tuniondecl* _temp1582;
struct Cyc_List_List* _temp1584; struct Cyc_List_List* _temp1586; struct Cyc_List_List*
_temp1588; struct Cyc_List_List* _temp1590; struct Cyc_Core_Opt* _temp1592;
struct Cyc_Absyn_Structdecl* _temp1594; struct Cyc_Absyn_Pat* _temp1596; _LL1509:
if(( unsigned int) _temp1507 > 2u?*(( int*) _temp1507) == Cyc_Absyn_Var_p: 0){
_LL1544: _temp1543=(( struct Cyc_Absyn_Var_p_struct*) _temp1507)->f1; _temp1545=*
_temp1543; _LL1547: _temp1546= _temp1545.name; goto _LL1510;} else{ goto _LL1511;}
_LL1511: if( _temp1507 ==( void*) Cyc_Absyn_Wild_p){ goto _LL1512;} else{ goto
_LL1513;} _LL1513: if(( unsigned int) _temp1507 > 2u?*(( int*) _temp1507) == Cyc_Absyn_Reference_p:
0){ _LL1549: _temp1548=(( struct Cyc_Absyn_Reference_p_struct*) _temp1507)->f1;
goto _LL1514;} else{ goto _LL1515;} _LL1515: if( _temp1507 ==( void*) Cyc_Absyn_Null_p){
goto _LL1516;} else{ goto _LL1517;} _LL1517: if(( unsigned int) _temp1507 > 2u?*((
int*) _temp1507) == Cyc_Absyn_Int_p: 0){ _LL1553: _temp1552=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1507)->f1; goto _LL1551; _LL1551: _temp1550=((
struct Cyc_Absyn_Int_p_struct*) _temp1507)->f2; goto _LL1518;} else{ goto
_LL1519;} _LL1519: if(( unsigned int) _temp1507 > 2u?*(( int*) _temp1507) == Cyc_Absyn_Char_p:
0){ _LL1555: _temp1554=(( struct Cyc_Absyn_Char_p_struct*) _temp1507)->f1; goto
_LL1520;} else{ goto _LL1521;} _LL1521: if(( unsigned int) _temp1507 > 2u?*((
int*) _temp1507) == Cyc_Absyn_Float_p: 0){ _LL1557: _temp1556=(( struct Cyc_Absyn_Float_p_struct*)
_temp1507)->f1; goto _LL1522;} else{ goto _LL1523;} _LL1523: if(( unsigned int)
_temp1507 > 2u?*(( int*) _temp1507) == Cyc_Absyn_Enum_p: 0){ _LL1561: _temp1560=((
struct Cyc_Absyn_Enum_p_struct*) _temp1507)->f1; goto _LL1559; _LL1559:
_temp1558=(( struct Cyc_Absyn_Enum_p_struct*) _temp1507)->f2; goto _LL1524;}
else{ goto _LL1525;} _LL1525: if(( unsigned int) _temp1507 > 2u?*(( int*)
_temp1507) == Cyc_Absyn_Tunion_p: 0){ _LL1569: _temp1568=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1507)->f1; goto _LL1567; _LL1567: _temp1566=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1507)->f2; goto _LL1565; _LL1565: _temp1564=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1507)->f3; goto _LL1563; _LL1563: _temp1562=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1507)->f4; if( _temp1562 == 0){ goto _LL1526;} else{ goto _LL1527;}} else{
goto _LL1527;} _LL1527: if(( unsigned int) _temp1507 > 2u?*(( int*) _temp1507)
== Cyc_Absyn_Pointer_p: 0){ _LL1572: _temp1571=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1507)->f1; _temp1573=* _temp1571; _LL1575: _temp1574=( void*) _temp1573.r;
if(( unsigned int) _temp1574 > 2u?*(( int*) _temp1574) == Cyc_Absyn_Tunion_p: 0){
_LL1583: _temp1582=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1574)->f1; goto
_LL1581; _LL1581: _temp1580=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1574)->f2;
goto _LL1579; _LL1579: _temp1578=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1574)->f3;
goto _LL1577; _LL1577: _temp1576=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1574)->f4;
goto _LL1570;} else{ goto _LL1529;}} else{ goto _LL1529;} _LL1570: if( _temp1576
!= 0){ goto _LL1528;} else{ goto _LL1529;} _LL1529: if(( unsigned int) _temp1507
> 2u?*(( int*) _temp1507) == Cyc_Absyn_Tunion_p: 0){ _LL1585: _temp1584=((
struct Cyc_Absyn_Tunion_p_struct*) _temp1507)->f4; goto _LL1530;} else{ goto
_LL1531;} _LL1531: if(( unsigned int) _temp1507 > 2u?*(( int*) _temp1507) == Cyc_Absyn_Tuple_p:
0){ _LL1587: _temp1586=(( struct Cyc_Absyn_Tuple_p_struct*) _temp1507)->f1; goto
_LL1532;} else{ goto _LL1533;} _LL1533: if(( unsigned int) _temp1507 > 2u?*((
int*) _temp1507) == Cyc_Absyn_Struct_p: 0){ _LL1595: _temp1594=(( struct Cyc_Absyn_Struct_p_struct*)
_temp1507)->f1; goto _LL1593; _LL1593: _temp1592=(( struct Cyc_Absyn_Struct_p_struct*)
_temp1507)->f2; goto _LL1591; _LL1591: _temp1590=(( struct Cyc_Absyn_Struct_p_struct*)
_temp1507)->f3; goto _LL1589; _LL1589: _temp1588=(( struct Cyc_Absyn_Struct_p_struct*)
_temp1507)->f4; goto _LL1534;} else{ goto _LL1535;} _LL1535: if(( unsigned int)
_temp1507 > 2u?*(( int*) _temp1507) == Cyc_Absyn_Pointer_p: 0){ _LL1597:
_temp1596=(( struct Cyc_Absyn_Pointer_p_struct*) _temp1507)->f1; goto _LL1536;}
else{ goto _LL1537;} _LL1537: if(( unsigned int) _temp1507 > 2u?*(( int*)
_temp1507) == Cyc_Absyn_UnknownId_p: 0){ goto _LL1538;} else{ goto _LL1539;}
_LL1539: if(( unsigned int) _temp1507 > 2u?*(( int*) _temp1507) == Cyc_Absyn_UnknownCall_p:
0){ goto _LL1540;} else{ goto _LL1541;} _LL1541: if(( unsigned int) _temp1507 >
2u?*(( int*) _temp1507) == Cyc_Absyn_UnknownFields_p: 0){ goto _LL1542;} else{
goto _LL1508;} _LL1510: nv= Cyc_Toc_add_varmap( nv, _temp1546, r); goto _LL1512;
_LL1512: s= Cyc_Absyn_goto_stmt( succ_lab, 0); goto _LL1508; _LL1514: { struct
_tuple0* _temp1598= Cyc_Toc_temp_var(); decls=({ struct Cyc_List_List* _temp1599=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1599->hd=( void*)({ struct _tuple12* _temp1600=( struct _tuple12*)
_region_malloc( rgn, sizeof( struct _tuple12)); _temp1600->f1= _temp1598;
_temp1600->f2= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq);
_temp1600;}); _temp1599->tl= decls; _temp1599;}); nv= Cyc_Toc_add_varmap( nv,
_temp1548->name, Cyc_Absyn_var_exp( _temp1598, 0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp1598, 0), Cyc_Absyn_address_exp( path, 0), 0), Cyc_Absyn_goto_stmt(
succ_lab, 0), 0); goto _LL1508;} _LL1516: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp(
0, 0), succ_lab, fail_lab); goto _LL1508; _LL1518: s= Cyc_Toc_if_eq_goto_stmt( r,
Cyc_Absyn_int_exp( _temp1552, _temp1550, 0), succ_lab, fail_lab); goto _LL1508;
_LL1520: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp( _temp1554, 0),
succ_lab, fail_lab); goto _LL1508; _LL1522: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp(
_temp1556, 0), succ_lab, fail_lab); goto _LL1508; _LL1524: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp1601=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp1601[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp1602; _temp1602.tag= Cyc_Absyn_Enum_e;
_temp1602.f1= _temp1558->name; _temp1602.f2=( struct Cyc_Absyn_Enumdecl*)
_temp1560; _temp1602.f3=( struct Cyc_Absyn_Enumfield*) _temp1558; _temp1602;});
_temp1601;}), 0), succ_lab, fail_lab); goto _LL1508; _LL1526: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp1566->name, 0); if( ! _temp1568->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL1508;} _LL1528: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1576);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp1580->name, _tag_arr("_struct",
sizeof( unsigned char), 8u)); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0); struct
Cyc_List_List* _temp1603=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1580->typs); struct Cyc_List_List*
_temp1604=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp1576); for( 0; _temp1604 != 0;((( _temp1604=((
struct Cyc_List_List*) _check_null( _temp1604))->tl, _temp1603=(( struct Cyc_List_List*)
_check_null( _temp1603))->tl)), cnt --)){ struct Cyc_Absyn_Pat* _temp1605=(
struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null( _temp1604))->hd;
if(( void*) _temp1605->r ==( void*) Cyc_Absyn_Wild_p){ continue;}{ void*
_temp1606=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1603))->hd)).f2;
struct _tuple0* _temp1607= Cyc_Toc_temp_var(); void* _temp1608=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1605->topt))->v; void* _temp1609= Cyc_Toc_typ_to_c(
_temp1608); struct _tagged_arr* _temp1610= Cyc_Toc_fresh_label(); struct Cyc_Absyn_Exp*
_temp1611= Cyc_Absyn_structarrow_exp( rcast, Cyc_Toc_fieldname( cnt), 0); if(
Cyc_Toc_is_void_star( Cyc_Toc_typ_to_c( _temp1606))){ _temp1611= Cyc_Absyn_cast_exp(
_temp1609, _temp1611, 0);} decls=({ struct Cyc_List_List* _temp1612=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1612->hd=( void*)({
struct _tuple12* _temp1613=( struct _tuple12*) _region_malloc( rgn, sizeof(
struct _tuple12)); _temp1613->f1= _temp1607; _temp1613->f2= _temp1609; _temp1613;});
_temp1612->tl= decls; _temp1612;});{ struct _tuple11 _temp1614= Cyc_Toc_xlate_pat(
nv, rgn, _temp1608, Cyc_Absyn_var_exp( _temp1607, 0), _temp1611, _temp1605,
succ_lab, fail_lab, decls); nv= _temp1614.f1; decls= _temp1614.f2;{ struct Cyc_Absyn_Stmt*
_temp1615= _temp1614.f3; struct Cyc_Absyn_Stmt* _temp1616= Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp1607, 0), _temp1611, 0); struct Cyc_Absyn_Stmt*
_temp1617= Cyc_Absyn_seq_stmt( _temp1616, _temp1615, 0); ss=({ struct Cyc_List_List*
_temp1618=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1618->hd=( void*) Cyc_Absyn_label_stmt( _temp1610, _temp1617, 0); _temp1618->tl=
ss; _temp1618;}); succ_lab= _temp1610;}}}} if( ss == 0){ ss=({ struct Cyc_List_List*
_temp1619=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1619->hd=( void*) Cyc_Absyn_goto_stmt( succ_lab, 0); _temp1619->tl= 0;
_temp1619;});}{ struct Cyc_Absyn_Exp* test_exp; if( _temp1582->is_xtunion){
struct Cyc_Absyn_Exp* e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(),
Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0);
struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp1580->name, 0); test_exp= Cyc_Absyn_eq_exp(
e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* _temp1620=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1582->fields))->v; for( 0; _temp1620 != 0; _temp1620=(( struct
Cyc_List_List*) _check_null( _temp1620))->tl){ struct Cyc_Absyn_Tunionfield*
_temp1621=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp1620))->hd; if( _temp1621->typs == 0){ max_tag ++;}}}{ struct Cyc_Absyn_Exp*
max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_t, r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp(
_temp1580->name, 0); test_exp= max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp(
e1, e, 0), 0): Cyc_Absyn_eq_exp( e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt(
test_exp, Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);
goto _LL1508;}} _LL1530: _temp1586= _temp1584; goto _LL1532; _LL1532: { struct
Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1586);{ struct Cyc_List_List* _temp1622=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1586); for(
0; _temp1622 != 0;( _temp1622=(( struct Cyc_List_List*) _check_null( _temp1622))->tl,
cnt --)){ struct Cyc_Absyn_Pat* _temp1623=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp1622))->hd; if(( void*) _temp1623->r ==( void*) Cyc_Absyn_Wild_p){
continue;}{ struct _tuple0* _temp1624= Cyc_Toc_temp_var(); void* _temp1625=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1623->topt))->v; struct
_tagged_arr* _temp1626= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1627=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1627->hd=( void*)({ struct _tuple12* _temp1628=( struct _tuple12*)
_region_malloc( rgn, sizeof( struct _tuple12)); _temp1628->f1= _temp1624;
_temp1628->f2= Cyc_Toc_typ_to_c( _temp1625); _temp1628;}); _temp1627->tl= decls;
_temp1627;});{ struct _tuple11 _temp1629= Cyc_Toc_xlate_pat( nv, rgn, _temp1625,
Cyc_Absyn_var_exp( _temp1624, 0), Cyc_Absyn_structmember_exp( path, Cyc_Toc_fieldname(
cnt), 0), _temp1623, succ_lab, fail_lab, decls); nv= _temp1629.f1; decls=
_temp1629.f2;{ struct Cyc_Absyn_Stmt* _temp1630= _temp1629.f3; struct Cyc_Absyn_Stmt*
_temp1631= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp1624, 0), Cyc_Absyn_structmember_exp( r, Cyc_Toc_fieldname( cnt), 0), 0),
_temp1630, 0); ss=({ struct Cyc_List_List* _temp1632=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1632->hd=( void*) Cyc_Absyn_label_stmt(
_temp1626, _temp1631, 0); _temp1632->tl= ss; _temp1632;}); succ_lab= _temp1626;}}}}}
if( ss == 0){ ss=({ struct Cyc_List_List* _temp1633=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1633->hd=( void*) Cyc_Absyn_goto_stmt(
succ_lab, 0); _temp1633->tl= 0; _temp1633;});} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL1508;} _LL1534: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
_temp1634=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp1588); for( 0; _temp1634 != 0; _temp1634=(( struct
Cyc_List_List*) _check_null( _temp1634))->tl){ struct _tuple13* _temp1635=(
struct _tuple13*)(( struct Cyc_List_List*) _check_null( _temp1634))->hd; struct
Cyc_Absyn_Pat* _temp1636=(* _temp1635).f2; if(( void*) _temp1636->r ==( void*)
Cyc_Absyn_Wild_p){ continue;}{ struct _tagged_arr* f;{ void* _temp1637=( void*)((
struct Cyc_List_List*) _check_null((* _temp1635).f1))->hd; struct _tagged_arr*
_temp1643; _LL1639: if(*(( int*) _temp1637) == Cyc_Absyn_FieldName){ _LL1644:
_temp1643=(( struct Cyc_Absyn_FieldName_struct*) _temp1637)->f1; goto _LL1640;}
else{ goto _LL1641;} _LL1641: goto _LL1642; _LL1640: f= _temp1643; goto _LL1638;
_LL1642:( int) _throw(( void*) Cyc_Toc_Match_error); _LL1638:;}{ struct _tuple0*
_temp1645= Cyc_Toc_temp_var(); void* _temp1646=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1636->topt))->v; void* _temp1647= Cyc_Toc_typ_to_c( _temp1646);
struct _tagged_arr* _temp1648= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1649=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1649->hd=( void*)({ struct _tuple12* _temp1650=( struct _tuple12*)
_region_malloc( rgn, sizeof( struct _tuple12)); _temp1650->f1= _temp1645;
_temp1650->f2= _temp1647; _temp1650;}); _temp1649->tl= decls; _temp1649;});{
struct _tuple11 _temp1651= Cyc_Toc_xlate_pat( nv, rgn, _temp1646, Cyc_Absyn_var_exp(
_temp1645, 0), Cyc_Absyn_structmember_exp( path, f, 0), _temp1636, succ_lab,
fail_lab, decls); nv= _temp1651.f1; decls= _temp1651.f2;{ struct Cyc_Absyn_Exp*
_temp1652= Cyc_Absyn_structmember_exp( r, f, 0); if( Cyc_Toc_is_void_star(( void*)((
struct Cyc_Absyn_Structfield*) _check_null( Cyc_Absyn_lookup_field(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1594->fields))->v, f)))->type)){
_temp1652= Cyc_Absyn_cast_exp( _temp1647, _temp1652, 0);}{ struct Cyc_Absyn_Stmt*
_temp1653= _temp1651.f3; struct Cyc_Absyn_Stmt* _temp1654= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1645, 0), _temp1652, 0),
_temp1653, 0); ss=({ struct Cyc_List_List* _temp1655=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1655->hd=( void*) Cyc_Absyn_label_stmt(
_temp1648, _temp1654, 0); _temp1655->tl= ss; _temp1655;}); succ_lab= _temp1648;}}}}}}}
if( ss == 0){ ss=({ struct Cyc_List_List* _temp1656=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1656->hd=( void*) Cyc_Absyn_goto_stmt(
succ_lab, 0); _temp1656->tl= 0; _temp1656;});} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL1508;} _LL1536: { struct _tuple0* _temp1657= Cyc_Toc_temp_var(); void*
_temp1658=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1596->topt))->v;
decls=({ struct Cyc_List_List* _temp1659=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp1659->hd=( void*)({ struct _tuple12*
_temp1660=( struct _tuple12*) _region_malloc( rgn, sizeof( struct _tuple12));
_temp1660->f1= _temp1657; _temp1660->f2= Cyc_Toc_typ_to_c( _temp1658); _temp1660;});
_temp1659->tl= decls; _temp1659;});{ struct _tuple11 _temp1661= Cyc_Toc_xlate_pat(
nv, rgn, _temp1658, Cyc_Absyn_var_exp( _temp1657, 0), Cyc_Absyn_deref_exp( path,
0), _temp1596, succ_lab, fail_lab, decls); nv= _temp1661.f1; decls= _temp1661.f2;{
struct Cyc_Absyn_Stmt* _temp1662= _temp1661.f3; struct Cyc_Absyn_Stmt* _temp1663=
Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1657, 0), Cyc_Absyn_deref_exp(
r, 0), 0), _temp1662, 0); if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp1663, 0);} else{ s= _temp1663;} goto _LL1508;}}} _LL1538: s=({
void* _temp1664[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknownid", sizeof(
unsigned char), 10u), _tag_arr( _temp1664, sizeof( void*), 0u));}); goto _LL1508;
_LL1540: s=({ void* _temp1665[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknowncall",
sizeof( unsigned char), 12u), _tag_arr( _temp1665, sizeof( void*), 0u));}); goto
_LL1508; _LL1542: s=({ void* _temp1666[ 0u]={};(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("unknownfields",
sizeof( unsigned char), 14u), _tag_arr( _temp1666, sizeof( void*), 0u));}); goto
_LL1508; _LL1508:;} return({ struct _tuple11 _temp1667; _temp1667.f1= nv;
_temp1667.f2= decls; _temp1667.f3= s; _temp1667;});} struct _tuple14{ struct
_tagged_arr* f1; struct _tagged_arr* f2; struct Cyc_Absyn_Switch_clause* f3; } ;
static struct _tuple14* Cyc_Toc_gen_label( struct _RegionHandle* r, struct Cyc_Absyn_Switch_clause*
sc){ return({ struct _tuple14* _temp1668=( struct _tuple14*) _region_malloc( r,
sizeof( struct _tuple14)); _temp1668->f1= Cyc_Toc_fresh_label(); _temp1668->f2=
Cyc_Toc_fresh_label(); _temp1668->f3= sc; _temp1668;});} static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* scs){ Cyc_Toc_exp_to_c( nv, e);{ void* _temp1669=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; int leave_as_switch;{ void*
_temp1670= Cyc_Tcutil_compress( _temp1669); _LL1672: if(( unsigned int)
_temp1670 > 4u?*(( int*) _temp1670) == Cyc_Absyn_IntType: 0){ goto _LL1673;}
else{ goto _LL1674;} _LL1674: if(( unsigned int) _temp1670 > 4u?*(( int*)
_temp1670) == Cyc_Absyn_EnumType: 0){ goto _LL1675;} else{ goto _LL1676;}
_LL1676: goto _LL1677; _LL1673: goto _LL1675; _LL1675: leave_as_switch= 1; goto
_LL1671; _LL1677: leave_as_switch= 0; goto _LL1671; _LL1671:;}{ struct Cyc_List_List*
_temp1678= scs; for( 0; _temp1678 != 0; _temp1678=(( struct Cyc_List_List*)
_check_null( _temp1678))->tl){ if(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp1678))->hd)->pat_vars))->v != 0? 1:(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp1678))->hd)->where_clause != 0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_arr* next_l=
Cyc_Toc_fresh_label();{ struct Cyc_List_List* _temp1679= scs; for( 0; _temp1679
!= 0; _temp1679=(( struct Cyc_List_List*) _check_null( _temp1679))->tl){ struct
Cyc_Absyn_Stmt* _temp1680=(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp1679))->hd)->body;(( struct Cyc_Absyn_Switch_clause*)(( struct
Cyc_List_List*) _check_null( _temp1679))->hd)->body= Cyc_Absyn_label_stmt(
next_l, _temp1680, 0); next_l= Cyc_Toc_fresh_label(); Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env(
nv, next_l), _temp1680);}} return;}{ struct _tuple0* v= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp( v, 0); struct Cyc_Absyn_Exp* path=
Cyc_Absyn_var_exp( v, 0); struct _tagged_arr* end_l= Cyc_Toc_fresh_label();
struct _RegionHandle _temp1681= _new_region(); struct _RegionHandle* rgn=&
_temp1681; _push_region( rgn);{ struct Cyc_List_List* lscs=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple14*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Toc_gen_label,
rgn, scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0;
struct Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp1682= lscs; for( 0;
_temp1682 != 0; _temp1682=(( struct Cyc_List_List*) _check_null( _temp1682))->tl){
struct Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null( _temp1682))->hd)).f3; struct _tagged_arr* fail_lab=(( struct Cyc_List_List*)
_check_null( _temp1682))->tl == 0? end_l:(*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1682))->tl))->hd)).f1;
struct _tagged_arr* succ_lab=(*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null( _temp1682))->hd)).f2; if( sc->where_clause == 0){ struct _tuple11
_temp1683= Cyc_Toc_xlate_pat( nv, rgn, _temp1669, r, path, sc->pattern, succ_lab,
fail_lab, decls); nvs=({ struct Cyc_List_List* _temp1684=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1684->hd=( void*)
_temp1683.f1; _temp1684->tl= nvs; _temp1684;}); decls= _temp1683.f2; test_stmts=({
struct Cyc_List_List* _temp1685=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp1685->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( _temp1682))->hd)).f1,
_temp1683.f3, 0); _temp1685->tl= test_stmts; _temp1685;});} else{ struct Cyc_Absyn_Exp*
_temp1686=( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause); struct
_tagged_arr* _temp1687= Cyc_Toc_fresh_label(); struct _tuple11 _temp1688= Cyc_Toc_xlate_pat(
nv, rgn, _temp1669, r, path, sc->pattern, _temp1687, fail_lab, decls); Cyc_Toc_exp_to_c(
_temp1688.f1, _temp1686); nvs=({ struct Cyc_List_List* _temp1689=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1689->hd=( void*)
_temp1688.f1; _temp1689->tl= nvs; _temp1689;}); decls= _temp1688.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp1686, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp1687, s2,
0); test_stmts=({ struct Cyc_List_List* _temp1690=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1690->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( _temp1682))->hd)).f1,
Cyc_Absyn_seq_stmt( _temp1688.f3, s3, 0), 0); _temp1690->tl= test_stmts;
_temp1690;});}}}}{ struct Cyc_Absyn_Stmt* _temp1691= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
test_stmts), 0); nvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt( 0); if( lscs != 0){
for( 0;(( struct Cyc_List_List*) _check_null( lscs))->tl != 0;( lscs=(( struct
Cyc_List_List*) _check_null( lscs))->tl, nvs=(( struct Cyc_List_List*)
_check_null( nvs))->tl)){ struct Cyc_Toc_Env* _temp1692=( struct Cyc_Toc_Env*)((
struct Cyc_List_List*) _check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env( _temp1692, end_l,(*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( lscs))->tl))->hd)).f2,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null(((*(( struct _tuple14*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f3)->pat_vars))->v,(
struct Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( nvs))->tl))->hd), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple14*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0),
0);}{ struct Cyc_Toc_Env* _temp1693=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*(( struct _tuple14*)((
struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body; Cyc_Toc_stmt_to_c(
Cyc_Toc_last_switchclause_env( _temp1693, end_l), s); clauses= Cyc_Absyn_seq_stmt(
clauses, Cyc_Absyn_label_stmt((*(( struct _tuple14*)(( struct Cyc_List_List*)
_check_null( lscs))->hd)).f2, s, 0), 0);}}{ struct Cyc_Absyn_Stmt* res= Cyc_Absyn_seq_stmt(
_temp1691, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt( end_l, Cyc_Absyn_skip_stmt(
0), 0), 0), 0); for( decls; decls != 0; decls=(( struct Cyc_List_List*)
_check_null( decls))->tl){ struct _tuple12 _temp1696; void* _temp1697; struct
_tuple0* _temp1699; struct _tuple12* _temp1694=( struct _tuple12*)(( struct Cyc_List_List*)
_check_null( decls))->hd; _temp1696=* _temp1694; _LL1700: _temp1699= _temp1696.f1;
goto _LL1698; _LL1698: _temp1697= _temp1696.f2; goto _LL1695; _LL1695: res= Cyc_Absyn_declare_stmt(
_temp1699, _temp1697, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}; _pop_region( rgn);}}} static
struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat*
p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt*
s); static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv, struct Cyc_Toc_Env*
init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt* s); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__npop_handler_e,({ struct Cyc_List_List* _temp1701=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1701[ 0]=({ struct Cyc_List_List
_temp1702; _temp1702.hd=( void*) Cyc_Absyn_uint_exp(( unsigned int)( n - 1), 0);
_temp1702.tl= 0; _temp1702;}); _temp1701;}), 0), 0);} void Cyc_Toc_do_npop_before(
int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt(( void*) s->r,
0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* s){ while( 1) { void* _temp1703=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1745; struct Cyc_Absyn_Stmt* _temp1747; struct Cyc_Absyn_Stmt* _temp1749;
struct Cyc_Absyn_Exp* _temp1751; struct Cyc_Absyn_Stmt* _temp1753; struct Cyc_Absyn_Stmt*
_temp1755; struct Cyc_Absyn_Exp* _temp1757; struct Cyc_Absyn_Stmt* _temp1759;
struct _tuple2 _temp1761; struct Cyc_Absyn_Exp* _temp1763; struct Cyc_Absyn_Stmt*
_temp1765; struct Cyc_Absyn_Stmt* _temp1767; struct Cyc_Absyn_Stmt* _temp1769;
struct Cyc_Absyn_Stmt* _temp1771; struct _tuple2 _temp1773; struct Cyc_Absyn_Exp*
_temp1775; struct _tuple2 _temp1777; struct Cyc_Absyn_Exp* _temp1779; struct Cyc_Absyn_Exp*
_temp1781; struct Cyc_List_List* _temp1783; struct Cyc_Absyn_Exp* _temp1785;
struct Cyc_Absyn_Switch_clause** _temp1787; struct Cyc_List_List* _temp1789;
struct Cyc_Absyn_Stmt* _temp1791; struct Cyc_Absyn_Decl* _temp1793; struct Cyc_Absyn_Stmt*
_temp1795; struct _tagged_arr* _temp1797; struct _tuple2 _temp1799; struct Cyc_Absyn_Exp*
_temp1801; struct Cyc_Absyn_Stmt* _temp1803; struct Cyc_List_List* _temp1805;
struct Cyc_Absyn_Stmt* _temp1807; struct Cyc_Absyn_Stmt* _temp1809; struct Cyc_Absyn_Vardecl*
_temp1811; struct Cyc_Absyn_Tvar* _temp1813; _LL1705: if( _temp1703 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL1706;} else{ goto _LL1707;} _LL1707: if((
unsigned int) _temp1703 > 1u?*(( int*) _temp1703) == Cyc_Absyn_Exp_s: 0){
_LL1746: _temp1745=(( struct Cyc_Absyn_Exp_s_struct*) _temp1703)->f1; goto
_LL1708;} else{ goto _LL1709;} _LL1709: if(( unsigned int) _temp1703 > 1u?*((
int*) _temp1703) == Cyc_Absyn_Seq_s: 0){ _LL1750: _temp1749=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1703)->f1; goto _LL1748; _LL1748: _temp1747=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1703)->f2; goto _LL1710;} else{ goto _LL1711;} _LL1711: if(( unsigned int)
_temp1703 > 1u?*(( int*) _temp1703) == Cyc_Absyn_Return_s: 0){ _LL1752:
_temp1751=(( struct Cyc_Absyn_Return_s_struct*) _temp1703)->f1; goto _LL1712;}
else{ goto _LL1713;} _LL1713: if(( unsigned int) _temp1703 > 1u?*(( int*)
_temp1703) == Cyc_Absyn_IfThenElse_s: 0){ _LL1758: _temp1757=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1703)->f1; goto _LL1756; _LL1756: _temp1755=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1703)->f2; goto _LL1754; _LL1754: _temp1753=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1703)->f3; goto _LL1714;} else{ goto _LL1715;} _LL1715: if(( unsigned int)
_temp1703 > 1u?*(( int*) _temp1703) == Cyc_Absyn_While_s: 0){ _LL1762: _temp1761=((
struct Cyc_Absyn_While_s_struct*) _temp1703)->f1; _LL1764: _temp1763= _temp1761.f1;
goto _LL1760; _LL1760: _temp1759=(( struct Cyc_Absyn_While_s_struct*) _temp1703)->f2;
goto _LL1716;} else{ goto _LL1717;} _LL1717: if(( unsigned int) _temp1703 > 1u?*((
int*) _temp1703) == Cyc_Absyn_Break_s: 0){ _LL1766: _temp1765=(( struct Cyc_Absyn_Break_s_struct*)
_temp1703)->f1; goto _LL1718;} else{ goto _LL1719;} _LL1719: if(( unsigned int)
_temp1703 > 1u?*(( int*) _temp1703) == Cyc_Absyn_Continue_s: 0){ _LL1768:
_temp1767=(( struct Cyc_Absyn_Continue_s_struct*) _temp1703)->f1; goto _LL1720;}
else{ goto _LL1721;} _LL1721: if(( unsigned int) _temp1703 > 1u?*(( int*)
_temp1703) == Cyc_Absyn_Goto_s: 0){ _LL1770: _temp1769=(( struct Cyc_Absyn_Goto_s_struct*)
_temp1703)->f2; goto _LL1722;} else{ goto _LL1723;} _LL1723: if(( unsigned int)
_temp1703 > 1u?*(( int*) _temp1703) == Cyc_Absyn_For_s: 0){ _LL1782: _temp1781=((
struct Cyc_Absyn_For_s_struct*) _temp1703)->f1; goto _LL1778; _LL1778: _temp1777=((
struct Cyc_Absyn_For_s_struct*) _temp1703)->f2; _LL1780: _temp1779= _temp1777.f1;
goto _LL1774; _LL1774: _temp1773=(( struct Cyc_Absyn_For_s_struct*) _temp1703)->f3;
_LL1776: _temp1775= _temp1773.f1; goto _LL1772; _LL1772: _temp1771=(( struct Cyc_Absyn_For_s_struct*)
_temp1703)->f4; goto _LL1724;} else{ goto _LL1725;} _LL1725: if(( unsigned int)
_temp1703 > 1u?*(( int*) _temp1703) == Cyc_Absyn_Switch_s: 0){ _LL1786:
_temp1785=(( struct Cyc_Absyn_Switch_s_struct*) _temp1703)->f1; goto _LL1784;
_LL1784: _temp1783=(( struct Cyc_Absyn_Switch_s_struct*) _temp1703)->f2; goto
_LL1726;} else{ goto _LL1727;} _LL1727: if(( unsigned int) _temp1703 > 1u?*((
int*) _temp1703) == Cyc_Absyn_Fallthru_s: 0){ _LL1790: _temp1789=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1703)->f1; goto _LL1788; _LL1788: _temp1787=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1703)->f2; goto _LL1728;} else{ goto _LL1729;} _LL1729: if(( unsigned int)
_temp1703 > 1u?*(( int*) _temp1703) == Cyc_Absyn_Decl_s: 0){ _LL1794: _temp1793=((
struct Cyc_Absyn_Decl_s_struct*) _temp1703)->f1; goto _LL1792; _LL1792:
_temp1791=(( struct Cyc_Absyn_Decl_s_struct*) _temp1703)->f2; goto _LL1730;}
else{ goto _LL1731;} _LL1731: if(( unsigned int) _temp1703 > 1u?*(( int*)
_temp1703) == Cyc_Absyn_Label_s: 0){ _LL1798: _temp1797=(( struct Cyc_Absyn_Label_s_struct*)
_temp1703)->f1; goto _LL1796; _LL1796: _temp1795=(( struct Cyc_Absyn_Label_s_struct*)
_temp1703)->f2; goto _LL1732;} else{ goto _LL1733;} _LL1733: if(( unsigned int)
_temp1703 > 1u?*(( int*) _temp1703) == Cyc_Absyn_Do_s: 0){ _LL1804: _temp1803=((
struct Cyc_Absyn_Do_s_struct*) _temp1703)->f1; goto _LL1800; _LL1800: _temp1799=((
struct Cyc_Absyn_Do_s_struct*) _temp1703)->f2; _LL1802: _temp1801= _temp1799.f1;
goto _LL1734;} else{ goto _LL1735;} _LL1735: if(( unsigned int) _temp1703 > 1u?*((
int*) _temp1703) == Cyc_Absyn_TryCatch_s: 0){ _LL1808: _temp1807=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1703)->f1; goto _LL1806; _LL1806: _temp1805=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1703)->f2; goto _LL1736;} else{ goto _LL1737;} _LL1737: if(( unsigned int)
_temp1703 > 1u?*(( int*) _temp1703) == Cyc_Absyn_Region_s: 0){ _LL1814:
_temp1813=(( struct Cyc_Absyn_Region_s_struct*) _temp1703)->f1; goto _LL1812;
_LL1812: _temp1811=(( struct Cyc_Absyn_Region_s_struct*) _temp1703)->f2; goto
_LL1810; _LL1810: _temp1809=(( struct Cyc_Absyn_Region_s_struct*) _temp1703)->f3;
goto _LL1738;} else{ goto _LL1739;} _LL1739: if(( unsigned int) _temp1703 > 1u?*((
int*) _temp1703) == Cyc_Absyn_SwitchC_s: 0){ goto _LL1740;} else{ goto _LL1741;}
_LL1741: if(( unsigned int) _temp1703 > 1u?*(( int*) _temp1703) == Cyc_Absyn_Cut_s:
0){ goto _LL1742;} else{ goto _LL1743;} _LL1743: if(( unsigned int) _temp1703 >
1u?*(( int*) _temp1703) == Cyc_Absyn_Splice_s: 0){ goto _LL1744;} else{ goto
_LL1704;} _LL1706: return; _LL1708: Cyc_Toc_exp_to_c( nv, _temp1745); return;
_LL1710: Cyc_Toc_stmt_to_c( nv, _temp1749); s= _temp1747; continue; _LL1712: {
struct Cyc_Core_Opt* topt= 0; if( _temp1751 != 0){ topt=({ struct Cyc_Core_Opt*
_temp1815=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1815->v=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*) _check_null( _temp1751))->topt))->v);
_temp1815;}); Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null(
_temp1751));} if( s->try_depth > 0){ if( topt != 0){ struct _tuple0* _temp1816=
Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* _temp1817= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( _temp1816, 0), 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( _temp1816,( void*)(( struct Cyc_Core_Opt*)
_check_null( topt))->v, _temp1751, Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler(
s->try_depth), _temp1817, 0), 0))->r));} else{ Cyc_Toc_do_npop_before( s->try_depth,
s);}} return;} _LL1714: Cyc_Toc_exp_to_c( nv, _temp1757); Cyc_Toc_stmt_to_c( nv,
_temp1755); s= _temp1753; continue; _LL1716: Cyc_Toc_exp_to_c( nv, _temp1763);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1759); return; _LL1718: if( nv->break_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( nv->break_lab))->v, 0))->r));}{ int
dest_depth= _temp1765 == 0? 0:(( struct Cyc_Absyn_Stmt*) _check_null( _temp1765))->try_depth;
Cyc_Toc_do_npop_before( s->try_depth - dest_depth, s); return;} _LL1720: if( nv->continue_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( nv->continue_lab))->v, 0))->r));} _temp1769=
_temp1767; goto _LL1722; _LL1722: Cyc_Toc_do_npop_before( s->try_depth -((
struct Cyc_Absyn_Stmt*) _check_null( _temp1769))->try_depth, s); return; _LL1724:
Cyc_Toc_exp_to_c( nv, _temp1781); Cyc_Toc_exp_to_c( nv, _temp1779); Cyc_Toc_exp_to_c(
nv, _temp1775); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp1771); return;
_LL1726: Cyc_Toc_xlate_switch( nv, s, _temp1785, _temp1783); return; _LL1728:
if( nv->fallthru_info == 0){( int) _throw(({ void* _temp1818[ 0u]={}; Cyc_Toc_toc_impos(
_tag_arr("fallthru in unexpected place", sizeof( unsigned char), 29u), _tag_arr(
_temp1818, sizeof( void*), 0u));}));}{ struct _tuple6 _temp1821; struct Cyc_Dict_Dict*
_temp1822; struct Cyc_List_List* _temp1824; struct _tagged_arr* _temp1826;
struct _tuple6* _temp1819=( struct _tuple6*)(( struct Cyc_Core_Opt*) _check_null(
nv->fallthru_info))->v; _temp1821=* _temp1819; _LL1827: _temp1826= _temp1821.f1;
goto _LL1825; _LL1825: _temp1824= _temp1821.f2; goto _LL1823; _LL1823: _temp1822=
_temp1821.f3; goto _LL1820; _LL1820: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt(
_temp1826, 0); Cyc_Toc_do_npop_before( s->try_depth -((*(( struct Cyc_Absyn_Switch_clause**)
_check_null( _temp1787)))->body)->try_depth, s2);{ struct Cyc_List_List*
_temp1828=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1824); struct Cyc_List_List* _temp1829=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( _temp1789); for( 0; _temp1828 != 0;( _temp1828=((
struct Cyc_List_List*) _check_null( _temp1828))->tl, _temp1829=(( struct Cyc_List_List*)
_check_null( _temp1829))->tl)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1829))->hd); s2= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key)) Cyc_Dict_lookup)( _temp1822,( struct _tuple0*)(( struct
Cyc_List_List*) _check_null( _temp1828))->hd),( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1829))->hd, 0), s2, 0);}( void*)( s->r=( void*)((
void*) s2->r)); return;}}} _LL1730:{ void* _temp1830=( void*) _temp1793->r;
struct Cyc_Absyn_Vardecl* _temp1840; int _temp1842; struct Cyc_Absyn_Exp*
_temp1844; struct Cyc_Core_Opt* _temp1846; struct Cyc_Absyn_Pat* _temp1848;
struct Cyc_List_List* _temp1850; _LL1832: if(*(( int*) _temp1830) == Cyc_Absyn_Var_d){
_LL1841: _temp1840=(( struct Cyc_Absyn_Var_d_struct*) _temp1830)->f1; goto
_LL1833;} else{ goto _LL1834;} _LL1834: if(*(( int*) _temp1830) == Cyc_Absyn_Let_d){
_LL1849: _temp1848=(( struct Cyc_Absyn_Let_d_struct*) _temp1830)->f1; goto
_LL1847; _LL1847: _temp1846=(( struct Cyc_Absyn_Let_d_struct*) _temp1830)->f3;
goto _LL1845; _LL1845: _temp1844=(( struct Cyc_Absyn_Let_d_struct*) _temp1830)->f4;
goto _LL1843; _LL1843: _temp1842=(( struct Cyc_Absyn_Let_d_struct*) _temp1830)->f5;
goto _LL1835;} else{ goto _LL1836;} _LL1836: if(*(( int*) _temp1830) == Cyc_Absyn_Letv_d){
_LL1851: _temp1850=(( struct Cyc_Absyn_Letv_d_struct*) _temp1830)->f1; goto
_LL1837;} else{ goto _LL1838;} _LL1838: goto _LL1839; _LL1833: { struct Cyc_Toc_Env*
_temp1852= Cyc_Toc_add_varmap( nv, _temp1840->name, Cyc_Absyn_varb_exp(
_temp1840->name,( void*)({ struct Cyc_Absyn_Local_b_struct* _temp1853=( struct
Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp1853[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp1854; _temp1854.tag= Cyc_Absyn_Local_b;
_temp1854.f1= _temp1840; _temp1854;}); _temp1853;}), 0)); Cyc_Toc_local_decl_to_c(
_temp1852, _temp1852, _temp1840, _temp1791); goto _LL1831;} _LL1835:{ void*
_temp1855=( void*) _temp1848->r; struct Cyc_Absyn_Vardecl* _temp1861; _LL1857:
if(( unsigned int) _temp1855 > 2u?*(( int*) _temp1855) == Cyc_Absyn_Var_p: 0){
_LL1862: _temp1861=(( struct Cyc_Absyn_Var_p_struct*) _temp1855)->f1; goto
_LL1858;} else{ goto _LL1859;} _LL1859: goto _LL1860; _LL1858: { struct _tuple0*
old_name= _temp1861->name; struct _tuple0* new_name= Cyc_Toc_temp_var();
_temp1861->name= new_name; _temp1861->initializer=( struct Cyc_Absyn_Exp*)
_temp1844;( void*)( _temp1793->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp1863=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1863[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp1864; _temp1864.tag= Cyc_Absyn_Var_d;
_temp1864.f1= _temp1861; _temp1864;}); _temp1863;})));{ struct Cyc_Toc_Env*
_temp1865= Cyc_Toc_add_varmap( nv, old_name, Cyc_Absyn_varb_exp( new_name,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp1866=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp1866[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp1867; _temp1867.tag= Cyc_Absyn_Local_b; _temp1867.f1=
_temp1861; _temp1867;}); _temp1866;}), 0)); Cyc_Toc_local_decl_to_c( _temp1865,
nv, _temp1861, _temp1791); goto _LL1856;}} _LL1860:( void*)( s->r=( void*)((
void*)( Cyc_Toc_letdecl_to_c( nv, _temp1848, _temp1846, _temp1844, _temp1842,
_temp1791))->r)); goto _LL1856; _LL1856:;} goto _LL1831; _LL1837: { struct Cyc_List_List*
_temp1868=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1850); if( _temp1868 == 0){({ void* _temp1869[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("empty Letv_d",
sizeof( unsigned char), 13u), _tag_arr( _temp1869, sizeof( void*), 0u));});}(
void*)( _temp1793->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp1870=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1870[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp1871; _temp1871.tag= Cyc_Absyn_Var_d;
_temp1871.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp1868))->hd; _temp1871;}); _temp1870;}))); _temp1868=(( struct Cyc_List_List*)
_check_null( _temp1868))->tl; for( 0; _temp1868 != 0; _temp1868=(( struct Cyc_List_List*)
_check_null( _temp1868))->tl){ struct Cyc_Absyn_Decl* _temp1872= Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp1873=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp1873[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp1874; _temp1874.tag= Cyc_Absyn_Var_d; _temp1874.f1=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp1868))->hd; _temp1874;}); _temp1873;}),
0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_decl_stmt( _temp1872, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));} Cyc_Toc_stmt_to_c( nv, s); goto _LL1831;} _LL1839:({
void* _temp1875[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("bad nested declaration within function", sizeof(
unsigned char), 39u), _tag_arr( _temp1875, sizeof( void*), 0u));}); goto _LL1831;
_LL1831:;} return; _LL1732: s= _temp1795; continue; _LL1734: Cyc_Toc_stmt_to_c(
Cyc_Toc_loop_env( nv), _temp1803); Cyc_Toc_exp_to_c( nv, _temp1801); return;
_LL1736: { struct _tuple0* h_var= Cyc_Toc_temp_var(); struct _tuple0* e_var= Cyc_Toc_temp_var();
struct _tuple0* was_thrown_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* h_exp=
Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp(
e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); e_exp->topt=({ struct Cyc_Core_Opt* _temp1876=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1876->v=( void*) e_typ; _temp1876;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp1807);{
struct Cyc_Absyn_Stmt* _temp1877= Cyc_Absyn_seq_stmt( _temp1807, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( Cyc_Toc__pop_handler_e, 0, 0), 0), 0); struct _tuple0*
_temp1878= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp1879= Cyc_Absyn_var_exp(
_temp1878, 0); struct Cyc_Absyn_Vardecl* _temp1880= Cyc_Absyn_new_vardecl(
_temp1878, Cyc_Absyn_exn_typ, 0); _temp1879->topt=({ struct Cyc_Core_Opt*
_temp1881=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1881->v=( void*) Cyc_Absyn_exn_typ; _temp1881;});{ struct Cyc_Absyn_Pat*
_temp1882=({ struct Cyc_Absyn_Pat* _temp1898=( struct Cyc_Absyn_Pat*) GC_malloc(
sizeof( struct Cyc_Absyn_Pat)); _temp1898->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp1900=( struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp1900[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp1901; _temp1901.tag= Cyc_Absyn_Var_p;
_temp1901.f1= _temp1880; _temp1901;}); _temp1900;})); _temp1898->topt=({ struct
Cyc_Core_Opt* _temp1899=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1899->v=( void*) Cyc_Absyn_exn_typ; _temp1899;}); _temp1898->loc= 0;
_temp1898;}); struct Cyc_Absyn_Exp* _temp1883= Cyc_Absyn_throw_exp( _temp1879, 0);
_temp1883->topt=({ struct Cyc_Core_Opt* _temp1884=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1884->v=( void*)(( void*) Cyc_Absyn_VoidType);
_temp1884;});{ struct Cyc_Absyn_Stmt* _temp1885= Cyc_Absyn_exp_stmt( _temp1883,
0); struct Cyc_Absyn_Switch_clause* _temp1886=({ struct Cyc_Absyn_Switch_clause*
_temp1895=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp1895->pattern= _temp1882; _temp1895->pat_vars=({ struct Cyc_Core_Opt*
_temp1896=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1896->v=( void*)({ struct Cyc_List_List* _temp1897=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1897->hd=( void*) _temp1880;
_temp1897->tl= 0; _temp1897;}); _temp1896;}); _temp1895->where_clause= 0;
_temp1895->body= _temp1885; _temp1895->loc= 0; _temp1895;}); struct Cyc_Absyn_Stmt*
_temp1887= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp1805,({ struct Cyc_List_List*
_temp1894=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1894->hd=( void*) _temp1886; _temp1894->tl= 0; _temp1894;})), 0); Cyc_Toc_stmt_to_c(
nv, _temp1887);{ struct Cyc_Absyn_Exp* _temp1888= Cyc_Absyn_fncall_exp( Cyc_Toc_setjmp_e,({
struct Cyc_List_List* _temp1893=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1893->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp1893->tl= 0; _temp1893;}), 0); struct Cyc_Absyn_Stmt*
_temp1889= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__push_handler_e,({
struct Cyc_List_List* _temp1892=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1892->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0);
_temp1892->tl= 0; _temp1892;}), 0), 0); struct Cyc_Absyn_Exp* _temp1890= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 0, 0); struct Cyc_Absyn_Exp* _temp1891= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 1, 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp1889, Cyc_Absyn_declare_stmt(
was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp1890, Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt( _temp1888, Cyc_Absyn_assign_stmt( was_thrown_exp,
_temp1891, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp1877, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, Cyc_Toc__exn_thrown_e,
0), _temp1887, 0), 0), 0), 0), 0), 0))->r)); return;}}}}} _LL1738: { void*
rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); struct
_tuple0* rh_var= Cyc_Toc_temp_var(); struct _tuple0* x_var= _temp1811->name;
struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp( rh_var, 0); struct Cyc_Absyn_Exp*
x_exp= Cyc_Absyn_var_exp( x_var, 0); Cyc_Toc_stmt_to_c( Cyc_Toc_add_varmap( nv,
x_var, x_exp), _temp1809);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_fncall_exp( Cyc_Toc__new_region_e,
0, 0), Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp( rh_exp, 0), Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__push_region_e,({ struct Cyc_Absyn_Exp* _temp1902[ 1u]; _temp1902[ 0u]=
x_exp;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp1902, sizeof( struct Cyc_Absyn_Exp*), 1u));}), 0), 0), Cyc_Absyn_seq_stmt(
_temp1809, Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__pop_region_e,({
struct Cyc_Absyn_Exp* _temp1903[ 1u]; _temp1903[ 0u]= x_exp;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp1903, sizeof( struct Cyc_Absyn_Exp*),
1u));}), 0), 0), 0), 0), 0), 0))->r)); return;} _LL1740:({ void* _temp1904[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_toc_impos)(
_tag_arr("switchC_s", sizeof( unsigned char), 10u), _tag_arr( _temp1904, sizeof(
void*), 0u));}); return; _LL1742:({ void* _temp1905[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("cut", sizeof(
unsigned char), 4u), _tag_arr( _temp1905, sizeof( void*), 0u));}); return;
_LL1744:({ void* _temp1906[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_unimp)( _tag_arr("splice", sizeof( unsigned char), 7u),
_tag_arr( _temp1906, sizeof( void*), 0u));}); return; _LL1704:;}} struct
_tuple15{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ;
static void Cyc_Toc_fndecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl*
f){ f->tvs= 0; f->effect= 0; f->rgn_po= 0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c((
void*) f->ret_type));{ struct Cyc_List_List* _temp1907= f->args; for( 0;
_temp1907 != 0; _temp1907=(( struct Cyc_List_List*) _check_null( _temp1907))->tl){(*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( _temp1907))->hd)).f3=
Cyc_Toc_typ_to_c((*(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
_temp1907))->hd)).f3);{ struct _tuple0* _temp1908=({ struct _tuple0* _temp1909=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp1909->f1=( void*) Cyc_Absyn_Loc_n;
_temp1909->f2=(*(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
_temp1907))->hd)).f1; _temp1909;}); nv= Cyc_Toc_add_varmap( nv, _temp1908, Cyc_Absyn_var_exp(
_temp1908, 0));}}} if(( unsigned int) f->cyc_varargs?(( struct Cyc_Absyn_VarargInfo*)
_check_null( f->cyc_varargs))->name != 0: 0){ int _temp1912; void* _temp1914;
void* _temp1916; struct Cyc_Absyn_Tqual _temp1918; struct Cyc_Core_Opt*
_temp1920; struct Cyc_Absyn_VarargInfo _temp1910=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( f->cyc_varargs)); _LL1921: _temp1920= _temp1910.name; goto _LL1919;
_LL1919: _temp1918= _temp1910.tq; goto _LL1917; _LL1917: _temp1916=( void*)
_temp1910.type; goto _LL1915; _LL1915: _temp1914=( void*) _temp1910.rgn; goto
_LL1913; _LL1913: _temp1912= _temp1910.inject; goto _LL1911; _LL1911: { void*
_temp1922= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ( _temp1916, _temp1914,
_temp1918)); struct _tuple0* _temp1923=({ struct _tuple0* _temp1926=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp1926->f1=( void*) Cyc_Absyn_Loc_n;
_temp1926->f2=( struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null(
_temp1920))->v; _temp1926;}); f->args=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( f->args,({ struct Cyc_List_List*
_temp1924=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1924->hd=( void*)({ struct _tuple15* _temp1925=( struct _tuple15*)
GC_malloc( sizeof( struct _tuple15)); _temp1925->f1=( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( _temp1920))->v; _temp1925->f2= _temp1918;
_temp1925->f3= _temp1922; _temp1925;}); _temp1924->tl= 0; _temp1924;})); nv= Cyc_Toc_add_varmap(
nv, _temp1923, Cyc_Absyn_var_exp( _temp1923, 0)); f->cyc_varargs= 0;}}{ struct
Cyc_List_List* _temp1927=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( f->param_vardecls))->v; for( 0; _temp1927 != 0; _temp1927=(( struct
Cyc_List_List*) _check_null( _temp1927))->tl){( void*)((( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp1927))->hd)->type=( void*) Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp1927))->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel( nv), f->body);}
static void* Cyc_Toc_scope_to_c( void* s){ void* _temp1928= s; _LL1930: if(
_temp1928 ==( void*) Cyc_Absyn_Abstract){ goto _LL1931;} else{ goto _LL1932;}
_LL1932: if( _temp1928 ==( void*) Cyc_Absyn_ExternC){ goto _LL1933;} else{ goto
_LL1934;} _LL1934: goto _LL1935; _LL1931: return( void*) Cyc_Absyn_Public;
_LL1933: return( void*) Cyc_Absyn_Extern; _LL1935: return s; _LL1929:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp1936=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; if( Cyc_Toc_structs_so_far == 0){
Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp1937=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1937->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp1937;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp1938=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(*
_temp1936).f2); if( _temp1938 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp1936).f2, s));} else{ if(((
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Core_Opt*) _check_null( _temp1938))->v)->fields
== 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(* _temp1936).f2,
s)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)((
void*) Cyc_Absyn_Public)); s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp1939=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1939->v=(
void*) _temp1936; _temp1939;}); if( s->fields != 0){ if( seen_defn_before){ s->fields=
0;} else{ struct Cyc_List_List* _temp1940=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; for( 0; _temp1940 != 0; _temp1940=(( struct Cyc_List_List*)
_check_null( _temp1940))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp1940))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1940))->hd)->type));}}}}} struct Cyc_Core_Opt* Cyc_Toc_unions_so_far= 0;
static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl* u){ struct
_tuple0* _temp1941=( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( u->name))->v;
if( Cyc_Toc_unions_so_far == 0){ Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt*
_temp1942=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1942->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp1942;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp1943=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp1941).f2); if( _temp1943 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp1941).f2, u));} else{ if(((
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Core_Opt*) _check_null( _temp1943))->v)->fields
== 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(* _temp1941).f2,
u)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)( u->sc=( void*)((
void*) Cyc_Absyn_Public)); u->tvs= 0; u->name=({ struct Cyc_Core_Opt* _temp1944=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1944->v=(
void*) _temp1941; _temp1944;}); if( u->fields != 0){ if( seen_defn_before){ u->fields=
0;} else{ struct Cyc_List_List* _temp1945=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( u->fields))->v; for( 0; _temp1945 != 0; _temp1945=(( struct Cyc_List_List*)
_check_null( _temp1945))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp1945))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1945))->hd)->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp1946=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1946->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp1946;});}{ struct
_tuple0* _temp1947= tud->name; if( tud->fields == 0){ return;} if((( int(*)(
struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)(( struct Cyc_Set_Set*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v, _temp1947)){
return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v=(
void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tuple0* elt))
Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp1947));{ int tag_count= 0; int box_count= 0; struct Cyc_List_List*
_temp1948=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( tud->fields))->v;
for( 0; _temp1948 != 0; _temp1948=(( struct Cyc_List_List*) _check_null(
_temp1948))->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1948))->hd; if( f->typs == 0){ struct
Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp( tag_count
++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name, Cyc_Absyn_sint_t,
eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp1949; _temp1949.q_const= 1;
_temp1949.q_volatile= 0; _temp1949.q_restrict= 0; _temp1949;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp1950=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1950->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp1951=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp1951[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp1952; _temp1952.tag= Cyc_Absyn_Var_d; _temp1952.f1= vd; _temp1952;});
_temp1951;}), 0); _temp1950->tl= Cyc_Toc_result_decls; _temp1950;});} else{
struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp1953; _temp1953.q_const=
1; _temp1953.q_volatile= 0; _temp1953.q_restrict= 0; _temp1953;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp1954=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1954->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp1955=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp1955[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp1956; _temp1956.tag= Cyc_Absyn_Var_d; _temp1956.f1= vd; _temp1956;});
_temp1955;}), 0); _temp1954->tl= Cyc_Toc_result_decls; _temp1954;});{ struct Cyc_List_List*
_temp1957= 0; int i= 1;{ struct Cyc_List_List* _temp1958= f->typs; for( 0;
_temp1958 != 0;( _temp1958=(( struct Cyc_List_List*) _check_null( _temp1958))->tl,
i ++)){ struct _tagged_arr* _temp1959= Cyc_Toc_fieldname( i); struct Cyc_Absyn_Structfield*
_temp1960=({ struct Cyc_Absyn_Structfield* _temp1962=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp1962->name= _temp1959;
_temp1962->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null(
_temp1958))->hd)).f1; _temp1962->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1958))->hd)).f2);
_temp1962->width= 0; _temp1962->attributes= 0; _temp1962;}); _temp1957=({ struct
Cyc_List_List* _temp1961=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1961->hd=( void*) _temp1960; _temp1961->tl= _temp1957; _temp1961;});}}
_temp1957=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp1957); _temp1957=({ struct Cyc_List_List* _temp1963=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1963->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp1964=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp1964->name= Cyc_Toc_tag_sp; _temp1964->tq= Cyc_Toc_mt_tq; _temp1964->type=(
void*) Cyc_Absyn_sint_t; _temp1964->width= 0; _temp1964->attributes= 0;
_temp1964;}); _temp1963->tl= _temp1957; _temp1963;});{ struct Cyc_Absyn_Structdecl*
_temp1965=({ struct Cyc_Absyn_Structdecl* _temp1969=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp1969->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp1969->name=({ struct Cyc_Core_Opt* _temp1971=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1971->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp1971;}); _temp1969->tvs= 0; _temp1969->fields=({ struct Cyc_Core_Opt*
_temp1970=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1970->v=( void*) _temp1957; _temp1970;}); _temp1969->attributes= 0;
_temp1969;}); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp1966=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1966->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp1967=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp1967[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp1968; _temp1968.tag= Cyc_Absyn_Struct_d;
_temp1968.f1= _temp1965; _temp1968;}); _temp1967;}), 0); _temp1966->tl= Cyc_Toc_result_decls;
_temp1966;});}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far == 0){ Cyc_Toc_xtunion_fields_so_far=({ struct Cyc_Core_Opt*
_temp1972=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1972->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp1972;});} if( xd->fields
== 0){ return;}{ struct _tuple0* _temp1973= xd->name; struct Cyc_List_List*
_temp1974=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( xd->fields))->v;
for( 0; _temp1974 != 0; _temp1974=(( struct Cyc_List_List*) _check_null(
_temp1974))->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1974))->hd; struct _tagged_arr* fn=(* f->name).f2;
struct Cyc_Absyn_Exp* _temp1975= Cyc_Absyn_uint_exp( _get_arr_size(* fn, sizeof(
unsigned char)) + 4, 0); struct Cyc_Absyn_ArrayType_struct* _temp1976=({ struct
Cyc_Absyn_ArrayType_struct* _temp2026=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2026[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2027; _temp2027.tag= Cyc_Absyn_ArrayType;
_temp2027.f1=( void*) Cyc_Absyn_uchar_t; _temp2027.f2= Cyc_Toc_mt_tq; _temp2027.f3=(
struct Cyc_Absyn_Exp*) _temp1975; _temp2027;}); _temp2026;}); struct Cyc_Core_Opt*
_temp1977=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tuple0*
key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct Cyc_Core_Opt
_temp1985; int _temp1986; _LL1979: if( _temp1977 == 0){ goto _LL1980;} else{
goto _LL1981;} _LL1981: if( _temp1977 == 0){ goto _LL1983;} else{ _temp1985=*
_temp1977; _LL1987: _temp1986=( int) _temp1985.v; if( _temp1986 == 0){ goto
_LL1982;} else{ goto _LL1983;}} _LL1983: goto _LL1984; _LL1980: { struct Cyc_Absyn_Exp*
initopt= 0; if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){ initopt=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_string_exp(( struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct
_temp1993; _temp1993.tag= Cyc_Stdio_String_pa; _temp1993.f1=( struct _tagged_arr)*
fn;{ struct Cyc_Stdio_Int_pa_struct _temp1992; _temp1992.tag= Cyc_Stdio_Int_pa;
_temp1992.f1=( int)(( unsigned int) 0);{ struct Cyc_Stdio_Int_pa_struct
_temp1991; _temp1991.tag= Cyc_Stdio_Int_pa; _temp1991.f1=( int)(( unsigned int)
0);{ struct Cyc_Stdio_Int_pa_struct _temp1990; _temp1990.tag= Cyc_Stdio_Int_pa;
_temp1990.f1=( int)(( unsigned int) 0);{ struct Cyc_Stdio_Int_pa_struct
_temp1989; _temp1989.tag= Cyc_Stdio_Int_pa; _temp1989.f1=( int)(( unsigned int)
0);{ void* _temp1988[ 5u]={& _temp1989,& _temp1990,& _temp1991,& _temp1992,&
_temp1993}; Cyc_Stdio_aprintf( _tag_arr("%c%c%c%c%s", sizeof( unsigned char), 11u),
_tag_arr( _temp1988, sizeof( void*), 5u));}}}}}}), 0);}{ struct Cyc_Absyn_Vardecl*
_temp1994= Cyc_Absyn_new_vardecl( f->name,( void*) _temp1976, initopt);( void*)(
_temp1994->sc=( void*)(( void*) f->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp1995=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1995->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp1996=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1996[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp1997; _temp1997.tag= Cyc_Absyn_Var_d;
_temp1997.f1= _temp1994; _temp1997;}); _temp1996;}), 0); _temp1995->tl= Cyc_Toc_result_decls;
_temp1995;});( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key,
int data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc !=( void*)
Cyc_Absyn_Extern)); if( f->typs != 0){ struct Cyc_List_List* fields= 0; int i= 1;{
struct Cyc_List_List* _temp1998= f->typs; for( 0; _temp1998 != 0;( _temp1998=((
struct Cyc_List_List*) _check_null( _temp1998))->tl, i ++)){ struct _tagged_arr*
_temp1999=({ struct _tagged_arr* _temp2003=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr)); _temp2003[ 0]=( struct _tagged_arr)({ struct Cyc_Stdio_Int_pa_struct
_temp2005; _temp2005.tag= Cyc_Stdio_Int_pa; _temp2005.f1=( int)(( unsigned int)
i);{ void* _temp2004[ 1u]={& _temp2005}; Cyc_Stdio_aprintf( _tag_arr("f%d",
sizeof( unsigned char), 4u), _tag_arr( _temp2004, sizeof( void*), 1u));}});
_temp2003;}); struct Cyc_Absyn_Structfield* _temp2000=({ struct Cyc_Absyn_Structfield*
_temp2002=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2002->name= _temp1999; _temp2002->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1998))->hd)).f1; _temp2002->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1998))->hd)).f2);
_temp2002->width= 0; _temp2002->attributes= 0; _temp2002;}); fields=({ struct
Cyc_List_List* _temp2001=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2001->hd=( void*) _temp2000; _temp2001->tl= fields; _temp2001;});}} fields=({
struct Cyc_List_List* _temp2006=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2006->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2007=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2007->name= Cyc_Toc_tag_sp; _temp2007->tq= Cyc_Toc_mt_tq; _temp2007->type=(
void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t, Cyc_Toc_mt_tq); _temp2007->width=
0; _temp2007->attributes= 0; _temp2007;}); _temp2006->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields); _temp2006;});{ struct Cyc_Absyn_Structdecl*
_temp2008=({ struct Cyc_Absyn_Structdecl* _temp2012=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2012->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2012->name=({ struct Cyc_Core_Opt* _temp2014=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2014->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp2014;}); _temp2012->tvs= 0; _temp2012->fields=({ struct Cyc_Core_Opt*
_temp2013=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2013->v=( void*) fields; _temp2013;}); _temp2012->attributes= 0; _temp2012;});
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2009=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2009->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2010=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2010[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2011; _temp2011.tag= Cyc_Absyn_Struct_d;
_temp2011.f1= _temp2008; _temp2011;}); _temp2010;}), 0); _temp2009->tl= Cyc_Toc_result_decls;
_temp2009;});}} goto _LL1978;}} _LL1982: if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2015= Cyc_Absyn_string_exp(( struct _tagged_arr)({
struct Cyc_Stdio_String_pa_struct _temp2025; _temp2025.tag= Cyc_Stdio_String_pa;
_temp2025.f1=( struct _tagged_arr)* fn;{ struct Cyc_Stdio_Int_pa_struct
_temp2024; _temp2024.tag= Cyc_Stdio_Int_pa; _temp2024.f1=( int)(( unsigned int)
0);{ struct Cyc_Stdio_Int_pa_struct _temp2023; _temp2023.tag= Cyc_Stdio_Int_pa;
_temp2023.f1=( int)(( unsigned int) 0);{ struct Cyc_Stdio_Int_pa_struct
_temp2022; _temp2022.tag= Cyc_Stdio_Int_pa; _temp2022.f1=( int)(( unsigned int)
0);{ struct Cyc_Stdio_Int_pa_struct _temp2021; _temp2021.tag= Cyc_Stdio_Int_pa;
_temp2021.f1=( int)(( unsigned int) 0);{ void* _temp2020[ 5u]={& _temp2021,&
_temp2022,& _temp2023,& _temp2024,& _temp2025}; Cyc_Stdio_aprintf( _tag_arr("%c%c%c%c%s",
sizeof( unsigned char), 11u), _tag_arr( _temp2020, sizeof( void*), 5u));}}}}}}),
0); struct Cyc_Absyn_Vardecl* _temp2016= Cyc_Absyn_new_vardecl( f->name,( void*)
_temp1976,( struct Cyc_Absyn_Exp*) _temp2015);( void*)( _temp2016->sc=( void*)((
void*) f->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2017=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2017->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2018=( struct
Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2018[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2019; _temp2019.tag= Cyc_Absyn_Var_d;
_temp2019.f1= _temp2016; _temp2019;}); _temp2018;}), 0); _temp2017->tl= Cyc_Toc_result_decls;
_temp2017;});( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key,
int data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));} goto _LL1978;
_LL1984: goto _LL1978; _LL1978:;}}} static void Cyc_Toc_enumdecl_to_c( struct
Cyc_Toc_Env* nv, struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2028= ed->name;(
void*)( ed->sc=( void*)(( void*) Cyc_Absyn_Public)); if( ed->fields != 0){
struct Cyc_List_List* _temp2029=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ed->fields))->v; for( 0; _temp2029 != 0; _temp2029=(( struct Cyc_List_List*)
_check_null( _temp2029))->tl){ if((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2029))->hd)->tag != 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2029))->hd)->tag));(( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_List_List*) _check_null( _temp2029))->hd)->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
i, 0);}}}} static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv,
struct Cyc_Toc_Env* init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt*
s){( void*)( vd->type=( void*) Cyc_Toc_typ_to_c_array(( void*) vd->type)); Cyc_Toc_stmt_to_c(
body_nv, s); if( vd->initializer != 0){ struct Cyc_Absyn_Exp* init=( struct Cyc_Absyn_Exp*)
_check_null( vd->initializer); void* _temp2030=( void*) init->r; struct Cyc_Absyn_Exp*
_temp2036; struct Cyc_Absyn_Exp* _temp2038; struct Cyc_Absyn_Vardecl* _temp2040;
_LL2032: if(*(( int*) _temp2030) == Cyc_Absyn_Comprehension_e){ _LL2041:
_temp2040=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp2030)->f1; goto
_LL2039; _LL2039: _temp2038=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2030)->f2; goto _LL2037; _LL2037: _temp2036=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2030)->f3; goto _LL2033;} else{ goto _LL2034;} _LL2034: goto _LL2035;
_LL2033: vd->initializer= 0;( void*)( s->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
init_nv, Cyc_Absyn_var_exp( vd->name, 0), _temp2040, _temp2038, _temp2036, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r)); goto _LL2031; _LL2035: if(( void*) vd->sc ==( void*)
Cyc_Absyn_Static){ struct Cyc_Toc_Env* _temp2042= Cyc_Toc_copy_env( init_nv);
_temp2042->toplevel= 1; Cyc_Toc_exp_to_c( _temp2042, init);} else{ Cyc_Toc_exp_to_c(
init_nv, init);} goto _LL2031; _LL2031:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt,
struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)((
struct Cyc_Core_Opt*) _check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct
_tuple0* x= Cyc_Toc_temp_var(); struct _tagged_arr* succ_lab= Cyc_Toc_fresh_label();
struct _tagged_arr* fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();{ struct
_RegionHandle _temp2043= _new_region(); struct _RegionHandle* prgn=& _temp2043;
_push_region( prgn);{ struct Cyc_Absyn_Stmt* _temp2046; struct Cyc_List_List*
_temp2048; struct Cyc_Toc_Env* _temp2050; struct _tuple11 _temp2044= Cyc_Toc_xlate_pat(
nv, prgn, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab,
fail_lab, 0); _LL2051: _temp2050= _temp2044.f1; goto _LL2049; _LL2049: _temp2048=
_temp2044.f2; goto _LL2047; _LL2047: _temp2046= _temp2044.f3; goto _LL2045;
_LL2045: Cyc_Toc_stmt_to_c( _temp2050, s);{ struct Cyc_Absyn_Stmt* _temp2052=
Cyc_Absyn_label_stmt( succ_lab, s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt(
x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2046, _temp2052, 0), 0);} else{ struct Cyc_Absyn_Exp* _temp2053= Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp( 0)); struct Cyc_Absyn_Stmt* _temp2054= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( _temp2053, 0), 0); s= Cyc_Absyn_declare_stmt( x,
Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2046,
Cyc_Absyn_seq_stmt( _temp2054, _temp2052, 0), 0), 0);} for( 0; _temp2048 != 0;
_temp2048=(( struct Cyc_List_List*) _check_null( _temp2048))->tl){ struct
_tuple12 _temp2057; void* _temp2058; struct _tuple0* _temp2060; struct _tuple12*
_temp2055=( struct _tuple12*)(( struct Cyc_List_List*) _check_null( _temp2048))->hd;
_temp2057=* _temp2055; _LL2061: _temp2060= _temp2057.f1; goto _LL2059; _LL2059:
_temp2058= _temp2057.f2; goto _LL2056; _LL2056: s= Cyc_Absyn_declare_stmt(
_temp2060, _temp2058, 0, s, 0);}}}; _pop_region( prgn);} return s;}} static
struct Cyc_Toc_Env* Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv, struct Cyc_List_List*
ds, int top){ for( 0; ds != 0; ds=(( struct Cyc_List_List*) _check_null( ds))->tl){
if( ! nv->toplevel){({ void* _temp2062[ 0u]={};(( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("decls_to_c: not at toplevel!",
sizeof( unsigned char), 29u), _tag_arr( _temp2062, sizeof( void*), 0u));});}{
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( ds))->hd; void* _temp2063=( void*) d->r; struct Cyc_Absyn_Vardecl*
_temp2089; struct Cyc_Absyn_Fndecl* _temp2091; struct Cyc_Absyn_Structdecl*
_temp2093; struct Cyc_Absyn_Uniondecl* _temp2095; struct Cyc_Absyn_Tuniondecl*
_temp2097; struct Cyc_Absyn_Enumdecl* _temp2099; struct Cyc_Absyn_Typedefdecl*
_temp2101; struct Cyc_List_List* _temp2103; struct Cyc_List_List* _temp2105;
struct Cyc_List_List* _temp2107; _LL2065: if(*(( int*) _temp2063) == Cyc_Absyn_Var_d){
_LL2090: _temp2089=(( struct Cyc_Absyn_Var_d_struct*) _temp2063)->f1; goto
_LL2066;} else{ goto _LL2067;} _LL2067: if(*(( int*) _temp2063) == Cyc_Absyn_Fn_d){
_LL2092: _temp2091=(( struct Cyc_Absyn_Fn_d_struct*) _temp2063)->f1; goto
_LL2068;} else{ goto _LL2069;} _LL2069: if(*(( int*) _temp2063) == Cyc_Absyn_Let_d){
goto _LL2070;} else{ goto _LL2071;} _LL2071: if(*(( int*) _temp2063) == Cyc_Absyn_Letv_d){
goto _LL2072;} else{ goto _LL2073;} _LL2073: if(*(( int*) _temp2063) == Cyc_Absyn_Struct_d){
_LL2094: _temp2093=(( struct Cyc_Absyn_Struct_d_struct*) _temp2063)->f1; goto
_LL2074;} else{ goto _LL2075;} _LL2075: if(*(( int*) _temp2063) == Cyc_Absyn_Union_d){
_LL2096: _temp2095=(( struct Cyc_Absyn_Union_d_struct*) _temp2063)->f1; goto
_LL2076;} else{ goto _LL2077;} _LL2077: if(*(( int*) _temp2063) == Cyc_Absyn_Tunion_d){
_LL2098: _temp2097=(( struct Cyc_Absyn_Tunion_d_struct*) _temp2063)->f1; goto
_LL2078;} else{ goto _LL2079;} _LL2079: if(*(( int*) _temp2063) == Cyc_Absyn_Enum_d){
_LL2100: _temp2099=(( struct Cyc_Absyn_Enum_d_struct*) _temp2063)->f1; goto
_LL2080;} else{ goto _LL2081;} _LL2081: if(*(( int*) _temp2063) == Cyc_Absyn_Typedef_d){
_LL2102: _temp2101=(( struct Cyc_Absyn_Typedef_d_struct*) _temp2063)->f1; goto
_LL2082;} else{ goto _LL2083;} _LL2083: if(*(( int*) _temp2063) == Cyc_Absyn_Namespace_d){
_LL2104: _temp2103=(( struct Cyc_Absyn_Namespace_d_struct*) _temp2063)->f2; goto
_LL2084;} else{ goto _LL2085;} _LL2085: if(*(( int*) _temp2063) == Cyc_Absyn_Using_d){
_LL2106: _temp2105=(( struct Cyc_Absyn_Using_d_struct*) _temp2063)->f2; goto
_LL2086;} else{ goto _LL2087;} _LL2087: if(*(( int*) _temp2063) == Cyc_Absyn_ExternC_d){
_LL2108: _temp2107=(( struct Cyc_Absyn_ExternC_d_struct*) _temp2063)->f1; goto
_LL2088;} else{ goto _LL2064;} _LL2066: { struct _tuple0* _temp2109= _temp2089->name;
if(( void*) _temp2089->sc ==( void*) Cyc_Absyn_ExternC){ _temp2109=({ struct
_tuple0* _temp2110=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2110->f1= Cyc_Absyn_rel_ns_null; _temp2110->f2=(* _temp2109).f2; _temp2110;});}
if( _temp2089->initializer != 0){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)
_check_null( _temp2089->initializer));} nv= Cyc_Toc_add_varmap( nv, _temp2089->name,
Cyc_Absyn_varb_exp( _temp2109,( void*)({ struct Cyc_Absyn_Global_b_struct*
_temp2111=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp2111[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp2112; _temp2112.tag= Cyc_Absyn_Global_b;
_temp2112.f1= _temp2089; _temp2112;}); _temp2111;}), 0)); _temp2089->name=
_temp2109;( void*)( _temp2089->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2089->sc));(
void*)( _temp2089->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2089->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2113=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2113->hd=( void*) d; _temp2113->tl=
Cyc_Toc_result_decls; _temp2113;}); goto _LL2064;} _LL2068: { struct _tuple0*
_temp2114= _temp2091->name; if(( void*) _temp2091->sc ==( void*) Cyc_Absyn_ExternC){
_temp2114=({ struct _tuple0* _temp2115=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp2115->f1= Cyc_Absyn_rel_ns_null; _temp2115->f2=(*
_temp2114).f2; _temp2115;});} nv= Cyc_Toc_add_varmap( nv, _temp2091->name, Cyc_Absyn_var_exp(
_temp2114, 0)); _temp2091->name= _temp2114; Cyc_Toc_fndecl_to_c( nv, _temp2091);
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2116=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2116->hd=( void*) d; _temp2116->tl=
Cyc_Toc_result_decls; _temp2116;}); goto _LL2064;} _LL2070: goto _LL2072;
_LL2072:({ void* _temp2117[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Toc_toc_impos)( _tag_arr("letdecl at toplevel", sizeof(
unsigned char), 20u), _tag_arr( _temp2117, sizeof( void*), 0u));}); goto _LL2064;
_LL2074: Cyc_Toc_structdecl_to_c( _temp2093); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2118=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2118->hd=( void*) d; _temp2118->tl= Cyc_Toc_result_decls; _temp2118;});
goto _LL2064; _LL2076: Cyc_Toc_uniondecl_to_c( _temp2095); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2119=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2119->hd=( void*) d; _temp2119->tl= Cyc_Toc_result_decls;
_temp2119;}); goto _LL2064; _LL2078: if( _temp2097->is_xtunion){ Cyc_Toc_xtuniondecl_to_c(
_temp2097);} else{ Cyc_Toc_tuniondecl_to_c( _temp2097);} goto _LL2064; _LL2080:
Cyc_Toc_enumdecl_to_c( nv, _temp2099); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2120=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2120->hd=( void*) d; _temp2120->tl= Cyc_Toc_result_decls; _temp2120;});
goto _LL2064; _LL2082: _temp2101->name= _temp2101->name; _temp2101->tvs= 0;(
void*)( _temp2101->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2101->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2121=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2121->hd=( void*) d; _temp2121->tl=
Cyc_Toc_result_decls; _temp2121;}); goto _LL2064; _LL2084: _temp2105= _temp2103;
goto _LL2086; _LL2086: _temp2107= _temp2105; goto _LL2088; _LL2088: nv= Cyc_Toc_decls_to_c(
nv, _temp2107, top); goto _LL2064; _LL2064:;}} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_temp_var_counter= 0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far=
0; Cyc_Toc_tunions_so_far= 0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List*
Cyc_Toc_toc( struct Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(),
ds, 1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}
