 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
extern int Cyc_Core_intcmp( int, int); extern unsigned char Cyc_Core_InvalidArg[
15u]; struct Cyc_Core_InvalidArg_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern struct _RegionHandle* Cyc_Core_heap_region; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, unsigned int);
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_std___sFILE;
extern struct Cyc_std___sFILE* Cyc_std_stderr; extern int Cyc_std_fflush( struct
Cyc_std___sFILE*); extern unsigned char Cyc_std_FileCloseError[ 19u]; extern
unsigned char Cyc_std_FileOpenError[ 18u]; struct Cyc_std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_std_String_pa=
0; struct Cyc_std_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_std_Int_pa= 1; struct Cyc_std_Int_pa_struct{ int tag; unsigned int
f1; } ; static const int Cyc_std_Double_pa= 2; struct Cyc_std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_std_ShortPtr_pa= 3; struct Cyc_std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_std_IntPtr_pa= 4; struct Cyc_std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_std_fprintf( struct Cyc_std___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); extern struct _tagged_arr Cyc_std_aprintf(
struct _tagged_arr fmt, struct _tagged_arr); extern struct _tagged_arr Cyc_std_vraprintf(
struct _RegionHandle* r1, struct _tagged_arr fmt, struct _tagged_arr ap); static
const int Cyc_std_ShortPtr_sa= 0; struct Cyc_std_ShortPtr_sa_struct{ int tag;
short* f1; } ; static const int Cyc_std_UShortPtr_sa= 1; struct Cyc_std_UShortPtr_sa_struct{
int tag; unsigned short* f1; } ; static const int Cyc_std_IntPtr_sa= 2; struct
Cyc_std_IntPtr_sa_struct{ int tag; int* f1; } ; static const int Cyc_std_UIntPtr_sa=
3; struct Cyc_std_UIntPtr_sa_struct{ int tag; unsigned int* f1; } ; static const
int Cyc_std_StringPtr_sa= 4; struct Cyc_std_StringPtr_sa_struct{ int tag; struct
_tagged_arr f1; } ; static const int Cyc_std_DoublePtr_sa= 5; struct Cyc_std_DoublePtr_sa_struct{
int tag; double* f1; } ; static const int Cyc_std_FloatPtr_sa= 6; struct Cyc_std_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; extern struct Cyc_List_List* Cyc_List_list( struct _tagged_arr); extern int
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
extern unsigned char Cyc_List_Nth[ 8u]; extern void* Cyc_List_nth( struct Cyc_List_List*
x, int i); extern int Cyc_List_exists_c( int(* pred)( void*, void*), void* env,
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_zip( struct Cyc_List_List*
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
Cyc_Absyn_Tqual tq; void* type; struct Cyc_Absyn_Exp* width; struct Cyc_List_List*
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
extern void* Cyc_Absyn_empty_effect; extern struct _tuple1* Cyc_Absyn_tunion_print_arg_qvar;
extern struct _tuple1* Cyc_Absyn_tunion_scanf_arg_qvar; extern void* Cyc_Absyn_bounds_one;
extern void* Cyc_Absyn_atb_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual tq,
void* b); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*);
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
struct Cyc_Tcenv_Tenv*, void* r1, void* r2); extern int Cyc_std_strcmp( struct
_tagged_arr s1, struct _tagged_arr s2); extern int Cyc_std_zstrptrcmp( struct
_tagged_arr*, struct _tagged_arr*); extern struct _tagged_arr Cyc_std_strconcat(
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
extern void Cyc_Tcutil_check_bitfield( struct Cyc_Position_Segment* loc, struct
Cyc_Tcenv_Tenv* te, void* field_typ, struct Cyc_Absyn_Exp* width, struct
_tagged_arr* fn); extern void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp*);
extern void Cyc_Tcutil_check_valid_toplevel_type( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, void*); extern void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Fndecl*);
extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, int allow_evars, void*); extern void
Cyc_Tcutil_check_unique_vars( struct Cyc_List_List* vs, struct Cyc_Position_Segment*
loc, struct _tagged_arr err_msg); extern void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*, struct Cyc_Absyn_Conref*); extern void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*, unsigned int i, struct Cyc_Absyn_Conref*); extern
int Cyc_Tcutil_equal_tqual( struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual
tq2); extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators( struct
_RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List* des,
struct Cyc_List_List* fields); extern int Cyc_Tcutil_is_tagged_pointer_typ( void*);
extern int Cyc_Tcutil_is_tagged_pointer_typ_elt( void* t, void** elt_typ_dest);
extern void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv*, void* t, struct
Cyc_Absyn_Exp* e); struct _tuple7{ int f1; void* f2; } ; extern struct _tuple7
Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e);
extern struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar( void* k); extern int* Cyc_Tcutil_new_tvar_id();
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
Cyc_Position_mk_err_elab( loc,( struct _tagged_arr) Cyc_std_vraprintf( Cyc_Core_heap_region,
fmt, ap)));} void* Cyc_Tcutil_impos( struct _tagged_arr fmt, struct _tagged_arr
ap){ struct _tagged_arr msg=( struct _tagged_arr) Cyc_std_vraprintf( Cyc_Core_heap_region,
fmt, ap);({ struct Cyc_std_String_pa_struct _temp1; _temp1.tag= Cyc_std_String_pa;
_temp1.f1=( struct _tagged_arr) msg;{ void* _temp0[ 1u]={& _temp1}; Cyc_std_fprintf(
Cyc_std_stderr, _tag_arr("Error: %s\n", sizeof( unsigned char), 11u), _tag_arr(
_temp0, sizeof( void*), 1u));}}); Cyc_std_fflush(( struct Cyc_std___sFILE*) Cyc_std_stderr);(
int) _throw(( void*) Cyc_Tcutil_TypeErr);} static struct _tagged_arr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar* tv){ return* tv->name;} void Cyc_Tcutil_print_tvars(
struct Cyc_List_List* tvs){ for( 0; tvs !=  0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){({ struct Cyc_std_String_pa_struct _temp4; _temp4.tag=
Cyc_std_String_pa; _temp4.f1=( struct _tagged_arr) Cyc_Absynpp_ckind2string(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);{
struct Cyc_std_String_pa_struct _temp3; _temp3.tag= Cyc_std_String_pa; _temp3.f1=(
struct _tagged_arr) Cyc_Tcutil_tvar2string(( struct Cyc_Absyn_Tvar*)(( struct
Cyc_List_List*) _check_null( tvs))->hd);{ void* _temp2[ 2u]={& _temp3,& _temp4};
Cyc_std_fprintf( Cyc_std_stderr, _tag_arr("%s::%s ", sizeof( unsigned char), 8u),
_tag_arr( _temp2, sizeof( void*), 2u));}}});}({ void* _temp5[ 0u]={}; Cyc_std_fprintf(
Cyc_std_stderr, _tag_arr("\n", sizeof( unsigned char), 2u), _tag_arr( _temp5,
sizeof( void*), 0u));}); Cyc_std_fflush(( struct Cyc_std___sFILE*) Cyc_std_stderr);}
static struct Cyc_List_List* Cyc_Tcutil_warning_segs= 0; static struct Cyc_List_List*
Cyc_Tcutil_warning_msgs= 0; void Cyc_Tcutil_warn( struct Cyc_Position_Segment*
sg, struct _tagged_arr fmt, struct _tagged_arr ap){ struct _tagged_arr msg=(
struct _tagged_arr) Cyc_std_vraprintf( Cyc_Core_heap_region, fmt, ap); Cyc_Tcutil_warning_segs=({
struct Cyc_List_List* _temp6=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp6->hd=( void*) sg; _temp6->tl= Cyc_Tcutil_warning_segs;
_temp6;}); Cyc_Tcutil_warning_msgs=({ struct Cyc_List_List* _temp7=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp7->hd=( void*)({ struct
_tagged_arr* _temp8=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr));
_temp8[ 0]= msg; _temp8;}); _temp7->tl= Cyc_Tcutil_warning_msgs; _temp7;});}
void Cyc_Tcutil_flush_warnings(){ if( Cyc_Tcutil_warning_segs ==  0){ return;}({
void* _temp9[ 0u]={}; Cyc_std_fprintf( Cyc_std_stderr, _tag_arr("***Warnings***\n",
sizeof( unsigned char), 16u), _tag_arr( _temp9, sizeof( void*), 0u));});{ struct
Cyc_List_List* _temp10= Cyc_Position_strings_of_segments( Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs= 0; Cyc_Tcutil_warning_msgs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_Tcutil_warning_msgs); while(
Cyc_Tcutil_warning_msgs !=  0) {({ struct Cyc_std_String_pa_struct _temp13;
_temp13.tag= Cyc_std_String_pa; _temp13.f1=( struct _tagged_arr)*(( struct
_tagged_arr*)(( struct Cyc_List_List*) _check_null( Cyc_Tcutil_warning_msgs))->hd);{
struct Cyc_std_String_pa_struct _temp12; _temp12.tag= Cyc_std_String_pa; _temp12.f1=(
struct _tagged_arr)*(( struct _tagged_arr*)(( struct Cyc_List_List*) _check_null(
_temp10))->hd);{ void* _temp11[ 2u]={& _temp12,& _temp13}; Cyc_std_fprintf( Cyc_std_stderr,
_tag_arr("%s: %s\n", sizeof( unsigned char), 8u), _tag_arr( _temp11, sizeof(
void*), 2u));}}}); _temp10=(( struct Cyc_List_List*) _check_null( _temp10))->tl;
Cyc_Tcutil_warning_msgs=(( struct Cyc_List_List*) _check_null( Cyc_Tcutil_warning_msgs))->tl;}({
void* _temp14[ 0u]={}; Cyc_std_fprintf( Cyc_std_stderr, _tag_arr("**************\n",
sizeof( unsigned char), 16u), _tag_arr( _temp14, sizeof( void*), 0u));}); Cyc_std_fflush((
struct Cyc_std___sFILE*) Cyc_std_stderr);}} struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set=
0; static int Cyc_Tcutil_fast_tvar_cmp( struct Cyc_Absyn_Tvar* tv1, struct Cyc_Absyn_Tvar*
tv2){ return*(( int*) _check_null( tv1->identity)) - *(( int*) _check_null( tv2->identity));}
void* Cyc_Tcutil_compress( void* t){ void* _temp15= t; struct Cyc_Core_Opt*
_temp27; struct Cyc_Core_Opt* _temp29; struct Cyc_Core_Opt** _temp31; struct Cyc_Core_Opt*
_temp32; struct Cyc_Core_Opt* _temp34; struct Cyc_Core_Opt** _temp36; _LL17: if((
unsigned int) _temp15 >  4u?*(( int*) _temp15) ==  Cyc_Absyn_Evar: 0){ _LL28:
_temp27=(( struct Cyc_Absyn_Evar_struct*) _temp15)->f2; if( _temp27 ==  0){ goto
_LL18;} else{ goto _LL19;}} else{ goto _LL19;} _LL19: if(( unsigned int) _temp15
>  4u?*(( int*) _temp15) ==  Cyc_Absyn_Evar: 0){ _LL30: _temp29=(( struct Cyc_Absyn_Evar_struct*)
_temp15)->f2; _temp31=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp15)->f2; goto _LL20;} else{ goto _LL21;} _LL21: if(( unsigned int) _temp15
>  4u?*(( int*) _temp15) ==  Cyc_Absyn_TypedefType: 0){ _LL33: _temp32=(( struct
Cyc_Absyn_TypedefType_struct*) _temp15)->f3; if( _temp32 ==  0){ goto _LL22;}
else{ goto _LL23;}} else{ goto _LL23;} _LL23: if(( unsigned int) _temp15 >  4u?*((
int*) _temp15) ==  Cyc_Absyn_TypedefType: 0){ _LL35: _temp34=(( struct Cyc_Absyn_TypedefType_struct*)
_temp15)->f3; _temp36=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_TypedefType_struct*)
_temp15)->f3; goto _LL24;} else{ goto _LL25;} _LL25: goto _LL26; _LL18: return t;
_LL20: { void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null(* _temp31))->v); if( t2 != ( void*)(( struct Cyc_Core_Opt*)
_check_null(* _temp31))->v){* _temp31=({ struct Cyc_Core_Opt* _temp37=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp37->v=( void*) t2;
_temp37;});} return t2;} _LL22: return t; _LL24: { void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null(* _temp36))->v); if( t2 != ( void*)((
struct Cyc_Core_Opt*) _check_null(* _temp36))->v){* _temp36=({ struct Cyc_Core_Opt*
_temp38=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp38->v=( void*) t2; _temp38;});} return t2;} _LL26: return t; _LL16:;} void*
Cyc_Tcutil_copy_type( void* t); static struct Cyc_List_List* Cyc_Tcutil_copy_types(
struct Cyc_List_List* ts){ return Cyc_List_map( Cyc_Tcutil_copy_type, ts);}
static struct Cyc_Absyn_Conref* Cyc_Tcutil_copy_conref( struct Cyc_Absyn_Conref*
c){ void* _temp39=( void*) c->v; void* _temp47; struct Cyc_Absyn_Conref* _temp49;
_LL41: if( _temp39 == ( void*) Cyc_Absyn_No_constr){ goto _LL42;} else{ goto
_LL43;} _LL43: if(( unsigned int) _temp39 >  1u?*(( int*) _temp39) ==  Cyc_Absyn_Eq_constr:
0){ _LL48: _temp47=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp39)->f1;
goto _LL44;} else{ goto _LL45;} _LL45: if(( unsigned int) _temp39 >  1u?*(( int*)
_temp39) ==  Cyc_Absyn_Forward_constr: 0){ _LL50: _temp49=(( struct Cyc_Absyn_Forward_constr_struct*)
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
_temp125; struct Cyc_Absyn_TunionInfo _temp127; void* _temp129; struct Cyc_List_List*
_temp131; void* _temp133; struct Cyc_Absyn_TunionFieldInfo _temp135; struct Cyc_List_List*
_temp137; void* _temp139; struct Cyc_Absyn_PtrInfo _temp141; struct Cyc_Absyn_Conref*
_temp143; struct Cyc_Absyn_Tqual _temp145; struct Cyc_Absyn_Conref* _temp147;
void* _temp149; void* _temp151; void* _temp153; void* _temp155; struct Cyc_Absyn_Exp*
_temp157; struct Cyc_Absyn_Tqual _temp159; void* _temp161; struct Cyc_Absyn_FnInfo
_temp163; struct Cyc_List_List* _temp165; struct Cyc_List_List* _temp167; struct
Cyc_Absyn_VarargInfo* _temp169; int _temp171; struct Cyc_List_List* _temp173;
void* _temp175; struct Cyc_Core_Opt* _temp177; struct Cyc_List_List* _temp179;
struct Cyc_List_List* _temp181; struct Cyc_List_List* _temp183; struct _tuple1*
_temp185; struct Cyc_List_List* _temp187; struct _tuple1* _temp189; struct Cyc_List_List*
_temp191; struct Cyc_List_List* _temp193; struct _tuple1* _temp195; void*
_temp197; struct Cyc_List_List* _temp199; struct _tuple1* _temp201; void*
_temp203; struct Cyc_List_List* _temp205; void* _temp207; _LL79: if( _temp77 == (
void*) Cyc_Absyn_VoidType){ goto _LL80;} else{ goto _LL81;} _LL81: if((
unsigned int) _temp77 >  4u?*(( int*) _temp77) ==  Cyc_Absyn_Evar: 0){ goto
_LL82;} else{ goto _LL83;} _LL83: if(( unsigned int) _temp77 >  4u?*(( int*)
_temp77) ==  Cyc_Absyn_VarType: 0){ _LL126: _temp125=(( struct Cyc_Absyn_VarType_struct*)
_temp77)->f1; goto _LL84;} else{ goto _LL85;} _LL85: if(( unsigned int) _temp77
>  4u?*(( int*) _temp77) ==  Cyc_Absyn_TunionType: 0){ _LL128: _temp127=((
struct Cyc_Absyn_TunionType_struct*) _temp77)->f1; _LL134: _temp133=( void*)
_temp127.tunion_info; goto _LL132; _LL132: _temp131= _temp127.targs; goto _LL130;
_LL130: _temp129=( void*) _temp127.rgn; goto _LL86;} else{ goto _LL87;} _LL87:
if(( unsigned int) _temp77 >  4u?*(( int*) _temp77) ==  Cyc_Absyn_TunionFieldType:
0){ _LL136: _temp135=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp77)->f1;
_LL140: _temp139=( void*) _temp135.field_info; goto _LL138; _LL138: _temp137=
_temp135.targs; goto _LL88;} else{ goto _LL89;} _LL89: if(( unsigned int)
_temp77 >  4u?*(( int*) _temp77) ==  Cyc_Absyn_PointerType: 0){ _LL142: _temp141=((
struct Cyc_Absyn_PointerType_struct*) _temp77)->f1; _LL152: _temp151=( void*)
_temp141.elt_typ; goto _LL150; _LL150: _temp149=( void*) _temp141.rgn_typ; goto
_LL148; _LL148: _temp147= _temp141.nullable; goto _LL146; _LL146: _temp145=
_temp141.tq; goto _LL144; _LL144: _temp143= _temp141.bounds; goto _LL90;} else{
goto _LL91;} _LL91: if(( unsigned int) _temp77 >  4u?*(( int*) _temp77) ==  Cyc_Absyn_IntType:
0){ _LL156: _temp155=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp77)->f1;
goto _LL154; _LL154: _temp153=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp77)->f2; goto _LL92;} else{ goto _LL93;} _LL93: if( _temp77 == ( void*) Cyc_Absyn_FloatType){
goto _LL94;} else{ goto _LL95;} _LL95: if( _temp77 == ( void*) Cyc_Absyn_DoubleType){
goto _LL96;} else{ goto _LL97;} _LL97: if(( unsigned int) _temp77 >  4u?*(( int*)
_temp77) ==  Cyc_Absyn_ArrayType: 0){ _LL162: _temp161=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp77)->f1; goto _LL160; _LL160: _temp159=(( struct Cyc_Absyn_ArrayType_struct*)
_temp77)->f2; goto _LL158; _LL158: _temp157=(( struct Cyc_Absyn_ArrayType_struct*)
_temp77)->f3; goto _LL98;} else{ goto _LL99;} _LL99: if(( unsigned int) _temp77
>  4u?*(( int*) _temp77) ==  Cyc_Absyn_FnType: 0){ _LL164: _temp163=(( struct
Cyc_Absyn_FnType_struct*) _temp77)->f1; _LL180: _temp179= _temp163.tvars; goto
_LL178; _LL178: _temp177= _temp163.effect; goto _LL176; _LL176: _temp175=( void*)
_temp163.ret_typ; goto _LL174; _LL174: _temp173= _temp163.args; goto _LL172;
_LL172: _temp171= _temp163.c_varargs; goto _LL170; _LL170: _temp169= _temp163.cyc_varargs;
goto _LL168; _LL168: _temp167= _temp163.rgn_po; goto _LL166; _LL166: _temp165=
_temp163.attributes; goto _LL100;} else{ goto _LL101;} _LL101: if(( unsigned int)
_temp77 >  4u?*(( int*) _temp77) ==  Cyc_Absyn_TupleType: 0){ _LL182: _temp181=((
struct Cyc_Absyn_TupleType_struct*) _temp77)->f1; goto _LL102;} else{ goto
_LL103;} _LL103: if(( unsigned int) _temp77 >  4u?*(( int*) _temp77) ==  Cyc_Absyn_StructType:
0){ _LL186: _temp185=(( struct Cyc_Absyn_StructType_struct*) _temp77)->f1; goto
_LL184; _LL184: _temp183=(( struct Cyc_Absyn_StructType_struct*) _temp77)->f2;
goto _LL104;} else{ goto _LL105;} _LL105: if(( unsigned int) _temp77 >  4u?*((
int*) _temp77) ==  Cyc_Absyn_UnionType: 0){ _LL190: _temp189=(( struct Cyc_Absyn_UnionType_struct*)
_temp77)->f1; goto _LL188; _LL188: _temp187=(( struct Cyc_Absyn_UnionType_struct*)
_temp77)->f2; goto _LL106;} else{ goto _LL107;} _LL107: if(( unsigned int)
_temp77 >  4u?*(( int*) _temp77) ==  Cyc_Absyn_AnonStructType: 0){ _LL192:
_temp191=(( struct Cyc_Absyn_AnonStructType_struct*) _temp77)->f1; goto _LL108;}
else{ goto _LL109;} _LL109: if(( unsigned int) _temp77 >  4u?*(( int*) _temp77)
==  Cyc_Absyn_AnonUnionType: 0){ _LL194: _temp193=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp77)->f1; goto _LL110;} else{ goto _LL111;} _LL111: if(( unsigned int)
_temp77 >  4u?*(( int*) _temp77) ==  Cyc_Absyn_EnumType: 0){ _LL196: _temp195=((
struct Cyc_Absyn_EnumType_struct*) _temp77)->f1; goto _LL112;} else{ goto _LL113;}
_LL113: if(( unsigned int) _temp77 >  4u?*(( int*) _temp77) ==  Cyc_Absyn_RgnHandleType:
0){ _LL198: _temp197=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp77)->f1;
goto _LL114;} else{ goto _LL115;} _LL115: if(( unsigned int) _temp77 >  4u?*((
int*) _temp77) ==  Cyc_Absyn_TypedefType: 0){ _LL202: _temp201=(( struct Cyc_Absyn_TypedefType_struct*)
_temp77)->f1; goto _LL200; _LL200: _temp199=(( struct Cyc_Absyn_TypedefType_struct*)
_temp77)->f2; goto _LL116;} else{ goto _LL117;} _LL117: if( _temp77 == ( void*)
Cyc_Absyn_HeapRgn){ goto _LL118;} else{ goto _LL119;} _LL119: if(( unsigned int)
_temp77 >  4u?*(( int*) _temp77) ==  Cyc_Absyn_AccessEff: 0){ _LL204: _temp203=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp77)->f1; goto _LL120;} else{
goto _LL121;} _LL121: if(( unsigned int) _temp77 >  4u?*(( int*) _temp77) == 
Cyc_Absyn_JoinEff: 0){ _LL206: _temp205=(( struct Cyc_Absyn_JoinEff_struct*)
_temp77)->f1; goto _LL122;} else{ goto _LL123;} _LL123: if(( unsigned int)
_temp77 >  4u?*(( int*) _temp77) ==  Cyc_Absyn_RgnsEff: 0){ _LL208: _temp207=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp77)->f1; goto _LL124;} else{
goto _LL78;} _LL80: return t; _LL82: return t; _LL84: return( void*)({ struct
Cyc_Absyn_VarType_struct* _temp209=( struct Cyc_Absyn_VarType_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_VarType_struct)); _temp209[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp210; _temp210.tag= Cyc_Absyn_VarType; _temp210.f1= Cyc_Tcutil_copy_tvar(
_temp125); _temp210;}); _temp209;}); _LL86: return( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp211=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp211[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp212; _temp212.tag= Cyc_Absyn_TunionType;
_temp212.f1=({ struct Cyc_Absyn_TunionInfo _temp213; _temp213.tunion_info=( void*)
_temp133; _temp213.targs= Cyc_Tcutil_copy_types( _temp131); _temp213.rgn=( void*)
Cyc_Tcutil_copy_type( _temp129); _temp213;}); _temp212;}); _temp211;}); _LL88:
return( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp214=( struct Cyc_Absyn_TunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp214[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp215; _temp215.tag= Cyc_Absyn_TunionFieldType;
_temp215.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp216; _temp216.field_info=(
void*) _temp139; _temp216.targs= Cyc_Tcutil_copy_types( _temp137); _temp216;});
_temp215;}); _temp214;}); _LL90: { void* _temp217= Cyc_Tcutil_copy_type(
_temp151); void* _temp218= Cyc_Tcutil_copy_type( _temp149); struct Cyc_Absyn_Conref*
_temp219=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* c)) Cyc_Tcutil_copy_conref)(
_temp147); struct Cyc_Absyn_Tqual _temp220= _temp145; struct Cyc_Absyn_Conref*
_temp221= Cyc_Tcutil_copy_conref( _temp143); return( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp222=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp222[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp223; _temp223.tag= Cyc_Absyn_PointerType;
_temp223.f1=({ struct Cyc_Absyn_PtrInfo _temp224; _temp224.elt_typ=( void*)
_temp217; _temp224.rgn_typ=( void*) _temp218; _temp224.nullable= _temp219;
_temp224.tq= _temp220; _temp224.bounds= _temp221; _temp224;}); _temp223;});
_temp222;});} _LL92: return( void*)({ struct Cyc_Absyn_IntType_struct* _temp225=(
struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp225[ 0]=({ struct Cyc_Absyn_IntType_struct _temp226; _temp226.tag= Cyc_Absyn_IntType;
_temp226.f1=( void*) _temp155; _temp226.f2=( void*) _temp153; _temp226;});
_temp225;}); _LL94: goto _LL96; _LL96: return t; _LL98: return( void*)({ struct
Cyc_Absyn_ArrayType_struct* _temp227=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp227[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp228; _temp228.tag= Cyc_Absyn_ArrayType; _temp228.f1=(
void*) Cyc_Tcutil_copy_type( _temp161); _temp228.f2= _temp159; _temp228.f3=
_temp157; _temp228;}); _temp227;}); _LL100: { struct Cyc_List_List* _temp229=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_tvar, _temp179); struct
Cyc_Core_Opt* _temp230= _temp177 ==  0? 0:({ struct Cyc_Core_Opt* _temp240=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp240->v=(
void*) Cyc_Tcutil_copy_type(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp177))->v); _temp240;}); void* _temp231= Cyc_Tcutil_copy_type( _temp175);
struct Cyc_List_List* _temp232=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)(
struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_arg,
_temp173); int _temp233= _temp171; struct Cyc_Absyn_VarargInfo* cyc_varargs2= 0;
if( _temp169 !=  0){ struct Cyc_Absyn_VarargInfo* cv=( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp169); cyc_varargs2=({ struct Cyc_Absyn_VarargInfo* _temp234=(
struct Cyc_Absyn_VarargInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp234->name= cv->name; _temp234->tq= cv->tq; _temp234->type=( void*) Cyc_Tcutil_copy_type((
void*) cv->type); _temp234->rgn=( void*) Cyc_Tcutil_copy_type(( void*) cv->rgn);
_temp234->inject= cv->inject; _temp234;});}{ struct Cyc_List_List* _temp235=((
struct Cyc_List_List*(*)( struct _tuple8*(* f)( struct _tuple8*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_copy_rgncmp, _temp167); struct Cyc_List_List*
_temp236= _temp165; return( void*)({ struct Cyc_Absyn_FnType_struct* _temp237=(
struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp237[ 0]=({ struct Cyc_Absyn_FnType_struct _temp238; _temp238.tag= Cyc_Absyn_FnType;
_temp238.f1=({ struct Cyc_Absyn_FnInfo _temp239; _temp239.tvars= _temp229;
_temp239.effect= _temp230; _temp239.ret_typ=( void*) _temp231; _temp239.args=
_temp232; _temp239.c_varargs= _temp233; _temp239.cyc_varargs= cyc_varargs2;
_temp239.rgn_po= _temp235; _temp239.attributes= _temp236; _temp239;}); _temp238;});
_temp237;});}} _LL102: return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp241=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp241[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp242; _temp242.tag= Cyc_Absyn_TupleType;
_temp242.f1=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_tqt, _temp181);
_temp242;}); _temp241;}); _LL104: return( void*)({ struct Cyc_Absyn_StructType_struct*
_temp243=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp243[ 0]=({ struct Cyc_Absyn_StructType_struct _temp244; _temp244.tag= Cyc_Absyn_StructType;
_temp244.f1= _temp185; _temp244.f2= Cyc_Tcutil_copy_types( _temp183); _temp244.f3=
0; _temp244;}); _temp243;}); _LL106: return( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp245=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp245[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp246; _temp246.tag= Cyc_Absyn_UnionType;
_temp246.f1= _temp189; _temp246.f2= Cyc_Tcutil_copy_types( _temp187); _temp246.f3=
0; _temp246;}); _temp245;}); _LL108: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp247=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp247[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp248; _temp248.tag= Cyc_Absyn_AnonStructType; _temp248.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_copy_field, _temp191); _temp248;}); _temp247;});
_LL110: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp249=(
struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp249[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp250; _temp250.tag=
Cyc_Absyn_AnonUnionType; _temp250.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_field,
_temp193); _temp250;}); _temp249;}); _LL112: return( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp251=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp251[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp252; _temp252.tag= Cyc_Absyn_EnumType;
_temp252.f1= _temp195; _temp252.f2= 0; _temp252;}); _temp251;}); _LL114: return(
void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp253=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp253[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp254; _temp254.tag= Cyc_Absyn_RgnHandleType;
_temp254.f1=( void*) Cyc_Tcutil_copy_type( _temp197); _temp254;}); _temp253;});
_LL116: return( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp255=( struct
Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp255[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp256; _temp256.tag= Cyc_Absyn_TypedefType;
_temp256.f1= _temp201; _temp256.f2= Cyc_Tcutil_copy_types( _temp199); _temp256.f3=
0; _temp256;}); _temp255;}); _LL118: return t; _LL120: return( void*)({ struct
Cyc_Absyn_AccessEff_struct* _temp257=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp257[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp258; _temp258.tag= Cyc_Absyn_AccessEff; _temp258.f1=(
void*) Cyc_Tcutil_copy_type( _temp203); _temp258;}); _temp257;}); _LL122: return(
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp259=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp259[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp260; _temp260.tag= Cyc_Absyn_JoinEff; _temp260.f1= Cyc_Tcutil_copy_types(
_temp205); _temp260;}); _temp259;}); _LL124: return( void*)({ struct Cyc_Absyn_RgnsEff_struct*
_temp261=( struct Cyc_Absyn_RgnsEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp261[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp262; _temp262.tag= Cyc_Absyn_RgnsEff;
_temp262.f1=( void*) Cyc_Tcutil_copy_type( _temp207); _temp262;}); _temp261;});
_LL78:;} static int Cyc_Tcutil_kind_leq( void* k1, void* k2){ if( k1 ==  k2){
return 1;}{ struct _tuple8 _temp264=({ struct _tuple8 _temp263; _temp263.f1= k1;
_temp263.f2= k2; _temp263;}); void* _temp274; void* _temp276; void* _temp278;
void* _temp280; void* _temp282; void* _temp284; _LL266: _LL277: _temp276=
_temp264.f1; if( _temp276 == ( void*) Cyc_Absyn_BoxKind){ goto _LL275;} else{
goto _LL268;} _LL275: _temp274= _temp264.f2; if( _temp274 == ( void*) Cyc_Absyn_MemKind){
goto _LL267;} else{ goto _LL268;} _LL268: _LL281: _temp280= _temp264.f1; if(
_temp280 == ( void*) Cyc_Absyn_BoxKind){ goto _LL279;} else{ goto _LL270;}
_LL279: _temp278= _temp264.f2; if( _temp278 == ( void*) Cyc_Absyn_AnyKind){ goto
_LL269;} else{ goto _LL270;} _LL270: _LL285: _temp284= _temp264.f1; if( _temp284
== ( void*) Cyc_Absyn_MemKind){ goto _LL283;} else{ goto _LL272;} _LL283:
_temp282= _temp264.f2; if( _temp282 == ( void*) Cyc_Absyn_AnyKind){ goto _LL271;}
else{ goto _LL272;} _LL272: goto _LL273; _LL267: return 1; _LL269: return 1;
_LL271: return 1; _LL273: return 0; _LL265:;}} void* Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar* tv){ return Cyc_Absyn_conref_val( tv->kind);} void* Cyc_Tcutil_typ_kind(
void* t){ void* _temp286= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt* _temp340;
struct Cyc_Core_Opt* _temp342; struct Cyc_Absyn_Tvar* _temp344; void* _temp346;
struct Cyc_Absyn_TunionFieldInfo _temp348; void* _temp350; struct Cyc_Absyn_Tunionfield*
_temp352; struct Cyc_Absyn_TunionFieldInfo _temp354; void* _temp356; struct Cyc_Absyn_Structdecl**
_temp358; struct Cyc_Absyn_Uniondecl** _temp360; struct Cyc_Absyn_PtrInfo
_temp362; void* _temp364; struct Cyc_List_List* _temp366; struct Cyc_Core_Opt*
_temp368; _LL288: if(( unsigned int) _temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_Evar:
0){ _LL343: _temp342=(( struct Cyc_Absyn_Evar_struct*) _temp286)->f1; goto
_LL341; _LL341: _temp340=(( struct Cyc_Absyn_Evar_struct*) _temp286)->f2; goto
_LL289;} else{ goto _LL290;} _LL290: if(( unsigned int) _temp286 >  4u?*(( int*)
_temp286) ==  Cyc_Absyn_VarType: 0){ _LL345: _temp344=(( struct Cyc_Absyn_VarType_struct*)
_temp286)->f1; goto _LL291;} else{ goto _LL292;} _LL292: if( _temp286 == ( void*)
Cyc_Absyn_VoidType){ goto _LL293;} else{ goto _LL294;} _LL294: if(( unsigned int)
_temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_IntType: 0){ _LL347: _temp346=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp286)->f2; goto _LL295;} else{
goto _LL296;} _LL296: if( _temp286 == ( void*) Cyc_Absyn_FloatType){ goto _LL297;}
else{ goto _LL298;} _LL298: if( _temp286 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL299;} else{ goto _LL300;} _LL300: if(( unsigned int) _temp286 >  4u?*(( int*)
_temp286) ==  Cyc_Absyn_FnType: 0){ goto _LL301;} else{ goto _LL302;} _LL302:
if(( unsigned int) _temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_RgnHandleType:
0){ goto _LL303;} else{ goto _LL304;} _LL304: if( _temp286 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL305;} else{ goto _LL306;} _LL306: if(( unsigned int) _temp286 >  4u?*((
int*) _temp286) ==  Cyc_Absyn_TunionType: 0){ goto _LL307;} else{ goto _LL308;}
_LL308: if(( unsigned int) _temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_TunionFieldType:
0){ _LL349: _temp348=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp286)->f1;
_LL351: _temp350=( void*) _temp348.field_info; if(*(( int*) _temp350) ==  Cyc_Absyn_KnownTunionfield){
_LL353: _temp352=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp350)->f2;
goto _LL309;} else{ goto _LL310;}} else{ goto _LL310;} _LL310: if(( unsigned int)
_temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_TunionFieldType: 0){ _LL355:
_temp354=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp286)->f1; _LL357:
_temp356=( void*) _temp354.field_info; if(*(( int*) _temp356) ==  Cyc_Absyn_UnknownTunionfield){
goto _LL311;} else{ goto _LL312;}} else{ goto _LL312;} _LL312: if(( unsigned int)
_temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_StructType: 0){ _LL359:
_temp358=(( struct Cyc_Absyn_StructType_struct*) _temp286)->f3; if( _temp358 == 
0){ goto _LL313;} else{ goto _LL314;}} else{ goto _LL314;} _LL314: if((
unsigned int) _temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_UnionType: 0){
_LL361: _temp360=(( struct Cyc_Absyn_UnionType_struct*) _temp286)->f3; if(
_temp360 ==  0){ goto _LL315;} else{ goto _LL316;}} else{ goto _LL316;} _LL316:
if(( unsigned int) _temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_StructType:
0){ goto _LL317;} else{ goto _LL318;} _LL318: if(( unsigned int) _temp286 >  4u?*((
int*) _temp286) ==  Cyc_Absyn_UnionType: 0){ goto _LL319;} else{ goto _LL320;}
_LL320: if(( unsigned int) _temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_AnonStructType:
0){ goto _LL321;} else{ goto _LL322;} _LL322: if(( unsigned int) _temp286 >  4u?*((
int*) _temp286) ==  Cyc_Absyn_AnonUnionType: 0){ goto _LL323;} else{ goto _LL324;}
_LL324: if(( unsigned int) _temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_EnumType:
0){ goto _LL325;} else{ goto _LL326;} _LL326: if(( unsigned int) _temp286 >  4u?*((
int*) _temp286) ==  Cyc_Absyn_PointerType: 0){ _LL363: _temp362=(( struct Cyc_Absyn_PointerType_struct*)
_temp286)->f1; goto _LL327;} else{ goto _LL328;} _LL328: if(( unsigned int)
_temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_ArrayType: 0){ _LL365: _temp364=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp286)->f1; goto _LL329;} else{
goto _LL330;} _LL330: if(( unsigned int) _temp286 >  4u?*(( int*) _temp286) == 
Cyc_Absyn_TupleType: 0){ _LL367: _temp366=(( struct Cyc_Absyn_TupleType_struct*)
_temp286)->f1; goto _LL331;} else{ goto _LL332;} _LL332: if(( unsigned int)
_temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_TypedefType: 0){ _LL369:
_temp368=(( struct Cyc_Absyn_TypedefType_struct*) _temp286)->f3; goto _LL333;}
else{ goto _LL334;} _LL334: if(( unsigned int) _temp286 >  4u?*(( int*) _temp286)
==  Cyc_Absyn_AccessEff: 0){ goto _LL335;} else{ goto _LL336;} _LL336: if((
unsigned int) _temp286 >  4u?*(( int*) _temp286) ==  Cyc_Absyn_JoinEff: 0){ goto
_LL337;} else{ goto _LL338;} _LL338: if(( unsigned int) _temp286 >  4u?*(( int*)
_temp286) ==  Cyc_Absyn_RgnsEff: 0){ goto _LL339;} else{ goto _LL287;} _LL289:
return( void*)(( struct Cyc_Core_Opt*) _check_null( _temp342))->v; _LL291:
return Cyc_Absyn_conref_val( _temp344->kind); _LL293: return( void*) Cyc_Absyn_MemKind;
_LL295: return _temp346 == ( void*) Cyc_Absyn_B4?( void*) Cyc_Absyn_BoxKind:(
void*) Cyc_Absyn_MemKind; _LL297: return( void*) Cyc_Absyn_MemKind; _LL299:
return( void*) Cyc_Absyn_MemKind; _LL301: return( void*) Cyc_Absyn_MemKind;
_LL303: return( void*) Cyc_Absyn_BoxKind; _LL305: return( void*) Cyc_Absyn_RgnKind;
_LL307: return( void*) Cyc_Absyn_BoxKind; _LL309: if( _temp352->typs ==  0){
return( void*) Cyc_Absyn_BoxKind;} else{ return( void*) Cyc_Absyn_MemKind;}
_LL311: return({ void* _temp370[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("typ_kind: Unresolved TunionFieldType",
sizeof( unsigned char), 37u), _tag_arr( _temp370, sizeof( void*), 0u));});
_LL313: return( void*) Cyc_Absyn_AnyKind; _LL315: return( void*) Cyc_Absyn_AnyKind;
_LL317: return( void*) Cyc_Absyn_MemKind; _LL319: return( void*) Cyc_Absyn_MemKind;
_LL321: return( void*) Cyc_Absyn_MemKind; _LL323: return( void*) Cyc_Absyn_MemKind;
_LL325: return( void*) Cyc_Absyn_BoxKind; _LL327: { void* _temp371=( void*)( Cyc_Absyn_compress_conref(
_temp362.bounds))->v; void* _temp381; void* _temp383; _LL373: if(( unsigned int)
_temp371 >  1u?*(( int*) _temp371) ==  Cyc_Absyn_Eq_constr: 0){ _LL382: _temp381=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp371)->f1; if( _temp381 == (
void*) Cyc_Absyn_Unknown_b){ goto _LL374;} else{ goto _LL375;}} else{ goto
_LL375;} _LL375: if(( unsigned int) _temp371 >  1u?*(( int*) _temp371) ==  Cyc_Absyn_Eq_constr:
0){ _LL384: _temp383=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp371)->f1;
if(( unsigned int) _temp383 >  1u?*(( int*) _temp383) ==  Cyc_Absyn_Upper_b: 0){
goto _LL376;} else{ goto _LL377;}} else{ goto _LL377;} _LL377: if( _temp371 == (
void*) Cyc_Absyn_No_constr){ goto _LL378;} else{ goto _LL379;} _LL379: if((
unsigned int) _temp371 >  1u?*(( int*) _temp371) ==  Cyc_Absyn_Forward_constr: 0){
goto _LL380;} else{ goto _LL372;} _LL374: return( void*) Cyc_Absyn_MemKind;
_LL376: return( void*) Cyc_Absyn_BoxKind; _LL378: return( void*) Cyc_Absyn_MemKind;
_LL380: return({ void* _temp385[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("typ_kind: forward constraint in ptr bounds",
sizeof( unsigned char), 43u), _tag_arr( _temp385, sizeof( void*), 0u));});
_LL372:;} _LL329: return( void*) Cyc_Absyn_MemKind; _LL331: return( void*) Cyc_Absyn_MemKind;
_LL333: return({ struct Cyc_std_String_pa_struct _temp387; _temp387.tag= Cyc_std_String_pa;
_temp387.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp386[ 1u]={&
_temp387}; Cyc_Tcutil_impos( _tag_arr("typ_kind: typedef found: %s", sizeof(
unsigned char), 28u), _tag_arr( _temp386, sizeof( void*), 1u));}}); _LL335:
return( void*) Cyc_Absyn_EffKind; _LL337: return( void*) Cyc_Absyn_EffKind;
_LL339: return( void*) Cyc_Absyn_EffKind; _LL287:;} unsigned char Cyc_Tcutil_Unify[
10u]="\000\000\000\000Unify"; int Cyc_Tcutil_unify( void* t1, void* t2){ struct
_handler_cons _temp388; _push_handler(& _temp388);{ int _temp390= 0; if( setjmp(
_temp388.handler)){ _temp390= 1;} if( ! _temp390){ Cyc_Tcutil_unify_it( t1, t2);{
int _temp391= 1; _npop_handler( 0u); return _temp391;}; _pop_handler();} else{
void* _temp389=( void*) _exn_thrown; void* _temp393= _temp389; _LL395: if(
_temp393 ==  Cyc_Tcutil_Unify){ goto _LL396;} else{ goto _LL397;} _LL397: goto
_LL398; _LL396: return 0; _LL398:( void) _throw( _temp393); _LL394:;}}} static
void Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List* env, struct Cyc_List_List*
ts); static void Cyc_Tcutil_occurs( void* evar, struct Cyc_List_List* env, void*
t){ void* _temp399= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar* _temp435;
struct Cyc_Core_Opt* _temp437; struct Cyc_Core_Opt** _temp439; struct Cyc_Core_Opt*
_temp440; struct Cyc_Absyn_PtrInfo _temp442; void* _temp444; struct Cyc_Absyn_FnInfo
_temp446; struct Cyc_List_List* _temp448; struct Cyc_List_List* _temp450; struct
Cyc_Absyn_VarargInfo* _temp452; int _temp454; struct Cyc_List_List* _temp456;
void* _temp458; struct Cyc_Core_Opt* _temp460; struct Cyc_List_List* _temp462;
struct Cyc_List_List* _temp464; void* _temp466; struct Cyc_Absyn_TunionInfo
_temp468; void* _temp470; struct Cyc_List_List* _temp472; struct Cyc_Core_Opt*
_temp474; struct Cyc_List_List* _temp476; struct Cyc_Absyn_TunionFieldInfo
_temp478; struct Cyc_List_List* _temp480; struct Cyc_List_List* _temp482; struct
Cyc_List_List* _temp484; struct Cyc_List_List* _temp486; void* _temp488; struct
Cyc_List_List* _temp490; void* _temp492; _LL401: if(( unsigned int) _temp399 > 
4u?*(( int*) _temp399) ==  Cyc_Absyn_VarType: 0){ _LL436: _temp435=(( struct Cyc_Absyn_VarType_struct*)
_temp399)->f1; goto _LL402;} else{ goto _LL403;} _LL403: if(( unsigned int)
_temp399 >  4u?*(( int*) _temp399) ==  Cyc_Absyn_Evar: 0){ _LL441: _temp440=((
struct Cyc_Absyn_Evar_struct*) _temp399)->f2; goto _LL438; _LL438: _temp437=((
struct Cyc_Absyn_Evar_struct*) _temp399)->f4; _temp439=( struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*) _temp399)->f4; goto _LL404;} else{ goto _LL405;}
_LL405: if(( unsigned int) _temp399 >  4u?*(( int*) _temp399) ==  Cyc_Absyn_PointerType:
0){ _LL443: _temp442=(( struct Cyc_Absyn_PointerType_struct*) _temp399)->f1;
goto _LL406;} else{ goto _LL407;} _LL407: if(( unsigned int) _temp399 >  4u?*((
int*) _temp399) ==  Cyc_Absyn_ArrayType: 0){ _LL445: _temp444=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp399)->f1; goto _LL408;} else{ goto _LL409;}
_LL409: if(( unsigned int) _temp399 >  4u?*(( int*) _temp399) ==  Cyc_Absyn_FnType:
0){ _LL447: _temp446=(( struct Cyc_Absyn_FnType_struct*) _temp399)->f1; _LL463:
_temp462= _temp446.tvars; goto _LL461; _LL461: _temp460= _temp446.effect; goto
_LL459; _LL459: _temp458=( void*) _temp446.ret_typ; goto _LL457; _LL457:
_temp456= _temp446.args; goto _LL455; _LL455: _temp454= _temp446.c_varargs; goto
_LL453; _LL453: _temp452= _temp446.cyc_varargs; goto _LL451; _LL451: _temp450=
_temp446.rgn_po; goto _LL449; _LL449: _temp448= _temp446.attributes; goto _LL410;}
else{ goto _LL411;} _LL411: if(( unsigned int) _temp399 >  4u?*(( int*) _temp399)
==  Cyc_Absyn_TupleType: 0){ _LL465: _temp464=(( struct Cyc_Absyn_TupleType_struct*)
_temp399)->f1; goto _LL412;} else{ goto _LL413;} _LL413: if(( unsigned int)
_temp399 >  4u?*(( int*) _temp399) ==  Cyc_Absyn_RgnHandleType: 0){ _LL467:
_temp466=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp399)->f1; goto
_LL414;} else{ goto _LL415;} _LL415: if(( unsigned int) _temp399 >  4u?*(( int*)
_temp399) ==  Cyc_Absyn_TunionType: 0){ _LL469: _temp468=(( struct Cyc_Absyn_TunionType_struct*)
_temp399)->f1; _LL473: _temp472= _temp468.targs; goto _LL471; _LL471: _temp470=(
void*) _temp468.rgn; goto _LL416;} else{ goto _LL417;} _LL417: if(( unsigned int)
_temp399 >  4u?*(( int*) _temp399) ==  Cyc_Absyn_TypedefType: 0){ _LL477:
_temp476=(( struct Cyc_Absyn_TypedefType_struct*) _temp399)->f2; goto _LL475;
_LL475: _temp474=(( struct Cyc_Absyn_TypedefType_struct*) _temp399)->f3; goto
_LL418;} else{ goto _LL419;} _LL419: if(( unsigned int) _temp399 >  4u?*(( int*)
_temp399) ==  Cyc_Absyn_TunionFieldType: 0){ _LL479: _temp478=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp399)->f1; _LL481: _temp480= _temp478.targs; goto _LL420;} else{ goto _LL421;}
_LL421: if(( unsigned int) _temp399 >  4u?*(( int*) _temp399) ==  Cyc_Absyn_StructType:
0){ _LL483: _temp482=(( struct Cyc_Absyn_StructType_struct*) _temp399)->f2; goto
_LL422;} else{ goto _LL423;} _LL423: if(( unsigned int) _temp399 >  4u?*(( int*)
_temp399) ==  Cyc_Absyn_AnonStructType: 0){ _LL485: _temp484=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp399)->f1; goto _LL424;} else{ goto _LL425;} _LL425: if(( unsigned int)
_temp399 >  4u?*(( int*) _temp399) ==  Cyc_Absyn_AnonUnionType: 0){ _LL487:
_temp486=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp399)->f1; goto _LL426;}
else{ goto _LL427;} _LL427: if(( unsigned int) _temp399 >  4u?*(( int*) _temp399)
==  Cyc_Absyn_AccessEff: 0){ _LL489: _temp488=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp399)->f1; goto _LL428;} else{ goto _LL429;} _LL429: if(( unsigned int)
_temp399 >  4u?*(( int*) _temp399) ==  Cyc_Absyn_JoinEff: 0){ _LL491: _temp490=((
struct Cyc_Absyn_JoinEff_struct*) _temp399)->f1; goto _LL430;} else{ goto _LL431;}
_LL431: if(( unsigned int) _temp399 >  4u?*(( int*) _temp399) ==  Cyc_Absyn_RgnsEff:
0){ _LL493: _temp492=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp399)->f1;
goto _LL432;} else{ goto _LL433;} _LL433: goto _LL434; _LL402: if( !(( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,
_temp435)){( int) _throw(( void*) Cyc_Tcutil_Unify);} goto _LL400; _LL404: if( t
==  evar){( int) _throw(( void*) Cyc_Tcutil_Unify);} else{ if( _temp440 !=  0){
Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp440))->v);} else{ int problem= 0;{ struct Cyc_List_List* s=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(* _temp439))->v; for( 0; s !=  0; s=(( struct
Cyc_List_List*) _check_null( s))->tl){ if( !(( int(*)( int(* compare)( struct
Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar*
x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( s))->hd)){ problem= 1; break;}}} if( problem){
struct Cyc_List_List* _temp494= 0;{ struct Cyc_List_List* s=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(* _temp439))->v; for( 0; s !=  0; s=(( struct
Cyc_List_List*) _check_null( s))->tl){ if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( s))->hd)){ _temp494=({ struct Cyc_List_List* _temp495=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp495->hd=( void*)(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( s))->hd); _temp495->tl= _temp494; _temp495;});}}}*
_temp439=({ struct Cyc_Core_Opt* _temp496=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp496->v=( void*) _temp494; _temp496;});}}}
goto _LL400; _LL406: Cyc_Tcutil_occurs( evar, env,( void*) _temp442.elt_typ);
Cyc_Tcutil_occurs( evar, env,( void*) _temp442.rgn_typ); goto _LL400; _LL408:
Cyc_Tcutil_occurs( evar, env, _temp444); goto _LL400; _LL410: if( _temp460 !=  0){
Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp460))->v);} Cyc_Tcutil_occurs( evar, env, _temp458); for( 0; _temp456 !=  0;
_temp456=(( struct Cyc_List_List*) _check_null( _temp456))->tl){ Cyc_Tcutil_occurs(
evar, env,(*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp456))->hd)).f3);}
if( _temp452 !=  0){ void* _temp499; void* _temp501; struct Cyc_Absyn_VarargInfo
_temp497=*(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp452)); _LL502:
_temp501=( void*) _temp497.type; goto _LL500; _LL500: _temp499=( void*) _temp497.rgn;
goto _LL498; _LL498: Cyc_Tcutil_occurs( evar, env, _temp501); Cyc_Tcutil_occurs(
evar, env, _temp499);} for( 0; _temp450 !=  0; _temp450=(( struct Cyc_List_List*)
_check_null( _temp450))->tl){ struct _tuple8 _temp505; void* _temp506; void*
_temp508; struct _tuple8* _temp503=( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( _temp450))->hd; _temp505=* _temp503; _LL509: _temp508= _temp505.f1;
goto _LL507; _LL507: _temp506= _temp505.f2; goto _LL504; _LL504: Cyc_Tcutil_occurs(
evar, env, _temp508); Cyc_Tcutil_occurs( evar, env, _temp506);} goto _LL400;
_LL412: for( 0; _temp464 !=  0; _temp464=(( struct Cyc_List_List*) _check_null(
_temp464))->tl){ Cyc_Tcutil_occurs( evar, env,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp464))->hd)).f2);} goto _LL400; _LL414: Cyc_Tcutil_occurs( evar,
env, _temp466); goto _LL400; _LL416: Cyc_Tcutil_occurs( evar, env, _temp470);
Cyc_Tcutil_occurslist( evar, env, _temp472); goto _LL400; _LL418: _temp480=
_temp476; goto _LL420; _LL420: _temp482= _temp480; goto _LL422; _LL422: Cyc_Tcutil_occurslist(
evar, env, _temp482); goto _LL400; _LL424: _temp486= _temp484; goto _LL426;
_LL426: for( 0; _temp486 !=  0; _temp486=(( struct Cyc_List_List*) _check_null(
_temp486))->tl){ Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp486))->hd)->type);} goto _LL400; _LL428:
Cyc_Tcutil_occurs( evar, env, _temp488); goto _LL400; _LL430: Cyc_Tcutil_occurslist(
evar, env, _temp490); goto _LL400; _LL432: Cyc_Tcutil_occurs( evar, env,
_temp492); goto _LL400; _LL434: goto _LL400; _LL400:;} static void Cyc_Tcutil_occurslist(
void* evar, struct Cyc_List_List* env, struct Cyc_List_List* ts){ for( 0; ts != 
0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){ Cyc_Tcutil_occurs( evar,
env,( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);}} static void Cyc_Tcutil_unify_list(
struct Cyc_List_List* t1, struct Cyc_List_List* t2){ struct _handler_cons
_temp510; _push_handler(& _temp510);{ int _temp512= 0; if( setjmp( _temp510.handler)){
_temp512= 1;} if( ! _temp512){ Cyc_List_iter2( Cyc_Tcutil_unify_it, t1, t2);;
_pop_handler();} else{ void* _temp511=( void*) _exn_thrown; void* _temp514=
_temp511; _LL516: if( _temp514 ==  Cyc_List_List_mismatch){ goto _LL517;} else{
goto _LL518;} _LL518: goto _LL519; _LL517:( int) _throw(( void*) Cyc_Tcutil_Unify);
_LL519:( void) _throw( _temp514); _LL515:;}}} static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2){ if(( tq1.q_const != 
tq2.q_const? 1: tq1.q_volatile !=  tq2.q_volatile)? 1: tq1.q_restrict !=  tq2.q_restrict){(
int) _throw(( void*) Cyc_Tcutil_Unify);}} int Cyc_Tcutil_equal_tqual( struct Cyc_Absyn_Tqual
tq1, struct Cyc_Absyn_Tqual tq2){ return( tq1.q_const ==  tq2.q_const? tq1.q_volatile
==  tq2.q_volatile: 0)? tq1.q_restrict ==  tq2.q_restrict: 0;} static void Cyc_Tcutil_unify_it_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y){ x= Cyc_Absyn_compress_conref( x); y= Cyc_Absyn_compress_conref( y); if( x == 
y){ return;}{ void* _temp520=( void*) x->v; void* _temp528; _LL522: if( _temp520
== ( void*) Cyc_Absyn_No_constr){ goto _LL523;} else{ goto _LL524;} _LL524: if((
unsigned int) _temp520 >  1u?*(( int*) _temp520) ==  Cyc_Absyn_Forward_constr: 0){
goto _LL525;} else{ goto _LL526;} _LL526: if(( unsigned int) _temp520 >  1u?*((
int*) _temp520) ==  Cyc_Absyn_Eq_constr: 0){ _LL529: _temp528=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp520)->f1; goto _LL527;} else{ goto _LL521;}
_LL523:( void*)( x->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp530=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp530[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp531; _temp531.tag= Cyc_Absyn_Forward_constr; _temp531.f1= y; _temp531;});
_temp530;}))); return; _LL525:( int) _throw(({ void* _temp532[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("unify_conref: forward after compress", sizeof( unsigned char), 37u),
_tag_arr( _temp532, sizeof( void*), 0u));})); _LL527: { void* _temp533=( void*)
y->v; void* _temp541; _LL535: if( _temp533 == ( void*) Cyc_Absyn_No_constr){
goto _LL536;} else{ goto _LL537;} _LL537: if(( unsigned int) _temp533 >  1u?*((
int*) _temp533) ==  Cyc_Absyn_Forward_constr: 0){ goto _LL538;} else{ goto
_LL539;} _LL539: if(( unsigned int) _temp533 >  1u?*(( int*) _temp533) ==  Cyc_Absyn_Eq_constr:
0){ _LL542: _temp541=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp533)->f1;
goto _LL540;} else{ goto _LL534;} _LL536:( void*)( y->v=( void*)(( void*) x->v));
return; _LL538:( int) _throw(({ void* _temp543[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("unify_conref: forward after compress(2)", sizeof( unsigned char), 40u),
_tag_arr( _temp543, sizeof( void*), 0u));})); _LL540: if( cmp( _temp528,
_temp541) !=  0){( int) _throw(( void*) Cyc_Tcutil_Unify);} return; _LL534:;}
_LL521:;}} static int Cyc_Tcutil_unify_conrefs( int(* cmp)( void*, void*),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y){ struct _handler_cons
_temp544; _push_handler(& _temp544);{ int _temp546= 0; if( setjmp( _temp544.handler)){
_temp546= 1;} if( ! _temp546){ Cyc_Tcutil_unify_it_conrefs( cmp, x, y);{ int
_temp547= 1; _npop_handler( 0u); return _temp547;}; _pop_handler();} else{ void*
_temp545=( void*) _exn_thrown; void* _temp549= _temp545; _LL551: if( _temp549 == 
Cyc_Tcutil_Unify){ goto _LL552;} else{ goto _LL553;} _LL553: goto _LL554; _LL552:
return 0; _LL554:( void) _throw( _temp549); _LL550:;}}} static int Cyc_Tcutil_boundscmp(
void* b1, void* b2){ struct _tuple8 _temp556=({ struct _tuple8 _temp555;
_temp555.f1= b1; _temp555.f2= b2; _temp555;}); void* _temp566; void* _temp568;
void* _temp570; void* _temp572; void* _temp574; struct Cyc_Absyn_Exp* _temp576;
void* _temp578; struct Cyc_Absyn_Exp* _temp580; _LL558: _LL569: _temp568=
_temp556.f1; if( _temp568 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL567;} else{
goto _LL560;} _LL567: _temp566= _temp556.f2; if( _temp566 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL559;} else{ goto _LL560;} _LL560: _LL571: _temp570= _temp556.f1; if(
_temp570 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL561;} else{ goto _LL562;}
_LL562: _LL573: _temp572= _temp556.f2; if( _temp572 == ( void*) Cyc_Absyn_Unknown_b){
goto _LL563;} else{ goto _LL564;} _LL564: _LL579: _temp578= _temp556.f1; if((
unsigned int) _temp578 >  1u?*(( int*) _temp578) ==  Cyc_Absyn_Upper_b: 0){
_LL581: _temp580=(( struct Cyc_Absyn_Upper_b_struct*) _temp578)->f1; goto _LL575;}
else{ goto _LL557;} _LL575: _temp574= _temp556.f2; if(( unsigned int) _temp574 > 
1u?*(( int*) _temp574) ==  Cyc_Absyn_Upper_b: 0){ _LL577: _temp576=(( struct Cyc_Absyn_Upper_b_struct*)
_temp574)->f1; goto _LL565;} else{ goto _LL557;} _LL559: return 0; _LL561:
return - 1; _LL563: return 1; _LL565: { int i1=( int) Cyc_Evexp_eval_const_uint_exp(
_temp580); int i2=( int) Cyc_Evexp_eval_const_uint_exp( _temp576); if( i1 ==  i2){
return 0;} if( i1 <  i2){ return - 1;} return 1;} _LL557:;} static int Cyc_Tcutil_equal_att(
void* a1, void* a2){ if( a1 ==  a2){ return 1;}{ struct _tuple8 _temp583=({
struct _tuple8 _temp582; _temp582.f1= a1; _temp582.f2= a2; _temp582;}); void*
_temp595; int _temp597; int _temp599; void* _temp601; void* _temp603; int
_temp605; int _temp607; void* _temp609; void* _temp611; int _temp613; void*
_temp615; int _temp617; void* _temp619; int _temp621; void* _temp623; int
_temp625; void* _temp627; struct _tagged_arr _temp629; void* _temp631; struct
_tagged_arr _temp633; _LL585: _LL604: _temp603= _temp583.f1; if(( unsigned int)
_temp603 >  16u?*(( int*) _temp603) ==  Cyc_Absyn_Format_att: 0){ _LL610:
_temp609=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp603)->f1; goto
_LL608; _LL608: _temp607=(( struct Cyc_Absyn_Format_att_struct*) _temp603)->f2;
goto _LL606; _LL606: _temp605=(( struct Cyc_Absyn_Format_att_struct*) _temp603)->f3;
goto _LL596;} else{ goto _LL587;} _LL596: _temp595= _temp583.f2; if((
unsigned int) _temp595 >  16u?*(( int*) _temp595) ==  Cyc_Absyn_Format_att: 0){
_LL602: _temp601=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp595)->f1;
goto _LL600; _LL600: _temp599=(( struct Cyc_Absyn_Format_att_struct*) _temp595)->f2;
goto _LL598; _LL598: _temp597=(( struct Cyc_Absyn_Format_att_struct*) _temp595)->f3;
goto _LL586;} else{ goto _LL587;} _LL587: _LL616: _temp615= _temp583.f1; if((
unsigned int) _temp615 >  16u?*(( int*) _temp615) ==  Cyc_Absyn_Regparm_att: 0){
_LL618: _temp617=(( struct Cyc_Absyn_Regparm_att_struct*) _temp615)->f1; goto
_LL612;} else{ goto _LL589;} _LL612: _temp611= _temp583.f2; if(( unsigned int)
_temp611 >  16u?*(( int*) _temp611) ==  Cyc_Absyn_Regparm_att: 0){ _LL614:
_temp613=(( struct Cyc_Absyn_Regparm_att_struct*) _temp611)->f1; goto _LL588;}
else{ goto _LL589;} _LL589: _LL624: _temp623= _temp583.f1; if(( unsigned int)
_temp623 >  16u?*(( int*) _temp623) ==  Cyc_Absyn_Aligned_att: 0){ _LL626:
_temp625=(( struct Cyc_Absyn_Aligned_att_struct*) _temp623)->f1; goto _LL620;}
else{ goto _LL591;} _LL620: _temp619= _temp583.f2; if(( unsigned int) _temp619 > 
16u?*(( int*) _temp619) ==  Cyc_Absyn_Aligned_att: 0){ _LL622: _temp621=((
struct Cyc_Absyn_Aligned_att_struct*) _temp619)->f1; goto _LL590;} else{ goto
_LL591;} _LL591: _LL632: _temp631= _temp583.f1; if(( unsigned int) _temp631 > 
16u?*(( int*) _temp631) ==  Cyc_Absyn_Section_att: 0){ _LL634: _temp633=((
struct Cyc_Absyn_Section_att_struct*) _temp631)->f1; goto _LL628;} else{ goto
_LL593;} _LL628: _temp627= _temp583.f2; if(( unsigned int) _temp627 >  16u?*((
int*) _temp627) ==  Cyc_Absyn_Section_att: 0){ _LL630: _temp629=(( struct Cyc_Absyn_Section_att_struct*)
_temp627)->f1; goto _LL592;} else{ goto _LL593;} _LL593: goto _LL594; _LL586:
return( _temp609 ==  _temp601? _temp607 ==  _temp599: 0)? _temp605 ==  _temp597:
0; _LL588: _temp625= _temp617; _temp621= _temp613; goto _LL590; _LL590: return
_temp625 ==  _temp621; _LL592: return Cyc_std_strcmp( _temp633, _temp629) ==  0;
_LL594: return 0; _LL584:;}} int Cyc_Tcutil_same_atts( struct Cyc_List_List* a1,
struct Cyc_List_List* a2){{ struct Cyc_List_List* a= a1; for( 0; a !=  0; a=((
struct Cyc_List_List*) _check_null( a))->tl){ if( ! Cyc_List_exists_c( Cyc_Tcutil_equal_att,(
void*)(( struct Cyc_List_List*) _check_null( a))->hd, a2)){ return 0;}}}{ struct
Cyc_List_List* a= a2; for( 0; a !=  0; a=(( struct Cyc_List_List*) _check_null(
a))->tl){ if( ! Cyc_List_exists_c( Cyc_Tcutil_equal_att,( void*)(( struct Cyc_List_List*)
_check_null( a))->hd, a1)){ return 0;}}} return 1;} static void* Cyc_Tcutil_normalize_effect(
void* e); static void* Cyc_Tcutil_rgns_of( void* t); static void* Cyc_Tcutil_rgns_of_field(
struct Cyc_Absyn_Structfield* sf){ return Cyc_Tcutil_rgns_of(( void*) sf->type);}
static struct _tuple5* Cyc_Tcutil_region_free_subst( struct Cyc_Absyn_Tvar* tv){
void* t;{ void* _temp635= Cyc_Absyn_conref_val( tv->kind); _LL637: if( _temp635
== ( void*) Cyc_Absyn_RgnKind){ goto _LL638;} else{ goto _LL639;} _LL639: if(
_temp635 == ( void*) Cyc_Absyn_EffKind){ goto _LL640;} else{ goto _LL641;}
_LL641: goto _LL642; _LL638: t=( void*) Cyc_Absyn_HeapRgn; goto _LL636; _LL640:
t= Cyc_Absyn_empty_effect; goto _LL636; _LL642: t= Cyc_Absyn_sint_t; goto _LL636;
_LL636:;} return({ struct _tuple5* _temp643=( struct _tuple5*) GC_malloc(
sizeof( struct _tuple5)); _temp643->f1= tv; _temp643->f2= t; _temp643;});}
static void* Cyc_Tcutil_rgns_of( void* t){ void* _temp644= Cyc_Tcutil_compress(
t); void* _temp692; struct Cyc_Absyn_TunionInfo _temp694; void* _temp696; struct
Cyc_List_List* _temp698; struct Cyc_Absyn_PtrInfo _temp700; void* _temp702; void*
_temp704; void* _temp706; struct Cyc_List_List* _temp708; struct Cyc_Absyn_TunionFieldInfo
_temp710; struct Cyc_List_List* _temp712; struct Cyc_List_List* _temp714; struct
Cyc_List_List* _temp716; struct Cyc_List_List* _temp718; struct Cyc_List_List*
_temp720; struct Cyc_Absyn_FnInfo _temp722; struct Cyc_List_List* _temp724;
struct Cyc_Absyn_VarargInfo* _temp726; struct Cyc_List_List* _temp728; void*
_temp730; struct Cyc_Core_Opt* _temp732; struct Cyc_List_List* _temp734; void*
_temp736; _LL646: if( _temp644 == ( void*) Cyc_Absyn_VoidType){ goto _LL647;}
else{ goto _LL648;} _LL648: if( _temp644 == ( void*) Cyc_Absyn_FloatType){ goto
_LL649;} else{ goto _LL650;} _LL650: if( _temp644 == ( void*) Cyc_Absyn_DoubleType){
goto _LL651;} else{ goto _LL652;} _LL652: if(( unsigned int) _temp644 >  4u?*((
int*) _temp644) ==  Cyc_Absyn_EnumType: 0){ goto _LL653;} else{ goto _LL654;}
_LL654: if(( unsigned int) _temp644 >  4u?*(( int*) _temp644) ==  Cyc_Absyn_IntType:
0){ goto _LL655;} else{ goto _LL656;} _LL656: if(( unsigned int) _temp644 >  4u?*((
int*) _temp644) ==  Cyc_Absyn_Evar: 0){ goto _LL657;} else{ goto _LL658;} _LL658:
if(( unsigned int) _temp644 >  4u?*(( int*) _temp644) ==  Cyc_Absyn_VarType: 0){
goto _LL659;} else{ goto _LL660;} _LL660: if(( unsigned int) _temp644 >  4u?*((
int*) _temp644) ==  Cyc_Absyn_RgnHandleType: 0){ _LL693: _temp692=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp644)->f1; goto _LL661;} else{ goto
_LL662;} _LL662: if(( unsigned int) _temp644 >  4u?*(( int*) _temp644) ==  Cyc_Absyn_TunionType:
0){ _LL695: _temp694=(( struct Cyc_Absyn_TunionType_struct*) _temp644)->f1;
_LL699: _temp698= _temp694.targs; goto _LL697; _LL697: _temp696=( void*)
_temp694.rgn; goto _LL663;} else{ goto _LL664;} _LL664: if(( unsigned int)
_temp644 >  4u?*(( int*) _temp644) ==  Cyc_Absyn_PointerType: 0){ _LL701:
_temp700=(( struct Cyc_Absyn_PointerType_struct*) _temp644)->f1; _LL705:
_temp704=( void*) _temp700.elt_typ; goto _LL703; _LL703: _temp702=( void*)
_temp700.rgn_typ; goto _LL665;} else{ goto _LL666;} _LL666: if(( unsigned int)
_temp644 >  4u?*(( int*) _temp644) ==  Cyc_Absyn_ArrayType: 0){ _LL707: _temp706=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp644)->f1; goto _LL667;} else{
goto _LL668;} _LL668: if(( unsigned int) _temp644 >  4u?*(( int*) _temp644) == 
Cyc_Absyn_TupleType: 0){ _LL709: _temp708=(( struct Cyc_Absyn_TupleType_struct*)
_temp644)->f1; goto _LL669;} else{ goto _LL670;} _LL670: if(( unsigned int)
_temp644 >  4u?*(( int*) _temp644) ==  Cyc_Absyn_TunionFieldType: 0){ _LL711:
_temp710=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp644)->f1; _LL713:
_temp712= _temp710.targs; goto _LL671;} else{ goto _LL672;} _LL672: if((
unsigned int) _temp644 >  4u?*(( int*) _temp644) ==  Cyc_Absyn_StructType: 0){
_LL715: _temp714=(( struct Cyc_Absyn_StructType_struct*) _temp644)->f2; goto
_LL673;} else{ goto _LL674;} _LL674: if(( unsigned int) _temp644 >  4u?*(( int*)
_temp644) ==  Cyc_Absyn_UnionType: 0){ _LL717: _temp716=(( struct Cyc_Absyn_UnionType_struct*)
_temp644)->f2; goto _LL675;} else{ goto _LL676;} _LL676: if(( unsigned int)
_temp644 >  4u?*(( int*) _temp644) ==  Cyc_Absyn_AnonStructType: 0){ _LL719:
_temp718=(( struct Cyc_Absyn_AnonStructType_struct*) _temp644)->f1; goto _LL677;}
else{ goto _LL678;} _LL678: if(( unsigned int) _temp644 >  4u?*(( int*) _temp644)
==  Cyc_Absyn_AnonUnionType: 0){ _LL721: _temp720=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp644)->f1; goto _LL679;} else{ goto _LL680;} _LL680: if(( unsigned int)
_temp644 >  4u?*(( int*) _temp644) ==  Cyc_Absyn_FnType: 0){ _LL723: _temp722=((
struct Cyc_Absyn_FnType_struct*) _temp644)->f1; _LL735: _temp734= _temp722.tvars;
goto _LL733; _LL733: _temp732= _temp722.effect; goto _LL731; _LL731: _temp730=(
void*) _temp722.ret_typ; goto _LL729; _LL729: _temp728= _temp722.args; goto
_LL727; _LL727: _temp726= _temp722.cyc_varargs; goto _LL725; _LL725: _temp724=
_temp722.rgn_po; goto _LL681;} else{ goto _LL682;} _LL682: if( _temp644 == (
void*) Cyc_Absyn_HeapRgn){ goto _LL683;} else{ goto _LL684;} _LL684: if((
unsigned int) _temp644 >  4u?*(( int*) _temp644) ==  Cyc_Absyn_AccessEff: 0){
goto _LL685;} else{ goto _LL686;} _LL686: if(( unsigned int) _temp644 >  4u?*((
int*) _temp644) ==  Cyc_Absyn_JoinEff: 0){ goto _LL687;} else{ goto _LL688;}
_LL688: if(( unsigned int) _temp644 >  4u?*(( int*) _temp644) ==  Cyc_Absyn_RgnsEff:
0){ _LL737: _temp736=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp644)->f1;
goto _LL689;} else{ goto _LL690;} _LL690: if(( unsigned int) _temp644 >  4u?*((
int*) _temp644) ==  Cyc_Absyn_TypedefType: 0){ goto _LL691;} else{ goto _LL645;}
_LL647: goto _LL649; _LL649: goto _LL651; _LL651: goto _LL653; _LL653: goto
_LL655; _LL655: return Cyc_Absyn_empty_effect; _LL657: goto _LL659; _LL659: {
void* _temp738= Cyc_Tcutil_typ_kind( t); _LL740: if( _temp738 == ( void*) Cyc_Absyn_RgnKind){
goto _LL741;} else{ goto _LL742;} _LL742: if( _temp738 == ( void*) Cyc_Absyn_EffKind){
goto _LL743;} else{ goto _LL744;} _LL744: goto _LL745; _LL741: return( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp746=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp746[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp747; _temp747.tag= Cyc_Absyn_AccessEff; _temp747.f1=(
void*) t; _temp747;}); _temp746;}); _LL743: return t; _LL745: return( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp748=( struct Cyc_Absyn_RgnsEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp748[ 0]=({ struct Cyc_Absyn_RgnsEff_struct
_temp749; _temp749.tag= Cyc_Absyn_RgnsEff; _temp749.f1=( void*) t; _temp749;});
_temp748;}); _LL739:;} _LL661: return( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp750=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp750[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp751; _temp751.tag= Cyc_Absyn_AccessEff;
_temp751.f1=( void*) _temp692; _temp751;}); _temp750;}); _LL663: { struct Cyc_List_List*
ts=({ struct Cyc_List_List* _temp754=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp754->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp755=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp755[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp756; _temp756.tag= Cyc_Absyn_AccessEff;
_temp756.f1=( void*) _temp696; _temp756;}); _temp755;})); _temp754->tl= Cyc_List_map(
Cyc_Tcutil_rgns_of, _temp698); _temp754;}); return Cyc_Tcutil_normalize_effect((
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp752=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp752[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp753; _temp753.tag= Cyc_Absyn_JoinEff; _temp753.f1= ts; _temp753;});
_temp752;}));} _LL665: return Cyc_Tcutil_normalize_effect(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp757=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp757[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp758; _temp758.tag= Cyc_Absyn_JoinEff;
_temp758.f1=({ void* _temp759[ 2u]; _temp759[ 1u]= Cyc_Tcutil_rgns_of( _temp704);
_temp759[ 0u]=( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp760=( struct
Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp760[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp761; _temp761.tag= Cyc_Absyn_AccessEff;
_temp761.f1=( void*) _temp702; _temp761;}); _temp760;}); Cyc_List_list( _tag_arr(
_temp759, sizeof( void*), 2u));}); _temp758;}); _temp757;})); _LL667: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of( _temp706)); _LL669: { struct Cyc_List_List* _temp762= 0;
for( 0; _temp708 !=  0; _temp708=(( struct Cyc_List_List*) _check_null( _temp708))->tl){
_temp762=({ struct Cyc_List_List* _temp763=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp763->hd=( void*)(*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp708))->hd)).f2; _temp763->tl= _temp762;
_temp763;});} _temp712= _temp762; goto _LL671;} _LL671: _temp714= _temp712; goto
_LL673; _LL673: _temp716= _temp714; goto _LL675; _LL675: return Cyc_Tcutil_normalize_effect((
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp764=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp764[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp765; _temp765.tag= Cyc_Absyn_JoinEff; _temp765.f1= Cyc_List_map( Cyc_Tcutil_rgns_of,
_temp716); _temp765;}); _temp764;})); _LL677: _temp720= _temp718; goto _LL679;
_LL679: return Cyc_Tcutil_normalize_effect(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp766=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp766[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp767; _temp767.tag= Cyc_Absyn_JoinEff;
_temp767.f1=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_rgns_of_field, _temp720);
_temp767;}); _temp766;})); _LL681: { void* _temp768= Cyc_Tcutil_substitute(((
struct Cyc_List_List*(*)( struct _tuple5*(* f)( struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_region_free_subst, _temp734),( void*)((
struct Cyc_Core_Opt*) _check_null( _temp732))->v); return Cyc_Tcutil_normalize_effect(
_temp768);} _LL683: return Cyc_Absyn_empty_effect; _LL685: return t; _LL687:
return t; _LL689: return Cyc_Tcutil_rgns_of( _temp736); _LL691: return({ void*
_temp769[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("typedef in compressed type",
sizeof( unsigned char), 27u), _tag_arr( _temp769, sizeof( void*), 0u));});
_LL645:;} static void* Cyc_Tcutil_normalize_effect( void* e){ e= Cyc_Tcutil_compress(
e);{ void* _temp770= e; struct Cyc_List_List* _temp778; struct Cyc_List_List**
_temp780; void* _temp781; _LL772: if(( unsigned int) _temp770 >  4u?*(( int*)
_temp770) ==  Cyc_Absyn_JoinEff: 0){ _LL779: _temp778=(( struct Cyc_Absyn_JoinEff_struct*)
_temp770)->f1; _temp780=( struct Cyc_List_List**)&(( struct Cyc_Absyn_JoinEff_struct*)
_temp770)->f1; goto _LL773;} else{ goto _LL774;} _LL774: if(( unsigned int)
_temp770 >  4u?*(( int*) _temp770) ==  Cyc_Absyn_RgnsEff: 0){ _LL782: _temp781=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp770)->f1; goto _LL775;} else{
goto _LL776;} _LL776: goto _LL777; _LL773: { int nested_join= 1;{ struct Cyc_List_List*
effs=* _temp780; for( 0; effs !=  0; effs=(( struct Cyc_List_List*) _check_null(
effs))->tl){ void* _temp783=( void*)(( struct Cyc_List_List*) _check_null( effs))->hd;(
void*)((( struct Cyc_List_List*) _check_null( effs))->hd=( void*) Cyc_Tcutil_compress(
Cyc_Tcutil_normalize_effect( _temp783)));{ void* _temp784=( void*)(( struct Cyc_List_List*)
_check_null( effs))->hd; _LL786: if(( unsigned int) _temp784 >  4u?*(( int*)
_temp784) ==  Cyc_Absyn_JoinEff: 0){ goto _LL787;} else{ goto _LL788;} _LL788:
goto _LL789; _LL787: nested_join= 1; goto _LL785; _LL789: goto _LL785; _LL785:;}}}
if( ! nested_join){ return e;}{ struct Cyc_List_List* effects= 0;{ struct Cyc_List_List*
effs=* _temp780; for( 0; effs !=  0; effs=(( struct Cyc_List_List*) _check_null(
effs))->tl){ void* _temp790= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*)
_check_null( effs))->hd); struct Cyc_List_List* _temp798; void* _temp800; _LL792:
if(( unsigned int) _temp790 >  4u?*(( int*) _temp790) ==  Cyc_Absyn_JoinEff: 0){
_LL799: _temp798=(( struct Cyc_Absyn_JoinEff_struct*) _temp790)->f1; goto _LL793;}
else{ goto _LL794;} _LL794: if(( unsigned int) _temp790 >  4u?*(( int*) _temp790)
==  Cyc_Absyn_AccessEff: 0){ _LL801: _temp800=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp790)->f1; if( _temp800 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL795;} else{
goto _LL796;}} else{ goto _LL796;} _LL796: goto _LL797; _LL793: effects= Cyc_List_revappend(
_temp798, effects); goto _LL791; _LL795: goto _LL791; _LL797: effects=({ struct
Cyc_List_List* _temp802=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp802->hd=( void*) _temp790; _temp802->tl= effects; _temp802;}); goto _LL791;
_LL791:;}}* _temp780= Cyc_List_imp_rev( effects); return e;}} _LL775: return Cyc_Tcutil_rgns_of(
_temp781); _LL777: return e; _LL771:;}} struct _tuple9{ void* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; static struct _tuple9* Cyc_Tcutil_get_effect_evar(
void* t){ void* _temp803= Cyc_Tcutil_compress( t); struct Cyc_List_List*
_temp811; struct Cyc_List_List _temp813; struct Cyc_List_List* _temp814; void*
_temp816; struct Cyc_Core_Opt* _temp818; struct Cyc_Core_Opt* _temp820; _LL805:
if(( unsigned int) _temp803 >  4u?*(( int*) _temp803) ==  Cyc_Absyn_JoinEff: 0){
_LL812: _temp811=(( struct Cyc_Absyn_JoinEff_struct*) _temp803)->f1; if(
_temp811 ==  0){ goto _LL807;} else{ _temp813=* _temp811; _LL817: _temp816=(
void*) _temp813.hd; goto _LL815; _LL815: _temp814= _temp813.tl; goto _LL806;}}
else{ goto _LL807;} _LL807: if(( unsigned int) _temp803 >  4u?*(( int*) _temp803)
==  Cyc_Absyn_Evar: 0){ _LL821: _temp820=(( struct Cyc_Absyn_Evar_struct*)
_temp803)->f1; goto _LL819; _LL819: _temp818=(( struct Cyc_Absyn_Evar_struct*)
_temp803)->f4; goto _LL808;} else{ goto _LL809;} _LL809: goto _LL810; _LL806: {
void* _temp822= Cyc_Tcutil_compress( _temp816); struct Cyc_Core_Opt* _temp828;
_LL824: if(( unsigned int) _temp822 >  4u?*(( int*) _temp822) ==  Cyc_Absyn_Evar:
0){ _LL829: _temp828=(( struct Cyc_Absyn_Evar_struct*) _temp822)->f4; goto
_LL825;} else{ goto _LL826;} _LL826: goto _LL827; _LL825: return({ struct
_tuple9* _temp830=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp830->f1= _temp816; _temp830->f2= _temp814; _temp830->f3=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp828))->v; _temp830;}); _LL827: return 0;
_LL823:;} _LL808: if( _temp820 ==  0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp820))->v != ( void*) Cyc_Absyn_EffKind){({ void* _temp831[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("effect evar has wrong kind", sizeof( unsigned char), 27u), _tag_arr(
_temp831, sizeof( void*), 0u));});} return({ struct _tuple9* _temp832=( struct
_tuple9*) GC_malloc( sizeof( struct _tuple9)); _temp832->f1= t; _temp832->f2= 0;
_temp832->f3=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp818))->v; _temp832;}); _LL810: return 0; _LL804:;} static struct Cyc_Core_Opt
Cyc_Tcutil_ek={( void*)(( void*) 4u)}; static void* Cyc_Tcutil_dummy_fntype(
void* eff){ struct Cyc_Absyn_FnType_struct* _temp833=({ struct Cyc_Absyn_FnType_struct*
_temp834=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp834[ 0]=({ struct Cyc_Absyn_FnType_struct _temp835; _temp835.tag= Cyc_Absyn_FnType;
_temp835.f1=({ struct Cyc_Absyn_FnInfo _temp836; _temp836.tvars= 0; _temp836.effect=({
struct Cyc_Core_Opt* _temp837=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp837->v=( void*) eff; _temp837;}); _temp836.ret_typ=( void*)((
void*) Cyc_Absyn_VoidType); _temp836.args= 0; _temp836.c_varargs= 0; _temp836.cyc_varargs=
0; _temp836.rgn_po= 0; _temp836.attributes= 0; _temp836;}); _temp835;});
_temp834;}); return Cyc_Absyn_atb_typ(( void*) _temp833,( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual(), Cyc_Absyn_bounds_one);} int Cyc_Tcutil_region_in_effect(
int constrain, void* r, void* e){ r= Cyc_Tcutil_compress( r); if( r == ( void*)
Cyc_Absyn_HeapRgn){ return 1;}{ void* _temp838= Cyc_Tcutil_compress( e); void*
_temp850; struct Cyc_List_List* _temp852; void* _temp854; struct Cyc_Core_Opt*
_temp856; struct Cyc_Core_Opt* _temp858; struct Cyc_Core_Opt** _temp860; struct
Cyc_Core_Opt* _temp861; _LL840: if(( unsigned int) _temp838 >  4u?*(( int*)
_temp838) ==  Cyc_Absyn_AccessEff: 0){ _LL851: _temp850=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp838)->f1; goto _LL841;} else{ goto _LL842;} _LL842: if(( unsigned int)
_temp838 >  4u?*(( int*) _temp838) ==  Cyc_Absyn_JoinEff: 0){ _LL853: _temp852=((
struct Cyc_Absyn_JoinEff_struct*) _temp838)->f1; goto _LL843;} else{ goto _LL844;}
_LL844: if(( unsigned int) _temp838 >  4u?*(( int*) _temp838) ==  Cyc_Absyn_RgnsEff:
0){ _LL855: _temp854=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp838)->f1;
goto _LL845;} else{ goto _LL846;} _LL846: if(( unsigned int) _temp838 >  4u?*((
int*) _temp838) ==  Cyc_Absyn_Evar: 0){ _LL862: _temp861=(( struct Cyc_Absyn_Evar_struct*)
_temp838)->f1; goto _LL859; _LL859: _temp858=(( struct Cyc_Absyn_Evar_struct*)
_temp838)->f2; _temp860=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp838)->f2; goto _LL857; _LL857: _temp856=(( struct Cyc_Absyn_Evar_struct*)
_temp838)->f4; goto _LL847;} else{ goto _LL848;} _LL848: goto _LL849; _LL841:
if( constrain){ return Cyc_Tcutil_unify( r, _temp850);} _temp850= Cyc_Tcutil_compress(
_temp850); if( r ==  _temp850){ return 1;}{ struct _tuple8 _temp864=({ struct
_tuple8 _temp863; _temp863.f1= r; _temp863.f2= _temp850; _temp863;}); void*
_temp870; struct Cyc_Absyn_Tvar* _temp872; void* _temp874; struct Cyc_Absyn_Tvar*
_temp876; _LL866: _LL875: _temp874= _temp864.f1; if(( unsigned int) _temp874 > 
4u?*(( int*) _temp874) ==  Cyc_Absyn_VarType: 0){ _LL877: _temp876=(( struct Cyc_Absyn_VarType_struct*)
_temp874)->f1; goto _LL871;} else{ goto _LL868;} _LL871: _temp870= _temp864.f2;
if(( unsigned int) _temp870 >  4u?*(( int*) _temp870) ==  Cyc_Absyn_VarType: 0){
_LL873: _temp872=(( struct Cyc_Absyn_VarType_struct*) _temp870)->f1; goto _LL867;}
else{ goto _LL868;} _LL868: goto _LL869; _LL867: return Cyc_Absyn_tvar_cmp(
_temp876, _temp872) ==  0; _LL869: return 0; _LL865:;} _LL843: for( 0; _temp852
!=  0; _temp852=(( struct Cyc_List_List*) _check_null( _temp852))->tl){ if( Cyc_Tcutil_region_in_effect(
constrain, r,( void*)(( struct Cyc_List_List*) _check_null( _temp852))->hd)){
return 1;}} return 0; _LL845: { void* _temp878= Cyc_Tcutil_rgns_of( _temp854);
void* _temp884; _LL880: if(( unsigned int) _temp878 >  4u?*(( int*) _temp878) == 
Cyc_Absyn_RgnsEff: 0){ _LL885: _temp884=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp878)->f1; goto _LL881;} else{ goto _LL882;} _LL882: goto _LL883; _LL881:
if( ! constrain){ return 0;}{ void* _temp886= Cyc_Tcutil_compress( _temp884);
struct Cyc_Core_Opt* _temp892; struct Cyc_Core_Opt* _temp894; struct Cyc_Core_Opt**
_temp896; struct Cyc_Core_Opt* _temp897; _LL888: if(( unsigned int) _temp886 > 
4u?*(( int*) _temp886) ==  Cyc_Absyn_Evar: 0){ _LL898: _temp897=(( struct Cyc_Absyn_Evar_struct*)
_temp886)->f1; goto _LL895; _LL895: _temp894=(( struct Cyc_Absyn_Evar_struct*)
_temp886)->f2; _temp896=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp886)->f2; goto _LL893; _LL893: _temp892=(( struct Cyc_Absyn_Evar_struct*)
_temp886)->f4; goto _LL889;} else{ goto _LL890;} _LL890: goto _LL891; _LL889: {
void* _temp899= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_temp892); Cyc_Tcutil_occurs( _temp899,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp892))->v, r);{ void* _temp900= Cyc_Tcutil_dummy_fntype(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp902=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp902[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp903; _temp903.tag= Cyc_Absyn_JoinEff; _temp903.f1=({ void* _temp904[ 2u];
_temp904[ 1u]=( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp905=( struct
Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp905[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp906; _temp906.tag= Cyc_Absyn_AccessEff;
_temp906.f1=( void*) r; _temp906;}); _temp905;}); _temp904[ 0u]= _temp899; Cyc_List_list(
_tag_arr( _temp904, sizeof( void*), 2u));}); _temp903;}); _temp902;}));*
_temp896=({ struct Cyc_Core_Opt* _temp901=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp901->v=( void*) _temp900; _temp901;});
return 1;}} _LL891: return 0; _LL887:;} _LL883: return Cyc_Tcutil_region_in_effect(
constrain, r, _temp878); _LL879:;} _LL847: if( _temp861 ==  0? 1:( void*)((
struct Cyc_Core_Opt*) _check_null( _temp861))->v != ( void*) Cyc_Absyn_EffKind){({
void* _temp907[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind", sizeof( unsigned char),
27u), _tag_arr( _temp907, sizeof( void*), 0u));});} if( ! constrain){ return 0;}{
void* _temp908= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_temp856); Cyc_Tcutil_occurs( _temp908,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp856))->v, r);{ struct Cyc_Absyn_JoinEff_struct* _temp909=({
struct Cyc_Absyn_JoinEff_struct* _temp911=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp911[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp912; _temp912.tag= Cyc_Absyn_JoinEff; _temp912.f1=({ struct Cyc_List_List*
_temp913=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp913->hd=( void*) _temp908; _temp913->tl=({ struct Cyc_List_List* _temp914=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp914->hd=(
void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp915=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp915[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp916; _temp916.tag= Cyc_Absyn_AccessEff; _temp916.f1=(
void*) r; _temp916;}); _temp915;})); _temp914->tl= 0; _temp914;}); _temp913;});
_temp912;}); _temp911;});* _temp860=({ struct Cyc_Core_Opt* _temp910=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp910->v=( void*)((
void*) _temp909); _temp910;}); return 1;}} _LL849: return 0; _LL839:;}} static
int Cyc_Tcutil_type_in_effect( int constrain, void* t, void* e){ t= Cyc_Tcutil_compress(
t);{ void* _temp917= Cyc_Tcutil_compress( e); struct Cyc_List_List* _temp929;
void* _temp931; struct Cyc_Core_Opt* _temp933; struct Cyc_Core_Opt* _temp935;
struct Cyc_Core_Opt** _temp937; struct Cyc_Core_Opt* _temp938; _LL919: if((
unsigned int) _temp917 >  4u?*(( int*) _temp917) ==  Cyc_Absyn_AccessEff: 0){
goto _LL920;} else{ goto _LL921;} _LL921: if(( unsigned int) _temp917 >  4u?*((
int*) _temp917) ==  Cyc_Absyn_JoinEff: 0){ _LL930: _temp929=(( struct Cyc_Absyn_JoinEff_struct*)
_temp917)->f1; goto _LL922;} else{ goto _LL923;} _LL923: if(( unsigned int)
_temp917 >  4u?*(( int*) _temp917) ==  Cyc_Absyn_RgnsEff: 0){ _LL932: _temp931=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp917)->f1; goto _LL924;} else{
goto _LL925;} _LL925: if(( unsigned int) _temp917 >  4u?*(( int*) _temp917) == 
Cyc_Absyn_Evar: 0){ _LL939: _temp938=(( struct Cyc_Absyn_Evar_struct*) _temp917)->f1;
goto _LL936; _LL936: _temp935=(( struct Cyc_Absyn_Evar_struct*) _temp917)->f2;
_temp937=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp917)->f2;
goto _LL934; _LL934: _temp933=(( struct Cyc_Absyn_Evar_struct*) _temp917)->f4;
goto _LL926;} else{ goto _LL927;} _LL927: goto _LL928; _LL920: return 0; _LL922:
for( 0; _temp929 !=  0; _temp929=(( struct Cyc_List_List*) _check_null( _temp929))->tl){
if( Cyc_Tcutil_type_in_effect( constrain, t,( void*)(( struct Cyc_List_List*)
_check_null( _temp929))->hd)){ return 1;}} return 0; _LL924: _temp931= Cyc_Tcutil_compress(
_temp931); if( t ==  _temp931){ return 1;} if( constrain){ return Cyc_Tcutil_unify(
t, _temp931);}{ void* _temp940= Cyc_Tcutil_rgns_of( t); void* _temp946; _LL942:
if(( unsigned int) _temp940 >  4u?*(( int*) _temp940) ==  Cyc_Absyn_RgnsEff: 0){
_LL947: _temp946=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp940)->f1;
goto _LL943;} else{ goto _LL944;} _LL944: goto _LL945; _LL943: { struct _tuple8
_temp949=({ struct _tuple8 _temp948; _temp948.f1= t; _temp948.f2= Cyc_Tcutil_compress(
_temp946); _temp948;}); void* _temp955; struct Cyc_Absyn_Tvar* _temp957; void*
_temp959; struct Cyc_Absyn_Tvar* _temp961; _LL951: _LL960: _temp959= _temp949.f1;
if(( unsigned int) _temp959 >  4u?*(( int*) _temp959) ==  Cyc_Absyn_VarType: 0){
_LL962: _temp961=(( struct Cyc_Absyn_VarType_struct*) _temp959)->f1; goto _LL956;}
else{ goto _LL953;} _LL956: _temp955= _temp949.f2; if(( unsigned int) _temp955 > 
4u?*(( int*) _temp955) ==  Cyc_Absyn_VarType: 0){ _LL958: _temp957=(( struct Cyc_Absyn_VarType_struct*)
_temp955)->f1; goto _LL952;} else{ goto _LL953;} _LL953: goto _LL954; _LL952:
return Cyc_Tcutil_unify( t, _temp946); _LL954: return t ==  _temp946; _LL950:;}
_LL945: return Cyc_Tcutil_type_in_effect( constrain, t, _temp940); _LL941:;}
_LL926: if( _temp938 ==  0? 1:( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp938))->v != ( void*) Cyc_Absyn_EffKind){({ void* _temp963[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u), _tag_arr( _temp963, sizeof( void*), 0u));});} if(
! constrain){ return 0;}{ void* _temp964= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek, _temp933); Cyc_Tcutil_occurs( _temp964,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp933))->v, t);{ struct Cyc_Absyn_JoinEff_struct*
_temp965=({ struct Cyc_Absyn_JoinEff_struct* _temp967=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp967[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp968; _temp968.tag= Cyc_Absyn_JoinEff; _temp968.f1=({ struct Cyc_List_List*
_temp969=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp969->hd=( void*) _temp964; _temp969->tl=({ struct Cyc_List_List* _temp970=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp970->hd=(
void*)(( void*)({ struct Cyc_Absyn_RgnsEff_struct* _temp971=( struct Cyc_Absyn_RgnsEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp971[ 0]=({ struct Cyc_Absyn_RgnsEff_struct
_temp972; _temp972.tag= Cyc_Absyn_RgnsEff; _temp972.f1=( void*) t; _temp972;});
_temp971;})); _temp970->tl= 0; _temp970;}); _temp969;}); _temp968;}); _temp967;});*
_temp937=({ struct Cyc_Core_Opt* _temp966=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp966->v=( void*)(( void*) _temp965); _temp966;});
return 1;}} _LL928: return 0; _LL918:;}} static int Cyc_Tcutil_variable_in_effect(
int constrain, struct Cyc_Absyn_Tvar* v, void* e){ e= Cyc_Tcutil_compress( e);{
void* _temp973= e; struct Cyc_Absyn_Tvar* _temp985; struct Cyc_List_List*
_temp987; void* _temp989; struct Cyc_Core_Opt* _temp991; struct Cyc_Core_Opt*
_temp993; struct Cyc_Core_Opt** _temp995; struct Cyc_Core_Opt* _temp996; _LL975:
if(( unsigned int) _temp973 >  4u?*(( int*) _temp973) ==  Cyc_Absyn_VarType: 0){
_LL986: _temp985=(( struct Cyc_Absyn_VarType_struct*) _temp973)->f1; goto _LL976;}
else{ goto _LL977;} _LL977: if(( unsigned int) _temp973 >  4u?*(( int*) _temp973)
==  Cyc_Absyn_JoinEff: 0){ _LL988: _temp987=(( struct Cyc_Absyn_JoinEff_struct*)
_temp973)->f1; goto _LL978;} else{ goto _LL979;} _LL979: if(( unsigned int)
_temp973 >  4u?*(( int*) _temp973) ==  Cyc_Absyn_RgnsEff: 0){ _LL990: _temp989=(
void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp973)->f1; goto _LL980;} else{
goto _LL981;} _LL981: if(( unsigned int) _temp973 >  4u?*(( int*) _temp973) == 
Cyc_Absyn_Evar: 0){ _LL997: _temp996=(( struct Cyc_Absyn_Evar_struct*) _temp973)->f1;
goto _LL994; _LL994: _temp993=(( struct Cyc_Absyn_Evar_struct*) _temp973)->f2;
_temp995=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp973)->f2;
goto _LL992; _LL992: _temp991=(( struct Cyc_Absyn_Evar_struct*) _temp973)->f4;
goto _LL982;} else{ goto _LL983;} _LL983: goto _LL984; _LL976: return Cyc_Absyn_tvar_cmp(
v, _temp985) ==  0; _LL978: for( 0; _temp987 !=  0; _temp987=(( struct Cyc_List_List*)
_check_null( _temp987))->tl){ if( Cyc_Tcutil_variable_in_effect( constrain, v,(
void*)(( struct Cyc_List_List*) _check_null( _temp987))->hd)){ return 1;}}
return 0; _LL980: { void* _temp998= Cyc_Tcutil_rgns_of( _temp989); void*
_temp1004; _LL1000: if(( unsigned int) _temp998 >  4u?*(( int*) _temp998) == 
Cyc_Absyn_RgnsEff: 0){ _LL1005: _temp1004=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp998)->f1; goto _LL1001;} else{ goto _LL1002;} _LL1002: goto _LL1003;
_LL1001: if( ! constrain){ return 0;}{ void* _temp1006= Cyc_Tcutil_compress(
_temp1004); struct Cyc_Core_Opt* _temp1012; struct Cyc_Core_Opt* _temp1014;
struct Cyc_Core_Opt** _temp1016; struct Cyc_Core_Opt* _temp1017; _LL1008: if((
unsigned int) _temp1006 >  4u?*(( int*) _temp1006) ==  Cyc_Absyn_Evar: 0){
_LL1018: _temp1017=(( struct Cyc_Absyn_Evar_struct*) _temp1006)->f1; goto
_LL1015; _LL1015: _temp1014=(( struct Cyc_Absyn_Evar_struct*) _temp1006)->f2;
_temp1016=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp1006)->f2;
goto _LL1013; _LL1013: _temp1012=(( struct Cyc_Absyn_Evar_struct*) _temp1006)->f4;
goto _LL1009;} else{ goto _LL1010;} _LL1010: goto _LL1011; _LL1009: { void*
_temp1019= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek, _temp1012);
if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1012))->v, v)){
return 0;}{ void* _temp1020= Cyc_Tcutil_dummy_fntype(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp1022=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp1022[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp1023; _temp1023.tag= Cyc_Absyn_JoinEff;
_temp1023.f1=({ void* _temp1024[ 2u]; _temp1024[ 1u]=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1025=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1025[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1026; _temp1026.tag= Cyc_Absyn_VarType;
_temp1026.f1= v; _temp1026;}); _temp1025;}); _temp1024[ 0u]= _temp1019; Cyc_List_list(
_tag_arr( _temp1024, sizeof( void*), 2u));}); _temp1023;}); _temp1022;}));*
_temp1016=({ struct Cyc_Core_Opt* _temp1021=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1021->v=( void*) _temp1020; _temp1021;});
return 1;}} _LL1011: return 0; _LL1007:;} _LL1003: return Cyc_Tcutil_variable_in_effect(
constrain, v, _temp998); _LL999:;} _LL982: if( _temp996 ==  0? 1:( void*)((
struct Cyc_Core_Opt*) _check_null( _temp996))->v != ( void*) Cyc_Absyn_EffKind){({
void* _temp1027[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind", sizeof( unsigned char),
27u), _tag_arr( _temp1027, sizeof( void*), 0u));});}{ void* _temp1028= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek, _temp991); if( !(( int(*)( int(* compare)(
struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct
Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp991))->v, v)){ return 0;}{ struct Cyc_Absyn_JoinEff_struct*
_temp1029=({ struct Cyc_Absyn_JoinEff_struct* _temp1031=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp1031[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp1032; _temp1032.tag= Cyc_Absyn_JoinEff; _temp1032.f1=({
struct Cyc_List_List* _temp1033=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1033->hd=( void*) _temp1028; _temp1033->tl=({
struct Cyc_List_List* _temp1034=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1034->hd=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1035=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1035[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1036; _temp1036.tag= Cyc_Absyn_VarType;
_temp1036.f1= v; _temp1036;}); _temp1035;})); _temp1034->tl= 0; _temp1034;});
_temp1033;}); _temp1032;}); _temp1031;});* _temp995=({ struct Cyc_Core_Opt*
_temp1030=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1030->v=( void*)(( void*) _temp1029); _temp1030;}); return 1;}} _LL984:
return 0; _LL974:;}} static int Cyc_Tcutil_evar_in_effect( void* evar, void* e){
e= Cyc_Tcutil_compress( e);{ void* _temp1037= e; struct Cyc_List_List* _temp1047;
void* _temp1049; _LL1039: if(( unsigned int) _temp1037 >  4u?*(( int*) _temp1037)
==  Cyc_Absyn_JoinEff: 0){ _LL1048: _temp1047=(( struct Cyc_Absyn_JoinEff_struct*)
_temp1037)->f1; goto _LL1040;} else{ goto _LL1041;} _LL1041: if(( unsigned int)
_temp1037 >  4u?*(( int*) _temp1037) ==  Cyc_Absyn_RgnsEff: 0){ _LL1050:
_temp1049=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp1037)->f1; goto
_LL1042;} else{ goto _LL1043;} _LL1043: if(( unsigned int) _temp1037 >  4u?*((
int*) _temp1037) ==  Cyc_Absyn_Evar: 0){ goto _LL1044;} else{ goto _LL1045;}
_LL1045: goto _LL1046; _LL1040: for( 0; _temp1047 !=  0; _temp1047=(( struct Cyc_List_List*)
_check_null( _temp1047))->tl){ if( Cyc_Tcutil_evar_in_effect( evar,( void*)((
struct Cyc_List_List*) _check_null( _temp1047))->hd)){ return 1;}} return 0;
_LL1042: { void* _temp1051= Cyc_Tcutil_rgns_of( _temp1049); void* _temp1057;
_LL1053: if(( unsigned int) _temp1051 >  4u?*(( int*) _temp1051) ==  Cyc_Absyn_RgnsEff:
0){ _LL1058: _temp1057=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp1051)->f1;
goto _LL1054;} else{ goto _LL1055;} _LL1055: goto _LL1056; _LL1054: return 0;
_LL1056: return Cyc_Tcutil_evar_in_effect( evar, _temp1051); _LL1052:;} _LL1044:
return evar ==  e; _LL1046: return 0; _LL1038:;}} int Cyc_Tcutil_subset_effect(
int set_to_empty, void* e1, void* e2){ void* _temp1059= Cyc_Tcutil_compress( e1);
struct Cyc_List_List* _temp1073; void* _temp1075; struct Cyc_Absyn_Tvar*
_temp1077; void* _temp1079; struct Cyc_Core_Opt* _temp1081; struct Cyc_Core_Opt*
_temp1083; struct Cyc_Core_Opt** _temp1085; _LL1061: if(( unsigned int)
_temp1059 >  4u?*(( int*) _temp1059) ==  Cyc_Absyn_JoinEff: 0){ _LL1074:
_temp1073=(( struct Cyc_Absyn_JoinEff_struct*) _temp1059)->f1; goto _LL1062;}
else{ goto _LL1063;} _LL1063: if(( unsigned int) _temp1059 >  4u?*(( int*)
_temp1059) ==  Cyc_Absyn_AccessEff: 0){ _LL1076: _temp1075=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp1059)->f1; goto _LL1064;} else{ goto _LL1065;} _LL1065: if(( unsigned int)
_temp1059 >  4u?*(( int*) _temp1059) ==  Cyc_Absyn_VarType: 0){ _LL1078:
_temp1077=(( struct Cyc_Absyn_VarType_struct*) _temp1059)->f1; goto _LL1066;}
else{ goto _LL1067;} _LL1067: if(( unsigned int) _temp1059 >  4u?*(( int*)
_temp1059) ==  Cyc_Absyn_RgnsEff: 0){ _LL1080: _temp1079=( void*)(( struct Cyc_Absyn_RgnsEff_struct*)
_temp1059)->f1; goto _LL1068;} else{ goto _LL1069;} _LL1069: if(( unsigned int)
_temp1059 >  4u?*(( int*) _temp1059) ==  Cyc_Absyn_Evar: 0){ _LL1084: _temp1083=((
struct Cyc_Absyn_Evar_struct*) _temp1059)->f2; _temp1085=( struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*) _temp1059)->f2; goto _LL1082; _LL1082: _temp1081=((
struct Cyc_Absyn_Evar_struct*) _temp1059)->f4; goto _LL1070;} else{ goto _LL1071;}
_LL1071: goto _LL1072; _LL1062: for( 0; _temp1073 !=  0; _temp1073=(( struct Cyc_List_List*)
_check_null( _temp1073))->tl){ if( ! Cyc_Tcutil_subset_effect( set_to_empty,(
void*)(( struct Cyc_List_List*) _check_null( _temp1073))->hd, e2)){ return 0;}}
return 1; _LL1064: return( Cyc_Tcutil_region_in_effect( 0, _temp1075, e2)? 1:
Cyc_Tcutil_region_in_effect( 1, _temp1075, e2))? 1: Cyc_Tcutil_unify( _temp1075,(
void*) Cyc_Absyn_HeapRgn); _LL1066: return Cyc_Tcutil_variable_in_effect( 0,
_temp1077, e2)? 1: Cyc_Tcutil_variable_in_effect( 1, _temp1077, e2); _LL1068: {
void* _temp1086= Cyc_Tcutil_rgns_of( _temp1079); void* _temp1092; _LL1088: if((
unsigned int) _temp1086 >  4u?*(( int*) _temp1086) ==  Cyc_Absyn_RgnsEff: 0){
_LL1093: _temp1092=( void*)(( struct Cyc_Absyn_RgnsEff_struct*) _temp1086)->f1;
goto _LL1089;} else{ goto _LL1090;} _LL1090: goto _LL1091; _LL1089: return( Cyc_Tcutil_type_in_effect(
0, _temp1092, e2)? 1: Cyc_Tcutil_type_in_effect( 1, _temp1092, e2))? 1: Cyc_Tcutil_unify(
_temp1092, Cyc_Absyn_sint_t); _LL1091: return Cyc_Tcutil_subset_effect(
set_to_empty, _temp1086, e2); _LL1087:;} _LL1070: if( ! Cyc_Tcutil_evar_in_effect(
e1, e2)){ if( set_to_empty){* _temp1085=({ struct Cyc_Core_Opt* _temp1094=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1094->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp1095=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp1095[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp1096; _temp1096.tag= Cyc_Absyn_JoinEff; _temp1096.f1=
0; _temp1096;}); _temp1095;})); _temp1094;});} else{ Cyc_Tcutil_occurs( e1,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1081))->v, e2);*
_temp1085=({ struct Cyc_Core_Opt* _temp1097=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1097->v=( void*) e2; _temp1097;});}} return
1; _LL1072: return({ struct Cyc_std_String_pa_struct _temp1099; _temp1099.tag=
Cyc_std_String_pa; _temp1099.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( e1);{
void* _temp1098[ 1u]={& _temp1099};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("subset_effect: bad effect: %s",
sizeof( unsigned char), 30u), _tag_arr( _temp1098, sizeof( void*), 1u));}});
_LL1060:;} struct _tuple10{ struct _tuple9* f1; struct _tuple9* f2; } ; static
int Cyc_Tcutil_unify_effect( void* e1, void* e2){ e1= Cyc_Tcutil_normalize_effect(
e1); e2= Cyc_Tcutil_normalize_effect( e2);{ struct _tuple10 _temp1101=({ struct
_tuple10 _temp1100; _temp1100.f1= Cyc_Tcutil_get_effect_evar( e1); _temp1100.f2=
Cyc_Tcutil_get_effect_evar( e2); _temp1100;}); _LL1103: goto _LL1104; _LL1104:
return Cyc_Tcutil_subset_effect( 0, e1, e2)? Cyc_Tcutil_subset_effect( 0, e2, e1):
0; _LL1102:;}} static int Cyc_Tcutil_sub_rgnpo( struct Cyc_List_List* rpo1,
struct Cyc_List_List* rpo2){{ struct Cyc_List_List* r1= rpo1; for( 0; r1 !=  0;
r1=(( struct Cyc_List_List*) _check_null( r1))->tl){ struct _tuple8 _temp1107;
void* _temp1108; void* _temp1110; struct _tuple8* _temp1105=( struct _tuple8*)((
struct Cyc_List_List*) _check_null( r1))->hd; _temp1107=* _temp1105; _LL1111:
_temp1110= _temp1107.f1; goto _LL1109; _LL1109: _temp1108= _temp1107.f2; goto
_LL1106; _LL1106: { int found= _temp1110 == ( void*) Cyc_Absyn_HeapRgn;{ struct
Cyc_List_List* r2= rpo2; for( 0; r2 !=  0? ! found: 0; r2=(( struct Cyc_List_List*)
_check_null( r2))->tl){ struct _tuple8 _temp1114; void* _temp1115; void*
_temp1117; struct _tuple8* _temp1112=( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( r2))->hd; _temp1114=* _temp1112; _LL1118: _temp1117= _temp1114.f1;
goto _LL1116; _LL1116: _temp1115= _temp1114.f2; goto _LL1113; _LL1113: if( Cyc_Tcutil_unify(
_temp1110, _temp1117)? Cyc_Tcutil_unify( _temp1108, _temp1115): 0){ found= 1;
break;}}} if( ! found){ return 0;}}}} return 1;} static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List* rpo1, struct Cyc_List_List* rpo2){ return Cyc_Tcutil_sub_rgnpo(
rpo1, rpo2)? Cyc_Tcutil_sub_rgnpo( rpo2, rpo1): 0;} struct _tuple11{ struct Cyc_Absyn_VarargInfo*
f1; struct Cyc_Absyn_VarargInfo* f2; } ; struct _tuple12{ struct Cyc_Core_Opt*
f1; struct Cyc_Core_Opt* f2; } ; void Cyc_Tcutil_unify_it( void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2); if( t1 ==  t2){
return;}{ void* _temp1119= t1; struct Cyc_Core_Opt* _temp1125; struct Cyc_Core_Opt*
_temp1127; struct Cyc_Core_Opt** _temp1129; struct Cyc_Core_Opt* _temp1130;
_LL1121: if(( unsigned int) _temp1119 >  4u?*(( int*) _temp1119) ==  Cyc_Absyn_Evar:
0){ _LL1131: _temp1130=(( struct Cyc_Absyn_Evar_struct*) _temp1119)->f1; goto
_LL1128; _LL1128: _temp1127=(( struct Cyc_Absyn_Evar_struct*) _temp1119)->f2;
_temp1129=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp1119)->f2;
goto _LL1126; _LL1126: _temp1125=(( struct Cyc_Absyn_Evar_struct*) _temp1119)->f4;
goto _LL1122;} else{ goto _LL1123;} _LL1123: goto _LL1124; _LL1122: Cyc_Tcutil_occurs(
t1,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1125))->v,
t2);{ void* _temp1132= Cyc_Tcutil_typ_kind( t2); if( Cyc_Tcutil_kind_leq(
_temp1132,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1130))->v)){*
_temp1129=({ struct Cyc_Core_Opt* _temp1133=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1133->v=( void*) t2; _temp1133;}); return;}
else{{ void* _temp1134= t2; struct Cyc_Core_Opt* _temp1142; struct Cyc_Core_Opt*
_temp1144; struct Cyc_Core_Opt** _temp1146; struct Cyc_Absyn_PtrInfo _temp1148;
_LL1136: if(( unsigned int) _temp1134 >  4u?*(( int*) _temp1134) ==  Cyc_Absyn_Evar:
0){ _LL1145: _temp1144=(( struct Cyc_Absyn_Evar_struct*) _temp1134)->f2;
_temp1146=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp1134)->f2;
goto _LL1143; _LL1143: _temp1142=(( struct Cyc_Absyn_Evar_struct*) _temp1134)->f4;
goto _LL1137;} else{ goto _LL1138;} _LL1138: if(( unsigned int) _temp1134 >  4u?*((
int*) _temp1134) ==  Cyc_Absyn_PointerType: 0){ _LL1149: _temp1148=(( struct Cyc_Absyn_PointerType_struct*)
_temp1134)->f1; goto _LL1147;} else{ goto _LL1140;} _LL1147: if(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1130))->v == ( void*) Cyc_Absyn_BoxKind){
goto _LL1139;} else{ goto _LL1140;} _LL1140: goto _LL1141; _LL1137: { struct Cyc_List_List*
_temp1150=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp1125))->v;{ struct Cyc_List_List* s2=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1142))->v; for( 0; s2 !=  0; s2=(( struct Cyc_List_List*)
_check_null( s2))->tl){ if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, _temp1150,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( s2))->hd)){( int) _throw(( void*) Cyc_Tcutil_Unify);}}} if( Cyc_Tcutil_kind_leq((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1130))->v, _temp1132)){*
_temp1146=({ struct Cyc_Core_Opt* _temp1151=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1151->v=( void*) t1; _temp1151;}); return;}
goto _LL1135;} _LL1139: { struct Cyc_Absyn_Conref* _temp1152= Cyc_Absyn_compress_conref(
_temp1148.bounds);{ void* _temp1153=( void*) _temp1152->v; _LL1155: if(
_temp1153 == ( void*) Cyc_Absyn_No_constr){ goto _LL1156;} else{ goto _LL1157;}
_LL1157: goto _LL1158; _LL1156:( void*)( _temp1152->v=( void*)(( void*)({ struct
Cyc_Absyn_Eq_constr_struct* _temp1159=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp1159[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp1160; _temp1160.tag= Cyc_Absyn_Eq_constr;
_temp1160.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1161=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1161[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1162; _temp1162.tag= Cyc_Absyn_Upper_b;
_temp1162.f1= Cyc_Absyn_signed_int_exp( 1, 0); _temp1162;}); _temp1161;}));
_temp1160;}); _temp1159;})));* _temp1129=({ struct Cyc_Core_Opt* _temp1163=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1163->v=(
void*) t2; _temp1163;}); return; _LL1158: goto _LL1154; _LL1154:;} goto _LL1135;}
_LL1141: goto _LL1135; _LL1135:;}( int) _throw(( void*) Cyc_Tcutil_Unify);}}
_LL1124: goto _LL1120; _LL1120:;}{ struct _tuple8 _temp1165=({ struct _tuple8
_temp1164; _temp1164.f1= t2; _temp1164.f2= t1; _temp1164;}); void* _temp1219;
void* _temp1221; void* _temp1223; void* _temp1225; struct Cyc_Absyn_Tvar*
_temp1227; void* _temp1229; struct Cyc_Absyn_Tvar* _temp1231; void* _temp1233;
struct Cyc_Absyn_Structdecl** _temp1235; struct Cyc_List_List* _temp1237; struct
_tuple1* _temp1239; void* _temp1241; struct Cyc_Absyn_Structdecl** _temp1243;
struct Cyc_List_List* _temp1245; struct _tuple1* _temp1247; void* _temp1249;
struct _tuple1* _temp1251; void* _temp1253; struct _tuple1* _temp1255; void*
_temp1257; struct Cyc_Absyn_Uniondecl** _temp1259; struct Cyc_List_List*
_temp1261; struct _tuple1* _temp1263; void* _temp1265; struct Cyc_Absyn_Uniondecl**
_temp1267; struct Cyc_List_List* _temp1269; struct _tuple1* _temp1271; void*
_temp1273; struct Cyc_Absyn_TunionInfo _temp1275; void* _temp1277; struct Cyc_List_List*
_temp1279; void* _temp1281; struct Cyc_Absyn_Tuniondecl* _temp1283; void*
_temp1285; struct Cyc_Absyn_TunionInfo _temp1287; void* _temp1289; struct Cyc_List_List*
_temp1291; void* _temp1293; struct Cyc_Absyn_Tuniondecl* _temp1295; void*
_temp1297; struct Cyc_Absyn_TunionFieldInfo _temp1299; struct Cyc_List_List*
_temp1301; void* _temp1303; struct Cyc_Absyn_Tunionfield* _temp1305; struct Cyc_Absyn_Tuniondecl*
_temp1307; void* _temp1309; struct Cyc_Absyn_TunionFieldInfo _temp1311; struct
Cyc_List_List* _temp1313; void* _temp1315; struct Cyc_Absyn_Tunionfield*
_temp1317; struct Cyc_Absyn_Tuniondecl* _temp1319; void* _temp1321; struct Cyc_Absyn_PtrInfo
_temp1323; struct Cyc_Absyn_Conref* _temp1325; struct Cyc_Absyn_Tqual _temp1327;
struct Cyc_Absyn_Conref* _temp1329; void* _temp1331; void* _temp1333; void*
_temp1335; struct Cyc_Absyn_PtrInfo _temp1337; struct Cyc_Absyn_Conref*
_temp1339; struct Cyc_Absyn_Tqual _temp1341; struct Cyc_Absyn_Conref* _temp1343;
void* _temp1345; void* _temp1347; void* _temp1349; void* _temp1351; void*
_temp1353; void* _temp1355; void* _temp1357; void* _temp1359; void* _temp1361;
void* _temp1363; void* _temp1365; void* _temp1367; void* _temp1369; struct Cyc_Absyn_Exp*
_temp1371; struct Cyc_Absyn_Tqual _temp1373; void* _temp1375; void* _temp1377;
struct Cyc_Absyn_Exp* _temp1379; struct Cyc_Absyn_Tqual _temp1381; void*
_temp1383; void* _temp1385; struct Cyc_Absyn_FnInfo _temp1387; struct Cyc_List_List*
_temp1389; struct Cyc_List_List* _temp1391; struct Cyc_Absyn_VarargInfo*
_temp1393; int _temp1395; struct Cyc_List_List* _temp1397; void* _temp1399;
struct Cyc_Core_Opt* _temp1401; struct Cyc_List_List* _temp1403; void* _temp1405;
struct Cyc_Absyn_FnInfo _temp1407; struct Cyc_List_List* _temp1409; struct Cyc_List_List*
_temp1411; struct Cyc_Absyn_VarargInfo* _temp1413; int _temp1415; struct Cyc_List_List*
_temp1417; void* _temp1419; struct Cyc_Core_Opt* _temp1421; struct Cyc_List_List*
_temp1423; void* _temp1425; struct Cyc_List_List* _temp1427; void* _temp1429;
struct Cyc_List_List* _temp1431; void* _temp1433; struct Cyc_List_List*
_temp1435; void* _temp1437; struct Cyc_List_List* _temp1439; void* _temp1441;
struct Cyc_List_List* _temp1443; void* _temp1445; struct Cyc_List_List*
_temp1447; void* _temp1449; void* _temp1451; void* _temp1453; void* _temp1455;
void* _temp1457; void* _temp1459; void* _temp1461; void* _temp1463; void*
_temp1465; void* _temp1467; void* _temp1469; void* _temp1471; _LL1167: _LL1220:
_temp1219= _temp1165.f1; if(( unsigned int) _temp1219 >  4u?*(( int*) _temp1219)
==  Cyc_Absyn_Evar: 0){ goto _LL1168;} else{ goto _LL1169;} _LL1169: _LL1224:
_temp1223= _temp1165.f1; if( _temp1223 == ( void*) Cyc_Absyn_VoidType){ goto
_LL1222;} else{ goto _LL1171;} _LL1222: _temp1221= _temp1165.f2; if( _temp1221
== ( void*) Cyc_Absyn_VoidType){ goto _LL1170;} else{ goto _LL1171;} _LL1171:
_LL1230: _temp1229= _temp1165.f1; if(( unsigned int) _temp1229 >  4u?*(( int*)
_temp1229) ==  Cyc_Absyn_VarType: 0){ _LL1232: _temp1231=(( struct Cyc_Absyn_VarType_struct*)
_temp1229)->f1; goto _LL1226;} else{ goto _LL1173;} _LL1226: _temp1225=
_temp1165.f2; if(( unsigned int) _temp1225 >  4u?*(( int*) _temp1225) ==  Cyc_Absyn_VarType:
0){ _LL1228: _temp1227=(( struct Cyc_Absyn_VarType_struct*) _temp1225)->f1; goto
_LL1172;} else{ goto _LL1173;} _LL1173: _LL1242: _temp1241= _temp1165.f1; if((
unsigned int) _temp1241 >  4u?*(( int*) _temp1241) ==  Cyc_Absyn_StructType: 0){
_LL1248: _temp1247=(( struct Cyc_Absyn_StructType_struct*) _temp1241)->f1; goto
_LL1246; _LL1246: _temp1245=(( struct Cyc_Absyn_StructType_struct*) _temp1241)->f2;
goto _LL1244; _LL1244: _temp1243=(( struct Cyc_Absyn_StructType_struct*)
_temp1241)->f3; goto _LL1234;} else{ goto _LL1175;} _LL1234: _temp1233=
_temp1165.f2; if(( unsigned int) _temp1233 >  4u?*(( int*) _temp1233) ==  Cyc_Absyn_StructType:
0){ _LL1240: _temp1239=(( struct Cyc_Absyn_StructType_struct*) _temp1233)->f1;
goto _LL1238; _LL1238: _temp1237=(( struct Cyc_Absyn_StructType_struct*)
_temp1233)->f2; goto _LL1236; _LL1236: _temp1235=(( struct Cyc_Absyn_StructType_struct*)
_temp1233)->f3; goto _LL1174;} else{ goto _LL1175;} _LL1175: _LL1254: _temp1253=
_temp1165.f1; if(( unsigned int) _temp1253 >  4u?*(( int*) _temp1253) ==  Cyc_Absyn_EnumType:
0){ _LL1256: _temp1255=(( struct Cyc_Absyn_EnumType_struct*) _temp1253)->f1;
goto _LL1250;} else{ goto _LL1177;} _LL1250: _temp1249= _temp1165.f2; if((
unsigned int) _temp1249 >  4u?*(( int*) _temp1249) ==  Cyc_Absyn_EnumType: 0){
_LL1252: _temp1251=(( struct Cyc_Absyn_EnumType_struct*) _temp1249)->f1; goto
_LL1176;} else{ goto _LL1177;} _LL1177: _LL1266: _temp1265= _temp1165.f1; if((
unsigned int) _temp1265 >  4u?*(( int*) _temp1265) ==  Cyc_Absyn_UnionType: 0){
_LL1272: _temp1271=(( struct Cyc_Absyn_UnionType_struct*) _temp1265)->f1; goto
_LL1270; _LL1270: _temp1269=(( struct Cyc_Absyn_UnionType_struct*) _temp1265)->f2;
goto _LL1268; _LL1268: _temp1267=(( struct Cyc_Absyn_UnionType_struct*)
_temp1265)->f3; goto _LL1258;} else{ goto _LL1179;} _LL1258: _temp1257=
_temp1165.f2; if(( unsigned int) _temp1257 >  4u?*(( int*) _temp1257) ==  Cyc_Absyn_UnionType:
0){ _LL1264: _temp1263=(( struct Cyc_Absyn_UnionType_struct*) _temp1257)->f1;
goto _LL1262; _LL1262: _temp1261=(( struct Cyc_Absyn_UnionType_struct*)
_temp1257)->f2; goto _LL1260; _LL1260: _temp1259=(( struct Cyc_Absyn_UnionType_struct*)
_temp1257)->f3; goto _LL1178;} else{ goto _LL1179;} _LL1179: _LL1286: _temp1285=
_temp1165.f1; if(( unsigned int) _temp1285 >  4u?*(( int*) _temp1285) ==  Cyc_Absyn_TunionType:
0){ _LL1288: _temp1287=(( struct Cyc_Absyn_TunionType_struct*) _temp1285)->f1;
_LL1294: _temp1293=( void*) _temp1287.tunion_info; if(*(( int*) _temp1293) == 
Cyc_Absyn_KnownTunion){ _LL1296: _temp1295=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1293)->f1; goto _LL1292;} else{ goto _LL1181;} _LL1292: _temp1291=
_temp1287.targs; goto _LL1290; _LL1290: _temp1289=( void*) _temp1287.rgn; goto
_LL1274;} else{ goto _LL1181;} _LL1274: _temp1273= _temp1165.f2; if((
unsigned int) _temp1273 >  4u?*(( int*) _temp1273) ==  Cyc_Absyn_TunionType: 0){
_LL1276: _temp1275=(( struct Cyc_Absyn_TunionType_struct*) _temp1273)->f1;
_LL1282: _temp1281=( void*) _temp1275.tunion_info; if(*(( int*) _temp1281) == 
Cyc_Absyn_KnownTunion){ _LL1284: _temp1283=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1281)->f1; goto _LL1280;} else{ goto _LL1181;} _LL1280: _temp1279=
_temp1275.targs; goto _LL1278; _LL1278: _temp1277=( void*) _temp1275.rgn; goto
_LL1180;} else{ goto _LL1181;} _LL1181: _LL1310: _temp1309= _temp1165.f1; if((
unsigned int) _temp1309 >  4u?*(( int*) _temp1309) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1312: _temp1311=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1309)->f1;
_LL1316: _temp1315=( void*) _temp1311.field_info; if(*(( int*) _temp1315) == 
Cyc_Absyn_KnownTunionfield){ _LL1320: _temp1319=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1315)->f1; goto _LL1318; _LL1318: _temp1317=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1315)->f2; goto _LL1314;} else{ goto _LL1183;} _LL1314: _temp1313=
_temp1311.targs; goto _LL1298;} else{ goto _LL1183;} _LL1298: _temp1297=
_temp1165.f2; if(( unsigned int) _temp1297 >  4u?*(( int*) _temp1297) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1300: _temp1299=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1297)->f1;
_LL1304: _temp1303=( void*) _temp1299.field_info; if(*(( int*) _temp1303) == 
Cyc_Absyn_KnownTunionfield){ _LL1308: _temp1307=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1303)->f1; goto _LL1306; _LL1306: _temp1305=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1303)->f2; goto _LL1302;} else{ goto _LL1183;} _LL1302: _temp1301=
_temp1299.targs; goto _LL1182;} else{ goto _LL1183;} _LL1183: _LL1336: _temp1335=
_temp1165.f1; if(( unsigned int) _temp1335 >  4u?*(( int*) _temp1335) ==  Cyc_Absyn_PointerType:
0){ _LL1338: _temp1337=(( struct Cyc_Absyn_PointerType_struct*) _temp1335)->f1;
_LL1348: _temp1347=( void*) _temp1337.elt_typ; goto _LL1346; _LL1346: _temp1345=(
void*) _temp1337.rgn_typ; goto _LL1344; _LL1344: _temp1343= _temp1337.nullable;
goto _LL1342; _LL1342: _temp1341= _temp1337.tq; goto _LL1340; _LL1340: _temp1339=
_temp1337.bounds; goto _LL1322;} else{ goto _LL1185;} _LL1322: _temp1321=
_temp1165.f2; if(( unsigned int) _temp1321 >  4u?*(( int*) _temp1321) ==  Cyc_Absyn_PointerType:
0){ _LL1324: _temp1323=(( struct Cyc_Absyn_PointerType_struct*) _temp1321)->f1;
_LL1334: _temp1333=( void*) _temp1323.elt_typ; goto _LL1332; _LL1332: _temp1331=(
void*) _temp1323.rgn_typ; goto _LL1330; _LL1330: _temp1329= _temp1323.nullable;
goto _LL1328; _LL1328: _temp1327= _temp1323.tq; goto _LL1326; _LL1326: _temp1325=
_temp1323.bounds; goto _LL1184;} else{ goto _LL1185;} _LL1185: _LL1356:
_temp1355= _temp1165.f1; if(( unsigned int) _temp1355 >  4u?*(( int*) _temp1355)
==  Cyc_Absyn_IntType: 0){ _LL1360: _temp1359=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1355)->f1; goto _LL1358; _LL1358: _temp1357=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1355)->f2; goto _LL1350;} else{ goto _LL1187;} _LL1350: _temp1349=
_temp1165.f2; if(( unsigned int) _temp1349 >  4u?*(( int*) _temp1349) ==  Cyc_Absyn_IntType:
0){ _LL1354: _temp1353=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1349)->f1;
goto _LL1352; _LL1352: _temp1351=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1349)->f2; goto _LL1186;} else{ goto _LL1187;} _LL1187: _LL1364: _temp1363=
_temp1165.f1; if( _temp1363 == ( void*) Cyc_Absyn_FloatType){ goto _LL1362;}
else{ goto _LL1189;} _LL1362: _temp1361= _temp1165.f2; if( _temp1361 == ( void*)
Cyc_Absyn_FloatType){ goto _LL1188;} else{ goto _LL1189;} _LL1189: _LL1368:
_temp1367= _temp1165.f1; if( _temp1367 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL1366;} else{ goto _LL1191;} _LL1366: _temp1365= _temp1165.f2; if( _temp1365
== ( void*) Cyc_Absyn_DoubleType){ goto _LL1190;} else{ goto _LL1191;} _LL1191:
_LL1378: _temp1377= _temp1165.f1; if(( unsigned int) _temp1377 >  4u?*(( int*)
_temp1377) ==  Cyc_Absyn_ArrayType: 0){ _LL1384: _temp1383=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1377)->f1; goto _LL1382; _LL1382: _temp1381=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1377)->f2; goto _LL1380; _LL1380: _temp1379=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1377)->f3; goto _LL1370;} else{ goto _LL1193;} _LL1370: _temp1369=
_temp1165.f2; if(( unsigned int) _temp1369 >  4u?*(( int*) _temp1369) ==  Cyc_Absyn_ArrayType:
0){ _LL1376: _temp1375=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1369)->f1;
goto _LL1374; _LL1374: _temp1373=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1369)->f2; goto _LL1372; _LL1372: _temp1371=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1369)->f3; goto _LL1192;} else{ goto _LL1193;} _LL1193: _LL1406: _temp1405=
_temp1165.f1; if(( unsigned int) _temp1405 >  4u?*(( int*) _temp1405) ==  Cyc_Absyn_FnType:
0){ _LL1408: _temp1407=(( struct Cyc_Absyn_FnType_struct*) _temp1405)->f1;
_LL1424: _temp1423= _temp1407.tvars; goto _LL1422; _LL1422: _temp1421= _temp1407.effect;
goto _LL1420; _LL1420: _temp1419=( void*) _temp1407.ret_typ; goto _LL1418;
_LL1418: _temp1417= _temp1407.args; goto _LL1416; _LL1416: _temp1415= _temp1407.c_varargs;
goto _LL1414; _LL1414: _temp1413= _temp1407.cyc_varargs; goto _LL1412; _LL1412:
_temp1411= _temp1407.rgn_po; goto _LL1410; _LL1410: _temp1409= _temp1407.attributes;
goto _LL1386;} else{ goto _LL1195;} _LL1386: _temp1385= _temp1165.f2; if((
unsigned int) _temp1385 >  4u?*(( int*) _temp1385) ==  Cyc_Absyn_FnType: 0){
_LL1388: _temp1387=(( struct Cyc_Absyn_FnType_struct*) _temp1385)->f1; _LL1404:
_temp1403= _temp1387.tvars; goto _LL1402; _LL1402: _temp1401= _temp1387.effect;
goto _LL1400; _LL1400: _temp1399=( void*) _temp1387.ret_typ; goto _LL1398;
_LL1398: _temp1397= _temp1387.args; goto _LL1396; _LL1396: _temp1395= _temp1387.c_varargs;
goto _LL1394; _LL1394: _temp1393= _temp1387.cyc_varargs; goto _LL1392; _LL1392:
_temp1391= _temp1387.rgn_po; goto _LL1390; _LL1390: _temp1389= _temp1387.attributes;
goto _LL1194;} else{ goto _LL1195;} _LL1195: _LL1430: _temp1429= _temp1165.f1;
if(( unsigned int) _temp1429 >  4u?*(( int*) _temp1429) ==  Cyc_Absyn_TupleType:
0){ _LL1432: _temp1431=(( struct Cyc_Absyn_TupleType_struct*) _temp1429)->f1;
goto _LL1426;} else{ goto _LL1197;} _LL1426: _temp1425= _temp1165.f2; if((
unsigned int) _temp1425 >  4u?*(( int*) _temp1425) ==  Cyc_Absyn_TupleType: 0){
_LL1428: _temp1427=(( struct Cyc_Absyn_TupleType_struct*) _temp1425)->f1; goto
_LL1196;} else{ goto _LL1197;} _LL1197: _LL1438: _temp1437= _temp1165.f1; if((
unsigned int) _temp1437 >  4u?*(( int*) _temp1437) ==  Cyc_Absyn_AnonStructType:
0){ _LL1440: _temp1439=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1437)->f1;
goto _LL1434;} else{ goto _LL1199;} _LL1434: _temp1433= _temp1165.f2; if((
unsigned int) _temp1433 >  4u?*(( int*) _temp1433) ==  Cyc_Absyn_AnonStructType:
0){ _LL1436: _temp1435=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1433)->f1;
goto _LL1198;} else{ goto _LL1199;} _LL1199: _LL1446: _temp1445= _temp1165.f1;
if(( unsigned int) _temp1445 >  4u?*(( int*) _temp1445) ==  Cyc_Absyn_AnonUnionType:
0){ _LL1448: _temp1447=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1445)->f1;
goto _LL1442;} else{ goto _LL1201;} _LL1442: _temp1441= _temp1165.f2; if((
unsigned int) _temp1441 >  4u?*(( int*) _temp1441) ==  Cyc_Absyn_AnonUnionType:
0){ _LL1444: _temp1443=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1441)->f1;
goto _LL1200;} else{ goto _LL1201;} _LL1201: _LL1452: _temp1451= _temp1165.f1;
if( _temp1451 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL1450;} else{ goto _LL1203;}
_LL1450: _temp1449= _temp1165.f2; if( _temp1449 == ( void*) Cyc_Absyn_HeapRgn){
goto _LL1202;} else{ goto _LL1203;} _LL1203: _LL1458: _temp1457= _temp1165.f1;
if(( unsigned int) _temp1457 >  4u?*(( int*) _temp1457) ==  Cyc_Absyn_RgnHandleType:
0){ _LL1460: _temp1459=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1457)->f1; goto _LL1454;} else{ goto _LL1205;} _LL1454: _temp1453=
_temp1165.f2; if(( unsigned int) _temp1453 >  4u?*(( int*) _temp1453) ==  Cyc_Absyn_RgnHandleType:
0){ _LL1456: _temp1455=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1453)->f1; goto _LL1204;} else{ goto _LL1205;} _LL1205: _LL1462: _temp1461=
_temp1165.f1; if(( unsigned int) _temp1461 >  4u?*(( int*) _temp1461) ==  Cyc_Absyn_JoinEff:
0){ goto _LL1206;} else{ goto _LL1207;} _LL1207: _LL1464: _temp1463= _temp1165.f2;
if(( unsigned int) _temp1463 >  4u?*(( int*) _temp1463) ==  Cyc_Absyn_JoinEff: 0){
goto _LL1208;} else{ goto _LL1209;} _LL1209: _LL1466: _temp1465= _temp1165.f1;
if(( unsigned int) _temp1465 >  4u?*(( int*) _temp1465) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1210;} else{ goto _LL1211;} _LL1211: _LL1468: _temp1467= _temp1165.f1;
if(( unsigned int) _temp1467 >  4u?*(( int*) _temp1467) ==  Cyc_Absyn_RgnsEff: 0){
goto _LL1212;} else{ goto _LL1213;} _LL1213: _LL1470: _temp1469= _temp1165.f2;
if(( unsigned int) _temp1469 >  4u?*(( int*) _temp1469) ==  Cyc_Absyn_RgnsEff: 0){
goto _LL1214;} else{ goto _LL1215;} _LL1215: _LL1472: _temp1471= _temp1165.f2;
if(( unsigned int) _temp1471 >  4u?*(( int*) _temp1471) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1216;} else{ goto _LL1217;} _LL1217: goto _LL1218; _LL1168: Cyc_Tcutil_unify_it(
t2, t1); return; _LL1170: return; _LL1172: { struct _tagged_arr* _temp1473=
_temp1231->name; struct _tagged_arr* _temp1474= _temp1227->name; int _temp1475=*((
int*) _check_null( _temp1231->identity)); int _temp1476=*(( int*) _check_null(
_temp1227->identity)); void* _temp1477= Cyc_Absyn_conref_val( _temp1231->kind);
void* _temp1478= Cyc_Absyn_conref_val( _temp1227->kind); if( _temp1476 == 
_temp1475? Cyc_std_zstrptrcmp( _temp1473, _temp1474) ==  0: 0){ if( _temp1477 != 
_temp1478){({ struct Cyc_std_String_pa_struct _temp1482; _temp1482.tag= Cyc_std_String_pa;
_temp1482.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( _temp1478);{ struct
Cyc_std_String_pa_struct _temp1481; _temp1481.tag= Cyc_std_String_pa; _temp1481.f1=(
struct _tagged_arr) Cyc_Absynpp_kind2string( _temp1477);{ struct Cyc_std_String_pa_struct
_temp1480; _temp1480.tag= Cyc_std_String_pa; _temp1480.f1=( struct _tagged_arr)*
_temp1473;{ void* _temp1479[ 3u]={& _temp1480,& _temp1481,& _temp1482};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("same type variable %s has kinds %s and %s",
sizeof( unsigned char), 42u), _tag_arr( _temp1479, sizeof( void*), 3u));}}}});}
return;} goto _LL1166;} _LL1174: if((( _temp1239 !=  0? _temp1247 !=  0: 0)? Cyc_Absyn_qvar_cmp((
struct _tuple1*) _check_null( _temp1239),( struct _tuple1*) _check_null(
_temp1247)) ==  0: 0)? 1:( _temp1239 ==  0? _temp1247 ==  0: 0)){ Cyc_Tcutil_unify_list(
_temp1237, _temp1245); return;} goto _LL1166; _LL1176: if( Cyc_Absyn_qvar_cmp(
_temp1255, _temp1251) ==  0){ return;} goto _LL1166; _LL1178: if((( _temp1263 != 
0? _temp1271 !=  0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*) _check_null(
_temp1263),( struct _tuple1*) _check_null( _temp1271)) ==  0: 0)? 1:( _temp1263
==  0? _temp1271 ==  0: 0)){ Cyc_Tcutil_unify_list( _temp1261, _temp1269);
return;} goto _LL1166; _LL1180: if( _temp1295 ==  _temp1283? 1: Cyc_Absyn_qvar_cmp(
_temp1295->name, _temp1283->name) ==  0){ Cyc_Tcutil_unify_it( _temp1277,
_temp1289); Cyc_Tcutil_unify_list( _temp1279, _temp1291); return;} goto _LL1166;
_LL1182: if(( _temp1319 ==  _temp1307? 1: Cyc_Absyn_qvar_cmp( _temp1319->name,
_temp1307->name) ==  0)? _temp1317 ==  _temp1305? 1: Cyc_Absyn_qvar_cmp(
_temp1317->name, _temp1305->name) ==  0: 0){ Cyc_Tcutil_unify_list( _temp1301,
_temp1313); return;} goto _LL1166; _LL1184: Cyc_Tcutil_unify_it( _temp1333,
_temp1347); Cyc_Tcutil_unify_it( _temp1345, _temp1331); Cyc_Tcutil_unify_tqual(
_temp1327, _temp1341); Cyc_Tcutil_unify_it_conrefs( Cyc_Tcutil_boundscmp,
_temp1325, _temp1339);{ void* _temp1483=( void*)( Cyc_Absyn_compress_conref(
_temp1325))->v; void* _temp1489; _LL1485: if(( unsigned int) _temp1483 >  1u?*((
int*) _temp1483) ==  Cyc_Absyn_Eq_constr: 0){ _LL1490: _temp1489=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1483)->f1; if( _temp1489 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL1486;} else{ goto _LL1487;}} else{ goto _LL1487;}
_LL1487: goto _LL1488; _LL1486: return; _LL1488: goto _LL1484; _LL1484:;}(( void(*)(
int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y))
Cyc_Tcutil_unify_it_conrefs)( Cyc_Core_intcmp, _temp1329, _temp1343); return;
_LL1186: if( _temp1353 ==  _temp1359? _temp1351 ==  _temp1357: 0){ return;} goto
_LL1166; _LL1188: return; _LL1190: return; _LL1192: Cyc_Tcutil_unify_it(
_temp1375, _temp1383); Cyc_Tcutil_unify_tqual( _temp1373, _temp1381); if(
_temp1379 ==  _temp1371){ return;} if( _temp1379 ==  0? 1: _temp1371 ==  0){
goto _LL1166;} if( Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1379)) ==  Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1371))){ return;} goto _LL1166; _LL1194: { int done= 0;{
struct _RegionHandle _temp1491= _new_region(); struct _RegionHandle* rgn=&
_temp1491; _push_region( rgn);{ struct Cyc_List_List* inst= 0; while( _temp1403
!=  0) { if( _temp1423 ==  0){ break;} inst=({ struct Cyc_List_List* _temp1492=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1492->hd=( void*)({ struct _tuple5* _temp1493=( struct _tuple5*)
_region_malloc( rgn, sizeof( struct _tuple5)); _temp1493->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp1423))->hd; _temp1493->f2=( void*)({
struct Cyc_Absyn_VarType_struct* _temp1494=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp1494[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp1495; _temp1495.tag= Cyc_Absyn_VarType; _temp1495.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp1403))->hd;
_temp1495;}); _temp1494;}); _temp1493;}); _temp1492->tl= inst; _temp1492;});
_temp1403=(( struct Cyc_List_List*) _check_null( _temp1403))->tl; _temp1423=((
struct Cyc_List_List*) _check_null( _temp1423))->tl;} if( _temp1423 !=  0){
_npop_handler( 0u); goto _LL1166;} if( inst !=  0){ Cyc_Tcutil_unify_it(( void*)({
struct Cyc_Absyn_FnType_struct* _temp1496=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1496[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1497; _temp1497.tag= Cyc_Absyn_FnType; _temp1497.f1=({ struct Cyc_Absyn_FnInfo
_temp1498; _temp1498.tvars= 0; _temp1498.effect= _temp1401; _temp1498.ret_typ=(
void*) _temp1399; _temp1498.args= _temp1397; _temp1498.c_varargs= _temp1395;
_temp1498.cyc_varargs= _temp1393; _temp1498.rgn_po= _temp1391; _temp1498.attributes=
_temp1389; _temp1498;}); _temp1497;}); _temp1496;}), Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1499=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1499[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1500; _temp1500.tag= Cyc_Absyn_FnType; _temp1500.f1=({ struct Cyc_Absyn_FnInfo
_temp1501; _temp1501.tvars= 0; _temp1501.effect= _temp1421; _temp1501.ret_typ=(
void*) _temp1419; _temp1501.args= _temp1417; _temp1501.c_varargs= _temp1415;
_temp1501.cyc_varargs= _temp1413; _temp1501.rgn_po= _temp1411; _temp1501.attributes=
_temp1409; _temp1501;}); _temp1500;}); _temp1499;}))); done= 1;}}; _pop_region(
rgn);} if( done){ return;} Cyc_Tcutil_unify_it( _temp1399, _temp1419); for( 0;
_temp1397 !=  0? _temp1417 !=  0: 0;( _temp1397=(( struct Cyc_List_List*)
_check_null( _temp1397))->tl, _temp1417=(( struct Cyc_List_List*) _check_null(
_temp1417))->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp1397))->hd)).f2,(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp1417))->hd)).f2); Cyc_Tcutil_unify_it((*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp1397))->hd)).f3,(*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp1417))->hd)).f3);} if( _temp1397 !=  0?
1: _temp1417 !=  0){ goto _LL1166;} if( _temp1395 !=  _temp1415){ goto _LL1166;}{
int bad_cyc_vararg= 0;{ struct _tuple11 _temp1503=({ struct _tuple11 _temp1502;
_temp1502.f1= _temp1393; _temp1502.f2= _temp1413; _temp1502;}); struct Cyc_Absyn_VarargInfo*
_temp1513; struct Cyc_Absyn_VarargInfo* _temp1515; struct Cyc_Absyn_VarargInfo*
_temp1517; struct Cyc_Absyn_VarargInfo* _temp1519; struct Cyc_Absyn_VarargInfo*
_temp1521; struct Cyc_Absyn_VarargInfo _temp1523; int _temp1524; void* _temp1526;
void* _temp1528; struct Cyc_Absyn_Tqual _temp1530; struct Cyc_Core_Opt*
_temp1532; struct Cyc_Absyn_VarargInfo* _temp1534; struct Cyc_Absyn_VarargInfo
_temp1536; int _temp1537; void* _temp1539; void* _temp1541; struct Cyc_Absyn_Tqual
_temp1543; struct Cyc_Core_Opt* _temp1545; _LL1505: _LL1516: _temp1515=
_temp1503.f1; if( _temp1515 ==  0){ goto _LL1514;} else{ goto _LL1507;} _LL1514:
_temp1513= _temp1503.f2; if( _temp1513 ==  0){ goto _LL1506;} else{ goto _LL1507;}
_LL1507: _LL1518: _temp1517= _temp1503.f1; if( _temp1517 ==  0){ goto _LL1508;}
else{ goto _LL1509;} _LL1509: _LL1520: _temp1519= _temp1503.f2; if( _temp1519 == 
0){ goto _LL1510;} else{ goto _LL1511;} _LL1511: _LL1535: _temp1534= _temp1503.f1;
if( _temp1534 ==  0){ goto _LL1504;} else{ _temp1536=* _temp1534; _LL1546:
_temp1545= _temp1536.name; goto _LL1544; _LL1544: _temp1543= _temp1536.tq; goto
_LL1542; _LL1542: _temp1541=( void*) _temp1536.type; goto _LL1540; _LL1540:
_temp1539=( void*) _temp1536.rgn; goto _LL1538; _LL1538: _temp1537= _temp1536.inject;
goto _LL1522;} _LL1522: _temp1521= _temp1503.f2; if( _temp1521 ==  0){ goto
_LL1504;} else{ _temp1523=* _temp1521; _LL1533: _temp1532= _temp1523.name; goto
_LL1531; _LL1531: _temp1530= _temp1523.tq; goto _LL1529; _LL1529: _temp1528=(
void*) _temp1523.type; goto _LL1527; _LL1527: _temp1526=( void*) _temp1523.rgn;
goto _LL1525; _LL1525: _temp1524= _temp1523.inject; goto _LL1512;} _LL1506: goto
_LL1504; _LL1508: goto _LL1510; _LL1510: bad_cyc_vararg= 1; goto _LL1504;
_LL1512: Cyc_Tcutil_unify_tqual( _temp1543, _temp1530); Cyc_Tcutil_unify_it(
_temp1541, _temp1528); Cyc_Tcutil_unify_it( _temp1539, _temp1526); if( _temp1537
!=  _temp1524){ bad_cyc_vararg= 1;} goto _LL1504; _LL1504:;} if( bad_cyc_vararg){
goto _LL1166;}{ int bad_effect= 0;{ struct _tuple12 _temp1548=({ struct _tuple12
_temp1547; _temp1547.f1= _temp1401; _temp1547.f2= _temp1421; _temp1547;});
struct Cyc_Core_Opt* _temp1558; struct Cyc_Core_Opt* _temp1560; struct Cyc_Core_Opt*
_temp1562; struct Cyc_Core_Opt* _temp1564; _LL1550: _LL1561: _temp1560=
_temp1548.f1; if( _temp1560 ==  0){ goto _LL1559;} else{ goto _LL1552;} _LL1559:
_temp1558= _temp1548.f2; if( _temp1558 ==  0){ goto _LL1551;} else{ goto _LL1552;}
_LL1552: _LL1563: _temp1562= _temp1548.f1; if( _temp1562 ==  0){ goto _LL1553;}
else{ goto _LL1554;} _LL1554: _LL1565: _temp1564= _temp1548.f2; if( _temp1564 == 
0){ goto _LL1555;} else{ goto _LL1556;} _LL1556: goto _LL1557; _LL1551: goto
_LL1549; _LL1553: goto _LL1555; _LL1555: bad_effect= 1; goto _LL1549; _LL1557:
bad_effect= ! Cyc_Tcutil_unify_effect(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1401))->v,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1421))->v); goto _LL1549; _LL1549:;} if( bad_effect){ goto _LL1166;} if( !
Cyc_Tcutil_same_atts( _temp1409, _temp1389)){ goto _LL1166;} if( ! Cyc_Tcutil_same_rgn_po(
_temp1411, _temp1391)){ goto _LL1166;} return;}}} _LL1196: for( 0; _temp1427 != 
0? _temp1431 !=  0: 0;( _temp1427=(( struct Cyc_List_List*) _check_null(
_temp1427))->tl, _temp1431=(( struct Cyc_List_List*) _check_null( _temp1431))->tl)){
Cyc_Tcutil_unify_tqual((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1427))->hd)).f1,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1431))->hd)).f1); Cyc_Tcutil_unify_it((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1427))->hd)).f2,(*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1431))->hd)).f2);} if( _temp1427 ==  0?
_temp1431 ==  0: 0){ return;} goto _LL1166; _LL1198: _temp1447= _temp1439;
_temp1443= _temp1435; goto _LL1200; _LL1200: for( 0; _temp1443 !=  0? _temp1447
!=  0: 0;( _temp1443=(( struct Cyc_List_List*) _check_null( _temp1443))->tl,
_temp1447=(( struct Cyc_List_List*) _check_null( _temp1447))->tl)){ struct Cyc_Absyn_Structfield*
_temp1566=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1443))->hd; struct Cyc_Absyn_Structfield* _temp1567=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp1447))->hd; if( Cyc_std_zstrptrcmp(
_temp1566->name, _temp1567->name) !=  0){( int) _throw(( void*) Cyc_Tcutil_Unify);}
Cyc_Tcutil_unify_tqual( _temp1566->tq, _temp1567->tq); Cyc_Tcutil_unify_it((
void*) _temp1566->type,( void*) _temp1567->type); if((( ! Cyc_Tcutil_same_atts(
_temp1566->attributes, _temp1567->attributes)? 1:( _temp1566->width !=  0?
_temp1567->width ==  0: 0))? 1:( _temp1567->width !=  0? _temp1566->width ==  0:
0))? 1:(( _temp1566->width !=  0? _temp1567->width !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1566->width)) !=  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1567->width)): 0)){( int) _throw(( void*)
Cyc_Tcutil_Unify);}} if( _temp1443 ==  0? _temp1447 ==  0: 0){ return;} goto
_LL1166; _LL1202: return; _LL1204: Cyc_Tcutil_unify_it( _temp1459, _temp1455);
return; _LL1206: goto _LL1208; _LL1208: goto _LL1210; _LL1210: goto _LL1212;
_LL1212: goto _LL1214; _LL1214: goto _LL1216; _LL1216: if( Cyc_Tcutil_unify_effect(
t1, t2)){ return;} else{ goto _LL1166;} _LL1218: goto _LL1166; _LL1166:;}( int)
_throw(( void*) Cyc_Tcutil_Unify);} int Cyc_Tcutil_is_arithmetic_type( void* t){
void* _temp1568= Cyc_Tcutil_compress( t); _LL1570: if(( unsigned int) _temp1568
>  4u?*(( int*) _temp1568) ==  Cyc_Absyn_IntType: 0){ goto _LL1571;} else{ goto
_LL1572;} _LL1572: if( _temp1568 == ( void*) Cyc_Absyn_FloatType){ goto _LL1573;}
else{ goto _LL1574;} _LL1574: if( _temp1568 == ( void*) Cyc_Absyn_DoubleType){
goto _LL1575;} else{ goto _LL1576;} _LL1576: if(( unsigned int) _temp1568 >  4u?*((
int*) _temp1568) ==  Cyc_Absyn_EnumType: 0){ goto _LL1577;} else{ goto _LL1578;}
_LL1578: goto _LL1579; _LL1571: return 1; _LL1573: return 1; _LL1575: return 1;
_LL1577: return 1; _LL1579: return 0; _LL1569:;} int Cyc_Tcutil_will_lose_precision(
void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple8 _temp1581=({ struct _tuple8 _temp1580; _temp1580.f1= t1;
_temp1580.f2= t2; _temp1580;}); void* _temp1603; void* _temp1605; void*
_temp1607; void* _temp1609; void* _temp1611; void* _temp1613; void* _temp1615;
void* _temp1617; void* _temp1619; void* _temp1621; void* _temp1623; void*
_temp1625; void* _temp1627; void* _temp1629; void* _temp1631; void* _temp1633;
void* _temp1635; void* _temp1637; void* _temp1639; void* _temp1641; void*
_temp1643; void* _temp1645; void* _temp1647; void* _temp1649; void* _temp1651;
void* _temp1653; void* _temp1655; _LL1583: _LL1606: _temp1605= _temp1581.f1; if(
_temp1605 == ( void*) Cyc_Absyn_DoubleType){ goto _LL1604;} else{ goto _LL1585;}
_LL1604: _temp1603= _temp1581.f2; if( _temp1603 == ( void*) Cyc_Absyn_FloatType){
goto _LL1584;} else{ goto _LL1585;} _LL1585: _LL1610: _temp1609= _temp1581.f1;
if( _temp1609 == ( void*) Cyc_Absyn_DoubleType){ goto _LL1608;} else{ goto
_LL1587;} _LL1608: _temp1607= _temp1581.f2; if(( unsigned int) _temp1607 >  4u?*((
int*) _temp1607) ==  Cyc_Absyn_IntType: 0){ goto _LL1586;} else{ goto _LL1587;}
_LL1587: _LL1614: _temp1613= _temp1581.f1; if( _temp1613 == ( void*) Cyc_Absyn_FloatType){
goto _LL1612;} else{ goto _LL1589;} _LL1612: _temp1611= _temp1581.f2; if((
unsigned int) _temp1611 >  4u?*(( int*) _temp1611) ==  Cyc_Absyn_IntType: 0){
goto _LL1588;} else{ goto _LL1589;} _LL1589: _LL1620: _temp1619= _temp1581.f1;
if(( unsigned int) _temp1619 >  4u?*(( int*) _temp1619) ==  Cyc_Absyn_IntType: 0){
_LL1622: _temp1621=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1619)->f2;
if( _temp1621 == ( void*) Cyc_Absyn_B8){ goto _LL1616;} else{ goto _LL1591;}}
else{ goto _LL1591;} _LL1616: _temp1615= _temp1581.f2; if(( unsigned int)
_temp1615 >  4u?*(( int*) _temp1615) ==  Cyc_Absyn_IntType: 0){ _LL1618:
_temp1617=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1615)->f2; if(
_temp1617 == ( void*) Cyc_Absyn_B8){ goto _LL1590;} else{ goto _LL1591;}} else{
goto _LL1591;} _LL1591: _LL1624: _temp1623= _temp1581.f1; if(( unsigned int)
_temp1623 >  4u?*(( int*) _temp1623) ==  Cyc_Absyn_IntType: 0){ _LL1626:
_temp1625=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1623)->f2; if(
_temp1625 == ( void*) Cyc_Absyn_B8){ goto _LL1592;} else{ goto _LL1593;}} else{
goto _LL1593;} _LL1593: _LL1630: _temp1629= _temp1581.f1; if(( unsigned int)
_temp1629 >  4u?*(( int*) _temp1629) ==  Cyc_Absyn_IntType: 0){ _LL1632:
_temp1631=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1629)->f2; if(
_temp1631 == ( void*) Cyc_Absyn_B4){ goto _LL1628;} else{ goto _LL1595;}} else{
goto _LL1595;} _LL1628: _temp1627= _temp1581.f2; if( _temp1627 == ( void*) Cyc_Absyn_FloatType){
goto _LL1594;} else{ goto _LL1595;} _LL1595: _LL1638: _temp1637= _temp1581.f1;
if(( unsigned int) _temp1637 >  4u?*(( int*) _temp1637) ==  Cyc_Absyn_IntType: 0){
_LL1640: _temp1639=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1637)->f2;
if( _temp1639 == ( void*) Cyc_Absyn_B4){ goto _LL1634;} else{ goto _LL1597;}}
else{ goto _LL1597;} _LL1634: _temp1633= _temp1581.f2; if(( unsigned int)
_temp1633 >  4u?*(( int*) _temp1633) ==  Cyc_Absyn_IntType: 0){ _LL1636:
_temp1635=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1633)->f2; if(
_temp1635 == ( void*) Cyc_Absyn_B2){ goto _LL1596;} else{ goto _LL1597;}} else{
goto _LL1597;} _LL1597: _LL1646: _temp1645= _temp1581.f1; if(( unsigned int)
_temp1645 >  4u?*(( int*) _temp1645) ==  Cyc_Absyn_IntType: 0){ _LL1648:
_temp1647=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1645)->f2; if(
_temp1647 == ( void*) Cyc_Absyn_B4){ goto _LL1642;} else{ goto _LL1599;}} else{
goto _LL1599;} _LL1642: _temp1641= _temp1581.f2; if(( unsigned int) _temp1641 > 
4u?*(( int*) _temp1641) ==  Cyc_Absyn_IntType: 0){ _LL1644: _temp1643=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1641)->f2; if( _temp1643 == ( void*) Cyc_Absyn_B1){
goto _LL1598;} else{ goto _LL1599;}} else{ goto _LL1599;} _LL1599: _LL1654:
_temp1653= _temp1581.f1; if(( unsigned int) _temp1653 >  4u?*(( int*) _temp1653)
==  Cyc_Absyn_IntType: 0){ _LL1656: _temp1655=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1653)->f2; if( _temp1655 == ( void*) Cyc_Absyn_B2){ goto _LL1650;} else{
goto _LL1601;}} else{ goto _LL1601;} _LL1650: _temp1649= _temp1581.f2; if((
unsigned int) _temp1649 >  4u?*(( int*) _temp1649) ==  Cyc_Absyn_IntType: 0){
_LL1652: _temp1651=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1649)->f2;
if( _temp1651 == ( void*) Cyc_Absyn_B1){ goto _LL1600;} else{ goto _LL1601;}}
else{ goto _LL1601;} _LL1601: goto _LL1602; _LL1584: return 1; _LL1586: return 1;
_LL1588: return 1; _LL1590: return 0; _LL1592: return 1; _LL1594: return 1;
_LL1596: return 1; _LL1598: return 1; _LL1600: return 1; _LL1602: return 0;
_LL1582:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv* te, void* t,
struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{ struct Cyc_List_List*
el= es; for( 0; el !=  0; el=(( struct Cyc_List_List*) _check_null( el))->tl){
void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);
if( Cyc_Tcutil_is_arithmetic_type( t1)){ if( max_arith_type ==  0? 1: Cyc_Tcutil_will_lose_precision(
t1,( void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){
max_arith_type=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp1657=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1657->v=( void*)
t1; _temp1657;});}}}} if( max_arith_type !=  0){ if( ! Cyc_Tcutil_unify( t,(
void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){ return 0;}}{
struct Cyc_List_List* el= es; for( 0; el !=  0; el=(( struct Cyc_List_List*)
_check_null( el))->tl){ if( ! Cyc_Tcutil_coerce_assign( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( el))->hd, t)){({ struct Cyc_std_String_pa_struct
_temp1660; _temp1660.tag= Cyc_std_String_pa; _temp1660.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null((( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);{
struct Cyc_std_String_pa_struct _temp1659; _temp1659.tag= Cyc_std_String_pa;
_temp1659.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1658[
2u]={& _temp1659,& _temp1660}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( el))->hd)->loc, _tag_arr("type mismatch: expecting %s but found %s",
sizeof( unsigned char), 41u), _tag_arr( _temp1658, sizeof( void*), 2u));}}});
return 0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ( te, e)){ void*
_temp1661= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
_LL1663: if(( unsigned int) _temp1661 >  4u?*(( int*) _temp1661) ==  Cyc_Absyn_PointerType:
0){ goto _LL1664;} else{ goto _LL1665;} _LL1665: goto _LL1666; _LL1664: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); goto _LL1662; _LL1666: return 0; _LL1662:;} return 1;}
int Cyc_Tcutil_is_integral_type( void* t){ void* _temp1667= Cyc_Tcutil_compress(
t); _LL1669: if(( unsigned int) _temp1667 >  4u?*(( int*) _temp1667) ==  Cyc_Absyn_IntType:
0){ goto _LL1670;} else{ goto _LL1671;} _LL1671: if(( unsigned int) _temp1667 > 
4u?*(( int*) _temp1667) ==  Cyc_Absyn_EnumType: 0){ goto _LL1672;} else{ goto
_LL1673;} _LL1673: goto _LL1674; _LL1670: return 1; _LL1672: return 1; _LL1674:
return 0; _LL1668:;} int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_uint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){ if( Cyc_Tcutil_will_lose_precision((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_uint_t)){({
void* _temp1675[ 0u]={}; Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u), _tag_arr( _temp1675, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;}
if( Cyc_Tcutil_is_integral_type(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){
if( Cyc_Tcutil_will_lose_precision(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, Cyc_Absyn_sint_t)){({ void* _temp1676[ 0u]={}; Cyc_Tcutil_warn( e->loc,
_tag_arr("integral size mismatch; conversion supplied", sizeof( unsigned char),
44u), _tag_arr( _temp1676, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); return 1;} return 0;} int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8
_temp1678=({ struct _tuple8 _temp1677; _temp1677.f1= t1; _temp1677.f2= t2;
_temp1677;}); void* _temp1690; struct Cyc_Absyn_PtrInfo _temp1692; void*
_temp1694; struct Cyc_Absyn_PtrInfo _temp1696; void* _temp1698; struct Cyc_Absyn_Exp*
_temp1700; struct Cyc_Absyn_Tqual _temp1702; void* _temp1704; void* _temp1706;
struct Cyc_Absyn_Exp* _temp1708; struct Cyc_Absyn_Tqual _temp1710; void*
_temp1712; void* _temp1714; struct Cyc_Absyn_TunionInfo _temp1716; struct Cyc_List_List*
_temp1718; void* _temp1720; struct Cyc_Absyn_Tuniondecl* _temp1722; void*
_temp1724; struct Cyc_Absyn_TunionFieldInfo _temp1726; struct Cyc_List_List*
_temp1728; void* _temp1730; struct Cyc_Absyn_Tunionfield* _temp1732; struct Cyc_Absyn_Tuniondecl*
_temp1734; void* _temp1736; struct Cyc_Absyn_TunionInfo _temp1738; void*
_temp1740; struct Cyc_List_List* _temp1742; void* _temp1744; struct Cyc_Absyn_Tuniondecl*
_temp1746; void* _temp1748; struct Cyc_Absyn_PtrInfo _temp1750; struct Cyc_Absyn_Conref*
_temp1752; struct Cyc_Absyn_Tqual _temp1754; struct Cyc_Absyn_Conref* _temp1756;
void* _temp1758; void* _temp1760; _LL1680: _LL1695: _temp1694= _temp1678.f1; if((
unsigned int) _temp1694 >  4u?*(( int*) _temp1694) ==  Cyc_Absyn_PointerType: 0){
_LL1697: _temp1696=(( struct Cyc_Absyn_PointerType_struct*) _temp1694)->f1; goto
_LL1691;} else{ goto _LL1682;} _LL1691: _temp1690= _temp1678.f2; if((
unsigned int) _temp1690 >  4u?*(( int*) _temp1690) ==  Cyc_Absyn_PointerType: 0){
_LL1693: _temp1692=(( struct Cyc_Absyn_PointerType_struct*) _temp1690)->f1; goto
_LL1681;} else{ goto _LL1682;} _LL1682: _LL1707: _temp1706= _temp1678.f1; if((
unsigned int) _temp1706 >  4u?*(( int*) _temp1706) ==  Cyc_Absyn_ArrayType: 0){
_LL1713: _temp1712=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1706)->f1;
goto _LL1711; _LL1711: _temp1710=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1706)->f2; goto _LL1709; _LL1709: _temp1708=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1706)->f3; goto _LL1699;} else{ goto _LL1684;} _LL1699: _temp1698=
_temp1678.f2; if(( unsigned int) _temp1698 >  4u?*(( int*) _temp1698) ==  Cyc_Absyn_ArrayType:
0){ _LL1705: _temp1704=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1698)->f1;
goto _LL1703; _LL1703: _temp1702=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1698)->f2; goto _LL1701; _LL1701: _temp1700=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1698)->f3; goto _LL1683;} else{ goto _LL1684;} _LL1684: _LL1725: _temp1724=
_temp1678.f1; if(( unsigned int) _temp1724 >  4u?*(( int*) _temp1724) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1727: _temp1726=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1724)->f1;
_LL1731: _temp1730=( void*) _temp1726.field_info; if(*(( int*) _temp1730) == 
Cyc_Absyn_KnownTunionfield){ _LL1735: _temp1734=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1730)->f1; goto _LL1733; _LL1733: _temp1732=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1730)->f2; goto _LL1729;} else{ goto _LL1686;} _LL1729: _temp1728=
_temp1726.targs; goto _LL1715;} else{ goto _LL1686;} _LL1715: _temp1714=
_temp1678.f2; if(( unsigned int) _temp1714 >  4u?*(( int*) _temp1714) ==  Cyc_Absyn_TunionType:
0){ _LL1717: _temp1716=(( struct Cyc_Absyn_TunionType_struct*) _temp1714)->f1;
_LL1721: _temp1720=( void*) _temp1716.tunion_info; if(*(( int*) _temp1720) == 
Cyc_Absyn_KnownTunion){ _LL1723: _temp1722=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1720)->f1; goto _LL1719;} else{ goto _LL1686;} _LL1719: _temp1718=
_temp1716.targs; goto _LL1685;} else{ goto _LL1686;} _LL1686: _LL1749: _temp1748=
_temp1678.f1; if(( unsigned int) _temp1748 >  4u?*(( int*) _temp1748) ==  Cyc_Absyn_PointerType:
0){ _LL1751: _temp1750=(( struct Cyc_Absyn_PointerType_struct*) _temp1748)->f1;
_LL1761: _temp1760=( void*) _temp1750.elt_typ; goto _LL1759; _LL1759: _temp1758=(
void*) _temp1750.rgn_typ; goto _LL1757; _LL1757: _temp1756= _temp1750.nullable;
goto _LL1755; _LL1755: _temp1754= _temp1750.tq; goto _LL1753; _LL1753: _temp1752=
_temp1750.bounds; goto _LL1737;} else{ goto _LL1688;} _LL1737: _temp1736=
_temp1678.f2; if(( unsigned int) _temp1736 >  4u?*(( int*) _temp1736) ==  Cyc_Absyn_TunionType:
0){ _LL1739: _temp1738=(( struct Cyc_Absyn_TunionType_struct*) _temp1736)->f1;
_LL1745: _temp1744=( void*) _temp1738.tunion_info; if(*(( int*) _temp1744) == 
Cyc_Absyn_KnownTunion){ _LL1747: _temp1746=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1744)->f1; goto _LL1743;} else{ goto _LL1688;} _LL1743: _temp1742=
_temp1738.targs; goto _LL1741; _LL1741: _temp1740=( void*) _temp1738.rgn; goto
_LL1687;} else{ goto _LL1688;} _LL1688: goto _LL1689; _LL1681: { int okay= 1;
if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1696.nullable, _temp1692.nullable)){
void* _temp1762=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp1696.nullable))->v; int _temp1768; _LL1764:
if(( unsigned int) _temp1762 >  1u?*(( int*) _temp1762) ==  Cyc_Absyn_Eq_constr:
0){ _LL1769: _temp1768=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1762)->f1;
goto _LL1765;} else{ goto _LL1766;} _LL1766: goto _LL1767; _LL1765: okay= !
_temp1768; goto _LL1763; _LL1767:( int) _throw(({ void* _temp1770[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("silent_castable conref not eq", sizeof( unsigned char), 30u), _tag_arr(
_temp1770, sizeof( void*), 0u));})); _LL1763:;} if( ! Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_boundscmp, _temp1696.bounds, _temp1692.bounds)){ struct _tuple8
_temp1772=({ struct _tuple8 _temp1771; _temp1771.f1=( void*)( Cyc_Absyn_compress_conref(
_temp1696.bounds))->v; _temp1771.f2=( void*)( Cyc_Absyn_compress_conref(
_temp1692.bounds))->v; _temp1771;}); void* _temp1782; void* _temp1784; void*
_temp1786; void* _temp1788; void* _temp1790; void* _temp1792; struct Cyc_Absyn_Exp*
_temp1794; void* _temp1796; void* _temp1798; struct Cyc_Absyn_Exp* _temp1800;
void* _temp1802; void* _temp1804; void* _temp1806; void* _temp1808; _LL1774:
_LL1787: _temp1786= _temp1772.f1; if(( unsigned int) _temp1786 >  1u?*(( int*)
_temp1786) ==  Cyc_Absyn_Eq_constr: 0){ _LL1789: _temp1788=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1786)->f1; if(( unsigned int) _temp1788 >  1u?*(( int*) _temp1788) ==  Cyc_Absyn_Upper_b:
0){ goto _LL1783;} else{ goto _LL1776;}} else{ goto _LL1776;} _LL1783: _temp1782=
_temp1772.f2; if(( unsigned int) _temp1782 >  1u?*(( int*) _temp1782) ==  Cyc_Absyn_Eq_constr:
0){ _LL1785: _temp1784=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1782)->f1;
if( _temp1784 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1775;} else{ goto
_LL1776;}} else{ goto _LL1776;} _LL1776: _LL1797: _temp1796= _temp1772.f1; if((
unsigned int) _temp1796 >  1u?*(( int*) _temp1796) ==  Cyc_Absyn_Eq_constr: 0){
_LL1799: _temp1798=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1796)->f1;
if(( unsigned int) _temp1798 >  1u?*(( int*) _temp1798) ==  Cyc_Absyn_Upper_b: 0){
_LL1801: _temp1800=(( struct Cyc_Absyn_Upper_b_struct*) _temp1798)->f1; goto
_LL1791;} else{ goto _LL1778;}} else{ goto _LL1778;} _LL1791: _temp1790=
_temp1772.f2; if(( unsigned int) _temp1790 >  1u?*(( int*) _temp1790) ==  Cyc_Absyn_Eq_constr:
0){ _LL1793: _temp1792=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1790)->f1;
if(( unsigned int) _temp1792 >  1u?*(( int*) _temp1792) ==  Cyc_Absyn_Upper_b: 0){
_LL1795: _temp1794=(( struct Cyc_Absyn_Upper_b_struct*) _temp1792)->f1; goto
_LL1777;} else{ goto _LL1778;}} else{ goto _LL1778;} _LL1778: _LL1807: _temp1806=
_temp1772.f1; if(( unsigned int) _temp1806 >  1u?*(( int*) _temp1806) ==  Cyc_Absyn_Eq_constr:
0){ _LL1809: _temp1808=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1806)->f1;
if( _temp1808 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1803;} else{ goto
_LL1780;}} else{ goto _LL1780;} _LL1803: _temp1802= _temp1772.f2; if((
unsigned int) _temp1802 >  1u?*(( int*) _temp1802) ==  Cyc_Absyn_Eq_constr: 0){
_LL1805: _temp1804=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1802)->f1;
if( _temp1804 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL1779;} else{ goto
_LL1780;}} else{ goto _LL1780;} _LL1780: goto _LL1781; _LL1775: okay= 1; goto
_LL1773; _LL1777: okay= okay? Cyc_Evexp_eval_const_uint_exp( _temp1800) >=  Cyc_Evexp_eval_const_uint_exp(
_temp1794): 0;({ void* _temp1810[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("implicit cast to shorter array",
sizeof( unsigned char), 31u), _tag_arr( _temp1810, sizeof( void*), 0u));}); goto
_LL1773; _LL1779: okay= 1; goto _LL1773; _LL1781: okay= 0; goto _LL1773; _LL1773:;}
okay= okay? Cyc_Tcutil_unify(( void*) _temp1696.elt_typ,( void*) _temp1692.elt_typ):
0; okay= okay? Cyc_Tcutil_unify(( void*) _temp1696.rgn_typ,( void*) _temp1692.rgn_typ)?
1: Cyc_Tcenv_region_outlives( te,( void*) _temp1696.rgn_typ,( void*) _temp1692.rgn_typ):
0; okay= okay? !( _temp1696.tq).q_const? 1:( _temp1692.tq).q_const: 0; return
okay;} _LL1683: { int okay; okay=( _temp1708 !=  0? _temp1700 !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1708)) ==  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1700)): 0; return( okay? Cyc_Tcutil_unify(
_temp1712, _temp1704): 0)? ! _temp1710.q_const? 1: _temp1702.q_const: 0;}
_LL1685: if(( _temp1734 ==  _temp1722? 1: Cyc_Absyn_qvar_cmp( _temp1734->name,
_temp1722->name) ==  0)? _temp1732->typs ==  0: 0){ for( 0; _temp1728 !=  0?
_temp1718 !=  0: 0;( _temp1728=(( struct Cyc_List_List*) _check_null( _temp1728))->tl,
_temp1718=(( struct Cyc_List_List*) _check_null( _temp1718))->tl)){ if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_List_List*) _check_null( _temp1728))->hd,( void*)(( struct
Cyc_List_List*) _check_null( _temp1718))->hd)){ break;}} if( _temp1728 ==  0?
_temp1718 ==  0: 0){ return 1;}} return 0; _LL1687:{ void* _temp1811= Cyc_Tcutil_compress(
_temp1760); struct Cyc_Absyn_TunionFieldInfo _temp1817; struct Cyc_List_List*
_temp1819; void* _temp1821; struct Cyc_Absyn_Tunionfield* _temp1823; struct Cyc_Absyn_Tuniondecl*
_temp1825; _LL1813: if(( unsigned int) _temp1811 >  4u?*(( int*) _temp1811) == 
Cyc_Absyn_TunionFieldType: 0){ _LL1818: _temp1817=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1811)->f1; _LL1822: _temp1821=( void*) _temp1817.field_info; if(*(( int*)
_temp1821) ==  Cyc_Absyn_KnownTunionfield){ _LL1826: _temp1825=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1821)->f1; goto _LL1824; _LL1824: _temp1823=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1821)->f2; goto _LL1820;} else{ goto _LL1815;} _LL1820: _temp1819=
_temp1817.targs; goto _LL1814;} else{ goto _LL1815;} _LL1815: goto _LL1816;
_LL1814: if( ! Cyc_Tcutil_unify( _temp1758, _temp1740)? ! Cyc_Tcenv_region_outlives(
te, _temp1758, _temp1740): 0){ return 0;} if( !(( int(*)( int(* cmp)( int, int),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp, _temp1756,(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
0))){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp, _temp1752,
Cyc_Absyn_new_conref( Cyc_Absyn_bounds_one))){ return 0;} if( Cyc_Absyn_qvar_cmp(
_temp1746->name, _temp1825->name) ==  0? _temp1823->typs !=  0: 0){ int okay= 1;
for( 0; _temp1819 !=  0? _temp1742 !=  0: 0;( _temp1819=(( struct Cyc_List_List*)
_check_null( _temp1819))->tl, _temp1742=(( struct Cyc_List_List*) _check_null(
_temp1742))->tl)){ if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_List_List*)
_check_null( _temp1819))->hd,( void*)(( struct Cyc_List_List*) _check_null(
_temp1742))->hd)){ okay= 0; break;}} if(( ! okay? 1: _temp1819 !=  0)? 1:
_temp1742 !=  0){ return 0;} return 1;} goto _LL1812; _LL1816: goto _LL1812;
_LL1812:;} return 0; _LL1689: return Cyc_Tcutil_unify( t1, t2); _LL1679:;}} int
Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void*
t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} if( Cyc_Tcutil_is_arithmetic_type(
t1)? Cyc_Tcutil_is_arithmetic_type( t2): 0){ if( Cyc_Tcutil_will_lose_precision(
t1, t2)){({ void* _temp1827[ 0u]={}; Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u), _tag_arr( _temp1827, sizeof( void*), 0u));});} Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){
Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2);({ struct Cyc_std_String_pa_struct
_temp1830; _temp1830.tag= Cyc_std_String_pa; _temp1830.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t2);{ struct Cyc_std_String_pa_struct _temp1829;
_temp1829.tag= Cyc_std_String_pa; _temp1829.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp1828[ 2u]={& _temp1829,& _temp1830}; Cyc_Tcutil_warn( e->loc,
_tag_arr("implicit cast from %s to %s", sizeof( unsigned char), 28u), _tag_arr(
_temp1828, sizeof( void*), 2u));}}}); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg(
te, e, t);} static int Cyc_Tcutil_any_promote( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e){ void* old_typ= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); void* t;{ void* _temp1831= old_typ; void* _temp1843;
void* _temp1845; _LL1833: if(( unsigned int) _temp1831 >  4u?*(( int*) _temp1831)
==  Cyc_Absyn_EnumType: 0){ goto _LL1834;} else{ goto _LL1835;} _LL1835: if((
unsigned int) _temp1831 >  4u?*(( int*) _temp1831) ==  Cyc_Absyn_IntType: 0){
_LL1846: _temp1845=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1831)->f1;
goto _LL1844; _LL1844: _temp1843=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1831)->f2; goto _LL1836;} else{ goto _LL1837;} _LL1837: if( _temp1831 == (
void*) Cyc_Absyn_FloatType){ goto _LL1838;} else{ goto _LL1839;} _LL1839: if(
_temp1831 == ( void*) Cyc_Absyn_DoubleType){ goto _LL1840;} else{ goto _LL1841;}
_LL1841: goto _LL1842; _LL1834: t= Cyc_Absyn_sint_t; goto _LL1832; _LL1836:{
void* _temp1847= _temp1843; _LL1849: if( _temp1847 == ( void*) Cyc_Absyn_B1){
goto _LL1850;} else{ goto _LL1851;} _LL1851: if( _temp1847 == ( void*) Cyc_Absyn_B2){
goto _LL1852;} else{ goto _LL1853;} _LL1853: if( _temp1847 == ( void*) Cyc_Absyn_B4){
goto _LL1854;} else{ goto _LL1855;} _LL1855: if( _temp1847 == ( void*) Cyc_Absyn_B8){
goto _LL1856;} else{ goto _LL1848;} _LL1850: t= Cyc_Absyn_sint_t; goto _LL1848;
_LL1852: t= Cyc_Absyn_sint_t; goto _LL1848; _LL1854: t= _temp1845 == ( void*)
Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL1848; _LL1856: t=
_temp1845 == ( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ulonglong_t: Cyc_Absyn_slonglong_t;
goto _LL1848; _LL1848:;} goto _LL1832; _LL1838: t= Cyc_Absyn_float_t; goto
_LL1832; _LL1840: t= Cyc_Absyn_double_t; goto _LL1832; _LL1842: return 0;
_LL1832:;} Cyc_Tcutil_unchecked_cast( te, e, t); return 1;} int Cyc_Tcutil_coerce_use(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t2){ void* t1= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); if( Cyc_Tcutil_unify(
t1, t2)){ return 1;} Cyc_Tcutil_any_promote( te, e); if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, t2)){ return 1;} else{ if( Cyc_Tcutil_silent_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{
if( Cyc_Tcutil_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e,
t2);({ struct Cyc_std_String_pa_struct _temp1859; _temp1859.tag= Cyc_std_String_pa;
_temp1859.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_std_String_pa_struct
_temp1858; _temp1858.tag= Cyc_std_String_pa; _temp1858.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t1);{ void* _temp1857[ 2u]={& _temp1858,& _temp1859};
Cyc_Tcutil_warn( e->loc, _tag_arr("implicit cast from %s to %s", sizeof(
unsigned char), 28u), _tag_arr( _temp1857, sizeof( void*), 2u));}}}); return 1;}
else{ return 0;}}}} int Cyc_Tcutil_coerceable( void* t){ void* _temp1860= Cyc_Tcutil_compress(
t); _LL1862: if(( unsigned int) _temp1860 >  4u?*(( int*) _temp1860) ==  Cyc_Absyn_IntType:
0){ goto _LL1863;} else{ goto _LL1864;} _LL1864: if( _temp1860 == ( void*) Cyc_Absyn_FloatType){
goto _LL1865;} else{ goto _LL1866;} _LL1866: if( _temp1860 == ( void*) Cyc_Absyn_DoubleType){
goto _LL1867;} else{ goto _LL1868;} _LL1868: goto _LL1869; _LL1863: return 1;
_LL1865: return 1; _LL1867: return 1; _LL1869: return 0; _LL1861:;} static
struct _tuple4* Cyc_Tcutil_flatten_typ_f( struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield*
x){ return({ struct _tuple4* _temp1870=( struct _tuple4*) GC_malloc( sizeof(
struct _tuple4)); _temp1870->f1= x->tq; _temp1870->f2= Cyc_Tcutil_substitute(
inst,( void*) x->type); _temp1870;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ(
struct Cyc_Tcenv_Tenv* te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void*
_temp1871= t1; struct Cyc_List_List* _temp1883; struct Cyc_Absyn_Structdecl**
_temp1885; struct Cyc_List_List* _temp1887; struct _tuple1* _temp1889; struct
Cyc_List_List* _temp1891; _LL1873: if( _temp1871 == ( void*) Cyc_Absyn_VoidType){
goto _LL1874;} else{ goto _LL1875;} _LL1875: if(( unsigned int) _temp1871 >  4u?*((
int*) _temp1871) ==  Cyc_Absyn_TupleType: 0){ _LL1884: _temp1883=(( struct Cyc_Absyn_TupleType_struct*)
_temp1871)->f1; goto _LL1876;} else{ goto _LL1877;} _LL1877: if(( unsigned int)
_temp1871 >  4u?*(( int*) _temp1871) ==  Cyc_Absyn_StructType: 0){ _LL1890:
_temp1889=(( struct Cyc_Absyn_StructType_struct*) _temp1871)->f1; goto _LL1888;
_LL1888: _temp1887=(( struct Cyc_Absyn_StructType_struct*) _temp1871)->f2; goto
_LL1886; _LL1886: _temp1885=(( struct Cyc_Absyn_StructType_struct*) _temp1871)->f3;
goto _LL1878;} else{ goto _LL1879;} _LL1879: if(( unsigned int) _temp1871 >  4u?*((
int*) _temp1871) ==  Cyc_Absyn_AnonStructType: 0){ _LL1892: _temp1891=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1871)->f1; goto _LL1880;} else{ goto
_LL1881;} _LL1881: goto _LL1882; _LL1874: return 0; _LL1876: return _temp1883;
_LL1878: if( _temp1885 ==  0? 1: _temp1889 ==  0){ return({ struct Cyc_List_List*
_temp1893=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1893->hd=( void*)({ struct _tuple4* _temp1894=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1894->f1= Cyc_Absyn_empty_tqual(); _temp1894->f2=
t1; _temp1894;}); _temp1893->tl= 0; _temp1893;});}{ struct Cyc_Absyn_Structdecl*
_temp1895=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp1885)); struct
_tuple1 _temp1896=*(( struct _tuple1*) _check_null( _temp1889)); struct Cyc_List_List*
_temp1897=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp1895->tvs, _temp1887); if( _temp1895->fields ==  0){
return({ struct Cyc_List_List* _temp1898=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1898->hd=( void*)({ struct _tuple4*
_temp1899=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1899->f1=
Cyc_Absyn_empty_tqual(); _temp1899->f2= t1; _temp1899;}); _temp1898->tl= 0;
_temp1898;});} return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, _temp1897,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1895->fields))->v);}
_LL1880: return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, 0, _temp1891); _LL1882: return({
struct Cyc_List_List* _temp1900=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1900->hd=( void*)({ struct _tuple4* _temp1901=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1901->f1= Cyc_Absyn_empty_tqual();
_temp1901->f2= t1; _temp1901;}); _temp1900->tl= 0; _temp1900;}); _LL1872:;}}
static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2); static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;}{ struct Cyc_List_List* a= assume; for( 0; a !=  0; a=(( struct
Cyc_List_List*) _check_null( a))->tl){ if( Cyc_Tcutil_unify( t1,(*(( struct
_tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f1)? Cyc_Tcutil_unify(
t2,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f2): 0){
return 1;}}} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct
_tuple8 _temp1903=({ struct _tuple8 _temp1902; _temp1902.f1= t1; _temp1902.f2=
t2; _temp1902;}); void* _temp1909; struct Cyc_Absyn_PtrInfo _temp1911; struct
Cyc_Absyn_Conref* _temp1913; struct Cyc_Absyn_Tqual _temp1915; struct Cyc_Absyn_Conref*
_temp1917; void* _temp1919; void* _temp1921; void* _temp1923; struct Cyc_Absyn_PtrInfo
_temp1925; struct Cyc_Absyn_Conref* _temp1927; struct Cyc_Absyn_Tqual _temp1929;
struct Cyc_Absyn_Conref* _temp1931; void* _temp1933; void* _temp1935; _LL1905:
_LL1924: _temp1923= _temp1903.f1; if(( unsigned int) _temp1923 >  4u?*(( int*)
_temp1923) ==  Cyc_Absyn_PointerType: 0){ _LL1926: _temp1925=(( struct Cyc_Absyn_PointerType_struct*)
_temp1923)->f1; _LL1936: _temp1935=( void*) _temp1925.elt_typ; goto _LL1934;
_LL1934: _temp1933=( void*) _temp1925.rgn_typ; goto _LL1932; _LL1932: _temp1931=
_temp1925.nullable; goto _LL1930; _LL1930: _temp1929= _temp1925.tq; goto _LL1928;
_LL1928: _temp1927= _temp1925.bounds; goto _LL1910;} else{ goto _LL1907;}
_LL1910: _temp1909= _temp1903.f2; if(( unsigned int) _temp1909 >  4u?*(( int*)
_temp1909) ==  Cyc_Absyn_PointerType: 0){ _LL1912: _temp1911=(( struct Cyc_Absyn_PointerType_struct*)
_temp1909)->f1; _LL1922: _temp1921=( void*) _temp1911.elt_typ; goto _LL1920;
_LL1920: _temp1919=( void*) _temp1911.rgn_typ; goto _LL1918; _LL1918: _temp1917=
_temp1911.nullable; goto _LL1916; _LL1916: _temp1915= _temp1911.tq; goto _LL1914;
_LL1914: _temp1913= _temp1911.bounds; goto _LL1906;} else{ goto _LL1907;}
_LL1907: goto _LL1908; _LL1906: if( _temp1929.q_const? ! _temp1915.q_const: 0){
return 0;} if(( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1931, _temp1917)?(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1931): 0)? !(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1917): 0){ return 0;} if( ! Cyc_Tcutil_unify( _temp1933, _temp1919)? ! Cyc_Tcenv_region_outlives(
te, _temp1933, _temp1919): 0){ return 0;} if( ! Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp,
_temp1927, _temp1913)){ struct _tuple8 _temp1938=({ struct _tuple8 _temp1937;
_temp1937.f1= Cyc_Absyn_conref_val( _temp1927); _temp1937.f2= Cyc_Absyn_conref_val(
_temp1913); _temp1937;}); void* _temp1946; void* _temp1948; void* _temp1950;
struct Cyc_Absyn_Exp* _temp1952; void* _temp1954; struct Cyc_Absyn_Exp*
_temp1956; _LL1940: _LL1949: _temp1948= _temp1938.f1; if(( unsigned int)
_temp1948 >  1u?*(( int*) _temp1948) ==  Cyc_Absyn_Upper_b: 0){ goto _LL1947;}
else{ goto _LL1942;} _LL1947: _temp1946= _temp1938.f2; if( _temp1946 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL1941;} else{ goto _LL1942;} _LL1942: _LL1955:
_temp1954= _temp1938.f1; if(( unsigned int) _temp1954 >  1u?*(( int*) _temp1954)
==  Cyc_Absyn_Upper_b: 0){ _LL1957: _temp1956=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1954)->f1; goto _LL1951;} else{ goto _LL1944;} _LL1951: _temp1950=
_temp1938.f2; if(( unsigned int) _temp1950 >  1u?*(( int*) _temp1950) ==  Cyc_Absyn_Upper_b:
0){ _LL1953: _temp1952=(( struct Cyc_Absyn_Upper_b_struct*) _temp1950)->f1; goto
_LL1943;} else{ goto _LL1944;} _LL1944: goto _LL1945; _LL1941: goto _LL1939;
_LL1943: if( Cyc_Evexp_eval_const_uint_exp( _temp1956) <  Cyc_Evexp_eval_const_uint_exp(
_temp1952)){ return 0;} goto _LL1939; _LL1945: return 0; _LL1939:;} return Cyc_Tcutil_ptrsubtype(
te,({ struct Cyc_List_List* _temp1958=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1958->hd=( void*)({ struct _tuple8*
_temp1959=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp1959->f1=
t1; _temp1959->f2= t2; _temp1959;}); _temp1958->tl= assume; _temp1958;}),
_temp1935, _temp1921); _LL1908: return 0; _LL1904:;}} static int Cyc_Tcutil_isomorphic(
void* t1, void* t2){ struct _tuple8 _temp1961=({ struct _tuple8 _temp1960;
_temp1960.f1= Cyc_Tcutil_compress( t1); _temp1960.f2= Cyc_Tcutil_compress( t2);
_temp1960;}); void* _temp1967; void* _temp1969; void* _temp1971; void* _temp1973;
_LL1963: _LL1972: _temp1971= _temp1961.f1; if(( unsigned int) _temp1971 >  4u?*((
int*) _temp1971) ==  Cyc_Absyn_IntType: 0){ _LL1974: _temp1973=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1971)->f2; goto _LL1968;} else{ goto _LL1965;}
_LL1968: _temp1967= _temp1961.f2; if(( unsigned int) _temp1967 >  4u?*(( int*)
_temp1967) ==  Cyc_Absyn_IntType: 0){ _LL1970: _temp1969=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1967)->f2; goto _LL1964;} else{ goto _LL1965;} _LL1965: goto _LL1966;
_LL1964: return _temp1973 ==  _temp1969; _LL1966: return 0; _LL1962:;} static
int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2){ struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ(
te, t1); struct Cyc_List_List* tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0;
tqs2 !=  0;( tqs2=(( struct Cyc_List_List*) _check_null( tqs2))->tl, tqs1=((
struct Cyc_List_List*) _check_null( tqs1))->tl)){ if( tqs1 ==  0){ return 0;}{
struct _tuple4 _temp1977; void* _temp1978; struct Cyc_Absyn_Tqual _temp1980;
struct _tuple4* _temp1975=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( tqs1))->hd; _temp1977=* _temp1975; _LL1981: _temp1980= _temp1977.f1;
goto _LL1979; _LL1979: _temp1978= _temp1977.f2; goto _LL1976; _LL1976: { struct
_tuple4 _temp1984; void* _temp1985; struct Cyc_Absyn_Tqual _temp1987; struct
_tuple4* _temp1982=( struct _tuple4*)(( struct Cyc_List_List*) _check_null( tqs2))->hd;
_temp1984=* _temp1982; _LL1988: _temp1987= _temp1984.f1; goto _LL1986; _LL1986:
_temp1985= _temp1984.f2; goto _LL1983; _LL1983: if( _temp1987.q_const? Cyc_Tcutil_subtype(
te, assume, _temp1978, _temp1985): 0){ continue;} else{ if( Cyc_Tcutil_unify(
_temp1978, _temp1985)){ continue;} else{ if( Cyc_Tcutil_isomorphic( _temp1978,
_temp1985)){ continue;} else{ return 0;}}}}}} return 1;} int Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
if( Cyc_Tcutil_unify( t1, t2)){ return 1;} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress(
t2);{ void* _temp1989= t2; struct Cyc_Absyn_Uniondecl** _temp1997; struct Cyc_List_List*
_temp1999; void* _temp2001; _LL1991: if(( unsigned int) _temp1989 >  4u?*(( int*)
_temp1989) ==  Cyc_Absyn_UnionType: 0){ _LL2000: _temp1999=(( struct Cyc_Absyn_UnionType_struct*)
_temp1989)->f2; goto _LL1998; _LL1998: _temp1997=(( struct Cyc_Absyn_UnionType_struct*)
_temp1989)->f3; goto _LL1992;} else{ goto _LL1993;} _LL1993: if(( unsigned int)
_temp1989 >  4u?*(( int*) _temp1989) ==  Cyc_Absyn_IntType: 0){ _LL2002:
_temp2001=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1989)->f2; if(
_temp2001 == ( void*) Cyc_Absyn_B4){ goto _LL1994;} else{ goto _LL1995;}} else{
goto _LL1995;} _LL1995: goto _LL1996; _LL1992: if( _temp1997 ==  0){ goto
_LL1990;}{ struct Cyc_Absyn_Uniondecl* _temp2003=*(( struct Cyc_Absyn_Uniondecl**)
_check_null( _temp1997)); if( _temp2003->fields ==  0){ goto _LL1990;}{ struct
Cyc_List_List* _temp2004=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_zip)( _temp2003->tvs, _temp1999);{ struct Cyc_List_List*
fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp2003->fields))->v;
for( 0; fs !=  0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){ if( Cyc_Tcutil_unify(
t1, Cyc_Tcutil_substitute( _temp2004,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( fs))->hd)->type))){ return 1;}}} goto
_LL1990;}} _LL1994: if( Cyc_Tcutil_typ_kind( t2) == ( void*) Cyc_Absyn_BoxKind){
return 1;} goto _LL1990; _LL1996: goto _LL1990; _LL1990:;}{ void* _temp2005= t1;
struct Cyc_Absyn_PtrInfo _temp2023; struct Cyc_Absyn_Conref* _temp2025; struct
Cyc_Absyn_Tqual _temp2027; struct Cyc_Absyn_Conref* _temp2029; void* _temp2031;
void* _temp2033; struct Cyc_Absyn_Exp* _temp2035; struct Cyc_Absyn_Tqual
_temp2037; void* _temp2039; struct Cyc_Absyn_Enumdecl* _temp2041; _LL2007: if((
unsigned int) _temp2005 >  4u?*(( int*) _temp2005) ==  Cyc_Absyn_PointerType: 0){
_LL2024: _temp2023=(( struct Cyc_Absyn_PointerType_struct*) _temp2005)->f1;
_LL2034: _temp2033=( void*) _temp2023.elt_typ; goto _LL2032; _LL2032: _temp2031=(
void*) _temp2023.rgn_typ; goto _LL2030; _LL2030: _temp2029= _temp2023.nullable;
goto _LL2028; _LL2028: _temp2027= _temp2023.tq; goto _LL2026; _LL2026: _temp2025=
_temp2023.bounds; goto _LL2008;} else{ goto _LL2009;} _LL2009: if(( unsigned int)
_temp2005 >  4u?*(( int*) _temp2005) ==  Cyc_Absyn_ArrayType: 0){ _LL2040:
_temp2039=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2005)->f1; goto
_LL2038; _LL2038: _temp2037=(( struct Cyc_Absyn_ArrayType_struct*) _temp2005)->f2;
goto _LL2036; _LL2036: _temp2035=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2005)->f3; goto _LL2010;} else{ goto _LL2011;} _LL2011: if(( unsigned int)
_temp2005 >  4u?*(( int*) _temp2005) ==  Cyc_Absyn_EnumType: 0){ _LL2042:
_temp2041=(( struct Cyc_Absyn_EnumType_struct*) _temp2005)->f2; goto _LL2012;}
else{ goto _LL2013;} _LL2013: if(( unsigned int) _temp2005 >  4u?*(( int*)
_temp2005) ==  Cyc_Absyn_IntType: 0){ goto _LL2014;} else{ goto _LL2015;}
_LL2015: if( _temp2005 == ( void*) Cyc_Absyn_FloatType){ goto _LL2016;} else{
goto _LL2017;} _LL2017: if( _temp2005 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL2018;} else{ goto _LL2019;} _LL2019: if( _temp2005 == ( void*) Cyc_Absyn_VoidType){
goto _LL2020;} else{ goto _LL2021;} _LL2021: goto _LL2022; _LL2008:{ void*
_temp2043= t2; struct Cyc_Absyn_PtrInfo _temp2051; struct Cyc_Absyn_Conref*
_temp2053; struct Cyc_Absyn_Tqual _temp2055; struct Cyc_Absyn_Conref* _temp2057;
void* _temp2059; void* _temp2061; _LL2045: if(( unsigned int) _temp2043 >  4u?*((
int*) _temp2043) ==  Cyc_Absyn_PointerType: 0){ _LL2052: _temp2051=(( struct Cyc_Absyn_PointerType_struct*)
_temp2043)->f1; _LL2062: _temp2061=( void*) _temp2051.elt_typ; goto _LL2060;
_LL2060: _temp2059=( void*) _temp2051.rgn_typ; goto _LL2058; _LL2058: _temp2057=
_temp2051.nullable; goto _LL2056; _LL2056: _temp2055= _temp2051.tq; goto _LL2054;
_LL2054: _temp2053= _temp2051.bounds; goto _LL2046;} else{ goto _LL2047;}
_LL2047: if( _temp2043 == ( void*) Cyc_Absyn_VoidType){ goto _LL2048;} else{
goto _LL2049;} _LL2049: goto _LL2050; _LL2046: { struct Cyc_List_List* _temp2063=({
struct Cyc_List_List* _temp2080=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2080->hd=( void*)({ struct _tuple8* _temp2081=(
struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp2081->f1= t1;
_temp2081->f2= t2; _temp2081;}); _temp2080->tl= 0; _temp2080;}); int _temp2064=
Cyc_Tcutil_ptrsubtype( te, _temp2063, _temp2033, _temp2061)? ! _temp2027.q_const?
1: _temp2055.q_const: 0; int bounds_ok= Cyc_Tcutil_unify_conrefs( Cyc_Tcutil_boundscmp,
_temp2025, _temp2053); if( ! bounds_ok){ struct _tuple8 _temp2066=({ struct
_tuple8 _temp2065; _temp2065.f1= Cyc_Absyn_conref_val( _temp2025); _temp2065.f2=
Cyc_Absyn_conref_val( _temp2053); _temp2065;}); void* _temp2072; struct Cyc_Absyn_Exp*
_temp2074; void* _temp2076; struct Cyc_Absyn_Exp* _temp2078; _LL2068: _LL2077:
_temp2076= _temp2066.f1; if(( unsigned int) _temp2076 >  1u?*(( int*) _temp2076)
==  Cyc_Absyn_Upper_b: 0){ _LL2079: _temp2078=(( struct Cyc_Absyn_Upper_b_struct*)
_temp2076)->f1; goto _LL2073;} else{ goto _LL2070;} _LL2073: _temp2072=
_temp2066.f2; if(( unsigned int) _temp2072 >  1u?*(( int*) _temp2072) ==  Cyc_Absyn_Upper_b:
0){ _LL2075: _temp2074=(( struct Cyc_Absyn_Upper_b_struct*) _temp2072)->f1; goto
_LL2069;} else{ goto _LL2070;} _LL2070: goto _LL2071; _LL2069: if( Cyc_Evexp_eval_const_uint_exp(
_temp2078) >=  Cyc_Evexp_eval_const_uint_exp( _temp2074)){ bounds_ok= 1;} goto
_LL2067; _LL2071: bounds_ok= 1; goto _LL2067; _LL2067:;} return( bounds_ok?
_temp2064: 0)? Cyc_Tcutil_unify( _temp2031, _temp2059)? 1: Cyc_Tcenv_region_outlives(
te, _temp2031, _temp2059): 0;} _LL2048: return 1; _LL2050: goto _LL2044; _LL2044:;}
return 0; _LL2010:{ void* _temp2082= t2; struct Cyc_Absyn_Exp* _temp2088; struct
Cyc_Absyn_Tqual _temp2090; void* _temp2092; _LL2084: if(( unsigned int)
_temp2082 >  4u?*(( int*) _temp2082) ==  Cyc_Absyn_ArrayType: 0){ _LL2093:
_temp2092=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2082)->f1; goto
_LL2091; _LL2091: _temp2090=(( struct Cyc_Absyn_ArrayType_struct*) _temp2082)->f2;
goto _LL2089; _LL2089: _temp2088=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2082)->f3; goto _LL2085;} else{ goto _LL2086;} _LL2086: goto _LL2087;
_LL2085: { int okay; okay=( _temp2035 !=  0? _temp2088 !=  0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2035)) >=  Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2088)): 0; return( okay? Cyc_Tcutil_unify(
_temp2039, _temp2092): 0)? ! _temp2037.q_const? 1: _temp2090.q_const: 0;}
_LL2087: return 0; _LL2083:;} return 0; _LL2012:{ void* _temp2094= t2; struct
Cyc_Absyn_Enumdecl* _temp2100; _LL2096: if(( unsigned int) _temp2094 >  4u?*((
int*) _temp2094) ==  Cyc_Absyn_EnumType: 0){ _LL2101: _temp2100=(( struct Cyc_Absyn_EnumType_struct*)
_temp2094)->f2; goto _LL2097;} else{ goto _LL2098;} _LL2098: goto _LL2099;
_LL2097: if(((( struct Cyc_Absyn_Enumdecl*) _check_null( _temp2041))->fields != 
0?(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp2100))->fields !=  0: 0)?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Enumdecl*) _check_null(
_temp2041))->fields))->v) >= (( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Enumdecl*)
_check_null( _temp2100))->fields))->v): 0){ return 1;} goto _LL2095; _LL2099:
goto _LL2095; _LL2095:;} goto _LL2014; _LL2014: goto _LL2016; _LL2016: goto
_LL2018; _LL2018: return Cyc_Tcutil_coerceable( t2)? 1: t2 == ( void*) Cyc_Absyn_VoidType;
_LL2020: return t2 == ( void*) Cyc_Absyn_VoidType; _LL2022: return 0; _LL2006:;}}
void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t){ if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, t)){ struct Cyc_Absyn_Exp* _temp2102= Cyc_Absyn_copy_exp( e);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2103=(
struct Cyc_Absyn_Cast_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp2103[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp2104; _temp2104.tag= Cyc_Absyn_Cast_e;
_temp2104.f1=( void*) t; _temp2104.f2= _temp2102; _temp2104;}); _temp2103;})));
e->topt=({ struct Cyc_Core_Opt* _temp2105=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp2105->v=( void*) t; _temp2105;});}} int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp* e){ void* _temp2106= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v); _LL2108: if(( unsigned int) _temp2106
>  4u?*(( int*) _temp2106) ==  Cyc_Absyn_EnumType: 0){ goto _LL2109;} else{ goto
_LL2110;} _LL2110: if(( unsigned int) _temp2106 >  4u?*(( int*) _temp2106) == 
Cyc_Absyn_IntType: 0){ goto _LL2111;} else{ goto _LL2112;} _LL2112: if((
unsigned int) _temp2106 >  4u?*(( int*) _temp2106) ==  Cyc_Absyn_Evar: 0){ goto
_LL2113;} else{ goto _LL2114;} _LL2114: goto _LL2115; _LL2109: goto _LL2111;
_LL2111: return 1; _LL2113: if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;} return 0; _LL2115:
return 0; _LL2107:;} int Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp* e){ if(
Cyc_Tcutil_is_integral( e)){ return 1;}{ void* _temp2116= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _LL2118: if(
_temp2116 == ( void*) Cyc_Absyn_FloatType){ goto _LL2119;} else{ goto _LL2120;}
_LL2120: if( _temp2116 == ( void*) Cyc_Absyn_DoubleType){ goto _LL2121;} else{
goto _LL2122;} _LL2122: goto _LL2123; _LL2119: goto _LL2121; _LL2121: return 1;
_LL2123: return 0; _LL2117:;}} int Cyc_Tcutil_is_function_type( void* t){ void*
_temp2124= Cyc_Tcutil_compress( t); _LL2126: if(( unsigned int) _temp2124 >  4u?*((
int*) _temp2124) ==  Cyc_Absyn_FnType: 0){ goto _LL2127;} else{ goto _LL2128;}
_LL2128: goto _LL2129; _LL2127: return 1; _LL2129: return 0; _LL2125:;} void*
Cyc_Tcutil_max_arithmetic_type( void* t1, void* t2){ struct _tuple8 _temp2131=({
struct _tuple8 _temp2130; _temp2130.f1= t1; _temp2130.f2= t2; _temp2130;}); void*
_temp2155; void* _temp2157; void* _temp2159; void* _temp2161; void* _temp2163;
void* _temp2165; void* _temp2167; void* _temp2169; void* _temp2171; void*
_temp2173; void* _temp2175; void* _temp2177; void* _temp2179; void* _temp2181;
void* _temp2183; void* _temp2185; void* _temp2187; void* _temp2189; void*
_temp2191; void* _temp2193; void* _temp2195; void* _temp2197; _LL2133: _LL2156:
_temp2155= _temp2131.f1; if( _temp2155 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL2134;} else{ goto _LL2135;} _LL2135: _LL2158: _temp2157= _temp2131.f2; if(
_temp2157 == ( void*) Cyc_Absyn_DoubleType){ goto _LL2136;} else{ goto _LL2137;}
_LL2137: _LL2160: _temp2159= _temp2131.f1; if( _temp2159 == ( void*) Cyc_Absyn_FloatType){
goto _LL2138;} else{ goto _LL2139;} _LL2139: _LL2162: _temp2161= _temp2131.f2;
if( _temp2161 == ( void*) Cyc_Absyn_FloatType){ goto _LL2140;} else{ goto
_LL2141;} _LL2141: _LL2164: _temp2163= _temp2131.f1; if(( unsigned int)
_temp2163 >  4u?*(( int*) _temp2163) ==  Cyc_Absyn_IntType: 0){ _LL2168:
_temp2167=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2163)->f1; if(
_temp2167 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2166;} else{ goto _LL2143;}
_LL2166: _temp2165=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2163)->f2;
if( _temp2165 == ( void*) Cyc_Absyn_B8){ goto _LL2142;} else{ goto _LL2143;}}
else{ goto _LL2143;} _LL2143: _LL2170: _temp2169= _temp2131.f2; if((
unsigned int) _temp2169 >  4u?*(( int*) _temp2169) ==  Cyc_Absyn_IntType: 0){
_LL2174: _temp2173=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2169)->f1;
if( _temp2173 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2172;} else{ goto _LL2145;}
_LL2172: _temp2171=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2169)->f2;
if( _temp2171 == ( void*) Cyc_Absyn_B8){ goto _LL2144;} else{ goto _LL2145;}}
else{ goto _LL2145;} _LL2145: _LL2176: _temp2175= _temp2131.f1; if((
unsigned int) _temp2175 >  4u?*(( int*) _temp2175) ==  Cyc_Absyn_IntType: 0){
_LL2180: _temp2179=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2175)->f1;
if( _temp2179 == ( void*) Cyc_Absyn_Signed){ goto _LL2178;} else{ goto _LL2147;}
_LL2178: _temp2177=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2175)->f2;
if( _temp2177 == ( void*) Cyc_Absyn_B8){ goto _LL2146;} else{ goto _LL2147;}}
else{ goto _LL2147;} _LL2147: _LL2182: _temp2181= _temp2131.f2; if((
unsigned int) _temp2181 >  4u?*(( int*) _temp2181) ==  Cyc_Absyn_IntType: 0){
_LL2186: _temp2185=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2181)->f1;
if( _temp2185 == ( void*) Cyc_Absyn_Signed){ goto _LL2184;} else{ goto _LL2149;}
_LL2184: _temp2183=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2181)->f2;
if( _temp2183 == ( void*) Cyc_Absyn_B8){ goto _LL2148;} else{ goto _LL2149;}}
else{ goto _LL2149;} _LL2149: _LL2188: _temp2187= _temp2131.f1; if((
unsigned int) _temp2187 >  4u?*(( int*) _temp2187) ==  Cyc_Absyn_IntType: 0){
_LL2192: _temp2191=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2187)->f1;
if( _temp2191 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2190;} else{ goto _LL2151;}
_LL2190: _temp2189=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2187)->f2;
if( _temp2189 == ( void*) Cyc_Absyn_B4){ goto _LL2150;} else{ goto _LL2151;}}
else{ goto _LL2151;} _LL2151: _LL2194: _temp2193= _temp2131.f2; if((
unsigned int) _temp2193 >  4u?*(( int*) _temp2193) ==  Cyc_Absyn_IntType: 0){
_LL2198: _temp2197=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2193)->f1;
if( _temp2197 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2196;} else{ goto _LL2153;}
_LL2196: _temp2195=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2193)->f2;
if( _temp2195 == ( void*) Cyc_Absyn_B4){ goto _LL2152;} else{ goto _LL2153;}}
else{ goto _LL2153;} _LL2153: goto _LL2154; _LL2134: goto _LL2136; _LL2136:
return( void*) Cyc_Absyn_DoubleType; _LL2138: goto _LL2140; _LL2140: return(
void*) Cyc_Absyn_FloatType; _LL2142: goto _LL2144; _LL2144: return Cyc_Absyn_ulonglong_t;
_LL2146: goto _LL2148; _LL2148: return Cyc_Absyn_slonglong_t; _LL2150: goto
_LL2152; _LL2152: return Cyc_Absyn_uint_t; _LL2154: return Cyc_Absyn_sint_t;
_LL2132:;} void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp* e){ void*
_temp2199=( void*) e->r; struct Cyc_Core_Opt* _temp2205; _LL2201: if(*(( int*)
_temp2199) ==  Cyc_Absyn_AssignOp_e){ _LL2206: _temp2205=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp2199)->f2; if( _temp2205 ==  0){ goto _LL2202;} else{ goto _LL2203;}} else{
goto _LL2203;} _LL2203: goto _LL2204; _LL2202:({ void* _temp2207[ 0u]={}; Cyc_Tcutil_warn(
e->loc, _tag_arr("assignment in test", sizeof( unsigned char), 19u), _tag_arr(
_temp2207, sizeof( void*), 0u));}); goto _LL2200; _LL2204: goto _LL2200; _LL2200:;}
static int Cyc_Tcutil_constrain_kinds( struct Cyc_Absyn_Conref* c1, struct Cyc_Absyn_Conref*
c2){ c1= Cyc_Absyn_compress_conref( c1); c2= Cyc_Absyn_compress_conref( c2); if(
c1 ==  c2){ return 1;} if(( void*) c1->v == ( void*) Cyc_Absyn_No_constr){( void*)(
c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2208=(
struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct));
_temp2208[ 0]=({ struct Cyc_Absyn_Forward_constr_struct _temp2209; _temp2209.tag=
Cyc_Absyn_Forward_constr; _temp2209.f1= c2; _temp2209;}); _temp2208;}))); return
1;} else{ if(( void*) c2->v == ( void*) Cyc_Absyn_No_constr){( void*)( c2->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2210=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2210[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2211; _temp2211.tag= Cyc_Absyn_Forward_constr;
_temp2211.f1= c1; _temp2211;}); _temp2210;}))); return 1;} else{ void* k1= Cyc_Absyn_conref_val(
c1); void* k2= Cyc_Absyn_conref_val( c2); if( Cyc_Tcutil_kind_leq( k1, k2)){(
void*)( c2->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp2212=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp2212[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp2213; _temp2213.tag= Cyc_Absyn_Forward_constr; _temp2213.f1= c1; _temp2213;});
_temp2212;}))); return 1;} else{ if( Cyc_Tcutil_kind_leq( k2, k1)){( void*)( c1->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2214=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2214[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2215; _temp2215.tag= Cyc_Absyn_Forward_constr;
_temp2215.f1= c2; _temp2215;}); _temp2214;}))); return 1;} else{ return 0;}}}}}
static int Cyc_Tcutil_tvar_id_counter= 0; int* Cyc_Tcutil_new_tvar_id(){ return({
int* _temp2216=( int*) GC_malloc_atomic( sizeof( int)); _temp2216[ 0]= Cyc_Tcutil_tvar_id_counter
++; _temp2216;});} static int Cyc_Tcutil_tvar_counter= 0; struct Cyc_Absyn_Tvar*
Cyc_Tcutil_new_tvar( void* k){ int i= Cyc_Tcutil_tvar_counter ++; struct
_tagged_arr s=( struct _tagged_arr)({ struct Cyc_std_Int_pa_struct _temp2220;
_temp2220.tag= Cyc_std_Int_pa; _temp2220.f1=( int)(( unsigned int) i);{ void*
_temp2219[ 1u]={& _temp2220}; Cyc_std_aprintf( _tag_arr("#%d", sizeof(
unsigned char), 4u), _tag_arr( _temp2219, sizeof( void*), 1u));}}); return({
struct Cyc_Absyn_Tvar* _temp2217=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof(
struct Cyc_Absyn_Tvar)); _temp2217->name=({ struct _tagged_arr* _temp2218=(
struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) *  1); _temp2218[ 0]=
s; _temp2218;}); _temp2217->identity= 0; _temp2217->kind= Cyc_Absyn_new_conref(
k); _temp2217;});} int Cyc_Tcutil_is_temp_tvar( struct Cyc_Absyn_Tvar* t){
struct _tagged_arr _temp2221=* t->name; return*(( const unsigned char*)
_check_unknown_subscript( _temp2221, sizeof( unsigned char), 0)) == '#';} void
Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar* t){ if( ! Cyc_Tcutil_is_temp_tvar(
t)){ return;}{ struct _tagged_arr _temp2222= Cyc_std_strconcat( _tag_arr("`",
sizeof( unsigned char), 2u),* t->name);*(( unsigned char*)
_check_unknown_subscript( _temp2222, sizeof( unsigned char), 1))='t'; t->name=({
struct _tagged_arr* _temp2223=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) *  1); _temp2223[ 0]=( struct _tagged_arr) _temp2222; _temp2223;});}}
struct _tuple13{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; }
; static struct _tuple2* Cyc_Tcutil_fndecl2typ_f( struct _tuple13* x){ return({
struct _tuple2* _temp2224=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2224->f1=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp2225=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2225->v=( void*)(*
x).f1; _temp2225;}); _temp2224->f2=(* x).f2; _temp2224->f3=(* x).f3; _temp2224;});}
void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl* fd){ if( fd->cached_typ == 
0){ struct Cyc_List_List* _temp2226= 0;{ struct Cyc_List_List* atts= fd->attributes;
for( 0; atts !=  0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ if(
Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*) _check_null( atts))->hd)){
_temp2226=({ struct Cyc_List_List* _temp2227=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2227->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); _temp2227->tl= _temp2226; _temp2227;});}}} return(
void*)({ struct Cyc_Absyn_FnType_struct* _temp2228=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2228[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2229; _temp2229.tag= Cyc_Absyn_FnType; _temp2229.f1=({ struct Cyc_Absyn_FnInfo
_temp2230; _temp2230.tvars= fd->tvs; _temp2230.effect= fd->effect; _temp2230.ret_typ=(
void*)(( void*) fd->ret_type); _temp2230.args=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple13*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_fndecl2typ_f, fd->args); _temp2230.c_varargs= fd->c_varargs;
_temp2230.cyc_varargs= fd->cyc_varargs; _temp2230.rgn_po= fd->rgn_po; _temp2230.attributes=
_temp2226; _temp2230;}); _temp2229;}); _temp2228;});} return( void*)(( struct
Cyc_Core_Opt*) _check_null( fd->cached_typ))->v;} struct _tuple14{ void* f1;
struct Cyc_Absyn_Tqual f2; void* f3; } ; static void* Cyc_Tcutil_fst_fdarg(
struct _tuple14* t){ return(* t).f1;} void* Cyc_Tcutil_snd_tqt( struct _tuple4*
t){ return(* t).f2;} static struct _tuple4* Cyc_Tcutil_map2_tq( struct _tuple4*
pr, void* t){ return({ struct _tuple4* _temp2231=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp2231->f1=(* pr).f1; _temp2231->f2= t; _temp2231;});}
struct _tuple15{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; } ; struct
_tuple16{ struct _tuple15* f1; void* f2; } ; static struct _tuple16* Cyc_Tcutil_substitute_f1(
struct _RegionHandle* rgn, struct _tuple2* y){ return({ struct _tuple16*
_temp2232=( struct _tuple16*) _region_malloc( rgn, sizeof( struct _tuple16));
_temp2232->f1=({ struct _tuple15* _temp2233=( struct _tuple15*) _region_malloc(
rgn, sizeof( struct _tuple15)); _temp2233->f1=(* y).f1; _temp2233->f2=(* y).f2;
_temp2233;}); _temp2232->f2=(* y).f3; _temp2232;});} static struct _tuple2* Cyc_Tcutil_substitute_f2(
struct _tuple16* w){ void* _temp2236; struct _tuple15* _temp2238; struct
_tuple16 _temp2234=* w; _LL2239: _temp2238= _temp2234.f1; goto _LL2237; _LL2237:
_temp2236= _temp2234.f2; goto _LL2235; _LL2235: { struct Cyc_Absyn_Tqual
_temp2242; struct Cyc_Core_Opt* _temp2244; struct _tuple15 _temp2240=* _temp2238;
_LL2245: _temp2244= _temp2240.f1; goto _LL2243; _LL2243: _temp2242= _temp2240.f2;
goto _LL2241; _LL2241: return({ struct _tuple2* _temp2246=( struct _tuple2*)
GC_malloc( sizeof( struct _tuple2)); _temp2246->f1= _temp2244; _temp2246->f2=
_temp2242; _temp2246->f3= _temp2236; _temp2246;});}} static void* Cyc_Tcutil_field_type(
struct Cyc_Absyn_Structfield* f){ return( void*) f->type;} static struct Cyc_Absyn_Structfield*
Cyc_Tcutil_zip_field_type( struct Cyc_Absyn_Structfield* f, void* t){ return({
struct Cyc_Absyn_Structfield* _temp2247=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2247->name= f->name;
_temp2247->tq= f->tq; _temp2247->type=( void*) t; _temp2247->width= f->width;
_temp2247->attributes= f->attributes; _temp2247;});} static struct Cyc_List_List*
Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List* inst, struct
Cyc_List_List* ts); void* Cyc_Tcutil_rsubstitute( struct _RegionHandle* rgn,
struct Cyc_List_List* inst, void* t){ void* _temp2248= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp2296; struct Cyc_Absyn_Structdecl** _temp2298;
struct Cyc_List_List* _temp2300; struct _tuple1* _temp2302; struct Cyc_Absyn_Uniondecl**
_temp2304; struct Cyc_List_List* _temp2306; struct _tuple1* _temp2308; struct
Cyc_Absyn_TunionInfo _temp2310; void* _temp2312; struct Cyc_List_List* _temp2314;
void* _temp2316; struct Cyc_Absyn_TunionFieldInfo _temp2318; struct Cyc_List_List*
_temp2320; void* _temp2322; struct Cyc_Core_Opt* _temp2324; struct Cyc_List_List*
_temp2326; struct _tuple1* _temp2328; struct Cyc_Absyn_Exp* _temp2330; struct
Cyc_Absyn_Tqual _temp2332; void* _temp2334; struct Cyc_Absyn_PtrInfo _temp2336;
struct Cyc_Absyn_Conref* _temp2338; struct Cyc_Absyn_Tqual _temp2340; struct Cyc_Absyn_Conref*
_temp2342; void* _temp2344; void* _temp2346; struct Cyc_Absyn_FnInfo _temp2348;
struct Cyc_List_List* _temp2350; struct Cyc_List_List* _temp2352; struct Cyc_Absyn_VarargInfo*
_temp2354; int _temp2356; struct Cyc_List_List* _temp2358; void* _temp2360;
struct Cyc_Core_Opt* _temp2362; struct Cyc_List_List* _temp2364; struct Cyc_List_List*
_temp2366; struct Cyc_List_List* _temp2368; struct Cyc_List_List* _temp2370;
struct Cyc_Core_Opt* _temp2372; void* _temp2374; void* _temp2376; void*
_temp2378; struct Cyc_List_List* _temp2380; _LL2250: if(( unsigned int)
_temp2248 >  4u?*(( int*) _temp2248) ==  Cyc_Absyn_VarType: 0){ _LL2297:
_temp2296=(( struct Cyc_Absyn_VarType_struct*) _temp2248)->f1; goto _LL2251;}
else{ goto _LL2252;} _LL2252: if(( unsigned int) _temp2248 >  4u?*(( int*)
_temp2248) ==  Cyc_Absyn_StructType: 0){ _LL2303: _temp2302=(( struct Cyc_Absyn_StructType_struct*)
_temp2248)->f1; goto _LL2301; _LL2301: _temp2300=(( struct Cyc_Absyn_StructType_struct*)
_temp2248)->f2; goto _LL2299; _LL2299: _temp2298=(( struct Cyc_Absyn_StructType_struct*)
_temp2248)->f3; goto _LL2253;} else{ goto _LL2254;} _LL2254: if(( unsigned int)
_temp2248 >  4u?*(( int*) _temp2248) ==  Cyc_Absyn_UnionType: 0){ _LL2309:
_temp2308=(( struct Cyc_Absyn_UnionType_struct*) _temp2248)->f1; goto _LL2307;
_LL2307: _temp2306=(( struct Cyc_Absyn_UnionType_struct*) _temp2248)->f2; goto
_LL2305; _LL2305: _temp2304=(( struct Cyc_Absyn_UnionType_struct*) _temp2248)->f3;
goto _LL2255;} else{ goto _LL2256;} _LL2256: if(( unsigned int) _temp2248 >  4u?*((
int*) _temp2248) ==  Cyc_Absyn_TunionType: 0){ _LL2311: _temp2310=(( struct Cyc_Absyn_TunionType_struct*)
_temp2248)->f1; _LL2317: _temp2316=( void*) _temp2310.tunion_info; goto _LL2315;
_LL2315: _temp2314= _temp2310.targs; goto _LL2313; _LL2313: _temp2312=( void*)
_temp2310.rgn; goto _LL2257;} else{ goto _LL2258;} _LL2258: if(( unsigned int)
_temp2248 >  4u?*(( int*) _temp2248) ==  Cyc_Absyn_TunionFieldType: 0){ _LL2319:
_temp2318=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2248)->f1; _LL2323:
_temp2322=( void*) _temp2318.field_info; goto _LL2321; _LL2321: _temp2320=
_temp2318.targs; goto _LL2259;} else{ goto _LL2260;} _LL2260: if(( unsigned int)
_temp2248 >  4u?*(( int*) _temp2248) ==  Cyc_Absyn_TypedefType: 0){ _LL2329:
_temp2328=(( struct Cyc_Absyn_TypedefType_struct*) _temp2248)->f1; goto _LL2327;
_LL2327: _temp2326=(( struct Cyc_Absyn_TypedefType_struct*) _temp2248)->f2; goto
_LL2325; _LL2325: _temp2324=(( struct Cyc_Absyn_TypedefType_struct*) _temp2248)->f3;
goto _LL2261;} else{ goto _LL2262;} _LL2262: if(( unsigned int) _temp2248 >  4u?*((
int*) _temp2248) ==  Cyc_Absyn_ArrayType: 0){ _LL2335: _temp2334=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2248)->f1; goto _LL2333; _LL2333:
_temp2332=(( struct Cyc_Absyn_ArrayType_struct*) _temp2248)->f2; goto _LL2331;
_LL2331: _temp2330=(( struct Cyc_Absyn_ArrayType_struct*) _temp2248)->f3; goto
_LL2263;} else{ goto _LL2264;} _LL2264: if(( unsigned int) _temp2248 >  4u?*((
int*) _temp2248) ==  Cyc_Absyn_PointerType: 0){ _LL2337: _temp2336=(( struct Cyc_Absyn_PointerType_struct*)
_temp2248)->f1; _LL2347: _temp2346=( void*) _temp2336.elt_typ; goto _LL2345;
_LL2345: _temp2344=( void*) _temp2336.rgn_typ; goto _LL2343; _LL2343: _temp2342=
_temp2336.nullable; goto _LL2341; _LL2341: _temp2340= _temp2336.tq; goto _LL2339;
_LL2339: _temp2338= _temp2336.bounds; goto _LL2265;} else{ goto _LL2266;}
_LL2266: if(( unsigned int) _temp2248 >  4u?*(( int*) _temp2248) ==  Cyc_Absyn_FnType:
0){ _LL2349: _temp2348=(( struct Cyc_Absyn_FnType_struct*) _temp2248)->f1;
_LL2365: _temp2364= _temp2348.tvars; goto _LL2363; _LL2363: _temp2362= _temp2348.effect;
goto _LL2361; _LL2361: _temp2360=( void*) _temp2348.ret_typ; goto _LL2359;
_LL2359: _temp2358= _temp2348.args; goto _LL2357; _LL2357: _temp2356= _temp2348.c_varargs;
goto _LL2355; _LL2355: _temp2354= _temp2348.cyc_varargs; goto _LL2353; _LL2353:
_temp2352= _temp2348.rgn_po; goto _LL2351; _LL2351: _temp2350= _temp2348.attributes;
goto _LL2267;} else{ goto _LL2268;} _LL2268: if(( unsigned int) _temp2248 >  4u?*((
int*) _temp2248) ==  Cyc_Absyn_TupleType: 0){ _LL2367: _temp2366=(( struct Cyc_Absyn_TupleType_struct*)
_temp2248)->f1; goto _LL2269;} else{ goto _LL2270;} _LL2270: if(( unsigned int)
_temp2248 >  4u?*(( int*) _temp2248) ==  Cyc_Absyn_AnonStructType: 0){ _LL2369:
_temp2368=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2248)->f1; goto
_LL2271;} else{ goto _LL2272;} _LL2272: if(( unsigned int) _temp2248 >  4u?*((
int*) _temp2248) ==  Cyc_Absyn_AnonUnionType: 0){ _LL2371: _temp2370=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp2248)->f1; goto _LL2273;} else{ goto
_LL2274;} _LL2274: if(( unsigned int) _temp2248 >  4u?*(( int*) _temp2248) == 
Cyc_Absyn_Evar: 0){ _LL2373: _temp2372=(( struct Cyc_Absyn_Evar_struct*)
_temp2248)->f2; goto _LL2275;} else{ goto _LL2276;} _LL2276: if(( unsigned int)
_temp2248 >  4u?*(( int*) _temp2248) ==  Cyc_Absyn_RgnHandleType: 0){ _LL2375:
_temp2374=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp2248)->f1;
goto _LL2277;} else{ goto _LL2278;} _LL2278: if(( unsigned int) _temp2248 >  4u?*((
int*) _temp2248) ==  Cyc_Absyn_EnumType: 0){ goto _LL2279;} else{ goto _LL2280;}
_LL2280: if( _temp2248 == ( void*) Cyc_Absyn_VoidType){ goto _LL2281;} else{
goto _LL2282;} _LL2282: if(( unsigned int) _temp2248 >  4u?*(( int*) _temp2248)
==  Cyc_Absyn_IntType: 0){ goto _LL2283;} else{ goto _LL2284;} _LL2284: if(
_temp2248 == ( void*) Cyc_Absyn_FloatType){ goto _LL2285;} else{ goto _LL2286;}
_LL2286: if( _temp2248 == ( void*) Cyc_Absyn_DoubleType){ goto _LL2287;} else{
goto _LL2288;} _LL2288: if( _temp2248 == ( void*) Cyc_Absyn_HeapRgn){ goto
_LL2289;} else{ goto _LL2290;} _LL2290: if(( unsigned int) _temp2248 >  4u?*((
int*) _temp2248) ==  Cyc_Absyn_RgnsEff: 0){ _LL2377: _temp2376=( void*)(( struct
Cyc_Absyn_RgnsEff_struct*) _temp2248)->f1; goto _LL2291;} else{ goto _LL2292;}
_LL2292: if(( unsigned int) _temp2248 >  4u?*(( int*) _temp2248) ==  Cyc_Absyn_AccessEff:
0){ _LL2379: _temp2378=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp2248)->f1;
goto _LL2293;} else{ goto _LL2294;} _LL2294: if(( unsigned int) _temp2248 >  4u?*((
int*) _temp2248) ==  Cyc_Absyn_JoinEff: 0){ _LL2381: _temp2380=(( struct Cyc_Absyn_JoinEff_struct*)
_temp2248)->f1; goto _LL2295;} else{ goto _LL2249;} _LL2251: { struct
_handler_cons _temp2382; _push_handler(& _temp2382);{ int _temp2384= 0; if(
setjmp( _temp2382.handler)){ _temp2384= 1;} if( ! _temp2384){{ void* _temp2385=((
void*(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp,
inst, _temp2296); _npop_handler( 0u); return _temp2385;}; _pop_handler();} else{
void* _temp2383=( void*) _exn_thrown; void* _temp2387= _temp2383; _LL2389: if(
_temp2387 ==  Cyc_Core_Not_found){ goto _LL2390;} else{ goto _LL2391;} _LL2391:
goto _LL2392; _LL2390: return t; _LL2392:( void) _throw( _temp2387); _LL2388:;}}}
_LL2253: { struct Cyc_List_List* _temp2393= Cyc_Tcutil_substs( rgn, inst,
_temp2300); return _temp2393 ==  _temp2300? t:( void*)({ struct Cyc_Absyn_StructType_struct*
_temp2394=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp2394[ 0]=({ struct Cyc_Absyn_StructType_struct _temp2395; _temp2395.tag=
Cyc_Absyn_StructType; _temp2395.f1= _temp2302; _temp2395.f2= _temp2393;
_temp2395.f3= _temp2298; _temp2395;}); _temp2394;});} _LL2255: { struct Cyc_List_List*
_temp2396= Cyc_Tcutil_substs( rgn, inst, _temp2306); return _temp2396 == 
_temp2306? t:( void*)({ struct Cyc_Absyn_UnionType_struct* _temp2397=( struct
Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp2397[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp2398; _temp2398.tag= Cyc_Absyn_UnionType;
_temp2398.f1= _temp2308; _temp2398.f2= _temp2396; _temp2398.f3= _temp2304;
_temp2398;}); _temp2397;});} _LL2257: { struct Cyc_List_List* _temp2399= Cyc_Tcutil_substs(
rgn, inst, _temp2314); void* _temp2400= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2312); return( _temp2399 ==  _temp2314? _temp2400 ==  _temp2312: 0)? t:(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp2401=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2401[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2402; _temp2402.tag= Cyc_Absyn_TunionType;
_temp2402.f1=({ struct Cyc_Absyn_TunionInfo _temp2403; _temp2403.tunion_info=(
void*) _temp2316; _temp2403.targs= _temp2399; _temp2403.rgn=( void*) _temp2400;
_temp2403;}); _temp2402;}); _temp2401;});} _LL2259: { struct Cyc_List_List*
_temp2404= Cyc_Tcutil_substs( rgn, inst, _temp2320); return _temp2404 == 
_temp2320? t:( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp2405=(
struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp2405[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp2406; _temp2406.tag=
Cyc_Absyn_TunionFieldType; _temp2406.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2407; _temp2407.field_info=( void*) _temp2322; _temp2407.targs= _temp2404;
_temp2407;}); _temp2406;}); _temp2405;});} _LL2261: { struct Cyc_List_List*
_temp2408= Cyc_Tcutil_substs( rgn, inst, _temp2326); return _temp2408 == 
_temp2326? t:( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp2409=( struct
Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2409[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp2410; _temp2410.tag=
Cyc_Absyn_TypedefType; _temp2410.f1= _temp2328; _temp2410.f2= _temp2408;
_temp2410.f3= _temp2324; _temp2410;}); _temp2409;});} _LL2263: { void* _temp2411=
Cyc_Tcutil_rsubstitute( rgn, inst, _temp2334); return _temp2411 ==  _temp2334? t:(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp2412=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2412[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2413; _temp2413.tag= Cyc_Absyn_ArrayType;
_temp2413.f1=( void*) _temp2411; _temp2413.f2= _temp2332; _temp2413.f3=
_temp2330; _temp2413;}); _temp2412;});} _LL2265: { void* _temp2414= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2346); void* _temp2415= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2344); if( _temp2414 ==  _temp2346? _temp2415 ==  _temp2344: 0){ return t;}
return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp2416=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp2416[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp2417; _temp2417.tag= Cyc_Absyn_PointerType;
_temp2417.f1=({ struct Cyc_Absyn_PtrInfo _temp2418; _temp2418.elt_typ=( void*)
_temp2414; _temp2418.rgn_typ=( void*) _temp2415; _temp2418.nullable= _temp2342;
_temp2418.tq= _temp2340; _temp2418.bounds= _temp2338; _temp2418;}); _temp2417;});
_temp2416;});} _LL2267:{ struct Cyc_List_List* _temp2419= _temp2364; for( 0;
_temp2419 !=  0; _temp2419=(( struct Cyc_List_List*) _check_null( _temp2419))->tl){
inst=({ struct Cyc_List_List* _temp2420=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2420->hd=( void*)({ struct _tuple5*
_temp2421=( struct _tuple5*) _region_malloc( rgn, sizeof( struct _tuple5));
_temp2421->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp2419))->hd; _temp2421->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2422=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2422[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2423; _temp2423.tag= Cyc_Absyn_VarType;
_temp2423.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp2419))->hd; _temp2423;}); _temp2422;}); _temp2421;}); _temp2420->tl= inst;
_temp2420;});}}{ struct Cyc_List_List* _temp2426; struct Cyc_List_List*
_temp2428; struct _tuple0 _temp2424=(( struct _tuple0(*)( struct _RegionHandle*
r3, struct _RegionHandle* r4, struct Cyc_List_List* x)) Cyc_List_rsplit)( rgn,
rgn,(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple16*(* f)(
struct _RegionHandle*, struct _tuple2*), struct _RegionHandle* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_substitute_f1, rgn, _temp2358)); _LL2429:
_temp2428= _temp2424.f1; goto _LL2427; _LL2427: _temp2426= _temp2424.f2; goto
_LL2425; _LL2425: { struct Cyc_List_List* _temp2430= Cyc_Tcutil_substs( rgn,
inst, _temp2426); struct Cyc_List_List* _temp2431=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple16*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_substitute_f2,(( struct Cyc_List_List*(*)( struct _RegionHandle* r3,
struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp2428, _temp2430)); struct Cyc_Core_Opt* eff2; if( _temp2362 ==  0){
eff2= 0;} else{ void* _temp2432= Cyc_Tcutil_rsubstitute( rgn, inst,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2362))->v); if( _temp2432 == ( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2362))->v){ eff2= _temp2362;} else{ eff2=({
struct Cyc_Core_Opt* _temp2433=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2433->v=( void*) _temp2432; _temp2433;});}}{ struct Cyc_Absyn_VarargInfo*
cyc_varargs2; if( _temp2354 ==  0){ cyc_varargs2= 0;} else{ int _temp2436; void*
_temp2438; void* _temp2440; struct Cyc_Absyn_Tqual _temp2442; struct Cyc_Core_Opt*
_temp2444; struct Cyc_Absyn_VarargInfo _temp2434=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp2354)); _LL2445: _temp2444= _temp2434.name; goto _LL2443;
_LL2443: _temp2442= _temp2434.tq; goto _LL2441; _LL2441: _temp2440=( void*)
_temp2434.type; goto _LL2439; _LL2439: _temp2438=( void*) _temp2434.rgn; goto
_LL2437; _LL2437: _temp2436= _temp2434.inject; goto _LL2435; _LL2435: { void*
_temp2446= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2440); void* _temp2447= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2438); if( _temp2446 ==  _temp2440? _temp2438 ==  _temp2447: 0){
cyc_varargs2= _temp2354;} else{ cyc_varargs2=({ struct Cyc_Absyn_VarargInfo*
_temp2448=( struct Cyc_Absyn_VarargInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2448->name= _temp2444; _temp2448->tq= _temp2442; _temp2448->type=( void*)
_temp2446; _temp2448->rgn=( void*) _temp2447; _temp2448->inject= _temp2436;
_temp2448;});}}}{ struct Cyc_List_List* rgn_po2; struct Cyc_List_List* _temp2451;
struct Cyc_List_List* _temp2453; struct _tuple0 _temp2449= Cyc_List_rsplit( rgn,
rgn, _temp2352); _LL2454: _temp2453= _temp2449.f1; goto _LL2452; _LL2452:
_temp2451= _temp2449.f2; goto _LL2450; _LL2450: { struct Cyc_List_List*
_temp2455= Cyc_Tcutil_substs( rgn, inst, _temp2453); struct Cyc_List_List*
_temp2456= Cyc_Tcutil_substs( rgn, inst, _temp2451); if( _temp2455 ==  _temp2453?
_temp2456 ==  _temp2451: 0){ rgn_po2= _temp2352;} else{ rgn_po2= Cyc_List_zip(
_temp2455, _temp2456);} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2457=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2457[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2458; _temp2458.tag= Cyc_Absyn_FnType;
_temp2458.f1=({ struct Cyc_Absyn_FnInfo _temp2459; _temp2459.tvars= _temp2364;
_temp2459.effect= eff2; _temp2459.ret_typ=( void*) Cyc_Tcutil_rsubstitute( rgn,
inst, _temp2360); _temp2459.args= _temp2431; _temp2459.c_varargs= _temp2356;
_temp2459.cyc_varargs= cyc_varargs2; _temp2459.rgn_po= rgn_po2; _temp2459.attributes=
_temp2350; _temp2459;}); _temp2458;}); _temp2457;});}}}}} _LL2269: { struct Cyc_List_List*
_temp2460=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_snd_tqt,
_temp2366); struct Cyc_List_List* _temp2461= Cyc_Tcutil_substs( rgn, inst,
_temp2460); if( _temp2461 ==  _temp2460){ return t;}{ struct Cyc_List_List*
_temp2462=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*,
void*), struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_map2)( Cyc_Tcutil_map2_tq,
_temp2366, _temp2461); return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp2463=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp2463[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp2464; _temp2464.tag= Cyc_Absyn_TupleType;
_temp2464.f1= _temp2462; _temp2464;}); _temp2463;});}} _LL2271: { struct Cyc_List_List*
_temp2465=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2368); struct Cyc_List_List* _temp2466= Cyc_Tcutil_substs( rgn, inst,
_temp2465); if( _temp2466 ==  _temp2465){ return t;}{ struct Cyc_List_List*
_temp2467=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2368, _temp2466); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp2468=( struct Cyc_Absyn_AnonStructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2468[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2469; _temp2469.tag= Cyc_Absyn_AnonStructType;
_temp2469.f1= _temp2467; _temp2469;}); _temp2468;});}} _LL2273: { struct Cyc_List_List*
_temp2470=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2370); struct Cyc_List_List* _temp2471= Cyc_Tcutil_substs( rgn, inst,
_temp2470); if( _temp2471 ==  _temp2470){ return t;}{ struct Cyc_List_List*
_temp2472=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2370, _temp2471); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp2473=( struct Cyc_Absyn_AnonStructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2473[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2474; _temp2474.tag= Cyc_Absyn_AnonStructType;
_temp2474.f1= _temp2472; _temp2474;}); _temp2473;});}} _LL2275: if( _temp2372 != 
0){ return Cyc_Tcutil_rsubstitute( rgn, inst,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2372))->v);} else{ return t;} _LL2277: { void* _temp2475= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2374); return _temp2475 ==  _temp2374? t:( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp2476=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp2476[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp2477; _temp2477.tag= Cyc_Absyn_RgnHandleType; _temp2477.f1=( void*)
_temp2475; _temp2477;}); _temp2476;});} _LL2279: return t; _LL2281: return t;
_LL2283: return t; _LL2285: return t; _LL2287: return t; _LL2289: return t;
_LL2291: { void* _temp2478= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2376);
return _temp2478 ==  _temp2376? t:( void*)({ struct Cyc_Absyn_RgnsEff_struct*
_temp2479=( struct Cyc_Absyn_RgnsEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnsEff_struct));
_temp2479[ 0]=({ struct Cyc_Absyn_RgnsEff_struct _temp2480; _temp2480.tag= Cyc_Absyn_RgnsEff;
_temp2480.f1=( void*) _temp2478; _temp2480;}); _temp2479;});} _LL2293: { void*
_temp2481= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2378); return _temp2481 == 
_temp2378? t:( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2482=( struct
Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2482[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2483; _temp2483.tag= Cyc_Absyn_AccessEff;
_temp2483.f1=( void*) _temp2481; _temp2483;}); _temp2482;});} _LL2295: { struct
Cyc_List_List* _temp2484= Cyc_Tcutil_substs( rgn, inst, _temp2380); return
_temp2484 ==  _temp2380? t:( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2485=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2485[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2486; _temp2486.tag= Cyc_Absyn_JoinEff;
_temp2486.f1= _temp2484; _temp2486;}); _temp2485;});} _LL2249:;} static struct
Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List*
inst, struct Cyc_List_List* ts){ if( ts ==  0){ return 0;}{ void* _temp2487=(
void*)(( struct Cyc_List_List*) _check_null( ts))->hd; struct Cyc_List_List*
_temp2488=(( struct Cyc_List_List*) _check_null( ts))->tl; void* _temp2489= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2487); struct Cyc_List_List* _temp2490= Cyc_Tcutil_substs( rgn,
inst, _temp2488); if( _temp2487 ==  _temp2489? _temp2488 ==  _temp2490: 0){
return ts;} return( struct Cyc_List_List*)({ struct Cyc_List_List* _temp2491=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2491->hd=(
void*) _temp2489; _temp2491->tl= _temp2490; _temp2491;});}} extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List* inst, void* t){ return Cyc_Tcutil_rsubstitute( Cyc_Core_heap_region,
inst, t);} struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2536=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2536[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2537; _temp2537.tag= Cyc_Absyn_Const_e; _temp2537.f1=(
void*)(( void*) Cyc_Absyn_Null_c); _temp2537;}); _temp2536;}), loc);{ void*
_temp2492= Cyc_Tcutil_compress( t); void* _temp2506; void* _temp2508; void*
_temp2510; void* _temp2512; _LL2494: if(( unsigned int) _temp2492 >  4u?*(( int*)
_temp2492) ==  Cyc_Absyn_PointerType: 0){ goto _LL2495;} else{ goto _LL2496;}
_LL2496: if(( unsigned int) _temp2492 >  4u?*(( int*) _temp2492) ==  Cyc_Absyn_IntType:
0){ _LL2509: _temp2508=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2492)->f1;
goto _LL2507; _LL2507: _temp2506=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2492)->f2; if( _temp2506 == ( void*) Cyc_Absyn_B1){ goto _LL2497;} else{
goto _LL2498;}} else{ goto _LL2498;} _LL2498: if(( unsigned int) _temp2492 >  4u?*((
int*) _temp2492) ==  Cyc_Absyn_IntType: 0){ _LL2513: _temp2512=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2492)->f1; goto _LL2511; _LL2511: _temp2510=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2492)->f2; goto _LL2499;} else{
goto _LL2500;} _LL2500: if( _temp2492 == ( void*) Cyc_Absyn_FloatType){ goto
_LL2501;} else{ goto _LL2502;} _LL2502: if( _temp2492 == ( void*) Cyc_Absyn_DoubleType){
goto _LL2503;} else{ goto _LL2504;} _LL2504: goto _LL2505; _LL2495: goto _LL2493;
_LL2497:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2514=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2514[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2515; _temp2515.tag= Cyc_Absyn_Const_e;
_temp2515.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp2516=(
struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp2516[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp2517; _temp2517.tag= Cyc_Absyn_Char_c;
_temp2517.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp2517.f2='\000'; _temp2517;});
_temp2516;})); _temp2515;}); _temp2514;}))); goto _LL2493; _LL2499:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2518=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2518[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2519; _temp2519.tag= Cyc_Absyn_Const_e; _temp2519.f1=(
void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp2520=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp2520[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp2521; _temp2521.tag= Cyc_Absyn_Int_c; _temp2521.f1=( void*) _temp2512;
_temp2521.f2= 0; _temp2521;}); _temp2520;})); _temp2519;}); _temp2518;}))); if(
_temp2510 != ( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp2522=( struct Cyc_Absyn_Cast_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp2522[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp2523; _temp2523.tag= Cyc_Absyn_Cast_e;
_temp2523.f1=( void*) t; _temp2523.f2= e; _temp2523;}); _temp2522;}), loc);}
goto _LL2493; _LL2501:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2524=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2524[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2525; _temp2525.tag= Cyc_Absyn_Const_e;
_temp2525.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2526=(
struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2526[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2527; _temp2527.tag= Cyc_Absyn_Float_c;
_temp2527.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp2527;});
_temp2526;})); _temp2525;}); _temp2524;}))); goto _LL2493; _LL2503:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2528=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2528[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2529; _temp2529.tag= Cyc_Absyn_Cast_e; _temp2529.f1=( void*) t; _temp2529.f2=
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2530=( struct
Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2530[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2531; _temp2531.tag= Cyc_Absyn_Const_e;
_temp2531.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2532=(
struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2532[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2533; _temp2533.tag= Cyc_Absyn_Float_c;
_temp2533.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp2533;});
_temp2532;})); _temp2531;}); _temp2530;}), loc); _temp2529;}); _temp2528;})));
goto _LL2493; _LL2505:({ struct Cyc_std_String_pa_struct _temp2535; _temp2535.tag=
Cyc_std_String_pa; _temp2535.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
void* _temp2534[ 1u]={& _temp2535}; Cyc_Tcutil_terr( loc, _tag_arr("declaration of type %s requires initializer",
sizeof( unsigned char), 44u), _tag_arr( _temp2534, sizeof( void*), 1u));}});
goto _LL2493; _LL2493:;} return e;} struct _tuple5* Cyc_Tcutil_make_inst_var(
struct Cyc_List_List* s, struct Cyc_Absyn_Tvar* tv){ void* k= Cyc_Absyn_conref_val(
tv->kind); return({ struct _tuple5* _temp2538=( struct _tuple5*) GC_malloc(
sizeof( struct _tuple5)); _temp2538->f1= tv; _temp2538->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp2539=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2539->v=( void*) k; _temp2539;}),({ struct Cyc_Core_Opt*
_temp2540=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2540->v=( void*) s; _temp2540;})); _temp2538;});} struct _tuple5* Cyc_Tcutil_r_make_inst_var(
struct _tuple6* env, struct Cyc_Absyn_Tvar* tv){ struct _tuple6 _temp2543;
struct _RegionHandle* _temp2544; struct Cyc_List_List* _temp2546; struct _tuple6*
_temp2541= env; _temp2543=* _temp2541; _LL2547: _temp2546= _temp2543.f1; goto
_LL2545; _LL2545: _temp2544= _temp2543.f2; goto _LL2542; _LL2542: { void* k= Cyc_Absyn_conref_val(
tv->kind); return({ struct _tuple5* _temp2548=( struct _tuple5*) _region_malloc(
_temp2544, sizeof( struct _tuple5)); _temp2548->f1= tv; _temp2548->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp2549=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2549->v=( void*) k; _temp2549;}),({ struct Cyc_Core_Opt*
_temp2550=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2550->v=( void*) _temp2546; _temp2550;})); _temp2548;});}} static struct
Cyc_List_List* Cyc_Tcutil_add_free_tvar( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 !=  0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
if( Cyc_std_zstrptrcmp((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->name, tv->name) ==  0){ struct Cyc_Absyn_Conref* k1=((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->kind;
struct Cyc_Absyn_Conref* k2= tv->kind; if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){({
struct Cyc_std_String_pa_struct _temp2554; _temp2554.tag= Cyc_std_String_pa;
_temp2554.f1=( struct _tagged_arr) Cyc_Absynpp_ckind2string( k2);{ struct Cyc_std_String_pa_struct
_temp2553; _temp2553.tag= Cyc_std_String_pa; _temp2553.f1=( struct _tagged_arr)
Cyc_Absynpp_ckind2string( k1);{ struct Cyc_std_String_pa_struct _temp2552;
_temp2552.tag= Cyc_std_String_pa; _temp2552.f1=( struct _tagged_arr)* tv->name;{
void* _temp2551[ 3u]={& _temp2552,& _temp2553,& _temp2554}; Cyc_Tcutil_terr( loc,
_tag_arr("type variable %s is used with inconsistent kinds %s and %s", sizeof(
unsigned char), 59u), _tag_arr( _temp2551, sizeof( void*), 3u));}}}});} if( tv->identity
==  0){ tv->identity=(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity;} else{ if(*(( int*) _check_null( tv->identity))
!= *(( int*) _check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity))){({ void* _temp2555[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("same type variable has different identity!",
sizeof( unsigned char), 43u), _tag_arr( _temp2555, sizeof( void*), 0u));});}}
return tvs;}}} tv->identity= Cyc_Tcutil_new_tvar_id(); return({ struct Cyc_List_List*
_temp2556=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2556->hd=( void*) tv; _temp2556->tl= tvs; _temp2556;});} static struct Cyc_List_List*
Cyc_Tcutil_fast_add_free_tvar( struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar*
tv){ if( tv->identity ==  0){({ void* _temp2557[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("fast_add_free_tvar: bad identity in tv",
sizeof( unsigned char), 39u), _tag_arr( _temp2557, sizeof( void*), 0u));});}{
struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 !=  0; tvs2=(( struct Cyc_List_List*)
_check_null( tvs2))->tl){ if((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity ==  0){({ void* _temp2558[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("fast_add_free_tvar: bad identity in tvs2",
sizeof( unsigned char), 41u), _tag_arr( _temp2558, sizeof( void*), 0u));});} if(*((
int*) _check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity)) == *(( int*) _check_null( tv->identity))){
return tvs;}}} return({ struct Cyc_List_List* _temp2559=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2559->hd=( void*) tv; _temp2559->tl=
tvs; _temp2559;});} static struct Cyc_List_List* Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){ if( tv->identity ==  0){({
struct Cyc_std_String_pa_struct _temp2561; _temp2561.tag= Cyc_std_String_pa;
_temp2561.f1=( struct _tagged_arr) Cyc_Tcutil_tvar2string( tv);{ void* _temp2560[
1u]={& _temp2561};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("bound tvar id for %s is NULL", sizeof( unsigned char), 29u), _tag_arr(
_temp2560, sizeof( void*), 1u));}});} return({ struct Cyc_List_List* _temp2562=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2562->hd=(
void*) tv; _temp2562->tl= tvs; _temp2562;});} static struct Cyc_List_List* Cyc_Tcutil_add_free_evar(
struct Cyc_List_List* es, void* e){ void* _temp2563= Cyc_Tcutil_compress( e);
int _temp2569; _LL2565: if(( unsigned int) _temp2563 >  4u?*(( int*) _temp2563)
==  Cyc_Absyn_Evar: 0){ _LL2570: _temp2569=(( struct Cyc_Absyn_Evar_struct*)
_temp2563)->f3; goto _LL2566;} else{ goto _LL2567;} _LL2567: goto _LL2568;
_LL2566:{ struct Cyc_List_List* es2= es; for( 0; es2 !=  0; es2=(( struct Cyc_List_List*)
_check_null( es2))->tl){ void* _temp2571= Cyc_Tcutil_compress(( void*)(( struct
Cyc_List_List*) _check_null( es2))->hd); int _temp2577; _LL2573: if((
unsigned int) _temp2571 >  4u?*(( int*) _temp2571) ==  Cyc_Absyn_Evar: 0){
_LL2578: _temp2577=(( struct Cyc_Absyn_Evar_struct*) _temp2571)->f3; goto
_LL2574;} else{ goto _LL2575;} _LL2575: goto _LL2576; _LL2574: if( _temp2569 == 
_temp2577){ return es;} goto _LL2572; _LL2576: goto _LL2572; _LL2572:;}} return({
struct Cyc_List_List* _temp2579=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2579->hd=( void*) e; _temp2579->tl= es; _temp2579;});
_LL2568: return es; _LL2564:;} static struct Cyc_List_List* Cyc_Tcutil_remove_bound_tvars(
struct Cyc_List_List* tvs, struct Cyc_List_List* btvs){ struct Cyc_List_List* r=
0; for( 0; tvs !=  0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ int
present= 0;{ struct Cyc_List_List* b= btvs; for( 0; b !=  0; b=(( struct Cyc_List_List*)
_check_null( b))->tl){ if(*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->identity)) == *(( int*)
_check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd)->identity))){
present= 1; break;}}} if( ! present){ r=({ struct Cyc_List_List* _temp2580=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2580->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd);
_temp2580->tl= r; _temp2580;});}} r=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( r); return r;} void Cyc_Tcutil_check_bitfield( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, void* field_typ, struct Cyc_Absyn_Exp* width,
struct _tagged_arr* fn){ if( width !=  0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*) _check_null( width))){({ struct Cyc_std_String_pa_struct
_temp2582; _temp2582.tag= Cyc_std_String_pa; _temp2582.f1=( struct _tagged_arr)*
fn;{ void* _temp2581[ 1u]={& _temp2582}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield %s does not have constant width",
sizeof( unsigned char), 41u), _tag_arr( _temp2581, sizeof( void*), 1u));}});}
else{ w= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*) _check_null(
width));}{ void* _temp2583= Cyc_Tcutil_compress( field_typ); void* _temp2589;
_LL2585: if(( unsigned int) _temp2583 >  4u?*(( int*) _temp2583) ==  Cyc_Absyn_IntType:
0){ _LL2590: _temp2589=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2583)->f2;
goto _LL2586;} else{ goto _LL2587;} _LL2587: goto _LL2588; _LL2586:{ void*
_temp2591= _temp2589; _LL2593: if( _temp2591 == ( void*) Cyc_Absyn_B1){ goto
_LL2594;} else{ goto _LL2595;} _LL2595: if( _temp2591 == ( void*) Cyc_Absyn_B2){
goto _LL2596;} else{ goto _LL2597;} _LL2597: if( _temp2591 == ( void*) Cyc_Absyn_B4){
goto _LL2598;} else{ goto _LL2599;} _LL2599: if( _temp2591 == ( void*) Cyc_Absyn_B8){
goto _LL2600;} else{ goto _LL2592;} _LL2594: if( w >  8){({ void* _temp2601[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type", sizeof(
unsigned char), 26u), _tag_arr( _temp2601, sizeof( void*), 0u));});} goto
_LL2592; _LL2596: if( w >  16){({ void* _temp2602[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("bitfield larger than type", sizeof( unsigned char), 26u), _tag_arr(
_temp2602, sizeof( void*), 0u));});} goto _LL2592; _LL2598: if( w >  32){({ void*
_temp2603[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u), _tag_arr( _temp2603, sizeof( void*), 0u));});}
goto _LL2592; _LL2600: if( w >  64){({ void* _temp2604[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u),
_tag_arr( _temp2604, sizeof( void*), 0u));});} goto _LL2592; _LL2592:;} goto
_LL2584; _LL2588:({ struct Cyc_std_String_pa_struct _temp2607; _temp2607.tag=
Cyc_std_String_pa; _temp2607.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
field_typ);{ struct Cyc_std_String_pa_struct _temp2606; _temp2606.tag= Cyc_std_String_pa;
_temp2606.f1=( struct _tagged_arr)* fn;{ void* _temp2605[ 2u]={& _temp2606,&
_temp2607}; Cyc_Tcutil_terr( loc, _tag_arr("bitfield %s must have integral type but has type %s",
sizeof( unsigned char), 52u), _tag_arr( _temp2605, sizeof( void*), 2u));}}});
goto _LL2584; _LL2584:;}}} static void Cyc_Tcutil_check_field_atts( struct Cyc_Position_Segment*
loc, struct _tagged_arr* fn, struct Cyc_List_List* atts){ for( 0; atts !=  0;
atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ void* _temp2608=( void*)((
struct Cyc_List_List*) _check_null( atts))->hd; _LL2610: if( _temp2608 == ( void*)
Cyc_Absyn_Packed_att){ goto _LL2611;} else{ goto _LL2612;} _LL2612: if((
unsigned int) _temp2608 >  16u?*(( int*) _temp2608) ==  Cyc_Absyn_Aligned_att: 0){
goto _LL2613;} else{ goto _LL2614;} _LL2614: goto _LL2615; _LL2611: continue;
_LL2613: continue; _LL2615:({ struct Cyc_std_String_pa_struct _temp2618;
_temp2618.tag= Cyc_std_String_pa; _temp2618.f1=( struct _tagged_arr)* fn;{
struct Cyc_std_String_pa_struct _temp2617; _temp2617.tag= Cyc_std_String_pa;
_temp2617.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*)(( struct
Cyc_List_List*) _check_null( atts))->hd);{ void* _temp2616[ 2u]={& _temp2617,&
_temp2618}; Cyc_Tcutil_terr( loc, _tag_arr("bad attribute %s on member %s",
sizeof( unsigned char), 30u), _tag_arr( _temp2616, sizeof( void*), 2u));}}});
goto _LL2609; _LL2609:;}} struct Cyc_Tcutil_CVTEnv{ struct Cyc_List_List*
kind_env; struct Cyc_List_List* free_vars; struct Cyc_List_List* free_evars; int
generalize_evars; int fn_result; } ; static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcutil_CVTEnv
cvtenv, void* expected_kind, void* t){{ void* _temp2619= Cyc_Tcutil_compress( t);
struct Cyc_Core_Opt* _temp2667; struct Cyc_Core_Opt** _temp2669; struct Cyc_Core_Opt*
_temp2670; struct Cyc_Core_Opt** _temp2672; struct Cyc_Absyn_Tvar* _temp2673;
struct Cyc_Absyn_Enumdecl* _temp2675; struct Cyc_Absyn_Enumdecl** _temp2677;
struct _tuple1* _temp2678; struct Cyc_Absyn_TunionInfo _temp2680; void*
_temp2682; struct Cyc_List_List* _temp2684; void* _temp2686; void** _temp2688;
struct Cyc_Absyn_TunionFieldInfo _temp2689; struct Cyc_List_List* _temp2691;
void* _temp2693; void** _temp2695; struct Cyc_Absyn_PtrInfo _temp2696; struct
Cyc_Absyn_Conref* _temp2698; struct Cyc_Absyn_Tqual _temp2700; struct Cyc_Absyn_Conref*
_temp2702; void* _temp2704; void* _temp2706; struct Cyc_Absyn_Exp* _temp2708;
struct Cyc_Absyn_Tqual _temp2710; void* _temp2712; struct Cyc_Absyn_FnInfo
_temp2714; struct Cyc_List_List* _temp2716; struct Cyc_List_List* _temp2718;
struct Cyc_Absyn_VarargInfo* _temp2720; int _temp2722; struct Cyc_List_List*
_temp2724; void* _temp2726; struct Cyc_Core_Opt* _temp2728; struct Cyc_Core_Opt**
_temp2730; struct Cyc_List_List* _temp2731; struct Cyc_List_List** _temp2733;
struct Cyc_List_List* _temp2734; struct Cyc_List_List* _temp2736; struct Cyc_List_List*
_temp2738; struct Cyc_Absyn_Structdecl** _temp2740; struct Cyc_Absyn_Structdecl***
_temp2742; struct Cyc_List_List* _temp2743; struct _tuple1* _temp2745; struct
Cyc_Absyn_Uniondecl** _temp2747; struct Cyc_Absyn_Uniondecl*** _temp2749; struct
Cyc_List_List* _temp2750; struct _tuple1* _temp2752; struct Cyc_Core_Opt*
_temp2754; struct Cyc_Core_Opt** _temp2756; struct Cyc_List_List* _temp2757;
struct _tuple1* _temp2759; void* _temp2761; void* _temp2763; void* _temp2765;
struct Cyc_List_List* _temp2767; _LL2621: if( _temp2619 == ( void*) Cyc_Absyn_VoidType){
goto _LL2622;} else{ goto _LL2623;} _LL2623: if(( unsigned int) _temp2619 >  4u?*((
int*) _temp2619) ==  Cyc_Absyn_Evar: 0){ _LL2671: _temp2670=(( struct Cyc_Absyn_Evar_struct*)
_temp2619)->f1; _temp2672=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp2619)->f1; goto _LL2668; _LL2668: _temp2667=(( struct Cyc_Absyn_Evar_struct*)
_temp2619)->f2; _temp2669=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*)
_temp2619)->f2; goto _LL2624;} else{ goto _LL2625;} _LL2625: if(( unsigned int)
_temp2619 >  4u?*(( int*) _temp2619) ==  Cyc_Absyn_VarType: 0){ _LL2674:
_temp2673=(( struct Cyc_Absyn_VarType_struct*) _temp2619)->f1; goto _LL2626;}
else{ goto _LL2627;} _LL2627: if(( unsigned int) _temp2619 >  4u?*(( int*)
_temp2619) ==  Cyc_Absyn_EnumType: 0){ _LL2679: _temp2678=(( struct Cyc_Absyn_EnumType_struct*)
_temp2619)->f1; goto _LL2676; _LL2676: _temp2675=(( struct Cyc_Absyn_EnumType_struct*)
_temp2619)->f2; _temp2677=( struct Cyc_Absyn_Enumdecl**)&(( struct Cyc_Absyn_EnumType_struct*)
_temp2619)->f2; goto _LL2628;} else{ goto _LL2629;} _LL2629: if(( unsigned int)
_temp2619 >  4u?*(( int*) _temp2619) ==  Cyc_Absyn_TunionType: 0){ _LL2681:
_temp2680=(( struct Cyc_Absyn_TunionType_struct*) _temp2619)->f1; _LL2687:
_temp2686=( void*) _temp2680.tunion_info; _temp2688=( void**)&((( struct Cyc_Absyn_TunionType_struct*)
_temp2619)->f1).tunion_info; goto _LL2685; _LL2685: _temp2684= _temp2680.targs;
goto _LL2683; _LL2683: _temp2682=( void*) _temp2680.rgn; goto _LL2630;} else{
goto _LL2631;} _LL2631: if(( unsigned int) _temp2619 >  4u?*(( int*) _temp2619)
==  Cyc_Absyn_TunionFieldType: 0){ _LL2690: _temp2689=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2619)->f1; _LL2694: _temp2693=( void*) _temp2689.field_info; _temp2695=(
void**)&((( struct Cyc_Absyn_TunionFieldType_struct*) _temp2619)->f1).field_info;
goto _LL2692; _LL2692: _temp2691= _temp2689.targs; goto _LL2632;} else{ goto
_LL2633;} _LL2633: if(( unsigned int) _temp2619 >  4u?*(( int*) _temp2619) == 
Cyc_Absyn_PointerType: 0){ _LL2697: _temp2696=(( struct Cyc_Absyn_PointerType_struct*)
_temp2619)->f1; _LL2707: _temp2706=( void*) _temp2696.elt_typ; goto _LL2705;
_LL2705: _temp2704=( void*) _temp2696.rgn_typ; goto _LL2703; _LL2703: _temp2702=
_temp2696.nullable; goto _LL2701; _LL2701: _temp2700= _temp2696.tq; goto _LL2699;
_LL2699: _temp2698= _temp2696.bounds; goto _LL2634;} else{ goto _LL2635;}
_LL2635: if(( unsigned int) _temp2619 >  4u?*(( int*) _temp2619) ==  Cyc_Absyn_IntType:
0){ goto _LL2636;} else{ goto _LL2637;} _LL2637: if( _temp2619 == ( void*) Cyc_Absyn_FloatType){
goto _LL2638;} else{ goto _LL2639;} _LL2639: if( _temp2619 == ( void*) Cyc_Absyn_DoubleType){
goto _LL2640;} else{ goto _LL2641;} _LL2641: if(( unsigned int) _temp2619 >  4u?*((
int*) _temp2619) ==  Cyc_Absyn_ArrayType: 0){ _LL2713: _temp2712=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2619)->f1; goto _LL2711; _LL2711:
_temp2710=(( struct Cyc_Absyn_ArrayType_struct*) _temp2619)->f2; goto _LL2709;
_LL2709: _temp2708=(( struct Cyc_Absyn_ArrayType_struct*) _temp2619)->f3; goto
_LL2642;} else{ goto _LL2643;} _LL2643: if(( unsigned int) _temp2619 >  4u?*((
int*) _temp2619) ==  Cyc_Absyn_FnType: 0){ _LL2715: _temp2714=(( struct Cyc_Absyn_FnType_struct*)
_temp2619)->f1; _LL2732: _temp2731= _temp2714.tvars; _temp2733=( struct Cyc_List_List**)&(((
struct Cyc_Absyn_FnType_struct*) _temp2619)->f1).tvars; goto _LL2729; _LL2729:
_temp2728= _temp2714.effect; _temp2730=( struct Cyc_Core_Opt**)&((( struct Cyc_Absyn_FnType_struct*)
_temp2619)->f1).effect; goto _LL2727; _LL2727: _temp2726=( void*) _temp2714.ret_typ;
goto _LL2725; _LL2725: _temp2724= _temp2714.args; goto _LL2723; _LL2723:
_temp2722= _temp2714.c_varargs; goto _LL2721; _LL2721: _temp2720= _temp2714.cyc_varargs;
goto _LL2719; _LL2719: _temp2718= _temp2714.rgn_po; goto _LL2717; _LL2717:
_temp2716= _temp2714.attributes; goto _LL2644;} else{ goto _LL2645;} _LL2645:
if(( unsigned int) _temp2619 >  4u?*(( int*) _temp2619) ==  Cyc_Absyn_TupleType:
0){ _LL2735: _temp2734=(( struct Cyc_Absyn_TupleType_struct*) _temp2619)->f1;
goto _LL2646;} else{ goto _LL2647;} _LL2647: if(( unsigned int) _temp2619 >  4u?*((
int*) _temp2619) ==  Cyc_Absyn_AnonStructType: 0){ _LL2737: _temp2736=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp2619)->f1; goto _LL2648;} else{ goto
_LL2649;} _LL2649: if(( unsigned int) _temp2619 >  4u?*(( int*) _temp2619) == 
Cyc_Absyn_AnonUnionType: 0){ _LL2739: _temp2738=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp2619)->f1; goto _LL2650;} else{ goto _LL2651;} _LL2651: if(( unsigned int)
_temp2619 >  4u?*(( int*) _temp2619) ==  Cyc_Absyn_StructType: 0){ _LL2746:
_temp2745=(( struct Cyc_Absyn_StructType_struct*) _temp2619)->f1; goto _LL2744;
_LL2744: _temp2743=(( struct Cyc_Absyn_StructType_struct*) _temp2619)->f2; goto
_LL2741; _LL2741: _temp2740=(( struct Cyc_Absyn_StructType_struct*) _temp2619)->f3;
_temp2742=( struct Cyc_Absyn_Structdecl***)&(( struct Cyc_Absyn_StructType_struct*)
_temp2619)->f3; goto _LL2652;} else{ goto _LL2653;} _LL2653: if(( unsigned int)
_temp2619 >  4u?*(( int*) _temp2619) ==  Cyc_Absyn_UnionType: 0){ _LL2753:
_temp2752=(( struct Cyc_Absyn_UnionType_struct*) _temp2619)->f1; goto _LL2751;
_LL2751: _temp2750=(( struct Cyc_Absyn_UnionType_struct*) _temp2619)->f2; goto
_LL2748; _LL2748: _temp2747=(( struct Cyc_Absyn_UnionType_struct*) _temp2619)->f3;
_temp2749=( struct Cyc_Absyn_Uniondecl***)&(( struct Cyc_Absyn_UnionType_struct*)
_temp2619)->f3; goto _LL2654;} else{ goto _LL2655;} _LL2655: if(( unsigned int)
_temp2619 >  4u?*(( int*) _temp2619) ==  Cyc_Absyn_TypedefType: 0){ _LL2760:
_temp2759=(( struct Cyc_Absyn_TypedefType_struct*) _temp2619)->f1; goto _LL2758;
_LL2758: _temp2757=(( struct Cyc_Absyn_TypedefType_struct*) _temp2619)->f2; goto
_LL2755; _LL2755: _temp2754=(( struct Cyc_Absyn_TypedefType_struct*) _temp2619)->f3;
_temp2756=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_TypedefType_struct*)
_temp2619)->f3; goto _LL2656;} else{ goto _LL2657;} _LL2657: if( _temp2619 == (
void*) Cyc_Absyn_HeapRgn){ goto _LL2658;} else{ goto _LL2659;} _LL2659: if((
unsigned int) _temp2619 >  4u?*(( int*) _temp2619) ==  Cyc_Absyn_RgnHandleType:
0){ _LL2762: _temp2761=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp2619)->f1; goto _LL2660;} else{ goto _LL2661;} _LL2661: if(( unsigned int)
_temp2619 >  4u?*(( int*) _temp2619) ==  Cyc_Absyn_AccessEff: 0){ _LL2764:
_temp2763=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp2619)->f1; goto
_LL2662;} else{ goto _LL2663;} _LL2663: if(( unsigned int) _temp2619 >  4u?*((
int*) _temp2619) ==  Cyc_Absyn_RgnsEff: 0){ _LL2766: _temp2765=( void*)(( struct
Cyc_Absyn_RgnsEff_struct*) _temp2619)->f1; goto _LL2664;} else{ goto _LL2665;}
_LL2665: if(( unsigned int) _temp2619 >  4u?*(( int*) _temp2619) ==  Cyc_Absyn_JoinEff:
0){ _LL2768: _temp2767=(( struct Cyc_Absyn_JoinEff_struct*) _temp2619)->f1; goto
_LL2666;} else{ goto _LL2620;} _LL2622: goto _LL2620; _LL2624: if(* _temp2672 == 
0){* _temp2672=({ struct Cyc_Core_Opt* _temp2769=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2769->v=( void*) expected_kind;
_temp2769;});} if(( cvtenv.fn_result? cvtenv.generalize_evars: 0)? expected_kind
== ( void*) Cyc_Absyn_RgnKind: 0){* _temp2669=({ struct Cyc_Core_Opt* _temp2770=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2770->v=(
void*)(( void*) Cyc_Absyn_HeapRgn); _temp2770;});} else{ if( cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar* _temp2771= Cyc_Tcutil_new_tvar( expected_kind);*
_temp2669=({ struct Cyc_Core_Opt* _temp2772=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp2772->v=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2773=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2773[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2774; _temp2774.tag= Cyc_Absyn_VarType;
_temp2774.f1= _temp2771; _temp2774;}); _temp2773;})); _temp2772;}); _temp2673=
_temp2771; goto _LL2626;} else{ cvtenv.free_evars= Cyc_Tcutil_add_free_evar(
cvtenv.free_evars, t);}} goto _LL2620; _LL2626: { struct Cyc_Absyn_Conref* c=
Cyc_Absyn_compress_conref( _temp2673->kind); if(( void*) c->v == ( void*) Cyc_Absyn_No_constr){(
void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2775=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2775[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2776; _temp2776.tag= Cyc_Absyn_Eq_constr;
_temp2776.f1=( void*) expected_kind; _temp2776;}); _temp2775;})));} cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar( loc, cvtenv.kind_env, _temp2673); cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars, _temp2673); goto _LL2620;}
_LL2628: if(* _temp2677 ==  0){ struct _handler_cons _temp2777; _push_handler(&
_temp2777);{ int _temp2779= 0; if( setjmp( _temp2777.handler)){ _temp2779= 1;}
if( ! _temp2779){{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl( te,
loc, _temp2678);* _temp2677=( struct Cyc_Absyn_Enumdecl*)* ed;}; _pop_handler();}
else{ void* _temp2778=( void*) _exn_thrown; void* _temp2781= _temp2778; _LL2783:
if( _temp2781 ==  Cyc_Dict_Absent){ goto _LL2784;} else{ goto _LL2785;} _LL2785:
goto _LL2786; _LL2784:({ struct Cyc_std_String_pa_struct _temp2788; _temp2788.tag=
Cyc_std_String_pa; _temp2788.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2678);{ void* _temp2787[ 1u]={& _temp2788}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type enum %s",
sizeof( unsigned char), 21u), _tag_arr( _temp2787, sizeof( void*), 1u));}});
return cvtenv; _LL2786:( void) _throw( _temp2781); _LL2782:;}}}{ struct Cyc_Absyn_Enumdecl*
ed=( struct Cyc_Absyn_Enumdecl*) _check_null(* _temp2677);* _temp2678=( ed->name)[
_check_known_subscript_notnull( 1u, 0)]; goto _LL2620;} _LL2630:{ void*
_temp2789=* _temp2688; struct Cyc_Absyn_UnknownTunionInfo _temp2795; int
_temp2797; struct _tuple1* _temp2799; struct Cyc_Absyn_Tuniondecl* _temp2801;
_LL2791: if(*(( int*) _temp2789) ==  Cyc_Absyn_UnknownTunion){ _LL2796:
_temp2795=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp2789)->f1; _LL2800:
_temp2799= _temp2795.name; goto _LL2798; _LL2798: _temp2797= _temp2795.is_xtunion;
goto _LL2792;} else{ goto _LL2793;} _LL2793: if(*(( int*) _temp2789) ==  Cyc_Absyn_KnownTunion){
_LL2802: _temp2801=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2789)->f1; goto
_LL2794;} else{ goto _LL2790;} _LL2792: { struct Cyc_Absyn_Tuniondecl* tud;{
struct _handler_cons _temp2803; _push_handler(& _temp2803);{ int _temp2805= 0;
if( setjmp( _temp2803.handler)){ _temp2805= 1;} if( ! _temp2805){ tud=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp2799);; _pop_handler();} else{ void* _temp2804=( void*)
_exn_thrown; void* _temp2807= _temp2804; _LL2809: if( _temp2807 ==  Cyc_Dict_Absent){
goto _LL2810;} else{ goto _LL2811;} _LL2811: goto _LL2812; _LL2810:({ struct Cyc_std_String_pa_struct
_temp2814; _temp2814.tag= Cyc_std_String_pa; _temp2814.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2799);{ void* _temp2813[ 1u]={& _temp2814}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound type [x]tunion %s", sizeof( unsigned char), 26u),
_tag_arr( _temp2813, sizeof( void*), 1u));}}); return cvtenv; _LL2812:( void)
_throw( _temp2807); _LL2808:;}}} if( tud->is_xtunion !=  _temp2797){({ struct
Cyc_std_String_pa_struct _temp2816; _temp2816.tag= Cyc_std_String_pa; _temp2816.f1=(
struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2799);{ void* _temp2815[ 1u]={&
_temp2816}; Cyc_Tcutil_terr( loc, _tag_arr("[x]tunion is different from type declaration %s",
sizeof( unsigned char), 48u), _tag_arr( _temp2815, sizeof( void*), 1u));}});}*
_temp2688=( void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp2817=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp2817[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp2818; _temp2818.tag= Cyc_Absyn_KnownTunion;
_temp2818.f1= tud; _temp2818;}); _temp2817;}); _temp2801= tud; goto _LL2794;}
_LL2794: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2682);{ struct Cyc_List_List* tvs= _temp2801->tvs; for( 0; _temp2684 !=  0?
tvs !=  0: 0;( _temp2684=(( struct Cyc_List_List*) _check_null( _temp2684))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2684))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} if( _temp2684
!=  0){({ struct Cyc_std_String_pa_struct _temp2820; _temp2820.tag= Cyc_std_String_pa;
_temp2820.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2801->name);{
void* _temp2819[ 1u]={& _temp2820}; Cyc_Tcutil_terr( loc, _tag_arr("too many type arguments for tunion %s",
sizeof( unsigned char), 38u), _tag_arr( _temp2819, sizeof( void*), 1u));}});}
if( tvs !=  0){({ struct Cyc_std_String_pa_struct _temp2822; _temp2822.tag= Cyc_std_String_pa;
_temp2822.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2801->name);{
void* _temp2821[ 1u]={& _temp2822}; Cyc_Tcutil_terr( loc, _tag_arr("too few type arguments for tunion %s",
sizeof( unsigned char), 37u), _tag_arr( _temp2821, sizeof( void*), 1u));}});}
goto _LL2790;} _LL2790:;} goto _LL2620; _LL2632:{ void* _temp2823=* _temp2695;
struct Cyc_Absyn_UnknownTunionFieldInfo _temp2829; int _temp2831; struct _tuple1*
_temp2833; struct _tuple1* _temp2835; struct Cyc_Absyn_Tunionfield* _temp2837;
struct Cyc_Absyn_Tuniondecl* _temp2839; _LL2825: if(*(( int*) _temp2823) ==  Cyc_Absyn_UnknownTunionfield){
_LL2830: _temp2829=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp2823)->f1;
_LL2836: _temp2835= _temp2829.tunion_name; goto _LL2834; _LL2834: _temp2833=
_temp2829.field_name; goto _LL2832; _LL2832: _temp2831= _temp2829.is_xtunion;
goto _LL2826;} else{ goto _LL2827;} _LL2827: if(*(( int*) _temp2823) ==  Cyc_Absyn_KnownTunionfield){
_LL2840: _temp2839=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp2823)->f1;
goto _LL2838; _LL2838: _temp2837=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2823)->f2; goto _LL2828;} else{ goto _LL2824;} _LL2826: { struct Cyc_Absyn_Tuniondecl*
tud; struct Cyc_Absyn_Tunionfield* tuf;{ struct _handler_cons _temp2841;
_push_handler(& _temp2841);{ int _temp2843= 0; if( setjmp( _temp2841.handler)){
_temp2843= 1;} if( ! _temp2843){* Cyc_Tcenv_lookup_tuniondecl( te, loc,
_temp2835);; _pop_handler();} else{ void* _temp2842=( void*) _exn_thrown; void*
_temp2845= _temp2842; _LL2847: if( _temp2845 ==  Cyc_Dict_Absent){ goto _LL2848;}
else{ goto _LL2849;} _LL2849: goto _LL2850; _LL2848:({ struct Cyc_std_String_pa_struct
_temp2852; _temp2852.tag= Cyc_std_String_pa; _temp2852.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2835);{ void* _temp2851[ 1u]={& _temp2852}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound type tunion %s", sizeof( unsigned char), 23u), _tag_arr(
_temp2851, sizeof( void*), 1u));}}); return cvtenv; _LL2850:( void) _throw(
_temp2845); _LL2846:;}}}{ struct _handler_cons _temp2853; _push_handler(&
_temp2853);{ int _temp2855= 0; if( setjmp( _temp2853.handler)){ _temp2855= 1;}
if( ! _temp2855){{ void* _temp2856= Cyc_Tcenv_lookup_ordinary( te, loc,
_temp2833); struct Cyc_Absyn_Tunionfield* _temp2862; struct Cyc_Absyn_Tuniondecl*
_temp2864; _LL2858: if(*(( int*) _temp2856) ==  Cyc_Tcenv_TunionRes){ _LL2865:
_temp2864=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2856)->f1; goto _LL2863;
_LL2863: _temp2862=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2856)->f2; goto
_LL2859;} else{ goto _LL2860;} _LL2860: goto _LL2861; _LL2859: tuf= _temp2862;
tud= _temp2864; if( tud->is_xtunion !=  _temp2831){({ struct Cyc_std_String_pa_struct
_temp2867; _temp2867.tag= Cyc_std_String_pa; _temp2867.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2835);{ void* _temp2866[ 1u]={& _temp2867}; Cyc_Tcutil_terr(
loc, _tag_arr("[x]tunion is different from type declaration %s", sizeof(
unsigned char), 48u), _tag_arr( _temp2866, sizeof( void*), 1u));}});} goto
_LL2857; _LL2861:({ struct Cyc_std_String_pa_struct _temp2870; _temp2870.tag=
Cyc_std_String_pa; _temp2870.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2835);{ struct Cyc_std_String_pa_struct _temp2869; _temp2869.tag= Cyc_std_String_pa;
_temp2869.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2833);{ void*
_temp2868[ 2u]={& _temp2869,& _temp2870}; Cyc_Tcutil_terr( loc, _tag_arr("unbound field %s in type tunion %s",
sizeof( unsigned char), 35u), _tag_arr( _temp2868, sizeof( void*), 2u));}}});{
struct Cyc_Tcutil_CVTEnv _temp2871= cvtenv; _npop_handler( 0u); return _temp2871;}
_LL2857:;}; _pop_handler();} else{ void* _temp2854=( void*) _exn_thrown; void*
_temp2873= _temp2854; _LL2875: if( _temp2873 ==  Cyc_Dict_Absent){ goto _LL2876;}
else{ goto _LL2877;} _LL2877: goto _LL2878; _LL2876:({ struct Cyc_std_String_pa_struct
_temp2881; _temp2881.tag= Cyc_std_String_pa; _temp2881.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2835);{ struct Cyc_std_String_pa_struct _temp2880;
_temp2880.tag= Cyc_std_String_pa; _temp2880.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp2833);{ void* _temp2879[ 2u]={& _temp2880,& _temp2881}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound field %s in type tunion %s", sizeof( unsigned char), 35u),
_tag_arr( _temp2879, sizeof( void*), 2u));}}}); return cvtenv; _LL2878:( void)
_throw( _temp2873); _LL2874:;}}}* _temp2695=( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp2882=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp2882[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp2883; _temp2883.tag= Cyc_Absyn_KnownTunionfield; _temp2883.f1= tud;
_temp2883.f2= tuf; _temp2883;}); _temp2882;}); _temp2839= tud; _temp2837= tuf;
goto _LL2828;} _LL2828: { struct Cyc_List_List* tvs= _temp2839->tvs; for( 0;
_temp2691 !=  0? tvs !=  0: 0;( _temp2691=(( struct Cyc_List_List*) _check_null(
_temp2691))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2691))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} if( _temp2691
!=  0){({ struct Cyc_std_String_pa_struct _temp2886; _temp2886.tag= Cyc_std_String_pa;
_temp2886.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2837->name);{
struct Cyc_std_String_pa_struct _temp2885; _temp2885.tag= Cyc_std_String_pa;
_temp2885.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2839->name);{
void* _temp2884[ 2u]={& _temp2885,& _temp2886}; Cyc_Tcutil_terr( loc, _tag_arr("too many type arguments for tunion %s.%s",
sizeof( unsigned char), 41u), _tag_arr( _temp2884, sizeof( void*), 2u));}}});}
if( tvs !=  0){({ struct Cyc_std_String_pa_struct _temp2889; _temp2889.tag= Cyc_std_String_pa;
_temp2889.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2837->name);{
struct Cyc_std_String_pa_struct _temp2888; _temp2888.tag= Cyc_std_String_pa;
_temp2888.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp2839->name);{
void* _temp2887[ 2u]={& _temp2888,& _temp2889}; Cyc_Tcutil_terr( loc, _tag_arr("too few type arguments for tunion %s.%s",
sizeof( unsigned char), 40u), _tag_arr( _temp2887, sizeof( void*), 2u));}}});}
goto _LL2824;} _LL2824:;} goto _LL2620; _LL2634: cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_AnyKind, _temp2706); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind, _temp2704);{ void* _temp2890=( void*)(
Cyc_Absyn_compress_conref( _temp2698))->v; void* _temp2896; struct Cyc_Absyn_Exp*
_temp2898; _LL2892: if(( unsigned int) _temp2890 >  1u?*(( int*) _temp2890) == 
Cyc_Absyn_Eq_constr: 0){ _LL2897: _temp2896=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2890)->f1; if(( unsigned int) _temp2896 >  1u?*(( int*) _temp2896) ==  Cyc_Absyn_Upper_b:
0){ _LL2899: _temp2898=(( struct Cyc_Absyn_Upper_b_struct*) _temp2896)->f1; goto
_LL2893;} else{ goto _LL2894;}} else{ goto _LL2894;} _LL2894: goto _LL2895;
_LL2893: if( ! Cyc_Tcutil_is_const_exp( te, _temp2898)){({ void* _temp2900[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("pointer bounds expression is not a constant",
sizeof( unsigned char), 44u), _tag_arr( _temp2900, sizeof( void*), 0u));});} Cyc_Tcexp_tcExp(
te, 0, _temp2898); if( ! Cyc_Tcutil_coerce_uint_typ( te, _temp2898)){({ void*
_temp2901[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("pointer bounds expression is not an unsigned int",
sizeof( unsigned char), 49u), _tag_arr( _temp2901, sizeof( void*), 0u));});} Cyc_Evexp_eval_const_uint_exp(
_temp2898); goto _LL2891; _LL2895: goto _LL2891; _LL2891:;} goto _LL2620;
_LL2636: goto _LL2620; _LL2638: goto _LL2620; _LL2640: goto _LL2620; _LL2642:
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,
_temp2712); if( _temp2708 ==  0? 1: ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp2708))){({ void* _temp2902[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("array bounds expression is not constant", sizeof( unsigned char), 40u),
_tag_arr( _temp2902, sizeof( void*), 0u));});} Cyc_Tcexp_tcExp( te, 0,( struct
Cyc_Absyn_Exp*) _check_null( _temp2708)); if( ! Cyc_Tcutil_coerce_uint_typ( te,(
struct Cyc_Absyn_Exp*) _check_null( _temp2708))){({ void* _temp2903[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("array bounds expression is not an unsigned int", sizeof(
unsigned char), 47u), _tag_arr( _temp2903, sizeof( void*), 0u));});} Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2708)); goto _LL2620; _LL2644: { int
num_convs= 0; int seen_cdecl= 0; int seen_stdcall= 0; int seen_fastcall= 0; int
seen_format= 0; void* ft=( void*) Cyc_Absyn_Printf_ft; int fmt_desc_arg= - 1;
int fmt_arg_start= - 1; for( 0; _temp2716 !=  0; _temp2716=(( struct Cyc_List_List*)
_check_null( _temp2716))->tl){ if( ! Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*)
_check_null( _temp2716))->hd)){({ struct Cyc_std_String_pa_struct _temp2905;
_temp2905.tag= Cyc_std_String_pa; _temp2905.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp2716))->hd);{ void* _temp2904[
1u]={& _temp2905}; Cyc_Tcutil_terr( loc, _tag_arr("bad function type attribute %s",
sizeof( unsigned char), 31u), _tag_arr( _temp2904, sizeof( void*), 1u));}});}{
void* _temp2906=( void*)(( struct Cyc_List_List*) _check_null( _temp2716))->hd;
int _temp2918; int _temp2920; void* _temp2922; _LL2908: if( _temp2906 == ( void*)
Cyc_Absyn_Stdcall_att){ goto _LL2909;} else{ goto _LL2910;} _LL2910: if(
_temp2906 == ( void*) Cyc_Absyn_Cdecl_att){ goto _LL2911;} else{ goto _LL2912;}
_LL2912: if( _temp2906 == ( void*) Cyc_Absyn_Fastcall_att){ goto _LL2913;} else{
goto _LL2914;} _LL2914: if(( unsigned int) _temp2906 >  16u?*(( int*) _temp2906)
==  Cyc_Absyn_Format_att: 0){ _LL2923: _temp2922=( void*)(( struct Cyc_Absyn_Format_att_struct*)
_temp2906)->f1; goto _LL2921; _LL2921: _temp2920=(( struct Cyc_Absyn_Format_att_struct*)
_temp2906)->f2; goto _LL2919; _LL2919: _temp2918=(( struct Cyc_Absyn_Format_att_struct*)
_temp2906)->f3; goto _LL2915;} else{ goto _LL2916;} _LL2916: goto _LL2917;
_LL2909: if( ! seen_stdcall){ seen_stdcall= 1; ++ num_convs;} goto _LL2907;
_LL2911: if( ! seen_cdecl){ seen_cdecl= 1; ++ num_convs;} goto _LL2907; _LL2913:
if( ! seen_fastcall){ seen_fastcall= 1; ++ num_convs;} goto _LL2907; _LL2915:
if( ! seen_format){ seen_format= 1; ft= _temp2922; fmt_desc_arg= _temp2920;
fmt_arg_start= _temp2918;} else{({ void* _temp2924[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("function can't have multiple format attributes", sizeof( unsigned char),
47u), _tag_arr( _temp2924, sizeof( void*), 0u));});} goto _LL2907; _LL2917: goto
_LL2907; _LL2907:;}} if( num_convs >  1){({ void* _temp2925[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("function can't have multiple calling conventions", sizeof(
unsigned char), 49u), _tag_arr( _temp2925, sizeof( void*), 0u));});} Cyc_Tcutil_check_unique_tvars(
loc,* _temp2733);{ struct Cyc_List_List* b=* _temp2733; for( 0; b !=  0; b=((
struct Cyc_List_List*) _check_null( b))->tl){(( struct Cyc_Absyn_Tvar*)(( struct
Cyc_List_List*) _check_null( b))->hd)->identity= Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env= Cyc_Tcutil_add_bound_tvar( cvtenv.kind_env,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( b))->hd);{ void* _temp2926=( void*)( Cyc_Absyn_compress_conref(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd)->kind))->v;
void* _temp2932; _LL2928: if(( unsigned int) _temp2926 >  1u?*(( int*) _temp2926)
==  Cyc_Absyn_Eq_constr: 0){ _LL2933: _temp2932=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2926)->f1; if( _temp2932 == ( void*) Cyc_Absyn_MemKind){ goto _LL2929;}
else{ goto _LL2930;}} else{ goto _LL2930;} _LL2930: goto _LL2931; _LL2929:({
struct Cyc_std_String_pa_struct _temp2935; _temp2935.tag= Cyc_std_String_pa;
_temp2935.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( b))->hd)->name;{ void* _temp2934[ 1u]={& _temp2935}; Cyc_Tcutil_terr(
loc, _tag_arr("function attempts to abstract Mem type variable %s", sizeof(
unsigned char), 51u), _tag_arr( _temp2934, sizeof( void*), 1u));}}); goto
_LL2927; _LL2931: goto _LL2927; _LL2927:;}}}{ struct Cyc_Tcutil_CVTEnv _temp2936=({
struct Cyc_Tcutil_CVTEnv _temp3096; _temp3096.kind_env= cvtenv.kind_env;
_temp3096.free_vars= 0; _temp3096.free_evars= 0; _temp3096.generalize_evars=
cvtenv.generalize_evars; _temp3096.fn_result= 1; _temp3096;}); _temp2936= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2936,( void*) Cyc_Absyn_MemKind, _temp2726); _temp2936.fn_result=
0;{ struct Cyc_List_List* a= _temp2724; for( 0; a !=  0; a=(( struct Cyc_List_List*)
_check_null( a))->tl){ _temp2936= Cyc_Tcutil_i_check_valid_type( loc, te,
_temp2936,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( a))->hd)).f3);}} if( _temp2720 !=  0){ if( _temp2722){({ void*
_temp2937[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("both c_vararg and cyc_vararg", sizeof( unsigned char), 29u), _tag_arr(
_temp2937, sizeof( void*), 0u));});}{ int _temp2940; void* _temp2942; void*
_temp2944; struct Cyc_Absyn_Tqual _temp2946; struct Cyc_Core_Opt* _temp2948;
struct Cyc_Absyn_VarargInfo _temp2938=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp2720)); _LL2949: _temp2948= _temp2938.name; goto _LL2947;
_LL2947: _temp2946= _temp2938.tq; goto _LL2945; _LL2945: _temp2944=( void*)
_temp2938.type; goto _LL2943; _LL2943: _temp2942=( void*) _temp2938.rgn; goto
_LL2941; _LL2941: _temp2940= _temp2938.inject; goto _LL2939; _LL2939: _temp2936=
Cyc_Tcutil_i_check_valid_type( loc, te, _temp2936,( void*) Cyc_Absyn_MemKind,
_temp2944); _temp2936= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2936,( void*)
Cyc_Absyn_RgnKind, _temp2942); if( _temp2940){ void* _temp2950= Cyc_Tcutil_compress(
_temp2944); struct Cyc_Absyn_TunionInfo _temp2956; void* _temp2958; void*
_temp2960; _LL2952: if(( unsigned int) _temp2950 >  4u?*(( int*) _temp2950) == 
Cyc_Absyn_TunionType: 0){ _LL2957: _temp2956=(( struct Cyc_Absyn_TunionType_struct*)
_temp2950)->f1; _LL2961: _temp2960=( void*) _temp2956.tunion_info; if(*(( int*)
_temp2960) ==  Cyc_Absyn_KnownTunion){ goto _LL2959;} else{ goto _LL2954;}
_LL2959: _temp2958=( void*) _temp2956.rgn; goto _LL2953;} else{ goto _LL2954;}
_LL2954: goto _LL2955; _LL2953: goto _LL2951; _LL2955:({ void* _temp2962[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("can't inject a non-[x]tunion type", sizeof(
unsigned char), 34u), _tag_arr( _temp2962, sizeof( void*), 0u));}); goto _LL2951;
_LL2951:;}}} if( seen_format){ int _temp2963=(( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp2724); if(((( fmt_desc_arg <  0? 1: fmt_desc_arg > 
_temp2963)? 1: fmt_arg_start <  0)? 1:( _temp2720 ==  0? fmt_arg_start !=  0: 0))?
1:( _temp2720 !=  0? fmt_arg_start !=  _temp2963 +  1: 0)){({ void* _temp2964[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("bad format descriptor", sizeof( unsigned char),
22u), _tag_arr( _temp2964, sizeof( void*), 0u));});} else{ void* _temp2967;
struct _tuple2 _temp2965=*(( struct _tuple2*(*)( struct Cyc_List_List* x, int i))
Cyc_List_nth)( _temp2724, fmt_desc_arg -  1); _LL2968: _temp2967= _temp2965.f3;
goto _LL2966; _LL2966:{ void* _temp2969= Cyc_Tcutil_compress( _temp2967); struct
Cyc_Absyn_PtrInfo _temp2975; struct Cyc_Absyn_Conref* _temp2977; void* _temp2979;
_LL2971: if(( unsigned int) _temp2969 >  4u?*(( int*) _temp2969) ==  Cyc_Absyn_PointerType:
0){ _LL2976: _temp2975=(( struct Cyc_Absyn_PointerType_struct*) _temp2969)->f1;
_LL2980: _temp2979=( void*) _temp2975.elt_typ; goto _LL2978; _LL2978: _temp2977=
_temp2975.bounds; goto _LL2972;} else{ goto _LL2973;} _LL2973: goto _LL2974;
_LL2972:{ struct _tuple8 _temp2982=({ struct _tuple8 _temp2981; _temp2981.f1=
Cyc_Tcutil_compress( _temp2979); _temp2981.f2= Cyc_Absyn_conref_def(( void*) Cyc_Absyn_Unknown_b,
_temp2977); _temp2981;}); void* _temp2988; void* _temp2990; void* _temp2992;
void* _temp2994; _LL2984: _LL2991: _temp2990= _temp2982.f1; if(( unsigned int)
_temp2990 >  4u?*(( int*) _temp2990) ==  Cyc_Absyn_IntType: 0){ _LL2995:
_temp2994=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2990)->f1; if(
_temp2994 == ( void*) Cyc_Absyn_Unsigned){ goto _LL2993;} else{ goto _LL2986;}
_LL2993: _temp2992=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2990)->f2;
if( _temp2992 == ( void*) Cyc_Absyn_B1){ goto _LL2989;} else{ goto _LL2986;}}
else{ goto _LL2986;} _LL2989: _temp2988= _temp2982.f2; if( _temp2988 == ( void*)
Cyc_Absyn_Unknown_b){ goto _LL2985;} else{ goto _LL2986;} _LL2986: goto _LL2987;
_LL2985: goto _LL2983; _LL2987:({ void* _temp2996[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("format descriptor is not a char ? type", sizeof( unsigned char), 39u),
_tag_arr( _temp2996, sizeof( void*), 0u));}); goto _LL2983; _LL2983:;} goto
_LL2970; _LL2974:({ void* _temp2997[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("format descriptor is not a char ? type",
sizeof( unsigned char), 39u), _tag_arr( _temp2997, sizeof( void*), 0u));}); goto
_LL2970; _LL2970:;} if( fmt_arg_start !=  0){ void* _temp2998= Cyc_Tcutil_compress((
void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp2720))->type); int
_temp2999= 0;{ void* _temp3000= ft; _LL3002: if( _temp3000 == ( void*) Cyc_Absyn_Printf_ft){
goto _LL3003;} else{ goto _LL3004;} _LL3004: if( _temp3000 == ( void*) Cyc_Absyn_Scanf_ft){
goto _LL3005;} else{ goto _LL3001;} _LL3003:{ void* _temp3006= _temp2998; struct
Cyc_Absyn_TunionInfo _temp3012; void* _temp3014; struct Cyc_Absyn_Tuniondecl*
_temp3016; _LL3008: if(( unsigned int) _temp3006 >  4u?*(( int*) _temp3006) == 
Cyc_Absyn_TunionType: 0){ _LL3013: _temp3012=(( struct Cyc_Absyn_TunionType_struct*)
_temp3006)->f1; _LL3015: _temp3014=( void*) _temp3012.tunion_info; if(*(( int*)
_temp3014) ==  Cyc_Absyn_KnownTunion){ _LL3017: _temp3016=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp3014)->f1; goto _LL3009;} else{ goto _LL3010;}} else{ goto _LL3010;}
_LL3010: goto _LL3011; _LL3009: _temp2999= Cyc_Absyn_qvar_cmp( _temp3016->name,
Cyc_Absyn_tunion_print_arg_qvar) !=  0; goto _LL3007; _LL3011: _temp2999= 1;
goto _LL3007; _LL3007:;} goto _LL3001; _LL3005:{ void* _temp3018= _temp2998;
struct Cyc_Absyn_TunionInfo _temp3024; void* _temp3026; struct Cyc_Absyn_Tuniondecl*
_temp3028; _LL3020: if(( unsigned int) _temp3018 >  4u?*(( int*) _temp3018) == 
Cyc_Absyn_TunionType: 0){ _LL3025: _temp3024=(( struct Cyc_Absyn_TunionType_struct*)
_temp3018)->f1; _LL3027: _temp3026=( void*) _temp3024.tunion_info; if(*(( int*)
_temp3026) ==  Cyc_Absyn_KnownTunion){ _LL3029: _temp3028=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp3026)->f1; goto _LL3021;} else{ goto _LL3022;}} else{ goto _LL3022;}
_LL3022: goto _LL3023; _LL3021: _temp2999= Cyc_Absyn_qvar_cmp( _temp3028->name,
Cyc_Absyn_tunion_scanf_arg_qvar) !=  0; goto _LL3019; _LL3023: _temp2999= 1;
goto _LL3019; _LL3019:;} goto _LL3001; _LL3001:;} if( _temp2999){({ void*
_temp3030[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("format attribute and vararg types don't match",
sizeof( unsigned char), 46u), _tag_arr( _temp3030, sizeof( void*), 0u));});}}}}{
struct Cyc_List_List* rpo= _temp2718; for( 0; rpo !=  0; rpo=(( struct Cyc_List_List*)
_check_null( rpo))->tl){ struct _tuple8 _temp3033; void* _temp3034; void*
_temp3036; struct _tuple8* _temp3031=( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( rpo))->hd; _temp3033=* _temp3031; _LL3037: _temp3036= _temp3033.f1;
goto _LL3035; _LL3035: _temp3034= _temp3033.f2; goto _LL3032; _LL3032: _temp2936=
Cyc_Tcutil_i_check_valid_type( loc, te, _temp2936,( void*) Cyc_Absyn_RgnKind,
_temp3036); _temp2936= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2936,( void*)
Cyc_Absyn_RgnKind, _temp3034);}} if(* _temp2730 !=  0){ _temp2936= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2936,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_Core_Opt*)
_check_null(* _temp2730))->v);} else{ if( cvtenv.generalize_evars){;}{ struct
Cyc_List_List* effect= 0;{ struct Cyc_List_List* tvs= _temp2936.free_vars; for(
0; tvs !=  0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ void*
_temp3038=( void*)( Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind))->v; void* _temp3046; void*
_temp3048; _LL3040: if(( unsigned int) _temp3038 >  1u?*(( int*) _temp3038) == 
Cyc_Absyn_Eq_constr: 0){ _LL3047: _temp3046=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3038)->f1; if( _temp3046 == ( void*) Cyc_Absyn_RgnKind){ goto _LL3041;}
else{ goto _LL3042;}} else{ goto _LL3042;} _LL3042: if(( unsigned int) _temp3038
>  1u?*(( int*) _temp3038) ==  Cyc_Absyn_Eq_constr: 0){ _LL3049: _temp3048=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3038)->f1; if( _temp3048 == (
void*) Cyc_Absyn_EffKind){ goto _LL3043;} else{ goto _LL3044;}} else{ goto
_LL3044;} _LL3044: goto _LL3045; _LL3041: effect=({ struct Cyc_List_List*
_temp3050=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3050->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3051=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3051[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3052; _temp3052.tag= Cyc_Absyn_AccessEff;
_temp3052.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3053=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3053[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3054; _temp3054.tag= Cyc_Absyn_VarType;
_temp3054.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3054;}); _temp3053;})); _temp3052;}); _temp3051;})); _temp3050->tl= effect;
_temp3050;}); goto _LL3039; _LL3043: effect=({ struct Cyc_List_List* _temp3055=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3055->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3056=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp3056[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp3057; _temp3057.tag= Cyc_Absyn_VarType; _temp3057.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3057;}); _temp3056;})); _temp3055->tl= effect; _temp3055;}); goto _LL3039;
_LL3045: effect=({ struct Cyc_List_List* _temp3058=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3058->hd=( void*)(( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp3059=( struct Cyc_Absyn_RgnsEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp3059[ 0]=({ struct
Cyc_Absyn_RgnsEff_struct _temp3060; _temp3060.tag= Cyc_Absyn_RgnsEff; _temp3060.f1=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3061=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp3061[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp3062; _temp3062.tag= Cyc_Absyn_VarType; _temp3062.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3062;}); _temp3061;})); _temp3060;}); _temp3059;})); _temp3058->tl= effect;
_temp3058;}); goto _LL3039; _LL3039:;}} effect= Cyc_List_imp_rev( effect);{
struct Cyc_List_List* ts= _temp2936.free_evars; for( 0; ts !=  0; ts=(( struct
Cyc_List_List*) _check_null( ts))->tl){ void* _temp3063= Cyc_Tcutil_typ_kind((
void*)(( struct Cyc_List_List*) _check_null( ts))->hd); _LL3065: if( _temp3063
== ( void*) Cyc_Absyn_RgnKind){ goto _LL3066;} else{ goto _LL3067;} _LL3067: if(
_temp3063 == ( void*) Cyc_Absyn_EffKind){ goto _LL3068;} else{ goto _LL3069;}
_LL3069: goto _LL3070; _LL3066: effect=({ struct Cyc_List_List* _temp3071=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3071->hd=(
void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3072=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp3072[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp3073; _temp3073.tag= Cyc_Absyn_AccessEff;
_temp3073.f1=( void*)(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);
_temp3073;}); _temp3072;})); _temp3071->tl= effect; _temp3071;}); goto _LL3064;
_LL3068: effect=({ struct Cyc_List_List* _temp3074=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3074->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( ts))->hd); _temp3074->tl= effect; _temp3074;});
goto _LL3064; _LL3070: effect=({ struct Cyc_List_List* _temp3075=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3075->hd=( void*)(( void*)({
struct Cyc_Absyn_RgnsEff_struct* _temp3076=( struct Cyc_Absyn_RgnsEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnsEff_struct)); _temp3076[ 0]=({ struct
Cyc_Absyn_RgnsEff_struct _temp3077; _temp3077.tag= Cyc_Absyn_RgnsEff; _temp3077.f1=(
void*)(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd); _temp3077;});
_temp3076;})); _temp3075->tl= effect; _temp3075;}); goto _LL3064; _LL3064:;}}*
_temp2730=({ struct Cyc_Core_Opt* _temp3078=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp3078->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3079=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3079[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3080; _temp3080.tag= Cyc_Absyn_JoinEff;
_temp3080.f1= Cyc_List_imp_rev( effect); _temp3080;}); _temp3079;})); _temp3078;});}}
if(* _temp2733 !=  0){ struct Cyc_List_List* bs=* _temp2733; for( 0; bs !=  0;
bs=(( struct Cyc_List_List*) _check_null( bs))->tl){ struct Cyc_Absyn_Conref*
_temp3081= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( bs))->hd)->kind); void* _temp3082=( void*) _temp3081->v; void*
_temp3090; _LL3084: if( _temp3082 == ( void*) Cyc_Absyn_No_constr){ goto _LL3085;}
else{ goto _LL3086;} _LL3086: if(( unsigned int) _temp3082 >  1u?*(( int*)
_temp3082) ==  Cyc_Absyn_Eq_constr: 0){ _LL3091: _temp3090=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3082)->f1; if( _temp3090 == ( void*) Cyc_Absyn_MemKind){ goto _LL3087;}
else{ goto _LL3088;}} else{ goto _LL3088;} _LL3088: goto _LL3089; _LL3085:({
struct Cyc_std_String_pa_struct _temp3093; _temp3093.tag= Cyc_std_String_pa;
_temp3093.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( bs))->hd)->name;{ void* _temp3092[ 1u]={& _temp3093}; Cyc_Tcutil_warn(
loc, _tag_arr("Type variable %s unconstrained, assuming boxed", sizeof(
unsigned char), 47u), _tag_arr( _temp3092, sizeof( void*), 1u));}}); goto
_LL3087; _LL3087:( void*)( _temp3081->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3094=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3094[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3095; _temp3095.tag= Cyc_Absyn_Eq_constr;
_temp3095.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3095;}); _temp3094;})));
goto _LL3083; _LL3089: goto _LL3083; _LL3083:;}} cvtenv.kind_env= Cyc_Tcutil_remove_bound_tvars(
_temp2936.kind_env,* _temp2733); _temp2936.free_vars= Cyc_Tcutil_remove_bound_tvars(
_temp2936.free_vars,* _temp2733);{ struct Cyc_List_List* tvs= _temp2936.free_vars;
for( 0; tvs !=  0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd);}}{ struct Cyc_List_List* evs=
_temp2936.free_evars; for( 0; evs !=  0; evs=(( struct Cyc_List_List*)
_check_null( evs))->tl){ cvtenv.free_evars= Cyc_Tcutil_add_free_evar( cvtenv.free_evars,(
void*)(( struct Cyc_List_List*) _check_null( evs))->hd);}} goto _LL2620;}}
_LL2646: for( 0; _temp2734 !=  0; _temp2734=(( struct Cyc_List_List*)
_check_null( _temp2734))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te,
cvtenv,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp2734))->hd)).f2);} goto _LL2620; _LL2648:{ struct
_RegionHandle _temp3097= _new_region(); struct _RegionHandle* sprev_rgn=&
_temp3097; _push_region( sprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for(
0; _temp2736 !=  0; _temp2736=(( struct Cyc_List_List*) _check_null( _temp2736))->tl){
struct Cyc_Absyn_Structfield _temp3100; struct Cyc_List_List* _temp3101; struct
Cyc_Absyn_Exp* _temp3103; void* _temp3105; struct Cyc_Absyn_Tqual _temp3107;
struct _tagged_arr* _temp3109; struct Cyc_Absyn_Structfield* _temp3098=( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2736))->hd;
_temp3100=* _temp3098; _LL3110: _temp3109= _temp3100.name; goto _LL3108; _LL3108:
_temp3107= _temp3100.tq; goto _LL3106; _LL3106: _temp3105=( void*) _temp3100.type;
goto _LL3104; _LL3104: _temp3103= _temp3100.width; goto _LL3102; _LL3102:
_temp3101= _temp3100.attributes; goto _LL3099; _LL3099: if((( int(*)( int(*
compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l,
struct _tagged_arr* x)) Cyc_List_mem)( Cyc_std_zstrptrcmp, prev_fields,
_temp3109)){({ struct Cyc_std_String_pa_struct _temp3112; _temp3112.tag= Cyc_std_String_pa;
_temp3112.f1=( struct _tagged_arr)* _temp3109;{ void* _temp3111[ 1u]={&
_temp3112}; Cyc_Tcutil_terr( loc, _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u), _tag_arr( _temp3111, sizeof( void*), 1u));}});}
if( Cyc_std_strcmp(* _temp3109, _tag_arr("", sizeof( unsigned char), 1u)) !=  0){
prev_fields=({ struct Cyc_List_List* _temp3113=( struct Cyc_List_List*)
_region_malloc( sprev_rgn, sizeof( struct Cyc_List_List)); _temp3113->hd=( void*)
_temp3109; _temp3113->tl= prev_fields; _temp3113;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp3105); Cyc_Tcutil_check_bitfield(
loc, te, _temp3105, _temp3103, _temp3109); Cyc_Tcutil_check_field_atts( loc,
_temp3109, _temp3101);}}; _pop_region( sprev_rgn);} goto _LL2620; _LL2650:{
struct _RegionHandle _temp3114= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp3114; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for(
0; _temp2738 !=  0; _temp2738=(( struct Cyc_List_List*) _check_null( _temp2738))->tl){
struct Cyc_Absyn_Structfield _temp3117; struct Cyc_List_List* _temp3118; struct
Cyc_Absyn_Exp* _temp3120; void* _temp3122; struct Cyc_Absyn_Tqual _temp3124;
struct _tagged_arr* _temp3126; struct Cyc_Absyn_Structfield* _temp3115=( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp2738))->hd;
_temp3117=* _temp3115; _LL3127: _temp3126= _temp3117.name; goto _LL3125; _LL3125:
_temp3124= _temp3117.tq; goto _LL3123; _LL3123: _temp3122=( void*) _temp3117.type;
goto _LL3121; _LL3121: _temp3120= _temp3117.width; goto _LL3119; _LL3119:
_temp3118= _temp3117.attributes; goto _LL3116; _LL3116: if((( int(*)( int(*
compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l,
struct _tagged_arr* x)) Cyc_List_mem)( Cyc_std_zstrptrcmp, prev_fields,
_temp3126)){({ struct Cyc_std_String_pa_struct _temp3129; _temp3129.tag= Cyc_std_String_pa;
_temp3129.f1=( struct _tagged_arr)* _temp3126;{ void* _temp3128[ 1u]={&
_temp3129}; Cyc_Tcutil_terr( loc, _tag_arr("duplicate field %s in struct",
sizeof( unsigned char), 29u), _tag_arr( _temp3128, sizeof( void*), 1u));}});}
if( Cyc_std_strcmp(* _temp3126, _tag_arr("", sizeof( unsigned char), 1u)) !=  0){
prev_fields=({ struct Cyc_List_List* _temp3130=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp3130->hd=( void*)
_temp3126; _temp3130->tl= prev_fields; _temp3130;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp3122); if( ! Cyc_Tcutil_bits_only(
_temp3122)){({ struct Cyc_std_String_pa_struct _temp3132; _temp3132.tag= Cyc_std_String_pa;
_temp3132.f1=( struct _tagged_arr)* _temp3126;{ void* _temp3131[ 1u]={&
_temp3132}; Cyc_Tcutil_terr( loc, _tag_arr("%s has a type that is possibly incompatible with other members of union",
sizeof( unsigned char), 72u), _tag_arr( _temp3131, sizeof( void*), 1u));}});}
Cyc_Tcutil_check_bitfield( loc, te, _temp3122, _temp3120, _temp3126); Cyc_Tcutil_check_field_atts(
loc, _temp3126, _temp3118);}}; _pop_region( uprev_rgn);} goto _LL2620; _LL2652:
if( _temp2745 ==  0){ if(* _temp2742 ==  0){({ void* _temp3133[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bad anonymous struct", sizeof( unsigned char), 21u), _tag_arr(
_temp3133, sizeof( void*), 0u));}); return cvtenv;}{ struct Cyc_Absyn_Structdecl*
sd=*(( struct Cyc_Absyn_Structdecl**) _check_null(* _temp2742));;}} if(*
_temp2742 ==  0){ struct _handler_cons _temp3134; _push_handler(& _temp3134);{
int _temp3136= 0; if( setjmp( _temp3134.handler)){ _temp3136= 1;} if( !
_temp3136){* _temp2742=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc,( struct _tuple1*) _check_null( _temp2745));; _pop_handler();} else{
void* _temp3135=( void*) _exn_thrown; void* _temp3138= _temp3135; _LL3140: if(
_temp3138 ==  Cyc_Dict_Absent){ goto _LL3141;} else{ goto _LL3142;} _LL3142:
goto _LL3143; _LL3141:({ struct Cyc_std_String_pa_struct _temp3145; _temp3145.tag=
Cyc_std_String_pa; _temp3145.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp2745));{ void* _temp3144[ 1u]={& _temp3145};
Cyc_Tcutil_terr( loc, _tag_arr("unbound type struct %s", sizeof( unsigned char),
23u), _tag_arr( _temp3144, sizeof( void*), 1u));}}); return cvtenv; _LL3143:(
void) _throw( _temp3138); _LL3139:;}}}{ struct Cyc_Absyn_Structdecl* sd=*((
struct Cyc_Absyn_Structdecl**) _check_null(* _temp2742)); if( sd->name !=  0){*((
struct _tuple1*) _check_null( _temp2745))=(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v)[ _check_known_subscript_notnull( 1u, 0)];}{ struct
Cyc_List_List* tvs= sd->tvs; int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
tvs); int largs= Cyc_List_length( _temp2743); if( lvs !=  largs){({ struct Cyc_std_Int_pa_struct
_temp3149; _temp3149.tag= Cyc_std_Int_pa; _temp3149.f1=( int)(( unsigned int)
largs);{ struct Cyc_std_Int_pa_struct _temp3148; _temp3148.tag= Cyc_std_Int_pa;
_temp3148.f1=( int)(( unsigned int) lvs);{ struct Cyc_std_String_pa_struct
_temp3147; _temp3147.tag= Cyc_std_String_pa; _temp3147.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp2745));{ void*
_temp3146[ 3u]={& _temp3147,& _temp3148,& _temp3149}; Cyc_Tcutil_terr( loc,
_tag_arr("struct %s expects %d type arguments but was given %d", sizeof(
unsigned char), 53u), _tag_arr( _temp3146, sizeof( void*), 3u));}}}});} for( 0;
_temp2743 !=  0;( _temp2743=(( struct Cyc_List_List*) _check_null( _temp2743))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2743))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} goto _LL2620;}}
_LL2654: if( _temp2752 ==  0){({ void* _temp3150[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("missing union name", sizeof( unsigned char), 19u), _tag_arr( _temp3150,
sizeof( void*), 0u));}); return cvtenv;} if(* _temp2749 ==  0){ struct
_handler_cons _temp3151; _push_handler(& _temp3151);{ int _temp3153= 0; if(
setjmp( _temp3151.handler)){ _temp3153= 1;} if( ! _temp3153){* _temp2749=(
struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl( te, loc,( struct
_tuple1*) _check_null( _temp2752));; _pop_handler();} else{ void* _temp3152=(
void*) _exn_thrown; void* _temp3155= _temp3152; _LL3157: if( _temp3155 ==  Cyc_Dict_Absent){
goto _LL3158;} else{ goto _LL3159;} _LL3159: goto _LL3160; _LL3158:({ struct Cyc_std_String_pa_struct
_temp3162; _temp3162.tag= Cyc_std_String_pa; _temp3162.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp2752));{ void*
_temp3161[ 1u]={& _temp3162}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type union %s",
sizeof( unsigned char), 22u), _tag_arr( _temp3161, sizeof( void*), 1u));}});
return cvtenv; _LL3160:( void) _throw( _temp3155); _LL3156:;}}}{ struct Cyc_Absyn_Uniondecl*
ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null(* _temp2749)); if( ud->name != 
0){*(( struct _tuple1*) _check_null( _temp2752))=(( struct _tuple1*)(( struct
Cyc_Core_Opt*) _check_null( ud->name))->v)[ _check_known_subscript_notnull( 1u,
0)];}{ struct Cyc_List_List* tvs= ud->tvs; int lvs=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( tvs); int largs= Cyc_List_length( _temp2750); if( lvs != 
largs){({ struct Cyc_std_Int_pa_struct _temp3166; _temp3166.tag= Cyc_std_Int_pa;
_temp3166.f1=( int)(( unsigned int) largs);{ struct Cyc_std_Int_pa_struct
_temp3165; _temp3165.tag= Cyc_std_Int_pa; _temp3165.f1=( int)(( unsigned int)
lvs);{ struct Cyc_std_String_pa_struct _temp3164; _temp3164.tag= Cyc_std_String_pa;
_temp3164.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(( struct _tuple1*)
_check_null( _temp2752));{ void* _temp3163[ 3u]={& _temp3164,& _temp3165,&
_temp3166}; Cyc_Tcutil_terr( loc, _tag_arr("union %s expects %d type arguments but was given %d",
sizeof( unsigned char), 52u), _tag_arr( _temp3163, sizeof( void*), 3u));}}}});}
for( 0; _temp2750 !=  0;( _temp2750=(( struct Cyc_List_List*) _check_null(
_temp2750))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2750))->hd; void* k1= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k1, t1);} goto _LL2620;}}
_LL2656: { struct Cyc_Absyn_Typedefdecl* td;{ struct _handler_cons _temp3167;
_push_handler(& _temp3167);{ int _temp3169= 0; if( setjmp( _temp3167.handler)){
_temp3169= 1;} if( ! _temp3169){ td= Cyc_Tcenv_lookup_typedefdecl( te, loc,
_temp2759);; _pop_handler();} else{ void* _temp3168=( void*) _exn_thrown; void*
_temp3171= _temp3168; _LL3173: if( _temp3171 ==  Cyc_Dict_Absent){ goto _LL3174;}
else{ goto _LL3175;} _LL3175: goto _LL3176; _LL3174:({ struct Cyc_std_String_pa_struct
_temp3178; _temp3178.tag= Cyc_std_String_pa; _temp3178.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2759);{ void* _temp3177[ 1u]={& _temp3178}; Cyc_Tcutil_terr(
loc, _tag_arr("unbound typedef name %s", sizeof( unsigned char), 24u), _tag_arr(
_temp3177, sizeof( void*), 1u));}}); return cvtenv; _LL3176:( void) _throw(
_temp3171); _LL3172:;}}} _temp2759[ _check_known_subscript_notnull( 1u, 0)]=( td->name)[
_check_known_subscript_notnull( 1u, 0)];{ struct Cyc_List_List* tvs= td->tvs;
struct Cyc_List_List* ts= _temp2757; struct Cyc_List_List* inst= 0; for( 0; ts
!=  0? tvs !=  0: 0;( ts=(( struct Cyc_List_List*) _check_null( ts))->tl, tvs=((
struct Cyc_List_List*) _check_null( tvs))->tl)){ void* k= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv, k,( void*)(( struct Cyc_List_List*)
_check_null( ts))->hd); inst=({ struct Cyc_List_List* _temp3179=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3179->hd=( void*)({ struct
_tuple5* _temp3180=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp3180->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
tvs))->hd; _temp3180->f2=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd;
_temp3180;}); _temp3179->tl= inst; _temp3179;});} if( ts !=  0){({ struct Cyc_std_String_pa_struct
_temp3182; _temp3182.tag= Cyc_std_String_pa; _temp3182.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( _temp2759);{ void* _temp3181[ 1u]={& _temp3182}; Cyc_Tcutil_terr(
loc, _tag_arr("too many parameters for typedef %s", sizeof( unsigned char), 35u),
_tag_arr( _temp3181, sizeof( void*), 1u));}});} if( tvs !=  0){ for( 0; tvs != 
0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ void* k= Cyc_Absyn_conref_val(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
void* e= Cyc_Absyn_new_evar( 0, 0); cvtenv= Cyc_Tcutil_i_check_valid_type( loc,
te, cvtenv, k, e); inst=({ struct Cyc_List_List* _temp3183=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3183->hd=( void*)({ struct
_tuple5* _temp3184=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp3184->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
tvs))->hd; _temp3184->f2= e; _temp3184;}); _temp3183->tl= inst; _temp3183;});}}{
void* new_typ= Cyc_Tcutil_substitute( inst,( void*) td->defn);* _temp2756=({
struct Cyc_Core_Opt* _temp3185=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3185->v=( void*) new_typ; _temp3185;}); goto _LL2620;}}}
_LL2658: goto _LL2620; _LL2660: _temp2763= _temp2761; goto _LL2662; _LL2662:
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2763); goto _LL2620; _LL2664: cvtenv= Cyc_Tcutil_i_check_valid_type( loc,
te, cvtenv,( void*) Cyc_Absyn_AnyKind, _temp2765); goto _LL2620; _LL2666: for( 0;
_temp2767 !=  0; _temp2767=(( struct Cyc_List_List*) _check_null( _temp2767))->tl){
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_EffKind,(
void*)(( struct Cyc_List_List*) _check_null( _temp2767))->hd);} goto _LL2620;
_LL2620:;} if( ! Cyc_Tcutil_kind_leq( Cyc_Tcutil_typ_kind( t), expected_kind)){({
struct Cyc_std_String_pa_struct _temp3189; _temp3189.tag= Cyc_std_String_pa;
_temp3189.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( expected_kind);{
struct Cyc_std_String_pa_struct _temp3188; _temp3188.tag= Cyc_std_String_pa;
_temp3188.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( Cyc_Tcutil_typ_kind(
t));{ struct Cyc_std_String_pa_struct _temp3187; _temp3187.tag= Cyc_std_String_pa;
_temp3187.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp3186[
3u]={& _temp3187,& _temp3188,& _temp3189}; Cyc_Tcutil_terr( loc, _tag_arr("type %s has kind %s but as used here needs kind %s",
sizeof( unsigned char), 51u), _tag_arr( _temp3186, sizeof( void*), 3u));}}}});}
return cvtenv;} static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
kind_env, void* expected_kind, int generalize_evars, void* t){ struct Cyc_Tcutil_CVTEnv
_temp3190= Cyc_Tcutil_i_check_valid_type( loc, te,({ struct Cyc_Tcutil_CVTEnv
_temp3205; _temp3205.kind_env= kind_env; _temp3205.free_vars= 0; _temp3205.free_evars=
0; _temp3205.generalize_evars= generalize_evars; _temp3205.fn_result= 0;
_temp3205;}), expected_kind, t);{ struct Cyc_List_List* vs= _temp3190.free_vars;
for( 0; vs !=  0; vs=(( struct Cyc_List_List*) _check_null( vs))->tl){ _temp3190.kind_env=
Cyc_Tcutil_fast_add_free_tvar( kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( vs))->hd);}}{ struct Cyc_List_List* evs= _temp3190.free_evars; for(
0; evs !=  0; evs=(( struct Cyc_List_List*) _check_null( evs))->tl){ void*
_temp3191= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*) _check_null(
evs))->hd); struct Cyc_Core_Opt* _temp3197; struct Cyc_Core_Opt** _temp3199;
_LL3193: if(( unsigned int) _temp3191 >  4u?*(( int*) _temp3191) ==  Cyc_Absyn_Evar:
0){ _LL3198: _temp3197=(( struct Cyc_Absyn_Evar_struct*) _temp3191)->f4;
_temp3199=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Evar_struct*) _temp3191)->f4;
goto _LL3194;} else{ goto _LL3195;} _LL3195: goto _LL3196; _LL3194: if(*
_temp3199 ==  0){* _temp3199=({ struct Cyc_Core_Opt* _temp3200=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3200->v=( void*) kind_env;
_temp3200;});} else{ struct Cyc_List_List* _temp3201=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(* _temp3199))->v; struct Cyc_List_List*
_temp3202= 0; for( 0; _temp3201 !=  0; _temp3201=(( struct Cyc_List_List*)
_check_null( _temp3201))->tl){ if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp3201))->hd)){ _temp3202=({ struct Cyc_List_List* _temp3203=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3203->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp3201))->hd);
_temp3203->tl= _temp3202; _temp3203;});}}* _temp3199=({ struct Cyc_Core_Opt*
_temp3204=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3204->v=( void*) _temp3202; _temp3204;});} goto _LL3192; _LL3196: goto
_LL3192; _LL3192:;}} return _temp3190;} void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, void* t){ int
generalize_evars= Cyc_Tcutil_is_function_type( t); struct Cyc_Tcutil_CVTEnv
_temp3206= Cyc_Tcutil_check_valid_type( loc, te, 0,( void*) Cyc_Absyn_MemKind,
generalize_evars, t); struct Cyc_List_List* _temp3207= _temp3206.free_vars;
struct Cyc_List_List* _temp3208= _temp3206.free_evars;{ struct Cyc_List_List* x=
_temp3207; for( 0; x !=  0; x=(( struct Cyc_List_List*) _check_null( x))->tl){
struct Cyc_Absyn_Conref* c= Cyc_Absyn_compress_conref((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( x))->hd)->kind); void* _temp3209=( void*) c->v;
void* _temp3217; _LL3211: if( _temp3209 == ( void*) Cyc_Absyn_No_constr){ goto
_LL3212;} else{ goto _LL3213;} _LL3213: if(( unsigned int) _temp3209 >  1u?*((
int*) _temp3209) ==  Cyc_Absyn_Eq_constr: 0){ _LL3218: _temp3217=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp3209)->f1; if( _temp3217 == ( void*)
Cyc_Absyn_MemKind){ goto _LL3214;} else{ goto _LL3215;}} else{ goto _LL3215;}
_LL3215: goto _LL3216; _LL3212: goto _LL3214; _LL3214:( void*)( c->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3219=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3219[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3220; _temp3220.tag= Cyc_Absyn_Eq_constr;
_temp3220.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3220;}); _temp3219;})));
goto _LL3210; _LL3216: goto _LL3210; _LL3210:;}} if( _temp3207 !=  0? 1:
_temp3208 !=  0){{ void* _temp3221= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo
_temp3227; struct Cyc_List_List* _temp3229; struct Cyc_List_List* _temp3231;
struct Cyc_Absyn_VarargInfo* _temp3233; int _temp3235; struct Cyc_List_List*
_temp3237; void* _temp3239; struct Cyc_Core_Opt* _temp3241; struct Cyc_List_List*
_temp3243; struct Cyc_List_List** _temp3245; _LL3223: if(( unsigned int)
_temp3221 >  4u?*(( int*) _temp3221) ==  Cyc_Absyn_FnType: 0){ _LL3228:
_temp3227=(( struct Cyc_Absyn_FnType_struct*) _temp3221)->f1; _LL3244: _temp3243=
_temp3227.tvars; _temp3245=( struct Cyc_List_List**)&((( struct Cyc_Absyn_FnType_struct*)
_temp3221)->f1).tvars; goto _LL3242; _LL3242: _temp3241= _temp3227.effect; goto
_LL3240; _LL3240: _temp3239=( void*) _temp3227.ret_typ; goto _LL3238; _LL3238:
_temp3237= _temp3227.args; goto _LL3236; _LL3236: _temp3235= _temp3227.c_varargs;
goto _LL3234; _LL3234: _temp3233= _temp3227.cyc_varargs; goto _LL3232; _LL3232:
_temp3231= _temp3227.rgn_po; goto _LL3230; _LL3230: _temp3229= _temp3227.attributes;
goto _LL3224;} else{ goto _LL3225;} _LL3225: goto _LL3226; _LL3224: if(*
_temp3245 ==  0){* _temp3245= _temp3207; _temp3207= 0;} goto _LL3222; _LL3226:
goto _LL3222; _LL3222:;} if( _temp3207 !=  0){({ struct Cyc_std_String_pa_struct
_temp3247; _temp3247.tag= Cyc_std_String_pa; _temp3247.f1=( struct _tagged_arr)*((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp3207))->hd)->name;{
void* _temp3246[ 1u]={& _temp3247}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type variable %s",
sizeof( unsigned char), 25u), _tag_arr( _temp3246, sizeof( void*), 1u));}});}
if( _temp3208 !=  0){ for( 0; _temp3208 !=  0; _temp3208=(( struct Cyc_List_List*)
_check_null( _temp3208))->tl){ void* e=( void*)(( struct Cyc_List_List*)
_check_null( _temp3208))->hd; void* _temp3248= Cyc_Tcutil_typ_kind( e); _LL3250:
if( _temp3248 == ( void*) Cyc_Absyn_RgnKind){ goto _LL3251;} else{ goto _LL3252;}
_LL3252: if( _temp3248 == ( void*) Cyc_Absyn_EffKind){ goto _LL3253;} else{ goto
_LL3254;} _LL3254: goto _LL3255; _LL3251: if( ! Cyc_Tcutil_unify( e,( void*) Cyc_Absyn_HeapRgn)){({
void* _temp3256[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("can't unify evar with heap!", sizeof( unsigned char),
28u), _tag_arr( _temp3256, sizeof( void*), 0u));});} goto _LL3249; _LL3253: if(
! Cyc_Tcutil_unify( e, Cyc_Absyn_empty_effect)){({ void* _temp3257[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("can't unify evar with {}!", sizeof( unsigned char), 26u), _tag_arr(
_temp3257, sizeof( void*), 0u));});} goto _LL3249; _LL3255:({ struct Cyc_std_String_pa_struct
_temp3260; _temp3260.tag= Cyc_std_String_pa; _temp3260.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ struct Cyc_std_String_pa_struct _temp3259;
_temp3259.tag= Cyc_std_String_pa; _temp3259.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
e);{ void* _temp3258[ 2u]={& _temp3259,& _temp3260}; Cyc_Tcutil_terr( loc,
_tag_arr("hidden type variable %s isn't abstracted in type %s", sizeof(
unsigned char), 52u), _tag_arr( _temp3258, sizeof( void*), 2u));}}}); goto
_LL3249; _LL3249:;}}}} void Cyc_Tcutil_check_fndecl_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl* fd){ void* t= Cyc_Tcutil_fndecl2typ(
fd); Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);{ void* _temp3261= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_FnInfo _temp3267; struct Cyc_List_List* _temp3269; struct
Cyc_List_List* _temp3271; struct Cyc_Absyn_VarargInfo* _temp3273; int _temp3275;
struct Cyc_List_List* _temp3277; void* _temp3279; struct Cyc_Core_Opt* _temp3281;
struct Cyc_List_List* _temp3283; _LL3263: if(( unsigned int) _temp3261 >  4u?*((
int*) _temp3261) ==  Cyc_Absyn_FnType: 0){ _LL3268: _temp3267=(( struct Cyc_Absyn_FnType_struct*)
_temp3261)->f1; _LL3284: _temp3283= _temp3267.tvars; goto _LL3282; _LL3282:
_temp3281= _temp3267.effect; goto _LL3280; _LL3280: _temp3279=( void*) _temp3267.ret_typ;
goto _LL3278; _LL3278: _temp3277= _temp3267.args; goto _LL3276; _LL3276:
_temp3275= _temp3267.c_varargs; goto _LL3274; _LL3274: _temp3273= _temp3267.cyc_varargs;
goto _LL3272; _LL3272: _temp3271= _temp3267.rgn_po; goto _LL3270; _LL3270:
_temp3269= _temp3267.attributes; goto _LL3264;} else{ goto _LL3265;} _LL3265:
goto _LL3266; _LL3264: fd->tvs= _temp3283; fd->effect= _temp3281; goto _LL3262;
_LL3266:({ void* _temp3285[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("check_fndecl_valid_type: not a FnType",
sizeof( unsigned char), 38u), _tag_arr( _temp3285, sizeof( void*), 0u));});
return; _LL3262:;}{ struct _RegionHandle _temp3286= _new_region(); struct
_RegionHandle* r=& _temp3286; _push_region( r); Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tagged_arr*(* f)(
struct _tuple13*), struct Cyc_List_List* x)) Cyc_List_rmap)( r,( struct
_tagged_arr*(*)( struct _tuple13* t)) Cyc_Tcutil_fst_fdarg, fd->args), loc,
_tag_arr("function declaration has repeated parameter", sizeof( unsigned char),
44u));; _pop_region( r);} fd->cached_typ=({ struct Cyc_Core_Opt* _temp3287=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3287->v=(
void*) t; _temp3287;});} void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* bound_tvars, void*
expected_kind, int allow_evars, void* t){ struct Cyc_Tcutil_CVTEnv _temp3288=
Cyc_Tcutil_check_valid_type( loc, te, bound_tvars, expected_kind, 0, t); struct
Cyc_List_List* _temp3289= Cyc_Tcutil_remove_bound_tvars( _temp3288.free_vars,
bound_tvars); struct Cyc_List_List* _temp3290= _temp3288.free_evars;{ struct Cyc_List_List*
fs= _temp3289; for( 0; fs !=  0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){
struct _tagged_arr* _temp3291=(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->name;({ struct Cyc_std_String_pa_struct _temp3294;
_temp3294.tag= Cyc_std_String_pa; _temp3294.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ struct Cyc_std_String_pa_struct _temp3293; _temp3293.tag= Cyc_std_String_pa;
_temp3293.f1=( struct _tagged_arr)* _temp3291;{ void* _temp3292[ 2u]={&
_temp3293,& _temp3294}; Cyc_Tcutil_terr( loc, _tag_arr("unbound type variable %s in type %s",
sizeof( unsigned char), 36u), _tag_arr( _temp3292, sizeof( void*), 2u));}}});}}
if( ! allow_evars? _temp3290 !=  0: 0){ for( 0; _temp3290 !=  0; _temp3290=((
struct Cyc_List_List*) _check_null( _temp3290))->tl){ void* e=( void*)(( struct
Cyc_List_List*) _check_null( _temp3290))->hd; void* _temp3295= Cyc_Tcutil_typ_kind(
e); _LL3297: if( _temp3295 == ( void*) Cyc_Absyn_RgnKind){ goto _LL3298;} else{
goto _LL3299;} _LL3299: if( _temp3295 == ( void*) Cyc_Absyn_EffKind){ goto
_LL3300;} else{ goto _LL3301;} _LL3301: goto _LL3302; _LL3298: if( ! Cyc_Tcutil_unify(
e,( void*) Cyc_Absyn_HeapRgn)){({ void* _temp3303[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("can't unify evar with heap!",
sizeof( unsigned char), 28u), _tag_arr( _temp3303, sizeof( void*), 0u));});}
goto _LL3296; _LL3300: if( ! Cyc_Tcutil_unify( e,( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3304=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3304[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3305; _temp3305.tag= Cyc_Absyn_JoinEff;
_temp3305.f1= 0; _temp3305;}); _temp3304;}))){({ void* _temp3306[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("can't unify evar with {}!",
sizeof( unsigned char), 26u), _tag_arr( _temp3306, sizeof( void*), 0u));});}
goto _LL3296; _LL3302:({ struct Cyc_std_String_pa_struct _temp3309; _temp3309.tag=
Cyc_std_String_pa; _temp3309.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
struct Cyc_std_String_pa_struct _temp3308; _temp3308.tag= Cyc_std_String_pa;
_temp3308.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( e);{ void* _temp3307[
2u]={& _temp3308,& _temp3309}; Cyc_Tcutil_terr( loc, _tag_arr("hidden type variable %s isn't abstracted in type %s",
sizeof( unsigned char), 52u), _tag_arr( _temp3307, sizeof( void*), 2u));}}});
goto _LL3296; _LL3296:;}}} void Cyc_Tcutil_add_tvar_identity( struct Cyc_Absyn_Tvar*
tv){ if( tv->identity ==  0){ tv->identity= Cyc_Tcutil_new_tvar_id();}} void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List* tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Tcutil_add_tvar_identity, tvs);}
static void Cyc_Tcutil_check_unique_unsorted( int(* cmp)( void*, void*), struct
Cyc_List_List* vs, struct Cyc_Position_Segment* loc, struct _tagged_arr(*
a2string)( void*), struct _tagged_arr msg){ for( 0; vs !=  0; vs=(( struct Cyc_List_List*)
_check_null( vs))->tl){ struct Cyc_List_List* vs2=(( struct Cyc_List_List*)
_check_null( vs))->tl; for( 0; vs2 !=  0; vs2=(( struct Cyc_List_List*)
_check_null( vs2))->tl){ if( cmp(( void*)(( struct Cyc_List_List*) _check_null(
vs))->hd,( void*)(( struct Cyc_List_List*) _check_null( vs2))->hd) ==  0){({
struct Cyc_std_String_pa_struct _temp3312; _temp3312.tag= Cyc_std_String_pa;
_temp3312.f1=( struct _tagged_arr) a2string(( void*)(( struct Cyc_List_List*)
_check_null( vs))->hd);{ struct Cyc_std_String_pa_struct _temp3311; _temp3311.tag=
Cyc_std_String_pa; _temp3311.f1=( struct _tagged_arr) msg;{ void* _temp3310[ 2u]={&
_temp3311,& _temp3312}; Cyc_Tcutil_terr( loc, _tag_arr("%s: %s", sizeof(
unsigned char), 7u), _tag_arr( _temp3310, sizeof( void*), 2u));}}});}}}} static
struct _tagged_arr Cyc_Tcutil_strptr2string( struct _tagged_arr* s){ return* s;}
void Cyc_Tcutil_check_unique_vars( struct Cyc_List_List* vs, struct Cyc_Position_Segment*
loc, struct _tagged_arr msg){(( void(*)( int(* cmp)( struct _tagged_arr*, struct
_tagged_arr*), struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc,
struct _tagged_arr(* a2string)( struct _tagged_arr*), struct _tagged_arr msg))
Cyc_Tcutil_check_unique_unsorted)( Cyc_std_zstrptrcmp, vs, loc, Cyc_Tcutil_strptr2string,
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
Cyc_List_List* sd_fields= sdfields; for( 0; sd_fields !=  0; sd_fields=(( struct
Cyc_List_List*) _check_null( sd_fields))->tl){ if( Cyc_std_strcmp(*(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( sd_fields))->hd)->name, _tag_arr("", sizeof(
unsigned char), 1u)) !=  0){ fields=({ struct Cyc_List_List* _temp3313=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3313->hd=( void*)({
struct _tuple17* _temp3314=( struct _tuple17*) GC_malloc( sizeof( struct
_tuple17)); _temp3314->f1=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( sd_fields))->hd; _temp3314->f2= 0; _temp3314;}); _temp3313->tl=
fields; _temp3313;});}}} fields=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( fields);{ struct Cyc_List_List* ans= 0; for( 0; des !=  0;
des=(( struct Cyc_List_List*) _check_null( des))->tl){ struct _tuple18 _temp3317;
void* _temp3318; struct Cyc_List_List* _temp3320; struct _tuple18* _temp3315=(
struct _tuple18*)(( struct Cyc_List_List*) _check_null( des))->hd; _temp3317=*
_temp3315; _LL3321: _temp3320= _temp3317.f1; goto _LL3319; _LL3319: _temp3318=
_temp3317.f2; goto _LL3316; _LL3316: if( _temp3320 ==  0){ struct Cyc_List_List*
_temp3322= fields; for( 0; _temp3322 !=  0; _temp3322=(( struct Cyc_List_List*)
_check_null( _temp3322))->tl){ if( !(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3322))->hd)).f2){(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3322))->hd)).f2= 1;(*(( struct _tuple18*)(( struct Cyc_List_List*)
_check_null( des))->hd)).f1=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp3323=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3323->hd=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp3324=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp3324[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp3325; _temp3325.tag= Cyc_Absyn_FieldName;
_temp3325.f1=((*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3322))->hd)).f1)->name; _temp3325;}); _temp3324;})); _temp3323->tl= 0;
_temp3323;}); ans=({ struct Cyc_List_List* _temp3326=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp3326->hd=( void*)({
struct _tuple19* _temp3327=( struct _tuple19*) _region_malloc( rgn, sizeof(
struct _tuple19)); _temp3327->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3322))->hd)).f1; _temp3327->f2= _temp3318; _temp3327;});
_temp3326->tl= ans; _temp3326;}); break;}} if( _temp3322 ==  0){({ void*
_temp3328[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("too many arguments to struct",
sizeof( unsigned char), 29u), _tag_arr( _temp3328, sizeof( void*), 0u));});}}
else{ if((( struct Cyc_List_List*) _check_null( _temp3320))->tl !=  0){({ void*
_temp3329[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("multiple designators are not supported",
sizeof( unsigned char), 39u), _tag_arr( _temp3329, sizeof( void*), 0u));});}
else{ void* _temp3330=( void*)(( struct Cyc_List_List*) _check_null( _temp3320))->hd;
struct _tagged_arr* _temp3336; _LL3332: if(*(( int*) _temp3330) ==  Cyc_Absyn_ArrayElement){
goto _LL3333;} else{ goto _LL3334;} _LL3334: if(*(( int*) _temp3330) ==  Cyc_Absyn_FieldName){
_LL3337: _temp3336=(( struct Cyc_Absyn_FieldName_struct*) _temp3330)->f1; goto
_LL3335;} else{ goto _LL3331;} _LL3333:({ void* _temp3338[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("array designator used in argument to struct", sizeof(
unsigned char), 44u), _tag_arr( _temp3338, sizeof( void*), 0u));}); goto _LL3331;
_LL3335: { struct Cyc_List_List* _temp3339= fields; for( 0; _temp3339 !=  0;
_temp3339=(( struct Cyc_List_List*) _check_null( _temp3339))->tl){ if( Cyc_std_zstrptrcmp(
_temp3336,((*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3339))->hd)).f1)->name) ==  0){ if((*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3339))->hd)).f2){({ struct Cyc_std_String_pa_struct _temp3341;
_temp3341.tag= Cyc_std_String_pa; _temp3341.f1=( struct _tagged_arr)* _temp3336;{
void* _temp3340[ 1u]={& _temp3341}; Cyc_Tcutil_terr( loc, _tag_arr("field %s has already been used as an argument",
sizeof( unsigned char), 46u), _tag_arr( _temp3340, sizeof( void*), 1u));}});}(*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp3339))->hd)).f2= 1;
ans=({ struct Cyc_List_List* _temp3342=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp3342->hd=( void*)({ struct _tuple19*
_temp3343=( struct _tuple19*) _region_malloc( rgn, sizeof( struct _tuple19));
_temp3343->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3339))->hd)).f1; _temp3343->f2= _temp3318; _temp3343;}); _temp3342->tl= ans;
_temp3342;}); break;}} if( _temp3339 ==  0){({ struct Cyc_std_String_pa_struct
_temp3345; _temp3345.tag= Cyc_std_String_pa; _temp3345.f1=( struct _tagged_arr)*
_temp3336;{ void* _temp3344[ 1u]={& _temp3345}; Cyc_Tcutil_terr( loc, _tag_arr("bad field designator %s",
sizeof( unsigned char), 24u), _tag_arr( _temp3344, sizeof( void*), 1u));}});}
goto _LL3331;} _LL3331:;}}} for( 0; fields !=  0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ if( !(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( fields))->hd)).f2){({ void* _temp3346[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("too few arguments to struct", sizeof( unsigned char), 28u), _tag_arr(
_temp3346, sizeof( void*), 0u));}); break;}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( ans);}} int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void* t, void** elt_typ_dest){ void* _temp3347= Cyc_Tcutil_compress( t); struct
Cyc_Absyn_PtrInfo _temp3353; struct Cyc_Absyn_Conref* _temp3355; void* _temp3357;
_LL3349: if(( unsigned int) _temp3347 >  4u?*(( int*) _temp3347) ==  Cyc_Absyn_PointerType:
0){ _LL3354: _temp3353=(( struct Cyc_Absyn_PointerType_struct*) _temp3347)->f1;
_LL3358: _temp3357=( void*) _temp3353.elt_typ; goto _LL3356; _LL3356: _temp3355=
_temp3353.bounds; goto _LL3350;} else{ goto _LL3351;} _LL3351: goto _LL3352;
_LL3350: { struct Cyc_Absyn_Conref* _temp3359= Cyc_Absyn_compress_conref(
_temp3355); void* _temp3360=( void*)( Cyc_Absyn_compress_conref( _temp3359))->v;
void* _temp3368; _LL3362: if(( unsigned int) _temp3360 >  1u?*(( int*) _temp3360)
==  Cyc_Absyn_Eq_constr: 0){ _LL3369: _temp3368=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3360)->f1; if( _temp3368 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL3363;}
else{ goto _LL3364;}} else{ goto _LL3364;} _LL3364: if( _temp3360 == ( void*)
Cyc_Absyn_No_constr){ goto _LL3365;} else{ goto _LL3366;} _LL3366: goto _LL3367;
_LL3363:* elt_typ_dest= _temp3357; return 1; _LL3365:( void*)( _temp3359->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3370=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3370[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3371; _temp3371.tag= Cyc_Absyn_Eq_constr;
_temp3371.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp3371;}); _temp3370;})));*
elt_typ_dest= _temp3357; return 1; _LL3367: return 0; _LL3361:;} _LL3352: return
0; _LL3348:;} int Cyc_Tcutil_is_tagged_pointer_typ( void* t){ void* ignore=(
void*) Cyc_Absyn_VoidType; return Cyc_Tcutil_is_tagged_pointer_typ_elt( t,&
ignore);} struct _tuple7 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ struct _tuple7 bogus_ans=({ struct _tuple7 _temp3554;
_temp3554.f1= 0; _temp3554.f2=( void*) Cyc_Absyn_HeapRgn; _temp3554;}); void*
_temp3372=( void*) e->r; void* _temp3386; struct _tuple1* _temp3388; struct
_tagged_arr* _temp3390; struct Cyc_Absyn_Exp* _temp3392; struct _tagged_arr*
_temp3394; struct Cyc_Absyn_Exp* _temp3396; struct Cyc_Absyn_Exp* _temp3398;
struct Cyc_Absyn_Exp* _temp3400; struct Cyc_Absyn_Exp* _temp3402; _LL3374: if(*((
int*) _temp3372) ==  Cyc_Absyn_Var_e){ _LL3389: _temp3388=(( struct Cyc_Absyn_Var_e_struct*)
_temp3372)->f1; goto _LL3387; _LL3387: _temp3386=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3372)->f2; goto _LL3375;} else{ goto _LL3376;} _LL3376: if(*(( int*)
_temp3372) ==  Cyc_Absyn_StructMember_e){ _LL3393: _temp3392=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3372)->f1; goto _LL3391; _LL3391: _temp3390=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3372)->f2; goto _LL3377;} else{ goto _LL3378;} _LL3378: if(*(( int*)
_temp3372) ==  Cyc_Absyn_StructArrow_e){ _LL3397: _temp3396=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3372)->f1; goto _LL3395; _LL3395: _temp3394=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3372)->f2; goto _LL3379;} else{ goto _LL3380;} _LL3380: if(*(( int*)
_temp3372) ==  Cyc_Absyn_Deref_e){ _LL3399: _temp3398=(( struct Cyc_Absyn_Deref_e_struct*)
_temp3372)->f1; goto _LL3381;} else{ goto _LL3382;} _LL3382: if(*(( int*)
_temp3372) ==  Cyc_Absyn_Subscript_e){ _LL3403: _temp3402=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3372)->f1; goto _LL3401; _LL3401: _temp3400=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3372)->f2; goto _LL3383;} else{ goto _LL3384;} _LL3384: goto _LL3385;
_LL3375: { void* _temp3404= _temp3386; struct Cyc_Absyn_Vardecl* _temp3418;
struct Cyc_Absyn_Vardecl* _temp3420; struct Cyc_Absyn_Vardecl* _temp3422; struct
Cyc_Absyn_Vardecl* _temp3424; _LL3406: if( _temp3404 == ( void*) Cyc_Absyn_Unresolved_b){
goto _LL3407;} else{ goto _LL3408;} _LL3408: if(( unsigned int) _temp3404 >  1u?*((
int*) _temp3404) ==  Cyc_Absyn_Funname_b: 0){ goto _LL3409;} else{ goto _LL3410;}
_LL3410: if(( unsigned int) _temp3404 >  1u?*(( int*) _temp3404) ==  Cyc_Absyn_Global_b:
0){ _LL3419: _temp3418=(( struct Cyc_Absyn_Global_b_struct*) _temp3404)->f1;
goto _LL3411;} else{ goto _LL3412;} _LL3412: if(( unsigned int) _temp3404 >  1u?*((
int*) _temp3404) ==  Cyc_Absyn_Local_b: 0){ _LL3421: _temp3420=(( struct Cyc_Absyn_Local_b_struct*)
_temp3404)->f1; goto _LL3413;} else{ goto _LL3414;} _LL3414: if(( unsigned int)
_temp3404 >  1u?*(( int*) _temp3404) ==  Cyc_Absyn_Pat_b: 0){ _LL3423: _temp3422=((
struct Cyc_Absyn_Pat_b_struct*) _temp3404)->f1; goto _LL3415;} else{ goto
_LL3416;} _LL3416: if(( unsigned int) _temp3404 >  1u?*(( int*) _temp3404) == 
Cyc_Absyn_Param_b: 0){ _LL3425: _temp3424=(( struct Cyc_Absyn_Param_b_struct*)
_temp3404)->f1; goto _LL3417;} else{ goto _LL3405;} _LL3407: return bogus_ans;
_LL3409: return({ struct _tuple7 _temp3426; _temp3426.f1= 0; _temp3426.f2=( void*)
Cyc_Absyn_HeapRgn; _temp3426;}); _LL3411: { void* _temp3427= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _LL3429: if((
unsigned int) _temp3427 >  4u?*(( int*) _temp3427) ==  Cyc_Absyn_ArrayType: 0){
goto _LL3430;} else{ goto _LL3431;} _LL3431: goto _LL3432; _LL3430: return({
struct _tuple7 _temp3433; _temp3433.f1= 1; _temp3433.f2=( void*) Cyc_Absyn_HeapRgn;
_temp3433;}); _LL3432: return({ struct _tuple7 _temp3434; _temp3434.f1=(
_temp3418->tq).q_const; _temp3434.f2=( void*) Cyc_Absyn_HeapRgn; _temp3434;});
_LL3428:;} _LL3413: { void* _temp3435= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); _LL3437: if(( unsigned int) _temp3435 >  4u?*(( int*)
_temp3435) ==  Cyc_Absyn_ArrayType: 0){ goto _LL3438;} else{ goto _LL3439;}
_LL3439: goto _LL3440; _LL3438: return({ struct _tuple7 _temp3441; _temp3441.f1=
1; _temp3441.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3420->rgn))->v;
_temp3441;}); _LL3440: return({ struct _tuple7 _temp3442; _temp3442.f1=(
_temp3420->tq).q_const; _temp3442.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3420->rgn))->v; _temp3442;}); _LL3436:;} _LL3415: _temp3424=
_temp3422; goto _LL3417; _LL3417: return({ struct _tuple7 _temp3443; _temp3443.f1=(
_temp3424->tq).q_const; _temp3443.f2=( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3424->rgn))->v; _temp3443;}); _LL3405:;} _LL3377: { void*
_temp3444= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp3392->topt))->v); struct Cyc_List_List* _temp3456; struct Cyc_List_List*
_temp3458; struct Cyc_Absyn_Structdecl** _temp3460; struct Cyc_Absyn_Structdecl*
_temp3462; struct Cyc_Absyn_Uniondecl** _temp3463; struct Cyc_Absyn_Uniondecl*
_temp3465; _LL3446: if(( unsigned int) _temp3444 >  4u?*(( int*) _temp3444) == 
Cyc_Absyn_AnonStructType: 0){ _LL3457: _temp3456=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3444)->f1; goto _LL3447;} else{ goto _LL3448;} _LL3448: if(( unsigned int)
_temp3444 >  4u?*(( int*) _temp3444) ==  Cyc_Absyn_AnonUnionType: 0){ _LL3459:
_temp3458=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3444)->f1; goto
_LL3449;} else{ goto _LL3450;} _LL3450: if(( unsigned int) _temp3444 >  4u?*((
int*) _temp3444) ==  Cyc_Absyn_StructType: 0){ _LL3461: _temp3460=(( struct Cyc_Absyn_StructType_struct*)
_temp3444)->f3; if( _temp3460 ==  0){ goto _LL3452;} else{ _temp3462=* _temp3460;
goto _LL3451;}} else{ goto _LL3452;} _LL3452: if(( unsigned int) _temp3444 >  4u?*((
int*) _temp3444) ==  Cyc_Absyn_UnionType: 0){ _LL3464: _temp3463=(( struct Cyc_Absyn_UnionType_struct*)
_temp3444)->f3; if( _temp3463 ==  0){ goto _LL3454;} else{ _temp3465=* _temp3463;
goto _LL3453;}} else{ goto _LL3454;} _LL3454: goto _LL3455; _LL3447: _temp3458=
_temp3456; goto _LL3449; _LL3449: { struct Cyc_Absyn_Structfield* _temp3466= Cyc_Absyn_lookup_field(
_temp3458, _temp3390); if( _temp3466 !=  0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3466))->width !=  0: 0){ return({ struct _tuple7 _temp3467;
_temp3467.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3466))->tq).q_const;
_temp3467.f2=( Cyc_Tcutil_addressof_props( te, _temp3392)).f2; _temp3467;});}
return bogus_ans;} _LL3451: { struct Cyc_Absyn_Structfield* _temp3468= Cyc_Absyn_lookup_struct_field(
_temp3462, _temp3390); if( _temp3468 !=  0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3468))->width !=  0: 0){ return({ struct _tuple7 _temp3469;
_temp3469.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3468))->tq).q_const;
_temp3469.f2=( Cyc_Tcutil_addressof_props( te, _temp3392)).f2; _temp3469;});}
return bogus_ans;} _LL3453: { struct Cyc_Absyn_Structfield* _temp3470= Cyc_Absyn_lookup_union_field(
_temp3465, _temp3390); if( _temp3470 !=  0){ return({ struct _tuple7 _temp3471;
_temp3471.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3470))->tq).q_const;
_temp3471.f2=( Cyc_Tcutil_addressof_props( te, _temp3392)).f2; _temp3471;});}
goto _LL3455;} _LL3455: return bogus_ans; _LL3445:;} _LL3379: { void* _temp3472=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3396->topt))->v);
struct Cyc_Absyn_PtrInfo _temp3478; void* _temp3480; void* _temp3482; _LL3474:
if(( unsigned int) _temp3472 >  4u?*(( int*) _temp3472) ==  Cyc_Absyn_PointerType:
0){ _LL3479: _temp3478=(( struct Cyc_Absyn_PointerType_struct*) _temp3472)->f1;
_LL3483: _temp3482=( void*) _temp3478.elt_typ; goto _LL3481; _LL3481: _temp3480=(
void*) _temp3478.rgn_typ; goto _LL3475;} else{ goto _LL3476;} _LL3476: goto
_LL3477; _LL3475: { void* _temp3484= Cyc_Tcutil_compress( _temp3482); struct Cyc_List_List*
_temp3496; struct Cyc_List_List* _temp3498; struct Cyc_Absyn_Structdecl**
_temp3500; struct Cyc_Absyn_Structdecl* _temp3502; struct Cyc_Absyn_Uniondecl**
_temp3503; struct Cyc_Absyn_Uniondecl* _temp3505; _LL3486: if(( unsigned int)
_temp3484 >  4u?*(( int*) _temp3484) ==  Cyc_Absyn_AnonStructType: 0){ _LL3497:
_temp3496=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3484)->f1; goto
_LL3487;} else{ goto _LL3488;} _LL3488: if(( unsigned int) _temp3484 >  4u?*((
int*) _temp3484) ==  Cyc_Absyn_AnonUnionType: 0){ _LL3499: _temp3498=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3484)->f1; goto _LL3489;} else{ goto
_LL3490;} _LL3490: if(( unsigned int) _temp3484 >  4u?*(( int*) _temp3484) == 
Cyc_Absyn_StructType: 0){ _LL3501: _temp3500=(( struct Cyc_Absyn_StructType_struct*)
_temp3484)->f3; if( _temp3500 ==  0){ goto _LL3492;} else{ _temp3502=* _temp3500;
goto _LL3491;}} else{ goto _LL3492;} _LL3492: if(( unsigned int) _temp3484 >  4u?*((
int*) _temp3484) ==  Cyc_Absyn_UnionType: 0){ _LL3504: _temp3503=(( struct Cyc_Absyn_UnionType_struct*)
_temp3484)->f3; if( _temp3503 ==  0){ goto _LL3494;} else{ _temp3505=* _temp3503;
goto _LL3493;}} else{ goto _LL3494;} _LL3494: goto _LL3495; _LL3487: _temp3498=
_temp3496; goto _LL3489; _LL3489: { struct Cyc_Absyn_Structfield* _temp3506= Cyc_Absyn_lookup_field(
_temp3498, _temp3394); if( _temp3506 !=  0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3506))->width !=  0: 0){ return({ struct _tuple7 _temp3507;
_temp3507.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3506))->tq).q_const;
_temp3507.f2= _temp3480; _temp3507;});} return bogus_ans;} _LL3491: { struct Cyc_Absyn_Structfield*
_temp3508= Cyc_Absyn_lookup_struct_field( _temp3502, _temp3394); if( _temp3508
!=  0?(( struct Cyc_Absyn_Structfield*) _check_null( _temp3508))->width !=  0: 0){
return({ struct _tuple7 _temp3509; _temp3509.f1=((( struct Cyc_Absyn_Structfield*)
_check_null( _temp3508))->tq).q_const; _temp3509.f2= _temp3480; _temp3509;});}
return bogus_ans;} _LL3493: { struct Cyc_Absyn_Structfield* _temp3510= Cyc_Absyn_lookup_union_field(
_temp3505, _temp3394); if( _temp3510 !=  0){ return({ struct _tuple7 _temp3511;
_temp3511.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3510))->tq).q_const;
_temp3511.f2= _temp3480; _temp3511;});} return bogus_ans;} _LL3495: return
bogus_ans; _LL3485:;} _LL3477: return bogus_ans; _LL3473:;} _LL3381: { void*
_temp3512= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp3398->topt))->v); struct Cyc_Absyn_PtrInfo _temp3518; struct Cyc_Absyn_Tqual
_temp3520; void* _temp3522; _LL3514: if(( unsigned int) _temp3512 >  4u?*(( int*)
_temp3512) ==  Cyc_Absyn_PointerType: 0){ _LL3519: _temp3518=(( struct Cyc_Absyn_PointerType_struct*)
_temp3512)->f1; _LL3523: _temp3522=( void*) _temp3518.rgn_typ; goto _LL3521;
_LL3521: _temp3520= _temp3518.tq; goto _LL3515;} else{ goto _LL3516;} _LL3516:
goto _LL3517; _LL3515: return({ struct _tuple7 _temp3524; _temp3524.f1=
_temp3520.q_const; _temp3524.f2= _temp3522; _temp3524;}); _LL3517: return
bogus_ans; _LL3513:;} _LL3383: { void* t= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp3402->topt))->v); void* _temp3525= t; struct
Cyc_Absyn_Tqual _temp3535; struct Cyc_List_List* _temp3537; struct Cyc_Absyn_PtrInfo
_temp3539; struct Cyc_Absyn_Conref* _temp3541; struct Cyc_Absyn_Tqual _temp3543;
void* _temp3545; void* _temp3547; _LL3527: if(( unsigned int) _temp3525 >  4u?*((
int*) _temp3525) ==  Cyc_Absyn_ArrayType: 0){ _LL3536: _temp3535=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3525)->f2; goto _LL3528;} else{ goto _LL3529;} _LL3529: if(( unsigned int)
_temp3525 >  4u?*(( int*) _temp3525) ==  Cyc_Absyn_TupleType: 0){ _LL3538:
_temp3537=(( struct Cyc_Absyn_TupleType_struct*) _temp3525)->f1; goto _LL3530;}
else{ goto _LL3531;} _LL3531: if(( unsigned int) _temp3525 >  4u?*(( int*)
_temp3525) ==  Cyc_Absyn_PointerType: 0){ _LL3540: _temp3539=(( struct Cyc_Absyn_PointerType_struct*)
_temp3525)->f1; _LL3548: _temp3547=( void*) _temp3539.elt_typ; goto _LL3546;
_LL3546: _temp3545=( void*) _temp3539.rgn_typ; goto _LL3544; _LL3544: _temp3543=
_temp3539.tq; goto _LL3542; _LL3542: _temp3541= _temp3539.bounds; goto _LL3532;}
else{ goto _LL3533;} _LL3533: goto _LL3534; _LL3528: return({ struct _tuple7
_temp3549; _temp3549.f1= _temp3535.q_const; _temp3549.f2=( Cyc_Tcutil_addressof_props(
te, _temp3402)).f2; _temp3549;}); _LL3530: { struct _tuple4* _temp3550= Cyc_Absyn_lookup_tuple_field(
_temp3537,( int) Cyc_Evexp_eval_const_uint_exp( _temp3400)); if( _temp3550 !=  0){
return({ struct _tuple7 _temp3551; _temp3551.f1=((*(( struct _tuple4*)
_check_null( _temp3550))).f1).q_const; _temp3551.f2=( Cyc_Tcutil_addressof_props(
te, _temp3402)).f2; _temp3551;});} return bogus_ans;} _LL3532: return({ struct
_tuple7 _temp3552; _temp3552.f1= _temp3543.q_const; _temp3552.f2= _temp3545;
_temp3552;}); _LL3534: return bogus_ans; _LL3526:;} _LL3385:({ void* _temp3553[
0u]={}; Cyc_Tcutil_terr( e->loc, _tag_arr("unary & applied to non-lvalue",
sizeof( unsigned char), 30u), _tag_arr( _temp3553, sizeof( void*), 0u));});
return bogus_ans; _LL3373:;} void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv*
te, void* e_typ, struct Cyc_Absyn_Exp* e){ void* _temp3555= Cyc_Tcutil_compress(
e_typ); struct Cyc_Absyn_Tqual _temp3561; void* _temp3563; _LL3557: if((
unsigned int) _temp3555 >  4u?*(( int*) _temp3555) ==  Cyc_Absyn_ArrayType: 0){
_LL3564: _temp3563=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3555)->f1;
goto _LL3562; _LL3562: _temp3561=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3555)->f2; goto _LL3558;} else{ goto _LL3559;} _LL3559: goto _LL3560;
_LL3558: { void* _temp3567; struct _tuple7 _temp3565= Cyc_Tcutil_addressof_props(
te, e); _LL3568: _temp3567= _temp3565.f2; goto _LL3566; _LL3566: return Cyc_Absyn_atb_typ(
_temp3563, _temp3567, _temp3561,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp3569=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3569[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3570; _temp3570.tag= Cyc_Absyn_Upper_b;
_temp3570.f1= e; _temp3570;}); _temp3569;}));} _LL3560: return e_typ; _LL3556:;}
void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment* loc, unsigned int i,
struct Cyc_Absyn_Conref* b){ b= Cyc_Absyn_compress_conref( b);{ void* _temp3571=(
void*) b->v; void* _temp3579; void* _temp3581; struct Cyc_Absyn_Exp* _temp3583;
_LL3573: if(( unsigned int) _temp3571 >  1u?*(( int*) _temp3571) ==  Cyc_Absyn_Eq_constr:
0){ _LL3580: _temp3579=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3571)->f1;
if( _temp3579 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL3574;} else{ goto
_LL3575;}} else{ goto _LL3575;} _LL3575: if(( unsigned int) _temp3571 >  1u?*((
int*) _temp3571) ==  Cyc_Absyn_Eq_constr: 0){ _LL3582: _temp3581=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp3571)->f1; if(( unsigned int) _temp3581
>  1u?*(( int*) _temp3581) ==  Cyc_Absyn_Upper_b: 0){ _LL3584: _temp3583=((
struct Cyc_Absyn_Upper_b_struct*) _temp3581)->f1; goto _LL3576;} else{ goto
_LL3577;}} else{ goto _LL3577;} _LL3577: goto _LL3578; _LL3574: return; _LL3576:
if( Cyc_Evexp_eval_const_uint_exp( _temp3583) <=  i){({ void* _temp3585[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("dereference is out of bounds", sizeof(
unsigned char), 29u), _tag_arr( _temp3585, sizeof( void*), 0u));});} return;
_LL3578:( void*)( b->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3586=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3586[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3587; _temp3587.tag= Cyc_Absyn_Eq_constr;
_temp3587.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3588=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3588[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3589; _temp3589.tag= Cyc_Absyn_Upper_b;
_temp3589.f1= Cyc_Absyn_uint_exp( i +  1, 0); _temp3589;}); _temp3588;}));
_temp3587;}); _temp3586;}))); return; _LL3572:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_bits_only( void* t){ void* _temp3590= Cyc_Tcutil_compress(
t); void* _temp3616; struct Cyc_List_List* _temp3618; struct Cyc_Absyn_Structdecl**
_temp3620; struct Cyc_List_List* _temp3622; struct Cyc_Absyn_Uniondecl**
_temp3624; struct Cyc_List_List* _temp3626; struct Cyc_List_List* _temp3628;
struct Cyc_List_List* _temp3630; _LL3592: if( _temp3590 == ( void*) Cyc_Absyn_VoidType){
goto _LL3593;} else{ goto _LL3594;} _LL3594: if(( unsigned int) _temp3590 >  4u?*((
int*) _temp3590) ==  Cyc_Absyn_IntType: 0){ goto _LL3595;} else{ goto _LL3596;}
_LL3596: if( _temp3590 == ( void*) Cyc_Absyn_FloatType){ goto _LL3597;} else{
goto _LL3598;} _LL3598: if( _temp3590 == ( void*) Cyc_Absyn_DoubleType){ goto
_LL3599;} else{ goto _LL3600;} _LL3600: if(( unsigned int) _temp3590 >  4u?*((
int*) _temp3590) ==  Cyc_Absyn_EnumType: 0){ goto _LL3601;} else{ goto _LL3602;}
_LL3602: if(( unsigned int) _temp3590 >  4u?*(( int*) _temp3590) ==  Cyc_Absyn_ArrayType:
0){ _LL3617: _temp3616=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3590)->f1;
goto _LL3603;} else{ goto _LL3604;} _LL3604: if(( unsigned int) _temp3590 >  4u?*((
int*) _temp3590) ==  Cyc_Absyn_TupleType: 0){ _LL3619: _temp3618=(( struct Cyc_Absyn_TupleType_struct*)
_temp3590)->f1; goto _LL3605;} else{ goto _LL3606;} _LL3606: if(( unsigned int)
_temp3590 >  4u?*(( int*) _temp3590) ==  Cyc_Absyn_StructType: 0){ _LL3623:
_temp3622=(( struct Cyc_Absyn_StructType_struct*) _temp3590)->f2; goto _LL3621;
_LL3621: _temp3620=(( struct Cyc_Absyn_StructType_struct*) _temp3590)->f3; goto
_LL3607;} else{ goto _LL3608;} _LL3608: if(( unsigned int) _temp3590 >  4u?*((
int*) _temp3590) ==  Cyc_Absyn_UnionType: 0){ _LL3627: _temp3626=(( struct Cyc_Absyn_UnionType_struct*)
_temp3590)->f2; goto _LL3625; _LL3625: _temp3624=(( struct Cyc_Absyn_UnionType_struct*)
_temp3590)->f3; goto _LL3609;} else{ goto _LL3610;} _LL3610: if(( unsigned int)
_temp3590 >  4u?*(( int*) _temp3590) ==  Cyc_Absyn_AnonStructType: 0){ _LL3629:
_temp3628=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3590)->f1; goto
_LL3611;} else{ goto _LL3612;} _LL3612: if(( unsigned int) _temp3590 >  4u?*((
int*) _temp3590) ==  Cyc_Absyn_AnonUnionType: 0){ _LL3631: _temp3630=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3590)->f1; goto _LL3613;} else{ goto
_LL3614;} _LL3614: goto _LL3615; _LL3593: return 1; _LL3595: return 1; _LL3597:
return 1; _LL3599: return 1; _LL3601: return 0; _LL3603: return Cyc_Tcutil_bits_only(
_temp3616); _LL3605: for( 0; _temp3618 !=  0; _temp3618=(( struct Cyc_List_List*)
_check_null( _temp3618))->tl){ if( ! Cyc_Tcutil_bits_only((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp3618))->hd)).f2)){ return 0;}} return 1;
_LL3607: if( _temp3620 ==  0){ return 0;}{ struct Cyc_Absyn_Structdecl*
_temp3632=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp3620)); if(
_temp3632->fields ==  0){ return 0;}{ struct _RegionHandle _temp3633=
_new_region(); struct _RegionHandle* rgn=& _temp3633; _push_region( rgn);{
struct Cyc_List_List* _temp3634=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp3632->tvs, _temp3622);{ struct Cyc_List_List*
fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp3632->fields))->v;
for( 0; fs !=  0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only(
Cyc_Tcutil_rsubstitute( rgn, _temp3634,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( fs))->hd)->type))){ int _temp3635= 0;
_npop_handler( 0u); return _temp3635;}}}{ int _temp3636= 1; _npop_handler( 0u);
return _temp3636;}}; _pop_region( rgn);}} _LL3609: if( _temp3624 ==  0){ return
0;}{ struct Cyc_Absyn_Uniondecl* _temp3637=*(( struct Cyc_Absyn_Uniondecl**)
_check_null( _temp3624)); if( _temp3637->fields ==  0){ return 0;}{ struct
_RegionHandle _temp3638= _new_region(); struct _RegionHandle* rgn=& _temp3638;
_push_region( rgn);{ struct Cyc_List_List* _temp3639=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp3637->tvs, _temp3626);{
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp3637->fields))->v; for( 0; fs !=  0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute( rgn,
_temp3639,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3640= 0; _npop_handler( 0u); return
_temp3640;}}}{ int _temp3641= 1; _npop_handler( 0u); return _temp3641;}};
_pop_region( rgn);}} _LL3611: _temp3630= _temp3628; goto _LL3613; _LL3613: for(
0; _temp3630 !=  0; _temp3630=(( struct Cyc_List_List*) _check_null( _temp3630))->tl){
if( ! Cyc_Tcutil_bits_only(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp3630))->hd)->type)){ return 0;}} return 1;
_LL3615: return 0; _LL3591:;} struct _tuple20{ struct Cyc_List_List* f1; struct
Cyc_Absyn_Exp* f2; } ; static int Cyc_Tcutil_cnst_exp( struct Cyc_Tcenv_Tenv* te,
int var_okay, struct Cyc_Absyn_Exp* e){ void* _temp3642=( void*) e->r; struct
_tuple1* _temp3682; struct Cyc_Absyn_Exp* _temp3684; struct Cyc_Absyn_Exp*
_temp3686; struct Cyc_Absyn_Exp* _temp3688; struct Cyc_Absyn_Exp* _temp3690;
struct Cyc_Absyn_Exp* _temp3692; struct Cyc_Absyn_Exp* _temp3694; struct Cyc_Absyn_Exp*
_temp3696; struct Cyc_Absyn_Exp* _temp3698; void* _temp3700; struct Cyc_Absyn_Exp*
_temp3702; struct Cyc_Absyn_Exp* _temp3704; struct Cyc_Absyn_Exp* _temp3706;
struct Cyc_List_List* _temp3708; struct Cyc_List_List* _temp3710; struct Cyc_List_List*
_temp3712; struct Cyc_List_List* _temp3714; void* _temp3716; struct Cyc_List_List*
_temp3718; struct Cyc_List_List* _temp3720; _LL3644: if(*(( int*) _temp3642) == 
Cyc_Absyn_Const_e){ goto _LL3645;} else{ goto _LL3646;} _LL3646: if(*(( int*)
_temp3642) ==  Cyc_Absyn_Sizeoftyp_e){ goto _LL3647;} else{ goto _LL3648;}
_LL3648: if(*(( int*) _temp3642) ==  Cyc_Absyn_Sizeofexp_e){ goto _LL3649;}
else{ goto _LL3650;} _LL3650: if(*(( int*) _temp3642) ==  Cyc_Absyn_Enum_e){
goto _LL3651;} else{ goto _LL3652;} _LL3652: if(*(( int*) _temp3642) ==  Cyc_Absyn_Var_e){
_LL3683: _temp3682=(( struct Cyc_Absyn_Var_e_struct*) _temp3642)->f1; goto
_LL3653;} else{ goto _LL3654;} _LL3654: if(*(( int*) _temp3642) ==  Cyc_Absyn_Conditional_e){
_LL3689: _temp3688=(( struct Cyc_Absyn_Conditional_e_struct*) _temp3642)->f1;
goto _LL3687; _LL3687: _temp3686=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3642)->f2; goto _LL3685; _LL3685: _temp3684=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3642)->f3; goto _LL3655;} else{ goto _LL3656;} _LL3656: if(*(( int*)
_temp3642) ==  Cyc_Absyn_SeqExp_e){ _LL3693: _temp3692=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3642)->f1; goto _LL3691; _LL3691: _temp3690=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3642)->f2; goto _LL3657;} else{ goto _LL3658;} _LL3658: if(*(( int*)
_temp3642) ==  Cyc_Absyn_NoInstantiate_e){ _LL3695: _temp3694=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp3642)->f1; goto _LL3659;} else{ goto _LL3660;} _LL3660: if(*(( int*)
_temp3642) ==  Cyc_Absyn_Instantiate_e){ _LL3697: _temp3696=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3642)->f1; goto _LL3661;} else{ goto _LL3662;} _LL3662: if(*(( int*)
_temp3642) ==  Cyc_Absyn_Cast_e){ _LL3701: _temp3700=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp3642)->f1; goto _LL3699; _LL3699: _temp3698=(( struct Cyc_Absyn_Cast_e_struct*)
_temp3642)->f2; goto _LL3663;} else{ goto _LL3664;} _LL3664: if(*(( int*)
_temp3642) ==  Cyc_Absyn_Address_e){ _LL3703: _temp3702=(( struct Cyc_Absyn_Address_e_struct*)
_temp3642)->f1; goto _LL3665;} else{ goto _LL3666;} _LL3666: if(*(( int*)
_temp3642) ==  Cyc_Absyn_Comprehension_e){ _LL3707: _temp3706=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3642)->f2; goto _LL3705; _LL3705: _temp3704=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3642)->f3; goto _LL3667;} else{ goto _LL3668;} _LL3668: if(*(( int*)
_temp3642) ==  Cyc_Absyn_Array_e){ _LL3709: _temp3708=(( struct Cyc_Absyn_Array_e_struct*)
_temp3642)->f1; goto _LL3669;} else{ goto _LL3670;} _LL3670: if(*(( int*)
_temp3642) ==  Cyc_Absyn_AnonStruct_e){ _LL3711: _temp3710=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3642)->f2; goto _LL3671;} else{ goto _LL3672;} _LL3672: if(*(( int*)
_temp3642) ==  Cyc_Absyn_Struct_e){ _LL3713: _temp3712=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3642)->f3; goto _LL3673;} else{ goto _LL3674;} _LL3674: if(*(( int*)
_temp3642) ==  Cyc_Absyn_Primop_e){ _LL3717: _temp3716=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp3642)->f1; goto _LL3715; _LL3715: _temp3714=(( struct Cyc_Absyn_Primop_e_struct*)
_temp3642)->f2; goto _LL3675;} else{ goto _LL3676;} _LL3676: if(*(( int*)
_temp3642) ==  Cyc_Absyn_Tuple_e){ _LL3719: _temp3718=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp3642)->f1; goto _LL3677;} else{ goto _LL3678;} _LL3678: if(*(( int*)
_temp3642) ==  Cyc_Absyn_Tunion_e){ _LL3721: _temp3720=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3642)->f3; goto _LL3679;} else{ goto _LL3680;} _LL3680: goto _LL3681;
_LL3645: return 1; _LL3647: return 1; _LL3649: return 1; _LL3651: return 1;
_LL3653: { struct _handler_cons _temp3722; _push_handler(& _temp3722);{ int
_temp3724= 0; if( setjmp( _temp3722.handler)){ _temp3724= 1;} if( ! _temp3724){{
void* _temp3725= Cyc_Tcenv_lookup_ordinary( te, e->loc, _temp3682); void*
_temp3733; void* _temp3735; struct Cyc_Absyn_Vardecl* _temp3737; _LL3727: if(*((
int*) _temp3725) ==  Cyc_Tcenv_VarRes){ _LL3734: _temp3733=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3725)->f1; if(( unsigned int) _temp3733 >  1u?*(( int*) _temp3733) ==  Cyc_Absyn_Funname_b:
0){ goto _LL3728;} else{ goto _LL3729;}} else{ goto _LL3729;} _LL3729: if(*((
int*) _temp3725) ==  Cyc_Tcenv_VarRes){ _LL3736: _temp3735=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3725)->f1; if(( unsigned int) _temp3735 >  1u?*(( int*) _temp3735) ==  Cyc_Absyn_Global_b:
0){ _LL3738: _temp3737=(( struct Cyc_Absyn_Global_b_struct*) _temp3735)->f1;
goto _LL3730;} else{ goto _LL3731;}} else{ goto _LL3731;} _LL3731: goto _LL3732;
_LL3728: { int _temp3739= 1; _npop_handler( 0u); return _temp3739;} _LL3730: {
void* _temp3740= Cyc_Tcutil_compress(( void*) _temp3737->type); _LL3742: if((
unsigned int) _temp3740 >  4u?*(( int*) _temp3740) ==  Cyc_Absyn_ArrayType: 0){
goto _LL3743;} else{ goto _LL3744;} _LL3744: goto _LL3745; _LL3743: { int
_temp3746= 1; _npop_handler( 0u); return _temp3746;} _LL3745: { int _temp3747=
var_okay; _npop_handler( 0u); return _temp3747;} _LL3741:;} _LL3732: { int
_temp3748= var_okay; _npop_handler( 0u); return _temp3748;} _LL3726:;};
_pop_handler();} else{ void* _temp3723=( void*) _exn_thrown; void* _temp3750=
_temp3723; _LL3752: if( _temp3750 ==  Cyc_Dict_Absent){ goto _LL3753;} else{
goto _LL3754;} _LL3754: goto _LL3755; _LL3753: return 0; _LL3755:( void) _throw(
_temp3750); _LL3751:;}}} _LL3655: return( Cyc_Tcutil_cnst_exp( te, 0, _temp3688)?
Cyc_Tcutil_cnst_exp( te, 0, _temp3686): 0)? Cyc_Tcutil_cnst_exp( te, 0,
_temp3684): 0; _LL3657: return Cyc_Tcutil_cnst_exp( te, 0, _temp3692)? Cyc_Tcutil_cnst_exp(
te, 0, _temp3690): 0; _LL3659: _temp3696= _temp3694; goto _LL3661; _LL3661:
return Cyc_Tcutil_cnst_exp( te, var_okay, _temp3696); _LL3663: return Cyc_Tcutil_cnst_exp(
te, var_okay, _temp3698); _LL3665: return Cyc_Tcutil_cnst_exp( te, 1, _temp3702);
_LL3667: return Cyc_Tcutil_cnst_exp( te, 0, _temp3706)? Cyc_Tcutil_cnst_exp( te,
0, _temp3704): 0; _LL3669: _temp3710= _temp3708; goto _LL3671; _LL3671:
_temp3712= _temp3710; goto _LL3673; _LL3673: for( 0; _temp3712 !=  0; _temp3712=((
struct Cyc_List_List*) _check_null( _temp3712))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,(*(( struct _tuple20*)(( struct Cyc_List_List*) _check_null( _temp3712))->hd)).f2)){
return 0;}} return 1; _LL3675: _temp3718= _temp3714; goto _LL3677; _LL3677:
_temp3720= _temp3718; goto _LL3679; _LL3679: for( 0; _temp3720 !=  0; _temp3720=((
struct Cyc_List_List*) _check_null( _temp3720))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp3720))->hd)){
return 0;}} return 1; _LL3681: return 0; _LL3643:;} int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcutil_cnst_exp(
te, 0, e);} static int Cyc_Tcutil_fields_support_default( struct Cyc_List_List*
tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs); int Cyc_Tcutil_supports_default(
void* t){ void* _temp3756= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp3794; struct Cyc_Absyn_Conref* _temp3796; struct Cyc_Absyn_Conref*
_temp3798; void* _temp3800; void* _temp3802; struct Cyc_List_List* _temp3804;
struct Cyc_Absyn_Structdecl** _temp3806; struct Cyc_List_List* _temp3808; struct
Cyc_Absyn_Uniondecl** _temp3810; struct Cyc_List_List* _temp3812; struct Cyc_List_List*
_temp3814; struct Cyc_List_List* _temp3816; _LL3758: if( _temp3756 == ( void*)
Cyc_Absyn_VoidType){ goto _LL3759;} else{ goto _LL3760;} _LL3760: if((
unsigned int) _temp3756 >  4u?*(( int*) _temp3756) ==  Cyc_Absyn_Evar: 0){ goto
_LL3761;} else{ goto _LL3762;} _LL3762: if(( unsigned int) _temp3756 >  4u?*((
int*) _temp3756) ==  Cyc_Absyn_VarType: 0){ goto _LL3763;} else{ goto _LL3764;}
_LL3764: if(( unsigned int) _temp3756 >  4u?*(( int*) _temp3756) ==  Cyc_Absyn_TunionType:
0){ goto _LL3765;} else{ goto _LL3766;} _LL3766: if(( unsigned int) _temp3756 > 
4u?*(( int*) _temp3756) ==  Cyc_Absyn_TunionFieldType: 0){ goto _LL3767;} else{
goto _LL3768;} _LL3768: if(( unsigned int) _temp3756 >  4u?*(( int*) _temp3756)
==  Cyc_Absyn_PointerType: 0){ _LL3795: _temp3794=(( struct Cyc_Absyn_PointerType_struct*)
_temp3756)->f1; _LL3801: _temp3800=( void*) _temp3794.elt_typ; goto _LL3799;
_LL3799: _temp3798= _temp3794.nullable; goto _LL3797; _LL3797: _temp3796=
_temp3794.bounds; goto _LL3769;} else{ goto _LL3770;} _LL3770: if(( unsigned int)
_temp3756 >  4u?*(( int*) _temp3756) ==  Cyc_Absyn_IntType: 0){ goto _LL3771;}
else{ goto _LL3772;} _LL3772: if( _temp3756 == ( void*) Cyc_Absyn_FloatType){
goto _LL3773;} else{ goto _LL3774;} _LL3774: if( _temp3756 == ( void*) Cyc_Absyn_DoubleType){
goto _LL3775;} else{ goto _LL3776;} _LL3776: if(( unsigned int) _temp3756 >  4u?*((
int*) _temp3756) ==  Cyc_Absyn_ArrayType: 0){ _LL3803: _temp3802=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3756)->f1; goto _LL3777;} else{ goto
_LL3778;} _LL3778: if(( unsigned int) _temp3756 >  4u?*(( int*) _temp3756) == 
Cyc_Absyn_FnType: 0){ goto _LL3779;} else{ goto _LL3780;} _LL3780: if((
unsigned int) _temp3756 >  4u?*(( int*) _temp3756) ==  Cyc_Absyn_TupleType: 0){
_LL3805: _temp3804=(( struct Cyc_Absyn_TupleType_struct*) _temp3756)->f1; goto
_LL3781;} else{ goto _LL3782;} _LL3782: if(( unsigned int) _temp3756 >  4u?*((
int*) _temp3756) ==  Cyc_Absyn_StructType: 0){ _LL3809: _temp3808=(( struct Cyc_Absyn_StructType_struct*)
_temp3756)->f2; goto _LL3807; _LL3807: _temp3806=(( struct Cyc_Absyn_StructType_struct*)
_temp3756)->f3; goto _LL3783;} else{ goto _LL3784;} _LL3784: if(( unsigned int)
_temp3756 >  4u?*(( int*) _temp3756) ==  Cyc_Absyn_UnionType: 0){ _LL3813:
_temp3812=(( struct Cyc_Absyn_UnionType_struct*) _temp3756)->f2; goto _LL3811;
_LL3811: _temp3810=(( struct Cyc_Absyn_UnionType_struct*) _temp3756)->f3; goto
_LL3785;} else{ goto _LL3786;} _LL3786: if(( unsigned int) _temp3756 >  4u?*((
int*) _temp3756) ==  Cyc_Absyn_AnonStructType: 0){ _LL3815: _temp3814=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp3756)->f1; goto _LL3787;} else{ goto
_LL3788;} _LL3788: if(( unsigned int) _temp3756 >  4u?*(( int*) _temp3756) == 
Cyc_Absyn_AnonUnionType: 0){ _LL3817: _temp3816=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp3756)->f1; goto _LL3789;} else{ goto _LL3790;} _LL3790: if(( unsigned int)
_temp3756 >  4u?*(( int*) _temp3756) ==  Cyc_Absyn_EnumType: 0){ goto _LL3791;}
else{ goto _LL3792;} _LL3792: goto _LL3793; _LL3759: return 1; _LL3761: return 0;
_LL3763: return 0; _LL3765: return 0; _LL3767: return 0; _LL3769: { void*
_temp3818=( void*)( Cyc_Absyn_compress_conref( _temp3796))->v; void* _temp3826;
void* _temp3828; _LL3820: if(( unsigned int) _temp3818 >  1u?*(( int*) _temp3818)
==  Cyc_Absyn_Eq_constr: 0){ _LL3827: _temp3826=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3818)->f1; if( _temp3826 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL3821;}
else{ goto _LL3822;}} else{ goto _LL3822;} _LL3822: if(( unsigned int) _temp3818
>  1u?*(( int*) _temp3818) ==  Cyc_Absyn_Eq_constr: 0){ _LL3829: _temp3828=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3818)->f1; if(( unsigned int)
_temp3828 >  1u?*(( int*) _temp3828) ==  Cyc_Absyn_Upper_b: 0){ goto _LL3823;}
else{ goto _LL3824;}} else{ goto _LL3824;} _LL3824: goto _LL3825; _LL3821:
return 1; _LL3823: { void* _temp3830=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3798))->v; int
_temp3836; _LL3832: if(( unsigned int) _temp3830 >  1u?*(( int*) _temp3830) == 
Cyc_Absyn_Eq_constr: 0){ _LL3837: _temp3836=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3830)->f1; goto _LL3833;} else{ goto _LL3834;} _LL3834: goto _LL3835;
_LL3833: return _temp3836; _LL3835: return 0; _LL3831:;} _LL3825: return 0;
_LL3819:;} _LL3771: return 1; _LL3773: return 1; _LL3775: return 1; _LL3777:
return Cyc_Tcutil_supports_default( _temp3802); _LL3779: return 0; _LL3781: for(
0; _temp3804 !=  0; _temp3804=(( struct Cyc_List_List*) _check_null( _temp3804))->tl){
if( ! Cyc_Tcutil_supports_default((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp3804))->hd)).f2)){ return 0;}} return 1; _LL3783: if(
_temp3806 ==  0){ return 0;}{ struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp3806)); if( sd->fields ==  0){ return 0;} return Cyc_Tcutil_fields_support_default(
sd->tvs, _temp3808,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
sd->fields))->v);} _LL3785: if( _temp3810 ==  0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp3810)); if( ud->fields == 
0){ return 0;} return Cyc_Tcutil_fields_support_default( ud->tvs, _temp3812,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ud->fields))->v);}
_LL3787: _temp3816= _temp3814; goto _LL3789; _LL3789: return Cyc_Tcutil_fields_support_default(
0, 0, _temp3816); _LL3791: return 1; _LL3793: return 0; _LL3757:;} static int
Cyc_Tcutil_fields_support_default( struct Cyc_List_List* tvs, struct Cyc_List_List*
ts, struct Cyc_List_List* fs){{ struct _RegionHandle _temp3838= _new_region();
struct _RegionHandle* rgn=& _temp3838; _push_region( rgn);{ struct Cyc_List_List*
_temp3839=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, tvs, ts); for( 0; fs !=  0; fs=(( struct Cyc_List_List*) _check_null(
fs))->tl){ void* t= Cyc_Tcutil_rsubstitute( rgn, _temp3839,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( fs))->hd)->type); if( ! Cyc_Tcutil_supports_default(
t)){ int _temp3840= 0; _npop_handler( 0u); return _temp3840;}}}; _pop_region(
rgn);} return 1;}
