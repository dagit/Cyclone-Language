 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 8u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern int Cyc_Core_intcmp( int, int); extern unsigned char Cyc_Core_InvalidArg[
15u]; struct Cyc_Core_InvalidArg_struct{ unsigned char* tag; struct
_tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct
_tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern
unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern struct _RegionHandle*
Cyc_Core_heap_region; extern unsigned char* string_to_Cstring( struct
_tagged_string); extern unsigned char* underlying_Cstring( struct _tagged_string);
extern struct _tagged_string Cstring_to_string( unsigned char*); extern int
system( unsigned char*); struct Cyc_Stdio___sFILE; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_stderr; extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*);
extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_string f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
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
struct _tagged_string logical_file; struct _tagged_string line; int line_no; int
col; } ; extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment;
extern struct Cyc_List_List* Cyc_Position_strings_of_segments( struct Cyc_List_List*);
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_string source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_string desc; } ; extern struct Cyc_Position_Error* Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*, struct _tagged_string); extern unsigned char Cyc_Position_Nocontext[
14u]; extern void Cyc_Position_post_error( struct Cyc_Position_Error*); struct
_tuple1{ void* f1; struct _tagged_string* f2; } ; struct Cyc_Absyn_Tvar; struct
Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref; struct Cyc_Absyn_PtrInfo; struct Cyc_Absyn_VarargInfo;
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
_tagged_string* name; int* identity; struct Cyc_Absyn_Conref* kind; } ; static
const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b= 0; struct
Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_PtrInfo{
void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref* nullable; struct Cyc_Absyn_Tqual
tq; struct Cyc_Absyn_Conref* bounds; } ; struct Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt*
name; struct Cyc_Absyn_Tqual tq; void* type; void* rgn; int inject; } ; struct
Cyc_Absyn_FnInfo{ struct Cyc_List_List* tvars; struct Cyc_Core_Opt* effect; void*
ret_typ; struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
cyc_varargs; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{
struct _tuple1* name; int is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion=
0; struct Cyc_Absyn_UnknownTunion_struct{ int tag; struct Cyc_Absyn_UnknownTunionInfo
f1; } ; static const int Cyc_Absyn_KnownTunion= 1; struct Cyc_Absyn_KnownTunion_struct{
int tag; struct Cyc_Absyn_Tuniondecl* f1; } ; struct Cyc_Absyn_TunionInfo{ void*
tunion_info; struct Cyc_List_List* targs; void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{
struct _tuple1* tunion_name; struct _tuple1* field_name; int is_xtunion; } ;
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
f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt* f4; } ; static
const int Cyc_Absyn_NonNullable_ps= 0; struct Cyc_Absyn_NonNullable_ps_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Nullable_ps= 1;
struct Cyc_Absyn_Nullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_TaggedArray_ps= 0; static const int Cyc_Absyn_Regparm_att=
0; struct Cyc_Absyn_Regparm_att_struct{ int tag; int f1; } ; static const int
Cyc_Absyn_Stdcall_att= 0; static const int Cyc_Absyn_Cdecl_att= 1; static const
int Cyc_Absyn_Fastcall_att= 2; static const int Cyc_Absyn_Noreturn_att= 3;
static const int Cyc_Absyn_Const_att= 4; static const int Cyc_Absyn_Aligned_att=
1; struct Cyc_Absyn_Aligned_att_struct{ int tag; int f1; } ; static const int
Cyc_Absyn_Packed_att= 5; static const int Cyc_Absyn_Section_att= 2; struct Cyc_Absyn_Section_att_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Nocommon_att=
6; static const int Cyc_Absyn_Shared_att= 7; static const int Cyc_Absyn_Unused_att=
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
_tagged_string f1; } ; static const int Cyc_Absyn_String_c= 5; struct Cyc_Absyn_String_c_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Null_c= 0;
static const int Cyc_Absyn_Plus= 0; static const int Cyc_Absyn_Times= 1; static
const int Cyc_Absyn_Minus= 2; static const int Cyc_Absyn_Div= 3; static const
int Cyc_Absyn_Mod= 4; static const int Cyc_Absyn_Eq= 5; static const int Cyc_Absyn_Neq=
6; static const int Cyc_Absyn_Gt= 7; static const int Cyc_Absyn_Lt= 8; static
const int Cyc_Absyn_Gte= 9; static const int Cyc_Absyn_Lte= 10; static const int
Cyc_Absyn_Not= 11; static const int Cyc_Absyn_Bitnot= 12; static const int Cyc_Absyn_Bitand=
13; static const int Cyc_Absyn_Bitor= 14; static const int Cyc_Absyn_Bitxor= 15;
static const int Cyc_Absyn_Bitlshift= 16; static const int Cyc_Absyn_Bitlrshift=
17; static const int Cyc_Absyn_Bitarshift= 18; static const int Cyc_Absyn_Size=
19; static const int Cyc_Absyn_Printf= 20; static const int Cyc_Absyn_Fprintf=
21; static const int Cyc_Absyn_Xprintf= 22; static const int Cyc_Absyn_Scanf= 23;
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
static const int Cyc_Absyn_Deref_e= 18; struct Cyc_Absyn_Deref_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_StructMember_e= 19;
struct Cyc_Absyn_StructMember_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct _tagged_string* f2; } ; static const int Cyc_Absyn_StructArrow_e= 20;
struct Cyc_Absyn_StructArrow_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tagged_string* f2; } ; static const int Cyc_Absyn_Subscript_e= 21; struct Cyc_Absyn_Subscript_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; static const
int Cyc_Absyn_Tuple_e= 22; struct Cyc_Absyn_Tuple_e_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_CompoundLit_e= 23; struct _tuple2{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; struct Cyc_Absyn_CompoundLit_e_struct{
int tag; struct _tuple2* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e=
24; struct Cyc_Absyn_Array_e_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_Comprehension_e= 25; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Struct_e= 26; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple1* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_AnonStruct_e= 27;
struct Cyc_Absyn_AnonStruct_e_struct{ int tag; void* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Absyn_Tunion_e= 28; struct Cyc_Absyn_Tunion_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ;
static const int Cyc_Absyn_Enum_e= 29; struct Cyc_Absyn_Enum_e_struct{ int tag;
struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_Malloc_e= 30; struct Cyc_Absyn_Malloc_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_UnresolvedMem_e=
31; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 32; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 33; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 34; struct Cyc_Absyn_Fill_e_struct{
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
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt*
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
struct _tagged_string* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Do_s=
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
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Tuple_p= 4;
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
cyc_varargs; struct Cyc_Absyn_Stmt* body; struct Cyc_Core_Opt* cached_typ;
struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List* attributes; } ;
struct Cyc_Absyn_Structfield{ struct _tagged_string* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List* attributes; }
; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Tunionfield{ struct _tuple1* name; struct Cyc_List_List* tvs; struct
Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ; struct Cyc_Absyn_Tuniondecl{
void* sc; struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{ struct _tuple1* name;
struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Enumdecl{
void* sc; struct _tuple1* name; struct Cyc_Core_Opt* fields; } ; struct Cyc_Absyn_Typedefdecl{
struct _tuple1* name; struct Cyc_List_List* tvs; void* defn; } ; static const
int Cyc_Absyn_Var_d= 0; struct Cyc_Absyn_Var_d_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; } ; static const int Cyc_Absyn_Fn_d= 1; struct Cyc_Absyn_Fn_d_struct{ int
tag; struct Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Let_d= 2;
struct Cyc_Absyn_Let_d_struct{ int tag; struct Cyc_Absyn_Pat* f1; struct Cyc_Core_Opt*
f2; struct Cyc_Core_Opt* f3; struct Cyc_Absyn_Exp* f4; int f5; } ; static const
int Cyc_Absyn_Letv_d= 3; struct Cyc_Absyn_Letv_d_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_Struct_d= 4; struct Cyc_Absyn_Struct_d_struct{
int tag; struct Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Absyn_Union_d=
5; struct Cyc_Absyn_Union_d_struct{ int tag; struct Cyc_Absyn_Uniondecl* f1; } ;
static const int Cyc_Absyn_Tunion_d= 6; struct Cyc_Absyn_Tunion_d_struct{ int
tag; struct Cyc_Absyn_Tuniondecl* f1; } ; static const int Cyc_Absyn_Enum_d= 7;
struct Cyc_Absyn_Enum_d_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; } ;
static const int Cyc_Absyn_Typedef_d= 8; struct Cyc_Absyn_Typedef_d_struct{ int
tag; struct Cyc_Absyn_Typedefdecl* f1; } ; static const int Cyc_Absyn_Namespace_d=
9; struct Cyc_Absyn_Namespace_d_struct{ int tag; struct _tagged_string* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
11; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static
const int Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1;
struct Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ;
extern unsigned char Cyc_Absyn_EmptyAnnot[ 15u]; extern int Cyc_Absyn_qvar_cmp(
struct _tuple1*, struct _tuple1*); extern int Cyc_Absyn_tvar_cmp( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*); extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x); extern struct
Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_new_evar( struct Cyc_Core_Opt* k, struct Cyc_Core_Opt*
tenv); extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulong_t; extern
void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_slong_t; extern void* Cyc_Absyn_float_t;
extern void* Cyc_Absyn_double_t; extern void* Cyc_Absyn_bounds_one; extern void*
Cyc_Absyn_atb_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual tq, void* b);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_field( struct Cyc_List_List*,
struct _tagged_string*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field(
struct Cyc_Absyn_Structdecl*, struct _tagged_string*); extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_union_field( struct Cyc_Absyn_Uniondecl*, struct _tagged_string*);
struct _tuple4{ struct Cyc_Absyn_Tqual f1; void* f2; } ; extern struct _tuple4*
Cyc_Absyn_lookup_tuple_field( struct Cyc_List_List*, int); extern struct
_tagged_string Cyc_Absyn_attribute2string( void*); extern int Cyc_Absyn_fntype_att(
void* a); struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc; extern
struct _tagged_string Cyc_Absynpp_typ2string( void*); extern struct
_tagged_string Cyc_Absynpp_kind2string( void*); extern struct _tagged_string Cyc_Absynpp_ckind2string(
struct Cyc_Absyn_Conref*); extern struct _tagged_string Cyc_Absynpp_qvar2string(
struct _tuple1*); struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[ 11u];
struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u]; extern
unsigned char Cyc_Dict_Absent[ 11u]; static const int Cyc_Tcenv_VarRes= 0;
struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes=
1; struct Cyc_Tcenv_StructRes_struct{ int tag; struct Cyc_Absyn_Structdecl* f1;
} ; static const int Cyc_Tcenv_TunionRes= 2; struct Cyc_Tcenv_TunionRes_struct{
int tag; struct Cyc_Absyn_Tuniondecl* f1; struct Cyc_Absyn_Tunionfield* f2; } ;
static const int Cyc_Tcenv_EnumRes= 3; struct Cyc_Tcenv_EnumRes_struct{ int tag;
struct Cyc_Absyn_Enumdecl* f1; struct Cyc_Absyn_Enumfield* f2; } ; struct Cyc_Tcenv_Genv{
struct Cyc_Set_Set* namespaces; struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict*
uniondecls; struct Cyc_Dict_Dict* tuniondecls; struct Cyc_Dict_Dict* enumdecls;
struct Cyc_Dict_Dict* typedefs; struct Cyc_Dict_Dict* ordinaries; struct Cyc_List_List*
availables; } ; struct Cyc_Tcenv_Fenv; static const int Cyc_Tcenv_NotLoop_j= 0;
static const int Cyc_Tcenv_CaseEnd_j= 1; static const int Cyc_Tcenv_FnEnd_j= 2;
static const int Cyc_Tcenv_Stmt_j= 0; struct Cyc_Tcenv_Stmt_j_struct{ int tag;
struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Tcenv_Outermost= 0; struct
Cyc_Tcenv_Outermost_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_Frame=
1; struct Cyc_Tcenv_Frame_struct{ int tag; void* f1; void* f2; } ; static const
int Cyc_Tcenv_Hidden= 2; struct Cyc_Tcenv_Hidden_struct{ int tag; void* f1; void*
f2; } ; struct Cyc_Tcenv_Tenv{ struct Cyc_List_List* ns; struct Cyc_Dict_Dict*
ae; struct Cyc_Core_Opt* le; } ; extern void* Cyc_Tcenv_lookup_ordinary( struct
Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern struct
Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern struct Cyc_Absyn_Uniondecl**
Cyc_Tcenv_lookup_uniondecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern struct Cyc_Absyn_Typedefdecl*
Cyc_Tcenv_lookup_typedefdecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern int Cyc_String_strcmp( struct _tagged_string s1, struct
_tagged_string s2); extern int Cyc_String_zstrptrcmp( struct _tagged_string*,
struct _tagged_string*); extern struct _tagged_string Cyc_String_strconcat(
struct _tagged_string, struct _tagged_string); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern
void* Cyc_Tcutil_impos( struct _tagged_string); extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_string); extern void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*, struct _tagged_string); extern void Cyc_Tcutil_flush_warnings();
extern struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set; extern void* Cyc_Tcutil_copy_type(
void* t); extern void* Cyc_Tcutil_typ_kind( void* t); extern void* Cyc_Tcutil_compress(
void* t); extern void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*, struct
Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*,
void*, struct Cyc_List_List*); extern int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_sint_typ( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerceable( void*); extern
int Cyc_Tcutil_silent_castable( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
void*, void*); extern int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*, struct
Cyc_Position_Segment*, void*, void*); extern int Cyc_Tcutil_is_integral( struct
Cyc_Absyn_Exp*); extern int Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_is_function_type( void* t); extern void* Cyc_Tcutil_max_arithmetic_type(
void*, void*); extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List*, void*); extern void* Cyc_Tcutil_rsubstitute( struct
_RegionHandle*, struct Cyc_List_List*, void*); extern int Cyc_Tcutil_subset_effect(
int set_to_empty, void* e1, void* e2); extern int Cyc_Tcutil_region_in_effect(
int constrain, void* r, void* e); extern void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*);
extern struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*,
void*, struct Cyc_Position_Segment*); struct _tuple5{ struct Cyc_Absyn_Tvar* f1;
void* f2; } ; extern struct _tuple5* Cyc_Tcutil_make_inst_var( struct Cyc_List_List*,
struct Cyc_Absyn_Tvar*); struct _tuple6{ struct Cyc_List_List* f1; struct
_RegionHandle* f2; } ; extern struct _tuple5* Cyc_Tcutil_r_make_inst_var( struct
_tuple6*, struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*); extern void Cyc_Tcutil_check_valid_toplevel_type( struct
Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, void*); extern void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Fndecl*);
extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, void*); extern void Cyc_Tcutil_check_unique_vars(
struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc, struct
_tagged_string err_msg); extern void Cyc_Tcutil_check_unique_tvars( struct Cyc_Position_Segment*,
struct Cyc_List_List*); extern void Cyc_Tcutil_check_nonzero_bound( struct Cyc_Position_Segment*,
struct Cyc_Absyn_Conref*); extern void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment*,
unsigned int i, struct Cyc_Absyn_Conref*); extern int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2); extern struct Cyc_List_List*
Cyc_Tcutil_resolve_struct_designators( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields); extern int Cyc_Tcutil_is_tagged_pointer_typ(
void*); extern void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv*, void* t,
struct Cyc_Absyn_Exp* e); struct _tuple7{ int f1; void* f2; } ; extern struct
_tuple7 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
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
loc, struct _tagged_string s){ Cyc_Position_post_error( Cyc_Position_mk_err_elab(
loc, s));} void* Cyc_Tcutil_impos( struct _tagged_string msg){({ struct
_tagged_string _temp0= msg; fprintf( sfile_to_file( Cyc_Stdio_stderr),"Error: %.*s\n",
_temp0.last_plus_one - _temp0.curr, _temp0.curr);}); Cyc_Stdio_fflush(( struct
Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( void) _throw(( void*) Cyc_Tcutil_TypeErr);}
static struct _tagged_string Cyc_Tcutil_tvar2string( struct Cyc_Absyn_Tvar* tv){
return* tv->name;} void Cyc_Tcutil_print_tvars( struct Cyc_List_List* tvs){ for(
0; tvs != 0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){({ struct
_tagged_string _temp1= Cyc_Tcutil_tvar2string(( struct Cyc_Absyn_Tvar*)(( struct
Cyc_List_List*) _check_null( tvs))->hd); struct _tagged_string _temp2= Cyc_Absynpp_ckind2string(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s::%.*s ", _temp1.last_plus_one -
_temp1.curr, _temp1.curr, _temp2.last_plus_one - _temp2.curr, _temp2.curr);});}
fprintf( sfile_to_file( Cyc_Stdio_stderr),"\n"); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr);} static struct Cyc_List_List* Cyc_Tcutil_warning_segs= 0;
static struct Cyc_List_List* Cyc_Tcutil_warning_msgs= 0; void Cyc_Tcutil_warn(
struct Cyc_Position_Segment* sg, struct _tagged_string msg){ Cyc_Tcutil_warning_segs=({
struct Cyc_List_List* _temp3=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp3->hd=( void*) sg; _temp3->tl= Cyc_Tcutil_warning_segs;
_temp3;}); Cyc_Tcutil_warning_msgs=({ struct Cyc_List_List* _temp4=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4->hd=( void*)({ struct
_tagged_string* _temp5=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp5[ 0]= msg; _temp5;}); _temp4->tl= Cyc_Tcutil_warning_msgs;
_temp4;});} void Cyc_Tcutil_flush_warnings(){ if( Cyc_Tcutil_warning_segs == 0){
return;} fprintf( sfile_to_file( Cyc_Stdio_stderr),"***Warnings***\n");{ struct
Cyc_List_List* _temp6= Cyc_Position_strings_of_segments( Cyc_Tcutil_warning_segs);
goto _LL7; _LL7: Cyc_Tcutil_warning_segs= 0; Cyc_Tcutil_warning_msgs=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_Tcutil_warning_msgs);
while( Cyc_Tcutil_warning_msgs != 0) {({ struct _tagged_string _temp8=( struct
_tagged_string)*(( struct _tagged_string*)(( struct Cyc_List_List*) _check_null(
_temp6))->hd); struct _tagged_string _temp9=*(( struct _tagged_string*)(( struct
Cyc_List_List*) _check_null( Cyc_Tcutil_warning_msgs))->hd); fprintf(
sfile_to_file( Cyc_Stdio_stderr),"%.*s: %.*s\n", _temp8.last_plus_one - _temp8.curr,
_temp8.curr, _temp9.last_plus_one - _temp9.curr, _temp9.curr);}); _temp6=((
struct Cyc_List_List*) _check_null( _temp6))->tl; Cyc_Tcutil_warning_msgs=((
struct Cyc_List_List*) _check_null( Cyc_Tcutil_warning_msgs))->tl;} fprintf(
sfile_to_file( Cyc_Stdio_stderr),"**************\n"); Cyc_Stdio_fflush(( struct
Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);}} struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set=
0; static int Cyc_Tcutil_fast_tvar_cmp( struct Cyc_Absyn_Tvar* tv1, struct Cyc_Absyn_Tvar*
tv2){ return*(( int*) _check_null( tv1->identity)) -*(( int*) _check_null( tv2->identity));}
void* Cyc_Tcutil_compress( void* t){ void* _temp10= t; struct Cyc_Core_Opt*
_temp22; int _temp24; struct Cyc_Core_Opt* _temp26; struct Cyc_Core_Opt* _temp28;
struct Cyc_Core_Opt* _temp30; int _temp32; struct Cyc_Core_Opt* _temp34; struct
Cyc_Core_Opt** _temp36; struct Cyc_Core_Opt* _temp37; struct Cyc_Core_Opt*
_temp39; struct Cyc_List_List* _temp41; struct _tuple1* _temp43; struct Cyc_Core_Opt*
_temp45; struct Cyc_Core_Opt** _temp47; struct Cyc_List_List* _temp48; struct
_tuple1* _temp50; _LL12: if(( unsigned int) _temp10 > 4u?*(( int*) _temp10) ==
Cyc_Absyn_Evar: 0){ _LL29: _temp28=(( struct Cyc_Absyn_Evar_struct*) _temp10)->f1;
goto _LL27; _LL27: _temp26=(( struct Cyc_Absyn_Evar_struct*) _temp10)->f2; if(
_temp26 == 0){ goto _LL25;} else{ goto _LL14;} _LL25: _temp24=(( struct Cyc_Absyn_Evar_struct*)
_temp10)->f3; goto _LL23; _LL23: _temp22=(( struct Cyc_Absyn_Evar_struct*)
_temp10)->f4; goto _LL13;} else{ goto _LL14;} _LL14: if(( unsigned int) _temp10
> 4u?*(( int*) _temp10) == Cyc_Absyn_Evar: 0){ _LL38: _temp37=(( struct Cyc_Absyn_Evar_struct*)
_temp10)->f1; goto _LL35; _LL35: _temp34=(( struct Cyc_Absyn_Evar_struct*)
_temp10)->f2; _temp36=&(( struct Cyc_Absyn_Evar_struct*) _temp10)->f2; goto
_LL33; _LL33: _temp32=(( struct Cyc_Absyn_Evar_struct*) _temp10)->f3; goto _LL31;
_LL31: _temp30=(( struct Cyc_Absyn_Evar_struct*) _temp10)->f4; goto _LL15;}
else{ goto _LL16;} _LL16: if(( unsigned int) _temp10 > 4u?*(( int*) _temp10) ==
Cyc_Absyn_TypedefType: 0){ _LL44: _temp43=(( struct Cyc_Absyn_TypedefType_struct*)
_temp10)->f1; goto _LL42; _LL42: _temp41=(( struct Cyc_Absyn_TypedefType_struct*)
_temp10)->f2; goto _LL40; _LL40: _temp39=(( struct Cyc_Absyn_TypedefType_struct*)
_temp10)->f3; if( _temp39 == 0){ goto _LL17;} else{ goto _LL18;}} else{ goto
_LL18;} _LL18: if(( unsigned int) _temp10 > 4u?*(( int*) _temp10) == Cyc_Absyn_TypedefType:
0){ _LL51: _temp50=(( struct Cyc_Absyn_TypedefType_struct*) _temp10)->f1; goto
_LL49; _LL49: _temp48=(( struct Cyc_Absyn_TypedefType_struct*) _temp10)->f2;
goto _LL46; _LL46: _temp45=(( struct Cyc_Absyn_TypedefType_struct*) _temp10)->f3;
_temp47=&(( struct Cyc_Absyn_TypedefType_struct*) _temp10)->f3; goto _LL19;}
else{ goto _LL20;} _LL20: goto _LL21; _LL13: return t; _LL15: { void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null(* _temp36))->v); if( t2 !=( void*)((
struct Cyc_Core_Opt*) _check_null(* _temp36))->v){* _temp36=({ struct Cyc_Core_Opt*
_temp52=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp52->v=( void*) t2; _temp52;});} return t2;} _LL17: return t; _LL19: { void*
t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(* _temp47))->v);
if( t2 !=( void*)(( struct Cyc_Core_Opt*) _check_null(* _temp47))->v){* _temp47=({
struct Cyc_Core_Opt* _temp53=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp53->v=( void*) t2; _temp53;});} return t2;} _LL21: return t;
_LL11:;} void* Cyc_Tcutil_copy_type( void* t); static struct Cyc_List_List* Cyc_Tcutil_copy_types(
struct Cyc_List_List* ts){ return(( struct Cyc_List_List*(*)( void*(* f)( void*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_type, ts);} static
struct Cyc_Absyn_Conref* Cyc_Tcutil_copy_conref( struct Cyc_Absyn_Conref* c){
void* _temp54=( void*) c->v; void* _temp62; struct Cyc_Absyn_Conref* _temp64;
_LL56: if( _temp54 ==( void*) Cyc_Absyn_No_constr){ goto _LL57;} else{ goto
_LL58;} _LL58: if(( unsigned int) _temp54 > 1u?*(( int*) _temp54) == Cyc_Absyn_Eq_constr:
0){ _LL63: _temp62=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp54)->f1;
goto _LL59;} else{ goto _LL60;} _LL60: if(( unsigned int) _temp54 > 1u?*(( int*)
_temp54) == Cyc_Absyn_Forward_constr: 0){ _LL65: _temp64=(( struct Cyc_Absyn_Forward_constr_struct*)
_temp54)->f1; goto _LL61;} else{ goto _LL55;} _LL57: return(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _LL59: return(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)( _temp62); _LL61: return(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* c)) Cyc_Tcutil_copy_conref)( _temp64); _LL55:;} static
struct Cyc_Absyn_Tvar* Cyc_Tcutil_copy_tvar( struct Cyc_Absyn_Tvar* tv){ return({
struct Cyc_Absyn_Tvar* _temp66=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof(
struct Cyc_Absyn_Tvar)); _temp66->name= tv->name; _temp66->identity= 0; _temp66->kind=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* c)) Cyc_Tcutil_copy_conref)(
tv->kind); _temp66;});} static struct _tuple2* Cyc_Tcutil_copy_arg( struct
_tuple2* arg){ void* _temp69; struct Cyc_Absyn_Tqual _temp71; struct Cyc_Core_Opt*
_temp73; struct _tuple2 _temp67=* arg; _LL74: _temp73= _temp67.f1; goto _LL72;
_LL72: _temp71= _temp67.f2; goto _LL70; _LL70: _temp69= _temp67.f3; goto _LL68;
_LL68: return({ struct _tuple2* _temp75=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp75->f1= _temp73; _temp75->f2= _temp71; _temp75->f3= Cyc_Tcutil_copy_type(
_temp69); _temp75;});} static struct _tuple4* Cyc_Tcutil_copy_tqt( struct
_tuple4* arg){ void* _temp78; struct Cyc_Absyn_Tqual _temp80; struct _tuple4
_temp76=* arg; _LL81: _temp80= _temp76.f1; goto _LL79; _LL79: _temp78= _temp76.f2;
goto _LL77; _LL77: return({ struct _tuple4* _temp82=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp82->f1= _temp80; _temp82->f2= Cyc_Tcutil_copy_type(
_temp78); _temp82;});} static struct Cyc_Absyn_Structfield* Cyc_Tcutil_copy_field(
struct Cyc_Absyn_Structfield* f){ return({ struct Cyc_Absyn_Structfield* _temp83=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp83->name= f->name; _temp83->tq= f->tq; _temp83->type=( void*) Cyc_Tcutil_copy_type((
void*) f->type); _temp83->width= f->width; _temp83->attributes= f->attributes;
_temp83;});} void* Cyc_Tcutil_copy_type( void* t){ void* _temp84= Cyc_Tcutil_compress(
t); struct Cyc_Core_Opt* _temp130; int _temp132; struct Cyc_Core_Opt* _temp134;
struct Cyc_Core_Opt* _temp136; struct Cyc_Absyn_Tvar* _temp138; struct Cyc_Absyn_TunionInfo
_temp140; void* _temp142; struct Cyc_List_List* _temp144; void* _temp146; struct
Cyc_Absyn_TunionFieldInfo _temp148; struct Cyc_List_List* _temp150; void*
_temp152; struct Cyc_Absyn_PtrInfo _temp154; struct Cyc_Absyn_Conref* _temp156;
struct Cyc_Absyn_Tqual _temp158; struct Cyc_Absyn_Conref* _temp160; void*
_temp162; void* _temp164; void* _temp166; void* _temp168; struct Cyc_Absyn_Exp*
_temp170; struct Cyc_Absyn_Tqual _temp172; void* _temp174; struct Cyc_Absyn_FnInfo
_temp176; struct Cyc_List_List* _temp178; struct Cyc_Absyn_VarargInfo* _temp180;
int _temp182; struct Cyc_List_List* _temp184; void* _temp186; struct Cyc_Core_Opt*
_temp188; struct Cyc_List_List* _temp190; struct Cyc_List_List* _temp192; struct
Cyc_Absyn_Structdecl** _temp194; struct Cyc_List_List* _temp196; struct _tuple1*
_temp198; struct Cyc_Absyn_Uniondecl** _temp200; struct Cyc_List_List* _temp202;
struct _tuple1* _temp204; struct Cyc_List_List* _temp206; struct Cyc_List_List*
_temp208; struct Cyc_Absyn_Enumdecl* _temp210; struct _tuple1* _temp212; void*
_temp214; struct Cyc_Core_Opt* _temp216; struct Cyc_List_List* _temp218; struct
_tuple1* _temp220; void* _temp222; struct Cyc_List_List* _temp224; _LL86: if(
_temp84 ==( void*) Cyc_Absyn_VoidType){ goto _LL87;} else{ goto _LL88;} _LL88:
if(( unsigned int) _temp84 > 4u?*(( int*) _temp84) == Cyc_Absyn_Evar: 0){ _LL137:
_temp136=(( struct Cyc_Absyn_Evar_struct*) _temp84)->f1; goto _LL135; _LL135:
_temp134=(( struct Cyc_Absyn_Evar_struct*) _temp84)->f2; goto _LL133; _LL133:
_temp132=(( struct Cyc_Absyn_Evar_struct*) _temp84)->f3; goto _LL131; _LL131:
_temp130=(( struct Cyc_Absyn_Evar_struct*) _temp84)->f4; goto _LL89;} else{ goto
_LL90;} _LL90: if(( unsigned int) _temp84 > 4u?*(( int*) _temp84) == Cyc_Absyn_VarType:
0){ _LL139: _temp138=(( struct Cyc_Absyn_VarType_struct*) _temp84)->f1; goto
_LL91;} else{ goto _LL92;} _LL92: if(( unsigned int) _temp84 > 4u?*(( int*)
_temp84) == Cyc_Absyn_TunionType: 0){ _LL141: _temp140=(( struct Cyc_Absyn_TunionType_struct*)
_temp84)->f1; _LL147: _temp146=( void*) _temp140.tunion_info; goto _LL145;
_LL145: _temp144= _temp140.targs; goto _LL143; _LL143: _temp142=( void*)
_temp140.rgn; goto _LL93;} else{ goto _LL94;} _LL94: if(( unsigned int) _temp84
> 4u?*(( int*) _temp84) == Cyc_Absyn_TunionFieldType: 0){ _LL149: _temp148=((
struct Cyc_Absyn_TunionFieldType_struct*) _temp84)->f1; _LL153: _temp152=( void*)
_temp148.field_info; goto _LL151; _LL151: _temp150= _temp148.targs; goto _LL95;}
else{ goto _LL96;} _LL96: if(( unsigned int) _temp84 > 4u?*(( int*) _temp84) ==
Cyc_Absyn_PointerType: 0){ _LL155: _temp154=(( struct Cyc_Absyn_PointerType_struct*)
_temp84)->f1; _LL165: _temp164=( void*) _temp154.elt_typ; goto _LL163; _LL163:
_temp162=( void*) _temp154.rgn_typ; goto _LL161; _LL161: _temp160= _temp154.nullable;
goto _LL159; _LL159: _temp158= _temp154.tq; goto _LL157; _LL157: _temp156=
_temp154.bounds; goto _LL97;} else{ goto _LL98;} _LL98: if(( unsigned int)
_temp84 > 4u?*(( int*) _temp84) == Cyc_Absyn_IntType: 0){ _LL169: _temp168=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp84)->f1; goto _LL167; _LL167:
_temp166=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp84)->f2; goto _LL99;}
else{ goto _LL100;} _LL100: if( _temp84 ==( void*) Cyc_Absyn_FloatType){ goto
_LL101;} else{ goto _LL102;} _LL102: if( _temp84 ==( void*) Cyc_Absyn_DoubleType){
goto _LL103;} else{ goto _LL104;} _LL104: if(( unsigned int) _temp84 > 4u?*((
int*) _temp84) == Cyc_Absyn_ArrayType: 0){ _LL175: _temp174=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp84)->f1; goto _LL173; _LL173: _temp172=((
struct Cyc_Absyn_ArrayType_struct*) _temp84)->f2; goto _LL171; _LL171: _temp170=((
struct Cyc_Absyn_ArrayType_struct*) _temp84)->f3; goto _LL105;} else{ goto
_LL106;} _LL106: if(( unsigned int) _temp84 > 4u?*(( int*) _temp84) == Cyc_Absyn_FnType:
0){ _LL177: _temp176=(( struct Cyc_Absyn_FnType_struct*) _temp84)->f1; _LL191:
_temp190= _temp176.tvars; goto _LL189; _LL189: _temp188= _temp176.effect; goto
_LL187; _LL187: _temp186=( void*) _temp176.ret_typ; goto _LL185; _LL185:
_temp184= _temp176.args; goto _LL183; _LL183: _temp182= _temp176.c_varargs; goto
_LL181; _LL181: _temp180= _temp176.cyc_varargs; goto _LL179; _LL179: _temp178=
_temp176.attributes; goto _LL107;} else{ goto _LL108;} _LL108: if(( unsigned int)
_temp84 > 4u?*(( int*) _temp84) == Cyc_Absyn_TupleType: 0){ _LL193: _temp192=((
struct Cyc_Absyn_TupleType_struct*) _temp84)->f1; goto _LL109;} else{ goto
_LL110;} _LL110: if(( unsigned int) _temp84 > 4u?*(( int*) _temp84) == Cyc_Absyn_StructType:
0){ _LL199: _temp198=(( struct Cyc_Absyn_StructType_struct*) _temp84)->f1; goto
_LL197; _LL197: _temp196=(( struct Cyc_Absyn_StructType_struct*) _temp84)->f2;
goto _LL195; _LL195: _temp194=(( struct Cyc_Absyn_StructType_struct*) _temp84)->f3;
goto _LL111;} else{ goto _LL112;} _LL112: if(( unsigned int) _temp84 > 4u?*((
int*) _temp84) == Cyc_Absyn_UnionType: 0){ _LL205: _temp204=(( struct Cyc_Absyn_UnionType_struct*)
_temp84)->f1; goto _LL203; _LL203: _temp202=(( struct Cyc_Absyn_UnionType_struct*)
_temp84)->f2; goto _LL201; _LL201: _temp200=(( struct Cyc_Absyn_UnionType_struct*)
_temp84)->f3; goto _LL113;} else{ goto _LL114;} _LL114: if(( unsigned int)
_temp84 > 4u?*(( int*) _temp84) == Cyc_Absyn_AnonStructType: 0){ _LL207:
_temp206=(( struct Cyc_Absyn_AnonStructType_struct*) _temp84)->f1; goto _LL115;}
else{ goto _LL116;} _LL116: if(( unsigned int) _temp84 > 4u?*(( int*) _temp84)
== Cyc_Absyn_AnonUnionType: 0){ _LL209: _temp208=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp84)->f1; goto _LL117;} else{ goto _LL118;} _LL118: if(( unsigned int)
_temp84 > 4u?*(( int*) _temp84) == Cyc_Absyn_EnumType: 0){ _LL213: _temp212=((
struct Cyc_Absyn_EnumType_struct*) _temp84)->f1; goto _LL211; _LL211: _temp210=((
struct Cyc_Absyn_EnumType_struct*) _temp84)->f2; goto _LL119;} else{ goto _LL120;}
_LL120: if(( unsigned int) _temp84 > 4u?*(( int*) _temp84) == Cyc_Absyn_RgnHandleType:
0){ _LL215: _temp214=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp84)->f1;
goto _LL121;} else{ goto _LL122;} _LL122: if(( unsigned int) _temp84 > 4u?*((
int*) _temp84) == Cyc_Absyn_TypedefType: 0){ _LL221: _temp220=(( struct Cyc_Absyn_TypedefType_struct*)
_temp84)->f1; goto _LL219; _LL219: _temp218=(( struct Cyc_Absyn_TypedefType_struct*)
_temp84)->f2; goto _LL217; _LL217: _temp216=(( struct Cyc_Absyn_TypedefType_struct*)
_temp84)->f3; goto _LL123;} else{ goto _LL124;} _LL124: if( _temp84 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL125;} else{ goto _LL126;} _LL126: if(( unsigned int)
_temp84 > 4u?*(( int*) _temp84) == Cyc_Absyn_AccessEff: 0){ _LL223: _temp222=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp84)->f1; goto _LL127;} else{
goto _LL128;} _LL128: if(( unsigned int) _temp84 > 4u?*(( int*) _temp84) == Cyc_Absyn_JoinEff:
0){ _LL225: _temp224=(( struct Cyc_Absyn_JoinEff_struct*) _temp84)->f1; goto
_LL129;} else{ goto _LL85;} _LL87: return t; _LL89: return t; _LL91: return(
void*)({ struct Cyc_Absyn_VarType_struct* _temp226=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp226[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp227; _temp227.tag= Cyc_Absyn_VarType; _temp227.f1= Cyc_Tcutil_copy_tvar(
_temp138); _temp227;}); _temp226;}); _LL93: return( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp228=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp228[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp229; _temp229.tag= Cyc_Absyn_TunionType;
_temp229.f1=({ struct Cyc_Absyn_TunionInfo _temp230; _temp230.tunion_info=( void*)
_temp146; _temp230.targs= Cyc_Tcutil_copy_types( _temp144); _temp230.rgn=( void*)
Cyc_Tcutil_copy_type( _temp142); _temp230;}); _temp229;}); _temp228;}); _LL95:
return( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp231=( struct Cyc_Absyn_TunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp231[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp232; _temp232.tag= Cyc_Absyn_TunionFieldType;
_temp232.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp233; _temp233.field_info=(
void*) _temp152; _temp233.targs= Cyc_Tcutil_copy_types( _temp150); _temp233;});
_temp232;}); _temp231;}); _LL97: { void* _temp234= Cyc_Tcutil_copy_type(
_temp164); goto _LL235; _LL235: { void* _temp236= Cyc_Tcutil_copy_type( _temp162);
goto _LL237; _LL237: { struct Cyc_Absyn_Conref* _temp238=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* c)) Cyc_Tcutil_copy_conref)( _temp160); goto _LL239;
_LL239: { struct Cyc_Absyn_Tqual _temp240= _temp158; goto _LL241; _LL241: {
struct Cyc_Absyn_Conref* _temp242=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
c)) Cyc_Tcutil_copy_conref)( _temp156); goto _LL243; _LL243: return( void*)({
struct Cyc_Absyn_PointerType_struct* _temp244=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp244[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp245; _temp245.tag= Cyc_Absyn_PointerType;
_temp245.f1=({ struct Cyc_Absyn_PtrInfo _temp246; _temp246.elt_typ=( void*)
_temp234; _temp246.rgn_typ=( void*) _temp236; _temp246.nullable= _temp238;
_temp246.tq= _temp240; _temp246.bounds= _temp242; _temp246;}); _temp245;});
_temp244;});}}}}} _LL99: return( void*)({ struct Cyc_Absyn_IntType_struct*
_temp247=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp247[ 0]=({ struct Cyc_Absyn_IntType_struct _temp248; _temp248.tag= Cyc_Absyn_IntType;
_temp248.f1=( void*) _temp168; _temp248.f2=( void*) _temp166; _temp248;});
_temp247;}); _LL101: goto _LL103; _LL103: return t; _LL105: return( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp249=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp249[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp250; _temp250.tag= Cyc_Absyn_ArrayType; _temp250.f1=(
void*) Cyc_Tcutil_copy_type( _temp174); _temp250.f2= _temp172; _temp250.f3=
_temp170; _temp250;}); _temp249;}); _LL107: { struct Cyc_List_List* _temp251=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_tvar, _temp190); goto
_LL252; _LL252: { struct Cyc_Core_Opt* _temp254= _temp188 == 0? 0:({ struct Cyc_Core_Opt*
_temp253=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp253->v=( void*) Cyc_Tcutil_copy_type(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp188))->v); _temp253;}); goto _LL255; _LL255: { void* _temp256=
Cyc_Tcutil_copy_type( _temp186); goto _LL257; _LL257: { struct Cyc_List_List*
_temp258=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple2*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_arg, _temp184); goto
_LL259; _LL259: { int _temp260= _temp182; goto _LL261; _LL261: { struct Cyc_Absyn_VarargInfo*
cyc_varargs2= 0; if( _temp180 != 0){ struct Cyc_Absyn_VarargInfo* cv=( struct
Cyc_Absyn_VarargInfo*) _check_null( _temp180); cyc_varargs2=({ struct Cyc_Absyn_VarargInfo*
_temp262=( struct Cyc_Absyn_VarargInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp262->name= cv->name; _temp262->tq= cv->tq; _temp262->type=( void*) Cyc_Tcutil_copy_type((
void*) cv->type); _temp262->rgn=( void*) Cyc_Tcutil_copy_type(( void*) cv->rgn);
_temp262->inject= cv->inject; _temp262;});}{ struct Cyc_List_List* _temp263=
_temp178; goto _LL264; _LL264: return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp265=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp265[ 0]=({ struct Cyc_Absyn_FnType_struct _temp266; _temp266.tag= Cyc_Absyn_FnType;
_temp266.f1=({ struct Cyc_Absyn_FnInfo _temp267; _temp267.tvars= _temp251;
_temp267.effect= _temp254; _temp267.ret_typ=( void*) _temp256; _temp267.args=
_temp258; _temp267.c_varargs= _temp260; _temp267.cyc_varargs= cyc_varargs2;
_temp267.attributes= _temp263; _temp267;}); _temp266;}); _temp265;});}}}}}}}
_LL109: return( void*)({ struct Cyc_Absyn_TupleType_struct* _temp268=( struct
Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp268[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp269; _temp269.tag= Cyc_Absyn_TupleType;
_temp269.f1=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_tqt, _temp192);
_temp269;}); _temp268;}); _LL111: return( void*)({ struct Cyc_Absyn_StructType_struct*
_temp270=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp270[ 0]=({ struct Cyc_Absyn_StructType_struct _temp271; _temp271.tag= Cyc_Absyn_StructType;
_temp271.f1= _temp198; _temp271.f2= Cyc_Tcutil_copy_types( _temp196); _temp271.f3=
0; _temp271;}); _temp270;}); _LL113: return( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp272=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp272[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp273; _temp273.tag= Cyc_Absyn_UnionType;
_temp273.f1= _temp204; _temp273.f2= Cyc_Tcutil_copy_types( _temp202); _temp273.f3=
0; _temp273;}); _temp272;}); _LL115: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp274=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp274[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp275; _temp275.tag= Cyc_Absyn_AnonStructType; _temp275.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_copy_field, _temp206); _temp275;}); _temp274;});
_LL117: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp276=(
struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp276[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp277; _temp277.tag=
Cyc_Absyn_AnonUnionType; _temp277.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_field,
_temp208); _temp277;}); _temp276;}); _LL119: return( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp278=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp278[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp279; _temp279.tag= Cyc_Absyn_EnumType;
_temp279.f1= _temp212; _temp279.f2= 0; _temp279;}); _temp278;}); _LL121: return(
void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp280=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp280[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp281; _temp281.tag= Cyc_Absyn_RgnHandleType;
_temp281.f1=( void*) Cyc_Tcutil_copy_type( _temp214); _temp281;}); _temp280;});
_LL123: return( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp282=( struct
Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp282[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp283; _temp283.tag= Cyc_Absyn_TypedefType;
_temp283.f1= _temp220; _temp283.f2= Cyc_Tcutil_copy_types( _temp218); _temp283.f3=
0; _temp283;}); _temp282;}); _LL125: return t; _LL127: return( void*)({ struct
Cyc_Absyn_AccessEff_struct* _temp284=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp284[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp285; _temp285.tag= Cyc_Absyn_AccessEff; _temp285.f1=(
void*) Cyc_Tcutil_copy_type( _temp222); _temp285;}); _temp284;}); _LL129: return(
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp286=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp286[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp287; _temp287.tag= Cyc_Absyn_JoinEff; _temp287.f1= Cyc_Tcutil_copy_types(
_temp224); _temp287;}); _temp286;}); _LL85:;} struct _tuple8{ void* f1; void* f2;
} ; static int Cyc_Tcutil_kind_leq( void* k1, void* k2){ if( k1 == k2){ return 1;}{
struct _tuple8 _temp289=({ struct _tuple8 _temp288; _temp288.f1= k1; _temp288.f2=
k2; _temp288;}); void* _temp299; void* _temp301; void* _temp303; void* _temp305;
void* _temp307; void* _temp309; _LL291: _LL302: _temp301= _temp289.f1; if(
_temp301 ==( void*) Cyc_Absyn_BoxKind){ goto _LL300;} else{ goto _LL293;} _LL300:
_temp299= _temp289.f2; if( _temp299 ==( void*) Cyc_Absyn_MemKind){ goto _LL292;}
else{ goto _LL293;} _LL293: _LL306: _temp305= _temp289.f1; if( _temp305 ==( void*)
Cyc_Absyn_BoxKind){ goto _LL304;} else{ goto _LL295;} _LL304: _temp303= _temp289.f2;
if( _temp303 ==( void*) Cyc_Absyn_AnyKind){ goto _LL294;} else{ goto _LL295;}
_LL295: _LL310: _temp309= _temp289.f1; if( _temp309 ==( void*) Cyc_Absyn_MemKind){
goto _LL308;} else{ goto _LL297;} _LL308: _temp307= _temp289.f2; if( _temp307 ==(
void*) Cyc_Absyn_AnyKind){ goto _LL296;} else{ goto _LL297;} _LL297: goto _LL298;
_LL292: return 1; _LL294: return 1; _LL296: return 1; _LL298: return 0; _LL290:;}}
void* Cyc_Tcutil_typ_kind( void* t){ void* _temp311= Cyc_Tcutil_compress( t);
struct Cyc_Core_Opt* _temp363; int _temp365; struct Cyc_Core_Opt* _temp367;
struct Cyc_Core_Opt* _temp369; struct Cyc_Absyn_Tvar* _temp371; void* _temp373;
void* _temp375; struct Cyc_Absyn_FnInfo _temp377; void* _temp379; struct Cyc_Absyn_TunionInfo
_temp381; struct Cyc_Absyn_TunionFieldInfo _temp383; struct Cyc_List_List*
_temp385; void* _temp387; struct Cyc_Absyn_Tunionfield* _temp389; struct Cyc_Absyn_Tuniondecl*
_temp391; struct Cyc_Absyn_TunionFieldInfo _temp393; struct Cyc_List_List*
_temp395; void* _temp397; struct Cyc_Absyn_UnknownTunionFieldInfo _temp399;
struct Cyc_Absyn_Structdecl** _temp401; struct Cyc_List_List* _temp403; struct
_tuple1* _temp405; struct Cyc_Absyn_Uniondecl** _temp407; struct Cyc_List_List*
_temp409; struct _tuple1* _temp411; struct Cyc_Absyn_Structdecl** _temp413;
struct Cyc_List_List* _temp415; struct _tuple1* _temp417; struct Cyc_Absyn_Uniondecl**
_temp419; struct Cyc_List_List* _temp421; struct _tuple1* _temp423; struct Cyc_List_List*
_temp425; struct Cyc_List_List* _temp427; struct Cyc_Absyn_Enumdecl* _temp429;
struct _tuple1* _temp431; struct Cyc_Absyn_PtrInfo _temp433; struct Cyc_Absyn_Exp*
_temp435; struct Cyc_Absyn_Tqual _temp437; void* _temp439; struct Cyc_List_List*
_temp441; struct Cyc_Core_Opt* _temp443; struct Cyc_List_List* _temp445; struct
_tuple1* _temp447; void* _temp449; struct Cyc_List_List* _temp451; _LL313: if((
unsigned int) _temp311 > 4u?*(( int*) _temp311) == Cyc_Absyn_Evar: 0){ _LL370:
_temp369=(( struct Cyc_Absyn_Evar_struct*) _temp311)->f1; goto _LL368; _LL368:
_temp367=(( struct Cyc_Absyn_Evar_struct*) _temp311)->f2; goto _LL366; _LL366:
_temp365=(( struct Cyc_Absyn_Evar_struct*) _temp311)->f3; goto _LL364; _LL364:
_temp363=(( struct Cyc_Absyn_Evar_struct*) _temp311)->f4; goto _LL314;} else{
goto _LL315;} _LL315: if(( unsigned int) _temp311 > 4u?*(( int*) _temp311) ==
Cyc_Absyn_VarType: 0){ _LL372: _temp371=(( struct Cyc_Absyn_VarType_struct*)
_temp311)->f1; goto _LL316;} else{ goto _LL317;} _LL317: if( _temp311 ==( void*)
Cyc_Absyn_VoidType){ goto _LL318;} else{ goto _LL319;} _LL319: if(( unsigned int)
_temp311 > 4u?*(( int*) _temp311) == Cyc_Absyn_IntType: 0){ _LL376: _temp375=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp311)->f1; goto _LL374; _LL374:
_temp373=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp311)->f2; goto _LL320;}
else{ goto _LL321;} _LL321: if( _temp311 ==( void*) Cyc_Absyn_FloatType){ goto
_LL322;} else{ goto _LL323;} _LL323: if( _temp311 ==( void*) Cyc_Absyn_DoubleType){
goto _LL324;} else{ goto _LL325;} _LL325: if(( unsigned int) _temp311 > 4u?*((
int*) _temp311) == Cyc_Absyn_FnType: 0){ _LL378: _temp377=(( struct Cyc_Absyn_FnType_struct*)
_temp311)->f1; goto _LL326;} else{ goto _LL327;} _LL327: if(( unsigned int)
_temp311 > 4u?*(( int*) _temp311) == Cyc_Absyn_RgnHandleType: 0){ _LL380:
_temp379=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp311)->f1; goto
_LL328;} else{ goto _LL329;} _LL329: if( _temp311 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL330;} else{ goto _LL331;} _LL331: if(( unsigned int) _temp311 > 4u?*((
int*) _temp311) == Cyc_Absyn_TunionType: 0){ _LL382: _temp381=(( struct Cyc_Absyn_TunionType_struct*)
_temp311)->f1; goto _LL332;} else{ goto _LL333;} _LL333: if(( unsigned int)
_temp311 > 4u?*(( int*) _temp311) == Cyc_Absyn_TunionFieldType: 0){ _LL384:
_temp383=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp311)->f1; _LL388:
_temp387=( void*) _temp383.field_info; if(*(( int*) _temp387) == Cyc_Absyn_KnownTunionfield){
_LL392: _temp391=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp387)->f1;
goto _LL390; _LL390: _temp389=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp387)->f2; goto _LL386;} else{ goto _LL335;} _LL386: _temp385= _temp383.targs;
goto _LL334;} else{ goto _LL335;} _LL335: if(( unsigned int) _temp311 > 4u?*((
int*) _temp311) == Cyc_Absyn_TunionFieldType: 0){ _LL394: _temp393=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp311)->f1; _LL398: _temp397=( void*) _temp393.field_info; if(*(( int*)
_temp397) == Cyc_Absyn_UnknownTunionfield){ _LL400: _temp399=(( struct Cyc_Absyn_UnknownTunionfield_struct*)
_temp397)->f1; goto _LL396;} else{ goto _LL337;} _LL396: _temp395= _temp393.targs;
goto _LL336;} else{ goto _LL337;} _LL337: if(( unsigned int) _temp311 > 4u?*((
int*) _temp311) == Cyc_Absyn_StructType: 0){ _LL406: _temp405=(( struct Cyc_Absyn_StructType_struct*)
_temp311)->f1; goto _LL404; _LL404: _temp403=(( struct Cyc_Absyn_StructType_struct*)
_temp311)->f2; goto _LL402; _LL402: _temp401=(( struct Cyc_Absyn_StructType_struct*)
_temp311)->f3; if( _temp401 == 0){ goto _LL338;} else{ goto _LL339;}} else{ goto
_LL339;} _LL339: if(( unsigned int) _temp311 > 4u?*(( int*) _temp311) == Cyc_Absyn_UnionType:
0){ _LL412: _temp411=(( struct Cyc_Absyn_UnionType_struct*) _temp311)->f1; goto
_LL410; _LL410: _temp409=(( struct Cyc_Absyn_UnionType_struct*) _temp311)->f2;
goto _LL408; _LL408: _temp407=(( struct Cyc_Absyn_UnionType_struct*) _temp311)->f3;
if( _temp407 == 0){ goto _LL340;} else{ goto _LL341;}} else{ goto _LL341;}
_LL341: if(( unsigned int) _temp311 > 4u?*(( int*) _temp311) == Cyc_Absyn_StructType:
0){ _LL418: _temp417=(( struct Cyc_Absyn_StructType_struct*) _temp311)->f1; goto
_LL416; _LL416: _temp415=(( struct Cyc_Absyn_StructType_struct*) _temp311)->f2;
goto _LL414; _LL414: _temp413=(( struct Cyc_Absyn_StructType_struct*) _temp311)->f3;
goto _LL342;} else{ goto _LL343;} _LL343: if(( unsigned int) _temp311 > 4u?*((
int*) _temp311) == Cyc_Absyn_UnionType: 0){ _LL424: _temp423=(( struct Cyc_Absyn_UnionType_struct*)
_temp311)->f1; goto _LL422; _LL422: _temp421=(( struct Cyc_Absyn_UnionType_struct*)
_temp311)->f2; goto _LL420; _LL420: _temp419=(( struct Cyc_Absyn_UnionType_struct*)
_temp311)->f3; goto _LL344;} else{ goto _LL345;} _LL345: if(( unsigned int)
_temp311 > 4u?*(( int*) _temp311) == Cyc_Absyn_AnonStructType: 0){ _LL426:
_temp425=(( struct Cyc_Absyn_AnonStructType_struct*) _temp311)->f1; goto _LL346;}
else{ goto _LL347;} _LL347: if(( unsigned int) _temp311 > 4u?*(( int*) _temp311)
== Cyc_Absyn_AnonUnionType: 0){ _LL428: _temp427=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp311)->f1; goto _LL348;} else{ goto _LL349;} _LL349: if(( unsigned int)
_temp311 > 4u?*(( int*) _temp311) == Cyc_Absyn_EnumType: 0){ _LL432: _temp431=((
struct Cyc_Absyn_EnumType_struct*) _temp311)->f1; goto _LL430; _LL430: _temp429=((
struct Cyc_Absyn_EnumType_struct*) _temp311)->f2; goto _LL350;} else{ goto
_LL351;} _LL351: if(( unsigned int) _temp311 > 4u?*(( int*) _temp311) == Cyc_Absyn_PointerType:
0){ _LL434: _temp433=(( struct Cyc_Absyn_PointerType_struct*) _temp311)->f1;
goto _LL352;} else{ goto _LL353;} _LL353: if(( unsigned int) _temp311 > 4u?*((
int*) _temp311) == Cyc_Absyn_ArrayType: 0){ _LL440: _temp439=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp311)->f1; goto _LL438; _LL438: _temp437=((
struct Cyc_Absyn_ArrayType_struct*) _temp311)->f2; goto _LL436; _LL436: _temp435=((
struct Cyc_Absyn_ArrayType_struct*) _temp311)->f3; goto _LL354;} else{ goto
_LL355;} _LL355: if(( unsigned int) _temp311 > 4u?*(( int*) _temp311) == Cyc_Absyn_TupleType:
0){ _LL442: _temp441=(( struct Cyc_Absyn_TupleType_struct*) _temp311)->f1; goto
_LL356;} else{ goto _LL357;} _LL357: if(( unsigned int) _temp311 > 4u?*(( int*)
_temp311) == Cyc_Absyn_TypedefType: 0){ _LL448: _temp447=(( struct Cyc_Absyn_TypedefType_struct*)
_temp311)->f1; goto _LL446; _LL446: _temp445=(( struct Cyc_Absyn_TypedefType_struct*)
_temp311)->f2; goto _LL444; _LL444: _temp443=(( struct Cyc_Absyn_TypedefType_struct*)
_temp311)->f3; goto _LL358;} else{ goto _LL359;} _LL359: if(( unsigned int)
_temp311 > 4u?*(( int*) _temp311) == Cyc_Absyn_AccessEff: 0){ _LL450: _temp449=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp311)->f1; goto _LL360;} else{
goto _LL361;} _LL361: if(( unsigned int) _temp311 > 4u?*(( int*) _temp311) ==
Cyc_Absyn_JoinEff: 0){ _LL452: _temp451=(( struct Cyc_Absyn_JoinEff_struct*)
_temp311)->f1; goto _LL362;} else{ goto _LL312;} _LL314: return( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp369))->v; _LL316: return(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp371->kind); _LL318: return( void*) Cyc_Absyn_MemKind;
_LL320: return _temp373 ==( void*) Cyc_Absyn_B4?( void*) Cyc_Absyn_BoxKind:(
void*) Cyc_Absyn_MemKind; _LL322: return( void*) Cyc_Absyn_MemKind; _LL324:
return( void*) Cyc_Absyn_MemKind; _LL326: return( void*) Cyc_Absyn_MemKind;
_LL328: return( void*) Cyc_Absyn_BoxKind; _LL330: return( void*) Cyc_Absyn_RgnKind;
_LL332: return( void*) Cyc_Absyn_BoxKind; _LL334: if( _temp389->typs == 0){
return( void*) Cyc_Absyn_BoxKind;} else{ return( void*) Cyc_Absyn_MemKind;}
_LL336: return(( void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
unsigned char* _temp453=( unsigned char*)"typ_kind: Unresolved TunionFieldType";
struct _tagged_string _temp454; _temp454.curr= _temp453; _temp454.base= _temp453;
_temp454.last_plus_one= _temp453 + 37; _temp454;})); _LL338: return( void*) Cyc_Absyn_AnyKind;
_LL340: return( void*) Cyc_Absyn_AnyKind; _LL342: return( void*) Cyc_Absyn_MemKind;
_LL344: return( void*) Cyc_Absyn_MemKind; _LL346: return( void*) Cyc_Absyn_MemKind;
_LL348: return( void*) Cyc_Absyn_MemKind; _LL350: return( void*) Cyc_Absyn_BoxKind;
_LL352: { void* _temp455=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp433.bounds))->v; void* _temp465; void*
_temp467; struct Cyc_Absyn_Exp* _temp469; struct Cyc_Absyn_Conref* _temp471;
_LL457: if(( unsigned int) _temp455 > 1u?*(( int*) _temp455) == Cyc_Absyn_Eq_constr:
0){ _LL466: _temp465=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp455)->f1;
if( _temp465 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL458;} else{ goto _LL459;}}
else{ goto _LL459;} _LL459: if(( unsigned int) _temp455 > 1u?*(( int*) _temp455)
== Cyc_Absyn_Eq_constr: 0){ _LL468: _temp467=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp455)->f1; if(( unsigned int) _temp467 > 1u?*(( int*) _temp467) == Cyc_Absyn_Upper_b:
0){ _LL470: _temp469=(( struct Cyc_Absyn_Upper_b_struct*) _temp467)->f1; goto
_LL460;} else{ goto _LL461;}} else{ goto _LL461;} _LL461: if( _temp455 ==( void*)
Cyc_Absyn_No_constr){ goto _LL462;} else{ goto _LL463;} _LL463: if((
unsigned int) _temp455 > 1u?*(( int*) _temp455) == Cyc_Absyn_Forward_constr: 0){
_LL472: _temp471=(( struct Cyc_Absyn_Forward_constr_struct*) _temp455)->f1; goto
_LL464;} else{ goto _LL456;} _LL458: return( void*) Cyc_Absyn_MemKind; _LL460:
return( void*) Cyc_Absyn_BoxKind; _LL462: return( void*) Cyc_Absyn_MemKind;
_LL464: return(( void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
unsigned char* _temp473=( unsigned char*)"typ_kind: forward constraint in ptr bounds";
struct _tagged_string _temp474; _temp474.curr= _temp473; _temp474.base= _temp473;
_temp474.last_plus_one= _temp473 + 43; _temp474;})); _LL456:;} _LL354: return(
void*) Cyc_Absyn_MemKind; _LL356: return( void*) Cyc_Absyn_MemKind; _LL358:
return(( void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ struct _tagged_string _temp475= Cyc_Absynpp_typ2string( t);
xprintf("typ_kind: typedef found: %.*s", _temp475.last_plus_one - _temp475.curr,
_temp475.curr);})); _LL360: return( void*) Cyc_Absyn_EffKind; _LL362: return(
void*) Cyc_Absyn_EffKind; _LL312:;} unsigned char Cyc_Tcutil_Unify[ 10u]="\000\000\000\000Unify";
int Cyc_Tcutil_unify( void* t1, void* t2){ struct _handler_cons _temp476;
_push_handler(& _temp476);{ int _temp478= 0; if( setjmp( _temp476.handler)){
_temp478= 1;} if( ! _temp478){ Cyc_Tcutil_unify_it( t1, t2);{ int _temp479= 1;
_npop_handler( 0u); return _temp479;}; _pop_handler();} else{ void* _temp477=(
void*) _exn_thrown; void* _temp481= _temp477; _LL483: if( _temp481 == Cyc_Tcutil_Unify){
goto _LL484;} else{ goto _LL485;} _LL485: goto _LL486; _LL484: return 0; _LL486:(
void) _throw( _temp481); _LL482:;}}} static void Cyc_Tcutil_occurslist( void*
evar, struct Cyc_List_List* env, struct Cyc_List_List* ts); static void Cyc_Tcutil_occurs(
void* evar, struct Cyc_List_List* env, void* t){ void* _temp487= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Tvar* _temp521; struct Cyc_Core_Opt* _temp523; struct Cyc_Core_Opt**
_temp525; int _temp526; struct Cyc_Core_Opt* _temp528; struct Cyc_Core_Opt*
_temp530; struct Cyc_Absyn_PtrInfo _temp532; struct Cyc_Absyn_Exp* _temp534;
struct Cyc_Absyn_Tqual _temp536; void* _temp538; struct Cyc_Absyn_FnInfo
_temp540; struct Cyc_List_List* _temp542; struct Cyc_Absyn_VarargInfo* _temp544;
int _temp546; struct Cyc_List_List* _temp548; void* _temp550; struct Cyc_Core_Opt*
_temp552; struct Cyc_List_List* _temp554; struct Cyc_List_List* _temp556; void*
_temp558; struct Cyc_Absyn_TunionInfo _temp560; void* _temp562; struct Cyc_List_List*
_temp564; void* _temp566; struct Cyc_Core_Opt* _temp568; struct Cyc_List_List*
_temp570; struct _tuple1* _temp572; struct Cyc_Absyn_TunionFieldInfo _temp574;
struct Cyc_List_List* _temp576; void* _temp578; struct Cyc_Absyn_Structdecl**
_temp580; struct Cyc_List_List* _temp582; struct _tuple1* _temp584; struct Cyc_List_List*
_temp586; struct Cyc_List_List* _temp588; void* _temp590; struct Cyc_List_List*
_temp592; _LL489: if(( unsigned int) _temp487 > 4u?*(( int*) _temp487) == Cyc_Absyn_VarType:
0){ _LL522: _temp521=(( struct Cyc_Absyn_VarType_struct*) _temp487)->f1; goto
_LL490;} else{ goto _LL491;} _LL491: if(( unsigned int) _temp487 > 4u?*(( int*)
_temp487) == Cyc_Absyn_Evar: 0){ _LL531: _temp530=(( struct Cyc_Absyn_Evar_struct*)
_temp487)->f1; goto _LL529; _LL529: _temp528=(( struct Cyc_Absyn_Evar_struct*)
_temp487)->f2; goto _LL527; _LL527: _temp526=(( struct Cyc_Absyn_Evar_struct*)
_temp487)->f3; goto _LL524; _LL524: _temp523=(( struct Cyc_Absyn_Evar_struct*)
_temp487)->f4; _temp525=&(( struct Cyc_Absyn_Evar_struct*) _temp487)->f4; goto
_LL492;} else{ goto _LL493;} _LL493: if(( unsigned int) _temp487 > 4u?*(( int*)
_temp487) == Cyc_Absyn_PointerType: 0){ _LL533: _temp532=(( struct Cyc_Absyn_PointerType_struct*)
_temp487)->f1; goto _LL494;} else{ goto _LL495;} _LL495: if(( unsigned int)
_temp487 > 4u?*(( int*) _temp487) == Cyc_Absyn_ArrayType: 0){ _LL539: _temp538=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp487)->f1; goto _LL537; _LL537:
_temp536=(( struct Cyc_Absyn_ArrayType_struct*) _temp487)->f2; goto _LL535;
_LL535: _temp534=(( struct Cyc_Absyn_ArrayType_struct*) _temp487)->f3; goto
_LL496;} else{ goto _LL497;} _LL497: if(( unsigned int) _temp487 > 4u?*(( int*)
_temp487) == Cyc_Absyn_FnType: 0){ _LL541: _temp540=(( struct Cyc_Absyn_FnType_struct*)
_temp487)->f1; _LL555: _temp554= _temp540.tvars; goto _LL553; _LL553: _temp552=
_temp540.effect; goto _LL551; _LL551: _temp550=( void*) _temp540.ret_typ; goto
_LL549; _LL549: _temp548= _temp540.args; goto _LL547; _LL547: _temp546= _temp540.c_varargs;
goto _LL545; _LL545: _temp544= _temp540.cyc_varargs; goto _LL543; _LL543:
_temp542= _temp540.attributes; goto _LL498;} else{ goto _LL499;} _LL499: if((
unsigned int) _temp487 > 4u?*(( int*) _temp487) == Cyc_Absyn_TupleType: 0){
_LL557: _temp556=(( struct Cyc_Absyn_TupleType_struct*) _temp487)->f1; goto
_LL500;} else{ goto _LL501;} _LL501: if(( unsigned int) _temp487 > 4u?*(( int*)
_temp487) == Cyc_Absyn_RgnHandleType: 0){ _LL559: _temp558=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp487)->f1; goto _LL502;} else{ goto _LL503;} _LL503: if(( unsigned int)
_temp487 > 4u?*(( int*) _temp487) == Cyc_Absyn_TunionType: 0){ _LL561: _temp560=((
struct Cyc_Absyn_TunionType_struct*) _temp487)->f1; _LL567: _temp566=( void*)
_temp560.tunion_info; goto _LL565; _LL565: _temp564= _temp560.targs; goto _LL563;
_LL563: _temp562=( void*) _temp560.rgn; goto _LL504;} else{ goto _LL505;} _LL505:
if(( unsigned int) _temp487 > 4u?*(( int*) _temp487) == Cyc_Absyn_TypedefType: 0){
_LL573: _temp572=(( struct Cyc_Absyn_TypedefType_struct*) _temp487)->f1; goto
_LL571; _LL571: _temp570=(( struct Cyc_Absyn_TypedefType_struct*) _temp487)->f2;
goto _LL569; _LL569: _temp568=(( struct Cyc_Absyn_TypedefType_struct*) _temp487)->f3;
goto _LL506;} else{ goto _LL507;} _LL507: if(( unsigned int) _temp487 > 4u?*((
int*) _temp487) == Cyc_Absyn_TunionFieldType: 0){ _LL575: _temp574=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp487)->f1; _LL579: _temp578=( void*) _temp574.field_info; goto _LL577;
_LL577: _temp576= _temp574.targs; goto _LL508;} else{ goto _LL509;} _LL509: if((
unsigned int) _temp487 > 4u?*(( int*) _temp487) == Cyc_Absyn_StructType: 0){
_LL585: _temp584=(( struct Cyc_Absyn_StructType_struct*) _temp487)->f1; goto
_LL583; _LL583: _temp582=(( struct Cyc_Absyn_StructType_struct*) _temp487)->f2;
goto _LL581; _LL581: _temp580=(( struct Cyc_Absyn_StructType_struct*) _temp487)->f3;
goto _LL510;} else{ goto _LL511;} _LL511: if(( unsigned int) _temp487 > 4u?*((
int*) _temp487) == Cyc_Absyn_AnonStructType: 0){ _LL587: _temp586=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp487)->f1; goto _LL512;} else{ goto _LL513;} _LL513: if(( unsigned int)
_temp487 > 4u?*(( int*) _temp487) == Cyc_Absyn_AnonUnionType: 0){ _LL589:
_temp588=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp487)->f1; goto _LL514;}
else{ goto _LL515;} _LL515: if(( unsigned int) _temp487 > 4u?*(( int*) _temp487)
== Cyc_Absyn_AccessEff: 0){ _LL591: _temp590=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp487)->f1; goto _LL516;} else{ goto _LL517;} _LL517: if(( unsigned int)
_temp487 > 4u?*(( int*) _temp487) == Cyc_Absyn_JoinEff: 0){ _LL593: _temp592=((
struct Cyc_Absyn_JoinEff_struct*) _temp487)->f1; goto _LL518;} else{ goto _LL519;}
_LL519: goto _LL520; _LL490: if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, env, _temp521)){( void) _throw(( void*) Cyc_Tcutil_Unify);}
goto _LL488; _LL492: if( t == evar){( void) _throw(( void*) Cyc_Tcutil_Unify);}
else{ if( _temp528 != 0){ Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp528))->v);} else{ int problem= 0;{ struct Cyc_List_List* s=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(* _temp525))->v; for(
0; s != 0; s=(( struct Cyc_List_List*) _check_null( s))->tl){ if( !(( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( s))->hd)){ problem=
1; break;}}} if( problem){ struct Cyc_List_List* _temp594= 0; goto _LL595;
_LL595:{ struct Cyc_List_List* s=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null(* _temp525))->v; for( 0; s != 0; s=(( struct Cyc_List_List*)
_check_null( s))->tl){ if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( s))->hd)){ _temp594=({ struct Cyc_List_List* _temp596=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp596->hd=( void*)(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( s))->hd); _temp596->tl= _temp594; _temp596;});}}}*
_temp525=({ struct Cyc_Core_Opt* _temp597=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp597->v=( void*) _temp594; _temp597;});}}}
goto _LL488; _LL494: Cyc_Tcutil_occurs( evar, env,( void*) _temp532.elt_typ);
Cyc_Tcutil_occurs( evar, env,( void*) _temp532.rgn_typ); goto _LL488; _LL496:
Cyc_Tcutil_occurs( evar, env, _temp538); goto _LL488; _LL498: if( _temp552 != 0){
Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp552))->v);} Cyc_Tcutil_occurs( evar, env, _temp550); for( 0; _temp548 != 0;
_temp548=(( struct Cyc_List_List*) _check_null( _temp548))->tl){ Cyc_Tcutil_occurs(
evar, env,(*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp548))->hd)).f3);}
if( _temp544 != 0){ int _temp600; void* _temp602; void* _temp604; struct Cyc_Absyn_Tqual
_temp606; struct Cyc_Core_Opt* _temp608; struct Cyc_Absyn_VarargInfo _temp598=*((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp544)); _LL609: _temp608=
_temp598.name; goto _LL607; _LL607: _temp606= _temp598.tq; goto _LL605; _LL605:
_temp604=( void*) _temp598.type; goto _LL603; _LL603: _temp602=( void*) _temp598.rgn;
goto _LL601; _LL601: _temp600= _temp598.inject; goto _LL599; _LL599: Cyc_Tcutil_occurs(
evar, env, _temp604); Cyc_Tcutil_occurs( evar, env, _temp602);} goto _LL488;
_LL500: for( 0; _temp556 != 0; _temp556=(( struct Cyc_List_List*) _check_null(
_temp556))->tl){ Cyc_Tcutil_occurs( evar, env,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp556))->hd)).f2);} goto _LL488; _LL502: Cyc_Tcutil_occurs( evar,
env, _temp558); goto _LL488; _LL504: Cyc_Tcutil_occurs( evar, env, _temp562);
Cyc_Tcutil_occurslist( evar, env, _temp564); goto _LL488; _LL506: _temp576=
_temp570; goto _LL508; _LL508: _temp582= _temp576; goto _LL510; _LL510: Cyc_Tcutil_occurslist(
evar, env, _temp582); goto _LL488; _LL512: _temp588= _temp586; goto _LL514;
_LL514: for( 0; _temp588 != 0; _temp588=(( struct Cyc_List_List*) _check_null(
_temp588))->tl){ Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp588))->hd)->type);} goto _LL488; _LL516:
Cyc_Tcutil_occurs( evar, env, _temp590); goto _LL488; _LL518: Cyc_Tcutil_occurslist(
evar, env, _temp592); goto _LL488; _LL520: goto _LL488; _LL488:;} static void
Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List* env, struct Cyc_List_List*
ts){ for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){ Cyc_Tcutil_occurs(
evar, env,( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);}} static
void Cyc_Tcutil_unify_list( struct Cyc_List_List* t1, struct Cyc_List_List* t2){
struct _handler_cons _temp610; _push_handler(& _temp610);{ int _temp612= 0; if(
setjmp( _temp610.handler)){ _temp612= 1;} if( ! _temp612){(( void(*)( void(* f)(
void*, void*), struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_iter2)(
Cyc_Tcutil_unify_it, t1, t2);; _pop_handler();} else{ void* _temp611=( void*)
_exn_thrown; void* _temp614= _temp611; _LL616: if( _temp614 == Cyc_List_List_mismatch){
goto _LL617;} else{ goto _LL618;} _LL618: goto _LL619; _LL617:( void) _throw((
void*) Cyc_Tcutil_Unify); _LL619:( void) _throw( _temp614); _LL615:;}}} static
void Cyc_Tcutil_unify_tqual( struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual
tq2){ if(( tq1.q_const != tq2.q_const? 1: tq1.q_volatile != tq2.q_volatile)? 1:
tq1.q_restrict != tq2.q_restrict){( void) _throw(( void*) Cyc_Tcutil_Unify);}}
int Cyc_Tcutil_equal_tqual( struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual
tq2){ return( tq1.q_const == tq2.q_const? tq1.q_volatile == tq2.q_volatile: 0)?
tq1.q_restrict == tq2.q_restrict: 0;} static void Cyc_Tcutil_unify_it_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y){ x=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
x); y=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
y); if( x == y){ return;}{ void* _temp620=( void*) x->v; struct Cyc_Absyn_Conref*
_temp628; void* _temp630; _LL622: if( _temp620 ==( void*) Cyc_Absyn_No_constr){
goto _LL623;} else{ goto _LL624;} _LL624: if(( unsigned int) _temp620 > 1u?*((
int*) _temp620) == Cyc_Absyn_Forward_constr: 0){ _LL629: _temp628=(( struct Cyc_Absyn_Forward_constr_struct*)
_temp620)->f1; goto _LL625;} else{ goto _LL626;} _LL626: if(( unsigned int)
_temp620 > 1u?*(( int*) _temp620) == Cyc_Absyn_Eq_constr: 0){ _LL631: _temp630=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp620)->f1; goto _LL627;} else{
goto _LL621;} _LL623:( void*)( x->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp632=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp632[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp633; _temp633.tag= Cyc_Absyn_Forward_constr; _temp633.f1= y; _temp633;});
_temp632;}))); return; _LL625:( void) _throw((( void*(*)( struct _tagged_string
msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp634=( unsigned char*)"unify_conref: forward after compress";
struct _tagged_string _temp635; _temp635.curr= _temp634; _temp635.base= _temp634;
_temp635.last_plus_one= _temp634 + 37; _temp635;}))); _LL627: { void* _temp636=(
void*) y->v; struct Cyc_Absyn_Conref* _temp644; void* _temp646; _LL638: if(
_temp636 ==( void*) Cyc_Absyn_No_constr){ goto _LL639;} else{ goto _LL640;}
_LL640: if(( unsigned int) _temp636 > 1u?*(( int*) _temp636) == Cyc_Absyn_Forward_constr:
0){ _LL645: _temp644=(( struct Cyc_Absyn_Forward_constr_struct*) _temp636)->f1;
goto _LL641;} else{ goto _LL642;} _LL642: if(( unsigned int) _temp636 > 1u?*((
int*) _temp636) == Cyc_Absyn_Eq_constr: 0){ _LL647: _temp646=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp636)->f1; goto _LL643;} else{ goto _LL637;}
_LL639:( void*)( y->v=( void*)(( void*) x->v)); return; _LL641:( void) _throw(((
void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char*
_temp648=( unsigned char*)"unify_conref: forward after compress(2)"; struct
_tagged_string _temp649; _temp649.curr= _temp648; _temp649.base= _temp648;
_temp649.last_plus_one= _temp648 + 40; _temp649;}))); _LL643: if( cmp( _temp630,
_temp646) != 0){( void) _throw(( void*) Cyc_Tcutil_Unify);} return; _LL637:;}
_LL621:;}} static int Cyc_Tcutil_unify_conrefs( int(* cmp)( void*, void*),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y){ struct _handler_cons
_temp650; _push_handler(& _temp650);{ int _temp652= 0; if( setjmp( _temp650.handler)){
_temp652= 1;} if( ! _temp652){(( void(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)( cmp, x, y);{ int
_temp653= 1; _npop_handler( 0u); return _temp653;}; _pop_handler();} else{ void*
_temp651=( void*) _exn_thrown; void* _temp655= _temp651; _LL657: if( _temp655 ==
Cyc_Tcutil_Unify){ goto _LL658;} else{ goto _LL659;} _LL659: goto _LL660; _LL658:
return 0; _LL660:( void) _throw( _temp655); _LL656:;}}} static int Cyc_Tcutil_boundscmp(
void* b1, void* b2){ struct _tuple8 _temp662=({ struct _tuple8 _temp661;
_temp661.f1= b1; _temp661.f2= b2; _temp661;}); void* _temp672; void* _temp674;
void* _temp676; void* _temp678; void* _temp680; void* _temp682; void* _temp684;
struct Cyc_Absyn_Exp* _temp686; void* _temp688; struct Cyc_Absyn_Exp* _temp690;
_LL664: _LL675: _temp674= _temp662.f1; if( _temp674 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL673;} else{ goto _LL666;} _LL673: _temp672= _temp662.f2; if( _temp672 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL665;} else{ goto _LL666;} _LL666: _LL679:
_temp678= _temp662.f1; if( _temp678 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL677;}
else{ goto _LL668;} _LL677: _temp676= _temp662.f2; goto _LL667; _LL668: _LL683:
_temp682= _temp662.f1; goto _LL681; _LL681: _temp680= _temp662.f2; if( _temp680
==( void*) Cyc_Absyn_Unknown_b){ goto _LL669;} else{ goto _LL670;} _LL670:
_LL689: _temp688= _temp662.f1; if(( unsigned int) _temp688 > 1u?*(( int*)
_temp688) == Cyc_Absyn_Upper_b: 0){ _LL691: _temp690=(( struct Cyc_Absyn_Upper_b_struct*)
_temp688)->f1; goto _LL685;} else{ goto _LL663;} _LL685: _temp684= _temp662.f2;
if(( unsigned int) _temp684 > 1u?*(( int*) _temp684) == Cyc_Absyn_Upper_b: 0){
_LL687: _temp686=(( struct Cyc_Absyn_Upper_b_struct*) _temp684)->f1; goto _LL671;}
else{ goto _LL663;} _LL665: return 0; _LL667: return - 1; _LL669: return 1;
_LL671: { int i1=( int) Cyc_Evexp_eval_const_uint_exp( _temp690); int i2=( int)
Cyc_Evexp_eval_const_uint_exp( _temp686); if( i1 == i2){ return 0;} if( i1 < i2){
return - 1;} return 1;} _LL663:;} static int Cyc_Tcutil_equal_att( void* a1,
void* a2){ if( a1 == a2){ return 1;}{ struct _tuple8 _temp693=({ struct _tuple8
_temp692; _temp692.f1= a1; _temp692.f2= a2; _temp692;}); void* _temp703; int
_temp705; void* _temp707; int _temp709; void* _temp711; int _temp713; void*
_temp715; int _temp717; void* _temp719; struct _tagged_string _temp721; void*
_temp723; struct _tagged_string _temp725; _LL695: _LL708: _temp707= _temp693.f1;
if(( unsigned int) _temp707 > 16u?*(( int*) _temp707) == Cyc_Absyn_Regparm_att:
0){ _LL710: _temp709=(( struct Cyc_Absyn_Regparm_att_struct*) _temp707)->f1;
goto _LL704;} else{ goto _LL697;} _LL704: _temp703= _temp693.f2; if((
unsigned int) _temp703 > 16u?*(( int*) _temp703) == Cyc_Absyn_Regparm_att: 0){
_LL706: _temp705=(( struct Cyc_Absyn_Regparm_att_struct*) _temp703)->f1; goto
_LL696;} else{ goto _LL697;} _LL697: _LL716: _temp715= _temp693.f1; if((
unsigned int) _temp715 > 16u?*(( int*) _temp715) == Cyc_Absyn_Aligned_att: 0){
_LL718: _temp717=(( struct Cyc_Absyn_Aligned_att_struct*) _temp715)->f1; goto
_LL712;} else{ goto _LL699;} _LL712: _temp711= _temp693.f2; if(( unsigned int)
_temp711 > 16u?*(( int*) _temp711) == Cyc_Absyn_Aligned_att: 0){ _LL714:
_temp713=(( struct Cyc_Absyn_Aligned_att_struct*) _temp711)->f1; goto _LL698;}
else{ goto _LL699;} _LL699: _LL724: _temp723= _temp693.f1; if(( unsigned int)
_temp723 > 16u?*(( int*) _temp723) == Cyc_Absyn_Section_att: 0){ _LL726:
_temp725=(( struct Cyc_Absyn_Section_att_struct*) _temp723)->f1; goto _LL720;}
else{ goto _LL701;} _LL720: _temp719= _temp693.f2; if(( unsigned int) _temp719 >
16u?*(( int*) _temp719) == Cyc_Absyn_Section_att: 0){ _LL722: _temp721=(( struct
Cyc_Absyn_Section_att_struct*) _temp719)->f1; goto _LL700;} else{ goto _LL701;}
_LL701: goto _LL702; _LL696: _temp717= _temp709; _temp713= _temp705; goto _LL698;
_LL698: return _temp717 == _temp713; _LL700: return Cyc_String_strcmp( _temp725,
_temp721) == 0; _LL702: return 0; _LL694:;}} int Cyc_Tcutil_same_atts( struct
Cyc_List_List* a1, struct Cyc_List_List* a2){{ struct Cyc_List_List* a= a1; for(
0; a != 0; a=(( struct Cyc_List_List*) _check_null( a))->tl){ if( !(( int(*)(
int(* pred)( void*, void*), void* env, struct Cyc_List_List* x)) Cyc_List_exists_c)(
Cyc_Tcutil_equal_att,( void*)(( struct Cyc_List_List*) _check_null( a))->hd, a2)){
return 0;}}}{ struct Cyc_List_List* a= a2; for( 0; a != 0; a=(( struct Cyc_List_List*)
_check_null( a))->tl){ if( !(( int(*)( int(* pred)( void*, void*), void* env,
struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcutil_equal_att,( void*)((
struct Cyc_List_List*) _check_null( a))->hd, a1)){ return 0;}}} return 1;}
static void Cyc_Tcutil_normalize_effect( void* e){ e= Cyc_Tcutil_compress( e);{
void* _temp727= e; struct Cyc_List_List* _temp733; struct Cyc_List_List**
_temp735; _LL729: if(( unsigned int) _temp727 > 4u?*(( int*) _temp727) == Cyc_Absyn_JoinEff:
0){ _LL734: _temp733=(( struct Cyc_Absyn_JoinEff_struct*) _temp727)->f1;
_temp735=&(( struct Cyc_Absyn_JoinEff_struct*) _temp727)->f1; goto _LL730;}
else{ goto _LL731;} _LL731: goto _LL732; _LL730: { int nested_join= 0;{ struct
Cyc_List_List* effs=* _temp735; for( 0; effs != 0; effs=(( struct Cyc_List_List*)
_check_null( effs))->tl){ void* _temp736=( void*)(( struct Cyc_List_List*)
_check_null( effs))->hd; goto _LL737; _LL737: Cyc_Tcutil_normalize_effect(
_temp736);{ void* _temp738= Cyc_Tcutil_compress( _temp736); struct Cyc_List_List*
_temp744; _LL740: if(( unsigned int) _temp738 > 4u?*(( int*) _temp738) == Cyc_Absyn_JoinEff:
0){ _LL745: _temp744=(( struct Cyc_Absyn_JoinEff_struct*) _temp738)->f1; goto
_LL741;} else{ goto _LL742;} _LL742: goto _LL743; _LL741: nested_join= 1; goto
_LL739; _LL743: goto _LL739; _LL739:;}}} if( ! nested_join){ return;}{ struct
Cyc_List_List* effects= 0;{ struct Cyc_List_List* effs=* _temp735; for( 0; effs
!= 0; effs=(( struct Cyc_List_List*) _check_null( effs))->tl){ void* _temp746=
Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*) _check_null( effs))->hd);
struct Cyc_List_List* _temp752; _LL748: if(( unsigned int) _temp746 > 4u?*(( int*)
_temp746) == Cyc_Absyn_JoinEff: 0){ _LL753: _temp752=(( struct Cyc_Absyn_JoinEff_struct*)
_temp746)->f1; goto _LL749;} else{ goto _LL750;} _LL750: goto _LL751; _LL749:
effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_revappend)( _temp752, effects); goto _LL747; _LL751: effects=({
struct Cyc_List_List* _temp754=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp754->hd=( void*) _temp746; _temp754->tl= effects;
_temp754;}); goto _LL747; _LL747:;}}* _temp735=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( effects); goto _LL728;}} _LL732:
goto _LL728; _LL728:;}} struct _tuple9{ void* f1; struct Cyc_List_List* f2;
struct Cyc_List_List* f3; } ; static struct _tuple9* Cyc_Tcutil_get_effect_evar(
void* t){ void* _temp755= Cyc_Tcutil_compress( t); struct Cyc_List_List*
_temp763; struct Cyc_List_List _temp765; struct Cyc_List_List* _temp766; void*
_temp768; struct Cyc_Core_Opt* _temp770; int _temp772; struct Cyc_Core_Opt*
_temp774; struct Cyc_Core_Opt* _temp776; _LL757: if(( unsigned int) _temp755 > 4u?*((
int*) _temp755) == Cyc_Absyn_JoinEff: 0){ _LL764: _temp763=(( struct Cyc_Absyn_JoinEff_struct*)
_temp755)->f1; if( _temp763 == 0){ goto _LL759;} else{ _temp765=* _temp763;
_LL769: _temp768=( void*) _temp765.hd; goto _LL767; _LL767: _temp766= _temp765.tl;
goto _LL758;}} else{ goto _LL759;} _LL759: if(( unsigned int) _temp755 > 4u?*((
int*) _temp755) == Cyc_Absyn_Evar: 0){ _LL777: _temp776=(( struct Cyc_Absyn_Evar_struct*)
_temp755)->f1; goto _LL775; _LL775: _temp774=(( struct Cyc_Absyn_Evar_struct*)
_temp755)->f2; goto _LL773; _LL773: _temp772=(( struct Cyc_Absyn_Evar_struct*)
_temp755)->f3; goto _LL771; _LL771: _temp770=(( struct Cyc_Absyn_Evar_struct*)
_temp755)->f4; goto _LL760;} else{ goto _LL761;} _LL761: goto _LL762; _LL758: {
void* _temp778= Cyc_Tcutil_compress( _temp768); struct Cyc_Core_Opt* _temp784;
int _temp786; struct Cyc_Core_Opt* _temp788; struct Cyc_Core_Opt* _temp790;
_LL780: if(( unsigned int) _temp778 > 4u?*(( int*) _temp778) == Cyc_Absyn_Evar:
0){ _LL791: _temp790=(( struct Cyc_Absyn_Evar_struct*) _temp778)->f1; goto
_LL789; _LL789: _temp788=(( struct Cyc_Absyn_Evar_struct*) _temp778)->f2; goto
_LL787; _LL787: _temp786=(( struct Cyc_Absyn_Evar_struct*) _temp778)->f3; goto
_LL785; _LL785: _temp784=(( struct Cyc_Absyn_Evar_struct*) _temp778)->f4; goto
_LL781;} else{ goto _LL782;} _LL782: goto _LL783; _LL781: return({ struct
_tuple9* _temp792=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp792->f1= _temp768; _temp792->f2= _temp766; _temp792->f3=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp784))->v; _temp792;}); _LL783: return 0;
_LL779:;} _LL760: if( _temp776 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp776))->v !=( void*) Cyc_Absyn_EffKind){(( void(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp793=(
unsigned char*)"effect evar has wrong kind"; struct _tagged_string _temp794;
_temp794.curr= _temp793; _temp794.base= _temp793; _temp794.last_plus_one=
_temp793 + 27; _temp794;}));} return({ struct _tuple9* _temp795=( struct _tuple9*)
GC_malloc( sizeof( struct _tuple9)); _temp795->f1= t; _temp795->f2= 0; _temp795->f3=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp770))->v;
_temp795;}); _LL762: return 0; _LL756:;} static struct Cyc_Core_Opt Cyc_Tcutil_ek={(
void*)(( void*) 4u)}; int Cyc_Tcutil_region_in_effect( int constrain, void* r,
void* e){ r= Cyc_Tcutil_compress( r); if( r ==( void*) Cyc_Absyn_HeapRgn){
return 1;}{ void* _temp796= Cyc_Tcutil_compress( e); void* _temp806; struct Cyc_List_List*
_temp808; struct Cyc_Core_Opt* _temp810; int _temp812; struct Cyc_Core_Opt*
_temp814; struct Cyc_Core_Opt** _temp816; struct Cyc_Core_Opt* _temp817; _LL798:
if(( unsigned int) _temp796 > 4u?*(( int*) _temp796) == Cyc_Absyn_AccessEff: 0){
_LL807: _temp806=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp796)->f1;
goto _LL799;} else{ goto _LL800;} _LL800: if(( unsigned int) _temp796 > 4u?*((
int*) _temp796) == Cyc_Absyn_JoinEff: 0){ _LL809: _temp808=(( struct Cyc_Absyn_JoinEff_struct*)
_temp796)->f1; goto _LL801;} else{ goto _LL802;} _LL802: if(( unsigned int)
_temp796 > 4u?*(( int*) _temp796) == Cyc_Absyn_Evar: 0){ _LL818: _temp817=((
struct Cyc_Absyn_Evar_struct*) _temp796)->f1; goto _LL815; _LL815: _temp814=((
struct Cyc_Absyn_Evar_struct*) _temp796)->f2; _temp816=&(( struct Cyc_Absyn_Evar_struct*)
_temp796)->f2; goto _LL813; _LL813: _temp812=(( struct Cyc_Absyn_Evar_struct*)
_temp796)->f3; goto _LL811; _LL811: _temp810=(( struct Cyc_Absyn_Evar_struct*)
_temp796)->f4; goto _LL803;} else{ goto _LL804;} _LL804: goto _LL805; _LL799:
if( constrain){ return Cyc_Tcutil_unify( r, _temp806);} Cyc_Tcutil_compress(
_temp806); if( r == _temp806){ return 1;}{ struct _tuple8 _temp820=({ struct
_tuple8 _temp819; _temp819.f1= r; _temp819.f2= _temp806; _temp819;}); void*
_temp826; struct Cyc_Absyn_Tvar* _temp828; void* _temp830; struct Cyc_Absyn_Tvar*
_temp832; _LL822: _LL831: _temp830= _temp820.f1; if(( unsigned int) _temp830 > 4u?*((
int*) _temp830) == Cyc_Absyn_VarType: 0){ _LL833: _temp832=(( struct Cyc_Absyn_VarType_struct*)
_temp830)->f1; goto _LL827;} else{ goto _LL824;} _LL827: _temp826= _temp820.f2;
if(( unsigned int) _temp826 > 4u?*(( int*) _temp826) == Cyc_Absyn_VarType: 0){
_LL829: _temp828=(( struct Cyc_Absyn_VarType_struct*) _temp826)->f1; goto _LL823;}
else{ goto _LL824;} _LL824: goto _LL825; _LL823: return Cyc_Absyn_tvar_cmp(
_temp832, _temp828) == 0; _LL825: return 0; _LL821:;} _LL801: for( 0; _temp808
!= 0; _temp808=(( struct Cyc_List_List*) _check_null( _temp808))->tl){ if( Cyc_Tcutil_region_in_effect(
constrain, r,( void*)(( struct Cyc_List_List*) _check_null( _temp808))->hd)){
return 1;}} return 0; _LL803: if( _temp817 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp817))->v !=( void*) Cyc_Absyn_EffKind){(( void(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp834=(
unsigned char*)"effect evar has wrong kind"; struct _tagged_string _temp835;
_temp835.curr= _temp834; _temp835.base= _temp834; _temp835.last_plus_one=
_temp834 + 27; _temp835;}));} if( ! constrain){ return 0;}{ void* _temp836= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek, _temp810); goto _LL837; _LL837: Cyc_Tcutil_occurs(
_temp836,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp810))->v,
r);{ struct Cyc_Absyn_JoinEff_struct* _temp844=({ struct Cyc_Absyn_JoinEff_struct*
_temp838=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp838[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp839; _temp839.tag= Cyc_Absyn_JoinEff;
_temp839.f1=({ struct Cyc_List_List* _temp840=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp840->hd=( void*) _temp836; _temp840->tl=({
struct Cyc_List_List* _temp841=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp841->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp842=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp842[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp843; _temp843.tag= Cyc_Absyn_AccessEff;
_temp843.f1=( void*) r; _temp843;}); _temp842;})); _temp841->tl= 0; _temp841;});
_temp840;}); _temp839;}); _temp838;}); goto _LL845; _LL845:* _temp816=({ struct
Cyc_Core_Opt* _temp846=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp846->v=( void*)(( void*) _temp844); _temp846;}); return 1;}} _LL805: return
0; _LL797:;}} static int Cyc_Tcutil_variable_in_effect( int constrain, struct
Cyc_Absyn_Tvar* v, void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp847= e;
struct Cyc_Absyn_Tvar* _temp857; struct Cyc_List_List* _temp859; struct Cyc_Core_Opt*
_temp861; int _temp863; struct Cyc_Core_Opt* _temp865; struct Cyc_Core_Opt**
_temp867; struct Cyc_Core_Opt* _temp868; _LL849: if(( unsigned int) _temp847 > 4u?*((
int*) _temp847) == Cyc_Absyn_VarType: 0){ _LL858: _temp857=(( struct Cyc_Absyn_VarType_struct*)
_temp847)->f1; goto _LL850;} else{ goto _LL851;} _LL851: if(( unsigned int)
_temp847 > 4u?*(( int*) _temp847) == Cyc_Absyn_JoinEff: 0){ _LL860: _temp859=((
struct Cyc_Absyn_JoinEff_struct*) _temp847)->f1; goto _LL852;} else{ goto _LL853;}
_LL853: if(( unsigned int) _temp847 > 4u?*(( int*) _temp847) == Cyc_Absyn_Evar:
0){ _LL869: _temp868=(( struct Cyc_Absyn_Evar_struct*) _temp847)->f1; goto
_LL866; _LL866: _temp865=(( struct Cyc_Absyn_Evar_struct*) _temp847)->f2;
_temp867=&(( struct Cyc_Absyn_Evar_struct*) _temp847)->f2; goto _LL864; _LL864:
_temp863=(( struct Cyc_Absyn_Evar_struct*) _temp847)->f3; goto _LL862; _LL862:
_temp861=(( struct Cyc_Absyn_Evar_struct*) _temp847)->f4; goto _LL854;} else{
goto _LL855;} _LL855: goto _LL856; _LL850: return Cyc_Absyn_tvar_cmp( v,
_temp857) == 0; _LL852: for( 0; _temp859 != 0; _temp859=(( struct Cyc_List_List*)
_check_null( _temp859))->tl){ if( Cyc_Tcutil_variable_in_effect( constrain, v,(
void*)(( struct Cyc_List_List*) _check_null( _temp859))->hd)){ return 1;}}
return 0; _LL854: if( _temp868 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp868))->v !=( void*) Cyc_Absyn_EffKind){(( void(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp870=(
unsigned char*)"effect evar has wrong kind"; struct _tagged_string _temp871;
_temp871.curr= _temp870; _temp871.base= _temp870; _temp871.last_plus_one=
_temp870 + 27; _temp871;}));}{ void* _temp872= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek, _temp861); goto _LL873; _LL873: if( !(( int(*)( int(* compare)(
struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct
Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp861))->v, v)){ return 0;}{ struct Cyc_Absyn_JoinEff_struct*
_temp880=({ struct Cyc_Absyn_JoinEff_struct* _temp874=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp874[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp875; _temp875.tag= Cyc_Absyn_JoinEff; _temp875.f1=({ struct Cyc_List_List*
_temp876=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp876->hd=( void*) _temp872; _temp876->tl=({ struct Cyc_List_List* _temp877=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp877->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp878=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp878[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp879; _temp879.tag= Cyc_Absyn_VarType; _temp879.f1= v; _temp879;}); _temp878;}));
_temp877->tl= 0; _temp877;}); _temp876;}); _temp875;}); _temp874;}); goto _LL881;
_LL881:* _temp867=({ struct Cyc_Core_Opt* _temp882=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp882->v=( void*)(( void*) _temp880);
_temp882;}); return 1;}} _LL856: return 0; _LL848:;}} static int Cyc_Tcutil_evar_in_effect(
void* evar, void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp883= e; struct Cyc_List_List*
_temp891; struct Cyc_Core_Opt* _temp893; int _temp895; struct Cyc_Core_Opt*
_temp897; struct Cyc_Core_Opt* _temp899; _LL885: if(( unsigned int) _temp883 > 4u?*((
int*) _temp883) == Cyc_Absyn_JoinEff: 0){ _LL892: _temp891=(( struct Cyc_Absyn_JoinEff_struct*)
_temp883)->f1; goto _LL886;} else{ goto _LL887;} _LL887: if(( unsigned int)
_temp883 > 4u?*(( int*) _temp883) == Cyc_Absyn_Evar: 0){ _LL900: _temp899=((
struct Cyc_Absyn_Evar_struct*) _temp883)->f1; goto _LL898; _LL898: _temp897=((
struct Cyc_Absyn_Evar_struct*) _temp883)->f2; goto _LL896; _LL896: _temp895=((
struct Cyc_Absyn_Evar_struct*) _temp883)->f3; goto _LL894; _LL894: _temp893=((
struct Cyc_Absyn_Evar_struct*) _temp883)->f4; goto _LL888;} else{ goto _LL889;}
_LL889: goto _LL890; _LL886: for( 0; _temp891 != 0; _temp891=(( struct Cyc_List_List*)
_check_null( _temp891))->tl){ if( Cyc_Tcutil_evar_in_effect( evar,( void*)((
struct Cyc_List_List*) _check_null( _temp891))->hd)){ return 1;}} return 0;
_LL888: return evar == e; _LL890: return 0; _LL884:;}} int Cyc_Tcutil_subset_effect(
int set_to_empty, void* e1, void* e2){{ struct _tuple9* _temp901= Cyc_Tcutil_get_effect_evar(
e2); struct _tuple9 _temp907; struct Cyc_List_List* _temp908; struct Cyc_List_List*
_temp910; void* _temp912; _LL903: if( _temp901 == 0){ goto _LL905;} else{
_temp907=* _temp901; _LL913: _temp912= _temp907.f1; goto _LL911; _LL911:
_temp910= _temp907.f2; goto _LL909; _LL909: _temp908= _temp907.f3; goto _LL904;}
_LL905: goto _LL906; _LL904: { void* _temp915= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,({ struct Cyc_Core_Opt* _temp914=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp914->v=( void*) _temp908; _temp914;})); goto
_LL916; _LL916: { struct Cyc_Absyn_JoinEff_struct* _temp921=({ struct Cyc_Absyn_JoinEff_struct*
_temp917=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp917[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp918; _temp918.tag= Cyc_Absyn_JoinEff;
_temp918.f1=({ struct Cyc_List_List* _temp919=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp919->hd=( void*) _temp915; _temp919->tl=({
struct Cyc_List_List* _temp920=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp920->hd=( void*) e1; _temp920->tl= 0; _temp920;});
_temp919;}); _temp918;}); _temp917;}); goto _LL922; _LL922: return Cyc_Tcutil_unify(
_temp912,( void*) _temp921);}} _LL906: goto _LL902; _LL902:;}{ void* _temp923=
Cyc_Tcutil_compress( e1); struct Cyc_List_List* _temp935; void* _temp937; struct
Cyc_Absyn_Tvar* _temp939; struct Cyc_Core_Opt* _temp941; int _temp943; struct
Cyc_Core_Opt* _temp945; struct Cyc_Core_Opt** _temp947; struct Cyc_Core_Opt*
_temp948; _LL925: if(( unsigned int) _temp923 > 4u?*(( int*) _temp923) == Cyc_Absyn_JoinEff:
0){ _LL936: _temp935=(( struct Cyc_Absyn_JoinEff_struct*) _temp923)->f1; goto
_LL926;} else{ goto _LL927;} _LL927: if(( unsigned int) _temp923 > 4u?*(( int*)
_temp923) == Cyc_Absyn_AccessEff: 0){ _LL938: _temp937=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp923)->f1; goto _LL928;} else{ goto _LL929;} _LL929: if(( unsigned int)
_temp923 > 4u?*(( int*) _temp923) == Cyc_Absyn_VarType: 0){ _LL940: _temp939=((
struct Cyc_Absyn_VarType_struct*) _temp923)->f1; goto _LL930;} else{ goto _LL931;}
_LL931: if(( unsigned int) _temp923 > 4u?*(( int*) _temp923) == Cyc_Absyn_Evar:
0){ _LL949: _temp948=(( struct Cyc_Absyn_Evar_struct*) _temp923)->f1; goto
_LL946; _LL946: _temp945=(( struct Cyc_Absyn_Evar_struct*) _temp923)->f2;
_temp947=&(( struct Cyc_Absyn_Evar_struct*) _temp923)->f2; goto _LL944; _LL944:
_temp943=(( struct Cyc_Absyn_Evar_struct*) _temp923)->f3; goto _LL942; _LL942:
_temp941=(( struct Cyc_Absyn_Evar_struct*) _temp923)->f4; goto _LL932;} else{
goto _LL933;} _LL933: goto _LL934; _LL926: for( 0; _temp935 != 0; _temp935=((
struct Cyc_List_List*) _check_null( _temp935))->tl){ if( ! Cyc_Tcutil_subset_effect(
set_to_empty,( void*)(( struct Cyc_List_List*) _check_null( _temp935))->hd, e2)){
return 0;}} return 1; _LL928: return( Cyc_Tcutil_region_in_effect( 0, _temp937,
e2)? 1: Cyc_Tcutil_region_in_effect( 1, _temp937, e2))? 1: Cyc_Tcutil_unify(
_temp937,( void*) Cyc_Absyn_HeapRgn); _LL930: return Cyc_Tcutil_variable_in_effect(
0, _temp939, e2)? 1: Cyc_Tcutil_variable_in_effect( 1, _temp939, e2); _LL932:
if( ! Cyc_Tcutil_evar_in_effect( e1, e2)){ if( set_to_empty){* _temp947=({
struct Cyc_Core_Opt* _temp950=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp950->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp951=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp951[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp952; _temp952.tag= Cyc_Absyn_JoinEff;
_temp952.f1= 0; _temp952;}); _temp951;})); _temp950;});} else{ Cyc_Tcutil_occurs(
e1,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp941))->v,
e2);* _temp947=({ struct Cyc_Core_Opt* _temp953=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp953->v=( void*) e2; _temp953;});}}
return 1; _LL934: return(( int(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)((
struct _tagged_string)({ struct _tagged_string _temp954= Cyc_Absynpp_typ2string(
e1); xprintf("subset_effect: bad effect: %.*s", _temp954.last_plus_one -
_temp954.curr, _temp954.curr);})); _LL924:;}} struct _tuple10{ struct _tuple9*
f1; struct _tuple9* f2; } ; static int Cyc_Tcutil_unify_effect( void* e1, void*
e2){ e1= Cyc_Tcutil_compress( e1); e2= Cyc_Tcutil_compress( e2); Cyc_Tcutil_normalize_effect(
e1); Cyc_Tcutil_normalize_effect( e2);{ struct _tuple10 _temp956=({ struct
_tuple10 _temp955; _temp955.f1= Cyc_Tcutil_get_effect_evar( e1); _temp955.f2=
Cyc_Tcutil_get_effect_evar( e2); _temp955;}); struct _tuple9* _temp962; struct
_tuple9 _temp964; struct Cyc_List_List* _temp965; struct Cyc_List_List* _temp967;
void* _temp969; struct _tuple9* _temp971; struct _tuple9 _temp973; struct Cyc_List_List*
_temp974; struct Cyc_List_List* _temp976; void* _temp978; _LL958: _LL972:
_temp971= _temp956.f1; if( _temp971 == 0){ goto _LL960;} else{ _temp973=*
_temp971; _LL979: _temp978= _temp973.f1; goto _LL977; _LL977: _temp976= _temp973.f2;
goto _LL975; _LL975: _temp974= _temp973.f3; goto _LL963;} _LL963: _temp962=
_temp956.f2; if( _temp962 == 0){ goto _LL960;} else{ _temp964=* _temp962; _LL970:
_temp969= _temp964.f1; goto _LL968; _LL968: _temp967= _temp964.f2; goto _LL966;
_LL966: _temp965= _temp964.f3; goto _LL959;} _LL960: goto _LL961; _LL959: {
struct Cyc_List_List* _temp980= 0; goto _LL981; _LL981: for( 0; _temp974 != 0;
_temp974=(( struct Cyc_List_List*) _check_null( _temp974))->tl){ if((( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, _temp965,(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp974))->hd)){
_temp980=({ struct Cyc_List_List* _temp982=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp982->hd=( void*)(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp974))->hd); _temp982->tl= _temp980;
_temp982;});}}{ void* _temp984= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,({
struct Cyc_Core_Opt* _temp983=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp983->v=( void*) _temp980; _temp983;})); goto _LL985; _LL985: {
struct Cyc_Absyn_JoinEff_struct* _temp989=({ struct Cyc_Absyn_JoinEff_struct*
_temp986=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp986[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp987; _temp987.tag= Cyc_Absyn_JoinEff;
_temp987.f1=({ struct Cyc_List_List* _temp988=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp988->hd=( void*) _temp984; _temp988->tl=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp976, _temp967); _temp988;}); _temp987;}); _temp986;}); goto _LL990; _LL990:
return Cyc_Tcutil_unify( _temp978,( void*) _temp989)? Cyc_Tcutil_unify( _temp969,(
void*) _temp989): 0;}}} _LL961: return Cyc_Tcutil_subset_effect( 0, e1, e2)? Cyc_Tcutil_subset_effect(
0, e2, e1): 0; _LL957:;}} struct _tuple11{ struct Cyc_Absyn_VarargInfo* f1;
struct Cyc_Absyn_VarargInfo* f2; } ; struct _tuple12{ struct Cyc_Core_Opt* f1;
struct Cyc_Core_Opt* f2; } ; void Cyc_Tcutil_unify_it( void* t1, void* t2){ t1=
Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2); if( t1 == t2){ return;}{
void* _temp991= t1; struct Cyc_Core_Opt* _temp997; int _temp999; struct Cyc_Core_Opt*
_temp1001; struct Cyc_Core_Opt** _temp1003; struct Cyc_Core_Opt* _temp1004;
_LL993: if(( unsigned int) _temp991 > 4u?*(( int*) _temp991) == Cyc_Absyn_Evar:
0){ _LL1005: _temp1004=(( struct Cyc_Absyn_Evar_struct*) _temp991)->f1; goto
_LL1002; _LL1002: _temp1001=(( struct Cyc_Absyn_Evar_struct*) _temp991)->f2;
_temp1003=&(( struct Cyc_Absyn_Evar_struct*) _temp991)->f2; goto _LL1000;
_LL1000: _temp999=(( struct Cyc_Absyn_Evar_struct*) _temp991)->f3; goto _LL998;
_LL998: _temp997=(( struct Cyc_Absyn_Evar_struct*) _temp991)->f4; goto _LL994;}
else{ goto _LL995;} _LL995: goto _LL996; _LL994: Cyc_Tcutil_occurs( t1,( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp997))->v, t2);{ void*
_temp1006= Cyc_Tcutil_typ_kind( t2); goto _LL1007; _LL1007: if( Cyc_Tcutil_kind_leq(
_temp1006,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1004))->v)){*
_temp1003=({ struct Cyc_Core_Opt* _temp1008=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1008->v=( void*) t2; _temp1008;}); return;}
else{{ void* _temp1009= t2; struct Cyc_Core_Opt* _temp1017; int _temp1019;
struct Cyc_Core_Opt* _temp1021; struct Cyc_Core_Opt** _temp1023; struct Cyc_Core_Opt*
_temp1024; struct Cyc_Absyn_PtrInfo _temp1027; _LL1011: if(( unsigned int)
_temp1009 > 4u?*(( int*) _temp1009) == Cyc_Absyn_Evar: 0){ _LL1025: _temp1024=((
struct Cyc_Absyn_Evar_struct*) _temp1009)->f1; goto _LL1022; _LL1022: _temp1021=((
struct Cyc_Absyn_Evar_struct*) _temp1009)->f2; _temp1023=&(( struct Cyc_Absyn_Evar_struct*)
_temp1009)->f2; goto _LL1020; _LL1020: _temp1019=(( struct Cyc_Absyn_Evar_struct*)
_temp1009)->f3; goto _LL1018; _LL1018: _temp1017=(( struct Cyc_Absyn_Evar_struct*)
_temp1009)->f4; goto _LL1012;} else{ goto _LL1013;} _LL1013: if(( unsigned int)
_temp1009 > 4u?*(( int*) _temp1009) == Cyc_Absyn_PointerType: 0){ _LL1028:
_temp1027=(( struct Cyc_Absyn_PointerType_struct*) _temp1009)->f1; goto _LL1026;}
else{ goto _LL1015;} _LL1026: if(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1004))->v ==( void*) Cyc_Absyn_BoxKind){ goto _LL1014;} else{ goto _LL1015;}
_LL1015: goto _LL1016; _LL1012: { struct Cyc_List_List* _temp1029=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp997))->v; goto _LL1030; _LL1030:{ struct
Cyc_List_List* s2=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp1017))->v; for( 0; s2 != 0; s2=(( struct Cyc_List_List*) _check_null( s2))->tl){
if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,
_temp1029,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( s2))->hd)){(
void) _throw(( void*) Cyc_Tcutil_Unify);}}} if( Cyc_Tcutil_kind_leq(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1004))->v, _temp1006)){* _temp1023=({
struct Cyc_Core_Opt* _temp1031=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1031->v=( void*) t1; _temp1031;}); return;} goto _LL1010;}
_LL1014: { struct Cyc_Absyn_Conref* _temp1032=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1027.bounds); goto
_LL1033; _LL1033:{ void* _temp1034=( void*) _temp1032->v; _LL1036: if( _temp1034
==( void*) Cyc_Absyn_No_constr){ goto _LL1037;} else{ goto _LL1038;} _LL1038:
goto _LL1039; _LL1037:( void*)( _temp1032->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp1040=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp1040[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp1041; _temp1041.tag= Cyc_Absyn_Eq_constr;
_temp1041.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1042=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1042[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1043; _temp1043.tag= Cyc_Absyn_Upper_b;
_temp1043.f1= Cyc_Absyn_signed_int_exp( 1, 0); _temp1043;}); _temp1042;}));
_temp1041;}); _temp1040;})));* _temp1003=({ struct Cyc_Core_Opt* _temp1044=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1044->v=(
void*) t2; _temp1044;}); return; _LL1039: goto _LL1035; _LL1035:;} goto _LL1010;}
_LL1016: goto _LL1010; _LL1010:;}( void) _throw(( void*) Cyc_Tcutil_Unify);}}
_LL996: goto _LL992; _LL992:;}{ struct _tuple8 _temp1046=({ struct _tuple8
_temp1045; _temp1045.f1= t2; _temp1045.f2= t1; _temp1045;}); void* _temp1096;
void* _temp1098; struct Cyc_Core_Opt* _temp1100; int _temp1102; struct Cyc_Core_Opt*
_temp1104; struct Cyc_Core_Opt* _temp1106; void* _temp1108; void* _temp1110;
void* _temp1112; struct Cyc_Absyn_Tvar* _temp1114; void* _temp1116; struct Cyc_Absyn_Tvar*
_temp1118; void* _temp1120; struct Cyc_Absyn_Structdecl** _temp1122; struct Cyc_List_List*
_temp1124; struct _tuple1* _temp1126; void* _temp1128; struct Cyc_Absyn_Structdecl**
_temp1130; struct Cyc_List_List* _temp1132; struct _tuple1* _temp1134; void*
_temp1136; struct Cyc_Absyn_Enumdecl* _temp1138; struct _tuple1* _temp1140; void*
_temp1142; struct Cyc_Absyn_Enumdecl* _temp1144; struct _tuple1* _temp1146; void*
_temp1148; struct Cyc_Absyn_Uniondecl** _temp1150; struct Cyc_List_List*
_temp1152; struct _tuple1* _temp1154; void* _temp1156; struct Cyc_Absyn_Uniondecl**
_temp1158; struct Cyc_List_List* _temp1160; struct _tuple1* _temp1162; void*
_temp1164; struct Cyc_Absyn_TunionInfo _temp1166; void* _temp1168; struct Cyc_List_List*
_temp1170; void* _temp1172; struct Cyc_Absyn_Tuniondecl* _temp1174; void*
_temp1176; struct Cyc_Absyn_TunionInfo _temp1178; void* _temp1180; struct Cyc_List_List*
_temp1182; void* _temp1184; struct Cyc_Absyn_Tuniondecl* _temp1186; void*
_temp1188; struct Cyc_Absyn_TunionFieldInfo _temp1190; struct Cyc_List_List*
_temp1192; void* _temp1194; struct Cyc_Absyn_Tunionfield* _temp1196; struct Cyc_Absyn_Tuniondecl*
_temp1198; void* _temp1200; struct Cyc_Absyn_TunionFieldInfo _temp1202; struct
Cyc_List_List* _temp1204; void* _temp1206; struct Cyc_Absyn_Tunionfield*
_temp1208; struct Cyc_Absyn_Tuniondecl* _temp1210; void* _temp1212; struct Cyc_Absyn_PtrInfo
_temp1214; struct Cyc_Absyn_Conref* _temp1216; struct Cyc_Absyn_Tqual _temp1218;
struct Cyc_Absyn_Conref* _temp1220; void* _temp1222; void* _temp1224; void*
_temp1226; struct Cyc_Absyn_PtrInfo _temp1228; struct Cyc_Absyn_Conref*
_temp1230; struct Cyc_Absyn_Tqual _temp1232; struct Cyc_Absyn_Conref* _temp1234;
void* _temp1236; void* _temp1238; void* _temp1240; void* _temp1242; void*
_temp1244; void* _temp1246; void* _temp1248; void* _temp1250; void* _temp1252;
void* _temp1254; void* _temp1256; void* _temp1258; void* _temp1260; struct Cyc_Absyn_Exp*
_temp1262; struct Cyc_Absyn_Tqual _temp1264; void* _temp1266; void* _temp1268;
struct Cyc_Absyn_Exp* _temp1270; struct Cyc_Absyn_Tqual _temp1272; void*
_temp1274; void* _temp1276; struct Cyc_Absyn_FnInfo _temp1278; struct Cyc_List_List*
_temp1280; struct Cyc_Absyn_VarargInfo* _temp1282; int _temp1284; struct Cyc_List_List*
_temp1286; void* _temp1288; struct Cyc_Core_Opt* _temp1290; struct Cyc_List_List*
_temp1292; void* _temp1294; struct Cyc_Absyn_FnInfo _temp1296; struct Cyc_List_List*
_temp1298; struct Cyc_Absyn_VarargInfo* _temp1300; int _temp1302; struct Cyc_List_List*
_temp1304; void* _temp1306; struct Cyc_Core_Opt* _temp1308; struct Cyc_List_List*
_temp1310; void* _temp1312; struct Cyc_List_List* _temp1314; void* _temp1316;
struct Cyc_List_List* _temp1318; void* _temp1320; struct Cyc_List_List*
_temp1322; void* _temp1324; struct Cyc_List_List* _temp1326; void* _temp1328;
struct Cyc_List_List* _temp1330; void* _temp1332; struct Cyc_List_List*
_temp1334; void* _temp1336; void* _temp1338; void* _temp1340; void* _temp1342;
void* _temp1344; void* _temp1346; void* _temp1348; void* _temp1350; struct Cyc_List_List*
_temp1352; void* _temp1354; struct Cyc_List_List* _temp1356; void* _temp1358;
void* _temp1360; void* _temp1362; void* _temp1364; void* _temp1366; void*
_temp1368; void* _temp1370; _LL1048: _LL1099: _temp1098= _temp1046.f1; if((
unsigned int) _temp1098 > 4u?*(( int*) _temp1098) == Cyc_Absyn_Evar: 0){ _LL1107:
_temp1106=(( struct Cyc_Absyn_Evar_struct*) _temp1098)->f1; goto _LL1105;
_LL1105: _temp1104=(( struct Cyc_Absyn_Evar_struct*) _temp1098)->f2; goto
_LL1103; _LL1103: _temp1102=(( struct Cyc_Absyn_Evar_struct*) _temp1098)->f3;
goto _LL1101; _LL1101: _temp1100=(( struct Cyc_Absyn_Evar_struct*) _temp1098)->f4;
goto _LL1097;} else{ goto _LL1050;} _LL1097: _temp1096= _temp1046.f2; goto
_LL1049; _LL1050: _LL1111: _temp1110= _temp1046.f1; if( _temp1110 ==( void*) Cyc_Absyn_VoidType){
goto _LL1109;} else{ goto _LL1052;} _LL1109: _temp1108= _temp1046.f2; if(
_temp1108 ==( void*) Cyc_Absyn_VoidType){ goto _LL1051;} else{ goto _LL1052;}
_LL1052: _LL1117: _temp1116= _temp1046.f1; if(( unsigned int) _temp1116 > 4u?*((
int*) _temp1116) == Cyc_Absyn_VarType: 0){ _LL1119: _temp1118=(( struct Cyc_Absyn_VarType_struct*)
_temp1116)->f1; goto _LL1113;} else{ goto _LL1054;} _LL1113: _temp1112=
_temp1046.f2; if(( unsigned int) _temp1112 > 4u?*(( int*) _temp1112) == Cyc_Absyn_VarType:
0){ _LL1115: _temp1114=(( struct Cyc_Absyn_VarType_struct*) _temp1112)->f1; goto
_LL1053;} else{ goto _LL1054;} _LL1054: _LL1129: _temp1128= _temp1046.f1; if((
unsigned int) _temp1128 > 4u?*(( int*) _temp1128) == Cyc_Absyn_StructType: 0){
_LL1135: _temp1134=(( struct Cyc_Absyn_StructType_struct*) _temp1128)->f1; goto
_LL1133; _LL1133: _temp1132=(( struct Cyc_Absyn_StructType_struct*) _temp1128)->f2;
goto _LL1131; _LL1131: _temp1130=(( struct Cyc_Absyn_StructType_struct*)
_temp1128)->f3; goto _LL1121;} else{ goto _LL1056;} _LL1121: _temp1120=
_temp1046.f2; if(( unsigned int) _temp1120 > 4u?*(( int*) _temp1120) == Cyc_Absyn_StructType:
0){ _LL1127: _temp1126=(( struct Cyc_Absyn_StructType_struct*) _temp1120)->f1;
goto _LL1125; _LL1125: _temp1124=(( struct Cyc_Absyn_StructType_struct*)
_temp1120)->f2; goto _LL1123; _LL1123: _temp1122=(( struct Cyc_Absyn_StructType_struct*)
_temp1120)->f3; goto _LL1055;} else{ goto _LL1056;} _LL1056: _LL1143: _temp1142=
_temp1046.f1; if(( unsigned int) _temp1142 > 4u?*(( int*) _temp1142) == Cyc_Absyn_EnumType:
0){ _LL1147: _temp1146=(( struct Cyc_Absyn_EnumType_struct*) _temp1142)->f1;
goto _LL1145; _LL1145: _temp1144=(( struct Cyc_Absyn_EnumType_struct*) _temp1142)->f2;
goto _LL1137;} else{ goto _LL1058;} _LL1137: _temp1136= _temp1046.f2; if((
unsigned int) _temp1136 > 4u?*(( int*) _temp1136) == Cyc_Absyn_EnumType: 0){
_LL1141: _temp1140=(( struct Cyc_Absyn_EnumType_struct*) _temp1136)->f1; goto
_LL1139; _LL1139: _temp1138=(( struct Cyc_Absyn_EnumType_struct*) _temp1136)->f2;
goto _LL1057;} else{ goto _LL1058;} _LL1058: _LL1157: _temp1156= _temp1046.f1;
if(( unsigned int) _temp1156 > 4u?*(( int*) _temp1156) == Cyc_Absyn_UnionType: 0){
_LL1163: _temp1162=(( struct Cyc_Absyn_UnionType_struct*) _temp1156)->f1; goto
_LL1161; _LL1161: _temp1160=(( struct Cyc_Absyn_UnionType_struct*) _temp1156)->f2;
goto _LL1159; _LL1159: _temp1158=(( struct Cyc_Absyn_UnionType_struct*)
_temp1156)->f3; goto _LL1149;} else{ goto _LL1060;} _LL1149: _temp1148=
_temp1046.f2; if(( unsigned int) _temp1148 > 4u?*(( int*) _temp1148) == Cyc_Absyn_UnionType:
0){ _LL1155: _temp1154=(( struct Cyc_Absyn_UnionType_struct*) _temp1148)->f1;
goto _LL1153; _LL1153: _temp1152=(( struct Cyc_Absyn_UnionType_struct*)
_temp1148)->f2; goto _LL1151; _LL1151: _temp1150=(( struct Cyc_Absyn_UnionType_struct*)
_temp1148)->f3; goto _LL1059;} else{ goto _LL1060;} _LL1060: _LL1177: _temp1176=
_temp1046.f1; if(( unsigned int) _temp1176 > 4u?*(( int*) _temp1176) == Cyc_Absyn_TunionType:
0){ _LL1179: _temp1178=(( struct Cyc_Absyn_TunionType_struct*) _temp1176)->f1;
_LL1185: _temp1184=( void*) _temp1178.tunion_info; if(*(( int*) _temp1184) ==
Cyc_Absyn_KnownTunion){ _LL1187: _temp1186=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1184)->f1; goto _LL1183;} else{ goto _LL1062;} _LL1183: _temp1182=
_temp1178.targs; goto _LL1181; _LL1181: _temp1180=( void*) _temp1178.rgn; goto
_LL1165;} else{ goto _LL1062;} _LL1165: _temp1164= _temp1046.f2; if((
unsigned int) _temp1164 > 4u?*(( int*) _temp1164) == Cyc_Absyn_TunionType: 0){
_LL1167: _temp1166=(( struct Cyc_Absyn_TunionType_struct*) _temp1164)->f1;
_LL1173: _temp1172=( void*) _temp1166.tunion_info; if(*(( int*) _temp1172) ==
Cyc_Absyn_KnownTunion){ _LL1175: _temp1174=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1172)->f1; goto _LL1171;} else{ goto _LL1062;} _LL1171: _temp1170=
_temp1166.targs; goto _LL1169; _LL1169: _temp1168=( void*) _temp1166.rgn; goto
_LL1061;} else{ goto _LL1062;} _LL1062: _LL1201: _temp1200= _temp1046.f1; if((
unsigned int) _temp1200 > 4u?*(( int*) _temp1200) == Cyc_Absyn_TunionFieldType:
0){ _LL1203: _temp1202=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1200)->f1;
_LL1207: _temp1206=( void*) _temp1202.field_info; if(*(( int*) _temp1206) == Cyc_Absyn_KnownTunionfield){
_LL1211: _temp1210=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1206)->f1;
goto _LL1209; _LL1209: _temp1208=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1206)->f2; goto _LL1205;} else{ goto _LL1064;} _LL1205: _temp1204=
_temp1202.targs; goto _LL1189;} else{ goto _LL1064;} _LL1189: _temp1188=
_temp1046.f2; if(( unsigned int) _temp1188 > 4u?*(( int*) _temp1188) == Cyc_Absyn_TunionFieldType:
0){ _LL1191: _temp1190=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1188)->f1;
_LL1195: _temp1194=( void*) _temp1190.field_info; if(*(( int*) _temp1194) == Cyc_Absyn_KnownTunionfield){
_LL1199: _temp1198=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1194)->f1;
goto _LL1197; _LL1197: _temp1196=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1194)->f2; goto _LL1193;} else{ goto _LL1064;} _LL1193: _temp1192=
_temp1190.targs; goto _LL1063;} else{ goto _LL1064;} _LL1064: _LL1227: _temp1226=
_temp1046.f1; if(( unsigned int) _temp1226 > 4u?*(( int*) _temp1226) == Cyc_Absyn_PointerType:
0){ _LL1229: _temp1228=(( struct Cyc_Absyn_PointerType_struct*) _temp1226)->f1;
_LL1239: _temp1238=( void*) _temp1228.elt_typ; goto _LL1237; _LL1237: _temp1236=(
void*) _temp1228.rgn_typ; goto _LL1235; _LL1235: _temp1234= _temp1228.nullable;
goto _LL1233; _LL1233: _temp1232= _temp1228.tq; goto _LL1231; _LL1231: _temp1230=
_temp1228.bounds; goto _LL1213;} else{ goto _LL1066;} _LL1213: _temp1212=
_temp1046.f2; if(( unsigned int) _temp1212 > 4u?*(( int*) _temp1212) == Cyc_Absyn_PointerType:
0){ _LL1215: _temp1214=(( struct Cyc_Absyn_PointerType_struct*) _temp1212)->f1;
_LL1225: _temp1224=( void*) _temp1214.elt_typ; goto _LL1223; _LL1223: _temp1222=(
void*) _temp1214.rgn_typ; goto _LL1221; _LL1221: _temp1220= _temp1214.nullable;
goto _LL1219; _LL1219: _temp1218= _temp1214.tq; goto _LL1217; _LL1217: _temp1216=
_temp1214.bounds; goto _LL1065;} else{ goto _LL1066;} _LL1066: _LL1247:
_temp1246= _temp1046.f1; if(( unsigned int) _temp1246 > 4u?*(( int*) _temp1246)
== Cyc_Absyn_IntType: 0){ _LL1251: _temp1250=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1246)->f1; goto _LL1249; _LL1249: _temp1248=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1246)->f2; goto _LL1241;} else{ goto _LL1068;} _LL1241: _temp1240=
_temp1046.f2; if(( unsigned int) _temp1240 > 4u?*(( int*) _temp1240) == Cyc_Absyn_IntType:
0){ _LL1245: _temp1244=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1240)->f1;
goto _LL1243; _LL1243: _temp1242=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1240)->f2; goto _LL1067;} else{ goto _LL1068;} _LL1068: _LL1255: _temp1254=
_temp1046.f1; if( _temp1254 ==( void*) Cyc_Absyn_FloatType){ goto _LL1253;}
else{ goto _LL1070;} _LL1253: _temp1252= _temp1046.f2; if( _temp1252 ==( void*)
Cyc_Absyn_FloatType){ goto _LL1069;} else{ goto _LL1070;} _LL1070: _LL1259:
_temp1258= _temp1046.f1; if( _temp1258 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL1257;} else{ goto _LL1072;} _LL1257: _temp1256= _temp1046.f2; if( _temp1256
==( void*) Cyc_Absyn_DoubleType){ goto _LL1071;} else{ goto _LL1072;} _LL1072:
_LL1269: _temp1268= _temp1046.f1; if(( unsigned int) _temp1268 > 4u?*(( int*)
_temp1268) == Cyc_Absyn_ArrayType: 0){ _LL1275: _temp1274=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1268)->f1; goto _LL1273; _LL1273: _temp1272=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1268)->f2; goto _LL1271; _LL1271: _temp1270=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1268)->f3; goto _LL1261;} else{ goto _LL1074;} _LL1261: _temp1260=
_temp1046.f2; if(( unsigned int) _temp1260 > 4u?*(( int*) _temp1260) == Cyc_Absyn_ArrayType:
0){ _LL1267: _temp1266=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1260)->f1;
goto _LL1265; _LL1265: _temp1264=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1260)->f2; goto _LL1263; _LL1263: _temp1262=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1260)->f3; goto _LL1073;} else{ goto _LL1074;} _LL1074: _LL1295: _temp1294=
_temp1046.f1; if(( unsigned int) _temp1294 > 4u?*(( int*) _temp1294) == Cyc_Absyn_FnType:
0){ _LL1297: _temp1296=(( struct Cyc_Absyn_FnType_struct*) _temp1294)->f1;
_LL1311: _temp1310= _temp1296.tvars; goto _LL1309; _LL1309: _temp1308= _temp1296.effect;
goto _LL1307; _LL1307: _temp1306=( void*) _temp1296.ret_typ; goto _LL1305;
_LL1305: _temp1304= _temp1296.args; goto _LL1303; _LL1303: _temp1302= _temp1296.c_varargs;
goto _LL1301; _LL1301: _temp1300= _temp1296.cyc_varargs; goto _LL1299; _LL1299:
_temp1298= _temp1296.attributes; goto _LL1277;} else{ goto _LL1076;} _LL1277:
_temp1276= _temp1046.f2; if(( unsigned int) _temp1276 > 4u?*(( int*) _temp1276)
== Cyc_Absyn_FnType: 0){ _LL1279: _temp1278=(( struct Cyc_Absyn_FnType_struct*)
_temp1276)->f1; _LL1293: _temp1292= _temp1278.tvars; goto _LL1291; _LL1291:
_temp1290= _temp1278.effect; goto _LL1289; _LL1289: _temp1288=( void*) _temp1278.ret_typ;
goto _LL1287; _LL1287: _temp1286= _temp1278.args; goto _LL1285; _LL1285:
_temp1284= _temp1278.c_varargs; goto _LL1283; _LL1283: _temp1282= _temp1278.cyc_varargs;
goto _LL1281; _LL1281: _temp1280= _temp1278.attributes; goto _LL1075;} else{
goto _LL1076;} _LL1076: _LL1317: _temp1316= _temp1046.f1; if(( unsigned int)
_temp1316 > 4u?*(( int*) _temp1316) == Cyc_Absyn_TupleType: 0){ _LL1319:
_temp1318=(( struct Cyc_Absyn_TupleType_struct*) _temp1316)->f1; goto _LL1313;}
else{ goto _LL1078;} _LL1313: _temp1312= _temp1046.f2; if(( unsigned int)
_temp1312 > 4u?*(( int*) _temp1312) == Cyc_Absyn_TupleType: 0){ _LL1315:
_temp1314=(( struct Cyc_Absyn_TupleType_struct*) _temp1312)->f1; goto _LL1077;}
else{ goto _LL1078;} _LL1078: _LL1325: _temp1324= _temp1046.f1; if((
unsigned int) _temp1324 > 4u?*(( int*) _temp1324) == Cyc_Absyn_AnonStructType: 0){
_LL1327: _temp1326=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1324)->f1;
goto _LL1321;} else{ goto _LL1080;} _LL1321: _temp1320= _temp1046.f2; if((
unsigned int) _temp1320 > 4u?*(( int*) _temp1320) == Cyc_Absyn_AnonStructType: 0){
_LL1323: _temp1322=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1320)->f1;
goto _LL1079;} else{ goto _LL1080;} _LL1080: _LL1333: _temp1332= _temp1046.f1;
if(( unsigned int) _temp1332 > 4u?*(( int*) _temp1332) == Cyc_Absyn_AnonUnionType:
0){ _LL1335: _temp1334=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1332)->f1;
goto _LL1329;} else{ goto _LL1082;} _LL1329: _temp1328= _temp1046.f2; if((
unsigned int) _temp1328 > 4u?*(( int*) _temp1328) == Cyc_Absyn_AnonUnionType: 0){
_LL1331: _temp1330=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1328)->f1;
goto _LL1081;} else{ goto _LL1082;} _LL1082: _LL1339: _temp1338= _temp1046.f1;
if( _temp1338 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1337;} else{ goto _LL1084;}
_LL1337: _temp1336= _temp1046.f2; if( _temp1336 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL1083;} else{ goto _LL1084;} _LL1084: _LL1345: _temp1344= _temp1046.f1;
if(( unsigned int) _temp1344 > 4u?*(( int*) _temp1344) == Cyc_Absyn_RgnHandleType:
0){ _LL1347: _temp1346=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1344)->f1; goto _LL1341;} else{ goto _LL1086;} _LL1341: _temp1340=
_temp1046.f2; if(( unsigned int) _temp1340 > 4u?*(( int*) _temp1340) == Cyc_Absyn_RgnHandleType:
0){ _LL1343: _temp1342=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1340)->f1; goto _LL1085;} else{ goto _LL1086;} _LL1086: _LL1351: _temp1350=
_temp1046.f1; if(( unsigned int) _temp1350 > 4u?*(( int*) _temp1350) == Cyc_Absyn_JoinEff:
0){ _LL1353: _temp1352=(( struct Cyc_Absyn_JoinEff_struct*) _temp1350)->f1; goto
_LL1349;} else{ goto _LL1088;} _LL1349: _temp1348= _temp1046.f2; goto _LL1087;
_LL1088: _LL1359: _temp1358= _temp1046.f1; goto _LL1355; _LL1355: _temp1354=
_temp1046.f2; if(( unsigned int) _temp1354 > 4u?*(( int*) _temp1354) == Cyc_Absyn_JoinEff:
0){ _LL1357: _temp1356=(( struct Cyc_Absyn_JoinEff_struct*) _temp1354)->f1; goto
_LL1089;} else{ goto _LL1090;} _LL1090: _LL1363: _temp1362= _temp1046.f1; if((
unsigned int) _temp1362 > 4u?*(( int*) _temp1362) == Cyc_Absyn_AccessEff: 0){
_LL1365: _temp1364=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp1362)->f1;
goto _LL1361;} else{ goto _LL1092;} _LL1361: _temp1360= _temp1046.f2; goto
_LL1091; _LL1092: _LL1371: _temp1370= _temp1046.f1; goto _LL1367; _LL1367:
_temp1366= _temp1046.f2; if(( unsigned int) _temp1366 > 4u?*(( int*) _temp1366)
== Cyc_Absyn_AccessEff: 0){ _LL1369: _temp1368=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp1366)->f1; goto _LL1093;} else{ goto _LL1094;} _LL1094: goto _LL1095;
_LL1049: Cyc_Tcutil_unify_it( t2, t1); return; _LL1051: return; _LL1053: {
struct _tagged_string* _temp1372= _temp1118->name; goto _LL1373; _LL1373: {
struct _tagged_string* _temp1374= _temp1114->name; goto _LL1375; _LL1375: { int
_temp1376=*(( int*) _check_null( _temp1118->identity)); goto _LL1377; _LL1377: {
int _temp1378=*(( int*) _check_null( _temp1114->identity)); goto _LL1379;
_LL1379: { void* _temp1380=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1118->kind); goto _LL1381; _LL1381: { void* _temp1382=(( void*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1114->kind); goto _LL1383;
_LL1383: if( _temp1378 == _temp1376? Cyc_String_zstrptrcmp( _temp1372, _temp1374)
== 0: 0){ if( _temp1380 != _temp1382){(( void(*)( struct _tagged_string msg))
Cyc_Tcutil_impos)(( struct _tagged_string)({ struct _tagged_string _temp1384=*
_temp1372; struct _tagged_string _temp1385= Cyc_Absynpp_kind2string( _temp1380);
struct _tagged_string _temp1386= Cyc_Absynpp_kind2string( _temp1382); xprintf("same type variable %.*s has kinds %.*s and %.*s",
_temp1384.last_plus_one - _temp1384.curr, _temp1384.curr, _temp1385.last_plus_one
- _temp1385.curr, _temp1385.curr, _temp1386.last_plus_one - _temp1386.curr,
_temp1386.curr);}));} return;} goto _LL1047;}}}}}} _LL1055: if((( _temp1126 != 0?
_temp1134 != 0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*) _check_null( _temp1126),(
struct _tuple1*) _check_null( _temp1134)) == 0: 0)? 1:( _temp1126 == 0?
_temp1134 == 0: 0)){ Cyc_Tcutil_unify_list( _temp1124, _temp1132); return;} goto
_LL1047; _LL1057: if( Cyc_Absyn_qvar_cmp( _temp1146, _temp1140) == 0){ return;}
goto _LL1047; _LL1059: if((( _temp1154 != 0? _temp1162 != 0: 0)? Cyc_Absyn_qvar_cmp((
struct _tuple1*) _check_null( _temp1154),( struct _tuple1*) _check_null(
_temp1162)) == 0: 0)? 1:( _temp1154 == 0? _temp1162 == 0: 0)){ Cyc_Tcutil_unify_list(
_temp1152, _temp1160); return;} goto _LL1047; _LL1061: if( _temp1186 ==
_temp1174? 1: Cyc_Absyn_qvar_cmp( _temp1186->name, _temp1174->name) == 0){ Cyc_Tcutil_unify_it(
_temp1168, _temp1180); Cyc_Tcutil_unify_list( _temp1170, _temp1182); return;}
goto _LL1047; _LL1063: if(( _temp1210 == _temp1198? 1: Cyc_Absyn_qvar_cmp(
_temp1210->name, _temp1198->name) == 0)? _temp1208 == _temp1196? 1: Cyc_Absyn_qvar_cmp(
_temp1208->name, _temp1196->name) == 0: 0){ Cyc_Tcutil_unify_list( _temp1192,
_temp1204); return;} goto _LL1047; _LL1065: Cyc_Tcutil_unify_it( _temp1224,
_temp1238); Cyc_Tcutil_unify_it( _temp1236, _temp1222); Cyc_Tcutil_unify_tqual(
_temp1218, _temp1232);(( void(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)( Cyc_Tcutil_boundscmp,
_temp1216, _temp1230);{ void* _temp1387=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1216))->v; void*
_temp1393; _LL1389: if(( unsigned int) _temp1387 > 1u?*(( int*) _temp1387) ==
Cyc_Absyn_Eq_constr: 0){ _LL1394: _temp1393=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1387)->f1; if( _temp1393 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1390;}
else{ goto _LL1391;}} else{ goto _LL1391;} _LL1391: goto _LL1392; _LL1390:
return; _LL1392: goto _LL1388; _LL1388:;}(( void(*)( int(* cmp)( int, int),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp, _temp1220, _temp1234); return; _LL1067: if( _temp1244 ==
_temp1250? _temp1242 == _temp1248: 0){ return;} goto _LL1047; _LL1069: return;
_LL1071: return; _LL1073: Cyc_Tcutil_unify_it( _temp1266, _temp1274); Cyc_Tcutil_unify_tqual(
_temp1264, _temp1272); if( _temp1270 == _temp1262){ return;} if( _temp1270 == 0?
1: _temp1262 == 0){ goto _LL1047;} if( Cyc_Evexp_eval_const_uint_exp(( struct
Cyc_Absyn_Exp*) _check_null( _temp1270)) == Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1262))){ return;} goto _LL1047; _LL1075: {
int done= 0;{ struct _RegionHandle _temp1395= _new_region(); struct
_RegionHandle* rgn=& _temp1395; _push_region( rgn);{ struct Cyc_List_List* inst=
0; while( _temp1292 != 0) { if( _temp1310 == 0){ break;} inst=({ struct Cyc_List_List*
_temp1396=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1396->hd=( void*)({ struct _tuple5* _temp1397=( struct _tuple5*)
_region_malloc( rgn, sizeof( struct _tuple5)); _temp1397->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp1310))->hd; _temp1397->f2=( void*)({
struct Cyc_Absyn_VarType_struct* _temp1398=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp1398[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp1399; _temp1399.tag= Cyc_Absyn_VarType; _temp1399.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp1292))->hd;
_temp1399;}); _temp1398;}); _temp1397;}); _temp1396->tl= inst; _temp1396;});
_temp1292=(( struct Cyc_List_List*) _check_null( _temp1292))->tl; _temp1310=((
struct Cyc_List_List*) _check_null( _temp1310))->tl;} if( _temp1310 != 0){
_npop_handler( 0u); goto _LL1047;} if( inst != 0){ Cyc_Tcutil_unify_it(( void*)({
struct Cyc_Absyn_FnType_struct* _temp1400=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1400[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1401; _temp1401.tag= Cyc_Absyn_FnType; _temp1401.f1=({ struct Cyc_Absyn_FnInfo
_temp1402; _temp1402.tvars= 0; _temp1402.effect= _temp1290; _temp1402.ret_typ=(
void*) _temp1288; _temp1402.args= _temp1286; _temp1402.c_varargs= _temp1284;
_temp1402.cyc_varargs= _temp1282; _temp1402.attributes= _temp1280; _temp1402;});
_temp1401;}); _temp1400;}), Cyc_Tcutil_rsubstitute( rgn, inst,( void*)({ struct
Cyc_Absyn_FnType_struct* _temp1403=( struct Cyc_Absyn_FnType_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_FnType_struct)); _temp1403[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1404; _temp1404.tag= Cyc_Absyn_FnType; _temp1404.f1=({ struct Cyc_Absyn_FnInfo
_temp1405; _temp1405.tvars= 0; _temp1405.effect= _temp1308; _temp1405.ret_typ=(
void*) _temp1306; _temp1405.args= _temp1304; _temp1405.c_varargs= _temp1302;
_temp1405.cyc_varargs= _temp1300; _temp1405.attributes= _temp1298; _temp1405;});
_temp1404;}); _temp1403;}))); done= 1;}}; _pop_region(& _temp1395);} if( done){
return;} Cyc_Tcutil_unify_it( _temp1288, _temp1306); for( 0; _temp1286 != 0?
_temp1304 != 0: 0;( _temp1286=(( struct Cyc_List_List*) _check_null( _temp1286))->tl,
_temp1304=(( struct Cyc_List_List*) _check_null( _temp1304))->tl)){ Cyc_Tcutil_unify_tqual((*((
struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp1286))->hd)).f2,(*((
struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp1304))->hd)).f2);
Cyc_Tcutil_unify_it((*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null(
_temp1286))->hd)).f3,(*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null(
_temp1304))->hd)).f3);} if( _temp1286 != 0? 1: _temp1304 != 0){ goto _LL1047;}
if( _temp1284 != _temp1302){ goto _LL1047;}{ int bad_cyc_vararg= 0;{ struct
_tuple11 _temp1407=({ struct _tuple11 _temp1406; _temp1406.f1= _temp1282;
_temp1406.f2= _temp1300; _temp1406;}); struct Cyc_Absyn_VarargInfo* _temp1417;
struct Cyc_Absyn_VarargInfo* _temp1419; struct Cyc_Absyn_VarargInfo* _temp1421;
struct Cyc_Absyn_VarargInfo* _temp1423; struct Cyc_Absyn_VarargInfo* _temp1425;
struct Cyc_Absyn_VarargInfo* _temp1427; struct Cyc_Absyn_VarargInfo* _temp1429;
struct Cyc_Absyn_VarargInfo _temp1431; int _temp1432; void* _temp1434; void*
_temp1436; struct Cyc_Absyn_Tqual _temp1438; struct Cyc_Core_Opt* _temp1440;
struct Cyc_Absyn_VarargInfo* _temp1442; struct Cyc_Absyn_VarargInfo _temp1444;
int _temp1445; void* _temp1447; void* _temp1449; struct Cyc_Absyn_Tqual
_temp1451; struct Cyc_Core_Opt* _temp1453; _LL1409: _LL1420: _temp1419=
_temp1407.f1; if( _temp1419 == 0){ goto _LL1418;} else{ goto _LL1411;} _LL1418:
_temp1417= _temp1407.f2; if( _temp1417 == 0){ goto _LL1410;} else{ goto _LL1411;}
_LL1411: _LL1424: _temp1423= _temp1407.f1; if( _temp1423 == 0){ goto _LL1422;}
else{ goto _LL1413;} _LL1422: _temp1421= _temp1407.f2; goto _LL1412; _LL1413:
_LL1428: _temp1427= _temp1407.f1; goto _LL1426; _LL1426: _temp1425= _temp1407.f2;
if( _temp1425 == 0){ goto _LL1414;} else{ goto _LL1415;} _LL1415: _LL1443:
_temp1442= _temp1407.f1; if( _temp1442 == 0){ goto _LL1408;} else{ _temp1444=*
_temp1442; _LL1454: _temp1453= _temp1444.name; goto _LL1452; _LL1452: _temp1451=
_temp1444.tq; goto _LL1450; _LL1450: _temp1449=( void*) _temp1444.type; goto
_LL1448; _LL1448: _temp1447=( void*) _temp1444.rgn; goto _LL1446; _LL1446:
_temp1445= _temp1444.inject; goto _LL1430;} _LL1430: _temp1429= _temp1407.f2;
if( _temp1429 == 0){ goto _LL1408;} else{ _temp1431=* _temp1429; _LL1441:
_temp1440= _temp1431.name; goto _LL1439; _LL1439: _temp1438= _temp1431.tq; goto
_LL1437; _LL1437: _temp1436=( void*) _temp1431.type; goto _LL1435; _LL1435:
_temp1434=( void*) _temp1431.rgn; goto _LL1433; _LL1433: _temp1432= _temp1431.inject;
goto _LL1416;} _LL1410: goto _LL1408; _LL1412: goto _LL1414; _LL1414:
bad_cyc_vararg= 1; goto _LL1408; _LL1416: Cyc_Tcutil_unify_tqual( _temp1451,
_temp1438); Cyc_Tcutil_unify_it( _temp1449, _temp1436); Cyc_Tcutil_unify_it(
_temp1447, _temp1434); if( _temp1445 != _temp1432){ bad_cyc_vararg= 1;} goto
_LL1408; _LL1408:;} if( bad_cyc_vararg){ goto _LL1047;}{ int bad_effect= 0;{
struct _tuple12 _temp1456=({ struct _tuple12 _temp1455; _temp1455.f1= _temp1290;
_temp1455.f2= _temp1308; _temp1455;}); struct Cyc_Core_Opt* _temp1466; struct
Cyc_Core_Opt* _temp1468; struct Cyc_Core_Opt* _temp1470; struct Cyc_Core_Opt*
_temp1472; struct Cyc_Core_Opt* _temp1474; struct Cyc_Core_Opt* _temp1476;
_LL1458: _LL1469: _temp1468= _temp1456.f1; if( _temp1468 == 0){ goto _LL1467;}
else{ goto _LL1460;} _LL1467: _temp1466= _temp1456.f2; if( _temp1466 == 0){ goto
_LL1459;} else{ goto _LL1460;} _LL1460: _LL1473: _temp1472= _temp1456.f1; if(
_temp1472 == 0){ goto _LL1471;} else{ goto _LL1462;} _LL1471: _temp1470=
_temp1456.f2; goto _LL1461; _LL1462: _LL1477: _temp1476= _temp1456.f1; goto
_LL1475; _LL1475: _temp1474= _temp1456.f2; if( _temp1474 == 0){ goto _LL1463;}
else{ goto _LL1464;} _LL1464: goto _LL1465; _LL1459: goto _LL1457; _LL1461: goto
_LL1463; _LL1463: bad_effect= 1; goto _LL1457; _LL1465: bad_effect= ! Cyc_Tcutil_unify_effect((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1290))->v,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1308))->v); goto _LL1457; _LL1457:;} if( bad_effect){ goto
_LL1047;} if( ! Cyc_Tcutil_same_atts( _temp1298, _temp1280)){ goto _LL1047;}
return;}}} _LL1077: for( 0; _temp1314 != 0? _temp1318 != 0: 0;( _temp1314=((
struct Cyc_List_List*) _check_null( _temp1314))->tl, _temp1318=(( struct Cyc_List_List*)
_check_null( _temp1318))->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1314))->hd)).f1,(*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1318))->hd)).f1); Cyc_Tcutil_unify_it((*((
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1314))->hd)).f2,(*((
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1318))->hd)).f2);}
if( _temp1314 == 0? _temp1318 == 0: 0){ return;} goto _LL1047; _LL1079:
_temp1334= _temp1326; _temp1330= _temp1322; goto _LL1081; _LL1081: for( 0;
_temp1330 != 0? _temp1334 != 0: 0;( _temp1330=(( struct Cyc_List_List*)
_check_null( _temp1330))->tl, _temp1334=(( struct Cyc_List_List*) _check_null(
_temp1334))->tl)){ struct Cyc_Absyn_Structfield* _temp1478=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp1330))->hd; goto _LL1479; _LL1479: {
struct Cyc_Absyn_Structfield* _temp1480=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp1334))->hd; goto _LL1481; _LL1481: if(
Cyc_String_zstrptrcmp( _temp1478->name, _temp1480->name) != 0){( void) _throw((
void*) Cyc_Tcutil_Unify);} Cyc_Tcutil_unify_tqual( _temp1478->tq, _temp1480->tq);
Cyc_Tcutil_unify_it(( void*) _temp1478->type,( void*) _temp1480->type); if((( !
Cyc_Tcutil_same_atts( _temp1478->attributes, _temp1480->attributes)? 1:(
_temp1478->width != 0? _temp1480->width == 0: 0))? 1:( _temp1480->width != 0?
_temp1478->width == 0: 0))? 1:(( _temp1478->width != 0? _temp1480->width != 0: 0)?
Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*)
_check_null( _temp1478->width))->v) != Cyc_Evexp_eval_const_uint_exp(( struct
Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp1480->width))->v): 0)){(
void) _throw(( void*) Cyc_Tcutil_Unify);}}} if( _temp1330 == 0? _temp1334 == 0:
0){ return;} goto _LL1047; _LL1083: return; _LL1085: Cyc_Tcutil_unify_it(
_temp1346, _temp1342); return; _LL1087: goto _LL1089; _LL1089: goto _LL1091;
_LL1091: goto _LL1093; _LL1093: if( Cyc_Tcutil_unify_effect( t1, t2)){ return;}
else{ goto _LL1047;} _LL1095: goto _LL1047; _LL1047:;}( void) _throw(( void*)
Cyc_Tcutil_Unify);} int Cyc_Tcutil_is_arithmetic_type( void* t){ void* _temp1482=
Cyc_Tcutil_compress( t); void* _temp1494; void* _temp1496; struct Cyc_Absyn_Enumdecl*
_temp1498; struct _tuple1* _temp1500; _LL1484: if(( unsigned int) _temp1482 > 4u?*((
int*) _temp1482) == Cyc_Absyn_IntType: 0){ _LL1497: _temp1496=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1482)->f1; goto _LL1495; _LL1495: _temp1494=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1482)->f2; goto _LL1485;} else{
goto _LL1486;} _LL1486: if( _temp1482 ==( void*) Cyc_Absyn_FloatType){ goto
_LL1487;} else{ goto _LL1488;} _LL1488: if( _temp1482 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1489;} else{ goto _LL1490;} _LL1490: if(( unsigned int) _temp1482 > 4u?*((
int*) _temp1482) == Cyc_Absyn_EnumType: 0){ _LL1501: _temp1500=(( struct Cyc_Absyn_EnumType_struct*)
_temp1482)->f1; goto _LL1499; _LL1499: _temp1498=(( struct Cyc_Absyn_EnumType_struct*)
_temp1482)->f2; goto _LL1491;} else{ goto _LL1492;} _LL1492: goto _LL1493;
_LL1485: return 1; _LL1487: return 1; _LL1489: return 1; _LL1491: return 1;
_LL1493: return 0; _LL1483:;} int Cyc_Tcutil_will_lose_precision( void* t1, void*
t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8
_temp1503=({ struct _tuple8 _temp1502; _temp1502.f1= t1; _temp1502.f2= t2;
_temp1502;}); void* _temp1525; void* _temp1527; void* _temp1529; void* _temp1531;
void* _temp1533; void* _temp1535; void* _temp1537; void* _temp1539; void*
_temp1541; void* _temp1543; void* _temp1545; void* _temp1547; void* _temp1549;
void* _temp1551; void* _temp1553; void* _temp1555; void* _temp1557; void*
_temp1559; void* _temp1561; void* _temp1563; void* _temp1565; void* _temp1567;
void* _temp1569; void* _temp1571; void* _temp1573; void* _temp1575; void*
_temp1577; void* _temp1579; void* _temp1581; void* _temp1583; void* _temp1585;
void* _temp1587; void* _temp1589; void* _temp1591; void* _temp1593; void*
_temp1595; void* _temp1597; void* _temp1599; void* _temp1601; void* _temp1603;
void* _temp1605; void* _temp1607; _LL1505: _LL1528: _temp1527= _temp1503.f1; if(
_temp1527 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1526;} else{ goto _LL1507;}
_LL1526: _temp1525= _temp1503.f2; if( _temp1525 ==( void*) Cyc_Absyn_FloatType){
goto _LL1506;} else{ goto _LL1507;} _LL1507: _LL1536: _temp1535= _temp1503.f1;
if( _temp1535 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1530;} else{ goto
_LL1509;} _LL1530: _temp1529= _temp1503.f2; if(( unsigned int) _temp1529 > 4u?*((
int*) _temp1529) == Cyc_Absyn_IntType: 0){ _LL1534: _temp1533=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1529)->f1; goto _LL1532; _LL1532: _temp1531=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1529)->f2; goto _LL1508;} else{
goto _LL1509;} _LL1509: _LL1544: _temp1543= _temp1503.f1; if( _temp1543 ==( void*)
Cyc_Absyn_FloatType){ goto _LL1538;} else{ goto _LL1511;} _LL1538: _temp1537=
_temp1503.f2; if(( unsigned int) _temp1537 > 4u?*(( int*) _temp1537) == Cyc_Absyn_IntType:
0){ _LL1542: _temp1541=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1537)->f1;
goto _LL1540; _LL1540: _temp1539=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1537)->f2; goto _LL1510;} else{ goto _LL1511;} _LL1511: _LL1552: _temp1551=
_temp1503.f1; if(( unsigned int) _temp1551 > 4u?*(( int*) _temp1551) == Cyc_Absyn_IntType:
0){ _LL1556: _temp1555=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1551)->f1;
goto _LL1554; _LL1554: _temp1553=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1551)->f2; if( _temp1553 ==( void*) Cyc_Absyn_B8){ goto _LL1546;} else{
goto _LL1513;}} else{ goto _LL1513;} _LL1546: _temp1545= _temp1503.f2; if((
unsigned int) _temp1545 > 4u?*(( int*) _temp1545) == Cyc_Absyn_IntType: 0){
_LL1550: _temp1549=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1545)->f1;
goto _LL1548; _LL1548: _temp1547=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1545)->f2; if( _temp1547 ==( void*) Cyc_Absyn_B8){ goto _LL1512;} else{
goto _LL1513;}} else{ goto _LL1513;} _LL1513: _LL1560: _temp1559= _temp1503.f1;
if(( unsigned int) _temp1559 > 4u?*(( int*) _temp1559) == Cyc_Absyn_IntType: 0){
_LL1564: _temp1563=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1559)->f1;
goto _LL1562; _LL1562: _temp1561=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1559)->f2; if( _temp1561 ==( void*) Cyc_Absyn_B8){ goto _LL1558;} else{
goto _LL1515;}} else{ goto _LL1515;} _LL1558: _temp1557= _temp1503.f2; goto
_LL1514; _LL1515: _LL1568: _temp1567= _temp1503.f1; if(( unsigned int) _temp1567
> 4u?*(( int*) _temp1567) == Cyc_Absyn_IntType: 0){ _LL1572: _temp1571=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1567)->f1; goto _LL1570; _LL1570:
_temp1569=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1567)->f2; if(
_temp1569 ==( void*) Cyc_Absyn_B4){ goto _LL1566;} else{ goto _LL1517;}} else{
goto _LL1517;} _LL1566: _temp1565= _temp1503.f2; if( _temp1565 ==( void*) Cyc_Absyn_FloatType){
goto _LL1516;} else{ goto _LL1517;} _LL1517: _LL1580: _temp1579= _temp1503.f1;
if(( unsigned int) _temp1579 > 4u?*(( int*) _temp1579) == Cyc_Absyn_IntType: 0){
_LL1584: _temp1583=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1579)->f1;
goto _LL1582; _LL1582: _temp1581=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1579)->f2; if( _temp1581 ==( void*) Cyc_Absyn_B4){ goto _LL1574;} else{
goto _LL1519;}} else{ goto _LL1519;} _LL1574: _temp1573= _temp1503.f2; if((
unsigned int) _temp1573 > 4u?*(( int*) _temp1573) == Cyc_Absyn_IntType: 0){
_LL1578: _temp1577=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1573)->f1;
goto _LL1576; _LL1576: _temp1575=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1573)->f2; if( _temp1575 ==( void*) Cyc_Absyn_B2){ goto _LL1518;} else{
goto _LL1519;}} else{ goto _LL1519;} _LL1519: _LL1592: _temp1591= _temp1503.f1;
if(( unsigned int) _temp1591 > 4u?*(( int*) _temp1591) == Cyc_Absyn_IntType: 0){
_LL1596: _temp1595=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1591)->f1;
goto _LL1594; _LL1594: _temp1593=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1591)->f2; if( _temp1593 ==( void*) Cyc_Absyn_B4){ goto _LL1586;} else{
goto _LL1521;}} else{ goto _LL1521;} _LL1586: _temp1585= _temp1503.f2; if((
unsigned int) _temp1585 > 4u?*(( int*) _temp1585) == Cyc_Absyn_IntType: 0){
_LL1590: _temp1589=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1585)->f1;
goto _LL1588; _LL1588: _temp1587=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1585)->f2; if( _temp1587 ==( void*) Cyc_Absyn_B1){ goto _LL1520;} else{
goto _LL1521;}} else{ goto _LL1521;} _LL1521: _LL1604: _temp1603= _temp1503.f1;
if(( unsigned int) _temp1603 > 4u?*(( int*) _temp1603) == Cyc_Absyn_IntType: 0){
_LL1608: _temp1607=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1603)->f1;
goto _LL1606; _LL1606: _temp1605=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1603)->f2; if( _temp1605 ==( void*) Cyc_Absyn_B2){ goto _LL1598;} else{
goto _LL1523;}} else{ goto _LL1523;} _LL1598: _temp1597= _temp1503.f2; if((
unsigned int) _temp1597 > 4u?*(( int*) _temp1597) == Cyc_Absyn_IntType: 0){
_LL1602: _temp1601=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1597)->f1;
goto _LL1600; _LL1600: _temp1599=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1597)->f2; if( _temp1599 ==( void*) Cyc_Absyn_B1){ goto _LL1522;} else{
goto _LL1523;}} else{ goto _LL1523;} _LL1523: goto _LL1524; _LL1506: return 1;
_LL1508: return 1; _LL1510: return 1; _LL1512: return 0; _LL1514: return 1;
_LL1516: return 1; _LL1518: return 1; _LL1520: return 1; _LL1522: return 1;
_LL1524: return 0; _LL1504:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{
struct Cyc_List_List* el= es; for( 0; el != 0; el=(( struct Cyc_List_List*)
_check_null( el))->tl){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);
if( Cyc_Tcutil_is_arithmetic_type( t1)){ if( max_arith_type == 0? 1: Cyc_Tcutil_will_lose_precision(
t1,( void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){
max_arith_type=({ struct Cyc_Core_Opt* _temp1609=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1609->v=( void*) t1; _temp1609;});}}}}
if( max_arith_type != 0){ if( ! Cyc_Tcutil_unify( t,( void*)(( struct Cyc_Core_Opt*)
_check_null( max_arith_type))->v)){ return 0;}}{ struct Cyc_List_List* el= es;
for( 0; el != 0; el=(( struct Cyc_List_List*) _check_null( el))->tl){ if( ! Cyc_Tcutil_coerce_assign(
te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd, t)){
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
el))->hd)->loc,( struct _tagged_string)({ struct _tagged_string _temp1610= Cyc_Absynpp_typ2string(
t); struct _tagged_string _temp1611= Cyc_Absynpp_typ2string(( void*)(( struct
Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( el))->hd)->topt))->v); xprintf("type mismatch: expecting %.*s but found %.*s",
_temp1610.last_plus_one - _temp1610.curr, _temp1610.curr, _temp1611.last_plus_one
- _temp1611.curr, _temp1611.curr);})); return 0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e)){ void* _temp1612= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); struct Cyc_Absyn_PtrInfo _temp1618; _LL1614: if((
unsigned int) _temp1612 > 4u?*(( int*) _temp1612) == Cyc_Absyn_PointerType: 0){
_LL1619: _temp1618=(( struct Cyc_Absyn_PointerType_struct*) _temp1612)->f1; goto
_LL1615;} else{ goto _LL1616;} _LL1616: goto _LL1617; _LL1615: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); goto _LL1613; _LL1617: return 0; _LL1613:;} return 1;}
int Cyc_Tcutil_is_integral_type( void* t){ void* _temp1620= Cyc_Tcutil_compress(
t); void* _temp1628; void* _temp1630; struct Cyc_Absyn_Enumdecl* _temp1632;
struct _tuple1* _temp1634; _LL1622: if(( unsigned int) _temp1620 > 4u?*(( int*)
_temp1620) == Cyc_Absyn_IntType: 0){ _LL1631: _temp1630=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1620)->f1; goto _LL1629; _LL1629: _temp1628=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1620)->f2; goto _LL1623;} else{ goto _LL1624;} _LL1624: if(( unsigned int)
_temp1620 > 4u?*(( int*) _temp1620) == Cyc_Absyn_EnumType: 0){ _LL1635:
_temp1634=(( struct Cyc_Absyn_EnumType_struct*) _temp1620)->f1; goto _LL1633;
_LL1633: _temp1632=(( struct Cyc_Absyn_EnumType_struct*) _temp1620)->f2; goto
_LL1625;} else{ goto _LL1626;} _LL1626: goto _LL1627; _LL1623: return 1; _LL1625:
return 1; _LL1627: return 0; _LL1621:;} int Cyc_Tcutil_coerce_uint_typ( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_uint_t)){ return 1;}
if( Cyc_Tcutil_is_integral_type(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){
if( Cyc_Tcutil_will_lose_precision(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, Cyc_Absyn_uint_t)){ Cyc_Tcutil_warn( e->loc,({ unsigned char*
_temp1636=( unsigned char*)"integral size mismatch; conversion supplied"; struct
_tagged_string _temp1637; _temp1637.curr= _temp1636; _temp1637.base= _temp1636;
_temp1637.last_plus_one= _temp1636 + 44; _temp1637;}));} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;}
if( Cyc_Tcutil_is_integral_type(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){
if( Cyc_Tcutil_will_lose_precision(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, Cyc_Absyn_sint_t)){ Cyc_Tcutil_warn( e->loc,({ unsigned char*
_temp1638=( unsigned char*)"integral size mismatch; conversion supplied"; struct
_tagged_string _temp1639; _temp1639.curr= _temp1638; _temp1639.base= _temp1638;
_temp1639.last_plus_one= _temp1638 + 44; _temp1639;}));} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); return 1;} return 0;} int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8
_temp1641=({ struct _tuple8 _temp1640; _temp1640.f1= t1; _temp1640.f2= t2;
_temp1640;}); void* _temp1653; struct Cyc_Absyn_PtrInfo _temp1655; void*
_temp1657; struct Cyc_Absyn_PtrInfo _temp1659; void* _temp1661; struct Cyc_Absyn_Exp*
_temp1663; struct Cyc_Absyn_Tqual _temp1665; void* _temp1667; void* _temp1669;
struct Cyc_Absyn_Exp* _temp1671; struct Cyc_Absyn_Tqual _temp1673; void*
_temp1675; void* _temp1677; struct Cyc_Absyn_TunionInfo _temp1679; void*
_temp1681; struct Cyc_List_List* _temp1683; void* _temp1685; struct Cyc_Absyn_Tuniondecl*
_temp1687; void* _temp1689; struct Cyc_Absyn_TunionFieldInfo _temp1691; struct
Cyc_List_List* _temp1693; void* _temp1695; struct Cyc_Absyn_Tunionfield*
_temp1697; struct Cyc_Absyn_Tuniondecl* _temp1699; void* _temp1701; struct Cyc_Absyn_TunionInfo
_temp1703; void* _temp1705; struct Cyc_List_List* _temp1707; void* _temp1709;
struct Cyc_Absyn_Tuniondecl* _temp1711; void* _temp1713; struct Cyc_Absyn_PtrInfo
_temp1715; struct Cyc_Absyn_Conref* _temp1717; struct Cyc_Absyn_Tqual _temp1719;
struct Cyc_Absyn_Conref* _temp1721; void* _temp1723; void* _temp1725; _LL1643:
_LL1658: _temp1657= _temp1641.f1; if(( unsigned int) _temp1657 > 4u?*(( int*)
_temp1657) == Cyc_Absyn_PointerType: 0){ _LL1660: _temp1659=(( struct Cyc_Absyn_PointerType_struct*)
_temp1657)->f1; goto _LL1654;} else{ goto _LL1645;} _LL1654: _temp1653=
_temp1641.f2; if(( unsigned int) _temp1653 > 4u?*(( int*) _temp1653) == Cyc_Absyn_PointerType:
0){ _LL1656: _temp1655=(( struct Cyc_Absyn_PointerType_struct*) _temp1653)->f1;
goto _LL1644;} else{ goto _LL1645;} _LL1645: _LL1670: _temp1669= _temp1641.f1;
if(( unsigned int) _temp1669 > 4u?*(( int*) _temp1669) == Cyc_Absyn_ArrayType: 0){
_LL1676: _temp1675=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1669)->f1;
goto _LL1674; _LL1674: _temp1673=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1669)->f2; goto _LL1672; _LL1672: _temp1671=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1669)->f3; goto _LL1662;} else{ goto _LL1647;} _LL1662: _temp1661=
_temp1641.f2; if(( unsigned int) _temp1661 > 4u?*(( int*) _temp1661) == Cyc_Absyn_ArrayType:
0){ _LL1668: _temp1667=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1661)->f1;
goto _LL1666; _LL1666: _temp1665=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1661)->f2; goto _LL1664; _LL1664: _temp1663=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1661)->f3; goto _LL1646;} else{ goto _LL1647;} _LL1647: _LL1690: _temp1689=
_temp1641.f1; if(( unsigned int) _temp1689 > 4u?*(( int*) _temp1689) == Cyc_Absyn_TunionFieldType:
0){ _LL1692: _temp1691=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1689)->f1;
_LL1696: _temp1695=( void*) _temp1691.field_info; if(*(( int*) _temp1695) == Cyc_Absyn_KnownTunionfield){
_LL1700: _temp1699=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1695)->f1;
goto _LL1698; _LL1698: _temp1697=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1695)->f2; goto _LL1694;} else{ goto _LL1649;} _LL1694: _temp1693=
_temp1691.targs; goto _LL1678;} else{ goto _LL1649;} _LL1678: _temp1677=
_temp1641.f2; if(( unsigned int) _temp1677 > 4u?*(( int*) _temp1677) == Cyc_Absyn_TunionType:
0){ _LL1680: _temp1679=(( struct Cyc_Absyn_TunionType_struct*) _temp1677)->f1;
_LL1686: _temp1685=( void*) _temp1679.tunion_info; if(*(( int*) _temp1685) ==
Cyc_Absyn_KnownTunion){ _LL1688: _temp1687=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1685)->f1; goto _LL1684;} else{ goto _LL1649;} _LL1684: _temp1683=
_temp1679.targs; goto _LL1682; _LL1682: _temp1681=( void*) _temp1679.rgn; goto
_LL1648;} else{ goto _LL1649;} _LL1649: _LL1714: _temp1713= _temp1641.f1; if((
unsigned int) _temp1713 > 4u?*(( int*) _temp1713) == Cyc_Absyn_PointerType: 0){
_LL1716: _temp1715=(( struct Cyc_Absyn_PointerType_struct*) _temp1713)->f1;
_LL1726: _temp1725=( void*) _temp1715.elt_typ; goto _LL1724; _LL1724: _temp1723=(
void*) _temp1715.rgn_typ; goto _LL1722; _LL1722: _temp1721= _temp1715.nullable;
goto _LL1720; _LL1720: _temp1719= _temp1715.tq; goto _LL1718; _LL1718: _temp1717=
_temp1715.bounds; goto _LL1702;} else{ goto _LL1651;} _LL1702: _temp1701=
_temp1641.f2; if(( unsigned int) _temp1701 > 4u?*(( int*) _temp1701) == Cyc_Absyn_TunionType:
0){ _LL1704: _temp1703=(( struct Cyc_Absyn_TunionType_struct*) _temp1701)->f1;
_LL1710: _temp1709=( void*) _temp1703.tunion_info; if(*(( int*) _temp1709) ==
Cyc_Absyn_KnownTunion){ _LL1712: _temp1711=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1709)->f1; goto _LL1708;} else{ goto _LL1651;} _LL1708: _temp1707=
_temp1703.targs; goto _LL1706; _LL1706: _temp1705=( void*) _temp1703.rgn; goto
_LL1650;} else{ goto _LL1651;} _LL1651: goto _LL1652; _LL1644: { int okay= 1;
if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1659.nullable, _temp1655.nullable)){
void* _temp1727=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp1659.nullable))->v; int _temp1733; _LL1729:
if(( unsigned int) _temp1727 > 1u?*(( int*) _temp1727) == Cyc_Absyn_Eq_constr: 0){
_LL1734: _temp1733=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1727)->f1;
goto _LL1730;} else{ goto _LL1731;} _LL1731: goto _LL1732; _LL1730: okay= !
_temp1733; goto _LL1728; _LL1732:( void) _throw((( void*(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp1735=(
unsigned char*)"silent_castable conref not eq"; struct _tagged_string _temp1736;
_temp1736.curr= _temp1735; _temp1736.base= _temp1735; _temp1736.last_plus_one=
_temp1735 + 30; _temp1736;}))); _LL1728:;} if( !(( int(*)( int(* cmp)( void*,
void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp1659.bounds, _temp1655.bounds)){ struct _tuple8
_temp1738=({ struct _tuple8 _temp1737; _temp1737.f1=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1659.bounds))->v;
_temp1737.f2=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp1655.bounds))->v; _temp1737;}); void* _temp1748;
void* _temp1750; void* _temp1752; void* _temp1754; struct Cyc_Absyn_Exp*
_temp1756; void* _temp1758; void* _temp1760; struct Cyc_Absyn_Exp* _temp1762;
void* _temp1764; void* _temp1766; struct Cyc_Absyn_Exp* _temp1768; void*
_temp1770; void* _temp1772; void* _temp1774; void* _temp1776; _LL1740: _LL1753:
_temp1752= _temp1738.f1; if(( unsigned int) _temp1752 > 1u?*(( int*) _temp1752)
== Cyc_Absyn_Eq_constr: 0){ _LL1755: _temp1754=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1752)->f1; if(( unsigned int) _temp1754 > 1u?*(( int*) _temp1754) == Cyc_Absyn_Upper_b:
0){ _LL1757: _temp1756=(( struct Cyc_Absyn_Upper_b_struct*) _temp1754)->f1; goto
_LL1749;} else{ goto _LL1742;}} else{ goto _LL1742;} _LL1749: _temp1748=
_temp1738.f2; if(( unsigned int) _temp1748 > 1u?*(( int*) _temp1748) == Cyc_Absyn_Eq_constr:
0){ _LL1751: _temp1750=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1748)->f1;
if( _temp1750 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1741;} else{ goto _LL1742;}}
else{ goto _LL1742;} _LL1742: _LL1765: _temp1764= _temp1738.f1; if((
unsigned int) _temp1764 > 1u?*(( int*) _temp1764) == Cyc_Absyn_Eq_constr: 0){
_LL1767: _temp1766=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1764)->f1;
if(( unsigned int) _temp1766 > 1u?*(( int*) _temp1766) == Cyc_Absyn_Upper_b: 0){
_LL1769: _temp1768=(( struct Cyc_Absyn_Upper_b_struct*) _temp1766)->f1; goto
_LL1759;} else{ goto _LL1744;}} else{ goto _LL1744;} _LL1759: _temp1758=
_temp1738.f2; if(( unsigned int) _temp1758 > 1u?*(( int*) _temp1758) == Cyc_Absyn_Eq_constr:
0){ _LL1761: _temp1760=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1758)->f1;
if(( unsigned int) _temp1760 > 1u?*(( int*) _temp1760) == Cyc_Absyn_Upper_b: 0){
_LL1763: _temp1762=(( struct Cyc_Absyn_Upper_b_struct*) _temp1760)->f1; goto
_LL1743;} else{ goto _LL1744;}} else{ goto _LL1744;} _LL1744: _LL1775: _temp1774=
_temp1738.f1; if(( unsigned int) _temp1774 > 1u?*(( int*) _temp1774) == Cyc_Absyn_Eq_constr:
0){ _LL1777: _temp1776=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1774)->f1;
if( _temp1776 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1771;} else{ goto _LL1746;}}
else{ goto _LL1746;} _LL1771: _temp1770= _temp1738.f2; if(( unsigned int)
_temp1770 > 1u?*(( int*) _temp1770) == Cyc_Absyn_Eq_constr: 0){ _LL1773:
_temp1772=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1770)->f1; if(
_temp1772 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1745;} else{ goto _LL1746;}}
else{ goto _LL1746;} _LL1746: goto _LL1747; _LL1741: okay= 1; goto _LL1739;
_LL1743: okay= okay? Cyc_Evexp_eval_const_uint_exp( _temp1768) >= Cyc_Evexp_eval_const_uint_exp(
_temp1762): 0; Cyc_Tcutil_warn( loc,({ unsigned char* _temp1778=( unsigned char*)"implicit cast to shorter array";
struct _tagged_string _temp1779; _temp1779.curr= _temp1778; _temp1779.base=
_temp1778; _temp1779.last_plus_one= _temp1778 + 31; _temp1779;})); goto _LL1739;
_LL1745: okay= 1; goto _LL1739; _LL1747: okay= 0; goto _LL1739; _LL1739:;}
return(( okay? Cyc_Tcutil_unify(( void*) _temp1659.elt_typ,( void*) _temp1655.elt_typ):
0)? Cyc_Tcutil_unify(( void*) _temp1659.rgn_typ,( void*) _temp1655.rgn_typ): 0)?
!( _temp1659.tq).q_const? 1:( _temp1655.tq).q_const: 0;} _LL1646: { int okay;
okay=( _temp1671 != 0? _temp1663 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1671)) == Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1663)): 0; return( okay? Cyc_Tcutil_unify(
_temp1675, _temp1667): 0)? ! _temp1673.q_const? 1: _temp1665.q_const: 0;}
_LL1648: if(( _temp1699 == _temp1687? 1: Cyc_Absyn_qvar_cmp( _temp1699->name,
_temp1687->name) == 0)? _temp1697->typs == 0: 0){ for( 0; _temp1693 != 0?
_temp1683 != 0: 0;( _temp1693=(( struct Cyc_List_List*) _check_null( _temp1693))->tl,
_temp1683=(( struct Cyc_List_List*) _check_null( _temp1683))->tl)){ if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_List_List*) _check_null( _temp1693))->hd,( void*)(( struct
Cyc_List_List*) _check_null( _temp1683))->hd)){ break;}} if( _temp1693 == 0?
_temp1683 == 0: 0){ return 1;}} return 0; _LL1650:{ void* _temp1780= Cyc_Tcutil_compress(
_temp1725); struct Cyc_Absyn_TunionFieldInfo _temp1786; struct Cyc_List_List*
_temp1788; void* _temp1790; struct Cyc_Absyn_Tunionfield* _temp1792; struct Cyc_Absyn_Tuniondecl*
_temp1794; _LL1782: if(( unsigned int) _temp1780 > 4u?*(( int*) _temp1780) ==
Cyc_Absyn_TunionFieldType: 0){ _LL1787: _temp1786=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1780)->f1; _LL1791: _temp1790=( void*) _temp1786.field_info; if(*(( int*)
_temp1790) == Cyc_Absyn_KnownTunionfield){ _LL1795: _temp1794=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1790)->f1; goto _LL1793; _LL1793: _temp1792=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1790)->f2; goto _LL1789;} else{ goto _LL1784;} _LL1789: _temp1788=
_temp1786.targs; goto _LL1783;} else{ goto _LL1784;} _LL1784: goto _LL1785;
_LL1783: if( ! Cyc_Tcutil_unify( _temp1723, _temp1705)? _temp1723 !=( void*) Cyc_Absyn_HeapRgn:
0){ return 0;} if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1721,(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)( 0))){
return 0;} if( !(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp,
_temp1717,(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)( Cyc_Absyn_bounds_one))){
return 0;} if( Cyc_Absyn_qvar_cmp( _temp1711->name, _temp1794->name) == 0?
_temp1792->typs != 0: 0){ int okay= 1; for( 0; _temp1788 != 0? _temp1707 != 0: 0;(
_temp1788=(( struct Cyc_List_List*) _check_null( _temp1788))->tl, _temp1707=((
struct Cyc_List_List*) _check_null( _temp1707))->tl)){ if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_List_List*) _check_null( _temp1788))->hd,( void*)(( struct
Cyc_List_List*) _check_null( _temp1707))->hd)){ okay= 0; break;}} if(( ! okay? 1:
_temp1788 != 0)? 1: _temp1707 != 0){ return 0;} return 1;} goto _LL1781; _LL1785:
goto _LL1781; _LL1781:;} return 0; _LL1652: return Cyc_Tcutil_unify( t1, t2);
_LL1642:;}} int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} if( Cyc_Tcutil_is_arithmetic_type(
t1)? Cyc_Tcutil_is_arithmetic_type( t2): 0){ if( Cyc_Tcutil_will_lose_precision(
t1, t2)){ Cyc_Tcutil_warn( e->loc,({ unsigned char* _temp1796=( unsigned char*)"integral size mismatch; conversion supplied";
struct _tagged_string _temp1797; _temp1797.curr= _temp1796; _temp1797.base=
_temp1796; _temp1797.last_plus_one= _temp1796 + 44; _temp1797;}));} Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){
Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2); Cyc_Tcutil_warn( e->loc,(
struct _tagged_string)({ struct _tagged_string _temp1798= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp1799= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1798.last_plus_one - _temp1798.curr, _temp1798.curr, _temp1799.last_plus_one
- _temp1799.curr, _temp1799.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg(
te, e, t);} static int Cyc_Tcutil_any_promote( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e){ void* old_typ= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); void* t;{ void* _temp1800= old_typ; struct Cyc_Absyn_Enumdecl*
_temp1812; struct _tuple1* _temp1814; void* _temp1816; void* _temp1818; _LL1802:
if(( unsigned int) _temp1800 > 4u?*(( int*) _temp1800) == Cyc_Absyn_EnumType: 0){
_LL1815: _temp1814=(( struct Cyc_Absyn_EnumType_struct*) _temp1800)->f1; goto
_LL1813; _LL1813: _temp1812=(( struct Cyc_Absyn_EnumType_struct*) _temp1800)->f2;
goto _LL1803;} else{ goto _LL1804;} _LL1804: if(( unsigned int) _temp1800 > 4u?*((
int*) _temp1800) == Cyc_Absyn_IntType: 0){ _LL1819: _temp1818=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1800)->f1; goto _LL1817; _LL1817: _temp1816=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1800)->f2; goto _LL1805;} else{
goto _LL1806;} _LL1806: if( _temp1800 ==( void*) Cyc_Absyn_FloatType){ goto
_LL1807;} else{ goto _LL1808;} _LL1808: if( _temp1800 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1809;} else{ goto _LL1810;} _LL1810: goto _LL1811; _LL1803: t= Cyc_Absyn_sint_t;
goto _LL1801; _LL1805:{ void* _temp1820= _temp1816; _LL1822: if( _temp1820 ==(
void*) Cyc_Absyn_B1){ goto _LL1823;} else{ goto _LL1824;} _LL1824: if( _temp1820
==( void*) Cyc_Absyn_B2){ goto _LL1825;} else{ goto _LL1826;} _LL1826: if(
_temp1820 ==( void*) Cyc_Absyn_B4){ goto _LL1827;} else{ goto _LL1828;} _LL1828:
if( _temp1820 ==( void*) Cyc_Absyn_B8){ goto _LL1829;} else{ goto _LL1821;}
_LL1823: t= Cyc_Absyn_sint_t; goto _LL1821; _LL1825: t= Cyc_Absyn_sint_t; goto
_LL1821; _LL1827: t= _temp1818 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL1821; _LL1829: t= _temp1818 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_ulong_t: Cyc_Absyn_slong_t; goto _LL1821; _LL1821:;} goto _LL1801;
_LL1807: t= Cyc_Absyn_float_t; goto _LL1801; _LL1809: t= Cyc_Absyn_double_t;
goto _LL1801; _LL1811: return 0; _LL1801:;} Cyc_Tcutil_unchecked_cast( te, e, t);
return 1;} int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} Cyc_Tcutil_any_promote(
te, e); if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v,
t2)){ return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); Cyc_Tcutil_warn( e->loc,( struct _tagged_string)({ struct
_tagged_string _temp1830= Cyc_Absynpp_typ2string( t1); struct _tagged_string
_temp1831= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1830.last_plus_one - _temp1830.curr, _temp1830.curr, _temp1831.last_plus_one
- _temp1831.curr, _temp1831.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerceable(
void* t){ void* _temp1832= Cyc_Tcutil_compress( t); void* _temp1842; void*
_temp1844; _LL1834: if(( unsigned int) _temp1832 > 4u?*(( int*) _temp1832) ==
Cyc_Absyn_IntType: 0){ _LL1845: _temp1844=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1832)->f1; goto _LL1843; _LL1843: _temp1842=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1832)->f2; goto _LL1835;} else{ goto _LL1836;} _LL1836: if( _temp1832 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1837;} else{ goto _LL1838;} _LL1838: if(
_temp1832 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1839;} else{ goto _LL1840;}
_LL1840: goto _LL1841; _LL1835: return 1; _LL1837: return 1; _LL1839: return 1;
_LL1841: return 0; _LL1833:;} static struct _tuple4* Cyc_Tcutil_flatten_typ_f(
struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield* x){ return({ struct
_tuple4* _temp1846=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1846->f1= x->tq; _temp1846->f2= Cyc_Tcutil_substitute( inst,( void*) x->type);
_temp1846;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ( struct Cyc_Tcenv_Tenv*
te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void* _temp1847= t1; struct Cyc_List_List*
_temp1859; struct Cyc_Absyn_Structdecl** _temp1861; struct Cyc_List_List*
_temp1863; struct _tuple1* _temp1865; struct Cyc_List_List* _temp1867; _LL1849:
if( _temp1847 ==( void*) Cyc_Absyn_VoidType){ goto _LL1850;} else{ goto _LL1851;}
_LL1851: if(( unsigned int) _temp1847 > 4u?*(( int*) _temp1847) == Cyc_Absyn_TupleType:
0){ _LL1860: _temp1859=(( struct Cyc_Absyn_TupleType_struct*) _temp1847)->f1;
goto _LL1852;} else{ goto _LL1853;} _LL1853: if(( unsigned int) _temp1847 > 4u?*((
int*) _temp1847) == Cyc_Absyn_StructType: 0){ _LL1866: _temp1865=(( struct Cyc_Absyn_StructType_struct*)
_temp1847)->f1; goto _LL1864; _LL1864: _temp1863=(( struct Cyc_Absyn_StructType_struct*)
_temp1847)->f2; goto _LL1862; _LL1862: _temp1861=(( struct Cyc_Absyn_StructType_struct*)
_temp1847)->f3; goto _LL1854;} else{ goto _LL1855;} _LL1855: if(( unsigned int)
_temp1847 > 4u?*(( int*) _temp1847) == Cyc_Absyn_AnonStructType: 0){ _LL1868:
_temp1867=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1847)->f1; goto
_LL1856;} else{ goto _LL1857;} _LL1857: goto _LL1858; _LL1850: return 0; _LL1852:
return _temp1859; _LL1854: if( _temp1861 == 0? 1: _temp1865 == 0){ return({
struct Cyc_List_List* _temp1869=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1869->hd=( void*)({ struct _tuple4* _temp1870=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1870->f1= Cyc_Absyn_empty_tqual();
_temp1870->f2= t1; _temp1870;}); _temp1869->tl= 0; _temp1869;});}{ struct Cyc_Absyn_Structdecl*
_temp1871=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp1861)); goto
_LL1872; _LL1872: { struct _tuple1 _temp1873=*(( struct _tuple1*) _check_null(
_temp1865)); goto _LL1874; _LL1874: { struct Cyc_List_List* _temp1875=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1871->tvs, _temp1863); goto _LL1876; _LL1876: if( _temp1871->fields == 0){
return({ struct Cyc_List_List* _temp1877=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1877->hd=( void*)({ struct _tuple4*
_temp1878=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1878->f1=
Cyc_Absyn_empty_tqual(); _temp1878->f2= t1; _temp1878;}); _temp1877->tl= 0;
_temp1877;});} return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, _temp1875,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1871->fields))->v);}}}
_LL1856: return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, 0, _temp1867); _LL1858: return({
struct Cyc_List_List* _temp1879=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1879->hd=( void*)({ struct _tuple4* _temp1880=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1880->f1= Cyc_Absyn_empty_tqual();
_temp1880->f2= t1; _temp1880;}); _temp1879->tl= 0; _temp1879;}); _LL1848:;}}
static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2); static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;}{ struct Cyc_List_List* a= assume; for( 0; a != 0; a=(( struct
Cyc_List_List*) _check_null( a))->tl){ if( Cyc_Tcutil_unify( t1,(*(( struct
_tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f1)? Cyc_Tcutil_unify(
t2,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f2): 0){
return 1;}}} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct
_tuple8 _temp1882=({ struct _tuple8 _temp1881; _temp1881.f1= t1; _temp1881.f2=
t2; _temp1881;}); void* _temp1888; struct Cyc_Absyn_PtrInfo _temp1890; struct
Cyc_Absyn_Conref* _temp1892; struct Cyc_Absyn_Tqual _temp1894; struct Cyc_Absyn_Conref*
_temp1896; void* _temp1898; void* _temp1900; void* _temp1902; struct Cyc_Absyn_PtrInfo
_temp1904; struct Cyc_Absyn_Conref* _temp1906; struct Cyc_Absyn_Tqual _temp1908;
struct Cyc_Absyn_Conref* _temp1910; void* _temp1912; void* _temp1914; _LL1884:
_LL1903: _temp1902= _temp1882.f1; if(( unsigned int) _temp1902 > 4u?*(( int*)
_temp1902) == Cyc_Absyn_PointerType: 0){ _LL1905: _temp1904=(( struct Cyc_Absyn_PointerType_struct*)
_temp1902)->f1; _LL1915: _temp1914=( void*) _temp1904.elt_typ; goto _LL1913;
_LL1913: _temp1912=( void*) _temp1904.rgn_typ; goto _LL1911; _LL1911: _temp1910=
_temp1904.nullable; goto _LL1909; _LL1909: _temp1908= _temp1904.tq; goto _LL1907;
_LL1907: _temp1906= _temp1904.bounds; goto _LL1889;} else{ goto _LL1886;}
_LL1889: _temp1888= _temp1882.f2; if(( unsigned int) _temp1888 > 4u?*(( int*)
_temp1888) == Cyc_Absyn_PointerType: 0){ _LL1891: _temp1890=(( struct Cyc_Absyn_PointerType_struct*)
_temp1888)->f1; _LL1901: _temp1900=( void*) _temp1890.elt_typ; goto _LL1899;
_LL1899: _temp1898=( void*) _temp1890.rgn_typ; goto _LL1897; _LL1897: _temp1896=
_temp1890.nullable; goto _LL1895; _LL1895: _temp1894= _temp1890.tq; goto _LL1893;
_LL1893: _temp1892= _temp1890.bounds; goto _LL1885;} else{ goto _LL1886;}
_LL1886: goto _LL1887; _LL1885: if( _temp1908.q_const? ! _temp1894.q_const: 0){
return 0;} if(( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1910, _temp1896)?(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1910): 0)? !(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1896): 0){ return 0;}{ void* _temp1916= _temp1912; _LL1918: if( _temp1916
==( void*) Cyc_Absyn_HeapRgn){ goto _LL1919;} else{ goto _LL1920;} _LL1920: goto
_LL1921; _LL1919: goto _LL1917; _LL1921: Cyc_Tcutil_unify( _temp1912, _temp1898);
goto _LL1917; _LL1917:;} if( !(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp,
_temp1906, _temp1892)){ struct _tuple8 _temp1923=({ struct _tuple8 _temp1922;
_temp1922.f1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1906); _temp1922.f2=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1892); _temp1922;}); void* _temp1931; void* _temp1933; struct Cyc_Absyn_Exp*
_temp1935; void* _temp1937; struct Cyc_Absyn_Exp* _temp1939; void* _temp1941;
struct Cyc_Absyn_Exp* _temp1943; _LL1925: _LL1934: _temp1933= _temp1923.f1; if((
unsigned int) _temp1933 > 1u?*(( int*) _temp1933) == Cyc_Absyn_Upper_b: 0){
_LL1936: _temp1935=(( struct Cyc_Absyn_Upper_b_struct*) _temp1933)->f1; goto
_LL1932;} else{ goto _LL1927;} _LL1932: _temp1931= _temp1923.f2; if( _temp1931
==( void*) Cyc_Absyn_Unknown_b){ goto _LL1926;} else{ goto _LL1927;} _LL1927:
_LL1942: _temp1941= _temp1923.f1; if(( unsigned int) _temp1941 > 1u?*(( int*)
_temp1941) == Cyc_Absyn_Upper_b: 0){ _LL1944: _temp1943=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1941)->f1; goto _LL1938;} else{ goto _LL1929;} _LL1938: _temp1937=
_temp1923.f2; if(( unsigned int) _temp1937 > 1u?*(( int*) _temp1937) == Cyc_Absyn_Upper_b:
0){ _LL1940: _temp1939=(( struct Cyc_Absyn_Upper_b_struct*) _temp1937)->f1; goto
_LL1928;} else{ goto _LL1929;} _LL1929: goto _LL1930; _LL1926: goto _LL1924;
_LL1928: if( Cyc_Evexp_eval_const_uint_exp( _temp1943) < Cyc_Evexp_eval_const_uint_exp(
_temp1939)){ return 0;} goto _LL1924; _LL1930: return 0; _LL1924:;} return Cyc_Tcutil_ptrsubtype(
te,({ struct Cyc_List_List* _temp1945=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1945->hd=( void*)({ struct _tuple8*
_temp1946=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp1946->f1=
t1; _temp1946->f2= t2; _temp1946;}); _temp1945->tl= assume; _temp1945;}),
_temp1914, _temp1900); _LL1887: return 0; _LL1883:;}} static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* assume, void* t1, void* t2){
struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ( te, t1); struct Cyc_List_List*
tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0; tqs2 != 0;( tqs2=(( struct Cyc_List_List*)
_check_null( tqs2))->tl, tqs1=(( struct Cyc_List_List*) _check_null( tqs1))->tl)){
if( tqs1 == 0){ return 0;}{ struct _tuple4 _temp1949; void* _temp1950; struct
Cyc_Absyn_Tqual _temp1952; struct _tuple4* _temp1947=( struct _tuple4*)(( struct
Cyc_List_List*) _check_null( tqs1))->hd; _temp1949=* _temp1947; _LL1953:
_temp1952= _temp1949.f1; goto _LL1951; _LL1951: _temp1950= _temp1949.f2; goto
_LL1948; _LL1948: { struct _tuple4 _temp1956; void* _temp1957; struct Cyc_Absyn_Tqual
_temp1959; struct _tuple4* _temp1954=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( tqs2))->hd; _temp1956=* _temp1954; _LL1960: _temp1959= _temp1956.f1;
goto _LL1958; _LL1958: _temp1957= _temp1956.f2; goto _LL1955; _LL1955: if(
_temp1959.q_const? Cyc_Tcutil_subtype( te, assume, _temp1950, _temp1957): 0){
continue;} else{ if( Cyc_Tcutil_unify( _temp1950, _temp1957)){ continue;} else{
return 0;}}}}} return 1;} int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
void* _temp1961= t2; struct Cyc_Absyn_Uniondecl** _temp1969; struct Cyc_List_List*
_temp1971; struct _tuple1* _temp1973; void* _temp1975; void* _temp1977; _LL1963:
if(( unsigned int) _temp1961 > 4u?*(( int*) _temp1961) == Cyc_Absyn_UnionType: 0){
_LL1974: _temp1973=(( struct Cyc_Absyn_UnionType_struct*) _temp1961)->f1; goto
_LL1972; _LL1972: _temp1971=(( struct Cyc_Absyn_UnionType_struct*) _temp1961)->f2;
goto _LL1970; _LL1970: _temp1969=(( struct Cyc_Absyn_UnionType_struct*)
_temp1961)->f3; goto _LL1964;} else{ goto _LL1965;} _LL1965: if(( unsigned int)
_temp1961 > 4u?*(( int*) _temp1961) == Cyc_Absyn_IntType: 0){ _LL1978: _temp1977=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1961)->f1; goto _LL1976; _LL1976:
_temp1975=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1961)->f2; if(
_temp1975 ==( void*) Cyc_Absyn_B4){ goto _LL1966;} else{ goto _LL1967;}} else{
goto _LL1967;} _LL1967: goto _LL1968; _LL1964: if( _temp1969 == 0){ goto _LL1962;}{
struct Cyc_Absyn_Uniondecl* _temp1979=*(( struct Cyc_Absyn_Uniondecl**)
_check_null( _temp1969)); goto _LL1980; _LL1980: if( _temp1979->fields == 0){
goto _LL1962;}{ struct Cyc_List_List* _temp1981=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp1979->tvs,
_temp1971); goto _LL1982; _LL1982:{ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1979->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ if( Cyc_Tcutil_unify( t1, Cyc_Tcutil_substitute(
_temp1981,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ return 1;}}} goto _LL1962;}} _LL1966: if( Cyc_Tcutil_typ_kind(
t2) ==( void*) Cyc_Absyn_BoxKind){ return 1;} goto _LL1962; _LL1968: goto
_LL1962; _LL1962:;}{ void* _temp1983= t1; struct Cyc_Absyn_PtrInfo _temp1999;
struct Cyc_Absyn_Conref* _temp2001; struct Cyc_Absyn_Tqual _temp2003; struct Cyc_Absyn_Conref*
_temp2005; void* _temp2007; void* _temp2009; struct Cyc_Absyn_Exp* _temp2011;
struct Cyc_Absyn_Tqual _temp2013; void* _temp2015; struct Cyc_Absyn_Enumdecl*
_temp2017; struct _tuple1* _temp2019; void* _temp2021; void* _temp2023; _LL1985:
if(( unsigned int) _temp1983 > 4u?*(( int*) _temp1983) == Cyc_Absyn_PointerType:
0){ _LL2000: _temp1999=(( struct Cyc_Absyn_PointerType_struct*) _temp1983)->f1;
_LL2010: _temp2009=( void*) _temp1999.elt_typ; goto _LL2008; _LL2008: _temp2007=(
void*) _temp1999.rgn_typ; goto _LL2006; _LL2006: _temp2005= _temp1999.nullable;
goto _LL2004; _LL2004: _temp2003= _temp1999.tq; goto _LL2002; _LL2002: _temp2001=
_temp1999.bounds; goto _LL1986;} else{ goto _LL1987;} _LL1987: if(( unsigned int)
_temp1983 > 4u?*(( int*) _temp1983) == Cyc_Absyn_ArrayType: 0){ _LL2016:
_temp2015=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1983)->f1; goto
_LL2014; _LL2014: _temp2013=(( struct Cyc_Absyn_ArrayType_struct*) _temp1983)->f2;
goto _LL2012; _LL2012: _temp2011=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1983)->f3; goto _LL1988;} else{ goto _LL1989;} _LL1989: if(( unsigned int)
_temp1983 > 4u?*(( int*) _temp1983) == Cyc_Absyn_EnumType: 0){ _LL2020:
_temp2019=(( struct Cyc_Absyn_EnumType_struct*) _temp1983)->f1; goto _LL2018;
_LL2018: _temp2017=(( struct Cyc_Absyn_EnumType_struct*) _temp1983)->f2; goto
_LL1990;} else{ goto _LL1991;} _LL1991: if(( unsigned int) _temp1983 > 4u?*((
int*) _temp1983) == Cyc_Absyn_IntType: 0){ _LL2024: _temp2023=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1983)->f1; goto _LL2022; _LL2022: _temp2021=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1983)->f2; goto _LL1992;} else{
goto _LL1993;} _LL1993: if( _temp1983 ==( void*) Cyc_Absyn_FloatType){ goto
_LL1994;} else{ goto _LL1995;} _LL1995: if( _temp1983 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1996;} else{ goto _LL1997;} _LL1997: goto _LL1998; _LL1986:{ void*
_temp2025= t2; struct Cyc_Absyn_PtrInfo _temp2031; struct Cyc_Absyn_Conref*
_temp2033; struct Cyc_Absyn_Tqual _temp2035; struct Cyc_Absyn_Conref* _temp2037;
void* _temp2039; void* _temp2041; _LL2027: if(( unsigned int) _temp2025 > 4u?*((
int*) _temp2025) == Cyc_Absyn_PointerType: 0){ _LL2032: _temp2031=(( struct Cyc_Absyn_PointerType_struct*)
_temp2025)->f1; _LL2042: _temp2041=( void*) _temp2031.elt_typ; goto _LL2040;
_LL2040: _temp2039=( void*) _temp2031.rgn_typ; goto _LL2038; _LL2038: _temp2037=
_temp2031.nullable; goto _LL2036; _LL2036: _temp2035= _temp2031.tq; goto _LL2034;
_LL2034: _temp2033= _temp2031.bounds; goto _LL2028;} else{ goto _LL2029;}
_LL2029: goto _LL2030; _LL2028: { struct Cyc_List_List* _temp2045=({ struct Cyc_List_List*
_temp2043=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2043->hd=( void*)({ struct _tuple8* _temp2044=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp2044->f1= t1; _temp2044->f2= t2; _temp2044;});
_temp2043->tl= 0; _temp2043;}); goto _LL2046; _LL2046: { int _temp2047= Cyc_Tcutil_ptrsubtype(
te, _temp2045, _temp2009, _temp2041)? ! _temp2003.q_const? 1: _temp2035.q_const:
0; goto _LL2048; _LL2048: { int bounds_ok=(( int(*)( int(* cmp)( void*, void*),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp2001, _temp2033); if( ! bounds_ok){ struct _tuple8
_temp2050=({ struct _tuple8 _temp2049; _temp2049.f1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp2001); _temp2049.f2=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp2033); _temp2049;}); void* _temp2056; struct Cyc_Absyn_Exp*
_temp2058; void* _temp2060; struct Cyc_Absyn_Exp* _temp2062; _LL2052: _LL2061:
_temp2060= _temp2050.f1; if(( unsigned int) _temp2060 > 1u?*(( int*) _temp2060)
== Cyc_Absyn_Upper_b: 0){ _LL2063: _temp2062=(( struct Cyc_Absyn_Upper_b_struct*)
_temp2060)->f1; goto _LL2057;} else{ goto _LL2054;} _LL2057: _temp2056=
_temp2050.f2; if(( unsigned int) _temp2056 > 1u?*(( int*) _temp2056) == Cyc_Absyn_Upper_b:
0){ _LL2059: _temp2058=(( struct Cyc_Absyn_Upper_b_struct*) _temp2056)->f1; goto
_LL2053;} else{ goto _LL2054;} _LL2054: goto _LL2055; _LL2053: if( Cyc_Evexp_eval_const_uint_exp(
_temp2062) >= Cyc_Evexp_eval_const_uint_exp( _temp2058)){ bounds_ok= 1;} goto
_LL2051; _LL2055: bounds_ok= 1; goto _LL2051; _LL2051:;} return( bounds_ok?
_temp2047: 0)? _temp2007 ==( void*) Cyc_Absyn_HeapRgn? 1: Cyc_Tcutil_unify(
_temp2007, _temp2039): 0;}}} _LL2030: goto _LL2026; _LL2026:;} return 0; _LL1988:{
void* _temp2064= t2; struct Cyc_Absyn_Exp* _temp2070; struct Cyc_Absyn_Tqual
_temp2072; void* _temp2074; _LL2066: if(( unsigned int) _temp2064 > 4u?*(( int*)
_temp2064) == Cyc_Absyn_ArrayType: 0){ _LL2075: _temp2074=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2064)->f1; goto _LL2073; _LL2073: _temp2072=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2064)->f2; goto _LL2071; _LL2071: _temp2070=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2064)->f3; goto _LL2067;} else{ goto _LL2068;} _LL2068: goto _LL2069;
_LL2067: { int okay; okay=( _temp2011 != 0? _temp2070 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2011)) >= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2070)): 0; return( okay? Cyc_Tcutil_unify(
_temp2015, _temp2074): 0)? ! _temp2013.q_const? 1: _temp2072.q_const: 0;}
_LL2069: return 0; _LL2065:;} return 0; _LL1990:{ void* _temp2076= t2; struct
Cyc_Absyn_Enumdecl* _temp2082; struct _tuple1* _temp2084; _LL2078: if((
unsigned int) _temp2076 > 4u?*(( int*) _temp2076) == Cyc_Absyn_EnumType: 0){
_LL2085: _temp2084=(( struct Cyc_Absyn_EnumType_struct*) _temp2076)->f1; goto
_LL2083; _LL2083: _temp2082=(( struct Cyc_Absyn_EnumType_struct*) _temp2076)->f2;
goto _LL2079;} else{ goto _LL2080;} _LL2080: goto _LL2081; _LL2079: if((((
struct Cyc_Absyn_Enumdecl*) _check_null( _temp2017))->fields != 0?(( struct Cyc_Absyn_Enumdecl*)
_check_null( _temp2082))->fields != 0: 0)?(( int(*)( struct Cyc_List_List* x))
Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(((
struct Cyc_Absyn_Enumdecl*) _check_null( _temp2017))->fields))->v) >=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Enumdecl*) _check_null( _temp2082))->fields))->v):
0){ return 1;} goto _LL2077; _LL2081: goto _LL2077; _LL2077:;} goto _LL1992;
_LL1992: goto _LL1994; _LL1994: goto _LL1996; _LL1996: return Cyc_Tcutil_coerceable(
t2); _LL1998: return 0; _LL1984:;}} void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, void* t){ if( ! Cyc_Tcutil_unify(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v, t)){ struct Cyc_Absyn_Exp* _temp2086=
Cyc_Absyn_copy_exp( e); goto _LL2087; _LL2087:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Cast_e_struct* _temp2088=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2088[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2089; _temp2089.tag= Cyc_Absyn_Cast_e; _temp2089.f1=( void*) t; _temp2089.f2=
_temp2086; _temp2089;}); _temp2088;}))); e->topt=({ struct Cyc_Core_Opt*
_temp2090=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2090->v=( void*) t; _temp2090;});}} int Cyc_Tcutil_is_integral( struct Cyc_Absyn_Exp*
e){ void* _temp2091= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); struct Cyc_Absyn_Enumdecl* _temp2101; struct _tuple1*
_temp2103; void* _temp2105; void* _temp2107; struct Cyc_Core_Opt* _temp2109; int
_temp2111; struct Cyc_Core_Opt* _temp2113; struct Cyc_Core_Opt* _temp2115;
_LL2093: if(( unsigned int) _temp2091 > 4u?*(( int*) _temp2091) == Cyc_Absyn_EnumType:
0){ _LL2104: _temp2103=(( struct Cyc_Absyn_EnumType_struct*) _temp2091)->f1;
goto _LL2102; _LL2102: _temp2101=(( struct Cyc_Absyn_EnumType_struct*) _temp2091)->f2;
goto _LL2094;} else{ goto _LL2095;} _LL2095: if(( unsigned int) _temp2091 > 4u?*((
int*) _temp2091) == Cyc_Absyn_IntType: 0){ _LL2108: _temp2107=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2091)->f1; goto _LL2106; _LL2106: _temp2105=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2091)->f2; goto _LL2096;} else{
goto _LL2097;} _LL2097: if(( unsigned int) _temp2091 > 4u?*(( int*) _temp2091)
== Cyc_Absyn_Evar: 0){ _LL2116: _temp2115=(( struct Cyc_Absyn_Evar_struct*)
_temp2091)->f1; goto _LL2114; _LL2114: _temp2113=(( struct Cyc_Absyn_Evar_struct*)
_temp2091)->f2; goto _LL2112; _LL2112: _temp2111=(( struct Cyc_Absyn_Evar_struct*)
_temp2091)->f3; goto _LL2110; _LL2110: _temp2109=(( struct Cyc_Absyn_Evar_struct*)
_temp2091)->f4; goto _LL2098;} else{ goto _LL2099;} _LL2099: goto _LL2100;
_LL2094: goto _LL2096; _LL2096: return 1; _LL2098: if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;}
return 0; _LL2100: return 0; _LL2092:;} int Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp*
e){ if( Cyc_Tcutil_is_integral( e)){ return 1;}{ void* _temp2117= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _LL2119: if(
_temp2117 ==( void*) Cyc_Absyn_FloatType){ goto _LL2120;} else{ goto _LL2121;}
_LL2121: if( _temp2117 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2122;} else{
goto _LL2123;} _LL2123: goto _LL2124; _LL2120: goto _LL2122; _LL2122: return 1;
_LL2124: return 0; _LL2118:;}} int Cyc_Tcutil_is_function_type( void* t){ void*
_temp2125= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp2131; _LL2127:
if(( unsigned int) _temp2125 > 4u?*(( int*) _temp2125) == Cyc_Absyn_FnType: 0){
_LL2132: _temp2131=(( struct Cyc_Absyn_FnType_struct*) _temp2125)->f1; goto
_LL2128;} else{ goto _LL2129;} _LL2129: goto _LL2130; _LL2128: return 1; _LL2130:
return 0; _LL2126:;} void* Cyc_Tcutil_max_arithmetic_type( void* t1, void* t2){
struct _tuple8 _temp2134=({ struct _tuple8 _temp2133; _temp2133.f1= t1;
_temp2133.f2= t2; _temp2133;}); void* _temp2158; void* _temp2160; void*
_temp2162; void* _temp2164; void* _temp2166; void* _temp2168; void* _temp2170;
void* _temp2172; void* _temp2174; void* _temp2176; void* _temp2178; void*
_temp2180; void* _temp2182; void* _temp2184; void* _temp2186; void* _temp2188;
void* _temp2190; void* _temp2192; void* _temp2194; void* _temp2196; void*
_temp2198; void* _temp2200; void* _temp2202; void* _temp2204; void* _temp2206;
void* _temp2208; void* _temp2210; void* _temp2212; void* _temp2214; void*
_temp2216; void* _temp2218; void* _temp2220; _LL2136: _LL2161: _temp2160=
_temp2134.f1; if( _temp2160 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2159;}
else{ goto _LL2138;} _LL2159: _temp2158= _temp2134.f2; goto _LL2137; _LL2138:
_LL2165: _temp2164= _temp2134.f1; goto _LL2163; _LL2163: _temp2162= _temp2134.f2;
if( _temp2162 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2139;} else{ goto
_LL2140;} _LL2140: _LL2169: _temp2168= _temp2134.f1; if( _temp2168 ==( void*)
Cyc_Absyn_FloatType){ goto _LL2167;} else{ goto _LL2142;} _LL2167: _temp2166=
_temp2134.f2; goto _LL2141; _LL2142: _LL2173: _temp2172= _temp2134.f1; goto
_LL2171; _LL2171: _temp2170= _temp2134.f2; if( _temp2170 ==( void*) Cyc_Absyn_FloatType){
goto _LL2143;} else{ goto _LL2144;} _LL2144: _LL2177: _temp2176= _temp2134.f1;
if(( unsigned int) _temp2176 > 4u?*(( int*) _temp2176) == Cyc_Absyn_IntType: 0){
_LL2181: _temp2180=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2176)->f1;
if( _temp2180 ==( void*) Cyc_Absyn_Unsigned){ goto _LL2179;} else{ goto _LL2146;}
_LL2179: _temp2178=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2176)->f2;
if( _temp2178 ==( void*) Cyc_Absyn_B8){ goto _LL2175;} else{ goto _LL2146;}}
else{ goto _LL2146;} _LL2175: _temp2174= _temp2134.f2; goto _LL2145; _LL2146:
_LL2189: _temp2188= _temp2134.f1; goto _LL2183; _LL2183: _temp2182= _temp2134.f2;
if(( unsigned int) _temp2182 > 4u?*(( int*) _temp2182) == Cyc_Absyn_IntType: 0){
_LL2187: _temp2186=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2182)->f1;
if( _temp2186 ==( void*) Cyc_Absyn_Unsigned){ goto _LL2185;} else{ goto _LL2148;}
_LL2185: _temp2184=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2182)->f2;
if( _temp2184 ==( void*) Cyc_Absyn_B8){ goto _LL2147;} else{ goto _LL2148;}}
else{ goto _LL2148;} _LL2148: _LL2193: _temp2192= _temp2134.f1; if((
unsigned int) _temp2192 > 4u?*(( int*) _temp2192) == Cyc_Absyn_IntType: 0){
_LL2197: _temp2196=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2192)->f1;
if( _temp2196 ==( void*) Cyc_Absyn_Signed){ goto _LL2195;} else{ goto _LL2150;}
_LL2195: _temp2194=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2192)->f2;
if( _temp2194 ==( void*) Cyc_Absyn_B8){ goto _LL2191;} else{ goto _LL2150;}}
else{ goto _LL2150;} _LL2191: _temp2190= _temp2134.f2; goto _LL2149; _LL2150:
_LL2205: _temp2204= _temp2134.f1; goto _LL2199; _LL2199: _temp2198= _temp2134.f2;
if(( unsigned int) _temp2198 > 4u?*(( int*) _temp2198) == Cyc_Absyn_IntType: 0){
_LL2203: _temp2202=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2198)->f1;
if( _temp2202 ==( void*) Cyc_Absyn_Signed){ goto _LL2201;} else{ goto _LL2152;}
_LL2201: _temp2200=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2198)->f2;
if( _temp2200 ==( void*) Cyc_Absyn_B8){ goto _LL2151;} else{ goto _LL2152;}}
else{ goto _LL2152;} _LL2152: _LL2209: _temp2208= _temp2134.f1; if((
unsigned int) _temp2208 > 4u?*(( int*) _temp2208) == Cyc_Absyn_IntType: 0){
_LL2213: _temp2212=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2208)->f1;
if( _temp2212 ==( void*) Cyc_Absyn_Unsigned){ goto _LL2211;} else{ goto _LL2154;}
_LL2211: _temp2210=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2208)->f2;
if( _temp2210 ==( void*) Cyc_Absyn_B4){ goto _LL2207;} else{ goto _LL2154;}}
else{ goto _LL2154;} _LL2207: _temp2206= _temp2134.f2; goto _LL2153; _LL2154:
_LL2221: _temp2220= _temp2134.f1; goto _LL2215; _LL2215: _temp2214= _temp2134.f2;
if(( unsigned int) _temp2214 > 4u?*(( int*) _temp2214) == Cyc_Absyn_IntType: 0){
_LL2219: _temp2218=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2214)->f1;
if( _temp2218 ==( void*) Cyc_Absyn_Unsigned){ goto _LL2217;} else{ goto _LL2156;}
_LL2217: _temp2216=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2214)->f2;
if( _temp2216 ==( void*) Cyc_Absyn_B4){ goto _LL2155;} else{ goto _LL2156;}}
else{ goto _LL2156;} _LL2156: goto _LL2157; _LL2137: goto _LL2139; _LL2139:
return( void*) Cyc_Absyn_DoubleType; _LL2141: goto _LL2143; _LL2143: return(
void*) Cyc_Absyn_FloatType; _LL2145: goto _LL2147; _LL2147: return Cyc_Absyn_ulong_t;
_LL2149: goto _LL2151; _LL2151: return Cyc_Absyn_slong_t; _LL2153: goto _LL2155;
_LL2155: return Cyc_Absyn_uint_t; _LL2157: return Cyc_Absyn_sint_t; _LL2135:;}
void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp* e){ void* _temp2222=(
void*) e->r; struct Cyc_Absyn_Exp* _temp2228; struct Cyc_Core_Opt* _temp2230;
struct Cyc_Absyn_Exp* _temp2232; _LL2224: if(*(( int*) _temp2222) == Cyc_Absyn_AssignOp_e){
_LL2233: _temp2232=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp2222)->f1; goto
_LL2231; _LL2231: _temp2230=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp2222)->f2;
if( _temp2230 == 0){ goto _LL2229;} else{ goto _LL2226;} _LL2229: _temp2228=((
struct Cyc_Absyn_AssignOp_e_struct*) _temp2222)->f3; goto _LL2225;} else{ goto
_LL2226;} _LL2226: goto _LL2227; _LL2225: Cyc_Tcutil_warn( e->loc,({
unsigned char* _temp2234=( unsigned char*)"assignment in test"; struct
_tagged_string _temp2235; _temp2235.curr= _temp2234; _temp2235.base= _temp2234;
_temp2235.last_plus_one= _temp2234 + 19; _temp2235;})); goto _LL2223; _LL2227:
goto _LL2223; _LL2223:;} static int Cyc_Tcutil_constrain_kinds( struct Cyc_Absyn_Conref*
c1, struct Cyc_Absyn_Conref* c2){ c1=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( c1); c2=(( struct Cyc_Absyn_Conref*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( c2); if( c1 == c2){ return 1;}
if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr){( void*)( c1->v=( void*)((
void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2236=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2236[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2237; _temp2237.tag= Cyc_Absyn_Forward_constr;
_temp2237.f1= c2; _temp2237;}); _temp2236;}))); return 1;} else{ if(( void*) c2->v
==( void*) Cyc_Absyn_No_constr){( void*)( c2->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp2238=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp2238[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp2239; _temp2239.tag= Cyc_Absyn_Forward_constr; _temp2239.f1= c1; _temp2239;});
_temp2238;}))); return 1;} else{ void* k1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( c1); void* k2=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( c2); if( Cyc_Tcutil_kind_leq( k1, k2)){( void*)( c2->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2240=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2240[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2241; _temp2241.tag= Cyc_Absyn_Forward_constr;
_temp2241.f1= c1; _temp2241;}); _temp2240;}))); return 1;} else{ if( Cyc_Tcutil_kind_leq(
k2, k1)){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp2242=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp2242[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp2243; _temp2243.tag= Cyc_Absyn_Forward_constr; _temp2243.f1= c2; _temp2243;});
_temp2242;}))); return 1;} else{ return 0;}}}}} static int Cyc_Tcutil_tvar_id_counter=
0; int* Cyc_Tcutil_new_tvar_id(){ return({ int* _temp2244=( int*)
GC_malloc_atomic( sizeof( int)); _temp2244[ 0]= Cyc_Tcutil_tvar_id_counter ++;
_temp2244;});} static int Cyc_Tcutil_tvar_counter= 0; struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar(
void* k){ int i= Cyc_Tcutil_tvar_counter ++; struct _tagged_string s=( struct
_tagged_string) xprintf("#%d", i); return({ struct Cyc_Absyn_Tvar* _temp2245=(
struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2245->name=({
struct _tagged_string* _temp2246=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp2246[ 0]= s; _temp2246;}); _temp2245->identity=
0; _temp2245->kind=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
k); _temp2245;});} int Cyc_Tcutil_is_temp_tvar( struct Cyc_Absyn_Tvar* t){
struct _tagged_string _temp2247=* t->name; goto _LL2248; _LL2248: return*((
const unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( _temp2247, sizeof( unsigned char), 0) =='#';} void
Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar* t){ if( ! Cyc_Tcutil_is_temp_tvar(
t)){ return;}{ struct _tagged_string _temp2251= Cyc_String_strconcat(({
unsigned char* _temp2249=( unsigned char*)"`"; struct _tagged_string _temp2250;
_temp2250.curr= _temp2249; _temp2250.base= _temp2249; _temp2250.last_plus_one=
_temp2249 + 2; _temp2250;}),* t->name); goto _LL2252; _LL2252:*(( unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
_temp2251, sizeof( unsigned char), 1)='t'; t->name=({ struct _tagged_string*
_temp2253=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2253[ 0]=( struct _tagged_string) _temp2251; _temp2253;});}} struct
_tuple13{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ;
static struct _tuple2* Cyc_Tcutil_fndecl2typ_f( struct _tuple13* x){ return({
struct _tuple2* _temp2254=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2254->f1=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp2255=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2255->v=( void*)(*
x).f1; _temp2255;}); _temp2254->f2=(* x).f2; _temp2254->f3=(* x).f3; _temp2254;});}
void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl* fd){ if( fd->cached_typ ==
0){ struct Cyc_List_List* _temp2256= 0; goto _LL2257; _LL2257:{ struct Cyc_List_List*
atts= fd->attributes; for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ if( Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd)){ _temp2256=({ struct Cyc_List_List* _temp2258=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2258->hd=( void*)((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd); _temp2258->tl=
_temp2256; _temp2258;});}}} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2259=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2259[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2260; _temp2260.tag= Cyc_Absyn_FnType;
_temp2260.f1=({ struct Cyc_Absyn_FnInfo _temp2261; _temp2261.tvars= fd->tvs;
_temp2261.effect= fd->effect; _temp2261.ret_typ=( void*)(( void*) fd->ret_type);
_temp2261.args=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct
_tuple13*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_fndecl2typ_f, fd->args);
_temp2261.c_varargs= fd->c_varargs; _temp2261.cyc_varargs= fd->cyc_varargs;
_temp2261.attributes= _temp2256; _temp2261;}); _temp2260;}); _temp2259;});}
return( void*)(( struct Cyc_Core_Opt*) _check_null( fd->cached_typ))->v;} struct
_tuple14{ void* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; static void* Cyc_Tcutil_fst_fdarg(
struct _tuple14* t){ return(* t).f1;} void* Cyc_Tcutil_snd_tqt( struct _tuple4*
t){ return(* t).f2;} static struct _tuple4* Cyc_Tcutil_map2_tq( struct _tuple4*
pr, void* t){ return({ struct _tuple4* _temp2262=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp2262->f1=(* pr).f1; _temp2262->f2= t; _temp2262;});}
void Cyc_Tcutil_check_fndecl_valid_type( struct Cyc_Position_Segment* loc,
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl* fd){ void* t= Cyc_Tcutil_fndecl2typ(
fd); Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);{ void* _temp2263= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_FnInfo _temp2269; struct Cyc_List_List* _temp2271; struct
Cyc_Absyn_VarargInfo* _temp2273; int _temp2275; struct Cyc_List_List* _temp2277;
void* _temp2279; struct Cyc_Core_Opt* _temp2281; struct Cyc_List_List* _temp2283;
_LL2265: if(( unsigned int) _temp2263 > 4u?*(( int*) _temp2263) == Cyc_Absyn_FnType:
0){ _LL2270: _temp2269=(( struct Cyc_Absyn_FnType_struct*) _temp2263)->f1;
_LL2284: _temp2283= _temp2269.tvars; goto _LL2282; _LL2282: _temp2281= _temp2269.effect;
goto _LL2280; _LL2280: _temp2279=( void*) _temp2269.ret_typ; goto _LL2278;
_LL2278: _temp2277= _temp2269.args; goto _LL2276; _LL2276: _temp2275= _temp2269.c_varargs;
goto _LL2274; _LL2274: _temp2273= _temp2269.cyc_varargs; goto _LL2272; _LL2272:
_temp2271= _temp2269.attributes; goto _LL2266;} else{ goto _LL2267;} _LL2267:
goto _LL2268; _LL2266: fd->tvs= _temp2283; fd->effect= _temp2281; goto _LL2264;
_LL2268:(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
unsigned char* _temp2285=( unsigned char*)"check_fndecl_valid_type: not a FnType";
struct _tagged_string _temp2286; _temp2286.curr= _temp2285; _temp2286.base=
_temp2285; _temp2286.last_plus_one= _temp2285 + 38; _temp2286;})); return;
_LL2264:;}{ struct _RegionHandle _temp2287= _new_region(); struct _RegionHandle*
r=& _temp2287; _push_region( r); Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tagged_string*(* f)( struct _tuple13*), struct
Cyc_List_List* x)) Cyc_List_rmap)( r,( struct _tagged_string*(*)( struct
_tuple13* t)) Cyc_Tcutil_fst_fdarg, fd->args), loc,({ unsigned char* _temp2288=(
unsigned char*)"function declaration has repeated parameter"; struct
_tagged_string _temp2289; _temp2289.curr= _temp2288; _temp2289.base= _temp2288;
_temp2289.last_plus_one= _temp2288 + 44; _temp2289;}));; _pop_region(& _temp2287);}
fd->cached_typ=({ struct Cyc_Core_Opt* _temp2290=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2290->v=( void*) t; _temp2290;});}
struct _tuple15{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; } ; struct
_tuple16{ struct _tuple15* f1; void* f2; } ; static struct _tuple16* Cyc_Tcutil_substitute_f1(
struct _RegionHandle* rgn, struct _tuple2* y){ return({ struct _tuple16*
_temp2291=( struct _tuple16*) _region_malloc( rgn, sizeof( struct _tuple16));
_temp2291->f1=({ struct _tuple15* _temp2292=( struct _tuple15*) _region_malloc(
rgn, sizeof( struct _tuple15)); _temp2292->f1=(* y).f1; _temp2292->f2=(* y).f2;
_temp2292;}); _temp2291->f2=(* y).f3; _temp2291;});} static struct _tuple2* Cyc_Tcutil_substitute_f2(
struct _tuple16* w){ void* _temp2295; struct _tuple15* _temp2297; struct
_tuple16 _temp2293=* w; _LL2298: _temp2297= _temp2293.f1; goto _LL2296; _LL2296:
_temp2295= _temp2293.f2; goto _LL2294; _LL2294: { struct Cyc_Absyn_Tqual
_temp2301; struct Cyc_Core_Opt* _temp2303; struct _tuple15 _temp2299=* _temp2297;
_LL2304: _temp2303= _temp2299.f1; goto _LL2302; _LL2302: _temp2301= _temp2299.f2;
goto _LL2300; _LL2300: return({ struct _tuple2* _temp2305=( struct _tuple2*)
GC_malloc( sizeof( struct _tuple2)); _temp2305->f1= _temp2303; _temp2305->f2=
_temp2301; _temp2305->f3= _temp2295; _temp2305;});}} static void* Cyc_Tcutil_field_type(
struct Cyc_Absyn_Structfield* f){ return( void*) f->type;} static struct Cyc_Absyn_Structfield*
Cyc_Tcutil_zip_field_type( struct Cyc_Absyn_Structfield* f, void* t){ return({
struct Cyc_Absyn_Structfield* _temp2306=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2306->name= f->name;
_temp2306->tq= f->tq; _temp2306->type=( void*) t; _temp2306->width= f->width;
_temp2306->attributes= f->attributes; _temp2306;});} static struct Cyc_List_List*
Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List* inst, struct
Cyc_List_List* ts); void* Cyc_Tcutil_rsubstitute( struct _RegionHandle* rgn,
struct Cyc_List_List* inst, void* t){ void* _temp2307= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp2353; struct Cyc_Absyn_Structdecl** _temp2355;
struct Cyc_List_List* _temp2357; struct _tuple1* _temp2359; struct Cyc_Absyn_Uniondecl**
_temp2361; struct Cyc_List_List* _temp2363; struct _tuple1* _temp2365; struct
Cyc_Absyn_TunionInfo _temp2367; void* _temp2369; struct Cyc_List_List* _temp2371;
void* _temp2373; struct Cyc_Absyn_TunionFieldInfo _temp2375; struct Cyc_List_List*
_temp2377; void* _temp2379; struct Cyc_Core_Opt* _temp2381; struct Cyc_List_List*
_temp2383; struct _tuple1* _temp2385; struct Cyc_Absyn_Exp* _temp2387; struct
Cyc_Absyn_Tqual _temp2389; void* _temp2391; struct Cyc_Absyn_PtrInfo _temp2393;
struct Cyc_Absyn_Conref* _temp2395; struct Cyc_Absyn_Tqual _temp2397; struct Cyc_Absyn_Conref*
_temp2399; void* _temp2401; void* _temp2403; struct Cyc_Absyn_FnInfo _temp2405;
struct Cyc_List_List* _temp2407; struct Cyc_Absyn_VarargInfo* _temp2409; int
_temp2411; struct Cyc_List_List* _temp2413; void* _temp2415; struct Cyc_Core_Opt*
_temp2417; struct Cyc_List_List* _temp2419; struct Cyc_List_List* _temp2421;
struct Cyc_List_List* _temp2423; struct Cyc_List_List* _temp2425; struct Cyc_Core_Opt*
_temp2427; int _temp2429; struct Cyc_Core_Opt* _temp2431; struct Cyc_Core_Opt*
_temp2433; void* _temp2435; struct Cyc_Absyn_Enumdecl* _temp2437; struct _tuple1*
_temp2439; void* _temp2441; void* _temp2443; void* _temp2445; struct Cyc_List_List*
_temp2447; _LL2309: if(( unsigned int) _temp2307 > 4u?*(( int*) _temp2307) ==
Cyc_Absyn_VarType: 0){ _LL2354: _temp2353=(( struct Cyc_Absyn_VarType_struct*)
_temp2307)->f1; goto _LL2310;} else{ goto _LL2311;} _LL2311: if(( unsigned int)
_temp2307 > 4u?*(( int*) _temp2307) == Cyc_Absyn_StructType: 0){ _LL2360:
_temp2359=(( struct Cyc_Absyn_StructType_struct*) _temp2307)->f1; goto _LL2358;
_LL2358: _temp2357=(( struct Cyc_Absyn_StructType_struct*) _temp2307)->f2; goto
_LL2356; _LL2356: _temp2355=(( struct Cyc_Absyn_StructType_struct*) _temp2307)->f3;
goto _LL2312;} else{ goto _LL2313;} _LL2313: if(( unsigned int) _temp2307 > 4u?*((
int*) _temp2307) == Cyc_Absyn_UnionType: 0){ _LL2366: _temp2365=(( struct Cyc_Absyn_UnionType_struct*)
_temp2307)->f1; goto _LL2364; _LL2364: _temp2363=(( struct Cyc_Absyn_UnionType_struct*)
_temp2307)->f2; goto _LL2362; _LL2362: _temp2361=(( struct Cyc_Absyn_UnionType_struct*)
_temp2307)->f3; goto _LL2314;} else{ goto _LL2315;} _LL2315: if(( unsigned int)
_temp2307 > 4u?*(( int*) _temp2307) == Cyc_Absyn_TunionType: 0){ _LL2368:
_temp2367=(( struct Cyc_Absyn_TunionType_struct*) _temp2307)->f1; _LL2374:
_temp2373=( void*) _temp2367.tunion_info; goto _LL2372; _LL2372: _temp2371=
_temp2367.targs; goto _LL2370; _LL2370: _temp2369=( void*) _temp2367.rgn; goto
_LL2316;} else{ goto _LL2317;} _LL2317: if(( unsigned int) _temp2307 > 4u?*((
int*) _temp2307) == Cyc_Absyn_TunionFieldType: 0){ _LL2376: _temp2375=(( struct
Cyc_Absyn_TunionFieldType_struct*) _temp2307)->f1; _LL2380: _temp2379=( void*)
_temp2375.field_info; goto _LL2378; _LL2378: _temp2377= _temp2375.targs; goto
_LL2318;} else{ goto _LL2319;} _LL2319: if(( unsigned int) _temp2307 > 4u?*((
int*) _temp2307) == Cyc_Absyn_TypedefType: 0){ _LL2386: _temp2385=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2307)->f1; goto _LL2384; _LL2384: _temp2383=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2307)->f2; goto _LL2382; _LL2382: _temp2381=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2307)->f3; goto _LL2320;} else{ goto _LL2321;} _LL2321: if(( unsigned int)
_temp2307 > 4u?*(( int*) _temp2307) == Cyc_Absyn_ArrayType: 0){ _LL2392:
_temp2391=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2307)->f1; goto
_LL2390; _LL2390: _temp2389=(( struct Cyc_Absyn_ArrayType_struct*) _temp2307)->f2;
goto _LL2388; _LL2388: _temp2387=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2307)->f3; goto _LL2322;} else{ goto _LL2323;} _LL2323: if(( unsigned int)
_temp2307 > 4u?*(( int*) _temp2307) == Cyc_Absyn_PointerType: 0){ _LL2394:
_temp2393=(( struct Cyc_Absyn_PointerType_struct*) _temp2307)->f1; _LL2404:
_temp2403=( void*) _temp2393.elt_typ; goto _LL2402; _LL2402: _temp2401=( void*)
_temp2393.rgn_typ; goto _LL2400; _LL2400: _temp2399= _temp2393.nullable; goto
_LL2398; _LL2398: _temp2397= _temp2393.tq; goto _LL2396; _LL2396: _temp2395=
_temp2393.bounds; goto _LL2324;} else{ goto _LL2325;} _LL2325: if(( unsigned int)
_temp2307 > 4u?*(( int*) _temp2307) == Cyc_Absyn_FnType: 0){ _LL2406: _temp2405=((
struct Cyc_Absyn_FnType_struct*) _temp2307)->f1; _LL2420: _temp2419= _temp2405.tvars;
goto _LL2418; _LL2418: _temp2417= _temp2405.effect; goto _LL2416; _LL2416:
_temp2415=( void*) _temp2405.ret_typ; goto _LL2414; _LL2414: _temp2413=
_temp2405.args; goto _LL2412; _LL2412: _temp2411= _temp2405.c_varargs; goto
_LL2410; _LL2410: _temp2409= _temp2405.cyc_varargs; goto _LL2408; _LL2408:
_temp2407= _temp2405.attributes; goto _LL2326;} else{ goto _LL2327;} _LL2327:
if(( unsigned int) _temp2307 > 4u?*(( int*) _temp2307) == Cyc_Absyn_TupleType: 0){
_LL2422: _temp2421=(( struct Cyc_Absyn_TupleType_struct*) _temp2307)->f1; goto
_LL2328;} else{ goto _LL2329;} _LL2329: if(( unsigned int) _temp2307 > 4u?*((
int*) _temp2307) == Cyc_Absyn_AnonStructType: 0){ _LL2424: _temp2423=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp2307)->f1; goto _LL2330;} else{ goto
_LL2331;} _LL2331: if(( unsigned int) _temp2307 > 4u?*(( int*) _temp2307) == Cyc_Absyn_AnonUnionType:
0){ _LL2426: _temp2425=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp2307)->f1;
goto _LL2332;} else{ goto _LL2333;} _LL2333: if(( unsigned int) _temp2307 > 4u?*((
int*) _temp2307) == Cyc_Absyn_Evar: 0){ _LL2434: _temp2433=(( struct Cyc_Absyn_Evar_struct*)
_temp2307)->f1; goto _LL2432; _LL2432: _temp2431=(( struct Cyc_Absyn_Evar_struct*)
_temp2307)->f2; goto _LL2430; _LL2430: _temp2429=(( struct Cyc_Absyn_Evar_struct*)
_temp2307)->f3; goto _LL2428; _LL2428: _temp2427=(( struct Cyc_Absyn_Evar_struct*)
_temp2307)->f4; goto _LL2334;} else{ goto _LL2335;} _LL2335: if(( unsigned int)
_temp2307 > 4u?*(( int*) _temp2307) == Cyc_Absyn_RgnHandleType: 0){ _LL2436:
_temp2435=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp2307)->f1;
goto _LL2336;} else{ goto _LL2337;} _LL2337: if(( unsigned int) _temp2307 > 4u?*((
int*) _temp2307) == Cyc_Absyn_EnumType: 0){ _LL2440: _temp2439=(( struct Cyc_Absyn_EnumType_struct*)
_temp2307)->f1; goto _LL2438; _LL2438: _temp2437=(( struct Cyc_Absyn_EnumType_struct*)
_temp2307)->f2; goto _LL2338;} else{ goto _LL2339;} _LL2339: if( _temp2307 ==(
void*) Cyc_Absyn_VoidType){ goto _LL2340;} else{ goto _LL2341;} _LL2341: if((
unsigned int) _temp2307 > 4u?*(( int*) _temp2307) == Cyc_Absyn_IntType: 0){
_LL2444: _temp2443=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2307)->f1;
goto _LL2442; _LL2442: _temp2441=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2307)->f2; goto _LL2342;} else{ goto _LL2343;} _LL2343: if( _temp2307 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2344;} else{ goto _LL2345;} _LL2345: if(
_temp2307 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2346;} else{ goto _LL2347;}
_LL2347: if( _temp2307 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL2348;} else{ goto
_LL2349;} _LL2349: if(( unsigned int) _temp2307 > 4u?*(( int*) _temp2307) == Cyc_Absyn_AccessEff:
0){ _LL2446: _temp2445=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp2307)->f1;
goto _LL2350;} else{ goto _LL2351;} _LL2351: if(( unsigned int) _temp2307 > 4u?*((
int*) _temp2307) == Cyc_Absyn_JoinEff: 0){ _LL2448: _temp2447=(( struct Cyc_Absyn_JoinEff_struct*)
_temp2307)->f1; goto _LL2352;} else{ goto _LL2308;} _LL2310: { struct
_handler_cons _temp2449; _push_handler(& _temp2449);{ int _temp2451= 0; if(
setjmp( _temp2449.handler)){ _temp2451= 1;} if( ! _temp2451){{ void* _temp2452=((
void*(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp,
inst, _temp2353); _npop_handler( 0u); return _temp2452;}; _pop_handler();} else{
void* _temp2450=( void*) _exn_thrown; void* _temp2454= _temp2450; _LL2456: if(
_temp2454 == Cyc_Core_Not_found){ goto _LL2457;} else{ goto _LL2458;} _LL2458:
goto _LL2459; _LL2457: return t; _LL2459:( void) _throw( _temp2454); _LL2455:;}}}
_LL2312: { struct Cyc_List_List* _temp2460= Cyc_Tcutil_substs( rgn, inst,
_temp2357); goto _LL2461; _LL2461: return _temp2460 == _temp2357? t:( void*)({
struct Cyc_Absyn_StructType_struct* _temp2462=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp2462[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp2463; _temp2463.tag= Cyc_Absyn_StructType;
_temp2463.f1= _temp2359; _temp2463.f2= _temp2460; _temp2463.f3= _temp2355;
_temp2463;}); _temp2462;});} _LL2314: { struct Cyc_List_List* _temp2464= Cyc_Tcutil_substs(
rgn, inst, _temp2363); goto _LL2465; _LL2465: return _temp2464 == _temp2363? t:(
void*)({ struct Cyc_Absyn_UnionType_struct* _temp2466=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp2466[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp2467; _temp2467.tag= Cyc_Absyn_UnionType;
_temp2467.f1= _temp2365; _temp2467.f2= _temp2464; _temp2467.f3= _temp2361;
_temp2467;}); _temp2466;});} _LL2316: { struct Cyc_List_List* _temp2468= Cyc_Tcutil_substs(
rgn, inst, _temp2371); goto _LL2469; _LL2469: { void* _temp2470= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2369); goto _LL2471; _LL2471: return( _temp2468 == _temp2371?
_temp2470 == _temp2369: 0)? t:( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp2472=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp2472[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp2473; _temp2473.tag=
Cyc_Absyn_TunionType; _temp2473.f1=({ struct Cyc_Absyn_TunionInfo _temp2474;
_temp2474.tunion_info=( void*) _temp2373; _temp2474.targs= _temp2468; _temp2474.rgn=(
void*) _temp2470; _temp2474;}); _temp2473;}); _temp2472;});}} _LL2318: { struct
Cyc_List_List* _temp2475= Cyc_Tcutil_substs( rgn, inst, _temp2377); goto _LL2476;
_LL2476: return _temp2475 == _temp2377? t:( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp2477=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp2477[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp2478; _temp2478.tag= Cyc_Absyn_TunionFieldType; _temp2478.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2479; _temp2479.field_info=( void*) _temp2379; _temp2479.targs= _temp2475;
_temp2479;}); _temp2478;}); _temp2477;});} _LL2320: { struct Cyc_List_List*
_temp2480= Cyc_Tcutil_substs( rgn, inst, _temp2383); goto _LL2481; _LL2481:
return _temp2480 == _temp2383? t:( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp2482=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2482[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp2483; _temp2483.tag=
Cyc_Absyn_TypedefType; _temp2483.f1= _temp2385; _temp2483.f2= _temp2480;
_temp2483.f3= _temp2381; _temp2483;}); _temp2482;});} _LL2322: { void* _temp2484=
Cyc_Tcutil_rsubstitute( rgn, inst, _temp2391); goto _LL2485; _LL2485: return
_temp2484 == _temp2391? t:( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp2486=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp2486[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp2487; _temp2487.tag= Cyc_Absyn_ArrayType;
_temp2487.f1=( void*) _temp2484; _temp2487.f2= _temp2389; _temp2487.f3=
_temp2387; _temp2487;}); _temp2486;});} _LL2324: { void* _temp2488= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2403); goto _LL2489; _LL2489: { void* _temp2490= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2401); goto _LL2491; _LL2491: if( _temp2488 == _temp2403?
_temp2490 == _temp2401: 0){ return t;} return( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp2492=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp2492[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp2493; _temp2493.tag=
Cyc_Absyn_PointerType; _temp2493.f1=({ struct Cyc_Absyn_PtrInfo _temp2494;
_temp2494.elt_typ=( void*) _temp2488; _temp2494.rgn_typ=( void*) _temp2490;
_temp2494.nullable= _temp2399; _temp2494.tq= _temp2397; _temp2494.bounds=
_temp2395; _temp2494;}); _temp2493;}); _temp2492;});}} _LL2326:{ struct Cyc_List_List*
_temp2495= _temp2419; goto _LL2496; _LL2496: for( 0; _temp2495 != 0; _temp2495=((
struct Cyc_List_List*) _check_null( _temp2495))->tl){ inst=({ struct Cyc_List_List*
_temp2497=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2497->hd=( void*)({ struct _tuple5* _temp2498=( struct _tuple5*)
_region_malloc( rgn, sizeof( struct _tuple5)); _temp2498->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp2495))->hd; _temp2498->f2=( void*)({
struct Cyc_Absyn_VarType_struct* _temp2499=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2499[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2500; _temp2500.tag= Cyc_Absyn_VarType; _temp2500.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp2495))->hd;
_temp2500;}); _temp2499;}); _temp2498;}); _temp2497->tl= inst; _temp2497;});}}{
struct Cyc_List_List* _temp2503; struct Cyc_List_List* _temp2505; struct _tuple0
_temp2501=(( struct _tuple0(*)( struct _RegionHandle* r3, struct _RegionHandle*
r4, struct Cyc_List_List* x)) Cyc_List_rsplit)( rgn, rgn,(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple16*(* f)( struct _RegionHandle*, struct
_tuple2*), struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)(
rgn, Cyc_Tcutil_substitute_f1, rgn, _temp2413)); _LL2506: _temp2505= _temp2501.f1;
goto _LL2504; _LL2504: _temp2503= _temp2501.f2; goto _LL2502; _LL2502: { struct
Cyc_List_List* _temp2507= Cyc_Tcutil_substs( rgn, inst, _temp2503); goto _LL2508;
_LL2508: { struct Cyc_List_List* _temp2509=(( struct Cyc_List_List*(*)( struct
_tuple2*(* f)( struct _tuple16*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct _RegionHandle* r4,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn,
_temp2505, _temp2507)); goto _LL2510; _LL2510: { struct Cyc_Core_Opt* eff2; if(
_temp2417 == 0){ eff2= 0;} else{ void* _temp2511= Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2417))->v); goto _LL2512;
_LL2512: if( _temp2511 ==( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2417))->v){
eff2= _temp2417;} else{ eff2=({ struct Cyc_Core_Opt* _temp2513=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2513->v=( void*) _temp2511;
_temp2513;});}}{ struct Cyc_Absyn_VarargInfo* cyc_varargs2; if( _temp2409 == 0){
cyc_varargs2= 0;} else{ int _temp2516; void* _temp2518; void* _temp2520; struct
Cyc_Absyn_Tqual _temp2522; struct Cyc_Core_Opt* _temp2524; struct Cyc_Absyn_VarargInfo
_temp2514=*(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp2409)); _LL2525:
_temp2524= _temp2514.name; goto _LL2523; _LL2523: _temp2522= _temp2514.tq; goto
_LL2521; _LL2521: _temp2520=( void*) _temp2514.type; goto _LL2519; _LL2519:
_temp2518=( void*) _temp2514.rgn; goto _LL2517; _LL2517: _temp2516= _temp2514.inject;
goto _LL2515; _LL2515: { void* _temp2526= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2520); goto _LL2527; _LL2527: { void* _temp2528= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2518); goto _LL2529; _LL2529: if( _temp2526 == _temp2520?
_temp2518 == _temp2528: 0){ cyc_varargs2= _temp2409;} else{ cyc_varargs2=({
struct Cyc_Absyn_VarargInfo* _temp2530=( struct Cyc_Absyn_VarargInfo*) GC_malloc(
sizeof( struct Cyc_Absyn_VarargInfo)); _temp2530->name= _temp2524; _temp2530->tq=
_temp2522; _temp2530->type=( void*) _temp2526; _temp2530->rgn=( void*) _temp2528;
_temp2530->inject= _temp2516; _temp2530;});}}}} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2531=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2531[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2532; _temp2532.tag= Cyc_Absyn_FnType;
_temp2532.f1=({ struct Cyc_Absyn_FnInfo _temp2533; _temp2533.tvars= _temp2419;
_temp2533.effect= eff2; _temp2533.ret_typ=( void*) Cyc_Tcutil_rsubstitute( rgn,
inst, _temp2415); _temp2533.args= _temp2509; _temp2533.c_varargs= _temp2411;
_temp2533.cyc_varargs= cyc_varargs2; _temp2533.attributes= _temp2407; _temp2533;});
_temp2532;}); _temp2531;});}}}}} _LL2328: { struct Cyc_List_List* _temp2534=((
struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_snd_tqt, _temp2421);
goto _LL2535; _LL2535: { struct Cyc_List_List* _temp2536= Cyc_Tcutil_substs( rgn,
inst, _temp2534); goto _LL2537; _LL2537: if( _temp2536 == _temp2534){ return t;}{
struct Cyc_List_List* _temp2538=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)(
struct _tuple4*, void*), struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_map2)(
Cyc_Tcutil_map2_tq, _temp2421, _temp2536); goto _LL2539; _LL2539: return( void*)({
struct Cyc_Absyn_TupleType_struct* _temp2540=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp2540[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp2541; _temp2541.tag= Cyc_Absyn_TupleType;
_temp2541.f1= _temp2538; _temp2541;}); _temp2540;});}}} _LL2330: { struct Cyc_List_List*
_temp2542=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2423); goto _LL2543; _LL2543: { struct Cyc_List_List* _temp2544= Cyc_Tcutil_substs(
rgn, inst, _temp2542); goto _LL2545; _LL2545: if( _temp2544 == _temp2542){
return t;}{ struct Cyc_List_List* _temp2546=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type,
_temp2423, _temp2544); goto _LL2547; _LL2547: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp2548=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp2548[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp2549; _temp2549.tag= Cyc_Absyn_AnonStructType; _temp2549.f1= _temp2546;
_temp2549;}); _temp2548;});}}} _LL2332: { struct Cyc_List_List* _temp2550=((
struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type, _temp2425);
goto _LL2551; _LL2551: { struct Cyc_List_List* _temp2552= Cyc_Tcutil_substs( rgn,
inst, _temp2550); goto _LL2553; _LL2553: if( _temp2552 == _temp2550){ return t;}{
struct Cyc_List_List* _temp2554=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2425, _temp2552); goto
_LL2555; _LL2555: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp2556=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp2556[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp2557; _temp2557.tag= Cyc_Absyn_AnonStructType; _temp2557.f1= _temp2554;
_temp2557;}); _temp2556;});}}} _LL2334: if( _temp2431 != 0){ return Cyc_Tcutil_rsubstitute(
rgn, inst,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2431))->v);} else{
return t;} _LL2336: { void* _temp2558= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2435); goto _LL2559; _LL2559: return _temp2558 == _temp2435? t:( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp2560=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp2560[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp2561; _temp2561.tag= Cyc_Absyn_RgnHandleType;
_temp2561.f1=( void*) _temp2558; _temp2561;}); _temp2560;});} _LL2338: return t;
_LL2340: return t; _LL2342: return t; _LL2344: return t; _LL2346: return t;
_LL2348: return t; _LL2350: { void* _temp2562= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2445); goto _LL2563; _LL2563: return _temp2562 == _temp2445? t:( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2564=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2564[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2565; _temp2565.tag= Cyc_Absyn_AccessEff;
_temp2565.f1=( void*) _temp2562; _temp2565;}); _temp2564;});} _LL2352: { struct
Cyc_List_List* _temp2566= Cyc_Tcutil_substs( rgn, inst, _temp2447); goto _LL2567;
_LL2567: return _temp2566 == _temp2447? t:( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp2568=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2568[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2569; _temp2569.tag= Cyc_Absyn_JoinEff;
_temp2569.f1= _temp2566; _temp2569;}); _temp2568;});} _LL2308:;} static struct
Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List*
inst, struct Cyc_List_List* ts){ if( ts == 0){ return 0;}{ void* _temp2570=(
void*)(( struct Cyc_List_List*) _check_null( ts))->hd; goto _LL2571; _LL2571: {
struct Cyc_List_List* _temp2572=(( struct Cyc_List_List*) _check_null( ts))->tl;
goto _LL2573; _LL2573: { void* _temp2574= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2570); goto _LL2575; _LL2575: { struct Cyc_List_List* _temp2576= Cyc_Tcutil_substs(
rgn, inst, _temp2572); goto _LL2577; _LL2577: if( _temp2570 == _temp2574?
_temp2572 == _temp2576: 0){ return ts;} return( struct Cyc_List_List*)({ struct
Cyc_List_List* _temp2578=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2578->hd=( void*) _temp2574; _temp2578->tl= _temp2576; _temp2578;});}}}}}
extern void* Cyc_Tcutil_substitute( struct Cyc_List_List* inst, void* t){ return
Cyc_Tcutil_rsubstitute( Cyc_Core_heap_region, inst, t);} struct Cyc_Absyn_Exp*
Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv* te, void* t, struct Cyc_Position_Segment*
loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2628=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2628[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2629; _temp2629.tag= Cyc_Absyn_Const_e;
_temp2629.f1=( void*)(( void*) Cyc_Absyn_Null_c); _temp2629;}); _temp2628;}),
loc);{ void* _temp2579= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp2593; void* _temp2595; void* _temp2597; void* _temp2599; void* _temp2601;
_LL2581: if(( unsigned int) _temp2579 > 4u?*(( int*) _temp2579) == Cyc_Absyn_PointerType:
0){ _LL2594: _temp2593=(( struct Cyc_Absyn_PointerType_struct*) _temp2579)->f1;
goto _LL2582;} else{ goto _LL2583;} _LL2583: if(( unsigned int) _temp2579 > 4u?*((
int*) _temp2579) == Cyc_Absyn_IntType: 0){ _LL2598: _temp2597=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2579)->f1; goto _LL2596; _LL2596: _temp2595=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2579)->f2; if( _temp2595 ==(
void*) Cyc_Absyn_B1){ goto _LL2584;} else{ goto _LL2585;}} else{ goto _LL2585;}
_LL2585: if(( unsigned int) _temp2579 > 4u?*(( int*) _temp2579) == Cyc_Absyn_IntType:
0){ _LL2602: _temp2601=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2579)->f1;
goto _LL2600; _LL2600: _temp2599=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2579)->f2; goto _LL2586;} else{ goto _LL2587;} _LL2587: if( _temp2579 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2588;} else{ goto _LL2589;} _LL2589: if(
_temp2579 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2590;} else{ goto _LL2591;}
_LL2591: goto _LL2592; _LL2582: goto _LL2580; _LL2584:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2603=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2603[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2604; _temp2604.tag= Cyc_Absyn_Const_e; _temp2604.f1=(
void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp2605=( struct Cyc_Absyn_Char_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct)); _temp2605[ 0]=({ struct Cyc_Absyn_Char_c_struct
_temp2606; _temp2606.tag= Cyc_Absyn_Char_c; _temp2606.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp2606.f2='\000'; _temp2606;}); _temp2605;})); _temp2604;}); _temp2603;})));
goto _LL2580; _LL2586:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2607=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2607[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2608; _temp2608.tag= Cyc_Absyn_Const_e;
_temp2608.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp2609=(
struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp2609[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp2610; _temp2610.tag= Cyc_Absyn_Int_c;
_temp2610.f1=( void*) _temp2601; _temp2610.f2= 0; _temp2610;}); _temp2609;}));
_temp2608;}); _temp2607;}))); if( _temp2599 !=( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2611=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2611[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2612; _temp2612.tag= Cyc_Absyn_Cast_e; _temp2612.f1=( void*) t; _temp2612.f2=
e; _temp2612;}); _temp2611;}), loc);} goto _LL2580; _LL2588:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2613=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2613[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2614; _temp2614.tag= Cyc_Absyn_Const_e; _temp2614.f1=(
void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2615=( struct Cyc_Absyn_Float_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct)); _temp2615[ 0]=({ struct
Cyc_Absyn_Float_c_struct _temp2616; _temp2616.tag= Cyc_Absyn_Float_c; _temp2616.f1=({
unsigned char* _temp2617=( unsigned char*)"0.0"; struct _tagged_string _temp2618;
_temp2618.curr= _temp2617; _temp2618.base= _temp2617; _temp2618.last_plus_one=
_temp2617 + 4; _temp2618;}); _temp2616;}); _temp2615;})); _temp2614;});
_temp2613;}))); goto _LL2580; _LL2590:( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_Cast_e_struct* _temp2619=( struct Cyc_Absyn_Cast_e_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2619[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2620; _temp2620.tag= Cyc_Absyn_Cast_e; _temp2620.f1=( void*) t; _temp2620.f2=
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2621=( struct
Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2621[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2622; _temp2622.tag= Cyc_Absyn_Const_e;
_temp2622.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2623=(
struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2623[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2624; _temp2624.tag= Cyc_Absyn_Float_c;
_temp2624.f1=({ unsigned char* _temp2625=( unsigned char*)"0.0"; struct
_tagged_string _temp2626; _temp2626.curr= _temp2625; _temp2626.base= _temp2625;
_temp2626.last_plus_one= _temp2625 + 4; _temp2626;}); _temp2624;}); _temp2623;}));
_temp2622;}); _temp2621;}), loc); _temp2620;}); _temp2619;}))); goto _LL2580;
_LL2592: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string
_temp2627= Cyc_Absynpp_typ2string( t); xprintf("declaration of type %.*s requires initializer",
_temp2627.last_plus_one - _temp2627.curr, _temp2627.curr);})); goto _LL2580;
_LL2580:;} return e;} struct _tuple5* Cyc_Tcutil_make_inst_var( struct Cyc_List_List*
s, struct Cyc_Absyn_Tvar* tv){ void* k=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( tv->kind); return({ struct _tuple5* _temp2630=( struct
_tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp2630->f1= tv; _temp2630->f2=
Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2631=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2631->v=( void*) k; _temp2631;}),({
struct Cyc_Core_Opt* _temp2632=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2632->v=( void*) s; _temp2632;})); _temp2630;});} struct
_tuple5* Cyc_Tcutil_r_make_inst_var( struct _tuple6* env, struct Cyc_Absyn_Tvar*
tv){ struct _tuple6 _temp2635; struct _RegionHandle* _temp2636; struct Cyc_List_List*
_temp2638; struct _tuple6* _temp2633= env; _temp2635=* _temp2633; _LL2639:
_temp2638= _temp2635.f1; goto _LL2637; _LL2637: _temp2636= _temp2635.f2; goto
_LL2634; _LL2634: { void* k=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
tv->kind); return({ struct _tuple5* _temp2640=( struct _tuple5*) _region_malloc(
_temp2636, sizeof( struct _tuple5)); _temp2640->f1= tv; _temp2640->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp2641=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2641->v=( void*) k; _temp2641;}),({ struct Cyc_Core_Opt*
_temp2642=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2642->v=( void*) _temp2638; _temp2642;})); _temp2640;});}} static struct
Cyc_List_List* Cyc_Tcutil_add_free_tvar( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 != 0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
if( Cyc_String_zstrptrcmp((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->name, tv->name) == 0){ struct Cyc_Absyn_Conref* k1=((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->kind;
struct Cyc_Absyn_Conref* k2= tv->kind; if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string _temp2643=*
tv->name; struct _tagged_string _temp2644= Cyc_Absynpp_ckind2string( k1); struct
_tagged_string _temp2645= Cyc_Absynpp_ckind2string( k2); xprintf("type variable %.*s is used with inconsistent kinds %.*s and %.*s",
_temp2643.last_plus_one - _temp2643.curr, _temp2643.curr, _temp2644.last_plus_one
- _temp2644.curr, _temp2644.curr, _temp2645.last_plus_one - _temp2645.curr,
_temp2645.curr);}));} if( tv->identity == 0){ tv->identity=(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->identity;} else{ if(*(( int*)
_check_null( tv->identity)) !=*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->identity))){(( void(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string) xprintf("same type variable has different identity!"));}}
return tvs;}}} tv->identity= Cyc_Tcutil_new_tvar_id(); return({ struct Cyc_List_List*
_temp2646=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2646->hd=( void*) tv; _temp2646->tl= tvs; _temp2646;});} static struct Cyc_List_List*
Cyc_Tcutil_fast_add_free_tvar( struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar*
tv){ if( tv->identity == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
unsigned char* _temp2647=( unsigned char*)"fast_add_free_tvar: bad identity in tv";
struct _tagged_string _temp2648; _temp2648.curr= _temp2647; _temp2648.base=
_temp2647; _temp2648.last_plus_one= _temp2647 + 39; _temp2648;}));}{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 != 0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
if((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->identity
== 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char*
_temp2649=( unsigned char*)"fast_add_free_tvar: bad identity in tvs2"; struct
_tagged_string _temp2650; _temp2650.curr= _temp2649; _temp2650.base= _temp2649;
_temp2650.last_plus_one= _temp2649 + 41; _temp2650;}));} if(*(( int*)
_check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
tvs2))->hd)->identity)) ==*(( int*) _check_null( tv->identity))){ return tvs;}}}
return({ struct Cyc_List_List* _temp2651=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2651->hd=( void*) tv; _temp2651->tl= tvs;
_temp2651;});} static struct Cyc_List_List* Cyc_Tcutil_add_bound_tvar( struct
Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){ if( tv->identity == 0){(( void(*)(
struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({ struct
_tagged_string _temp2652= Cyc_Tcutil_tvar2string( tv); xprintf("bound tvar id for %.*s is null",
_temp2652.last_plus_one - _temp2652.curr, _temp2652.curr);}));} return({ struct
Cyc_List_List* _temp2653=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2653->hd=( void*) tv; _temp2653->tl= tvs; _temp2653;});} static struct Cyc_List_List*
Cyc_Tcutil_add_free_evar( struct Cyc_List_List* es, void* e){ void* _temp2654=
Cyc_Tcutil_compress( e); struct Cyc_Core_Opt* _temp2660; int _temp2662; struct
Cyc_Core_Opt* _temp2664; struct Cyc_Core_Opt* _temp2666; _LL2656: if((
unsigned int) _temp2654 > 4u?*(( int*) _temp2654) == Cyc_Absyn_Evar: 0){ _LL2667:
_temp2666=(( struct Cyc_Absyn_Evar_struct*) _temp2654)->f1; goto _LL2665;
_LL2665: _temp2664=(( struct Cyc_Absyn_Evar_struct*) _temp2654)->f2; goto
_LL2663; _LL2663: _temp2662=(( struct Cyc_Absyn_Evar_struct*) _temp2654)->f3;
goto _LL2661; _LL2661: _temp2660=(( struct Cyc_Absyn_Evar_struct*) _temp2654)->f4;
goto _LL2657;} else{ goto _LL2658;} _LL2658: goto _LL2659; _LL2657:{ struct Cyc_List_List*
es2= es; for( 0; es2 != 0; es2=(( struct Cyc_List_List*) _check_null( es2))->tl){
void* _temp2668= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*)
_check_null( es2))->hd); struct Cyc_Core_Opt* _temp2674; int _temp2676; struct
Cyc_Core_Opt* _temp2678; struct Cyc_Core_Opt* _temp2680; _LL2670: if((
unsigned int) _temp2668 > 4u?*(( int*) _temp2668) == Cyc_Absyn_Evar: 0){ _LL2681:
_temp2680=(( struct Cyc_Absyn_Evar_struct*) _temp2668)->f1; goto _LL2679;
_LL2679: _temp2678=(( struct Cyc_Absyn_Evar_struct*) _temp2668)->f2; goto
_LL2677; _LL2677: _temp2676=(( struct Cyc_Absyn_Evar_struct*) _temp2668)->f3;
goto _LL2675; _LL2675: _temp2674=(( struct Cyc_Absyn_Evar_struct*) _temp2668)->f4;
goto _LL2671;} else{ goto _LL2672;} _LL2672: goto _LL2673; _LL2671: if(
_temp2662 == _temp2676){ return es;} goto _LL2669; _LL2673: goto _LL2669;
_LL2669:;}} return({ struct Cyc_List_List* _temp2682=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2682->hd=( void*) e; _temp2682->tl=
es; _temp2682;}); _LL2659: return es; _LL2655:;} static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars( struct Cyc_List_List* tvs, struct Cyc_List_List*
btvs){ struct Cyc_List_List* r= 0; for( 0; tvs != 0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){ int present= 0;{ struct Cyc_List_List* b= btvs; for( 0;
b != 0; b=(( struct Cyc_List_List*) _check_null( b))->tl){ if(*(( int*)
_check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->identity))
==*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( b))->hd)->identity))){ present= 1; break;}}} if( ! present){ r=({
struct Cyc_List_List* _temp2683=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2683->hd=( void*)(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd); _temp2683->tl= r; _temp2683;});}}
r=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( r);
return r;} struct Cyc_Tcutil_CVTEnv{ struct Cyc_List_List* kind_env; struct Cyc_List_List*
free_vars; struct Cyc_List_List* free_evars; } ; static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcutil_CVTEnv cvtenv, void* expected_kind, void* t){{ void*
_temp2684= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt* _temp2730; int
_temp2732; struct Cyc_Core_Opt* _temp2734; struct Cyc_Core_Opt* _temp2736;
struct Cyc_Core_Opt** _temp2738; struct Cyc_Absyn_Tvar* _temp2739; struct Cyc_Absyn_Enumdecl*
_temp2741; struct Cyc_Absyn_Enumdecl** _temp2743; struct _tuple1* _temp2744;
struct Cyc_Absyn_TunionInfo _temp2746; void* _temp2748; struct Cyc_List_List*
_temp2750; void* _temp2752; void** _temp2754; struct Cyc_Absyn_TunionFieldInfo
_temp2755; struct Cyc_List_List* _temp2757; void* _temp2759; void** _temp2761;
struct Cyc_Absyn_PtrInfo _temp2762; struct Cyc_Absyn_Conref* _temp2764; struct
Cyc_Absyn_Tqual _temp2766; struct Cyc_Absyn_Conref* _temp2768; void* _temp2770;
void* _temp2772; void* _temp2774; void* _temp2776; struct Cyc_Absyn_Exp*
_temp2778; struct Cyc_Absyn_Tqual _temp2780; void* _temp2782; struct Cyc_Absyn_FnInfo
_temp2784; struct Cyc_List_List* _temp2786; struct Cyc_Absyn_VarargInfo*
_temp2788; int _temp2790; struct Cyc_List_List* _temp2792; void* _temp2794;
struct Cyc_Core_Opt* _temp2796; struct Cyc_Core_Opt** _temp2798; struct Cyc_List_List*
_temp2799; struct Cyc_List_List** _temp2801; struct Cyc_List_List* _temp2802;
struct Cyc_List_List* _temp2804; struct Cyc_List_List* _temp2806; struct Cyc_Absyn_Structdecl**
_temp2808; struct Cyc_Absyn_Structdecl*** _temp2810; struct Cyc_List_List*
_temp2811; struct _tuple1* _temp2813; struct Cyc_Absyn_Uniondecl** _temp2815;
struct Cyc_Absyn_Uniondecl*** _temp2817; struct Cyc_List_List* _temp2818; struct
_tuple1* _temp2820; struct Cyc_Core_Opt* _temp2822; struct Cyc_Core_Opt**
_temp2824; struct Cyc_List_List* _temp2825; struct _tuple1* _temp2827; void*
_temp2829; void* _temp2831; struct Cyc_List_List* _temp2833; _LL2686: if(
_temp2684 ==( void*) Cyc_Absyn_VoidType){ goto _LL2687;} else{ goto _LL2688;}
_LL2688: if(( unsigned int) _temp2684 > 4u?*(( int*) _temp2684) == Cyc_Absyn_Evar:
0){ _LL2737: _temp2736=(( struct Cyc_Absyn_Evar_struct*) _temp2684)->f1;
_temp2738=&(( struct Cyc_Absyn_Evar_struct*) _temp2684)->f1; goto _LL2735;
_LL2735: _temp2734=(( struct Cyc_Absyn_Evar_struct*) _temp2684)->f2; goto
_LL2733; _LL2733: _temp2732=(( struct Cyc_Absyn_Evar_struct*) _temp2684)->f3;
goto _LL2731; _LL2731: _temp2730=(( struct Cyc_Absyn_Evar_struct*) _temp2684)->f4;
goto _LL2689;} else{ goto _LL2690;} _LL2690: if(( unsigned int) _temp2684 > 4u?*((
int*) _temp2684) == Cyc_Absyn_VarType: 0){ _LL2740: _temp2739=(( struct Cyc_Absyn_VarType_struct*)
_temp2684)->f1; goto _LL2691;} else{ goto _LL2692;} _LL2692: if(( unsigned int)
_temp2684 > 4u?*(( int*) _temp2684) == Cyc_Absyn_EnumType: 0){ _LL2745:
_temp2744=(( struct Cyc_Absyn_EnumType_struct*) _temp2684)->f1; goto _LL2742;
_LL2742: _temp2741=(( struct Cyc_Absyn_EnumType_struct*) _temp2684)->f2;
_temp2743=&(( struct Cyc_Absyn_EnumType_struct*) _temp2684)->f2; goto _LL2693;}
else{ goto _LL2694;} _LL2694: if(( unsigned int) _temp2684 > 4u?*(( int*)
_temp2684) == Cyc_Absyn_TunionType: 0){ _LL2747: _temp2746=(( struct Cyc_Absyn_TunionType_struct*)
_temp2684)->f1; _LL2753: _temp2752=( void*) _temp2746.tunion_info; _temp2754=&(((
struct Cyc_Absyn_TunionType_struct*) _temp2684)->f1).tunion_info; goto _LL2751;
_LL2751: _temp2750= _temp2746.targs; goto _LL2749; _LL2749: _temp2748=( void*)
_temp2746.rgn; goto _LL2695;} else{ goto _LL2696;} _LL2696: if(( unsigned int)
_temp2684 > 4u?*(( int*) _temp2684) == Cyc_Absyn_TunionFieldType: 0){ _LL2756:
_temp2755=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2684)->f1; _LL2760:
_temp2759=( void*) _temp2755.field_info; _temp2761=&((( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2684)->f1).field_info; goto _LL2758; _LL2758: _temp2757= _temp2755.targs;
goto _LL2697;} else{ goto _LL2698;} _LL2698: if(( unsigned int) _temp2684 > 4u?*((
int*) _temp2684) == Cyc_Absyn_PointerType: 0){ _LL2763: _temp2762=(( struct Cyc_Absyn_PointerType_struct*)
_temp2684)->f1; _LL2773: _temp2772=( void*) _temp2762.elt_typ; goto _LL2771;
_LL2771: _temp2770=( void*) _temp2762.rgn_typ; goto _LL2769; _LL2769: _temp2768=
_temp2762.nullable; goto _LL2767; _LL2767: _temp2766= _temp2762.tq; goto _LL2765;
_LL2765: _temp2764= _temp2762.bounds; goto _LL2699;} else{ goto _LL2700;}
_LL2700: if(( unsigned int) _temp2684 > 4u?*(( int*) _temp2684) == Cyc_Absyn_IntType:
0){ _LL2777: _temp2776=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2684)->f1;
goto _LL2775; _LL2775: _temp2774=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2684)->f2; goto _LL2701;} else{ goto _LL2702;} _LL2702: if( _temp2684 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2703;} else{ goto _LL2704;} _LL2704: if(
_temp2684 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2705;} else{ goto _LL2706;}
_LL2706: if(( unsigned int) _temp2684 > 4u?*(( int*) _temp2684) == Cyc_Absyn_ArrayType:
0){ _LL2783: _temp2782=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2684)->f1;
goto _LL2781; _LL2781: _temp2780=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2684)->f2; goto _LL2779; _LL2779: _temp2778=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2684)->f3; goto _LL2707;} else{ goto _LL2708;} _LL2708: if(( unsigned int)
_temp2684 > 4u?*(( int*) _temp2684) == Cyc_Absyn_FnType: 0){ _LL2785: _temp2784=((
struct Cyc_Absyn_FnType_struct*) _temp2684)->f1; _LL2800: _temp2799= _temp2784.tvars;
_temp2801=&((( struct Cyc_Absyn_FnType_struct*) _temp2684)->f1).tvars; goto
_LL2797; _LL2797: _temp2796= _temp2784.effect; _temp2798=&((( struct Cyc_Absyn_FnType_struct*)
_temp2684)->f1).effect; goto _LL2795; _LL2795: _temp2794=( void*) _temp2784.ret_typ;
goto _LL2793; _LL2793: _temp2792= _temp2784.args; goto _LL2791; _LL2791:
_temp2790= _temp2784.c_varargs; goto _LL2789; _LL2789: _temp2788= _temp2784.cyc_varargs;
goto _LL2787; _LL2787: _temp2786= _temp2784.attributes; goto _LL2709;} else{
goto _LL2710;} _LL2710: if(( unsigned int) _temp2684 > 4u?*(( int*) _temp2684)
== Cyc_Absyn_TupleType: 0){ _LL2803: _temp2802=(( struct Cyc_Absyn_TupleType_struct*)
_temp2684)->f1; goto _LL2711;} else{ goto _LL2712;} _LL2712: if(( unsigned int)
_temp2684 > 4u?*(( int*) _temp2684) == Cyc_Absyn_AnonStructType: 0){ _LL2805:
_temp2804=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2684)->f1; goto
_LL2713;} else{ goto _LL2714;} _LL2714: if(( unsigned int) _temp2684 > 4u?*((
int*) _temp2684) == Cyc_Absyn_AnonUnionType: 0){ _LL2807: _temp2806=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp2684)->f1; goto _LL2715;} else{ goto
_LL2716;} _LL2716: if(( unsigned int) _temp2684 > 4u?*(( int*) _temp2684) == Cyc_Absyn_StructType:
0){ _LL2814: _temp2813=(( struct Cyc_Absyn_StructType_struct*) _temp2684)->f1;
goto _LL2812; _LL2812: _temp2811=(( struct Cyc_Absyn_StructType_struct*)
_temp2684)->f2; goto _LL2809; _LL2809: _temp2808=(( struct Cyc_Absyn_StructType_struct*)
_temp2684)->f3; _temp2810=&(( struct Cyc_Absyn_StructType_struct*) _temp2684)->f3;
goto _LL2717;} else{ goto _LL2718;} _LL2718: if(( unsigned int) _temp2684 > 4u?*((
int*) _temp2684) == Cyc_Absyn_UnionType: 0){ _LL2821: _temp2820=(( struct Cyc_Absyn_UnionType_struct*)
_temp2684)->f1; goto _LL2819; _LL2819: _temp2818=(( struct Cyc_Absyn_UnionType_struct*)
_temp2684)->f2; goto _LL2816; _LL2816: _temp2815=(( struct Cyc_Absyn_UnionType_struct*)
_temp2684)->f3; _temp2817=&(( struct Cyc_Absyn_UnionType_struct*) _temp2684)->f3;
goto _LL2719;} else{ goto _LL2720;} _LL2720: if(( unsigned int) _temp2684 > 4u?*((
int*) _temp2684) == Cyc_Absyn_TypedefType: 0){ _LL2828: _temp2827=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2684)->f1; goto _LL2826; _LL2826: _temp2825=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2684)->f2; goto _LL2823; _LL2823: _temp2822=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2684)->f3; _temp2824=&(( struct Cyc_Absyn_TypedefType_struct*) _temp2684)->f3;
goto _LL2721;} else{ goto _LL2722;} _LL2722: if( _temp2684 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL2723;} else{ goto _LL2724;} _LL2724: if(( unsigned int) _temp2684 > 4u?*((
int*) _temp2684) == Cyc_Absyn_RgnHandleType: 0){ _LL2830: _temp2829=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp2684)->f1; goto _LL2725;} else{
goto _LL2726;} _LL2726: if(( unsigned int) _temp2684 > 4u?*(( int*) _temp2684)
== Cyc_Absyn_AccessEff: 0){ _LL2832: _temp2831=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp2684)->f1; goto _LL2727;} else{ goto _LL2728;} _LL2728: if(( unsigned int)
_temp2684 > 4u?*(( int*) _temp2684) == Cyc_Absyn_JoinEff: 0){ _LL2834: _temp2833=((
struct Cyc_Absyn_JoinEff_struct*) _temp2684)->f1; goto _LL2729;} else{ goto
_LL2685;} _LL2687: goto _LL2685; _LL2689: if(* _temp2738 == 0){* _temp2738=({
struct Cyc_Core_Opt* _temp2835=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2835->v=( void*) expected_kind; _temp2835;});} cvtenv.free_evars=
Cyc_Tcutil_add_free_evar( cvtenv.free_evars, t); goto _LL2685; _LL2691: { struct
Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp2739->kind); if(( void*) c->v ==( void*) Cyc_Absyn_No_constr){(
void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2836=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2836[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2837; _temp2837.tag= Cyc_Absyn_Eq_constr;
_temp2837.f1=( void*) expected_kind; _temp2837;}); _temp2836;})));} cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar( loc, cvtenv.kind_env, _temp2739); cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars, _temp2739); goto _LL2685;}
_LL2693: if(* _temp2743 == 0){ struct _handler_cons _temp2838; _push_handler(&
_temp2838);{ int _temp2840= 0; if( setjmp( _temp2838.handler)){ _temp2840= 1;}
if( ! _temp2840){{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl( te,
loc, _temp2744);* _temp2743=( struct Cyc_Absyn_Enumdecl*)* ed;}; _pop_handler();}
else{ void* _temp2839=( void*) _exn_thrown; void* _temp2842= _temp2839; _LL2844:
if( _temp2842 == Cyc_Dict_Absent){ goto _LL2845;} else{ goto _LL2846;} _LL2846:
goto _LL2847; _LL2845: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp2848= Cyc_Absynpp_qvar2string( _temp2744); xprintf("unbound type enum %.*s",
_temp2848.last_plus_one - _temp2848.curr, _temp2848.curr);})); return cvtenv;
_LL2847:( void) _throw( _temp2842); _LL2843:;}}}{ struct Cyc_Absyn_Enumdecl* ed=(
struct Cyc_Absyn_Enumdecl*) _check_null(* _temp2743);* _temp2744=( ed->name)[
_check_known_subscript_notnull( 1u, 0)]; goto _LL2685;} _LL2695:{ void*
_temp2849=* _temp2754; struct Cyc_Absyn_UnknownTunionInfo _temp2855; int
_temp2857; struct _tuple1* _temp2859; struct Cyc_Absyn_Tuniondecl* _temp2861;
_LL2851: if(*(( int*) _temp2849) == Cyc_Absyn_UnknownTunion){ _LL2856: _temp2855=((
struct Cyc_Absyn_UnknownTunion_struct*) _temp2849)->f1; _LL2860: _temp2859=
_temp2855.name; goto _LL2858; _LL2858: _temp2857= _temp2855.is_xtunion; goto
_LL2852;} else{ goto _LL2853;} _LL2853: if(*(( int*) _temp2849) == Cyc_Absyn_KnownTunion){
_LL2862: _temp2861=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2849)->f1; goto
_LL2854;} else{ goto _LL2850;} _LL2852: { struct Cyc_Absyn_Tuniondecl* tud;{
struct _handler_cons _temp2863; _push_handler(& _temp2863);{ int _temp2865= 0;
if( setjmp( _temp2863.handler)){ _temp2865= 1;} if( ! _temp2865){ tud=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp2859);; _pop_handler();} else{ void* _temp2864=( void*)
_exn_thrown; void* _temp2867= _temp2864; _LL2869: if( _temp2867 == Cyc_Dict_Absent){
goto _LL2870;} else{ goto _LL2871;} _LL2871: goto _LL2872; _LL2870: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ struct _tagged_string _temp2873= Cyc_Absynpp_qvar2string(
_temp2859); xprintf("unbound type [x]tunion %.*s", _temp2873.last_plus_one -
_temp2873.curr, _temp2873.curr);})); return cvtenv; _LL2872:( void) _throw(
_temp2867); _LL2868:;}}} if( tud->is_xtunion != _temp2857){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp2874= Cyc_Absynpp_qvar2string(
_temp2859); xprintf("[x]tunion is different from type declaration %.*s",
_temp2874.last_plus_one - _temp2874.curr, _temp2874.curr);}));}* _temp2754=(
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp2875=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp2875[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp2876; _temp2876.tag= Cyc_Absyn_KnownTunion;
_temp2876.f1= tud; _temp2876;}); _temp2875;}); _temp2861= tud; goto _LL2854;}
_LL2854: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2748);{ struct Cyc_List_List* tvs= _temp2861->tvs; for( 0; _temp2750 != 0?
tvs != 0: 0;( _temp2750=(( struct Cyc_List_List*) _check_null( _temp2750))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2750))->hd; void* k1=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} if( _temp2750 != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp2877= Cyc_Absynpp_qvar2string(
_temp2861->name); xprintf("too many type arguments for tunion %.*s", _temp2877.last_plus_one
- _temp2877.curr, _temp2877.curr);}));} if( tvs != 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp2878= Cyc_Absynpp_qvar2string(
_temp2861->name); xprintf("too few type arguments for tunion %.*s", _temp2878.last_plus_one
- _temp2878.curr, _temp2878.curr);}));} goto _LL2850;} _LL2850:;} goto _LL2685;
_LL2697:{ void* _temp2879=* _temp2761; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp2885; int _temp2887; struct _tuple1* _temp2889; struct _tuple1* _temp2891;
struct Cyc_Absyn_Tunionfield* _temp2893; struct Cyc_Absyn_Tuniondecl* _temp2895;
_LL2881: if(*(( int*) _temp2879) == Cyc_Absyn_UnknownTunionfield){ _LL2886:
_temp2885=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp2879)->f1;
_LL2892: _temp2891= _temp2885.tunion_name; goto _LL2890; _LL2890: _temp2889=
_temp2885.field_name; goto _LL2888; _LL2888: _temp2887= _temp2885.is_xtunion;
goto _LL2882;} else{ goto _LL2883;} _LL2883: if(*(( int*) _temp2879) == Cyc_Absyn_KnownTunionfield){
_LL2896: _temp2895=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp2879)->f1;
goto _LL2894; _LL2894: _temp2893=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2879)->f2; goto _LL2884;} else{ goto _LL2880;} _LL2882: { struct Cyc_Absyn_Tuniondecl*
tud; struct Cyc_Absyn_Tunionfield* tuf;{ struct _handler_cons _temp2897;
_push_handler(& _temp2897);{ int _temp2899= 0; if( setjmp( _temp2897.handler)){
_temp2899= 1;} if( ! _temp2899){* Cyc_Tcenv_lookup_tuniondecl( te, loc,
_temp2891);; _pop_handler();} else{ void* _temp2898=( void*) _exn_thrown; void*
_temp2901= _temp2898; _LL2903: if( _temp2901 == Cyc_Dict_Absent){ goto _LL2904;}
else{ goto _LL2905;} _LL2905: goto _LL2906; _LL2904: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp2907= Cyc_Absynpp_qvar2string(
_temp2891); xprintf("unbound type tunion %.*s", _temp2907.last_plus_one -
_temp2907.curr, _temp2907.curr);})); return cvtenv; _LL2906:( void) _throw(
_temp2901); _LL2902:;}}}{ struct _handler_cons _temp2908; _push_handler(&
_temp2908);{ int _temp2910= 0; if( setjmp( _temp2908.handler)){ _temp2910= 1;}
if( ! _temp2910){{ void* _temp2911= Cyc_Tcenv_lookup_ordinary( te, loc,
_temp2889); struct Cyc_Absyn_Tunionfield* _temp2917; struct Cyc_Absyn_Tuniondecl*
_temp2919; _LL2913: if(*(( int*) _temp2911) == Cyc_Tcenv_TunionRes){ _LL2920:
_temp2919=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2911)->f1; goto _LL2918;
_LL2918: _temp2917=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2911)->f2; goto
_LL2914;} else{ goto _LL2915;} _LL2915: goto _LL2916; _LL2914: tuf= _temp2917;
tud= _temp2919; if( tud->is_xtunion != _temp2887){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp2921= Cyc_Absynpp_qvar2string(
_temp2891); xprintf("[x]tunion is different from type declaration %.*s",
_temp2921.last_plus_one - _temp2921.curr, _temp2921.curr);}));} goto _LL2912;
_LL2916: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string
_temp2922= Cyc_Absynpp_qvar2string( _temp2889); struct _tagged_string _temp2923=
Cyc_Absynpp_qvar2string( _temp2891); xprintf("unbound field %.*s in type tunion %.*s",
_temp2922.last_plus_one - _temp2922.curr, _temp2922.curr, _temp2923.last_plus_one
- _temp2923.curr, _temp2923.curr);}));{ struct Cyc_Tcutil_CVTEnv _temp2924=
cvtenv; _npop_handler( 0u); return _temp2924;} _LL2912:;}; _pop_handler();}
else{ void* _temp2909=( void*) _exn_thrown; void* _temp2926= _temp2909; _LL2928:
if( _temp2926 == Cyc_Dict_Absent){ goto _LL2929;} else{ goto _LL2930;} _LL2930:
goto _LL2931; _LL2929: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp2932= Cyc_Absynpp_qvar2string( _temp2889); struct
_tagged_string _temp2933= Cyc_Absynpp_qvar2string( _temp2891); xprintf("unbound field %.*s in type tunion %.*s",
_temp2932.last_plus_one - _temp2932.curr, _temp2932.curr, _temp2933.last_plus_one
- _temp2933.curr, _temp2933.curr);})); return cvtenv; _LL2931:( void) _throw(
_temp2926); _LL2927:;}}}* _temp2761=( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp2934=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp2934[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp2935; _temp2935.tag= Cyc_Absyn_KnownTunionfield; _temp2935.f1= tud;
_temp2935.f2= tuf; _temp2935;}); _temp2934;}); _temp2895= tud; _temp2893= tuf;
goto _LL2884;} _LL2884: { struct Cyc_List_List* tvs= _temp2895->tvs; for( 0;
_temp2757 != 0? tvs != 0: 0;( _temp2757=(( struct Cyc_List_List*) _check_null(
_temp2757))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2757))->hd; void* k1=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} if( _temp2757 != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp2936= Cyc_Absynpp_qvar2string(
_temp2895->name); struct _tagged_string _temp2937= Cyc_Absynpp_qvar2string(
_temp2893->name); xprintf("too many type arguments for tunion %.*s.%.*s",
_temp2936.last_plus_one - _temp2936.curr, _temp2936.curr, _temp2937.last_plus_one
- _temp2937.curr, _temp2937.curr);}));} if( tvs != 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp2938= Cyc_Absynpp_qvar2string(
_temp2895->name); struct _tagged_string _temp2939= Cyc_Absynpp_qvar2string(
_temp2893->name); xprintf("too few type arguments for tunion %.*s.%.*s",
_temp2938.last_plus_one - _temp2938.curr, _temp2938.curr, _temp2939.last_plus_one
- _temp2939.curr, _temp2939.curr);}));} goto _LL2880;} _LL2880:;} goto _LL2685;
_LL2699: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_AnyKind,
_temp2772); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2770);{ void* _temp2940=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp2764))->v; void* _temp2946; struct Cyc_Absyn_Exp*
_temp2948; _LL2942: if(( unsigned int) _temp2940 > 1u?*(( int*) _temp2940) ==
Cyc_Absyn_Eq_constr: 0){ _LL2947: _temp2946=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2940)->f1; if(( unsigned int) _temp2946 > 1u?*(( int*) _temp2946) == Cyc_Absyn_Upper_b:
0){ _LL2949: _temp2948=(( struct Cyc_Absyn_Upper_b_struct*) _temp2946)->f1; goto
_LL2943;} else{ goto _LL2944;}} else{ goto _LL2944;} _LL2944: goto _LL2945;
_LL2943: if( ! Cyc_Tcutil_is_const_exp( te, _temp2948)){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp2950=( unsigned char*)"pointer bounds expression is not a constant";
struct _tagged_string _temp2951; _temp2951.curr= _temp2950; _temp2951.base=
_temp2950; _temp2951.last_plus_one= _temp2950 + 44; _temp2951;}));} Cyc_Tcexp_tcExp(
te, 0, _temp2948); if( ! Cyc_Tcutil_coerce_uint_typ( te, _temp2948)){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp2952=( unsigned char*)"pointer bounds expression is not an unsigned int";
struct _tagged_string _temp2953; _temp2953.curr= _temp2952; _temp2953.base=
_temp2952; _temp2953.last_plus_one= _temp2952 + 49; _temp2953;}));} Cyc_Evexp_eval_const_uint_exp(
_temp2948); goto _LL2941; _LL2945: goto _LL2941; _LL2941:;} goto _LL2685;
_LL2701: goto _LL2685; _LL2703: goto _LL2685; _LL2705: goto _LL2685; _LL2707:
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,
_temp2782); if( _temp2778 == 0? 1: ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp2778))){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp2954=(
unsigned char*)"array bounds expression is not constant"; struct _tagged_string
_temp2955; _temp2955.curr= _temp2954; _temp2955.base= _temp2954; _temp2955.last_plus_one=
_temp2954 + 40; _temp2955;}));} Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)
_check_null( _temp2778)); if( ! Cyc_Tcutil_coerce_uint_typ( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp2778))){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp2956=(
unsigned char*)"array bounds expression is not an unsigned int"; struct
_tagged_string _temp2957; _temp2957.curr= _temp2956; _temp2957.base= _temp2956;
_temp2957.last_plus_one= _temp2956 + 47; _temp2957;}));} Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2778)); goto _LL2685; _LL2709: { int
num_convs= 0; int seen_cdecl= 0; int seen_stdcall= 0; int seen_fastcall= 0; for(
0; _temp2786 != 0; _temp2786=(( struct Cyc_List_List*) _check_null( _temp2786))->tl){
if( ! Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*) _check_null(
_temp2786))->hd)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp2958= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null( _temp2786))->hd); xprintf("bad function type attribute %.*s",
_temp2958.last_plus_one - _temp2958.curr, _temp2958.curr);}));}{ void* _temp2959=(
void*)(( struct Cyc_List_List*) _check_null( _temp2786))->hd; _LL2961: if(
_temp2959 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL2962;} else{ goto _LL2963;}
_LL2963: if( _temp2959 ==( void*) Cyc_Absyn_Cdecl_att){ goto _LL2964;} else{
goto _LL2965;} _LL2965: if( _temp2959 ==( void*) Cyc_Absyn_Fastcall_att){ goto
_LL2966;} else{ goto _LL2967;} _LL2967: goto _LL2968; _LL2962: if( !
seen_stdcall){ seen_stdcall= 1; ++ num_convs;} goto _LL2960; _LL2964: if( !
seen_cdecl){ seen_cdecl= 1; ++ num_convs;} goto _LL2960; _LL2966: if( !
seen_fastcall){ seen_fastcall= 1; ++ num_convs;} goto _LL2960; _LL2968: goto
_LL2960; _LL2960:;}} if( num_convs > 1){ Cyc_Tcutil_terr( loc,( struct
_tagged_string) xprintf("function can't have multiple calling conventions"));}
Cyc_Tcutil_check_unique_tvars( loc,* _temp2801);{ struct Cyc_List_List* b=*
_temp2801; for( 0; b != 0; b=(( struct Cyc_List_List*) _check_null( b))->tl){((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd)->identity=
Cyc_Tcutil_new_tvar_id(); cvtenv.kind_env= Cyc_Tcutil_add_bound_tvar( cvtenv.kind_env,(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd);{ void*
_temp2969=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( b))->hd)->kind))->v; void* _temp2975; _LL2971: if(( unsigned int)
_temp2969 > 1u?*(( int*) _temp2969) == Cyc_Absyn_Eq_constr: 0){ _LL2976:
_temp2975=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2969)->f1; if(
_temp2975 ==( void*) Cyc_Absyn_MemKind){ goto _LL2972;} else{ goto _LL2973;}}
else{ goto _LL2973;} _LL2973: goto _LL2974; _LL2972: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp2977=*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( b))->hd)->name; xprintf("function attempts to abstract Mem type variable %.*s",
_temp2977.last_plus_one - _temp2977.curr, _temp2977.curr);})); goto _LL2970;
_LL2974: goto _LL2970; _LL2970:;}}}{ struct Cyc_Tcutil_CVTEnv _temp2979=({
struct Cyc_Tcutil_CVTEnv _temp2978; _temp2978.kind_env= cvtenv.kind_env;
_temp2978.free_vars= 0; _temp2978.free_evars= 0; _temp2978;}); goto _LL2980;
_LL2980: _temp2979= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2979,( void*)
Cyc_Absyn_MemKind, _temp2794); for( 0; _temp2792 != 0; _temp2792=(( struct Cyc_List_List*)
_check_null( _temp2792))->tl){ _temp2979= Cyc_Tcutil_i_check_valid_type( loc, te,
_temp2979,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp2792))->hd)).f3);} if( _temp2788 != 0){ if( _temp2790){(( void(*)(
struct _tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp2981=(
unsigned char*)"both c_vararg and cyc_vararg"; struct _tagged_string _temp2982;
_temp2982.curr= _temp2981; _temp2982.base= _temp2981; _temp2982.last_plus_one=
_temp2981 + 29; _temp2982;}));}{ int _temp2985; void* _temp2987; void* _temp2989;
struct Cyc_Absyn_Tqual _temp2991; struct Cyc_Core_Opt* _temp2993; struct Cyc_Absyn_VarargInfo
_temp2983=*(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp2788)); _LL2994:
_temp2993= _temp2983.name; goto _LL2992; _LL2992: _temp2991= _temp2983.tq; goto
_LL2990; _LL2990: _temp2989=( void*) _temp2983.type; goto _LL2988; _LL2988:
_temp2987=( void*) _temp2983.rgn; goto _LL2986; _LL2986: _temp2985= _temp2983.inject;
goto _LL2984; _LL2984: _temp2979= Cyc_Tcutil_i_check_valid_type( loc, te,
_temp2979,( void*) Cyc_Absyn_MemKind, _temp2989);{ void* _temp2995= Cyc_Tcutil_compress(
_temp2987); struct Cyc_Core_Opt* _temp3005; int _temp3007; struct Cyc_Core_Opt*
_temp3009; struct Cyc_Core_Opt* _temp3011; struct Cyc_Absyn_Tvar* _temp3013;
_LL2997: if(( unsigned int) _temp2995 > 4u?*(( int*) _temp2995) == Cyc_Absyn_Evar:
0){ _LL3012: _temp3011=(( struct Cyc_Absyn_Evar_struct*) _temp2995)->f1; goto
_LL3010; _LL3010: _temp3009=(( struct Cyc_Absyn_Evar_struct*) _temp2995)->f2;
goto _LL3008; _LL3008: _temp3007=(( struct Cyc_Absyn_Evar_struct*) _temp2995)->f3;
goto _LL3006; _LL3006: _temp3005=(( struct Cyc_Absyn_Evar_struct*) _temp2995)->f4;
goto _LL2998;} else{ goto _LL2999;} _LL2999: if(( unsigned int) _temp2995 > 4u?*((
int*) _temp2995) == Cyc_Absyn_VarType: 0){ _LL3014: _temp3013=(( struct Cyc_Absyn_VarType_struct*)
_temp2995)->f1; goto _LL3000;} else{ goto _LL3001;} _LL3001: if( _temp2995 ==(
void*) Cyc_Absyn_HeapRgn){ goto _LL3002;} else{ goto _LL3003;} _LL3003: goto
_LL3004; _LL2998: { void* vr_tvar=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp3015=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3015[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3016; _temp3016.tag= Cyc_Absyn_VarType;
_temp3016.f1= Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_RgnKind); _temp3016;});
_temp3015;}); Cyc_Tcutil_unify( _temp2987, vr_tvar); goto _LL2996;} _LL3000:
goto _LL2996; _LL3002: Cyc_Tcutil_terr( loc,({ unsigned char* _temp3017=(
unsigned char*)"varargs should be in a region, not heap-allocated"; struct
_tagged_string _temp3018; _temp3018.curr= _temp3017; _temp3018.base= _temp3017;
_temp3018.last_plus_one= _temp3017 + 50; _temp3018;})); goto _LL2996; _LL3004:
goto _LL2996; _LL2996:;} _temp2979= Cyc_Tcutil_i_check_valid_type( loc, te,
_temp2979,( void*) Cyc_Absyn_RgnKind, _temp2987); if( _temp2985){ void*
_temp3019= Cyc_Tcutil_compress( _temp2989); struct Cyc_Absyn_TunionInfo
_temp3025; void* _temp3027; struct Cyc_List_List* _temp3029; void* _temp3031;
struct Cyc_Absyn_Tuniondecl* _temp3033; _LL3021: if(( unsigned int) _temp3019 >
4u?*(( int*) _temp3019) == Cyc_Absyn_TunionType: 0){ _LL3026: _temp3025=((
struct Cyc_Absyn_TunionType_struct*) _temp3019)->f1; _LL3032: _temp3031=( void*)
_temp3025.tunion_info; if(*(( int*) _temp3031) == Cyc_Absyn_KnownTunion){
_LL3034: _temp3033=(( struct Cyc_Absyn_KnownTunion_struct*) _temp3031)->f1; goto
_LL3030;} else{ goto _LL3023;} _LL3030: _temp3029= _temp3025.targs; goto _LL3028;
_LL3028: _temp3027=( void*) _temp3025.rgn; goto _LL3022;} else{ goto _LL3023;}
_LL3023: goto _LL3024; _LL3022:{ void* _temp3035= Cyc_Tcutil_compress( _temp3027);
struct Cyc_Core_Opt* _temp3045; int _temp3047; struct Cyc_Core_Opt* _temp3049;
struct Cyc_Core_Opt* _temp3051; struct Cyc_Absyn_Tvar* _temp3053; _LL3037: if((
unsigned int) _temp3035 > 4u?*(( int*) _temp3035) == Cyc_Absyn_Evar: 0){ _LL3052:
_temp3051=(( struct Cyc_Absyn_Evar_struct*) _temp3035)->f1; goto _LL3050;
_LL3050: _temp3049=(( struct Cyc_Absyn_Evar_struct*) _temp3035)->f2; goto
_LL3048; _LL3048: _temp3047=(( struct Cyc_Absyn_Evar_struct*) _temp3035)->f3;
goto _LL3046; _LL3046: _temp3045=(( struct Cyc_Absyn_Evar_struct*) _temp3035)->f4;
goto _LL3038;} else{ goto _LL3039;} _LL3039: if(( unsigned int) _temp3035 > 4u?*((
int*) _temp3035) == Cyc_Absyn_VarType: 0){ _LL3054: _temp3053=(( struct Cyc_Absyn_VarType_struct*)
_temp3035)->f1; goto _LL3040;} else{ goto _LL3041;} _LL3041: if( _temp3035 ==(
void*) Cyc_Absyn_HeapRgn){ goto _LL3042;} else{ goto _LL3043;} _LL3043: goto
_LL3044; _LL3038: Cyc_Tcutil_unify( _temp2987, _temp3027); goto _LL3036; _LL3040:
goto _LL3036; _LL3042: Cyc_Tcutil_terr( loc,({ unsigned char* _temp3055=(
unsigned char*)"injected vararg [x]tunion should be in a region, not heap-allocated";
struct _tagged_string _temp3056; _temp3056.curr= _temp3055; _temp3056.base=
_temp3055; _temp3056.last_plus_one= _temp3055 + 68; _temp3056;})); goto _LL3036;
_LL3044: goto _LL3036; _LL3036:;} goto _LL3020; _LL3024: Cyc_Tcutil_terr( loc,({
unsigned char* _temp3057=( unsigned char*)"can't inject a non-[x]tunion type";
struct _tagged_string _temp3058; _temp3058.curr= _temp3057; _temp3058.base=
_temp3057; _temp3058.last_plus_one= _temp3057 + 34; _temp3058;})); goto _LL3020;
_LL3020:;}}} if(* _temp2798 != 0){ _temp2979= Cyc_Tcutil_i_check_valid_type( loc,
te, _temp2979,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_Core_Opt*)
_check_null(* _temp2798))->v);} else{ struct Cyc_Absyn_Tvar* e= Cyc_Tcutil_new_tvar((
void*) Cyc_Absyn_EffKind); _temp2979.free_vars= Cyc_Tcutil_add_free_tvar( loc,
_temp2979.free_vars, e);{ struct Cyc_List_List* effect= 0;{ struct Cyc_List_List*
tvs= _temp2979.free_vars; for( 0; tvs != 0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){ void* _temp3059=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind))->v; void* _temp3067; void*
_temp3069; _LL3061: if(( unsigned int) _temp3059 > 1u?*(( int*) _temp3059) ==
Cyc_Absyn_Eq_constr: 0){ _LL3068: _temp3067=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3059)->f1; if( _temp3067 ==( void*) Cyc_Absyn_RgnKind){ goto _LL3062;}
else{ goto _LL3063;}} else{ goto _LL3063;} _LL3063: if(( unsigned int) _temp3059
> 1u?*(( int*) _temp3059) == Cyc_Absyn_Eq_constr: 0){ _LL3070: _temp3069=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp3059)->f1; if( _temp3069 ==( void*) Cyc_Absyn_EffKind){
goto _LL3064;} else{ goto _LL3065;}} else{ goto _LL3065;} _LL3065: goto _LL3066;
_LL3062: effect=({ struct Cyc_List_List* _temp3071=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3071->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp3072=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp3072[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp3073; _temp3073.tag= Cyc_Absyn_AccessEff;
_temp3073.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3074=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3074[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3075; _temp3075.tag= Cyc_Absyn_VarType;
_temp3075.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3075;}); _temp3074;})); _temp3073;}); _temp3072;})); _temp3071->tl= effect;
_temp3071;}); goto _LL3060; _LL3064: effect=({ struct Cyc_List_List* _temp3076=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3076->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3077=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp3077[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp3078; _temp3078.tag= Cyc_Absyn_VarType; _temp3078.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3078;}); _temp3077;})); _temp3076->tl= effect; _temp3076;}); goto _LL3060;
_LL3066: goto _LL3060; _LL3060:;}}{ struct Cyc_List_List* ts= _temp2979.free_evars;
for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){ void*
_temp3079= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);
_LL3081: if( _temp3079 ==( void*) Cyc_Absyn_RgnKind){ goto _LL3082;} else{ goto
_LL3083;} _LL3083: if( _temp3079 ==( void*) Cyc_Absyn_EffKind){ goto _LL3084;}
else{ goto _LL3085;} _LL3085: goto _LL3086; _LL3082: effect=({ struct Cyc_List_List*
_temp3087=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3087->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3088=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3088[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3089; _temp3089.tag= Cyc_Absyn_AccessEff;
_temp3089.f1=( void*)(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);
_temp3089;}); _temp3088;})); _temp3087->tl= effect; _temp3087;}); goto _LL3080;
_LL3084: effect=({ struct Cyc_List_List* _temp3090=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3090->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( ts))->hd); _temp3090->tl= effect; _temp3090;});
goto _LL3080; _LL3086: goto _LL3080; _LL3080:;}}* _temp2798=({ struct Cyc_Core_Opt*
_temp3091=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3091->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3092=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3092[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3093; _temp3093.tag= Cyc_Absyn_JoinEff;
_temp3093.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
effect); _temp3093;}); _temp3092;})); _temp3091;});}} if(* _temp2801 != 0){
struct Cyc_List_List* bs=* _temp2801; for( 0; bs != 0; bs=(( struct Cyc_List_List*)
_check_null( bs))->tl){ struct Cyc_Absyn_Conref* _temp3094=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( bs))->hd)->kind); goto _LL3095; _LL3095: {
void* _temp3096=( void*) _temp3094->v; void* _temp3104; _LL3098: if( _temp3096
==( void*) Cyc_Absyn_No_constr){ goto _LL3099;} else{ goto _LL3100;} _LL3100:
if(( unsigned int) _temp3096 > 1u?*(( int*) _temp3096) == Cyc_Absyn_Eq_constr: 0){
_LL3105: _temp3104=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3096)->f1;
if( _temp3104 ==( void*) Cyc_Absyn_MemKind){ goto _LL3101;} else{ goto _LL3102;}}
else{ goto _LL3102;} _LL3102: goto _LL3103; _LL3099: Cyc_Tcutil_warn( loc,(
struct _tagged_string)({ struct _tagged_string _temp3106=*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( bs))->hd)->name; xprintf("Type variable %.*s unconstrained, assuming boxed",
_temp3106.last_plus_one - _temp3106.curr, _temp3106.curr);})); goto _LL3101;
_LL3101:( void*)( _temp3094->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3107=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3107[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3108; _temp3108.tag= Cyc_Absyn_Eq_constr;
_temp3108.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3108;}); _temp3107;})));
goto _LL3097; _LL3103: goto _LL3097; _LL3097:;}}} cvtenv.kind_env= Cyc_Tcutil_remove_bound_tvars(
_temp2979.kind_env,* _temp2801); _temp2979.free_vars= Cyc_Tcutil_remove_bound_tvars(
_temp2979.free_vars,* _temp2801);{ struct Cyc_List_List* tvs= _temp2979.free_vars;
for( 0; tvs != 0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd);}}{ struct Cyc_List_List* evs=
_temp2979.free_evars; for( 0; evs != 0; evs=(( struct Cyc_List_List*)
_check_null( evs))->tl){ cvtenv.free_evars= Cyc_Tcutil_add_free_evar( cvtenv.free_evars,(
void*)(( struct Cyc_List_List*) _check_null( evs))->hd);}} goto _LL2685;}}
_LL2711: for( 0; _temp2802 != 0; _temp2802=(( struct Cyc_List_List*) _check_null(
_temp2802))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*)
Cyc_Absyn_MemKind,(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp2802))->hd)).f2);} goto _LL2685; _LL2713:{ struct _RegionHandle _temp3109=
_new_region(); struct _RegionHandle* sprev_rgn=& _temp3109; _push_region(
sprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for( 0; _temp2804 != 0;
_temp2804=(( struct Cyc_List_List*) _check_null( _temp2804))->tl){ struct Cyc_Absyn_Structfield
_temp3112; struct Cyc_List_List* _temp3113; struct Cyc_Core_Opt* _temp3115; void*
_temp3117; struct Cyc_Absyn_Tqual _temp3119; struct _tagged_string* _temp3121;
struct Cyc_Absyn_Structfield* _temp3110=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2804))->hd; _temp3112=* _temp3110;
_LL3122: _temp3121= _temp3112.name; goto _LL3120; _LL3120: _temp3119= _temp3112.tq;
goto _LL3118; _LL3118: _temp3117=( void*) _temp3112.type; goto _LL3116; _LL3116:
_temp3115= _temp3112.width; goto _LL3114; _LL3114: _temp3113= _temp3112.attributes;
goto _LL3111; _LL3111: if((( int(*)( int(* compare)( struct _tagged_string*,
struct _tagged_string*), struct Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields, _temp3121)){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp3123=* _temp3121; xprintf("duplicate field %.*s in struct",
_temp3123.last_plus_one - _temp3123.curr, _temp3123.curr);}));} if( Cyc_String_strcmp(*
_temp3121,({ unsigned char* _temp3124=( unsigned char*)""; struct _tagged_string
_temp3125; _temp3125.curr= _temp3124; _temp3125.base= _temp3124; _temp3125.last_plus_one=
_temp3124 + 1; _temp3125;})) != 0){ prev_fields=({ struct Cyc_List_List*
_temp3126=( struct Cyc_List_List*) _region_malloc( sprev_rgn, sizeof( struct Cyc_List_List));
_temp3126->hd=( void*) _temp3121; _temp3126->tl= prev_fields; _temp3126;});}
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,
_temp3117); if( _temp3115 != 0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp3115))->v)){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string _temp3127=*
_temp3121; xprintf("bitfield %.*s does not have constant width", _temp3127.last_plus_one
- _temp3127.curr, _temp3127.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp3115))->v);}{
void* _temp3128= Cyc_Tcutil_compress( _temp3117); void* _temp3134; void*
_temp3136; _LL3130: if(( unsigned int) _temp3128 > 4u?*(( int*) _temp3128) ==
Cyc_Absyn_IntType: 0){ _LL3137: _temp3136=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3128)->f1; goto _LL3135; _LL3135: _temp3134=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3128)->f2; goto _LL3131;} else{ goto _LL3132;} _LL3132: goto _LL3133;
_LL3131:{ void* _temp3138= _temp3134; _LL3140: if( _temp3138 ==( void*) Cyc_Absyn_B1){
goto _LL3141;} else{ goto _LL3142;} _LL3142: if( _temp3138 ==( void*) Cyc_Absyn_B2){
goto _LL3143;} else{ goto _LL3144;} _LL3144: if( _temp3138 ==( void*) Cyc_Absyn_B4){
goto _LL3145;} else{ goto _LL3146;} _LL3146: if( _temp3138 ==( void*) Cyc_Absyn_B8){
goto _LL3147;} else{ goto _LL3139;} _LL3141: if( w > 8){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp3148=( unsigned char*)"bitfield larger than type"; struct
_tagged_string _temp3149; _temp3149.curr= _temp3148; _temp3149.base= _temp3148;
_temp3149.last_plus_one= _temp3148 + 26; _temp3149;}));} goto _LL3139; _LL3143:
if( w > 16){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp3150=( unsigned char*)"bitfield larger than type";
struct _tagged_string _temp3151; _temp3151.curr= _temp3150; _temp3151.base=
_temp3150; _temp3151.last_plus_one= _temp3150 + 26; _temp3151;}));} goto _LL3139;
_LL3145: if( w > 32){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp3152=(
unsigned char*)"bitfield larger than type"; struct _tagged_string _temp3153;
_temp3153.curr= _temp3152; _temp3153.base= _temp3152; _temp3153.last_plus_one=
_temp3152 + 26; _temp3153;}));} goto _LL3139; _LL3147: if( w > 64){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp3154=( unsigned char*)"bitfield larger than type";
struct _tagged_string _temp3155; _temp3155.curr= _temp3154; _temp3155.base=
_temp3154; _temp3155.last_plus_one= _temp3154 + 26; _temp3155;}));} goto _LL3139;
_LL3139:;} goto _LL3129; _LL3133: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
struct _tagged_string _temp3156=* _temp3121; struct _tagged_string _temp3157=
Cyc_Absynpp_typ2string( _temp3117); xprintf("bitfield %.*s must have integral type but has type %.*s",
_temp3156.last_plus_one - _temp3156.curr, _temp3156.curr, _temp3157.last_plus_one
- _temp3157.curr, _temp3157.curr);})); goto _LL3129; _LL3129:;}} for( 0;
_temp3113 != 0; _temp3113=(( struct Cyc_List_List*) _check_null( _temp3113))->tl){
void* _temp3158=( void*)(( struct Cyc_List_List*) _check_null( _temp3113))->hd;
int _temp3166; _LL3160: if( _temp3158 ==( void*) Cyc_Absyn_Packed_att){ goto
_LL3161;} else{ goto _LL3162;} _LL3162: if(( unsigned int) _temp3158 > 16u?*((
int*) _temp3158) == Cyc_Absyn_Aligned_att: 0){ _LL3167: _temp3166=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp3158)->f1; goto _LL3163;} else{ goto _LL3164;} _LL3164: goto _LL3165;
_LL3161: continue; _LL3163: continue; _LL3165: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp3168= Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp3113))->hd); struct
_tagged_string _temp3169=* _temp3121; xprintf("bad attribute %.*s on struct member %.*s",
_temp3168.last_plus_one - _temp3168.curr, _temp3168.curr, _temp3169.last_plus_one
- _temp3169.curr, _temp3169.curr);})); goto _LL3159; _LL3159:;}}}; _pop_region(&
_temp3109);} goto _LL2685; _LL2715:{ struct _RegionHandle _temp3170= _new_region();
struct _RegionHandle* uprev_rgn=& _temp3170; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0; for( 0; _temp2806 != 0; _temp2806=(( struct Cyc_List_List*)
_check_null( _temp2806))->tl){ struct Cyc_Absyn_Structfield _temp3173; struct
Cyc_List_List* _temp3174; struct Cyc_Core_Opt* _temp3176; void* _temp3178;
struct Cyc_Absyn_Tqual _temp3180; struct _tagged_string* _temp3182; struct Cyc_Absyn_Structfield*
_temp3171=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2806))->hd; _temp3173=* _temp3171; _LL3183: _temp3182= _temp3173.name; goto
_LL3181; _LL3181: _temp3180= _temp3173.tq; goto _LL3179; _LL3179: _temp3178=(
void*) _temp3173.type; goto _LL3177; _LL3177: _temp3176= _temp3173.width; goto
_LL3175; _LL3175: _temp3174= _temp3173.attributes; goto _LL3172; _LL3172: if(((
int(*)( int(* compare)( struct _tagged_string*, struct _tagged_string*), struct
Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp,
prev_fields, _temp3182)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp3184=* _temp3182; xprintf("duplicate field %.*s in struct",
_temp3184.last_plus_one - _temp3184.curr, _temp3184.curr);}));} if( Cyc_String_strcmp(*
_temp3182,({ unsigned char* _temp3185=( unsigned char*)""; struct _tagged_string
_temp3186; _temp3186.curr= _temp3185; _temp3186.base= _temp3185; _temp3186.last_plus_one=
_temp3185 + 1; _temp3186;})) != 0){ prev_fields=({ struct Cyc_List_List*
_temp3187=( struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp3187->hd=( void*) _temp3182; _temp3187->tl= prev_fields; _temp3187;});}
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,
_temp3178); if( ! Cyc_Tcutil_bits_only( _temp3178)){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp3188=* _temp3182; xprintf("%.*s has a type that is possibly incompatible with other members of union",
_temp3188.last_plus_one - _temp3188.curr, _temp3188.curr);}));} if( _temp3176 !=
0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp3176))->v)){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp3189=* _temp3182; xprintf("bitfield %.*s does not have constant width",
_temp3189.last_plus_one - _temp3189.curr, _temp3189.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp3176))->v);}{
void* _temp3190= Cyc_Tcutil_compress( _temp3178); void* _temp3196; void*
_temp3198; _LL3192: if(( unsigned int) _temp3190 > 4u?*(( int*) _temp3190) ==
Cyc_Absyn_IntType: 0){ _LL3199: _temp3198=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3190)->f1; goto _LL3197; _LL3197: _temp3196=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3190)->f2; goto _LL3193;} else{ goto _LL3194;} _LL3194: goto _LL3195;
_LL3193:{ void* _temp3200= _temp3196; _LL3202: if( _temp3200 ==( void*) Cyc_Absyn_B1){
goto _LL3203;} else{ goto _LL3204;} _LL3204: if( _temp3200 ==( void*) Cyc_Absyn_B2){
goto _LL3205;} else{ goto _LL3206;} _LL3206: if( _temp3200 ==( void*) Cyc_Absyn_B4){
goto _LL3207;} else{ goto _LL3208;} _LL3208: if( _temp3200 ==( void*) Cyc_Absyn_B8){
goto _LL3209;} else{ goto _LL3201;} _LL3203: if( w > 8){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp3210=( unsigned char*)"bitfield larger than type"; struct
_tagged_string _temp3211; _temp3211.curr= _temp3210; _temp3211.base= _temp3210;
_temp3211.last_plus_one= _temp3210 + 26; _temp3211;}));} goto _LL3201; _LL3205:
if( w > 16){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp3212=( unsigned char*)"bitfield larger than type";
struct _tagged_string _temp3213; _temp3213.curr= _temp3212; _temp3213.base=
_temp3212; _temp3213.last_plus_one= _temp3212 + 26; _temp3213;}));} goto _LL3201;
_LL3207: if( w > 32){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp3214=(
unsigned char*)"bitfield larger than type"; struct _tagged_string _temp3215;
_temp3215.curr= _temp3214; _temp3215.base= _temp3214; _temp3215.last_plus_one=
_temp3214 + 26; _temp3215;}));} goto _LL3201; _LL3209: if( w > 64){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp3216=( unsigned char*)"bitfield larger than type";
struct _tagged_string _temp3217; _temp3217.curr= _temp3216; _temp3217.base=
_temp3216; _temp3217.last_plus_one= _temp3216 + 26; _temp3217;}));} goto _LL3201;
_LL3201:;} goto _LL3191; _LL3195: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
struct _tagged_string _temp3218=* _temp3182; struct _tagged_string _temp3219=
Cyc_Absynpp_typ2string( _temp3178); xprintf("bitfield %.*s must have integral type but has type %.*s",
_temp3218.last_plus_one - _temp3218.curr, _temp3218.curr, _temp3219.last_plus_one
- _temp3219.curr, _temp3219.curr);})); goto _LL3191; _LL3191:;}} for( 0;
_temp3174 != 0; _temp3174=(( struct Cyc_List_List*) _check_null( _temp3174))->tl){
void* _temp3220=( void*)(( struct Cyc_List_List*) _check_null( _temp3174))->hd;
int _temp3228; _LL3222: if( _temp3220 ==( void*) Cyc_Absyn_Packed_att){ goto
_LL3223;} else{ goto _LL3224;} _LL3224: if(( unsigned int) _temp3220 > 16u?*((
int*) _temp3220) == Cyc_Absyn_Aligned_att: 0){ _LL3229: _temp3228=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp3220)->f1; goto _LL3225;} else{ goto _LL3226;} _LL3226: goto _LL3227;
_LL3223: continue; _LL3225: continue; _LL3227: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp3230= Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp3174))->hd); struct
_tagged_string _temp3231=* _temp3182; xprintf("bad attribute %.*s on struct member %.*s",
_temp3230.last_plus_one - _temp3230.curr, _temp3230.curr, _temp3231.last_plus_one
- _temp3231.curr, _temp3231.curr);})); goto _LL3221; _LL3221:;}}}; _pop_region(&
_temp3170);} goto _LL2685; _LL2717: if( _temp2813 == 0){ if(* _temp2810 == 0){
Cyc_Tcutil_terr( loc,( struct _tagged_string) xprintf("bad anonymous struct"));
return cvtenv;}{ struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**)
_check_null(* _temp2810));;}} if(* _temp2810 == 0){ struct _handler_cons
_temp3232; _push_handler(& _temp3232);{ int _temp3234= 0; if( setjmp( _temp3232.handler)){
_temp3234= 1;} if( ! _temp3234){* _temp2810=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc,( struct _tuple1*) _check_null( _temp2813));; _pop_handler();} else{
void* _temp3233=( void*) _exn_thrown; void* _temp3236= _temp3233; _LL3238: if(
_temp3236 == Cyc_Dict_Absent){ goto _LL3239;} else{ goto _LL3240;} _LL3240: goto
_LL3241; _LL3239: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp3242= Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null(
_temp2813)); xprintf("unbound type struct %.*s", _temp3242.last_plus_one -
_temp3242.curr, _temp3242.curr);})); return cvtenv; _LL3241:( void) _throw(
_temp3236); _LL3237:;}}}{ struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**)
_check_null(* _temp2810)); if( sd->name != 0){*(( struct _tuple1*) _check_null(
_temp2813))=(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)[
_check_known_subscript_notnull( 1u, 0)];}{ struct Cyc_List_List* tvs= sd->tvs;
int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2811); if( lvs != largs){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string _temp3243=
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp2813)); int
_temp3244= lvs; int _temp3245= largs; xprintf("struct %.*s expects %d type arguments but was given %d",
_temp3243.last_plus_one - _temp3243.curr, _temp3243.curr, _temp3244, _temp3245);}));}
for( 0; _temp2811 != 0;( _temp2811=(( struct Cyc_List_List*) _check_null(
_temp2811))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2811))->hd; void* k1=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} goto _LL2685;}} _LL2719: if( _temp2820 == 0){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp3246=( unsigned char*)"missing union name"; struct
_tagged_string _temp3247; _temp3247.curr= _temp3246; _temp3247.base= _temp3246;
_temp3247.last_plus_one= _temp3246 + 19; _temp3247;})); return cvtenv;} if(*
_temp2817 == 0){ struct _handler_cons _temp3248; _push_handler(& _temp3248);{
int _temp3250= 0; if( setjmp( _temp3248.handler)){ _temp3250= 1;} if( !
_temp3250){* _temp2817=( struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl(
te, loc,( struct _tuple1*) _check_null( _temp2820));; _pop_handler();} else{
void* _temp3249=( void*) _exn_thrown; void* _temp3252= _temp3249; _LL3254: if(
_temp3252 == Cyc_Dict_Absent){ goto _LL3255;} else{ goto _LL3256;} _LL3256: goto
_LL3257; _LL3255: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp3258= Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null(
_temp2820)); xprintf("unbound type union %.*s", _temp3258.last_plus_one -
_temp3258.curr, _temp3258.curr);})); return cvtenv; _LL3257:( void) _throw(
_temp3252); _LL3253:;}}}{ struct Cyc_Absyn_Uniondecl* ud=*(( struct Cyc_Absyn_Uniondecl**)
_check_null(* _temp2817)); if( ud->name != 0){*(( struct _tuple1*) _check_null(
_temp2820))=(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( ud->name))->v)[
_check_known_subscript_notnull( 1u, 0)];}{ struct Cyc_List_List* tvs= ud->tvs;
int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2818); if( lvs != largs){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string _temp3259=
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp2820)); int
_temp3260= lvs; int _temp3261= largs; xprintf("union %.*s expects %d type arguments but was given %d",
_temp3259.last_plus_one - _temp3259.curr, _temp3259.curr, _temp3260, _temp3261);}));}
for( 0; _temp2818 != 0;( _temp2818=(( struct Cyc_List_List*) _check_null(
_temp2818))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2818))->hd; void* k1=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} goto _LL2685;}} _LL2721: { struct Cyc_Absyn_Typedefdecl*
td;{ struct _handler_cons _temp3262; _push_handler(& _temp3262);{ int _temp3264=
0; if( setjmp( _temp3262.handler)){ _temp3264= 1;} if( ! _temp3264){ td= Cyc_Tcenv_lookup_typedefdecl(
te, loc, _temp2827);; _pop_handler();} else{ void* _temp3263=( void*)
_exn_thrown; void* _temp3266= _temp3263; _LL3268: if( _temp3266 == Cyc_Dict_Absent){
goto _LL3269;} else{ goto _LL3270;} _LL3270: goto _LL3271; _LL3269: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ struct _tagged_string _temp3272= Cyc_Absynpp_qvar2string(
_temp2827); xprintf("unbound typedef name %.*s", _temp3272.last_plus_one -
_temp3272.curr, _temp3272.curr);})); return cvtenv; _LL3271:( void) _throw(
_temp3266); _LL3267:;}}} _temp2827[ _check_known_subscript_notnull( 1u, 0)]=( td->name)[
_check_known_subscript_notnull( 1u, 0)];{ struct Cyc_List_List* tvs= td->tvs;
struct Cyc_List_List* ts= _temp2825; struct Cyc_List_List* inst= 0; for( 0; ts
!= 0? tvs != 0: 0;( ts=(( struct Cyc_List_List*) _check_null( ts))->tl, tvs=((
struct Cyc_List_List*) _check_null( tvs))->tl)){ void* k=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te,
cvtenv, k,( void*)(( struct Cyc_List_List*) _check_null( ts))->hd); inst=({
struct Cyc_List_List* _temp3273=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3273->hd=( void*)({ struct _tuple5* _temp3274=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp3274->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd; _temp3274->f2=( void*)(( struct
Cyc_List_List*) _check_null( ts))->hd; _temp3274;}); _temp3273->tl= inst;
_temp3273;});} if( tvs != 0? 1: ts != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp3275= Cyc_Absynpp_qvar2string(
_temp2827); xprintf("typedef %.*s expects a different number of arguments",
_temp3275.last_plus_one - _temp3275.curr, _temp3275.curr);})); return cvtenv;}{
void* new_typ= Cyc_Tcutil_substitute( inst,( void*) td->defn);* _temp2824=({
struct Cyc_Core_Opt* _temp3276=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3276->v=( void*) new_typ; _temp3276;}); goto _LL2685;}}}
_LL2723: goto _LL2685; _LL2725: _temp2831= _temp2829; goto _LL2727; _LL2727:
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2831); goto _LL2685; _LL2729: for( 0; _temp2833 != 0; _temp2833=(( struct
Cyc_List_List*) _check_null( _temp2833))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_List_List*)
_check_null( _temp2833))->hd);} goto _LL2685; _LL2685:;} if( ! Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind( t), expected_kind)){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp3277= Cyc_Absynpp_typ2string( t);
struct _tagged_string _temp3278= Cyc_Absynpp_kind2string( Cyc_Tcutil_typ_kind( t));
struct _tagged_string _temp3279= Cyc_Absynpp_kind2string( expected_kind);
xprintf("type %.*s has kind %.*s but as used here needs kind %.*s", _temp3277.last_plus_one
- _temp3277.curr, _temp3277.curr, _temp3278.last_plus_one - _temp3278.curr,
_temp3278.curr, _temp3279.last_plus_one - _temp3279.curr, _temp3279.curr);}));}
return cvtenv;} struct Cyc_List_List* Cyc_Tcutil_check_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* kind_env, void*
expected_kind, void* t){ struct Cyc_Tcutil_CVTEnv _temp3281= Cyc_Tcutil_i_check_valid_type(
loc, te,({ struct Cyc_Tcutil_CVTEnv _temp3280; _temp3280.kind_env= kind_env;
_temp3280.free_vars= 0; _temp3280.free_evars= 0; _temp3280;}), expected_kind, t);
goto _LL3282; _LL3282:{ struct Cyc_List_List* vs= _temp3281.free_vars; for( 0;
vs != 0; vs=(( struct Cyc_List_List*) _check_null( vs))->tl){ Cyc_Tcutil_fast_add_free_tvar(
kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( vs))->hd);}}{
struct Cyc_List_List* evs= _temp3281.free_evars; for( 0; evs != 0; evs=(( struct
Cyc_List_List*) _check_null( evs))->tl){ void* _temp3283= Cyc_Tcutil_compress((
void*)(( struct Cyc_List_List*) _check_null( evs))->hd); struct Cyc_Core_Opt*
_temp3289; struct Cyc_Core_Opt** _temp3291; int _temp3292; struct Cyc_Core_Opt*
_temp3294; struct Cyc_Core_Opt* _temp3296; _LL3285: if(( unsigned int) _temp3283
> 4u?*(( int*) _temp3283) == Cyc_Absyn_Evar: 0){ _LL3297: _temp3296=(( struct
Cyc_Absyn_Evar_struct*) _temp3283)->f1; goto _LL3295; _LL3295: _temp3294=((
struct Cyc_Absyn_Evar_struct*) _temp3283)->f2; goto _LL3293; _LL3293: _temp3292=((
struct Cyc_Absyn_Evar_struct*) _temp3283)->f3; goto _LL3290; _LL3290: _temp3289=((
struct Cyc_Absyn_Evar_struct*) _temp3283)->f4; _temp3291=&(( struct Cyc_Absyn_Evar_struct*)
_temp3283)->f4; goto _LL3286;} else{ goto _LL3287;} _LL3287: goto _LL3288;
_LL3286: if(* _temp3291 == 0){* _temp3291=({ struct Cyc_Core_Opt* _temp3298=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3298->v=(
void*) kind_env; _temp3298;});} else{ struct Cyc_List_List* _temp3299=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(* _temp3291))->v; goto
_LL3300; _LL3300: { struct Cyc_List_List* _temp3301= 0; goto _LL3302; _LL3302:
for( 0; _temp3299 != 0; _temp3299=(( struct Cyc_List_List*) _check_null(
_temp3299))->tl){ if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct
Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp3299))->hd)){ _temp3301=({ struct Cyc_List_List* _temp3303=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3303->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp3299))->hd);
_temp3303->tl= _temp3301; _temp3303;});}}* _temp3291=({ struct Cyc_Core_Opt*
_temp3304=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3304->v=( void*) _temp3301; _temp3304;});}} goto _LL3284; _LL3288: goto
_LL3284; _LL3284:;}} return _temp3281.free_vars;} void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, void* t){ struct
Cyc_List_List* _temp3305= Cyc_Tcutil_check_valid_type( loc, te, 0,( void*) Cyc_Absyn_MemKind,
t); goto _LL3306; _LL3306:{ struct Cyc_List_List* x= _temp3305; for( 0; x != 0;
x=(( struct Cyc_List_List*) _check_null( x))->tl){ struct Cyc_Absyn_Conref* c=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( x))->hd)->kind);
void* _temp3307=( void*) c->v; void* _temp3315; _LL3309: if( _temp3307 ==( void*)
Cyc_Absyn_No_constr){ goto _LL3310;} else{ goto _LL3311;} _LL3311: if((
unsigned int) _temp3307 > 1u?*(( int*) _temp3307) == Cyc_Absyn_Eq_constr: 0){
_LL3316: _temp3315=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3307)->f1;
if( _temp3315 ==( void*) Cyc_Absyn_MemKind){ goto _LL3312;} else{ goto _LL3313;}}
else{ goto _LL3313;} _LL3313: goto _LL3314; _LL3310: goto _LL3312; _LL3312:(
void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3317=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3317[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3318; _temp3318.tag= Cyc_Absyn_Eq_constr;
_temp3318.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3318;}); _temp3317;})));
goto _LL3308; _LL3314: goto _LL3308; _LL3308:;}} if( _temp3305 != 0){ void*
_temp3319= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp3325; struct
Cyc_List_List* _temp3327; struct Cyc_Absyn_VarargInfo* _temp3329; int _temp3331;
struct Cyc_List_List* _temp3333; void* _temp3335; struct Cyc_Core_Opt* _temp3337;
struct Cyc_List_List* _temp3339; struct Cyc_List_List** _temp3341; _LL3321: if((
unsigned int) _temp3319 > 4u?*(( int*) _temp3319) == Cyc_Absyn_FnType: 0){
_LL3326: _temp3325=(( struct Cyc_Absyn_FnType_struct*) _temp3319)->f1; _LL3340:
_temp3339= _temp3325.tvars; _temp3341=&((( struct Cyc_Absyn_FnType_struct*)
_temp3319)->f1).tvars; goto _LL3338; _LL3338: _temp3337= _temp3325.effect; goto
_LL3336; _LL3336: _temp3335=( void*) _temp3325.ret_typ; goto _LL3334; _LL3334:
_temp3333= _temp3325.args; goto _LL3332; _LL3332: _temp3331= _temp3325.c_varargs;
goto _LL3330; _LL3330: _temp3329= _temp3325.cyc_varargs; goto _LL3328; _LL3328:
_temp3327= _temp3325.attributes; goto _LL3322;} else{ goto _LL3323;} _LL3323:
goto _LL3324; _LL3322: if(* _temp3341 == 0){* _temp3341= _temp3305;} else{ if(
_temp3305 != 0){ struct _tagged_string _temp3342=*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp3305))->hd)->name; goto _LL3343;
_LL3343: if(*(( const unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( _temp3342, sizeof( unsigned char), 0)
=='?'){ Cyc_Tcutil_terr( loc,( struct _tagged_string) xprintf("You failed to abstract over the generated type variables. \nRemove the bound type variables or else make the effects of the type explicit\nand then abstract the free type variables properly."));}
else{ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string
_temp3344= _temp3342; xprintf("unbound type variable %.*s", _temp3344.last_plus_one
- _temp3344.curr, _temp3344.curr);}));}}} goto _LL3320; _LL3324: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ struct _tagged_string _temp3345=*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp3305))->hd)->name; xprintf("unbound type variable %.*s",
_temp3345.last_plus_one - _temp3345.curr, _temp3345.curr);})); return; _LL3320:;}}
void Cyc_Tcutil_add_tvar_identity( struct Cyc_Absyn_Tvar* tv){ if( tv->identity
== 0){ tv->identity= Cyc_Tcutil_new_tvar_id();}} void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List* tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Tcutil_add_tvar_identity, tvs);}
void Cyc_Tcutil_check_type( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* bound_tvars, void* expected_kind, void* t){ struct Cyc_List_List*
_temp3346= Cyc_Tcutil_check_valid_type( loc, te, bound_tvars, expected_kind, t);
goto _LL3347; _LL3347: { struct Cyc_List_List* _temp3348= Cyc_Tcutil_remove_bound_tvars(
_temp3346, bound_tvars); goto _LL3349; _LL3349: { struct Cyc_List_List* fs=
_temp3348; for( 0; fs != 0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){
struct _tagged_string* _temp3350=(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->name; goto _LL3351; _LL3351: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp3352=* _temp3350; struct
_tagged_string _temp3353= Cyc_Absynpp_typ2string( t); xprintf("unbound type variable %.*s in type %.*s",
_temp3352.last_plus_one - _temp3352.curr, _temp3352.curr, _temp3353.last_plus_one
- _temp3353.curr, _temp3353.curr);}));}}}} static void Cyc_Tcutil_check_unique_unsorted(
int(* cmp)( void*, void*), struct Cyc_List_List* vs, struct Cyc_Position_Segment*
loc, struct _tagged_string(* a2string)( void*), struct _tagged_string msg){ for(
0; vs != 0; vs=(( struct Cyc_List_List*) _check_null( vs))->tl){ struct Cyc_List_List*
vs2=(( struct Cyc_List_List*) _check_null( vs))->tl; for( 0; vs2 != 0; vs2=((
struct Cyc_List_List*) _check_null( vs2))->tl){ if( cmp(( void*)(( struct Cyc_List_List*)
_check_null( vs))->hd,( void*)(( struct Cyc_List_List*) _check_null( vs2))->hd)
== 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string
_temp3354= msg; struct _tagged_string _temp3355= a2string(( void*)(( struct Cyc_List_List*)
_check_null( vs))->hd); xprintf("%.*s: %.*s", _temp3354.last_plus_one -
_temp3354.curr, _temp3354.curr, _temp3355.last_plus_one - _temp3355.curr,
_temp3355.curr);}));}}}} static struct _tagged_string Cyc_Tcutil_strptr2string(
struct _tagged_string* s){ return* s;} void Cyc_Tcutil_check_unique_vars( struct
Cyc_List_List* vs, struct Cyc_Position_Segment* loc, struct _tagged_string msg){((
void(*)( int(* cmp)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List*
vs, struct Cyc_Position_Segment* loc, struct _tagged_string(* a2string)( struct
_tagged_string*), struct _tagged_string msg)) Cyc_Tcutil_check_unique_unsorted)(
Cyc_String_zstrptrcmp, vs, loc, Cyc_Tcutil_strptr2string, msg);} void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* tvs){(( void(*)( int(*
cmp)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* vs,
struct Cyc_Position_Segment* loc, struct _tagged_string(* a2string)( struct Cyc_Absyn_Tvar*),
struct _tagged_string msg)) Cyc_Tcutil_check_unique_unsorted)( Cyc_Absyn_tvar_cmp,
tvs, loc, Cyc_Tcutil_tvar2string,({ unsigned char* _temp3356=( unsigned char*)"duplicate type variable";
struct _tagged_string _temp3357; _temp3357.curr= _temp3356; _temp3357.base=
_temp3356; _temp3357.last_plus_one= _temp3356 + 24; _temp3357;}));} struct
_tuple17{ struct Cyc_Absyn_Structfield* f1; int f2; } ; struct _tuple18{ struct
Cyc_List_List* f1; void* f2; } ; struct _tuple19{ struct Cyc_Absyn_Structfield*
f1; void* f2; } ; struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* sdfields){ struct Cyc_List_List* fields= 0;{ struct
Cyc_List_List* sd_fields= sdfields; for( 0; sd_fields != 0; sd_fields=(( struct
Cyc_List_List*) _check_null( sd_fields))->tl){ if( Cyc_String_strcmp(*(( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( sd_fields))->hd)->name,({
unsigned char* _temp3358=( unsigned char*)""; struct _tagged_string _temp3359;
_temp3359.curr= _temp3358; _temp3359.base= _temp3358; _temp3359.last_plus_one=
_temp3358 + 1; _temp3359;})) != 0){ fields=({ struct Cyc_List_List* _temp3360=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3360->hd=(
void*)({ struct _tuple17* _temp3361=( struct _tuple17*) GC_malloc( sizeof(
struct _tuple17)); _temp3361->f1=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( sd_fields))->hd; _temp3361->f2= 0; _temp3361;}); _temp3360->tl=
fields; _temp3360;});}}} fields=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( fields);{ struct Cyc_List_List* ans= 0; for( 0; des != 0;
des=(( struct Cyc_List_List*) _check_null( des))->tl){ struct _tuple18 _temp3364;
void* _temp3365; struct Cyc_List_List* _temp3367; struct _tuple18* _temp3362=(
struct _tuple18*)(( struct Cyc_List_List*) _check_null( des))->hd; _temp3364=*
_temp3362; _LL3368: _temp3367= _temp3364.f1; goto _LL3366; _LL3366: _temp3365=
_temp3364.f2; goto _LL3363; _LL3363: if( _temp3367 == 0){ struct Cyc_List_List*
_temp3369= fields; goto _LL3370; _LL3370: for( 0; _temp3369 != 0; _temp3369=((
struct Cyc_List_List*) _check_null( _temp3369))->tl){ if( !(*(( struct _tuple17*)((
struct Cyc_List_List*) _check_null( _temp3369))->hd)).f2){(*(( struct _tuple17*)((
struct Cyc_List_List*) _check_null( _temp3369))->hd)).f2= 1;(*(( struct _tuple18*)((
struct Cyc_List_List*) _check_null( des))->hd)).f1=({ struct Cyc_List_List*
_temp3371=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3371->hd=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp3372=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp3372[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp3373; _temp3373.tag= Cyc_Absyn_FieldName;
_temp3373.f1=((*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3369))->hd)).f1)->name; _temp3373;}); _temp3372;})); _temp3371->tl= 0;
_temp3371;}); ans=({ struct Cyc_List_List* _temp3374=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp3374->hd=( void*)({
struct _tuple19* _temp3375=( struct _tuple19*) _region_malloc( rgn, sizeof(
struct _tuple19)); _temp3375->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3369))->hd)).f1; _temp3375->f2= _temp3365; _temp3375;});
_temp3374->tl= ans; _temp3374;}); break;}} if( _temp3369 == 0){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp3376=( unsigned char*)"too many arguments to struct";
struct _tagged_string _temp3377; _temp3377.curr= _temp3376; _temp3377.base=
_temp3376; _temp3377.last_plus_one= _temp3376 + 29; _temp3377;}));}} else{ if(((
struct Cyc_List_List*) _check_null( _temp3367))->tl != 0){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp3378=( unsigned char*)"multiple designators are not supported";
struct _tagged_string _temp3379; _temp3379.curr= _temp3378; _temp3379.base=
_temp3378; _temp3379.last_plus_one= _temp3378 + 39; _temp3379;}));} else{ void*
_temp3380=( void*)(( struct Cyc_List_List*) _check_null( _temp3367))->hd; struct
Cyc_Absyn_Exp* _temp3386; struct _tagged_string* _temp3388; _LL3382: if(*(( int*)
_temp3380) == Cyc_Absyn_ArrayElement){ _LL3387: _temp3386=(( struct Cyc_Absyn_ArrayElement_struct*)
_temp3380)->f1; goto _LL3383;} else{ goto _LL3384;} _LL3384: if(*(( int*)
_temp3380) == Cyc_Absyn_FieldName){ _LL3389: _temp3388=(( struct Cyc_Absyn_FieldName_struct*)
_temp3380)->f1; goto _LL3385;} else{ goto _LL3381;} _LL3383: Cyc_Tcutil_terr(
loc,({ unsigned char* _temp3390=( unsigned char*)"array designator used in argument to struct";
struct _tagged_string _temp3391; _temp3391.curr= _temp3390; _temp3391.base=
_temp3390; _temp3391.last_plus_one= _temp3390 + 44; _temp3391;})); goto _LL3381;
_LL3385: { struct Cyc_List_List* _temp3392= fields; goto _LL3393; _LL3393: for(
0; _temp3392 != 0; _temp3392=(( struct Cyc_List_List*) _check_null( _temp3392))->tl){
if( Cyc_String_zstrptrcmp( _temp3388,((*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3392))->hd)).f1)->name) == 0){ if((*(( struct _tuple17*)((
struct Cyc_List_List*) _check_null( _temp3392))->hd)).f2){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp3394=* _temp3388; xprintf("field %.*s has already been used as an argument",
_temp3394.last_plus_one - _temp3394.curr, _temp3394.curr);}));}(*(( struct
_tuple17*)(( struct Cyc_List_List*) _check_null( _temp3392))->hd)).f2= 1; ans=({
struct Cyc_List_List* _temp3395=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp3395->hd=( void*)({ struct _tuple19*
_temp3396=( struct _tuple19*) _region_malloc( rgn, sizeof( struct _tuple19));
_temp3396->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3392))->hd)).f1; _temp3396->f2= _temp3365; _temp3396;}); _temp3395->tl= ans;
_temp3395;}); break;}} if( _temp3392 == 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp3397=* _temp3388; xprintf("bad field designator %.*s",
_temp3397.last_plus_one - _temp3397.curr, _temp3397.curr);}));} goto _LL3381;}
_LL3381:;}}} for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null(
fields))->tl){ if( !(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
fields))->hd)).f2){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp3398=(
unsigned char*)"too few arguments to struct"; struct _tagged_string _temp3399;
_temp3399.curr= _temp3398; _temp3399.base= _temp3398; _temp3399.last_plus_one=
_temp3398 + 28; _temp3399;})); break;}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( ans);}} int Cyc_Tcutil_is_tagged_pointer_typ(
void* t){ void* _temp3400= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp3406; struct Cyc_Absyn_Conref* _temp3408; struct Cyc_Absyn_Tqual _temp3410;
struct Cyc_Absyn_Conref* _temp3412; void* _temp3414; void* _temp3416; _LL3402:
if(( unsigned int) _temp3400 > 4u?*(( int*) _temp3400) == Cyc_Absyn_PointerType:
0){ _LL3407: _temp3406=(( struct Cyc_Absyn_PointerType_struct*) _temp3400)->f1;
_LL3417: _temp3416=( void*) _temp3406.elt_typ; goto _LL3415; _LL3415: _temp3414=(
void*) _temp3406.rgn_typ; goto _LL3413; _LL3413: _temp3412= _temp3406.nullable;
goto _LL3411; _LL3411: _temp3410= _temp3406.tq; goto _LL3409; _LL3409: _temp3408=
_temp3406.bounds; goto _LL3403;} else{ goto _LL3404;} _LL3404: goto _LL3405;
_LL3403: { struct Cyc_Absyn_Conref* _temp3418=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3408); goto
_LL3419; _LL3419: { void* _temp3420=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3418))->v; void*
_temp3428; _LL3422: if(( unsigned int) _temp3420 > 1u?*(( int*) _temp3420) ==
Cyc_Absyn_Eq_constr: 0){ _LL3429: _temp3428=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3420)->f1; if( _temp3428 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3423;}
else{ goto _LL3424;}} else{ goto _LL3424;} _LL3424: if( _temp3420 ==( void*) Cyc_Absyn_No_constr){
goto _LL3425;} else{ goto _LL3426;} _LL3426: goto _LL3427; _LL3423: return 1;
_LL3425:( void*)( _temp3418->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3430=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3430[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3431; _temp3431.tag= Cyc_Absyn_Eq_constr;
_temp3431.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp3431;}); _temp3430;})));
return 1; _LL3427: return 0; _LL3421:;}} _LL3405: return 0; _LL3401:;} struct
_tuple7 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e){ struct _tuple7 bogus_ans=({ struct _tuple7 _temp3670; _temp3670.f1= 0;
_temp3670.f2=( void*) Cyc_Absyn_HeapRgn; _temp3670;}); void* _temp3432=( void*)
e->r; void* _temp3446; struct _tuple1* _temp3448; struct _tagged_string*
_temp3450; struct Cyc_Absyn_Exp* _temp3452; struct _tagged_string* _temp3454;
struct Cyc_Absyn_Exp* _temp3456; struct Cyc_Absyn_Exp* _temp3458; struct Cyc_Absyn_Exp*
_temp3460; struct Cyc_Absyn_Exp* _temp3462; _LL3434: if(*(( int*) _temp3432) ==
Cyc_Absyn_Var_e){ _LL3449: _temp3448=(( struct Cyc_Absyn_Var_e_struct*)
_temp3432)->f1; goto _LL3447; _LL3447: _temp3446=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3432)->f2; goto _LL3435;} else{ goto _LL3436;} _LL3436: if(*(( int*)
_temp3432) == Cyc_Absyn_StructMember_e){ _LL3453: _temp3452=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3432)->f1; goto _LL3451; _LL3451: _temp3450=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3432)->f2; goto _LL3437;} else{ goto _LL3438;} _LL3438: if(*(( int*)
_temp3432) == Cyc_Absyn_StructArrow_e){ _LL3457: _temp3456=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3432)->f1; goto _LL3455; _LL3455: _temp3454=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3432)->f2; goto _LL3439;} else{ goto _LL3440;} _LL3440: if(*(( int*)
_temp3432) == Cyc_Absyn_Deref_e){ _LL3459: _temp3458=(( struct Cyc_Absyn_Deref_e_struct*)
_temp3432)->f1; goto _LL3441;} else{ goto _LL3442;} _LL3442: if(*(( int*)
_temp3432) == Cyc_Absyn_Subscript_e){ _LL3463: _temp3462=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3432)->f1; goto _LL3461; _LL3461: _temp3460=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3432)->f2; goto _LL3443;} else{ goto _LL3444;} _LL3444: goto _LL3445;
_LL3435: { void* _temp3464= _temp3446; struct Cyc_Absyn_Fndecl* _temp3478;
struct Cyc_Absyn_Vardecl* _temp3480; struct Cyc_Absyn_Vardecl* _temp3482; struct
Cyc_Absyn_Vardecl* _temp3484; struct Cyc_Absyn_Vardecl* _temp3486; _LL3466: if(
_temp3464 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL3467;} else{ goto _LL3468;}
_LL3468: if(( unsigned int) _temp3464 > 1u?*(( int*) _temp3464) == Cyc_Absyn_Funname_b:
0){ _LL3479: _temp3478=(( struct Cyc_Absyn_Funname_b_struct*) _temp3464)->f1;
goto _LL3469;} else{ goto _LL3470;} _LL3470: if(( unsigned int) _temp3464 > 1u?*((
int*) _temp3464) == Cyc_Absyn_Global_b: 0){ _LL3481: _temp3480=(( struct Cyc_Absyn_Global_b_struct*)
_temp3464)->f1; goto _LL3471;} else{ goto _LL3472;} _LL3472: if(( unsigned int)
_temp3464 > 1u?*(( int*) _temp3464) == Cyc_Absyn_Local_b: 0){ _LL3483: _temp3482=((
struct Cyc_Absyn_Local_b_struct*) _temp3464)->f1; goto _LL3473;} else{ goto
_LL3474;} _LL3474: if(( unsigned int) _temp3464 > 1u?*(( int*) _temp3464) == Cyc_Absyn_Pat_b:
0){ _LL3485: _temp3484=(( struct Cyc_Absyn_Pat_b_struct*) _temp3464)->f1; goto
_LL3475;} else{ goto _LL3476;} _LL3476: if(( unsigned int) _temp3464 > 1u?*((
int*) _temp3464) == Cyc_Absyn_Param_b: 0){ _LL3487: _temp3486=(( struct Cyc_Absyn_Param_b_struct*)
_temp3464)->f1; goto _LL3477;} else{ goto _LL3465;} _LL3467: return bogus_ans;
_LL3469: return({ struct _tuple7 _temp3488; _temp3488.f1= 1; _temp3488.f2=( void*)
Cyc_Absyn_HeapRgn; _temp3488;}); _LL3471: { void* _temp3489= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); struct Cyc_Absyn_Exp*
_temp3495; struct Cyc_Absyn_Tqual _temp3497; void* _temp3499; _LL3491: if((
unsigned int) _temp3489 > 4u?*(( int*) _temp3489) == Cyc_Absyn_ArrayType: 0){
_LL3500: _temp3499=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3489)->f1;
goto _LL3498; _LL3498: _temp3497=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3489)->f2; goto _LL3496; _LL3496: _temp3495=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3489)->f3; goto _LL3492;} else{ goto _LL3493;} _LL3493: goto _LL3494;
_LL3492: return({ struct _tuple7 _temp3501; _temp3501.f1= 1; _temp3501.f2=( void*)
Cyc_Absyn_HeapRgn; _temp3501;}); _LL3494: return({ struct _tuple7 _temp3502;
_temp3502.f1=( _temp3480->tq).q_const; _temp3502.f2=( void*) Cyc_Absyn_HeapRgn;
_temp3502;}); _LL3490:;} _LL3473: { void* _temp3503= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v); struct Cyc_Absyn_Exp*
_temp3509; struct Cyc_Absyn_Tqual _temp3511; void* _temp3513; _LL3505: if((
unsigned int) _temp3503 > 4u?*(( int*) _temp3503) == Cyc_Absyn_ArrayType: 0){
_LL3514: _temp3513=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3503)->f1;
goto _LL3512; _LL3512: _temp3511=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3503)->f2; goto _LL3510; _LL3510: _temp3509=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3503)->f3; goto _LL3506;} else{ goto _LL3507;} _LL3507: goto _LL3508;
_LL3506: return({ struct _tuple7 _temp3515; _temp3515.f1= 1; _temp3515.f2=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp3482->rgn))->v; _temp3515;}); _LL3508:
return({ struct _tuple7 _temp3516; _temp3516.f1=( _temp3482->tq).q_const;
_temp3516.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3482->rgn))->v;
_temp3516;}); _LL3504:;} _LL3475: _temp3486= _temp3484; goto _LL3477; _LL3477:
return({ struct _tuple7 _temp3517; _temp3517.f1=( _temp3486->tq).q_const;
_temp3517.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3486->rgn))->v;
_temp3517;}); _LL3465:;} _LL3437: { void* _temp3518= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp3452->topt))->v); struct Cyc_List_List*
_temp3530; struct Cyc_List_List* _temp3532; struct Cyc_Absyn_Structdecl**
_temp3534; struct Cyc_Absyn_Structdecl* _temp3536; struct Cyc_List_List*
_temp3537; struct _tuple1* _temp3539; struct Cyc_Absyn_Uniondecl** _temp3541;
struct Cyc_Absyn_Uniondecl* _temp3543; struct Cyc_List_List* _temp3544; struct
_tuple1* _temp3546; _LL3520: if(( unsigned int) _temp3518 > 4u?*(( int*)
_temp3518) == Cyc_Absyn_AnonStructType: 0){ _LL3531: _temp3530=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3518)->f1; goto _LL3521;} else{ goto _LL3522;} _LL3522: if(( unsigned int)
_temp3518 > 4u?*(( int*) _temp3518) == Cyc_Absyn_AnonUnionType: 0){ _LL3533:
_temp3532=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3518)->f1; goto
_LL3523;} else{ goto _LL3524;} _LL3524: if(( unsigned int) _temp3518 > 4u?*((
int*) _temp3518) == Cyc_Absyn_StructType: 0){ _LL3540: _temp3539=(( struct Cyc_Absyn_StructType_struct*)
_temp3518)->f1; goto _LL3538; _LL3538: _temp3537=(( struct Cyc_Absyn_StructType_struct*)
_temp3518)->f2; goto _LL3535; _LL3535: _temp3534=(( struct Cyc_Absyn_StructType_struct*)
_temp3518)->f3; if( _temp3534 == 0){ goto _LL3526;} else{ _temp3536=* _temp3534;
goto _LL3525;}} else{ goto _LL3526;} _LL3526: if(( unsigned int) _temp3518 > 4u?*((
int*) _temp3518) == Cyc_Absyn_UnionType: 0){ _LL3547: _temp3546=(( struct Cyc_Absyn_UnionType_struct*)
_temp3518)->f1; goto _LL3545; _LL3545: _temp3544=(( struct Cyc_Absyn_UnionType_struct*)
_temp3518)->f2; goto _LL3542; _LL3542: _temp3541=(( struct Cyc_Absyn_UnionType_struct*)
_temp3518)->f3; if( _temp3541 == 0){ goto _LL3528;} else{ _temp3543=* _temp3541;
goto _LL3527;}} else{ goto _LL3528;} _LL3528: goto _LL3529; _LL3521: _temp3532=
_temp3530; goto _LL3523; _LL3523: { struct Cyc_Absyn_Structfield* _temp3548= Cyc_Absyn_lookup_field(
_temp3532, _temp3450); goto _LL3549; _LL3549: if( _temp3548 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3548))->width != 0: 0){ return({ struct _tuple7 _temp3550;
_temp3550.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3548))->tq).q_const;
_temp3550.f2=( Cyc_Tcutil_addressof_props( te, _temp3452)).f2; _temp3550;});}
return bogus_ans;} _LL3525: { struct Cyc_Absyn_Structfield* _temp3551= Cyc_Absyn_lookup_struct_field(
_temp3536, _temp3450); goto _LL3552; _LL3552: if( _temp3551 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3551))->width != 0: 0){ return({ struct _tuple7 _temp3553;
_temp3553.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3551))->tq).q_const;
_temp3553.f2=( Cyc_Tcutil_addressof_props( te, _temp3452)).f2; _temp3553;});}
return bogus_ans;} _LL3527: { struct Cyc_Absyn_Structfield* _temp3554= Cyc_Absyn_lookup_union_field(
_temp3543, _temp3450); goto _LL3555; _LL3555: if( _temp3554 != 0){ return({
struct _tuple7 _temp3556; _temp3556.f1=((( struct Cyc_Absyn_Structfield*)
_check_null( _temp3554))->tq).q_const; _temp3556.f2=( Cyc_Tcutil_addressof_props(
te, _temp3452)).f2; _temp3556;});} goto _LL3529;} _LL3529: return bogus_ans;
_LL3519:;} _LL3439: { void* _temp3557= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3456->topt))->v); struct Cyc_Absyn_PtrInfo _temp3563; struct
Cyc_Absyn_Conref* _temp3565; struct Cyc_Absyn_Tqual _temp3567; struct Cyc_Absyn_Conref*
_temp3569; void* _temp3571; void* _temp3573; _LL3559: if(( unsigned int)
_temp3557 > 4u?*(( int*) _temp3557) == Cyc_Absyn_PointerType: 0){ _LL3564:
_temp3563=(( struct Cyc_Absyn_PointerType_struct*) _temp3557)->f1; _LL3574:
_temp3573=( void*) _temp3563.elt_typ; goto _LL3572; _LL3572: _temp3571=( void*)
_temp3563.rgn_typ; goto _LL3570; _LL3570: _temp3569= _temp3563.nullable; goto
_LL3568; _LL3568: _temp3567= _temp3563.tq; goto _LL3566; _LL3566: _temp3565=
_temp3563.bounds; goto _LL3560;} else{ goto _LL3561;} _LL3561: goto _LL3562;
_LL3560: { void* _temp3575= Cyc_Tcutil_compress( _temp3573); struct Cyc_List_List*
_temp3587; struct Cyc_List_List* _temp3589; struct Cyc_Absyn_Structdecl**
_temp3591; struct Cyc_Absyn_Structdecl* _temp3593; struct Cyc_List_List*
_temp3594; struct _tuple1* _temp3596; struct Cyc_Absyn_Uniondecl** _temp3598;
struct Cyc_Absyn_Uniondecl* _temp3600; struct Cyc_List_List* _temp3601; struct
_tuple1* _temp3603; _LL3577: if(( unsigned int) _temp3575 > 4u?*(( int*)
_temp3575) == Cyc_Absyn_AnonStructType: 0){ _LL3588: _temp3587=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3575)->f1; goto _LL3578;} else{ goto _LL3579;} _LL3579: if(( unsigned int)
_temp3575 > 4u?*(( int*) _temp3575) == Cyc_Absyn_AnonUnionType: 0){ _LL3590:
_temp3589=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3575)->f1; goto
_LL3580;} else{ goto _LL3581;} _LL3581: if(( unsigned int) _temp3575 > 4u?*((
int*) _temp3575) == Cyc_Absyn_StructType: 0){ _LL3597: _temp3596=(( struct Cyc_Absyn_StructType_struct*)
_temp3575)->f1; goto _LL3595; _LL3595: _temp3594=(( struct Cyc_Absyn_StructType_struct*)
_temp3575)->f2; goto _LL3592; _LL3592: _temp3591=(( struct Cyc_Absyn_StructType_struct*)
_temp3575)->f3; if( _temp3591 == 0){ goto _LL3583;} else{ _temp3593=* _temp3591;
goto _LL3582;}} else{ goto _LL3583;} _LL3583: if(( unsigned int) _temp3575 > 4u?*((
int*) _temp3575) == Cyc_Absyn_UnionType: 0){ _LL3604: _temp3603=(( struct Cyc_Absyn_UnionType_struct*)
_temp3575)->f1; goto _LL3602; _LL3602: _temp3601=(( struct Cyc_Absyn_UnionType_struct*)
_temp3575)->f2; goto _LL3599; _LL3599: _temp3598=(( struct Cyc_Absyn_UnionType_struct*)
_temp3575)->f3; if( _temp3598 == 0){ goto _LL3585;} else{ _temp3600=* _temp3598;
goto _LL3584;}} else{ goto _LL3585;} _LL3585: goto _LL3586; _LL3578: _temp3589=
_temp3587; goto _LL3580; _LL3580: { struct Cyc_Absyn_Structfield* _temp3605= Cyc_Absyn_lookup_field(
_temp3589, _temp3454); goto _LL3606; _LL3606: if( _temp3605 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3605))->width != 0: 0){ return({ struct _tuple7 _temp3607;
_temp3607.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3605))->tq).q_const;
_temp3607.f2= _temp3571; _temp3607;});} return bogus_ans;} _LL3582: { struct Cyc_Absyn_Structfield*
_temp3608= Cyc_Absyn_lookup_struct_field( _temp3593, _temp3454); goto _LL3609;
_LL3609: if( _temp3608 != 0?(( struct Cyc_Absyn_Structfield*) _check_null(
_temp3608))->width != 0: 0){ return({ struct _tuple7 _temp3610; _temp3610.f1=(((
struct Cyc_Absyn_Structfield*) _check_null( _temp3608))->tq).q_const; _temp3610.f2=
_temp3571; _temp3610;});} return bogus_ans;} _LL3584: { struct Cyc_Absyn_Structfield*
_temp3611= Cyc_Absyn_lookup_union_field( _temp3600, _temp3454); goto _LL3612;
_LL3612: if( _temp3611 != 0){ return({ struct _tuple7 _temp3613; _temp3613.f1=(((
struct Cyc_Absyn_Structfield*) _check_null( _temp3611))->tq).q_const; _temp3613.f2=
_temp3571; _temp3613;});} return bogus_ans;} _LL3586: return bogus_ans; _LL3576:;}
_LL3562: return bogus_ans; _LL3558:;} _LL3441: { void* _temp3614= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp3458->topt))->v); struct Cyc_Absyn_PtrInfo
_temp3620; struct Cyc_Absyn_Conref* _temp3622; struct Cyc_Absyn_Tqual _temp3624;
struct Cyc_Absyn_Conref* _temp3626; void* _temp3628; void* _temp3630; _LL3616:
if(( unsigned int) _temp3614 > 4u?*(( int*) _temp3614) == Cyc_Absyn_PointerType:
0){ _LL3621: _temp3620=(( struct Cyc_Absyn_PointerType_struct*) _temp3614)->f1;
_LL3631: _temp3630=( void*) _temp3620.elt_typ; goto _LL3629; _LL3629: _temp3628=(
void*) _temp3620.rgn_typ; goto _LL3627; _LL3627: _temp3626= _temp3620.nullable;
goto _LL3625; _LL3625: _temp3624= _temp3620.tq; goto _LL3623; _LL3623: _temp3622=
_temp3620.bounds; goto _LL3617;} else{ goto _LL3618;} _LL3618: goto _LL3619;
_LL3617: return({ struct _tuple7 _temp3632; _temp3632.f1= _temp3624.q_const;
_temp3632.f2= _temp3628; _temp3632;}); _LL3619: return bogus_ans; _LL3615:;}
_LL3443: { void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3462->topt))->v); void* _temp3633= t; struct Cyc_Absyn_Exp*
_temp3643; struct Cyc_Absyn_Tqual _temp3645; void* _temp3647; struct Cyc_List_List*
_temp3649; struct Cyc_Absyn_PtrInfo _temp3651; struct Cyc_Absyn_Conref*
_temp3653; struct Cyc_Absyn_Tqual _temp3655; struct Cyc_Absyn_Conref* _temp3657;
void* _temp3659; void* _temp3661; _LL3635: if(( unsigned int) _temp3633 > 4u?*((
int*) _temp3633) == Cyc_Absyn_ArrayType: 0){ _LL3648: _temp3647=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3633)->f1; goto _LL3646; _LL3646:
_temp3645=(( struct Cyc_Absyn_ArrayType_struct*) _temp3633)->f2; goto _LL3644;
_LL3644: _temp3643=(( struct Cyc_Absyn_ArrayType_struct*) _temp3633)->f3; goto
_LL3636;} else{ goto _LL3637;} _LL3637: if(( unsigned int) _temp3633 > 4u?*((
int*) _temp3633) == Cyc_Absyn_TupleType: 0){ _LL3650: _temp3649=(( struct Cyc_Absyn_TupleType_struct*)
_temp3633)->f1; goto _LL3638;} else{ goto _LL3639;} _LL3639: if(( unsigned int)
_temp3633 > 4u?*(( int*) _temp3633) == Cyc_Absyn_PointerType: 0){ _LL3652:
_temp3651=(( struct Cyc_Absyn_PointerType_struct*) _temp3633)->f1; _LL3662:
_temp3661=( void*) _temp3651.elt_typ; goto _LL3660; _LL3660: _temp3659=( void*)
_temp3651.rgn_typ; goto _LL3658; _LL3658: _temp3657= _temp3651.nullable; goto
_LL3656; _LL3656: _temp3655= _temp3651.tq; goto _LL3654; _LL3654: _temp3653=
_temp3651.bounds; goto _LL3640;} else{ goto _LL3641;} _LL3641: goto _LL3642;
_LL3636: return({ struct _tuple7 _temp3663; _temp3663.f1= _temp3645.q_const;
_temp3663.f2=( Cyc_Tcutil_addressof_props( te, _temp3462)).f2; _temp3663;});
_LL3638: { struct _tuple4* _temp3664= Cyc_Absyn_lookup_tuple_field( _temp3649,(
int) Cyc_Evexp_eval_const_uint_exp( _temp3460)); goto _LL3665; _LL3665: if(
_temp3664 != 0){ return({ struct _tuple7 _temp3666; _temp3666.f1=((*(( struct
_tuple4*) _check_null( _temp3664))).f1).q_const; _temp3666.f2=( Cyc_Tcutil_addressof_props(
te, _temp3462)).f2; _temp3666;});} return bogus_ans;} _LL3640: return({ struct
_tuple7 _temp3667; _temp3667.f1= _temp3655.q_const; _temp3667.f2= _temp3659;
_temp3667;}); _LL3642: return bogus_ans; _LL3634:;} _LL3445: Cyc_Tcutil_terr( e->loc,({
unsigned char* _temp3668=( unsigned char*)"unary & applied to non-lvalue";
struct _tagged_string _temp3669; _temp3669.curr= _temp3668; _temp3669.base=
_temp3668; _temp3669.last_plus_one= _temp3668 + 30; _temp3669;})); return
bogus_ans; _LL3433:;} void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv* te,
void* e_typ, struct Cyc_Absyn_Exp* e){ void* _temp3671= Cyc_Tcutil_compress(
e_typ); struct Cyc_Absyn_Exp* _temp3677; struct Cyc_Absyn_Tqual _temp3679; void*
_temp3681; _LL3673: if(( unsigned int) _temp3671 > 4u?*(( int*) _temp3671) ==
Cyc_Absyn_ArrayType: 0){ _LL3682: _temp3681=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3671)->f1; goto _LL3680; _LL3680: _temp3679=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3671)->f2; goto _LL3678; _LL3678: _temp3677=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3671)->f3; goto _LL3674;} else{ goto _LL3675;} _LL3675: goto _LL3676;
_LL3674: { void* _temp3685; int _temp3687; struct _tuple7 _temp3683= Cyc_Tcutil_addressof_props(
te, e); _LL3688: _temp3687= _temp3683.f1; goto _LL3686; _LL3686: _temp3685=
_temp3683.f2; goto _LL3684; _LL3684: return Cyc_Absyn_atb_typ( _temp3681,
_temp3685, _temp3679,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3689=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3689[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3690; _temp3690.tag= Cyc_Absyn_Upper_b;
_temp3690.f1= e; _temp3690;}); _temp3689;}));} _LL3676: return e_typ; _LL3672:;}
void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment* loc, unsigned int i,
struct Cyc_Absyn_Conref* b){ b=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( b);{ void* _temp3691=( void*) b->v; void*
_temp3699; void* _temp3701; struct Cyc_Absyn_Exp* _temp3703; _LL3693: if((
unsigned int) _temp3691 > 1u?*(( int*) _temp3691) == Cyc_Absyn_Eq_constr: 0){
_LL3700: _temp3699=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3691)->f1;
if( _temp3699 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3694;} else{ goto _LL3695;}}
else{ goto _LL3695;} _LL3695: if(( unsigned int) _temp3691 > 1u?*(( int*)
_temp3691) == Cyc_Absyn_Eq_constr: 0){ _LL3702: _temp3701=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3691)->f1; if(( unsigned int) _temp3701 > 1u?*(( int*) _temp3701) == Cyc_Absyn_Upper_b:
0){ _LL3704: _temp3703=(( struct Cyc_Absyn_Upper_b_struct*) _temp3701)->f1; goto
_LL3696;} else{ goto _LL3697;}} else{ goto _LL3697;} _LL3697: goto _LL3698;
_LL3694: return; _LL3696: if( Cyc_Evexp_eval_const_uint_exp( _temp3703) <= i){
Cyc_Tcutil_terr( loc,({ unsigned char* _temp3705=( unsigned char*)"dereference is out of bounds";
struct _tagged_string _temp3706; _temp3706.curr= _temp3705; _temp3706.base=
_temp3705; _temp3706.last_plus_one= _temp3705 + 29; _temp3706;}));} return;
_LL3698:( void*)( b->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3707=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3707[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3708; _temp3708.tag= Cyc_Absyn_Eq_constr;
_temp3708.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3709=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3709[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3710; _temp3710.tag= Cyc_Absyn_Upper_b;
_temp3710.f1= Cyc_Absyn_uint_exp( i + 1, 0); _temp3710;}); _temp3709;}));
_temp3708;}); _temp3707;}))); return; _LL3692:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_bits_only( void* t){ void* _temp3711= Cyc_Tcutil_compress(
t); void* _temp3737; void* _temp3739; struct Cyc_Absyn_Enumdecl* _temp3741;
struct _tuple1* _temp3743; struct Cyc_Absyn_Exp* _temp3745; struct Cyc_Absyn_Tqual
_temp3747; void* _temp3749; struct Cyc_List_List* _temp3751; struct Cyc_Absyn_Structdecl**
_temp3753; struct Cyc_List_List* _temp3755; struct _tuple1* _temp3757; struct
Cyc_Absyn_Uniondecl** _temp3759; struct Cyc_List_List* _temp3761; struct _tuple1*
_temp3763; struct Cyc_List_List* _temp3765; struct Cyc_List_List* _temp3767;
_LL3713: if( _temp3711 ==( void*) Cyc_Absyn_VoidType){ goto _LL3714;} else{ goto
_LL3715;} _LL3715: if(( unsigned int) _temp3711 > 4u?*(( int*) _temp3711) == Cyc_Absyn_IntType:
0){ _LL3740: _temp3739=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3711)->f1;
goto _LL3738; _LL3738: _temp3737=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3711)->f2; goto _LL3716;} else{ goto _LL3717;} _LL3717: if( _temp3711 ==(
void*) Cyc_Absyn_FloatType){ goto _LL3718;} else{ goto _LL3719;} _LL3719: if(
_temp3711 ==( void*) Cyc_Absyn_DoubleType){ goto _LL3720;} else{ goto _LL3721;}
_LL3721: if(( unsigned int) _temp3711 > 4u?*(( int*) _temp3711) == Cyc_Absyn_EnumType:
0){ _LL3744: _temp3743=(( struct Cyc_Absyn_EnumType_struct*) _temp3711)->f1;
goto _LL3742; _LL3742: _temp3741=(( struct Cyc_Absyn_EnumType_struct*) _temp3711)->f2;
goto _LL3722;} else{ goto _LL3723;} _LL3723: if(( unsigned int) _temp3711 > 4u?*((
int*) _temp3711) == Cyc_Absyn_ArrayType: 0){ _LL3750: _temp3749=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3711)->f1; goto _LL3748; _LL3748:
_temp3747=(( struct Cyc_Absyn_ArrayType_struct*) _temp3711)->f2; goto _LL3746;
_LL3746: _temp3745=(( struct Cyc_Absyn_ArrayType_struct*) _temp3711)->f3; goto
_LL3724;} else{ goto _LL3725;} _LL3725: if(( unsigned int) _temp3711 > 4u?*((
int*) _temp3711) == Cyc_Absyn_TupleType: 0){ _LL3752: _temp3751=(( struct Cyc_Absyn_TupleType_struct*)
_temp3711)->f1; goto _LL3726;} else{ goto _LL3727;} _LL3727: if(( unsigned int)
_temp3711 > 4u?*(( int*) _temp3711) == Cyc_Absyn_StructType: 0){ _LL3758:
_temp3757=(( struct Cyc_Absyn_StructType_struct*) _temp3711)->f1; goto _LL3756;
_LL3756: _temp3755=(( struct Cyc_Absyn_StructType_struct*) _temp3711)->f2; goto
_LL3754; _LL3754: _temp3753=(( struct Cyc_Absyn_StructType_struct*) _temp3711)->f3;
goto _LL3728;} else{ goto _LL3729;} _LL3729: if(( unsigned int) _temp3711 > 4u?*((
int*) _temp3711) == Cyc_Absyn_UnionType: 0){ _LL3764: _temp3763=(( struct Cyc_Absyn_UnionType_struct*)
_temp3711)->f1; goto _LL3762; _LL3762: _temp3761=(( struct Cyc_Absyn_UnionType_struct*)
_temp3711)->f2; goto _LL3760; _LL3760: _temp3759=(( struct Cyc_Absyn_UnionType_struct*)
_temp3711)->f3; goto _LL3730;} else{ goto _LL3731;} _LL3731: if(( unsigned int)
_temp3711 > 4u?*(( int*) _temp3711) == Cyc_Absyn_AnonStructType: 0){ _LL3766:
_temp3765=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3711)->f1; goto
_LL3732;} else{ goto _LL3733;} _LL3733: if(( unsigned int) _temp3711 > 4u?*((
int*) _temp3711) == Cyc_Absyn_AnonUnionType: 0){ _LL3768: _temp3767=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3711)->f1; goto _LL3734;} else{ goto
_LL3735;} _LL3735: goto _LL3736; _LL3714: return 1; _LL3716: return 1; _LL3718:
return 1; _LL3720: return 1; _LL3722: return 0; _LL3724: return Cyc_Tcutil_bits_only(
_temp3749); _LL3726: for( 0; _temp3751 != 0; _temp3751=(( struct Cyc_List_List*)
_check_null( _temp3751))->tl){ if( ! Cyc_Tcutil_bits_only((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp3751))->hd)).f2)){ return 0;}} return 1;
_LL3728: if( _temp3753 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* _temp3769=*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp3753)); goto _LL3770; _LL3770:
if( _temp3769->fields == 0){ return 0;}{ struct _RegionHandle _temp3771=
_new_region(); struct _RegionHandle* rgn=& _temp3771; _push_region( rgn);{
struct Cyc_List_List* _temp3772=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp3769->tvs, _temp3755); goto _LL3773; _LL3773:{
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp3769->fields))->v; for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute( rgn,
_temp3772,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3774= 0; _npop_handler( 0u); return
_temp3774;}}}{ int _temp3775= 1; _npop_handler( 0u); return _temp3775;}};
_pop_region(& _temp3771);}} _LL3730: if( _temp3759 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
_temp3776=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp3759)); goto
_LL3777; _LL3777: if( _temp3776->fields == 0){ return 0;}{ struct _RegionHandle
_temp3778= _new_region(); struct _RegionHandle* rgn=& _temp3778; _push_region(
rgn);{ struct Cyc_List_List* _temp3779=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp3776->tvs, _temp3761); goto _LL3780; _LL3780:{
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp3776->fields))->v; for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute( rgn,
_temp3779,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3781= 0; _npop_handler( 0u); return
_temp3781;}}}{ int _temp3782= 1; _npop_handler( 0u); return _temp3782;}};
_pop_region(& _temp3778);}} _LL3732: _temp3767= _temp3765; goto _LL3734; _LL3734:
for( 0; _temp3767 != 0; _temp3767=(( struct Cyc_List_List*) _check_null(
_temp3767))->tl){ if( ! Cyc_Tcutil_bits_only(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp3767))->hd)->type)){ return 0;}} return
1; _LL3736: return 0; _LL3712:;} struct _tuple20{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; static int Cyc_Tcutil_cnst_exp( struct Cyc_Tcenv_Tenv*
te, int var_okay, struct Cyc_Absyn_Exp* e){ void* _temp3783=( void*) e->r; void*
_temp3823; void* _temp3825; struct Cyc_Absyn_Exp* _temp3827; struct Cyc_Absyn_Enumfield*
_temp3829; struct Cyc_Absyn_Enumdecl* _temp3831; struct _tuple1* _temp3833; void*
_temp3835; struct _tuple1* _temp3837; struct Cyc_Absyn_Exp* _temp3839; struct
Cyc_Absyn_Exp* _temp3841; struct Cyc_Absyn_Exp* _temp3843; struct Cyc_Absyn_Exp*
_temp3845; struct Cyc_Absyn_Exp* _temp3847; struct Cyc_Absyn_Exp* _temp3849;
struct Cyc_List_List* _temp3851; struct Cyc_Absyn_Exp* _temp3853; struct Cyc_Absyn_Exp*
_temp3855; void* _temp3857; struct Cyc_Absyn_Exp* _temp3859; struct Cyc_Absyn_Exp*
_temp3861; struct Cyc_Absyn_Exp* _temp3863; struct Cyc_Absyn_Vardecl* _temp3865;
struct Cyc_List_List* _temp3867; struct Cyc_List_List* _temp3869; void*
_temp3871; struct Cyc_Absyn_Structdecl* _temp3873; struct Cyc_List_List*
_temp3875; struct Cyc_Core_Opt* _temp3877; struct _tuple1* _temp3879; struct Cyc_List_List*
_temp3881; void* _temp3883; struct Cyc_List_List* _temp3885; struct Cyc_Absyn_Tunionfield*
_temp3887; struct Cyc_Absyn_Tuniondecl* _temp3889; struct Cyc_List_List*
_temp3891; struct Cyc_Core_Opt* _temp3893; struct Cyc_Core_Opt* _temp3895;
_LL3785: if(*(( int*) _temp3783) == Cyc_Absyn_Const_e){ _LL3824: _temp3823=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp3783)->f1; goto _LL3786;} else{
goto _LL3787;} _LL3787: if(*(( int*) _temp3783) == Cyc_Absyn_Sizeoftyp_e){
_LL3826: _temp3825=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp3783)->f1;
goto _LL3788;} else{ goto _LL3789;} _LL3789: if(*(( int*) _temp3783) == Cyc_Absyn_Sizeofexp_e){
_LL3828: _temp3827=(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp3783)->f1; goto
_LL3790;} else{ goto _LL3791;} _LL3791: if(*(( int*) _temp3783) == Cyc_Absyn_Enum_e){
_LL3834: _temp3833=(( struct Cyc_Absyn_Enum_e_struct*) _temp3783)->f1; goto
_LL3832; _LL3832: _temp3831=(( struct Cyc_Absyn_Enum_e_struct*) _temp3783)->f2;
goto _LL3830; _LL3830: _temp3829=(( struct Cyc_Absyn_Enum_e_struct*) _temp3783)->f3;
goto _LL3792;} else{ goto _LL3793;} _LL3793: if(*(( int*) _temp3783) == Cyc_Absyn_Var_e){
_LL3838: _temp3837=(( struct Cyc_Absyn_Var_e_struct*) _temp3783)->f1; goto
_LL3836; _LL3836: _temp3835=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp3783)->f2;
goto _LL3794;} else{ goto _LL3795;} _LL3795: if(*(( int*) _temp3783) == Cyc_Absyn_Conditional_e){
_LL3844: _temp3843=(( struct Cyc_Absyn_Conditional_e_struct*) _temp3783)->f1;
goto _LL3842; _LL3842: _temp3841=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3783)->f2; goto _LL3840; _LL3840: _temp3839=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3783)->f3; goto _LL3796;} else{ goto _LL3797;} _LL3797: if(*(( int*)
_temp3783) == Cyc_Absyn_SeqExp_e){ _LL3848: _temp3847=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3783)->f1; goto _LL3846; _LL3846: _temp3845=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3783)->f2; goto _LL3798;} else{ goto _LL3799;} _LL3799: if(*(( int*)
_temp3783) == Cyc_Absyn_NoInstantiate_e){ _LL3850: _temp3849=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp3783)->f1; goto _LL3800;} else{ goto _LL3801;} _LL3801: if(*(( int*)
_temp3783) == Cyc_Absyn_Instantiate_e){ _LL3854: _temp3853=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3783)->f1; goto _LL3852; _LL3852: _temp3851=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3783)->f2; goto _LL3802;} else{ goto _LL3803;} _LL3803: if(*(( int*)
_temp3783) == Cyc_Absyn_Cast_e){ _LL3858: _temp3857=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp3783)->f1; goto _LL3856; _LL3856: _temp3855=(( struct Cyc_Absyn_Cast_e_struct*)
_temp3783)->f2; goto _LL3804;} else{ goto _LL3805;} _LL3805: if(*(( int*)
_temp3783) == Cyc_Absyn_Address_e){ _LL3860: _temp3859=(( struct Cyc_Absyn_Address_e_struct*)
_temp3783)->f1; goto _LL3806;} else{ goto _LL3807;} _LL3807: if(*(( int*)
_temp3783) == Cyc_Absyn_Comprehension_e){ _LL3866: _temp3865=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3783)->f1; goto _LL3864; _LL3864: _temp3863=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3783)->f2; goto _LL3862; _LL3862: _temp3861=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3783)->f3; goto _LL3808;} else{ goto _LL3809;} _LL3809: if(*(( int*)
_temp3783) == Cyc_Absyn_Array_e){ _LL3868: _temp3867=(( struct Cyc_Absyn_Array_e_struct*)
_temp3783)->f1; goto _LL3810;} else{ goto _LL3811;} _LL3811: if(*(( int*)
_temp3783) == Cyc_Absyn_AnonStruct_e){ _LL3872: _temp3871=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3783)->f1; goto _LL3870; _LL3870: _temp3869=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3783)->f2; goto _LL3812;} else{ goto _LL3813;} _LL3813: if(*(( int*)
_temp3783) == Cyc_Absyn_Struct_e){ _LL3880: _temp3879=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3783)->f1; goto _LL3878; _LL3878: _temp3877=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3783)->f2; goto _LL3876; _LL3876: _temp3875=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3783)->f3; goto _LL3874; _LL3874: _temp3873=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3783)->f4; goto _LL3814;} else{ goto _LL3815;} _LL3815: if(*(( int*)
_temp3783) == Cyc_Absyn_Primop_e){ _LL3884: _temp3883=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp3783)->f1; goto _LL3882; _LL3882: _temp3881=(( struct Cyc_Absyn_Primop_e_struct*)
_temp3783)->f2; goto _LL3816;} else{ goto _LL3817;} _LL3817: if(*(( int*)
_temp3783) == Cyc_Absyn_Tuple_e){ _LL3886: _temp3885=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp3783)->f1; goto _LL3818;} else{ goto _LL3819;} _LL3819: if(*(( int*)
_temp3783) == Cyc_Absyn_Tunion_e){ _LL3896: _temp3895=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3783)->f1; goto _LL3894; _LL3894: _temp3893=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3783)->f2; goto _LL3892; _LL3892: _temp3891=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3783)->f3; goto _LL3890; _LL3890: _temp3889=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3783)->f4; goto _LL3888; _LL3888: _temp3887=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3783)->f5; goto _LL3820;} else{ goto _LL3821;} _LL3821: goto _LL3822;
_LL3786: return 1; _LL3788: return 1; _LL3790: return Cyc_Tcutil_cnst_exp( te, 1,
_temp3827); _LL3792: return 1; _LL3794: { struct _handler_cons _temp3897;
_push_handler(& _temp3897);{ int _temp3899= 0; if( setjmp( _temp3897.handler)){
_temp3899= 1;} if( ! _temp3899){{ void* _temp3900= Cyc_Tcenv_lookup_ordinary( te,
e->loc, _temp3837); void* _temp3908; struct Cyc_Absyn_Fndecl* _temp3910; void*
_temp3912; struct Cyc_Absyn_Vardecl* _temp3914; _LL3902: if(*(( int*) _temp3900)
== Cyc_Tcenv_VarRes){ _LL3909: _temp3908=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3900)->f1; if(( unsigned int) _temp3908 > 1u?*(( int*) _temp3908) == Cyc_Absyn_Funname_b:
0){ _LL3911: _temp3910=(( struct Cyc_Absyn_Funname_b_struct*) _temp3908)->f1;
goto _LL3903;} else{ goto _LL3904;}} else{ goto _LL3904;} _LL3904: if(*(( int*)
_temp3900) == Cyc_Tcenv_VarRes){ _LL3913: _temp3912=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3900)->f1; if(( unsigned int) _temp3912 > 1u?*(( int*) _temp3912) == Cyc_Absyn_Global_b:
0){ _LL3915: _temp3914=(( struct Cyc_Absyn_Global_b_struct*) _temp3912)->f1;
goto _LL3905;} else{ goto _LL3906;}} else{ goto _LL3906;} _LL3906: goto _LL3907;
_LL3903: { int _temp3916= 1; _npop_handler( 0u); return _temp3916;} _LL3905: {
void* _temp3917= Cyc_Tcutil_compress(( void*) _temp3914->type); struct Cyc_Absyn_Exp*
_temp3923; struct Cyc_Absyn_Tqual _temp3925; void* _temp3927; _LL3919: if((
unsigned int) _temp3917 > 4u?*(( int*) _temp3917) == Cyc_Absyn_ArrayType: 0){
_LL3928: _temp3927=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3917)->f1;
goto _LL3926; _LL3926: _temp3925=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3917)->f2; goto _LL3924; _LL3924: _temp3923=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3917)->f3; goto _LL3920;} else{ goto _LL3921;} _LL3921: goto _LL3922;
_LL3920: { int _temp3929= 1; _npop_handler( 0u); return _temp3929;} _LL3922: {
int _temp3930= var_okay; _npop_handler( 0u); return _temp3930;} _LL3918:;}
_LL3907: { int _temp3931= var_okay; _npop_handler( 0u); return _temp3931;}
_LL3901:;}; _pop_handler();} else{ void* _temp3898=( void*) _exn_thrown; void*
_temp3933= _temp3898; _LL3935: if( _temp3933 == Cyc_Dict_Absent){ goto _LL3936;}
else{ goto _LL3937;} _LL3937: goto _LL3938; _LL3936: return 0; _LL3938:( void)
_throw( _temp3933); _LL3934:;}}} _LL3796: return( Cyc_Tcutil_cnst_exp( te, 0,
_temp3843)? Cyc_Tcutil_cnst_exp( te, 0, _temp3841): 0)? Cyc_Tcutil_cnst_exp( te,
0, _temp3839): 0; _LL3798: return Cyc_Tcutil_cnst_exp( te, 0, _temp3847)? Cyc_Tcutil_cnst_exp(
te, 0, _temp3845): 0; _LL3800: _temp3853= _temp3849; goto _LL3802; _LL3802:
return Cyc_Tcutil_cnst_exp( te, var_okay, _temp3853); _LL3804: return Cyc_Tcutil_cnst_exp(
te, var_okay, _temp3855); _LL3806: return Cyc_Tcutil_cnst_exp( te, 1, _temp3859);
_LL3808: return Cyc_Tcutil_cnst_exp( te, 0, _temp3863)? Cyc_Tcutil_cnst_exp( te,
0, _temp3861): 0; _LL3810: _temp3869= _temp3867; goto _LL3812; _LL3812:
_temp3875= _temp3869; goto _LL3814; _LL3814: for( 0; _temp3875 != 0; _temp3875=((
struct Cyc_List_List*) _check_null( _temp3875))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,(*(( struct _tuple20*)(( struct Cyc_List_List*) _check_null( _temp3875))->hd)).f2)){
return 0;}} return 1; _LL3816: _temp3885= _temp3881; goto _LL3818; _LL3818:
_temp3891= _temp3885; goto _LL3820; _LL3820: for( 0; _temp3891 != 0; _temp3891=((
struct Cyc_List_List*) _check_null( _temp3891))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp3891))->hd)){
return 0;}} return 1; _LL3822: return 0; _LL3784:;} int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcutil_cnst_exp(
te, 0, e);} static int Cyc_Tcutil_fields_support_default( struct Cyc_List_List*
tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs); int Cyc_Tcutil_supports_default(
void* t){ void* _temp3939= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt*
_temp3977; int _temp3979; struct Cyc_Core_Opt* _temp3981; struct Cyc_Core_Opt*
_temp3983; struct Cyc_Absyn_Tvar* _temp3985; struct Cyc_Absyn_TunionInfo
_temp3987; struct Cyc_Absyn_TunionFieldInfo _temp3989; struct Cyc_Absyn_PtrInfo
_temp3991; struct Cyc_Absyn_Conref* _temp3993; struct Cyc_Absyn_Tqual _temp3995;
struct Cyc_Absyn_Conref* _temp3997; void* _temp3999; void* _temp4001; void*
_temp4003; void* _temp4005; struct Cyc_Absyn_Exp* _temp4007; struct Cyc_Absyn_Tqual
_temp4009; void* _temp4011; struct Cyc_Absyn_FnInfo _temp4013; struct Cyc_List_List*
_temp4015; struct Cyc_Absyn_Structdecl** _temp4017; struct Cyc_List_List*
_temp4019; struct _tuple1* _temp4021; struct Cyc_Absyn_Uniondecl** _temp4023;
struct Cyc_List_List* _temp4025; struct _tuple1* _temp4027; struct Cyc_List_List*
_temp4029; struct Cyc_List_List* _temp4031; struct Cyc_Absyn_Enumdecl* _temp4033;
struct _tuple1* _temp4035; _LL3941: if( _temp3939 ==( void*) Cyc_Absyn_VoidType){
goto _LL3942;} else{ goto _LL3943;} _LL3943: if(( unsigned int) _temp3939 > 4u?*((
int*) _temp3939) == Cyc_Absyn_Evar: 0){ _LL3984: _temp3983=(( struct Cyc_Absyn_Evar_struct*)
_temp3939)->f1; goto _LL3982; _LL3982: _temp3981=(( struct Cyc_Absyn_Evar_struct*)
_temp3939)->f2; goto _LL3980; _LL3980: _temp3979=(( struct Cyc_Absyn_Evar_struct*)
_temp3939)->f3; goto _LL3978; _LL3978: _temp3977=(( struct Cyc_Absyn_Evar_struct*)
_temp3939)->f4; goto _LL3944;} else{ goto _LL3945;} _LL3945: if(( unsigned int)
_temp3939 > 4u?*(( int*) _temp3939) == Cyc_Absyn_VarType: 0){ _LL3986: _temp3985=((
struct Cyc_Absyn_VarType_struct*) _temp3939)->f1; goto _LL3946;} else{ goto
_LL3947;} _LL3947: if(( unsigned int) _temp3939 > 4u?*(( int*) _temp3939) == Cyc_Absyn_TunionType:
0){ _LL3988: _temp3987=(( struct Cyc_Absyn_TunionType_struct*) _temp3939)->f1;
goto _LL3948;} else{ goto _LL3949;} _LL3949: if(( unsigned int) _temp3939 > 4u?*((
int*) _temp3939) == Cyc_Absyn_TunionFieldType: 0){ _LL3990: _temp3989=(( struct
Cyc_Absyn_TunionFieldType_struct*) _temp3939)->f1; goto _LL3950;} else{ goto
_LL3951;} _LL3951: if(( unsigned int) _temp3939 > 4u?*(( int*) _temp3939) == Cyc_Absyn_PointerType:
0){ _LL3992: _temp3991=(( struct Cyc_Absyn_PointerType_struct*) _temp3939)->f1;
_LL4002: _temp4001=( void*) _temp3991.elt_typ; goto _LL4000; _LL4000: _temp3999=(
void*) _temp3991.rgn_typ; goto _LL3998; _LL3998: _temp3997= _temp3991.nullable;
goto _LL3996; _LL3996: _temp3995= _temp3991.tq; goto _LL3994; _LL3994: _temp3993=
_temp3991.bounds; goto _LL3952;} else{ goto _LL3953;} _LL3953: if(( unsigned int)
_temp3939 > 4u?*(( int*) _temp3939) == Cyc_Absyn_IntType: 0){ _LL4006: _temp4005=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp3939)->f1; goto _LL4004; _LL4004:
_temp4003=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3939)->f2; goto
_LL3954;} else{ goto _LL3955;} _LL3955: if( _temp3939 ==( void*) Cyc_Absyn_FloatType){
goto _LL3956;} else{ goto _LL3957;} _LL3957: if( _temp3939 ==( void*) Cyc_Absyn_DoubleType){
goto _LL3958;} else{ goto _LL3959;} _LL3959: if(( unsigned int) _temp3939 > 4u?*((
int*) _temp3939) == Cyc_Absyn_ArrayType: 0){ _LL4012: _temp4011=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3939)->f1; goto _LL4010; _LL4010:
_temp4009=(( struct Cyc_Absyn_ArrayType_struct*) _temp3939)->f2; goto _LL4008;
_LL4008: _temp4007=(( struct Cyc_Absyn_ArrayType_struct*) _temp3939)->f3; goto
_LL3960;} else{ goto _LL3961;} _LL3961: if(( unsigned int) _temp3939 > 4u?*((
int*) _temp3939) == Cyc_Absyn_FnType: 0){ _LL4014: _temp4013=(( struct Cyc_Absyn_FnType_struct*)
_temp3939)->f1; goto _LL3962;} else{ goto _LL3963;} _LL3963: if(( unsigned int)
_temp3939 > 4u?*(( int*) _temp3939) == Cyc_Absyn_TupleType: 0){ _LL4016:
_temp4015=(( struct Cyc_Absyn_TupleType_struct*) _temp3939)->f1; goto _LL3964;}
else{ goto _LL3965;} _LL3965: if(( unsigned int) _temp3939 > 4u?*(( int*)
_temp3939) == Cyc_Absyn_StructType: 0){ _LL4022: _temp4021=(( struct Cyc_Absyn_StructType_struct*)
_temp3939)->f1; goto _LL4020; _LL4020: _temp4019=(( struct Cyc_Absyn_StructType_struct*)
_temp3939)->f2; goto _LL4018; _LL4018: _temp4017=(( struct Cyc_Absyn_StructType_struct*)
_temp3939)->f3; goto _LL3966;} else{ goto _LL3967;} _LL3967: if(( unsigned int)
_temp3939 > 4u?*(( int*) _temp3939) == Cyc_Absyn_UnionType: 0){ _LL4028:
_temp4027=(( struct Cyc_Absyn_UnionType_struct*) _temp3939)->f1; goto _LL4026;
_LL4026: _temp4025=(( struct Cyc_Absyn_UnionType_struct*) _temp3939)->f2; goto
_LL4024; _LL4024: _temp4023=(( struct Cyc_Absyn_UnionType_struct*) _temp3939)->f3;
goto _LL3968;} else{ goto _LL3969;} _LL3969: if(( unsigned int) _temp3939 > 4u?*((
int*) _temp3939) == Cyc_Absyn_AnonStructType: 0){ _LL4030: _temp4029=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp3939)->f1; goto _LL3970;} else{ goto
_LL3971;} _LL3971: if(( unsigned int) _temp3939 > 4u?*(( int*) _temp3939) == Cyc_Absyn_AnonUnionType:
0){ _LL4032: _temp4031=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3939)->f1;
goto _LL3972;} else{ goto _LL3973;} _LL3973: if(( unsigned int) _temp3939 > 4u?*((
int*) _temp3939) == Cyc_Absyn_EnumType: 0){ _LL4036: _temp4035=(( struct Cyc_Absyn_EnumType_struct*)
_temp3939)->f1; goto _LL4034; _LL4034: _temp4033=(( struct Cyc_Absyn_EnumType_struct*)
_temp3939)->f2; goto _LL3974;} else{ goto _LL3975;} _LL3975: goto _LL3976;
_LL3942: return 1; _LL3944: return 0; _LL3946: return 0; _LL3948: return 0;
_LL3950: return 0; _LL3952: { void* _temp4037=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3993))->v; void*
_temp4045; void* _temp4047; struct Cyc_Absyn_Exp* _temp4049; _LL4039: if((
unsigned int) _temp4037 > 1u?*(( int*) _temp4037) == Cyc_Absyn_Eq_constr: 0){
_LL4046: _temp4045=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp4037)->f1;
if( _temp4045 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL4040;} else{ goto _LL4041;}}
else{ goto _LL4041;} _LL4041: if(( unsigned int) _temp4037 > 1u?*(( int*)
_temp4037) == Cyc_Absyn_Eq_constr: 0){ _LL4048: _temp4047=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp4037)->f1; if(( unsigned int) _temp4047 > 1u?*(( int*) _temp4047) == Cyc_Absyn_Upper_b:
0){ _LL4050: _temp4049=(( struct Cyc_Absyn_Upper_b_struct*) _temp4047)->f1; goto
_LL4042;} else{ goto _LL4043;}} else{ goto _LL4043;} _LL4043: goto _LL4044;
_LL4040: return 1; _LL4042: { void* _temp4051=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3997))->v; int
_temp4057; _LL4053: if(( unsigned int) _temp4051 > 1u?*(( int*) _temp4051) ==
Cyc_Absyn_Eq_constr: 0){ _LL4058: _temp4057=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp4051)->f1; goto _LL4054;} else{ goto _LL4055;} _LL4055: goto _LL4056;
_LL4054: return _temp4057; _LL4056: return 0; _LL4052:;} _LL4044: return 0;
_LL4038:;} _LL3954: return 1; _LL3956: return 1; _LL3958: return 1; _LL3960:
return Cyc_Tcutil_supports_default( _temp4011); _LL3962: return 0; _LL3964: for(
0; _temp4015 != 0; _temp4015=(( struct Cyc_List_List*) _check_null( _temp4015))->tl){
if( ! Cyc_Tcutil_supports_default((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp4015))->hd)).f2)){ return 0;}} return 1; _LL3966: if(
_temp4017 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp4017)); if( sd->fields == 0){ return 0;} return Cyc_Tcutil_fields_support_default(
sd->tvs, _temp4019,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
sd->fields))->v);} _LL3968: if( _temp4023 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp4023)); if( ud->fields ==
0){ return 0;} return Cyc_Tcutil_fields_support_default( ud->tvs, _temp4025,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ud->fields))->v);}
_LL3970: _temp4031= _temp4029; goto _LL3972; _LL3972: return Cyc_Tcutil_fields_support_default(
0, 0, _temp4031); _LL3974: return 1; _LL3976: return 0; _LL3940:;} static int
Cyc_Tcutil_fields_support_default( struct Cyc_List_List* tvs, struct Cyc_List_List*
ts, struct Cyc_List_List* fs){{ struct _RegionHandle _temp4059= _new_region();
struct _RegionHandle* rgn=& _temp4059; _push_region( rgn);{ struct Cyc_List_List*
_temp4060=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, tvs, ts); goto _LL4061; _LL4061: for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ void* t= Cyc_Tcutil_rsubstitute( rgn, _temp4060,( void*)((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( fs))->hd)->type);
if( ! Cyc_Tcutil_supports_default( t)){ int _temp4062= 0; _npop_handler( 0u);
return _temp4062;}}}; _pop_region(& _temp4059);} return 1;}