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
fmt, ap);({ struct Cyc_Stdio_String_pa_struct _temp1; _temp1.tag= Cyc_Stdio_String_pa;
_temp1.f1=( struct _tagged_arr) msg;{ void* _temp0[ 1u]={& _temp1}; Cyc_Stdio_fprintf(
Cyc_Stdio_stderr, _tag_arr("Error: %s\n", sizeof( unsigned char), 11u), _tag_arr(
_temp0, sizeof( void*), 1u));}}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr);( int) _throw(( void*) Cyc_Tcutil_TypeErr);} static struct
_tagged_arr Cyc_Tcutil_tvar2string( struct Cyc_Absyn_Tvar* tv){ return* tv->name;}
void Cyc_Tcutil_print_tvars( struct Cyc_List_List* tvs){ for( 0; tvs != 0; tvs=((
struct Cyc_List_List*) _check_null( tvs))->tl){({ struct Cyc_Stdio_String_pa_struct
_temp4; _temp4.tag= Cyc_Stdio_String_pa; _temp4.f1=( struct _tagged_arr) Cyc_Absynpp_ckind2string(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);{
struct Cyc_Stdio_String_pa_struct _temp3; _temp3.tag= Cyc_Stdio_String_pa;
_temp3.f1=( struct _tagged_arr) Cyc_Tcutil_tvar2string(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd);{ void* _temp2[ 2u]={& _temp3,&
_temp4}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr, _tag_arr("%s::%s ", sizeof(
unsigned char), 8u), _tag_arr( _temp2, sizeof( void*), 2u));}}});}({ void*
_temp5[ 0u]={}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr, _tag_arr("\n", sizeof(
unsigned char), 2u), _tag_arr( _temp5, sizeof( void*), 0u));}); Cyc_Stdio_fflush((
struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);} static struct Cyc_List_List* Cyc_Tcutil_warning_segs=
0; static struct Cyc_List_List* Cyc_Tcutil_warning_msgs= 0; void Cyc_Tcutil_warn(
struct Cyc_Position_Segment* sg, struct _tagged_arr fmt, struct _tagged_arr ap){
struct _tagged_arr msg=( struct _tagged_arr) Cyc_Stdio_vraprintf( Cyc_Core_heap_region,
fmt, ap); Cyc_Tcutil_warning_segs=({ struct Cyc_List_List* _temp6=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp6->hd=( void*) sg; _temp6->tl=
Cyc_Tcutil_warning_segs; _temp6;}); Cyc_Tcutil_warning_msgs=({ struct Cyc_List_List*
_temp7=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp7->hd=( void*)({ struct _tagged_arr* _temp8=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp8[ 0]= msg; _temp8;}); _temp7->tl=
Cyc_Tcutil_warning_msgs; _temp7;});} void Cyc_Tcutil_flush_warnings(){ if( Cyc_Tcutil_warning_segs
== 0){ return;}({ void* _temp9[ 0u]={}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("***Warnings***\n", sizeof( unsigned char), 16u), _tag_arr( _temp9,
sizeof( void*), 0u));});{ struct Cyc_List_List* _temp10= Cyc_Position_strings_of_segments(
Cyc_Tcutil_warning_segs); Cyc_Tcutil_warning_segs= 0; Cyc_Tcutil_warning_msgs=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_Tcutil_warning_msgs);
while( Cyc_Tcutil_warning_msgs != 0) {({ struct Cyc_Stdio_String_pa_struct
_temp13; _temp13.tag= Cyc_Stdio_String_pa; _temp13.f1=( struct _tagged_arr)*((
struct _tagged_arr*)(( struct Cyc_List_List*) _check_null( Cyc_Tcutil_warning_msgs))->hd);{
struct Cyc_Stdio_String_pa_struct _temp12; _temp12.tag= Cyc_Stdio_String_pa;
_temp12.f1=( struct _tagged_arr)*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( _temp10))->hd);{ void* _temp11[ 2u]={& _temp12,& _temp13}; Cyc_Stdio_fprintf(
Cyc_Stdio_stderr, _tag_arr("%s: %s\n", sizeof( unsigned char), 8u), _tag_arr(
_temp11, sizeof( void*), 2u));}}}); _temp10=(( struct Cyc_List_List*)
_check_null( _temp10))->tl; Cyc_Tcutil_warning_msgs=(( struct Cyc_List_List*)
_check_null( Cyc_Tcutil_warning_msgs))->tl;}({ void* _temp14[ 0u]={}; Cyc_Stdio_fprintf(
Cyc_Stdio_stderr, _tag_arr("**************\n", sizeof( unsigned char), 16u),
_tag_arr( _temp14, sizeof( void*), 0u));}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr);}} struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set= 0; static
int Cyc_Tcutil_fast_tvar_cmp( struct Cyc_Absyn_Tvar* tv1, struct Cyc_Absyn_Tvar*
tv2){ return*(( int*) _check_null( tv1->identity)) -*(( int*) _check_null( tv2->identity));}
void* Cyc_Tcutil_compress( void* t){ void* _temp15= t; struct Cyc_Core_Opt*
_temp27; struct Cyc_Core_Opt* _temp29; struct Cyc_Core_Opt** _temp31; struct Cyc_Core_Opt*
_temp32; struct Cyc_Core_Opt* _temp34; struct Cyc_Core_Opt** _temp36; _LL17: if((
unsigned int) _temp15 > 4u?*(( int*) _temp15) == Cyc_Absyn_Evar: 0){ _LL28:
_temp27=(( struct Cyc_Absyn_Evar_struct*) _temp15)->f2; if( _temp27 == 0){ goto
_LL18;} else{ goto _LL19;}} else{ goto _LL19;} _LL19: if(( unsigned int) _temp15
> 4u?*(( int*) _temp15) == Cyc_Absyn_Evar: 0){ _LL30: _temp29=(( struct Cyc_Absyn_Evar_struct*)
_temp15)->f2; _temp31=&(( struct Cyc_Absyn_Evar_struct*) _temp15)->f2; goto
_LL20;} else{ goto _LL21;} _LL21: if(( unsigned int) _temp15 > 4u?*(( int*)
_temp15) == Cyc_Absyn_TypedefType: 0){ _LL33: _temp32=(( struct Cyc_Absyn_TypedefType_struct*)
_temp15)->f3; if( _temp32 == 0){ goto _LL22;} else{ goto _LL23;}} else{ goto
_LL23;} _LL23: if(( unsigned int) _temp15 > 4u?*(( int*) _temp15) == Cyc_Absyn_TypedefType:
0){ _LL35: _temp34=(( struct Cyc_Absyn_TypedefType_struct*) _temp15)->f3;
_temp36=&(( struct Cyc_Absyn_TypedefType_struct*) _temp15)->f3; goto _LL24;}
else{ goto _LL25;} _LL25: goto _LL26; _LL18: return t; _LL20: { void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null(* _temp31))->v); if( t2 !=( void*)((
struct Cyc_Core_Opt*) _check_null(* _temp31))->v){* _temp31=({ struct Cyc_Core_Opt*
_temp37=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp37->v=( void*) t2; _temp37;});} return t2;} _LL22: return t; _LL24: { void*
t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(* _temp36))->v);
if( t2 !=( void*)(( struct Cyc_Core_Opt*) _check_null(* _temp36))->v){* _temp36=({
struct Cyc_Core_Opt* _temp38=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp38->v=( void*) t2; _temp38;});} return t2;} _LL26: return t;
_LL16:;} void* Cyc_Tcutil_copy_type( void* t); static struct Cyc_List_List* Cyc_Tcutil_copy_types(
struct Cyc_List_List* ts){ return Cyc_List_map( Cyc_Tcutil_copy_type, ts);}
static struct Cyc_Absyn_Conref* Cyc_Tcutil_copy_conref( struct Cyc_Absyn_Conref*
c){ void* _temp39=( void*) c->v; void* _temp47; struct Cyc_Absyn_Conref* _temp49;
_LL41: if( _temp39 ==( void*) Cyc_Absyn_No_constr){ goto _LL42;} else{ goto
_LL43;} _LL43: if(( unsigned int) _temp39 > 1u?*(( int*) _temp39) == Cyc_Absyn_Eq_constr:
0){ _LL48: _temp47=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp39)->f1;
goto _LL44;} else{ goto _LL45;} _LL45: if(( unsigned int) _temp39 > 1u?*(( int*)
_temp39) == Cyc_Absyn_Forward_constr: 0){ _LL50: _temp49=(( struct Cyc_Absyn_Forward_constr_struct*)
_temp39)->f1; goto _LL46;} else{ goto _LL40;} _LL42: return Cyc_Absyn_empty_conref();
_LL44: return Cyc_Absyn_new_conref( _temp47); _LL46: return Cyc_Tcutil_copy_conref(
_temp49); _LL40:;} static struct Cyc_Absyn_Tvar* Cyc_Tcutil_copy_tvar( struct
Cyc_Absyn_Tvar* tv){ return({ struct Cyc_Absyn_Tvar* _temp51=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp51->name= tv->name; _temp51->identity=
0; _temp51->kind= Cyc_Tcutil_copy_conref( tv->kind); _temp51;});} static struct
_tuple2* Cyc_Tcutil_copy_arg( struct _tuple2* arg){ void* _temp54; struct Cyc_Absyn_Tqual
_temp56; struct Cyc_Core_Opt* _temp58; struct _tuple2 _temp52=* arg; _LL59:
_temp58= _temp52.f1; goto _LL57; _LL57: _temp56= _temp52.f2; goto _LL55; _LL55:
_temp54= _temp52.f3; goto _LL53; _LL53: return({ struct _tuple2* _temp60=(
struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp60->f1= _temp58;
_temp60->f2= _temp56; _temp60->f3= Cyc_Tcutil_copy_type( _temp54); _temp60;});}
static struct _tuple4* Cyc_Tcutil_copy_tqt( struct _tuple4* arg){ void* _temp63;
struct Cyc_Absyn_Tqual _temp65; struct _tuple4 _temp61=* arg; _LL66: _temp65=
_temp61.f1; goto _LL64; _LL64: _temp63= _temp61.f2; goto _LL62; _LL62: return({
struct _tuple4* _temp67=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp67->f1= _temp65; _temp67->f2= Cyc_Tcutil_copy_type( _temp63); _temp67;});}
static struct Cyc_Absyn_Structfield* Cyc_Tcutil_copy_field( struct Cyc_Absyn_Structfield*
f){ return({ struct Cyc_Absyn_Structfield* _temp68=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp68->name= f->name;
_temp68->tq= f->tq; _temp68->type=( void*) Cyc_Tcutil_copy_type(( void*) f->type);
_temp68->width= f->width; _temp68->attributes= f->attributes; _temp68;});}
struct _tuple8{ void* f1; void* f2; } ; static struct _tuple8* Cyc_Tcutil_copy_rgncmp(
struct _tuple8* x){ struct _tuple8 _temp71; void* _temp72; void* _temp74; struct
_tuple8* _temp69= x; _temp71=* _temp69; _LL75: _temp74= _temp71.f1; goto _LL73;
_LL73: _temp72= _temp71.f2; goto _LL70; _LL70: return({ struct _tuple8* _temp76=(
struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp76->f1= Cyc_Tcutil_copy_type(
_temp74); _temp76->f2= Cyc_Tcutil_copy_type( _temp72); _temp76;});} void* Cyc_Tcutil_copy_type(
void* t){ void* _temp77= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar*
_temp123; struct Cyc_Absyn_TunionInfo _temp125; void* _temp127; struct Cyc_List_List*
_temp129; void* _temp131; struct Cyc_Absyn_TunionFieldInfo _temp133; struct Cyc_List_List*
_temp135; void* _temp137; struct Cyc_Absyn_PtrInfo _temp139; struct Cyc_Absyn_Conref*
_temp141; struct Cyc_Absyn_Tqual _temp143; struct Cyc_Absyn_Conref* _temp145;
void* _temp147; void* _temp149; void* _temp151; void* _temp153; struct Cyc_Absyn_Exp*
_temp155; struct Cyc_Absyn_Tqual _temp157; void* _temp159; struct Cyc_Absyn_FnInfo
_temp161; struct Cyc_List_List* _temp163; struct Cyc_List_List* _temp165; struct
Cyc_Absyn_VarargInfo* _temp167; int _temp169; struct Cyc_List_List* _temp171;
void* _temp173; struct Cyc_Core_Opt* _temp175; struct Cyc_List_List* _temp177;
struct Cyc_List_List* _temp179; struct Cyc_List_List* _temp181; struct _tuple1*
_temp183; struct Cyc_List_List* _temp185; struct _tuple1* _temp187; struct Cyc_List_List*
_temp189; struct Cyc_List_List* _temp191; struct _tuple1* _temp193; void*
_temp195; struct Cyc_List_List* _temp197; struct _tuple1* _temp199; void*
_temp201; struct Cyc_List_List* _temp203; _LL79: if( _temp77 ==( void*) Cyc_Absyn_VoidType){
goto _LL80;} else{ goto _LL81;} _LL81: if(( unsigned int) _temp77 > 4u?*(( int*)
_temp77) == Cyc_Absyn_Evar: 0){ goto _LL82;} else{ goto _LL83;} _LL83: if((
unsigned int) _temp77 > 4u?*(( int*) _temp77) == Cyc_Absyn_VarType: 0){ _LL124:
_temp123=(( struct Cyc_Absyn_VarType_struct*) _temp77)->f1; goto _LL84;} else{
goto _LL85;} _LL85: if(( unsigned int) _temp77 > 4u?*(( int*) _temp77) == Cyc_Absyn_TunionType:
0){ _LL126: _temp125=(( struct Cyc_Absyn_TunionType_struct*) _temp77)->f1;
_LL132: _temp131=( void*) _temp125.tunion_info; goto _LL130; _LL130: _temp129=
_temp125.targs; goto _LL128; _LL128: _temp127=( void*) _temp125.rgn; goto _LL86;}
else{ goto _LL87;} _LL87: if(( unsigned int) _temp77 > 4u?*(( int*) _temp77) ==
Cyc_Absyn_TunionFieldType: 0){ _LL134: _temp133=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp77)->f1; _LL138: _temp137=( void*) _temp133.field_info; goto _LL136; _LL136:
_temp135= _temp133.targs; goto _LL88;} else{ goto _LL89;} _LL89: if((
unsigned int) _temp77 > 4u?*(( int*) _temp77) == Cyc_Absyn_PointerType: 0){
_LL140: _temp139=(( struct Cyc_Absyn_PointerType_struct*) _temp77)->f1; _LL150:
_temp149=( void*) _temp139.elt_typ; goto _LL148; _LL148: _temp147=( void*)
_temp139.rgn_typ; goto _LL146; _LL146: _temp145= _temp139.nullable; goto _LL144;
_LL144: _temp143= _temp139.tq; goto _LL142; _LL142: _temp141= _temp139.bounds;
goto _LL90;} else{ goto _LL91;} _LL91: if(( unsigned int) _temp77 > 4u?*(( int*)
_temp77) == Cyc_Absyn_IntType: 0){ _LL154: _temp153=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp77)->f1; goto _LL152; _LL152: _temp151=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp77)->f2; goto _LL92;} else{ goto _LL93;} _LL93: if( _temp77 ==( void*) Cyc_Absyn_FloatType){
goto _LL94;} else{ goto _LL95;} _LL95: if( _temp77 ==( void*) Cyc_Absyn_DoubleType){
goto _LL96;} else{ goto _LL97;} _LL97: if(( unsigned int) _temp77 > 4u?*(( int*)
_temp77) == Cyc_Absyn_ArrayType: 0){ _LL160: _temp159=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp77)->f1; goto _LL158; _LL158: _temp157=(( struct Cyc_Absyn_ArrayType_struct*)
_temp77)->f2; goto _LL156; _LL156: _temp155=(( struct Cyc_Absyn_ArrayType_struct*)
_temp77)->f3; goto _LL98;} else{ goto _LL99;} _LL99: if(( unsigned int) _temp77
> 4u?*(( int*) _temp77) == Cyc_Absyn_FnType: 0){ _LL162: _temp161=(( struct Cyc_Absyn_FnType_struct*)
_temp77)->f1; _LL178: _temp177= _temp161.tvars; goto _LL176; _LL176: _temp175=
_temp161.effect; goto _LL174; _LL174: _temp173=( void*) _temp161.ret_typ; goto
_LL172; _LL172: _temp171= _temp161.args; goto _LL170; _LL170: _temp169= _temp161.c_varargs;
goto _LL168; _LL168: _temp167= _temp161.cyc_varargs; goto _LL166; _LL166:
_temp165= _temp161.rgn_po; goto _LL164; _LL164: _temp163= _temp161.attributes;
goto _LL100;} else{ goto _LL101;} _LL101: if(( unsigned int) _temp77 > 4u?*((
int*) _temp77) == Cyc_Absyn_TupleType: 0){ _LL180: _temp179=(( struct Cyc_Absyn_TupleType_struct*)
_temp77)->f1; goto _LL102;} else{ goto _LL103;} _LL103: if(( unsigned int)
_temp77 > 4u?*(( int*) _temp77) == Cyc_Absyn_StructType: 0){ _LL184: _temp183=((
struct Cyc_Absyn_StructType_struct*) _temp77)->f1; goto _LL182; _LL182: _temp181=((
struct Cyc_Absyn_StructType_struct*) _temp77)->f2; goto _LL104;} else{ goto
_LL105;} _LL105: if(( unsigned int) _temp77 > 4u?*(( int*) _temp77) == Cyc_Absyn_UnionType:
0){ _LL188: _temp187=(( struct Cyc_Absyn_UnionType_struct*) _temp77)->f1; goto
_LL186; _LL186: _temp185=(( struct Cyc_Absyn_UnionType_struct*) _temp77)->f2;
goto _LL106;} else{ goto _LL107;} _LL107: if(( unsigned int) _temp77 > 4u?*((
int*) _temp77) == Cyc_Absyn_AnonStructType: 0){ _LL190: _temp189=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp77)->f1; goto _LL108;} else{ goto _LL109;} _LL109: if(( unsigned int)
_temp77 > 4u?*(( int*) _temp77) == Cyc_Absyn_AnonUnionType: 0){ _LL192: _temp191=((
struct Cyc_Absyn_AnonUnionType_struct*) _temp77)->f1; goto _LL110;} else{ goto
_LL111;} _LL111: if(( unsigned int) _temp77 > 4u?*(( int*) _temp77) == Cyc_Absyn_EnumType:
0){ _LL194: _temp193=(( struct Cyc_Absyn_EnumType_struct*) _temp77)->f1; goto
_LL112;} else{ goto _LL113;} _LL113: if(( unsigned int) _temp77 > 4u?*(( int*)
_temp77) == Cyc_Absyn_RgnHandleType: 0){ _LL196: _temp195=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp77)->f1; goto _LL114;} else{ goto _LL115;} _LL115: if(( unsigned int)
_temp77 > 4u?*(( int*) _temp77) == Cyc_Absyn_TypedefType: 0){ _LL200: _temp199=((
struct Cyc_Absyn_TypedefType_struct*) _temp77)->f1; goto _LL198; _LL198:
_temp197=(( struct Cyc_Absyn_TypedefType_struct*) _temp77)->f2; goto _LL116;}
else{ goto _LL117;} _LL117: if( _temp77 ==( void*) Cyc_Absyn_HeapRgn){ goto
_LL118;} else{ goto _LL119;} _LL119: if(( unsigned int) _temp77 > 4u?*(( int*)
_temp77) == Cyc_Absyn_AccessEff: 0){ _LL202: _temp201=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp77)->f1; goto _LL120;} else{ goto _LL121;} _LL121: if(( unsigned int)
_temp77 > 4u?*(( int*) _temp77) == Cyc_Absyn_JoinEff: 0){ _LL204: _temp203=((
struct Cyc_Absyn_JoinEff_struct*) _temp77)->f1; goto _LL122;} else{ goto _LL78;}
_LL80: return t; _LL82: return t; _LL84: return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp205=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp205[ 0]=({ struct Cyc_Absyn_VarType_struct _temp206; _temp206.tag= Cyc_Absyn_VarType;
_temp206.f1= Cyc_Tcutil_copy_tvar( _temp123); _temp206;}); _temp205;}); _LL86:
return( void*)({ struct Cyc_Absyn_TunionType_struct* _temp207=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp207[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp208; _temp208.tag= Cyc_Absyn_TunionType;
_temp208.f1=({ struct Cyc_Absyn_TunionInfo _temp209; _temp209.tunion_info=( void*)
_temp131; _temp209.targs= Cyc_Tcutil_copy_types( _temp129); _temp209.rgn=( void*)
Cyc_Tcutil_copy_type( _temp127); _temp209;}); _temp208;}); _temp207;}); _LL88:
return( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp210=( struct Cyc_Absyn_TunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp210[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp211; _temp211.tag= Cyc_Absyn_TunionFieldType;
_temp211.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp212; _temp212.field_info=(
void*) _temp137; _temp212.targs= Cyc_Tcutil_copy_types( _temp135); _temp212;});
_temp211;}); _temp210;}); _LL90: { void* _temp213= Cyc_Tcutil_copy_type(
_temp149); void* _temp214= Cyc_Tcutil_copy_type( _temp147); struct Cyc_Absyn_Conref*
_temp215=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* c)) Cyc_Tcutil_copy_conref)(
_temp145); struct Cyc_Absyn_Tqual _temp216= _temp143; struct Cyc_Absyn_Conref*
_temp217= Cyc_Tcutil_copy_conref( _temp141); return( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp218=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp218[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp219; _temp219.tag= Cyc_Absyn_PointerType;
_temp219.f1=({ struct Cyc_Absyn_PtrInfo _temp220; _temp220.elt_typ=( void*)
_temp213; _temp220.rgn_typ=( void*) _temp214; _temp220.nullable= _temp215;
_temp220.tq= _temp216; _temp220.bounds= _temp217; _temp220;}); _temp219;});
_temp218;});} _LL92: return( void*)({ struct Cyc_Absyn_IntType_struct* _temp221=(
struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp221[ 0]=({ struct Cyc_Absyn_IntType_struct _temp222; _temp222.tag= Cyc_Absyn_IntType;
_temp222.f1=( void*) _temp153; _temp222.f2=( void*) _temp151; _temp222;});
_temp221;}); _LL94: goto _LL96; _LL96: return t; _LL98: return( void*)({ struct
Cyc_Absyn_ArrayType_struct* _temp223=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp223[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp224; _temp224.tag= Cyc_Absyn_ArrayType; _temp224.f1=(
void*) Cyc_Tcutil_copy_type( _temp159); _temp224.f2= _temp157; _temp224.f3=
_temp155; _temp224;}); _temp223;}); _LL100: { struct Cyc_List_List* _temp225=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_tvar, _temp177); struct
Cyc_Core_Opt* _temp226= _temp175 == 0? 0:({ struct Cyc_Core_Opt* _temp236=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp236->v=(
void*) Cyc_Tcutil_copy_type(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp175))->v); _temp236;}); void* _temp227= Cyc_Tcutil_copy_type( _temp173);
struct Cyc_List_List* _temp228=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)(
struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_arg,
_temp171); int _temp229= _temp169; struct Cyc_Absyn_VarargInfo* cyc_varargs2= 0;
if( _temp167 != 0){ struct Cyc_Absyn_VarargInfo* cv=( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp167); cyc_varargs2=({ struct Cyc_Absyn_VarargInfo* _temp230=(
struct Cyc_Absyn_VarargInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp230->name= cv->name; _temp230->tq= cv->tq; _temp230->type=( void*) Cyc_Tcutil_copy_type((
void*) cv->type); _temp230->rgn=( void*) Cyc_Tcutil_copy_type(( void*) cv->rgn);
_temp230->inject= cv->inject; _temp230;});}{ struct Cyc_List_List* _temp231=((
struct Cyc_List_List*(*)( struct _tuple8*(* f)( struct _tuple8*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_copy_rgncmp, _temp165); struct Cyc_List_List*
_temp232= _temp163; return( void*)({ struct Cyc_Absyn_FnType_struct* _temp233=(
struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp233[ 0]=({ struct Cyc_Absyn_FnType_struct _temp234; _temp234.tag= Cyc_Absyn_FnType;
_temp234.f1=({ struct Cyc_Absyn_FnInfo _temp235; _temp235.tvars= _temp225;
_temp235.effect= _temp226; _temp235.ret_typ=( void*) _temp227; _temp235.args=
_temp228; _temp235.c_varargs= _temp229; _temp235.cyc_varargs= cyc_varargs2;
_temp235.rgn_po= _temp231; _temp235.attributes= _temp232; _temp235;}); _temp234;});
_temp233;});}} _LL102: return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp237=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp237[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp238; _temp238.tag= Cyc_Absyn_TupleType;
_temp238.f1=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_tqt, _temp179);
_temp238;}); _temp237;}); _LL104: return( void*)({ struct Cyc_Absyn_StructType_struct*
_temp239=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp239[ 0]=({ struct Cyc_Absyn_StructType_struct _temp240; _temp240.tag= Cyc_Absyn_StructType;
_temp240.f1= _temp183; _temp240.f2= Cyc_Tcutil_copy_types( _temp181); _temp240.f3=
0; _temp240;}); _temp239;}); _LL106: return( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp241=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp241[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp242; _temp242.tag= Cyc_Absyn_UnionType;
_temp242.f1= _temp187; _temp242.f2= Cyc_Tcutil_copy_types( _temp185); _temp242.f3=
0; _temp242;}); _temp241;}); _LL108: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp243=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp243[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp244; _temp244.tag= Cyc_Absyn_AnonStructType; _temp244.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_copy_field, _temp189); _temp244;}); _temp243;});
_LL110: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp245=(
struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp245[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp246; _temp246.tag=
Cyc_Absyn_AnonUnionType; _temp246.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_field,
_temp191); _temp246;}); _temp245;}); _LL112: return( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp247=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp247[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp248; _temp248.tag= Cyc_Absyn_EnumType;
_temp248.f1= _temp193; _temp248.f2= 0; _temp248;}); _temp247;}); _LL114: return(
void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp249=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp249[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp250; _temp250.tag= Cyc_Absyn_RgnHandleType;
_temp250.f1=( void*) Cyc_Tcutil_copy_type( _temp195); _temp250;}); _temp249;});
_LL116: return( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp251=( struct
Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp251[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp252; _temp252.tag= Cyc_Absyn_TypedefType;
_temp252.f1= _temp199; _temp252.f2= Cyc_Tcutil_copy_types( _temp197); _temp252.f3=
0; _temp252;}); _temp251;}); _LL118: return t; _LL120: return( void*)({ struct
Cyc_Absyn_AccessEff_struct* _temp253=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp253[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp254; _temp254.tag= Cyc_Absyn_AccessEff; _temp254.f1=(
void*) Cyc_Tcutil_copy_type( _temp201); _temp254;}); _temp253;}); _LL122: return(
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp255=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp255[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp256; _temp256.tag= Cyc_Absyn_JoinEff; _temp256.f1= Cyc_Tcutil_copy_types(
_temp203); _temp256;}); _temp255;}); _LL78:;} static int Cyc_Tcutil_kind_leq(
void* k1, void* k2){ if( k1 == k2){ return 1;}{ struct _tuple8 _temp258=({
struct _tuple8 _temp257; _temp257.f1= k1; _temp257.f2= k2; _temp257;}); void*
_temp268; void* _temp270; void* _temp272; void* _temp274; void* _temp276; void*
_temp278; _LL260: _LL271: _temp270= _temp258.f1; if( _temp270 ==( void*) Cyc_Absyn_BoxKind){
goto _LL269;} else{ goto _LL262;} _LL269: _temp268= _temp258.f2; if( _temp268 ==(
void*) Cyc_Absyn_MemKind){ goto _LL261;} else{ goto _LL262;} _LL262: _LL275:
_temp274= _temp258.f1; if( _temp274 ==( void*) Cyc_Absyn_BoxKind){ goto _LL273;}
else{ goto _LL264;} _LL273: _temp272= _temp258.f2; if( _temp272 ==( void*) Cyc_Absyn_AnyKind){
goto _LL263;} else{ goto _LL264;} _LL264: _LL279: _temp278= _temp258.f1; if(
_temp278 ==( void*) Cyc_Absyn_MemKind){ goto _LL277;} else{ goto _LL266;} _LL277:
_temp276= _temp258.f2; if( _temp276 ==( void*) Cyc_Absyn_AnyKind){ goto _LL265;}
else{ goto _LL266;} _LL266: goto _LL267; _LL261: return 1; _LL263: return 1;
_LL265: return 1; _LL267: return 0; _LL259:;}} void* Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar* tv){ return Cyc_Absyn_conref_val( tv->kind);} void* Cyc_Tcutil_typ_kind(
void* t){ void* _temp280= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt* _temp332;
struct Cyc_Core_Opt* _temp334; struct Cyc_Absyn_Tvar* _temp336; void* _temp338;
struct Cyc_Absyn_TunionFieldInfo _temp340; void* _temp342; struct Cyc_Absyn_Tunionfield*
_temp344; struct Cyc_Absyn_TunionFieldInfo _temp346; void* _temp348; struct Cyc_Absyn_Structdecl**
_temp350; struct Cyc_Absyn_Uniondecl** _temp352; struct Cyc_Absyn_PtrInfo
_temp354; void* _temp356; struct Cyc_List_List* _temp358; struct Cyc_Core_Opt*
_temp360; _LL282: if(( unsigned int) _temp280 > 4u?*(( int*) _temp280) == Cyc_Absyn_Evar:
0){ _LL335: _temp334=(( struct Cyc_Absyn_Evar_struct*) _temp280)->f1; goto
_LL333; _LL333: _temp332=(( struct Cyc_Absyn_Evar_struct*) _temp280)->f2; goto
_LL283;} else{ goto _LL284;} _LL284: if(( unsigned int) _temp280 > 4u?*(( int*)
_temp280) == Cyc_Absyn_VarType: 0){ _LL337: _temp336=(( struct Cyc_Absyn_VarType_struct*)
_temp280)->f1; goto _LL285;} else{ goto _LL286;} _LL286: if( _temp280 ==( void*)
Cyc_Absyn_VoidType){ goto _LL287;} else{ goto _LL288;} _LL288: if(( unsigned int)
_temp280 > 4u?*(( int*) _temp280) == Cyc_Absyn_IntType: 0){ _LL339: _temp338=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp280)->f2; goto _LL289;} else{
goto _LL290;} _LL290: if( _temp280 ==( void*) Cyc_Absyn_FloatType){ goto _LL291;}
else{ goto _LL292;} _LL292: if( _temp280 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL293;} else{ goto _LL294;} _LL294: if(( unsigned int) _temp280 > 4u?*(( int*)
_temp280) == Cyc_Absyn_FnType: 0){ goto _LL295;} else{ goto _LL296;} _LL296: if((
unsigned int) _temp280 > 4u?*(( int*) _temp280) == Cyc_Absyn_RgnHandleType: 0){
goto _LL297;} else{ goto _LL298;} _LL298: if( _temp280 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL299;} else{ goto _LL300;} _LL300: if(( unsigned int) _temp280 > 4u?*((
int*) _temp280) == Cyc_Absyn_TunionType: 0){ goto _LL301;} else{ goto _LL302;}
_LL302: if(( unsigned int) _temp280 > 4u?*(( int*) _temp280) == Cyc_Absyn_TunionFieldType:
0){ _LL341: _temp340=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp280)->f1;
_LL343: _temp342=( void*) _temp340.field_info; if(*(( int*) _temp342) == Cyc_Absyn_KnownTunionfield){
_LL345: _temp344=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp342)->f2;
goto _LL303;} else{ goto _LL304;}} else{ goto _LL304;} _LL304: if(( unsigned int)
_temp280 > 4u?*(( int*) _temp280) == Cyc_Absyn_TunionFieldType: 0){ _LL347:
_temp346=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp280)->f1; _LL349:
_temp348=( void*) _temp346.field_info; if(*(( int*) _temp348) == Cyc_Absyn_UnknownTunionfield){
goto _LL305;} else{ goto _LL306;}} else{ goto _LL306;} _LL306: if(( unsigned int)
_temp280 > 4u?*(( int*) _temp280) == Cyc_Absyn_StructType: 0){ _LL351: _temp350=((
struct Cyc_Absyn_StructType_struct*) _temp280)->f3; if( _temp350 == 0){ goto
_LL307;} else{ goto _LL308;}} else{ goto _LL308;} _LL308: if(( unsigned int)
_temp280 > 4u?*(( int*) _temp280) == Cyc_Absyn_UnionType: 0){ _LL353: _temp352=((
struct Cyc_Absyn_UnionType_struct*) _temp280)->f3; if( _temp352 == 0){ goto
_LL309;} else{ goto _LL310;}} else{ goto _LL310;} _LL310: if(( unsigned int)
_temp280 > 4u?*(( int*) _temp280) == Cyc_Absyn_StructType: 0){ goto _LL311;}
else{ goto _LL312;} _LL312: if(( unsigned int) _temp280 > 4u?*(( int*) _temp280)
== Cyc_Absyn_UnionType: 0){ goto _LL313;} else{ goto _LL314;} _LL314: if((
unsigned int) _temp280 > 4u?*(( int*) _temp280) == Cyc_Absyn_AnonStructType: 0){
goto _LL315;} else{ goto _LL316;} _LL316: if(( unsigned int) _temp280 > 4u?*((
int*) _temp280) == Cyc_Absyn_AnonUnionType: 0){ goto _LL317;} else{ goto _LL318;}
_LL318: if(( unsigned int) _temp280 > 4u?*(( int*) _temp280) == Cyc_Absyn_EnumType:
0){ goto _LL319;} else{ goto _LL320;} _LL320: if(( unsigned int) _temp280 > 4u?*((
int*) _temp280) == Cyc_Absyn_PointerType: 0){ _LL355: _temp354=(( struct Cyc_Absyn_PointerType_struct*)
_temp280)->f1; goto _LL321;} else{ goto _LL322;} _LL322: if(( unsigned int)
_temp280 > 4u?*(( int*) _temp280) == Cyc_Absyn_ArrayType: 0){ _LL357: _temp356=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp280)->f1; goto _LL323;} else{
goto _LL324;} _LL324: if(( unsigned int) _temp280 > 4u?*(( int*) _temp280) ==
Cyc_Absyn_TupleType: 0){ _LL359: _temp358=(( struct Cyc_Absyn_TupleType_struct*)
_temp280)->f1; goto _LL325;} else{ goto _LL326;} _LL326: if(( unsigned int)
_temp280 > 4u?*(( int*) _temp280) == Cyc_Absyn_TypedefType: 0){ _LL361: _temp360=((
struct Cyc_Absyn_TypedefType_struct*) _temp280)->f3; goto _LL327;} else{ goto
_LL328;} _LL328: if(( unsigned int) _temp280 > 4u?*(( int*) _temp280) == Cyc_Absyn_AccessEff:
0){ goto _LL329;} else{ goto _LL330;} _LL330: if(( unsigned int) _temp280 > 4u?*((
int*) _temp280) == Cyc_Absyn_JoinEff: 0){ goto _LL331;} else{ goto _LL281;}
_LL283: return( void*)(( struct Cyc_Core_Opt*) _check_null( _temp334))->v;
_LL285: return Cyc_Absyn_conref_val( _temp336->kind); _LL287: return( void*) Cyc_Absyn_MemKind;
_LL289: return _temp338 ==( void*) Cyc_Absyn_B4?( void*) Cyc_Absyn_BoxKind:(
void*) Cyc_Absyn_MemKind; _LL291: return( void*) Cyc_Absyn_MemKind; _LL293:
return( void*) Cyc_Absyn_MemKind; _LL295: return( void*) Cyc_Absyn_MemKind;
_LL297: return( void*) Cyc_Absyn_BoxKind; _LL299: return( void*) Cyc_Absyn_RgnKind;
_LL301: return( void*) Cyc_Absyn_BoxKind; _LL303: if( _temp344->typs == 0){
return( void*) Cyc_Absyn_BoxKind;} else{ return( void*) Cyc_Absyn_MemKind;}
_LL305: return({ void* _temp362[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("typ_kind: Unresolved TunionFieldType",
sizeof( unsigned char), 37u), _tag_arr( _temp362, sizeof( void*), 0u));});
_LL307: return( void*) Cyc_Absyn_AnyKind; _LL309: return( void*) Cyc_Absyn_AnyKind;
_LL311: return( void*) Cyc_Absyn_MemKind; _LL313: return( void*) Cyc_Absyn_MemKind;
_LL315: return( void*) Cyc_Absyn_MemKind; _LL317: return( void*) Cyc_Absyn_MemKind;
_LL319: return( void*) Cyc_Absyn_BoxKind; _LL321: { void* _temp363=( void*)( Cyc_Absyn_compress_conref(
_temp354.bounds))->v; void* _temp373; void* _temp375; _LL365: if(( unsigned int)
_temp363 > 1u?*(( int*) _temp363) == Cyc_Absyn_Eq_constr: 0){ _LL374: _temp373=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp363)->f1; if( _temp373 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL366;} else{ goto _LL367;}} else{ goto
_LL367;} _LL367: if(( unsigned int) _temp363 > 1u?*(( int*) _temp363) == Cyc_Absyn_Eq_constr:
0){ _LL376: _temp375=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp363)->f1;
if(( unsigned int) _temp375 > 1u?*(( int*) _temp375) == Cyc_Absyn_Upper_b: 0){
goto _LL368;} else{ goto _LL369;}} else{ goto _LL369;} _LL369: if( _temp363 ==(
void*) Cyc_Absyn_No_constr){ goto _LL370;} else{ goto _LL371;} _LL371: if((
unsigned int) _temp363 > 1u?*(( int*) _temp363) == Cyc_Absyn_Forward_constr: 0){
goto _LL372;} else{ goto _LL364;} _LL366: return( void*) Cyc_Absyn_MemKind;
_LL368: return( void*) Cyc_Absyn_BoxKind; _LL370: return( void*) Cyc_Absyn_MemKind;
_LL372: return({ void* _temp377[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("typ_kind: forward constraint in ptr bounds",
sizeof( unsigned char), 43u), _tag_arr( _temp377, sizeof( void*), 0u));});
_LL364:;} _LL323: return( void*) Cyc_Absyn_MemKind; _LL325: return( void*) Cyc_Absyn_MemKind;
_LL327: return({ struct Cyc_Stdio_String_pa_struct _temp379; _temp379.tag= Cyc_Stdio_String_pa;
_temp379.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp378[ 1u]={&
_temp379}; Cyc_Tcutil_impos( _tag_arr("typ_kind: typedef found: %s", sizeof(
unsigned char), 28u), _tag_arr( _temp378, sizeof( void*), 1u));}}); _LL329:
return( void*) Cyc_Absyn_EffKind; _LL331: return( void*) Cyc_Absyn_EffKind;
_LL281:;} unsigned char Cyc_Tcutil_Unify[ 10u]="\000\000\000\000Unify"; int Cyc_Tcutil_unify(
void* t1, void* t2){ struct _handler_cons _temp380; _push_handler(& _temp380);{
int _temp382= 0; if( setjmp( _temp380.handler)){ _temp382= 1;} if( ! _temp382){
Cyc_Tcutil_unify_it( t1, t2);{ int _temp383= 1; _npop_handler( 0u); return
_temp383;}; _pop_handler();} else{ void* _temp381=( void*) _exn_thrown; void*
_temp385= _temp381; _LL387: if( _temp385 == Cyc_Tcutil_Unify){ goto _LL388;}
else{ goto _LL389;} _LL389: goto _LL390; _LL388: return 0; _LL390:( void) _throw(
_temp385); _LL386:;}}} static void Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List*
env, struct Cyc_List_List* ts); static void Cyc_Tcutil_occurs( void* evar,
struct Cyc_List_List* env, void* t){ void* _temp391= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp425; struct Cyc_Core_Opt* _temp427; struct Cyc_Core_Opt**
_temp429; struct Cyc_Core_Opt* _temp430; struct Cyc_Absyn_PtrInfo _temp432; void*
_temp434; struct Cyc_Absyn_FnInfo _temp436; struct Cyc_List_List* _temp438;
struct Cyc_List_List* _temp440; struct Cyc_Absyn_VarargInfo* _temp442; int
_temp444; struct Cyc_List_List* _temp446; void* _temp448; struct Cyc_Core_Opt*
_temp450; struct Cyc_List_List* _temp452; struct Cyc_List_List* _temp454; void*
_temp456; struct Cyc_Absyn_TunionInfo _temp458; void* _temp460; struct Cyc_List_List*
_temp462; struct Cyc_Core_Opt* _temp464; struct Cyc_List_List* _temp466; struct
Cyc_Absyn_TunionFieldInfo _temp468; struct Cyc_List_List* _temp470; struct Cyc_List_List*
_temp472; struct Cyc_List_List* _temp474; struct Cyc_List_List* _temp476; void*
_temp478; struct Cyc_List_List* _temp480; _LL393: if(( unsigned int) _temp391 >
4u?*(( int*) _temp391) == Cyc_Absyn_VarType: 0){ _LL426: _temp425=(( struct Cyc_Absyn_VarType_struct*)
_temp391)->f1; goto _LL394;} else{ goto _LL395;} _LL395: if(( unsigned int)
_temp391 > 4u?*(( int*) _temp391) == Cyc_Absyn_Evar: 0){ _LL431: _temp430=((
struct Cyc_Absyn_Evar_struct*) _temp391)->f2; goto _LL428; _LL428: _temp427=((
struct Cyc_Absyn_Evar_struct*) _temp391)->f4; _temp429=&(( struct Cyc_Absyn_Evar_struct*)
_temp391)->f4; goto _LL396;} else{ goto _LL397;} _LL397: if(( unsigned int)
_temp391 > 4u?*(( int*) _temp391) == Cyc_Absyn_PointerType: 0){ _LL433: _temp432=((
struct Cyc_Absyn_PointerType_struct*) _temp391)->f1; goto _LL398;} else{ goto
_LL399;} _LL399: if(( unsigned int) _temp391 > 4u?*(( int*) _temp391) == Cyc_Absyn_ArrayType:
0){ _LL435: _temp434=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp391)->f1;
goto _LL400;} else{ goto _LL401;} _LL401: if(( unsigned int) _temp391 > 4u?*((
int*) _temp391) == Cyc_Absyn_FnType: 0){ _LL437: _temp436=(( struct Cyc_Absyn_FnType_struct*)
_temp391)->f1; _LL453: _temp452= _temp436.tvars; goto _LL451; _LL451: _temp450=
_temp436.effect; goto _LL449; _LL449: _temp448=( void*) _temp436.ret_typ; goto
_LL447; _LL447: _temp446= _temp436.args; goto _LL445; _LL445: _temp444= _temp436.c_varargs;
goto _LL443; _LL443: _temp442= _temp436.cyc_varargs; goto _LL441; _LL441:
_temp440= _temp436.rgn_po; goto _LL439; _LL439: _temp438= _temp436.attributes;
goto _LL402;} else{ goto _LL403;} _LL403: if(( unsigned int) _temp391 > 4u?*((
int*) _temp391) == Cyc_Absyn_TupleType: 0){ _LL455: _temp454=(( struct Cyc_Absyn_TupleType_struct*)
_temp391)->f1; goto _LL404;} else{ goto _LL405;} _LL405: if(( unsigned int)
_temp391 > 4u?*(( int*) _temp391) == Cyc_Absyn_RgnHandleType: 0){ _LL457:
_temp456=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp391)->f1; goto
_LL406;} else{ goto _LL407;} _LL407: if(( unsigned int) _temp391 > 4u?*(( int*)
_temp391) == Cyc_Absyn_TunionType: 0){ _LL459: _temp458=(( struct Cyc_Absyn_TunionType_struct*)
_temp391)->f1; _LL463: _temp462= _temp458.targs; goto _LL461; _LL461: _temp460=(
void*) _temp458.rgn; goto _LL408;} else{ goto _LL409;} _LL409: if(( unsigned int)
_temp391 > 4u?*(( int*) _temp391) == Cyc_Absyn_TypedefType: 0){ _LL467: _temp466=((
struct Cyc_Absyn_TypedefType_struct*) _temp391)->f2; goto _LL465; _LL465:
_temp464=(( struct Cyc_Absyn_TypedefType_struct*) _temp391)->f3; goto _LL410;}
else{ goto _LL411;} _LL411: if(( unsigned int) _temp391 > 4u?*(( int*) _temp391)
== Cyc_Absyn_TunionFieldType: 0){ _LL469: _temp468=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp391)->f1; _LL471: _temp470= _temp468.targs; goto _LL412;} else{ goto _LL413;}
_LL413: if(( unsigned int) _temp391 > 4u?*(( int*) _temp391) == Cyc_Absyn_StructType:
0){ _LL473: _temp472=(( struct Cyc_Absyn_StructType_struct*) _temp391)->f2; goto
_LL414;} else{ goto _LL415;} _LL415: if(( unsigned int) _temp391 > 4u?*(( int*)
_temp391) == Cyc_Absyn_AnonStructType: 0){ _LL475: _temp474=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp391)->f1; goto _LL416;} else{ goto _LL417;} _LL417: if(( unsigned int)
_temp391 > 4u?*(( int*) _temp391) == Cyc_Absyn_AnonUnionType: 0){ _LL477:
_temp476=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp391)->f1; goto _LL418;}
else{ goto _LL419;} _LL419: if(( unsigned int) _temp391 > 4u?*(( int*) _temp391)
== Cyc_Absyn_AccessEff: 0){ _LL479: _temp478=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp391)->f1; goto _LL420;} else{ goto _LL421;} _LL421: if(( unsigned int)
_temp391 > 4u?*(( int*) _temp391) == Cyc_Absyn_JoinEff: 0){ _LL481: _temp480=((
struct Cyc_Absyn_JoinEff_struct*) _temp391)->f1; goto _LL422;} else{ goto _LL423;}
_LL423: goto _LL424; _LL394: if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, env, _temp425)){( int) _throw(( void*) Cyc_Tcutil_Unify);}
goto _LL392; _LL396: if( t == evar){( int) _throw(( void*) Cyc_Tcutil_Unify);}
else{ if( _temp430 != 0){ Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp430))->v);} else{ int problem= 0;{ struct Cyc_List_List* s=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(* _temp429))->v; for(
0; s != 0; s=(( struct Cyc_List_List*) _check_null( s))->tl){ if( !(( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( s))->hd)){ problem=
1; break;}}} if( problem){ struct Cyc_List_List* _temp482= 0;{ struct Cyc_List_List*
s=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(* _temp429))->v;
for( 0; s != 0; s=(( struct Cyc_List_List*) _check_null( s))->tl){ if((( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( s))->hd)){
_temp482=({ struct Cyc_List_List* _temp483=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp483->hd=( void*)(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( s))->hd); _temp483->tl= _temp482; _temp483;});}}}*
_temp429=({ struct Cyc_Core_Opt* _temp484=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp484->v=( void*) _temp482; _temp484;});}}}
goto _LL392; _LL398: Cyc_Tcutil_occurs( evar, env,( void*) _temp432.elt_typ);
Cyc_Tcutil_occurs( evar, env,( void*) _temp432.rgn_typ); goto _LL392; _LL400:
Cyc_Tcutil_occurs( evar, env, _temp434); goto _LL392; _LL402: if( _temp450 != 0){
Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp450))->v);} Cyc_Tcutil_occurs( evar, env, _temp448); for( 0; _temp446 != 0;
_temp446=(( struct Cyc_List_List*) _check_null( _temp446))->tl){ Cyc_Tcutil_occurs(
evar, env,(*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp446))->hd)).f3);}
if( _temp442 != 0){ void* _temp487; void* _temp489; struct Cyc_Absyn_VarargInfo
_temp485=*(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp442)); _LL490:
_temp489=( void*) _temp485.type; goto _LL488; _LL488: _temp487=( void*) _temp485.rgn;
goto _LL486; _LL486: Cyc_Tcutil_occurs( evar, env, _temp489); Cyc_Tcutil_occurs(
evar, env, _temp487);} for( 0; _temp440 != 0; _temp440=(( struct Cyc_List_List*)
_check_null( _temp440))->tl){ struct _tuple8 _temp493; void* _temp494; void*
_temp496; struct _tuple8* _temp491=( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( _temp440))->hd; _temp493=* _temp491; _LL497: _temp496= _temp493.f1;
goto _LL495; _LL495: _temp494= _temp493.f2; goto _LL492; _LL492: Cyc_Tcutil_occurs(
evar, env, _temp496); Cyc_Tcutil_occurs( evar, env, _temp494);} goto _LL392;
_LL404: for( 0; _temp454 != 0; _temp454=(( struct Cyc_List_List*) _check_null(
_temp454))->tl){ Cyc_Tcutil_occurs( evar, env,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp454))->hd)).f2);} goto _LL392; _LL406: Cyc_Tcutil_occurs( evar,
env, _temp456); goto _LL392; _LL408: Cyc_Tcutil_occurs( evar, env, _temp460);
Cyc_Tcutil_occurslist( evar, env, _temp462); goto _LL392; _LL410: _temp470=
_temp466; goto _LL412; _LL412: _temp472= _temp470; goto _LL414; _LL414: Cyc_Tcutil_occurslist(
evar, env, _temp472); goto _LL392; _LL416: _temp476= _temp474; goto _LL418;
_LL418: for( 0; _temp476 != 0; _temp476=(( struct Cyc_List_List*) _check_null(
_temp476))->tl){ Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp476))->hd)->type);} goto _LL392; _LL420:
Cyc_Tcutil_occurs( evar, env, _temp478); goto _LL392; _LL422: Cyc_Tcutil_occurslist(
evar, env, _temp480); goto _LL392; _LL424: goto _LL392; _LL392:;} static void
Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List* env, struct Cyc_List_List*
ts){ for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){ Cyc_Tcutil_occurs(
evar, env,( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);}} static
void Cyc_Tcutil_unify_list( struct Cyc_List_List* t1, struct Cyc_List_List* t2){
struct _handler_cons _temp498; _push_handler(& _temp498);{ int _temp500= 0; if(
setjmp( _temp498.handler)){ _temp500= 1;} if( ! _temp500){ Cyc_List_iter2( Cyc_Tcutil_unify_it,
t1, t2);; _pop_handler();} else{ void* _temp499=( void*) _exn_thrown; void*
_temp502= _temp499; _LL504: if( _temp502 == Cyc_List_List_mismatch){ goto _LL505;}
else{ goto _LL506;} _LL506: goto _LL507; _LL505:( int) _throw(( void*) Cyc_Tcutil_Unify);
_LL507:( void) _throw( _temp502); _LL503:;}}} static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2){ if(( tq1.q_const != tq2.q_const?
1: tq1.q_volatile != tq2.q_volatile)? 1: tq1.q_restrict != tq2.q_restrict){( int)
_throw(( void*) Cyc_Tcutil_Unify);}} int Cyc_Tcutil_equal_tqual( struct Cyc_Absyn_Tqual
tq1, struct Cyc_Absyn_Tqual tq2){ return( tq1.q_const == tq2.q_const? tq1.q_volatile
== tq2.q_volatile: 0)? tq1.q_restrict == tq2.q_restrict: 0;} static void Cyc_Tcutil_unify_it_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y){ x= Cyc_Absyn_compress_conref( x); y= Cyc_Absyn_compress_conref( y); if( x ==
y){ return;}{ void* _temp508=( void*) x->v; void* _temp516; _LL510: if( _temp508
==( void*) Cyc_Absyn_No_constr){ goto _LL511;} else{ goto _LL512;} _LL512: if((
unsigned int) _temp508 > 1u?*(( int*) _temp508) == Cyc_Absyn_Forward_constr: 0){
goto _LL513;} else{ goto _LL514;} _LL514: if(( unsigned int) _temp508 > 1u?*((
int*) _temp508) == Cyc_Absyn_Eq_constr: 0){ _LL517: _temp516=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp508)->f1; goto _LL515;} else{ goto _LL509;}
_LL511:( void*)( x->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp518=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp518[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp519; _temp519.tag= Cyc_Absyn_Forward_constr; _temp519.f1= y; _temp519;});
_temp518;}))); return; _LL513:( int) _throw(({ void* _temp520[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("unify_conref: forward after compress", sizeof( unsigned char), 37u),
_tag_arr( _temp520, sizeof( void*), 0u));})); _LL515: { void* _temp521=( void*)
y->v; void* _temp529; _LL523: if( _temp521 ==( void*) Cyc_Absyn_No_constr){ goto
_LL524;} else{ goto _LL525;} _LL525: if(( unsigned int) _temp521 > 1u?*(( int*)
_temp521) == Cyc_Absyn_Forward_constr: 0){ goto _LL526;} else{ goto _LL527;}
_LL527: if(( unsigned int) _temp521 > 1u?*(( int*) _temp521) == Cyc_Absyn_Eq_constr:
0){ _LL530: _temp529=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp521)->f1;
goto _LL528;} else{ goto _LL522;} _LL524:( void*)( y->v=( void*)(( void*) x->v));
return; _LL526:( int) _throw(({ void* _temp531[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("unify_conref: forward after compress(2)", sizeof( unsigned char), 40u),
_tag_arr( _temp531, sizeof( void*), 0u));})); _LL528: if( cmp( _temp516,
_temp529) != 0){( int) _throw(( void*) Cyc_Tcutil_Unify);} return; _LL522:;}
_LL509:;}} static int Cyc_Tcutil_unify_conrefs( int(* cmp)( void*, void*),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y){ struct _handler_cons
_temp532; _push_handler(& _temp532);{ int _temp534= 0; if( setjmp( _temp532.handler)){
_temp534= 1;} if( ! _temp534){ Cyc_Tcutil_unify_it_conrefs( cmp, x, y);{ int
_temp535= 1; _npop_handler( 0u); return _temp535;}; _pop_handler();} else{ void*
_temp533=( void*) _exn_thrown; void* _temp537= _temp533; _LL539: if( _temp537 ==
Cyc_Tcutil_Unify){ goto _LL540;} else{ goto _LL541;} _LL541: goto _LL542; _LL540:
return 0; _LL542:( void) _throw( _temp537); _LL538:;}}} static int Cyc_Tcutil_boundscmp(
void* b1, void* b2){ struct _tuple8 _temp544=({ struct _tuple8 _temp543;
_temp543.f1= b1; _temp543.f2= b2; _temp543;}); void* _temp554; void* _temp556;
void* _temp558; void* _temp560; void* _temp562; struct Cyc_Absyn_Exp* _temp564;
void* _temp566; struct Cyc_Absyn_Exp* _temp568; _LL546: _LL557: _temp556=
_temp544.f1; if( _temp556 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL555;} else{
goto _LL548;} _LL555: _temp554= _temp544.f2; if( _temp554 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL547;} else{ goto _LL548;} _LL548: _LL559: _temp558= _temp544.f1; if(
_temp558 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL549;} else{ goto _LL550;}
_LL550: _LL561: _temp560= _temp544.f2; if( _temp560 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL551;} else{ goto _LL552;} _LL552: _LL567: _temp566= _temp544.f1; if((
unsigned int) _temp566 > 1u?*(( int*) _temp566) == Cyc_Absyn_Upper_b: 0){ _LL569:
_temp568=(( struct Cyc_Absyn_Upper_b_struct*) _temp566)->f1; goto _LL563;} else{
goto _LL545;} _LL563: _temp562= _temp544.f2; if(( unsigned int) _temp562 > 1u?*((
int*) _temp562) == Cyc_Absyn_Upper_b: 0){ _LL565: _temp564=(( struct Cyc_Absyn_Upper_b_struct*)
_temp562)->f1; goto _LL553;} else{ goto _LL545;} _LL547: return 0; _LL549:
return - 1; _LL551: return 1; _LL553: { int i1=( int) Cyc_Evexp_eval_const_uint_exp(
_temp568); int i2=( int) Cyc_Evexp_eval_const_uint_exp( _temp564); if( i1 == i2){
return 0;} if( i1 < i2){ return - 1;} return 1;} _LL545:;} static int Cyc_Tcutil_equal_att(
void* a1, void* a2){ if( a1 == a2){ return 1;}{ struct _tuple8 _temp571=({
struct _tuple8 _temp570; _temp570.f1= a1; _temp570.f2= a2; _temp570;}); void*
_temp583; int _temp585; int _temp587; void* _temp589; void* _temp591; int
_temp593; int _temp595; void* _temp597; void* _temp599; int _temp601; void*
_temp603; int _temp605; void* _temp607; int _temp609; void* _temp611; int
_temp613; void* _temp615; struct _tagged_arr _temp617; void* _temp619; struct
_tagged_arr _temp621; _LL573: _LL592: _temp591= _temp571.f1; if(( unsigned int)
_temp591 > 16u?*(( int*) _temp591) == Cyc_Absyn_Format_att: 0){ _LL598: _temp597=(
void*)(( struct Cyc_Absyn_Format_att_struct*) _temp591)->f1; goto _LL596; _LL596:
_temp595=(( struct Cyc_Absyn_Format_att_struct*) _temp591)->f2; goto _LL594;
_LL594: _temp593=(( struct Cyc_Absyn_Format_att_struct*) _temp591)->f3; goto
_LL584;} else{ goto _LL575;} _LL584: _temp583= _temp571.f2; if(( unsigned int)
_temp583 > 16u?*(( int*) _temp583) == Cyc_Absyn_Format_att: 0){ _LL590: _temp589=(
void*)(( struct Cyc_Absyn_Format_att_struct*) _temp583)->f1; goto _LL588; _LL588:
_temp587=(( struct Cyc_Absyn_Format_att_struct*) _temp583)->f2; goto _LL586;
_LL586: _temp585=(( struct Cyc_Absyn_Format_att_struct*) _temp583)->f3; goto
_LL574;} else{ goto _LL575;} _LL575: _LL604: _temp603= _temp571.f1; if((
unsigned int) _temp603 > 16u?*(( int*) _temp603) == Cyc_Absyn_Regparm_att: 0){
_LL606: _temp605=(( struct Cyc_Absyn_Regparm_att_struct*) _temp603)->f1; goto
_LL600;} else{ goto _LL577;} _LL600: _temp599= _temp571.f2; if(( unsigned int)
_temp599 > 16u?*(( int*) _temp599) == Cyc_Absyn_Regparm_att: 0){ _LL602:
_temp601=(( struct Cyc_Absyn_Regparm_att_struct*) _temp599)->f1; goto _LL576;}
else{ goto _LL577;} _LL577: _LL612: _temp611= _temp571.f1; if(( unsigned int)
_temp611 > 16u?*(( int*) _temp611) == Cyc_Absyn_Aligned_att: 0){ _LL614:
_temp613=(( struct Cyc_Absyn_Aligned_att_struct*) _temp611)->f1; goto _LL608;}
else{ goto _LL579;} _LL608: _temp607= _temp571.f2; if(( unsigned int) _temp607 >
16u?*(( int*) _temp607) == Cyc_Absyn_Aligned_att: 0){ _LL610: _temp609=(( struct
Cyc_Absyn_Aligned_att_struct*) _temp607)->f1; goto _LL578;} else{ goto _LL579;}
_LL579: _LL620: _temp619= _temp571.f1; if(( unsigned int) _temp619 > 16u?*(( int*)
_temp619) == Cyc_Absyn_Section_att: 0){ _LL622: _temp621=(( struct Cyc_Absyn_Section_att_struct*)
_temp619)->f1; goto _LL616;} else{ goto _LL581;} _LL616: _temp615= _temp571.f2;
if(( unsigned int) _temp615 > 16u?*(( int*) _temp615) == Cyc_Absyn_Section_att:
0){ _LL618: _temp617=(( struct Cyc_Absyn_Section_att_struct*) _temp615)->f1;
goto _LL580;} else{ goto _LL581;} _LL581: goto _LL582; _LL574: return( _temp597
== _temp589? _temp595 == _temp587: 0)? _temp593 == _temp585: 0; _LL576: _temp613=
_temp605; _temp609= _temp601; goto _LL578; _LL578: return _temp613 == _temp609;
_LL580: return Cyc_String_strcmp( _temp621, _temp617) == 0; _LL582: return 0;
_LL572:;}} int Cyc_Tcutil_same_atts( struct Cyc_List_List* a1, struct Cyc_List_List*
a2){{ struct Cyc_List_List* a= a1; for( 0; a != 0; a=(( struct Cyc_List_List*)
_check_null( a))->tl){ if( ! Cyc_List_exists_c( Cyc_Tcutil_equal_att,( void*)((
struct Cyc_List_List*) _check_null( a))->hd, a2)){ return 0;}}}{ struct Cyc_List_List*
a= a2; for( 0; a != 0; a=(( struct Cyc_List_List*) _check_null( a))->tl){ if( !
Cyc_List_exists_c( Cyc_Tcutil_equal_att,( void*)(( struct Cyc_List_List*)
_check_null( a))->hd, a1)){ return 0;}}} return 1;} static void Cyc_Tcutil_normalize_effect(
void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp623= e; struct Cyc_List_List*
_temp629; struct Cyc_List_List** _temp631; _LL625: if(( unsigned int) _temp623 >
4u?*(( int*) _temp623) == Cyc_Absyn_JoinEff: 0){ _LL630: _temp629=(( struct Cyc_Absyn_JoinEff_struct*)
_temp623)->f1; _temp631=&(( struct Cyc_Absyn_JoinEff_struct*) _temp623)->f1;
goto _LL626;} else{ goto _LL627;} _LL627: goto _LL628; _LL626: { int nested_join=
0;{ struct Cyc_List_List* effs=* _temp631; for( 0; effs != 0; effs=(( struct Cyc_List_List*)
_check_null( effs))->tl){ void* _temp632=( void*)(( struct Cyc_List_List*)
_check_null( effs))->hd; Cyc_Tcutil_normalize_effect( _temp632);{ void* _temp633=
Cyc_Tcutil_compress( _temp632); _LL635: if(( unsigned int) _temp633 > 4u?*(( int*)
_temp633) == Cyc_Absyn_JoinEff: 0){ goto _LL636;} else{ goto _LL637;} _LL637:
goto _LL638; _LL636: nested_join= 1; goto _LL634; _LL638: goto _LL634; _LL634:;}}}
if( ! nested_join){ return;}{ struct Cyc_List_List* effects= 0;{ struct Cyc_List_List*
effs=* _temp631; for( 0; effs != 0; effs=(( struct Cyc_List_List*) _check_null(
effs))->tl){ void* _temp639= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*)
_check_null( effs))->hd); struct Cyc_List_List* _temp645; _LL641: if((
unsigned int) _temp639 > 4u?*(( int*) _temp639) == Cyc_Absyn_JoinEff: 0){ _LL646:
_temp645=(( struct Cyc_Absyn_JoinEff_struct*) _temp639)->f1; goto _LL642;} else{
goto _LL643;} _LL643: goto _LL644; _LL642: effects= Cyc_List_revappend( _temp645,
effects); goto _LL640; _LL644: effects=({ struct Cyc_List_List* _temp647=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp647->hd=(
void*) _temp639; _temp647->tl= effects; _temp647;}); goto _LL640; _LL640:;}}*
_temp631= Cyc_List_imp_rev( effects); goto _LL624;}} _LL628: goto _LL624; _LL624:;}}
struct _tuple9{ void* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; }
; static struct _tuple9* Cyc_Tcutil_get_effect_evar( void* t){ void* _temp648=
Cyc_Tcutil_compress( t); struct Cyc_List_List* _temp656; struct Cyc_List_List
_temp658; struct Cyc_List_List* _temp659; void* _temp661; struct Cyc_Core_Opt*
_temp663; struct Cyc_Core_Opt* _temp665; _LL650: if(( unsigned int) _temp648 > 4u?*((
int*) _temp648) == Cyc_Absyn_JoinEff: 0){ _LL657: _temp656=(( struct Cyc_Absyn_JoinEff_struct*)
_temp648)->f1; if( _temp656 == 0){ goto _LL652;} else{ _temp658=* _temp656;
_LL662: _temp661=( void*) _temp658.hd; goto _LL660; _LL660: _temp659= _temp658.tl;
goto _LL651;}} else{ goto _LL652;} _LL652: if(( unsigned int) _temp648 > 4u?*((
int*) _temp648) == Cyc_Absyn_Evar: 0){ _LL666: _temp665=(( struct Cyc_Absyn_Evar_struct*)
_temp648)->f1; goto _LL664; _LL664: _temp663=(( struct Cyc_Absyn_Evar_struct*)
_temp648)->f4; goto _LL653;} else{ goto _LL654;} _LL654: goto _LL655; _LL651: {
void* _temp667= Cyc_Tcutil_compress( _temp661); struct Cyc_Core_Opt* _temp673;
_LL669: if(( unsigned int) _temp667 > 4u?*(( int*) _temp667) == Cyc_Absyn_Evar:
0){ _LL674: _temp673=(( struct Cyc_Absyn_Evar_struct*) _temp667)->f4; goto
_LL670;} else{ goto _LL671;} _LL671: goto _LL672; _LL670: return({ struct
_tuple9* _temp675=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp675->f1= _temp661; _temp675->f2= _temp659; _temp675->f3=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp673))->v; _temp675;}); _LL672: return 0;
_LL668:;} _LL653: if( _temp665 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp665))->v !=( void*) Cyc_Absyn_EffKind){({ void* _temp676[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("effect evar has wrong kind", sizeof( unsigned char), 27u), _tag_arr(
_temp676, sizeof( void*), 0u));});} return({ struct _tuple9* _temp677=( struct
_tuple9*) GC_malloc( sizeof( struct _tuple9)); _temp677->f1= t; _temp677->f2= 0;
_temp677->f3=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp663))->v; _temp677;}); _LL655: return 0; _LL649:;} static struct Cyc_Core_Opt
Cyc_Tcutil_ek={( void*)(( void*) 4u)}; int Cyc_Tcutil_region_in_effect( int
constrain, void* r, void* e){ r= Cyc_Tcutil_compress( r); if( r ==( void*) Cyc_Absyn_HeapRgn){
return 1;}{ void* _temp678= Cyc_Tcutil_compress( e); void* _temp688; struct Cyc_List_List*
_temp690; struct Cyc_Core_Opt* _temp692; struct Cyc_Core_Opt* _temp694; struct
Cyc_Core_Opt** _temp696; struct Cyc_Core_Opt* _temp697; _LL680: if((
unsigned int) _temp678 > 4u?*(( int*) _temp678) == Cyc_Absyn_AccessEff: 0){
_LL689: _temp688=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp678)->f1;
goto _LL681;} else{ goto _LL682;} _LL682: if(( unsigned int) _temp678 > 4u?*((
int*) _temp678) == Cyc_Absyn_JoinEff: 0){ _LL691: _temp690=(( struct Cyc_Absyn_JoinEff_struct*)
_temp678)->f1; goto _LL683;} else{ goto _LL684;} _LL684: if(( unsigned int)
_temp678 > 4u?*(( int*) _temp678) == Cyc_Absyn_Evar: 0){ _LL698: _temp697=((
struct Cyc_Absyn_Evar_struct*) _temp678)->f1; goto _LL695; _LL695: _temp694=((
struct Cyc_Absyn_Evar_struct*) _temp678)->f2; _temp696=&(( struct Cyc_Absyn_Evar_struct*)
_temp678)->f2; goto _LL693; _LL693: _temp692=(( struct Cyc_Absyn_Evar_struct*)
_temp678)->f4; goto _LL685;} else{ goto _LL686;} _LL686: goto _LL687; _LL681:
if( constrain){ return Cyc_Tcutil_unify( r, _temp688);} Cyc_Tcutil_compress(
_temp688); if( r == _temp688){ return 1;}{ struct _tuple8 _temp700=({ struct
_tuple8 _temp699; _temp699.f1= r; _temp699.f2= _temp688; _temp699;}); void*
_temp706; struct Cyc_Absyn_Tvar* _temp708; void* _temp710; struct Cyc_Absyn_Tvar*
_temp712; _LL702: _LL711: _temp710= _temp700.f1; if(( unsigned int) _temp710 > 4u?*((
int*) _temp710) == Cyc_Absyn_VarType: 0){ _LL713: _temp712=(( struct Cyc_Absyn_VarType_struct*)
_temp710)->f1; goto _LL707;} else{ goto _LL704;} _LL707: _temp706= _temp700.f2;
if(( unsigned int) _temp706 > 4u?*(( int*) _temp706) == Cyc_Absyn_VarType: 0){
_LL709: _temp708=(( struct Cyc_Absyn_VarType_struct*) _temp706)->f1; goto _LL703;}
else{ goto _LL704;} _LL704: goto _LL705; _LL703: return Cyc_Absyn_tvar_cmp(
_temp712, _temp708) == 0; _LL705: return 0; _LL701:;} _LL683: for( 0; _temp690
!= 0; _temp690=(( struct Cyc_List_List*) _check_null( _temp690))->tl){ if( Cyc_Tcutil_region_in_effect(
constrain, r,( void*)(( struct Cyc_List_List*) _check_null( _temp690))->hd)){
return 1;}} return 0; _LL685: if( _temp697 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp697))->v !=( void*) Cyc_Absyn_EffKind){({ void* _temp714[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("effect evar has wrong kind", sizeof( unsigned char), 27u), _tag_arr(
_temp714, sizeof( void*), 0u));});} if( ! constrain){ return 0;}{ void* _temp715=
Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek, _temp692); Cyc_Tcutil_occurs(
_temp715,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp692))->v,
r);{ struct Cyc_Absyn_JoinEff_struct* _temp716=({ struct Cyc_Absyn_JoinEff_struct*
_temp718=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp718[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp719; _temp719.tag= Cyc_Absyn_JoinEff;
_temp719.f1=({ struct Cyc_List_List* _temp720=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp720->hd=( void*) _temp715; _temp720->tl=({
struct Cyc_List_List* _temp721=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp721->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp722=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp722[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp723; _temp723.tag= Cyc_Absyn_AccessEff;
_temp723.f1=( void*) r; _temp723;}); _temp722;})); _temp721->tl= 0; _temp721;});
_temp720;}); _temp719;}); _temp718;});* _temp696=({ struct Cyc_Core_Opt*
_temp717=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp717->v=( void*)(( void*) _temp716); _temp717;}); return 1;}} _LL687: return
0; _LL679:;}} static int Cyc_Tcutil_variable_in_effect( int constrain, struct
Cyc_Absyn_Tvar* v, void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp724= e;
struct Cyc_Absyn_Tvar* _temp734; struct Cyc_List_List* _temp736; struct Cyc_Core_Opt*
_temp738; struct Cyc_Core_Opt* _temp740; struct Cyc_Core_Opt** _temp742; struct
Cyc_Core_Opt* _temp743; _LL726: if(( unsigned int) _temp724 > 4u?*(( int*)
_temp724) == Cyc_Absyn_VarType: 0){ _LL735: _temp734=(( struct Cyc_Absyn_VarType_struct*)
_temp724)->f1; goto _LL727;} else{ goto _LL728;} _LL728: if(( unsigned int)
_temp724 > 4u?*(( int*) _temp724) == Cyc_Absyn_JoinEff: 0){ _LL737: _temp736=((
struct Cyc_Absyn_JoinEff_struct*) _temp724)->f1; goto _LL729;} else{ goto _LL730;}
_LL730: if(( unsigned int) _temp724 > 4u?*(( int*) _temp724) == Cyc_Absyn_Evar:
0){ _LL744: _temp743=(( struct Cyc_Absyn_Evar_struct*) _temp724)->f1; goto
_LL741; _LL741: _temp740=(( struct Cyc_Absyn_Evar_struct*) _temp724)->f2;
_temp742=&(( struct Cyc_Absyn_Evar_struct*) _temp724)->f2; goto _LL739; _LL739:
_temp738=(( struct Cyc_Absyn_Evar_struct*) _temp724)->f4; goto _LL731;} else{
goto _LL732;} _LL732: goto _LL733; _LL727: return Cyc_Absyn_tvar_cmp( v,
_temp734) == 0; _LL729: for( 0; _temp736 != 0; _temp736=(( struct Cyc_List_List*)
_check_null( _temp736))->tl){ if( Cyc_Tcutil_variable_in_effect( constrain, v,(
void*)(( struct Cyc_List_List*) _check_null( _temp736))->hd)){ return 1;}}
return 0; _LL731: if( _temp743 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp743))->v !=( void*) Cyc_Absyn_EffKind){({ void* _temp745[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("effect evar has wrong kind", sizeof( unsigned char), 27u), _tag_arr(
_temp745, sizeof( void*), 0u));});}{ void* _temp746= Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Tcutil_ek, _temp738); if( !(( int(*)( int(* compare)( struct
Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar*
x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( _temp738))->v, v)){ return 0;}{ struct Cyc_Absyn_JoinEff_struct*
_temp747=({ struct Cyc_Absyn_JoinEff_struct* _temp749=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp749[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp750; _temp750.tag= Cyc_Absyn_JoinEff; _temp750.f1=({ struct Cyc_List_List*
_temp751=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp751->hd=( void*) _temp746; _temp751->tl=({ struct Cyc_List_List* _temp752=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp752->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp753=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp753[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp754; _temp754.tag= Cyc_Absyn_VarType; _temp754.f1= v; _temp754;}); _temp753;}));
_temp752->tl= 0; _temp752;}); _temp751;}); _temp750;}); _temp749;});* _temp742=({
struct Cyc_Core_Opt* _temp748=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp748->v=( void*)(( void*) _temp747); _temp748;}); return 1;}}
_LL733: return 0; _LL725:;}} static int Cyc_Tcutil_evar_in_effect( void* evar,
void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp755= e; struct Cyc_List_List*
_temp763; _LL757: if(( unsigned int) _temp755 > 4u?*(( int*) _temp755) == Cyc_Absyn_JoinEff:
0){ _LL764: _temp763=(( struct Cyc_Absyn_JoinEff_struct*) _temp755)->f1; goto
_LL758;} else{ goto _LL759;} _LL759: if(( unsigned int) _temp755 > 4u?*(( int*)
_temp755) == Cyc_Absyn_Evar: 0){ goto _LL760;} else{ goto _LL761;} _LL761: goto
_LL762; _LL758: for( 0; _temp763 != 0; _temp763=(( struct Cyc_List_List*)
_check_null( _temp763))->tl){ if( Cyc_Tcutil_evar_in_effect( evar,( void*)((
struct Cyc_List_List*) _check_null( _temp763))->hd)){ return 1;}} return 0;
_LL760: return evar == e; _LL762: return 0; _LL756:;}} int Cyc_Tcutil_subset_effect(
int set_to_empty, void* e1, void* e2){{ struct _tuple9* _temp765= Cyc_Tcutil_get_effect_evar(
e2); struct _tuple9 _temp771; struct Cyc_List_List* _temp772; struct Cyc_List_List*
_temp774; void* _temp776; _LL767: if( _temp765 == 0){ goto _LL769;} else{
_temp771=* _temp765; _LL777: _temp776= _temp771.f1; goto _LL775; _LL775:
_temp774= _temp771.f2; goto _LL773; _LL773: _temp772= _temp771.f3; goto _LL768;}
_LL769: goto _LL770; _LL768: { void* _temp778= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,({ struct Cyc_Core_Opt* _temp784=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp784->v=( void*) _temp772; _temp784;}));
struct Cyc_Absyn_JoinEff_struct* _temp779=({ struct Cyc_Absyn_JoinEff_struct*
_temp780=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp780[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp781; _temp781.tag= Cyc_Absyn_JoinEff;
_temp781.f1=({ struct Cyc_List_List* _temp782=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp782->hd=( void*) _temp778; _temp782->tl=({
struct Cyc_List_List* _temp783=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp783->hd=( void*) e1; _temp783->tl= 0; _temp783;});
_temp782;}); _temp781;}); _temp780;}); return Cyc_Tcutil_unify( _temp776,( void*)
_temp779);} _LL770: goto _LL766; _LL766:;}{ void* _temp785= Cyc_Tcutil_compress(
e1); struct Cyc_List_List* _temp797; void* _temp799; struct Cyc_Absyn_Tvar*
_temp801; struct Cyc_Core_Opt* _temp803; struct Cyc_Core_Opt* _temp805; struct
Cyc_Core_Opt** _temp807; _LL787: if(( unsigned int) _temp785 > 4u?*(( int*)
_temp785) == Cyc_Absyn_JoinEff: 0){ _LL798: _temp797=(( struct Cyc_Absyn_JoinEff_struct*)
_temp785)->f1; goto _LL788;} else{ goto _LL789;} _LL789: if(( unsigned int)
_temp785 > 4u?*(( int*) _temp785) == Cyc_Absyn_AccessEff: 0){ _LL800: _temp799=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp785)->f1; goto _LL790;} else{
goto _LL791;} _LL791: if(( unsigned int) _temp785 > 4u?*(( int*) _temp785) ==
Cyc_Absyn_VarType: 0){ _LL802: _temp801=(( struct Cyc_Absyn_VarType_struct*)
_temp785)->f1; goto _LL792;} else{ goto _LL793;} _LL793: if(( unsigned int)
_temp785 > 4u?*(( int*) _temp785) == Cyc_Absyn_Evar: 0){ _LL806: _temp805=((
struct Cyc_Absyn_Evar_struct*) _temp785)->f2; _temp807=&(( struct Cyc_Absyn_Evar_struct*)
_temp785)->f2; goto _LL804; _LL804: _temp803=(( struct Cyc_Absyn_Evar_struct*)
_temp785)->f4; goto _LL794;} else{ goto _LL795;} _LL795: goto _LL796; _LL788:
for( 0; _temp797 != 0; _temp797=(( struct Cyc_List_List*) _check_null( _temp797))->tl){
if( ! Cyc_Tcutil_subset_effect( set_to_empty,( void*)(( struct Cyc_List_List*)
_check_null( _temp797))->hd, e2)){ return 0;}} return 1; _LL790: return( Cyc_Tcutil_region_in_effect(
0, _temp799, e2)? 1: Cyc_Tcutil_region_in_effect( 1, _temp799, e2))? 1: Cyc_Tcutil_unify(
_temp799,( void*) Cyc_Absyn_HeapRgn); _LL792: return Cyc_Tcutil_variable_in_effect(
0, _temp801, e2)? 1: Cyc_Tcutil_variable_in_effect( 1, _temp801, e2); _LL794:
if( ! Cyc_Tcutil_evar_in_effect( e1, e2)){ if( set_to_empty){* _temp807=({
struct Cyc_Core_Opt* _temp808=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp808->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp809=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp809[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp810; _temp810.tag= Cyc_Absyn_JoinEff;
_temp810.f1= 0; _temp810;}); _temp809;})); _temp808;});} else{ Cyc_Tcutil_occurs(
e1,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp803))->v,
e2);* _temp807=({ struct Cyc_Core_Opt* _temp811=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp811->v=( void*) e2; _temp811;});}}
return 1; _LL796: return({ struct Cyc_Stdio_String_pa_struct _temp813; _temp813.tag=
Cyc_Stdio_String_pa; _temp813.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
e1);{ void* _temp812[ 1u]={& _temp813};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("subset_effect: bad effect: %s",
sizeof( unsigned char), 30u), _tag_arr( _temp812, sizeof( void*), 1u));}});
_LL786:;}} struct _tuple10{ struct _tuple9* f1; struct _tuple9* f2; } ; static
int Cyc_Tcutil_unify_effect( void* e1, void* e2){ e1= Cyc_Tcutil_compress( e1);
e2= Cyc_Tcutil_compress( e2); Cyc_Tcutil_normalize_effect( e1); Cyc_Tcutil_normalize_effect(
e2);{ struct _tuple10 _temp815=({ struct _tuple10 _temp814; _temp814.f1= Cyc_Tcutil_get_effect_evar(
e1); _temp814.f2= Cyc_Tcutil_get_effect_evar( e2); _temp814;}); struct _tuple9*
_temp821; struct _tuple9 _temp823; struct Cyc_List_List* _temp824; struct Cyc_List_List*
_temp826; void* _temp828; struct _tuple9* _temp830; struct _tuple9 _temp832;
struct Cyc_List_List* _temp833; struct Cyc_List_List* _temp835; void* _temp837;
_LL817: _LL831: _temp830= _temp815.f1; if( _temp830 == 0){ goto _LL819;} else{
_temp832=* _temp830; _LL838: _temp837= _temp832.f1; goto _LL836; _LL836:
_temp835= _temp832.f2; goto _LL834; _LL834: _temp833= _temp832.f3; goto _LL822;}
_LL822: _temp821= _temp815.f2; if( _temp821 == 0){ goto _LL819;} else{ _temp823=*
_temp821; _LL829: _temp828= _temp823.f1; goto _LL827; _LL827: _temp826= _temp823.f2;
goto _LL825; _LL825: _temp824= _temp823.f3; goto _LL818;} _LL819: goto _LL820;
_LL818: { struct Cyc_List_List* _temp839= 0; for( 0; _temp833 != 0; _temp833=((
struct Cyc_List_List*) _check_null( _temp833))->tl){ if((( int(*)( int(* compare)(
struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct
Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, _temp824,( struct
Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp833))->hd)){
_temp839=({ struct Cyc_List_List* _temp840=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp840->hd=( void*)(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp833))->hd); _temp840->tl= _temp839;
_temp840;});}}{ void* _temp841= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,({
struct Cyc_Core_Opt* _temp846=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp846->v=( void*) _temp839; _temp846;})); struct Cyc_Absyn_JoinEff_struct*
_temp842=({ struct Cyc_Absyn_JoinEff_struct* _temp843=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp843[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp844; _temp844.tag= Cyc_Absyn_JoinEff; _temp844.f1=({ struct Cyc_List_List*
_temp845=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp845->hd=( void*) _temp841; _temp845->tl= Cyc_List_append( _temp835,
_temp826); _temp845;}); _temp844;}); _temp843;}); return Cyc_Tcutil_unify(
_temp837,( void*) _temp842)? Cyc_Tcutil_unify( _temp828,( void*) _temp842): 0;}}
_LL820: return Cyc_Tcutil_subset_effect( 0, e1, e2)? Cyc_Tcutil_subset_effect( 0,
e2, e1): 0; _LL816:;}} static int Cyc_Tcutil_sub_rgnpo( struct Cyc_List_List*
rpo1, struct Cyc_List_List* rpo2){{ struct Cyc_List_List* r1= rpo1; for( 0; r1
!= 0; r1=(( struct Cyc_List_List*) _check_null( r1))->tl){ struct _tuple8
_temp849; void* _temp850; void* _temp852; struct _tuple8* _temp847=( struct
_tuple8*)(( struct Cyc_List_List*) _check_null( r1))->hd; _temp849=* _temp847;
_LL853: _temp852= _temp849.f1; goto _LL851; _LL851: _temp850= _temp849.f2; goto
_LL848; _LL848: { int found= _temp852 ==( void*) Cyc_Absyn_HeapRgn;{ struct Cyc_List_List*
r2= rpo2; for( 0; r2 != 0? ! found: 0; r2=(( struct Cyc_List_List*) _check_null(
r2))->tl){ struct _tuple8 _temp856; void* _temp857; void* _temp859; struct
_tuple8* _temp854=( struct _tuple8*)(( struct Cyc_List_List*) _check_null( r2))->hd;
_temp856=* _temp854; _LL860: _temp859= _temp856.f1; goto _LL858; _LL858:
_temp857= _temp856.f2; goto _LL855; _LL855: if( Cyc_Tcutil_unify( _temp852,
_temp859)? Cyc_Tcutil_unify( _temp850, _temp857): 0){ found= 1; break;}}} if( !
found){ return 0;}}}} return 1;} static int Cyc_Tcutil_same_rgn_po( struct Cyc_List_List*
rpo1, struct Cyc_List_List* rpo2){ return Cyc_Tcutil_sub_rgnpo( rpo1, rpo2)? Cyc_Tcutil_sub_rgnpo(
rpo2, rpo1): 0;} struct _tuple11{ struct Cyc_Absyn_VarargInfo* f1; struct Cyc_Absyn_VarargInfo*
f2; } ; struct _tuple12{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ;
void Cyc_Tcutil_unify_it( void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2=
Cyc_Tcutil_compress( t2); if( t1 == t2){ return;}{ void* _temp861= t1; struct
Cyc_Core_Opt* _temp867; struct Cyc_Core_Opt* _temp869; struct Cyc_Core_Opt**
_temp871; struct Cyc_Core_Opt* _temp872; _LL863: if(( unsigned int) _temp861 > 4u?*((
int*) _temp861) == Cyc_Absyn_Evar: 0){ _LL873: _temp872=(( struct Cyc_Absyn_Evar_struct*)
_temp861)->f1; goto _LL870; _LL870: _temp869=(( struct Cyc_Absyn_Evar_struct*)
_temp861)->f2; _temp871=&(( struct Cyc_Absyn_Evar_struct*) _temp861)->f2; goto
_LL868; _LL868: _temp867=(( struct Cyc_Absyn_Evar_struct*) _temp861)->f4; goto
_LL864;} else{ goto _LL865;} _LL865: goto _LL866; _LL864: Cyc_Tcutil_occurs( t1,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp867))->v, t2);{
void* _temp874= Cyc_Tcutil_typ_kind( t2); if( Cyc_Tcutil_kind_leq( _temp874,(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp872))->v)){* _temp871=({ struct
Cyc_Core_Opt* _temp875=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp875->v=( void*) t2; _temp875;}); return;} else{{ void* _temp876= t2; struct
Cyc_Core_Opt* _temp884; struct Cyc_Core_Opt* _temp886; struct Cyc_Core_Opt**
_temp888; struct Cyc_Absyn_PtrInfo _temp890; _LL878: if(( unsigned int) _temp876
> 4u?*(( int*) _temp876) == Cyc_Absyn_Evar: 0){ _LL887: _temp886=(( struct Cyc_Absyn_Evar_struct*)
_temp876)->f2; _temp888=&(( struct Cyc_Absyn_Evar_struct*) _temp876)->f2; goto
_LL885; _LL885: _temp884=(( struct Cyc_Absyn_Evar_struct*) _temp876)->f4; goto
_LL879;} else{ goto _LL880;} _LL880: if(( unsigned int) _temp876 > 4u?*(( int*)
_temp876) == Cyc_Absyn_PointerType: 0){ _LL891: _temp890=(( struct Cyc_Absyn_PointerType_struct*)
_temp876)->f1; goto _LL889;} else{ goto _LL882;} _LL889: if(( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp872))->v ==( void*) Cyc_Absyn_BoxKind){ goto
_LL881;} else{ goto _LL882;} _LL882: goto _LL883; _LL879: { struct Cyc_List_List*
_temp892=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp867))->v;{
struct Cyc_List_List* s2=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp884))->v; for( 0; s2 != 0; s2=(( struct Cyc_List_List*)
_check_null( s2))->tl){ if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, _temp892,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( s2))->hd)){( int) _throw(( void*) Cyc_Tcutil_Unify);}}} if( Cyc_Tcutil_kind_leq((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp872))->v, _temp874)){* _temp888=({
struct Cyc_Core_Opt* _temp893=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp893->v=( void*) t1; _temp893;}); return;} goto _LL877;}
_LL881: { struct Cyc_Absyn_Conref* _temp894= Cyc_Absyn_compress_conref( _temp890.bounds);{
void* _temp895=( void*) _temp894->v; _LL897: if( _temp895 ==( void*) Cyc_Absyn_No_constr){
goto _LL898;} else{ goto _LL899;} _LL899: goto _LL900; _LL898:( void*)( _temp894->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp901=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp901[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp902; _temp902.tag= Cyc_Absyn_Eq_constr; _temp902.f1=(
void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp903=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp903[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp904; _temp904.tag= Cyc_Absyn_Upper_b; _temp904.f1= Cyc_Absyn_signed_int_exp(
1, 0); _temp904;}); _temp903;})); _temp902;}); _temp901;})));* _temp871=({
struct Cyc_Core_Opt* _temp905=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp905->v=( void*) t2; _temp905;}); return; _LL900: goto
_LL896; _LL896:;} goto _LL877;} _LL883: goto _LL877; _LL877:;}( int) _throw((
void*) Cyc_Tcutil_Unify);}} _LL866: goto _LL862; _LL862:;}{ struct _tuple8
_temp907=({ struct _tuple8 _temp906; _temp906.f1= t2; _temp906.f2= t1; _temp906;});
void* _temp957; void* _temp959; void* _temp961; void* _temp963; struct Cyc_Absyn_Tvar*
_temp965; void* _temp967; struct Cyc_Absyn_Tvar* _temp969; void* _temp971;
struct Cyc_Absyn_Structdecl** _temp973; struct Cyc_List_List* _temp975; struct
_tuple1* _temp977; void* _temp979; struct Cyc_Absyn_Structdecl** _temp981;
struct Cyc_List_List* _temp983; struct _tuple1* _temp985; void* _temp987; struct
_tuple1* _temp989; void* _temp991; struct _tuple1* _temp993; void* _temp995;
struct Cyc_Absyn_Uniondecl** _temp997; struct Cyc_List_List* _temp999; struct
_tuple1* _temp1001; void* _temp1003; struct Cyc_Absyn_Uniondecl** _temp1005;
struct Cyc_List_List* _temp1007; struct _tuple1* _temp1009; void* _temp1011;
struct Cyc_Absyn_TunionInfo _temp1013; void* _temp1015; struct Cyc_List_List*
_temp1017; void* _temp1019; struct Cyc_Absyn_Tuniondecl* _temp1021; void*
_temp1023; struct Cyc_Absyn_TunionInfo _temp1025; void* _temp1027; struct Cyc_List_List*
_temp1029; void* _temp1031; struct Cyc_Absyn_Tuniondecl* _temp1033; void*
_temp1035; struct Cyc_Absyn_TunionFieldInfo _temp1037; struct Cyc_List_List*
_temp1039; void* _temp1041; struct Cyc_Absyn_Tunionfield* _temp1043; struct Cyc_Absyn_Tuniondecl*
_temp1045; void* _temp1047; struct Cyc_Absyn_TunionFieldInfo _temp1049; struct
Cyc_List_List* _temp1051; void* _temp1053; struct Cyc_Absyn_Tunionfield*
_temp1055; struct Cyc_Absyn_Tuniondecl* _temp1057; void* _temp1059; struct Cyc_Absyn_PtrInfo
_temp1061; struct Cyc_Absyn_Conref* _temp1063; struct Cyc_Absyn_Tqual _temp1065;
struct Cyc_Absyn_Conref* _temp1067; void* _temp1069; void* _temp1071; void*
_temp1073; struct Cyc_Absyn_PtrInfo _temp1075; struct Cyc_Absyn_Conref*
_temp1077; struct Cyc_Absyn_Tqual _temp1079; struct Cyc_Absyn_Conref* _temp1081;
void* _temp1083; void* _temp1085; void* _temp1087; void* _temp1089; void*
_temp1091; void* _temp1093; void* _temp1095; void* _temp1097; void* _temp1099;
void* _temp1101; void* _temp1103; void* _temp1105; void* _temp1107; struct Cyc_Absyn_Exp*
_temp1109; struct Cyc_Absyn_Tqual _temp1111; void* _temp1113; void* _temp1115;
struct Cyc_Absyn_Exp* _temp1117; struct Cyc_Absyn_Tqual _temp1119; void*
_temp1121; void* _temp1123; struct Cyc_Absyn_FnInfo _temp1125; struct Cyc_List_List*
_temp1127; struct Cyc_List_List* _temp1129; struct Cyc_Absyn_VarargInfo*
_temp1131; int _temp1133; struct Cyc_List_List* _temp1135; void* _temp1137;
struct Cyc_Core_Opt* _temp1139; struct Cyc_List_List* _temp1141; void* _temp1143;
struct Cyc_Absyn_FnInfo _temp1145; struct Cyc_List_List* _temp1147; struct Cyc_List_List*
_temp1149; struct Cyc_Absyn_VarargInfo* _temp1151; int _temp1153; struct Cyc_List_List*
_temp1155; void* _temp1157; struct Cyc_Core_Opt* _temp1159; struct Cyc_List_List*
_temp1161; void* _temp1163; struct Cyc_List_List* _temp1165; void* _temp1167;
struct Cyc_List_List* _temp1169; void* _temp1171; struct Cyc_List_List*
_temp1173; void* _temp1175; struct Cyc_List_List* _temp1177; void* _temp1179;
struct Cyc_List_List* _temp1181; void* _temp1183; struct Cyc_List_List*
_temp1185; void* _temp1187; void* _temp1189; void* _temp1191; void* _temp1193;
void* _temp1195; void* _temp1197; void* _temp1199; void* _temp1201; void*
_temp1203; void* _temp1205; _LL909: _LL958: _temp957= _temp907.f1; if((
unsigned int) _temp957 > 4u?*(( int*) _temp957) == Cyc_Absyn_Evar: 0){ goto
_LL910;} else{ goto _LL911;} _LL911: _LL962: _temp961= _temp907.f1; if( _temp961
==( void*) Cyc_Absyn_VoidType){ goto _LL960;} else{ goto _LL913;} _LL960:
_temp959= _temp907.f2; if( _temp959 ==( void*) Cyc_Absyn_VoidType){ goto _LL912;}
else{ goto _LL913;} _LL913: _LL968: _temp967= _temp907.f1; if(( unsigned int)
_temp967 > 4u?*(( int*) _temp967) == Cyc_Absyn_VarType: 0){ _LL970: _temp969=((
struct Cyc_Absyn_VarType_struct*) _temp967)->f1; goto _LL964;} else{ goto _LL915;}
_LL964: _temp963= _temp907.f2; if(( unsigned int) _temp963 > 4u?*(( int*)
_temp963) == Cyc_Absyn_VarType: 0){ _LL966: _temp965=(( struct Cyc_Absyn_VarType_struct*)
_temp963)->f1; goto _LL914;} else{ goto _LL915;} _LL915: _LL980: _temp979=
_temp907.f1; if(( unsigned int) _temp979 > 4u?*(( int*) _temp979) == Cyc_Absyn_StructType:
0){ _LL986: _temp985=(( struct Cyc_Absyn_StructType_struct*) _temp979)->f1; goto
_LL984; _LL984: _temp983=(( struct Cyc_Absyn_StructType_struct*) _temp979)->f2;
goto _LL982; _LL982: _temp981=(( struct Cyc_Absyn_StructType_struct*) _temp979)->f3;
goto _LL972;} else{ goto _LL917;} _LL972: _temp971= _temp907.f2; if((
unsigned int) _temp971 > 4u?*(( int*) _temp971) == Cyc_Absyn_StructType: 0){
_LL978: _temp977=(( struct Cyc_Absyn_StructType_struct*) _temp971)->f1; goto
_LL976; _LL976: _temp975=(( struct Cyc_Absyn_StructType_struct*) _temp971)->f2;
goto _LL974; _LL974: _temp973=(( struct Cyc_Absyn_StructType_struct*) _temp971)->f3;
goto _LL916;} else{ goto _LL917;} _LL917: _LL992: _temp991= _temp907.f1; if((
unsigned int) _temp991 > 4u?*(( int*) _temp991) == Cyc_Absyn_EnumType: 0){
_LL994: _temp993=(( struct Cyc_Absyn_EnumType_struct*) _temp991)->f1; goto
_LL988;} else{ goto _LL919;} _LL988: _temp987= _temp907.f2; if(( unsigned int)
_temp987 > 4u?*(( int*) _temp987) == Cyc_Absyn_EnumType: 0){ _LL990: _temp989=((
struct Cyc_Absyn_EnumType_struct*) _temp987)->f1; goto _LL918;} else{ goto
_LL919;} _LL919: _LL1004: _temp1003= _temp907.f1; if(( unsigned int) _temp1003 >
4u?*(( int*) _temp1003) == Cyc_Absyn_UnionType: 0){ _LL1010: _temp1009=(( struct
Cyc_Absyn_UnionType_struct*) _temp1003)->f1; goto _LL1008; _LL1008: _temp1007=((
struct Cyc_Absyn_UnionType_struct*) _temp1003)->f2; goto _LL1006; _LL1006:
_temp1005=(( struct Cyc_Absyn_UnionType_struct*) _temp1003)->f3; goto _LL996;}
else{ goto _LL921;} _LL996: _temp995= _temp907.f2; if(( unsigned int) _temp995 >
4u?*(( int*) _temp995) == Cyc_Absyn_UnionType: 0){ _LL1002: _temp1001=(( struct
Cyc_Absyn_UnionType_struct*) _temp995)->f1; goto _LL1000; _LL1000: _temp999=((
struct Cyc_Absyn_UnionType_struct*) _temp995)->f2; goto _LL998; _LL998: _temp997=((
struct Cyc_Absyn_UnionType_struct*) _temp995)->f3; goto _LL920;} else{ goto
_LL921;} _LL921: _LL1024: _temp1023= _temp907.f1; if(( unsigned int) _temp1023 >
4u?*(( int*) _temp1023) == Cyc_Absyn_TunionType: 0){ _LL1026: _temp1025=((
struct Cyc_Absyn_TunionType_struct*) _temp1023)->f1; _LL1032: _temp1031=( void*)
_temp1025.tunion_info; if(*(( int*) _temp1031) == Cyc_Absyn_KnownTunion){
_LL1034: _temp1033=(( struct Cyc_Absyn_KnownTunion_struct*) _temp1031)->f1; goto
_LL1030;} else{ goto _LL923;} _LL1030: _temp1029= _temp1025.targs; goto _LL1028;
_LL1028: _temp1027=( void*) _temp1025.rgn; goto _LL1012;} else{ goto _LL923;}
_LL1012: _temp1011= _temp907.f2; if(( unsigned int) _temp1011 > 4u?*(( int*)
_temp1011) == Cyc_Absyn_TunionType: 0){ _LL1014: _temp1013=(( struct Cyc_Absyn_TunionType_struct*)
_temp1011)->f1; _LL1020: _temp1019=( void*) _temp1013.tunion_info; if(*(( int*)
_temp1019) == Cyc_Absyn_KnownTunion){ _LL1022: _temp1021=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1019)->f1; goto _LL1018;} else{ goto _LL923;} _LL1018: _temp1017= _temp1013.targs;
goto _LL1016; _LL1016: _temp1015=( void*) _temp1013.rgn; goto _LL922;} else{
goto _LL923;} _LL923: _LL1048: _temp1047= _temp907.f1; if(( unsigned int)
_temp1047 > 4u?*(( int*) _temp1047) == Cyc_Absyn_TunionFieldType: 0){ _LL1050:
_temp1049=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1047)->f1; _LL1054:
_temp1053=( void*) _temp1049.field_info; if(*(( int*) _temp1053) == Cyc_Absyn_KnownTunionfield){
_LL1058: _temp1057=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1053)->f1;
goto _LL1056; _LL1056: _temp1055=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1053)->f2; goto _LL1052;} else{ goto _LL925;} _LL1052: _temp1051= _temp1049.targs;
goto _LL1036;} else{ goto _LL925;} _LL1036: _temp1035= _temp907.f2; if((
unsigned int) _temp1035 > 4u?*(( int*) _temp1035) == Cyc_Absyn_TunionFieldType:
0){ _LL1038: _temp1037=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1035)->f1;
_LL1042: _temp1041=( void*) _temp1037.field_info; if(*(( int*) _temp1041) == Cyc_Absyn_KnownTunionfield){
_LL1046: _temp1045=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1041)->f1;
goto _LL1044; _LL1044: _temp1043=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1041)->f2; goto _LL1040;} else{ goto _LL925;} _LL1040: _temp1039= _temp1037.targs;
goto _LL924;} else{ goto _LL925;} _LL925: _LL1074: _temp1073= _temp907.f1; if((
unsigned int) _temp1073 > 4u?*(( int*) _temp1073) == Cyc_Absyn_PointerType: 0){
_LL1076: _temp1075=(( struct Cyc_Absyn_PointerType_struct*) _temp1073)->f1;
_LL1086: _temp1085=( void*) _temp1075.elt_typ; goto _LL1084; _LL1084: _temp1083=(
void*) _temp1075.rgn_typ; goto _LL1082; _LL1082: _temp1081= _temp1075.nullable;
goto _LL1080; _LL1080: _temp1079= _temp1075.tq; goto _LL1078; _LL1078: _temp1077=
_temp1075.bounds; goto _LL1060;} else{ goto _LL927;} _LL1060: _temp1059=
_temp907.f2; if(( unsigned int) _temp1059 > 4u?*(( int*) _temp1059) == Cyc_Absyn_PointerType:
0){ _LL1062: _temp1061=(( struct Cyc_Absyn_PointerType_struct*) _temp1059)->f1;
_LL1072: _temp1071=( void*) _temp1061.elt_typ; goto _LL1070; _LL1070: _temp1069=(
void*) _temp1061.rgn_typ; goto _LL1068; _LL1068: _temp1067= _temp1061.nullable;
goto _LL1066; _LL1066: _temp1065= _temp1061.tq; goto _LL1064; _LL1064: _temp1063=
_temp1061.bounds; goto _LL926;} else{ goto _LL927;} _LL927: _LL1094: _temp1093=
_temp907.f1; if(( unsigned int) _temp1093 > 4u?*(( int*) _temp1093) == Cyc_Absyn_IntType:
0){ _LL1098: _temp1097=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1093)->f1;
goto _LL1096; _LL1096: _temp1095=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1093)->f2; goto _LL1088;} else{ goto _LL929;} _LL1088: _temp1087= _temp907.f2;
if(( unsigned int) _temp1087 > 4u?*(( int*) _temp1087) == Cyc_Absyn_IntType: 0){
_LL1092: _temp1091=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1087)->f1;
goto _LL1090; _LL1090: _temp1089=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1087)->f2; goto _LL928;} else{ goto _LL929;} _LL929: _LL1102: _temp1101=
_temp907.f1; if( _temp1101 ==( void*) Cyc_Absyn_FloatType){ goto _LL1100;} else{
goto _LL931;} _LL1100: _temp1099= _temp907.f2; if( _temp1099 ==( void*) Cyc_Absyn_FloatType){
goto _LL930;} else{ goto _LL931;} _LL931: _LL1106: _temp1105= _temp907.f1; if(
_temp1105 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1104;} else{ goto _LL933;}
_LL1104: _temp1103= _temp907.f2; if( _temp1103 ==( void*) Cyc_Absyn_DoubleType){
goto _LL932;} else{ goto _LL933;} _LL933: _LL1116: _temp1115= _temp907.f1; if((
unsigned int) _temp1115 > 4u?*(( int*) _temp1115) == Cyc_Absyn_ArrayType: 0){
_LL1122: _temp1121=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1115)->f1;
goto _LL1120; _LL1120: _temp1119=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1115)->f2; goto _LL1118; _LL1118: _temp1117=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1115)->f3; goto _LL1108;} else{ goto _LL935;} _LL1108: _temp1107= _temp907.f2;
if(( unsigned int) _temp1107 > 4u?*(( int*) _temp1107) == Cyc_Absyn_ArrayType: 0){
_LL1114: _temp1113=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1107)->f1;
goto _LL1112; _LL1112: _temp1111=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1107)->f2; goto _LL1110; _LL1110: _temp1109=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1107)->f3; goto _LL934;} else{ goto _LL935;} _LL935: _LL1144: _temp1143=
_temp907.f1; if(( unsigned int) _temp1143 > 4u?*(( int*) _temp1143) == Cyc_Absyn_FnType:
0){ _LL1146: _temp1145=(( struct Cyc_Absyn_FnType_struct*) _temp1143)->f1;
_LL1162: _temp1161= _temp1145.tvars; goto _LL1160; _LL1160: _temp1159= _temp1145.effect;
goto _LL1158; _LL1158: _temp1157=( void*) _temp1145.ret_typ; goto _LL1156;
_LL1156: _temp1155= _temp1145.args; goto _LL1154; _LL1154: _temp1153= _temp1145.c_varargs;
goto _LL1152; _LL1152: _temp1151= _temp1145.cyc_varargs; goto _LL1150; _LL1150:
_temp1149= _temp1145.rgn_po; goto _LL1148; _LL1148: _temp1147= _temp1145.attributes;
goto _LL1124;} else{ goto _LL937;} _LL1124: _temp1123= _temp907.f2; if((
unsigned int) _temp1123 > 4u?*(( int*) _temp1123) == Cyc_Absyn_FnType: 0){
_LL1126: _temp1125=(( struct Cyc_Absyn_FnType_struct*) _temp1123)->f1; _LL1142:
_temp1141= _temp1125.tvars; goto _LL1140; _LL1140: _temp1139= _temp1125.effect;
goto _LL1138; _LL1138: _temp1137=( void*) _temp1125.ret_typ; goto _LL1136;
_LL1136: _temp1135= _temp1125.args; goto _LL1134; _LL1134: _temp1133= _temp1125.c_varargs;
goto _LL1132; _LL1132: _temp1131= _temp1125.cyc_varargs; goto _LL1130; _LL1130:
_temp1129= _temp1125.rgn_po; goto _LL1128; _LL1128: _temp1127= _temp1125.attributes;
goto _LL936;} else{ goto _LL937;} _LL937: _LL1168: _temp1167= _temp907.f1; if((
unsigned int) _temp1167 > 4u?*(( int*) _temp1167) == Cyc_Absyn_TupleType: 0){
_LL1170: _temp1169=(( struct Cyc_Absyn_TupleType_struct*) _temp1167)->f1; goto
_LL1164;} else{ goto _LL939;} _LL1164: _temp1163= _temp907.f2; if(( unsigned int)
_temp1163 > 4u?*(( int*) _temp1163) == Cyc_Absyn_TupleType: 0){ _LL1166:
_temp1165=(( struct Cyc_Absyn_TupleType_struct*) _temp1163)->f1; goto _LL938;}
else{ goto _LL939;} _LL939: _LL1176: _temp1175= _temp907.f1; if(( unsigned int)
_temp1175 > 4u?*(( int*) _temp1175) == Cyc_Absyn_AnonStructType: 0){ _LL1178:
_temp1177=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1175)->f1; goto
_LL1172;} else{ goto _LL941;} _LL1172: _temp1171= _temp907.f2; if(( unsigned int)
_temp1171 > 4u?*(( int*) _temp1171) == Cyc_Absyn_AnonStructType: 0){ _LL1174:
_temp1173=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1171)->f1; goto
_LL940;} else{ goto _LL941;} _LL941: _LL1184: _temp1183= _temp907.f1; if((
unsigned int) _temp1183 > 4u?*(( int*) _temp1183) == Cyc_Absyn_AnonUnionType: 0){
_LL1186: _temp1185=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1183)->f1;
goto _LL1180;} else{ goto _LL943;} _LL1180: _temp1179= _temp907.f2; if((
unsigned int) _temp1179 > 4u?*(( int*) _temp1179) == Cyc_Absyn_AnonUnionType: 0){
_LL1182: _temp1181=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1179)->f1;
goto _LL942;} else{ goto _LL943;} _LL943: _LL1190: _temp1189= _temp907.f1; if(
_temp1189 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1188;} else{ goto _LL945;}
_LL1188: _temp1187= _temp907.f2; if( _temp1187 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL944;} else{ goto _LL945;} _LL945: _LL1196: _temp1195= _temp907.f1; if((
unsigned int) _temp1195 > 4u?*(( int*) _temp1195) == Cyc_Absyn_RgnHandleType: 0){
_LL1198: _temp1197=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1195)->f1;
goto _LL1192;} else{ goto _LL947;} _LL1192: _temp1191= _temp907.f2; if((
unsigned int) _temp1191 > 4u?*(( int*) _temp1191) == Cyc_Absyn_RgnHandleType: 0){
_LL1194: _temp1193=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1191)->f1;
goto _LL946;} else{ goto _LL947;} _LL947: _LL1200: _temp1199= _temp907.f1; if((
unsigned int) _temp1199 > 4u?*(( int*) _temp1199) == Cyc_Absyn_JoinEff: 0){ goto
_LL948;} else{ goto _LL949;} _LL949: _LL1202: _temp1201= _temp907.f2; if((
unsigned int) _temp1201 > 4u?*(( int*) _temp1201) == Cyc_Absyn_JoinEff: 0){ goto
_LL950;} else{ goto _LL951;} _LL951: _LL1204: _temp1203= _temp907.f1; if((
unsigned int) _temp1203 > 4u?*(( int*) _temp1203) == Cyc_Absyn_AccessEff: 0){
goto _LL952;} else{ goto _LL953;} _LL953: _LL1206: _temp1205= _temp907.f2; if((
unsigned int) _temp1205 > 4u?*(( int*) _temp1205) == Cyc_Absyn_AccessEff: 0){
goto _LL954;} else{ goto _LL955;} _LL955: goto _LL956; _LL910: Cyc_Tcutil_unify_it(
t2, t1); return; _LL912: return; _LL914: { struct _tagged_arr* _temp1207=
_temp969->name; struct _tagged_arr* _temp1208= _temp965->name; int _temp1209=*((
int*) _check_null( _temp969->identity)); int _temp1210=*(( int*) _check_null(
_temp965->identity)); void* _temp1211= Cyc_Absyn_conref_val( _temp969->kind);
void* _temp1212= Cyc_Absyn_conref_val( _temp965->kind); if( _temp1210 ==
_temp1209? Cyc_String_zstrptrcmp( _temp1207, _temp1208) == 0: 0){ if( _temp1211
!= _temp1212){({ struct Cyc_Stdio_String_pa_struct _temp1216; _temp1216.tag= Cyc_Stdio_String_pa;
_temp1216.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( _temp1212);{ struct
Cyc_Stdio_String_pa_struct _temp1215; _temp1215.tag= Cyc_Stdio_String_pa;
_temp1215.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( _temp1211);{ struct
Cyc_Stdio_String_pa_struct _temp1214; _temp1214.tag= Cyc_Stdio_String_pa;
_temp1214.f1=( struct _tagged_arr)* _temp1207;{ void* _temp1213[ 3u]={&
_temp1214,& _temp1215,& _temp1216};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("same type variable %s has kinds %s and %s",
sizeof( unsigned char), 42u), _tag_arr( _temp1213, sizeof( void*), 3u));}}}});}
return;} goto _LL908;} _LL916: if((( _temp977 != 0? _temp985 != 0: 0)? Cyc_Absyn_qvar_cmp((
struct _tuple1*) _check_null( _temp977),( struct _tuple1*) _check_null( _temp985))
== 0: 0)? 1:( _temp977 == 0? _temp985 == 0: 0)){ Cyc_Tcutil_unify_list( _temp975,
_temp983); return;} goto _LL908; _LL918: if( Cyc_Absyn_qvar_cmp( _temp993,
_temp989) == 0){ return;} goto _LL908; _LL920: if((( _temp1001 != 0? _temp1009
!= 0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*) _check_null( _temp1001),( struct
_tuple1*) _check_null( _temp1009)) == 0: 0)? 1:( _temp1001 == 0? _temp1009 == 0:
0)){ Cyc_Tcutil_unify_list( _temp999, _temp1007); return;} goto _LL908; _LL922:
if( _temp1033 == _temp1021? 1: Cyc_Absyn_qvar_cmp( _temp1033->name, _temp1021->name)
== 0){ Cyc_Tcutil_unify_it( _temp1015, _temp1027); Cyc_Tcutil_unify_list(
_temp1017, _temp1029); return;} goto _LL908; _LL924: if(( _temp1057 == _temp1045?
1: Cyc_Absyn_qvar_cmp( _temp1057->name, _temp1045->name) == 0)? _temp1055 ==
_temp1043? 1: Cyc_Absyn_qvar_cmp( _temp1055->name, _temp1043->name) == 0: 0){
Cyc_Tcutil_unify_list( _temp1039, _temp1051); return;} goto _LL908; _LL926: Cyc_Tcutil_unify_it(
_temp1071, _temp1085); Cyc_Tcutil_unify_it( _temp1083, _temp1069); Cyc_Tcutil_unify_tqual(
_temp1065, _temp1079); Cyc_Tcutil_unify_it_conrefs( Cyc_Tcutil_boundscmp,
_temp1063, _temp1077);{ void* _temp1217=( void*)( Cyc_Absyn_compress_conref(
_temp1063))->v; void* _temp1223; _LL1219: if(( unsigned int) _temp1217 > 1u?*((
int*) _temp1217) == Cyc_Absyn_Eq_constr: 0){ _LL1224: _temp1223=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1217)->f1; if( _temp1223 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1220;} else{ goto _LL1221;}} else{ goto _LL1221;} _LL1221: goto _LL1222;
_LL1220: return; _LL1222: goto _LL1218; _LL1218:;}(( void(*)( int(* cmp)( int,
int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp, _temp1067, _temp1081); return; _LL928: if( _temp1091 ==
_temp1097? _temp1089 == _temp1095: 0){ return;} goto _LL908; _LL930: return;
_LL932: return; _LL934: Cyc_Tcutil_unify_it( _temp1113, _temp1121); Cyc_Tcutil_unify_tqual(
_temp1111, _temp1119); if( _temp1117 == _temp1109){ return;} if( _temp1117 == 0?
1: _temp1109 == 0){ goto _LL908;} if( Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1117)) == Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1109))){ return;} goto _LL908; _LL936: { int done= 0;{ struct
_RegionHandle _temp1225= _new_region(); struct _RegionHandle* rgn=& _temp1225;
_push_region( rgn);{ struct Cyc_List_List* inst= 0; while( _temp1141 != 0) { if(
_temp1161 == 0){ break;} inst=({ struct Cyc_List_List* _temp1226=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1226->hd=( void*)({
struct _tuple5* _temp1227=( struct _tuple5*) _region_malloc( rgn, sizeof( struct
_tuple5)); _temp1227->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp1161))->hd; _temp1227->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1228=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1228[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1229; _temp1229.tag= Cyc_Absyn_VarType;
_temp1229.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp1141))->hd; _temp1229;}); _temp1228;}); _temp1227;}); _temp1226->tl= inst;
_temp1226;}); _temp1141=(( struct Cyc_List_List*) _check_null( _temp1141))->tl;
_temp1161=(( struct Cyc_List_List*) _check_null( _temp1161))->tl;} if( _temp1161
!= 0){ _npop_handler( 0u); goto _LL908;} if( inst != 0){ Cyc_Tcutil_unify_it((
void*)({ struct Cyc_Absyn_FnType_struct* _temp1230=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1230[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1231; _temp1231.tag= Cyc_Absyn_FnType; _temp1231.f1=({ struct Cyc_Absyn_FnInfo
_temp1232; _temp1232.tvars= 0; _temp1232.effect= _temp1139; _temp1232.ret_typ=(
void*) _temp1137; _temp1232.args= _temp1135; _temp1232.c_varargs= _temp1133;
_temp1232.cyc_varargs= _temp1131; _temp1232.rgn_po= _temp1129; _temp1232.attributes=
_temp1127; _temp1232;}); _temp1231;}); _temp1230;}), Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1233=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1233[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1234; _temp1234.tag= Cyc_Absyn_FnType; _temp1234.f1=({ struct Cyc_Absyn_FnInfo
_temp1235; _temp1235.tvars= 0; _temp1235.effect= _temp1159; _temp1235.ret_typ=(
void*) _temp1157; _temp1235.args= _temp1155; _temp1235.c_varargs= _temp1153;
_temp1235.cyc_varargs= _temp1151; _temp1235.rgn_po= _temp1149; _temp1235.attributes=
_temp1147; _temp1235;}); _temp1234;}); _temp1233;}))); done= 1;}}; _pop_region(
rgn);} if( done){ return;} Cyc_Tcutil_unify_it( _temp1137, _temp1157); for( 0;
_temp1135 != 0? _temp1155 != 0: 0;( _temp1135=(( struct Cyc_List_List*)
_check_null( _temp1135))->tl, _temp1155=(( struct Cyc_List_List*) _check_null(
_temp1155))->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp1135))->hd)).f2,(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp1155))->hd)).f2); Cyc_Tcutil_unify_it((*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp1135))->hd)).f3,(*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp1155))->hd)).f3);} if( _temp1135 != 0?
1: _temp1155 != 0){ goto _LL908;} if( _temp1133 != _temp1153){ goto _LL908;}{
int bad_cyc_vararg= 0;{ struct _tuple11 _temp1237=({ struct _tuple11 _temp1236;
_temp1236.f1= _temp1131; _temp1236.f2= _temp1151; _temp1236;}); struct Cyc_Absyn_VarargInfo*
_temp1247; struct Cyc_Absyn_VarargInfo* _temp1249; struct Cyc_Absyn_VarargInfo*
_temp1251; struct Cyc_Absyn_VarargInfo* _temp1253; struct Cyc_Absyn_VarargInfo*
_temp1255; struct Cyc_Absyn_VarargInfo _temp1257; int _temp1258; void* _temp1260;
void* _temp1262; struct Cyc_Absyn_Tqual _temp1264; struct Cyc_Core_Opt*
_temp1266; struct Cyc_Absyn_VarargInfo* _temp1268; struct Cyc_Absyn_VarargInfo
_temp1270; int _temp1271; void* _temp1273; void* _temp1275; struct Cyc_Absyn_Tqual
_temp1277; struct Cyc_Core_Opt* _temp1279; _LL1239: _LL1250: _temp1249=
_temp1237.f1; if( _temp1249 == 0){ goto _LL1248;} else{ goto _LL1241;} _LL1248:
_temp1247= _temp1237.f2; if( _temp1247 == 0){ goto _LL1240;} else{ goto _LL1241;}
_LL1241: _LL1252: _temp1251= _temp1237.f1; if( _temp1251 == 0){ goto _LL1242;}
else{ goto _LL1243;} _LL1243: _LL1254: _temp1253= _temp1237.f2; if( _temp1253 ==
0){ goto _LL1244;} else{ goto _LL1245;} _LL1245: _LL1269: _temp1268= _temp1237.f1;
if( _temp1268 == 0){ goto _LL1238;} else{ _temp1270=* _temp1268; _LL1280:
_temp1279= _temp1270.name; goto _LL1278; _LL1278: _temp1277= _temp1270.tq; goto
_LL1276; _LL1276: _temp1275=( void*) _temp1270.type; goto _LL1274; _LL1274:
_temp1273=( void*) _temp1270.rgn; goto _LL1272; _LL1272: _temp1271= _temp1270.inject;
goto _LL1256;} _LL1256: _temp1255= _temp1237.f2; if( _temp1255 == 0){ goto
_LL1238;} else{ _temp1257=* _temp1255; _LL1267: _temp1266= _temp1257.name; goto
_LL1265; _LL1265: _temp1264= _temp1257.tq; goto _LL1263; _LL1263: _temp1262=(
void*) _temp1257.type; goto _LL1261; _LL1261: _temp1260=( void*) _temp1257.rgn;
goto _LL1259; _LL1259: _temp1258= _temp1257.inject; goto _LL1246;} _LL1240: goto
_LL1238; _LL1242: goto _LL1244; _LL1244: bad_cyc_vararg= 1; goto _LL1238;
_LL1246: Cyc_Tcutil_unify_tqual( _temp1277, _temp1264); Cyc_Tcutil_unify_it(
_temp1275, _temp1262); Cyc_Tcutil_unify_it( _temp1273, _temp1260); if( _temp1271
!= _temp1258){ bad_cyc_vararg= 1;} goto _LL1238; _LL1238:;} if( bad_cyc_vararg){
goto _LL908;}{ int bad_effect= 0;{ struct _tuple12 _temp1282=({ struct _tuple12
_temp1281; _temp1281.f1= _temp1139; _temp1281.f2= _temp1159; _temp1281;});
struct Cyc_Core_Opt* _temp1292; struct Cyc_Core_Opt* _temp1294; struct Cyc_Core_Opt*
_temp1296; struct Cyc_Core_Opt* _temp1298; _LL1284: _LL1295: _temp1294=
_temp1282.f1; if( _temp1294 == 0){ goto _LL1293;} else{ goto _LL1286;} _LL1293:
_temp1292= _temp1282.f2; if( _temp1292 == 0){ goto _LL1285;} else{ goto _LL1286;}
_LL1286: _LL1297: _temp1296= _temp1282.f1; if( _temp1296 == 0){ goto _LL1287;}
else{ goto _LL1288;} _LL1288: _LL1299: _temp1298= _temp1282.f2; if( _temp1298 ==
0){ goto _LL1289;} else{ goto _LL1290;} _LL1290: goto _LL1291; _LL1285: goto
_LL1283; _LL1287: goto _LL1289; _LL1289: bad_effect= 1; goto _LL1283; _LL1291:
bad_effect= ! Cyc_Tcutil_unify_effect(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1139))->v,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1159))->v); goto _LL1283; _LL1283:;} if( bad_effect){ goto _LL908;} if( !
Cyc_Tcutil_same_atts( _temp1147, _temp1127)){ goto _LL908;} if( ! Cyc_Tcutil_same_rgn_po(
_temp1149, _temp1129)){ goto _LL908;} return;}}} _LL938: for( 0; _temp1165 != 0?
_temp1169 != 0: 0;( _temp1165=(( struct Cyc_List_List*) _check_null( _temp1165))->tl,
_temp1169=(( struct Cyc_List_List*) _check_null( _temp1169))->tl)){ Cyc_Tcutil_unify_tqual((*((
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1165))->hd)).f1,(*((
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1169))->hd)).f1);
Cyc_Tcutil_unify_it((*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp1165))->hd)).f2,(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp1169))->hd)).f2);} if( _temp1165 == 0? _temp1169 == 0: 0){ return;} goto
_LL908; _LL940: _temp1185= _temp1177; _temp1181= _temp1173; goto _LL942; _LL942:
for( 0; _temp1181 != 0? _temp1185 != 0: 0;( _temp1181=(( struct Cyc_List_List*)
_check_null( _temp1181))->tl, _temp1185=(( struct Cyc_List_List*) _check_null(
_temp1185))->tl)){ struct Cyc_Absyn_Structfield* _temp1300=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp1181))->hd; struct Cyc_Absyn_Structfield*
_temp1301=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1185))->hd; if( Cyc_String_zstrptrcmp( _temp1300->name, _temp1301->name) !=
0){( int) _throw(( void*) Cyc_Tcutil_Unify);} Cyc_Tcutil_unify_tqual( _temp1300->tq,
_temp1301->tq); Cyc_Tcutil_unify_it(( void*) _temp1300->type,( void*) _temp1301->type);
if((( ! Cyc_Tcutil_same_atts( _temp1300->attributes, _temp1301->attributes)? 1:(
_temp1300->width != 0? _temp1301->width == 0: 0))? 1:( _temp1301->width != 0?
_temp1300->width == 0: 0))? 1:(( _temp1300->width != 0? _temp1301->width != 0: 0)?
Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*)
_check_null( _temp1300->width))->v) != Cyc_Evexp_eval_const_uint_exp(( struct
Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp1301->width))->v): 0)){(
int) _throw(( void*) Cyc_Tcutil_Unify);}} if( _temp1181 == 0? _temp1185 == 0: 0){
return;} goto _LL908; _LL944: return; _LL946: Cyc_Tcutil_unify_it( _temp1197,
_temp1193); return; _LL948: goto _LL950; _LL950: goto _LL952; _LL952: goto
_LL954; _LL954: if( Cyc_Tcutil_unify_effect( t1, t2)){ return;} else{ goto
_LL908;} _LL956: goto _LL908; _LL908:;}( int) _throw(( void*) Cyc_Tcutil_Unify);}
int Cyc_Tcutil_is_arithmetic_type( void* t){ void* _temp1302= Cyc_Tcutil_compress(
t); _LL1304: if(( unsigned int) _temp1302 > 4u?*(( int*) _temp1302) == Cyc_Absyn_IntType:
0){ goto _LL1305;} else{ goto _LL1306;} _LL1306: if( _temp1302 ==( void*) Cyc_Absyn_FloatType){
goto _LL1307;} else{ goto _LL1308;} _LL1308: if( _temp1302 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1309;} else{ goto _LL1310;} _LL1310: if(( unsigned int) _temp1302 > 4u?*((
int*) _temp1302) == Cyc_Absyn_EnumType: 0){ goto _LL1311;} else{ goto _LL1312;}
_LL1312: goto _LL1313; _LL1305: return 1; _LL1307: return 1; _LL1309: return 1;
_LL1311: return 1; _LL1313: return 0; _LL1303:;} int Cyc_Tcutil_will_lose_precision(
void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple8 _temp1315=({ struct _tuple8 _temp1314; _temp1314.f1= t1;
_temp1314.f2= t2; _temp1314;}); void* _temp1337; void* _temp1339; void*
_temp1341; void* _temp1343; void* _temp1345; void* _temp1347; void* _temp1349;
void* _temp1351; void* _temp1353; void* _temp1355; void* _temp1357; void*
_temp1359; void* _temp1361; void* _temp1363; void* _temp1365; void* _temp1367;
void* _temp1369; void* _temp1371; void* _temp1373; void* _temp1375; void*
_temp1377; void* _temp1379; void* _temp1381; void* _temp1383; void* _temp1385;
void* _temp1387; void* _temp1389; _LL1317: _LL1340: _temp1339= _temp1315.f1; if(
_temp1339 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1338;} else{ goto _LL1319;}
_LL1338: _temp1337= _temp1315.f2; if( _temp1337 ==( void*) Cyc_Absyn_FloatType){
goto _LL1318;} else{ goto _LL1319;} _LL1319: _LL1344: _temp1343= _temp1315.f1;
if( _temp1343 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1342;} else{ goto
_LL1321;} _LL1342: _temp1341= _temp1315.f2; if(( unsigned int) _temp1341 > 4u?*((
int*) _temp1341) == Cyc_Absyn_IntType: 0){ goto _LL1320;} else{ goto _LL1321;}
_LL1321: _LL1348: _temp1347= _temp1315.f1; if( _temp1347 ==( void*) Cyc_Absyn_FloatType){
goto _LL1346;} else{ goto _LL1323;} _LL1346: _temp1345= _temp1315.f2; if((
unsigned int) _temp1345 > 4u?*(( int*) _temp1345) == Cyc_Absyn_IntType: 0){ goto
_LL1322;} else{ goto _LL1323;} _LL1323: _LL1354: _temp1353= _temp1315.f1; if((
unsigned int) _temp1353 > 4u?*(( int*) _temp1353) == Cyc_Absyn_IntType: 0){
_LL1356: _temp1355=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1353)->f2;
if( _temp1355 ==( void*) Cyc_Absyn_B8){ goto _LL1350;} else{ goto _LL1325;}}
else{ goto _LL1325;} _LL1350: _temp1349= _temp1315.f2; if(( unsigned int)
_temp1349 > 4u?*(( int*) _temp1349) == Cyc_Absyn_IntType: 0){ _LL1352: _temp1351=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1349)->f2; if( _temp1351 ==(
void*) Cyc_Absyn_B8){ goto _LL1324;} else{ goto _LL1325;}} else{ goto _LL1325;}
_LL1325: _LL1358: _temp1357= _temp1315.f1; if(( unsigned int) _temp1357 > 4u?*((
int*) _temp1357) == Cyc_Absyn_IntType: 0){ _LL1360: _temp1359=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1357)->f2; if( _temp1359 ==( void*) Cyc_Absyn_B8){
goto _LL1326;} else{ goto _LL1327;}} else{ goto _LL1327;} _LL1327: _LL1364:
_temp1363= _temp1315.f1; if(( unsigned int) _temp1363 > 4u?*(( int*) _temp1363)
== Cyc_Absyn_IntType: 0){ _LL1366: _temp1365=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1363)->f2; if( _temp1365 ==( void*) Cyc_Absyn_B4){ goto _LL1362;} else{
goto _LL1329;}} else{ goto _LL1329;} _LL1362: _temp1361= _temp1315.f2; if(
_temp1361 ==( void*) Cyc_Absyn_FloatType){ goto _LL1328;} else{ goto _LL1329;}
_LL1329: _LL1372: _temp1371= _temp1315.f1; if(( unsigned int) _temp1371 > 4u?*((
int*) _temp1371) == Cyc_Absyn_IntType: 0){ _LL1374: _temp1373=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1371)->f2; if( _temp1373 ==( void*) Cyc_Absyn_B4){
goto _LL1368;} else{ goto _LL1331;}} else{ goto _LL1331;} _LL1368: _temp1367=
_temp1315.f2; if(( unsigned int) _temp1367 > 4u?*(( int*) _temp1367) == Cyc_Absyn_IntType:
0){ _LL1370: _temp1369=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1367)->f2;
if( _temp1369 ==( void*) Cyc_Absyn_B2){ goto _LL1330;} else{ goto _LL1331;}}
else{ goto _LL1331;} _LL1331: _LL1380: _temp1379= _temp1315.f1; if((
unsigned int) _temp1379 > 4u?*(( int*) _temp1379) == Cyc_Absyn_IntType: 0){
_LL1382: _temp1381=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1379)->f2;
if( _temp1381 ==( void*) Cyc_Absyn_B4){ goto _LL1376;} else{ goto _LL1333;}}
else{ goto _LL1333;} _LL1376: _temp1375= _temp1315.f2; if(( unsigned int)
_temp1375 > 4u?*(( int*) _temp1375) == Cyc_Absyn_IntType: 0){ _LL1378: _temp1377=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1375)->f2; if( _temp1377 ==(
void*) Cyc_Absyn_B1){ goto _LL1332;} else{ goto _LL1333;}} else{ goto _LL1333;}
_LL1333: _LL1388: _temp1387= _temp1315.f1; if(( unsigned int) _temp1387 > 4u?*((
int*) _temp1387) == Cyc_Absyn_IntType: 0){ _LL1390: _temp1389=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1387)->f2; if( _temp1389 ==( void*) Cyc_Absyn_B2){
goto _LL1384;} else{ goto _LL1335;}} else{ goto _LL1335;} _LL1384: _temp1383=
_temp1315.f2; if(( unsigned int) _temp1383 > 4u?*(( int*) _temp1383) == Cyc_Absyn_IntType:
0){ _LL1386: _temp1385=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1383)->f2;
if( _temp1385 ==( void*) Cyc_Absyn_B1){ goto _LL1334;} else{ goto _LL1335;}}
else{ goto _LL1335;} _LL1335: goto _LL1336; _LL1318: return 1; _LL1320: return 1;
_LL1322: return 1; _LL1324: return 0; _LL1326: return 1; _LL1328: return 1;
_LL1330: return 1; _LL1332: return 1; _LL1334: return 1; _LL1336: return 0;
_LL1316:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv* te, void* t,
struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{ struct Cyc_List_List*
el= es; for( 0; el != 0; el=(( struct Cyc_List_List*) _check_null( el))->tl){
void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);
if( Cyc_Tcutil_is_arithmetic_type( t1)){ if( max_arith_type == 0? 1: Cyc_Tcutil_will_lose_precision(
t1,( void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){
max_arith_type=({ struct Cyc_Core_Opt* _temp1391=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1391->v=( void*) t1; _temp1391;});}}}}
if( max_arith_type != 0){ if( ! Cyc_Tcutil_unify( t,( void*)(( struct Cyc_Core_Opt*)
_check_null( max_arith_type))->v)){ return 0;}}{ struct Cyc_List_List* el= es;
for( 0; el != 0; el=(( struct Cyc_List_List*) _check_null( el))->tl){ if( ! Cyc_Tcutil_coerce_assign(
te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd, t)){({
struct Cyc_Stdio_String_pa_struct _temp1394; _temp1394.tag= Cyc_Stdio_String_pa;
_temp1394.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);{
struct Cyc_Stdio_String_pa_struct _temp1393; _temp1393.tag= Cyc_Stdio_String_pa;
_temp1393.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1392[
2u]={& _temp1393,& _temp1394}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( el))->hd)->loc, _tag_arr("type mismatch: expecting %s but found %s",
sizeof( unsigned char), 41u), _tag_arr( _temp1392, sizeof( void*), 2u));}}});
return 0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ( te, e)){ void*
_temp1395= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_LL1397: if(( unsigned int) _temp1395 > 4u?*(( int*) _temp1395) == Cyc_Absyn_PointerType:
0){ goto _LL1398;} else{ goto _LL1399;} _LL1399: goto _LL1400; _LL1398: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); goto _LL1396; _LL1400: return 0; _LL1396:;} return 1;}
int Cyc_Tcutil_is_integral_type( void* t){ void* _temp1401= Cyc_Tcutil_compress(
t); _LL1403: if(( unsigned int) _temp1401 > 4u?*(( int*) _temp1401) == Cyc_Absyn_IntType:
0){ goto _LL1404;} else{ goto _LL1405;} _LL1405: if(( unsigned int) _temp1401 >
4u?*(( int*) _temp1401) == Cyc_Absyn_EnumType: 0){ goto _LL1406;} else{ goto
_LL1407;} _LL1407: goto _LL1408; _LL1404: return 1; _LL1406: return 1; _LL1408:
return 0; _LL1402:;} int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_uint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){ if( Cyc_Tcutil_will_lose_precision((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_uint_t)){({
void* _temp1409[ 0u]={}; Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u), _tag_arr( _temp1409, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;}
if( Cyc_Tcutil_is_integral_type(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){
if( Cyc_Tcutil_will_lose_precision(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, Cyc_Absyn_sint_t)){({ void* _temp1410[ 0u]={}; Cyc_Tcutil_warn( e->loc,
_tag_arr("integral size mismatch; conversion supplied", sizeof( unsigned char),
44u), _tag_arr( _temp1410, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); return 1;} return 0;} int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8
_temp1412=({ struct _tuple8 _temp1411; _temp1411.f1= t1; _temp1411.f2= t2;
_temp1411;}); void* _temp1424; struct Cyc_Absyn_PtrInfo _temp1426; void*
_temp1428; struct Cyc_Absyn_PtrInfo _temp1430; void* _temp1432; struct Cyc_Absyn_Exp*
_temp1434; struct Cyc_Absyn_Tqual _temp1436; void* _temp1438; void* _temp1440;
struct Cyc_Absyn_Exp* _temp1442; struct Cyc_Absyn_Tqual _temp1444; void*
_temp1446; void* _temp1448; struct Cyc_Absyn_TunionInfo _temp1450; struct Cyc_List_List*
_temp1452; void* _temp1454; struct Cyc_Absyn_Tuniondecl* _temp1456; void*
_temp1458; struct Cyc_Absyn_TunionFieldInfo _temp1460; struct Cyc_List_List*
_temp1462; void* _temp1464; struct Cyc_Absyn_Tunionfield* _temp1466; struct Cyc_Absyn_Tuniondecl*
_temp1468; void* _temp1470; struct Cyc_Absyn_TunionInfo _temp1472; void*
_temp1474; struct Cyc_List_List* _temp1476; void* _temp1478; struct Cyc_Absyn_Tuniondecl*
_temp1480; void* _temp1482; struct Cyc_Absyn_PtrInfo _temp1484; struct Cyc_Absyn_Conref*
_temp1486; struct Cyc_Absyn_Tqual _temp1488; struct Cyc_Absyn_Conref* _temp1490;
void* _temp1492; void* _temp1494; _LL1414: _LL1429: _temp1428= _temp1412.f1; if((
unsigned int) _temp1428 > 4u?*(( int*) _temp1428) == Cyc_Absyn_PointerType: 0){
_LL1431: _temp1430=(( struct Cyc_Absyn_PointerType_struct*) _temp1428)->f1; goto
_LL1425;} else{ goto _LL1416;} _LL1425: _temp1424= _temp1412.f2; if((
unsigned int) _temp1424 > 4u?*(( int*) _temp1424) == Cyc_Absyn_PointerType: 0){
_LL1427: _temp1426=(( struct Cyc_Absyn_PointerType_struct*) _temp1424)->f1; goto
_LL1415;} else{ goto _LL1416;} _LL1416: _LL1441: _temp1440= _temp1412.f1; if((
unsigned int) _temp1440 > 4u?*(( int*) _temp1440) == Cyc_Absyn_ArrayType: 0){
_LL1447: _temp1446=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1440)->f1;
goto _LL1445; _LL1445: _temp1444=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1440)->f2; goto _LL1443; _LL1443: _temp1442=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1440)->f3; goto _LL1433;} else{ goto _LL1418;} _LL1433: _temp1432=
_temp1412.f2; if(( unsigned int) _temp1432 > 4u?*(( int*) _temp1432) == Cyc_Absyn_ArrayType:
0){ _LL1439: _temp1438=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1432)->f1;
goto _LL1437; _LL1437: _temp1436=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1432)->f2; goto _LL1435; _LL1435: _temp1434=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1432)->f3; goto _LL1417;} else{ goto _LL1418;} _LL1418: _LL1459: _temp1458=
_temp1412.f1; if(( unsigned int) _temp1458 > 4u?*(( int*) _temp1458) == Cyc_Absyn_TunionFieldType:
0){ _LL1461: _temp1460=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1458)->f1;
_LL1465: _temp1464=( void*) _temp1460.field_info; if(*(( int*) _temp1464) == Cyc_Absyn_KnownTunionfield){
_LL1469: _temp1468=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1464)->f1;
goto _LL1467; _LL1467: _temp1466=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1464)->f2; goto _LL1463;} else{ goto _LL1420;} _LL1463: _temp1462=
_temp1460.targs; goto _LL1449;} else{ goto _LL1420;} _LL1449: _temp1448=
_temp1412.f2; if(( unsigned int) _temp1448 > 4u?*(( int*) _temp1448) == Cyc_Absyn_TunionType:
0){ _LL1451: _temp1450=(( struct Cyc_Absyn_TunionType_struct*) _temp1448)->f1;
_LL1455: _temp1454=( void*) _temp1450.tunion_info; if(*(( int*) _temp1454) ==
Cyc_Absyn_KnownTunion){ _LL1457: _temp1456=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1454)->f1; goto _LL1453;} else{ goto _LL1420;} _LL1453: _temp1452=
_temp1450.targs; goto _LL1419;} else{ goto _LL1420;} _LL1420: _LL1483: _temp1482=
_temp1412.f1; if(( unsigned int) _temp1482 > 4u?*(( int*) _temp1482) == Cyc_Absyn_PointerType:
0){ _LL1485: _temp1484=(( struct Cyc_Absyn_PointerType_struct*) _temp1482)->f1;
_LL1495: _temp1494=( void*) _temp1484.elt_typ; goto _LL1493; _LL1493: _temp1492=(
void*) _temp1484.rgn_typ; goto _LL1491; _LL1491: _temp1490= _temp1484.nullable;
goto _LL1489; _LL1489: _temp1488= _temp1484.tq; goto _LL1487; _LL1487: _temp1486=
_temp1484.bounds; goto _LL1471;} else{ goto _LL1422;} _LL1471: _temp1470=
_temp1412.f2; if(( unsigned int) _temp1470 > 4u?*(( int*) _temp1470) == Cyc_Absyn_TunionType:
0){ _LL1473: _temp1472=(( struct Cyc_Absyn_TunionType_struct*) _temp1470)->f1;
_LL1479: _temp1478=( void*) _temp1472.tunion_info; if(*(( int*) _temp1478) ==
Cyc_Absyn_KnownTunion){ _LL1481: _temp1480=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1478)->f1; goto _LL1477;} else{ goto _LL1422;} _LL1477: _temp1476=
_temp1472.targs; goto _LL1475; _LL1475: _temp1474=( void*) _temp1472.rgn; goto
_LL1421;} else{ goto _LL1422;} _LL1422: goto _LL1423; _LL1415: { int okay= 1;
if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1430.nullable, _temp1426.nullable)){
void* _temp1496=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp1430.nullable))->v; int _temp1502; _LL1498:
if(( unsigned int) _temp1496 > 1u?*(( int*) _temp1496) == Cyc_Absyn_Eq_constr: 0){
_LL1503: _temp1502=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1496)->f1;
goto _LL1499;} else{ goto _LL1500;} _LL1500: goto _LL1501; _LL1499: okay= !
_temp1502; goto _LL1497; _LL1501:( int) _throw(({ void* _temp1504[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("silent_castable conref not eq", sizeof( unsigned char), 30u), _tag_arr(
_temp1504, sizeof( void*), 0u));})); _LL1497:;} if( ! Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_boundscmp, _temp1430.bounds, _temp1426.bounds)){ struct _tuple8
_temp1506=({ struct _tuple8 _temp1505; _temp1505.f1=( void*)( Cyc_Absyn_compress_conref(
_temp1430.bounds))->v; _temp1505.f2=( void*)( Cyc_Absyn_compress_conref(
_temp1426.bounds))->v; _temp1505;}); void* _temp1516; void* _temp1518; void*
_temp1520; void* _temp1522; void* _temp1524; void* _temp1526; struct Cyc_Absyn_Exp*
_temp1528; void* _temp1530; void* _temp1532; struct Cyc_Absyn_Exp* _temp1534;
void* _temp1536; void* _temp1538; void* _temp1540; void* _temp1542; _LL1508:
_LL1521: _temp1520= _temp1506.f1; if(( unsigned int) _temp1520 > 1u?*(( int*)
_temp1520) == Cyc_Absyn_Eq_constr: 0){ _LL1523: _temp1522=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1520)->f1; if(( unsigned int) _temp1522 > 1u?*(( int*) _temp1522) == Cyc_Absyn_Upper_b:
0){ goto _LL1517;} else{ goto _LL1510;}} else{ goto _LL1510;} _LL1517: _temp1516=
_temp1506.f2; if(( unsigned int) _temp1516 > 1u?*(( int*) _temp1516) == Cyc_Absyn_Eq_constr:
0){ _LL1519: _temp1518=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1516)->f1;
if( _temp1518 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1509;} else{ goto _LL1510;}}
else{ goto _LL1510;} _LL1510: _LL1531: _temp1530= _temp1506.f1; if((
unsigned int) _temp1530 > 1u?*(( int*) _temp1530) == Cyc_Absyn_Eq_constr: 0){
_LL1533: _temp1532=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1530)->f1;
if(( unsigned int) _temp1532 > 1u?*(( int*) _temp1532) == Cyc_Absyn_Upper_b: 0){
_LL1535: _temp1534=(( struct Cyc_Absyn_Upper_b_struct*) _temp1532)->f1; goto
_LL1525;} else{ goto _LL1512;}} else{ goto _LL1512;} _LL1525: _temp1524=
_temp1506.f2; if(( unsigned int) _temp1524 > 1u?*(( int*) _temp1524) == Cyc_Absyn_Eq_constr:
0){ _LL1527: _temp1526=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1524)->f1;
if(( unsigned int) _temp1526 > 1u?*(( int*) _temp1526) == Cyc_Absyn_Upper_b: 0){
_LL1529: _temp1528=(( struct Cyc_Absyn_Upper_b_struct*) _temp1526)->f1; goto
_LL1511;} else{ goto _LL1512;}} else{ goto _LL1512;} _LL1512: _LL1541: _temp1540=
_temp1506.f1; if(( unsigned int) _temp1540 > 1u?*(( int*) _temp1540) == Cyc_Absyn_Eq_constr:
0){ _LL1543: _temp1542=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1540)->f1;
if( _temp1542 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1537;} else{ goto _LL1514;}}
else{ goto _LL1514;} _LL1537: _temp1536= _temp1506.f2; if(( unsigned int)
_temp1536 > 1u?*(( int*) _temp1536) == Cyc_Absyn_Eq_constr: 0){ _LL1539:
_temp1538=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1536)->f1; if(
_temp1538 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1513;} else{ goto _LL1514;}}
else{ goto _LL1514;} _LL1514: goto _LL1515; _LL1509: okay= 1; goto _LL1507;
_LL1511: okay= okay? Cyc_Evexp_eval_const_uint_exp( _temp1534) >= Cyc_Evexp_eval_const_uint_exp(
_temp1528): 0;({ void* _temp1544[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("implicit cast to shorter array",
sizeof( unsigned char), 31u), _tag_arr( _temp1544, sizeof( void*), 0u));}); goto
_LL1507; _LL1513: okay= 1; goto _LL1507; _LL1515: okay= 0; goto _LL1507; _LL1507:;}
okay= okay? Cyc_Tcutil_unify(( void*) _temp1430.elt_typ,( void*) _temp1426.elt_typ):
0; okay= okay? Cyc_Tcutil_unify(( void*) _temp1430.rgn_typ,( void*) _temp1426.rgn_typ)?
1: Cyc_Tcenv_region_outlives( te,( void*) _temp1430.rgn_typ,( void*) _temp1426.rgn_typ):
0; okay= okay? !( _temp1430.tq).q_const? 1:( _temp1426.tq).q_const: 0; return
okay;} _LL1417: { int okay; okay=( _temp1442 != 0? _temp1434 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1442)) == Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1434)): 0; return( okay? Cyc_Tcutil_unify(
_temp1446, _temp1438): 0)? ! _temp1444.q_const? 1: _temp1436.q_const: 0;}
_LL1419: if(( _temp1468 == _temp1456? 1: Cyc_Absyn_qvar_cmp( _temp1468->name,
_temp1456->name) == 0)? _temp1466->typs == 0: 0){ for( 0; _temp1462 != 0?
_temp1452 != 0: 0;( _temp1462=(( struct Cyc_List_List*) _check_null( _temp1462))->tl,
_temp1452=(( struct Cyc_List_List*) _check_null( _temp1452))->tl)){ if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_List_List*) _check_null( _temp1462))->hd,( void*)(( struct
Cyc_List_List*) _check_null( _temp1452))->hd)){ break;}} if( _temp1462 == 0?
_temp1452 == 0: 0){ return 1;}} return 0; _LL1421:{ void* _temp1545= Cyc_Tcutil_compress(
_temp1494); struct Cyc_Absyn_TunionFieldInfo _temp1551; struct Cyc_List_List*
_temp1553; void* _temp1555; struct Cyc_Absyn_Tunionfield* _temp1557; struct Cyc_Absyn_Tuniondecl*
_temp1559; _LL1547: if(( unsigned int) _temp1545 > 4u?*(( int*) _temp1545) ==
Cyc_Absyn_TunionFieldType: 0){ _LL1552: _temp1551=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1545)->f1; _LL1556: _temp1555=( void*) _temp1551.field_info; if(*(( int*)
_temp1555) == Cyc_Absyn_KnownTunionfield){ _LL1560: _temp1559=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1555)->f1; goto _LL1558; _LL1558: _temp1557=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1555)->f2; goto _LL1554;} else{ goto _LL1549;} _LL1554: _temp1553=
_temp1551.targs; goto _LL1548;} else{ goto _LL1549;} _LL1549: goto _LL1550;
_LL1548: if( ! Cyc_Tcutil_unify( _temp1492, _temp1474)? ! Cyc_Tcenv_region_outlives(
te, _temp1492, _temp1474): 0){ return 0;} if( !(( int(*)( int(* cmp)( int, int),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp, _temp1490,(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
0))){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp, _temp1486,
Cyc_Absyn_new_conref( Cyc_Absyn_bounds_one))){ return 0;} if( Cyc_Absyn_qvar_cmp(
_temp1480->name, _temp1559->name) == 0? _temp1557->typs != 0: 0){ int okay= 1;
for( 0; _temp1553 != 0? _temp1476 != 0: 0;( _temp1553=(( struct Cyc_List_List*)
_check_null( _temp1553))->tl, _temp1476=(( struct Cyc_List_List*) _check_null(
_temp1476))->tl)){ if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_List_List*)
_check_null( _temp1553))->hd,( void*)(( struct Cyc_List_List*) _check_null(
_temp1476))->hd)){ okay= 0; break;}} if(( ! okay? 1: _temp1553 != 0)? 1:
_temp1476 != 0){ return 0;} return 1;} goto _LL1546; _LL1550: goto _LL1546;
_LL1546:;} return 0; _LL1423: return Cyc_Tcutil_unify( t1, t2); _LL1413:;}} int
Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void*
t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} if( Cyc_Tcutil_is_arithmetic_type(
t1)? Cyc_Tcutil_is_arithmetic_type( t2): 0){ if( Cyc_Tcutil_will_lose_precision(
t1, t2)){({ void* _temp1561[ 0u]={}; Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u), _tag_arr( _temp1561, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){
Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2);({ struct Cyc_Stdio_String_pa_struct
_temp1564; _temp1564.tag= Cyc_Stdio_String_pa; _temp1564.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t2);{ struct Cyc_Stdio_String_pa_struct _temp1563;
_temp1563.tag= Cyc_Stdio_String_pa; _temp1563.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp1562[ 2u]={& _temp1563,& _temp1564}; Cyc_Tcutil_warn( e->loc,
_tag_arr("implicit cast from %s to %s", sizeof( unsigned char), 28u), _tag_arr(
_temp1562, sizeof( void*), 2u));}}}); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg(
te, e, t);} static int Cyc_Tcutil_any_promote( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e){ void* old_typ= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); void* t;{ void* _temp1565= old_typ; void* _temp1577;
void* _temp1579; _LL1567: if(( unsigned int) _temp1565 > 4u?*(( int*) _temp1565)
== Cyc_Absyn_EnumType: 0){ goto _LL1568;} else{ goto _LL1569;} _LL1569: if((
unsigned int) _temp1565 > 4u?*(( int*) _temp1565) == Cyc_Absyn_IntType: 0){
_LL1580: _temp1579=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1565)->f1;
goto _LL1578; _LL1578: _temp1577=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1565)->f2; goto _LL1570;} else{ goto _LL1571;} _LL1571: if( _temp1565 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1572;} else{ goto _LL1573;} _LL1573: if(
_temp1565 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1574;} else{ goto _LL1575;}
_LL1575: goto _LL1576; _LL1568: t= Cyc_Absyn_sint_t; goto _LL1566; _LL1570:{
void* _temp1581= _temp1577; _LL1583: if( _temp1581 ==( void*) Cyc_Absyn_B1){
goto _LL1584;} else{ goto _LL1585;} _LL1585: if( _temp1581 ==( void*) Cyc_Absyn_B2){
goto _LL1586;} else{ goto _LL1587;} _LL1587: if( _temp1581 ==( void*) Cyc_Absyn_B4){
goto _LL1588;} else{ goto _LL1589;} _LL1589: if( _temp1581 ==( void*) Cyc_Absyn_B8){
goto _LL1590;} else{ goto _LL1582;} _LL1584: t= Cyc_Absyn_sint_t; goto _LL1582;
_LL1586: t= Cyc_Absyn_sint_t; goto _LL1582; _LL1588: t= _temp1579 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL1582; _LL1590: t= _temp1579 ==( void*)
Cyc_Absyn_Unsigned? Cyc_Absyn_ulonglong_t: Cyc_Absyn_slonglong_t; goto _LL1582;
_LL1582:;} goto _LL1566; _LL1572: t= Cyc_Absyn_float_t; goto _LL1566; _LL1574: t=
Cyc_Absyn_double_t; goto _LL1566; _LL1576: return 0; _LL1566:;} Cyc_Tcutil_unchecked_cast(
te, e, t); return 1;} int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){
return 1;} Cyc_Tcutil_any_promote( te, e); if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, t2)){ return 1;} else{ if( Cyc_Tcutil_silent_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{
if( Cyc_Tcutil_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e,
t2);({ struct Cyc_Stdio_String_pa_struct _temp1593; _temp1593.tag= Cyc_Stdio_String_pa;
_temp1593.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_Stdio_String_pa_struct
_temp1592; _temp1592.tag= Cyc_Stdio_String_pa; _temp1592.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t1);{ void* _temp1591[ 2u]={& _temp1592,& _temp1593};
Cyc_Tcutil_warn( e->loc, _tag_arr("implicit cast from %s to %s", sizeof(
unsigned char), 28u), _tag_arr( _temp1591, sizeof( void*), 2u));}}}); return 1;}
else{ return 0;}}}} int Cyc_Tcutil_coerceable( void* t){ void* _temp1594= Cyc_Tcutil_compress(
t); _LL1596: if(( unsigned int) _temp1594 > 4u?*(( int*) _temp1594) == Cyc_Absyn_IntType:
0){ goto _LL1597;} else{ goto _LL1598;} _LL1598: if( _temp1594 ==( void*) Cyc_Absyn_FloatType){
goto _LL1599;} else{ goto _LL1600;} _LL1600: if( _temp1594 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1601;} else{ goto _LL1602;} _LL1602: goto _LL1603; _LL1597: return 1;
_LL1599: return 1; _LL1601: return 1; _LL1603: return 0; _LL1595:;} static
struct _tuple4* Cyc_Tcutil_flatten_typ_f( struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield*
x){ return({ struct _tuple4* _temp1604=( struct _tuple4*) GC_malloc( sizeof(
struct _tuple4)); _temp1604->f1= x->tq; _temp1604->f2= Cyc_Tcutil_substitute(
inst,( void*) x->type); _temp1604;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ(
struct Cyc_Tcenv_Tenv* te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void*
_temp1605= t1; struct Cyc_List_List* _temp1617; struct Cyc_Absyn_Structdecl**
_temp1619; struct Cyc_List_List* _temp1621; struct _tuple1* _temp1623; struct
Cyc_List_List* _temp1625; _LL1607: if( _temp1605 ==( void*) Cyc_Absyn_VoidType){
goto _LL1608;} else{ goto _LL1609;} _LL1609: if(( unsigned int) _temp1605 > 4u?*((
int*) _temp1605) == Cyc_Absyn_TupleType: 0){ _LL1618: _temp1617=(( struct Cyc_Absyn_TupleType_struct*)
_temp1605)->f1; goto _LL1610;} else{ goto _LL1611;} _LL1611: if(( unsigned int)
_temp1605 > 4u?*(( int*) _temp1605) == Cyc_Absyn_StructType: 0){ _LL1624:
_temp1623=(( struct Cyc_Absyn_StructType_struct*) _temp1605)->f1; goto _LL1622;
_LL1622: _temp1621=(( struct Cyc_Absyn_StructType_struct*) _temp1605)->f2; goto
_LL1620; _LL1620: _temp1619=(( struct Cyc_Absyn_StructType_struct*) _temp1605)->f3;
goto _LL1612;} else{ goto _LL1613;} _LL1613: if(( unsigned int) _temp1605 > 4u?*((
int*) _temp1605) == Cyc_Absyn_AnonStructType: 0){ _LL1626: _temp1625=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1605)->f1; goto _LL1614;} else{ goto
_LL1615;} _LL1615: goto _LL1616; _LL1608: return 0; _LL1610: return _temp1617;
_LL1612: if( _temp1619 == 0? 1: _temp1623 == 0){ return({ struct Cyc_List_List*
_temp1627=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1627->hd=( void*)({ struct _tuple4* _temp1628=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1628->f1= Cyc_Absyn_empty_tqual(); _temp1628->f2=
t1; _temp1628;}); _temp1627->tl= 0; _temp1627;});}{ struct Cyc_Absyn_Structdecl*
_temp1629=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp1619)); struct
_tuple1 _temp1630=*(( struct _tuple1*) _check_null( _temp1623)); struct Cyc_List_List*
_temp1631=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp1629->tvs, _temp1621); if( _temp1629->fields == 0){
return({ struct Cyc_List_List* _temp1632=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1632->hd=( void*)({ struct _tuple4*
_temp1633=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1633->f1=
Cyc_Absyn_empty_tqual(); _temp1633->f2= t1; _temp1633;}); _temp1632->tl= 0;
_temp1632;});} return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, _temp1631,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1629->fields))->v);}
_LL1614: return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, 0, _temp1625); _LL1616: return({
struct Cyc_List_List* _temp1634=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1634->hd=( void*)({ struct _tuple4* _temp1635=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1635->f1= Cyc_Absyn_empty_tqual();
_temp1635->f2= t1; _temp1635;}); _temp1634->tl= 0; _temp1634;}); _LL1606:;}}
static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2); static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;}{ struct Cyc_List_List* a= assume; for( 0; a != 0; a=(( struct
Cyc_List_List*) _check_null( a))->tl){ if( Cyc_Tcutil_unify( t1,(*(( struct
_tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f1)? Cyc_Tcutil_unify(
t2,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f2): 0){
return 1;}}} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct
_tuple8 _temp1637=({ struct _tuple8 _temp1636; _temp1636.f1= t1; _temp1636.f2=
t2; _temp1636;}); void* _temp1643; struct Cyc_Absyn_PtrInfo _temp1645; struct
Cyc_Absyn_Conref* _temp1647; struct Cyc_Absyn_Tqual _temp1649; struct Cyc_Absyn_Conref*
_temp1651; void* _temp1653; void* _temp1655; void* _temp1657; struct Cyc_Absyn_PtrInfo
_temp1659; struct Cyc_Absyn_Conref* _temp1661; struct Cyc_Absyn_Tqual _temp1663;
struct Cyc_Absyn_Conref* _temp1665; void* _temp1667; void* _temp1669; _LL1639:
_LL1658: _temp1657= _temp1637.f1; if(( unsigned int) _temp1657 > 4u?*(( int*)
_temp1657) == Cyc_Absyn_PointerType: 0){ _LL1660: _temp1659=(( struct Cyc_Absyn_PointerType_struct*)
_temp1657)->f1; _LL1670: _temp1669=( void*) _temp1659.elt_typ; goto _LL1668;
_LL1668: _temp1667=( void*) _temp1659.rgn_typ; goto _LL1666; _LL1666: _temp1665=
_temp1659.nullable; goto _LL1664; _LL1664: _temp1663= _temp1659.tq; goto _LL1662;
_LL1662: _temp1661= _temp1659.bounds; goto _LL1644;} else{ goto _LL1641;}
_LL1644: _temp1643= _temp1637.f2; if(( unsigned int) _temp1643 > 4u?*(( int*)
_temp1643) == Cyc_Absyn_PointerType: 0){ _LL1646: _temp1645=(( struct Cyc_Absyn_PointerType_struct*)
_temp1643)->f1; _LL1656: _temp1655=( void*) _temp1645.elt_typ; goto _LL1654;
_LL1654: _temp1653=( void*) _temp1645.rgn_typ; goto _LL1652; _LL1652: _temp1651=
_temp1645.nullable; goto _LL1650; _LL1650: _temp1649= _temp1645.tq; goto _LL1648;
_LL1648: _temp1647= _temp1645.bounds; goto _LL1640;} else{ goto _LL1641;}
_LL1641: goto _LL1642; _LL1640: if( _temp1663.q_const? ! _temp1649.q_const: 0){
return 0;} if(( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1665, _temp1651)?(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1665): 0)? !(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1651): 0){ return 0;} if( ! Cyc_Tcutil_unify( _temp1667, _temp1653)? ! Cyc_Tcenv_region_outlives(
te, _temp1667, _temp1653): 0){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp,
_temp1661, _temp1647)){ struct _tuple8 _temp1672=({ struct _tuple8 _temp1671;
_temp1671.f1= Cyc_Absyn_conref_val( _temp1661); _temp1671.f2= Cyc_Absyn_conref_val(
_temp1647); _temp1671;}); void* _temp1680; void* _temp1682; void* _temp1684;
struct Cyc_Absyn_Exp* _temp1686; void* _temp1688; struct Cyc_Absyn_Exp*
_temp1690; _LL1674: _LL1683: _temp1682= _temp1672.f1; if(( unsigned int)
_temp1682 > 1u?*(( int*) _temp1682) == Cyc_Absyn_Upper_b: 0){ goto _LL1681;}
else{ goto _LL1676;} _LL1681: _temp1680= _temp1672.f2; if( _temp1680 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL1675;} else{ goto _LL1676;} _LL1676: _LL1689:
_temp1688= _temp1672.f1; if(( unsigned int) _temp1688 > 1u?*(( int*) _temp1688)
== Cyc_Absyn_Upper_b: 0){ _LL1691: _temp1690=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1688)->f1; goto _LL1685;} else{ goto _LL1678;} _LL1685: _temp1684=
_temp1672.f2; if(( unsigned int) _temp1684 > 1u?*(( int*) _temp1684) == Cyc_Absyn_Upper_b:
0){ _LL1687: _temp1686=(( struct Cyc_Absyn_Upper_b_struct*) _temp1684)->f1; goto
_LL1677;} else{ goto _LL1678;} _LL1678: goto _LL1679; _LL1675: goto _LL1673;
_LL1677: if( Cyc_Evexp_eval_const_uint_exp( _temp1690) < Cyc_Evexp_eval_const_uint_exp(
_temp1686)){ return 0;} goto _LL1673; _LL1679: return 0; _LL1673:;} return Cyc_Tcutil_ptrsubtype(
te,({ struct Cyc_List_List* _temp1692=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1692->hd=( void*)({ struct _tuple8*
_temp1693=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp1693->f1=
t1; _temp1693->f2= t2; _temp1693;}); _temp1692->tl= assume; _temp1692;}),
_temp1669, _temp1655); _LL1642: return 0; _LL1638:;}} static int Cyc_Tcutil_isomorphic(
void* t1, void* t2){ struct _tuple8 _temp1695=({ struct _tuple8 _temp1694;
_temp1694.f1= Cyc_Tcutil_compress( t1); _temp1694.f2= Cyc_Tcutil_compress( t2);
_temp1694;}); void* _temp1701; void* _temp1703; void* _temp1705; void* _temp1707;
_LL1697: _LL1706: _temp1705= _temp1695.f1; if(( unsigned int) _temp1705 > 4u?*((
int*) _temp1705) == Cyc_Absyn_IntType: 0){ _LL1708: _temp1707=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1705)->f2; goto _LL1702;} else{ goto _LL1699;}
_LL1702: _temp1701= _temp1695.f2; if(( unsigned int) _temp1701 > 4u?*(( int*)
_temp1701) == Cyc_Absyn_IntType: 0){ _LL1704: _temp1703=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1701)->f2; goto _LL1698;} else{ goto _LL1699;} _LL1699: goto _LL1700;
_LL1698: return _temp1707 == _temp1703; _LL1700: return 0; _LL1696:;} static int
Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* assume,
void* t1, void* t2){ struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ( te, t1);
struct Cyc_List_List* tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0; tqs2 != 0;(
tqs2=(( struct Cyc_List_List*) _check_null( tqs2))->tl, tqs1=(( struct Cyc_List_List*)
_check_null( tqs1))->tl)){ if( tqs1 == 0){ return 0;}{ struct _tuple4 _temp1711;
void* _temp1712; struct Cyc_Absyn_Tqual _temp1714; struct _tuple4* _temp1709=(
struct _tuple4*)(( struct Cyc_List_List*) _check_null( tqs1))->hd; _temp1711=*
_temp1709; _LL1715: _temp1714= _temp1711.f1; goto _LL1713; _LL1713: _temp1712=
_temp1711.f2; goto _LL1710; _LL1710: { struct _tuple4 _temp1718; void* _temp1719;
struct Cyc_Absyn_Tqual _temp1721; struct _tuple4* _temp1716=( struct _tuple4*)((
struct Cyc_List_List*) _check_null( tqs2))->hd; _temp1718=* _temp1716; _LL1722:
_temp1721= _temp1718.f1; goto _LL1720; _LL1720: _temp1719= _temp1718.f2; goto
_LL1717; _LL1717: if( _temp1721.q_const? Cyc_Tcutil_subtype( te, assume,
_temp1712, _temp1719): 0){ continue;} else{ if( Cyc_Tcutil_unify( _temp1712,
_temp1719)){ continue;} else{ if( Cyc_Tcutil_isomorphic( _temp1712, _temp1719)){
continue;} else{ return 0;}}}}}} return 1;} int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void* t1, void* t2){ if( Cyc_Tcutil_unify(
t1, t2)){ return 1;} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
void* _temp1723= t2; struct Cyc_Absyn_Uniondecl** _temp1731; struct Cyc_List_List*
_temp1733; void* _temp1735; _LL1725: if(( unsigned int) _temp1723 > 4u?*(( int*)
_temp1723) == Cyc_Absyn_UnionType: 0){ _LL1734: _temp1733=(( struct Cyc_Absyn_UnionType_struct*)
_temp1723)->f2; goto _LL1732; _LL1732: _temp1731=(( struct Cyc_Absyn_UnionType_struct*)
_temp1723)->f3; goto _LL1726;} else{ goto _LL1727;} _LL1727: if(( unsigned int)
_temp1723 > 4u?*(( int*) _temp1723) == Cyc_Absyn_IntType: 0){ _LL1736: _temp1735=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1723)->f2; if( _temp1735 ==(
void*) Cyc_Absyn_B4){ goto _LL1728;} else{ goto _LL1729;}} else{ goto _LL1729;}
_LL1729: goto _LL1730; _LL1726: if( _temp1731 == 0){ goto _LL1724;}{ struct Cyc_Absyn_Uniondecl*
_temp1737=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp1731)); if(
_temp1737->fields == 0){ goto _LL1724;}{ struct Cyc_List_List* _temp1738=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1737->tvs, _temp1733);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1737->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ if( Cyc_Tcutil_unify( t1, Cyc_Tcutil_substitute(
_temp1738,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ return 1;}}} goto _LL1724;}} _LL1728: if( Cyc_Tcutil_typ_kind(
t2) ==( void*) Cyc_Absyn_BoxKind){ return 1;} goto _LL1724; _LL1730: goto
_LL1724; _LL1724:;}{ void* _temp1739= t1; struct Cyc_Absyn_PtrInfo _temp1755;
struct Cyc_Absyn_Conref* _temp1757; struct Cyc_Absyn_Tqual _temp1759; struct Cyc_Absyn_Conref*
_temp1761; void* _temp1763; void* _temp1765; struct Cyc_Absyn_Exp* _temp1767;
struct Cyc_Absyn_Tqual _temp1769; void* _temp1771; struct Cyc_Absyn_Enumdecl*
_temp1773; _LL1741: if(( unsigned int) _temp1739 > 4u?*(( int*) _temp1739) ==
Cyc_Absyn_PointerType: 0){ _LL1756: _temp1755=(( struct Cyc_Absyn_PointerType_struct*)
_temp1739)->f1; _LL1766: _temp1765=( void*) _temp1755.elt_typ; goto _LL1764;
_LL1764: _temp1763=( void*) _temp1755.rgn_typ; goto _LL1762; _LL1762: _temp1761=
_temp1755.nullable; goto _LL1760; _LL1760: _temp1759= _temp1755.tq; goto _LL1758;
_LL1758: _temp1757= _temp1755.bounds; goto _LL1742;} else{ goto _LL1743;}
_LL1743: if(( unsigned int) _temp1739 > 4u?*(( int*) _temp1739) == Cyc_Absyn_ArrayType:
0){ _LL1772: _temp1771=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1739)->f1;
goto _LL1770; _LL1770: _temp1769=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1739)->f2; goto _LL1768; _LL1768: _temp1767=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1739)->f3; goto _LL1744;} else{ goto _LL1745;} _LL1745: if(( unsigned int)
_temp1739 > 4u?*(( int*) _temp1739) == Cyc_Absyn_EnumType: 0){ _LL1774:
_temp1773=(( struct Cyc_Absyn_EnumType_struct*) _temp1739)->f2; goto _LL1746;}
else{ goto _LL1747;} _LL1747: if(( unsigned int) _temp1739 > 4u?*(( int*)
_temp1739) == Cyc_Absyn_IntType: 0){ goto _LL1748;} else{ goto _LL1749;} _LL1749:
if( _temp1739 ==( void*) Cyc_Absyn_FloatType){ goto _LL1750;} else{ goto _LL1751;}
_LL1751: if( _temp1739 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1752;} else{
goto _LL1753;} _LL1753: goto _LL1754; _LL1742:{ void* _temp1775= t2; struct Cyc_Absyn_PtrInfo
_temp1781; struct Cyc_Absyn_Conref* _temp1783; struct Cyc_Absyn_Tqual _temp1785;
struct Cyc_Absyn_Conref* _temp1787; void* _temp1789; void* _temp1791; _LL1777:
if(( unsigned int) _temp1775 > 4u?*(( int*) _temp1775) == Cyc_Absyn_PointerType:
0){ _LL1782: _temp1781=(( struct Cyc_Absyn_PointerType_struct*) _temp1775)->f1;
_LL1792: _temp1791=( void*) _temp1781.elt_typ; goto _LL1790; _LL1790: _temp1789=(
void*) _temp1781.rgn_typ; goto _LL1788; _LL1788: _temp1787= _temp1781.nullable;
goto _LL1786; _LL1786: _temp1785= _temp1781.tq; goto _LL1784; _LL1784: _temp1783=
_temp1781.bounds; goto _LL1778;} else{ goto _LL1779;} _LL1779: goto _LL1780;
_LL1778: { struct Cyc_List_List* _temp1793=({ struct Cyc_List_List* _temp1810=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1810->hd=(
void*)({ struct _tuple8* _temp1811=( struct _tuple8*) GC_malloc( sizeof( struct
_tuple8)); _temp1811->f1= t1; _temp1811->f2= t2; _temp1811;}); _temp1810->tl= 0;
_temp1810;}); int _temp1794= Cyc_Tcutil_ptrsubtype( te, _temp1793, _temp1765,
_temp1791)? ! _temp1759.q_const? 1: _temp1785.q_const: 0; int bounds_ok= Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_boundscmp, _temp1757, _temp1783); if( ! bounds_ok){ struct _tuple8
_temp1796=({ struct _tuple8 _temp1795; _temp1795.f1= Cyc_Absyn_conref_val(
_temp1757); _temp1795.f2= Cyc_Absyn_conref_val( _temp1783); _temp1795;}); void*
_temp1802; struct Cyc_Absyn_Exp* _temp1804; void* _temp1806; struct Cyc_Absyn_Exp*
_temp1808; _LL1798: _LL1807: _temp1806= _temp1796.f1; if(( unsigned int)
_temp1806 > 1u?*(( int*) _temp1806) == Cyc_Absyn_Upper_b: 0){ _LL1809: _temp1808=((
struct Cyc_Absyn_Upper_b_struct*) _temp1806)->f1; goto _LL1803;} else{ goto
_LL1800;} _LL1803: _temp1802= _temp1796.f2; if(( unsigned int) _temp1802 > 1u?*((
int*) _temp1802) == Cyc_Absyn_Upper_b: 0){ _LL1805: _temp1804=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1802)->f1; goto _LL1799;} else{ goto _LL1800;} _LL1800: goto _LL1801;
_LL1799: if( Cyc_Evexp_eval_const_uint_exp( _temp1808) >= Cyc_Evexp_eval_const_uint_exp(
_temp1804)){ bounds_ok= 1;} goto _LL1797; _LL1801: bounds_ok= 1; goto _LL1797;
_LL1797:;} return( bounds_ok? _temp1794: 0)? Cyc_Tcutil_unify( _temp1763,
_temp1789)? 1: Cyc_Tcenv_region_outlives( te, _temp1763, _temp1789): 0;} _LL1780:
goto _LL1776; _LL1776:;} return 0; _LL1744:{ void* _temp1812= t2; struct Cyc_Absyn_Exp*
_temp1818; struct Cyc_Absyn_Tqual _temp1820; void* _temp1822; _LL1814: if((
unsigned int) _temp1812 > 4u?*(( int*) _temp1812) == Cyc_Absyn_ArrayType: 0){
_LL1823: _temp1822=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1812)->f1;
goto _LL1821; _LL1821: _temp1820=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1812)->f2; goto _LL1819; _LL1819: _temp1818=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1812)->f3; goto _LL1815;} else{ goto _LL1816;} _LL1816: goto _LL1817;
_LL1815: { int okay; okay=( _temp1767 != 0? _temp1818 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1767)) >= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1818)): 0; return( okay? Cyc_Tcutil_unify(
_temp1771, _temp1822): 0)? ! _temp1769.q_const? 1: _temp1820.q_const: 0;}
_LL1817: return 0; _LL1813:;} return 0; _LL1746:{ void* _temp1824= t2; struct
Cyc_Absyn_Enumdecl* _temp1830; _LL1826: if(( unsigned int) _temp1824 > 4u?*((
int*) _temp1824) == Cyc_Absyn_EnumType: 0){ _LL1831: _temp1830=(( struct Cyc_Absyn_EnumType_struct*)
_temp1824)->f2; goto _LL1827;} else{ goto _LL1828;} _LL1828: goto _LL1829;
_LL1827: if(((( struct Cyc_Absyn_Enumdecl*) _check_null( _temp1773))->fields !=
0?(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp1830))->fields != 0: 0)?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Enumdecl*) _check_null(
_temp1773))->fields))->v) >=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Enumdecl*)
_check_null( _temp1830))->fields))->v): 0){ return 1;} goto _LL1825; _LL1829:
goto _LL1825; _LL1825:;} goto _LL1748; _LL1748: goto _LL1750; _LL1750: goto
_LL1752; _LL1752: return Cyc_Tcutil_coerceable( t2); _LL1754: return 0; _LL1740:;}}
void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t){ if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, t)){ struct Cyc_Absyn_Exp* _temp1832= Cyc_Absyn_copy_exp( e);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct* _temp1833=(
struct Cyc_Absyn_Cast_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp1833[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp1834; _temp1834.tag= Cyc_Absyn_Cast_e;
_temp1834.f1=( void*) t; _temp1834.f2= _temp1832; _temp1834;}); _temp1833;})));
e->topt=({ struct Cyc_Core_Opt* _temp1835=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1835->v=( void*) t; _temp1835;});}} int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp* e){ void* _temp1836= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v); _LL1838: if(( unsigned int) _temp1836
> 4u?*(( int*) _temp1836) == Cyc_Absyn_EnumType: 0){ goto _LL1839;} else{ goto
_LL1840;} _LL1840: if(( unsigned int) _temp1836 > 4u?*(( int*) _temp1836) == Cyc_Absyn_IntType:
0){ goto _LL1841;} else{ goto _LL1842;} _LL1842: if(( unsigned int) _temp1836 >
4u?*(( int*) _temp1836) == Cyc_Absyn_Evar: 0){ goto _LL1843;} else{ goto _LL1844;}
_LL1844: goto _LL1845; _LL1839: goto _LL1841; _LL1841: return 1; _LL1843: if(
Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){
return 1;} return 0; _LL1845: return 0; _LL1837:;} int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_is_integral( e)){ return 1;}{ void*
_temp1846= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_LL1848: if( _temp1846 ==( void*) Cyc_Absyn_FloatType){ goto _LL1849;} else{
goto _LL1850;} _LL1850: if( _temp1846 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL1851;} else{ goto _LL1852;} _LL1852: goto _LL1853; _LL1849: goto _LL1851;
_LL1851: return 1; _LL1853: return 0; _LL1847:;}} int Cyc_Tcutil_is_function_type(
void* t){ void* _temp1854= Cyc_Tcutil_compress( t); _LL1856: if(( unsigned int)
_temp1854 > 4u?*(( int*) _temp1854) == Cyc_Absyn_FnType: 0){ goto _LL1857;}
else{ goto _LL1858;} _LL1858: goto _LL1859; _LL1857: return 1; _LL1859: return 0;
_LL1855:;} void* Cyc_Tcutil_max_arithmetic_type( void* t1, void* t2){ struct
_tuple8 _temp1861=({ struct _tuple8 _temp1860; _temp1860.f1= t1; _temp1860.f2=
t2; _temp1860;}); void* _temp1885; void* _temp1887; void* _temp1889; void*
_temp1891; void* _temp1893; void* _temp1895; void* _temp1897; void* _temp1899;
void* _temp1901; void* _temp1903; void* _temp1905; void* _temp1907; void*
_temp1909; void* _temp1911; void* _temp1913; void* _temp1915; void* _temp1917;
void* _temp1919; void* _temp1921; void* _temp1923; void* _temp1925; void*
_temp1927; _LL1863: _LL1886: _temp1885= _temp1861.f1; if( _temp1885 ==( void*)
Cyc_Absyn_DoubleType){ goto _LL1864;} else{ goto _LL1865;} _LL1865: _LL1888:
_temp1887= _temp1861.f2; if( _temp1887 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL1866;} else{ goto _LL1867;} _LL1867: _LL1890: _temp1889= _temp1861.f1; if(
_temp1889 ==( void*) Cyc_Absyn_FloatType){ goto _LL1868;} else{ goto _LL1869;}
_LL1869: _LL1892: _temp1891= _temp1861.f2; if( _temp1891 ==( void*) Cyc_Absyn_FloatType){
goto _LL1870;} else{ goto _LL1871;} _LL1871: _LL1894: _temp1893= _temp1861.f1;
if(( unsigned int) _temp1893 > 4u?*(( int*) _temp1893) == Cyc_Absyn_IntType: 0){
_LL1898: _temp1897=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1893)->f1;
if( _temp1897 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1896;} else{ goto _LL1873;}
_LL1896: _temp1895=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1893)->f2;
if( _temp1895 ==( void*) Cyc_Absyn_B8){ goto _LL1872;} else{ goto _LL1873;}}
else{ goto _LL1873;} _LL1873: _LL1900: _temp1899= _temp1861.f2; if((
unsigned int) _temp1899 > 4u?*(( int*) _temp1899) == Cyc_Absyn_IntType: 0){
_LL1904: _temp1903=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1899)->f1;
if( _temp1903 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1902;} else{ goto _LL1875;}
_LL1902: _temp1901=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1899)->f2;
if( _temp1901 ==( void*) Cyc_Absyn_B8){ goto _LL1874;} else{ goto _LL1875;}}
else{ goto _LL1875;} _LL1875: _LL1906: _temp1905= _temp1861.f1; if((
unsigned int) _temp1905 > 4u?*(( int*) _temp1905) == Cyc_Absyn_IntType: 0){
_LL1910: _temp1909=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1905)->f1;
if( _temp1909 ==( void*) Cyc_Absyn_Signed){ goto _LL1908;} else{ goto _LL1877;}
_LL1908: _temp1907=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1905)->f2;
if( _temp1907 ==( void*) Cyc_Absyn_B8){ goto _LL1876;} else{ goto _LL1877;}}
else{ goto _LL1877;} _LL1877: _LL1912: _temp1911= _temp1861.f2; if((
unsigned int) _temp1911 > 4u?*(( int*) _temp1911) == Cyc_Absyn_IntType: 0){
_LL1916: _temp1915=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1911)->f1;
if( _temp1915 ==( void*) Cyc_Absyn_Signed){ goto _LL1914;} else{ goto _LL1879;}
_LL1914: _temp1913=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1911)->f2;
if( _temp1913 ==( void*) Cyc_Absyn_B8){ goto _LL1878;} else{ goto _LL1879;}}
else{ goto _LL1879;} _LL1879: _LL1918: _temp1917= _temp1861.f1; if((
unsigned int) _temp1917 > 4u?*(( int*) _temp1917) == Cyc_Absyn_IntType: 0){
_LL1922: _temp1921=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1917)->f1;
if( _temp1921 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1920;} else{ goto _LL1881;}
_LL1920: _temp1919=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1917)->f2;
if( _temp1919 ==( void*) Cyc_Absyn_B4){ goto _LL1880;} else{ goto _LL1881;}}
else{ goto _LL1881;} _LL1881: _LL1924: _temp1923= _temp1861.f2; if((
unsigned int) _temp1923 > 4u?*(( int*) _temp1923) == Cyc_Absyn_IntType: 0){
_LL1928: _temp1927=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1923)->f1;
if( _temp1927 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1926;} else{ goto _LL1883;}
_LL1926: _temp1925=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1923)->f2;
if( _temp1925 ==( void*) Cyc_Absyn_B4){ goto _LL1882;} else{ goto _LL1883;}}
else{ goto _LL1883;} _LL1883: goto _LL1884; _LL1864: goto _LL1866; _LL1866:
return( void*) Cyc_Absyn_DoubleType; _LL1868: goto _LL1870; _LL1870: return(
void*) Cyc_Absyn_FloatType; _LL1872: goto _LL1874; _LL1874: return Cyc_Absyn_ulonglong_t;
_LL1876: goto _LL1878; _LL1878: return Cyc_Absyn_slonglong_t; _LL1880: goto
_LL1882; _LL1882: return Cyc_Absyn_uint_t; _LL1884: return Cyc_Absyn_sint_t;
_LL1862:;} void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp* e){ void*
_temp1929=( void*) e->r; struct Cyc_Core_Opt* _temp1935; _LL1931: if(*(( int*)
_temp1929) == Cyc_Absyn_AssignOp_e){ _LL1936: _temp1935=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1929)->f2; if( _temp1935 == 0){ goto _LL1932;} else{ goto _LL1933;}} else{
goto _LL1933;} _LL1933: goto _LL1934; _LL1932:({ void* _temp1937[ 0u]={}; Cyc_Tcutil_warn(
e->loc, _tag_arr("assignment in test", sizeof( unsigned char), 19u), _tag_arr(
_temp1937, sizeof( void*), 0u));}); goto _LL1930; _LL1934: goto _LL1930; _LL1930:;}
static int Cyc_Tcutil_constrain_kinds( struct Cyc_Absyn_Conref* c1, struct Cyc_Absyn_Conref*
c2){ c1= Cyc_Absyn_compress_conref( c1); c2= Cyc_Absyn_compress_conref( c2); if(
c1 == c2){ return 1;} if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr){( void*)(
c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp1938=(
struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct));
_temp1938[ 0]=({ struct Cyc_Absyn_Forward_constr_struct _temp1939; _temp1939.tag=
Cyc_Absyn_Forward_constr; _temp1939.f1= c2; _temp1939;}); _temp1938;}))); return
1;} else{ if(( void*) c2->v ==( void*) Cyc_Absyn_No_constr){( void*)( c2->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp1940=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp1940[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp1941; _temp1941.tag= Cyc_Absyn_Forward_constr;
_temp1941.f1= c1; _temp1941;}); _temp1940;}))); return 1;} else{ void* k1= Cyc_Absyn_conref_val(
c1); void* k2= Cyc_Absyn_conref_val( c2); if( Cyc_Tcutil_kind_leq( k1, k2)){(
void*)( c2->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1942=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1942[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp1943; _temp1943.tag= Cyc_Absyn_Forward_constr; _temp1943.f1= c1; _temp1943;});
_temp1942;}))); return 1;} else{ if( Cyc_Tcutil_kind_leq( k2, k1)){( void*)( c1->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp1944=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp1944[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp1945; _temp1945.tag= Cyc_Absyn_Forward_constr;
_temp1945.f1= c2; _temp1945;}); _temp1944;}))); return 1;} else{ return 0;}}}}}
static int Cyc_Tcutil_tvar_id_counter= 0; int* Cyc_Tcutil_new_tvar_id(){ return({
int* _temp1946=( int*) GC_malloc_atomic( sizeof( int)); _temp1946[ 0]= Cyc_Tcutil_tvar_id_counter
++; _temp1946;});} static int Cyc_Tcutil_tvar_counter= 0; struct Cyc_Absyn_Tvar*
Cyc_Tcutil_new_tvar( void* k){ int i= Cyc_Tcutil_tvar_counter ++; struct
_tagged_arr s=( struct _tagged_arr)({ struct Cyc_Stdio_Int_pa_struct _temp1950;
_temp1950.tag= Cyc_Stdio_Int_pa; _temp1950.f1=( int)(( unsigned int) i);{ void*
_temp1949[ 1u]={& _temp1950}; Cyc_Stdio_aprintf( _tag_arr("#%d", sizeof(
unsigned char), 4u), _tag_arr( _temp1949, sizeof( void*), 1u));}}); return({
struct Cyc_Absyn_Tvar* _temp1947=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof(
struct Cyc_Absyn_Tvar)); _temp1947->name=({ struct _tagged_arr* _temp1948=(
struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1); _temp1948[ 0]=
s; _temp1948;}); _temp1947->identity= 0; _temp1947->kind= Cyc_Absyn_new_conref(
k); _temp1947;});} int Cyc_Tcutil_is_temp_tvar( struct Cyc_Absyn_Tvar* t){
struct _tagged_arr _temp1951=* t->name; return*(( const unsigned char*)
_check_unknown_subscript( _temp1951, sizeof( unsigned char), 0)) =='#';} void
Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar* t){ if( ! Cyc_Tcutil_is_temp_tvar(
t)){ return;}{ struct _tagged_arr _temp1952= Cyc_String_strconcat( _tag_arr("`",
sizeof( unsigned char), 2u),* t->name);*(( unsigned char*)
_check_unknown_subscript( _temp1952, sizeof( unsigned char), 1))='t'; t->name=({
struct _tagged_arr* _temp1953=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp1953[ 0]=( struct _tagged_arr) _temp1952; _temp1953;});}}
struct _tuple13{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; }
; static struct _tuple2* Cyc_Tcutil_fndecl2typ_f( struct _tuple13* x){ return({
struct _tuple2* _temp1954=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp1954->f1=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp1955=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1955->v=( void*)(*
x).f1; _temp1955;}); _temp1954->f2=(* x).f2; _temp1954->f3=(* x).f3; _temp1954;});}
void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl* fd){ if( fd->cached_typ ==
0){ struct Cyc_List_List* _temp1956= 0;{ struct Cyc_List_List* atts= fd->attributes;
for( 0; atts != 0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ if(
Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*) _check_null( atts))->hd)){
_temp1956=({ struct Cyc_List_List* _temp1957=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1957->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); _temp1957->tl= _temp1956; _temp1957;});}}} return(
void*)({ struct Cyc_Absyn_FnType_struct* _temp1958=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1958[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1959; _temp1959.tag= Cyc_Absyn_FnType; _temp1959.f1=({ struct Cyc_Absyn_FnInfo
_temp1960; _temp1960.tvars= fd->tvs; _temp1960.effect= fd->effect; _temp1960.ret_typ=(
void*)(( void*) fd->ret_type); _temp1960.args=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple13*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_fndecl2typ_f, fd->args); _temp1960.c_varargs= fd->c_varargs;
_temp1960.cyc_varargs= fd->cyc_varargs; _temp1960.rgn_po= fd->rgn_po; _temp1960.attributes=
_temp1956; _temp1960;}); _temp1959;}); _temp1958;});} return( void*)(( struct
Cyc_Core_Opt*) _check_null( fd->cached_typ))->v;} struct _tuple14{ void* f1;
struct Cyc_Absyn_Tqual f2; void* f3; } ; static void* Cyc_Tcutil_fst_fdarg(
struct _tuple14* t){ return(* t).f1;} void* Cyc_Tcutil_snd_tqt( struct _tuple4*
t){ return(* t).f2;} static struct _tuple4* Cyc_Tcutil_map2_tq( struct _tuple4*
pr, void* t){ return({ struct _tuple4* _temp1961=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1961->f1=(* pr).f1; _temp1961->f2= t; _temp1961;});}
void Cyc_Tcutil_check_fndecl_valid_type( struct Cyc_Position_Segment* loc,
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl* fd){ void* t= Cyc_Tcutil_fndecl2typ(
fd); Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);{ void* _temp1962= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_FnInfo _temp1968; struct Cyc_List_List* _temp1970; struct
Cyc_List_List* _temp1972; struct Cyc_Absyn_VarargInfo* _temp1974; int _temp1976;
struct Cyc_List_List* _temp1978; void* _temp1980; struct Cyc_Core_Opt* _temp1982;
struct Cyc_List_List* _temp1984; _LL1964: if(( unsigned int) _temp1962 > 4u?*((
int*) _temp1962) == Cyc_Absyn_FnType: 0){ _LL1969: _temp1968=(( struct Cyc_Absyn_FnType_struct*)
_temp1962)->f1; _LL1985: _temp1984= _temp1968.tvars; goto _LL1983; _LL1983:
_temp1982= _temp1968.effect; goto _LL1981; _LL1981: _temp1980=( void*) _temp1968.ret_typ;
goto _LL1979; _LL1979: _temp1978= _temp1968.args; goto _LL1977; _LL1977:
_temp1976= _temp1968.c_varargs; goto _LL1975; _LL1975: _temp1974= _temp1968.cyc_varargs;
goto _LL1973; _LL1973: _temp1972= _temp1968.rgn_po; goto _LL1971; _LL1971:
_temp1970= _temp1968.attributes; goto _LL1965;} else{ goto _LL1966;} _LL1966:
goto _LL1967; _LL1965: fd->tvs= _temp1984; fd->effect= _temp1982; goto _LL1963;
_LL1967:({ void* _temp1986[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("check_fndecl_valid_type: not a FnType",
sizeof( unsigned char), 38u), _tag_arr( _temp1986, sizeof( void*), 0u));});
return; _LL1963:;}{ struct _RegionHandle _temp1987= _new_region(); struct
_RegionHandle* r=& _temp1987; _push_region( r); Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tagged_arr*(* f)(
struct _tuple13*), struct Cyc_List_List* x)) Cyc_List_rmap)( r,( struct
_tagged_arr*(*)( struct _tuple13* t)) Cyc_Tcutil_fst_fdarg, fd->args), loc,
_tag_arr("function declaration has repeated parameter", sizeof( unsigned char),
44u));; _pop_region( r);} fd->cached_typ=({ struct Cyc_Core_Opt* _temp1988=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1988->v=(
void*) t; _temp1988;});} struct _tuple15{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual
f2; } ; struct _tuple16{ struct _tuple15* f1; void* f2; } ; static struct
_tuple16* Cyc_Tcutil_substitute_f1( struct _RegionHandle* rgn, struct _tuple2* y){
return({ struct _tuple16* _temp1989=( struct _tuple16*) _region_malloc( rgn,
sizeof( struct _tuple16)); _temp1989->f1=({ struct _tuple15* _temp1990=( struct
_tuple15*) _region_malloc( rgn, sizeof( struct _tuple15)); _temp1990->f1=(* y).f1;
_temp1990->f2=(* y).f2; _temp1990;}); _temp1989->f2=(* y).f3; _temp1989;});}
static struct _tuple2* Cyc_Tcutil_substitute_f2( struct _tuple16* w){ void*
_temp1993; struct _tuple15* _temp1995; struct _tuple16 _temp1991=* w; _LL1996:
_temp1995= _temp1991.f1; goto _LL1994; _LL1994: _temp1993= _temp1991.f2; goto
_LL1992; _LL1992: { struct Cyc_Absyn_Tqual _temp1999; struct Cyc_Core_Opt*
_temp2001; struct _tuple15 _temp1997=* _temp1995; _LL2002: _temp2001= _temp1997.f1;
goto _LL2000; _LL2000: _temp1999= _temp1997.f2; goto _LL1998; _LL1998: return({
struct _tuple2* _temp2003=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2003->f1= _temp2001; _temp2003->f2= _temp1999; _temp2003->f3= _temp1993;
_temp2003;});}} static void* Cyc_Tcutil_field_type( struct Cyc_Absyn_Structfield*
f){ return( void*) f->type;} static struct Cyc_Absyn_Structfield* Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Structfield* f, void* t){ return({ struct Cyc_Absyn_Structfield*
_temp2004=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2004->name= f->name; _temp2004->tq= f->tq; _temp2004->type=( void*) t;
_temp2004->width= f->width; _temp2004->attributes= f->attributes; _temp2004;});}
static struct Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn,
struct Cyc_List_List* inst, struct Cyc_List_List* ts); void* Cyc_Tcutil_rsubstitute(
struct _RegionHandle* rgn, struct Cyc_List_List* inst, void* t){ void* _temp2005=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar* _temp2051; struct Cyc_Absyn_Structdecl**
_temp2053; struct Cyc_List_List* _temp2055; struct _tuple1* _temp2057; struct
Cyc_Absyn_Uniondecl** _temp2059; struct Cyc_List_List* _temp2061; struct _tuple1*
_temp2063; struct Cyc_Absyn_TunionInfo _temp2065; void* _temp2067; struct Cyc_List_List*
_temp2069; void* _temp2071; struct Cyc_Absyn_TunionFieldInfo _temp2073; struct
Cyc_List_List* _temp2075; void* _temp2077; struct Cyc_Core_Opt* _temp2079;
struct Cyc_List_List* _temp2081; struct _tuple1* _temp2083; struct Cyc_Absyn_Exp*
_temp2085; struct Cyc_Absyn_Tqual _temp2087; void* _temp2089; struct Cyc_Absyn_PtrInfo
_temp2091; struct Cyc_Absyn_Conref* _temp2093; struct Cyc_Absyn_Tqual _temp2095;
struct Cyc_Absyn_Conref* _temp2097; void* _temp2099; void* _temp2101; struct Cyc_Absyn_FnInfo
_temp2103; struct Cyc_List_List* _temp2105; struct Cyc_List_List* _temp2107;
struct Cyc_Absyn_VarargInfo* _temp2109; int _temp2111; struct Cyc_List_List*
_temp2113; void* _temp2115; struct Cyc_Core_Opt* _temp2117; struct Cyc_List_List*
_temp2119; struct Cyc_List_List* _temp2121; struct Cyc_List_List* _temp2123;
struct Cyc_List_List* _temp2125; struct Cyc_Core_Opt* _temp2127; void* _temp2129;
void* _temp2131; struct Cyc_List_List* _temp2133; _LL2007: if(( unsigned int)
_temp2005 > 4u?*(( int*) _temp2005) == Cyc_Absyn_VarType: 0){ _LL2052: _temp2051=((
struct Cyc_Absyn_VarType_struct*) _temp2005)->f1; goto _LL2008;} else{ goto
_LL2009;} _LL2009: if(( unsigned int) _temp2005 > 4u?*(( int*) _temp2005) == Cyc_Absyn_StructType:
0){ _LL2058: _temp2057=(( struct Cyc_Absyn_StructType_struct*) _temp2005)->f1;
goto _LL2056; _LL2056: _temp2055=(( struct Cyc_Absyn_StructType_struct*)
_temp2005)->f2; goto _LL2054; _LL2054: _temp2053=(( struct Cyc_Absyn_StructType_struct*)
_temp2005)->f3; goto _LL2010;} else{ goto _LL2011;} _LL2011: if(( unsigned int)
_temp2005 > 4u?*(( int*) _temp2005) == Cyc_Absyn_UnionType: 0){ _LL2064:
_temp2063=(( struct Cyc_Absyn_UnionType_struct*) _temp2005)->f1; goto _LL2062;
_LL2062: _temp2061=(( struct Cyc_Absyn_UnionType_struct*) _temp2005)->f2; goto
_LL2060; _LL2060: _temp2059=(( struct Cyc_Absyn_UnionType_struct*) _temp2005)->f3;
goto _LL2012;} else{ goto _LL2013;} _LL2013: if(( unsigned int) _temp2005 > 4u?*((
int*) _temp2005) == Cyc_Absyn_TunionType: 0){ _LL2066: _temp2065=(( struct Cyc_Absyn_TunionType_struct*)
_temp2005)->f1; _LL2072: _temp2071=( void*) _temp2065.tunion_info; goto _LL2070;
_LL2070: _temp2069= _temp2065.targs; goto _LL2068; _LL2068: _temp2067=( void*)
_temp2065.rgn; goto _LL2014;} else{ goto _LL2015;} _LL2015: if(( unsigned int)
_temp2005 > 4u?*(( int*) _temp2005) == Cyc_Absyn_TunionFieldType: 0){ _LL2074:
_temp2073=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2005)->f1; _LL2078:
_temp2077=( void*) _temp2073.field_info; goto _LL2076; _LL2076: _temp2075=
_temp2073.targs; goto _LL2016;} else{ goto _LL2017;} _LL2017: if(( unsigned int)
_temp2005 > 4u?*(( int*) _temp2005) == Cyc_Absyn_TypedefType: 0){ _LL2084:
_temp2083=(( struct Cyc_Absyn_TypedefType_struct*) _temp2005)->f1; goto _LL2082;
_LL2082: _temp2081=(( struct Cyc_Absyn_TypedefType_struct*) _temp2005)->f2; goto
_LL2080; _LL2080: _temp2079=(( struct Cyc_Absyn_TypedefType_struct*) _temp2005)->f3;
goto _LL2018;} else{ goto _LL2019;} _LL2019: if(( unsigned int) _temp2005 > 4u?*((
int*) _temp2005) == Cyc_Absyn_ArrayType: 0){ _LL2090: _temp2089=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2005)->f1; goto _LL2088; _LL2088:
_temp2087=(( struct Cyc_Absyn_ArrayType_struct*) _temp2005)->f2; goto _LL2086;
_LL2086: _temp2085=(( struct Cyc_Absyn_ArrayType_struct*) _temp2005)->f3; goto
_LL2020;} else{ goto _LL2021;} _LL2021: if(( unsigned int) _temp2005 > 4u?*((
int*) _temp2005) == Cyc_Absyn_PointerType: 0){ _LL2092: _temp2091=(( struct Cyc_Absyn_PointerType_struct*)
_temp2005)->f1; _LL2102: _temp2101=( void*) _temp2091.elt_typ; goto _LL2100;
_LL2100: _temp2099=( void*) _temp2091.rgn_typ; goto _LL2098; _LL2098: _temp2097=
_temp2091.nullable; goto _LL2096; _LL2096: _temp2095= _temp2091.tq; goto _LL2094;
_LL2094: _temp2093= _temp2091.bounds; goto _LL2022;} else{ goto _LL2023;}
_LL2023: if(( unsigned int) _temp2005 > 4u?*(( int*) _temp2005) == Cyc_Absyn_FnType:
0){ _LL2104: _temp2103=(( struct Cyc_Absyn_FnType_struct*) _temp2005)->f1;
_LL2120: _temp2119= _temp2103.tvars; goto _LL2118; _LL2118: _temp2117= _temp2103.effect;
goto _LL2116; _LL2116: _temp2115=( void*) _temp2103.ret_typ; goto _LL2114;
_LL2114: _temp2113= _temp2103.args; goto _LL2112; _LL2112: _temp2111= _temp2103.c_varargs;
goto _LL2110; _LL2110: _temp2109= _temp2103.cyc_varargs; goto _LL2108; _LL2108:
_temp2107= _temp2103.rgn_po; goto _LL2106; _LL2106: _temp2105= _temp2103.attributes;
goto _LL2024;} else{ goto _LL2025;} _LL2025: if(( unsigned int) _temp2005 > 4u?*((
int*) _temp2005) == Cyc_Absyn_TupleType: 0){ _LL2122: _temp2121=(( struct Cyc_Absyn_TupleType_struct*)
_temp2005)->f1; goto _LL2026;} else{ goto _LL2027;} _LL2027: if(( unsigned int)
_temp2005 > 4u?*(( int*) _temp2005) == Cyc_Absyn_AnonStructType: 0){ _LL2124:
_temp2123=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2005)->f1; goto
_LL2028;} else{ goto _LL2029;} _LL2029: if(( unsigned int) _temp2005 > 4u?*((
int*) _temp2005) == Cyc_Absyn_AnonUnionType: 0){ _LL2126: _temp2125=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp2005)->f1; goto _LL2030;} else{ goto
_LL2031;} _LL2031: if(( unsigned int) _temp2005 > 4u?*(( int*) _temp2005) == Cyc_Absyn_Evar:
0){ _LL2128: _temp2127=(( struct Cyc_Absyn_Evar_struct*) _temp2005)->f2; goto
_LL2032;} else{ goto _LL2033;} _LL2033: if(( unsigned int) _temp2005 > 4u?*((
int*) _temp2005) == Cyc_Absyn_RgnHandleType: 0){ _LL2130: _temp2129=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp2005)->f1; goto _LL2034;} else{
goto _LL2035;} _LL2035: if(( unsigned int) _temp2005 > 4u?*(( int*) _temp2005)
== Cyc_Absyn_EnumType: 0){ goto _LL2036;} else{ goto _LL2037;} _LL2037: if(
_temp2005 ==( void*) Cyc_Absyn_VoidType){ goto _LL2038;} else{ goto _LL2039;}
_LL2039: if(( unsigned int) _temp2005 > 4u?*(( int*) _temp2005) == Cyc_Absyn_IntType:
0){ goto _LL2040;} else{ goto _LL2041;} _LL2041: if( _temp2005 ==( void*) Cyc_Absyn_FloatType){
goto _LL2042;} else{ goto _LL2043;} _LL2043: if( _temp2005 ==( void*) Cyc_Absyn_DoubleType){
goto _LL2044;} else{ goto _LL2045;} _LL2045: if( _temp2005 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL2046;} else{ goto _LL2047;} _LL2047: if(( unsigned int) _temp2005 > 4u?*((
int*) _temp2005) == Cyc_Absyn_AccessEff: 0){ _LL2132: _temp2131=( void*)((
struct Cyc_Absyn_AccessEff_struct*) _temp2005)->f1; goto _LL2048;} else{ goto
_LL2049;} _LL2049: if(( unsigned int) _temp2005 > 4u?*(( int*) _temp2005) == Cyc_Absyn_JoinEff:
0){ _LL2134: _temp2133=(( struct Cyc_Absyn_JoinEff_struct*) _temp2005)->f1; goto
_LL2050;} else{ goto _LL2006;} _LL2008: { struct _handler_cons _temp2135;
_push_handler(& _temp2135);{ int _temp2137= 0; if( setjmp( _temp2135.handler)){
_temp2137= 1;} if( ! _temp2137){{ void* _temp2138=(( void*(*)( int(* compare)(
struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct
Cyc_Absyn_Tvar* x)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp, inst, _temp2051);
_npop_handler( 0u); return _temp2138;}; _pop_handler();} else{ void* _temp2136=(
void*) _exn_thrown; void* _temp2140= _temp2136; _LL2142: if( _temp2140 == Cyc_Core_Not_found){
goto _LL2143;} else{ goto _LL2144;} _LL2144: goto _LL2145; _LL2143: return t;
_LL2145:( void) _throw( _temp2140); _LL2141:;}}} _LL2010: { struct Cyc_List_List*
_temp2146= Cyc_Tcutil_substs( rgn, inst, _temp2055); return _temp2146 ==
_temp2055? t:( void*)({ struct Cyc_Absyn_StructType_struct* _temp2147=( struct
Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp2147[ 0]=({ struct Cyc_Absyn_StructType_struct _temp2148; _temp2148.tag=
Cyc_Absyn_StructType; _temp2148.f1= _temp2057; _temp2148.f2= _temp2146;
_temp2148.f3= _temp2053; _temp2148;}); _temp2147;});} _LL2012: { struct Cyc_List_List*
_temp2149= Cyc_Tcutil_substs( rgn, inst, _temp2061); return _temp2149 ==
_temp2061? t:( void*)({ struct Cyc_Absyn_UnionType_struct* _temp2150=( struct
Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp2150[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp2151; _temp2151.tag= Cyc_Absyn_UnionType;
_temp2151.f1= _temp2063; _temp2151.f2= _temp2149; _temp2151.f3= _temp2059;
_temp2151;}); _temp2150;});} _LL2014: { struct Cyc_List_List* _temp2152= Cyc_Tcutil_substs(
rgn, inst, _temp2069); void* _temp2153= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2067); return( _temp2152 == _temp2069? _temp2153 == _temp2067: 0)? t:( void*)({
struct Cyc_Absyn_TunionType_struct* _temp2154=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2154[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2155; _temp2155.tag= Cyc_Absyn_TunionType;
_temp2155.f1=({ struct Cyc_Absyn_TunionInfo _temp2156; _temp2156.tunion_info=(
void*) _temp2071; _temp2156.targs= _temp2152; _temp2156.rgn=( void*) _temp2153;
_temp2156;}); _temp2155;}); _temp2154;});} _LL2016: { struct Cyc_List_List*
_temp2157= Cyc_Tcutil_substs( rgn, inst, _temp2075); return _temp2157 ==
_temp2075? t:( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp2158=(
struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp2158[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp2159; _temp2159.tag=
Cyc_Absyn_TunionFieldType; _temp2159.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2160; _temp2160.field_info=( void*) _temp2077; _temp2160.targs= _temp2157;
_temp2160;}); _temp2159;}); _temp2158;});} _LL2018: { struct Cyc_List_List*
_temp2161= Cyc_Tcutil_substs( rgn, inst, _temp2081); return _temp2161 ==
_temp2081? t:( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp2162=( struct
Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2162[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp2163; _temp2163.tag=
Cyc_Absyn_TypedefType; _temp2163.f1= _temp2083; _temp2163.f2= _temp2161;
_temp2163.f3= _temp2079; _temp2163;}); _temp2162;});} _LL2020: { void* _temp2164=
Cyc_Tcutil_rsubstitute( rgn, inst, _temp2089); return _temp2164 == _temp2089? t:(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp2165=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2165[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2166; _temp2166.tag= Cyc_Absyn_ArrayType;
_temp2166.f1=( void*) _temp2164; _temp2166.f2= _temp2087; _temp2166.f3=
_temp2085; _temp2166;}); _temp2165;});} _LL2022: { void* _temp2167= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2101); void* _temp2168= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2099); if( _temp2167 == _temp2101? _temp2168 == _temp2099: 0){ return t;}
return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp2169=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp2169[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp2170; _temp2170.tag= Cyc_Absyn_PointerType;
_temp2170.f1=({ struct Cyc_Absyn_PtrInfo _temp2171; _temp2171.elt_typ=( void*)
_temp2167; _temp2171.rgn_typ=( void*) _temp2168; _temp2171.nullable= _temp2097;
_temp2171.tq= _temp2095; _temp2171.bounds= _temp2093; _temp2171;}); _temp2170;});
_temp2169;});} _LL2024:{ struct Cyc_List_List* _temp2172= _temp2119; for( 0;
_temp2172 != 0; _temp2172=(( struct Cyc_List_List*) _check_null( _temp2172))->tl){
inst=({ struct Cyc_List_List* _temp2173=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2173->hd=( void*)({ struct _tuple5*
_temp2174=( struct _tuple5*) _region_malloc( rgn, sizeof( struct _tuple5));
_temp2174->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp2172))->hd; _temp2174->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2175=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2175[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2176; _temp2176.tag= Cyc_Absyn_VarType;
_temp2176.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp2172))->hd; _temp2176;}); _temp2175;}); _temp2174;}); _temp2173->tl= inst;
_temp2173;});}}{ struct Cyc_List_List* _temp2179; struct Cyc_List_List*
_temp2181; struct _tuple0 _temp2177=(( struct _tuple0(*)( struct _RegionHandle*
r3, struct _RegionHandle* r4, struct Cyc_List_List* x)) Cyc_List_rsplit)( rgn,
rgn,(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple16*(* f)(
struct _RegionHandle*, struct _tuple2*), struct _RegionHandle* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_substitute_f1, rgn, _temp2113)); _LL2182:
_temp2181= _temp2177.f1; goto _LL2180; _LL2180: _temp2179= _temp2177.f2; goto
_LL2178; _LL2178: { struct Cyc_List_List* _temp2183= Cyc_Tcutil_substs( rgn,
inst, _temp2179); struct Cyc_List_List* _temp2184=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple16*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_substitute_f2,(( struct Cyc_List_List*(*)( struct _RegionHandle* r3,
struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp2181, _temp2183)); struct Cyc_Core_Opt* eff2; if( _temp2117 == 0){
eff2= 0;} else{ void* _temp2185= Cyc_Tcutil_rsubstitute( rgn, inst,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2117))->v); if( _temp2185 ==( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2117))->v){ eff2= _temp2117;} else{ eff2=({
struct Cyc_Core_Opt* _temp2186=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2186->v=( void*) _temp2185; _temp2186;});}}{ struct Cyc_Absyn_VarargInfo*
cyc_varargs2; if( _temp2109 == 0){ cyc_varargs2= 0;} else{ int _temp2189; void*
_temp2191; void* _temp2193; struct Cyc_Absyn_Tqual _temp2195; struct Cyc_Core_Opt*
_temp2197; struct Cyc_Absyn_VarargInfo _temp2187=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp2109)); _LL2198: _temp2197= _temp2187.name; goto _LL2196;
_LL2196: _temp2195= _temp2187.tq; goto _LL2194; _LL2194: _temp2193=( void*)
_temp2187.type; goto _LL2192; _LL2192: _temp2191=( void*) _temp2187.rgn; goto
_LL2190; _LL2190: _temp2189= _temp2187.inject; goto _LL2188; _LL2188: { void*
_temp2199= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2193); void* _temp2200= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2191); if( _temp2199 == _temp2193? _temp2191 == _temp2200: 0){
cyc_varargs2= _temp2109;} else{ cyc_varargs2=({ struct Cyc_Absyn_VarargInfo*
_temp2201=( struct Cyc_Absyn_VarargInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2201->name= _temp2197; _temp2201->tq= _temp2195; _temp2201->type=( void*)
_temp2199; _temp2201->rgn=( void*) _temp2200; _temp2201->inject= _temp2189;
_temp2201;});}}}{ struct Cyc_List_List* rgn_po2; struct Cyc_List_List* _temp2204;
struct Cyc_List_List* _temp2206; struct _tuple0 _temp2202= Cyc_List_rsplit( rgn,
rgn, _temp2107); _LL2207: _temp2206= _temp2202.f1; goto _LL2205; _LL2205:
_temp2204= _temp2202.f2; goto _LL2203; _LL2203: { struct Cyc_List_List*
_temp2208= Cyc_Tcutil_substs( rgn, inst, _temp2206); struct Cyc_List_List*
_temp2209= Cyc_Tcutil_substs( rgn, inst, _temp2204); if( _temp2208 == _temp2206?
_temp2209 == _temp2204: 0){ rgn_po2= _temp2107;} else{ rgn_po2= Cyc_List_zip(
_temp2208, _temp2209);} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2210=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2210[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2211; _temp2211.tag= Cyc_Absyn_FnType;
_temp2211.f1=({ struct Cyc_Absyn_FnInfo _temp2212; _temp2212.tvars= _temp2119;
_temp2212.effect= eff2; _temp2212.ret_typ=( void*) Cyc_Tcutil_rsubstitute( rgn,
inst, _temp2115); _temp2212.args= _temp2184; _temp2212.c_varargs= _temp2111;
_temp2212.cyc_varargs= cyc_varargs2; _temp2212.rgn_po= rgn_po2; _temp2212.attributes=
_temp2105; _temp2212;}); _temp2211;}); _temp2210;});}}}}} _LL2026: { struct Cyc_List_List*
_temp2213=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_snd_tqt,
_temp2121); struct Cyc_List_List* _temp2214= Cyc_Tcutil_substs( rgn, inst,
_temp2213); if( _temp2214 == _temp2213){ return t;}{ struct Cyc_List_List*
_temp2215=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*,
void*), struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_map2)( Cyc_Tcutil_map2_tq,
_temp2121, _temp2214); return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp2216=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp2216[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp2217; _temp2217.tag= Cyc_Absyn_TupleType;
_temp2217.f1= _temp2215; _temp2217;}); _temp2216;});}} _LL2028: { struct Cyc_List_List*
_temp2218=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2123); struct Cyc_List_List* _temp2219= Cyc_Tcutil_substs( rgn, inst,
_temp2218); if( _temp2219 == _temp2218){ return t;}{ struct Cyc_List_List*
_temp2220=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2123, _temp2219); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp2221=( struct Cyc_Absyn_AnonStructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2221[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2222; _temp2222.tag= Cyc_Absyn_AnonStructType;
_temp2222.f1= _temp2220; _temp2222;}); _temp2221;});}} _LL2030: { struct Cyc_List_List*
_temp2223=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2125); struct Cyc_List_List* _temp2224= Cyc_Tcutil_substs( rgn, inst,
_temp2223); if( _temp2224 == _temp2223){ return t;}{ struct Cyc_List_List*
_temp2225=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2125, _temp2224); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp2226=( struct Cyc_Absyn_AnonStructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2226[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2227; _temp2227.tag= Cyc_Absyn_AnonStructType;
_temp2227.f1= _temp2225; _temp2227;}); _temp2226;});}} _LL2032: if( _temp2127 !=
0){ return Cyc_Tcutil_rsubstitute( rgn, inst,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2127))->v);} else{ return t;} _LL2034: { void* _temp2228= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2129); return _temp2228 == _temp2129? t:( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp2229=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp2229[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp2230; _temp2230.tag= Cyc_Absyn_RgnHandleType; _temp2230.f1=( void*)
_temp2228; _temp2230;}); _temp2229;});} _LL2036: return t; _LL2038: return t;
_LL2040: return t; _LL2042: return t; _LL2044: return t; _LL2046: return t;
_LL2048: { void* _temp2231= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2131);
return _temp2231 == _temp2131? t:( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp2232=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2232[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2233; _temp2233.tag= Cyc_Absyn_AccessEff;
_temp2233.f1=( void*) _temp2231; _temp2233;}); _temp2232;});} _LL2050: { struct
Cyc_List_List* _temp2234= Cyc_Tcutil_substs( rgn, inst, _temp2133); return
_temp2234 == _temp2133? t:( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2235=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2235[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2236; _temp2236.tag= Cyc_Absyn_JoinEff;
_temp2236.f1= _temp2234; _temp2236;}); _temp2235;});} _LL2006:;} static struct
Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List*
inst, struct Cyc_List_List* ts){ if( ts == 0){ return 0;}{ void* _temp2237=(
void*)(( struct Cyc_List_List*) _check_null( ts))->hd; struct Cyc_List_List*
_temp2238=(( struct Cyc_List_List*) _check_null( ts))->tl; void* _temp2239= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2237); struct Cyc_List_List* _temp2240= Cyc_Tcutil_substs( rgn,
inst, _temp2238); if( _temp2237 == _temp2239? _temp2238 == _temp2240: 0){ return
ts;} return( struct Cyc_List_List*)({ struct Cyc_List_List* _temp2241=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2241->hd=( void*)
_temp2239; _temp2241->tl= _temp2240; _temp2241;});}} extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List* inst, void* t){ return Cyc_Tcutil_rsubstitute( Cyc_Core_heap_region,
inst, t);} struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2286=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2286[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2287; _temp2287.tag= Cyc_Absyn_Const_e; _temp2287.f1=(
void*)(( void*) Cyc_Absyn_Null_c); _temp2287;}); _temp2286;}), loc);{ void*
_temp2242= Cyc_Tcutil_compress( t); void* _temp2256; void* _temp2258; void*
_temp2260; void* _temp2262; _LL2244: if(( unsigned int) _temp2242 > 4u?*(( int*)
_temp2242) == Cyc_Absyn_PointerType: 0){ goto _LL2245;} else{ goto _LL2246;}
_LL2246: if(( unsigned int) _temp2242 > 4u?*(( int*) _temp2242) == Cyc_Absyn_IntType:
0){ _LL2259: _temp2258=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2242)->f1;
goto _LL2257; _LL2257: _temp2256=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2242)->f2; if( _temp2256 ==( void*) Cyc_Absyn_B1){ goto _LL2247;} else{
goto _LL2248;}} else{ goto _LL2248;} _LL2248: if(( unsigned int) _temp2242 > 4u?*((
int*) _temp2242) == Cyc_Absyn_IntType: 0){ _LL2263: _temp2262=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2242)->f1; goto _LL2261; _LL2261: _temp2260=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2242)->f2; goto _LL2249;} else{
goto _LL2250;} _LL2250: if( _temp2242 ==( void*) Cyc_Absyn_FloatType){ goto
_LL2251;} else{ goto _LL2252;} _LL2252: if( _temp2242 ==( void*) Cyc_Absyn_DoubleType){
goto _LL2253;} else{ goto _LL2254;} _LL2254: goto _LL2255; _LL2245: goto _LL2243;
_LL2247:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2264=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2264[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2265; _temp2265.tag= Cyc_Absyn_Const_e;
_temp2265.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp2266=(
struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp2266[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp2267; _temp2267.tag= Cyc_Absyn_Char_c;
_temp2267.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2267.f2='\000'; _temp2267;});
_temp2266;})); _temp2265;}); _temp2264;}))); goto _LL2243; _LL2249:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2268=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2268[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2269; _temp2269.tag= Cyc_Absyn_Const_e; _temp2269.f1=(
void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp2270=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp2270[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp2271; _temp2271.tag= Cyc_Absyn_Int_c; _temp2271.f1=( void*) _temp2262;
_temp2271.f2= 0; _temp2271;}); _temp2270;})); _temp2269;}); _temp2268;}))); if(
_temp2260 !=( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp2272=( struct Cyc_Absyn_Cast_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp2272[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp2273; _temp2273.tag= Cyc_Absyn_Cast_e;
_temp2273.f1=( void*) t; _temp2273.f2= e; _temp2273;}); _temp2272;}), loc);}
goto _LL2243; _LL2251:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2274=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2274[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2275; _temp2275.tag= Cyc_Absyn_Const_e;
_temp2275.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2276=(
struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2276[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2277; _temp2277.tag= Cyc_Absyn_Float_c;
_temp2277.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp2277;});
_temp2276;})); _temp2275;}); _temp2274;}))); goto _LL2243; _LL2253:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2278=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2278[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2279; _temp2279.tag= Cyc_Absyn_Cast_e; _temp2279.f1=( void*) t; _temp2279.f2=
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2280=( struct
Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2280[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2281; _temp2281.tag= Cyc_Absyn_Const_e;
_temp2281.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2282=(
struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2282[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2283; _temp2283.tag= Cyc_Absyn_Float_c;
_temp2283.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp2283;});
_temp2282;})); _temp2281;}); _temp2280;}), loc); _temp2279;}); _temp2278;})));
goto _LL2243; _LL2255:({ struct Cyc_Stdio_String_pa_struct _temp2285; _temp2285.tag=
Cyc_Stdio_String_pa; _temp2285.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp2284[ 1u]={& _temp2285}; Cyc_Tcutil_terr( loc, _tag_arr("declaration of type %s requires initializer",
sizeof( unsigned char), 44u), _tag_arr( _temp2284, sizeof( void*), 1u));}});
goto _LL2243; _LL2243:;} return e;} struct _tuple5* Cyc_Tcutil_make_inst_var(
struct Cyc_List_List* s, struct Cyc_Absyn_Tvar* tv){ void* k= Cyc_Absyn_conref_val(
tv->kind); return({ struct _tuple5* _temp2288=( struct _tuple5*) GC_malloc(
sizeof( struct _tuple5)); _temp2288->f1= tv; _temp2288->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp2289=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2289->v=( void*) k; _temp2289;}),({ struct Cyc_Core_Opt*
_temp2290=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2290->v=( void*) s; _temp2290;})); _temp2288;});} struct _tuple5* Cyc_Tcutil_r_make_inst_var(
struct _tuple6* env, struct Cyc_Absyn_Tvar* tv){ struct _tuple6 _temp2293;
struct _RegionHandle* _temp2294; struct Cyc_List_List* _temp2296; struct _tuple6*
_temp2291= env; _temp2293=* _temp2291; _LL2297: _temp2296= _temp2293.f1; goto
_LL2295; _LL2295: _temp2294= _temp2293.f2; goto _LL2292; _LL2292: { void* k= Cyc_Absyn_conref_val(
tv->kind); return({ struct _tuple5* _temp2298=( struct _tuple5*) _region_malloc(
_temp2294, sizeof( struct _tuple5)); _temp2298->f1= tv; _temp2298->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp2299=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2299->v=( void*) k; _temp2299;}),({ struct Cyc_Core_Opt*
_temp2300=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2300->v=( void*) _temp2296; _temp2300;})); _temp2298;});}} static struct
Cyc_List_List* Cyc_Tcutil_add_free_tvar( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 != 0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
if( Cyc_String_zstrptrcmp((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->name, tv->name) == 0){ struct Cyc_Absyn_Conref* k1=((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->kind;
struct Cyc_Absyn_Conref* k2= tv->kind; if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){({
struct Cyc_Stdio_String_pa_struct _temp2304; _temp2304.tag= Cyc_Stdio_String_pa;
_temp2304.f1=( struct _tagged_arr) Cyc_Absynpp_ckind2string( k2);{ struct Cyc_Stdio_String_pa_struct
_temp2303; _temp2303.tag= Cyc_Stdio_String_pa; _temp2303.f1=( struct _tagged_arr)
Cyc_Absynpp_ckind2string( k1);{ struct Cyc_Stdio_String_pa_struct _temp2302;
_temp2302.tag= Cyc_Stdio_String_pa; _temp2302.f1=( struct _tagged_arr)* tv->name;{
void* _temp2301[ 3u]={& _temp2302,& _temp2303,& _temp2304}; Cyc_Tcutil_terr( loc,
_tag_arr("type variable %s is used with inconsistent kinds %s and %s", sizeof(
unsigned char), 59u), _tag_arr( _temp2301, sizeof( void*), 3u));}}}});} if( tv->identity
== 0){ tv->identity=(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity;} else{ if(*(( int*) _check_null( tv->identity))
!=*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity))){({ void* _temp2305[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("same type variable has different identity!",
sizeof( unsigned char), 43u), _tag_arr( _temp2305, sizeof( void*), 0u));});}}
return tvs;}}} tv->identity= Cyc_Tcutil_new_tvar_id(); return({ struct Cyc_List_List*
_temp2306=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2306->hd=( void*) tv; _temp2306->tl= tvs; _temp2306;});} static struct Cyc_List_List*
Cyc_Tcutil_fast_add_free_tvar( struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar*
tv){ if( tv->identity == 0){({ void* _temp2307[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("fast_add_free_tvar: bad identity in tv",
sizeof( unsigned char), 39u), _tag_arr( _temp2307, sizeof( void*), 0u));});}{
struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 != 0; tvs2=(( struct Cyc_List_List*)
_check_null( tvs2))->tl){ if((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity == 0){({ void* _temp2308[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("fast_add_free_tvar: bad identity in tvs2",
sizeof( unsigned char), 41u), _tag_arr( _temp2308, sizeof( void*), 0u));});} if(*((
int*) _check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity)) ==*(( int*) _check_null( tv->identity))){
return tvs;}}} return({ struct Cyc_List_List* _temp2309=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2309->hd=( void*) tv; _temp2309->tl=
tvs; _temp2309;});} static struct Cyc_List_List* Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){ if( tv->identity == 0){({
struct Cyc_Stdio_String_pa_struct _temp2311; _temp2311.tag= Cyc_Stdio_String_pa;
_temp2311.f1=( struct _tagged_arr) Cyc_Tcutil_tvar2string( tv);{ void* _temp2310[
1u]={& _temp2311};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("bound tvar id for %s is null", sizeof( unsigned char), 29u), _tag_arr(
_temp2310, sizeof( void*), 1u));}});} return({ struct Cyc_List_List* _temp2312=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2312->hd=(
void*) tv; _temp2312->tl= tvs; _temp2312;});} static struct Cyc_List_List* Cyc_Tcutil_add_free_evar(
struct Cyc_List_List* es, void* e){ void* _temp2313= Cyc_Tcutil_compress( e);
int _temp2319; _LL2315: if(( unsigned int) _temp2313 > 4u?*(( int*) _temp2313)
== Cyc_Absyn_Evar: 0){ _LL2320: _temp2319=(( struct Cyc_Absyn_Evar_struct*)
_temp2313)->f3; goto _LL2316;} else{ goto _LL2317;} _LL2317: goto _LL2318;
_LL2316:{ struct Cyc_List_List* es2= es; for( 0; es2 != 0; es2=(( struct Cyc_List_List*)
_check_null( es2))->tl){ void* _temp2321= Cyc_Tcutil_compress(( void*)(( struct
Cyc_List_List*) _check_null( es2))->hd); int _temp2327; _LL2323: if((
unsigned int) _temp2321 > 4u?*(( int*) _temp2321) == Cyc_Absyn_Evar: 0){ _LL2328:
_temp2327=(( struct Cyc_Absyn_Evar_struct*) _temp2321)->f3; goto _LL2324;} else{
goto _LL2325;} _LL2325: goto _LL2326; _LL2324: if( _temp2319 == _temp2327){
return es;} goto _LL2322; _LL2326: goto _LL2322; _LL2322:;}} return({ struct Cyc_List_List*
_temp2329=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2329->hd=( void*) e; _temp2329->tl= es; _temp2329;}); _LL2318: return es;
_LL2314:;} static struct Cyc_List_List* Cyc_Tcutil_remove_bound_tvars( struct
Cyc_List_List* tvs, struct Cyc_List_List* btvs){ struct Cyc_List_List* r= 0;
for( 0; tvs != 0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ int
present= 0;{ struct Cyc_List_List* b= btvs; for( 0; b != 0; b=(( struct Cyc_List_List*)
_check_null( b))->tl){ if(*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->identity)) ==*(( int*)
_check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd)->identity))){
present= 1; break;}}} if( ! present){ r=({ struct Cyc_List_List* _temp2330=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2330->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd);
_temp2330->tl= r; _temp2330;});}} r=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( r); return r;} struct Cyc_Tcutil_CVTEnv{ struct Cyc_List_List*
kind_env; struct Cyc_List_List* free_vars; struct Cyc_List_List* free_evars; } ;
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcutil_CVTEnv cvtenv, void*
expected_kind, void* t){{ void* _temp2331= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt*
_temp2377; struct Cyc_Core_Opt** _temp2379; struct Cyc_Absyn_Tvar* _temp2380;
struct Cyc_Absyn_Enumdecl* _temp2382; struct Cyc_Absyn_Enumdecl** _temp2384;
struct _tuple1* _temp2385; struct Cyc_Absyn_TunionInfo _temp2387; void*
_temp2389; struct Cyc_List_List* _temp2391; void* _temp2393; void** _temp2395;
struct Cyc_Absyn_TunionFieldInfo _temp2396; struct Cyc_List_List* _temp2398;
void* _temp2400; void** _temp2402; struct Cyc_Absyn_PtrInfo _temp2403; struct
Cyc_Absyn_Conref* _temp2405; struct Cyc_Absyn_Tqual _temp2407; struct Cyc_Absyn_Conref*
_temp2409; void* _temp2411; void* _temp2413; struct Cyc_Absyn_Exp* _temp2415;
struct Cyc_Absyn_Tqual _temp2417; void* _temp2419; struct Cyc_Absyn_FnInfo
_temp2421; struct Cyc_List_List* _temp2423; struct Cyc_List_List* _temp2425;
struct Cyc_Absyn_VarargInfo* _temp2427; int _temp2429; struct Cyc_List_List*
_temp2431; void* _temp2433; struct Cyc_Core_Opt* _temp2435; struct Cyc_Core_Opt**
_temp2437; struct Cyc_List_List* _temp2438; struct Cyc_List_List** _temp2440;
struct Cyc_List_List* _temp2441; struct Cyc_List_List* _temp2443; struct Cyc_List_List*
_temp2445; struct Cyc_Absyn_Structdecl** _temp2447; struct Cyc_Absyn_Structdecl***
_temp2449; struct Cyc_List_List* _temp2450; struct _tuple1* _temp2452; struct
Cyc_Absyn_Uniondecl** _temp2454; struct Cyc_Absyn_Uniondecl*** _temp2456; struct
Cyc_List_List* _temp2457; struct _tuple1* _temp2459; struct Cyc_Core_Opt*
_temp2461; struct Cyc_Core_Opt** _temp2463; struct Cyc_List_List* _temp2464;
struct _tuple1* _temp2466; void* _temp2468; void* _temp2470; struct Cyc_List_List*
_temp2472; _LL2333: if( _temp2331 ==( void*) Cyc_Absyn_VoidType){ goto _LL2334;}
else{ goto _LL2335;} _LL2335: if(( unsigned int) _temp2331 > 4u?*(( int*)
_temp2331) == Cyc_Absyn_Evar: 0){ _LL2378: _temp2377=(( struct Cyc_Absyn_Evar_struct*)
_temp2331)->f1; _temp2379=&(( struct Cyc_Absyn_Evar_struct*) _temp2331)->f1;
goto _LL2336;} else{ goto _LL2337;} _LL2337: if(( unsigned int) _temp2331 > 4u?*((
int*) _temp2331) == Cyc_Absyn_VarType: 0){ _LL2381: _temp2380=(( struct Cyc_Absyn_VarType_struct*)
_temp2331)->f1; goto _LL2338;} else{ goto _LL2339;} _LL2339: if(( unsigned int)
_temp2331 > 4u?*(( int*) _temp2331) == Cyc_Absyn_EnumType: 0){ _LL2386:
_temp2385=(( struct Cyc_Absyn_EnumType_struct*) _temp2331)->f1; goto _LL2383;
_LL2383: _temp2382=(( struct Cyc_Absyn_EnumType_struct*) _temp2331)->f2;
_temp2384=&(( struct Cyc_Absyn_EnumType_struct*) _temp2331)->f2; goto _LL2340;}
else{ goto _LL2341;} _LL2341: if(( unsigned int) _temp2331 > 4u?*(( int*)
_temp2331) == Cyc_Absyn_TunionType: 0){ _LL2388: _temp2387=(( struct Cyc_Absyn_TunionType_struct*)
_temp2331)->f1; _LL2394: _temp2393=( void*) _temp2387.tunion_info; _temp2395=&(((
struct Cyc_Absyn_TunionType_struct*) _temp2331)->f1).tunion_info; goto _LL2392;
_LL2392: _temp2391= _temp2387.targs; goto _LL2390; _LL2390: _temp2389=( void*)
_temp2387.rgn; goto _LL2342;} else{ goto _LL2343;} _LL2343: if(( unsigned int)
_temp2331 > 4u?*(( int*) _temp2331) == Cyc_Absyn_TunionFieldType: 0){ _LL2397:
_temp2396=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2331)->f1; _LL2401:
_temp2400=( void*) _temp2396.field_info; _temp2402=&((( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2331)->f1).field_info; goto _LL2399; _LL2399: _temp2398= _temp2396.targs;
goto _LL2344;} else{ goto _LL2345;} _LL2345: if(( unsigned int) _temp2331 > 4u?*((
int*) _temp2331) == Cyc_Absyn_PointerType: 0){ _LL2404: _temp2403=(( struct Cyc_Absyn_PointerType_struct*)
_temp2331)->f1; _LL2414: _temp2413=( void*) _temp2403.elt_typ; goto _LL2412;
_LL2412: _temp2411=( void*) _temp2403.rgn_typ; goto _LL2410; _LL2410: _temp2409=
_temp2403.nullable; goto _LL2408; _LL2408: _temp2407= _temp2403.tq; goto _LL2406;
_LL2406: _temp2405= _temp2403.bounds; goto _LL2346;} else{ goto _LL2347;}
_LL2347: if(( unsigned int) _temp2331 > 4u?*(( int*) _temp2331) == Cyc_Absyn_IntType:
0){ goto _LL2348;} else{ goto _LL2349;} _LL2349: if( _temp2331 ==( void*) Cyc_Absyn_FloatType){
goto _LL2350;} else{ goto _LL2351;} _LL2351: if( _temp2331 ==( void*) Cyc_Absyn_DoubleType){
goto _LL2352;} else{ goto _LL2353;} _LL2353: if(( unsigned int) _temp2331 > 4u?*((
int*) _temp2331) == Cyc_Absyn_ArrayType: 0){ _LL2420: _temp2419=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2331)->f1; goto _LL2418; _LL2418:
_temp2417=(( struct Cyc_Absyn_ArrayType_struct*) _temp2331)->f2; goto _LL2416;
_LL2416: _temp2415=(( struct Cyc_Absyn_ArrayType_struct*) _temp2331)->f3; goto
_LL2354;} else{ goto _LL2355;} _LL2355: if(( unsigned int) _temp2331 > 4u?*((
int*) _temp2331) == Cyc_Absyn_FnType: 0){ _LL2422: _temp2421=(( struct Cyc_Absyn_FnType_struct*)
_temp2331)->f1; _LL2439: _temp2438= _temp2421.tvars; _temp2440=&((( struct Cyc_Absyn_FnType_struct*)
_temp2331)->f1).tvars; goto _LL2436; _LL2436: _temp2435= _temp2421.effect;
_temp2437=&((( struct Cyc_Absyn_FnType_struct*) _temp2331)->f1).effect; goto
_LL2434; _LL2434: _temp2433=( void*) _temp2421.ret_typ; goto _LL2432; _LL2432:
_temp2431= _temp2421.args; goto _LL2430; _LL2430: _temp2429= _temp2421.c_varargs;
goto _LL2428; _LL2428: _temp2427= _temp2421.cyc_varargs; goto _LL2426; _LL2426:
_temp2425= _temp2421.rgn_po; goto _LL2424; _LL2424: _temp2423= _temp2421.attributes;
goto _LL2356;} else{ goto _LL2357;} _LL2357: if(( unsigned int) _temp2331 > 4u?*((
int*) _temp2331) == Cyc_Absyn_TupleType: 0){ _LL2442: _temp2441=(( struct Cyc_Absyn_TupleType_struct*)
_temp2331)->f1; goto _LL2358;} else{ goto _LL2359;} _LL2359: if(( unsigned int)
_temp2331 > 4u?*(( int*) _temp2331) == Cyc_Absyn_AnonStructType: 0){ _LL2444:
_temp2443=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2331)->f1; goto
_LL2360;} else{ goto _LL2361;} _LL2361: if(( unsigned int) _temp2331 > 4u?*((
int*) _temp2331) == Cyc_Absyn_AnonUnionType: 0){ _LL2446: _temp2445=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp2331)->f1; goto _LL2362;} else{ goto
_LL2363;} _LL2363: if(( unsigned int) _temp2331 > 4u?*(( int*) _temp2331) == Cyc_Absyn_StructType:
0){ _LL2453: _temp2452=(( struct Cyc_Absyn_StructType_struct*) _temp2331)->f1;
goto _LL2451; _LL2451: _temp2450=(( struct Cyc_Absyn_StructType_struct*)
_temp2331)->f2; goto _LL2448; _LL2448: _temp2447=(( struct Cyc_Absyn_StructType_struct*)
_temp2331)->f3; _temp2449=&(( struct Cyc_Absyn_StructType_struct*) _temp2331)->f3;
goto _LL2364;} else{ goto _LL2365;} _LL2365: if(( unsigned int) _temp2331 > 4u?*((
int*) _temp2331) == Cyc_Absyn_UnionType: 0){ _LL2460: _temp2459=(( struct Cyc_Absyn_UnionType_struct*)
_temp2331)->f1; goto _LL2458; _LL2458: _temp2457=(( struct Cyc_Absyn_UnionType_struct*)
_temp2331)->f2; goto _LL2455; _LL2455: _temp2454=(( struct Cyc_Absyn_UnionType_struct*)
_temp2331)->f3; _temp2456=&(( struct Cyc_Absyn_UnionType_struct*) _temp2331)->f3;
goto _LL2366;} else{ goto _LL2367;} _LL2367: if(( unsigned int) _temp2331 > 4u?*((
int*) _temp2331) == Cyc_Absyn_TypedefType: 0){ _LL2467: _temp2466=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2331)->f1; goto _LL2465; _LL2465: _temp2464=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2331)->f2; goto _LL2462; _LL2462: _temp2461=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2331)->f3; _temp2463=&(( struct Cyc_Absyn_TypedefType_struct*) _temp2331)->f3;
goto _LL2368;} else{ goto _LL2369;} _LL2369: if( _temp2331 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL2370;} else{ goto _LL2371;} _LL2371: if(( unsigned int) _temp2331 > 4u?*((
int*) _temp2331) == Cyc_Absyn_RgnHandleType: 0){ _LL2469: _temp2468=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp2331)->f1; goto _LL2372;} else{
goto _LL2373;} _LL2373: if(( unsigned int) _temp2331 > 4u?*(( int*) _temp2331)
== Cyc_Absyn_AccessEff: 0){ _LL2471: _temp2470=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp2331)->f1; goto _LL2374;} else{ goto _LL2375;} _LL2375: if(( unsigned int)
_temp2331 > 4u?*(( int*) _temp2331) == Cyc_Absyn_JoinEff: 0){ _LL2473: _temp2472=((
struct Cyc_Absyn_JoinEff_struct*) _temp2331)->f1; goto _LL2376;} else{ goto
_LL2332;} _LL2334: goto _LL2332; _LL2336: if(* _temp2379 == 0){* _temp2379=({
struct Cyc_Core_Opt* _temp2474=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2474->v=( void*) expected_kind; _temp2474;});} cvtenv.free_evars=
Cyc_Tcutil_add_free_evar( cvtenv.free_evars, t); goto _LL2332; _LL2338: { struct
Cyc_Absyn_Conref* c= Cyc_Absyn_compress_conref( _temp2380->kind); if(( void*) c->v
==( void*) Cyc_Absyn_No_constr){( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2475=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2475[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2476; _temp2476.tag= Cyc_Absyn_Eq_constr;
_temp2476.f1=( void*) expected_kind; _temp2476;}); _temp2475;})));} cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar( loc, cvtenv.kind_env, _temp2380); cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars, _temp2380); goto _LL2332;}
_LL2340: if(* _temp2384 == 0){ struct _handler_cons _temp2477; _push_handler(&
_temp2477);{ int _temp2479= 0; if( setjmp( _temp2477.handler)){ _temp2479= 1;}
if( ! _temp2479){{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl( te,
loc, _temp2385);* _temp2384=( struct Cyc_Absyn_Enumdecl*)* ed;}; _pop_handler();}
else{ void* _temp2478=( void*) _exn_thrown; void* _temp2481= _temp2478; _LL2483:
if( _temp2481 == Cyc_Dict_Absent){ goto _LL2484;} else{ goto _LL2485;} _LL2485:
goto _LL2486; _LL2484:({ struct Cyc_Stdio_String_pa_struct _temp2488; _temp2488.tag=
Cyc_Stdio_String_pa; _temp2488.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2385);{ void* _temp2487[ 1u]={& _temp2488}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type enum %s",
sizeof( unsigned char), 21u), _tag_arr( _temp2487, sizeof( void*), 1u));}});
return cvtenv; _LL2486:( void) _throw( _temp2481); _LL2482:;}}}{ struct Cyc_Absyn_Enumdecl*
ed=( struct Cyc_Absyn_Enumdecl*) _check_null(* _temp2384);* _temp2385=( ed->name)[
_check_known_subscript_notnull( 1u, 0)]; goto _LL2332;} _LL2342:{ void*
_temp2489=* _temp2395; struct Cyc_Absyn_UnknownTunionInfo _temp2495; int
_temp2497; struct _tuple1* _temp2499; struct Cyc_Absyn_Tuniondecl* _temp2501;
_LL2491: if(*(( int*) _temp2489) == Cyc_Absyn_UnknownTunion){ _LL2496: _temp2495=((
struct Cyc_Absyn_UnknownTunion_struct*) _temp2489)->f1; _LL2500: _temp2499=
_temp2495.name; goto _LL2498; _LL2498: _temp2497= _temp2495.is_xtunion; goto
_LL2492;} else{ goto _LL2493;} _LL2493: if(*(( int*) _temp2489) == Cyc_Absyn_KnownTunion){
_LL2502: _temp2501=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2489)->f1; goto
_LL2494;} else{ goto _LL2490;} _LL2492: { struct Cyc_Absyn_Tuniondecl* tud;{
struct _handler_cons _temp2503; _push_handler(& _temp2503);{ int _temp2505= 0;
if( setjmp( _temp2503.handler)){ _temp2505= 1;} if( ! _temp2505){ tud=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp2499);; _pop_handler();} else{ void* _temp2504=( void*)
_exn_thrown; void* _temp2507= _temp2504; _LL2509: if( _temp2507 == Cyc_Dict_Absent){
goto _LL2510;} else{ goto _LL2511;} _LL2511: goto _LL2512; _LL2510:({ struct Cyc_Stdio_String_pa_struct
_temp2514; _temp2514.tag= Cyc_Stdio_String_pa; _temp2514.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2499);{ void* _temp2513[ 1u]={& _temp2514}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound type [x]tunion %s", sizeof( unsigned char), 26u),
_tag_arr( _temp2513, sizeof( void*), 1u));}}); return cvtenv; _LL2512:( void)
_throw( _temp2507); _LL2508:;}}} if( tud->is_xtunion != _temp2497){({ struct Cyc_Stdio_String_pa_struct
_temp2516; _temp2516.tag= Cyc_Stdio_String_pa; _temp2516.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2499);{ void* _temp2515[ 1u]={& _temp2516}; Cyc_Tcutil_terr(
loc, _tag_arr("[x]tunion is different from type declaration %s", sizeof(
unsigned char), 48u), _tag_arr( _temp2515, sizeof( void*), 1u));}});}* _temp2395=(
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp2517=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp2517[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp2518; _temp2518.tag= Cyc_Absyn_KnownTunion;
_temp2518.f1= tud; _temp2518;}); _temp2517;}); _temp2501= tud; goto _LL2494;}
_LL2494: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2389);{ struct Cyc_List_List* tvs= _temp2501->tvs; for( 0; _temp2391 != 0?
tvs != 0: 0;( _temp2391=(( struct Cyc_List_List*) _check_null( _temp2391))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2391))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} if( _temp2391
!= 0){({ struct Cyc_Stdio_String_pa_struct _temp2520; _temp2520.tag= Cyc_Stdio_String_pa;
_temp2520.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2501->name);{
void* _temp2519[ 1u]={& _temp2520}; Cyc_Tcutil_terr( loc, _tag_arr("too many type arguments for tunion %s",
sizeof( unsigned char), 38u), _tag_arr( _temp2519, sizeof( void*), 1u));}});}
if( tvs != 0){({ struct Cyc_Stdio_String_pa_struct _temp2522; _temp2522.tag= Cyc_Stdio_String_pa;
_temp2522.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2501->name);{
void* _temp2521[ 1u]={& _temp2522}; Cyc_Tcutil_terr( loc, _tag_arr("too few type arguments for tunion %s",
sizeof( unsigned char), 37u), _tag_arr( _temp2521, sizeof( void*), 1u));}});}
goto _LL2490;} _LL2490:;} goto _LL2332; _LL2344:{ void* _temp2523=* _temp2402;
struct Cyc_Absyn_UnknownTunionFieldInfo _temp2529; int _temp2531; struct _tuple1*
_temp2533; struct _tuple1* _temp2535; struct Cyc_Absyn_Tunionfield* _temp2537;
struct Cyc_Absyn_Tuniondecl* _temp2539; _LL2525: if(*(( int*) _temp2523) == Cyc_Absyn_UnknownTunionfield){
_LL2530: _temp2529=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp2523)->f1;
_LL2536: _temp2535= _temp2529.tunion_name; goto _LL2534; _LL2534: _temp2533=
_temp2529.field_name; goto _LL2532; _LL2532: _temp2531= _temp2529.is_xtunion;
goto _LL2526;} else{ goto _LL2527;} _LL2527: if(*(( int*) _temp2523) == Cyc_Absyn_KnownTunionfield){
_LL2540: _temp2539=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp2523)->f1;
goto _LL2538; _LL2538: _temp2537=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2523)->f2; goto _LL2528;} else{ goto _LL2524;} _LL2526: { struct Cyc_Absyn_Tuniondecl*
tud; struct Cyc_Absyn_Tunionfield* tuf;{ struct _handler_cons _temp2541;
_push_handler(& _temp2541);{ int _temp2543= 0; if( setjmp( _temp2541.handler)){
_temp2543= 1;} if( ! _temp2543){* Cyc_Tcenv_lookup_tuniondecl( te, loc,
_temp2535);; _pop_handler();} else{ void* _temp2542=( void*) _exn_thrown; void*
_temp2545= _temp2542; _LL2547: if( _temp2545 == Cyc_Dict_Absent){ goto _LL2548;}
else{ goto _LL2549;} _LL2549: goto _LL2550; _LL2548:({ struct Cyc_Stdio_String_pa_struct
_temp2552; _temp2552.tag= Cyc_Stdio_String_pa; _temp2552.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2535);{ void* _temp2551[ 1u]={& _temp2552}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound type tunion %s", sizeof( unsigned char), 23u), _tag_arr(
_temp2551, sizeof( void*), 1u));}}); return cvtenv; _LL2550:( void) _throw(
_temp2545); _LL2546:;}}}{ struct _handler_cons _temp2553; _push_handler(&
_temp2553);{ int _temp2555= 0; if( setjmp( _temp2553.handler)){ _temp2555= 1;}
if( ! _temp2555){{ void* _temp2556= Cyc_Tcenv_lookup_ordinary( te, loc,
_temp2533); struct Cyc_Absyn_Tunionfield* _temp2562; struct Cyc_Absyn_Tuniondecl*
_temp2564; _LL2558: if(*(( int*) _temp2556) == Cyc_Tcenv_TunionRes){ _LL2565:
_temp2564=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2556)->f1; goto _LL2563;
_LL2563: _temp2562=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2556)->f2; goto
_LL2559;} else{ goto _LL2560;} _LL2560: goto _LL2561; _LL2559: tuf= _temp2562;
tud= _temp2564; if( tud->is_xtunion != _temp2531){({ struct Cyc_Stdio_String_pa_struct
_temp2567; _temp2567.tag= Cyc_Stdio_String_pa; _temp2567.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2535);{ void* _temp2566[ 1u]={& _temp2567}; Cyc_Tcutil_terr(
loc, _tag_arr("[x]tunion is different from type declaration %s", sizeof(
unsigned char), 48u), _tag_arr( _temp2566, sizeof( void*), 1u));}});} goto
_LL2557; _LL2561:({ struct Cyc_Stdio_String_pa_struct _temp2570; _temp2570.tag=
Cyc_Stdio_String_pa; _temp2570.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2535);{ struct Cyc_Stdio_String_pa_struct _temp2569; _temp2569.tag= Cyc_Stdio_String_pa;
_temp2569.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2533);{ void*
_temp2568[ 2u]={& _temp2569,& _temp2570}; Cyc_Tcutil_terr( loc, _tag_arr("unbound field %s in type tunion %s",
sizeof( unsigned char), 35u), _tag_arr( _temp2568, sizeof( void*), 2u));}}});{
struct Cyc_Tcutil_CVTEnv _temp2571= cvtenv; _npop_handler( 0u); return _temp2571;}
_LL2557:;}; _pop_handler();} else{ void* _temp2554=( void*) _exn_thrown; void*
_temp2573= _temp2554; _LL2575: if( _temp2573 == Cyc_Dict_Absent){ goto _LL2576;}
else{ goto _LL2577;} _LL2577: goto _LL2578; _LL2576:({ struct Cyc_Stdio_String_pa_struct
_temp2581; _temp2581.tag= Cyc_Stdio_String_pa; _temp2581.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2535);{ struct Cyc_Stdio_String_pa_struct
_temp2580; _temp2580.tag= Cyc_Stdio_String_pa; _temp2580.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2533);{ void* _temp2579[ 2u]={& _temp2580,&
_temp2581}; Cyc_Tcutil_terr( loc, _tag_arr("unbound field %s in type tunion %s",
sizeof( unsigned char), 35u), _tag_arr( _temp2579, sizeof( void*), 2u));}}});
return cvtenv; _LL2578:( void) _throw( _temp2573); _LL2574:;}}}* _temp2402=(
void*)({ struct Cyc_Absyn_KnownTunionfield_struct* _temp2582=( struct Cyc_Absyn_KnownTunionfield_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunionfield_struct)); _temp2582[ 0]=({
struct Cyc_Absyn_KnownTunionfield_struct _temp2583; _temp2583.tag= Cyc_Absyn_KnownTunionfield;
_temp2583.f1= tud; _temp2583.f2= tuf; _temp2583;}); _temp2582;}); _temp2539= tud;
_temp2537= tuf; goto _LL2528;} _LL2528: { struct Cyc_List_List* tvs= _temp2539->tvs;
for( 0; _temp2398 != 0? tvs != 0: 0;( _temp2398=(( struct Cyc_List_List*)
_check_null( _temp2398))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){
void* t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2398))->hd; void*
k1= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te,
cvtenv, k1, t1);} if( _temp2398 != 0){({ struct Cyc_Stdio_String_pa_struct
_temp2586; _temp2586.tag= Cyc_Stdio_String_pa; _temp2586.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2537->name);{ struct Cyc_Stdio_String_pa_struct
_temp2585; _temp2585.tag= Cyc_Stdio_String_pa; _temp2585.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2539->name);{ void* _temp2584[ 2u]={& _temp2585,&
_temp2586}; Cyc_Tcutil_terr( loc, _tag_arr("too many type arguments for tunion %s.%s",
sizeof( unsigned char), 41u), _tag_arr( _temp2584, sizeof( void*), 2u));}}});}
if( tvs != 0){({ struct Cyc_Stdio_String_pa_struct _temp2589; _temp2589.tag= Cyc_Stdio_String_pa;
_temp2589.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2537->name);{
struct Cyc_Stdio_String_pa_struct _temp2588; _temp2588.tag= Cyc_Stdio_String_pa;
_temp2588.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2539->name);{
void* _temp2587[ 2u]={& _temp2588,& _temp2589}; Cyc_Tcutil_terr( loc, _tag_arr("too few type arguments for tunion %s.%s",
sizeof( unsigned char), 40u), _tag_arr( _temp2587, sizeof( void*), 2u));}}});}
goto _LL2524;} _LL2524:;} goto _LL2332; _LL2346: cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_AnyKind, _temp2413); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind, _temp2411);{ void* _temp2590=( void*)(
Cyc_Absyn_compress_conref( _temp2405))->v; void* _temp2596; struct Cyc_Absyn_Exp*
_temp2598; _LL2592: if(( unsigned int) _temp2590 > 1u?*(( int*) _temp2590) ==
Cyc_Absyn_Eq_constr: 0){ _LL2597: _temp2596=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2590)->f1; if(( unsigned int) _temp2596 > 1u?*(( int*) _temp2596) == Cyc_Absyn_Upper_b:
0){ _LL2599: _temp2598=(( struct Cyc_Absyn_Upper_b_struct*) _temp2596)->f1; goto
_LL2593;} else{ goto _LL2594;}} else{ goto _LL2594;} _LL2594: goto _LL2595;
_LL2593: if( ! Cyc_Tcutil_is_const_exp( te, _temp2598)){({ void* _temp2600[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("pointer bounds expression is not a constant",
sizeof( unsigned char), 44u), _tag_arr( _temp2600, sizeof( void*), 0u));});} Cyc_Tcexp_tcExp(
te, 0, _temp2598); if( ! Cyc_Tcutil_coerce_uint_typ( te, _temp2598)){({ void*
_temp2601[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("pointer bounds expression is not an unsigned int",
sizeof( unsigned char), 49u), _tag_arr( _temp2601, sizeof( void*), 0u));});} Cyc_Evexp_eval_const_uint_exp(
_temp2598); goto _LL2591; _LL2595: goto _LL2591; _LL2591:;} goto _LL2332;
_LL2348: goto _LL2332; _LL2350: goto _LL2332; _LL2352: goto _LL2332; _LL2354:
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,
_temp2419); if( _temp2415 == 0? 1: ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp2415))){({ void* _temp2602[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("array bounds expression is not constant", sizeof( unsigned char), 40u),
_tag_arr( _temp2602, sizeof( void*), 0u));});} Cyc_Tcexp_tcExp( te, 0,( struct
Cyc_Absyn_Exp*) _check_null( _temp2415)); if( ! Cyc_Tcutil_coerce_uint_typ( te,(
struct Cyc_Absyn_Exp*) _check_null( _temp2415))){({ void* _temp2603[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("array bounds expression is not an unsigned int", sizeof(
unsigned char), 47u), _tag_arr( _temp2603, sizeof( void*), 0u));});} Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2415)); goto _LL2332; _LL2356: { int
num_convs= 0; int seen_cdecl= 0; int seen_stdcall= 0; int seen_fastcall= 0; int
seen_format= 0; void* ft=( void*) Cyc_Absyn_Printf_ft; int fmt_desc_arg= - 1;
int fmt_arg_start= - 1; for( 0; _temp2423 != 0; _temp2423=(( struct Cyc_List_List*)
_check_null( _temp2423))->tl){ if( ! Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*)
_check_null( _temp2423))->hd)){({ struct Cyc_Stdio_String_pa_struct _temp2605;
_temp2605.tag= Cyc_Stdio_String_pa; _temp2605.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp2423))->hd);{ void* _temp2604[
1u]={& _temp2605}; Cyc_Tcutil_terr( loc, _tag_arr("bad function type attribute %s",
sizeof( unsigned char), 31u), _tag_arr( _temp2604, sizeof( void*), 1u));}});}{
void* _temp2606=( void*)(( struct Cyc_List_List*) _check_null( _temp2423))->hd;
int _temp2618; int _temp2620; void* _temp2622; _LL2608: if( _temp2606 ==( void*)
Cyc_Absyn_Stdcall_att){ goto _LL2609;} else{ goto _LL2610;} _LL2610: if(
_temp2606 ==( void*) Cyc_Absyn_Cdecl_att){ goto _LL2611;} else{ goto _LL2612;}
_LL2612: if( _temp2606 ==( void*) Cyc_Absyn_Fastcall_att){ goto _LL2613;} else{
goto _LL2614;} _LL2614: if(( unsigned int) _temp2606 > 16u?*(( int*) _temp2606)
== Cyc_Absyn_Format_att: 0){ _LL2623: _temp2622=( void*)(( struct Cyc_Absyn_Format_att_struct*)
_temp2606)->f1; goto _LL2621; _LL2621: _temp2620=(( struct Cyc_Absyn_Format_att_struct*)
_temp2606)->f2; goto _LL2619; _LL2619: _temp2618=(( struct Cyc_Absyn_Format_att_struct*)
_temp2606)->f3; goto _LL2615;} else{ goto _LL2616;} _LL2616: goto _LL2617;
_LL2609: if( ! seen_stdcall){ seen_stdcall= 1; ++ num_convs;} goto _LL2607;
_LL2611: if( ! seen_cdecl){ seen_cdecl= 1; ++ num_convs;} goto _LL2607; _LL2613:
if( ! seen_fastcall){ seen_fastcall= 1; ++ num_convs;} goto _LL2607; _LL2615:
if( ! seen_format){ seen_format= 1; ft= _temp2622; fmt_desc_arg= _temp2620;
fmt_arg_start= _temp2618;} else{({ void* _temp2624[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("function can't have multiple format attributes", sizeof( unsigned char),
47u), _tag_arr( _temp2624, sizeof( void*), 0u));});} goto _LL2607; _LL2617: goto
_LL2607; _LL2607:;}} if( num_convs > 1){({ void* _temp2625[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("function can't have multiple calling conventions", sizeof(
unsigned char), 49u), _tag_arr( _temp2625, sizeof( void*), 0u));});} Cyc_Tcutil_check_unique_tvars(
loc,* _temp2440);{ struct Cyc_List_List* b=* _temp2440; for( 0; b != 0; b=((
struct Cyc_List_List*) _check_null( b))->tl){(( struct Cyc_Absyn_Tvar*)(( struct
Cyc_List_List*) _check_null( b))->hd)->identity= Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env= Cyc_Tcutil_add_bound_tvar( cvtenv.kind_env,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( b))->hd);{ void* _temp2626=( void*)( Cyc_Absyn_compress_conref(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd)->kind))->v;
void* _temp2632; _LL2628: if(( unsigned int) _temp2626 > 1u?*(( int*) _temp2626)
== Cyc_Absyn_Eq_constr: 0){ _LL2633: _temp2632=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2626)->f1; if( _temp2632 ==( void*) Cyc_Absyn_MemKind){ goto _LL2629;}
else{ goto _LL2630;}} else{ goto _LL2630;} _LL2630: goto _LL2631; _LL2629:({
struct Cyc_Stdio_String_pa_struct _temp2635; _temp2635.tag= Cyc_Stdio_String_pa;
_temp2635.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( b))->hd)->name;{ void* _temp2634[ 1u]={& _temp2635}; Cyc_Tcutil_terr(
loc, _tag_arr("function attempts to abstract Mem type variable %s", sizeof(
unsigned char), 51u), _tag_arr( _temp2634, sizeof( void*), 1u));}}); goto
_LL2627; _LL2631: goto _LL2627; _LL2627:;}}}{ struct Cyc_Tcutil_CVTEnv _temp2636=({
struct Cyc_Tcutil_CVTEnv _temp2812; _temp2812.kind_env= cvtenv.kind_env;
_temp2812.free_vars= 0; _temp2812.free_evars= 0; _temp2812;}); _temp2636= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2636,( void*) Cyc_Absyn_MemKind, _temp2433);{ struct Cyc_List_List*
a= _temp2431; for( 0; a != 0; a=(( struct Cyc_List_List*) _check_null( a))->tl){
_temp2636= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2636,( void*) Cyc_Absyn_MemKind,(*((
struct _tuple2*)(( struct Cyc_List_List*) _check_null( a))->hd)).f3);}} if(
_temp2427 != 0){ if( _temp2429){({ void* _temp2637[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("both c_vararg and cyc_vararg",
sizeof( unsigned char), 29u), _tag_arr( _temp2637, sizeof( void*), 0u));});}{
int _temp2640; void* _temp2642; void* _temp2644; struct Cyc_Absyn_Tqual
_temp2646; struct Cyc_Core_Opt* _temp2648; struct Cyc_Absyn_VarargInfo _temp2638=*((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp2427)); _LL2649: _temp2648=
_temp2638.name; goto _LL2647; _LL2647: _temp2646= _temp2638.tq; goto _LL2645;
_LL2645: _temp2644=( void*) _temp2638.type; goto _LL2643; _LL2643: _temp2642=(
void*) _temp2638.rgn; goto _LL2641; _LL2641: _temp2640= _temp2638.inject; goto
_LL2639; _LL2639: _temp2636= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2636,(
void*) Cyc_Absyn_MemKind, _temp2644);{ void* _temp2650= Cyc_Tcutil_compress(
_temp2642); _LL2652: if(( unsigned int) _temp2650 > 4u?*(( int*) _temp2650) ==
Cyc_Absyn_Evar: 0){ goto _LL2653;} else{ goto _LL2654;} _LL2654: if((
unsigned int) _temp2650 > 4u?*(( int*) _temp2650) == Cyc_Absyn_VarType: 0){ goto
_LL2655;} else{ goto _LL2656;} _LL2656: if( _temp2650 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL2657;} else{ goto _LL2658;} _LL2658: goto _LL2659; _LL2653: { void*
vr_tvar=( void*)({ struct Cyc_Absyn_VarType_struct* _temp2660=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2660[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2661; _temp2661.tag= Cyc_Absyn_VarType; _temp2661.f1=
Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_RgnKind); _temp2661;}); _temp2660;}); Cyc_Tcutil_unify(
_temp2642, vr_tvar); goto _LL2651;} _LL2655: goto _LL2651; _LL2657:({ void*
_temp2662[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("varargs should be in a region, not heap-allocated",
sizeof( unsigned char), 50u), _tag_arr( _temp2662, sizeof( void*), 0u));}); goto
_LL2651; _LL2659: goto _LL2651; _LL2651:;} _temp2636= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2636,( void*) Cyc_Absyn_RgnKind, _temp2642); if( _temp2640){ void*
_temp2663= Cyc_Tcutil_compress( _temp2644); struct Cyc_Absyn_TunionInfo
_temp2669; void* _temp2671; void* _temp2673; _LL2665: if(( unsigned int)
_temp2663 > 4u?*(( int*) _temp2663) == Cyc_Absyn_TunionType: 0){ _LL2670:
_temp2669=(( struct Cyc_Absyn_TunionType_struct*) _temp2663)->f1; _LL2674:
_temp2673=( void*) _temp2669.tunion_info; if(*(( int*) _temp2673) == Cyc_Absyn_KnownTunion){
goto _LL2672;} else{ goto _LL2667;} _LL2672: _temp2671=( void*) _temp2669.rgn;
goto _LL2666;} else{ goto _LL2667;} _LL2667: goto _LL2668; _LL2666:{ void*
_temp2675= Cyc_Tcutil_compress( _temp2671); _LL2677: if(( unsigned int)
_temp2675 > 4u?*(( int*) _temp2675) == Cyc_Absyn_Evar: 0){ goto _LL2678;} else{
goto _LL2679;} _LL2679: if(( unsigned int) _temp2675 > 4u?*(( int*) _temp2675)
== Cyc_Absyn_VarType: 0){ goto _LL2680;} else{ goto _LL2681;} _LL2681: if(
_temp2675 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL2682;} else{ goto _LL2683;}
_LL2683: goto _LL2684; _LL2678: Cyc_Tcutil_unify( _temp2642, _temp2671); goto
_LL2676; _LL2680: goto _LL2676; _LL2682:({ void* _temp2685[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("injected vararg [x]tunion should be in a region, not heap-allocated",
sizeof( unsigned char), 68u), _tag_arr( _temp2685, sizeof( void*), 0u));}); goto
_LL2676; _LL2684: goto _LL2676; _LL2676:;} goto _LL2664; _LL2668:({ void*
_temp2686[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("can't inject a non-[x]tunion type",
sizeof( unsigned char), 34u), _tag_arr( _temp2686, sizeof( void*), 0u));}); goto
_LL2664; _LL2664:;}}} if( seen_format){ int _temp2687=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp2431); if(((( fmt_desc_arg < 0? 1: fmt_desc_arg >
_temp2687)? 1: fmt_arg_start < 0)? 1:( _temp2427 == 0? fmt_arg_start != 0: 0))?
1:( _temp2427 != 0? fmt_arg_start != _temp2687 + 1: 0)){({ void* _temp2688[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("bad format descriptor", sizeof( unsigned char),
22u), _tag_arr( _temp2688, sizeof( void*), 0u));});} else{ void* _temp2691;
struct _tuple2 _temp2689=*(( struct _tuple2*(*)( struct Cyc_List_List* x, int i))
Cyc_List_nth)( _temp2431, fmt_desc_arg - 1); _LL2692: _temp2691= _temp2689.f3;
goto _LL2690; _LL2690:{ void* _temp2693= Cyc_Tcutil_compress( _temp2691); struct
Cyc_Absyn_PtrInfo _temp2699; struct Cyc_Absyn_Conref* _temp2701; void* _temp2703;
_LL2695: if(( unsigned int) _temp2693 > 4u?*(( int*) _temp2693) == Cyc_Absyn_PointerType:
0){ _LL2700: _temp2699=(( struct Cyc_Absyn_PointerType_struct*) _temp2693)->f1;
_LL2704: _temp2703=( void*) _temp2699.elt_typ; goto _LL2702; _LL2702: _temp2701=
_temp2699.bounds; goto _LL2696;} else{ goto _LL2697;} _LL2697: goto _LL2698;
_LL2696:{ struct _tuple8 _temp2706=({ struct _tuple8 _temp2705; _temp2705.f1=
Cyc_Tcutil_compress( _temp2703); _temp2705.f2= Cyc_Absyn_conref_def(( void*) Cyc_Absyn_Unknown_b,
_temp2701); _temp2705;}); void* _temp2712; void* _temp2714; void* _temp2716;
void* _temp2718; _LL2708: _LL2715: _temp2714= _temp2706.f1; if(( unsigned int)
_temp2714 > 4u?*(( int*) _temp2714) == Cyc_Absyn_IntType: 0){ _LL2719: _temp2718=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2714)->f1; if( _temp2718 ==(
void*) Cyc_Absyn_Unsigned){ goto _LL2717;} else{ goto _LL2710;} _LL2717:
_temp2716=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2714)->f2; if(
_temp2716 ==( void*) Cyc_Absyn_B1){ goto _LL2713;} else{ goto _LL2710;}} else{
goto _LL2710;} _LL2713: _temp2712= _temp2706.f2; if( _temp2712 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL2709;} else{ goto _LL2710;} _LL2710: goto _LL2711; _LL2709: goto _LL2707;
_LL2711:({ void* _temp2720[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("format descriptor is not a char ? type",
sizeof( unsigned char), 39u), _tag_arr( _temp2720, sizeof( void*), 0u));}); goto
_LL2707; _LL2707:;} goto _LL2694; _LL2698:({ void* _temp2721[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("format descriptor is not a char ? type", sizeof( unsigned char),
39u), _tag_arr( _temp2721, sizeof( void*), 0u));}); goto _LL2694; _LL2694:;} if(
fmt_arg_start != 0){ void* _temp2722= Cyc_Tcutil_compress(( void*)(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp2427))->type); int _temp2723= 0;{ void* _temp2724= ft; _LL2726:
if( _temp2724 ==( void*) Cyc_Absyn_Printf_ft){ goto _LL2727;} else{ goto _LL2728;}
_LL2728: if( _temp2724 ==( void*) Cyc_Absyn_Scanf_ft){ goto _LL2729;} else{ goto
_LL2725;} _LL2727:{ void* _temp2730= _temp2722; struct Cyc_Absyn_TunionInfo
_temp2736; void* _temp2738; struct Cyc_Absyn_Tuniondecl* _temp2740; _LL2732: if((
unsigned int) _temp2730 > 4u?*(( int*) _temp2730) == Cyc_Absyn_TunionType: 0){
_LL2737: _temp2736=(( struct Cyc_Absyn_TunionType_struct*) _temp2730)->f1;
_LL2739: _temp2738=( void*) _temp2736.tunion_info; if(*(( int*) _temp2738) ==
Cyc_Absyn_KnownTunion){ _LL2741: _temp2740=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp2738)->f1; goto _LL2733;} else{ goto _LL2734;}} else{ goto _LL2734;}
_LL2734: goto _LL2735; _LL2733: _temp2723= Cyc_Absyn_qvar_cmp( _temp2740->name,
Cyc_Absyn_tunion_print_arg_qvar) != 0; goto _LL2731; _LL2735: _temp2723= 1; goto
_LL2731; _LL2731:;} goto _LL2725; _LL2729:{ void* _temp2742= _temp2722; struct
Cyc_Absyn_TunionInfo _temp2748; void* _temp2750; struct Cyc_Absyn_Tuniondecl*
_temp2752; _LL2744: if(( unsigned int) _temp2742 > 4u?*(( int*) _temp2742) ==
Cyc_Absyn_TunionType: 0){ _LL2749: _temp2748=(( struct Cyc_Absyn_TunionType_struct*)
_temp2742)->f1; _LL2751: _temp2750=( void*) _temp2748.tunion_info; if(*(( int*)
_temp2750) == Cyc_Absyn_KnownTunion){ _LL2753: _temp2752=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp2750)->f1; goto _LL2745;} else{ goto _LL2746;}} else{ goto _LL2746;}
_LL2746: goto _LL2747; _LL2745: _temp2723= Cyc_Absyn_qvar_cmp( _temp2752->name,
Cyc_Absyn_tunion_scanf_arg_qvar) != 0; goto _LL2743; _LL2747: _temp2723= 1; goto
_LL2743; _LL2743:;} goto _LL2725; _LL2725:;} if( _temp2723){({ void* _temp2754[
0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("format attribute and vararg types don't match",
sizeof( unsigned char), 46u), _tag_arr( _temp2754, sizeof( void*), 0u));});}}}}{
struct Cyc_List_List* rpo= _temp2425; for( 0; rpo != 0; rpo=(( struct Cyc_List_List*)
_check_null( rpo))->tl){ struct _tuple8 _temp2757; void* _temp2758; void*
_temp2760; struct _tuple8* _temp2755=( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( rpo))->hd; _temp2757=* _temp2755; _LL2761: _temp2760= _temp2757.f1;
goto _LL2759; _LL2759: _temp2758= _temp2757.f2; goto _LL2756; _LL2756: _temp2636=
Cyc_Tcutil_i_check_valid_type( loc, te, _temp2636,( void*) Cyc_Absyn_RgnKind,
_temp2760); _temp2636= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2636,( void*)
Cyc_Absyn_RgnKind, _temp2758);}} if(* _temp2437 != 0){ _temp2636= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2636,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_Core_Opt*)
_check_null(* _temp2437))->v);} else{ struct Cyc_Absyn_Tvar* e= Cyc_Tcutil_new_tvar((
void*) Cyc_Absyn_EffKind); _temp2636.free_vars= Cyc_Tcutil_add_free_tvar( loc,
_temp2636.free_vars, e);{ struct Cyc_List_List* effect= 0;{ struct Cyc_List_List*
tvs= _temp2636.free_vars; for( 0; tvs != 0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){ void* _temp2762=( void*)( Cyc_Absyn_compress_conref(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind))->v;
void* _temp2770; void* _temp2772; _LL2764: if(( unsigned int) _temp2762 > 1u?*((
int*) _temp2762) == Cyc_Absyn_Eq_constr: 0){ _LL2771: _temp2770=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2762)->f1; if( _temp2770 ==( void*) Cyc_Absyn_RgnKind){
goto _LL2765;} else{ goto _LL2766;}} else{ goto _LL2766;} _LL2766: if((
unsigned int) _temp2762 > 1u?*(( int*) _temp2762) == Cyc_Absyn_Eq_constr: 0){
_LL2773: _temp2772=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2762)->f1;
if( _temp2772 ==( void*) Cyc_Absyn_EffKind){ goto _LL2767;} else{ goto _LL2768;}}
else{ goto _LL2768;} _LL2768: goto _LL2769; _LL2765: effect=({ struct Cyc_List_List*
_temp2774=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2774->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2775=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2775[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2776; _temp2776.tag= Cyc_Absyn_AccessEff;
_temp2776.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp2777=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2777[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2778; _temp2778.tag= Cyc_Absyn_VarType;
_temp2778.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp2778;}); _temp2777;})); _temp2776;}); _temp2775;})); _temp2774->tl= effect;
_temp2774;}); goto _LL2763; _LL2767: effect=({ struct Cyc_List_List* _temp2779=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2779->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp2780=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2780[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2781; _temp2781.tag= Cyc_Absyn_VarType; _temp2781.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp2781;}); _temp2780;})); _temp2779->tl= effect; _temp2779;}); goto _LL2763;
_LL2769: goto _LL2763; _LL2763:;}}{ struct Cyc_List_List* ts= _temp2636.free_evars;
for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){ void*
_temp2782= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);
_LL2784: if( _temp2782 ==( void*) Cyc_Absyn_RgnKind){ goto _LL2785;} else{ goto
_LL2786;} _LL2786: if( _temp2782 ==( void*) Cyc_Absyn_EffKind){ goto _LL2787;}
else{ goto _LL2788;} _LL2788: goto _LL2789; _LL2785: effect=({ struct Cyc_List_List*
_temp2790=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2790->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2791=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2791[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2792; _temp2792.tag= Cyc_Absyn_AccessEff;
_temp2792.f1=( void*)(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);
_temp2792;}); _temp2791;})); _temp2790->tl= effect; _temp2790;}); goto _LL2783;
_LL2787: effect=({ struct Cyc_List_List* _temp2793=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2793->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( ts))->hd); _temp2793->tl= effect; _temp2793;});
goto _LL2783; _LL2789: goto _LL2783; _LL2783:;}}* _temp2437=({ struct Cyc_Core_Opt*
_temp2794=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2794->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2795=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2795[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2796; _temp2796.tag= Cyc_Absyn_JoinEff;
_temp2796.f1= Cyc_List_imp_rev( effect); _temp2796;}); _temp2795;})); _temp2794;});}}
if(* _temp2440 != 0){ struct Cyc_List_List* bs=* _temp2440; for( 0; bs != 0; bs=((
struct Cyc_List_List*) _check_null( bs))->tl){ struct Cyc_Absyn_Conref*
_temp2797= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( bs))->hd)->kind); void* _temp2798=( void*) _temp2797->v; void*
_temp2806; _LL2800: if( _temp2798 ==( void*) Cyc_Absyn_No_constr){ goto _LL2801;}
else{ goto _LL2802;} _LL2802: if(( unsigned int) _temp2798 > 1u?*(( int*)
_temp2798) == Cyc_Absyn_Eq_constr: 0){ _LL2807: _temp2806=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2798)->f1; if( _temp2806 ==( void*) Cyc_Absyn_MemKind){ goto _LL2803;}
else{ goto _LL2804;}} else{ goto _LL2804;} _LL2804: goto _LL2805; _LL2801:({
struct Cyc_Stdio_String_pa_struct _temp2809; _temp2809.tag= Cyc_Stdio_String_pa;
_temp2809.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( bs))->hd)->name;{ void* _temp2808[ 1u]={& _temp2809}; Cyc_Tcutil_warn(
loc, _tag_arr("Type variable %s unconstrained, assuming boxed", sizeof(
unsigned char), 47u), _tag_arr( _temp2808, sizeof( void*), 1u));}}); goto
_LL2803; _LL2803:( void*)( _temp2797->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2810=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2810[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2811; _temp2811.tag= Cyc_Absyn_Eq_constr;
_temp2811.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp2811;}); _temp2810;})));
goto _LL2799; _LL2805: goto _LL2799; _LL2799:;}} cvtenv.kind_env= Cyc_Tcutil_remove_bound_tvars(
_temp2636.kind_env,* _temp2440); _temp2636.free_vars= Cyc_Tcutil_remove_bound_tvars(
_temp2636.free_vars,* _temp2440);{ struct Cyc_List_List* tvs= _temp2636.free_vars;
for( 0; tvs != 0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd);}}{ struct Cyc_List_List* evs=
_temp2636.free_evars; for( 0; evs != 0; evs=(( struct Cyc_List_List*)
_check_null( evs))->tl){ cvtenv.free_evars= Cyc_Tcutil_add_free_evar( cvtenv.free_evars,(
void*)(( struct Cyc_List_List*) _check_null( evs))->hd);}} goto _LL2332;}}
_LL2358: for( 0; _temp2441 != 0; _temp2441=(( struct Cyc_List_List*) _check_null(
_temp2441))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*)
Cyc_Absyn_MemKind,(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp2441))->hd)).f2);} goto _LL2332; _LL2360:{ struct _RegionHandle _temp2813=
_new_region(); struct _RegionHandle* sprev_rgn=& _temp2813; _push_region(
sprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for( 0; _temp2443 != 0;
_temp2443=(( struct Cyc_List_List*) _check_null( _temp2443))->tl){ struct Cyc_Absyn_Structfield
_temp2816; struct Cyc_List_List* _temp2817; struct Cyc_Core_Opt* _temp2819; void*
_temp2821; struct Cyc_Absyn_Tqual _temp2823; struct _tagged_arr* _temp2825;
struct Cyc_Absyn_Structfield* _temp2814=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2443))->hd; _temp2816=* _temp2814;
_LL2826: _temp2825= _temp2816.name; goto _LL2824; _LL2824: _temp2823= _temp2816.tq;
goto _LL2822; _LL2822: _temp2821=( void*) _temp2816.type; goto _LL2820; _LL2820:
_temp2819= _temp2816.width; goto _LL2818; _LL2818: _temp2817= _temp2816.attributes;
goto _LL2815; _LL2815: if((( int(*)( int(* compare)( struct _tagged_arr*, struct
_tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields, _temp2825)){({ struct Cyc_Stdio_String_pa_struct
_temp2828; _temp2828.tag= Cyc_Stdio_String_pa; _temp2828.f1=( struct _tagged_arr)*
_temp2825;{ void* _temp2827[ 1u]={& _temp2828}; Cyc_Tcutil_terr( loc, _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u), _tag_arr( _temp2827, sizeof( void*), 1u));}});}
if( Cyc_String_strcmp(* _temp2825, _tag_arr("", sizeof( unsigned char), 1u)) !=
0){ prev_fields=({ struct Cyc_List_List* _temp2829=( struct Cyc_List_List*)
_region_malloc( sprev_rgn, sizeof( struct Cyc_List_List)); _temp2829->hd=( void*)
_temp2825; _temp2829->tl= prev_fields; _temp2829;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp2821); if( _temp2819 != 0){
unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp2819))->v)){({ struct Cyc_Stdio_String_pa_struct
_temp2831; _temp2831.tag= Cyc_Stdio_String_pa; _temp2831.f1=( struct _tagged_arr)*
_temp2825;{ void* _temp2830[ 1u]={& _temp2831}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield %s does not have constant width",
sizeof( unsigned char), 41u), _tag_arr( _temp2830, sizeof( void*), 1u));}});}
else{ w= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*)
_check_null( _temp2819))->v);}{ void* _temp2832= Cyc_Tcutil_compress( _temp2821);
void* _temp2838; _LL2834: if(( unsigned int) _temp2832 > 4u?*(( int*) _temp2832)
== Cyc_Absyn_IntType: 0){ _LL2839: _temp2838=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2832)->f2; goto _LL2835;} else{ goto _LL2836;} _LL2836: goto _LL2837;
_LL2835:{ void* _temp2840= _temp2838; _LL2842: if( _temp2840 ==( void*) Cyc_Absyn_B1){
goto _LL2843;} else{ goto _LL2844;} _LL2844: if( _temp2840 ==( void*) Cyc_Absyn_B2){
goto _LL2845;} else{ goto _LL2846;} _LL2846: if( _temp2840 ==( void*) Cyc_Absyn_B4){
goto _LL2847;} else{ goto _LL2848;} _LL2848: if( _temp2840 ==( void*) Cyc_Absyn_B8){
goto _LL2849;} else{ goto _LL2841;} _LL2843: if( w > 8){({ void* _temp2850[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type", sizeof(
unsigned char), 26u), _tag_arr( _temp2850, sizeof( void*), 0u));});} goto
_LL2841; _LL2845: if( w > 16){({ void* _temp2851[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("bitfield larger than type", sizeof( unsigned char), 26u), _tag_arr(
_temp2851, sizeof( void*), 0u));});} goto _LL2841; _LL2847: if( w > 32){({ void*
_temp2852[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u), _tag_arr( _temp2852, sizeof( void*), 0u));});}
goto _LL2841; _LL2849: if( w > 64){({ void* _temp2853[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u),
_tag_arr( _temp2853, sizeof( void*), 0u));});} goto _LL2841; _LL2841:;} goto
_LL2833; _LL2837:({ struct Cyc_Stdio_String_pa_struct _temp2856; _temp2856.tag=
Cyc_Stdio_String_pa; _temp2856.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp2821);{ struct Cyc_Stdio_String_pa_struct _temp2855; _temp2855.tag= Cyc_Stdio_String_pa;
_temp2855.f1=( struct _tagged_arr)* _temp2825;{ void* _temp2854[ 2u]={&
_temp2855,& _temp2856}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield %s must have integral type but has type %s",
sizeof( unsigned char), 52u), _tag_arr( _temp2854, sizeof( void*), 2u));}}});
goto _LL2833; _LL2833:;}} for( 0; _temp2817 != 0; _temp2817=(( struct Cyc_List_List*)
_check_null( _temp2817))->tl){ void* _temp2857=( void*)(( struct Cyc_List_List*)
_check_null( _temp2817))->hd; _LL2859: if( _temp2857 ==( void*) Cyc_Absyn_Packed_att){
goto _LL2860;} else{ goto _LL2861;} _LL2861: if(( unsigned int) _temp2857 > 16u?*((
int*) _temp2857) == Cyc_Absyn_Aligned_att: 0){ goto _LL2862;} else{ goto _LL2863;}
_LL2863: goto _LL2864; _LL2860: continue; _LL2862: continue; _LL2864:({ struct
Cyc_Stdio_String_pa_struct _temp2867; _temp2867.tag= Cyc_Stdio_String_pa;
_temp2867.f1=( struct _tagged_arr)* _temp2825;{ struct Cyc_Stdio_String_pa_struct
_temp2866; _temp2866.tag= Cyc_Stdio_String_pa; _temp2866.f1=( struct _tagged_arr)
Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*) _check_null(
_temp2817))->hd);{ void* _temp2865[ 2u]={& _temp2866,& _temp2867}; Cyc_Tcutil_terr(
loc, _tag_arr("bad attribute %s on struct member %s", sizeof( unsigned char), 37u),
_tag_arr( _temp2865, sizeof( void*), 2u));}}}); goto _LL2858; _LL2858:;}}};
_pop_region( sprev_rgn);} goto _LL2332; _LL2362:{ struct _RegionHandle _temp2868=
_new_region(); struct _RegionHandle* uprev_rgn=& _temp2868; _push_region(
uprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for( 0; _temp2445 != 0;
_temp2445=(( struct Cyc_List_List*) _check_null( _temp2445))->tl){ struct Cyc_Absyn_Structfield
_temp2871; struct Cyc_List_List* _temp2872; struct Cyc_Core_Opt* _temp2874; void*
_temp2876; struct Cyc_Absyn_Tqual _temp2878; struct _tagged_arr* _temp2880;
struct Cyc_Absyn_Structfield* _temp2869=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2445))->hd; _temp2871=* _temp2869;
_LL2881: _temp2880= _temp2871.name; goto _LL2879; _LL2879: _temp2878= _temp2871.tq;
goto _LL2877; _LL2877: _temp2876=( void*) _temp2871.type; goto _LL2875; _LL2875:
_temp2874= _temp2871.width; goto _LL2873; _LL2873: _temp2872= _temp2871.attributes;
goto _LL2870; _LL2870: if((( int(*)( int(* compare)( struct _tagged_arr*, struct
_tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields, _temp2880)){({ struct Cyc_Stdio_String_pa_struct
_temp2883; _temp2883.tag= Cyc_Stdio_String_pa; _temp2883.f1=( struct _tagged_arr)*
_temp2880;{ void* _temp2882[ 1u]={& _temp2883}; Cyc_Tcutil_terr( loc, _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u), _tag_arr( _temp2882, sizeof( void*), 1u));}});}
if( Cyc_String_strcmp(* _temp2880, _tag_arr("", sizeof( unsigned char), 1u)) !=
0){ prev_fields=({ struct Cyc_List_List* _temp2884=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp2884->hd=( void*)
_temp2880; _temp2884->tl= prev_fields; _temp2884;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp2876); if( ! Cyc_Tcutil_bits_only(
_temp2876)){({ struct Cyc_Stdio_String_pa_struct _temp2886; _temp2886.tag= Cyc_Stdio_String_pa;
_temp2886.f1=( struct _tagged_arr)* _temp2880;{ void* _temp2885[ 1u]={&
_temp2886}; Cyc_Tcutil_terr( loc, _tag_arr("%s has a type that is possibly incompatible with other members of union",
sizeof( unsigned char), 72u), _tag_arr( _temp2885, sizeof( void*), 1u));}});}
if( _temp2874 != 0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,(
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp2874))->v)){({
struct Cyc_Stdio_String_pa_struct _temp2888; _temp2888.tag= Cyc_Stdio_String_pa;
_temp2888.f1=( struct _tagged_arr)* _temp2880;{ void* _temp2887[ 1u]={&
_temp2888}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield %s does not have constant width",
sizeof( unsigned char), 41u), _tag_arr( _temp2887, sizeof( void*), 1u));}});}
else{ w= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*)
_check_null( _temp2874))->v);}{ void* _temp2889= Cyc_Tcutil_compress( _temp2876);
void* _temp2895; _LL2891: if(( unsigned int) _temp2889 > 4u?*(( int*) _temp2889)
== Cyc_Absyn_IntType: 0){ _LL2896: _temp2895=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2889)->f2; goto _LL2892;} else{ goto _LL2893;} _LL2893: goto _LL2894;
_LL2892:{ void* _temp2897= _temp2895; _LL2899: if( _temp2897 ==( void*) Cyc_Absyn_B1){
goto _LL2900;} else{ goto _LL2901;} _LL2901: if( _temp2897 ==( void*) Cyc_Absyn_B2){
goto _LL2902;} else{ goto _LL2903;} _LL2903: if( _temp2897 ==( void*) Cyc_Absyn_B4){
goto _LL2904;} else{ goto _LL2905;} _LL2905: if( _temp2897 ==( void*) Cyc_Absyn_B8){
goto _LL2906;} else{ goto _LL2898;} _LL2900: if( w > 8){({ void* _temp2907[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type", sizeof(
unsigned char), 26u), _tag_arr( _temp2907, sizeof( void*), 0u));});} goto
_LL2898; _LL2902: if( w > 16){({ void* _temp2908[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("bitfield larger than type", sizeof( unsigned char), 26u), _tag_arr(
_temp2908, sizeof( void*), 0u));});} goto _LL2898; _LL2904: if( w > 32){({ void*
_temp2909[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u), _tag_arr( _temp2909, sizeof( void*), 0u));});}
goto _LL2898; _LL2906: if( w > 64){({ void* _temp2910[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u),
_tag_arr( _temp2910, sizeof( void*), 0u));});} goto _LL2898; _LL2898:;} goto
_LL2890; _LL2894:({ struct Cyc_Stdio_String_pa_struct _temp2913; _temp2913.tag=
Cyc_Stdio_String_pa; _temp2913.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp2876);{ struct Cyc_Stdio_String_pa_struct _temp2912; _temp2912.tag= Cyc_Stdio_String_pa;
_temp2912.f1=( struct _tagged_arr)* _temp2880;{ void* _temp2911[ 2u]={&
_temp2912,& _temp2913}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield %s must have integral type but has type %s",
sizeof( unsigned char), 52u), _tag_arr( _temp2911, sizeof( void*), 2u));}}});
goto _LL2890; _LL2890:;}} for( 0; _temp2872 != 0; _temp2872=(( struct Cyc_List_List*)
_check_null( _temp2872))->tl){ void* _temp2914=( void*)(( struct Cyc_List_List*)
_check_null( _temp2872))->hd; _LL2916: if( _temp2914 ==( void*) Cyc_Absyn_Packed_att){
goto _LL2917;} else{ goto _LL2918;} _LL2918: if(( unsigned int) _temp2914 > 16u?*((
int*) _temp2914) == Cyc_Absyn_Aligned_att: 0){ goto _LL2919;} else{ goto _LL2920;}
_LL2920: goto _LL2921; _LL2917: continue; _LL2919: continue; _LL2921:({ struct
Cyc_Stdio_String_pa_struct _temp2924; _temp2924.tag= Cyc_Stdio_String_pa;
_temp2924.f1=( struct _tagged_arr)* _temp2880;{ struct Cyc_Stdio_String_pa_struct
_temp2923; _temp2923.tag= Cyc_Stdio_String_pa; _temp2923.f1=( struct _tagged_arr)
Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*) _check_null(
_temp2872))->hd);{ void* _temp2922[ 2u]={& _temp2923,& _temp2924}; Cyc_Tcutil_terr(
loc, _tag_arr("bad attribute %s on struct member %s", sizeof( unsigned char), 37u),
_tag_arr( _temp2922, sizeof( void*), 2u));}}}); goto _LL2915; _LL2915:;}}};
_pop_region( uprev_rgn);} goto _LL2332; _LL2364: if( _temp2452 == 0){ if(*
_temp2449 == 0){({ void* _temp2925[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bad anonymous struct",
sizeof( unsigned char), 21u), _tag_arr( _temp2925, sizeof( void*), 0u));});
return cvtenv;}{ struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**)
_check_null(* _temp2449));;}} if(* _temp2449 == 0){ struct _handler_cons
_temp2926; _push_handler(& _temp2926);{ int _temp2928= 0; if( setjmp( _temp2926.handler)){
_temp2928= 1;} if( ! _temp2928){* _temp2449=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc,( struct _tuple1*) _check_null( _temp2452));; _pop_handler();} else{
void* _temp2927=( void*) _exn_thrown; void* _temp2930= _temp2927; _LL2932: if(
_temp2930 == Cyc_Dict_Absent){ goto _LL2933;} else{ goto _LL2934;} _LL2934: goto
_LL2935; _LL2933:({ struct Cyc_Stdio_String_pa_struct _temp2937; _temp2937.tag=
Cyc_Stdio_String_pa; _temp2937.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp2452));{ void* _temp2936[ 1u]={& _temp2937};
Cyc_Tcutil_terr( loc, _tag_arr("unbound type struct %s", sizeof( unsigned char),
23u), _tag_arr( _temp2936, sizeof( void*), 1u));}}); return cvtenv; _LL2935:(
void) _throw( _temp2930); _LL2931:;}}}{ struct Cyc_Absyn_Structdecl* sd=*((
struct Cyc_Absyn_Structdecl**) _check_null(* _temp2449)); if( sd->name != 0){*((
struct _tuple1*) _check_null( _temp2452))=(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v)[ _check_known_subscript_notnull( 1u, 0)];}{ struct
Cyc_List_List* tvs= sd->tvs; int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
tvs); int largs= Cyc_List_length( _temp2450); if( lvs != largs){({ struct Cyc_Stdio_Int_pa_struct
_temp2941; _temp2941.tag= Cyc_Stdio_Int_pa; _temp2941.f1=( int)(( unsigned int)
largs);{ struct Cyc_Stdio_Int_pa_struct _temp2940; _temp2940.tag= Cyc_Stdio_Int_pa;
_temp2940.f1=( int)(( unsigned int) lvs);{ struct Cyc_Stdio_String_pa_struct
_temp2939; _temp2939.tag= Cyc_Stdio_String_pa; _temp2939.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp2452));{ void*
_temp2938[ 3u]={& _temp2939,& _temp2940,& _temp2941}; Cyc_Tcutil_terr( loc,
_tag_arr("struct %s expects %d type arguments but was given %d", sizeof(
unsigned char), 53u), _tag_arr( _temp2938, sizeof( void*), 3u));}}}});} for( 0;
_temp2450 != 0;( _temp2450=(( struct Cyc_List_List*) _check_null( _temp2450))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2450))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} goto _LL2332;}}
_LL2366: if( _temp2459 == 0){({ void* _temp2942[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("missing union name", sizeof( unsigned char), 19u), _tag_arr( _temp2942,
sizeof( void*), 0u));}); return cvtenv;} if(* _temp2456 == 0){ struct
_handler_cons _temp2943; _push_handler(& _temp2943);{ int _temp2945= 0; if(
setjmp( _temp2943.handler)){ _temp2945= 1;} if( ! _temp2945){* _temp2456=(
struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl( te, loc,( struct
_tuple1*) _check_null( _temp2459));; _pop_handler();} else{ void* _temp2944=(
void*) _exn_thrown; void* _temp2947= _temp2944; _LL2949: if( _temp2947 == Cyc_Dict_Absent){
goto _LL2950;} else{ goto _LL2951;} _LL2951: goto _LL2952; _LL2950:({ struct Cyc_Stdio_String_pa_struct
_temp2954; _temp2954.tag= Cyc_Stdio_String_pa; _temp2954.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp2459));{ void*
_temp2953[ 1u]={& _temp2954}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type union %s",
sizeof( unsigned char), 22u), _tag_arr( _temp2953, sizeof( void*), 1u));}});
return cvtenv; _LL2952:( void) _throw( _temp2947); _LL2948:;}}}{ struct Cyc_Absyn_Uniondecl*
ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null(* _temp2456)); if( ud->name !=
0){*(( struct _tuple1*) _check_null( _temp2459))=(( struct _tuple1*)(( struct
Cyc_Core_Opt*) _check_null( ud->name))->v)[ _check_known_subscript_notnull( 1u,
0)];}{ struct Cyc_List_List* tvs= ud->tvs; int lvs=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( tvs); int largs= Cyc_List_length( _temp2457); if( lvs !=
largs){({ struct Cyc_Stdio_Int_pa_struct _temp2958; _temp2958.tag= Cyc_Stdio_Int_pa;
_temp2958.f1=( int)(( unsigned int) largs);{ struct Cyc_Stdio_Int_pa_struct
_temp2957; _temp2957.tag= Cyc_Stdio_Int_pa; _temp2957.f1=( int)(( unsigned int)
lvs);{ struct Cyc_Stdio_String_pa_struct _temp2956; _temp2956.tag= Cyc_Stdio_String_pa;
_temp2956.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(( struct _tuple1*)
_check_null( _temp2459));{ void* _temp2955[ 3u]={& _temp2956,& _temp2957,&
_temp2958}; Cyc_Tcutil_terr( loc, _tag_arr("union %s expects %d type arguments but was given %d",
sizeof( unsigned char), 52u), _tag_arr( _temp2955, sizeof( void*), 3u));}}}});}
for( 0; _temp2457 != 0;( _temp2457=(( struct Cyc_List_List*) _check_null(
_temp2457))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2457))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} goto _LL2332;}}
_LL2368: { struct Cyc_Absyn_Typedefdecl* td;{ struct _handler_cons _temp2959;
_push_handler(& _temp2959);{ int _temp2961= 0; if( setjmp( _temp2959.handler)){
_temp2961= 1;} if( ! _temp2961){ td= Cyc_Tcenv_lookup_typedefdecl( te, loc,
_temp2466);; _pop_handler();} else{ void* _temp2960=( void*) _exn_thrown; void*
_temp2963= _temp2960; _LL2965: if( _temp2963 == Cyc_Dict_Absent){ goto _LL2966;}
else{ goto _LL2967;} _LL2967: goto _LL2968; _LL2966:({ struct Cyc_Stdio_String_pa_struct
_temp2970; _temp2970.tag= Cyc_Stdio_String_pa; _temp2970.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2466);{ void* _temp2969[ 1u]={& _temp2970}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound typedef name %s", sizeof( unsigned char), 24u), _tag_arr(
_temp2969, sizeof( void*), 1u));}}); return cvtenv; _LL2968:( void) _throw(
_temp2963); _LL2964:;}}} _temp2466[ _check_known_subscript_notnull( 1u, 0)]=( td->name)[
_check_known_subscript_notnull( 1u, 0)];{ struct Cyc_List_List* tvs= td->tvs;
struct Cyc_List_List* ts= _temp2464; struct Cyc_List_List* inst= 0; for( 0; ts
!= 0? tvs != 0: 0;( ts=(( struct Cyc_List_List*) _check_null( ts))->tl, tvs=((
struct Cyc_List_List*) _check_null( tvs))->tl)){ void* k= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k,( void*)(( struct Cyc_List_List*)
_check_null( ts))->hd); inst=({ struct Cyc_List_List* _temp2971=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2971->hd=( void*)({ struct
_tuple5* _temp2972=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp2972->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
tvs))->hd; _temp2972->f2=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd;
_temp2972;}); _temp2971->tl= inst; _temp2971;});} if( tvs != 0? 1: ts != 0){({
struct Cyc_Stdio_String_pa_struct _temp2974; _temp2974.tag= Cyc_Stdio_String_pa;
_temp2974.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2466);{ void*
_temp2973[ 1u]={& _temp2974}; Cyc_Tcutil_terr( loc, _tag_arr("typedef %s expects a different number of arguments",
sizeof( unsigned char), 51u), _tag_arr( _temp2973, sizeof( void*), 1u));}});
return cvtenv;}{ void* new_typ= Cyc_Tcutil_substitute( inst,( void*) td->defn);*
_temp2463=({ struct Cyc_Core_Opt* _temp2975=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp2975->v=( void*) new_typ; _temp2975;}); goto
_LL2332;}}} _LL2370: goto _LL2332; _LL2372: _temp2470= _temp2468; goto _LL2374;
_LL2374: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2470); goto _LL2332; _LL2376: for( 0; _temp2472 != 0; _temp2472=(( struct
Cyc_List_List*) _check_null( _temp2472))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_List_List*)
_check_null( _temp2472))->hd);} goto _LL2332; _LL2332:;} if( ! Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind( t), expected_kind)){({ struct Cyc_Stdio_String_pa_struct
_temp2979; _temp2979.tag= Cyc_Stdio_String_pa; _temp2979.f1=( struct _tagged_arr)
Cyc_Absynpp_kind2string( expected_kind);{ struct Cyc_Stdio_String_pa_struct
_temp2978; _temp2978.tag= Cyc_Stdio_String_pa; _temp2978.f1=( struct _tagged_arr)
Cyc_Absynpp_kind2string( Cyc_Tcutil_typ_kind( t));{ struct Cyc_Stdio_String_pa_struct
_temp2977; _temp2977.tag= Cyc_Stdio_String_pa; _temp2977.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp2976[ 3u]={& _temp2977,& _temp2978,&
_temp2979}; Cyc_Tcutil_terr( loc, _tag_arr("type %s has kind %s but as used here needs kind %s",
sizeof( unsigned char), 51u), _tag_arr( _temp2976, sizeof( void*), 3u));}}}});}
return cvtenv;} struct Cyc_List_List* Cyc_Tcutil_check_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* kind_env, void*
expected_kind, void* t){ struct Cyc_Tcutil_CVTEnv _temp2980= Cyc_Tcutil_i_check_valid_type(
loc, te,({ struct Cyc_Tcutil_CVTEnv _temp2995; _temp2995.kind_env= kind_env;
_temp2995.free_vars= 0; _temp2995.free_evars= 0; _temp2995;}), expected_kind, t);{
struct Cyc_List_List* vs= _temp2980.free_vars; for( 0; vs != 0; vs=(( struct Cyc_List_List*)
_check_null( vs))->tl){ Cyc_Tcutil_fast_add_free_tvar( kind_env,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( vs))->hd);}}{ struct Cyc_List_List* evs=
_temp2980.free_evars; for( 0; evs != 0; evs=(( struct Cyc_List_List*)
_check_null( evs))->tl){ void* _temp2981= Cyc_Tcutil_compress(( void*)(( struct
Cyc_List_List*) _check_null( evs))->hd); struct Cyc_Core_Opt* _temp2987; struct
Cyc_Core_Opt** _temp2989; _LL2983: if(( unsigned int) _temp2981 > 4u?*(( int*)
_temp2981) == Cyc_Absyn_Evar: 0){ _LL2988: _temp2987=(( struct Cyc_Absyn_Evar_struct*)
_temp2981)->f4; _temp2989=&(( struct Cyc_Absyn_Evar_struct*) _temp2981)->f4;
goto _LL2984;} else{ goto _LL2985;} _LL2985: goto _LL2986; _LL2984: if(*
_temp2989 == 0){* _temp2989=({ struct Cyc_Core_Opt* _temp2990=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2990->v=( void*) kind_env;
_temp2990;});} else{ struct Cyc_List_List* _temp2991=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(* _temp2989))->v; struct Cyc_List_List*
_temp2992= 0; for( 0; _temp2991 != 0; _temp2991=(( struct Cyc_List_List*)
_check_null( _temp2991))->tl){ if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp2991))->hd)){ _temp2992=({ struct Cyc_List_List* _temp2993=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2993->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp2991))->hd);
_temp2993->tl= _temp2992; _temp2993;});}}* _temp2989=({ struct Cyc_Core_Opt*
_temp2994=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2994->v=( void*) _temp2992; _temp2994;});} goto _LL2982; _LL2986: goto
_LL2982; _LL2982:;}} return _temp2980.free_vars;} void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, void* t){ struct
Cyc_List_List* _temp2996= Cyc_Tcutil_check_valid_type( loc, te, 0,( void*) Cyc_Absyn_MemKind,
t);{ struct Cyc_List_List* x= _temp2996; for( 0; x != 0; x=(( struct Cyc_List_List*)
_check_null( x))->tl){ struct Cyc_Absyn_Conref* c= Cyc_Absyn_compress_conref(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( x))->hd)->kind);
void* _temp2997=( void*) c->v; void* _temp3005; _LL2999: if( _temp2997 ==( void*)
Cyc_Absyn_No_constr){ goto _LL3000;} else{ goto _LL3001;} _LL3001: if((
unsigned int) _temp2997 > 1u?*(( int*) _temp2997) == Cyc_Absyn_Eq_constr: 0){
_LL3006: _temp3005=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2997)->f1;
if( _temp3005 ==( void*) Cyc_Absyn_MemKind){ goto _LL3002;} else{ goto _LL3003;}}
else{ goto _LL3003;} _LL3003: goto _LL3004; _LL3000: goto _LL3002; _LL3002:(
void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3007=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3007[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3008; _temp3008.tag= Cyc_Absyn_Eq_constr;
_temp3008.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3008;}); _temp3007;})));
goto _LL2998; _LL3004: goto _LL2998; _LL2998:;}} if( _temp2996 != 0){ void*
_temp3009= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp3015; struct
Cyc_List_List* _temp3017; struct Cyc_List_List* _temp3019; struct Cyc_Absyn_VarargInfo*
_temp3021; int _temp3023; struct Cyc_List_List* _temp3025; void* _temp3027;
struct Cyc_Core_Opt* _temp3029; struct Cyc_List_List* _temp3031; struct Cyc_List_List**
_temp3033; _LL3011: if(( unsigned int) _temp3009 > 4u?*(( int*) _temp3009) ==
Cyc_Absyn_FnType: 0){ _LL3016: _temp3015=(( struct Cyc_Absyn_FnType_struct*)
_temp3009)->f1; _LL3032: _temp3031= _temp3015.tvars; _temp3033=&((( struct Cyc_Absyn_FnType_struct*)
_temp3009)->f1).tvars; goto _LL3030; _LL3030: _temp3029= _temp3015.effect; goto
_LL3028; _LL3028: _temp3027=( void*) _temp3015.ret_typ; goto _LL3026; _LL3026:
_temp3025= _temp3015.args; goto _LL3024; _LL3024: _temp3023= _temp3015.c_varargs;
goto _LL3022; _LL3022: _temp3021= _temp3015.cyc_varargs; goto _LL3020; _LL3020:
_temp3019= _temp3015.rgn_po; goto _LL3018; _LL3018: _temp3017= _temp3015.attributes;
goto _LL3012;} else{ goto _LL3013;} _LL3013: goto _LL3014; _LL3012: if(*
_temp3033 == 0){* _temp3033= _temp2996;} else{ if( _temp2996 != 0){ struct
_tagged_arr _temp3034=*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp2996))->hd)->name; if(*(( const unsigned char*)
_check_unknown_subscript( _temp3034, sizeof( unsigned char), 0)) =='?'){({ void*
_temp3035[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("You failed to abstract over the generated type variables. \nPlease remove the bound type variables or else make the effects of the type explicit\nand then abstract the free type variables properly. (Sorry.)",
sizeof( unsigned char), 205u), _tag_arr( _temp3035, sizeof( void*), 0u));});}
else{({ struct Cyc_Stdio_String_pa_struct _temp3037; _temp3037.tag= Cyc_Stdio_String_pa;
_temp3037.f1=( struct _tagged_arr) _temp3034;{ void* _temp3036[ 1u]={& _temp3037};
Cyc_Tcutil_terr( loc, _tag_arr("unbound type variable %s", sizeof( unsigned char),
25u), _tag_arr( _temp3036, sizeof( void*), 1u));}});}}} goto _LL3010; _LL3014:({
struct Cyc_Stdio_String_pa_struct _temp3039; _temp3039.tag= Cyc_Stdio_String_pa;
_temp3039.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp2996))->hd)->name;{ void* _temp3038[ 1u]={& _temp3039}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound type variable %s", sizeof( unsigned char), 25u), _tag_arr(
_temp3038, sizeof( void*), 1u));}}); return; _LL3010:;}} void Cyc_Tcutil_add_tvar_identity(
struct Cyc_Absyn_Tvar* tv){ if( tv->identity == 0){ tv->identity= Cyc_Tcutil_new_tvar_id();}}
void Cyc_Tcutil_add_tvar_identities( struct Cyc_List_List* tvs){(( void(*)( void(*
f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Tcutil_add_tvar_identity,
tvs);} void Cyc_Tcutil_check_type( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* bound_tvars, void* expected_kind, void* t){ struct Cyc_List_List*
_temp3040= Cyc_Tcutil_check_valid_type( loc, te, bound_tvars, expected_kind, t);
struct Cyc_List_List* _temp3041= Cyc_Tcutil_remove_bound_tvars( _temp3040,
bound_tvars); struct Cyc_List_List* fs= _temp3041; for( 0; fs != 0; fs=(( struct
Cyc_List_List*) _check_null( fs))->tl){ struct _tagged_arr* _temp3042=(( struct
Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( fs))->hd)->name;({ struct
Cyc_Stdio_String_pa_struct _temp3045; _temp3045.tag= Cyc_Stdio_String_pa;
_temp3045.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ struct Cyc_Stdio_String_pa_struct
_temp3044; _temp3044.tag= Cyc_Stdio_String_pa; _temp3044.f1=( struct _tagged_arr)*
_temp3042;{ void* _temp3043[ 2u]={& _temp3044,& _temp3045}; Cyc_Tcutil_terr( loc,
_tag_arr("unbound type variable %s in type %s", sizeof( unsigned char), 36u),
_tag_arr( _temp3043, sizeof( void*), 2u));}}});}} static void Cyc_Tcutil_check_unique_unsorted(
int(* cmp)( void*, void*), struct Cyc_List_List* vs, struct Cyc_Position_Segment*
loc, struct _tagged_arr(* a2string)( void*), struct _tagged_arr msg){ for( 0; vs
!= 0; vs=(( struct Cyc_List_List*) _check_null( vs))->tl){ struct Cyc_List_List*
vs2=(( struct Cyc_List_List*) _check_null( vs))->tl; for( 0; vs2 != 0; vs2=((
struct Cyc_List_List*) _check_null( vs2))->tl){ if( cmp(( void*)(( struct Cyc_List_List*)
_check_null( vs))->hd,( void*)(( struct Cyc_List_List*) _check_null( vs2))->hd)
== 0){({ struct Cyc_Stdio_String_pa_struct _temp3048; _temp3048.tag= Cyc_Stdio_String_pa;
_temp3048.f1=( struct _tagged_arr) a2string(( void*)(( struct Cyc_List_List*)
_check_null( vs))->hd);{ struct Cyc_Stdio_String_pa_struct _temp3047; _temp3047.tag=
Cyc_Stdio_String_pa; _temp3047.f1=( struct _tagged_arr) msg;{ void* _temp3046[ 2u]={&
_temp3047,& _temp3048}; Cyc_Tcutil_terr( loc, _tag_arr("%s: %s", sizeof(
unsigned char), 7u), _tag_arr( _temp3046, sizeof( void*), 2u));}}});}}}} static
struct _tagged_arr Cyc_Tcutil_strptr2string( struct _tagged_arr* s){ return* s;}
void Cyc_Tcutil_check_unique_vars( struct Cyc_List_List* vs, struct Cyc_Position_Segment*
loc, struct _tagged_arr msg){(( void(*)( int(* cmp)( struct _tagged_arr*, struct
_tagged_arr*), struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc,
struct _tagged_arr(* a2string)( struct _tagged_arr*), struct _tagged_arr msg))
Cyc_Tcutil_check_unique_unsorted)( Cyc_String_zstrptrcmp, vs, loc, Cyc_Tcutil_strptr2string,
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
_temp3049=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3049->hd=( void*)({ struct _tuple17* _temp3050=( struct _tuple17*)
GC_malloc( sizeof( struct _tuple17)); _temp3050->f1=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( sd_fields))->hd; _temp3050->f2= 0; _temp3050;});
_temp3049->tl= fields; _temp3049;});}}} fields=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields);{ struct Cyc_List_List* ans=
0; for( 0; des != 0; des=(( struct Cyc_List_List*) _check_null( des))->tl){
struct _tuple18 _temp3053; void* _temp3054; struct Cyc_List_List* _temp3056;
struct _tuple18* _temp3051=( struct _tuple18*)(( struct Cyc_List_List*)
_check_null( des))->hd; _temp3053=* _temp3051; _LL3057: _temp3056= _temp3053.f1;
goto _LL3055; _LL3055: _temp3054= _temp3053.f2; goto _LL3052; _LL3052: if(
_temp3056 == 0){ struct Cyc_List_List* _temp3058= fields; for( 0; _temp3058 != 0;
_temp3058=(( struct Cyc_List_List*) _check_null( _temp3058))->tl){ if( !(*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp3058))->hd)).f2){(*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp3058))->hd)).f2= 1;(*((
struct _tuple18*)(( struct Cyc_List_List*) _check_null( des))->hd)).f1=({ struct
Cyc_List_List* _temp3059=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3059->hd=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp3060=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp3060[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp3061; _temp3061.tag= Cyc_Absyn_FieldName;
_temp3061.f1=((*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3058))->hd)).f1)->name; _temp3061;}); _temp3060;})); _temp3059->tl= 0;
_temp3059;}); ans=({ struct Cyc_List_List* _temp3062=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp3062->hd=( void*)({
struct _tuple19* _temp3063=( struct _tuple19*) _region_malloc( rgn, sizeof(
struct _tuple19)); _temp3063->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3058))->hd)).f1; _temp3063->f2= _temp3054; _temp3063;});
_temp3062->tl= ans; _temp3062;}); break;}} if( _temp3058 == 0){({ void*
_temp3064[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("too many arguments to struct",
sizeof( unsigned char), 29u), _tag_arr( _temp3064, sizeof( void*), 0u));});}}
else{ if((( struct Cyc_List_List*) _check_null( _temp3056))->tl != 0){({ void*
_temp3065[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("multiple designators are not supported",
sizeof( unsigned char), 39u), _tag_arr( _temp3065, sizeof( void*), 0u));});}
else{ void* _temp3066=( void*)(( struct Cyc_List_List*) _check_null( _temp3056))->hd;
struct _tagged_arr* _temp3072; _LL3068: if(*(( int*) _temp3066) == Cyc_Absyn_ArrayElement){
goto _LL3069;} else{ goto _LL3070;} _LL3070: if(*(( int*) _temp3066) == Cyc_Absyn_FieldName){
_LL3073: _temp3072=(( struct Cyc_Absyn_FieldName_struct*) _temp3066)->f1; goto
_LL3071;} else{ goto _LL3067;} _LL3069:({ void* _temp3074[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("array designator used in argument to struct", sizeof(
unsigned char), 44u), _tag_arr( _temp3074, sizeof( void*), 0u));}); goto _LL3067;
_LL3071: { struct Cyc_List_List* _temp3075= fields; for( 0; _temp3075 != 0;
_temp3075=(( struct Cyc_List_List*) _check_null( _temp3075))->tl){ if( Cyc_String_zstrptrcmp(
_temp3072,((*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3075))->hd)).f1)->name) == 0){ if((*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3075))->hd)).f2){({ struct Cyc_Stdio_String_pa_struct
_temp3077; _temp3077.tag= Cyc_Stdio_String_pa; _temp3077.f1=( struct _tagged_arr)*
_temp3072;{ void* _temp3076[ 1u]={& _temp3077}; Cyc_Tcutil_terr( loc, _tag_arr("field %s has already been used as an argument",
sizeof( unsigned char), 46u), _tag_arr( _temp3076, sizeof( void*), 1u));}});}(*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp3075))->hd)).f2= 1;
ans=({ struct Cyc_List_List* _temp3078=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp3078->hd=( void*)({ struct _tuple19*
_temp3079=( struct _tuple19*) _region_malloc( rgn, sizeof( struct _tuple19));
_temp3079->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3075))->hd)).f1; _temp3079->f2= _temp3054; _temp3079;}); _temp3078->tl= ans;
_temp3078;}); break;}} if( _temp3075 == 0){({ struct Cyc_Stdio_String_pa_struct
_temp3081; _temp3081.tag= Cyc_Stdio_String_pa; _temp3081.f1=( struct _tagged_arr)*
_temp3072;{ void* _temp3080[ 1u]={& _temp3081}; Cyc_Tcutil_terr( loc, _tag_arr("bad field designator %s",
sizeof( unsigned char), 24u), _tag_arr( _temp3080, sizeof( void*), 1u));}});}
goto _LL3067;} _LL3067:;}}} for( 0; fields != 0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ if( !(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( fields))->hd)).f2){({ void* _temp3082[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("too few arguments to struct", sizeof( unsigned char), 28u), _tag_arr(
_temp3082, sizeof( void*), 0u));}); break;}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( ans);}} int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void* t, void** elt_typ_dest){ void* _temp3083= Cyc_Tcutil_compress( t); struct
Cyc_Absyn_PtrInfo _temp3089; struct Cyc_Absyn_Conref* _temp3091; void* _temp3093;
_LL3085: if(( unsigned int) _temp3083 > 4u?*(( int*) _temp3083) == Cyc_Absyn_PointerType:
0){ _LL3090: _temp3089=(( struct Cyc_Absyn_PointerType_struct*) _temp3083)->f1;
_LL3094: _temp3093=( void*) _temp3089.elt_typ; goto _LL3092; _LL3092: _temp3091=
_temp3089.bounds; goto _LL3086;} else{ goto _LL3087;} _LL3087: goto _LL3088;
_LL3086: { struct Cyc_Absyn_Conref* _temp3095= Cyc_Absyn_compress_conref(
_temp3091); void* _temp3096=( void*)( Cyc_Absyn_compress_conref( _temp3095))->v;
void* _temp3104; _LL3098: if(( unsigned int) _temp3096 > 1u?*(( int*) _temp3096)
== Cyc_Absyn_Eq_constr: 0){ _LL3105: _temp3104=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3096)->f1; if( _temp3104 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3099;}
else{ goto _LL3100;}} else{ goto _LL3100;} _LL3100: if( _temp3096 ==( void*) Cyc_Absyn_No_constr){
goto _LL3101;} else{ goto _LL3102;} _LL3102: goto _LL3103; _LL3099:*
elt_typ_dest= _temp3093; return 1; _LL3101:( void*)( _temp3095->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3106=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3106[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3107; _temp3107.tag= Cyc_Absyn_Eq_constr;
_temp3107.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp3107;}); _temp3106;})));*
elt_typ_dest= _temp3093; return 1; _LL3103: return 0; _LL3097:;} _LL3088: return
0; _LL3084:;} int Cyc_Tcutil_is_tagged_pointer_typ( void* t){ void* ignore=(
void*) Cyc_Absyn_VoidType; return Cyc_Tcutil_is_tagged_pointer_typ_elt( t,&
ignore);} struct _tuple7 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ struct _tuple7 bogus_ans=({ struct _tuple7 _temp3290;
_temp3290.f1= 0; _temp3290.f2=( void*) Cyc_Absyn_HeapRgn; _temp3290;}); void*
_temp3108=( void*) e->r; void* _temp3122; struct _tuple1* _temp3124; struct
_tagged_arr* _temp3126; struct Cyc_Absyn_Exp* _temp3128; struct _tagged_arr*
_temp3130; struct Cyc_Absyn_Exp* _temp3132; struct Cyc_Absyn_Exp* _temp3134;
struct Cyc_Absyn_Exp* _temp3136; struct Cyc_Absyn_Exp* _temp3138; _LL3110: if(*((
int*) _temp3108) == Cyc_Absyn_Var_e){ _LL3125: _temp3124=(( struct Cyc_Absyn_Var_e_struct*)
_temp3108)->f1; goto _LL3123; _LL3123: _temp3122=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3108)->f2; goto _LL3111;} else{ goto _LL3112;} _LL3112: if(*(( int*)
_temp3108) == Cyc_Absyn_StructMember_e){ _LL3129: _temp3128=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3108)->f1; goto _LL3127; _LL3127: _temp3126=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3108)->f2; goto _LL3113;} else{ goto _LL3114;} _LL3114: if(*(( int*)
_temp3108) == Cyc_Absyn_StructArrow_e){ _LL3133: _temp3132=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3108)->f1; goto _LL3131; _LL3131: _temp3130=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3108)->f2; goto _LL3115;} else{ goto _LL3116;} _LL3116: if(*(( int*)
_temp3108) == Cyc_Absyn_Deref_e){ _LL3135: _temp3134=(( struct Cyc_Absyn_Deref_e_struct*)
_temp3108)->f1; goto _LL3117;} else{ goto _LL3118;} _LL3118: if(*(( int*)
_temp3108) == Cyc_Absyn_Subscript_e){ _LL3139: _temp3138=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3108)->f1; goto _LL3137; _LL3137: _temp3136=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3108)->f2; goto _LL3119;} else{ goto _LL3120;} _LL3120: goto _LL3121;
_LL3111: { void* _temp3140= _temp3122; struct Cyc_Absyn_Vardecl* _temp3154;
struct Cyc_Absyn_Vardecl* _temp3156; struct Cyc_Absyn_Vardecl* _temp3158; struct
Cyc_Absyn_Vardecl* _temp3160; _LL3142: if( _temp3140 ==( void*) Cyc_Absyn_Unresolved_b){
goto _LL3143;} else{ goto _LL3144;} _LL3144: if(( unsigned int) _temp3140 > 1u?*((
int*) _temp3140) == Cyc_Absyn_Funname_b: 0){ goto _LL3145;} else{ goto _LL3146;}
_LL3146: if(( unsigned int) _temp3140 > 1u?*(( int*) _temp3140) == Cyc_Absyn_Global_b:
0){ _LL3155: _temp3154=(( struct Cyc_Absyn_Global_b_struct*) _temp3140)->f1;
goto _LL3147;} else{ goto _LL3148;} _LL3148: if(( unsigned int) _temp3140 > 1u?*((
int*) _temp3140) == Cyc_Absyn_Local_b: 0){ _LL3157: _temp3156=(( struct Cyc_Absyn_Local_b_struct*)
_temp3140)->f1; goto _LL3149;} else{ goto _LL3150;} _LL3150: if(( unsigned int)
_temp3140 > 1u?*(( int*) _temp3140) == Cyc_Absyn_Pat_b: 0){ _LL3159: _temp3158=((
struct Cyc_Absyn_Pat_b_struct*) _temp3140)->f1; goto _LL3151;} else{ goto
_LL3152;} _LL3152: if(( unsigned int) _temp3140 > 1u?*(( int*) _temp3140) == Cyc_Absyn_Param_b:
0){ _LL3161: _temp3160=(( struct Cyc_Absyn_Param_b_struct*) _temp3140)->f1; goto
_LL3153;} else{ goto _LL3141;} _LL3143: return bogus_ans; _LL3145: return({
struct _tuple7 _temp3162; _temp3162.f1= 1; _temp3162.f2=( void*) Cyc_Absyn_HeapRgn;
_temp3162;}); _LL3147: { void* _temp3163= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v); _LL3165: if(( unsigned int) _temp3163
> 4u?*(( int*) _temp3163) == Cyc_Absyn_ArrayType: 0){ goto _LL3166;} else{ goto
_LL3167;} _LL3167: goto _LL3168; _LL3166: return({ struct _tuple7 _temp3169;
_temp3169.f1= 1; _temp3169.f2=( void*) Cyc_Absyn_HeapRgn; _temp3169;}); _LL3168:
return({ struct _tuple7 _temp3170; _temp3170.f1=( _temp3154->tq).q_const;
_temp3170.f2=( void*) Cyc_Absyn_HeapRgn; _temp3170;}); _LL3164:;} _LL3149: {
void* _temp3171= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); _LL3173: if(( unsigned int) _temp3171 > 4u?*(( int*)
_temp3171) == Cyc_Absyn_ArrayType: 0){ goto _LL3174;} else{ goto _LL3175;}
_LL3175: goto _LL3176; _LL3174: return({ struct _tuple7 _temp3177; _temp3177.f1=
1; _temp3177.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3156->rgn))->v;
_temp3177;}); _LL3176: return({ struct _tuple7 _temp3178; _temp3178.f1=(
_temp3156->tq).q_const; _temp3178.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3156->rgn))->v; _temp3178;}); _LL3172:;} _LL3151: _temp3160=
_temp3158; goto _LL3153; _LL3153: return({ struct _tuple7 _temp3179; _temp3179.f1=(
_temp3160->tq).q_const; _temp3179.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3160->rgn))->v; _temp3179;}); _LL3141:;} _LL3113: { void*
_temp3180= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp3128->topt))->v); struct Cyc_List_List* _temp3192; struct Cyc_List_List*
_temp3194; struct Cyc_Absyn_Structdecl** _temp3196; struct Cyc_Absyn_Structdecl*
_temp3198; struct Cyc_Absyn_Uniondecl** _temp3199; struct Cyc_Absyn_Uniondecl*
_temp3201; _LL3182: if(( unsigned int) _temp3180 > 4u?*(( int*) _temp3180) ==
Cyc_Absyn_AnonStructType: 0){ _LL3193: _temp3192=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3180)->f1; goto _LL3183;} else{ goto _LL3184;} _LL3184: if(( unsigned int)
_temp3180 > 4u?*(( int*) _temp3180) == Cyc_Absyn_AnonUnionType: 0){ _LL3195:
_temp3194=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3180)->f1; goto
_LL3185;} else{ goto _LL3186;} _LL3186: if(( unsigned int) _temp3180 > 4u?*((
int*) _temp3180) == Cyc_Absyn_StructType: 0){ _LL3197: _temp3196=(( struct Cyc_Absyn_StructType_struct*)
_temp3180)->f3; if( _temp3196 == 0){ goto _LL3188;} else{ _temp3198=* _temp3196;
goto _LL3187;}} else{ goto _LL3188;} _LL3188: if(( unsigned int) _temp3180 > 4u?*((
int*) _temp3180) == Cyc_Absyn_UnionType: 0){ _LL3200: _temp3199=(( struct Cyc_Absyn_UnionType_struct*)
_temp3180)->f3; if( _temp3199 == 0){ goto _LL3190;} else{ _temp3201=* _temp3199;
goto _LL3189;}} else{ goto _LL3190;} _LL3190: goto _LL3191; _LL3183: _temp3194=
_temp3192; goto _LL3185; _LL3185: { struct Cyc_Absyn_Structfield* _temp3202= Cyc_Absyn_lookup_field(
_temp3194, _temp3126); if( _temp3202 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3202))->width != 0: 0){ return({ struct _tuple7 _temp3203;
_temp3203.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3202))->tq).q_const;
_temp3203.f2=( Cyc_Tcutil_addressof_props( te, _temp3128)).f2; _temp3203;});}
return bogus_ans;} _LL3187: { struct Cyc_Absyn_Structfield* _temp3204= Cyc_Absyn_lookup_struct_field(
_temp3198, _temp3126); if( _temp3204 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3204))->width != 0: 0){ return({ struct _tuple7 _temp3205;
_temp3205.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3204))->tq).q_const;
_temp3205.f2=( Cyc_Tcutil_addressof_props( te, _temp3128)).f2; _temp3205;});}
return bogus_ans;} _LL3189: { struct Cyc_Absyn_Structfield* _temp3206= Cyc_Absyn_lookup_union_field(
_temp3201, _temp3126); if( _temp3206 != 0){ return({ struct _tuple7 _temp3207;
_temp3207.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3206))->tq).q_const;
_temp3207.f2=( Cyc_Tcutil_addressof_props( te, _temp3128)).f2; _temp3207;});}
goto _LL3191;} _LL3191: return bogus_ans; _LL3181:;} _LL3115: { void* _temp3208=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3132->topt))->v);
struct Cyc_Absyn_PtrInfo _temp3214; void* _temp3216; void* _temp3218; _LL3210:
if(( unsigned int) _temp3208 > 4u?*(( int*) _temp3208) == Cyc_Absyn_PointerType:
0){ _LL3215: _temp3214=(( struct Cyc_Absyn_PointerType_struct*) _temp3208)->f1;
_LL3219: _temp3218=( void*) _temp3214.elt_typ; goto _LL3217; _LL3217: _temp3216=(
void*) _temp3214.rgn_typ; goto _LL3211;} else{ goto _LL3212;} _LL3212: goto
_LL3213; _LL3211: { void* _temp3220= Cyc_Tcutil_compress( _temp3218); struct Cyc_List_List*
_temp3232; struct Cyc_List_List* _temp3234; struct Cyc_Absyn_Structdecl**
_temp3236; struct Cyc_Absyn_Structdecl* _temp3238; struct Cyc_Absyn_Uniondecl**
_temp3239; struct Cyc_Absyn_Uniondecl* _temp3241; _LL3222: if(( unsigned int)
_temp3220 > 4u?*(( int*) _temp3220) == Cyc_Absyn_AnonStructType: 0){ _LL3233:
_temp3232=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3220)->f1; goto
_LL3223;} else{ goto _LL3224;} _LL3224: if(( unsigned int) _temp3220 > 4u?*((
int*) _temp3220) == Cyc_Absyn_AnonUnionType: 0){ _LL3235: _temp3234=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3220)->f1; goto _LL3225;} else{ goto
_LL3226;} _LL3226: if(( unsigned int) _temp3220 > 4u?*(( int*) _temp3220) == Cyc_Absyn_StructType:
0){ _LL3237: _temp3236=(( struct Cyc_Absyn_StructType_struct*) _temp3220)->f3;
if( _temp3236 == 0){ goto _LL3228;} else{ _temp3238=* _temp3236; goto _LL3227;}}
else{ goto _LL3228;} _LL3228: if(( unsigned int) _temp3220 > 4u?*(( int*)
_temp3220) == Cyc_Absyn_UnionType: 0){ _LL3240: _temp3239=(( struct Cyc_Absyn_UnionType_struct*)
_temp3220)->f3; if( _temp3239 == 0){ goto _LL3230;} else{ _temp3241=* _temp3239;
goto _LL3229;}} else{ goto _LL3230;} _LL3230: goto _LL3231; _LL3223: _temp3234=
_temp3232; goto _LL3225; _LL3225: { struct Cyc_Absyn_Structfield* _temp3242= Cyc_Absyn_lookup_field(
_temp3234, _temp3130); if( _temp3242 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3242))->width != 0: 0){ return({ struct _tuple7 _temp3243;
_temp3243.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3242))->tq).q_const;
_temp3243.f2= _temp3216; _temp3243;});} return bogus_ans;} _LL3227: { struct Cyc_Absyn_Structfield*
_temp3244= Cyc_Absyn_lookup_struct_field( _temp3238, _temp3130); if( _temp3244
!= 0?(( struct Cyc_Absyn_Structfield*) _check_null( _temp3244))->width != 0: 0){
return({ struct _tuple7 _temp3245; _temp3245.f1=((( struct Cyc_Absyn_Structfield*)
_check_null( _temp3244))->tq).q_const; _temp3245.f2= _temp3216; _temp3245;});}
return bogus_ans;} _LL3229: { struct Cyc_Absyn_Structfield* _temp3246= Cyc_Absyn_lookup_union_field(
_temp3241, _temp3130); if( _temp3246 != 0){ return({ struct _tuple7 _temp3247;
_temp3247.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3246))->tq).q_const;
_temp3247.f2= _temp3216; _temp3247;});} return bogus_ans;} _LL3231: return
bogus_ans; _LL3221:;} _LL3213: return bogus_ans; _LL3209:;} _LL3117: { void*
_temp3248= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp3134->topt))->v); struct Cyc_Absyn_PtrInfo _temp3254; struct Cyc_Absyn_Tqual
_temp3256; void* _temp3258; _LL3250: if(( unsigned int) _temp3248 > 4u?*(( int*)
_temp3248) == Cyc_Absyn_PointerType: 0){ _LL3255: _temp3254=(( struct Cyc_Absyn_PointerType_struct*)
_temp3248)->f1; _LL3259: _temp3258=( void*) _temp3254.rgn_typ; goto _LL3257;
_LL3257: _temp3256= _temp3254.tq; goto _LL3251;} else{ goto _LL3252;} _LL3252:
goto _LL3253; _LL3251: return({ struct _tuple7 _temp3260; _temp3260.f1=
_temp3256.q_const; _temp3260.f2= _temp3258; _temp3260;}); _LL3253: return
bogus_ans; _LL3249:;} _LL3119: { void* t= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp3138->topt))->v); void* _temp3261= t; struct
Cyc_Absyn_Tqual _temp3271; struct Cyc_List_List* _temp3273; struct Cyc_Absyn_PtrInfo
_temp3275; struct Cyc_Absyn_Conref* _temp3277; struct Cyc_Absyn_Tqual _temp3279;
void* _temp3281; void* _temp3283; _LL3263: if(( unsigned int) _temp3261 > 4u?*((
int*) _temp3261) == Cyc_Absyn_ArrayType: 0){ _LL3272: _temp3271=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3261)->f2; goto _LL3264;} else{ goto _LL3265;} _LL3265: if(( unsigned int)
_temp3261 > 4u?*(( int*) _temp3261) == Cyc_Absyn_TupleType: 0){ _LL3274:
_temp3273=(( struct Cyc_Absyn_TupleType_struct*) _temp3261)->f1; goto _LL3266;}
else{ goto _LL3267;} _LL3267: if(( unsigned int) _temp3261 > 4u?*(( int*)
_temp3261) == Cyc_Absyn_PointerType: 0){ _LL3276: _temp3275=(( struct Cyc_Absyn_PointerType_struct*)
_temp3261)->f1; _LL3284: _temp3283=( void*) _temp3275.elt_typ; goto _LL3282;
_LL3282: _temp3281=( void*) _temp3275.rgn_typ; goto _LL3280; _LL3280: _temp3279=
_temp3275.tq; goto _LL3278; _LL3278: _temp3277= _temp3275.bounds; goto _LL3268;}
else{ goto _LL3269;} _LL3269: goto _LL3270; _LL3264: return({ struct _tuple7
_temp3285; _temp3285.f1= _temp3271.q_const; _temp3285.f2=( Cyc_Tcutil_addressof_props(
te, _temp3138)).f2; _temp3285;}); _LL3266: { struct _tuple4* _temp3286= Cyc_Absyn_lookup_tuple_field(
_temp3273,( int) Cyc_Evexp_eval_const_uint_exp( _temp3136)); if( _temp3286 != 0){
return({ struct _tuple7 _temp3287; _temp3287.f1=((*(( struct _tuple4*)
_check_null( _temp3286))).f1).q_const; _temp3287.f2=( Cyc_Tcutil_addressof_props(
te, _temp3138)).f2; _temp3287;});} return bogus_ans;} _LL3268: return({ struct
_tuple7 _temp3288; _temp3288.f1= _temp3279.q_const; _temp3288.f2= _temp3281;
_temp3288;}); _LL3270: return bogus_ans; _LL3262:;} _LL3121:({ void* _temp3289[
0u]={}; Cyc_Tcutil_terr( e->loc, _tag_arr("unary & applied to non-lvalue",
sizeof( unsigned char), 30u), _tag_arr( _temp3289, sizeof( void*), 0u));});
return bogus_ans; _LL3109:;} void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv*
te, void* e_typ, struct Cyc_Absyn_Exp* e){ void* _temp3291= Cyc_Tcutil_compress(
e_typ); struct Cyc_Absyn_Tqual _temp3297; void* _temp3299; _LL3293: if((
unsigned int) _temp3291 > 4u?*(( int*) _temp3291) == Cyc_Absyn_ArrayType: 0){
_LL3300: _temp3299=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3291)->f1;
goto _LL3298; _LL3298: _temp3297=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3291)->f2; goto _LL3294;} else{ goto _LL3295;} _LL3295: goto _LL3296;
_LL3294: { void* _temp3303; struct _tuple7 _temp3301= Cyc_Tcutil_addressof_props(
te, e); _LL3304: _temp3303= _temp3301.f2; goto _LL3302; _LL3302: return Cyc_Absyn_atb_typ(
_temp3299, _temp3303, _temp3297,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp3305=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3305[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3306; _temp3306.tag= Cyc_Absyn_Upper_b;
_temp3306.f1= e; _temp3306;}); _temp3305;}));} _LL3296: return e_typ; _LL3292:;}
void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment* loc, unsigned int i,
struct Cyc_Absyn_Conref* b){ b= Cyc_Absyn_compress_conref( b);{ void* _temp3307=(
void*) b->v; void* _temp3315; void* _temp3317; struct Cyc_Absyn_Exp* _temp3319;
_LL3309: if(( unsigned int) _temp3307 > 1u?*(( int*) _temp3307) == Cyc_Absyn_Eq_constr:
0){ _LL3316: _temp3315=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3307)->f1;
if( _temp3315 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3310;} else{ goto _LL3311;}}
else{ goto _LL3311;} _LL3311: if(( unsigned int) _temp3307 > 1u?*(( int*)
_temp3307) == Cyc_Absyn_Eq_constr: 0){ _LL3318: _temp3317=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3307)->f1; if(( unsigned int) _temp3317 > 1u?*(( int*) _temp3317) == Cyc_Absyn_Upper_b:
0){ _LL3320: _temp3319=(( struct Cyc_Absyn_Upper_b_struct*) _temp3317)->f1; goto
_LL3312;} else{ goto _LL3313;}} else{ goto _LL3313;} _LL3313: goto _LL3314;
_LL3310: return; _LL3312: if( Cyc_Evexp_eval_const_uint_exp( _temp3319) <= i){({
void* _temp3321[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("dereference is out of bounds",
sizeof( unsigned char), 29u), _tag_arr( _temp3321, sizeof( void*), 0u));});}
return; _LL3314:( void*)( b->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3322=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3322[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3323; _temp3323.tag= Cyc_Absyn_Eq_constr;
_temp3323.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3324=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3324[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3325; _temp3325.tag= Cyc_Absyn_Upper_b;
_temp3325.f1= Cyc_Absyn_uint_exp( i + 1, 0); _temp3325;}); _temp3324;}));
_temp3323;}); _temp3322;}))); return; _LL3308:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_bits_only( void* t){ void* _temp3326= Cyc_Tcutil_compress(
t); void* _temp3352; struct Cyc_List_List* _temp3354; struct Cyc_Absyn_Structdecl**
_temp3356; struct Cyc_List_List* _temp3358; struct Cyc_Absyn_Uniondecl**
_temp3360; struct Cyc_List_List* _temp3362; struct Cyc_List_List* _temp3364;
struct Cyc_List_List* _temp3366; _LL3328: if( _temp3326 ==( void*) Cyc_Absyn_VoidType){
goto _LL3329;} else{ goto _LL3330;} _LL3330: if(( unsigned int) _temp3326 > 4u?*((
int*) _temp3326) == Cyc_Absyn_IntType: 0){ goto _LL3331;} else{ goto _LL3332;}
_LL3332: if( _temp3326 ==( void*) Cyc_Absyn_FloatType){ goto _LL3333;} else{
goto _LL3334;} _LL3334: if( _temp3326 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL3335;} else{ goto _LL3336;} _LL3336: if(( unsigned int) _temp3326 > 4u?*((
int*) _temp3326) == Cyc_Absyn_EnumType: 0){ goto _LL3337;} else{ goto _LL3338;}
_LL3338: if(( unsigned int) _temp3326 > 4u?*(( int*) _temp3326) == Cyc_Absyn_ArrayType:
0){ _LL3353: _temp3352=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3326)->f1;
goto _LL3339;} else{ goto _LL3340;} _LL3340: if(( unsigned int) _temp3326 > 4u?*((
int*) _temp3326) == Cyc_Absyn_TupleType: 0){ _LL3355: _temp3354=(( struct Cyc_Absyn_TupleType_struct*)
_temp3326)->f1; goto _LL3341;} else{ goto _LL3342;} _LL3342: if(( unsigned int)
_temp3326 > 4u?*(( int*) _temp3326) == Cyc_Absyn_StructType: 0){ _LL3359:
_temp3358=(( struct Cyc_Absyn_StructType_struct*) _temp3326)->f2; goto _LL3357;
_LL3357: _temp3356=(( struct Cyc_Absyn_StructType_struct*) _temp3326)->f3; goto
_LL3343;} else{ goto _LL3344;} _LL3344: if(( unsigned int) _temp3326 > 4u?*((
int*) _temp3326) == Cyc_Absyn_UnionType: 0){ _LL3363: _temp3362=(( struct Cyc_Absyn_UnionType_struct*)
_temp3326)->f2; goto _LL3361; _LL3361: _temp3360=(( struct Cyc_Absyn_UnionType_struct*)
_temp3326)->f3; goto _LL3345;} else{ goto _LL3346;} _LL3346: if(( unsigned int)
_temp3326 > 4u?*(( int*) _temp3326) == Cyc_Absyn_AnonStructType: 0){ _LL3365:
_temp3364=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3326)->f1; goto
_LL3347;} else{ goto _LL3348;} _LL3348: if(( unsigned int) _temp3326 > 4u?*((
int*) _temp3326) == Cyc_Absyn_AnonUnionType: 0){ _LL3367: _temp3366=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3326)->f1; goto _LL3349;} else{ goto
_LL3350;} _LL3350: goto _LL3351; _LL3329: return 1; _LL3331: return 1; _LL3333:
return 1; _LL3335: return 1; _LL3337: return 0; _LL3339: return Cyc_Tcutil_bits_only(
_temp3352); _LL3341: for( 0; _temp3354 != 0; _temp3354=(( struct Cyc_List_List*)
_check_null( _temp3354))->tl){ if( ! Cyc_Tcutil_bits_only((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp3354))->hd)).f2)){ return 0;}} return 1;
_LL3343: if( _temp3356 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* _temp3368=*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp3356)); if( _temp3368->fields
== 0){ return 0;}{ struct _RegionHandle _temp3369= _new_region(); struct
_RegionHandle* rgn=& _temp3369; _push_region( rgn);{ struct Cyc_List_List*
_temp3370=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp3368->tvs, _temp3358);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp3368->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute(
rgn, _temp3370,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3371= 0; _npop_handler( 0u); return
_temp3371;}}}{ int _temp3372= 1; _npop_handler( 0u); return _temp3372;}};
_pop_region( rgn);}} _LL3345: if( _temp3360 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
_temp3373=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp3360)); if(
_temp3373->fields == 0){ return 0;}{ struct _RegionHandle _temp3374= _new_region();
struct _RegionHandle* rgn=& _temp3374; _push_region( rgn);{ struct Cyc_List_List*
_temp3375=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp3373->tvs, _temp3362);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp3373->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute(
rgn, _temp3375,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3376= 0; _npop_handler( 0u); return
_temp3376;}}}{ int _temp3377= 1; _npop_handler( 0u); return _temp3377;}};
_pop_region( rgn);}} _LL3347: _temp3366= _temp3364; goto _LL3349; _LL3349: for(
0; _temp3366 != 0; _temp3366=(( struct Cyc_List_List*) _check_null( _temp3366))->tl){
if( ! Cyc_Tcutil_bits_only(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp3366))->hd)->type)){ return 0;}} return 1;
_LL3351: return 0; _LL3327:;} struct _tuple20{ struct Cyc_List_List* f1; struct
Cyc_Absyn_Exp* f2; } ; static int Cyc_Tcutil_cnst_exp( struct Cyc_Tcenv_Tenv* te,
int var_okay, struct Cyc_Absyn_Exp* e){ void* _temp3378=( void*) e->r; struct
_tuple1* _temp3418; struct Cyc_Absyn_Exp* _temp3420; struct Cyc_Absyn_Exp*
_temp3422; struct Cyc_Absyn_Exp* _temp3424; struct Cyc_Absyn_Exp* _temp3426;
struct Cyc_Absyn_Exp* _temp3428; struct Cyc_Absyn_Exp* _temp3430; struct Cyc_Absyn_Exp*
_temp3432; struct Cyc_Absyn_Exp* _temp3434; void* _temp3436; struct Cyc_Absyn_Exp*
_temp3438; struct Cyc_Absyn_Exp* _temp3440; struct Cyc_Absyn_Exp* _temp3442;
struct Cyc_List_List* _temp3444; struct Cyc_List_List* _temp3446; struct Cyc_List_List*
_temp3448; struct Cyc_List_List* _temp3450; void* _temp3452; struct Cyc_List_List*
_temp3454; struct Cyc_List_List* _temp3456; _LL3380: if(*(( int*) _temp3378) ==
Cyc_Absyn_Const_e){ goto _LL3381;} else{ goto _LL3382;} _LL3382: if(*(( int*)
_temp3378) == Cyc_Absyn_Sizeoftyp_e){ goto _LL3383;} else{ goto _LL3384;}
_LL3384: if(*(( int*) _temp3378) == Cyc_Absyn_Sizeofexp_e){ goto _LL3385;} else{
goto _LL3386;} _LL3386: if(*(( int*) _temp3378) == Cyc_Absyn_Enum_e){ goto
_LL3387;} else{ goto _LL3388;} _LL3388: if(*(( int*) _temp3378) == Cyc_Absyn_Var_e){
_LL3419: _temp3418=(( struct Cyc_Absyn_Var_e_struct*) _temp3378)->f1; goto
_LL3389;} else{ goto _LL3390;} _LL3390: if(*(( int*) _temp3378) == Cyc_Absyn_Conditional_e){
_LL3425: _temp3424=(( struct Cyc_Absyn_Conditional_e_struct*) _temp3378)->f1;
goto _LL3423; _LL3423: _temp3422=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3378)->f2; goto _LL3421; _LL3421: _temp3420=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3378)->f3; goto _LL3391;} else{ goto _LL3392;} _LL3392: if(*(( int*)
_temp3378) == Cyc_Absyn_SeqExp_e){ _LL3429: _temp3428=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3378)->f1; goto _LL3427; _LL3427: _temp3426=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3378)->f2; goto _LL3393;} else{ goto _LL3394;} _LL3394: if(*(( int*)
_temp3378) == Cyc_Absyn_NoInstantiate_e){ _LL3431: _temp3430=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp3378)->f1; goto _LL3395;} else{ goto _LL3396;} _LL3396: if(*(( int*)
_temp3378) == Cyc_Absyn_Instantiate_e){ _LL3433: _temp3432=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3378)->f1; goto _LL3397;} else{ goto _LL3398;} _LL3398: if(*(( int*)
_temp3378) == Cyc_Absyn_Cast_e){ _LL3437: _temp3436=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp3378)->f1; goto _LL3435; _LL3435: _temp3434=(( struct Cyc_Absyn_Cast_e_struct*)
_temp3378)->f2; goto _LL3399;} else{ goto _LL3400;} _LL3400: if(*(( int*)
_temp3378) == Cyc_Absyn_Address_e){ _LL3439: _temp3438=(( struct Cyc_Absyn_Address_e_struct*)
_temp3378)->f1; goto _LL3401;} else{ goto _LL3402;} _LL3402: if(*(( int*)
_temp3378) == Cyc_Absyn_Comprehension_e){ _LL3443: _temp3442=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3378)->f2; goto _LL3441; _LL3441: _temp3440=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3378)->f3; goto _LL3403;} else{ goto _LL3404;} _LL3404: if(*(( int*)
_temp3378) == Cyc_Absyn_Array_e){ _LL3445: _temp3444=(( struct Cyc_Absyn_Array_e_struct*)
_temp3378)->f1; goto _LL3405;} else{ goto _LL3406;} _LL3406: if(*(( int*)
_temp3378) == Cyc_Absyn_AnonStruct_e){ _LL3447: _temp3446=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3378)->f2; goto _LL3407;} else{ goto _LL3408;} _LL3408: if(*(( int*)
_temp3378) == Cyc_Absyn_Struct_e){ _LL3449: _temp3448=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3378)->f3; goto _LL3409;} else{ goto _LL3410;} _LL3410: if(*(( int*)
_temp3378) == Cyc_Absyn_Primop_e){ _LL3453: _temp3452=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp3378)->f1; goto _LL3451; _LL3451: _temp3450=(( struct Cyc_Absyn_Primop_e_struct*)
_temp3378)->f2; goto _LL3411;} else{ goto _LL3412;} _LL3412: if(*(( int*)
_temp3378) == Cyc_Absyn_Tuple_e){ _LL3455: _temp3454=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp3378)->f1; goto _LL3413;} else{ goto _LL3414;} _LL3414: if(*(( int*)
_temp3378) == Cyc_Absyn_Tunion_e){ _LL3457: _temp3456=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3378)->f3; goto _LL3415;} else{ goto _LL3416;} _LL3416: goto _LL3417;
_LL3381: return 1; _LL3383: return 1; _LL3385: return 1; _LL3387: return 1;
_LL3389: { struct _handler_cons _temp3458; _push_handler(& _temp3458);{ int
_temp3460= 0; if( setjmp( _temp3458.handler)){ _temp3460= 1;} if( ! _temp3460){{
void* _temp3461= Cyc_Tcenv_lookup_ordinary( te, e->loc, _temp3418); void*
_temp3469; void* _temp3471; struct Cyc_Absyn_Vardecl* _temp3473; _LL3463: if(*((
int*) _temp3461) == Cyc_Tcenv_VarRes){ _LL3470: _temp3469=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3461)->f1; if(( unsigned int) _temp3469 > 1u?*(( int*) _temp3469) == Cyc_Absyn_Funname_b:
0){ goto _LL3464;} else{ goto _LL3465;}} else{ goto _LL3465;} _LL3465: if(*((
int*) _temp3461) == Cyc_Tcenv_VarRes){ _LL3472: _temp3471=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3461)->f1; if(( unsigned int) _temp3471 > 1u?*(( int*) _temp3471) == Cyc_Absyn_Global_b:
0){ _LL3474: _temp3473=(( struct Cyc_Absyn_Global_b_struct*) _temp3471)->f1;
goto _LL3466;} else{ goto _LL3467;}} else{ goto _LL3467;} _LL3467: goto _LL3468;
_LL3464: { int _temp3475= 1; _npop_handler( 0u); return _temp3475;} _LL3466: {
void* _temp3476= Cyc_Tcutil_compress(( void*) _temp3473->type); _LL3478: if((
unsigned int) _temp3476 > 4u?*(( int*) _temp3476) == Cyc_Absyn_ArrayType: 0){
goto _LL3479;} else{ goto _LL3480;} _LL3480: goto _LL3481; _LL3479: { int
_temp3482= 1; _npop_handler( 0u); return _temp3482;} _LL3481: { int _temp3483=
var_okay; _npop_handler( 0u); return _temp3483;} _LL3477:;} _LL3468: { int
_temp3484= var_okay; _npop_handler( 0u); return _temp3484;} _LL3462:;};
_pop_handler();} else{ void* _temp3459=( void*) _exn_thrown; void* _temp3486=
_temp3459; _LL3488: if( _temp3486 == Cyc_Dict_Absent){ goto _LL3489;} else{ goto
_LL3490;} _LL3490: goto _LL3491; _LL3489: return 0; _LL3491:( void) _throw(
_temp3486); _LL3487:;}}} _LL3391: return( Cyc_Tcutil_cnst_exp( te, 0, _temp3424)?
Cyc_Tcutil_cnst_exp( te, 0, _temp3422): 0)? Cyc_Tcutil_cnst_exp( te, 0,
_temp3420): 0; _LL3393: return Cyc_Tcutil_cnst_exp( te, 0, _temp3428)? Cyc_Tcutil_cnst_exp(
te, 0, _temp3426): 0; _LL3395: _temp3432= _temp3430; goto _LL3397; _LL3397:
return Cyc_Tcutil_cnst_exp( te, var_okay, _temp3432); _LL3399: return Cyc_Tcutil_cnst_exp(
te, var_okay, _temp3434); _LL3401: return Cyc_Tcutil_cnst_exp( te, 1, _temp3438);
_LL3403: return Cyc_Tcutil_cnst_exp( te, 0, _temp3442)? Cyc_Tcutil_cnst_exp( te,
0, _temp3440): 0; _LL3405: _temp3446= _temp3444; goto _LL3407; _LL3407:
_temp3448= _temp3446; goto _LL3409; _LL3409: for( 0; _temp3448 != 0; _temp3448=((
struct Cyc_List_List*) _check_null( _temp3448))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,(*(( struct _tuple20*)(( struct Cyc_List_List*) _check_null( _temp3448))->hd)).f2)){
return 0;}} return 1; _LL3411: _temp3454= _temp3450; goto _LL3413; _LL3413:
_temp3456= _temp3454; goto _LL3415; _LL3415: for( 0; _temp3456 != 0; _temp3456=((
struct Cyc_List_List*) _check_null( _temp3456))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp3456))->hd)){
return 0;}} return 1; _LL3417: return 0; _LL3379:;} int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcutil_cnst_exp(
te, 0, e);} static int Cyc_Tcutil_fields_support_default( struct Cyc_List_List*
tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs); int Cyc_Tcutil_supports_default(
void* t){ void* _temp3492= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp3530; struct Cyc_Absyn_Conref* _temp3532; struct Cyc_Absyn_Conref*
_temp3534; void* _temp3536; void* _temp3538; struct Cyc_List_List* _temp3540;
struct Cyc_Absyn_Structdecl** _temp3542; struct Cyc_List_List* _temp3544; struct
Cyc_Absyn_Uniondecl** _temp3546; struct Cyc_List_List* _temp3548; struct Cyc_List_List*
_temp3550; struct Cyc_List_List* _temp3552; _LL3494: if( _temp3492 ==( void*)
Cyc_Absyn_VoidType){ goto _LL3495;} else{ goto _LL3496;} _LL3496: if((
unsigned int) _temp3492 > 4u?*(( int*) _temp3492) == Cyc_Absyn_Evar: 0){ goto
_LL3497;} else{ goto _LL3498;} _LL3498: if(( unsigned int) _temp3492 > 4u?*((
int*) _temp3492) == Cyc_Absyn_VarType: 0){ goto _LL3499;} else{ goto _LL3500;}
_LL3500: if(( unsigned int) _temp3492 > 4u?*(( int*) _temp3492) == Cyc_Absyn_TunionType:
0){ goto _LL3501;} else{ goto _LL3502;} _LL3502: if(( unsigned int) _temp3492 >
4u?*(( int*) _temp3492) == Cyc_Absyn_TunionFieldType: 0){ goto _LL3503;} else{
goto _LL3504;} _LL3504: if(( unsigned int) _temp3492 > 4u?*(( int*) _temp3492)
== Cyc_Absyn_PointerType: 0){ _LL3531: _temp3530=(( struct Cyc_Absyn_PointerType_struct*)
_temp3492)->f1; _LL3537: _temp3536=( void*) _temp3530.elt_typ; goto _LL3535;
_LL3535: _temp3534= _temp3530.nullable; goto _LL3533; _LL3533: _temp3532=
_temp3530.bounds; goto _LL3505;} else{ goto _LL3506;} _LL3506: if(( unsigned int)
_temp3492 > 4u?*(( int*) _temp3492) == Cyc_Absyn_IntType: 0){ goto _LL3507;}
else{ goto _LL3508;} _LL3508: if( _temp3492 ==( void*) Cyc_Absyn_FloatType){
goto _LL3509;} else{ goto _LL3510;} _LL3510: if( _temp3492 ==( void*) Cyc_Absyn_DoubleType){
goto _LL3511;} else{ goto _LL3512;} _LL3512: if(( unsigned int) _temp3492 > 4u?*((
int*) _temp3492) == Cyc_Absyn_ArrayType: 0){ _LL3539: _temp3538=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3492)->f1; goto _LL3513;} else{ goto
_LL3514;} _LL3514: if(( unsigned int) _temp3492 > 4u?*(( int*) _temp3492) == Cyc_Absyn_FnType:
0){ goto _LL3515;} else{ goto _LL3516;} _LL3516: if(( unsigned int) _temp3492 >
4u?*(( int*) _temp3492) == Cyc_Absyn_TupleType: 0){ _LL3541: _temp3540=(( struct
Cyc_Absyn_TupleType_struct*) _temp3492)->f1; goto _LL3517;} else{ goto _LL3518;}
_LL3518: if(( unsigned int) _temp3492 > 4u?*(( int*) _temp3492) == Cyc_Absyn_StructType:
0){ _LL3545: _temp3544=(( struct Cyc_Absyn_StructType_struct*) _temp3492)->f2;
goto _LL3543; _LL3543: _temp3542=(( struct Cyc_Absyn_StructType_struct*)
_temp3492)->f3; goto _LL3519;} else{ goto _LL3520;} _LL3520: if(( unsigned int)
_temp3492 > 4u?*(( int*) _temp3492) == Cyc_Absyn_UnionType: 0){ _LL3549:
_temp3548=(( struct Cyc_Absyn_UnionType_struct*) _temp3492)->f2; goto _LL3547;
_LL3547: _temp3546=(( struct Cyc_Absyn_UnionType_struct*) _temp3492)->f3; goto
_LL3521;} else{ goto _LL3522;} _LL3522: if(( unsigned int) _temp3492 > 4u?*((
int*) _temp3492) == Cyc_Absyn_AnonStructType: 0){ _LL3551: _temp3550=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp3492)->f1; goto _LL3523;} else{ goto
_LL3524;} _LL3524: if(( unsigned int) _temp3492 > 4u?*(( int*) _temp3492) == Cyc_Absyn_AnonUnionType:
0){ _LL3553: _temp3552=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3492)->f1;
goto _LL3525;} else{ goto _LL3526;} _LL3526: if(( unsigned int) _temp3492 > 4u?*((
int*) _temp3492) == Cyc_Absyn_EnumType: 0){ goto _LL3527;} else{ goto _LL3528;}
_LL3528: goto _LL3529; _LL3495: return 1; _LL3497: return 0; _LL3499: return 0;
_LL3501: return 0; _LL3503: return 0; _LL3505: { void* _temp3554=( void*)( Cyc_Absyn_compress_conref(
_temp3532))->v; void* _temp3562; void* _temp3564; _LL3556: if(( unsigned int)
_temp3554 > 1u?*(( int*) _temp3554) == Cyc_Absyn_Eq_constr: 0){ _LL3563:
_temp3562=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3554)->f1; if(
_temp3562 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3557;} else{ goto _LL3558;}}
else{ goto _LL3558;} _LL3558: if(( unsigned int) _temp3554 > 1u?*(( int*)
_temp3554) == Cyc_Absyn_Eq_constr: 0){ _LL3565: _temp3564=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3554)->f1; if(( unsigned int) _temp3564 > 1u?*(( int*) _temp3564) == Cyc_Absyn_Upper_b:
0){ goto _LL3559;} else{ goto _LL3560;}} else{ goto _LL3560;} _LL3560: goto
_LL3561; _LL3557: return 1; _LL3559: { void* _temp3566=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3534))->v; int
_temp3572; _LL3568: if(( unsigned int) _temp3566 > 1u?*(( int*) _temp3566) ==
Cyc_Absyn_Eq_constr: 0){ _LL3573: _temp3572=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3566)->f1; goto _LL3569;} else{ goto _LL3570;} _LL3570: goto _LL3571;
_LL3569: return _temp3572; _LL3571: return 0; _LL3567:;} _LL3561: return 0;
_LL3555:;} _LL3507: return 1; _LL3509: return 1; _LL3511: return 1; _LL3513:
return Cyc_Tcutil_supports_default( _temp3538); _LL3515: return 0; _LL3517: for(
0; _temp3540 != 0; _temp3540=(( struct Cyc_List_List*) _check_null( _temp3540))->tl){
if( ! Cyc_Tcutil_supports_default((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp3540))->hd)).f2)){ return 0;}} return 1; _LL3519: if(
_temp3542 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp3542)); if( sd->fields == 0){ return 0;} return Cyc_Tcutil_fields_support_default(
sd->tvs, _temp3544,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
sd->fields))->v);} _LL3521: if( _temp3546 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp3546)); if( ud->fields ==
0){ return 0;} return Cyc_Tcutil_fields_support_default( ud->tvs, _temp3548,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ud->fields))->v);}
_LL3523: _temp3552= _temp3550; goto _LL3525; _LL3525: return Cyc_Tcutil_fields_support_default(
0, 0, _temp3552); _LL3527: return 1; _LL3529: return 0; _LL3493:;} static int
Cyc_Tcutil_fields_support_default( struct Cyc_List_List* tvs, struct Cyc_List_List*
ts, struct Cyc_List_List* fs){{ struct _RegionHandle _temp3574= _new_region();
struct _RegionHandle* rgn=& _temp3574; _push_region( rgn);{ struct Cyc_List_List*
_temp3575=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, tvs, ts); for( 0; fs != 0; fs=(( struct Cyc_List_List*) _check_null(
fs))->tl){ void* t= Cyc_Tcutil_rsubstitute( rgn, _temp3575,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( fs))->hd)->type); if( ! Cyc_Tcutil_supports_default(
t)){ int _temp3576= 0; _npop_handler( 0u); return _temp3576;}}}; _pop_region(
rgn);} return 1;}
