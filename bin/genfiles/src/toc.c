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
} ; extern struct Cyc_List_List* Cyc_List_list( struct _tagged_arr); extern
struct Cyc_List_List* Cyc_List_rlist( struct _RegionHandle*, struct _tagged_arr);
extern int Cyc_List_length( struct Cyc_List_List* x); extern unsigned char Cyc_List_List_empty[
15u]; extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_rmap( struct _RegionHandle*, void*(* f)(
void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap_c(
struct _RegionHandle*, void*(* f)( void*, void*), void* env, struct Cyc_List_List*
x); extern unsigned char Cyc_List_List_mismatch[ 18u]; extern void Cyc_List_iter_c(
void(* f)( void*, void*), void* env, struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_rrevappend( struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List*
y); extern struct Cyc_List_List* Cyc_List_rev( struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_rrev( struct _RegionHandle*, struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct
Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_imp_append( struct Cyc_List_List*
x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_rflatten(
struct _RegionHandle*, struct Cyc_List_List* x); extern unsigned char Cyc_List_Nth[
8u]; extern int Cyc_List_forall( int(* pred)( void*), struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_rfrom_array( struct _RegionHandle* r2,
struct _tagged_arr arr); extern int Cyc_String_zstrptrcmp( struct _tagged_arr*,
struct _tagged_arr*); extern struct _tagged_arr Cyc_String_strconcat( struct
_tagged_arr, struct _tagged_arr); extern struct _tagged_arr Cyc_String_implode(
struct Cyc_List_List* c); struct Cyc_Set_Set; extern struct Cyc_Set_Set* Cyc_Set_empty(
int(* comp)( void*, void*)); extern struct Cyc_Set_Set* Cyc_Set_insert( struct
Cyc_Set_Set* s, void* elt); extern int Cyc_Set_member( struct Cyc_Set_Set* s,
void* elt); extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict;
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
extern struct Cyc_Absyn_Exp* Cyc_Absyn_primop_exp( void*, struct Cyc_List_List*
es, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim1_exp(
void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_prim2_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_add_exp( struct
Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_times_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_eq_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Core_Opt*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
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
struct _tuple0*); extern struct _tagged_arr Cyc_Absynpp_prim2string( void* p);
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos(
struct _tagged_arr); extern void* Cyc_Tcutil_typ_kind( void* t); extern void*
Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_unify( void*, void*);
extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators( struct
_RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List* des,
struct Cyc_List_List* fields); extern int Cyc_Tcutil_is_tagged_pointer_typ( void*);
extern int Cyc_Tcutil_is_tagged_pointer_typ_elt( void* t, void** elt_typ_dest);
struct _tuple3{ struct Cyc_Absyn_Tqual f1; void* f2; } ; extern void* Cyc_Tcutil_snd_tqt(
struct _tuple3*); extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp*
e); extern struct Cyc_Core_Opt* Cyc_Formatstr_parse_conversionspecification(
struct _tagged_arr s, int i); extern struct Cyc_List_List* Cyc_Toc_toc( struct
Cyc_List_List* ds); extern struct _tuple0* Cyc_Toc_temp_var(); static struct Cyc_List_List*
Cyc_Toc_result_decls= 0; static struct Cyc_List_List* Cyc_Toc_tuple_types= 0;
static int Cyc_Toc_tuple_type_counter= 0; static int Cyc_Toc_temp_var_counter= 0;
static int Cyc_Toc_fresh_label_counter= 0; unsigned char Cyc_Toc_Toc_Unimplemented[
22u]="\000\000\000\000Toc_Unimplemented"; unsigned char Cyc_Toc_Toc_Impossible[
19u]="\000\000\000\000Toc_Impossible"; static void* Cyc_Toc_unimp( struct
_tagged_arr s){({ int(* _temp0)( struct Cyc_Stdio___sFILE*, struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp4=
Cyc_Stdio_stderr; struct _tagged_arr _temp5= _tag_arr("%s\n", sizeof(
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
Cyc_Stdio_Int_pa; _temp80.f1= j;{ void* _temp78=( void*)& _temp80; void* _temp76[
1u]={ _temp78}; struct _tagged_arr _temp77={( void*) _temp76,( void*) _temp76,(
void*)( _temp76 + 1u)}; _temp75( _temp79, _temp77);}}); _temp74;});}}; _temp81;});}
return*(( struct _tagged_arr**) _check_unknown_subscript( Cyc_Toc_field_names,
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
_temp178.f1=( unsigned int) Cyc_Toc_tuple_type_counter ++;{ void* _temp176=(
void*)& _temp178; void* _temp174[ 1u]={ _temp176}; struct _tagged_arr _temp175={(
void*) _temp174,( void*) _temp174,( void*)( _temp174 + 1u)}; _temp173( _temp177,
_temp175);}}); _temp172;}); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_snd_tqt,
tqs0); struct Cyc_List_List* _temp159= 0; struct Cyc_List_List* ts2= ts;{ int i=
1; for( 0; ts2 != 0;( ts2=(( struct Cyc_List_List*) _check_null( ts2))->tl, i ++)){
_temp159=({ struct Cyc_List_List* _temp160=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp160->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp161=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp161->name= Cyc_Toc_fieldname( i); _temp161->tq= Cyc_Toc_mt_tq; _temp161->type=(
void*)(( void*)(( struct Cyc_List_List*) _check_null( ts2))->hd); _temp161->width=
0; _temp161->attributes= 0; _temp161;}); _temp160->tl= _temp159; _temp160;});}}
_temp159=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp159);{ struct Cyc_Absyn_Structdecl* _temp162=({ struct Cyc_Absyn_Structdecl*
_temp168=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp168->sc=( void*)(( void*) Cyc_Absyn_Public); _temp168->name=({ struct Cyc_Core_Opt*
_temp170=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp170->v=( void*)({ struct _tuple0* _temp171=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp171->f1= Cyc_Absyn_rel_ns_null; _temp171->f2= x;
_temp171;}); _temp170;}); _temp168->tvs= 0; _temp168->fields=({ struct Cyc_Core_Opt*
_temp169=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp169->v=( void*) _temp159; _temp169;}); _temp168->attributes= 0; _temp168;});
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp163=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp163->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp164=( struct Cyc_Absyn_Struct_d_struct*)
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
_temp186; _temp186.tag= Cyc_Stdio_Int_pa; _temp186.f1=( unsigned int) Cyc_Toc_temp_var_counter
++;{ void* _temp184=( void*)& _temp186; void* _temp182[ 1u]={ _temp184}; struct
_tagged_arr _temp183={( void*) _temp182,( void*) _temp182,( void*)( _temp182 + 1u)};
_temp181( _temp185, _temp183);}}); _temp180;}); _temp179;});} static struct
_tagged_arr* Cyc_Toc_fresh_label(){ return({ struct _tagged_arr* _temp187=(
struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp187[ 0]=(
struct _tagged_arr)({ struct _tagged_arr(* _temp188)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp192= _tag_arr("_LL%d",
sizeof( unsigned char), 6u); struct Cyc_Stdio_Int_pa_struct _temp193; _temp193.tag=
Cyc_Stdio_Int_pa; _temp193.f1=( unsigned int) Cyc_Toc_temp_var_counter ++;{ void*
_temp191=( void*)& _temp193; void* _temp189[ 1u]={ _temp191}; struct _tagged_arr
_temp190={( void*) _temp189,( void*) _temp189,( void*)( _temp189 + 1u)};
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
else{ goto _LL368;} _LL368: goto _LL369; _LL365: goto _LL367; _LL367: continue;
_LL369: _temp361=({ struct Cyc_List_List* _temp370=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp370->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( _temp321))->hd); _temp370->tl= _temp361;
_temp370;}); goto _LL363; _LL363:;}{ struct Cyc_List_List* _temp371=(( struct
Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_arg_to_c, _temp327); if( _temp323 != 0){ void*
_temp372= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ(( void*)(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp323))->type,( void*)(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp323))->rgn, Cyc_Toc_mt_tq)); struct _tuple1* _temp373=({
struct _tuple1* _temp375=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp375->f1=(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp323))->name;
_temp375->f2=(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp323))->tq;
_temp375->f3= _temp372; _temp375;}); _temp371=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp371,({ struct Cyc_List_List* _temp374=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp374->hd=( void*) _temp373; _temp374->tl= 0;
_temp374;}));} return( void*)({ struct Cyc_Absyn_FnType_struct* _temp376=(
struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp376[ 0]=({ struct Cyc_Absyn_FnType_struct _temp377; _temp377.tag= Cyc_Absyn_FnType;
_temp377.f1=({ struct Cyc_Absyn_FnInfo _temp378; _temp378.tvars= 0; _temp378.effect=
0; _temp378.ret_typ=( void*) Cyc_Toc_typ_to_c( _temp329); _temp378.args=
_temp371; _temp378.c_varargs= _temp325; _temp378.cyc_varargs= 0; _temp378.rgn_po=
0; _temp378.attributes= _temp361; _temp378;}); _temp377;}); _temp376;});}}
_LL263: _temp331=(( struct Cyc_List_List*(*)( struct _tuple3*(* f)( struct
_tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f, _temp331);{
struct _tagged_arr* _temp379= Cyc_Toc_add_tuple_type( _temp331); return Cyc_Absyn_strct(
_temp379);} _LL265: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp380=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp380[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp381; _temp381.tag= Cyc_Absyn_AnonStructType; _temp381.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_structfield_to_c, _temp333); _temp381;}); _temp380;});
_LL267: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp382=(
struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp382[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp383; _temp383.tag=
Cyc_Absyn_AnonUnionType; _temp383.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_structfield_to_c,
_temp335); _temp383;}); _temp382;}); _LL269: if( _temp337 == 0){(( int(*)(
struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("anonymous union", sizeof(
unsigned char), 16u));} return Cyc_Absyn_unionq_typ(( struct _tuple0*)
_check_null( _temp337)); _LL271: if( _temp339 == 0){(( int(*)( struct
_tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("anonymous struct", sizeof(
unsigned char), 17u));} return Cyc_Absyn_strctq(( struct _tuple0*) _check_null(
_temp339)); _LL273: return t; _LL275: if( _temp343 == 0){ return Cyc_Toc_toc_impos(
_tag_arr("Toc::typ_to_c: unresolved TypedefType", sizeof( unsigned char), 38u));}
return( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp384=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp384[ 0]=({ struct
Cyc_Absyn_TypedefType_struct _temp385; _temp385.tag= Cyc_Absyn_TypedefType;
_temp385.f1= _temp347; _temp385.f2= 0; _temp385.f3=({ struct Cyc_Core_Opt*
_temp386=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp386->v=( void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp343))->v); _temp386;}); _temp385;}); _temp384;}); _LL277:
return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp), Cyc_Toc_mt_tq);
_LL279: return Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the heap region",
sizeof( unsigned char), 55u)); _LL281: return Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the access eff",
sizeof( unsigned char), 54u)); _LL283: return Cyc_Toc_toc_impos( _tag_arr("Toc::typ_to_c: type translation passed the join eff",
sizeof( unsigned char), 52u)); _LL233:;} static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast(
void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* l){ void*
_temp387= t; struct Cyc_Absyn_Tqual _temp393; void* _temp395; _LL389: if((
unsigned int) _temp387 > 4u?*(( int*) _temp387) == Cyc_Absyn_ArrayType: 0){
_LL396: _temp395=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp387)->f1;
goto _LL394; _LL394: _temp393=(( struct Cyc_Absyn_ArrayType_struct*) _temp387)->f2;
goto _LL390;} else{ goto _LL391;} _LL391: goto _LL392; _LL390: return Cyc_Absyn_cast_exp(
Cyc_Absyn_star_typ( _temp395,( void*) Cyc_Absyn_HeapRgn, _temp393), e, l);
_LL392: return Cyc_Absyn_cast_exp( t, e, l); _LL388:;} static int Cyc_Toc_atomic_typ(
void* t){ void* _temp397= Cyc_Tcutil_compress( t); void* _temp433; struct Cyc_Absyn_Structdecl**
_temp435; struct Cyc_List_List* _temp437; struct Cyc_List_List* _temp439; struct
Cyc_Absyn_TunionFieldInfo _temp441; void* _temp443; struct Cyc_Absyn_Tunionfield*
_temp445; struct Cyc_Absyn_Tuniondecl* _temp447; struct Cyc_List_List* _temp449;
_LL399: if( _temp397 ==( void*) Cyc_Absyn_VoidType){ goto _LL400;} else{ goto
_LL401;} _LL401: if(( unsigned int) _temp397 > 4u?*(( int*) _temp397) == Cyc_Absyn_VarType:
0){ goto _LL402;} else{ goto _LL403;} _LL403: if(( unsigned int) _temp397 > 4u?*((
int*) _temp397) == Cyc_Absyn_IntType: 0){ goto _LL404;} else{ goto _LL405;}
_LL405: if( _temp397 ==( void*) Cyc_Absyn_FloatType){ goto _LL406;} else{ goto
_LL407;} _LL407: if( _temp397 ==( void*) Cyc_Absyn_DoubleType){ goto _LL408;}
else{ goto _LL409;} _LL409: if(( unsigned int) _temp397 > 4u?*(( int*) _temp397)
== Cyc_Absyn_FnType: 0){ goto _LL410;} else{ goto _LL411;} _LL411: if((
unsigned int) _temp397 > 4u?*(( int*) _temp397) == Cyc_Absyn_ArrayType: 0){
_LL434: _temp433=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp397)->f1;
goto _LL412;} else{ goto _LL413;} _LL413: if(( unsigned int) _temp397 > 4u?*((
int*) _temp397) == Cyc_Absyn_StructType: 0){ _LL436: _temp435=(( struct Cyc_Absyn_StructType_struct*)
_temp397)->f3; goto _LL414;} else{ goto _LL415;} _LL415: if(( unsigned int)
_temp397 > 4u?*(( int*) _temp397) == Cyc_Absyn_AnonStructType: 0){ _LL438:
_temp437=(( struct Cyc_Absyn_AnonStructType_struct*) _temp397)->f1; goto _LL416;}
else{ goto _LL417;} _LL417: if(( unsigned int) _temp397 > 4u?*(( int*) _temp397)
== Cyc_Absyn_AnonUnionType: 0){ _LL440: _temp439=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp397)->f1; goto _LL418;} else{ goto _LL419;} _LL419: if(( unsigned int)
_temp397 > 4u?*(( int*) _temp397) == Cyc_Absyn_TunionFieldType: 0){ _LL442:
_temp441=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp397)->f1; _LL444:
_temp443=( void*) _temp441.field_info; if(*(( int*) _temp443) == Cyc_Absyn_KnownTunionfield){
_LL448: _temp447=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp443)->f1;
goto _LL446; _LL446: _temp445=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp443)->f2; goto _LL420;} else{ goto _LL421;}} else{ goto _LL421;} _LL421:
if(( unsigned int) _temp397 > 4u?*(( int*) _temp397) == Cyc_Absyn_TupleType: 0){
_LL450: _temp449=(( struct Cyc_Absyn_TupleType_struct*) _temp397)->f1; goto
_LL422;} else{ goto _LL423;} _LL423: if(( unsigned int) _temp397 > 4u?*(( int*)
_temp397) == Cyc_Absyn_TunionType: 0){ goto _LL424;} else{ goto _LL425;} _LL425:
if(( unsigned int) _temp397 > 4u?*(( int*) _temp397) == Cyc_Absyn_UnionType: 0){
goto _LL426;} else{ goto _LL427;} _LL427: if(( unsigned int) _temp397 > 4u?*((
int*) _temp397) == Cyc_Absyn_PointerType: 0){ goto _LL428;} else{ goto _LL429;}
_LL429: if(( unsigned int) _temp397 > 4u?*(( int*) _temp397) == Cyc_Absyn_RgnHandleType:
0){ goto _LL430;} else{ goto _LL431;} _LL431: goto _LL432; _LL400: return 1;
_LL402: return 0; _LL404: return 1; _LL406: return 1; _LL408: return 1; _LL410:
return 1; _LL412: return Cyc_Toc_atomic_typ( _temp433); _LL414: if( _temp435 ==
0? 1:(*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp435)))->fields == 0){
return 0;}{ struct Cyc_List_List* _temp451=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp435)))->fields))->v;
for( 0; _temp451 != 0; _temp451=(( struct Cyc_List_List*) _check_null( _temp451))->tl){
if( ! Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp451))->hd)->type)){ return 0;}}} return 1; _LL416: _temp439=
_temp437; goto _LL418; _LL418: for( 0; _temp439 != 0; _temp439=(( struct Cyc_List_List*)
_check_null( _temp439))->tl){ if( ! Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp439))->hd)->type)){ return 0;}} return
1; _LL420: _temp449= _temp445->typs; goto _LL422; _LL422: for( 0; _temp449 != 0;
_temp449=(( struct Cyc_List_List*) _check_null( _temp449))->tl){ if( ! Cyc_Toc_atomic_typ((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp449))->hd)).f2)){
return 0;}} return 1; _LL424: return 0; _LL426: return 0; _LL428: return 0;
_LL430: return 0; _LL432:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr(* _temp452)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp456= _tag_arr("atomic_typ:  bad type %s",
sizeof( unsigned char), 25u); struct Cyc_Stdio_String_pa_struct _temp457;
_temp457.tag= Cyc_Stdio_String_pa; _temp457.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp455=( void*)& _temp457; void* _temp453[ 1u]={ _temp455}; struct
_tagged_arr _temp454={( void*) _temp453,( void*) _temp453,( void*)( _temp453 + 1u)};
_temp452( _temp456, _temp454);}})); return 0; _LL398:;} static int Cyc_Toc_is_void_star(
void* t){ void* _temp458= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp464; void* _temp466; _LL460: if(( unsigned int) _temp458 > 4u?*(( int*)
_temp458) == Cyc_Absyn_PointerType: 0){ _LL465: _temp464=(( struct Cyc_Absyn_PointerType_struct*)
_temp458)->f1; _LL467: _temp466=( void*) _temp464.elt_typ; goto _LL461;} else{
goto _LL462;} _LL462: goto _LL463; _LL461: { void* _temp468= Cyc_Tcutil_compress(
_temp466); _LL470: if( _temp468 ==( void*) Cyc_Absyn_VoidType){ goto _LL471;}
else{ goto _LL472;} _LL472: goto _LL473; _LL471: return 1; _LL473: return 0;
_LL469:;} _LL463: return 0; _LL459:;} static int Cyc_Toc_is_poly_field( void* t,
struct _tagged_arr* f){ void* _temp474= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp486; struct Cyc_Absyn_Structdecl* _temp488; struct Cyc_List_List* _temp489;
_LL476: if(( unsigned int) _temp474 > 4u?*(( int*) _temp474) == Cyc_Absyn_StructType:
0){ _LL487: _temp486=(( struct Cyc_Absyn_StructType_struct*) _temp474)->f3; if(
_temp486 == 0){ goto _LL478;} else{ _temp488=* _temp486; goto _LL477;}} else{
goto _LL478;} _LL478: if(( unsigned int) _temp474 > 4u?*(( int*) _temp474) ==
Cyc_Absyn_AnonStructType: 0){ _LL490: _temp489=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp474)->f1; goto _LL479;} else{ goto _LL480;} _LL480: if(( unsigned int)
_temp474 > 4u?*(( int*) _temp474) == Cyc_Absyn_AnonUnionType: 0){ goto _LL481;}
else{ goto _LL482;} _LL482: if(( unsigned int) _temp474 > 4u?*(( int*) _temp474)
== Cyc_Absyn_UnionType: 0){ goto _LL483;} else{ goto _LL484;} _LL484: goto
_LL485; _LL477: if( _temp488->fields == 0){( int) _throw( Cyc_Toc_toc_impos(
_tag_arr("is_poly_field: struct missing fields", sizeof( unsigned char), 37u)));}
_temp489=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp488->fields))->v;
goto _LL479; _LL479: { struct Cyc_Absyn_Structfield* _temp491= Cyc_Absyn_lookup_field(
_temp489, f); if( _temp491 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr(* _temp492)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp496= _tag_arr("is_poly_field: bad field %s",
sizeof( unsigned char), 28u); struct Cyc_Stdio_String_pa_struct _temp497;
_temp497.tag= Cyc_Stdio_String_pa; _temp497.f1=( struct _tagged_arr)*(( struct
Cyc_Absyn_Structfield*) _check_null( _temp491))->name;{ void* _temp495=( void*)&
_temp497; void* _temp493[ 1u]={ _temp495}; struct _tagged_arr _temp494={( void*)
_temp493,( void*) _temp493,( void*)( _temp493 + 1u)}; _temp492( _temp496,
_temp494);}}));} return Cyc_Toc_is_void_star(( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp491))->type);} _LL481: return 0; _LL483: return 0; _LL485:(
int) _throw( Cyc_Toc_toc_impos(( struct _tagged_arr)({ struct _tagged_arr(*
_temp498)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp502= _tag_arr("is_poly_field: bad type %s", sizeof(
unsigned char), 27u); struct Cyc_Stdio_String_pa_struct _temp503; _temp503.tag=
Cyc_Stdio_String_pa; _temp503.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp501=( void*)& _temp503; void* _temp499[ 1u]={ _temp501}; struct
_tagged_arr _temp500={( void*) _temp499,( void*) _temp499,( void*)( _temp499 + 1u)};
_temp498( _temp502, _temp500);}}))); _LL475:;} static int Cyc_Toc_is_poly_project(
struct Cyc_Absyn_Exp* e){ void* _temp504=( void*) e->r; struct _tagged_arr*
_temp512; struct Cyc_Absyn_Exp* _temp514; struct _tagged_arr* _temp516; struct
Cyc_Absyn_Exp* _temp518; _LL506: if(*(( int*) _temp504) == Cyc_Absyn_StructMember_e){
_LL515: _temp514=(( struct Cyc_Absyn_StructMember_e_struct*) _temp504)->f1; goto
_LL513; _LL513: _temp512=(( struct Cyc_Absyn_StructMember_e_struct*) _temp504)->f2;
goto _LL507;} else{ goto _LL508;} _LL508: if(*(( int*) _temp504) == Cyc_Absyn_StructArrow_e){
_LL519: _temp518=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp504)->f1; goto
_LL517; _LL517: _temp516=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp504)->f2;
goto _LL509;} else{ goto _LL510;} _LL510: goto _LL511; _LL507: return Cyc_Toc_is_poly_field((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp514->topt))->v, _temp512);
_LL509: { void* _temp520= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp518->topt))->v); struct Cyc_Absyn_PtrInfo _temp526; void*
_temp528; _LL522: if(( unsigned int) _temp520 > 4u?*(( int*) _temp520) == Cyc_Absyn_PointerType:
0){ _LL527: _temp526=(( struct Cyc_Absyn_PointerType_struct*) _temp520)->f1;
_LL529: _temp528=( void*) _temp526.elt_typ; goto _LL523;} else{ goto _LL524;}
_LL524: goto _LL525; _LL523: return Cyc_Toc_is_poly_field( _temp528, _temp516);
_LL525:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({
struct _tagged_arr(* _temp530)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp534= _tag_arr("is_poly_project:  bad type %s", sizeof(
unsigned char), 30u); struct Cyc_Stdio_String_pa_struct _temp535; _temp535.tag=
Cyc_Stdio_String_pa; _temp535.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp518->topt))->v);{ void*
_temp533=( void*)& _temp535; void* _temp531[ 1u]={ _temp533}; struct _tagged_arr
_temp532={( void*) _temp531,( void*) _temp531,( void*)( _temp531 + 1u)};
_temp530( _temp534, _temp532);}})); return 0; _LL521:;} _LL511: return 0; _LL505:;}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr( struct Cyc_Absyn_Exp* s){
return Cyc_Absyn_fncall_exp( Cyc_Toc_GC_malloc_e,({ struct Cyc_List_List*
_temp536=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp536->hd=( void*) s; _temp536->tl= 0; _temp536;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_atomic( struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp(
Cyc_Toc_GC_malloc_atomic_e,({ struct Cyc_List_List* _temp537=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp537->hd=( void*) s; _temp537->tl=
0; _temp537;}), 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t,
struct Cyc_Absyn_Exp* s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic(
s);} return Cyc_Toc_malloc_ptr( s);} static struct Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp* rgn, struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_malloc_e,({ struct Cyc_List_List*(* _temp538)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp541= rgn; struct Cyc_Absyn_Exp* _temp542= s; struct
Cyc_Absyn_Exp* _temp539[ 2u]={ _temp541, _temp542}; struct _tagged_arr _temp540={(
void*) _temp539,( void*) _temp539,( void*)( _temp539 + 2u)}; _temp538( _temp540);}),
0);} static struct Cyc_Absyn_Exp* Cyc_Toc_fdconv( struct Cyc_Absyn_Exp* e){
return Cyc_Absyn_fncall_exp( Cyc_Toc__sfile_to_file_e,({ struct Cyc_List_List*
_temp543=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp543->hd=( void*) e; _temp543->tl= 0; _temp543;}), 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp* e){ return Cyc_Absyn_fncall_exp( Cyc_Toc__throw_e,({
struct Cyc_List_List* _temp544=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp544->hd=( void*) e; _temp544->tl= 0; _temp544;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_make_toplevel_tagged_arr( void* t, struct
Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp* e){ int is_string= 0;{ void* _temp545=(
void*) e->r; void* _temp551; _LL547: if(*(( int*) _temp545) == Cyc_Absyn_Const_e){
_LL552: _temp551=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp545)->f1; if((
unsigned int) _temp551 > 1u?*(( int*) _temp551) == Cyc_Absyn_String_c: 0){ goto
_LL548;} else{ goto _LL549;}} else{ goto _LL549;} _LL549: goto _LL550; _LL548:
is_string= 1; goto _LL546; _LL550: goto _LL546; _LL546:;}{ struct Cyc_Absyn_Exp*
xexp; if( is_string){ struct _tuple0* x= Cyc_Toc_temp_var(); void* vd_typ=( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp556=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp556[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp557; _temp557.tag= Cyc_Absyn_ArrayType; _temp557.f1=(
void*) Cyc_Absyn_uchar_t; _temp557.f2= Cyc_Toc_mt_tq; _temp557.f3=( struct Cyc_Absyn_Exp*)
sz; _temp557;}); _temp556;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp553=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp553->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp554=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp554[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp555; _temp555.tag= Cyc_Absyn_Var_d;
_temp555.f1= vd; _temp555;}); _temp554;}), 0); _temp553->tl= Cyc_Toc_result_decls;
_temp553;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ xexp= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), e, 0);}{ struct Cyc_Absyn_Exp* xplussz= Cyc_Absyn_add_exp(
xexp, sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp( 0,({
struct Cyc_List_List*(* _temp558)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct _tuple4* _temp561=({ struct _tuple4*
_temp566=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp566->f1= 0;
_temp566->f2= xexp; _temp566;}); struct _tuple4* _temp562=({ struct _tuple4*
_temp565=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp565->f1= 0;
_temp565->f2= xexp; _temp565;}); struct _tuple4* _temp563=({ struct _tuple4*
_temp564=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp564->f1= 0;
_temp564->f2= xplussz; _temp564;}); struct _tuple4* _temp559[ 3u]={ _temp561,
_temp562, _temp563}; struct _tagged_arr _temp560={( void*) _temp559,( void*)
_temp559,( void*)( _temp559 + 3u)}; _temp558( _temp560);}), 0); return urm_exp;}}}
struct Cyc_Toc_Env{ struct Cyc_Core_Opt* break_lab; struct Cyc_Core_Opt*
continue_lab; struct Cyc_Core_Opt* fallthru_info; struct Cyc_Dict_Dict* varmap;
int toplevel; } ; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){ return({
struct Cyc_Toc_Env* _temp567=( struct Cyc_Toc_Env*) GC_malloc( sizeof( struct
Cyc_Toc_Env)); _temp567->break_lab= 0; _temp567->continue_lab= 0; _temp567->fallthru_info=
0; _temp567->varmap=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp567->toplevel= 1;
_temp567;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env( struct Cyc_Toc_Env* e){
return({ struct Cyc_Toc_Env* _temp568=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp568->break_lab= e->break_lab; _temp568->continue_lab=
e->continue_lab; _temp568->fallthru_info= e->fallthru_info; _temp568->varmap= e->varmap;
_temp568->toplevel= e->toplevel; _temp568;});} static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel=
0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env*
e, struct _tuple0* x, struct Cyc_Absyn_Exp* y){{ void* _temp569=(* x).f1; _LL571:
if(( unsigned int) _temp569 > 1u?*(( int*) _temp569) == Cyc_Absyn_Rel_n: 0){
goto _LL572;} else{ goto _LL573;} _LL573: goto _LL574; _LL572:( int) _throw( Cyc_Toc_toc_impos((
struct _tagged_arr)({ struct _tagged_arr(* _temp575)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp579= _tag_arr("Toc::add_varmap on Rel_n: %s\n",
sizeof( unsigned char), 30u); struct Cyc_Stdio_String_pa_struct _temp580;
_temp580.tag= Cyc_Stdio_String_pa; _temp580.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
x);{ void* _temp578=( void*)& _temp580; void* _temp576[ 1u]={ _temp578}; struct
_tagged_arr _temp577={( void*) _temp576,( void*) _temp576,( void*)( _temp576 + 1u)};
_temp575( _temp579, _temp577);}}))); _LL574: goto _LL570; _LL570:;}{ struct Cyc_Toc_Env*
ans= Cyc_Toc_copy_env( e); ans->varmap=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tuple0* key, struct Cyc_Absyn_Exp* data)) Cyc_Dict_insert)( ans->varmap,
x, y); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_loop_env( struct Cyc_Toc_Env*
e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->continue_lab=
0; return ans;} struct _tuple6{ struct _tagged_arr* f1; struct Cyc_List_List* f2;
struct Cyc_Dict_Dict* f3; } ; static struct Cyc_Toc_Env* Cyc_Toc_non_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_arr* break_l, struct _tagged_arr*
fallthru_l, struct Cyc_List_List* fallthru_binders, struct Cyc_Toc_Env*
next_case_env){ struct Cyc_List_List* _temp581= 0; for( 0; fallthru_binders != 0;
fallthru_binders=(( struct Cyc_List_List*) _check_null( fallthru_binders))->tl){
_temp581=({ struct Cyc_List_List* _temp582=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp582->hd=( void*)(( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( fallthru_binders))->hd)->name; _temp582->tl=
_temp581; _temp582;});} _temp581=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp581);{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e);
ans->break_lab=({ struct Cyc_Core_Opt* _temp583=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp583->v=( void*) break_l; _temp583;});
ans->fallthru_info=({ struct Cyc_Core_Opt* _temp584=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp584->v=( void*)({ struct _tuple6*
_temp585=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp585->f1=
fallthru_l; _temp585->f2= _temp581; _temp585->f3= next_case_env->varmap;
_temp585;}); _temp584;}); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_arr* break_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab=({ struct Cyc_Core_Opt* _temp586=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp586->v=( void*)
break_l; _temp586;}); ans->fallthru_info= 0; return ans;} static struct Cyc_Toc_Env*
Cyc_Toc_switch_as_switch_env( struct Cyc_Toc_Env* e, struct _tagged_arr* next_l){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->fallthru_info=({
struct Cyc_Core_Opt* _temp587=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp587->v=( void*)({ struct _tuple6* _temp588=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp588->f1= next_l; _temp588->f2= 0;
_temp588->f3=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct
_tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp588;}); _temp587;});
return ans;} struct _tuple7{ struct Cyc_List_List* f1; struct Cyc_List_List* f2;
struct Cyc_List_List* f3; struct Cyc_List_List* f4; unsigned char f5; int f6; }
; struct _tuple8{ struct _tuple0* f1; void* f2; struct Cyc_Absyn_Exp* f3; } ;
static struct Cyc_Absyn_Exp* Cyc_Toc_printf_to_c( void* p, struct Cyc_Core_Opt*
aopt, struct Cyc_List_List* args, struct Cyc_List_List* typs){ if( args == 0){
return(( struct Cyc_Absyn_Exp*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("expecting a literal format string", sizeof( unsigned char), 34u));}{
struct _tagged_arr fmt_str;{ void* _temp589=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( args))->hd)->r; void* _temp595; struct
_tagged_arr _temp597; _LL591: if(*(( int*) _temp589) == Cyc_Absyn_Const_e){
_LL596: _temp595=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp589)->f1; if((
unsigned int) _temp595 > 1u?*(( int*) _temp595) == Cyc_Absyn_String_c: 0){
_LL598: _temp597=(( struct Cyc_Absyn_String_c_struct*) _temp595)->f1; goto
_LL592;} else{ goto _LL593;}} else{ goto _LL593;} _LL593: goto _LL594; _LL592:
fmt_str= _temp597; goto _LL590; _LL594: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("expecting a literal format string",
sizeof( unsigned char), 34u)); _LL590:;}{ int len=( int) _get_arr_size( fmt_str,
sizeof( unsigned char)); int i= 0; for( i= 0; i < len; i ++){ if(*(( const
unsigned char*) _check_unknown_subscript( fmt_str, sizeof( unsigned char), i))
=='%'){ i ++; if( i < len?*(( const unsigned char*) _check_unknown_subscript(
fmt_str, sizeof( unsigned char), i)) =='s': 0){ break;}}} if( i == len){ if(
aopt != 0){ args=({ struct Cyc_List_List* _temp599=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp599->hd=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( aopt))->v); _temp599->tl= args; _temp599;});}
return Cyc_Absyn_primop_exp( p, args, 0);} args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;{
struct Cyc_Absyn_Stmt* s;{ struct _RegionHandle _temp600= _new_region(); struct
_RegionHandle* r=& _temp600; _push_region( r);{ struct Cyc_List_List* rev_fmt= 0;
struct Cyc_List_List* rev_result= 0; struct Cyc_List_List* rev_temps= 0; for( i=
0; i < len; i ++){ unsigned char c=*(( const unsigned char*)
_check_unknown_subscript( fmt_str, sizeof( unsigned char), i)); rev_fmt=({
struct Cyc_List_List* _temp601=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp601->hd=( void*)(( int) c); _temp601->tl=
rev_fmt; _temp601;}); if( c !='%'){ continue;}{ struct Cyc_Core_Opt* _temp602=
Cyc_Formatstr_parse_conversionspecification( fmt_str, i + 1); if( _temp602 == 0){((
int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("bad format string",
sizeof( unsigned char), 18u));}{ struct _tuple7 _temp605; int _temp606;
unsigned char _temp608; struct Cyc_List_List* _temp610; struct Cyc_List_List*
_temp612; struct Cyc_List_List* _temp614; struct Cyc_List_List* _temp616; struct
_tuple7* _temp603=( struct _tuple7*)(( struct Cyc_Core_Opt*) _check_null(
_temp602))->v; _temp605=* _temp603; _LL617: _temp616= _temp605.f1; goto _LL615;
_LL615: _temp614= _temp605.f2; goto _LL613; _LL613: _temp612= _temp605.f3; goto
_LL611; _LL611: _temp610= _temp605.f4; goto _LL609; _LL609: _temp608= _temp605.f5;
goto _LL607; _LL607: _temp606= _temp605.f6; goto _LL604; _LL604: i= _temp606 - 1;
if( _temp608 !='s'){ rev_fmt=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rrevappend)( r,((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rflatten)(
r,({ struct Cyc_List_List*(* _temp618)( struct _RegionHandle*, struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _RegionHandle*, struct
_tagged_arr)) Cyc_List_rlist; struct _RegionHandle* _temp625= r; struct Cyc_List_List*
_temp621= _temp616; struct Cyc_List_List* _temp622= _temp614; struct Cyc_List_List*
_temp623= _temp612; struct Cyc_List_List* _temp624= _temp610; struct Cyc_List_List*
_temp619[ 4u]={ _temp621, _temp622, _temp623, _temp624}; struct _tagged_arr
_temp620={( void*) _temp619,( void*) _temp619,( void*)( _temp619 + 4u)};
_temp618( _temp625, _temp620);})), rev_fmt); rev_fmt=({ struct Cyc_List_List*
_temp626=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp626->hd=( void*)(( int) _temp608); _temp626->tl= rev_fmt; _temp626;});{
struct Cyc_List_List* _temp627= _temp614; struct Cyc_List_List _temp634; struct
Cyc_List_List* _temp635; int _temp637; _LL629: if( _temp627 == 0){ goto _LL631;}
else{ _temp634=* _temp627; _LL638: _temp637=( int) _temp634.hd; goto _LL636;
_LL636: _temp635= _temp634.tl; if( _temp635 == 0){ goto _LL633;} else{ goto
_LL631;}} _LL633: if( _temp637 =='*'){ goto _LL630;} else{ goto _LL631;} _LL631:
goto _LL632; _LL630: { struct _tuple0* _temp639= Cyc_Toc_temp_var(); rev_temps=({
struct Cyc_List_List* _temp640=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp640->hd=( void*)({ struct _tuple8* _temp641=(
struct _tuple8*) _region_malloc( r, sizeof( struct _tuple8)); _temp641->f1=
_temp639; _temp641->f2=( void*)(( struct Cyc_List_List*) _check_null( typs))->hd;
_temp641->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd;
_temp641;}); _temp640->tl= rev_temps; _temp640;}); rev_result=({ struct Cyc_List_List*
_temp642=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp642->hd=( void*) Cyc_Absyn_var_exp( _temp639, 0); _temp642->tl= rev_result;
_temp642;}); args=(( struct Cyc_List_List*) _check_null( args))->tl; typs=((
struct Cyc_List_List*) _check_null( typs))->tl; goto _LL628;} _LL632: goto
_LL628; _LL628:;}{ struct Cyc_List_List* _temp643= _temp612; struct Cyc_List_List
_temp650; struct Cyc_List_List* _temp651; struct Cyc_List_List _temp653; struct
Cyc_List_List* _temp654; int _temp656; int _temp658; _LL645: if( _temp643 == 0){
goto _LL647;} else{ _temp650=* _temp643; _LL659: _temp658=( int) _temp650.hd;
goto _LL652; _LL652: _temp651= _temp650.tl; if( _temp651 == 0){ goto _LL647;}
else{ _temp653=* _temp651; _LL657: _temp656=( int) _temp653.hd; goto _LL655;
_LL655: _temp654= _temp653.tl; if( _temp654 == 0){ goto _LL649;} else{ goto
_LL647;}}} _LL649: if( _temp658 =='.'? _temp656 =='*': 0){ goto _LL646;} else{
goto _LL647;} _LL647: goto _LL648; _LL646: { struct _tuple0* _temp660= Cyc_Toc_temp_var();
rev_temps=({ struct Cyc_List_List* _temp661=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp661->hd=( void*)({
struct _tuple8* _temp662=( struct _tuple8*) _region_malloc( r, sizeof( struct
_tuple8)); _temp662->f1= _temp660; _temp662->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp662->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp662;}); _temp661->tl= rev_temps; _temp661;});
rev_result=({ struct Cyc_List_List* _temp663=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp663->hd=( void*) Cyc_Absyn_var_exp(
_temp660, 0); _temp663->tl= rev_result; _temp663;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;
goto _LL644;} _LL648: goto _LL644; _LL644:;} if( _temp608 !='%'){ if( args == 0?
1: typs == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct
_tagged_arr)({ struct _tagged_arr(* _temp664)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp668= _tag_arr("missing arg to %s",
sizeof( unsigned char), 18u); struct Cyc_Stdio_String_pa_struct _temp669;
_temp669.tag= Cyc_Stdio_String_pa; _temp669.f1=( struct _tagged_arr) Cyc_Absynpp_prim2string(
p);{ void* _temp667=( void*)& _temp669; void* _temp665[ 1u]={ _temp667}; struct
_tagged_arr _temp666={( void*) _temp665,( void*) _temp665,( void*)( _temp665 + 1u)};
_temp664( _temp668, _temp666);}}));}{ struct _tuple0* _temp670= Cyc_Toc_temp_var();
rev_temps=({ struct Cyc_List_List* _temp671=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp671->hd=( void*)({
struct _tuple8* _temp672=( struct _tuple8*) _region_malloc( r, sizeof( struct
_tuple8)); _temp672->f1= _temp670; _temp672->f2=( void*)(( struct Cyc_List_List*)
_check_null( typs))->hd; _temp672->f3=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( args))->hd; _temp672;}); _temp671->tl= rev_temps; _temp671;});
rev_result=({ struct Cyc_List_List* _temp673=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp673->hd=( void*) Cyc_Absyn_var_exp(
_temp670, 0); _temp673->tl= rev_result; _temp673;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;}}}
else{ rev_fmt=({ struct Cyc_List_List* _temp674=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp674->hd=( void*)(( int)'.');
_temp674->tl= rev_fmt; _temp674;}); rev_fmt=({ struct Cyc_List_List* _temp675=(
struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp675->hd=( void*)(( int)'*'); _temp675->tl= rev_fmt; _temp675;}); rev_fmt=({
struct Cyc_List_List* _temp676=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp676->hd=( void*)(( int)'s'); _temp676->tl=
rev_fmt; _temp676;});{ struct _tuple0* _temp677= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp678= Cyc_Absyn_var_exp( _temp677, 0); rev_temps=({ struct Cyc_List_List*
_temp679=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp679->hd=( void*)({ struct _tuple8* _temp680=( struct _tuple8*)
_region_malloc( r, sizeof( struct _tuple8)); _temp680->f1= _temp677; _temp680->f2=(
void*)(( struct Cyc_List_List*) _check_null( typs))->hd; _temp680->f3=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd; _temp680;});
_temp679->tl= rev_temps; _temp679;});{ struct Cyc_Absyn_Exp* _temp681= Cyc_Absyn_fncall_exp(
Cyc_Toc__get_arr_size_e,({ struct Cyc_List_List*(* _temp685)( struct _tagged_arr)=(
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp*
_temp688= _temp678; struct Cyc_Absyn_Exp* _temp689= Cyc_Absyn_uint_exp( 1, 0);
struct Cyc_Absyn_Exp* _temp686[ 2u]={ _temp688, _temp689}; struct _tagged_arr
_temp687={( void*) _temp686,( void*) _temp686,( void*)( _temp686 + 2u)};
_temp685( _temp687);}), 0); struct Cyc_Absyn_Exp* _temp682= Cyc_Absyn_structmember_exp(
_temp678, Cyc_Toc_curr_sp, 0); rev_result=({ struct Cyc_List_List* _temp683=(
struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp683->hd=( void*) _temp681; _temp683->tl= rev_result; _temp683;});
rev_result=({ struct Cyc_List_List* _temp684=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp684->hd=( void*)
_temp682; _temp684->tl= rev_result; _temp684;}); args=(( struct Cyc_List_List*)
_check_null( args))->tl; typs=(( struct Cyc_List_List*) _check_null( typs))->tl;}}}}}}{
struct Cyc_List_List* _temp690=({ struct Cyc_List_List* _temp693=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp693->hd=( void*) Cyc_Absyn_string_exp((
struct _tagged_arr) Cyc_String_implode((( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( rev_fmt)), 0); _temp693->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( rev_result); _temp693;}); if( aopt != 0){
_temp690=({ struct Cyc_List_List* _temp691=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp691->hd=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( aopt))->v); _temp691->tl= _temp690; _temp691;});}{
struct Cyc_Absyn_Exp* _temp692= Cyc_Absyn_primop_exp( p, _temp690, 0); s= Cyc_Absyn_exp_stmt(
_temp692, 0); for( 0; rev_temps != 0; rev_temps=(( struct Cyc_List_List*)
_check_null( rev_temps))->tl){ s= Cyc_Absyn_declare_stmt((*(( struct _tuple8*)((
struct Cyc_List_List*) _check_null( rev_temps))->hd)).f1,(*(( struct _tuple8*)((
struct Cyc_List_List*) _check_null( rev_temps))->hd)).f2,( struct Cyc_Absyn_Exp*)(*((
struct _tuple8*)(( struct Cyc_List_List*) _check_null( rev_temps))->hd)).f3, s,
0);}}}}; _pop_region( r);} return Cyc_Absyn_stmt_exp( s, 0);}}}} static void Cyc_Toc_exp_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e); static void Cyc_Toc_stmt_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt* s); static void* Cyc_Toc_get_c_typ(
struct Cyc_Absyn_Exp* e){ if( e->topt == 0){(( int(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("Missing type in primop ", sizeof( unsigned char),
24u));} return Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);}
static void* Cyc_Toc_get_cyc_typ( struct Cyc_Absyn_Exp* e){ if( e->topt == 0){((
int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("Missing type in primop ",
sizeof( unsigned char), 24u));} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v;} static struct _tuple3* Cyc_Toc_tup_to_c( struct Cyc_Absyn_Exp*
e){ return({ struct _tuple3* _temp694=( struct _tuple3*) GC_malloc( sizeof(
struct _tuple3)); _temp694->f1= Cyc_Toc_mt_tq; _temp694->f2= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _temp694;});} static
struct _tuple4* Cyc_Toc_add_designator( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp*
e){ Cyc_Toc_exp_to_c( nv, e); return({ struct _tuple4* _temp695=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp695->f1= 0; _temp695->f2= e; _temp695;});}
static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct( struct Cyc_Toc_Env* nv, struct
_tuple0* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer, struct Cyc_Absyn_Exp*
rgnopt, int is_atomic){ struct Cyc_Absyn_Exp* eo; void* t; if( pointer){ t= Cyc_Absyn_cstar_typ(
struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp* _temp696= Cyc_Absyn_sizeoftyp_exp(
struct_typ, 0); if( rgnopt == 0){ eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp(
t, is_atomic? Cyc_Toc_malloc_atomic( _temp696): Cyc_Toc_malloc_ptr( _temp696), 0);}
else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( rgnopt); Cyc_Toc_exp_to_c(
nv, r); eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp(
r, _temp696), 0);}}} else{ t= struct_typ; eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, t, eo, s, 0), 0);} static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
int e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, void* struct_type, struct Cyc_List_List*
dles, struct Cyc_Absyn_Stmt* s); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0,
struct Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) - 1;{ struct Cyc_List_List* _temp697=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( dles0); for( 0; _temp697 != 0; _temp697=((
struct Cyc_List_List*) _check_null( _temp697))->tl){ struct _tuple4 _temp700;
struct Cyc_Absyn_Exp* _temp701; struct Cyc_List_List* _temp703; struct _tuple4*
_temp698=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp697))->hd;
_temp700=* _temp698; _LL704: _temp703= _temp700.f1; goto _LL702; _LL702:
_temp701= _temp700.f2; goto _LL699; _LL699: { struct Cyc_Absyn_Exp* e_index; if(
_temp703 == 0){ e_index= Cyc_Absyn_signed_int_exp( count --, 0);} else{ if(((
struct Cyc_List_List*) _check_null( _temp703))->tl != 0){(( int(*)( struct
_tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("multiple designators in array",
sizeof( unsigned char), 30u));}{ void* _temp705=( void*)(( struct Cyc_List_List*)
_check_null( _temp703))->hd; void* _temp706= _temp705; struct Cyc_Absyn_Exp*
_temp712; _LL708: if(*(( int*) _temp706) == Cyc_Absyn_ArrayElement){ _LL713:
_temp712=(( struct Cyc_Absyn_ArrayElement_struct*) _temp706)->f1; goto _LL709;}
else{ goto _LL710;} _LL710: if(*(( int*) _temp706) == Cyc_Absyn_FieldName){ goto
_LL711;} else{ goto _LL707;} _LL709: Cyc_Toc_exp_to_c( nv, _temp712); e_index=
_temp712; goto _LL707; _LL711: e_index=(( struct Cyc_Absyn_Exp*(*)( struct
_tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("field name designators in array",
sizeof( unsigned char), 32u)); goto _LL707; _LL707:;}}{ struct Cyc_Absyn_Exp*
lval= Cyc_Absyn_subscript_exp( lhs, e_index, 0); void* _temp714=( void*)
_temp701->r; struct Cyc_List_List* _temp724; struct Cyc_Absyn_Exp* _temp726;
struct Cyc_Absyn_Exp* _temp728; struct Cyc_Absyn_Vardecl* _temp730; struct Cyc_List_List*
_temp732; void* _temp734; _LL716: if(*(( int*) _temp714) == Cyc_Absyn_Array_e){
_LL725: _temp724=(( struct Cyc_Absyn_Array_e_struct*) _temp714)->f1; goto _LL717;}
else{ goto _LL718;} _LL718: if(*(( int*) _temp714) == Cyc_Absyn_Comprehension_e){
_LL731: _temp730=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp714)->f1;
goto _LL729; _LL729: _temp728=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp714)->f2; goto _LL727; _LL727: _temp726=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp714)->f3; goto _LL719;} else{ goto _LL720;} _LL720: if(*(( int*) _temp714)
== Cyc_Absyn_AnonStruct_e){ _LL735: _temp734=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp714)->f1; goto _LL733; _LL733: _temp732=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp714)->f2; goto _LL721;} else{ goto _LL722;} _LL722: goto _LL723; _LL717: s=
Cyc_Toc_init_array( nv, lval, _temp724, s); goto _LL715; _LL719: s= Cyc_Toc_init_comprehension(
nv, lval, _temp730, _temp728, _temp726, s, 0); goto _LL715; _LL721: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp734, _temp732, s); goto _LL715; _LL723: Cyc_Toc_exp_to_c( nv,
_temp701); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
lhs, e_index, 0), _temp701, 0), s, 0); goto _LL715; _LL715:;}}}} return s;}
static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s, int
e1_already_translated){ struct _tuple0* _temp736= vd->name; if( !
e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env* nv2=
Cyc_Toc_add_varmap( nv, _temp736, Cyc_Absyn_varb_exp( _temp736,( void*)({ struct
Cyc_Absyn_Local_b_struct* _temp759=( struct Cyc_Absyn_Local_b_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Local_b_struct)); _temp759[ 0]=({ struct Cyc_Absyn_Local_b_struct
_temp760; _temp760.tag= Cyc_Absyn_Local_b; _temp760.f1= vd; _temp760;});
_temp759;}), 0)); struct _tuple0* max= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( _temp736, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp(
_temp736, 0), Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp(
Cyc_Absyn_var_exp( _temp736, 0), 0); struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp(
lhs, Cyc_Absyn_var_exp( _temp736, 0), 0); struct Cyc_Absyn_Stmt* body;{ void*
_temp737=( void*) e2->r; struct Cyc_List_List* _temp747; struct Cyc_Absyn_Exp*
_temp749; struct Cyc_Absyn_Exp* _temp751; struct Cyc_Absyn_Vardecl* _temp753;
struct Cyc_List_List* _temp755; void* _temp757; _LL739: if(*(( int*) _temp737)
== Cyc_Absyn_Array_e){ _LL748: _temp747=(( struct Cyc_Absyn_Array_e_struct*)
_temp737)->f1; goto _LL740;} else{ goto _LL741;} _LL741: if(*(( int*) _temp737)
== Cyc_Absyn_Comprehension_e){ _LL754: _temp753=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp737)->f1; goto _LL752; _LL752: _temp751=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp737)->f2; goto _LL750; _LL750: _temp749=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp737)->f3; goto _LL742;} else{ goto _LL743;} _LL743: if(*(( int*) _temp737)
== Cyc_Absyn_AnonStruct_e){ _LL758: _temp757=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp737)->f1; goto _LL756; _LL756: _temp755=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp737)->f2; goto _LL744;} else{ goto _LL745;} _LL745: goto _LL746; _LL740:
body= Cyc_Toc_init_array( nv2, lval, _temp747, Cyc_Absyn_skip_stmt( 0)); goto
_LL738; _LL742: body= Cyc_Toc_init_comprehension( nv2, lval, _temp753, _temp751,
_temp749, Cyc_Absyn_skip_stmt( 0), 0); goto _LL738; _LL744: body= Cyc_Toc_init_anon_struct(
nv, lval, _temp757, _temp755, Cyc_Absyn_skip_stmt( 0)); goto _LL738; _LL746: Cyc_Toc_exp_to_c(
nv2, e2); body= Cyc_Absyn_assign_stmt( lval, e2, 0); goto _LL738; _LL738:;}{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt( ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt(
_temp736, Cyc_Absyn_uint_t, 0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_anon_struct( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs,
void* struct_type, struct Cyc_List_List* dles, struct Cyc_Absyn_Stmt* s){{
struct Cyc_List_List* _temp761=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( dles); for( 0; _temp761 != 0; _temp761=(( struct Cyc_List_List*)
_check_null( _temp761))->tl){ struct _tuple4 _temp764; struct Cyc_Absyn_Exp*
_temp765; struct Cyc_List_List* _temp767; struct _tuple4* _temp762=( struct
_tuple4*)(( struct Cyc_List_List*) _check_null( _temp761))->hd; _temp764=*
_temp762; _LL768: _temp767= _temp764.f1; goto _LL766; _LL766: _temp765= _temp764.f2;
goto _LL763; _LL763: if( _temp767 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("empty designator list", sizeof( unsigned char), 22u));} if((( struct
Cyc_List_List*) _check_null( _temp767))->tl != 0){(( int(*)( struct _tagged_arr
s)) Cyc_Toc_toc_impos)( _tag_arr("too many designators in anonymous struct",
sizeof( unsigned char), 41u));}{ void* _temp769=( void*)(( struct Cyc_List_List*)
_check_null( _temp767))->hd; struct _tagged_arr* _temp775; _LL771: if(*(( int*)
_temp769) == Cyc_Absyn_FieldName){ _LL776: _temp775=(( struct Cyc_Absyn_FieldName_struct*)
_temp769)->f1; goto _LL772;} else{ goto _LL773;} _LL773: goto _LL774; _LL772: {
struct Cyc_Absyn_Exp* lval= Cyc_Absyn_structmember_exp( lhs, _temp775, 0);{ void*
_temp777=( void*) _temp765->r; struct Cyc_List_List* _temp787; struct Cyc_Absyn_Exp*
_temp789; struct Cyc_Absyn_Exp* _temp791; struct Cyc_Absyn_Vardecl* _temp793;
struct Cyc_List_List* _temp795; void* _temp797; _LL779: if(*(( int*) _temp777)
== Cyc_Absyn_Array_e){ _LL788: _temp787=(( struct Cyc_Absyn_Array_e_struct*)
_temp777)->f1; goto _LL780;} else{ goto _LL781;} _LL781: if(*(( int*) _temp777)
== Cyc_Absyn_Comprehension_e){ _LL794: _temp793=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp777)->f1; goto _LL792; _LL792: _temp791=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp777)->f2; goto _LL790; _LL790: _temp789=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp777)->f3; goto _LL782;} else{ goto _LL783;} _LL783: if(*(( int*) _temp777)
== Cyc_Absyn_AnonStruct_e){ _LL798: _temp797=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp777)->f1; goto _LL796; _LL796: _temp795=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp777)->f2; goto _LL784;} else{ goto _LL785;} _LL785: goto _LL786; _LL780: s=
Cyc_Toc_init_array( nv, lval, _temp787, s); goto _LL778; _LL782: s= Cyc_Toc_init_comprehension(
nv, lval, _temp793, _temp791, _temp789, s, 0); goto _LL778; _LL784: s= Cyc_Toc_init_anon_struct(
nv, lval, _temp797, _temp795, s); goto _LL778; _LL786: Cyc_Toc_exp_to_c( nv,
_temp765); if( Cyc_Toc_is_poly_field( struct_type, _temp775)){ _temp765= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp765, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp765, 0), 0), s, 0); goto _LL778; _LL778:;} goto
_LL770;} _LL774:( int) _throw( Cyc_Toc_toc_impos( _tag_arr("array designator in struct",
sizeof( unsigned char), 27u))); _LL770:;}}} return s;} static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple( struct Cyc_Toc_Env* nv, int pointer, struct Cyc_Absyn_Exp*
rgnopt, struct Cyc_List_List* es){ struct _RegionHandle _temp799= _new_region();
struct _RegionHandle* r=& _temp799; _push_region( r);{ struct Cyc_List_List*
_temp800=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple3*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_rmap)( r, Cyc_Toc_tup_to_c,
es); struct _tagged_arr* _temp801= Cyc_Toc_add_tuple_type( _temp800); struct
_tuple0* _temp802= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp803= Cyc_Absyn_var_exp(
_temp802, 0); struct Cyc_Absyn_Stmt* _temp804= Cyc_Absyn_exp_stmt( _temp803, 0);
struct Cyc_Absyn_Exp*(* _temp805)( struct Cyc_Absyn_Exp*, struct _tagged_arr*,
struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp;
int is_atomic= 1; struct Cyc_List_List* _temp806=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, es);{ int i=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp806); for( 0; _temp806
!= 0;( _temp806=(( struct Cyc_List_List*) _check_null( _temp806))->tl, -- i)){
struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp806))->hd; struct Cyc_Absyn_Exp* lval= _temp805( _temp803, Cyc_Toc_fieldname(
i), 0); is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v): 0;{ void* _temp807=( void*) e->r; struct Cyc_List_List*
_temp815; struct Cyc_Absyn_Exp* _temp817; struct Cyc_Absyn_Exp* _temp819; struct
Cyc_Absyn_Vardecl* _temp821; _LL809: if(*(( int*) _temp807) == Cyc_Absyn_Array_e){
_LL816: _temp815=(( struct Cyc_Absyn_Array_e_struct*) _temp807)->f1; goto _LL810;}
else{ goto _LL811;} _LL811: if(*(( int*) _temp807) == Cyc_Absyn_Comprehension_e){
_LL822: _temp821=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp807)->f1;
goto _LL820; _LL820: _temp819=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp807)->f2; goto _LL818; _LL818: _temp817=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp807)->f3; goto _LL812;} else{ goto _LL813;} _LL813: goto _LL814; _LL810:
_temp804= Cyc_Toc_init_array( nv, lval, _temp815, _temp804); goto _LL808; _LL812:
_temp804= Cyc_Toc_init_comprehension( nv, lval, _temp821, _temp819, _temp817,
_temp804, 0); goto _LL808; _LL814: Cyc_Toc_exp_to_c( nv, e); _temp804= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp805( _temp803, Cyc_Toc_fieldname(
i), 0), e, 0), 0), _temp804, 0); goto _LL808; _LL808:;}}}{ struct Cyc_Absyn_Exp*
_temp823= Cyc_Toc_make_struct( nv, _temp802, Cyc_Absyn_strct( _temp801),
_temp804, pointer, rgnopt, is_atomic); _npop_handler( 0u); return _temp823;}};
_pop_region( r);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env*
nv, void* struct_type, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
dles, struct _tuple0* tdn){ struct _tuple0* _temp824= Cyc_Toc_temp_var(); struct
Cyc_Absyn_Exp* _temp825= Cyc_Absyn_var_exp( _temp824, 0); struct Cyc_Absyn_Stmt*
_temp826= Cyc_Absyn_exp_stmt( _temp825, 0); struct Cyc_Absyn_Exp*(* _temp827)(
struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; int is_atomic= 1;
struct _RegionHandle _temp828= _new_region(); struct _RegionHandle* r=& _temp828;
_push_region( r);{ struct Cyc_List_List* _temp829=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, dles); for(
0; _temp829 != 0; _temp829=(( struct Cyc_List_List*) _check_null( _temp829))->tl){
struct _tuple4 _temp832; struct Cyc_Absyn_Exp* _temp833; struct Cyc_List_List*
_temp835; struct _tuple4* _temp830=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp829))->hd; _temp832=* _temp830; _LL836: _temp835= _temp832.f1;
goto _LL834; _LL834: _temp833= _temp832.f2; goto _LL831; _LL831: is_atomic=
is_atomic? Cyc_Toc_atomic_typ(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp833->topt))->v): 0; if( _temp835 == 0){(( int(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("empty designator list", sizeof( unsigned char), 22u));}
if((( struct Cyc_List_List*) _check_null( _temp835))->tl != 0){ struct _tuple0*
_temp837= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp838= Cyc_Absyn_var_exp(
_temp837, 0); for( 0; _temp835 != 0; _temp835=(( struct Cyc_List_List*)
_check_null( _temp835))->tl){ void* _temp839=( void*)(( struct Cyc_List_List*)
_check_null( _temp835))->hd; struct _tagged_arr* _temp845; _LL841: if(*(( int*)
_temp839) == Cyc_Absyn_FieldName){ _LL846: _temp845=(( struct Cyc_Absyn_FieldName_struct*)
_temp839)->f1; goto _LL842;} else{ goto _LL843;} _LL843: goto _LL844; _LL842:
if( Cyc_Toc_is_poly_field( struct_type, _temp845)){ _temp838= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp838, 0);} _temp826= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( _temp827( _temp825, _temp845, 0), _temp838, 0), 0),
_temp826, 0); goto _LL840; _LL844:( int) _throw( Cyc_Toc_toc_impos( _tag_arr("array designator in struct",
sizeof( unsigned char), 27u))); _LL840:;} Cyc_Toc_exp_to_c( nv, _temp833);
_temp826= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( _temp838,
_temp833, 0), 0), _temp826, 0);} else{ void* _temp847=( void*)(( struct Cyc_List_List*)
_check_null( _temp835))->hd; struct _tagged_arr* _temp853; _LL849: if(*(( int*)
_temp847) == Cyc_Absyn_FieldName){ _LL854: _temp853=(( struct Cyc_Absyn_FieldName_struct*)
_temp847)->f1; goto _LL850;} else{ goto _LL851;} _LL851: goto _LL852; _LL850: {
struct Cyc_Absyn_Exp* lval= _temp827( _temp825, _temp853, 0);{ void* _temp855=(
void*) _temp833->r; struct Cyc_List_List* _temp865; struct Cyc_Absyn_Exp*
_temp867; struct Cyc_Absyn_Exp* _temp869; struct Cyc_Absyn_Vardecl* _temp871;
struct Cyc_List_List* _temp873; void* _temp875; _LL857: if(*(( int*) _temp855)
== Cyc_Absyn_Array_e){ _LL866: _temp865=(( struct Cyc_Absyn_Array_e_struct*)
_temp855)->f1; goto _LL858;} else{ goto _LL859;} _LL859: if(*(( int*) _temp855)
== Cyc_Absyn_Comprehension_e){ _LL872: _temp871=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp855)->f1; goto _LL870; _LL870: _temp869=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp855)->f2; goto _LL868; _LL868: _temp867=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp855)->f3; goto _LL860;} else{ goto _LL861;} _LL861: if(*(( int*) _temp855)
== Cyc_Absyn_AnonStruct_e){ _LL876: _temp875=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp855)->f1; goto _LL874; _LL874: _temp873=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp855)->f2; goto _LL862;} else{ goto _LL863;} _LL863: goto _LL864; _LL858:
_temp826= Cyc_Toc_init_array( nv, lval, _temp865, _temp826); goto _LL856; _LL860:
_temp826= Cyc_Toc_init_comprehension( nv, lval, _temp871, _temp869, _temp867,
_temp826, 0); goto _LL856; _LL862: _temp826= Cyc_Toc_init_anon_struct( nv, lval,
_temp875, _temp873, _temp826); goto _LL856; _LL864: Cyc_Toc_exp_to_c( nv,
_temp833); if( Cyc_Toc_is_poly_field( struct_type, _temp853)){ _temp833= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp833, 0);} _temp826= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval, _temp833, 0), 0), _temp826, 0); goto _LL856; _LL856:;}
goto _LL848;} _LL852:( int) _throw( Cyc_Toc_toc_impos( _tag_arr("array designator in struct",
sizeof( unsigned char), 27u))); _LL848:;}}}{ struct Cyc_Absyn_Exp* _temp877= Cyc_Toc_make_struct(
nv, _temp824, Cyc_Absyn_strctq( tdn), _temp826, pointer, rgnopt, is_atomic);
_npop_handler( 0u); return _temp877;}; _pop_region( r);} struct _tuple9{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ; static struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue(
struct Cyc_Absyn_Exp* el, struct _tuple9* pr){ return Cyc_Absyn_assignop_exp( el,(*
pr).f1,(* pr).f2, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue(
struct Cyc_Absyn_Exp* e1, int ignore){ return Cyc_Absyn_address_exp( e1, 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp* e1, void*
incr){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Increment_e_struct*
_temp878=( struct Cyc_Absyn_Increment_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct));
_temp878[ 0]=({ struct Cyc_Absyn_Increment_e_struct _temp879; _temp879.tag= Cyc_Absyn_Increment_e;
_temp879.f1= e1; _temp879.f2=( void*) incr; _temp879;}); _temp878;}), 0);}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env);
static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp880=( void*) e1->r; struct Cyc_Absyn_Stmt* _temp890; struct Cyc_Absyn_Exp*
_temp892; void* _temp894; struct _tagged_arr* _temp896; struct Cyc_Absyn_Exp*
_temp898; _LL882: if(*(( int*) _temp880) == Cyc_Absyn_StmtExp_e){ _LL891:
_temp890=(( struct Cyc_Absyn_StmtExp_e_struct*) _temp880)->f1; goto _LL883;}
else{ goto _LL884;} _LL884: if(*(( int*) _temp880) == Cyc_Absyn_Cast_e){ _LL895:
_temp894=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp880)->f1; goto _LL893;
_LL893: _temp892=(( struct Cyc_Absyn_Cast_e_struct*) _temp880)->f2; goto _LL885;}
else{ goto _LL886;} _LL886: if(*(( int*) _temp880) == Cyc_Absyn_StructMember_e){
_LL899: _temp898=(( struct Cyc_Absyn_StructMember_e_struct*) _temp880)->f1; goto
_LL897; _LL897: _temp896=(( struct Cyc_Absyn_StructMember_e_struct*) _temp880)->f2;
goto _LL887;} else{ goto _LL888;} _LL888: goto _LL889; _LL883: Cyc_Toc_lvalue_assign_stmt(
_temp890, fs, f, f_env); goto _LL881; _LL885: Cyc_Toc_lvalue_assign( _temp892,
fs, f, f_env); goto _LL881; _LL887:( void*)( e1->r=( void*)(( void*) _temp898->r));
Cyc_Toc_lvalue_assign( e1,({ struct Cyc_List_List* _temp900=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp900->hd=( void*) _temp896;
_temp900->tl= fs; _temp900;}), f, f_env); goto _LL881; _LL889: if( ! Cyc_Absyn_is_lvalue(
e1)){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({
struct _tagged_arr(* _temp901)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp905= _tag_arr("lvalue_assign: could not find lvalue in `%s'",
sizeof( unsigned char), 45u); struct Cyc_Stdio_String_pa_struct _temp906;
_temp906.tag= Cyc_Stdio_String_pa; _temp906.f1=( struct _tagged_arr) Cyc_Absynpp_exp2string(
e1);{ void* _temp904=( void*)& _temp906; void* _temp902[ 1u]={ _temp904}; struct
_tagged_arr _temp903={( void*) _temp902,( void*) _temp902,( void*)( _temp902 + 1u)};
_temp901( _temp905, _temp903);}}));}{ struct Cyc_Absyn_Exp* e1_copy= Cyc_Absyn_copy_exp(
e1); for( 0; fs != 0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){
e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( fs))->hd, e1_copy->loc);}( void*)( e1->r=( void*)(( void*)( f(
e1_copy, f_env))->r)); goto _LL881;} _LL881:;} static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env){ void* _temp907=( void*) s->r;
struct Cyc_Absyn_Exp* _temp917; struct Cyc_Absyn_Stmt* _temp919; struct Cyc_Absyn_Decl*
_temp921; struct Cyc_Absyn_Stmt* _temp923; _LL909: if(( unsigned int) _temp907 >
1u?*(( int*) _temp907) == Cyc_Absyn_Exp_s: 0){ _LL918: _temp917=(( struct Cyc_Absyn_Exp_s_struct*)
_temp907)->f1; goto _LL910;} else{ goto _LL911;} _LL911: if(( unsigned int)
_temp907 > 1u?*(( int*) _temp907) == Cyc_Absyn_Decl_s: 0){ _LL922: _temp921=((
struct Cyc_Absyn_Decl_s_struct*) _temp907)->f1; goto _LL920; _LL920: _temp919=((
struct Cyc_Absyn_Decl_s_struct*) _temp907)->f2; goto _LL912;} else{ goto _LL913;}
_LL913: if(( unsigned int) _temp907 > 1u?*(( int*) _temp907) == Cyc_Absyn_Seq_s:
0){ _LL924: _temp923=(( struct Cyc_Absyn_Seq_s_struct*) _temp907)->f2; goto
_LL914;} else{ goto _LL915;} _LL915: goto _LL916; _LL910: Cyc_Toc_lvalue_assign(
_temp917, fs, f, f_env); goto _LL908; _LL912: Cyc_Toc_lvalue_assign_stmt(
_temp919, fs, f, f_env); goto _LL908; _LL914: Cyc_Toc_lvalue_assign_stmt(
_temp923, fs, f, f_env); goto _LL908; _LL916:(( int(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct _tagged_arr(* _temp925)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp929= _tag_arr("lvalue_assign_stmt: %s", sizeof( unsigned char),
23u); struct Cyc_Stdio_String_pa_struct _temp930; _temp930.tag= Cyc_Stdio_String_pa;
_temp930.f1=( struct _tagged_arr) Cyc_Absynpp_stmt2string( s);{ void* _temp928=(
void*)& _temp930; void* _temp926[ 1u]={ _temp928}; struct _tagged_arr _temp927={(
void*) _temp926,( void*) _temp926,( void*)( _temp926 + 1u)}; _temp925( _temp929,
_temp927);}})); goto _LL908; _LL908:;} static struct Cyc_List_List* Cyc_Toc_rmap_2c(
struct _RegionHandle* r2, void*(* f)( void*, void*), void* env, struct Cyc_List_List*
x){ struct Cyc_List_List* result; struct Cyc_List_List* prev; if( x == 0){
return 0;} result=({ struct Cyc_List_List* _temp931=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp931->hd=( void*) f((
void*)(( struct Cyc_List_List*) _check_null( x))->hd, env); _temp931->tl= 0;
_temp931;}); prev= result; for( x=(( struct Cyc_List_List*) _check_null( x))->tl;
x != 0; x=(( struct Cyc_List_List*) _check_null( x))->tl){(( struct Cyc_List_List*)
_check_null( prev))->tl=({ struct Cyc_List_List* _temp932=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp932->hd=( void*) f((
void*)(( struct Cyc_List_List*) _check_null( x))->hd, env); _temp932->tl= 0;
_temp932;}); prev=(( struct Cyc_List_List*) _check_null( prev))->tl;} return
result;} static struct Cyc_List_List* Cyc_Toc_map_2c( void*(* f)( void*, void*),
void* env, struct Cyc_List_List* x){ return Cyc_Toc_rmap_2c( Cyc_Core_heap_region,
f, env, x);} static struct _tuple4* Cyc_Toc_make_dle( struct Cyc_Absyn_Exp* e){
return({ struct _tuple4* _temp933=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp933->f1= 0; _temp933->f2= e; _temp933;});} static struct Cyc_Absyn_PtrInfo
Cyc_Toc_get_ptr_type( void* t){ void* _temp934= Cyc_Tcutil_compress( t); struct
Cyc_Absyn_PtrInfo _temp940; _LL936: if(( unsigned int) _temp934 > 4u?*(( int*)
_temp934) == Cyc_Absyn_PointerType: 0){ _LL941: _temp940=(( struct Cyc_Absyn_PointerType_struct*)
_temp934)->f1; goto _LL937;} else{ goto _LL938;} _LL938: goto _LL939; _LL937:
return _temp940; _LL939:( int) _throw( Cyc_Tcutil_impos( _tag_arr("get_ptr_typ:  not a pointer!",
sizeof( unsigned char), 29u))); _LL935:;} struct _tuple10{ void* f1; void* f2; }
; struct _tuple11{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Exp* f2; }
; static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e){
void* _temp942=( void*) e->r; if( e->topt == 0){(( int(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct _tagged_arr(* _temp943)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp947= _tag_arr("exp_to_c: no type for %s", sizeof( unsigned char),
25u); struct Cyc_Stdio_String_pa_struct _temp948; _temp948.tag= Cyc_Stdio_String_pa;
_temp948.f1=( struct _tagged_arr) Cyc_Absynpp_exp2string( e);{ void* _temp946=(
void*)& _temp948; void* _temp944[ 1u]={ _temp946}; struct _tagged_arr _temp945={(
void*) _temp944,( void*) _temp944,( void*)( _temp944 + 1u)}; _temp943( _temp947,
_temp945);}})); return;}{ void* old_typ=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; void* _temp949= _temp942; void* _temp1029; void*
_temp1031; struct _tuple0* _temp1033; struct _tuple0* _temp1035; struct Cyc_List_List*
_temp1037; void* _temp1039; void* _temp1041; struct Cyc_Absyn_Exp* _temp1043;
struct Cyc_Absyn_Exp* _temp1045; struct Cyc_Core_Opt* _temp1047; struct Cyc_Absyn_Exp*
_temp1049; struct Cyc_Absyn_Exp* _temp1051; struct Cyc_Absyn_Exp* _temp1053;
struct Cyc_Absyn_Exp* _temp1055; struct Cyc_Absyn_Exp* _temp1057; struct Cyc_Absyn_Exp*
_temp1059; struct Cyc_List_List* _temp1061; struct Cyc_Absyn_Exp* _temp1063;
struct Cyc_Absyn_VarargCallInfo* _temp1065; struct Cyc_List_List* _temp1067;
struct Cyc_Absyn_Exp* _temp1069; struct Cyc_Absyn_VarargCallInfo* _temp1071;
struct Cyc_Absyn_VarargCallInfo _temp1073; struct Cyc_Absyn_VarargInfo*
_temp1074; struct Cyc_List_List* _temp1076; int _temp1078; struct Cyc_List_List*
_temp1080; struct Cyc_Absyn_Exp* _temp1082; struct Cyc_Absyn_Exp* _temp1084;
struct Cyc_Absyn_Exp* _temp1086; struct Cyc_List_List* _temp1088; struct Cyc_Absyn_Exp*
_temp1090; struct Cyc_Absyn_Exp* _temp1092; void* _temp1094; void** _temp1096;
struct Cyc_Absyn_Exp* _temp1097; struct Cyc_Absyn_Exp* _temp1099; struct Cyc_Absyn_Exp*
_temp1101; struct Cyc_Absyn_Exp* _temp1103; void* _temp1105; struct _tagged_arr*
_temp1107; void* _temp1109; struct Cyc_Absyn_Exp* _temp1111; struct _tagged_arr*
_temp1113; struct Cyc_Absyn_Exp* _temp1115; struct _tagged_arr* _temp1117;
struct Cyc_Absyn_Exp* _temp1119; struct Cyc_Absyn_Exp* _temp1121; struct Cyc_Absyn_Exp*
_temp1123; struct Cyc_List_List* _temp1125; struct Cyc_List_List* _temp1127;
struct Cyc_Absyn_Exp* _temp1129; struct Cyc_Absyn_Exp* _temp1131; struct Cyc_Absyn_Vardecl*
_temp1133; struct Cyc_Absyn_Structdecl* _temp1135; struct Cyc_List_List*
_temp1137; struct Cyc_Core_Opt* _temp1139; struct _tuple0* _temp1141; struct Cyc_List_List*
_temp1143; void* _temp1145; struct Cyc_Absyn_Tunionfield* _temp1147; struct Cyc_Absyn_Tuniondecl*
_temp1149; struct Cyc_List_List* _temp1151; struct Cyc_Absyn_Tunionfield*
_temp1153; struct Cyc_Absyn_Tuniondecl* _temp1155; struct Cyc_List_List*
_temp1157; void* _temp1159; struct Cyc_Absyn_Exp* _temp1161; struct Cyc_Absyn_Stmt*
_temp1163; struct Cyc_Absyn_Fndecl* _temp1165; _LL951: if(*(( int*) _temp949) ==
Cyc_Absyn_Const_e){ _LL1030: _temp1029=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp949)->f1; if( _temp1029 ==( void*) Cyc_Absyn_Null_c){ goto _LL952;} else{
goto _LL953;}} else{ goto _LL953;} _LL953: if(*(( int*) _temp949) == Cyc_Absyn_Const_e){
goto _LL954;} else{ goto _LL955;} _LL955: if(*(( int*) _temp949) == Cyc_Absyn_Var_e){
_LL1034: _temp1033=(( struct Cyc_Absyn_Var_e_struct*) _temp949)->f1; goto
_LL1032; _LL1032: _temp1031=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp949)->f2;
goto _LL956;} else{ goto _LL957;} _LL957: if(*(( int*) _temp949) == Cyc_Absyn_UnknownId_e){
_LL1036: _temp1035=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp949)->f1; goto
_LL958;} else{ goto _LL959;} _LL959: if(*(( int*) _temp949) == Cyc_Absyn_Primop_e){
_LL1040: _temp1039=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp949)->f1;
goto _LL1038; _LL1038: _temp1037=(( struct Cyc_Absyn_Primop_e_struct*) _temp949)->f2;
goto _LL960;} else{ goto _LL961;} _LL961: if(*(( int*) _temp949) == Cyc_Absyn_Increment_e){
_LL1044: _temp1043=(( struct Cyc_Absyn_Increment_e_struct*) _temp949)->f1; goto
_LL1042; _LL1042: _temp1041=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp949)->f2; goto _LL962;} else{ goto _LL963;} _LL963: if(*(( int*) _temp949)
== Cyc_Absyn_AssignOp_e){ _LL1050: _temp1049=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp949)->f1; goto _LL1048; _LL1048: _temp1047=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp949)->f2; goto _LL1046; _LL1046: _temp1045=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp949)->f3; goto _LL964;} else{ goto _LL965;} _LL965: if(*(( int*) _temp949)
== Cyc_Absyn_Conditional_e){ _LL1056: _temp1055=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp949)->f1; goto _LL1054; _LL1054: _temp1053=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp949)->f2; goto _LL1052; _LL1052: _temp1051=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp949)->f3; goto _LL966;} else{ goto _LL967;} _LL967: if(*(( int*) _temp949)
== Cyc_Absyn_SeqExp_e){ _LL1060: _temp1059=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp949)->f1; goto _LL1058; _LL1058: _temp1057=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp949)->f2; goto _LL968;} else{ goto _LL969;} _LL969: if(*(( int*) _temp949)
== Cyc_Absyn_UnknownCall_e){ _LL1064: _temp1063=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp949)->f1; goto _LL1062; _LL1062: _temp1061=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp949)->f2; goto _LL970;} else{ goto _LL971;} _LL971: if(*(( int*) _temp949)
== Cyc_Absyn_FnCall_e){ _LL1070: _temp1069=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp949)->f1; goto _LL1068; _LL1068: _temp1067=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp949)->f2; goto _LL1066; _LL1066: _temp1065=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp949)->f3; if( _temp1065 == 0){ goto _LL972;} else{ goto _LL973;}} else{
goto _LL973;} _LL973: if(*(( int*) _temp949) == Cyc_Absyn_FnCall_e){ _LL1083:
_temp1082=(( struct Cyc_Absyn_FnCall_e_struct*) _temp949)->f1; goto _LL1081;
_LL1081: _temp1080=(( struct Cyc_Absyn_FnCall_e_struct*) _temp949)->f2; goto
_LL1072; _LL1072: _temp1071=(( struct Cyc_Absyn_FnCall_e_struct*) _temp949)->f3;
if( _temp1071 == 0){ goto _LL975;} else{ _temp1073=* _temp1071; _LL1079:
_temp1078= _temp1073.num_varargs; goto _LL1077; _LL1077: _temp1076= _temp1073.injectors;
goto _LL1075; _LL1075: _temp1074= _temp1073.vai; goto _LL974;}} else{ goto
_LL975;} _LL975: if(*(( int*) _temp949) == Cyc_Absyn_Throw_e){ _LL1085:
_temp1084=(( struct Cyc_Absyn_Throw_e_struct*) _temp949)->f1; goto _LL976;}
else{ goto _LL977;} _LL977: if(*(( int*) _temp949) == Cyc_Absyn_NoInstantiate_e){
_LL1087: _temp1086=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp949)->f1;
goto _LL978;} else{ goto _LL979;} _LL979: if(*(( int*) _temp949) == Cyc_Absyn_Instantiate_e){
_LL1091: _temp1090=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp949)->f1;
goto _LL1089; _LL1089: _temp1088=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp949)->f2; goto _LL980;} else{ goto _LL981;} _LL981: if(*(( int*) _temp949)
== Cyc_Absyn_Cast_e){ _LL1095: _temp1094=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp949)->f1; _temp1096=&(( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp949)->f1);
goto _LL1093; _LL1093: _temp1092=(( struct Cyc_Absyn_Cast_e_struct*) _temp949)->f2;
goto _LL982;} else{ goto _LL983;} _LL983: if(*(( int*) _temp949) == Cyc_Absyn_Address_e){
_LL1098: _temp1097=(( struct Cyc_Absyn_Address_e_struct*) _temp949)->f1; goto
_LL984;} else{ goto _LL985;} _LL985: if(*(( int*) _temp949) == Cyc_Absyn_New_e){
_LL1102: _temp1101=(( struct Cyc_Absyn_New_e_struct*) _temp949)->f1; goto
_LL1100; _LL1100: _temp1099=(( struct Cyc_Absyn_New_e_struct*) _temp949)->f2;
goto _LL986;} else{ goto _LL987;} _LL987: if(*(( int*) _temp949) == Cyc_Absyn_Sizeofexp_e){
_LL1104: _temp1103=(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp949)->f1; goto
_LL988;} else{ goto _LL989;} _LL989: if(*(( int*) _temp949) == Cyc_Absyn_Sizeoftyp_e){
_LL1106: _temp1105=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp949)->f1;
goto _LL990;} else{ goto _LL991;} _LL991: if(*(( int*) _temp949) == Cyc_Absyn_Offsetof_e){
_LL1110: _temp1109=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*) _temp949)->f1;
goto _LL1108; _LL1108: _temp1107=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp949)->f2; goto _LL992;} else{ goto _LL993;} _LL993: if(*(( int*) _temp949)
== Cyc_Absyn_Deref_e){ _LL1112: _temp1111=(( struct Cyc_Absyn_Deref_e_struct*)
_temp949)->f1; goto _LL994;} else{ goto _LL995;} _LL995: if(*(( int*) _temp949)
== Cyc_Absyn_StructMember_e){ _LL1116: _temp1115=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp949)->f1; goto _LL1114; _LL1114: _temp1113=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp949)->f2; goto _LL996;} else{ goto _LL997;} _LL997: if(*(( int*) _temp949)
== Cyc_Absyn_StructArrow_e){ _LL1120: _temp1119=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp949)->f1; goto _LL1118; _LL1118: _temp1117=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp949)->f2; goto _LL998;} else{ goto _LL999;} _LL999: if(*(( int*) _temp949)
== Cyc_Absyn_Subscript_e){ _LL1124: _temp1123=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp949)->f1; goto _LL1122; _LL1122: _temp1121=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp949)->f2; goto _LL1000;} else{ goto _LL1001;} _LL1001: if(*(( int*)
_temp949) == Cyc_Absyn_Tuple_e){ _LL1126: _temp1125=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp949)->f1; goto _LL1002;} else{ goto _LL1003;} _LL1003: if(*(( int*)
_temp949) == Cyc_Absyn_Array_e){ _LL1128: _temp1127=(( struct Cyc_Absyn_Array_e_struct*)
_temp949)->f1; goto _LL1004;} else{ goto _LL1005;} _LL1005: if(*(( int*)
_temp949) == Cyc_Absyn_Comprehension_e){ _LL1134: _temp1133=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp949)->f1; goto _LL1132; _LL1132: _temp1131=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp949)->f2; goto _LL1130; _LL1130: _temp1129=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp949)->f3; goto _LL1006;} else{ goto _LL1007;} _LL1007: if(*(( int*)
_temp949) == Cyc_Absyn_Struct_e){ _LL1142: _temp1141=(( struct Cyc_Absyn_Struct_e_struct*)
_temp949)->f1; goto _LL1140; _LL1140: _temp1139=(( struct Cyc_Absyn_Struct_e_struct*)
_temp949)->f2; goto _LL1138; _LL1138: _temp1137=(( struct Cyc_Absyn_Struct_e_struct*)
_temp949)->f3; goto _LL1136; _LL1136: _temp1135=(( struct Cyc_Absyn_Struct_e_struct*)
_temp949)->f4; goto _LL1008;} else{ goto _LL1009;} _LL1009: if(*(( int*)
_temp949) == Cyc_Absyn_AnonStruct_e){ _LL1146: _temp1145=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp949)->f1; goto _LL1144; _LL1144: _temp1143=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp949)->f2; goto _LL1010;} else{ goto _LL1011;} _LL1011: if(*(( int*)
_temp949) == Cyc_Absyn_Tunion_e){ _LL1152: _temp1151=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp949)->f3; if( _temp1151 == 0){ goto _LL1150;} else{ goto _LL1013;} _LL1150:
_temp1149=(( struct Cyc_Absyn_Tunion_e_struct*) _temp949)->f4; goto _LL1148;
_LL1148: _temp1147=(( struct Cyc_Absyn_Tunion_e_struct*) _temp949)->f5; goto
_LL1012;} else{ goto _LL1013;} _LL1013: if(*(( int*) _temp949) == Cyc_Absyn_Tunion_e){
_LL1158: _temp1157=(( struct Cyc_Absyn_Tunion_e_struct*) _temp949)->f3; goto
_LL1156; _LL1156: _temp1155=(( struct Cyc_Absyn_Tunion_e_struct*) _temp949)->f4;
goto _LL1154; _LL1154: _temp1153=(( struct Cyc_Absyn_Tunion_e_struct*) _temp949)->f5;
goto _LL1014;} else{ goto _LL1015;} _LL1015: if(*(( int*) _temp949) == Cyc_Absyn_Enum_e){
goto _LL1016;} else{ goto _LL1017;} _LL1017: if(*(( int*) _temp949) == Cyc_Absyn_Malloc_e){
_LL1162: _temp1161=(( struct Cyc_Absyn_Malloc_e_struct*) _temp949)->f1; goto
_LL1160; _LL1160: _temp1159=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp949)->f2; goto _LL1018;} else{ goto _LL1019;} _LL1019: if(*(( int*)
_temp949) == Cyc_Absyn_StmtExp_e){ _LL1164: _temp1163=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp949)->f1; goto _LL1020;} else{ goto _LL1021;} _LL1021: if(*(( int*)
_temp949) == Cyc_Absyn_UnresolvedMem_e){ goto _LL1022;} else{ goto _LL1023;}
_LL1023: if(*(( int*) _temp949) == Cyc_Absyn_CompoundLit_e){ goto _LL1024;}
else{ goto _LL1025;} _LL1025: if(*(( int*) _temp949) == Cyc_Absyn_Codegen_e){
_LL1166: _temp1165=(( struct Cyc_Absyn_Codegen_e_struct*) _temp949)->f1; goto
_LL1026;} else{ goto _LL1027;} _LL1027: if(*(( int*) _temp949) == Cyc_Absyn_Fill_e){
goto _LL1028;} else{ goto _LL950;} _LL952: { struct Cyc_Absyn_Exp* _temp1167=
Cyc_Absyn_uint_exp( 0, 0); if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){ if(
nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr(
old_typ, _temp1167, _temp1167))->r));} else{( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({ struct Cyc_List_List*(* _temp1168)(
struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1171= _temp1167; struct Cyc_Absyn_Exp* _temp1172=
_temp1167; struct Cyc_Absyn_Exp* _temp1173= _temp1167; struct Cyc_Absyn_Exp*
_temp1169[ 3u]={ _temp1171, _temp1172, _temp1173}; struct _tagged_arr _temp1170={(
void*) _temp1169,( void*) _temp1169,( void*)( _temp1169 + 3u)}; _temp1168(
_temp1170);}), 0))->r));}} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp(
0, 0))->r));} goto _LL950;} _LL954: goto _LL950; _LL956:{ struct _handler_cons
_temp1174; _push_handler(& _temp1174);{ int _temp1176= 0; if( setjmp( _temp1174.handler)){
_temp1176= 1;} if( ! _temp1176){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( nv->varmap,
_temp1033))->r));; _pop_handler();} else{ void* _temp1175=( void*) _exn_thrown;
void* _temp1178= _temp1175; _LL1180: if( _temp1178 == Cyc_Dict_Absent){ goto
_LL1181;} else{ goto _LL1182;} _LL1182: goto _LL1183; _LL1181:(( int(*)( struct
_tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct _tagged_arr(*
_temp1184)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1188= _tag_arr("Can't find %s in exp_to_c, Var\n",
sizeof( unsigned char), 32u); struct Cyc_Stdio_String_pa_struct _temp1189;
_temp1189.tag= Cyc_Stdio_String_pa; _temp1189.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp1033);{ void* _temp1187=( void*)& _temp1189; void* _temp1185[ 1u]={
_temp1187}; struct _tagged_arr _temp1186={( void*) _temp1185,( void*) _temp1185,(
void*)( _temp1185 + 1u)}; _temp1184( _temp1188, _temp1186);}})); return; _LL1183:(
void) _throw( _temp1178); _LL1179:;}}} goto _LL950; _LL958:(( int(*)( struct
_tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknownid", sizeof( unsigned char),
10u)); goto _LL950; _LL960: { struct Cyc_List_List* _temp1190=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ,
_temp1037); struct Cyc_List_List* _temp1191=(( struct Cyc_List_List*(*)( void*(*
f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ,
_temp1037);(( void(*)( void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*),
struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c,
nv, _temp1037);{ void* _temp1192= _temp1039; _LL1194: if( _temp1192 ==( void*)
Cyc_Absyn_Size){ goto _LL1195;} else{ goto _LL1196;} _LL1196: if( _temp1192 ==(
void*) Cyc_Absyn_Printf){ goto _LL1197;} else{ goto _LL1198;} _LL1198: if(
_temp1192 ==( void*) Cyc_Absyn_Aprintf){ goto _LL1199;} else{ goto _LL1200;}
_LL1200: if( _temp1192 ==( void*) Cyc_Absyn_Fprintf){ goto _LL1201;} else{ goto
_LL1202;} _LL1202: if( _temp1192 ==( void*) Cyc_Absyn_Sscanf){ goto _LL1203;}
else{ goto _LL1204;} _LL1204: if( _temp1192 ==( void*) Cyc_Absyn_Plus){ goto
_LL1205;} else{ goto _LL1206;} _LL1206: if( _temp1192 ==( void*) Cyc_Absyn_Minus){
goto _LL1207;} else{ goto _LL1208;} _LL1208: if( _temp1192 ==( void*) Cyc_Absyn_Eq){
goto _LL1209;} else{ goto _LL1210;} _LL1210: if( _temp1192 ==( void*) Cyc_Absyn_Neq){
goto _LL1211;} else{ goto _LL1212;} _LL1212: if( _temp1192 ==( void*) Cyc_Absyn_Gt){
goto _LL1213;} else{ goto _LL1214;} _LL1214: if( _temp1192 ==( void*) Cyc_Absyn_Gte){
goto _LL1215;} else{ goto _LL1216;} _LL1216: if( _temp1192 ==( void*) Cyc_Absyn_Lt){
goto _LL1217;} else{ goto _LL1218;} _LL1218: if( _temp1192 ==( void*) Cyc_Absyn_Lte){
goto _LL1219;} else{ goto _LL1220;} _LL1220: goto _LL1221; _LL1195: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1037))->hd;{
void* _temp1222=( void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v;
struct Cyc_Absyn_Exp* _temp1230; struct Cyc_Absyn_PtrInfo _temp1232; struct Cyc_Absyn_Conref*
_temp1234; void* _temp1236; _LL1224: if(( unsigned int) _temp1222 > 4u?*(( int*)
_temp1222) == Cyc_Absyn_ArrayType: 0){ _LL1231: _temp1230=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1222)->f3; goto _LL1225;} else{ goto _LL1226;} _LL1226: if(( unsigned int)
_temp1222 > 4u?*(( int*) _temp1222) == Cyc_Absyn_PointerType: 0){ _LL1233:
_temp1232=(( struct Cyc_Absyn_PointerType_struct*) _temp1222)->f1; _LL1237:
_temp1236=( void*) _temp1232.elt_typ; goto _LL1235; _LL1235: _temp1234=
_temp1232.bounds; goto _LL1227;} else{ goto _LL1228;} _LL1228: goto _LL1229;
_LL1225:( void*)( e->r=( void*)(( void*)(( struct Cyc_Absyn_Exp*) _check_null(
_temp1230))->r)); goto _LL1223; _LL1227:{ void* _temp1238= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1234); struct Cyc_Absyn_Exp* _temp1244; _LL1240: if(
_temp1238 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1241;} else{ goto _LL1242;}
_LL1242: if(( unsigned int) _temp1238 > 1u?*(( int*) _temp1238) == Cyc_Absyn_Upper_b:
0){ _LL1245: _temp1244=(( struct Cyc_Absyn_Upper_b_struct*) _temp1238)->f1; goto
_LL1243;} else{ goto _LL1239;} _LL1241:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__get_arr_size_e,({ struct Cyc_List_List*(* _temp1246)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1249=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1037))->hd; struct Cyc_Absyn_Exp* _temp1250= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( _temp1236), 0); struct Cyc_Absyn_Exp* _temp1247[ 2u]={
_temp1249, _temp1250}; struct _tagged_arr _temp1248={( void*) _temp1247,( void*)
_temp1247,( void*)( _temp1247 + 2u)}; _temp1246( _temp1248);}), 0))->r)); goto
_LL1239; _LL1243:( void*)( e->r=( void*)(( void*) _temp1244->r)); goto _LL1239;
_LL1239:;} goto _LL1223; _LL1229:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)((
struct _tagged_arr)({ struct _tagged_arr(* _temp1251)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp1256= _tag_arr("size primop applied to non-array %s (%s)",
sizeof( unsigned char), 41u); struct Cyc_Stdio_String_pa_struct _temp1258;
_temp1258.tag= Cyc_Stdio_String_pa; _temp1258.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_List_List*) _check_null( _temp1191))->hd);{ void* _temp1254=(
void*)& _temp1258; struct Cyc_Stdio_String_pa_struct _temp1257; _temp1257.tag=
Cyc_Stdio_String_pa; _temp1257.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v);{ void* _temp1255=(
void*)& _temp1257; void* _temp1252[ 2u]={ _temp1254, _temp1255}; struct
_tagged_arr _temp1253={( void*) _temp1252,( void*) _temp1252,( void*)( _temp1252
+ 2u)}; _temp1251( _temp1256, _temp1253);}}})); return; _LL1223:;} goto _LL1193;}
_LL1197:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c( _temp1039, 0,
_temp1037, _temp1191))->r)); goto _LL1193; _LL1199:( void*)( e->r=( void*)((
void*)( Cyc_Toc_printf_to_c( _temp1039, 0, _temp1037, _temp1191))->r)); goto
_LL1193; _LL1201: if( _temp1037 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("fprintf without arguments", sizeof( unsigned char), 26u));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_printf_to_c( _temp1039,({ struct Cyc_Core_Opt*
_temp1259=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1259->v=( void*) Cyc_Toc_fdconv(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1037))->hd); _temp1259;}),(( struct Cyc_List_List*)
_check_null( _temp1037))->tl,(( struct Cyc_List_List*) _check_null( _temp1191))->tl))->r));
goto _LL1193; _LL1203: if( _temp1037 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("sscanf without arguments", sizeof( unsigned char), 25u));}( void*)(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1037))->hd)->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*)(( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1037))->hd)->r,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1037))->hd)->loc),
Cyc_Toc_curr_sp, 0))->r)); goto _LL1193; _LL1205: { void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt(( void*)(( struct Cyc_List_List*)
_check_null( _temp1190))->hd,& elt_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1037))->hd; struct Cyc_Absyn_Exp* e2=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1037))->tl))->hd;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_plus_e,({ struct Cyc_List_List*(* _temp1260)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1263= e1; struct Cyc_Absyn_Exp* _temp1264= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( elt_typ), 0); struct Cyc_Absyn_Exp* _temp1265= e2; struct Cyc_Absyn_Exp*
_temp1261[ 3u]={ _temp1263, _temp1264, _temp1265}; struct _tagged_arr _temp1262={(
void*) _temp1261,( void*) _temp1261,( void*)( _temp1261 + 3u)}; _temp1260(
_temp1262);}), 0))->r));} goto _LL1193;} _LL1207: { void* elt_typ=( void*) Cyc_Absyn_VoidType;
if( Cyc_Tcutil_is_tagged_pointer_typ_elt(( void*)(( struct Cyc_List_List*)
_check_null( _temp1190))->hd,& elt_typ)){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1037))->hd; struct Cyc_Absyn_Exp* e2=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1037))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ(( void*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1190))->tl))->hd)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0),
Cyc_Toc_curr_sp, 0))->r));( void*)( e->r=( void*)(( void*)( Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Div, Cyc_Absyn_copy_exp( e), Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(
elt_typ), 0), 0))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tagged_arr_plus_e,({ struct Cyc_List_List*(* _temp1266)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1269= e1; struct Cyc_Absyn_Exp* _temp1270= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c( elt_typ), 0); struct Cyc_Absyn_Exp* _temp1271= Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Minus, e2, 0); struct Cyc_Absyn_Exp* _temp1267[ 3u]={ _temp1269,
_temp1270, _temp1271}; struct _tagged_arr _temp1268={( void*) _temp1267,( void*)
_temp1267,( void*)( _temp1267 + 3u)}; _temp1266( _temp1268);}), 0))->r));}} goto
_LL1193;} _LL1209: goto _LL1211; _LL1211: goto _LL1213; _LL1213: goto _LL1215;
_LL1215: goto _LL1217; _LL1217: goto _LL1219; _LL1219: { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1037))->hd;
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1037))->tl))->hd; void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp1190))->hd; void* t2=(
void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1190))->tl))->hd; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){(
void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1193;} _LL1221: goto
_LL1193; _LL1193:;} goto _LL950;} _LL962: { void* e2_cyc_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1043->topt))->v; Cyc_Toc_exp_to_c( nv,
_temp1043);{ void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){ struct Cyc_Absyn_Exp* _temp1272=( _temp1041 ==( void*) Cyc_Absyn_PostInc?
1: _temp1041 ==( void*) Cyc_Absyn_PostDec)? Cyc_Toc__tagged_arr_inplace_plus_post_e:
Cyc_Toc__tagged_arr_inplace_plus_e; int _temp1273=( _temp1041 ==( void*) Cyc_Absyn_PreInc?
1: _temp1041 ==( void*) Cyc_Absyn_PostInc)? 1: - 1;( void*)( e->r=( void*)((
void*)( Cyc_Absyn_fncall_exp( _temp1272,({ struct Cyc_List_List*(* _temp1274)(
struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1277= Cyc_Absyn_address_exp( _temp1043, 0); struct
Cyc_Absyn_Exp* _temp1278= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c( elt_typ), 0);
struct Cyc_Absyn_Exp* _temp1279= Cyc_Absyn_signed_int_exp( _temp1273, 0); struct
Cyc_Absyn_Exp* _temp1275[ 3u]={ _temp1277, _temp1278, _temp1279}; struct
_tagged_arr _temp1276={( void*) _temp1275,( void*) _temp1275,( void*)( _temp1275
+ 3u)}; _temp1274( _temp1276);}), 0))->r));} if( elt_typ ==( void*) Cyc_Absyn_VoidType?
! Cyc_Absyn_is_lvalue( _temp1043): 0){ Cyc_Toc_lvalue_assign( _temp1043, 0, Cyc_Toc_incr_lvalue,
_temp1041);( void*)( e->r=( void*)(( void*) _temp1043->r));} goto _LL950;}}
_LL964: { int e1_poly= Cyc_Toc_is_poly_project( _temp1049); void* e1_old_typ=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1049->topt))->v; void*
e2_old_typ=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1045->topt))->v;
Cyc_Toc_exp_to_c( nv, _temp1049); Cyc_Toc_exp_to_c( nv, _temp1045);{ int done= 0;
if( _temp1047 != 0){ void* elt_typ=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){ struct Cyc_Absyn_Exp* change;{ void* _temp1280=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1047))->v; _LL1282: if( _temp1280 ==(
void*) Cyc_Absyn_Plus){ goto _LL1283;} else{ goto _LL1284;} _LL1284: if(
_temp1280 ==( void*) Cyc_Absyn_Minus){ goto _LL1285;} else{ goto _LL1286;}
_LL1286: goto _LL1287; _LL1283: change= _temp1045; goto _LL1281; _LL1285: change=
Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Minus, _temp1045, 0); goto _LL1281;
_LL1287:( int) _throw( Cyc_Tcutil_impos( _tag_arr("bad t ? pointer arithmetic",
sizeof( unsigned char), 27u))); goto _LL1281; _LL1281:;} done= 1;( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__tagged_arr_inplace_plus_e,({
struct Cyc_List_List*(* _temp1288)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1291= Cyc_Absyn_address_exp(
_temp1049, 0); struct Cyc_Absyn_Exp* _temp1292= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c(
elt_typ), 0); struct Cyc_Absyn_Exp* _temp1293= change; struct Cyc_Absyn_Exp*
_temp1289[ 3u]={ _temp1291, _temp1292, _temp1293}; struct _tagged_arr _temp1290={(
void*) _temp1289,( void*) _temp1289,( void*)( _temp1289 + 3u)}; _temp1288(
_temp1290);}), 0))->r));}} if( ! done){ if( e1_poly){( void*)( _temp1045->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp((
void*) _temp1045->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue( _temp1049)){((
void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Absyn_Exp*, struct _tuple9*), struct _tuple9* f_env)) Cyc_Toc_lvalue_assign)(
_temp1049, 0, Cyc_Toc_assignop_lvalue,({ struct _tuple9* _temp1294=( struct
_tuple9*) GC_malloc( sizeof( struct _tuple9) * 1); _temp1294[ 0]=({ struct
_tuple9 _temp1295; _temp1295.f1= _temp1047; _temp1295.f2= _temp1045; _temp1295;});
_temp1294;}));( void*)( e->r=( void*)(( void*) _temp1049->r));}} goto _LL950;}}
_LL966: Cyc_Toc_exp_to_c( nv, _temp1055); Cyc_Toc_exp_to_c( nv, _temp1053); Cyc_Toc_exp_to_c(
nv, _temp1051); goto _LL950; _LL968: Cyc_Toc_exp_to_c( nv, _temp1059); Cyc_Toc_exp_to_c(
nv, _temp1057); goto _LL950; _LL970: _temp1069= _temp1063; _temp1067= _temp1061;
goto _LL972; _LL972: Cyc_Toc_exp_to_c( nv, _temp1069);(( void(*)( void(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1067); goto _LL950; _LL974:{
struct _RegionHandle _temp1296= _new_region(); struct _RegionHandle* r=&
_temp1296; _push_region( r);{ void* _temp1297=( void*) _temp1074->type; void*
_temp1298= Cyc_Toc_typ_to_c( _temp1297); void* _temp1299= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1082->topt))->v); struct Cyc_Absyn_Exp*
_temp1300= Cyc_Absyn_uint_exp(( unsigned int) _temp1078, 0); void* arr_type= Cyc_Toc_typ_to_c((
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp1349=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1349[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1350; _temp1350.tag= Cyc_Absyn_ArrayType;
_temp1350.f1=( void*) _temp1297; _temp1350.f2= Cyc_Toc_mt_tq; _temp1350.f3=(
struct Cyc_Absyn_Exp*) _temp1300; _temp1350;}); _temp1349;})); struct _tuple0*
_temp1301= Cyc_Toc_temp_var(); struct _tuple0* _temp1302= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* _temp1303= Cyc_Absyn_var_exp( _temp1302, 0); struct
_tuple0* _temp1304= Cyc_Toc_temp_var(); int _temp1305=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp1080); int _temp1306= _temp1305 - _temp1078; struct
_tagged_arr vs=({ unsigned int _temp1345=( unsigned int) _temp1078; struct
_tuple0** _temp1346=( struct _tuple0**) _region_malloc( r, sizeof( struct
_tuple0*) * _temp1345); struct _tagged_arr _temp1348= _tag_arr( _temp1346,
sizeof( struct _tuple0*),( unsigned int) _temp1078);{ unsigned int _temp1347=
_temp1345; unsigned int i; for( i= 0; i < _temp1347; i ++){ _temp1346[ i]= Cyc_Toc_temp_var();}};
_temp1348;}); struct _tagged_arr xs=({ unsigned int _temp1341=( unsigned int)
_temp1306; struct _tuple0** _temp1342=( struct _tuple0**) _region_malloc( r,
sizeof( struct _tuple0*) * _temp1341); struct _tagged_arr _temp1344= _tag_arr(
_temp1342, sizeof( struct _tuple0*),( unsigned int) _temp1306);{ unsigned int
_temp1343= _temp1341; unsigned int i; for( i= 0; i < _temp1343; i ++){ _temp1342[
i]= Cyc_Toc_temp_var();}}; _temp1344;}); struct Cyc_List_List* xexps=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple0*, struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_Toc_map_2c)( Cyc_Absyn_var_exp,(
struct Cyc_Position_Segment*) 0,(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, struct _tagged_arr arr)) Cyc_List_rfrom_array)( r, xs));
xexps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( xexps,({ struct Cyc_List_List* _temp1307=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1307->hd=( void*) Cyc_Absyn_var_exp(
_temp1304, 0); _temp1307->tl= 0; _temp1307;}));{ struct Cyc_Absyn_Stmt*
_temp1308= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(
_temp1301, 0), xexps, 0), 0); struct Cyc_Absyn_Exp* _temp1309= Cyc_Absyn_add_exp(
_temp1303, _temp1300, 0); struct Cyc_List_List* _temp1310=({ struct Cyc_List_List*(*
_temp1332)( struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list; struct _tuple4* _temp1335=({ struct _tuple4* _temp1340=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1340->f1= 0; _temp1340->f2=
Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), _temp1303, 0); _temp1340;});
struct _tuple4* _temp1336=({ struct _tuple4* _temp1339=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp1339->f1= 0; _temp1339->f2= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp1303, 0); _temp1339;}); struct _tuple4*
_temp1337=({ struct _tuple4* _temp1338=( struct _tuple4*) GC_malloc( sizeof(
struct _tuple4)); _temp1338->f1= 0; _temp1338->f2= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
_temp1309, 0); _temp1338;}); struct _tuple4* _temp1333[ 3u]={ _temp1335,
_temp1336, _temp1337}; struct _tagged_arr _temp1334={( void*) _temp1333,( void*)
_temp1333,( void*)( _temp1333 + 3u)}; _temp1332( _temp1334);}); _temp1308= Cyc_Absyn_declare_stmt(
_temp1304, Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ( _temp1297,( void*) _temp1074->rgn,
Cyc_Toc_mt_tq)),( struct Cyc_Absyn_Exp*) Cyc_Absyn_unresolvedmem_exp( 0,
_temp1310, 0), _temp1308, 0);{ struct Cyc_List_List* _temp1311=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct Cyc_Absyn_Exp*(* f)( struct _tuple0*, struct
Cyc_Position_Segment*), struct Cyc_Position_Segment* env, struct Cyc_List_List*
x)) Cyc_Toc_rmap_2c)( r, Cyc_Absyn_var_exp, 0,(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct _tagged_arr arr)) Cyc_List_rfrom_array)( r, vs));
struct Cyc_List_List* _temp1312=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_make_dle,
_temp1311); _temp1308= Cyc_Absyn_declare_stmt( _temp1302, arr_type,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_unresolvedmem_exp( 0, _temp1312, 0), _temp1308, 0);{ struct Cyc_List_List*
_temp1313=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( r, _temp1080); struct Cyc_List_List* _temp1314=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( r, _temp1076);{
int i= _temp1078 - 1; for( 0; i >= 0;( i --, _temp1313=(( struct Cyc_List_List*)
_check_null( _temp1313))->tl)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1313))->hd; void* arg_type=( void*)((
struct Cyc_Core_Opt*) _check_null( arg->topt))->v; Cyc_Toc_exp_to_c( nv, arg);
if( _temp1074->inject){ void* _temp1317; struct Cyc_Position_Segment* _temp1319;
struct Cyc_List_List* _temp1321; struct Cyc_List_List* _temp1323; struct _tuple0*
_temp1325; struct Cyc_Absyn_Tunionfield _temp1315=*(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1314))->hd); _LL1326: _temp1325=
_temp1315.name; goto _LL1324; _LL1324: _temp1323= _temp1315.tvs; goto _LL1322;
_LL1322: _temp1321= _temp1315.typs; goto _LL1320; _LL1320: _temp1319= _temp1315.loc;
goto _LL1318; _LL1318: _temp1317=( void*) _temp1315.sc; goto _LL1316; _LL1316:
_temp1314=(( struct Cyc_List_List*) _check_null( _temp1314))->tl;{ struct
_tuple0* _temp1327= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp1328= Cyc_Absyn_var_exp(
_temp1327, 0); void* _temp1329= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp1325, _tag_arr("_struct", sizeof( unsigned char), 8u))); _temp1308= Cyc_Absyn_declare_stmt(*((
struct _tuple0**) _check_unknown_subscript( vs, sizeof( struct _tuple0*), i)),
_temp1298,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( _temp1298, Cyc_Absyn_address_exp(
_temp1328, 0), 0), _temp1308, 0);{ void* field_typ= Cyc_Toc_typ_to_c((*(( struct
_tuple3*)(( struct Cyc_List_List*) _check_null( _temp1321))->hd)).f2); if( Cyc_Toc_is_void_star(
field_typ)){ arg= Cyc_Absyn_cast_exp( field_typ, arg, 0);}{ struct Cyc_Absyn_Stmt*
_temp1330= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1328, Cyc_Toc_fieldname(
1), 0), arg, 0); _temp1308= Cyc_Absyn_seq_stmt( _temp1330, _temp1308, 0);{
struct Cyc_Absyn_Stmt* _temp1331= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
_temp1328, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( _temp1325, 0), 0); _temp1308=
Cyc_Absyn_seq_stmt( _temp1331, _temp1308, 0); _temp1308= Cyc_Absyn_declare_stmt(
_temp1327, _temp1329, 0, _temp1308, 0);}}}}} else{ _temp1308= Cyc_Absyn_declare_stmt(*((
struct _tuple0**) _check_unknown_subscript( vs, sizeof( struct _tuple0*), i)),
_temp1298,( struct Cyc_Absyn_Exp*) arg, _temp1308, 0);}}}{ int i= _temp1306 - 1;
for( 0; i >= 0;( i --, _temp1313=(( struct Cyc_List_List*) _check_null(
_temp1313))->tl)){ struct Cyc_Absyn_Exp* arg=( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp1313))->hd; void* argtype= Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( arg->topt))->v); Cyc_Toc_exp_to_c(
nv, arg); _temp1308= Cyc_Absyn_declare_stmt(*(( struct _tuple0**)
_check_unknown_subscript( xs, sizeof( struct _tuple0*), i)), argtype,( struct
Cyc_Absyn_Exp*) arg, _temp1308, 0);}} Cyc_Toc_exp_to_c( nv, _temp1082);
_temp1308= Cyc_Absyn_declare_stmt( _temp1301, _temp1299,( struct Cyc_Absyn_Exp*)
_temp1082, _temp1308, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
_temp1308, 0))->r));}}}}; _pop_region( r);} goto _LL950; _LL976: Cyc_Toc_exp_to_c(
nv, _temp1084);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(
old_typ), Cyc_Toc_newthrow_exp( _temp1084), 0))->r)); goto _LL950; _LL978: Cyc_Toc_exp_to_c(
nv, _temp1086); goto _LL950; _LL980: Cyc_Toc_exp_to_c( nv, _temp1090); for( 0;
_temp1088 != 0; _temp1088=(( struct Cyc_List_List*) _check_null( _temp1088))->tl){
void* k= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*) _check_null(
_temp1088))->hd); if( k !=( void*) Cyc_Absyn_EffKind? k !=( void*) Cyc_Absyn_RgnKind:
0){{ void* _temp1351= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*)
_check_null( _temp1088))->hd); _LL1353: if(( unsigned int) _temp1351 > 4u?*((
int*) _temp1351) == Cyc_Absyn_VarType: 0){ goto _LL1354;} else{ goto _LL1355;}
_LL1355: if(( unsigned int) _temp1351 > 4u?*(( int*) _temp1351) == Cyc_Absyn_TunionType:
0){ goto _LL1356;} else{ goto _LL1357;} _LL1357: goto _LL1358; _LL1354:
continue; _LL1356: continue; _LL1358:( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp1090, 0))->r)); goto _LL1352; _LL1352:;} break;}} goto _LL950; _LL982: {
void* old_t2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1092->topt))->v;
void* new_typ=* _temp1096;* _temp1096= Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c(
nv, _temp1092);{ struct _tuple10 _temp1360=({ struct _tuple10 _temp1359;
_temp1359.f1= Cyc_Tcutil_compress( old_t2); _temp1359.f2= Cyc_Tcutil_compress(
new_typ); _temp1359;}); void* _temp1368; struct Cyc_Absyn_PtrInfo _temp1370;
void* _temp1372; struct Cyc_Absyn_PtrInfo _temp1374; void* _temp1376; void*
_temp1378; struct Cyc_Absyn_PtrInfo _temp1380; _LL1362: _LL1373: _temp1372=
_temp1360.f1; if(( unsigned int) _temp1372 > 4u?*(( int*) _temp1372) == Cyc_Absyn_PointerType:
0){ _LL1375: _temp1374=(( struct Cyc_Absyn_PointerType_struct*) _temp1372)->f1;
goto _LL1369;} else{ goto _LL1364;} _LL1369: _temp1368= _temp1360.f2; if((
unsigned int) _temp1368 > 4u?*(( int*) _temp1368) == Cyc_Absyn_PointerType: 0){
_LL1371: _temp1370=(( struct Cyc_Absyn_PointerType_struct*) _temp1368)->f1; goto
_LL1363;} else{ goto _LL1364;} _LL1364: _LL1379: _temp1378= _temp1360.f1; if((
unsigned int) _temp1378 > 4u?*(( int*) _temp1378) == Cyc_Absyn_PointerType: 0){
_LL1381: _temp1380=(( struct Cyc_Absyn_PointerType_struct*) _temp1378)->f1; goto
_LL1377;} else{ goto _LL1366;} _LL1377: _temp1376= _temp1360.f2; if((
unsigned int) _temp1376 > 4u?*(( int*) _temp1376) == Cyc_Absyn_IntType: 0){ goto
_LL1365;} else{ goto _LL1366;} _LL1366: goto _LL1367; _LL1363: { int _temp1382=((
int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)( 0, _temp1374.nullable);
int _temp1383=(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
0, _temp1370.nullable); void* _temp1384= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1374.bounds); void* _temp1385= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1370.bounds);{ struct _tuple10 _temp1387=({ struct _tuple10 _temp1386;
_temp1386.f1= _temp1384; _temp1386.f2= _temp1385; _temp1386;}); void* _temp1397;
void* _temp1399; void* _temp1401; void* _temp1403; struct Cyc_Absyn_Exp*
_temp1405; void* _temp1407; struct Cyc_Absyn_Exp* _temp1409; void* _temp1411;
_LL1389: _LL1400: _temp1399= _temp1387.f1; if(( unsigned int) _temp1399 > 1u?*((
int*) _temp1399) == Cyc_Absyn_Upper_b: 0){ goto _LL1398;} else{ goto _LL1391;}
_LL1398: _temp1397= _temp1387.f2; if(( unsigned int) _temp1397 > 1u?*(( int*)
_temp1397) == Cyc_Absyn_Upper_b: 0){ goto _LL1390;} else{ goto _LL1391;} _LL1391:
_LL1404: _temp1403= _temp1387.f1; if(( unsigned int) _temp1403 > 1u?*(( int*)
_temp1403) == Cyc_Absyn_Upper_b: 0){ _LL1406: _temp1405=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1403)->f1; goto _LL1402;} else{ goto _LL1393;} _LL1402: _temp1401=
_temp1387.f2; if( _temp1401 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1392;}
else{ goto _LL1393;} _LL1393: _LL1412: _temp1411= _temp1387.f1; if( _temp1411 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL1408;} else{ goto _LL1395;} _LL1408:
_temp1407= _temp1387.f2; if(( unsigned int) _temp1407 > 1u?*(( int*) _temp1407)
== Cyc_Absyn_Upper_b: 0){ _LL1410: _temp1409=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1407)->f1; goto _LL1394;} else{ goto _LL1395;} _LL1395: goto _LL1396;
_LL1390: if( _temp1382? ! _temp1383: 0){ if( nv->toplevel){(( int(*)( struct
_tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("can't do null-check conversion at top-level",
sizeof( unsigned char), 44u));}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(*
_temp1096, Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({ struct Cyc_List_List*
_temp1413=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1413->hd=( void*) _temp1092; _temp1413->tl= 0; _temp1413;}), 0), 0))->r));}
goto _LL1388; _LL1392: if( nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Toc_make_toplevel_tagged_arr(
old_t2, _temp1405, _temp1092))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({ struct Cyc_List_List*(* _temp1414)( struct _tagged_arr)=(
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp*
_temp1417= _temp1092; struct Cyc_Absyn_Exp* _temp1418= Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(( void*) _temp1370.elt_typ), 0); struct Cyc_Absyn_Exp*
_temp1419= _temp1405; struct Cyc_Absyn_Exp* _temp1415[ 3u]={ _temp1417,
_temp1418, _temp1419}; struct _tagged_arr _temp1416={( void*) _temp1415,( void*)
_temp1415,( void*)( _temp1415 + 3u)}; _temp1414( _temp1416);}), 0))->r));} goto
_LL1388; _LL1394: if( nv->toplevel){(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(
_tag_arr("can't coerce t? to t* or t@ at the top-level", sizeof( unsigned char),
45u));}{ struct Cyc_Absyn_Exp* _temp1420= Cyc_Absyn_fncall_exp( Cyc_Toc__untag_arr_e,({
struct Cyc_List_List*(* _temp1422)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1425= _temp1092;
struct Cyc_Absyn_Exp* _temp1426= Cyc_Absyn_sizeoftyp_exp( Cyc_Toc_typ_to_c((
void*) _temp1374.elt_typ), 0); struct Cyc_Absyn_Exp* _temp1427= _temp1409;
struct Cyc_Absyn_Exp* _temp1423[ 3u]={ _temp1425, _temp1426, _temp1427}; struct
_tagged_arr _temp1424={( void*) _temp1423,( void*) _temp1423,( void*)( _temp1423
+ 3u)}; _temp1422( _temp1424);}), 0); if( _temp1383){( void*)( _temp1420->r=(
void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__check_null_e,({ struct Cyc_List_List*
_temp1421=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1421->hd=( void*) Cyc_Absyn_copy_exp( _temp1420); _temp1421->tl= 0;
_temp1421;}), 0))->r));}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(*
_temp1096, _temp1420, 0))->r)); goto _LL1388;} _LL1396: goto _LL1388; _LL1388:;}
goto _LL1361;} _LL1365:{ void* _temp1428= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1380.bounds); _LL1430: if( _temp1428 ==( void*) Cyc_Absyn_Unknown_b){ goto
_LL1431;} else{ goto _LL1432;} _LL1432: goto _LL1433; _LL1431:( void*)(
_temp1092->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) _temp1092->r, _temp1092->loc), Cyc_Toc_curr_sp, 0))->r)); goto _LL1429;
_LL1433: goto _LL1429; _LL1429:;} goto _LL1361; _LL1367: goto _LL1361; _LL1361:;}
goto _LL950;} _LL984:{ void* _temp1434=( void*) _temp1097->r; struct Cyc_List_List*
_temp1442; struct _tuple0* _temp1444; struct Cyc_List_List* _temp1446; _LL1436:
if(*(( int*) _temp1434) == Cyc_Absyn_Struct_e){ _LL1445: _temp1444=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1434)->f1; goto _LL1443; _LL1443: _temp1442=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1434)->f3; goto _LL1437;} else{ goto _LL1438;} _LL1438: if(*(( int*)
_temp1434) == Cyc_Absyn_Tuple_e){ _LL1447: _temp1446=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1434)->f1; goto _LL1439;} else{ goto _LL1440;} _LL1440: goto _LL1441;
_LL1437: if( nv->toplevel){(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)((
struct _tagged_arr)({ struct _tagged_arr(* _temp1448)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp1452= _tag_arr("%s: & on non-identifiers at the top-level",
sizeof( unsigned char), 42u); struct Cyc_Stdio_String_pa_struct _temp1453;
_temp1453.tag= Cyc_Stdio_String_pa; _temp1453.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp1097->loc);{ void* _temp1451=( void*)& _temp1453; void* _temp1449[ 1u]={
_temp1451}; struct _tagged_arr _temp1450={( void*) _temp1449,( void*) _temp1449,(
void*)( _temp1449 + 1u)}; _temp1448( _temp1452, _temp1450);}}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1097->topt))->v, 1, 0, _temp1442, _temp1444))->r)); goto
_LL1435; _LL1439: if( nv->toplevel){(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)((
struct _tagged_arr)({ struct _tagged_arr(* _temp1454)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp1458= _tag_arr("%s: & on non-identifiers at the top-level",
sizeof( unsigned char), 42u); struct Cyc_Stdio_String_pa_struct _temp1459;
_temp1459.tag= Cyc_Stdio_String_pa; _temp1459.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp1097->loc);{ void* _temp1457=( void*)& _temp1459; void* _temp1455[ 1u]={
_temp1457}; struct _tagged_arr _temp1456={( void*) _temp1455,( void*) _temp1455,(
void*)( _temp1455 + 1u)}; _temp1454( _temp1458, _temp1456);}}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1446))->r)); goto _LL1435;
_LL1441: Cyc_Toc_exp_to_c( nv, _temp1097); if( ! Cyc_Absyn_is_lvalue( _temp1097)){((
void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1097,
0, Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
_temp1097, 0))->r));} goto _LL1435; _LL1435:;} goto _LL950; _LL986: if( nv->toplevel){((
int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)(( struct _tagged_arr)({ struct
_tagged_arr(* _temp1460)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1464= _tag_arr("%s: new at top-level", sizeof(
unsigned char), 21u); struct Cyc_Stdio_String_pa_struct _temp1465; _temp1465.tag=
Cyc_Stdio_String_pa; _temp1465.f1=( struct _tagged_arr) Cyc_Position_string_of_segment(
_temp1099->loc);{ void* _temp1463=( void*)& _temp1465; void* _temp1461[ 1u]={
_temp1463}; struct _tagged_arr _temp1462={( void*) _temp1461,( void*) _temp1461,(
void*)( _temp1461 + 1u)}; _temp1460( _temp1464, _temp1462);}}));}{ void*
_temp1466=( void*) _temp1099->r; struct Cyc_List_List* _temp1478; struct Cyc_Absyn_Exp*
_temp1480; struct Cyc_Absyn_Exp* _temp1482; struct Cyc_Absyn_Vardecl* _temp1484;
struct Cyc_List_List* _temp1486; struct _tuple0* _temp1488; struct Cyc_List_List*
_temp1490; _LL1468: if(*(( int*) _temp1466) == Cyc_Absyn_Array_e){ _LL1479:
_temp1478=(( struct Cyc_Absyn_Array_e_struct*) _temp1466)->f1; goto _LL1469;}
else{ goto _LL1470;} _LL1470: if(*(( int*) _temp1466) == Cyc_Absyn_Comprehension_e){
_LL1485: _temp1484=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1466)->f1;
goto _LL1483; _LL1483: _temp1482=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1466)->f2; goto _LL1481; _LL1481: _temp1480=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1466)->f3; goto _LL1471;} else{ goto _LL1472;} _LL1472: if(*(( int*)
_temp1466) == Cyc_Absyn_Struct_e){ _LL1489: _temp1488=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1466)->f1; goto _LL1487; _LL1487: _temp1486=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1466)->f3; goto _LL1473;} else{ goto _LL1474;} _LL1474: if(*(( int*)
_temp1466) == Cyc_Absyn_Tuple_e){ _LL1491: _temp1490=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1466)->f1; goto _LL1475;} else{ goto _LL1476;} _LL1476: goto _LL1477;
_LL1469: { struct _tuple0* _temp1492= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
_temp1493= Cyc_Absyn_var_exp( _temp1492, 0); struct Cyc_Absyn_Stmt* _temp1494=
Cyc_Toc_init_array( nv, _temp1493, _temp1478, Cyc_Absyn_exp_stmt( _temp1493, 0));
void* old_elt_typ;{ void* _temp1495= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1501; struct Cyc_Absyn_Tqual _temp1503; void* _temp1505; _LL1497: if((
unsigned int) _temp1495 > 4u?*(( int*) _temp1495) == Cyc_Absyn_PointerType: 0){
_LL1502: _temp1501=(( struct Cyc_Absyn_PointerType_struct*) _temp1495)->f1;
_LL1506: _temp1505=( void*) _temp1501.elt_typ; goto _LL1504; _LL1504: _temp1503=
_temp1501.tq; goto _LL1498;} else{ goto _LL1499;} _LL1499: goto _LL1500; _LL1498:
old_elt_typ= _temp1505; goto _LL1496; _LL1500: old_elt_typ= Cyc_Toc_toc_impos(
_tag_arr("exp_to_c:new array expression doesn't have ptr type", sizeof(
unsigned char), 52u)); goto _LL1496; _LL1496:;}{ void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); void* _temp1507= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp* _temp1508= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_signed_int_exp((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1478), 0), 0); struct Cyc_Absyn_Exp* e1; if( _temp1101 == 0){ e1= Cyc_Toc_malloc_exp(
old_elt_typ, _temp1508);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)
_check_null( _temp1101); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp( r,
_temp1508);}{ struct Cyc_Absyn_Exp* _temp1509= Cyc_Absyn_cast_exp( _temp1507, e1,
0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1492, _temp1507,( struct Cyc_Absyn_Exp*) _temp1509, _temp1494, 0), 0))->r));
goto _LL1467;}}} _LL1471: { int is_tagged_ptr= 0;{ void* _temp1510= Cyc_Tcutil_compress(
old_typ); struct Cyc_Absyn_PtrInfo _temp1516; struct Cyc_Absyn_Conref* _temp1518;
struct Cyc_Absyn_Tqual _temp1520; void* _temp1522; _LL1512: if(( unsigned int)
_temp1510 > 4u?*(( int*) _temp1510) == Cyc_Absyn_PointerType: 0){ _LL1517:
_temp1516=(( struct Cyc_Absyn_PointerType_struct*) _temp1510)->f1; _LL1523:
_temp1522=( void*) _temp1516.elt_typ; goto _LL1521; _LL1521: _temp1520=
_temp1516.tq; goto _LL1519; _LL1519: _temp1518= _temp1516.bounds; goto _LL1513;}
else{ goto _LL1514;} _LL1514: goto _LL1515; _LL1513: is_tagged_ptr= Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one, _temp1518) ==( void*) Cyc_Absyn_Unknown_b; goto _LL1511;
_LL1515:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: comprehension not an array type",
sizeof( unsigned char), 42u)); goto _LL1511; _LL1511:;}{ struct _tuple0* max=
Cyc_Toc_temp_var(); struct _tuple0* a= Cyc_Toc_temp_var(); void* old_elt_typ=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1480->topt))->v; void* elt_typ=
Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c( nv, _temp1482);{ struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( a, 0), _temp1484, Cyc_Absyn_var_exp( max, 0), _temp1480,
Cyc_Absyn_skip_stmt( 0), 1);{ struct _RegionHandle _temp1524= _new_region();
struct _RegionHandle* r=& _temp1524; _push_region( r);{ struct Cyc_List_List*
decls=({ struct Cyc_List_List* _temp1547=( struct Cyc_List_List*) _region_malloc(
r, sizeof( struct Cyc_List_List)); _temp1547->hd=( void*)({ struct _tuple8*
_temp1548=( struct _tuple8*) _region_malloc( r, sizeof( struct _tuple8));
_temp1548->f1= max; _temp1548->f2= Cyc_Absyn_uint_t; _temp1548->f3=( struct Cyc_Absyn_Exp*)
_temp1482; _temp1548;}); _temp1547->tl= 0; _temp1547;}); struct Cyc_Absyn_Exp*
ai; if( _temp1101 == 0){ ai= Cyc_Toc_malloc_exp( old_elt_typ, Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0));} else{
struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( _temp1101); Cyc_Toc_exp_to_c(
nv, r); ai= Cyc_Toc_rmalloc_exp( r, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0));}{ struct Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp(
ptr_typ, ai, 0); decls=({ struct Cyc_List_List* _temp1525=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1525->hd=( void*)({
struct _tuple8* _temp1526=( struct _tuple8*) _region_malloc( r, sizeof( struct
_tuple8)); _temp1526->f1= a; _temp1526->f2= ptr_typ; _temp1526->f3=( struct Cyc_Absyn_Exp*)
ainit; _temp1526;}); _temp1525->tl= decls; _temp1525;}); if( is_tagged_ptr){
struct _tuple0* _temp1527= Cyc_Toc_temp_var(); void* _temp1528= Cyc_Toc_typ_to_c(
old_typ); struct Cyc_Absyn_Exp* _temp1529= Cyc_Absyn_fncall_exp( Cyc_Toc__tag_arr_e,({
struct Cyc_List_List*(* _temp1532)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1535= Cyc_Absyn_var_exp(
a, 0); struct Cyc_Absyn_Exp* _temp1536= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);
struct Cyc_Absyn_Exp* _temp1537= _temp1482; struct Cyc_Absyn_Exp* _temp1533[ 3u]={
_temp1535, _temp1536, _temp1537}; struct _tagged_arr _temp1534={( void*)
_temp1533,( void*) _temp1533,( void*)( _temp1533 + 3u)}; _temp1532( _temp1534);}),
0); decls=({ struct Cyc_List_List* _temp1530=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp1530->hd=( void*)({
struct _tuple8* _temp1531=( struct _tuple8*) _region_malloc( r, sizeof( struct
_tuple8)); _temp1531->f1= _temp1527; _temp1531->f2= _temp1528; _temp1531->f3=(
struct Cyc_Absyn_Exp*) _temp1529; _temp1531;}); _temp1530->tl= decls; _temp1530;});
s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( _temp1527, 0),
0), 0);} else{ s= Cyc_Absyn_seq_stmt( s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
a, 0), 0), 0);}{ struct Cyc_List_List* _temp1538= decls; for( 0; _temp1538 != 0;
_temp1538=(( struct Cyc_List_List*) _check_null( _temp1538))->tl){ struct Cyc_Absyn_Exp*
_temp1541; void* _temp1543; struct _tuple0* _temp1545; struct _tuple8 _temp1539=*((
struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp1538))->hd); _LL1546:
_temp1545= _temp1539.f1; goto _LL1544; _LL1544: _temp1543= _temp1539.f2; goto
_LL1542; _LL1542: _temp1541= _temp1539.f3; goto _LL1540; _LL1540: s= Cyc_Absyn_declare_stmt(
_temp1545, _temp1543, _temp1541, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r));}}; _pop_region( r);} goto _LL1467;}}} _LL1473:( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_struct( nv,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1099->topt))->v, 1, _temp1101, _temp1486, _temp1488))->r)); goto _LL1467;
_LL1475:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, _temp1101,
_temp1490))->r)); goto _LL1467; _LL1477: { void* old_elt_typ=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1099->topt))->v; void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); struct Cyc_Absyn_Exp* mexp= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0);
if( _temp1101 == 0){ mexp= Cyc_Toc_malloc_exp( old_elt_typ, mexp);} else{ struct
Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*) _check_null( _temp1101); Cyc_Toc_exp_to_c(
nv, r); mexp= Cyc_Toc_rmalloc_exp( r, mexp);} mexp= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq), mexp, 0);{ int done= 0;{ void* _temp1549=( void*)
_temp1099->r; struct Cyc_Absyn_Exp* _temp1555; void* _temp1557; _LL1551: if(*((
int*) _temp1549) == Cyc_Absyn_Cast_e){ _LL1558: _temp1557=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1549)->f1; goto _LL1556; _LL1556: _temp1555=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1549)->f2; goto _LL1552;} else{ goto _LL1553;} _LL1553: goto _LL1554;
_LL1552:{ struct _tuple10 _temp1560=({ struct _tuple10 _temp1559; _temp1559.f1=
Cyc_Tcutil_compress( _temp1557); _temp1559.f2= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1555->topt))->v); _temp1559;}); void*
_temp1566; struct Cyc_Absyn_PtrInfo _temp1568; struct Cyc_Absyn_Conref*
_temp1570; void* _temp1572; struct Cyc_Absyn_PtrInfo _temp1574; struct Cyc_Absyn_Conref*
_temp1576; void* _temp1578; _LL1562: _LL1573: _temp1572= _temp1560.f1; if((
unsigned int) _temp1572 > 4u?*(( int*) _temp1572) == Cyc_Absyn_PointerType: 0){
_LL1575: _temp1574=(( struct Cyc_Absyn_PointerType_struct*) _temp1572)->f1;
_LL1579: _temp1578=( void*) _temp1574.elt_typ; goto _LL1577; _LL1577: _temp1576=
_temp1574.bounds; goto _LL1567;} else{ goto _LL1564;} _LL1567: _temp1566=
_temp1560.f2; if(( unsigned int) _temp1566 > 4u?*(( int*) _temp1566) == Cyc_Absyn_PointerType:
0){ _LL1569: _temp1568=(( struct Cyc_Absyn_PointerType_struct*) _temp1566)->f1;
_LL1571: _temp1570= _temp1568.bounds; goto _LL1563;} else{ goto _LL1564;}
_LL1564: goto _LL1565; _LL1563:{ struct _tuple10 _temp1581=({ struct _tuple10
_temp1580; _temp1580.f1= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1576);
_temp1580.f2= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1570); _temp1580;});
void* _temp1587; struct Cyc_Absyn_Exp* _temp1589; void* _temp1591; _LL1583:
_LL1592: _temp1591= _temp1581.f1; if( _temp1591 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1588;} else{ goto _LL1585;} _LL1588: _temp1587= _temp1581.f2; if((
unsigned int) _temp1587 > 1u?*(( int*) _temp1587) == Cyc_Absyn_Upper_b: 0){
_LL1590: _temp1589=(( struct Cyc_Absyn_Upper_b_struct*) _temp1587)->f1; goto
_LL1584;} else{ goto _LL1585;} _LL1585: goto _LL1586; _LL1584: done= 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__init_tag_arr_e,({ struct
Cyc_List_List*(* _temp1593)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1596= mexp;
struct Cyc_Absyn_Exp* _temp1597= _temp1555; struct Cyc_Absyn_Exp* _temp1598= Cyc_Absyn_sizeoftyp_exp(
_temp1578, 0); struct Cyc_Absyn_Exp* _temp1599= _temp1589; struct Cyc_Absyn_Exp*
_temp1594[ 4u]={ _temp1596, _temp1597, _temp1598, _temp1599}; struct _tagged_arr
_temp1595={( void*) _temp1594,( void*) _temp1594,( void*)( _temp1594 + 4u)};
_temp1593( _temp1595);}), 0))->r)); goto _LL1582; _LL1586: goto _LL1582; _LL1582:;}
goto _LL1561; _LL1565: goto _LL1561; _LL1561:;} goto _LL1550; _LL1554: goto
_LL1550; _LL1550:;} if( ! done){ struct _tuple0* _temp1600= Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt* _temp1601= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp(
_temp1600, 0), 0); struct Cyc_Absyn_Exp* _temp1602= Cyc_Absyn_signed_int_exp( 0,
0); Cyc_Toc_exp_to_c( nv, _temp1099); _temp1601= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( _temp1600, 0), _temp1602, 0),
_temp1099, 0), _temp1601, 0);{ void* _temp1603= Cyc_Absyn_cstar_typ( elt_typ,
Cyc_Toc_mt_tq);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1600, _temp1603,( struct Cyc_Absyn_Exp*) mexp, _temp1601, 0), 0))->r));}}
goto _LL1467;}} _LL1467:;} goto _LL950; _LL988: Cyc_Toc_exp_to_c( nv, _temp1103);
goto _LL950; _LL990:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1604=( struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1604[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1605; _temp1605.tag=
Cyc_Absyn_Sizeoftyp_e; _temp1605.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1105);
_temp1605;}); _temp1604;}))); goto _LL950; _LL992:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Offsetof_e_struct* _temp1606=( struct Cyc_Absyn_Offsetof_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp1606[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp1607; _temp1607.tag= Cyc_Absyn_Offsetof_e;
_temp1607.f1=( void*) Cyc_Toc_typ_to_c_array( _temp1109); _temp1607.f2=
_temp1107; _temp1607;}); _temp1606;}))); goto _LL950; _LL994: { void* _temp1608=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1111->topt))->v);{
void* _temp1609= _temp1608; struct Cyc_Absyn_PtrInfo _temp1615; struct Cyc_Absyn_Conref*
_temp1617; struct Cyc_Absyn_Tqual _temp1619; struct Cyc_Absyn_Conref* _temp1621;
void* _temp1623; void* _temp1625; _LL1611: if(( unsigned int) _temp1609 > 4u?*((
int*) _temp1609) == Cyc_Absyn_PointerType: 0){ _LL1616: _temp1615=(( struct Cyc_Absyn_PointerType_struct*)
_temp1609)->f1; _LL1626: _temp1625=( void*) _temp1615.elt_typ; goto _LL1624;
_LL1624: _temp1623=( void*) _temp1615.rgn_typ; goto _LL1622; _LL1622: _temp1621=
_temp1615.nullable; goto _LL1620; _LL1620: _temp1619= _temp1615.tq; goto _LL1618;
_LL1618: _temp1617= _temp1615.bounds; goto _LL1612;} else{ goto _LL1613;}
_LL1613: goto _LL1614; _LL1612:{ void* _temp1627= Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one,
_temp1617); _LL1629: if(( unsigned int) _temp1627 > 1u?*(( int*) _temp1627) ==
Cyc_Absyn_Upper_b: 0){ goto _LL1630;} else{ goto _LL1631;} _LL1631: if(
_temp1627 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1632;} else{ goto _LL1628;}
_LL1630: Cyc_Toc_exp_to_c( nv, _temp1111); if( Cyc_Toc_is_nullable( _temp1608)){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1111->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1633=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1633->hd=( void*) _temp1111;
_temp1633->tl= 0; _temp1633;}), 0), 0), 0))->r));} goto _LL1628; _LL1632: {
struct Cyc_Absyn_Exp* _temp1634= Cyc_Absyn_uint_exp( 0, 0); _temp1634->topt=({
struct Cyc_Core_Opt* _temp1635=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1635->v=( void*) Cyc_Absyn_uint_t; _temp1635;});( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_subscript_exp( _temp1111, _temp1634, 0))->r)); Cyc_Toc_exp_to_c(
nv, e); goto _LL1628;} _LL1628:;} goto _LL1610; _LL1614:(( int(*)( struct
_tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: Deref: non-pointer",
sizeof( unsigned char), 29u)); goto _LL1610; _LL1610:;} goto _LL950;} _LL996:
Cyc_Toc_exp_to_c( nv, _temp1115); if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v), Cyc_Absyn_new_exp(( void*) e->r, 0),
0))->r));} goto _LL950; _LL998: { void* e1typ= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1119->topt))->v); Cyc_Toc_exp_to_c( nv,
_temp1119);{ int is_poly= Cyc_Toc_is_poly_project( e); struct Cyc_Absyn_Conref*
_temp1638; struct Cyc_Absyn_Tqual _temp1640; struct Cyc_Absyn_Conref* _temp1642;
void* _temp1644; void* _temp1646; struct Cyc_Absyn_PtrInfo _temp1636= Cyc_Toc_get_ptr_type(
e1typ); _LL1647: _temp1646=( void*) _temp1636.elt_typ; goto _LL1645; _LL1645:
_temp1644=( void*) _temp1636.rgn_typ; goto _LL1643; _LL1643: _temp1642=
_temp1636.nullable; goto _LL1641; _LL1641: _temp1640= _temp1636.tq; goto _LL1639;
_LL1639: _temp1638= _temp1636.bounds; goto _LL1637; _LL1637:{ void* _temp1648=
Cyc_Absyn_conref_def( Cyc_Absyn_bounds_one, _temp1638); struct Cyc_Absyn_Exp*
_temp1654; _LL1650: if(( unsigned int) _temp1648 > 1u?*(( int*) _temp1648) ==
Cyc_Absyn_Upper_b: 0){ _LL1655: _temp1654=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1648)->f1; goto _LL1651;} else{ goto _LL1652;} _LL1652: if( _temp1648 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL1653;} else{ goto _LL1649;} _LL1651: if(
Cyc_Evexp_eval_const_uint_exp( _temp1654) < 1){(( int(*)( struct _tagged_arr))
Cyc_Tcutil_impos)( _tag_arr("exp_to_c:  StructArrow_e on pointer of size 0",
sizeof( unsigned char), 46u));}{ int possibly_null=(( int(*)( int, struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_def)( 0, _temp1642); if( Cyc_Toc_is_nullable(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1119->topt))->v)){( void*)( e->r=( void*)((
void*)( Cyc_Absyn_structarrow_exp( Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1119->topt))->v), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({ struct Cyc_List_List* _temp1656=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1656->hd=( void*) _temp1119;
_temp1656->tl= 0; _temp1656;}), 0), 0), _temp1117, 0))->r));} goto _LL1649;}
_LL1653: { void* ta1= Cyc_Toc_typ_to_c_array( _temp1646);( void*)( _temp1119->r=(
void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( ta1, _temp1640), Cyc_Absyn_fncall_exp(
Cyc_Toc__check_unknown_subscript_e,({ struct Cyc_List_List*(* _temp1657)( struct
_tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp1660= Cyc_Absyn_copy_exp( _temp1119); struct Cyc_Absyn_Exp*
_temp1661= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp* _temp1662=
Cyc_Absyn_uint_exp( 0, 0); struct Cyc_Absyn_Exp* _temp1658[ 3u]={ _temp1660,
_temp1661, _temp1662}; struct _tagged_arr _temp1659={( void*) _temp1658,( void*)
_temp1658,( void*)( _temp1658 + 3u)}; _temp1657( _temp1659);}), 0), 0))->r));
goto _LL1649;} _LL1649:;} if( is_poly){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),
Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));} goto _LL950;}} _LL1000: { void*
_temp1663= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1123->topt))->v); Cyc_Toc_exp_to_c( nv, _temp1123); Cyc_Toc_exp_to_c( nv,
_temp1121);{ void* _temp1664= _temp1663; struct Cyc_List_List* _temp1672; struct
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
_temp1121) + 1;( void*)( e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
_temp1123, Cyc_Toc_fieldname(( int) i), 0))->r)); goto _LL1665;} _LL1669:{ void*
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
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1697= _temp1123;
struct Cyc_Absyn_Exp* _temp1698= _temp1692; struct Cyc_Absyn_Exp* _temp1699= Cyc_Absyn_sizeoftyp_exp(
ta1, 0); struct Cyc_Absyn_Exp* _temp1700= _temp1121; struct Cyc_Absyn_Exp*
_temp1695[ 4u]={ _temp1697, _temp1698, _temp1699, _temp1700}; struct _tagged_arr
_temp1696={( void*) _temp1695,( void*) _temp1695,( void*)( _temp1695 + 4u)};
_temp1694( _temp1696);}), 0), 0), 0))->r));} else{( void*)( _temp1121->r=( void*)((
void*)( Cyc_Absyn_fncall_exp( Cyc_Toc__check_known_subscript_notnull_e,({ struct
Cyc_List_List*(* _temp1701)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1704= _temp1692;
struct Cyc_Absyn_Exp* _temp1705= Cyc_Absyn_copy_exp( _temp1121); struct Cyc_Absyn_Exp*
_temp1702[ 2u]={ _temp1704, _temp1705}; struct _tagged_arr _temp1703={( void*)
_temp1702,( void*) _temp1702,( void*)( _temp1702 + 2u)}; _temp1701( _temp1703);}),
0))->r));} goto _LL1687;} _LL1691: { void* ta1= Cyc_Toc_typ_to_c_array(
_temp1684);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( ta1, _temp1678), Cyc_Absyn_fncall_exp( Cyc_Toc__check_unknown_subscript_e,({
struct Cyc_List_List*(* _temp1706)( struct _tagged_arr)=( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp* _temp1709= _temp1123;
struct Cyc_Absyn_Exp* _temp1710= Cyc_Absyn_sizeoftyp_exp( ta1, 0); struct Cyc_Absyn_Exp*
_temp1711= _temp1121; struct Cyc_Absyn_Exp* _temp1707[ 3u]={ _temp1709,
_temp1710, _temp1711}; struct _tagged_arr _temp1708={( void*) _temp1707,( void*)
_temp1707,( void*)( _temp1707 + 3u)}; _temp1706( _temp1708);}), 0), 0), 0))->r));
goto _LL1687;} _LL1687:;} goto _LL1665; _LL1671:(( int(*)( struct _tagged_arr s))
Cyc_Toc_toc_impos)( _tag_arr("exp_to_c: Subscript on non-tuple/array/tuple ptr",
sizeof( unsigned char), 49u)); goto _LL1665; _LL1665:;} goto _LL950;} _LL1002:
if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv, 0,
0, _temp1125))->r));} else{ struct Cyc_List_List* _temp1712=(( struct Cyc_List_List*(*)(
struct _tuple3*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Toc_tup_to_c, _temp1125); struct _tagged_arr* _temp1713= Cyc_Toc_add_tuple_type(
_temp1712); struct Cyc_List_List* dles= 0;{ int i= 1; for( 0; _temp1125 != 0;(
_temp1125=(( struct Cyc_List_List*) _check_null( _temp1125))->tl, i ++)){ Cyc_Toc_exp_to_c(
nv,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1125))->hd);
dles=({ struct Cyc_List_List* _temp1714=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1714->hd=( void*)({ struct _tuple4*
_temp1715=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1715->f1=
0; _temp1715->f2=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp1125))->hd; _temp1715;}); _temp1714->tl= dles; _temp1714;});}} dles=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, dles, 0))->r));}
goto _LL950; _LL1004:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, _temp1127, 0))->r));{ struct Cyc_List_List* _temp1716= _temp1127; for( 0;
_temp1716 != 0; _temp1716=(( struct Cyc_List_List*) _check_null( _temp1716))->tl){
struct _tuple4 _temp1719; struct Cyc_Absyn_Exp* _temp1720; struct _tuple4*
_temp1717=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1716))->hd;
_temp1719=* _temp1717; _LL1721: _temp1720= _temp1719.f2; goto _LL1718; _LL1718:
Cyc_Toc_exp_to_c( nv, _temp1720);}} goto _LL950; _LL1006: { unsigned int
_temp1722= Cyc_Evexp_eval_const_uint_exp( _temp1131); Cyc_Toc_exp_to_c( nv,
_temp1129);{ struct Cyc_List_List* es= 0; if( ! Cyc_Toc_is_zero( _temp1129)){
unsigned int i= 0; for( 0; i < _temp1722; i ++){ es=({ struct Cyc_List_List*
_temp1723=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1723->hd=( void*)({ struct _tuple4* _temp1724=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1724->f1= 0; _temp1724->f2= _temp1129; _temp1724;});
_temp1723->tl= es; _temp1723;});}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0, es, 0))->r)); goto _LL950;}} _LL1008: if( ! nv->toplevel){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1137, _temp1141))->r));}
else{ if( _temp1135 == 0){(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("Struct_e: missing structdecl pointer", sizeof( unsigned char), 37u));}{
struct Cyc_Absyn_Structdecl* sd2=( struct Cyc_Absyn_Structdecl*) _check_null(
_temp1135); struct _RegionHandle _temp1725= _new_region(); struct _RegionHandle*
rgn=& _temp1725; _push_region( rgn);{ struct Cyc_List_List* _temp1726=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp1137,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( sd2->fields))->v); struct Cyc_List_List* _temp1727= 0;{ struct Cyc_List_List*
_temp1728=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd2->fields))->v;
for( 0; _temp1728 != 0; _temp1728=(( struct Cyc_List_List*) _check_null(
_temp1728))->tl){ struct Cyc_List_List* _temp1729= _temp1726; for( 0; _temp1729
!= 0; _temp1729=(( struct Cyc_List_List*) _check_null( _temp1729))->tl){ if((*((
struct _tuple11*)(( struct Cyc_List_List*) _check_null( _temp1729))->hd)).f1 ==(
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp1728))->hd){
struct _tuple11 _temp1732; struct Cyc_Absyn_Exp* _temp1733; struct Cyc_Absyn_Structfield*
_temp1735; struct _tuple11* _temp1730=( struct _tuple11*)(( struct Cyc_List_List*)
_check_null( _temp1729))->hd; _temp1732=* _temp1730; _LL1736: _temp1735=
_temp1732.f1; goto _LL1734; _LL1734: _temp1733= _temp1732.f2; goto _LL1731;
_LL1731: { void* _temp1737=( void*) _temp1735->type; Cyc_Toc_exp_to_c( nv,
_temp1733); if( Cyc_Toc_is_void_star( _temp1737)){( void*)( _temp1733->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), Cyc_Absyn_new_exp(( void*)
_temp1733->r, 0), 0))->r));} _temp1727=({ struct Cyc_List_List* _temp1738=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1738->hd=(
void*)({ struct _tuple4* _temp1739=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp1739->f1= 0; _temp1739->f2= _temp1733; _temp1739;}); _temp1738->tl=
_temp1727; _temp1738;}); break;}}}}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(
0,(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp1727), 0))->r));}; _pop_region( rgn);}} goto _LL950; _LL1010: { struct Cyc_List_List*
fs;{ void* _temp1740= Cyc_Tcutil_compress( _temp1145); struct Cyc_List_List*
_temp1746; _LL1742: if(( unsigned int) _temp1740 > 4u?*(( int*) _temp1740) ==
Cyc_Absyn_AnonStructType: 0){ _LL1747: _temp1746=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp1740)->f1; goto _LL1743;} else{ goto _LL1744;} _LL1744: goto _LL1745;
_LL1743: fs= _temp1746; goto _LL1741; _LL1745: fs=(( struct Cyc_List_List*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)(( struct _tagged_arr)({ struct
_tagged_arr(* _temp1748)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1752= _tag_arr("anon struct has type %s", sizeof(
unsigned char), 24u); struct Cyc_Stdio_String_pa_struct _temp1753; _temp1753.tag=
Cyc_Stdio_String_pa; _temp1753.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp1145);{ void* _temp1751=( void*)& _temp1753; void* _temp1749[ 1u]={
_temp1751}; struct _tagged_arr _temp1750={( void*) _temp1749,( void*) _temp1749,(
void*)( _temp1749 + 1u)}; _temp1748( _temp1752, _temp1750);}})); goto _LL1741;
_LL1741:;}{ struct _RegionHandle _temp1754= _new_region(); struct _RegionHandle*
rgn=& _temp1754; _push_region( rgn);{ struct Cyc_List_List* _temp1755=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, e->loc, _temp1143, fs); for( 0; _temp1755 != 0; _temp1755=(( struct Cyc_List_List*)
_check_null( _temp1755))->tl){ struct _tuple11 _temp1758; struct Cyc_Absyn_Exp*
_temp1759; struct Cyc_Absyn_Structfield* _temp1761; struct _tuple11* _temp1756=(
struct _tuple11*)(( struct Cyc_List_List*) _check_null( _temp1755))->hd;
_temp1758=* _temp1756; _LL1762: _temp1761= _temp1758.f1; goto _LL1760; _LL1760:
_temp1759= _temp1758.f2; goto _LL1757; _LL1757: { void* _temp1763=( void*)
_temp1761->type; Cyc_Toc_exp_to_c( nv, _temp1759); if( Cyc_Toc_is_void_star(
_temp1763)){( void*)( _temp1759->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1759->r, 0), 0))->r));}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_unresolvedmem_exp( 0, _temp1143, 0))->r));}; _pop_region( rgn);}
goto _LL950;} _LL1012: { struct _tuple0* qv= _temp1147->name; if( _temp1149->is_xtunion?
1: ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));}
else{ int tag_count= 0; struct Cyc_List_List* _temp1764= _temp1149->fields == 0?
0:( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1149->fields))->v;
while( Cyc_Absyn_qvar_cmp( qv,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp1764))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1764))->hd)->typs == 0){ tag_count ++;}
_temp1764=(( struct Cyc_List_List*) _check_null( _temp1764))->tl;}( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));} goto
_LL950;} _LL1014: { struct _tuple0* _temp1765= _temp1153->name; struct Cyc_List_List*
_temp1766= _temp1153->typs; struct _tuple0* _temp1767= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* _temp1768= Cyc_Absyn_var_exp( _temp1767, 0); void*
_temp1769= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp1765, _tag_arr("_struct",
sizeof( unsigned char), 8u))); if( nv->toplevel){ struct Cyc_Absyn_Exp* tag_exp;
if( _temp1155->is_xtunion){ tag_exp= Cyc_Absyn_var_exp( _temp1765, 0);} else{
int tag_count= 0; struct Cyc_List_List* _temp1770= _temp1155->fields == 0? 0:(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1155->fields))->v;
while( Cyc_Absyn_qvar_cmp( _temp1765,(( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp1770))->hd)->name) != 0) { if((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp1770))->hd)->typs != 0){ tag_count ++;}
_temp1770=(( struct Cyc_List_List*) _check_null( _temp1770))->tl;} tag_exp= Cyc_Absyn_uint_exp((
unsigned int) tag_count, 0);}{ struct Cyc_List_List* _temp1771= 0; for( 0;
_temp1157 != 0;( _temp1157=(( struct Cyc_List_List*) _check_null( _temp1157))->tl,
_temp1766=(( struct Cyc_List_List*) _check_null( _temp1766))->tl)){ struct Cyc_Absyn_Exp*
cur_e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1157))->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1766))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);} _temp1771=({
struct Cyc_List_List* _temp1772=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1772->hd=( void*)({ struct _tuple4* _temp1773=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1773->f1= 0;
_temp1773->f2= cur_e; _temp1773;}); _temp1772->tl= _temp1771; _temp1772;});}
_temp1771=({ struct Cyc_List_List* _temp1774=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1774->hd=( void*)({ struct _tuple4*
_temp1775=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1775->f1=
0; _temp1775->f2= tag_exp; _temp1775;}); _temp1774->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1771); _temp1774;});( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp1776=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1776->v=( void*) Cyc_Toc_collapse_qvar_tag( _temp1765, _tag_arr("_struct",
sizeof( unsigned char), 8u)); _temp1776;}), _temp1771, 0))->r));}} else{ struct
Cyc_List_List* _temp1777=({ struct Cyc_List_List* _temp1783=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1783->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( _temp1768, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp(
_temp1765, 0), 0); _temp1783->tl= 0; _temp1783;});{ int i= 1; for( 0; _temp1157
!= 0;((( _temp1157=(( struct Cyc_List_List*) _check_null( _temp1157))->tl, i ++)),
_temp1766=(( struct Cyc_List_List*) _check_null( _temp1766))->tl)){ struct Cyc_Absyn_Exp*
cur_e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1157))->hd;
void* field_typ= Cyc_Toc_typ_to_c((*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp1766))->hd)).f2); Cyc_Toc_exp_to_c( nv, cur_e); if( Cyc_Toc_is_void_star(
field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt*
_temp1778= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( _temp1768, Cyc_Toc_fieldname(
i), 0), cur_e, 0); _temp1777=({ struct Cyc_List_List* _temp1779=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1779->hd=( void*) _temp1778;
_temp1779->tl= _temp1777; _temp1779;});}}}{ struct Cyc_Absyn_Stmt* _temp1780=
Cyc_Absyn_exp_stmt( _temp1768, 0); struct Cyc_Absyn_Stmt* _temp1781= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct
Cyc_List_List* _temp1782=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1782->hd=( void*) _temp1780; _temp1782->tl= _temp1777; _temp1782;})), 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
_temp1767, _temp1769, 0, _temp1781, 0), 0))->r));}} goto _LL950;} _LL1016: goto
_LL950; _LL1018: { void* t_c= Cyc_Toc_typ_to_c( _temp1159); if( _temp1161 != 0){
struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*) _check_null( _temp1161); Cyc_Toc_exp_to_c(
nv, rgn);( void*)( e->r=( void*)(( void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp(
_temp1159, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));} goto _LL950;} _LL1020: Cyc_Toc_stmt_to_c(
nv, _temp1163); goto _LL950; _LL1022:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)(
_tag_arr("UnresolvedMem", sizeof( unsigned char), 14u)); goto _LL950; _LL1024:((
int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("compoundlit", sizeof(
unsigned char), 12u)); goto _LL950; _LL1026:(( int(*)( struct _tagged_arr s))
Cyc_Toc_unimp)( _tag_arr("codegen", sizeof( unsigned char), 8u)); goto _LL950;
_LL1028:(( int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("fill",
sizeof( unsigned char), 5u)); goto _LL950; _LL950:;}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_eq_goto_stmt( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
struct _tagged_arr* succ_lab, struct _tagged_arr* fail_lab){ return Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0);} struct _tuple12{ struct Cyc_Toc_Env* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Stmt* f3; } ; struct _tuple13{ struct _tuple0* f1; void* f2;
} ; struct _tuple14{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ;
static struct _tuple12 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env* nv, struct
_RegionHandle* rgn, void* t, struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path,
struct Cyc_Absyn_Pat* p, struct _tagged_arr* succ_lab, struct _tagged_arr*
fail_lab, struct Cyc_List_List* decls){ struct Cyc_Absyn_Stmt* s;{ void*
_temp1784=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp1820; struct Cyc_Absyn_Vardecl
_temp1822; struct _tuple0* _temp1823; struct Cyc_Absyn_Vardecl* _temp1825; int
_temp1827; void* _temp1829; unsigned char _temp1831; struct _tagged_arr
_temp1833; struct Cyc_Absyn_Enumfield* _temp1835; struct Cyc_Absyn_Enumdecl*
_temp1837; struct Cyc_List_List* _temp1839; struct Cyc_List_List* _temp1841;
struct Cyc_Absyn_Tunionfield* _temp1843; struct Cyc_Absyn_Tuniondecl* _temp1845;
struct Cyc_Absyn_Pat* _temp1848; struct Cyc_Absyn_Pat _temp1850; void* _temp1851;
struct Cyc_List_List* _temp1853; struct Cyc_List_List* _temp1855; struct Cyc_Absyn_Tunionfield*
_temp1857; struct Cyc_Absyn_Tuniondecl* _temp1859; struct Cyc_List_List*
_temp1861; struct Cyc_List_List* _temp1863; struct Cyc_List_List* _temp1865;
struct Cyc_List_List* _temp1867; struct Cyc_Core_Opt* _temp1869; struct Cyc_Absyn_Structdecl*
_temp1871; struct Cyc_Absyn_Pat* _temp1873; _LL1786: if(( unsigned int)
_temp1784 > 2u?*(( int*) _temp1784) == Cyc_Absyn_Var_p: 0){ _LL1821: _temp1820=((
struct Cyc_Absyn_Var_p_struct*) _temp1784)->f1; _temp1822=* _temp1820; _LL1824:
_temp1823= _temp1822.name; goto _LL1787;} else{ goto _LL1788;} _LL1788: if(
_temp1784 ==( void*) Cyc_Absyn_Wild_p){ goto _LL1789;} else{ goto _LL1790;}
_LL1790: if(( unsigned int) _temp1784 > 2u?*(( int*) _temp1784) == Cyc_Absyn_Reference_p:
0){ _LL1826: _temp1825=(( struct Cyc_Absyn_Reference_p_struct*) _temp1784)->f1;
goto _LL1791;} else{ goto _LL1792;} _LL1792: if( _temp1784 ==( void*) Cyc_Absyn_Null_p){
goto _LL1793;} else{ goto _LL1794;} _LL1794: if(( unsigned int) _temp1784 > 2u?*((
int*) _temp1784) == Cyc_Absyn_Int_p: 0){ _LL1830: _temp1829=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1784)->f1; goto _LL1828; _LL1828: _temp1827=((
struct Cyc_Absyn_Int_p_struct*) _temp1784)->f2; goto _LL1795;} else{ goto
_LL1796;} _LL1796: if(( unsigned int) _temp1784 > 2u?*(( int*) _temp1784) == Cyc_Absyn_Char_p:
0){ _LL1832: _temp1831=(( struct Cyc_Absyn_Char_p_struct*) _temp1784)->f1; goto
_LL1797;} else{ goto _LL1798;} _LL1798: if(( unsigned int) _temp1784 > 2u?*((
int*) _temp1784) == Cyc_Absyn_Float_p: 0){ _LL1834: _temp1833=(( struct Cyc_Absyn_Float_p_struct*)
_temp1784)->f1; goto _LL1799;} else{ goto _LL1800;} _LL1800: if(( unsigned int)
_temp1784 > 2u?*(( int*) _temp1784) == Cyc_Absyn_Enum_p: 0){ _LL1838: _temp1837=((
struct Cyc_Absyn_Enum_p_struct*) _temp1784)->f1; goto _LL1836; _LL1836:
_temp1835=(( struct Cyc_Absyn_Enum_p_struct*) _temp1784)->f2; goto _LL1801;}
else{ goto _LL1802;} _LL1802: if(( unsigned int) _temp1784 > 2u?*(( int*)
_temp1784) == Cyc_Absyn_Tunion_p: 0){ _LL1846: _temp1845=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1784)->f1; goto _LL1844; _LL1844: _temp1843=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1784)->f2; goto _LL1842; _LL1842: _temp1841=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1784)->f3; goto _LL1840; _LL1840: _temp1839=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1784)->f4; if( _temp1839 == 0){ goto _LL1803;} else{ goto _LL1804;}} else{
goto _LL1804;} _LL1804: if(( unsigned int) _temp1784 > 2u?*(( int*) _temp1784)
== Cyc_Absyn_Pointer_p: 0){ _LL1849: _temp1848=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1784)->f1; _temp1850=* _temp1848; _LL1852: _temp1851=( void*) _temp1850.r;
if(( unsigned int) _temp1851 > 2u?*(( int*) _temp1851) == Cyc_Absyn_Tunion_p: 0){
_LL1860: _temp1859=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1851)->f1; goto
_LL1858; _LL1858: _temp1857=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1851)->f2;
goto _LL1856; _LL1856: _temp1855=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1851)->f3;
goto _LL1854; _LL1854: _temp1853=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1851)->f4;
goto _LL1847;} else{ goto _LL1806;}} else{ goto _LL1806;} _LL1847: if( _temp1853
!= 0){ goto _LL1805;} else{ goto _LL1806;} _LL1806: if(( unsigned int) _temp1784
> 2u?*(( int*) _temp1784) == Cyc_Absyn_Tunion_p: 0){ _LL1862: _temp1861=((
struct Cyc_Absyn_Tunion_p_struct*) _temp1784)->f4; goto _LL1807;} else{ goto
_LL1808;} _LL1808: if(( unsigned int) _temp1784 > 2u?*(( int*) _temp1784) == Cyc_Absyn_Tuple_p:
0){ _LL1864: _temp1863=(( struct Cyc_Absyn_Tuple_p_struct*) _temp1784)->f1; goto
_LL1809;} else{ goto _LL1810;} _LL1810: if(( unsigned int) _temp1784 > 2u?*((
int*) _temp1784) == Cyc_Absyn_Struct_p: 0){ _LL1872: _temp1871=(( struct Cyc_Absyn_Struct_p_struct*)
_temp1784)->f1; goto _LL1870; _LL1870: _temp1869=(( struct Cyc_Absyn_Struct_p_struct*)
_temp1784)->f2; goto _LL1868; _LL1868: _temp1867=(( struct Cyc_Absyn_Struct_p_struct*)
_temp1784)->f3; goto _LL1866; _LL1866: _temp1865=(( struct Cyc_Absyn_Struct_p_struct*)
_temp1784)->f4; goto _LL1811;} else{ goto _LL1812;} _LL1812: if(( unsigned int)
_temp1784 > 2u?*(( int*) _temp1784) == Cyc_Absyn_Pointer_p: 0){ _LL1874:
_temp1873=(( struct Cyc_Absyn_Pointer_p_struct*) _temp1784)->f1; goto _LL1813;}
else{ goto _LL1814;} _LL1814: if(( unsigned int) _temp1784 > 2u?*(( int*)
_temp1784) == Cyc_Absyn_UnknownId_p: 0){ goto _LL1815;} else{ goto _LL1816;}
_LL1816: if(( unsigned int) _temp1784 > 2u?*(( int*) _temp1784) == Cyc_Absyn_UnknownCall_p:
0){ goto _LL1817;} else{ goto _LL1818;} _LL1818: if(( unsigned int) _temp1784 >
2u?*(( int*) _temp1784) == Cyc_Absyn_UnknownFields_p: 0){ goto _LL1819;} else{
goto _LL1785;} _LL1787: nv= Cyc_Toc_add_varmap( nv, _temp1823, r); goto _LL1789;
_LL1789: s= Cyc_Absyn_goto_stmt( succ_lab, 0); goto _LL1785; _LL1791: { struct
_tuple0* _temp1875= Cyc_Toc_temp_var(); decls=({ struct Cyc_List_List* _temp1876=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1876->hd=( void*)({ struct _tuple13* _temp1877=( struct _tuple13*)
_region_malloc( rgn, sizeof( struct _tuple13)); _temp1877->f1= _temp1875;
_temp1877->f2= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq);
_temp1877;}); _temp1876->tl= decls; _temp1876;}); nv= Cyc_Toc_add_varmap( nv,
_temp1825->name, Cyc_Absyn_var_exp( _temp1875, 0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp1875, 0), Cyc_Absyn_address_exp( path, 0), 0), Cyc_Absyn_goto_stmt(
succ_lab, 0), 0); goto _LL1785;} _LL1793: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp(
0, 0), succ_lab, fail_lab); goto _LL1785; _LL1795: s= Cyc_Toc_if_eq_goto_stmt( r,
Cyc_Absyn_int_exp( _temp1829, _temp1827, 0), succ_lab, fail_lab); goto _LL1785;
_LL1797: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_char_exp( _temp1831, 0),
succ_lab, fail_lab); goto _LL1785; _LL1799: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp(
_temp1833, 0), succ_lab, fail_lab); goto _LL1785; _LL1801: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp1878=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp1878[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp1879; _temp1879.tag= Cyc_Absyn_Enum_e;
_temp1879.f1= _temp1835->name; _temp1879.f2=( struct Cyc_Absyn_Enumdecl*)
_temp1837; _temp1879.f3=( struct Cyc_Absyn_Enumfield*) _temp1835; _temp1879;});
_temp1878;}), 0), succ_lab, fail_lab); goto _LL1785; _LL1803: { struct Cyc_Absyn_Exp*
cmp_exp= Cyc_Absyn_var_exp( _temp1843->name, 0); if( ! _temp1845->is_xtunion){
cmp_exp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), cmp_exp, 0);} s= Cyc_Toc_if_eq_goto_stmt(
r, cmp_exp, succ_lab, fail_lab); goto _LL1785;} _LL1805: { struct Cyc_List_List*
ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1853);
struct _tuple0* tufstrct= Cyc_Toc_collapse_qvar_tag( _temp1857->name, _tag_arr("_struct",
sizeof( unsigned char), 8u)); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( tufstrct), Cyc_Toc_mt_tq), r, 0); struct
Cyc_List_List* _temp1880=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1857->typs); struct Cyc_List_List*
_temp1881=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp1853); for( 0; _temp1881 != 0;((( _temp1881=((
struct Cyc_List_List*) _check_null( _temp1881))->tl, _temp1880=(( struct Cyc_List_List*)
_check_null( _temp1880))->tl)), cnt --)){ struct Cyc_Absyn_Pat* _temp1882=(
struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null( _temp1881))->hd;
if(( void*) _temp1882->r ==( void*) Cyc_Absyn_Wild_p){ continue;}{ void*
_temp1883=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp1880))->hd)).f2;
struct _tuple0* _temp1884= Cyc_Toc_temp_var(); void* _temp1885=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1882->topt))->v; void* _temp1886= Cyc_Toc_typ_to_c(
_temp1885); struct _tagged_arr* _temp1887= Cyc_Toc_fresh_label(); struct Cyc_Absyn_Exp*
_temp1888= Cyc_Absyn_structarrow_exp( rcast, Cyc_Toc_fieldname( cnt), 0); if(
Cyc_Toc_is_void_star( Cyc_Toc_typ_to_c( _temp1883))){ _temp1888= Cyc_Absyn_cast_exp(
_temp1886, _temp1888, 0);} decls=({ struct Cyc_List_List* _temp1889=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1889->hd=( void*)({
struct _tuple13* _temp1890=( struct _tuple13*) _region_malloc( rgn, sizeof(
struct _tuple13)); _temp1890->f1= _temp1884; _temp1890->f2= _temp1886; _temp1890;});
_temp1889->tl= decls; _temp1889;});{ struct _tuple12 _temp1891= Cyc_Toc_xlate_pat(
nv, rgn, _temp1885, Cyc_Absyn_var_exp( _temp1884, 0), _temp1888, _temp1882,
succ_lab, fail_lab, decls); nv= _temp1891.f1; decls= _temp1891.f2;{ struct Cyc_Absyn_Stmt*
_temp1892= _temp1891.f3; struct Cyc_Absyn_Stmt* _temp1893= Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( _temp1884, 0), _temp1888, 0); struct Cyc_Absyn_Stmt*
_temp1894= Cyc_Absyn_seq_stmt( _temp1893, _temp1892, 0); ss=({ struct Cyc_List_List*
_temp1895=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1895->hd=( void*) Cyc_Absyn_label_stmt( _temp1887, _temp1894, 0); _temp1895->tl=
ss; _temp1895;}); succ_lab= _temp1887;}}}} if( ss == 0){ ss=({ struct Cyc_List_List*
_temp1896=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1896->hd=( void*) Cyc_Absyn_goto_stmt( succ_lab, 0); _temp1896->tl= 0;
_temp1896;});}{ struct Cyc_Absyn_Exp* test_exp; if( _temp1859->is_xtunion){
struct Cyc_Absyn_Exp* e2= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_void_star_typ(),
Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_deref_exp( e2, 0);
struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( _temp1857->name, 0); test_exp= Cyc_Absyn_eq_exp(
e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts(
ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);} else{ unsigned int max_tag= 0;{
struct Cyc_List_List* _temp1897=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1859->fields))->v; for( 0; _temp1897 != 0; _temp1897=(( struct
Cyc_List_List*) _check_null( _temp1897))->tl){ struct Cyc_Absyn_Tunionfield*
_temp1898=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp1897))->hd; if( _temp1898->typs == 0){ max_tag ++;}}}{ struct Cyc_Absyn_Exp*
max_tag_exp= Cyc_Absyn_uint_exp( max_tag, 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ( Cyc_Absyn_sint_t, Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_t, r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp(
_temp1857->name, 0); test_exp= max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp(
e1, e, 0), 0): Cyc_Absyn_eq_exp( e1, e, 0);}} s= Cyc_Absyn_ifthenelse_stmt(
test_exp, Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);
goto _LL1785;}} _LL1807: _temp1863= _temp1861; goto _LL1809; _LL1809: { struct
Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1863);{ struct Cyc_List_List* _temp1899=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x)) Cyc_List_rrev)( rgn, _temp1863); for(
0; _temp1899 != 0;( _temp1899=(( struct Cyc_List_List*) _check_null( _temp1899))->tl,
cnt --)){ struct Cyc_Absyn_Pat* _temp1900=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp1899))->hd; if(( void*) _temp1900->r ==( void*) Cyc_Absyn_Wild_p){
continue;}{ struct _tuple0* _temp1901= Cyc_Toc_temp_var(); void* _temp1902=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1900->topt))->v; struct
_tagged_arr* _temp1903= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1904=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1904->hd=( void*)({ struct _tuple13* _temp1905=( struct _tuple13*)
_region_malloc( rgn, sizeof( struct _tuple13)); _temp1905->f1= _temp1901;
_temp1905->f2= Cyc_Toc_typ_to_c( _temp1902); _temp1905;}); _temp1904->tl= decls;
_temp1904;});{ struct _tuple12 _temp1906= Cyc_Toc_xlate_pat( nv, rgn, _temp1902,
Cyc_Absyn_var_exp( _temp1901, 0), Cyc_Absyn_structmember_exp( path, Cyc_Toc_fieldname(
cnt), 0), _temp1900, succ_lab, fail_lab, decls); nv= _temp1906.f1; decls=
_temp1906.f2;{ struct Cyc_Absyn_Stmt* _temp1907= _temp1906.f3; struct Cyc_Absyn_Stmt*
_temp1908= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp(
_temp1901, 0), Cyc_Absyn_structmember_exp( r, Cyc_Toc_fieldname( cnt), 0), 0),
_temp1907, 0); ss=({ struct Cyc_List_List* _temp1909=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1909->hd=( void*) Cyc_Absyn_label_stmt(
_temp1903, _temp1908, 0); _temp1909->tl= ss; _temp1909;}); succ_lab= _temp1903;}}}}}
if( ss == 0){ ss=({ struct Cyc_List_List* _temp1910=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1910->hd=( void*) Cyc_Absyn_goto_stmt(
succ_lab, 0); _temp1910->tl= 0; _temp1910;});} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL1785;} _LL1811: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
_temp1911=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x)) Cyc_List_rrev)( rgn, _temp1865); for( 0; _temp1911 != 0; _temp1911=(( struct
Cyc_List_List*) _check_null( _temp1911))->tl){ struct _tuple14* _temp1912=(
struct _tuple14*)(( struct Cyc_List_List*) _check_null( _temp1911))->hd; struct
Cyc_Absyn_Pat* _temp1913=(* _temp1912).f2; if(( void*) _temp1913->r ==( void*)
Cyc_Absyn_Wild_p){ continue;}{ struct _tagged_arr* f;{ void* _temp1914=( void*)((
struct Cyc_List_List*) _check_null((* _temp1912).f1))->hd; struct _tagged_arr*
_temp1920; _LL1916: if(*(( int*) _temp1914) == Cyc_Absyn_FieldName){ _LL1921:
_temp1920=(( struct Cyc_Absyn_FieldName_struct*) _temp1914)->f1; goto _LL1917;}
else{ goto _LL1918;} _LL1918: goto _LL1919; _LL1917: f= _temp1920; goto _LL1915;
_LL1919:( int) _throw(( void*) Cyc_Toc_Match_error); _LL1915:;}{ struct _tuple0*
_temp1922= Cyc_Toc_temp_var(); void* _temp1923=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1913->topt))->v; void* _temp1924= Cyc_Toc_typ_to_c( _temp1923);
struct _tagged_arr* _temp1925= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1926=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1926->hd=( void*)({ struct _tuple13* _temp1927=( struct _tuple13*)
_region_malloc( rgn, sizeof( struct _tuple13)); _temp1927->f1= _temp1922;
_temp1927->f2= _temp1924; _temp1927;}); _temp1926->tl= decls; _temp1926;});{
struct _tuple12 _temp1928= Cyc_Toc_xlate_pat( nv, rgn, _temp1923, Cyc_Absyn_var_exp(
_temp1922, 0), Cyc_Absyn_structmember_exp( path, f, 0), _temp1913, succ_lab,
fail_lab, decls); nv= _temp1928.f1; decls= _temp1928.f2;{ struct Cyc_Absyn_Exp*
_temp1929= Cyc_Absyn_structmember_exp( r, f, 0); if( Cyc_Toc_is_void_star(( void*)((
struct Cyc_Absyn_Structfield*) _check_null( Cyc_Absyn_lookup_field(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1871->fields))->v, f)))->type)){
_temp1929= Cyc_Absyn_cast_exp( _temp1924, _temp1929, 0);}{ struct Cyc_Absyn_Stmt*
_temp1930= _temp1928.f3; struct Cyc_Absyn_Stmt* _temp1931= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1922, 0), _temp1929, 0),
_temp1930, 0); ss=({ struct Cyc_List_List* _temp1932=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1932->hd=( void*) Cyc_Absyn_label_stmt(
_temp1925, _temp1931, 0); _temp1932->tl= ss; _temp1932;}); succ_lab= _temp1925;}}}}}}}
if( ss == 0){ ss=({ struct Cyc_List_List* _temp1933=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1933->hd=( void*) Cyc_Absyn_goto_stmt(
succ_lab, 0); _temp1933->tl= 0; _temp1933;});} s= Cyc_Absyn_seq_stmts( ss, 0);
goto _LL1785;} _LL1813: { struct _tuple0* _temp1934= Cyc_Toc_temp_var(); void*
_temp1935=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1873->topt))->v;
decls=({ struct Cyc_List_List* _temp1936=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp1936->hd=( void*)({ struct _tuple13*
_temp1937=( struct _tuple13*) _region_malloc( rgn, sizeof( struct _tuple13));
_temp1937->f1= _temp1934; _temp1937->f2= Cyc_Toc_typ_to_c( _temp1935); _temp1937;});
_temp1936->tl= decls; _temp1936;});{ struct _tuple12 _temp1938= Cyc_Toc_xlate_pat(
nv, rgn, _temp1935, Cyc_Absyn_var_exp( _temp1934, 0), Cyc_Absyn_deref_exp( path,
0), _temp1873, succ_lab, fail_lab, decls); nv= _temp1938.f1; decls= _temp1938.f2;{
struct Cyc_Absyn_Stmt* _temp1939= _temp1938.f3; struct Cyc_Absyn_Stmt* _temp1940=
Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( _temp1934, 0), Cyc_Absyn_deref_exp(
r, 0), 0), _temp1939, 0); if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), _temp1940, 0);} else{ s= _temp1940;} goto _LL1785;}}} _LL1815: s=((
struct Cyc_Absyn_Stmt*(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknownid",
sizeof( unsigned char), 10u)); goto _LL1785; _LL1817: s=(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknowncall", sizeof(
unsigned char), 12u)); goto _LL1785; _LL1819: s=(( struct Cyc_Absyn_Stmt*(*)(
struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("unknownfields", sizeof(
unsigned char), 14u)); goto _LL1785; _LL1785:;} return({ struct _tuple12
_temp1941; _temp1941.f1= nv; _temp1941.f2= decls; _temp1941.f3= s; _temp1941;});}
struct _tuple15{ struct _tagged_arr* f1; struct _tagged_arr* f2; struct Cyc_Absyn_Switch_clause*
f3; } ; static struct _tuple15* Cyc_Toc_gen_label( struct _RegionHandle* r,
struct Cyc_Absyn_Switch_clause* sc){ return({ struct _tuple15* _temp1942=(
struct _tuple15*) _region_malloc( r, sizeof( struct _tuple15)); _temp1942->f1=
Cyc_Toc_fresh_label(); _temp1942->f2= Cyc_Toc_fresh_label(); _temp1942->f3= sc;
_temp1942;});} static void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* scs){
Cyc_Toc_exp_to_c( nv, e);{ void* _temp1943=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; int leave_as_switch;{ void* _temp1944= Cyc_Tcutil_compress(
_temp1943); _LL1946: if(( unsigned int) _temp1944 > 4u?*(( int*) _temp1944) ==
Cyc_Absyn_IntType: 0){ goto _LL1947;} else{ goto _LL1948;} _LL1948: goto _LL1949;
_LL1947: leave_as_switch= 1; goto _LL1945; _LL1949: leave_as_switch= 0; goto
_LL1945; _LL1945:;}{ struct Cyc_List_List* _temp1950= scs; for( 0; _temp1950 !=
0; _temp1950=(( struct Cyc_List_List*) _check_null( _temp1950))->tl){ if((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp1950))->hd)->pat_vars))->v != 0? 1:((
struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( _temp1950))->hd)->where_clause
!= 0){ leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_arr*
next_l= Cyc_Toc_fresh_label();{ struct Cyc_List_List* _temp1951= scs; for( 0;
_temp1951 != 0; _temp1951=(( struct Cyc_List_List*) _check_null( _temp1951))->tl){
struct Cyc_Absyn_Stmt* _temp1952=(( struct Cyc_Absyn_Switch_clause*)(( struct
Cyc_List_List*) _check_null( _temp1951))->hd)->body;(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp1951))->hd)->body= Cyc_Absyn_label_stmt(
next_l, _temp1952, 0); next_l= Cyc_Toc_fresh_label(); Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env(
nv, next_l), _temp1952);}} return;}{ struct _tuple0* v= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* r= Cyc_Absyn_var_exp( v, 0); struct Cyc_Absyn_Exp* path=
Cyc_Absyn_var_exp( v, 0); struct _tagged_arr* end_l= Cyc_Toc_fresh_label();
struct _RegionHandle _temp1953= _new_region(); struct _RegionHandle* rgn=&
_temp1953; _push_region( rgn);{ struct Cyc_List_List* lscs=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple15*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Toc_gen_label,
rgn, scs); struct Cyc_List_List* test_stmts= 0; struct Cyc_List_List* nvs= 0;
struct Cyc_List_List* decls= 0;{ struct Cyc_List_List* _temp1954= lscs; for( 0;
_temp1954 != 0; _temp1954=(( struct Cyc_List_List*) _check_null( _temp1954))->tl){
struct Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple15*)(( struct Cyc_List_List*)
_check_null( _temp1954))->hd)).f3; struct _tagged_arr* fail_lab=(( struct Cyc_List_List*)
_check_null( _temp1954))->tl == 0? end_l:(*(( struct _tuple15*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp1954))->tl))->hd)).f1;
struct _tagged_arr* succ_lab=(*(( struct _tuple15*)(( struct Cyc_List_List*)
_check_null( _temp1954))->hd)).f2; if( sc->where_clause == 0){ struct _tuple12
_temp1955= Cyc_Toc_xlate_pat( nv, rgn, _temp1943, r, path, sc->pattern, succ_lab,
fail_lab, decls); nvs=({ struct Cyc_List_List* _temp1956=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1956->hd=( void*)
_temp1955.f1; _temp1956->tl= nvs; _temp1956;}); decls= _temp1955.f2; test_stmts=({
struct Cyc_List_List* _temp1957=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp1957->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( _temp1954))->hd)).f1,
_temp1955.f3, 0); _temp1957->tl= test_stmts; _temp1957;});} else{ struct Cyc_Absyn_Exp*
_temp1958=( struct Cyc_Absyn_Exp*) _check_null( sc->where_clause); struct
_tagged_arr* _temp1959= Cyc_Toc_fresh_label(); struct _tuple12 _temp1960= Cyc_Toc_xlate_pat(
nv, rgn, _temp1943, r, path, sc->pattern, _temp1959, fail_lab, decls); Cyc_Toc_exp_to_c(
_temp1960.f1, _temp1958); nvs=({ struct Cyc_List_List* _temp1961=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1961->hd=( void*)
_temp1960.f1; _temp1961->tl= nvs; _temp1961;}); decls= _temp1960.f2;{ struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_ifthenelse_stmt( _temp1958, Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_label_stmt( _temp1959, s2,
0); test_stmts=({ struct Cyc_List_List* _temp1962=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1962->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( _temp1954))->hd)).f1,
Cyc_Absyn_seq_stmt( _temp1960.f3, s3, 0), 0); _temp1962->tl= test_stmts;
_temp1962;});}}}}{ struct Cyc_Absyn_Stmt* _temp1963= Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
test_stmts), 0); nvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
nvs);{ struct Cyc_Absyn_Stmt* clauses= Cyc_Absyn_skip_stmt( 0); if( lscs != 0){
for( 0;(( struct Cyc_List_List*) _check_null( lscs))->tl != 0;( lscs=(( struct
Cyc_List_List*) _check_null( lscs))->tl, nvs=(( struct Cyc_List_List*)
_check_null( nvs))->tl)){ struct Cyc_Toc_Env* _temp1964=( struct Cyc_Toc_Env*)((
struct Cyc_List_List*) _check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body;
Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env( _temp1964, end_l,(*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( lscs))->tl))->hd)).f2,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null(((*(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( lscs))->tl))->hd)).f3)->pat_vars))->v,(
struct Cyc_Toc_Env*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( nvs))->tl))->hd), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple15*)(( struct Cyc_List_List*) _check_null( lscs))->hd)).f2, s, 0),
0);}{ struct Cyc_Toc_Env* _temp1965=( struct Cyc_Toc_Env*)(( struct Cyc_List_List*)
_check_null( nvs))->hd; struct Cyc_Absyn_Stmt* s=((*(( struct _tuple15*)((
struct Cyc_List_List*) _check_null( lscs))->hd)).f3)->body; Cyc_Toc_stmt_to_c(
Cyc_Toc_last_switchclause_env( _temp1965, end_l), s); clauses= Cyc_Absyn_seq_stmt(
clauses, Cyc_Absyn_label_stmt((*(( struct _tuple15*)(( struct Cyc_List_List*)
_check_null( lscs))->hd)).f2, s, 0), 0);}}{ struct Cyc_Absyn_Stmt* res= Cyc_Absyn_seq_stmt(
_temp1963, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt( end_l, Cyc_Absyn_skip_stmt(
0), 0), 0), 0); for( decls; decls != 0; decls=(( struct Cyc_List_List*)
_check_null( decls))->tl){ struct _tuple13 _temp1968; void* _temp1969; struct
_tuple0* _temp1971; struct _tuple13* _temp1966=( struct _tuple13*)(( struct Cyc_List_List*)
_check_null( decls))->hd; _temp1968=* _temp1966; _LL1972: _temp1971= _temp1968.f1;
goto _LL1970; _LL1970: _temp1969= _temp1968.f2; goto _LL1967; _LL1967: res= Cyc_Absyn_declare_stmt(
_temp1971, _temp1969, 0, res, 0);}( void*)( whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
v, Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v),(
struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}; _pop_region( rgn);}}} static
struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat*
p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt*
s); static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv, struct Cyc_Toc_Env*
init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt* s); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__npop_handler_e,({ struct Cyc_List_List* _temp1973=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1973[ 0]=({ struct Cyc_List_List
_temp1974; _temp1974.hd=( void*) Cyc_Absyn_uint_exp(( unsigned int)( n - 1), 0);
_temp1974.tl= 0; _temp1974;}); _temp1973;}), 0), 0);} void Cyc_Toc_do_npop_before(
int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt(( void*) s->r,
0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env* nv, struct
Cyc_Absyn_Stmt* s){ while( 1) { void* _temp1975=( void*) s->r; struct Cyc_Absyn_Exp*
_temp2017; struct Cyc_Absyn_Stmt* _temp2019; struct Cyc_Absyn_Stmt* _temp2021;
struct Cyc_Absyn_Exp* _temp2023; struct Cyc_Absyn_Stmt* _temp2025; struct Cyc_Absyn_Stmt*
_temp2027; struct Cyc_Absyn_Exp* _temp2029; struct Cyc_Absyn_Stmt* _temp2031;
struct _tuple2 _temp2033; struct Cyc_Absyn_Exp* _temp2035; struct Cyc_Absyn_Stmt*
_temp2037; struct Cyc_Absyn_Stmt* _temp2039; struct Cyc_Absyn_Stmt* _temp2041;
struct Cyc_Absyn_Stmt* _temp2043; struct _tuple2 _temp2045; struct Cyc_Absyn_Exp*
_temp2047; struct _tuple2 _temp2049; struct Cyc_Absyn_Exp* _temp2051; struct Cyc_Absyn_Exp*
_temp2053; struct Cyc_List_List* _temp2055; struct Cyc_Absyn_Exp* _temp2057;
struct Cyc_Absyn_Switch_clause** _temp2059; struct Cyc_List_List* _temp2061;
struct Cyc_Absyn_Stmt* _temp2063; struct Cyc_Absyn_Decl* _temp2065; struct Cyc_Absyn_Stmt*
_temp2067; struct _tagged_arr* _temp2069; struct _tuple2 _temp2071; struct Cyc_Absyn_Exp*
_temp2073; struct Cyc_Absyn_Stmt* _temp2075; struct Cyc_List_List* _temp2077;
struct Cyc_Absyn_Stmt* _temp2079; struct Cyc_Absyn_Stmt* _temp2081; struct Cyc_Absyn_Vardecl*
_temp2083; struct Cyc_Absyn_Tvar* _temp2085; _LL1977: if( _temp1975 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL1978;} else{ goto _LL1979;} _LL1979: if((
unsigned int) _temp1975 > 1u?*(( int*) _temp1975) == Cyc_Absyn_Exp_s: 0){
_LL2018: _temp2017=(( struct Cyc_Absyn_Exp_s_struct*) _temp1975)->f1; goto
_LL1980;} else{ goto _LL1981;} _LL1981: if(( unsigned int) _temp1975 > 1u?*((
int*) _temp1975) == Cyc_Absyn_Seq_s: 0){ _LL2022: _temp2021=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1975)->f1; goto _LL2020; _LL2020: _temp2019=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1975)->f2; goto _LL1982;} else{ goto _LL1983;} _LL1983: if(( unsigned int)
_temp1975 > 1u?*(( int*) _temp1975) == Cyc_Absyn_Return_s: 0){ _LL2024:
_temp2023=(( struct Cyc_Absyn_Return_s_struct*) _temp1975)->f1; goto _LL1984;}
else{ goto _LL1985;} _LL1985: if(( unsigned int) _temp1975 > 1u?*(( int*)
_temp1975) == Cyc_Absyn_IfThenElse_s: 0){ _LL2030: _temp2029=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1975)->f1; goto _LL2028; _LL2028: _temp2027=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1975)->f2; goto _LL2026; _LL2026: _temp2025=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1975)->f3; goto _LL1986;} else{ goto _LL1987;} _LL1987: if(( unsigned int)
_temp1975 > 1u?*(( int*) _temp1975) == Cyc_Absyn_While_s: 0){ _LL2034: _temp2033=((
struct Cyc_Absyn_While_s_struct*) _temp1975)->f1; _LL2036: _temp2035= _temp2033.f1;
goto _LL2032; _LL2032: _temp2031=(( struct Cyc_Absyn_While_s_struct*) _temp1975)->f2;
goto _LL1988;} else{ goto _LL1989;} _LL1989: if(( unsigned int) _temp1975 > 1u?*((
int*) _temp1975) == Cyc_Absyn_Break_s: 0){ _LL2038: _temp2037=(( struct Cyc_Absyn_Break_s_struct*)
_temp1975)->f1; goto _LL1990;} else{ goto _LL1991;} _LL1991: if(( unsigned int)
_temp1975 > 1u?*(( int*) _temp1975) == Cyc_Absyn_Continue_s: 0){ _LL2040:
_temp2039=(( struct Cyc_Absyn_Continue_s_struct*) _temp1975)->f1; goto _LL1992;}
else{ goto _LL1993;} _LL1993: if(( unsigned int) _temp1975 > 1u?*(( int*)
_temp1975) == Cyc_Absyn_Goto_s: 0){ _LL2042: _temp2041=(( struct Cyc_Absyn_Goto_s_struct*)
_temp1975)->f2; goto _LL1994;} else{ goto _LL1995;} _LL1995: if(( unsigned int)
_temp1975 > 1u?*(( int*) _temp1975) == Cyc_Absyn_For_s: 0){ _LL2054: _temp2053=((
struct Cyc_Absyn_For_s_struct*) _temp1975)->f1; goto _LL2050; _LL2050: _temp2049=((
struct Cyc_Absyn_For_s_struct*) _temp1975)->f2; _LL2052: _temp2051= _temp2049.f1;
goto _LL2046; _LL2046: _temp2045=(( struct Cyc_Absyn_For_s_struct*) _temp1975)->f3;
_LL2048: _temp2047= _temp2045.f1; goto _LL2044; _LL2044: _temp2043=(( struct Cyc_Absyn_For_s_struct*)
_temp1975)->f4; goto _LL1996;} else{ goto _LL1997;} _LL1997: if(( unsigned int)
_temp1975 > 1u?*(( int*) _temp1975) == Cyc_Absyn_Switch_s: 0){ _LL2058:
_temp2057=(( struct Cyc_Absyn_Switch_s_struct*) _temp1975)->f1; goto _LL2056;
_LL2056: _temp2055=(( struct Cyc_Absyn_Switch_s_struct*) _temp1975)->f2; goto
_LL1998;} else{ goto _LL1999;} _LL1999: if(( unsigned int) _temp1975 > 1u?*((
int*) _temp1975) == Cyc_Absyn_Fallthru_s: 0){ _LL2062: _temp2061=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1975)->f1; goto _LL2060; _LL2060: _temp2059=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1975)->f2; goto _LL2000;} else{ goto _LL2001;} _LL2001: if(( unsigned int)
_temp1975 > 1u?*(( int*) _temp1975) == Cyc_Absyn_Decl_s: 0){ _LL2066: _temp2065=((
struct Cyc_Absyn_Decl_s_struct*) _temp1975)->f1; goto _LL2064; _LL2064:
_temp2063=(( struct Cyc_Absyn_Decl_s_struct*) _temp1975)->f2; goto _LL2002;}
else{ goto _LL2003;} _LL2003: if(( unsigned int) _temp1975 > 1u?*(( int*)
_temp1975) == Cyc_Absyn_Label_s: 0){ _LL2070: _temp2069=(( struct Cyc_Absyn_Label_s_struct*)
_temp1975)->f1; goto _LL2068; _LL2068: _temp2067=(( struct Cyc_Absyn_Label_s_struct*)
_temp1975)->f2; goto _LL2004;} else{ goto _LL2005;} _LL2005: if(( unsigned int)
_temp1975 > 1u?*(( int*) _temp1975) == Cyc_Absyn_Do_s: 0){ _LL2076: _temp2075=((
struct Cyc_Absyn_Do_s_struct*) _temp1975)->f1; goto _LL2072; _LL2072: _temp2071=((
struct Cyc_Absyn_Do_s_struct*) _temp1975)->f2; _LL2074: _temp2073= _temp2071.f1;
goto _LL2006;} else{ goto _LL2007;} _LL2007: if(( unsigned int) _temp1975 > 1u?*((
int*) _temp1975) == Cyc_Absyn_TryCatch_s: 0){ _LL2080: _temp2079=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1975)->f1; goto _LL2078; _LL2078: _temp2077=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1975)->f2; goto _LL2008;} else{ goto _LL2009;} _LL2009: if(( unsigned int)
_temp1975 > 1u?*(( int*) _temp1975) == Cyc_Absyn_Region_s: 0){ _LL2086:
_temp2085=(( struct Cyc_Absyn_Region_s_struct*) _temp1975)->f1; goto _LL2084;
_LL2084: _temp2083=(( struct Cyc_Absyn_Region_s_struct*) _temp1975)->f2; goto
_LL2082; _LL2082: _temp2081=(( struct Cyc_Absyn_Region_s_struct*) _temp1975)->f3;
goto _LL2010;} else{ goto _LL2011;} _LL2011: if(( unsigned int) _temp1975 > 1u?*((
int*) _temp1975) == Cyc_Absyn_SwitchC_s: 0){ goto _LL2012;} else{ goto _LL2013;}
_LL2013: if(( unsigned int) _temp1975 > 1u?*(( int*) _temp1975) == Cyc_Absyn_Cut_s:
0){ goto _LL2014;} else{ goto _LL2015;} _LL2015: if(( unsigned int) _temp1975 >
1u?*(( int*) _temp1975) == Cyc_Absyn_Splice_s: 0){ goto _LL2016;} else{ goto
_LL1976;} _LL1978: return; _LL1980: Cyc_Toc_exp_to_c( nv, _temp2017); return;
_LL1982: Cyc_Toc_stmt_to_c( nv, _temp2021); s= _temp2019; continue; _LL1984: {
struct Cyc_Core_Opt* topt= 0; if( _temp2023 != 0){ topt=({ struct Cyc_Core_Opt*
_temp2087=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2087->v=( void*) Cyc_Toc_typ_to_c(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*) _check_null( _temp2023))->topt))->v);
_temp2087;}); Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*) _check_null(
_temp2023));} if( s->try_depth > 0){ if( topt != 0){ struct _tuple0* _temp2088=
Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* _temp2089= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( _temp2088, 0), 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( _temp2088,( void*)(( struct Cyc_Core_Opt*)
_check_null( topt))->v, _temp2023, Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler(
s->try_depth), _temp2089, 0), 0))->r));} else{ Cyc_Toc_do_npop_before( s->try_depth,
s);}} return;} _LL1986: Cyc_Toc_exp_to_c( nv, _temp2029); Cyc_Toc_stmt_to_c( nv,
_temp2027); s= _temp2025; continue; _LL1988: Cyc_Toc_exp_to_c( nv, _temp2035);
Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2031); return; _LL1990: if( nv->break_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( nv->break_lab))->v, 0))->r));}{ int
dest_depth= _temp2037 == 0? 0:(( struct Cyc_Absyn_Stmt*) _check_null( _temp2037))->try_depth;
Cyc_Toc_do_npop_before( s->try_depth - dest_depth, s); return;} _LL1992: if( nv->continue_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( nv->continue_lab))->v, 0))->r));} _temp2041=
_temp2039; goto _LL1994; _LL1994: Cyc_Toc_do_npop_before( s->try_depth -((
struct Cyc_Absyn_Stmt*) _check_null( _temp2041))->try_depth, s); return; _LL1996:
Cyc_Toc_exp_to_c( nv, _temp2053); Cyc_Toc_exp_to_c( nv, _temp2051); Cyc_Toc_exp_to_c(
nv, _temp2047); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2043); return;
_LL1998: Cyc_Toc_xlate_switch( nv, s, _temp2057, _temp2055); return; _LL2000:
if( nv->fallthru_info == 0){( int) _throw( Cyc_Toc_toc_impos( _tag_arr("fallthru in unexpected place",
sizeof( unsigned char), 29u)));}{ struct _tuple6 _temp2092; struct Cyc_Dict_Dict*
_temp2093; struct Cyc_List_List* _temp2095; struct _tagged_arr* _temp2097;
struct _tuple6* _temp2090=( struct _tuple6*)(( struct Cyc_Core_Opt*) _check_null(
nv->fallthru_info))->v; _temp2092=* _temp2090; _LL2098: _temp2097= _temp2092.f1;
goto _LL2096; _LL2096: _temp2095= _temp2092.f2; goto _LL2094; _LL2094: _temp2093=
_temp2092.f3; goto _LL2091; _LL2091: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt(
_temp2097, 0); Cyc_Toc_do_npop_before( s->try_depth -((*(( struct Cyc_Absyn_Switch_clause**)
_check_null( _temp2059)))->body)->try_depth, s2);{ struct Cyc_List_List*
_temp2099=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2095); struct Cyc_List_List* _temp2100=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( _temp2061); for( 0; _temp2099 != 0;( _temp2099=((
struct Cyc_List_List*) _check_null( _temp2099))->tl, _temp2100=(( struct Cyc_List_List*)
_check_null( _temp2100))->tl)){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp2100))->hd); s2= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key)) Cyc_Dict_lookup)( _temp2093,( struct _tuple0*)(( struct
Cyc_List_List*) _check_null( _temp2099))->hd),( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( _temp2100))->hd, 0), s2, 0);}( void*)( s->r=( void*)((
void*) s2->r)); return;}}} _LL2002:{ void* _temp2101=( void*) _temp2065->r;
struct Cyc_Absyn_Vardecl* _temp2111; int _temp2113; struct Cyc_Absyn_Exp*
_temp2115; struct Cyc_Core_Opt* _temp2117; struct Cyc_Absyn_Pat* _temp2119;
struct Cyc_List_List* _temp2121; _LL2103: if(*(( int*) _temp2101) == Cyc_Absyn_Var_d){
_LL2112: _temp2111=(( struct Cyc_Absyn_Var_d_struct*) _temp2101)->f1; goto
_LL2104;} else{ goto _LL2105;} _LL2105: if(*(( int*) _temp2101) == Cyc_Absyn_Let_d){
_LL2120: _temp2119=(( struct Cyc_Absyn_Let_d_struct*) _temp2101)->f1; goto
_LL2118; _LL2118: _temp2117=(( struct Cyc_Absyn_Let_d_struct*) _temp2101)->f3;
goto _LL2116; _LL2116: _temp2115=(( struct Cyc_Absyn_Let_d_struct*) _temp2101)->f4;
goto _LL2114; _LL2114: _temp2113=(( struct Cyc_Absyn_Let_d_struct*) _temp2101)->f5;
goto _LL2106;} else{ goto _LL2107;} _LL2107: if(*(( int*) _temp2101) == Cyc_Absyn_Letv_d){
_LL2122: _temp2121=(( struct Cyc_Absyn_Letv_d_struct*) _temp2101)->f1; goto
_LL2108;} else{ goto _LL2109;} _LL2109: goto _LL2110; _LL2104: { struct Cyc_Toc_Env*
_temp2123= Cyc_Toc_add_varmap( nv, _temp2111->name, Cyc_Absyn_varb_exp(
_temp2111->name,( void*)({ struct Cyc_Absyn_Local_b_struct* _temp2124=( struct
Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp2124[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp2125; _temp2125.tag= Cyc_Absyn_Local_b;
_temp2125.f1= _temp2111; _temp2125;}); _temp2124;}), 0)); Cyc_Toc_local_decl_to_c(
_temp2123, _temp2123, _temp2111, _temp2063); goto _LL2102;} _LL2106:{ void*
_temp2126=( void*) _temp2119->r; struct Cyc_Absyn_Vardecl* _temp2132; _LL2128:
if(( unsigned int) _temp2126 > 2u?*(( int*) _temp2126) == Cyc_Absyn_Var_p: 0){
_LL2133: _temp2132=(( struct Cyc_Absyn_Var_p_struct*) _temp2126)->f1; goto
_LL2129;} else{ goto _LL2130;} _LL2130: goto _LL2131; _LL2129: { struct _tuple0*
old_name= _temp2132->name; struct _tuple0* new_name= Cyc_Toc_temp_var();
_temp2132->name= new_name; _temp2132->initializer=( struct Cyc_Absyn_Exp*)
_temp2115;( void*)( _temp2065->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2134=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2134[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2135; _temp2135.tag= Cyc_Absyn_Var_d;
_temp2135.f1= _temp2132; _temp2135;}); _temp2134;})));{ struct Cyc_Toc_Env*
_temp2136= Cyc_Toc_add_varmap( nv, old_name, Cyc_Absyn_varb_exp( new_name,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2137=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2137[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2138; _temp2138.tag= Cyc_Absyn_Local_b; _temp2138.f1=
_temp2132; _temp2138;}); _temp2137;}), 0)); Cyc_Toc_local_decl_to_c( _temp2136,
nv, _temp2132, _temp2063); goto _LL2127;}} _LL2131:( void*)( s->r=( void*)((
void*)( Cyc_Toc_letdecl_to_c( nv, _temp2119, _temp2117, _temp2115, _temp2113,
_temp2063))->r)); goto _LL2127; _LL2127:;} goto _LL2102; _LL2108: { struct Cyc_List_List*
_temp2139=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp2121); if( _temp2139 == 0){(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)(
_tag_arr("empty Letv_d", sizeof( unsigned char), 13u));}( void*)( _temp2065->r=(
void*)(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp2140=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2140[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2141; _temp2141.tag= Cyc_Absyn_Var_d; _temp2141.f1=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp2139))->hd; _temp2141;}); _temp2140;})));
_temp2139=(( struct Cyc_List_List*) _check_null( _temp2139))->tl; for( 0;
_temp2139 != 0; _temp2139=(( struct Cyc_List_List*) _check_null( _temp2139))->tl){
struct Cyc_Absyn_Decl* _temp2142= Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2143=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2143[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2144; _temp2144.tag= Cyc_Absyn_Var_d;
_temp2144.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp2139))->hd; _temp2144;}); _temp2143;}), 0);( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_decl_stmt( _temp2142, Cyc_Absyn_new_stmt(( void*) s->r, 0), 0))->r));}
Cyc_Toc_stmt_to_c( nv, s); goto _LL2102;} _LL2110:(( int(*)( struct _tagged_arr))
Cyc_Tcutil_impos)( _tag_arr("bad nested declaration within function", sizeof(
unsigned char), 39u)); goto _LL2102; _LL2102:;} return; _LL2004: s= _temp2067;
continue; _LL2006: Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2075); Cyc_Toc_exp_to_c(
nv, _temp2073); return; _LL2008: { struct _tuple0* h_var= Cyc_Toc_temp_var();
struct _tuple0* e_var= Cyc_Toc_temp_var(); struct _tuple0* was_thrown_var= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* h_exp= Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp*
e_exp= Cyc_Absyn_var_exp( e_var, 0); struct Cyc_Absyn_Exp* was_thrown_exp= Cyc_Absyn_var_exp(
was_thrown_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); void* was_thrown_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_sint_t); e_exp->topt=({ struct Cyc_Core_Opt* _temp2145=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2145->v=( void*) e_typ; _temp2145;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp2079);{
struct Cyc_Absyn_Stmt* _temp2146= Cyc_Absyn_seq_stmt( _temp2079, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( Cyc_Toc__pop_handler_e, 0, 0), 0), 0); struct _tuple0*
_temp2147= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* _temp2148= Cyc_Absyn_var_exp(
_temp2147, 0); struct Cyc_Absyn_Vardecl* _temp2149= Cyc_Absyn_new_vardecl(
_temp2147, Cyc_Absyn_exn_typ, 0); _temp2148->topt=({ struct Cyc_Core_Opt*
_temp2150=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2150->v=( void*) Cyc_Absyn_exn_typ; _temp2150;});{ struct Cyc_Absyn_Pat*
_temp2151=({ struct Cyc_Absyn_Pat* _temp2167=( struct Cyc_Absyn_Pat*) GC_malloc(
sizeof( struct Cyc_Absyn_Pat)); _temp2167->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp2169=( struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2169[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2170; _temp2170.tag= Cyc_Absyn_Var_p;
_temp2170.f1= _temp2149; _temp2170;}); _temp2169;})); _temp2167->topt=({ struct
Cyc_Core_Opt* _temp2168=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2168->v=( void*) Cyc_Absyn_exn_typ; _temp2168;}); _temp2167->loc= 0;
_temp2167;}); struct Cyc_Absyn_Exp* _temp2152= Cyc_Absyn_throw_exp( _temp2148, 0);
_temp2152->topt=({ struct Cyc_Core_Opt* _temp2153=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2153->v=( void*)(( void*) Cyc_Absyn_VoidType);
_temp2153;});{ struct Cyc_Absyn_Stmt* _temp2154= Cyc_Absyn_exp_stmt( _temp2152,
0); struct Cyc_Absyn_Switch_clause* _temp2155=({ struct Cyc_Absyn_Switch_clause*
_temp2164=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp2164->pattern= _temp2151; _temp2164->pat_vars=({ struct Cyc_Core_Opt*
_temp2165=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2165->v=( void*)({ struct Cyc_List_List* _temp2166=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2166->hd=( void*) _temp2149;
_temp2166->tl= 0; _temp2166;}); _temp2165;}); _temp2164->where_clause= 0;
_temp2164->body= _temp2154; _temp2164->loc= 0; _temp2164;}); struct Cyc_Absyn_Stmt*
_temp2156= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp2077,({ struct Cyc_List_List*
_temp2163=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2163->hd=( void*) _temp2155; _temp2163->tl= 0; _temp2163;})), 0); Cyc_Toc_stmt_to_c(
nv, _temp2156);{ struct Cyc_Absyn_Exp* _temp2157= Cyc_Absyn_fncall_exp( Cyc_Toc_setjmp_e,({
struct Cyc_List_List* _temp2162=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2162->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp2162->tl= 0; _temp2162;}), 0); struct Cyc_Absyn_Stmt*
_temp2158= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( Cyc_Toc__push_handler_e,({
struct Cyc_List_List* _temp2161=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2161->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0);
_temp2161->tl= 0; _temp2161;}), 0), 0); struct Cyc_Absyn_Exp* _temp2159= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 0, 0); struct Cyc_Absyn_Exp* _temp2160= Cyc_Absyn_int_exp((
void*) Cyc_Absyn_Signed, 1, 0);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
h_var, h_typ, 0, Cyc_Absyn_seq_stmt( _temp2158, Cyc_Absyn_declare_stmt(
was_thrown_var, was_thrown_typ,( struct Cyc_Absyn_Exp*) _temp2159, Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt( _temp2157, Cyc_Absyn_assign_stmt( was_thrown_exp,
_temp2160, 0), Cyc_Absyn_skip_stmt( 0), 0), Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, was_thrown_exp, 0), _temp2146, Cyc_Absyn_declare_stmt(
e_var, e_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( e_typ, Cyc_Toc__exn_thrown_e,
0), _temp2156, 0), 0), 0), 0), 0), 0))->r)); return;}}}}} _LL2010: { void*
rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__RegionHandle_sp); void*
rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq); struct
_tuple0* rh_var= Cyc_Toc_temp_var(); struct _tuple0* x_var= _temp2083->name;
struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp( rh_var, 0); struct Cyc_Absyn_Exp*
x_exp= Cyc_Absyn_var_exp( x_var, 0); Cyc_Toc_stmt_to_c( Cyc_Toc_add_varmap( nv,
x_var, x_exp), _temp2081);( void*)( s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt(
rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_fncall_exp( Cyc_Toc__new_region_e,
0, 0), Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp( rh_exp, 0), Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Toc__push_region_e,({ struct Cyc_List_List*(* _temp2171)( struct _tagged_arr)=(
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list; struct Cyc_Absyn_Exp*
_temp2174= x_exp; struct Cyc_Absyn_Exp* _temp2172[ 1u]={ _temp2174}; struct
_tagged_arr _temp2173={( void*) _temp2172,( void*) _temp2172,( void*)( _temp2172
+ 1u)}; _temp2171( _temp2173);}), 0), 0), Cyc_Absyn_seq_stmt( _temp2081, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( Cyc_Toc__pop_region_e,({ struct Cyc_List_List*(* _temp2175)(
struct _tagged_arr)=( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list;
struct Cyc_Absyn_Exp* _temp2178= x_exp; struct Cyc_Absyn_Exp* _temp2176[ 1u]={
_temp2178}; struct _tagged_arr _temp2177={( void*) _temp2176,( void*) _temp2176,(
void*)( _temp2176 + 1u)}; _temp2175( _temp2177);}), 0), 0), 0), 0), 0), 0))->r));
return;} _LL2012:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("switchC_s",
sizeof( unsigned char), 10u)); return; _LL2014:(( int(*)( struct _tagged_arr s))
Cyc_Toc_unimp)( _tag_arr("cut", sizeof( unsigned char), 4u)); return; _LL2016:((
int(*)( struct _tagged_arr s)) Cyc_Toc_unimp)( _tag_arr("splice", sizeof(
unsigned char), 7u)); return; _LL1976:;}} struct _tuple16{ struct _tagged_arr*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl* f){ f->tvs= 0; f->effect= 0; f->rgn_po=
0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c(( void*) f->ret_type));{
struct Cyc_List_List* _temp2179= f->args; for( 0; _temp2179 != 0; _temp2179=((
struct Cyc_List_List*) _check_null( _temp2179))->tl){(*(( struct _tuple16*)((
struct Cyc_List_List*) _check_null( _temp2179))->hd)).f3= Cyc_Toc_typ_to_c((*((
struct _tuple16*)(( struct Cyc_List_List*) _check_null( _temp2179))->hd)).f3);{
struct _tuple0* _temp2180=({ struct _tuple0* _temp2181=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp2181->f1=( void*) Cyc_Absyn_Loc_n;
_temp2181->f2=(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(
_temp2179))->hd)).f1; _temp2181;}); nv= Cyc_Toc_add_varmap( nv, _temp2180, Cyc_Absyn_var_exp(
_temp2180, 0));}}} if(( unsigned int) f->cyc_varargs?(( struct Cyc_Absyn_VarargInfo*)
_check_null( f->cyc_varargs))->name != 0: 0){ int _temp2184; void* _temp2186;
void* _temp2188; struct Cyc_Absyn_Tqual _temp2190; struct Cyc_Core_Opt*
_temp2192; struct Cyc_Absyn_VarargInfo _temp2182=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( f->cyc_varargs)); _LL2193: _temp2192= _temp2182.name; goto _LL2191;
_LL2191: _temp2190= _temp2182.tq; goto _LL2189; _LL2189: _temp2188=( void*)
_temp2182.type; goto _LL2187; _LL2187: _temp2186=( void*) _temp2182.rgn; goto
_LL2185; _LL2185: _temp2184= _temp2182.inject; goto _LL2183; _LL2183: { void*
_temp2194= Cyc_Toc_typ_to_c( Cyc_Absyn_tagged_typ( _temp2188, _temp2186,
_temp2190)); struct _tuple0* _temp2195=({ struct _tuple0* _temp2198=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp2198->f1=( void*) Cyc_Absyn_Loc_n;
_temp2198->f2=( struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null(
_temp2192))->v; _temp2198;}); f->args=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( f->args,({ struct Cyc_List_List*
_temp2196=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2196->hd=( void*)({ struct _tuple16* _temp2197=( struct _tuple16*)
GC_malloc( sizeof( struct _tuple16)); _temp2197->f1=( struct _tagged_arr*)((
struct Cyc_Core_Opt*) _check_null( _temp2192))->v; _temp2197->f2= _temp2190;
_temp2197->f3= _temp2194; _temp2197;}); _temp2196->tl= 0; _temp2196;})); nv= Cyc_Toc_add_varmap(
nv, _temp2195, Cyc_Absyn_var_exp( _temp2195, 0)); f->cyc_varargs= 0;}}{ struct
Cyc_List_List* _temp2199=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( f->param_vardecls))->v; for( 0; _temp2199 != 0; _temp2199=(( struct
Cyc_List_List*) _check_null( _temp2199))->tl){( void*)((( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp2199))->hd)->type=( void*) Cyc_Toc_typ_to_c((
void*)(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
_temp2199))->hd)->type));}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel( nv), f->body);}
static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2200= s; _LL2202: if(
_temp2200 ==( void*) Cyc_Absyn_Abstract){ goto _LL2203;} else{ goto _LL2204;}
_LL2204: if( _temp2200 ==( void*) Cyc_Absyn_ExternC){ goto _LL2205;} else{ goto
_LL2206;} _LL2206: goto _LL2207; _LL2203: return( void*) Cyc_Absyn_Public;
_LL2205: return( void*) Cyc_Absyn_Extern; _LL2207: return s; _LL2201:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple0* _temp2208=( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( s->name))->v; if( Cyc_Toc_structs_so_far == 0){
Cyc_Toc_structs_so_far=({ struct Cyc_Core_Opt* _temp2209=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2209->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2209;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp2210=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(*
_temp2208).f2); if( _temp2210 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_structs_so_far))->v,(* _temp2208).f2, s));} else{ if(((
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Core_Opt*) _check_null( _temp2210))->v)->fields
== 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_structs_so_far))->v,(* _temp2208).f2,
s)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)( s->sc=( void*)((
void*) Cyc_Absyn_Public)); s->tvs= 0; s->name=({ struct Cyc_Core_Opt* _temp2211=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2211->v=(
void*) _temp2208; _temp2211;}); if( s->fields != 0){ if( seen_defn_before){ s->fields=
0;} else{ struct Cyc_List_List* _temp2212=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( s->fields))->v; for( 0; _temp2212 != 0; _temp2212=(( struct Cyc_List_List*)
_check_null( _temp2212))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2212))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2212))->hd)->type));}}}}} struct Cyc_Core_Opt* Cyc_Toc_unions_so_far= 0;
static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl* u){ struct
_tuple0* _temp2213=( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( u->name))->v;
if( Cyc_Toc_unions_so_far == 0){ Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt*
_temp2214=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2214->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2214;});}{ int seen_defn_before; struct Cyc_Core_Opt* _temp2215=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(*
_temp2213).f2); if( _temp2215 == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_unions_so_far))->v,(* _temp2213).f2, u));} else{ if(((
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Core_Opt*) _check_null( _temp2215))->v)->fields
== 0){( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr*
key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_unions_so_far))->v,(* _temp2213).f2,
u)); seen_defn_before= 0;} else{ seen_defn_before= 1;}}( void*)( u->sc=( void*)((
void*) Cyc_Absyn_Public)); u->tvs= 0; u->name=({ struct Cyc_Core_Opt* _temp2216=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2216->v=(
void*) _temp2213; _temp2216;}); if( u->fields != 0){ if( seen_defn_before){ u->fields=
0;} else{ struct Cyc_List_List* _temp2217=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( u->fields))->v; for( 0; _temp2217 != 0; _temp2217=(( struct Cyc_List_List*)
_check_null( _temp2217))->tl){( void*)((( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2217))->hd)->type=( void*) Cyc_Toc_typ_to_c_array((
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2217))->hd)->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static void Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* tud){ if(
Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({ struct Cyc_Core_Opt*
_temp2218=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2218->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Set_empty)( Cyc_Absyn_qvar_cmp); _temp2218;});}{ struct
_tuple0* _temp2219= tud->name; if( tud->fields == 0){ return;} if((( int(*)(
struct Cyc_Set_Set* s, struct _tuple0* elt)) Cyc_Set_member)(( struct Cyc_Set_Set*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v, _temp2219)){
return;}( struct Cyc_Set_Set*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v=(
void*)(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tuple0* elt))
Cyc_Set_insert)(( struct Cyc_Set_Set*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_tunions_so_far))->v,
_temp2219));{ int tag_count= 0; int box_count= 0; struct Cyc_List_List*
_temp2220=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( tud->fields))->v;
for( 0; _temp2220 != 0; _temp2220=(( struct Cyc_List_List*) _check_null(
_temp2220))->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2220))->hd; if( f->typs == 0){ struct
Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp( tag_count
++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name, Cyc_Absyn_sint_t,
eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2221; _temp2221.q_const= 1;
_temp2221.q_volatile= 0; _temp2221.q_restrict= 0; _temp2221;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2222=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2222->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2223=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2223[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2224; _temp2224.tag= Cyc_Absyn_Var_d; _temp2224.f1= vd; _temp2224;});
_temp2223;}), 0); _temp2222->tl= Cyc_Toc_result_decls; _temp2222;});} else{
struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(
box_count ++, 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_sint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual _temp2225; _temp2225.q_const=
1; _temp2225.q_volatile= 0; _temp2225.q_restrict= 0; _temp2225;}); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2226=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2226->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2227=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2227[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2228; _temp2228.tag= Cyc_Absyn_Var_d; _temp2228.f1= vd; _temp2228;});
_temp2227;}), 0); _temp2226->tl= Cyc_Toc_result_decls; _temp2226;});{ struct Cyc_List_List*
_temp2229= 0; int i= 1;{ struct Cyc_List_List* _temp2230= f->typs; for( 0;
_temp2230 != 0;( _temp2230=(( struct Cyc_List_List*) _check_null( _temp2230))->tl,
i ++)){ struct _tagged_arr* _temp2231= Cyc_Toc_fieldname( i); struct Cyc_Absyn_Structfield*
_temp2232=({ struct Cyc_Absyn_Structfield* _temp2234=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2234->name= _temp2231;
_temp2234->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*) _check_null(
_temp2230))->hd)).f1; _temp2234->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2230))->hd)).f2);
_temp2234->width= 0; _temp2234->attributes= 0; _temp2234;}); _temp2229=({ struct
Cyc_List_List* _temp2233=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2233->hd=( void*) _temp2232; _temp2233->tl= _temp2229; _temp2233;});}}
_temp2229=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp2229); _temp2229=({ struct Cyc_List_List* _temp2235=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2235->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2236=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2236->name= Cyc_Toc_tag_sp; _temp2236->tq= Cyc_Toc_mt_tq; _temp2236->type=(
void*) Cyc_Absyn_sint_t; _temp2236->width= 0; _temp2236->attributes= 0;
_temp2236;}); _temp2235->tl= _temp2229; _temp2235;});{ struct Cyc_Absyn_Structdecl*
_temp2237=({ struct Cyc_Absyn_Structdecl* _temp2241=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2241->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2241->name=({ struct Cyc_Core_Opt* _temp2243=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2243->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp2243;}); _temp2241->tvs= 0; _temp2241->fields=({ struct Cyc_Core_Opt*
_temp2242=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2242->v=( void*) _temp2229; _temp2242;}); _temp2241->attributes= 0;
_temp2241;}); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2238=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2238->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2239=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2239[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2240; _temp2240.tag= Cyc_Absyn_Struct_d;
_temp2240.f1= _temp2237; _temp2240;}); _temp2239;}), 0); _temp2238->tl= Cyc_Toc_result_decls;
_temp2238;});}}}}}}} static struct Cyc_Core_Opt* Cyc_Toc_xtunion_fields_so_far=
0; static void Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_Tuniondecl* xd){ if(
Cyc_Toc_xtunion_fields_so_far == 0){ Cyc_Toc_xtunion_fields_so_far=({ struct Cyc_Core_Opt*
_temp2244=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2244->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp2244;});} if( xd->fields
== 0){ return;}{ struct _tuple0* _temp2245= xd->name; struct Cyc_List_List*
_temp2246=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( xd->fields))->v;
for( 0; _temp2246 != 0; _temp2246=(( struct Cyc_List_List*) _check_null(
_temp2246))->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp2246))->hd; struct _tagged_arr* fn=(* f->name).f2;
struct Cyc_Absyn_Exp* _temp2247= Cyc_Absyn_uint_exp( _get_arr_size(* fn, sizeof(
unsigned char)) + 4, 0); struct Cyc_Absyn_ArrayType_struct* _temp2248=({ struct
Cyc_Absyn_ArrayType_struct* _temp2318=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2318[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2319; _temp2319.tag= Cyc_Absyn_ArrayType;
_temp2319.f1=( void*) Cyc_Absyn_uchar_t; _temp2319.f2= Cyc_Toc_mt_tq; _temp2319.f3=(
struct Cyc_Absyn_Exp*) _temp2247; _temp2319;}); _temp2318;}); struct Cyc_Core_Opt*
_temp2249=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tuple0*
key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name); struct Cyc_Core_Opt
_temp2257; int _temp2258; _LL2251: if( _temp2249 == 0){ goto _LL2252;} else{
goto _LL2253;} _LL2253: if( _temp2249 == 0){ goto _LL2255;} else{ _temp2257=*
_temp2249; _LL2259: _temp2258=( int) _temp2257.v; if( _temp2258 == 0){ goto
_LL2254;} else{ goto _LL2255;}} _LL2255: goto _LL2256; _LL2252: { struct Cyc_Absyn_Exp*
initopt= 0; if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){ initopt=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_string_exp(( struct _tagged_arr)({ struct _tagged_arr(* _temp2260)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp2268= _tag_arr("%c%c%c%c%s", sizeof( unsigned char), 11u);
struct Cyc_Stdio_Int_pa_struct _temp2273; _temp2273.tag= Cyc_Stdio_Int_pa;
_temp2273.f1=( unsigned int) 0;{ void* _temp2263=( void*)& _temp2273; struct Cyc_Stdio_Int_pa_struct
_temp2272; _temp2272.tag= Cyc_Stdio_Int_pa; _temp2272.f1=( unsigned int) 0;{
void* _temp2264=( void*)& _temp2272; struct Cyc_Stdio_Int_pa_struct _temp2271;
_temp2271.tag= Cyc_Stdio_Int_pa; _temp2271.f1=( unsigned int) 0;{ void*
_temp2265=( void*)& _temp2271; struct Cyc_Stdio_Int_pa_struct _temp2270;
_temp2270.tag= Cyc_Stdio_Int_pa; _temp2270.f1=( unsigned int) 0;{ void*
_temp2266=( void*)& _temp2270; struct Cyc_Stdio_String_pa_struct _temp2269;
_temp2269.tag= Cyc_Stdio_String_pa; _temp2269.f1=( struct _tagged_arr)* fn;{
void* _temp2267=( void*)& _temp2269; void* _temp2261[ 5u]={ _temp2263, _temp2264,
_temp2265, _temp2266, _temp2267}; struct _tagged_arr _temp2262={( void*)
_temp2261,( void*) _temp2261,( void*)( _temp2261 + 5u)}; _temp2260( _temp2268,
_temp2262);}}}}}}), 0);}{ struct Cyc_Absyn_Vardecl* _temp2274= Cyc_Absyn_new_vardecl(
f->name,( void*) _temp2248, initopt);( void*)( _temp2274->sc=( void*)(( void*) f->sc));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2275=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2275->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp2276=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2276[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2277; _temp2277.tag= Cyc_Absyn_Var_d; _temp2277.f1= _temp2274; _temp2277;});
_temp2276;}), 0); _temp2275->tl= Cyc_Toc_result_decls; _temp2275;});( struct Cyc_Dict_Dict*)(((
struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=( void*)((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Toc_xtunion_fields_so_far))->v, f->name,( void*) f->sc !=( void*) Cyc_Absyn_Extern));
if( f->typs != 0){ struct Cyc_List_List* fields= 0; int i= 1;{ struct Cyc_List_List*
_temp2278= f->typs; for( 0; _temp2278 != 0;( _temp2278=(( struct Cyc_List_List*)
_check_null( _temp2278))->tl, i ++)){ struct _tagged_arr* _temp2279=({ struct
_tagged_arr* _temp2283=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp2283[ 0]=( struct _tagged_arr)({ struct _tagged_arr(*
_temp2284)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp2288= _tag_arr("f%d", sizeof( unsigned char), 4u);
struct Cyc_Stdio_Int_pa_struct _temp2289; _temp2289.tag= Cyc_Stdio_Int_pa;
_temp2289.f1=( unsigned int) i;{ void* _temp2287=( void*)& _temp2289; void*
_temp2285[ 1u]={ _temp2287}; struct _tagged_arr _temp2286={( void*) _temp2285,(
void*) _temp2285,( void*)( _temp2285 + 1u)}; _temp2284( _temp2288, _temp2286);}});
_temp2283;}); struct Cyc_Absyn_Structfield* _temp2280=({ struct Cyc_Absyn_Structfield*
_temp2282=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2282->name= _temp2279; _temp2282->tq=(*(( struct _tuple3*)(( struct Cyc_List_List*)
_check_null( _temp2278))->hd)).f1; _temp2282->type=( void*) Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp2278))->hd)).f2);
_temp2282->width= 0; _temp2282->attributes= 0; _temp2282;}); fields=({ struct
Cyc_List_List* _temp2281=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2281->hd=( void*) _temp2280; _temp2281->tl= fields; _temp2281;});}} fields=({
struct Cyc_List_List* _temp2290=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2290->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2291=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2291->name= Cyc_Toc_tag_sp; _temp2291->tq= Cyc_Toc_mt_tq; _temp2291->type=(
void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t, Cyc_Toc_mt_tq); _temp2291->width=
0; _temp2291->attributes= 0; _temp2291;}); _temp2290->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields); _temp2290;});{ struct Cyc_Absyn_Structdecl*
_temp2292=({ struct Cyc_Absyn_Structdecl* _temp2296=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2296->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2296->name=({ struct Cyc_Core_Opt* _temp2298=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2298->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name, _tag_arr("_struct", sizeof( unsigned char),
8u)); _temp2298;}); _temp2296->tvs= 0; _temp2296->fields=({ struct Cyc_Core_Opt*
_temp2297=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2297->v=( void*) fields; _temp2297;}); _temp2296->attributes= 0; _temp2296;});
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2293=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2293->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2294=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2294[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2295; _temp2295.tag= Cyc_Absyn_Struct_d;
_temp2295.f1= _temp2292; _temp2295;}); _temp2294;}), 0); _temp2293->tl= Cyc_Toc_result_decls;
_temp2293;});}} goto _LL2250;}} _LL2254: if(( void*) f->sc !=( void*) Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp* _temp2299= Cyc_Absyn_string_exp(( struct _tagged_arr)({
struct _tagged_arr(* _temp2304)( struct _tagged_arr fmt, struct _tagged_arr)=
Cyc_Stdio_aprintf; struct _tagged_arr _temp2312= _tag_arr("%c%c%c%c%s", sizeof(
unsigned char), 11u); struct Cyc_Stdio_Int_pa_struct _temp2317; _temp2317.tag=
Cyc_Stdio_Int_pa; _temp2317.f1=( unsigned int) 0;{ void* _temp2307=( void*)&
_temp2317; struct Cyc_Stdio_Int_pa_struct _temp2316; _temp2316.tag= Cyc_Stdio_Int_pa;
_temp2316.f1=( unsigned int) 0;{ void* _temp2308=( void*)& _temp2316; struct Cyc_Stdio_Int_pa_struct
_temp2315; _temp2315.tag= Cyc_Stdio_Int_pa; _temp2315.f1=( unsigned int) 0;{
void* _temp2309=( void*)& _temp2315; struct Cyc_Stdio_Int_pa_struct _temp2314;
_temp2314.tag= Cyc_Stdio_Int_pa; _temp2314.f1=( unsigned int) 0;{ void*
_temp2310=( void*)& _temp2314; struct Cyc_Stdio_String_pa_struct _temp2313;
_temp2313.tag= Cyc_Stdio_String_pa; _temp2313.f1=( struct _tagged_arr)* fn;{
void* _temp2311=( void*)& _temp2313; void* _temp2305[ 5u]={ _temp2307, _temp2308,
_temp2309, _temp2310, _temp2311}; struct _tagged_arr _temp2306={( void*)
_temp2305,( void*) _temp2305,( void*)( _temp2305 + 5u)}; _temp2304( _temp2312,
_temp2306);}}}}}}), 0); struct Cyc_Absyn_Vardecl* _temp2300= Cyc_Absyn_new_vardecl(
f->name,( void*) _temp2248,( struct Cyc_Absyn_Exp*) _temp2299);( void*)(
_temp2300->sc=( void*)(( void*) f->sc)); Cyc_Toc_result_decls=({ struct Cyc_List_List*
_temp2301=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2301->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2302=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2302[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2303; _temp2303.tag= Cyc_Absyn_Var_d;
_temp2303.f1= _temp2300; _temp2303;}); _temp2302;}), 0); _temp2301->tl= Cyc_Toc_result_decls;
_temp2301;});( struct Cyc_Dict_Dict*)((( struct Cyc_Core_Opt*) _check_null( Cyc_Toc_xtunion_fields_so_far))->v=(
void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key,
int data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Toc_xtunion_fields_so_far))->v, f->name, 1));} goto _LL2250;
_LL2256: goto _LL2250; _LL2250:;}}} static void Cyc_Toc_enumdecl_to_c( struct
Cyc_Toc_Env* nv, struct Cyc_Absyn_Enumdecl* ed){ struct _tuple0* _temp2320= ed->name;(
void*)( ed->sc=( void*)(( void*) Cyc_Absyn_Public)); if( ed->fields != 0){
struct Cyc_List_List* _temp2321=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ed->fields))->v; for( 0; _temp2321 != 0; _temp2321=(( struct Cyc_List_List*)
_check_null( _temp2321))->tl){ if((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2321))->hd)->tag != 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null((( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*)
_check_null( _temp2321))->hd)->tag));(( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_List_List*) _check_null( _temp2321))->hd)->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
i, 0);}}}} static void Cyc_Toc_local_decl_to_c( struct Cyc_Toc_Env* body_nv,
struct Cyc_Toc_Env* init_nv, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Stmt*
s){( void*)( vd->type=( void*) Cyc_Toc_typ_to_c_array(( void*) vd->type)); Cyc_Toc_stmt_to_c(
body_nv, s); if( vd->initializer != 0){ struct Cyc_Absyn_Exp* init=( struct Cyc_Absyn_Exp*)
_check_null( vd->initializer); void* _temp2322=( void*) init->r; struct Cyc_Absyn_Exp*
_temp2328; struct Cyc_Absyn_Exp* _temp2330; struct Cyc_Absyn_Vardecl* _temp2332;
_LL2324: if(*(( int*) _temp2322) == Cyc_Absyn_Comprehension_e){ _LL2333:
_temp2332=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp2322)->f1; goto
_LL2331; _LL2331: _temp2330=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2322)->f2; goto _LL2329; _LL2329: _temp2328=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2322)->f3; goto _LL2325;} else{ goto _LL2326;} _LL2326: goto _LL2327;
_LL2325: vd->initializer= 0;( void*)( s->r=( void*)(( void*)( Cyc_Toc_init_comprehension(
init_nv, Cyc_Absyn_var_exp( vd->name, 0), _temp2332, _temp2330, _temp2328, Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r)); goto _LL2323; _LL2327: Cyc_Toc_exp_to_c( init_nv,
init); goto _LL2323; _LL2323:;}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt,
struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)((
struct Cyc_Core_Opt*) _check_null( topt))->v; Cyc_Toc_exp_to_c( nv, e);{ struct
_tuple0* x= Cyc_Toc_temp_var(); struct _tagged_arr* succ_lab= Cyc_Toc_fresh_label();
struct _tagged_arr* fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();{ struct
_RegionHandle _temp2334= _new_region(); struct _RegionHandle* prgn=& _temp2334;
_push_region( prgn);{ struct Cyc_Absyn_Stmt* _temp2337; struct Cyc_List_List*
_temp2339; struct Cyc_Toc_Env* _temp2341; struct _tuple12 _temp2335= Cyc_Toc_xlate_pat(
nv, prgn, t, Cyc_Absyn_var_exp( x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab,
fail_lab, 0); _LL2342: _temp2341= _temp2335.f1; goto _LL2340; _LL2340: _temp2339=
_temp2335.f2; goto _LL2338; _LL2338: _temp2337= _temp2335.f3; goto _LL2336;
_LL2336: Cyc_Toc_stmt_to_c( _temp2341, s);{ struct Cyc_Absyn_Stmt* _temp2343=
Cyc_Absyn_label_stmt( succ_lab, s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt(
x, Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt(
_temp2337, _temp2343, 0), 0);} else{ struct Cyc_Absyn_Exp* _temp2344= Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp( 0)); struct Cyc_Absyn_Stmt* _temp2345= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( _temp2344, 0), 0); s= Cyc_Absyn_declare_stmt( x,
Cyc_Toc_typ_to_c( t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2337,
Cyc_Absyn_seq_stmt( _temp2345, _temp2343, 0), 0), 0);} for( 0; _temp2339 != 0;
_temp2339=(( struct Cyc_List_List*) _check_null( _temp2339))->tl){ struct
_tuple13 _temp2348; void* _temp2349; struct _tuple0* _temp2351; struct _tuple13*
_temp2346=( struct _tuple13*)(( struct Cyc_List_List*) _check_null( _temp2339))->hd;
_temp2348=* _temp2346; _LL2352: _temp2351= _temp2348.f1; goto _LL2350; _LL2350:
_temp2349= _temp2348.f2; goto _LL2347; _LL2347: s= Cyc_Absyn_declare_stmt(
_temp2351, _temp2349, 0, s, 0);}}}; _pop_region( prgn);} return s;}} static
struct Cyc_Toc_Env* Cyc_Toc_decls_to_c( struct Cyc_Toc_Env* nv, struct Cyc_List_List*
ds, int top){ for( 0; ds != 0; ds=(( struct Cyc_List_List*) _check_null( ds))->tl){
if( ! nv->toplevel){(( int(*)( struct _tagged_arr)) Cyc_Tcutil_impos)( _tag_arr("decls_to_c: not at toplevel!",
sizeof( unsigned char), 29u));}{ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( ds))->hd; void* _temp2353=( void*) d->r;
struct Cyc_Absyn_Vardecl* _temp2379; struct Cyc_Absyn_Fndecl* _temp2381; struct
Cyc_Absyn_Structdecl* _temp2383; struct Cyc_Absyn_Uniondecl* _temp2385; struct
Cyc_Absyn_Tuniondecl* _temp2387; struct Cyc_Absyn_Enumdecl* _temp2389; struct
Cyc_Absyn_Typedefdecl* _temp2391; struct Cyc_List_List* _temp2393; struct Cyc_List_List*
_temp2395; struct Cyc_List_List* _temp2397; _LL2355: if(*(( int*) _temp2353) ==
Cyc_Absyn_Var_d){ _LL2380: _temp2379=(( struct Cyc_Absyn_Var_d_struct*)
_temp2353)->f1; goto _LL2356;} else{ goto _LL2357;} _LL2357: if(*(( int*)
_temp2353) == Cyc_Absyn_Fn_d){ _LL2382: _temp2381=(( struct Cyc_Absyn_Fn_d_struct*)
_temp2353)->f1; goto _LL2358;} else{ goto _LL2359;} _LL2359: if(*(( int*)
_temp2353) == Cyc_Absyn_Let_d){ goto _LL2360;} else{ goto _LL2361;} _LL2361: if(*((
int*) _temp2353) == Cyc_Absyn_Letv_d){ goto _LL2362;} else{ goto _LL2363;}
_LL2363: if(*(( int*) _temp2353) == Cyc_Absyn_Struct_d){ _LL2384: _temp2383=((
struct Cyc_Absyn_Struct_d_struct*) _temp2353)->f1; goto _LL2364;} else{ goto
_LL2365;} _LL2365: if(*(( int*) _temp2353) == Cyc_Absyn_Union_d){ _LL2386:
_temp2385=(( struct Cyc_Absyn_Union_d_struct*) _temp2353)->f1; goto _LL2366;}
else{ goto _LL2367;} _LL2367: if(*(( int*) _temp2353) == Cyc_Absyn_Tunion_d){
_LL2388: _temp2387=(( struct Cyc_Absyn_Tunion_d_struct*) _temp2353)->f1; goto
_LL2368;} else{ goto _LL2369;} _LL2369: if(*(( int*) _temp2353) == Cyc_Absyn_Enum_d){
_LL2390: _temp2389=(( struct Cyc_Absyn_Enum_d_struct*) _temp2353)->f1; goto
_LL2370;} else{ goto _LL2371;} _LL2371: if(*(( int*) _temp2353) == Cyc_Absyn_Typedef_d){
_LL2392: _temp2391=(( struct Cyc_Absyn_Typedef_d_struct*) _temp2353)->f1; goto
_LL2372;} else{ goto _LL2373;} _LL2373: if(*(( int*) _temp2353) == Cyc_Absyn_Namespace_d){
_LL2394: _temp2393=(( struct Cyc_Absyn_Namespace_d_struct*) _temp2353)->f2; goto
_LL2374;} else{ goto _LL2375;} _LL2375: if(*(( int*) _temp2353) == Cyc_Absyn_Using_d){
_LL2396: _temp2395=(( struct Cyc_Absyn_Using_d_struct*) _temp2353)->f2; goto
_LL2376;} else{ goto _LL2377;} _LL2377: if(*(( int*) _temp2353) == Cyc_Absyn_ExternC_d){
_LL2398: _temp2397=(( struct Cyc_Absyn_ExternC_d_struct*) _temp2353)->f1; goto
_LL2378;} else{ goto _LL2354;} _LL2356: { struct _tuple0* _temp2399= _temp2379->name;
if(( void*) _temp2379->sc ==( void*) Cyc_Absyn_ExternC){ _temp2399=({ struct
_tuple0* _temp2400=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp2400->f1= Cyc_Absyn_rel_ns_null; _temp2400->f2=(* _temp2399).f2; _temp2400;});}
if( _temp2379->initializer != 0){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)
_check_null( _temp2379->initializer));} nv= Cyc_Toc_add_varmap( nv, _temp2379->name,
Cyc_Absyn_varb_exp( _temp2399,( void*)({ struct Cyc_Absyn_Global_b_struct*
_temp2401=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp2401[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp2402; _temp2402.tag= Cyc_Absyn_Global_b;
_temp2402.f1= _temp2379; _temp2402;}); _temp2401;}), 0)); _temp2379->name=
_temp2399;( void*)( _temp2379->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2379->sc));(
void*)( _temp2379->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2379->type));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2403=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2403->hd=( void*) d; _temp2403->tl=
Cyc_Toc_result_decls; _temp2403;}); goto _LL2354;} _LL2358: { struct _tuple0*
_temp2404= _temp2381->name; if(( void*) _temp2381->sc ==( void*) Cyc_Absyn_ExternC){
_temp2404=({ struct _tuple0* _temp2405=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp2405->f1= Cyc_Absyn_rel_ns_null; _temp2405->f2=(*
_temp2404).f2; _temp2405;});} nv= Cyc_Toc_add_varmap( nv, _temp2381->name, Cyc_Absyn_var_exp(
_temp2404, 0)); _temp2381->name= _temp2404; Cyc_Toc_fndecl_to_c( nv, _temp2381);
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2406=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2406->hd=( void*) d; _temp2406->tl=
Cyc_Toc_result_decls; _temp2406;}); goto _LL2354;} _LL2360: goto _LL2362;
_LL2362:(( int(*)( struct _tagged_arr s)) Cyc_Toc_toc_impos)( _tag_arr("letdecl at toplevel",
sizeof( unsigned char), 20u)); goto _LL2354; _LL2364: Cyc_Toc_structdecl_to_c(
_temp2383); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2407=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2407->hd=( void*) d; _temp2407->tl=
Cyc_Toc_result_decls; _temp2407;}); goto _LL2354; _LL2366: Cyc_Toc_uniondecl_to_c(
_temp2385); Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2408=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2408->hd=( void*) d; _temp2408->tl=
Cyc_Toc_result_decls; _temp2408;}); goto _LL2354; _LL2368: if( _temp2387->is_xtunion){
Cyc_Toc_xtuniondecl_to_c( _temp2387);} else{ Cyc_Toc_tuniondecl_to_c( _temp2387);}
goto _LL2354; _LL2370: Cyc_Toc_enumdecl_to_c( nv, _temp2389); Cyc_Toc_result_decls=({
struct Cyc_List_List* _temp2409=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2409->hd=( void*) d; _temp2409->tl= Cyc_Toc_result_decls;
_temp2409;}); goto _LL2354; _LL2372: _temp2391->name= _temp2391->name; _temp2391->tvs=
0;( void*)( _temp2391->defn=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2391->defn));
Cyc_Toc_result_decls=({ struct Cyc_List_List* _temp2410=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2410->hd=( void*) d; _temp2410->tl=
Cyc_Toc_result_decls; _temp2410;}); goto _LL2354; _LL2374: _temp2395= _temp2393;
goto _LL2376; _LL2376: _temp2397= _temp2395; goto _LL2378; _LL2378: nv= Cyc_Toc_decls_to_c(
nv, _temp2397, top); goto _LL2354; _LL2354:;}} return nv;} static void Cyc_Toc_init(){
Cyc_Toc_result_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_temp_var_counter= 0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_structs_so_far=
0; Cyc_Toc_tunions_so_far= 0; Cyc_Toc_xtunion_fields_so_far= 0;} struct Cyc_List_List*
Cyc_Toc_toc( struct Cyc_List_List* ds){ Cyc_Toc_init(); Cyc_Toc_decls_to_c( Cyc_Toc_empty_env(),
ds, 1); return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}