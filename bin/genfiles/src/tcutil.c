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
8u]; extern void* Cyc_List_nth( struct Cyc_List_List* x, int i); extern int Cyc_List_exists_c(
int(* pred)( void*, void*), void* env, struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_List_zip( struct Cyc_List_List* x, struct Cyc_List_List* y);
extern struct Cyc_List_List* Cyc_List_rzip( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y); struct
_tuple0{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ; extern struct
_tuple0 Cyc_List_rsplit( struct _RegionHandle* r3, struct _RegionHandle* r4,
struct Cyc_List_List* x); extern int Cyc_List_mem( int(* compare)( void*, void*),
struct Cyc_List_List* l, void* x); extern void* Cyc_List_assoc_cmp( int(*
compare)( void*, void*), struct Cyc_List_List* l, void* x); struct Cyc_Lineno_Pos{
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
_temp599.f2= a2; _temp599;}); void* _temp612; int _temp614; int _temp616; void*
_temp618; void* _temp620; int _temp622; int _temp624; void* _temp626; void*
_temp628; int _temp630; void* _temp632; int _temp634; void* _temp636; int
_temp638; void* _temp640; int _temp642; void* _temp644; struct _tagged_arr
_temp646; void* _temp648; struct _tagged_arr _temp650; _LL602: _LL621: _temp620=
_temp600.f1; if(( unsigned int) _temp620 > 16u?*(( int*) _temp620) == Cyc_Absyn_Format_att:
0){ _LL627: _temp626=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp620)->f1;
goto _LL625; _LL625: _temp624=(( struct Cyc_Absyn_Format_att_struct*) _temp620)->f2;
goto _LL623; _LL623: _temp622=(( struct Cyc_Absyn_Format_att_struct*) _temp620)->f3;
goto _LL613;} else{ goto _LL604;} _LL613: _temp612= _temp600.f2; if((
unsigned int) _temp612 > 16u?*(( int*) _temp612) == Cyc_Absyn_Format_att: 0){
_LL619: _temp618=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp612)->f1;
goto _LL617; _LL617: _temp616=(( struct Cyc_Absyn_Format_att_struct*) _temp612)->f2;
goto _LL615; _LL615: _temp614=(( struct Cyc_Absyn_Format_att_struct*) _temp612)->f3;
goto _LL603;} else{ goto _LL604;} _LL604: _LL633: _temp632= _temp600.f1; if((
unsigned int) _temp632 > 16u?*(( int*) _temp632) == Cyc_Absyn_Regparm_att: 0){
_LL635: _temp634=(( struct Cyc_Absyn_Regparm_att_struct*) _temp632)->f1; goto
_LL629;} else{ goto _LL606;} _LL629: _temp628= _temp600.f2; if(( unsigned int)
_temp628 > 16u?*(( int*) _temp628) == Cyc_Absyn_Regparm_att: 0){ _LL631:
_temp630=(( struct Cyc_Absyn_Regparm_att_struct*) _temp628)->f1; goto _LL605;}
else{ goto _LL606;} _LL606: _LL641: _temp640= _temp600.f1; if(( unsigned int)
_temp640 > 16u?*(( int*) _temp640) == Cyc_Absyn_Aligned_att: 0){ _LL643:
_temp642=(( struct Cyc_Absyn_Aligned_att_struct*) _temp640)->f1; goto _LL637;}
else{ goto _LL608;} _LL637: _temp636= _temp600.f2; if(( unsigned int) _temp636 >
16u?*(( int*) _temp636) == Cyc_Absyn_Aligned_att: 0){ _LL639: _temp638=(( struct
Cyc_Absyn_Aligned_att_struct*) _temp636)->f1; goto _LL607;} else{ goto _LL608;}
_LL608: _LL649: _temp648= _temp600.f1; if(( unsigned int) _temp648 > 16u?*(( int*)
_temp648) == Cyc_Absyn_Section_att: 0){ _LL651: _temp650=(( struct Cyc_Absyn_Section_att_struct*)
_temp648)->f1; goto _LL645;} else{ goto _LL610;} _LL645: _temp644= _temp600.f2;
if(( unsigned int) _temp644 > 16u?*(( int*) _temp644) == Cyc_Absyn_Section_att:
0){ _LL647: _temp646=(( struct Cyc_Absyn_Section_att_struct*) _temp644)->f1;
goto _LL609;} else{ goto _LL610;} _LL610: goto _LL611; _LL603: return( _temp626
== _temp618? _temp624 == _temp616: 0)? _temp622 == _temp614: 0; _LL605: _temp642=
_temp634; _temp638= _temp630; goto _LL607; _LL607: return _temp642 == _temp638;
_LL609: return Cyc_String_strcmp( _temp650, _temp646) == 0; _LL611: return 0;
_LL601:;}} int Cyc_Tcutil_same_atts( struct Cyc_List_List* a1, struct Cyc_List_List*
a2){{ struct Cyc_List_List* a= a1; for( 0; a != 0; a=(( struct Cyc_List_List*)
_check_null( a))->tl){ if( ! Cyc_List_exists_c( Cyc_Tcutil_equal_att,( void*)((
struct Cyc_List_List*) _check_null( a))->hd, a2)){ return 0;}}}{ struct Cyc_List_List*
a= a2; for( 0; a != 0; a=(( struct Cyc_List_List*) _check_null( a))->tl){ if( !
Cyc_List_exists_c( Cyc_Tcutil_equal_att,( void*)(( struct Cyc_List_List*)
_check_null( a))->hd, a1)){ return 0;}}} return 1;} static void Cyc_Tcutil_normalize_effect(
void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp652= e; struct Cyc_List_List*
_temp658; struct Cyc_List_List** _temp660; _LL654: if(( unsigned int) _temp652 >
4u?*(( int*) _temp652) == Cyc_Absyn_JoinEff: 0){ _LL659: _temp658=(( struct Cyc_Absyn_JoinEff_struct*)
_temp652)->f1; _temp660=&(( struct Cyc_Absyn_JoinEff_struct*) _temp652)->f1;
goto _LL655;} else{ goto _LL656;} _LL656: goto _LL657; _LL655: { int nested_join=
0;{ struct Cyc_List_List* effs=* _temp660; for( 0; effs != 0; effs=(( struct Cyc_List_List*)
_check_null( effs))->tl){ void* _temp661=( void*)(( struct Cyc_List_List*)
_check_null( effs))->hd; Cyc_Tcutil_normalize_effect( _temp661);{ void* _temp662=
Cyc_Tcutil_compress( _temp661); _LL664: if(( unsigned int) _temp662 > 4u?*(( int*)
_temp662) == Cyc_Absyn_JoinEff: 0){ goto _LL665;} else{ goto _LL666;} _LL666:
goto _LL667; _LL665: nested_join= 1; goto _LL663; _LL667: goto _LL663; _LL663:;}}}
if( ! nested_join){ return;}{ struct Cyc_List_List* effects= 0;{ struct Cyc_List_List*
effs=* _temp660; for( 0; effs != 0; effs=(( struct Cyc_List_List*) _check_null(
effs))->tl){ void* _temp668= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*)
_check_null( effs))->hd); struct Cyc_List_List* _temp674; _LL670: if((
unsigned int) _temp668 > 4u?*(( int*) _temp668) == Cyc_Absyn_JoinEff: 0){ _LL675:
_temp674=(( struct Cyc_Absyn_JoinEff_struct*) _temp668)->f1; goto _LL671;} else{
goto _LL672;} _LL672: goto _LL673; _LL671: effects= Cyc_List_revappend( _temp674,
effects); goto _LL669; _LL673: effects=({ struct Cyc_List_List* _temp676=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp676->hd=(
void*) _temp668; _temp676->tl= effects; _temp676;}); goto _LL669; _LL669:;}}*
_temp660= Cyc_List_imp_rev( effects); goto _LL653;}} _LL657: goto _LL653; _LL653:;}}
struct _tuple9{ void* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; }
; static struct _tuple9* Cyc_Tcutil_get_effect_evar( void* t){ void* _temp677=
Cyc_Tcutil_compress( t); struct Cyc_List_List* _temp685; struct Cyc_List_List
_temp687; struct Cyc_List_List* _temp688; void* _temp690; struct Cyc_Core_Opt*
_temp692; struct Cyc_Core_Opt* _temp694; _LL679: if(( unsigned int) _temp677 > 4u?*((
int*) _temp677) == Cyc_Absyn_JoinEff: 0){ _LL686: _temp685=(( struct Cyc_Absyn_JoinEff_struct*)
_temp677)->f1; if( _temp685 == 0){ goto _LL681;} else{ _temp687=* _temp685;
_LL691: _temp690=( void*) _temp687.hd; goto _LL689; _LL689: _temp688= _temp687.tl;
goto _LL680;}} else{ goto _LL681;} _LL681: if(( unsigned int) _temp677 > 4u?*((
int*) _temp677) == Cyc_Absyn_Evar: 0){ _LL695: _temp694=(( struct Cyc_Absyn_Evar_struct*)
_temp677)->f1; goto _LL693; _LL693: _temp692=(( struct Cyc_Absyn_Evar_struct*)
_temp677)->f4; goto _LL682;} else{ goto _LL683;} _LL683: goto _LL684; _LL680: {
void* _temp696= Cyc_Tcutil_compress( _temp690); struct Cyc_Core_Opt* _temp702;
_LL698: if(( unsigned int) _temp696 > 4u?*(( int*) _temp696) == Cyc_Absyn_Evar:
0){ _LL703: _temp702=(( struct Cyc_Absyn_Evar_struct*) _temp696)->f4; goto
_LL699;} else{ goto _LL700;} _LL700: goto _LL701; _LL699: return({ struct
_tuple9* _temp704=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp704->f1= _temp690; _temp704->f2= _temp688; _temp704->f3=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp702))->v; _temp704;}); _LL701: return 0;
_LL697:;} _LL682: if( _temp694 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp694))->v !=( void*) Cyc_Absyn_EffKind){(( int(*)( struct
_tagged_arr msg)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u));} return({ struct _tuple9* _temp705=( struct
_tuple9*) GC_malloc( sizeof( struct _tuple9)); _temp705->f1= t; _temp705->f2= 0;
_temp705->f3=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp692))->v; _temp705;}); _LL684: return 0; _LL678:;} static struct Cyc_Core_Opt
Cyc_Tcutil_ek={( void*)(( void*) 4u)}; int Cyc_Tcutil_region_in_effect( int
constrain, void* r, void* e){ r= Cyc_Tcutil_compress( r); if( r ==( void*) Cyc_Absyn_HeapRgn){
return 1;}{ void* _temp706= Cyc_Tcutil_compress( e); void* _temp716; struct Cyc_List_List*
_temp718; struct Cyc_Core_Opt* _temp720; struct Cyc_Core_Opt* _temp722; struct
Cyc_Core_Opt** _temp724; struct Cyc_Core_Opt* _temp725; _LL708: if((
unsigned int) _temp706 > 4u?*(( int*) _temp706) == Cyc_Absyn_AccessEff: 0){
_LL717: _temp716=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp706)->f1;
goto _LL709;} else{ goto _LL710;} _LL710: if(( unsigned int) _temp706 > 4u?*((
int*) _temp706) == Cyc_Absyn_JoinEff: 0){ _LL719: _temp718=(( struct Cyc_Absyn_JoinEff_struct*)
_temp706)->f1; goto _LL711;} else{ goto _LL712;} _LL712: if(( unsigned int)
_temp706 > 4u?*(( int*) _temp706) == Cyc_Absyn_Evar: 0){ _LL726: _temp725=((
struct Cyc_Absyn_Evar_struct*) _temp706)->f1; goto _LL723; _LL723: _temp722=((
struct Cyc_Absyn_Evar_struct*) _temp706)->f2; _temp724=&(( struct Cyc_Absyn_Evar_struct*)
_temp706)->f2; goto _LL721; _LL721: _temp720=(( struct Cyc_Absyn_Evar_struct*)
_temp706)->f4; goto _LL713;} else{ goto _LL714;} _LL714: goto _LL715; _LL709:
if( constrain){ return Cyc_Tcutil_unify( r, _temp716);} Cyc_Tcutil_compress(
_temp716); if( r == _temp716){ return 1;}{ struct _tuple8 _temp728=({ struct
_tuple8 _temp727; _temp727.f1= r; _temp727.f2= _temp716; _temp727;}); void*
_temp734; struct Cyc_Absyn_Tvar* _temp736; void* _temp738; struct Cyc_Absyn_Tvar*
_temp740; _LL730: _LL739: _temp738= _temp728.f1; if(( unsigned int) _temp738 > 4u?*((
int*) _temp738) == Cyc_Absyn_VarType: 0){ _LL741: _temp740=(( struct Cyc_Absyn_VarType_struct*)
_temp738)->f1; goto _LL735;} else{ goto _LL732;} _LL735: _temp734= _temp728.f2;
if(( unsigned int) _temp734 > 4u?*(( int*) _temp734) == Cyc_Absyn_VarType: 0){
_LL737: _temp736=(( struct Cyc_Absyn_VarType_struct*) _temp734)->f1; goto _LL731;}
else{ goto _LL732;} _LL732: goto _LL733; _LL731: return Cyc_Absyn_tvar_cmp(
_temp740, _temp736) == 0; _LL733: return 0; _LL729:;} _LL711: for( 0; _temp718
!= 0; _temp718=(( struct Cyc_List_List*) _check_null( _temp718))->tl){ if( Cyc_Tcutil_region_in_effect(
constrain, r,( void*)(( struct Cyc_List_List*) _check_null( _temp718))->hd)){
return 1;}} return 0; _LL713: if( _temp725 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp725))->v !=( void*) Cyc_Absyn_EffKind){(( int(*)( struct
_tagged_arr msg)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u));} if( ! constrain){ return 0;}{ void* _temp742=
Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek, _temp720); Cyc_Tcutil_occurs(
_temp742,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp720))->v,
r);{ struct Cyc_Absyn_JoinEff_struct* _temp743=({ struct Cyc_Absyn_JoinEff_struct*
_temp745=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp745[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp746; _temp746.tag= Cyc_Absyn_JoinEff;
_temp746.f1=({ struct Cyc_List_List* _temp747=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp747->hd=( void*) _temp742; _temp747->tl=({
struct Cyc_List_List* _temp748=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp748->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp749=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp749[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp750; _temp750.tag= Cyc_Absyn_AccessEff;
_temp750.f1=( void*) r; _temp750;}); _temp749;})); _temp748->tl= 0; _temp748;});
_temp747;}); _temp746;}); _temp745;});* _temp724=({ struct Cyc_Core_Opt*
_temp744=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp744->v=( void*)(( void*) _temp743); _temp744;}); return 1;}} _LL715: return
0; _LL707:;}} static int Cyc_Tcutil_variable_in_effect( int constrain, struct
Cyc_Absyn_Tvar* v, void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp751= e;
struct Cyc_Absyn_Tvar* _temp761; struct Cyc_List_List* _temp763; struct Cyc_Core_Opt*
_temp765; struct Cyc_Core_Opt* _temp767; struct Cyc_Core_Opt** _temp769; struct
Cyc_Core_Opt* _temp770; _LL753: if(( unsigned int) _temp751 > 4u?*(( int*)
_temp751) == Cyc_Absyn_VarType: 0){ _LL762: _temp761=(( struct Cyc_Absyn_VarType_struct*)
_temp751)->f1; goto _LL754;} else{ goto _LL755;} _LL755: if(( unsigned int)
_temp751 > 4u?*(( int*) _temp751) == Cyc_Absyn_JoinEff: 0){ _LL764: _temp763=((
struct Cyc_Absyn_JoinEff_struct*) _temp751)->f1; goto _LL756;} else{ goto _LL757;}
_LL757: if(( unsigned int) _temp751 > 4u?*(( int*) _temp751) == Cyc_Absyn_Evar:
0){ _LL771: _temp770=(( struct Cyc_Absyn_Evar_struct*) _temp751)->f1; goto
_LL768; _LL768: _temp767=(( struct Cyc_Absyn_Evar_struct*) _temp751)->f2;
_temp769=&(( struct Cyc_Absyn_Evar_struct*) _temp751)->f2; goto _LL766; _LL766:
_temp765=(( struct Cyc_Absyn_Evar_struct*) _temp751)->f4; goto _LL758;} else{
goto _LL759;} _LL759: goto _LL760; _LL754: return Cyc_Absyn_tvar_cmp( v,
_temp761) == 0; _LL756: for( 0; _temp763 != 0; _temp763=(( struct Cyc_List_List*)
_check_null( _temp763))->tl){ if( Cyc_Tcutil_variable_in_effect( constrain, v,(
void*)(( struct Cyc_List_List*) _check_null( _temp763))->hd)){ return 1;}}
return 0; _LL758: if( _temp770 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp770))->v !=( void*) Cyc_Absyn_EffKind){(( int(*)( struct
_tagged_arr msg)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u));}{ void* _temp772= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek, _temp765); if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp765))->v, v)){ return 0;}{ struct Cyc_Absyn_JoinEff_struct*
_temp773=({ struct Cyc_Absyn_JoinEff_struct* _temp775=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp775[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp776; _temp776.tag= Cyc_Absyn_JoinEff; _temp776.f1=({ struct Cyc_List_List*
_temp777=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp777->hd=( void*) _temp772; _temp777->tl=({ struct Cyc_List_List* _temp778=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp778->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp779=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp779[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp780; _temp780.tag= Cyc_Absyn_VarType; _temp780.f1= v; _temp780;}); _temp779;}));
_temp778->tl= 0; _temp778;}); _temp777;}); _temp776;}); _temp775;});* _temp769=({
struct Cyc_Core_Opt* _temp774=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp774->v=( void*)(( void*) _temp773); _temp774;}); return 1;}}
_LL760: return 0; _LL752:;}} static int Cyc_Tcutil_evar_in_effect( void* evar,
void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp781= e; struct Cyc_List_List*
_temp789; _LL783: if(( unsigned int) _temp781 > 4u?*(( int*) _temp781) == Cyc_Absyn_JoinEff:
0){ _LL790: _temp789=(( struct Cyc_Absyn_JoinEff_struct*) _temp781)->f1; goto
_LL784;} else{ goto _LL785;} _LL785: if(( unsigned int) _temp781 > 4u?*(( int*)
_temp781) == Cyc_Absyn_Evar: 0){ goto _LL786;} else{ goto _LL787;} _LL787: goto
_LL788; _LL784: for( 0; _temp789 != 0; _temp789=(( struct Cyc_List_List*)
_check_null( _temp789))->tl){ if( Cyc_Tcutil_evar_in_effect( evar,( void*)((
struct Cyc_List_List*) _check_null( _temp789))->hd)){ return 1;}} return 0;
_LL786: return evar == e; _LL788: return 0; _LL782:;}} int Cyc_Tcutil_subset_effect(
int set_to_empty, void* e1, void* e2){{ struct _tuple9* _temp791= Cyc_Tcutil_get_effect_evar(
e2); struct _tuple9 _temp797; struct Cyc_List_List* _temp798; struct Cyc_List_List*
_temp800; void* _temp802; _LL793: if( _temp791 == 0){ goto _LL795;} else{
_temp797=* _temp791; _LL803: _temp802= _temp797.f1; goto _LL801; _LL801:
_temp800= _temp797.f2; goto _LL799; _LL799: _temp798= _temp797.f3; goto _LL794;}
_LL795: goto _LL796; _LL794: { void* _temp804= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,({ struct Cyc_Core_Opt* _temp810=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp810->v=( void*) _temp798; _temp810;}));
struct Cyc_Absyn_JoinEff_struct* _temp805=({ struct Cyc_Absyn_JoinEff_struct*
_temp806=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp806[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp807; _temp807.tag= Cyc_Absyn_JoinEff;
_temp807.f1=({ struct Cyc_List_List* _temp808=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp808->hd=( void*) _temp804; _temp808->tl=({
struct Cyc_List_List* _temp809=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp809->hd=( void*) e1; _temp809->tl= 0; _temp809;});
_temp808;}); _temp807;}); _temp806;}); return Cyc_Tcutil_unify( _temp802,( void*)
_temp805);} _LL796: goto _LL792; _LL792:;}{ void* _temp811= Cyc_Tcutil_compress(
e1); struct Cyc_List_List* _temp823; void* _temp825; struct Cyc_Absyn_Tvar*
_temp827; struct Cyc_Core_Opt* _temp829; struct Cyc_Core_Opt* _temp831; struct
Cyc_Core_Opt** _temp833; _LL813: if(( unsigned int) _temp811 > 4u?*(( int*)
_temp811) == Cyc_Absyn_JoinEff: 0){ _LL824: _temp823=(( struct Cyc_Absyn_JoinEff_struct*)
_temp811)->f1; goto _LL814;} else{ goto _LL815;} _LL815: if(( unsigned int)
_temp811 > 4u?*(( int*) _temp811) == Cyc_Absyn_AccessEff: 0){ _LL826: _temp825=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp811)->f1; goto _LL816;} else{
goto _LL817;} _LL817: if(( unsigned int) _temp811 > 4u?*(( int*) _temp811) ==
Cyc_Absyn_VarType: 0){ _LL828: _temp827=(( struct Cyc_Absyn_VarType_struct*)
_temp811)->f1; goto _LL818;} else{ goto _LL819;} _LL819: if(( unsigned int)
_temp811 > 4u?*(( int*) _temp811) == Cyc_Absyn_Evar: 0){ _LL832: _temp831=((
struct Cyc_Absyn_Evar_struct*) _temp811)->f2; _temp833=&(( struct Cyc_Absyn_Evar_struct*)
_temp811)->f2; goto _LL830; _LL830: _temp829=(( struct Cyc_Absyn_Evar_struct*)
_temp811)->f4; goto _LL820;} else{ goto _LL821;} _LL821: goto _LL822; _LL814:
for( 0; _temp823 != 0; _temp823=(( struct Cyc_List_List*) _check_null( _temp823))->tl){
if( ! Cyc_Tcutil_subset_effect( set_to_empty,( void*)(( struct Cyc_List_List*)
_check_null( _temp823))->hd, e2)){ return 0;}} return 1; _LL816: return( Cyc_Tcutil_region_in_effect(
0, _temp825, e2)? 1: Cyc_Tcutil_region_in_effect( 1, _temp825, e2))? 1: Cyc_Tcutil_unify(
_temp825,( void*) Cyc_Absyn_HeapRgn); _LL818: return Cyc_Tcutil_variable_in_effect(
0, _temp827, e2)? 1: Cyc_Tcutil_variable_in_effect( 1, _temp827, e2); _LL820:
if( ! Cyc_Tcutil_evar_in_effect( e1, e2)){ if( set_to_empty){* _temp833=({
struct Cyc_Core_Opt* _temp834=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp834->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp835=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp835[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp836; _temp836.tag= Cyc_Absyn_JoinEff;
_temp836.f1= 0; _temp836;}); _temp835;})); _temp834;});} else{ Cyc_Tcutil_occurs(
e1,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp829))->v,
e2);* _temp833=({ struct Cyc_Core_Opt* _temp837=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp837->v=( void*) e2; _temp837;});}}
return 1; _LL822: return(( int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)((
struct _tagged_arr)({ struct _tagged_arr(* _temp838)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp842= _tag_arr("subset_effect: bad effect: %s",
sizeof( unsigned char), 30u); struct Cyc_Stdio_String_pa_struct _temp843;
_temp843.tag= Cyc_Stdio_String_pa; _temp843.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
e1);{ void* _temp841=( void*)& _temp843; void* _temp839[ 1u]={ _temp841}; struct
_tagged_arr _temp840={( void*) _temp839,( void*) _temp839,( void*)( _temp839 + 1u)};
_temp838( _temp842, _temp840);}})); _LL812:;}} struct _tuple10{ struct _tuple9*
f1; struct _tuple9* f2; } ; static int Cyc_Tcutil_unify_effect( void* e1, void*
e2){ e1= Cyc_Tcutil_compress( e1); e2= Cyc_Tcutil_compress( e2); Cyc_Tcutil_normalize_effect(
e1); Cyc_Tcutil_normalize_effect( e2);{ struct _tuple10 _temp845=({ struct
_tuple10 _temp844; _temp844.f1= Cyc_Tcutil_get_effect_evar( e1); _temp844.f2=
Cyc_Tcutil_get_effect_evar( e2); _temp844;}); struct _tuple9* _temp851; struct
_tuple9 _temp853; struct Cyc_List_List* _temp854; struct Cyc_List_List* _temp856;
void* _temp858; struct _tuple9* _temp860; struct _tuple9 _temp862; struct Cyc_List_List*
_temp863; struct Cyc_List_List* _temp865; void* _temp867; _LL847: _LL861:
_temp860= _temp845.f1; if( _temp860 == 0){ goto _LL849;} else{ _temp862=*
_temp860; _LL868: _temp867= _temp862.f1; goto _LL866; _LL866: _temp865= _temp862.f2;
goto _LL864; _LL864: _temp863= _temp862.f3; goto _LL852;} _LL852: _temp851=
_temp845.f2; if( _temp851 == 0){ goto _LL849;} else{ _temp853=* _temp851; _LL859:
_temp858= _temp853.f1; goto _LL857; _LL857: _temp856= _temp853.f2; goto _LL855;
_LL855: _temp854= _temp853.f3; goto _LL848;} _LL849: goto _LL850; _LL848: {
struct Cyc_List_List* _temp869= 0; for( 0; _temp863 != 0; _temp863=(( struct Cyc_List_List*)
_check_null( _temp863))->tl){ if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, _temp854,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp863))->hd)){ _temp869=({ struct Cyc_List_List* _temp870=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp870->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp863))->hd);
_temp870->tl= _temp869; _temp870;});}}{ void* _temp871= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,({ struct Cyc_Core_Opt* _temp876=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp876->v=( void*)
_temp869; _temp876;})); struct Cyc_Absyn_JoinEff_struct* _temp872=({ struct Cyc_Absyn_JoinEff_struct*
_temp873=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp873[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp874; _temp874.tag= Cyc_Absyn_JoinEff;
_temp874.f1=({ struct Cyc_List_List* _temp875=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp875->hd=( void*) _temp871; _temp875->tl=
Cyc_List_append( _temp865, _temp856); _temp875;}); _temp874;}); _temp873;});
return Cyc_Tcutil_unify( _temp867,( void*) _temp872)? Cyc_Tcutil_unify( _temp858,(
void*) _temp872): 0;}} _LL850: return Cyc_Tcutil_subset_effect( 0, e1, e2)? Cyc_Tcutil_subset_effect(
0, e2, e1): 0; _LL846:;}} static int Cyc_Tcutil_sub_rgnpo( struct Cyc_List_List*
rpo1, struct Cyc_List_List* rpo2){{ struct Cyc_List_List* r1= rpo1; for( 0; r1
!= 0; r1=(( struct Cyc_List_List*) _check_null( r1))->tl){ struct _tuple8
_temp879; void* _temp880; void* _temp882; struct _tuple8* _temp877=( struct
_tuple8*)(( struct Cyc_List_List*) _check_null( r1))->hd; _temp879=* _temp877;
_LL883: _temp882= _temp879.f1; goto _LL881; _LL881: _temp880= _temp879.f2; goto
_LL878; _LL878: { int found= _temp882 ==( void*) Cyc_Absyn_HeapRgn;{ struct Cyc_List_List*
r2= rpo2; for( 0; r2 != 0? ! found: 0; r2=(( struct Cyc_List_List*) _check_null(
r2))->tl){ struct _tuple8 _temp886; void* _temp887; void* _temp889; struct
_tuple8* _temp884=( struct _tuple8*)(( struct Cyc_List_List*) _check_null( r2))->hd;
_temp886=* _temp884; _LL890: _temp889= _temp886.f1; goto _LL888; _LL888:
_temp887= _temp886.f2; goto _LL885; _LL885: if( Cyc_Tcutil_unify( _temp882,
_temp889)? Cyc_Tcutil_unify( _temp880, _temp887): 0){ found= 1; break;}}} if( !
found){ return 0;}}}} return 1;} static int Cyc_Tcutil_same_rgn_po( struct Cyc_List_List*
rpo1, struct Cyc_List_List* rpo2){ return Cyc_Tcutil_sub_rgnpo( rpo1, rpo2)? Cyc_Tcutil_sub_rgnpo(
rpo2, rpo1): 0;} struct _tuple11{ struct Cyc_Absyn_VarargInfo* f1; struct Cyc_Absyn_VarargInfo*
f2; } ; struct _tuple12{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ;
void Cyc_Tcutil_unify_it( void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2=
Cyc_Tcutil_compress( t2); if( t1 == t2){ return;}{ void* _temp891= t1; struct
Cyc_Core_Opt* _temp897; struct Cyc_Core_Opt* _temp899; struct Cyc_Core_Opt**
_temp901; struct Cyc_Core_Opt* _temp902; _LL893: if(( unsigned int) _temp891 > 4u?*((
int*) _temp891) == Cyc_Absyn_Evar: 0){ _LL903: _temp902=(( struct Cyc_Absyn_Evar_struct*)
_temp891)->f1; goto _LL900; _LL900: _temp899=(( struct Cyc_Absyn_Evar_struct*)
_temp891)->f2; _temp901=&(( struct Cyc_Absyn_Evar_struct*) _temp891)->f2; goto
_LL898; _LL898: _temp897=(( struct Cyc_Absyn_Evar_struct*) _temp891)->f4; goto
_LL894;} else{ goto _LL895;} _LL895: goto _LL896; _LL894: Cyc_Tcutil_occurs( t1,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp897))->v, t2);{
void* _temp904= Cyc_Tcutil_typ_kind( t2); if( Cyc_Tcutil_kind_leq( _temp904,(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp902))->v)){* _temp901=({ struct
Cyc_Core_Opt* _temp905=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp905->v=( void*) t2; _temp905;}); return;} else{{ void* _temp906= t2; struct
Cyc_Core_Opt* _temp914; struct Cyc_Core_Opt* _temp916; struct Cyc_Core_Opt**
_temp918; struct Cyc_Absyn_PtrInfo _temp920; _LL908: if(( unsigned int) _temp906
> 4u?*(( int*) _temp906) == Cyc_Absyn_Evar: 0){ _LL917: _temp916=(( struct Cyc_Absyn_Evar_struct*)
_temp906)->f2; _temp918=&(( struct Cyc_Absyn_Evar_struct*) _temp906)->f2; goto
_LL915; _LL915: _temp914=(( struct Cyc_Absyn_Evar_struct*) _temp906)->f4; goto
_LL909;} else{ goto _LL910;} _LL910: if(( unsigned int) _temp906 > 4u?*(( int*)
_temp906) == Cyc_Absyn_PointerType: 0){ _LL921: _temp920=(( struct Cyc_Absyn_PointerType_struct*)
_temp906)->f1; goto _LL919;} else{ goto _LL912;} _LL919: if(( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp902))->v ==( void*) Cyc_Absyn_BoxKind){ goto
_LL911;} else{ goto _LL912;} _LL912: goto _LL913; _LL909: { struct Cyc_List_List*
_temp922=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp897))->v;{
struct Cyc_List_List* s2=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp914))->v; for( 0; s2 != 0; s2=(( struct Cyc_List_List*)
_check_null( s2))->tl){ if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, _temp922,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( s2))->hd)){( int) _throw(( void*) Cyc_Tcutil_Unify);}}} if( Cyc_Tcutil_kind_leq((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp902))->v, _temp904)){* _temp918=({
struct Cyc_Core_Opt* _temp923=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp923->v=( void*) t1; _temp923;}); return;} goto _LL907;}
_LL911: { struct Cyc_Absyn_Conref* _temp924= Cyc_Absyn_compress_conref( _temp920.bounds);{
void* _temp925=( void*) _temp924->v; _LL927: if( _temp925 ==( void*) Cyc_Absyn_No_constr){
goto _LL928;} else{ goto _LL929;} _LL929: goto _LL930; _LL928:( void*)( _temp924->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp931=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp931[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp932; _temp932.tag= Cyc_Absyn_Eq_constr; _temp932.f1=(
void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp933=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp933[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp934; _temp934.tag= Cyc_Absyn_Upper_b; _temp934.f1= Cyc_Absyn_signed_int_exp(
1, 0); _temp934;}); _temp933;})); _temp932;}); _temp931;})));* _temp901=({
struct Cyc_Core_Opt* _temp935=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp935->v=( void*) t2; _temp935;}); return; _LL930: goto
_LL926; _LL926:;} goto _LL907;} _LL913: goto _LL907; _LL907:;}( int) _throw((
void*) Cyc_Tcutil_Unify);}} _LL896: goto _LL892; _LL892:;}{ struct _tuple8
_temp937=({ struct _tuple8 _temp936; _temp936.f1= t2; _temp936.f2= t1; _temp936;});
void* _temp987; void* _temp989; void* _temp991; void* _temp993; struct Cyc_Absyn_Tvar*
_temp995; void* _temp997; struct Cyc_Absyn_Tvar* _temp999; void* _temp1001;
struct Cyc_Absyn_Structdecl** _temp1003; struct Cyc_List_List* _temp1005; struct
_tuple1* _temp1007; void* _temp1009; struct Cyc_Absyn_Structdecl** _temp1011;
struct Cyc_List_List* _temp1013; struct _tuple1* _temp1015; void* _temp1017;
struct _tuple1* _temp1019; void* _temp1021; struct _tuple1* _temp1023; void*
_temp1025; struct Cyc_Absyn_Uniondecl** _temp1027; struct Cyc_List_List*
_temp1029; struct _tuple1* _temp1031; void* _temp1033; struct Cyc_Absyn_Uniondecl**
_temp1035; struct Cyc_List_List* _temp1037; struct _tuple1* _temp1039; void*
_temp1041; struct Cyc_Absyn_TunionInfo _temp1043; void* _temp1045; struct Cyc_List_List*
_temp1047; void* _temp1049; struct Cyc_Absyn_Tuniondecl* _temp1051; void*
_temp1053; struct Cyc_Absyn_TunionInfo _temp1055; void* _temp1057; struct Cyc_List_List*
_temp1059; void* _temp1061; struct Cyc_Absyn_Tuniondecl* _temp1063; void*
_temp1065; struct Cyc_Absyn_TunionFieldInfo _temp1067; struct Cyc_List_List*
_temp1069; void* _temp1071; struct Cyc_Absyn_Tunionfield* _temp1073; struct Cyc_Absyn_Tuniondecl*
_temp1075; void* _temp1077; struct Cyc_Absyn_TunionFieldInfo _temp1079; struct
Cyc_List_List* _temp1081; void* _temp1083; struct Cyc_Absyn_Tunionfield*
_temp1085; struct Cyc_Absyn_Tuniondecl* _temp1087; void* _temp1089; struct Cyc_Absyn_PtrInfo
_temp1091; struct Cyc_Absyn_Conref* _temp1093; struct Cyc_Absyn_Tqual _temp1095;
struct Cyc_Absyn_Conref* _temp1097; void* _temp1099; void* _temp1101; void*
_temp1103; struct Cyc_Absyn_PtrInfo _temp1105; struct Cyc_Absyn_Conref*
_temp1107; struct Cyc_Absyn_Tqual _temp1109; struct Cyc_Absyn_Conref* _temp1111;
void* _temp1113; void* _temp1115; void* _temp1117; void* _temp1119; void*
_temp1121; void* _temp1123; void* _temp1125; void* _temp1127; void* _temp1129;
void* _temp1131; void* _temp1133; void* _temp1135; void* _temp1137; struct Cyc_Absyn_Exp*
_temp1139; struct Cyc_Absyn_Tqual _temp1141; void* _temp1143; void* _temp1145;
struct Cyc_Absyn_Exp* _temp1147; struct Cyc_Absyn_Tqual _temp1149; void*
_temp1151; void* _temp1153; struct Cyc_Absyn_FnInfo _temp1155; struct Cyc_List_List*
_temp1157; struct Cyc_List_List* _temp1159; struct Cyc_Absyn_VarargInfo*
_temp1161; int _temp1163; struct Cyc_List_List* _temp1165; void* _temp1167;
struct Cyc_Core_Opt* _temp1169; struct Cyc_List_List* _temp1171; void* _temp1173;
struct Cyc_Absyn_FnInfo _temp1175; struct Cyc_List_List* _temp1177; struct Cyc_List_List*
_temp1179; struct Cyc_Absyn_VarargInfo* _temp1181; int _temp1183; struct Cyc_List_List*
_temp1185; void* _temp1187; struct Cyc_Core_Opt* _temp1189; struct Cyc_List_List*
_temp1191; void* _temp1193; struct Cyc_List_List* _temp1195; void* _temp1197;
struct Cyc_List_List* _temp1199; void* _temp1201; struct Cyc_List_List*
_temp1203; void* _temp1205; struct Cyc_List_List* _temp1207; void* _temp1209;
struct Cyc_List_List* _temp1211; void* _temp1213; struct Cyc_List_List*
_temp1215; void* _temp1217; void* _temp1219; void* _temp1221; void* _temp1223;
void* _temp1225; void* _temp1227; void* _temp1229; void* _temp1231; void*
_temp1233; void* _temp1235; _LL939: _LL988: _temp987= _temp937.f1; if((
unsigned int) _temp987 > 4u?*(( int*) _temp987) == Cyc_Absyn_Evar: 0){ goto
_LL940;} else{ goto _LL941;} _LL941: _LL992: _temp991= _temp937.f1; if( _temp991
==( void*) Cyc_Absyn_VoidType){ goto _LL990;} else{ goto _LL943;} _LL990:
_temp989= _temp937.f2; if( _temp989 ==( void*) Cyc_Absyn_VoidType){ goto _LL942;}
else{ goto _LL943;} _LL943: _LL998: _temp997= _temp937.f1; if(( unsigned int)
_temp997 > 4u?*(( int*) _temp997) == Cyc_Absyn_VarType: 0){ _LL1000: _temp999=((
struct Cyc_Absyn_VarType_struct*) _temp997)->f1; goto _LL994;} else{ goto _LL945;}
_LL994: _temp993= _temp937.f2; if(( unsigned int) _temp993 > 4u?*(( int*)
_temp993) == Cyc_Absyn_VarType: 0){ _LL996: _temp995=(( struct Cyc_Absyn_VarType_struct*)
_temp993)->f1; goto _LL944;} else{ goto _LL945;} _LL945: _LL1010: _temp1009=
_temp937.f1; if(( unsigned int) _temp1009 > 4u?*(( int*) _temp1009) == Cyc_Absyn_StructType:
0){ _LL1016: _temp1015=(( struct Cyc_Absyn_StructType_struct*) _temp1009)->f1;
goto _LL1014; _LL1014: _temp1013=(( struct Cyc_Absyn_StructType_struct*)
_temp1009)->f2; goto _LL1012; _LL1012: _temp1011=(( struct Cyc_Absyn_StructType_struct*)
_temp1009)->f3; goto _LL1002;} else{ goto _LL947;} _LL1002: _temp1001= _temp937.f2;
if(( unsigned int) _temp1001 > 4u?*(( int*) _temp1001) == Cyc_Absyn_StructType:
0){ _LL1008: _temp1007=(( struct Cyc_Absyn_StructType_struct*) _temp1001)->f1;
goto _LL1006; _LL1006: _temp1005=(( struct Cyc_Absyn_StructType_struct*)
_temp1001)->f2; goto _LL1004; _LL1004: _temp1003=(( struct Cyc_Absyn_StructType_struct*)
_temp1001)->f3; goto _LL946;} else{ goto _LL947;} _LL947: _LL1022: _temp1021=
_temp937.f1; if(( unsigned int) _temp1021 > 4u?*(( int*) _temp1021) == Cyc_Absyn_EnumType:
0){ _LL1024: _temp1023=(( struct Cyc_Absyn_EnumType_struct*) _temp1021)->f1;
goto _LL1018;} else{ goto _LL949;} _LL1018: _temp1017= _temp937.f2; if((
unsigned int) _temp1017 > 4u?*(( int*) _temp1017) == Cyc_Absyn_EnumType: 0){
_LL1020: _temp1019=(( struct Cyc_Absyn_EnumType_struct*) _temp1017)->f1; goto
_LL948;} else{ goto _LL949;} _LL949: _LL1034: _temp1033= _temp937.f1; if((
unsigned int) _temp1033 > 4u?*(( int*) _temp1033) == Cyc_Absyn_UnionType: 0){
_LL1040: _temp1039=(( struct Cyc_Absyn_UnionType_struct*) _temp1033)->f1; goto
_LL1038; _LL1038: _temp1037=(( struct Cyc_Absyn_UnionType_struct*) _temp1033)->f2;
goto _LL1036; _LL1036: _temp1035=(( struct Cyc_Absyn_UnionType_struct*)
_temp1033)->f3; goto _LL1026;} else{ goto _LL951;} _LL1026: _temp1025= _temp937.f2;
if(( unsigned int) _temp1025 > 4u?*(( int*) _temp1025) == Cyc_Absyn_UnionType: 0){
_LL1032: _temp1031=(( struct Cyc_Absyn_UnionType_struct*) _temp1025)->f1; goto
_LL1030; _LL1030: _temp1029=(( struct Cyc_Absyn_UnionType_struct*) _temp1025)->f2;
goto _LL1028; _LL1028: _temp1027=(( struct Cyc_Absyn_UnionType_struct*)
_temp1025)->f3; goto _LL950;} else{ goto _LL951;} _LL951: _LL1054: _temp1053=
_temp937.f1; if(( unsigned int) _temp1053 > 4u?*(( int*) _temp1053) == Cyc_Absyn_TunionType:
0){ _LL1056: _temp1055=(( struct Cyc_Absyn_TunionType_struct*) _temp1053)->f1;
_LL1062: _temp1061=( void*) _temp1055.tunion_info; if(*(( int*) _temp1061) ==
Cyc_Absyn_KnownTunion){ _LL1064: _temp1063=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1061)->f1; goto _LL1060;} else{ goto _LL953;} _LL1060: _temp1059= _temp1055.targs;
goto _LL1058; _LL1058: _temp1057=( void*) _temp1055.rgn; goto _LL1042;} else{
goto _LL953;} _LL1042: _temp1041= _temp937.f2; if(( unsigned int) _temp1041 > 4u?*((
int*) _temp1041) == Cyc_Absyn_TunionType: 0){ _LL1044: _temp1043=(( struct Cyc_Absyn_TunionType_struct*)
_temp1041)->f1; _LL1050: _temp1049=( void*) _temp1043.tunion_info; if(*(( int*)
_temp1049) == Cyc_Absyn_KnownTunion){ _LL1052: _temp1051=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1049)->f1; goto _LL1048;} else{ goto _LL953;} _LL1048: _temp1047= _temp1043.targs;
goto _LL1046; _LL1046: _temp1045=( void*) _temp1043.rgn; goto _LL952;} else{
goto _LL953;} _LL953: _LL1078: _temp1077= _temp937.f1; if(( unsigned int)
_temp1077 > 4u?*(( int*) _temp1077) == Cyc_Absyn_TunionFieldType: 0){ _LL1080:
_temp1079=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1077)->f1; _LL1084:
_temp1083=( void*) _temp1079.field_info; if(*(( int*) _temp1083) == Cyc_Absyn_KnownTunionfield){
_LL1088: _temp1087=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1083)->f1;
goto _LL1086; _LL1086: _temp1085=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1083)->f2; goto _LL1082;} else{ goto _LL955;} _LL1082: _temp1081= _temp1079.targs;
goto _LL1066;} else{ goto _LL955;} _LL1066: _temp1065= _temp937.f2; if((
unsigned int) _temp1065 > 4u?*(( int*) _temp1065) == Cyc_Absyn_TunionFieldType:
0){ _LL1068: _temp1067=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1065)->f1;
_LL1072: _temp1071=( void*) _temp1067.field_info; if(*(( int*) _temp1071) == Cyc_Absyn_KnownTunionfield){
_LL1076: _temp1075=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1071)->f1;
goto _LL1074; _LL1074: _temp1073=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1071)->f2; goto _LL1070;} else{ goto _LL955;} _LL1070: _temp1069= _temp1067.targs;
goto _LL954;} else{ goto _LL955;} _LL955: _LL1104: _temp1103= _temp937.f1; if((
unsigned int) _temp1103 > 4u?*(( int*) _temp1103) == Cyc_Absyn_PointerType: 0){
_LL1106: _temp1105=(( struct Cyc_Absyn_PointerType_struct*) _temp1103)->f1;
_LL1116: _temp1115=( void*) _temp1105.elt_typ; goto _LL1114; _LL1114: _temp1113=(
void*) _temp1105.rgn_typ; goto _LL1112; _LL1112: _temp1111= _temp1105.nullable;
goto _LL1110; _LL1110: _temp1109= _temp1105.tq; goto _LL1108; _LL1108: _temp1107=
_temp1105.bounds; goto _LL1090;} else{ goto _LL957;} _LL1090: _temp1089=
_temp937.f2; if(( unsigned int) _temp1089 > 4u?*(( int*) _temp1089) == Cyc_Absyn_PointerType:
0){ _LL1092: _temp1091=(( struct Cyc_Absyn_PointerType_struct*) _temp1089)->f1;
_LL1102: _temp1101=( void*) _temp1091.elt_typ; goto _LL1100; _LL1100: _temp1099=(
void*) _temp1091.rgn_typ; goto _LL1098; _LL1098: _temp1097= _temp1091.nullable;
goto _LL1096; _LL1096: _temp1095= _temp1091.tq; goto _LL1094; _LL1094: _temp1093=
_temp1091.bounds; goto _LL956;} else{ goto _LL957;} _LL957: _LL1124: _temp1123=
_temp937.f1; if(( unsigned int) _temp1123 > 4u?*(( int*) _temp1123) == Cyc_Absyn_IntType:
0){ _LL1128: _temp1127=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1123)->f1;
goto _LL1126; _LL1126: _temp1125=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1123)->f2; goto _LL1118;} else{ goto _LL959;} _LL1118: _temp1117= _temp937.f2;
if(( unsigned int) _temp1117 > 4u?*(( int*) _temp1117) == Cyc_Absyn_IntType: 0){
_LL1122: _temp1121=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1117)->f1;
goto _LL1120; _LL1120: _temp1119=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1117)->f2; goto _LL958;} else{ goto _LL959;} _LL959: _LL1132: _temp1131=
_temp937.f1; if( _temp1131 ==( void*) Cyc_Absyn_FloatType){ goto _LL1130;} else{
goto _LL961;} _LL1130: _temp1129= _temp937.f2; if( _temp1129 ==( void*) Cyc_Absyn_FloatType){
goto _LL960;} else{ goto _LL961;} _LL961: _LL1136: _temp1135= _temp937.f1; if(
_temp1135 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1134;} else{ goto _LL963;}
_LL1134: _temp1133= _temp937.f2; if( _temp1133 ==( void*) Cyc_Absyn_DoubleType){
goto _LL962;} else{ goto _LL963;} _LL963: _LL1146: _temp1145= _temp937.f1; if((
unsigned int) _temp1145 > 4u?*(( int*) _temp1145) == Cyc_Absyn_ArrayType: 0){
_LL1152: _temp1151=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1145)->f1;
goto _LL1150; _LL1150: _temp1149=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1145)->f2; goto _LL1148; _LL1148: _temp1147=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1145)->f3; goto _LL1138;} else{ goto _LL965;} _LL1138: _temp1137= _temp937.f2;
if(( unsigned int) _temp1137 > 4u?*(( int*) _temp1137) == Cyc_Absyn_ArrayType: 0){
_LL1144: _temp1143=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1137)->f1;
goto _LL1142; _LL1142: _temp1141=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1137)->f2; goto _LL1140; _LL1140: _temp1139=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1137)->f3; goto _LL964;} else{ goto _LL965;} _LL965: _LL1174: _temp1173=
_temp937.f1; if(( unsigned int) _temp1173 > 4u?*(( int*) _temp1173) == Cyc_Absyn_FnType:
0){ _LL1176: _temp1175=(( struct Cyc_Absyn_FnType_struct*) _temp1173)->f1;
_LL1192: _temp1191= _temp1175.tvars; goto _LL1190; _LL1190: _temp1189= _temp1175.effect;
goto _LL1188; _LL1188: _temp1187=( void*) _temp1175.ret_typ; goto _LL1186;
_LL1186: _temp1185= _temp1175.args; goto _LL1184; _LL1184: _temp1183= _temp1175.c_varargs;
goto _LL1182; _LL1182: _temp1181= _temp1175.cyc_varargs; goto _LL1180; _LL1180:
_temp1179= _temp1175.rgn_po; goto _LL1178; _LL1178: _temp1177= _temp1175.attributes;
goto _LL1154;} else{ goto _LL967;} _LL1154: _temp1153= _temp937.f2; if((
unsigned int) _temp1153 > 4u?*(( int*) _temp1153) == Cyc_Absyn_FnType: 0){
_LL1156: _temp1155=(( struct Cyc_Absyn_FnType_struct*) _temp1153)->f1; _LL1172:
_temp1171= _temp1155.tvars; goto _LL1170; _LL1170: _temp1169= _temp1155.effect;
goto _LL1168; _LL1168: _temp1167=( void*) _temp1155.ret_typ; goto _LL1166;
_LL1166: _temp1165= _temp1155.args; goto _LL1164; _LL1164: _temp1163= _temp1155.c_varargs;
goto _LL1162; _LL1162: _temp1161= _temp1155.cyc_varargs; goto _LL1160; _LL1160:
_temp1159= _temp1155.rgn_po; goto _LL1158; _LL1158: _temp1157= _temp1155.attributes;
goto _LL966;} else{ goto _LL967;} _LL967: _LL1198: _temp1197= _temp937.f1; if((
unsigned int) _temp1197 > 4u?*(( int*) _temp1197) == Cyc_Absyn_TupleType: 0){
_LL1200: _temp1199=(( struct Cyc_Absyn_TupleType_struct*) _temp1197)->f1; goto
_LL1194;} else{ goto _LL969;} _LL1194: _temp1193= _temp937.f2; if(( unsigned int)
_temp1193 > 4u?*(( int*) _temp1193) == Cyc_Absyn_TupleType: 0){ _LL1196:
_temp1195=(( struct Cyc_Absyn_TupleType_struct*) _temp1193)->f1; goto _LL968;}
else{ goto _LL969;} _LL969: _LL1206: _temp1205= _temp937.f1; if(( unsigned int)
_temp1205 > 4u?*(( int*) _temp1205) == Cyc_Absyn_AnonStructType: 0){ _LL1208:
_temp1207=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1205)->f1; goto
_LL1202;} else{ goto _LL971;} _LL1202: _temp1201= _temp937.f2; if(( unsigned int)
_temp1201 > 4u?*(( int*) _temp1201) == Cyc_Absyn_AnonStructType: 0){ _LL1204:
_temp1203=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1201)->f1; goto
_LL970;} else{ goto _LL971;} _LL971: _LL1214: _temp1213= _temp937.f1; if((
unsigned int) _temp1213 > 4u?*(( int*) _temp1213) == Cyc_Absyn_AnonUnionType: 0){
_LL1216: _temp1215=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1213)->f1;
goto _LL1210;} else{ goto _LL973;} _LL1210: _temp1209= _temp937.f2; if((
unsigned int) _temp1209 > 4u?*(( int*) _temp1209) == Cyc_Absyn_AnonUnionType: 0){
_LL1212: _temp1211=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1209)->f1;
goto _LL972;} else{ goto _LL973;} _LL973: _LL1220: _temp1219= _temp937.f1; if(
_temp1219 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1218;} else{ goto _LL975;}
_LL1218: _temp1217= _temp937.f2; if( _temp1217 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL974;} else{ goto _LL975;} _LL975: _LL1226: _temp1225= _temp937.f1; if((
unsigned int) _temp1225 > 4u?*(( int*) _temp1225) == Cyc_Absyn_RgnHandleType: 0){
_LL1228: _temp1227=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1225)->f1;
goto _LL1222;} else{ goto _LL977;} _LL1222: _temp1221= _temp937.f2; if((
unsigned int) _temp1221 > 4u?*(( int*) _temp1221) == Cyc_Absyn_RgnHandleType: 0){
_LL1224: _temp1223=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1221)->f1;
goto _LL976;} else{ goto _LL977;} _LL977: _LL1230: _temp1229= _temp937.f1; if((
unsigned int) _temp1229 > 4u?*(( int*) _temp1229) == Cyc_Absyn_JoinEff: 0){ goto
_LL978;} else{ goto _LL979;} _LL979: _LL1232: _temp1231= _temp937.f2; if((
unsigned int) _temp1231 > 4u?*(( int*) _temp1231) == Cyc_Absyn_JoinEff: 0){ goto
_LL980;} else{ goto _LL981;} _LL981: _LL1234: _temp1233= _temp937.f1; if((
unsigned int) _temp1233 > 4u?*(( int*) _temp1233) == Cyc_Absyn_AccessEff: 0){
goto _LL982;} else{ goto _LL983;} _LL983: _LL1236: _temp1235= _temp937.f2; if((
unsigned int) _temp1235 > 4u?*(( int*) _temp1235) == Cyc_Absyn_AccessEff: 0){
goto _LL984;} else{ goto _LL985;} _LL985: goto _LL986; _LL940: Cyc_Tcutil_unify_it(
t2, t1); return; _LL942: return; _LL944: { struct _tagged_arr* _temp1237=
_temp999->name; struct _tagged_arr* _temp1238= _temp995->name; int _temp1239=*((
int*) _check_null( _temp999->identity)); int _temp1240=*(( int*) _check_null(
_temp995->identity)); void* _temp1241= Cyc_Absyn_conref_val( _temp999->kind);
void* _temp1242= Cyc_Absyn_conref_val( _temp995->kind); if( _temp1240 ==
_temp1239? Cyc_String_zstrptrcmp( _temp1237, _temp1238) == 0: 0){ if( _temp1241
!= _temp1242){(( int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)(( struct
_tagged_arr)({ struct _tagged_arr(* _temp1243)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp1249= _tag_arr("same type variable %s has kinds %s and %s",
sizeof( unsigned char), 42u); struct Cyc_Stdio_String_pa_struct _temp1252;
_temp1252.tag= Cyc_Stdio_String_pa; _temp1252.f1=( struct _tagged_arr)*
_temp1237;{ void* _temp1246=( void*)& _temp1252; struct Cyc_Stdio_String_pa_struct
_temp1251; _temp1251.tag= Cyc_Stdio_String_pa; _temp1251.f1=( struct _tagged_arr)
Cyc_Absynpp_kind2string( _temp1241);{ void* _temp1247=( void*)& _temp1251;
struct Cyc_Stdio_String_pa_struct _temp1250; _temp1250.tag= Cyc_Stdio_String_pa;
_temp1250.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( _temp1242);{ void*
_temp1248=( void*)& _temp1250; void* _temp1244[ 3u]={ _temp1246, _temp1247,
_temp1248}; struct _tagged_arr _temp1245={( void*) _temp1244,( void*) _temp1244,(
void*)( _temp1244 + 3u)}; _temp1243( _temp1249, _temp1245);}}}}));} return;}
goto _LL938;} _LL946: if((( _temp1007 != 0? _temp1015 != 0: 0)? Cyc_Absyn_qvar_cmp((
struct _tuple1*) _check_null( _temp1007),( struct _tuple1*) _check_null(
_temp1015)) == 0: 0)? 1:( _temp1007 == 0? _temp1015 == 0: 0)){ Cyc_Tcutil_unify_list(
_temp1005, _temp1013); return;} goto _LL938; _LL948: if( Cyc_Absyn_qvar_cmp(
_temp1023, _temp1019) == 0){ return;} goto _LL938; _LL950: if((( _temp1031 != 0?
_temp1039 != 0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*) _check_null( _temp1031),(
struct _tuple1*) _check_null( _temp1039)) == 0: 0)? 1:( _temp1031 == 0?
_temp1039 == 0: 0)){ Cyc_Tcutil_unify_list( _temp1029, _temp1037); return;} goto
_LL938; _LL952: if( _temp1063 == _temp1051? 1: Cyc_Absyn_qvar_cmp( _temp1063->name,
_temp1051->name) == 0){ Cyc_Tcutil_unify_it( _temp1045, _temp1057); Cyc_Tcutil_unify_list(
_temp1047, _temp1059); return;} goto _LL938; _LL954: if(( _temp1087 == _temp1075?
1: Cyc_Absyn_qvar_cmp( _temp1087->name, _temp1075->name) == 0)? _temp1085 ==
_temp1073? 1: Cyc_Absyn_qvar_cmp( _temp1085->name, _temp1073->name) == 0: 0){
Cyc_Tcutil_unify_list( _temp1069, _temp1081); return;} goto _LL938; _LL956: Cyc_Tcutil_unify_it(
_temp1101, _temp1115); Cyc_Tcutil_unify_it( _temp1113, _temp1099); Cyc_Tcutil_unify_tqual(
_temp1095, _temp1109); Cyc_Tcutil_unify_it_conrefs( Cyc_Tcutil_boundscmp,
_temp1093, _temp1107);{ void* _temp1253=( void*)( Cyc_Absyn_compress_conref(
_temp1093))->v; void* _temp1259; _LL1255: if(( unsigned int) _temp1253 > 1u?*((
int*) _temp1253) == Cyc_Absyn_Eq_constr: 0){ _LL1260: _temp1259=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1253)->f1; if( _temp1259 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1256;} else{ goto _LL1257;}} else{ goto _LL1257;} _LL1257: goto _LL1258;
_LL1256: return; _LL1258: goto _LL1254; _LL1254:;}(( void(*)( int(* cmp)( int,
int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp, _temp1097, _temp1111); return; _LL958: if( _temp1121 ==
_temp1127? _temp1119 == _temp1125: 0){ return;} goto _LL938; _LL960: return;
_LL962: return; _LL964: Cyc_Tcutil_unify_it( _temp1143, _temp1151); Cyc_Tcutil_unify_tqual(
_temp1141, _temp1149); if( _temp1147 == _temp1139){ return;} if( _temp1147 == 0?
1: _temp1139 == 0){ goto _LL938;} if( Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1147)) == Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1139))){ return;} goto _LL938; _LL966: { int done= 0;{ struct
_RegionHandle _temp1261= _new_region(); struct _RegionHandle* rgn=& _temp1261;
_push_region( rgn);{ struct Cyc_List_List* inst= 0; while( _temp1171 != 0) { if(
_temp1191 == 0){ break;} inst=({ struct Cyc_List_List* _temp1262=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1262->hd=( void*)({
struct _tuple5* _temp1263=( struct _tuple5*) _region_malloc( rgn, sizeof( struct
_tuple5)); _temp1263->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp1191))->hd; _temp1263->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1264=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1264[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1265; _temp1265.tag= Cyc_Absyn_VarType;
_temp1265.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp1171))->hd; _temp1265;}); _temp1264;}); _temp1263;}); _temp1262->tl= inst;
_temp1262;}); _temp1171=(( struct Cyc_List_List*) _check_null( _temp1171))->tl;
_temp1191=(( struct Cyc_List_List*) _check_null( _temp1191))->tl;} if( _temp1191
!= 0){ _npop_handler( 0u); goto _LL938;} if( inst != 0){ Cyc_Tcutil_unify_it((
void*)({ struct Cyc_Absyn_FnType_struct* _temp1266=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1266[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1267; _temp1267.tag= Cyc_Absyn_FnType; _temp1267.f1=({ struct Cyc_Absyn_FnInfo
_temp1268; _temp1268.tvars= 0; _temp1268.effect= _temp1169; _temp1268.ret_typ=(
void*) _temp1167; _temp1268.args= _temp1165; _temp1268.c_varargs= _temp1163;
_temp1268.cyc_varargs= _temp1161; _temp1268.rgn_po= _temp1159; _temp1268.attributes=
_temp1157; _temp1268;}); _temp1267;}); _temp1266;}), Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1269=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1269[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1270; _temp1270.tag= Cyc_Absyn_FnType; _temp1270.f1=({ struct Cyc_Absyn_FnInfo
_temp1271; _temp1271.tvars= 0; _temp1271.effect= _temp1189; _temp1271.ret_typ=(
void*) _temp1187; _temp1271.args= _temp1185; _temp1271.c_varargs= _temp1183;
_temp1271.cyc_varargs= _temp1181; _temp1271.rgn_po= _temp1179; _temp1271.attributes=
_temp1177; _temp1271;}); _temp1270;}); _temp1269;}))); done= 1;}}; _pop_region(
rgn);} if( done){ return;} Cyc_Tcutil_unify_it( _temp1167, _temp1187); for( 0;
_temp1165 != 0? _temp1185 != 0: 0;( _temp1165=(( struct Cyc_List_List*)
_check_null( _temp1165))->tl, _temp1185=(( struct Cyc_List_List*) _check_null(
_temp1185))->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp1165))->hd)).f2,(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp1185))->hd)).f2); Cyc_Tcutil_unify_it((*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp1165))->hd)).f3,(*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp1185))->hd)).f3);} if( _temp1165 != 0?
1: _temp1185 != 0){ goto _LL938;} if( _temp1163 != _temp1183){ goto _LL938;}{
int bad_cyc_vararg= 0;{ struct _tuple11 _temp1273=({ struct _tuple11 _temp1272;
_temp1272.f1= _temp1161; _temp1272.f2= _temp1181; _temp1272;}); struct Cyc_Absyn_VarargInfo*
_temp1283; struct Cyc_Absyn_VarargInfo* _temp1285; struct Cyc_Absyn_VarargInfo*
_temp1287; struct Cyc_Absyn_VarargInfo* _temp1289; struct Cyc_Absyn_VarargInfo*
_temp1291; struct Cyc_Absyn_VarargInfo _temp1293; int _temp1294; void* _temp1296;
void* _temp1298; struct Cyc_Absyn_Tqual _temp1300; struct Cyc_Core_Opt*
_temp1302; struct Cyc_Absyn_VarargInfo* _temp1304; struct Cyc_Absyn_VarargInfo
_temp1306; int _temp1307; void* _temp1309; void* _temp1311; struct Cyc_Absyn_Tqual
_temp1313; struct Cyc_Core_Opt* _temp1315; _LL1275: _LL1286: _temp1285=
_temp1273.f1; if( _temp1285 == 0){ goto _LL1284;} else{ goto _LL1277;} _LL1284:
_temp1283= _temp1273.f2; if( _temp1283 == 0){ goto _LL1276;} else{ goto _LL1277;}
_LL1277: _LL1288: _temp1287= _temp1273.f1; if( _temp1287 == 0){ goto _LL1278;}
else{ goto _LL1279;} _LL1279: _LL1290: _temp1289= _temp1273.f2; if( _temp1289 ==
0){ goto _LL1280;} else{ goto _LL1281;} _LL1281: _LL1305: _temp1304= _temp1273.f1;
if( _temp1304 == 0){ goto _LL1274;} else{ _temp1306=* _temp1304; _LL1316:
_temp1315= _temp1306.name; goto _LL1314; _LL1314: _temp1313= _temp1306.tq; goto
_LL1312; _LL1312: _temp1311=( void*) _temp1306.type; goto _LL1310; _LL1310:
_temp1309=( void*) _temp1306.rgn; goto _LL1308; _LL1308: _temp1307= _temp1306.inject;
goto _LL1292;} _LL1292: _temp1291= _temp1273.f2; if( _temp1291 == 0){ goto
_LL1274;} else{ _temp1293=* _temp1291; _LL1303: _temp1302= _temp1293.name; goto
_LL1301; _LL1301: _temp1300= _temp1293.tq; goto _LL1299; _LL1299: _temp1298=(
void*) _temp1293.type; goto _LL1297; _LL1297: _temp1296=( void*) _temp1293.rgn;
goto _LL1295; _LL1295: _temp1294= _temp1293.inject; goto _LL1282;} _LL1276: goto
_LL1274; _LL1278: goto _LL1280; _LL1280: bad_cyc_vararg= 1; goto _LL1274;
_LL1282: Cyc_Tcutil_unify_tqual( _temp1313, _temp1300); Cyc_Tcutil_unify_it(
_temp1311, _temp1298); Cyc_Tcutil_unify_it( _temp1309, _temp1296); if( _temp1307
!= _temp1294){ bad_cyc_vararg= 1;} goto _LL1274; _LL1274:;} if( bad_cyc_vararg){
goto _LL938;}{ int bad_effect= 0;{ struct _tuple12 _temp1318=({ struct _tuple12
_temp1317; _temp1317.f1= _temp1169; _temp1317.f2= _temp1189; _temp1317;});
struct Cyc_Core_Opt* _temp1328; struct Cyc_Core_Opt* _temp1330; struct Cyc_Core_Opt*
_temp1332; struct Cyc_Core_Opt* _temp1334; _LL1320: _LL1331: _temp1330=
_temp1318.f1; if( _temp1330 == 0){ goto _LL1329;} else{ goto _LL1322;} _LL1329:
_temp1328= _temp1318.f2; if( _temp1328 == 0){ goto _LL1321;} else{ goto _LL1322;}
_LL1322: _LL1333: _temp1332= _temp1318.f1; if( _temp1332 == 0){ goto _LL1323;}
else{ goto _LL1324;} _LL1324: _LL1335: _temp1334= _temp1318.f2; if( _temp1334 ==
0){ goto _LL1325;} else{ goto _LL1326;} _LL1326: goto _LL1327; _LL1321: goto
_LL1319; _LL1323: goto _LL1325; _LL1325: bad_effect= 1; goto _LL1319; _LL1327:
bad_effect= ! Cyc_Tcutil_unify_effect(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1169))->v,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1189))->v); goto _LL1319; _LL1319:;} if( bad_effect){ goto _LL938;} if( !
Cyc_Tcutil_same_atts( _temp1177, _temp1157)){ goto _LL938;} if( ! Cyc_Tcutil_same_rgn_po(
_temp1179, _temp1159)){ goto _LL938;} return;}}} _LL968: for( 0; _temp1195 != 0?
_temp1199 != 0: 0;( _temp1195=(( struct Cyc_List_List*) _check_null( _temp1195))->tl,
_temp1199=(( struct Cyc_List_List*) _check_null( _temp1199))->tl)){ Cyc_Tcutil_unify_tqual((*((
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1195))->hd)).f1,(*((
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1199))->hd)).f1);
Cyc_Tcutil_unify_it((*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp1195))->hd)).f2,(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp1199))->hd)).f2);} if( _temp1195 == 0? _temp1199 == 0: 0){ return;} goto
_LL938; _LL970: _temp1215= _temp1207; _temp1211= _temp1203; goto _LL972; _LL972:
for( 0; _temp1211 != 0? _temp1215 != 0: 0;( _temp1211=(( struct Cyc_List_List*)
_check_null( _temp1211))->tl, _temp1215=(( struct Cyc_List_List*) _check_null(
_temp1215))->tl)){ struct Cyc_Absyn_Structfield* _temp1336=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp1211))->hd; struct Cyc_Absyn_Structfield*
_temp1337=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1215))->hd; if( Cyc_String_zstrptrcmp( _temp1336->name, _temp1337->name) !=
0){( int) _throw(( void*) Cyc_Tcutil_Unify);} Cyc_Tcutil_unify_tqual( _temp1336->tq,
_temp1337->tq); Cyc_Tcutil_unify_it(( void*) _temp1336->type,( void*) _temp1337->type);
if((( ! Cyc_Tcutil_same_atts( _temp1336->attributes, _temp1337->attributes)? 1:(
_temp1336->width != 0? _temp1337->width == 0: 0))? 1:( _temp1337->width != 0?
_temp1336->width == 0: 0))? 1:(( _temp1336->width != 0? _temp1337->width != 0: 0)?
Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*)
_check_null( _temp1336->width))->v) != Cyc_Evexp_eval_const_uint_exp(( struct
Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp1337->width))->v): 0)){(
int) _throw(( void*) Cyc_Tcutil_Unify);}} if( _temp1211 == 0? _temp1215 == 0: 0){
return;} goto _LL938; _LL974: return; _LL976: Cyc_Tcutil_unify_it( _temp1227,
_temp1223); return; _LL978: goto _LL980; _LL980: goto _LL982; _LL982: goto
_LL984; _LL984: if( Cyc_Tcutil_unify_effect( t1, t2)){ return;} else{ goto
_LL938;} _LL986: goto _LL938; _LL938:;}( int) _throw(( void*) Cyc_Tcutil_Unify);}
int Cyc_Tcutil_is_arithmetic_type( void* t){ void* _temp1338= Cyc_Tcutil_compress(
t); _LL1340: if(( unsigned int) _temp1338 > 4u?*(( int*) _temp1338) == Cyc_Absyn_IntType:
0){ goto _LL1341;} else{ goto _LL1342;} _LL1342: if( _temp1338 ==( void*) Cyc_Absyn_FloatType){
goto _LL1343;} else{ goto _LL1344;} _LL1344: if( _temp1338 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1345;} else{ goto _LL1346;} _LL1346: if(( unsigned int) _temp1338 > 4u?*((
int*) _temp1338) == Cyc_Absyn_EnumType: 0){ goto _LL1347;} else{ goto _LL1348;}
_LL1348: goto _LL1349; _LL1341: return 1; _LL1343: return 1; _LL1345: return 1;
_LL1347: return 1; _LL1349: return 0; _LL1339:;} int Cyc_Tcutil_will_lose_precision(
void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple8 _temp1351=({ struct _tuple8 _temp1350; _temp1350.f1= t1;
_temp1350.f2= t2; _temp1350;}); void* _temp1373; void* _temp1375; void*
_temp1377; void* _temp1379; void* _temp1381; void* _temp1383; void* _temp1385;
void* _temp1387; void* _temp1389; void* _temp1391; void* _temp1393; void*
_temp1395; void* _temp1397; void* _temp1399; void* _temp1401; void* _temp1403;
void* _temp1405; void* _temp1407; void* _temp1409; void* _temp1411; void*
_temp1413; void* _temp1415; void* _temp1417; void* _temp1419; void* _temp1421;
void* _temp1423; void* _temp1425; _LL1353: _LL1376: _temp1375= _temp1351.f1; if(
_temp1375 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1374;} else{ goto _LL1355;}
_LL1374: _temp1373= _temp1351.f2; if( _temp1373 ==( void*) Cyc_Absyn_FloatType){
goto _LL1354;} else{ goto _LL1355;} _LL1355: _LL1380: _temp1379= _temp1351.f1;
if( _temp1379 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1378;} else{ goto
_LL1357;} _LL1378: _temp1377= _temp1351.f2; if(( unsigned int) _temp1377 > 4u?*((
int*) _temp1377) == Cyc_Absyn_IntType: 0){ goto _LL1356;} else{ goto _LL1357;}
_LL1357: _LL1384: _temp1383= _temp1351.f1; if( _temp1383 ==( void*) Cyc_Absyn_FloatType){
goto _LL1382;} else{ goto _LL1359;} _LL1382: _temp1381= _temp1351.f2; if((
unsigned int) _temp1381 > 4u?*(( int*) _temp1381) == Cyc_Absyn_IntType: 0){ goto
_LL1358;} else{ goto _LL1359;} _LL1359: _LL1390: _temp1389= _temp1351.f1; if((
unsigned int) _temp1389 > 4u?*(( int*) _temp1389) == Cyc_Absyn_IntType: 0){
_LL1392: _temp1391=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1389)->f2;
if( _temp1391 ==( void*) Cyc_Absyn_B8){ goto _LL1386;} else{ goto _LL1361;}}
else{ goto _LL1361;} _LL1386: _temp1385= _temp1351.f2; if(( unsigned int)
_temp1385 > 4u?*(( int*) _temp1385) == Cyc_Absyn_IntType: 0){ _LL1388: _temp1387=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1385)->f2; if( _temp1387 ==(
void*) Cyc_Absyn_B8){ goto _LL1360;} else{ goto _LL1361;}} else{ goto _LL1361;}
_LL1361: _LL1394: _temp1393= _temp1351.f1; if(( unsigned int) _temp1393 > 4u?*((
int*) _temp1393) == Cyc_Absyn_IntType: 0){ _LL1396: _temp1395=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1393)->f2; if( _temp1395 ==( void*) Cyc_Absyn_B8){
goto _LL1362;} else{ goto _LL1363;}} else{ goto _LL1363;} _LL1363: _LL1400:
_temp1399= _temp1351.f1; if(( unsigned int) _temp1399 > 4u?*(( int*) _temp1399)
== Cyc_Absyn_IntType: 0){ _LL1402: _temp1401=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1399)->f2; if( _temp1401 ==( void*) Cyc_Absyn_B4){ goto _LL1398;} else{
goto _LL1365;}} else{ goto _LL1365;} _LL1398: _temp1397= _temp1351.f2; if(
_temp1397 ==( void*) Cyc_Absyn_FloatType){ goto _LL1364;} else{ goto _LL1365;}
_LL1365: _LL1408: _temp1407= _temp1351.f1; if(( unsigned int) _temp1407 > 4u?*((
int*) _temp1407) == Cyc_Absyn_IntType: 0){ _LL1410: _temp1409=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1407)->f2; if( _temp1409 ==( void*) Cyc_Absyn_B4){
goto _LL1404;} else{ goto _LL1367;}} else{ goto _LL1367;} _LL1404: _temp1403=
_temp1351.f2; if(( unsigned int) _temp1403 > 4u?*(( int*) _temp1403) == Cyc_Absyn_IntType:
0){ _LL1406: _temp1405=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1403)->f2;
if( _temp1405 ==( void*) Cyc_Absyn_B2){ goto _LL1366;} else{ goto _LL1367;}}
else{ goto _LL1367;} _LL1367: _LL1416: _temp1415= _temp1351.f1; if((
unsigned int) _temp1415 > 4u?*(( int*) _temp1415) == Cyc_Absyn_IntType: 0){
_LL1418: _temp1417=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1415)->f2;
if( _temp1417 ==( void*) Cyc_Absyn_B4){ goto _LL1412;} else{ goto _LL1369;}}
else{ goto _LL1369;} _LL1412: _temp1411= _temp1351.f2; if(( unsigned int)
_temp1411 > 4u?*(( int*) _temp1411) == Cyc_Absyn_IntType: 0){ _LL1414: _temp1413=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1411)->f2; if( _temp1413 ==(
void*) Cyc_Absyn_B1){ goto _LL1368;} else{ goto _LL1369;}} else{ goto _LL1369;}
_LL1369: _LL1424: _temp1423= _temp1351.f1; if(( unsigned int) _temp1423 > 4u?*((
int*) _temp1423) == Cyc_Absyn_IntType: 0){ _LL1426: _temp1425=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1423)->f2; if( _temp1425 ==( void*) Cyc_Absyn_B2){
goto _LL1420;} else{ goto _LL1371;}} else{ goto _LL1371;} _LL1420: _temp1419=
_temp1351.f2; if(( unsigned int) _temp1419 > 4u?*(( int*) _temp1419) == Cyc_Absyn_IntType:
0){ _LL1422: _temp1421=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1419)->f2;
if( _temp1421 ==( void*) Cyc_Absyn_B1){ goto _LL1370;} else{ goto _LL1371;}}
else{ goto _LL1371;} _LL1371: goto _LL1372; _LL1354: return 1; _LL1356: return 1;
_LL1358: return 1; _LL1360: return 0; _LL1362: return 1; _LL1364: return 1;
_LL1366: return 1; _LL1368: return 1; _LL1370: return 1; _LL1372: return 0;
_LL1352:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv* te, void* t,
struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{ struct Cyc_List_List*
el= es; for( 0; el != 0; el=(( struct Cyc_List_List*) _check_null( el))->tl){
void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);
if( Cyc_Tcutil_is_arithmetic_type( t1)){ if( max_arith_type == 0? 1: Cyc_Tcutil_will_lose_precision(
t1,( void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){
max_arith_type=({ struct Cyc_Core_Opt* _temp1427=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1427->v=( void*) t1; _temp1427;});}}}}
if( max_arith_type != 0){ if( ! Cyc_Tcutil_unify( t,( void*)(( struct Cyc_Core_Opt*)
_check_null( max_arith_type))->v)){ return 0;}}{ struct Cyc_List_List* el= es;
for( 0; el != 0; el=(( struct Cyc_List_List*) _check_null( el))->tl){ if( ! Cyc_Tcutil_coerce_assign(
te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd, t)){
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
el))->hd)->loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp1428)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp1433= _tag_arr("type mismatch: expecting %s but found %s", sizeof(
unsigned char), 41u); struct Cyc_Stdio_String_pa_struct _temp1435; _temp1435.tag=
Cyc_Stdio_String_pa; _temp1435.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp1431=( void*)& _temp1435; struct Cyc_Stdio_String_pa_struct
_temp1434; _temp1434.tag= Cyc_Stdio_String_pa; _temp1434.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null((( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);{
void* _temp1432=( void*)& _temp1434; void* _temp1429[ 2u]={ _temp1431, _temp1432};
struct _tagged_arr _temp1430={( void*) _temp1429,( void*) _temp1429,( void*)(
_temp1429 + 2u)}; _temp1428( _temp1433, _temp1430);}}})); return 0;}}} return 1;}
int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e){ if( ! Cyc_Tcutil_coerce_sint_typ( te, e)){ void* _temp1436= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _LL1438: if((
unsigned int) _temp1436 > 4u?*(( int*) _temp1436) == Cyc_Absyn_PointerType: 0){
goto _LL1439;} else{ goto _LL1440;} _LL1440: goto _LL1441; _LL1439: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); goto _LL1437; _LL1441: return 0; _LL1437:;} return 1;}
int Cyc_Tcutil_is_integral_type( void* t){ void* _temp1442= Cyc_Tcutil_compress(
t); _LL1444: if(( unsigned int) _temp1442 > 4u?*(( int*) _temp1442) == Cyc_Absyn_IntType:
0){ goto _LL1445;} else{ goto _LL1446;} _LL1446: if(( unsigned int) _temp1442 >
4u?*(( int*) _temp1442) == Cyc_Absyn_EnumType: 0){ goto _LL1447;} else{ goto
_LL1448;} _LL1448: goto _LL1449; _LL1445: return 1; _LL1447: return 1; _LL1449:
return 0; _LL1443:;} int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv* te,
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
t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8 _temp1451=({ struct _tuple8
_temp1450; _temp1450.f1= t1; _temp1450.f2= t2; _temp1450;}); void* _temp1463;
struct Cyc_Absyn_PtrInfo _temp1465; void* _temp1467; struct Cyc_Absyn_PtrInfo
_temp1469; void* _temp1471; struct Cyc_Absyn_Exp* _temp1473; struct Cyc_Absyn_Tqual
_temp1475; void* _temp1477; void* _temp1479; struct Cyc_Absyn_Exp* _temp1481;
struct Cyc_Absyn_Tqual _temp1483; void* _temp1485; void* _temp1487; struct Cyc_Absyn_TunionInfo
_temp1489; struct Cyc_List_List* _temp1491; void* _temp1493; struct Cyc_Absyn_Tuniondecl*
_temp1495; void* _temp1497; struct Cyc_Absyn_TunionFieldInfo _temp1499; struct
Cyc_List_List* _temp1501; void* _temp1503; struct Cyc_Absyn_Tunionfield*
_temp1505; struct Cyc_Absyn_Tuniondecl* _temp1507; void* _temp1509; struct Cyc_Absyn_TunionInfo
_temp1511; void* _temp1513; struct Cyc_List_List* _temp1515; void* _temp1517;
struct Cyc_Absyn_Tuniondecl* _temp1519; void* _temp1521; struct Cyc_Absyn_PtrInfo
_temp1523; struct Cyc_Absyn_Conref* _temp1525; struct Cyc_Absyn_Tqual _temp1527;
struct Cyc_Absyn_Conref* _temp1529; void* _temp1531; void* _temp1533; _LL1453:
_LL1468: _temp1467= _temp1451.f1; if(( unsigned int) _temp1467 > 4u?*(( int*)
_temp1467) == Cyc_Absyn_PointerType: 0){ _LL1470: _temp1469=(( struct Cyc_Absyn_PointerType_struct*)
_temp1467)->f1; goto _LL1464;} else{ goto _LL1455;} _LL1464: _temp1463=
_temp1451.f2; if(( unsigned int) _temp1463 > 4u?*(( int*) _temp1463) == Cyc_Absyn_PointerType:
0){ _LL1466: _temp1465=(( struct Cyc_Absyn_PointerType_struct*) _temp1463)->f1;
goto _LL1454;} else{ goto _LL1455;} _LL1455: _LL1480: _temp1479= _temp1451.f1;
if(( unsigned int) _temp1479 > 4u?*(( int*) _temp1479) == Cyc_Absyn_ArrayType: 0){
_LL1486: _temp1485=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1479)->f1;
goto _LL1484; _LL1484: _temp1483=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1479)->f2; goto _LL1482; _LL1482: _temp1481=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1479)->f3; goto _LL1472;} else{ goto _LL1457;} _LL1472: _temp1471=
_temp1451.f2; if(( unsigned int) _temp1471 > 4u?*(( int*) _temp1471) == Cyc_Absyn_ArrayType:
0){ _LL1478: _temp1477=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1471)->f1;
goto _LL1476; _LL1476: _temp1475=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1471)->f2; goto _LL1474; _LL1474: _temp1473=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1471)->f3; goto _LL1456;} else{ goto _LL1457;} _LL1457: _LL1498: _temp1497=
_temp1451.f1; if(( unsigned int) _temp1497 > 4u?*(( int*) _temp1497) == Cyc_Absyn_TunionFieldType:
0){ _LL1500: _temp1499=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1497)->f1;
_LL1504: _temp1503=( void*) _temp1499.field_info; if(*(( int*) _temp1503) == Cyc_Absyn_KnownTunionfield){
_LL1508: _temp1507=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1503)->f1;
goto _LL1506; _LL1506: _temp1505=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1503)->f2; goto _LL1502;} else{ goto _LL1459;} _LL1502: _temp1501=
_temp1499.targs; goto _LL1488;} else{ goto _LL1459;} _LL1488: _temp1487=
_temp1451.f2; if(( unsigned int) _temp1487 > 4u?*(( int*) _temp1487) == Cyc_Absyn_TunionType:
0){ _LL1490: _temp1489=(( struct Cyc_Absyn_TunionType_struct*) _temp1487)->f1;
_LL1494: _temp1493=( void*) _temp1489.tunion_info; if(*(( int*) _temp1493) ==
Cyc_Absyn_KnownTunion){ _LL1496: _temp1495=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1493)->f1; goto _LL1492;} else{ goto _LL1459;} _LL1492: _temp1491=
_temp1489.targs; goto _LL1458;} else{ goto _LL1459;} _LL1459: _LL1522: _temp1521=
_temp1451.f1; if(( unsigned int) _temp1521 > 4u?*(( int*) _temp1521) == Cyc_Absyn_PointerType:
0){ _LL1524: _temp1523=(( struct Cyc_Absyn_PointerType_struct*) _temp1521)->f1;
_LL1534: _temp1533=( void*) _temp1523.elt_typ; goto _LL1532; _LL1532: _temp1531=(
void*) _temp1523.rgn_typ; goto _LL1530; _LL1530: _temp1529= _temp1523.nullable;
goto _LL1528; _LL1528: _temp1527= _temp1523.tq; goto _LL1526; _LL1526: _temp1525=
_temp1523.bounds; goto _LL1510;} else{ goto _LL1461;} _LL1510: _temp1509=
_temp1451.f2; if(( unsigned int) _temp1509 > 4u?*(( int*) _temp1509) == Cyc_Absyn_TunionType:
0){ _LL1512: _temp1511=(( struct Cyc_Absyn_TunionType_struct*) _temp1509)->f1;
_LL1518: _temp1517=( void*) _temp1511.tunion_info; if(*(( int*) _temp1517) ==
Cyc_Absyn_KnownTunion){ _LL1520: _temp1519=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1517)->f1; goto _LL1516;} else{ goto _LL1461;} _LL1516: _temp1515=
_temp1511.targs; goto _LL1514; _LL1514: _temp1513=( void*) _temp1511.rgn; goto
_LL1460;} else{ goto _LL1461;} _LL1461: goto _LL1462; _LL1454: { int okay= 1;
if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1469.nullable, _temp1465.nullable)){
void* _temp1535=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp1469.nullable))->v; int _temp1541; _LL1537:
if(( unsigned int) _temp1535 > 1u?*(( int*) _temp1535) == Cyc_Absyn_Eq_constr: 0){
_LL1542: _temp1541=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1535)->f1;
goto _LL1538;} else{ goto _LL1539;} _LL1539: goto _LL1540; _LL1538: okay= !
_temp1541; goto _LL1536; _LL1540:( int) _throw( Cyc_Tcutil_impos( _tag_arr("silent_castable conref not eq",
sizeof( unsigned char), 30u))); _LL1536:;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp,
_temp1469.bounds, _temp1465.bounds)){ struct _tuple8 _temp1544=({ struct _tuple8
_temp1543; _temp1543.f1=( void*)( Cyc_Absyn_compress_conref( _temp1469.bounds))->v;
_temp1543.f2=( void*)( Cyc_Absyn_compress_conref( _temp1465.bounds))->v;
_temp1543;}); void* _temp1554; void* _temp1556; void* _temp1558; void* _temp1560;
void* _temp1562; void* _temp1564; struct Cyc_Absyn_Exp* _temp1566; void*
_temp1568; void* _temp1570; struct Cyc_Absyn_Exp* _temp1572; void* _temp1574;
void* _temp1576; void* _temp1578; void* _temp1580; _LL1546: _LL1559: _temp1558=
_temp1544.f1; if(( unsigned int) _temp1558 > 1u?*(( int*) _temp1558) == Cyc_Absyn_Eq_constr:
0){ _LL1561: _temp1560=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1558)->f1;
if(( unsigned int) _temp1560 > 1u?*(( int*) _temp1560) == Cyc_Absyn_Upper_b: 0){
goto _LL1555;} else{ goto _LL1548;}} else{ goto _LL1548;} _LL1555: _temp1554=
_temp1544.f2; if(( unsigned int) _temp1554 > 1u?*(( int*) _temp1554) == Cyc_Absyn_Eq_constr:
0){ _LL1557: _temp1556=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1554)->f1;
if( _temp1556 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1547;} else{ goto _LL1548;}}
else{ goto _LL1548;} _LL1548: _LL1569: _temp1568= _temp1544.f1; if((
unsigned int) _temp1568 > 1u?*(( int*) _temp1568) == Cyc_Absyn_Eq_constr: 0){
_LL1571: _temp1570=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1568)->f1;
if(( unsigned int) _temp1570 > 1u?*(( int*) _temp1570) == Cyc_Absyn_Upper_b: 0){
_LL1573: _temp1572=(( struct Cyc_Absyn_Upper_b_struct*) _temp1570)->f1; goto
_LL1563;} else{ goto _LL1550;}} else{ goto _LL1550;} _LL1563: _temp1562=
_temp1544.f2; if(( unsigned int) _temp1562 > 1u?*(( int*) _temp1562) == Cyc_Absyn_Eq_constr:
0){ _LL1565: _temp1564=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1562)->f1;
if(( unsigned int) _temp1564 > 1u?*(( int*) _temp1564) == Cyc_Absyn_Upper_b: 0){
_LL1567: _temp1566=(( struct Cyc_Absyn_Upper_b_struct*) _temp1564)->f1; goto
_LL1549;} else{ goto _LL1550;}} else{ goto _LL1550;} _LL1550: _LL1579: _temp1578=
_temp1544.f1; if(( unsigned int) _temp1578 > 1u?*(( int*) _temp1578) == Cyc_Absyn_Eq_constr:
0){ _LL1581: _temp1580=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1578)->f1;
if( _temp1580 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1575;} else{ goto _LL1552;}}
else{ goto _LL1552;} _LL1575: _temp1574= _temp1544.f2; if(( unsigned int)
_temp1574 > 1u?*(( int*) _temp1574) == Cyc_Absyn_Eq_constr: 0){ _LL1577:
_temp1576=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1574)->f1; if(
_temp1576 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1551;} else{ goto _LL1552;}}
else{ goto _LL1552;} _LL1552: goto _LL1553; _LL1547: okay= 1; goto _LL1545;
_LL1549: okay= okay? Cyc_Evexp_eval_const_uint_exp( _temp1572) >= Cyc_Evexp_eval_const_uint_exp(
_temp1566): 0; Cyc_Tcutil_warn( loc, _tag_arr("implicit cast to shorter array",
sizeof( unsigned char), 31u)); goto _LL1545; _LL1551: okay= 1; goto _LL1545;
_LL1553: okay= 0; goto _LL1545; _LL1545:;} okay= okay? Cyc_Tcutil_unify(( void*)
_temp1469.elt_typ,( void*) _temp1465.elt_typ): 0; okay= okay? Cyc_Tcutil_unify((
void*) _temp1469.rgn_typ,( void*) _temp1465.rgn_typ)? 1: Cyc_Tcenv_region_outlives(
te,( void*) _temp1469.rgn_typ,( void*) _temp1465.rgn_typ): 0; okay= okay? !(
_temp1469.tq).q_const? 1:( _temp1465.tq).q_const: 0; return okay;} _LL1456: {
int okay; okay=( _temp1481 != 0? _temp1473 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1481)) == Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1473)): 0; return( okay? Cyc_Tcutil_unify(
_temp1485, _temp1477): 0)? ! _temp1483.q_const? 1: _temp1475.q_const: 0;}
_LL1458: if(( _temp1507 == _temp1495? 1: Cyc_Absyn_qvar_cmp( _temp1507->name,
_temp1495->name) == 0)? _temp1505->typs == 0: 0){ for( 0; _temp1501 != 0?
_temp1491 != 0: 0;( _temp1501=(( struct Cyc_List_List*) _check_null( _temp1501))->tl,
_temp1491=(( struct Cyc_List_List*) _check_null( _temp1491))->tl)){ if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_List_List*) _check_null( _temp1501))->hd,( void*)(( struct
Cyc_List_List*) _check_null( _temp1491))->hd)){ break;}} if( _temp1501 == 0?
_temp1491 == 0: 0){ return 1;}} return 0; _LL1460:{ void* _temp1582= Cyc_Tcutil_compress(
_temp1533); struct Cyc_Absyn_TunionFieldInfo _temp1588; struct Cyc_List_List*
_temp1590; void* _temp1592; struct Cyc_Absyn_Tunionfield* _temp1594; struct Cyc_Absyn_Tuniondecl*
_temp1596; _LL1584: if(( unsigned int) _temp1582 > 4u?*(( int*) _temp1582) ==
Cyc_Absyn_TunionFieldType: 0){ _LL1589: _temp1588=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1582)->f1; _LL1593: _temp1592=( void*) _temp1588.field_info; if(*(( int*)
_temp1592) == Cyc_Absyn_KnownTunionfield){ _LL1597: _temp1596=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1592)->f1; goto _LL1595; _LL1595: _temp1594=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1592)->f2; goto _LL1591;} else{ goto _LL1586;} _LL1591: _temp1590=
_temp1588.targs; goto _LL1585;} else{ goto _LL1586;} _LL1586: goto _LL1587;
_LL1585: if( ! Cyc_Tcutil_unify( _temp1531, _temp1513)? ! Cyc_Tcenv_region_outlives(
te, _temp1531, _temp1513): 0){ return 0;} if( !(( int(*)( int(* cmp)( int, int),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp, _temp1529,(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
0))){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp, _temp1525,
Cyc_Absyn_new_conref( Cyc_Absyn_bounds_one))){ return 0;} if( Cyc_Absyn_qvar_cmp(
_temp1519->name, _temp1596->name) == 0? _temp1594->typs != 0: 0){ int okay= 1;
for( 0; _temp1590 != 0? _temp1515 != 0: 0;( _temp1590=(( struct Cyc_List_List*)
_check_null( _temp1590))->tl, _temp1515=(( struct Cyc_List_List*) _check_null(
_temp1515))->tl)){ if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_List_List*)
_check_null( _temp1590))->hd,( void*)(( struct Cyc_List_List*) _check_null(
_temp1515))->hd)){ okay= 0; break;}} if(( ! okay? 1: _temp1590 != 0)? 1:
_temp1515 != 0){ return 0;} return 1;} goto _LL1583; _LL1587: goto _LL1583;
_LL1583:;} return 0; _LL1462: return Cyc_Tcutil_unify( t1, t2); _LL1452:;}} int
Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void*
t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} if( Cyc_Tcutil_is_arithmetic_type(
t1)? Cyc_Tcutil_is_arithmetic_type( t2): 0){ if( Cyc_Tcutil_will_lose_precision(
t1, t2)){ Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u));} Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;}
else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); Cyc_Tcutil_warn( e->loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp1598)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1603= _tag_arr("implicit cast from %s to %s", sizeof(
unsigned char), 28u); struct Cyc_Stdio_String_pa_struct _temp1605; _temp1605.tag=
Cyc_Stdio_String_pa; _temp1605.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp1601=( void*)& _temp1605; struct Cyc_Stdio_String_pa_struct
_temp1604; _temp1604.tag= Cyc_Stdio_String_pa; _temp1604.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t2);{ void* _temp1602=( void*)& _temp1604; void*
_temp1599[ 2u]={ _temp1601, _temp1602}; struct _tagged_arr _temp1600={( void*)
_temp1599,( void*) _temp1599,( void*)( _temp1599 + 2u)}; _temp1598( _temp1603,
_temp1600);}}})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg(
te, e, t);} static int Cyc_Tcutil_any_promote( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e){ void* old_typ= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); void* t;{ void* _temp1606= old_typ; void* _temp1618;
void* _temp1620; _LL1608: if(( unsigned int) _temp1606 > 4u?*(( int*) _temp1606)
== Cyc_Absyn_EnumType: 0){ goto _LL1609;} else{ goto _LL1610;} _LL1610: if((
unsigned int) _temp1606 > 4u?*(( int*) _temp1606) == Cyc_Absyn_IntType: 0){
_LL1621: _temp1620=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1606)->f1;
goto _LL1619; _LL1619: _temp1618=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1606)->f2; goto _LL1611;} else{ goto _LL1612;} _LL1612: if( _temp1606 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1613;} else{ goto _LL1614;} _LL1614: if(
_temp1606 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1615;} else{ goto _LL1616;}
_LL1616: goto _LL1617; _LL1609: t= Cyc_Absyn_sint_t; goto _LL1607; _LL1611:{
void* _temp1622= _temp1618; _LL1624: if( _temp1622 ==( void*) Cyc_Absyn_B1){
goto _LL1625;} else{ goto _LL1626;} _LL1626: if( _temp1622 ==( void*) Cyc_Absyn_B2){
goto _LL1627;} else{ goto _LL1628;} _LL1628: if( _temp1622 ==( void*) Cyc_Absyn_B4){
goto _LL1629;} else{ goto _LL1630;} _LL1630: if( _temp1622 ==( void*) Cyc_Absyn_B8){
goto _LL1631;} else{ goto _LL1623;} _LL1625: t= Cyc_Absyn_sint_t; goto _LL1623;
_LL1627: t= Cyc_Absyn_sint_t; goto _LL1623; _LL1629: t= _temp1620 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL1623; _LL1631: t= _temp1620 ==( void*)
Cyc_Absyn_Unsigned? Cyc_Absyn_ulonglong_t: Cyc_Absyn_slonglong_t; goto _LL1623;
_LL1623:;} goto _LL1607; _LL1613: t= Cyc_Absyn_float_t; goto _LL1607; _LL1615: t=
Cyc_Absyn_double_t; goto _LL1607; _LL1617: return 0; _LL1607:;} Cyc_Tcutil_unchecked_cast(
te, e, t); return 1;} int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){
return 1;} Cyc_Tcutil_any_promote( te, e); if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, t2)){ return 1;} else{ if( Cyc_Tcutil_silent_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{
if( Cyc_Tcutil_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e,
t2); Cyc_Tcutil_warn( e->loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp1632)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp1637= _tag_arr("implicit cast from %s to %s", sizeof(
unsigned char), 28u); struct Cyc_Stdio_String_pa_struct _temp1639; _temp1639.tag=
Cyc_Stdio_String_pa; _temp1639.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp1635=( void*)& _temp1639; struct Cyc_Stdio_String_pa_struct
_temp1638; _temp1638.tag= Cyc_Stdio_String_pa; _temp1638.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t2);{ void* _temp1636=( void*)& _temp1638; void*
_temp1633[ 2u]={ _temp1635, _temp1636}; struct _tagged_arr _temp1634={( void*)
_temp1633,( void*) _temp1633,( void*)( _temp1633 + 2u)}; _temp1632( _temp1637,
_temp1634);}}})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerceable( void*
t){ void* _temp1640= Cyc_Tcutil_compress( t); _LL1642: if(( unsigned int)
_temp1640 > 4u?*(( int*) _temp1640) == Cyc_Absyn_IntType: 0){ goto _LL1643;}
else{ goto _LL1644;} _LL1644: if( _temp1640 ==( void*) Cyc_Absyn_FloatType){
goto _LL1645;} else{ goto _LL1646;} _LL1646: if( _temp1640 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1647;} else{ goto _LL1648;} _LL1648: goto _LL1649; _LL1643: return 1;
_LL1645: return 1; _LL1647: return 1; _LL1649: return 0; _LL1641:;} static
struct _tuple4* Cyc_Tcutil_flatten_typ_f( struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield*
x){ return({ struct _tuple4* _temp1650=( struct _tuple4*) GC_malloc( sizeof(
struct _tuple4)); _temp1650->f1= x->tq; _temp1650->f2= Cyc_Tcutil_substitute(
inst,( void*) x->type); _temp1650;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ(
struct Cyc_Tcenv_Tenv* te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void*
_temp1651= t1; struct Cyc_List_List* _temp1663; struct Cyc_Absyn_Structdecl**
_temp1665; struct Cyc_List_List* _temp1667; struct _tuple1* _temp1669; struct
Cyc_List_List* _temp1671; _LL1653: if( _temp1651 ==( void*) Cyc_Absyn_VoidType){
goto _LL1654;} else{ goto _LL1655;} _LL1655: if(( unsigned int) _temp1651 > 4u?*((
int*) _temp1651) == Cyc_Absyn_TupleType: 0){ _LL1664: _temp1663=(( struct Cyc_Absyn_TupleType_struct*)
_temp1651)->f1; goto _LL1656;} else{ goto _LL1657;} _LL1657: if(( unsigned int)
_temp1651 > 4u?*(( int*) _temp1651) == Cyc_Absyn_StructType: 0){ _LL1670:
_temp1669=(( struct Cyc_Absyn_StructType_struct*) _temp1651)->f1; goto _LL1668;
_LL1668: _temp1667=(( struct Cyc_Absyn_StructType_struct*) _temp1651)->f2; goto
_LL1666; _LL1666: _temp1665=(( struct Cyc_Absyn_StructType_struct*) _temp1651)->f3;
goto _LL1658;} else{ goto _LL1659;} _LL1659: if(( unsigned int) _temp1651 > 4u?*((
int*) _temp1651) == Cyc_Absyn_AnonStructType: 0){ _LL1672: _temp1671=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1651)->f1; goto _LL1660;} else{ goto
_LL1661;} _LL1661: goto _LL1662; _LL1654: return 0; _LL1656: return _temp1663;
_LL1658: if( _temp1665 == 0? 1: _temp1669 == 0){ return({ struct Cyc_List_List*
_temp1673=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1673->hd=( void*)({ struct _tuple4* _temp1674=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1674->f1= Cyc_Absyn_empty_tqual(); _temp1674->f2=
t1; _temp1674;}); _temp1673->tl= 0; _temp1673;});}{ struct Cyc_Absyn_Structdecl*
_temp1675=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp1665)); struct
_tuple1 _temp1676=*(( struct _tuple1*) _check_null( _temp1669)); struct Cyc_List_List*
_temp1677=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp1675->tvs, _temp1667); if( _temp1675->fields == 0){
return({ struct Cyc_List_List* _temp1678=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1678->hd=( void*)({ struct _tuple4*
_temp1679=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1679->f1=
Cyc_Absyn_empty_tqual(); _temp1679->f2= t1; _temp1679;}); _temp1678->tl= 0;
_temp1678;});} return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, _temp1677,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1675->fields))->v);}
_LL1660: return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, 0, _temp1671); _LL1662: return({
struct Cyc_List_List* _temp1680=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1680->hd=( void*)({ struct _tuple4* _temp1681=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1681->f1= Cyc_Absyn_empty_tqual();
_temp1681->f2= t1; _temp1681;}); _temp1680->tl= 0; _temp1680;}); _LL1652:;}}
static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2); static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;}{ struct Cyc_List_List* a= assume; for( 0; a != 0; a=(( struct
Cyc_List_List*) _check_null( a))->tl){ if( Cyc_Tcutil_unify( t1,(*(( struct
_tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f1)? Cyc_Tcutil_unify(
t2,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f2): 0){
return 1;}}} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct
_tuple8 _temp1683=({ struct _tuple8 _temp1682; _temp1682.f1= t1; _temp1682.f2=
t2; _temp1682;}); void* _temp1689; struct Cyc_Absyn_PtrInfo _temp1691; struct
Cyc_Absyn_Conref* _temp1693; struct Cyc_Absyn_Tqual _temp1695; struct Cyc_Absyn_Conref*
_temp1697; void* _temp1699; void* _temp1701; void* _temp1703; struct Cyc_Absyn_PtrInfo
_temp1705; struct Cyc_Absyn_Conref* _temp1707; struct Cyc_Absyn_Tqual _temp1709;
struct Cyc_Absyn_Conref* _temp1711; void* _temp1713; void* _temp1715; _LL1685:
_LL1704: _temp1703= _temp1683.f1; if(( unsigned int) _temp1703 > 4u?*(( int*)
_temp1703) == Cyc_Absyn_PointerType: 0){ _LL1706: _temp1705=(( struct Cyc_Absyn_PointerType_struct*)
_temp1703)->f1; _LL1716: _temp1715=( void*) _temp1705.elt_typ; goto _LL1714;
_LL1714: _temp1713=( void*) _temp1705.rgn_typ; goto _LL1712; _LL1712: _temp1711=
_temp1705.nullable; goto _LL1710; _LL1710: _temp1709= _temp1705.tq; goto _LL1708;
_LL1708: _temp1707= _temp1705.bounds; goto _LL1690;} else{ goto _LL1687;}
_LL1690: _temp1689= _temp1683.f2; if(( unsigned int) _temp1689 > 4u?*(( int*)
_temp1689) == Cyc_Absyn_PointerType: 0){ _LL1692: _temp1691=(( struct Cyc_Absyn_PointerType_struct*)
_temp1689)->f1; _LL1702: _temp1701=( void*) _temp1691.elt_typ; goto _LL1700;
_LL1700: _temp1699=( void*) _temp1691.rgn_typ; goto _LL1698; _LL1698: _temp1697=
_temp1691.nullable; goto _LL1696; _LL1696: _temp1695= _temp1691.tq; goto _LL1694;
_LL1694: _temp1693= _temp1691.bounds; goto _LL1686;} else{ goto _LL1687;}
_LL1687: goto _LL1688; _LL1686: if( _temp1709.q_const? ! _temp1695.q_const: 0){
return 0;} if(( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1711, _temp1697)?(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1711): 0)? !(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1697): 0){ return 0;} if( ! Cyc_Tcutil_unify( _temp1713, _temp1699)? ! Cyc_Tcenv_region_outlives(
te, _temp1713, _temp1699): 0){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp,
_temp1707, _temp1693)){ struct _tuple8 _temp1718=({ struct _tuple8 _temp1717;
_temp1717.f1= Cyc_Absyn_conref_val( _temp1707); _temp1717.f2= Cyc_Absyn_conref_val(
_temp1693); _temp1717;}); void* _temp1726; void* _temp1728; void* _temp1730;
struct Cyc_Absyn_Exp* _temp1732; void* _temp1734; struct Cyc_Absyn_Exp*
_temp1736; _LL1720: _LL1729: _temp1728= _temp1718.f1; if(( unsigned int)
_temp1728 > 1u?*(( int*) _temp1728) == Cyc_Absyn_Upper_b: 0){ goto _LL1727;}
else{ goto _LL1722;} _LL1727: _temp1726= _temp1718.f2; if( _temp1726 ==( void*)
Cyc_Absyn_Unknown_b){ goto _LL1721;} else{ goto _LL1722;} _LL1722: _LL1735:
_temp1734= _temp1718.f1; if(( unsigned int) _temp1734 > 1u?*(( int*) _temp1734)
== Cyc_Absyn_Upper_b: 0){ _LL1737: _temp1736=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1734)->f1; goto _LL1731;} else{ goto _LL1724;} _LL1731: _temp1730=
_temp1718.f2; if(( unsigned int) _temp1730 > 1u?*(( int*) _temp1730) == Cyc_Absyn_Upper_b:
0){ _LL1733: _temp1732=(( struct Cyc_Absyn_Upper_b_struct*) _temp1730)->f1; goto
_LL1723;} else{ goto _LL1724;} _LL1724: goto _LL1725; _LL1721: goto _LL1719;
_LL1723: if( Cyc_Evexp_eval_const_uint_exp( _temp1736) < Cyc_Evexp_eval_const_uint_exp(
_temp1732)){ return 0;} goto _LL1719; _LL1725: return 0; _LL1719:;} return Cyc_Tcutil_ptrsubtype(
te,({ struct Cyc_List_List* _temp1738=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1738->hd=( void*)({ struct _tuple8*
_temp1739=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp1739->f1=
t1; _temp1739->f2= t2; _temp1739;}); _temp1738->tl= assume; _temp1738;}),
_temp1715, _temp1701); _LL1688: return 0; _LL1684:;}} static int Cyc_Tcutil_isomorphic(
void* t1, void* t2){ struct _tuple8 _temp1741=({ struct _tuple8 _temp1740;
_temp1740.f1= Cyc_Tcutil_compress( t1); _temp1740.f2= Cyc_Tcutil_compress( t2);
_temp1740;}); void* _temp1747; void* _temp1749; void* _temp1751; void* _temp1753;
_LL1743: _LL1752: _temp1751= _temp1741.f1; if(( unsigned int) _temp1751 > 4u?*((
int*) _temp1751) == Cyc_Absyn_IntType: 0){ _LL1754: _temp1753=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1751)->f2; goto _LL1748;} else{ goto _LL1745;}
_LL1748: _temp1747= _temp1741.f2; if(( unsigned int) _temp1747 > 4u?*(( int*)
_temp1747) == Cyc_Absyn_IntType: 0){ _LL1750: _temp1749=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1747)->f2; goto _LL1744;} else{ goto _LL1745;} _LL1745: goto _LL1746;
_LL1744: return _temp1753 == _temp1749; _LL1746: return 0; _LL1742:;} static int
Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* assume,
void* t1, void* t2){ struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ( te, t1);
struct Cyc_List_List* tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0; tqs2 != 0;(
tqs2=(( struct Cyc_List_List*) _check_null( tqs2))->tl, tqs1=(( struct Cyc_List_List*)
_check_null( tqs1))->tl)){ if( tqs1 == 0){ return 0;}{ struct _tuple4 _temp1757;
void* _temp1758; struct Cyc_Absyn_Tqual _temp1760; struct _tuple4* _temp1755=(
struct _tuple4*)(( struct Cyc_List_List*) _check_null( tqs1))->hd; _temp1757=*
_temp1755; _LL1761: _temp1760= _temp1757.f1; goto _LL1759; _LL1759: _temp1758=
_temp1757.f2; goto _LL1756; _LL1756: { struct _tuple4 _temp1764; void* _temp1765;
struct Cyc_Absyn_Tqual _temp1767; struct _tuple4* _temp1762=( struct _tuple4*)((
struct Cyc_List_List*) _check_null( tqs2))->hd; _temp1764=* _temp1762; _LL1768:
_temp1767= _temp1764.f1; goto _LL1766; _LL1766: _temp1765= _temp1764.f2; goto
_LL1763; _LL1763: if( _temp1767.q_const? Cyc_Tcutil_subtype( te, assume,
_temp1758, _temp1765): 0){ continue;} else{ if( Cyc_Tcutil_unify( _temp1758,
_temp1765)){ continue;} else{ if( Cyc_Tcutil_isomorphic( _temp1758, _temp1765)){
continue;} else{ return 0;}}}}}} return 1;} int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void* t1, void* t2){ if( Cyc_Tcutil_unify(
t1, t2)){ return 1;} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
void* _temp1769= t2; struct Cyc_Absyn_Uniondecl** _temp1777; struct Cyc_List_List*
_temp1779; void* _temp1781; _LL1771: if(( unsigned int) _temp1769 > 4u?*(( int*)
_temp1769) == Cyc_Absyn_UnionType: 0){ _LL1780: _temp1779=(( struct Cyc_Absyn_UnionType_struct*)
_temp1769)->f2; goto _LL1778; _LL1778: _temp1777=(( struct Cyc_Absyn_UnionType_struct*)
_temp1769)->f3; goto _LL1772;} else{ goto _LL1773;} _LL1773: if(( unsigned int)
_temp1769 > 4u?*(( int*) _temp1769) == Cyc_Absyn_IntType: 0){ _LL1782: _temp1781=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1769)->f2; if( _temp1781 ==(
void*) Cyc_Absyn_B4){ goto _LL1774;} else{ goto _LL1775;}} else{ goto _LL1775;}
_LL1775: goto _LL1776; _LL1772: if( _temp1777 == 0){ goto _LL1770;}{ struct Cyc_Absyn_Uniondecl*
_temp1783=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp1777)); if(
_temp1783->fields == 0){ goto _LL1770;}{ struct Cyc_List_List* _temp1784=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1783->tvs, _temp1779);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1783->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ if( Cyc_Tcutil_unify( t1, Cyc_Tcutil_substitute(
_temp1784,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ return 1;}}} goto _LL1770;}} _LL1774: if( Cyc_Tcutil_typ_kind(
t2) ==( void*) Cyc_Absyn_BoxKind){ return 1;} goto _LL1770; _LL1776: goto
_LL1770; _LL1770:;}{ void* _temp1785= t1; struct Cyc_Absyn_PtrInfo _temp1801;
struct Cyc_Absyn_Conref* _temp1803; struct Cyc_Absyn_Tqual _temp1805; struct Cyc_Absyn_Conref*
_temp1807; void* _temp1809; void* _temp1811; struct Cyc_Absyn_Exp* _temp1813;
struct Cyc_Absyn_Tqual _temp1815; void* _temp1817; struct Cyc_Absyn_Enumdecl*
_temp1819; _LL1787: if(( unsigned int) _temp1785 > 4u?*(( int*) _temp1785) ==
Cyc_Absyn_PointerType: 0){ _LL1802: _temp1801=(( struct Cyc_Absyn_PointerType_struct*)
_temp1785)->f1; _LL1812: _temp1811=( void*) _temp1801.elt_typ; goto _LL1810;
_LL1810: _temp1809=( void*) _temp1801.rgn_typ; goto _LL1808; _LL1808: _temp1807=
_temp1801.nullable; goto _LL1806; _LL1806: _temp1805= _temp1801.tq; goto _LL1804;
_LL1804: _temp1803= _temp1801.bounds; goto _LL1788;} else{ goto _LL1789;}
_LL1789: if(( unsigned int) _temp1785 > 4u?*(( int*) _temp1785) == Cyc_Absyn_ArrayType:
0){ _LL1818: _temp1817=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1785)->f1;
goto _LL1816; _LL1816: _temp1815=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1785)->f2; goto _LL1814; _LL1814: _temp1813=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1785)->f3; goto _LL1790;} else{ goto _LL1791;} _LL1791: if(( unsigned int)
_temp1785 > 4u?*(( int*) _temp1785) == Cyc_Absyn_EnumType: 0){ _LL1820:
_temp1819=(( struct Cyc_Absyn_EnumType_struct*) _temp1785)->f2; goto _LL1792;}
else{ goto _LL1793;} _LL1793: if(( unsigned int) _temp1785 > 4u?*(( int*)
_temp1785) == Cyc_Absyn_IntType: 0){ goto _LL1794;} else{ goto _LL1795;} _LL1795:
if( _temp1785 ==( void*) Cyc_Absyn_FloatType){ goto _LL1796;} else{ goto _LL1797;}
_LL1797: if( _temp1785 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1798;} else{
goto _LL1799;} _LL1799: goto _LL1800; _LL1788:{ void* _temp1821= t2; struct Cyc_Absyn_PtrInfo
_temp1827; struct Cyc_Absyn_Conref* _temp1829; struct Cyc_Absyn_Tqual _temp1831;
struct Cyc_Absyn_Conref* _temp1833; void* _temp1835; void* _temp1837; _LL1823:
if(( unsigned int) _temp1821 > 4u?*(( int*) _temp1821) == Cyc_Absyn_PointerType:
0){ _LL1828: _temp1827=(( struct Cyc_Absyn_PointerType_struct*) _temp1821)->f1;
_LL1838: _temp1837=( void*) _temp1827.elt_typ; goto _LL1836; _LL1836: _temp1835=(
void*) _temp1827.rgn_typ; goto _LL1834; _LL1834: _temp1833= _temp1827.nullable;
goto _LL1832; _LL1832: _temp1831= _temp1827.tq; goto _LL1830; _LL1830: _temp1829=
_temp1827.bounds; goto _LL1824;} else{ goto _LL1825;} _LL1825: goto _LL1826;
_LL1824: { struct Cyc_List_List* _temp1839=({ struct Cyc_List_List* _temp1856=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1856->hd=(
void*)({ struct _tuple8* _temp1857=( struct _tuple8*) GC_malloc( sizeof( struct
_tuple8)); _temp1857->f1= t1; _temp1857->f2= t2; _temp1857;}); _temp1856->tl= 0;
_temp1856;}); int _temp1840= Cyc_Tcutil_ptrsubtype( te, _temp1839, _temp1811,
_temp1837)? ! _temp1805.q_const? 1: _temp1831.q_const: 0; int bounds_ok= Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_boundscmp, _temp1803, _temp1829); if( ! bounds_ok){ struct _tuple8
_temp1842=({ struct _tuple8 _temp1841; _temp1841.f1= Cyc_Absyn_conref_val(
_temp1803); _temp1841.f2= Cyc_Absyn_conref_val( _temp1829); _temp1841;}); void*
_temp1848; struct Cyc_Absyn_Exp* _temp1850; void* _temp1852; struct Cyc_Absyn_Exp*
_temp1854; _LL1844: _LL1853: _temp1852= _temp1842.f1; if(( unsigned int)
_temp1852 > 1u?*(( int*) _temp1852) == Cyc_Absyn_Upper_b: 0){ _LL1855: _temp1854=((
struct Cyc_Absyn_Upper_b_struct*) _temp1852)->f1; goto _LL1849;} else{ goto
_LL1846;} _LL1849: _temp1848= _temp1842.f2; if(( unsigned int) _temp1848 > 1u?*((
int*) _temp1848) == Cyc_Absyn_Upper_b: 0){ _LL1851: _temp1850=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1848)->f1; goto _LL1845;} else{ goto _LL1846;} _LL1846: goto _LL1847;
_LL1845: if( Cyc_Evexp_eval_const_uint_exp( _temp1854) >= Cyc_Evexp_eval_const_uint_exp(
_temp1850)){ bounds_ok= 1;} goto _LL1843; _LL1847: bounds_ok= 1; goto _LL1843;
_LL1843:;} return( bounds_ok? _temp1840: 0)? Cyc_Tcutil_unify( _temp1809,
_temp1835)? 1: Cyc_Tcenv_region_outlives( te, _temp1809, _temp1835): 0;} _LL1826:
goto _LL1822; _LL1822:;} return 0; _LL1790:{ void* _temp1858= t2; struct Cyc_Absyn_Exp*
_temp1864; struct Cyc_Absyn_Tqual _temp1866; void* _temp1868; _LL1860: if((
unsigned int) _temp1858 > 4u?*(( int*) _temp1858) == Cyc_Absyn_ArrayType: 0){
_LL1869: _temp1868=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1858)->f1;
goto _LL1867; _LL1867: _temp1866=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1858)->f2; goto _LL1865; _LL1865: _temp1864=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1858)->f3; goto _LL1861;} else{ goto _LL1862;} _LL1862: goto _LL1863;
_LL1861: { int okay; okay=( _temp1813 != 0? _temp1864 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1813)) >= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1864)): 0; return( okay? Cyc_Tcutil_unify(
_temp1817, _temp1868): 0)? ! _temp1815.q_const? 1: _temp1866.q_const: 0;}
_LL1863: return 0; _LL1859:;} return 0; _LL1792:{ void* _temp1870= t2; struct
Cyc_Absyn_Enumdecl* _temp1876; _LL1872: if(( unsigned int) _temp1870 > 4u?*((
int*) _temp1870) == Cyc_Absyn_EnumType: 0){ _LL1877: _temp1876=(( struct Cyc_Absyn_EnumType_struct*)
_temp1870)->f2; goto _LL1873;} else{ goto _LL1874;} _LL1874: goto _LL1875;
_LL1873: if(((( struct Cyc_Absyn_Enumdecl*) _check_null( _temp1819))->fields !=
0?(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp1876))->fields != 0: 0)?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Enumdecl*) _check_null(
_temp1819))->fields))->v) >=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Enumdecl*)
_check_null( _temp1876))->fields))->v): 0){ return 1;} goto _LL1871; _LL1875:
goto _LL1871; _LL1871:;} goto _LL1794; _LL1794: goto _LL1796; _LL1796: goto
_LL1798; _LL1798: return Cyc_Tcutil_coerceable( t2); _LL1800: return 0; _LL1786:;}}
void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t){ if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, t)){ struct Cyc_Absyn_Exp* _temp1878= Cyc_Absyn_copy_exp( e);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct* _temp1879=(
struct Cyc_Absyn_Cast_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp1879[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp1880; _temp1880.tag= Cyc_Absyn_Cast_e;
_temp1880.f1=( void*) t; _temp1880.f2= _temp1878; _temp1880;}); _temp1879;})));
e->topt=({ struct Cyc_Core_Opt* _temp1881=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1881->v=( void*) t; _temp1881;});}} int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp* e){ void* _temp1882= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v); _LL1884: if(( unsigned int) _temp1882
> 4u?*(( int*) _temp1882) == Cyc_Absyn_EnumType: 0){ goto _LL1885;} else{ goto
_LL1886;} _LL1886: if(( unsigned int) _temp1882 > 4u?*(( int*) _temp1882) == Cyc_Absyn_IntType:
0){ goto _LL1887;} else{ goto _LL1888;} _LL1888: if(( unsigned int) _temp1882 >
4u?*(( int*) _temp1882) == Cyc_Absyn_Evar: 0){ goto _LL1889;} else{ goto _LL1890;}
_LL1890: goto _LL1891; _LL1885: goto _LL1887; _LL1887: return 1; _LL1889: if(
Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){
return 1;} return 0; _LL1891: return 0; _LL1883:;} int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_is_integral( e)){ return 1;}{ void*
_temp1892= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_LL1894: if( _temp1892 ==( void*) Cyc_Absyn_FloatType){ goto _LL1895;} else{
goto _LL1896;} _LL1896: if( _temp1892 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL1897;} else{ goto _LL1898;} _LL1898: goto _LL1899; _LL1895: goto _LL1897;
_LL1897: return 1; _LL1899: return 0; _LL1893:;}} int Cyc_Tcutil_is_function_type(
void* t){ void* _temp1900= Cyc_Tcutil_compress( t); _LL1902: if(( unsigned int)
_temp1900 > 4u?*(( int*) _temp1900) == Cyc_Absyn_FnType: 0){ goto _LL1903;}
else{ goto _LL1904;} _LL1904: goto _LL1905; _LL1903: return 1; _LL1905: return 0;
_LL1901:;} void* Cyc_Tcutil_max_arithmetic_type( void* t1, void* t2){ struct
_tuple8 _temp1907=({ struct _tuple8 _temp1906; _temp1906.f1= t1; _temp1906.f2=
t2; _temp1906;}); void* _temp1931; void* _temp1933; void* _temp1935; void*
_temp1937; void* _temp1939; void* _temp1941; void* _temp1943; void* _temp1945;
void* _temp1947; void* _temp1949; void* _temp1951; void* _temp1953; void*
_temp1955; void* _temp1957; void* _temp1959; void* _temp1961; void* _temp1963;
void* _temp1965; void* _temp1967; void* _temp1969; void* _temp1971; void*
_temp1973; _LL1909: _LL1932: _temp1931= _temp1907.f1; if( _temp1931 ==( void*)
Cyc_Absyn_DoubleType){ goto _LL1910;} else{ goto _LL1911;} _LL1911: _LL1934:
_temp1933= _temp1907.f2; if( _temp1933 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL1912;} else{ goto _LL1913;} _LL1913: _LL1936: _temp1935= _temp1907.f1; if(
_temp1935 ==( void*) Cyc_Absyn_FloatType){ goto _LL1914;} else{ goto _LL1915;}
_LL1915: _LL1938: _temp1937= _temp1907.f2; if( _temp1937 ==( void*) Cyc_Absyn_FloatType){
goto _LL1916;} else{ goto _LL1917;} _LL1917: _LL1940: _temp1939= _temp1907.f1;
if(( unsigned int) _temp1939 > 4u?*(( int*) _temp1939) == Cyc_Absyn_IntType: 0){
_LL1944: _temp1943=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1939)->f1;
if( _temp1943 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1942;} else{ goto _LL1919;}
_LL1942: _temp1941=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1939)->f2;
if( _temp1941 ==( void*) Cyc_Absyn_B8){ goto _LL1918;} else{ goto _LL1919;}}
else{ goto _LL1919;} _LL1919: _LL1946: _temp1945= _temp1907.f2; if((
unsigned int) _temp1945 > 4u?*(( int*) _temp1945) == Cyc_Absyn_IntType: 0){
_LL1950: _temp1949=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1945)->f1;
if( _temp1949 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1948;} else{ goto _LL1921;}
_LL1948: _temp1947=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1945)->f2;
if( _temp1947 ==( void*) Cyc_Absyn_B8){ goto _LL1920;} else{ goto _LL1921;}}
else{ goto _LL1921;} _LL1921: _LL1952: _temp1951= _temp1907.f1; if((
unsigned int) _temp1951 > 4u?*(( int*) _temp1951) == Cyc_Absyn_IntType: 0){
_LL1956: _temp1955=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1951)->f1;
if( _temp1955 ==( void*) Cyc_Absyn_Signed){ goto _LL1954;} else{ goto _LL1923;}
_LL1954: _temp1953=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1951)->f2;
if( _temp1953 ==( void*) Cyc_Absyn_B8){ goto _LL1922;} else{ goto _LL1923;}}
else{ goto _LL1923;} _LL1923: _LL1958: _temp1957= _temp1907.f2; if((
unsigned int) _temp1957 > 4u?*(( int*) _temp1957) == Cyc_Absyn_IntType: 0){
_LL1962: _temp1961=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1957)->f1;
if( _temp1961 ==( void*) Cyc_Absyn_Signed){ goto _LL1960;} else{ goto _LL1925;}
_LL1960: _temp1959=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1957)->f2;
if( _temp1959 ==( void*) Cyc_Absyn_B8){ goto _LL1924;} else{ goto _LL1925;}}
else{ goto _LL1925;} _LL1925: _LL1964: _temp1963= _temp1907.f1; if((
unsigned int) _temp1963 > 4u?*(( int*) _temp1963) == Cyc_Absyn_IntType: 0){
_LL1968: _temp1967=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1963)->f1;
if( _temp1967 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1966;} else{ goto _LL1927;}
_LL1966: _temp1965=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1963)->f2;
if( _temp1965 ==( void*) Cyc_Absyn_B4){ goto _LL1926;} else{ goto _LL1927;}}
else{ goto _LL1927;} _LL1927: _LL1970: _temp1969= _temp1907.f2; if((
unsigned int) _temp1969 > 4u?*(( int*) _temp1969) == Cyc_Absyn_IntType: 0){
_LL1974: _temp1973=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1969)->f1;
if( _temp1973 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1972;} else{ goto _LL1929;}
_LL1972: _temp1971=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1969)->f2;
if( _temp1971 ==( void*) Cyc_Absyn_B4){ goto _LL1928;} else{ goto _LL1929;}}
else{ goto _LL1929;} _LL1929: goto _LL1930; _LL1910: goto _LL1912; _LL1912:
return( void*) Cyc_Absyn_DoubleType; _LL1914: goto _LL1916; _LL1916: return(
void*) Cyc_Absyn_FloatType; _LL1918: goto _LL1920; _LL1920: return Cyc_Absyn_ulonglong_t;
_LL1922: goto _LL1924; _LL1924: return Cyc_Absyn_slonglong_t; _LL1926: goto
_LL1928; _LL1928: return Cyc_Absyn_uint_t; _LL1930: return Cyc_Absyn_sint_t;
_LL1908:;} void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp* e){ void*
_temp1975=( void*) e->r; struct Cyc_Core_Opt* _temp1981; _LL1977: if(*(( int*)
_temp1975) == Cyc_Absyn_AssignOp_e){ _LL1982: _temp1981=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1975)->f2; if( _temp1981 == 0){ goto _LL1978;} else{ goto _LL1979;}} else{
goto _LL1979;} _LL1979: goto _LL1980; _LL1978: Cyc_Tcutil_warn( e->loc, _tag_arr("assignment in test",
sizeof( unsigned char), 19u)); goto _LL1976; _LL1980: goto _LL1976; _LL1976:;}
static int Cyc_Tcutil_constrain_kinds( struct Cyc_Absyn_Conref* c1, struct Cyc_Absyn_Conref*
c2){ c1= Cyc_Absyn_compress_conref( c1); c2= Cyc_Absyn_compress_conref( c2); if(
c1 == c2){ return 1;} if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr){( void*)(
c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp1983=(
struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct));
_temp1983[ 0]=({ struct Cyc_Absyn_Forward_constr_struct _temp1984; _temp1984.tag=
Cyc_Absyn_Forward_constr; _temp1984.f1= c2; _temp1984;}); _temp1983;}))); return
1;} else{ if(( void*) c2->v ==( void*) Cyc_Absyn_No_constr){( void*)( c2->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp1985=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp1985[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp1986; _temp1986.tag= Cyc_Absyn_Forward_constr;
_temp1986.f1= c1; _temp1986;}); _temp1985;}))); return 1;} else{ void* k1= Cyc_Absyn_conref_val(
c1); void* k2= Cyc_Absyn_conref_val( c2); if( Cyc_Tcutil_kind_leq( k1, k2)){(
void*)( c2->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1987=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1987[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp1988; _temp1988.tag= Cyc_Absyn_Forward_constr; _temp1988.f1= c1; _temp1988;});
_temp1987;}))); return 1;} else{ if( Cyc_Tcutil_kind_leq( k2, k1)){( void*)( c1->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp1989=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp1989[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp1990; _temp1990.tag= Cyc_Absyn_Forward_constr;
_temp1990.f1= c2; _temp1990;}); _temp1989;}))); return 1;} else{ return 0;}}}}}
static int Cyc_Tcutil_tvar_id_counter= 0; int* Cyc_Tcutil_new_tvar_id(){ return({
int* _temp1991=( int*) GC_malloc_atomic( sizeof( int)); _temp1991[ 0]= Cyc_Tcutil_tvar_id_counter
++; _temp1991;});} static int Cyc_Tcutil_tvar_counter= 0; struct Cyc_Absyn_Tvar*
Cyc_Tcutil_new_tvar( void* k){ int i= Cyc_Tcutil_tvar_counter ++; struct
_tagged_arr s=( struct _tagged_arr)({ struct _tagged_arr(* _temp1994)( struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr
_temp1998= _tag_arr("#%d", sizeof( unsigned char), 4u); struct Cyc_Stdio_Int_pa_struct
_temp1999; _temp1999.tag= Cyc_Stdio_Int_pa; _temp1999.f1=( int)(( unsigned int)
i);{ void* _temp1997=( void*)& _temp1999; void* _temp1995[ 1u]={ _temp1997};
struct _tagged_arr _temp1996={( void*) _temp1995,( void*) _temp1995,( void*)(
_temp1995 + 1u)}; _temp1994( _temp1998, _temp1996);}}); return({ struct Cyc_Absyn_Tvar*
_temp1992=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp1992->name=({ struct _tagged_arr* _temp1993=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr) * 1); _temp1993[ 0]= s; _temp1993;});
_temp1992->identity= 0; _temp1992->kind= Cyc_Absyn_new_conref( k); _temp1992;});}
int Cyc_Tcutil_is_temp_tvar( struct Cyc_Absyn_Tvar* t){ struct _tagged_arr
_temp2000=* t->name; return*(( const unsigned char*) _check_unknown_subscript(
_temp2000, sizeof( unsigned char), 0)) =='#';} void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar* t){ if( ! Cyc_Tcutil_is_temp_tvar( t)){ return;}{ struct
_tagged_arr _temp2001= Cyc_String_strconcat( _tag_arr("`", sizeof( unsigned char),
2u),* t->name);*(( unsigned char*) _check_unknown_subscript( _temp2001, sizeof(
unsigned char), 1))='t'; t->name=({ struct _tagged_arr* _temp2002=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1); _temp2002[ 0]=(
struct _tagged_arr) _temp2001; _temp2002;});}} struct _tuple13{ struct
_tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; static struct _tuple2*
Cyc_Tcutil_fndecl2typ_f( struct _tuple13* x){ return({ struct _tuple2* _temp2003=(
struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp2003->f1=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp2004=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2004->v=( void*)(* x).f1; _temp2004;}); _temp2003->f2=(* x).f2;
_temp2003->f3=(* x).f3; _temp2003;});} void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*
fd){ if( fd->cached_typ == 0){ struct Cyc_List_List* _temp2005= 0;{ struct Cyc_List_List*
atts= fd->attributes; for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ if( Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd)){ _temp2005=({ struct Cyc_List_List* _temp2006=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2006->hd=( void*)((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd); _temp2006->tl=
_temp2005; _temp2006;});}}} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2007=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2007[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2008; _temp2008.tag= Cyc_Absyn_FnType;
_temp2008.f1=({ struct Cyc_Absyn_FnInfo _temp2009; _temp2009.tvars= fd->tvs;
_temp2009.effect= fd->effect; _temp2009.ret_typ=( void*)(( void*) fd->ret_type);
_temp2009.args=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct
_tuple13*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_fndecl2typ_f, fd->args);
_temp2009.c_varargs= fd->c_varargs; _temp2009.cyc_varargs= fd->cyc_varargs;
_temp2009.rgn_po= fd->rgn_po; _temp2009.attributes= _temp2005; _temp2009;});
_temp2008;}); _temp2007;});} return( void*)(( struct Cyc_Core_Opt*) _check_null(
fd->cached_typ))->v;} struct _tuple14{ void* f1; struct Cyc_Absyn_Tqual f2; void*
f3; } ; static void* Cyc_Tcutil_fst_fdarg( struct _tuple14* t){ return(* t).f1;}
void* Cyc_Tcutil_snd_tqt( struct _tuple4* t){ return(* t).f2;} static struct
_tuple4* Cyc_Tcutil_map2_tq( struct _tuple4* pr, void* t){ return({ struct
_tuple4* _temp2010=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp2010->f1=(* pr).f1; _temp2010->f2= t; _temp2010;});} void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl*
fd){ void* t= Cyc_Tcutil_fndecl2typ( fd); Cyc_Tcutil_check_valid_toplevel_type(
loc, te, t);{ void* _temp2011= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo
_temp2017; struct Cyc_List_List* _temp2019; struct Cyc_List_List* _temp2021;
struct Cyc_Absyn_VarargInfo* _temp2023; int _temp2025; struct Cyc_List_List*
_temp2027; void* _temp2029; struct Cyc_Core_Opt* _temp2031; struct Cyc_List_List*
_temp2033; _LL2013: if(( unsigned int) _temp2011 > 4u?*(( int*) _temp2011) ==
Cyc_Absyn_FnType: 0){ _LL2018: _temp2017=(( struct Cyc_Absyn_FnType_struct*)
_temp2011)->f1; _LL2034: _temp2033= _temp2017.tvars; goto _LL2032; _LL2032:
_temp2031= _temp2017.effect; goto _LL2030; _LL2030: _temp2029=( void*) _temp2017.ret_typ;
goto _LL2028; _LL2028: _temp2027= _temp2017.args; goto _LL2026; _LL2026:
_temp2025= _temp2017.c_varargs; goto _LL2024; _LL2024: _temp2023= _temp2017.cyc_varargs;
goto _LL2022; _LL2022: _temp2021= _temp2017.rgn_po; goto _LL2020; _LL2020:
_temp2019= _temp2017.attributes; goto _LL2014;} else{ goto _LL2015;} _LL2015:
goto _LL2016; _LL2014: fd->tvs= _temp2033; fd->effect= _temp2031; goto _LL2012;
_LL2016:(( int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)( _tag_arr("check_fndecl_valid_type: not a FnType",
sizeof( unsigned char), 38u)); return; _LL2012:;}{ struct _RegionHandle
_temp2035= _new_region(); struct _RegionHandle* r=& _temp2035; _push_region( r);
Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tagged_arr*(* f)( struct _tuple13*), struct Cyc_List_List* x)) Cyc_List_rmap)(
r,( struct _tagged_arr*(*)( struct _tuple13* t)) Cyc_Tcutil_fst_fdarg, fd->args),
loc, _tag_arr("function declaration has repeated parameter", sizeof(
unsigned char), 44u));; _pop_region( r);} fd->cached_typ=({ struct Cyc_Core_Opt*
_temp2036=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2036->v=( void*) t; _temp2036;});} struct _tuple15{ struct Cyc_Core_Opt* f1;
struct Cyc_Absyn_Tqual f2; } ; struct _tuple16{ struct _tuple15* f1; void* f2; }
; static struct _tuple16* Cyc_Tcutil_substitute_f1( struct _RegionHandle* rgn,
struct _tuple2* y){ return({ struct _tuple16* _temp2037=( struct _tuple16*)
_region_malloc( rgn, sizeof( struct _tuple16)); _temp2037->f1=({ struct _tuple15*
_temp2038=( struct _tuple15*) _region_malloc( rgn, sizeof( struct _tuple15));
_temp2038->f1=(* y).f1; _temp2038->f2=(* y).f2; _temp2038;}); _temp2037->f2=(* y).f3;
_temp2037;});} static struct _tuple2* Cyc_Tcutil_substitute_f2( struct _tuple16*
w){ void* _temp2041; struct _tuple15* _temp2043; struct _tuple16 _temp2039=* w;
_LL2044: _temp2043= _temp2039.f1; goto _LL2042; _LL2042: _temp2041= _temp2039.f2;
goto _LL2040; _LL2040: { struct Cyc_Absyn_Tqual _temp2047; struct Cyc_Core_Opt*
_temp2049; struct _tuple15 _temp2045=* _temp2043; _LL2050: _temp2049= _temp2045.f1;
goto _LL2048; _LL2048: _temp2047= _temp2045.f2; goto _LL2046; _LL2046: return({
struct _tuple2* _temp2051=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2051->f1= _temp2049; _temp2051->f2= _temp2047; _temp2051->f3= _temp2041;
_temp2051;});}} static void* Cyc_Tcutil_field_type( struct Cyc_Absyn_Structfield*
f){ return( void*) f->type;} static struct Cyc_Absyn_Structfield* Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Structfield* f, void* t){ return({ struct Cyc_Absyn_Structfield*
_temp2052=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2052->name= f->name; _temp2052->tq= f->tq; _temp2052->type=( void*) t;
_temp2052->width= f->width; _temp2052->attributes= f->attributes; _temp2052;});}
static struct Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn,
struct Cyc_List_List* inst, struct Cyc_List_List* ts); void* Cyc_Tcutil_rsubstitute(
struct _RegionHandle* rgn, struct Cyc_List_List* inst, void* t){ void* _temp2053=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar* _temp2099; struct Cyc_Absyn_Structdecl**
_temp2101; struct Cyc_List_List* _temp2103; struct _tuple1* _temp2105; struct
Cyc_Absyn_Uniondecl** _temp2107; struct Cyc_List_List* _temp2109; struct _tuple1*
_temp2111; struct Cyc_Absyn_TunionInfo _temp2113; void* _temp2115; struct Cyc_List_List*
_temp2117; void* _temp2119; struct Cyc_Absyn_TunionFieldInfo _temp2121; struct
Cyc_List_List* _temp2123; void* _temp2125; struct Cyc_Core_Opt* _temp2127;
struct Cyc_List_List* _temp2129; struct _tuple1* _temp2131; struct Cyc_Absyn_Exp*
_temp2133; struct Cyc_Absyn_Tqual _temp2135; void* _temp2137; struct Cyc_Absyn_PtrInfo
_temp2139; struct Cyc_Absyn_Conref* _temp2141; struct Cyc_Absyn_Tqual _temp2143;
struct Cyc_Absyn_Conref* _temp2145; void* _temp2147; void* _temp2149; struct Cyc_Absyn_FnInfo
_temp2151; struct Cyc_List_List* _temp2153; struct Cyc_List_List* _temp2155;
struct Cyc_Absyn_VarargInfo* _temp2157; int _temp2159; struct Cyc_List_List*
_temp2161; void* _temp2163; struct Cyc_Core_Opt* _temp2165; struct Cyc_List_List*
_temp2167; struct Cyc_List_List* _temp2169; struct Cyc_List_List* _temp2171;
struct Cyc_List_List* _temp2173; struct Cyc_Core_Opt* _temp2175; void* _temp2177;
void* _temp2179; struct Cyc_List_List* _temp2181; _LL2055: if(( unsigned int)
_temp2053 > 4u?*(( int*) _temp2053) == Cyc_Absyn_VarType: 0){ _LL2100: _temp2099=((
struct Cyc_Absyn_VarType_struct*) _temp2053)->f1; goto _LL2056;} else{ goto
_LL2057;} _LL2057: if(( unsigned int) _temp2053 > 4u?*(( int*) _temp2053) == Cyc_Absyn_StructType:
0){ _LL2106: _temp2105=(( struct Cyc_Absyn_StructType_struct*) _temp2053)->f1;
goto _LL2104; _LL2104: _temp2103=(( struct Cyc_Absyn_StructType_struct*)
_temp2053)->f2; goto _LL2102; _LL2102: _temp2101=(( struct Cyc_Absyn_StructType_struct*)
_temp2053)->f3; goto _LL2058;} else{ goto _LL2059;} _LL2059: if(( unsigned int)
_temp2053 > 4u?*(( int*) _temp2053) == Cyc_Absyn_UnionType: 0){ _LL2112:
_temp2111=(( struct Cyc_Absyn_UnionType_struct*) _temp2053)->f1; goto _LL2110;
_LL2110: _temp2109=(( struct Cyc_Absyn_UnionType_struct*) _temp2053)->f2; goto
_LL2108; _LL2108: _temp2107=(( struct Cyc_Absyn_UnionType_struct*) _temp2053)->f3;
goto _LL2060;} else{ goto _LL2061;} _LL2061: if(( unsigned int) _temp2053 > 4u?*((
int*) _temp2053) == Cyc_Absyn_TunionType: 0){ _LL2114: _temp2113=(( struct Cyc_Absyn_TunionType_struct*)
_temp2053)->f1; _LL2120: _temp2119=( void*) _temp2113.tunion_info; goto _LL2118;
_LL2118: _temp2117= _temp2113.targs; goto _LL2116; _LL2116: _temp2115=( void*)
_temp2113.rgn; goto _LL2062;} else{ goto _LL2063;} _LL2063: if(( unsigned int)
_temp2053 > 4u?*(( int*) _temp2053) == Cyc_Absyn_TunionFieldType: 0){ _LL2122:
_temp2121=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2053)->f1; _LL2126:
_temp2125=( void*) _temp2121.field_info; goto _LL2124; _LL2124: _temp2123=
_temp2121.targs; goto _LL2064;} else{ goto _LL2065;} _LL2065: if(( unsigned int)
_temp2053 > 4u?*(( int*) _temp2053) == Cyc_Absyn_TypedefType: 0){ _LL2132:
_temp2131=(( struct Cyc_Absyn_TypedefType_struct*) _temp2053)->f1; goto _LL2130;
_LL2130: _temp2129=(( struct Cyc_Absyn_TypedefType_struct*) _temp2053)->f2; goto
_LL2128; _LL2128: _temp2127=(( struct Cyc_Absyn_TypedefType_struct*) _temp2053)->f3;
goto _LL2066;} else{ goto _LL2067;} _LL2067: if(( unsigned int) _temp2053 > 4u?*((
int*) _temp2053) == Cyc_Absyn_ArrayType: 0){ _LL2138: _temp2137=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2053)->f1; goto _LL2136; _LL2136:
_temp2135=(( struct Cyc_Absyn_ArrayType_struct*) _temp2053)->f2; goto _LL2134;
_LL2134: _temp2133=(( struct Cyc_Absyn_ArrayType_struct*) _temp2053)->f3; goto
_LL2068;} else{ goto _LL2069;} _LL2069: if(( unsigned int) _temp2053 > 4u?*((
int*) _temp2053) == Cyc_Absyn_PointerType: 0){ _LL2140: _temp2139=(( struct Cyc_Absyn_PointerType_struct*)
_temp2053)->f1; _LL2150: _temp2149=( void*) _temp2139.elt_typ; goto _LL2148;
_LL2148: _temp2147=( void*) _temp2139.rgn_typ; goto _LL2146; _LL2146: _temp2145=
_temp2139.nullable; goto _LL2144; _LL2144: _temp2143= _temp2139.tq; goto _LL2142;
_LL2142: _temp2141= _temp2139.bounds; goto _LL2070;} else{ goto _LL2071;}
_LL2071: if(( unsigned int) _temp2053 > 4u?*(( int*) _temp2053) == Cyc_Absyn_FnType:
0){ _LL2152: _temp2151=(( struct Cyc_Absyn_FnType_struct*) _temp2053)->f1;
_LL2168: _temp2167= _temp2151.tvars; goto _LL2166; _LL2166: _temp2165= _temp2151.effect;
goto _LL2164; _LL2164: _temp2163=( void*) _temp2151.ret_typ; goto _LL2162;
_LL2162: _temp2161= _temp2151.args; goto _LL2160; _LL2160: _temp2159= _temp2151.c_varargs;
goto _LL2158; _LL2158: _temp2157= _temp2151.cyc_varargs; goto _LL2156; _LL2156:
_temp2155= _temp2151.rgn_po; goto _LL2154; _LL2154: _temp2153= _temp2151.attributes;
goto _LL2072;} else{ goto _LL2073;} _LL2073: if(( unsigned int) _temp2053 > 4u?*((
int*) _temp2053) == Cyc_Absyn_TupleType: 0){ _LL2170: _temp2169=(( struct Cyc_Absyn_TupleType_struct*)
_temp2053)->f1; goto _LL2074;} else{ goto _LL2075;} _LL2075: if(( unsigned int)
_temp2053 > 4u?*(( int*) _temp2053) == Cyc_Absyn_AnonStructType: 0){ _LL2172:
_temp2171=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2053)->f1; goto
_LL2076;} else{ goto _LL2077;} _LL2077: if(( unsigned int) _temp2053 > 4u?*((
int*) _temp2053) == Cyc_Absyn_AnonUnionType: 0){ _LL2174: _temp2173=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp2053)->f1; goto _LL2078;} else{ goto
_LL2079;} _LL2079: if(( unsigned int) _temp2053 > 4u?*(( int*) _temp2053) == Cyc_Absyn_Evar:
0){ _LL2176: _temp2175=(( struct Cyc_Absyn_Evar_struct*) _temp2053)->f2; goto
_LL2080;} else{ goto _LL2081;} _LL2081: if(( unsigned int) _temp2053 > 4u?*((
int*) _temp2053) == Cyc_Absyn_RgnHandleType: 0){ _LL2178: _temp2177=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp2053)->f1; goto _LL2082;} else{
goto _LL2083;} _LL2083: if(( unsigned int) _temp2053 > 4u?*(( int*) _temp2053)
== Cyc_Absyn_EnumType: 0){ goto _LL2084;} else{ goto _LL2085;} _LL2085: if(
_temp2053 ==( void*) Cyc_Absyn_VoidType){ goto _LL2086;} else{ goto _LL2087;}
_LL2087: if(( unsigned int) _temp2053 > 4u?*(( int*) _temp2053) == Cyc_Absyn_IntType:
0){ goto _LL2088;} else{ goto _LL2089;} _LL2089: if( _temp2053 ==( void*) Cyc_Absyn_FloatType){
goto _LL2090;} else{ goto _LL2091;} _LL2091: if( _temp2053 ==( void*) Cyc_Absyn_DoubleType){
goto _LL2092;} else{ goto _LL2093;} _LL2093: if( _temp2053 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL2094;} else{ goto _LL2095;} _LL2095: if(( unsigned int) _temp2053 > 4u?*((
int*) _temp2053) == Cyc_Absyn_AccessEff: 0){ _LL2180: _temp2179=( void*)((
struct Cyc_Absyn_AccessEff_struct*) _temp2053)->f1; goto _LL2096;} else{ goto
_LL2097;} _LL2097: if(( unsigned int) _temp2053 > 4u?*(( int*) _temp2053) == Cyc_Absyn_JoinEff:
0){ _LL2182: _temp2181=(( struct Cyc_Absyn_JoinEff_struct*) _temp2053)->f1; goto
_LL2098;} else{ goto _LL2054;} _LL2056: { struct _handler_cons _temp2183;
_push_handler(& _temp2183);{ int _temp2185= 0; if( setjmp( _temp2183.handler)){
_temp2185= 1;} if( ! _temp2185){{ void* _temp2186=(( void*(*)( int(* compare)(
struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct
Cyc_Absyn_Tvar* x)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp, inst, _temp2099);
_npop_handler( 0u); return _temp2186;}; _pop_handler();} else{ void* _temp2184=(
void*) _exn_thrown; void* _temp2188= _temp2184; _LL2190: if( _temp2188 == Cyc_Core_Not_found){
goto _LL2191;} else{ goto _LL2192;} _LL2192: goto _LL2193; _LL2191: return t;
_LL2193:( void) _throw( _temp2188); _LL2189:;}}} _LL2058: { struct Cyc_List_List*
_temp2194= Cyc_Tcutil_substs( rgn, inst, _temp2103); return _temp2194 ==
_temp2103? t:( void*)({ struct Cyc_Absyn_StructType_struct* _temp2195=( struct
Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp2195[ 0]=({ struct Cyc_Absyn_StructType_struct _temp2196; _temp2196.tag=
Cyc_Absyn_StructType; _temp2196.f1= _temp2105; _temp2196.f2= _temp2194;
_temp2196.f3= _temp2101; _temp2196;}); _temp2195;});} _LL2060: { struct Cyc_List_List*
_temp2197= Cyc_Tcutil_substs( rgn, inst, _temp2109); return _temp2197 ==
_temp2109? t:( void*)({ struct Cyc_Absyn_UnionType_struct* _temp2198=( struct
Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp2198[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp2199; _temp2199.tag= Cyc_Absyn_UnionType;
_temp2199.f1= _temp2111; _temp2199.f2= _temp2197; _temp2199.f3= _temp2107;
_temp2199;}); _temp2198;});} _LL2062: { struct Cyc_List_List* _temp2200= Cyc_Tcutil_substs(
rgn, inst, _temp2117); void* _temp2201= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2115); return( _temp2200 == _temp2117? _temp2201 == _temp2115: 0)? t:( void*)({
struct Cyc_Absyn_TunionType_struct* _temp2202=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2202[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2203; _temp2203.tag= Cyc_Absyn_TunionType;
_temp2203.f1=({ struct Cyc_Absyn_TunionInfo _temp2204; _temp2204.tunion_info=(
void*) _temp2119; _temp2204.targs= _temp2200; _temp2204.rgn=( void*) _temp2201;
_temp2204;}); _temp2203;}); _temp2202;});} _LL2064: { struct Cyc_List_List*
_temp2205= Cyc_Tcutil_substs( rgn, inst, _temp2123); return _temp2205 ==
_temp2123? t:( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp2206=(
struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp2206[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp2207; _temp2207.tag=
Cyc_Absyn_TunionFieldType; _temp2207.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2208; _temp2208.field_info=( void*) _temp2125; _temp2208.targs= _temp2205;
_temp2208;}); _temp2207;}); _temp2206;});} _LL2066: { struct Cyc_List_List*
_temp2209= Cyc_Tcutil_substs( rgn, inst, _temp2129); return _temp2209 ==
_temp2129? t:( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp2210=( struct
Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2210[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp2211; _temp2211.tag=
Cyc_Absyn_TypedefType; _temp2211.f1= _temp2131; _temp2211.f2= _temp2209;
_temp2211.f3= _temp2127; _temp2211;}); _temp2210;});} _LL2068: { void* _temp2212=
Cyc_Tcutil_rsubstitute( rgn, inst, _temp2137); return _temp2212 == _temp2137? t:(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp2213=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2213[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2214; _temp2214.tag= Cyc_Absyn_ArrayType;
_temp2214.f1=( void*) _temp2212; _temp2214.f2= _temp2135; _temp2214.f3=
_temp2133; _temp2214;}); _temp2213;});} _LL2070: { void* _temp2215= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2149); void* _temp2216= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2147); if( _temp2215 == _temp2149? _temp2216 == _temp2147: 0){ return t;}
return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp2217=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp2217[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp2218; _temp2218.tag= Cyc_Absyn_PointerType;
_temp2218.f1=({ struct Cyc_Absyn_PtrInfo _temp2219; _temp2219.elt_typ=( void*)
_temp2215; _temp2219.rgn_typ=( void*) _temp2216; _temp2219.nullable= _temp2145;
_temp2219.tq= _temp2143; _temp2219.bounds= _temp2141; _temp2219;}); _temp2218;});
_temp2217;});} _LL2072:{ struct Cyc_List_List* _temp2220= _temp2167; for( 0;
_temp2220 != 0; _temp2220=(( struct Cyc_List_List*) _check_null( _temp2220))->tl){
inst=({ struct Cyc_List_List* _temp2221=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2221->hd=( void*)({ struct _tuple5*
_temp2222=( struct _tuple5*) _region_malloc( rgn, sizeof( struct _tuple5));
_temp2222->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp2220))->hd; _temp2222->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2223=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2223[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2224; _temp2224.tag= Cyc_Absyn_VarType;
_temp2224.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp2220))->hd; _temp2224;}); _temp2223;}); _temp2222;}); _temp2221->tl= inst;
_temp2221;});}}{ struct Cyc_List_List* _temp2227; struct Cyc_List_List*
_temp2229; struct _tuple0 _temp2225=(( struct _tuple0(*)( struct _RegionHandle*
r3, struct _RegionHandle* r4, struct Cyc_List_List* x)) Cyc_List_rsplit)( rgn,
rgn,(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple16*(* f)(
struct _RegionHandle*, struct _tuple2*), struct _RegionHandle* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_substitute_f1, rgn, _temp2161)); _LL2230:
_temp2229= _temp2225.f1; goto _LL2228; _LL2228: _temp2227= _temp2225.f2; goto
_LL2226; _LL2226: { struct Cyc_List_List* _temp2231= Cyc_Tcutil_substs( rgn,
inst, _temp2227); struct Cyc_List_List* _temp2232=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple16*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_substitute_f2,(( struct Cyc_List_List*(*)( struct _RegionHandle* r3,
struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp2229, _temp2231)); struct Cyc_Core_Opt* eff2; if( _temp2165 == 0){
eff2= 0;} else{ void* _temp2233= Cyc_Tcutil_rsubstitute( rgn, inst,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2165))->v); if( _temp2233 ==( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2165))->v){ eff2= _temp2165;} else{ eff2=({
struct Cyc_Core_Opt* _temp2234=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2234->v=( void*) _temp2233; _temp2234;});}}{ struct Cyc_Absyn_VarargInfo*
cyc_varargs2; if( _temp2157 == 0){ cyc_varargs2= 0;} else{ int _temp2237; void*
_temp2239; void* _temp2241; struct Cyc_Absyn_Tqual _temp2243; struct Cyc_Core_Opt*
_temp2245; struct Cyc_Absyn_VarargInfo _temp2235=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp2157)); _LL2246: _temp2245= _temp2235.name; goto _LL2244;
_LL2244: _temp2243= _temp2235.tq; goto _LL2242; _LL2242: _temp2241=( void*)
_temp2235.type; goto _LL2240; _LL2240: _temp2239=( void*) _temp2235.rgn; goto
_LL2238; _LL2238: _temp2237= _temp2235.inject; goto _LL2236; _LL2236: { void*
_temp2247= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2241); void* _temp2248= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2239); if( _temp2247 == _temp2241? _temp2239 == _temp2248: 0){
cyc_varargs2= _temp2157;} else{ cyc_varargs2=({ struct Cyc_Absyn_VarargInfo*
_temp2249=( struct Cyc_Absyn_VarargInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2249->name= _temp2245; _temp2249->tq= _temp2243; _temp2249->type=( void*)
_temp2247; _temp2249->rgn=( void*) _temp2248; _temp2249->inject= _temp2237;
_temp2249;});}}}{ struct Cyc_List_List* rgn_po2; struct Cyc_List_List* _temp2252;
struct Cyc_List_List* _temp2254; struct _tuple0 _temp2250= Cyc_List_rsplit( rgn,
rgn, _temp2155); _LL2255: _temp2254= _temp2250.f1; goto _LL2253; _LL2253:
_temp2252= _temp2250.f2; goto _LL2251; _LL2251: { struct Cyc_List_List*
_temp2256= Cyc_Tcutil_substs( rgn, inst, _temp2254); struct Cyc_List_List*
_temp2257= Cyc_Tcutil_substs( rgn, inst, _temp2252); if( _temp2256 == _temp2254?
_temp2257 == _temp2252: 0){ rgn_po2= _temp2155;} else{ rgn_po2= Cyc_List_zip(
_temp2256, _temp2257);} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2258=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2258[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2259; _temp2259.tag= Cyc_Absyn_FnType;
_temp2259.f1=({ struct Cyc_Absyn_FnInfo _temp2260; _temp2260.tvars= _temp2167;
_temp2260.effect= eff2; _temp2260.ret_typ=( void*) Cyc_Tcutil_rsubstitute( rgn,
inst, _temp2163); _temp2260.args= _temp2232; _temp2260.c_varargs= _temp2159;
_temp2260.cyc_varargs= cyc_varargs2; _temp2260.rgn_po= rgn_po2; _temp2260.attributes=
_temp2153; _temp2260;}); _temp2259;}); _temp2258;});}}}}} _LL2074: { struct Cyc_List_List*
_temp2261=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_snd_tqt,
_temp2169); struct Cyc_List_List* _temp2262= Cyc_Tcutil_substs( rgn, inst,
_temp2261); if( _temp2262 == _temp2261){ return t;}{ struct Cyc_List_List*
_temp2263=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*,
void*), struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_map2)( Cyc_Tcutil_map2_tq,
_temp2169, _temp2262); return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp2264=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp2264[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp2265; _temp2265.tag= Cyc_Absyn_TupleType;
_temp2265.f1= _temp2263; _temp2265;}); _temp2264;});}} _LL2076: { struct Cyc_List_List*
_temp2266=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2171); struct Cyc_List_List* _temp2267= Cyc_Tcutil_substs( rgn, inst,
_temp2266); if( _temp2267 == _temp2266){ return t;}{ struct Cyc_List_List*
_temp2268=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2171, _temp2267); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp2269=( struct Cyc_Absyn_AnonStructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2269[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2270; _temp2270.tag= Cyc_Absyn_AnonStructType;
_temp2270.f1= _temp2268; _temp2270;}); _temp2269;});}} _LL2078: { struct Cyc_List_List*
_temp2271=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2173); struct Cyc_List_List* _temp2272= Cyc_Tcutil_substs( rgn, inst,
_temp2271); if( _temp2272 == _temp2271){ return t;}{ struct Cyc_List_List*
_temp2273=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2173, _temp2272); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp2274=( struct Cyc_Absyn_AnonStructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2274[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2275; _temp2275.tag= Cyc_Absyn_AnonStructType;
_temp2275.f1= _temp2273; _temp2275;}); _temp2274;});}} _LL2080: if( _temp2175 !=
0){ return Cyc_Tcutil_rsubstitute( rgn, inst,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2175))->v);} else{ return t;} _LL2082: { void* _temp2276= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2177); return _temp2276 == _temp2177? t:( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp2277=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp2277[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp2278; _temp2278.tag= Cyc_Absyn_RgnHandleType; _temp2278.f1=( void*)
_temp2276; _temp2278;}); _temp2277;});} _LL2084: return t; _LL2086: return t;
_LL2088: return t; _LL2090: return t; _LL2092: return t; _LL2094: return t;
_LL2096: { void* _temp2279= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2179);
return _temp2279 == _temp2179? t:( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp2280=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2280[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2281; _temp2281.tag= Cyc_Absyn_AccessEff;
_temp2281.f1=( void*) _temp2279; _temp2281;}); _temp2280;});} _LL2098: { struct
Cyc_List_List* _temp2282= Cyc_Tcutil_substs( rgn, inst, _temp2181); return
_temp2282 == _temp2181? t:( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2283=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2283[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2284; _temp2284.tag= Cyc_Absyn_JoinEff;
_temp2284.f1= _temp2282; _temp2284;}); _temp2283;});} _LL2054:;} static struct
Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List*
inst, struct Cyc_List_List* ts){ if( ts == 0){ return 0;}{ void* _temp2285=(
void*)(( struct Cyc_List_List*) _check_null( ts))->hd; struct Cyc_List_List*
_temp2286=(( struct Cyc_List_List*) _check_null( ts))->tl; void* _temp2287= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2285); struct Cyc_List_List* _temp2288= Cyc_Tcutil_substs( rgn,
inst, _temp2286); if( _temp2285 == _temp2287? _temp2286 == _temp2288: 0){ return
ts;} return( struct Cyc_List_List*)({ struct Cyc_List_List* _temp2289=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2289->hd=( void*)
_temp2287; _temp2289->tl= _temp2288; _temp2289;});}} extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List* inst, void* t){ return Cyc_Tcutil_rsubstitute( Cyc_Core_heap_region,
inst, t);} struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2338=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2338[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2339; _temp2339.tag= Cyc_Absyn_Const_e; _temp2339.f1=(
void*)(( void*) Cyc_Absyn_Null_c); _temp2339;}); _temp2338;}), loc);{ void*
_temp2290= Cyc_Tcutil_compress( t); void* _temp2304; void* _temp2306; void*
_temp2308; void* _temp2310; _LL2292: if(( unsigned int) _temp2290 > 4u?*(( int*)
_temp2290) == Cyc_Absyn_PointerType: 0){ goto _LL2293;} else{ goto _LL2294;}
_LL2294: if(( unsigned int) _temp2290 > 4u?*(( int*) _temp2290) == Cyc_Absyn_IntType:
0){ _LL2307: _temp2306=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2290)->f1;
goto _LL2305; _LL2305: _temp2304=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2290)->f2; if( _temp2304 ==( void*) Cyc_Absyn_B1){ goto _LL2295;} else{
goto _LL2296;}} else{ goto _LL2296;} _LL2296: if(( unsigned int) _temp2290 > 4u?*((
int*) _temp2290) == Cyc_Absyn_IntType: 0){ _LL2311: _temp2310=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2290)->f1; goto _LL2309; _LL2309: _temp2308=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2290)->f2; goto _LL2297;} else{
goto _LL2298;} _LL2298: if( _temp2290 ==( void*) Cyc_Absyn_FloatType){ goto
_LL2299;} else{ goto _LL2300;} _LL2300: if( _temp2290 ==( void*) Cyc_Absyn_DoubleType){
goto _LL2301;} else{ goto _LL2302;} _LL2302: goto _LL2303; _LL2293: goto _LL2291;
_LL2295:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2312=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2312[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2313; _temp2313.tag= Cyc_Absyn_Const_e;
_temp2313.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp2314=(
struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp2314[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp2315; _temp2315.tag= Cyc_Absyn_Char_c;
_temp2315.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2315.f2='\000'; _temp2315;});
_temp2314;})); _temp2313;}); _temp2312;}))); goto _LL2291; _LL2297:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2316=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2316[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2317; _temp2317.tag= Cyc_Absyn_Const_e; _temp2317.f1=(
void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp2318=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp2318[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp2319; _temp2319.tag= Cyc_Absyn_Int_c; _temp2319.f1=( void*) _temp2310;
_temp2319.f2= 0; _temp2319;}); _temp2318;})); _temp2317;}); _temp2316;}))); if(
_temp2308 !=( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp2320=( struct Cyc_Absyn_Cast_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp2320[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp2321; _temp2321.tag= Cyc_Absyn_Cast_e;
_temp2321.f1=( void*) t; _temp2321.f2= e; _temp2321;}); _temp2320;}), loc);}
goto _LL2291; _LL2299:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2322=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2322[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2323; _temp2323.tag= Cyc_Absyn_Const_e;
_temp2323.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2324=(
struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2324[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2325; _temp2325.tag= Cyc_Absyn_Float_c;
_temp2325.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp2325;});
_temp2324;})); _temp2323;}); _temp2322;}))); goto _LL2291; _LL2301:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2326=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2326[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2327; _temp2327.tag= Cyc_Absyn_Cast_e; _temp2327.f1=( void*) t; _temp2327.f2=
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2328=( struct
Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2328[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2329; _temp2329.tag= Cyc_Absyn_Const_e;
_temp2329.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2330=(
struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2330[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2331; _temp2331.tag= Cyc_Absyn_Float_c;
_temp2331.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp2331;});
_temp2330;})); _temp2329;}); _temp2328;}), loc); _temp2327;}); _temp2326;})));
goto _LL2291; _LL2303: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp2332)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp2336= _tag_arr("declaration of type %s requires initializer",
sizeof( unsigned char), 44u); struct Cyc_Stdio_String_pa_struct _temp2337;
_temp2337.tag= Cyc_Stdio_String_pa; _temp2337.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp2335=( void*)& _temp2337; void* _temp2333[ 1u]={ _temp2335};
struct _tagged_arr _temp2334={( void*) _temp2333,( void*) _temp2333,( void*)(
_temp2333 + 1u)}; _temp2332( _temp2336, _temp2334);}})); goto _LL2291; _LL2291:;}
return e;} struct _tuple5* Cyc_Tcutil_make_inst_var( struct Cyc_List_List* s,
struct Cyc_Absyn_Tvar* tv){ void* k= Cyc_Absyn_conref_val( tv->kind); return({
struct _tuple5* _temp2340=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp2340->f1= tv; _temp2340->f2= Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt*
_temp2341=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2341->v=( void*) k; _temp2341;}),({ struct Cyc_Core_Opt* _temp2342=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2342->v=( void*) s;
_temp2342;})); _temp2340;});} struct _tuple5* Cyc_Tcutil_r_make_inst_var( struct
_tuple6* env, struct Cyc_Absyn_Tvar* tv){ struct _tuple6 _temp2345; struct
_RegionHandle* _temp2346; struct Cyc_List_List* _temp2348; struct _tuple6*
_temp2343= env; _temp2345=* _temp2343; _LL2349: _temp2348= _temp2345.f1; goto
_LL2347; _LL2347: _temp2346= _temp2345.f2; goto _LL2344; _LL2344: { void* k= Cyc_Absyn_conref_val(
tv->kind); return({ struct _tuple5* _temp2350=( struct _tuple5*) _region_malloc(
_temp2346, sizeof( struct _tuple5)); _temp2350->f1= tv; _temp2350->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp2351=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2351->v=( void*) k; _temp2351;}),({ struct Cyc_Core_Opt*
_temp2352=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2352->v=( void*) _temp2348; _temp2352;})); _temp2350;});}} static struct
Cyc_List_List* Cyc_Tcutil_add_free_tvar( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 != 0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
if( Cyc_String_zstrptrcmp((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->name, tv->name) == 0){ struct Cyc_Absyn_Conref* k1=((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->kind;
struct Cyc_Absyn_Conref* k2= tv->kind; if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp2353)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp2359= _tag_arr("type variable %s is used with inconsistent kinds %s and %s",
sizeof( unsigned char), 59u); struct Cyc_Stdio_String_pa_struct _temp2362;
_temp2362.tag= Cyc_Stdio_String_pa; _temp2362.f1=( struct _tagged_arr)* tv->name;{
void* _temp2356=( void*)& _temp2362; struct Cyc_Stdio_String_pa_struct _temp2361;
_temp2361.tag= Cyc_Stdio_String_pa; _temp2361.f1=( struct _tagged_arr) Cyc_Absynpp_ckind2string(
k1);{ void* _temp2357=( void*)& _temp2361; struct Cyc_Stdio_String_pa_struct
_temp2360; _temp2360.tag= Cyc_Stdio_String_pa; _temp2360.f1=( struct _tagged_arr)
Cyc_Absynpp_ckind2string( k2);{ void* _temp2358=( void*)& _temp2360; void*
_temp2354[ 3u]={ _temp2356, _temp2357, _temp2358}; struct _tagged_arr _temp2355={(
void*) _temp2354,( void*) _temp2354,( void*)( _temp2354 + 3u)}; _temp2353(
_temp2359, _temp2355);}}}}));} if( tv->identity == 0){ tv->identity=(( struct
Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->identity;}
else{ if(*(( int*) _check_null( tv->identity)) !=*(( int*) _check_null((( struct
Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->identity))){((
int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)(( struct _tagged_arr)({
struct _tagged_arr(* _temp2363)( struct _tagged_arr fmt, struct _tagged_arr)=
Cyc_Stdio_aprintf; struct _tagged_arr _temp2366= _tag_arr("same type variable has different identity!",
sizeof( unsigned char), 43u); void* _temp2364[ 0u]={}; struct _tagged_arr
_temp2365={( void*) _temp2364,( void*) _temp2364,( void*)( _temp2364 + 0u)};
_temp2363( _temp2366, _temp2365);}));}} return tvs;}}} tv->identity= Cyc_Tcutil_new_tvar_id();
return({ struct Cyc_List_List* _temp2367=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2367->hd=( void*) tv; _temp2367->tl= tvs;
_temp2367;});} static struct Cyc_List_List* Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){ if( tv->identity == 0){((
int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)( _tag_arr("fast_add_free_tvar: bad identity in tv",
sizeof( unsigned char), 39u));}{ struct Cyc_List_List* tvs2= tvs; for( 0; tvs2
!= 0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){ if((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->identity == 0){(( int(*)(
struct _tagged_arr msg)) Cyc_Tcutil_impos)( _tag_arr("fast_add_free_tvar: bad identity in tvs2",
sizeof( unsigned char), 41u));} if(*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->identity)) ==*(( int*)
_check_null( tv->identity))){ return tvs;}}} return({ struct Cyc_List_List*
_temp2368=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2368->hd=( void*) tv; _temp2368->tl= tvs; _temp2368;});} static struct Cyc_List_List*
Cyc_Tcutil_add_bound_tvar( struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){
if( tv->identity == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)((
struct _tagged_arr)({ struct _tagged_arr(* _temp2369)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp2373= _tag_arr("bound tvar id for %s is null",
sizeof( unsigned char), 29u); struct Cyc_Stdio_String_pa_struct _temp2374;
_temp2374.tag= Cyc_Stdio_String_pa; _temp2374.f1=( struct _tagged_arr) Cyc_Tcutil_tvar2string(
tv);{ void* _temp2372=( void*)& _temp2374; void* _temp2370[ 1u]={ _temp2372};
struct _tagged_arr _temp2371={( void*) _temp2370,( void*) _temp2370,( void*)(
_temp2370 + 1u)}; _temp2369( _temp2373, _temp2371);}}));} return({ struct Cyc_List_List*
_temp2375=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2375->hd=( void*) tv; _temp2375->tl= tvs; _temp2375;});} static struct Cyc_List_List*
Cyc_Tcutil_add_free_evar( struct Cyc_List_List* es, void* e){ void* _temp2376=
Cyc_Tcutil_compress( e); int _temp2382; _LL2378: if(( unsigned int) _temp2376 >
4u?*(( int*) _temp2376) == Cyc_Absyn_Evar: 0){ _LL2383: _temp2382=(( struct Cyc_Absyn_Evar_struct*)
_temp2376)->f3; goto _LL2379;} else{ goto _LL2380;} _LL2380: goto _LL2381;
_LL2379:{ struct Cyc_List_List* es2= es; for( 0; es2 != 0; es2=(( struct Cyc_List_List*)
_check_null( es2))->tl){ void* _temp2384= Cyc_Tcutil_compress(( void*)(( struct
Cyc_List_List*) _check_null( es2))->hd); int _temp2390; _LL2386: if((
unsigned int) _temp2384 > 4u?*(( int*) _temp2384) == Cyc_Absyn_Evar: 0){ _LL2391:
_temp2390=(( struct Cyc_Absyn_Evar_struct*) _temp2384)->f3; goto _LL2387;} else{
goto _LL2388;} _LL2388: goto _LL2389; _LL2387: if( _temp2382 == _temp2390){
return es;} goto _LL2385; _LL2389: goto _LL2385; _LL2385:;}} return({ struct Cyc_List_List*
_temp2392=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2392->hd=( void*) e; _temp2392->tl= es; _temp2392;}); _LL2381: return es;
_LL2377:;} static struct Cyc_List_List* Cyc_Tcutil_remove_bound_tvars( struct
Cyc_List_List* tvs, struct Cyc_List_List* btvs){ struct Cyc_List_List* r= 0;
for( 0; tvs != 0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ int
present= 0;{ struct Cyc_List_List* b= btvs; for( 0; b != 0; b=(( struct Cyc_List_List*)
_check_null( b))->tl){ if(*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->identity)) ==*(( int*)
_check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd)->identity))){
present= 1; break;}}} if( ! present){ r=({ struct Cyc_List_List* _temp2393=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2393->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd);
_temp2393->tl= r; _temp2393;});}} r=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( r); return r;} struct Cyc_Tcutil_CVTEnv{ struct Cyc_List_List*
kind_env; struct Cyc_List_List* free_vars; struct Cyc_List_List* free_evars; } ;
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcutil_CVTEnv cvtenv, void*
expected_kind, void* t){{ void* _temp2394= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt*
_temp2440; struct Cyc_Core_Opt** _temp2442; struct Cyc_Absyn_Tvar* _temp2443;
struct Cyc_Absyn_Enumdecl* _temp2445; struct Cyc_Absyn_Enumdecl** _temp2447;
struct _tuple1* _temp2448; struct Cyc_Absyn_TunionInfo _temp2450; void*
_temp2452; struct Cyc_List_List* _temp2454; void* _temp2456; void** _temp2458;
struct Cyc_Absyn_TunionFieldInfo _temp2459; struct Cyc_List_List* _temp2461;
void* _temp2463; void** _temp2465; struct Cyc_Absyn_PtrInfo _temp2466; struct
Cyc_Absyn_Conref* _temp2468; struct Cyc_Absyn_Tqual _temp2470; struct Cyc_Absyn_Conref*
_temp2472; void* _temp2474; void* _temp2476; struct Cyc_Absyn_Exp* _temp2478;
struct Cyc_Absyn_Tqual _temp2480; void* _temp2482; struct Cyc_Absyn_FnInfo
_temp2484; struct Cyc_List_List* _temp2486; struct Cyc_List_List* _temp2488;
struct Cyc_Absyn_VarargInfo* _temp2490; int _temp2492; struct Cyc_List_List*
_temp2494; void* _temp2496; struct Cyc_Core_Opt* _temp2498; struct Cyc_Core_Opt**
_temp2500; struct Cyc_List_List* _temp2501; struct Cyc_List_List** _temp2503;
struct Cyc_List_List* _temp2504; struct Cyc_List_List* _temp2506; struct Cyc_List_List*
_temp2508; struct Cyc_Absyn_Structdecl** _temp2510; struct Cyc_Absyn_Structdecl***
_temp2512; struct Cyc_List_List* _temp2513; struct _tuple1* _temp2515; struct
Cyc_Absyn_Uniondecl** _temp2517; struct Cyc_Absyn_Uniondecl*** _temp2519; struct
Cyc_List_List* _temp2520; struct _tuple1* _temp2522; struct Cyc_Core_Opt*
_temp2524; struct Cyc_Core_Opt** _temp2526; struct Cyc_List_List* _temp2527;
struct _tuple1* _temp2529; void* _temp2531; void* _temp2533; struct Cyc_List_List*
_temp2535; _LL2396: if( _temp2394 ==( void*) Cyc_Absyn_VoidType){ goto _LL2397;}
else{ goto _LL2398;} _LL2398: if(( unsigned int) _temp2394 > 4u?*(( int*)
_temp2394) == Cyc_Absyn_Evar: 0){ _LL2441: _temp2440=(( struct Cyc_Absyn_Evar_struct*)
_temp2394)->f1; _temp2442=&(( struct Cyc_Absyn_Evar_struct*) _temp2394)->f1;
goto _LL2399;} else{ goto _LL2400;} _LL2400: if(( unsigned int) _temp2394 > 4u?*((
int*) _temp2394) == Cyc_Absyn_VarType: 0){ _LL2444: _temp2443=(( struct Cyc_Absyn_VarType_struct*)
_temp2394)->f1; goto _LL2401;} else{ goto _LL2402;} _LL2402: if(( unsigned int)
_temp2394 > 4u?*(( int*) _temp2394) == Cyc_Absyn_EnumType: 0){ _LL2449:
_temp2448=(( struct Cyc_Absyn_EnumType_struct*) _temp2394)->f1; goto _LL2446;
_LL2446: _temp2445=(( struct Cyc_Absyn_EnumType_struct*) _temp2394)->f2;
_temp2447=&(( struct Cyc_Absyn_EnumType_struct*) _temp2394)->f2; goto _LL2403;}
else{ goto _LL2404;} _LL2404: if(( unsigned int) _temp2394 > 4u?*(( int*)
_temp2394) == Cyc_Absyn_TunionType: 0){ _LL2451: _temp2450=(( struct Cyc_Absyn_TunionType_struct*)
_temp2394)->f1; _LL2457: _temp2456=( void*) _temp2450.tunion_info; _temp2458=&(((
struct Cyc_Absyn_TunionType_struct*) _temp2394)->f1).tunion_info; goto _LL2455;
_LL2455: _temp2454= _temp2450.targs; goto _LL2453; _LL2453: _temp2452=( void*)
_temp2450.rgn; goto _LL2405;} else{ goto _LL2406;} _LL2406: if(( unsigned int)
_temp2394 > 4u?*(( int*) _temp2394) == Cyc_Absyn_TunionFieldType: 0){ _LL2460:
_temp2459=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2394)->f1; _LL2464:
_temp2463=( void*) _temp2459.field_info; _temp2465=&((( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2394)->f1).field_info; goto _LL2462; _LL2462: _temp2461= _temp2459.targs;
goto _LL2407;} else{ goto _LL2408;} _LL2408: if(( unsigned int) _temp2394 > 4u?*((
int*) _temp2394) == Cyc_Absyn_PointerType: 0){ _LL2467: _temp2466=(( struct Cyc_Absyn_PointerType_struct*)
_temp2394)->f1; _LL2477: _temp2476=( void*) _temp2466.elt_typ; goto _LL2475;
_LL2475: _temp2474=( void*) _temp2466.rgn_typ; goto _LL2473; _LL2473: _temp2472=
_temp2466.nullable; goto _LL2471; _LL2471: _temp2470= _temp2466.tq; goto _LL2469;
_LL2469: _temp2468= _temp2466.bounds; goto _LL2409;} else{ goto _LL2410;}
_LL2410: if(( unsigned int) _temp2394 > 4u?*(( int*) _temp2394) == Cyc_Absyn_IntType:
0){ goto _LL2411;} else{ goto _LL2412;} _LL2412: if( _temp2394 ==( void*) Cyc_Absyn_FloatType){
goto _LL2413;} else{ goto _LL2414;} _LL2414: if( _temp2394 ==( void*) Cyc_Absyn_DoubleType){
goto _LL2415;} else{ goto _LL2416;} _LL2416: if(( unsigned int) _temp2394 > 4u?*((
int*) _temp2394) == Cyc_Absyn_ArrayType: 0){ _LL2483: _temp2482=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2394)->f1; goto _LL2481; _LL2481:
_temp2480=(( struct Cyc_Absyn_ArrayType_struct*) _temp2394)->f2; goto _LL2479;
_LL2479: _temp2478=(( struct Cyc_Absyn_ArrayType_struct*) _temp2394)->f3; goto
_LL2417;} else{ goto _LL2418;} _LL2418: if(( unsigned int) _temp2394 > 4u?*((
int*) _temp2394) == Cyc_Absyn_FnType: 0){ _LL2485: _temp2484=(( struct Cyc_Absyn_FnType_struct*)
_temp2394)->f1; _LL2502: _temp2501= _temp2484.tvars; _temp2503=&((( struct Cyc_Absyn_FnType_struct*)
_temp2394)->f1).tvars; goto _LL2499; _LL2499: _temp2498= _temp2484.effect;
_temp2500=&((( struct Cyc_Absyn_FnType_struct*) _temp2394)->f1).effect; goto
_LL2497; _LL2497: _temp2496=( void*) _temp2484.ret_typ; goto _LL2495; _LL2495:
_temp2494= _temp2484.args; goto _LL2493; _LL2493: _temp2492= _temp2484.c_varargs;
goto _LL2491; _LL2491: _temp2490= _temp2484.cyc_varargs; goto _LL2489; _LL2489:
_temp2488= _temp2484.rgn_po; goto _LL2487; _LL2487: _temp2486= _temp2484.attributes;
goto _LL2419;} else{ goto _LL2420;} _LL2420: if(( unsigned int) _temp2394 > 4u?*((
int*) _temp2394) == Cyc_Absyn_TupleType: 0){ _LL2505: _temp2504=(( struct Cyc_Absyn_TupleType_struct*)
_temp2394)->f1; goto _LL2421;} else{ goto _LL2422;} _LL2422: if(( unsigned int)
_temp2394 > 4u?*(( int*) _temp2394) == Cyc_Absyn_AnonStructType: 0){ _LL2507:
_temp2506=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2394)->f1; goto
_LL2423;} else{ goto _LL2424;} _LL2424: if(( unsigned int) _temp2394 > 4u?*((
int*) _temp2394) == Cyc_Absyn_AnonUnionType: 0){ _LL2509: _temp2508=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp2394)->f1; goto _LL2425;} else{ goto
_LL2426;} _LL2426: if(( unsigned int) _temp2394 > 4u?*(( int*) _temp2394) == Cyc_Absyn_StructType:
0){ _LL2516: _temp2515=(( struct Cyc_Absyn_StructType_struct*) _temp2394)->f1;
goto _LL2514; _LL2514: _temp2513=(( struct Cyc_Absyn_StructType_struct*)
_temp2394)->f2; goto _LL2511; _LL2511: _temp2510=(( struct Cyc_Absyn_StructType_struct*)
_temp2394)->f3; _temp2512=&(( struct Cyc_Absyn_StructType_struct*) _temp2394)->f3;
goto _LL2427;} else{ goto _LL2428;} _LL2428: if(( unsigned int) _temp2394 > 4u?*((
int*) _temp2394) == Cyc_Absyn_UnionType: 0){ _LL2523: _temp2522=(( struct Cyc_Absyn_UnionType_struct*)
_temp2394)->f1; goto _LL2521; _LL2521: _temp2520=(( struct Cyc_Absyn_UnionType_struct*)
_temp2394)->f2; goto _LL2518; _LL2518: _temp2517=(( struct Cyc_Absyn_UnionType_struct*)
_temp2394)->f3; _temp2519=&(( struct Cyc_Absyn_UnionType_struct*) _temp2394)->f3;
goto _LL2429;} else{ goto _LL2430;} _LL2430: if(( unsigned int) _temp2394 > 4u?*((
int*) _temp2394) == Cyc_Absyn_TypedefType: 0){ _LL2530: _temp2529=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2394)->f1; goto _LL2528; _LL2528: _temp2527=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2394)->f2; goto _LL2525; _LL2525: _temp2524=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2394)->f3; _temp2526=&(( struct Cyc_Absyn_TypedefType_struct*) _temp2394)->f3;
goto _LL2431;} else{ goto _LL2432;} _LL2432: if( _temp2394 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL2433;} else{ goto _LL2434;} _LL2434: if(( unsigned int) _temp2394 > 4u?*((
int*) _temp2394) == Cyc_Absyn_RgnHandleType: 0){ _LL2532: _temp2531=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp2394)->f1; goto _LL2435;} else{
goto _LL2436;} _LL2436: if(( unsigned int) _temp2394 > 4u?*(( int*) _temp2394)
== Cyc_Absyn_AccessEff: 0){ _LL2534: _temp2533=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp2394)->f1; goto _LL2437;} else{ goto _LL2438;} _LL2438: if(( unsigned int)
_temp2394 > 4u?*(( int*) _temp2394) == Cyc_Absyn_JoinEff: 0){ _LL2536: _temp2535=((
struct Cyc_Absyn_JoinEff_struct*) _temp2394)->f1; goto _LL2439;} else{ goto
_LL2395;} _LL2397: goto _LL2395; _LL2399: if(* _temp2442 == 0){* _temp2442=({
struct Cyc_Core_Opt* _temp2537=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2537->v=( void*) expected_kind; _temp2537;});} cvtenv.free_evars=
Cyc_Tcutil_add_free_evar( cvtenv.free_evars, t); goto _LL2395; _LL2401: { struct
Cyc_Absyn_Conref* c= Cyc_Absyn_compress_conref( _temp2443->kind); if(( void*) c->v
==( void*) Cyc_Absyn_No_constr){( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2538=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2538[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2539; _temp2539.tag= Cyc_Absyn_Eq_constr;
_temp2539.f1=( void*) expected_kind; _temp2539;}); _temp2538;})));} cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar( loc, cvtenv.kind_env, _temp2443); cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars, _temp2443); goto _LL2395;}
_LL2403: if(* _temp2447 == 0){ struct _handler_cons _temp2540; _push_handler(&
_temp2540);{ int _temp2542= 0; if( setjmp( _temp2540.handler)){ _temp2542= 1;}
if( ! _temp2542){{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl( te,
loc, _temp2448);* _temp2447=( struct Cyc_Absyn_Enumdecl*)* ed;}; _pop_handler();}
else{ void* _temp2541=( void*) _exn_thrown; void* _temp2544= _temp2541; _LL2546:
if( _temp2544 == Cyc_Dict_Absent){ goto _LL2547;} else{ goto _LL2548;} _LL2548:
goto _LL2549; _LL2547: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp2550)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp2554= _tag_arr("unbound type enum %s", sizeof(
unsigned char), 21u); struct Cyc_Stdio_String_pa_struct _temp2555; _temp2555.tag=
Cyc_Stdio_String_pa; _temp2555.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2448);{ void* _temp2553=( void*)& _temp2555; void* _temp2551[ 1u]={
_temp2553}; struct _tagged_arr _temp2552={( void*) _temp2551,( void*) _temp2551,(
void*)( _temp2551 + 1u)}; _temp2550( _temp2554, _temp2552);}})); return cvtenv;
_LL2549:( void) _throw( _temp2544); _LL2545:;}}}{ struct Cyc_Absyn_Enumdecl* ed=(
struct Cyc_Absyn_Enumdecl*) _check_null(* _temp2447);* _temp2448=( ed->name)[
_check_known_subscript_notnull( 1u, 0)]; goto _LL2395;} _LL2405:{ void*
_temp2556=* _temp2458; struct Cyc_Absyn_UnknownTunionInfo _temp2562; int
_temp2564; struct _tuple1* _temp2566; struct Cyc_Absyn_Tuniondecl* _temp2568;
_LL2558: if(*(( int*) _temp2556) == Cyc_Absyn_UnknownTunion){ _LL2563: _temp2562=((
struct Cyc_Absyn_UnknownTunion_struct*) _temp2556)->f1; _LL2567: _temp2566=
_temp2562.name; goto _LL2565; _LL2565: _temp2564= _temp2562.is_xtunion; goto
_LL2559;} else{ goto _LL2560;} _LL2560: if(*(( int*) _temp2556) == Cyc_Absyn_KnownTunion){
_LL2569: _temp2568=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2556)->f1; goto
_LL2561;} else{ goto _LL2557;} _LL2559: { struct Cyc_Absyn_Tuniondecl* tud;{
struct _handler_cons _temp2570; _push_handler(& _temp2570);{ int _temp2572= 0;
if( setjmp( _temp2570.handler)){ _temp2572= 1;} if( ! _temp2572){ tud=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp2566);; _pop_handler();} else{ void* _temp2571=( void*)
_exn_thrown; void* _temp2574= _temp2571; _LL2576: if( _temp2574 == Cyc_Dict_Absent){
goto _LL2577;} else{ goto _LL2578;} _LL2578: goto _LL2579; _LL2577: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp2580)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp2584=
_tag_arr("unbound type [x]tunion %s", sizeof( unsigned char), 26u); struct Cyc_Stdio_String_pa_struct
_temp2585; _temp2585.tag= Cyc_Stdio_String_pa; _temp2585.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2566);{ void* _temp2583=( void*)& _temp2585; void*
_temp2581[ 1u]={ _temp2583}; struct _tagged_arr _temp2582={( void*) _temp2581,(
void*) _temp2581,( void*)( _temp2581 + 1u)}; _temp2580( _temp2584, _temp2582);}}));
return cvtenv; _LL2579:( void) _throw( _temp2574); _LL2575:;}}} if( tud->is_xtunion
!= _temp2564){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp2586)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp2590= _tag_arr("[x]tunion is different from type declaration %s",
sizeof( unsigned char), 48u); struct Cyc_Stdio_String_pa_struct _temp2591;
_temp2591.tag= Cyc_Stdio_String_pa; _temp2591.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2566);{ void* _temp2589=( void*)& _temp2591; void* _temp2587[ 1u]={
_temp2589}; struct _tagged_arr _temp2588={( void*) _temp2587,( void*) _temp2587,(
void*)( _temp2587 + 1u)}; _temp2586( _temp2590, _temp2588);}}));}* _temp2458=(
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp2592=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp2592[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp2593; _temp2593.tag= Cyc_Absyn_KnownTunion;
_temp2593.f1= tud; _temp2593;}); _temp2592;}); _temp2568= tud; goto _LL2561;}
_LL2561: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2452);{ struct Cyc_List_List* tvs= _temp2568->tvs; for( 0; _temp2454 != 0?
tvs != 0: 0;( _temp2454=(( struct Cyc_List_List*) _check_null( _temp2454))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2454))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} if( _temp2454
!= 0){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp2594)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp2598= _tag_arr("too many type arguments for tunion %s",
sizeof( unsigned char), 38u); struct Cyc_Stdio_String_pa_struct _temp2599;
_temp2599.tag= Cyc_Stdio_String_pa; _temp2599.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2568->name);{ void* _temp2597=( void*)& _temp2599; void* _temp2595[ 1u]={
_temp2597}; struct _tagged_arr _temp2596={( void*) _temp2595,( void*) _temp2595,(
void*)( _temp2595 + 1u)}; _temp2594( _temp2598, _temp2596);}}));} if( tvs != 0){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp2600)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp2604= _tag_arr("too few type arguments for tunion %s", sizeof(
unsigned char), 37u); struct Cyc_Stdio_String_pa_struct _temp2605; _temp2605.tag=
Cyc_Stdio_String_pa; _temp2605.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2568->name);{ void* _temp2603=( void*)& _temp2605; void* _temp2601[ 1u]={
_temp2603}; struct _tagged_arr _temp2602={( void*) _temp2601,( void*) _temp2601,(
void*)( _temp2601 + 1u)}; _temp2600( _temp2604, _temp2602);}}));} goto _LL2557;}
_LL2557:;} goto _LL2395; _LL2407:{ void* _temp2606=* _temp2465; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp2612; int _temp2614; struct _tuple1* _temp2616; struct _tuple1* _temp2618;
struct Cyc_Absyn_Tunionfield* _temp2620; struct Cyc_Absyn_Tuniondecl* _temp2622;
_LL2608: if(*(( int*) _temp2606) == Cyc_Absyn_UnknownTunionfield){ _LL2613:
_temp2612=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp2606)->f1;
_LL2619: _temp2618= _temp2612.tunion_name; goto _LL2617; _LL2617: _temp2616=
_temp2612.field_name; goto _LL2615; _LL2615: _temp2614= _temp2612.is_xtunion;
goto _LL2609;} else{ goto _LL2610;} _LL2610: if(*(( int*) _temp2606) == Cyc_Absyn_KnownTunionfield){
_LL2623: _temp2622=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp2606)->f1;
goto _LL2621; _LL2621: _temp2620=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2606)->f2; goto _LL2611;} else{ goto _LL2607;} _LL2609: { struct Cyc_Absyn_Tuniondecl*
tud; struct Cyc_Absyn_Tunionfield* tuf;{ struct _handler_cons _temp2624;
_push_handler(& _temp2624);{ int _temp2626= 0; if( setjmp( _temp2624.handler)){
_temp2626= 1;} if( ! _temp2626){* Cyc_Tcenv_lookup_tuniondecl( te, loc,
_temp2618);; _pop_handler();} else{ void* _temp2625=( void*) _exn_thrown; void*
_temp2628= _temp2625; _LL2630: if( _temp2628 == Cyc_Dict_Absent){ goto _LL2631;}
else{ goto _LL2632;} _LL2632: goto _LL2633; _LL2631: Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp2634)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp2638= _tag_arr("unbound type tunion %s",
sizeof( unsigned char), 23u); struct Cyc_Stdio_String_pa_struct _temp2639;
_temp2639.tag= Cyc_Stdio_String_pa; _temp2639.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2618);{ void* _temp2637=( void*)& _temp2639; void* _temp2635[ 1u]={
_temp2637}; struct _tagged_arr _temp2636={( void*) _temp2635,( void*) _temp2635,(
void*)( _temp2635 + 1u)}; _temp2634( _temp2638, _temp2636);}})); return cvtenv;
_LL2633:( void) _throw( _temp2628); _LL2629:;}}}{ struct _handler_cons _temp2640;
_push_handler(& _temp2640);{ int _temp2642= 0; if( setjmp( _temp2640.handler)){
_temp2642= 1;} if( ! _temp2642){{ void* _temp2643= Cyc_Tcenv_lookup_ordinary( te,
loc, _temp2616); struct Cyc_Absyn_Tunionfield* _temp2649; struct Cyc_Absyn_Tuniondecl*
_temp2651; _LL2645: if(*(( int*) _temp2643) == Cyc_Tcenv_TunionRes){ _LL2652:
_temp2651=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2643)->f1; goto _LL2650;
_LL2650: _temp2649=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2643)->f2; goto
_LL2646;} else{ goto _LL2647;} _LL2647: goto _LL2648; _LL2646: tuf= _temp2649;
tud= _temp2651; if( tud->is_xtunion != _temp2614){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp2653)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp2657= _tag_arr("[x]tunion is different from type declaration %s",
sizeof( unsigned char), 48u); struct Cyc_Stdio_String_pa_struct _temp2658;
_temp2658.tag= Cyc_Stdio_String_pa; _temp2658.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2618);{ void* _temp2656=( void*)& _temp2658; void* _temp2654[ 1u]={
_temp2656}; struct _tagged_arr _temp2655={( void*) _temp2654,( void*) _temp2654,(
void*)( _temp2654 + 1u)}; _temp2653( _temp2657, _temp2655);}}));} goto _LL2644;
_LL2648: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp2659)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp2664= _tag_arr("unbound field %s in type tunion %s",
sizeof( unsigned char), 35u); struct Cyc_Stdio_String_pa_struct _temp2666;
_temp2666.tag= Cyc_Stdio_String_pa; _temp2666.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2616);{ void* _temp2662=( void*)& _temp2666; struct Cyc_Stdio_String_pa_struct
_temp2665; _temp2665.tag= Cyc_Stdio_String_pa; _temp2665.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2618);{ void* _temp2663=( void*)& _temp2665; void*
_temp2660[ 2u]={ _temp2662, _temp2663}; struct _tagged_arr _temp2661={( void*)
_temp2660,( void*) _temp2660,( void*)( _temp2660 + 2u)}; _temp2659( _temp2664,
_temp2661);}}}));{ struct Cyc_Tcutil_CVTEnv _temp2667= cvtenv; _npop_handler( 0u);
return _temp2667;} _LL2644:;}; _pop_handler();} else{ void* _temp2641=( void*)
_exn_thrown; void* _temp2669= _temp2641; _LL2671: if( _temp2669 == Cyc_Dict_Absent){
goto _LL2672;} else{ goto _LL2673;} _LL2673: goto _LL2674; _LL2672: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp2675)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp2680=
_tag_arr("unbound field %s in type tunion %s", sizeof( unsigned char), 35u);
struct Cyc_Stdio_String_pa_struct _temp2682; _temp2682.tag= Cyc_Stdio_String_pa;
_temp2682.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2616);{ void*
_temp2678=( void*)& _temp2682; struct Cyc_Stdio_String_pa_struct _temp2681;
_temp2681.tag= Cyc_Stdio_String_pa; _temp2681.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2618);{ void* _temp2679=( void*)& _temp2681; void* _temp2676[ 2u]={
_temp2678, _temp2679}; struct _tagged_arr _temp2677={( void*) _temp2676,( void*)
_temp2676,( void*)( _temp2676 + 2u)}; _temp2675( _temp2680, _temp2677);}}}));
return cvtenv; _LL2674:( void) _throw( _temp2669); _LL2670:;}}}* _temp2465=(
void*)({ struct Cyc_Absyn_KnownTunionfield_struct* _temp2683=( struct Cyc_Absyn_KnownTunionfield_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunionfield_struct)); _temp2683[ 0]=({
struct Cyc_Absyn_KnownTunionfield_struct _temp2684; _temp2684.tag= Cyc_Absyn_KnownTunionfield;
_temp2684.f1= tud; _temp2684.f2= tuf; _temp2684;}); _temp2683;}); _temp2622= tud;
_temp2620= tuf; goto _LL2611;} _LL2611: { struct Cyc_List_List* tvs= _temp2622->tvs;
for( 0; _temp2461 != 0? tvs != 0: 0;( _temp2461=(( struct Cyc_List_List*)
_check_null( _temp2461))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){
void* t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2461))->hd; void*
k1= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te,
cvtenv, k1, t1);} if( _temp2461 != 0){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr(* _temp2685)( struct _tagged_arr fmt, struct _tagged_arr)=
Cyc_Stdio_aprintf; struct _tagged_arr _temp2690= _tag_arr("too many type arguments for tunion %s.%s",
sizeof( unsigned char), 41u); struct Cyc_Stdio_String_pa_struct _temp2692;
_temp2692.tag= Cyc_Stdio_String_pa; _temp2692.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2622->name);{ void* _temp2688=( void*)& _temp2692; struct Cyc_Stdio_String_pa_struct
_temp2691; _temp2691.tag= Cyc_Stdio_String_pa; _temp2691.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2620->name);{ void* _temp2689=( void*)& _temp2691;
void* _temp2686[ 2u]={ _temp2688, _temp2689}; struct _tagged_arr _temp2687={(
void*) _temp2686,( void*) _temp2686,( void*)( _temp2686 + 2u)}; _temp2685(
_temp2690, _temp2687);}}}));} if( tvs != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp2693)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp2698= _tag_arr("too few type arguments for tunion %s.%s",
sizeof( unsigned char), 40u); struct Cyc_Stdio_String_pa_struct _temp2700;
_temp2700.tag= Cyc_Stdio_String_pa; _temp2700.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2622->name);{ void* _temp2696=( void*)& _temp2700; struct Cyc_Stdio_String_pa_struct
_temp2699; _temp2699.tag= Cyc_Stdio_String_pa; _temp2699.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2620->name);{ void* _temp2697=( void*)& _temp2699;
void* _temp2694[ 2u]={ _temp2696, _temp2697}; struct _tagged_arr _temp2695={(
void*) _temp2694,( void*) _temp2694,( void*)( _temp2694 + 2u)}; _temp2693(
_temp2698, _temp2695);}}}));} goto _LL2607;} _LL2607:;} goto _LL2395; _LL2409:
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_AnyKind,
_temp2476); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2474);{ void* _temp2701=( void*)( Cyc_Absyn_compress_conref( _temp2468))->v;
void* _temp2707; struct Cyc_Absyn_Exp* _temp2709; _LL2703: if(( unsigned int)
_temp2701 > 1u?*(( int*) _temp2701) == Cyc_Absyn_Eq_constr: 0){ _LL2708:
_temp2707=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2701)->f1; if((
unsigned int) _temp2707 > 1u?*(( int*) _temp2707) == Cyc_Absyn_Upper_b: 0){
_LL2710: _temp2709=(( struct Cyc_Absyn_Upper_b_struct*) _temp2707)->f1; goto
_LL2704;} else{ goto _LL2705;}} else{ goto _LL2705;} _LL2705: goto _LL2706;
_LL2704: if( ! Cyc_Tcutil_is_const_exp( te, _temp2709)){ Cyc_Tcutil_terr( loc,
_tag_arr("pointer bounds expression is not a constant", sizeof( unsigned char),
44u));} Cyc_Tcexp_tcExp( te, 0, _temp2709); if( ! Cyc_Tcutil_coerce_uint_typ( te,
_temp2709)){ Cyc_Tcutil_terr( loc, _tag_arr("pointer bounds expression is not an unsigned int",
sizeof( unsigned char), 49u));} Cyc_Evexp_eval_const_uint_exp( _temp2709); goto
_LL2702; _LL2706: goto _LL2702; _LL2702:;} goto _LL2395; _LL2411: goto _LL2395;
_LL2413: goto _LL2395; _LL2415: goto _LL2395; _LL2417: cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp2482); if( _temp2478 == 0? 1: !
Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*) _check_null( _temp2478))){
Cyc_Tcutil_terr( loc, _tag_arr("array bounds expression is not constant",
sizeof( unsigned char), 40u));} Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)
_check_null( _temp2478)); if( ! Cyc_Tcutil_coerce_uint_typ( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp2478))){ Cyc_Tcutil_terr( loc, _tag_arr("array bounds expression is not an unsigned int",
sizeof( unsigned char), 47u));} Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp2478)); goto _LL2395; _LL2419: { int num_convs= 0; int
seen_cdecl= 0; int seen_stdcall= 0; int seen_fastcall= 0; int seen_format= 0;
void* ft=( void*) Cyc_Absyn_Printf_ft; int fmt_desc_arg= - 1; int fmt_arg_start=
- 1; for( 0; _temp2486 != 0; _temp2486=(( struct Cyc_List_List*) _check_null(
_temp2486))->tl){ if( ! Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*)
_check_null( _temp2486))->hd)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr(* _temp2711)( struct _tagged_arr fmt, struct _tagged_arr)=
Cyc_Stdio_aprintf; struct _tagged_arr _temp2715= _tag_arr("bad function type attribute %s",
sizeof( unsigned char), 31u); struct Cyc_Stdio_String_pa_struct _temp2716;
_temp2716.tag= Cyc_Stdio_String_pa; _temp2716.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp2486))->hd);{ void* _temp2714=(
void*)& _temp2716; void* _temp2712[ 1u]={ _temp2714}; struct _tagged_arr
_temp2713={( void*) _temp2712,( void*) _temp2712,( void*)( _temp2712 + 1u)};
_temp2711( _temp2715, _temp2713);}}));}{ void* _temp2717=( void*)(( struct Cyc_List_List*)
_check_null( _temp2486))->hd; int _temp2729; int _temp2731; void* _temp2733;
_LL2719: if( _temp2717 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL2720;} else{
goto _LL2721;} _LL2721: if( _temp2717 ==( void*) Cyc_Absyn_Cdecl_att){ goto
_LL2722;} else{ goto _LL2723;} _LL2723: if( _temp2717 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL2724;} else{ goto _LL2725;} _LL2725: if(( unsigned int) _temp2717 > 16u?*((
int*) _temp2717) == Cyc_Absyn_Format_att: 0){ _LL2734: _temp2733=( void*)((
struct Cyc_Absyn_Format_att_struct*) _temp2717)->f1; goto _LL2732; _LL2732:
_temp2731=(( struct Cyc_Absyn_Format_att_struct*) _temp2717)->f2; goto _LL2730;
_LL2730: _temp2729=(( struct Cyc_Absyn_Format_att_struct*) _temp2717)->f3; goto
_LL2726;} else{ goto _LL2727;} _LL2727: goto _LL2728; _LL2720: if( !
seen_stdcall){ seen_stdcall= 1; ++ num_convs;} goto _LL2718; _LL2722: if( !
seen_cdecl){ seen_cdecl= 1; ++ num_convs;} goto _LL2718; _LL2724: if( !
seen_fastcall){ seen_fastcall= 1; ++ num_convs;} goto _LL2718; _LL2726: if( !
seen_format){ seen_format= 1; ft= _temp2733; fmt_desc_arg= _temp2731;
fmt_arg_start= _temp2729;} else{ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr(* _temp2735)( struct _tagged_arr fmt, struct _tagged_arr)=
Cyc_Stdio_aprintf; struct _tagged_arr _temp2738= _tag_arr("function can't have multiple format attributes",
sizeof( unsigned char), 47u); void* _temp2736[ 0u]={}; struct _tagged_arr
_temp2737={( void*) _temp2736,( void*) _temp2736,( void*)( _temp2736 + 0u)};
_temp2735( _temp2738, _temp2737);}));} goto _LL2718; _LL2728: goto _LL2718;
_LL2718:;}} if( num_convs > 1){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr(* _temp2739)( struct _tagged_arr fmt, struct _tagged_arr)=
Cyc_Stdio_aprintf; struct _tagged_arr _temp2742= _tag_arr("function can't have multiple calling conventions",
sizeof( unsigned char), 49u); void* _temp2740[ 0u]={}; struct _tagged_arr
_temp2741={( void*) _temp2740,( void*) _temp2740,( void*)( _temp2740 + 0u)};
_temp2739( _temp2742, _temp2741);}));} Cyc_Tcutil_check_unique_tvars( loc,*
_temp2503);{ struct Cyc_List_List* b=* _temp2503; for( 0; b != 0; b=(( struct
Cyc_List_List*) _check_null( b))->tl){(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( b))->hd)->identity= Cyc_Tcutil_new_tvar_id(); cvtenv.kind_env= Cyc_Tcutil_add_bound_tvar(
cvtenv.kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
b))->hd);{ void* _temp2743=( void*)( Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( b))->hd)->kind))->v; void* _temp2749;
_LL2745: if(( unsigned int) _temp2743 > 1u?*(( int*) _temp2743) == Cyc_Absyn_Eq_constr:
0){ _LL2750: _temp2749=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2743)->f1;
if( _temp2749 ==( void*) Cyc_Absyn_MemKind){ goto _LL2746;} else{ goto _LL2747;}}
else{ goto _LL2747;} _LL2747: goto _LL2748; _LL2746: Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp2751)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp2755= _tag_arr("function attempts to abstract Mem type variable %s",
sizeof( unsigned char), 51u); struct Cyc_Stdio_String_pa_struct _temp2756;
_temp2756.tag= Cyc_Stdio_String_pa; _temp2756.f1=( struct _tagged_arr)*(( struct
Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd)->name;{ void*
_temp2754=( void*)& _temp2756; void* _temp2752[ 1u]={ _temp2754}; struct
_tagged_arr _temp2753={( void*) _temp2752,( void*) _temp2752,( void*)( _temp2752
+ 1u)}; _temp2751( _temp2755, _temp2753);}})); goto _LL2744; _LL2748: goto
_LL2744; _LL2744:;}}}{ struct Cyc_Tcutil_CVTEnv _temp2757=({ struct Cyc_Tcutil_CVTEnv
_temp2929; _temp2929.kind_env= cvtenv.kind_env; _temp2929.free_vars= 0;
_temp2929.free_evars= 0; _temp2929;}); _temp2757= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2757,( void*) Cyc_Absyn_MemKind, _temp2496);{ struct Cyc_List_List*
a= _temp2494; for( 0; a != 0; a=(( struct Cyc_List_List*) _check_null( a))->tl){
_temp2757= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2757,( void*) Cyc_Absyn_MemKind,(*((
struct _tuple2*)(( struct Cyc_List_List*) _check_null( a))->hd)).f3);}} if(
_temp2490 != 0){ if( _temp2492){(( int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)(
_tag_arr("both c_vararg and cyc_vararg", sizeof( unsigned char), 29u));}{ int
_temp2760; void* _temp2762; void* _temp2764; struct Cyc_Absyn_Tqual _temp2766;
struct Cyc_Core_Opt* _temp2768; struct Cyc_Absyn_VarargInfo _temp2758=*(( struct
Cyc_Absyn_VarargInfo*) _check_null( _temp2490)); _LL2769: _temp2768= _temp2758.name;
goto _LL2767; _LL2767: _temp2766= _temp2758.tq; goto _LL2765; _LL2765: _temp2764=(
void*) _temp2758.type; goto _LL2763; _LL2763: _temp2762=( void*) _temp2758.rgn;
goto _LL2761; _LL2761: _temp2760= _temp2758.inject; goto _LL2759; _LL2759:
_temp2757= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2757,( void*) Cyc_Absyn_MemKind,
_temp2764);{ void* _temp2770= Cyc_Tcutil_compress( _temp2762); _LL2772: if((
unsigned int) _temp2770 > 4u?*(( int*) _temp2770) == Cyc_Absyn_Evar: 0){ goto
_LL2773;} else{ goto _LL2774;} _LL2774: if(( unsigned int) _temp2770 > 4u?*((
int*) _temp2770) == Cyc_Absyn_VarType: 0){ goto _LL2775;} else{ goto _LL2776;}
_LL2776: if( _temp2770 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL2777;} else{ goto
_LL2778;} _LL2778: goto _LL2779; _LL2773: { void* vr_tvar=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2780=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2780[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2781; _temp2781.tag= Cyc_Absyn_VarType;
_temp2781.f1= Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_RgnKind); _temp2781;});
_temp2780;}); Cyc_Tcutil_unify( _temp2762, vr_tvar); goto _LL2771;} _LL2775:
goto _LL2771; _LL2777: Cyc_Tcutil_terr( loc, _tag_arr("varargs should be in a region, not heap-allocated",
sizeof( unsigned char), 50u)); goto _LL2771; _LL2779: goto _LL2771; _LL2771:;}
_temp2757= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2757,( void*) Cyc_Absyn_RgnKind,
_temp2762); if( _temp2760){ void* _temp2782= Cyc_Tcutil_compress( _temp2764);
struct Cyc_Absyn_TunionInfo _temp2788; void* _temp2790; void* _temp2792; _LL2784:
if(( unsigned int) _temp2782 > 4u?*(( int*) _temp2782) == Cyc_Absyn_TunionType:
0){ _LL2789: _temp2788=(( struct Cyc_Absyn_TunionType_struct*) _temp2782)->f1;
_LL2793: _temp2792=( void*) _temp2788.tunion_info; if(*(( int*) _temp2792) ==
Cyc_Absyn_KnownTunion){ goto _LL2791;} else{ goto _LL2786;} _LL2791: _temp2790=(
void*) _temp2788.rgn; goto _LL2785;} else{ goto _LL2786;} _LL2786: goto _LL2787;
_LL2785:{ void* _temp2794= Cyc_Tcutil_compress( _temp2790); _LL2796: if((
unsigned int) _temp2794 > 4u?*(( int*) _temp2794) == Cyc_Absyn_Evar: 0){ goto
_LL2797;} else{ goto _LL2798;} _LL2798: if(( unsigned int) _temp2794 > 4u?*((
int*) _temp2794) == Cyc_Absyn_VarType: 0){ goto _LL2799;} else{ goto _LL2800;}
_LL2800: if( _temp2794 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL2801;} else{ goto
_LL2802;} _LL2802: goto _LL2803; _LL2797: Cyc_Tcutil_unify( _temp2762, _temp2790);
goto _LL2795; _LL2799: goto _LL2795; _LL2801: Cyc_Tcutil_terr( loc, _tag_arr("injected vararg [x]tunion should be in a region, not heap-allocated",
sizeof( unsigned char), 68u)); goto _LL2795; _LL2803: goto _LL2795; _LL2795:;}
goto _LL2783; _LL2787: Cyc_Tcutil_terr( loc, _tag_arr("can't inject a non-[x]tunion type",
sizeof( unsigned char), 34u)); goto _LL2783; _LL2783:;}}} if( seen_format){ int
_temp2804=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2494); if((((
fmt_desc_arg < 0? 1: fmt_desc_arg > _temp2804)? 1: fmt_arg_start < 0)? 1:(
_temp2490 == 0? fmt_arg_start != 0: 0))? 1:( _temp2490 != 0? fmt_arg_start !=
_temp2804 + 1: 0)){ Cyc_Tcutil_terr( loc, _tag_arr("bad format descriptor",
sizeof( unsigned char), 22u));} else{ void* _temp2807; struct _tuple2 _temp2805=*((
struct _tuple2*(*)( struct Cyc_List_List* x, int i)) Cyc_List_nth)( _temp2494,
fmt_desc_arg - 1); _LL2808: _temp2807= _temp2805.f3; goto _LL2806; _LL2806:{
void* _temp2809= Cyc_Tcutil_compress( _temp2807); struct Cyc_Absyn_PtrInfo
_temp2815; struct Cyc_Absyn_Conref* _temp2817; void* _temp2819; _LL2811: if((
unsigned int) _temp2809 > 4u?*(( int*) _temp2809) == Cyc_Absyn_PointerType: 0){
_LL2816: _temp2815=(( struct Cyc_Absyn_PointerType_struct*) _temp2809)->f1;
_LL2820: _temp2819=( void*) _temp2815.elt_typ; goto _LL2818; _LL2818: _temp2817=
_temp2815.bounds; goto _LL2812;} else{ goto _LL2813;} _LL2813: goto _LL2814;
_LL2812:{ struct _tuple8 _temp2822=({ struct _tuple8 _temp2821; _temp2821.f1=
Cyc_Tcutil_compress( _temp2819); _temp2821.f2= Cyc_Absyn_conref_def(( void*) Cyc_Absyn_Unknown_b,
_temp2817); _temp2821;}); void* _temp2828; void* _temp2830; void* _temp2832;
void* _temp2834; _LL2824: _LL2831: _temp2830= _temp2822.f1; if(( unsigned int)
_temp2830 > 4u?*(( int*) _temp2830) == Cyc_Absyn_IntType: 0){ _LL2835: _temp2834=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2830)->f1; if( _temp2834 ==(
void*) Cyc_Absyn_Unsigned){ goto _LL2833;} else{ goto _LL2826;} _LL2833:
_temp2832=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2830)->f2; if(
_temp2832 ==( void*) Cyc_Absyn_B1){ goto _LL2829;} else{ goto _LL2826;}} else{
goto _LL2826;} _LL2829: _temp2828= _temp2822.f2; if( _temp2828 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL2825;} else{ goto _LL2826;} _LL2826: goto _LL2827; _LL2825: goto _LL2823;
_LL2827: Cyc_Tcutil_terr( loc, _tag_arr("format descriptor is not a char ? type",
sizeof( unsigned char), 39u)); goto _LL2823; _LL2823:;} goto _LL2810; _LL2814:
Cyc_Tcutil_terr( loc, _tag_arr("format descriptor is not a char ? type", sizeof(
unsigned char), 39u)); goto _LL2810; _LL2810:;} if( fmt_arg_start != 0){ void*
_temp2836= Cyc_Tcutil_compress(( void*)(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp2490))->type); int _temp2837= 0;{ void* _temp2838= ft; _LL2840:
if( _temp2838 ==( void*) Cyc_Absyn_Printf_ft){ goto _LL2841;} else{ goto _LL2842;}
_LL2842: if( _temp2838 ==( void*) Cyc_Absyn_Scanf_ft){ goto _LL2843;} else{ goto
_LL2839;} _LL2841:{ void* _temp2844= _temp2836; struct Cyc_Absyn_TunionInfo
_temp2850; void* _temp2852; struct Cyc_Absyn_Tuniondecl* _temp2854; _LL2846: if((
unsigned int) _temp2844 > 4u?*(( int*) _temp2844) == Cyc_Absyn_TunionType: 0){
_LL2851: _temp2850=(( struct Cyc_Absyn_TunionType_struct*) _temp2844)->f1;
_LL2853: _temp2852=( void*) _temp2850.tunion_info; if(*(( int*) _temp2852) ==
Cyc_Absyn_KnownTunion){ _LL2855: _temp2854=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp2852)->f1; goto _LL2847;} else{ goto _LL2848;}} else{ goto _LL2848;}
_LL2848: goto _LL2849; _LL2847: _temp2837= Cyc_Absyn_qvar_cmp( _temp2854->name,
Cyc_Absyn_tunion_print_arg_qvar) != 0; goto _LL2845; _LL2849: _temp2837= 1; goto
_LL2845; _LL2845:;} goto _LL2839; _LL2843:{ void* _temp2856= _temp2836; struct
Cyc_Absyn_TunionInfo _temp2862; void* _temp2864; struct Cyc_Absyn_Tuniondecl*
_temp2866; _LL2858: if(( unsigned int) _temp2856 > 4u?*(( int*) _temp2856) ==
Cyc_Absyn_TunionType: 0){ _LL2863: _temp2862=(( struct Cyc_Absyn_TunionType_struct*)
_temp2856)->f1; _LL2865: _temp2864=( void*) _temp2862.tunion_info; if(*(( int*)
_temp2864) == Cyc_Absyn_KnownTunion){ _LL2867: _temp2866=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp2864)->f1; goto _LL2859;} else{ goto _LL2860;}} else{ goto _LL2860;}
_LL2860: goto _LL2861; _LL2859: _temp2837= Cyc_Absyn_qvar_cmp( _temp2866->name,
Cyc_Absyn_tunion_scanf_arg_qvar) != 0; goto _LL2857; _LL2861: _temp2837= 1; goto
_LL2857; _LL2857:;} goto _LL2839; _LL2839:;} if( _temp2837){ Cyc_Tcutil_terr(
loc, _tag_arr("format attribute and vararg types don't match", sizeof(
unsigned char), 46u));}}}}{ struct Cyc_List_List* rpo= _temp2488; for( 0; rpo !=
0; rpo=(( struct Cyc_List_List*) _check_null( rpo))->tl){ struct _tuple8
_temp2870; void* _temp2871; void* _temp2873; struct _tuple8* _temp2868=( struct
_tuple8*)(( struct Cyc_List_List*) _check_null( rpo))->hd; _temp2870=* _temp2868;
_LL2874: _temp2873= _temp2870.f1; goto _LL2872; _LL2872: _temp2871= _temp2870.f2;
goto _LL2869; _LL2869: _temp2757= Cyc_Tcutil_i_check_valid_type( loc, te,
_temp2757,( void*) Cyc_Absyn_RgnKind, _temp2873); _temp2757= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2757,( void*) Cyc_Absyn_RgnKind, _temp2871);}} if(* _temp2500 != 0){
_temp2757= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2757,( void*) Cyc_Absyn_EffKind,(
void*)(( struct Cyc_Core_Opt*) _check_null(* _temp2500))->v);} else{ struct Cyc_Absyn_Tvar*
e= Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_EffKind); _temp2757.free_vars= Cyc_Tcutil_add_free_tvar(
loc, _temp2757.free_vars, e);{ struct Cyc_List_List* effect= 0;{ struct Cyc_List_List*
tvs= _temp2757.free_vars; for( 0; tvs != 0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){ void* _temp2875=( void*)( Cyc_Absyn_compress_conref(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind))->v;
void* _temp2883; void* _temp2885; _LL2877: if(( unsigned int) _temp2875 > 1u?*((
int*) _temp2875) == Cyc_Absyn_Eq_constr: 0){ _LL2884: _temp2883=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2875)->f1; if( _temp2883 ==( void*) Cyc_Absyn_RgnKind){
goto _LL2878;} else{ goto _LL2879;}} else{ goto _LL2879;} _LL2879: if((
unsigned int) _temp2875 > 1u?*(( int*) _temp2875) == Cyc_Absyn_Eq_constr: 0){
_LL2886: _temp2885=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2875)->f1;
if( _temp2885 ==( void*) Cyc_Absyn_EffKind){ goto _LL2880;} else{ goto _LL2881;}}
else{ goto _LL2881;} _LL2881: goto _LL2882; _LL2878: effect=({ struct Cyc_List_List*
_temp2887=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2887->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2888=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2888[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2889; _temp2889.tag= Cyc_Absyn_AccessEff;
_temp2889.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp2890=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2890[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2891; _temp2891.tag= Cyc_Absyn_VarType;
_temp2891.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp2891;}); _temp2890;})); _temp2889;}); _temp2888;})); _temp2887->tl= effect;
_temp2887;}); goto _LL2876; _LL2880: effect=({ struct Cyc_List_List* _temp2892=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2892->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp2893=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2893[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2894; _temp2894.tag= Cyc_Absyn_VarType; _temp2894.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp2894;}); _temp2893;})); _temp2892->tl= effect; _temp2892;}); goto _LL2876;
_LL2882: goto _LL2876; _LL2876:;}}{ struct Cyc_List_List* ts= _temp2757.free_evars;
for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){ void*
_temp2895= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);
_LL2897: if( _temp2895 ==( void*) Cyc_Absyn_RgnKind){ goto _LL2898;} else{ goto
_LL2899;} _LL2899: if( _temp2895 ==( void*) Cyc_Absyn_EffKind){ goto _LL2900;}
else{ goto _LL2901;} _LL2901: goto _LL2902; _LL2898: effect=({ struct Cyc_List_List*
_temp2903=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2903->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2904=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2904[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2905; _temp2905.tag= Cyc_Absyn_AccessEff;
_temp2905.f1=( void*)(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);
_temp2905;}); _temp2904;})); _temp2903->tl= effect; _temp2903;}); goto _LL2896;
_LL2900: effect=({ struct Cyc_List_List* _temp2906=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2906->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( ts))->hd); _temp2906->tl= effect; _temp2906;});
goto _LL2896; _LL2902: goto _LL2896; _LL2896:;}}* _temp2500=({ struct Cyc_Core_Opt*
_temp2907=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2907->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2908=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2908[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2909; _temp2909.tag= Cyc_Absyn_JoinEff;
_temp2909.f1= Cyc_List_imp_rev( effect); _temp2909;}); _temp2908;})); _temp2907;});}}
if(* _temp2503 != 0){ struct Cyc_List_List* bs=* _temp2503; for( 0; bs != 0; bs=((
struct Cyc_List_List*) _check_null( bs))->tl){ struct Cyc_Absyn_Conref*
_temp2910= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( bs))->hd)->kind); void* _temp2911=( void*) _temp2910->v; void*
_temp2919; _LL2913: if( _temp2911 ==( void*) Cyc_Absyn_No_constr){ goto _LL2914;}
else{ goto _LL2915;} _LL2915: if(( unsigned int) _temp2911 > 1u?*(( int*)
_temp2911) == Cyc_Absyn_Eq_constr: 0){ _LL2920: _temp2919=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2911)->f1; if( _temp2919 ==( void*) Cyc_Absyn_MemKind){ goto _LL2916;}
else{ goto _LL2917;}} else{ goto _LL2917;} _LL2917: goto _LL2918; _LL2914: Cyc_Tcutil_warn(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp2921)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp2925=
_tag_arr("Type variable %s unconstrained, assuming boxed", sizeof( unsigned char),
47u); struct Cyc_Stdio_String_pa_struct _temp2926; _temp2926.tag= Cyc_Stdio_String_pa;
_temp2926.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( bs))->hd)->name;{ void* _temp2924=( void*)& _temp2926; void*
_temp2922[ 1u]={ _temp2924}; struct _tagged_arr _temp2923={( void*) _temp2922,(
void*) _temp2922,( void*)( _temp2922 + 1u)}; _temp2921( _temp2925, _temp2923);}}));
goto _LL2916; _LL2916:( void*)( _temp2910->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2927=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2927[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2928; _temp2928.tag= Cyc_Absyn_Eq_constr;
_temp2928.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp2928;}); _temp2927;})));
goto _LL2912; _LL2918: goto _LL2912; _LL2912:;}} cvtenv.kind_env= Cyc_Tcutil_remove_bound_tvars(
_temp2757.kind_env,* _temp2503); _temp2757.free_vars= Cyc_Tcutil_remove_bound_tvars(
_temp2757.free_vars,* _temp2503);{ struct Cyc_List_List* tvs= _temp2757.free_vars;
for( 0; tvs != 0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd);}}{ struct Cyc_List_List* evs=
_temp2757.free_evars; for( 0; evs != 0; evs=(( struct Cyc_List_List*)
_check_null( evs))->tl){ cvtenv.free_evars= Cyc_Tcutil_add_free_evar( cvtenv.free_evars,(
void*)(( struct Cyc_List_List*) _check_null( evs))->hd);}} goto _LL2395;}}
_LL2421: for( 0; _temp2504 != 0; _temp2504=(( struct Cyc_List_List*) _check_null(
_temp2504))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*)
Cyc_Absyn_MemKind,(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp2504))->hd)).f2);} goto _LL2395; _LL2423:{ struct _RegionHandle _temp2930=
_new_region(); struct _RegionHandle* sprev_rgn=& _temp2930; _push_region(
sprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for( 0; _temp2506 != 0;
_temp2506=(( struct Cyc_List_List*) _check_null( _temp2506))->tl){ struct Cyc_Absyn_Structfield
_temp2933; struct Cyc_List_List* _temp2934; struct Cyc_Core_Opt* _temp2936; void*
_temp2938; struct Cyc_Absyn_Tqual _temp2940; struct _tagged_arr* _temp2942;
struct Cyc_Absyn_Structfield* _temp2931=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2506))->hd; _temp2933=* _temp2931;
_LL2943: _temp2942= _temp2933.name; goto _LL2941; _LL2941: _temp2940= _temp2933.tq;
goto _LL2939; _LL2939: _temp2938=( void*) _temp2933.type; goto _LL2937; _LL2937:
_temp2936= _temp2933.width; goto _LL2935; _LL2935: _temp2934= _temp2933.attributes;
goto _LL2932; _LL2932: if((( int(*)( int(* compare)( struct _tagged_arr*, struct
_tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields, _temp2942)){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp2944)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp2948= _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u); struct Cyc_Stdio_String_pa_struct _temp2949;
_temp2949.tag= Cyc_Stdio_String_pa; _temp2949.f1=( struct _tagged_arr)*
_temp2942;{ void* _temp2947=( void*)& _temp2949; void* _temp2945[ 1u]={
_temp2947}; struct _tagged_arr _temp2946={( void*) _temp2945,( void*) _temp2945,(
void*)( _temp2945 + 1u)}; _temp2944( _temp2948, _temp2946);}}));} if( Cyc_String_strcmp(*
_temp2942, _tag_arr("", sizeof( unsigned char), 1u)) != 0){ prev_fields=({
struct Cyc_List_List* _temp2950=( struct Cyc_List_List*) _region_malloc(
sprev_rgn, sizeof( struct Cyc_List_List)); _temp2950->hd=( void*) _temp2942;
_temp2950->tl= prev_fields; _temp2950;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp2938); if( _temp2936 != 0){
unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp2936))->v)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp2951)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp2955= _tag_arr("bitfield %s does not have constant width",
sizeof( unsigned char), 41u); struct Cyc_Stdio_String_pa_struct _temp2956;
_temp2956.tag= Cyc_Stdio_String_pa; _temp2956.f1=( struct _tagged_arr)*
_temp2942;{ void* _temp2954=( void*)& _temp2956; void* _temp2952[ 1u]={
_temp2954}; struct _tagged_arr _temp2953={( void*) _temp2952,( void*) _temp2952,(
void*)( _temp2952 + 1u)}; _temp2951( _temp2955, _temp2953);}}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp2936))->v);}{
void* _temp2957= Cyc_Tcutil_compress( _temp2938); void* _temp2963; _LL2959: if((
unsigned int) _temp2957 > 4u?*(( int*) _temp2957) == Cyc_Absyn_IntType: 0){
_LL2964: _temp2963=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2957)->f2;
goto _LL2960;} else{ goto _LL2961;} _LL2961: goto _LL2962; _LL2960:{ void*
_temp2965= _temp2963; _LL2967: if( _temp2965 ==( void*) Cyc_Absyn_B1){ goto
_LL2968;} else{ goto _LL2969;} _LL2969: if( _temp2965 ==( void*) Cyc_Absyn_B2){
goto _LL2970;} else{ goto _LL2971;} _LL2971: if( _temp2965 ==( void*) Cyc_Absyn_B4){
goto _LL2972;} else{ goto _LL2973;} _LL2973: if( _temp2965 ==( void*) Cyc_Absyn_B8){
goto _LL2974;} else{ goto _LL2966;} _LL2968: if( w > 8){ Cyc_Tcutil_terr( loc,
_tag_arr("bitfield larger than type", sizeof( unsigned char), 26u));} goto
_LL2966; _LL2970: if( w > 16){ Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u));} goto _LL2966; _LL2972: if( w > 32){ Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u));} goto
_LL2966; _LL2974: if( w > 64){ Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u));} goto _LL2966; _LL2966:;} goto _LL2958; _LL2962:
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp2975)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp2980= _tag_arr("bitfield %s must have integral type but has type %s",
sizeof( unsigned char), 52u); struct Cyc_Stdio_String_pa_struct _temp2982;
_temp2982.tag= Cyc_Stdio_String_pa; _temp2982.f1=( struct _tagged_arr)*
_temp2942;{ void* _temp2978=( void*)& _temp2982; struct Cyc_Stdio_String_pa_struct
_temp2981; _temp2981.tag= Cyc_Stdio_String_pa; _temp2981.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( _temp2938);{ void* _temp2979=( void*)& _temp2981; void*
_temp2976[ 2u]={ _temp2978, _temp2979}; struct _tagged_arr _temp2977={( void*)
_temp2976,( void*) _temp2976,( void*)( _temp2976 + 2u)}; _temp2975( _temp2980,
_temp2977);}}})); goto _LL2958; _LL2958:;}} for( 0; _temp2934 != 0; _temp2934=((
struct Cyc_List_List*) _check_null( _temp2934))->tl){ void* _temp2983=( void*)((
struct Cyc_List_List*) _check_null( _temp2934))->hd; _LL2985: if( _temp2983 ==(
void*) Cyc_Absyn_Packed_att){ goto _LL2986;} else{ goto _LL2987;} _LL2987: if((
unsigned int) _temp2983 > 16u?*(( int*) _temp2983) == Cyc_Absyn_Aligned_att: 0){
goto _LL2988;} else{ goto _LL2989;} _LL2989: goto _LL2990; _LL2986: continue;
_LL2988: continue; _LL2990: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp2991)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp2996= _tag_arr("bad attribute %s on struct member %s",
sizeof( unsigned char), 37u); struct Cyc_Stdio_String_pa_struct _temp2998;
_temp2998.tag= Cyc_Stdio_String_pa; _temp2998.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp2934))->hd);{ void* _temp2994=(
void*)& _temp2998; struct Cyc_Stdio_String_pa_struct _temp2997; _temp2997.tag=
Cyc_Stdio_String_pa; _temp2997.f1=( struct _tagged_arr)* _temp2942;{ void*
_temp2995=( void*)& _temp2997; void* _temp2992[ 2u]={ _temp2994, _temp2995};
struct _tagged_arr _temp2993={( void*) _temp2992,( void*) _temp2992,( void*)(
_temp2992 + 2u)}; _temp2991( _temp2996, _temp2993);}}})); goto _LL2984; _LL2984:;}}};
_pop_region( sprev_rgn);} goto _LL2395; _LL2425:{ struct _RegionHandle _temp2999=
_new_region(); struct _RegionHandle* uprev_rgn=& _temp2999; _push_region(
uprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for( 0; _temp2508 != 0;
_temp2508=(( struct Cyc_List_List*) _check_null( _temp2508))->tl){ struct Cyc_Absyn_Structfield
_temp3002; struct Cyc_List_List* _temp3003; struct Cyc_Core_Opt* _temp3005; void*
_temp3007; struct Cyc_Absyn_Tqual _temp3009; struct _tagged_arr* _temp3011;
struct Cyc_Absyn_Structfield* _temp3000=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2508))->hd; _temp3002=* _temp3000;
_LL3012: _temp3011= _temp3002.name; goto _LL3010; _LL3010: _temp3009= _temp3002.tq;
goto _LL3008; _LL3008: _temp3007=( void*) _temp3002.type; goto _LL3006; _LL3006:
_temp3005= _temp3002.width; goto _LL3004; _LL3004: _temp3003= _temp3002.attributes;
goto _LL3001; _LL3001: if((( int(*)( int(* compare)( struct _tagged_arr*, struct
_tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields, _temp3011)){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp3013)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp3017= _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u); struct Cyc_Stdio_String_pa_struct _temp3018;
_temp3018.tag= Cyc_Stdio_String_pa; _temp3018.f1=( struct _tagged_arr)*
_temp3011;{ void* _temp3016=( void*)& _temp3018; void* _temp3014[ 1u]={
_temp3016}; struct _tagged_arr _temp3015={( void*) _temp3014,( void*) _temp3014,(
void*)( _temp3014 + 1u)}; _temp3013( _temp3017, _temp3015);}}));} if( Cyc_String_strcmp(*
_temp3011, _tag_arr("", sizeof( unsigned char), 1u)) != 0){ prev_fields=({
struct Cyc_List_List* _temp3019=( struct Cyc_List_List*) _region_malloc(
uprev_rgn, sizeof( struct Cyc_List_List)); _temp3019->hd=( void*) _temp3011;
_temp3019->tl= prev_fields; _temp3019;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp3007); if( ! Cyc_Tcutil_bits_only(
_temp3007)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp3020)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp3024= _tag_arr("%s has a type that is possibly incompatible with other members of union",
sizeof( unsigned char), 72u); struct Cyc_Stdio_String_pa_struct _temp3025;
_temp3025.tag= Cyc_Stdio_String_pa; _temp3025.f1=( struct _tagged_arr)*
_temp3011;{ void* _temp3023=( void*)& _temp3025; void* _temp3021[ 1u]={
_temp3023}; struct _tagged_arr _temp3022={( void*) _temp3021,( void*) _temp3021,(
void*)( _temp3021 + 1u)}; _temp3020( _temp3024, _temp3022);}}));} if( _temp3005
!= 0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp3005))->v)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr(* _temp3026)( struct _tagged_arr fmt,
struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp3030= _tag_arr("bitfield %s does not have constant width",
sizeof( unsigned char), 41u); struct Cyc_Stdio_String_pa_struct _temp3031;
_temp3031.tag= Cyc_Stdio_String_pa; _temp3031.f1=( struct _tagged_arr)*
_temp3011;{ void* _temp3029=( void*)& _temp3031; void* _temp3027[ 1u]={
_temp3029}; struct _tagged_arr _temp3028={( void*) _temp3027,( void*) _temp3027,(
void*)( _temp3027 + 1u)}; _temp3026( _temp3030, _temp3028);}}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp3005))->v);}{
void* _temp3032= Cyc_Tcutil_compress( _temp3007); void* _temp3038; _LL3034: if((
unsigned int) _temp3032 > 4u?*(( int*) _temp3032) == Cyc_Absyn_IntType: 0){
_LL3039: _temp3038=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3032)->f2;
goto _LL3035;} else{ goto _LL3036;} _LL3036: goto _LL3037; _LL3035:{ void*
_temp3040= _temp3038; _LL3042: if( _temp3040 ==( void*) Cyc_Absyn_B1){ goto
_LL3043;} else{ goto _LL3044;} _LL3044: if( _temp3040 ==( void*) Cyc_Absyn_B2){
goto _LL3045;} else{ goto _LL3046;} _LL3046: if( _temp3040 ==( void*) Cyc_Absyn_B4){
goto _LL3047;} else{ goto _LL3048;} _LL3048: if( _temp3040 ==( void*) Cyc_Absyn_B8){
goto _LL3049;} else{ goto _LL3041;} _LL3043: if( w > 8){ Cyc_Tcutil_terr( loc,
_tag_arr("bitfield larger than type", sizeof( unsigned char), 26u));} goto
_LL3041; _LL3045: if( w > 16){ Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u));} goto _LL3041; _LL3047: if( w > 32){ Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u));} goto
_LL3041; _LL3049: if( w > 64){ Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u));} goto _LL3041; _LL3041:;} goto _LL3033; _LL3037:
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp3050)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp3055= _tag_arr("bitfield %s must have integral type but has type %s",
sizeof( unsigned char), 52u); struct Cyc_Stdio_String_pa_struct _temp3057;
_temp3057.tag= Cyc_Stdio_String_pa; _temp3057.f1=( struct _tagged_arr)*
_temp3011;{ void* _temp3053=( void*)& _temp3057; struct Cyc_Stdio_String_pa_struct
_temp3056; _temp3056.tag= Cyc_Stdio_String_pa; _temp3056.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( _temp3007);{ void* _temp3054=( void*)& _temp3056; void*
_temp3051[ 2u]={ _temp3053, _temp3054}; struct _tagged_arr _temp3052={( void*)
_temp3051,( void*) _temp3051,( void*)( _temp3051 + 2u)}; _temp3050( _temp3055,
_temp3052);}}})); goto _LL3033; _LL3033:;}} for( 0; _temp3003 != 0; _temp3003=((
struct Cyc_List_List*) _check_null( _temp3003))->tl){ void* _temp3058=( void*)((
struct Cyc_List_List*) _check_null( _temp3003))->hd; _LL3060: if( _temp3058 ==(
void*) Cyc_Absyn_Packed_att){ goto _LL3061;} else{ goto _LL3062;} _LL3062: if((
unsigned int) _temp3058 > 16u?*(( int*) _temp3058) == Cyc_Absyn_Aligned_att: 0){
goto _LL3063;} else{ goto _LL3064;} _LL3064: goto _LL3065; _LL3061: continue;
_LL3063: continue; _LL3065: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp3066)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp3071= _tag_arr("bad attribute %s on struct member %s",
sizeof( unsigned char), 37u); struct Cyc_Stdio_String_pa_struct _temp3073;
_temp3073.tag= Cyc_Stdio_String_pa; _temp3073.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp3003))->hd);{ void* _temp3069=(
void*)& _temp3073; struct Cyc_Stdio_String_pa_struct _temp3072; _temp3072.tag=
Cyc_Stdio_String_pa; _temp3072.f1=( struct _tagged_arr)* _temp3011;{ void*
_temp3070=( void*)& _temp3072; void* _temp3067[ 2u]={ _temp3069, _temp3070};
struct _tagged_arr _temp3068={( void*) _temp3067,( void*) _temp3067,( void*)(
_temp3067 + 2u)}; _temp3066( _temp3071, _temp3068);}}})); goto _LL3059; _LL3059:;}}};
_pop_region( uprev_rgn);} goto _LL2395; _LL2427: if( _temp2515 == 0){ if(*
_temp2512 == 0){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp3074)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp3077= _tag_arr("bad anonymous struct", sizeof(
unsigned char), 21u); void* _temp3075[ 0u]={}; struct _tagged_arr _temp3076={(
void*) _temp3075,( void*) _temp3075,( void*)( _temp3075 + 0u)}; _temp3074(
_temp3077, _temp3076);})); return cvtenv;}{ struct Cyc_Absyn_Structdecl* sd=*((
struct Cyc_Absyn_Structdecl**) _check_null(* _temp2512));;}} if(* _temp2512 == 0){
struct _handler_cons _temp3078; _push_handler(& _temp3078);{ int _temp3080= 0;
if( setjmp( _temp3078.handler)){ _temp3080= 1;} if( ! _temp3080){* _temp2512=(
struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl( te, loc,( struct
_tuple1*) _check_null( _temp2515));; _pop_handler();} else{ void* _temp3079=(
void*) _exn_thrown; void* _temp3082= _temp3079; _LL3084: if( _temp3082 == Cyc_Dict_Absent){
goto _LL3085;} else{ goto _LL3086;} _LL3086: goto _LL3087; _LL3085: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp3088)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp3092=
_tag_arr("unbound type struct %s", sizeof( unsigned char), 23u); struct Cyc_Stdio_String_pa_struct
_temp3093; _temp3093.tag= Cyc_Stdio_String_pa; _temp3093.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp2515));{ void*
_temp3091=( void*)& _temp3093; void* _temp3089[ 1u]={ _temp3091}; struct
_tagged_arr _temp3090={( void*) _temp3089,( void*) _temp3089,( void*)( _temp3089
+ 1u)}; _temp3088( _temp3092, _temp3090);}})); return cvtenv; _LL3087:( void)
_throw( _temp3082); _LL3083:;}}}{ struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**)
_check_null(* _temp2512)); if( sd->name != 0){*(( struct _tuple1*) _check_null(
_temp2515))=(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)[
_check_known_subscript_notnull( 1u, 0)];}{ struct Cyc_List_List* tvs= sd->tvs;
int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=
Cyc_List_length( _temp2513); if( lvs != largs){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp3094)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp3100= _tag_arr("struct %s expects %d type arguments but was given %d",
sizeof( unsigned char), 53u); struct Cyc_Stdio_String_pa_struct _temp3103;
_temp3103.tag= Cyc_Stdio_String_pa; _temp3103.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp2515));{ void* _temp3097=( void*)& _temp3103;
struct Cyc_Stdio_Int_pa_struct _temp3102; _temp3102.tag= Cyc_Stdio_Int_pa;
_temp3102.f1=( int)(( unsigned int) lvs);{ void* _temp3098=( void*)& _temp3102;
struct Cyc_Stdio_Int_pa_struct _temp3101; _temp3101.tag= Cyc_Stdio_Int_pa;
_temp3101.f1=( int)(( unsigned int) largs);{ void* _temp3099=( void*)& _temp3101;
void* _temp3095[ 3u]={ _temp3097, _temp3098, _temp3099}; struct _tagged_arr
_temp3096={( void*) _temp3095,( void*) _temp3095,( void*)( _temp3095 + 3u)};
_temp3094( _temp3100, _temp3096);}}}}));} for( 0; _temp2513 != 0;( _temp2513=((
struct Cyc_List_List*) _check_null( _temp2513))->tl, tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl)){ void* t1=( void*)(( struct Cyc_List_List*) _check_null(
_temp2513))->hd; void* k1= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} goto _LL2395;}} _LL2429: if( _temp2522 == 0){ Cyc_Tcutil_terr(
loc, _tag_arr("missing union name", sizeof( unsigned char), 19u)); return cvtenv;}
if(* _temp2519 == 0){ struct _handler_cons _temp3104; _push_handler(& _temp3104);{
int _temp3106= 0; if( setjmp( _temp3104.handler)){ _temp3106= 1;} if( !
_temp3106){* _temp2519=( struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl(
te, loc,( struct _tuple1*) _check_null( _temp2522));; _pop_handler();} else{
void* _temp3105=( void*) _exn_thrown; void* _temp3108= _temp3105; _LL3110: if(
_temp3108 == Cyc_Dict_Absent){ goto _LL3111;} else{ goto _LL3112;} _LL3112: goto
_LL3113; _LL3111: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr(* _temp3114)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp3118= _tag_arr("unbound type union %s", sizeof(
unsigned char), 22u); struct Cyc_Stdio_String_pa_struct _temp3119; _temp3119.tag=
Cyc_Stdio_String_pa; _temp3119.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp2522));{ void* _temp3117=( void*)& _temp3119;
void* _temp3115[ 1u]={ _temp3117}; struct _tagged_arr _temp3116={( void*)
_temp3115,( void*) _temp3115,( void*)( _temp3115 + 1u)}; _temp3114( _temp3118,
_temp3116);}})); return cvtenv; _LL3113:( void) _throw( _temp3108); _LL3109:;}}}{
struct Cyc_Absyn_Uniondecl* ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null(*
_temp2519)); if( ud->name != 0){*(( struct _tuple1*) _check_null( _temp2522))=((
struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( ud->name))->v)[
_check_known_subscript_notnull( 1u, 0)];}{ struct Cyc_List_List* tvs= ud->tvs;
int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=
Cyc_List_length( _temp2520); if( lvs != largs){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp3120)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp3126= _tag_arr("union %s expects %d type arguments but was given %d",
sizeof( unsigned char), 52u); struct Cyc_Stdio_String_pa_struct _temp3129;
_temp3129.tag= Cyc_Stdio_String_pa; _temp3129.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp2522));{ void* _temp3123=( void*)& _temp3129;
struct Cyc_Stdio_Int_pa_struct _temp3128; _temp3128.tag= Cyc_Stdio_Int_pa;
_temp3128.f1=( int)(( unsigned int) lvs);{ void* _temp3124=( void*)& _temp3128;
struct Cyc_Stdio_Int_pa_struct _temp3127; _temp3127.tag= Cyc_Stdio_Int_pa;
_temp3127.f1=( int)(( unsigned int) largs);{ void* _temp3125=( void*)& _temp3127;
void* _temp3121[ 3u]={ _temp3123, _temp3124, _temp3125}; struct _tagged_arr
_temp3122={( void*) _temp3121,( void*) _temp3121,( void*)( _temp3121 + 3u)};
_temp3120( _temp3126, _temp3122);}}}}));} for( 0; _temp2520 != 0;( _temp2520=((
struct Cyc_List_List*) _check_null( _temp2520))->tl, tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl)){ void* t1=( void*)(( struct Cyc_List_List*) _check_null(
_temp2520))->hd; void* k1= Cyc_Absyn_conref_val((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} goto _LL2395;}} _LL2431: { struct Cyc_Absyn_Typedefdecl*
td;{ struct _handler_cons _temp3130; _push_handler(& _temp3130);{ int _temp3132=
0; if( setjmp( _temp3130.handler)){ _temp3132= 1;} if( ! _temp3132){ td= Cyc_Tcenv_lookup_typedefdecl(
te, loc, _temp2529);; _pop_handler();} else{ void* _temp3131=( void*)
_exn_thrown; void* _temp3134= _temp3131; _LL3136: if( _temp3134 == Cyc_Dict_Absent){
goto _LL3137;} else{ goto _LL3138;} _LL3138: goto _LL3139; _LL3137: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp3140)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp3144=
_tag_arr("unbound typedef name %s", sizeof( unsigned char), 24u); struct Cyc_Stdio_String_pa_struct
_temp3145; _temp3145.tag= Cyc_Stdio_String_pa; _temp3145.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2529);{ void* _temp3143=( void*)& _temp3145; void*
_temp3141[ 1u]={ _temp3143}; struct _tagged_arr _temp3142={( void*) _temp3141,(
void*) _temp3141,( void*)( _temp3141 + 1u)}; _temp3140( _temp3144, _temp3142);}}));
return cvtenv; _LL3139:( void) _throw( _temp3134); _LL3135:;}}} _temp2529[
_check_known_subscript_notnull( 1u, 0)]=( td->name)[
_check_known_subscript_notnull( 1u, 0)];{ struct Cyc_List_List* tvs= td->tvs;
struct Cyc_List_List* ts= _temp2527; struct Cyc_List_List* inst= 0; for( 0; ts
!= 0? tvs != 0: 0;( ts=(( struct Cyc_List_List*) _check_null( ts))->tl, tvs=((
struct Cyc_List_List*) _check_null( tvs))->tl)){ void* k= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k,( void*)(( struct Cyc_List_List*)
_check_null( ts))->hd); inst=({ struct Cyc_List_List* _temp3146=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3146->hd=( void*)({ struct
_tuple5* _temp3147=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp3147->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
tvs))->hd; _temp3147->f2=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd;
_temp3147;}); _temp3146->tl= inst; _temp3146;});} if( tvs != 0? 1: ts != 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp3148)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp3152=
_tag_arr("typedef %s expects a different number of arguments", sizeof(
unsigned char), 51u); struct Cyc_Stdio_String_pa_struct _temp3153; _temp3153.tag=
Cyc_Stdio_String_pa; _temp3153.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2529);{ void* _temp3151=( void*)& _temp3153; void* _temp3149[ 1u]={
_temp3151}; struct _tagged_arr _temp3150={( void*) _temp3149,( void*) _temp3149,(
void*)( _temp3149 + 1u)}; _temp3148( _temp3152, _temp3150);}})); return cvtenv;}{
void* new_typ= Cyc_Tcutil_substitute( inst,( void*) td->defn);* _temp2526=({
struct Cyc_Core_Opt* _temp3154=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3154->v=( void*) new_typ; _temp3154;}); goto _LL2395;}}}
_LL2433: goto _LL2395; _LL2435: _temp2533= _temp2531; goto _LL2437; _LL2437:
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2533); goto _LL2395; _LL2439: for( 0; _temp2535 != 0; _temp2535=(( struct
Cyc_List_List*) _check_null( _temp2535))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_List_List*)
_check_null( _temp2535))->hd);} goto _LL2395; _LL2395:;} if( ! Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind( t), expected_kind)){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp3155)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp3161= _tag_arr("type %s has kind %s but as used here needs kind %s",
sizeof( unsigned char), 51u); struct Cyc_Stdio_String_pa_struct _temp3164;
_temp3164.tag= Cyc_Stdio_String_pa; _temp3164.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp3158=( void*)& _temp3164; struct Cyc_Stdio_String_pa_struct
_temp3163; _temp3163.tag= Cyc_Stdio_String_pa; _temp3163.f1=( struct _tagged_arr)
Cyc_Absynpp_kind2string( Cyc_Tcutil_typ_kind( t));{ void* _temp3159=( void*)&
_temp3163; struct Cyc_Stdio_String_pa_struct _temp3162; _temp3162.tag= Cyc_Stdio_String_pa;
_temp3162.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( expected_kind);{
void* _temp3160=( void*)& _temp3162; void* _temp3156[ 3u]={ _temp3158, _temp3159,
_temp3160}; struct _tagged_arr _temp3157={( void*) _temp3156,( void*) _temp3156,(
void*)( _temp3156 + 3u)}; _temp3155( _temp3161, _temp3157);}}}}));} return
cvtenv;} struct Cyc_List_List* Cyc_Tcutil_check_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* kind_env, void*
expected_kind, void* t){ struct Cyc_Tcutil_CVTEnv _temp3165= Cyc_Tcutil_i_check_valid_type(
loc, te,({ struct Cyc_Tcutil_CVTEnv _temp3180; _temp3180.kind_env= kind_env;
_temp3180.free_vars= 0; _temp3180.free_evars= 0; _temp3180;}), expected_kind, t);{
struct Cyc_List_List* vs= _temp3165.free_vars; for( 0; vs != 0; vs=(( struct Cyc_List_List*)
_check_null( vs))->tl){ Cyc_Tcutil_fast_add_free_tvar( kind_env,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( vs))->hd);}}{ struct Cyc_List_List* evs=
_temp3165.free_evars; for( 0; evs != 0; evs=(( struct Cyc_List_List*)
_check_null( evs))->tl){ void* _temp3166= Cyc_Tcutil_compress(( void*)(( struct
Cyc_List_List*) _check_null( evs))->hd); struct Cyc_Core_Opt* _temp3172; struct
Cyc_Core_Opt** _temp3174; _LL3168: if(( unsigned int) _temp3166 > 4u?*(( int*)
_temp3166) == Cyc_Absyn_Evar: 0){ _LL3173: _temp3172=(( struct Cyc_Absyn_Evar_struct*)
_temp3166)->f4; _temp3174=&(( struct Cyc_Absyn_Evar_struct*) _temp3166)->f4;
goto _LL3169;} else{ goto _LL3170;} _LL3170: goto _LL3171; _LL3169: if(*
_temp3174 == 0){* _temp3174=({ struct Cyc_Core_Opt* _temp3175=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3175->v=( void*) kind_env;
_temp3175;});} else{ struct Cyc_List_List* _temp3176=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(* _temp3174))->v; struct Cyc_List_List*
_temp3177= 0; for( 0; _temp3176 != 0; _temp3176=(( struct Cyc_List_List*)
_check_null( _temp3176))->tl){ if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp3176))->hd)){ _temp3177=({ struct Cyc_List_List* _temp3178=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3178->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp3176))->hd);
_temp3178->tl= _temp3177; _temp3178;});}}* _temp3174=({ struct Cyc_Core_Opt*
_temp3179=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3179->v=( void*) _temp3177; _temp3179;});} goto _LL3167; _LL3171: goto
_LL3167; _LL3167:;}} return _temp3165.free_vars;} void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, void* t){ struct
Cyc_List_List* _temp3181= Cyc_Tcutil_check_valid_type( loc, te, 0,( void*) Cyc_Absyn_MemKind,
t);{ struct Cyc_List_List* x= _temp3181; for( 0; x != 0; x=(( struct Cyc_List_List*)
_check_null( x))->tl){ struct Cyc_Absyn_Conref* c= Cyc_Absyn_compress_conref(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( x))->hd)->kind);
void* _temp3182=( void*) c->v; void* _temp3190; _LL3184: if( _temp3182 ==( void*)
Cyc_Absyn_No_constr){ goto _LL3185;} else{ goto _LL3186;} _LL3186: if((
unsigned int) _temp3182 > 1u?*(( int*) _temp3182) == Cyc_Absyn_Eq_constr: 0){
_LL3191: _temp3190=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3182)->f1;
if( _temp3190 ==( void*) Cyc_Absyn_MemKind){ goto _LL3187;} else{ goto _LL3188;}}
else{ goto _LL3188;} _LL3188: goto _LL3189; _LL3185: goto _LL3187; _LL3187:(
void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3192=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3192[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3193; _temp3193.tag= Cyc_Absyn_Eq_constr;
_temp3193.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3193;}); _temp3192;})));
goto _LL3183; _LL3189: goto _LL3183; _LL3183:;}} if( _temp3181 != 0){ void*
_temp3194= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp3200; struct
Cyc_List_List* _temp3202; struct Cyc_List_List* _temp3204; struct Cyc_Absyn_VarargInfo*
_temp3206; int _temp3208; struct Cyc_List_List* _temp3210; void* _temp3212;
struct Cyc_Core_Opt* _temp3214; struct Cyc_List_List* _temp3216; struct Cyc_List_List**
_temp3218; _LL3196: if(( unsigned int) _temp3194 > 4u?*(( int*) _temp3194) ==
Cyc_Absyn_FnType: 0){ _LL3201: _temp3200=(( struct Cyc_Absyn_FnType_struct*)
_temp3194)->f1; _LL3217: _temp3216= _temp3200.tvars; _temp3218=&((( struct Cyc_Absyn_FnType_struct*)
_temp3194)->f1).tvars; goto _LL3215; _LL3215: _temp3214= _temp3200.effect; goto
_LL3213; _LL3213: _temp3212=( void*) _temp3200.ret_typ; goto _LL3211; _LL3211:
_temp3210= _temp3200.args; goto _LL3209; _LL3209: _temp3208= _temp3200.c_varargs;
goto _LL3207; _LL3207: _temp3206= _temp3200.cyc_varargs; goto _LL3205; _LL3205:
_temp3204= _temp3200.rgn_po; goto _LL3203; _LL3203: _temp3202= _temp3200.attributes;
goto _LL3197;} else{ goto _LL3198;} _LL3198: goto _LL3199; _LL3197: if(*
_temp3218 == 0){* _temp3218= _temp3181;} else{ if( _temp3181 != 0){ struct
_tagged_arr _temp3219=*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp3181))->hd)->name; if(*(( const unsigned char*)
_check_unknown_subscript( _temp3219, sizeof( unsigned char), 0)) =='?'){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp3220)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp3223=
_tag_arr("You failed to abstract over the generated type variables. \nPlease remove the bound type variables or else make the effects of the type explicit\nand then abstract the free type variables properly. (Sorry.)",
sizeof( unsigned char), 205u); void* _temp3221[ 0u]={}; struct _tagged_arr
_temp3222={( void*) _temp3221,( void*) _temp3221,( void*)( _temp3221 + 0u)};
_temp3220( _temp3223, _temp3222);}));} else{ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp3224)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp3228= _tag_arr("unbound type variable %s",
sizeof( unsigned char), 25u); struct Cyc_Stdio_String_pa_struct _temp3229;
_temp3229.tag= Cyc_Stdio_String_pa; _temp3229.f1=( struct _tagged_arr) _temp3219;{
void* _temp3227=( void*)& _temp3229; void* _temp3225[ 1u]={ _temp3227}; struct
_tagged_arr _temp3226={( void*) _temp3225,( void*) _temp3225,( void*)( _temp3225
+ 1u)}; _temp3224( _temp3228, _temp3226);}}));}}} goto _LL3195; _LL3199: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp3230)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp3234=
_tag_arr("unbound type variable %s", sizeof( unsigned char), 25u); struct Cyc_Stdio_String_pa_struct
_temp3235; _temp3235.tag= Cyc_Stdio_String_pa; _temp3235.f1=( struct _tagged_arr)*((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp3181))->hd)->name;{
void* _temp3233=( void*)& _temp3235; void* _temp3231[ 1u]={ _temp3233}; struct
_tagged_arr _temp3232={( void*) _temp3231,( void*) _temp3231,( void*)( _temp3231
+ 1u)}; _temp3230( _temp3234, _temp3232);}})); return; _LL3195:;}} void Cyc_Tcutil_add_tvar_identity(
struct Cyc_Absyn_Tvar* tv){ if( tv->identity == 0){ tv->identity= Cyc_Tcutil_new_tvar_id();}}
void Cyc_Tcutil_add_tvar_identities( struct Cyc_List_List* tvs){(( void(*)( void(*
f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Tcutil_add_tvar_identity,
tvs);} void Cyc_Tcutil_check_type( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* bound_tvars, void* expected_kind, void* t){ struct Cyc_List_List*
_temp3236= Cyc_Tcutil_check_valid_type( loc, te, bound_tvars, expected_kind, t);
struct Cyc_List_List* _temp3237= Cyc_Tcutil_remove_bound_tvars( _temp3236,
bound_tvars); struct Cyc_List_List* fs= _temp3237; for( 0; fs != 0; fs=(( struct
Cyc_List_List*) _check_null( fs))->tl){ struct _tagged_arr* _temp3238=(( struct
Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( fs))->hd)->name; Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp3239)( struct _tagged_arr
fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp3244=
_tag_arr("unbound type variable %s in type %s", sizeof( unsigned char), 36u);
struct Cyc_Stdio_String_pa_struct _temp3246; _temp3246.tag= Cyc_Stdio_String_pa;
_temp3246.f1=( struct _tagged_arr)* _temp3238;{ void* _temp3242=( void*)&
_temp3246; struct Cyc_Stdio_String_pa_struct _temp3245; _temp3245.tag= Cyc_Stdio_String_pa;
_temp3245.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp3243=(
void*)& _temp3245; void* _temp3240[ 2u]={ _temp3242, _temp3243}; struct
_tagged_arr _temp3241={( void*) _temp3240,( void*) _temp3240,( void*)( _temp3240
+ 2u)}; _temp3239( _temp3244, _temp3241);}}}));}} static void Cyc_Tcutil_check_unique_unsorted(
int(* cmp)( void*, void*), struct Cyc_List_List* vs, struct Cyc_Position_Segment*
loc, struct _tagged_arr(* a2string)( void*), struct _tagged_arr msg){ for( 0; vs
!= 0; vs=(( struct Cyc_List_List*) _check_null( vs))->tl){ struct Cyc_List_List*
vs2=(( struct Cyc_List_List*) _check_null( vs))->tl; for( 0; vs2 != 0; vs2=((
struct Cyc_List_List*) _check_null( vs2))->tl){ if( cmp(( void*)(( struct Cyc_List_List*)
_check_null( vs))->hd,( void*)(( struct Cyc_List_List*) _check_null( vs2))->hd)
== 0){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(*
_temp3247)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp3252= _tag_arr("%s: %s", sizeof( unsigned char), 7u);
struct Cyc_Stdio_String_pa_struct _temp3254; _temp3254.tag= Cyc_Stdio_String_pa;
_temp3254.f1=( struct _tagged_arr) msg;{ void* _temp3250=( void*)& _temp3254;
struct Cyc_Stdio_String_pa_struct _temp3253; _temp3253.tag= Cyc_Stdio_String_pa;
_temp3253.f1=( struct _tagged_arr) a2string(( void*)(( struct Cyc_List_List*)
_check_null( vs))->hd);{ void* _temp3251=( void*)& _temp3253; void* _temp3248[ 2u]={
_temp3250, _temp3251}; struct _tagged_arr _temp3249={( void*) _temp3248,( void*)
_temp3248,( void*)( _temp3248 + 2u)}; _temp3247( _temp3252, _temp3249);}}}));}}}}
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
_temp3255=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3255->hd=( void*)({ struct _tuple17* _temp3256=( struct _tuple17*)
GC_malloc( sizeof( struct _tuple17)); _temp3256->f1=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( sd_fields))->hd; _temp3256->f2= 0; _temp3256;});
_temp3255->tl= fields; _temp3255;});}}} fields=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields);{ struct Cyc_List_List* ans=
0; for( 0; des != 0; des=(( struct Cyc_List_List*) _check_null( des))->tl){
struct _tuple18 _temp3259; void* _temp3260; struct Cyc_List_List* _temp3262;
struct _tuple18* _temp3257=( struct _tuple18*)(( struct Cyc_List_List*)
_check_null( des))->hd; _temp3259=* _temp3257; _LL3263: _temp3262= _temp3259.f1;
goto _LL3261; _LL3261: _temp3260= _temp3259.f2; goto _LL3258; _LL3258: if(
_temp3262 == 0){ struct Cyc_List_List* _temp3264= fields; for( 0; _temp3264 != 0;
_temp3264=(( struct Cyc_List_List*) _check_null( _temp3264))->tl){ if( !(*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp3264))->hd)).f2){(*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp3264))->hd)).f2= 1;(*((
struct _tuple18*)(( struct Cyc_List_List*) _check_null( des))->hd)).f1=({ struct
Cyc_List_List* _temp3265=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3265->hd=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp3266=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp3266[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp3267; _temp3267.tag= Cyc_Absyn_FieldName;
_temp3267.f1=((*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3264))->hd)).f1)->name; _temp3267;}); _temp3266;})); _temp3265->tl= 0;
_temp3265;}); ans=({ struct Cyc_List_List* _temp3268=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp3268->hd=( void*)({
struct _tuple19* _temp3269=( struct _tuple19*) _region_malloc( rgn, sizeof(
struct _tuple19)); _temp3269->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3264))->hd)).f1; _temp3269->f2= _temp3260; _temp3269;});
_temp3268->tl= ans; _temp3268;}); break;}} if( _temp3264 == 0){ Cyc_Tcutil_terr(
loc, _tag_arr("too many arguments to struct", sizeof( unsigned char), 29u));}}
else{ if((( struct Cyc_List_List*) _check_null( _temp3262))->tl != 0){ Cyc_Tcutil_terr(
loc, _tag_arr("multiple designators are not supported", sizeof( unsigned char),
39u));} else{ void* _temp3270=( void*)(( struct Cyc_List_List*) _check_null(
_temp3262))->hd; struct _tagged_arr* _temp3276; _LL3272: if(*(( int*) _temp3270)
== Cyc_Absyn_ArrayElement){ goto _LL3273;} else{ goto _LL3274;} _LL3274: if(*((
int*) _temp3270) == Cyc_Absyn_FieldName){ _LL3277: _temp3276=(( struct Cyc_Absyn_FieldName_struct*)
_temp3270)->f1; goto _LL3275;} else{ goto _LL3271;} _LL3273: Cyc_Tcutil_terr(
loc, _tag_arr("array designator used in argument to struct", sizeof(
unsigned char), 44u)); goto _LL3271; _LL3275: { struct Cyc_List_List* _temp3278=
fields; for( 0; _temp3278 != 0; _temp3278=(( struct Cyc_List_List*) _check_null(
_temp3278))->tl){ if( Cyc_String_zstrptrcmp( _temp3276,((*(( struct _tuple17*)((
struct Cyc_List_List*) _check_null( _temp3278))->hd)).f1)->name) == 0){ if((*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp3278))->hd)).f2){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr(* _temp3279)(
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp3283= _tag_arr("field %s has already been used as an argument",
sizeof( unsigned char), 46u); struct Cyc_Stdio_String_pa_struct _temp3284;
_temp3284.tag= Cyc_Stdio_String_pa; _temp3284.f1=( struct _tagged_arr)*
_temp3276;{ void* _temp3282=( void*)& _temp3284; void* _temp3280[ 1u]={
_temp3282}; struct _tagged_arr _temp3281={( void*) _temp3280,( void*) _temp3280,(
void*)( _temp3280 + 1u)}; _temp3279( _temp3283, _temp3281);}}));}(*(( struct
_tuple17*)(( struct Cyc_List_List*) _check_null( _temp3278))->hd)).f2= 1; ans=({
struct Cyc_List_List* _temp3285=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp3285->hd=( void*)({ struct _tuple19*
_temp3286=( struct _tuple19*) _region_malloc( rgn, sizeof( struct _tuple19));
_temp3286->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3278))->hd)).f1; _temp3286->f2= _temp3260; _temp3286;}); _temp3285->tl= ans;
_temp3285;}); break;}} if( _temp3278 == 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr(* _temp3287)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp3291= _tag_arr("bad field designator %s",
sizeof( unsigned char), 24u); struct Cyc_Stdio_String_pa_struct _temp3292;
_temp3292.tag= Cyc_Stdio_String_pa; _temp3292.f1=( struct _tagged_arr)*
_temp3276;{ void* _temp3290=( void*)& _temp3292; void* _temp3288[ 1u]={
_temp3290}; struct _tagged_arr _temp3289={( void*) _temp3288,( void*) _temp3288,(
void*)( _temp3288 + 1u)}; _temp3287( _temp3291, _temp3289);}}));} goto _LL3271;}
_LL3271:;}}} for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null(
fields))->tl){ if( !(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
fields))->hd)).f2){ Cyc_Tcutil_terr( loc, _tag_arr("too few arguments to struct",
sizeof( unsigned char), 28u)); break;}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( ans);}} int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void* t, void** elt_typ_dest){ void* _temp3293= Cyc_Tcutil_compress( t); struct
Cyc_Absyn_PtrInfo _temp3299; struct Cyc_Absyn_Conref* _temp3301; void* _temp3303;
_LL3295: if(( unsigned int) _temp3293 > 4u?*(( int*) _temp3293) == Cyc_Absyn_PointerType:
0){ _LL3300: _temp3299=(( struct Cyc_Absyn_PointerType_struct*) _temp3293)->f1;
_LL3304: _temp3303=( void*) _temp3299.elt_typ; goto _LL3302; _LL3302: _temp3301=
_temp3299.bounds; goto _LL3296;} else{ goto _LL3297;} _LL3297: goto _LL3298;
_LL3296: { struct Cyc_Absyn_Conref* _temp3305= Cyc_Absyn_compress_conref(
_temp3301); void* _temp3306=( void*)( Cyc_Absyn_compress_conref( _temp3305))->v;
void* _temp3314; _LL3308: if(( unsigned int) _temp3306 > 1u?*(( int*) _temp3306)
== Cyc_Absyn_Eq_constr: 0){ _LL3315: _temp3314=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3306)->f1; if( _temp3314 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3309;}
else{ goto _LL3310;}} else{ goto _LL3310;} _LL3310: if( _temp3306 ==( void*) Cyc_Absyn_No_constr){
goto _LL3311;} else{ goto _LL3312;} _LL3312: goto _LL3313; _LL3309:*
elt_typ_dest= _temp3303; return 1; _LL3311:( void*)( _temp3305->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3316=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3316[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3317; _temp3317.tag= Cyc_Absyn_Eq_constr;
_temp3317.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp3317;}); _temp3316;})));*
elt_typ_dest= _temp3303; return 1; _LL3313: return 0; _LL3307:;} _LL3298: return
0; _LL3294:;} int Cyc_Tcutil_is_tagged_pointer_typ( void* t){ void* ignore=(
void*) Cyc_Absyn_VoidType; return Cyc_Tcutil_is_tagged_pointer_typ_elt( t,&
ignore);} struct _tuple7 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ struct _tuple7 bogus_ans=({ struct _tuple7 _temp3499;
_temp3499.f1= 0; _temp3499.f2=( void*) Cyc_Absyn_HeapRgn; _temp3499;}); void*
_temp3318=( void*) e->r; void* _temp3332; struct _tuple1* _temp3334; struct
_tagged_arr* _temp3336; struct Cyc_Absyn_Exp* _temp3338; struct _tagged_arr*
_temp3340; struct Cyc_Absyn_Exp* _temp3342; struct Cyc_Absyn_Exp* _temp3344;
struct Cyc_Absyn_Exp* _temp3346; struct Cyc_Absyn_Exp* _temp3348; _LL3320: if(*((
int*) _temp3318) == Cyc_Absyn_Var_e){ _LL3335: _temp3334=(( struct Cyc_Absyn_Var_e_struct*)
_temp3318)->f1; goto _LL3333; _LL3333: _temp3332=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3318)->f2; goto _LL3321;} else{ goto _LL3322;} _LL3322: if(*(( int*)
_temp3318) == Cyc_Absyn_StructMember_e){ _LL3339: _temp3338=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3318)->f1; goto _LL3337; _LL3337: _temp3336=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3318)->f2; goto _LL3323;} else{ goto _LL3324;} _LL3324: if(*(( int*)
_temp3318) == Cyc_Absyn_StructArrow_e){ _LL3343: _temp3342=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3318)->f1; goto _LL3341; _LL3341: _temp3340=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3318)->f2; goto _LL3325;} else{ goto _LL3326;} _LL3326: if(*(( int*)
_temp3318) == Cyc_Absyn_Deref_e){ _LL3345: _temp3344=(( struct Cyc_Absyn_Deref_e_struct*)
_temp3318)->f1; goto _LL3327;} else{ goto _LL3328;} _LL3328: if(*(( int*)
_temp3318) == Cyc_Absyn_Subscript_e){ _LL3349: _temp3348=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3318)->f1; goto _LL3347; _LL3347: _temp3346=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3318)->f2; goto _LL3329;} else{ goto _LL3330;} _LL3330: goto _LL3331;
_LL3321: { void* _temp3350= _temp3332; struct Cyc_Absyn_Vardecl* _temp3364;
struct Cyc_Absyn_Vardecl* _temp3366; struct Cyc_Absyn_Vardecl* _temp3368; struct
Cyc_Absyn_Vardecl* _temp3370; _LL3352: if( _temp3350 ==( void*) Cyc_Absyn_Unresolved_b){
goto _LL3353;} else{ goto _LL3354;} _LL3354: if(( unsigned int) _temp3350 > 1u?*((
int*) _temp3350) == Cyc_Absyn_Funname_b: 0){ goto _LL3355;} else{ goto _LL3356;}
_LL3356: if(( unsigned int) _temp3350 > 1u?*(( int*) _temp3350) == Cyc_Absyn_Global_b:
0){ _LL3365: _temp3364=(( struct Cyc_Absyn_Global_b_struct*) _temp3350)->f1;
goto _LL3357;} else{ goto _LL3358;} _LL3358: if(( unsigned int) _temp3350 > 1u?*((
int*) _temp3350) == Cyc_Absyn_Local_b: 0){ _LL3367: _temp3366=(( struct Cyc_Absyn_Local_b_struct*)
_temp3350)->f1; goto _LL3359;} else{ goto _LL3360;} _LL3360: if(( unsigned int)
_temp3350 > 1u?*(( int*) _temp3350) == Cyc_Absyn_Pat_b: 0){ _LL3369: _temp3368=((
struct Cyc_Absyn_Pat_b_struct*) _temp3350)->f1; goto _LL3361;} else{ goto
_LL3362;} _LL3362: if(( unsigned int) _temp3350 > 1u?*(( int*) _temp3350) == Cyc_Absyn_Param_b:
0){ _LL3371: _temp3370=(( struct Cyc_Absyn_Param_b_struct*) _temp3350)->f1; goto
_LL3363;} else{ goto _LL3351;} _LL3353: return bogus_ans; _LL3355: return({
struct _tuple7 _temp3372; _temp3372.f1= 1; _temp3372.f2=( void*) Cyc_Absyn_HeapRgn;
_temp3372;}); _LL3357: { void* _temp3373= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v); _LL3375: if(( unsigned int) _temp3373
> 4u?*(( int*) _temp3373) == Cyc_Absyn_ArrayType: 0){ goto _LL3376;} else{ goto
_LL3377;} _LL3377: goto _LL3378; _LL3376: return({ struct _tuple7 _temp3379;
_temp3379.f1= 1; _temp3379.f2=( void*) Cyc_Absyn_HeapRgn; _temp3379;}); _LL3378:
return({ struct _tuple7 _temp3380; _temp3380.f1=( _temp3364->tq).q_const;
_temp3380.f2=( void*) Cyc_Absyn_HeapRgn; _temp3380;}); _LL3374:;} _LL3359: {
void* _temp3381= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); _LL3383: if(( unsigned int) _temp3381 > 4u?*(( int*)
_temp3381) == Cyc_Absyn_ArrayType: 0){ goto _LL3384;} else{ goto _LL3385;}
_LL3385: goto _LL3386; _LL3384: return({ struct _tuple7 _temp3387; _temp3387.f1=
1; _temp3387.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3366->rgn))->v;
_temp3387;}); _LL3386: return({ struct _tuple7 _temp3388; _temp3388.f1=(
_temp3366->tq).q_const; _temp3388.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3366->rgn))->v; _temp3388;}); _LL3382:;} _LL3361: _temp3370=
_temp3368; goto _LL3363; _LL3363: return({ struct _tuple7 _temp3389; _temp3389.f1=(
_temp3370->tq).q_const; _temp3389.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3370->rgn))->v; _temp3389;}); _LL3351:;} _LL3323: { void*
_temp3390= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp3338->topt))->v); struct Cyc_List_List* _temp3402; struct Cyc_List_List*
_temp3404; struct Cyc_Absyn_Structdecl** _temp3406; struct Cyc_Absyn_Structdecl*
_temp3408; struct Cyc_Absyn_Uniondecl** _temp3409; struct Cyc_Absyn_Uniondecl*
_temp3411; _LL3392: if(( unsigned int) _temp3390 > 4u?*(( int*) _temp3390) ==
Cyc_Absyn_AnonStructType: 0){ _LL3403: _temp3402=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3390)->f1; goto _LL3393;} else{ goto _LL3394;} _LL3394: if(( unsigned int)
_temp3390 > 4u?*(( int*) _temp3390) == Cyc_Absyn_AnonUnionType: 0){ _LL3405:
_temp3404=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3390)->f1; goto
_LL3395;} else{ goto _LL3396;} _LL3396: if(( unsigned int) _temp3390 > 4u?*((
int*) _temp3390) == Cyc_Absyn_StructType: 0){ _LL3407: _temp3406=(( struct Cyc_Absyn_StructType_struct*)
_temp3390)->f3; if( _temp3406 == 0){ goto _LL3398;} else{ _temp3408=* _temp3406;
goto _LL3397;}} else{ goto _LL3398;} _LL3398: if(( unsigned int) _temp3390 > 4u?*((
int*) _temp3390) == Cyc_Absyn_UnionType: 0){ _LL3410: _temp3409=(( struct Cyc_Absyn_UnionType_struct*)
_temp3390)->f3; if( _temp3409 == 0){ goto _LL3400;} else{ _temp3411=* _temp3409;
goto _LL3399;}} else{ goto _LL3400;} _LL3400: goto _LL3401; _LL3393: _temp3404=
_temp3402; goto _LL3395; _LL3395: { struct Cyc_Absyn_Structfield* _temp3412= Cyc_Absyn_lookup_field(
_temp3404, _temp3336); if( _temp3412 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3412))->width != 0: 0){ return({ struct _tuple7 _temp3413;
_temp3413.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3412))->tq).q_const;
_temp3413.f2=( Cyc_Tcutil_addressof_props( te, _temp3338)).f2; _temp3413;});}
return bogus_ans;} _LL3397: { struct Cyc_Absyn_Structfield* _temp3414= Cyc_Absyn_lookup_struct_field(
_temp3408, _temp3336); if( _temp3414 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3414))->width != 0: 0){ return({ struct _tuple7 _temp3415;
_temp3415.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3414))->tq).q_const;
_temp3415.f2=( Cyc_Tcutil_addressof_props( te, _temp3338)).f2; _temp3415;});}
return bogus_ans;} _LL3399: { struct Cyc_Absyn_Structfield* _temp3416= Cyc_Absyn_lookup_union_field(
_temp3411, _temp3336); if( _temp3416 != 0){ return({ struct _tuple7 _temp3417;
_temp3417.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3416))->tq).q_const;
_temp3417.f2=( Cyc_Tcutil_addressof_props( te, _temp3338)).f2; _temp3417;});}
goto _LL3401;} _LL3401: return bogus_ans; _LL3391:;} _LL3325: { void* _temp3418=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3342->topt))->v);
struct Cyc_Absyn_PtrInfo _temp3424; void* _temp3426; void* _temp3428; _LL3420:
if(( unsigned int) _temp3418 > 4u?*(( int*) _temp3418) == Cyc_Absyn_PointerType:
0){ _LL3425: _temp3424=(( struct Cyc_Absyn_PointerType_struct*) _temp3418)->f1;
_LL3429: _temp3428=( void*) _temp3424.elt_typ; goto _LL3427; _LL3427: _temp3426=(
void*) _temp3424.rgn_typ; goto _LL3421;} else{ goto _LL3422;} _LL3422: goto
_LL3423; _LL3421: { void* _temp3430= Cyc_Tcutil_compress( _temp3428); struct Cyc_List_List*
_temp3442; struct Cyc_List_List* _temp3444; struct Cyc_Absyn_Structdecl**
_temp3446; struct Cyc_Absyn_Structdecl* _temp3448; struct Cyc_Absyn_Uniondecl**
_temp3449; struct Cyc_Absyn_Uniondecl* _temp3451; _LL3432: if(( unsigned int)
_temp3430 > 4u?*(( int*) _temp3430) == Cyc_Absyn_AnonStructType: 0){ _LL3443:
_temp3442=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3430)->f1; goto
_LL3433;} else{ goto _LL3434;} _LL3434: if(( unsigned int) _temp3430 > 4u?*((
int*) _temp3430) == Cyc_Absyn_AnonUnionType: 0){ _LL3445: _temp3444=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3430)->f1; goto _LL3435;} else{ goto
_LL3436;} _LL3436: if(( unsigned int) _temp3430 > 4u?*(( int*) _temp3430) == Cyc_Absyn_StructType:
0){ _LL3447: _temp3446=(( struct Cyc_Absyn_StructType_struct*) _temp3430)->f3;
if( _temp3446 == 0){ goto _LL3438;} else{ _temp3448=* _temp3446; goto _LL3437;}}
else{ goto _LL3438;} _LL3438: if(( unsigned int) _temp3430 > 4u?*(( int*)
_temp3430) == Cyc_Absyn_UnionType: 0){ _LL3450: _temp3449=(( struct Cyc_Absyn_UnionType_struct*)
_temp3430)->f3; if( _temp3449 == 0){ goto _LL3440;} else{ _temp3451=* _temp3449;
goto _LL3439;}} else{ goto _LL3440;} _LL3440: goto _LL3441; _LL3433: _temp3444=
_temp3442; goto _LL3435; _LL3435: { struct Cyc_Absyn_Structfield* _temp3452= Cyc_Absyn_lookup_field(
_temp3444, _temp3340); if( _temp3452 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3452))->width != 0: 0){ return({ struct _tuple7 _temp3453;
_temp3453.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3452))->tq).q_const;
_temp3453.f2= _temp3426; _temp3453;});} return bogus_ans;} _LL3437: { struct Cyc_Absyn_Structfield*
_temp3454= Cyc_Absyn_lookup_struct_field( _temp3448, _temp3340); if( _temp3454
!= 0?(( struct Cyc_Absyn_Structfield*) _check_null( _temp3454))->width != 0: 0){
return({ struct _tuple7 _temp3455; _temp3455.f1=((( struct Cyc_Absyn_Structfield*)
_check_null( _temp3454))->tq).q_const; _temp3455.f2= _temp3426; _temp3455;});}
return bogus_ans;} _LL3439: { struct Cyc_Absyn_Structfield* _temp3456= Cyc_Absyn_lookup_union_field(
_temp3451, _temp3340); if( _temp3456 != 0){ return({ struct _tuple7 _temp3457;
_temp3457.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3456))->tq).q_const;
_temp3457.f2= _temp3426; _temp3457;});} return bogus_ans;} _LL3441: return
bogus_ans; _LL3431:;} _LL3423: return bogus_ans; _LL3419:;} _LL3327: { void*
_temp3458= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp3344->topt))->v); struct Cyc_Absyn_PtrInfo _temp3464; struct Cyc_Absyn_Tqual
_temp3466; void* _temp3468; _LL3460: if(( unsigned int) _temp3458 > 4u?*(( int*)
_temp3458) == Cyc_Absyn_PointerType: 0){ _LL3465: _temp3464=(( struct Cyc_Absyn_PointerType_struct*)
_temp3458)->f1; _LL3469: _temp3468=( void*) _temp3464.rgn_typ; goto _LL3467;
_LL3467: _temp3466= _temp3464.tq; goto _LL3461;} else{ goto _LL3462;} _LL3462:
goto _LL3463; _LL3461: return({ struct _tuple7 _temp3470; _temp3470.f1=
_temp3466.q_const; _temp3470.f2= _temp3468; _temp3470;}); _LL3463: return
bogus_ans; _LL3459:;} _LL3329: { void* t= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp3348->topt))->v); void* _temp3471= t; struct
Cyc_Absyn_Tqual _temp3481; struct Cyc_List_List* _temp3483; struct Cyc_Absyn_PtrInfo
_temp3485; struct Cyc_Absyn_Conref* _temp3487; struct Cyc_Absyn_Tqual _temp3489;
void* _temp3491; void* _temp3493; _LL3473: if(( unsigned int) _temp3471 > 4u?*((
int*) _temp3471) == Cyc_Absyn_ArrayType: 0){ _LL3482: _temp3481=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3471)->f2; goto _LL3474;} else{ goto _LL3475;} _LL3475: if(( unsigned int)
_temp3471 > 4u?*(( int*) _temp3471) == Cyc_Absyn_TupleType: 0){ _LL3484:
_temp3483=(( struct Cyc_Absyn_TupleType_struct*) _temp3471)->f1; goto _LL3476;}
else{ goto _LL3477;} _LL3477: if(( unsigned int) _temp3471 > 4u?*(( int*)
_temp3471) == Cyc_Absyn_PointerType: 0){ _LL3486: _temp3485=(( struct Cyc_Absyn_PointerType_struct*)
_temp3471)->f1; _LL3494: _temp3493=( void*) _temp3485.elt_typ; goto _LL3492;
_LL3492: _temp3491=( void*) _temp3485.rgn_typ; goto _LL3490; _LL3490: _temp3489=
_temp3485.tq; goto _LL3488; _LL3488: _temp3487= _temp3485.bounds; goto _LL3478;}
else{ goto _LL3479;} _LL3479: goto _LL3480; _LL3474: return({ struct _tuple7
_temp3495; _temp3495.f1= _temp3481.q_const; _temp3495.f2=( Cyc_Tcutil_addressof_props(
te, _temp3348)).f2; _temp3495;}); _LL3476: { struct _tuple4* _temp3496= Cyc_Absyn_lookup_tuple_field(
_temp3483,( int) Cyc_Evexp_eval_const_uint_exp( _temp3346)); if( _temp3496 != 0){
return({ struct _tuple7 _temp3497; _temp3497.f1=((*(( struct _tuple4*)
_check_null( _temp3496))).f1).q_const; _temp3497.f2=( Cyc_Tcutil_addressof_props(
te, _temp3348)).f2; _temp3497;});} return bogus_ans;} _LL3478: return({ struct
_tuple7 _temp3498; _temp3498.f1= _temp3489.q_const; _temp3498.f2= _temp3491;
_temp3498;}); _LL3480: return bogus_ans; _LL3472:;} _LL3331: Cyc_Tcutil_terr( e->loc,
_tag_arr("unary & applied to non-lvalue", sizeof( unsigned char), 30u)); return
bogus_ans; _LL3319:;} void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv* te,
void* e_typ, struct Cyc_Absyn_Exp* e){ void* _temp3500= Cyc_Tcutil_compress(
e_typ); struct Cyc_Absyn_Tqual _temp3506; void* _temp3508; _LL3502: if((
unsigned int) _temp3500 > 4u?*(( int*) _temp3500) == Cyc_Absyn_ArrayType: 0){
_LL3509: _temp3508=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3500)->f1;
goto _LL3507; _LL3507: _temp3506=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3500)->f2; goto _LL3503;} else{ goto _LL3504;} _LL3504: goto _LL3505;
_LL3503: { void* _temp3512; struct _tuple7 _temp3510= Cyc_Tcutil_addressof_props(
te, e); _LL3513: _temp3512= _temp3510.f2; goto _LL3511; _LL3511: return Cyc_Absyn_atb_typ(
_temp3508, _temp3512, _temp3506,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp3514=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3514[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3515; _temp3515.tag= Cyc_Absyn_Upper_b;
_temp3515.f1= e; _temp3515;}); _temp3514;}));} _LL3505: return e_typ; _LL3501:;}
void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment* loc, unsigned int i,
struct Cyc_Absyn_Conref* b){ b= Cyc_Absyn_compress_conref( b);{ void* _temp3516=(
void*) b->v; void* _temp3524; void* _temp3526; struct Cyc_Absyn_Exp* _temp3528;
_LL3518: if(( unsigned int) _temp3516 > 1u?*(( int*) _temp3516) == Cyc_Absyn_Eq_constr:
0){ _LL3525: _temp3524=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3516)->f1;
if( _temp3524 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3519;} else{ goto _LL3520;}}
else{ goto _LL3520;} _LL3520: if(( unsigned int) _temp3516 > 1u?*(( int*)
_temp3516) == Cyc_Absyn_Eq_constr: 0){ _LL3527: _temp3526=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3516)->f1; if(( unsigned int) _temp3526 > 1u?*(( int*) _temp3526) == Cyc_Absyn_Upper_b:
0){ _LL3529: _temp3528=(( struct Cyc_Absyn_Upper_b_struct*) _temp3526)->f1; goto
_LL3521;} else{ goto _LL3522;}} else{ goto _LL3522;} _LL3522: goto _LL3523;
_LL3519: return; _LL3521: if( Cyc_Evexp_eval_const_uint_exp( _temp3528) <= i){
Cyc_Tcutil_terr( loc, _tag_arr("dereference is out of bounds", sizeof(
unsigned char), 29u));} return; _LL3523:( void*)( b->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_constr_struct* _temp3530=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3530[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3531; _temp3531.tag= Cyc_Absyn_Eq_constr;
_temp3531.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3532=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3532[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3533; _temp3533.tag= Cyc_Absyn_Upper_b;
_temp3533.f1= Cyc_Absyn_uint_exp( i + 1, 0); _temp3533;}); _temp3532;}));
_temp3531;}); _temp3530;}))); return; _LL3517:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_bits_only( void* t){ void* _temp3534= Cyc_Tcutil_compress(
t); void* _temp3560; struct Cyc_List_List* _temp3562; struct Cyc_Absyn_Structdecl**
_temp3564; struct Cyc_List_List* _temp3566; struct Cyc_Absyn_Uniondecl**
_temp3568; struct Cyc_List_List* _temp3570; struct Cyc_List_List* _temp3572;
struct Cyc_List_List* _temp3574; _LL3536: if( _temp3534 ==( void*) Cyc_Absyn_VoidType){
goto _LL3537;} else{ goto _LL3538;} _LL3538: if(( unsigned int) _temp3534 > 4u?*((
int*) _temp3534) == Cyc_Absyn_IntType: 0){ goto _LL3539;} else{ goto _LL3540;}
_LL3540: if( _temp3534 ==( void*) Cyc_Absyn_FloatType){ goto _LL3541;} else{
goto _LL3542;} _LL3542: if( _temp3534 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL3543;} else{ goto _LL3544;} _LL3544: if(( unsigned int) _temp3534 > 4u?*((
int*) _temp3534) == Cyc_Absyn_EnumType: 0){ goto _LL3545;} else{ goto _LL3546;}
_LL3546: if(( unsigned int) _temp3534 > 4u?*(( int*) _temp3534) == Cyc_Absyn_ArrayType:
0){ _LL3561: _temp3560=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3534)->f1;
goto _LL3547;} else{ goto _LL3548;} _LL3548: if(( unsigned int) _temp3534 > 4u?*((
int*) _temp3534) == Cyc_Absyn_TupleType: 0){ _LL3563: _temp3562=(( struct Cyc_Absyn_TupleType_struct*)
_temp3534)->f1; goto _LL3549;} else{ goto _LL3550;} _LL3550: if(( unsigned int)
_temp3534 > 4u?*(( int*) _temp3534) == Cyc_Absyn_StructType: 0){ _LL3567:
_temp3566=(( struct Cyc_Absyn_StructType_struct*) _temp3534)->f2; goto _LL3565;
_LL3565: _temp3564=(( struct Cyc_Absyn_StructType_struct*) _temp3534)->f3; goto
_LL3551;} else{ goto _LL3552;} _LL3552: if(( unsigned int) _temp3534 > 4u?*((
int*) _temp3534) == Cyc_Absyn_UnionType: 0){ _LL3571: _temp3570=(( struct Cyc_Absyn_UnionType_struct*)
_temp3534)->f2; goto _LL3569; _LL3569: _temp3568=(( struct Cyc_Absyn_UnionType_struct*)
_temp3534)->f3; goto _LL3553;} else{ goto _LL3554;} _LL3554: if(( unsigned int)
_temp3534 > 4u?*(( int*) _temp3534) == Cyc_Absyn_AnonStructType: 0){ _LL3573:
_temp3572=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3534)->f1; goto
_LL3555;} else{ goto _LL3556;} _LL3556: if(( unsigned int) _temp3534 > 4u?*((
int*) _temp3534) == Cyc_Absyn_AnonUnionType: 0){ _LL3575: _temp3574=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3534)->f1; goto _LL3557;} else{ goto
_LL3558;} _LL3558: goto _LL3559; _LL3537: return 1; _LL3539: return 1; _LL3541:
return 1; _LL3543: return 1; _LL3545: return 0; _LL3547: return Cyc_Tcutil_bits_only(
_temp3560); _LL3549: for( 0; _temp3562 != 0; _temp3562=(( struct Cyc_List_List*)
_check_null( _temp3562))->tl){ if( ! Cyc_Tcutil_bits_only((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp3562))->hd)).f2)){ return 0;}} return 1;
_LL3551: if( _temp3564 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* _temp3576=*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp3564)); if( _temp3576->fields
== 0){ return 0;}{ struct _RegionHandle _temp3577= _new_region(); struct
_RegionHandle* rgn=& _temp3577; _push_region( rgn);{ struct Cyc_List_List*
_temp3578=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp3576->tvs, _temp3566);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp3576->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute(
rgn, _temp3578,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3579= 0; _npop_handler( 0u); return
_temp3579;}}}{ int _temp3580= 1; _npop_handler( 0u); return _temp3580;}};
_pop_region( rgn);}} _LL3553: if( _temp3568 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
_temp3581=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp3568)); if(
_temp3581->fields == 0){ return 0;}{ struct _RegionHandle _temp3582= _new_region();
struct _RegionHandle* rgn=& _temp3582; _push_region( rgn);{ struct Cyc_List_List*
_temp3583=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp3581->tvs, _temp3570);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp3581->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute(
rgn, _temp3583,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3584= 0; _npop_handler( 0u); return
_temp3584;}}}{ int _temp3585= 1; _npop_handler( 0u); return _temp3585;}};
_pop_region( rgn);}} _LL3555: _temp3574= _temp3572; goto _LL3557; _LL3557: for(
0; _temp3574 != 0; _temp3574=(( struct Cyc_List_List*) _check_null( _temp3574))->tl){
if( ! Cyc_Tcutil_bits_only(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp3574))->hd)->type)){ return 0;}} return 1;
_LL3559: return 0; _LL3535:;} struct _tuple20{ struct Cyc_List_List* f1; struct
Cyc_Absyn_Exp* f2; } ; static int Cyc_Tcutil_cnst_exp( struct Cyc_Tcenv_Tenv* te,
int var_okay, struct Cyc_Absyn_Exp* e){ void* _temp3586=( void*) e->r; struct
_tuple1* _temp3626; struct Cyc_Absyn_Exp* _temp3628; struct Cyc_Absyn_Exp*
_temp3630; struct Cyc_Absyn_Exp* _temp3632; struct Cyc_Absyn_Exp* _temp3634;
struct Cyc_Absyn_Exp* _temp3636; struct Cyc_Absyn_Exp* _temp3638; struct Cyc_Absyn_Exp*
_temp3640; struct Cyc_Absyn_Exp* _temp3642; void* _temp3644; struct Cyc_Absyn_Exp*
_temp3646; struct Cyc_Absyn_Exp* _temp3648; struct Cyc_Absyn_Exp* _temp3650;
struct Cyc_List_List* _temp3652; struct Cyc_List_List* _temp3654; struct Cyc_List_List*
_temp3656; struct Cyc_List_List* _temp3658; void* _temp3660; struct Cyc_List_List*
_temp3662; struct Cyc_List_List* _temp3664; _LL3588: if(*(( int*) _temp3586) ==
Cyc_Absyn_Const_e){ goto _LL3589;} else{ goto _LL3590;} _LL3590: if(*(( int*)
_temp3586) == Cyc_Absyn_Sizeoftyp_e){ goto _LL3591;} else{ goto _LL3592;}
_LL3592: if(*(( int*) _temp3586) == Cyc_Absyn_Sizeofexp_e){ goto _LL3593;} else{
goto _LL3594;} _LL3594: if(*(( int*) _temp3586) == Cyc_Absyn_Enum_e){ goto
_LL3595;} else{ goto _LL3596;} _LL3596: if(*(( int*) _temp3586) == Cyc_Absyn_Var_e){
_LL3627: _temp3626=(( struct Cyc_Absyn_Var_e_struct*) _temp3586)->f1; goto
_LL3597;} else{ goto _LL3598;} _LL3598: if(*(( int*) _temp3586) == Cyc_Absyn_Conditional_e){
_LL3633: _temp3632=(( struct Cyc_Absyn_Conditional_e_struct*) _temp3586)->f1;
goto _LL3631; _LL3631: _temp3630=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3586)->f2; goto _LL3629; _LL3629: _temp3628=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3586)->f3; goto _LL3599;} else{ goto _LL3600;} _LL3600: if(*(( int*)
_temp3586) == Cyc_Absyn_SeqExp_e){ _LL3637: _temp3636=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3586)->f1; goto _LL3635; _LL3635: _temp3634=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3586)->f2; goto _LL3601;} else{ goto _LL3602;} _LL3602: if(*(( int*)
_temp3586) == Cyc_Absyn_NoInstantiate_e){ _LL3639: _temp3638=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp3586)->f1; goto _LL3603;} else{ goto _LL3604;} _LL3604: if(*(( int*)
_temp3586) == Cyc_Absyn_Instantiate_e){ _LL3641: _temp3640=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3586)->f1; goto _LL3605;} else{ goto _LL3606;} _LL3606: if(*(( int*)
_temp3586) == Cyc_Absyn_Cast_e){ _LL3645: _temp3644=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp3586)->f1; goto _LL3643; _LL3643: _temp3642=(( struct Cyc_Absyn_Cast_e_struct*)
_temp3586)->f2; goto _LL3607;} else{ goto _LL3608;} _LL3608: if(*(( int*)
_temp3586) == Cyc_Absyn_Address_e){ _LL3647: _temp3646=(( struct Cyc_Absyn_Address_e_struct*)
_temp3586)->f1; goto _LL3609;} else{ goto _LL3610;} _LL3610: if(*(( int*)
_temp3586) == Cyc_Absyn_Comprehension_e){ _LL3651: _temp3650=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3586)->f2; goto _LL3649; _LL3649: _temp3648=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3586)->f3; goto _LL3611;} else{ goto _LL3612;} _LL3612: if(*(( int*)
_temp3586) == Cyc_Absyn_Array_e){ _LL3653: _temp3652=(( struct Cyc_Absyn_Array_e_struct*)
_temp3586)->f1; goto _LL3613;} else{ goto _LL3614;} _LL3614: if(*(( int*)
_temp3586) == Cyc_Absyn_AnonStruct_e){ _LL3655: _temp3654=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3586)->f2; goto _LL3615;} else{ goto _LL3616;} _LL3616: if(*(( int*)
_temp3586) == Cyc_Absyn_Struct_e){ _LL3657: _temp3656=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3586)->f3; goto _LL3617;} else{ goto _LL3618;} _LL3618: if(*(( int*)
_temp3586) == Cyc_Absyn_Primop_e){ _LL3661: _temp3660=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp3586)->f1; goto _LL3659; _LL3659: _temp3658=(( struct Cyc_Absyn_Primop_e_struct*)
_temp3586)->f2; goto _LL3619;} else{ goto _LL3620;} _LL3620: if(*(( int*)
_temp3586) == Cyc_Absyn_Tuple_e){ _LL3663: _temp3662=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp3586)->f1; goto _LL3621;} else{ goto _LL3622;} _LL3622: if(*(( int*)
_temp3586) == Cyc_Absyn_Tunion_e){ _LL3665: _temp3664=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3586)->f3; goto _LL3623;} else{ goto _LL3624;} _LL3624: goto _LL3625;
_LL3589: return 1; _LL3591: return 1; _LL3593: return 1; _LL3595: return 1;
_LL3597: { struct _handler_cons _temp3666; _push_handler(& _temp3666);{ int
_temp3668= 0; if( setjmp( _temp3666.handler)){ _temp3668= 1;} if( ! _temp3668){{
void* _temp3669= Cyc_Tcenv_lookup_ordinary( te, e->loc, _temp3626); void*
_temp3677; void* _temp3679; struct Cyc_Absyn_Vardecl* _temp3681; _LL3671: if(*((
int*) _temp3669) == Cyc_Tcenv_VarRes){ _LL3678: _temp3677=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3669)->f1; if(( unsigned int) _temp3677 > 1u?*(( int*) _temp3677) == Cyc_Absyn_Funname_b:
0){ goto _LL3672;} else{ goto _LL3673;}} else{ goto _LL3673;} _LL3673: if(*((
int*) _temp3669) == Cyc_Tcenv_VarRes){ _LL3680: _temp3679=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3669)->f1; if(( unsigned int) _temp3679 > 1u?*(( int*) _temp3679) == Cyc_Absyn_Global_b:
0){ _LL3682: _temp3681=(( struct Cyc_Absyn_Global_b_struct*) _temp3679)->f1;
goto _LL3674;} else{ goto _LL3675;}} else{ goto _LL3675;} _LL3675: goto _LL3676;
_LL3672: { int _temp3683= 1; _npop_handler( 0u); return _temp3683;} _LL3674: {
void* _temp3684= Cyc_Tcutil_compress(( void*) _temp3681->type); _LL3686: if((
unsigned int) _temp3684 > 4u?*(( int*) _temp3684) == Cyc_Absyn_ArrayType: 0){
goto _LL3687;} else{ goto _LL3688;} _LL3688: goto _LL3689; _LL3687: { int
_temp3690= 1; _npop_handler( 0u); return _temp3690;} _LL3689: { int _temp3691=
var_okay; _npop_handler( 0u); return _temp3691;} _LL3685:;} _LL3676: { int
_temp3692= var_okay; _npop_handler( 0u); return _temp3692;} _LL3670:;};
_pop_handler();} else{ void* _temp3667=( void*) _exn_thrown; void* _temp3694=
_temp3667; _LL3696: if( _temp3694 == Cyc_Dict_Absent){ goto _LL3697;} else{ goto
_LL3698;} _LL3698: goto _LL3699; _LL3697: return 0; _LL3699:( void) _throw(
_temp3694); _LL3695:;}}} _LL3599: return( Cyc_Tcutil_cnst_exp( te, 0, _temp3632)?
Cyc_Tcutil_cnst_exp( te, 0, _temp3630): 0)? Cyc_Tcutil_cnst_exp( te, 0,
_temp3628): 0; _LL3601: return Cyc_Tcutil_cnst_exp( te, 0, _temp3636)? Cyc_Tcutil_cnst_exp(
te, 0, _temp3634): 0; _LL3603: _temp3640= _temp3638; goto _LL3605; _LL3605:
return Cyc_Tcutil_cnst_exp( te, var_okay, _temp3640); _LL3607: return Cyc_Tcutil_cnst_exp(
te, var_okay, _temp3642); _LL3609: return Cyc_Tcutil_cnst_exp( te, 1, _temp3646);
_LL3611: return Cyc_Tcutil_cnst_exp( te, 0, _temp3650)? Cyc_Tcutil_cnst_exp( te,
0, _temp3648): 0; _LL3613: _temp3654= _temp3652; goto _LL3615; _LL3615:
_temp3656= _temp3654; goto _LL3617; _LL3617: for( 0; _temp3656 != 0; _temp3656=((
struct Cyc_List_List*) _check_null( _temp3656))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,(*(( struct _tuple20*)(( struct Cyc_List_List*) _check_null( _temp3656))->hd)).f2)){
return 0;}} return 1; _LL3619: _temp3662= _temp3658; goto _LL3621; _LL3621:
_temp3664= _temp3662; goto _LL3623; _LL3623: for( 0; _temp3664 != 0; _temp3664=((
struct Cyc_List_List*) _check_null( _temp3664))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp3664))->hd)){
return 0;}} return 1; _LL3625: return 0; _LL3587:;} int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcutil_cnst_exp(
te, 0, e);} static int Cyc_Tcutil_fields_support_default( struct Cyc_List_List*
tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs); int Cyc_Tcutil_supports_default(
void* t){ void* _temp3700= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp3738; struct Cyc_Absyn_Conref* _temp3740; struct Cyc_Absyn_Conref*
_temp3742; void* _temp3744; void* _temp3746; struct Cyc_List_List* _temp3748;
struct Cyc_Absyn_Structdecl** _temp3750; struct Cyc_List_List* _temp3752; struct
Cyc_Absyn_Uniondecl** _temp3754; struct Cyc_List_List* _temp3756; struct Cyc_List_List*
_temp3758; struct Cyc_List_List* _temp3760; _LL3702: if( _temp3700 ==( void*)
Cyc_Absyn_VoidType){ goto _LL3703;} else{ goto _LL3704;} _LL3704: if((
unsigned int) _temp3700 > 4u?*(( int*) _temp3700) == Cyc_Absyn_Evar: 0){ goto
_LL3705;} else{ goto _LL3706;} _LL3706: if(( unsigned int) _temp3700 > 4u?*((
int*) _temp3700) == Cyc_Absyn_VarType: 0){ goto _LL3707;} else{ goto _LL3708;}
_LL3708: if(( unsigned int) _temp3700 > 4u?*(( int*) _temp3700) == Cyc_Absyn_TunionType:
0){ goto _LL3709;} else{ goto _LL3710;} _LL3710: if(( unsigned int) _temp3700 >
4u?*(( int*) _temp3700) == Cyc_Absyn_TunionFieldType: 0){ goto _LL3711;} else{
goto _LL3712;} _LL3712: if(( unsigned int) _temp3700 > 4u?*(( int*) _temp3700)
== Cyc_Absyn_PointerType: 0){ _LL3739: _temp3738=(( struct Cyc_Absyn_PointerType_struct*)
_temp3700)->f1; _LL3745: _temp3744=( void*) _temp3738.elt_typ; goto _LL3743;
_LL3743: _temp3742= _temp3738.nullable; goto _LL3741; _LL3741: _temp3740=
_temp3738.bounds; goto _LL3713;} else{ goto _LL3714;} _LL3714: if(( unsigned int)
_temp3700 > 4u?*(( int*) _temp3700) == Cyc_Absyn_IntType: 0){ goto _LL3715;}
else{ goto _LL3716;} _LL3716: if( _temp3700 ==( void*) Cyc_Absyn_FloatType){
goto _LL3717;} else{ goto _LL3718;} _LL3718: if( _temp3700 ==( void*) Cyc_Absyn_DoubleType){
goto _LL3719;} else{ goto _LL3720;} _LL3720: if(( unsigned int) _temp3700 > 4u?*((
int*) _temp3700) == Cyc_Absyn_ArrayType: 0){ _LL3747: _temp3746=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3700)->f1; goto _LL3721;} else{ goto
_LL3722;} _LL3722: if(( unsigned int) _temp3700 > 4u?*(( int*) _temp3700) == Cyc_Absyn_FnType:
0){ goto _LL3723;} else{ goto _LL3724;} _LL3724: if(( unsigned int) _temp3700 >
4u?*(( int*) _temp3700) == Cyc_Absyn_TupleType: 0){ _LL3749: _temp3748=(( struct
Cyc_Absyn_TupleType_struct*) _temp3700)->f1; goto _LL3725;} else{ goto _LL3726;}
_LL3726: if(( unsigned int) _temp3700 > 4u?*(( int*) _temp3700) == Cyc_Absyn_StructType:
0){ _LL3753: _temp3752=(( struct Cyc_Absyn_StructType_struct*) _temp3700)->f2;
goto _LL3751; _LL3751: _temp3750=(( struct Cyc_Absyn_StructType_struct*)
_temp3700)->f3; goto _LL3727;} else{ goto _LL3728;} _LL3728: if(( unsigned int)
_temp3700 > 4u?*(( int*) _temp3700) == Cyc_Absyn_UnionType: 0){ _LL3757:
_temp3756=(( struct Cyc_Absyn_UnionType_struct*) _temp3700)->f2; goto _LL3755;
_LL3755: _temp3754=(( struct Cyc_Absyn_UnionType_struct*) _temp3700)->f3; goto
_LL3729;} else{ goto _LL3730;} _LL3730: if(( unsigned int) _temp3700 > 4u?*((
int*) _temp3700) == Cyc_Absyn_AnonStructType: 0){ _LL3759: _temp3758=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp3700)->f1; goto _LL3731;} else{ goto
_LL3732;} _LL3732: if(( unsigned int) _temp3700 > 4u?*(( int*) _temp3700) == Cyc_Absyn_AnonUnionType:
0){ _LL3761: _temp3760=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3700)->f1;
goto _LL3733;} else{ goto _LL3734;} _LL3734: if(( unsigned int) _temp3700 > 4u?*((
int*) _temp3700) == Cyc_Absyn_EnumType: 0){ goto _LL3735;} else{ goto _LL3736;}
_LL3736: goto _LL3737; _LL3703: return 1; _LL3705: return 0; _LL3707: return 0;
_LL3709: return 0; _LL3711: return 0; _LL3713: { void* _temp3762=( void*)( Cyc_Absyn_compress_conref(
_temp3740))->v; void* _temp3770; void* _temp3772; _LL3764: if(( unsigned int)
_temp3762 > 1u?*(( int*) _temp3762) == Cyc_Absyn_Eq_constr: 0){ _LL3771:
_temp3770=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3762)->f1; if(
_temp3770 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3765;} else{ goto _LL3766;}}
else{ goto _LL3766;} _LL3766: if(( unsigned int) _temp3762 > 1u?*(( int*)
_temp3762) == Cyc_Absyn_Eq_constr: 0){ _LL3773: _temp3772=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3762)->f1; if(( unsigned int) _temp3772 > 1u?*(( int*) _temp3772) == Cyc_Absyn_Upper_b:
0){ goto _LL3767;} else{ goto _LL3768;}} else{ goto _LL3768;} _LL3768: goto
_LL3769; _LL3765: return 1; _LL3767: { void* _temp3774=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3742))->v; int
_temp3780; _LL3776: if(( unsigned int) _temp3774 > 1u?*(( int*) _temp3774) ==
Cyc_Absyn_Eq_constr: 0){ _LL3781: _temp3780=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3774)->f1; goto _LL3777;} else{ goto _LL3778;} _LL3778: goto _LL3779;
_LL3777: return _temp3780; _LL3779: return 0; _LL3775:;} _LL3769: return 0;
_LL3763:;} _LL3715: return 1; _LL3717: return 1; _LL3719: return 1; _LL3721:
return Cyc_Tcutil_supports_default( _temp3746); _LL3723: return 0; _LL3725: for(
0; _temp3748 != 0; _temp3748=(( struct Cyc_List_List*) _check_null( _temp3748))->tl){
if( ! Cyc_Tcutil_supports_default((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp3748))->hd)).f2)){ return 0;}} return 1; _LL3727: if(
_temp3750 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp3750)); if( sd->fields == 0){ return 0;} return Cyc_Tcutil_fields_support_default(
sd->tvs, _temp3752,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
sd->fields))->v);} _LL3729: if( _temp3754 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp3754)); if( ud->fields ==
0){ return 0;} return Cyc_Tcutil_fields_support_default( ud->tvs, _temp3756,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ud->fields))->v);}
_LL3731: _temp3760= _temp3758; goto _LL3733; _LL3733: return Cyc_Tcutil_fields_support_default(
0, 0, _temp3760); _LL3735: return 1; _LL3737: return 0; _LL3701:;} static int
Cyc_Tcutil_fields_support_default( struct Cyc_List_List* tvs, struct Cyc_List_List*
ts, struct Cyc_List_List* fs){{ struct _RegionHandle _temp3782= _new_region();
struct _RegionHandle* rgn=& _temp3782; _push_region( rgn);{ struct Cyc_List_List*
_temp3783=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, tvs, ts); for( 0; fs != 0; fs=(( struct Cyc_List_List*) _check_null(
fs))->tl){ void* t= Cyc_Tcutil_rsubstitute( rgn, _temp3783,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( fs))->hd)->type); if( ! Cyc_Tcutil_supports_default(
t)){ int _temp3784= 0; _npop_handler( 0u); return _temp3784;}}}; _pop_region(
rgn);} return 1;}