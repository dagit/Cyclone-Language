 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_dirent{ int d_ino; int d_off; unsigned short
d_reclen; unsigned char d_type; unsigned char d_name[ 256u]; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern int
Cyc_Core_intcmp( int, int); extern unsigned char Cyc_Core_InvalidArg[ 15u];
struct Cyc_Core_InvalidArg_struct{ unsigned char* tag; struct _tagged_arr f1; }
; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern struct _RegionHandle* Cyc_Core_heap_region; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, int); extern
struct _tagged_arr ntCsl_to_ntsl( unsigned char**); extern int system(
unsigned char*); extern int* __errno(); struct Cyc_Stdio___sFILE; extern struct
Cyc_Stdio___sFILE* Cyc_Stdio_stderr; extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*);
extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; static const int Cyc_Stdio_String_pa= 0; struct Cyc_Stdio_String_pa_struct{
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
} ; extern int Cyc_List_length( struct Cyc_List_List* x); extern unsigned char
Cyc_List_List_empty[ 15u]; extern struct Cyc_List_List* Cyc_List_map( void*(* f)(
void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap(
struct _RegionHandle*, void*(* f)( void*), struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_map_c( void*(* f)( void*, void*), void* env,
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap_c( struct
_RegionHandle*, void*(* f)( void*, void*), void* env, struct Cyc_List_List* x);
extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_map2( void*(* f)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y); extern void Cyc_List_iter( void(* f)( void*), struct Cyc_List_List* x);
extern void Cyc_List_iter2( void(* f)( void*, void*), struct Cyc_List_List* x,
struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_revappend(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[
8u]; extern int Cyc_List_exists_c( int(* pred)( void*, void*), void* env, struct
Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_zip( struct Cyc_List_List*
x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_rzip( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y); struct _tuple0{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ;
extern struct _tuple0 Cyc_List_rsplit( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x); extern int Cyc_List_mem( int(*
compare)( void*, void*), struct Cyc_List_List* l, void* x); extern void* Cyc_List_assoc_cmp(
int(* compare)( void*, void*), struct Cyc_List_List* l, void* x); struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file; struct _tagged_arr line; int line_no; int col;
} ; extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment;
extern struct Cyc_List_List* Cyc_Position_strings_of_segments( struct Cyc_List_List*);
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern struct Cyc_Position_Error* Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*, struct _tagged_arr); extern unsigned char Cyc_Position_Nocontext[
14u]; extern void Cyc_Position_post_error( struct Cyc_Position_Error*); struct
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
15u]; extern int Cyc_Absyn_qvar_cmp( struct _tuple1*, struct _tuple1*); extern
int Cyc_Absyn_tvar_cmp( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*); extern
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref( void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern void*
Cyc_Absyn_new_evar( struct Cyc_Core_Opt* k, struct Cyc_Core_Opt* tenv); extern
void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulonglong_t; extern void* Cyc_Absyn_sint_t;
extern void* Cyc_Absyn_slonglong_t; extern void* Cyc_Absyn_float_t; extern void*
Cyc_Absyn_double_t; extern void* Cyc_Absyn_bounds_one; extern void* Cyc_Absyn_atb_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq, void* b); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp(
int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_field( struct Cyc_List_List*, struct _tagged_arr*); extern
struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field( struct Cyc_Absyn_Structdecl*,
struct _tagged_arr*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_union_field(
struct Cyc_Absyn_Uniondecl*, struct _tagged_arr*); struct _tuple4{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; extern struct _tuple4* Cyc_Absyn_lookup_tuple_field( struct
Cyc_List_List*, int); extern struct _tagged_arr Cyc_Absyn_attribute2string( void*);
extern int Cyc_Absyn_fntype_att( void* a); struct Cyc_PP_Ppstate; struct Cyc_PP_Out;
struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int
qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int
rewrite_temp_tvars: 1; int print_all_tvars: 1; int print_all_kinds: 1; int
print_using_stmts: 1; int print_externC_stmts: 1; int print_full_evars: 1; int
use_curr_namespace: 1; struct Cyc_List_List* curr_namespace; } ; extern struct
_tagged_arr Cyc_Absynpp_typ2string( void*); extern struct _tagged_arr Cyc_Absynpp_kind2string(
void*); extern struct _tagged_arr Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref*);
extern struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple1*); struct Cyc_Set_Set;
extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
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
struct _tuple1*); extern struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv*, void* r1, void* r2); extern int Cyc_String_strcmp(
struct _tagged_arr s1, struct _tagged_arr s2); extern int Cyc_String_zstrptrcmp(
struct _tagged_arr*, struct _tagged_arr*); extern struct _tagged_arr Cyc_String_strconcat(
struct _tagged_arr, struct _tagged_arr); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern
void* Cyc_Tcutil_impos( struct _tagged_arr); extern void Cyc_Tcutil_terr( struct
Cyc_Position_Segment*, struct _tagged_arr); extern void Cyc_Tcutil_warn( struct
Cyc_Position_Segment*, struct _tagged_arr); extern void Cyc_Tcutil_flush_warnings();
extern struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set; extern void* Cyc_Tcutil_copy_type(
void* t); extern void* Cyc_Tcutil_tvar_kind( struct Cyc_Absyn_Tvar* t); extern
void* Cyc_Tcutil_typ_kind( void* t); extern void* Cyc_Tcutil_compress( void* t);
extern void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*, void*, struct Cyc_List_List*);
extern int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_coerce_sint_typ( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_coerceable( void*); extern int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void*, void*); extern int
Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void*,
void*); extern int Cyc_Tcutil_is_integral( struct Cyc_Absyn_Exp*); extern int
Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_is_function_type(
void* t); extern void* Cyc_Tcutil_max_arithmetic_type( void*, void*); extern int
Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute( struct Cyc_List_List*,
void*); extern void* Cyc_Tcutil_rsubstitute( struct _RegionHandle*, struct Cyc_List_List*,
void*); extern int Cyc_Tcutil_subset_effect( int set_to_empty, void* e1, void*
e2); extern int Cyc_Tcutil_region_in_effect( int constrain, void* r, void* e);
extern void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*); extern struct Cyc_Absyn_Exp*
Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*, void*, struct Cyc_Position_Segment*);
struct _tuple5{ struct Cyc_Absyn_Tvar* f1; void* f2; } ; extern struct _tuple5*
Cyc_Tcutil_make_inst_var( struct Cyc_List_List*, struct Cyc_Absyn_Tvar*); struct
_tuple6{ struct Cyc_List_List* f1; struct _RegionHandle* f2; } ; extern struct
_tuple5* Cyc_Tcutil_r_make_inst_var( struct _tuple6*, struct Cyc_Absyn_Tvar*);
extern void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp*); extern
void Cyc_Tcutil_check_valid_toplevel_type( struct Cyc_Position_Segment*, struct
Cyc_Tcenv_Tenv*, void*); extern void Cyc_Tcutil_check_fndecl_valid_type( struct
Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Fndecl*); extern
void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, void*); extern void Cyc_Tcutil_check_unique_vars(
struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc, struct _tagged_arr
err_msg); extern void Cyc_Tcutil_check_unique_tvars( struct Cyc_Position_Segment*,
struct Cyc_List_List*); extern void Cyc_Tcutil_check_nonzero_bound( struct Cyc_Position_Segment*,
struct Cyc_Absyn_Conref*); extern void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment*,
unsigned int i, struct Cyc_Absyn_Conref*); extern int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2); extern struct Cyc_List_List*
Cyc_Tcutil_resolve_struct_designators( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields); extern int Cyc_Tcutil_is_tagged_pointer_typ(
void*); extern int Cyc_Tcutil_is_tagged_pointer_typ_elt( void* t, void**
elt_typ_dest); extern void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv*,
void* t, struct Cyc_Absyn_Exp* e); struct _tuple7{ int f1; void* f2; } ; extern
struct _tuple7 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e); extern struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar( void* k); extern int* Cyc_Tcutil_new_tvar_id();
extern void Cyc_Tcutil_add_tvar_identity( struct Cyc_Absyn_Tvar*); extern void
Cyc_Tcutil_add_tvar_identities( struct Cyc_List_List*); extern int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar*);
extern int Cyc_Tcutil_same_atts( struct Cyc_List_List*, struct Cyc_List_List*);
extern int Cyc_Tcutil_bits_only( void* t); extern int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e); extern void* Cyc_Tcutil_snd_tqt(
struct _tuple4*); extern int Cyc_Tcutil_supports_default( void*); extern void*
Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*);
unsigned char Cyc_Tcutil_TypeErr[ 12u]="\000\000\000\000TypeErr"; extern void
Cyc_Tcutil_unify_it( void* t1, void* t2); void Cyc_Tcutil_terr( struct Cyc_Position_Segment*
loc, struct _tagged_arr s){ Cyc_Position_post_error( Cyc_Position_mk_err_elab(
loc, s));} void* Cyc_Tcutil_impos( struct _tagged_arr msg){({ int(* _temp0)(
struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf;
struct Cyc_Stdio___sFILE* _temp4= Cyc_Stdio_stderr; struct _tagged_arr _temp5=
_tag_arr("Error: %s\n", sizeof( unsigned char), 11u); struct Cyc_Stdio_String_pa_struct
_temp6; _temp6.tag= Cyc_Stdio_String_pa; _temp6.f1=( struct _tagged_arr) msg;{
void* _temp3=( void*)& _temp6; void* _temp1[ 1u]={ _temp3}; struct _tagged_arr
_temp2={( void*) _temp1,( void*) _temp1,( void*)( _temp1 + 1u)}; _temp0( _temp4,
_temp5, _temp2);}}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);(
int) _throw(( void*) Cyc_Tcutil_TypeErr);} static struct _tagged_arr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar* tv){ return* tv->name;} void Cyc_Tcutil_print_tvars(
struct Cyc_List_List* tvs){ for( 0; tvs != 0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){({ int(* _temp7)( struct Cyc_Stdio___sFILE*, struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp12= Cyc_Stdio_stderr; struct _tagged_arr _temp13= _tag_arr("%s::%s ",
sizeof( unsigned char), 8u); struct Cyc_Stdio_String_pa_struct _temp15; _temp15.tag=
Cyc_Stdio_String_pa; _temp15.f1=( struct _tagged_arr) Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd);{ void*
_temp10=( void*)& _temp15; struct Cyc_Stdio_String_pa_struct _temp14; _temp14.tag=
Cyc_Stdio_String_pa; _temp14.f1=( struct _tagged_arr) Cyc_Absynpp_ckind2string(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);{
void* _temp11=( void*)& _temp14; void* _temp8[ 2u]={ _temp10, _temp11}; struct
_tagged_arr _temp9={( void*) _temp8,( void*) _temp8,( void*)( _temp8 + 2u)};
_temp7( _temp12, _temp13, _temp9);}}});}({ int(* _temp16)( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp19= Cyc_Stdio_stderr; struct _tagged_arr _temp20= _tag_arr("\n", sizeof(
unsigned char), 2u); void* _temp17[ 0u]={}; struct _tagged_arr _temp18={( void*)
_temp17,( void*) _temp17,( void*)( _temp17 + 0u)}; _temp16( _temp19, _temp20,
_temp18);}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);}
static struct Cyc_List_List* Cyc_Tcutil_warning_segs= 0; static struct Cyc_List_List*
Cyc_Tcutil_warning_msgs= 0; void Cyc_Tcutil_warn( struct Cyc_Position_Segment*
sg, struct _tagged_arr msg){ Cyc_Tcutil_warning_segs=({ struct Cyc_List_List*
_temp21=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp21->hd=( void*) sg; _temp21->tl= Cyc_Tcutil_warning_segs; _temp21;}); Cyc_Tcutil_warning_msgs=({
struct Cyc_List_List* _temp22=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp22->hd=( void*)({ struct _tagged_arr* _temp23=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp23[ 0]= msg; _temp23;});
_temp22->tl= Cyc_Tcutil_warning_msgs; _temp22;});} void Cyc_Tcutil_flush_warnings(){
if( Cyc_Tcutil_warning_segs == 0){ return;}({ int(* _temp24)( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp27= Cyc_Stdio_stderr; struct _tagged_arr _temp28= _tag_arr("***Warnings***\n",
sizeof( unsigned char), 16u); void* _temp25[ 0u]={}; struct _tagged_arr _temp26={(
void*) _temp25,( void*) _temp25,( void*)( _temp25 + 0u)}; _temp24( _temp27,
_temp28, _temp26);});{ struct Cyc_List_List* _temp29= Cyc_Position_strings_of_segments(
Cyc_Tcutil_warning_segs); Cyc_Tcutil_warning_segs= 0; Cyc_Tcutil_warning_msgs=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_Tcutil_warning_msgs);
while( Cyc_Tcutil_warning_msgs != 0) {({ int(* _temp30)( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp35= Cyc_Stdio_stderr; struct _tagged_arr _temp36= _tag_arr("%s: %s\n",
sizeof( unsigned char), 8u); struct Cyc_Stdio_String_pa_struct _temp38; _temp38.tag=
Cyc_Stdio_String_pa; _temp38.f1=( struct _tagged_arr)*(( struct _tagged_arr*)((
struct Cyc_List_List*) _check_null( _temp29))->hd);{ void* _temp33=( void*)&
_temp38; struct Cyc_Stdio_String_pa_struct _temp37; _temp37.tag= Cyc_Stdio_String_pa;
_temp37.f1=( struct _tagged_arr)*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( Cyc_Tcutil_warning_msgs))->hd);{ void* _temp34=( void*)& _temp37;
void* _temp31[ 2u]={ _temp33, _temp34}; struct _tagged_arr _temp32={( void*)
_temp31,( void*) _temp31,( void*)( _temp31 + 2u)}; _temp30( _temp35, _temp36,
_temp32);}}}); _temp29=(( struct Cyc_List_List*) _check_null( _temp29))->tl; Cyc_Tcutil_warning_msgs=((
struct Cyc_List_List*) _check_null( Cyc_Tcutil_warning_msgs))->tl;}({ int(*
_temp39)( struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt, struct _tagged_arr)=
Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp42= Cyc_Stdio_stderr; struct
_tagged_arr _temp43= _tag_arr("**************\n", sizeof( unsigned char), 16u);
void* _temp40[ 0u]={}; struct _tagged_arr _temp41={( void*) _temp40,( void*)
_temp40,( void*)( _temp40 + 0u)}; _temp39( _temp42, _temp43, _temp41);}); Cyc_Stdio_fflush((
struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);}} struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set=
0; static int Cyc_Tcutil_fast_tvar_cmp( struct Cyc_Absyn_Tvar* tv1, struct Cyc_Absyn_Tvar*
tv2){ return*(( int*) _check_null( tv1->identity)) -*(( int*) _check_null( tv2->identity));}
void* Cyc_Tcutil_compress( void* t){ void* _temp44= t; struct Cyc_Core_Opt*
_temp56; struct Cyc_Core_Opt* _temp58; struct Cyc_Core_Opt** _temp60; struct Cyc_Core_Opt*
_temp61; struct Cyc_Core_Opt* _temp63; struct Cyc_Core_Opt** _temp65; _LL46: if((
unsigned int) _temp44 > 4u?*(( int*) _temp44) == Cyc_Absyn_Evar: 0){ _LL57:
_temp56=(( struct Cyc_Absyn_Evar_struct*) _temp44)->f2; if( _temp56 == 0){ goto
_LL47;} else{ goto _LL48;}} else{ goto _LL48;} _LL48: if(( unsigned int) _temp44
> 4u?*(( int*) _temp44) == Cyc_Absyn_Evar: 0){ _LL59: _temp58=(( struct Cyc_Absyn_Evar_struct*)
_temp44)->f2; _temp60=&(( struct Cyc_Absyn_Evar_struct*) _temp44)->f2; goto
_LL49;} else{ goto _LL50;} _LL50: if(( unsigned int) _temp44 > 4u?*(( int*)
_temp44) == Cyc_Absyn_TypedefType: 0){ _LL62: _temp61=(( struct Cyc_Absyn_TypedefType_struct*)
_temp44)->f3; if( _temp61 == 0){ goto _LL51;} else{ goto _LL52;}} else{ goto
_LL52;} _LL52: if(( unsigned int) _temp44 > 4u?*(( int*) _temp44) == Cyc_Absyn_TypedefType:
0){ _LL64: _temp63=(( struct Cyc_Absyn_TypedefType_struct*) _temp44)->f3;
_temp65=&(( struct Cyc_Absyn_TypedefType_struct*) _temp44)->f3; goto _LL53;}
else{ goto _LL54;} _LL54: goto _LL55; _LL47: return t; _LL49: { void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null(* _temp60))->v); if( t2 !=( void*)((
struct Cyc_Core_Opt*) _check_null(* _temp60))->v){* _temp60=({ struct Cyc_Core_Opt*
_temp66=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp66->v=( void*) t2; _temp66;});} return t2;} _LL51: return t; _LL53: { void*
t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(* _temp65))->v);
if( t2 !=( void*)(( struct Cyc_Core_Opt*) _check_null(* _temp65))->v){* _temp65=({
struct Cyc_Core_Opt* _temp67=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp67->v=( void*) t2; _temp67;});} return t2;} _LL55: return t;
_LL45:;} void* Cyc_Tcutil_copy_type( void* t); static struct Cyc_List_List* Cyc_Tcutil_copy_types(
struct Cyc_List_List* ts){ return Cyc_List_map( Cyc_Tcutil_copy_type, ts);}
static struct Cyc_Absyn_Conref* Cyc_Tcutil_copy_conref( struct Cyc_Absyn_Conref*
c){ void* _temp68=( void*) c->v; void* _temp76; struct Cyc_Absyn_Conref* _temp78;
_LL70: if( _temp68 ==( void*) Cyc_Absyn_No_constr){ goto _LL71;} else{ goto
_LL72;} _LL72: if(( unsigned int) _temp68 > 1u?*(( int*) _temp68) == Cyc_Absyn_Eq_constr:
0){ _LL77: _temp76=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp68)->f1;
goto _LL73;} else{ goto _LL74;} _LL74: if(( unsigned int) _temp68 > 1u?*(( int*)
_temp68) == Cyc_Absyn_Forward_constr: 0){ _LL79: _temp78=(( struct Cyc_Absyn_Forward_constr_struct*)
_temp68)->f1; goto _LL75;} else{ goto _LL69;} _LL71: return Cyc_Absyn_empty_conref();
_LL73: return Cyc_Absyn_new_conref( _temp76); _LL75: return Cyc_Tcutil_copy_conref(
_temp78); _LL69:;} static struct Cyc_Absyn_Tvar* Cyc_Tcutil_copy_tvar( struct
Cyc_Absyn_Tvar* tv){ return({ struct Cyc_Absyn_Tvar* _temp80=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp80->name= tv->name; _temp80->identity=
0; _temp80->kind= Cyc_Tcutil_copy_conref( tv->kind); _temp80;});} static struct
_tuple2* Cyc_Tcutil_copy_arg( struct _tuple2* arg){ void* _temp83; struct Cyc_Absyn_Tqual
_temp85; struct Cyc_Core_Opt* _temp87; struct _tuple2 _temp81=* arg; _LL88:
_temp87= _temp81.f1; goto _LL86; _LL86: _temp85= _temp81.f2; goto _LL84; _LL84:
_temp83= _temp81.f3; goto _LL82; _LL82: return({ struct _tuple2* _temp89=(
struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp89->f1= _temp87;
_temp89->f2= _temp85; _temp89->f3= Cyc_Tcutil_copy_type( _temp83); _temp89;});}
static struct _tuple4* Cyc_Tcutil_copy_tqt( struct _tuple4* arg){ void* _temp92;
struct Cyc_Absyn_Tqual _temp94; struct _tuple4 _temp90=* arg; _LL95: _temp94=
_temp90.f1; goto _LL93; _LL93: _temp92= _temp90.f2; goto _LL91; _LL91: return({
struct _tuple4* _temp96=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp96->f1= _temp94; _temp96->f2= Cyc_Tcutil_copy_type( _temp92); _temp96;});}
static struct Cyc_Absyn_Structfield* Cyc_Tcutil_copy_field( struct Cyc_Absyn_Structfield*
f){ return({ struct Cyc_Absyn_Structfield* _temp97=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp97->name= f->name;
_temp97->tq= f->tq; _temp97->type=( void*) Cyc_Tcutil_copy_type(( void*) f->type);
_temp97->width= f->width; _temp97->attributes= f->attributes; _temp97;});}
struct _tuple8{ void* f1; void* f2; } ; static struct _tuple8* Cyc_Tcutil_copy_rgncmp(
struct _tuple8* x){ struct _tuple8 _temp100; void* _temp101; void* _temp103;
struct _tuple8* _temp98= x; _temp100=* _temp98; _LL104: _temp103= _temp100.f1;
goto _LL102; _LL102: _temp101= _temp100.f2; goto _LL99; _LL99: return({ struct
_tuple8* _temp105=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8));
_temp105->f1= Cyc_Tcutil_copy_type( _temp103); _temp105->f2= Cyc_Tcutil_copy_type(
_temp101); _temp105;});} void* Cyc_Tcutil_copy_type( void* t){ void* _temp106=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar* _temp152; struct Cyc_Absyn_TunionInfo
_temp154; void* _temp156; struct Cyc_List_List* _temp158; void* _temp160; struct
Cyc_Absyn_TunionFieldInfo _temp162; struct Cyc_List_List* _temp164; void*
_temp166; struct Cyc_Absyn_PtrInfo _temp168; struct Cyc_Absyn_Conref* _temp170;
struct Cyc_Absyn_Tqual _temp172; struct Cyc_Absyn_Conref* _temp174; void*
_temp176; void* _temp178; void* _temp180; void* _temp182; struct Cyc_Absyn_Exp*
_temp184; struct Cyc_Absyn_Tqual _temp186; void* _temp188; struct Cyc_Absyn_FnInfo
_temp190; struct Cyc_List_List* _temp192; struct Cyc_List_List* _temp194; struct
Cyc_Absyn_VarargInfo* _temp196; int _temp198; struct Cyc_List_List* _temp200;
void* _temp202; struct Cyc_Core_Opt* _temp204; struct Cyc_List_List* _temp206;
struct Cyc_List_List* _temp208; struct Cyc_List_List* _temp210; struct _tuple1*
_temp212; struct Cyc_List_List* _temp214; struct _tuple1* _temp216; struct Cyc_List_List*
_temp218; struct Cyc_List_List* _temp220; struct _tuple1* _temp222; void*
_temp224; struct Cyc_List_List* _temp226; struct _tuple1* _temp228; void*
_temp230; struct Cyc_List_List* _temp232; _LL108: if( _temp106 ==( void*) Cyc_Absyn_VoidType){
goto _LL109;} else{ goto _LL110;} _LL110: if(( unsigned int) _temp106 > 4u?*((
int*) _temp106) == Cyc_Absyn_Evar: 0){ goto _LL111;} else{ goto _LL112;} _LL112:
if(( unsigned int) _temp106 > 4u?*(( int*) _temp106) == Cyc_Absyn_VarType: 0){
_LL153: _temp152=(( struct Cyc_Absyn_VarType_struct*) _temp106)->f1; goto _LL113;}
else{ goto _LL114;} _LL114: if(( unsigned int) _temp106 > 4u?*(( int*) _temp106)
== Cyc_Absyn_TunionType: 0){ _LL155: _temp154=(( struct Cyc_Absyn_TunionType_struct*)
_temp106)->f1; _LL161: _temp160=( void*) _temp154.tunion_info; goto _LL159;
_LL159: _temp158= _temp154.targs; goto _LL157; _LL157: _temp156=( void*)
_temp154.rgn; goto _LL115;} else{ goto _LL116;} _LL116: if(( unsigned int)
_temp106 > 4u?*(( int*) _temp106) == Cyc_Absyn_TunionFieldType: 0){ _LL163:
_temp162=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp106)->f1; _LL167:
_temp166=( void*) _temp162.field_info; goto _LL165; _LL165: _temp164= _temp162.targs;
goto _LL117;} else{ goto _LL118;} _LL118: if(( unsigned int) _temp106 > 4u?*((
int*) _temp106) == Cyc_Absyn_PointerType: 0){ _LL169: _temp168=(( struct Cyc_Absyn_PointerType_struct*)
_temp106)->f1; _LL179: _temp178=( void*) _temp168.elt_typ; goto _LL177; _LL177:
_temp176=( void*) _temp168.rgn_typ; goto _LL175; _LL175: _temp174= _temp168.nullable;
goto _LL173; _LL173: _temp172= _temp168.tq; goto _LL171; _LL171: _temp170=
_temp168.bounds; goto _LL119;} else{ goto _LL120;} _LL120: if(( unsigned int)
_temp106 > 4u?*(( int*) _temp106) == Cyc_Absyn_IntType: 0){ _LL183: _temp182=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp106)->f1; goto _LL181; _LL181:
_temp180=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp106)->f2; goto _LL121;}
else{ goto _LL122;} _LL122: if( _temp106 ==( void*) Cyc_Absyn_FloatType){ goto
_LL123;} else{ goto _LL124;} _LL124: if( _temp106 ==( void*) Cyc_Absyn_DoubleType){
goto _LL125;} else{ goto _LL126;} _LL126: if(( unsigned int) _temp106 > 4u?*((
int*) _temp106) == Cyc_Absyn_ArrayType: 0){ _LL189: _temp188=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp106)->f1; goto _LL187; _LL187: _temp186=((
struct Cyc_Absyn_ArrayType_struct*) _temp106)->f2; goto _LL185; _LL185: _temp184=((
struct Cyc_Absyn_ArrayType_struct*) _temp106)->f3; goto _LL127;} else{ goto
_LL128;} _LL128: if(( unsigned int) _temp106 > 4u?*(( int*) _temp106) == Cyc_Absyn_FnType:
0){ _LL191: _temp190=(( struct Cyc_Absyn_FnType_struct*) _temp106)->f1; _LL207:
_temp206= _temp190.tvars; goto _LL205; _LL205: _temp204= _temp190.effect; goto
_LL203; _LL203: _temp202=( void*) _temp190.ret_typ; goto _LL201; _LL201:
_temp200= _temp190.args; goto _LL199; _LL199: _temp198= _temp190.c_varargs; goto
_LL197; _LL197: _temp196= _temp190.cyc_varargs; goto _LL195; _LL195: _temp194=
_temp190.rgn_po; goto _LL193; _LL193: _temp192= _temp190.attributes; goto _LL129;}
else{ goto _LL130;} _LL130: if(( unsigned int) _temp106 > 4u?*(( int*) _temp106)
== Cyc_Absyn_TupleType: 0){ _LL209: _temp208=(( struct Cyc_Absyn_TupleType_struct*)
_temp106)->f1; goto _LL131;} else{ goto _LL132;} _LL132: if(( unsigned int)
_temp106 > 4u?*(( int*) _temp106) == Cyc_Absyn_StructType: 0){ _LL213: _temp212=((
struct Cyc_Absyn_StructType_struct*) _temp106)->f1; goto _LL211; _LL211:
_temp210=(( struct Cyc_Absyn_StructType_struct*) _temp106)->f2; goto _LL133;}
else{ goto _LL134;} _LL134: if(( unsigned int) _temp106 > 4u?*(( int*) _temp106)
== Cyc_Absyn_UnionType: 0){ _LL217: _temp216=(( struct Cyc_Absyn_UnionType_struct*)
_temp106)->f1; goto _LL215; _LL215: _temp214=(( struct Cyc_Absyn_UnionType_struct*)
_temp106)->f2; goto _LL135;} else{ goto _LL136;} _LL136: if(( unsigned int)
_temp106 > 4u?*(( int*) _temp106) == Cyc_Absyn_AnonStructType: 0){ _LL219:
_temp218=(( struct Cyc_Absyn_AnonStructType_struct*) _temp106)->f1; goto _LL137;}
else{ goto _LL138;} _LL138: if(( unsigned int) _temp106 > 4u?*(( int*) _temp106)
== Cyc_Absyn_AnonUnionType: 0){ _LL221: _temp220=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp106)->f1; goto _LL139;} else{ goto _LL140;} _LL140: if(( unsigned int)
_temp106 > 4u?*(( int*) _temp106) == Cyc_Absyn_EnumType: 0){ _LL223: _temp222=((
struct Cyc_Absyn_EnumType_struct*) _temp106)->f1; goto _LL141;} else{ goto
_LL142;} _LL142: if(( unsigned int) _temp106 > 4u?*(( int*) _temp106) == Cyc_Absyn_RgnHandleType:
0){ _LL225: _temp224=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp106)->f1;
goto _LL143;} else{ goto _LL144;} _LL144: if(( unsigned int) _temp106 > 4u?*((
int*) _temp106) == Cyc_Absyn_TypedefType: 0){ _LL229: _temp228=(( struct Cyc_Absyn_TypedefType_struct*)
_temp106)->f1; goto _LL227; _LL227: _temp226=(( struct Cyc_Absyn_TypedefType_struct*)
_temp106)->f2; goto _LL145;} else{ goto _LL146;} _LL146: if( _temp106 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL147;} else{ goto _LL148;} _LL148: if(( unsigned int)
_temp106 > 4u?*(( int*) _temp106) == Cyc_Absyn_AccessEff: 0){ _LL231: _temp230=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp106)->f1; goto _LL149;} else{
goto _LL150;} _LL150: if(( unsigned int) _temp106 > 4u?*(( int*) _temp106) ==
Cyc_Absyn_JoinEff: 0){ _LL233: _temp232=(( struct Cyc_Absyn_JoinEff_struct*)
_temp106)->f1; goto _LL151;} else{ goto _LL107;} _LL109: return t; _LL111:
return t; _LL113: return( void*)({ struct Cyc_Absyn_VarType_struct* _temp234=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp234[ 0]=({ struct Cyc_Absyn_VarType_struct _temp235; _temp235.tag= Cyc_Absyn_VarType;
_temp235.f1= Cyc_Tcutil_copy_tvar( _temp152); _temp235;}); _temp234;}); _LL115:
return( void*)({ struct Cyc_Absyn_TunionType_struct* _temp236=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp236[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp237; _temp237.tag= Cyc_Absyn_TunionType;
_temp237.f1=({ struct Cyc_Absyn_TunionInfo _temp238; _temp238.tunion_info=( void*)
_temp160; _temp238.targs= Cyc_Tcutil_copy_types( _temp158); _temp238.rgn=( void*)
Cyc_Tcutil_copy_type( _temp156); _temp238;}); _temp237;}); _temp236;}); _LL117:
return( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp239=( struct Cyc_Absyn_TunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp239[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp240; _temp240.tag= Cyc_Absyn_TunionFieldType;
_temp240.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp241; _temp241.field_info=(
void*) _temp166; _temp241.targs= Cyc_Tcutil_copy_types( _temp164); _temp241;});
_temp240;}); _temp239;}); _LL119: { void* _temp242= Cyc_Tcutil_copy_type(
_temp178); void* _temp243= Cyc_Tcutil_copy_type( _temp176); struct Cyc_Absyn_Conref*
_temp244=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* c)) Cyc_Tcutil_copy_conref)(
_temp174); struct Cyc_Absyn_Tqual _temp245= _temp172; struct Cyc_Absyn_Conref*
_temp246= Cyc_Tcutil_copy_conref( _temp170); return( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp247=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp247[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp248; _temp248.tag= Cyc_Absyn_PointerType;
_temp248.f1=({ struct Cyc_Absyn_PtrInfo _temp249; _temp249.elt_typ=( void*)
_temp242; _temp249.rgn_typ=( void*) _temp243; _temp249.nullable= _temp244;
_temp249.tq= _temp245; _temp249.bounds= _temp246; _temp249;}); _temp248;});
_temp247;});} _LL121: return( void*)({ struct Cyc_Absyn_IntType_struct* _temp250=(
struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp250[ 0]=({ struct Cyc_Absyn_IntType_struct _temp251; _temp251.tag= Cyc_Absyn_IntType;
_temp251.f1=( void*) _temp182; _temp251.f2=( void*) _temp180; _temp251;});
_temp250;}); _LL123: goto _LL125; _LL125: return t; _LL127: return( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp252=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp252[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp253; _temp253.tag= Cyc_Absyn_ArrayType; _temp253.f1=(
void*) Cyc_Tcutil_copy_type( _temp188); _temp253.f2= _temp186; _temp253.f3=
_temp184; _temp253;}); _temp252;}); _LL129: { struct Cyc_List_List* _temp254=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_tvar, _temp206); struct
Cyc_Core_Opt* _temp255= _temp204 == 0? 0:({ struct Cyc_Core_Opt* _temp265=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp265->v=(
void*) Cyc_Tcutil_copy_type(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp204))->v); _temp265;}); void* _temp256= Cyc_Tcutil_copy_type( _temp202);
struct Cyc_List_List* _temp257=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)(
struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_arg,
_temp200); int _temp258= _temp198; struct Cyc_Absyn_VarargInfo* cyc_varargs2= 0;
if( _temp196 != 0){ struct Cyc_Absyn_VarargInfo* cv=( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp196); cyc_varargs2=({ struct Cyc_Absyn_VarargInfo* _temp259=(
struct Cyc_Absyn_VarargInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp259->name= cv->name; _temp259->tq= cv->tq; _temp259->type=( void*) Cyc_Tcutil_copy_type((
void*) cv->type); _temp259->rgn=( void*) Cyc_Tcutil_copy_type(( void*) cv->rgn);
_temp259->inject= cv->inject; _temp259;});}{ struct Cyc_List_List* _temp260=((
struct Cyc_List_List*(*)( struct _tuple8*(* f)( struct _tuple8*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_copy_rgncmp, _temp194); struct Cyc_List_List*
_temp261= _temp192; return( void*)({ struct Cyc_Absyn_FnType_struct* _temp262=(
struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp262[ 0]=({ struct Cyc_Absyn_FnType_struct _temp263; _temp263.tag= Cyc_Absyn_FnType;
_temp263.f1=({ struct Cyc_Absyn_FnInfo _temp264; _temp264.tvars= _temp254;
_temp264.effect= _temp255; _temp264.ret_typ=( void*) _temp256; _temp264.args=
_temp257; _temp264.c_varargs= _temp258; _temp264.cyc_varargs= cyc_varargs2;
_temp264.rgn_po= _temp260; _temp264.attributes= _temp261; _temp264;}); _temp263;});
_temp262;});}} _LL131: return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp266=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp266[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp267; _temp267.tag= Cyc_Absyn_TupleType;
_temp267.f1=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_tqt, _temp208);
_temp267;}); _temp266;}); _LL133: return( void*)({ struct Cyc_Absyn_StructType_struct*
_temp268=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp268[ 0]=({ struct Cyc_Absyn_StructType_struct _temp269; _temp269.tag= Cyc_Absyn_StructType;
_temp269.f1= _temp212; _temp269.f2= Cyc_Tcutil_copy_types( _temp210); _temp269.f3=
0; _temp269;}); _temp268;}); _LL135: return( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp270=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp270[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp271; _temp271.tag= Cyc_Absyn_UnionType;
_temp271.f1= _temp216; _temp271.f2= Cyc_Tcutil_copy_types( _temp214); _temp271.f3=
0; _temp271;}); _temp270;}); _LL137: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp272=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp272[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp273; _temp273.tag= Cyc_Absyn_AnonStructType; _temp273.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_copy_field, _temp218); _temp273;}); _temp272;});
_LL139: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp274=(
struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp274[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp275; _temp275.tag=
Cyc_Absyn_AnonUnionType; _temp275.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_field,
_temp220); _temp275;}); _temp274;}); _LL141: return( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp276=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp276[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp277; _temp277.tag= Cyc_Absyn_EnumType;
_temp277.f1= _temp222; _temp277.f2= 0; _temp277;}); _temp276;}); _LL143: return(
void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp278=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp278[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp279; _temp279.tag= Cyc_Absyn_RgnHandleType;
_temp279.f1=( void*) Cyc_Tcutil_copy_type( _temp224); _temp279;}); _temp278;});
_LL145: return( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp280=( struct
Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp280[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp281; _temp281.tag= Cyc_Absyn_TypedefType;
_temp281.f1= _temp228; _temp281.f2= Cyc_Tcutil_copy_types( _temp226); _temp281.f3=
0; _temp281;}); _temp280;}); _LL147: return t; _LL149: return( void*)({ struct
Cyc_Absyn_AccessEff_struct* _temp282=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp282[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp283; _temp283.tag= Cyc_Absyn_AccessEff; _temp283.f1=(
void*) Cyc_Tcutil_copy_type( _temp230); _temp283;}); _temp282;}); _LL151: return(
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp284=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp284[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp285; _temp285.tag= Cyc_Absyn_JoinEff; _temp285.f1= Cyc_Tcutil_copy_types(
_temp232); _temp285;}); _temp284;}); _LL107:;} static int Cyc_Tcutil_kind_leq(
void* k1, void* k2){ if( k1 == k2){ return 1;}{ struct _tuple8 _temp287=({
struct _tuple8 _temp286; _temp286.f1= k1; _temp286.f2= k2; _temp286;}); void*
_temp297; void* _temp299; void* _temp301; void* _temp303; void* _temp305; void*
_temp307; _LL289: _LL300: _temp299= _temp287.f1; if( _temp299 ==( void*) Cyc_Absyn_BoxKind){
goto _LL298;} else{ goto _LL291;} _LL298: _temp297= _temp287.f2; if( _temp297 ==(
void*) Cyc_Absyn_MemKind){ goto _LL290;} else{ goto _LL291;} _LL291: _LL304:
_temp303= _temp287.f1; if( _temp303 ==( void*) Cyc_Absyn_BoxKind){ goto _LL302;}
else{ goto _LL293;} _LL302: _temp301= _temp287.f2; if( _temp301 ==( void*) Cyc_Absyn_AnyKind){
goto _LL292;} else{ goto _LL293;} _LL293: _LL308: _temp307= _temp287.f1; if(
_temp307 ==( void*) Cyc_Absyn_MemKind){ goto _LL306;} else{ goto _LL295;} _LL306:
_temp305= _temp287.f2; if( _temp305 ==( void*) Cyc_Absyn_AnyKind){ goto _LL294;}
else{ goto _LL295;} _LL295: goto _LL296; _LL290: return 1; _LL292: return 1;
_LL294: return 1; _LL296: return 0; _LL288:;}} void* Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar* tv){ return Cyc_Absyn_conref_val( tv->kind);} void* Cyc_Tcutil_typ_kind(
void* t){ void* _temp309= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt* _temp361;
struct Cyc_Core_Opt* _temp363; struct Cyc_Absyn_Tvar* _temp365; void* _temp367;
struct Cyc_Absyn_TunionFieldInfo _temp369; void* _temp371; struct Cyc_Absyn_Tunionfield*
_temp373; struct Cyc_Absyn_TunionFieldInfo _temp375; void* _temp377; struct Cyc_Absyn_Structdecl**
_temp379; struct Cyc_Absyn_Uniondecl** _temp381; struct Cyc_Absyn_PtrInfo
_temp383; void* _temp385; struct Cyc_List_List* _temp387; struct Cyc_Core_Opt*
_temp389; _LL311: if(( unsigned int) _temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_Evar:
0){ _LL364: _temp363=(( struct Cyc_Absyn_Evar_struct*) _temp309)->f1; goto
_LL362; _LL362: _temp361=(( struct Cyc_Absyn_Evar_struct*) _temp309)->f2; goto
_LL312;} else{ goto _LL313;} _LL313: if(( unsigned int) _temp309 > 4u?*(( int*)
_temp309) == Cyc_Absyn_VarType: 0){ _LL366: _temp365=(( struct Cyc_Absyn_VarType_struct*)
_temp309)->f1; goto _LL314;} else{ goto _LL315;} _LL315: if( _temp309 ==( void*)
Cyc_Absyn_VoidType){ goto _LL316;} else{ goto _LL317;} _LL317: if(( unsigned int)
_temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_IntType: 0){ _LL368: _temp367=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp309)->f2; goto _LL318;} else{
goto _LL319;} _LL319: if( _temp309 ==( void*) Cyc_Absyn_FloatType){ goto _LL320;}
else{ goto _LL321;} _LL321: if( _temp309 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL322;} else{ goto _LL323;} _LL323: if(( unsigned int) _temp309 > 4u?*(( int*)
_temp309) == Cyc_Absyn_FnType: 0){ goto _LL324;} else{ goto _LL325;} _LL325: if((
unsigned int) _temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_RgnHandleType: 0){
goto _LL326;} else{ goto _LL327;} _LL327: if( _temp309 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL328;} else{ goto _LL329;} _LL329: if(( unsigned int) _temp309 > 4u?*((
int*) _temp309) == Cyc_Absyn_TunionType: 0){ goto _LL330;} else{ goto _LL331;}
_LL331: if(( unsigned int) _temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_TunionFieldType:
0){ _LL370: _temp369=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp309)->f1;
_LL372: _temp371=( void*) _temp369.field_info; if(*(( int*) _temp371) == Cyc_Absyn_KnownTunionfield){
_LL374: _temp373=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp371)->f2;
goto _LL332;} else{ goto _LL333;}} else{ goto _LL333;} _LL333: if(( unsigned int)
_temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_TunionFieldType: 0){ _LL376:
_temp375=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp309)->f1; _LL378:
_temp377=( void*) _temp375.field_info; if(*(( int*) _temp377) == Cyc_Absyn_UnknownTunionfield){
goto _LL334;} else{ goto _LL335;}} else{ goto _LL335;} _LL335: if(( unsigned int)
_temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_StructType: 0){ _LL380: _temp379=((
struct Cyc_Absyn_StructType_struct*) _temp309)->f3; if( _temp379 == 0){ goto
_LL336;} else{ goto _LL337;}} else{ goto _LL337;} _LL337: if(( unsigned int)
_temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_UnionType: 0){ _LL382: _temp381=((
struct Cyc_Absyn_UnionType_struct*) _temp309)->f3; if( _temp381 == 0){ goto
_LL338;} else{ goto _LL339;}} else{ goto _LL339;} _LL339: if(( unsigned int)
_temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_StructType: 0){ goto _LL340;}
else{ goto _LL341;} _LL341: if(( unsigned int) _temp309 > 4u?*(( int*) _temp309)
== Cyc_Absyn_UnionType: 0){ goto _LL342;} else{ goto _LL343;} _LL343: if((
unsigned int) _temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_AnonStructType: 0){
goto _LL344;} else{ goto _LL345;} _LL345: if(( unsigned int) _temp309 > 4u?*((
int*) _temp309) == Cyc_Absyn_AnonUnionType: 0){ goto _LL346;} else{ goto _LL347;}
_LL347: if(( unsigned int) _temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_EnumType:
0){ goto _LL348;} else{ goto _LL349;} _LL349: if(( unsigned int) _temp309 > 4u?*((
int*) _temp309) == Cyc_Absyn_PointerType: 0){ _LL384: _temp383=(( struct Cyc_Absyn_PointerType_struct*)
_temp309)->f1; goto _LL350;} else{ goto _LL351;} _LL351: if(( unsigned int)
_temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_ArrayType: 0){ _LL386: _temp385=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp309)->f1; goto _LL352;} else{
goto _LL353;} _LL353: if(( unsigned int) _temp309 > 4u?*(( int*) _temp309) ==
Cyc_Absyn_TupleType: 0){ _LL388: _temp387=(( struct Cyc_Absyn_TupleType_struct*)
_temp309)->f1; goto _LL354;} else{ goto _LL355;} _LL355: if(( unsigned int)
_temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_TypedefType: 0){ _LL390: _temp389=((
struct Cyc_Absyn_TypedefType_struct*) _temp309)->f3; goto _LL356;} else{ goto
_LL357;} _LL357: if(( unsigned int) _temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_AccessEff:
0){ goto _LL358;} else{ goto _LL359;} _LL359: if(( unsigned int) _temp309 > 4u?*((
int*) _temp309) == Cyc_Absyn_JoinEff: 0){ goto _LL360;} else{ goto _LL310;}
_LL312: return( void*)(( struct Cyc_Core_Opt*) _check_null( _temp363))->v;
_LL314: return Cyc_Absyn_conref_val( _temp365->kind); _LL316: return( void*) Cyc_Absyn_MemKind;
_LL318: return _temp367 ==( void*) Cyc_Absyn_B4?( void*) Cyc_Absyn_BoxKind:(
void*) Cyc_Absyn_MemKind; _LL320: return( void*) Cyc_Absyn_MemKind; _LL322:
return( void*) Cyc_Absyn_MemKind; _LL324: return( void*) Cyc_Absyn_MemKind;
_LL326: return( void*) Cyc_Absyn_BoxKind; _LL328: return( void*) Cyc_Absyn_RgnKind;
_LL330: return( void*) Cyc_Absyn_BoxKind; _LL332: if( _temp373->typs == 0){
return( void*) Cyc_Absyn_BoxKind;} else{ return( void*) Cyc_Absyn_MemKind;}
_LL334: return Cyc_Tcutil_impos( _tag_arr("typ_kind: Unresolved TunionFieldType",
sizeof( unsigned char), 37u)); _LL336: return( void*) Cyc_Absyn_AnyKind; _LL338:
return( void*) Cyc_Absyn_AnyKind; _LL340: return( void*) Cyc_Absyn_MemKind;
_LL342: return( void*) Cyc_Absyn_MemKind; _LL344: return( void*) Cyc_Absyn_MemKind;
_LL346: return( void*) Cyc_Absyn_MemKind; _LL348: return( void*) Cyc_Absyn_BoxKind;
_LL350: { void* _temp391=( void*)( Cyc_Absyn_compress_conref( _temp383.bounds))->v;
void* _temp401; void* _temp403; _LL393: if(( unsigned int) _temp391 > 1u?*(( int*)
_temp391) == Cyc_Absyn_Eq_constr: 0){ _LL402: _temp401=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp391)->f1; if( _temp401 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL394;} else{
goto _LL395;}} else{ goto _LL395;} _LL395: if(( unsigned int) _temp391 > 1u?*((
int*) _temp391) == Cyc_Absyn_Eq_constr: 0){ _LL404: _temp403=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp391)->f1; if(( unsigned int) _temp403 > 1u?*((
int*) _temp403) == Cyc_Absyn_Upper_b: 0){ goto _LL396;} else{ goto _LL397;}}
else{ goto _LL397;} _LL397: if( _temp391 ==( void*) Cyc_Absyn_No_constr){ goto
_LL398;} else{ goto _LL399;} _LL399: if(( unsigned int) _temp391 > 1u?*(( int*)
_temp391) == Cyc_Absyn_Forward_constr: 0){ goto _LL400;} else{ goto _LL392;}
_LL394: return( void*) Cyc_Absyn_MemKind; _LL396: return( void*) Cyc_Absyn_BoxKind;
_LL398: return( void*) Cyc_Absyn_MemKind; _LL400: return Cyc_Tcutil_impos(
_tag_arr("typ_kind: forward constraint in ptr bounds", sizeof( unsigned char),
43u)); _LL392:;} _LL352: return( void*) Cyc_Absyn_MemKind; _LL354: return( void*)
Cyc_Absyn_MemKind; _LL356: return Cyc_Tcutil_impos(( struct _tagged_arr)({
struct _tagged_arr(* _temp405)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp409= _tag_arr("typ_kind: typedef found: %s", sizeof(
unsigned char), 28u); struct Cyc_Stdio_String_pa_struct _temp410; _temp410.tag=
Cyc_Stdio_String_pa; _temp410.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp408=( void*)& _temp410; void* _temp406[ 1u]={ _temp408}; struct
_tagged_arr _temp407={( void*) _temp406,( void*) _temp406,( void*)( _temp406 + 1u)};
_temp405( _temp409, _temp407);}})); _LL358: return( void*) Cyc_Absyn_EffKind;
_LL360: return( void*) Cyc_Absyn_EffKind; _LL310:;} unsigned char Cyc_Tcutil_Unify[
10u]="\000\000\000\000Unify"; int Cyc_Tcutil_unify( void* t1, void* t2){ struct
_handler_cons _temp411; _push_handler(& _temp411);{ int _temp413= 0; if( setjmp(
_temp411.handler)){ _temp413= 1;} if( ! _temp413){ Cyc_Tcutil_unify_it( t1, t2);{
int _temp414= 1; _npop_handler( 0u); return _temp414;}; _pop_handler();} else{
void* _temp412=( void*) _exn_thrown; void* _temp416= _temp412; _LL418: if(
_temp416 == Cyc_Tcutil_Unify){ goto _LL419;} else{ goto _LL420;} _LL420: goto
_LL421; _LL419: return 0; _LL421:( void) _throw( _temp416); _LL417:;}}} static
void Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List* env, struct Cyc_List_List*
ts); static void Cyc_Tcutil_occurs( void* evar, struct Cyc_List_List* env, void*
t){ void* _temp422= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar* _temp456;
struct Cyc_Core_Opt* _temp458; struct Cyc_Core_Opt** _temp460; struct Cyc_Core_Opt*
_temp461; struct Cyc_Absyn_PtrInfo _temp463; void* _temp465; struct Cyc_Absyn_FnInfo
_temp467; struct Cyc_List_List* _temp469; struct Cyc_List_List* _temp471; struct
Cyc_Absyn_VarargInfo* _temp473; int _temp475; struct Cyc_List_List* _temp477;
void* _temp479; struct Cyc_Core_Opt* _temp481; struct Cyc_List_List* _temp483;
struct Cyc_List_List* _temp485; void* _temp487; struct Cyc_Absyn_TunionInfo
_temp489; void* _temp491; struct Cyc_List_List* _temp493; struct Cyc_Core_Opt*
_temp495; struct Cyc_List_List* _temp497; struct Cyc_Absyn_TunionFieldInfo
_temp499; struct Cyc_List_List* _temp501; struct Cyc_List_List* _temp503; struct
Cyc_List_List* _temp505; struct Cyc_List_List* _temp507; void* _temp509; struct
Cyc_List_List* _temp511; _LL424: if(( unsigned int) _temp422 > 4u?*(( int*)
_temp422) == Cyc_Absyn_VarType: 0){ _LL457: _temp456=(( struct Cyc_Absyn_VarType_struct*)
_temp422)->f1; goto _LL425;} else{ goto _LL426;} _LL426: if(( unsigned int)
_temp422 > 4u?*(( int*) _temp422) == Cyc_Absyn_Evar: 0){ _LL462: _temp461=((
struct Cyc_Absyn_Evar_struct*) _temp422)->f2; goto _LL459; _LL459: _temp458=((
struct Cyc_Absyn_Evar_struct*) _temp422)->f4; _temp460=&(( struct Cyc_Absyn_Evar_struct*)
_temp422)->f4; goto _LL427;} else{ goto _LL428;} _LL428: if(( unsigned int)
_temp422 > 4u?*(( int*) _temp422) == Cyc_Absyn_PointerType: 0){ _LL464: _temp463=((
struct Cyc_Absyn_PointerType_struct*) _temp422)->f1; goto _LL429;} else{ goto
_LL430;} _LL430: if(( unsigned int) _temp422 > 4u?*(( int*) _temp422) == Cyc_Absyn_ArrayType:
0){ _LL466: _temp465=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp422)->f1;
goto _LL431;} else{ goto _LL432;} _LL432: if(( unsigned int) _temp422 > 4u?*((
int*) _temp422) == Cyc_Absyn_FnType: 0){ _LL468: _temp467=(( struct Cyc_Absyn_FnType_struct*)
_temp422)->f1; _LL484: _temp483= _temp467.tvars; goto _LL482; _LL482: _temp481=
_temp467.effect; goto _LL480; _LL480: _temp479=( void*) _temp467.ret_typ; goto
_LL478; _LL478: _temp477= _temp467.args; goto _LL476; _LL476: _temp475= _temp467.c_varargs;
goto _LL474; _LL474: _temp473= _temp467.cyc_varargs; goto _LL472; _LL472:
_temp471= _temp467.rgn_po; goto _LL470; _LL470: _temp469= _temp467.attributes;
goto _LL433;} else{ goto _LL434;} _LL434: if(( unsigned int) _temp422 > 4u?*((
int*) _temp422) == Cyc_Absyn_TupleType: 0){ _LL486: _temp485=(( struct Cyc_Absyn_TupleType_struct*)
_temp422)->f1; goto _LL435;} else{ goto _LL436;} _LL436: if(( unsigned int)
_temp422 > 4u?*(( int*) _temp422) == Cyc_Absyn_RgnHandleType: 0){ _LL488:
_temp487=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp422)->f1; goto
_LL437;} else{ goto _LL438;} _LL438: if(( unsigned int) _temp422 > 4u?*(( int*)
_temp422) == Cyc_Absyn_TunionType: 0){ _LL490: _temp489=(( struct Cyc_Absyn_TunionType_struct*)
_temp422)->f1; _LL494: _temp493= _temp489.targs; goto _LL492; _LL492: _temp491=(
void*) _temp489.rgn; goto _LL439;} else{ goto _LL440;} _LL440: if(( unsigned int)
_temp422 > 4u?*(( int*) _temp422) == Cyc_Absyn_TypedefType: 0){ _LL498: _temp497=((
struct Cyc_Absyn_TypedefType_struct*) _temp422)->f2; goto _LL496; _LL496:
_temp495=(( struct Cyc_Absyn_TypedefType_struct*) _temp422)->f3; goto _LL441;}
else{ goto _LL442;} _LL442: if(( unsigned int) _temp422 > 4u?*(( int*) _temp422)
== Cyc_Absyn_TunionFieldType: 0){ _LL500: _temp499=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp422)->f1; _LL502: _temp501= _temp499.targs; goto _LL443;} else{ goto _LL444;}
_LL444: if(( unsigned int) _temp422 > 4u?*(( int*) _temp422) == Cyc_Absyn_StructType:
0){ _LL504: _temp503=(( struct Cyc_Absyn_StructType_struct*) _temp422)->f2; goto
_LL445;} else{ goto _LL446;} _LL446: if(( unsigned int) _temp422 > 4u?*(( int*)
_temp422) == Cyc_Absyn_AnonStructType: 0){ _LL506: _temp505=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp422)->f1; goto _LL447;} else{ goto _LL448;} _LL448: if(( unsigned int)
_temp422 > 4u?*(( int*) _temp422) == Cyc_Absyn_AnonUnionType: 0){ _LL508:
_temp507=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp422)->f1; goto _LL449;}
else{ goto _LL450;} _LL450: if(( unsigned int) _temp422 > 4u?*(( int*) _temp422)
== Cyc_Absyn_AccessEff: 0){ _LL510: _temp509=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp422)->f1; goto _LL451;} else{ goto _LL452;} _LL452: if(( unsigned int)
_temp422 > 4u?*(( int*) _temp422) == Cyc_Absyn_JoinEff: 0){ _LL512: _temp511=((
struct Cyc_Absyn_JoinEff_struct*) _temp422)->f1; goto _LL453;} else{ goto _LL454;}
_LL454: goto _LL455; _LL425: if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, env, _temp456)){( int) _throw(( void*) Cyc_Tcutil_Unify);}
goto _LL423; _LL427: if( t == evar){( int) _throw(( void*) Cyc_Tcutil_Unify);}
else{ if( _temp461 != 0){ Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp461))->v);} else{ int problem= 0;{ struct Cyc_List_List* s=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(* _temp460))->v; for(
0; s != 0; s=(( struct Cyc_List_List*) _check_null( s))->tl){ if( !(( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( s))->hd)){ problem=
1; break;}}} if( problem){ struct Cyc_List_List* _temp513= 0;{ struct Cyc_List_List*
s=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(* _temp460))->v;
for( 0; s != 0; s=(( struct Cyc_List_List*) _check_null( s))->tl){ if((( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( s))->hd)){
_temp513=({ struct Cyc_List_List* _temp514=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp514->hd=( void*)(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( s))->hd); _temp514->tl= _temp513; _temp514;});}}}*
_temp460=({ struct Cyc_Core_Opt* _temp515=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp515->v=( void*) _temp513; _temp515;});}}}
goto _LL423; _LL429: Cyc_Tcutil_occurs( evar, env,( void*) _temp463.elt_typ);
Cyc_Tcutil_occurs( evar, env,( void*) _temp463.rgn_typ); goto _LL423; _LL431:
Cyc_Tcutil_occurs( evar, env, _temp465); goto _LL423; _LL433: if( _temp481 != 0){
Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp481))->v);} Cyc_Tcutil_occurs( evar, env, _temp479); for( 0; _temp477 != 0;
_temp477=(( struct Cyc_List_List*) _check_null( _temp477))->tl){ Cyc_Tcutil_occurs(
evar, env,(*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp477))->hd)).f3);}
if( _temp473 != 0){ void* _temp518; void* _temp520; struct Cyc_Absyn_VarargInfo
_temp516=*(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp473)); _LL521:
_temp520=( void*) _temp516.type; goto _LL519; _LL519: _temp518=( void*) _temp516.rgn;
goto _LL517; _LL517: Cyc_Tcutil_occurs( evar, env, _temp520); Cyc_Tcutil_occurs(
evar, env, _temp518);} for( 0; _temp471 != 0; _temp471=(( struct Cyc_List_List*)
_check_null( _temp471))->tl){ struct _tuple8 _temp524; void* _temp525; void*
_temp527; struct _tuple8* _temp522=( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( _temp471))->hd; _temp524=* _temp522; _LL528: _temp527= _temp524.f1;
goto _LL526; _LL526: _temp525= _temp524.f2; goto _LL523; _LL523: Cyc_Tcutil_occurs(
evar, env, _temp527); Cyc_Tcutil_occurs( evar, env, _temp525);} goto _LL423;
_LL435: for( 0; _temp485 != 0; _temp485=(( struct Cyc_List_List*) _check_null(
_temp485))->tl){ Cyc_Tcutil_occurs( evar, env,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp485))->hd)).f2);} goto _LL423; _LL437: Cyc_Tcutil_occurs( evar,
env, _temp487); goto _LL423; _LL439: Cyc_Tcutil_occurs( evar, env, _temp491);
Cyc_Tcutil_occurslist( evar, env, _temp493); goto _LL423; _LL441: _temp501=
_temp497; goto _LL443; _LL443: _temp503= _temp501; goto _LL445; _LL445: Cyc_Tcutil_occurslist(
evar, env, _temp503); goto _LL423; _LL447: _temp507= _temp505; goto _LL449;
_LL449: for( 0; _temp507 != 0; _temp507=(( struct Cyc_List_List*) _check_null(
_temp507))->tl){ Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp507))->hd)->type);} goto _LL423; _LL451:
Cyc_Tcutil_occurs( evar, env, _temp509); goto _LL423; _LL453: Cyc_Tcutil_occurslist(
evar, env, _temp511); goto _LL423; _LL455: goto _LL423; _LL423:;} static void
Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List* env, struct Cyc_List_List*
ts){ for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){ Cyc_Tcutil_occurs(
evar, env,( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);}} static
void Cyc_Tcutil_unify_list( struct Cyc_List_List* t1, struct Cyc_List_List* t2){
struct _handler_cons _temp529; _push_handler(& _temp529);{ int _temp531= 0; if(
setjmp( _temp529.handler)){ _temp531= 1;} if( ! _temp531){ Cyc_List_iter2( Cyc_Tcutil_unify_it,
t1, t2);; _pop_handler();} else{ void* _temp530=( void*) _exn_thrown; void*
_temp533= _temp530; _LL535: if( _temp533 == Cyc_List_List_mismatch){ goto _LL536;}
else{ goto _LL537;} _LL537: goto _LL538; _LL536:( int) _throw(( void*) Cyc_Tcutil_Unify);
_LL538:( void) _throw( _temp533); _LL534:;}}} static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2){ if(( tq1.q_const != tq2.q_const?
1: tq1.q_volatile != tq2.q_volatile)? 1: tq1.q_restrict != tq2.q_restrict){( int)
_throw(( void*) Cyc_Tcutil_Unify);}} int Cyc_Tcutil_equal_tqual( struct Cyc_Absyn_Tqual
tq1, struct Cyc_Absyn_Tqual tq2){ return( tq1.q_const == tq2.q_const? tq1.q_volatile
== tq2.q_volatile: 0)? tq1.q_restrict == tq2.q_restrict: 0;} static void Cyc_Tcutil_unify_it_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y){ x= Cyc_Absyn_compress_conref( x); y= Cyc_Absyn_compress_conref( y); if( x ==
y){ return;}{ void* _temp539=( void*) x->v; void* _temp547; _LL541: if( _temp539
==( void*) Cyc_Absyn_No_constr){ goto _LL542;} else{ goto _LL543;} _LL543: if((
unsigned int) _temp539 > 1u?*(( int*) _temp539) == Cyc_Absyn_Forward_constr: 0){
goto _LL544;} else{ goto _LL545;} _LL545: if(( unsigned int) _temp539 > 1u?*((
int*) _temp539) == Cyc_Absyn_Eq_constr: 0){ _LL548: _temp547=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp539)->f1; goto _LL546;} else{ goto _LL540;}
_LL542:( void*)( x->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp549=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp549[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp550; _temp550.tag= Cyc_Absyn_Forward_constr; _temp550.f1= y; _temp550;});
_temp549;}))); return; _LL544:( int) _throw( Cyc_Tcutil_impos( _tag_arr("unify_conref: forward after compress",
sizeof( unsigned char), 37u))); _LL546: { void* _temp551=( void*) y->v; void*
_temp559; _LL553: if( _temp551 ==( void*) Cyc_Absyn_No_constr){ goto _LL554;}
else{ goto _LL555;} _LL555: if(( unsigned int) _temp551 > 1u?*(( int*) _temp551)
== Cyc_Absyn_Forward_constr: 0){ goto _LL556;} else{ goto _LL557;} _LL557: if((
unsigned int) _temp551 > 1u?*(( int*) _temp551) == Cyc_Absyn_Eq_constr: 0){
_LL560: _temp559=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp551)->f1;
goto _LL558;} else{ goto _LL552;} _LL554:( void*)( y->v=( void*)(( void*) x->v));
return; _LL556:( int) _throw( Cyc_Tcutil_impos( _tag_arr("unify_conref: forward after compress(2)",
sizeof( unsigned char), 40u))); _LL558: if( cmp( _temp547, _temp559) != 0){( int)
_throw(( void*) Cyc_Tcutil_Unify);} return; _LL552:;} _LL540:;}} static int Cyc_Tcutil_unify_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y){ struct _handler_cons _temp561; _push_handler(& _temp561);{ int _temp563= 0;
if( setjmp( _temp561.handler)){ _temp563= 1;} if( ! _temp563){ Cyc_Tcutil_unify_it_conrefs(
cmp, x, y);{ int _temp564= 1; _npop_handler( 0u); return _temp564;};
_pop_handler();} else{ void* _temp562=( void*) _exn_thrown; void* _temp566=
_temp562; _LL568: if( _temp566 == Cyc_Tcutil_Unify){ goto _LL569;} else{ goto
_LL570;} _LL570: goto _LL571; _LL569: return 0; _LL571:( void) _throw( _temp566);
_LL567:;}}} static int Cyc_Tcutil_boundscmp( void* b1, void* b2){ struct _tuple8
_temp573=({ struct _tuple8 _temp572; _temp572.f1= b1; _temp572.f2= b2; _temp572;});
void* _temp583; void* _temp585; void* _temp587; void* _temp589; void* _temp591;
struct Cyc_Absyn_Exp* _temp593; void* _temp595; struct Cyc_Absyn_Exp* _temp597;
_LL575: _LL586: _temp585= _temp573.f1; if( _temp585 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL584;} else{ goto _LL577;} _LL584: _temp583= _temp573.f2; if( _temp583 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL576;} else{ goto _LL577;} _LL577: _LL588:
_temp587= _temp573.f1; if( _temp587 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL578;}
else{ goto _LL579;} _LL579: _LL590: _temp589= _temp573.f2; if( _temp589 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL580;} else{ goto _LL581;} _LL581: _LL596: _temp595=
_temp573.f1; if(( unsigned int) _temp595 > 1u?*(( int*) _temp595) == Cyc_Absyn_Upper_b:
0){ _LL598: _temp597=(( struct Cyc_Absyn_Upper_b_struct*) _temp595)->f1; goto
_LL592;} else{ goto _LL574;} _LL592: _temp591= _temp573.f2; if(( unsigned int)
_temp591 > 1u?*(( int*) _temp591) == Cyc_Absyn_Upper_b: 0){ _LL594: _temp593=((
struct Cyc_Absyn_Upper_b_struct*) _temp591)->f1; goto _LL582;} else{ goto _LL574;}
_LL576: return 0; _LL578: return - 1; _LL580: return 1; _LL582: { int i1=( int)
Cyc_Evexp_eval_const_uint_exp( _temp597); int i2=( int) Cyc_Evexp_eval_const_uint_exp(
_temp593); if( i1 == i2){ return 0;} if( i1 < i2){ return - 1;} return 1;}
_LL574:;} static int Cyc_Tcutil_equal_att( void* a1, void* a2){ if( a1 == a2){
return 1;}{ struct _tuple8 _temp600=({ struct _tuple8 _temp599; _temp599.f1= a1;
_temp599.f2= a2; _temp599;}); void* _temp610; int _temp612; void* _temp614; int
_temp616; void* _temp618; int _temp620; void* _temp622; int _temp624; void*
_temp626; struct _tagged_arr _temp628; void* _temp630; struct _tagged_arr
_temp632; _LL602: _LL615: _temp614= _temp600.f1; if(( unsigned int) _temp614 >
16u?*(( int*) _temp614) == Cyc_Absyn_Regparm_att: 0){ _LL617: _temp616=(( struct
Cyc_Absyn_Regparm_att_struct*) _temp614)->f1; goto _LL611;} else{ goto _LL604;}
_LL611: _temp610= _temp600.f2; if(( unsigned int) _temp610 > 16u?*(( int*)
_temp610) == Cyc_Absyn_Regparm_att: 0){ _LL613: _temp612=(( struct Cyc_Absyn_Regparm_att_struct*)
_temp610)->f1; goto _LL603;} else{ goto _LL604;} _LL604: _LL623: _temp622=
_temp600.f1; if(( unsigned int) _temp622 > 16u?*(( int*) _temp622) == Cyc_Absyn_Aligned_att:
0){ _LL625: _temp624=(( struct Cyc_Absyn_Aligned_att_struct*) _temp622)->f1;
goto _LL619;} else{ goto _LL606;} _LL619: _temp618= _temp600.f2; if((
unsigned int) _temp618 > 16u?*(( int*) _temp618) == Cyc_Absyn_Aligned_att: 0){
_LL621: _temp620=(( struct Cyc_Absyn_Aligned_att_struct*) _temp618)->f1; goto
_LL605;} else{ goto _LL606;} _LL606: _LL631: _temp630= _temp600.f1; if((
unsigned int) _temp630 > 16u?*(( int*) _temp630) == Cyc_Absyn_Section_att: 0){
_LL633: _temp632=(( struct Cyc_Absyn_Section_att_struct*) _temp630)->f1; goto
_LL627;} else{ goto _LL608;} _LL627: _temp626= _temp600.f2; if(( unsigned int)
_temp626 > 16u?*(( int*) _temp626) == Cyc_Absyn_Section_att: 0){ _LL629:
_temp628=(( struct Cyc_Absyn_Section_att_struct*) _temp626)->f1; goto _LL607;}
else{ goto _LL608;} _LL608: goto _LL609; _LL603: _temp624= _temp616; _temp620=
_temp612; goto _LL605; _LL605: return _temp624 == _temp620; _LL607: return Cyc_String_strcmp(
_temp632, _temp628) == 0; _LL609: return 0; _LL601:;}} int Cyc_Tcutil_same_atts(
struct Cyc_List_List* a1, struct Cyc_List_List* a2){{ struct Cyc_List_List* a=
a1; for( 0; a != 0; a=(( struct Cyc_List_List*) _check_null( a))->tl){ if( ! Cyc_List_exists_c(
Cyc_Tcutil_equal_att,( void*)(( struct Cyc_List_List*) _check_null( a))->hd, a2)){
return 0;}}}{ struct Cyc_List_List* a= a2; for( 0; a != 0; a=(( struct Cyc_List_List*)
_check_null( a))->tl){ if( ! Cyc_List_exists_c( Cyc_Tcutil_equal_att,( void*)((
struct Cyc_List_List*) _check_null( a))->hd, a1)){ return 0;}}} return 1;}
static void Cyc_Tcutil_normalize_effect( void* e){ e= Cyc_Tcutil_compress( e);{
void* _temp634= e; struct Cyc_List_List* _temp640; struct Cyc_List_List**
_temp642; _LL636: if(( unsigned int) _temp634 > 4u?*(( int*) _temp634) == Cyc_Absyn_JoinEff:
0){ _LL641: _temp640=(( struct Cyc_Absyn_JoinEff_struct*) _temp634)->f1;
_temp642=&(( struct Cyc_Absyn_JoinEff_struct*) _temp634)->f1; goto _LL637;}
else{ goto _LL638;} _LL638: goto _LL639; _LL637: { int nested_join= 0;{ struct
Cyc_List_List* effs=* _temp642; for( 0; effs != 0; effs=(( struct Cyc_List_List*)
_check_null( effs))->tl){ void* _temp643=( void*)(( struct Cyc_List_List*)
_check_null( effs))->hd; Cyc_Tcutil_normalize_effect( _temp643);{ void* _temp644=
Cyc_Tcutil_compress( _temp643); _LL646: if(( unsigned int) _temp644 > 4u?*(( int*)
_temp644) == Cyc_Absyn_JoinEff: 0){ goto _LL647;} else{ goto _LL648;} _LL648:
goto _LL649; _LL647: nested_join= 1; goto _LL645; _LL649: goto _LL645; _LL645:;}}}
if( ! nested_join){ return;}{ struct Cyc_List_List* effects= 0;{ struct Cyc_List_List*
effs=* _temp642; for( 0; effs != 0; effs=(( struct Cyc_List_List*) _check_null(
effs))->tl){ void* _temp650= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*)
_check_null( effs))->hd); struct Cyc_List_List* _temp656; _LL652: if((
unsigned int) _temp650 > 4u?*(( int*) _temp650) == Cyc_Absyn_JoinEff: 0){ _LL657:
_temp656=(( struct Cyc_Absyn_JoinEff_struct*) _temp650)->f1; goto _LL653;} else{
goto _LL654;} _LL654: goto _LL655; _LL653: effects= Cyc_List_revappend( _temp656,
effects); goto _LL651; _LL655: effects=({ struct Cyc_List_List* _temp658=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp658->hd=(
void*) _temp650; _temp658->tl= effects; _temp658;}); goto _LL651; _LL651:;}}*
_temp642= Cyc_List_imp_rev( effects); goto _LL635;}} _LL639: goto _LL635; _LL635:;}}
struct _tuple9{ void* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; }
; static struct _tuple9* Cyc_Tcutil_get_effect_evar( void* t){ void* _temp659=
Cyc_Tcutil_compress( t); struct Cyc_List_List* _temp667; struct Cyc_List_List
_temp669; struct Cyc_List_List* _temp670; void* _temp672; struct Cyc_Core_Opt*
_temp674; struct Cyc_Core_Opt* _temp676; _LL661: if(( unsigned int) _temp659 > 4u?*((
int*) _temp659) == Cyc_Absyn_JoinEff: 0){ _LL668: _temp667=(( struct Cyc_Absyn_JoinEff_struct*)
_temp659)->f1; if( _temp667 == 0){ goto _LL663;} else{ _temp669=* _temp667;
_LL673: _temp672=( void*) _temp669.hd; goto _LL671; _LL671: _temp670= _temp669.tl;
goto _LL662;}} else{ goto _LL663;} _LL663: if(( unsigned int) _temp659 > 4u?*((
int*) _temp659) == Cyc_Absyn_Evar: 0){ _LL677: _temp676=(( struct Cyc_Absyn_Evar_struct*)
_temp659)->f1; goto _LL675; _LL675: _temp674=(( struct Cyc_Absyn_Evar_struct*)
_temp659)->f4; goto _LL664;} else{ goto _LL665;} _LL665: goto _LL666; _LL662: {
void* _temp678= Cyc_Tcutil_compress( _temp672); struct Cyc_Core_Opt* _temp684;
_LL680: if(( unsigned int) _temp678 > 4u?*(( int*) _temp678) == Cyc_Absyn_Evar:
0){ _LL685: _temp684=(( struct Cyc_Absyn_Evar_struct*) _temp678)->f4; goto
_LL681;} else{ goto _LL682;} _LL682: goto _LL683; _LL681: return({ struct
_tuple9* _temp686=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp686->f1= _temp672; _temp686->f2= _temp670; _temp686->f3=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp684))->v; _temp686;}); _LL683: return 0;
_LL679:;} _LL664: if( _temp676 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp676))->v !=( void*) Cyc_Absyn_EffKind){(( int(*)( struct
_tagged_arr msg)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u));} return({ struct _tuple9* _temp687=( struct
_tuple9*) GC_malloc( sizeof( struct _tuple9)); _temp687->f1= t; _temp687->f2= 0;
_temp687->f3=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp674))->v; _temp687;}); _LL666: return 0; _LL660:;} static struct Cyc_Core_Opt
Cyc_Tcutil_ek={( void*)(( void*) 4u)}; int Cyc_Tcutil_region_in_effect( int
constrain, void* r, void* e){ r= Cyc_Tcutil_compress( r); if( r ==( void*) Cyc_Absyn_HeapRgn){
return 1;}{ void* _temp688= Cyc_Tcutil_compress( e); void* _temp698; struct Cyc_List_List*
_temp700; struct Cyc_Core_Opt* _temp702; struct Cyc_Core_Opt* _temp704; struct
Cyc_Core_Opt** _temp706; struct Cyc_Core_Opt* _temp707; _LL690: if((
unsigned int) _temp688 > 4u?*(( int*) _temp688) == Cyc_Absyn_AccessEff: 0){
_LL699: _temp698=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp688)->f1;
goto _LL691;} else{ goto _LL692;} _LL692: if(( unsigned int) _temp688 > 4u?*((
int*) _temp688) == Cyc_Absyn_JoinEff: 0){ _LL701: _temp700=(( struct Cyc_Absyn_JoinEff_struct*)
_temp688)->f1; goto _LL693;} else{ goto _LL694;} _LL694: if(( unsigned int)
_temp688 > 4u?*(( int*) _temp688) == Cyc_Absyn_Evar: 0){ _LL708: _temp707=((
struct Cyc_Absyn_Evar_struct*) _temp688)->f1; goto _LL705; _LL705: _temp704=((
struct Cyc_Absyn_Evar_struct*) _temp688)->f2; _temp706=&(( struct Cyc_Absyn_Evar_struct*)
_temp688)->f2; goto _LL703; _LL703: _temp702=(( struct Cyc_Absyn_Evar_struct*)
_temp688)->f4; goto _LL695;} else{ goto _LL696;} _LL696: goto _LL697; _LL691:
if( constrain){ return Cyc_Tcutil_unify( r, _temp698);} Cyc_Tcutil_compress(
_temp698); if( r == _temp698){ return 1;}{ struct _tuple8 _temp710=({ struct
_tuple8 _temp709; _temp709.f1= r; _temp709.f2= _temp698; _temp709;}); void*
_temp716; struct Cyc_Absyn_Tvar* _temp718; void* _temp720; struct Cyc_Absyn_Tvar*
_temp722; _LL712: _LL721: _temp720= _temp710.f1; if(( unsigned int) _temp720 > 4u?*((
int*) _temp720) == Cyc_Absyn_VarType: 0){ _LL723: _temp722=(( struct Cyc_Absyn_VarType_struct*)
_temp720)->f1; goto _LL717;} else{ goto _LL714;} _LL717: _temp716= _temp710.f2;
if(( unsigned int) _temp716 > 4u?*(( int*) _temp716) == Cyc_Absyn_VarType: 0){
_LL719: _temp718=(( struct Cyc_Absyn_VarType_struct*) _temp716)->f1; goto _LL713;}
else{ goto _LL714;} _LL714: goto _LL715; _LL713: return Cyc_Absyn_tvar_cmp(
_temp722, _temp718) == 0; _LL715: return 0; _LL711:;} _LL693: for( 0; _temp700
!= 0; _temp700=(( struct Cyc_List_List*) _check_null( _temp700))->tl){ if( Cyc_Tcutil_region_in_effect(
constrain, r,( void*)(( struct Cyc_List_List*) _check_null( _temp700))->hd)){
return 1;}} return 0; _LL695: if( _temp707 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp707))->v !=( void*) Cyc_Absyn_EffKind){(( int(*)( struct
_tagged_arr msg)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u));} if( ! constrain){ return 0;}{ void* _temp724=
Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek, _temp702); Cyc_Tcutil_occurs(
_temp724,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp702))->v,
r);{ struct Cyc_Absyn_JoinEff_struct* _temp725=({ struct Cyc_Absyn_JoinEff_struct*
_temp727=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp727[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp728; _temp728.tag= Cyc_Absyn_JoinEff;
_temp728.f1=({ struct Cyc_List_List* _temp729=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp729->hd=( void*) _temp724; _temp729->tl=({
struct Cyc_List_List* _temp730=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp730->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp731=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp731[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp732; _temp732.tag= Cyc_Absyn_AccessEff;
_temp732.f1=( void*) r; _temp732;}); _temp731;})); _temp730->tl= 0; _temp730;});
_temp729;}); _temp728;}); _temp727;});* _temp706=({ struct Cyc_Core_Opt*
_temp726=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp726->v=( void*)(( void*) _temp725); _temp726;}); return 1;}} _LL697: return
0; _LL689:;}} static int Cyc_Tcutil_variable_in_effect( int constrain, struct
Cyc_Absyn_Tvar* v, void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp733= e;
struct Cyc_Absyn_Tvar* _temp743; struct Cyc_List_List* _temp745; struct Cyc_Core_Opt*
_temp747; struct Cyc_Core_Opt* _temp749; struct Cyc_Core_Opt** _temp751; struct
Cyc_Core_Opt* _temp752; _LL735: if(( unsigned int) _temp733 > 4u?*(( int*)
_temp733) == Cyc_Absyn_VarType: 0){ _LL744: _temp743=(( struct Cyc_Absyn_VarType_struct*)
_temp733)->f1; goto _LL736;} else{ goto _LL737;} _LL737: if(( unsigned int)
_temp733 > 4u?*(( int*) _temp733) == Cyc_Absyn_JoinEff: 0){ _LL746: _temp745=((
struct Cyc_Absyn_JoinEff_struct*) _temp733)->f1; goto _LL738;} else{ goto _LL739;}
_LL739: if(( unsigned int) _temp733 > 4u?*(( int*) _temp733) == Cyc_Absyn_Evar:
0){ _LL753: _temp752=(( struct Cyc_Absyn_Evar_struct*) _temp733)->f1; goto
_LL750; _LL750: _temp749=(( struct Cyc_Absyn_Evar_struct*) _temp733)->f2;
_temp751=&(( struct Cyc_Absyn_Evar_struct*) _temp733)->f2; goto _LL748; _LL748:
_temp747=(( struct Cyc_Absyn_Evar_struct*) _temp733)->f4; goto _LL740;} else{
goto _LL741;} _LL741: goto _LL742; _LL736: return Cyc_Absyn_tvar_cmp( v,
_temp743) == 0; _LL738: for( 0; _temp745 != 0; _temp745=(( struct Cyc_List_List*)
_check_null( _temp745))->tl){ if( Cyc_Tcutil_variable_in_effect( constrain, v,(
void*)(( struct Cyc_List_List*) _check_null( _temp745))->hd)){ return 1;}}
return 0; _LL740: if( _temp752 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp752))->v !=( void*) Cyc_Absyn_EffKind){(( int(*)( struct
_tagged_arr msg)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u));}{ void* _temp754= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek, _temp747); if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp747))->v, v)){ return 0;}{ struct Cyc_Absyn_JoinEff_struct*
_temp755=({ struct Cyc_Absyn_JoinEff_struct* _temp757=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp757[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp758; _temp758.tag= Cyc_Absyn_JoinEff; _temp758.f1=({ struct Cyc_List_List*
_temp759=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp759->hd=( void*) _temp754; _temp759->tl=({ struct Cyc_List_List* _temp760=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp760->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp761=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp761[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp762; _temp762.tag= Cyc_Absyn_VarType; _temp762.f1= v; _temp762;}); _temp761;}));
_temp760->tl= 0; _temp760;}); _temp759;}); _temp758;}); _temp757;});* _temp751=({
struct Cyc_Core_Opt* _temp756=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp756->v=( void*)(( void*) _temp755); _temp756;}); return 1;}}
_LL742: return 0; _LL734:;}} static int Cyc_Tcutil_evar_in_effect( void* evar,
void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp763= e; struct Cyc_List_List*
_temp771; _LL765: if(( unsigned int) _temp763 > 4u?*(( int*) _temp763) == Cyc_Absyn_JoinEff:
0){ _LL772: _temp771=(( struct Cyc_Absyn_JoinEff_struct*) _temp763)->f1; goto
_LL766;} else{ goto _LL767;} _LL767: if(( unsigned int) _temp763 > 4u?*(( int*)
_temp763) == Cyc_Absyn_Evar: 0){ goto _LL768;} else{ goto _LL769;} _LL769: goto
_LL770; _LL766: for( 0; _temp771 != 0; _temp771=(( struct Cyc_List_List*)
_check_null( _temp771))->tl){ if( Cyc_Tcutil_evar_in_effect( evar,( void*)((
struct Cyc_List_List*) _check_null( _temp771))->hd)){ return 1;}} return 0;
_LL768: return evar == e; _LL770: return 0; _LL764:;}} int Cyc_Tcutil_subset_effect(
int set_to_empty, void* e1, void* e2){{ struct _tuple9* _temp773= Cyc_Tcutil_get_effect_evar(
e2); struct _tuple9 _temp779; struct Cyc_List_List* _temp780; struct Cyc_List_List*
_temp782; void* _temp784; _LL775: if( _temp773 == 0){ goto _LL777;} else{
_temp779=* _temp773; _LL785: _temp784= _temp779.f1; goto _LL783; _LL783:
_temp782= _temp779.f2; goto _LL781; _LL781: _temp780= _temp779.f3; goto _LL776;}
_LL777: goto _LL778; _LL776: { void* _temp786= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,({ struct Cyc_Core_Opt* _temp792=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp792->v=( void*) _temp780; _temp792;}));
struct Cyc_Absyn_JoinEff_struct* _temp787=({ struct Cyc_Absyn_JoinEff_struct*
_temp788=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp788[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp789; _temp789.tag= Cyc_Absyn_JoinEff;
_temp789.f1=({ struct Cyc_List_List* _temp790=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp790->hd=( void*) _temp786; _temp790->tl=({
struct Cyc_List_List* _temp791=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp791->hd=( void*) e1; _temp791->tl= 0; _temp791;});
_temp790;}); _temp789;}); _temp788;}); return Cyc_Tcutil_unify( _temp784,( void*)
_temp787);} _LL778: goto _LL774; _LL774:;}{ void* _temp793= Cyc_Tcutil_compress(
e1); struct Cyc_List_List* _temp805; void* _temp807; struct Cyc_Absyn_Tvar*
_temp809; struct Cyc_Core_Opt* _temp811; struct Cyc_Core_Opt* _temp813; struct
Cyc_Core_Opt** _temp815; _LL795: if(( unsigned int) _temp793 > 4u?*(( int*)
_temp793) == Cyc_Absyn_JoinEff: 0){ _LL806: _temp805=(( struct Cyc_Absyn_JoinEff_struct*)
_temp793)->f1; goto _LL796;} else{ goto _LL797;} _LL797: if(( unsigned int)
_temp793 > 4u?*(( int*) _temp793) == Cyc_Absyn_AccessEff: 0){ _LL808: _temp807=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp793)->f1; goto _LL798;} else{
goto _LL799;} _LL799: if(( unsigned int) _temp793 > 4u?*(( int*) _temp793) ==
Cyc_Absyn_VarType: 0){ _LL810: _temp809=(( struct Cyc_Absyn_VarType_struct*)
_temp793)->f1; goto _LL800;} else{ goto _LL801;} _LL801: if(( unsigned int)
_temp793 > 4u?*(( int*) _temp793) == Cyc_Absyn_Evar: 0){ _LL814: _temp813=((
struct Cyc_Absyn_Evar_struct*) _temp793)->f2; _temp815=&(( struct Cyc_Absyn_Evar_struct*)
_temp793)->f2; goto _LL812; _LL812: _temp811=(( struct Cyc_Absyn_Evar_struct*)
_temp793)->f4; goto _LL802;} else{ goto _LL803;} _LL803: goto _LL804; _LL796:
for( 0; _temp805 != 0; _temp805=(( struct Cyc_List_List*) _check_null( _temp805))->tl){
if( ! Cyc_Tcutil_subset_effect( set_to_empty,( void*)(( struct Cyc_List_List*)
_check_null( _temp805))->hd, e2)){ return 0;}} return 1; _LL798: return( Cyc_Tcutil_region_in_effect(
0, _temp807, e2)? 1: Cyc_Tcutil_region_in_effect( 1, _temp807, e2))? 1: Cyc_Tcutil_unify(
_temp807,( void*) Cyc_Absyn_HeapRgn); _LL800: return Cyc_Tcutil_variable_in_effect(
0, _temp809, e2)? 1: Cyc_Tcutil_variable_in_effect( 1, _temp809, e2); _LL802:
if( ! Cyc_Tcutil_evar_in_effect( e1, e2)){ if( set_to_empty){* _temp815=({
struct Cyc_Core_Opt* _temp816=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp816->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp817=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp817[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp818; _temp818.tag= Cyc_Absyn_JoinEff;
_temp818.f1= 0; _temp818;}); _temp817;})); _temp816;});} else{ Cyc_Tcutil_occurs(
e1,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp811))->v,
e2);* _temp815=({ struct Cyc_Core_Opt* _temp819=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp819->v=( void*) e2; _temp819;});}}
return 1; _LL804: return(( int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)((
struct _tagged_arr)({ struct _tagged_arr(* _temp820)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp824= _tag_arr("subset_effect: bad effect: %s",
sizeof( unsigned char), 30u); struct Cyc_Stdio_String_pa_struct _temp825;
_temp825.tag= Cyc_Stdio_String_pa; _temp825.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
e1);{ void* _temp823=( void*)& _temp825; void* _temp821[ 1u]={ _temp823}; struct
_tagged_arr _temp822={( void*) _temp821,( void*) _temp821,( void*)( _temp821 + 1u)};
_temp820( _temp824, _temp822);}})); _LL794:;}} struct _tuple10{ struct _tuple9*
f1; struct _tuple9* f2; } ; static int Cyc_Tcutil_unify_effect( void* e1, void*
e2){ e1= Cyc_Tcutil_compress( e1); e2= Cyc_Tcutil_compress( e2); Cyc_Tcutil_normalize_effect(
e1); Cyc_Tcutil_normalize_effect( e2);{ struct _tuple10 _temp827=({ struct
_tuple10 _temp826; _temp826.f1= Cyc_Tcutil_get_effect_evar( e1); _temp826.f2=
Cyc_Tcutil_get_effect_evar( e2); _temp826;}); struct _tuple9* _temp833; struct
_tuple9 _temp835; struct Cyc_List_List* _temp836; struct Cyc_List_List* _temp838;
void* _temp840; struct _tuple9* _temp842; struct _tuple9 _temp844; struct Cyc_List_List*
_temp845; struct Cyc_List_List* _temp847; void* _temp849; _LL829: _LL843:
_temp842= _temp827.f1; if( _temp842 == 0){ goto _LL831;} else{ _temp844=*
_temp842; _LL850: _temp849= _temp844.f1; goto _LL848; _LL848: _temp847= _temp844.f2;
goto _LL846; _LL846: _temp845= _temp844.f3; goto _LL834;} _LL834: _temp833=
_temp827.f2; if( _temp833 == 0){ goto _LL831;} else{ _temp835=* _temp833; _LL841:
_temp840= _temp835.f1; goto _LL839; _LL839: _temp838= _temp835.f2; goto _LL837;
_LL837: _temp836= _temp835.f3; goto _LL830;} _LL831: goto _LL832; _LL830: {
struct Cyc_List_List* _temp851= 0; for( 0; _temp845 != 0; _temp845=(( struct Cyc_List_List*)
_check_null( _temp845))->tl){ if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, _temp836,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp845))->hd)){ _temp851=({ struct Cyc_List_List* _temp852=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp852->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp845))->hd);
_temp852->tl= _temp851; _temp852;});}}{ void* _temp853= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,({ struct Cyc_Core_Opt* _temp858=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp858->v=( void*)
_temp851; _temp858;})); struct Cyc_Absyn_JoinEff_struct* _temp854=({ struct Cyc_Absyn_JoinEff_struct*
_temp855=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp855[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp856; _temp856.tag= Cyc_Absyn_JoinEff;
_temp856.f1=({ struct Cyc_List_List* _temp857=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp857->hd=( void*) _temp853; _temp857->tl=
Cyc_List_append( _temp847, _temp838); _temp857;}); _temp856;}); _temp855;});
return Cyc_Tcutil_unify( _temp849,( void*) _temp854)? Cyc_Tcutil_unify( _temp840,(
void*) _temp854): 0;}} _LL832: return Cyc_Tcutil_subset_effect( 0, e1, e2)? Cyc_Tcutil_subset_effect(
0, e2, e1): 0; _LL828:;}} static int Cyc_Tcutil_sub_rgnpo( struct Cyc_List_List*
rpo1, struct Cyc_List_List* rpo2){{ struct Cyc_List_List* r1= rpo1; for( 0; r1
!= 0; r1=(( struct Cyc_List_List*) _check_null( r1))->tl){ struct _tuple8
_temp861; void* _temp862; void* _temp864; struct _tuple8* _temp859=( struct
_tuple8*)(( struct Cyc_List_List*) _check_null( r1))->hd; _temp861=* _temp859;
_LL865: _temp864= _temp861.f1; goto _LL863; _LL863: _temp862= _temp861.f2; goto
_LL860; _LL860: { int found= _temp864 ==( void*) Cyc_Absyn_HeapRgn;{ struct Cyc_List_List*
r2= rpo2; for( 0; r2 != 0? ! found: 0; r2=(( struct Cyc_List_List*) _check_null(
r2))->tl){ struct _tuple8 _temp868; void* _temp869; void* _temp871; struct
_tuple8* _temp866=( struct _tuple8*)(( struct Cyc_List_List*) _check_null( r2))->hd;
_temp868=* _temp866; _LL872: _temp871= _temp868.f1; goto _LL870; _LL870:
_temp869= _temp868.f2; goto _LL867; _LL867: if( Cyc_Tcutil_unify( _temp864,
_temp871)? Cyc_Tcutil_unify( _temp862, _temp869): 0){ found= 1; break;}}} if( !
found){ return 0;}}}} return 1;} static int Cyc_Tcutil_same_rgn_po( struct Cyc_List_List*
rpo1, struct Cyc_List_List* rpo2){ return Cyc_Tcutil_sub_rgnpo( rpo1, rpo2)? Cyc_Tcutil_sub_rgnpo(
rpo2, rpo1): 0;} struct _tuple11{ struct Cyc_Absyn_VarargInfo* f1; struct Cyc_Absyn_VarargInfo*
f2; } ; struct _tuple12{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ;
void Cyc_Tcutil_unify_it( void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2=
Cyc_Tcutil_compress( t2); if( t1 == t2){ return;}{ void* _temp873= t1; struct
Cyc_Core_Opt* _temp879; struct Cyc_Core_Opt* _temp881; struct Cyc_Core_Opt**
_temp883; struct Cyc_Core_Opt* _temp884; _LL875: if(( unsigned int) _temp873 > 4u?*((
int*) _temp873) == Cyc_Absyn_Evar: 0){ _LL885: _temp884=(( struct Cyc_Absyn_Evar_struct*)
_temp873)->f1; goto _LL882; _LL882: _temp881=(( struct Cyc_Absyn_Evar_struct*)
_temp873)->f2; _temp883=&(( struct Cyc_Absyn_Evar_struct*) _temp873)->f2; goto
_LL880; _LL880: _temp879=(( struct Cyc_Absyn_Evar_struct*) _temp873)->f4; goto
_LL876;} else{ goto _LL877;} _LL877: goto _LL878; _LL876: Cyc_Tcutil_occurs( t1,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp879))->v, t2);{
void* _temp886= Cyc_Tcutil_typ_kind( t2); if( Cyc_Tcutil_kind_leq( _temp886,(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp884))->v)){* _temp883=({ struct
Cyc_Core_Opt* _temp887=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp887->v=( void*) t2; _temp887;}); return;} else{{ void* _temp888= t2; struct
Cyc_Core_Opt* _temp896; struct Cyc_Core_Opt* _temp898; struct Cyc_Core_Opt**
_temp900; struct Cyc_Absyn_PtrInfo _temp902; _LL890: if(( unsigned int) _temp888
> 4u?*(( int*) _temp888) == Cyc_Absyn_Evar: 0){ _LL899: _temp898=(( struct Cyc_Absyn_Evar_struct*)
_temp888)->f2; _temp900=&(( struct Cyc_Absyn_Evar_struct*) _temp888)->f2; goto
_LL897; _LL897: _temp896=(( struct Cyc_Absyn_Evar_struct*) _temp888)->f4; goto
_LL891;} else{ goto _LL892;} _LL892: if(( unsigned int) _temp888 > 4u?*(( int*)
_temp888) == Cyc_Absyn_PointerType: 0){ _LL903: _temp902=(( struct Cyc_Absyn_PointerType_struct*)
_temp888)->f1; goto _LL901;} else{ goto _LL894;} _LL901: if(( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp884))->v ==( void*) Cyc_Absyn_BoxKind){ goto
_LL893;} else{ goto _LL894;} _LL894: goto _LL895; _LL891: { struct Cyc_List_List*
_temp904=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp879))->v;{
struct Cyc_List_List* s2=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp896))->v; for( 0; s2 != 0; s2=(( struct Cyc_List_List*)
_check_null( s2))->tl){ if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, _temp904,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( s2))->hd)){( int) _throw(( void*) Cyc_Tcutil_Unify);}}} if( Cyc_Tcutil_kind_leq((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp884))->v, _temp886)){* _temp900=({
struct Cyc_Core_Opt* _temp905=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp905->v=( void*) t1; _temp905;}); return;} goto _LL889;}
_LL893: { struct Cyc_Absyn_Conref* _temp906= Cyc_Absyn_compress_conref( _temp902.bounds);{
void* _temp907=( void*) _temp906->v; _LL909: if( _temp907 ==( void*) Cyc_Absyn_No_constr){
goto _LL910;} else{ goto _LL911;} _LL911: goto _LL912; _LL910:( void*)( _temp906->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp913=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp913[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp914; _temp914.tag= Cyc_Absyn_Eq_constr; _temp914.f1=(
void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp915=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp915[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp916; _temp916.tag= Cyc_Absyn_Upper_b; _temp916.f1= Cyc_Absyn_signed_int_exp(
1, 0); _temp916;}); _temp915;})); _temp914;}); _temp913;})));* _temp883=({
struct Cyc_Core_Opt* _temp917=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp917->v=( void*) t2; _temp917;}); return; _LL912: goto
_LL908; _LL908:;} goto _LL889;} _LL895: goto _LL889; _LL889:;}( int) _throw((
void*) Cyc_Tcutil_Unify);}} _LL878: goto _LL874; _LL874:;}{ struct _tuple8
_temp919=({ struct _tuple8 _temp918; _temp918.f1= t2; _temp918.f2= t1; _temp918;});
void* _temp969; void* _temp971; void* _temp973; void* _temp975; struct Cyc_Absyn_Tvar*
_temp977; void* _temp979; struct Cyc_Absyn_Tvar* _temp981; void* _temp983;
struct Cyc_Absyn_Structdecl** _temp985; struct Cyc_List_List* _temp987; struct
_tuple1* _temp989; void* _temp991; struct Cyc_Absyn_Structdecl** _temp993;
struct Cyc_List_List* _temp995; struct _tuple1* _temp997; void* _temp999; struct
_tuple1* _temp1001; void* _temp1003; struct _tuple1* _temp1005; void* _temp1007;
struct Cyc_Absyn_Uniondecl** _temp1009; struct Cyc_List_List* _temp1011; struct
_tuple1* _temp1013; void* _temp1015; struct Cyc_Absyn_Uniondecl** _temp1017;
struct Cyc_List_List* _temp1019; struct _tuple1* _temp1021; void* _temp1023;
struct Cyc_Absyn_TunionInfo _temp1025; void* _temp1027; struct Cyc_List_List*
_temp1029; void* _temp1031; struct Cyc_Absyn_Tuniondecl* _temp1033; void*
_temp1035; struct Cyc_Absyn_TunionInfo _temp1037; void* _temp1039; struct Cyc_List_List*
_temp1041; void* _temp1043; struct Cyc_Absyn_Tuniondecl* _temp1045; void*
_temp1047; struct Cyc_Absyn_TunionFieldInfo _temp1049; struct Cyc_List_List*
_temp1051; void* _temp1053; struct Cyc_Absyn_Tunionfield* _temp1055; struct Cyc_Absyn_Tuniondecl*
_temp1057; void* _temp1059; struct Cyc_Absyn_TunionFieldInfo _temp1061; struct
Cyc_List_List* _temp1063; void* _temp1065; struct Cyc_Absyn_Tunionfield*
_temp1067; struct Cyc_Absyn_Tuniondecl* _temp1069; void* _temp1071; struct Cyc_Absyn_PtrInfo
_temp1073; struct Cyc_Absyn_Conref* _temp1075; struct Cyc_Absyn_Tqual _temp1077;
struct Cyc_Absyn_Conref* _temp1079; void* _temp1081; void* _temp1083; void*
_temp1085; struct Cyc_Absyn_PtrInfo _temp1087; struct Cyc_Absyn_Conref*
_temp1089; struct Cyc_Absyn_Tqual _temp1091; struct Cyc_Absyn_Conref* _temp1093;
void* _temp1095; void* _temp1097; void* _temp1099; void* _temp1101; void*
_temp1103; void* _temp1105; void* _temp1107; void* _temp1109; void* _temp1111;
void* _temp1113; void* _temp1115; void* _temp1117; void* _temp1119; struct Cyc_Absyn_Exp*
_temp1121; struct Cyc_Absyn_Tqual _temp1123; void* _temp1125; void* _temp1127;
struct Cyc_Absyn_Exp* _temp1129; struct Cyc_Absyn_Tqual _temp1131; void*
_temp1133; void* _temp1135; struct Cyc_Absyn_FnInfo _temp1137; struct Cyc_List_List*
_temp1139; struct Cyc_List_List* _temp1141; struct Cyc_Absyn_VarargInfo*
_temp1143; int _temp1145; struct Cyc_List_List* _temp1147; void* _temp1149;
struct Cyc_Core_Opt* _temp1151; struct Cyc_List_List* _temp1153; void* _temp1155;
struct Cyc_Absyn_FnInfo _temp1157; struct Cyc_List_List* _temp1159; struct Cyc_List_List*
_temp1161; struct Cyc_Absyn_VarargInfo* _temp1163; int _temp1165; struct Cyc_List_List*
_temp1167; void* _temp1169; struct Cyc_Core_Opt* _temp1171; struct Cyc_List_List*
_temp1173; void* _temp1175; struct Cyc_List_List* _temp1177; void* _temp1179;
struct Cyc_List_List* _temp1181; void* _temp1183; struct Cyc_List_List*
_temp1185; void* _temp1187; struct Cyc_List_List* _temp1189; void* _temp1191;
struct Cyc_List_List* _temp1193; void* _temp1195; struct Cyc_List_List*
_temp1197; void* _temp1199; void* _temp1201; void* _temp1203; void* _temp1205;
void* _temp1207; void* _temp1209; void* _temp1211; void* _temp1213; void*
_temp1215; void* _temp1217; _LL921: _LL970: _temp969= _temp919.f1; if((
unsigned int) _temp969 > 4u?*(( int*) _temp969) == Cyc_Absyn_Evar: 0){ goto
_LL922;} else{ goto _LL923;} _LL923: _LL974: _temp973= _temp919.f1; if( _temp973
==( void*) Cyc_Absyn_VoidType){ goto _LL972;} else{ goto _LL925;} _LL972:
_temp971= _temp919.f2; if( _temp971 ==( void*) Cyc_Absyn_VoidType){ goto _LL924;}
else{ goto _LL925;} _LL925: _LL980: _temp979= _temp919.f1; if(( unsigned int)
_temp979 > 4u?*(( int*) _temp979) == Cyc_Absyn_VarType: 0){ _LL982: _temp981=((
struct Cyc_Absyn_VarType_struct*) _temp979)->f1; goto _LL976;} else{ goto _LL927;}
_LL976: _temp975= _temp919.f2; if(( unsigned int) _temp975 > 4u?*(( int*)
_temp975) == Cyc_Absyn_VarType: 0){ _LL978: _temp977=(( struct Cyc_Absyn_VarType_struct*)
_temp975)->f1; goto _LL926;} else{ goto _LL927;} _LL927: _LL992: _temp991=
_temp919.f1; if(( unsigned int) _temp991 > 4u?*(( int*) _temp991) == Cyc_Absyn_StructType:
0){ _LL998: _temp997=(( struct Cyc_Absyn_StructType_struct*) _temp991)->f1; goto
_LL996; _LL996: _temp995=(( struct Cyc_Absyn_StructType_struct*) _temp991)->f2;
goto _LL994; _LL994: _temp993=(( struct Cyc_Absyn_StructType_struct*) _temp991)->f3;
goto _LL984;} else{ goto _LL929;} _LL984: _temp983= _temp919.f2; if((
unsigned int) _temp983 > 4u?*(( int*) _temp983) == Cyc_Absyn_StructType: 0){
_LL990: _temp989=(( struct Cyc_Absyn_StructType_struct*) _temp983)->f1; goto
_LL988; _LL988: _temp987=(( struct Cyc_Absyn_StructType_struct*) _temp983)->f2;
goto _LL986; _LL986: _temp985=(( struct Cyc_Absyn_StructType_struct*) _temp983)->f3;
goto _LL928;} else{ goto _LL929;} _LL929: _LL1004: _temp1003= _temp919.f1; if((
unsigned int) _temp1003 > 4u?*(( int*) _temp1003) == Cyc_Absyn_EnumType: 0){
_LL1006: _temp1005=(( struct Cyc_Absyn_EnumType_struct*) _temp1003)->f1; goto
_LL1000;} else{ goto _LL931;} _LL1000: _temp999= _temp919.f2; if(( unsigned int)
_temp999 > 4u?*(( int*) _temp999) == Cyc_Absyn_EnumType: 0){ _LL1002: _temp1001=((
struct Cyc_Absyn_EnumType_struct*) _temp999)->f1; goto _LL930;} else{ goto
_LL931;} _LL931: _LL1016: _temp1015= _temp919.f1; if(( unsigned int) _temp1015 >
4u?*(( int*) _temp1015) == Cyc_Absyn_UnionType: 0){ _LL1022: _temp1021=(( struct
Cyc_Absyn_UnionType_struct*) _temp1015)->f1; goto _LL1020; _LL1020: _temp1019=((
struct Cyc_Absyn_UnionType_struct*) _temp1015)->f2; goto _LL1018; _LL1018:
_temp1017=(( struct Cyc_Absyn_UnionType_struct*) _temp1015)->f3; goto _LL1008;}
else{ goto _LL933;} _LL1008: _temp1007= _temp919.f2; if(( unsigned int)
_temp1007 > 4u?*(( int*) _temp1007) == Cyc_Absyn_UnionType: 0){ _LL1014:
_temp1013=(( struct Cyc_Absyn_UnionType_struct*) _temp1007)->f1; goto _LL1012;
_LL1012: _temp1011=(( struct Cyc_Absyn_UnionType_struct*) _temp1007)->f2; goto
_LL1010; _LL1010: _temp1009=(( struct Cyc_Absyn_UnionType_struct*) _temp1007)->f3;
goto _LL932;} else{ goto _LL933;} _LL933: _LL1036: _temp1035= _temp919.f1; if((
unsigned int) _temp1035 > 4u?*(( int*) _temp1035) == Cyc_Absyn_TunionType: 0){
_LL1038: _temp1037=(( struct Cyc_Absyn_TunionType_struct*) _temp1035)->f1;
_LL1044: _temp1043=( void*) _temp1037.tunion_info; if(*(( int*) _temp1043) ==
Cyc_Absyn_KnownTunion){ _LL1046: _temp1045=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1043)->f1; goto _LL1042;} else{ goto _LL935;} _LL1042: _temp1041= _temp1037.targs;
goto _LL1040; _LL1040: _temp1039=( void*) _temp1037.rgn; goto _LL1024;} else{
goto _LL935;} _LL1024: _temp1023= _temp919.f2; if(( unsigned int) _temp1023 > 4u?*((
int*) _temp1023) == Cyc_Absyn_TunionType: 0){ _LL1026: _temp1025=(( struct Cyc_Absyn_TunionType_struct*)
_temp1023)->f1; _LL1032: _temp1031=( void*) _temp1025.tunion_info; if(*(( int*)
_temp1031) == Cyc_Absyn_KnownTunion){ _LL1034: _temp1033=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1031)->f1; goto _LL1030;} else{ goto _LL935;} _LL1030: _temp1029= _temp1025.targs;
goto _LL1028; _LL1028: _temp1027=( void*) _temp1025.rgn; goto _LL934;} else{
goto _LL935;} _LL935: _LL1060: _temp1059= _temp919.f1; if(( unsigned int)
_temp1059 > 4u?*(( int*) _temp1059) == Cyc_Absyn_TunionFieldType: 0){ _LL1062:
_temp1061=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1059)->f1; _LL1066:
_temp1065=( void*) _temp1061.field_info; if(*(( int*) _temp1065) == Cyc_Absyn_KnownTunionfield){
_LL1070: _temp1069=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1065)->f1;
goto _LL1068; _LL1068: _temp1067=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1065)->f2; goto _LL1064;} else{ goto _LL937;} _LL1064: _temp1063= _temp1061.targs;
goto _LL1048;} else{ goto _LL937;} _LL1048: _temp1047= _temp919.f2; if((
unsigned int) _temp1047 > 4u?*(( int*) _temp1047) == Cyc_Absyn_TunionFieldType:
0){ _LL1050: _temp1049=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1047)->f1;
_LL1054: _temp1053=( void*) _temp1049.field_info; if(*(( int*) _temp1053) == Cyc_Absyn_KnownTunionfield){
_LL1058: _temp1057=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1053)->f1;
goto _LL1056; _LL1056: _temp1055=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1053)->f2; goto _LL1052;} else{ goto _LL937;} _LL1052: _temp1051= _temp1049.targs;
goto _LL936;} else{ goto _LL937;} _LL937: _LL1086: _temp1085= _temp919.f1; if((
unsigned int) _temp1085 > 4u?*(( int*) _temp1085) == Cyc_Absyn_PointerType: 0){
_LL1088: _temp1087=(( struct Cyc_Absyn_PointerType_struct*) _temp1085)->f1;
_LL1098: _temp1097=( void*) _temp1087.elt_typ; goto _LL1096; _LL1096: _temp1095=(
void*) _temp1087.rgn_typ; goto _LL1094; _LL1094: _temp1093= _temp1087.nullable;
goto _LL1092; _LL1092: _temp1091= _temp1087.tq; goto _LL1090; _LL1090: _temp1089=
_temp1087.bounds; goto _LL1072;} else{ goto _LL939;} _LL1072: _temp1071=
_temp919.f2; if(( unsigned int) _temp1071 > 4u?*(( int*) _temp1071) == Cyc_Absyn_PointerType:
0){ _LL1074: _temp1073=(( struct Cyc_Absyn_PointerType_struct*) _temp1071)->f1;
_LL1084: _temp1083=( void*) _temp1073.elt_typ; goto _LL1082; _LL1082: _temp1081=(
void*) _temp1073.rgn_typ; goto _LL1080; _LL1080: _temp1079= _temp1073.nullable;
goto _LL1078; _LL1078: _temp1077= _temp1073.tq; goto _LL1076; _LL1076: _temp1075=
_temp1073.bounds; goto _LL938;} else{ goto _LL939;} _LL939: _LL1106: _temp1105=
_temp919.f1; if(( unsigned int) _temp1105 > 4u?*(( int*) _temp1105) == Cyc_Absyn_IntType:
0){ _LL1110: _temp1109=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1105)->f1;
goto _LL1108; _LL1108: _temp1107=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1105)->f2; goto _LL1100;} else{ goto _LL941;} _LL1100: _temp1099= _temp919.f2;
if(( unsigned int) _temp1099 > 4u?*(( int*) _temp1099) == Cyc_Absyn_IntType: 0){
_LL1104: _temp1103=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1099)->f1;
goto _LL1102; _LL1102: _temp1101=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1099)->f2; goto _LL940;} else{ goto _LL941;} _LL941: _LL1114: _temp1113=
_temp919.f1; if( _temp1113 ==( void*) Cyc_Absyn_FloatType){ goto _LL1112;} else{
goto _LL943;} _LL1112: _temp1111= _temp919.f2; if( _temp1111 ==( void*) Cyc_Absyn_FloatType){
goto _LL942;} else{ goto _LL943;} _LL943: _LL1118: _temp1117= _temp919.f1; if(
_temp1117 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1116;} else{ goto _LL945;}
_LL1116: _temp1115= _temp919.f2; if( _temp1115 ==( void*) Cyc_Absyn_DoubleType){
goto _LL944;} else{ goto _LL945;} _LL945: _LL1128: _temp1127= _temp919.f1; if((
unsigned int) _temp1127 > 4u?*(( int*) _temp1127) == Cyc_Absyn_ArrayType: 0){
_LL1134: _temp1133=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1127)->f1;
goto _LL1132; _LL1132: _temp1131=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1127)->f2; goto _LL1130; _LL1130: _temp1129=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1127)->f3; goto _LL1120;} else{ goto _LL947;} _LL1120: _temp1119= _temp919.f2;
if(( unsigned int) _temp1119 > 4u?*(( int*) _temp1119) == Cyc_Absyn_ArrayType: 0){
_LL1126: _temp1125=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1119)->f1;
goto _LL1124; _LL1124: _temp1123=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1119)->f2; goto _LL1122; _LL1122: _temp1121=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1119)->f3; goto _LL946;} else{ goto _LL947;} _LL947: _LL1156: _temp1155=
_temp919.f1; if(( unsigned int) _temp1155 > 4u?*(( int*) _temp1155) == Cyc_Absyn_FnType:
0){ _LL1158: _temp1157=(( struct Cyc_Absyn_FnType_struct*) _temp1155)->f1;
_LL1174: _temp1173= _temp1157.tvars; goto _LL1172; _LL1172: _temp1171= _temp1157.effect;
goto _LL1170; _LL1170: _temp1169=( void*) _temp1157.ret_typ; goto _LL1168;
_LL1168: _temp1167= _temp1157.args; goto _LL1166; _LL1166: _temp1165= _temp1157.c_varargs;
goto _LL1164; _LL1164: _temp1163= _temp1157.cyc_varargs; goto _LL1162; _LL1162:
_temp1161= _temp1157.rgn_po; goto _LL1160; _LL1160: _temp1159= _temp1157.attributes;
goto _LL1136;} else{ goto _LL949;} _LL1136: _temp1135= _temp919.f2; if((
unsigned int) _temp1135 > 4u?*(( int*) _temp1135) == Cyc_Absyn_FnType: 0){
_LL1138: _temp1137=(( struct Cyc_Absyn_FnType_struct*) _temp1135)->f1; _LL1154:
_temp1153= _temp1137.tvars; goto _LL1152; _LL1152: _temp1151= _temp1137.effect;
goto _LL1150; _LL1150: _temp1149=( void*) _temp1137.ret_typ; goto _LL1148;
_LL1148: _temp1147= _temp1137.args; goto _LL1146; _LL1146: _temp1145= _temp1137.c_varargs;
goto _LL1144; _LL1144: _temp1143= _temp1137.cyc_varargs; goto _LL1142; _LL1142:
_temp1141= _temp1137.rgn_po; goto _LL1140; _LL1140: _temp1139= _temp1137.attributes;
goto _LL948;} else{ goto _LL949;} _LL949: _LL1180: _temp1179= _temp919.f1; if((
unsigned int) _temp1179 > 4u?*(( int*) _temp1179) == Cyc_Absyn_TupleType: 0){
_LL1182: _temp1181=(( struct Cyc_Absyn_TupleType_struct*) _temp1179)->f1; goto
_LL1176;} else{ goto _LL951;} _LL1176: _temp1175= _temp919.f2; if(( unsigned int)
_temp1175 > 4u?*(( int*) _temp1175) == Cyc_Absyn_TupleType: 0){ _LL1178:
_temp1177=(( struct Cyc_Absyn_TupleType_struct*) _temp1175)->f1; goto _LL950;}
else{ goto _LL951;} _LL951: _LL1188: _temp1187= _temp919.f1; if(( unsigned int)
_temp1187 > 4u?*(( int*) _temp1187) == Cyc_Absyn_AnonStructType: 0){ _LL1190:
_temp1189=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1187)->f1; goto
_LL1184;} else{ goto _LL953;} _LL1184: _temp1183= _temp919.f2; if(( unsigned int)
_temp1183 > 4u?*(( int*) _temp1183) == Cyc_Absyn_AnonStructType: 0){ _LL1186:
_temp1185=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1183)->f1; goto
_LL952;} else{ goto _LL953;} _LL953: _LL1196: _temp1195= _temp919.f1; if((
unsigned int) _temp1195 > 4u?*(( int*) _temp1195) == Cyc_Absyn_AnonUnionType: 0){
_LL1198: _temp1197=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1195)->f1;
goto _LL1192;} else{ goto _LL955;} _LL1192: _temp1191= _temp919.f2; if((
unsigned int) _temp1191 > 4u?*(( int*) _temp1191) == Cyc_Absyn_AnonUnionType: 0){
_LL1194: _temp1193=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1191)->f1;
goto _LL954;} else{ goto _LL955;} _LL955: _LL1202: _temp1201= _temp919.f1; if(
_temp1201 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1200;} else{ goto _LL957;}
_LL1200: _temp1199= _temp919.f2; if( _temp1199 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL956;} else{ goto _LL957;} _LL957: _LL1208: _temp1207= _temp919.f1; if((
unsigned int) _temp1207 > 4u?*(( int*) _temp1207) == Cyc_Absyn_RgnHandleType: 0){
_LL1210: _temp1209=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1207)->f1;
goto _LL1204;} else{ goto _LL959;} _LL1204: _temp1203= _temp919.f2; if((
unsigned int) _temp1203 > 4u?*(( int*) _temp1203) == Cyc_Absyn_RgnHandleType: 0){
_LL1206: _temp1205=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1203)->f1;
goto _LL958;} else{ goto _LL959;} _LL959: _LL1212: _temp1211= _temp919.f1; if((
unsigned int) _temp1211 > 4u?*(( int*) _temp1211) == Cyc_Absyn_JoinEff: 0){ goto
_LL960;} else{ goto _LL961;} _LL961: _LL1214: _temp1213= _temp919.f2; if((
unsigned int) _temp1213 > 4u?*(( int*) _temp1213) == Cyc_Absyn_JoinEff: 0){ goto
_LL962;} else{ goto _LL963;} _LL963: _LL1216: _temp1215= _temp919.f1; if((
unsigned int) _temp1215 > 4u?*(( int*) _temp1215) == Cyc_Absyn_AccessEff: 0){
goto _LL964;} else{ goto _LL965;} _LL965: _LL1218: _temp1217= _temp919.f2; if((
unsigned int) _temp1217 > 4u?*(( int*) _temp1217) == Cyc_Absyn_AccessEff: 0){
goto _LL966;} else{ goto _LL967;} _LL967: goto _LL968; _LL922: Cyc_Tcutil_unify_it(
t2, t1); return; _LL924: return; _LL926: { struct _tagged_arr* _temp1219=
_temp981->name; struct _tagged_arr* _temp1220= _temp977->name; int _temp1221=*((
int*) _check_null( _temp981->identity)); int _temp1222=*(( int*) _check_null(
_temp977->identity)); void* _temp1223= Cyc_Absyn_conref_val( _temp981->kind);
void* _temp1224= Cyc_Absyn_conref_val( _temp977->kind); if( _temp1222 ==
_temp1221? Cyc_String_zstrptrcmp( _temp1219, _temp1220) == 0: 0){ if( _temp1223
!= _temp1224){(( int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)(( struct
_tagged_arr)({ struct _tagged_arr(* _temp1225)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp1231= _tag_arr("same type variable %s has kinds %s and %s",
sizeof( unsigned char), 42u); struct Cyc_Stdio_String_pa_struct _temp1234;
_temp1234.tag= Cyc_Stdio_String_pa; _temp1234.f1=( struct _tagged_arr)*
_temp1219;{ void* _temp1228=( void*)& _temp1234; struct Cyc_Stdio_String_pa_struct
_temp1233; _temp1233.tag= Cyc_Stdio_String_pa; _temp1233.f1=( struct _tagged_arr)
Cyc_Absynpp_kind2string( _temp1223);{ void* _temp1229=( void*)& _temp1233;
struct Cyc_Stdio_String_pa_struct _temp1232; _temp1232.tag= Cyc_Stdio_String_pa;
_temp1232.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( _temp1224);{ void*
_temp1230=( void*)& _temp1232; void* _temp1226[ 3u]={ _temp1228, _temp1229,
_temp1230}; struct _tagged_arr _temp1227={( void*) _temp1226,( void*) _temp1226,(
void*)( _temp1226 + 3u)}; _temp1225( _temp1231, _temp1227);}}}}));} return;}
goto _LL920;} _LL928: if((( _temp989 != 0? _temp997 != 0: 0)? Cyc_Absyn_qvar_cmp((
struct _tuple1*) _check_null( _temp989),( struct _tuple1*) _check_null( _temp997))
== 0: 0)? 1:( _temp989 == 0? _temp997 == 0: 0)){ Cyc_Tcutil_unify_list( _temp987,
_temp995); return;} goto _LL920; _LL930: if( Cyc_Absyn_qvar_cmp( _temp1005,
_temp1001) == 0){ return;} goto _LL920; _LL932: if((( _temp1013 != 0? _temp1021
!= 0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*) _check_null( _temp1013),( struct
_tuple1*) _check_null( _temp1021)) == 0: 0)? 1:( _temp1013 == 0? _temp1021 == 0:
0)){ Cyc_Tcutil_unify_list( _temp1011, _temp1019); return;} goto _LL920; _LL934:
if( _temp1045 == _temp1033? 1: Cyc_Absyn_qvar_cmp( _temp1045->name, _temp1033->name)
== 0){ Cyc_Tcutil_unify_it( _temp1027, _temp1039); Cyc_Tcutil_unify_list(
_temp1029, _temp1041); return;} goto _LL920; _LL936: if(( _temp1069 == _temp1057?
1: Cyc_Absyn_qvar_cmp( _temp1069->name, _temp1057->name) == 0)? _temp1067 ==
_temp1055? 1: Cyc_Absyn_qvar_cmp( _temp1067->name, _temp1055->name) == 0: 0){
Cyc_Tcutil_unify_list( _temp1051, _temp1063); return;} goto _LL920; _LL938: Cyc_Tcutil_unify_it(
_temp1083, _temp1097); Cyc_Tcutil_unify_it( _temp1095, _temp1081); Cyc_Tcutil_unify_tqual(
_temp1077, _temp1091); Cyc_Tcutil_unify_it_conrefs( Cyc_Tcutil_boundscmp,
_temp1075, _temp1089);{ void* _temp1235=( void*)( Cyc_Absyn_compress_conref(
_temp1075))->v; void* _temp1241; _LL1237: if(( unsigned int) _temp1235 > 1u?*((
int*) _temp1235) == Cyc_Absyn_Eq_constr: 0){ _LL1242: _temp1241=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1235)->f1; if( _temp1241 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1238;} else{ goto _LL1239;}} else{ goto _LL1239;} _LL1239: goto _LL1240;
_LL1238: return; _LL1240: goto _LL1236; _LL1236:;}(( void(*)( int(* cmp)( int,
int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp, _temp1079, _temp1093); return; _LL940: if( _temp1103 ==
_temp1109? _temp1101 == _temp1107: 0){ return;} goto _LL920; _LL942: return;
_LL944: return; _LL946: Cyc_Tcutil_unify_it( _temp1125, _temp1133); Cyc_Tcutil_unify_tqual(
_temp1123, _temp1131); if( _temp1129 == _temp1121){ return;} if( _temp1129 == 0?
1: _temp1121 == 0){ goto _LL920;} if( Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1129)) == Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1121))){ return;} goto _LL920; _LL948: { int done= 0;{ struct
_RegionHandle _temp1243= _new_region(); struct _RegionHandle* rgn=& _temp1243;
_push_region( rgn);{ struct Cyc_List_List* inst= 0; while( _temp1153 != 0) { if(
_temp1173 == 0){ break;} inst=({ struct Cyc_List_List* _temp1244=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1244->hd=( void*)({
struct _tuple5* _temp1245=( struct _tuple5*) _region_malloc( rgn, sizeof( struct
_tuple5)); _temp1245->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp1173))->hd; _temp1245->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1246=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1246[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1247; _temp1247.tag= Cyc_Absyn_VarType;
_temp1247.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp1153))->hd; _temp1247;}); _temp1246;}); _temp1245;}); _temp1244->tl= inst;
_temp1244;}); _temp1153=(( struct Cyc_List_List*) _check_null( _temp1153))->tl;
_temp1173=(( struct Cyc_List_List*) _check_null( _temp1173))->tl;} if( _temp1173
!= 0){ _npop_handler( 0u); goto _LL920;} if( inst != 0){ Cyc_Tcutil_unify_it((
void*)({ struct Cyc_Absyn_FnType_struct* _temp1248=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1248[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1249; _temp1249.tag= Cyc_Absyn_FnType; _temp1249.f1=({ struct Cyc_Absyn_FnInfo
_temp1250; _temp1250.tvars= 0; _temp1250.effect= _temp1151; _temp1250.ret_typ=(
void*) _temp1149; _temp1250.args= _temp1147; _temp1250.c_varargs= _temp1145;
_temp1250.cyc_varargs= _temp1143; _temp1250.rgn_po= _temp1141; _temp1250.attributes=
_temp1139; _temp1250;}); _temp1249;}); _temp1248;}), Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1251=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1251[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1252; _temp1252.tag= Cyc_Absyn_FnType; _temp1252.f1=({ struct Cyc_Absyn_FnInfo
_temp1253; _temp1253.tvars= 0; _temp1253.effect= _temp1171; _temp1253.ret_typ=(
void*) _temp1169; _temp1253.args= _temp1167; _temp1253.c_varargs= _temp1165;
_temp1253.cyc_varargs= _temp1163; _temp1253.rgn_po= _temp1161; _temp1253.attributes=
_temp1159; _temp1253;}); _temp1252;}); _temp1251;}))); done= 1;}}; _pop_region(
rgn);} if( done){ return;} Cyc_Tcutil_unify_it( _temp1149, _temp1169); for( 0;
_temp1147 != 0? _temp1167 != 0: 0;( _temp1147=(( struct Cyc_List_List*)
_check_null( _temp1147))->tl, _temp1167=(( struct Cyc_List_List*) _check_null(
_temp1167))->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp1147))->hd)).f2,(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp1167))->hd)).f2); Cyc_Tcutil_unify_it((*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp1147))->hd)).f3,(*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp1167))->hd)).f3);} if( _temp1147 != 0?
1: _temp1167 != 0){ goto _LL920;} if( _temp1145 != _temp1165){ goto _LL920;}{
int bad_cyc_vararg= 0;{ struct _tuple11 _temp1255=({ struct _tuple11 _temp1254;
_temp1254.f1= _temp1143; _temp1254.f2= _temp1163; _temp1254;}); struct Cyc_Absyn_VarargInfo*
_temp1265; struct Cyc_Absyn_VarargInfo* _temp1267; struct Cyc_Absyn_VarargInfo*
_temp1269; struct Cyc_Absyn_VarargInfo* _temp1271; struct Cyc_Absyn_VarargInfo*
_temp1273; struct Cyc_Absyn_VarargInfo _temp1275; int _temp1276; void* _temp1278;
void* _temp1280; struct Cyc_Absyn_Tqual _temp1282; struct Cyc_Core_Opt*
_temp1284; struct Cyc_Absyn_VarargInfo* _temp1286; struct Cyc_Absyn_VarargInfo
_temp1288; int _temp1289; void* _temp1291; void* _temp1293; struct Cyc_Absyn_Tqual
_temp1295; struct Cyc_Core_Opt* _temp1297; _LL1257: _LL1268: _temp1267=
_temp1255.f1; if( _temp1267 == 0){ goto _LL1266;} else{ goto _LL1259;} _LL1266:
_temp1265= _temp1255.f2; if( _temp1265 == 0){ goto _LL1258;} else{ goto _LL1259;}
_LL1259: _LL1270: _temp1269= _temp1255.f1; if( _temp1269 == 0){ goto _LL1260;}
else{ goto _LL1261;} _LL1261: _LL1272: _temp1271= _temp1255.f2; if( _temp1271 ==
0){ goto _LL1262;} else{ goto _LL1263;} _LL1263: _LL1287: _temp1286= _temp1255.f1;
if( _temp1286 == 0){ goto _LL1256;} else{ _temp1288=* _temp1286; _LL1298:
_temp1297= _temp1288.name; goto _LL1296; _LL1296: _temp1295= _temp1288.tq; goto
_LL1294; _LL1294: _temp1293=( void*) _temp1288.type; goto _LL1292; _LL1292:
_temp1291=( void*) _temp1288.rgn; goto _LL1290; _LL1290: _temp1289= _temp1288.inject;
goto _LL1274;} _LL1274: _temp1273= _temp1255.f2; if( _temp1273 == 0){ goto
_LL1256;} else{ _temp1275=* _temp1273; _LL1285: _temp1284= _temp1275.name; goto
_LL1283; _LL1283: _temp1282= _temp1275.tq; goto _LL1281; _LL1281: _temp1280=(
void*) _temp1275.type; goto _LL1279; _LL1279: _temp1278=( void*) _temp1275.rgn;
goto _LL1277; _LL1277: _temp1276= _temp1275.inject; goto _LL1264;} _LL1258: goto
_LL1256; _LL1260: goto _LL1262; _LL1262: bad_cyc_vararg= 1; goto _LL1256;
_LL1264: Cyc_Tcutil_unify_tqual( _temp1295, _temp1282); Cyc_Tcutil_unify_it(
_temp1293, _temp1280); Cyc_Tcutil_unify_it( _temp1291, _temp1278); if( _temp1289
!= _temp1276){ bad_cyc_vararg= 1;} goto _LL1256; _LL1256:;} if( bad_cyc_vararg){
goto _LL920;}{ int bad_effect= 0;{ struct _tuple12 _temp1300=({ struct _tuple12
_temp1299; _temp1299.f1= _temp1151; _temp1299.f2= _temp1171; _temp1299;});
struct Cyc_Core_Opt* _temp1310; struct Cyc_Core_Opt* _temp1312; struct Cyc_Core_Opt*
_temp1314; struct Cyc_Core_Opt* _temp1316; _LL1302: _LL1313: _temp1312=
_temp1300.f1; if( _temp1312 == 0){ goto _LL1311;} else{ goto _LL1304;} _LL1311:
_temp1310= _temp1300.f2; if( _temp1310 == 0){ goto _LL1303;} else{ goto _LL1304;}
_LL1304: _LL1315: _temp1314= _temp1300.f1; if( _temp1314 == 0){ goto _LL1305;}
else{ goto _LL1306;} _LL1306: _LL1317: _temp1316= _temp1300.f2; if( _temp1316 ==
0){ goto _LL1307;} else{ goto _LL1308;} _LL1308: goto _LL1309; _LL1303: goto
_LL1301; _LL1305: goto _LL1307; _LL1307: bad_effect= 1; goto _LL1301; _LL1309:
bad_effect= ! Cyc_Tcutil_unify_effect(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1151))->v,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1171))->v); goto _LL1301; _LL1301:;} if( bad_effect){ goto _LL920;} if( !
Cyc_Tcutil_same_atts( _temp1159, _temp1139)){ goto _LL920;} if( ! Cyc_Tcutil_same_rgn_po(
_temp1161, _temp1141)){ goto _LL920;} return;}}} _LL950: for( 0; _temp1177 != 0?
_temp1181 != 0: 0;( _temp1177=(( struct Cyc_List_List*) _check_null( _temp1177))->tl,
_temp1181=(( struct Cyc_List_List*) _check_null( _temp1181))->tl)){ Cyc_Tcutil_unify_tqual((*((
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1177))->hd)).f1,(*((
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1181))->hd)).f1);
Cyc_Tcutil_unify_it((*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp1177))->hd)).f2,(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp1181))->hd)).f2);} if( _temp1177 == 0? _temp1181 == 0: 0){ return;} goto
_LL920; _LL952: _temp1197= _temp1189; _temp1193= _temp1185; goto _LL954; _LL954:
for( 0; _temp1193 != 0? _temp1197 != 0: 0;( _temp1193=(( struct Cyc_List_List*)
_check_null( _temp1193))->tl, _temp1197=(( struct Cyc_List_List*) _check_null(
_temp1197))->tl)){ struct Cyc_Absyn_Structfield* _temp1318=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp1193))->hd; struct Cyc_Absyn_Structfield*
_temp1319=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1197))->hd; if( Cyc_String_zstrptrcmp( _temp1318->name, _temp1319->name) !=
0){( int) _throw(( void*) Cyc_Tcutil_Unify);} Cyc_Tcutil_unify_tqual( _temp1318->tq,
_temp1319->tq); Cyc_Tcutil_unify_it(( void*) _temp1318->type,( void*) _temp1319->type);
if((( ! Cyc_Tcutil_same_atts( _temp1318->attributes, _temp1319->attributes)? 1:(
_temp1318->width != 0? _temp1319->width == 0: 0))? 1:( _temp1319->width != 0?
_temp1318->width == 0: 0))? 1:(( _temp1318->width != 0? _temp1319->width != 0: 0)?
Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*)
_check_null( _temp1318->width))->v) != Cyc_Evexp_eval_const_uint_exp(( struct
Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp1319->width))->v): 0)){(
int) _throw(( void*) Cyc_Tcutil_Unify);}} if( _temp1193 == 0? _temp1197 == 0: 0){
return;} goto _LL920; _LL956: return; _LL958: Cyc_Tcutil_unify_it( _temp1209,
_temp1205); return; _LL960: goto _LL962; _LL962: goto _LL964; _LL964: goto
_LL966; _LL966: if( Cyc_Tcutil_unify_effect( t1, t2)){ return;} else{ goto
_LL920;} _LL968: goto _LL920; _LL920:;}( int) _throw(( void*) Cyc_Tcutil_Unify);}
int Cyc_Tcutil_is_arithmetic_type( void* t){ void* _temp1320= Cyc_Tcutil_compress(
t); _LL1322: if(( unsigned int) _temp1320 > 4u?*(( int*) _temp1320) == Cyc_Absyn_IntType:
0){ goto _LL1323;} else{ goto _LL1324;} _LL1324: if( _temp1320 ==( void*) Cyc_Absyn_FloatType){
goto _LL1325;} else{ goto _LL1326;} _LL1326: if( _temp1320 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1327;} else{ goto _LL1328;} _LL1328: if(( unsigned int) _temp1320 > 4u?*((
int*) _temp1320) == Cyc_Absyn_EnumType: 0){ goto _LL1329;} else{ goto _LL1330;}
_LL1330: goto _LL1331; _LL1323: return 1; _LL1325: return 1; _LL1327: return 1;
_LL1329: return 1; _LL1331: return 0; _LL1321:;} int Cyc_Tcutil_will_lose_precision(
void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple8 _temp1333=({ struct _tuple8 _temp1332; _temp1332.f1= t1;
_temp1332.f2= t2; _temp1332;}); void* _temp1355; void* _temp1357; void*
_temp1359; void* _temp1361; void* _temp1363; void* _temp1365; void* _temp1367;
void* _temp1369; void* _temp1371; void* _temp1373; void* _temp1375; void*
_temp1377; void* _temp1379; void* _temp1381; void* _temp1383; void* _temp1385;
void* _temp1387; void* _temp1389; void* _temp1391; void* _temp1393; void*
_temp1395; void* _temp1397; void* _temp1399; void* _temp1401; void* _temp1403;
void* _temp1405; void* _temp1407; _LL1335: _LL1358: _temp1357= _temp1333.f1; if(
_temp1357 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1356;} else{ goto _LL1337;}
_LL1356: _temp1355= _temp1333.f2; if( _temp1355 ==( void*) Cyc_Absyn_FloatType){
goto _LL1336;} else{ goto _LL1337;} _LL1337: _LL1362: _temp1361= _temp1333.f1;
if( _temp1361 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1360;} else{ goto
_LL1339;} _LL1360: _temp1359= _temp1333.f2; if(( unsigned int) _temp1359 > 4u?*((
int*) _temp1359) == Cyc_Absyn_IntType: 0){ goto _LL1338;} else{ goto _LL1339;}
_LL1339: _LL1366: _temp1365= _temp1333.f1; if( _temp1365 ==( void*) Cyc_Absyn_FloatType){
goto _LL1364;} else{ goto _LL1341;} _LL1364: _temp1363= _temp1333.f2; if((
unsigned int) _temp1363 > 4u?*(( int*) _temp1363) == Cyc_Absyn_IntType: 0){ goto
_LL1340;} else{ goto _LL1341;} _LL1341: _LL1372: _temp1371= _temp1333.f1; if((
unsigned int) _temp1371 > 4u?*(( int*) _temp1371) == Cyc_Absyn_IntType: 0){
_LL1374: _temp1373=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1371)->f2;
if( _temp1373 ==( void*) Cyc_Absyn_B8){ goto _LL1368;} else{ goto _LL1343;}}
else{ goto _LL1343;} _LL1368: _temp1367= _temp1333.f2; if(( unsigned int)
_temp1367 > 4u?*(( int*) _temp1367) == Cyc_Absyn_IntType: 0){ _LL1370: _temp1369=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1367)->f2; if( _temp1369 ==(
void*) Cyc_Absyn_B8){ goto _LL1342;} else{ goto _LL1343;}} else{ goto _LL1343;}
_LL1343: _LL1376: _temp1375= _temp1333.f1; if(( unsigned int) _temp1375 > 4u?*((
int*) _temp1375) == Cyc_Absyn_IntType: 0){ _LL1378: _temp1377=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1375)->f2; if( _temp1377 ==( void*) Cyc_Absyn_B8){
goto _LL1344;} else{ goto _LL1345;}} else{ goto _LL1345;} _LL1345: _LL1382:
_temp1381= _temp1333.f1; if(( unsigned int) _temp1381 > 4u?*(( int*) _temp1381)
== Cyc_Absyn_IntType: 0){ _LL1384: _temp1383=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1381)->f2; if( _temp1383 ==( void*) Cyc_Absyn_B4){ goto _LL1380;} else{
goto _LL1347;}} else{ goto _LL1347;} _LL1380: _temp1379= _temp1333.f2; if(
_temp1379 ==( void*) Cyc_Absyn_FloatType){ goto _LL1346;} else{ goto _LL1347;}
_LL1347: _LL1390: _temp1389= _temp1333.f1; if(( unsigned int) _temp1389 > 4u?*((
int*) _temp1389) == Cyc_Absyn_IntType: 0){ _LL1392: _temp1391=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1389)->f2; if( _temp1391 ==( void*) Cyc_Absyn_B4){
goto _LL1386;} else{ goto _LL1349;}} else{ goto _LL1349;} _LL1386: _temp1385=
_temp1333.f2; if(( unsigned int) _temp1385 > 4u?*(( int*) _temp1385) == Cyc_Absyn_IntType:
0){ _LL1388: _temp1387=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1385)->f2;
if( _temp1387 ==( void*) Cyc_Absyn_B2){ goto _LL1348;} else{ goto _LL1349;}}
else{ goto _LL1349;} _LL1349: _LL1398: _temp1397= _temp1333.f1; if((
unsigned int) _temp1397 > 4u?*(( int*) _temp1397) == Cyc_Absyn_IntType: 0){
_LL1400: _temp1399=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1397)->f2;
if( _temp1399 ==( void*) Cyc_Absyn_B4){ goto _LL1394;} else{ goto _LL1351;}}
else{ goto _LL1351;} _LL1394: _temp1393= _temp1333.f2; if(( unsigned int)
_temp1393 > 4u?*(( int*) _temp1393) == Cyc_Absyn_IntType: 0){ _LL1396: _temp1395=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1393)->f2; if( _temp1395 ==(
void*) Cyc_Absyn_B1){ goto _LL1350;} else{ goto _LL1351;}} else{ goto _LL1351;}
_LL1351: _LL1406: _temp1405= _temp1333.f1; if(( unsigned int) _temp1405 > 4u?*((
int*) _temp1405) == Cyc_Absyn_IntType: 0){ _LL1408: _temp1407=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1405)->f2; if( _temp1407 ==( void*) Cyc_Absyn_B2){
goto _LL1402;} else{ goto _LL1353;}} else{ goto _LL1353;} _LL1402: _temp1401=
_temp1333.f2; if(( unsigned int) _temp1401 > 4u?*(( int*) _temp1401) == Cyc_Absyn_IntType:
0){ _LL1404: _temp1403=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1401)->f2;
if( _temp1403 ==( void*) Cyc_Absyn_B1){ goto _LL1352;} else{ goto _LL1353;}}
else{ goto _LL1353;} _LL1353: goto _LL1354; _LL1336: return 1; _LL1338: return 1;
_LL1340: return 1; _LL1342: return 0; _LL1344: return 1; _LL1346: return 1;
_LL1348: return 1; _LL1350: return 1; _LL1352: return 1; _LL1354: return 0;
_LL1334:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv* te, void* t,
struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{ struct Cyc_List_List*
el= es; for( 0; el != 0; el=(( struct Cyc_List_List*) _check_null( el))->tl){
void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);
if( Cyc_Tcutil_is_arithmetic_type( t1)){ if( max_arith_type == 0? 1: Cyc_Tcutil_will_lose_precision(
t1,( void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){
max_arith_type=({ struct Cyc_Core_Opt* _temp1409=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1409->v=( void*) t1; _temp1409;});}}}}
if( max_arith_type != 0){ if( ! Cyc_Tcutil_unify( t,( void*)(( struct Cyc_Core_Opt*)
_check_null( max_arith_type))->v)){ return 0;}}{ struct Cyc_List_List* el= es;
for( 0; el != 0; el=(( struct Cyc_List_List*) _check_null( el))->tl){ if( ! Cyc_Tcutil_coerce_assign(
te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd, t)){
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
el))->hd)->loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp1410)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp1415= _tag_arr("type mismatch: expecting %s but found %s", sizeof(
unsigned char), 41u); struct Cyc_Stdio_String_pa_struct _temp1417; _temp1417.tag=
Cyc_Stdio_String_pa; _temp1417.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp1413=( void*)& _temp1417; struct Cyc_Stdio_String_pa_struct
_temp1416; _temp1416.tag= Cyc_Stdio_String_pa; _temp1416.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null((( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);{
void* _temp1414=( void*)& _temp1416; void* _temp1411[ 2u]={ _temp1413, _temp1414};
struct _tagged_arr _temp1412={( void*) _temp1411,( void*) _temp1411,( void*)(
_temp1411 + 2u)}; _temp1410( _temp1415, _temp1412);}}})); return 0;}}} return 1;}
int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e){ if( ! Cyc_Tcutil_coerce_sint_typ( te, e)){ void* _temp1418= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _LL1420: if((
unsigned int) _temp1418 > 4u?*(( int*) _temp1418) == Cyc_Absyn_PointerType: 0){
goto _LL1421;} else{ goto _LL1422;} _LL1422: goto _LL1423; _LL1421: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); goto _LL1419; _LL1423: return 0; _LL1419:;} return 1;}
int Cyc_Tcutil_is_integral_type( void* t){ void* _temp1424= Cyc_Tcutil_compress(
t); _LL1426: if(( unsigned int) _temp1424 > 4u?*(( int*) _temp1424) == Cyc_Absyn_IntType:
0){ goto _LL1427;} else{ goto _LL1428;} _LL1428: if(( unsigned int) _temp1424 >
4u?*(( int*) _temp1424) == Cyc_Absyn_EnumType: 0){ goto _LL1429;} else{ goto
_LL1430;} _LL1430: goto _LL1431; _LL1427: return 1; _LL1429: return 1; _LL1431:
return 0; _LL1425:;} int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_uint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){ if( Cyc_Tcutil_will_lose_precision((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_uint_t)){
Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u));} Cyc_Tcutil_unchecked_cast( te, e, Cyc_Absyn_uint_t);
return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){ if( Cyc_Tcutil_will_lose_precision((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){
Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u));} Cyc_Tcutil_unchecked_cast( te, e, Cyc_Absyn_sint_t);
return 1;} return 0;} int Cyc_Tcutil_silent_castable( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8 _temp1433=({ struct _tuple8
_temp1432; _temp1432.f1= t1; _temp1432.f2= t2; _temp1432;}); void* _temp1445;
struct Cyc_Absyn_PtrInfo _temp1447; void* _temp1449; struct Cyc_Absyn_PtrInfo
_temp1451; void* _temp1453; struct Cyc_Absyn_Exp* _temp1455; struct Cyc_Absyn_Tqual
_temp1457; void* _temp1459; void* _temp1461; struct Cyc_Absyn_Exp* _temp1463;
struct Cyc_Absyn_Tqual _temp1465; void* _temp1467; void* _temp1469; struct Cyc_Absyn_TunionInfo
_temp1471; struct Cyc_List_List* _temp1473; void* _temp1475; struct Cyc_Absyn_Tuniondecl*
_temp1477; void* _temp1479; struct Cyc_Absyn_TunionFieldInfo _temp1481; struct
Cyc_List_List* _temp1483; void* _temp1485; struct Cyc_Absyn_Tunionfield*
_temp1487; struct Cyc_Absyn_Tuniondecl* _temp1489; void* _temp1491; struct Cyc_Absyn_TunionInfo
_temp1493; void* _temp1495; struct Cyc_List_List* _temp1497; void* _temp1499;
struct Cyc_Absyn_Tuniondecl* _temp1501; void* _temp1503; struct Cyc_Absyn_PtrInfo
_temp1505; struct Cyc_Absyn_Conref* _temp1507; struct Cyc_Absyn_Tqual _temp1509;
struct Cyc_Absyn_Conref* _temp1511; void* _temp1513; void* _temp1515; _LL1435:
_LL1450: _temp1449= _temp1433.f1; if(( unsigned int) _temp1449 > 4u?*(( int*)
_temp1449) == Cyc_Absyn_PointerType: 0){ _LL1452: _temp1451=(( struct Cyc_Absyn_PointerType_struct*)
_temp1449)->f1; goto _LL1446;} else{ goto _LL1437;} _LL1446: _temp1445=
_temp1433.f2; if(( unsigned int) _temp1445 > 4u?*(( int*) _temp1445) == Cyc_Absyn_PointerType:
0){ _LL1448: _temp1447=(( struct Cyc_Absyn_PointerType_struct*) _temp1445)->f1;
goto _LL1436;} else{ goto _LL1437;} _LL1437: _LL1462: _temp1461= _temp1433.f1;
if(( unsigned int) _temp1461 > 4u?*(( int*) _temp1461) == Cyc_Absyn_ArrayType: 0){
_LL1468: _temp1467=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1461)->f1;
goto _LL1466; _LL1466: _temp1465=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1461)->f2; goto _LL1464; _LL1464: _temp1463=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1461)->f3; goto _LL1454;} else{ goto _LL1439;} _LL1454: _temp1453=
_temp1433.f2; if(( unsigned int) _temp1453 > 4u?*(( int*) _temp1453) == Cyc_Absyn_ArrayType:
0){ _LL1460: _temp1459=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1453)->f1;
goto _LL1458; _LL1458: _temp1457=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1453)->f2; goto _LL1456; _LL1456: _temp1455=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1453)->f3; goto _LL1438;} else{ goto _LL1439;} _LL1439: _LL1480: _temp1479=
_temp1433.f1; if(( unsigned int) _temp1479 > 4u?*(( int*) _temp1479) == Cyc_Absyn_TunionFieldType:
0){ _LL1482: _temp1481=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1479)->f1;
_LL1486: _temp1485=( void*) _temp1481.field_info; if(*(( int*) _temp1485) == Cyc_Absyn_KnownTunionfield){
_LL1490: _temp1489=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1485)->f1;
goto _LL1488; _LL1488: _temp1487=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1485)->f2; goto _LL1484;} else{ goto _LL1441;} _LL1484: _temp1483=
_temp1481.targs; goto _LL1470;} else{ goto _LL1441;} _LL1470: _temp1469=
_temp1433.f2; if(( unsigned int) _temp1469 > 4u?*(( int*) _temp1469) == Cyc_Absyn_TunionType:
0){ _LL1472: _temp1471=(( struct Cyc_Absyn_TunionType_struct*) _temp1469)->f1;
_LL1476: _temp1475=( void*) _temp1471.tunion_info; if(*(( int*) _temp1475) ==
Cyc_Absyn_KnownTunion){ _LL1478: _temp1477=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1475)->f1; goto _LL1474;} else{ goto _LL1441;} _LL1474: _temp1473=
_temp1471.targs; goto _LL1440;} else{ goto _LL1441;} _LL1441: _LL1504: _temp1503=
_temp1433.f1; if(( unsigned int) _temp1503 > 4u?*(( int*) _temp1503) == Cyc_Absyn_PointerType:
0){ _LL1506: _temp1505=(( struct Cyc_Absyn_PointerType_struct*) _temp1503)->f1;
_LL1516: _temp1515=( void*) _temp1505.elt_typ; goto _LL1514; _LL1514: _temp1513=(
void*) _temp1505.rgn_typ; goto _LL1512; _LL1512: _temp1511= _temp1505.nullable;
goto _LL1510; _LL1510: _temp1509= _temp1505.tq; goto _LL1508; _LL1508: _temp1507=
_temp1505.bounds; goto _LL1492;} else{ goto _LL1443;} _LL1492: _temp1491=
_temp1433.f2; if(( unsigned int) _temp1491 > 4u?*(( int*) _temp1491) == Cyc_Absyn_TunionType:
0){ _LL1494: _temp1493=(( struct Cyc_Absyn_TunionType_struct*) _temp1491)->f1;
_LL1500: _temp1499=( void*) _temp1493.tunion_info; if(*(( int*) _temp1499) ==
Cyc_Absyn_KnownTunion){ _LL1502: _temp1501=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1499)->f1; goto _LL1498;} else{ goto _LL1443;} _LL1498: _temp1497=
_temp1493.targs; goto _LL1496; _LL1496: _temp1495=( void*) _temp1493.rgn; goto
_LL1442;} else{ goto _LL1443;} _LL1443: goto _LL1444; _LL1436: { int okay= 1;
if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1451.nullable, _temp1447.nullable)){
void* _temp1517=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp1451.nullable))->v; int _temp1523; _LL1519:
if(( unsigned int) _temp1517 > 1u?*(( int*) _temp1517) == Cyc_Absyn_Eq_constr: 0){
_LL1524: _temp1523=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1517)->f1;
goto _LL1520;} else{ goto _LL1521;} _LL1521: goto _LL1522; _LL1520: okay= !
_temp1523; goto _LL1518; _LL1522:( int) _throw( Cyc_Tcutil_impos( _tag_arr("silent_castable conref not eq",
sizeof( unsigned char), 30u))); _LL1518:;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp,
_temp1451.bounds, _temp1447.bounds)){ struct _tuple8 _temp1526=({ struct _tuple8
_temp1525; _temp1525.f1=( void*)( Cyc_Absyn_compress_conref( _temp1451.bounds))->v;
_temp1525.f2=( void*)( Cyc_Absyn_compress_conref( _temp1447.bounds))->v;
_temp1525;}); void* _temp1536; void* _temp1538; void* _temp1540; void* _temp1542;
void* _temp1544; void* _temp1546; struct Cyc_Absyn_Exp* _temp1548; void*
_temp1550; void* _temp1552; struct Cyc_Absyn_Exp* _temp1554; void* _temp1556;
void* _temp1558; void* _temp1560; void* _temp1562; _LL1528: _LL1541: _temp1540=
_temp1526.f1; if(( unsigned int) _temp1540 > 1u?*(( int*) _temp1540) == Cyc_Absyn_Eq_constr:
0){ _LL1543: _temp1542=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1540)->f1;
if(( unsigned int) _temp1542 > 1u?*(( int*) _temp1542) == Cyc_Absyn_Upper_b: 0){
goto _LL1537;} else{ goto _LL1530;}} else{ goto _LL1530;} _LL1537: _temp1536=
_temp1526.f2; if(( unsigned int) _temp1536 > 1u?*(( int*) _temp1536) == Cyc_Absyn_Eq_constr:
0){ _LL1539: _temp1538=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1536)->f1;
if( _temp1538 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1529;} else{ goto _LL1530;}}
else{ goto _LL1530;} _LL1530: _LL1551: _temp1550= _temp1526.f1; if((
unsigned int) _temp1550 > 1u?*(( int*) _temp1550) == Cyc_Absyn_Eq_constr: 0){
_LL1553: _temp1552=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1550)->f1;
if(( unsigned int) _temp1552 > 1u?*(( int*) _temp1552) == Cyc_Absyn_Upper_b: 0){
_LL1555: _temp1554=(( struct Cyc_Absyn_Upper_b_struct*) _temp1552)->f1; goto
_LL1545;} else{ goto _LL1532;}} else{ goto _LL1532;} _LL1545: _temp1544=
_temp1526.f2; if(( unsigned int) _temp1544 > 1u?*(( int*) _temp1544) == Cyc_Absyn_Eq_constr:
0){ _LL1547: _temp1546=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1544)->f1;
if(( unsigned int) _temp1546 > 1u?*(( int*) _temp1546) == Cyc_Absyn_Upper_b: 0){
_LL1549: _temp1548=(( struct Cyc_Absyn_Upper_b_struct*) _temp1546)->f1; goto
_LL1531;} else{ goto _LL1532;}} else{ goto _LL1532;} _LL1532: _LL1561: _temp1560=
_temp1526.f1; if(( unsigned int) _temp1560 > 1u?*(( int*) _temp1560) == Cyc_Absyn_Eq_constr:
0){ _LL1563: _temp1562=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1560)->f1;
if( _temp1562 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1557;} else{ goto _LL1534;}}
else{ goto _LL1534;} _LL1557: _temp1556= _temp1526.f2; if(( unsigned int)
_temp1556 > 1u?*(( int*) _temp1556) == Cyc_Absyn_Eq_constr: 0){ _LL1559:
_temp1558=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1556)->f1; if(
_temp1558 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1533;} else{ goto _LL1534;}}
else{ goto _LL1534;} _LL1534: goto _LL1535; _LL1529: okay= 1; goto _LL1527;
_LL1531: okay= okay? Cyc_Evexp_eval_const_uint_exp( _temp1554) >= Cyc_Evexp_eval_const_uint_exp(
_temp1548): 0; Cyc_Tcutil_warn( loc, _tag_arr("implicit cast to shorter array",
sizeof( unsigned char), 31u)); goto _LL1527; _LL1533: okay= 1; goto _LL1527;
_LL1535: okay= 0; goto _LL1527; _LL1527:;} okay= okay? Cyc_Tcutil_unify(( void*)
_temp1451.elt_typ,( void*) _temp1447.elt_typ): 0; okay= okay? Cyc_Tcutil_unify((
void*) _temp1451.rgn_typ,( void*) _temp1447.rgn_typ)? 1: Cyc_Tcenv_region_outlives(
te,( void*) _temp1451.rgn_typ,( void*) _temp1447.rgn_typ): 0; okay= okay? !(
_temp1451.tq).q_const? 1:( _temp1447.tq).q_const: 0; return okay;} _LL1438: {
int okay; okay=( _temp1463 != 0? _temp1455 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1463)) == Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1455)): 0; return( okay? Cyc_Tcutil_unify(
_temp1467, _temp1459): 0)? ! _temp1465.q_const? 1: _temp1457.q_const: 0;}
_LL1440: if(( _temp1489 == _temp1477? 1: Cyc_Absyn_qvar_cmp( _temp1489->name,
_temp1477->name) == 0)? _temp1487->typs == 0: 0){ for( 0; _temp1483 != 0?
_temp1473 != 0: 0;( _temp1483=(( struct Cyc_List_List*) _check_null( _temp1483))->tl,
_temp1473=(( struct Cyc_List_List*) _check_null( _temp1473))->tl)){ if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_List_List*) _check_null( _temp1483))->hd,( void*)(( struct
Cyc_List_List*) _check_null( _temp1473))->hd)){ break;}} if( _temp1483 == 0?
_temp1473 == 0: 0){ return 1;}} return 0; _LL1442:{ void* _temp1564= Cyc_Tcutil_compress(
_temp1515); struct Cyc_Absyn_TunionFieldInfo _temp1570; struct Cyc_List_List*
_temp1572; void* _temp1574; struct Cyc_Absyn_Tunionfield* _temp1576; struct Cyc_Absyn_Tuniondecl*
_temp1578; _LL1566: if(( unsigned int) _temp1564 > 4u?*(( int*) _temp1564) ==
Cyc_Absyn_TunionFieldType: 0){ _LL1571: _temp1570=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1564)->f1; _LL1575: _temp1574=( void*) _temp1570.field_info; if(*(( int*)
_temp1574) == Cyc_Absyn_KnownTunionfield){ _LL1579: _temp1578=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1574)->f1; goto _LL1577; _LL1577: _temp1576=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1574)->f2; goto _LL1573;} else{ goto _LL1568;} _LL1573: _temp1572=
_temp1570.targs; goto _LL1567;} else{ goto _LL1568;} _LL1568: goto _LL1569;
_LL1567: if( ! Cyc_Tcutil_unify( _temp1513, _temp1495)? ! Cyc_Tcenv_region_outlives(
te, _temp1513, _temp1495): 0){ return 0;} if( !(( int(*)( int(* cmp)( int, int),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp, _temp1511,(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
0))){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp, _temp1507,
Cyc_Absyn_new_conref( Cyc_Absyn_bounds_one))){ return 0;} if( Cyc_Absyn_qvar_cmp(
_temp1501->name, _temp1578->name) == 0? _temp1576->typs != 0: 0){ int okay= 1;
for( 0; _temp1572 != 0? _temp1497 != 0: 0;( _temp1572=(( struct Cyc_List_List*)
_check_null( _temp1572))->tl, _temp1497=(( struct Cyc_List_List*) _check_null(
_temp1497))->tl)){ if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_List_List*)
_check_null( _temp1572))->hd,( void*)(( struct Cyc_List_List*) _check_null(
_temp1497))->hd)){ okay= 0; break;}} if(( ! okay? 1: _temp1572 != 0)? 1:
_temp1497 != 0){ return 0;} return 1;} goto _LL1565; _LL1569: goto _LL1565;
_LL1565:;} return 0; _LL1444: return Cyc_Tcutil_unify( t1, t2); _LL1434:;}} int
Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void*
t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} if( Cyc_Tcutil_is_arithmetic_type(
t1)? Cyc_Tcutil_is_arithmetic_type( t2): 0){ if( Cyc_Tcutil_will_lose_precision(
t1, t2)){ Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u));} Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;}
else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); Cyc_Tcutil_warn( e->loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp1580)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1585= _tag_arr("implicit cast from %s to %s", sizeof(
unsigned char), 28u); struct Cyc_Stdio_String_pa_struct _temp1587; _temp1587.tag=
Cyc_Stdio_String_pa; _temp1587.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp1583=( void*)& _temp1587; struct Cyc_Stdio_String_pa_struct
_temp1586; _temp1586.tag= Cyc_Stdio_String_pa; _temp1586.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t2);{ void* _temp1584=( void*)& _temp1586; void*
_temp1581[ 2u]={ _temp1583, _temp1584}; struct _tagged_arr _temp1582={( void*)
_temp1581,( void*) _temp1581,( void*)( _temp1581 + 2u)}; _temp1580( _temp1585,
_temp1582);}}})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg(
te, e, t);} static int Cyc_Tcutil_any_promote( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e){ void* old_typ= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); void* t;{ void* _temp1588= old_typ; void* _temp1600;
void* _temp1602; _LL1590: if(( unsigned int) _temp1588 > 4u?*(( int*) _temp1588)
== Cyc_Absyn_EnumType: 0){ goto _LL1591;} else{ goto _LL1592;} _LL1592: if((
unsigned int) _temp1588 > 4u?*(( int*) _temp1588) == Cyc_Absyn_IntType: 0){
_LL1603: _temp1602=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1588)->f1;
goto _LL1601; _LL1601: _temp1600=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1588)->f2; goto _LL1593;} else{ goto _LL1594;} _LL1594: if( _temp1588 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1595;} else{ goto _LL1596;} _LL1596: if(
_temp1588 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1597;} else{ goto _LL1598;}
_LL1598: goto _LL1599; _LL1591: t= Cyc_Absyn_sint_t; goto _LL1589; _LL1593:{
void* _temp1604= _temp1600; _LL1606: if( _temp1604 ==( void*) Cyc_Absyn_B1){
goto _LL1607;} else{ goto _LL1608;} _LL1608: if( _temp1604 ==( void*) Cyc_Absyn_B2){
goto _LL1609;} else{ goto _LL1610;} _LL1610: if( _temp1604 ==( void*) Cyc_Absyn_B4){
goto _LL1611;} else{ goto _LL1612;} _LL1612: if( _temp1604 ==( void*) Cyc_Absyn_B8){
goto _LL1613;} else{ goto _LL1605;} _LL1607: t= Cyc_Absyn_sint_t; goto _LL1605;
_LL1609: t= Cyc_Absyn_sint_t; goto _LL1605; _LL1611: t= _temp1602 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL1605; _LL1613: t= _temp1602 ==( void*)
Cyc_Absyn_Unsigned? Cyc_Absyn_ulonglong_t: Cyc_Absyn_slonglong_t; goto _LL1605;
_LL1605:;} goto _LL1589; _LL1595: t= Cyc_Absyn_float_t; goto _LL1589; _LL1597: t=
Cyc_Absyn_double_t; goto _LL1589; _LL1599: return 0; _LL1589:;} Cyc_Tcutil_unchecked_cast(
te, e, t); return 1;} int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){
return 1;} Cyc_Tcutil_any_promote( te, e); if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, t2)){ return 1;} else{ if( Cyc_Tcutil_silent_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{
if( Cyc_Tcutil_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e,
t2); Cyc_Tcutil_warn( e->loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp1614)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1619= _tag_arr("implicit cast from %s to %s", sizeof(
unsigned char), 28u); struct Cyc_Stdio_String_pa_struct _temp1621; _temp1621.tag=
Cyc_Stdio_String_pa; _temp1621.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp1617=( void*)& _temp1621; struct Cyc_Stdio_String_pa_struct
_temp1620; _temp1620.tag= Cyc_Stdio_String_pa; _temp1620.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t2);{ void* _temp1618=( void*)& _temp1620; void*
_temp1615[ 2u]={ _temp1617, _temp1618}; struct _tagged_arr _temp1616={( void*)
_temp1615,( void*) _temp1615,( void*)( _temp1615 + 2u)}; _temp1614( _temp1619,
_temp1616);}}})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerceable( void*
t){ void* _temp1622= Cyc_Tcutil_compress( t); _LL1624: if(( unsigned int)
_temp1622 > 4u?*(( int*) _temp1622) == Cyc_Absyn_IntType: 0){ goto _LL1625;}
else{ goto _LL1626;} _LL1626: if( _temp1622 ==( void*) Cyc_Absyn_FloatType){
goto _LL1627;} else{ goto _LL1628;} _LL1628: if( _temp1622 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1629;} else{ goto _LL1630;} _LL1630: goto _LL1631; _LL1625: return 1;
_LL1627: return 1; _LL1629: return 1; _LL1631: return 0; _LL1623:;} static
struct _tuple4* Cyc_Tcutil_flatten_typ_f( struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield*
x){ return({ struct _tuple4* _temp1632=( struct _tuple4*) GC_malloc( sizeof(
struct _tuple4)); _temp1632->f1= x->tq; _temp1632->f2= Cyc_Tcutil_substitute(
inst,( void*) x->type); _temp1632;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ(
struct Cyc_Tcenv_Tenv* te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void*
_temp1633= t1; struct Cyc_List_List* _temp1645; struct Cyc_Absyn_Structdecl**
_temp1647; struct Cyc_List_List* _temp1649; struct _tuple1* _temp1651; struct
Cyc_List_List* _temp1653; _LL1635: if( _temp1633 ==( void*) Cyc_Absyn_VoidType){
goto _LL1636;} else{ goto _LL1637;} _LL1637: if(( unsigned int) _temp1633 > 4u?*((
int*) _temp1633) == Cyc_Absyn_TupleType: 0){ _LL1646: _temp1645=(( struct Cyc_Absyn_TupleType_struct*)
_temp1633)->f1; goto _LL1638;} else{ goto _LL1639;} _LL1639: if(( unsigned int)
_temp1633 > 4u?*(( int*) _temp1633) == Cyc_Absyn_StructType: 0){ _LL1652:
_temp1651=(( struct Cyc_Absyn_StructType_struct*) _temp1633)->f1; goto _LL1650;
_LL1650: _temp1649=(( struct Cyc_Absyn_StructType_struct*) _temp1633)->f2; goto
_LL1648; _LL1648: _temp1647=(( struct Cyc_Absyn_StructType_struct*) _temp1633)->f3;
goto _LL1640;} else{ goto _LL1641;} _LL1641: if(( unsigned int) _temp1633 > 4u?*((
int*) _temp1633) == Cyc_Absyn_AnonStructType: 0){ _LL1654: _temp1653=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1633)->f1; goto _LL1642;} else{ goto
_LL1643;} _LL1643: goto _LL1644; _LL1636: return 0; _LL1638: return _temp1645;
_LL1640: if( _temp1647 == 0? 1: _temp1651 == 0){ return({ struct Cyc_List_List*
_temp1655=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1655->hd=( void*)({ struct _tuple4* _temp1656=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1656->f1= Cyc_Absyn_empty_tqual(); _temp1656->f2=
t1; _temp1656;}); _temp1655->tl= 0; _temp1655;});}{ struct Cyc_Absyn_Structdecl*
_temp1657=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp1647)); struct
_tuple1 _temp1658=*(( struct _tuple1*) _check_null( _temp1651)); struct Cyc_List_List*
_temp1659=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp1657->tvs, _temp1649); if( _temp1657->fields == 0){
return({ struct Cyc_List_List* _temp1660=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1660->hd=( void*)({ struct _tuple4*
_temp1661=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1661->f1=
Cyc_Absyn_empty_tqual(); _temp1661->f2= t1; _temp1661;}); _temp1660->tl= 0;
_temp1660;});} return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, _temp1659,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1657->fields))->v);}
_LL1642: return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, 0, _temp1653); _LL1644: return({
struct Cyc_List_List* _temp1662=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1662->hd=( void*)({ struct _tuple4* _temp1663=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1663->f1= Cyc_Absyn_empty_tqual();
_temp1663->f2= t1; _temp1663;}); _temp1662->tl= 0; _temp1662;}); _LL1634:;}}
static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2); static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;}{ struct Cyc_List_List* a= assume; for( 0; a != 0; a=(( struct
Cyc_List_List*) _check_null( a))->tl){ if( Cyc_Tcutil_unify( t1,(*(( struct
_tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f1)? Cyc_Tcutil_unify(
t2,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f2): 0){
return 1;}}} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct
_tuple8 _temp1665=({ struct _tuple8 _temp1664; _temp1664.f1= t1; _temp1664.f2=
t2; _temp1664;}); void* _temp1671; struct Cyc_Absyn_PtrInfo _temp1673; struct
Cyc_Absyn_Conref* _temp1675; struct Cyc_Absyn_Tqual _temp1677; struct Cyc_Absyn_Conref*
_temp1679; void* _temp1681; void* _temp1683; void* _temp1685; struct Cyc_Absyn_PtrInfo
_temp1687; struct Cyc_Absyn_Conref* _temp1689; struct Cyc_Absyn_Tqual _temp1691;
struct Cyc_Absyn_Conref* _temp1693; void* _temp1695; void* _temp1697; _LL1667:
_LL1686: _temp1685= _temp1665.f1; if(( unsigned int) _temp1685 > 4u?*(( int*)
_temp1685) == Cyc_Absyn_PointerType: 0){ _LL1688: _temp1687=(( struct Cyc_Absyn_PointerType_struct*)
_temp1685)->f1; _LL1698: _temp1697=( void*) _temp1687.elt_typ; goto _LL1696;
_LL1696: _temp1695=( void*) _temp1687.rgn_typ; goto _LL1694; _LL1694: _temp1693=
_temp1687.nullable; goto _LL1692; _LL1692: _temp1691= _temp1687.tq; goto _LL1690;
_LL1690: _temp1689= _temp1687.bounds; goto _LL1672;} else{ goto _LL1669;}
_LL1672: _temp1671= _temp1665.f2; if(( unsigned int) _temp1671 > 4u?*(( int*)
_temp1671) == Cyc_Absyn_PointerType: 0){ _LL1674: _temp1673=(( struct Cyc_Absyn_PointerType_struct*)
_temp1671)->f1; _LL1684: _temp1683=( void*) _temp1673.elt_typ; goto _LL1682;
_LL1682: _temp1681=( void*) _temp1673.rgn_typ; goto _LL1680; _LL1680: _temp1679=
_temp1673.nullable; goto _LL1678; _LL1678: _temp1677= _temp1673.tq; goto _LL1676;
_LL1676: _temp1675= _temp1673.bounds; goto _LL1668;} else{ goto _LL1669;}
_LL1669: goto _LL1670; _LL1668: if( _temp1691.q_const? ! _temp1677.q_const: 0){
return 0;} if(( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1693, _temp1679)?(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1693): 0)? !(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1679): 0){ return 0;} if( ! Cyc_Tcutil_unify( _temp1695, _temp1681)? ! Cyc_Tcenv_region_outlives(
te, _temp1695, _temp1681): 0){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp,
_temp1689, _temp1675)){ struct _tuple8 _temp1700=({ struct _tuple8 _temp1699;
_temp1699.f1= Cyc_Absyn_conref_val( _temp1689); _temp1699.f2= Cyc_Absyn_conref_val(
_temp1675); _temp1699;}); void* _temp1708; void* _temp1710; void* _temp1712;
struct Cyc_Absyn_Exp* _temp1714; void* _temp1716; struct Cyc_Absyn_Exp*
_temp1718; _LL1702: _LL1711: _temp1710= _temp1700.f1; if(( unsigned int)
_temp1710 > 1u?*(( int*) _temp1710) == Cyc_Absyn_Upper_b: 0){ goto _LL1709;}
else{ goto _LL1704;} _LL1709: _temp1708= _temp1700.f2; if( _temp1708 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL1703;} else{ goto _LL1704;} _LL1704: _LL1717:
_temp1716= _temp1700.f1; if(( unsigned int) _temp1716 > 1u?*(( int*) _temp1716)
== Cyc_Absyn_Upper_b: 0){ _LL1719: _temp1718=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1716)->f1; goto _LL1713;} else{ goto _LL1706;} _LL1713: _temp1712=
_temp1700.f2; if(( unsigned int) _temp1712 > 1u?*(( int*) _temp1712) == Cyc_Absyn_Upper_b:
0){ _LL1715: _temp1714=(( struct Cyc_Absyn_Upper_b_struct*) _temp1712)->f1; goto
_LL1705;} else{ goto _LL1706;} _LL1706: goto _LL1707; _LL1703: goto _LL1701;
_LL1705: if( Cyc_Evexp_eval_const_uint_exp( _temp1718) < Cyc_Evexp_eval_const_uint_exp(
_temp1714)){ return 0;} goto _LL1701; _LL1707: return 0; _LL1701:;} return Cyc_Tcutil_ptrsubtype(
te,({ struct Cyc_List_List* _temp1720=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1720->hd=( void*)({ struct _tuple8*
_temp1721=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp1721->f1=
t1; _temp1721->f2= t2; _temp1721;}); _temp1720->tl= assume; _temp1720;}),
_temp1697, _temp1683); _LL1670: return 0; _LL1666:;}} static int Cyc_Tcutil_isomorphic(
void* t1, void* t2){ struct _tuple8 _temp1723=({ struct _tuple8 _temp1722;
_temp1722.f1= Cyc_Tcutil_compress( t1); _temp1722.f2= Cyc_Tcutil_compress( t2);
_temp1722;}); void* _temp1729; void* _temp1731; void* _temp1733; void* _temp1735;
_LL1725: _LL1734: _temp1733= _temp1723.f1; if(( unsigned int) _temp1733 > 4u?*((
int*) _temp1733) == Cyc_Absyn_IntType: 0){ _LL1736: _temp1735=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1733)->f2; goto _LL1730;} else{ goto _LL1727;}
_LL1730: _temp1729= _temp1723.f2; if(( unsigned int) _temp1729 > 4u?*(( int*)
_temp1729) == Cyc_Absyn_IntType: 0){ _LL1732: _temp1731=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1729)->f2; goto _LL1726;} else{ goto _LL1727;} _LL1727: goto _LL1728;
_LL1726: return _temp1735 == _temp1731; _LL1728: return 0; _LL1724:;} static int
Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* assume,
void* t1, void* t2){ struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ( te, t1);
struct Cyc_List_List* tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0; tqs2 != 0;(
tqs2=(( struct Cyc_List_List*) _check_null( tqs2))->tl, tqs1=(( struct Cyc_List_List*)
_check_null( tqs1))->tl)){ if( tqs1 == 0){ return 0;}{ struct _tuple4 _temp1739;
void* _temp1740; struct Cyc_Absyn_Tqual _temp1742; struct _tuple4* _temp1737=(
struct _tuple4*)(( struct Cyc_List_List*) _check_null( tqs1))->hd; _temp1739=*
_temp1737; _LL1743: _temp1742= _temp1739.f1; goto _LL1741; _LL1741: _temp1740=
_temp1739.f2; goto _LL1738; _LL1738: { struct _tuple4 _temp1746; void* _temp1747;
struct Cyc_Absyn_Tqual _temp1749; struct _tuple4* _temp1744=( struct _tuple4*)((
struct Cyc_List_List*) _check_null( tqs2))->hd; _temp1746=* _temp1744; _LL1750:
_temp1749= _temp1746.f1; goto _LL1748; _LL1748: _temp1747= _temp1746.f2; goto
_LL1745; _LL1745: if( _temp1749.q_const? Cyc_Tcutil_subtype( te, assume,
_temp1740, _temp1747): 0){ continue;} else{ if( Cyc_Tcutil_unify( _temp1740,
_temp1747)){ continue;} else{ if( Cyc_Tcutil_isomorphic( _temp1740, _temp1747)){
continue;} else{ return 0;}}}}}} return 1;} int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void* t1, void* t2){ if( Cyc_Tcutil_unify(
t1, t2)){ return 1;} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
void* _temp1751= t2; struct Cyc_Absyn_Uniondecl** _temp1759; struct Cyc_List_List*
_temp1761; void* _temp1763; _LL1753: if(( unsigned int) _temp1751 > 4u?*(( int*)
_temp1751) == Cyc_Absyn_UnionType: 0){ _LL1762: _temp1761=(( struct Cyc_Absyn_UnionType_struct*)
_temp1751)->f2; goto _LL1760; _LL1760: _temp1759=(( struct Cyc_Absyn_UnionType_struct*)
_temp1751)->f3; goto _LL1754;} else{ goto _LL1755;} _LL1755: if(( unsigned int)
_temp1751 > 4u?*(( int*) _temp1751) == Cyc_Absyn_IntType: 0){ _LL1764: _temp1763=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1751)->f2; if( _temp1763 ==(
void*) Cyc_Absyn_B4){ goto _LL1756;} else{ goto _LL1757;}} else{ goto _LL1757;}
_LL1757: goto _LL1758; _LL1754: if( _temp1759 == 0){ goto _LL1752;}{ struct Cyc_Absyn_Uniondecl*
_temp1765=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp1759)); if(
_temp1765->fields == 0){ goto _LL1752;}{ struct Cyc_List_List* _temp1766=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1765->tvs, _temp1761);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1765->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ if( Cyc_Tcutil_unify( t1, Cyc_Tcutil_substitute(
_temp1766,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ return 1;}}} goto _LL1752;}} _LL1756: if( Cyc_Tcutil_typ_kind(
t2) ==( void*) Cyc_Absyn_BoxKind){ return 1;} goto _LL1752; _LL1758: goto
_LL1752; _LL1752:;}{ void* _temp1767= t1; struct Cyc_Absyn_PtrInfo _temp1783;
struct Cyc_Absyn_Conref* _temp1785; struct Cyc_Absyn_Tqual _temp1787; struct Cyc_Absyn_Conref*
_temp1789; void* _temp1791; void* _temp1793; struct Cyc_Absyn_Exp* _temp1795;
struct Cyc_Absyn_Tqual _temp1797; void* _temp1799; struct Cyc_Absyn_Enumdecl*
_temp1801; _LL1769: if(( unsigned int) _temp1767 > 4u?*(( int*) _temp1767) ==
Cyc_Absyn_PointerType: 0){ _LL1784: _temp1783=(( struct Cyc_Absyn_PointerType_struct*)
_temp1767)->f1; _LL1794: _temp1793=( void*) _temp1783.elt_typ; goto _LL1792;
_LL1792: _temp1791=( void*) _temp1783.rgn_typ; goto _LL1790; _LL1790: _temp1789=
_temp1783.nullable; goto _LL1788; _LL1788: _temp1787= _temp1783.tq; goto _LL1786;
_LL1786: _temp1785= _temp1783.bounds; goto _LL1770;} else{ goto _LL1771;}
_LL1771: if(( unsigned int) _temp1767 > 4u?*(( int*) _temp1767) == Cyc_Absyn_ArrayType:
0){ _LL1800: _temp1799=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1767)->f1;
goto _LL1798; _LL1798: _temp1797=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1767)->f2; goto _LL1796; _LL1796: _temp1795=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1767)->f3; goto _LL1772;} else{ goto _LL1773;} _LL1773: if(( unsigned int)
_temp1767 > 4u?*(( int*) _temp1767) == Cyc_Absyn_EnumType: 0){ _LL1802:
_temp1801=(( struct Cyc_Absyn_EnumType_struct*) _temp1767)->f2; goto _LL1774;}
else{ goto _LL1775;} _LL1775: if(( unsigned int) _temp1767 > 4u?*(( int*)
_temp1767) == Cyc_Absyn_IntType: 0){ goto _LL1776;} else{ goto _LL1777;} _LL1777:
if( _temp1767 ==( void*) Cyc_Absyn_FloatType){ goto _LL1778;} else{ goto _LL1779;}
_LL1779: if( _temp1767 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1780;} else{
goto _LL1781;} _LL1781: goto _LL1782; _LL1770:{ void* _temp1803= t2; struct Cyc_Absyn_PtrInfo
_temp1809; struct Cyc_Absyn_Conref* _temp1811; struct Cyc_Absyn_Tqual _temp1813;
struct Cyc_Absyn_Conref* _temp1815; void* _temp1817; void* _temp1819; _LL1805:
if(( unsigned int) _temp1803 > 4u?*(( int*) _temp1803) == Cyc_Absyn_PointerType:
0){ _LL1810: _temp1809=(( struct Cyc_Absyn_PointerType_struct*) _temp1803)->f1;
_LL1820: _temp1819=( void*) _temp1809.elt_typ; goto _LL1818; _LL1818: _temp1817=(
void*) _temp1809.rgn_typ; goto _LL1816; _LL1816: _temp1815= _temp1809.nullable;
goto _LL1814; _LL1814: _temp1813= _temp1809.tq; goto _LL1812; _LL1812: _temp1811=
_temp1809.bounds; goto _LL1806;} else{ goto _LL1807;} _LL1807: goto _LL1808;
_LL1806: { struct Cyc_List_List* _temp1821=({ struct Cyc_List_List* _temp1838=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1838->hd=(
void*)({ struct _tuple8* _temp1839=( struct _tuple8*) GC_malloc( sizeof( struct
_tuple8)); _temp1839->f1= t1; _temp1839->f2= t2; _temp1839;}); _temp1838->tl= 0;
_temp1838;}); int _temp1822= Cyc_Tcutil_ptrsubtype( te, _temp1821, _temp1793,
_temp1819)? ! _temp1787.q_const? 1: _temp1813.q_const: 0; int bounds_ok= Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_boundscmp, _temp1785, _temp1811); if( ! bounds_ok){ struct _tuple8
_temp1824=({ struct _tuple8 _temp1823; _temp1823.f1= Cyc_Absyn_conref_val(
_temp1785); _temp1823.f2= Cyc_Absyn_conref_val( _temp1811); _temp1823;}); void*
_temp1830; struct Cyc_Absyn_Exp* _temp1832; void* _temp1834; struct Cyc_Absyn_Exp*
_temp1836; _LL1826: _LL1835: _temp1834= _temp1824.f1; if(( unsigned int)
_temp1834 > 1u?*(( int*) _temp1834) == Cyc_Absyn_Upper_b: 0){ _LL1837: _temp1836=((
struct Cyc_Absyn_Upper_b_struct*) _temp1834)->f1; goto _LL1831;} else{ goto
_LL1828;} _LL1831: _temp1830= _temp1824.f2; if(( unsigned int) _temp1830 > 1u?*((
int*) _temp1830) == Cyc_Absyn_Upper_b: 0){ _LL1833: _temp1832=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1830)->f1; goto _LL1827;} else{ goto _LL1828;} _LL1828: goto _LL1829;
_LL1827: if( Cyc_Evexp_eval_const_uint_exp( _temp1836) >= Cyc_Evexp_eval_const_uint_exp(
_temp1832)){ bounds_ok= 1;} goto _LL1825; _LL1829: bounds_ok= 1; goto _LL1825;
_LL1825:;} return( bounds_ok? _temp1822: 0)? Cyc_Tcutil_unify( _temp1791,
_temp1817)? 1: Cyc_Tcenv_region_outlives( te, _temp1791, _temp1817): 0;} _LL1808:
goto _LL1804; _LL1804:;} return 0; _LL1772:{ void* _temp1840= t2; struct Cyc_Absyn_Exp*
_temp1846; struct Cyc_Absyn_Tqual _temp1848; void* _temp1850; _LL1842: if((
unsigned int) _temp1840 > 4u?*(( int*) _temp1840) == Cyc_Absyn_ArrayType: 0){
_LL1851: _temp1850=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1840)->f1;
goto _LL1849; _LL1849: _temp1848=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1840)->f2; goto _LL1847; _LL1847: _temp1846=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1840)->f3; goto _LL1843;} else{ goto _LL1844;} _LL1844: goto _LL1845;
_LL1843: { int okay; okay=( _temp1795 != 0? _temp1846 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1795)) >= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1846)): 0; return( okay? Cyc_Tcutil_unify(
_temp1799, _temp1850): 0)? ! _temp1797.q_const? 1: _temp1848.q_const: 0;}
_LL1845: return 0; _LL1841:;} return 0; _LL1774:{ void* _temp1852= t2; struct
Cyc_Absyn_Enumdecl* _temp1858; _LL1854: if(( unsigned int) _temp1852 > 4u?*((
int*) _temp1852) == Cyc_Absyn_EnumType: 0){ _LL1859: _temp1858=(( struct Cyc_Absyn_EnumType_struct*)
_temp1852)->f2; goto _LL1855;} else{ goto _LL1856;} _LL1856: goto _LL1857;
_LL1855: if(((( struct Cyc_Absyn_Enumdecl*) _check_null( _temp1801))->fields !=
0?(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp1858))->fields != 0: 0)?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Enumdecl*) _check_null(
_temp1801))->fields))->v) >=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Enumdecl*)
_check_null( _temp1858))->fields))->v): 0){ return 1;} goto _LL1853; _LL1857:
goto _LL1853; _LL1853:;} goto _LL1776; _LL1776: goto _LL1778; _LL1778: goto
_LL1780; _LL1780: return Cyc_Tcutil_coerceable( t2); _LL1782: return 0; _LL1768:;}}
void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t){ if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, t)){ struct Cyc_Absyn_Exp* _temp1860= Cyc_Absyn_copy_exp( e);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct* _temp1861=(
struct Cyc_Absyn_Cast_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp1861[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp1862; _temp1862.tag= Cyc_Absyn_Cast_e;
_temp1862.f1=( void*) t; _temp1862.f2= _temp1860; _temp1862;}); _temp1861;})));
e->topt=({ struct Cyc_Core_Opt* _temp1863=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1863->v=( void*) t; _temp1863;});}} int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp* e){ void* _temp1864= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v); _LL1866: if(( unsigned int) _temp1864
> 4u?*(( int*) _temp1864) == Cyc_Absyn_EnumType: 0){ goto _LL1867;} else{ goto
_LL1868;} _LL1868: if(( unsigned int) _temp1864 > 4u?*(( int*) _temp1864) == Cyc_Absyn_IntType:
0){ goto _LL1869;} else{ goto _LL1870;} _LL1870: if(( unsigned int) _temp1864 >
4u?*(( int*) _temp1864) == Cyc_Absyn_Evar: 0){ goto _LL1871;} else{ goto _LL1872;}
_LL1872: goto _LL1873; _LL1867: goto _LL1869; _LL1869: return 1; _LL1871: if(
Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){
return 1;} return 0; _LL1873: return 0; _LL1865:;} int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_is_integral( e)){ return 1;}{ void*
_temp1874= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_LL1876: if( _temp1874 ==( void*) Cyc_Absyn_FloatType){ goto _LL1877;} else{
goto _LL1878;} _LL1878: if( _temp1874 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL1879;} else{ goto _LL1880;} _LL1880: goto _LL1881; _LL1877: goto _LL1879;
_LL1879: return 1; _LL1881: return 0; _LL1875:;}} int Cyc_Tcutil_is_function_type(
void* t){ void* _temp1882= Cyc_Tcutil_compress( t); _LL1884: if(( unsigned int)
_temp1882 > 4u?*(( int*) _temp1882) == Cyc_Absyn_FnType: 0){ goto _LL1885;}
else{ goto _LL1886;} _LL1886: goto _LL1887; _LL1885: return 1; _LL1887: return 0;
_LL1883:;} void* Cyc_Tcutil_max_arithmetic_type( void* t1, void* t2){ struct
_tuple8 _temp1889=({ struct _tuple8 _temp1888; _temp1888.f1= t1; _temp1888.f2=
t2; _temp1888;}); void* _temp1913; void* _temp1915; void* _temp1917; void*
_temp1919; void* _temp1921; void* _temp1923; void* _temp1925; void* _temp1927;
void* _temp1929; void* _temp1931; void* _temp1933; void* _temp1935; void*
_temp1937; void* _temp1939; void* _temp1941; void* _temp1943; void* _temp1945;
void* _temp1947; void* _temp1949; void* _temp1951; void* _temp1953; void*
_temp1955; _LL1891: _LL1914: _temp1913= _temp1889.f1; if( _temp1913 ==( void*)
Cyc_Absyn_DoubleType){ goto _LL1892;} else{ goto _LL1893;} _LL1893: _LL1916:
_temp1915= _temp1889.f2; if( _temp1915 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL1894;} else{ goto _LL1895;} _LL1895: _LL1918: _temp1917= _temp1889.f1; if(
_temp1917 ==( void*) Cyc_Absyn_FloatType){ goto _LL1896;} else{ goto _LL1897;}
_LL1897: _LL1920: _temp1919= _temp1889.f2; if( _temp1919 ==( void*) Cyc_Absyn_FloatType){
goto _LL1898;} else{ goto _LL1899;} _LL1899: _LL1922: _temp1921= _temp1889.f1;
if(( unsigned int) _temp1921 > 4u?*(( int*) _temp1921) == Cyc_Absyn_IntType: 0){
_LL1926: _temp1925=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1921)->f1;
if( _temp1925 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1924;} else{ goto _LL1901;}
_LL1924: _temp1923=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1921)->f2;
if( _temp1923 ==( void*) Cyc_Absyn_B8){ goto _LL1900;} else{ goto _LL1901;}}
else{ goto _LL1901;} _LL1901: _LL1928: _temp1927= _temp1889.f2; if((
unsigned int) _temp1927 > 4u?*(( int*) _temp1927) == Cyc_Absyn_IntType: 0){
_LL1932: _temp1931=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1927)->f1;
if( _temp1931 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1930;} else{ goto _LL1903;}
_LL1930: _temp1929=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1927)->f2;
if( _temp1929 ==( void*) Cyc_Absyn_B8){ goto _LL1902;} else{ goto _LL1903;}}
else{ goto _LL1903;} _LL1903: _LL1934: _temp1933= _temp1889.f1; if((
unsigned int) _temp1933 > 4u?*(( int*) _temp1933) == Cyc_Absyn_IntType: 0){
_LL1938: _temp1937=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1933)->f1;
if( _temp1937 ==( void*) Cyc_Absyn_Signed){ goto _LL1936;} else{ goto _LL1905;}
_LL1936: _temp1935=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1933)->f2;
if( _temp1935 ==( void*) Cyc_Absyn_B8){ goto _LL1904;} else{ goto _LL1905;}}
else{ goto _LL1905;} _LL1905: _LL1940: _temp1939= _temp1889.f2; if((
unsigned int) _temp1939 > 4u?*(( int*) _temp1939) == Cyc_Absyn_IntType: 0){
_LL1944: _temp1943=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1939)->f1;
if( _temp1943 ==( void*) Cyc_Absyn_Signed){ goto _LL1942;} else{ goto _LL1907;}
_LL1942: _temp1941=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1939)->f2;
if( _temp1941 ==( void*) Cyc_Absyn_B8){ goto _LL1906;} else{ goto _LL1907;}}
else{ goto _LL1907;} _LL1907: _LL1946: _temp1945= _temp1889.f1; if((
unsigned int) _temp1945 > 4u?*(( int*) _temp1945) == Cyc_Absyn_IntType: 0){
_LL1950: _temp1949=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1945)->f1;
if( _temp1949 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1948;} else{ goto _LL1909;}
_LL1948: _temp1947=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1945)->f2;
if( _temp1947 ==( void*) Cyc_Absyn_B4){ goto _LL1908;} else{ goto _LL1909;}}
else{ goto _LL1909;} _LL1909: _LL1952: _temp1951= _temp1889.f2; if((
unsigned int) _temp1951 > 4u?*(( int*) _temp1951) == Cyc_Absyn_IntType: 0){
_LL1956: _temp1955=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1951)->f1;
if( _temp1955 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1954;} else{ goto _LL1911;}
_LL1954: _temp1953=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1951)->f2;
if( _temp1953 ==( void*) Cyc_Absyn_B4){ goto _LL1910;} else{ goto _LL1911;}}
else{ goto _LL1911;} _LL1911: goto _LL1912; _LL1892: goto _LL1894; _LL1894:
return( void*) Cyc_Absyn_DoubleType; _LL1896: goto _LL1898; _LL1898: return(
void*) Cyc_Absyn_FloatType; _LL1900: goto _LL1902; _LL1902: return Cyc_Absyn_ulonglong_t;
_LL1904: goto _LL1906; _LL1906: return Cyc_Absyn_slonglong_t; _LL1908: goto
_LL1910; _LL1910: return Cyc_Absyn_uint_t; _LL1912: return Cyc_Absyn_sint_t;
_LL1890:;} void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp* e){ void*
_temp1957=( void*) e->r; struct Cyc_Core_Opt* _temp1963; _LL1959: if(*(( int*)
_temp1957) == Cyc_Absyn_AssignOp_e){ _LL1964: _temp1963=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1957)->f2; if( _temp1963 == 0){ goto _LL1960;} else{ goto _LL1961;}} else{
goto _LL1961;} _LL1961: goto _LL1962; _LL1960: Cyc_Tcutil_warn( e->loc, _tag_arr("assignment in test",
sizeof( unsigned char), 19u)); goto _LL1958; _LL1962: goto _LL1958; _LL1958:;}
static int Cyc_Tcutil_constrain_kinds( struct Cyc_Absyn_Conref* c1, struct Cyc_Absyn_Conref*
c2){ c1= Cyc_Absyn_compress_conref( c1); c2= Cyc_Absyn_compress_conref( c2); if(
c1 == c2){ return 1;} if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr){( void*)(
c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp1965=(
struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct));
_temp1965[ 0]=({ struct Cyc_Absyn_Forward_constr_struct _temp1966; _temp1966.tag=
Cyc_Absyn_Forward_constr; _temp1966.f1= c2; _temp1966;}); _temp1965;}))); return
1;} else{ if(( void*) c2->v ==( void*) Cyc_Absyn_No_constr){( void*)( c2->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp1967=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp1967[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp1968; _temp1968.tag= Cyc_Absyn_Forward_constr;
_temp1968.f1= c1; _temp1968;}); _temp1967;}))); return 1;} else{ void* k1= Cyc_Absyn_conref_val(
c1); void* k2= Cyc_Absyn_conref_val( c2); if( Cyc_Tcutil_kind_leq( k1, k2)){(
void*)( c2->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1969=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1969[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp1970; _temp1970.tag= Cyc_Absyn_Forward_constr; _temp1970.f1= c1; _temp1970;});
_temp1969;}))); return 1;} else{ if( Cyc_Tcutil_kind_leq( k2, k1)){( void*)( c1->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp1971=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp1971[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp1972; _temp1972.tag= Cyc_Absyn_Forward_constr;
_temp1972.f1= c2; _temp1972;}); _temp1971;}))); return 1;} else{ return 0;}}}}}
static int Cyc_Tcutil_tvar_id_counter= 0; int* Cyc_Tcutil_new_tvar_id(){ return({
int* _temp1973=( int*) GC_malloc_atomic( sizeof( int)); _temp1973[ 0]= Cyc_Tcutil_tvar_id_counter
++; _temp1973;});} static int Cyc_Tcutil_tvar_counter= 0; struct Cyc_Absyn_Tvar*
Cyc_Tcutil_new_tvar( void* k){ int i= Cyc_Tcutil_tvar_counter ++; struct
_tagged_arr s=( struct _tagged_arr)({ struct _tagged_arr(* _temp1976)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp1980= _tag_arr("#%d", sizeof( unsigned char), 4u); struct Cyc_Stdio_Int_pa_struct
_temp1981; _temp1981.tag= Cyc_Stdio_Int_pa; _temp1981.f1=( unsigned int) i;{
void* _temp1979=( void*)& _temp1981; void* _temp1977[ 1u]={ _temp1979}; struct
_tagged_arr _temp1978={( void*) _temp1977,( void*) _temp1977,( void*)( _temp1977
+ 1u)}; _temp1976( _temp1980, _temp1978);}}); return({ struct Cyc_Absyn_Tvar*
_temp1974=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp1974->name=({ struct _tagged_arr* _temp1975=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr) * 1); _temp1975[ 0]= s; _temp1975;});
_temp1974->identity= 0; _temp1974->kind= Cyc_Absyn_new_conref( k); _temp1974;});}
int Cyc_Tcutil_is_temp_tvar( struct Cyc_Absyn_Tvar* t){ struct _tagged_arr
_temp1982=* t->name; return*(( const unsigned char*) _check_unknown_subscript(
_temp1982, sizeof( unsigned char), 0)) =='#';} void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar* t){ if( ! Cyc_Tcutil_is_temp_tvar( t)){ return;}{ struct
_tagged_arr _temp1983= Cyc_String_strconcat( _tag_arr("`", sizeof( unsigned char),
2u),* t->name);*(( unsigned char*) _check_unknown_subscript( _temp1983, sizeof(
unsigned char), 1))='t'; t->name=({ struct _tagged_arr* _temp1984=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1); _temp1984[ 0]=(
struct _tagged_arr) _temp1983; _temp1984;});}} struct _tuple13{ struct
_tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; static struct _tuple2*
Cyc_Tcutil_fndecl2typ_f( struct _tuple13* x){ return({ struct _tuple2* _temp1985=(
struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp1985->f1=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp1986=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1986->v=( void*)(* x).f1; _temp1986;}); _temp1985->f2=(* x).f2;
_temp1985->f3=(* x).f3; _temp1985;});} void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*
fd){ if( fd->cached_typ == 0){ struct Cyc_List_List* _temp1987= 0;{ struct Cyc_List_List*
atts= fd->attributes; for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ if( Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd)){ _temp1987=({ struct Cyc_List_List* _temp1988=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1988->hd=( void*)((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd); _temp1988->tl=
_temp1987; _temp1988;});}}} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp1989=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1989[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1990; _temp1990.tag= Cyc_Absyn_FnType;
_temp1990.f1=({ struct Cyc_Absyn_FnInfo _temp1991; _temp1991.tvars= fd->tvs;
_temp1991.effect= fd->effect; _temp1991.ret_typ=( void*)(( void*) fd->ret_type);
_temp1991.args=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct
_tuple13*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_fndecl2typ_f, fd->args);
_temp1991.c_varargs= fd->c_varargs; _temp1991.cyc_varargs= fd->cyc_varargs;
_temp1991.rgn_po= fd->rgn_po; _temp1991.attributes= _temp1987; _temp1991;});
_temp1990;}); _temp1989;});} return( void*)(( struct Cyc_Core_Opt*) _check_null(
fd->cached_typ))->v;} struct _tuple14{ void* f1; struct Cyc_Absyn_Tqual f2; void*
f3; } ; static void* Cyc_Tcutil_fst_fdarg( struct _tuple14* t){ return(* t).f1;}
void* Cyc_Tcutil_snd_tqt( struct _tuple4* t){ return(* t).f2;} static struct
_tuple4* Cyc_Tcutil_map2_tq( struct _tuple4* pr, void* t){ return({ struct
_tuple4* _temp1992=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1992->f1=(* pr).f1; _temp1992->f2= t; _temp1992;});} void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl*
fd){ void* t= Cyc_Tcutil_fndecl2typ( fd); Cyc_Tcutil_check_valid_toplevel_type(
loc, te, t);{ void* _temp1993= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo
_temp1999; struct Cyc_List_List* _temp2001; struct Cyc_List_List* _temp2003;
struct Cyc_Absyn_VarargInfo* _temp2005; int _temp2007; struct Cyc_List_List*
_temp2009; void* _temp2011; struct Cyc_Core_Opt* _temp2013; struct Cyc_List_List*
_temp2015; _LL1995: if(( unsigned int) _temp1993 > 4u?*(( int*) _temp1993) ==
Cyc_Absyn_FnType: 0){ _LL2000: _temp1999=(( struct Cyc_Absyn_FnType_struct*)
_temp1993)->f1; _LL2016: _temp2015= _temp1999.tvars; goto _LL2014; _LL2014:
_temp2013= _temp1999.effect; goto _LL2012; _LL2012: _temp2011=( void*) _temp1999.ret_typ;
goto _LL2010; _LL2010: _temp2009= _temp1999.args; goto _LL2008; _LL2008:
_temp2007= _temp1999.c_varargs; goto _LL2006; _LL2006: _temp2005= _temp1999.cyc_varargs;
goto _LL2004; _LL2004: _temp2003= _temp1999.rgn_po; goto _LL2002; _LL2002:
_temp2001= _temp1999.attributes; goto _LL1996;} else{ goto _LL1997;} _LL1997:
goto _LL1998; _LL1996: fd->tvs= _temp2015; fd->effect= _temp2013; goto _LL1994;
_LL1998:(( int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)( _tag_arr("check_fndecl_valid_type: not a FnType",
sizeof( unsigned char), 38u)); return; _LL1994:;}{ struct _RegionHandle
_temp2017= _new_region(); struct _RegionHandle* r=& _temp2017; _push_region( r);
Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tagged_arr*(* f)( struct _tuple13*), struct Cyc_List_List* x)) Cyc_List_rmap)(
r,( struct _tagged_arr*(*)( struct _tuple13* t)) Cyc_Tcutil_fst_fdarg, fd->args),
loc, _tag_arr("function declaration has repeated parameter", sizeof(
unsigned char), 44u));; _pop_region( r);} fd->cached_typ=({ struct Cyc_Core_Opt*
_temp2018=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2018->v=( void*) t; _temp2018;});} struct _tuple15{ struct Cyc_Core_Opt* f1;
struct Cyc_Absyn_Tqual f2; } ; struct _tuple16{ struct _tuple15* f1; void* f2; }
; static struct _tuple16* Cyc_Tcutil_substitute_f1( struct _RegionHandle* rgn,
struct _tuple2* y){ return({ struct _tuple16* _temp2019=( struct _tuple16*)
_region_malloc( rgn, sizeof( struct _tuple16)); _temp2019->f1=({ struct _tuple15*
_temp2020=( struct _tuple15*) _region_malloc( rgn, sizeof( struct _tuple15));
_temp2020->f1=(* y).f1; _temp2020->f2=(* y).f2; _temp2020;}); _temp2019->f2=(* y).f3;
_temp2019;});} static struct _tuple2* Cyc_Tcutil_substitute_f2( struct _tuple16*
w){ void* _temp2023; struct _tuple15* _temp2025; struct _tuple16 _temp2021=* w;
_LL2026: _temp2025= _temp2021.f1; goto _LL2024; _LL2024: _temp2023= _temp2021.f2;
goto _LL2022; _LL2022: { struct Cyc_Absyn_Tqual _temp2029; struct Cyc_Core_Opt*
_temp2031; struct _tuple15 _temp2027=* _temp2025; _LL2032: _temp2031= _temp2027.f1;
goto _LL2030; _LL2030: _temp2029= _temp2027.f2; goto _LL2028; _LL2028: return({
struct _tuple2* _temp2033=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2033->f1= _temp2031; _temp2033->f2= _temp2029; _temp2033->f3= _temp2023;
_temp2033;});}} static void* Cyc_Tcutil_field_type( struct Cyc_Absyn_Structfield*
f){ return( void*) f->type;} static struct Cyc_Absyn_Structfield* Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Structfield* f, void* t){ return({ struct Cyc_Absyn_Structfield*
_temp2034=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2034->name= f->name; _temp2034->tq= f->tq; _temp2034->type=( void*) t;
_temp2034->width= f->width; _temp2034->attributes= f->attributes; _temp2034;});}
static struct Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn,
struct Cyc_List_List* inst, struct Cyc_List_List* ts); void* Cyc_Tcutil_rsubstitute(
struct _RegionHandle* rgn, struct Cyc_List_List* inst, void* t){ void* _temp2035=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar* _temp2081; struct Cyc_Absyn_Structdecl**
_temp2083; struct Cyc_List_List* _temp2085; struct _tuple1* _temp2087; struct
Cyc_Absyn_Uniondecl** _temp2089; struct Cyc_List_List* _temp2091; struct _tuple1*
_temp2093; struct Cyc_Absyn_TunionInfo _temp2095; void* _temp2097; struct Cyc_List_List*
_temp2099; void* _temp2101; struct Cyc_Absyn_TunionFieldInfo _temp2103; struct
Cyc_List_List* _temp2105; void* _temp2107; struct Cyc_Core_Opt* _temp2109;
struct Cyc_List_List* _temp2111; struct _tuple1* _temp2113; struct Cyc_Absyn_Exp*
_temp2115; struct Cyc_Absyn_Tqual _temp2117; void* _temp2119; struct Cyc_Absyn_PtrInfo
_temp2121; struct Cyc_Absyn_Conref* _temp2123; struct Cyc_Absyn_Tqual _temp2125;
struct Cyc_Absyn_Conref* _temp2127; void* _temp2129; void* _temp2131; struct Cyc_Absyn_FnInfo
_temp2133; struct Cyc_List_List* _temp2135; struct Cyc_List_List* _temp2137;
struct Cyc_Absyn_VarargInfo* _temp2139; int _temp2141; struct Cyc_List_List*
_temp2143; void* _temp2145; struct Cyc_Core_Opt* _temp2147; struct Cyc_List_List*
_temp2149; struct Cyc_List_List* _temp2151; struct Cyc_List_List* _temp2153;
struct Cyc_List_List* _temp2155; struct Cyc_Core_Opt* _temp2157; void* _temp2159;
void* _temp2161; struct Cyc_List_List* _temp2163; _LL2037: if(( unsigned int)
_temp2035 > 4u?*(( int*) _temp2035) == Cyc_Absyn_VarType: 0){ _LL2082: _temp2081=((
struct Cyc_Absyn_VarType_struct*) _temp2035)->f1; goto _LL2038;} else{ goto
_LL2039;} _LL2039: if(( unsigned int) _temp2035 > 4u?*(( int*) _temp2035) == Cyc_Absyn_StructType:
0){ _LL2088: _temp2087=(( struct Cyc_Absyn_StructType_struct*) _temp2035)->f1;
goto _LL2086; _LL2086: _temp2085=(( struct Cyc_Absyn_StructType_struct*)
_temp2035)->f2; goto _LL2084; _LL2084: _temp2083=(( struct Cyc_Absyn_StructType_struct*)
_temp2035)->f3; goto _LL2040;} else{ goto _LL2041;} _LL2041: if(( unsigned int)
_temp2035 > 4u?*(( int*) _temp2035) == Cyc_Absyn_UnionType: 0){ _LL2094:
_temp2093=(( struct Cyc_Absyn_UnionType_struct*) _temp2035)->f1; goto _LL2092;
_LL2092: _temp2091=(( struct Cyc_Absyn_UnionType_struct*) _temp2035)->f2; goto
_LL2090; _LL2090: _temp2089=(( struct Cyc_Absyn_UnionType_struct*) _temp2035)->f3;
goto _LL2042;} else{ goto _LL2043;} _LL2043: if(( unsigned int) _temp2035 > 4u?*((
int*) _temp2035) == Cyc_Absyn_TunionType: 0){ _LL2096: _temp2095=(( struct Cyc_Absyn_TunionType_struct*)
_temp2035)->f1; _LL2102: _temp2101=( void*) _temp2095.tunion_info; goto _LL2100;
_LL2100: _temp2099= _temp2095.targs; goto _LL2098; _LL2098: _temp2097=( void*)
_temp2095.rgn; goto _LL2044;} else{ goto _LL2045;} _LL2045: if(( unsigned int)
_temp2035 > 4u?*(( int*) _temp2035) == Cyc_Absyn_TunionFieldType: 0){ _LL2104:
_temp2103=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2035)->f1; _LL2108:
_temp2107=( void*) _temp2103.field_info; goto _LL2106; _LL2106: _temp2105=
_temp2103.targs; goto _LL2046;} else{ goto _LL2047;} _LL2047: if(( unsigned int)
_temp2035 > 4u?*(( int*) _temp2035) == Cyc_Absyn_TypedefType: 0){ _LL2114:
_temp2113=(( struct Cyc_Absyn_TypedefType_struct*) _temp2035)->f1; goto _LL2112;
_LL2112: _temp2111=(( struct Cyc_Absyn_TypedefType_struct*) _temp2035)->f2; goto
_LL2110; _LL2110: _temp2109=(( struct Cyc_Absyn_TypedefType_struct*) _temp2035)->f3;
goto _LL2048;} else{ goto _LL2049;} _LL2049: if(( unsigned int) _temp2035 > 4u?*((
int*) _temp2035) == Cyc_Absyn_ArrayType: 0){ _LL2120: _temp2119=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2035)->f1; goto _LL2118; _LL2118:
_temp2117=(( struct Cyc_Absyn_ArrayType_struct*) _temp2035)->f2; goto _LL2116;
_LL2116: _temp2115=(( struct Cyc_Absyn_ArrayType_struct*) _temp2035)->f3; goto
_LL2050;} else{ goto _LL2051;} _LL2051: if(( unsigned int) _temp2035 > 4u?*((
int*) _temp2035) == Cyc_Absyn_PointerType: 0){ _LL2122: _temp2121=(( struct Cyc_Absyn_PointerType_struct*)
_temp2035)->f1; _LL2132: _temp2131=( void*) _temp2121.elt_typ; goto _LL2130;
_LL2130: _temp2129=( void*) _temp2121.rgn_typ; goto _LL2128; _LL2128: _temp2127=
_temp2121.nullable; goto _LL2126; _LL2126: _temp2125= _temp2121.tq; goto _LL2124;
_LL2124: _temp2123= _temp2121.bounds; goto _LL2052;} else{ goto _LL2053;}
_LL2053: if(( unsigned int) _temp2035 > 4u?*(( int*) _temp2035) == Cyc_Absyn_FnType:
0){ _LL2134: _temp2133=(( struct Cyc_Absyn_FnType_struct*) _temp2035)->f1;
_LL2150: _temp2149= _temp2133.tvars; goto _LL2148; _LL2148: _temp2147= _temp2133.effect;
goto _LL2146; _LL2146: _temp2145=( void*) _temp2133.ret_typ; goto _LL2144;
_LL2144: _temp2143= _temp2133.args; goto _LL2142; _LL2142: _temp2141= _temp2133.c_varargs;
goto _LL2140; _LL2140: _temp2139= _temp2133.cyc_varargs; goto _LL2138; _LL2138:
_temp2137= _temp2133.rgn_po; goto _LL2136; _LL2136: _temp2135= _temp2133.attributes;
goto _LL2054;} else{ goto _LL2055;} _LL2055: if(( unsigned int) _temp2035 > 4u?*((
int*) _temp2035) == Cyc_Absyn_TupleType: 0){ _LL2152: _temp2151=(( struct Cyc_Absyn_TupleType_struct*)
_temp2035)->f1; goto _LL2056;} else{ goto _LL2057;} _LL2057: if(( unsigned int)
_temp2035 > 4u?*(( int*) _temp2035) == Cyc_Absyn_AnonStructType: 0){ _LL2154:
_temp2153=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2035)->f1; goto
_LL2058;} else{ goto _LL2059;} _LL2059: if(( unsigned int) _temp2035 > 4u?*((
int*) _temp2035) == Cyc_Absyn_AnonUnionType: 0){ _LL2156: _temp2155=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp2035)->f1; goto _LL2060;} else{ goto
_LL2061;} _LL2061: if(( unsigned int) _temp2035 > 4u?*(( int*) _temp2035) == Cyc_Absyn_Evar:
0){ _LL2158: _temp2157=(( struct Cyc_Absyn_Evar_struct*) _temp2035)->f2; goto
_LL2062;} else{ goto _LL2063;} _LL2063: if(( unsigned int) _temp2035 > 4u?*((
int*) _temp2035) == Cyc_Absyn_RgnHandleType: 0){ _LL2160: _temp2159=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp2035)->f1; goto _LL2064;} else{
goto _LL2065;} _LL2065: if(( unsigned int) _temp2035 > 4u?*(( int*) _temp2035)
== Cyc_Absyn_EnumType: 0){ goto _LL2066;} else{ goto _LL2067;} _LL2067: if(
_temp2035 ==( void*) Cyc_Absyn_VoidType){ goto _LL2068;} else{ goto _LL2069;}
_LL2069: if(( unsigned int) _temp2035 > 4u?*(( int*) _temp2035) == Cyc_Absyn_IntType:
0){ goto _LL2070;} else{ goto _LL2071;} _LL2071: if( _temp2035 ==( void*) Cyc_Absyn_FloatType){
goto _LL2072;} else{ goto _LL2073;} _LL2073: if( _temp2035 ==( void*) Cyc_Absyn_DoubleType){
goto _LL2074;} else{ goto _LL2075;} _LL2075: if( _temp2035 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL2076;} else{ goto _LL2077;} _LL2077: if(( unsigned int) _temp2035 > 4u?*((
int*) _temp2035) == Cyc_Absyn_AccessEff: 0){ _LL2162: _temp2161=( void*)((
struct Cyc_Absyn_AccessEff_struct*) _temp2035)->f1; goto _LL2078;} else{ goto
_LL2079;} _LL2079: if(( unsigned int) _temp2035 > 4u?*(( int*) _temp2035) == Cyc_Absyn_JoinEff:
0){ _LL2164: _temp2163=(( struct Cyc_Absyn_JoinEff_struct*) _temp2035)->f1; goto
_LL2080;} else{ goto _LL2036;} _LL2038: { struct _handler_cons _temp2165;
_push_handler(& _temp2165);{ int _temp2167= 0; if( setjmp( _temp2165.handler)){
_temp2167= 1;} if( ! _temp2167){{ void* _temp2168=(( void*(*)( int(* compare)(
struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct
Cyc_Absyn_Tvar* x)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp, inst, _temp2081);
_npop_handler( 0u); return _temp2168;}; _pop_handler();} else{ void* _temp2166=(
void*) _exn_thrown; void* _temp2170= _temp2166; _LL2172: if( _temp2170 == Cyc_Core_Not_found){
goto _LL2173;} else{ goto _LL2174;} _LL2174: goto _LL2175; _LL2173: return t;
_LL2175:( void) _throw( _temp2170); _LL2171:;}}} _LL2040: { struct Cyc_List_List*
_temp2176= Cyc_Tcutil_substs( rgn, inst, _temp2085); return _temp2176 ==
_temp2085? t:( void*)({ struct Cyc_Absyn_StructType_struct* _temp2177=( struct
Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp2177[ 0]=({ struct Cyc_Absyn_StructType_struct _temp2178; _temp2178.tag=
Cyc_Absyn_StructType; _temp2178.f1= _temp2087; _temp2178.f2= _temp2176;
_temp2178.f3= _temp2083; _temp2178;}); _temp2177;});} _LL2042: { struct Cyc_List_List*
_temp2179= Cyc_Tcutil_substs( rgn, inst, _temp2091); return _temp2179 ==
_temp2091? t:( void*)({ struct Cyc_Absyn_UnionType_struct* _temp2180=( struct
Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp2180[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp2181; _temp2181.tag= Cyc_Absyn_UnionType;
_temp2181.f1= _temp2093; _temp2181.f2= _temp2179; _temp2181.f3= _temp2089;
_temp2181;}); _temp2180;});} _LL2044: { struct Cyc_List_List* _temp2182= Cyc_Tcutil_substs(
rgn, inst, _temp2099); void* _temp2183= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2097); return( _temp2182 == _temp2099? _temp2183 == _temp2097: 0)? t:( void*)({
struct Cyc_Absyn_TunionType_struct* _temp2184=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2184[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2185; _temp2185.tag= Cyc_Absyn_TunionType;
_temp2185.f1=({ struct Cyc_Absyn_TunionInfo _temp2186; _temp2186.tunion_info=(
void*) _temp2101; _temp2186.targs= _temp2182; _temp2186.rgn=( void*) _temp2183;
_temp2186;}); _temp2185;}); _temp2184;});} _LL2046: { struct Cyc_List_List*
_temp2187= Cyc_Tcutil_substs( rgn, inst, _temp2105); return _temp2187 ==
_temp2105? t:( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp2188=(
struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp2188[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp2189; _temp2189.tag=
Cyc_Absyn_TunionFieldType; _temp2189.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2190; _temp2190.field_info=( void*) _temp2107; _temp2190.targs= _temp2187;
_temp2190;}); _temp2189;}); _temp2188;});} _LL2048: { struct Cyc_List_List*
_temp2191= Cyc_Tcutil_substs( rgn, inst, _temp2111); return _temp2191 ==
_temp2111? t:( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp2192=( struct
Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2192[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp2193; _temp2193.tag=
Cyc_Absyn_TypedefType; _temp2193.f1= _temp2113; _temp2193.f2= _temp2191;
_temp2193.f3= _temp2109; _temp2193;}); _temp2192;});} _LL2050: { void* _temp2194=
Cyc_Tcutil_rsubstitute( rgn, inst, _temp2119); return _temp2194 == _temp2119? t:(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp2195=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2195[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2196; _temp2196.tag= Cyc_Absyn_ArrayType;
_temp2196.f1=( void*) _temp2194; _temp2196.f2= _temp2117; _temp2196.f3=
_temp2115; _temp2196;}); _temp2195;});} _LL2052: { void* _temp2197= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2131); void* _temp2198= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2129); if( _temp2197 == _temp2131? _temp2198 == _temp2129: 0){ return t;}
return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp2199=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp2199[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp2200; _temp2200.tag= Cyc_Absyn_PointerType;
_temp2200.f1=({ struct Cyc_Absyn_PtrInfo _temp2201; _temp2201.elt_typ=( void*)
_temp2197; _temp2201.rgn_typ=( void*) _temp2198; _temp2201.nullable= _temp2127;
_temp2201.tq= _temp2125; _temp2201.bounds= _temp2123; _temp2201;}); _temp2200;});
_temp2199;});} _LL2054:{ struct Cyc_List_List* _temp2202= _temp2149; for( 0;
_temp2202 != 0; _temp2202=(( struct Cyc_List_List*) _check_null( _temp2202))->tl){
inst=({ struct Cyc_List_List* _temp2203=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2203->hd=( void*)({ struct _tuple5*
_temp2204=( struct _tuple5*) _region_malloc( rgn, sizeof( struct _tuple5));
_temp2204->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp2202))->hd; _temp2204->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2205=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2205[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2206; _temp2206.tag= Cyc_Absyn_VarType;
_temp2206.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp2202))->hd; _temp2206;}); _temp2205;}); _temp2204;}); _temp2203->tl= inst;
_temp2203;});}}{ struct Cyc_List_List* _temp2209; struct Cyc_List_List*
_temp2211; struct _tuple0 _temp2207=(( struct _tuple0(*)( struct _RegionHandle*
r3, struct _RegionHandle* r4, struct Cyc_List_List* x)) Cyc_List_rsplit)( rgn,
rgn,(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple16*(* f)(
struct _RegionHandle*, struct _tuple2*), struct _RegionHandle* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_substitute_f1, rgn, _temp2143)); _LL2212:
_temp2211= _temp2207.f1; goto _LL2210; _LL2210: _temp2209= _temp2207.f2; goto
_LL2208; _LL2208: { struct Cyc_List_List* _temp2213= Cyc_Tcutil_substs( rgn,
inst, _temp2209); struct Cyc_List_List* _temp2214=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple16*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_substitute_f2,(( struct Cyc_List_List*(*)( struct _RegionHandle* r3,
struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp2211, _temp2213)); struct Cyc_Core_Opt* eff2; if( _temp2147 == 0){
eff2= 0;} else{ void* _temp2215= Cyc_Tcutil_rsubstitute( rgn, inst,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2147))->v); if( _temp2215 ==( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2147))->v){ eff2= _temp2147;} else{ eff2=({
struct Cyc_Core_Opt* _temp2216=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2216->v=( void*) _temp2215; _temp2216;});}}{ struct Cyc_Absyn_VarargInfo*
cyc_varargs2; if( _temp2139 == 0){ cyc_varargs2= 0;} else{ int _temp2219; void*
_temp2221; void* _temp2223; struct Cyc_Absyn_Tqual _temp2225; struct Cyc_Core_Opt*
_temp2227; struct Cyc_Absyn_VarargInfo _temp2217=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp2139)); _LL2228: _temp2227= _temp2217.name; goto _LL2226;
_LL2226: _temp2225= _temp2217.tq; goto _LL2224; _LL2224: _temp2223=( void*)
_temp2217.type; goto _LL2222; _LL2222: _temp2221=( void*) _temp2217.rgn; goto
_LL2220; _LL2220: _temp2219= _temp2217.inject; goto _LL2218; _LL2218: { void*
_temp2229= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2223); void* _temp2230= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2221); if( _temp2229 == _temp2223? _temp2221 == _temp2230: 0){
cyc_varargs2= _temp2139;} else{ cyc_varargs2=({ struct Cyc_Absyn_VarargInfo*
_temp2231=( struct Cyc_Absyn_VarargInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2231->name= _temp2227; _temp2231->tq= _temp2225; _temp2231->type=( void*)
_temp2229; _temp2231->rgn=( void*) _temp2230; _temp2231->inject= _temp2219;
_temp2231;});}}}{ struct Cyc_List_List* rgn_po2; struct Cyc_List_List* _temp2234;
struct Cyc_List_List* _temp2236; struct _tuple0 _temp2232= Cyc_List_rsplit( rgn,
rgn, _temp2137); _LL2237: _temp2236= _temp2232.f1; goto _LL2235; _LL2235:
_temp2234= _temp2232.f2; goto _LL2233; _LL2233: { struct Cyc_List_List*
_temp2238= Cyc_Tcutil_substs( rgn, inst, _temp2236); struct Cyc_List_List*
_temp2239= Cyc_Tcutil_substs( rgn, inst, _temp2234); if( _temp2238 == _temp2236?
_temp2239 == _temp2234: 0){ rgn_po2= _temp2137;} else{ rgn_po2= Cyc_List_zip(
_temp2238, _temp2239);} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2240=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2240[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2241; _temp2241.tag= Cyc_Absyn_FnType;
_temp2241.f1=({ struct Cyc_Absyn_FnInfo _temp2242; _temp2242.tvars= _temp2149;
_temp2242.effect= eff2; _temp2242.ret_typ=( void*) Cyc_Tcutil_rsubstitute( rgn,
inst, _temp2145); _temp2242.args= _temp2214; _temp2242.c_varargs= _temp2141;
_temp2242.cyc_varargs= cyc_varargs2; _temp2242.rgn_po= rgn_po2; _temp2242.attributes=
_temp2135; _temp2242;}); _temp2241;}); _temp2240;});}}}}} _LL2056: { struct Cyc_List_List*
_temp2243=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_snd_tqt,
_temp2151); struct Cyc_List_List* _temp2244= Cyc_Tcutil_substs( rgn, inst,
_temp2243); if( _temp2244 == _temp2243){ return t;}{ struct Cyc_List_List*
_temp2245=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*,
void*), struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_map2)( Cyc_Tcutil_map2_tq,
_temp2151, _temp2244); return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp2246=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp2246[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp2247; _temp2247.tag= Cyc_Absyn_TupleType;
_temp2247.f1= _temp2245; _temp2247;}); _temp2246;});}} _LL2058: { struct Cyc_List_List*
_temp2248=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2153); struct Cyc_List_List* _temp2249= Cyc_Tcutil_substs( rgn, inst,
_temp2248); if( _temp2249 == _temp2248){ return t;}{ struct Cyc_List_List*
_temp2250=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2153, _temp2249); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp2251=( struct Cyc_Absyn_AnonStructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2251[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2252; _temp2252.tag= Cyc_Absyn_AnonStructType;
_temp2252.f1= _temp2250; _temp2252;}); _temp2251;});}} _LL2060: { struct Cyc_List_List*
_temp2253=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2155); struct Cyc_List_List* _temp2254= Cyc_Tcutil_substs( rgn, inst,
_temp2253); if( _temp2254 == _temp2253){ return t;}{ struct Cyc_List_List*
_temp2255=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2155, _temp2254); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp2256=( struct Cyc_Absyn_AnonStructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2256[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2257; _temp2257.tag= Cyc_Absyn_AnonStructType;
_temp2257.f1= _temp2255; _temp2257;}); _temp2256;});}} _LL2062: if( _temp2157 !=
0){ return Cyc_Tcutil_rsubstitute( rgn, inst,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2157))->v);} else{ return t;} _LL2064: { void* _temp2258= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2159); return _temp2258 == _temp2159? t:( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp2259=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp2259[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp2260; _temp2260.tag= Cyc_Absyn_RgnHandleType; _temp2260.f1=( void*)
_temp2258; _temp2260;}); _temp2259;});} _LL2066: return t; _LL2068: return t;
_LL2070: return t; _LL2072: return t; _LL2074: return t; _LL2076: return t;
_LL2078: { void* _temp2261= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2161);
return _temp2261 == _temp2161? t:( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp2262=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2262[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2263; _temp2263.tag= Cyc_Absyn_AccessEff;
_temp2263.f1=( void*) _temp2261; _temp2263;}); _temp2262;});} _LL2080: { struct
Cyc_List_List* _temp2264= Cyc_Tcutil_substs( rgn, inst, _temp2163); return
_temp2264 == _temp2163? t:( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2265=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2265[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2266; _temp2266.tag= Cyc_Absyn_JoinEff;
_temp2266.f1= _temp2264; _temp2266;}); _temp2265;});} _LL2036:;} static struct
Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List*
inst, struct Cyc_List_List* ts){ if( ts == 0){ return 0;}{ void* _temp2267=(
void*)(( struct Cyc_List_List*) _check_null( ts))->hd; struct Cyc_List_List*
_temp2268=(( struct Cyc_List_List*) _check_null( ts))->tl; void* _temp2269= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2267); struct Cyc_List_List* _temp2270= Cyc_Tcutil_substs( rgn,
inst, _temp2268); if( _temp2267 == _temp2269? _temp2268 == _temp2270: 0){ return
ts;} return( struct Cyc_List_List*)({ struct Cyc_List_List* _temp2271=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2271->hd=( void*)
_temp2269; _temp2271->tl= _temp2270; _temp2271;});}} extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List* inst, void* t){ return Cyc_Tcutil_rsubstitute( Cyc_Core_heap_region,
inst, t);} struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2320=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2320[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2321; _temp2321.tag= Cyc_Absyn_Const_e; _temp2321.f1=(
void*)(( void*) Cyc_Absyn_Null_c); _temp2321;}); _temp2320;}), loc);{ void*
_temp2272= Cyc_Tcutil_compress( t); void* _temp2286; void* _temp2288; void*
_temp2290; void* _temp2292; _LL2274: if(( unsigned int) _temp2272 > 4u?*(( int*)
_temp2272) == Cyc_Absyn_PointerType: 0){ goto _LL2275;} else{ goto _LL2276;}
_LL2276: if(( unsigned int) _temp2272 > 4u?*(( int*) _temp2272) == Cyc_Absyn_IntType:
0){ _LL2289: _temp2288=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2272)->f1;
goto _LL2287; _LL2287: _temp2286=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2272)->f2; if( _temp2286 ==( void*) Cyc_Absyn_B1){ goto _LL2277;} else{
goto _LL2278;}} else{ goto _LL2278;} _LL2278: if(( unsigned int) _temp2272 > 4u?*((
int*) _temp2272) == Cyc_Absyn_IntType: 0){ _LL2293: _temp2292=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2272)->f1; goto _LL2291; _LL2291: _temp2290=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2272)->f2; goto _LL2279;} else{
goto _LL2280;} _LL2280: if( _temp2272 ==( void*) Cyc_Absyn_FloatType){ goto
_LL2281;} else{ goto _LL2282;} _LL2282: if( _temp2272 ==( void*) Cyc_Absyn_DoubleType){
goto _LL2283;} else{ goto _LL2284;} _LL2284: goto _LL2285; _LL2275: goto _LL2273;
_LL2277:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2294=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2294[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2295; _temp2295.tag= Cyc_Absyn_Const_e;
_temp2295.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp2296=(
struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp2296[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp2297; _temp2297.tag= Cyc_Absyn_Char_c;
_temp2297.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2297.f2='\000'; _temp2297;});
_temp2296;})); _temp2295;}); _temp2294;}))); goto _LL2273; _LL2279:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2298=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2298[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2299; _temp2299.tag= Cyc_Absyn_Const_e; _temp2299.f1=(
void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp2300=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp2300[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp2301; _temp2301.tag= Cyc_Absyn_Int_c; _temp2301.f1=( void*) _temp2292;
_temp2301.f2= 0; _temp2301;}); _temp2300;})); _temp2299;}); _temp2298;}))); if(
_temp2290 !=( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp2302=( struct Cyc_Absyn_Cast_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp2302[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp2303; _temp2303.tag= Cyc_Absyn_Cast_e;
_temp2303.f1=( void*) t; _temp2303.f2= e; _temp2303;}); _temp2302;}), loc);}
goto _LL2273; _LL2281:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2304=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2304[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2305; _temp2305.tag= Cyc_Absyn_Const_e;
_temp2305.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2306=(
struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2306[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2307; _temp2307.tag= Cyc_Absyn_Float_c;
_temp2307.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp2307;});
_temp2306;})); _temp2305;}); _temp2304;}))); goto _LL2273; _LL2283:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2308=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2308[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2309; _temp2309.tag= Cyc_Absyn_Cast_e; _temp2309.f1=( void*) t; _temp2309.f2=
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2310=( struct
Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2310[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2311; _temp2311.tag= Cyc_Absyn_Const_e;
_temp2311.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2312=(
struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2312[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2313; _temp2313.tag= Cyc_Absyn_Float_c;
_temp2313.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp2313;});
_temp2312;})); _temp2311;}); _temp2310;}), loc); _temp2309;}); _temp2308;})));
goto _LL2273; _LL2285: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp2314)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp2318= _tag_arr("declaration of type %s requires initializer",
sizeof( unsigned char), 44u); struct Cyc_Stdio_String_pa_struct _temp2319;
_temp2319.tag= Cyc_Stdio_String_pa; _temp2319.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp2317=( void*)& _temp2319; void* _temp2315[ 1u]={ _temp2317};
struct _tagged_arr _temp2316={( void*) _temp2315,( void*) _temp2315,( void*)(
_temp2315 + 1u)}; _temp2314( _temp2318, _temp2316);}})); goto _LL2273; _LL2273:;}
return e;} struct _tuple5* Cyc_Tcutil_make_inst_var( struct Cyc_List_List* s,
struct Cyc_Absyn_Tvar* tv){ void* k= Cyc_Absyn_conref_val( tv->kind); return({
struct _tuple5* _temp2322=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp2322->f1= tv; _temp2322->f2= Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt*
_temp2323=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2323->v=( void*) k; _temp2323;}),({ struct Cyc_Core_Opt* _temp2324=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2324->v=( void*) s;
_temp2324;})); _temp2322;});} struct _tuple5* Cyc_Tcutil_r_make_inst_var( struct
_tuple6* env, struct Cyc_Absyn_Tvar* tv){ struct _tuple6 _temp2327; struct
_RegionHandle* _temp2328; struct Cyc_List_List* _temp2330; struct _tuple6*
_temp2325= env; _temp2327=* _temp2325; _LL2331: _temp2330= _temp2327.f1; goto
_LL2329; _LL2329: _temp2328= _temp2327.f2; goto _LL2326; _LL2326: { void* k= Cyc_Absyn_conref_val(
tv->kind); return({ struct _tuple5* _temp2332=( struct _tuple5*) _region_malloc(
_temp2328, sizeof( struct _tuple5)); _temp2332->f1= tv; _temp2332->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp2333=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2333->v=( void*) k; _temp2333;}),({ struct Cyc_Core_Opt*
_temp2334=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2334->v=( void*) _temp2330; _temp2334;})); _temp2332;});}} static struct
Cyc_List_List* Cyc_Tcutil_add_free_tvar( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 != 0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
if( Cyc_String_zstrptrcmp((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->name, tv->name) == 0){ struct Cyc_Absyn_Conref* k1=((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->kind;
struct Cyc_Absyn_Conref* k2= tv->kind; if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp2335)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp2341= _tag_arr("type variable %s is used with inconsistent kinds %s and %s",
sizeof( unsigned char), 59u); struct Cyc_Stdio_String_pa_struct _temp2344;
_temp2344.tag= Cyc_Stdio_String_pa; _temp2344.f1=( struct _tagged_arr)* tv->name;{
void* _temp2338=( void*)& _temp2344; struct Cyc_Stdio_String_pa_struct _temp2343;
_temp2343.tag= Cyc_Stdio_String_pa; _temp2343.f1=( struct _tagged_arr) Cyc_Absynpp_ckind2string(
k1);{ void* _temp2339=( void*)& _temp2343; struct Cyc_Stdio_String_pa_struct
_temp2342; _temp2342.tag= Cyc_Stdio_String_pa; _temp2342.f1=( struct _tagged_arr)
Cyc_Absynpp_ckind2string( k2);{ void* _temp2340=( void*)& _temp2342; void*
_temp2336[ 3u]={ _temp2338, _temp2339, _temp2340}; struct _tagged_arr _temp2337={(
void*) _temp2336,( void*) _temp2336,( void*)( _temp2336 + 3u)}; _temp2335(
_temp2341, _temp2337);}}}}));} if( tv->identity == 0){ tv->identity=(( struct
Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->identity;}
else{ if(*(( int*) _check_null( tv->identity)) !=*(( int*) _check_null((( struct
Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->identity))){((
int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)(( struct _tagged_arr)({
struct _tagged_arr(* _temp2345)( struct _tagged_arr fmt, struct _tagged_arr)=
Cyc_Stdio_aprintf; struct _tagged_arr _temp2348= _tag_arr("same type variable has different identity!",
sizeof( unsigned char), 43u); void* _temp2346[ 0u]={}; struct _tagged_arr
_temp2347={( void*) _temp2346,( void*) _temp2346,( void*)( _temp2346 + 0u)};
_temp2345( _temp2348, _temp2347);}));}} return tvs;}}} tv->identity= Cyc_Tcutil_new_tvar_id();
return({ struct Cyc_List_List* _temp2349=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2349->hd=( void*) tv; _temp2349->tl= tvs;
_temp2349;});} static struct Cyc_List_List* Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){ if( tv->identity == 0){((
int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)( _tag_arr("fast_add_free_tvar: bad identity in tv",
sizeof( unsigned char), 39u));}{ struct Cyc_List_List* tvs2= tvs; for( 0; tvs2
!= 0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){ if((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->identity == 0){(( int(*)(
struct _tagged_arr msg)) Cyc_Tcutil_impos)( _tag_arr("fast_add_free_tvar: bad identity in tvs2",
sizeof( unsigned char), 41u));} if(*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->identity)) ==*(( int*)
_check_null( tv->identity))){ return tvs;}}} return({ struct Cyc_List_List*
_temp2350=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2350->hd=( void*) tv; _temp2350->tl= tvs; _temp2350;});} static struct Cyc_List_List*
Cyc_Tcutil_add_bound_tvar( struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){
if( tv->identity == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)((
struct _tagged_arr)({ struct _tagged_arr(* _temp2351)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp2355= _tag_arr("bound tvar id for %s is null",
sizeof( unsigned char), 29u); struct Cyc_Stdio_String_pa_struct _temp2356;
_temp2356.tag= Cyc_Stdio_String_pa; _temp2356.f1=( struct _tagged_arr) Cyc_Tcutil_tvar2string(
tv);{ void* _temp2354=( void*)& _temp2356; void* _temp2352[ 1u]={ _temp2354};
struct _tagged_arr _temp2353={( void*) _temp2352,( void*) _temp2352,( void*)(
_temp2352 + 1u)}; _temp2351( _temp2355, _temp2353);}}));} return({ struct Cyc_List_List*
_temp2357=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2357->hd=( void*) tv; _temp2357->tl= tvs; _temp2357;});} static struct Cyc_List_List*
Cyc_Tcutil_add_free_evar( struct Cyc_List_List* es, void* e){ void* _temp2358=
Cyc_Tcutil_compress( e); int _temp2364; _LL2360: if(( unsigned int) _temp2358 >
4u?*(( int*) _temp2358) == Cyc_Absyn_Evar: 0){ _LL2365: _temp2364=(( struct Cyc_Absyn_Evar_struct*)
_temp2358)->f3; goto _LL2361;} else{ goto _LL2362;} _LL2362: goto _LL2363;
_LL2361:{ struct Cyc_List_List* es2= es; for( 0; es2 != 0; es2=(( struct Cyc_List_List*)
_check_null( es2))->tl){ void* _temp2366= Cyc_Tcutil_compress(( void*)(( struct
Cyc_List_List*) _check_null( es2))->hd); int _temp2372; _LL2368: if((
unsigned int) _temp2366 > 4u?*(( int*) _temp2366) == Cyc_Absyn_Evar: 0){ _LL2373:
_temp2372=(( struct Cyc_Absyn_Evar_struct*) _temp2366)->f3; goto _LL2369;} else{
goto _LL2370;} _LL2370: goto _LL2371; _LL2369: if( _temp2364 == _temp2372){
return es;} goto _LL2367; _LL2371: goto _LL2367; _LL2367:;}} return({ struct Cyc_List_List*
_temp2374=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2374->hd=( void*) e; _temp2374->tl= es; _temp2374;}); _LL2363: return es;
_LL2359:;} static struct Cyc_List_List* Cyc_Tcutil_remove_bound_tvars( struct
Cyc_List_List* tvs, struct Cyc_List_List* btvs){ struct Cyc_List_List* r= 0;
for( 0; tvs != 0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ int
present= 0;{ struct Cyc_List_List* b= btvs; for( 0; b != 0; b=(( struct Cyc_List_List*)
_check_null( b))->tl){ if(*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->identity)) ==*(( int*)
_check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd)->identity))){
present= 1; break;}}} if( ! present){ r=({ struct Cyc_List_List* _temp2375=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2375->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd);
_temp2375->tl= r; _temp2375;});}} r=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( r); return r;} struct Cyc_Tcutil_CVTEnv{ struct Cyc_List_List*
kind_env; struct Cyc_List_List* free_vars; struct Cyc_List_List* free_evars; } ;
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcutil_CVTEnv cvtenv, void*
expected_kind, void* t){{ void* _temp2376= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt*
_temp2422; struct Cyc_Core_Opt** _temp2424; struct Cyc_Absyn_Tvar* _temp2425;
struct Cyc_Absyn_Enumdecl* _temp2427; struct Cyc_Absyn_Enumdecl** _temp2429;
struct _tuple1* _temp2430; struct Cyc_Absyn_TunionInfo _temp2432; void*
_temp2434; struct Cyc_List_List* _temp2436; void* _temp2438; void** _temp2440;
struct Cyc_Absyn_TunionFieldInfo _temp2441; struct Cyc_List_List* _temp2443;
void* _temp2445; void** _temp2447; struct Cyc_Absyn_PtrInfo _temp2448; struct
Cyc_Absyn_Conref* _temp2450; struct Cyc_Absyn_Tqual _temp2452; struct Cyc_Absyn_Conref*
_temp2454; void* _temp2456; void* _temp2458; struct Cyc_Absyn_Exp* _temp2460;
struct Cyc_Absyn_Tqual _temp2462; void* _temp2464; struct Cyc_Absyn_FnInfo
_temp2466; struct Cyc_List_List* _temp2468; struct Cyc_List_List* _temp2470;
struct Cyc_Absyn_VarargInfo* _temp2472; int _temp2474; struct Cyc_List_List*
_temp2476; void* _temp2478; struct Cyc_Core_Opt* _temp2480; struct Cyc_Core_Opt**
_temp2482; struct Cyc_List_List* _temp2483; struct Cyc_List_List** _temp2485;
struct Cyc_List_List* _temp2486; struct Cyc_List_List* _temp2488; struct Cyc_List_List*
_temp2490; struct Cyc_Absyn_Structdecl** _temp2492; struct Cyc_Absyn_Structdecl***
_temp2494; struct Cyc_List_List* _temp2495; struct _tuple1* _temp2497; struct
Cyc_Absyn_Uniondecl** _temp2499; struct Cyc_Absyn_Uniondecl*** _temp2501; struct
Cyc_List_List* _temp2502; struct _tuple1* _temp2504; struct Cyc_Core_Opt*
_temp2506; struct Cyc_Core_Opt** _temp2508; struct Cyc_List_List* _temp2509;
struct _tuple1* _temp2511; void* _temp2513; void* _temp2515; struct Cyc_List_List*
_temp2517; _LL2378: if( _temp2376 ==( void*) Cyc_Absyn_VoidType){ goto _LL2379;}
else{ goto _LL2380;} _LL2380: if(( unsigned int) _temp2376 > 4u?*(( int*)
_temp2376) == Cyc_Absyn_Evar: 0){ _LL2423: _temp2422=(( struct Cyc_Absyn_Evar_struct*)
_temp2376)->f1; _temp2424=&(( struct Cyc_Absyn_Evar_struct*) _temp2376)->f1;
goto _LL2381;} else{ goto _LL2382;} _LL2382: if(( unsigned int) _temp2376 > 4u?*((
int*) _temp2376) == Cyc_Absyn_VarType: 0){ _LL2426: _temp2425=(( struct Cyc_Absyn_VarType_struct*)
_temp2376)->f1; goto _LL2383;} else{ goto _LL2384;} _LL2384: if(( unsigned int)
_temp2376 > 4u?*(( int*) _temp2376) == Cyc_Absyn_EnumType: 0){ _LL2431:
_temp2430=(( struct Cyc_Absyn_EnumType_struct*) _temp2376)->f1; goto _LL2428;
_LL2428: _temp2427=(( struct Cyc_Absyn_EnumType_struct*) _temp2376)->f2;
_temp2429=&(( struct Cyc_Absyn_EnumType_struct*) _temp2376)->f2; goto _LL2385;}
else{ goto _LL2386;} _LL2386: if(( unsigned int) _temp2376 > 4u?*(( int*)
_temp2376) == Cyc_Absyn_TunionType: 0){ _LL2433: _temp2432=(( struct Cyc_Absyn_TunionType_struct*)
_temp2376)->f1; _LL2439: _temp2438=( void*) _temp2432.tunion_info; _temp2440=&(((
struct Cyc_Absyn_TunionType_struct*) _temp2376)->f1).tunion_info; goto _LL2437;
_LL2437: _temp2436= _temp2432.targs; goto _LL2435; _LL2435: _temp2434=( void*)
_temp2432.rgn; goto _LL2387;} else{ goto _LL2388;} _LL2388: if(( unsigned int)
_temp2376 > 4u?*(( int*) _temp2376) == Cyc_Absyn_TunionFieldType: 0){ _LL2442:
_temp2441=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2376)->f1; _LL2446:
_temp2445=( void*) _temp2441.field_info; _temp2447=&((( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2376)->f1).field_info; goto _LL2444; _LL2444: _temp2443= _temp2441.targs;
goto _LL2389;} else{ goto _LL2390;} _LL2390: if(( unsigned int) _temp2376 > 4u?*((
int*) _temp2376) == Cyc_Absyn_PointerType: 0){ _LL2449: _temp2448=(( struct Cyc_Absyn_PointerType_struct*)
_temp2376)->f1; _LL2459: _temp2458=( void*) _temp2448.elt_typ; goto _LL2457;
_LL2457: _temp2456=( void*) _temp2448.rgn_typ; goto _LL2455; _LL2455: _temp2454=
_temp2448.nullable; goto _LL2453; _LL2453: _temp2452= _temp2448.tq; goto _LL2451;
_LL2451: _temp2450= _temp2448.bounds; goto _LL2391;} else{ goto _LL2392;}
_LL2392: if(( unsigned int) _temp2376 > 4u?*(( int*) _temp2376) == Cyc_Absyn_IntType:
0){ goto _LL2393;} else{ goto _LL2394;} _LL2394: if( _temp2376 ==( void*) Cyc_Absyn_FloatType){
goto _LL2395;} else{ goto _LL2396;} _LL2396: if( _temp2376 ==( void*) Cyc_Absyn_DoubleType){
goto _LL2397;} else{ goto _LL2398;} _LL2398: if(( unsigned int) _temp2376 > 4u?*((
int*) _temp2376) == Cyc_Absyn_ArrayType: 0){ _LL2465: _temp2464=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2376)->f1; goto _LL2463; _LL2463:
_temp2462=(( struct Cyc_Absyn_ArrayType_struct*) _temp2376)->f2; goto _LL2461;
_LL2461: _temp2460=(( struct Cyc_Absyn_ArrayType_struct*) _temp2376)->f3; goto
_LL2399;} else{ goto _LL2400;} _LL2400: if(( unsigned int) _temp2376 > 4u?*((
int*) _temp2376) == Cyc_Absyn_FnType: 0){ _LL2467: _temp2466=(( struct Cyc_Absyn_FnType_struct*)
_temp2376)->f1; _LL2484: _temp2483= _temp2466.tvars; _temp2485=&((( struct Cyc_Absyn_FnType_struct*)
_temp2376)->f1).tvars; goto _LL2481; _LL2481: _temp2480= _temp2466.effect;
_temp2482=&((( struct Cyc_Absyn_FnType_struct*) _temp2376)->f1).effect; goto
_LL2479; _LL2479: _temp2478=( void*) _temp2466.ret_typ; goto _LL2477; _LL2477:
_temp2476= _temp2466.args; goto _LL2475; _LL2475: _temp2474= _temp2466.c_varargs;
goto _LL2473; _LL2473: _temp2472= _temp2466.cyc_varargs; goto _LL2471; _LL2471:
_temp2470= _temp2466.rgn_po; goto _LL2469; _LL2469: _temp2468= _temp2466.attributes;
goto _LL2401;} else{ goto _LL2402;} _LL2402: if(( unsigned int) _temp2376 > 4u?*((
int*) _temp2376) == Cyc_Absyn_TupleType: 0){ _LL2487: _temp2486=(( struct Cyc_Absyn_TupleType_struct*)
_temp2376)->f1; goto _LL2403;} else{ goto _LL2404;} _LL2404: if(( unsigned int)
_temp2376 > 4u?*(( int*) _temp2376) == Cyc_Absyn_AnonStructType: 0){ _LL2489:
_temp2488=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2376)->f1; goto
_LL2405;} else{ goto _LL2406;} _LL2406: if(( unsigned int) _temp2376 > 4u?*((
int*) _temp2376) == Cyc_Absyn_AnonUnionType: 0){ _LL2491: _temp2490=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp2376)->f1; goto _LL2407;} else{ goto
_LL2408;} _LL2408: if(( unsigned int) _temp2376 > 4u?*(( int*) _temp2376) == Cyc_Absyn_StructType:
0){ _LL2498: _temp2497=(( struct Cyc_Absyn_StructType_struct*) _temp2376)->f1;
goto _LL2496; _LL2496: _temp2495=(( struct Cyc_Absyn_StructType_struct*)
_temp2376)->f2; goto _LL2493; _LL2493: _temp2492=(( struct Cyc_Absyn_StructType_struct*)
_temp2376)->f3; _temp2494=&(( struct Cyc_Absyn_StructType_struct*) _temp2376)->f3;
goto _LL2409;} else{ goto _LL2410;} _LL2410: if(( unsigned int) _temp2376 > 4u?*((
int*) _temp2376) == Cyc_Absyn_UnionType: 0){ _LL2505: _temp2504=(( struct Cyc_Absyn_UnionType_struct*)
_temp2376)->f1; goto _LL2503; _LL2503: _temp2502=(( struct Cyc_Absyn_UnionType_struct*)
_temp2376)->f2; goto _LL2500; _LL2500: _temp2499=(( struct Cyc_Absyn_UnionType_struct*)
_temp2376)->f3; _temp2501=&(( struct Cyc_Absyn_UnionType_struct*) _temp2376)->f3;
goto _LL2411;} else{ goto _LL2412;} _LL2412: if(( unsigned int) _temp2376 > 4u?*((
int*) _temp2376) == Cyc_Absyn_TypedefType: 0){ _LL2512: _temp2511=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2376)->f1; goto _LL2510; _LL2510: _temp2509=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2376)->f2; goto _LL2507; _LL2507: _temp2506=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2376)->f3; _temp2508=&(( struct Cyc_Absyn_TypedefType_struct*) _temp2376)->f3;
goto _LL2413;} else{ goto _LL2414;} _LL2414: if( _temp2376 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL2415;} else{ goto _LL2416;} _LL2416: if(( unsigned int) _temp2376 > 4u?*((
int*) _temp2376) == Cyc_Absyn_RgnHandleType: 0){ _LL2514: _temp2513=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp2376)->f1; goto _LL2417;} else{
goto _LL2418;} _LL2418: if(( unsigned int) _temp2376 > 4u?*(( int*) _temp2376)
== Cyc_Absyn_AccessEff: 0){ _LL2516: _temp2515=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp2376)->f1; goto _LL2419;} else{ goto _LL2420;} _LL2420: if(( unsigned int)
_temp2376 > 4u?*(( int*) _temp2376) == Cyc_Absyn_JoinEff: 0){ _LL2518: _temp2517=((
struct Cyc_Absyn_JoinEff_struct*) _temp2376)->f1; goto _LL2421;} else{ goto
_LL2377;} _LL2379: goto _LL2377; _LL2381: if(* _temp2424 == 0){* _temp2424=({
struct Cyc_Core_Opt* _temp2519=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2519->v=( void*) expected_kind; _temp2519;});} cvtenv.free_evars=
Cyc_Tcutil_add_free_evar( cvtenv.free_evars, t); goto _LL2377; _LL2383: { struct
Cyc_Absyn_Conref* c= Cyc_Absyn_compress_conref( _temp2425->kind); if(( void*) c->v
==( void*) Cyc_Absyn_No_constr){( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2520=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2520[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2521; _temp2521.tag= Cyc_Absyn_Eq_constr;
_temp2521.f1=( void*) expected_kind; _temp2521;}); _temp2520;})));} cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar( loc, cvtenv.kind_env, _temp2425); cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars, _temp2425); goto _LL2377;}
_LL2385: if(* _temp2429 == 0){ struct _handler_cons _temp2522; _push_handler(&
_temp2522);{ int _temp2524= 0; if( setjmp( _temp2522.handler)){ _temp2524= 1;}
if( ! _temp2524){{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl( te,
loc, _temp2430);* _temp2429=( struct Cyc_Absyn_Enumdecl*)* ed;}; _pop_handler();}
else{ void* _temp2523=( void*) _exn_thrown; void* _temp2526= _temp2523; _LL2528:
if( _temp2526 == Cyc_Dict_Absent){ goto _LL2529;} else{ goto _LL2530;} _LL2530:
goto _LL2531; _LL2529: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp2532)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp2536= _tag_arr("unbound type enum %s", sizeof(
unsigned char), 21u); struct Cyc_Stdio_String_pa_struct _temp2537; _temp2537.tag=
Cyc_Stdio_String_pa; _temp2537.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2430);{ void* _temp2535=( void*)& _temp2537; void* _temp2533[ 1u]={
_temp2535}; struct _tagged_arr _temp2534={( void*) _temp2533,( void*) _temp2533,(
void*)( _temp2533 + 1u)}; _temp2532( _temp2536, _temp2534);}})); return cvtenv;
_LL2531:( void) _throw( _temp2526); _LL2527:;}}}{ struct Cyc_Absyn_Enumdecl* ed=(
struct Cyc_Absyn_Enumdecl*) _check_null(* _temp2429);* _temp2430=( ed->name)[
_check_known_subscript_notnull( 1u, 0)]; goto _LL2377;} _LL2387:{ void*
_temp2538=* _temp2440; struct Cyc_Absyn_UnknownTunionInfo _temp2544; int
_temp2546; struct _tuple1* _temp2548; struct Cyc_Absyn_Tuniondecl* _temp2550;
_LL2540: if(*(( int*) _temp2538) == Cyc_Absyn_UnknownTunion){ _LL2545: _temp2544=((
struct Cyc_Absyn_UnknownTunion_struct*) _temp2538)->f1; _LL2549: _temp2548=
_temp2544.name; goto _LL2547; _LL2547: _temp2546= _temp2544.is_xtunion; goto
_LL2541;} else{ goto _LL2542;} _LL2542: if(*(( int*) _temp2538) == Cyc_Absyn_KnownTunion){
_LL2551: _temp2550=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2538)->f1; goto
_LL2543;} else{ goto _LL2539;} _LL2541: { struct Cyc_Absyn_Tuniondecl* tud;{
struct _handler_cons _temp2552; _push_handler(& _temp2552);{ int _temp2554= 0;
if( setjmp( _temp2552.handler)){ _temp2554= 1;} if( ! _temp2554){ tud=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp2548);; _pop_handler();} else{ void* _temp2553=( void*)
_exn_thrown; void* _temp2556= _temp2553; _LL2558: if( _temp2556 == Cyc_Dict_Absent){
goto _LL2559;} else{ goto _LL2560;} _LL2560: goto _LL2561; _LL2559: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp2562)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp2566=
_tag_arr("unbound type [x]tunion %s", sizeof( unsigned char), 26u); struct Cyc_Stdio_String_pa_struct
_temp2567; _temp2567.tag= Cyc_Stdio_String_pa; _temp2567.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2548);{ void* _temp2565=( void*)& _temp2567; void*
_temp2563[ 1u]={ _temp2565}; struct _tagged_arr _temp2564={( void*) _temp2563,(
void*) _temp2563,( void*)( _temp2563 + 1u)}; _temp2562( _temp2566, _temp2564);}}));
return cvtenv; _LL2561:( void) _throw( _temp2556); _LL2557:;}}} if( tud->is_xtunion
!= _temp2546){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp2568)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp2572= _tag_arr("[x]tunion is different from type declaration %s",
sizeof( unsigned char), 48u); struct Cyc_Stdio_String_pa_struct _temp2573;
_temp2573.tag= Cyc_Stdio_String_pa; _temp2573.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2548);{ void* _temp2571=( void*)& _temp2573; void* _temp2569[ 1u]={
_temp2571}; struct _tagged_arr _temp2570={( void*) _temp2569,( void*) _temp2569,(
void*)( _temp2569 + 1u)}; _temp2568( _temp2572, _temp2570);}}));}* _temp2440=(
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp2574=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp2574[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp2575; _temp2575.tag= Cyc_Absyn_KnownTunion;
_temp2575.f1= tud; _temp2575;}); _temp2574;}); _temp2550= tud; goto _LL2543;}
_LL2543: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2434);{ struct Cyc_List_List* tvs= _temp2550->tvs; for( 0; _temp2436 != 0?
tvs != 0: 0;( _temp2436=(( struct Cyc_List_List*) _check_null( _temp2436))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2436))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} if( _temp2436
!= 0){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp2576)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp2580= _tag_arr("too many type arguments for tunion %s",
sizeof( unsigned char), 38u); struct Cyc_Stdio_String_pa_struct _temp2581;
_temp2581.tag= Cyc_Stdio_String_pa; _temp2581.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2550->name);{ void* _temp2579=( void*)& _temp2581; void* _temp2577[ 1u]={
_temp2579}; struct _tagged_arr _temp2578={( void*) _temp2577,( void*) _temp2577,(
void*)( _temp2577 + 1u)}; _temp2576( _temp2580, _temp2578);}}));} if( tvs != 0){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp2582)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp2586= _tag_arr("too few type arguments for tunion %s", sizeof(
unsigned char), 37u); struct Cyc_Stdio_String_pa_struct _temp2587; _temp2587.tag=
Cyc_Stdio_String_pa; _temp2587.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2550->name);{ void* _temp2585=( void*)& _temp2587; void* _temp2583[ 1u]={
_temp2585}; struct _tagged_arr _temp2584={( void*) _temp2583,( void*) _temp2583,(
void*)( _temp2583 + 1u)}; _temp2582( _temp2586, _temp2584);}}));} goto _LL2539;}
_LL2539:;} goto _LL2377; _LL2389:{ void* _temp2588=* _temp2447; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp2594; int _temp2596; struct _tuple1* _temp2598; struct _tuple1* _temp2600;
struct Cyc_Absyn_Tunionfield* _temp2602; struct Cyc_Absyn_Tuniondecl* _temp2604;
_LL2590: if(*(( int*) _temp2588) == Cyc_Absyn_UnknownTunionfield){ _LL2595:
_temp2594=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp2588)->f1;
_LL2601: _temp2600= _temp2594.tunion_name; goto _LL2599; _LL2599: _temp2598=
_temp2594.field_name; goto _LL2597; _LL2597: _temp2596= _temp2594.is_xtunion;
goto _LL2591;} else{ goto _LL2592;} _LL2592: if(*(( int*) _temp2588) == Cyc_Absyn_KnownTunionfield){
_LL2605: _temp2604=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp2588)->f1;
goto _LL2603; _LL2603: _temp2602=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2588)->f2; goto _LL2593;} else{ goto _LL2589;} _LL2591: { struct Cyc_Absyn_Tuniondecl*
tud; struct Cyc_Absyn_Tunionfield* tuf;{ struct _handler_cons _temp2606;
_push_handler(& _temp2606);{ int _temp2608= 0; if( setjmp( _temp2606.handler)){
_temp2608= 1;} if( ! _temp2608){* Cyc_Tcenv_lookup_tuniondecl( te, loc,
_temp2600);; _pop_handler();} else{ void* _temp2607=( void*) _exn_thrown; void*
_temp2610= _temp2607; _LL2612: if( _temp2610 == Cyc_Dict_Absent){ goto _LL2613;}
else{ goto _LL2614;} _LL2614: goto _LL2615; _LL2613: Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp2616)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp2620= _tag_arr("unbound type tunion %s",
sizeof( unsigned char), 23u); struct Cyc_Stdio_String_pa_struct _temp2621;
_temp2621.tag= Cyc_Stdio_String_pa; _temp2621.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2600);{ void* _temp2619=( void*)& _temp2621; void* _temp2617[ 1u]={
_temp2619}; struct _tagged_arr _temp2618={( void*) _temp2617,( void*) _temp2617,(
void*)( _temp2617 + 1u)}; _temp2616( _temp2620, _temp2618);}})); return cvtenv;
_LL2615:( void) _throw( _temp2610); _LL2611:;}}}{ struct _handler_cons _temp2622;
_push_handler(& _temp2622);{ int _temp2624= 0; if( setjmp( _temp2622.handler)){
_temp2624= 1;} if( ! _temp2624){{ void* _temp2625= Cyc_Tcenv_lookup_ordinary( te,
loc, _temp2598); struct Cyc_Absyn_Tunionfield* _temp2631; struct Cyc_Absyn_Tuniondecl*
_temp2633; _LL2627: if(*(( int*) _temp2625) == Cyc_Tcenv_TunionRes){ _LL2634:
_temp2633=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2625)->f1; goto _LL2632;
_LL2632: _temp2631=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2625)->f2; goto
_LL2628;} else{ goto _LL2629;} _LL2629: goto _LL2630; _LL2628: tuf= _temp2631;
tud= _temp2633; if( tud->is_xtunion != _temp2596){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp2635)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp2639= _tag_arr("[x]tunion is different from type declaration %s",
sizeof( unsigned char), 48u); struct Cyc_Stdio_String_pa_struct _temp2640;
_temp2640.tag= Cyc_Stdio_String_pa; _temp2640.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2600);{ void* _temp2638=( void*)& _temp2640; void* _temp2636[ 1u]={
_temp2638}; struct _tagged_arr _temp2637={( void*) _temp2636,( void*) _temp2636,(
void*)( _temp2636 + 1u)}; _temp2635( _temp2639, _temp2637);}}));} goto _LL2626;
_LL2630: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp2641)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp2646= _tag_arr("unbound field %s in type tunion %s",
sizeof( unsigned char), 35u); struct Cyc_Stdio_String_pa_struct _temp2648;
_temp2648.tag= Cyc_Stdio_String_pa; _temp2648.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2598);{ void* _temp2644=( void*)& _temp2648; struct Cyc_Stdio_String_pa_struct
_temp2647; _temp2647.tag= Cyc_Stdio_String_pa; _temp2647.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2600);{ void* _temp2645=( void*)& _temp2647; void*
_temp2642[ 2u]={ _temp2644, _temp2645}; struct _tagged_arr _temp2643={( void*)
_temp2642,( void*) _temp2642,( void*)( _temp2642 + 2u)}; _temp2641( _temp2646,
_temp2643);}}}));{ struct Cyc_Tcutil_CVTEnv _temp2649= cvtenv; _npop_handler( 0u);
return _temp2649;} _LL2626:;}; _pop_handler();} else{ void* _temp2623=( void*)
_exn_thrown; void* _temp2651= _temp2623; _LL2653: if( _temp2651 == Cyc_Dict_Absent){
goto _LL2654;} else{ goto _LL2655;} _LL2655: goto _LL2656; _LL2654: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp2657)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp2662=
_tag_arr("unbound field %s in type tunion %s", sizeof( unsigned char), 35u);
struct Cyc_Stdio_String_pa_struct _temp2664; _temp2664.tag= Cyc_Stdio_String_pa;
_temp2664.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2598);{ void*
_temp2660=( void*)& _temp2664; struct Cyc_Stdio_String_pa_struct _temp2663;
_temp2663.tag= Cyc_Stdio_String_pa; _temp2663.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2600);{ void* _temp2661=( void*)& _temp2663; void* _temp2658[ 2u]={
_temp2660, _temp2661}; struct _tagged_arr _temp2659={( void*) _temp2658,( void*)
_temp2658,( void*)( _temp2658 + 2u)}; _temp2657( _temp2662, _temp2659);}}}));
return cvtenv; _LL2656:( void) _throw( _temp2651); _LL2652:;}}}* _temp2447=(
void*)({ struct Cyc_Absyn_KnownTunionfield_struct* _temp2665=( struct Cyc_Absyn_KnownTunionfield_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunionfield_struct)); _temp2665[ 0]=({
struct Cyc_Absyn_KnownTunionfield_struct _temp2666; _temp2666.tag= Cyc_Absyn_KnownTunionfield;
_temp2666.f1= tud; _temp2666.f2= tuf; _temp2666;}); _temp2665;}); _temp2604= tud;
_temp2602= tuf; goto _LL2593;} _LL2593: { struct Cyc_List_List* tvs= _temp2604->tvs;
for( 0; _temp2443 != 0? tvs != 0: 0;( _temp2443=(( struct Cyc_List_List*)
_check_null( _temp2443))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){
void* t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2443))->hd; void*
k1= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te,
cvtenv, k1, t1);} if( _temp2443 != 0){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr(* _temp2667)( struct _tagged_arr fmt, struct _tagged_arr)=
Cyc_Stdio_aprintf; struct _tagged_arr _temp2672= _tag_arr("too many type arguments for tunion %s.%s",
sizeof( unsigned char), 41u); struct Cyc_Stdio_String_pa_struct _temp2674;
_temp2674.tag= Cyc_Stdio_String_pa; _temp2674.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2604->name);{ void* _temp2670=( void*)& _temp2674; struct Cyc_Stdio_String_pa_struct
_temp2673; _temp2673.tag= Cyc_Stdio_String_pa; _temp2673.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2602->name);{ void* _temp2671=( void*)& _temp2673;
void* _temp2668[ 2u]={ _temp2670, _temp2671}; struct _tagged_arr _temp2669={(
void*) _temp2668,( void*) _temp2668,( void*)( _temp2668 + 2u)}; _temp2667(
_temp2672, _temp2669);}}}));} if( tvs != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp2675)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp2680= _tag_arr("too few type arguments for tunion %s.%s",
sizeof( unsigned char), 40u); struct Cyc_Stdio_String_pa_struct _temp2682;
_temp2682.tag= Cyc_Stdio_String_pa; _temp2682.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2604->name);{ void* _temp2678=( void*)& _temp2682; struct Cyc_Stdio_String_pa_struct
_temp2681; _temp2681.tag= Cyc_Stdio_String_pa; _temp2681.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2602->name);{ void* _temp2679=( void*)& _temp2681;
void* _temp2676[ 2u]={ _temp2678, _temp2679}; struct _tagged_arr _temp2677={(
void*) _temp2676,( void*) _temp2676,( void*)( _temp2676 + 2u)}; _temp2675(
_temp2680, _temp2677);}}}));} goto _LL2589;} _LL2589:;} goto _LL2377; _LL2391:
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_AnyKind,
_temp2458); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2456);{ void* _temp2683=( void*)( Cyc_Absyn_compress_conref( _temp2450))->v;
void* _temp2689; struct Cyc_Absyn_Exp* _temp2691; _LL2685: if(( unsigned int)
_temp2683 > 1u?*(( int*) _temp2683) == Cyc_Absyn_Eq_constr: 0){ _LL2690:
_temp2689=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2683)->f1; if((
unsigned int) _temp2689 > 1u?*(( int*) _temp2689) == Cyc_Absyn_Upper_b: 0){
_LL2692: _temp2691=(( struct Cyc_Absyn_Upper_b_struct*) _temp2689)->f1; goto
_LL2686;} else{ goto _LL2687;}} else{ goto _LL2687;} _LL2687: goto _LL2688;
_LL2686: if( ! Cyc_Tcutil_is_const_exp( te, _temp2691)){ Cyc_Tcutil_terr( loc,
_tag_arr("pointer bounds expression is not a constant", sizeof( unsigned char),
44u));} Cyc_Tcexp_tcExp( te, 0, _temp2691); if( ! Cyc_Tcutil_coerce_uint_typ( te,
_temp2691)){ Cyc_Tcutil_terr( loc, _tag_arr("pointer bounds expression is not an unsigned int",
sizeof( unsigned char), 49u));} Cyc_Evexp_eval_const_uint_exp( _temp2691); goto
_LL2684; _LL2688: goto _LL2684; _LL2684:;} goto _LL2377; _LL2393: goto _LL2377;
_LL2395: goto _LL2377; _LL2397: goto _LL2377; _LL2399: cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp2464); if( _temp2460 == 0? 1: !
Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*) _check_null( _temp2460))){
Cyc_Tcutil_terr( loc, _tag_arr("array bounds expression is not constant",
sizeof( unsigned char), 40u));} Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)
_check_null( _temp2460)); if( ! Cyc_Tcutil_coerce_uint_typ( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp2460))){ Cyc_Tcutil_terr( loc, _tag_arr("array bounds expression is not an unsigned int",
sizeof( unsigned char), 47u));} Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp2460)); goto _LL2377; _LL2401: { int num_convs= 0; int
seen_cdecl= 0; int seen_stdcall= 0; int seen_fastcall= 0; for( 0; _temp2468 != 0;
_temp2468=(( struct Cyc_List_List*) _check_null( _temp2468))->tl){ if( ! Cyc_Absyn_fntype_att((
void*)(( struct Cyc_List_List*) _check_null( _temp2468))->hd)){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp2693)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp2697=
_tag_arr("bad function type attribute %s", sizeof( unsigned char), 31u); struct
Cyc_Stdio_String_pa_struct _temp2698; _temp2698.tag= Cyc_Stdio_String_pa;
_temp2698.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*)(( struct
Cyc_List_List*) _check_null( _temp2468))->hd);{ void* _temp2696=( void*)&
_temp2698; void* _temp2694[ 1u]={ _temp2696}; struct _tagged_arr _temp2695={(
void*) _temp2694,( void*) _temp2694,( void*)( _temp2694 + 1u)}; _temp2693(
_temp2697, _temp2695);}}));}{ void* _temp2699=( void*)(( struct Cyc_List_List*)
_check_null( _temp2468))->hd; _LL2701: if( _temp2699 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL2702;} else{ goto _LL2703;} _LL2703: if( _temp2699 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL2704;} else{ goto _LL2705;} _LL2705: if( _temp2699 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL2706;} else{ goto _LL2707;} _LL2707: goto _LL2708; _LL2702: if( !
seen_stdcall){ seen_stdcall= 1; ++ num_convs;} goto _LL2700; _LL2704: if( !
seen_cdecl){ seen_cdecl= 1; ++ num_convs;} goto _LL2700; _LL2706: if( !
seen_fastcall){ seen_fastcall= 1; ++ num_convs;} goto _LL2700; _LL2708: goto
_LL2700; _LL2700:;}} if( num_convs > 1){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp2709)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp2712= _tag_arr("function can't have multiple calling conventions",
sizeof( unsigned char), 49u); void* _temp2710[ 0u]={}; struct _tagged_arr
_temp2711={( void*) _temp2710,( void*) _temp2710,( void*)( _temp2710 + 0u)};
_temp2709( _temp2712, _temp2711);}));} Cyc_Tcutil_check_unique_tvars( loc,*
_temp2485);{ struct Cyc_List_List* b=* _temp2485; for( 0; b != 0; b=(( struct
Cyc_List_List*) _check_null( b))->tl){(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( b))->hd)->identity= Cyc_Tcutil_new_tvar_id(); cvtenv.kind_env= Cyc_Tcutil_add_bound_tvar(
cvtenv.kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
b))->hd);{ void* _temp2713=( void*)( Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( b))->hd)->kind))->v; void* _temp2719;
_LL2715: if(( unsigned int) _temp2713 > 1u?*(( int*) _temp2713) == Cyc_Absyn_Eq_constr:
0){ _LL2720: _temp2719=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2713)->f1;
if( _temp2719 ==( void*) Cyc_Absyn_MemKind){ goto _LL2716;} else{ goto _LL2717;}}
else{ goto _LL2717;} _LL2717: goto _LL2718; _LL2716: Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp2721)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp2725= _tag_arr("function attempts to abstract Mem type variable %s",
sizeof( unsigned char), 51u); struct Cyc_Stdio_String_pa_struct _temp2726;
_temp2726.tag= Cyc_Stdio_String_pa; _temp2726.f1=( struct _tagged_arr)*(( struct
Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd)->name;{ void*
_temp2724=( void*)& _temp2726; void* _temp2722[ 1u]={ _temp2724}; struct
_tagged_arr _temp2723={( void*) _temp2722,( void*) _temp2722,( void*)( _temp2722
+ 1u)}; _temp2721( _temp2725, _temp2723);}})); goto _LL2714; _LL2718: goto
_LL2714; _LL2714:;}}}{ struct Cyc_Tcutil_CVTEnv _temp2727=({ struct Cyc_Tcutil_CVTEnv
_temp2835; _temp2835.kind_env= cvtenv.kind_env; _temp2835.free_vars= 0;
_temp2835.free_evars= 0; _temp2835;}); _temp2727= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2727,( void*) Cyc_Absyn_MemKind, _temp2478); for( 0; _temp2476 !=
0; _temp2476=(( struct Cyc_List_List*) _check_null( _temp2476))->tl){ _temp2727=
Cyc_Tcutil_i_check_valid_type( loc, te, _temp2727,( void*) Cyc_Absyn_MemKind,(*((
struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp2476))->hd)).f3);}
if( _temp2472 != 0){ if( _temp2474){(( int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)(
_tag_arr("both c_vararg and cyc_vararg", sizeof( unsigned char), 29u));}{ int
_temp2730; void* _temp2732; void* _temp2734; struct Cyc_Absyn_Tqual _temp2736;
struct Cyc_Core_Opt* _temp2738; struct Cyc_Absyn_VarargInfo _temp2728=*(( struct
Cyc_Absyn_VarargInfo*) _check_null( _temp2472)); _LL2739: _temp2738= _temp2728.name;
goto _LL2737; _LL2737: _temp2736= _temp2728.tq; goto _LL2735; _LL2735: _temp2734=(
void*) _temp2728.type; goto _LL2733; _LL2733: _temp2732=( void*) _temp2728.rgn;
goto _LL2731; _LL2731: _temp2730= _temp2728.inject; goto _LL2729; _LL2729:
_temp2727= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2727,( void*) Cyc_Absyn_MemKind,
_temp2734);{ void* _temp2740= Cyc_Tcutil_compress( _temp2732); _LL2742: if((
unsigned int) _temp2740 > 4u?*(( int*) _temp2740) == Cyc_Absyn_Evar: 0){ goto
_LL2743;} else{ goto _LL2744;} _LL2744: if(( unsigned int) _temp2740 > 4u?*((
int*) _temp2740) == Cyc_Absyn_VarType: 0){ goto _LL2745;} else{ goto _LL2746;}
_LL2746: if( _temp2740 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL2747;} else{ goto
_LL2748;} _LL2748: goto _LL2749; _LL2743: { void* vr_tvar=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2750=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2750[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2751; _temp2751.tag= Cyc_Absyn_VarType;
_temp2751.f1= Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_RgnKind); _temp2751;});
_temp2750;}); Cyc_Tcutil_unify( _temp2732, vr_tvar); goto _LL2741;} _LL2745:
goto _LL2741; _LL2747: Cyc_Tcutil_terr( loc, _tag_arr("varargs should be in a region, not heap-allocated",
sizeof( unsigned char), 50u)); goto _LL2741; _LL2749: goto _LL2741; _LL2741:;}
_temp2727= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2727,( void*) Cyc_Absyn_RgnKind,
_temp2732); if( _temp2730){ void* _temp2752= Cyc_Tcutil_compress( _temp2734);
struct Cyc_Absyn_TunionInfo _temp2758; void* _temp2760; void* _temp2762; _LL2754:
if(( unsigned int) _temp2752 > 4u?*(( int*) _temp2752) == Cyc_Absyn_TunionType:
0){ _LL2759: _temp2758=(( struct Cyc_Absyn_TunionType_struct*) _temp2752)->f1;
_LL2763: _temp2762=( void*) _temp2758.tunion_info; if(*(( int*) _temp2762) ==
Cyc_Absyn_KnownTunion){ goto _LL2761;} else{ goto _LL2756;} _LL2761: _temp2760=(
void*) _temp2758.rgn; goto _LL2755;} else{ goto _LL2756;} _LL2756: goto _LL2757;
_LL2755:{ void* _temp2764= Cyc_Tcutil_compress( _temp2760); _LL2766: if((
unsigned int) _temp2764 > 4u?*(( int*) _temp2764) == Cyc_Absyn_Evar: 0){ goto
_LL2767;} else{ goto _LL2768;} _LL2768: if(( unsigned int) _temp2764 > 4u?*((
int*) _temp2764) == Cyc_Absyn_VarType: 0){ goto _LL2769;} else{ goto _LL2770;}
_LL2770: if( _temp2764 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL2771;} else{ goto
_LL2772;} _LL2772: goto _LL2773; _LL2767: Cyc_Tcutil_unify( _temp2732, _temp2760);
goto _LL2765; _LL2769: goto _LL2765; _LL2771: Cyc_Tcutil_terr( loc, _tag_arr("injected vararg [x]tunion should be in a region, not heap-allocated",
sizeof( unsigned char), 68u)); goto _LL2765; _LL2773: goto _LL2765; _LL2765:;}
goto _LL2753; _LL2757: Cyc_Tcutil_terr( loc, _tag_arr("can't inject a non-[x]tunion type",
sizeof( unsigned char), 34u)); goto _LL2753; _LL2753:;}}}{ struct Cyc_List_List*
rpo= _temp2470; for( 0; rpo != 0; rpo=(( struct Cyc_List_List*) _check_null( rpo))->tl){
struct _tuple8 _temp2776; void* _temp2777; void* _temp2779; struct _tuple8*
_temp2774=( struct _tuple8*)(( struct Cyc_List_List*) _check_null( rpo))->hd;
_temp2776=* _temp2774; _LL2780: _temp2779= _temp2776.f1; goto _LL2778; _LL2778:
_temp2777= _temp2776.f2; goto _LL2775; _LL2775: _temp2727= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2727,( void*) Cyc_Absyn_RgnKind, _temp2779); _temp2727= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2727,( void*) Cyc_Absyn_RgnKind, _temp2777);}} if(* _temp2482 != 0){
_temp2727= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2727,( void*) Cyc_Absyn_EffKind,(
void*)(( struct Cyc_Core_Opt*) _check_null(* _temp2482))->v);} else{ struct Cyc_Absyn_Tvar*
e= Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_EffKind); _temp2727.free_vars= Cyc_Tcutil_add_free_tvar(
loc, _temp2727.free_vars, e);{ struct Cyc_List_List* effect= 0;{ struct Cyc_List_List*
tvs= _temp2727.free_vars; for( 0; tvs != 0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){ void* _temp2781=( void*)( Cyc_Absyn_compress_conref(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind))->v;
void* _temp2789; void* _temp2791; _LL2783: if(( unsigned int) _temp2781 > 1u?*((
int*) _temp2781) == Cyc_Absyn_Eq_constr: 0){ _LL2790: _temp2789=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2781)->f1; if( _temp2789 ==( void*) Cyc_Absyn_RgnKind){
goto _LL2784;} else{ goto _LL2785;}} else{ goto _LL2785;} _LL2785: if((
unsigned int) _temp2781 > 1u?*(( int*) _temp2781) == Cyc_Absyn_Eq_constr: 0){
_LL2792: _temp2791=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2781)->f1;
if( _temp2791 ==( void*) Cyc_Absyn_EffKind){ goto _LL2786;} else{ goto _LL2787;}}
else{ goto _LL2787;} _LL2787: goto _LL2788; _LL2784: effect=({ struct Cyc_List_List*
_temp2793=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2793->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2794=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2794[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2795; _temp2795.tag= Cyc_Absyn_AccessEff;
_temp2795.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp2796=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2796[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2797; _temp2797.tag= Cyc_Absyn_VarType;
_temp2797.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp2797;}); _temp2796;})); _temp2795;}); _temp2794;})); _temp2793->tl= effect;
_temp2793;}); goto _LL2782; _LL2786: effect=({ struct Cyc_List_List* _temp2798=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2798->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp2799=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2799[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2800; _temp2800.tag= Cyc_Absyn_VarType; _temp2800.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp2800;}); _temp2799;})); _temp2798->tl= effect; _temp2798;}); goto _LL2782;
_LL2788: goto _LL2782; _LL2782:;}}{ struct Cyc_List_List* ts= _temp2727.free_evars;
for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){ void*
_temp2801= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);
_LL2803: if( _temp2801 ==( void*) Cyc_Absyn_RgnKind){ goto _LL2804;} else{ goto
_LL2805;} _LL2805: if( _temp2801 ==( void*) Cyc_Absyn_EffKind){ goto _LL2806;}
else{ goto _LL2807;} _LL2807: goto _LL2808; _LL2804: effect=({ struct Cyc_List_List*
_temp2809=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2809->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2810=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2810[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2811; _temp2811.tag= Cyc_Absyn_AccessEff;
_temp2811.f1=( void*)(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);
_temp2811;}); _temp2810;})); _temp2809->tl= effect; _temp2809;}); goto _LL2802;
_LL2806: effect=({ struct Cyc_List_List* _temp2812=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2812->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( ts))->hd); _temp2812->tl= effect; _temp2812;});
goto _LL2802; _LL2808: goto _LL2802; _LL2802:;}}* _temp2482=({ struct Cyc_Core_Opt*
_temp2813=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2813->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2814=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2814[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2815; _temp2815.tag= Cyc_Absyn_JoinEff;
_temp2815.f1= Cyc_List_imp_rev( effect); _temp2815;}); _temp2814;})); _temp2813;});}}
if(* _temp2485 != 0){ struct Cyc_List_List* bs=* _temp2485; for( 0; bs != 0; bs=((
struct Cyc_List_List*) _check_null( bs))->tl){ struct Cyc_Absyn_Conref*
_temp2816= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( bs))->hd)->kind); void* _temp2817=( void*) _temp2816->v; void*
_temp2825; _LL2819: if( _temp2817 ==( void*) Cyc_Absyn_No_constr){ goto _LL2820;}
else{ goto _LL2821;} _LL2821: if(( unsigned int) _temp2817 > 1u?*(( int*)
_temp2817) == Cyc_Absyn_Eq_constr: 0){ _LL2826: _temp2825=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2817)->f1; if( _temp2825 ==( void*) Cyc_Absyn_MemKind){ goto _LL2822;}
else{ goto _LL2823;}} else{ goto _LL2823;} _LL2823: goto _LL2824; _LL2820: Cyc_Tcutil_warn(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp2827)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp2831=
_tag_arr("Type variable %s unconstrained, assuming boxed", sizeof( unsigned char),
47u); struct Cyc_Stdio_String_pa_struct _temp2832; _temp2832.tag= Cyc_Stdio_String_pa;
_temp2832.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( bs))->hd)->name;{ void* _temp2830=( void*)& _temp2832; void*
_temp2828[ 1u]={ _temp2830}; struct _tagged_arr _temp2829={( void*) _temp2828,(
void*) _temp2828,( void*)( _temp2828 + 1u)}; _temp2827( _temp2831, _temp2829);}}));
goto _LL2822; _LL2822:( void*)( _temp2816->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2833=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2833[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2834; _temp2834.tag= Cyc_Absyn_Eq_constr;
_temp2834.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp2834;}); _temp2833;})));
goto _LL2818; _LL2824: goto _LL2818; _LL2818:;}} cvtenv.kind_env= Cyc_Tcutil_remove_bound_tvars(
_temp2727.kind_env,* _temp2485); _temp2727.free_vars= Cyc_Tcutil_remove_bound_tvars(
_temp2727.free_vars,* _temp2485);{ struct Cyc_List_List* tvs= _temp2727.free_vars;
for( 0; tvs != 0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd);}}{ struct Cyc_List_List* evs=
_temp2727.free_evars; for( 0; evs != 0; evs=(( struct Cyc_List_List*)
_check_null( evs))->tl){ cvtenv.free_evars= Cyc_Tcutil_add_free_evar( cvtenv.free_evars,(
void*)(( struct Cyc_List_List*) _check_null( evs))->hd);}} goto _LL2377;}}
_LL2403: for( 0; _temp2486 != 0; _temp2486=(( struct Cyc_List_List*) _check_null(
_temp2486))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*)
Cyc_Absyn_MemKind,(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp2486))->hd)).f2);} goto _LL2377; _LL2405:{ struct _RegionHandle _temp2836=
_new_region(); struct _RegionHandle* sprev_rgn=& _temp2836; _push_region(
sprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for( 0; _temp2488 != 0;
_temp2488=(( struct Cyc_List_List*) _check_null( _temp2488))->tl){ struct Cyc_Absyn_Structfield
_temp2839; struct Cyc_List_List* _temp2840; struct Cyc_Core_Opt* _temp2842; void*
_temp2844; struct Cyc_Absyn_Tqual _temp2846; struct _tagged_arr* _temp2848;
struct Cyc_Absyn_Structfield* _temp2837=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2488))->hd; _temp2839=* _temp2837;
_LL2849: _temp2848= _temp2839.name; goto _LL2847; _LL2847: _temp2846= _temp2839.tq;
goto _LL2845; _LL2845: _temp2844=( void*) _temp2839.type; goto _LL2843; _LL2843:
_temp2842= _temp2839.width; goto _LL2841; _LL2841: _temp2840= _temp2839.attributes;
goto _LL2838; _LL2838: if((( int(*)( int(* compare)( struct _tagged_arr*, struct
_tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields, _temp2848)){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp2850)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp2854= _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u); struct Cyc_Stdio_String_pa_struct _temp2855;
_temp2855.tag= Cyc_Stdio_String_pa; _temp2855.f1=( struct _tagged_arr)*
_temp2848;{ void* _temp2853=( void*)& _temp2855; void* _temp2851[ 1u]={
_temp2853}; struct _tagged_arr _temp2852={( void*) _temp2851,( void*) _temp2851,(
void*)( _temp2851 + 1u)}; _temp2850( _temp2854, _temp2852);}}));} if( Cyc_String_strcmp(*
_temp2848, _tag_arr("", sizeof( unsigned char), 1u)) != 0){ prev_fields=({
struct Cyc_List_List* _temp2856=( struct Cyc_List_List*) _region_malloc(
sprev_rgn, sizeof( struct Cyc_List_List)); _temp2856->hd=( void*) _temp2848;
_temp2856->tl= prev_fields; _temp2856;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp2844); if( _temp2842 != 0){
unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp2842))->v)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp2857)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp2861= _tag_arr("bitfield %s does not have constant width",
sizeof( unsigned char), 41u); struct Cyc_Stdio_String_pa_struct _temp2862;
_temp2862.tag= Cyc_Stdio_String_pa; _temp2862.f1=( struct _tagged_arr)*
_temp2848;{ void* _temp2860=( void*)& _temp2862; void* _temp2858[ 1u]={
_temp2860}; struct _tagged_arr _temp2859={( void*) _temp2858,( void*) _temp2858,(
void*)( _temp2858 + 1u)}; _temp2857( _temp2861, _temp2859);}}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp2842))->v);}{
void* _temp2863= Cyc_Tcutil_compress( _temp2844); void* _temp2869; _LL2865: if((
unsigned int) _temp2863 > 4u?*(( int*) _temp2863) == Cyc_Absyn_IntType: 0){
_LL2870: _temp2869=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2863)->f2;
goto _LL2866;} else{ goto _LL2867;} _LL2867: goto _LL2868; _LL2866:{ void*
_temp2871= _temp2869; _LL2873: if( _temp2871 ==( void*) Cyc_Absyn_B1){ goto
_LL2874;} else{ goto _LL2875;} _LL2875: if( _temp2871 ==( void*) Cyc_Absyn_B2){
goto _LL2876;} else{ goto _LL2877;} _LL2877: if( _temp2871 ==( void*) Cyc_Absyn_B4){
goto _LL2878;} else{ goto _LL2879;} _LL2879: if( _temp2871 ==( void*) Cyc_Absyn_B8){
goto _LL2880;} else{ goto _LL2872;} _LL2874: if( w > 8){ Cyc_Tcutil_terr( loc,
_tag_arr("bitfield larger than type", sizeof( unsigned char), 26u));} goto
_LL2872; _LL2876: if( w > 16){ Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u));} goto _LL2872; _LL2878: if( w > 32){ Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u));} goto
_LL2872; _LL2880: if( w > 64){ Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u));} goto _LL2872; _LL2872:;} goto _LL2864; _LL2868:
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp2881)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp2886= _tag_arr("bitfield %s must have integral type but has type %s",
sizeof( unsigned char), 52u); struct Cyc_Stdio_String_pa_struct _temp2888;
_temp2888.tag= Cyc_Stdio_String_pa; _temp2888.f1=( struct _tagged_arr)*
_temp2848;{ void* _temp2884=( void*)& _temp2888; struct Cyc_Stdio_String_pa_struct
_temp2887; _temp2887.tag= Cyc_Stdio_String_pa; _temp2887.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( _temp2844);{ void* _temp2885=( void*)& _temp2887; void*
_temp2882[ 2u]={ _temp2884, _temp2885}; struct _tagged_arr _temp2883={( void*)
_temp2882,( void*) _temp2882,( void*)( _temp2882 + 2u)}; _temp2881( _temp2886,
_temp2883);}}})); goto _LL2864; _LL2864:;}} for( 0; _temp2840 != 0; _temp2840=((
struct Cyc_List_List*) _check_null( _temp2840))->tl){ void* _temp2889=( void*)((
struct Cyc_List_List*) _check_null( _temp2840))->hd; _LL2891: if( _temp2889 ==(
void*) Cyc_Absyn_Packed_att){ goto _LL2892;} else{ goto _LL2893;} _LL2893: if((
unsigned int) _temp2889 > 16u?*(( int*) _temp2889) == Cyc_Absyn_Aligned_att: 0){
goto _LL2894;} else{ goto _LL2895;} _LL2895: goto _LL2896; _LL2892: continue;
_LL2894: continue; _LL2896: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp2897)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp2902= _tag_arr("bad attribute %s on struct member %s",
sizeof( unsigned char), 37u); struct Cyc_Stdio_String_pa_struct _temp2904;
_temp2904.tag= Cyc_Stdio_String_pa; _temp2904.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp2840))->hd);{ void* _temp2900=(
void*)& _temp2904; struct Cyc_Stdio_String_pa_struct _temp2903; _temp2903.tag=
Cyc_Stdio_String_pa; _temp2903.f1=( struct _tagged_arr)* _temp2848;{ void*
_temp2901=( void*)& _temp2903; void* _temp2898[ 2u]={ _temp2900, _temp2901};
struct _tagged_arr _temp2899={( void*) _temp2898,( void*) _temp2898,( void*)(
_temp2898 + 2u)}; _temp2897( _temp2902, _temp2899);}}})); goto _LL2890; _LL2890:;}}};
_pop_region( sprev_rgn);} goto _LL2377; _LL2407:{ struct _RegionHandle _temp2905=
_new_region(); struct _RegionHandle* uprev_rgn=& _temp2905; _push_region(
uprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for( 0; _temp2490 != 0;
_temp2490=(( struct Cyc_List_List*) _check_null( _temp2490))->tl){ struct Cyc_Absyn_Structfield
_temp2908; struct Cyc_List_List* _temp2909; struct Cyc_Core_Opt* _temp2911; void*
_temp2913; struct Cyc_Absyn_Tqual _temp2915; struct _tagged_arr* _temp2917;
struct Cyc_Absyn_Structfield* _temp2906=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2490))->hd; _temp2908=* _temp2906;
_LL2918: _temp2917= _temp2908.name; goto _LL2916; _LL2916: _temp2915= _temp2908.tq;
goto _LL2914; _LL2914: _temp2913=( void*) _temp2908.type; goto _LL2912; _LL2912:
_temp2911= _temp2908.width; goto _LL2910; _LL2910: _temp2909= _temp2908.attributes;
goto _LL2907; _LL2907: if((( int(*)( int(* compare)( struct _tagged_arr*, struct
_tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields, _temp2917)){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp2919)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp2923= _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u); struct Cyc_Stdio_String_pa_struct _temp2924;
_temp2924.tag= Cyc_Stdio_String_pa; _temp2924.f1=( struct _tagged_arr)*
_temp2917;{ void* _temp2922=( void*)& _temp2924; void* _temp2920[ 1u]={
_temp2922}; struct _tagged_arr _temp2921={( void*) _temp2920,( void*) _temp2920,(
void*)( _temp2920 + 1u)}; _temp2919( _temp2923, _temp2921);}}));} if( Cyc_String_strcmp(*
_temp2917, _tag_arr("", sizeof( unsigned char), 1u)) != 0){ prev_fields=({
struct Cyc_List_List* _temp2925=( struct Cyc_List_List*) _region_malloc(
uprev_rgn, sizeof( struct Cyc_List_List)); _temp2925->hd=( void*) _temp2917;
_temp2925->tl= prev_fields; _temp2925;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp2913); if( ! Cyc_Tcutil_bits_only(
_temp2913)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp2926)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp2930= _tag_arr("%s has a type that is possibly incompatible with other members of union",
sizeof( unsigned char), 72u); struct Cyc_Stdio_String_pa_struct _temp2931;
_temp2931.tag= Cyc_Stdio_String_pa; _temp2931.f1=( struct _tagged_arr)*
_temp2917;{ void* _temp2929=( void*)& _temp2931; void* _temp2927[ 1u]={
_temp2929}; struct _tagged_arr _temp2928={( void*) _temp2927,( void*) _temp2927,(
void*)( _temp2927 + 1u)}; _temp2926( _temp2930, _temp2928);}}));} if( _temp2911
!= 0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp2911))->v)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp2932)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp2936= _tag_arr("bitfield %s does not have constant width",
sizeof( unsigned char), 41u); struct Cyc_Stdio_String_pa_struct _temp2937;
_temp2937.tag= Cyc_Stdio_String_pa; _temp2937.f1=( struct _tagged_arr)*
_temp2917;{ void* _temp2935=( void*)& _temp2937; void* _temp2933[ 1u]={
_temp2935}; struct _tagged_arr _temp2934={( void*) _temp2933,( void*) _temp2933,(
void*)( _temp2933 + 1u)}; _temp2932( _temp2936, _temp2934);}}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp2911))->v);}{
void* _temp2938= Cyc_Tcutil_compress( _temp2913); void* _temp2944; _LL2940: if((
unsigned int) _temp2938 > 4u?*(( int*) _temp2938) == Cyc_Absyn_IntType: 0){
_LL2945: _temp2944=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2938)->f2;
goto _LL2941;} else{ goto _LL2942;} _LL2942: goto _LL2943; _LL2941:{ void*
_temp2946= _temp2944; _LL2948: if( _temp2946 ==( void*) Cyc_Absyn_B1){ goto
_LL2949;} else{ goto _LL2950;} _LL2950: if( _temp2946 ==( void*) Cyc_Absyn_B2){
goto _LL2951;} else{ goto _LL2952;} _LL2952: if( _temp2946 ==( void*) Cyc_Absyn_B4){
goto _LL2953;} else{ goto _LL2954;} _LL2954: if( _temp2946 ==( void*) Cyc_Absyn_B8){
goto _LL2955;} else{ goto _LL2947;} _LL2949: if( w > 8){ Cyc_Tcutil_terr( loc,
_tag_arr("bitfield larger than type", sizeof( unsigned char), 26u));} goto
_LL2947; _LL2951: if( w > 16){ Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u));} goto _LL2947; _LL2953: if( w > 32){ Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u));} goto
_LL2947; _LL2955: if( w > 64){ Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u));} goto _LL2947; _LL2947:;} goto _LL2939; _LL2943:
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp2956)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp2961= _tag_arr("bitfield %s must have integral type but has type %s",
sizeof( unsigned char), 52u); struct Cyc_Stdio_String_pa_struct _temp2963;
_temp2963.tag= Cyc_Stdio_String_pa; _temp2963.f1=( struct _tagged_arr)*
_temp2917;{ void* _temp2959=( void*)& _temp2963; struct Cyc_Stdio_String_pa_struct
_temp2962; _temp2962.tag= Cyc_Stdio_String_pa; _temp2962.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( _temp2913);{ void* _temp2960=( void*)& _temp2962; void*
_temp2957[ 2u]={ _temp2959, _temp2960}; struct _tagged_arr _temp2958={( void*)
_temp2957,( void*) _temp2957,( void*)( _temp2957 + 2u)}; _temp2956( _temp2961,
_temp2958);}}})); goto _LL2939; _LL2939:;}} for( 0; _temp2909 != 0; _temp2909=((
struct Cyc_List_List*) _check_null( _temp2909))->tl){ void* _temp2964=( void*)((
struct Cyc_List_List*) _check_null( _temp2909))->hd; _LL2966: if( _temp2964 ==(
void*) Cyc_Absyn_Packed_att){ goto _LL2967;} else{ goto _LL2968;} _LL2968: if((
unsigned int) _temp2964 > 16u?*(( int*) _temp2964) == Cyc_Absyn_Aligned_att: 0){
goto _LL2969;} else{ goto _LL2970;} _LL2970: goto _LL2971; _LL2967: continue;
_LL2969: continue; _LL2971: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp2972)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp2977= _tag_arr("bad attribute %s on struct member %s",
sizeof( unsigned char), 37u); struct Cyc_Stdio_String_pa_struct _temp2979;
_temp2979.tag= Cyc_Stdio_String_pa; _temp2979.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp2909))->hd);{ void* _temp2975=(
void*)& _temp2979; struct Cyc_Stdio_String_pa_struct _temp2978; _temp2978.tag=
Cyc_Stdio_String_pa; _temp2978.f1=( struct _tagged_arr)* _temp2917;{ void*
_temp2976=( void*)& _temp2978; void* _temp2973[ 2u]={ _temp2975, _temp2976};
struct _tagged_arr _temp2974={( void*) _temp2973,( void*) _temp2973,( void*)(
_temp2973 + 2u)}; _temp2972( _temp2977, _temp2974);}}})); goto _LL2965; _LL2965:;}}};
_pop_region( uprev_rgn);} goto _LL2377; _LL2409: if( _temp2497 == 0){ if(*
_temp2494 == 0){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp2980)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp2983= _tag_arr("bad anonymous struct", sizeof(
unsigned char), 21u); void* _temp2981[ 0u]={}; struct _tagged_arr _temp2982={(
void*) _temp2981,( void*) _temp2981,( void*)( _temp2981 + 0u)}; _temp2980(
_temp2983, _temp2982);})); return cvtenv;}{ struct Cyc_Absyn_Structdecl* sd=*((
struct Cyc_Absyn_Structdecl**) _check_null(* _temp2494));;}} if(* _temp2494 == 0){
struct _handler_cons _temp2984; _push_handler(& _temp2984);{ int _temp2986= 0;
if( setjmp( _temp2984.handler)){ _temp2986= 1;} if( ! _temp2986){* _temp2494=(
struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl( te, loc,( struct
_tuple1*) _check_null( _temp2497));; _pop_handler();} else{ void* _temp2985=(
void*) _exn_thrown; void* _temp2988= _temp2985; _LL2990: if( _temp2988 == Cyc_Dict_Absent){
goto _LL2991;} else{ goto _LL2992;} _LL2992: goto _LL2993; _LL2991: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp2994)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp2998=
_tag_arr("unbound type struct %s", sizeof( unsigned char), 23u); struct Cyc_Stdio_String_pa_struct
_temp2999; _temp2999.tag= Cyc_Stdio_String_pa; _temp2999.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp2497));{ void*
_temp2997=( void*)& _temp2999; void* _temp2995[ 1u]={ _temp2997}; struct
_tagged_arr _temp2996={( void*) _temp2995,( void*) _temp2995,( void*)( _temp2995
+ 1u)}; _temp2994( _temp2998, _temp2996);}})); return cvtenv; _LL2993:( void)
_throw( _temp2988); _LL2989:;}}}{ struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**)
_check_null(* _temp2494)); if( sd->name != 0){*(( struct _tuple1*) _check_null(
_temp2497))=(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)[
_check_known_subscript_notnull( 1u, 0)];}{ struct Cyc_List_List* tvs= sd->tvs;
int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=
Cyc_List_length( _temp2495); if( lvs != largs){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp3000)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp3006= _tag_arr("struct %s expects %d type arguments but was given %d",
sizeof( unsigned char), 53u); struct Cyc_Stdio_String_pa_struct _temp3009;
_temp3009.tag= Cyc_Stdio_String_pa; _temp3009.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp2497));{ void* _temp3003=( void*)& _temp3009;
struct Cyc_Stdio_Int_pa_struct _temp3008; _temp3008.tag= Cyc_Stdio_Int_pa;
_temp3008.f1=( unsigned int) lvs;{ void* _temp3004=( void*)& _temp3008; struct
Cyc_Stdio_Int_pa_struct _temp3007; _temp3007.tag= Cyc_Stdio_Int_pa; _temp3007.f1=(
unsigned int) largs;{ void* _temp3005=( void*)& _temp3007; void* _temp3001[ 3u]={
_temp3003, _temp3004, _temp3005}; struct _tagged_arr _temp3002={( void*)
_temp3001,( void*) _temp3001,( void*)( _temp3001 + 3u)}; _temp3000( _temp3006,
_temp3002);}}}}));} for( 0; _temp2495 != 0;( _temp2495=(( struct Cyc_List_List*)
_check_null( _temp2495))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){
void* t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2495))->hd; void*
k1= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te,
cvtenv, k1, t1);} goto _LL2377;}} _LL2411: if( _temp2504 == 0){ Cyc_Tcutil_terr(
loc, _tag_arr("missing union name", sizeof( unsigned char), 19u)); return cvtenv;}
if(* _temp2501 == 0){ struct _handler_cons _temp3010; _push_handler(& _temp3010);{
int _temp3012= 0; if( setjmp( _temp3010.handler)){ _temp3012= 1;} if( !
_temp3012){* _temp2501=( struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl(
te, loc,( struct _tuple1*) _check_null( _temp2504));; _pop_handler();} else{
void* _temp3011=( void*) _exn_thrown; void* _temp3014= _temp3011; _LL3016: if(
_temp3014 == Cyc_Dict_Absent){ goto _LL3017;} else{ goto _LL3018;} _LL3018: goto
_LL3019; _LL3017: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp3020)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp3024= _tag_arr("unbound type union %s", sizeof(
unsigned char), 22u); struct Cyc_Stdio_String_pa_struct _temp3025; _temp3025.tag=
Cyc_Stdio_String_pa; _temp3025.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp2504));{ void* _temp3023=( void*)& _temp3025;
void* _temp3021[ 1u]={ _temp3023}; struct _tagged_arr _temp3022={( void*)
_temp3021,( void*) _temp3021,( void*)( _temp3021 + 1u)}; _temp3020( _temp3024,
_temp3022);}})); return cvtenv; _LL3019:( void) _throw( _temp3014); _LL3015:;}}}{
struct Cyc_Absyn_Uniondecl* ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null(*
_temp2501)); if( ud->name != 0){*(( struct _tuple1*) _check_null( _temp2504))=((
struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( ud->name))->v)[
_check_known_subscript_notnull( 1u, 0)];}{ struct Cyc_List_List* tvs= ud->tvs;
int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=
Cyc_List_length( _temp2502); if( lvs != largs){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp3026)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp3032= _tag_arr("union %s expects %d type arguments but was given %d",
sizeof( unsigned char), 52u); struct Cyc_Stdio_String_pa_struct _temp3035;
_temp3035.tag= Cyc_Stdio_String_pa; _temp3035.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp2504));{ void* _temp3029=( void*)& _temp3035;
struct Cyc_Stdio_Int_pa_struct _temp3034; _temp3034.tag= Cyc_Stdio_Int_pa;
_temp3034.f1=( unsigned int) lvs;{ void* _temp3030=( void*)& _temp3034; struct
Cyc_Stdio_Int_pa_struct _temp3033; _temp3033.tag= Cyc_Stdio_Int_pa; _temp3033.f1=(
unsigned int) largs;{ void* _temp3031=( void*)& _temp3033; void* _temp3027[ 3u]={
_temp3029, _temp3030, _temp3031}; struct _tagged_arr _temp3028={( void*)
_temp3027,( void*) _temp3027,( void*)( _temp3027 + 3u)}; _temp3026( _temp3032,
_temp3028);}}}}));} for( 0; _temp2502 != 0;( _temp2502=(( struct Cyc_List_List*)
_check_null( _temp2502))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){
void* t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2502))->hd; void*
k1= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te,
cvtenv, k1, t1);} goto _LL2377;}} _LL2413: { struct Cyc_Absyn_Typedefdecl* td;{
struct _handler_cons _temp3036; _push_handler(& _temp3036);{ int _temp3038= 0;
if( setjmp( _temp3036.handler)){ _temp3038= 1;} if( ! _temp3038){ td= Cyc_Tcenv_lookup_typedefdecl(
te, loc, _temp2511);; _pop_handler();} else{ void* _temp3037=( void*)
_exn_thrown; void* _temp3040= _temp3037; _LL3042: if( _temp3040 == Cyc_Dict_Absent){
goto _LL3043;} else{ goto _LL3044;} _LL3044: goto _LL3045; _LL3043: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp3046)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp3050=
_tag_arr("unbound typedef name %s", sizeof( unsigned char), 24u); struct Cyc_Stdio_String_pa_struct
_temp3051; _temp3051.tag= Cyc_Stdio_String_pa; _temp3051.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2511);{ void* _temp3049=( void*)& _temp3051; void*
_temp3047[ 1u]={ _temp3049}; struct _tagged_arr _temp3048={( void*) _temp3047,(
void*) _temp3047,( void*)( _temp3047 + 1u)}; _temp3046( _temp3050, _temp3048);}}));
return cvtenv; _LL3045:( void) _throw( _temp3040); _LL3041:;}}} _temp2511[
_check_known_subscript_notnull( 1u, 0)]=( td->name)[
_check_known_subscript_notnull( 1u, 0)];{ struct Cyc_List_List* tvs= td->tvs;
struct Cyc_List_List* ts= _temp2509; struct Cyc_List_List* inst= 0; for( 0; ts
!= 0? tvs != 0: 0;( ts=(( struct Cyc_List_List*) _check_null( ts))->tl, tvs=((
struct Cyc_List_List*) _check_null( tvs))->tl)){ void* k= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k,( void*)(( struct Cyc_List_List*)
_check_null( ts))->hd); inst=({ struct Cyc_List_List* _temp3052=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3052->hd=( void*)({ struct
_tuple5* _temp3053=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp3053->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
tvs))->hd; _temp3053->f2=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd;
_temp3053;}); _temp3052->tl= inst; _temp3052;});} if( tvs != 0? 1: ts != 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp3054)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp3058=
_tag_arr("typedef %s expects a different number of arguments", sizeof(
unsigned char), 51u); struct Cyc_Stdio_String_pa_struct _temp3059; _temp3059.tag=
Cyc_Stdio_String_pa; _temp3059.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2511);{ void* _temp3057=( void*)& _temp3059; void* _temp3055[ 1u]={
_temp3057}; struct _tagged_arr _temp3056={( void*) _temp3055,( void*) _temp3055,(
void*)( _temp3055 + 1u)}; _temp3054( _temp3058, _temp3056);}})); return cvtenv;}{
void* new_typ= Cyc_Tcutil_substitute( inst,( void*) td->defn);* _temp2508=({
struct Cyc_Core_Opt* _temp3060=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3060->v=( void*) new_typ; _temp3060;}); goto _LL2377;}}}
_LL2415: goto _LL2377; _LL2417: _temp2515= _temp2513; goto _LL2419; _LL2419:
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2515); goto _LL2377; _LL2421: for( 0; _temp2517 != 0; _temp2517=(( struct
Cyc_List_List*) _check_null( _temp2517))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_List_List*)
_check_null( _temp2517))->hd);} goto _LL2377; _LL2377:;} if( ! Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind( t), expected_kind)){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp3061)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp3067= _tag_arr("type %s has kind %s but as used here needs kind %s",
sizeof( unsigned char), 51u); struct Cyc_Stdio_String_pa_struct _temp3070;
_temp3070.tag= Cyc_Stdio_String_pa; _temp3070.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp3064=( void*)& _temp3070; struct Cyc_Stdio_String_pa_struct
_temp3069; _temp3069.tag= Cyc_Stdio_String_pa; _temp3069.f1=( struct _tagged_arr)
Cyc_Absynpp_kind2string( Cyc_Tcutil_typ_kind( t));{ void* _temp3065=( void*)&
_temp3069; struct Cyc_Stdio_String_pa_struct _temp3068; _temp3068.tag= Cyc_Stdio_String_pa;
_temp3068.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( expected_kind);{
void* _temp3066=( void*)& _temp3068; void* _temp3062[ 3u]={ _temp3064, _temp3065,
_temp3066}; struct _tagged_arr _temp3063={( void*) _temp3062,( void*) _temp3062,(
void*)( _temp3062 + 3u)}; _temp3061( _temp3067, _temp3063);}}}}));} return
cvtenv;} struct Cyc_List_List* Cyc_Tcutil_check_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* kind_env, void*
expected_kind, void* t){ struct Cyc_Tcutil_CVTEnv _temp3071= Cyc_Tcutil_i_check_valid_type(
loc, te,({ struct Cyc_Tcutil_CVTEnv _temp3086; _temp3086.kind_env= kind_env;
_temp3086.free_vars= 0; _temp3086.free_evars= 0; _temp3086;}), expected_kind, t);{
struct Cyc_List_List* vs= _temp3071.free_vars; for( 0; vs != 0; vs=(( struct Cyc_List_List*)
_check_null( vs))->tl){ Cyc_Tcutil_fast_add_free_tvar( kind_env,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( vs))->hd);}}{ struct Cyc_List_List* evs=
_temp3071.free_evars; for( 0; evs != 0; evs=(( struct Cyc_List_List*)
_check_null( evs))->tl){ void* _temp3072= Cyc_Tcutil_compress(( void*)(( struct
Cyc_List_List*) _check_null( evs))->hd); struct Cyc_Core_Opt* _temp3078; struct
Cyc_Core_Opt** _temp3080; _LL3074: if(( unsigned int) _temp3072 > 4u?*(( int*)
_temp3072) == Cyc_Absyn_Evar: 0){ _LL3079: _temp3078=(( struct Cyc_Absyn_Evar_struct*)
_temp3072)->f4; _temp3080=&(( struct Cyc_Absyn_Evar_struct*) _temp3072)->f4;
goto _LL3075;} else{ goto _LL3076;} _LL3076: goto _LL3077; _LL3075: if(*
_temp3080 == 0){* _temp3080=({ struct Cyc_Core_Opt* _temp3081=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3081->v=( void*) kind_env;
_temp3081;});} else{ struct Cyc_List_List* _temp3082=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(* _temp3080))->v; struct Cyc_List_List*
_temp3083= 0; for( 0; _temp3082 != 0; _temp3082=(( struct Cyc_List_List*)
_check_null( _temp3082))->tl){ if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp3082))->hd)){ _temp3083=({ struct Cyc_List_List* _temp3084=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3084->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp3082))->hd);
_temp3084->tl= _temp3083; _temp3084;});}}* _temp3080=({ struct Cyc_Core_Opt*
_temp3085=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3085->v=( void*) _temp3083; _temp3085;});} goto _LL3073; _LL3077: goto
_LL3073; _LL3073:;}} return _temp3071.free_vars;} void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, void* t){ struct
Cyc_List_List* _temp3087= Cyc_Tcutil_check_valid_type( loc, te, 0,( void*) Cyc_Absyn_MemKind,
t);{ struct Cyc_List_List* x= _temp3087; for( 0; x != 0; x=(( struct Cyc_List_List*)
_check_null( x))->tl){ struct Cyc_Absyn_Conref* c= Cyc_Absyn_compress_conref(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( x))->hd)->kind);
void* _temp3088=( void*) c->v; void* _temp3096; _LL3090: if( _temp3088 ==( void*)
Cyc_Absyn_No_constr){ goto _LL3091;} else{ goto _LL3092;} _LL3092: if((
unsigned int) _temp3088 > 1u?*(( int*) _temp3088) == Cyc_Absyn_Eq_constr: 0){
_LL3097: _temp3096=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3088)->f1;
if( _temp3096 ==( void*) Cyc_Absyn_MemKind){ goto _LL3093;} else{ goto _LL3094;}}
else{ goto _LL3094;} _LL3094: goto _LL3095; _LL3091: goto _LL3093; _LL3093:(
void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3098=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3098[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3099; _temp3099.tag= Cyc_Absyn_Eq_constr;
_temp3099.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3099;}); _temp3098;})));
goto _LL3089; _LL3095: goto _LL3089; _LL3089:;}} if( _temp3087 != 0){ void*
_temp3100= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp3106; struct
Cyc_List_List* _temp3108; struct Cyc_List_List* _temp3110; struct Cyc_Absyn_VarargInfo*
_temp3112; int _temp3114; struct Cyc_List_List* _temp3116; void* _temp3118;
struct Cyc_Core_Opt* _temp3120; struct Cyc_List_List* _temp3122; struct Cyc_List_List**
_temp3124; _LL3102: if(( unsigned int) _temp3100 > 4u?*(( int*) _temp3100) ==
Cyc_Absyn_FnType: 0){ _LL3107: _temp3106=(( struct Cyc_Absyn_FnType_struct*)
_temp3100)->f1; _LL3123: _temp3122= _temp3106.tvars; _temp3124=&((( struct Cyc_Absyn_FnType_struct*)
_temp3100)->f1).tvars; goto _LL3121; _LL3121: _temp3120= _temp3106.effect; goto
_LL3119; _LL3119: _temp3118=( void*) _temp3106.ret_typ; goto _LL3117; _LL3117:
_temp3116= _temp3106.args; goto _LL3115; _LL3115: _temp3114= _temp3106.c_varargs;
goto _LL3113; _LL3113: _temp3112= _temp3106.cyc_varargs; goto _LL3111; _LL3111:
_temp3110= _temp3106.rgn_po; goto _LL3109; _LL3109: _temp3108= _temp3106.attributes;
goto _LL3103;} else{ goto _LL3104;} _LL3104: goto _LL3105; _LL3103: if(*
_temp3124 == 0){* _temp3124= _temp3087;} else{ if( _temp3087 != 0){ struct
_tagged_arr _temp3125=*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp3087))->hd)->name; if(*(( const unsigned char*)
_check_unknown_subscript( _temp3125, sizeof( unsigned char), 0)) =='?'){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp3126)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp3129=
_tag_arr("You failed to abstract over the generated type variables. \nRemove the bound type variables or else make the effects of the type explicit\nand then abstract the free type variables properly.",
sizeof( unsigned char), 189u); void* _temp3127[ 0u]={}; struct _tagged_arr
_temp3128={( void*) _temp3127,( void*) _temp3127,( void*)( _temp3127 + 0u)};
_temp3126( _temp3129, _temp3128);}));} else{ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp3130)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp3134= _tag_arr("unbound type variable %s",
sizeof( unsigned char), 25u); struct Cyc_Stdio_String_pa_struct _temp3135;
_temp3135.tag= Cyc_Stdio_String_pa; _temp3135.f1=( struct _tagged_arr) _temp3125;{
void* _temp3133=( void*)& _temp3135; void* _temp3131[ 1u]={ _temp3133}; struct
_tagged_arr _temp3132={( void*) _temp3131,( void*) _temp3131,( void*)( _temp3131
+ 1u)}; _temp3130( _temp3134, _temp3132);}}));}}} goto _LL3101; _LL3105: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp3136)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp3140=
_tag_arr("unbound type variable %s", sizeof( unsigned char), 25u); struct Cyc_Stdio_String_pa_struct
_temp3141; _temp3141.tag= Cyc_Stdio_String_pa; _temp3141.f1=( struct _tagged_arr)*((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp3087))->hd)->name;{
void* _temp3139=( void*)& _temp3141; void* _temp3137[ 1u]={ _temp3139}; struct
_tagged_arr _temp3138={( void*) _temp3137,( void*) _temp3137,( void*)( _temp3137
+ 1u)}; _temp3136( _temp3140, _temp3138);}})); return; _LL3101:;}} void Cyc_Tcutil_add_tvar_identity(
struct Cyc_Absyn_Tvar* tv){ if( tv->identity == 0){ tv->identity= Cyc_Tcutil_new_tvar_id();}}
void Cyc_Tcutil_add_tvar_identities( struct Cyc_List_List* tvs){(( void(*)( void(*
f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Tcutil_add_tvar_identity,
tvs);} void Cyc_Tcutil_check_type( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* bound_tvars, void* expected_kind, void* t){ struct Cyc_List_List*
_temp3142= Cyc_Tcutil_check_valid_type( loc, te, bound_tvars, expected_kind, t);
struct Cyc_List_List* _temp3143= Cyc_Tcutil_remove_bound_tvars( _temp3142,
bound_tvars); struct Cyc_List_List* fs= _temp3143; for( 0; fs != 0; fs=(( struct
Cyc_List_List*) _check_null( fs))->tl){ struct _tagged_arr* _temp3144=(( struct
Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( fs))->hd)->name; Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp3145)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp3150=
_tag_arr("unbound type variable %s in type %s", sizeof( unsigned char), 36u);
struct Cyc_Stdio_String_pa_struct _temp3152; _temp3152.tag= Cyc_Stdio_String_pa;
_temp3152.f1=( struct _tagged_arr)* _temp3144;{ void* _temp3148=( void*)&
_temp3152; struct Cyc_Stdio_String_pa_struct _temp3151; _temp3151.tag= Cyc_Stdio_String_pa;
_temp3151.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp3149=(
void*)& _temp3151; void* _temp3146[ 2u]={ _temp3148, _temp3149}; struct
_tagged_arr _temp3147={( void*) _temp3146,( void*) _temp3146,( void*)( _temp3146
+ 2u)}; _temp3145( _temp3150, _temp3147);}}}));}} static void Cyc_Tcutil_check_unique_unsorted(
int(* cmp)( void*, void*), struct Cyc_List_List* vs, struct Cyc_Position_Segment*
loc, struct _tagged_arr(* a2string)( void*), struct _tagged_arr msg){ for( 0; vs
!= 0; vs=(( struct Cyc_List_List*) _check_null( vs))->tl){ struct Cyc_List_List*
vs2=(( struct Cyc_List_List*) _check_null( vs))->tl; for( 0; vs2 != 0; vs2=((
struct Cyc_List_List*) _check_null( vs2))->tl){ if( cmp(( void*)(( struct Cyc_List_List*)
_check_null( vs))->hd,( void*)(( struct Cyc_List_List*) _check_null( vs2))->hd)
== 0){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp3153)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp3158= _tag_arr("%s: %s", sizeof( unsigned char), 7u);
struct Cyc_Stdio_String_pa_struct _temp3160; _temp3160.tag= Cyc_Stdio_String_pa;
_temp3160.f1=( struct _tagged_arr) msg;{ void* _temp3156=( void*)& _temp3160;
struct Cyc_Stdio_String_pa_struct _temp3159; _temp3159.tag= Cyc_Stdio_String_pa;
_temp3159.f1=( struct _tagged_arr) a2string(( void*)(( struct Cyc_List_List*)
_check_null( vs))->hd);{ void* _temp3157=( void*)& _temp3159; void* _temp3154[ 2u]={
_temp3156, _temp3157}; struct _tagged_arr _temp3155={( void*) _temp3154,( void*)
_temp3154,( void*)( _temp3154 + 2u)}; _temp3153( _temp3158, _temp3155);}}}));}}}}
static struct _tagged_arr Cyc_Tcutil_strptr2string( struct _tagged_arr* s){
return* s;} void Cyc_Tcutil_check_unique_vars( struct Cyc_List_List* vs, struct
Cyc_Position_Segment* loc, struct _tagged_arr msg){(( void(*)( int(* cmp)(
struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* vs, struct Cyc_Position_Segment*
loc, struct _tagged_arr(* a2string)( struct _tagged_arr*), struct _tagged_arr
msg)) Cyc_Tcutil_check_unique_unsorted)( Cyc_String_zstrptrcmp, vs, loc, Cyc_Tcutil_strptr2string,
msg);} void Cyc_Tcutil_check_unique_tvars( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* tvs){(( void(*)( int(* cmp)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* vs, struct Cyc_Position_Segment*
loc, struct _tagged_arr(* a2string)( struct Cyc_Absyn_Tvar*), struct _tagged_arr
msg)) Cyc_Tcutil_check_unique_unsorted)( Cyc_Absyn_tvar_cmp, tvs, loc, Cyc_Tcutil_tvar2string,
_tag_arr("duplicate type variable", sizeof( unsigned char), 24u));} struct
_tuple17{ struct Cyc_Absyn_Structfield* f1; int f2; } ; struct _tuple18{ struct
Cyc_List_List* f1; void* f2; } ; struct _tuple19{ struct Cyc_Absyn_Structfield*
f1; void* f2; } ; struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* sdfields){ struct Cyc_List_List* fields= 0;{ struct
Cyc_List_List* sd_fields= sdfields; for( 0; sd_fields != 0; sd_fields=(( struct
Cyc_List_List*) _check_null( sd_fields))->tl){ if( Cyc_String_strcmp(*(( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( sd_fields))->hd)->name,
_tag_arr("", sizeof( unsigned char), 1u)) != 0){ fields=({ struct Cyc_List_List*
_temp3161=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3161->hd=( void*)({ struct _tuple17* _temp3162=( struct _tuple17*)
GC_malloc( sizeof( struct _tuple17)); _temp3162->f1=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( sd_fields))->hd; _temp3162->f2= 0; _temp3162;});
_temp3161->tl= fields; _temp3161;});}}} fields=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields);{ struct Cyc_List_List* ans=
0; for( 0; des != 0; des=(( struct Cyc_List_List*) _check_null( des))->tl){
struct _tuple18 _temp3165; void* _temp3166; struct Cyc_List_List* _temp3168;
struct _tuple18* _temp3163=( struct _tuple18*)(( struct Cyc_List_List*)
_check_null( des))->hd; _temp3165=* _temp3163; _LL3169: _temp3168= _temp3165.f1;
goto _LL3167; _LL3167: _temp3166= _temp3165.f2; goto _LL3164; _LL3164: if(
_temp3168 == 0){ struct Cyc_List_List* _temp3170= fields; for( 0; _temp3170 != 0;
_temp3170=(( struct Cyc_List_List*) _check_null( _temp3170))->tl){ if( !(*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp3170))->hd)).f2){(*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp3170))->hd)).f2= 1;(*((
struct _tuple18*)(( struct Cyc_List_List*) _check_null( des))->hd)).f1=({ struct
Cyc_List_List* _temp3171=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3171->hd=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp3172=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp3172[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp3173; _temp3173.tag= Cyc_Absyn_FieldName;
_temp3173.f1=((*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3170))->hd)).f1)->name; _temp3173;}); _temp3172;})); _temp3171->tl= 0;
_temp3171;}); ans=({ struct Cyc_List_List* _temp3174=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp3174->hd=( void*)({
struct _tuple19* _temp3175=( struct _tuple19*) _region_malloc( rgn, sizeof(
struct _tuple19)); _temp3175->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3170))->hd)).f1; _temp3175->f2= _temp3166; _temp3175;});
_temp3174->tl= ans; _temp3174;}); break;}} if( _temp3170 == 0){ Cyc_Tcutil_terr(
loc, _tag_arr("too many arguments to struct", sizeof( unsigned char), 29u));}}
else{ if((( struct Cyc_List_List*) _check_null( _temp3168))->tl != 0){ Cyc_Tcutil_terr(
loc, _tag_arr("multiple designators are not supported", sizeof( unsigned char),
39u));} else{ void* _temp3176=( void*)(( struct Cyc_List_List*) _check_null(
_temp3168))->hd; struct _tagged_arr* _temp3182; _LL3178: if(*(( int*) _temp3176)
== Cyc_Absyn_ArrayElement){ goto _LL3179;} else{ goto _LL3180;} _LL3180: if(*((
int*) _temp3176) == Cyc_Absyn_FieldName){ _LL3183: _temp3182=(( struct Cyc_Absyn_FieldName_struct*)
_temp3176)->f1; goto _LL3181;} else{ goto _LL3177;} _LL3179: Cyc_Tcutil_terr(
loc, _tag_arr("array designator used in argument to struct", sizeof(
unsigned char), 44u)); goto _LL3177; _LL3181: { struct Cyc_List_List* _temp3184=
fields; for( 0; _temp3184 != 0; _temp3184=(( struct Cyc_List_List*) _check_null(
_temp3184))->tl){ if( Cyc_String_zstrptrcmp( _temp3182,((*(( struct _tuple17*)((
struct Cyc_List_List*) _check_null( _temp3184))->hd)).f1)->name) == 0){ if((*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp3184))->hd)).f2){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp3185)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp3189= _tag_arr("field %s has already been used as an argument",
sizeof( unsigned char), 46u); struct Cyc_Stdio_String_pa_struct _temp3190;
_temp3190.tag= Cyc_Stdio_String_pa; _temp3190.f1=( struct _tagged_arr)*
_temp3182;{ void* _temp3188=( void*)& _temp3190; void* _temp3186[ 1u]={
_temp3188}; struct _tagged_arr _temp3187={( void*) _temp3186,( void*) _temp3186,(
void*)( _temp3186 + 1u)}; _temp3185( _temp3189, _temp3187);}}));}(*(( struct
_tuple17*)(( struct Cyc_List_List*) _check_null( _temp3184))->hd)).f2= 1; ans=({
struct Cyc_List_List* _temp3191=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp3191->hd=( void*)({ struct _tuple19*
_temp3192=( struct _tuple19*) _region_malloc( rgn, sizeof( struct _tuple19));
_temp3192->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3184))->hd)).f1; _temp3192->f2= _temp3166; _temp3192;}); _temp3191->tl= ans;
_temp3191;}); break;}} if( _temp3184 == 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp3193)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp3197= _tag_arr("bad field designator %s",
sizeof( unsigned char), 24u); struct Cyc_Stdio_String_pa_struct _temp3198;
_temp3198.tag= Cyc_Stdio_String_pa; _temp3198.f1=( struct _tagged_arr)*
_temp3182;{ void* _temp3196=( void*)& _temp3198; void* _temp3194[ 1u]={
_temp3196}; struct _tagged_arr _temp3195={( void*) _temp3194,( void*) _temp3194,(
void*)( _temp3194 + 1u)}; _temp3193( _temp3197, _temp3195);}}));} goto _LL3177;}
_LL3177:;}}} for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null(
fields))->tl){ if( !(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
fields))->hd)).f2){ Cyc_Tcutil_terr( loc, _tag_arr("too few arguments to struct",
sizeof( unsigned char), 28u)); break;}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( ans);}} int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void* t, void** elt_typ_dest){ void* _temp3199= Cyc_Tcutil_compress( t); struct
Cyc_Absyn_PtrInfo _temp3205; struct Cyc_Absyn_Conref* _temp3207; void* _temp3209;
_LL3201: if(( unsigned int) _temp3199 > 4u?*(( int*) _temp3199) == Cyc_Absyn_PointerType:
0){ _LL3206: _temp3205=(( struct Cyc_Absyn_PointerType_struct*) _temp3199)->f1;
_LL3210: _temp3209=( void*) _temp3205.elt_typ; goto _LL3208; _LL3208: _temp3207=
_temp3205.bounds; goto _LL3202;} else{ goto _LL3203;} _LL3203: goto _LL3204;
_LL3202: { struct Cyc_Absyn_Conref* _temp3211= Cyc_Absyn_compress_conref(
_temp3207); void* _temp3212=( void*)( Cyc_Absyn_compress_conref( _temp3211))->v;
void* _temp3220; _LL3214: if(( unsigned int) _temp3212 > 1u?*(( int*) _temp3212)
== Cyc_Absyn_Eq_constr: 0){ _LL3221: _temp3220=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3212)->f1; if( _temp3220 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3215;}
else{ goto _LL3216;}} else{ goto _LL3216;} _LL3216: if( _temp3212 ==( void*) Cyc_Absyn_No_constr){
goto _LL3217;} else{ goto _LL3218;} _LL3218: goto _LL3219; _LL3215:*
elt_typ_dest= _temp3209; return 1; _LL3217:( void*)( _temp3211->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3222=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3222[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3223; _temp3223.tag= Cyc_Absyn_Eq_constr;
_temp3223.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp3223;}); _temp3222;})));*
elt_typ_dest= _temp3209; return 1; _LL3219: return 0; _LL3213:;} _LL3204: return
0; _LL3200:;} int Cyc_Tcutil_is_tagged_pointer_typ( void* t){ void* ignore=(
void*) Cyc_Absyn_VoidType; return Cyc_Tcutil_is_tagged_pointer_typ_elt( t,&
ignore);} struct _tuple7 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ struct _tuple7 bogus_ans=({ struct _tuple7 _temp3405;
_temp3405.f1= 0; _temp3405.f2=( void*) Cyc_Absyn_HeapRgn; _temp3405;}); void*
_temp3224=( void*) e->r; void* _temp3238; struct _tuple1* _temp3240; struct
_tagged_arr* _temp3242; struct Cyc_Absyn_Exp* _temp3244; struct _tagged_arr*
_temp3246; struct Cyc_Absyn_Exp* _temp3248; struct Cyc_Absyn_Exp* _temp3250;
struct Cyc_Absyn_Exp* _temp3252; struct Cyc_Absyn_Exp* _temp3254; _LL3226: if(*((
int*) _temp3224) == Cyc_Absyn_Var_e){ _LL3241: _temp3240=(( struct Cyc_Absyn_Var_e_struct*)
_temp3224)->f1; goto _LL3239; _LL3239: _temp3238=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3224)->f2; goto _LL3227;} else{ goto _LL3228;} _LL3228: if(*(( int*)
_temp3224) == Cyc_Absyn_StructMember_e){ _LL3245: _temp3244=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3224)->f1; goto _LL3243; _LL3243: _temp3242=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3224)->f2; goto _LL3229;} else{ goto _LL3230;} _LL3230: if(*(( int*)
_temp3224) == Cyc_Absyn_StructArrow_e){ _LL3249: _temp3248=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3224)->f1; goto _LL3247; _LL3247: _temp3246=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3224)->f2; goto _LL3231;} else{ goto _LL3232;} _LL3232: if(*(( int*)
_temp3224) == Cyc_Absyn_Deref_e){ _LL3251: _temp3250=(( struct Cyc_Absyn_Deref_e_struct*)
_temp3224)->f1; goto _LL3233;} else{ goto _LL3234;} _LL3234: if(*(( int*)
_temp3224) == Cyc_Absyn_Subscript_e){ _LL3255: _temp3254=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3224)->f1; goto _LL3253; _LL3253: _temp3252=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3224)->f2; goto _LL3235;} else{ goto _LL3236;} _LL3236: goto _LL3237;
_LL3227: { void* _temp3256= _temp3238; struct Cyc_Absyn_Vardecl* _temp3270;
struct Cyc_Absyn_Vardecl* _temp3272; struct Cyc_Absyn_Vardecl* _temp3274; struct
Cyc_Absyn_Vardecl* _temp3276; _LL3258: if( _temp3256 ==( void*) Cyc_Absyn_Unresolved_b){
goto _LL3259;} else{ goto _LL3260;} _LL3260: if(( unsigned int) _temp3256 > 1u?*((
int*) _temp3256) == Cyc_Absyn_Funname_b: 0){ goto _LL3261;} else{ goto _LL3262;}
_LL3262: if(( unsigned int) _temp3256 > 1u?*(( int*) _temp3256) == Cyc_Absyn_Global_b:
0){ _LL3271: _temp3270=(( struct Cyc_Absyn_Global_b_struct*) _temp3256)->f1;
goto _LL3263;} else{ goto _LL3264;} _LL3264: if(( unsigned int) _temp3256 > 1u?*((
int*) _temp3256) == Cyc_Absyn_Local_b: 0){ _LL3273: _temp3272=(( struct Cyc_Absyn_Local_b_struct*)
_temp3256)->f1; goto _LL3265;} else{ goto _LL3266;} _LL3266: if(( unsigned int)
_temp3256 > 1u?*(( int*) _temp3256) == Cyc_Absyn_Pat_b: 0){ _LL3275: _temp3274=((
struct Cyc_Absyn_Pat_b_struct*) _temp3256)->f1; goto _LL3267;} else{ goto
_LL3268;} _LL3268: if(( unsigned int) _temp3256 > 1u?*(( int*) _temp3256) == Cyc_Absyn_Param_b:
0){ _LL3277: _temp3276=(( struct Cyc_Absyn_Param_b_struct*) _temp3256)->f1; goto
_LL3269;} else{ goto _LL3257;} _LL3259: return bogus_ans; _LL3261: return({
struct _tuple7 _temp3278; _temp3278.f1= 1; _temp3278.f2=( void*) Cyc_Absyn_HeapRgn;
_temp3278;}); _LL3263: { void* _temp3279= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v); _LL3281: if(( unsigned int) _temp3279
> 4u?*(( int*) _temp3279) == Cyc_Absyn_ArrayType: 0){ goto _LL3282;} else{ goto
_LL3283;} _LL3283: goto _LL3284; _LL3282: return({ struct _tuple7 _temp3285;
_temp3285.f1= 1; _temp3285.f2=( void*) Cyc_Absyn_HeapRgn; _temp3285;}); _LL3284:
return({ struct _tuple7 _temp3286; _temp3286.f1=( _temp3270->tq).q_const;
_temp3286.f2=( void*) Cyc_Absyn_HeapRgn; _temp3286;}); _LL3280:;} _LL3265: {
void* _temp3287= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); _LL3289: if(( unsigned int) _temp3287 > 4u?*(( int*)
_temp3287) == Cyc_Absyn_ArrayType: 0){ goto _LL3290;} else{ goto _LL3291;}
_LL3291: goto _LL3292; _LL3290: return({ struct _tuple7 _temp3293; _temp3293.f1=
1; _temp3293.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3272->rgn))->v;
_temp3293;}); _LL3292: return({ struct _tuple7 _temp3294; _temp3294.f1=(
_temp3272->tq).q_const; _temp3294.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3272->rgn))->v; _temp3294;}); _LL3288:;} _LL3267: _temp3276=
_temp3274; goto _LL3269; _LL3269: return({ struct _tuple7 _temp3295; _temp3295.f1=(
_temp3276->tq).q_const; _temp3295.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3276->rgn))->v; _temp3295;}); _LL3257:;} _LL3229: { void*
_temp3296= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp3244->topt))->v); struct Cyc_List_List* _temp3308; struct Cyc_List_List*
_temp3310; struct Cyc_Absyn_Structdecl** _temp3312; struct Cyc_Absyn_Structdecl*
_temp3314; struct Cyc_Absyn_Uniondecl** _temp3315; struct Cyc_Absyn_Uniondecl*
_temp3317; _LL3298: if(( unsigned int) _temp3296 > 4u?*(( int*) _temp3296) ==
Cyc_Absyn_AnonStructType: 0){ _LL3309: _temp3308=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3296)->f1; goto _LL3299;} else{ goto _LL3300;} _LL3300: if(( unsigned int)
_temp3296 > 4u?*(( int*) _temp3296) == Cyc_Absyn_AnonUnionType: 0){ _LL3311:
_temp3310=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3296)->f1; goto
_LL3301;} else{ goto _LL3302;} _LL3302: if(( unsigned int) _temp3296 > 4u?*((
int*) _temp3296) == Cyc_Absyn_StructType: 0){ _LL3313: _temp3312=(( struct Cyc_Absyn_StructType_struct*)
_temp3296)->f3; if( _temp3312 == 0){ goto _LL3304;} else{ _temp3314=* _temp3312;
goto _LL3303;}} else{ goto _LL3304;} _LL3304: if(( unsigned int) _temp3296 > 4u?*((
int*) _temp3296) == Cyc_Absyn_UnionType: 0){ _LL3316: _temp3315=(( struct Cyc_Absyn_UnionType_struct*)
_temp3296)->f3; if( _temp3315 == 0){ goto _LL3306;} else{ _temp3317=* _temp3315;
goto _LL3305;}} else{ goto _LL3306;} _LL3306: goto _LL3307; _LL3299: _temp3310=
_temp3308; goto _LL3301; _LL3301: { struct Cyc_Absyn_Structfield* _temp3318= Cyc_Absyn_lookup_field(
_temp3310, _temp3242); if( _temp3318 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3318))->width != 0: 0){ return({ struct _tuple7 _temp3319;
_temp3319.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3318))->tq).q_const;
_temp3319.f2=( Cyc_Tcutil_addressof_props( te, _temp3244)).f2; _temp3319;});}
return bogus_ans;} _LL3303: { struct Cyc_Absyn_Structfield* _temp3320= Cyc_Absyn_lookup_struct_field(
_temp3314, _temp3242); if( _temp3320 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3320))->width != 0: 0){ return({ struct _tuple7 _temp3321;
_temp3321.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3320))->tq).q_const;
_temp3321.f2=( Cyc_Tcutil_addressof_props( te, _temp3244)).f2; _temp3321;});}
return bogus_ans;} _LL3305: { struct Cyc_Absyn_Structfield* _temp3322= Cyc_Absyn_lookup_union_field(
_temp3317, _temp3242); if( _temp3322 != 0){ return({ struct _tuple7 _temp3323;
_temp3323.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3322))->tq).q_const;
_temp3323.f2=( Cyc_Tcutil_addressof_props( te, _temp3244)).f2; _temp3323;});}
goto _LL3307;} _LL3307: return bogus_ans; _LL3297:;} _LL3231: { void* _temp3324=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3248->topt))->v);
struct Cyc_Absyn_PtrInfo _temp3330; void* _temp3332; void* _temp3334; _LL3326:
if(( unsigned int) _temp3324 > 4u?*(( int*) _temp3324) == Cyc_Absyn_PointerType:
0){ _LL3331: _temp3330=(( struct Cyc_Absyn_PointerType_struct*) _temp3324)->f1;
_LL3335: _temp3334=( void*) _temp3330.elt_typ; goto _LL3333; _LL3333: _temp3332=(
void*) _temp3330.rgn_typ; goto _LL3327;} else{ goto _LL3328;} _LL3328: goto
_LL3329; _LL3327: { void* _temp3336= Cyc_Tcutil_compress( _temp3334); struct Cyc_List_List*
_temp3348; struct Cyc_List_List* _temp3350; struct Cyc_Absyn_Structdecl**
_temp3352; struct Cyc_Absyn_Structdecl* _temp3354; struct Cyc_Absyn_Uniondecl**
_temp3355; struct Cyc_Absyn_Uniondecl* _temp3357; _LL3338: if(( unsigned int)
_temp3336 > 4u?*(( int*) _temp3336) == Cyc_Absyn_AnonStructType: 0){ _LL3349:
_temp3348=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3336)->f1; goto
_LL3339;} else{ goto _LL3340;} _LL3340: if(( unsigned int) _temp3336 > 4u?*((
int*) _temp3336) == Cyc_Absyn_AnonUnionType: 0){ _LL3351: _temp3350=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3336)->f1; goto _LL3341;} else{ goto
_LL3342;} _LL3342: if(( unsigned int) _temp3336 > 4u?*(( int*) _temp3336) == Cyc_Absyn_StructType:
0){ _LL3353: _temp3352=(( struct Cyc_Absyn_StructType_struct*) _temp3336)->f3;
if( _temp3352 == 0){ goto _LL3344;} else{ _temp3354=* _temp3352; goto _LL3343;}}
else{ goto _LL3344;} _LL3344: if(( unsigned int) _temp3336 > 4u?*(( int*)
_temp3336) == Cyc_Absyn_UnionType: 0){ _LL3356: _temp3355=(( struct Cyc_Absyn_UnionType_struct*)
_temp3336)->f3; if( _temp3355 == 0){ goto _LL3346;} else{ _temp3357=* _temp3355;
goto _LL3345;}} else{ goto _LL3346;} _LL3346: goto _LL3347; _LL3339: _temp3350=
_temp3348; goto _LL3341; _LL3341: { struct Cyc_Absyn_Structfield* _temp3358= Cyc_Absyn_lookup_field(
_temp3350, _temp3246); if( _temp3358 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3358))->width != 0: 0){ return({ struct _tuple7 _temp3359;
_temp3359.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3358))->tq).q_const;
_temp3359.f2= _temp3332; _temp3359;});} return bogus_ans;} _LL3343: { struct Cyc_Absyn_Structfield*
_temp3360= Cyc_Absyn_lookup_struct_field( _temp3354, _temp3246); if( _temp3360
!= 0?(( struct Cyc_Absyn_Structfield*) _check_null( _temp3360))->width != 0: 0){
return({ struct _tuple7 _temp3361; _temp3361.f1=((( struct Cyc_Absyn_Structfield*)
_check_null( _temp3360))->tq).q_const; _temp3361.f2= _temp3332; _temp3361;});}
return bogus_ans;} _LL3345: { struct Cyc_Absyn_Structfield* _temp3362= Cyc_Absyn_lookup_union_field(
_temp3357, _temp3246); if( _temp3362 != 0){ return({ struct _tuple7 _temp3363;
_temp3363.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3362))->tq).q_const;
_temp3363.f2= _temp3332; _temp3363;});} return bogus_ans;} _LL3347: return
bogus_ans; _LL3337:;} _LL3329: return bogus_ans; _LL3325:;} _LL3233: { void*
_temp3364= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp3250->topt))->v); struct Cyc_Absyn_PtrInfo _temp3370; struct Cyc_Absyn_Tqual
_temp3372; void* _temp3374; _LL3366: if(( unsigned int) _temp3364 > 4u?*(( int*)
_temp3364) == Cyc_Absyn_PointerType: 0){ _LL3371: _temp3370=(( struct Cyc_Absyn_PointerType_struct*)
_temp3364)->f1; _LL3375: _temp3374=( void*) _temp3370.rgn_typ; goto _LL3373;
_LL3373: _temp3372= _temp3370.tq; goto _LL3367;} else{ goto _LL3368;} _LL3368:
goto _LL3369; _LL3367: return({ struct _tuple7 _temp3376; _temp3376.f1=
_temp3372.q_const; _temp3376.f2= _temp3374; _temp3376;}); _LL3369: return
bogus_ans; _LL3365:;} _LL3235: { void* t= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp3254->topt))->v); void* _temp3377= t; struct
Cyc_Absyn_Tqual _temp3387; struct Cyc_List_List* _temp3389; struct Cyc_Absyn_PtrInfo
_temp3391; struct Cyc_Absyn_Conref* _temp3393; struct Cyc_Absyn_Tqual _temp3395;
void* _temp3397; void* _temp3399; _LL3379: if(( unsigned int) _temp3377 > 4u?*((
int*) _temp3377) == Cyc_Absyn_ArrayType: 0){ _LL3388: _temp3387=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3377)->f2; goto _LL3380;} else{ goto _LL3381;} _LL3381: if(( unsigned int)
_temp3377 > 4u?*(( int*) _temp3377) == Cyc_Absyn_TupleType: 0){ _LL3390:
_temp3389=(( struct Cyc_Absyn_TupleType_struct*) _temp3377)->f1; goto _LL3382;}
else{ goto _LL3383;} _LL3383: if(( unsigned int) _temp3377 > 4u?*(( int*)
_temp3377) == Cyc_Absyn_PointerType: 0){ _LL3392: _temp3391=(( struct Cyc_Absyn_PointerType_struct*)
_temp3377)->f1; _LL3400: _temp3399=( void*) _temp3391.elt_typ; goto _LL3398;
_LL3398: _temp3397=( void*) _temp3391.rgn_typ; goto _LL3396; _LL3396: _temp3395=
_temp3391.tq; goto _LL3394; _LL3394: _temp3393= _temp3391.bounds; goto _LL3384;}
else{ goto _LL3385;} _LL3385: goto _LL3386; _LL3380: return({ struct _tuple7
_temp3401; _temp3401.f1= _temp3387.q_const; _temp3401.f2=( Cyc_Tcutil_addressof_props(
te, _temp3254)).f2; _temp3401;}); _LL3382: { struct _tuple4* _temp3402= Cyc_Absyn_lookup_tuple_field(
_temp3389,( int) Cyc_Evexp_eval_const_uint_exp( _temp3252)); if( _temp3402 != 0){
return({ struct _tuple7 _temp3403; _temp3403.f1=((*(( struct _tuple4*)
_check_null( _temp3402))).f1).q_const; _temp3403.f2=( Cyc_Tcutil_addressof_props(
te, _temp3254)).f2; _temp3403;});} return bogus_ans;} _LL3384: return({ struct
_tuple7 _temp3404; _temp3404.f1= _temp3395.q_const; _temp3404.f2= _temp3397;
_temp3404;}); _LL3386: return bogus_ans; _LL3378:;} _LL3237: Cyc_Tcutil_terr( e->loc,
_tag_arr("unary & applied to non-lvalue", sizeof( unsigned char), 30u)); return
bogus_ans; _LL3225:;} void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv* te,
void* e_typ, struct Cyc_Absyn_Exp* e){ void* _temp3406= Cyc_Tcutil_compress(
e_typ); struct Cyc_Absyn_Tqual _temp3412; void* _temp3414; _LL3408: if((
unsigned int) _temp3406 > 4u?*(( int*) _temp3406) == Cyc_Absyn_ArrayType: 0){
_LL3415: _temp3414=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3406)->f1;
goto _LL3413; _LL3413: _temp3412=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3406)->f2; goto _LL3409;} else{ goto _LL3410;} _LL3410: goto _LL3411;
_LL3409: { void* _temp3418; struct _tuple7 _temp3416= Cyc_Tcutil_addressof_props(
te, e); _LL3419: _temp3418= _temp3416.f2; goto _LL3417; _LL3417: return Cyc_Absyn_atb_typ(
_temp3414, _temp3418, _temp3412,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp3420=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3420[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3421; _temp3421.tag= Cyc_Absyn_Upper_b;
_temp3421.f1= e; _temp3421;}); _temp3420;}));} _LL3411: return e_typ; _LL3407:;}
void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment* loc, unsigned int i,
struct Cyc_Absyn_Conref* b){ b= Cyc_Absyn_compress_conref( b);{ void* _temp3422=(
void*) b->v; void* _temp3430; void* _temp3432; struct Cyc_Absyn_Exp* _temp3434;
_LL3424: if(( unsigned int) _temp3422 > 1u?*(( int*) _temp3422) == Cyc_Absyn_Eq_constr:
0){ _LL3431: _temp3430=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3422)->f1;
if( _temp3430 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3425;} else{ goto _LL3426;}}
else{ goto _LL3426;} _LL3426: if(( unsigned int) _temp3422 > 1u?*(( int*)
_temp3422) == Cyc_Absyn_Eq_constr: 0){ _LL3433: _temp3432=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3422)->f1; if(( unsigned int) _temp3432 > 1u?*(( int*) _temp3432) == Cyc_Absyn_Upper_b:
0){ _LL3435: _temp3434=(( struct Cyc_Absyn_Upper_b_struct*) _temp3432)->f1; goto
_LL3427;} else{ goto _LL3428;}} else{ goto _LL3428;} _LL3428: goto _LL3429;
_LL3425: return; _LL3427: if( Cyc_Evexp_eval_const_uint_exp( _temp3434) <= i){
Cyc_Tcutil_terr( loc, _tag_arr("dereference is out of bounds", sizeof(
unsigned char), 29u));} return; _LL3429:( void*)( b->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_constr_struct* _temp3436=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3436[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3437; _temp3437.tag= Cyc_Absyn_Eq_constr;
_temp3437.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3438=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3438[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3439; _temp3439.tag= Cyc_Absyn_Upper_b;
_temp3439.f1= Cyc_Absyn_uint_exp( i + 1, 0); _temp3439;}); _temp3438;}));
_temp3437;}); _temp3436;}))); return; _LL3423:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_bits_only( void* t){ void* _temp3440= Cyc_Tcutil_compress(
t); void* _temp3466; struct Cyc_List_List* _temp3468; struct Cyc_Absyn_Structdecl**
_temp3470; struct Cyc_List_List* _temp3472; struct Cyc_Absyn_Uniondecl**
_temp3474; struct Cyc_List_List* _temp3476; struct Cyc_List_List* _temp3478;
struct Cyc_List_List* _temp3480; _LL3442: if( _temp3440 ==( void*) Cyc_Absyn_VoidType){
goto _LL3443;} else{ goto _LL3444;} _LL3444: if(( unsigned int) _temp3440 > 4u?*((
int*) _temp3440) == Cyc_Absyn_IntType: 0){ goto _LL3445;} else{ goto _LL3446;}
_LL3446: if( _temp3440 ==( void*) Cyc_Absyn_FloatType){ goto _LL3447;} else{
goto _LL3448;} _LL3448: if( _temp3440 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL3449;} else{ goto _LL3450;} _LL3450: if(( unsigned int) _temp3440 > 4u?*((
int*) _temp3440) == Cyc_Absyn_EnumType: 0){ goto _LL3451;} else{ goto _LL3452;}
_LL3452: if(( unsigned int) _temp3440 > 4u?*(( int*) _temp3440) == Cyc_Absyn_ArrayType:
0){ _LL3467: _temp3466=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3440)->f1;
goto _LL3453;} else{ goto _LL3454;} _LL3454: if(( unsigned int) _temp3440 > 4u?*((
int*) _temp3440) == Cyc_Absyn_TupleType: 0){ _LL3469: _temp3468=(( struct Cyc_Absyn_TupleType_struct*)
_temp3440)->f1; goto _LL3455;} else{ goto _LL3456;} _LL3456: if(( unsigned int)
_temp3440 > 4u?*(( int*) _temp3440) == Cyc_Absyn_StructType: 0){ _LL3473:
_temp3472=(( struct Cyc_Absyn_StructType_struct*) _temp3440)->f2; goto _LL3471;
_LL3471: _temp3470=(( struct Cyc_Absyn_StructType_struct*) _temp3440)->f3; goto
_LL3457;} else{ goto _LL3458;} _LL3458: if(( unsigned int) _temp3440 > 4u?*((
int*) _temp3440) == Cyc_Absyn_UnionType: 0){ _LL3477: _temp3476=(( struct Cyc_Absyn_UnionType_struct*)
_temp3440)->f2; goto _LL3475; _LL3475: _temp3474=(( struct Cyc_Absyn_UnionType_struct*)
_temp3440)->f3; goto _LL3459;} else{ goto _LL3460;} _LL3460: if(( unsigned int)
_temp3440 > 4u?*(( int*) _temp3440) == Cyc_Absyn_AnonStructType: 0){ _LL3479:
_temp3478=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3440)->f1; goto
_LL3461;} else{ goto _LL3462;} _LL3462: if(( unsigned int) _temp3440 > 4u?*((
int*) _temp3440) == Cyc_Absyn_AnonUnionType: 0){ _LL3481: _temp3480=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3440)->f1; goto _LL3463;} else{ goto
_LL3464;} _LL3464: goto _LL3465; _LL3443: return 1; _LL3445: return 1; _LL3447:
return 1; _LL3449: return 1; _LL3451: return 0; _LL3453: return Cyc_Tcutil_bits_only(
_temp3466); _LL3455: for( 0; _temp3468 != 0; _temp3468=(( struct Cyc_List_List*)
_check_null( _temp3468))->tl){ if( ! Cyc_Tcutil_bits_only((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp3468))->hd)).f2)){ return 0;}} return 1;
_LL3457: if( _temp3470 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* _temp3482=*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp3470)); if( _temp3482->fields
== 0){ return 0;}{ struct _RegionHandle _temp3483= _new_region(); struct
_RegionHandle* rgn=& _temp3483; _push_region( rgn);{ struct Cyc_List_List*
_temp3484=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp3482->tvs, _temp3472);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp3482->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute(
rgn, _temp3484,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3485= 0; _npop_handler( 0u); return
_temp3485;}}}{ int _temp3486= 1; _npop_handler( 0u); return _temp3486;}};
_pop_region( rgn);}} _LL3459: if( _temp3474 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
_temp3487=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp3474)); if(
_temp3487->fields == 0){ return 0;}{ struct _RegionHandle _temp3488= _new_region();
struct _RegionHandle* rgn=& _temp3488; _push_region( rgn);{ struct Cyc_List_List*
_temp3489=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp3487->tvs, _temp3476);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp3487->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute(
rgn, _temp3489,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3490= 0; _npop_handler( 0u); return
_temp3490;}}}{ int _temp3491= 1; _npop_handler( 0u); return _temp3491;}};
_pop_region( rgn);}} _LL3461: _temp3480= _temp3478; goto _LL3463; _LL3463: for(
0; _temp3480 != 0; _temp3480=(( struct Cyc_List_List*) _check_null( _temp3480))->tl){
if( ! Cyc_Tcutil_bits_only(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp3480))->hd)->type)){ return 0;}} return 1;
_LL3465: return 0; _LL3441:;} struct _tuple20{ struct Cyc_List_List* f1; struct
Cyc_Absyn_Exp* f2; } ; static int Cyc_Tcutil_cnst_exp( struct Cyc_Tcenv_Tenv* te,
int var_okay, struct Cyc_Absyn_Exp* e){ void* _temp3492=( void*) e->r; struct
_tuple1* _temp3532; struct Cyc_Absyn_Exp* _temp3534; struct Cyc_Absyn_Exp*
_temp3536; struct Cyc_Absyn_Exp* _temp3538; struct Cyc_Absyn_Exp* _temp3540;
struct Cyc_Absyn_Exp* _temp3542; struct Cyc_Absyn_Exp* _temp3544; struct Cyc_Absyn_Exp*
_temp3546; struct Cyc_Absyn_Exp* _temp3548; void* _temp3550; struct Cyc_Absyn_Exp*
_temp3552; struct Cyc_Absyn_Exp* _temp3554; struct Cyc_Absyn_Exp* _temp3556;
struct Cyc_List_List* _temp3558; struct Cyc_List_List* _temp3560; struct Cyc_List_List*
_temp3562; struct Cyc_List_List* _temp3564; void* _temp3566; struct Cyc_List_List*
_temp3568; struct Cyc_List_List* _temp3570; _LL3494: if(*(( int*) _temp3492) ==
Cyc_Absyn_Const_e){ goto _LL3495;} else{ goto _LL3496;} _LL3496: if(*(( int*)
_temp3492) == Cyc_Absyn_Sizeoftyp_e){ goto _LL3497;} else{ goto _LL3498;}
_LL3498: if(*(( int*) _temp3492) == Cyc_Absyn_Sizeofexp_e){ goto _LL3499;} else{
goto _LL3500;} _LL3500: if(*(( int*) _temp3492) == Cyc_Absyn_Enum_e){ goto
_LL3501;} else{ goto _LL3502;} _LL3502: if(*(( int*) _temp3492) == Cyc_Absyn_Var_e){
_LL3533: _temp3532=(( struct Cyc_Absyn_Var_e_struct*) _temp3492)->f1; goto
_LL3503;} else{ goto _LL3504;} _LL3504: if(*(( int*) _temp3492) == Cyc_Absyn_Conditional_e){
_LL3539: _temp3538=(( struct Cyc_Absyn_Conditional_e_struct*) _temp3492)->f1;
goto _LL3537; _LL3537: _temp3536=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3492)->f2; goto _LL3535; _LL3535: _temp3534=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3492)->f3; goto _LL3505;} else{ goto _LL3506;} _LL3506: if(*(( int*)
_temp3492) == Cyc_Absyn_SeqExp_e){ _LL3543: _temp3542=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3492)->f1; goto _LL3541; _LL3541: _temp3540=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3492)->f2; goto _LL3507;} else{ goto _LL3508;} _LL3508: if(*(( int*)
_temp3492) == Cyc_Absyn_NoInstantiate_e){ _LL3545: _temp3544=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp3492)->f1; goto _LL3509;} else{ goto _LL3510;} _LL3510: if(*(( int*)
_temp3492) == Cyc_Absyn_Instantiate_e){ _LL3547: _temp3546=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3492)->f1; goto _LL3511;} else{ goto _LL3512;} _LL3512: if(*(( int*)
_temp3492) == Cyc_Absyn_Cast_e){ _LL3551: _temp3550=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp3492)->f1; goto _LL3549; _LL3549: _temp3548=(( struct Cyc_Absyn_Cast_e_struct*)
_temp3492)->f2; goto _LL3513;} else{ goto _LL3514;} _LL3514: if(*(( int*)
_temp3492) == Cyc_Absyn_Address_e){ _LL3553: _temp3552=(( struct Cyc_Absyn_Address_e_struct*)
_temp3492)->f1; goto _LL3515;} else{ goto _LL3516;} _LL3516: if(*(( int*)
_temp3492) == Cyc_Absyn_Comprehension_e){ _LL3557: _temp3556=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3492)->f2; goto _LL3555; _LL3555: _temp3554=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3492)->f3; goto _LL3517;} else{ goto _LL3518;} _LL3518: if(*(( int*)
_temp3492) == Cyc_Absyn_Array_e){ _LL3559: _temp3558=(( struct Cyc_Absyn_Array_e_struct*)
_temp3492)->f1; goto _LL3519;} else{ goto _LL3520;} _LL3520: if(*(( int*)
_temp3492) == Cyc_Absyn_AnonStruct_e){ _LL3561: _temp3560=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3492)->f2; goto _LL3521;} else{ goto _LL3522;} _LL3522: if(*(( int*)
_temp3492) == Cyc_Absyn_Struct_e){ _LL3563: _temp3562=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3492)->f3; goto _LL3523;} else{ goto _LL3524;} _LL3524: if(*(( int*)
_temp3492) == Cyc_Absyn_Primop_e){ _LL3567: _temp3566=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp3492)->f1; goto _LL3565; _LL3565: _temp3564=(( struct Cyc_Absyn_Primop_e_struct*)
_temp3492)->f2; goto _LL3525;} else{ goto _LL3526;} _LL3526: if(*(( int*)
_temp3492) == Cyc_Absyn_Tuple_e){ _LL3569: _temp3568=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp3492)->f1; goto _LL3527;} else{ goto _LL3528;} _LL3528: if(*(( int*)
_temp3492) == Cyc_Absyn_Tunion_e){ _LL3571: _temp3570=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3492)->f3; goto _LL3529;} else{ goto _LL3530;} _LL3530: goto _LL3531;
_LL3495: return 1; _LL3497: return 1; _LL3499: return 1; _LL3501: return 1;
_LL3503: { struct _handler_cons _temp3572; _push_handler(& _temp3572);{ int
_temp3574= 0; if( setjmp( _temp3572.handler)){ _temp3574= 1;} if( ! _temp3574){{
void* _temp3575= Cyc_Tcenv_lookup_ordinary( te, e->loc, _temp3532); void*
_temp3583; void* _temp3585; struct Cyc_Absyn_Vardecl* _temp3587; _LL3577: if(*((
int*) _temp3575) == Cyc_Tcenv_VarRes){ _LL3584: _temp3583=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3575)->f1; if(( unsigned int) _temp3583 > 1u?*(( int*) _temp3583) == Cyc_Absyn_Funname_b:
0){ goto _LL3578;} else{ goto _LL3579;}} else{ goto _LL3579;} _LL3579: if(*((
int*) _temp3575) == Cyc_Tcenv_VarRes){ _LL3586: _temp3585=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3575)->f1; if(( unsigned int) _temp3585 > 1u?*(( int*) _temp3585) == Cyc_Absyn_Global_b:
0){ _LL3588: _temp3587=(( struct Cyc_Absyn_Global_b_struct*) _temp3585)->f1;
goto _LL3580;} else{ goto _LL3581;}} else{ goto _LL3581;} _LL3581: goto _LL3582;
_LL3578: { int _temp3589= 1; _npop_handler( 0u); return _temp3589;} _LL3580: {
void* _temp3590= Cyc_Tcutil_compress(( void*) _temp3587->type); _LL3592: if((
unsigned int) _temp3590 > 4u?*(( int*) _temp3590) == Cyc_Absyn_ArrayType: 0){
goto _LL3593;} else{ goto _LL3594;} _LL3594: goto _LL3595; _LL3593: { int
_temp3596= 1; _npop_handler( 0u); return _temp3596;} _LL3595: { int _temp3597=
var_okay; _npop_handler( 0u); return _temp3597;} _LL3591:;} _LL3582: { int
_temp3598= var_okay; _npop_handler( 0u); return _temp3598;} _LL3576:;};
_pop_handler();} else{ void* _temp3573=( void*) _exn_thrown; void* _temp3600=
_temp3573; _LL3602: if( _temp3600 == Cyc_Dict_Absent){ goto _LL3603;} else{ goto
_LL3604;} _LL3604: goto _LL3605; _LL3603: return 0; _LL3605:( void) _throw(
_temp3600); _LL3601:;}}} _LL3505: return( Cyc_Tcutil_cnst_exp( te, 0, _temp3538)?
Cyc_Tcutil_cnst_exp( te, 0, _temp3536): 0)? Cyc_Tcutil_cnst_exp( te, 0,
_temp3534): 0; _LL3507: return Cyc_Tcutil_cnst_exp( te, 0, _temp3542)? Cyc_Tcutil_cnst_exp(
te, 0, _temp3540): 0; _LL3509: _temp3546= _temp3544; goto _LL3511; _LL3511:
return Cyc_Tcutil_cnst_exp( te, var_okay, _temp3546); _LL3513: return Cyc_Tcutil_cnst_exp(
te, var_okay, _temp3548); _LL3515: return Cyc_Tcutil_cnst_exp( te, 1, _temp3552);
_LL3517: return Cyc_Tcutil_cnst_exp( te, 0, _temp3556)? Cyc_Tcutil_cnst_exp( te,
0, _temp3554): 0; _LL3519: _temp3560= _temp3558; goto _LL3521; _LL3521:
_temp3562= _temp3560; goto _LL3523; _LL3523: for( 0; _temp3562 != 0; _temp3562=((
struct Cyc_List_List*) _check_null( _temp3562))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,(*(( struct _tuple20*)(( struct Cyc_List_List*) _check_null( _temp3562))->hd)).f2)){
return 0;}} return 1; _LL3525: _temp3568= _temp3564; goto _LL3527; _LL3527:
_temp3570= _temp3568; goto _LL3529; _LL3529: for( 0; _temp3570 != 0; _temp3570=((
struct Cyc_List_List*) _check_null( _temp3570))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp3570))->hd)){
return 0;}} return 1; _LL3531: return 0; _LL3493:;} int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcutil_cnst_exp(
te, 0, e);} static int Cyc_Tcutil_fields_support_default( struct Cyc_List_List*
tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs); int Cyc_Tcutil_supports_default(
void* t){ void* _temp3606= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp3644; struct Cyc_Absyn_Conref* _temp3646; struct Cyc_Absyn_Conref*
_temp3648; void* _temp3650; void* _temp3652; struct Cyc_List_List* _temp3654;
struct Cyc_Absyn_Structdecl** _temp3656; struct Cyc_List_List* _temp3658; struct
Cyc_Absyn_Uniondecl** _temp3660; struct Cyc_List_List* _temp3662; struct Cyc_List_List*
_temp3664; struct Cyc_List_List* _temp3666; _LL3608: if( _temp3606 ==( void*)
Cyc_Absyn_VoidType){ goto _LL3609;} else{ goto _LL3610;} _LL3610: if((
unsigned int) _temp3606 > 4u?*(( int*) _temp3606) == Cyc_Absyn_Evar: 0){ goto
_LL3611;} else{ goto _LL3612;} _LL3612: if(( unsigned int) _temp3606 > 4u?*((
int*) _temp3606) == Cyc_Absyn_VarType: 0){ goto _LL3613;} else{ goto _LL3614;}
_LL3614: if(( unsigned int) _temp3606 > 4u?*(( int*) _temp3606) == Cyc_Absyn_TunionType:
0){ goto _LL3615;} else{ goto _LL3616;} _LL3616: if(( unsigned int) _temp3606 >
4u?*(( int*) _temp3606) == Cyc_Absyn_TunionFieldType: 0){ goto _LL3617;} else{
goto _LL3618;} _LL3618: if(( unsigned int) _temp3606 > 4u?*(( int*) _temp3606)
== Cyc_Absyn_PointerType: 0){ _LL3645: _temp3644=(( struct Cyc_Absyn_PointerType_struct*)
_temp3606)->f1; _LL3651: _temp3650=( void*) _temp3644.elt_typ; goto _LL3649;
_LL3649: _temp3648= _temp3644.nullable; goto _LL3647; _LL3647: _temp3646=
_temp3644.bounds; goto _LL3619;} else{ goto _LL3620;} _LL3620: if(( unsigned int)
_temp3606 > 4u?*(( int*) _temp3606) == Cyc_Absyn_IntType: 0){ goto _LL3621;}
else{ goto _LL3622;} _LL3622: if( _temp3606 ==( void*) Cyc_Absyn_FloatType){
goto _LL3623;} else{ goto _LL3624;} _LL3624: if( _temp3606 ==( void*) Cyc_Absyn_DoubleType){
goto _LL3625;} else{ goto _LL3626;} _LL3626: if(( unsigned int) _temp3606 > 4u?*((
int*) _temp3606) == Cyc_Absyn_ArrayType: 0){ _LL3653: _temp3652=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3606)->f1; goto _LL3627;} else{ goto
_LL3628;} _LL3628: if(( unsigned int) _temp3606 > 4u?*(( int*) _temp3606) == Cyc_Absyn_FnType:
0){ goto _LL3629;} else{ goto _LL3630;} _LL3630: if(( unsigned int) _temp3606 >
4u?*(( int*) _temp3606) == Cyc_Absyn_TupleType: 0){ _LL3655: _temp3654=(( struct
Cyc_Absyn_TupleType_struct*) _temp3606)->f1; goto _LL3631;} else{ goto _LL3632;}
_LL3632: if(( unsigned int) _temp3606 > 4u?*(( int*) _temp3606) == Cyc_Absyn_StructType:
0){ _LL3659: _temp3658=(( struct Cyc_Absyn_StructType_struct*) _temp3606)->f2;
goto _LL3657; _LL3657: _temp3656=(( struct Cyc_Absyn_StructType_struct*)
_temp3606)->f3; goto _LL3633;} else{ goto _LL3634;} _LL3634: if(( unsigned int)
_temp3606 > 4u?*(( int*) _temp3606) == Cyc_Absyn_UnionType: 0){ _LL3663:
_temp3662=(( struct Cyc_Absyn_UnionType_struct*) _temp3606)->f2; goto _LL3661;
_LL3661: _temp3660=(( struct Cyc_Absyn_UnionType_struct*) _temp3606)->f3; goto
_LL3635;} else{ goto _LL3636;} _LL3636: if(( unsigned int) _temp3606 > 4u?*((
int*) _temp3606) == Cyc_Absyn_AnonStructType: 0){ _LL3665: _temp3664=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp3606)->f1; goto _LL3637;} else{ goto
_LL3638;} _LL3638: if(( unsigned int) _temp3606 > 4u?*(( int*) _temp3606) == Cyc_Absyn_AnonUnionType:
0){ _LL3667: _temp3666=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3606)->f1;
goto _LL3639;} else{ goto _LL3640;} _LL3640: if(( unsigned int) _temp3606 > 4u?*((
int*) _temp3606) == Cyc_Absyn_EnumType: 0){ goto _LL3641;} else{ goto _LL3642;}
_LL3642: goto _LL3643; _LL3609: return 1; _LL3611: return 0; _LL3613: return 0;
_LL3615: return 0; _LL3617: return 0; _LL3619: { void* _temp3668=( void*)( Cyc_Absyn_compress_conref(
_temp3646))->v; void* _temp3676; void* _temp3678; _LL3670: if(( unsigned int)
_temp3668 > 1u?*(( int*) _temp3668) == Cyc_Absyn_Eq_constr: 0){ _LL3677:
_temp3676=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3668)->f1; if(
_temp3676 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3671;} else{ goto _LL3672;}}
else{ goto _LL3672;} _LL3672: if(( unsigned int) _temp3668 > 1u?*(( int*)
_temp3668) == Cyc_Absyn_Eq_constr: 0){ _LL3679: _temp3678=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3668)->f1; if(( unsigned int) _temp3678 > 1u?*(( int*) _temp3678) == Cyc_Absyn_Upper_b:
0){ goto _LL3673;} else{ goto _LL3674;}} else{ goto _LL3674;} _LL3674: goto
_LL3675; _LL3671: return 1; _LL3673: { void* _temp3680=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3648))->v; int
_temp3686; _LL3682: if(( unsigned int) _temp3680 > 1u?*(( int*) _temp3680) ==
Cyc_Absyn_Eq_constr: 0){ _LL3687: _temp3686=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3680)->f1; goto _LL3683;} else{ goto _LL3684;} _LL3684: goto _LL3685;
_LL3683: return _temp3686; _LL3685: return 0; _LL3681:;} _LL3675: return 0;
_LL3669:;} _LL3621: return 1; _LL3623: return 1; _LL3625: return 1; _LL3627:
return Cyc_Tcutil_supports_default( _temp3652); _LL3629: return 0; _LL3631: for(
0; _temp3654 != 0; _temp3654=(( struct Cyc_List_List*) _check_null( _temp3654))->tl){
if( ! Cyc_Tcutil_supports_default((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp3654))->hd)).f2)){ return 0;}} return 1; _LL3633: if(
_temp3656 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp3656)); if( sd->fields == 0){ return 0;} return Cyc_Tcutil_fields_support_default(
sd->tvs, _temp3658,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
sd->fields))->v);} _LL3635: if( _temp3660 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp3660)); if( ud->fields ==
0){ return 0;} return Cyc_Tcutil_fields_support_default( ud->tvs, _temp3662,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ud->fields))->v);}
_LL3637: _temp3666= _temp3664; goto _LL3639; _LL3639: return Cyc_Tcutil_fields_support_default(
0, 0, _temp3666); _LL3641: return 1; _LL3643: return 0; _LL3607:;} static int
Cyc_Tcutil_fields_support_default( struct Cyc_List_List* tvs, struct Cyc_List_List*
ts, struct Cyc_List_List* fs){{ struct _RegionHandle _temp3688= _new_region();
struct _RegionHandle* rgn=& _temp3688; _push_region( rgn);{ struct Cyc_List_List*
_temp3689=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, tvs, ts); for( 0; fs != 0; fs=(( struct Cyc_List_List*) _check_null(
fs))->tl){ void* t= Cyc_Tcutil_rsubstitute( rgn, _temp3689,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( fs))->hd)->type); if( ! Cyc_Tcutil_supports_default(
t)){ int _temp3690= 0; _npop_handler( 0u); return _temp3690;}}}; _pop_region(
rgn);} return 1;}