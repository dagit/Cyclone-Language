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
struct _tagged_arr fmt, struct _tagged_arr); extern struct _tagged_arr Cyc_Stdio_vraprintf(
struct _RegionHandle* r1, struct _tagged_arr fmt, struct _tagged_arr ap); static
const int Cyc_Stdio_ShortPtr_sa= 0; struct Cyc_Stdio_ShortPtr_sa_struct{ int tag;
short* f1; } ; static const int Cyc_Stdio_UShortPtr_sa= 1; struct Cyc_Stdio_UShortPtr_sa_struct{
int tag; unsigned short* f1; } ; static const int Cyc_Stdio_IntPtr_sa= 2; struct
Cyc_Stdio_IntPtr_sa_struct{ int tag; int* f1; } ; static const int Cyc_Stdio_UIntPtr_sa=
3; struct Cyc_Stdio_UIntPtr_sa_struct{ int tag; unsigned int* f1; } ; static
const int Cyc_Stdio_StringPtr_sa= 4; struct Cyc_Stdio_StringPtr_sa_struct{ int
tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_DoublePtr_sa= 5;
struct Cyc_Stdio_DoublePtr_sa_struct{ int tag; double* f1; } ; static const int
Cyc_Stdio_FloatPtr_sa= 6; struct Cyc_Stdio_FloatPtr_sa_struct{ int tag; float*
f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; extern int
Cyc_List_length( struct Cyc_List_List* x); extern unsigned char Cyc_List_List_empty[
15u]; extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_rmap( struct _RegionHandle*, void*(* f)(
void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map_c(
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_List_rmap_c( struct _RegionHandle*, void*(* f)( void*, void*),
void* env, struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_map2( void*(* f)( void*, void*),
struct Cyc_List_List* x, struct Cyc_List_List* y); extern void Cyc_List_iter(
void(* f)( void*), struct Cyc_List_List* x); extern void Cyc_List_iter2( void(*
f)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List* y); extern
struct Cyc_List_List* Cyc_List_revappend( struct Cyc_List_List* x, struct Cyc_List_List*
y); extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct
Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u]; extern void* Cyc_List_nth(
struct Cyc_List_List* x, int i); extern int Cyc_List_exists_c( int(* pred)( void*,
void*), void* env, struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_zip(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List*
Cyc_List_rzip( struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List*
x, struct Cyc_List_List* y); struct _tuple0{ struct Cyc_List_List* f1; struct
Cyc_List_List* f2; } ; extern struct _tuple0 Cyc_List_rsplit( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x); extern
int Cyc_List_mem( int(* compare)( void*, void*), struct Cyc_List_List* l, void*
x); extern void* Cyc_List_assoc_cmp( int(* compare)( void*, void*), struct Cyc_List_List*
l, void* x); struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct
_tagged_arr line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; extern struct Cyc_List_List* Cyc_Position_strings_of_segments(
struct Cyc_List_List*); static const int Cyc_Position_Lex= 0; static const int
Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{
struct _tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
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
15u]; extern int Cyc_Absyn_qvar_cmp( struct _tuple1*, struct _tuple1*); extern
int Cyc_Absyn_tvar_cmp( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*); extern
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref( void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern void*
Cyc_Absyn_conref_def( void*, struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_new_evar(
struct Cyc_Core_Opt* k, struct Cyc_Core_Opt* tenv); extern void* Cyc_Absyn_uint_t;
extern void* Cyc_Absyn_ulonglong_t; extern void* Cyc_Absyn_sint_t; extern void*
Cyc_Absyn_slonglong_t; extern void* Cyc_Absyn_float_t; extern void* Cyc_Absyn_double_t;
extern struct _tuple1* Cyc_Absyn_tunion_print_arg_qvar; extern struct _tuple1*
Cyc_Absyn_tunion_scanf_arg_qvar; extern void* Cyc_Absyn_bounds_one; extern void*
Cyc_Absyn_atb_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual tq, void* b);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_field( struct Cyc_List_List*,
struct _tagged_arr*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field(
struct Cyc_Absyn_Structdecl*, struct _tagged_arr*); extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_union_field( struct Cyc_Absyn_Uniondecl*, struct _tagged_arr*);
struct _tuple4{ struct Cyc_Absyn_Tqual f1; void* f2; } ; extern struct _tuple4*
Cyc_Absyn_lookup_tuple_field( struct Cyc_List_List*, int); extern struct
_tagged_arr Cyc_Absyn_attribute2string( void*); extern int Cyc_Absyn_fntype_att(
void* a); struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc; struct
Cyc_Absynpp_Params{ int expand_typedefs: 1; int qvar_to_Cids: 1; int
add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int rewrite_temp_tvars: 1;
int print_all_tvars: 1; int print_all_kinds: 1; int print_using_stmts: 1; int
print_externC_stmts: 1; int print_full_evars: 1; int use_curr_namespace: 1;
struct Cyc_List_List* curr_namespace; } ; extern struct _tagged_arr Cyc_Absynpp_typ2string(
void*); extern struct _tagged_arr Cyc_Absynpp_kind2string( void*); extern struct
_tagged_arr Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref*); extern struct
_tagged_arr Cyc_Absynpp_qvar2string( struct _tuple1*); struct Cyc_Set_Set;
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
void* Cyc_Tcutil_impos( struct _tagged_arr fmt, struct _tagged_arr ap); extern
void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct _tagged_arr fmt,
struct _tagged_arr ap); extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap); extern void Cyc_Tcutil_flush_warnings();
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
loc, struct _tagged_arr fmt, struct _tagged_arr ap){ Cyc_Position_post_error(
Cyc_Position_mk_err_elab( loc,( struct _tagged_arr) Cyc_Stdio_vraprintf( Cyc_Core_heap_region,
fmt, ap)));} void* Cyc_Tcutil_impos( struct _tagged_arr fmt, struct _tagged_arr
ap){ struct _tagged_arr msg=( struct _tagged_arr) Cyc_Stdio_vraprintf( Cyc_Core_heap_region,
fmt, ap);({ int(* _temp0)( struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp4= Cyc_Stdio_stderr;
struct _tagged_arr _temp5= _tag_arr("Error: %s\n", sizeof( unsigned char), 11u);
struct Cyc_Stdio_String_pa_struct _temp6; _temp6.tag= Cyc_Stdio_String_pa;
_temp6.f1=( struct _tagged_arr) msg;{ void* _temp3=( void*)& _temp6; void*
_temp1[ 1u]={ _temp3}; struct _tagged_arr _temp2={( void*) _temp1,( void*)
_temp1,( void*)( _temp1 + 1u)}; _temp0( _temp4, _temp5, _temp2);}}); Cyc_Stdio_fflush((
struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( int) _throw(( void*) Cyc_Tcutil_TypeErr);}
static struct _tagged_arr Cyc_Tcutil_tvar2string( struct Cyc_Absyn_Tvar* tv){
return* tv->name;} void Cyc_Tcutil_print_tvars( struct Cyc_List_List* tvs){ for(
0; tvs != 0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){({ int(*
_temp7)( struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt, struct _tagged_arr)=
Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp12= Cyc_Stdio_stderr; struct
_tagged_arr _temp13= _tag_arr("%s::%s ", sizeof( unsigned char), 8u); struct Cyc_Stdio_String_pa_struct
_temp15; _temp15.tag= Cyc_Stdio_String_pa; _temp15.f1=( struct _tagged_arr) Cyc_Tcutil_tvar2string((
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
sg, struct _tagged_arr fmt, struct _tagged_arr ap){ struct _tagged_arr msg=(
struct _tagged_arr) Cyc_Stdio_vraprintf( Cyc_Core_heap_region, fmt, ap); Cyc_Tcutil_warning_segs=({
struct Cyc_List_List* _temp21=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp21->hd=( void*) sg; _temp21->tl= Cyc_Tcutil_warning_segs;
_temp21;}); Cyc_Tcutil_warning_msgs=({ struct Cyc_List_List* _temp22=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp22->hd=( void*)({
struct _tagged_arr* _temp23=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp23[ 0]= msg; _temp23;}); _temp22->tl= Cyc_Tcutil_warning_msgs;
_temp22;});} void Cyc_Tcutil_flush_warnings(){ if( Cyc_Tcutil_warning_segs == 0){
return;}({ int(* _temp24)( struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp27= Cyc_Stdio_stderr;
struct _tagged_arr _temp28= _tag_arr("***Warnings***\n", sizeof( unsigned char),
16u); void* _temp25[ 0u]={}; struct _tagged_arr _temp26={( void*) _temp25,( void*)
_temp25,( void*)( _temp25 + 0u)}; _temp24( _temp27, _temp28, _temp26);});{
struct Cyc_List_List* _temp29= Cyc_Position_strings_of_segments( Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs= 0; Cyc_Tcutil_warning_msgs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_Tcutil_warning_msgs); while(
Cyc_Tcutil_warning_msgs != 0) {({ int(* _temp30)( struct Cyc_Stdio___sFILE*,
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
_LL334: return({ void*(* _temp391)( struct _tagged_arr fmt, struct _tagged_arr
ap)= Cyc_Tcutil_impos; struct _tagged_arr _temp394= _tag_arr("typ_kind: Unresolved TunionFieldType",
sizeof( unsigned char), 37u); void* _temp392[ 0u]={}; struct _tagged_arr
_temp393={( void*) _temp392,( void*) _temp392,( void*)( _temp392 + 0u)};
_temp391( _temp394, _temp393);}); _LL336: return( void*) Cyc_Absyn_AnyKind;
_LL338: return( void*) Cyc_Absyn_AnyKind; _LL340: return( void*) Cyc_Absyn_MemKind;
_LL342: return( void*) Cyc_Absyn_MemKind; _LL344: return( void*) Cyc_Absyn_MemKind;
_LL346: return( void*) Cyc_Absyn_MemKind; _LL348: return( void*) Cyc_Absyn_BoxKind;
_LL350: { void* _temp395=( void*)( Cyc_Absyn_compress_conref( _temp383.bounds))->v;
void* _temp405; void* _temp407; _LL397: if(( unsigned int) _temp395 > 1u?*(( int*)
_temp395) == Cyc_Absyn_Eq_constr: 0){ _LL406: _temp405=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp395)->f1; if( _temp405 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL398;} else{
goto _LL399;}} else{ goto _LL399;} _LL399: if(( unsigned int) _temp395 > 1u?*((
int*) _temp395) == Cyc_Absyn_Eq_constr: 0){ _LL408: _temp407=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp395)->f1; if(( unsigned int) _temp407 > 1u?*((
int*) _temp407) == Cyc_Absyn_Upper_b: 0){ goto _LL400;} else{ goto _LL401;}}
else{ goto _LL401;} _LL401: if( _temp395 ==( void*) Cyc_Absyn_No_constr){ goto
_LL402;} else{ goto _LL403;} _LL403: if(( unsigned int) _temp395 > 1u?*(( int*)
_temp395) == Cyc_Absyn_Forward_constr: 0){ goto _LL404;} else{ goto _LL396;}
_LL398: return( void*) Cyc_Absyn_MemKind; _LL400: return( void*) Cyc_Absyn_BoxKind;
_LL402: return( void*) Cyc_Absyn_MemKind; _LL404: return({ void*(* _temp409)(
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_impos; struct
_tagged_arr _temp412= _tag_arr("typ_kind: forward constraint in ptr bounds",
sizeof( unsigned char), 43u); void* _temp410[ 0u]={}; struct _tagged_arr
_temp411={( void*) _temp410,( void*) _temp410,( void*)( _temp410 + 0u)};
_temp409( _temp412, _temp411);}); _LL396:;} _LL352: return( void*) Cyc_Absyn_MemKind;
_LL354: return( void*) Cyc_Absyn_MemKind; _LL356: return({ void*(* _temp413)(
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_impos; struct
_tagged_arr _temp417= _tag_arr("typ_kind: typedef found: %s", sizeof(
unsigned char), 28u); struct Cyc_Stdio_String_pa_struct _temp418; _temp418.tag=
Cyc_Stdio_String_pa; _temp418.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp416=( void*)& _temp418; void* _temp414[ 1u]={ _temp416}; struct
_tagged_arr _temp415={( void*) _temp414,( void*) _temp414,( void*)( _temp414 + 1u)};
_temp413( _temp417, _temp415);}}); _LL358: return( void*) Cyc_Absyn_EffKind;
_LL360: return( void*) Cyc_Absyn_EffKind; _LL310:;} unsigned char Cyc_Tcutil_Unify[
10u]="\000\000\000\000Unify"; int Cyc_Tcutil_unify( void* t1, void* t2){ struct
_handler_cons _temp419; _push_handler(& _temp419);{ int _temp421= 0; if( setjmp(
_temp419.handler)){ _temp421= 1;} if( ! _temp421){ Cyc_Tcutil_unify_it( t1, t2);{
int _temp422= 1; _npop_handler( 0u); return _temp422;}; _pop_handler();} else{
void* _temp420=( void*) _exn_thrown; void* _temp424= _temp420; _LL426: if(
_temp424 == Cyc_Tcutil_Unify){ goto _LL427;} else{ goto _LL428;} _LL428: goto
_LL429; _LL427: return 0; _LL429:( void) _throw( _temp424); _LL425:;}}} static
void Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List* env, struct Cyc_List_List*
ts); static void Cyc_Tcutil_occurs( void* evar, struct Cyc_List_List* env, void*
t){ void* _temp430= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar* _temp464;
struct Cyc_Core_Opt* _temp466; struct Cyc_Core_Opt** _temp468; struct Cyc_Core_Opt*
_temp469; struct Cyc_Absyn_PtrInfo _temp471; void* _temp473; struct Cyc_Absyn_FnInfo
_temp475; struct Cyc_List_List* _temp477; struct Cyc_List_List* _temp479; struct
Cyc_Absyn_VarargInfo* _temp481; int _temp483; struct Cyc_List_List* _temp485;
void* _temp487; struct Cyc_Core_Opt* _temp489; struct Cyc_List_List* _temp491;
struct Cyc_List_List* _temp493; void* _temp495; struct Cyc_Absyn_TunionInfo
_temp497; void* _temp499; struct Cyc_List_List* _temp501; struct Cyc_Core_Opt*
_temp503; struct Cyc_List_List* _temp505; struct Cyc_Absyn_TunionFieldInfo
_temp507; struct Cyc_List_List* _temp509; struct Cyc_List_List* _temp511; struct
Cyc_List_List* _temp513; struct Cyc_List_List* _temp515; void* _temp517; struct
Cyc_List_List* _temp519; _LL432: if(( unsigned int) _temp430 > 4u?*(( int*)
_temp430) == Cyc_Absyn_VarType: 0){ _LL465: _temp464=(( struct Cyc_Absyn_VarType_struct*)
_temp430)->f1; goto _LL433;} else{ goto _LL434;} _LL434: if(( unsigned int)
_temp430 > 4u?*(( int*) _temp430) == Cyc_Absyn_Evar: 0){ _LL470: _temp469=((
struct Cyc_Absyn_Evar_struct*) _temp430)->f2; goto _LL467; _LL467: _temp466=((
struct Cyc_Absyn_Evar_struct*) _temp430)->f4; _temp468=&(( struct Cyc_Absyn_Evar_struct*)
_temp430)->f4; goto _LL435;} else{ goto _LL436;} _LL436: if(( unsigned int)
_temp430 > 4u?*(( int*) _temp430) == Cyc_Absyn_PointerType: 0){ _LL472: _temp471=((
struct Cyc_Absyn_PointerType_struct*) _temp430)->f1; goto _LL437;} else{ goto
_LL438;} _LL438: if(( unsigned int) _temp430 > 4u?*(( int*) _temp430) == Cyc_Absyn_ArrayType:
0){ _LL474: _temp473=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp430)->f1;
goto _LL439;} else{ goto _LL440;} _LL440: if(( unsigned int) _temp430 > 4u?*((
int*) _temp430) == Cyc_Absyn_FnType: 0){ _LL476: _temp475=(( struct Cyc_Absyn_FnType_struct*)
_temp430)->f1; _LL492: _temp491= _temp475.tvars; goto _LL490; _LL490: _temp489=
_temp475.effect; goto _LL488; _LL488: _temp487=( void*) _temp475.ret_typ; goto
_LL486; _LL486: _temp485= _temp475.args; goto _LL484; _LL484: _temp483= _temp475.c_varargs;
goto _LL482; _LL482: _temp481= _temp475.cyc_varargs; goto _LL480; _LL480:
_temp479= _temp475.rgn_po; goto _LL478; _LL478: _temp477= _temp475.attributes;
goto _LL441;} else{ goto _LL442;} _LL442: if(( unsigned int) _temp430 > 4u?*((
int*) _temp430) == Cyc_Absyn_TupleType: 0){ _LL494: _temp493=(( struct Cyc_Absyn_TupleType_struct*)
_temp430)->f1; goto _LL443;} else{ goto _LL444;} _LL444: if(( unsigned int)
_temp430 > 4u?*(( int*) _temp430) == Cyc_Absyn_RgnHandleType: 0){ _LL496:
_temp495=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp430)->f1; goto
_LL445;} else{ goto _LL446;} _LL446: if(( unsigned int) _temp430 > 4u?*(( int*)
_temp430) == Cyc_Absyn_TunionType: 0){ _LL498: _temp497=(( struct Cyc_Absyn_TunionType_struct*)
_temp430)->f1; _LL502: _temp501= _temp497.targs; goto _LL500; _LL500: _temp499=(
void*) _temp497.rgn; goto _LL447;} else{ goto _LL448;} _LL448: if(( unsigned int)
_temp430 > 4u?*(( int*) _temp430) == Cyc_Absyn_TypedefType: 0){ _LL506: _temp505=((
struct Cyc_Absyn_TypedefType_struct*) _temp430)->f2; goto _LL504; _LL504:
_temp503=(( struct Cyc_Absyn_TypedefType_struct*) _temp430)->f3; goto _LL449;}
else{ goto _LL450;} _LL450: if(( unsigned int) _temp430 > 4u?*(( int*) _temp430)
== Cyc_Absyn_TunionFieldType: 0){ _LL508: _temp507=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp430)->f1; _LL510: _temp509= _temp507.targs; goto _LL451;} else{ goto _LL452;}
_LL452: if(( unsigned int) _temp430 > 4u?*(( int*) _temp430) == Cyc_Absyn_StructType:
0){ _LL512: _temp511=(( struct Cyc_Absyn_StructType_struct*) _temp430)->f2; goto
_LL453;} else{ goto _LL454;} _LL454: if(( unsigned int) _temp430 > 4u?*(( int*)
_temp430) == Cyc_Absyn_AnonStructType: 0){ _LL514: _temp513=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp430)->f1; goto _LL455;} else{ goto _LL456;} _LL456: if(( unsigned int)
_temp430 > 4u?*(( int*) _temp430) == Cyc_Absyn_AnonUnionType: 0){ _LL516:
_temp515=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp430)->f1; goto _LL457;}
else{ goto _LL458;} _LL458: if(( unsigned int) _temp430 > 4u?*(( int*) _temp430)
== Cyc_Absyn_AccessEff: 0){ _LL518: _temp517=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp430)->f1; goto _LL459;} else{ goto _LL460;} _LL460: if(( unsigned int)
_temp430 > 4u?*(( int*) _temp430) == Cyc_Absyn_JoinEff: 0){ _LL520: _temp519=((
struct Cyc_Absyn_JoinEff_struct*) _temp430)->f1; goto _LL461;} else{ goto _LL462;}
_LL462: goto _LL463; _LL433: if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, env, _temp464)){( int) _throw(( void*) Cyc_Tcutil_Unify);}
goto _LL431; _LL435: if( t == evar){( int) _throw(( void*) Cyc_Tcutil_Unify);}
else{ if( _temp469 != 0){ Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp469))->v);} else{ int problem= 0;{ struct Cyc_List_List* s=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(* _temp468))->v; for(
0; s != 0; s=(( struct Cyc_List_List*) _check_null( s))->tl){ if( !(( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( s))->hd)){ problem=
1; break;}}} if( problem){ struct Cyc_List_List* _temp521= 0;{ struct Cyc_List_List*
s=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(* _temp468))->v;
for( 0; s != 0; s=(( struct Cyc_List_List*) _check_null( s))->tl){ if((( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( s))->hd)){
_temp521=({ struct Cyc_List_List* _temp522=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp522->hd=( void*)(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( s))->hd); _temp522->tl= _temp521; _temp522;});}}}*
_temp468=({ struct Cyc_Core_Opt* _temp523=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp523->v=( void*) _temp521; _temp523;});}}}
goto _LL431; _LL437: Cyc_Tcutil_occurs( evar, env,( void*) _temp471.elt_typ);
Cyc_Tcutil_occurs( evar, env,( void*) _temp471.rgn_typ); goto _LL431; _LL439:
Cyc_Tcutil_occurs( evar, env, _temp473); goto _LL431; _LL441: if( _temp489 != 0){
Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp489))->v);} Cyc_Tcutil_occurs( evar, env, _temp487); for( 0; _temp485 != 0;
_temp485=(( struct Cyc_List_List*) _check_null( _temp485))->tl){ Cyc_Tcutil_occurs(
evar, env,(*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp485))->hd)).f3);}
if( _temp481 != 0){ void* _temp526; void* _temp528; struct Cyc_Absyn_VarargInfo
_temp524=*(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp481)); _LL529:
_temp528=( void*) _temp524.type; goto _LL527; _LL527: _temp526=( void*) _temp524.rgn;
goto _LL525; _LL525: Cyc_Tcutil_occurs( evar, env, _temp528); Cyc_Tcutil_occurs(
evar, env, _temp526);} for( 0; _temp479 != 0; _temp479=(( struct Cyc_List_List*)
_check_null( _temp479))->tl){ struct _tuple8 _temp532; void* _temp533; void*
_temp535; struct _tuple8* _temp530=( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( _temp479))->hd; _temp532=* _temp530; _LL536: _temp535= _temp532.f1;
goto _LL534; _LL534: _temp533= _temp532.f2; goto _LL531; _LL531: Cyc_Tcutil_occurs(
evar, env, _temp535); Cyc_Tcutil_occurs( evar, env, _temp533);} goto _LL431;
_LL443: for( 0; _temp493 != 0; _temp493=(( struct Cyc_List_List*) _check_null(
_temp493))->tl){ Cyc_Tcutil_occurs( evar, env,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp493))->hd)).f2);} goto _LL431; _LL445: Cyc_Tcutil_occurs( evar,
env, _temp495); goto _LL431; _LL447: Cyc_Tcutil_occurs( evar, env, _temp499);
Cyc_Tcutil_occurslist( evar, env, _temp501); goto _LL431; _LL449: _temp509=
_temp505; goto _LL451; _LL451: _temp511= _temp509; goto _LL453; _LL453: Cyc_Tcutil_occurslist(
evar, env, _temp511); goto _LL431; _LL455: _temp515= _temp513; goto _LL457;
_LL457: for( 0; _temp515 != 0; _temp515=(( struct Cyc_List_List*) _check_null(
_temp515))->tl){ Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp515))->hd)->type);} goto _LL431; _LL459:
Cyc_Tcutil_occurs( evar, env, _temp517); goto _LL431; _LL461: Cyc_Tcutil_occurslist(
evar, env, _temp519); goto _LL431; _LL463: goto _LL431; _LL431:;} static void
Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List* env, struct Cyc_List_List*
ts){ for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){ Cyc_Tcutil_occurs(
evar, env,( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);}} static
void Cyc_Tcutil_unify_list( struct Cyc_List_List* t1, struct Cyc_List_List* t2){
struct _handler_cons _temp537; _push_handler(& _temp537);{ int _temp539= 0; if(
setjmp( _temp537.handler)){ _temp539= 1;} if( ! _temp539){ Cyc_List_iter2( Cyc_Tcutil_unify_it,
t1, t2);; _pop_handler();} else{ void* _temp538=( void*) _exn_thrown; void*
_temp541= _temp538; _LL543: if( _temp541 == Cyc_List_List_mismatch){ goto _LL544;}
else{ goto _LL545;} _LL545: goto _LL546; _LL544:( int) _throw(( void*) Cyc_Tcutil_Unify);
_LL546:( void) _throw( _temp541); _LL542:;}}} static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2){ if(( tq1.q_const != tq2.q_const?
1: tq1.q_volatile != tq2.q_volatile)? 1: tq1.q_restrict != tq2.q_restrict){( int)
_throw(( void*) Cyc_Tcutil_Unify);}} int Cyc_Tcutil_equal_tqual( struct Cyc_Absyn_Tqual
tq1, struct Cyc_Absyn_Tqual tq2){ return( tq1.q_const == tq2.q_const? tq1.q_volatile
== tq2.q_volatile: 0)? tq1.q_restrict == tq2.q_restrict: 0;} static void Cyc_Tcutil_unify_it_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y){ x= Cyc_Absyn_compress_conref( x); y= Cyc_Absyn_compress_conref( y); if( x ==
y){ return;}{ void* _temp547=( void*) x->v; void* _temp555; _LL549: if( _temp547
==( void*) Cyc_Absyn_No_constr){ goto _LL550;} else{ goto _LL551;} _LL551: if((
unsigned int) _temp547 > 1u?*(( int*) _temp547) == Cyc_Absyn_Forward_constr: 0){
goto _LL552;} else{ goto _LL553;} _LL553: if(( unsigned int) _temp547 > 1u?*((
int*) _temp547) == Cyc_Absyn_Eq_constr: 0){ _LL556: _temp555=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp547)->f1; goto _LL554;} else{ goto _LL548;}
_LL550:( void*)( x->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp557=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp557[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp558; _temp558.tag= Cyc_Absyn_Forward_constr; _temp558.f1= y; _temp558;});
_temp557;}))); return; _LL552:( int) _throw(({ void*(* _temp559)( struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_impos; struct _tagged_arr
_temp562= _tag_arr("unify_conref: forward after compress", sizeof( unsigned char),
37u); void* _temp560[ 0u]={}; struct _tagged_arr _temp561={( void*) _temp560,(
void*) _temp560,( void*)( _temp560 + 0u)}; _temp559( _temp562, _temp561);}));
_LL554: { void* _temp563=( void*) y->v; void* _temp571; _LL565: if( _temp563 ==(
void*) Cyc_Absyn_No_constr){ goto _LL566;} else{ goto _LL567;} _LL567: if((
unsigned int) _temp563 > 1u?*(( int*) _temp563) == Cyc_Absyn_Forward_constr: 0){
goto _LL568;} else{ goto _LL569;} _LL569: if(( unsigned int) _temp563 > 1u?*((
int*) _temp563) == Cyc_Absyn_Eq_constr: 0){ _LL572: _temp571=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp563)->f1; goto _LL570;} else{ goto _LL564;}
_LL566:( void*)( y->v=( void*)(( void*) x->v)); return; _LL568:( int) _throw(({
void*(* _temp573)( struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_impos;
struct _tagged_arr _temp576= _tag_arr("unify_conref: forward after compress(2)",
sizeof( unsigned char), 40u); void* _temp574[ 0u]={}; struct _tagged_arr
_temp575={( void*) _temp574,( void*) _temp574,( void*)( _temp574 + 0u)};
_temp573( _temp576, _temp575);})); _LL570: if( cmp( _temp555, _temp571) != 0){(
int) _throw(( void*) Cyc_Tcutil_Unify);} return; _LL564:;} _LL548:;}} static int
Cyc_Tcutil_unify_conrefs( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y){ struct _handler_cons _temp577; _push_handler(&
_temp577);{ int _temp579= 0; if( setjmp( _temp577.handler)){ _temp579= 1;} if( !
_temp579){ Cyc_Tcutil_unify_it_conrefs( cmp, x, y);{ int _temp580= 1;
_npop_handler( 0u); return _temp580;}; _pop_handler();} else{ void* _temp578=(
void*) _exn_thrown; void* _temp582= _temp578; _LL584: if( _temp582 == Cyc_Tcutil_Unify){
goto _LL585;} else{ goto _LL586;} _LL586: goto _LL587; _LL585: return 0; _LL587:(
void) _throw( _temp582); _LL583:;}}} static int Cyc_Tcutil_boundscmp( void* b1,
void* b2){ struct _tuple8 _temp589=({ struct _tuple8 _temp588; _temp588.f1= b1;
_temp588.f2= b2; _temp588;}); void* _temp599; void* _temp601; void* _temp603;
void* _temp605; void* _temp607; struct Cyc_Absyn_Exp* _temp609; void* _temp611;
struct Cyc_Absyn_Exp* _temp613; _LL591: _LL602: _temp601= _temp589.f1; if(
_temp601 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL600;} else{ goto _LL593;}
_LL600: _temp599= _temp589.f2; if( _temp599 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL592;} else{ goto _LL593;} _LL593: _LL604: _temp603= _temp589.f1; if(
_temp603 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL594;} else{ goto _LL595;}
_LL595: _LL606: _temp605= _temp589.f2; if( _temp605 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL596;} else{ goto _LL597;} _LL597: _LL612: _temp611= _temp589.f1; if((
unsigned int) _temp611 > 1u?*(( int*) _temp611) == Cyc_Absyn_Upper_b: 0){ _LL614:
_temp613=(( struct Cyc_Absyn_Upper_b_struct*) _temp611)->f1; goto _LL608;} else{
goto _LL590;} _LL608: _temp607= _temp589.f2; if(( unsigned int) _temp607 > 1u?*((
int*) _temp607) == Cyc_Absyn_Upper_b: 0){ _LL610: _temp609=(( struct Cyc_Absyn_Upper_b_struct*)
_temp607)->f1; goto _LL598;} else{ goto _LL590;} _LL592: return 0; _LL594:
return - 1; _LL596: return 1; _LL598: { int i1=( int) Cyc_Evexp_eval_const_uint_exp(
_temp613); int i2=( int) Cyc_Evexp_eval_const_uint_exp( _temp609); if( i1 == i2){
return 0;} if( i1 < i2){ return - 1;} return 1;} _LL590:;} static int Cyc_Tcutil_equal_att(
void* a1, void* a2){ if( a1 == a2){ return 1;}{ struct _tuple8 _temp616=({
struct _tuple8 _temp615; _temp615.f1= a1; _temp615.f2= a2; _temp615;}); void*
_temp628; int _temp630; int _temp632; void* _temp634; void* _temp636; int
_temp638; int _temp640; void* _temp642; void* _temp644; int _temp646; void*
_temp648; int _temp650; void* _temp652; int _temp654; void* _temp656; int
_temp658; void* _temp660; struct _tagged_arr _temp662; void* _temp664; struct
_tagged_arr _temp666; _LL618: _LL637: _temp636= _temp616.f1; if(( unsigned int)
_temp636 > 16u?*(( int*) _temp636) == Cyc_Absyn_Format_att: 0){ _LL643: _temp642=(
void*)(( struct Cyc_Absyn_Format_att_struct*) _temp636)->f1; goto _LL641; _LL641:
_temp640=(( struct Cyc_Absyn_Format_att_struct*) _temp636)->f2; goto _LL639;
_LL639: _temp638=(( struct Cyc_Absyn_Format_att_struct*) _temp636)->f3; goto
_LL629;} else{ goto _LL620;} _LL629: _temp628= _temp616.f2; if(( unsigned int)
_temp628 > 16u?*(( int*) _temp628) == Cyc_Absyn_Format_att: 0){ _LL635: _temp634=(
void*)(( struct Cyc_Absyn_Format_att_struct*) _temp628)->f1; goto _LL633; _LL633:
_temp632=(( struct Cyc_Absyn_Format_att_struct*) _temp628)->f2; goto _LL631;
_LL631: _temp630=(( struct Cyc_Absyn_Format_att_struct*) _temp628)->f3; goto
_LL619;} else{ goto _LL620;} _LL620: _LL649: _temp648= _temp616.f1; if((
unsigned int) _temp648 > 16u?*(( int*) _temp648) == Cyc_Absyn_Regparm_att: 0){
_LL651: _temp650=(( struct Cyc_Absyn_Regparm_att_struct*) _temp648)->f1; goto
_LL645;} else{ goto _LL622;} _LL645: _temp644= _temp616.f2; if(( unsigned int)
_temp644 > 16u?*(( int*) _temp644) == Cyc_Absyn_Regparm_att: 0){ _LL647:
_temp646=(( struct Cyc_Absyn_Regparm_att_struct*) _temp644)->f1; goto _LL621;}
else{ goto _LL622;} _LL622: _LL657: _temp656= _temp616.f1; if(( unsigned int)
_temp656 > 16u?*(( int*) _temp656) == Cyc_Absyn_Aligned_att: 0){ _LL659:
_temp658=(( struct Cyc_Absyn_Aligned_att_struct*) _temp656)->f1; goto _LL653;}
else{ goto _LL624;} _LL653: _temp652= _temp616.f2; if(( unsigned int) _temp652 >
16u?*(( int*) _temp652) == Cyc_Absyn_Aligned_att: 0){ _LL655: _temp654=(( struct
Cyc_Absyn_Aligned_att_struct*) _temp652)->f1; goto _LL623;} else{ goto _LL624;}
_LL624: _LL665: _temp664= _temp616.f1; if(( unsigned int) _temp664 > 16u?*(( int*)
_temp664) == Cyc_Absyn_Section_att: 0){ _LL667: _temp666=(( struct Cyc_Absyn_Section_att_struct*)
_temp664)->f1; goto _LL661;} else{ goto _LL626;} _LL661: _temp660= _temp616.f2;
if(( unsigned int) _temp660 > 16u?*(( int*) _temp660) == Cyc_Absyn_Section_att:
0){ _LL663: _temp662=(( struct Cyc_Absyn_Section_att_struct*) _temp660)->f1;
goto _LL625;} else{ goto _LL626;} _LL626: goto _LL627; _LL619: return( _temp642
== _temp634? _temp640 == _temp632: 0)? _temp638 == _temp630: 0; _LL621: _temp658=
_temp650; _temp654= _temp646; goto _LL623; _LL623: return _temp658 == _temp654;
_LL625: return Cyc_String_strcmp( _temp666, _temp662) == 0; _LL627: return 0;
_LL617:;}} int Cyc_Tcutil_same_atts( struct Cyc_List_List* a1, struct Cyc_List_List*
a2){{ struct Cyc_List_List* a= a1; for( 0; a != 0; a=(( struct Cyc_List_List*)
_check_null( a))->tl){ if( ! Cyc_List_exists_c( Cyc_Tcutil_equal_att,( void*)((
struct Cyc_List_List*) _check_null( a))->hd, a2)){ return 0;}}}{ struct Cyc_List_List*
a= a2; for( 0; a != 0; a=(( struct Cyc_List_List*) _check_null( a))->tl){ if( !
Cyc_List_exists_c( Cyc_Tcutil_equal_att,( void*)(( struct Cyc_List_List*)
_check_null( a))->hd, a1)){ return 0;}}} return 1;} static void Cyc_Tcutil_normalize_effect(
void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp668= e; struct Cyc_List_List*
_temp674; struct Cyc_List_List** _temp676; _LL670: if(( unsigned int) _temp668 >
4u?*(( int*) _temp668) == Cyc_Absyn_JoinEff: 0){ _LL675: _temp674=(( struct Cyc_Absyn_JoinEff_struct*)
_temp668)->f1; _temp676=&(( struct Cyc_Absyn_JoinEff_struct*) _temp668)->f1;
goto _LL671;} else{ goto _LL672;} _LL672: goto _LL673; _LL671: { int nested_join=
0;{ struct Cyc_List_List* effs=* _temp676; for( 0; effs != 0; effs=(( struct Cyc_List_List*)
_check_null( effs))->tl){ void* _temp677=( void*)(( struct Cyc_List_List*)
_check_null( effs))->hd; Cyc_Tcutil_normalize_effect( _temp677);{ void* _temp678=
Cyc_Tcutil_compress( _temp677); _LL680: if(( unsigned int) _temp678 > 4u?*(( int*)
_temp678) == Cyc_Absyn_JoinEff: 0){ goto _LL681;} else{ goto _LL682;} _LL682:
goto _LL683; _LL681: nested_join= 1; goto _LL679; _LL683: goto _LL679; _LL679:;}}}
if( ! nested_join){ return;}{ struct Cyc_List_List* effects= 0;{ struct Cyc_List_List*
effs=* _temp676; for( 0; effs != 0; effs=(( struct Cyc_List_List*) _check_null(
effs))->tl){ void* _temp684= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*)
_check_null( effs))->hd); struct Cyc_List_List* _temp690; _LL686: if((
unsigned int) _temp684 > 4u?*(( int*) _temp684) == Cyc_Absyn_JoinEff: 0){ _LL691:
_temp690=(( struct Cyc_Absyn_JoinEff_struct*) _temp684)->f1; goto _LL687;} else{
goto _LL688;} _LL688: goto _LL689; _LL687: effects= Cyc_List_revappend( _temp690,
effects); goto _LL685; _LL689: effects=({ struct Cyc_List_List* _temp692=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp692->hd=(
void*) _temp684; _temp692->tl= effects; _temp692;}); goto _LL685; _LL685:;}}*
_temp676= Cyc_List_imp_rev( effects); goto _LL669;}} _LL673: goto _LL669; _LL669:;}}
struct _tuple9{ void* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; }
; static struct _tuple9* Cyc_Tcutil_get_effect_evar( void* t){ void* _temp693=
Cyc_Tcutil_compress( t); struct Cyc_List_List* _temp701; struct Cyc_List_List
_temp703; struct Cyc_List_List* _temp704; void* _temp706; struct Cyc_Core_Opt*
_temp708; struct Cyc_Core_Opt* _temp710; _LL695: if(( unsigned int) _temp693 > 4u?*((
int*) _temp693) == Cyc_Absyn_JoinEff: 0){ _LL702: _temp701=(( struct Cyc_Absyn_JoinEff_struct*)
_temp693)->f1; if( _temp701 == 0){ goto _LL697;} else{ _temp703=* _temp701;
_LL707: _temp706=( void*) _temp703.hd; goto _LL705; _LL705: _temp704= _temp703.tl;
goto _LL696;}} else{ goto _LL697;} _LL697: if(( unsigned int) _temp693 > 4u?*((
int*) _temp693) == Cyc_Absyn_Evar: 0){ _LL711: _temp710=(( struct Cyc_Absyn_Evar_struct*)
_temp693)->f1; goto _LL709; _LL709: _temp708=(( struct Cyc_Absyn_Evar_struct*)
_temp693)->f4; goto _LL698;} else{ goto _LL699;} _LL699: goto _LL700; _LL696: {
void* _temp712= Cyc_Tcutil_compress( _temp706); struct Cyc_Core_Opt* _temp718;
_LL714: if(( unsigned int) _temp712 > 4u?*(( int*) _temp712) == Cyc_Absyn_Evar:
0){ _LL719: _temp718=(( struct Cyc_Absyn_Evar_struct*) _temp712)->f4; goto
_LL715;} else{ goto _LL716;} _LL716: goto _LL717; _LL715: return({ struct
_tuple9* _temp720=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp720->f1= _temp706; _temp720->f2= _temp704; _temp720->f3=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp718))->v; _temp720;}); _LL717: return 0;
_LL713:;} _LL698: if( _temp710 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp710))->v !=( void*) Cyc_Absyn_EffKind){({ int(* _temp721)(
struct _tagged_arr fmt, struct _tagged_arr ap)=( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Tcutil_impos; struct _tagged_arr _temp724= _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u); void* _temp722[ 0u]={}; struct _tagged_arr
_temp723={( void*) _temp722,( void*) _temp722,( void*)( _temp722 + 0u)};
_temp721( _temp724, _temp723);});} return({ struct _tuple9* _temp725=( struct
_tuple9*) GC_malloc( sizeof( struct _tuple9)); _temp725->f1= t; _temp725->f2= 0;
_temp725->f3=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp708))->v; _temp725;}); _LL700: return 0; _LL694:;} static struct Cyc_Core_Opt
Cyc_Tcutil_ek={( void*)(( void*) 4u)}; int Cyc_Tcutil_region_in_effect( int
constrain, void* r, void* e){ r= Cyc_Tcutil_compress( r); if( r ==( void*) Cyc_Absyn_HeapRgn){
return 1;}{ void* _temp726= Cyc_Tcutil_compress( e); void* _temp736; struct Cyc_List_List*
_temp738; struct Cyc_Core_Opt* _temp740; struct Cyc_Core_Opt* _temp742; struct
Cyc_Core_Opt** _temp744; struct Cyc_Core_Opt* _temp745; _LL728: if((
unsigned int) _temp726 > 4u?*(( int*) _temp726) == Cyc_Absyn_AccessEff: 0){
_LL737: _temp736=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp726)->f1;
goto _LL729;} else{ goto _LL730;} _LL730: if(( unsigned int) _temp726 > 4u?*((
int*) _temp726) == Cyc_Absyn_JoinEff: 0){ _LL739: _temp738=(( struct Cyc_Absyn_JoinEff_struct*)
_temp726)->f1; goto _LL731;} else{ goto _LL732;} _LL732: if(( unsigned int)
_temp726 > 4u?*(( int*) _temp726) == Cyc_Absyn_Evar: 0){ _LL746: _temp745=((
struct Cyc_Absyn_Evar_struct*) _temp726)->f1; goto _LL743; _LL743: _temp742=((
struct Cyc_Absyn_Evar_struct*) _temp726)->f2; _temp744=&(( struct Cyc_Absyn_Evar_struct*)
_temp726)->f2; goto _LL741; _LL741: _temp740=(( struct Cyc_Absyn_Evar_struct*)
_temp726)->f4; goto _LL733;} else{ goto _LL734;} _LL734: goto _LL735; _LL729:
if( constrain){ return Cyc_Tcutil_unify( r, _temp736);} Cyc_Tcutil_compress(
_temp736); if( r == _temp736){ return 1;}{ struct _tuple8 _temp748=({ struct
_tuple8 _temp747; _temp747.f1= r; _temp747.f2= _temp736; _temp747;}); void*
_temp754; struct Cyc_Absyn_Tvar* _temp756; void* _temp758; struct Cyc_Absyn_Tvar*
_temp760; _LL750: _LL759: _temp758= _temp748.f1; if(( unsigned int) _temp758 > 4u?*((
int*) _temp758) == Cyc_Absyn_VarType: 0){ _LL761: _temp760=(( struct Cyc_Absyn_VarType_struct*)
_temp758)->f1; goto _LL755;} else{ goto _LL752;} _LL755: _temp754= _temp748.f2;
if(( unsigned int) _temp754 > 4u?*(( int*) _temp754) == Cyc_Absyn_VarType: 0){
_LL757: _temp756=(( struct Cyc_Absyn_VarType_struct*) _temp754)->f1; goto _LL751;}
else{ goto _LL752;} _LL752: goto _LL753; _LL751: return Cyc_Absyn_tvar_cmp(
_temp760, _temp756) == 0; _LL753: return 0; _LL749:;} _LL731: for( 0; _temp738
!= 0; _temp738=(( struct Cyc_List_List*) _check_null( _temp738))->tl){ if( Cyc_Tcutil_region_in_effect(
constrain, r,( void*)(( struct Cyc_List_List*) _check_null( _temp738))->hd)){
return 1;}} return 0; _LL733: if( _temp745 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp745))->v !=( void*) Cyc_Absyn_EffKind){({ int(* _temp762)(
struct _tagged_arr fmt, struct _tagged_arr ap)=( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Tcutil_impos; struct _tagged_arr _temp765= _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u); void* _temp763[ 0u]={}; struct _tagged_arr
_temp764={( void*) _temp763,( void*) _temp763,( void*)( _temp763 + 0u)};
_temp762( _temp765, _temp764);});} if( ! constrain){ return 0;}{ void* _temp766=
Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek, _temp740); Cyc_Tcutil_occurs(
_temp766,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp740))->v,
r);{ struct Cyc_Absyn_JoinEff_struct* _temp767=({ struct Cyc_Absyn_JoinEff_struct*
_temp769=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp769[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp770; _temp770.tag= Cyc_Absyn_JoinEff;
_temp770.f1=({ struct Cyc_List_List* _temp771=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp771->hd=( void*) _temp766; _temp771->tl=({
struct Cyc_List_List* _temp772=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp772->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp773=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp773[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp774; _temp774.tag= Cyc_Absyn_AccessEff;
_temp774.f1=( void*) r; _temp774;}); _temp773;})); _temp772->tl= 0; _temp772;});
_temp771;}); _temp770;}); _temp769;});* _temp744=({ struct Cyc_Core_Opt*
_temp768=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp768->v=( void*)(( void*) _temp767); _temp768;}); return 1;}} _LL735: return
0; _LL727:;}} static int Cyc_Tcutil_variable_in_effect( int constrain, struct
Cyc_Absyn_Tvar* v, void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp775= e;
struct Cyc_Absyn_Tvar* _temp785; struct Cyc_List_List* _temp787; struct Cyc_Core_Opt*
_temp789; struct Cyc_Core_Opt* _temp791; struct Cyc_Core_Opt** _temp793; struct
Cyc_Core_Opt* _temp794; _LL777: if(( unsigned int) _temp775 > 4u?*(( int*)
_temp775) == Cyc_Absyn_VarType: 0){ _LL786: _temp785=(( struct Cyc_Absyn_VarType_struct*)
_temp775)->f1; goto _LL778;} else{ goto _LL779;} _LL779: if(( unsigned int)
_temp775 > 4u?*(( int*) _temp775) == Cyc_Absyn_JoinEff: 0){ _LL788: _temp787=((
struct Cyc_Absyn_JoinEff_struct*) _temp775)->f1; goto _LL780;} else{ goto _LL781;}
_LL781: if(( unsigned int) _temp775 > 4u?*(( int*) _temp775) == Cyc_Absyn_Evar:
0){ _LL795: _temp794=(( struct Cyc_Absyn_Evar_struct*) _temp775)->f1; goto
_LL792; _LL792: _temp791=(( struct Cyc_Absyn_Evar_struct*) _temp775)->f2;
_temp793=&(( struct Cyc_Absyn_Evar_struct*) _temp775)->f2; goto _LL790; _LL790:
_temp789=(( struct Cyc_Absyn_Evar_struct*) _temp775)->f4; goto _LL782;} else{
goto _LL783;} _LL783: goto _LL784; _LL778: return Cyc_Absyn_tvar_cmp( v,
_temp785) == 0; _LL780: for( 0; _temp787 != 0; _temp787=(( struct Cyc_List_List*)
_check_null( _temp787))->tl){ if( Cyc_Tcutil_variable_in_effect( constrain, v,(
void*)(( struct Cyc_List_List*) _check_null( _temp787))->hd)){ return 1;}}
return 0; _LL782: if( _temp794 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp794))->v !=( void*) Cyc_Absyn_EffKind){({ int(* _temp796)(
struct _tagged_arr fmt, struct _tagged_arr ap)=( int(*)( struct _tagged_arr fmt,
struct _tagged_arr ap)) Cyc_Tcutil_impos; struct _tagged_arr _temp799= _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u); void* _temp797[ 0u]={}; struct _tagged_arr
_temp798={( void*) _temp797,( void*) _temp797,( void*)( _temp797 + 0u)};
_temp796( _temp799, _temp798);});}{ void* _temp800= Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Tcutil_ek, _temp789); if( !(( int(*)( int(* compare)( struct
Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar*
x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( _temp789))->v, v)){ return 0;}{ struct Cyc_Absyn_JoinEff_struct*
_temp801=({ struct Cyc_Absyn_JoinEff_struct* _temp803=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp803[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp804; _temp804.tag= Cyc_Absyn_JoinEff; _temp804.f1=({ struct Cyc_List_List*
_temp805=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp805->hd=( void*) _temp800; _temp805->tl=({ struct Cyc_List_List* _temp806=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp806->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp807=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp807[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp808; _temp808.tag= Cyc_Absyn_VarType; _temp808.f1= v; _temp808;}); _temp807;}));
_temp806->tl= 0; _temp806;}); _temp805;}); _temp804;}); _temp803;});* _temp793=({
struct Cyc_Core_Opt* _temp802=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp802->v=( void*)(( void*) _temp801); _temp802;}); return 1;}}
_LL784: return 0; _LL776:;}} static int Cyc_Tcutil_evar_in_effect( void* evar,
void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp809= e; struct Cyc_List_List*
_temp817; _LL811: if(( unsigned int) _temp809 > 4u?*(( int*) _temp809) == Cyc_Absyn_JoinEff:
0){ _LL818: _temp817=(( struct Cyc_Absyn_JoinEff_struct*) _temp809)->f1; goto
_LL812;} else{ goto _LL813;} _LL813: if(( unsigned int) _temp809 > 4u?*(( int*)
_temp809) == Cyc_Absyn_Evar: 0){ goto _LL814;} else{ goto _LL815;} _LL815: goto
_LL816; _LL812: for( 0; _temp817 != 0; _temp817=(( struct Cyc_List_List*)
_check_null( _temp817))->tl){ if( Cyc_Tcutil_evar_in_effect( evar,( void*)((
struct Cyc_List_List*) _check_null( _temp817))->hd)){ return 1;}} return 0;
_LL814: return evar == e; _LL816: return 0; _LL810:;}} int Cyc_Tcutil_subset_effect(
int set_to_empty, void* e1, void* e2){{ struct _tuple9* _temp819= Cyc_Tcutil_get_effect_evar(
e2); struct _tuple9 _temp825; struct Cyc_List_List* _temp826; struct Cyc_List_List*
_temp828; void* _temp830; _LL821: if( _temp819 == 0){ goto _LL823;} else{
_temp825=* _temp819; _LL831: _temp830= _temp825.f1; goto _LL829; _LL829:
_temp828= _temp825.f2; goto _LL827; _LL827: _temp826= _temp825.f3; goto _LL822;}
_LL823: goto _LL824; _LL822: { void* _temp832= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,({ struct Cyc_Core_Opt* _temp838=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp838->v=( void*) _temp826; _temp838;}));
struct Cyc_Absyn_JoinEff_struct* _temp833=({ struct Cyc_Absyn_JoinEff_struct*
_temp834=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp834[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp835; _temp835.tag= Cyc_Absyn_JoinEff;
_temp835.f1=({ struct Cyc_List_List* _temp836=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp836->hd=( void*) _temp832; _temp836->tl=({
struct Cyc_List_List* _temp837=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp837->hd=( void*) e1; _temp837->tl= 0; _temp837;});
_temp836;}); _temp835;}); _temp834;}); return Cyc_Tcutil_unify( _temp830,( void*)
_temp833);} _LL824: goto _LL820; _LL820:;}{ void* _temp839= Cyc_Tcutil_compress(
e1); struct Cyc_List_List* _temp851; void* _temp853; struct Cyc_Absyn_Tvar*
_temp855; struct Cyc_Core_Opt* _temp857; struct Cyc_Core_Opt* _temp859; struct
Cyc_Core_Opt** _temp861; _LL841: if(( unsigned int) _temp839 > 4u?*(( int*)
_temp839) == Cyc_Absyn_JoinEff: 0){ _LL852: _temp851=(( struct Cyc_Absyn_JoinEff_struct*)
_temp839)->f1; goto _LL842;} else{ goto _LL843;} _LL843: if(( unsigned int)
_temp839 > 4u?*(( int*) _temp839) == Cyc_Absyn_AccessEff: 0){ _LL854: _temp853=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp839)->f1; goto _LL844;} else{
goto _LL845;} _LL845: if(( unsigned int) _temp839 > 4u?*(( int*) _temp839) ==
Cyc_Absyn_VarType: 0){ _LL856: _temp855=(( struct Cyc_Absyn_VarType_struct*)
_temp839)->f1; goto _LL846;} else{ goto _LL847;} _LL847: if(( unsigned int)
_temp839 > 4u?*(( int*) _temp839) == Cyc_Absyn_Evar: 0){ _LL860: _temp859=((
struct Cyc_Absyn_Evar_struct*) _temp839)->f2; _temp861=&(( struct Cyc_Absyn_Evar_struct*)
_temp839)->f2; goto _LL858; _LL858: _temp857=(( struct Cyc_Absyn_Evar_struct*)
_temp839)->f4; goto _LL848;} else{ goto _LL849;} _LL849: goto _LL850; _LL842:
for( 0; _temp851 != 0; _temp851=(( struct Cyc_List_List*) _check_null( _temp851))->tl){
if( ! Cyc_Tcutil_subset_effect( set_to_empty,( void*)(( struct Cyc_List_List*)
_check_null( _temp851))->hd, e2)){ return 0;}} return 1; _LL844: return( Cyc_Tcutil_region_in_effect(
0, _temp853, e2)? 1: Cyc_Tcutil_region_in_effect( 1, _temp853, e2))? 1: Cyc_Tcutil_unify(
_temp853,( void*) Cyc_Absyn_HeapRgn); _LL846: return Cyc_Tcutil_variable_in_effect(
0, _temp855, e2)? 1: Cyc_Tcutil_variable_in_effect( 1, _temp855, e2); _LL848:
if( ! Cyc_Tcutil_evar_in_effect( e1, e2)){ if( set_to_empty){* _temp861=({
struct Cyc_Core_Opt* _temp862=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp862->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp863=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp863[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp864; _temp864.tag= Cyc_Absyn_JoinEff;
_temp864.f1= 0; _temp864;}); _temp863;})); _temp862;});} else{ Cyc_Tcutil_occurs(
e1,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp857))->v,
e2);* _temp861=({ struct Cyc_Core_Opt* _temp865=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp865->v=( void*) e2; _temp865;});}}
return 1; _LL850: return({ int(* _temp866)( struct _tagged_arr fmt, struct
_tagged_arr ap)=( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos;
struct _tagged_arr _temp870= _tag_arr("subset_effect: bad effect: %s", sizeof(
unsigned char), 30u); struct Cyc_Stdio_String_pa_struct _temp871; _temp871.tag=
Cyc_Stdio_String_pa; _temp871.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
e1);{ void* _temp869=( void*)& _temp871; void* _temp867[ 1u]={ _temp869}; struct
_tagged_arr _temp868={( void*) _temp867,( void*) _temp867,( void*)( _temp867 + 1u)};
_temp866( _temp870, _temp868);}}); _LL840:;}} struct _tuple10{ struct _tuple9*
f1; struct _tuple9* f2; } ; static int Cyc_Tcutil_unify_effect( void* e1, void*
e2){ e1= Cyc_Tcutil_compress( e1); e2= Cyc_Tcutil_compress( e2); Cyc_Tcutil_normalize_effect(
e1); Cyc_Tcutil_normalize_effect( e2);{ struct _tuple10 _temp873=({ struct
_tuple10 _temp872; _temp872.f1= Cyc_Tcutil_get_effect_evar( e1); _temp872.f2=
Cyc_Tcutil_get_effect_evar( e2); _temp872;}); struct _tuple9* _temp879; struct
_tuple9 _temp881; struct Cyc_List_List* _temp882; struct Cyc_List_List* _temp884;
void* _temp886; struct _tuple9* _temp888; struct _tuple9 _temp890; struct Cyc_List_List*
_temp891; struct Cyc_List_List* _temp893; void* _temp895; _LL875: _LL889:
_temp888= _temp873.f1; if( _temp888 == 0){ goto _LL877;} else{ _temp890=*
_temp888; _LL896: _temp895= _temp890.f1; goto _LL894; _LL894: _temp893= _temp890.f2;
goto _LL892; _LL892: _temp891= _temp890.f3; goto _LL880;} _LL880: _temp879=
_temp873.f2; if( _temp879 == 0){ goto _LL877;} else{ _temp881=* _temp879; _LL887:
_temp886= _temp881.f1; goto _LL885; _LL885: _temp884= _temp881.f2; goto _LL883;
_LL883: _temp882= _temp881.f3; goto _LL876;} _LL877: goto _LL878; _LL876: {
struct Cyc_List_List* _temp897= 0; for( 0; _temp891 != 0; _temp891=(( struct Cyc_List_List*)
_check_null( _temp891))->tl){ if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, _temp882,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp891))->hd)){ _temp897=({ struct Cyc_List_List* _temp898=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp898->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp891))->hd);
_temp898->tl= _temp897; _temp898;});}}{ void* _temp899= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,({ struct Cyc_Core_Opt* _temp904=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp904->v=( void*)
_temp897; _temp904;})); struct Cyc_Absyn_JoinEff_struct* _temp900=({ struct Cyc_Absyn_JoinEff_struct*
_temp901=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp901[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp902; _temp902.tag= Cyc_Absyn_JoinEff;
_temp902.f1=({ struct Cyc_List_List* _temp903=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp903->hd=( void*) _temp899; _temp903->tl=
Cyc_List_append( _temp893, _temp884); _temp903;}); _temp902;}); _temp901;});
return Cyc_Tcutil_unify( _temp895,( void*) _temp900)? Cyc_Tcutil_unify( _temp886,(
void*) _temp900): 0;}} _LL878: return Cyc_Tcutil_subset_effect( 0, e1, e2)? Cyc_Tcutil_subset_effect(
0, e2, e1): 0; _LL874:;}} static int Cyc_Tcutil_sub_rgnpo( struct Cyc_List_List*
rpo1, struct Cyc_List_List* rpo2){{ struct Cyc_List_List* r1= rpo1; for( 0; r1
!= 0; r1=(( struct Cyc_List_List*) _check_null( r1))->tl){ struct _tuple8
_temp907; void* _temp908; void* _temp910; struct _tuple8* _temp905=( struct
_tuple8*)(( struct Cyc_List_List*) _check_null( r1))->hd; _temp907=* _temp905;
_LL911: _temp910= _temp907.f1; goto _LL909; _LL909: _temp908= _temp907.f2; goto
_LL906; _LL906: { int found= _temp910 ==( void*) Cyc_Absyn_HeapRgn;{ struct Cyc_List_List*
r2= rpo2; for( 0; r2 != 0? ! found: 0; r2=(( struct Cyc_List_List*) _check_null(
r2))->tl){ struct _tuple8 _temp914; void* _temp915; void* _temp917; struct
_tuple8* _temp912=( struct _tuple8*)(( struct Cyc_List_List*) _check_null( r2))->hd;
_temp914=* _temp912; _LL918: _temp917= _temp914.f1; goto _LL916; _LL916:
_temp915= _temp914.f2; goto _LL913; _LL913: if( Cyc_Tcutil_unify( _temp910,
_temp917)? Cyc_Tcutil_unify( _temp908, _temp915): 0){ found= 1; break;}}} if( !
found){ return 0;}}}} return 1;} static int Cyc_Tcutil_same_rgn_po( struct Cyc_List_List*
rpo1, struct Cyc_List_List* rpo2){ return Cyc_Tcutil_sub_rgnpo( rpo1, rpo2)? Cyc_Tcutil_sub_rgnpo(
rpo2, rpo1): 0;} struct _tuple11{ struct Cyc_Absyn_VarargInfo* f1; struct Cyc_Absyn_VarargInfo*
f2; } ; struct _tuple12{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ;
void Cyc_Tcutil_unify_it( void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2=
Cyc_Tcutil_compress( t2); if( t1 == t2){ return;}{ void* _temp919= t1; struct
Cyc_Core_Opt* _temp925; struct Cyc_Core_Opt* _temp927; struct Cyc_Core_Opt**
_temp929; struct Cyc_Core_Opt* _temp930; _LL921: if(( unsigned int) _temp919 > 4u?*((
int*) _temp919) == Cyc_Absyn_Evar: 0){ _LL931: _temp930=(( struct Cyc_Absyn_Evar_struct*)
_temp919)->f1; goto _LL928; _LL928: _temp927=(( struct Cyc_Absyn_Evar_struct*)
_temp919)->f2; _temp929=&(( struct Cyc_Absyn_Evar_struct*) _temp919)->f2; goto
_LL926; _LL926: _temp925=(( struct Cyc_Absyn_Evar_struct*) _temp919)->f4; goto
_LL922;} else{ goto _LL923;} _LL923: goto _LL924; _LL922: Cyc_Tcutil_occurs( t1,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp925))->v, t2);{
void* _temp932= Cyc_Tcutil_typ_kind( t2); if( Cyc_Tcutil_kind_leq( _temp932,(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp930))->v)){* _temp929=({ struct
Cyc_Core_Opt* _temp933=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp933->v=( void*) t2; _temp933;}); return;} else{{ void* _temp934= t2; struct
Cyc_Core_Opt* _temp942; struct Cyc_Core_Opt* _temp944; struct Cyc_Core_Opt**
_temp946; struct Cyc_Absyn_PtrInfo _temp948; _LL936: if(( unsigned int) _temp934
> 4u?*(( int*) _temp934) == Cyc_Absyn_Evar: 0){ _LL945: _temp944=(( struct Cyc_Absyn_Evar_struct*)
_temp934)->f2; _temp946=&(( struct Cyc_Absyn_Evar_struct*) _temp934)->f2; goto
_LL943; _LL943: _temp942=(( struct Cyc_Absyn_Evar_struct*) _temp934)->f4; goto
_LL937;} else{ goto _LL938;} _LL938: if(( unsigned int) _temp934 > 4u?*(( int*)
_temp934) == Cyc_Absyn_PointerType: 0){ _LL949: _temp948=(( struct Cyc_Absyn_PointerType_struct*)
_temp934)->f1; goto _LL947;} else{ goto _LL940;} _LL947: if(( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp930))->v ==( void*) Cyc_Absyn_BoxKind){ goto
_LL939;} else{ goto _LL940;} _LL940: goto _LL941; _LL937: { struct Cyc_List_List*
_temp950=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp925))->v;{
struct Cyc_List_List* s2=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp942))->v; for( 0; s2 != 0; s2=(( struct Cyc_List_List*)
_check_null( s2))->tl){ if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, _temp950,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( s2))->hd)){( int) _throw(( void*) Cyc_Tcutil_Unify);}}} if( Cyc_Tcutil_kind_leq((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp930))->v, _temp932)){* _temp946=({
struct Cyc_Core_Opt* _temp951=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp951->v=( void*) t1; _temp951;}); return;} goto _LL935;}
_LL939: { struct Cyc_Absyn_Conref* _temp952= Cyc_Absyn_compress_conref( _temp948.bounds);{
void* _temp953=( void*) _temp952->v; _LL955: if( _temp953 ==( void*) Cyc_Absyn_No_constr){
goto _LL956;} else{ goto _LL957;} _LL957: goto _LL958; _LL956:( void*)( _temp952->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp959=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp959[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp960; _temp960.tag= Cyc_Absyn_Eq_constr; _temp960.f1=(
void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp961=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp961[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp962; _temp962.tag= Cyc_Absyn_Upper_b; _temp962.f1= Cyc_Absyn_signed_int_exp(
1, 0); _temp962;}); _temp961;})); _temp960;}); _temp959;})));* _temp929=({
struct Cyc_Core_Opt* _temp963=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp963->v=( void*) t2; _temp963;}); return; _LL958: goto
_LL954; _LL954:;} goto _LL935;} _LL941: goto _LL935; _LL935:;}( int) _throw((
void*) Cyc_Tcutil_Unify);}} _LL924: goto _LL920; _LL920:;}{ struct _tuple8
_temp965=({ struct _tuple8 _temp964; _temp964.f1= t2; _temp964.f2= t1; _temp964;});
void* _temp1015; void* _temp1017; void* _temp1019; void* _temp1021; struct Cyc_Absyn_Tvar*
_temp1023; void* _temp1025; struct Cyc_Absyn_Tvar* _temp1027; void* _temp1029;
struct Cyc_Absyn_Structdecl** _temp1031; struct Cyc_List_List* _temp1033; struct
_tuple1* _temp1035; void* _temp1037; struct Cyc_Absyn_Structdecl** _temp1039;
struct Cyc_List_List* _temp1041; struct _tuple1* _temp1043; void* _temp1045;
struct _tuple1* _temp1047; void* _temp1049; struct _tuple1* _temp1051; void*
_temp1053; struct Cyc_Absyn_Uniondecl** _temp1055; struct Cyc_List_List*
_temp1057; struct _tuple1* _temp1059; void* _temp1061; struct Cyc_Absyn_Uniondecl**
_temp1063; struct Cyc_List_List* _temp1065; struct _tuple1* _temp1067; void*
_temp1069; struct Cyc_Absyn_TunionInfo _temp1071; void* _temp1073; struct Cyc_List_List*
_temp1075; void* _temp1077; struct Cyc_Absyn_Tuniondecl* _temp1079; void*
_temp1081; struct Cyc_Absyn_TunionInfo _temp1083; void* _temp1085; struct Cyc_List_List*
_temp1087; void* _temp1089; struct Cyc_Absyn_Tuniondecl* _temp1091; void*
_temp1093; struct Cyc_Absyn_TunionFieldInfo _temp1095; struct Cyc_List_List*
_temp1097; void* _temp1099; struct Cyc_Absyn_Tunionfield* _temp1101; struct Cyc_Absyn_Tuniondecl*
_temp1103; void* _temp1105; struct Cyc_Absyn_TunionFieldInfo _temp1107; struct
Cyc_List_List* _temp1109; void* _temp1111; struct Cyc_Absyn_Tunionfield*
_temp1113; struct Cyc_Absyn_Tuniondecl* _temp1115; void* _temp1117; struct Cyc_Absyn_PtrInfo
_temp1119; struct Cyc_Absyn_Conref* _temp1121; struct Cyc_Absyn_Tqual _temp1123;
struct Cyc_Absyn_Conref* _temp1125; void* _temp1127; void* _temp1129; void*
_temp1131; struct Cyc_Absyn_PtrInfo _temp1133; struct Cyc_Absyn_Conref*
_temp1135; struct Cyc_Absyn_Tqual _temp1137; struct Cyc_Absyn_Conref* _temp1139;
void* _temp1141; void* _temp1143; void* _temp1145; void* _temp1147; void*
_temp1149; void* _temp1151; void* _temp1153; void* _temp1155; void* _temp1157;
void* _temp1159; void* _temp1161; void* _temp1163; void* _temp1165; struct Cyc_Absyn_Exp*
_temp1167; struct Cyc_Absyn_Tqual _temp1169; void* _temp1171; void* _temp1173;
struct Cyc_Absyn_Exp* _temp1175; struct Cyc_Absyn_Tqual _temp1177; void*
_temp1179; void* _temp1181; struct Cyc_Absyn_FnInfo _temp1183; struct Cyc_List_List*
_temp1185; struct Cyc_List_List* _temp1187; struct Cyc_Absyn_VarargInfo*
_temp1189; int _temp1191; struct Cyc_List_List* _temp1193; void* _temp1195;
struct Cyc_Core_Opt* _temp1197; struct Cyc_List_List* _temp1199; void* _temp1201;
struct Cyc_Absyn_FnInfo _temp1203; struct Cyc_List_List* _temp1205; struct Cyc_List_List*
_temp1207; struct Cyc_Absyn_VarargInfo* _temp1209; int _temp1211; struct Cyc_List_List*
_temp1213; void* _temp1215; struct Cyc_Core_Opt* _temp1217; struct Cyc_List_List*
_temp1219; void* _temp1221; struct Cyc_List_List* _temp1223; void* _temp1225;
struct Cyc_List_List* _temp1227; void* _temp1229; struct Cyc_List_List*
_temp1231; void* _temp1233; struct Cyc_List_List* _temp1235; void* _temp1237;
struct Cyc_List_List* _temp1239; void* _temp1241; struct Cyc_List_List*
_temp1243; void* _temp1245; void* _temp1247; void* _temp1249; void* _temp1251;
void* _temp1253; void* _temp1255; void* _temp1257; void* _temp1259; void*
_temp1261; void* _temp1263; _LL967: _LL1016: _temp1015= _temp965.f1; if((
unsigned int) _temp1015 > 4u?*(( int*) _temp1015) == Cyc_Absyn_Evar: 0){ goto
_LL968;} else{ goto _LL969;} _LL969: _LL1020: _temp1019= _temp965.f1; if(
_temp1019 ==( void*) Cyc_Absyn_VoidType){ goto _LL1018;} else{ goto _LL971;}
_LL1018: _temp1017= _temp965.f2; if( _temp1017 ==( void*) Cyc_Absyn_VoidType){
goto _LL970;} else{ goto _LL971;} _LL971: _LL1026: _temp1025= _temp965.f1; if((
unsigned int) _temp1025 > 4u?*(( int*) _temp1025) == Cyc_Absyn_VarType: 0){
_LL1028: _temp1027=(( struct Cyc_Absyn_VarType_struct*) _temp1025)->f1; goto
_LL1022;} else{ goto _LL973;} _LL1022: _temp1021= _temp965.f2; if(( unsigned int)
_temp1021 > 4u?*(( int*) _temp1021) == Cyc_Absyn_VarType: 0){ _LL1024: _temp1023=((
struct Cyc_Absyn_VarType_struct*) _temp1021)->f1; goto _LL972;} else{ goto
_LL973;} _LL973: _LL1038: _temp1037= _temp965.f1; if(( unsigned int) _temp1037 >
4u?*(( int*) _temp1037) == Cyc_Absyn_StructType: 0){ _LL1044: _temp1043=((
struct Cyc_Absyn_StructType_struct*) _temp1037)->f1; goto _LL1042; _LL1042:
_temp1041=(( struct Cyc_Absyn_StructType_struct*) _temp1037)->f2; goto _LL1040;
_LL1040: _temp1039=(( struct Cyc_Absyn_StructType_struct*) _temp1037)->f3; goto
_LL1030;} else{ goto _LL975;} _LL1030: _temp1029= _temp965.f2; if(( unsigned int)
_temp1029 > 4u?*(( int*) _temp1029) == Cyc_Absyn_StructType: 0){ _LL1036:
_temp1035=(( struct Cyc_Absyn_StructType_struct*) _temp1029)->f1; goto _LL1034;
_LL1034: _temp1033=(( struct Cyc_Absyn_StructType_struct*) _temp1029)->f2; goto
_LL1032; _LL1032: _temp1031=(( struct Cyc_Absyn_StructType_struct*) _temp1029)->f3;
goto _LL974;} else{ goto _LL975;} _LL975: _LL1050: _temp1049= _temp965.f1; if((
unsigned int) _temp1049 > 4u?*(( int*) _temp1049) == Cyc_Absyn_EnumType: 0){
_LL1052: _temp1051=(( struct Cyc_Absyn_EnumType_struct*) _temp1049)->f1; goto
_LL1046;} else{ goto _LL977;} _LL1046: _temp1045= _temp965.f2; if(( unsigned int)
_temp1045 > 4u?*(( int*) _temp1045) == Cyc_Absyn_EnumType: 0){ _LL1048:
_temp1047=(( struct Cyc_Absyn_EnumType_struct*) _temp1045)->f1; goto _LL976;}
else{ goto _LL977;} _LL977: _LL1062: _temp1061= _temp965.f1; if(( unsigned int)
_temp1061 > 4u?*(( int*) _temp1061) == Cyc_Absyn_UnionType: 0){ _LL1068:
_temp1067=(( struct Cyc_Absyn_UnionType_struct*) _temp1061)->f1; goto _LL1066;
_LL1066: _temp1065=(( struct Cyc_Absyn_UnionType_struct*) _temp1061)->f2; goto
_LL1064; _LL1064: _temp1063=(( struct Cyc_Absyn_UnionType_struct*) _temp1061)->f3;
goto _LL1054;} else{ goto _LL979;} _LL1054: _temp1053= _temp965.f2; if((
unsigned int) _temp1053 > 4u?*(( int*) _temp1053) == Cyc_Absyn_UnionType: 0){
_LL1060: _temp1059=(( struct Cyc_Absyn_UnionType_struct*) _temp1053)->f1; goto
_LL1058; _LL1058: _temp1057=(( struct Cyc_Absyn_UnionType_struct*) _temp1053)->f2;
goto _LL1056; _LL1056: _temp1055=(( struct Cyc_Absyn_UnionType_struct*)
_temp1053)->f3; goto _LL978;} else{ goto _LL979;} _LL979: _LL1082: _temp1081=
_temp965.f1; if(( unsigned int) _temp1081 > 4u?*(( int*) _temp1081) == Cyc_Absyn_TunionType:
0){ _LL1084: _temp1083=(( struct Cyc_Absyn_TunionType_struct*) _temp1081)->f1;
_LL1090: _temp1089=( void*) _temp1083.tunion_info; if(*(( int*) _temp1089) ==
Cyc_Absyn_KnownTunion){ _LL1092: _temp1091=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1089)->f1; goto _LL1088;} else{ goto _LL981;} _LL1088: _temp1087= _temp1083.targs;
goto _LL1086; _LL1086: _temp1085=( void*) _temp1083.rgn; goto _LL1070;} else{
goto _LL981;} _LL1070: _temp1069= _temp965.f2; if(( unsigned int) _temp1069 > 4u?*((
int*) _temp1069) == Cyc_Absyn_TunionType: 0){ _LL1072: _temp1071=(( struct Cyc_Absyn_TunionType_struct*)
_temp1069)->f1; _LL1078: _temp1077=( void*) _temp1071.tunion_info; if(*(( int*)
_temp1077) == Cyc_Absyn_KnownTunion){ _LL1080: _temp1079=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1077)->f1; goto _LL1076;} else{ goto _LL981;} _LL1076: _temp1075= _temp1071.targs;
goto _LL1074; _LL1074: _temp1073=( void*) _temp1071.rgn; goto _LL980;} else{
goto _LL981;} _LL981: _LL1106: _temp1105= _temp965.f1; if(( unsigned int)
_temp1105 > 4u?*(( int*) _temp1105) == Cyc_Absyn_TunionFieldType: 0){ _LL1108:
_temp1107=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1105)->f1; _LL1112:
_temp1111=( void*) _temp1107.field_info; if(*(( int*) _temp1111) == Cyc_Absyn_KnownTunionfield){
_LL1116: _temp1115=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1111)->f1;
goto _LL1114; _LL1114: _temp1113=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1111)->f2; goto _LL1110;} else{ goto _LL983;} _LL1110: _temp1109= _temp1107.targs;
goto _LL1094;} else{ goto _LL983;} _LL1094: _temp1093= _temp965.f2; if((
unsigned int) _temp1093 > 4u?*(( int*) _temp1093) == Cyc_Absyn_TunionFieldType:
0){ _LL1096: _temp1095=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1093)->f1;
_LL1100: _temp1099=( void*) _temp1095.field_info; if(*(( int*) _temp1099) == Cyc_Absyn_KnownTunionfield){
_LL1104: _temp1103=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1099)->f1;
goto _LL1102; _LL1102: _temp1101=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1099)->f2; goto _LL1098;} else{ goto _LL983;} _LL1098: _temp1097= _temp1095.targs;
goto _LL982;} else{ goto _LL983;} _LL983: _LL1132: _temp1131= _temp965.f1; if((
unsigned int) _temp1131 > 4u?*(( int*) _temp1131) == Cyc_Absyn_PointerType: 0){
_LL1134: _temp1133=(( struct Cyc_Absyn_PointerType_struct*) _temp1131)->f1;
_LL1144: _temp1143=( void*) _temp1133.elt_typ; goto _LL1142; _LL1142: _temp1141=(
void*) _temp1133.rgn_typ; goto _LL1140; _LL1140: _temp1139= _temp1133.nullable;
goto _LL1138; _LL1138: _temp1137= _temp1133.tq; goto _LL1136; _LL1136: _temp1135=
_temp1133.bounds; goto _LL1118;} else{ goto _LL985;} _LL1118: _temp1117=
_temp965.f2; if(( unsigned int) _temp1117 > 4u?*(( int*) _temp1117) == Cyc_Absyn_PointerType:
0){ _LL1120: _temp1119=(( struct Cyc_Absyn_PointerType_struct*) _temp1117)->f1;
_LL1130: _temp1129=( void*) _temp1119.elt_typ; goto _LL1128; _LL1128: _temp1127=(
void*) _temp1119.rgn_typ; goto _LL1126; _LL1126: _temp1125= _temp1119.nullable;
goto _LL1124; _LL1124: _temp1123= _temp1119.tq; goto _LL1122; _LL1122: _temp1121=
_temp1119.bounds; goto _LL984;} else{ goto _LL985;} _LL985: _LL1152: _temp1151=
_temp965.f1; if(( unsigned int) _temp1151 > 4u?*(( int*) _temp1151) == Cyc_Absyn_IntType:
0){ _LL1156: _temp1155=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1151)->f1;
goto _LL1154; _LL1154: _temp1153=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1151)->f2; goto _LL1146;} else{ goto _LL987;} _LL1146: _temp1145= _temp965.f2;
if(( unsigned int) _temp1145 > 4u?*(( int*) _temp1145) == Cyc_Absyn_IntType: 0){
_LL1150: _temp1149=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1145)->f1;
goto _LL1148; _LL1148: _temp1147=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1145)->f2; goto _LL986;} else{ goto _LL987;} _LL987: _LL1160: _temp1159=
_temp965.f1; if( _temp1159 ==( void*) Cyc_Absyn_FloatType){ goto _LL1158;} else{
goto _LL989;} _LL1158: _temp1157= _temp965.f2; if( _temp1157 ==( void*) Cyc_Absyn_FloatType){
goto _LL988;} else{ goto _LL989;} _LL989: _LL1164: _temp1163= _temp965.f1; if(
_temp1163 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1162;} else{ goto _LL991;}
_LL1162: _temp1161= _temp965.f2; if( _temp1161 ==( void*) Cyc_Absyn_DoubleType){
goto _LL990;} else{ goto _LL991;} _LL991: _LL1174: _temp1173= _temp965.f1; if((
unsigned int) _temp1173 > 4u?*(( int*) _temp1173) == Cyc_Absyn_ArrayType: 0){
_LL1180: _temp1179=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1173)->f1;
goto _LL1178; _LL1178: _temp1177=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1173)->f2; goto _LL1176; _LL1176: _temp1175=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1173)->f3; goto _LL1166;} else{ goto _LL993;} _LL1166: _temp1165= _temp965.f2;
if(( unsigned int) _temp1165 > 4u?*(( int*) _temp1165) == Cyc_Absyn_ArrayType: 0){
_LL1172: _temp1171=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1165)->f1;
goto _LL1170; _LL1170: _temp1169=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1165)->f2; goto _LL1168; _LL1168: _temp1167=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1165)->f3; goto _LL992;} else{ goto _LL993;} _LL993: _LL1202: _temp1201=
_temp965.f1; if(( unsigned int) _temp1201 > 4u?*(( int*) _temp1201) == Cyc_Absyn_FnType:
0){ _LL1204: _temp1203=(( struct Cyc_Absyn_FnType_struct*) _temp1201)->f1;
_LL1220: _temp1219= _temp1203.tvars; goto _LL1218; _LL1218: _temp1217= _temp1203.effect;
goto _LL1216; _LL1216: _temp1215=( void*) _temp1203.ret_typ; goto _LL1214;
_LL1214: _temp1213= _temp1203.args; goto _LL1212; _LL1212: _temp1211= _temp1203.c_varargs;
goto _LL1210; _LL1210: _temp1209= _temp1203.cyc_varargs; goto _LL1208; _LL1208:
_temp1207= _temp1203.rgn_po; goto _LL1206; _LL1206: _temp1205= _temp1203.attributes;
goto _LL1182;} else{ goto _LL995;} _LL1182: _temp1181= _temp965.f2; if((
unsigned int) _temp1181 > 4u?*(( int*) _temp1181) == Cyc_Absyn_FnType: 0){
_LL1184: _temp1183=(( struct Cyc_Absyn_FnType_struct*) _temp1181)->f1; _LL1200:
_temp1199= _temp1183.tvars; goto _LL1198; _LL1198: _temp1197= _temp1183.effect;
goto _LL1196; _LL1196: _temp1195=( void*) _temp1183.ret_typ; goto _LL1194;
_LL1194: _temp1193= _temp1183.args; goto _LL1192; _LL1192: _temp1191= _temp1183.c_varargs;
goto _LL1190; _LL1190: _temp1189= _temp1183.cyc_varargs; goto _LL1188; _LL1188:
_temp1187= _temp1183.rgn_po; goto _LL1186; _LL1186: _temp1185= _temp1183.attributes;
goto _LL994;} else{ goto _LL995;} _LL995: _LL1226: _temp1225= _temp965.f1; if((
unsigned int) _temp1225 > 4u?*(( int*) _temp1225) == Cyc_Absyn_TupleType: 0){
_LL1228: _temp1227=(( struct Cyc_Absyn_TupleType_struct*) _temp1225)->f1; goto
_LL1222;} else{ goto _LL997;} _LL1222: _temp1221= _temp965.f2; if(( unsigned int)
_temp1221 > 4u?*(( int*) _temp1221) == Cyc_Absyn_TupleType: 0){ _LL1224:
_temp1223=(( struct Cyc_Absyn_TupleType_struct*) _temp1221)->f1; goto _LL996;}
else{ goto _LL997;} _LL997: _LL1234: _temp1233= _temp965.f1; if(( unsigned int)
_temp1233 > 4u?*(( int*) _temp1233) == Cyc_Absyn_AnonStructType: 0){ _LL1236:
_temp1235=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1233)->f1; goto
_LL1230;} else{ goto _LL999;} _LL1230: _temp1229= _temp965.f2; if(( unsigned int)
_temp1229 > 4u?*(( int*) _temp1229) == Cyc_Absyn_AnonStructType: 0){ _LL1232:
_temp1231=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1229)->f1; goto
_LL998;} else{ goto _LL999;} _LL999: _LL1242: _temp1241= _temp965.f1; if((
unsigned int) _temp1241 > 4u?*(( int*) _temp1241) == Cyc_Absyn_AnonUnionType: 0){
_LL1244: _temp1243=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1241)->f1;
goto _LL1238;} else{ goto _LL1001;} _LL1238: _temp1237= _temp965.f2; if((
unsigned int) _temp1237 > 4u?*(( int*) _temp1237) == Cyc_Absyn_AnonUnionType: 0){
_LL1240: _temp1239=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1237)->f1;
goto _LL1000;} else{ goto _LL1001;} _LL1001: _LL1248: _temp1247= _temp965.f1;
if( _temp1247 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1246;} else{ goto _LL1003;}
_LL1246: _temp1245= _temp965.f2; if( _temp1245 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL1002;} else{ goto _LL1003;} _LL1003: _LL1254: _temp1253= _temp965.f1;
if(( unsigned int) _temp1253 > 4u?*(( int*) _temp1253) == Cyc_Absyn_RgnHandleType:
0){ _LL1256: _temp1255=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1253)->f1; goto _LL1250;} else{ goto _LL1005;} _LL1250: _temp1249= _temp965.f2;
if(( unsigned int) _temp1249 > 4u?*(( int*) _temp1249) == Cyc_Absyn_RgnHandleType:
0){ _LL1252: _temp1251=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1249)->f1; goto _LL1004;} else{ goto _LL1005;} _LL1005: _LL1258: _temp1257=
_temp965.f1; if(( unsigned int) _temp1257 > 4u?*(( int*) _temp1257) == Cyc_Absyn_JoinEff:
0){ goto _LL1006;} else{ goto _LL1007;} _LL1007: _LL1260: _temp1259= _temp965.f2;
if(( unsigned int) _temp1259 > 4u?*(( int*) _temp1259) == Cyc_Absyn_JoinEff: 0){
goto _LL1008;} else{ goto _LL1009;} _LL1009: _LL1262: _temp1261= _temp965.f1;
if(( unsigned int) _temp1261 > 4u?*(( int*) _temp1261) == Cyc_Absyn_AccessEff: 0){
goto _LL1010;} else{ goto _LL1011;} _LL1011: _LL1264: _temp1263= _temp965.f2;
if(( unsigned int) _temp1263 > 4u?*(( int*) _temp1263) == Cyc_Absyn_AccessEff: 0){
goto _LL1012;} else{ goto _LL1013;} _LL1013: goto _LL1014; _LL968: Cyc_Tcutil_unify_it(
t2, t1); return; _LL970: return; _LL972: { struct _tagged_arr* _temp1265=
_temp1027->name; struct _tagged_arr* _temp1266= _temp1023->name; int _temp1267=*((
int*) _check_null( _temp1027->identity)); int _temp1268=*(( int*) _check_null(
_temp1023->identity)); void* _temp1269= Cyc_Absyn_conref_val( _temp1027->kind);
void* _temp1270= Cyc_Absyn_conref_val( _temp1023->kind); if( _temp1268 ==
_temp1267? Cyc_String_zstrptrcmp( _temp1265, _temp1266) == 0: 0){ if( _temp1269
!= _temp1270){({ int(* _temp1271)( struct _tagged_arr fmt, struct _tagged_arr ap)=(
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos; struct
_tagged_arr _temp1277= _tag_arr("same type variable %s has kinds %s and %s",
sizeof( unsigned char), 42u); struct Cyc_Stdio_String_pa_struct _temp1280;
_temp1280.tag= Cyc_Stdio_String_pa; _temp1280.f1=( struct _tagged_arr)*
_temp1265;{ void* _temp1274=( void*)& _temp1280; struct Cyc_Stdio_String_pa_struct
_temp1279; _temp1279.tag= Cyc_Stdio_String_pa; _temp1279.f1=( struct _tagged_arr)
Cyc_Absynpp_kind2string( _temp1269);{ void* _temp1275=( void*)& _temp1279;
struct Cyc_Stdio_String_pa_struct _temp1278; _temp1278.tag= Cyc_Stdio_String_pa;
_temp1278.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( _temp1270);{ void*
_temp1276=( void*)& _temp1278; void* _temp1272[ 3u]={ _temp1274, _temp1275,
_temp1276}; struct _tagged_arr _temp1273={( void*) _temp1272,( void*) _temp1272,(
void*)( _temp1272 + 3u)}; _temp1271( _temp1277, _temp1273);}}}});} return;} goto
_LL966;} _LL974: if((( _temp1035 != 0? _temp1043 != 0: 0)? Cyc_Absyn_qvar_cmp((
struct _tuple1*) _check_null( _temp1035),( struct _tuple1*) _check_null(
_temp1043)) == 0: 0)? 1:( _temp1035 == 0? _temp1043 == 0: 0)){ Cyc_Tcutil_unify_list(
_temp1033, _temp1041); return;} goto _LL966; _LL976: if( Cyc_Absyn_qvar_cmp(
_temp1051, _temp1047) == 0){ return;} goto _LL966; _LL978: if((( _temp1059 != 0?
_temp1067 != 0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*) _check_null( _temp1059),(
struct _tuple1*) _check_null( _temp1067)) == 0: 0)? 1:( _temp1059 == 0?
_temp1067 == 0: 0)){ Cyc_Tcutil_unify_list( _temp1057, _temp1065); return;} goto
_LL966; _LL980: if( _temp1091 == _temp1079? 1: Cyc_Absyn_qvar_cmp( _temp1091->name,
_temp1079->name) == 0){ Cyc_Tcutil_unify_it( _temp1073, _temp1085); Cyc_Tcutil_unify_list(
_temp1075, _temp1087); return;} goto _LL966; _LL982: if(( _temp1115 == _temp1103?
1: Cyc_Absyn_qvar_cmp( _temp1115->name, _temp1103->name) == 0)? _temp1113 ==
_temp1101? 1: Cyc_Absyn_qvar_cmp( _temp1113->name, _temp1101->name) == 0: 0){
Cyc_Tcutil_unify_list( _temp1097, _temp1109); return;} goto _LL966; _LL984: Cyc_Tcutil_unify_it(
_temp1129, _temp1143); Cyc_Tcutil_unify_it( _temp1141, _temp1127); Cyc_Tcutil_unify_tqual(
_temp1123, _temp1137); Cyc_Tcutil_unify_it_conrefs( Cyc_Tcutil_boundscmp,
_temp1121, _temp1135);{ void* _temp1281=( void*)( Cyc_Absyn_compress_conref(
_temp1121))->v; void* _temp1287; _LL1283: if(( unsigned int) _temp1281 > 1u?*((
int*) _temp1281) == Cyc_Absyn_Eq_constr: 0){ _LL1288: _temp1287=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1281)->f1; if( _temp1287 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1284;} else{ goto _LL1285;}} else{ goto _LL1285;} _LL1285: goto _LL1286;
_LL1284: return; _LL1286: goto _LL1282; _LL1282:;}(( void(*)( int(* cmp)( int,
int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp, _temp1125, _temp1139); return; _LL986: if( _temp1149 ==
_temp1155? _temp1147 == _temp1153: 0){ return;} goto _LL966; _LL988: return;
_LL990: return; _LL992: Cyc_Tcutil_unify_it( _temp1171, _temp1179); Cyc_Tcutil_unify_tqual(
_temp1169, _temp1177); if( _temp1175 == _temp1167){ return;} if( _temp1175 == 0?
1: _temp1167 == 0){ goto _LL966;} if( Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1175)) == Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1167))){ return;} goto _LL966; _LL994: { int done= 0;{ struct
_RegionHandle _temp1289= _new_region(); struct _RegionHandle* rgn=& _temp1289;
_push_region( rgn);{ struct Cyc_List_List* inst= 0; while( _temp1199 != 0) { if(
_temp1219 == 0){ break;} inst=({ struct Cyc_List_List* _temp1290=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1290->hd=( void*)({
struct _tuple5* _temp1291=( struct _tuple5*) _region_malloc( rgn, sizeof( struct
_tuple5)); _temp1291->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp1219))->hd; _temp1291->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1292=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1292[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1293; _temp1293.tag= Cyc_Absyn_VarType;
_temp1293.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp1199))->hd; _temp1293;}); _temp1292;}); _temp1291;}); _temp1290->tl= inst;
_temp1290;}); _temp1199=(( struct Cyc_List_List*) _check_null( _temp1199))->tl;
_temp1219=(( struct Cyc_List_List*) _check_null( _temp1219))->tl;} if( _temp1219
!= 0){ _npop_handler( 0u); goto _LL966;} if( inst != 0){ Cyc_Tcutil_unify_it((
void*)({ struct Cyc_Absyn_FnType_struct* _temp1294=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1294[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1295; _temp1295.tag= Cyc_Absyn_FnType; _temp1295.f1=({ struct Cyc_Absyn_FnInfo
_temp1296; _temp1296.tvars= 0; _temp1296.effect= _temp1197; _temp1296.ret_typ=(
void*) _temp1195; _temp1296.args= _temp1193; _temp1296.c_varargs= _temp1191;
_temp1296.cyc_varargs= _temp1189; _temp1296.rgn_po= _temp1187; _temp1296.attributes=
_temp1185; _temp1296;}); _temp1295;}); _temp1294;}), Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1297=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1297[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1298; _temp1298.tag= Cyc_Absyn_FnType; _temp1298.f1=({ struct Cyc_Absyn_FnInfo
_temp1299; _temp1299.tvars= 0; _temp1299.effect= _temp1217; _temp1299.ret_typ=(
void*) _temp1215; _temp1299.args= _temp1213; _temp1299.c_varargs= _temp1211;
_temp1299.cyc_varargs= _temp1209; _temp1299.rgn_po= _temp1207; _temp1299.attributes=
_temp1205; _temp1299;}); _temp1298;}); _temp1297;}))); done= 1;}}; _pop_region(
rgn);} if( done){ return;} Cyc_Tcutil_unify_it( _temp1195, _temp1215); for( 0;
_temp1193 != 0? _temp1213 != 0: 0;( _temp1193=(( struct Cyc_List_List*)
_check_null( _temp1193))->tl, _temp1213=(( struct Cyc_List_List*) _check_null(
_temp1213))->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp1193))->hd)).f2,(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp1213))->hd)).f2); Cyc_Tcutil_unify_it((*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp1193))->hd)).f3,(*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp1213))->hd)).f3);} if( _temp1193 != 0?
1: _temp1213 != 0){ goto _LL966;} if( _temp1191 != _temp1211){ goto _LL966;}{
int bad_cyc_vararg= 0;{ struct _tuple11 _temp1301=({ struct _tuple11 _temp1300;
_temp1300.f1= _temp1189; _temp1300.f2= _temp1209; _temp1300;}); struct Cyc_Absyn_VarargInfo*
_temp1311; struct Cyc_Absyn_VarargInfo* _temp1313; struct Cyc_Absyn_VarargInfo*
_temp1315; struct Cyc_Absyn_VarargInfo* _temp1317; struct Cyc_Absyn_VarargInfo*
_temp1319; struct Cyc_Absyn_VarargInfo _temp1321; int _temp1322; void* _temp1324;
void* _temp1326; struct Cyc_Absyn_Tqual _temp1328; struct Cyc_Core_Opt*
_temp1330; struct Cyc_Absyn_VarargInfo* _temp1332; struct Cyc_Absyn_VarargInfo
_temp1334; int _temp1335; void* _temp1337; void* _temp1339; struct Cyc_Absyn_Tqual
_temp1341; struct Cyc_Core_Opt* _temp1343; _LL1303: _LL1314: _temp1313=
_temp1301.f1; if( _temp1313 == 0){ goto _LL1312;} else{ goto _LL1305;} _LL1312:
_temp1311= _temp1301.f2; if( _temp1311 == 0){ goto _LL1304;} else{ goto _LL1305;}
_LL1305: _LL1316: _temp1315= _temp1301.f1; if( _temp1315 == 0){ goto _LL1306;}
else{ goto _LL1307;} _LL1307: _LL1318: _temp1317= _temp1301.f2; if( _temp1317 ==
0){ goto _LL1308;} else{ goto _LL1309;} _LL1309: _LL1333: _temp1332= _temp1301.f1;
if( _temp1332 == 0){ goto _LL1302;} else{ _temp1334=* _temp1332; _LL1344:
_temp1343= _temp1334.name; goto _LL1342; _LL1342: _temp1341= _temp1334.tq; goto
_LL1340; _LL1340: _temp1339=( void*) _temp1334.type; goto _LL1338; _LL1338:
_temp1337=( void*) _temp1334.rgn; goto _LL1336; _LL1336: _temp1335= _temp1334.inject;
goto _LL1320;} _LL1320: _temp1319= _temp1301.f2; if( _temp1319 == 0){ goto
_LL1302;} else{ _temp1321=* _temp1319; _LL1331: _temp1330= _temp1321.name; goto
_LL1329; _LL1329: _temp1328= _temp1321.tq; goto _LL1327; _LL1327: _temp1326=(
void*) _temp1321.type; goto _LL1325; _LL1325: _temp1324=( void*) _temp1321.rgn;
goto _LL1323; _LL1323: _temp1322= _temp1321.inject; goto _LL1310;} _LL1304: goto
_LL1302; _LL1306: goto _LL1308; _LL1308: bad_cyc_vararg= 1; goto _LL1302;
_LL1310: Cyc_Tcutil_unify_tqual( _temp1341, _temp1328); Cyc_Tcutil_unify_it(
_temp1339, _temp1326); Cyc_Tcutil_unify_it( _temp1337, _temp1324); if( _temp1335
!= _temp1322){ bad_cyc_vararg= 1;} goto _LL1302; _LL1302:;} if( bad_cyc_vararg){
goto _LL966;}{ int bad_effect= 0;{ struct _tuple12 _temp1346=({ struct _tuple12
_temp1345; _temp1345.f1= _temp1197; _temp1345.f2= _temp1217; _temp1345;});
struct Cyc_Core_Opt* _temp1356; struct Cyc_Core_Opt* _temp1358; struct Cyc_Core_Opt*
_temp1360; struct Cyc_Core_Opt* _temp1362; _LL1348: _LL1359: _temp1358=
_temp1346.f1; if( _temp1358 == 0){ goto _LL1357;} else{ goto _LL1350;} _LL1357:
_temp1356= _temp1346.f2; if( _temp1356 == 0){ goto _LL1349;} else{ goto _LL1350;}
_LL1350: _LL1361: _temp1360= _temp1346.f1; if( _temp1360 == 0){ goto _LL1351;}
else{ goto _LL1352;} _LL1352: _LL1363: _temp1362= _temp1346.f2; if( _temp1362 ==
0){ goto _LL1353;} else{ goto _LL1354;} _LL1354: goto _LL1355; _LL1349: goto
_LL1347; _LL1351: goto _LL1353; _LL1353: bad_effect= 1; goto _LL1347; _LL1355:
bad_effect= ! Cyc_Tcutil_unify_effect(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1197))->v,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1217))->v); goto _LL1347; _LL1347:;} if( bad_effect){ goto _LL966;} if( !
Cyc_Tcutil_same_atts( _temp1205, _temp1185)){ goto _LL966;} if( ! Cyc_Tcutil_same_rgn_po(
_temp1207, _temp1187)){ goto _LL966;} return;}}} _LL996: for( 0; _temp1223 != 0?
_temp1227 != 0: 0;( _temp1223=(( struct Cyc_List_List*) _check_null( _temp1223))->tl,
_temp1227=(( struct Cyc_List_List*) _check_null( _temp1227))->tl)){ Cyc_Tcutil_unify_tqual((*((
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1223))->hd)).f1,(*((
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1227))->hd)).f1);
Cyc_Tcutil_unify_it((*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp1223))->hd)).f2,(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp1227))->hd)).f2);} if( _temp1223 == 0? _temp1227 == 0: 0){ return;} goto
_LL966; _LL998: _temp1243= _temp1235; _temp1239= _temp1231; goto _LL1000;
_LL1000: for( 0; _temp1239 != 0? _temp1243 != 0: 0;( _temp1239=(( struct Cyc_List_List*)
_check_null( _temp1239))->tl, _temp1243=(( struct Cyc_List_List*) _check_null(
_temp1243))->tl)){ struct Cyc_Absyn_Structfield* _temp1364=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp1239))->hd; struct Cyc_Absyn_Structfield*
_temp1365=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1243))->hd; if( Cyc_String_zstrptrcmp( _temp1364->name, _temp1365->name) !=
0){( int) _throw(( void*) Cyc_Tcutil_Unify);} Cyc_Tcutil_unify_tqual( _temp1364->tq,
_temp1365->tq); Cyc_Tcutil_unify_it(( void*) _temp1364->type,( void*) _temp1365->type);
if((( ! Cyc_Tcutil_same_atts( _temp1364->attributes, _temp1365->attributes)? 1:(
_temp1364->width != 0? _temp1365->width == 0: 0))? 1:( _temp1365->width != 0?
_temp1364->width == 0: 0))? 1:(( _temp1364->width != 0? _temp1365->width != 0: 0)?
Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*)
_check_null( _temp1364->width))->v) != Cyc_Evexp_eval_const_uint_exp(( struct
Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp1365->width))->v): 0)){(
int) _throw(( void*) Cyc_Tcutil_Unify);}} if( _temp1239 == 0? _temp1243 == 0: 0){
return;} goto _LL966; _LL1002: return; _LL1004: Cyc_Tcutil_unify_it( _temp1255,
_temp1251); return; _LL1006: goto _LL1008; _LL1008: goto _LL1010; _LL1010: goto
_LL1012; _LL1012: if( Cyc_Tcutil_unify_effect( t1, t2)){ return;} else{ goto
_LL966;} _LL1014: goto _LL966; _LL966:;}( int) _throw(( void*) Cyc_Tcutil_Unify);}
int Cyc_Tcutil_is_arithmetic_type( void* t){ void* _temp1366= Cyc_Tcutil_compress(
t); _LL1368: if(( unsigned int) _temp1366 > 4u?*(( int*) _temp1366) == Cyc_Absyn_IntType:
0){ goto _LL1369;} else{ goto _LL1370;} _LL1370: if( _temp1366 ==( void*) Cyc_Absyn_FloatType){
goto _LL1371;} else{ goto _LL1372;} _LL1372: if( _temp1366 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1373;} else{ goto _LL1374;} _LL1374: if(( unsigned int) _temp1366 > 4u?*((
int*) _temp1366) == Cyc_Absyn_EnumType: 0){ goto _LL1375;} else{ goto _LL1376;}
_LL1376: goto _LL1377; _LL1369: return 1; _LL1371: return 1; _LL1373: return 1;
_LL1375: return 1; _LL1377: return 0; _LL1367:;} int Cyc_Tcutil_will_lose_precision(
void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple8 _temp1379=({ struct _tuple8 _temp1378; _temp1378.f1= t1;
_temp1378.f2= t2; _temp1378;}); void* _temp1401; void* _temp1403; void*
_temp1405; void* _temp1407; void* _temp1409; void* _temp1411; void* _temp1413;
void* _temp1415; void* _temp1417; void* _temp1419; void* _temp1421; void*
_temp1423; void* _temp1425; void* _temp1427; void* _temp1429; void* _temp1431;
void* _temp1433; void* _temp1435; void* _temp1437; void* _temp1439; void*
_temp1441; void* _temp1443; void* _temp1445; void* _temp1447; void* _temp1449;
void* _temp1451; void* _temp1453; _LL1381: _LL1404: _temp1403= _temp1379.f1; if(
_temp1403 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1402;} else{ goto _LL1383;}
_LL1402: _temp1401= _temp1379.f2; if( _temp1401 ==( void*) Cyc_Absyn_FloatType){
goto _LL1382;} else{ goto _LL1383;} _LL1383: _LL1408: _temp1407= _temp1379.f1;
if( _temp1407 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1406;} else{ goto
_LL1385;} _LL1406: _temp1405= _temp1379.f2; if(( unsigned int) _temp1405 > 4u?*((
int*) _temp1405) == Cyc_Absyn_IntType: 0){ goto _LL1384;} else{ goto _LL1385;}
_LL1385: _LL1412: _temp1411= _temp1379.f1; if( _temp1411 ==( void*) Cyc_Absyn_FloatType){
goto _LL1410;} else{ goto _LL1387;} _LL1410: _temp1409= _temp1379.f2; if((
unsigned int) _temp1409 > 4u?*(( int*) _temp1409) == Cyc_Absyn_IntType: 0){ goto
_LL1386;} else{ goto _LL1387;} _LL1387: _LL1418: _temp1417= _temp1379.f1; if((
unsigned int) _temp1417 > 4u?*(( int*) _temp1417) == Cyc_Absyn_IntType: 0){
_LL1420: _temp1419=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1417)->f2;
if( _temp1419 ==( void*) Cyc_Absyn_B8){ goto _LL1414;} else{ goto _LL1389;}}
else{ goto _LL1389;} _LL1414: _temp1413= _temp1379.f2; if(( unsigned int)
_temp1413 > 4u?*(( int*) _temp1413) == Cyc_Absyn_IntType: 0){ _LL1416: _temp1415=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1413)->f2; if( _temp1415 ==(
void*) Cyc_Absyn_B8){ goto _LL1388;} else{ goto _LL1389;}} else{ goto _LL1389;}
_LL1389: _LL1422: _temp1421= _temp1379.f1; if(( unsigned int) _temp1421 > 4u?*((
int*) _temp1421) == Cyc_Absyn_IntType: 0){ _LL1424: _temp1423=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1421)->f2; if( _temp1423 ==( void*) Cyc_Absyn_B8){
goto _LL1390;} else{ goto _LL1391;}} else{ goto _LL1391;} _LL1391: _LL1428:
_temp1427= _temp1379.f1; if(( unsigned int) _temp1427 > 4u?*(( int*) _temp1427)
== Cyc_Absyn_IntType: 0){ _LL1430: _temp1429=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1427)->f2; if( _temp1429 ==( void*) Cyc_Absyn_B4){ goto _LL1426;} else{
goto _LL1393;}} else{ goto _LL1393;} _LL1426: _temp1425= _temp1379.f2; if(
_temp1425 ==( void*) Cyc_Absyn_FloatType){ goto _LL1392;} else{ goto _LL1393;}
_LL1393: _LL1436: _temp1435= _temp1379.f1; if(( unsigned int) _temp1435 > 4u?*((
int*) _temp1435) == Cyc_Absyn_IntType: 0){ _LL1438: _temp1437=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1435)->f2; if( _temp1437 ==( void*) Cyc_Absyn_B4){
goto _LL1432;} else{ goto _LL1395;}} else{ goto _LL1395;} _LL1432: _temp1431=
_temp1379.f2; if(( unsigned int) _temp1431 > 4u?*(( int*) _temp1431) == Cyc_Absyn_IntType:
0){ _LL1434: _temp1433=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1431)->f2;
if( _temp1433 ==( void*) Cyc_Absyn_B2){ goto _LL1394;} else{ goto _LL1395;}}
else{ goto _LL1395;} _LL1395: _LL1444: _temp1443= _temp1379.f1; if((
unsigned int) _temp1443 > 4u?*(( int*) _temp1443) == Cyc_Absyn_IntType: 0){
_LL1446: _temp1445=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1443)->f2;
if( _temp1445 ==( void*) Cyc_Absyn_B4){ goto _LL1440;} else{ goto _LL1397;}}
else{ goto _LL1397;} _LL1440: _temp1439= _temp1379.f2; if(( unsigned int)
_temp1439 > 4u?*(( int*) _temp1439) == Cyc_Absyn_IntType: 0){ _LL1442: _temp1441=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1439)->f2; if( _temp1441 ==(
void*) Cyc_Absyn_B1){ goto _LL1396;} else{ goto _LL1397;}} else{ goto _LL1397;}
_LL1397: _LL1452: _temp1451= _temp1379.f1; if(( unsigned int) _temp1451 > 4u?*((
int*) _temp1451) == Cyc_Absyn_IntType: 0){ _LL1454: _temp1453=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1451)->f2; if( _temp1453 ==( void*) Cyc_Absyn_B2){
goto _LL1448;} else{ goto _LL1399;}} else{ goto _LL1399;} _LL1448: _temp1447=
_temp1379.f2; if(( unsigned int) _temp1447 > 4u?*(( int*) _temp1447) == Cyc_Absyn_IntType:
0){ _LL1450: _temp1449=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1447)->f2;
if( _temp1449 ==( void*) Cyc_Absyn_B1){ goto _LL1398;} else{ goto _LL1399;}}
else{ goto _LL1399;} _LL1399: goto _LL1400; _LL1382: return 1; _LL1384: return 1;
_LL1386: return 1; _LL1388: return 0; _LL1390: return 1; _LL1392: return 1;
_LL1394: return 1; _LL1396: return 1; _LL1398: return 1; _LL1400: return 0;
_LL1380:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv* te, void* t,
struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{ struct Cyc_List_List*
el= es; for( 0; el != 0; el=(( struct Cyc_List_List*) _check_null( el))->tl){
void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);
if( Cyc_Tcutil_is_arithmetic_type( t1)){ if( max_arith_type == 0? 1: Cyc_Tcutil_will_lose_precision(
t1,( void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){
max_arith_type=({ struct Cyc_Core_Opt* _temp1455=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1455->v=( void*) t1; _temp1455;});}}}}
if( max_arith_type != 0){ if( ! Cyc_Tcutil_unify( t,( void*)(( struct Cyc_Core_Opt*)
_check_null( max_arith_type))->v)){ return 0;}}{ struct Cyc_List_List* el= es;
for( 0; el != 0; el=(( struct Cyc_List_List*) _check_null( el))->tl){ if( ! Cyc_Tcutil_coerce_assign(
te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd, t)){({
void(* _temp1456)( struct Cyc_Position_Segment* loc, struct _tagged_arr fmt,
struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp1461=((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->loc;
struct _tagged_arr _temp1462= _tag_arr("type mismatch: expecting %s but found %s",
sizeof( unsigned char), 41u); struct Cyc_Stdio_String_pa_struct _temp1464;
_temp1464.tag= Cyc_Stdio_String_pa; _temp1464.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp1459=( void*)& _temp1464; struct Cyc_Stdio_String_pa_struct
_temp1463; _temp1463.tag= Cyc_Stdio_String_pa; _temp1463.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null((( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);{
void* _temp1460=( void*)& _temp1463; void* _temp1457[ 2u]={ _temp1459, _temp1460};
struct _tagged_arr _temp1458={( void*) _temp1457,( void*) _temp1457,( void*)(
_temp1457 + 2u)}; _temp1456( _temp1461, _temp1462, _temp1458);}}}); return 0;}}}
return 1;} int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e){ if( ! Cyc_Tcutil_coerce_sint_typ( te, e)){ void* _temp1465= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _LL1467: if((
unsigned int) _temp1465 > 4u?*(( int*) _temp1465) == Cyc_Absyn_PointerType: 0){
goto _LL1468;} else{ goto _LL1469;} _LL1469: goto _LL1470; _LL1468: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); goto _LL1466; _LL1470: return 0; _LL1466:;} return 1;}
int Cyc_Tcutil_is_integral_type( void* t){ void* _temp1471= Cyc_Tcutil_compress(
t); _LL1473: if(( unsigned int) _temp1471 > 4u?*(( int*) _temp1471) == Cyc_Absyn_IntType:
0){ goto _LL1474;} else{ goto _LL1475;} _LL1475: if(( unsigned int) _temp1471 >
4u?*(( int*) _temp1471) == Cyc_Absyn_EnumType: 0){ goto _LL1476;} else{ goto
_LL1477;} _LL1477: goto _LL1478; _LL1474: return 1; _LL1476: return 1; _LL1478:
return 0; _LL1472:;} int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_uint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){ if( Cyc_Tcutil_will_lose_precision((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_uint_t)){({
void(* _temp1479)( struct Cyc_Position_Segment* sg, struct _tagged_arr fmt,
struct _tagged_arr ap)= Cyc_Tcutil_warn; struct Cyc_Position_Segment* _temp1482=
e->loc; struct _tagged_arr _temp1483= _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u); void* _temp1480[ 0u]={}; struct _tagged_arr
_temp1481={( void*) _temp1480,( void*) _temp1480,( void*)( _temp1480 + 0u)};
_temp1479( _temp1482, _temp1483, _temp1481);});} Cyc_Tcutil_unchecked_cast( te,
e, Cyc_Absyn_uint_t); return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;}
if( Cyc_Tcutil_is_integral_type(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){
if( Cyc_Tcutil_will_lose_precision(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, Cyc_Absyn_sint_t)){({ void(* _temp1484)( struct Cyc_Position_Segment*
sg, struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_warn; struct Cyc_Position_Segment*
_temp1487= e->loc; struct _tagged_arr _temp1488= _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u); void* _temp1485[ 0u]={}; struct _tagged_arr
_temp1486={( void*) _temp1485,( void*) _temp1485,( void*)( _temp1485 + 0u)};
_temp1484( _temp1487, _temp1488, _temp1486);});} Cyc_Tcutil_unchecked_cast( te,
e, Cyc_Absyn_sint_t); return 1;} return 0;} int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8
_temp1490=({ struct _tuple8 _temp1489; _temp1489.f1= t1; _temp1489.f2= t2;
_temp1489;}); void* _temp1502; struct Cyc_Absyn_PtrInfo _temp1504; void*
_temp1506; struct Cyc_Absyn_PtrInfo _temp1508; void* _temp1510; struct Cyc_Absyn_Exp*
_temp1512; struct Cyc_Absyn_Tqual _temp1514; void* _temp1516; void* _temp1518;
struct Cyc_Absyn_Exp* _temp1520; struct Cyc_Absyn_Tqual _temp1522; void*
_temp1524; void* _temp1526; struct Cyc_Absyn_TunionInfo _temp1528; struct Cyc_List_List*
_temp1530; void* _temp1532; struct Cyc_Absyn_Tuniondecl* _temp1534; void*
_temp1536; struct Cyc_Absyn_TunionFieldInfo _temp1538; struct Cyc_List_List*
_temp1540; void* _temp1542; struct Cyc_Absyn_Tunionfield* _temp1544; struct Cyc_Absyn_Tuniondecl*
_temp1546; void* _temp1548; struct Cyc_Absyn_TunionInfo _temp1550; void*
_temp1552; struct Cyc_List_List* _temp1554; void* _temp1556; struct Cyc_Absyn_Tuniondecl*
_temp1558; void* _temp1560; struct Cyc_Absyn_PtrInfo _temp1562; struct Cyc_Absyn_Conref*
_temp1564; struct Cyc_Absyn_Tqual _temp1566; struct Cyc_Absyn_Conref* _temp1568;
void* _temp1570; void* _temp1572; _LL1492: _LL1507: _temp1506= _temp1490.f1; if((
unsigned int) _temp1506 > 4u?*(( int*) _temp1506) == Cyc_Absyn_PointerType: 0){
_LL1509: _temp1508=(( struct Cyc_Absyn_PointerType_struct*) _temp1506)->f1; goto
_LL1503;} else{ goto _LL1494;} _LL1503: _temp1502= _temp1490.f2; if((
unsigned int) _temp1502 > 4u?*(( int*) _temp1502) == Cyc_Absyn_PointerType: 0){
_LL1505: _temp1504=(( struct Cyc_Absyn_PointerType_struct*) _temp1502)->f1; goto
_LL1493;} else{ goto _LL1494;} _LL1494: _LL1519: _temp1518= _temp1490.f1; if((
unsigned int) _temp1518 > 4u?*(( int*) _temp1518) == Cyc_Absyn_ArrayType: 0){
_LL1525: _temp1524=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1518)->f1;
goto _LL1523; _LL1523: _temp1522=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1518)->f2; goto _LL1521; _LL1521: _temp1520=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1518)->f3; goto _LL1511;} else{ goto _LL1496;} _LL1511: _temp1510=
_temp1490.f2; if(( unsigned int) _temp1510 > 4u?*(( int*) _temp1510) == Cyc_Absyn_ArrayType:
0){ _LL1517: _temp1516=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1510)->f1;
goto _LL1515; _LL1515: _temp1514=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1510)->f2; goto _LL1513; _LL1513: _temp1512=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1510)->f3; goto _LL1495;} else{ goto _LL1496;} _LL1496: _LL1537: _temp1536=
_temp1490.f1; if(( unsigned int) _temp1536 > 4u?*(( int*) _temp1536) == Cyc_Absyn_TunionFieldType:
0){ _LL1539: _temp1538=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1536)->f1;
_LL1543: _temp1542=( void*) _temp1538.field_info; if(*(( int*) _temp1542) == Cyc_Absyn_KnownTunionfield){
_LL1547: _temp1546=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1542)->f1;
goto _LL1545; _LL1545: _temp1544=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1542)->f2; goto _LL1541;} else{ goto _LL1498;} _LL1541: _temp1540=
_temp1538.targs; goto _LL1527;} else{ goto _LL1498;} _LL1527: _temp1526=
_temp1490.f2; if(( unsigned int) _temp1526 > 4u?*(( int*) _temp1526) == Cyc_Absyn_TunionType:
0){ _LL1529: _temp1528=(( struct Cyc_Absyn_TunionType_struct*) _temp1526)->f1;
_LL1533: _temp1532=( void*) _temp1528.tunion_info; if(*(( int*) _temp1532) ==
Cyc_Absyn_KnownTunion){ _LL1535: _temp1534=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1532)->f1; goto _LL1531;} else{ goto _LL1498;} _LL1531: _temp1530=
_temp1528.targs; goto _LL1497;} else{ goto _LL1498;} _LL1498: _LL1561: _temp1560=
_temp1490.f1; if(( unsigned int) _temp1560 > 4u?*(( int*) _temp1560) == Cyc_Absyn_PointerType:
0){ _LL1563: _temp1562=(( struct Cyc_Absyn_PointerType_struct*) _temp1560)->f1;
_LL1573: _temp1572=( void*) _temp1562.elt_typ; goto _LL1571; _LL1571: _temp1570=(
void*) _temp1562.rgn_typ; goto _LL1569; _LL1569: _temp1568= _temp1562.nullable;
goto _LL1567; _LL1567: _temp1566= _temp1562.tq; goto _LL1565; _LL1565: _temp1564=
_temp1562.bounds; goto _LL1549;} else{ goto _LL1500;} _LL1549: _temp1548=
_temp1490.f2; if(( unsigned int) _temp1548 > 4u?*(( int*) _temp1548) == Cyc_Absyn_TunionType:
0){ _LL1551: _temp1550=(( struct Cyc_Absyn_TunionType_struct*) _temp1548)->f1;
_LL1557: _temp1556=( void*) _temp1550.tunion_info; if(*(( int*) _temp1556) ==
Cyc_Absyn_KnownTunion){ _LL1559: _temp1558=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1556)->f1; goto _LL1555;} else{ goto _LL1500;} _LL1555: _temp1554=
_temp1550.targs; goto _LL1553; _LL1553: _temp1552=( void*) _temp1550.rgn; goto
_LL1499;} else{ goto _LL1500;} _LL1500: goto _LL1501; _LL1493: { int okay= 1;
if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1508.nullable, _temp1504.nullable)){
void* _temp1574=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp1508.nullable))->v; int _temp1580; _LL1576:
if(( unsigned int) _temp1574 > 1u?*(( int*) _temp1574) == Cyc_Absyn_Eq_constr: 0){
_LL1581: _temp1580=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1574)->f1;
goto _LL1577;} else{ goto _LL1578;} _LL1578: goto _LL1579; _LL1577: okay= !
_temp1580; goto _LL1575; _LL1579:( int) _throw(({ void*(* _temp1582)( struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_impos; struct _tagged_arr
_temp1585= _tag_arr("silent_castable conref not eq", sizeof( unsigned char), 30u);
void* _temp1583[ 0u]={}; struct _tagged_arr _temp1584={( void*) _temp1583,( void*)
_temp1583,( void*)( _temp1583 + 0u)}; _temp1582( _temp1585, _temp1584);}));
_LL1575:;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp, _temp1508.bounds,
_temp1504.bounds)){ struct _tuple8 _temp1587=({ struct _tuple8 _temp1586;
_temp1586.f1=( void*)( Cyc_Absyn_compress_conref( _temp1508.bounds))->v;
_temp1586.f2=( void*)( Cyc_Absyn_compress_conref( _temp1504.bounds))->v;
_temp1586;}); void* _temp1597; void* _temp1599; void* _temp1601; void* _temp1603;
void* _temp1605; void* _temp1607; struct Cyc_Absyn_Exp* _temp1609; void*
_temp1611; void* _temp1613; struct Cyc_Absyn_Exp* _temp1615; void* _temp1617;
void* _temp1619; void* _temp1621; void* _temp1623; _LL1589: _LL1602: _temp1601=
_temp1587.f1; if(( unsigned int) _temp1601 > 1u?*(( int*) _temp1601) == Cyc_Absyn_Eq_constr:
0){ _LL1604: _temp1603=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1601)->f1;
if(( unsigned int) _temp1603 > 1u?*(( int*) _temp1603) == Cyc_Absyn_Upper_b: 0){
goto _LL1598;} else{ goto _LL1591;}} else{ goto _LL1591;} _LL1598: _temp1597=
_temp1587.f2; if(( unsigned int) _temp1597 > 1u?*(( int*) _temp1597) == Cyc_Absyn_Eq_constr:
0){ _LL1600: _temp1599=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1597)->f1;
if( _temp1599 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1590;} else{ goto _LL1591;}}
else{ goto _LL1591;} _LL1591: _LL1612: _temp1611= _temp1587.f1; if((
unsigned int) _temp1611 > 1u?*(( int*) _temp1611) == Cyc_Absyn_Eq_constr: 0){
_LL1614: _temp1613=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1611)->f1;
if(( unsigned int) _temp1613 > 1u?*(( int*) _temp1613) == Cyc_Absyn_Upper_b: 0){
_LL1616: _temp1615=(( struct Cyc_Absyn_Upper_b_struct*) _temp1613)->f1; goto
_LL1606;} else{ goto _LL1593;}} else{ goto _LL1593;} _LL1606: _temp1605=
_temp1587.f2; if(( unsigned int) _temp1605 > 1u?*(( int*) _temp1605) == Cyc_Absyn_Eq_constr:
0){ _LL1608: _temp1607=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1605)->f1;
if(( unsigned int) _temp1607 > 1u?*(( int*) _temp1607) == Cyc_Absyn_Upper_b: 0){
_LL1610: _temp1609=(( struct Cyc_Absyn_Upper_b_struct*) _temp1607)->f1; goto
_LL1592;} else{ goto _LL1593;}} else{ goto _LL1593;} _LL1593: _LL1622: _temp1621=
_temp1587.f1; if(( unsigned int) _temp1621 > 1u?*(( int*) _temp1621) == Cyc_Absyn_Eq_constr:
0){ _LL1624: _temp1623=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1621)->f1;
if( _temp1623 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1618;} else{ goto _LL1595;}}
else{ goto _LL1595;} _LL1618: _temp1617= _temp1587.f2; if(( unsigned int)
_temp1617 > 1u?*(( int*) _temp1617) == Cyc_Absyn_Eq_constr: 0){ _LL1620:
_temp1619=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1617)->f1; if(
_temp1619 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1594;} else{ goto _LL1595;}}
else{ goto _LL1595;} _LL1595: goto _LL1596; _LL1590: okay= 1; goto _LL1588;
_LL1592: okay= okay? Cyc_Evexp_eval_const_uint_exp( _temp1615) >= Cyc_Evexp_eval_const_uint_exp(
_temp1609): 0;({ void(* _temp1625)( struct Cyc_Position_Segment* sg, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_warn; struct Cyc_Position_Segment*
_temp1628= loc; struct _tagged_arr _temp1629= _tag_arr("implicit cast to shorter array",
sizeof( unsigned char), 31u); void* _temp1626[ 0u]={}; struct _tagged_arr
_temp1627={( void*) _temp1626,( void*) _temp1626,( void*)( _temp1626 + 0u)};
_temp1625( _temp1628, _temp1629, _temp1627);}); goto _LL1588; _LL1594: okay= 1;
goto _LL1588; _LL1596: okay= 0; goto _LL1588; _LL1588:;} okay= okay? Cyc_Tcutil_unify((
void*) _temp1508.elt_typ,( void*) _temp1504.elt_typ): 0; okay= okay? Cyc_Tcutil_unify((
void*) _temp1508.rgn_typ,( void*) _temp1504.rgn_typ)? 1: Cyc_Tcenv_region_outlives(
te,( void*) _temp1508.rgn_typ,( void*) _temp1504.rgn_typ): 0; okay= okay? !(
_temp1508.tq).q_const? 1:( _temp1504.tq).q_const: 0; return okay;} _LL1495: {
int okay; okay=( _temp1520 != 0? _temp1512 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1520)) == Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1512)): 0; return( okay? Cyc_Tcutil_unify(
_temp1524, _temp1516): 0)? ! _temp1522.q_const? 1: _temp1514.q_const: 0;}
_LL1497: if(( _temp1546 == _temp1534? 1: Cyc_Absyn_qvar_cmp( _temp1546->name,
_temp1534->name) == 0)? _temp1544->typs == 0: 0){ for( 0; _temp1540 != 0?
_temp1530 != 0: 0;( _temp1540=(( struct Cyc_List_List*) _check_null( _temp1540))->tl,
_temp1530=(( struct Cyc_List_List*) _check_null( _temp1530))->tl)){ if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_List_List*) _check_null( _temp1540))->hd,( void*)(( struct
Cyc_List_List*) _check_null( _temp1530))->hd)){ break;}} if( _temp1540 == 0?
_temp1530 == 0: 0){ return 1;}} return 0; _LL1499:{ void* _temp1630= Cyc_Tcutil_compress(
_temp1572); struct Cyc_Absyn_TunionFieldInfo _temp1636; struct Cyc_List_List*
_temp1638; void* _temp1640; struct Cyc_Absyn_Tunionfield* _temp1642; struct Cyc_Absyn_Tuniondecl*
_temp1644; _LL1632: if(( unsigned int) _temp1630 > 4u?*(( int*) _temp1630) ==
Cyc_Absyn_TunionFieldType: 0){ _LL1637: _temp1636=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1630)->f1; _LL1641: _temp1640=( void*) _temp1636.field_info; if(*(( int*)
_temp1640) == Cyc_Absyn_KnownTunionfield){ _LL1645: _temp1644=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1640)->f1; goto _LL1643; _LL1643: _temp1642=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1640)->f2; goto _LL1639;} else{ goto _LL1634;} _LL1639: _temp1638=
_temp1636.targs; goto _LL1633;} else{ goto _LL1634;} _LL1634: goto _LL1635;
_LL1633: if( ! Cyc_Tcutil_unify( _temp1570, _temp1552)? ! Cyc_Tcenv_region_outlives(
te, _temp1570, _temp1552): 0){ return 0;} if( !(( int(*)( int(* cmp)( int, int),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp, _temp1568,(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
0))){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp, _temp1564,
Cyc_Absyn_new_conref( Cyc_Absyn_bounds_one))){ return 0;} if( Cyc_Absyn_qvar_cmp(
_temp1558->name, _temp1644->name) == 0? _temp1642->typs != 0: 0){ int okay= 1;
for( 0; _temp1638 != 0? _temp1554 != 0: 0;( _temp1638=(( struct Cyc_List_List*)
_check_null( _temp1638))->tl, _temp1554=(( struct Cyc_List_List*) _check_null(
_temp1554))->tl)){ if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_List_List*)
_check_null( _temp1638))->hd,( void*)(( struct Cyc_List_List*) _check_null(
_temp1554))->hd)){ okay= 0; break;}} if(( ! okay? 1: _temp1638 != 0)? 1:
_temp1554 != 0){ return 0;} return 1;} goto _LL1631; _LL1635: goto _LL1631;
_LL1631:;} return 0; _LL1501: return Cyc_Tcutil_unify( t1, t2); _LL1491:;}} int
Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void*
t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} if( Cyc_Tcutil_is_arithmetic_type(
t1)? Cyc_Tcutil_is_arithmetic_type( t2): 0){ if( Cyc_Tcutil_will_lose_precision(
t1, t2)){({ void(* _temp1646)( struct Cyc_Position_Segment* sg, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_warn; struct Cyc_Position_Segment*
_temp1649= e->loc; struct _tagged_arr _temp1650= _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u); void* _temp1647[ 0u]={}; struct _tagged_arr
_temp1648={( void*) _temp1647,( void*) _temp1647,( void*)( _temp1647 + 0u)};
_temp1646( _temp1649, _temp1650, _temp1648);});} Cyc_Tcutil_unchecked_cast( te,
e, t2); return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){
Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2);({ void(* _temp1651)(
struct Cyc_Position_Segment* sg, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_warn; struct Cyc_Position_Segment* _temp1656= e->loc; struct
_tagged_arr _temp1657= _tag_arr("implicit cast from %s to %s", sizeof(
unsigned char), 28u); struct Cyc_Stdio_String_pa_struct _temp1659; _temp1659.tag=
Cyc_Stdio_String_pa; _temp1659.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp1654=( void*)& _temp1659; struct Cyc_Stdio_String_pa_struct
_temp1658; _temp1658.tag= Cyc_Stdio_String_pa; _temp1658.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t2);{ void* _temp1655=( void*)& _temp1658; void*
_temp1652[ 2u]={ _temp1654, _temp1655}; struct _tagged_arr _temp1653={( void*)
_temp1652,( void*) _temp1652,( void*)( _temp1652 + 2u)}; _temp1651( _temp1656,
_temp1657, _temp1653);}}}); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg(
te, e, t);} static int Cyc_Tcutil_any_promote( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e){ void* old_typ= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); void* t;{ void* _temp1660= old_typ; void* _temp1672;
void* _temp1674; _LL1662: if(( unsigned int) _temp1660 > 4u?*(( int*) _temp1660)
== Cyc_Absyn_EnumType: 0){ goto _LL1663;} else{ goto _LL1664;} _LL1664: if((
unsigned int) _temp1660 > 4u?*(( int*) _temp1660) == Cyc_Absyn_IntType: 0){
_LL1675: _temp1674=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1660)->f1;
goto _LL1673; _LL1673: _temp1672=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1660)->f2; goto _LL1665;} else{ goto _LL1666;} _LL1666: if( _temp1660 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1667;} else{ goto _LL1668;} _LL1668: if(
_temp1660 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1669;} else{ goto _LL1670;}
_LL1670: goto _LL1671; _LL1663: t= Cyc_Absyn_sint_t; goto _LL1661; _LL1665:{
void* _temp1676= _temp1672; _LL1678: if( _temp1676 ==( void*) Cyc_Absyn_B1){
goto _LL1679;} else{ goto _LL1680;} _LL1680: if( _temp1676 ==( void*) Cyc_Absyn_B2){
goto _LL1681;} else{ goto _LL1682;} _LL1682: if( _temp1676 ==( void*) Cyc_Absyn_B4){
goto _LL1683;} else{ goto _LL1684;} _LL1684: if( _temp1676 ==( void*) Cyc_Absyn_B8){
goto _LL1685;} else{ goto _LL1677;} _LL1679: t= Cyc_Absyn_sint_t; goto _LL1677;
_LL1681: t= Cyc_Absyn_sint_t; goto _LL1677; _LL1683: t= _temp1674 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL1677; _LL1685: t= _temp1674 ==( void*)
Cyc_Absyn_Unsigned? Cyc_Absyn_ulonglong_t: Cyc_Absyn_slonglong_t; goto _LL1677;
_LL1677:;} goto _LL1661; _LL1667: t= Cyc_Absyn_float_t; goto _LL1661; _LL1669: t=
Cyc_Absyn_double_t; goto _LL1661; _LL1671: return 0; _LL1661:;} Cyc_Tcutil_unchecked_cast(
te, e, t); return 1;} int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){
return 1;} Cyc_Tcutil_any_promote( te, e); if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, t2)){ return 1;} else{ if( Cyc_Tcutil_silent_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{
if( Cyc_Tcutil_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e,
t2);({ void(* _temp1686)( struct Cyc_Position_Segment* sg, struct _tagged_arr
fmt, struct _tagged_arr ap)= Cyc_Tcutil_warn; struct Cyc_Position_Segment*
_temp1691= e->loc; struct _tagged_arr _temp1692= _tag_arr("implicit cast from %s to %s",
sizeof( unsigned char), 28u); struct Cyc_Stdio_String_pa_struct _temp1694;
_temp1694.tag= Cyc_Stdio_String_pa; _temp1694.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp1689=( void*)& _temp1694; struct Cyc_Stdio_String_pa_struct
_temp1693; _temp1693.tag= Cyc_Stdio_String_pa; _temp1693.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t2);{ void* _temp1690=( void*)& _temp1693; void*
_temp1687[ 2u]={ _temp1689, _temp1690}; struct _tagged_arr _temp1688={( void*)
_temp1687,( void*) _temp1687,( void*)( _temp1687 + 2u)}; _temp1686( _temp1691,
_temp1692, _temp1688);}}}); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerceable(
void* t){ void* _temp1695= Cyc_Tcutil_compress( t); _LL1697: if(( unsigned int)
_temp1695 > 4u?*(( int*) _temp1695) == Cyc_Absyn_IntType: 0){ goto _LL1698;}
else{ goto _LL1699;} _LL1699: if( _temp1695 ==( void*) Cyc_Absyn_FloatType){
goto _LL1700;} else{ goto _LL1701;} _LL1701: if( _temp1695 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1702;} else{ goto _LL1703;} _LL1703: goto _LL1704; _LL1698: return 1;
_LL1700: return 1; _LL1702: return 1; _LL1704: return 0; _LL1696:;} static
struct _tuple4* Cyc_Tcutil_flatten_typ_f( struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield*
x){ return({ struct _tuple4* _temp1705=( struct _tuple4*) GC_malloc( sizeof(
struct _tuple4)); _temp1705->f1= x->tq; _temp1705->f2= Cyc_Tcutil_substitute(
inst,( void*) x->type); _temp1705;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ(
struct Cyc_Tcenv_Tenv* te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void*
_temp1706= t1; struct Cyc_List_List* _temp1718; struct Cyc_Absyn_Structdecl**
_temp1720; struct Cyc_List_List* _temp1722; struct _tuple1* _temp1724; struct
Cyc_List_List* _temp1726; _LL1708: if( _temp1706 ==( void*) Cyc_Absyn_VoidType){
goto _LL1709;} else{ goto _LL1710;} _LL1710: if(( unsigned int) _temp1706 > 4u?*((
int*) _temp1706) == Cyc_Absyn_TupleType: 0){ _LL1719: _temp1718=(( struct Cyc_Absyn_TupleType_struct*)
_temp1706)->f1; goto _LL1711;} else{ goto _LL1712;} _LL1712: if(( unsigned int)
_temp1706 > 4u?*(( int*) _temp1706) == Cyc_Absyn_StructType: 0){ _LL1725:
_temp1724=(( struct Cyc_Absyn_StructType_struct*) _temp1706)->f1; goto _LL1723;
_LL1723: _temp1722=(( struct Cyc_Absyn_StructType_struct*) _temp1706)->f2; goto
_LL1721; _LL1721: _temp1720=(( struct Cyc_Absyn_StructType_struct*) _temp1706)->f3;
goto _LL1713;} else{ goto _LL1714;} _LL1714: if(( unsigned int) _temp1706 > 4u?*((
int*) _temp1706) == Cyc_Absyn_AnonStructType: 0){ _LL1727: _temp1726=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1706)->f1; goto _LL1715;} else{ goto
_LL1716;} _LL1716: goto _LL1717; _LL1709: return 0; _LL1711: return _temp1718;
_LL1713: if( _temp1720 == 0? 1: _temp1724 == 0){ return({ struct Cyc_List_List*
_temp1728=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1728->hd=( void*)({ struct _tuple4* _temp1729=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1729->f1= Cyc_Absyn_empty_tqual(); _temp1729->f2=
t1; _temp1729;}); _temp1728->tl= 0; _temp1728;});}{ struct Cyc_Absyn_Structdecl*
_temp1730=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp1720)); struct
_tuple1 _temp1731=*(( struct _tuple1*) _check_null( _temp1724)); struct Cyc_List_List*
_temp1732=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp1730->tvs, _temp1722); if( _temp1730->fields == 0){
return({ struct Cyc_List_List* _temp1733=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1733->hd=( void*)({ struct _tuple4*
_temp1734=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1734->f1=
Cyc_Absyn_empty_tqual(); _temp1734->f2= t1; _temp1734;}); _temp1733->tl= 0;
_temp1733;});} return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, _temp1732,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1730->fields))->v);}
_LL1715: return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, 0, _temp1726); _LL1717: return({
struct Cyc_List_List* _temp1735=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1735->hd=( void*)({ struct _tuple4* _temp1736=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1736->f1= Cyc_Absyn_empty_tqual();
_temp1736->f2= t1; _temp1736;}); _temp1735->tl= 0; _temp1735;}); _LL1707:;}}
static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2); static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;}{ struct Cyc_List_List* a= assume; for( 0; a != 0; a=(( struct
Cyc_List_List*) _check_null( a))->tl){ if( Cyc_Tcutil_unify( t1,(*(( struct
_tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f1)? Cyc_Tcutil_unify(
t2,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f2): 0){
return 1;}}} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct
_tuple8 _temp1738=({ struct _tuple8 _temp1737; _temp1737.f1= t1; _temp1737.f2=
t2; _temp1737;}); void* _temp1744; struct Cyc_Absyn_PtrInfo _temp1746; struct
Cyc_Absyn_Conref* _temp1748; struct Cyc_Absyn_Tqual _temp1750; struct Cyc_Absyn_Conref*
_temp1752; void* _temp1754; void* _temp1756; void* _temp1758; struct Cyc_Absyn_PtrInfo
_temp1760; struct Cyc_Absyn_Conref* _temp1762; struct Cyc_Absyn_Tqual _temp1764;
struct Cyc_Absyn_Conref* _temp1766; void* _temp1768; void* _temp1770; _LL1740:
_LL1759: _temp1758= _temp1738.f1; if(( unsigned int) _temp1758 > 4u?*(( int*)
_temp1758) == Cyc_Absyn_PointerType: 0){ _LL1761: _temp1760=(( struct Cyc_Absyn_PointerType_struct*)
_temp1758)->f1; _LL1771: _temp1770=( void*) _temp1760.elt_typ; goto _LL1769;
_LL1769: _temp1768=( void*) _temp1760.rgn_typ; goto _LL1767; _LL1767: _temp1766=
_temp1760.nullable; goto _LL1765; _LL1765: _temp1764= _temp1760.tq; goto _LL1763;
_LL1763: _temp1762= _temp1760.bounds; goto _LL1745;} else{ goto _LL1742;}
_LL1745: _temp1744= _temp1738.f2; if(( unsigned int) _temp1744 > 4u?*(( int*)
_temp1744) == Cyc_Absyn_PointerType: 0){ _LL1747: _temp1746=(( struct Cyc_Absyn_PointerType_struct*)
_temp1744)->f1; _LL1757: _temp1756=( void*) _temp1746.elt_typ; goto _LL1755;
_LL1755: _temp1754=( void*) _temp1746.rgn_typ; goto _LL1753; _LL1753: _temp1752=
_temp1746.nullable; goto _LL1751; _LL1751: _temp1750= _temp1746.tq; goto _LL1749;
_LL1749: _temp1748= _temp1746.bounds; goto _LL1741;} else{ goto _LL1742;}
_LL1742: goto _LL1743; _LL1741: if( _temp1764.q_const? ! _temp1750.q_const: 0){
return 0;} if(( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1766, _temp1752)?(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1766): 0)? !(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1752): 0){ return 0;} if( ! Cyc_Tcutil_unify( _temp1768, _temp1754)? ! Cyc_Tcenv_region_outlives(
te, _temp1768, _temp1754): 0){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp,
_temp1762, _temp1748)){ struct _tuple8 _temp1773=({ struct _tuple8 _temp1772;
_temp1772.f1= Cyc_Absyn_conref_val( _temp1762); _temp1772.f2= Cyc_Absyn_conref_val(
_temp1748); _temp1772;}); void* _temp1781; void* _temp1783; void* _temp1785;
struct Cyc_Absyn_Exp* _temp1787; void* _temp1789; struct Cyc_Absyn_Exp*
_temp1791; _LL1775: _LL1784: _temp1783= _temp1773.f1; if(( unsigned int)
_temp1783 > 1u?*(( int*) _temp1783) == Cyc_Absyn_Upper_b: 0){ goto _LL1782;}
else{ goto _LL1777;} _LL1782: _temp1781= _temp1773.f2; if( _temp1781 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL1776;} else{ goto _LL1777;} _LL1777: _LL1790:
_temp1789= _temp1773.f1; if(( unsigned int) _temp1789 > 1u?*(( int*) _temp1789)
== Cyc_Absyn_Upper_b: 0){ _LL1792: _temp1791=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1789)->f1; goto _LL1786;} else{ goto _LL1779;} _LL1786: _temp1785=
_temp1773.f2; if(( unsigned int) _temp1785 > 1u?*(( int*) _temp1785) == Cyc_Absyn_Upper_b:
0){ _LL1788: _temp1787=(( struct Cyc_Absyn_Upper_b_struct*) _temp1785)->f1; goto
_LL1778;} else{ goto _LL1779;} _LL1779: goto _LL1780; _LL1776: goto _LL1774;
_LL1778: if( Cyc_Evexp_eval_const_uint_exp( _temp1791) < Cyc_Evexp_eval_const_uint_exp(
_temp1787)){ return 0;} goto _LL1774; _LL1780: return 0; _LL1774:;} return Cyc_Tcutil_ptrsubtype(
te,({ struct Cyc_List_List* _temp1793=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1793->hd=( void*)({ struct _tuple8*
_temp1794=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp1794->f1=
t1; _temp1794->f2= t2; _temp1794;}); _temp1793->tl= assume; _temp1793;}),
_temp1770, _temp1756); _LL1743: return 0; _LL1739:;}} static int Cyc_Tcutil_isomorphic(
void* t1, void* t2){ struct _tuple8 _temp1796=({ struct _tuple8 _temp1795;
_temp1795.f1= Cyc_Tcutil_compress( t1); _temp1795.f2= Cyc_Tcutil_compress( t2);
_temp1795;}); void* _temp1802; void* _temp1804; void* _temp1806; void* _temp1808;
_LL1798: _LL1807: _temp1806= _temp1796.f1; if(( unsigned int) _temp1806 > 4u?*((
int*) _temp1806) == Cyc_Absyn_IntType: 0){ _LL1809: _temp1808=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1806)->f2; goto _LL1803;} else{ goto _LL1800;}
_LL1803: _temp1802= _temp1796.f2; if(( unsigned int) _temp1802 > 4u?*(( int*)
_temp1802) == Cyc_Absyn_IntType: 0){ _LL1805: _temp1804=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1802)->f2; goto _LL1799;} else{ goto _LL1800;} _LL1800: goto _LL1801;
_LL1799: return _temp1808 == _temp1804; _LL1801: return 0; _LL1797:;} static int
Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* assume,
void* t1, void* t2){ struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ( te, t1);
struct Cyc_List_List* tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0; tqs2 != 0;(
tqs2=(( struct Cyc_List_List*) _check_null( tqs2))->tl, tqs1=(( struct Cyc_List_List*)
_check_null( tqs1))->tl)){ if( tqs1 == 0){ return 0;}{ struct _tuple4 _temp1812;
void* _temp1813; struct Cyc_Absyn_Tqual _temp1815; struct _tuple4* _temp1810=(
struct _tuple4*)(( struct Cyc_List_List*) _check_null( tqs1))->hd; _temp1812=*
_temp1810; _LL1816: _temp1815= _temp1812.f1; goto _LL1814; _LL1814: _temp1813=
_temp1812.f2; goto _LL1811; _LL1811: { struct _tuple4 _temp1819; void* _temp1820;
struct Cyc_Absyn_Tqual _temp1822; struct _tuple4* _temp1817=( struct _tuple4*)((
struct Cyc_List_List*) _check_null( tqs2))->hd; _temp1819=* _temp1817; _LL1823:
_temp1822= _temp1819.f1; goto _LL1821; _LL1821: _temp1820= _temp1819.f2; goto
_LL1818; _LL1818: if( _temp1822.q_const? Cyc_Tcutil_subtype( te, assume,
_temp1813, _temp1820): 0){ continue;} else{ if( Cyc_Tcutil_unify( _temp1813,
_temp1820)){ continue;} else{ if( Cyc_Tcutil_isomorphic( _temp1813, _temp1820)){
continue;} else{ return 0;}}}}}} return 1;} int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void* t1, void* t2){ if( Cyc_Tcutil_unify(
t1, t2)){ return 1;} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
void* _temp1824= t2; struct Cyc_Absyn_Uniondecl** _temp1832; struct Cyc_List_List*
_temp1834; void* _temp1836; _LL1826: if(( unsigned int) _temp1824 > 4u?*(( int*)
_temp1824) == Cyc_Absyn_UnionType: 0){ _LL1835: _temp1834=(( struct Cyc_Absyn_UnionType_struct*)
_temp1824)->f2; goto _LL1833; _LL1833: _temp1832=(( struct Cyc_Absyn_UnionType_struct*)
_temp1824)->f3; goto _LL1827;} else{ goto _LL1828;} _LL1828: if(( unsigned int)
_temp1824 > 4u?*(( int*) _temp1824) == Cyc_Absyn_IntType: 0){ _LL1837: _temp1836=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1824)->f2; if( _temp1836 ==(
void*) Cyc_Absyn_B4){ goto _LL1829;} else{ goto _LL1830;}} else{ goto _LL1830;}
_LL1830: goto _LL1831; _LL1827: if( _temp1832 == 0){ goto _LL1825;}{ struct Cyc_Absyn_Uniondecl*
_temp1838=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp1832)); if(
_temp1838->fields == 0){ goto _LL1825;}{ struct Cyc_List_List* _temp1839=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1838->tvs, _temp1834);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1838->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ if( Cyc_Tcutil_unify( t1, Cyc_Tcutil_substitute(
_temp1839,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ return 1;}}} goto _LL1825;}} _LL1829: if( Cyc_Tcutil_typ_kind(
t2) ==( void*) Cyc_Absyn_BoxKind){ return 1;} goto _LL1825; _LL1831: goto
_LL1825; _LL1825:;}{ void* _temp1840= t1; struct Cyc_Absyn_PtrInfo _temp1856;
struct Cyc_Absyn_Conref* _temp1858; struct Cyc_Absyn_Tqual _temp1860; struct Cyc_Absyn_Conref*
_temp1862; void* _temp1864; void* _temp1866; struct Cyc_Absyn_Exp* _temp1868;
struct Cyc_Absyn_Tqual _temp1870; void* _temp1872; struct Cyc_Absyn_Enumdecl*
_temp1874; _LL1842: if(( unsigned int) _temp1840 > 4u?*(( int*) _temp1840) ==
Cyc_Absyn_PointerType: 0){ _LL1857: _temp1856=(( struct Cyc_Absyn_PointerType_struct*)
_temp1840)->f1; _LL1867: _temp1866=( void*) _temp1856.elt_typ; goto _LL1865;
_LL1865: _temp1864=( void*) _temp1856.rgn_typ; goto _LL1863; _LL1863: _temp1862=
_temp1856.nullable; goto _LL1861; _LL1861: _temp1860= _temp1856.tq; goto _LL1859;
_LL1859: _temp1858= _temp1856.bounds; goto _LL1843;} else{ goto _LL1844;}
_LL1844: if(( unsigned int) _temp1840 > 4u?*(( int*) _temp1840) == Cyc_Absyn_ArrayType:
0){ _LL1873: _temp1872=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1840)->f1;
goto _LL1871; _LL1871: _temp1870=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1840)->f2; goto _LL1869; _LL1869: _temp1868=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1840)->f3; goto _LL1845;} else{ goto _LL1846;} _LL1846: if(( unsigned int)
_temp1840 > 4u?*(( int*) _temp1840) == Cyc_Absyn_EnumType: 0){ _LL1875:
_temp1874=(( struct Cyc_Absyn_EnumType_struct*) _temp1840)->f2; goto _LL1847;}
else{ goto _LL1848;} _LL1848: if(( unsigned int) _temp1840 > 4u?*(( int*)
_temp1840) == Cyc_Absyn_IntType: 0){ goto _LL1849;} else{ goto _LL1850;} _LL1850:
if( _temp1840 ==( void*) Cyc_Absyn_FloatType){ goto _LL1851;} else{ goto _LL1852;}
_LL1852: if( _temp1840 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1853;} else{
goto _LL1854;} _LL1854: goto _LL1855; _LL1843:{ void* _temp1876= t2; struct Cyc_Absyn_PtrInfo
_temp1882; struct Cyc_Absyn_Conref* _temp1884; struct Cyc_Absyn_Tqual _temp1886;
struct Cyc_Absyn_Conref* _temp1888; void* _temp1890; void* _temp1892; _LL1878:
if(( unsigned int) _temp1876 > 4u?*(( int*) _temp1876) == Cyc_Absyn_PointerType:
0){ _LL1883: _temp1882=(( struct Cyc_Absyn_PointerType_struct*) _temp1876)->f1;
_LL1893: _temp1892=( void*) _temp1882.elt_typ; goto _LL1891; _LL1891: _temp1890=(
void*) _temp1882.rgn_typ; goto _LL1889; _LL1889: _temp1888= _temp1882.nullable;
goto _LL1887; _LL1887: _temp1886= _temp1882.tq; goto _LL1885; _LL1885: _temp1884=
_temp1882.bounds; goto _LL1879;} else{ goto _LL1880;} _LL1880: goto _LL1881;
_LL1879: { struct Cyc_List_List* _temp1894=({ struct Cyc_List_List* _temp1911=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1911->hd=(
void*)({ struct _tuple8* _temp1912=( struct _tuple8*) GC_malloc( sizeof( struct
_tuple8)); _temp1912->f1= t1; _temp1912->f2= t2; _temp1912;}); _temp1911->tl= 0;
_temp1911;}); int _temp1895= Cyc_Tcutil_ptrsubtype( te, _temp1894, _temp1866,
_temp1892)? ! _temp1860.q_const? 1: _temp1886.q_const: 0; int bounds_ok= Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_boundscmp, _temp1858, _temp1884); if( ! bounds_ok){ struct _tuple8
_temp1897=({ struct _tuple8 _temp1896; _temp1896.f1= Cyc_Absyn_conref_val(
_temp1858); _temp1896.f2= Cyc_Absyn_conref_val( _temp1884); _temp1896;}); void*
_temp1903; struct Cyc_Absyn_Exp* _temp1905; void* _temp1907; struct Cyc_Absyn_Exp*
_temp1909; _LL1899: _LL1908: _temp1907= _temp1897.f1; if(( unsigned int)
_temp1907 > 1u?*(( int*) _temp1907) == Cyc_Absyn_Upper_b: 0){ _LL1910: _temp1909=((
struct Cyc_Absyn_Upper_b_struct*) _temp1907)->f1; goto _LL1904;} else{ goto
_LL1901;} _LL1904: _temp1903= _temp1897.f2; if(( unsigned int) _temp1903 > 1u?*((
int*) _temp1903) == Cyc_Absyn_Upper_b: 0){ _LL1906: _temp1905=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1903)->f1; goto _LL1900;} else{ goto _LL1901;} _LL1901: goto _LL1902;
_LL1900: if( Cyc_Evexp_eval_const_uint_exp( _temp1909) >= Cyc_Evexp_eval_const_uint_exp(
_temp1905)){ bounds_ok= 1;} goto _LL1898; _LL1902: bounds_ok= 1; goto _LL1898;
_LL1898:;} return( bounds_ok? _temp1895: 0)? Cyc_Tcutil_unify( _temp1864,
_temp1890)? 1: Cyc_Tcenv_region_outlives( te, _temp1864, _temp1890): 0;} _LL1881:
goto _LL1877; _LL1877:;} return 0; _LL1845:{ void* _temp1913= t2; struct Cyc_Absyn_Exp*
_temp1919; struct Cyc_Absyn_Tqual _temp1921; void* _temp1923; _LL1915: if((
unsigned int) _temp1913 > 4u?*(( int*) _temp1913) == Cyc_Absyn_ArrayType: 0){
_LL1924: _temp1923=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1913)->f1;
goto _LL1922; _LL1922: _temp1921=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1913)->f2; goto _LL1920; _LL1920: _temp1919=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1913)->f3; goto _LL1916;} else{ goto _LL1917;} _LL1917: goto _LL1918;
_LL1916: { int okay; okay=( _temp1868 != 0? _temp1919 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1868)) >= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1919)): 0; return( okay? Cyc_Tcutil_unify(
_temp1872, _temp1923): 0)? ! _temp1870.q_const? 1: _temp1921.q_const: 0;}
_LL1918: return 0; _LL1914:;} return 0; _LL1847:{ void* _temp1925= t2; struct
Cyc_Absyn_Enumdecl* _temp1931; _LL1927: if(( unsigned int) _temp1925 > 4u?*((
int*) _temp1925) == Cyc_Absyn_EnumType: 0){ _LL1932: _temp1931=(( struct Cyc_Absyn_EnumType_struct*)
_temp1925)->f2; goto _LL1928;} else{ goto _LL1929;} _LL1929: goto _LL1930;
_LL1928: if(((( struct Cyc_Absyn_Enumdecl*) _check_null( _temp1874))->fields !=
0?(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp1931))->fields != 0: 0)?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Enumdecl*) _check_null(
_temp1874))->fields))->v) >=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Enumdecl*)
_check_null( _temp1931))->fields))->v): 0){ return 1;} goto _LL1926; _LL1930:
goto _LL1926; _LL1926:;} goto _LL1849; _LL1849: goto _LL1851; _LL1851: goto
_LL1853; _LL1853: return Cyc_Tcutil_coerceable( t2); _LL1855: return 0; _LL1841:;}}
void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t){ if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, t)){ struct Cyc_Absyn_Exp* _temp1933= Cyc_Absyn_copy_exp( e);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct* _temp1934=(
struct Cyc_Absyn_Cast_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp1934[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp1935; _temp1935.tag= Cyc_Absyn_Cast_e;
_temp1935.f1=( void*) t; _temp1935.f2= _temp1933; _temp1935;}); _temp1934;})));
e->topt=({ struct Cyc_Core_Opt* _temp1936=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1936->v=( void*) t; _temp1936;});}} int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp* e){ void* _temp1937= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v); _LL1939: if(( unsigned int) _temp1937
> 4u?*(( int*) _temp1937) == Cyc_Absyn_EnumType: 0){ goto _LL1940;} else{ goto
_LL1941;} _LL1941: if(( unsigned int) _temp1937 > 4u?*(( int*) _temp1937) == Cyc_Absyn_IntType:
0){ goto _LL1942;} else{ goto _LL1943;} _LL1943: if(( unsigned int) _temp1937 >
4u?*(( int*) _temp1937) == Cyc_Absyn_Evar: 0){ goto _LL1944;} else{ goto _LL1945;}
_LL1945: goto _LL1946; _LL1940: goto _LL1942; _LL1942: return 1; _LL1944: if(
Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){
return 1;} return 0; _LL1946: return 0; _LL1938:;} int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_is_integral( e)){ return 1;}{ void*
_temp1947= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_LL1949: if( _temp1947 ==( void*) Cyc_Absyn_FloatType){ goto _LL1950;} else{
goto _LL1951;} _LL1951: if( _temp1947 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL1952;} else{ goto _LL1953;} _LL1953: goto _LL1954; _LL1950: goto _LL1952;
_LL1952: return 1; _LL1954: return 0; _LL1948:;}} int Cyc_Tcutil_is_function_type(
void* t){ void* _temp1955= Cyc_Tcutil_compress( t); _LL1957: if(( unsigned int)
_temp1955 > 4u?*(( int*) _temp1955) == Cyc_Absyn_FnType: 0){ goto _LL1958;}
else{ goto _LL1959;} _LL1959: goto _LL1960; _LL1958: return 1; _LL1960: return 0;
_LL1956:;} void* Cyc_Tcutil_max_arithmetic_type( void* t1, void* t2){ struct
_tuple8 _temp1962=({ struct _tuple8 _temp1961; _temp1961.f1= t1; _temp1961.f2=
t2; _temp1961;}); void* _temp1986; void* _temp1988; void* _temp1990; void*
_temp1992; void* _temp1994; void* _temp1996; void* _temp1998; void* _temp2000;
void* _temp2002; void* _temp2004; void* _temp2006; void* _temp2008; void*
_temp2010; void* _temp2012; void* _temp2014; void* _temp2016; void* _temp2018;
void* _temp2020; void* _temp2022; void* _temp2024; void* _temp2026; void*
_temp2028; _LL1964: _LL1987: _temp1986= _temp1962.f1; if( _temp1986 ==( void*)
Cyc_Absyn_DoubleType){ goto _LL1965;} else{ goto _LL1966;} _LL1966: _LL1989:
_temp1988= _temp1962.f2; if( _temp1988 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL1967;} else{ goto _LL1968;} _LL1968: _LL1991: _temp1990= _temp1962.f1; if(
_temp1990 ==( void*) Cyc_Absyn_FloatType){ goto _LL1969;} else{ goto _LL1970;}
_LL1970: _LL1993: _temp1992= _temp1962.f2; if( _temp1992 ==( void*) Cyc_Absyn_FloatType){
goto _LL1971;} else{ goto _LL1972;} _LL1972: _LL1995: _temp1994= _temp1962.f1;
if(( unsigned int) _temp1994 > 4u?*(( int*) _temp1994) == Cyc_Absyn_IntType: 0){
_LL1999: _temp1998=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1994)->f1;
if( _temp1998 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1997;} else{ goto _LL1974;}
_LL1997: _temp1996=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1994)->f2;
if( _temp1996 ==( void*) Cyc_Absyn_B8){ goto _LL1973;} else{ goto _LL1974;}}
else{ goto _LL1974;} _LL1974: _LL2001: _temp2000= _temp1962.f2; if((
unsigned int) _temp2000 > 4u?*(( int*) _temp2000) == Cyc_Absyn_IntType: 0){
_LL2005: _temp2004=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2000)->f1;
if( _temp2004 ==( void*) Cyc_Absyn_Unsigned){ goto _LL2003;} else{ goto _LL1976;}
_LL2003: _temp2002=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2000)->f2;
if( _temp2002 ==( void*) Cyc_Absyn_B8){ goto _LL1975;} else{ goto _LL1976;}}
else{ goto _LL1976;} _LL1976: _LL2007: _temp2006= _temp1962.f1; if((
unsigned int) _temp2006 > 4u?*(( int*) _temp2006) == Cyc_Absyn_IntType: 0){
_LL2011: _temp2010=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2006)->f1;
if( _temp2010 ==( void*) Cyc_Absyn_Signed){ goto _LL2009;} else{ goto _LL1978;}
_LL2009: _temp2008=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2006)->f2;
if( _temp2008 ==( void*) Cyc_Absyn_B8){ goto _LL1977;} else{ goto _LL1978;}}
else{ goto _LL1978;} _LL1978: _LL2013: _temp2012= _temp1962.f2; if((
unsigned int) _temp2012 > 4u?*(( int*) _temp2012) == Cyc_Absyn_IntType: 0){
_LL2017: _temp2016=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2012)->f1;
if( _temp2016 ==( void*) Cyc_Absyn_Signed){ goto _LL2015;} else{ goto _LL1980;}
_LL2015: _temp2014=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2012)->f2;
if( _temp2014 ==( void*) Cyc_Absyn_B8){ goto _LL1979;} else{ goto _LL1980;}}
else{ goto _LL1980;} _LL1980: _LL2019: _temp2018= _temp1962.f1; if((
unsigned int) _temp2018 > 4u?*(( int*) _temp2018) == Cyc_Absyn_IntType: 0){
_LL2023: _temp2022=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2018)->f1;
if( _temp2022 ==( void*) Cyc_Absyn_Unsigned){ goto _LL2021;} else{ goto _LL1982;}
_LL2021: _temp2020=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2018)->f2;
if( _temp2020 ==( void*) Cyc_Absyn_B4){ goto _LL1981;} else{ goto _LL1982;}}
else{ goto _LL1982;} _LL1982: _LL2025: _temp2024= _temp1962.f2; if((
unsigned int) _temp2024 > 4u?*(( int*) _temp2024) == Cyc_Absyn_IntType: 0){
_LL2029: _temp2028=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2024)->f1;
if( _temp2028 ==( void*) Cyc_Absyn_Unsigned){ goto _LL2027;} else{ goto _LL1984;}
_LL2027: _temp2026=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2024)->f2;
if( _temp2026 ==( void*) Cyc_Absyn_B4){ goto _LL1983;} else{ goto _LL1984;}}
else{ goto _LL1984;} _LL1984: goto _LL1985; _LL1965: goto _LL1967; _LL1967:
return( void*) Cyc_Absyn_DoubleType; _LL1969: goto _LL1971; _LL1971: return(
void*) Cyc_Absyn_FloatType; _LL1973: goto _LL1975; _LL1975: return Cyc_Absyn_ulonglong_t;
_LL1977: goto _LL1979; _LL1979: return Cyc_Absyn_slonglong_t; _LL1981: goto
_LL1983; _LL1983: return Cyc_Absyn_uint_t; _LL1985: return Cyc_Absyn_sint_t;
_LL1963:;} void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp* e){ void*
_temp2030=( void*) e->r; struct Cyc_Core_Opt* _temp2036; _LL2032: if(*(( int*)
_temp2030) == Cyc_Absyn_AssignOp_e){ _LL2037: _temp2036=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp2030)->f2; if( _temp2036 == 0){ goto _LL2033;} else{ goto _LL2034;}} else{
goto _LL2034;} _LL2034: goto _LL2035; _LL2033:({ void(* _temp2038)( struct Cyc_Position_Segment*
sg, struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_warn; struct Cyc_Position_Segment*
_temp2041= e->loc; struct _tagged_arr _temp2042= _tag_arr("assignment in test",
sizeof( unsigned char), 19u); void* _temp2039[ 0u]={}; struct _tagged_arr
_temp2040={( void*) _temp2039,( void*) _temp2039,( void*)( _temp2039 + 0u)};
_temp2038( _temp2041, _temp2042, _temp2040);}); goto _LL2031; _LL2035: goto
_LL2031; _LL2031:;} static int Cyc_Tcutil_constrain_kinds( struct Cyc_Absyn_Conref*
c1, struct Cyc_Absyn_Conref* c2){ c1= Cyc_Absyn_compress_conref( c1); c2= Cyc_Absyn_compress_conref(
c2); if( c1 == c2){ return 1;} if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr){(
void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp2043=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp2043[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp2044; _temp2044.tag= Cyc_Absyn_Forward_constr; _temp2044.f1= c2; _temp2044;});
_temp2043;}))); return 1;} else{ if(( void*) c2->v ==( void*) Cyc_Absyn_No_constr){(
void*)( c2->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp2045=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp2045[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp2046; _temp2046.tag= Cyc_Absyn_Forward_constr; _temp2046.f1= c1; _temp2046;});
_temp2045;}))); return 1;} else{ void* k1= Cyc_Absyn_conref_val( c1); void* k2=
Cyc_Absyn_conref_val( c2); if( Cyc_Tcutil_kind_leq( k1, k2)){( void*)( c2->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2047=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2047[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2048; _temp2048.tag= Cyc_Absyn_Forward_constr;
_temp2048.f1= c1; _temp2048;}); _temp2047;}))); return 1;} else{ if( Cyc_Tcutil_kind_leq(
k2, k1)){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp2049=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp2049[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp2050; _temp2050.tag= Cyc_Absyn_Forward_constr; _temp2050.f1= c2; _temp2050;});
_temp2049;}))); return 1;} else{ return 0;}}}}} static int Cyc_Tcutil_tvar_id_counter=
0; int* Cyc_Tcutil_new_tvar_id(){ return({ int* _temp2051=( int*)
GC_malloc_atomic( sizeof( int)); _temp2051[ 0]= Cyc_Tcutil_tvar_id_counter ++;
_temp2051;});} static int Cyc_Tcutil_tvar_counter= 0; struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar(
void* k){ int i= Cyc_Tcutil_tvar_counter ++; struct _tagged_arr s=( struct
_tagged_arr)({ struct _tagged_arr(* _temp2054)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp2058= _tag_arr("#%d",
sizeof( unsigned char), 4u); struct Cyc_Stdio_Int_pa_struct _temp2059; _temp2059.tag=
Cyc_Stdio_Int_pa; _temp2059.f1=( int)(( unsigned int) i);{ void* _temp2057=(
void*)& _temp2059; void* _temp2055[ 1u]={ _temp2057}; struct _tagged_arr
_temp2056={( void*) _temp2055,( void*) _temp2055,( void*)( _temp2055 + 1u)};
_temp2054( _temp2058, _temp2056);}}); return({ struct Cyc_Absyn_Tvar* _temp2052=(
struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2052->name=({
struct _tagged_arr* _temp2053=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp2053[ 0]= s; _temp2053;}); _temp2052->identity= 0;
_temp2052->kind= Cyc_Absyn_new_conref( k); _temp2052;});} int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar* t){ struct _tagged_arr _temp2060=* t->name; return*((
const unsigned char*) _check_unknown_subscript( _temp2060, sizeof( unsigned char),
0)) =='#';} void Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar* t){ if( !
Cyc_Tcutil_is_temp_tvar( t)){ return;}{ struct _tagged_arr _temp2061= Cyc_String_strconcat(
_tag_arr("`", sizeof( unsigned char), 2u),* t->name);*(( unsigned char*)
_check_unknown_subscript( _temp2061, sizeof( unsigned char), 1))='t'; t->name=({
struct _tagged_arr* _temp2062=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp2062[ 0]=( struct _tagged_arr) _temp2061; _temp2062;});}}
struct _tuple13{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; }
; static struct _tuple2* Cyc_Tcutil_fndecl2typ_f( struct _tuple13* x){ return({
struct _tuple2* _temp2063=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2063->f1=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp2064=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2064->v=( void*)(*
x).f1; _temp2064;}); _temp2063->f2=(* x).f2; _temp2063->f3=(* x).f3; _temp2063;});}
void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl* fd){ if( fd->cached_typ ==
0){ struct Cyc_List_List* _temp2065= 0;{ struct Cyc_List_List* atts= fd->attributes;
for( 0; atts != 0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ if(
Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*) _check_null( atts))->hd)){
_temp2065=({ struct Cyc_List_List* _temp2066=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2066->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); _temp2066->tl= _temp2065; _temp2066;});}}} return(
void*)({ struct Cyc_Absyn_FnType_struct* _temp2067=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2067[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2068; _temp2068.tag= Cyc_Absyn_FnType; _temp2068.f1=({ struct Cyc_Absyn_FnInfo
_temp2069; _temp2069.tvars= fd->tvs; _temp2069.effect= fd->effect; _temp2069.ret_typ=(
void*)(( void*) fd->ret_type); _temp2069.args=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple13*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_fndecl2typ_f, fd->args); _temp2069.c_varargs= fd->c_varargs;
_temp2069.cyc_varargs= fd->cyc_varargs; _temp2069.rgn_po= fd->rgn_po; _temp2069.attributes=
_temp2065; _temp2069;}); _temp2068;}); _temp2067;});} return( void*)(( struct
Cyc_Core_Opt*) _check_null( fd->cached_typ))->v;} struct _tuple14{ void* f1;
struct Cyc_Absyn_Tqual f2; void* f3; } ; static void* Cyc_Tcutil_fst_fdarg(
struct _tuple14* t){ return(* t).f1;} void* Cyc_Tcutil_snd_tqt( struct _tuple4*
t){ return(* t).f2;} static struct _tuple4* Cyc_Tcutil_map2_tq( struct _tuple4*
pr, void* t){ return({ struct _tuple4* _temp2070=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp2070->f1=(* pr).f1; _temp2070->f2= t; _temp2070;});}
void Cyc_Tcutil_check_fndecl_valid_type( struct Cyc_Position_Segment* loc,
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl* fd){ void* t= Cyc_Tcutil_fndecl2typ(
fd); Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);{ void* _temp2071= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_FnInfo _temp2077; struct Cyc_List_List* _temp2079; struct
Cyc_List_List* _temp2081; struct Cyc_Absyn_VarargInfo* _temp2083; int _temp2085;
struct Cyc_List_List* _temp2087; void* _temp2089; struct Cyc_Core_Opt* _temp2091;
struct Cyc_List_List* _temp2093; _LL2073: if(( unsigned int) _temp2071 > 4u?*((
int*) _temp2071) == Cyc_Absyn_FnType: 0){ _LL2078: _temp2077=(( struct Cyc_Absyn_FnType_struct*)
_temp2071)->f1; _LL2094: _temp2093= _temp2077.tvars; goto _LL2092; _LL2092:
_temp2091= _temp2077.effect; goto _LL2090; _LL2090: _temp2089=( void*) _temp2077.ret_typ;
goto _LL2088; _LL2088: _temp2087= _temp2077.args; goto _LL2086; _LL2086:
_temp2085= _temp2077.c_varargs; goto _LL2084; _LL2084: _temp2083= _temp2077.cyc_varargs;
goto _LL2082; _LL2082: _temp2081= _temp2077.rgn_po; goto _LL2080; _LL2080:
_temp2079= _temp2077.attributes; goto _LL2074;} else{ goto _LL2075;} _LL2075:
goto _LL2076; _LL2074: fd->tvs= _temp2093; fd->effect= _temp2091; goto _LL2072;
_LL2076:({ int(* _temp2095)( struct _tagged_arr fmt, struct _tagged_arr ap)=(
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos; struct
_tagged_arr _temp2098= _tag_arr("check_fndecl_valid_type: not a FnType", sizeof(
unsigned char), 38u); void* _temp2096[ 0u]={}; struct _tagged_arr _temp2097={(
void*) _temp2096,( void*) _temp2096,( void*)( _temp2096 + 0u)}; _temp2095(
_temp2098, _temp2097);}); return; _LL2072:;}{ struct _RegionHandle _temp2099=
_new_region(); struct _RegionHandle* r=& _temp2099; _push_region( r); Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tagged_arr*(* f)(
struct _tuple13*), struct Cyc_List_List* x)) Cyc_List_rmap)( r,( struct
_tagged_arr*(*)( struct _tuple13* t)) Cyc_Tcutil_fst_fdarg, fd->args), loc,
_tag_arr("function declaration has repeated parameter", sizeof( unsigned char),
44u));; _pop_region( r);} fd->cached_typ=({ struct Cyc_Core_Opt* _temp2100=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2100->v=(
void*) t; _temp2100;});} struct _tuple15{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual
f2; } ; struct _tuple16{ struct _tuple15* f1; void* f2; } ; static struct
_tuple16* Cyc_Tcutil_substitute_f1( struct _RegionHandle* rgn, struct _tuple2* y){
return({ struct _tuple16* _temp2101=( struct _tuple16*) _region_malloc( rgn,
sizeof( struct _tuple16)); _temp2101->f1=({ struct _tuple15* _temp2102=( struct
_tuple15*) _region_malloc( rgn, sizeof( struct _tuple15)); _temp2102->f1=(* y).f1;
_temp2102->f2=(* y).f2; _temp2102;}); _temp2101->f2=(* y).f3; _temp2101;});}
static struct _tuple2* Cyc_Tcutil_substitute_f2( struct _tuple16* w){ void*
_temp2105; struct _tuple15* _temp2107; struct _tuple16 _temp2103=* w; _LL2108:
_temp2107= _temp2103.f1; goto _LL2106; _LL2106: _temp2105= _temp2103.f2; goto
_LL2104; _LL2104: { struct Cyc_Absyn_Tqual _temp2111; struct Cyc_Core_Opt*
_temp2113; struct _tuple15 _temp2109=* _temp2107; _LL2114: _temp2113= _temp2109.f1;
goto _LL2112; _LL2112: _temp2111= _temp2109.f2; goto _LL2110; _LL2110: return({
struct _tuple2* _temp2115=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2115->f1= _temp2113; _temp2115->f2= _temp2111; _temp2115->f3= _temp2105;
_temp2115;});}} static void* Cyc_Tcutil_field_type( struct Cyc_Absyn_Structfield*
f){ return( void*) f->type;} static struct Cyc_Absyn_Structfield* Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Structfield* f, void* t){ return({ struct Cyc_Absyn_Structfield*
_temp2116=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2116->name= f->name; _temp2116->tq= f->tq; _temp2116->type=( void*) t;
_temp2116->width= f->width; _temp2116->attributes= f->attributes; _temp2116;});}
static struct Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn,
struct Cyc_List_List* inst, struct Cyc_List_List* ts); void* Cyc_Tcutil_rsubstitute(
struct _RegionHandle* rgn, struct Cyc_List_List* inst, void* t){ void* _temp2117=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar* _temp2163; struct Cyc_Absyn_Structdecl**
_temp2165; struct Cyc_List_List* _temp2167; struct _tuple1* _temp2169; struct
Cyc_Absyn_Uniondecl** _temp2171; struct Cyc_List_List* _temp2173; struct _tuple1*
_temp2175; struct Cyc_Absyn_TunionInfo _temp2177; void* _temp2179; struct Cyc_List_List*
_temp2181; void* _temp2183; struct Cyc_Absyn_TunionFieldInfo _temp2185; struct
Cyc_List_List* _temp2187; void* _temp2189; struct Cyc_Core_Opt* _temp2191;
struct Cyc_List_List* _temp2193; struct _tuple1* _temp2195; struct Cyc_Absyn_Exp*
_temp2197; struct Cyc_Absyn_Tqual _temp2199; void* _temp2201; struct Cyc_Absyn_PtrInfo
_temp2203; struct Cyc_Absyn_Conref* _temp2205; struct Cyc_Absyn_Tqual _temp2207;
struct Cyc_Absyn_Conref* _temp2209; void* _temp2211; void* _temp2213; struct Cyc_Absyn_FnInfo
_temp2215; struct Cyc_List_List* _temp2217; struct Cyc_List_List* _temp2219;
struct Cyc_Absyn_VarargInfo* _temp2221; int _temp2223; struct Cyc_List_List*
_temp2225; void* _temp2227; struct Cyc_Core_Opt* _temp2229; struct Cyc_List_List*
_temp2231; struct Cyc_List_List* _temp2233; struct Cyc_List_List* _temp2235;
struct Cyc_List_List* _temp2237; struct Cyc_Core_Opt* _temp2239; void* _temp2241;
void* _temp2243; struct Cyc_List_List* _temp2245; _LL2119: if(( unsigned int)
_temp2117 > 4u?*(( int*) _temp2117) == Cyc_Absyn_VarType: 0){ _LL2164: _temp2163=((
struct Cyc_Absyn_VarType_struct*) _temp2117)->f1; goto _LL2120;} else{ goto
_LL2121;} _LL2121: if(( unsigned int) _temp2117 > 4u?*(( int*) _temp2117) == Cyc_Absyn_StructType:
0){ _LL2170: _temp2169=(( struct Cyc_Absyn_StructType_struct*) _temp2117)->f1;
goto _LL2168; _LL2168: _temp2167=(( struct Cyc_Absyn_StructType_struct*)
_temp2117)->f2; goto _LL2166; _LL2166: _temp2165=(( struct Cyc_Absyn_StructType_struct*)
_temp2117)->f3; goto _LL2122;} else{ goto _LL2123;} _LL2123: if(( unsigned int)
_temp2117 > 4u?*(( int*) _temp2117) == Cyc_Absyn_UnionType: 0){ _LL2176:
_temp2175=(( struct Cyc_Absyn_UnionType_struct*) _temp2117)->f1; goto _LL2174;
_LL2174: _temp2173=(( struct Cyc_Absyn_UnionType_struct*) _temp2117)->f2; goto
_LL2172; _LL2172: _temp2171=(( struct Cyc_Absyn_UnionType_struct*) _temp2117)->f3;
goto _LL2124;} else{ goto _LL2125;} _LL2125: if(( unsigned int) _temp2117 > 4u?*((
int*) _temp2117) == Cyc_Absyn_TunionType: 0){ _LL2178: _temp2177=(( struct Cyc_Absyn_TunionType_struct*)
_temp2117)->f1; _LL2184: _temp2183=( void*) _temp2177.tunion_info; goto _LL2182;
_LL2182: _temp2181= _temp2177.targs; goto _LL2180; _LL2180: _temp2179=( void*)
_temp2177.rgn; goto _LL2126;} else{ goto _LL2127;} _LL2127: if(( unsigned int)
_temp2117 > 4u?*(( int*) _temp2117) == Cyc_Absyn_TunionFieldType: 0){ _LL2186:
_temp2185=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2117)->f1; _LL2190:
_temp2189=( void*) _temp2185.field_info; goto _LL2188; _LL2188: _temp2187=
_temp2185.targs; goto _LL2128;} else{ goto _LL2129;} _LL2129: if(( unsigned int)
_temp2117 > 4u?*(( int*) _temp2117) == Cyc_Absyn_TypedefType: 0){ _LL2196:
_temp2195=(( struct Cyc_Absyn_TypedefType_struct*) _temp2117)->f1; goto _LL2194;
_LL2194: _temp2193=(( struct Cyc_Absyn_TypedefType_struct*) _temp2117)->f2; goto
_LL2192; _LL2192: _temp2191=(( struct Cyc_Absyn_TypedefType_struct*) _temp2117)->f3;
goto _LL2130;} else{ goto _LL2131;} _LL2131: if(( unsigned int) _temp2117 > 4u?*((
int*) _temp2117) == Cyc_Absyn_ArrayType: 0){ _LL2202: _temp2201=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2117)->f1; goto _LL2200; _LL2200:
_temp2199=(( struct Cyc_Absyn_ArrayType_struct*) _temp2117)->f2; goto _LL2198;
_LL2198: _temp2197=(( struct Cyc_Absyn_ArrayType_struct*) _temp2117)->f3; goto
_LL2132;} else{ goto _LL2133;} _LL2133: if(( unsigned int) _temp2117 > 4u?*((
int*) _temp2117) == Cyc_Absyn_PointerType: 0){ _LL2204: _temp2203=(( struct Cyc_Absyn_PointerType_struct*)
_temp2117)->f1; _LL2214: _temp2213=( void*) _temp2203.elt_typ; goto _LL2212;
_LL2212: _temp2211=( void*) _temp2203.rgn_typ; goto _LL2210; _LL2210: _temp2209=
_temp2203.nullable; goto _LL2208; _LL2208: _temp2207= _temp2203.tq; goto _LL2206;
_LL2206: _temp2205= _temp2203.bounds; goto _LL2134;} else{ goto _LL2135;}
_LL2135: if(( unsigned int) _temp2117 > 4u?*(( int*) _temp2117) == Cyc_Absyn_FnType:
0){ _LL2216: _temp2215=(( struct Cyc_Absyn_FnType_struct*) _temp2117)->f1;
_LL2232: _temp2231= _temp2215.tvars; goto _LL2230; _LL2230: _temp2229= _temp2215.effect;
goto _LL2228; _LL2228: _temp2227=( void*) _temp2215.ret_typ; goto _LL2226;
_LL2226: _temp2225= _temp2215.args; goto _LL2224; _LL2224: _temp2223= _temp2215.c_varargs;
goto _LL2222; _LL2222: _temp2221= _temp2215.cyc_varargs; goto _LL2220; _LL2220:
_temp2219= _temp2215.rgn_po; goto _LL2218; _LL2218: _temp2217= _temp2215.attributes;
goto _LL2136;} else{ goto _LL2137;} _LL2137: if(( unsigned int) _temp2117 > 4u?*((
int*) _temp2117) == Cyc_Absyn_TupleType: 0){ _LL2234: _temp2233=(( struct Cyc_Absyn_TupleType_struct*)
_temp2117)->f1; goto _LL2138;} else{ goto _LL2139;} _LL2139: if(( unsigned int)
_temp2117 > 4u?*(( int*) _temp2117) == Cyc_Absyn_AnonStructType: 0){ _LL2236:
_temp2235=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2117)->f1; goto
_LL2140;} else{ goto _LL2141;} _LL2141: if(( unsigned int) _temp2117 > 4u?*((
int*) _temp2117) == Cyc_Absyn_AnonUnionType: 0){ _LL2238: _temp2237=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp2117)->f1; goto _LL2142;} else{ goto
_LL2143;} _LL2143: if(( unsigned int) _temp2117 > 4u?*(( int*) _temp2117) == Cyc_Absyn_Evar:
0){ _LL2240: _temp2239=(( struct Cyc_Absyn_Evar_struct*) _temp2117)->f2; goto
_LL2144;} else{ goto _LL2145;} _LL2145: if(( unsigned int) _temp2117 > 4u?*((
int*) _temp2117) == Cyc_Absyn_RgnHandleType: 0){ _LL2242: _temp2241=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp2117)->f1; goto _LL2146;} else{
goto _LL2147;} _LL2147: if(( unsigned int) _temp2117 > 4u?*(( int*) _temp2117)
== Cyc_Absyn_EnumType: 0){ goto _LL2148;} else{ goto _LL2149;} _LL2149: if(
_temp2117 ==( void*) Cyc_Absyn_VoidType){ goto _LL2150;} else{ goto _LL2151;}
_LL2151: if(( unsigned int) _temp2117 > 4u?*(( int*) _temp2117) == Cyc_Absyn_IntType:
0){ goto _LL2152;} else{ goto _LL2153;} _LL2153: if( _temp2117 ==( void*) Cyc_Absyn_FloatType){
goto _LL2154;} else{ goto _LL2155;} _LL2155: if( _temp2117 ==( void*) Cyc_Absyn_DoubleType){
goto _LL2156;} else{ goto _LL2157;} _LL2157: if( _temp2117 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL2158;} else{ goto _LL2159;} _LL2159: if(( unsigned int) _temp2117 > 4u?*((
int*) _temp2117) == Cyc_Absyn_AccessEff: 0){ _LL2244: _temp2243=( void*)((
struct Cyc_Absyn_AccessEff_struct*) _temp2117)->f1; goto _LL2160;} else{ goto
_LL2161;} _LL2161: if(( unsigned int) _temp2117 > 4u?*(( int*) _temp2117) == Cyc_Absyn_JoinEff:
0){ _LL2246: _temp2245=(( struct Cyc_Absyn_JoinEff_struct*) _temp2117)->f1; goto
_LL2162;} else{ goto _LL2118;} _LL2120: { struct _handler_cons _temp2247;
_push_handler(& _temp2247);{ int _temp2249= 0; if( setjmp( _temp2247.handler)){
_temp2249= 1;} if( ! _temp2249){{ void* _temp2250=(( void*(*)( int(* compare)(
struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct
Cyc_Absyn_Tvar* x)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp, inst, _temp2163);
_npop_handler( 0u); return _temp2250;}; _pop_handler();} else{ void* _temp2248=(
void*) _exn_thrown; void* _temp2252= _temp2248; _LL2254: if( _temp2252 == Cyc_Core_Not_found){
goto _LL2255;} else{ goto _LL2256;} _LL2256: goto _LL2257; _LL2255: return t;
_LL2257:( void) _throw( _temp2252); _LL2253:;}}} _LL2122: { struct Cyc_List_List*
_temp2258= Cyc_Tcutil_substs( rgn, inst, _temp2167); return _temp2258 ==
_temp2167? t:( void*)({ struct Cyc_Absyn_StructType_struct* _temp2259=( struct
Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp2259[ 0]=({ struct Cyc_Absyn_StructType_struct _temp2260; _temp2260.tag=
Cyc_Absyn_StructType; _temp2260.f1= _temp2169; _temp2260.f2= _temp2258;
_temp2260.f3= _temp2165; _temp2260;}); _temp2259;});} _LL2124: { struct Cyc_List_List*
_temp2261= Cyc_Tcutil_substs( rgn, inst, _temp2173); return _temp2261 ==
_temp2173? t:( void*)({ struct Cyc_Absyn_UnionType_struct* _temp2262=( struct
Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp2262[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp2263; _temp2263.tag= Cyc_Absyn_UnionType;
_temp2263.f1= _temp2175; _temp2263.f2= _temp2261; _temp2263.f3= _temp2171;
_temp2263;}); _temp2262;});} _LL2126: { struct Cyc_List_List* _temp2264= Cyc_Tcutil_substs(
rgn, inst, _temp2181); void* _temp2265= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2179); return( _temp2264 == _temp2181? _temp2265 == _temp2179: 0)? t:( void*)({
struct Cyc_Absyn_TunionType_struct* _temp2266=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2266[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2267; _temp2267.tag= Cyc_Absyn_TunionType;
_temp2267.f1=({ struct Cyc_Absyn_TunionInfo _temp2268; _temp2268.tunion_info=(
void*) _temp2183; _temp2268.targs= _temp2264; _temp2268.rgn=( void*) _temp2265;
_temp2268;}); _temp2267;}); _temp2266;});} _LL2128: { struct Cyc_List_List*
_temp2269= Cyc_Tcutil_substs( rgn, inst, _temp2187); return _temp2269 ==
_temp2187? t:( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp2270=(
struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp2270[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp2271; _temp2271.tag=
Cyc_Absyn_TunionFieldType; _temp2271.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2272; _temp2272.field_info=( void*) _temp2189; _temp2272.targs= _temp2269;
_temp2272;}); _temp2271;}); _temp2270;});} _LL2130: { struct Cyc_List_List*
_temp2273= Cyc_Tcutil_substs( rgn, inst, _temp2193); return _temp2273 ==
_temp2193? t:( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp2274=( struct
Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2274[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp2275; _temp2275.tag=
Cyc_Absyn_TypedefType; _temp2275.f1= _temp2195; _temp2275.f2= _temp2273;
_temp2275.f3= _temp2191; _temp2275;}); _temp2274;});} _LL2132: { void* _temp2276=
Cyc_Tcutil_rsubstitute( rgn, inst, _temp2201); return _temp2276 == _temp2201? t:(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp2277=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2277[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2278; _temp2278.tag= Cyc_Absyn_ArrayType;
_temp2278.f1=( void*) _temp2276; _temp2278.f2= _temp2199; _temp2278.f3=
_temp2197; _temp2278;}); _temp2277;});} _LL2134: { void* _temp2279= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2213); void* _temp2280= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2211); if( _temp2279 == _temp2213? _temp2280 == _temp2211: 0){ return t;}
return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp2281=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp2281[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp2282; _temp2282.tag= Cyc_Absyn_PointerType;
_temp2282.f1=({ struct Cyc_Absyn_PtrInfo _temp2283; _temp2283.elt_typ=( void*)
_temp2279; _temp2283.rgn_typ=( void*) _temp2280; _temp2283.nullable= _temp2209;
_temp2283.tq= _temp2207; _temp2283.bounds= _temp2205; _temp2283;}); _temp2282;});
_temp2281;});} _LL2136:{ struct Cyc_List_List* _temp2284= _temp2231; for( 0;
_temp2284 != 0; _temp2284=(( struct Cyc_List_List*) _check_null( _temp2284))->tl){
inst=({ struct Cyc_List_List* _temp2285=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2285->hd=( void*)({ struct _tuple5*
_temp2286=( struct _tuple5*) _region_malloc( rgn, sizeof( struct _tuple5));
_temp2286->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp2284))->hd; _temp2286->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2287=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2287[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2288; _temp2288.tag= Cyc_Absyn_VarType;
_temp2288.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp2284))->hd; _temp2288;}); _temp2287;}); _temp2286;}); _temp2285->tl= inst;
_temp2285;});}}{ struct Cyc_List_List* _temp2291; struct Cyc_List_List*
_temp2293; struct _tuple0 _temp2289=(( struct _tuple0(*)( struct _RegionHandle*
r3, struct _RegionHandle* r4, struct Cyc_List_List* x)) Cyc_List_rsplit)( rgn,
rgn,(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple16*(* f)(
struct _RegionHandle*, struct _tuple2*), struct _RegionHandle* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_substitute_f1, rgn, _temp2225)); _LL2294:
_temp2293= _temp2289.f1; goto _LL2292; _LL2292: _temp2291= _temp2289.f2; goto
_LL2290; _LL2290: { struct Cyc_List_List* _temp2295= Cyc_Tcutil_substs( rgn,
inst, _temp2291); struct Cyc_List_List* _temp2296=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple16*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_substitute_f2,(( struct Cyc_List_List*(*)( struct _RegionHandle* r3,
struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp2293, _temp2295)); struct Cyc_Core_Opt* eff2; if( _temp2229 == 0){
eff2= 0;} else{ void* _temp2297= Cyc_Tcutil_rsubstitute( rgn, inst,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2229))->v); if( _temp2297 ==( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2229))->v){ eff2= _temp2229;} else{ eff2=({
struct Cyc_Core_Opt* _temp2298=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2298->v=( void*) _temp2297; _temp2298;});}}{ struct Cyc_Absyn_VarargInfo*
cyc_varargs2; if( _temp2221 == 0){ cyc_varargs2= 0;} else{ int _temp2301; void*
_temp2303; void* _temp2305; struct Cyc_Absyn_Tqual _temp2307; struct Cyc_Core_Opt*
_temp2309; struct Cyc_Absyn_VarargInfo _temp2299=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp2221)); _LL2310: _temp2309= _temp2299.name; goto _LL2308;
_LL2308: _temp2307= _temp2299.tq; goto _LL2306; _LL2306: _temp2305=( void*)
_temp2299.type; goto _LL2304; _LL2304: _temp2303=( void*) _temp2299.rgn; goto
_LL2302; _LL2302: _temp2301= _temp2299.inject; goto _LL2300; _LL2300: { void*
_temp2311= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2305); void* _temp2312= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2303); if( _temp2311 == _temp2305? _temp2303 == _temp2312: 0){
cyc_varargs2= _temp2221;} else{ cyc_varargs2=({ struct Cyc_Absyn_VarargInfo*
_temp2313=( struct Cyc_Absyn_VarargInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2313->name= _temp2309; _temp2313->tq= _temp2307; _temp2313->type=( void*)
_temp2311; _temp2313->rgn=( void*) _temp2312; _temp2313->inject= _temp2301;
_temp2313;});}}}{ struct Cyc_List_List* rgn_po2; struct Cyc_List_List* _temp2316;
struct Cyc_List_List* _temp2318; struct _tuple0 _temp2314= Cyc_List_rsplit( rgn,
rgn, _temp2219); _LL2319: _temp2318= _temp2314.f1; goto _LL2317; _LL2317:
_temp2316= _temp2314.f2; goto _LL2315; _LL2315: { struct Cyc_List_List*
_temp2320= Cyc_Tcutil_substs( rgn, inst, _temp2318); struct Cyc_List_List*
_temp2321= Cyc_Tcutil_substs( rgn, inst, _temp2316); if( _temp2320 == _temp2318?
_temp2321 == _temp2316: 0){ rgn_po2= _temp2219;} else{ rgn_po2= Cyc_List_zip(
_temp2320, _temp2321);} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2322=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2322[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2323; _temp2323.tag= Cyc_Absyn_FnType;
_temp2323.f1=({ struct Cyc_Absyn_FnInfo _temp2324; _temp2324.tvars= _temp2231;
_temp2324.effect= eff2; _temp2324.ret_typ=( void*) Cyc_Tcutil_rsubstitute( rgn,
inst, _temp2227); _temp2324.args= _temp2296; _temp2324.c_varargs= _temp2223;
_temp2324.cyc_varargs= cyc_varargs2; _temp2324.rgn_po= rgn_po2; _temp2324.attributes=
_temp2217; _temp2324;}); _temp2323;}); _temp2322;});}}}}} _LL2138: { struct Cyc_List_List*
_temp2325=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_snd_tqt,
_temp2233); struct Cyc_List_List* _temp2326= Cyc_Tcutil_substs( rgn, inst,
_temp2325); if( _temp2326 == _temp2325){ return t;}{ struct Cyc_List_List*
_temp2327=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*,
void*), struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_map2)( Cyc_Tcutil_map2_tq,
_temp2233, _temp2326); return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp2328=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp2328[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp2329; _temp2329.tag= Cyc_Absyn_TupleType;
_temp2329.f1= _temp2327; _temp2329;}); _temp2328;});}} _LL2140: { struct Cyc_List_List*
_temp2330=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2235); struct Cyc_List_List* _temp2331= Cyc_Tcutil_substs( rgn, inst,
_temp2330); if( _temp2331 == _temp2330){ return t;}{ struct Cyc_List_List*
_temp2332=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2235, _temp2331); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp2333=( struct Cyc_Absyn_AnonStructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2333[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2334; _temp2334.tag= Cyc_Absyn_AnonStructType;
_temp2334.f1= _temp2332; _temp2334;}); _temp2333;});}} _LL2142: { struct Cyc_List_List*
_temp2335=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2237); struct Cyc_List_List* _temp2336= Cyc_Tcutil_substs( rgn, inst,
_temp2335); if( _temp2336 == _temp2335){ return t;}{ struct Cyc_List_List*
_temp2337=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2237, _temp2336); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp2338=( struct Cyc_Absyn_AnonStructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2338[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2339; _temp2339.tag= Cyc_Absyn_AnonStructType;
_temp2339.f1= _temp2337; _temp2339;}); _temp2338;});}} _LL2144: if( _temp2239 !=
0){ return Cyc_Tcutil_rsubstitute( rgn, inst,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2239))->v);} else{ return t;} _LL2146: { void* _temp2340= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2241); return _temp2340 == _temp2241? t:( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp2341=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp2341[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp2342; _temp2342.tag= Cyc_Absyn_RgnHandleType; _temp2342.f1=( void*)
_temp2340; _temp2342;}); _temp2341;});} _LL2148: return t; _LL2150: return t;
_LL2152: return t; _LL2154: return t; _LL2156: return t; _LL2158: return t;
_LL2160: { void* _temp2343= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2243);
return _temp2343 == _temp2243? t:( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp2344=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2344[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2345; _temp2345.tag= Cyc_Absyn_AccessEff;
_temp2345.f1=( void*) _temp2343; _temp2345;}); _temp2344;});} _LL2162: { struct
Cyc_List_List* _temp2346= Cyc_Tcutil_substs( rgn, inst, _temp2245); return
_temp2346 == _temp2245? t:( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2347=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2347[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2348; _temp2348.tag= Cyc_Absyn_JoinEff;
_temp2348.f1= _temp2346; _temp2348;}); _temp2347;});} _LL2118:;} static struct
Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List*
inst, struct Cyc_List_List* ts){ if( ts == 0){ return 0;}{ void* _temp2349=(
void*)(( struct Cyc_List_List*) _check_null( ts))->hd; struct Cyc_List_List*
_temp2350=(( struct Cyc_List_List*) _check_null( ts))->tl; void* _temp2351= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2349); struct Cyc_List_List* _temp2352= Cyc_Tcutil_substs( rgn,
inst, _temp2350); if( _temp2349 == _temp2351? _temp2350 == _temp2352: 0){ return
ts;} return( struct Cyc_List_List*)({ struct Cyc_List_List* _temp2353=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2353->hd=( void*)
_temp2351; _temp2353->tl= _temp2352; _temp2353;});}} extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List* inst, void* t){ return Cyc_Tcutil_rsubstitute( Cyc_Core_heap_region,
inst, t);} struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2403=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2403[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2404; _temp2404.tag= Cyc_Absyn_Const_e; _temp2404.f1=(
void*)(( void*) Cyc_Absyn_Null_c); _temp2404;}); _temp2403;}), loc);{ void*
_temp2354= Cyc_Tcutil_compress( t); void* _temp2368; void* _temp2370; void*
_temp2372; void* _temp2374; _LL2356: if(( unsigned int) _temp2354 > 4u?*(( int*)
_temp2354) == Cyc_Absyn_PointerType: 0){ goto _LL2357;} else{ goto _LL2358;}
_LL2358: if(( unsigned int) _temp2354 > 4u?*(( int*) _temp2354) == Cyc_Absyn_IntType:
0){ _LL2371: _temp2370=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2354)->f1;
goto _LL2369; _LL2369: _temp2368=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2354)->f2; if( _temp2368 ==( void*) Cyc_Absyn_B1){ goto _LL2359;} else{
goto _LL2360;}} else{ goto _LL2360;} _LL2360: if(( unsigned int) _temp2354 > 4u?*((
int*) _temp2354) == Cyc_Absyn_IntType: 0){ _LL2375: _temp2374=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2354)->f1; goto _LL2373; _LL2373: _temp2372=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2354)->f2; goto _LL2361;} else{
goto _LL2362;} _LL2362: if( _temp2354 ==( void*) Cyc_Absyn_FloatType){ goto
_LL2363;} else{ goto _LL2364;} _LL2364: if( _temp2354 ==( void*) Cyc_Absyn_DoubleType){
goto _LL2365;} else{ goto _LL2366;} _LL2366: goto _LL2367; _LL2357: goto _LL2355;
_LL2359:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2376=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2376[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2377; _temp2377.tag= Cyc_Absyn_Const_e;
_temp2377.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp2378=(
struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp2378[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp2379; _temp2379.tag= Cyc_Absyn_Char_c;
_temp2379.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2379.f2='\000'; _temp2379;});
_temp2378;})); _temp2377;}); _temp2376;}))); goto _LL2355; _LL2361:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2380=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2380[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2381; _temp2381.tag= Cyc_Absyn_Const_e; _temp2381.f1=(
void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp2382=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp2382[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp2383; _temp2383.tag= Cyc_Absyn_Int_c; _temp2383.f1=( void*) _temp2374;
_temp2383.f2= 0; _temp2383;}); _temp2382;})); _temp2381;}); _temp2380;}))); if(
_temp2372 !=( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp2384=( struct Cyc_Absyn_Cast_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp2384[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp2385; _temp2385.tag= Cyc_Absyn_Cast_e;
_temp2385.f1=( void*) t; _temp2385.f2= e; _temp2385;}); _temp2384;}), loc);}
goto _LL2355; _LL2363:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2386=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2386[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2387; _temp2387.tag= Cyc_Absyn_Const_e;
_temp2387.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2388=(
struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2388[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2389; _temp2389.tag= Cyc_Absyn_Float_c;
_temp2389.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp2389;});
_temp2388;})); _temp2387;}); _temp2386;}))); goto _LL2355; _LL2365:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2390=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2390[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2391; _temp2391.tag= Cyc_Absyn_Cast_e; _temp2391.f1=( void*) t; _temp2391.f2=
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2392=( struct
Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2392[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2393; _temp2393.tag= Cyc_Absyn_Const_e;
_temp2393.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2394=(
struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2394[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2395; _temp2395.tag= Cyc_Absyn_Float_c;
_temp2395.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp2395;});
_temp2394;})); _temp2393;}); _temp2392;}), loc); _temp2391;}); _temp2390;})));
goto _LL2355; _LL2367:({ void(* _temp2396)( struct Cyc_Position_Segment* loc,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp2400= loc; struct _tagged_arr _temp2401= _tag_arr("declaration of type %s requires initializer",
sizeof( unsigned char), 44u); struct Cyc_Stdio_String_pa_struct _temp2402;
_temp2402.tag= Cyc_Stdio_String_pa; _temp2402.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp2399=( void*)& _temp2402; void* _temp2397[ 1u]={ _temp2399};
struct _tagged_arr _temp2398={( void*) _temp2397,( void*) _temp2397,( void*)(
_temp2397 + 1u)}; _temp2396( _temp2400, _temp2401, _temp2398);}}); goto _LL2355;
_LL2355:;} return e;} struct _tuple5* Cyc_Tcutil_make_inst_var( struct Cyc_List_List*
s, struct Cyc_Absyn_Tvar* tv){ void* k= Cyc_Absyn_conref_val( tv->kind); return({
struct _tuple5* _temp2405=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp2405->f1= tv; _temp2405->f2= Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt*
_temp2406=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2406->v=( void*) k; _temp2406;}),({ struct Cyc_Core_Opt* _temp2407=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2407->v=( void*) s;
_temp2407;})); _temp2405;});} struct _tuple5* Cyc_Tcutil_r_make_inst_var( struct
_tuple6* env, struct Cyc_Absyn_Tvar* tv){ struct _tuple6 _temp2410; struct
_RegionHandle* _temp2411; struct Cyc_List_List* _temp2413; struct _tuple6*
_temp2408= env; _temp2410=* _temp2408; _LL2414: _temp2413= _temp2410.f1; goto
_LL2412; _LL2412: _temp2411= _temp2410.f2; goto _LL2409; _LL2409: { void* k= Cyc_Absyn_conref_val(
tv->kind); return({ struct _tuple5* _temp2415=( struct _tuple5*) _region_malloc(
_temp2411, sizeof( struct _tuple5)); _temp2415->f1= tv; _temp2415->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp2416=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2416->v=( void*) k; _temp2416;}),({ struct Cyc_Core_Opt*
_temp2417=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2417->v=( void*) _temp2413; _temp2417;})); _temp2415;});}} static struct
Cyc_List_List* Cyc_Tcutil_add_free_tvar( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 != 0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
if( Cyc_String_zstrptrcmp((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->name, tv->name) == 0){ struct Cyc_Absyn_Conref* k1=((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->kind;
struct Cyc_Absyn_Conref* k2= tv->kind; if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){({
void(* _temp2418)( struct Cyc_Position_Segment* loc, struct _tagged_arr fmt,
struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp2424=
loc; struct _tagged_arr _temp2425= _tag_arr("type variable %s is used with inconsistent kinds %s and %s",
sizeof( unsigned char), 59u); struct Cyc_Stdio_String_pa_struct _temp2428;
_temp2428.tag= Cyc_Stdio_String_pa; _temp2428.f1=( struct _tagged_arr)* tv->name;{
void* _temp2421=( void*)& _temp2428; struct Cyc_Stdio_String_pa_struct _temp2427;
_temp2427.tag= Cyc_Stdio_String_pa; _temp2427.f1=( struct _tagged_arr) Cyc_Absynpp_ckind2string(
k1);{ void* _temp2422=( void*)& _temp2427; struct Cyc_Stdio_String_pa_struct
_temp2426; _temp2426.tag= Cyc_Stdio_String_pa; _temp2426.f1=( struct _tagged_arr)
Cyc_Absynpp_ckind2string( k2);{ void* _temp2423=( void*)& _temp2426; void*
_temp2419[ 3u]={ _temp2421, _temp2422, _temp2423}; struct _tagged_arr _temp2420={(
void*) _temp2419,( void*) _temp2419,( void*)( _temp2419 + 3u)}; _temp2418(
_temp2424, _temp2425, _temp2420);}}}});} if( tv->identity == 0){ tv->identity=((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->identity;}
else{ if(*(( int*) _check_null( tv->identity)) !=*(( int*) _check_null((( struct
Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->identity))){({
int(* _temp2429)( struct _tagged_arr fmt, struct _tagged_arr ap)=( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos; struct
_tagged_arr _temp2432= _tag_arr("same type variable has different identity!",
sizeof( unsigned char), 43u); void* _temp2430[ 0u]={}; struct _tagged_arr
_temp2431={( void*) _temp2430,( void*) _temp2430,( void*)( _temp2430 + 0u)};
_temp2429( _temp2432, _temp2431);});}} return tvs;}}} tv->identity= Cyc_Tcutil_new_tvar_id();
return({ struct Cyc_List_List* _temp2433=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2433->hd=( void*) tv; _temp2433->tl= tvs;
_temp2433;});} static struct Cyc_List_List* Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){ if( tv->identity == 0){({
int(* _temp2434)( struct _tagged_arr fmt, struct _tagged_arr ap)=( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos; struct
_tagged_arr _temp2437= _tag_arr("fast_add_free_tvar: bad identity in tv",
sizeof( unsigned char), 39u); void* _temp2435[ 0u]={}; struct _tagged_arr
_temp2436={( void*) _temp2435,( void*) _temp2435,( void*)( _temp2435 + 0u)};
_temp2434( _temp2437, _temp2436);});}{ struct Cyc_List_List* tvs2= tvs; for( 0;
tvs2 != 0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){ if((( struct
Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->identity ==
0){({ int(* _temp2438)( struct _tagged_arr fmt, struct _tagged_arr ap)=( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos; struct
_tagged_arr _temp2441= _tag_arr("fast_add_free_tvar: bad identity in tvs2",
sizeof( unsigned char), 41u); void* _temp2439[ 0u]={}; struct _tagged_arr
_temp2440={( void*) _temp2439,( void*) _temp2439,( void*)( _temp2439 + 0u)};
_temp2438( _temp2441, _temp2440);});} if(*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->identity)) ==*(( int*)
_check_null( tv->identity))){ return tvs;}}} return({ struct Cyc_List_List*
_temp2442=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2442->hd=( void*) tv; _temp2442->tl= tvs; _temp2442;});} static struct Cyc_List_List*
Cyc_Tcutil_add_bound_tvar( struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){
if( tv->identity == 0){({ int(* _temp2443)( struct _tagged_arr fmt, struct
_tagged_arr ap)=( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos;
struct _tagged_arr _temp2447= _tag_arr("bound tvar id for %s is null", sizeof(
unsigned char), 29u); struct Cyc_Stdio_String_pa_struct _temp2448; _temp2448.tag=
Cyc_Stdio_String_pa; _temp2448.f1=( struct _tagged_arr) Cyc_Tcutil_tvar2string(
tv);{ void* _temp2446=( void*)& _temp2448; void* _temp2444[ 1u]={ _temp2446};
struct _tagged_arr _temp2445={( void*) _temp2444,( void*) _temp2444,( void*)(
_temp2444 + 1u)}; _temp2443( _temp2447, _temp2445);}});} return({ struct Cyc_List_List*
_temp2449=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2449->hd=( void*) tv; _temp2449->tl= tvs; _temp2449;});} static struct Cyc_List_List*
Cyc_Tcutil_add_free_evar( struct Cyc_List_List* es, void* e){ void* _temp2450=
Cyc_Tcutil_compress( e); int _temp2456; _LL2452: if(( unsigned int) _temp2450 >
4u?*(( int*) _temp2450) == Cyc_Absyn_Evar: 0){ _LL2457: _temp2456=(( struct Cyc_Absyn_Evar_struct*)
_temp2450)->f3; goto _LL2453;} else{ goto _LL2454;} _LL2454: goto _LL2455;
_LL2453:{ struct Cyc_List_List* es2= es; for( 0; es2 != 0; es2=(( struct Cyc_List_List*)
_check_null( es2))->tl){ void* _temp2458= Cyc_Tcutil_compress(( void*)(( struct
Cyc_List_List*) _check_null( es2))->hd); int _temp2464; _LL2460: if((
unsigned int) _temp2458 > 4u?*(( int*) _temp2458) == Cyc_Absyn_Evar: 0){ _LL2465:
_temp2464=(( struct Cyc_Absyn_Evar_struct*) _temp2458)->f3; goto _LL2461;} else{
goto _LL2462;} _LL2462: goto _LL2463; _LL2461: if( _temp2456 == _temp2464){
return es;} goto _LL2459; _LL2463: goto _LL2459; _LL2459:;}} return({ struct Cyc_List_List*
_temp2466=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2466->hd=( void*) e; _temp2466->tl= es; _temp2466;}); _LL2455: return es;
_LL2451:;} static struct Cyc_List_List* Cyc_Tcutil_remove_bound_tvars( struct
Cyc_List_List* tvs, struct Cyc_List_List* btvs){ struct Cyc_List_List* r= 0;
for( 0; tvs != 0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ int
present= 0;{ struct Cyc_List_List* b= btvs; for( 0; b != 0; b=(( struct Cyc_List_List*)
_check_null( b))->tl){ if(*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->identity)) ==*(( int*)
_check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd)->identity))){
present= 1; break;}}} if( ! present){ r=({ struct Cyc_List_List* _temp2467=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2467->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd);
_temp2467->tl= r; _temp2467;});}} r=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( r); return r;} struct Cyc_Tcutil_CVTEnv{ struct Cyc_List_List*
kind_env; struct Cyc_List_List* free_vars; struct Cyc_List_List* free_evars; } ;
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcutil_CVTEnv cvtenv, void*
expected_kind, void* t){{ void* _temp2468= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt*
_temp2514; struct Cyc_Core_Opt** _temp2516; struct Cyc_Absyn_Tvar* _temp2517;
struct Cyc_Absyn_Enumdecl* _temp2519; struct Cyc_Absyn_Enumdecl** _temp2521;
struct _tuple1* _temp2522; struct Cyc_Absyn_TunionInfo _temp2524; void*
_temp2526; struct Cyc_List_List* _temp2528; void* _temp2530; void** _temp2532;
struct Cyc_Absyn_TunionFieldInfo _temp2533; struct Cyc_List_List* _temp2535;
void* _temp2537; void** _temp2539; struct Cyc_Absyn_PtrInfo _temp2540; struct
Cyc_Absyn_Conref* _temp2542; struct Cyc_Absyn_Tqual _temp2544; struct Cyc_Absyn_Conref*
_temp2546; void* _temp2548; void* _temp2550; struct Cyc_Absyn_Exp* _temp2552;
struct Cyc_Absyn_Tqual _temp2554; void* _temp2556; struct Cyc_Absyn_FnInfo
_temp2558; struct Cyc_List_List* _temp2560; struct Cyc_List_List* _temp2562;
struct Cyc_Absyn_VarargInfo* _temp2564; int _temp2566; struct Cyc_List_List*
_temp2568; void* _temp2570; struct Cyc_Core_Opt* _temp2572; struct Cyc_Core_Opt**
_temp2574; struct Cyc_List_List* _temp2575; struct Cyc_List_List** _temp2577;
struct Cyc_List_List* _temp2578; struct Cyc_List_List* _temp2580; struct Cyc_List_List*
_temp2582; struct Cyc_Absyn_Structdecl** _temp2584; struct Cyc_Absyn_Structdecl***
_temp2586; struct Cyc_List_List* _temp2587; struct _tuple1* _temp2589; struct
Cyc_Absyn_Uniondecl** _temp2591; struct Cyc_Absyn_Uniondecl*** _temp2593; struct
Cyc_List_List* _temp2594; struct _tuple1* _temp2596; struct Cyc_Core_Opt*
_temp2598; struct Cyc_Core_Opt** _temp2600; struct Cyc_List_List* _temp2601;
struct _tuple1* _temp2603; void* _temp2605; void* _temp2607; struct Cyc_List_List*
_temp2609; _LL2470: if( _temp2468 ==( void*) Cyc_Absyn_VoidType){ goto _LL2471;}
else{ goto _LL2472;} _LL2472: if(( unsigned int) _temp2468 > 4u?*(( int*)
_temp2468) == Cyc_Absyn_Evar: 0){ _LL2515: _temp2514=(( struct Cyc_Absyn_Evar_struct*)
_temp2468)->f1; _temp2516=&(( struct Cyc_Absyn_Evar_struct*) _temp2468)->f1;
goto _LL2473;} else{ goto _LL2474;} _LL2474: if(( unsigned int) _temp2468 > 4u?*((
int*) _temp2468) == Cyc_Absyn_VarType: 0){ _LL2518: _temp2517=(( struct Cyc_Absyn_VarType_struct*)
_temp2468)->f1; goto _LL2475;} else{ goto _LL2476;} _LL2476: if(( unsigned int)
_temp2468 > 4u?*(( int*) _temp2468) == Cyc_Absyn_EnumType: 0){ _LL2523:
_temp2522=(( struct Cyc_Absyn_EnumType_struct*) _temp2468)->f1; goto _LL2520;
_LL2520: _temp2519=(( struct Cyc_Absyn_EnumType_struct*) _temp2468)->f2;
_temp2521=&(( struct Cyc_Absyn_EnumType_struct*) _temp2468)->f2; goto _LL2477;}
else{ goto _LL2478;} _LL2478: if(( unsigned int) _temp2468 > 4u?*(( int*)
_temp2468) == Cyc_Absyn_TunionType: 0){ _LL2525: _temp2524=(( struct Cyc_Absyn_TunionType_struct*)
_temp2468)->f1; _LL2531: _temp2530=( void*) _temp2524.tunion_info; _temp2532=&(((
struct Cyc_Absyn_TunionType_struct*) _temp2468)->f1).tunion_info; goto _LL2529;
_LL2529: _temp2528= _temp2524.targs; goto _LL2527; _LL2527: _temp2526=( void*)
_temp2524.rgn; goto _LL2479;} else{ goto _LL2480;} _LL2480: if(( unsigned int)
_temp2468 > 4u?*(( int*) _temp2468) == Cyc_Absyn_TunionFieldType: 0){ _LL2534:
_temp2533=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2468)->f1; _LL2538:
_temp2537=( void*) _temp2533.field_info; _temp2539=&((( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2468)->f1).field_info; goto _LL2536; _LL2536: _temp2535= _temp2533.targs;
goto _LL2481;} else{ goto _LL2482;} _LL2482: if(( unsigned int) _temp2468 > 4u?*((
int*) _temp2468) == Cyc_Absyn_PointerType: 0){ _LL2541: _temp2540=(( struct Cyc_Absyn_PointerType_struct*)
_temp2468)->f1; _LL2551: _temp2550=( void*) _temp2540.elt_typ; goto _LL2549;
_LL2549: _temp2548=( void*) _temp2540.rgn_typ; goto _LL2547; _LL2547: _temp2546=
_temp2540.nullable; goto _LL2545; _LL2545: _temp2544= _temp2540.tq; goto _LL2543;
_LL2543: _temp2542= _temp2540.bounds; goto _LL2483;} else{ goto _LL2484;}
_LL2484: if(( unsigned int) _temp2468 > 4u?*(( int*) _temp2468) == Cyc_Absyn_IntType:
0){ goto _LL2485;} else{ goto _LL2486;} _LL2486: if( _temp2468 ==( void*) Cyc_Absyn_FloatType){
goto _LL2487;} else{ goto _LL2488;} _LL2488: if( _temp2468 ==( void*) Cyc_Absyn_DoubleType){
goto _LL2489;} else{ goto _LL2490;} _LL2490: if(( unsigned int) _temp2468 > 4u?*((
int*) _temp2468) == Cyc_Absyn_ArrayType: 0){ _LL2557: _temp2556=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2468)->f1; goto _LL2555; _LL2555:
_temp2554=(( struct Cyc_Absyn_ArrayType_struct*) _temp2468)->f2; goto _LL2553;
_LL2553: _temp2552=(( struct Cyc_Absyn_ArrayType_struct*) _temp2468)->f3; goto
_LL2491;} else{ goto _LL2492;} _LL2492: if(( unsigned int) _temp2468 > 4u?*((
int*) _temp2468) == Cyc_Absyn_FnType: 0){ _LL2559: _temp2558=(( struct Cyc_Absyn_FnType_struct*)
_temp2468)->f1; _LL2576: _temp2575= _temp2558.tvars; _temp2577=&((( struct Cyc_Absyn_FnType_struct*)
_temp2468)->f1).tvars; goto _LL2573; _LL2573: _temp2572= _temp2558.effect;
_temp2574=&((( struct Cyc_Absyn_FnType_struct*) _temp2468)->f1).effect; goto
_LL2571; _LL2571: _temp2570=( void*) _temp2558.ret_typ; goto _LL2569; _LL2569:
_temp2568= _temp2558.args; goto _LL2567; _LL2567: _temp2566= _temp2558.c_varargs;
goto _LL2565; _LL2565: _temp2564= _temp2558.cyc_varargs; goto _LL2563; _LL2563:
_temp2562= _temp2558.rgn_po; goto _LL2561; _LL2561: _temp2560= _temp2558.attributes;
goto _LL2493;} else{ goto _LL2494;} _LL2494: if(( unsigned int) _temp2468 > 4u?*((
int*) _temp2468) == Cyc_Absyn_TupleType: 0){ _LL2579: _temp2578=(( struct Cyc_Absyn_TupleType_struct*)
_temp2468)->f1; goto _LL2495;} else{ goto _LL2496;} _LL2496: if(( unsigned int)
_temp2468 > 4u?*(( int*) _temp2468) == Cyc_Absyn_AnonStructType: 0){ _LL2581:
_temp2580=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2468)->f1; goto
_LL2497;} else{ goto _LL2498;} _LL2498: if(( unsigned int) _temp2468 > 4u?*((
int*) _temp2468) == Cyc_Absyn_AnonUnionType: 0){ _LL2583: _temp2582=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp2468)->f1; goto _LL2499;} else{ goto
_LL2500;} _LL2500: if(( unsigned int) _temp2468 > 4u?*(( int*) _temp2468) == Cyc_Absyn_StructType:
0){ _LL2590: _temp2589=(( struct Cyc_Absyn_StructType_struct*) _temp2468)->f1;
goto _LL2588; _LL2588: _temp2587=(( struct Cyc_Absyn_StructType_struct*)
_temp2468)->f2; goto _LL2585; _LL2585: _temp2584=(( struct Cyc_Absyn_StructType_struct*)
_temp2468)->f3; _temp2586=&(( struct Cyc_Absyn_StructType_struct*) _temp2468)->f3;
goto _LL2501;} else{ goto _LL2502;} _LL2502: if(( unsigned int) _temp2468 > 4u?*((
int*) _temp2468) == Cyc_Absyn_UnionType: 0){ _LL2597: _temp2596=(( struct Cyc_Absyn_UnionType_struct*)
_temp2468)->f1; goto _LL2595; _LL2595: _temp2594=(( struct Cyc_Absyn_UnionType_struct*)
_temp2468)->f2; goto _LL2592; _LL2592: _temp2591=(( struct Cyc_Absyn_UnionType_struct*)
_temp2468)->f3; _temp2593=&(( struct Cyc_Absyn_UnionType_struct*) _temp2468)->f3;
goto _LL2503;} else{ goto _LL2504;} _LL2504: if(( unsigned int) _temp2468 > 4u?*((
int*) _temp2468) == Cyc_Absyn_TypedefType: 0){ _LL2604: _temp2603=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2468)->f1; goto _LL2602; _LL2602: _temp2601=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2468)->f2; goto _LL2599; _LL2599: _temp2598=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2468)->f3; _temp2600=&(( struct Cyc_Absyn_TypedefType_struct*) _temp2468)->f3;
goto _LL2505;} else{ goto _LL2506;} _LL2506: if( _temp2468 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL2507;} else{ goto _LL2508;} _LL2508: if(( unsigned int) _temp2468 > 4u?*((
int*) _temp2468) == Cyc_Absyn_RgnHandleType: 0){ _LL2606: _temp2605=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp2468)->f1; goto _LL2509;} else{
goto _LL2510;} _LL2510: if(( unsigned int) _temp2468 > 4u?*(( int*) _temp2468)
== Cyc_Absyn_AccessEff: 0){ _LL2608: _temp2607=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp2468)->f1; goto _LL2511;} else{ goto _LL2512;} _LL2512: if(( unsigned int)
_temp2468 > 4u?*(( int*) _temp2468) == Cyc_Absyn_JoinEff: 0){ _LL2610: _temp2609=((
struct Cyc_Absyn_JoinEff_struct*) _temp2468)->f1; goto _LL2513;} else{ goto
_LL2469;} _LL2471: goto _LL2469; _LL2473: if(* _temp2516 == 0){* _temp2516=({
struct Cyc_Core_Opt* _temp2611=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2611->v=( void*) expected_kind; _temp2611;});} cvtenv.free_evars=
Cyc_Tcutil_add_free_evar( cvtenv.free_evars, t); goto _LL2469; _LL2475: { struct
Cyc_Absyn_Conref* c= Cyc_Absyn_compress_conref( _temp2517->kind); if(( void*) c->v
==( void*) Cyc_Absyn_No_constr){( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2612=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2612[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2613; _temp2613.tag= Cyc_Absyn_Eq_constr;
_temp2613.f1=( void*) expected_kind; _temp2613;}); _temp2612;})));} cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar( loc, cvtenv.kind_env, _temp2517); cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars, _temp2517); goto _LL2469;}
_LL2477: if(* _temp2521 == 0){ struct _handler_cons _temp2614; _push_handler(&
_temp2614);{ int _temp2616= 0; if( setjmp( _temp2614.handler)){ _temp2616= 1;}
if( ! _temp2616){{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl( te,
loc, _temp2522);* _temp2521=( struct Cyc_Absyn_Enumdecl*)* ed;}; _pop_handler();}
else{ void* _temp2615=( void*) _exn_thrown; void* _temp2618= _temp2615; _LL2620:
if( _temp2618 == Cyc_Dict_Absent){ goto _LL2621;} else{ goto _LL2622;} _LL2622:
goto _LL2623; _LL2621:({ void(* _temp2624)( struct Cyc_Position_Segment* loc,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp2628= loc; struct _tagged_arr _temp2629= _tag_arr("unbound type enum %s",
sizeof( unsigned char), 21u); struct Cyc_Stdio_String_pa_struct _temp2630;
_temp2630.tag= Cyc_Stdio_String_pa; _temp2630.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2522);{ void* _temp2627=( void*)& _temp2630; void* _temp2625[ 1u]={
_temp2627}; struct _tagged_arr _temp2626={( void*) _temp2625,( void*) _temp2625,(
void*)( _temp2625 + 1u)}; _temp2624( _temp2628, _temp2629, _temp2626);}});
return cvtenv; _LL2623:( void) _throw( _temp2618); _LL2619:;}}}{ struct Cyc_Absyn_Enumdecl*
ed=( struct Cyc_Absyn_Enumdecl*) _check_null(* _temp2521);* _temp2522=( ed->name)[
_check_known_subscript_notnull( 1u, 0)]; goto _LL2469;} _LL2479:{ void*
_temp2631=* _temp2532; struct Cyc_Absyn_UnknownTunionInfo _temp2637; int
_temp2639; struct _tuple1* _temp2641; struct Cyc_Absyn_Tuniondecl* _temp2643;
_LL2633: if(*(( int*) _temp2631) == Cyc_Absyn_UnknownTunion){ _LL2638: _temp2637=((
struct Cyc_Absyn_UnknownTunion_struct*) _temp2631)->f1; _LL2642: _temp2641=
_temp2637.name; goto _LL2640; _LL2640: _temp2639= _temp2637.is_xtunion; goto
_LL2634;} else{ goto _LL2635;} _LL2635: if(*(( int*) _temp2631) == Cyc_Absyn_KnownTunion){
_LL2644: _temp2643=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2631)->f1; goto
_LL2636;} else{ goto _LL2632;} _LL2634: { struct Cyc_Absyn_Tuniondecl* tud;{
struct _handler_cons _temp2645; _push_handler(& _temp2645);{ int _temp2647= 0;
if( setjmp( _temp2645.handler)){ _temp2647= 1;} if( ! _temp2647){ tud=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp2641);; _pop_handler();} else{ void* _temp2646=( void*)
_exn_thrown; void* _temp2649= _temp2646; _LL2651: if( _temp2649 == Cyc_Dict_Absent){
goto _LL2652;} else{ goto _LL2653;} _LL2653: goto _LL2654; _LL2652:({ void(*
_temp2655)( struct Cyc_Position_Segment* loc, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp2659= loc;
struct _tagged_arr _temp2660= _tag_arr("unbound type [x]tunion %s", sizeof(
unsigned char), 26u); struct Cyc_Stdio_String_pa_struct _temp2661; _temp2661.tag=
Cyc_Stdio_String_pa; _temp2661.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2641);{ void* _temp2658=( void*)& _temp2661; void* _temp2656[ 1u]={
_temp2658}; struct _tagged_arr _temp2657={( void*) _temp2656,( void*) _temp2656,(
void*)( _temp2656 + 1u)}; _temp2655( _temp2659, _temp2660, _temp2657);}});
return cvtenv; _LL2654:( void) _throw( _temp2649); _LL2650:;}}} if( tud->is_xtunion
!= _temp2639){({ void(* _temp2662)( struct Cyc_Position_Segment* loc, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp2666= loc; struct _tagged_arr _temp2667= _tag_arr("[x]tunion is different from type declaration %s",
sizeof( unsigned char), 48u); struct Cyc_Stdio_String_pa_struct _temp2668;
_temp2668.tag= Cyc_Stdio_String_pa; _temp2668.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2641);{ void* _temp2665=( void*)& _temp2668; void* _temp2663[ 1u]={
_temp2665}; struct _tagged_arr _temp2664={( void*) _temp2663,( void*) _temp2663,(
void*)( _temp2663 + 1u)}; _temp2662( _temp2666, _temp2667, _temp2664);}});}*
_temp2532=( void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp2669=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp2669[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp2670; _temp2670.tag= Cyc_Absyn_KnownTunion;
_temp2670.f1= tud; _temp2670;}); _temp2669;}); _temp2643= tud; goto _LL2636;}
_LL2636: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2526);{ struct Cyc_List_List* tvs= _temp2643->tvs; for( 0; _temp2528 != 0?
tvs != 0: 0;( _temp2528=(( struct Cyc_List_List*) _check_null( _temp2528))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2528))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} if( _temp2528
!= 0){({ void(* _temp2671)( struct Cyc_Position_Segment* loc, struct _tagged_arr
fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp2675= loc; struct _tagged_arr _temp2676= _tag_arr("too many type arguments for tunion %s",
sizeof( unsigned char), 38u); struct Cyc_Stdio_String_pa_struct _temp2677;
_temp2677.tag= Cyc_Stdio_String_pa; _temp2677.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2643->name);{ void* _temp2674=( void*)& _temp2677; void* _temp2672[ 1u]={
_temp2674}; struct _tagged_arr _temp2673={( void*) _temp2672,( void*) _temp2672,(
void*)( _temp2672 + 1u)}; _temp2671( _temp2675, _temp2676, _temp2673);}});} if(
tvs != 0){({ void(* _temp2678)( struct Cyc_Position_Segment* loc, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp2682= loc; struct _tagged_arr _temp2683= _tag_arr("too few type arguments for tunion %s",
sizeof( unsigned char), 37u); struct Cyc_Stdio_String_pa_struct _temp2684;
_temp2684.tag= Cyc_Stdio_String_pa; _temp2684.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2643->name);{ void* _temp2681=( void*)& _temp2684; void* _temp2679[ 1u]={
_temp2681}; struct _tagged_arr _temp2680={( void*) _temp2679,( void*) _temp2679,(
void*)( _temp2679 + 1u)}; _temp2678( _temp2682, _temp2683, _temp2680);}});} goto
_LL2632;} _LL2632:;} goto _LL2469; _LL2481:{ void* _temp2685=* _temp2539; struct
Cyc_Absyn_UnknownTunionFieldInfo _temp2691; int _temp2693; struct _tuple1*
_temp2695; struct _tuple1* _temp2697; struct Cyc_Absyn_Tunionfield* _temp2699;
struct Cyc_Absyn_Tuniondecl* _temp2701; _LL2687: if(*(( int*) _temp2685) == Cyc_Absyn_UnknownTunionfield){
_LL2692: _temp2691=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp2685)->f1;
_LL2698: _temp2697= _temp2691.tunion_name; goto _LL2696; _LL2696: _temp2695=
_temp2691.field_name; goto _LL2694; _LL2694: _temp2693= _temp2691.is_xtunion;
goto _LL2688;} else{ goto _LL2689;} _LL2689: if(*(( int*) _temp2685) == Cyc_Absyn_KnownTunionfield){
_LL2702: _temp2701=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp2685)->f1;
goto _LL2700; _LL2700: _temp2699=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2685)->f2; goto _LL2690;} else{ goto _LL2686;} _LL2688: { struct Cyc_Absyn_Tuniondecl*
tud; struct Cyc_Absyn_Tunionfield* tuf;{ struct _handler_cons _temp2703;
_push_handler(& _temp2703);{ int _temp2705= 0; if( setjmp( _temp2703.handler)){
_temp2705= 1;} if( ! _temp2705){* Cyc_Tcenv_lookup_tuniondecl( te, loc,
_temp2697);; _pop_handler();} else{ void* _temp2704=( void*) _exn_thrown; void*
_temp2707= _temp2704; _LL2709: if( _temp2707 == Cyc_Dict_Absent){ goto _LL2710;}
else{ goto _LL2711;} _LL2711: goto _LL2712; _LL2710:({ void(* _temp2713)( struct
Cyc_Position_Segment* loc, struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr;
struct Cyc_Position_Segment* _temp2717= loc; struct _tagged_arr _temp2718=
_tag_arr("unbound type tunion %s", sizeof( unsigned char), 23u); struct Cyc_Stdio_String_pa_struct
_temp2719; _temp2719.tag= Cyc_Stdio_String_pa; _temp2719.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2697);{ void* _temp2716=( void*)& _temp2719; void*
_temp2714[ 1u]={ _temp2716}; struct _tagged_arr _temp2715={( void*) _temp2714,(
void*) _temp2714,( void*)( _temp2714 + 1u)}; _temp2713( _temp2717, _temp2718,
_temp2715);}}); return cvtenv; _LL2712:( void) _throw( _temp2707); _LL2708:;}}}{
struct _handler_cons _temp2720; _push_handler(& _temp2720);{ int _temp2722= 0;
if( setjmp( _temp2720.handler)){ _temp2722= 1;} if( ! _temp2722){{ void*
_temp2723= Cyc_Tcenv_lookup_ordinary( te, loc, _temp2695); struct Cyc_Absyn_Tunionfield*
_temp2729; struct Cyc_Absyn_Tuniondecl* _temp2731; _LL2725: if(*(( int*)
_temp2723) == Cyc_Tcenv_TunionRes){ _LL2732: _temp2731=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp2723)->f1; goto _LL2730; _LL2730: _temp2729=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp2723)->f2; goto _LL2726;} else{ goto _LL2727;} _LL2727: goto _LL2728;
_LL2726: tuf= _temp2729; tud= _temp2731; if( tud->is_xtunion != _temp2693){({
void(* _temp2733)( struct Cyc_Position_Segment* loc, struct _tagged_arr fmt,
struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp2737=
loc; struct _tagged_arr _temp2738= _tag_arr("[x]tunion is different from type declaration %s",
sizeof( unsigned char), 48u); struct Cyc_Stdio_String_pa_struct _temp2739;
_temp2739.tag= Cyc_Stdio_String_pa; _temp2739.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2697);{ void* _temp2736=( void*)& _temp2739; void* _temp2734[ 1u]={
_temp2736}; struct _tagged_arr _temp2735={( void*) _temp2734,( void*) _temp2734,(
void*)( _temp2734 + 1u)}; _temp2733( _temp2737, _temp2738, _temp2735);}});} goto
_LL2724; _LL2728:({ void(* _temp2740)( struct Cyc_Position_Segment* loc, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp2745= loc; struct _tagged_arr _temp2746= _tag_arr("unbound field %s in type tunion %s",
sizeof( unsigned char), 35u); struct Cyc_Stdio_String_pa_struct _temp2748;
_temp2748.tag= Cyc_Stdio_String_pa; _temp2748.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2695);{ void* _temp2743=( void*)& _temp2748; struct Cyc_Stdio_String_pa_struct
_temp2747; _temp2747.tag= Cyc_Stdio_String_pa; _temp2747.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2697);{ void* _temp2744=( void*)& _temp2747; void*
_temp2741[ 2u]={ _temp2743, _temp2744}; struct _tagged_arr _temp2742={( void*)
_temp2741,( void*) _temp2741,( void*)( _temp2741 + 2u)}; _temp2740( _temp2745,
_temp2746, _temp2742);}}});{ struct Cyc_Tcutil_CVTEnv _temp2749= cvtenv;
_npop_handler( 0u); return _temp2749;} _LL2724:;}; _pop_handler();} else{ void*
_temp2721=( void*) _exn_thrown; void* _temp2751= _temp2721; _LL2753: if(
_temp2751 == Cyc_Dict_Absent){ goto _LL2754;} else{ goto _LL2755;} _LL2755: goto
_LL2756; _LL2754:({ void(* _temp2757)( struct Cyc_Position_Segment* loc, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp2762= loc; struct _tagged_arr _temp2763= _tag_arr("unbound field %s in type tunion %s",
sizeof( unsigned char), 35u); struct Cyc_Stdio_String_pa_struct _temp2765;
_temp2765.tag= Cyc_Stdio_String_pa; _temp2765.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2695);{ void* _temp2760=( void*)& _temp2765; struct Cyc_Stdio_String_pa_struct
_temp2764; _temp2764.tag= Cyc_Stdio_String_pa; _temp2764.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2697);{ void* _temp2761=( void*)& _temp2764; void*
_temp2758[ 2u]={ _temp2760, _temp2761}; struct _tagged_arr _temp2759={( void*)
_temp2758,( void*) _temp2758,( void*)( _temp2758 + 2u)}; _temp2757( _temp2762,
_temp2763, _temp2759);}}}); return cvtenv; _LL2756:( void) _throw( _temp2751);
_LL2752:;}}}* _temp2539=( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp2766=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp2766[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp2767; _temp2767.tag= Cyc_Absyn_KnownTunionfield; _temp2767.f1= tud;
_temp2767.f2= tuf; _temp2767;}); _temp2766;}); _temp2701= tud; _temp2699= tuf;
goto _LL2690;} _LL2690: { struct Cyc_List_List* tvs= _temp2701->tvs; for( 0;
_temp2535 != 0? tvs != 0: 0;( _temp2535=(( struct Cyc_List_List*) _check_null(
_temp2535))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2535))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} if( _temp2535
!= 0){({ void(* _temp2768)( struct Cyc_Position_Segment* loc, struct _tagged_arr
fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp2773= loc; struct _tagged_arr _temp2774= _tag_arr("too many type arguments for tunion %s.%s",
sizeof( unsigned char), 41u); struct Cyc_Stdio_String_pa_struct _temp2776;
_temp2776.tag= Cyc_Stdio_String_pa; _temp2776.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2701->name);{ void* _temp2771=( void*)& _temp2776; struct Cyc_Stdio_String_pa_struct
_temp2775; _temp2775.tag= Cyc_Stdio_String_pa; _temp2775.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2699->name);{ void* _temp2772=( void*)& _temp2775;
void* _temp2769[ 2u]={ _temp2771, _temp2772}; struct _tagged_arr _temp2770={(
void*) _temp2769,( void*) _temp2769,( void*)( _temp2769 + 2u)}; _temp2768(
_temp2773, _temp2774, _temp2770);}}});} if( tvs != 0){({ void(* _temp2777)(
struct Cyc_Position_Segment* loc, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp2782= loc; struct _tagged_arr
_temp2783= _tag_arr("too few type arguments for tunion %s.%s", sizeof(
unsigned char), 40u); struct Cyc_Stdio_String_pa_struct _temp2785; _temp2785.tag=
Cyc_Stdio_String_pa; _temp2785.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2701->name);{ void* _temp2780=( void*)& _temp2785; struct Cyc_Stdio_String_pa_struct
_temp2784; _temp2784.tag= Cyc_Stdio_String_pa; _temp2784.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2699->name);{ void* _temp2781=( void*)& _temp2784;
void* _temp2778[ 2u]={ _temp2780, _temp2781}; struct _tagged_arr _temp2779={(
void*) _temp2778,( void*) _temp2778,( void*)( _temp2778 + 2u)}; _temp2777(
_temp2782, _temp2783, _temp2779);}}});} goto _LL2686;} _LL2686:;} goto _LL2469;
_LL2483: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_AnyKind,
_temp2550); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2548);{ void* _temp2786=( void*)( Cyc_Absyn_compress_conref( _temp2542))->v;
void* _temp2792; struct Cyc_Absyn_Exp* _temp2794; _LL2788: if(( unsigned int)
_temp2786 > 1u?*(( int*) _temp2786) == Cyc_Absyn_Eq_constr: 0){ _LL2793:
_temp2792=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2786)->f1; if((
unsigned int) _temp2792 > 1u?*(( int*) _temp2792) == Cyc_Absyn_Upper_b: 0){
_LL2795: _temp2794=(( struct Cyc_Absyn_Upper_b_struct*) _temp2792)->f1; goto
_LL2789;} else{ goto _LL2790;}} else{ goto _LL2790;} _LL2790: goto _LL2791;
_LL2789: if( ! Cyc_Tcutil_is_const_exp( te, _temp2794)){({ void(* _temp2796)(
struct Cyc_Position_Segment* loc, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp2799= loc; struct _tagged_arr
_temp2800= _tag_arr("pointer bounds expression is not a constant", sizeof(
unsigned char), 44u); void* _temp2797[ 0u]={}; struct _tagged_arr _temp2798={(
void*) _temp2797,( void*) _temp2797,( void*)( _temp2797 + 0u)}; _temp2796(
_temp2799, _temp2800, _temp2798);});} Cyc_Tcexp_tcExp( te, 0, _temp2794); if( !
Cyc_Tcutil_coerce_uint_typ( te, _temp2794)){({ void(* _temp2801)( struct Cyc_Position_Segment*
loc, struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp2804= loc; struct _tagged_arr _temp2805= _tag_arr("pointer bounds expression is not an unsigned int",
sizeof( unsigned char), 49u); void* _temp2802[ 0u]={}; struct _tagged_arr
_temp2803={( void*) _temp2802,( void*) _temp2802,( void*)( _temp2802 + 0u)};
_temp2801( _temp2804, _temp2805, _temp2803);});} Cyc_Evexp_eval_const_uint_exp(
_temp2794); goto _LL2787; _LL2791: goto _LL2787; _LL2787:;} goto _LL2469;
_LL2485: goto _LL2469; _LL2487: goto _LL2469; _LL2489: goto _LL2469; _LL2491:
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,
_temp2556); if( _temp2552 == 0? 1: ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp2552))){({ void(* _temp2806)( struct Cyc_Position_Segment* loc,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp2809= loc; struct _tagged_arr _temp2810= _tag_arr("array bounds expression is not constant",
sizeof( unsigned char), 40u); void* _temp2807[ 0u]={}; struct _tagged_arr
_temp2808={( void*) _temp2807,( void*) _temp2807,( void*)( _temp2807 + 0u)};
_temp2806( _temp2809, _temp2810, _temp2808);});} Cyc_Tcexp_tcExp( te, 0,( struct
Cyc_Absyn_Exp*) _check_null( _temp2552)); if( ! Cyc_Tcutil_coerce_uint_typ( te,(
struct Cyc_Absyn_Exp*) _check_null( _temp2552))){({ void(* _temp2811)( struct
Cyc_Position_Segment* loc, struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr;
struct Cyc_Position_Segment* _temp2814= loc; struct _tagged_arr _temp2815=
_tag_arr("array bounds expression is not an unsigned int", sizeof( unsigned char),
47u); void* _temp2812[ 0u]={}; struct _tagged_arr _temp2813={( void*) _temp2812,(
void*) _temp2812,( void*)( _temp2812 + 0u)}; _temp2811( _temp2814, _temp2815,
_temp2813);});} Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp2552)); goto _LL2469; _LL2493: { int num_convs= 0; int
seen_cdecl= 0; int seen_stdcall= 0; int seen_fastcall= 0; int seen_format= 0;
void* ft=( void*) Cyc_Absyn_Printf_ft; int fmt_desc_arg= - 1; int fmt_arg_start=
- 1; for( 0; _temp2560 != 0; _temp2560=(( struct Cyc_List_List*) _check_null(
_temp2560))->tl){ if( ! Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*)
_check_null( _temp2560))->hd)){({ void(* _temp2816)( struct Cyc_Position_Segment*
loc, struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp2820= loc; struct _tagged_arr _temp2821= _tag_arr("bad function type attribute %s",
sizeof( unsigned char), 31u); struct Cyc_Stdio_String_pa_struct _temp2822;
_temp2822.tag= Cyc_Stdio_String_pa; _temp2822.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp2560))->hd);{ void* _temp2819=(
void*)& _temp2822; void* _temp2817[ 1u]={ _temp2819}; struct _tagged_arr
_temp2818={( void*) _temp2817,( void*) _temp2817,( void*)( _temp2817 + 1u)};
_temp2816( _temp2820, _temp2821, _temp2818);}});}{ void* _temp2823=( void*)((
struct Cyc_List_List*) _check_null( _temp2560))->hd; int _temp2835; int
_temp2837; void* _temp2839; _LL2825: if( _temp2823 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL2826;} else{ goto _LL2827;} _LL2827: if( _temp2823 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL2828;} else{ goto _LL2829;} _LL2829: if( _temp2823 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL2830;} else{ goto _LL2831;} _LL2831: if(( unsigned int) _temp2823 > 16u?*((
int*) _temp2823) == Cyc_Absyn_Format_att: 0){ _LL2840: _temp2839=( void*)((
struct Cyc_Absyn_Format_att_struct*) _temp2823)->f1; goto _LL2838; _LL2838:
_temp2837=(( struct Cyc_Absyn_Format_att_struct*) _temp2823)->f2; goto _LL2836;
_LL2836: _temp2835=(( struct Cyc_Absyn_Format_att_struct*) _temp2823)->f3; goto
_LL2832;} else{ goto _LL2833;} _LL2833: goto _LL2834; _LL2826: if( !
seen_stdcall){ seen_stdcall= 1; ++ num_convs;} goto _LL2824; _LL2828: if( !
seen_cdecl){ seen_cdecl= 1; ++ num_convs;} goto _LL2824; _LL2830: if( !
seen_fastcall){ seen_fastcall= 1; ++ num_convs;} goto _LL2824; _LL2832: if( !
seen_format){ seen_format= 1; ft= _temp2839; fmt_desc_arg= _temp2837;
fmt_arg_start= _temp2835;} else{({ void(* _temp2841)( struct Cyc_Position_Segment*
loc, struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp2844= loc; struct _tagged_arr _temp2845= _tag_arr("function can't have multiple format attributes",
sizeof( unsigned char), 47u); void* _temp2842[ 0u]={}; struct _tagged_arr
_temp2843={( void*) _temp2842,( void*) _temp2842,( void*)( _temp2842 + 0u)};
_temp2841( _temp2844, _temp2845, _temp2843);});} goto _LL2824; _LL2834: goto
_LL2824; _LL2824:;}} if( num_convs > 1){({ void(* _temp2846)( struct Cyc_Position_Segment*
loc, struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp2849= loc; struct _tagged_arr _temp2850= _tag_arr("function can't have multiple calling conventions",
sizeof( unsigned char), 49u); void* _temp2847[ 0u]={}; struct _tagged_arr
_temp2848={( void*) _temp2847,( void*) _temp2847,( void*)( _temp2847 + 0u)};
_temp2846( _temp2849, _temp2850, _temp2848);});} Cyc_Tcutil_check_unique_tvars(
loc,* _temp2577);{ struct Cyc_List_List* b=* _temp2577; for( 0; b != 0; b=((
struct Cyc_List_List*) _check_null( b))->tl){(( struct Cyc_Absyn_Tvar*)(( struct
Cyc_List_List*) _check_null( b))->hd)->identity= Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env= Cyc_Tcutil_add_bound_tvar( cvtenv.kind_env,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( b))->hd);{ void* _temp2851=( void*)( Cyc_Absyn_compress_conref(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd)->kind))->v;
void* _temp2857; _LL2853: if(( unsigned int) _temp2851 > 1u?*(( int*) _temp2851)
== Cyc_Absyn_Eq_constr: 0){ _LL2858: _temp2857=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2851)->f1; if( _temp2857 ==( void*) Cyc_Absyn_MemKind){ goto _LL2854;}
else{ goto _LL2855;}} else{ goto _LL2855;} _LL2855: goto _LL2856; _LL2854:({
void(* _temp2859)( struct Cyc_Position_Segment* loc, struct _tagged_arr fmt,
struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp2863=
loc; struct _tagged_arr _temp2864= _tag_arr("function attempts to abstract Mem type variable %s",
sizeof( unsigned char), 51u); struct Cyc_Stdio_String_pa_struct _temp2865;
_temp2865.tag= Cyc_Stdio_String_pa; _temp2865.f1=( struct _tagged_arr)*(( struct
Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd)->name;{ void*
_temp2862=( void*)& _temp2865; void* _temp2860[ 1u]={ _temp2862}; struct
_tagged_arr _temp2861={( void*) _temp2860,( void*) _temp2860,( void*)( _temp2860
+ 1u)}; _temp2859( _temp2863, _temp2864, _temp2861);}}); goto _LL2852; _LL2856:
goto _LL2852; _LL2852:;}}}{ struct Cyc_Tcutil_CVTEnv _temp2866=({ struct Cyc_Tcutil_CVTEnv
_temp3078; _temp3078.kind_env= cvtenv.kind_env; _temp3078.free_vars= 0;
_temp3078.free_evars= 0; _temp3078;}); _temp2866= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2866,( void*) Cyc_Absyn_MemKind, _temp2570);{ struct Cyc_List_List*
a= _temp2568; for( 0; a != 0; a=(( struct Cyc_List_List*) _check_null( a))->tl){
_temp2866= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2866,( void*) Cyc_Absyn_MemKind,(*((
struct _tuple2*)(( struct Cyc_List_List*) _check_null( a))->hd)).f3);}} if(
_temp2564 != 0){ if( _temp2566){({ int(* _temp2867)( struct _tagged_arr fmt,
struct _tagged_arr ap)=( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos; struct _tagged_arr _temp2870= _tag_arr("both c_vararg and cyc_vararg",
sizeof( unsigned char), 29u); void* _temp2868[ 0u]={}; struct _tagged_arr
_temp2869={( void*) _temp2868,( void*) _temp2868,( void*)( _temp2868 + 0u)};
_temp2867( _temp2870, _temp2869);});}{ int _temp2873; void* _temp2875; void*
_temp2877; struct Cyc_Absyn_Tqual _temp2879; struct Cyc_Core_Opt* _temp2881;
struct Cyc_Absyn_VarargInfo _temp2871=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp2564)); _LL2882: _temp2881= _temp2871.name; goto _LL2880;
_LL2880: _temp2879= _temp2871.tq; goto _LL2878; _LL2878: _temp2877=( void*)
_temp2871.type; goto _LL2876; _LL2876: _temp2875=( void*) _temp2871.rgn; goto
_LL2874; _LL2874: _temp2873= _temp2871.inject; goto _LL2872; _LL2872: _temp2866=
Cyc_Tcutil_i_check_valid_type( loc, te, _temp2866,( void*) Cyc_Absyn_MemKind,
_temp2877);{ void* _temp2883= Cyc_Tcutil_compress( _temp2875); _LL2885: if((
unsigned int) _temp2883 > 4u?*(( int*) _temp2883) == Cyc_Absyn_Evar: 0){ goto
_LL2886;} else{ goto _LL2887;} _LL2887: if(( unsigned int) _temp2883 > 4u?*((
int*) _temp2883) == Cyc_Absyn_VarType: 0){ goto _LL2888;} else{ goto _LL2889;}
_LL2889: if( _temp2883 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL2890;} else{ goto
_LL2891;} _LL2891: goto _LL2892; _LL2886: { void* vr_tvar=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2893=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2893[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2894; _temp2894.tag= Cyc_Absyn_VarType;
_temp2894.f1= Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_RgnKind); _temp2894;});
_temp2893;}); Cyc_Tcutil_unify( _temp2875, vr_tvar); goto _LL2884;} _LL2888:
goto _LL2884; _LL2890:({ void(* _temp2895)( struct Cyc_Position_Segment* loc,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp2898= loc; struct _tagged_arr _temp2899= _tag_arr("varargs should be in a region, not heap-allocated",
sizeof( unsigned char), 50u); void* _temp2896[ 0u]={}; struct _tagged_arr
_temp2897={( void*) _temp2896,( void*) _temp2896,( void*)( _temp2896 + 0u)};
_temp2895( _temp2898, _temp2899, _temp2897);}); goto _LL2884; _LL2892: goto
_LL2884; _LL2884:;} _temp2866= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2866,(
void*) Cyc_Absyn_RgnKind, _temp2875); if( _temp2873){ void* _temp2900= Cyc_Tcutil_compress(
_temp2877); struct Cyc_Absyn_TunionInfo _temp2906; void* _temp2908; void*
_temp2910; _LL2902: if(( unsigned int) _temp2900 > 4u?*(( int*) _temp2900) ==
Cyc_Absyn_TunionType: 0){ _LL2907: _temp2906=(( struct Cyc_Absyn_TunionType_struct*)
_temp2900)->f1; _LL2911: _temp2910=( void*) _temp2906.tunion_info; if(*(( int*)
_temp2910) == Cyc_Absyn_KnownTunion){ goto _LL2909;} else{ goto _LL2904;}
_LL2909: _temp2908=( void*) _temp2906.rgn; goto _LL2903;} else{ goto _LL2904;}
_LL2904: goto _LL2905; _LL2903:{ void* _temp2912= Cyc_Tcutil_compress( _temp2908);
_LL2914: if(( unsigned int) _temp2912 > 4u?*(( int*) _temp2912) == Cyc_Absyn_Evar:
0){ goto _LL2915;} else{ goto _LL2916;} _LL2916: if(( unsigned int) _temp2912 >
4u?*(( int*) _temp2912) == Cyc_Absyn_VarType: 0){ goto _LL2917;} else{ goto
_LL2918;} _LL2918: if( _temp2912 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL2919;}
else{ goto _LL2920;} _LL2920: goto _LL2921; _LL2915: Cyc_Tcutil_unify( _temp2875,
_temp2908); goto _LL2913; _LL2917: goto _LL2913; _LL2919:({ void(* _temp2922)(
struct Cyc_Position_Segment* loc, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp2925= loc; struct _tagged_arr
_temp2926= _tag_arr("injected vararg [x]tunion should be in a region, not heap-allocated",
sizeof( unsigned char), 68u); void* _temp2923[ 0u]={}; struct _tagged_arr
_temp2924={( void*) _temp2923,( void*) _temp2923,( void*)( _temp2923 + 0u)};
_temp2922( _temp2925, _temp2926, _temp2924);}); goto _LL2913; _LL2921: goto
_LL2913; _LL2913:;} goto _LL2901; _LL2905:({ void(* _temp2927)( struct Cyc_Position_Segment*
loc, struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp2930= loc; struct _tagged_arr _temp2931= _tag_arr("can't inject a non-[x]tunion type",
sizeof( unsigned char), 34u); void* _temp2928[ 0u]={}; struct _tagged_arr
_temp2929={( void*) _temp2928,( void*) _temp2928,( void*)( _temp2928 + 0u)};
_temp2927( _temp2930, _temp2931, _temp2929);}); goto _LL2901; _LL2901:;}}} if(
seen_format){ int _temp2932=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp2568); if(((( fmt_desc_arg < 0? 1: fmt_desc_arg > _temp2932)? 1:
fmt_arg_start < 0)? 1:( _temp2564 == 0? fmt_arg_start != 0: 0))? 1:( _temp2564
!= 0? fmt_arg_start != _temp2932 + 1: 0)){({ void(* _temp2933)( struct Cyc_Position_Segment*
loc, struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp2936= loc; struct _tagged_arr _temp2937= _tag_arr("bad format descriptor",
sizeof( unsigned char), 22u); void* _temp2934[ 0u]={}; struct _tagged_arr
_temp2935={( void*) _temp2934,( void*) _temp2934,( void*)( _temp2934 + 0u)};
_temp2933( _temp2936, _temp2937, _temp2935);});} else{ void* _temp2940; struct
_tuple2 _temp2938=*(( struct _tuple2*(*)( struct Cyc_List_List* x, int i)) Cyc_List_nth)(
_temp2568, fmt_desc_arg - 1); _LL2941: _temp2940= _temp2938.f3; goto _LL2939;
_LL2939:{ void* _temp2942= Cyc_Tcutil_compress( _temp2940); struct Cyc_Absyn_PtrInfo
_temp2948; struct Cyc_Absyn_Conref* _temp2950; void* _temp2952; _LL2944: if((
unsigned int) _temp2942 > 4u?*(( int*) _temp2942) == Cyc_Absyn_PointerType: 0){
_LL2949: _temp2948=(( struct Cyc_Absyn_PointerType_struct*) _temp2942)->f1;
_LL2953: _temp2952=( void*) _temp2948.elt_typ; goto _LL2951; _LL2951: _temp2950=
_temp2948.bounds; goto _LL2945;} else{ goto _LL2946;} _LL2946: goto _LL2947;
_LL2945:{ struct _tuple8 _temp2955=({ struct _tuple8 _temp2954; _temp2954.f1=
Cyc_Tcutil_compress( _temp2952); _temp2954.f2= Cyc_Absyn_conref_def(( void*) Cyc_Absyn_Unknown_b,
_temp2950); _temp2954;}); void* _temp2961; void* _temp2963; void* _temp2965;
void* _temp2967; _LL2957: _LL2964: _temp2963= _temp2955.f1; if(( unsigned int)
_temp2963 > 4u?*(( int*) _temp2963) == Cyc_Absyn_IntType: 0){ _LL2968: _temp2967=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2963)->f1; if( _temp2967 ==(
void*) Cyc_Absyn_Unsigned){ goto _LL2966;} else{ goto _LL2959;} _LL2966:
_temp2965=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2963)->f2; if(
_temp2965 ==( void*) Cyc_Absyn_B1){ goto _LL2962;} else{ goto _LL2959;}} else{
goto _LL2959;} _LL2962: _temp2961= _temp2955.f2; if( _temp2961 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL2958;} else{ goto _LL2959;} _LL2959: goto _LL2960; _LL2958: goto _LL2956;
_LL2960:({ void(* _temp2969)( struct Cyc_Position_Segment* loc, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp2972= loc; struct _tagged_arr _temp2973= _tag_arr("format descriptor is not a char ? type",
sizeof( unsigned char), 39u); void* _temp2970[ 0u]={}; struct _tagged_arr
_temp2971={( void*) _temp2970,( void*) _temp2970,( void*)( _temp2970 + 0u)};
_temp2969( _temp2972, _temp2973, _temp2971);}); goto _LL2956; _LL2956:;} goto
_LL2943; _LL2947:({ void(* _temp2974)( struct Cyc_Position_Segment* loc, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp2977= loc; struct _tagged_arr _temp2978= _tag_arr("format descriptor is not a char ? type",
sizeof( unsigned char), 39u); void* _temp2975[ 0u]={}; struct _tagged_arr
_temp2976={( void*) _temp2975,( void*) _temp2975,( void*)( _temp2975 + 0u)};
_temp2974( _temp2977, _temp2978, _temp2976);}); goto _LL2943; _LL2943:;} if(
fmt_arg_start != 0){ void* _temp2979= Cyc_Tcutil_compress(( void*)(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp2564))->type); int _temp2980= 0;{ void* _temp2981= ft; _LL2983:
if( _temp2981 ==( void*) Cyc_Absyn_Printf_ft){ goto _LL2984;} else{ goto _LL2985;}
_LL2985: if( _temp2981 ==( void*) Cyc_Absyn_Scanf_ft){ goto _LL2986;} else{ goto
_LL2982;} _LL2984:{ void* _temp2987= _temp2979; struct Cyc_Absyn_TunionInfo
_temp2993; void* _temp2995; struct Cyc_Absyn_Tuniondecl* _temp2997; _LL2989: if((
unsigned int) _temp2987 > 4u?*(( int*) _temp2987) == Cyc_Absyn_TunionType: 0){
_LL2994: _temp2993=(( struct Cyc_Absyn_TunionType_struct*) _temp2987)->f1;
_LL2996: _temp2995=( void*) _temp2993.tunion_info; if(*(( int*) _temp2995) ==
Cyc_Absyn_KnownTunion){ _LL2998: _temp2997=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp2995)->f1; goto _LL2990;} else{ goto _LL2991;}} else{ goto _LL2991;}
_LL2991: goto _LL2992; _LL2990: _temp2980= Cyc_Absyn_qvar_cmp( _temp2997->name,
Cyc_Absyn_tunion_print_arg_qvar) != 0; goto _LL2988; _LL2992: _temp2980= 1; goto
_LL2988; _LL2988:;} goto _LL2982; _LL2986:{ void* _temp2999= _temp2979; struct
Cyc_Absyn_TunionInfo _temp3005; void* _temp3007; struct Cyc_Absyn_Tuniondecl*
_temp3009; _LL3001: if(( unsigned int) _temp2999 > 4u?*(( int*) _temp2999) ==
Cyc_Absyn_TunionType: 0){ _LL3006: _temp3005=(( struct Cyc_Absyn_TunionType_struct*)
_temp2999)->f1; _LL3008: _temp3007=( void*) _temp3005.tunion_info; if(*(( int*)
_temp3007) == Cyc_Absyn_KnownTunion){ _LL3010: _temp3009=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp3007)->f1; goto _LL3002;} else{ goto _LL3003;}} else{ goto _LL3003;}
_LL3003: goto _LL3004; _LL3002: _temp2980= Cyc_Absyn_qvar_cmp( _temp3009->name,
Cyc_Absyn_tunion_scanf_arg_qvar) != 0; goto _LL3000; _LL3004: _temp2980= 1; goto
_LL3000; _LL3000:;} goto _LL2982; _LL2982:;} if( _temp2980){({ void(* _temp3011)(
struct Cyc_Position_Segment* loc, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp3014= loc; struct _tagged_arr
_temp3015= _tag_arr("format attribute and vararg types don't match", sizeof(
unsigned char), 46u); void* _temp3012[ 0u]={}; struct _tagged_arr _temp3013={(
void*) _temp3012,( void*) _temp3012,( void*)( _temp3012 + 0u)}; _temp3011(
_temp3014, _temp3015, _temp3013);});}}}}{ struct Cyc_List_List* rpo= _temp2562;
for( 0; rpo != 0; rpo=(( struct Cyc_List_List*) _check_null( rpo))->tl){ struct
_tuple8 _temp3018; void* _temp3019; void* _temp3021; struct _tuple8* _temp3016=(
struct _tuple8*)(( struct Cyc_List_List*) _check_null( rpo))->hd; _temp3018=*
_temp3016; _LL3022: _temp3021= _temp3018.f1; goto _LL3020; _LL3020: _temp3019=
_temp3018.f2; goto _LL3017; _LL3017: _temp2866= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2866,( void*) Cyc_Absyn_RgnKind, _temp3021); _temp2866= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2866,( void*) Cyc_Absyn_RgnKind, _temp3019);}} if(* _temp2574 != 0){
_temp2866= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2866,( void*) Cyc_Absyn_EffKind,(
void*)(( struct Cyc_Core_Opt*) _check_null(* _temp2574))->v);} else{ struct Cyc_Absyn_Tvar*
e= Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_EffKind); _temp2866.free_vars= Cyc_Tcutil_add_free_tvar(
loc, _temp2866.free_vars, e);{ struct Cyc_List_List* effect= 0;{ struct Cyc_List_List*
tvs= _temp2866.free_vars; for( 0; tvs != 0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){ void* _temp3023=( void*)( Cyc_Absyn_compress_conref(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind))->v;
void* _temp3031; void* _temp3033; _LL3025: if(( unsigned int) _temp3023 > 1u?*((
int*) _temp3023) == Cyc_Absyn_Eq_constr: 0){ _LL3032: _temp3031=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp3023)->f1; if( _temp3031 ==( void*) Cyc_Absyn_RgnKind){
goto _LL3026;} else{ goto _LL3027;}} else{ goto _LL3027;} _LL3027: if((
unsigned int) _temp3023 > 1u?*(( int*) _temp3023) == Cyc_Absyn_Eq_constr: 0){
_LL3034: _temp3033=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3023)->f1;
if( _temp3033 ==( void*) Cyc_Absyn_EffKind){ goto _LL3028;} else{ goto _LL3029;}}
else{ goto _LL3029;} _LL3029: goto _LL3030; _LL3026: effect=({ struct Cyc_List_List*
_temp3035=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3035->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3036=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3036[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3037; _temp3037.tag= Cyc_Absyn_AccessEff;
_temp3037.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3038=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3038[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3039; _temp3039.tag= Cyc_Absyn_VarType;
_temp3039.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3039;}); _temp3038;})); _temp3037;}); _temp3036;})); _temp3035->tl= effect;
_temp3035;}); goto _LL3024; _LL3028: effect=({ struct Cyc_List_List* _temp3040=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3040->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3041=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp3041[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp3042; _temp3042.tag= Cyc_Absyn_VarType; _temp3042.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3042;}); _temp3041;})); _temp3040->tl= effect; _temp3040;}); goto _LL3024;
_LL3030: goto _LL3024; _LL3024:;}}{ struct Cyc_List_List* ts= _temp2866.free_evars;
for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){ void*
_temp3043= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);
_LL3045: if( _temp3043 ==( void*) Cyc_Absyn_RgnKind){ goto _LL3046;} else{ goto
_LL3047;} _LL3047: if( _temp3043 ==( void*) Cyc_Absyn_EffKind){ goto _LL3048;}
else{ goto _LL3049;} _LL3049: goto _LL3050; _LL3046: effect=({ struct Cyc_List_List*
_temp3051=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3051->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3052=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3052[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3053; _temp3053.tag= Cyc_Absyn_AccessEff;
_temp3053.f1=( void*)(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);
_temp3053;}); _temp3052;})); _temp3051->tl= effect; _temp3051;}); goto _LL3044;
_LL3048: effect=({ struct Cyc_List_List* _temp3054=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3054->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( ts))->hd); _temp3054->tl= effect; _temp3054;});
goto _LL3044; _LL3050: goto _LL3044; _LL3044:;}}* _temp2574=({ struct Cyc_Core_Opt*
_temp3055=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3055->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3056=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3056[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3057; _temp3057.tag= Cyc_Absyn_JoinEff;
_temp3057.f1= Cyc_List_imp_rev( effect); _temp3057;}); _temp3056;})); _temp3055;});}}
if(* _temp2577 != 0){ struct Cyc_List_List* bs=* _temp2577; for( 0; bs != 0; bs=((
struct Cyc_List_List*) _check_null( bs))->tl){ struct Cyc_Absyn_Conref*
_temp3058= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( bs))->hd)->kind); void* _temp3059=( void*) _temp3058->v; void*
_temp3067; _LL3061: if( _temp3059 ==( void*) Cyc_Absyn_No_constr){ goto _LL3062;}
else{ goto _LL3063;} _LL3063: if(( unsigned int) _temp3059 > 1u?*(( int*)
_temp3059) == Cyc_Absyn_Eq_constr: 0){ _LL3068: _temp3067=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3059)->f1; if( _temp3067 ==( void*) Cyc_Absyn_MemKind){ goto _LL3064;}
else{ goto _LL3065;}} else{ goto _LL3065;} _LL3065: goto _LL3066; _LL3062:({
void(* _temp3069)( struct Cyc_Position_Segment* sg, struct _tagged_arr fmt,
struct _tagged_arr ap)= Cyc_Tcutil_warn; struct Cyc_Position_Segment* _temp3073=
loc; struct _tagged_arr _temp3074= _tag_arr("Type variable %s unconstrained, assuming boxed",
sizeof( unsigned char), 47u); struct Cyc_Stdio_String_pa_struct _temp3075;
_temp3075.tag= Cyc_Stdio_String_pa; _temp3075.f1=( struct _tagged_arr)*(( struct
Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( bs))->hd)->name;{ void*
_temp3072=( void*)& _temp3075; void* _temp3070[ 1u]={ _temp3072}; struct
_tagged_arr _temp3071={( void*) _temp3070,( void*) _temp3070,( void*)( _temp3070
+ 1u)}; _temp3069( _temp3073, _temp3074, _temp3071);}}); goto _LL3064; _LL3064:(
void*)( _temp3058->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3076=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3076[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3077; _temp3077.tag= Cyc_Absyn_Eq_constr;
_temp3077.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3077;}); _temp3076;})));
goto _LL3060; _LL3066: goto _LL3060; _LL3060:;}} cvtenv.kind_env= Cyc_Tcutil_remove_bound_tvars(
_temp2866.kind_env,* _temp2577); _temp2866.free_vars= Cyc_Tcutil_remove_bound_tvars(
_temp2866.free_vars,* _temp2577);{ struct Cyc_List_List* tvs= _temp2866.free_vars;
for( 0; tvs != 0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd);}}{ struct Cyc_List_List* evs=
_temp2866.free_evars; for( 0; evs != 0; evs=(( struct Cyc_List_List*)
_check_null( evs))->tl){ cvtenv.free_evars= Cyc_Tcutil_add_free_evar( cvtenv.free_evars,(
void*)(( struct Cyc_List_List*) _check_null( evs))->hd);}} goto _LL2469;}}
_LL2495: for( 0; _temp2578 != 0; _temp2578=(( struct Cyc_List_List*) _check_null(
_temp2578))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*)
Cyc_Absyn_MemKind,(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp2578))->hd)).f2);} goto _LL2469; _LL2497:{ struct _RegionHandle _temp3079=
_new_region(); struct _RegionHandle* sprev_rgn=& _temp3079; _push_region(
sprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for( 0; _temp2580 != 0;
_temp2580=(( struct Cyc_List_List*) _check_null( _temp2580))->tl){ struct Cyc_Absyn_Structfield
_temp3082; struct Cyc_List_List* _temp3083; struct Cyc_Core_Opt* _temp3085; void*
_temp3087; struct Cyc_Absyn_Tqual _temp3089; struct _tagged_arr* _temp3091;
struct Cyc_Absyn_Structfield* _temp3080=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2580))->hd; _temp3082=* _temp3080;
_LL3092: _temp3091= _temp3082.name; goto _LL3090; _LL3090: _temp3089= _temp3082.tq;
goto _LL3088; _LL3088: _temp3087=( void*) _temp3082.type; goto _LL3086; _LL3086:
_temp3085= _temp3082.width; goto _LL3084; _LL3084: _temp3083= _temp3082.attributes;
goto _LL3081; _LL3081: if((( int(*)( int(* compare)( struct _tagged_arr*, struct
_tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields, _temp3091)){({ void(* _temp3093)( struct Cyc_Position_Segment*
loc, struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp3097= loc; struct _tagged_arr _temp3098= _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u); struct Cyc_Stdio_String_pa_struct _temp3099;
_temp3099.tag= Cyc_Stdio_String_pa; _temp3099.f1=( struct _tagged_arr)*
_temp3091;{ void* _temp3096=( void*)& _temp3099; void* _temp3094[ 1u]={
_temp3096}; struct _tagged_arr _temp3095={( void*) _temp3094,( void*) _temp3094,(
void*)( _temp3094 + 1u)}; _temp3093( _temp3097, _temp3098, _temp3095);}});} if(
Cyc_String_strcmp(* _temp3091, _tag_arr("", sizeof( unsigned char), 1u)) != 0){
prev_fields=({ struct Cyc_List_List* _temp3100=( struct Cyc_List_List*)
_region_malloc( sprev_rgn, sizeof( struct Cyc_List_List)); _temp3100->hd=( void*)
_temp3091; _temp3100->tl= prev_fields; _temp3100;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp3087); if( _temp3085 != 0){
unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp3085))->v)){({ void(* _temp3101)( struct
Cyc_Position_Segment* loc, struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr;
struct Cyc_Position_Segment* _temp3105= loc; struct _tagged_arr _temp3106=
_tag_arr("bitfield %s does not have constant width", sizeof( unsigned char), 41u);
struct Cyc_Stdio_String_pa_struct _temp3107; _temp3107.tag= Cyc_Stdio_String_pa;
_temp3107.f1=( struct _tagged_arr)* _temp3091;{ void* _temp3104=( void*)&
_temp3107; void* _temp3102[ 1u]={ _temp3104}; struct _tagged_arr _temp3103={(
void*) _temp3102,( void*) _temp3102,( void*)( _temp3102 + 1u)}; _temp3101(
_temp3105, _temp3106, _temp3103);}});} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp3085))->v);}{
void* _temp3108= Cyc_Tcutil_compress( _temp3087); void* _temp3114; _LL3110: if((
unsigned int) _temp3108 > 4u?*(( int*) _temp3108) == Cyc_Absyn_IntType: 0){
_LL3115: _temp3114=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3108)->f2;
goto _LL3111;} else{ goto _LL3112;} _LL3112: goto _LL3113; _LL3111:{ void*
_temp3116= _temp3114; _LL3118: if( _temp3116 ==( void*) Cyc_Absyn_B1){ goto
_LL3119;} else{ goto _LL3120;} _LL3120: if( _temp3116 ==( void*) Cyc_Absyn_B2){
goto _LL3121;} else{ goto _LL3122;} _LL3122: if( _temp3116 ==( void*) Cyc_Absyn_B4){
goto _LL3123;} else{ goto _LL3124;} _LL3124: if( _temp3116 ==( void*) Cyc_Absyn_B8){
goto _LL3125;} else{ goto _LL3117;} _LL3119: if( w > 8){({ void(* _temp3126)(
struct Cyc_Position_Segment* loc, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp3129= loc; struct _tagged_arr
_temp3130= _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u);
void* _temp3127[ 0u]={}; struct _tagged_arr _temp3128={( void*) _temp3127,( void*)
_temp3127,( void*)( _temp3127 + 0u)}; _temp3126( _temp3129, _temp3130, _temp3128);});}
goto _LL3117; _LL3121: if( w > 16){({ void(* _temp3131)( struct Cyc_Position_Segment*
loc, struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp3134= loc; struct _tagged_arr _temp3135= _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u); void* _temp3132[ 0u]={}; struct _tagged_arr
_temp3133={( void*) _temp3132,( void*) _temp3132,( void*)( _temp3132 + 0u)};
_temp3131( _temp3134, _temp3135, _temp3133);});} goto _LL3117; _LL3123: if( w >
32){({ void(* _temp3136)( struct Cyc_Position_Segment* loc, struct _tagged_arr
fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp3139= loc; struct _tagged_arr _temp3140= _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u); void* _temp3137[ 0u]={}; struct _tagged_arr
_temp3138={( void*) _temp3137,( void*) _temp3137,( void*)( _temp3137 + 0u)};
_temp3136( _temp3139, _temp3140, _temp3138);});} goto _LL3117; _LL3125: if( w >
64){({ void(* _temp3141)( struct Cyc_Position_Segment* loc, struct _tagged_arr
fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp3144= loc; struct _tagged_arr _temp3145= _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u); void* _temp3142[ 0u]={}; struct _tagged_arr
_temp3143={( void*) _temp3142,( void*) _temp3142,( void*)( _temp3142 + 0u)};
_temp3141( _temp3144, _temp3145, _temp3143);});} goto _LL3117; _LL3117:;} goto
_LL3109; _LL3113:({ void(* _temp3146)( struct Cyc_Position_Segment* loc, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp3151= loc; struct _tagged_arr _temp3152= _tag_arr("bitfield %s must have integral type but has type %s",
sizeof( unsigned char), 52u); struct Cyc_Stdio_String_pa_struct _temp3154;
_temp3154.tag= Cyc_Stdio_String_pa; _temp3154.f1=( struct _tagged_arr)*
_temp3091;{ void* _temp3149=( void*)& _temp3154; struct Cyc_Stdio_String_pa_struct
_temp3153; _temp3153.tag= Cyc_Stdio_String_pa; _temp3153.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( _temp3087);{ void* _temp3150=( void*)& _temp3153; void*
_temp3147[ 2u]={ _temp3149, _temp3150}; struct _tagged_arr _temp3148={( void*)
_temp3147,( void*) _temp3147,( void*)( _temp3147 + 2u)}; _temp3146( _temp3151,
_temp3152, _temp3148);}}}); goto _LL3109; _LL3109:;}} for( 0; _temp3083 != 0;
_temp3083=(( struct Cyc_List_List*) _check_null( _temp3083))->tl){ void*
_temp3155=( void*)(( struct Cyc_List_List*) _check_null( _temp3083))->hd;
_LL3157: if( _temp3155 ==( void*) Cyc_Absyn_Packed_att){ goto _LL3158;} else{
goto _LL3159;} _LL3159: if(( unsigned int) _temp3155 > 16u?*(( int*) _temp3155)
== Cyc_Absyn_Aligned_att: 0){ goto _LL3160;} else{ goto _LL3161;} _LL3161: goto
_LL3162; _LL3158: continue; _LL3160: continue; _LL3162:({ void(* _temp3163)(
struct Cyc_Position_Segment* loc, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp3168= loc; struct _tagged_arr
_temp3169= _tag_arr("bad attribute %s on struct member %s", sizeof(
unsigned char), 37u); struct Cyc_Stdio_String_pa_struct _temp3171; _temp3171.tag=
Cyc_Stdio_String_pa; _temp3171.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp3083))->hd);{ void* _temp3166=(
void*)& _temp3171; struct Cyc_Stdio_String_pa_struct _temp3170; _temp3170.tag=
Cyc_Stdio_String_pa; _temp3170.f1=( struct _tagged_arr)* _temp3091;{ void*
_temp3167=( void*)& _temp3170; void* _temp3164[ 2u]={ _temp3166, _temp3167};
struct _tagged_arr _temp3165={( void*) _temp3164,( void*) _temp3164,( void*)(
_temp3164 + 2u)}; _temp3163( _temp3168, _temp3169, _temp3165);}}}); goto _LL3156;
_LL3156:;}}}; _pop_region( sprev_rgn);} goto _LL2469; _LL2499:{ struct
_RegionHandle _temp3172= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp3172; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for(
0; _temp2582 != 0; _temp2582=(( struct Cyc_List_List*) _check_null( _temp2582))->tl){
struct Cyc_Absyn_Structfield _temp3175; struct Cyc_List_List* _temp3176; struct
Cyc_Core_Opt* _temp3178; void* _temp3180; struct Cyc_Absyn_Tqual _temp3182;
struct _tagged_arr* _temp3184; struct Cyc_Absyn_Structfield* _temp3173=( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2582))->hd;
_temp3175=* _temp3173; _LL3185: _temp3184= _temp3175.name; goto _LL3183; _LL3183:
_temp3182= _temp3175.tq; goto _LL3181; _LL3181: _temp3180=( void*) _temp3175.type;
goto _LL3179; _LL3179: _temp3178= _temp3175.width; goto _LL3177; _LL3177:
_temp3176= _temp3175.attributes; goto _LL3174; _LL3174: if((( int(*)( int(*
compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l,
struct _tagged_arr* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,
_temp3184)){({ void(* _temp3186)( struct Cyc_Position_Segment* loc, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp3190= loc; struct _tagged_arr _temp3191= _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u); struct Cyc_Stdio_String_pa_struct _temp3192;
_temp3192.tag= Cyc_Stdio_String_pa; _temp3192.f1=( struct _tagged_arr)*
_temp3184;{ void* _temp3189=( void*)& _temp3192; void* _temp3187[ 1u]={
_temp3189}; struct _tagged_arr _temp3188={( void*) _temp3187,( void*) _temp3187,(
void*)( _temp3187 + 1u)}; _temp3186( _temp3190, _temp3191, _temp3188);}});} if(
Cyc_String_strcmp(* _temp3184, _tag_arr("", sizeof( unsigned char), 1u)) != 0){
prev_fields=({ struct Cyc_List_List* _temp3193=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp3193->hd=( void*)
_temp3184; _temp3193->tl= prev_fields; _temp3193;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp3180); if( ! Cyc_Tcutil_bits_only(
_temp3180)){({ void(* _temp3194)( struct Cyc_Position_Segment* loc, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp3198= loc; struct _tagged_arr _temp3199= _tag_arr("%s has a type that is possibly incompatible with other members of union",
sizeof( unsigned char), 72u); struct Cyc_Stdio_String_pa_struct _temp3200;
_temp3200.tag= Cyc_Stdio_String_pa; _temp3200.f1=( struct _tagged_arr)*
_temp3184;{ void* _temp3197=( void*)& _temp3200; void* _temp3195[ 1u]={
_temp3197}; struct _tagged_arr _temp3196={( void*) _temp3195,( void*) _temp3195,(
void*)( _temp3195 + 1u)}; _temp3194( _temp3198, _temp3199, _temp3196);}});} if(
_temp3178 != 0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct
Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp3178))->v)){({ void(*
_temp3201)( struct Cyc_Position_Segment* loc, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp3205= loc;
struct _tagged_arr _temp3206= _tag_arr("bitfield %s does not have constant width",
sizeof( unsigned char), 41u); struct Cyc_Stdio_String_pa_struct _temp3207;
_temp3207.tag= Cyc_Stdio_String_pa; _temp3207.f1=( struct _tagged_arr)*
_temp3184;{ void* _temp3204=( void*)& _temp3207; void* _temp3202[ 1u]={
_temp3204}; struct _tagged_arr _temp3203={( void*) _temp3202,( void*) _temp3202,(
void*)( _temp3202 + 1u)}; _temp3201( _temp3205, _temp3206, _temp3203);}});}
else{ w= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*)
_check_null( _temp3178))->v);}{ void* _temp3208= Cyc_Tcutil_compress( _temp3180);
void* _temp3214; _LL3210: if(( unsigned int) _temp3208 > 4u?*(( int*) _temp3208)
== Cyc_Absyn_IntType: 0){ _LL3215: _temp3214=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3208)->f2; goto _LL3211;} else{ goto _LL3212;} _LL3212: goto _LL3213;
_LL3211:{ void* _temp3216= _temp3214; _LL3218: if( _temp3216 ==( void*) Cyc_Absyn_B1){
goto _LL3219;} else{ goto _LL3220;} _LL3220: if( _temp3216 ==( void*) Cyc_Absyn_B2){
goto _LL3221;} else{ goto _LL3222;} _LL3222: if( _temp3216 ==( void*) Cyc_Absyn_B4){
goto _LL3223;} else{ goto _LL3224;} _LL3224: if( _temp3216 ==( void*) Cyc_Absyn_B8){
goto _LL3225;} else{ goto _LL3217;} _LL3219: if( w > 8){({ void(* _temp3226)(
struct Cyc_Position_Segment* loc, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp3229= loc; struct _tagged_arr
_temp3230= _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u);
void* _temp3227[ 0u]={}; struct _tagged_arr _temp3228={( void*) _temp3227,( void*)
_temp3227,( void*)( _temp3227 + 0u)}; _temp3226( _temp3229, _temp3230, _temp3228);});}
goto _LL3217; _LL3221: if( w > 16){({ void(* _temp3231)( struct Cyc_Position_Segment*
loc, struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp3234= loc; struct _tagged_arr _temp3235= _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u); void* _temp3232[ 0u]={}; struct _tagged_arr
_temp3233={( void*) _temp3232,( void*) _temp3232,( void*)( _temp3232 + 0u)};
_temp3231( _temp3234, _temp3235, _temp3233);});} goto _LL3217; _LL3223: if( w >
32){({ void(* _temp3236)( struct Cyc_Position_Segment* loc, struct _tagged_arr
fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp3239= loc; struct _tagged_arr _temp3240= _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u); void* _temp3237[ 0u]={}; struct _tagged_arr
_temp3238={( void*) _temp3237,( void*) _temp3237,( void*)( _temp3237 + 0u)};
_temp3236( _temp3239, _temp3240, _temp3238);});} goto _LL3217; _LL3225: if( w >
64){({ void(* _temp3241)( struct Cyc_Position_Segment* loc, struct _tagged_arr
fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp3244= loc; struct _tagged_arr _temp3245= _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u); void* _temp3242[ 0u]={}; struct _tagged_arr
_temp3243={( void*) _temp3242,( void*) _temp3242,( void*)( _temp3242 + 0u)};
_temp3241( _temp3244, _temp3245, _temp3243);});} goto _LL3217; _LL3217:;} goto
_LL3209; _LL3213:({ void(* _temp3246)( struct Cyc_Position_Segment* loc, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp3251= loc; struct _tagged_arr _temp3252= _tag_arr("bitfield %s must have integral type but has type %s",
sizeof( unsigned char), 52u); struct Cyc_Stdio_String_pa_struct _temp3254;
_temp3254.tag= Cyc_Stdio_String_pa; _temp3254.f1=( struct _tagged_arr)*
_temp3184;{ void* _temp3249=( void*)& _temp3254; struct Cyc_Stdio_String_pa_struct
_temp3253; _temp3253.tag= Cyc_Stdio_String_pa; _temp3253.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( _temp3180);{ void* _temp3250=( void*)& _temp3253; void*
_temp3247[ 2u]={ _temp3249, _temp3250}; struct _tagged_arr _temp3248={( void*)
_temp3247,( void*) _temp3247,( void*)( _temp3247 + 2u)}; _temp3246( _temp3251,
_temp3252, _temp3248);}}}); goto _LL3209; _LL3209:;}} for( 0; _temp3176 != 0;
_temp3176=(( struct Cyc_List_List*) _check_null( _temp3176))->tl){ void*
_temp3255=( void*)(( struct Cyc_List_List*) _check_null( _temp3176))->hd;
_LL3257: if( _temp3255 ==( void*) Cyc_Absyn_Packed_att){ goto _LL3258;} else{
goto _LL3259;} _LL3259: if(( unsigned int) _temp3255 > 16u?*(( int*) _temp3255)
== Cyc_Absyn_Aligned_att: 0){ goto _LL3260;} else{ goto _LL3261;} _LL3261: goto
_LL3262; _LL3258: continue; _LL3260: continue; _LL3262:({ void(* _temp3263)(
struct Cyc_Position_Segment* loc, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp3268= loc; struct _tagged_arr
_temp3269= _tag_arr("bad attribute %s on struct member %s", sizeof(
unsigned char), 37u); struct Cyc_Stdio_String_pa_struct _temp3271; _temp3271.tag=
Cyc_Stdio_String_pa; _temp3271.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp3176))->hd);{ void* _temp3266=(
void*)& _temp3271; struct Cyc_Stdio_String_pa_struct _temp3270; _temp3270.tag=
Cyc_Stdio_String_pa; _temp3270.f1=( struct _tagged_arr)* _temp3184;{ void*
_temp3267=( void*)& _temp3270; void* _temp3264[ 2u]={ _temp3266, _temp3267};
struct _tagged_arr _temp3265={( void*) _temp3264,( void*) _temp3264,( void*)(
_temp3264 + 2u)}; _temp3263( _temp3268, _temp3269, _temp3265);}}}); goto _LL3256;
_LL3256:;}}}; _pop_region( uprev_rgn);} goto _LL2469; _LL2501: if( _temp2589 ==
0){ if(* _temp2586 == 0){({ void(* _temp3272)( struct Cyc_Position_Segment* loc,
struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp3275= loc; struct _tagged_arr _temp3276= _tag_arr("bad anonymous struct",
sizeof( unsigned char), 21u); void* _temp3273[ 0u]={}; struct _tagged_arr
_temp3274={( void*) _temp3273,( void*) _temp3273,( void*)( _temp3273 + 0u)};
_temp3272( _temp3275, _temp3276, _temp3274);}); return cvtenv;}{ struct Cyc_Absyn_Structdecl*
sd=*(( struct Cyc_Absyn_Structdecl**) _check_null(* _temp2586));;}} if(*
_temp2586 == 0){ struct _handler_cons _temp3277; _push_handler(& _temp3277);{
int _temp3279= 0; if( setjmp( _temp3277.handler)){ _temp3279= 1;} if( !
_temp3279){* _temp2586=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc,( struct _tuple1*) _check_null( _temp2589));; _pop_handler();} else{
void* _temp3278=( void*) _exn_thrown; void* _temp3281= _temp3278; _LL3283: if(
_temp3281 == Cyc_Dict_Absent){ goto _LL3284;} else{ goto _LL3285;} _LL3285: goto
_LL3286; _LL3284:({ void(* _temp3287)( struct Cyc_Position_Segment* loc, struct
_tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp3291= loc; struct _tagged_arr _temp3292= _tag_arr("unbound type struct %s",
sizeof( unsigned char), 23u); struct Cyc_Stdio_String_pa_struct _temp3293;
_temp3293.tag= Cyc_Stdio_String_pa; _temp3293.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp2589));{ void* _temp3290=( void*)& _temp3293;
void* _temp3288[ 1u]={ _temp3290}; struct _tagged_arr _temp3289={( void*)
_temp3288,( void*) _temp3288,( void*)( _temp3288 + 1u)}; _temp3287( _temp3291,
_temp3292, _temp3289);}}); return cvtenv; _LL3286:( void) _throw( _temp3281);
_LL3282:;}}}{ struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**)
_check_null(* _temp2586)); if( sd->name != 0){*(( struct _tuple1*) _check_null(
_temp2589))=(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)[
_check_known_subscript_notnull( 1u, 0)];}{ struct Cyc_List_List* tvs= sd->tvs;
int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=
Cyc_List_length( _temp2587); if( lvs != largs){({ void(* _temp3294)( struct Cyc_Position_Segment*
loc, struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp3300= loc; struct _tagged_arr _temp3301= _tag_arr("struct %s expects %d type arguments but was given %d",
sizeof( unsigned char), 53u); struct Cyc_Stdio_String_pa_struct _temp3304;
_temp3304.tag= Cyc_Stdio_String_pa; _temp3304.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp2589));{ void* _temp3297=( void*)& _temp3304;
struct Cyc_Stdio_Int_pa_struct _temp3303; _temp3303.tag= Cyc_Stdio_Int_pa;
_temp3303.f1=( int)(( unsigned int) lvs);{ void* _temp3298=( void*)& _temp3303;
struct Cyc_Stdio_Int_pa_struct _temp3302; _temp3302.tag= Cyc_Stdio_Int_pa;
_temp3302.f1=( int)(( unsigned int) largs);{ void* _temp3299=( void*)& _temp3302;
void* _temp3295[ 3u]={ _temp3297, _temp3298, _temp3299}; struct _tagged_arr
_temp3296={( void*) _temp3295,( void*) _temp3295,( void*)( _temp3295 + 3u)};
_temp3294( _temp3300, _temp3301, _temp3296);}}}});} for( 0; _temp2587 != 0;(
_temp2587=(( struct Cyc_List_List*) _check_null( _temp2587))->tl, tvs=(( struct
Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)(( struct Cyc_List_List*)
_check_null( _temp2587))->hd; void* k1= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} goto _LL2469;}} _LL2503: if( _temp2596 == 0){({ void(*
_temp3305)( struct Cyc_Position_Segment* loc, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp3308= loc;
struct _tagged_arr _temp3309= _tag_arr("missing union name", sizeof(
unsigned char), 19u); void* _temp3306[ 0u]={}; struct _tagged_arr _temp3307={(
void*) _temp3306,( void*) _temp3306,( void*)( _temp3306 + 0u)}; _temp3305(
_temp3308, _temp3309, _temp3307);}); return cvtenv;} if(* _temp2593 == 0){
struct _handler_cons _temp3310; _push_handler(& _temp3310);{ int _temp3312= 0;
if( setjmp( _temp3310.handler)){ _temp3312= 1;} if( ! _temp3312){* _temp2593=(
struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl( te, loc,( struct
_tuple1*) _check_null( _temp2596));; _pop_handler();} else{ void* _temp3311=(
void*) _exn_thrown; void* _temp3314= _temp3311; _LL3316: if( _temp3314 == Cyc_Dict_Absent){
goto _LL3317;} else{ goto _LL3318;} _LL3318: goto _LL3319; _LL3317:({ void(*
_temp3320)( struct Cyc_Position_Segment* loc, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp3324= loc;
struct _tagged_arr _temp3325= _tag_arr("unbound type union %s", sizeof(
unsigned char), 22u); struct Cyc_Stdio_String_pa_struct _temp3326; _temp3326.tag=
Cyc_Stdio_String_pa; _temp3326.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp2596));{ void* _temp3323=( void*)& _temp3326;
void* _temp3321[ 1u]={ _temp3323}; struct _tagged_arr _temp3322={( void*)
_temp3321,( void*) _temp3321,( void*)( _temp3321 + 1u)}; _temp3320( _temp3324,
_temp3325, _temp3322);}}); return cvtenv; _LL3319:( void) _throw( _temp3314);
_LL3315:;}}}{ struct Cyc_Absyn_Uniondecl* ud=*(( struct Cyc_Absyn_Uniondecl**)
_check_null(* _temp2593)); if( ud->name != 0){*(( struct _tuple1*) _check_null(
_temp2596))=(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( ud->name))->v)[
_check_known_subscript_notnull( 1u, 0)];}{ struct Cyc_List_List* tvs= ud->tvs;
int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=
Cyc_List_length( _temp2594); if( lvs != largs){({ void(* _temp3327)( struct Cyc_Position_Segment*
loc, struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp3333= loc; struct _tagged_arr _temp3334= _tag_arr("union %s expects %d type arguments but was given %d",
sizeof( unsigned char), 52u); struct Cyc_Stdio_String_pa_struct _temp3337;
_temp3337.tag= Cyc_Stdio_String_pa; _temp3337.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp2596));{ void* _temp3330=( void*)& _temp3337;
struct Cyc_Stdio_Int_pa_struct _temp3336; _temp3336.tag= Cyc_Stdio_Int_pa;
_temp3336.f1=( int)(( unsigned int) lvs);{ void* _temp3331=( void*)& _temp3336;
struct Cyc_Stdio_Int_pa_struct _temp3335; _temp3335.tag= Cyc_Stdio_Int_pa;
_temp3335.f1=( int)(( unsigned int) largs);{ void* _temp3332=( void*)& _temp3335;
void* _temp3328[ 3u]={ _temp3330, _temp3331, _temp3332}; struct _tagged_arr
_temp3329={( void*) _temp3328,( void*) _temp3328,( void*)( _temp3328 + 3u)};
_temp3327( _temp3333, _temp3334, _temp3329);}}}});} for( 0; _temp2594 != 0;(
_temp2594=(( struct Cyc_List_List*) _check_null( _temp2594))->tl, tvs=(( struct
Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)(( struct Cyc_List_List*)
_check_null( _temp2594))->hd; void* k1= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} goto _LL2469;}} _LL2505: { struct Cyc_Absyn_Typedefdecl*
td;{ struct _handler_cons _temp3338; _push_handler(& _temp3338);{ int _temp3340=
0; if( setjmp( _temp3338.handler)){ _temp3340= 1;} if( ! _temp3340){ td= Cyc_Tcenv_lookup_typedefdecl(
te, loc, _temp2603);; _pop_handler();} else{ void* _temp3339=( void*)
_exn_thrown; void* _temp3342= _temp3339; _LL3344: if( _temp3342 == Cyc_Dict_Absent){
goto _LL3345;} else{ goto _LL3346;} _LL3346: goto _LL3347; _LL3345:({ void(*
_temp3348)( struct Cyc_Position_Segment* loc, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp3352= loc;
struct _tagged_arr _temp3353= _tag_arr("unbound typedef name %s", sizeof(
unsigned char), 24u); struct Cyc_Stdio_String_pa_struct _temp3354; _temp3354.tag=
Cyc_Stdio_String_pa; _temp3354.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2603);{ void* _temp3351=( void*)& _temp3354; void* _temp3349[ 1u]={
_temp3351}; struct _tagged_arr _temp3350={( void*) _temp3349,( void*) _temp3349,(
void*)( _temp3349 + 1u)}; _temp3348( _temp3352, _temp3353, _temp3350);}});
return cvtenv; _LL3347:( void) _throw( _temp3342); _LL3343:;}}} _temp2603[
_check_known_subscript_notnull( 1u, 0)]=( td->name)[
_check_known_subscript_notnull( 1u, 0)];{ struct Cyc_List_List* tvs= td->tvs;
struct Cyc_List_List* ts= _temp2601; struct Cyc_List_List* inst= 0; for( 0; ts
!= 0? tvs != 0: 0;( ts=(( struct Cyc_List_List*) _check_null( ts))->tl, tvs=((
struct Cyc_List_List*) _check_null( tvs))->tl)){ void* k= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k,( void*)(( struct Cyc_List_List*)
_check_null( ts))->hd); inst=({ struct Cyc_List_List* _temp3355=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3355->hd=( void*)({ struct
_tuple5* _temp3356=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp3356->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
tvs))->hd; _temp3356->f2=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd;
_temp3356;}); _temp3355->tl= inst; _temp3355;});} if( tvs != 0? 1: ts != 0){({
void(* _temp3357)( struct Cyc_Position_Segment* loc, struct _tagged_arr fmt,
struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp3361=
loc; struct _tagged_arr _temp3362= _tag_arr("typedef %s expects a different number of arguments",
sizeof( unsigned char), 51u); struct Cyc_Stdio_String_pa_struct _temp3363;
_temp3363.tag= Cyc_Stdio_String_pa; _temp3363.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2603);{ void* _temp3360=( void*)& _temp3363; void* _temp3358[ 1u]={
_temp3360}; struct _tagged_arr _temp3359={( void*) _temp3358,( void*) _temp3358,(
void*)( _temp3358 + 1u)}; _temp3357( _temp3361, _temp3362, _temp3359);}});
return cvtenv;}{ void* new_typ= Cyc_Tcutil_substitute( inst,( void*) td->defn);*
_temp2600=({ struct Cyc_Core_Opt* _temp3364=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp3364->v=( void*) new_typ; _temp3364;}); goto
_LL2469;}}} _LL2507: goto _LL2469; _LL2509: _temp2607= _temp2605; goto _LL2511;
_LL2511: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2607); goto _LL2469; _LL2513: for( 0; _temp2609 != 0; _temp2609=(( struct
Cyc_List_List*) _check_null( _temp2609))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_List_List*)
_check_null( _temp2609))->hd);} goto _LL2469; _LL2469:;} if( ! Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind( t), expected_kind)){({ void(* _temp3365)( struct Cyc_Position_Segment*
loc, struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp3371= loc; struct _tagged_arr _temp3372= _tag_arr("type %s has kind %s but as used here needs kind %s",
sizeof( unsigned char), 51u); struct Cyc_Stdio_String_pa_struct _temp3375;
_temp3375.tag= Cyc_Stdio_String_pa; _temp3375.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp3368=( void*)& _temp3375; struct Cyc_Stdio_String_pa_struct
_temp3374; _temp3374.tag= Cyc_Stdio_String_pa; _temp3374.f1=( struct _tagged_arr)
Cyc_Absynpp_kind2string( Cyc_Tcutil_typ_kind( t));{ void* _temp3369=( void*)&
_temp3374; struct Cyc_Stdio_String_pa_struct _temp3373; _temp3373.tag= Cyc_Stdio_String_pa;
_temp3373.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( expected_kind);{
void* _temp3370=( void*)& _temp3373; void* _temp3366[ 3u]={ _temp3368, _temp3369,
_temp3370}; struct _tagged_arr _temp3367={( void*) _temp3366,( void*) _temp3366,(
void*)( _temp3366 + 3u)}; _temp3365( _temp3371, _temp3372, _temp3367);}}}});}
return cvtenv;} struct Cyc_List_List* Cyc_Tcutil_check_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* kind_env, void*
expected_kind, void* t){ struct Cyc_Tcutil_CVTEnv _temp3376= Cyc_Tcutil_i_check_valid_type(
loc, te,({ struct Cyc_Tcutil_CVTEnv _temp3391; _temp3391.kind_env= kind_env;
_temp3391.free_vars= 0; _temp3391.free_evars= 0; _temp3391;}), expected_kind, t);{
struct Cyc_List_List* vs= _temp3376.free_vars; for( 0; vs != 0; vs=(( struct Cyc_List_List*)
_check_null( vs))->tl){ Cyc_Tcutil_fast_add_free_tvar( kind_env,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( vs))->hd);}}{ struct Cyc_List_List* evs=
_temp3376.free_evars; for( 0; evs != 0; evs=(( struct Cyc_List_List*)
_check_null( evs))->tl){ void* _temp3377= Cyc_Tcutil_compress(( void*)(( struct
Cyc_List_List*) _check_null( evs))->hd); struct Cyc_Core_Opt* _temp3383; struct
Cyc_Core_Opt** _temp3385; _LL3379: if(( unsigned int) _temp3377 > 4u?*(( int*)
_temp3377) == Cyc_Absyn_Evar: 0){ _LL3384: _temp3383=(( struct Cyc_Absyn_Evar_struct*)
_temp3377)->f4; _temp3385=&(( struct Cyc_Absyn_Evar_struct*) _temp3377)->f4;
goto _LL3380;} else{ goto _LL3381;} _LL3381: goto _LL3382; _LL3380: if(*
_temp3385 == 0){* _temp3385=({ struct Cyc_Core_Opt* _temp3386=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3386->v=( void*) kind_env;
_temp3386;});} else{ struct Cyc_List_List* _temp3387=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(* _temp3385))->v; struct Cyc_List_List*
_temp3388= 0; for( 0; _temp3387 != 0; _temp3387=(( struct Cyc_List_List*)
_check_null( _temp3387))->tl){ if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp3387))->hd)){ _temp3388=({ struct Cyc_List_List* _temp3389=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3389->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp3387))->hd);
_temp3389->tl= _temp3388; _temp3389;});}}* _temp3385=({ struct Cyc_Core_Opt*
_temp3390=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3390->v=( void*) _temp3388; _temp3390;});} goto _LL3378; _LL3382: goto
_LL3378; _LL3378:;}} return _temp3376.free_vars;} void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, void* t){ struct
Cyc_List_List* _temp3392= Cyc_Tcutil_check_valid_type( loc, te, 0,( void*) Cyc_Absyn_MemKind,
t);{ struct Cyc_List_List* x= _temp3392; for( 0; x != 0; x=(( struct Cyc_List_List*)
_check_null( x))->tl){ struct Cyc_Absyn_Conref* c= Cyc_Absyn_compress_conref(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( x))->hd)->kind);
void* _temp3393=( void*) c->v; void* _temp3401; _LL3395: if( _temp3393 ==( void*)
Cyc_Absyn_No_constr){ goto _LL3396;} else{ goto _LL3397;} _LL3397: if((
unsigned int) _temp3393 > 1u?*(( int*) _temp3393) == Cyc_Absyn_Eq_constr: 0){
_LL3402: _temp3401=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3393)->f1;
if( _temp3401 ==( void*) Cyc_Absyn_MemKind){ goto _LL3398;} else{ goto _LL3399;}}
else{ goto _LL3399;} _LL3399: goto _LL3400; _LL3396: goto _LL3398; _LL3398:(
void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3403=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3403[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3404; _temp3404.tag= Cyc_Absyn_Eq_constr;
_temp3404.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3404;}); _temp3403;})));
goto _LL3394; _LL3400: goto _LL3394; _LL3394:;}} if( _temp3392 != 0){ void*
_temp3405= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp3411; struct
Cyc_List_List* _temp3413; struct Cyc_List_List* _temp3415; struct Cyc_Absyn_VarargInfo*
_temp3417; int _temp3419; struct Cyc_List_List* _temp3421; void* _temp3423;
struct Cyc_Core_Opt* _temp3425; struct Cyc_List_List* _temp3427; struct Cyc_List_List**
_temp3429; _LL3407: if(( unsigned int) _temp3405 > 4u?*(( int*) _temp3405) ==
Cyc_Absyn_FnType: 0){ _LL3412: _temp3411=(( struct Cyc_Absyn_FnType_struct*)
_temp3405)->f1; _LL3428: _temp3427= _temp3411.tvars; _temp3429=&((( struct Cyc_Absyn_FnType_struct*)
_temp3405)->f1).tvars; goto _LL3426; _LL3426: _temp3425= _temp3411.effect; goto
_LL3424; _LL3424: _temp3423=( void*) _temp3411.ret_typ; goto _LL3422; _LL3422:
_temp3421= _temp3411.args; goto _LL3420; _LL3420: _temp3419= _temp3411.c_varargs;
goto _LL3418; _LL3418: _temp3417= _temp3411.cyc_varargs; goto _LL3416; _LL3416:
_temp3415= _temp3411.rgn_po; goto _LL3414; _LL3414: _temp3413= _temp3411.attributes;
goto _LL3408;} else{ goto _LL3409;} _LL3409: goto _LL3410; _LL3408: if(*
_temp3429 == 0){* _temp3429= _temp3392;} else{ if( _temp3392 != 0){ struct
_tagged_arr _temp3430=*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp3392))->hd)->name; if(*(( const unsigned char*)
_check_unknown_subscript( _temp3430, sizeof( unsigned char), 0)) =='?'){({ void(*
_temp3431)( struct Cyc_Position_Segment* loc, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp3434= loc;
struct _tagged_arr _temp3435= _tag_arr("You failed to abstract over the generated type variables. \nPlease remove the bound type variables or else make the effects of the type explicit\nand then abstract the free type variables properly. (Sorry.)",
sizeof( unsigned char), 205u); void* _temp3432[ 0u]={}; struct _tagged_arr
_temp3433={( void*) _temp3432,( void*) _temp3432,( void*)( _temp3432 + 0u)};
_temp3431( _temp3434, _temp3435, _temp3433);});} else{({ void(* _temp3436)(
struct Cyc_Position_Segment* loc, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp3440= loc; struct _tagged_arr
_temp3441= _tag_arr("unbound type variable %s", sizeof( unsigned char), 25u);
struct Cyc_Stdio_String_pa_struct _temp3442; _temp3442.tag= Cyc_Stdio_String_pa;
_temp3442.f1=( struct _tagged_arr) _temp3430;{ void* _temp3439=( void*)&
_temp3442; void* _temp3437[ 1u]={ _temp3439}; struct _tagged_arr _temp3438={(
void*) _temp3437,( void*) _temp3437,( void*)( _temp3437 + 1u)}; _temp3436(
_temp3440, _temp3441, _temp3438);}});}}} goto _LL3406; _LL3410:({ void(*
_temp3443)( struct Cyc_Position_Segment* loc, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp3447= loc;
struct _tagged_arr _temp3448= _tag_arr("unbound type variable %s", sizeof(
unsigned char), 25u); struct Cyc_Stdio_String_pa_struct _temp3449; _temp3449.tag=
Cyc_Stdio_String_pa; _temp3449.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp3392))->hd)->name;{ void* _temp3446=(
void*)& _temp3449; void* _temp3444[ 1u]={ _temp3446}; struct _tagged_arr
_temp3445={( void*) _temp3444,( void*) _temp3444,( void*)( _temp3444 + 1u)};
_temp3443( _temp3447, _temp3448, _temp3445);}}); return; _LL3406:;}} void Cyc_Tcutil_add_tvar_identity(
struct Cyc_Absyn_Tvar* tv){ if( tv->identity == 0){ tv->identity= Cyc_Tcutil_new_tvar_id();}}
void Cyc_Tcutil_add_tvar_identities( struct Cyc_List_List* tvs){(( void(*)( void(*
f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Tcutil_add_tvar_identity,
tvs);} void Cyc_Tcutil_check_type( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* bound_tvars, void* expected_kind, void* t){ struct Cyc_List_List*
_temp3450= Cyc_Tcutil_check_valid_type( loc, te, bound_tvars, expected_kind, t);
struct Cyc_List_List* _temp3451= Cyc_Tcutil_remove_bound_tvars( _temp3450,
bound_tvars); struct Cyc_List_List* fs= _temp3451; for( 0; fs != 0; fs=(( struct
Cyc_List_List*) _check_null( fs))->tl){ struct _tagged_arr* _temp3452=(( struct
Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( fs))->hd)->name;({ void(*
_temp3453)( struct Cyc_Position_Segment* loc, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp3458= loc;
struct _tagged_arr _temp3459= _tag_arr("unbound type variable %s in type %s",
sizeof( unsigned char), 36u); struct Cyc_Stdio_String_pa_struct _temp3461;
_temp3461.tag= Cyc_Stdio_String_pa; _temp3461.f1=( struct _tagged_arr)*
_temp3452;{ void* _temp3456=( void*)& _temp3461; struct Cyc_Stdio_String_pa_struct
_temp3460; _temp3460.tag= Cyc_Stdio_String_pa; _temp3460.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp3457=( void*)& _temp3460; void*
_temp3454[ 2u]={ _temp3456, _temp3457}; struct _tagged_arr _temp3455={( void*)
_temp3454,( void*) _temp3454,( void*)( _temp3454 + 2u)}; _temp3453( _temp3458,
_temp3459, _temp3455);}}});}} static void Cyc_Tcutil_check_unique_unsorted( int(*
cmp)( void*, void*), struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc,
struct _tagged_arr(* a2string)( void*), struct _tagged_arr msg){ for( 0; vs != 0;
vs=(( struct Cyc_List_List*) _check_null( vs))->tl){ struct Cyc_List_List* vs2=((
struct Cyc_List_List*) _check_null( vs))->tl; for( 0; vs2 != 0; vs2=(( struct
Cyc_List_List*) _check_null( vs2))->tl){ if( cmp(( void*)(( struct Cyc_List_List*)
_check_null( vs))->hd,( void*)(( struct Cyc_List_List*) _check_null( vs2))->hd)
== 0){({ void(* _temp3462)( struct Cyc_Position_Segment* loc, struct _tagged_arr
fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp3467= loc; struct _tagged_arr _temp3468= _tag_arr("%s: %s", sizeof(
unsigned char), 7u); struct Cyc_Stdio_String_pa_struct _temp3470; _temp3470.tag=
Cyc_Stdio_String_pa; _temp3470.f1=( struct _tagged_arr) msg;{ void* _temp3465=(
void*)& _temp3470; struct Cyc_Stdio_String_pa_struct _temp3469; _temp3469.tag=
Cyc_Stdio_String_pa; _temp3469.f1=( struct _tagged_arr) a2string(( void*)((
struct Cyc_List_List*) _check_null( vs))->hd);{ void* _temp3466=( void*)&
_temp3469; void* _temp3463[ 2u]={ _temp3465, _temp3466}; struct _tagged_arr
_temp3464={( void*) _temp3463,( void*) _temp3463,( void*)( _temp3463 + 2u)};
_temp3462( _temp3467, _temp3468, _temp3464);}}});}}}} static struct _tagged_arr
Cyc_Tcutil_strptr2string( struct _tagged_arr* s){ return* s;} void Cyc_Tcutil_check_unique_vars(
struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc, struct _tagged_arr
msg){(( void(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*), struct
Cyc_List_List* vs, struct Cyc_Position_Segment* loc, struct _tagged_arr(*
a2string)( struct _tagged_arr*), struct _tagged_arr msg)) Cyc_Tcutil_check_unique_unsorted)(
Cyc_String_zstrptrcmp, vs, loc, Cyc_Tcutil_strptr2string, msg);} void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* tvs){(( void(*)( int(*
cmp)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* vs,
struct Cyc_Position_Segment* loc, struct _tagged_arr(* a2string)( struct Cyc_Absyn_Tvar*),
struct _tagged_arr msg)) Cyc_Tcutil_check_unique_unsorted)( Cyc_Absyn_tvar_cmp,
tvs, loc, Cyc_Tcutil_tvar2string, _tag_arr("duplicate type variable", sizeof(
unsigned char), 24u));} struct _tuple17{ struct Cyc_Absyn_Structfield* f1; int
f2; } ; struct _tuple18{ struct Cyc_List_List* f1; void* f2; } ; struct _tuple19{
struct Cyc_Absyn_Structfield* f1; void* f2; } ; struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* sdfields){ struct Cyc_List_List* fields= 0;{ struct
Cyc_List_List* sd_fields= sdfields; for( 0; sd_fields != 0; sd_fields=(( struct
Cyc_List_List*) _check_null( sd_fields))->tl){ if( Cyc_String_strcmp(*(( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( sd_fields))->hd)->name,
_tag_arr("", sizeof( unsigned char), 1u)) != 0){ fields=({ struct Cyc_List_List*
_temp3471=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3471->hd=( void*)({ struct _tuple17* _temp3472=( struct _tuple17*)
GC_malloc( sizeof( struct _tuple17)); _temp3472->f1=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( sd_fields))->hd; _temp3472->f2= 0; _temp3472;});
_temp3471->tl= fields; _temp3471;});}}} fields=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields);{ struct Cyc_List_List* ans=
0; for( 0; des != 0; des=(( struct Cyc_List_List*) _check_null( des))->tl){
struct _tuple18 _temp3475; void* _temp3476; struct Cyc_List_List* _temp3478;
struct _tuple18* _temp3473=( struct _tuple18*)(( struct Cyc_List_List*)
_check_null( des))->hd; _temp3475=* _temp3473; _LL3479: _temp3478= _temp3475.f1;
goto _LL3477; _LL3477: _temp3476= _temp3475.f2; goto _LL3474; _LL3474: if(
_temp3478 == 0){ struct Cyc_List_List* _temp3480= fields; for( 0; _temp3480 != 0;
_temp3480=(( struct Cyc_List_List*) _check_null( _temp3480))->tl){ if( !(*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp3480))->hd)).f2){(*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp3480))->hd)).f2= 1;(*((
struct _tuple18*)(( struct Cyc_List_List*) _check_null( des))->hd)).f1=({ struct
Cyc_List_List* _temp3481=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3481->hd=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp3482=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp3482[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp3483; _temp3483.tag= Cyc_Absyn_FieldName;
_temp3483.f1=((*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3480))->hd)).f1)->name; _temp3483;}); _temp3482;})); _temp3481->tl= 0;
_temp3481;}); ans=({ struct Cyc_List_List* _temp3484=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp3484->hd=( void*)({
struct _tuple19* _temp3485=( struct _tuple19*) _region_malloc( rgn, sizeof(
struct _tuple19)); _temp3485->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3480))->hd)).f1; _temp3485->f2= _temp3476; _temp3485;});
_temp3484->tl= ans; _temp3484;}); break;}} if( _temp3480 == 0){({ void(*
_temp3486)( struct Cyc_Position_Segment* loc, struct _tagged_arr fmt, struct
_tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp3489= loc;
struct _tagged_arr _temp3490= _tag_arr("too many arguments to struct", sizeof(
unsigned char), 29u); void* _temp3487[ 0u]={}; struct _tagged_arr _temp3488={(
void*) _temp3487,( void*) _temp3487,( void*)( _temp3487 + 0u)}; _temp3486(
_temp3489, _temp3490, _temp3488);});}} else{ if((( struct Cyc_List_List*)
_check_null( _temp3478))->tl != 0){({ void(* _temp3491)( struct Cyc_Position_Segment*
loc, struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp3494= loc; struct _tagged_arr _temp3495= _tag_arr("multiple designators are not supported",
sizeof( unsigned char), 39u); void* _temp3492[ 0u]={}; struct _tagged_arr
_temp3493={( void*) _temp3492,( void*) _temp3492,( void*)( _temp3492 + 0u)};
_temp3491( _temp3494, _temp3495, _temp3493);});} else{ void* _temp3496=( void*)((
struct Cyc_List_List*) _check_null( _temp3478))->hd; struct _tagged_arr*
_temp3502; _LL3498: if(*(( int*) _temp3496) == Cyc_Absyn_ArrayElement){ goto
_LL3499;} else{ goto _LL3500;} _LL3500: if(*(( int*) _temp3496) == Cyc_Absyn_FieldName){
_LL3503: _temp3502=(( struct Cyc_Absyn_FieldName_struct*) _temp3496)->f1; goto
_LL3501;} else{ goto _LL3497;} _LL3499:({ void(* _temp3504)( struct Cyc_Position_Segment*
loc, struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp3507= loc; struct _tagged_arr _temp3508= _tag_arr("array designator used in argument to struct",
sizeof( unsigned char), 44u); void* _temp3505[ 0u]={}; struct _tagged_arr
_temp3506={( void*) _temp3505,( void*) _temp3505,( void*)( _temp3505 + 0u)};
_temp3504( _temp3507, _temp3508, _temp3506);}); goto _LL3497; _LL3501: { struct
Cyc_List_List* _temp3509= fields; for( 0; _temp3509 != 0; _temp3509=(( struct
Cyc_List_List*) _check_null( _temp3509))->tl){ if( Cyc_String_zstrptrcmp(
_temp3502,((*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3509))->hd)).f1)->name) == 0){ if((*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3509))->hd)).f2){({ void(* _temp3510)( struct Cyc_Position_Segment*
loc, struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp3514= loc; struct _tagged_arr _temp3515= _tag_arr("field %s has already been used as an argument",
sizeof( unsigned char), 46u); struct Cyc_Stdio_String_pa_struct _temp3516;
_temp3516.tag= Cyc_Stdio_String_pa; _temp3516.f1=( struct _tagged_arr)*
_temp3502;{ void* _temp3513=( void*)& _temp3516; void* _temp3511[ 1u]={
_temp3513}; struct _tagged_arr _temp3512={( void*) _temp3511,( void*) _temp3511,(
void*)( _temp3511 + 1u)}; _temp3510( _temp3514, _temp3515, _temp3512);}});}(*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp3509))->hd)).f2= 1;
ans=({ struct Cyc_List_List* _temp3517=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp3517->hd=( void*)({ struct _tuple19*
_temp3518=( struct _tuple19*) _region_malloc( rgn, sizeof( struct _tuple19));
_temp3518->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3509))->hd)).f1; _temp3518->f2= _temp3476; _temp3518;}); _temp3517->tl= ans;
_temp3517;}); break;}} if( _temp3509 == 0){({ void(* _temp3519)( struct Cyc_Position_Segment*
loc, struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp3523= loc; struct _tagged_arr _temp3524= _tag_arr("bad field designator %s",
sizeof( unsigned char), 24u); struct Cyc_Stdio_String_pa_struct _temp3525;
_temp3525.tag= Cyc_Stdio_String_pa; _temp3525.f1=( struct _tagged_arr)*
_temp3502;{ void* _temp3522=( void*)& _temp3525; void* _temp3520[ 1u]={
_temp3522}; struct _tagged_arr _temp3521={( void*) _temp3520,( void*) _temp3520,(
void*)( _temp3520 + 1u)}; _temp3519( _temp3523, _temp3524, _temp3521);}});} goto
_LL3497;} _LL3497:;}}} for( 0; fields != 0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ if( !(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( fields))->hd)).f2){({ void(* _temp3526)( struct Cyc_Position_Segment*
loc, struct _tagged_arr fmt, struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment*
_temp3529= loc; struct _tagged_arr _temp3530= _tag_arr("too few arguments to struct",
sizeof( unsigned char), 28u); void* _temp3527[ 0u]={}; struct _tagged_arr
_temp3528={( void*) _temp3527,( void*) _temp3527,( void*)( _temp3527 + 0u)};
_temp3526( _temp3529, _temp3530, _temp3528);}); break;}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( ans);}} int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void* t, void** elt_typ_dest){ void* _temp3531= Cyc_Tcutil_compress( t); struct
Cyc_Absyn_PtrInfo _temp3537; struct Cyc_Absyn_Conref* _temp3539; void* _temp3541;
_LL3533: if(( unsigned int) _temp3531 > 4u?*(( int*) _temp3531) == Cyc_Absyn_PointerType:
0){ _LL3538: _temp3537=(( struct Cyc_Absyn_PointerType_struct*) _temp3531)->f1;
_LL3542: _temp3541=( void*) _temp3537.elt_typ; goto _LL3540; _LL3540: _temp3539=
_temp3537.bounds; goto _LL3534;} else{ goto _LL3535;} _LL3535: goto _LL3536;
_LL3534: { struct Cyc_Absyn_Conref* _temp3543= Cyc_Absyn_compress_conref(
_temp3539); void* _temp3544=( void*)( Cyc_Absyn_compress_conref( _temp3543))->v;
void* _temp3552; _LL3546: if(( unsigned int) _temp3544 > 1u?*(( int*) _temp3544)
== Cyc_Absyn_Eq_constr: 0){ _LL3553: _temp3552=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3544)->f1; if( _temp3552 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3547;}
else{ goto _LL3548;}} else{ goto _LL3548;} _LL3548: if( _temp3544 ==( void*) Cyc_Absyn_No_constr){
goto _LL3549;} else{ goto _LL3550;} _LL3550: goto _LL3551; _LL3547:*
elt_typ_dest= _temp3541; return 1; _LL3549:( void*)( _temp3543->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3554=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3554[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3555; _temp3555.tag= Cyc_Absyn_Eq_constr;
_temp3555.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp3555;}); _temp3554;})));*
elt_typ_dest= _temp3541; return 1; _LL3551: return 0; _LL3545:;} _LL3536: return
0; _LL3532:;} int Cyc_Tcutil_is_tagged_pointer_typ( void* t){ void* ignore=(
void*) Cyc_Absyn_VoidType; return Cyc_Tcutil_is_tagged_pointer_typ_elt( t,&
ignore);} struct _tuple7 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ struct _tuple7 bogus_ans=({ struct _tuple7 _temp3742;
_temp3742.f1= 0; _temp3742.f2=( void*) Cyc_Absyn_HeapRgn; _temp3742;}); void*
_temp3556=( void*) e->r; void* _temp3570; struct _tuple1* _temp3572; struct
_tagged_arr* _temp3574; struct Cyc_Absyn_Exp* _temp3576; struct _tagged_arr*
_temp3578; struct Cyc_Absyn_Exp* _temp3580; struct Cyc_Absyn_Exp* _temp3582;
struct Cyc_Absyn_Exp* _temp3584; struct Cyc_Absyn_Exp* _temp3586; _LL3558: if(*((
int*) _temp3556) == Cyc_Absyn_Var_e){ _LL3573: _temp3572=(( struct Cyc_Absyn_Var_e_struct*)
_temp3556)->f1; goto _LL3571; _LL3571: _temp3570=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3556)->f2; goto _LL3559;} else{ goto _LL3560;} _LL3560: if(*(( int*)
_temp3556) == Cyc_Absyn_StructMember_e){ _LL3577: _temp3576=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3556)->f1; goto _LL3575; _LL3575: _temp3574=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3556)->f2; goto _LL3561;} else{ goto _LL3562;} _LL3562: if(*(( int*)
_temp3556) == Cyc_Absyn_StructArrow_e){ _LL3581: _temp3580=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3556)->f1; goto _LL3579; _LL3579: _temp3578=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3556)->f2; goto _LL3563;} else{ goto _LL3564;} _LL3564: if(*(( int*)
_temp3556) == Cyc_Absyn_Deref_e){ _LL3583: _temp3582=(( struct Cyc_Absyn_Deref_e_struct*)
_temp3556)->f1; goto _LL3565;} else{ goto _LL3566;} _LL3566: if(*(( int*)
_temp3556) == Cyc_Absyn_Subscript_e){ _LL3587: _temp3586=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3556)->f1; goto _LL3585; _LL3585: _temp3584=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3556)->f2; goto _LL3567;} else{ goto _LL3568;} _LL3568: goto _LL3569;
_LL3559: { void* _temp3588= _temp3570; struct Cyc_Absyn_Vardecl* _temp3602;
struct Cyc_Absyn_Vardecl* _temp3604; struct Cyc_Absyn_Vardecl* _temp3606; struct
Cyc_Absyn_Vardecl* _temp3608; _LL3590: if( _temp3588 ==( void*) Cyc_Absyn_Unresolved_b){
goto _LL3591;} else{ goto _LL3592;} _LL3592: if(( unsigned int) _temp3588 > 1u?*((
int*) _temp3588) == Cyc_Absyn_Funname_b: 0){ goto _LL3593;} else{ goto _LL3594;}
_LL3594: if(( unsigned int) _temp3588 > 1u?*(( int*) _temp3588) == Cyc_Absyn_Global_b:
0){ _LL3603: _temp3602=(( struct Cyc_Absyn_Global_b_struct*) _temp3588)->f1;
goto _LL3595;} else{ goto _LL3596;} _LL3596: if(( unsigned int) _temp3588 > 1u?*((
int*) _temp3588) == Cyc_Absyn_Local_b: 0){ _LL3605: _temp3604=(( struct Cyc_Absyn_Local_b_struct*)
_temp3588)->f1; goto _LL3597;} else{ goto _LL3598;} _LL3598: if(( unsigned int)
_temp3588 > 1u?*(( int*) _temp3588) == Cyc_Absyn_Pat_b: 0){ _LL3607: _temp3606=((
struct Cyc_Absyn_Pat_b_struct*) _temp3588)->f1; goto _LL3599;} else{ goto
_LL3600;} _LL3600: if(( unsigned int) _temp3588 > 1u?*(( int*) _temp3588) == Cyc_Absyn_Param_b:
0){ _LL3609: _temp3608=(( struct Cyc_Absyn_Param_b_struct*) _temp3588)->f1; goto
_LL3601;} else{ goto _LL3589;} _LL3591: return bogus_ans; _LL3593: return({
struct _tuple7 _temp3610; _temp3610.f1= 1; _temp3610.f2=( void*) Cyc_Absyn_HeapRgn;
_temp3610;}); _LL3595: { void* _temp3611= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v); _LL3613: if(( unsigned int) _temp3611
> 4u?*(( int*) _temp3611) == Cyc_Absyn_ArrayType: 0){ goto _LL3614;} else{ goto
_LL3615;} _LL3615: goto _LL3616; _LL3614: return({ struct _tuple7 _temp3617;
_temp3617.f1= 1; _temp3617.f2=( void*) Cyc_Absyn_HeapRgn; _temp3617;}); _LL3616:
return({ struct _tuple7 _temp3618; _temp3618.f1=( _temp3602->tq).q_const;
_temp3618.f2=( void*) Cyc_Absyn_HeapRgn; _temp3618;}); _LL3612:;} _LL3597: {
void* _temp3619= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); _LL3621: if(( unsigned int) _temp3619 > 4u?*(( int*)
_temp3619) == Cyc_Absyn_ArrayType: 0){ goto _LL3622;} else{ goto _LL3623;}
_LL3623: goto _LL3624; _LL3622: return({ struct _tuple7 _temp3625; _temp3625.f1=
1; _temp3625.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3604->rgn))->v;
_temp3625;}); _LL3624: return({ struct _tuple7 _temp3626; _temp3626.f1=(
_temp3604->tq).q_const; _temp3626.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3604->rgn))->v; _temp3626;}); _LL3620:;} _LL3599: _temp3608=
_temp3606; goto _LL3601; _LL3601: return({ struct _tuple7 _temp3627; _temp3627.f1=(
_temp3608->tq).q_const; _temp3627.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3608->rgn))->v; _temp3627;}); _LL3589:;} _LL3561: { void*
_temp3628= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp3576->topt))->v); struct Cyc_List_List* _temp3640; struct Cyc_List_List*
_temp3642; struct Cyc_Absyn_Structdecl** _temp3644; struct Cyc_Absyn_Structdecl*
_temp3646; struct Cyc_Absyn_Uniondecl** _temp3647; struct Cyc_Absyn_Uniondecl*
_temp3649; _LL3630: if(( unsigned int) _temp3628 > 4u?*(( int*) _temp3628) ==
Cyc_Absyn_AnonStructType: 0){ _LL3641: _temp3640=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3628)->f1; goto _LL3631;} else{ goto _LL3632;} _LL3632: if(( unsigned int)
_temp3628 > 4u?*(( int*) _temp3628) == Cyc_Absyn_AnonUnionType: 0){ _LL3643:
_temp3642=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3628)->f1; goto
_LL3633;} else{ goto _LL3634;} _LL3634: if(( unsigned int) _temp3628 > 4u?*((
int*) _temp3628) == Cyc_Absyn_StructType: 0){ _LL3645: _temp3644=(( struct Cyc_Absyn_StructType_struct*)
_temp3628)->f3; if( _temp3644 == 0){ goto _LL3636;} else{ _temp3646=* _temp3644;
goto _LL3635;}} else{ goto _LL3636;} _LL3636: if(( unsigned int) _temp3628 > 4u?*((
int*) _temp3628) == Cyc_Absyn_UnionType: 0){ _LL3648: _temp3647=(( struct Cyc_Absyn_UnionType_struct*)
_temp3628)->f3; if( _temp3647 == 0){ goto _LL3638;} else{ _temp3649=* _temp3647;
goto _LL3637;}} else{ goto _LL3638;} _LL3638: goto _LL3639; _LL3631: _temp3642=
_temp3640; goto _LL3633; _LL3633: { struct Cyc_Absyn_Structfield* _temp3650= Cyc_Absyn_lookup_field(
_temp3642, _temp3574); if( _temp3650 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3650))->width != 0: 0){ return({ struct _tuple7 _temp3651;
_temp3651.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3650))->tq).q_const;
_temp3651.f2=( Cyc_Tcutil_addressof_props( te, _temp3576)).f2; _temp3651;});}
return bogus_ans;} _LL3635: { struct Cyc_Absyn_Structfield* _temp3652= Cyc_Absyn_lookup_struct_field(
_temp3646, _temp3574); if( _temp3652 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3652))->width != 0: 0){ return({ struct _tuple7 _temp3653;
_temp3653.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3652))->tq).q_const;
_temp3653.f2=( Cyc_Tcutil_addressof_props( te, _temp3576)).f2; _temp3653;});}
return bogus_ans;} _LL3637: { struct Cyc_Absyn_Structfield* _temp3654= Cyc_Absyn_lookup_union_field(
_temp3649, _temp3574); if( _temp3654 != 0){ return({ struct _tuple7 _temp3655;
_temp3655.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3654))->tq).q_const;
_temp3655.f2=( Cyc_Tcutil_addressof_props( te, _temp3576)).f2; _temp3655;});}
goto _LL3639;} _LL3639: return bogus_ans; _LL3629:;} _LL3563: { void* _temp3656=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3580->topt))->v);
struct Cyc_Absyn_PtrInfo _temp3662; void* _temp3664; void* _temp3666; _LL3658:
if(( unsigned int) _temp3656 > 4u?*(( int*) _temp3656) == Cyc_Absyn_PointerType:
0){ _LL3663: _temp3662=(( struct Cyc_Absyn_PointerType_struct*) _temp3656)->f1;
_LL3667: _temp3666=( void*) _temp3662.elt_typ; goto _LL3665; _LL3665: _temp3664=(
void*) _temp3662.rgn_typ; goto _LL3659;} else{ goto _LL3660;} _LL3660: goto
_LL3661; _LL3659: { void* _temp3668= Cyc_Tcutil_compress( _temp3666); struct Cyc_List_List*
_temp3680; struct Cyc_List_List* _temp3682; struct Cyc_Absyn_Structdecl**
_temp3684; struct Cyc_Absyn_Structdecl* _temp3686; struct Cyc_Absyn_Uniondecl**
_temp3687; struct Cyc_Absyn_Uniondecl* _temp3689; _LL3670: if(( unsigned int)
_temp3668 > 4u?*(( int*) _temp3668) == Cyc_Absyn_AnonStructType: 0){ _LL3681:
_temp3680=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3668)->f1; goto
_LL3671;} else{ goto _LL3672;} _LL3672: if(( unsigned int) _temp3668 > 4u?*((
int*) _temp3668) == Cyc_Absyn_AnonUnionType: 0){ _LL3683: _temp3682=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3668)->f1; goto _LL3673;} else{ goto
_LL3674;} _LL3674: if(( unsigned int) _temp3668 > 4u?*(( int*) _temp3668) == Cyc_Absyn_StructType:
0){ _LL3685: _temp3684=(( struct Cyc_Absyn_StructType_struct*) _temp3668)->f3;
if( _temp3684 == 0){ goto _LL3676;} else{ _temp3686=* _temp3684; goto _LL3675;}}
else{ goto _LL3676;} _LL3676: if(( unsigned int) _temp3668 > 4u?*(( int*)
_temp3668) == Cyc_Absyn_UnionType: 0){ _LL3688: _temp3687=(( struct Cyc_Absyn_UnionType_struct*)
_temp3668)->f3; if( _temp3687 == 0){ goto _LL3678;} else{ _temp3689=* _temp3687;
goto _LL3677;}} else{ goto _LL3678;} _LL3678: goto _LL3679; _LL3671: _temp3682=
_temp3680; goto _LL3673; _LL3673: { struct Cyc_Absyn_Structfield* _temp3690= Cyc_Absyn_lookup_field(
_temp3682, _temp3578); if( _temp3690 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3690))->width != 0: 0){ return({ struct _tuple7 _temp3691;
_temp3691.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3690))->tq).q_const;
_temp3691.f2= _temp3664; _temp3691;});} return bogus_ans;} _LL3675: { struct Cyc_Absyn_Structfield*
_temp3692= Cyc_Absyn_lookup_struct_field( _temp3686, _temp3578); if( _temp3692
!= 0?(( struct Cyc_Absyn_Structfield*) _check_null( _temp3692))->width != 0: 0){
return({ struct _tuple7 _temp3693; _temp3693.f1=((( struct Cyc_Absyn_Structfield*)
_check_null( _temp3692))->tq).q_const; _temp3693.f2= _temp3664; _temp3693;});}
return bogus_ans;} _LL3677: { struct Cyc_Absyn_Structfield* _temp3694= Cyc_Absyn_lookup_union_field(
_temp3689, _temp3578); if( _temp3694 != 0){ return({ struct _tuple7 _temp3695;
_temp3695.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3694))->tq).q_const;
_temp3695.f2= _temp3664; _temp3695;});} return bogus_ans;} _LL3679: return
bogus_ans; _LL3669:;} _LL3661: return bogus_ans; _LL3657:;} _LL3565: { void*
_temp3696= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp3582->topt))->v); struct Cyc_Absyn_PtrInfo _temp3702; struct Cyc_Absyn_Tqual
_temp3704; void* _temp3706; _LL3698: if(( unsigned int) _temp3696 > 4u?*(( int*)
_temp3696) == Cyc_Absyn_PointerType: 0){ _LL3703: _temp3702=(( struct Cyc_Absyn_PointerType_struct*)
_temp3696)->f1; _LL3707: _temp3706=( void*) _temp3702.rgn_typ; goto _LL3705;
_LL3705: _temp3704= _temp3702.tq; goto _LL3699;} else{ goto _LL3700;} _LL3700:
goto _LL3701; _LL3699: return({ struct _tuple7 _temp3708; _temp3708.f1=
_temp3704.q_const; _temp3708.f2= _temp3706; _temp3708;}); _LL3701: return
bogus_ans; _LL3697:;} _LL3567: { void* t= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp3586->topt))->v); void* _temp3709= t; struct
Cyc_Absyn_Tqual _temp3719; struct Cyc_List_List* _temp3721; struct Cyc_Absyn_PtrInfo
_temp3723; struct Cyc_Absyn_Conref* _temp3725; struct Cyc_Absyn_Tqual _temp3727;
void* _temp3729; void* _temp3731; _LL3711: if(( unsigned int) _temp3709 > 4u?*((
int*) _temp3709) == Cyc_Absyn_ArrayType: 0){ _LL3720: _temp3719=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3709)->f2; goto _LL3712;} else{ goto _LL3713;} _LL3713: if(( unsigned int)
_temp3709 > 4u?*(( int*) _temp3709) == Cyc_Absyn_TupleType: 0){ _LL3722:
_temp3721=(( struct Cyc_Absyn_TupleType_struct*) _temp3709)->f1; goto _LL3714;}
else{ goto _LL3715;} _LL3715: if(( unsigned int) _temp3709 > 4u?*(( int*)
_temp3709) == Cyc_Absyn_PointerType: 0){ _LL3724: _temp3723=(( struct Cyc_Absyn_PointerType_struct*)
_temp3709)->f1; _LL3732: _temp3731=( void*) _temp3723.elt_typ; goto _LL3730;
_LL3730: _temp3729=( void*) _temp3723.rgn_typ; goto _LL3728; _LL3728: _temp3727=
_temp3723.tq; goto _LL3726; _LL3726: _temp3725= _temp3723.bounds; goto _LL3716;}
else{ goto _LL3717;} _LL3717: goto _LL3718; _LL3712: return({ struct _tuple7
_temp3733; _temp3733.f1= _temp3719.q_const; _temp3733.f2=( Cyc_Tcutil_addressof_props(
te, _temp3586)).f2; _temp3733;}); _LL3714: { struct _tuple4* _temp3734= Cyc_Absyn_lookup_tuple_field(
_temp3721,( int) Cyc_Evexp_eval_const_uint_exp( _temp3584)); if( _temp3734 != 0){
return({ struct _tuple7 _temp3735; _temp3735.f1=((*(( struct _tuple4*)
_check_null( _temp3734))).f1).q_const; _temp3735.f2=( Cyc_Tcutil_addressof_props(
te, _temp3586)).f2; _temp3735;});} return bogus_ans;} _LL3716: return({ struct
_tuple7 _temp3736; _temp3736.f1= _temp3727.q_const; _temp3736.f2= _temp3729;
_temp3736;}); _LL3718: return bogus_ans; _LL3710:;} _LL3569:({ void(* _temp3737)(
struct Cyc_Position_Segment* loc, struct _tagged_arr fmt, struct _tagged_arr ap)=
Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp3740= e->loc; struct
_tagged_arr _temp3741= _tag_arr("unary & applied to non-lvalue", sizeof(
unsigned char), 30u); void* _temp3738[ 0u]={}; struct _tagged_arr _temp3739={(
void*) _temp3738,( void*) _temp3738,( void*)( _temp3738 + 0u)}; _temp3737(
_temp3740, _temp3741, _temp3739);}); return bogus_ans; _LL3557:;} void* Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv* te, void* e_typ, struct Cyc_Absyn_Exp* e){ void*
_temp3743= Cyc_Tcutil_compress( e_typ); struct Cyc_Absyn_Tqual _temp3749; void*
_temp3751; _LL3745: if(( unsigned int) _temp3743 > 4u?*(( int*) _temp3743) ==
Cyc_Absyn_ArrayType: 0){ _LL3752: _temp3751=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3743)->f1; goto _LL3750; _LL3750: _temp3749=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3743)->f2; goto _LL3746;} else{ goto _LL3747;} _LL3747: goto _LL3748;
_LL3746: { void* _temp3755; struct _tuple7 _temp3753= Cyc_Tcutil_addressof_props(
te, e); _LL3756: _temp3755= _temp3753.f2; goto _LL3754; _LL3754: return Cyc_Absyn_atb_typ(
_temp3751, _temp3755, _temp3749,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp3757=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3757[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3758; _temp3758.tag= Cyc_Absyn_Upper_b;
_temp3758.f1= e; _temp3758;}); _temp3757;}));} _LL3748: return e_typ; _LL3744:;}
void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment* loc, unsigned int i,
struct Cyc_Absyn_Conref* b){ b= Cyc_Absyn_compress_conref( b);{ void* _temp3759=(
void*) b->v; void* _temp3767; void* _temp3769; struct Cyc_Absyn_Exp* _temp3771;
_LL3761: if(( unsigned int) _temp3759 > 1u?*(( int*) _temp3759) == Cyc_Absyn_Eq_constr:
0){ _LL3768: _temp3767=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3759)->f1;
if( _temp3767 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3762;} else{ goto _LL3763;}}
else{ goto _LL3763;} _LL3763: if(( unsigned int) _temp3759 > 1u?*(( int*)
_temp3759) == Cyc_Absyn_Eq_constr: 0){ _LL3770: _temp3769=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3759)->f1; if(( unsigned int) _temp3769 > 1u?*(( int*) _temp3769) == Cyc_Absyn_Upper_b:
0){ _LL3772: _temp3771=(( struct Cyc_Absyn_Upper_b_struct*) _temp3769)->f1; goto
_LL3764;} else{ goto _LL3765;}} else{ goto _LL3765;} _LL3765: goto _LL3766;
_LL3762: return; _LL3764: if( Cyc_Evexp_eval_const_uint_exp( _temp3771) <= i){({
void(* _temp3773)( struct Cyc_Position_Segment* loc, struct _tagged_arr fmt,
struct _tagged_arr ap)= Cyc_Tcutil_terr; struct Cyc_Position_Segment* _temp3776=
loc; struct _tagged_arr _temp3777= _tag_arr("dereference is out of bounds",
sizeof( unsigned char), 29u); void* _temp3774[ 0u]={}; struct _tagged_arr
_temp3775={( void*) _temp3774,( void*) _temp3774,( void*)( _temp3774 + 0u)};
_temp3773( _temp3776, _temp3777, _temp3775);});} return; _LL3766:( void*)( b->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3778=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3778[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3779; _temp3779.tag= Cyc_Absyn_Eq_constr;
_temp3779.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3780=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3780[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3781; _temp3781.tag= Cyc_Absyn_Upper_b;
_temp3781.f1= Cyc_Absyn_uint_exp( i + 1, 0); _temp3781;}); _temp3780;}));
_temp3779;}); _temp3778;}))); return; _LL3760:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_bits_only( void* t){ void* _temp3782= Cyc_Tcutil_compress(
t); void* _temp3808; struct Cyc_List_List* _temp3810; struct Cyc_Absyn_Structdecl**
_temp3812; struct Cyc_List_List* _temp3814; struct Cyc_Absyn_Uniondecl**
_temp3816; struct Cyc_List_List* _temp3818; struct Cyc_List_List* _temp3820;
struct Cyc_List_List* _temp3822; _LL3784: if( _temp3782 ==( void*) Cyc_Absyn_VoidType){
goto _LL3785;} else{ goto _LL3786;} _LL3786: if(( unsigned int) _temp3782 > 4u?*((
int*) _temp3782) == Cyc_Absyn_IntType: 0){ goto _LL3787;} else{ goto _LL3788;}
_LL3788: if( _temp3782 ==( void*) Cyc_Absyn_FloatType){ goto _LL3789;} else{
goto _LL3790;} _LL3790: if( _temp3782 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL3791;} else{ goto _LL3792;} _LL3792: if(( unsigned int) _temp3782 > 4u?*((
int*) _temp3782) == Cyc_Absyn_EnumType: 0){ goto _LL3793;} else{ goto _LL3794;}
_LL3794: if(( unsigned int) _temp3782 > 4u?*(( int*) _temp3782) == Cyc_Absyn_ArrayType:
0){ _LL3809: _temp3808=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3782)->f1;
goto _LL3795;} else{ goto _LL3796;} _LL3796: if(( unsigned int) _temp3782 > 4u?*((
int*) _temp3782) == Cyc_Absyn_TupleType: 0){ _LL3811: _temp3810=(( struct Cyc_Absyn_TupleType_struct*)
_temp3782)->f1; goto _LL3797;} else{ goto _LL3798;} _LL3798: if(( unsigned int)
_temp3782 > 4u?*(( int*) _temp3782) == Cyc_Absyn_StructType: 0){ _LL3815:
_temp3814=(( struct Cyc_Absyn_StructType_struct*) _temp3782)->f2; goto _LL3813;
_LL3813: _temp3812=(( struct Cyc_Absyn_StructType_struct*) _temp3782)->f3; goto
_LL3799;} else{ goto _LL3800;} _LL3800: if(( unsigned int) _temp3782 > 4u?*((
int*) _temp3782) == Cyc_Absyn_UnionType: 0){ _LL3819: _temp3818=(( struct Cyc_Absyn_UnionType_struct*)
_temp3782)->f2; goto _LL3817; _LL3817: _temp3816=(( struct Cyc_Absyn_UnionType_struct*)
_temp3782)->f3; goto _LL3801;} else{ goto _LL3802;} _LL3802: if(( unsigned int)
_temp3782 > 4u?*(( int*) _temp3782) == Cyc_Absyn_AnonStructType: 0){ _LL3821:
_temp3820=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3782)->f1; goto
_LL3803;} else{ goto _LL3804;} _LL3804: if(( unsigned int) _temp3782 > 4u?*((
int*) _temp3782) == Cyc_Absyn_AnonUnionType: 0){ _LL3823: _temp3822=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3782)->f1; goto _LL3805;} else{ goto
_LL3806;} _LL3806: goto _LL3807; _LL3785: return 1; _LL3787: return 1; _LL3789:
return 1; _LL3791: return 1; _LL3793: return 0; _LL3795: return Cyc_Tcutil_bits_only(
_temp3808); _LL3797: for( 0; _temp3810 != 0; _temp3810=(( struct Cyc_List_List*)
_check_null( _temp3810))->tl){ if( ! Cyc_Tcutil_bits_only((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp3810))->hd)).f2)){ return 0;}} return 1;
_LL3799: if( _temp3812 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* _temp3824=*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp3812)); if( _temp3824->fields
== 0){ return 0;}{ struct _RegionHandle _temp3825= _new_region(); struct
_RegionHandle* rgn=& _temp3825; _push_region( rgn);{ struct Cyc_List_List*
_temp3826=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp3824->tvs, _temp3814);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp3824->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute(
rgn, _temp3826,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3827= 0; _npop_handler( 0u); return
_temp3827;}}}{ int _temp3828= 1; _npop_handler( 0u); return _temp3828;}};
_pop_region( rgn);}} _LL3801: if( _temp3816 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
_temp3829=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp3816)); if(
_temp3829->fields == 0){ return 0;}{ struct _RegionHandle _temp3830= _new_region();
struct _RegionHandle* rgn=& _temp3830; _push_region( rgn);{ struct Cyc_List_List*
_temp3831=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp3829->tvs, _temp3818);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp3829->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute(
rgn, _temp3831,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3832= 0; _npop_handler( 0u); return
_temp3832;}}}{ int _temp3833= 1; _npop_handler( 0u); return _temp3833;}};
_pop_region( rgn);}} _LL3803: _temp3822= _temp3820; goto _LL3805; _LL3805: for(
0; _temp3822 != 0; _temp3822=(( struct Cyc_List_List*) _check_null( _temp3822))->tl){
if( ! Cyc_Tcutil_bits_only(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp3822))->hd)->type)){ return 0;}} return 1;
_LL3807: return 0; _LL3783:;} struct _tuple20{ struct Cyc_List_List* f1; struct
Cyc_Absyn_Exp* f2; } ; static int Cyc_Tcutil_cnst_exp( struct Cyc_Tcenv_Tenv* te,
int var_okay, struct Cyc_Absyn_Exp* e){ void* _temp3834=( void*) e->r; struct
_tuple1* _temp3874; struct Cyc_Absyn_Exp* _temp3876; struct Cyc_Absyn_Exp*
_temp3878; struct Cyc_Absyn_Exp* _temp3880; struct Cyc_Absyn_Exp* _temp3882;
struct Cyc_Absyn_Exp* _temp3884; struct Cyc_Absyn_Exp* _temp3886; struct Cyc_Absyn_Exp*
_temp3888; struct Cyc_Absyn_Exp* _temp3890; void* _temp3892; struct Cyc_Absyn_Exp*
_temp3894; struct Cyc_Absyn_Exp* _temp3896; struct Cyc_Absyn_Exp* _temp3898;
struct Cyc_List_List* _temp3900; struct Cyc_List_List* _temp3902; struct Cyc_List_List*
_temp3904; struct Cyc_List_List* _temp3906; void* _temp3908; struct Cyc_List_List*
_temp3910; struct Cyc_List_List* _temp3912; _LL3836: if(*(( int*) _temp3834) ==
Cyc_Absyn_Const_e){ goto _LL3837;} else{ goto _LL3838;} _LL3838: if(*(( int*)
_temp3834) == Cyc_Absyn_Sizeoftyp_e){ goto _LL3839;} else{ goto _LL3840;}
_LL3840: if(*(( int*) _temp3834) == Cyc_Absyn_Sizeofexp_e){ goto _LL3841;} else{
goto _LL3842;} _LL3842: if(*(( int*) _temp3834) == Cyc_Absyn_Enum_e){ goto
_LL3843;} else{ goto _LL3844;} _LL3844: if(*(( int*) _temp3834) == Cyc_Absyn_Var_e){
_LL3875: _temp3874=(( struct Cyc_Absyn_Var_e_struct*) _temp3834)->f1; goto
_LL3845;} else{ goto _LL3846;} _LL3846: if(*(( int*) _temp3834) == Cyc_Absyn_Conditional_e){
_LL3881: _temp3880=(( struct Cyc_Absyn_Conditional_e_struct*) _temp3834)->f1;
goto _LL3879; _LL3879: _temp3878=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3834)->f2; goto _LL3877; _LL3877: _temp3876=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3834)->f3; goto _LL3847;} else{ goto _LL3848;} _LL3848: if(*(( int*)
_temp3834) == Cyc_Absyn_SeqExp_e){ _LL3885: _temp3884=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3834)->f1; goto _LL3883; _LL3883: _temp3882=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3834)->f2; goto _LL3849;} else{ goto _LL3850;} _LL3850: if(*(( int*)
_temp3834) == Cyc_Absyn_NoInstantiate_e){ _LL3887: _temp3886=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp3834)->f1; goto _LL3851;} else{ goto _LL3852;} _LL3852: if(*(( int*)
_temp3834) == Cyc_Absyn_Instantiate_e){ _LL3889: _temp3888=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3834)->f1; goto _LL3853;} else{ goto _LL3854;} _LL3854: if(*(( int*)
_temp3834) == Cyc_Absyn_Cast_e){ _LL3893: _temp3892=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp3834)->f1; goto _LL3891; _LL3891: _temp3890=(( struct Cyc_Absyn_Cast_e_struct*)
_temp3834)->f2; goto _LL3855;} else{ goto _LL3856;} _LL3856: if(*(( int*)
_temp3834) == Cyc_Absyn_Address_e){ _LL3895: _temp3894=(( struct Cyc_Absyn_Address_e_struct*)
_temp3834)->f1; goto _LL3857;} else{ goto _LL3858;} _LL3858: if(*(( int*)
_temp3834) == Cyc_Absyn_Comprehension_e){ _LL3899: _temp3898=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3834)->f2; goto _LL3897; _LL3897: _temp3896=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3834)->f3; goto _LL3859;} else{ goto _LL3860;} _LL3860: if(*(( int*)
_temp3834) == Cyc_Absyn_Array_e){ _LL3901: _temp3900=(( struct Cyc_Absyn_Array_e_struct*)
_temp3834)->f1; goto _LL3861;} else{ goto _LL3862;} _LL3862: if(*(( int*)
_temp3834) == Cyc_Absyn_AnonStruct_e){ _LL3903: _temp3902=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3834)->f2; goto _LL3863;} else{ goto _LL3864;} _LL3864: if(*(( int*)
_temp3834) == Cyc_Absyn_Struct_e){ _LL3905: _temp3904=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3834)->f3; goto _LL3865;} else{ goto _LL3866;} _LL3866: if(*(( int*)
_temp3834) == Cyc_Absyn_Primop_e){ _LL3909: _temp3908=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp3834)->f1; goto _LL3907; _LL3907: _temp3906=(( struct Cyc_Absyn_Primop_e_struct*)
_temp3834)->f2; goto _LL3867;} else{ goto _LL3868;} _LL3868: if(*(( int*)
_temp3834) == Cyc_Absyn_Tuple_e){ _LL3911: _temp3910=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp3834)->f1; goto _LL3869;} else{ goto _LL3870;} _LL3870: if(*(( int*)
_temp3834) == Cyc_Absyn_Tunion_e){ _LL3913: _temp3912=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3834)->f3; goto _LL3871;} else{ goto _LL3872;} _LL3872: goto _LL3873;
_LL3837: return 1; _LL3839: return 1; _LL3841: return 1; _LL3843: return 1;
_LL3845: { struct _handler_cons _temp3914; _push_handler(& _temp3914);{ int
_temp3916= 0; if( setjmp( _temp3914.handler)){ _temp3916= 1;} if( ! _temp3916){{
void* _temp3917= Cyc_Tcenv_lookup_ordinary( te, e->loc, _temp3874); void*
_temp3925; void* _temp3927; struct Cyc_Absyn_Vardecl* _temp3929; _LL3919: if(*((
int*) _temp3917) == Cyc_Tcenv_VarRes){ _LL3926: _temp3925=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3917)->f1; if(( unsigned int) _temp3925 > 1u?*(( int*) _temp3925) == Cyc_Absyn_Funname_b:
0){ goto _LL3920;} else{ goto _LL3921;}} else{ goto _LL3921;} _LL3921: if(*((
int*) _temp3917) == Cyc_Tcenv_VarRes){ _LL3928: _temp3927=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3917)->f1; if(( unsigned int) _temp3927 > 1u?*(( int*) _temp3927) == Cyc_Absyn_Global_b:
0){ _LL3930: _temp3929=(( struct Cyc_Absyn_Global_b_struct*) _temp3927)->f1;
goto _LL3922;} else{ goto _LL3923;}} else{ goto _LL3923;} _LL3923: goto _LL3924;
_LL3920: { int _temp3931= 1; _npop_handler( 0u); return _temp3931;} _LL3922: {
void* _temp3932= Cyc_Tcutil_compress(( void*) _temp3929->type); _LL3934: if((
unsigned int) _temp3932 > 4u?*(( int*) _temp3932) == Cyc_Absyn_ArrayType: 0){
goto _LL3935;} else{ goto _LL3936;} _LL3936: goto _LL3937; _LL3935: { int
_temp3938= 1; _npop_handler( 0u); return _temp3938;} _LL3937: { int _temp3939=
var_okay; _npop_handler( 0u); return _temp3939;} _LL3933:;} _LL3924: { int
_temp3940= var_okay; _npop_handler( 0u); return _temp3940;} _LL3918:;};
_pop_handler();} else{ void* _temp3915=( void*) _exn_thrown; void* _temp3942=
_temp3915; _LL3944: if( _temp3942 == Cyc_Dict_Absent){ goto _LL3945;} else{ goto
_LL3946;} _LL3946: goto _LL3947; _LL3945: return 0; _LL3947:( void) _throw(
_temp3942); _LL3943:;}}} _LL3847: return( Cyc_Tcutil_cnst_exp( te, 0, _temp3880)?
Cyc_Tcutil_cnst_exp( te, 0, _temp3878): 0)? Cyc_Tcutil_cnst_exp( te, 0,
_temp3876): 0; _LL3849: return Cyc_Tcutil_cnst_exp( te, 0, _temp3884)? Cyc_Tcutil_cnst_exp(
te, 0, _temp3882): 0; _LL3851: _temp3888= _temp3886; goto _LL3853; _LL3853:
return Cyc_Tcutil_cnst_exp( te, var_okay, _temp3888); _LL3855: return Cyc_Tcutil_cnst_exp(
te, var_okay, _temp3890); _LL3857: return Cyc_Tcutil_cnst_exp( te, 1, _temp3894);
_LL3859: return Cyc_Tcutil_cnst_exp( te, 0, _temp3898)? Cyc_Tcutil_cnst_exp( te,
0, _temp3896): 0; _LL3861: _temp3902= _temp3900; goto _LL3863; _LL3863:
_temp3904= _temp3902; goto _LL3865; _LL3865: for( 0; _temp3904 != 0; _temp3904=((
struct Cyc_List_List*) _check_null( _temp3904))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,(*(( struct _tuple20*)(( struct Cyc_List_List*) _check_null( _temp3904))->hd)).f2)){
return 0;}} return 1; _LL3867: _temp3910= _temp3906; goto _LL3869; _LL3869:
_temp3912= _temp3910; goto _LL3871; _LL3871: for( 0; _temp3912 != 0; _temp3912=((
struct Cyc_List_List*) _check_null( _temp3912))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp3912))->hd)){
return 0;}} return 1; _LL3873: return 0; _LL3835:;} int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcutil_cnst_exp(
te, 0, e);} static int Cyc_Tcutil_fields_support_default( struct Cyc_List_List*
tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs); int Cyc_Tcutil_supports_default(
void* t){ void* _temp3948= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp3986; struct Cyc_Absyn_Conref* _temp3988; struct Cyc_Absyn_Conref*
_temp3990; void* _temp3992; void* _temp3994; struct Cyc_List_List* _temp3996;
struct Cyc_Absyn_Structdecl** _temp3998; struct Cyc_List_List* _temp4000; struct
Cyc_Absyn_Uniondecl** _temp4002; struct Cyc_List_List* _temp4004; struct Cyc_List_List*
_temp4006; struct Cyc_List_List* _temp4008; _LL3950: if( _temp3948 ==( void*)
Cyc_Absyn_VoidType){ goto _LL3951;} else{ goto _LL3952;} _LL3952: if((
unsigned int) _temp3948 > 4u?*(( int*) _temp3948) == Cyc_Absyn_Evar: 0){ goto
_LL3953;} else{ goto _LL3954;} _LL3954: if(( unsigned int) _temp3948 > 4u?*((
int*) _temp3948) == Cyc_Absyn_VarType: 0){ goto _LL3955;} else{ goto _LL3956;}
_LL3956: if(( unsigned int) _temp3948 > 4u?*(( int*) _temp3948) == Cyc_Absyn_TunionType:
0){ goto _LL3957;} else{ goto _LL3958;} _LL3958: if(( unsigned int) _temp3948 >
4u?*(( int*) _temp3948) == Cyc_Absyn_TunionFieldType: 0){ goto _LL3959;} else{
goto _LL3960;} _LL3960: if(( unsigned int) _temp3948 > 4u?*(( int*) _temp3948)
== Cyc_Absyn_PointerType: 0){ _LL3987: _temp3986=(( struct Cyc_Absyn_PointerType_struct*)
_temp3948)->f1; _LL3993: _temp3992=( void*) _temp3986.elt_typ; goto _LL3991;
_LL3991: _temp3990= _temp3986.nullable; goto _LL3989; _LL3989: _temp3988=
_temp3986.bounds; goto _LL3961;} else{ goto _LL3962;} _LL3962: if(( unsigned int)
_temp3948 > 4u?*(( int*) _temp3948) == Cyc_Absyn_IntType: 0){ goto _LL3963;}
else{ goto _LL3964;} _LL3964: if( _temp3948 ==( void*) Cyc_Absyn_FloatType){
goto _LL3965;} else{ goto _LL3966;} _LL3966: if( _temp3948 ==( void*) Cyc_Absyn_DoubleType){
goto _LL3967;} else{ goto _LL3968;} _LL3968: if(( unsigned int) _temp3948 > 4u?*((
int*) _temp3948) == Cyc_Absyn_ArrayType: 0){ _LL3995: _temp3994=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3948)->f1; goto _LL3969;} else{ goto
_LL3970;} _LL3970: if(( unsigned int) _temp3948 > 4u?*(( int*) _temp3948) == Cyc_Absyn_FnType:
0){ goto _LL3971;} else{ goto _LL3972;} _LL3972: if(( unsigned int) _temp3948 >
4u?*(( int*) _temp3948) == Cyc_Absyn_TupleType: 0){ _LL3997: _temp3996=(( struct
Cyc_Absyn_TupleType_struct*) _temp3948)->f1; goto _LL3973;} else{ goto _LL3974;}
_LL3974: if(( unsigned int) _temp3948 > 4u?*(( int*) _temp3948) == Cyc_Absyn_StructType:
0){ _LL4001: _temp4000=(( struct Cyc_Absyn_StructType_struct*) _temp3948)->f2;
goto _LL3999; _LL3999: _temp3998=(( struct Cyc_Absyn_StructType_struct*)
_temp3948)->f3; goto _LL3975;} else{ goto _LL3976;} _LL3976: if(( unsigned int)
_temp3948 > 4u?*(( int*) _temp3948) == Cyc_Absyn_UnionType: 0){ _LL4005:
_temp4004=(( struct Cyc_Absyn_UnionType_struct*) _temp3948)->f2; goto _LL4003;
_LL4003: _temp4002=(( struct Cyc_Absyn_UnionType_struct*) _temp3948)->f3; goto
_LL3977;} else{ goto _LL3978;} _LL3978: if(( unsigned int) _temp3948 > 4u?*((
int*) _temp3948) == Cyc_Absyn_AnonStructType: 0){ _LL4007: _temp4006=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp3948)->f1; goto _LL3979;} else{ goto
_LL3980;} _LL3980: if(( unsigned int) _temp3948 > 4u?*(( int*) _temp3948) == Cyc_Absyn_AnonUnionType:
0){ _LL4009: _temp4008=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3948)->f1;
goto _LL3981;} else{ goto _LL3982;} _LL3982: if(( unsigned int) _temp3948 > 4u?*((
int*) _temp3948) == Cyc_Absyn_EnumType: 0){ goto _LL3983;} else{ goto _LL3984;}
_LL3984: goto _LL3985; _LL3951: return 1; _LL3953: return 0; _LL3955: return 0;
_LL3957: return 0; _LL3959: return 0; _LL3961: { void* _temp4010=( void*)( Cyc_Absyn_compress_conref(
_temp3988))->v; void* _temp4018; void* _temp4020; _LL4012: if(( unsigned int)
_temp4010 > 1u?*(( int*) _temp4010) == Cyc_Absyn_Eq_constr: 0){ _LL4019:
_temp4018=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp4010)->f1; if(
_temp4018 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL4013;} else{ goto _LL4014;}}
else{ goto _LL4014;} _LL4014: if(( unsigned int) _temp4010 > 1u?*(( int*)
_temp4010) == Cyc_Absyn_Eq_constr: 0){ _LL4021: _temp4020=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp4010)->f1; if(( unsigned int) _temp4020 > 1u?*(( int*) _temp4020) == Cyc_Absyn_Upper_b:
0){ goto _LL4015;} else{ goto _LL4016;}} else{ goto _LL4016;} _LL4016: goto
_LL4017; _LL4013: return 1; _LL4015: { void* _temp4022=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3990))->v; int
_temp4028; _LL4024: if(( unsigned int) _temp4022 > 1u?*(( int*) _temp4022) ==
Cyc_Absyn_Eq_constr: 0){ _LL4029: _temp4028=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp4022)->f1; goto _LL4025;} else{ goto _LL4026;} _LL4026: goto _LL4027;
_LL4025: return _temp4028; _LL4027: return 0; _LL4023:;} _LL4017: return 0;
_LL4011:;} _LL3963: return 1; _LL3965: return 1; _LL3967: return 1; _LL3969:
return Cyc_Tcutil_supports_default( _temp3994); _LL3971: return 0; _LL3973: for(
0; _temp3996 != 0; _temp3996=(( struct Cyc_List_List*) _check_null( _temp3996))->tl){
if( ! Cyc_Tcutil_supports_default((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp3996))->hd)).f2)){ return 0;}} return 1; _LL3975: if(
_temp3998 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp3998)); if( sd->fields == 0){ return 0;} return Cyc_Tcutil_fields_support_default(
sd->tvs, _temp4000,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
sd->fields))->v);} _LL3977: if( _temp4002 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp4002)); if( ud->fields ==
0){ return 0;} return Cyc_Tcutil_fields_support_default( ud->tvs, _temp4004,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ud->fields))->v);}
_LL3979: _temp4008= _temp4006; goto _LL3981; _LL3981: return Cyc_Tcutil_fields_support_default(
0, 0, _temp4008); _LL3983: return 1; _LL3985: return 0; _LL3949:;} static int
Cyc_Tcutil_fields_support_default( struct Cyc_List_List* tvs, struct Cyc_List_List*
ts, struct Cyc_List_List* fs){{ struct _RegionHandle _temp4030= _new_region();
struct _RegionHandle* rgn=& _temp4030; _push_region( rgn);{ struct Cyc_List_List*
_temp4031=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, tvs, ts); for( 0; fs != 0; fs=(( struct Cyc_List_List*) _check_null(
fs))->tl){ void* t= Cyc_Tcutil_rsubstitute( rgn, _temp4031,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( fs))->hd)->type); if( ! Cyc_Tcutil_supports_default(
t)){ int _temp4032= 0; _npop_handler( 0u); return _temp4032;}}}; _pop_region(
rgn);} return 1;}
