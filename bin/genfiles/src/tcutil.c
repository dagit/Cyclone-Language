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
Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x); extern
void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_new_evar(
struct Cyc_Core_Opt* k, struct Cyc_Core_Opt* tenv); extern void* Cyc_Absyn_uint_t;
extern void* Cyc_Absyn_ulong_t; extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_slong_t;
extern void* Cyc_Absyn_float_t; extern void* Cyc_Absyn_double_t; extern void*
Cyc_Absyn_bounds_one; extern void* Cyc_Absyn_atb_typ( void* t, void* rgn, struct
Cyc_Absyn_Tqual tq, void* b); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp(
void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp(
int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_field( struct Cyc_List_List*, struct _tagged_string*); extern
struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field( struct Cyc_Absyn_Structdecl*,
struct _tagged_string*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_union_field(
struct Cyc_Absyn_Uniondecl*, struct _tagged_string*); struct _tuple4{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; extern struct _tuple4* Cyc_Absyn_lookup_tuple_field( struct
Cyc_List_List*, int); extern struct _tagged_string Cyc_Absyn_attribute2string(
void*); extern int Cyc_Absyn_fntype_att( void* a); struct Cyc_PP_Ppstate; struct
Cyc_PP_Out; struct Cyc_PP_Doc; extern struct _tagged_string Cyc_Absynpp_typ2string(
void*); extern struct _tagged_string Cyc_Absynpp_kind2string( void*); extern
struct _tagged_string Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref*);
extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple1*); struct
Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[
11u]; static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int
tag; void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
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
struct Cyc_Position_Segment*, struct _tuple1*); extern int Cyc_String_strcmp(
struct _tagged_string s1, struct _tagged_string s2); extern int Cyc_String_zstrptrcmp(
struct _tagged_string*, struct _tagged_string*); extern struct _tagged_string
Cyc_String_strconcat( struct _tagged_string, struct _tagged_string); extern
unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); extern
unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos( struct
_tagged_string); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_string); extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*,
struct _tagged_string); extern void Cyc_Tcutil_flush_warnings(); extern struct
Cyc_Core_Opt* Cyc_Tcutil_empty_var_set; extern void* Cyc_Tcutil_typ_kind( void*
t); extern void* Cyc_Tcutil_compress( void* t); extern void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*, void*, struct Cyc_List_List*); extern int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_use(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerceable(
void*); extern int Cyc_Tcutil_silent_castable( struct Cyc_Tcenv_Tenv*, struct
Cyc_Position_Segment*, void*, void*); extern int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, void*, void*); extern int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp*);
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
GC_malloc( sizeof( struct Cyc_List_List)); _temp4->hd=( void*)(( struct
_tagged_string*)({ struct _tagged_string* _temp5=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp5[ 0]= msg; _temp5;}));
_temp4->tl= Cyc_Tcutil_warning_msgs; _temp4;});} void Cyc_Tcutil_flush_warnings(){
if( Cyc_Tcutil_warning_segs == 0){ return;} fprintf( sfile_to_file( Cyc_Stdio_stderr),"***Warnings***\n");{
struct Cyc_List_List* seg_strs= Cyc_Position_strings_of_segments( Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs= 0; Cyc_Tcutil_warning_msgs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_Tcutil_warning_msgs); while(
Cyc_Tcutil_warning_msgs != 0) {({ struct _tagged_string _temp6=*(( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( seg_strs))->hd); struct
_tagged_string _temp7=*(( struct _tagged_string*)(( struct Cyc_List_List*)
_check_null( Cyc_Tcutil_warning_msgs))->hd); fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s: %.*s\n",
_temp6.last_plus_one - _temp6.curr, _temp6.curr, _temp7.last_plus_one - _temp7.curr,
_temp7.curr);}); seg_strs=(( struct Cyc_List_List*) _check_null( seg_strs))->tl;
Cyc_Tcutil_warning_msgs=(( struct Cyc_List_List*) _check_null( Cyc_Tcutil_warning_msgs))->tl;}
fprintf( sfile_to_file( Cyc_Stdio_stderr),"**************\n"); Cyc_Stdio_fflush((
struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);}} struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set=
0; static int Cyc_Tcutil_fast_tvar_cmp( struct Cyc_Absyn_Tvar* tv1, struct Cyc_Absyn_Tvar*
tv2){ return*(( int*) _check_null( tv1->identity)) -*(( int*) _check_null( tv2->identity));}
void* Cyc_Tcutil_compress( void* t){ void* _temp8= t; struct Cyc_Core_Opt*
_temp20; int _temp22; struct Cyc_Core_Opt* _temp24; struct Cyc_Core_Opt* _temp26;
struct Cyc_Core_Opt* _temp28; int _temp30; struct Cyc_Core_Opt* _temp32; struct
Cyc_Core_Opt** _temp34; struct Cyc_Core_Opt* _temp35; struct Cyc_Core_Opt*
_temp37; struct Cyc_List_List* _temp39; struct _tuple1* _temp41; struct Cyc_Core_Opt*
_temp43; struct Cyc_Core_Opt** _temp45; struct Cyc_List_List* _temp46; struct
_tuple1* _temp48; _LL10: if(( unsigned int) _temp8 > 4u?*(( int*) _temp8) == Cyc_Absyn_Evar:
0){ _LL27: _temp26=(( struct Cyc_Absyn_Evar_struct*) _temp8)->f1; goto _LL25;
_LL25: _temp24=(( struct Cyc_Absyn_Evar_struct*) _temp8)->f2; if( _temp24 == 0){
goto _LL23;} else{ goto _LL12;} _LL23: _temp22=(( struct Cyc_Absyn_Evar_struct*)
_temp8)->f3; goto _LL21; _LL21: _temp20=(( struct Cyc_Absyn_Evar_struct*) _temp8)->f4;
goto _LL11;} else{ goto _LL12;} _LL12: if(( unsigned int) _temp8 > 4u?*(( int*)
_temp8) == Cyc_Absyn_Evar: 0){ _LL36: _temp35=(( struct Cyc_Absyn_Evar_struct*)
_temp8)->f1; goto _LL33; _LL33: _temp32=(( struct Cyc_Absyn_Evar_struct*) _temp8)->f2;
_temp34=&(( struct Cyc_Absyn_Evar_struct*) _temp8)->f2; goto _LL31; _LL31:
_temp30=(( struct Cyc_Absyn_Evar_struct*) _temp8)->f3; goto _LL29; _LL29:
_temp28=(( struct Cyc_Absyn_Evar_struct*) _temp8)->f4; goto _LL13;} else{ goto
_LL14;} _LL14: if(( unsigned int) _temp8 > 4u?*(( int*) _temp8) == Cyc_Absyn_TypedefType:
0){ _LL42: _temp41=(( struct Cyc_Absyn_TypedefType_struct*) _temp8)->f1; goto
_LL40; _LL40: _temp39=(( struct Cyc_Absyn_TypedefType_struct*) _temp8)->f2; goto
_LL38; _LL38: _temp37=(( struct Cyc_Absyn_TypedefType_struct*) _temp8)->f3; if(
_temp37 == 0){ goto _LL15;} else{ goto _LL16;}} else{ goto _LL16;} _LL16: if((
unsigned int) _temp8 > 4u?*(( int*) _temp8) == Cyc_Absyn_TypedefType: 0){ _LL49:
_temp48=(( struct Cyc_Absyn_TypedefType_struct*) _temp8)->f1; goto _LL47; _LL47:
_temp46=(( struct Cyc_Absyn_TypedefType_struct*) _temp8)->f2; goto _LL44; _LL44:
_temp43=(( struct Cyc_Absyn_TypedefType_struct*) _temp8)->f3; _temp45=&(( struct
Cyc_Absyn_TypedefType_struct*) _temp8)->f3; goto _LL17;} else{ goto _LL18;}
_LL18: goto _LL19; _LL11: return t; _LL13: { void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null(* _temp34))->v); if( t2 !=( void*)((
struct Cyc_Core_Opt*) _check_null(* _temp34))->v){* _temp34=({ struct Cyc_Core_Opt*
_temp50=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp50->v=( void*) t2; _temp50;});} return t2;} _LL15: return t; _LL17: { void*
t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(* _temp45))->v);
if( t2 !=( void*)(( struct Cyc_Core_Opt*) _check_null(* _temp45))->v){* _temp45=({
struct Cyc_Core_Opt* _temp51=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp51->v=( void*) t2; _temp51;});} return t2;} _LL19: return t;
_LL9:;} struct _tuple8{ void* f1; void* f2; } ; static int Cyc_Tcutil_kind_leq(
void* k1, void* k2){ if( k1 == k2){ return 1;}{ struct _tuple8 _temp53=({ struct
_tuple8 _temp52; _temp52.f1= k1; _temp52.f2= k2; _temp52;}); void* _temp63; void*
_temp65; void* _temp67; void* _temp69; void* _temp71; void* _temp73; _LL55:
_LL66: _temp65= _temp53.f1; if( _temp65 ==( void*) Cyc_Absyn_BoxKind){ goto
_LL64;} else{ goto _LL57;} _LL64: _temp63= _temp53.f2; if( _temp63 ==( void*)
Cyc_Absyn_MemKind){ goto _LL56;} else{ goto _LL57;} _LL57: _LL70: _temp69=
_temp53.f1; if( _temp69 ==( void*) Cyc_Absyn_BoxKind){ goto _LL68;} else{ goto
_LL59;} _LL68: _temp67= _temp53.f2; if( _temp67 ==( void*) Cyc_Absyn_AnyKind){
goto _LL58;} else{ goto _LL59;} _LL59: _LL74: _temp73= _temp53.f1; if( _temp73
==( void*) Cyc_Absyn_MemKind){ goto _LL72;} else{ goto _LL61;} _LL72: _temp71=
_temp53.f2; if( _temp71 ==( void*) Cyc_Absyn_AnyKind){ goto _LL60;} else{ goto
_LL61;} _LL61: goto _LL62; _LL56: return 1; _LL58: return 1; _LL60: return 1;
_LL62: return 0; _LL54:;}} void* Cyc_Tcutil_typ_kind( void* t){ void* _temp75=
Cyc_Tcutil_compress( t); struct Cyc_Core_Opt* _temp127; int _temp129; struct Cyc_Core_Opt*
_temp131; struct Cyc_Core_Opt* _temp133; struct Cyc_Absyn_Tvar* _temp135; void*
_temp137; void* _temp139; struct Cyc_Absyn_FnInfo _temp141; void* _temp143;
struct Cyc_Absyn_TunionInfo _temp145; struct Cyc_Absyn_TunionFieldInfo _temp147;
struct Cyc_List_List* _temp149; void* _temp151; struct Cyc_Absyn_Tunionfield*
_temp153; struct Cyc_Absyn_Tuniondecl* _temp155; struct Cyc_Absyn_TunionFieldInfo
_temp157; struct Cyc_List_List* _temp159; void* _temp161; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp163; struct Cyc_Absyn_Structdecl** _temp165; struct Cyc_List_List* _temp167;
struct _tuple1* _temp169; struct Cyc_Absyn_Uniondecl** _temp171; struct Cyc_List_List*
_temp173; struct _tuple1* _temp175; struct Cyc_Absyn_Structdecl** _temp177;
struct Cyc_List_List* _temp179; struct _tuple1* _temp181; struct Cyc_Absyn_Uniondecl**
_temp183; struct Cyc_List_List* _temp185; struct _tuple1* _temp187; struct Cyc_List_List*
_temp189; struct Cyc_List_List* _temp191; struct Cyc_Absyn_Enumdecl* _temp193;
struct _tuple1* _temp195; struct Cyc_Absyn_PtrInfo _temp197; struct Cyc_Absyn_Exp*
_temp199; struct Cyc_Absyn_Tqual _temp201; void* _temp203; struct Cyc_List_List*
_temp205; struct Cyc_Core_Opt* _temp207; struct Cyc_List_List* _temp209; struct
_tuple1* _temp211; void* _temp213; struct Cyc_List_List* _temp215; _LL77: if((
unsigned int) _temp75 > 4u?*(( int*) _temp75) == Cyc_Absyn_Evar: 0){ _LL134:
_temp133=(( struct Cyc_Absyn_Evar_struct*) _temp75)->f1; goto _LL132; _LL132:
_temp131=(( struct Cyc_Absyn_Evar_struct*) _temp75)->f2; goto _LL130; _LL130:
_temp129=(( struct Cyc_Absyn_Evar_struct*) _temp75)->f3; goto _LL128; _LL128:
_temp127=(( struct Cyc_Absyn_Evar_struct*) _temp75)->f4; goto _LL78;} else{ goto
_LL79;} _LL79: if(( unsigned int) _temp75 > 4u?*(( int*) _temp75) == Cyc_Absyn_VarType:
0){ _LL136: _temp135=(( struct Cyc_Absyn_VarType_struct*) _temp75)->f1; goto
_LL80;} else{ goto _LL81;} _LL81: if( _temp75 ==( void*) Cyc_Absyn_VoidType){
goto _LL82;} else{ goto _LL83;} _LL83: if(( unsigned int) _temp75 > 4u?*(( int*)
_temp75) == Cyc_Absyn_IntType: 0){ _LL140: _temp139=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp75)->f1; goto _LL138; _LL138: _temp137=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp75)->f2; goto _LL84;} else{ goto _LL85;} _LL85: if( _temp75 ==( void*) Cyc_Absyn_FloatType){
goto _LL86;} else{ goto _LL87;} _LL87: if( _temp75 ==( void*) Cyc_Absyn_DoubleType){
goto _LL88;} else{ goto _LL89;} _LL89: if(( unsigned int) _temp75 > 4u?*(( int*)
_temp75) == Cyc_Absyn_FnType: 0){ _LL142: _temp141=(( struct Cyc_Absyn_FnType_struct*)
_temp75)->f1; goto _LL90;} else{ goto _LL91;} _LL91: if(( unsigned int) _temp75
> 4u?*(( int*) _temp75) == Cyc_Absyn_RgnHandleType: 0){ _LL144: _temp143=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp75)->f1; goto _LL92;} else{ goto
_LL93;} _LL93: if( _temp75 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL94;} else{
goto _LL95;} _LL95: if(( unsigned int) _temp75 > 4u?*(( int*) _temp75) == Cyc_Absyn_TunionType:
0){ _LL146: _temp145=(( struct Cyc_Absyn_TunionType_struct*) _temp75)->f1; goto
_LL96;} else{ goto _LL97;} _LL97: if(( unsigned int) _temp75 > 4u?*(( int*)
_temp75) == Cyc_Absyn_TunionFieldType: 0){ _LL148: _temp147=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp75)->f1; _LL152: _temp151=( void*) _temp147.field_info; if(*(( int*)
_temp151) == Cyc_Absyn_KnownTunionfield){ _LL156: _temp155=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp151)->f1; goto _LL154; _LL154: _temp153=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp151)->f2; goto _LL150;} else{ goto _LL99;} _LL150: _temp149= _temp147.targs;
goto _LL98;} else{ goto _LL99;} _LL99: if(( unsigned int) _temp75 > 4u?*(( int*)
_temp75) == Cyc_Absyn_TunionFieldType: 0){ _LL158: _temp157=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp75)->f1; _LL162: _temp161=( void*) _temp157.field_info; if(*(( int*)
_temp161) == Cyc_Absyn_UnknownTunionfield){ _LL164: _temp163=(( struct Cyc_Absyn_UnknownTunionfield_struct*)
_temp161)->f1; goto _LL160;} else{ goto _LL101;} _LL160: _temp159= _temp157.targs;
goto _LL100;} else{ goto _LL101;} _LL101: if(( unsigned int) _temp75 > 4u?*((
int*) _temp75) == Cyc_Absyn_StructType: 0){ _LL170: _temp169=(( struct Cyc_Absyn_StructType_struct*)
_temp75)->f1; goto _LL168; _LL168: _temp167=(( struct Cyc_Absyn_StructType_struct*)
_temp75)->f2; goto _LL166; _LL166: _temp165=(( struct Cyc_Absyn_StructType_struct*)
_temp75)->f3; if( _temp165 == 0){ goto _LL102;} else{ goto _LL103;}} else{ goto
_LL103;} _LL103: if(( unsigned int) _temp75 > 4u?*(( int*) _temp75) == Cyc_Absyn_UnionType:
0){ _LL176: _temp175=(( struct Cyc_Absyn_UnionType_struct*) _temp75)->f1; goto
_LL174; _LL174: _temp173=(( struct Cyc_Absyn_UnionType_struct*) _temp75)->f2;
goto _LL172; _LL172: _temp171=(( struct Cyc_Absyn_UnionType_struct*) _temp75)->f3;
if( _temp171 == 0){ goto _LL104;} else{ goto _LL105;}} else{ goto _LL105;}
_LL105: if(( unsigned int) _temp75 > 4u?*(( int*) _temp75) == Cyc_Absyn_StructType:
0){ _LL182: _temp181=(( struct Cyc_Absyn_StructType_struct*) _temp75)->f1; goto
_LL180; _LL180: _temp179=(( struct Cyc_Absyn_StructType_struct*) _temp75)->f2;
goto _LL178; _LL178: _temp177=(( struct Cyc_Absyn_StructType_struct*) _temp75)->f3;
goto _LL106;} else{ goto _LL107;} _LL107: if(( unsigned int) _temp75 > 4u?*((
int*) _temp75) == Cyc_Absyn_UnionType: 0){ _LL188: _temp187=(( struct Cyc_Absyn_UnionType_struct*)
_temp75)->f1; goto _LL186; _LL186: _temp185=(( struct Cyc_Absyn_UnionType_struct*)
_temp75)->f2; goto _LL184; _LL184: _temp183=(( struct Cyc_Absyn_UnionType_struct*)
_temp75)->f3; goto _LL108;} else{ goto _LL109;} _LL109: if(( unsigned int)
_temp75 > 4u?*(( int*) _temp75) == Cyc_Absyn_AnonStructType: 0){ _LL190:
_temp189=(( struct Cyc_Absyn_AnonStructType_struct*) _temp75)->f1; goto _LL110;}
else{ goto _LL111;} _LL111: if(( unsigned int) _temp75 > 4u?*(( int*) _temp75)
== Cyc_Absyn_AnonUnionType: 0){ _LL192: _temp191=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp75)->f1; goto _LL112;} else{ goto _LL113;} _LL113: if(( unsigned int)
_temp75 > 4u?*(( int*) _temp75) == Cyc_Absyn_EnumType: 0){ _LL196: _temp195=((
struct Cyc_Absyn_EnumType_struct*) _temp75)->f1; goto _LL194; _LL194: _temp193=((
struct Cyc_Absyn_EnumType_struct*) _temp75)->f2; goto _LL114;} else{ goto _LL115;}
_LL115: if(( unsigned int) _temp75 > 4u?*(( int*) _temp75) == Cyc_Absyn_PointerType:
0){ _LL198: _temp197=(( struct Cyc_Absyn_PointerType_struct*) _temp75)->f1; goto
_LL116;} else{ goto _LL117;} _LL117: if(( unsigned int) _temp75 > 4u?*(( int*)
_temp75) == Cyc_Absyn_ArrayType: 0){ _LL204: _temp203=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp75)->f1; goto _LL202; _LL202: _temp201=(( struct Cyc_Absyn_ArrayType_struct*)
_temp75)->f2; goto _LL200; _LL200: _temp199=(( struct Cyc_Absyn_ArrayType_struct*)
_temp75)->f3; goto _LL118;} else{ goto _LL119;} _LL119: if(( unsigned int)
_temp75 > 4u?*(( int*) _temp75) == Cyc_Absyn_TupleType: 0){ _LL206: _temp205=((
struct Cyc_Absyn_TupleType_struct*) _temp75)->f1; goto _LL120;} else{ goto
_LL121;} _LL121: if(( unsigned int) _temp75 > 4u?*(( int*) _temp75) == Cyc_Absyn_TypedefType:
0){ _LL212: _temp211=(( struct Cyc_Absyn_TypedefType_struct*) _temp75)->f1; goto
_LL210; _LL210: _temp209=(( struct Cyc_Absyn_TypedefType_struct*) _temp75)->f2;
goto _LL208; _LL208: _temp207=(( struct Cyc_Absyn_TypedefType_struct*) _temp75)->f3;
goto _LL122;} else{ goto _LL123;} _LL123: if(( unsigned int) _temp75 > 4u?*((
int*) _temp75) == Cyc_Absyn_AccessEff: 0){ _LL214: _temp213=( void*)(( struct
Cyc_Absyn_AccessEff_struct*) _temp75)->f1; goto _LL124;} else{ goto _LL125;}
_LL125: if(( unsigned int) _temp75 > 4u?*(( int*) _temp75) == Cyc_Absyn_JoinEff:
0){ _LL216: _temp215=(( struct Cyc_Absyn_JoinEff_struct*) _temp75)->f1; goto
_LL126;} else{ goto _LL76;} _LL78: return( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp133))->v; _LL80: return(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( _temp135->kind); _LL82: return( void*) Cyc_Absyn_MemKind;
_LL84: return _temp137 ==( void*) Cyc_Absyn_B4?( void*) Cyc_Absyn_BoxKind:( void*)
Cyc_Absyn_MemKind; _LL86: return( void*) Cyc_Absyn_MemKind; _LL88: return( void*)
Cyc_Absyn_MemKind; _LL90: return( void*) Cyc_Absyn_MemKind; _LL92: return( void*)
Cyc_Absyn_BoxKind; _LL94: return( void*) Cyc_Absyn_RgnKind; _LL96: return( void*)
Cyc_Absyn_BoxKind; _LL98: if( _temp153->typs == 0){ return( void*) Cyc_Absyn_BoxKind;}
else{ return( void*) Cyc_Absyn_MemKind;} _LL100: return(( void*(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp217="typ_kind: Unresolved TunionFieldType";
struct _tagged_string _temp218; _temp218.curr= _temp217; _temp218.base= _temp217;
_temp218.last_plus_one= _temp217 + 37; _temp218;})); _LL102: return( void*) Cyc_Absyn_AnyKind;
_LL104: return( void*) Cyc_Absyn_AnyKind; _LL106: return( void*) Cyc_Absyn_MemKind;
_LL108: return( void*) Cyc_Absyn_MemKind; _LL110: return( void*) Cyc_Absyn_MemKind;
_LL112: return( void*) Cyc_Absyn_MemKind; _LL114: return( void*) Cyc_Absyn_BoxKind;
_LL116: { void* _temp219=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp197.bounds))->v; void* _temp229; void*
_temp231; struct Cyc_Absyn_Exp* _temp233; struct Cyc_Absyn_Conref* _temp235;
_LL221: if(( unsigned int) _temp219 > 1u?*(( int*) _temp219) == Cyc_Absyn_Eq_constr:
0){ _LL230: _temp229=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp219)->f1;
if( _temp229 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL222;} else{ goto _LL223;}}
else{ goto _LL223;} _LL223: if(( unsigned int) _temp219 > 1u?*(( int*) _temp219)
== Cyc_Absyn_Eq_constr: 0){ _LL232: _temp231=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp219)->f1; if(( unsigned int) _temp231 > 1u?*(( int*) _temp231) == Cyc_Absyn_Upper_b:
0){ _LL234: _temp233=(( struct Cyc_Absyn_Upper_b_struct*) _temp231)->f1; goto
_LL224;} else{ goto _LL225;}} else{ goto _LL225;} _LL225: if( _temp219 ==( void*)
Cyc_Absyn_No_constr){ goto _LL226;} else{ goto _LL227;} _LL227: if((
unsigned int) _temp219 > 1u?*(( int*) _temp219) == Cyc_Absyn_Forward_constr: 0){
_LL236: _temp235=(( struct Cyc_Absyn_Forward_constr_struct*) _temp219)->f1; goto
_LL228;} else{ goto _LL220;} _LL222: return( void*) Cyc_Absyn_MemKind; _LL224:
return( void*) Cyc_Absyn_BoxKind; _LL226: return( void*) Cyc_Absyn_MemKind;
_LL228: return(( void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
unsigned char* _temp237="typ_kind: forward constraint in ptr bounds"; struct
_tagged_string _temp238; _temp238.curr= _temp237; _temp238.base= _temp237;
_temp238.last_plus_one= _temp237 + 43; _temp238;})); _LL220:;} _LL118: return(
void*) Cyc_Absyn_MemKind; _LL120: return( void*) Cyc_Absyn_MemKind; _LL122:
return(( void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({ struct
_tagged_string _temp239= Cyc_Absynpp_typ2string( t); xprintf("typ_kind: typedef found: %.*s",
_temp239.last_plus_one - _temp239.curr, _temp239.curr);})); _LL124: return( void*)
Cyc_Absyn_EffKind; _LL126: return( void*) Cyc_Absyn_EffKind; _LL76:;}
unsigned char Cyc_Tcutil_Unify[ 10u]="\000\000\000\000Unify"; int Cyc_Tcutil_unify(
void* t1, void* t2){ struct _handler_cons _temp240; _push_handler(& _temp240);{
int _temp242= 0; if( setjmp( _temp240.handler)){ _temp242= 1;} if( ! _temp242){
Cyc_Tcutil_unify_it( t1, t2);{ int _temp243= 1; _npop_handler( 0u); return
_temp243;}; _pop_handler();} else{ void* _temp241=( void*) _exn_thrown; void*
_temp245= _temp241; _LL247: if( _temp245 == Cyc_Tcutil_Unify){ goto _LL248;}
else{ goto _LL249;} _LL249: goto _LL250; _LL248: return 0; _LL250:( void) _throw(
_temp245); _LL246:;}}} static void Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List*
env, struct Cyc_List_List* ts); static void Cyc_Tcutil_occurs( void* evar,
struct Cyc_List_List* env, void* t){ void* _temp251= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp285; struct Cyc_Core_Opt* _temp287; struct Cyc_Core_Opt**
_temp289; int _temp290; struct Cyc_Core_Opt* _temp292; struct Cyc_Core_Opt*
_temp294; struct Cyc_Absyn_PtrInfo _temp296; struct Cyc_Absyn_Exp* _temp298;
struct Cyc_Absyn_Tqual _temp300; void* _temp302; struct Cyc_Absyn_FnInfo
_temp304; struct Cyc_List_List* _temp306; struct Cyc_Absyn_VarargInfo* _temp308;
int _temp310; struct Cyc_List_List* _temp312; void* _temp314; struct Cyc_Core_Opt*
_temp316; struct Cyc_List_List* _temp318; struct Cyc_List_List* _temp320; void*
_temp322; struct Cyc_Absyn_TunionInfo _temp324; void* _temp326; struct Cyc_List_List*
_temp328; void* _temp330; struct Cyc_Core_Opt* _temp332; struct Cyc_List_List*
_temp334; struct _tuple1* _temp336; struct Cyc_Absyn_TunionFieldInfo _temp338;
struct Cyc_List_List* _temp340; void* _temp342; struct Cyc_Absyn_Structdecl**
_temp344; struct Cyc_List_List* _temp346; struct _tuple1* _temp348; struct Cyc_List_List*
_temp350; struct Cyc_List_List* _temp352; void* _temp354; struct Cyc_List_List*
_temp356; _LL253: if(( unsigned int) _temp251 > 4u?*(( int*) _temp251) == Cyc_Absyn_VarType:
0){ _LL286: _temp285=(( struct Cyc_Absyn_VarType_struct*) _temp251)->f1; goto
_LL254;} else{ goto _LL255;} _LL255: if(( unsigned int) _temp251 > 4u?*(( int*)
_temp251) == Cyc_Absyn_Evar: 0){ _LL295: _temp294=(( struct Cyc_Absyn_Evar_struct*)
_temp251)->f1; goto _LL293; _LL293: _temp292=(( struct Cyc_Absyn_Evar_struct*)
_temp251)->f2; goto _LL291; _LL291: _temp290=(( struct Cyc_Absyn_Evar_struct*)
_temp251)->f3; goto _LL288; _LL288: _temp287=(( struct Cyc_Absyn_Evar_struct*)
_temp251)->f4; _temp289=&(( struct Cyc_Absyn_Evar_struct*) _temp251)->f4; goto
_LL256;} else{ goto _LL257;} _LL257: if(( unsigned int) _temp251 > 4u?*(( int*)
_temp251) == Cyc_Absyn_PointerType: 0){ _LL297: _temp296=(( struct Cyc_Absyn_PointerType_struct*)
_temp251)->f1; goto _LL258;} else{ goto _LL259;} _LL259: if(( unsigned int)
_temp251 > 4u?*(( int*) _temp251) == Cyc_Absyn_ArrayType: 0){ _LL303: _temp302=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp251)->f1; goto _LL301; _LL301:
_temp300=(( struct Cyc_Absyn_ArrayType_struct*) _temp251)->f2; goto _LL299;
_LL299: _temp298=(( struct Cyc_Absyn_ArrayType_struct*) _temp251)->f3; goto
_LL260;} else{ goto _LL261;} _LL261: if(( unsigned int) _temp251 > 4u?*(( int*)
_temp251) == Cyc_Absyn_FnType: 0){ _LL305: _temp304=(( struct Cyc_Absyn_FnType_struct*)
_temp251)->f1; _LL319: _temp318= _temp304.tvars; goto _LL317; _LL317: _temp316=
_temp304.effect; goto _LL315; _LL315: _temp314=( void*) _temp304.ret_typ; goto
_LL313; _LL313: _temp312= _temp304.args; goto _LL311; _LL311: _temp310= _temp304.c_varargs;
goto _LL309; _LL309: _temp308= _temp304.cyc_varargs; goto _LL307; _LL307:
_temp306= _temp304.attributes; goto _LL262;} else{ goto _LL263;} _LL263: if((
unsigned int) _temp251 > 4u?*(( int*) _temp251) == Cyc_Absyn_TupleType: 0){
_LL321: _temp320=(( struct Cyc_Absyn_TupleType_struct*) _temp251)->f1; goto
_LL264;} else{ goto _LL265;} _LL265: if(( unsigned int) _temp251 > 4u?*(( int*)
_temp251) == Cyc_Absyn_RgnHandleType: 0){ _LL323: _temp322=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp251)->f1; goto _LL266;} else{ goto _LL267;} _LL267: if(( unsigned int)
_temp251 > 4u?*(( int*) _temp251) == Cyc_Absyn_TunionType: 0){ _LL325: _temp324=((
struct Cyc_Absyn_TunionType_struct*) _temp251)->f1; _LL331: _temp330=( void*)
_temp324.tunion_info; goto _LL329; _LL329: _temp328= _temp324.targs; goto _LL327;
_LL327: _temp326=( void*) _temp324.rgn; goto _LL268;} else{ goto _LL269;} _LL269:
if(( unsigned int) _temp251 > 4u?*(( int*) _temp251) == Cyc_Absyn_TypedefType: 0){
_LL337: _temp336=(( struct Cyc_Absyn_TypedefType_struct*) _temp251)->f1; goto
_LL335; _LL335: _temp334=(( struct Cyc_Absyn_TypedefType_struct*) _temp251)->f2;
goto _LL333; _LL333: _temp332=(( struct Cyc_Absyn_TypedefType_struct*) _temp251)->f3;
goto _LL270;} else{ goto _LL271;} _LL271: if(( unsigned int) _temp251 > 4u?*((
int*) _temp251) == Cyc_Absyn_TunionFieldType: 0){ _LL339: _temp338=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp251)->f1; _LL343: _temp342=( void*) _temp338.field_info; goto _LL341;
_LL341: _temp340= _temp338.targs; goto _LL272;} else{ goto _LL273;} _LL273: if((
unsigned int) _temp251 > 4u?*(( int*) _temp251) == Cyc_Absyn_StructType: 0){
_LL349: _temp348=(( struct Cyc_Absyn_StructType_struct*) _temp251)->f1; goto
_LL347; _LL347: _temp346=(( struct Cyc_Absyn_StructType_struct*) _temp251)->f2;
goto _LL345; _LL345: _temp344=(( struct Cyc_Absyn_StructType_struct*) _temp251)->f3;
goto _LL274;} else{ goto _LL275;} _LL275: if(( unsigned int) _temp251 > 4u?*((
int*) _temp251) == Cyc_Absyn_AnonStructType: 0){ _LL351: _temp350=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp251)->f1; goto _LL276;} else{ goto _LL277;} _LL277: if(( unsigned int)
_temp251 > 4u?*(( int*) _temp251) == Cyc_Absyn_AnonUnionType: 0){ _LL353:
_temp352=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp251)->f1; goto _LL278;}
else{ goto _LL279;} _LL279: if(( unsigned int) _temp251 > 4u?*(( int*) _temp251)
== Cyc_Absyn_AccessEff: 0){ _LL355: _temp354=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp251)->f1; goto _LL280;} else{ goto _LL281;} _LL281: if(( unsigned int)
_temp251 > 4u?*(( int*) _temp251) == Cyc_Absyn_JoinEff: 0){ _LL357: _temp356=((
struct Cyc_Absyn_JoinEff_struct*) _temp251)->f1; goto _LL282;} else{ goto _LL283;}
_LL283: goto _LL284; _LL254: if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, env, _temp285)){( void) _throw(( void*) Cyc_Tcutil_Unify);}
goto _LL252; _LL256: if( t == evar){( void) _throw(( void*) Cyc_Tcutil_Unify);}
else{ if( _temp292 != 0){ Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp292))->v);} else{ int problem= 0;{ struct Cyc_List_List* s=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(* _temp289))->v; for(
0; s != 0; s=(( struct Cyc_List_List*) _check_null( s))->tl){ if( !(( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( s))->hd)){ problem=
1; break;}}} if( problem){ struct Cyc_List_List* _temp358= 0; goto _LL359;
_LL359:{ struct Cyc_List_List* s=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null(* _temp289))->v; for( 0; s != 0; s=(( struct Cyc_List_List*)
_check_null( s))->tl){ if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( s))->hd)){ _temp358=({ struct Cyc_List_List* _temp360=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp360->hd=( void*)(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( s))->hd); _temp360->tl= _temp358; _temp360;});}}}*
_temp289=({ struct Cyc_Core_Opt* _temp361=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp361->v=( void*) _temp358; _temp361;});}}}
goto _LL252; _LL258: Cyc_Tcutil_occurs( evar, env,( void*) _temp296.elt_typ);
Cyc_Tcutil_occurs( evar, env,( void*) _temp296.rgn_typ); goto _LL252; _LL260:
Cyc_Tcutil_occurs( evar, env, _temp302); goto _LL252; _LL262: if( _temp316 != 0){
Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp316))->v);} Cyc_Tcutil_occurs( evar, env, _temp314); for( 0; _temp312 != 0;
_temp312=(( struct Cyc_List_List*) _check_null( _temp312))->tl){ Cyc_Tcutil_occurs(
evar, env,(*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp312))->hd)).f3);}
if( _temp308 != 0){ int _temp364; void* _temp366; void* _temp368; struct Cyc_Absyn_Tqual
_temp370; struct Cyc_Core_Opt* _temp372; struct Cyc_Absyn_VarargInfo _temp362=*((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp308)); _LL373: _temp372=
_temp362.name; goto _LL371; _LL371: _temp370= _temp362.tq; goto _LL369; _LL369:
_temp368=( void*) _temp362.type; goto _LL367; _LL367: _temp366=( void*) _temp362.rgn;
goto _LL365; _LL365: _temp364= _temp362.inject; goto _LL363; _LL363: Cyc_Tcutil_occurs(
evar, env, _temp368); Cyc_Tcutil_occurs( evar, env, _temp366);} goto _LL252;
_LL264: for( 0; _temp320 != 0; _temp320=(( struct Cyc_List_List*) _check_null(
_temp320))->tl){ Cyc_Tcutil_occurs( evar, env,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp320))->hd)).f2);} goto _LL252; _LL266: Cyc_Tcutil_occurs( evar,
env, _temp322); goto _LL252; _LL268: Cyc_Tcutil_occurs( evar, env, _temp326);
Cyc_Tcutil_occurslist( evar, env, _temp328); goto _LL252; _LL270: _temp340=
_temp334; goto _LL272; _LL272: _temp346= _temp340; goto _LL274; _LL274: Cyc_Tcutil_occurslist(
evar, env, _temp346); goto _LL252; _LL276: _temp352= _temp350; goto _LL278;
_LL278: for( 0; _temp352 != 0; _temp352=(( struct Cyc_List_List*) _check_null(
_temp352))->tl){ Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp352))->hd)->type);} goto _LL252; _LL280:
Cyc_Tcutil_occurs( evar, env, _temp354); goto _LL252; _LL282: Cyc_Tcutil_occurslist(
evar, env, _temp356); goto _LL252; _LL284: goto _LL252; _LL252:;} static void
Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List* env, struct Cyc_List_List*
ts){ for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){ Cyc_Tcutil_occurs(
evar, env,( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);}} static
void Cyc_Tcutil_unify_list( struct Cyc_List_List* t1, struct Cyc_List_List* t2){
struct _handler_cons _temp374; _push_handler(& _temp374);{ int _temp376= 0; if(
setjmp( _temp374.handler)){ _temp376= 1;} if( ! _temp376){(( void(*)( void(* f)(
void*, void*), struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_iter2)(
Cyc_Tcutil_unify_it, t1, t2);; _pop_handler();} else{ void* _temp375=( void*)
_exn_thrown; void* _temp378= _temp375; _LL380: if( _temp378 == Cyc_List_List_mismatch){
goto _LL381;} else{ goto _LL382;} _LL382: goto _LL383; _LL381:( void) _throw((
void*) Cyc_Tcutil_Unify); _LL383:( void) _throw( _temp378); _LL379:;}}} static
void Cyc_Tcutil_unify_tqual( struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual
tq2){ if(( tq1.q_const != tq2.q_const? 1: tq1.q_volatile != tq2.q_volatile)? 1:
tq1.q_restrict != tq2.q_restrict){( void) _throw(( void*) Cyc_Tcutil_Unify);}}
int Cyc_Tcutil_equal_tqual( struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual
tq2){ return( tq1.q_const == tq2.q_const? tq1.q_volatile == tq2.q_volatile: 0)?
tq1.q_restrict == tq2.q_restrict: 0;} static void Cyc_Tcutil_unify_it_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y){ x=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
x); y=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
y); if( x == y){ return;}{ void* _temp384=( void*) x->v; struct Cyc_Absyn_Conref*
_temp392; void* _temp394; _LL386: if( _temp384 ==( void*) Cyc_Absyn_No_constr){
goto _LL387;} else{ goto _LL388;} _LL388: if(( unsigned int) _temp384 > 1u?*((
int*) _temp384) == Cyc_Absyn_Forward_constr: 0){ _LL393: _temp392=(( struct Cyc_Absyn_Forward_constr_struct*)
_temp384)->f1; goto _LL389;} else{ goto _LL390;} _LL390: if(( unsigned int)
_temp384 > 1u?*(( int*) _temp384) == Cyc_Absyn_Eq_constr: 0){ _LL395: _temp394=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp384)->f1; goto _LL391;} else{
goto _LL385;} _LL387:( void*)( x->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp396=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp396[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp397; _temp397.tag= Cyc_Absyn_Forward_constr; _temp397.f1= y; _temp397;});
_temp396;}))); return; _LL389:( void) _throw((( void*(*)( struct _tagged_string
msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp398="unify_conref: forward after compress";
struct _tagged_string _temp399; _temp399.curr= _temp398; _temp399.base= _temp398;
_temp399.last_plus_one= _temp398 + 37; _temp399;}))); _LL391: { void* _temp400=(
void*) y->v; struct Cyc_Absyn_Conref* _temp408; void* _temp410; _LL402: if(
_temp400 ==( void*) Cyc_Absyn_No_constr){ goto _LL403;} else{ goto _LL404;}
_LL404: if(( unsigned int) _temp400 > 1u?*(( int*) _temp400) == Cyc_Absyn_Forward_constr:
0){ _LL409: _temp408=(( struct Cyc_Absyn_Forward_constr_struct*) _temp400)->f1;
goto _LL405;} else{ goto _LL406;} _LL406: if(( unsigned int) _temp400 > 1u?*((
int*) _temp400) == Cyc_Absyn_Eq_constr: 0){ _LL411: _temp410=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp400)->f1; goto _LL407;} else{ goto _LL401;}
_LL403:( void*)( y->v=( void*)(( void*) x->v)); return; _LL405:( void) _throw(((
void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char*
_temp412="unify_conref: forward after compress(2)"; struct _tagged_string
_temp413; _temp413.curr= _temp412; _temp413.base= _temp412; _temp413.last_plus_one=
_temp412 + 40; _temp413;}))); _LL407: if( cmp( _temp394, _temp410) != 0){( void)
_throw(( void*) Cyc_Tcutil_Unify);} return; _LL401:;} _LL385:;}} static int Cyc_Tcutil_unify_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y){ struct _handler_cons _temp414; _push_handler(& _temp414);{ int _temp416= 0;
if( setjmp( _temp414.handler)){ _temp416= 1;} if( ! _temp416){(( void(*)( int(*
cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y))
Cyc_Tcutil_unify_it_conrefs)( cmp, x, y);{ int _temp417= 1; _npop_handler( 0u);
return _temp417;}; _pop_handler();} else{ void* _temp415=( void*) _exn_thrown;
void* _temp419= _temp415; _LL421: if( _temp419 == Cyc_Tcutil_Unify){ goto _LL422;}
else{ goto _LL423;} _LL423: goto _LL424; _LL422: return 0; _LL424:( void) _throw(
_temp419); _LL420:;}}} static int Cyc_Tcutil_boundscmp( void* b1, void* b2){
struct _tuple8 _temp426=({ struct _tuple8 _temp425; _temp425.f1= b1; _temp425.f2=
b2; _temp425;}); void* _temp436; void* _temp438; void* _temp440; void* _temp442;
void* _temp444; void* _temp446; void* _temp448; struct Cyc_Absyn_Exp* _temp450;
void* _temp452; struct Cyc_Absyn_Exp* _temp454; _LL428: _LL439: _temp438=
_temp426.f1; if( _temp438 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL437;} else{
goto _LL430;} _LL437: _temp436= _temp426.f2; if( _temp436 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL429;} else{ goto _LL430;} _LL430: _LL443: _temp442= _temp426.f1; if(
_temp442 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL441;} else{ goto _LL432;}
_LL441: _temp440= _temp426.f2; goto _LL431; _LL432: _LL447: _temp446= _temp426.f1;
goto _LL445; _LL445: _temp444= _temp426.f2; if( _temp444 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL433;} else{ goto _LL434;} _LL434: _LL453: _temp452= _temp426.f1; if((
unsigned int) _temp452 > 1u?*(( int*) _temp452) == Cyc_Absyn_Upper_b: 0){ _LL455:
_temp454=(( struct Cyc_Absyn_Upper_b_struct*) _temp452)->f1; goto _LL449;} else{
goto _LL427;} _LL449: _temp448= _temp426.f2; if(( unsigned int) _temp448 > 1u?*((
int*) _temp448) == Cyc_Absyn_Upper_b: 0){ _LL451: _temp450=(( struct Cyc_Absyn_Upper_b_struct*)
_temp448)->f1; goto _LL435;} else{ goto _LL427;} _LL429: return 0; _LL431:
return - 1; _LL433: return 1; _LL435: { int i1=( int) Cyc_Evexp_eval_const_uint_exp(
_temp454); int i2=( int) Cyc_Evexp_eval_const_uint_exp( _temp450); if( i1 == i2){
return 0;} if( i1 < i2){ return - 1;} return 1;} _LL427:;} static int Cyc_Tcutil_equal_att(
void* a1, void* a2){ if( a1 == a2){ return 1;}{ struct _tuple8 _temp457=({
struct _tuple8 _temp456; _temp456.f1= a1; _temp456.f2= a2; _temp456;}); void*
_temp467; int _temp469; void* _temp471; int _temp473; void* _temp475; int
_temp477; void* _temp479; int _temp481; void* _temp483; struct _tagged_string
_temp485; void* _temp487; struct _tagged_string _temp489; _LL459: _LL472:
_temp471= _temp457.f1; if(( unsigned int) _temp471 > 16u?*(( int*) _temp471) ==
Cyc_Absyn_Regparm_att: 0){ _LL474: _temp473=(( struct Cyc_Absyn_Regparm_att_struct*)
_temp471)->f1; goto _LL468;} else{ goto _LL461;} _LL468: _temp467= _temp457.f2;
if(( unsigned int) _temp467 > 16u?*(( int*) _temp467) == Cyc_Absyn_Regparm_att:
0){ _LL470: _temp469=(( struct Cyc_Absyn_Regparm_att_struct*) _temp467)->f1;
goto _LL460;} else{ goto _LL461;} _LL461: _LL480: _temp479= _temp457.f1; if((
unsigned int) _temp479 > 16u?*(( int*) _temp479) == Cyc_Absyn_Aligned_att: 0){
_LL482: _temp481=(( struct Cyc_Absyn_Aligned_att_struct*) _temp479)->f1; goto
_LL476;} else{ goto _LL463;} _LL476: _temp475= _temp457.f2; if(( unsigned int)
_temp475 > 16u?*(( int*) _temp475) == Cyc_Absyn_Aligned_att: 0){ _LL478:
_temp477=(( struct Cyc_Absyn_Aligned_att_struct*) _temp475)->f1; goto _LL462;}
else{ goto _LL463;} _LL463: _LL488: _temp487= _temp457.f1; if(( unsigned int)
_temp487 > 16u?*(( int*) _temp487) == Cyc_Absyn_Section_att: 0){ _LL490:
_temp489=(( struct Cyc_Absyn_Section_att_struct*) _temp487)->f1; goto _LL484;}
else{ goto _LL465;} _LL484: _temp483= _temp457.f2; if(( unsigned int) _temp483 >
16u?*(( int*) _temp483) == Cyc_Absyn_Section_att: 0){ _LL486: _temp485=(( struct
Cyc_Absyn_Section_att_struct*) _temp483)->f1; goto _LL464;} else{ goto _LL465;}
_LL465: goto _LL466; _LL460: _temp481= _temp473; _temp477= _temp469; goto _LL462;
_LL462: return _temp481 == _temp477; _LL464: return Cyc_String_strcmp( _temp489,
_temp485) == 0; _LL466: return 0; _LL458:;}} int Cyc_Tcutil_same_atts( struct
Cyc_List_List* a1, struct Cyc_List_List* a2){{ struct Cyc_List_List* a= a1; for(
0; a != 0; a=(( struct Cyc_List_List*) _check_null( a))->tl){ if( !(( int(*)(
int(* pred)( void*, void*), void* env, struct Cyc_List_List* x)) Cyc_List_exists_c)(
Cyc_Tcutil_equal_att,( void*)(( struct Cyc_List_List*) _check_null( a))->hd, a2)){
return 0;}}}{ struct Cyc_List_List* a= a2; for( 0; a != 0; a=(( struct Cyc_List_List*)
_check_null( a))->tl){ if( !(( int(*)( int(* pred)( void*, void*), void* env,
struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcutil_equal_att,( void*)((
struct Cyc_List_List*) _check_null( a))->hd, a1)){ return 0;}}} return 1;}
static void Cyc_Tcutil_normalize_effect( void* e){ e= Cyc_Tcutil_compress( e);{
void* _temp491= e; struct Cyc_List_List* _temp497; struct Cyc_List_List**
_temp499; _LL493: if(( unsigned int) _temp491 > 4u?*(( int*) _temp491) == Cyc_Absyn_JoinEff:
0){ _LL498: _temp497=(( struct Cyc_Absyn_JoinEff_struct*) _temp491)->f1;
_temp499=&(( struct Cyc_Absyn_JoinEff_struct*) _temp491)->f1; goto _LL494;}
else{ goto _LL495;} _LL495: goto _LL496; _LL494: { int nested_join= 0;{ struct
Cyc_List_List* effs=* _temp499; for( 0; effs != 0; effs=(( struct Cyc_List_List*)
_check_null( effs))->tl){ void* _temp500=( void*)(( struct Cyc_List_List*)
_check_null( effs))->hd; goto _LL501; _LL501: Cyc_Tcutil_normalize_effect(
_temp500);{ void* _temp502= Cyc_Tcutil_compress( _temp500); struct Cyc_List_List*
_temp508; _LL504: if(( unsigned int) _temp502 > 4u?*(( int*) _temp502) == Cyc_Absyn_JoinEff:
0){ _LL509: _temp508=(( struct Cyc_Absyn_JoinEff_struct*) _temp502)->f1; goto
_LL505;} else{ goto _LL506;} _LL506: goto _LL507; _LL505: nested_join= 1; goto
_LL503; _LL507: goto _LL503; _LL503:;}}} if( ! nested_join){ return;}{ struct
Cyc_List_List* effects= 0;{ struct Cyc_List_List* effs=* _temp499; for( 0; effs
!= 0; effs=(( struct Cyc_List_List*) _check_null( effs))->tl){ void* _temp510=
Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*) _check_null( effs))->hd);
struct Cyc_List_List* _temp516; _LL512: if(( unsigned int) _temp510 > 4u?*(( int*)
_temp510) == Cyc_Absyn_JoinEff: 0){ _LL517: _temp516=(( struct Cyc_Absyn_JoinEff_struct*)
_temp510)->f1; goto _LL513;} else{ goto _LL514;} _LL514: goto _LL515; _LL513:
effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_revappend)( _temp516, effects); goto _LL511; _LL515: effects=({
struct Cyc_List_List* _temp518=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp518->hd=( void*) _temp510; _temp518->tl= effects;
_temp518;}); goto _LL511; _LL511:;}}* _temp499=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( effects); goto _LL492;}} _LL496:
goto _LL492; _LL492:;}} struct _tuple9{ void* f1; struct Cyc_List_List* f2;
struct Cyc_List_List* f3; } ; static struct _tuple9* Cyc_Tcutil_get_effect_evar(
void* t){ void* _temp519= Cyc_Tcutil_compress( t); struct Cyc_List_List*
_temp527; struct Cyc_List_List _temp529; struct Cyc_List_List* _temp530; void*
_temp532; struct Cyc_Core_Opt* _temp534; int _temp536; struct Cyc_Core_Opt*
_temp538; struct Cyc_Core_Opt* _temp540; _LL521: if(( unsigned int) _temp519 > 4u?*((
int*) _temp519) == Cyc_Absyn_JoinEff: 0){ _LL528: _temp527=(( struct Cyc_Absyn_JoinEff_struct*)
_temp519)->f1; if( _temp527 == 0){ goto _LL523;} else{ _temp529=* _temp527;
_LL533: _temp532=( void*) _temp529.hd; goto _LL531; _LL531: _temp530= _temp529.tl;
goto _LL522;}} else{ goto _LL523;} _LL523: if(( unsigned int) _temp519 > 4u?*((
int*) _temp519) == Cyc_Absyn_Evar: 0){ _LL541: _temp540=(( struct Cyc_Absyn_Evar_struct*)
_temp519)->f1; goto _LL539; _LL539: _temp538=(( struct Cyc_Absyn_Evar_struct*)
_temp519)->f2; goto _LL537; _LL537: _temp536=(( struct Cyc_Absyn_Evar_struct*)
_temp519)->f3; goto _LL535; _LL535: _temp534=(( struct Cyc_Absyn_Evar_struct*)
_temp519)->f4; goto _LL524;} else{ goto _LL525;} _LL525: goto _LL526; _LL522: {
void* _temp542= Cyc_Tcutil_compress( _temp532); struct Cyc_Core_Opt* _temp548;
int _temp550; struct Cyc_Core_Opt* _temp552; struct Cyc_Core_Opt* _temp554;
_LL544: if(( unsigned int) _temp542 > 4u?*(( int*) _temp542) == Cyc_Absyn_Evar:
0){ _LL555: _temp554=(( struct Cyc_Absyn_Evar_struct*) _temp542)->f1; goto
_LL553; _LL553: _temp552=(( struct Cyc_Absyn_Evar_struct*) _temp542)->f2; goto
_LL551; _LL551: _temp550=(( struct Cyc_Absyn_Evar_struct*) _temp542)->f3; goto
_LL549; _LL549: _temp548=(( struct Cyc_Absyn_Evar_struct*) _temp542)->f4; goto
_LL545;} else{ goto _LL546;} _LL546: goto _LL547; _LL545: return({ struct
_tuple9* _temp556=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp556->f1= _temp532; _temp556->f2= _temp530; _temp556->f3=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp548))->v; _temp556;}); _LL547: return 0;
_LL543:;} _LL524: if( _temp540 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp540))->v !=( void*) Cyc_Absyn_EffKind){(( void(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp557="effect evar has wrong kind";
struct _tagged_string _temp558; _temp558.curr= _temp557; _temp558.base= _temp557;
_temp558.last_plus_one= _temp557 + 27; _temp558;}));} return({ struct _tuple9*
_temp559=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9)); _temp559->f1= t;
_temp559->f2= 0; _temp559->f3=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp534))->v; _temp559;}); _LL526: return 0; _LL520:;} static
struct Cyc_Core_Opt Cyc_Tcutil_ek={( void*)(( void*) 4u)}; int Cyc_Tcutil_region_in_effect(
int constrain, void* r, void* e){ r= Cyc_Tcutil_compress( r); if( r ==( void*)
Cyc_Absyn_HeapRgn){ return 1;}{ void* _temp560= Cyc_Tcutil_compress( e); void*
_temp570; struct Cyc_List_List* _temp572; struct Cyc_Core_Opt* _temp574; int
_temp576; struct Cyc_Core_Opt* _temp578; struct Cyc_Core_Opt** _temp580; struct
Cyc_Core_Opt* _temp581; _LL562: if(( unsigned int) _temp560 > 4u?*(( int*)
_temp560) == Cyc_Absyn_AccessEff: 0){ _LL571: _temp570=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp560)->f1; goto _LL563;} else{ goto _LL564;} _LL564: if(( unsigned int)
_temp560 > 4u?*(( int*) _temp560) == Cyc_Absyn_JoinEff: 0){ _LL573: _temp572=((
struct Cyc_Absyn_JoinEff_struct*) _temp560)->f1; goto _LL565;} else{ goto _LL566;}
_LL566: if(( unsigned int) _temp560 > 4u?*(( int*) _temp560) == Cyc_Absyn_Evar:
0){ _LL582: _temp581=(( struct Cyc_Absyn_Evar_struct*) _temp560)->f1; goto
_LL579; _LL579: _temp578=(( struct Cyc_Absyn_Evar_struct*) _temp560)->f2;
_temp580=&(( struct Cyc_Absyn_Evar_struct*) _temp560)->f2; goto _LL577; _LL577:
_temp576=(( struct Cyc_Absyn_Evar_struct*) _temp560)->f3; goto _LL575; _LL575:
_temp574=(( struct Cyc_Absyn_Evar_struct*) _temp560)->f4; goto _LL567;} else{
goto _LL568;} _LL568: goto _LL569; _LL563: if( constrain){ return Cyc_Tcutil_unify(
r, _temp570);} Cyc_Tcutil_compress( _temp570); if( r == _temp570){ return 1;}{
struct _tuple8 _temp584=({ struct _tuple8 _temp583; _temp583.f1= r; _temp583.f2=
_temp570; _temp583;}); void* _temp590; struct Cyc_Absyn_Tvar* _temp592; void*
_temp594; struct Cyc_Absyn_Tvar* _temp596; _LL586: _LL595: _temp594= _temp584.f1;
if(( unsigned int) _temp594 > 4u?*(( int*) _temp594) == Cyc_Absyn_VarType: 0){
_LL597: _temp596=(( struct Cyc_Absyn_VarType_struct*) _temp594)->f1; goto _LL591;}
else{ goto _LL588;} _LL591: _temp590= _temp584.f2; if(( unsigned int) _temp590 >
4u?*(( int*) _temp590) == Cyc_Absyn_VarType: 0){ _LL593: _temp592=(( struct Cyc_Absyn_VarType_struct*)
_temp590)->f1; goto _LL587;} else{ goto _LL588;} _LL588: goto _LL589; _LL587:
return Cyc_Absyn_tvar_cmp( _temp596, _temp592) == 0; _LL589: return 0; _LL585:;}
_LL565: for( 0; _temp572 != 0; _temp572=(( struct Cyc_List_List*) _check_null(
_temp572))->tl){ if( Cyc_Tcutil_region_in_effect( constrain, r,( void*)(( struct
Cyc_List_List*) _check_null( _temp572))->hd)){ return 1;}} return 0; _LL567: if(
_temp581 == 0? 1:( void*)(( struct Cyc_Core_Opt*) _check_null( _temp581))->v !=(
void*) Cyc_Absyn_EffKind){(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
unsigned char* _temp598="effect evar has wrong kind"; struct _tagged_string
_temp599; _temp599.curr= _temp598; _temp599.base= _temp598; _temp599.last_plus_one=
_temp598 + 27; _temp599;}));} if( ! constrain){ return 0;}{ void* _temp600= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek, _temp574); goto _LL601; _LL601: Cyc_Tcutil_occurs(
_temp600,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp574))->v,
r);{ struct Cyc_Absyn_JoinEff_struct* _temp608=({ struct Cyc_Absyn_JoinEff_struct*
_temp602=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp602[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp603; _temp603.tag= Cyc_Absyn_JoinEff;
_temp603.f1=({ struct Cyc_List_List* _temp604=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp604->hd=( void*) _temp600; _temp604->tl=({
struct Cyc_List_List* _temp605=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp605->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp606=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp606[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp607; _temp607.tag= Cyc_Absyn_AccessEff;
_temp607.f1=( void*) r; _temp607;}); _temp606;})); _temp605->tl= 0; _temp605;});
_temp604;}); _temp603;}); _temp602;}); goto _LL609; _LL609:* _temp580=({ struct
Cyc_Core_Opt* _temp610=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp610->v=( void*)(( void*) _temp608); _temp610;}); return 1;}} _LL569: return
0; _LL561:;}} static int Cyc_Tcutil_variable_in_effect( int constrain, struct
Cyc_Absyn_Tvar* v, void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp611= e;
struct Cyc_Absyn_Tvar* _temp621; struct Cyc_List_List* _temp623; struct Cyc_Core_Opt*
_temp625; int _temp627; struct Cyc_Core_Opt* _temp629; struct Cyc_Core_Opt**
_temp631; struct Cyc_Core_Opt* _temp632; _LL613: if(( unsigned int) _temp611 > 4u?*((
int*) _temp611) == Cyc_Absyn_VarType: 0){ _LL622: _temp621=(( struct Cyc_Absyn_VarType_struct*)
_temp611)->f1; goto _LL614;} else{ goto _LL615;} _LL615: if(( unsigned int)
_temp611 > 4u?*(( int*) _temp611) == Cyc_Absyn_JoinEff: 0){ _LL624: _temp623=((
struct Cyc_Absyn_JoinEff_struct*) _temp611)->f1; goto _LL616;} else{ goto _LL617;}
_LL617: if(( unsigned int) _temp611 > 4u?*(( int*) _temp611) == Cyc_Absyn_Evar:
0){ _LL633: _temp632=(( struct Cyc_Absyn_Evar_struct*) _temp611)->f1; goto
_LL630; _LL630: _temp629=(( struct Cyc_Absyn_Evar_struct*) _temp611)->f2;
_temp631=&(( struct Cyc_Absyn_Evar_struct*) _temp611)->f2; goto _LL628; _LL628:
_temp627=(( struct Cyc_Absyn_Evar_struct*) _temp611)->f3; goto _LL626; _LL626:
_temp625=(( struct Cyc_Absyn_Evar_struct*) _temp611)->f4; goto _LL618;} else{
goto _LL619;} _LL619: goto _LL620; _LL614: return Cyc_Absyn_tvar_cmp( v,
_temp621) == 0; _LL616: for( 0; _temp623 != 0; _temp623=(( struct Cyc_List_List*)
_check_null( _temp623))->tl){ if( Cyc_Tcutil_variable_in_effect( constrain, v,(
void*)(( struct Cyc_List_List*) _check_null( _temp623))->hd)){ return 1;}}
return 0; _LL618: if( _temp632 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp632))->v !=( void*) Cyc_Absyn_EffKind){(( void(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp634="effect evar has wrong kind";
struct _tagged_string _temp635; _temp635.curr= _temp634; _temp635.base= _temp634;
_temp635.last_plus_one= _temp634 + 27; _temp635;}));}{ void* _temp636= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek, _temp625); goto _LL637; _LL637: if( !((
int(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp625))->v, v)){
return 0;}{ struct Cyc_Absyn_JoinEff_struct* _temp644=({ struct Cyc_Absyn_JoinEff_struct*
_temp638=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp638[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp639; _temp639.tag= Cyc_Absyn_JoinEff;
_temp639.f1=({ struct Cyc_List_List* _temp640=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp640->hd=( void*) _temp636; _temp640->tl=({
struct Cyc_List_List* _temp641=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp641->hd=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp642=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp642[ 0]=({ struct Cyc_Absyn_VarType_struct _temp643; _temp643.tag= Cyc_Absyn_VarType;
_temp643.f1= v; _temp643;}); _temp642;})); _temp641->tl= 0; _temp641;});
_temp640;}); _temp639;}); _temp638;}); goto _LL645; _LL645:* _temp631=({ struct
Cyc_Core_Opt* _temp646=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp646->v=( void*)(( void*) _temp644); _temp646;}); return 1;}} _LL620: return
0; _LL612:;}} static int Cyc_Tcutil_evar_in_effect( void* evar, void* e){ e= Cyc_Tcutil_compress(
e);{ void* _temp647= e; struct Cyc_List_List* _temp655; struct Cyc_Core_Opt*
_temp657; int _temp659; struct Cyc_Core_Opt* _temp661; struct Cyc_Core_Opt*
_temp663; _LL649: if(( unsigned int) _temp647 > 4u?*(( int*) _temp647) == Cyc_Absyn_JoinEff:
0){ _LL656: _temp655=(( struct Cyc_Absyn_JoinEff_struct*) _temp647)->f1; goto
_LL650;} else{ goto _LL651;} _LL651: if(( unsigned int) _temp647 > 4u?*(( int*)
_temp647) == Cyc_Absyn_Evar: 0){ _LL664: _temp663=(( struct Cyc_Absyn_Evar_struct*)
_temp647)->f1; goto _LL662; _LL662: _temp661=(( struct Cyc_Absyn_Evar_struct*)
_temp647)->f2; goto _LL660; _LL660: _temp659=(( struct Cyc_Absyn_Evar_struct*)
_temp647)->f3; goto _LL658; _LL658: _temp657=(( struct Cyc_Absyn_Evar_struct*)
_temp647)->f4; goto _LL652;} else{ goto _LL653;} _LL653: goto _LL654; _LL650:
for( 0; _temp655 != 0; _temp655=(( struct Cyc_List_List*) _check_null( _temp655))->tl){
if( Cyc_Tcutil_evar_in_effect( evar,( void*)(( struct Cyc_List_List*)
_check_null( _temp655))->hd)){ return 1;}} return 0; _LL652: return evar == e;
_LL654: return 0; _LL648:;}} int Cyc_Tcutil_subset_effect( int set_to_empty,
void* e1, void* e2){{ struct _tuple9* _temp665= Cyc_Tcutil_get_effect_evar( e2);
struct _tuple9 _temp671; struct Cyc_List_List* _temp672; struct Cyc_List_List*
_temp674; void* _temp676; _LL667: if( _temp665 == 0){ goto _LL669;} else{
_temp671=* _temp665; _LL677: _temp676= _temp671.f1; goto _LL675; _LL675:
_temp674= _temp671.f2; goto _LL673; _LL673: _temp672= _temp671.f3; goto _LL668;}
_LL669: goto _LL670; _LL668: { void* _temp679= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,({ struct Cyc_Core_Opt* _temp678=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp678->v=( void*) _temp672; _temp678;})); goto
_LL680; _LL680: { struct Cyc_Absyn_JoinEff_struct* _temp685=({ struct Cyc_Absyn_JoinEff_struct*
_temp681=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp681[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp682; _temp682.tag= Cyc_Absyn_JoinEff;
_temp682.f1=({ struct Cyc_List_List* _temp683=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp683->hd=( void*) _temp679; _temp683->tl=({
struct Cyc_List_List* _temp684=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp684->hd=( void*) e1; _temp684->tl= 0; _temp684;});
_temp683;}); _temp682;}); _temp681;}); goto _LL686; _LL686: return Cyc_Tcutil_unify(
_temp676,( void*) _temp685);}} _LL670: goto _LL666; _LL666:;}{ void* _temp687=
Cyc_Tcutil_compress( e1); struct Cyc_List_List* _temp699; void* _temp701; struct
Cyc_Absyn_Tvar* _temp703; struct Cyc_Core_Opt* _temp705; int _temp707; struct
Cyc_Core_Opt* _temp709; struct Cyc_Core_Opt** _temp711; struct Cyc_Core_Opt*
_temp712; _LL689: if(( unsigned int) _temp687 > 4u?*(( int*) _temp687) == Cyc_Absyn_JoinEff:
0){ _LL700: _temp699=(( struct Cyc_Absyn_JoinEff_struct*) _temp687)->f1; goto
_LL690;} else{ goto _LL691;} _LL691: if(( unsigned int) _temp687 > 4u?*(( int*)
_temp687) == Cyc_Absyn_AccessEff: 0){ _LL702: _temp701=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp687)->f1; goto _LL692;} else{ goto _LL693;} _LL693: if(( unsigned int)
_temp687 > 4u?*(( int*) _temp687) == Cyc_Absyn_VarType: 0){ _LL704: _temp703=((
struct Cyc_Absyn_VarType_struct*) _temp687)->f1; goto _LL694;} else{ goto _LL695;}
_LL695: if(( unsigned int) _temp687 > 4u?*(( int*) _temp687) == Cyc_Absyn_Evar:
0){ _LL713: _temp712=(( struct Cyc_Absyn_Evar_struct*) _temp687)->f1; goto
_LL710; _LL710: _temp709=(( struct Cyc_Absyn_Evar_struct*) _temp687)->f2;
_temp711=&(( struct Cyc_Absyn_Evar_struct*) _temp687)->f2; goto _LL708; _LL708:
_temp707=(( struct Cyc_Absyn_Evar_struct*) _temp687)->f3; goto _LL706; _LL706:
_temp705=(( struct Cyc_Absyn_Evar_struct*) _temp687)->f4; goto _LL696;} else{
goto _LL697;} _LL697: goto _LL698; _LL690: for( 0; _temp699 != 0; _temp699=((
struct Cyc_List_List*) _check_null( _temp699))->tl){ if( ! Cyc_Tcutil_subset_effect(
set_to_empty,( void*)(( struct Cyc_List_List*) _check_null( _temp699))->hd, e2)){
return 0;}} return 1; _LL692: return( Cyc_Tcutil_region_in_effect( 0, _temp701,
e2)? 1: Cyc_Tcutil_region_in_effect( 1, _temp701, e2))? 1: Cyc_Tcutil_unify(
_temp701,( void*) Cyc_Absyn_HeapRgn); _LL694: return Cyc_Tcutil_variable_in_effect(
0, _temp703, e2)? 1: Cyc_Tcutil_variable_in_effect( 1, _temp703, e2); _LL696:
if( ! Cyc_Tcutil_evar_in_effect( e1, e2)){ if( set_to_empty){* _temp711=({
struct Cyc_Core_Opt* _temp714=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp714->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp715=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp715[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp716; _temp716.tag= Cyc_Absyn_JoinEff;
_temp716.f1= 0; _temp716;}); _temp715;})); _temp714;});} else{ Cyc_Tcutil_occurs(
e1,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp705))->v,
e2);* _temp711=({ struct Cyc_Core_Opt* _temp717=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp717->v=( void*) e2; _temp717;});}}
return 1; _LL698: return(( int(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
struct _tagged_string _temp718= Cyc_Absynpp_typ2string( e1); xprintf("subset_effect: bad effect: %.*s",
_temp718.last_plus_one - _temp718.curr, _temp718.curr);})); _LL688:;}} struct
_tuple10{ struct _tuple9* f1; struct _tuple9* f2; } ; static int Cyc_Tcutil_unify_effect(
void* e1, void* e2){ e1= Cyc_Tcutil_compress( e1); e2= Cyc_Tcutil_compress( e2);
Cyc_Tcutil_normalize_effect( e1); Cyc_Tcutil_normalize_effect( e2);{ struct
_tuple10 _temp720=({ struct _tuple10 _temp719; _temp719.f1= Cyc_Tcutil_get_effect_evar(
e1); _temp719.f2= Cyc_Tcutil_get_effect_evar( e2); _temp719;}); struct _tuple9*
_temp726; struct _tuple9 _temp728; struct Cyc_List_List* _temp729; struct Cyc_List_List*
_temp731; void* _temp733; struct _tuple9* _temp735; struct _tuple9 _temp737;
struct Cyc_List_List* _temp738; struct Cyc_List_List* _temp740; void* _temp742;
_LL722: _LL736: _temp735= _temp720.f1; if( _temp735 == 0){ goto _LL724;} else{
_temp737=* _temp735; _LL743: _temp742= _temp737.f1; goto _LL741; _LL741:
_temp740= _temp737.f2; goto _LL739; _LL739: _temp738= _temp737.f3; goto _LL727;}
_LL727: _temp726= _temp720.f2; if( _temp726 == 0){ goto _LL724;} else{ _temp728=*
_temp726; _LL734: _temp733= _temp728.f1; goto _LL732; _LL732: _temp731= _temp728.f2;
goto _LL730; _LL730: _temp729= _temp728.f3; goto _LL723;} _LL724: goto _LL725;
_LL723: { struct Cyc_List_List* _temp744= 0; goto _LL745; _LL745: for( 0;
_temp738 != 0; _temp738=(( struct Cyc_List_List*) _check_null( _temp738))->tl){
if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,
_temp729,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp738))->hd)){ _temp744=({ struct Cyc_List_List* _temp746=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp746->hd=( void*)(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp738))->hd); _temp746->tl= _temp744;
_temp746;});}}{ void* _temp748= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,({
struct Cyc_Core_Opt* _temp747=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp747->v=( void*) _temp744; _temp747;})); goto _LL749; _LL749: {
struct Cyc_Absyn_JoinEff_struct* _temp753=({ struct Cyc_Absyn_JoinEff_struct*
_temp750=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp750[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp751; _temp751.tag= Cyc_Absyn_JoinEff;
_temp751.f1=({ struct Cyc_List_List* _temp752=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp752->hd=( void*) _temp748; _temp752->tl=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp740, _temp731); _temp752;}); _temp751;}); _temp750;}); goto _LL754; _LL754:
return Cyc_Tcutil_unify( _temp742,( void*) _temp753)? Cyc_Tcutil_unify( _temp733,(
void*) _temp753): 0;}}} _LL725: return Cyc_Tcutil_subset_effect( 0, e1, e2)? Cyc_Tcutil_subset_effect(
0, e2, e1): 0; _LL721:;}} struct _tuple11{ struct Cyc_Absyn_VarargInfo* f1;
struct Cyc_Absyn_VarargInfo* f2; } ; struct _tuple12{ struct Cyc_Core_Opt* f1;
struct Cyc_Core_Opt* f2; } ; void Cyc_Tcutil_unify_it( void* t1, void* t2){ t1=
Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2); if( t1 == t2){ return;}{
void* _temp755= t1; struct Cyc_Core_Opt* _temp761; int _temp763; struct Cyc_Core_Opt*
_temp765; struct Cyc_Core_Opt** _temp767; struct Cyc_Core_Opt* _temp768; _LL757:
if(( unsigned int) _temp755 > 4u?*(( int*) _temp755) == Cyc_Absyn_Evar: 0){
_LL769: _temp768=(( struct Cyc_Absyn_Evar_struct*) _temp755)->f1; goto _LL766;
_LL766: _temp765=(( struct Cyc_Absyn_Evar_struct*) _temp755)->f2; _temp767=&((
struct Cyc_Absyn_Evar_struct*) _temp755)->f2; goto _LL764; _LL764: _temp763=((
struct Cyc_Absyn_Evar_struct*) _temp755)->f3; goto _LL762; _LL762: _temp761=((
struct Cyc_Absyn_Evar_struct*) _temp755)->f4; goto _LL758;} else{ goto _LL759;}
_LL759: goto _LL760; _LL758: Cyc_Tcutil_occurs( t1,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp761))->v, t2);{ void* _temp770= Cyc_Tcutil_typ_kind(
t2); goto _LL771; _LL771: if( Cyc_Tcutil_kind_leq( _temp770,( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp768))->v)){* _temp767=({ struct Cyc_Core_Opt*
_temp772=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp772->v=( void*) t2; _temp772;}); return;} else{{ void* _temp773= t2; struct
Cyc_Core_Opt* _temp781; int _temp783; struct Cyc_Core_Opt* _temp785; struct Cyc_Core_Opt**
_temp787; struct Cyc_Core_Opt* _temp788; struct Cyc_Absyn_PtrInfo _temp791;
_LL775: if(( unsigned int) _temp773 > 4u?*(( int*) _temp773) == Cyc_Absyn_Evar:
0){ _LL789: _temp788=(( struct Cyc_Absyn_Evar_struct*) _temp773)->f1; goto
_LL786; _LL786: _temp785=(( struct Cyc_Absyn_Evar_struct*) _temp773)->f2;
_temp787=&(( struct Cyc_Absyn_Evar_struct*) _temp773)->f2; goto _LL784; _LL784:
_temp783=(( struct Cyc_Absyn_Evar_struct*) _temp773)->f3; goto _LL782; _LL782:
_temp781=(( struct Cyc_Absyn_Evar_struct*) _temp773)->f4; goto _LL776;} else{
goto _LL777;} _LL777: if(( unsigned int) _temp773 > 4u?*(( int*) _temp773) ==
Cyc_Absyn_PointerType: 0){ _LL792: _temp791=(( struct Cyc_Absyn_PointerType_struct*)
_temp773)->f1; goto _LL790;} else{ goto _LL779;} _LL790: if(( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp768))->v ==( void*) Cyc_Absyn_BoxKind){ goto
_LL778;} else{ goto _LL779;} _LL779: goto _LL780; _LL776: { struct Cyc_List_List*
_temp793=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp761))->v;
goto _LL794; _LL794:{ struct Cyc_List_List* s2=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( _temp781))->v; for( 0; s2 != 0; s2=(( struct Cyc_List_List*)
_check_null( s2))->tl){ if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, _temp793,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( s2))->hd)){( void) _throw(( void*) Cyc_Tcutil_Unify);}}} if( Cyc_Tcutil_kind_leq((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp768))->v, _temp770)){* _temp787=({
struct Cyc_Core_Opt* _temp795=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp795->v=( void*) t1; _temp795;}); return;} goto _LL774;}
_LL778: { struct Cyc_Absyn_Conref* _temp796=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp791.bounds); goto
_LL797; _LL797:{ void* _temp798=( void*) _temp796->v; _LL800: if( _temp798 ==(
void*) Cyc_Absyn_No_constr){ goto _LL801;} else{ goto _LL802;} _LL802: goto
_LL803; _LL801:( void*)( _temp796->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp804=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp804[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp805; _temp805.tag= Cyc_Absyn_Eq_constr;
_temp805.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp806=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp806[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp807; _temp807.tag= Cyc_Absyn_Upper_b;
_temp807.f1= Cyc_Absyn_signed_int_exp( 1, 0); _temp807;}); _temp806;}));
_temp805;}); _temp804;})));* _temp767=({ struct Cyc_Core_Opt* _temp808=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp808->v=( void*) t2;
_temp808;}); return; _LL803: goto _LL799; _LL799:;} goto _LL774;} _LL780: goto
_LL774; _LL774:;}( void) _throw(( void*) Cyc_Tcutil_Unify);}} _LL760: goto
_LL756; _LL756:;}{ struct _tuple8 _temp810=({ struct _tuple8 _temp809; _temp809.f1=
t2; _temp809.f2= t1; _temp809;}); void* _temp860; void* _temp862; struct Cyc_Core_Opt*
_temp864; int _temp866; struct Cyc_Core_Opt* _temp868; struct Cyc_Core_Opt*
_temp870; void* _temp872; void* _temp874; void* _temp876; struct Cyc_Absyn_Tvar*
_temp878; void* _temp880; struct Cyc_Absyn_Tvar* _temp882; void* _temp884;
struct Cyc_Absyn_Structdecl** _temp886; struct Cyc_List_List* _temp888; struct
_tuple1* _temp890; void* _temp892; struct Cyc_Absyn_Structdecl** _temp894;
struct Cyc_List_List* _temp896; struct _tuple1* _temp898; void* _temp900; struct
Cyc_Absyn_Enumdecl* _temp902; struct _tuple1* _temp904; void* _temp906; struct
Cyc_Absyn_Enumdecl* _temp908; struct _tuple1* _temp910; void* _temp912; struct
Cyc_Absyn_Uniondecl** _temp914; struct Cyc_List_List* _temp916; struct _tuple1*
_temp918; void* _temp920; struct Cyc_Absyn_Uniondecl** _temp922; struct Cyc_List_List*
_temp924; struct _tuple1* _temp926; void* _temp928; struct Cyc_Absyn_TunionInfo
_temp930; void* _temp932; struct Cyc_List_List* _temp934; void* _temp936; struct
Cyc_Absyn_Tuniondecl* _temp938; void* _temp940; struct Cyc_Absyn_TunionInfo
_temp942; void* _temp944; struct Cyc_List_List* _temp946; void* _temp948; struct
Cyc_Absyn_Tuniondecl* _temp950; void* _temp952; struct Cyc_Absyn_TunionFieldInfo
_temp954; struct Cyc_List_List* _temp956; void* _temp958; struct Cyc_Absyn_Tunionfield*
_temp960; struct Cyc_Absyn_Tuniondecl* _temp962; void* _temp964; struct Cyc_Absyn_TunionFieldInfo
_temp966; struct Cyc_List_List* _temp968; void* _temp970; struct Cyc_Absyn_Tunionfield*
_temp972; struct Cyc_Absyn_Tuniondecl* _temp974; void* _temp976; struct Cyc_Absyn_PtrInfo
_temp978; struct Cyc_Absyn_Conref* _temp980; struct Cyc_Absyn_Tqual _temp982;
struct Cyc_Absyn_Conref* _temp984; void* _temp986; void* _temp988; void*
_temp990; struct Cyc_Absyn_PtrInfo _temp992; struct Cyc_Absyn_Conref* _temp994;
struct Cyc_Absyn_Tqual _temp996; struct Cyc_Absyn_Conref* _temp998; void*
_temp1000; void* _temp1002; void* _temp1004; void* _temp1006; void* _temp1008;
void* _temp1010; void* _temp1012; void* _temp1014; void* _temp1016; void*
_temp1018; void* _temp1020; void* _temp1022; void* _temp1024; struct Cyc_Absyn_Exp*
_temp1026; struct Cyc_Absyn_Tqual _temp1028; void* _temp1030; void* _temp1032;
struct Cyc_Absyn_Exp* _temp1034; struct Cyc_Absyn_Tqual _temp1036; void*
_temp1038; void* _temp1040; struct Cyc_Absyn_FnInfo _temp1042; struct Cyc_List_List*
_temp1044; struct Cyc_Absyn_VarargInfo* _temp1046; int _temp1048; struct Cyc_List_List*
_temp1050; void* _temp1052; struct Cyc_Core_Opt* _temp1054; struct Cyc_List_List*
_temp1056; void* _temp1058; struct Cyc_Absyn_FnInfo _temp1060; struct Cyc_List_List*
_temp1062; struct Cyc_Absyn_VarargInfo* _temp1064; int _temp1066; struct Cyc_List_List*
_temp1068; void* _temp1070; struct Cyc_Core_Opt* _temp1072; struct Cyc_List_List*
_temp1074; void* _temp1076; struct Cyc_List_List* _temp1078; void* _temp1080;
struct Cyc_List_List* _temp1082; void* _temp1084; struct Cyc_List_List*
_temp1086; void* _temp1088; struct Cyc_List_List* _temp1090; void* _temp1092;
struct Cyc_List_List* _temp1094; void* _temp1096; struct Cyc_List_List*
_temp1098; void* _temp1100; void* _temp1102; void* _temp1104; void* _temp1106;
void* _temp1108; void* _temp1110; void* _temp1112; void* _temp1114; struct Cyc_List_List*
_temp1116; void* _temp1118; struct Cyc_List_List* _temp1120; void* _temp1122;
void* _temp1124; void* _temp1126; void* _temp1128; void* _temp1130; void*
_temp1132; void* _temp1134; _LL812: _LL863: _temp862= _temp810.f1; if((
unsigned int) _temp862 > 4u?*(( int*) _temp862) == Cyc_Absyn_Evar: 0){ _LL871:
_temp870=(( struct Cyc_Absyn_Evar_struct*) _temp862)->f1; goto _LL869; _LL869:
_temp868=(( struct Cyc_Absyn_Evar_struct*) _temp862)->f2; goto _LL867; _LL867:
_temp866=(( struct Cyc_Absyn_Evar_struct*) _temp862)->f3; goto _LL865; _LL865:
_temp864=(( struct Cyc_Absyn_Evar_struct*) _temp862)->f4; goto _LL861;} else{
goto _LL814;} _LL861: _temp860= _temp810.f2; goto _LL813; _LL814: _LL875:
_temp874= _temp810.f1; if( _temp874 ==( void*) Cyc_Absyn_VoidType){ goto _LL873;}
else{ goto _LL816;} _LL873: _temp872= _temp810.f2; if( _temp872 ==( void*) Cyc_Absyn_VoidType){
goto _LL815;} else{ goto _LL816;} _LL816: _LL881: _temp880= _temp810.f1; if((
unsigned int) _temp880 > 4u?*(( int*) _temp880) == Cyc_Absyn_VarType: 0){ _LL883:
_temp882=(( struct Cyc_Absyn_VarType_struct*) _temp880)->f1; goto _LL877;} else{
goto _LL818;} _LL877: _temp876= _temp810.f2; if(( unsigned int) _temp876 > 4u?*((
int*) _temp876) == Cyc_Absyn_VarType: 0){ _LL879: _temp878=(( struct Cyc_Absyn_VarType_struct*)
_temp876)->f1; goto _LL817;} else{ goto _LL818;} _LL818: _LL893: _temp892=
_temp810.f1; if(( unsigned int) _temp892 > 4u?*(( int*) _temp892) == Cyc_Absyn_StructType:
0){ _LL899: _temp898=(( struct Cyc_Absyn_StructType_struct*) _temp892)->f1; goto
_LL897; _LL897: _temp896=(( struct Cyc_Absyn_StructType_struct*) _temp892)->f2;
goto _LL895; _LL895: _temp894=(( struct Cyc_Absyn_StructType_struct*) _temp892)->f3;
goto _LL885;} else{ goto _LL820;} _LL885: _temp884= _temp810.f2; if((
unsigned int) _temp884 > 4u?*(( int*) _temp884) == Cyc_Absyn_StructType: 0){
_LL891: _temp890=(( struct Cyc_Absyn_StructType_struct*) _temp884)->f1; goto
_LL889; _LL889: _temp888=(( struct Cyc_Absyn_StructType_struct*) _temp884)->f2;
goto _LL887; _LL887: _temp886=(( struct Cyc_Absyn_StructType_struct*) _temp884)->f3;
goto _LL819;} else{ goto _LL820;} _LL820: _LL907: _temp906= _temp810.f1; if((
unsigned int) _temp906 > 4u?*(( int*) _temp906) == Cyc_Absyn_EnumType: 0){
_LL911: _temp910=(( struct Cyc_Absyn_EnumType_struct*) _temp906)->f1; goto
_LL909; _LL909: _temp908=(( struct Cyc_Absyn_EnumType_struct*) _temp906)->f2;
goto _LL901;} else{ goto _LL822;} _LL901: _temp900= _temp810.f2; if((
unsigned int) _temp900 > 4u?*(( int*) _temp900) == Cyc_Absyn_EnumType: 0){
_LL905: _temp904=(( struct Cyc_Absyn_EnumType_struct*) _temp900)->f1; goto
_LL903; _LL903: _temp902=(( struct Cyc_Absyn_EnumType_struct*) _temp900)->f2;
goto _LL821;} else{ goto _LL822;} _LL822: _LL921: _temp920= _temp810.f1; if((
unsigned int) _temp920 > 4u?*(( int*) _temp920) == Cyc_Absyn_UnionType: 0){
_LL927: _temp926=(( struct Cyc_Absyn_UnionType_struct*) _temp920)->f1; goto
_LL925; _LL925: _temp924=(( struct Cyc_Absyn_UnionType_struct*) _temp920)->f2;
goto _LL923; _LL923: _temp922=(( struct Cyc_Absyn_UnionType_struct*) _temp920)->f3;
goto _LL913;} else{ goto _LL824;} _LL913: _temp912= _temp810.f2; if((
unsigned int) _temp912 > 4u?*(( int*) _temp912) == Cyc_Absyn_UnionType: 0){
_LL919: _temp918=(( struct Cyc_Absyn_UnionType_struct*) _temp912)->f1; goto
_LL917; _LL917: _temp916=(( struct Cyc_Absyn_UnionType_struct*) _temp912)->f2;
goto _LL915; _LL915: _temp914=(( struct Cyc_Absyn_UnionType_struct*) _temp912)->f3;
goto _LL823;} else{ goto _LL824;} _LL824: _LL941: _temp940= _temp810.f1; if((
unsigned int) _temp940 > 4u?*(( int*) _temp940) == Cyc_Absyn_TunionType: 0){
_LL943: _temp942=(( struct Cyc_Absyn_TunionType_struct*) _temp940)->f1; _LL949:
_temp948=( void*) _temp942.tunion_info; if(*(( int*) _temp948) == Cyc_Absyn_KnownTunion){
_LL951: _temp950=(( struct Cyc_Absyn_KnownTunion_struct*) _temp948)->f1; goto
_LL947;} else{ goto _LL826;} _LL947: _temp946= _temp942.targs; goto _LL945;
_LL945: _temp944=( void*) _temp942.rgn; goto _LL929;} else{ goto _LL826;} _LL929:
_temp928= _temp810.f2; if(( unsigned int) _temp928 > 4u?*(( int*) _temp928) ==
Cyc_Absyn_TunionType: 0){ _LL931: _temp930=(( struct Cyc_Absyn_TunionType_struct*)
_temp928)->f1; _LL937: _temp936=( void*) _temp930.tunion_info; if(*(( int*)
_temp936) == Cyc_Absyn_KnownTunion){ _LL939: _temp938=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp936)->f1; goto _LL935;} else{ goto _LL826;} _LL935: _temp934= _temp930.targs;
goto _LL933; _LL933: _temp932=( void*) _temp930.rgn; goto _LL825;} else{ goto
_LL826;} _LL826: _LL965: _temp964= _temp810.f1; if(( unsigned int) _temp964 > 4u?*((
int*) _temp964) == Cyc_Absyn_TunionFieldType: 0){ _LL967: _temp966=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp964)->f1; _LL971: _temp970=( void*) _temp966.field_info; if(*(( int*)
_temp970) == Cyc_Absyn_KnownTunionfield){ _LL975: _temp974=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp970)->f1; goto _LL973; _LL973: _temp972=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp970)->f2; goto _LL969;} else{ goto _LL828;} _LL969: _temp968= _temp966.targs;
goto _LL953;} else{ goto _LL828;} _LL953: _temp952= _temp810.f2; if((
unsigned int) _temp952 > 4u?*(( int*) _temp952) == Cyc_Absyn_TunionFieldType: 0){
_LL955: _temp954=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp952)->f1;
_LL959: _temp958=( void*) _temp954.field_info; if(*(( int*) _temp958) == Cyc_Absyn_KnownTunionfield){
_LL963: _temp962=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp958)->f1;
goto _LL961; _LL961: _temp960=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp958)->f2; goto _LL957;} else{ goto _LL828;} _LL957: _temp956= _temp954.targs;
goto _LL827;} else{ goto _LL828;} _LL828: _LL991: _temp990= _temp810.f1; if((
unsigned int) _temp990 > 4u?*(( int*) _temp990) == Cyc_Absyn_PointerType: 0){
_LL993: _temp992=(( struct Cyc_Absyn_PointerType_struct*) _temp990)->f1; _LL1003:
_temp1002=( void*) _temp992.elt_typ; goto _LL1001; _LL1001: _temp1000=( void*)
_temp992.rgn_typ; goto _LL999; _LL999: _temp998= _temp992.nullable; goto _LL997;
_LL997: _temp996= _temp992.tq; goto _LL995; _LL995: _temp994= _temp992.bounds;
goto _LL977;} else{ goto _LL830;} _LL977: _temp976= _temp810.f2; if((
unsigned int) _temp976 > 4u?*(( int*) _temp976) == Cyc_Absyn_PointerType: 0){
_LL979: _temp978=(( struct Cyc_Absyn_PointerType_struct*) _temp976)->f1; _LL989:
_temp988=( void*) _temp978.elt_typ; goto _LL987; _LL987: _temp986=( void*)
_temp978.rgn_typ; goto _LL985; _LL985: _temp984= _temp978.nullable; goto _LL983;
_LL983: _temp982= _temp978.tq; goto _LL981; _LL981: _temp980= _temp978.bounds;
goto _LL829;} else{ goto _LL830;} _LL830: _LL1011: _temp1010= _temp810.f1; if((
unsigned int) _temp1010 > 4u?*(( int*) _temp1010) == Cyc_Absyn_IntType: 0){
_LL1015: _temp1014=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1010)->f1;
goto _LL1013; _LL1013: _temp1012=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1010)->f2; goto _LL1005;} else{ goto _LL832;} _LL1005: _temp1004= _temp810.f2;
if(( unsigned int) _temp1004 > 4u?*(( int*) _temp1004) == Cyc_Absyn_IntType: 0){
_LL1009: _temp1008=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1004)->f1;
goto _LL1007; _LL1007: _temp1006=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1004)->f2; goto _LL831;} else{ goto _LL832;} _LL832: _LL1019: _temp1018=
_temp810.f1; if( _temp1018 ==( void*) Cyc_Absyn_FloatType){ goto _LL1017;} else{
goto _LL834;} _LL1017: _temp1016= _temp810.f2; if( _temp1016 ==( void*) Cyc_Absyn_FloatType){
goto _LL833;} else{ goto _LL834;} _LL834: _LL1023: _temp1022= _temp810.f1; if(
_temp1022 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1021;} else{ goto _LL836;}
_LL1021: _temp1020= _temp810.f2; if( _temp1020 ==( void*) Cyc_Absyn_DoubleType){
goto _LL835;} else{ goto _LL836;} _LL836: _LL1033: _temp1032= _temp810.f1; if((
unsigned int) _temp1032 > 4u?*(( int*) _temp1032) == Cyc_Absyn_ArrayType: 0){
_LL1039: _temp1038=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1032)->f1;
goto _LL1037; _LL1037: _temp1036=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1032)->f2; goto _LL1035; _LL1035: _temp1034=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1032)->f3; goto _LL1025;} else{ goto _LL838;} _LL1025: _temp1024= _temp810.f2;
if(( unsigned int) _temp1024 > 4u?*(( int*) _temp1024) == Cyc_Absyn_ArrayType: 0){
_LL1031: _temp1030=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1024)->f1;
goto _LL1029; _LL1029: _temp1028=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1024)->f2; goto _LL1027; _LL1027: _temp1026=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1024)->f3; goto _LL837;} else{ goto _LL838;} _LL838: _LL1059: _temp1058=
_temp810.f1; if(( unsigned int) _temp1058 > 4u?*(( int*) _temp1058) == Cyc_Absyn_FnType:
0){ _LL1061: _temp1060=(( struct Cyc_Absyn_FnType_struct*) _temp1058)->f1;
_LL1075: _temp1074= _temp1060.tvars; goto _LL1073; _LL1073: _temp1072= _temp1060.effect;
goto _LL1071; _LL1071: _temp1070=( void*) _temp1060.ret_typ; goto _LL1069;
_LL1069: _temp1068= _temp1060.args; goto _LL1067; _LL1067: _temp1066= _temp1060.c_varargs;
goto _LL1065; _LL1065: _temp1064= _temp1060.cyc_varargs; goto _LL1063; _LL1063:
_temp1062= _temp1060.attributes; goto _LL1041;} else{ goto _LL840;} _LL1041:
_temp1040= _temp810.f2; if(( unsigned int) _temp1040 > 4u?*(( int*) _temp1040)
== Cyc_Absyn_FnType: 0){ _LL1043: _temp1042=(( struct Cyc_Absyn_FnType_struct*)
_temp1040)->f1; _LL1057: _temp1056= _temp1042.tvars; goto _LL1055; _LL1055:
_temp1054= _temp1042.effect; goto _LL1053; _LL1053: _temp1052=( void*) _temp1042.ret_typ;
goto _LL1051; _LL1051: _temp1050= _temp1042.args; goto _LL1049; _LL1049:
_temp1048= _temp1042.c_varargs; goto _LL1047; _LL1047: _temp1046= _temp1042.cyc_varargs;
goto _LL1045; _LL1045: _temp1044= _temp1042.attributes; goto _LL839;} else{ goto
_LL840;} _LL840: _LL1081: _temp1080= _temp810.f1; if(( unsigned int) _temp1080 >
4u?*(( int*) _temp1080) == Cyc_Absyn_TupleType: 0){ _LL1083: _temp1082=(( struct
Cyc_Absyn_TupleType_struct*) _temp1080)->f1; goto _LL1077;} else{ goto _LL842;}
_LL1077: _temp1076= _temp810.f2; if(( unsigned int) _temp1076 > 4u?*(( int*)
_temp1076) == Cyc_Absyn_TupleType: 0){ _LL1079: _temp1078=(( struct Cyc_Absyn_TupleType_struct*)
_temp1076)->f1; goto _LL841;} else{ goto _LL842;} _LL842: _LL1089: _temp1088=
_temp810.f1; if(( unsigned int) _temp1088 > 4u?*(( int*) _temp1088) == Cyc_Absyn_AnonStructType:
0){ _LL1091: _temp1090=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1088)->f1;
goto _LL1085;} else{ goto _LL844;} _LL1085: _temp1084= _temp810.f2; if((
unsigned int) _temp1084 > 4u?*(( int*) _temp1084) == Cyc_Absyn_AnonStructType: 0){
_LL1087: _temp1086=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1084)->f1;
goto _LL843;} else{ goto _LL844;} _LL844: _LL1097: _temp1096= _temp810.f1; if((
unsigned int) _temp1096 > 4u?*(( int*) _temp1096) == Cyc_Absyn_AnonUnionType: 0){
_LL1099: _temp1098=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1096)->f1;
goto _LL1093;} else{ goto _LL846;} _LL1093: _temp1092= _temp810.f2; if((
unsigned int) _temp1092 > 4u?*(( int*) _temp1092) == Cyc_Absyn_AnonUnionType: 0){
_LL1095: _temp1094=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1092)->f1;
goto _LL845;} else{ goto _LL846;} _LL846: _LL1103: _temp1102= _temp810.f1; if(
_temp1102 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1101;} else{ goto _LL848;}
_LL1101: _temp1100= _temp810.f2; if( _temp1100 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL847;} else{ goto _LL848;} _LL848: _LL1109: _temp1108= _temp810.f1; if((
unsigned int) _temp1108 > 4u?*(( int*) _temp1108) == Cyc_Absyn_RgnHandleType: 0){
_LL1111: _temp1110=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1108)->f1;
goto _LL1105;} else{ goto _LL850;} _LL1105: _temp1104= _temp810.f2; if((
unsigned int) _temp1104 > 4u?*(( int*) _temp1104) == Cyc_Absyn_RgnHandleType: 0){
_LL1107: _temp1106=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1104)->f1;
goto _LL849;} else{ goto _LL850;} _LL850: _LL1115: _temp1114= _temp810.f1; if((
unsigned int) _temp1114 > 4u?*(( int*) _temp1114) == Cyc_Absyn_JoinEff: 0){
_LL1117: _temp1116=(( struct Cyc_Absyn_JoinEff_struct*) _temp1114)->f1; goto
_LL1113;} else{ goto _LL852;} _LL1113: _temp1112= _temp810.f2; goto _LL851;
_LL852: _LL1123: _temp1122= _temp810.f1; goto _LL1119; _LL1119: _temp1118=
_temp810.f2; if(( unsigned int) _temp1118 > 4u?*(( int*) _temp1118) == Cyc_Absyn_JoinEff:
0){ _LL1121: _temp1120=(( struct Cyc_Absyn_JoinEff_struct*) _temp1118)->f1; goto
_LL853;} else{ goto _LL854;} _LL854: _LL1127: _temp1126= _temp810.f1; if((
unsigned int) _temp1126 > 4u?*(( int*) _temp1126) == Cyc_Absyn_AccessEff: 0){
_LL1129: _temp1128=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp1126)->f1;
goto _LL1125;} else{ goto _LL856;} _LL1125: _temp1124= _temp810.f2; goto _LL855;
_LL856: _LL1135: _temp1134= _temp810.f1; goto _LL1131; _LL1131: _temp1130=
_temp810.f2; if(( unsigned int) _temp1130 > 4u?*(( int*) _temp1130) == Cyc_Absyn_AccessEff:
0){ _LL1133: _temp1132=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp1130)->f1;
goto _LL857;} else{ goto _LL858;} _LL858: goto _LL859; _LL813: Cyc_Tcutil_unify_it(
t2, t1); return; _LL815: return; _LL817: { struct _tagged_string* _temp1136=
_temp882->name; goto _LL1137; _LL1137: { struct _tagged_string* _temp1138=
_temp878->name; goto _LL1139; _LL1139: { int _temp1140=*(( int*) _check_null(
_temp882->identity)); goto _LL1141; _LL1141: { int _temp1142=*(( int*)
_check_null( _temp878->identity)); goto _LL1143; _LL1143: { void* _temp1144=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp882->kind);
goto _LL1145; _LL1145: { void* _temp1146=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( _temp878->kind); goto _LL1147; _LL1147: if( _temp1142 ==
_temp1140? Cyc_String_zstrptrcmp( _temp1136, _temp1138) == 0: 0){ if( _temp1144
!= _temp1146){(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
struct _tagged_string _temp1148=* _temp1136; struct _tagged_string _temp1149=
Cyc_Absynpp_kind2string( _temp1144); struct _tagged_string _temp1150= Cyc_Absynpp_kind2string(
_temp1146); xprintf("same type variable %.*s has kinds %.*s and %.*s", _temp1148.last_plus_one
- _temp1148.curr, _temp1148.curr, _temp1149.last_plus_one - _temp1149.curr,
_temp1149.curr, _temp1150.last_plus_one - _temp1150.curr, _temp1150.curr);}));}
return;} goto _LL811;}}}}}} _LL819: if((( _temp890 != 0? _temp898 != 0: 0)? Cyc_Absyn_qvar_cmp((
struct _tuple1*) _check_null( _temp890),( struct _tuple1*) _check_null( _temp898))
== 0: 0)? 1:( _temp890 == 0? _temp898 == 0: 0)){ Cyc_Tcutil_unify_list( _temp888,
_temp896); return;} goto _LL811; _LL821: if( Cyc_Absyn_qvar_cmp( _temp910,
_temp904) == 0){ return;} goto _LL811; _LL823: if((( _temp918 != 0? _temp926 !=
0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*) _check_null( _temp918),( struct
_tuple1*) _check_null( _temp926)) == 0: 0)? 1:( _temp918 == 0? _temp926 == 0: 0)){
Cyc_Tcutil_unify_list( _temp916, _temp924); return;} goto _LL811; _LL825: if(
_temp950 == _temp938? 1: Cyc_Absyn_qvar_cmp( _temp950->name, _temp938->name) ==
0){ Cyc_Tcutil_unify_it( _temp932, _temp944); Cyc_Tcutil_unify_list( _temp934,
_temp946); return;} goto _LL811; _LL827: if(( _temp974 == _temp962? 1: Cyc_Absyn_qvar_cmp(
_temp974->name, _temp962->name) == 0)? _temp972 == _temp960? 1: Cyc_Absyn_qvar_cmp(
_temp972->name, _temp960->name) == 0: 0){ Cyc_Tcutil_unify_list( _temp956,
_temp968); return;} goto _LL811; _LL829: Cyc_Tcutil_unify_it( _temp988,
_temp1002); Cyc_Tcutil_unify_it( _temp1000, _temp986); Cyc_Tcutil_unify_tqual(
_temp982, _temp996);(( void(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)( Cyc_Tcutil_boundscmp,
_temp980, _temp994);{ void* _temp1151=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp980))->v; void*
_temp1157; _LL1153: if(( unsigned int) _temp1151 > 1u?*(( int*) _temp1151) ==
Cyc_Absyn_Eq_constr: 0){ _LL1158: _temp1157=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1151)->f1; if( _temp1157 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1154;}
else{ goto _LL1155;}} else{ goto _LL1155;} _LL1155: goto _LL1156; _LL1154:
return; _LL1156: goto _LL1152; _LL1152:;}(( void(*)( int(* cmp)( int, int),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp, _temp984, _temp998); return; _LL831: if( _temp1008 == _temp1014?
_temp1006 == _temp1012: 0){ return;} goto _LL811; _LL833: return; _LL835:
return; _LL837: Cyc_Tcutil_unify_it( _temp1030, _temp1038); Cyc_Tcutil_unify_tqual(
_temp1028, _temp1036); if( _temp1034 == _temp1026){ return;} if( _temp1034 == 0?
1: _temp1026 == 0){ goto _LL811;} if( Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1034)) == Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1026))){ return;} goto _LL811; _LL839: { int done= 0;{ struct
_RegionHandle _temp1159= _new_region(); struct _RegionHandle* rgn=& _temp1159;
_push_region( rgn);{ struct Cyc_List_List* inst= 0; while( _temp1056 != 0) { if(
_temp1074 == 0){ break;} inst=({ struct Cyc_List_List* _temp1160=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1160->hd=( void*)({
struct _tuple5* _temp1161=( struct _tuple5*) _region_malloc( rgn, sizeof( struct
_tuple5)); _temp1161->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp1074))->hd; _temp1161->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1162=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1162[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1163; _temp1163.tag= Cyc_Absyn_VarType;
_temp1163.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp1056))->hd; _temp1163;}); _temp1162;}); _temp1161;}); _temp1160->tl= inst;
_temp1160;}); _temp1056=(( struct Cyc_List_List*) _check_null( _temp1056))->tl;
_temp1074=(( struct Cyc_List_List*) _check_null( _temp1074))->tl;} if( _temp1074
!= 0){ _npop_handler( 0u); goto _LL811;} if( inst != 0){ Cyc_Tcutil_unify_it((
void*)({ struct Cyc_Absyn_FnType_struct* _temp1164=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1164[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1165; _temp1165.tag= Cyc_Absyn_FnType; _temp1165.f1=({ struct Cyc_Absyn_FnInfo
_temp1166; _temp1166.tvars= 0; _temp1166.effect= _temp1054; _temp1166.ret_typ=(
void*) _temp1052; _temp1166.args= _temp1050; _temp1166.c_varargs= _temp1048;
_temp1166.cyc_varargs= _temp1046; _temp1166.attributes= _temp1044; _temp1166;});
_temp1165;}); _temp1164;}), Cyc_Tcutil_rsubstitute( rgn, inst,( void*)({ struct
Cyc_Absyn_FnType_struct* _temp1167=( struct Cyc_Absyn_FnType_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_FnType_struct)); _temp1167[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1168; _temp1168.tag= Cyc_Absyn_FnType; _temp1168.f1=({ struct Cyc_Absyn_FnInfo
_temp1169; _temp1169.tvars= 0; _temp1169.effect= _temp1072; _temp1169.ret_typ=(
void*) _temp1070; _temp1169.args= _temp1068; _temp1169.c_varargs= _temp1066;
_temp1169.cyc_varargs= _temp1064; _temp1169.attributes= _temp1062; _temp1169;});
_temp1168;}); _temp1167;}))); done= 1;}}; _pop_region(& _temp1159);} if( done){
return;} Cyc_Tcutil_unify_it( _temp1052, _temp1070); for( 0; _temp1050 != 0?
_temp1068 != 0: 0;( _temp1050=(( struct Cyc_List_List*) _check_null( _temp1050))->tl,
_temp1068=(( struct Cyc_List_List*) _check_null( _temp1068))->tl)){ Cyc_Tcutil_unify_tqual((*((
struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp1050))->hd)).f2,(*((
struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp1068))->hd)).f2);
Cyc_Tcutil_unify_it((*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null(
_temp1050))->hd)).f3,(*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null(
_temp1068))->hd)).f3);} if( _temp1050 != 0? 1: _temp1068 != 0){ goto _LL811;}
if( _temp1048 != _temp1066){ goto _LL811;}{ int bad_cyc_vararg= 0;{ struct
_tuple11 _temp1171=({ struct _tuple11 _temp1170; _temp1170.f1= _temp1046;
_temp1170.f2= _temp1064; _temp1170;}); struct Cyc_Absyn_VarargInfo* _temp1181;
struct Cyc_Absyn_VarargInfo* _temp1183; struct Cyc_Absyn_VarargInfo* _temp1185;
struct Cyc_Absyn_VarargInfo* _temp1187; struct Cyc_Absyn_VarargInfo* _temp1189;
struct Cyc_Absyn_VarargInfo* _temp1191; struct Cyc_Absyn_VarargInfo* _temp1193;
struct Cyc_Absyn_VarargInfo _temp1195; int _temp1196; void* _temp1198; void*
_temp1200; struct Cyc_Absyn_Tqual _temp1202; struct Cyc_Core_Opt* _temp1204;
struct Cyc_Absyn_VarargInfo* _temp1206; struct Cyc_Absyn_VarargInfo _temp1208;
int _temp1209; void* _temp1211; void* _temp1213; struct Cyc_Absyn_Tqual
_temp1215; struct Cyc_Core_Opt* _temp1217; _LL1173: _LL1184: _temp1183=
_temp1171.f1; if( _temp1183 == 0){ goto _LL1182;} else{ goto _LL1175;} _LL1182:
_temp1181= _temp1171.f2; if( _temp1181 == 0){ goto _LL1174;} else{ goto _LL1175;}
_LL1175: _LL1188: _temp1187= _temp1171.f1; if( _temp1187 == 0){ goto _LL1186;}
else{ goto _LL1177;} _LL1186: _temp1185= _temp1171.f2; goto _LL1176; _LL1177:
_LL1192: _temp1191= _temp1171.f1; goto _LL1190; _LL1190: _temp1189= _temp1171.f2;
if( _temp1189 == 0){ goto _LL1178;} else{ goto _LL1179;} _LL1179: _LL1207:
_temp1206= _temp1171.f1; if( _temp1206 == 0){ goto _LL1172;} else{ _temp1208=*
_temp1206; _LL1218: _temp1217= _temp1208.name; goto _LL1216; _LL1216: _temp1215=
_temp1208.tq; goto _LL1214; _LL1214: _temp1213=( void*) _temp1208.type; goto
_LL1212; _LL1212: _temp1211=( void*) _temp1208.rgn; goto _LL1210; _LL1210:
_temp1209= _temp1208.inject; goto _LL1194;} _LL1194: _temp1193= _temp1171.f2;
if( _temp1193 == 0){ goto _LL1172;} else{ _temp1195=* _temp1193; _LL1205:
_temp1204= _temp1195.name; goto _LL1203; _LL1203: _temp1202= _temp1195.tq; goto
_LL1201; _LL1201: _temp1200=( void*) _temp1195.type; goto _LL1199; _LL1199:
_temp1198=( void*) _temp1195.rgn; goto _LL1197; _LL1197: _temp1196= _temp1195.inject;
goto _LL1180;} _LL1174: goto _LL1172; _LL1176: goto _LL1178; _LL1178:
bad_cyc_vararg= 1; goto _LL1172; _LL1180: Cyc_Tcutil_unify_tqual( _temp1215,
_temp1202); Cyc_Tcutil_unify_it( _temp1213, _temp1200); Cyc_Tcutil_unify_it(
_temp1211, _temp1198); if( _temp1209 != _temp1196){ bad_cyc_vararg= 1;} goto
_LL1172; _LL1172:;} if( bad_cyc_vararg){ goto _LL811;}{ int bad_effect= 0;{
struct _tuple12 _temp1220=({ struct _tuple12 _temp1219; _temp1219.f1= _temp1054;
_temp1219.f2= _temp1072; _temp1219;}); struct Cyc_Core_Opt* _temp1230; struct
Cyc_Core_Opt* _temp1232; struct Cyc_Core_Opt* _temp1234; struct Cyc_Core_Opt*
_temp1236; struct Cyc_Core_Opt* _temp1238; struct Cyc_Core_Opt* _temp1240;
_LL1222: _LL1233: _temp1232= _temp1220.f1; if( _temp1232 == 0){ goto _LL1231;}
else{ goto _LL1224;} _LL1231: _temp1230= _temp1220.f2; if( _temp1230 == 0){ goto
_LL1223;} else{ goto _LL1224;} _LL1224: _LL1237: _temp1236= _temp1220.f1; if(
_temp1236 == 0){ goto _LL1235;} else{ goto _LL1226;} _LL1235: _temp1234=
_temp1220.f2; goto _LL1225; _LL1226: _LL1241: _temp1240= _temp1220.f1; goto
_LL1239; _LL1239: _temp1238= _temp1220.f2; if( _temp1238 == 0){ goto _LL1227;}
else{ goto _LL1228;} _LL1228: goto _LL1229; _LL1223: goto _LL1221; _LL1225: goto
_LL1227; _LL1227: bad_effect= 1; goto _LL1221; _LL1229: bad_effect= ! Cyc_Tcutil_unify_effect((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1054))->v,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1072))->v); goto _LL1221; _LL1221:;} if( bad_effect){ goto
_LL811;} if( ! Cyc_Tcutil_same_atts( _temp1062, _temp1044)){ goto _LL811;}
return;}}} _LL841: for( 0; _temp1078 != 0? _temp1082 != 0: 0;( _temp1078=((
struct Cyc_List_List*) _check_null( _temp1078))->tl, _temp1082=(( struct Cyc_List_List*)
_check_null( _temp1082))->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1078))->hd)).f1,(*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1082))->hd)).f1); Cyc_Tcutil_unify_it((*((
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1078))->hd)).f2,(*((
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1082))->hd)).f2);}
if( _temp1078 == 0? _temp1082 == 0: 0){ return;} goto _LL811; _LL843: _temp1098=
_temp1090; _temp1094= _temp1086; goto _LL845; _LL845: for( 0; _temp1094 != 0?
_temp1098 != 0: 0;( _temp1094=(( struct Cyc_List_List*) _check_null( _temp1094))->tl,
_temp1098=(( struct Cyc_List_List*) _check_null( _temp1098))->tl)){ struct Cyc_Absyn_Structfield*
_temp1242=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1094))->hd; goto _LL1243; _LL1243: { struct Cyc_Absyn_Structfield*
_temp1244=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1098))->hd; goto _LL1245; _LL1245: if( Cyc_String_zstrptrcmp( _temp1242->name,
_temp1244->name) != 0){( void) _throw(( void*) Cyc_Tcutil_Unify);} Cyc_Tcutil_unify_tqual(
_temp1242->tq, _temp1244->tq); Cyc_Tcutil_unify_it(( void*) _temp1242->type,(
void*) _temp1244->type); if((( ! Cyc_Tcutil_same_atts( _temp1242->attributes,
_temp1244->attributes)? 1:( _temp1242->width != 0? _temp1244->width == 0: 0))? 1:(
_temp1244->width != 0? _temp1242->width == 0: 0))? 1:(( _temp1242->width != 0?
_temp1244->width != 0: 0)? Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp1242->width))->v) != Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp1244->width))->v):
0)){( void) _throw(( void*) Cyc_Tcutil_Unify);}}} if( _temp1094 == 0? _temp1098
== 0: 0){ return;} goto _LL811; _LL847: return; _LL849: Cyc_Tcutil_unify_it(
_temp1110, _temp1106); return; _LL851: goto _LL853; _LL853: goto _LL855; _LL855:
goto _LL857; _LL857: if( Cyc_Tcutil_unify_effect( t1, t2)){ return;} else{ goto
_LL811;} _LL859: goto _LL811; _LL811:;}( void) _throw(( void*) Cyc_Tcutil_Unify);}
int Cyc_Tcutil_is_arithmetic_type( void* t){ void* _temp1246= Cyc_Tcutil_compress(
t); void* _temp1258; void* _temp1260; struct Cyc_Absyn_Enumdecl* _temp1262;
struct _tuple1* _temp1264; _LL1248: if(( unsigned int) _temp1246 > 4u?*(( int*)
_temp1246) == Cyc_Absyn_IntType: 0){ _LL1261: _temp1260=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1246)->f1; goto _LL1259; _LL1259: _temp1258=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1246)->f2; goto _LL1249;} else{ goto _LL1250;} _LL1250: if( _temp1246 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1251;} else{ goto _LL1252;} _LL1252: if(
_temp1246 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1253;} else{ goto _LL1254;}
_LL1254: if(( unsigned int) _temp1246 > 4u?*(( int*) _temp1246) == Cyc_Absyn_EnumType:
0){ _LL1265: _temp1264=(( struct Cyc_Absyn_EnumType_struct*) _temp1246)->f1;
goto _LL1263; _LL1263: _temp1262=(( struct Cyc_Absyn_EnumType_struct*) _temp1246)->f2;
goto _LL1255;} else{ goto _LL1256;} _LL1256: goto _LL1257; _LL1249: return 1;
_LL1251: return 1; _LL1253: return 1; _LL1255: return 1; _LL1257: return 0;
_LL1247:;} int Cyc_Tcutil_will_lose_precision( void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8 _temp1267=({ struct _tuple8
_temp1266; _temp1266.f1= t1; _temp1266.f2= t2; _temp1266;}); void* _temp1289;
void* _temp1291; void* _temp1293; void* _temp1295; void* _temp1297; void*
_temp1299; void* _temp1301; void* _temp1303; void* _temp1305; void* _temp1307;
void* _temp1309; void* _temp1311; void* _temp1313; void* _temp1315; void*
_temp1317; void* _temp1319; void* _temp1321; void* _temp1323; void* _temp1325;
void* _temp1327; void* _temp1329; void* _temp1331; void* _temp1333; void*
_temp1335; void* _temp1337; void* _temp1339; void* _temp1341; void* _temp1343;
void* _temp1345; void* _temp1347; void* _temp1349; void* _temp1351; void*
_temp1353; void* _temp1355; void* _temp1357; void* _temp1359; void* _temp1361;
void* _temp1363; void* _temp1365; void* _temp1367; void* _temp1369; void*
_temp1371; _LL1269: _LL1292: _temp1291= _temp1267.f1; if( _temp1291 ==( void*)
Cyc_Absyn_DoubleType){ goto _LL1290;} else{ goto _LL1271;} _LL1290: _temp1289=
_temp1267.f2; if( _temp1289 ==( void*) Cyc_Absyn_FloatType){ goto _LL1270;}
else{ goto _LL1271;} _LL1271: _LL1300: _temp1299= _temp1267.f1; if( _temp1299 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL1294;} else{ goto _LL1273;} _LL1294:
_temp1293= _temp1267.f2; if(( unsigned int) _temp1293 > 4u?*(( int*) _temp1293)
== Cyc_Absyn_IntType: 0){ _LL1298: _temp1297=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1293)->f1; goto _LL1296; _LL1296: _temp1295=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1293)->f2; goto _LL1272;} else{ goto _LL1273;} _LL1273: _LL1308: _temp1307=
_temp1267.f1; if( _temp1307 ==( void*) Cyc_Absyn_FloatType){ goto _LL1302;}
else{ goto _LL1275;} _LL1302: _temp1301= _temp1267.f2; if(( unsigned int)
_temp1301 > 4u?*(( int*) _temp1301) == Cyc_Absyn_IntType: 0){ _LL1306: _temp1305=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1301)->f1; goto _LL1304; _LL1304:
_temp1303=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1301)->f2; goto
_LL1274;} else{ goto _LL1275;} _LL1275: _LL1316: _temp1315= _temp1267.f1; if((
unsigned int) _temp1315 > 4u?*(( int*) _temp1315) == Cyc_Absyn_IntType: 0){
_LL1320: _temp1319=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1315)->f1;
goto _LL1318; _LL1318: _temp1317=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1315)->f2; if( _temp1317 ==( void*) Cyc_Absyn_B8){ goto _LL1310;} else{
goto _LL1277;}} else{ goto _LL1277;} _LL1310: _temp1309= _temp1267.f2; if((
unsigned int) _temp1309 > 4u?*(( int*) _temp1309) == Cyc_Absyn_IntType: 0){
_LL1314: _temp1313=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1309)->f1;
goto _LL1312; _LL1312: _temp1311=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1309)->f2; if( _temp1311 ==( void*) Cyc_Absyn_B8){ goto _LL1276;} else{
goto _LL1277;}} else{ goto _LL1277;} _LL1277: _LL1324: _temp1323= _temp1267.f1;
if(( unsigned int) _temp1323 > 4u?*(( int*) _temp1323) == Cyc_Absyn_IntType: 0){
_LL1328: _temp1327=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1323)->f1;
goto _LL1326; _LL1326: _temp1325=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1323)->f2; if( _temp1325 ==( void*) Cyc_Absyn_B8){ goto _LL1322;} else{
goto _LL1279;}} else{ goto _LL1279;} _LL1322: _temp1321= _temp1267.f2; goto
_LL1278; _LL1279: _LL1332: _temp1331= _temp1267.f1; if(( unsigned int) _temp1331
> 4u?*(( int*) _temp1331) == Cyc_Absyn_IntType: 0){ _LL1336: _temp1335=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1331)->f1; goto _LL1334; _LL1334:
_temp1333=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1331)->f2; if(
_temp1333 ==( void*) Cyc_Absyn_B4){ goto _LL1330;} else{ goto _LL1281;}} else{
goto _LL1281;} _LL1330: _temp1329= _temp1267.f2; if( _temp1329 ==( void*) Cyc_Absyn_FloatType){
goto _LL1280;} else{ goto _LL1281;} _LL1281: _LL1344: _temp1343= _temp1267.f1;
if(( unsigned int) _temp1343 > 4u?*(( int*) _temp1343) == Cyc_Absyn_IntType: 0){
_LL1348: _temp1347=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1343)->f1;
goto _LL1346; _LL1346: _temp1345=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1343)->f2; if( _temp1345 ==( void*) Cyc_Absyn_B4){ goto _LL1338;} else{
goto _LL1283;}} else{ goto _LL1283;} _LL1338: _temp1337= _temp1267.f2; if((
unsigned int) _temp1337 > 4u?*(( int*) _temp1337) == Cyc_Absyn_IntType: 0){
_LL1342: _temp1341=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1337)->f1;
goto _LL1340; _LL1340: _temp1339=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1337)->f2; if( _temp1339 ==( void*) Cyc_Absyn_B2){ goto _LL1282;} else{
goto _LL1283;}} else{ goto _LL1283;} _LL1283: _LL1356: _temp1355= _temp1267.f1;
if(( unsigned int) _temp1355 > 4u?*(( int*) _temp1355) == Cyc_Absyn_IntType: 0){
_LL1360: _temp1359=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1355)->f1;
goto _LL1358; _LL1358: _temp1357=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1355)->f2; if( _temp1357 ==( void*) Cyc_Absyn_B4){ goto _LL1350;} else{
goto _LL1285;}} else{ goto _LL1285;} _LL1350: _temp1349= _temp1267.f2; if((
unsigned int) _temp1349 > 4u?*(( int*) _temp1349) == Cyc_Absyn_IntType: 0){
_LL1354: _temp1353=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1349)->f1;
goto _LL1352; _LL1352: _temp1351=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1349)->f2; if( _temp1351 ==( void*) Cyc_Absyn_B1){ goto _LL1284;} else{
goto _LL1285;}} else{ goto _LL1285;} _LL1285: _LL1368: _temp1367= _temp1267.f1;
if(( unsigned int) _temp1367 > 4u?*(( int*) _temp1367) == Cyc_Absyn_IntType: 0){
_LL1372: _temp1371=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1367)->f1;
goto _LL1370; _LL1370: _temp1369=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1367)->f2; if( _temp1369 ==( void*) Cyc_Absyn_B2){ goto _LL1362;} else{
goto _LL1287;}} else{ goto _LL1287;} _LL1362: _temp1361= _temp1267.f2; if((
unsigned int) _temp1361 > 4u?*(( int*) _temp1361) == Cyc_Absyn_IntType: 0){
_LL1366: _temp1365=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1361)->f1;
goto _LL1364; _LL1364: _temp1363=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1361)->f2; if( _temp1363 ==( void*) Cyc_Absyn_B1){ goto _LL1286;} else{
goto _LL1287;}} else{ goto _LL1287;} _LL1287: goto _LL1288; _LL1270: return 1;
_LL1272: return 1; _LL1274: return 1; _LL1276: return 0; _LL1278: return 1;
_LL1280: return 1; _LL1282: return 1; _LL1284: return 1; _LL1286: return 1;
_LL1288: return 0; _LL1268:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{
struct Cyc_List_List* el= es; for( 0; el != 0; el=(( struct Cyc_List_List*)
_check_null( el))->tl){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);
if( Cyc_Tcutil_is_arithmetic_type( t1)){ if( max_arith_type == 0? 1: Cyc_Tcutil_will_lose_precision(
t1,( void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){
max_arith_type=({ struct Cyc_Core_Opt* _temp1373=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1373->v=( void*) t1; _temp1373;});}}}}
if( max_arith_type != 0){ if( ! Cyc_Tcutil_unify( t,( void*)(( struct Cyc_Core_Opt*)
_check_null( max_arith_type))->v)){ return 0;}}{ struct Cyc_List_List* el= es;
for( 0; el != 0; el=(( struct Cyc_List_List*) _check_null( el))->tl){ if( ! Cyc_Tcutil_coerce_assign(
te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd, t)){
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
el))->hd)->loc,({ struct _tagged_string _temp1374= Cyc_Absynpp_typ2string( t);
struct _tagged_string _temp1375= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);
xprintf("type mismatch: expecting %.*s but found %.*s", _temp1374.last_plus_one
- _temp1374.curr, _temp1374.curr, _temp1375.last_plus_one - _temp1375.curr,
_temp1375.curr);})); return 0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e)){ void* _temp1376= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); struct Cyc_Absyn_PtrInfo _temp1382; _LL1378: if((
unsigned int) _temp1376 > 4u?*(( int*) _temp1376) == Cyc_Absyn_PointerType: 0){
_LL1383: _temp1382=(( struct Cyc_Absyn_PointerType_struct*) _temp1376)->f1; goto
_LL1379;} else{ goto _LL1380;} _LL1380: goto _LL1381; _LL1379: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); goto _LL1377; _LL1381: return 0; _LL1377:;} return 1;}
int Cyc_Tcutil_is_integral_type( void* t){ void* _temp1384= Cyc_Tcutil_compress(
t); void* _temp1392; void* _temp1394; struct Cyc_Absyn_Enumdecl* _temp1396;
struct _tuple1* _temp1398; _LL1386: if(( unsigned int) _temp1384 > 4u?*(( int*)
_temp1384) == Cyc_Absyn_IntType: 0){ _LL1395: _temp1394=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1384)->f1; goto _LL1393; _LL1393: _temp1392=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1384)->f2; goto _LL1387;} else{ goto _LL1388;} _LL1388: if(( unsigned int)
_temp1384 > 4u?*(( int*) _temp1384) == Cyc_Absyn_EnumType: 0){ _LL1399:
_temp1398=(( struct Cyc_Absyn_EnumType_struct*) _temp1384)->f1; goto _LL1397;
_LL1397: _temp1396=(( struct Cyc_Absyn_EnumType_struct*) _temp1384)->f2; goto
_LL1389;} else{ goto _LL1390;} _LL1390: goto _LL1391; _LL1387: return 1; _LL1389:
return 1; _LL1391: return 0; _LL1385:;} int Cyc_Tcutil_coerce_uint_typ( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_uint_t)){ return 1;}
if( Cyc_Tcutil_is_integral_type(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){
if( Cyc_Tcutil_will_lose_precision(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, Cyc_Absyn_uint_t)){ Cyc_Tcutil_warn( e->loc,({ unsigned char*
_temp1400="integral size mismatch; conversion supplied"; struct _tagged_string
_temp1401; _temp1401.curr= _temp1400; _temp1401.base= _temp1400; _temp1401.last_plus_one=
_temp1400 + 44; _temp1401;}));} Cyc_Tcutil_unchecked_cast( te, e, Cyc_Absyn_uint_t);
return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){ if( Cyc_Tcutil_will_lose_precision((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){
Cyc_Tcutil_warn( e->loc,({ unsigned char* _temp1402="integral size mismatch; conversion supplied";
struct _tagged_string _temp1403; _temp1403.curr= _temp1402; _temp1403.base=
_temp1402; _temp1403.last_plus_one= _temp1402 + 44; _temp1403;}));} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); return 1;} return 0;} int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8
_temp1405=({ struct _tuple8 _temp1404; _temp1404.f1= t1; _temp1404.f2= t2;
_temp1404;}); void* _temp1417; struct Cyc_Absyn_PtrInfo _temp1419; void*
_temp1421; struct Cyc_Absyn_PtrInfo _temp1423; void* _temp1425; struct Cyc_Absyn_Exp*
_temp1427; struct Cyc_Absyn_Tqual _temp1429; void* _temp1431; void* _temp1433;
struct Cyc_Absyn_Exp* _temp1435; struct Cyc_Absyn_Tqual _temp1437; void*
_temp1439; void* _temp1441; struct Cyc_Absyn_TunionInfo _temp1443; void*
_temp1445; struct Cyc_List_List* _temp1447; void* _temp1449; struct Cyc_Absyn_Tuniondecl*
_temp1451; void* _temp1453; struct Cyc_Absyn_TunionFieldInfo _temp1455; struct
Cyc_List_List* _temp1457; void* _temp1459; struct Cyc_Absyn_Tunionfield*
_temp1461; struct Cyc_Absyn_Tuniondecl* _temp1463; void* _temp1465; struct Cyc_Absyn_TunionInfo
_temp1467; void* _temp1469; struct Cyc_List_List* _temp1471; void* _temp1473;
struct Cyc_Absyn_Tuniondecl* _temp1475; void* _temp1477; struct Cyc_Absyn_PtrInfo
_temp1479; struct Cyc_Absyn_Conref* _temp1481; struct Cyc_Absyn_Tqual _temp1483;
struct Cyc_Absyn_Conref* _temp1485; void* _temp1487; void* _temp1489; _LL1407:
_LL1422: _temp1421= _temp1405.f1; if(( unsigned int) _temp1421 > 4u?*(( int*)
_temp1421) == Cyc_Absyn_PointerType: 0){ _LL1424: _temp1423=(( struct Cyc_Absyn_PointerType_struct*)
_temp1421)->f1; goto _LL1418;} else{ goto _LL1409;} _LL1418: _temp1417=
_temp1405.f2; if(( unsigned int) _temp1417 > 4u?*(( int*) _temp1417) == Cyc_Absyn_PointerType:
0){ _LL1420: _temp1419=(( struct Cyc_Absyn_PointerType_struct*) _temp1417)->f1;
goto _LL1408;} else{ goto _LL1409;} _LL1409: _LL1434: _temp1433= _temp1405.f1;
if(( unsigned int) _temp1433 > 4u?*(( int*) _temp1433) == Cyc_Absyn_ArrayType: 0){
_LL1440: _temp1439=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1433)->f1;
goto _LL1438; _LL1438: _temp1437=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1433)->f2; goto _LL1436; _LL1436: _temp1435=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1433)->f3; goto _LL1426;} else{ goto _LL1411;} _LL1426: _temp1425=
_temp1405.f2; if(( unsigned int) _temp1425 > 4u?*(( int*) _temp1425) == Cyc_Absyn_ArrayType:
0){ _LL1432: _temp1431=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1425)->f1;
goto _LL1430; _LL1430: _temp1429=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1425)->f2; goto _LL1428; _LL1428: _temp1427=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1425)->f3; goto _LL1410;} else{ goto _LL1411;} _LL1411: _LL1454: _temp1453=
_temp1405.f1; if(( unsigned int) _temp1453 > 4u?*(( int*) _temp1453) == Cyc_Absyn_TunionFieldType:
0){ _LL1456: _temp1455=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1453)->f1;
_LL1460: _temp1459=( void*) _temp1455.field_info; if(*(( int*) _temp1459) == Cyc_Absyn_KnownTunionfield){
_LL1464: _temp1463=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1459)->f1;
goto _LL1462; _LL1462: _temp1461=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1459)->f2; goto _LL1458;} else{ goto _LL1413;} _LL1458: _temp1457=
_temp1455.targs; goto _LL1442;} else{ goto _LL1413;} _LL1442: _temp1441=
_temp1405.f2; if(( unsigned int) _temp1441 > 4u?*(( int*) _temp1441) == Cyc_Absyn_TunionType:
0){ _LL1444: _temp1443=(( struct Cyc_Absyn_TunionType_struct*) _temp1441)->f1;
_LL1450: _temp1449=( void*) _temp1443.tunion_info; if(*(( int*) _temp1449) ==
Cyc_Absyn_KnownTunion){ _LL1452: _temp1451=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1449)->f1; goto _LL1448;} else{ goto _LL1413;} _LL1448: _temp1447=
_temp1443.targs; goto _LL1446; _LL1446: _temp1445=( void*) _temp1443.rgn; goto
_LL1412;} else{ goto _LL1413;} _LL1413: _LL1478: _temp1477= _temp1405.f1; if((
unsigned int) _temp1477 > 4u?*(( int*) _temp1477) == Cyc_Absyn_PointerType: 0){
_LL1480: _temp1479=(( struct Cyc_Absyn_PointerType_struct*) _temp1477)->f1;
_LL1490: _temp1489=( void*) _temp1479.elt_typ; goto _LL1488; _LL1488: _temp1487=(
void*) _temp1479.rgn_typ; goto _LL1486; _LL1486: _temp1485= _temp1479.nullable;
goto _LL1484; _LL1484: _temp1483= _temp1479.tq; goto _LL1482; _LL1482: _temp1481=
_temp1479.bounds; goto _LL1466;} else{ goto _LL1415;} _LL1466: _temp1465=
_temp1405.f2; if(( unsigned int) _temp1465 > 4u?*(( int*) _temp1465) == Cyc_Absyn_TunionType:
0){ _LL1468: _temp1467=(( struct Cyc_Absyn_TunionType_struct*) _temp1465)->f1;
_LL1474: _temp1473=( void*) _temp1467.tunion_info; if(*(( int*) _temp1473) ==
Cyc_Absyn_KnownTunion){ _LL1476: _temp1475=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1473)->f1; goto _LL1472;} else{ goto _LL1415;} _LL1472: _temp1471=
_temp1467.targs; goto _LL1470; _LL1470: _temp1469=( void*) _temp1467.rgn; goto
_LL1414;} else{ goto _LL1415;} _LL1415: goto _LL1416; _LL1408: { int okay= 1;
if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1423.nullable, _temp1419.nullable)){
void* _temp1491=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp1423.nullable))->v; int _temp1497; _LL1493:
if(( unsigned int) _temp1491 > 1u?*(( int*) _temp1491) == Cyc_Absyn_Eq_constr: 0){
_LL1498: _temp1497=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1491)->f1;
goto _LL1494;} else{ goto _LL1495;} _LL1495: goto _LL1496; _LL1494: okay= !
_temp1497; goto _LL1492; _LL1496:( void) _throw((( void*(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp1499="silent_castable conref not eq";
struct _tagged_string _temp1500; _temp1500.curr= _temp1499; _temp1500.base=
_temp1499; _temp1500.last_plus_one= _temp1499 + 30; _temp1500;}))); _LL1492:;}
if( !(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct
Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp, _temp1423.bounds,
_temp1419.bounds)){ struct _tuple8 _temp1502=({ struct _tuple8 _temp1501;
_temp1501.f1=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp1423.bounds))->v; _temp1501.f2=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp1419.bounds))->v; _temp1501;}); void* _temp1512; void* _temp1514; void*
_temp1516; void* _temp1518; struct Cyc_Absyn_Exp* _temp1520; void* _temp1522;
void* _temp1524; struct Cyc_Absyn_Exp* _temp1526; void* _temp1528; void*
_temp1530; struct Cyc_Absyn_Exp* _temp1532; void* _temp1534; void* _temp1536;
void* _temp1538; void* _temp1540; _LL1504: _LL1517: _temp1516= _temp1502.f1; if((
unsigned int) _temp1516 > 1u?*(( int*) _temp1516) == Cyc_Absyn_Eq_constr: 0){
_LL1519: _temp1518=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1516)->f1;
if(( unsigned int) _temp1518 > 1u?*(( int*) _temp1518) == Cyc_Absyn_Upper_b: 0){
_LL1521: _temp1520=(( struct Cyc_Absyn_Upper_b_struct*) _temp1518)->f1; goto
_LL1513;} else{ goto _LL1506;}} else{ goto _LL1506;} _LL1513: _temp1512=
_temp1502.f2; if(( unsigned int) _temp1512 > 1u?*(( int*) _temp1512) == Cyc_Absyn_Eq_constr:
0){ _LL1515: _temp1514=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1512)->f1;
if( _temp1514 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1505;} else{ goto _LL1506;}}
else{ goto _LL1506;} _LL1506: _LL1529: _temp1528= _temp1502.f1; if((
unsigned int) _temp1528 > 1u?*(( int*) _temp1528) == Cyc_Absyn_Eq_constr: 0){
_LL1531: _temp1530=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1528)->f1;
if(( unsigned int) _temp1530 > 1u?*(( int*) _temp1530) == Cyc_Absyn_Upper_b: 0){
_LL1533: _temp1532=(( struct Cyc_Absyn_Upper_b_struct*) _temp1530)->f1; goto
_LL1523;} else{ goto _LL1508;}} else{ goto _LL1508;} _LL1523: _temp1522=
_temp1502.f2; if(( unsigned int) _temp1522 > 1u?*(( int*) _temp1522) == Cyc_Absyn_Eq_constr:
0){ _LL1525: _temp1524=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1522)->f1;
if(( unsigned int) _temp1524 > 1u?*(( int*) _temp1524) == Cyc_Absyn_Upper_b: 0){
_LL1527: _temp1526=(( struct Cyc_Absyn_Upper_b_struct*) _temp1524)->f1; goto
_LL1507;} else{ goto _LL1508;}} else{ goto _LL1508;} _LL1508: _LL1539: _temp1538=
_temp1502.f1; if(( unsigned int) _temp1538 > 1u?*(( int*) _temp1538) == Cyc_Absyn_Eq_constr:
0){ _LL1541: _temp1540=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1538)->f1;
if( _temp1540 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1535;} else{ goto _LL1510;}}
else{ goto _LL1510;} _LL1535: _temp1534= _temp1502.f2; if(( unsigned int)
_temp1534 > 1u?*(( int*) _temp1534) == Cyc_Absyn_Eq_constr: 0){ _LL1537:
_temp1536=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1534)->f1; if(
_temp1536 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1509;} else{ goto _LL1510;}}
else{ goto _LL1510;} _LL1510: goto _LL1511; _LL1505: okay= 1; goto _LL1503;
_LL1507: okay= okay? Cyc_Evexp_eval_const_uint_exp( _temp1532) >= Cyc_Evexp_eval_const_uint_exp(
_temp1526): 0; Cyc_Tcutil_warn( loc,({ unsigned char* _temp1542="implicit cast to shorter array";
struct _tagged_string _temp1543; _temp1543.curr= _temp1542; _temp1543.base=
_temp1542; _temp1543.last_plus_one= _temp1542 + 31; _temp1543;})); goto _LL1503;
_LL1509: okay= 1; goto _LL1503; _LL1511: okay= 0; goto _LL1503; _LL1503:;}
return(( okay? Cyc_Tcutil_unify(( void*) _temp1423.elt_typ,( void*) _temp1419.elt_typ):
0)? Cyc_Tcutil_unify(( void*) _temp1423.rgn_typ,( void*) _temp1419.rgn_typ): 0)?
!( _temp1423.tq).q_const? 1:( _temp1419.tq).q_const: 0;} _LL1410: { int okay;
okay=( _temp1435 != 0? _temp1427 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1435)) == Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1427)): 0; return( okay? Cyc_Tcutil_unify(
_temp1439, _temp1431): 0)? ! _temp1437.q_const? 1: _temp1429.q_const: 0;}
_LL1412: if(( _temp1463 == _temp1451? 1: Cyc_Absyn_qvar_cmp( _temp1463->name,
_temp1451->name) == 0)? _temp1461->typs == 0: 0){ for( 0; _temp1457 != 0?
_temp1447 != 0: 0;( _temp1457=(( struct Cyc_List_List*) _check_null( _temp1457))->tl,
_temp1447=(( struct Cyc_List_List*) _check_null( _temp1447))->tl)){ if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_List_List*) _check_null( _temp1457))->hd,( void*)(( struct
Cyc_List_List*) _check_null( _temp1447))->hd)){ break;}} if( _temp1457 == 0?
_temp1447 == 0: 0){ return 1;}} return 0; _LL1414:{ void* _temp1544= Cyc_Tcutil_compress(
_temp1489); struct Cyc_Absyn_TunionFieldInfo _temp1550; struct Cyc_List_List*
_temp1552; void* _temp1554; struct Cyc_Absyn_Tunionfield* _temp1556; struct Cyc_Absyn_Tuniondecl*
_temp1558; _LL1546: if(( unsigned int) _temp1544 > 4u?*(( int*) _temp1544) ==
Cyc_Absyn_TunionFieldType: 0){ _LL1551: _temp1550=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1544)->f1; _LL1555: _temp1554=( void*) _temp1550.field_info; if(*(( int*)
_temp1554) == Cyc_Absyn_KnownTunionfield){ _LL1559: _temp1558=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1554)->f1; goto _LL1557; _LL1557: _temp1556=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1554)->f2; goto _LL1553;} else{ goto _LL1548;} _LL1553: _temp1552=
_temp1550.targs; goto _LL1547;} else{ goto _LL1548;} _LL1548: goto _LL1549;
_LL1547: if( ! Cyc_Tcutil_unify( _temp1487, _temp1469)? _temp1487 !=( void*) Cyc_Absyn_HeapRgn:
0){ return 0;} if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1485,(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)( 0))){
return 0;} if( !(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp,
_temp1481,(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)( Cyc_Absyn_bounds_one))){
return 0;} if( Cyc_Absyn_qvar_cmp( _temp1475->name, _temp1558->name) == 0?
_temp1556->typs != 0: 0){ int okay= 1; for( 0; _temp1552 != 0? _temp1471 != 0: 0;(
_temp1552=(( struct Cyc_List_List*) _check_null( _temp1552))->tl, _temp1471=((
struct Cyc_List_List*) _check_null( _temp1471))->tl)){ if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_List_List*) _check_null( _temp1552))->hd,( void*)(( struct
Cyc_List_List*) _check_null( _temp1471))->hd)){ okay= 0; break;}} if(( ! okay? 1:
_temp1552 != 0)? 1: _temp1471 != 0){ return 0;} return 1;} goto _LL1545; _LL1549:
goto _LL1545; _LL1545:;} return 0; _LL1416: return Cyc_Tcutil_unify( t1, t2);
_LL1406:;}} int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} if( Cyc_Tcutil_is_arithmetic_type(
t1)? Cyc_Tcutil_is_arithmetic_type( t2): 0){ if( Cyc_Tcutil_will_lose_precision(
t1, t2)){ Cyc_Tcutil_warn( e->loc,({ unsigned char* _temp1560="integral size mismatch; conversion supplied";
struct _tagged_string _temp1561; _temp1561.curr= _temp1560; _temp1561.base=
_temp1560; _temp1561.last_plus_one= _temp1560 + 44; _temp1561;}));} Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){
Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2); Cyc_Tcutil_warn( e->loc,({
struct _tagged_string _temp1562= Cyc_Absynpp_typ2string( t1); struct
_tagged_string _temp1563= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1562.last_plus_one - _temp1562.curr, _temp1562.curr, _temp1563.last_plus_one
- _temp1563.curr, _temp1563.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg(
te, e, t);} static int Cyc_Tcutil_any_promote( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e){ void* old_typ= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); void* t;{ void* _temp1564= old_typ; struct Cyc_Absyn_Enumdecl*
_temp1576; struct _tuple1* _temp1578; void* _temp1580; void* _temp1582; _LL1566:
if(( unsigned int) _temp1564 > 4u?*(( int*) _temp1564) == Cyc_Absyn_EnumType: 0){
_LL1579: _temp1578=(( struct Cyc_Absyn_EnumType_struct*) _temp1564)->f1; goto
_LL1577; _LL1577: _temp1576=(( struct Cyc_Absyn_EnumType_struct*) _temp1564)->f2;
goto _LL1567;} else{ goto _LL1568;} _LL1568: if(( unsigned int) _temp1564 > 4u?*((
int*) _temp1564) == Cyc_Absyn_IntType: 0){ _LL1583: _temp1582=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1564)->f1; goto _LL1581; _LL1581: _temp1580=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1564)->f2; goto _LL1569;} else{
goto _LL1570;} _LL1570: if( _temp1564 ==( void*) Cyc_Absyn_FloatType){ goto
_LL1571;} else{ goto _LL1572;} _LL1572: if( _temp1564 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1573;} else{ goto _LL1574;} _LL1574: goto _LL1575; _LL1567: t= Cyc_Absyn_sint_t;
goto _LL1565; _LL1569:{ void* _temp1584= _temp1580; _LL1586: if( _temp1584 ==(
void*) Cyc_Absyn_B1){ goto _LL1587;} else{ goto _LL1588;} _LL1588: if( _temp1584
==( void*) Cyc_Absyn_B2){ goto _LL1589;} else{ goto _LL1590;} _LL1590: if(
_temp1584 ==( void*) Cyc_Absyn_B4){ goto _LL1591;} else{ goto _LL1592;} _LL1592:
if( _temp1584 ==( void*) Cyc_Absyn_B8){ goto _LL1593;} else{ goto _LL1585;}
_LL1587: t= Cyc_Absyn_sint_t; goto _LL1585; _LL1589: t= Cyc_Absyn_sint_t; goto
_LL1585; _LL1591: t= _temp1582 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL1585; _LL1593: t= _temp1582 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_ulong_t: Cyc_Absyn_slong_t; goto _LL1585; _LL1585:;} goto _LL1565;
_LL1571: t= Cyc_Absyn_float_t; goto _LL1565; _LL1573: t= Cyc_Absyn_double_t;
goto _LL1565; _LL1575: return 0; _LL1565:;} Cyc_Tcutil_unchecked_cast( te, e, t);
return 1;} int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} Cyc_Tcutil_any_promote(
te, e); if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v,
t2)){ return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); Cyc_Tcutil_warn( e->loc,({ struct _tagged_string _temp1594= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp1595= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1594.last_plus_one - _temp1594.curr, _temp1594.curr, _temp1595.last_plus_one
- _temp1595.curr, _temp1595.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerceable(
void* t){ void* _temp1596= Cyc_Tcutil_compress( t); void* _temp1606; void*
_temp1608; _LL1598: if(( unsigned int) _temp1596 > 4u?*(( int*) _temp1596) ==
Cyc_Absyn_IntType: 0){ _LL1609: _temp1608=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1596)->f1; goto _LL1607; _LL1607: _temp1606=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1596)->f2; goto _LL1599;} else{ goto _LL1600;} _LL1600: if( _temp1596 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1601;} else{ goto _LL1602;} _LL1602: if(
_temp1596 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1603;} else{ goto _LL1604;}
_LL1604: goto _LL1605; _LL1599: return 1; _LL1601: return 1; _LL1603: return 1;
_LL1605: return 0; _LL1597:;} static struct _tuple4* Cyc_Tcutil_flatten_typ_f(
struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield* x){ return({ struct
_tuple4* _temp1610=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1610->f1= x->tq; _temp1610->f2= Cyc_Tcutil_substitute( inst,( void*) x->type);
_temp1610;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ( struct Cyc_Tcenv_Tenv*
te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void* _temp1611= t1; struct Cyc_List_List*
_temp1623; struct Cyc_Absyn_Structdecl** _temp1625; struct Cyc_List_List*
_temp1627; struct _tuple1* _temp1629; struct Cyc_List_List* _temp1631; _LL1613:
if( _temp1611 ==( void*) Cyc_Absyn_VoidType){ goto _LL1614;} else{ goto _LL1615;}
_LL1615: if(( unsigned int) _temp1611 > 4u?*(( int*) _temp1611) == Cyc_Absyn_TupleType:
0){ _LL1624: _temp1623=(( struct Cyc_Absyn_TupleType_struct*) _temp1611)->f1;
goto _LL1616;} else{ goto _LL1617;} _LL1617: if(( unsigned int) _temp1611 > 4u?*((
int*) _temp1611) == Cyc_Absyn_StructType: 0){ _LL1630: _temp1629=(( struct Cyc_Absyn_StructType_struct*)
_temp1611)->f1; goto _LL1628; _LL1628: _temp1627=(( struct Cyc_Absyn_StructType_struct*)
_temp1611)->f2; goto _LL1626; _LL1626: _temp1625=(( struct Cyc_Absyn_StructType_struct*)
_temp1611)->f3; goto _LL1618;} else{ goto _LL1619;} _LL1619: if(( unsigned int)
_temp1611 > 4u?*(( int*) _temp1611) == Cyc_Absyn_AnonStructType: 0){ _LL1632:
_temp1631=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1611)->f1; goto
_LL1620;} else{ goto _LL1621;} _LL1621: goto _LL1622; _LL1614: return 0; _LL1616:
return _temp1623; _LL1618: if( _temp1625 == 0? 1: _temp1629 == 0){ return({
struct Cyc_List_List* _temp1633=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1633->hd=( void*)({ struct _tuple4* _temp1634=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1634->f1= Cyc_Absyn_empty_tqual();
_temp1634->f2= t1; _temp1634;}); _temp1633->tl= 0; _temp1633;});}{ struct Cyc_Absyn_Structdecl*
_temp1635=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp1625)); goto
_LL1636; _LL1636: { struct _tuple1 _temp1637=*(( struct _tuple1*) _check_null(
_temp1629)); goto _LL1638; _LL1638: { struct Cyc_List_List* _temp1639=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1635->tvs, _temp1627); goto _LL1640; _LL1640: if( _temp1635->fields == 0){
return({ struct Cyc_List_List* _temp1641=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1641->hd=( void*)({ struct _tuple4*
_temp1642=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1642->f1=
Cyc_Absyn_empty_tqual(); _temp1642->f2= t1; _temp1642;}); _temp1641->tl= 0;
_temp1641;});} return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, _temp1639,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1635->fields))->v);}}}
_LL1620: return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, 0, _temp1631); _LL1622: return({
struct Cyc_List_List* _temp1643=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1643->hd=( void*)({ struct _tuple4* _temp1644=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1644->f1= Cyc_Absyn_empty_tqual();
_temp1644->f2= t1; _temp1644;}); _temp1643->tl= 0; _temp1643;}); _LL1612:;}}
static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2); static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;}{ struct Cyc_List_List* a= assume; for( 0; a != 0; a=(( struct
Cyc_List_List*) _check_null( a))->tl){ if( Cyc_Tcutil_unify( t1,(*(( struct
_tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f1)? Cyc_Tcutil_unify(
t2,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f2): 0){
return 1;}}} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct
_tuple8 _temp1646=({ struct _tuple8 _temp1645; _temp1645.f1= t1; _temp1645.f2=
t2; _temp1645;}); void* _temp1652; struct Cyc_Absyn_PtrInfo _temp1654; struct
Cyc_Absyn_Conref* _temp1656; struct Cyc_Absyn_Tqual _temp1658; struct Cyc_Absyn_Conref*
_temp1660; void* _temp1662; void* _temp1664; void* _temp1666; struct Cyc_Absyn_PtrInfo
_temp1668; struct Cyc_Absyn_Conref* _temp1670; struct Cyc_Absyn_Tqual _temp1672;
struct Cyc_Absyn_Conref* _temp1674; void* _temp1676; void* _temp1678; _LL1648:
_LL1667: _temp1666= _temp1646.f1; if(( unsigned int) _temp1666 > 4u?*(( int*)
_temp1666) == Cyc_Absyn_PointerType: 0){ _LL1669: _temp1668=(( struct Cyc_Absyn_PointerType_struct*)
_temp1666)->f1; _LL1679: _temp1678=( void*) _temp1668.elt_typ; goto _LL1677;
_LL1677: _temp1676=( void*) _temp1668.rgn_typ; goto _LL1675; _LL1675: _temp1674=
_temp1668.nullable; goto _LL1673; _LL1673: _temp1672= _temp1668.tq; goto _LL1671;
_LL1671: _temp1670= _temp1668.bounds; goto _LL1653;} else{ goto _LL1650;}
_LL1653: _temp1652= _temp1646.f2; if(( unsigned int) _temp1652 > 4u?*(( int*)
_temp1652) == Cyc_Absyn_PointerType: 0){ _LL1655: _temp1654=(( struct Cyc_Absyn_PointerType_struct*)
_temp1652)->f1; _LL1665: _temp1664=( void*) _temp1654.elt_typ; goto _LL1663;
_LL1663: _temp1662=( void*) _temp1654.rgn_typ; goto _LL1661; _LL1661: _temp1660=
_temp1654.nullable; goto _LL1659; _LL1659: _temp1658= _temp1654.tq; goto _LL1657;
_LL1657: _temp1656= _temp1654.bounds; goto _LL1649;} else{ goto _LL1650;}
_LL1650: goto _LL1651; _LL1649: if( _temp1672.q_const? ! _temp1658.q_const: 0){
return 0;} if(( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1674, _temp1660)?(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1674): 0)? !(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1660): 0){ return 0;}{ void* _temp1680= _temp1676; _LL1682: if( _temp1680
==( void*) Cyc_Absyn_HeapRgn){ goto _LL1683;} else{ goto _LL1684;} _LL1684: goto
_LL1685; _LL1683: goto _LL1681; _LL1685: Cyc_Tcutil_unify( _temp1676, _temp1662);
goto _LL1681; _LL1681:;} if( !(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp,
_temp1670, _temp1656)){ struct _tuple8 _temp1687=({ struct _tuple8 _temp1686;
_temp1686.f1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1670); _temp1686.f2=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1656); _temp1686;}); void* _temp1695; void* _temp1697; struct Cyc_Absyn_Exp*
_temp1699; void* _temp1701; struct Cyc_Absyn_Exp* _temp1703; void* _temp1705;
struct Cyc_Absyn_Exp* _temp1707; _LL1689: _LL1698: _temp1697= _temp1687.f1; if((
unsigned int) _temp1697 > 1u?*(( int*) _temp1697) == Cyc_Absyn_Upper_b: 0){
_LL1700: _temp1699=(( struct Cyc_Absyn_Upper_b_struct*) _temp1697)->f1; goto
_LL1696;} else{ goto _LL1691;} _LL1696: _temp1695= _temp1687.f2; if( _temp1695
==( void*) Cyc_Absyn_Unknown_b){ goto _LL1690;} else{ goto _LL1691;} _LL1691:
_LL1706: _temp1705= _temp1687.f1; if(( unsigned int) _temp1705 > 1u?*(( int*)
_temp1705) == Cyc_Absyn_Upper_b: 0){ _LL1708: _temp1707=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1705)->f1; goto _LL1702;} else{ goto _LL1693;} _LL1702: _temp1701=
_temp1687.f2; if(( unsigned int) _temp1701 > 1u?*(( int*) _temp1701) == Cyc_Absyn_Upper_b:
0){ _LL1704: _temp1703=(( struct Cyc_Absyn_Upper_b_struct*) _temp1701)->f1; goto
_LL1692;} else{ goto _LL1693;} _LL1693: goto _LL1694; _LL1690: goto _LL1688;
_LL1692: if( Cyc_Evexp_eval_const_uint_exp( _temp1707) < Cyc_Evexp_eval_const_uint_exp(
_temp1703)){ return 0;} goto _LL1688; _LL1694: return 0; _LL1688:;} return Cyc_Tcutil_ptrsubtype(
te,({ struct Cyc_List_List* _temp1709=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1709->hd=( void*)({ struct _tuple8*
_temp1710=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp1710->f1=
t1; _temp1710->f2= t2; _temp1710;}); _temp1709->tl= assume; _temp1709;}),
_temp1678, _temp1664); _LL1651: return 0; _LL1647:;}} static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* assume, void* t1, void* t2){
struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ( te, t1); struct Cyc_List_List*
tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0; tqs2 != 0;( tqs2=(( struct Cyc_List_List*)
_check_null( tqs2))->tl, tqs1=(( struct Cyc_List_List*) _check_null( tqs1))->tl)){
if( tqs1 == 0){ return 0;}{ struct _tuple4 _temp1713; void* _temp1714; struct
Cyc_Absyn_Tqual _temp1716; struct _tuple4* _temp1711=( struct _tuple4*)(( struct
Cyc_List_List*) _check_null( tqs1))->hd; _temp1713=* _temp1711; _LL1717:
_temp1716= _temp1713.f1; goto _LL1715; _LL1715: _temp1714= _temp1713.f2; goto
_LL1712; _LL1712: { struct _tuple4 _temp1720; void* _temp1721; struct Cyc_Absyn_Tqual
_temp1723; struct _tuple4* _temp1718=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( tqs2))->hd; _temp1720=* _temp1718; _LL1724: _temp1723= _temp1720.f1;
goto _LL1722; _LL1722: _temp1721= _temp1720.f2; goto _LL1719; _LL1719: if(
_temp1723.q_const? Cyc_Tcutil_subtype( te, assume, _temp1714, _temp1721): 0){
continue;} else{ if( Cyc_Tcutil_unify( _temp1714, _temp1721)){ continue;} else{
return 0;}}}}} return 1;} int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
void* _temp1725= t2; struct Cyc_Absyn_Uniondecl** _temp1733; struct Cyc_List_List*
_temp1735; struct _tuple1* _temp1737; void* _temp1739; void* _temp1741; _LL1727:
if(( unsigned int) _temp1725 > 4u?*(( int*) _temp1725) == Cyc_Absyn_UnionType: 0){
_LL1738: _temp1737=(( struct Cyc_Absyn_UnionType_struct*) _temp1725)->f1; goto
_LL1736; _LL1736: _temp1735=(( struct Cyc_Absyn_UnionType_struct*) _temp1725)->f2;
goto _LL1734; _LL1734: _temp1733=(( struct Cyc_Absyn_UnionType_struct*)
_temp1725)->f3; goto _LL1728;} else{ goto _LL1729;} _LL1729: if(( unsigned int)
_temp1725 > 4u?*(( int*) _temp1725) == Cyc_Absyn_IntType: 0){ _LL1742: _temp1741=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1725)->f1; goto _LL1740; _LL1740:
_temp1739=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1725)->f2; if(
_temp1739 ==( void*) Cyc_Absyn_B4){ goto _LL1730;} else{ goto _LL1731;}} else{
goto _LL1731;} _LL1731: goto _LL1732; _LL1728: if( _temp1733 == 0){ goto _LL1726;}{
struct Cyc_Absyn_Uniondecl* _temp1743=*(( struct Cyc_Absyn_Uniondecl**)
_check_null( _temp1733)); goto _LL1744; _LL1744: if( _temp1743->fields == 0){
goto _LL1726;}{ struct Cyc_List_List* _temp1745=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp1743->tvs,
_temp1735); goto _LL1746; _LL1746:{ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1743->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ if( Cyc_Tcutil_unify( t1, Cyc_Tcutil_substitute(
_temp1745,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ return 1;}}} goto _LL1726;}} _LL1730: if( Cyc_Tcutil_typ_kind(
t2) ==( void*) Cyc_Absyn_BoxKind){ return 1;} goto _LL1726; _LL1732: goto
_LL1726; _LL1726:;}{ void* _temp1747= t1; struct Cyc_Absyn_PtrInfo _temp1763;
struct Cyc_Absyn_Conref* _temp1765; struct Cyc_Absyn_Tqual _temp1767; struct Cyc_Absyn_Conref*
_temp1769; void* _temp1771; void* _temp1773; struct Cyc_Absyn_Exp* _temp1775;
struct Cyc_Absyn_Tqual _temp1777; void* _temp1779; struct Cyc_Absyn_Enumdecl*
_temp1781; struct _tuple1* _temp1783; void* _temp1785; void* _temp1787; _LL1749:
if(( unsigned int) _temp1747 > 4u?*(( int*) _temp1747) == Cyc_Absyn_PointerType:
0){ _LL1764: _temp1763=(( struct Cyc_Absyn_PointerType_struct*) _temp1747)->f1;
_LL1774: _temp1773=( void*) _temp1763.elt_typ; goto _LL1772; _LL1772: _temp1771=(
void*) _temp1763.rgn_typ; goto _LL1770; _LL1770: _temp1769= _temp1763.nullable;
goto _LL1768; _LL1768: _temp1767= _temp1763.tq; goto _LL1766; _LL1766: _temp1765=
_temp1763.bounds; goto _LL1750;} else{ goto _LL1751;} _LL1751: if(( unsigned int)
_temp1747 > 4u?*(( int*) _temp1747) == Cyc_Absyn_ArrayType: 0){ _LL1780:
_temp1779=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1747)->f1; goto
_LL1778; _LL1778: _temp1777=(( struct Cyc_Absyn_ArrayType_struct*) _temp1747)->f2;
goto _LL1776; _LL1776: _temp1775=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1747)->f3; goto _LL1752;} else{ goto _LL1753;} _LL1753: if(( unsigned int)
_temp1747 > 4u?*(( int*) _temp1747) == Cyc_Absyn_EnumType: 0){ _LL1784:
_temp1783=(( struct Cyc_Absyn_EnumType_struct*) _temp1747)->f1; goto _LL1782;
_LL1782: _temp1781=(( struct Cyc_Absyn_EnumType_struct*) _temp1747)->f2; goto
_LL1754;} else{ goto _LL1755;} _LL1755: if(( unsigned int) _temp1747 > 4u?*((
int*) _temp1747) == Cyc_Absyn_IntType: 0){ _LL1788: _temp1787=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1747)->f1; goto _LL1786; _LL1786: _temp1785=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1747)->f2; goto _LL1756;} else{
goto _LL1757;} _LL1757: if( _temp1747 ==( void*) Cyc_Absyn_FloatType){ goto
_LL1758;} else{ goto _LL1759;} _LL1759: if( _temp1747 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1760;} else{ goto _LL1761;} _LL1761: goto _LL1762; _LL1750:{ void*
_temp1789= t2; struct Cyc_Absyn_PtrInfo _temp1795; struct Cyc_Absyn_Conref*
_temp1797; struct Cyc_Absyn_Tqual _temp1799; struct Cyc_Absyn_Conref* _temp1801;
void* _temp1803; void* _temp1805; _LL1791: if(( unsigned int) _temp1789 > 4u?*((
int*) _temp1789) == Cyc_Absyn_PointerType: 0){ _LL1796: _temp1795=(( struct Cyc_Absyn_PointerType_struct*)
_temp1789)->f1; _LL1806: _temp1805=( void*) _temp1795.elt_typ; goto _LL1804;
_LL1804: _temp1803=( void*) _temp1795.rgn_typ; goto _LL1802; _LL1802: _temp1801=
_temp1795.nullable; goto _LL1800; _LL1800: _temp1799= _temp1795.tq; goto _LL1798;
_LL1798: _temp1797= _temp1795.bounds; goto _LL1792;} else{ goto _LL1793;}
_LL1793: goto _LL1794; _LL1792: { struct Cyc_List_List* _temp1809=({ struct Cyc_List_List*
_temp1807=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1807->hd=( void*)({ struct _tuple8* _temp1808=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp1808->f1= t1; _temp1808->f2= t2; _temp1808;});
_temp1807->tl= 0; _temp1807;}); goto _LL1810; _LL1810: { int _temp1811= Cyc_Tcutil_ptrsubtype(
te, _temp1809, _temp1773, _temp1805)? ! _temp1767.q_const? 1: _temp1799.q_const:
0; goto _LL1812; _LL1812: { int bounds_ok=(( int(*)( int(* cmp)( void*, void*),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp1765, _temp1797); if( ! bounds_ok){ struct _tuple8
_temp1814=({ struct _tuple8 _temp1813; _temp1813.f1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp1765); _temp1813.f2=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp1797); _temp1813;}); void* _temp1820; struct Cyc_Absyn_Exp*
_temp1822; void* _temp1824; struct Cyc_Absyn_Exp* _temp1826; _LL1816: _LL1825:
_temp1824= _temp1814.f1; if(( unsigned int) _temp1824 > 1u?*(( int*) _temp1824)
== Cyc_Absyn_Upper_b: 0){ _LL1827: _temp1826=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1824)->f1; goto _LL1821;} else{ goto _LL1818;} _LL1821: _temp1820=
_temp1814.f2; if(( unsigned int) _temp1820 > 1u?*(( int*) _temp1820) == Cyc_Absyn_Upper_b:
0){ _LL1823: _temp1822=(( struct Cyc_Absyn_Upper_b_struct*) _temp1820)->f1; goto
_LL1817;} else{ goto _LL1818;} _LL1818: goto _LL1819; _LL1817: if( Cyc_Evexp_eval_const_uint_exp(
_temp1826) >= Cyc_Evexp_eval_const_uint_exp( _temp1822)){ bounds_ok= 1;} goto
_LL1815; _LL1819: bounds_ok= 1; goto _LL1815; _LL1815:;} return( bounds_ok?
_temp1811: 0)? _temp1771 ==( void*) Cyc_Absyn_HeapRgn? 1: Cyc_Tcutil_unify(
_temp1771, _temp1803): 0;}}} _LL1794: goto _LL1790; _LL1790:;} return 0; _LL1752:{
void* _temp1828= t2; struct Cyc_Absyn_Exp* _temp1834; struct Cyc_Absyn_Tqual
_temp1836; void* _temp1838; _LL1830: if(( unsigned int) _temp1828 > 4u?*(( int*)
_temp1828) == Cyc_Absyn_ArrayType: 0){ _LL1839: _temp1838=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1828)->f1; goto _LL1837; _LL1837: _temp1836=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1828)->f2; goto _LL1835; _LL1835: _temp1834=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1828)->f3; goto _LL1831;} else{ goto _LL1832;} _LL1832: goto _LL1833;
_LL1831: { int okay; okay=( _temp1775 != 0? _temp1834 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1775)) >= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1834)): 0; return( okay? Cyc_Tcutil_unify(
_temp1779, _temp1838): 0)? ! _temp1777.q_const? 1: _temp1836.q_const: 0;}
_LL1833: return 0; _LL1829:;} return 0; _LL1754:{ void* _temp1840= t2; struct
Cyc_Absyn_Enumdecl* _temp1846; struct _tuple1* _temp1848; _LL1842: if((
unsigned int) _temp1840 > 4u?*(( int*) _temp1840) == Cyc_Absyn_EnumType: 0){
_LL1849: _temp1848=(( struct Cyc_Absyn_EnumType_struct*) _temp1840)->f1; goto
_LL1847; _LL1847: _temp1846=(( struct Cyc_Absyn_EnumType_struct*) _temp1840)->f2;
goto _LL1843;} else{ goto _LL1844;} _LL1844: goto _LL1845; _LL1843: if((((
struct Cyc_Absyn_Enumdecl*) _check_null( _temp1781))->fields != 0?(( struct Cyc_Absyn_Enumdecl*)
_check_null( _temp1846))->fields != 0: 0)?(( int(*)( struct Cyc_List_List* x))
Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(((
struct Cyc_Absyn_Enumdecl*) _check_null( _temp1781))->fields))->v) >=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Enumdecl*) _check_null( _temp1846))->fields))->v):
0){ return 1;} goto _LL1841; _LL1845: goto _LL1841; _LL1841:;} goto _LL1756;
_LL1756: goto _LL1758; _LL1758: goto _LL1760; _LL1760: return Cyc_Tcutil_coerceable(
t2); _LL1762: return 0; _LL1748:;}} void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, void* t){ if( ! Cyc_Tcutil_unify(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v, t)){ struct Cyc_Absyn_Exp* _temp1850=
Cyc_Absyn_copy_exp( e); goto _LL1851; _LL1851:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Cast_e_struct* _temp1852=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp1852[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp1853; _temp1853.tag= Cyc_Absyn_Cast_e; _temp1853.f1=( void*) t; _temp1853.f2=
_temp1850; _temp1853;}); _temp1852;}))); e->topt=({ struct Cyc_Core_Opt*
_temp1854=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1854->v=( void*) t; _temp1854;});}} int Cyc_Tcutil_is_integral( struct Cyc_Absyn_Exp*
e){ void* _temp1855= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); struct Cyc_Absyn_Enumdecl* _temp1865; struct _tuple1*
_temp1867; void* _temp1869; void* _temp1871; struct Cyc_Core_Opt* _temp1873; int
_temp1875; struct Cyc_Core_Opt* _temp1877; struct Cyc_Core_Opt* _temp1879;
_LL1857: if(( unsigned int) _temp1855 > 4u?*(( int*) _temp1855) == Cyc_Absyn_EnumType:
0){ _LL1868: _temp1867=(( struct Cyc_Absyn_EnumType_struct*) _temp1855)->f1;
goto _LL1866; _LL1866: _temp1865=(( struct Cyc_Absyn_EnumType_struct*) _temp1855)->f2;
goto _LL1858;} else{ goto _LL1859;} _LL1859: if(( unsigned int) _temp1855 > 4u?*((
int*) _temp1855) == Cyc_Absyn_IntType: 0){ _LL1872: _temp1871=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1855)->f1; goto _LL1870; _LL1870: _temp1869=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1855)->f2; goto _LL1860;} else{
goto _LL1861;} _LL1861: if(( unsigned int) _temp1855 > 4u?*(( int*) _temp1855)
== Cyc_Absyn_Evar: 0){ _LL1880: _temp1879=(( struct Cyc_Absyn_Evar_struct*)
_temp1855)->f1; goto _LL1878; _LL1878: _temp1877=(( struct Cyc_Absyn_Evar_struct*)
_temp1855)->f2; goto _LL1876; _LL1876: _temp1875=(( struct Cyc_Absyn_Evar_struct*)
_temp1855)->f3; goto _LL1874; _LL1874: _temp1873=(( struct Cyc_Absyn_Evar_struct*)
_temp1855)->f4; goto _LL1862;} else{ goto _LL1863;} _LL1863: goto _LL1864;
_LL1858: goto _LL1860; _LL1860: return 1; _LL1862: if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;}
return 0; _LL1864: return 0; _LL1856:;} int Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp*
e){ if( Cyc_Tcutil_is_integral( e)){ return 1;}{ void* _temp1881= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _LL1883: if(
_temp1881 ==( void*) Cyc_Absyn_FloatType){ goto _LL1884;} else{ goto _LL1885;}
_LL1885: if( _temp1881 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1886;} else{
goto _LL1887;} _LL1887: goto _LL1888; _LL1884: goto _LL1886; _LL1886: return 1;
_LL1888: return 0; _LL1882:;}} int Cyc_Tcutil_is_function_type( void* t){ void*
_temp1889= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp1895; _LL1891:
if(( unsigned int) _temp1889 > 4u?*(( int*) _temp1889) == Cyc_Absyn_FnType: 0){
_LL1896: _temp1895=(( struct Cyc_Absyn_FnType_struct*) _temp1889)->f1; goto
_LL1892;} else{ goto _LL1893;} _LL1893: goto _LL1894; _LL1892: return 1; _LL1894:
return 0; _LL1890:;} void* Cyc_Tcutil_max_arithmetic_type( void* t1, void* t2){
struct _tuple8 _temp1898=({ struct _tuple8 _temp1897; _temp1897.f1= t1;
_temp1897.f2= t2; _temp1897;}); void* _temp1922; void* _temp1924; void*
_temp1926; void* _temp1928; void* _temp1930; void* _temp1932; void* _temp1934;
void* _temp1936; void* _temp1938; void* _temp1940; void* _temp1942; void*
_temp1944; void* _temp1946; void* _temp1948; void* _temp1950; void* _temp1952;
void* _temp1954; void* _temp1956; void* _temp1958; void* _temp1960; void*
_temp1962; void* _temp1964; void* _temp1966; void* _temp1968; void* _temp1970;
void* _temp1972; void* _temp1974; void* _temp1976; void* _temp1978; void*
_temp1980; void* _temp1982; void* _temp1984; _LL1900: _LL1925: _temp1924=
_temp1898.f1; if( _temp1924 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1923;}
else{ goto _LL1902;} _LL1923: _temp1922= _temp1898.f2; goto _LL1901; _LL1902:
_LL1929: _temp1928= _temp1898.f1; goto _LL1927; _LL1927: _temp1926= _temp1898.f2;
if( _temp1926 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1903;} else{ goto
_LL1904;} _LL1904: _LL1933: _temp1932= _temp1898.f1; if( _temp1932 ==( void*)
Cyc_Absyn_FloatType){ goto _LL1931;} else{ goto _LL1906;} _LL1931: _temp1930=
_temp1898.f2; goto _LL1905; _LL1906: _LL1937: _temp1936= _temp1898.f1; goto
_LL1935; _LL1935: _temp1934= _temp1898.f2; if( _temp1934 ==( void*) Cyc_Absyn_FloatType){
goto _LL1907;} else{ goto _LL1908;} _LL1908: _LL1941: _temp1940= _temp1898.f1;
if(( unsigned int) _temp1940 > 4u?*(( int*) _temp1940) == Cyc_Absyn_IntType: 0){
_LL1945: _temp1944=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1940)->f1;
if( _temp1944 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1943;} else{ goto _LL1910;}
_LL1943: _temp1942=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1940)->f2;
if( _temp1942 ==( void*) Cyc_Absyn_B8){ goto _LL1939;} else{ goto _LL1910;}}
else{ goto _LL1910;} _LL1939: _temp1938= _temp1898.f2; goto _LL1909; _LL1910:
_LL1953: _temp1952= _temp1898.f1; goto _LL1947; _LL1947: _temp1946= _temp1898.f2;
if(( unsigned int) _temp1946 > 4u?*(( int*) _temp1946) == Cyc_Absyn_IntType: 0){
_LL1951: _temp1950=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1946)->f1;
if( _temp1950 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1949;} else{ goto _LL1912;}
_LL1949: _temp1948=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1946)->f2;
if( _temp1948 ==( void*) Cyc_Absyn_B8){ goto _LL1911;} else{ goto _LL1912;}}
else{ goto _LL1912;} _LL1912: _LL1957: _temp1956= _temp1898.f1; if((
unsigned int) _temp1956 > 4u?*(( int*) _temp1956) == Cyc_Absyn_IntType: 0){
_LL1961: _temp1960=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1956)->f1;
if( _temp1960 ==( void*) Cyc_Absyn_Signed){ goto _LL1959;} else{ goto _LL1914;}
_LL1959: _temp1958=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1956)->f2;
if( _temp1958 ==( void*) Cyc_Absyn_B8){ goto _LL1955;} else{ goto _LL1914;}}
else{ goto _LL1914;} _LL1955: _temp1954= _temp1898.f2; goto _LL1913; _LL1914:
_LL1969: _temp1968= _temp1898.f1; goto _LL1963; _LL1963: _temp1962= _temp1898.f2;
if(( unsigned int) _temp1962 > 4u?*(( int*) _temp1962) == Cyc_Absyn_IntType: 0){
_LL1967: _temp1966=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1962)->f1;
if( _temp1966 ==( void*) Cyc_Absyn_Signed){ goto _LL1965;} else{ goto _LL1916;}
_LL1965: _temp1964=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1962)->f2;
if( _temp1964 ==( void*) Cyc_Absyn_B8){ goto _LL1915;} else{ goto _LL1916;}}
else{ goto _LL1916;} _LL1916: _LL1973: _temp1972= _temp1898.f1; if((
unsigned int) _temp1972 > 4u?*(( int*) _temp1972) == Cyc_Absyn_IntType: 0){
_LL1977: _temp1976=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1972)->f1;
if( _temp1976 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1975;} else{ goto _LL1918;}
_LL1975: _temp1974=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1972)->f2;
if( _temp1974 ==( void*) Cyc_Absyn_B4){ goto _LL1971;} else{ goto _LL1918;}}
else{ goto _LL1918;} _LL1971: _temp1970= _temp1898.f2; goto _LL1917; _LL1918:
_LL1985: _temp1984= _temp1898.f1; goto _LL1979; _LL1979: _temp1978= _temp1898.f2;
if(( unsigned int) _temp1978 > 4u?*(( int*) _temp1978) == Cyc_Absyn_IntType: 0){
_LL1983: _temp1982=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1978)->f1;
if( _temp1982 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1981;} else{ goto _LL1920;}
_LL1981: _temp1980=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1978)->f2;
if( _temp1980 ==( void*) Cyc_Absyn_B4){ goto _LL1919;} else{ goto _LL1920;}}
else{ goto _LL1920;} _LL1920: goto _LL1921; _LL1901: goto _LL1903; _LL1903:
return( void*) Cyc_Absyn_DoubleType; _LL1905: goto _LL1907; _LL1907: return(
void*) Cyc_Absyn_FloatType; _LL1909: goto _LL1911; _LL1911: return Cyc_Absyn_ulong_t;
_LL1913: goto _LL1915; _LL1915: return Cyc_Absyn_slong_t; _LL1917: goto _LL1919;
_LL1919: return Cyc_Absyn_uint_t; _LL1921: return Cyc_Absyn_sint_t; _LL1899:;}
void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp* e){ void* _temp1986=(
void*) e->r; struct Cyc_Absyn_Exp* _temp1992; struct Cyc_Core_Opt* _temp1994;
struct Cyc_Absyn_Exp* _temp1996; _LL1988: if(*(( int*) _temp1986) == Cyc_Absyn_AssignOp_e){
_LL1997: _temp1996=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp1986)->f1; goto
_LL1995; _LL1995: _temp1994=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp1986)->f2;
if( _temp1994 == 0){ goto _LL1993;} else{ goto _LL1990;} _LL1993: _temp1992=((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1986)->f3; goto _LL1989;} else{ goto
_LL1990;} _LL1990: goto _LL1991; _LL1989: Cyc_Tcutil_warn( e->loc,({
unsigned char* _temp1998="assignment in test"; struct _tagged_string _temp1999;
_temp1999.curr= _temp1998; _temp1999.base= _temp1998; _temp1999.last_plus_one=
_temp1998 + 19; _temp1999;})); goto _LL1987; _LL1991: goto _LL1987; _LL1987:;}
static int Cyc_Tcutil_constrain_kinds( struct Cyc_Absyn_Conref* c1, struct Cyc_Absyn_Conref*
c2){ c1=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
c1); c2=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
c2); if( c1 == c2){ return 1;} if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr){(
void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp2000=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp2000[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp2001; _temp2001.tag= Cyc_Absyn_Forward_constr; _temp2001.f1= c2; _temp2001;});
_temp2000;}))); return 1;} else{ if(( void*) c2->v ==( void*) Cyc_Absyn_No_constr){(
void*)( c2->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp2002=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp2002[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp2003; _temp2003.tag= Cyc_Absyn_Forward_constr; _temp2003.f1= c1; _temp2003;});
_temp2002;}))); return 1;} else{ void* k1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( c1); void* k2=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( c2); if( Cyc_Tcutil_kind_leq( k1, k2)){( void*)( c2->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2004=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2004[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2005; _temp2005.tag= Cyc_Absyn_Forward_constr;
_temp2005.f1= c1; _temp2005;}); _temp2004;}))); return 1;} else{ if( Cyc_Tcutil_kind_leq(
k2, k1)){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp2006=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp2006[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp2007; _temp2007.tag= Cyc_Absyn_Forward_constr; _temp2007.f1= c2; _temp2007;});
_temp2006;}))); return 1;} else{ return 0;}}}}} static int Cyc_Tcutil_tvar_id_counter=
0; int* Cyc_Tcutil_new_tvar_id(){ return({ int* _temp2008=( int*)
GC_malloc_atomic( sizeof( int)); _temp2008[ 0]= Cyc_Tcutil_tvar_id_counter ++;
_temp2008;});} static int Cyc_Tcutil_tvar_counter= 0; struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar(
void* k){ int i= Cyc_Tcutil_tvar_counter ++; struct _tagged_string s= xprintf("#%d",
i); return({ struct Cyc_Absyn_Tvar* _temp2009=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2009->name=({ struct
_tagged_string* _temp2010=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2010[ 0]= s; _temp2010;}); _temp2009->identity= 0;
_temp2009->kind=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
k); _temp2009;});} int Cyc_Tcutil_is_temp_tvar( struct Cyc_Absyn_Tvar* t){
struct _tagged_string _temp2011=* t->name; goto _LL2012; _LL2012: return*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( _temp2011, sizeof( unsigned char), 0) =='#';} void
Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar* t){ if( ! Cyc_Tcutil_is_temp_tvar(
t)){ return;}{ struct _tagged_string _temp2015= Cyc_String_strconcat(({
unsigned char* _temp2013="`"; struct _tagged_string _temp2014; _temp2014.curr=
_temp2013; _temp2014.base= _temp2013; _temp2014.last_plus_one= _temp2013 + 2;
_temp2014;}),* t->name); goto _LL2016; _LL2016:*(( unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
_temp2015, sizeof( unsigned char), 1)='t'; t->name=({ struct _tagged_string*
_temp2017=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2017[ 0]= _temp2015; _temp2017;});}} struct _tuple13{ struct
_tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; static struct
_tuple2* Cyc_Tcutil_fndecl2typ_f( struct _tuple13* x){ return({ struct _tuple2*
_temp2018=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp2018->f1=(
struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp2019=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2019->v=( void*)(* x).f1;
_temp2019;}); _temp2018->f2=(* x).f2; _temp2018->f3=(* x).f3; _temp2018;});}
void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl* fd){ if( fd->cached_typ ==
0){ struct Cyc_List_List* _temp2020= 0; goto _LL2021; _LL2021:{ struct Cyc_List_List*
atts= fd->attributes; for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ if( Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd)){ _temp2020=({ struct Cyc_List_List* _temp2022=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2022->hd=( void*)((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd); _temp2022->tl=
_temp2020; _temp2022;});}}} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2023=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2023[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2024; _temp2024.tag= Cyc_Absyn_FnType;
_temp2024.f1=({ struct Cyc_Absyn_FnInfo _temp2025; _temp2025.tvars= fd->tvs;
_temp2025.effect= fd->effect; _temp2025.ret_typ=( void*)(( void*) fd->ret_type);
_temp2025.args=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct
_tuple13*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_fndecl2typ_f, fd->args);
_temp2025.c_varargs= fd->c_varargs; _temp2025.cyc_varargs= fd->cyc_varargs;
_temp2025.attributes= _temp2020; _temp2025;}); _temp2024;}); _temp2023;});}
return( void*)(( struct Cyc_Core_Opt*) _check_null( fd->cached_typ))->v;} struct
_tuple14{ void* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; static void* Cyc_Tcutil_fst_fdarg(
struct _tuple14* t){ return(* t).f1;} void* Cyc_Tcutil_snd_tqt( struct _tuple4*
t){ return(* t).f2;} static struct _tuple4* Cyc_Tcutil_map2_tq( struct _tuple4*
pr, void* t){ return({ struct _tuple4* _temp2026=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp2026->f1=(* pr).f1; _temp2026->f2= t; _temp2026;});}
void Cyc_Tcutil_check_fndecl_valid_type( struct Cyc_Position_Segment* loc,
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl* fd){ void* t= Cyc_Tcutil_fndecl2typ(
fd); Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);{ void* _temp2027= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_FnInfo _temp2033; struct Cyc_List_List* _temp2035; struct
Cyc_Absyn_VarargInfo* _temp2037; int _temp2039; struct Cyc_List_List* _temp2041;
void* _temp2043; struct Cyc_Core_Opt* _temp2045; struct Cyc_List_List* _temp2047;
_LL2029: if(( unsigned int) _temp2027 > 4u?*(( int*) _temp2027) == Cyc_Absyn_FnType:
0){ _LL2034: _temp2033=(( struct Cyc_Absyn_FnType_struct*) _temp2027)->f1;
_LL2048: _temp2047= _temp2033.tvars; goto _LL2046; _LL2046: _temp2045= _temp2033.effect;
goto _LL2044; _LL2044: _temp2043=( void*) _temp2033.ret_typ; goto _LL2042;
_LL2042: _temp2041= _temp2033.args; goto _LL2040; _LL2040: _temp2039= _temp2033.c_varargs;
goto _LL2038; _LL2038: _temp2037= _temp2033.cyc_varargs; goto _LL2036; _LL2036:
_temp2035= _temp2033.attributes; goto _LL2030;} else{ goto _LL2031;} _LL2031:
goto _LL2032; _LL2030: fd->tvs= _temp2047; fd->effect= _temp2045; goto _LL2028;
_LL2032:(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
unsigned char* _temp2049="check_fndecl_valid_type: not a FnType"; struct
_tagged_string _temp2050; _temp2050.curr= _temp2049; _temp2050.base= _temp2049;
_temp2050.last_plus_one= _temp2049 + 38; _temp2050;})); return; _LL2028:;}{
struct _RegionHandle _temp2051= _new_region(); struct _RegionHandle* r=&
_temp2051; _push_region( r); Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tagged_string*(* f)( struct _tuple13*), struct
Cyc_List_List* x)) Cyc_List_rmap)( r,( struct _tagged_string*(*)( struct
_tuple13* t)) Cyc_Tcutil_fst_fdarg, fd->args), loc,({ unsigned char* _temp2052="function declaration has repeated parameter";
struct _tagged_string _temp2053; _temp2053.curr= _temp2052; _temp2053.base=
_temp2052; _temp2053.last_plus_one= _temp2052 + 44; _temp2053;}));; _pop_region(&
_temp2051);} fd->cached_typ=({ struct Cyc_Core_Opt* _temp2054=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2054->v=( void*) t; _temp2054;});}
struct _tuple15{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; } ; struct
_tuple16{ struct _tuple15* f1; void* f2; } ; static struct _tuple16* Cyc_Tcutil_substitute_f1(
struct _RegionHandle* rgn, struct _tuple2* y){ return({ struct _tuple16*
_temp2055=( struct _tuple16*) _region_malloc( rgn, sizeof( struct _tuple16));
_temp2055->f1=({ struct _tuple15* _temp2056=( struct _tuple15*) _region_malloc(
rgn, sizeof( struct _tuple15)); _temp2056->f1=(* y).f1; _temp2056->f2=(* y).f2;
_temp2056;}); _temp2055->f2=(* y).f3; _temp2055;});} static struct _tuple2* Cyc_Tcutil_substitute_f2(
struct _tuple16* w){ void* _temp2059; struct _tuple15* _temp2061; struct
_tuple16 _temp2057=* w; _LL2062: _temp2061= _temp2057.f1; goto _LL2060; _LL2060:
_temp2059= _temp2057.f2; goto _LL2058; _LL2058: { struct Cyc_Absyn_Tqual
_temp2065; struct Cyc_Core_Opt* _temp2067; struct _tuple15 _temp2063=* _temp2061;
_LL2068: _temp2067= _temp2063.f1; goto _LL2066; _LL2066: _temp2065= _temp2063.f2;
goto _LL2064; _LL2064: return({ struct _tuple2* _temp2069=( struct _tuple2*)
GC_malloc( sizeof( struct _tuple2)); _temp2069->f1= _temp2067; _temp2069->f2=
_temp2065; _temp2069->f3= _temp2059; _temp2069;});}} static void* Cyc_Tcutil_field_type(
struct Cyc_Absyn_Structfield* f){ return( void*) f->type;} static struct Cyc_Absyn_Structfield*
Cyc_Tcutil_zip_field_type( struct Cyc_Absyn_Structfield* f, void* t){ return({
struct Cyc_Absyn_Structfield* _temp2070=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2070->name= f->name;
_temp2070->tq= f->tq; _temp2070->type=( void*) t; _temp2070->width= f->width;
_temp2070->attributes= f->attributes; _temp2070;});} static struct Cyc_List_List*
Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List* inst, struct
Cyc_List_List* ts); void* Cyc_Tcutil_rsubstitute( struct _RegionHandle* rgn,
struct Cyc_List_List* inst, void* t){ void* _temp2071= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp2117; struct Cyc_Absyn_Structdecl** _temp2119;
struct Cyc_List_List* _temp2121; struct _tuple1* _temp2123; struct Cyc_Absyn_Uniondecl**
_temp2125; struct Cyc_List_List* _temp2127; struct _tuple1* _temp2129; struct
Cyc_Absyn_TunionInfo _temp2131; void* _temp2133; struct Cyc_List_List* _temp2135;
void* _temp2137; struct Cyc_Absyn_TunionFieldInfo _temp2139; struct Cyc_List_List*
_temp2141; void* _temp2143; struct Cyc_Core_Opt* _temp2145; struct Cyc_List_List*
_temp2147; struct _tuple1* _temp2149; struct Cyc_Absyn_Exp* _temp2151; struct
Cyc_Absyn_Tqual _temp2153; void* _temp2155; struct Cyc_Absyn_PtrInfo _temp2157;
struct Cyc_Absyn_Conref* _temp2159; struct Cyc_Absyn_Tqual _temp2161; struct Cyc_Absyn_Conref*
_temp2163; void* _temp2165; void* _temp2167; struct Cyc_Absyn_FnInfo _temp2169;
struct Cyc_List_List* _temp2171; struct Cyc_Absyn_VarargInfo* _temp2173; int
_temp2175; struct Cyc_List_List* _temp2177; void* _temp2179; struct Cyc_Core_Opt*
_temp2181; struct Cyc_List_List* _temp2183; struct Cyc_List_List* _temp2185;
struct Cyc_List_List* _temp2187; struct Cyc_List_List* _temp2189; struct Cyc_Core_Opt*
_temp2191; int _temp2193; struct Cyc_Core_Opt* _temp2195; struct Cyc_Core_Opt*
_temp2197; void* _temp2199; struct Cyc_Absyn_Enumdecl* _temp2201; struct _tuple1*
_temp2203; void* _temp2205; void* _temp2207; void* _temp2209; struct Cyc_List_List*
_temp2211; _LL2073: if(( unsigned int) _temp2071 > 4u?*(( int*) _temp2071) ==
Cyc_Absyn_VarType: 0){ _LL2118: _temp2117=(( struct Cyc_Absyn_VarType_struct*)
_temp2071)->f1; goto _LL2074;} else{ goto _LL2075;} _LL2075: if(( unsigned int)
_temp2071 > 4u?*(( int*) _temp2071) == Cyc_Absyn_StructType: 0){ _LL2124:
_temp2123=(( struct Cyc_Absyn_StructType_struct*) _temp2071)->f1; goto _LL2122;
_LL2122: _temp2121=(( struct Cyc_Absyn_StructType_struct*) _temp2071)->f2; goto
_LL2120; _LL2120: _temp2119=(( struct Cyc_Absyn_StructType_struct*) _temp2071)->f3;
goto _LL2076;} else{ goto _LL2077;} _LL2077: if(( unsigned int) _temp2071 > 4u?*((
int*) _temp2071) == Cyc_Absyn_UnionType: 0){ _LL2130: _temp2129=(( struct Cyc_Absyn_UnionType_struct*)
_temp2071)->f1; goto _LL2128; _LL2128: _temp2127=(( struct Cyc_Absyn_UnionType_struct*)
_temp2071)->f2; goto _LL2126; _LL2126: _temp2125=(( struct Cyc_Absyn_UnionType_struct*)
_temp2071)->f3; goto _LL2078;} else{ goto _LL2079;} _LL2079: if(( unsigned int)
_temp2071 > 4u?*(( int*) _temp2071) == Cyc_Absyn_TunionType: 0){ _LL2132:
_temp2131=(( struct Cyc_Absyn_TunionType_struct*) _temp2071)->f1; _LL2138:
_temp2137=( void*) _temp2131.tunion_info; goto _LL2136; _LL2136: _temp2135=
_temp2131.targs; goto _LL2134; _LL2134: _temp2133=( void*) _temp2131.rgn; goto
_LL2080;} else{ goto _LL2081;} _LL2081: if(( unsigned int) _temp2071 > 4u?*((
int*) _temp2071) == Cyc_Absyn_TunionFieldType: 0){ _LL2140: _temp2139=(( struct
Cyc_Absyn_TunionFieldType_struct*) _temp2071)->f1; _LL2144: _temp2143=( void*)
_temp2139.field_info; goto _LL2142; _LL2142: _temp2141= _temp2139.targs; goto
_LL2082;} else{ goto _LL2083;} _LL2083: if(( unsigned int) _temp2071 > 4u?*((
int*) _temp2071) == Cyc_Absyn_TypedefType: 0){ _LL2150: _temp2149=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2071)->f1; goto _LL2148; _LL2148: _temp2147=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2071)->f2; goto _LL2146; _LL2146: _temp2145=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2071)->f3; goto _LL2084;} else{ goto _LL2085;} _LL2085: if(( unsigned int)
_temp2071 > 4u?*(( int*) _temp2071) == Cyc_Absyn_ArrayType: 0){ _LL2156:
_temp2155=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2071)->f1; goto
_LL2154; _LL2154: _temp2153=(( struct Cyc_Absyn_ArrayType_struct*) _temp2071)->f2;
goto _LL2152; _LL2152: _temp2151=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2071)->f3; goto _LL2086;} else{ goto _LL2087;} _LL2087: if(( unsigned int)
_temp2071 > 4u?*(( int*) _temp2071) == Cyc_Absyn_PointerType: 0){ _LL2158:
_temp2157=(( struct Cyc_Absyn_PointerType_struct*) _temp2071)->f1; _LL2168:
_temp2167=( void*) _temp2157.elt_typ; goto _LL2166; _LL2166: _temp2165=( void*)
_temp2157.rgn_typ; goto _LL2164; _LL2164: _temp2163= _temp2157.nullable; goto
_LL2162; _LL2162: _temp2161= _temp2157.tq; goto _LL2160; _LL2160: _temp2159=
_temp2157.bounds; goto _LL2088;} else{ goto _LL2089;} _LL2089: if(( unsigned int)
_temp2071 > 4u?*(( int*) _temp2071) == Cyc_Absyn_FnType: 0){ _LL2170: _temp2169=((
struct Cyc_Absyn_FnType_struct*) _temp2071)->f1; _LL2184: _temp2183= _temp2169.tvars;
goto _LL2182; _LL2182: _temp2181= _temp2169.effect; goto _LL2180; _LL2180:
_temp2179=( void*) _temp2169.ret_typ; goto _LL2178; _LL2178: _temp2177=
_temp2169.args; goto _LL2176; _LL2176: _temp2175= _temp2169.c_varargs; goto
_LL2174; _LL2174: _temp2173= _temp2169.cyc_varargs; goto _LL2172; _LL2172:
_temp2171= _temp2169.attributes; goto _LL2090;} else{ goto _LL2091;} _LL2091:
if(( unsigned int) _temp2071 > 4u?*(( int*) _temp2071) == Cyc_Absyn_TupleType: 0){
_LL2186: _temp2185=(( struct Cyc_Absyn_TupleType_struct*) _temp2071)->f1; goto
_LL2092;} else{ goto _LL2093;} _LL2093: if(( unsigned int) _temp2071 > 4u?*((
int*) _temp2071) == Cyc_Absyn_AnonStructType: 0){ _LL2188: _temp2187=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp2071)->f1; goto _LL2094;} else{ goto
_LL2095;} _LL2095: if(( unsigned int) _temp2071 > 4u?*(( int*) _temp2071) == Cyc_Absyn_AnonUnionType:
0){ _LL2190: _temp2189=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp2071)->f1;
goto _LL2096;} else{ goto _LL2097;} _LL2097: if(( unsigned int) _temp2071 > 4u?*((
int*) _temp2071) == Cyc_Absyn_Evar: 0){ _LL2198: _temp2197=(( struct Cyc_Absyn_Evar_struct*)
_temp2071)->f1; goto _LL2196; _LL2196: _temp2195=(( struct Cyc_Absyn_Evar_struct*)
_temp2071)->f2; goto _LL2194; _LL2194: _temp2193=(( struct Cyc_Absyn_Evar_struct*)
_temp2071)->f3; goto _LL2192; _LL2192: _temp2191=(( struct Cyc_Absyn_Evar_struct*)
_temp2071)->f4; goto _LL2098;} else{ goto _LL2099;} _LL2099: if(( unsigned int)
_temp2071 > 4u?*(( int*) _temp2071) == Cyc_Absyn_RgnHandleType: 0){ _LL2200:
_temp2199=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp2071)->f1;
goto _LL2100;} else{ goto _LL2101;} _LL2101: if(( unsigned int) _temp2071 > 4u?*((
int*) _temp2071) == Cyc_Absyn_EnumType: 0){ _LL2204: _temp2203=(( struct Cyc_Absyn_EnumType_struct*)
_temp2071)->f1; goto _LL2202; _LL2202: _temp2201=(( struct Cyc_Absyn_EnumType_struct*)
_temp2071)->f2; goto _LL2102;} else{ goto _LL2103;} _LL2103: if( _temp2071 ==(
void*) Cyc_Absyn_VoidType){ goto _LL2104;} else{ goto _LL2105;} _LL2105: if((
unsigned int) _temp2071 > 4u?*(( int*) _temp2071) == Cyc_Absyn_IntType: 0){
_LL2208: _temp2207=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2071)->f1;
goto _LL2206; _LL2206: _temp2205=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2071)->f2; goto _LL2106;} else{ goto _LL2107;} _LL2107: if( _temp2071 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2108;} else{ goto _LL2109;} _LL2109: if(
_temp2071 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2110;} else{ goto _LL2111;}
_LL2111: if( _temp2071 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL2112;} else{ goto
_LL2113;} _LL2113: if(( unsigned int) _temp2071 > 4u?*(( int*) _temp2071) == Cyc_Absyn_AccessEff:
0){ _LL2210: _temp2209=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp2071)->f1;
goto _LL2114;} else{ goto _LL2115;} _LL2115: if(( unsigned int) _temp2071 > 4u?*((
int*) _temp2071) == Cyc_Absyn_JoinEff: 0){ _LL2212: _temp2211=(( struct Cyc_Absyn_JoinEff_struct*)
_temp2071)->f1; goto _LL2116;} else{ goto _LL2072;} _LL2074: { struct
_handler_cons _temp2213; _push_handler(& _temp2213);{ int _temp2215= 0; if(
setjmp( _temp2213.handler)){ _temp2215= 1;} if( ! _temp2215){{ void* _temp2216=((
void*(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp,
inst, _temp2117); _npop_handler( 0u); return _temp2216;}; _pop_handler();} else{
void* _temp2214=( void*) _exn_thrown; void* _temp2218= _temp2214; _LL2220: if(
_temp2218 == Cyc_Core_Not_found){ goto _LL2221;} else{ goto _LL2222;} _LL2222:
goto _LL2223; _LL2221: return t; _LL2223:( void) _throw( _temp2218); _LL2219:;}}}
_LL2076: { struct Cyc_List_List* _temp2224= Cyc_Tcutil_substs( rgn, inst,
_temp2121); goto _LL2225; _LL2225: return _temp2224 == _temp2121? t:( void*)({
struct Cyc_Absyn_StructType_struct* _temp2226=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp2226[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp2227; _temp2227.tag= Cyc_Absyn_StructType;
_temp2227.f1= _temp2123; _temp2227.f2= _temp2224; _temp2227.f3= _temp2119;
_temp2227;}); _temp2226;});} _LL2078: { struct Cyc_List_List* _temp2228= Cyc_Tcutil_substs(
rgn, inst, _temp2127); goto _LL2229; _LL2229: return _temp2228 == _temp2127? t:(
void*)({ struct Cyc_Absyn_UnionType_struct* _temp2230=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp2230[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp2231; _temp2231.tag= Cyc_Absyn_UnionType;
_temp2231.f1= _temp2129; _temp2231.f2= _temp2228; _temp2231.f3= _temp2125;
_temp2231;}); _temp2230;});} _LL2080: { struct Cyc_List_List* _temp2232= Cyc_Tcutil_substs(
rgn, inst, _temp2135); goto _LL2233; _LL2233: { void* _temp2234= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2133); goto _LL2235; _LL2235: return( _temp2232 == _temp2135?
_temp2234 == _temp2133: 0)? t:( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp2236=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp2236[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp2237; _temp2237.tag=
Cyc_Absyn_TunionType; _temp2237.f1=({ struct Cyc_Absyn_TunionInfo _temp2238;
_temp2238.tunion_info=( void*) _temp2137; _temp2238.targs= _temp2232; _temp2238.rgn=(
void*) _temp2234; _temp2238;}); _temp2237;}); _temp2236;});}} _LL2082: { struct
Cyc_List_List* _temp2239= Cyc_Tcutil_substs( rgn, inst, _temp2141); goto _LL2240;
_LL2240: return _temp2239 == _temp2141? t:( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp2241=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp2241[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp2242; _temp2242.tag= Cyc_Absyn_TunionFieldType; _temp2242.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2243; _temp2243.field_info=( void*) _temp2143; _temp2243.targs= _temp2239;
_temp2243;}); _temp2242;}); _temp2241;});} _LL2084: { struct Cyc_List_List*
_temp2244= Cyc_Tcutil_substs( rgn, inst, _temp2147); goto _LL2245; _LL2245:
return _temp2244 == _temp2147? t:( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp2246=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2246[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp2247; _temp2247.tag=
Cyc_Absyn_TypedefType; _temp2247.f1= _temp2149; _temp2247.f2= _temp2244;
_temp2247.f3= _temp2145; _temp2247;}); _temp2246;});} _LL2086: { void* _temp2248=
Cyc_Tcutil_rsubstitute( rgn, inst, _temp2155); goto _LL2249; _LL2249: return
_temp2248 == _temp2155? t:( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp2250=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp2250[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp2251; _temp2251.tag= Cyc_Absyn_ArrayType;
_temp2251.f1=( void*) _temp2248; _temp2251.f2= _temp2153; _temp2251.f3=
_temp2151; _temp2251;}); _temp2250;});} _LL2088: { void* _temp2252= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2167); goto _LL2253; _LL2253: { void* _temp2254= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2165); goto _LL2255; _LL2255: if( _temp2252 == _temp2167?
_temp2254 == _temp2165: 0){ return t;} return( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp2256=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp2256[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp2257; _temp2257.tag=
Cyc_Absyn_PointerType; _temp2257.f1=({ struct Cyc_Absyn_PtrInfo _temp2258;
_temp2258.elt_typ=( void*) _temp2252; _temp2258.rgn_typ=( void*) _temp2254;
_temp2258.nullable= _temp2163; _temp2258.tq= _temp2161; _temp2258.bounds=
_temp2159; _temp2258;}); _temp2257;}); _temp2256;});}} _LL2090:{ struct Cyc_List_List*
_temp2259= _temp2183; goto _LL2260; _LL2260: for( 0; _temp2259 != 0; _temp2259=((
struct Cyc_List_List*) _check_null( _temp2259))->tl){ inst=({ struct Cyc_List_List*
_temp2261=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2261->hd=( void*)({ struct _tuple5* _temp2262=( struct _tuple5*)
_region_malloc( rgn, sizeof( struct _tuple5)); _temp2262->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp2259))->hd; _temp2262->f2=( void*)({
struct Cyc_Absyn_VarType_struct* _temp2263=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2263[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2264; _temp2264.tag= Cyc_Absyn_VarType; _temp2264.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp2259))->hd;
_temp2264;}); _temp2263;}); _temp2262;}); _temp2261->tl= inst; _temp2261;});}}{
struct Cyc_List_List* _temp2267; struct Cyc_List_List* _temp2269; struct _tuple0
_temp2265=(( struct _tuple0(*)( struct _RegionHandle* r3, struct _RegionHandle*
r4, struct Cyc_List_List* x)) Cyc_List_rsplit)( rgn, rgn,(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple16*(* f)( struct _RegionHandle*, struct
_tuple2*), struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)(
rgn, Cyc_Tcutil_substitute_f1, rgn, _temp2177)); _LL2270: _temp2269= _temp2265.f1;
goto _LL2268; _LL2268: _temp2267= _temp2265.f2; goto _LL2266; _LL2266: { struct
Cyc_List_List* _temp2271= Cyc_Tcutil_substs( rgn, inst, _temp2267); goto _LL2272;
_LL2272: { struct Cyc_List_List* _temp2273=(( struct Cyc_List_List*(*)( struct
_tuple2*(* f)( struct _tuple16*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct _RegionHandle* r4,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn,
_temp2269, _temp2271)); goto _LL2274; _LL2274: { struct Cyc_Core_Opt* eff2; if(
_temp2181 == 0){ eff2= 0;} else{ void* _temp2275= Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2181))->v); goto _LL2276;
_LL2276: if( _temp2275 ==( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2181))->v){
eff2= _temp2181;} else{ eff2=({ struct Cyc_Core_Opt* _temp2277=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2277->v=( void*) _temp2275;
_temp2277;});}}{ struct Cyc_Absyn_VarargInfo* cyc_varargs2; if( _temp2173 == 0){
cyc_varargs2= 0;} else{ int _temp2280; void* _temp2282; void* _temp2284; struct
Cyc_Absyn_Tqual _temp2286; struct Cyc_Core_Opt* _temp2288; struct Cyc_Absyn_VarargInfo
_temp2278=*(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp2173)); _LL2289:
_temp2288= _temp2278.name; goto _LL2287; _LL2287: _temp2286= _temp2278.tq; goto
_LL2285; _LL2285: _temp2284=( void*) _temp2278.type; goto _LL2283; _LL2283:
_temp2282=( void*) _temp2278.rgn; goto _LL2281; _LL2281: _temp2280= _temp2278.inject;
goto _LL2279; _LL2279: { void* _temp2290= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2284); goto _LL2291; _LL2291: { void* _temp2292= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2282); goto _LL2293; _LL2293: if( _temp2290 == _temp2284?
_temp2282 == _temp2292: 0){ cyc_varargs2= _temp2173;} else{ cyc_varargs2=({
struct Cyc_Absyn_VarargInfo* _temp2294=( struct Cyc_Absyn_VarargInfo*) GC_malloc(
sizeof( struct Cyc_Absyn_VarargInfo)); _temp2294->name= _temp2288; _temp2294->tq=
_temp2286; _temp2294->type=( void*) _temp2290; _temp2294->rgn=( void*) _temp2292;
_temp2294->inject= _temp2280; _temp2294;});}}}} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2295=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2295[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2296; _temp2296.tag= Cyc_Absyn_FnType;
_temp2296.f1=({ struct Cyc_Absyn_FnInfo _temp2297; _temp2297.tvars= _temp2183;
_temp2297.effect= eff2; _temp2297.ret_typ=( void*) Cyc_Tcutil_rsubstitute( rgn,
inst, _temp2179); _temp2297.args= _temp2273; _temp2297.c_varargs= _temp2175;
_temp2297.cyc_varargs= cyc_varargs2; _temp2297.attributes= _temp2171; _temp2297;});
_temp2296;}); _temp2295;});}}}}} _LL2092: { struct Cyc_List_List* _temp2298=((
struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_snd_tqt, _temp2185);
goto _LL2299; _LL2299: { struct Cyc_List_List* _temp2300= Cyc_Tcutil_substs( rgn,
inst, _temp2298); goto _LL2301; _LL2301: if( _temp2300 == _temp2298){ return t;}{
struct Cyc_List_List* _temp2302=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)(
struct _tuple4*, void*), struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_map2)(
Cyc_Tcutil_map2_tq, _temp2185, _temp2300); goto _LL2303; _LL2303: return( void*)({
struct Cyc_Absyn_TupleType_struct* _temp2304=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp2304[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp2305; _temp2305.tag= Cyc_Absyn_TupleType;
_temp2305.f1= _temp2302; _temp2305;}); _temp2304;});}}} _LL2094: { struct Cyc_List_List*
_temp2306=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2187); goto _LL2307; _LL2307: { struct Cyc_List_List* _temp2308= Cyc_Tcutil_substs(
rgn, inst, _temp2306); goto _LL2309; _LL2309: if( _temp2308 == _temp2306){
return t;}{ struct Cyc_List_List* _temp2310=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type,
_temp2187, _temp2308); goto _LL2311; _LL2311: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp2312=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp2312[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp2313; _temp2313.tag= Cyc_Absyn_AnonStructType; _temp2313.f1= _temp2310;
_temp2313;}); _temp2312;});}}} _LL2096: { struct Cyc_List_List* _temp2314=((
struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type, _temp2189);
goto _LL2315; _LL2315: { struct Cyc_List_List* _temp2316= Cyc_Tcutil_substs( rgn,
inst, _temp2314); goto _LL2317; _LL2317: if( _temp2316 == _temp2314){ return t;}{
struct Cyc_List_List* _temp2318=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2189, _temp2316); goto
_LL2319; _LL2319: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp2320=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp2320[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp2321; _temp2321.tag= Cyc_Absyn_AnonStructType; _temp2321.f1= _temp2318;
_temp2321;}); _temp2320;});}}} _LL2098: if( _temp2195 != 0){ return Cyc_Tcutil_rsubstitute(
rgn, inst,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2195))->v);} else{
return t;} _LL2100: { void* _temp2322= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2199); goto _LL2323; _LL2323: return _temp2322 == _temp2199? t:( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp2324=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp2324[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp2325; _temp2325.tag= Cyc_Absyn_RgnHandleType;
_temp2325.f1=( void*) _temp2322; _temp2325;}); _temp2324;});} _LL2102: return t;
_LL2104: return t; _LL2106: return t; _LL2108: return t; _LL2110: return t;
_LL2112: return t; _LL2114: { void* _temp2326= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2209); goto _LL2327; _LL2327: return _temp2326 == _temp2209? t:( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2328=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2328[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2329; _temp2329.tag= Cyc_Absyn_AccessEff;
_temp2329.f1=( void*) _temp2326; _temp2329;}); _temp2328;});} _LL2116: { struct
Cyc_List_List* _temp2330= Cyc_Tcutil_substs( rgn, inst, _temp2211); goto _LL2331;
_LL2331: return _temp2330 == _temp2211? t:( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp2332=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2332[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2333; _temp2333.tag= Cyc_Absyn_JoinEff;
_temp2333.f1= _temp2330; _temp2333;}); _temp2332;});} _LL2072:;} static struct
Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List*
inst, struct Cyc_List_List* ts){ if( ts == 0){ return 0;}{ void* _temp2334=(
void*)(( struct Cyc_List_List*) _check_null( ts))->hd; goto _LL2335; _LL2335: {
struct Cyc_List_List* _temp2336=(( struct Cyc_List_List*) _check_null( ts))->tl;
goto _LL2337; _LL2337: { void* _temp2338= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2334); goto _LL2339; _LL2339: { struct Cyc_List_List* _temp2340= Cyc_Tcutil_substs(
rgn, inst, _temp2336); goto _LL2341; _LL2341: if( _temp2334 == _temp2338?
_temp2336 == _temp2340: 0){ return ts;} return( struct Cyc_List_List*)({ struct
Cyc_List_List* _temp2342=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2342->hd=( void*) _temp2338; _temp2342->tl= _temp2340; _temp2342;});}}}}}
extern void* Cyc_Tcutil_substitute( struct Cyc_List_List* inst, void* t){ return
Cyc_Tcutil_rsubstitute( Cyc_Core_heap_region, inst, t);} struct Cyc_Absyn_Exp*
Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv* te, void* t, struct Cyc_Position_Segment*
loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2392=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2392[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2393; _temp2393.tag= Cyc_Absyn_Const_e;
_temp2393.f1=( void*)(( void*) Cyc_Absyn_Null_c); _temp2393;}); _temp2392;}),
loc);{ void* _temp2343= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp2357; void* _temp2359; void* _temp2361; void* _temp2363; void* _temp2365;
_LL2345: if(( unsigned int) _temp2343 > 4u?*(( int*) _temp2343) == Cyc_Absyn_PointerType:
0){ _LL2358: _temp2357=(( struct Cyc_Absyn_PointerType_struct*) _temp2343)->f1;
goto _LL2346;} else{ goto _LL2347;} _LL2347: if(( unsigned int) _temp2343 > 4u?*((
int*) _temp2343) == Cyc_Absyn_IntType: 0){ _LL2362: _temp2361=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2343)->f1; goto _LL2360; _LL2360: _temp2359=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2343)->f2; if( _temp2359 ==(
void*) Cyc_Absyn_B1){ goto _LL2348;} else{ goto _LL2349;}} else{ goto _LL2349;}
_LL2349: if(( unsigned int) _temp2343 > 4u?*(( int*) _temp2343) == Cyc_Absyn_IntType:
0){ _LL2366: _temp2365=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2343)->f1;
goto _LL2364; _LL2364: _temp2363=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2343)->f2; goto _LL2350;} else{ goto _LL2351;} _LL2351: if( _temp2343 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2352;} else{ goto _LL2353;} _LL2353: if(
_temp2343 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2354;} else{ goto _LL2355;}
_LL2355: goto _LL2356; _LL2346: goto _LL2344; _LL2348:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2367=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2367[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2368; _temp2368.tag= Cyc_Absyn_Const_e; _temp2368.f1=(
void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp2369=( struct Cyc_Absyn_Char_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct)); _temp2369[ 0]=({ struct Cyc_Absyn_Char_c_struct
_temp2370; _temp2370.tag= Cyc_Absyn_Char_c; _temp2370.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp2370.f2='\000'; _temp2370;}); _temp2369;})); _temp2368;}); _temp2367;})));
goto _LL2344; _LL2350:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2371=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2371[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2372; _temp2372.tag= Cyc_Absyn_Const_e;
_temp2372.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp2373=(
struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp2373[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp2374; _temp2374.tag= Cyc_Absyn_Int_c;
_temp2374.f1=( void*) _temp2365; _temp2374.f2= 0; _temp2374;}); _temp2373;}));
_temp2372;}); _temp2371;}))); if( _temp2363 !=( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2375=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2375[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2376; _temp2376.tag= Cyc_Absyn_Cast_e; _temp2376.f1=( void*) t; _temp2376.f2=
e; _temp2376;}); _temp2375;}), loc);} goto _LL2344; _LL2352:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2377=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2377[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2378; _temp2378.tag= Cyc_Absyn_Const_e; _temp2378.f1=(
void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2379=( struct Cyc_Absyn_Float_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct)); _temp2379[ 0]=({ struct
Cyc_Absyn_Float_c_struct _temp2380; _temp2380.tag= Cyc_Absyn_Float_c; _temp2380.f1=({
unsigned char* _temp2381="0.0"; struct _tagged_string _temp2382; _temp2382.curr=
_temp2381; _temp2382.base= _temp2381; _temp2382.last_plus_one= _temp2381 + 4;
_temp2382;}); _temp2380;}); _temp2379;})); _temp2378;}); _temp2377;}))); goto
_LL2344; _LL2354:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp2383=( struct Cyc_Absyn_Cast_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp2383[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp2384; _temp2384.tag= Cyc_Absyn_Cast_e;
_temp2384.f1=( void*) t; _temp2384.f2= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2385=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2385[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2386; _temp2386.tag= Cyc_Absyn_Const_e;
_temp2386.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2387=(
struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2387[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2388; _temp2388.tag= Cyc_Absyn_Float_c;
_temp2388.f1=({ unsigned char* _temp2389="0.0"; struct _tagged_string _temp2390;
_temp2390.curr= _temp2389; _temp2390.base= _temp2389; _temp2390.last_plus_one=
_temp2389 + 4; _temp2390;}); _temp2388;}); _temp2387;})); _temp2386;});
_temp2385;}), loc); _temp2384;}); _temp2383;}))); goto _LL2344; _LL2356: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2391= Cyc_Absynpp_typ2string( t); xprintf("declaration of type %.*s requires initializer",
_temp2391.last_plus_one - _temp2391.curr, _temp2391.curr);})); goto _LL2344;
_LL2344:;} return e;} struct _tuple5* Cyc_Tcutil_make_inst_var( struct Cyc_List_List*
s, struct Cyc_Absyn_Tvar* tv){ void* k=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( tv->kind); return({ struct _tuple5* _temp2394=( struct
_tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp2394->f1= tv; _temp2394->f2=
Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2395=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2395->v=( void*) k; _temp2395;}),({
struct Cyc_Core_Opt* _temp2396=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2396->v=( void*) s; _temp2396;})); _temp2394;});} struct
_tuple5* Cyc_Tcutil_r_make_inst_var( struct _tuple6* env, struct Cyc_Absyn_Tvar*
tv){ struct _tuple6 _temp2399; struct _RegionHandle* _temp2400; struct Cyc_List_List*
_temp2402; struct _tuple6* _temp2397= env; _temp2399=* _temp2397; _LL2403:
_temp2402= _temp2399.f1; goto _LL2401; _LL2401: _temp2400= _temp2399.f2; goto
_LL2398; _LL2398: { void* k=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
tv->kind); return({ struct _tuple5* _temp2404=( struct _tuple5*) _region_malloc(
_temp2400, sizeof( struct _tuple5)); _temp2404->f1= tv; _temp2404->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp2405=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2405->v=( void*) k; _temp2405;}),({ struct Cyc_Core_Opt*
_temp2406=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2406->v=( void*) _temp2402; _temp2406;})); _temp2404;});}} static struct
Cyc_List_List* Cyc_Tcutil_add_free_tvar( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 != 0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
if( Cyc_String_zstrptrcmp((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->name, tv->name) == 0){ struct Cyc_Absyn_Conref* k1=((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->kind;
struct Cyc_Absyn_Conref* k2= tv->kind; if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2407=* tv->name; struct
_tagged_string _temp2408= Cyc_Absynpp_ckind2string( k1); struct _tagged_string
_temp2409= Cyc_Absynpp_ckind2string( k2); xprintf("type variable %.*s is used with inconsistent kinds %.*s and %.*s",
_temp2407.last_plus_one - _temp2407.curr, _temp2407.curr, _temp2408.last_plus_one
- _temp2408.curr, _temp2408.curr, _temp2409.last_plus_one - _temp2409.curr,
_temp2409.curr);}));} if( tv->identity == 0){ tv->identity=(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->identity;} else{ if(*(( int*)
_check_null( tv->identity)) !=*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->identity))){(( void(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)( xprintf("same type variable has different identity!"));}}
return tvs;}}} tv->identity= Cyc_Tcutil_new_tvar_id(); return({ struct Cyc_List_List*
_temp2410=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2410->hd=( void*) tv; _temp2410->tl= tvs; _temp2410;});} static struct Cyc_List_List*
Cyc_Tcutil_fast_add_free_tvar( struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar*
tv){ if( tv->identity == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
unsigned char* _temp2411="fast_add_free_tvar: bad identity in tv"; struct
_tagged_string _temp2412; _temp2412.curr= _temp2411; _temp2412.base= _temp2411;
_temp2412.last_plus_one= _temp2411 + 39; _temp2412;}));}{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 != 0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
if((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->identity
== 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char*
_temp2413="fast_add_free_tvar: bad identity in tvs2"; struct _tagged_string
_temp2414; _temp2414.curr= _temp2413; _temp2414.base= _temp2413; _temp2414.last_plus_one=
_temp2413 + 41; _temp2414;}));} if(*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->identity)) ==*(( int*)
_check_null( tv->identity))){ return tvs;}}} return({ struct Cyc_List_List*
_temp2415=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2415->hd=( void*) tv; _temp2415->tl= tvs; _temp2415;});} static struct Cyc_List_List*
Cyc_Tcutil_add_bound_tvar( struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){
if( tv->identity == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
struct _tagged_string _temp2416= Cyc_Tcutil_tvar2string( tv); xprintf("bound tvar id for %.*s is null",
_temp2416.last_plus_one - _temp2416.curr, _temp2416.curr);}));} return({ struct
Cyc_List_List* _temp2417=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2417->hd=( void*) tv; _temp2417->tl= tvs; _temp2417;});} static struct Cyc_List_List*
Cyc_Tcutil_add_free_evar( struct Cyc_List_List* es, void* e){ void* _temp2418=
Cyc_Tcutil_compress( e); struct Cyc_Core_Opt* _temp2424; int _temp2426; struct
Cyc_Core_Opt* _temp2428; struct Cyc_Core_Opt* _temp2430; _LL2420: if((
unsigned int) _temp2418 > 4u?*(( int*) _temp2418) == Cyc_Absyn_Evar: 0){ _LL2431:
_temp2430=(( struct Cyc_Absyn_Evar_struct*) _temp2418)->f1; goto _LL2429;
_LL2429: _temp2428=(( struct Cyc_Absyn_Evar_struct*) _temp2418)->f2; goto
_LL2427; _LL2427: _temp2426=(( struct Cyc_Absyn_Evar_struct*) _temp2418)->f3;
goto _LL2425; _LL2425: _temp2424=(( struct Cyc_Absyn_Evar_struct*) _temp2418)->f4;
goto _LL2421;} else{ goto _LL2422;} _LL2422: goto _LL2423; _LL2421:{ struct Cyc_List_List*
es2= es; for( 0; es2 != 0; es2=(( struct Cyc_List_List*) _check_null( es2))->tl){
void* _temp2432= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*)
_check_null( es2))->hd); struct Cyc_Core_Opt* _temp2438; int _temp2440; struct
Cyc_Core_Opt* _temp2442; struct Cyc_Core_Opt* _temp2444; _LL2434: if((
unsigned int) _temp2432 > 4u?*(( int*) _temp2432) == Cyc_Absyn_Evar: 0){ _LL2445:
_temp2444=(( struct Cyc_Absyn_Evar_struct*) _temp2432)->f1; goto _LL2443;
_LL2443: _temp2442=(( struct Cyc_Absyn_Evar_struct*) _temp2432)->f2; goto
_LL2441; _LL2441: _temp2440=(( struct Cyc_Absyn_Evar_struct*) _temp2432)->f3;
goto _LL2439; _LL2439: _temp2438=(( struct Cyc_Absyn_Evar_struct*) _temp2432)->f4;
goto _LL2435;} else{ goto _LL2436;} _LL2436: goto _LL2437; _LL2435: if(
_temp2426 == _temp2440){ return es;} goto _LL2433; _LL2437: goto _LL2433;
_LL2433:;}} return({ struct Cyc_List_List* _temp2446=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2446->hd=( void*) e; _temp2446->tl=
es; _temp2446;}); _LL2423: return es; _LL2419:;} static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars( struct Cyc_List_List* tvs, struct Cyc_List_List*
btvs){ struct Cyc_List_List* r= 0; for( 0; tvs != 0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){ int present= 0;{ struct Cyc_List_List* b= btvs; for( 0;
b != 0; b=(( struct Cyc_List_List*) _check_null( b))->tl){ if(*(( int*)
_check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->identity))
==*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( b))->hd)->identity))){ present= 1; break;}}} if( ! present){ r=({
struct Cyc_List_List* _temp2447=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2447->hd=( void*)(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd); _temp2447->tl= r; _temp2447;});}}
r=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( r);
return r;} struct Cyc_Tcutil_CVTEnv{ struct Cyc_List_List* kind_env; struct Cyc_List_List*
free_vars; struct Cyc_List_List* free_evars; } ; static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcutil_CVTEnv cvtenv, void* expected_kind, void* t){{ void*
_temp2448= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt* _temp2494; int
_temp2496; struct Cyc_Core_Opt* _temp2498; struct Cyc_Core_Opt* _temp2500;
struct Cyc_Core_Opt** _temp2502; struct Cyc_Absyn_Tvar* _temp2503; struct Cyc_Absyn_Enumdecl*
_temp2505; struct Cyc_Absyn_Enumdecl** _temp2507; struct _tuple1* _temp2508;
struct Cyc_Absyn_TunionInfo _temp2510; void* _temp2512; struct Cyc_List_List*
_temp2514; void* _temp2516; void** _temp2518; struct Cyc_Absyn_TunionFieldInfo
_temp2519; struct Cyc_List_List* _temp2521; void* _temp2523; void** _temp2525;
struct Cyc_Absyn_PtrInfo _temp2526; struct Cyc_Absyn_Conref* _temp2528; struct
Cyc_Absyn_Tqual _temp2530; struct Cyc_Absyn_Conref* _temp2532; void* _temp2534;
void* _temp2536; void* _temp2538; void* _temp2540; struct Cyc_Absyn_Exp*
_temp2542; struct Cyc_Absyn_Tqual _temp2544; void* _temp2546; struct Cyc_Absyn_FnInfo
_temp2548; struct Cyc_List_List* _temp2550; struct Cyc_Absyn_VarargInfo*
_temp2552; int _temp2554; struct Cyc_List_List* _temp2556; void* _temp2558;
struct Cyc_Core_Opt* _temp2560; struct Cyc_Core_Opt** _temp2562; struct Cyc_List_List*
_temp2563; struct Cyc_List_List** _temp2565; struct Cyc_List_List* _temp2566;
struct Cyc_List_List* _temp2568; struct Cyc_List_List* _temp2570; struct Cyc_Absyn_Structdecl**
_temp2572; struct Cyc_Absyn_Structdecl*** _temp2574; struct Cyc_List_List*
_temp2575; struct _tuple1* _temp2577; struct Cyc_Absyn_Uniondecl** _temp2579;
struct Cyc_Absyn_Uniondecl*** _temp2581; struct Cyc_List_List* _temp2582; struct
_tuple1* _temp2584; struct Cyc_Core_Opt* _temp2586; struct Cyc_Core_Opt**
_temp2588; struct Cyc_List_List* _temp2589; struct _tuple1* _temp2591; void*
_temp2593; void* _temp2595; struct Cyc_List_List* _temp2597; _LL2450: if(
_temp2448 ==( void*) Cyc_Absyn_VoidType){ goto _LL2451;} else{ goto _LL2452;}
_LL2452: if(( unsigned int) _temp2448 > 4u?*(( int*) _temp2448) == Cyc_Absyn_Evar:
0){ _LL2501: _temp2500=(( struct Cyc_Absyn_Evar_struct*) _temp2448)->f1;
_temp2502=&(( struct Cyc_Absyn_Evar_struct*) _temp2448)->f1; goto _LL2499;
_LL2499: _temp2498=(( struct Cyc_Absyn_Evar_struct*) _temp2448)->f2; goto
_LL2497; _LL2497: _temp2496=(( struct Cyc_Absyn_Evar_struct*) _temp2448)->f3;
goto _LL2495; _LL2495: _temp2494=(( struct Cyc_Absyn_Evar_struct*) _temp2448)->f4;
goto _LL2453;} else{ goto _LL2454;} _LL2454: if(( unsigned int) _temp2448 > 4u?*((
int*) _temp2448) == Cyc_Absyn_VarType: 0){ _LL2504: _temp2503=(( struct Cyc_Absyn_VarType_struct*)
_temp2448)->f1; goto _LL2455;} else{ goto _LL2456;} _LL2456: if(( unsigned int)
_temp2448 > 4u?*(( int*) _temp2448) == Cyc_Absyn_EnumType: 0){ _LL2509:
_temp2508=(( struct Cyc_Absyn_EnumType_struct*) _temp2448)->f1; goto _LL2506;
_LL2506: _temp2505=(( struct Cyc_Absyn_EnumType_struct*) _temp2448)->f2;
_temp2507=&(( struct Cyc_Absyn_EnumType_struct*) _temp2448)->f2; goto _LL2457;}
else{ goto _LL2458;} _LL2458: if(( unsigned int) _temp2448 > 4u?*(( int*)
_temp2448) == Cyc_Absyn_TunionType: 0){ _LL2511: _temp2510=(( struct Cyc_Absyn_TunionType_struct*)
_temp2448)->f1; _LL2517: _temp2516=( void*) _temp2510.tunion_info; _temp2518=&(((
struct Cyc_Absyn_TunionType_struct*) _temp2448)->f1).tunion_info; goto _LL2515;
_LL2515: _temp2514= _temp2510.targs; goto _LL2513; _LL2513: _temp2512=( void*)
_temp2510.rgn; goto _LL2459;} else{ goto _LL2460;} _LL2460: if(( unsigned int)
_temp2448 > 4u?*(( int*) _temp2448) == Cyc_Absyn_TunionFieldType: 0){ _LL2520:
_temp2519=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2448)->f1; _LL2524:
_temp2523=( void*) _temp2519.field_info; _temp2525=&((( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2448)->f1).field_info; goto _LL2522; _LL2522: _temp2521= _temp2519.targs;
goto _LL2461;} else{ goto _LL2462;} _LL2462: if(( unsigned int) _temp2448 > 4u?*((
int*) _temp2448) == Cyc_Absyn_PointerType: 0){ _LL2527: _temp2526=(( struct Cyc_Absyn_PointerType_struct*)
_temp2448)->f1; _LL2537: _temp2536=( void*) _temp2526.elt_typ; goto _LL2535;
_LL2535: _temp2534=( void*) _temp2526.rgn_typ; goto _LL2533; _LL2533: _temp2532=
_temp2526.nullable; goto _LL2531; _LL2531: _temp2530= _temp2526.tq; goto _LL2529;
_LL2529: _temp2528= _temp2526.bounds; goto _LL2463;} else{ goto _LL2464;}
_LL2464: if(( unsigned int) _temp2448 > 4u?*(( int*) _temp2448) == Cyc_Absyn_IntType:
0){ _LL2541: _temp2540=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2448)->f1;
goto _LL2539; _LL2539: _temp2538=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2448)->f2; goto _LL2465;} else{ goto _LL2466;} _LL2466: if( _temp2448 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2467;} else{ goto _LL2468;} _LL2468: if(
_temp2448 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2469;} else{ goto _LL2470;}
_LL2470: if(( unsigned int) _temp2448 > 4u?*(( int*) _temp2448) == Cyc_Absyn_ArrayType:
0){ _LL2547: _temp2546=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2448)->f1;
goto _LL2545; _LL2545: _temp2544=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2448)->f2; goto _LL2543; _LL2543: _temp2542=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2448)->f3; goto _LL2471;} else{ goto _LL2472;} _LL2472: if(( unsigned int)
_temp2448 > 4u?*(( int*) _temp2448) == Cyc_Absyn_FnType: 0){ _LL2549: _temp2548=((
struct Cyc_Absyn_FnType_struct*) _temp2448)->f1; _LL2564: _temp2563= _temp2548.tvars;
_temp2565=&((( struct Cyc_Absyn_FnType_struct*) _temp2448)->f1).tvars; goto
_LL2561; _LL2561: _temp2560= _temp2548.effect; _temp2562=&((( struct Cyc_Absyn_FnType_struct*)
_temp2448)->f1).effect; goto _LL2559; _LL2559: _temp2558=( void*) _temp2548.ret_typ;
goto _LL2557; _LL2557: _temp2556= _temp2548.args; goto _LL2555; _LL2555:
_temp2554= _temp2548.c_varargs; goto _LL2553; _LL2553: _temp2552= _temp2548.cyc_varargs;
goto _LL2551; _LL2551: _temp2550= _temp2548.attributes; goto _LL2473;} else{
goto _LL2474;} _LL2474: if(( unsigned int) _temp2448 > 4u?*(( int*) _temp2448)
== Cyc_Absyn_TupleType: 0){ _LL2567: _temp2566=(( struct Cyc_Absyn_TupleType_struct*)
_temp2448)->f1; goto _LL2475;} else{ goto _LL2476;} _LL2476: if(( unsigned int)
_temp2448 > 4u?*(( int*) _temp2448) == Cyc_Absyn_AnonStructType: 0){ _LL2569:
_temp2568=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2448)->f1; goto
_LL2477;} else{ goto _LL2478;} _LL2478: if(( unsigned int) _temp2448 > 4u?*((
int*) _temp2448) == Cyc_Absyn_AnonUnionType: 0){ _LL2571: _temp2570=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp2448)->f1; goto _LL2479;} else{ goto
_LL2480;} _LL2480: if(( unsigned int) _temp2448 > 4u?*(( int*) _temp2448) == Cyc_Absyn_StructType:
0){ _LL2578: _temp2577=(( struct Cyc_Absyn_StructType_struct*) _temp2448)->f1;
goto _LL2576; _LL2576: _temp2575=(( struct Cyc_Absyn_StructType_struct*)
_temp2448)->f2; goto _LL2573; _LL2573: _temp2572=(( struct Cyc_Absyn_StructType_struct*)
_temp2448)->f3; _temp2574=&(( struct Cyc_Absyn_StructType_struct*) _temp2448)->f3;
goto _LL2481;} else{ goto _LL2482;} _LL2482: if(( unsigned int) _temp2448 > 4u?*((
int*) _temp2448) == Cyc_Absyn_UnionType: 0){ _LL2585: _temp2584=(( struct Cyc_Absyn_UnionType_struct*)
_temp2448)->f1; goto _LL2583; _LL2583: _temp2582=(( struct Cyc_Absyn_UnionType_struct*)
_temp2448)->f2; goto _LL2580; _LL2580: _temp2579=(( struct Cyc_Absyn_UnionType_struct*)
_temp2448)->f3; _temp2581=&(( struct Cyc_Absyn_UnionType_struct*) _temp2448)->f3;
goto _LL2483;} else{ goto _LL2484;} _LL2484: if(( unsigned int) _temp2448 > 4u?*((
int*) _temp2448) == Cyc_Absyn_TypedefType: 0){ _LL2592: _temp2591=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2448)->f1; goto _LL2590; _LL2590: _temp2589=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2448)->f2; goto _LL2587; _LL2587: _temp2586=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2448)->f3; _temp2588=&(( struct Cyc_Absyn_TypedefType_struct*) _temp2448)->f3;
goto _LL2485;} else{ goto _LL2486;} _LL2486: if( _temp2448 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL2487;} else{ goto _LL2488;} _LL2488: if(( unsigned int) _temp2448 > 4u?*((
int*) _temp2448) == Cyc_Absyn_RgnHandleType: 0){ _LL2594: _temp2593=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp2448)->f1; goto _LL2489;} else{
goto _LL2490;} _LL2490: if(( unsigned int) _temp2448 > 4u?*(( int*) _temp2448)
== Cyc_Absyn_AccessEff: 0){ _LL2596: _temp2595=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp2448)->f1; goto _LL2491;} else{ goto _LL2492;} _LL2492: if(( unsigned int)
_temp2448 > 4u?*(( int*) _temp2448) == Cyc_Absyn_JoinEff: 0){ _LL2598: _temp2597=((
struct Cyc_Absyn_JoinEff_struct*) _temp2448)->f1; goto _LL2493;} else{ goto
_LL2449;} _LL2451: goto _LL2449; _LL2453: if(* _temp2502 == 0){* _temp2502=({
struct Cyc_Core_Opt* _temp2599=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2599->v=( void*) expected_kind; _temp2599;});} cvtenv.free_evars=
Cyc_Tcutil_add_free_evar( cvtenv.free_evars, t); goto _LL2449; _LL2455: { struct
Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp2503->kind); if(( void*) c->v ==( void*) Cyc_Absyn_No_constr){(
void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2600=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2600[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2601; _temp2601.tag= Cyc_Absyn_Eq_constr;
_temp2601.f1=( void*) expected_kind; _temp2601;}); _temp2600;})));} cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar( loc, cvtenv.kind_env, _temp2503); cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars, _temp2503); goto _LL2449;}
_LL2457: if(* _temp2507 == 0){ struct _handler_cons _temp2602; _push_handler(&
_temp2602);{ int _temp2604= 0; if( setjmp( _temp2602.handler)){ _temp2604= 1;}
if( ! _temp2604){{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl( te,
loc, _temp2508);* _temp2507=( struct Cyc_Absyn_Enumdecl*)* ed;}; _pop_handler();}
else{ void* _temp2603=( void*) _exn_thrown; void* _temp2606= _temp2603; _LL2608:
if( _temp2606 == Cyc_Dict_Absent){ goto _LL2609;} else{ goto _LL2610;} _LL2610:
goto _LL2611; _LL2609: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2612=
Cyc_Absynpp_qvar2string( _temp2508); xprintf("unbound type enum %.*s", _temp2612.last_plus_one
- _temp2612.curr, _temp2612.curr);})); return cvtenv; _LL2611:( void) _throw(
_temp2606); _LL2607:;}}}{ struct Cyc_Absyn_Enumdecl* ed=( struct Cyc_Absyn_Enumdecl*)
_check_null(* _temp2507);* _temp2508=( ed->name)[ _check_known_subscript_notnull(
1u, 0)]; goto _LL2449;} _LL2459:{ void* _temp2613=* _temp2518; struct Cyc_Absyn_UnknownTunionInfo
_temp2619; int _temp2621; struct _tuple1* _temp2623; struct Cyc_Absyn_Tuniondecl*
_temp2625; _LL2615: if(*(( int*) _temp2613) == Cyc_Absyn_UnknownTunion){ _LL2620:
_temp2619=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp2613)->f1; _LL2624:
_temp2623= _temp2619.name; goto _LL2622; _LL2622: _temp2621= _temp2619.is_xtunion;
goto _LL2616;} else{ goto _LL2617;} _LL2617: if(*(( int*) _temp2613) == Cyc_Absyn_KnownTunion){
_LL2626: _temp2625=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2613)->f1; goto
_LL2618;} else{ goto _LL2614;} _LL2616: { struct Cyc_Absyn_Tuniondecl* tud;{
struct _handler_cons _temp2627; _push_handler(& _temp2627);{ int _temp2629= 0;
if( setjmp( _temp2627.handler)){ _temp2629= 1;} if( ! _temp2629){ tud=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp2623);; _pop_handler();} else{ void* _temp2628=( void*)
_exn_thrown; void* _temp2631= _temp2628; _LL2633: if( _temp2631 == Cyc_Dict_Absent){
goto _LL2634;} else{ goto _LL2635;} _LL2635: goto _LL2636; _LL2634: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2637= Cyc_Absynpp_qvar2string( _temp2623);
xprintf("unbound type [x]tunion %.*s", _temp2637.last_plus_one - _temp2637.curr,
_temp2637.curr);})); return cvtenv; _LL2636:( void) _throw( _temp2631); _LL2632:;}}}
if( tud->is_xtunion != _temp2621){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2638= Cyc_Absynpp_qvar2string( _temp2623); xprintf("[x]tunion is different from type declaration %.*s",
_temp2638.last_plus_one - _temp2638.curr, _temp2638.curr);}));}* _temp2518=(
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp2639=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp2639[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp2640; _temp2640.tag= Cyc_Absyn_KnownTunion;
_temp2640.f1= tud; _temp2640;}); _temp2639;}); _temp2625= tud; goto _LL2618;}
_LL2618: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2512);{ struct Cyc_List_List* tvs= _temp2625->tvs; for( 0; _temp2514 != 0?
tvs != 0: 0;( _temp2514=(( struct Cyc_List_List*) _check_null( _temp2514))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2514))->hd; void* k1=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} if( _temp2514 != 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2641= Cyc_Absynpp_qvar2string( _temp2625->name); xprintf("too many type arguments for tunion %.*s",
_temp2641.last_plus_one - _temp2641.curr, _temp2641.curr);}));} if( tvs != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2642= Cyc_Absynpp_qvar2string(
_temp2625->name); xprintf("too few type arguments for tunion %.*s", _temp2642.last_plus_one
- _temp2642.curr, _temp2642.curr);}));} goto _LL2614;} _LL2614:;} goto _LL2449;
_LL2461:{ void* _temp2643=* _temp2525; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp2649; int _temp2651; struct _tuple1* _temp2653; struct _tuple1* _temp2655;
struct Cyc_Absyn_Tunionfield* _temp2657; struct Cyc_Absyn_Tuniondecl* _temp2659;
_LL2645: if(*(( int*) _temp2643) == Cyc_Absyn_UnknownTunionfield){ _LL2650:
_temp2649=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp2643)->f1;
_LL2656: _temp2655= _temp2649.tunion_name; goto _LL2654; _LL2654: _temp2653=
_temp2649.field_name; goto _LL2652; _LL2652: _temp2651= _temp2649.is_xtunion;
goto _LL2646;} else{ goto _LL2647;} _LL2647: if(*(( int*) _temp2643) == Cyc_Absyn_KnownTunionfield){
_LL2660: _temp2659=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp2643)->f1;
goto _LL2658; _LL2658: _temp2657=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2643)->f2; goto _LL2648;} else{ goto _LL2644;} _LL2646: { struct Cyc_Absyn_Tuniondecl*
tud; struct Cyc_Absyn_Tunionfield* tuf;{ struct _handler_cons _temp2661;
_push_handler(& _temp2661);{ int _temp2663= 0; if( setjmp( _temp2661.handler)){
_temp2663= 1;} if( ! _temp2663){* Cyc_Tcenv_lookup_tuniondecl( te, loc,
_temp2655);; _pop_handler();} else{ void* _temp2662=( void*) _exn_thrown; void*
_temp2665= _temp2662; _LL2667: if( _temp2665 == Cyc_Dict_Absent){ goto _LL2668;}
else{ goto _LL2669;} _LL2669: goto _LL2670; _LL2668: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2671= Cyc_Absynpp_qvar2string( _temp2655); xprintf("unbound type tunion %.*s",
_temp2671.last_plus_one - _temp2671.curr, _temp2671.curr);})); return cvtenv;
_LL2670:( void) _throw( _temp2665); _LL2666:;}}}{ struct _handler_cons _temp2672;
_push_handler(& _temp2672);{ int _temp2674= 0; if( setjmp( _temp2672.handler)){
_temp2674= 1;} if( ! _temp2674){{ void* _temp2675= Cyc_Tcenv_lookup_ordinary( te,
loc, _temp2653); struct Cyc_Absyn_Tunionfield* _temp2681; struct Cyc_Absyn_Tuniondecl*
_temp2683; _LL2677: if(*(( int*) _temp2675) == Cyc_Tcenv_TunionRes){ _LL2684:
_temp2683=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2675)->f1; goto _LL2682;
_LL2682: _temp2681=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2675)->f2; goto
_LL2678;} else{ goto _LL2679;} _LL2679: goto _LL2680; _LL2678: tuf= _temp2681;
tud= _temp2683; if( tud->is_xtunion != _temp2651){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2685= Cyc_Absynpp_qvar2string( _temp2655); xprintf("[x]tunion is different from type declaration %.*s",
_temp2685.last_plus_one - _temp2685.curr, _temp2685.curr);}));} goto _LL2676;
_LL2680: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2686= Cyc_Absynpp_qvar2string(
_temp2653); struct _tagged_string _temp2687= Cyc_Absynpp_qvar2string( _temp2655);
xprintf("unbound field %.*s in type tunion %.*s", _temp2686.last_plus_one -
_temp2686.curr, _temp2686.curr, _temp2687.last_plus_one - _temp2687.curr,
_temp2687.curr);}));{ struct Cyc_Tcutil_CVTEnv _temp2688= cvtenv; _npop_handler(
0u); return _temp2688;} _LL2676:;}; _pop_handler();} else{ void* _temp2673=(
void*) _exn_thrown; void* _temp2690= _temp2673; _LL2692: if( _temp2690 == Cyc_Dict_Absent){
goto _LL2693;} else{ goto _LL2694;} _LL2694: goto _LL2695; _LL2693: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2696= Cyc_Absynpp_qvar2string( _temp2653);
struct _tagged_string _temp2697= Cyc_Absynpp_qvar2string( _temp2655); xprintf("unbound field %.*s in type tunion %.*s",
_temp2696.last_plus_one - _temp2696.curr, _temp2696.curr, _temp2697.last_plus_one
- _temp2697.curr, _temp2697.curr);})); return cvtenv; _LL2695:( void) _throw(
_temp2690); _LL2691:;}}}* _temp2525=( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp2698=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp2698[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp2699; _temp2699.tag= Cyc_Absyn_KnownTunionfield; _temp2699.f1= tud;
_temp2699.f2= tuf; _temp2699;}); _temp2698;}); _temp2659= tud; _temp2657= tuf;
goto _LL2648;} _LL2648: { struct Cyc_List_List* tvs= _temp2659->tvs; for( 0;
_temp2521 != 0? tvs != 0: 0;( _temp2521=(( struct Cyc_List_List*) _check_null(
_temp2521))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2521))->hd; void* k1=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} if( _temp2521 != 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2700= Cyc_Absynpp_qvar2string( _temp2659->name); struct
_tagged_string _temp2701= Cyc_Absynpp_qvar2string( _temp2657->name); xprintf("too many type arguments for tunion %.*s.%.*s",
_temp2700.last_plus_one - _temp2700.curr, _temp2700.curr, _temp2701.last_plus_one
- _temp2701.curr, _temp2701.curr);}));} if( tvs != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2702= Cyc_Absynpp_qvar2string( _temp2659->name);
struct _tagged_string _temp2703= Cyc_Absynpp_qvar2string( _temp2657->name);
xprintf("too few type arguments for tunion %.*s.%.*s", _temp2702.last_plus_one -
_temp2702.curr, _temp2702.curr, _temp2703.last_plus_one - _temp2703.curr,
_temp2703.curr);}));} goto _LL2644;} _LL2644:;} goto _LL2449; _LL2463: cvtenv=
Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_AnyKind,
_temp2536); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2534);{ void* _temp2704=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp2528))->v; void* _temp2710; struct Cyc_Absyn_Exp*
_temp2712; _LL2706: if(( unsigned int) _temp2704 > 1u?*(( int*) _temp2704) ==
Cyc_Absyn_Eq_constr: 0){ _LL2711: _temp2710=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2704)->f1; if(( unsigned int) _temp2710 > 1u?*(( int*) _temp2710) == Cyc_Absyn_Upper_b:
0){ _LL2713: _temp2712=(( struct Cyc_Absyn_Upper_b_struct*) _temp2710)->f1; goto
_LL2707;} else{ goto _LL2708;}} else{ goto _LL2708;} _LL2708: goto _LL2709;
_LL2707: if( ! Cyc_Tcutil_is_const_exp( te, _temp2712)){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp2714="pointer bounds expression is not a constant"; struct
_tagged_string _temp2715; _temp2715.curr= _temp2714; _temp2715.base= _temp2714;
_temp2715.last_plus_one= _temp2714 + 44; _temp2715;}));} Cyc_Tcexp_tcExp( te, 0,
_temp2712); if( ! Cyc_Tcutil_coerce_uint_typ( te, _temp2712)){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp2716="pointer bounds expression is not an unsigned int";
struct _tagged_string _temp2717; _temp2717.curr= _temp2716; _temp2717.base=
_temp2716; _temp2717.last_plus_one= _temp2716 + 49; _temp2717;}));} Cyc_Evexp_eval_const_uint_exp(
_temp2712); goto _LL2705; _LL2709: goto _LL2705; _LL2705:;} goto _LL2449;
_LL2465: goto _LL2449; _LL2467: goto _LL2449; _LL2469: goto _LL2449; _LL2471:
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,
_temp2546); if( _temp2542 == 0? 1: ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp2542))){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp2718="array bounds expression is not constant";
struct _tagged_string _temp2719; _temp2719.curr= _temp2718; _temp2719.base=
_temp2718; _temp2719.last_plus_one= _temp2718 + 40; _temp2719;}));} Cyc_Tcexp_tcExp(
te, 0,( struct Cyc_Absyn_Exp*) _check_null( _temp2542)); if( ! Cyc_Tcutil_coerce_uint_typ(
te,( struct Cyc_Absyn_Exp*) _check_null( _temp2542))){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp2720="array bounds expression is not an unsigned int";
struct _tagged_string _temp2721; _temp2721.curr= _temp2720; _temp2721.base=
_temp2720; _temp2721.last_plus_one= _temp2720 + 47; _temp2721;}));} Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2542)); goto _LL2449; _LL2473: { int
num_convs= 0; int seen_cdecl= 0; int seen_stdcall= 0; int seen_fastcall= 0; for(
0; _temp2550 != 0; _temp2550=(( struct Cyc_List_List*) _check_null( _temp2550))->tl){
if( ! Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*) _check_null(
_temp2550))->hd)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2722= Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp2550))->hd); xprintf("bad function type attribute %.*s",
_temp2722.last_plus_one - _temp2722.curr, _temp2722.curr);}));}{ void* _temp2723=(
void*)(( struct Cyc_List_List*) _check_null( _temp2550))->hd; _LL2725: if(
_temp2723 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL2726;} else{ goto _LL2727;}
_LL2727: if( _temp2723 ==( void*) Cyc_Absyn_Cdecl_att){ goto _LL2728;} else{
goto _LL2729;} _LL2729: if( _temp2723 ==( void*) Cyc_Absyn_Fastcall_att){ goto
_LL2730;} else{ goto _LL2731;} _LL2731: goto _LL2732; _LL2726: if( !
seen_stdcall){ seen_stdcall= 1; ++ num_convs;} goto _LL2724; _LL2728: if( !
seen_cdecl){ seen_cdecl= 1; ++ num_convs;} goto _LL2724; _LL2730: if( !
seen_fastcall){ seen_fastcall= 1; ++ num_convs;} goto _LL2724; _LL2732: goto
_LL2724; _LL2724:;}} if( num_convs > 1){ Cyc_Tcutil_terr( loc, xprintf("function can't have multiple calling conventions"));}
Cyc_Tcutil_check_unique_tvars( loc,* _temp2565);{ struct Cyc_List_List* b=*
_temp2565; for( 0; b != 0; b=(( struct Cyc_List_List*) _check_null( b))->tl){((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd)->identity=
Cyc_Tcutil_new_tvar_id(); cvtenv.kind_env= Cyc_Tcutil_add_bound_tvar( cvtenv.kind_env,(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd);{ void*
_temp2733=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( b))->hd)->kind))->v; void* _temp2739; _LL2735: if(( unsigned int)
_temp2733 > 1u?*(( int*) _temp2733) == Cyc_Absyn_Eq_constr: 0){ _LL2740:
_temp2739=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2733)->f1; if(
_temp2739 ==( void*) Cyc_Absyn_MemKind){ goto _LL2736;} else{ goto _LL2737;}}
else{ goto _LL2737;} _LL2737: goto _LL2738; _LL2736: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2741=*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( b))->hd)->name; xprintf("function attempts to abstract Mem type variable %.*s",
_temp2741.last_plus_one - _temp2741.curr, _temp2741.curr);})); goto _LL2734;
_LL2738: goto _LL2734; _LL2734:;}}}{ struct Cyc_Tcutil_CVTEnv _temp2743=({
struct Cyc_Tcutil_CVTEnv _temp2742; _temp2742.kind_env= cvtenv.kind_env;
_temp2742.free_vars= 0; _temp2742.free_evars= 0; _temp2742;}); goto _LL2744;
_LL2744: _temp2743= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2743,( void*)
Cyc_Absyn_MemKind, _temp2558); for( 0; _temp2556 != 0; _temp2556=(( struct Cyc_List_List*)
_check_null( _temp2556))->tl){ _temp2743= Cyc_Tcutil_i_check_valid_type( loc, te,
_temp2743,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp2556))->hd)).f3);} if( _temp2552 != 0){ if( _temp2554){(( void(*)(
struct _tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp2745="both c_vararg and cyc_vararg";
struct _tagged_string _temp2746; _temp2746.curr= _temp2745; _temp2746.base=
_temp2745; _temp2746.last_plus_one= _temp2745 + 29; _temp2746;}));}{ int
_temp2749; void* _temp2751; void* _temp2753; struct Cyc_Absyn_Tqual _temp2755;
struct Cyc_Core_Opt* _temp2757; struct Cyc_Absyn_VarargInfo _temp2747=*(( struct
Cyc_Absyn_VarargInfo*) _check_null( _temp2552)); _LL2758: _temp2757= _temp2747.name;
goto _LL2756; _LL2756: _temp2755= _temp2747.tq; goto _LL2754; _LL2754: _temp2753=(
void*) _temp2747.type; goto _LL2752; _LL2752: _temp2751=( void*) _temp2747.rgn;
goto _LL2750; _LL2750: _temp2749= _temp2747.inject; goto _LL2748; _LL2748:
_temp2743= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2743,( void*) Cyc_Absyn_MemKind,
_temp2753);{ void* _temp2759= Cyc_Tcutil_compress( _temp2751); struct Cyc_Core_Opt*
_temp2769; int _temp2771; struct Cyc_Core_Opt* _temp2773; struct Cyc_Core_Opt*
_temp2775; struct Cyc_Absyn_Tvar* _temp2777; _LL2761: if(( unsigned int)
_temp2759 > 4u?*(( int*) _temp2759) == Cyc_Absyn_Evar: 0){ _LL2776: _temp2775=((
struct Cyc_Absyn_Evar_struct*) _temp2759)->f1; goto _LL2774; _LL2774: _temp2773=((
struct Cyc_Absyn_Evar_struct*) _temp2759)->f2; goto _LL2772; _LL2772: _temp2771=((
struct Cyc_Absyn_Evar_struct*) _temp2759)->f3; goto _LL2770; _LL2770: _temp2769=((
struct Cyc_Absyn_Evar_struct*) _temp2759)->f4; goto _LL2762;} else{ goto _LL2763;}
_LL2763: if(( unsigned int) _temp2759 > 4u?*(( int*) _temp2759) == Cyc_Absyn_VarType:
0){ _LL2778: _temp2777=(( struct Cyc_Absyn_VarType_struct*) _temp2759)->f1; goto
_LL2764;} else{ goto _LL2765;} _LL2765: if( _temp2759 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL2766;} else{ goto _LL2767;} _LL2767: goto _LL2768; _LL2762: { void*
vr_tvar=( void*)({ struct Cyc_Absyn_VarType_struct* _temp2779=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2779[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2780; _temp2780.tag= Cyc_Absyn_VarType; _temp2780.f1=
Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_RgnKind); _temp2780;}); _temp2779;}); Cyc_Tcutil_unify(
_temp2751, vr_tvar); goto _LL2760;} _LL2764: goto _LL2760; _LL2766: Cyc_Tcutil_terr(
loc,({ unsigned char* _temp2781="varargs should be in a region, not heap-allocated";
struct _tagged_string _temp2782; _temp2782.curr= _temp2781; _temp2782.base=
_temp2781; _temp2782.last_plus_one= _temp2781 + 50; _temp2782;})); goto _LL2760;
_LL2768: goto _LL2760; _LL2760:;} _temp2743= Cyc_Tcutil_i_check_valid_type( loc,
te, _temp2743,( void*) Cyc_Absyn_RgnKind, _temp2751); if( _temp2749){ void*
_temp2783= Cyc_Tcutil_compress( _temp2753); struct Cyc_Absyn_TunionInfo
_temp2789; void* _temp2791; struct Cyc_List_List* _temp2793; void* _temp2795;
struct Cyc_Absyn_Tuniondecl* _temp2797; _LL2785: if(( unsigned int) _temp2783 >
4u?*(( int*) _temp2783) == Cyc_Absyn_TunionType: 0){ _LL2790: _temp2789=((
struct Cyc_Absyn_TunionType_struct*) _temp2783)->f1; _LL2796: _temp2795=( void*)
_temp2789.tunion_info; if(*(( int*) _temp2795) == Cyc_Absyn_KnownTunion){
_LL2798: _temp2797=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2795)->f1; goto
_LL2794;} else{ goto _LL2787;} _LL2794: _temp2793= _temp2789.targs; goto _LL2792;
_LL2792: _temp2791=( void*) _temp2789.rgn; goto _LL2786;} else{ goto _LL2787;}
_LL2787: goto _LL2788; _LL2786:{ void* _temp2799= Cyc_Tcutil_compress( _temp2791);
struct Cyc_Core_Opt* _temp2809; int _temp2811; struct Cyc_Core_Opt* _temp2813;
struct Cyc_Core_Opt* _temp2815; struct Cyc_Absyn_Tvar* _temp2817; _LL2801: if((
unsigned int) _temp2799 > 4u?*(( int*) _temp2799) == Cyc_Absyn_Evar: 0){ _LL2816:
_temp2815=(( struct Cyc_Absyn_Evar_struct*) _temp2799)->f1; goto _LL2814;
_LL2814: _temp2813=(( struct Cyc_Absyn_Evar_struct*) _temp2799)->f2; goto
_LL2812; _LL2812: _temp2811=(( struct Cyc_Absyn_Evar_struct*) _temp2799)->f3;
goto _LL2810; _LL2810: _temp2809=(( struct Cyc_Absyn_Evar_struct*) _temp2799)->f4;
goto _LL2802;} else{ goto _LL2803;} _LL2803: if(( unsigned int) _temp2799 > 4u?*((
int*) _temp2799) == Cyc_Absyn_VarType: 0){ _LL2818: _temp2817=(( struct Cyc_Absyn_VarType_struct*)
_temp2799)->f1; goto _LL2804;} else{ goto _LL2805;} _LL2805: if( _temp2799 ==(
void*) Cyc_Absyn_HeapRgn){ goto _LL2806;} else{ goto _LL2807;} _LL2807: goto
_LL2808; _LL2802: Cyc_Tcutil_unify( _temp2751, _temp2791); goto _LL2800; _LL2804:
goto _LL2800; _LL2806: Cyc_Tcutil_terr( loc,({ unsigned char* _temp2819="injected vararg [x]tunion should be in a region, not heap-allocated";
struct _tagged_string _temp2820; _temp2820.curr= _temp2819; _temp2820.base=
_temp2819; _temp2820.last_plus_one= _temp2819 + 68; _temp2820;})); goto _LL2800;
_LL2808: goto _LL2800; _LL2800:;} goto _LL2784; _LL2788: Cyc_Tcutil_terr( loc,({
unsigned char* _temp2821="can't inject a non-[x]tunion type"; struct
_tagged_string _temp2822; _temp2822.curr= _temp2821; _temp2822.base= _temp2821;
_temp2822.last_plus_one= _temp2821 + 34; _temp2822;})); goto _LL2784; _LL2784:;}}}
if(* _temp2562 != 0){ _temp2743= Cyc_Tcutil_i_check_valid_type( loc, te,
_temp2743,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_Core_Opt*)
_check_null(* _temp2562))->v);} else{ struct Cyc_Absyn_Tvar* e= Cyc_Tcutil_new_tvar((
void*) Cyc_Absyn_EffKind); _temp2743.free_vars= Cyc_Tcutil_add_free_tvar( loc,
_temp2743.free_vars, e);{ struct Cyc_List_List* effect= 0;{ struct Cyc_List_List*
tvs= _temp2743.free_vars; for( 0; tvs != 0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){ void* _temp2823=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind))->v; void* _temp2831; void*
_temp2833; _LL2825: if(( unsigned int) _temp2823 > 1u?*(( int*) _temp2823) ==
Cyc_Absyn_Eq_constr: 0){ _LL2832: _temp2831=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2823)->f1; if( _temp2831 ==( void*) Cyc_Absyn_RgnKind){ goto _LL2826;}
else{ goto _LL2827;}} else{ goto _LL2827;} _LL2827: if(( unsigned int) _temp2823
> 1u?*(( int*) _temp2823) == Cyc_Absyn_Eq_constr: 0){ _LL2834: _temp2833=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2823)->f1; if( _temp2833 ==( void*) Cyc_Absyn_EffKind){
goto _LL2828;} else{ goto _LL2829;}} else{ goto _LL2829;} _LL2829: goto _LL2830;
_LL2826: effect=({ struct Cyc_List_List* _temp2835=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2835->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2836=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2836[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2837; _temp2837.tag= Cyc_Absyn_AccessEff;
_temp2837.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp2838=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2838[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2839; _temp2839.tag= Cyc_Absyn_VarType;
_temp2839.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp2839;}); _temp2838;})); _temp2837;}); _temp2836;})); _temp2835->tl= effect;
_temp2835;}); goto _LL2824; _LL2828: effect=({ struct Cyc_List_List* _temp2840=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2840->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp2841=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2841[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2842; _temp2842.tag= Cyc_Absyn_VarType; _temp2842.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp2842;}); _temp2841;})); _temp2840->tl= effect; _temp2840;}); goto _LL2824;
_LL2830: goto _LL2824; _LL2824:;}}{ struct Cyc_List_List* ts= _temp2743.free_evars;
for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){ void*
_temp2843= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);
_LL2845: if( _temp2843 ==( void*) Cyc_Absyn_RgnKind){ goto _LL2846;} else{ goto
_LL2847;} _LL2847: if( _temp2843 ==( void*) Cyc_Absyn_EffKind){ goto _LL2848;}
else{ goto _LL2849;} _LL2849: goto _LL2850; _LL2846: effect=({ struct Cyc_List_List*
_temp2851=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2851->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2852=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2852[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2853; _temp2853.tag= Cyc_Absyn_AccessEff;
_temp2853.f1=( void*)(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);
_temp2853;}); _temp2852;})); _temp2851->tl= effect; _temp2851;}); goto _LL2844;
_LL2848: effect=({ struct Cyc_List_List* _temp2854=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2854->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( ts))->hd); _temp2854->tl= effect; _temp2854;});
goto _LL2844; _LL2850: goto _LL2844; _LL2844:;}}* _temp2562=({ struct Cyc_Core_Opt*
_temp2855=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2855->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2856=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2856[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2857; _temp2857.tag= Cyc_Absyn_JoinEff;
_temp2857.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
effect); _temp2857;}); _temp2856;})); _temp2855;});}} if(* _temp2565 != 0){
struct Cyc_List_List* bs=* _temp2565; for( 0; bs != 0; bs=(( struct Cyc_List_List*)
_check_null( bs))->tl){ struct Cyc_Absyn_Conref* _temp2858=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( bs))->hd)->kind); goto _LL2859; _LL2859: {
void* _temp2860=( void*) _temp2858->v; void* _temp2868; _LL2862: if( _temp2860
==( void*) Cyc_Absyn_No_constr){ goto _LL2863;} else{ goto _LL2864;} _LL2864:
if(( unsigned int) _temp2860 > 1u?*(( int*) _temp2860) == Cyc_Absyn_Eq_constr: 0){
_LL2869: _temp2868=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2860)->f1;
if( _temp2868 ==( void*) Cyc_Absyn_MemKind){ goto _LL2865;} else{ goto _LL2866;}}
else{ goto _LL2866;} _LL2866: goto _LL2867; _LL2863: Cyc_Tcutil_warn( loc,({
struct _tagged_string _temp2870=*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( bs))->hd)->name; xprintf("Type variable %.*s unconstrained, assuming boxed",
_temp2870.last_plus_one - _temp2870.curr, _temp2870.curr);})); goto _LL2865;
_LL2865:( void*)( _temp2858->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2871=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2871[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2872; _temp2872.tag= Cyc_Absyn_Eq_constr;
_temp2872.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp2872;}); _temp2871;})));
goto _LL2861; _LL2867: goto _LL2861; _LL2861:;}}} cvtenv.kind_env= Cyc_Tcutil_remove_bound_tvars(
_temp2743.kind_env,* _temp2565); _temp2743.free_vars= Cyc_Tcutil_remove_bound_tvars(
_temp2743.free_vars,* _temp2565);{ struct Cyc_List_List* tvs= _temp2743.free_vars;
for( 0; tvs != 0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd);}}{ struct Cyc_List_List* evs=
_temp2743.free_evars; for( 0; evs != 0; evs=(( struct Cyc_List_List*)
_check_null( evs))->tl){ cvtenv.free_evars= Cyc_Tcutil_add_free_evar( cvtenv.free_evars,(
void*)(( struct Cyc_List_List*) _check_null( evs))->hd);}} goto _LL2449;}}
_LL2475: for( 0; _temp2566 != 0; _temp2566=(( struct Cyc_List_List*) _check_null(
_temp2566))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*)
Cyc_Absyn_MemKind,(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp2566))->hd)).f2);} goto _LL2449; _LL2477:{ struct _RegionHandle _temp2873=
_new_region(); struct _RegionHandle* sprev_rgn=& _temp2873; _push_region(
sprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for( 0; _temp2568 != 0;
_temp2568=(( struct Cyc_List_List*) _check_null( _temp2568))->tl){ struct Cyc_Absyn_Structfield
_temp2876; struct Cyc_List_List* _temp2877; struct Cyc_Core_Opt* _temp2879; void*
_temp2881; struct Cyc_Absyn_Tqual _temp2883; struct _tagged_string* _temp2885;
struct Cyc_Absyn_Structfield* _temp2874=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2568))->hd; _temp2876=* _temp2874;
_LL2886: _temp2885= _temp2876.name; goto _LL2884; _LL2884: _temp2883= _temp2876.tq;
goto _LL2882; _LL2882: _temp2881=( void*) _temp2876.type; goto _LL2880; _LL2880:
_temp2879= _temp2876.width; goto _LL2878; _LL2878: _temp2877= _temp2876.attributes;
goto _LL2875; _LL2875: if((( int(*)( int(* compare)( struct _tagged_string*,
struct _tagged_string*), struct Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields, _temp2885)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2887=* _temp2885; xprintf("duplicate field %.*s in struct",
_temp2887.last_plus_one - _temp2887.curr, _temp2887.curr);}));} if( Cyc_String_strcmp(*
_temp2885,({ unsigned char* _temp2888=""; struct _tagged_string _temp2889;
_temp2889.curr= _temp2888; _temp2889.base= _temp2888; _temp2889.last_plus_one=
_temp2888 + 1; _temp2889;})) != 0){ prev_fields=({ struct Cyc_List_List*
_temp2890=( struct Cyc_List_List*) _region_malloc( sprev_rgn, sizeof( struct Cyc_List_List));
_temp2890->hd=( void*) _temp2885; _temp2890->tl= prev_fields; _temp2890;});}
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,
_temp2881); if( _temp2879 != 0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp2879))->v)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2891=* _temp2885; xprintf("bitfield %.*s does not have constant width",
_temp2891.last_plus_one - _temp2891.curr, _temp2891.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp2879))->v);}{
void* _temp2892= Cyc_Tcutil_compress( _temp2881); void* _temp2898; void*
_temp2900; _LL2894: if(( unsigned int) _temp2892 > 4u?*(( int*) _temp2892) ==
Cyc_Absyn_IntType: 0){ _LL2901: _temp2900=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2892)->f1; goto _LL2899; _LL2899: _temp2898=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2892)->f2; goto _LL2895;} else{ goto _LL2896;} _LL2896: goto _LL2897;
_LL2895:{ void* _temp2902= _temp2898; _LL2904: if( _temp2902 ==( void*) Cyc_Absyn_B1){
goto _LL2905;} else{ goto _LL2906;} _LL2906: if( _temp2902 ==( void*) Cyc_Absyn_B2){
goto _LL2907;} else{ goto _LL2908;} _LL2908: if( _temp2902 ==( void*) Cyc_Absyn_B4){
goto _LL2909;} else{ goto _LL2910;} _LL2910: if( _temp2902 ==( void*) Cyc_Absyn_B8){
goto _LL2911;} else{ goto _LL2903;} _LL2905: if( w > 8){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp2912="bitfield larger than type"; struct _tagged_string
_temp2913; _temp2913.curr= _temp2912; _temp2913.base= _temp2912; _temp2913.last_plus_one=
_temp2912 + 26; _temp2913;}));} goto _LL2903; _LL2907: if( w > 16){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp2914="bitfield larger than type"; struct
_tagged_string _temp2915; _temp2915.curr= _temp2914; _temp2915.base= _temp2914;
_temp2915.last_plus_one= _temp2914 + 26; _temp2915;}));} goto _LL2903; _LL2909:
if( w > 32){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp2916="bitfield larger than type";
struct _tagged_string _temp2917; _temp2917.curr= _temp2916; _temp2917.base=
_temp2916; _temp2917.last_plus_one= _temp2916 + 26; _temp2917;}));} goto _LL2903;
_LL2911: if( w > 64){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp2918="bitfield larger than type";
struct _tagged_string _temp2919; _temp2919.curr= _temp2918; _temp2919.base=
_temp2918; _temp2919.last_plus_one= _temp2918 + 26; _temp2919;}));} goto _LL2903;
_LL2903:;} goto _LL2893; _LL2897: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2920=* _temp2885; struct _tagged_string _temp2921= Cyc_Absynpp_typ2string(
_temp2881); xprintf("bitfield %.*s must have integral type but has type %.*s",
_temp2920.last_plus_one - _temp2920.curr, _temp2920.curr, _temp2921.last_plus_one
- _temp2921.curr, _temp2921.curr);})); goto _LL2893; _LL2893:;}} for( 0;
_temp2877 != 0; _temp2877=(( struct Cyc_List_List*) _check_null( _temp2877))->tl){
void* _temp2922=( void*)(( struct Cyc_List_List*) _check_null( _temp2877))->hd;
int _temp2930; _LL2924: if( _temp2922 ==( void*) Cyc_Absyn_Packed_att){ goto
_LL2925;} else{ goto _LL2926;} _LL2926: if(( unsigned int) _temp2922 > 16u?*((
int*) _temp2922) == Cyc_Absyn_Aligned_att: 0){ _LL2931: _temp2930=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp2922)->f1; goto _LL2927;} else{ goto _LL2928;} _LL2928: goto _LL2929;
_LL2925: continue; _LL2927: continue; _LL2929: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2932= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null( _temp2877))->hd); struct _tagged_string _temp2933=* _temp2885;
xprintf("bad attribute %.*s on struct member %.*s", _temp2932.last_plus_one -
_temp2932.curr, _temp2932.curr, _temp2933.last_plus_one - _temp2933.curr,
_temp2933.curr);})); goto _LL2923; _LL2923:;}}}; _pop_region(& _temp2873);} goto
_LL2449; _LL2479:{ struct _RegionHandle _temp2934= _new_region(); struct
_RegionHandle* uprev_rgn=& _temp2934; _push_region( uprev_rgn);{ struct Cyc_List_List*
prev_fields= 0; for( 0; _temp2570 != 0; _temp2570=(( struct Cyc_List_List*)
_check_null( _temp2570))->tl){ struct Cyc_Absyn_Structfield _temp2937; struct
Cyc_List_List* _temp2938; struct Cyc_Core_Opt* _temp2940; void* _temp2942;
struct Cyc_Absyn_Tqual _temp2944; struct _tagged_string* _temp2946; struct Cyc_Absyn_Structfield*
_temp2935=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2570))->hd; _temp2937=* _temp2935; _LL2947: _temp2946= _temp2937.name; goto
_LL2945; _LL2945: _temp2944= _temp2937.tq; goto _LL2943; _LL2943: _temp2942=(
void*) _temp2937.type; goto _LL2941; _LL2941: _temp2940= _temp2937.width; goto
_LL2939; _LL2939: _temp2938= _temp2937.attributes; goto _LL2936; _LL2936: if(((
int(*)( int(* compare)( struct _tagged_string*, struct _tagged_string*), struct
Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp,
prev_fields, _temp2946)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2948=* _temp2946; xprintf("duplicate field %.*s in struct", _temp2948.last_plus_one
- _temp2948.curr, _temp2948.curr);}));} if( Cyc_String_strcmp(* _temp2946,({
unsigned char* _temp2949=""; struct _tagged_string _temp2950; _temp2950.curr=
_temp2949; _temp2950.base= _temp2949; _temp2950.last_plus_one= _temp2949 + 1;
_temp2950;})) != 0){ prev_fields=({ struct Cyc_List_List* _temp2951=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp2951->hd=( void*)
_temp2946; _temp2951->tl= prev_fields; _temp2951;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp2942); if( ! Cyc_Tcutil_bits_only(
_temp2942)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2952=* _temp2946;
xprintf("%.*s has a type that is possibly incompatible with other members of union",
_temp2952.last_plus_one - _temp2952.curr, _temp2952.curr);}));} if( _temp2940 !=
0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp2940))->v)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2953=* _temp2946; xprintf("bitfield %.*s does not have constant width",
_temp2953.last_plus_one - _temp2953.curr, _temp2953.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp2940))->v);}{
void* _temp2954= Cyc_Tcutil_compress( _temp2942); void* _temp2960; void*
_temp2962; _LL2956: if(( unsigned int) _temp2954 > 4u?*(( int*) _temp2954) ==
Cyc_Absyn_IntType: 0){ _LL2963: _temp2962=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2954)->f1; goto _LL2961; _LL2961: _temp2960=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2954)->f2; goto _LL2957;} else{ goto _LL2958;} _LL2958: goto _LL2959;
_LL2957:{ void* _temp2964= _temp2960; _LL2966: if( _temp2964 ==( void*) Cyc_Absyn_B1){
goto _LL2967;} else{ goto _LL2968;} _LL2968: if( _temp2964 ==( void*) Cyc_Absyn_B2){
goto _LL2969;} else{ goto _LL2970;} _LL2970: if( _temp2964 ==( void*) Cyc_Absyn_B4){
goto _LL2971;} else{ goto _LL2972;} _LL2972: if( _temp2964 ==( void*) Cyc_Absyn_B8){
goto _LL2973;} else{ goto _LL2965;} _LL2967: if( w > 8){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp2974="bitfield larger than type"; struct _tagged_string
_temp2975; _temp2975.curr= _temp2974; _temp2975.base= _temp2974; _temp2975.last_plus_one=
_temp2974 + 26; _temp2975;}));} goto _LL2965; _LL2969: if( w > 16){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp2976="bitfield larger than type"; struct
_tagged_string _temp2977; _temp2977.curr= _temp2976; _temp2977.base= _temp2976;
_temp2977.last_plus_one= _temp2976 + 26; _temp2977;}));} goto _LL2965; _LL2971:
if( w > 32){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp2978="bitfield larger than type";
struct _tagged_string _temp2979; _temp2979.curr= _temp2978; _temp2979.base=
_temp2978; _temp2979.last_plus_one= _temp2978 + 26; _temp2979;}));} goto _LL2965;
_LL2973: if( w > 64){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp2980="bitfield larger than type";
struct _tagged_string _temp2981; _temp2981.curr= _temp2980; _temp2981.base=
_temp2980; _temp2981.last_plus_one= _temp2980 + 26; _temp2981;}));} goto _LL2965;
_LL2965:;} goto _LL2955; _LL2959: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2982=* _temp2946; struct _tagged_string _temp2983= Cyc_Absynpp_typ2string(
_temp2942); xprintf("bitfield %.*s must have integral type but has type %.*s",
_temp2982.last_plus_one - _temp2982.curr, _temp2982.curr, _temp2983.last_plus_one
- _temp2983.curr, _temp2983.curr);})); goto _LL2955; _LL2955:;}} for( 0;
_temp2938 != 0; _temp2938=(( struct Cyc_List_List*) _check_null( _temp2938))->tl){
void* _temp2984=( void*)(( struct Cyc_List_List*) _check_null( _temp2938))->hd;
int _temp2992; _LL2986: if( _temp2984 ==( void*) Cyc_Absyn_Packed_att){ goto
_LL2987;} else{ goto _LL2988;} _LL2988: if(( unsigned int) _temp2984 > 16u?*((
int*) _temp2984) == Cyc_Absyn_Aligned_att: 0){ _LL2993: _temp2992=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp2984)->f1; goto _LL2989;} else{ goto _LL2990;} _LL2990: goto _LL2991;
_LL2987: continue; _LL2989: continue; _LL2991: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2994= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null( _temp2938))->hd); struct _tagged_string _temp2995=* _temp2946;
xprintf("bad attribute %.*s on struct member %.*s", _temp2994.last_plus_one -
_temp2994.curr, _temp2994.curr, _temp2995.last_plus_one - _temp2995.curr,
_temp2995.curr);})); goto _LL2985; _LL2985:;}}}; _pop_region(& _temp2934);} goto
_LL2449; _LL2481: if( _temp2577 == 0){ if(* _temp2574 == 0){ Cyc_Tcutil_terr(
loc, xprintf("bad anonymous struct")); return cvtenv;}{ struct Cyc_Absyn_Structdecl*
sd=*(( struct Cyc_Absyn_Structdecl**) _check_null(* _temp2574));;}} if(*
_temp2574 == 0){ struct _handler_cons _temp2996; _push_handler(& _temp2996);{
int _temp2998= 0; if( setjmp( _temp2996.handler)){ _temp2998= 1;} if( !
_temp2998){* _temp2574=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc,( struct _tuple1*) _check_null( _temp2577));; _pop_handler();} else{
void* _temp2997=( void*) _exn_thrown; void* _temp3000= _temp2997; _LL3002: if(
_temp3000 == Cyc_Dict_Absent){ goto _LL3003;} else{ goto _LL3004;} _LL3004: goto
_LL3005; _LL3003: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp3006= Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp2577)); xprintf("unbound type struct %.*s",
_temp3006.last_plus_one - _temp3006.curr, _temp3006.curr);})); return cvtenv;
_LL3005:( void) _throw( _temp3000); _LL3001:;}}}{ struct Cyc_Absyn_Structdecl*
sd=*(( struct Cyc_Absyn_Structdecl**) _check_null(* _temp2574)); if( sd->name !=
0){*(( struct _tuple1*) _check_null( _temp2577))=(( struct _tuple1*)(( struct
Cyc_Core_Opt*) _check_null( sd->name))->v)[ _check_known_subscript_notnull( 1u,
0)];}{ struct Cyc_List_List* tvs= sd->tvs; int lvs=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( tvs); int largs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp2575); if( lvs != largs){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp3007= Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp2577));
int _temp3008= lvs; int _temp3009= largs; xprintf("struct %.*s expects %d type arguments but was given %d",
_temp3007.last_plus_one - _temp3007.curr, _temp3007.curr, _temp3008, _temp3009);}));}
for( 0; _temp2575 != 0;( _temp2575=(( struct Cyc_List_List*) _check_null(
_temp2575))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2575))->hd; void* k1=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} goto _LL2449;}} _LL2483: if( _temp2584 == 0){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp3010="missing union name"; struct _tagged_string
_temp3011; _temp3011.curr= _temp3010; _temp3011.base= _temp3010; _temp3011.last_plus_one=
_temp3010 + 19; _temp3011;})); return cvtenv;} if(* _temp2581 == 0){ struct
_handler_cons _temp3012; _push_handler(& _temp3012);{ int _temp3014= 0; if(
setjmp( _temp3012.handler)){ _temp3014= 1;} if( ! _temp3014){* _temp2581=(
struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl( te, loc,( struct
_tuple1*) _check_null( _temp2584));; _pop_handler();} else{ void* _temp3013=(
void*) _exn_thrown; void* _temp3016= _temp3013; _LL3018: if( _temp3016 == Cyc_Dict_Absent){
goto _LL3019;} else{ goto _LL3020;} _LL3020: goto _LL3021; _LL3019: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp3022= Cyc_Absynpp_qvar2string(( struct _tuple1*)
_check_null( _temp2584)); xprintf("unbound type union %.*s", _temp3022.last_plus_one
- _temp3022.curr, _temp3022.curr);})); return cvtenv; _LL3021:( void) _throw(
_temp3016); _LL3017:;}}}{ struct Cyc_Absyn_Uniondecl* ud=*(( struct Cyc_Absyn_Uniondecl**)
_check_null(* _temp2581)); if( ud->name != 0){*(( struct _tuple1*) _check_null(
_temp2584))=(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( ud->name))->v)[
_check_known_subscript_notnull( 1u, 0)];}{ struct Cyc_List_List* tvs= ud->tvs;
int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2582); if( lvs != largs){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp3023= Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp2584)); int _temp3024= lvs; int _temp3025=
largs; xprintf("union %.*s expects %d type arguments but was given %d",
_temp3023.last_plus_one - _temp3023.curr, _temp3023.curr, _temp3024, _temp3025);}));}
for( 0; _temp2582 != 0;( _temp2582=(( struct Cyc_List_List*) _check_null(
_temp2582))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2582))->hd; void* k1=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} goto _LL2449;}} _LL2485: { struct Cyc_Absyn_Typedefdecl*
td;{ struct _handler_cons _temp3026; _push_handler(& _temp3026);{ int _temp3028=
0; if( setjmp( _temp3026.handler)){ _temp3028= 1;} if( ! _temp3028){ td= Cyc_Tcenv_lookup_typedefdecl(
te, loc, _temp2591);; _pop_handler();} else{ void* _temp3027=( void*)
_exn_thrown; void* _temp3030= _temp3027; _LL3032: if( _temp3030 == Cyc_Dict_Absent){
goto _LL3033;} else{ goto _LL3034;} _LL3034: goto _LL3035; _LL3033: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp3036= Cyc_Absynpp_qvar2string( _temp2591);
xprintf("unbound typedef name %.*s", _temp3036.last_plus_one - _temp3036.curr,
_temp3036.curr);})); return cvtenv; _LL3035:( void) _throw( _temp3030); _LL3031:;}}}
_temp2591[ _check_known_subscript_notnull( 1u, 0)]=( td->name)[
_check_known_subscript_notnull( 1u, 0)];{ struct Cyc_List_List* tvs= td->tvs;
struct Cyc_List_List* ts= _temp2589; struct Cyc_List_List* inst= 0; for( 0; ts
!= 0? tvs != 0: 0;( ts=(( struct Cyc_List_List*) _check_null( ts))->tl, tvs=((
struct Cyc_List_List*) _check_null( tvs))->tl)){ void* k=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te,
cvtenv, k,( void*)(( struct Cyc_List_List*) _check_null( ts))->hd); inst=({
struct Cyc_List_List* _temp3037=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3037->hd=( void*)({ struct _tuple5* _temp3038=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp3038->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd; _temp3038->f2=( void*)(( struct
Cyc_List_List*) _check_null( ts))->hd; _temp3038;}); _temp3037->tl= inst;
_temp3037;});} if( tvs != 0? 1: ts != 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp3039= Cyc_Absynpp_qvar2string( _temp2591); xprintf("typedef %.*s expects a different number of arguments",
_temp3039.last_plus_one - _temp3039.curr, _temp3039.curr);})); return cvtenv;}{
void* new_typ= Cyc_Tcutil_substitute( inst,( void*) td->defn);* _temp2588=({
struct Cyc_Core_Opt* _temp3040=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3040->v=( void*) new_typ; _temp3040;}); goto _LL2449;}}}
_LL2487: goto _LL2449; _LL2489: _temp2595= _temp2593; goto _LL2491; _LL2491:
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2595); goto _LL2449; _LL2493: for( 0; _temp2597 != 0; _temp2597=(( struct
Cyc_List_List*) _check_null( _temp2597))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_List_List*)
_check_null( _temp2597))->hd);} goto _LL2449; _LL2449:;} if( ! Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind( t), expected_kind)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp3041= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp3042= Cyc_Absynpp_kind2string( Cyc_Tcutil_typ_kind( t)); struct
_tagged_string _temp3043= Cyc_Absynpp_kind2string( expected_kind); xprintf("type %.*s has kind %.*s but as used here needs kind %.*s",
_temp3041.last_plus_one - _temp3041.curr, _temp3041.curr, _temp3042.last_plus_one
- _temp3042.curr, _temp3042.curr, _temp3043.last_plus_one - _temp3043.curr,
_temp3043.curr);}));} return cvtenv;} struct Cyc_List_List* Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
kind_env, void* expected_kind, void* t){ struct Cyc_Tcutil_CVTEnv _temp3045= Cyc_Tcutil_i_check_valid_type(
loc, te,({ struct Cyc_Tcutil_CVTEnv _temp3044; _temp3044.kind_env= kind_env;
_temp3044.free_vars= 0; _temp3044.free_evars= 0; _temp3044;}), expected_kind, t);
goto _LL3046; _LL3046:{ struct Cyc_List_List* vs= _temp3045.free_vars; for( 0;
vs != 0; vs=(( struct Cyc_List_List*) _check_null( vs))->tl){ Cyc_Tcutil_fast_add_free_tvar(
kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( vs))->hd);}}{
struct Cyc_List_List* evs= _temp3045.free_evars; for( 0; evs != 0; evs=(( struct
Cyc_List_List*) _check_null( evs))->tl){ void* _temp3047= Cyc_Tcutil_compress((
void*)(( struct Cyc_List_List*) _check_null( evs))->hd); struct Cyc_Core_Opt*
_temp3053; struct Cyc_Core_Opt** _temp3055; int _temp3056; struct Cyc_Core_Opt*
_temp3058; struct Cyc_Core_Opt* _temp3060; _LL3049: if(( unsigned int) _temp3047
> 4u?*(( int*) _temp3047) == Cyc_Absyn_Evar: 0){ _LL3061: _temp3060=(( struct
Cyc_Absyn_Evar_struct*) _temp3047)->f1; goto _LL3059; _LL3059: _temp3058=((
struct Cyc_Absyn_Evar_struct*) _temp3047)->f2; goto _LL3057; _LL3057: _temp3056=((
struct Cyc_Absyn_Evar_struct*) _temp3047)->f3; goto _LL3054; _LL3054: _temp3053=((
struct Cyc_Absyn_Evar_struct*) _temp3047)->f4; _temp3055=&(( struct Cyc_Absyn_Evar_struct*)
_temp3047)->f4; goto _LL3050;} else{ goto _LL3051;} _LL3051: goto _LL3052;
_LL3050: if(* _temp3055 == 0){* _temp3055=({ struct Cyc_Core_Opt* _temp3062=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3062->v=(
void*) kind_env; _temp3062;});} else{ struct Cyc_List_List* _temp3063=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(* _temp3055))->v; goto
_LL3064; _LL3064: { struct Cyc_List_List* _temp3065= 0; goto _LL3066; _LL3066:
for( 0; _temp3063 != 0; _temp3063=(( struct Cyc_List_List*) _check_null(
_temp3063))->tl){ if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct
Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp3063))->hd)){ _temp3065=({ struct Cyc_List_List* _temp3067=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3067->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp3063))->hd);
_temp3067->tl= _temp3065; _temp3067;});}}* _temp3055=({ struct Cyc_Core_Opt*
_temp3068=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3068->v=( void*) _temp3065; _temp3068;});}} goto _LL3048; _LL3052: goto
_LL3048; _LL3048:;}} return _temp3045.free_vars;} void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, void* t){ struct
Cyc_List_List* _temp3069= Cyc_Tcutil_check_valid_type( loc, te, 0,( void*) Cyc_Absyn_MemKind,
t); goto _LL3070; _LL3070:{ struct Cyc_List_List* x= _temp3069; for( 0; x != 0;
x=(( struct Cyc_List_List*) _check_null( x))->tl){ struct Cyc_Absyn_Conref* c=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( x))->hd)->kind);
void* _temp3071=( void*) c->v; void* _temp3079; _LL3073: if( _temp3071 ==( void*)
Cyc_Absyn_No_constr){ goto _LL3074;} else{ goto _LL3075;} _LL3075: if((
unsigned int) _temp3071 > 1u?*(( int*) _temp3071) == Cyc_Absyn_Eq_constr: 0){
_LL3080: _temp3079=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3071)->f1;
if( _temp3079 ==( void*) Cyc_Absyn_MemKind){ goto _LL3076;} else{ goto _LL3077;}}
else{ goto _LL3077;} _LL3077: goto _LL3078; _LL3074: goto _LL3076; _LL3076:(
void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3081=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3081[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3082; _temp3082.tag= Cyc_Absyn_Eq_constr;
_temp3082.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3082;}); _temp3081;})));
goto _LL3072; _LL3078: goto _LL3072; _LL3072:;}} if( _temp3069 != 0){ void*
_temp3083= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp3089; struct
Cyc_List_List* _temp3091; struct Cyc_Absyn_VarargInfo* _temp3093; int _temp3095;
struct Cyc_List_List* _temp3097; void* _temp3099; struct Cyc_Core_Opt* _temp3101;
struct Cyc_List_List* _temp3103; struct Cyc_List_List** _temp3105; _LL3085: if((
unsigned int) _temp3083 > 4u?*(( int*) _temp3083) == Cyc_Absyn_FnType: 0){
_LL3090: _temp3089=(( struct Cyc_Absyn_FnType_struct*) _temp3083)->f1; _LL3104:
_temp3103= _temp3089.tvars; _temp3105=&((( struct Cyc_Absyn_FnType_struct*)
_temp3083)->f1).tvars; goto _LL3102; _LL3102: _temp3101= _temp3089.effect; goto
_LL3100; _LL3100: _temp3099=( void*) _temp3089.ret_typ; goto _LL3098; _LL3098:
_temp3097= _temp3089.args; goto _LL3096; _LL3096: _temp3095= _temp3089.c_varargs;
goto _LL3094; _LL3094: _temp3093= _temp3089.cyc_varargs; goto _LL3092; _LL3092:
_temp3091= _temp3089.attributes; goto _LL3086;} else{ goto _LL3087;} _LL3087:
goto _LL3088; _LL3086: if(* _temp3105 == 0){* _temp3105= _temp3069;} else{ if(
_temp3069 != 0){ struct _tagged_string s=*(( struct Cyc_Absyn_Tvar*)(( struct
Cyc_List_List*) _check_null( _temp3069))->hd)->name; if(*(( unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( unsigned char), 0) =='?'){ Cyc_Tcutil_terr( loc, xprintf("You failed to abstract over the generated type variables. \nRemove the bound type variables or else make the effects of the type explicit\nand then abstract the free type variables properly."));}
else{ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp3106= s; xprintf("unbound type variable %.*s",
_temp3106.last_plus_one - _temp3106.curr, _temp3106.curr);}));}}} goto _LL3084;
_LL3088: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp3107=*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp3069))->hd)->name; xprintf("unbound type variable %.*s",
_temp3107.last_plus_one - _temp3107.curr, _temp3107.curr);})); return; _LL3084:;}}
void Cyc_Tcutil_add_tvar_identity( struct Cyc_Absyn_Tvar* tv){ if( tv->identity
== 0){ tv->identity= Cyc_Tcutil_new_tvar_id();}} void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List* tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Tcutil_add_tvar_identity, tvs);}
void Cyc_Tcutil_check_type( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* bound_tvars, void* expected_kind, void* t){ struct Cyc_List_List*
_temp3108= Cyc_Tcutil_check_valid_type( loc, te, bound_tvars, expected_kind, t);
goto _LL3109; _LL3109: { struct Cyc_List_List* _temp3110= Cyc_Tcutil_remove_bound_tvars(
_temp3108, bound_tvars); goto _LL3111; _LL3111: { struct Cyc_List_List* fs=
_temp3110; for( 0; fs != 0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){
struct _tagged_string* _temp3112=(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->name; goto _LL3113; _LL3113: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp3114=* _temp3112; struct _tagged_string _temp3115=
Cyc_Absynpp_typ2string( t); xprintf("unbound type variable %.*s in type %.*s",
_temp3114.last_plus_one - _temp3114.curr, _temp3114.curr, _temp3115.last_plus_one
- _temp3115.curr, _temp3115.curr);}));}}}} static void Cyc_Tcutil_check_unique_unsorted(
int(* cmp)( void*, void*), struct Cyc_List_List* vs, struct Cyc_Position_Segment*
loc, struct _tagged_string(* a2string)( void*), struct _tagged_string msg){ for(
0; vs != 0; vs=(( struct Cyc_List_List*) _check_null( vs))->tl){ struct Cyc_List_List*
vs2=(( struct Cyc_List_List*) _check_null( vs))->tl; for( 0; vs2 != 0; vs2=((
struct Cyc_List_List*) _check_null( vs2))->tl){ if( cmp(( void*)(( struct Cyc_List_List*)
_check_null( vs))->hd,( void*)(( struct Cyc_List_List*) _check_null( vs2))->hd)
== 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp3116= msg; struct
_tagged_string _temp3117= a2string(( void*)(( struct Cyc_List_List*) _check_null(
vs))->hd); xprintf("%.*s: %.*s", _temp3116.last_plus_one - _temp3116.curr,
_temp3116.curr, _temp3117.last_plus_one - _temp3117.curr, _temp3117.curr);}));}}}}
static struct _tagged_string Cyc_Tcutil_strptr2string( struct _tagged_string* s){
return* s;} void Cyc_Tcutil_check_unique_vars( struct Cyc_List_List* vs, struct
Cyc_Position_Segment* loc, struct _tagged_string msg){(( void(*)( int(* cmp)(
struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List* vs,
struct Cyc_Position_Segment* loc, struct _tagged_string(* a2string)( struct
_tagged_string*), struct _tagged_string msg)) Cyc_Tcutil_check_unique_unsorted)(
Cyc_String_zstrptrcmp, vs, loc, Cyc_Tcutil_strptr2string, msg);} void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* tvs){(( void(*)( int(*
cmp)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* vs,
struct Cyc_Position_Segment* loc, struct _tagged_string(* a2string)( struct Cyc_Absyn_Tvar*),
struct _tagged_string msg)) Cyc_Tcutil_check_unique_unsorted)( Cyc_Absyn_tvar_cmp,
tvs, loc, Cyc_Tcutil_tvar2string,({ unsigned char* _temp3118="duplicate type variable";
struct _tagged_string _temp3119; _temp3119.curr= _temp3118; _temp3119.base=
_temp3118; _temp3119.last_plus_one= _temp3118 + 24; _temp3119;}));} struct
_tuple17{ struct Cyc_Absyn_Structfield* f1; int f2; } ; struct _tuple18{ struct
Cyc_List_List* f1; void* f2; } ; struct _tuple19{ struct Cyc_Absyn_Structfield*
f1; void* f2; } ; struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* sdfields){ struct Cyc_List_List* fields= 0;{ struct
Cyc_List_List* sd_fields= sdfields; for( 0; sd_fields != 0; sd_fields=(( struct
Cyc_List_List*) _check_null( sd_fields))->tl){ if( Cyc_String_strcmp(*(( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( sd_fields))->hd)->name,({
unsigned char* _temp3120=""; struct _tagged_string _temp3121; _temp3121.curr=
_temp3120; _temp3121.base= _temp3120; _temp3121.last_plus_one= _temp3120 + 1;
_temp3121;})) != 0){ fields=({ struct Cyc_List_List* _temp3122=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3122->hd=( void*)({ struct
_tuple17* _temp3123=( struct _tuple17*) GC_malloc( sizeof( struct _tuple17));
_temp3123->f1=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( sd_fields))->hd; _temp3123->f2= 0; _temp3123;}); _temp3122->tl=
fields; _temp3122;});}}} fields=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( fields);{ struct Cyc_List_List* ans= 0; for( 0; des != 0;
des=(( struct Cyc_List_List*) _check_null( des))->tl){ struct _tuple18 _temp3126;
void* _temp3127; struct Cyc_List_List* _temp3129; struct _tuple18* _temp3124=(
struct _tuple18*)(( struct Cyc_List_List*) _check_null( des))->hd; _temp3126=*
_temp3124; _LL3130: _temp3129= _temp3126.f1; goto _LL3128; _LL3128: _temp3127=
_temp3126.f2; goto _LL3125; _LL3125: if( _temp3129 == 0){ struct Cyc_List_List*
_temp3131= fields; goto _LL3132; _LL3132: for( 0; _temp3131 != 0; _temp3131=((
struct Cyc_List_List*) _check_null( _temp3131))->tl){ if( !(*(( struct _tuple17*)((
struct Cyc_List_List*) _check_null( _temp3131))->hd)).f2){(*(( struct _tuple17*)((
struct Cyc_List_List*) _check_null( _temp3131))->hd)).f2= 1;(*(( struct _tuple18*)((
struct Cyc_List_List*) _check_null( des))->hd)).f1=({ struct Cyc_List_List*
_temp3133=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3133->hd=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp3134=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp3134[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp3135; _temp3135.tag= Cyc_Absyn_FieldName;
_temp3135.f1=((*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3131))->hd)).f1)->name; _temp3135;}); _temp3134;})); _temp3133->tl= 0;
_temp3133;}); ans=({ struct Cyc_List_List* _temp3136=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp3136->hd=( void*)({
struct _tuple19* _temp3137=( struct _tuple19*) _region_malloc( rgn, sizeof(
struct _tuple19)); _temp3137->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3131))->hd)).f1; _temp3137->f2= _temp3127; _temp3137;});
_temp3136->tl= ans; _temp3136;}); break;}} if( _temp3131 == 0){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp3138="too many arguments to struct"; struct
_tagged_string _temp3139; _temp3139.curr= _temp3138; _temp3139.base= _temp3138;
_temp3139.last_plus_one= _temp3138 + 29; _temp3139;}));}} else{ if((( struct Cyc_List_List*)
_check_null( _temp3129))->tl != 0){ Cyc_Tcutil_terr( loc,({ unsigned char*
_temp3140="multiple designators are not supported"; struct _tagged_string
_temp3141; _temp3141.curr= _temp3140; _temp3141.base= _temp3140; _temp3141.last_plus_one=
_temp3140 + 39; _temp3141;}));} else{ void* _temp3142=( void*)(( struct Cyc_List_List*)
_check_null( _temp3129))->hd; struct Cyc_Absyn_Exp* _temp3148; struct
_tagged_string* _temp3150; _LL3144: if(*(( int*) _temp3142) == Cyc_Absyn_ArrayElement){
_LL3149: _temp3148=(( struct Cyc_Absyn_ArrayElement_struct*) _temp3142)->f1;
goto _LL3145;} else{ goto _LL3146;} _LL3146: if(*(( int*) _temp3142) == Cyc_Absyn_FieldName){
_LL3151: _temp3150=(( struct Cyc_Absyn_FieldName_struct*) _temp3142)->f1; goto
_LL3147;} else{ goto _LL3143;} _LL3145: Cyc_Tcutil_terr( loc,({ unsigned char*
_temp3152="array designator used in argument to struct"; struct _tagged_string
_temp3153; _temp3153.curr= _temp3152; _temp3153.base= _temp3152; _temp3153.last_plus_one=
_temp3152 + 44; _temp3153;})); goto _LL3143; _LL3147: { struct Cyc_List_List*
_temp3154= fields; goto _LL3155; _LL3155: for( 0; _temp3154 != 0; _temp3154=((
struct Cyc_List_List*) _check_null( _temp3154))->tl){ if( Cyc_String_zstrptrcmp(
_temp3150,((*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3154))->hd)).f1)->name) == 0){ if((*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3154))->hd)).f2){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp3156=* _temp3150; xprintf("field %.*s has already been used as an argument",
_temp3156.last_plus_one - _temp3156.curr, _temp3156.curr);}));}(*(( struct
_tuple17*)(( struct Cyc_List_List*) _check_null( _temp3154))->hd)).f2= 1; ans=({
struct Cyc_List_List* _temp3157=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp3157->hd=( void*)({ struct _tuple19*
_temp3158=( struct _tuple19*) _region_malloc( rgn, sizeof( struct _tuple19));
_temp3158->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3154))->hd)).f1; _temp3158->f2= _temp3127; _temp3158;}); _temp3157->tl= ans;
_temp3157;}); break;}} if( _temp3154 == 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp3159=* _temp3150; xprintf("bad field designator %.*s",
_temp3159.last_plus_one - _temp3159.curr, _temp3159.curr);}));} goto _LL3143;}
_LL3143:;}}} for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null(
fields))->tl){ if( !(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
fields))->hd)).f2){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp3160="too few arguments to struct";
struct _tagged_string _temp3161; _temp3161.curr= _temp3160; _temp3161.base=
_temp3160; _temp3161.last_plus_one= _temp3160 + 28; _temp3161;})); break;}}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
ans);}} int Cyc_Tcutil_is_tagged_pointer_typ( void* t){ void* _temp3162= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_PtrInfo _temp3168; struct Cyc_Absyn_Conref* _temp3170;
struct Cyc_Absyn_Tqual _temp3172; struct Cyc_Absyn_Conref* _temp3174; void*
_temp3176; void* _temp3178; _LL3164: if(( unsigned int) _temp3162 > 4u?*(( int*)
_temp3162) == Cyc_Absyn_PointerType: 0){ _LL3169: _temp3168=(( struct Cyc_Absyn_PointerType_struct*)
_temp3162)->f1; _LL3179: _temp3178=( void*) _temp3168.elt_typ; goto _LL3177;
_LL3177: _temp3176=( void*) _temp3168.rgn_typ; goto _LL3175; _LL3175: _temp3174=
_temp3168.nullable; goto _LL3173; _LL3173: _temp3172= _temp3168.tq; goto _LL3171;
_LL3171: _temp3170= _temp3168.bounds; goto _LL3165;} else{ goto _LL3166;}
_LL3166: goto _LL3167; _LL3165: { struct Cyc_Absyn_Conref* _temp3180=(( struct
Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp3170); goto _LL3181; _LL3181: { void* _temp3182=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3180))->v; void*
_temp3190; _LL3184: if(( unsigned int) _temp3182 > 1u?*(( int*) _temp3182) ==
Cyc_Absyn_Eq_constr: 0){ _LL3191: _temp3190=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3182)->f1; if( _temp3190 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3185;}
else{ goto _LL3186;}} else{ goto _LL3186;} _LL3186: if( _temp3182 ==( void*) Cyc_Absyn_No_constr){
goto _LL3187;} else{ goto _LL3188;} _LL3188: goto _LL3189; _LL3185: return 1;
_LL3187:( void*)( _temp3180->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3192=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3192[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3193; _temp3193.tag= Cyc_Absyn_Eq_constr;
_temp3193.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp3193;}); _temp3192;})));
return 1; _LL3189: return 0; _LL3183:;}} _LL3167: return 0; _LL3163:;} struct
_tuple7 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e){ struct _tuple7 bogus_ans=({ struct _tuple7 _temp3432; _temp3432.f1= 0;
_temp3432.f2=( void*) Cyc_Absyn_HeapRgn; _temp3432;}); void* _temp3194=( void*)
e->r; void* _temp3208; struct _tuple1* _temp3210; struct _tagged_string*
_temp3212; struct Cyc_Absyn_Exp* _temp3214; struct _tagged_string* _temp3216;
struct Cyc_Absyn_Exp* _temp3218; struct Cyc_Absyn_Exp* _temp3220; struct Cyc_Absyn_Exp*
_temp3222; struct Cyc_Absyn_Exp* _temp3224; _LL3196: if(*(( int*) _temp3194) ==
Cyc_Absyn_Var_e){ _LL3211: _temp3210=(( struct Cyc_Absyn_Var_e_struct*)
_temp3194)->f1; goto _LL3209; _LL3209: _temp3208=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3194)->f2; goto _LL3197;} else{ goto _LL3198;} _LL3198: if(*(( int*)
_temp3194) == Cyc_Absyn_StructMember_e){ _LL3215: _temp3214=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3194)->f1; goto _LL3213; _LL3213: _temp3212=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3194)->f2; goto _LL3199;} else{ goto _LL3200;} _LL3200: if(*(( int*)
_temp3194) == Cyc_Absyn_StructArrow_e){ _LL3219: _temp3218=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3194)->f1; goto _LL3217; _LL3217: _temp3216=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3194)->f2; goto _LL3201;} else{ goto _LL3202;} _LL3202: if(*(( int*)
_temp3194) == Cyc_Absyn_Deref_e){ _LL3221: _temp3220=(( struct Cyc_Absyn_Deref_e_struct*)
_temp3194)->f1; goto _LL3203;} else{ goto _LL3204;} _LL3204: if(*(( int*)
_temp3194) == Cyc_Absyn_Subscript_e){ _LL3225: _temp3224=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3194)->f1; goto _LL3223; _LL3223: _temp3222=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3194)->f2; goto _LL3205;} else{ goto _LL3206;} _LL3206: goto _LL3207;
_LL3197: { void* _temp3226= _temp3208; struct Cyc_Absyn_Fndecl* _temp3240;
struct Cyc_Absyn_Vardecl* _temp3242; struct Cyc_Absyn_Vardecl* _temp3244; struct
Cyc_Absyn_Vardecl* _temp3246; struct Cyc_Absyn_Vardecl* _temp3248; _LL3228: if(
_temp3226 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL3229;} else{ goto _LL3230;}
_LL3230: if(( unsigned int) _temp3226 > 1u?*(( int*) _temp3226) == Cyc_Absyn_Funname_b:
0){ _LL3241: _temp3240=(( struct Cyc_Absyn_Funname_b_struct*) _temp3226)->f1;
goto _LL3231;} else{ goto _LL3232;} _LL3232: if(( unsigned int) _temp3226 > 1u?*((
int*) _temp3226) == Cyc_Absyn_Global_b: 0){ _LL3243: _temp3242=(( struct Cyc_Absyn_Global_b_struct*)
_temp3226)->f1; goto _LL3233;} else{ goto _LL3234;} _LL3234: if(( unsigned int)
_temp3226 > 1u?*(( int*) _temp3226) == Cyc_Absyn_Local_b: 0){ _LL3245: _temp3244=((
struct Cyc_Absyn_Local_b_struct*) _temp3226)->f1; goto _LL3235;} else{ goto
_LL3236;} _LL3236: if(( unsigned int) _temp3226 > 1u?*(( int*) _temp3226) == Cyc_Absyn_Pat_b:
0){ _LL3247: _temp3246=(( struct Cyc_Absyn_Pat_b_struct*) _temp3226)->f1; goto
_LL3237;} else{ goto _LL3238;} _LL3238: if(( unsigned int) _temp3226 > 1u?*((
int*) _temp3226) == Cyc_Absyn_Param_b: 0){ _LL3249: _temp3248=(( struct Cyc_Absyn_Param_b_struct*)
_temp3226)->f1; goto _LL3239;} else{ goto _LL3227;} _LL3229: return bogus_ans;
_LL3231: return({ struct _tuple7 _temp3250; _temp3250.f1= 1; _temp3250.f2=( void*)
Cyc_Absyn_HeapRgn; _temp3250;}); _LL3233: { void* _temp3251= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); struct Cyc_Absyn_Exp*
_temp3257; struct Cyc_Absyn_Tqual _temp3259; void* _temp3261; _LL3253: if((
unsigned int) _temp3251 > 4u?*(( int*) _temp3251) == Cyc_Absyn_ArrayType: 0){
_LL3262: _temp3261=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3251)->f1;
goto _LL3260; _LL3260: _temp3259=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3251)->f2; goto _LL3258; _LL3258: _temp3257=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3251)->f3; goto _LL3254;} else{ goto _LL3255;} _LL3255: goto _LL3256;
_LL3254: return({ struct _tuple7 _temp3263; _temp3263.f1= 1; _temp3263.f2=( void*)
Cyc_Absyn_HeapRgn; _temp3263;}); _LL3256: return({ struct _tuple7 _temp3264;
_temp3264.f1=( _temp3242->tq).q_const; _temp3264.f2=( void*) Cyc_Absyn_HeapRgn;
_temp3264;}); _LL3252:;} _LL3235: { void* _temp3265= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v); struct Cyc_Absyn_Exp*
_temp3271; struct Cyc_Absyn_Tqual _temp3273; void* _temp3275; _LL3267: if((
unsigned int) _temp3265 > 4u?*(( int*) _temp3265) == Cyc_Absyn_ArrayType: 0){
_LL3276: _temp3275=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3265)->f1;
goto _LL3274; _LL3274: _temp3273=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3265)->f2; goto _LL3272; _LL3272: _temp3271=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3265)->f3; goto _LL3268;} else{ goto _LL3269;} _LL3269: goto _LL3270;
_LL3268: return({ struct _tuple7 _temp3277; _temp3277.f1= 1; _temp3277.f2=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp3244->rgn))->v; _temp3277;}); _LL3270:
return({ struct _tuple7 _temp3278; _temp3278.f1=( _temp3244->tq).q_const;
_temp3278.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3244->rgn))->v;
_temp3278;}); _LL3266:;} _LL3237: _temp3248= _temp3246; goto _LL3239; _LL3239:
return({ struct _tuple7 _temp3279; _temp3279.f1=( _temp3248->tq).q_const;
_temp3279.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3248->rgn))->v;
_temp3279;}); _LL3227:;} _LL3199: { void* _temp3280= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp3214->topt))->v); struct Cyc_List_List*
_temp3292; struct Cyc_List_List* _temp3294; struct Cyc_Absyn_Structdecl**
_temp3296; struct Cyc_Absyn_Structdecl* _temp3298; struct Cyc_List_List*
_temp3299; struct _tuple1* _temp3301; struct Cyc_Absyn_Uniondecl** _temp3303;
struct Cyc_Absyn_Uniondecl* _temp3305; struct Cyc_List_List* _temp3306; struct
_tuple1* _temp3308; _LL3282: if(( unsigned int) _temp3280 > 4u?*(( int*)
_temp3280) == Cyc_Absyn_AnonStructType: 0){ _LL3293: _temp3292=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3280)->f1; goto _LL3283;} else{ goto _LL3284;} _LL3284: if(( unsigned int)
_temp3280 > 4u?*(( int*) _temp3280) == Cyc_Absyn_AnonUnionType: 0){ _LL3295:
_temp3294=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3280)->f1; goto
_LL3285;} else{ goto _LL3286;} _LL3286: if(( unsigned int) _temp3280 > 4u?*((
int*) _temp3280) == Cyc_Absyn_StructType: 0){ _LL3302: _temp3301=(( struct Cyc_Absyn_StructType_struct*)
_temp3280)->f1; goto _LL3300; _LL3300: _temp3299=(( struct Cyc_Absyn_StructType_struct*)
_temp3280)->f2; goto _LL3297; _LL3297: _temp3296=(( struct Cyc_Absyn_StructType_struct*)
_temp3280)->f3; if( _temp3296 == 0){ goto _LL3288;} else{ _temp3298=* _temp3296;
goto _LL3287;}} else{ goto _LL3288;} _LL3288: if(( unsigned int) _temp3280 > 4u?*((
int*) _temp3280) == Cyc_Absyn_UnionType: 0){ _LL3309: _temp3308=(( struct Cyc_Absyn_UnionType_struct*)
_temp3280)->f1; goto _LL3307; _LL3307: _temp3306=(( struct Cyc_Absyn_UnionType_struct*)
_temp3280)->f2; goto _LL3304; _LL3304: _temp3303=(( struct Cyc_Absyn_UnionType_struct*)
_temp3280)->f3; if( _temp3303 == 0){ goto _LL3290;} else{ _temp3305=* _temp3303;
goto _LL3289;}} else{ goto _LL3290;} _LL3290: goto _LL3291; _LL3283: _temp3294=
_temp3292; goto _LL3285; _LL3285: { struct Cyc_Absyn_Structfield* _temp3310= Cyc_Absyn_lookup_field(
_temp3294, _temp3212); goto _LL3311; _LL3311: if( _temp3310 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3310))->width != 0: 0){ return({ struct _tuple7 _temp3312;
_temp3312.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3310))->tq).q_const;
_temp3312.f2=( Cyc_Tcutil_addressof_props( te, _temp3214)).f2; _temp3312;});}
return bogus_ans;} _LL3287: { struct Cyc_Absyn_Structfield* _temp3313= Cyc_Absyn_lookup_struct_field(
_temp3298, _temp3212); goto _LL3314; _LL3314: if( _temp3313 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3313))->width != 0: 0){ return({ struct _tuple7 _temp3315;
_temp3315.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3313))->tq).q_const;
_temp3315.f2=( Cyc_Tcutil_addressof_props( te, _temp3214)).f2; _temp3315;});}
return bogus_ans;} _LL3289: { struct Cyc_Absyn_Structfield* _temp3316= Cyc_Absyn_lookup_union_field(
_temp3305, _temp3212); goto _LL3317; _LL3317: if( _temp3316 != 0){ return({
struct _tuple7 _temp3318; _temp3318.f1=((( struct Cyc_Absyn_Structfield*)
_check_null( _temp3316))->tq).q_const; _temp3318.f2=( Cyc_Tcutil_addressof_props(
te, _temp3214)).f2; _temp3318;});} goto _LL3291;} _LL3291: return bogus_ans;
_LL3281:;} _LL3201: { void* _temp3319= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3218->topt))->v); struct Cyc_Absyn_PtrInfo _temp3325; struct
Cyc_Absyn_Conref* _temp3327; struct Cyc_Absyn_Tqual _temp3329; struct Cyc_Absyn_Conref*
_temp3331; void* _temp3333; void* _temp3335; _LL3321: if(( unsigned int)
_temp3319 > 4u?*(( int*) _temp3319) == Cyc_Absyn_PointerType: 0){ _LL3326:
_temp3325=(( struct Cyc_Absyn_PointerType_struct*) _temp3319)->f1; _LL3336:
_temp3335=( void*) _temp3325.elt_typ; goto _LL3334; _LL3334: _temp3333=( void*)
_temp3325.rgn_typ; goto _LL3332; _LL3332: _temp3331= _temp3325.nullable; goto
_LL3330; _LL3330: _temp3329= _temp3325.tq; goto _LL3328; _LL3328: _temp3327=
_temp3325.bounds; goto _LL3322;} else{ goto _LL3323;} _LL3323: goto _LL3324;
_LL3322: { void* _temp3337= Cyc_Tcutil_compress( _temp3335); struct Cyc_List_List*
_temp3349; struct Cyc_List_List* _temp3351; struct Cyc_Absyn_Structdecl**
_temp3353; struct Cyc_Absyn_Structdecl* _temp3355; struct Cyc_List_List*
_temp3356; struct _tuple1* _temp3358; struct Cyc_Absyn_Uniondecl** _temp3360;
struct Cyc_Absyn_Uniondecl* _temp3362; struct Cyc_List_List* _temp3363; struct
_tuple1* _temp3365; _LL3339: if(( unsigned int) _temp3337 > 4u?*(( int*)
_temp3337) == Cyc_Absyn_AnonStructType: 0){ _LL3350: _temp3349=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3337)->f1; goto _LL3340;} else{ goto _LL3341;} _LL3341: if(( unsigned int)
_temp3337 > 4u?*(( int*) _temp3337) == Cyc_Absyn_AnonUnionType: 0){ _LL3352:
_temp3351=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3337)->f1; goto
_LL3342;} else{ goto _LL3343;} _LL3343: if(( unsigned int) _temp3337 > 4u?*((
int*) _temp3337) == Cyc_Absyn_StructType: 0){ _LL3359: _temp3358=(( struct Cyc_Absyn_StructType_struct*)
_temp3337)->f1; goto _LL3357; _LL3357: _temp3356=(( struct Cyc_Absyn_StructType_struct*)
_temp3337)->f2; goto _LL3354; _LL3354: _temp3353=(( struct Cyc_Absyn_StructType_struct*)
_temp3337)->f3; if( _temp3353 == 0){ goto _LL3345;} else{ _temp3355=* _temp3353;
goto _LL3344;}} else{ goto _LL3345;} _LL3345: if(( unsigned int) _temp3337 > 4u?*((
int*) _temp3337) == Cyc_Absyn_UnionType: 0){ _LL3366: _temp3365=(( struct Cyc_Absyn_UnionType_struct*)
_temp3337)->f1; goto _LL3364; _LL3364: _temp3363=(( struct Cyc_Absyn_UnionType_struct*)
_temp3337)->f2; goto _LL3361; _LL3361: _temp3360=(( struct Cyc_Absyn_UnionType_struct*)
_temp3337)->f3; if( _temp3360 == 0){ goto _LL3347;} else{ _temp3362=* _temp3360;
goto _LL3346;}} else{ goto _LL3347;} _LL3347: goto _LL3348; _LL3340: _temp3351=
_temp3349; goto _LL3342; _LL3342: { struct Cyc_Absyn_Structfield* _temp3367= Cyc_Absyn_lookup_field(
_temp3351, _temp3216); goto _LL3368; _LL3368: if( _temp3367 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3367))->width != 0: 0){ return({ struct _tuple7 _temp3369;
_temp3369.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3367))->tq).q_const;
_temp3369.f2= _temp3333; _temp3369;});} return bogus_ans;} _LL3344: { struct Cyc_Absyn_Structfield*
_temp3370= Cyc_Absyn_lookup_struct_field( _temp3355, _temp3216); goto _LL3371;
_LL3371: if( _temp3370 != 0?(( struct Cyc_Absyn_Structfield*) _check_null(
_temp3370))->width != 0: 0){ return({ struct _tuple7 _temp3372; _temp3372.f1=(((
struct Cyc_Absyn_Structfield*) _check_null( _temp3370))->tq).q_const; _temp3372.f2=
_temp3333; _temp3372;});} return bogus_ans;} _LL3346: { struct Cyc_Absyn_Structfield*
_temp3373= Cyc_Absyn_lookup_union_field( _temp3362, _temp3216); goto _LL3374;
_LL3374: if( _temp3373 != 0){ return({ struct _tuple7 _temp3375; _temp3375.f1=(((
struct Cyc_Absyn_Structfield*) _check_null( _temp3373))->tq).q_const; _temp3375.f2=
_temp3333; _temp3375;});} return bogus_ans;} _LL3348: return bogus_ans; _LL3338:;}
_LL3324: return bogus_ans; _LL3320:;} _LL3203: { void* _temp3376= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp3220->topt))->v); struct Cyc_Absyn_PtrInfo
_temp3382; struct Cyc_Absyn_Conref* _temp3384; struct Cyc_Absyn_Tqual _temp3386;
struct Cyc_Absyn_Conref* _temp3388; void* _temp3390; void* _temp3392; _LL3378:
if(( unsigned int) _temp3376 > 4u?*(( int*) _temp3376) == Cyc_Absyn_PointerType:
0){ _LL3383: _temp3382=(( struct Cyc_Absyn_PointerType_struct*) _temp3376)->f1;
_LL3393: _temp3392=( void*) _temp3382.elt_typ; goto _LL3391; _LL3391: _temp3390=(
void*) _temp3382.rgn_typ; goto _LL3389; _LL3389: _temp3388= _temp3382.nullable;
goto _LL3387; _LL3387: _temp3386= _temp3382.tq; goto _LL3385; _LL3385: _temp3384=
_temp3382.bounds; goto _LL3379;} else{ goto _LL3380;} _LL3380: goto _LL3381;
_LL3379: return({ struct _tuple7 _temp3394; _temp3394.f1= _temp3386.q_const;
_temp3394.f2= _temp3390; _temp3394;}); _LL3381: return bogus_ans; _LL3377:;}
_LL3205: { void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3224->topt))->v); void* _temp3395= t; struct Cyc_Absyn_Exp*
_temp3405; struct Cyc_Absyn_Tqual _temp3407; void* _temp3409; struct Cyc_List_List*
_temp3411; struct Cyc_Absyn_PtrInfo _temp3413; struct Cyc_Absyn_Conref*
_temp3415; struct Cyc_Absyn_Tqual _temp3417; struct Cyc_Absyn_Conref* _temp3419;
void* _temp3421; void* _temp3423; _LL3397: if(( unsigned int) _temp3395 > 4u?*((
int*) _temp3395) == Cyc_Absyn_ArrayType: 0){ _LL3410: _temp3409=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3395)->f1; goto _LL3408; _LL3408:
_temp3407=(( struct Cyc_Absyn_ArrayType_struct*) _temp3395)->f2; goto _LL3406;
_LL3406: _temp3405=(( struct Cyc_Absyn_ArrayType_struct*) _temp3395)->f3; goto
_LL3398;} else{ goto _LL3399;} _LL3399: if(( unsigned int) _temp3395 > 4u?*((
int*) _temp3395) == Cyc_Absyn_TupleType: 0){ _LL3412: _temp3411=(( struct Cyc_Absyn_TupleType_struct*)
_temp3395)->f1; goto _LL3400;} else{ goto _LL3401;} _LL3401: if(( unsigned int)
_temp3395 > 4u?*(( int*) _temp3395) == Cyc_Absyn_PointerType: 0){ _LL3414:
_temp3413=(( struct Cyc_Absyn_PointerType_struct*) _temp3395)->f1; _LL3424:
_temp3423=( void*) _temp3413.elt_typ; goto _LL3422; _LL3422: _temp3421=( void*)
_temp3413.rgn_typ; goto _LL3420; _LL3420: _temp3419= _temp3413.nullable; goto
_LL3418; _LL3418: _temp3417= _temp3413.tq; goto _LL3416; _LL3416: _temp3415=
_temp3413.bounds; goto _LL3402;} else{ goto _LL3403;} _LL3403: goto _LL3404;
_LL3398: return({ struct _tuple7 _temp3425; _temp3425.f1= _temp3407.q_const;
_temp3425.f2=( Cyc_Tcutil_addressof_props( te, _temp3224)).f2; _temp3425;});
_LL3400: { struct _tuple4* _temp3426= Cyc_Absyn_lookup_tuple_field( _temp3411,(
int) Cyc_Evexp_eval_const_uint_exp( _temp3222)); goto _LL3427; _LL3427: if(
_temp3426 != 0){ return({ struct _tuple7 _temp3428; _temp3428.f1=((*(( struct
_tuple4*) _check_null( _temp3426))).f1).q_const; _temp3428.f2=( Cyc_Tcutil_addressof_props(
te, _temp3224)).f2; _temp3428;});} return bogus_ans;} _LL3402: return({ struct
_tuple7 _temp3429; _temp3429.f1= _temp3417.q_const; _temp3429.f2= _temp3421;
_temp3429;}); _LL3404: return bogus_ans; _LL3396:;} _LL3207: Cyc_Tcutil_terr( e->loc,({
unsigned char* _temp3430="unary & applied to non-lvalue"; struct _tagged_string
_temp3431; _temp3431.curr= _temp3430; _temp3431.base= _temp3430; _temp3431.last_plus_one=
_temp3430 + 30; _temp3431;})); return bogus_ans; _LL3195:;} void* Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv* te, void* e_typ, struct Cyc_Absyn_Exp* e){ void*
_temp3433= Cyc_Tcutil_compress( e_typ); struct Cyc_Absyn_Exp* _temp3439; struct
Cyc_Absyn_Tqual _temp3441; void* _temp3443; _LL3435: if(( unsigned int)
_temp3433 > 4u?*(( int*) _temp3433) == Cyc_Absyn_ArrayType: 0){ _LL3444:
_temp3443=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3433)->f1; goto
_LL3442; _LL3442: _temp3441=(( struct Cyc_Absyn_ArrayType_struct*) _temp3433)->f2;
goto _LL3440; _LL3440: _temp3439=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3433)->f3; goto _LL3436;} else{ goto _LL3437;} _LL3437: goto _LL3438;
_LL3436: { void* _temp3447; int _temp3449; struct _tuple7 _temp3445= Cyc_Tcutil_addressof_props(
te, e); _LL3450: _temp3449= _temp3445.f1; goto _LL3448; _LL3448: _temp3447=
_temp3445.f2; goto _LL3446; _LL3446: return Cyc_Absyn_atb_typ( _temp3443,
_temp3447, _temp3441,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3451=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3451[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3452; _temp3452.tag= Cyc_Absyn_Upper_b;
_temp3452.f1= e; _temp3452;}); _temp3451;}));} _LL3438: return e_typ; _LL3434:;}
void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment* loc, unsigned int i,
struct Cyc_Absyn_Conref* b){ b=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( b);{ void* _temp3453=( void*) b->v; void*
_temp3461; void* _temp3463; struct Cyc_Absyn_Exp* _temp3465; _LL3455: if((
unsigned int) _temp3453 > 1u?*(( int*) _temp3453) == Cyc_Absyn_Eq_constr: 0){
_LL3462: _temp3461=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3453)->f1;
if( _temp3461 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3456;} else{ goto _LL3457;}}
else{ goto _LL3457;} _LL3457: if(( unsigned int) _temp3453 > 1u?*(( int*)
_temp3453) == Cyc_Absyn_Eq_constr: 0){ _LL3464: _temp3463=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3453)->f1; if(( unsigned int) _temp3463 > 1u?*(( int*) _temp3463) == Cyc_Absyn_Upper_b:
0){ _LL3466: _temp3465=(( struct Cyc_Absyn_Upper_b_struct*) _temp3463)->f1; goto
_LL3458;} else{ goto _LL3459;}} else{ goto _LL3459;} _LL3459: goto _LL3460;
_LL3456: return; _LL3458: if( Cyc_Evexp_eval_const_uint_exp( _temp3465) <= i){
Cyc_Tcutil_terr( loc,({ unsigned char* _temp3467="dereference is out of bounds";
struct _tagged_string _temp3468; _temp3468.curr= _temp3467; _temp3468.base=
_temp3467; _temp3468.last_plus_one= _temp3467 + 29; _temp3468;}));} return;
_LL3460:( void*)( b->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3469=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3469[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3470; _temp3470.tag= Cyc_Absyn_Eq_constr;
_temp3470.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3471=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3471[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3472; _temp3472.tag= Cyc_Absyn_Upper_b;
_temp3472.f1= Cyc_Absyn_uint_exp( i + 1, 0); _temp3472;}); _temp3471;}));
_temp3470;}); _temp3469;}))); return; _LL3454:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_bits_only( void* t){ void* _temp3473= Cyc_Tcutil_compress(
t); void* _temp3499; void* _temp3501; struct Cyc_Absyn_Enumdecl* _temp3503;
struct _tuple1* _temp3505; struct Cyc_Absyn_Exp* _temp3507; struct Cyc_Absyn_Tqual
_temp3509; void* _temp3511; struct Cyc_List_List* _temp3513; struct Cyc_Absyn_Structdecl**
_temp3515; struct Cyc_List_List* _temp3517; struct _tuple1* _temp3519; struct
Cyc_Absyn_Uniondecl** _temp3521; struct Cyc_List_List* _temp3523; struct _tuple1*
_temp3525; struct Cyc_List_List* _temp3527; struct Cyc_List_List* _temp3529;
_LL3475: if( _temp3473 ==( void*) Cyc_Absyn_VoidType){ goto _LL3476;} else{ goto
_LL3477;} _LL3477: if(( unsigned int) _temp3473 > 4u?*(( int*) _temp3473) == Cyc_Absyn_IntType:
0){ _LL3502: _temp3501=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3473)->f1;
goto _LL3500; _LL3500: _temp3499=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3473)->f2; goto _LL3478;} else{ goto _LL3479;} _LL3479: if( _temp3473 ==(
void*) Cyc_Absyn_FloatType){ goto _LL3480;} else{ goto _LL3481;} _LL3481: if(
_temp3473 ==( void*) Cyc_Absyn_DoubleType){ goto _LL3482;} else{ goto _LL3483;}
_LL3483: if(( unsigned int) _temp3473 > 4u?*(( int*) _temp3473) == Cyc_Absyn_EnumType:
0){ _LL3506: _temp3505=(( struct Cyc_Absyn_EnumType_struct*) _temp3473)->f1;
goto _LL3504; _LL3504: _temp3503=(( struct Cyc_Absyn_EnumType_struct*) _temp3473)->f2;
goto _LL3484;} else{ goto _LL3485;} _LL3485: if(( unsigned int) _temp3473 > 4u?*((
int*) _temp3473) == Cyc_Absyn_ArrayType: 0){ _LL3512: _temp3511=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3473)->f1; goto _LL3510; _LL3510:
_temp3509=(( struct Cyc_Absyn_ArrayType_struct*) _temp3473)->f2; goto _LL3508;
_LL3508: _temp3507=(( struct Cyc_Absyn_ArrayType_struct*) _temp3473)->f3; goto
_LL3486;} else{ goto _LL3487;} _LL3487: if(( unsigned int) _temp3473 > 4u?*((
int*) _temp3473) == Cyc_Absyn_TupleType: 0){ _LL3514: _temp3513=(( struct Cyc_Absyn_TupleType_struct*)
_temp3473)->f1; goto _LL3488;} else{ goto _LL3489;} _LL3489: if(( unsigned int)
_temp3473 > 4u?*(( int*) _temp3473) == Cyc_Absyn_StructType: 0){ _LL3520:
_temp3519=(( struct Cyc_Absyn_StructType_struct*) _temp3473)->f1; goto _LL3518;
_LL3518: _temp3517=(( struct Cyc_Absyn_StructType_struct*) _temp3473)->f2; goto
_LL3516; _LL3516: _temp3515=(( struct Cyc_Absyn_StructType_struct*) _temp3473)->f3;
goto _LL3490;} else{ goto _LL3491;} _LL3491: if(( unsigned int) _temp3473 > 4u?*((
int*) _temp3473) == Cyc_Absyn_UnionType: 0){ _LL3526: _temp3525=(( struct Cyc_Absyn_UnionType_struct*)
_temp3473)->f1; goto _LL3524; _LL3524: _temp3523=(( struct Cyc_Absyn_UnionType_struct*)
_temp3473)->f2; goto _LL3522; _LL3522: _temp3521=(( struct Cyc_Absyn_UnionType_struct*)
_temp3473)->f3; goto _LL3492;} else{ goto _LL3493;} _LL3493: if(( unsigned int)
_temp3473 > 4u?*(( int*) _temp3473) == Cyc_Absyn_AnonStructType: 0){ _LL3528:
_temp3527=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3473)->f1; goto
_LL3494;} else{ goto _LL3495;} _LL3495: if(( unsigned int) _temp3473 > 4u?*((
int*) _temp3473) == Cyc_Absyn_AnonUnionType: 0){ _LL3530: _temp3529=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3473)->f1; goto _LL3496;} else{ goto
_LL3497;} _LL3497: goto _LL3498; _LL3476: return 1; _LL3478: return 1; _LL3480:
return 1; _LL3482: return 1; _LL3484: return 0; _LL3486: return Cyc_Tcutil_bits_only(
_temp3511); _LL3488: for( 0; _temp3513 != 0; _temp3513=(( struct Cyc_List_List*)
_check_null( _temp3513))->tl){ if( ! Cyc_Tcutil_bits_only((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp3513))->hd)).f2)){ return 0;}} return 1;
_LL3490: if( _temp3515 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* _temp3531=*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp3515)); goto _LL3532; _LL3532:
if( _temp3531->fields == 0){ return 0;}{ struct _RegionHandle _temp3533=
_new_region(); struct _RegionHandle* rgn=& _temp3533; _push_region( rgn);{
struct Cyc_List_List* _temp3534=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp3531->tvs, _temp3517); goto _LL3535; _LL3535:{
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp3531->fields))->v; for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute( rgn,
_temp3534,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3536= 0; _npop_handler( 0u); return
_temp3536;}}}{ int _temp3537= 1; _npop_handler( 0u); return _temp3537;}};
_pop_region(& _temp3533);}} _LL3492: if( _temp3521 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
_temp3538=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp3521)); goto
_LL3539; _LL3539: if( _temp3538->fields == 0){ return 0;}{ struct _RegionHandle
_temp3540= _new_region(); struct _RegionHandle* rgn=& _temp3540; _push_region(
rgn);{ struct Cyc_List_List* _temp3541=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp3538->tvs, _temp3523); goto _LL3542; _LL3542:{
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp3538->fields))->v; for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute( rgn,
_temp3541,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3543= 0; _npop_handler( 0u); return
_temp3543;}}}{ int _temp3544= 1; _npop_handler( 0u); return _temp3544;}};
_pop_region(& _temp3540);}} _LL3494: _temp3529= _temp3527; goto _LL3496; _LL3496:
for( 0; _temp3529 != 0; _temp3529=(( struct Cyc_List_List*) _check_null(
_temp3529))->tl){ if( ! Cyc_Tcutil_bits_only(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp3529))->hd)->type)){ return 0;}} return
1; _LL3498: return 0; _LL3474:;} struct _tuple20{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; static int Cyc_Tcutil_cnst_exp( struct Cyc_Tcenv_Tenv*
te, int var_okay, struct Cyc_Absyn_Exp* e){ void* _temp3545=( void*) e->r; void*
_temp3585; void* _temp3587; struct Cyc_Absyn_Exp* _temp3589; struct Cyc_Absyn_Enumfield*
_temp3591; struct Cyc_Absyn_Enumdecl* _temp3593; struct _tuple1* _temp3595; void*
_temp3597; struct _tuple1* _temp3599; struct Cyc_Absyn_Exp* _temp3601; struct
Cyc_Absyn_Exp* _temp3603; struct Cyc_Absyn_Exp* _temp3605; struct Cyc_Absyn_Exp*
_temp3607; struct Cyc_Absyn_Exp* _temp3609; struct Cyc_Absyn_Exp* _temp3611;
struct Cyc_List_List* _temp3613; struct Cyc_Absyn_Exp* _temp3615; struct Cyc_Absyn_Exp*
_temp3617; void* _temp3619; struct Cyc_Absyn_Exp* _temp3621; struct Cyc_Absyn_Exp*
_temp3623; struct Cyc_Absyn_Exp* _temp3625; struct Cyc_Absyn_Vardecl* _temp3627;
struct Cyc_List_List* _temp3629; struct Cyc_List_List* _temp3631; void*
_temp3633; struct Cyc_Absyn_Structdecl* _temp3635; struct Cyc_List_List*
_temp3637; struct Cyc_Core_Opt* _temp3639; struct _tuple1* _temp3641; struct Cyc_List_List*
_temp3643; void* _temp3645; struct Cyc_List_List* _temp3647; struct Cyc_Absyn_Tunionfield*
_temp3649; struct Cyc_Absyn_Tuniondecl* _temp3651; struct Cyc_List_List*
_temp3653; struct Cyc_Core_Opt* _temp3655; struct Cyc_Core_Opt* _temp3657;
_LL3547: if(*(( int*) _temp3545) == Cyc_Absyn_Const_e){ _LL3586: _temp3585=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp3545)->f1; goto _LL3548;} else{
goto _LL3549;} _LL3549: if(*(( int*) _temp3545) == Cyc_Absyn_Sizeoftyp_e){
_LL3588: _temp3587=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp3545)->f1;
goto _LL3550;} else{ goto _LL3551;} _LL3551: if(*(( int*) _temp3545) == Cyc_Absyn_Sizeofexp_e){
_LL3590: _temp3589=(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp3545)->f1; goto
_LL3552;} else{ goto _LL3553;} _LL3553: if(*(( int*) _temp3545) == Cyc_Absyn_Enum_e){
_LL3596: _temp3595=(( struct Cyc_Absyn_Enum_e_struct*) _temp3545)->f1; goto
_LL3594; _LL3594: _temp3593=(( struct Cyc_Absyn_Enum_e_struct*) _temp3545)->f2;
goto _LL3592; _LL3592: _temp3591=(( struct Cyc_Absyn_Enum_e_struct*) _temp3545)->f3;
goto _LL3554;} else{ goto _LL3555;} _LL3555: if(*(( int*) _temp3545) == Cyc_Absyn_Var_e){
_LL3600: _temp3599=(( struct Cyc_Absyn_Var_e_struct*) _temp3545)->f1; goto
_LL3598; _LL3598: _temp3597=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp3545)->f2;
goto _LL3556;} else{ goto _LL3557;} _LL3557: if(*(( int*) _temp3545) == Cyc_Absyn_Conditional_e){
_LL3606: _temp3605=(( struct Cyc_Absyn_Conditional_e_struct*) _temp3545)->f1;
goto _LL3604; _LL3604: _temp3603=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3545)->f2; goto _LL3602; _LL3602: _temp3601=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3545)->f3; goto _LL3558;} else{ goto _LL3559;} _LL3559: if(*(( int*)
_temp3545) == Cyc_Absyn_SeqExp_e){ _LL3610: _temp3609=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3545)->f1; goto _LL3608; _LL3608: _temp3607=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3545)->f2; goto _LL3560;} else{ goto _LL3561;} _LL3561: if(*(( int*)
_temp3545) == Cyc_Absyn_NoInstantiate_e){ _LL3612: _temp3611=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp3545)->f1; goto _LL3562;} else{ goto _LL3563;} _LL3563: if(*(( int*)
_temp3545) == Cyc_Absyn_Instantiate_e){ _LL3616: _temp3615=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3545)->f1; goto _LL3614; _LL3614: _temp3613=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3545)->f2; goto _LL3564;} else{ goto _LL3565;} _LL3565: if(*(( int*)
_temp3545) == Cyc_Absyn_Cast_e){ _LL3620: _temp3619=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp3545)->f1; goto _LL3618; _LL3618: _temp3617=(( struct Cyc_Absyn_Cast_e_struct*)
_temp3545)->f2; goto _LL3566;} else{ goto _LL3567;} _LL3567: if(*(( int*)
_temp3545) == Cyc_Absyn_Address_e){ _LL3622: _temp3621=(( struct Cyc_Absyn_Address_e_struct*)
_temp3545)->f1; goto _LL3568;} else{ goto _LL3569;} _LL3569: if(*(( int*)
_temp3545) == Cyc_Absyn_Comprehension_e){ _LL3628: _temp3627=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3545)->f1; goto _LL3626; _LL3626: _temp3625=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3545)->f2; goto _LL3624; _LL3624: _temp3623=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3545)->f3; goto _LL3570;} else{ goto _LL3571;} _LL3571: if(*(( int*)
_temp3545) == Cyc_Absyn_Array_e){ _LL3630: _temp3629=(( struct Cyc_Absyn_Array_e_struct*)
_temp3545)->f1; goto _LL3572;} else{ goto _LL3573;} _LL3573: if(*(( int*)
_temp3545) == Cyc_Absyn_AnonStruct_e){ _LL3634: _temp3633=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3545)->f1; goto _LL3632; _LL3632: _temp3631=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3545)->f2; goto _LL3574;} else{ goto _LL3575;} _LL3575: if(*(( int*)
_temp3545) == Cyc_Absyn_Struct_e){ _LL3642: _temp3641=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3545)->f1; goto _LL3640; _LL3640: _temp3639=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3545)->f2; goto _LL3638; _LL3638: _temp3637=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3545)->f3; goto _LL3636; _LL3636: _temp3635=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3545)->f4; goto _LL3576;} else{ goto _LL3577;} _LL3577: if(*(( int*)
_temp3545) == Cyc_Absyn_Primop_e){ _LL3646: _temp3645=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp3545)->f1; goto _LL3644; _LL3644: _temp3643=(( struct Cyc_Absyn_Primop_e_struct*)
_temp3545)->f2; goto _LL3578;} else{ goto _LL3579;} _LL3579: if(*(( int*)
_temp3545) == Cyc_Absyn_Tuple_e){ _LL3648: _temp3647=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp3545)->f1; goto _LL3580;} else{ goto _LL3581;} _LL3581: if(*(( int*)
_temp3545) == Cyc_Absyn_Tunion_e){ _LL3658: _temp3657=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3545)->f1; goto _LL3656; _LL3656: _temp3655=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3545)->f2; goto _LL3654; _LL3654: _temp3653=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3545)->f3; goto _LL3652; _LL3652: _temp3651=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3545)->f4; goto _LL3650; _LL3650: _temp3649=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3545)->f5; goto _LL3582;} else{ goto _LL3583;} _LL3583: goto _LL3584;
_LL3548: return 1; _LL3550: return 1; _LL3552: return Cyc_Tcutil_cnst_exp( te, 1,
_temp3589); _LL3554: return 1; _LL3556: { struct _handler_cons _temp3659;
_push_handler(& _temp3659);{ int _temp3661= 0; if( setjmp( _temp3659.handler)){
_temp3661= 1;} if( ! _temp3661){{ void* _temp3662= Cyc_Tcenv_lookup_ordinary( te,
e->loc, _temp3599); void* _temp3670; struct Cyc_Absyn_Fndecl* _temp3672; void*
_temp3674; struct Cyc_Absyn_Vardecl* _temp3676; _LL3664: if(*(( int*) _temp3662)
== Cyc_Tcenv_VarRes){ _LL3671: _temp3670=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3662)->f1; if(( unsigned int) _temp3670 > 1u?*(( int*) _temp3670) == Cyc_Absyn_Funname_b:
0){ _LL3673: _temp3672=(( struct Cyc_Absyn_Funname_b_struct*) _temp3670)->f1;
goto _LL3665;} else{ goto _LL3666;}} else{ goto _LL3666;} _LL3666: if(*(( int*)
_temp3662) == Cyc_Tcenv_VarRes){ _LL3675: _temp3674=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3662)->f1; if(( unsigned int) _temp3674 > 1u?*(( int*) _temp3674) == Cyc_Absyn_Global_b:
0){ _LL3677: _temp3676=(( struct Cyc_Absyn_Global_b_struct*) _temp3674)->f1;
goto _LL3667;} else{ goto _LL3668;}} else{ goto _LL3668;} _LL3668: goto _LL3669;
_LL3665: { int _temp3678= 1; _npop_handler( 0u); return _temp3678;} _LL3667: {
void* _temp3679= Cyc_Tcutil_compress(( void*) _temp3676->type); struct Cyc_Absyn_Exp*
_temp3685; struct Cyc_Absyn_Tqual _temp3687; void* _temp3689; _LL3681: if((
unsigned int) _temp3679 > 4u?*(( int*) _temp3679) == Cyc_Absyn_ArrayType: 0){
_LL3690: _temp3689=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3679)->f1;
goto _LL3688; _LL3688: _temp3687=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3679)->f2; goto _LL3686; _LL3686: _temp3685=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3679)->f3; goto _LL3682;} else{ goto _LL3683;} _LL3683: goto _LL3684;
_LL3682: { int _temp3691= 1; _npop_handler( 0u); return _temp3691;} _LL3684: {
int _temp3692= var_okay; _npop_handler( 0u); return _temp3692;} _LL3680:;}
_LL3669: { int _temp3693= var_okay; _npop_handler( 0u); return _temp3693;}
_LL3663:;}; _pop_handler();} else{ void* _temp3660=( void*) _exn_thrown; void*
_temp3695= _temp3660; _LL3697: if( _temp3695 == Cyc_Dict_Absent){ goto _LL3698;}
else{ goto _LL3699;} _LL3699: goto _LL3700; _LL3698: return 0; _LL3700:( void)
_throw( _temp3695); _LL3696:;}}} _LL3558: return( Cyc_Tcutil_cnst_exp( te, 0,
_temp3605)? Cyc_Tcutil_cnst_exp( te, 0, _temp3603): 0)? Cyc_Tcutil_cnst_exp( te,
0, _temp3601): 0; _LL3560: return Cyc_Tcutil_cnst_exp( te, 0, _temp3609)? Cyc_Tcutil_cnst_exp(
te, 0, _temp3607): 0; _LL3562: _temp3615= _temp3611; goto _LL3564; _LL3564:
return Cyc_Tcutil_cnst_exp( te, var_okay, _temp3615); _LL3566: return Cyc_Tcutil_cnst_exp(
te, var_okay, _temp3617); _LL3568: return Cyc_Tcutil_cnst_exp( te, 1, _temp3621);
_LL3570: return Cyc_Tcutil_cnst_exp( te, 0, _temp3625)? Cyc_Tcutil_cnst_exp( te,
0, _temp3623): 0; _LL3572: _temp3631= _temp3629; goto _LL3574; _LL3574:
_temp3637= _temp3631; goto _LL3576; _LL3576: for( 0; _temp3637 != 0; _temp3637=((
struct Cyc_List_List*) _check_null( _temp3637))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,(*(( struct _tuple20*)(( struct Cyc_List_List*) _check_null( _temp3637))->hd)).f2)){
return 0;}} return 1; _LL3578: _temp3647= _temp3643; goto _LL3580; _LL3580:
_temp3653= _temp3647; goto _LL3582; _LL3582: for( 0; _temp3653 != 0; _temp3653=((
struct Cyc_List_List*) _check_null( _temp3653))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp3653))->hd)){
return 0;}} return 1; _LL3584: return 0; _LL3546:;} int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcutil_cnst_exp(
te, 0, e);} static int Cyc_Tcutil_fields_support_default( struct Cyc_List_List*
tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs); int Cyc_Tcutil_supports_default(
void* t){ void* _temp3701= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt*
_temp3739; int _temp3741; struct Cyc_Core_Opt* _temp3743; struct Cyc_Core_Opt*
_temp3745; struct Cyc_Absyn_Tvar* _temp3747; struct Cyc_Absyn_TunionInfo
_temp3749; struct Cyc_Absyn_TunionFieldInfo _temp3751; struct Cyc_Absyn_PtrInfo
_temp3753; struct Cyc_Absyn_Conref* _temp3755; struct Cyc_Absyn_Tqual _temp3757;
struct Cyc_Absyn_Conref* _temp3759; void* _temp3761; void* _temp3763; void*
_temp3765; void* _temp3767; struct Cyc_Absyn_Exp* _temp3769; struct Cyc_Absyn_Tqual
_temp3771; void* _temp3773; struct Cyc_Absyn_FnInfo _temp3775; struct Cyc_List_List*
_temp3777; struct Cyc_Absyn_Structdecl** _temp3779; struct Cyc_List_List*
_temp3781; struct _tuple1* _temp3783; struct Cyc_Absyn_Uniondecl** _temp3785;
struct Cyc_List_List* _temp3787; struct _tuple1* _temp3789; struct Cyc_List_List*
_temp3791; struct Cyc_List_List* _temp3793; struct Cyc_Absyn_Enumdecl* _temp3795;
struct _tuple1* _temp3797; _LL3703: if( _temp3701 ==( void*) Cyc_Absyn_VoidType){
goto _LL3704;} else{ goto _LL3705;} _LL3705: if(( unsigned int) _temp3701 > 4u?*((
int*) _temp3701) == Cyc_Absyn_Evar: 0){ _LL3746: _temp3745=(( struct Cyc_Absyn_Evar_struct*)
_temp3701)->f1; goto _LL3744; _LL3744: _temp3743=(( struct Cyc_Absyn_Evar_struct*)
_temp3701)->f2; goto _LL3742; _LL3742: _temp3741=(( struct Cyc_Absyn_Evar_struct*)
_temp3701)->f3; goto _LL3740; _LL3740: _temp3739=(( struct Cyc_Absyn_Evar_struct*)
_temp3701)->f4; goto _LL3706;} else{ goto _LL3707;} _LL3707: if(( unsigned int)
_temp3701 > 4u?*(( int*) _temp3701) == Cyc_Absyn_VarType: 0){ _LL3748: _temp3747=((
struct Cyc_Absyn_VarType_struct*) _temp3701)->f1; goto _LL3708;} else{ goto
_LL3709;} _LL3709: if(( unsigned int) _temp3701 > 4u?*(( int*) _temp3701) == Cyc_Absyn_TunionType:
0){ _LL3750: _temp3749=(( struct Cyc_Absyn_TunionType_struct*) _temp3701)->f1;
goto _LL3710;} else{ goto _LL3711;} _LL3711: if(( unsigned int) _temp3701 > 4u?*((
int*) _temp3701) == Cyc_Absyn_TunionFieldType: 0){ _LL3752: _temp3751=(( struct
Cyc_Absyn_TunionFieldType_struct*) _temp3701)->f1; goto _LL3712;} else{ goto
_LL3713;} _LL3713: if(( unsigned int) _temp3701 > 4u?*(( int*) _temp3701) == Cyc_Absyn_PointerType:
0){ _LL3754: _temp3753=(( struct Cyc_Absyn_PointerType_struct*) _temp3701)->f1;
_LL3764: _temp3763=( void*) _temp3753.elt_typ; goto _LL3762; _LL3762: _temp3761=(
void*) _temp3753.rgn_typ; goto _LL3760; _LL3760: _temp3759= _temp3753.nullable;
goto _LL3758; _LL3758: _temp3757= _temp3753.tq; goto _LL3756; _LL3756: _temp3755=
_temp3753.bounds; goto _LL3714;} else{ goto _LL3715;} _LL3715: if(( unsigned int)
_temp3701 > 4u?*(( int*) _temp3701) == Cyc_Absyn_IntType: 0){ _LL3768: _temp3767=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp3701)->f1; goto _LL3766; _LL3766:
_temp3765=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3701)->f2; goto
_LL3716;} else{ goto _LL3717;} _LL3717: if( _temp3701 ==( void*) Cyc_Absyn_FloatType){
goto _LL3718;} else{ goto _LL3719;} _LL3719: if( _temp3701 ==( void*) Cyc_Absyn_DoubleType){
goto _LL3720;} else{ goto _LL3721;} _LL3721: if(( unsigned int) _temp3701 > 4u?*((
int*) _temp3701) == Cyc_Absyn_ArrayType: 0){ _LL3774: _temp3773=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3701)->f1; goto _LL3772; _LL3772:
_temp3771=(( struct Cyc_Absyn_ArrayType_struct*) _temp3701)->f2; goto _LL3770;
_LL3770: _temp3769=(( struct Cyc_Absyn_ArrayType_struct*) _temp3701)->f3; goto
_LL3722;} else{ goto _LL3723;} _LL3723: if(( unsigned int) _temp3701 > 4u?*((
int*) _temp3701) == Cyc_Absyn_FnType: 0){ _LL3776: _temp3775=(( struct Cyc_Absyn_FnType_struct*)
_temp3701)->f1; goto _LL3724;} else{ goto _LL3725;} _LL3725: if(( unsigned int)
_temp3701 > 4u?*(( int*) _temp3701) == Cyc_Absyn_TupleType: 0){ _LL3778:
_temp3777=(( struct Cyc_Absyn_TupleType_struct*) _temp3701)->f1; goto _LL3726;}
else{ goto _LL3727;} _LL3727: if(( unsigned int) _temp3701 > 4u?*(( int*)
_temp3701) == Cyc_Absyn_StructType: 0){ _LL3784: _temp3783=(( struct Cyc_Absyn_StructType_struct*)
_temp3701)->f1; goto _LL3782; _LL3782: _temp3781=(( struct Cyc_Absyn_StructType_struct*)
_temp3701)->f2; goto _LL3780; _LL3780: _temp3779=(( struct Cyc_Absyn_StructType_struct*)
_temp3701)->f3; goto _LL3728;} else{ goto _LL3729;} _LL3729: if(( unsigned int)
_temp3701 > 4u?*(( int*) _temp3701) == Cyc_Absyn_UnionType: 0){ _LL3790:
_temp3789=(( struct Cyc_Absyn_UnionType_struct*) _temp3701)->f1; goto _LL3788;
_LL3788: _temp3787=(( struct Cyc_Absyn_UnionType_struct*) _temp3701)->f2; goto
_LL3786; _LL3786: _temp3785=(( struct Cyc_Absyn_UnionType_struct*) _temp3701)->f3;
goto _LL3730;} else{ goto _LL3731;} _LL3731: if(( unsigned int) _temp3701 > 4u?*((
int*) _temp3701) == Cyc_Absyn_AnonStructType: 0){ _LL3792: _temp3791=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp3701)->f1; goto _LL3732;} else{ goto
_LL3733;} _LL3733: if(( unsigned int) _temp3701 > 4u?*(( int*) _temp3701) == Cyc_Absyn_AnonUnionType:
0){ _LL3794: _temp3793=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3701)->f1;
goto _LL3734;} else{ goto _LL3735;} _LL3735: if(( unsigned int) _temp3701 > 4u?*((
int*) _temp3701) == Cyc_Absyn_EnumType: 0){ _LL3798: _temp3797=(( struct Cyc_Absyn_EnumType_struct*)
_temp3701)->f1; goto _LL3796; _LL3796: _temp3795=(( struct Cyc_Absyn_EnumType_struct*)
_temp3701)->f2; goto _LL3736;} else{ goto _LL3737;} _LL3737: goto _LL3738;
_LL3704: return 1; _LL3706: return 0; _LL3708: return 0; _LL3710: return 0;
_LL3712: return 0; _LL3714: { void* _temp3799=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3755))->v; void*
_temp3807; void* _temp3809; struct Cyc_Absyn_Exp* _temp3811; _LL3801: if((
unsigned int) _temp3799 > 1u?*(( int*) _temp3799) == Cyc_Absyn_Eq_constr: 0){
_LL3808: _temp3807=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3799)->f1;
if( _temp3807 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3802;} else{ goto _LL3803;}}
else{ goto _LL3803;} _LL3803: if(( unsigned int) _temp3799 > 1u?*(( int*)
_temp3799) == Cyc_Absyn_Eq_constr: 0){ _LL3810: _temp3809=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3799)->f1; if(( unsigned int) _temp3809 > 1u?*(( int*) _temp3809) == Cyc_Absyn_Upper_b:
0){ _LL3812: _temp3811=(( struct Cyc_Absyn_Upper_b_struct*) _temp3809)->f1; goto
_LL3804;} else{ goto _LL3805;}} else{ goto _LL3805;} _LL3805: goto _LL3806;
_LL3802: return 1; _LL3804: { void* _temp3813=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3759))->v; int
_temp3819; _LL3815: if(( unsigned int) _temp3813 > 1u?*(( int*) _temp3813) ==
Cyc_Absyn_Eq_constr: 0){ _LL3820: _temp3819=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3813)->f1; goto _LL3816;} else{ goto _LL3817;} _LL3817: goto _LL3818;
_LL3816: return _temp3819; _LL3818: return 0; _LL3814:;} _LL3806: return 0;
_LL3800:;} _LL3716: return 1; _LL3718: return 1; _LL3720: return 1; _LL3722:
return Cyc_Tcutil_supports_default( _temp3773); _LL3724: return 0; _LL3726: for(
0; _temp3777 != 0; _temp3777=(( struct Cyc_List_List*) _check_null( _temp3777))->tl){
if( ! Cyc_Tcutil_supports_default((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp3777))->hd)).f2)){ return 0;}} return 1; _LL3728: if(
_temp3779 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp3779)); if( sd->fields == 0){ return 0;} return Cyc_Tcutil_fields_support_default(
sd->tvs, _temp3781,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
sd->fields))->v);} _LL3730: if( _temp3785 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp3785)); if( ud->fields ==
0){ return 0;} return Cyc_Tcutil_fields_support_default( ud->tvs, _temp3787,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ud->fields))->v);}
_LL3732: _temp3793= _temp3791; goto _LL3734; _LL3734: return Cyc_Tcutil_fields_support_default(
0, 0, _temp3793); _LL3736: return 1; _LL3738: return 0; _LL3702:;} static int
Cyc_Tcutil_fields_support_default( struct Cyc_List_List* tvs, struct Cyc_List_List*
ts, struct Cyc_List_List* fs){{ struct _RegionHandle _temp3821= _new_region();
struct _RegionHandle* rgn=& _temp3821; _push_region( rgn);{ struct Cyc_List_List*
_temp3822=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, tvs, ts); goto _LL3823; _LL3823: for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ void* t= Cyc_Tcutil_rsubstitute( rgn, _temp3822,( void*)((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( fs))->hd)->type);
if( ! Cyc_Tcutil_supports_default( t)){ int _temp3824= 0; _npop_handler( 0u);
return _temp3824;}}}; _pop_region(& _temp3821);} return 1;}