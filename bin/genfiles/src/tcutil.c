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
_LL11:;} struct _tuple8{ void* f1; void* f2; } ; static int Cyc_Tcutil_kind_leq(
void* k1, void* k2){ if( k1 == k2){ return 1;}{ struct _tuple8 _temp55=({ struct
_tuple8 _temp54; _temp54.f1= k1; _temp54.f2= k2; _temp54;}); void* _temp65; void*
_temp67; void* _temp69; void* _temp71; void* _temp73; void* _temp75; _LL57:
_LL68: _temp67= _temp55.f1; if( _temp67 ==( void*) Cyc_Absyn_BoxKind){ goto
_LL66;} else{ goto _LL59;} _LL66: _temp65= _temp55.f2; if( _temp65 ==( void*)
Cyc_Absyn_MemKind){ goto _LL58;} else{ goto _LL59;} _LL59: _LL72: _temp71=
_temp55.f1; if( _temp71 ==( void*) Cyc_Absyn_BoxKind){ goto _LL70;} else{ goto
_LL61;} _LL70: _temp69= _temp55.f2; if( _temp69 ==( void*) Cyc_Absyn_AnyKind){
goto _LL60;} else{ goto _LL61;} _LL61: _LL76: _temp75= _temp55.f1; if( _temp75
==( void*) Cyc_Absyn_MemKind){ goto _LL74;} else{ goto _LL63;} _LL74: _temp73=
_temp55.f2; if( _temp73 ==( void*) Cyc_Absyn_AnyKind){ goto _LL62;} else{ goto
_LL63;} _LL63: goto _LL64; _LL58: return 1; _LL60: return 1; _LL62: return 1;
_LL64: return 0; _LL56:;}} void* Cyc_Tcutil_typ_kind( void* t){ void* _temp77=
Cyc_Tcutil_compress( t); struct Cyc_Core_Opt* _temp129; int _temp131; struct Cyc_Core_Opt*
_temp133; struct Cyc_Core_Opt* _temp135; struct Cyc_Absyn_Tvar* _temp137; void*
_temp139; void* _temp141; struct Cyc_Absyn_FnInfo _temp143; void* _temp145;
struct Cyc_Absyn_TunionInfo _temp147; struct Cyc_Absyn_TunionFieldInfo _temp149;
struct Cyc_List_List* _temp151; void* _temp153; struct Cyc_Absyn_Tunionfield*
_temp155; struct Cyc_Absyn_Tuniondecl* _temp157; struct Cyc_Absyn_TunionFieldInfo
_temp159; struct Cyc_List_List* _temp161; void* _temp163; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp165; struct Cyc_Absyn_Structdecl** _temp167; struct Cyc_List_List* _temp169;
struct _tuple1* _temp171; struct Cyc_Absyn_Uniondecl** _temp173; struct Cyc_List_List*
_temp175; struct _tuple1* _temp177; struct Cyc_Absyn_Structdecl** _temp179;
struct Cyc_List_List* _temp181; struct _tuple1* _temp183; struct Cyc_Absyn_Uniondecl**
_temp185; struct Cyc_List_List* _temp187; struct _tuple1* _temp189; struct Cyc_List_List*
_temp191; struct Cyc_List_List* _temp193; struct Cyc_Absyn_Enumdecl* _temp195;
struct _tuple1* _temp197; struct Cyc_Absyn_PtrInfo _temp199; struct Cyc_Absyn_Exp*
_temp201; struct Cyc_Absyn_Tqual _temp203; void* _temp205; struct Cyc_List_List*
_temp207; struct Cyc_Core_Opt* _temp209; struct Cyc_List_List* _temp211; struct
_tuple1* _temp213; void* _temp215; struct Cyc_List_List* _temp217; _LL79: if((
unsigned int) _temp77 > 4u?*(( int*) _temp77) == Cyc_Absyn_Evar: 0){ _LL136:
_temp135=(( struct Cyc_Absyn_Evar_struct*) _temp77)->f1; goto _LL134; _LL134:
_temp133=(( struct Cyc_Absyn_Evar_struct*) _temp77)->f2; goto _LL132; _LL132:
_temp131=(( struct Cyc_Absyn_Evar_struct*) _temp77)->f3; goto _LL130; _LL130:
_temp129=(( struct Cyc_Absyn_Evar_struct*) _temp77)->f4; goto _LL80;} else{ goto
_LL81;} _LL81: if(( unsigned int) _temp77 > 4u?*(( int*) _temp77) == Cyc_Absyn_VarType:
0){ _LL138: _temp137=(( struct Cyc_Absyn_VarType_struct*) _temp77)->f1; goto
_LL82;} else{ goto _LL83;} _LL83: if( _temp77 ==( void*) Cyc_Absyn_VoidType){
goto _LL84;} else{ goto _LL85;} _LL85: if(( unsigned int) _temp77 > 4u?*(( int*)
_temp77) == Cyc_Absyn_IntType: 0){ _LL142: _temp141=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp77)->f1; goto _LL140; _LL140: _temp139=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp77)->f2; goto _LL86;} else{ goto _LL87;} _LL87: if( _temp77 ==( void*) Cyc_Absyn_FloatType){
goto _LL88;} else{ goto _LL89;} _LL89: if( _temp77 ==( void*) Cyc_Absyn_DoubleType){
goto _LL90;} else{ goto _LL91;} _LL91: if(( unsigned int) _temp77 > 4u?*(( int*)
_temp77) == Cyc_Absyn_FnType: 0){ _LL144: _temp143=(( struct Cyc_Absyn_FnType_struct*)
_temp77)->f1; goto _LL92;} else{ goto _LL93;} _LL93: if(( unsigned int) _temp77
> 4u?*(( int*) _temp77) == Cyc_Absyn_RgnHandleType: 0){ _LL146: _temp145=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp77)->f1; goto _LL94;} else{ goto
_LL95;} _LL95: if( _temp77 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL96;} else{
goto _LL97;} _LL97: if(( unsigned int) _temp77 > 4u?*(( int*) _temp77) == Cyc_Absyn_TunionType:
0){ _LL148: _temp147=(( struct Cyc_Absyn_TunionType_struct*) _temp77)->f1; goto
_LL98;} else{ goto _LL99;} _LL99: if(( unsigned int) _temp77 > 4u?*(( int*)
_temp77) == Cyc_Absyn_TunionFieldType: 0){ _LL150: _temp149=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp77)->f1; _LL154: _temp153=( void*) _temp149.field_info; if(*(( int*)
_temp153) == Cyc_Absyn_KnownTunionfield){ _LL158: _temp157=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp153)->f1; goto _LL156; _LL156: _temp155=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp153)->f2; goto _LL152;} else{ goto _LL101;} _LL152: _temp151= _temp149.targs;
goto _LL100;} else{ goto _LL101;} _LL101: if(( unsigned int) _temp77 > 4u?*((
int*) _temp77) == Cyc_Absyn_TunionFieldType: 0){ _LL160: _temp159=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp77)->f1; _LL164: _temp163=( void*) _temp159.field_info; if(*(( int*)
_temp163) == Cyc_Absyn_UnknownTunionfield){ _LL166: _temp165=(( struct Cyc_Absyn_UnknownTunionfield_struct*)
_temp163)->f1; goto _LL162;} else{ goto _LL103;} _LL162: _temp161= _temp159.targs;
goto _LL102;} else{ goto _LL103;} _LL103: if(( unsigned int) _temp77 > 4u?*((
int*) _temp77) == Cyc_Absyn_StructType: 0){ _LL172: _temp171=(( struct Cyc_Absyn_StructType_struct*)
_temp77)->f1; goto _LL170; _LL170: _temp169=(( struct Cyc_Absyn_StructType_struct*)
_temp77)->f2; goto _LL168; _LL168: _temp167=(( struct Cyc_Absyn_StructType_struct*)
_temp77)->f3; if( _temp167 == 0){ goto _LL104;} else{ goto _LL105;}} else{ goto
_LL105;} _LL105: if(( unsigned int) _temp77 > 4u?*(( int*) _temp77) == Cyc_Absyn_UnionType:
0){ _LL178: _temp177=(( struct Cyc_Absyn_UnionType_struct*) _temp77)->f1; goto
_LL176; _LL176: _temp175=(( struct Cyc_Absyn_UnionType_struct*) _temp77)->f2;
goto _LL174; _LL174: _temp173=(( struct Cyc_Absyn_UnionType_struct*) _temp77)->f3;
if( _temp173 == 0){ goto _LL106;} else{ goto _LL107;}} else{ goto _LL107;}
_LL107: if(( unsigned int) _temp77 > 4u?*(( int*) _temp77) == Cyc_Absyn_StructType:
0){ _LL184: _temp183=(( struct Cyc_Absyn_StructType_struct*) _temp77)->f1; goto
_LL182; _LL182: _temp181=(( struct Cyc_Absyn_StructType_struct*) _temp77)->f2;
goto _LL180; _LL180: _temp179=(( struct Cyc_Absyn_StructType_struct*) _temp77)->f3;
goto _LL108;} else{ goto _LL109;} _LL109: if(( unsigned int) _temp77 > 4u?*((
int*) _temp77) == Cyc_Absyn_UnionType: 0){ _LL190: _temp189=(( struct Cyc_Absyn_UnionType_struct*)
_temp77)->f1; goto _LL188; _LL188: _temp187=(( struct Cyc_Absyn_UnionType_struct*)
_temp77)->f2; goto _LL186; _LL186: _temp185=(( struct Cyc_Absyn_UnionType_struct*)
_temp77)->f3; goto _LL110;} else{ goto _LL111;} _LL111: if(( unsigned int)
_temp77 > 4u?*(( int*) _temp77) == Cyc_Absyn_AnonStructType: 0){ _LL192:
_temp191=(( struct Cyc_Absyn_AnonStructType_struct*) _temp77)->f1; goto _LL112;}
else{ goto _LL113;} _LL113: if(( unsigned int) _temp77 > 4u?*(( int*) _temp77)
== Cyc_Absyn_AnonUnionType: 0){ _LL194: _temp193=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp77)->f1; goto _LL114;} else{ goto _LL115;} _LL115: if(( unsigned int)
_temp77 > 4u?*(( int*) _temp77) == Cyc_Absyn_EnumType: 0){ _LL198: _temp197=((
struct Cyc_Absyn_EnumType_struct*) _temp77)->f1; goto _LL196; _LL196: _temp195=((
struct Cyc_Absyn_EnumType_struct*) _temp77)->f2; goto _LL116;} else{ goto _LL117;}
_LL117: if(( unsigned int) _temp77 > 4u?*(( int*) _temp77) == Cyc_Absyn_PointerType:
0){ _LL200: _temp199=(( struct Cyc_Absyn_PointerType_struct*) _temp77)->f1; goto
_LL118;} else{ goto _LL119;} _LL119: if(( unsigned int) _temp77 > 4u?*(( int*)
_temp77) == Cyc_Absyn_ArrayType: 0){ _LL206: _temp205=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp77)->f1; goto _LL204; _LL204: _temp203=(( struct Cyc_Absyn_ArrayType_struct*)
_temp77)->f2; goto _LL202; _LL202: _temp201=(( struct Cyc_Absyn_ArrayType_struct*)
_temp77)->f3; goto _LL120;} else{ goto _LL121;} _LL121: if(( unsigned int)
_temp77 > 4u?*(( int*) _temp77) == Cyc_Absyn_TupleType: 0){ _LL208: _temp207=((
struct Cyc_Absyn_TupleType_struct*) _temp77)->f1; goto _LL122;} else{ goto
_LL123;} _LL123: if(( unsigned int) _temp77 > 4u?*(( int*) _temp77) == Cyc_Absyn_TypedefType:
0){ _LL214: _temp213=(( struct Cyc_Absyn_TypedefType_struct*) _temp77)->f1; goto
_LL212; _LL212: _temp211=(( struct Cyc_Absyn_TypedefType_struct*) _temp77)->f2;
goto _LL210; _LL210: _temp209=(( struct Cyc_Absyn_TypedefType_struct*) _temp77)->f3;
goto _LL124;} else{ goto _LL125;} _LL125: if(( unsigned int) _temp77 > 4u?*((
int*) _temp77) == Cyc_Absyn_AccessEff: 0){ _LL216: _temp215=( void*)(( struct
Cyc_Absyn_AccessEff_struct*) _temp77)->f1; goto _LL126;} else{ goto _LL127;}
_LL127: if(( unsigned int) _temp77 > 4u?*(( int*) _temp77) == Cyc_Absyn_JoinEff:
0){ _LL218: _temp217=(( struct Cyc_Absyn_JoinEff_struct*) _temp77)->f1; goto
_LL128;} else{ goto _LL78;} _LL80: return( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp135))->v; _LL82: return(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( _temp137->kind); _LL84: return( void*) Cyc_Absyn_MemKind;
_LL86: return _temp139 ==( void*) Cyc_Absyn_B4?( void*) Cyc_Absyn_BoxKind:( void*)
Cyc_Absyn_MemKind; _LL88: return( void*) Cyc_Absyn_MemKind; _LL90: return( void*)
Cyc_Absyn_MemKind; _LL92: return( void*) Cyc_Absyn_MemKind; _LL94: return( void*)
Cyc_Absyn_BoxKind; _LL96: return( void*) Cyc_Absyn_RgnKind; _LL98: return( void*)
Cyc_Absyn_BoxKind; _LL100: if( _temp155->typs == 0){ return( void*) Cyc_Absyn_BoxKind;}
else{ return( void*) Cyc_Absyn_MemKind;} _LL102: return(( void*(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp219=(
unsigned char*)"typ_kind: Unresolved TunionFieldType"; struct _tagged_string
_temp220; _temp220.curr= _temp219; _temp220.base= _temp219; _temp220.last_plus_one=
_temp219 + 37; _temp220;})); _LL104: return( void*) Cyc_Absyn_AnyKind; _LL106:
return( void*) Cyc_Absyn_AnyKind; _LL108: return( void*) Cyc_Absyn_MemKind;
_LL110: return( void*) Cyc_Absyn_MemKind; _LL112: return( void*) Cyc_Absyn_MemKind;
_LL114: return( void*) Cyc_Absyn_MemKind; _LL116: return( void*) Cyc_Absyn_BoxKind;
_LL118: { void* _temp221=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp199.bounds))->v; void* _temp231; void*
_temp233; struct Cyc_Absyn_Exp* _temp235; struct Cyc_Absyn_Conref* _temp237;
_LL223: if(( unsigned int) _temp221 > 1u?*(( int*) _temp221) == Cyc_Absyn_Eq_constr:
0){ _LL232: _temp231=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp221)->f1;
if( _temp231 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL224;} else{ goto _LL225;}}
else{ goto _LL225;} _LL225: if(( unsigned int) _temp221 > 1u?*(( int*) _temp221)
== Cyc_Absyn_Eq_constr: 0){ _LL234: _temp233=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp221)->f1; if(( unsigned int) _temp233 > 1u?*(( int*) _temp233) == Cyc_Absyn_Upper_b:
0){ _LL236: _temp235=(( struct Cyc_Absyn_Upper_b_struct*) _temp233)->f1; goto
_LL226;} else{ goto _LL227;}} else{ goto _LL227;} _LL227: if( _temp221 ==( void*)
Cyc_Absyn_No_constr){ goto _LL228;} else{ goto _LL229;} _LL229: if((
unsigned int) _temp221 > 1u?*(( int*) _temp221) == Cyc_Absyn_Forward_constr: 0){
_LL238: _temp237=(( struct Cyc_Absyn_Forward_constr_struct*) _temp221)->f1; goto
_LL230;} else{ goto _LL222;} _LL224: return( void*) Cyc_Absyn_MemKind; _LL226:
return( void*) Cyc_Absyn_BoxKind; _LL228: return( void*) Cyc_Absyn_MemKind;
_LL230: return(( void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
unsigned char* _temp239=( unsigned char*)"typ_kind: forward constraint in ptr bounds";
struct _tagged_string _temp240; _temp240.curr= _temp239; _temp240.base= _temp239;
_temp240.last_plus_one= _temp239 + 43; _temp240;})); _LL222:;} _LL120: return(
void*) Cyc_Absyn_MemKind; _LL122: return( void*) Cyc_Absyn_MemKind; _LL124:
return(( void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ struct _tagged_string _temp241= Cyc_Absynpp_typ2string( t);
xprintf("typ_kind: typedef found: %.*s", _temp241.last_plus_one - _temp241.curr,
_temp241.curr);})); _LL126: return( void*) Cyc_Absyn_EffKind; _LL128: return(
void*) Cyc_Absyn_EffKind; _LL78:;} unsigned char Cyc_Tcutil_Unify[ 10u]="\000\000\000\000Unify";
int Cyc_Tcutil_unify( void* t1, void* t2){ struct _handler_cons _temp242;
_push_handler(& _temp242);{ int _temp244= 0; if( setjmp( _temp242.handler)){
_temp244= 1;} if( ! _temp244){ Cyc_Tcutil_unify_it( t1, t2);{ int _temp245= 1;
_npop_handler( 0u); return _temp245;}; _pop_handler();} else{ void* _temp243=(
void*) _exn_thrown; void* _temp247= _temp243; _LL249: if( _temp247 == Cyc_Tcutil_Unify){
goto _LL250;} else{ goto _LL251;} _LL251: goto _LL252; _LL250: return 0; _LL252:(
void) _throw( _temp247); _LL248:;}}} static void Cyc_Tcutil_occurslist( void*
evar, struct Cyc_List_List* env, struct Cyc_List_List* ts); static void Cyc_Tcutil_occurs(
void* evar, struct Cyc_List_List* env, void* t){ void* _temp253= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Tvar* _temp287; struct Cyc_Core_Opt* _temp289; struct Cyc_Core_Opt**
_temp291; int _temp292; struct Cyc_Core_Opt* _temp294; struct Cyc_Core_Opt*
_temp296; struct Cyc_Absyn_PtrInfo _temp298; struct Cyc_Absyn_Exp* _temp300;
struct Cyc_Absyn_Tqual _temp302; void* _temp304; struct Cyc_Absyn_FnInfo
_temp306; struct Cyc_List_List* _temp308; struct Cyc_Absyn_VarargInfo* _temp310;
int _temp312; struct Cyc_List_List* _temp314; void* _temp316; struct Cyc_Core_Opt*
_temp318; struct Cyc_List_List* _temp320; struct Cyc_List_List* _temp322; void*
_temp324; struct Cyc_Absyn_TunionInfo _temp326; void* _temp328; struct Cyc_List_List*
_temp330; void* _temp332; struct Cyc_Core_Opt* _temp334; struct Cyc_List_List*
_temp336; struct _tuple1* _temp338; struct Cyc_Absyn_TunionFieldInfo _temp340;
struct Cyc_List_List* _temp342; void* _temp344; struct Cyc_Absyn_Structdecl**
_temp346; struct Cyc_List_List* _temp348; struct _tuple1* _temp350; struct Cyc_List_List*
_temp352; struct Cyc_List_List* _temp354; void* _temp356; struct Cyc_List_List*
_temp358; _LL255: if(( unsigned int) _temp253 > 4u?*(( int*) _temp253) == Cyc_Absyn_VarType:
0){ _LL288: _temp287=(( struct Cyc_Absyn_VarType_struct*) _temp253)->f1; goto
_LL256;} else{ goto _LL257;} _LL257: if(( unsigned int) _temp253 > 4u?*(( int*)
_temp253) == Cyc_Absyn_Evar: 0){ _LL297: _temp296=(( struct Cyc_Absyn_Evar_struct*)
_temp253)->f1; goto _LL295; _LL295: _temp294=(( struct Cyc_Absyn_Evar_struct*)
_temp253)->f2; goto _LL293; _LL293: _temp292=(( struct Cyc_Absyn_Evar_struct*)
_temp253)->f3; goto _LL290; _LL290: _temp289=(( struct Cyc_Absyn_Evar_struct*)
_temp253)->f4; _temp291=&(( struct Cyc_Absyn_Evar_struct*) _temp253)->f4; goto
_LL258;} else{ goto _LL259;} _LL259: if(( unsigned int) _temp253 > 4u?*(( int*)
_temp253) == Cyc_Absyn_PointerType: 0){ _LL299: _temp298=(( struct Cyc_Absyn_PointerType_struct*)
_temp253)->f1; goto _LL260;} else{ goto _LL261;} _LL261: if(( unsigned int)
_temp253 > 4u?*(( int*) _temp253) == Cyc_Absyn_ArrayType: 0){ _LL305: _temp304=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp253)->f1; goto _LL303; _LL303:
_temp302=(( struct Cyc_Absyn_ArrayType_struct*) _temp253)->f2; goto _LL301;
_LL301: _temp300=(( struct Cyc_Absyn_ArrayType_struct*) _temp253)->f3; goto
_LL262;} else{ goto _LL263;} _LL263: if(( unsigned int) _temp253 > 4u?*(( int*)
_temp253) == Cyc_Absyn_FnType: 0){ _LL307: _temp306=(( struct Cyc_Absyn_FnType_struct*)
_temp253)->f1; _LL321: _temp320= _temp306.tvars; goto _LL319; _LL319: _temp318=
_temp306.effect; goto _LL317; _LL317: _temp316=( void*) _temp306.ret_typ; goto
_LL315; _LL315: _temp314= _temp306.args; goto _LL313; _LL313: _temp312= _temp306.c_varargs;
goto _LL311; _LL311: _temp310= _temp306.cyc_varargs; goto _LL309; _LL309:
_temp308= _temp306.attributes; goto _LL264;} else{ goto _LL265;} _LL265: if((
unsigned int) _temp253 > 4u?*(( int*) _temp253) == Cyc_Absyn_TupleType: 0){
_LL323: _temp322=(( struct Cyc_Absyn_TupleType_struct*) _temp253)->f1; goto
_LL266;} else{ goto _LL267;} _LL267: if(( unsigned int) _temp253 > 4u?*(( int*)
_temp253) == Cyc_Absyn_RgnHandleType: 0){ _LL325: _temp324=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp253)->f1; goto _LL268;} else{ goto _LL269;} _LL269: if(( unsigned int)
_temp253 > 4u?*(( int*) _temp253) == Cyc_Absyn_TunionType: 0){ _LL327: _temp326=((
struct Cyc_Absyn_TunionType_struct*) _temp253)->f1; _LL333: _temp332=( void*)
_temp326.tunion_info; goto _LL331; _LL331: _temp330= _temp326.targs; goto _LL329;
_LL329: _temp328=( void*) _temp326.rgn; goto _LL270;} else{ goto _LL271;} _LL271:
if(( unsigned int) _temp253 > 4u?*(( int*) _temp253) == Cyc_Absyn_TypedefType: 0){
_LL339: _temp338=(( struct Cyc_Absyn_TypedefType_struct*) _temp253)->f1; goto
_LL337; _LL337: _temp336=(( struct Cyc_Absyn_TypedefType_struct*) _temp253)->f2;
goto _LL335; _LL335: _temp334=(( struct Cyc_Absyn_TypedefType_struct*) _temp253)->f3;
goto _LL272;} else{ goto _LL273;} _LL273: if(( unsigned int) _temp253 > 4u?*((
int*) _temp253) == Cyc_Absyn_TunionFieldType: 0){ _LL341: _temp340=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp253)->f1; _LL345: _temp344=( void*) _temp340.field_info; goto _LL343;
_LL343: _temp342= _temp340.targs; goto _LL274;} else{ goto _LL275;} _LL275: if((
unsigned int) _temp253 > 4u?*(( int*) _temp253) == Cyc_Absyn_StructType: 0){
_LL351: _temp350=(( struct Cyc_Absyn_StructType_struct*) _temp253)->f1; goto
_LL349; _LL349: _temp348=(( struct Cyc_Absyn_StructType_struct*) _temp253)->f2;
goto _LL347; _LL347: _temp346=(( struct Cyc_Absyn_StructType_struct*) _temp253)->f3;
goto _LL276;} else{ goto _LL277;} _LL277: if(( unsigned int) _temp253 > 4u?*((
int*) _temp253) == Cyc_Absyn_AnonStructType: 0){ _LL353: _temp352=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp253)->f1; goto _LL278;} else{ goto _LL279;} _LL279: if(( unsigned int)
_temp253 > 4u?*(( int*) _temp253) == Cyc_Absyn_AnonUnionType: 0){ _LL355:
_temp354=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp253)->f1; goto _LL280;}
else{ goto _LL281;} _LL281: if(( unsigned int) _temp253 > 4u?*(( int*) _temp253)
== Cyc_Absyn_AccessEff: 0){ _LL357: _temp356=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp253)->f1; goto _LL282;} else{ goto _LL283;} _LL283: if(( unsigned int)
_temp253 > 4u?*(( int*) _temp253) == Cyc_Absyn_JoinEff: 0){ _LL359: _temp358=((
struct Cyc_Absyn_JoinEff_struct*) _temp253)->f1; goto _LL284;} else{ goto _LL285;}
_LL285: goto _LL286; _LL256: if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, env, _temp287)){( void) _throw(( void*) Cyc_Tcutil_Unify);}
goto _LL254; _LL258: if( t == evar){( void) _throw(( void*) Cyc_Tcutil_Unify);}
else{ if( _temp294 != 0){ Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp294))->v);} else{ int problem= 0;{ struct Cyc_List_List* s=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(* _temp291))->v; for(
0; s != 0; s=(( struct Cyc_List_List*) _check_null( s))->tl){ if( !(( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( s))->hd)){ problem=
1; break;}}} if( problem){ struct Cyc_List_List* _temp360= 0; goto _LL361;
_LL361:{ struct Cyc_List_List* s=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null(* _temp291))->v; for( 0; s != 0; s=(( struct Cyc_List_List*)
_check_null( s))->tl){ if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( s))->hd)){ _temp360=({ struct Cyc_List_List* _temp362=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp362->hd=( void*)(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( s))->hd); _temp362->tl= _temp360; _temp362;});}}}*
_temp291=({ struct Cyc_Core_Opt* _temp363=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp363->v=( void*) _temp360; _temp363;});}}}
goto _LL254; _LL260: Cyc_Tcutil_occurs( evar, env,( void*) _temp298.elt_typ);
Cyc_Tcutil_occurs( evar, env,( void*) _temp298.rgn_typ); goto _LL254; _LL262:
Cyc_Tcutil_occurs( evar, env, _temp304); goto _LL254; _LL264: if( _temp318 != 0){
Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp318))->v);} Cyc_Tcutil_occurs( evar, env, _temp316); for( 0; _temp314 != 0;
_temp314=(( struct Cyc_List_List*) _check_null( _temp314))->tl){ Cyc_Tcutil_occurs(
evar, env,(*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp314))->hd)).f3);}
if( _temp310 != 0){ int _temp366; void* _temp368; void* _temp370; struct Cyc_Absyn_Tqual
_temp372; struct Cyc_Core_Opt* _temp374; struct Cyc_Absyn_VarargInfo _temp364=*((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp310)); _LL375: _temp374=
_temp364.name; goto _LL373; _LL373: _temp372= _temp364.tq; goto _LL371; _LL371:
_temp370=( void*) _temp364.type; goto _LL369; _LL369: _temp368=( void*) _temp364.rgn;
goto _LL367; _LL367: _temp366= _temp364.inject; goto _LL365; _LL365: Cyc_Tcutil_occurs(
evar, env, _temp370); Cyc_Tcutil_occurs( evar, env, _temp368);} goto _LL254;
_LL266: for( 0; _temp322 != 0; _temp322=(( struct Cyc_List_List*) _check_null(
_temp322))->tl){ Cyc_Tcutil_occurs( evar, env,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp322))->hd)).f2);} goto _LL254; _LL268: Cyc_Tcutil_occurs( evar,
env, _temp324); goto _LL254; _LL270: Cyc_Tcutil_occurs( evar, env, _temp328);
Cyc_Tcutil_occurslist( evar, env, _temp330); goto _LL254; _LL272: _temp342=
_temp336; goto _LL274; _LL274: _temp348= _temp342; goto _LL276; _LL276: Cyc_Tcutil_occurslist(
evar, env, _temp348); goto _LL254; _LL278: _temp354= _temp352; goto _LL280;
_LL280: for( 0; _temp354 != 0; _temp354=(( struct Cyc_List_List*) _check_null(
_temp354))->tl){ Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp354))->hd)->type);} goto _LL254; _LL282:
Cyc_Tcutil_occurs( evar, env, _temp356); goto _LL254; _LL284: Cyc_Tcutil_occurslist(
evar, env, _temp358); goto _LL254; _LL286: goto _LL254; _LL254:;} static void
Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List* env, struct Cyc_List_List*
ts){ for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){ Cyc_Tcutil_occurs(
evar, env,( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);}} static
void Cyc_Tcutil_unify_list( struct Cyc_List_List* t1, struct Cyc_List_List* t2){
struct _handler_cons _temp376; _push_handler(& _temp376);{ int _temp378= 0; if(
setjmp( _temp376.handler)){ _temp378= 1;} if( ! _temp378){(( void(*)( void(* f)(
void*, void*), struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_iter2)(
Cyc_Tcutil_unify_it, t1, t2);; _pop_handler();} else{ void* _temp377=( void*)
_exn_thrown; void* _temp380= _temp377; _LL382: if( _temp380 == Cyc_List_List_mismatch){
goto _LL383;} else{ goto _LL384;} _LL384: goto _LL385; _LL383:( void) _throw((
void*) Cyc_Tcutil_Unify); _LL385:( void) _throw( _temp380); _LL381:;}}} static
void Cyc_Tcutil_unify_tqual( struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual
tq2){ if(( tq1.q_const != tq2.q_const? 1: tq1.q_volatile != tq2.q_volatile)? 1:
tq1.q_restrict != tq2.q_restrict){( void) _throw(( void*) Cyc_Tcutil_Unify);}}
int Cyc_Tcutil_equal_tqual( struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual
tq2){ return( tq1.q_const == tq2.q_const? tq1.q_volatile == tq2.q_volatile: 0)?
tq1.q_restrict == tq2.q_restrict: 0;} static void Cyc_Tcutil_unify_it_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y){ x=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
x); y=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
y); if( x == y){ return;}{ void* _temp386=( void*) x->v; struct Cyc_Absyn_Conref*
_temp394; void* _temp396; _LL388: if( _temp386 ==( void*) Cyc_Absyn_No_constr){
goto _LL389;} else{ goto _LL390;} _LL390: if(( unsigned int) _temp386 > 1u?*((
int*) _temp386) == Cyc_Absyn_Forward_constr: 0){ _LL395: _temp394=(( struct Cyc_Absyn_Forward_constr_struct*)
_temp386)->f1; goto _LL391;} else{ goto _LL392;} _LL392: if(( unsigned int)
_temp386 > 1u?*(( int*) _temp386) == Cyc_Absyn_Eq_constr: 0){ _LL397: _temp396=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp386)->f1; goto _LL393;} else{
goto _LL387;} _LL389:( void*)( x->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp398=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp398[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp399; _temp399.tag= Cyc_Absyn_Forward_constr; _temp399.f1= y; _temp399;});
_temp398;}))); return; _LL391:( void) _throw((( void*(*)( struct _tagged_string
msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp400=( unsigned char*)"unify_conref: forward after compress";
struct _tagged_string _temp401; _temp401.curr= _temp400; _temp401.base= _temp400;
_temp401.last_plus_one= _temp400 + 37; _temp401;}))); _LL393: { void* _temp402=(
void*) y->v; struct Cyc_Absyn_Conref* _temp410; void* _temp412; _LL404: if(
_temp402 ==( void*) Cyc_Absyn_No_constr){ goto _LL405;} else{ goto _LL406;}
_LL406: if(( unsigned int) _temp402 > 1u?*(( int*) _temp402) == Cyc_Absyn_Forward_constr:
0){ _LL411: _temp410=(( struct Cyc_Absyn_Forward_constr_struct*) _temp402)->f1;
goto _LL407;} else{ goto _LL408;} _LL408: if(( unsigned int) _temp402 > 1u?*((
int*) _temp402) == Cyc_Absyn_Eq_constr: 0){ _LL413: _temp412=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp402)->f1; goto _LL409;} else{ goto _LL403;}
_LL405:( void*)( y->v=( void*)(( void*) x->v)); return; _LL407:( void) _throw(((
void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char*
_temp414=( unsigned char*)"unify_conref: forward after compress(2)"; struct
_tagged_string _temp415; _temp415.curr= _temp414; _temp415.base= _temp414;
_temp415.last_plus_one= _temp414 + 40; _temp415;}))); _LL409: if( cmp( _temp396,
_temp412) != 0){( void) _throw(( void*) Cyc_Tcutil_Unify);} return; _LL403:;}
_LL387:;}} static int Cyc_Tcutil_unify_conrefs( int(* cmp)( void*, void*),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y){ struct _handler_cons
_temp416; _push_handler(& _temp416);{ int _temp418= 0; if( setjmp( _temp416.handler)){
_temp418= 1;} if( ! _temp418){(( void(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)( cmp, x, y);{ int
_temp419= 1; _npop_handler( 0u); return _temp419;}; _pop_handler();} else{ void*
_temp417=( void*) _exn_thrown; void* _temp421= _temp417; _LL423: if( _temp421 ==
Cyc_Tcutil_Unify){ goto _LL424;} else{ goto _LL425;} _LL425: goto _LL426; _LL424:
return 0; _LL426:( void) _throw( _temp421); _LL422:;}}} static int Cyc_Tcutil_boundscmp(
void* b1, void* b2){ struct _tuple8 _temp428=({ struct _tuple8 _temp427;
_temp427.f1= b1; _temp427.f2= b2; _temp427;}); void* _temp438; void* _temp440;
void* _temp442; void* _temp444; void* _temp446; void* _temp448; void* _temp450;
struct Cyc_Absyn_Exp* _temp452; void* _temp454; struct Cyc_Absyn_Exp* _temp456;
_LL430: _LL441: _temp440= _temp428.f1; if( _temp440 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL439;} else{ goto _LL432;} _LL439: _temp438= _temp428.f2; if( _temp438 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL431;} else{ goto _LL432;} _LL432: _LL445:
_temp444= _temp428.f1; if( _temp444 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL443;}
else{ goto _LL434;} _LL443: _temp442= _temp428.f2; goto _LL433; _LL434: _LL449:
_temp448= _temp428.f1; goto _LL447; _LL447: _temp446= _temp428.f2; if( _temp446
==( void*) Cyc_Absyn_Unknown_b){ goto _LL435;} else{ goto _LL436;} _LL436:
_LL455: _temp454= _temp428.f1; if(( unsigned int) _temp454 > 1u?*(( int*)
_temp454) == Cyc_Absyn_Upper_b: 0){ _LL457: _temp456=(( struct Cyc_Absyn_Upper_b_struct*)
_temp454)->f1; goto _LL451;} else{ goto _LL429;} _LL451: _temp450= _temp428.f2;
if(( unsigned int) _temp450 > 1u?*(( int*) _temp450) == Cyc_Absyn_Upper_b: 0){
_LL453: _temp452=(( struct Cyc_Absyn_Upper_b_struct*) _temp450)->f1; goto _LL437;}
else{ goto _LL429;} _LL431: return 0; _LL433: return - 1; _LL435: return 1;
_LL437: { int i1=( int) Cyc_Evexp_eval_const_uint_exp( _temp456); int i2=( int)
Cyc_Evexp_eval_const_uint_exp( _temp452); if( i1 == i2){ return 0;} if( i1 < i2){
return - 1;} return 1;} _LL429:;} static int Cyc_Tcutil_equal_att( void* a1,
void* a2){ if( a1 == a2){ return 1;}{ struct _tuple8 _temp459=({ struct _tuple8
_temp458; _temp458.f1= a1; _temp458.f2= a2; _temp458;}); void* _temp469; int
_temp471; void* _temp473; int _temp475; void* _temp477; int _temp479; void*
_temp481; int _temp483; void* _temp485; struct _tagged_string _temp487; void*
_temp489; struct _tagged_string _temp491; _LL461: _LL474: _temp473= _temp459.f1;
if(( unsigned int) _temp473 > 16u?*(( int*) _temp473) == Cyc_Absyn_Regparm_att:
0){ _LL476: _temp475=(( struct Cyc_Absyn_Regparm_att_struct*) _temp473)->f1;
goto _LL470;} else{ goto _LL463;} _LL470: _temp469= _temp459.f2; if((
unsigned int) _temp469 > 16u?*(( int*) _temp469) == Cyc_Absyn_Regparm_att: 0){
_LL472: _temp471=(( struct Cyc_Absyn_Regparm_att_struct*) _temp469)->f1; goto
_LL462;} else{ goto _LL463;} _LL463: _LL482: _temp481= _temp459.f1; if((
unsigned int) _temp481 > 16u?*(( int*) _temp481) == Cyc_Absyn_Aligned_att: 0){
_LL484: _temp483=(( struct Cyc_Absyn_Aligned_att_struct*) _temp481)->f1; goto
_LL478;} else{ goto _LL465;} _LL478: _temp477= _temp459.f2; if(( unsigned int)
_temp477 > 16u?*(( int*) _temp477) == Cyc_Absyn_Aligned_att: 0){ _LL480:
_temp479=(( struct Cyc_Absyn_Aligned_att_struct*) _temp477)->f1; goto _LL464;}
else{ goto _LL465;} _LL465: _LL490: _temp489= _temp459.f1; if(( unsigned int)
_temp489 > 16u?*(( int*) _temp489) == Cyc_Absyn_Section_att: 0){ _LL492:
_temp491=(( struct Cyc_Absyn_Section_att_struct*) _temp489)->f1; goto _LL486;}
else{ goto _LL467;} _LL486: _temp485= _temp459.f2; if(( unsigned int) _temp485 >
16u?*(( int*) _temp485) == Cyc_Absyn_Section_att: 0){ _LL488: _temp487=(( struct
Cyc_Absyn_Section_att_struct*) _temp485)->f1; goto _LL466;} else{ goto _LL467;}
_LL467: goto _LL468; _LL462: _temp483= _temp475; _temp479= _temp471; goto _LL464;
_LL464: return _temp483 == _temp479; _LL466: return Cyc_String_strcmp( _temp491,
_temp487) == 0; _LL468: return 0; _LL460:;}} int Cyc_Tcutil_same_atts( struct
Cyc_List_List* a1, struct Cyc_List_List* a2){{ struct Cyc_List_List* a= a1; for(
0; a != 0; a=(( struct Cyc_List_List*) _check_null( a))->tl){ if( !(( int(*)(
int(* pred)( void*, void*), void* env, struct Cyc_List_List* x)) Cyc_List_exists_c)(
Cyc_Tcutil_equal_att,( void*)(( struct Cyc_List_List*) _check_null( a))->hd, a2)){
return 0;}}}{ struct Cyc_List_List* a= a2; for( 0; a != 0; a=(( struct Cyc_List_List*)
_check_null( a))->tl){ if( !(( int(*)( int(* pred)( void*, void*), void* env,
struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcutil_equal_att,( void*)((
struct Cyc_List_List*) _check_null( a))->hd, a1)){ return 0;}}} return 1;}
static void Cyc_Tcutil_normalize_effect( void* e){ e= Cyc_Tcutil_compress( e);{
void* _temp493= e; struct Cyc_List_List* _temp499; struct Cyc_List_List**
_temp501; _LL495: if(( unsigned int) _temp493 > 4u?*(( int*) _temp493) == Cyc_Absyn_JoinEff:
0){ _LL500: _temp499=(( struct Cyc_Absyn_JoinEff_struct*) _temp493)->f1;
_temp501=&(( struct Cyc_Absyn_JoinEff_struct*) _temp493)->f1; goto _LL496;}
else{ goto _LL497;} _LL497: goto _LL498; _LL496: { int nested_join= 0;{ struct
Cyc_List_List* effs=* _temp501; for( 0; effs != 0; effs=(( struct Cyc_List_List*)
_check_null( effs))->tl){ void* _temp502=( void*)(( struct Cyc_List_List*)
_check_null( effs))->hd; goto _LL503; _LL503: Cyc_Tcutil_normalize_effect(
_temp502);{ void* _temp504= Cyc_Tcutil_compress( _temp502); struct Cyc_List_List*
_temp510; _LL506: if(( unsigned int) _temp504 > 4u?*(( int*) _temp504) == Cyc_Absyn_JoinEff:
0){ _LL511: _temp510=(( struct Cyc_Absyn_JoinEff_struct*) _temp504)->f1; goto
_LL507;} else{ goto _LL508;} _LL508: goto _LL509; _LL507: nested_join= 1; goto
_LL505; _LL509: goto _LL505; _LL505:;}}} if( ! nested_join){ return;}{ struct
Cyc_List_List* effects= 0;{ struct Cyc_List_List* effs=* _temp501; for( 0; effs
!= 0; effs=(( struct Cyc_List_List*) _check_null( effs))->tl){ void* _temp512=
Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*) _check_null( effs))->hd);
struct Cyc_List_List* _temp518; _LL514: if(( unsigned int) _temp512 > 4u?*(( int*)
_temp512) == Cyc_Absyn_JoinEff: 0){ _LL519: _temp518=(( struct Cyc_Absyn_JoinEff_struct*)
_temp512)->f1; goto _LL515;} else{ goto _LL516;} _LL516: goto _LL517; _LL515:
effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_revappend)( _temp518, effects); goto _LL513; _LL517: effects=({
struct Cyc_List_List* _temp520=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp520->hd=( void*) _temp512; _temp520->tl= effects;
_temp520;}); goto _LL513; _LL513:;}}* _temp501=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( effects); goto _LL494;}} _LL498:
goto _LL494; _LL494:;}} struct _tuple9{ void* f1; struct Cyc_List_List* f2;
struct Cyc_List_List* f3; } ; static struct _tuple9* Cyc_Tcutil_get_effect_evar(
void* t){ void* _temp521= Cyc_Tcutil_compress( t); struct Cyc_List_List*
_temp529; struct Cyc_List_List _temp531; struct Cyc_List_List* _temp532; void*
_temp534; struct Cyc_Core_Opt* _temp536; int _temp538; struct Cyc_Core_Opt*
_temp540; struct Cyc_Core_Opt* _temp542; _LL523: if(( unsigned int) _temp521 > 4u?*((
int*) _temp521) == Cyc_Absyn_JoinEff: 0){ _LL530: _temp529=(( struct Cyc_Absyn_JoinEff_struct*)
_temp521)->f1; if( _temp529 == 0){ goto _LL525;} else{ _temp531=* _temp529;
_LL535: _temp534=( void*) _temp531.hd; goto _LL533; _LL533: _temp532= _temp531.tl;
goto _LL524;}} else{ goto _LL525;} _LL525: if(( unsigned int) _temp521 > 4u?*((
int*) _temp521) == Cyc_Absyn_Evar: 0){ _LL543: _temp542=(( struct Cyc_Absyn_Evar_struct*)
_temp521)->f1; goto _LL541; _LL541: _temp540=(( struct Cyc_Absyn_Evar_struct*)
_temp521)->f2; goto _LL539; _LL539: _temp538=(( struct Cyc_Absyn_Evar_struct*)
_temp521)->f3; goto _LL537; _LL537: _temp536=(( struct Cyc_Absyn_Evar_struct*)
_temp521)->f4; goto _LL526;} else{ goto _LL527;} _LL527: goto _LL528; _LL524: {
void* _temp544= Cyc_Tcutil_compress( _temp534); struct Cyc_Core_Opt* _temp550;
int _temp552; struct Cyc_Core_Opt* _temp554; struct Cyc_Core_Opt* _temp556;
_LL546: if(( unsigned int) _temp544 > 4u?*(( int*) _temp544) == Cyc_Absyn_Evar:
0){ _LL557: _temp556=(( struct Cyc_Absyn_Evar_struct*) _temp544)->f1; goto
_LL555; _LL555: _temp554=(( struct Cyc_Absyn_Evar_struct*) _temp544)->f2; goto
_LL553; _LL553: _temp552=(( struct Cyc_Absyn_Evar_struct*) _temp544)->f3; goto
_LL551; _LL551: _temp550=(( struct Cyc_Absyn_Evar_struct*) _temp544)->f4; goto
_LL547;} else{ goto _LL548;} _LL548: goto _LL549; _LL547: return({ struct
_tuple9* _temp558=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp558->f1= _temp534; _temp558->f2= _temp532; _temp558->f3=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp550))->v; _temp558;}); _LL549: return 0;
_LL545:;} _LL526: if( _temp542 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp542))->v !=( void*) Cyc_Absyn_EffKind){(( void(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp559=(
unsigned char*)"effect evar has wrong kind"; struct _tagged_string _temp560;
_temp560.curr= _temp559; _temp560.base= _temp559; _temp560.last_plus_one=
_temp559 + 27; _temp560;}));} return({ struct _tuple9* _temp561=( struct _tuple9*)
GC_malloc( sizeof( struct _tuple9)); _temp561->f1= t; _temp561->f2= 0; _temp561->f3=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp536))->v;
_temp561;}); _LL528: return 0; _LL522:;} static struct Cyc_Core_Opt Cyc_Tcutil_ek={(
void*)(( void*) 4u)}; int Cyc_Tcutil_region_in_effect( int constrain, void* r,
void* e){ r= Cyc_Tcutil_compress( r); if( r ==( void*) Cyc_Absyn_HeapRgn){
return 1;}{ void* _temp562= Cyc_Tcutil_compress( e); void* _temp572; struct Cyc_List_List*
_temp574; struct Cyc_Core_Opt* _temp576; int _temp578; struct Cyc_Core_Opt*
_temp580; struct Cyc_Core_Opt** _temp582; struct Cyc_Core_Opt* _temp583; _LL564:
if(( unsigned int) _temp562 > 4u?*(( int*) _temp562) == Cyc_Absyn_AccessEff: 0){
_LL573: _temp572=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp562)->f1;
goto _LL565;} else{ goto _LL566;} _LL566: if(( unsigned int) _temp562 > 4u?*((
int*) _temp562) == Cyc_Absyn_JoinEff: 0){ _LL575: _temp574=(( struct Cyc_Absyn_JoinEff_struct*)
_temp562)->f1; goto _LL567;} else{ goto _LL568;} _LL568: if(( unsigned int)
_temp562 > 4u?*(( int*) _temp562) == Cyc_Absyn_Evar: 0){ _LL584: _temp583=((
struct Cyc_Absyn_Evar_struct*) _temp562)->f1; goto _LL581; _LL581: _temp580=((
struct Cyc_Absyn_Evar_struct*) _temp562)->f2; _temp582=&(( struct Cyc_Absyn_Evar_struct*)
_temp562)->f2; goto _LL579; _LL579: _temp578=(( struct Cyc_Absyn_Evar_struct*)
_temp562)->f3; goto _LL577; _LL577: _temp576=(( struct Cyc_Absyn_Evar_struct*)
_temp562)->f4; goto _LL569;} else{ goto _LL570;} _LL570: goto _LL571; _LL565:
if( constrain){ return Cyc_Tcutil_unify( r, _temp572);} Cyc_Tcutil_compress(
_temp572); if( r == _temp572){ return 1;}{ struct _tuple8 _temp586=({ struct
_tuple8 _temp585; _temp585.f1= r; _temp585.f2= _temp572; _temp585;}); void*
_temp592; struct Cyc_Absyn_Tvar* _temp594; void* _temp596; struct Cyc_Absyn_Tvar*
_temp598; _LL588: _LL597: _temp596= _temp586.f1; if(( unsigned int) _temp596 > 4u?*((
int*) _temp596) == Cyc_Absyn_VarType: 0){ _LL599: _temp598=(( struct Cyc_Absyn_VarType_struct*)
_temp596)->f1; goto _LL593;} else{ goto _LL590;} _LL593: _temp592= _temp586.f2;
if(( unsigned int) _temp592 > 4u?*(( int*) _temp592) == Cyc_Absyn_VarType: 0){
_LL595: _temp594=(( struct Cyc_Absyn_VarType_struct*) _temp592)->f1; goto _LL589;}
else{ goto _LL590;} _LL590: goto _LL591; _LL589: return Cyc_Absyn_tvar_cmp(
_temp598, _temp594) == 0; _LL591: return 0; _LL587:;} _LL567: for( 0; _temp574
!= 0; _temp574=(( struct Cyc_List_List*) _check_null( _temp574))->tl){ if( Cyc_Tcutil_region_in_effect(
constrain, r,( void*)(( struct Cyc_List_List*) _check_null( _temp574))->hd)){
return 1;}} return 0; _LL569: if( _temp583 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp583))->v !=( void*) Cyc_Absyn_EffKind){(( void(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp600=(
unsigned char*)"effect evar has wrong kind"; struct _tagged_string _temp601;
_temp601.curr= _temp600; _temp601.base= _temp600; _temp601.last_plus_one=
_temp600 + 27; _temp601;}));} if( ! constrain){ return 0;}{ void* _temp602= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek, _temp576); goto _LL603; _LL603: Cyc_Tcutil_occurs(
_temp602,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp576))->v,
r);{ struct Cyc_Absyn_JoinEff_struct* _temp610=({ struct Cyc_Absyn_JoinEff_struct*
_temp604=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp604[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp605; _temp605.tag= Cyc_Absyn_JoinEff;
_temp605.f1=({ struct Cyc_List_List* _temp606=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp606->hd=( void*) _temp602; _temp606->tl=({
struct Cyc_List_List* _temp607=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp607->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp608=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp608[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp609; _temp609.tag= Cyc_Absyn_AccessEff;
_temp609.f1=( void*) r; _temp609;}); _temp608;})); _temp607->tl= 0; _temp607;});
_temp606;}); _temp605;}); _temp604;}); goto _LL611; _LL611:* _temp582=({ struct
Cyc_Core_Opt* _temp612=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp612->v=( void*)(( void*) _temp610); _temp612;}); return 1;}} _LL571: return
0; _LL563:;}} static int Cyc_Tcutil_variable_in_effect( int constrain, struct
Cyc_Absyn_Tvar* v, void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp613= e;
struct Cyc_Absyn_Tvar* _temp623; struct Cyc_List_List* _temp625; struct Cyc_Core_Opt*
_temp627; int _temp629; struct Cyc_Core_Opt* _temp631; struct Cyc_Core_Opt**
_temp633; struct Cyc_Core_Opt* _temp634; _LL615: if(( unsigned int) _temp613 > 4u?*((
int*) _temp613) == Cyc_Absyn_VarType: 0){ _LL624: _temp623=(( struct Cyc_Absyn_VarType_struct*)
_temp613)->f1; goto _LL616;} else{ goto _LL617;} _LL617: if(( unsigned int)
_temp613 > 4u?*(( int*) _temp613) == Cyc_Absyn_JoinEff: 0){ _LL626: _temp625=((
struct Cyc_Absyn_JoinEff_struct*) _temp613)->f1; goto _LL618;} else{ goto _LL619;}
_LL619: if(( unsigned int) _temp613 > 4u?*(( int*) _temp613) == Cyc_Absyn_Evar:
0){ _LL635: _temp634=(( struct Cyc_Absyn_Evar_struct*) _temp613)->f1; goto
_LL632; _LL632: _temp631=(( struct Cyc_Absyn_Evar_struct*) _temp613)->f2;
_temp633=&(( struct Cyc_Absyn_Evar_struct*) _temp613)->f2; goto _LL630; _LL630:
_temp629=(( struct Cyc_Absyn_Evar_struct*) _temp613)->f3; goto _LL628; _LL628:
_temp627=(( struct Cyc_Absyn_Evar_struct*) _temp613)->f4; goto _LL620;} else{
goto _LL621;} _LL621: goto _LL622; _LL616: return Cyc_Absyn_tvar_cmp( v,
_temp623) == 0; _LL618: for( 0; _temp625 != 0; _temp625=(( struct Cyc_List_List*)
_check_null( _temp625))->tl){ if( Cyc_Tcutil_variable_in_effect( constrain, v,(
void*)(( struct Cyc_List_List*) _check_null( _temp625))->hd)){ return 1;}}
return 0; _LL620: if( _temp634 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp634))->v !=( void*) Cyc_Absyn_EffKind){(( void(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp636=(
unsigned char*)"effect evar has wrong kind"; struct _tagged_string _temp637;
_temp637.curr= _temp636; _temp637.base= _temp636; _temp637.last_plus_one=
_temp636 + 27; _temp637;}));}{ void* _temp638= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek, _temp627); goto _LL639; _LL639: if( !(( int(*)( int(* compare)(
struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct
Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp627))->v, v)){ return 0;}{ struct Cyc_Absyn_JoinEff_struct*
_temp646=({ struct Cyc_Absyn_JoinEff_struct* _temp640=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp640[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp641; _temp641.tag= Cyc_Absyn_JoinEff; _temp641.f1=({ struct Cyc_List_List*
_temp642=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp642->hd=( void*) _temp638; _temp642->tl=({ struct Cyc_List_List* _temp643=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp643->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp644=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp644[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp645; _temp645.tag= Cyc_Absyn_VarType; _temp645.f1= v; _temp645;}); _temp644;}));
_temp643->tl= 0; _temp643;}); _temp642;}); _temp641;}); _temp640;}); goto _LL647;
_LL647:* _temp633=({ struct Cyc_Core_Opt* _temp648=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp648->v=( void*)(( void*) _temp646);
_temp648;}); return 1;}} _LL622: return 0; _LL614:;}} static int Cyc_Tcutil_evar_in_effect(
void* evar, void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp649= e; struct Cyc_List_List*
_temp657; struct Cyc_Core_Opt* _temp659; int _temp661; struct Cyc_Core_Opt*
_temp663; struct Cyc_Core_Opt* _temp665; _LL651: if(( unsigned int) _temp649 > 4u?*((
int*) _temp649) == Cyc_Absyn_JoinEff: 0){ _LL658: _temp657=(( struct Cyc_Absyn_JoinEff_struct*)
_temp649)->f1; goto _LL652;} else{ goto _LL653;} _LL653: if(( unsigned int)
_temp649 > 4u?*(( int*) _temp649) == Cyc_Absyn_Evar: 0){ _LL666: _temp665=((
struct Cyc_Absyn_Evar_struct*) _temp649)->f1; goto _LL664; _LL664: _temp663=((
struct Cyc_Absyn_Evar_struct*) _temp649)->f2; goto _LL662; _LL662: _temp661=((
struct Cyc_Absyn_Evar_struct*) _temp649)->f3; goto _LL660; _LL660: _temp659=((
struct Cyc_Absyn_Evar_struct*) _temp649)->f4; goto _LL654;} else{ goto _LL655;}
_LL655: goto _LL656; _LL652: for( 0; _temp657 != 0; _temp657=(( struct Cyc_List_List*)
_check_null( _temp657))->tl){ if( Cyc_Tcutil_evar_in_effect( evar,( void*)((
struct Cyc_List_List*) _check_null( _temp657))->hd)){ return 1;}} return 0;
_LL654: return evar == e; _LL656: return 0; _LL650:;}} int Cyc_Tcutil_subset_effect(
int set_to_empty, void* e1, void* e2){{ struct _tuple9* _temp667= Cyc_Tcutil_get_effect_evar(
e2); struct _tuple9 _temp673; struct Cyc_List_List* _temp674; struct Cyc_List_List*
_temp676; void* _temp678; _LL669: if( _temp667 == 0){ goto _LL671;} else{
_temp673=* _temp667; _LL679: _temp678= _temp673.f1; goto _LL677; _LL677:
_temp676= _temp673.f2; goto _LL675; _LL675: _temp674= _temp673.f3; goto _LL670;}
_LL671: goto _LL672; _LL670: { void* _temp681= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,({ struct Cyc_Core_Opt* _temp680=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp680->v=( void*) _temp674; _temp680;})); goto
_LL682; _LL682: { struct Cyc_Absyn_JoinEff_struct* _temp687=({ struct Cyc_Absyn_JoinEff_struct*
_temp683=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp683[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp684; _temp684.tag= Cyc_Absyn_JoinEff;
_temp684.f1=({ struct Cyc_List_List* _temp685=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp685->hd=( void*) _temp681; _temp685->tl=({
struct Cyc_List_List* _temp686=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp686->hd=( void*) e1; _temp686->tl= 0; _temp686;});
_temp685;}); _temp684;}); _temp683;}); goto _LL688; _LL688: return Cyc_Tcutil_unify(
_temp678,( void*) _temp687);}} _LL672: goto _LL668; _LL668:;}{ void* _temp689=
Cyc_Tcutil_compress( e1); struct Cyc_List_List* _temp701; void* _temp703; struct
Cyc_Absyn_Tvar* _temp705; struct Cyc_Core_Opt* _temp707; int _temp709; struct
Cyc_Core_Opt* _temp711; struct Cyc_Core_Opt** _temp713; struct Cyc_Core_Opt*
_temp714; _LL691: if(( unsigned int) _temp689 > 4u?*(( int*) _temp689) == Cyc_Absyn_JoinEff:
0){ _LL702: _temp701=(( struct Cyc_Absyn_JoinEff_struct*) _temp689)->f1; goto
_LL692;} else{ goto _LL693;} _LL693: if(( unsigned int) _temp689 > 4u?*(( int*)
_temp689) == Cyc_Absyn_AccessEff: 0){ _LL704: _temp703=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp689)->f1; goto _LL694;} else{ goto _LL695;} _LL695: if(( unsigned int)
_temp689 > 4u?*(( int*) _temp689) == Cyc_Absyn_VarType: 0){ _LL706: _temp705=((
struct Cyc_Absyn_VarType_struct*) _temp689)->f1; goto _LL696;} else{ goto _LL697;}
_LL697: if(( unsigned int) _temp689 > 4u?*(( int*) _temp689) == Cyc_Absyn_Evar:
0){ _LL715: _temp714=(( struct Cyc_Absyn_Evar_struct*) _temp689)->f1; goto
_LL712; _LL712: _temp711=(( struct Cyc_Absyn_Evar_struct*) _temp689)->f2;
_temp713=&(( struct Cyc_Absyn_Evar_struct*) _temp689)->f2; goto _LL710; _LL710:
_temp709=(( struct Cyc_Absyn_Evar_struct*) _temp689)->f3; goto _LL708; _LL708:
_temp707=(( struct Cyc_Absyn_Evar_struct*) _temp689)->f4; goto _LL698;} else{
goto _LL699;} _LL699: goto _LL700; _LL692: for( 0; _temp701 != 0; _temp701=((
struct Cyc_List_List*) _check_null( _temp701))->tl){ if( ! Cyc_Tcutil_subset_effect(
set_to_empty,( void*)(( struct Cyc_List_List*) _check_null( _temp701))->hd, e2)){
return 0;}} return 1; _LL694: return( Cyc_Tcutil_region_in_effect( 0, _temp703,
e2)? 1: Cyc_Tcutil_region_in_effect( 1, _temp703, e2))? 1: Cyc_Tcutil_unify(
_temp703,( void*) Cyc_Absyn_HeapRgn); _LL696: return Cyc_Tcutil_variable_in_effect(
0, _temp705, e2)? 1: Cyc_Tcutil_variable_in_effect( 1, _temp705, e2); _LL698:
if( ! Cyc_Tcutil_evar_in_effect( e1, e2)){ if( set_to_empty){* _temp713=({
struct Cyc_Core_Opt* _temp716=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp716->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp717=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp717[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp718; _temp718.tag= Cyc_Absyn_JoinEff;
_temp718.f1= 0; _temp718;}); _temp717;})); _temp716;});} else{ Cyc_Tcutil_occurs(
e1,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp707))->v,
e2);* _temp713=({ struct Cyc_Core_Opt* _temp719=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp719->v=( void*) e2; _temp719;});}}
return 1; _LL700: return(( int(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)((
struct _tagged_string)({ struct _tagged_string _temp720= Cyc_Absynpp_typ2string(
e1); xprintf("subset_effect: bad effect: %.*s", _temp720.last_plus_one -
_temp720.curr, _temp720.curr);})); _LL690:;}} struct _tuple10{ struct _tuple9*
f1; struct _tuple9* f2; } ; static int Cyc_Tcutil_unify_effect( void* e1, void*
e2){ e1= Cyc_Tcutil_compress( e1); e2= Cyc_Tcutil_compress( e2); Cyc_Tcutil_normalize_effect(
e1); Cyc_Tcutil_normalize_effect( e2);{ struct _tuple10 _temp722=({ struct
_tuple10 _temp721; _temp721.f1= Cyc_Tcutil_get_effect_evar( e1); _temp721.f2=
Cyc_Tcutil_get_effect_evar( e2); _temp721;}); struct _tuple9* _temp728; struct
_tuple9 _temp730; struct Cyc_List_List* _temp731; struct Cyc_List_List* _temp733;
void* _temp735; struct _tuple9* _temp737; struct _tuple9 _temp739; struct Cyc_List_List*
_temp740; struct Cyc_List_List* _temp742; void* _temp744; _LL724: _LL738:
_temp737= _temp722.f1; if( _temp737 == 0){ goto _LL726;} else{ _temp739=*
_temp737; _LL745: _temp744= _temp739.f1; goto _LL743; _LL743: _temp742= _temp739.f2;
goto _LL741; _LL741: _temp740= _temp739.f3; goto _LL729;} _LL729: _temp728=
_temp722.f2; if( _temp728 == 0){ goto _LL726;} else{ _temp730=* _temp728; _LL736:
_temp735= _temp730.f1; goto _LL734; _LL734: _temp733= _temp730.f2; goto _LL732;
_LL732: _temp731= _temp730.f3; goto _LL725;} _LL726: goto _LL727; _LL725: {
struct Cyc_List_List* _temp746= 0; goto _LL747; _LL747: for( 0; _temp740 != 0;
_temp740=(( struct Cyc_List_List*) _check_null( _temp740))->tl){ if((( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, _temp731,(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp740))->hd)){
_temp746=({ struct Cyc_List_List* _temp748=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp748->hd=( void*)(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp740))->hd); _temp748->tl= _temp746;
_temp748;});}}{ void* _temp750= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,({
struct Cyc_Core_Opt* _temp749=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp749->v=( void*) _temp746; _temp749;})); goto _LL751; _LL751: {
struct Cyc_Absyn_JoinEff_struct* _temp755=({ struct Cyc_Absyn_JoinEff_struct*
_temp752=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp752[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp753; _temp753.tag= Cyc_Absyn_JoinEff;
_temp753.f1=({ struct Cyc_List_List* _temp754=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp754->hd=( void*) _temp750; _temp754->tl=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp742, _temp733); _temp754;}); _temp753;}); _temp752;}); goto _LL756; _LL756:
return Cyc_Tcutil_unify( _temp744,( void*) _temp755)? Cyc_Tcutil_unify( _temp735,(
void*) _temp755): 0;}}} _LL727: return Cyc_Tcutil_subset_effect( 0, e1, e2)? Cyc_Tcutil_subset_effect(
0, e2, e1): 0; _LL723:;}} struct _tuple11{ struct Cyc_Absyn_VarargInfo* f1;
struct Cyc_Absyn_VarargInfo* f2; } ; struct _tuple12{ struct Cyc_Core_Opt* f1;
struct Cyc_Core_Opt* f2; } ; void Cyc_Tcutil_unify_it( void* t1, void* t2){ t1=
Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2); if( t1 == t2){ return;}{
void* _temp757= t1; struct Cyc_Core_Opt* _temp763; int _temp765; struct Cyc_Core_Opt*
_temp767; struct Cyc_Core_Opt** _temp769; struct Cyc_Core_Opt* _temp770; _LL759:
if(( unsigned int) _temp757 > 4u?*(( int*) _temp757) == Cyc_Absyn_Evar: 0){
_LL771: _temp770=(( struct Cyc_Absyn_Evar_struct*) _temp757)->f1; goto _LL768;
_LL768: _temp767=(( struct Cyc_Absyn_Evar_struct*) _temp757)->f2; _temp769=&((
struct Cyc_Absyn_Evar_struct*) _temp757)->f2; goto _LL766; _LL766: _temp765=((
struct Cyc_Absyn_Evar_struct*) _temp757)->f3; goto _LL764; _LL764: _temp763=((
struct Cyc_Absyn_Evar_struct*) _temp757)->f4; goto _LL760;} else{ goto _LL761;}
_LL761: goto _LL762; _LL760: Cyc_Tcutil_occurs( t1,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp763))->v, t2);{ void* _temp772= Cyc_Tcutil_typ_kind(
t2); goto _LL773; _LL773: if( Cyc_Tcutil_kind_leq( _temp772,( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp770))->v)){* _temp769=({ struct Cyc_Core_Opt*
_temp774=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp774->v=( void*) t2; _temp774;}); return;} else{{ void* _temp775= t2; struct
Cyc_Core_Opt* _temp783; int _temp785; struct Cyc_Core_Opt* _temp787; struct Cyc_Core_Opt**
_temp789; struct Cyc_Core_Opt* _temp790; struct Cyc_Absyn_PtrInfo _temp793;
_LL777: if(( unsigned int) _temp775 > 4u?*(( int*) _temp775) == Cyc_Absyn_Evar:
0){ _LL791: _temp790=(( struct Cyc_Absyn_Evar_struct*) _temp775)->f1; goto
_LL788; _LL788: _temp787=(( struct Cyc_Absyn_Evar_struct*) _temp775)->f2;
_temp789=&(( struct Cyc_Absyn_Evar_struct*) _temp775)->f2; goto _LL786; _LL786:
_temp785=(( struct Cyc_Absyn_Evar_struct*) _temp775)->f3; goto _LL784; _LL784:
_temp783=(( struct Cyc_Absyn_Evar_struct*) _temp775)->f4; goto _LL778;} else{
goto _LL779;} _LL779: if(( unsigned int) _temp775 > 4u?*(( int*) _temp775) ==
Cyc_Absyn_PointerType: 0){ _LL794: _temp793=(( struct Cyc_Absyn_PointerType_struct*)
_temp775)->f1; goto _LL792;} else{ goto _LL781;} _LL792: if(( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp770))->v ==( void*) Cyc_Absyn_BoxKind){ goto
_LL780;} else{ goto _LL781;} _LL781: goto _LL782; _LL778: { struct Cyc_List_List*
_temp795=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp763))->v;
goto _LL796; _LL796:{ struct Cyc_List_List* s2=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( _temp783))->v; for( 0; s2 != 0; s2=(( struct Cyc_List_List*)
_check_null( s2))->tl){ if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, _temp795,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( s2))->hd)){( void) _throw(( void*) Cyc_Tcutil_Unify);}}} if( Cyc_Tcutil_kind_leq((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp770))->v, _temp772)){* _temp789=({
struct Cyc_Core_Opt* _temp797=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp797->v=( void*) t1; _temp797;}); return;} goto _LL776;}
_LL780: { struct Cyc_Absyn_Conref* _temp798=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp793.bounds); goto
_LL799; _LL799:{ void* _temp800=( void*) _temp798->v; _LL802: if( _temp800 ==(
void*) Cyc_Absyn_No_constr){ goto _LL803;} else{ goto _LL804;} _LL804: goto
_LL805; _LL803:( void*)( _temp798->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp806=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp806[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp807; _temp807.tag= Cyc_Absyn_Eq_constr;
_temp807.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp808=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp808[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp809; _temp809.tag= Cyc_Absyn_Upper_b;
_temp809.f1= Cyc_Absyn_signed_int_exp( 1, 0); _temp809;}); _temp808;}));
_temp807;}); _temp806;})));* _temp769=({ struct Cyc_Core_Opt* _temp810=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp810->v=( void*) t2;
_temp810;}); return; _LL805: goto _LL801; _LL801:;} goto _LL776;} _LL782: goto
_LL776; _LL776:;}( void) _throw(( void*) Cyc_Tcutil_Unify);}} _LL762: goto
_LL758; _LL758:;}{ struct _tuple8 _temp812=({ struct _tuple8 _temp811; _temp811.f1=
t2; _temp811.f2= t1; _temp811;}); void* _temp862; void* _temp864; struct Cyc_Core_Opt*
_temp866; int _temp868; struct Cyc_Core_Opt* _temp870; struct Cyc_Core_Opt*
_temp872; void* _temp874; void* _temp876; void* _temp878; struct Cyc_Absyn_Tvar*
_temp880; void* _temp882; struct Cyc_Absyn_Tvar* _temp884; void* _temp886;
struct Cyc_Absyn_Structdecl** _temp888; struct Cyc_List_List* _temp890; struct
_tuple1* _temp892; void* _temp894; struct Cyc_Absyn_Structdecl** _temp896;
struct Cyc_List_List* _temp898; struct _tuple1* _temp900; void* _temp902; struct
Cyc_Absyn_Enumdecl* _temp904; struct _tuple1* _temp906; void* _temp908; struct
Cyc_Absyn_Enumdecl* _temp910; struct _tuple1* _temp912; void* _temp914; struct
Cyc_Absyn_Uniondecl** _temp916; struct Cyc_List_List* _temp918; struct _tuple1*
_temp920; void* _temp922; struct Cyc_Absyn_Uniondecl** _temp924; struct Cyc_List_List*
_temp926; struct _tuple1* _temp928; void* _temp930; struct Cyc_Absyn_TunionInfo
_temp932; void* _temp934; struct Cyc_List_List* _temp936; void* _temp938; struct
Cyc_Absyn_Tuniondecl* _temp940; void* _temp942; struct Cyc_Absyn_TunionInfo
_temp944; void* _temp946; struct Cyc_List_List* _temp948; void* _temp950; struct
Cyc_Absyn_Tuniondecl* _temp952; void* _temp954; struct Cyc_Absyn_TunionFieldInfo
_temp956; struct Cyc_List_List* _temp958; void* _temp960; struct Cyc_Absyn_Tunionfield*
_temp962; struct Cyc_Absyn_Tuniondecl* _temp964; void* _temp966; struct Cyc_Absyn_TunionFieldInfo
_temp968; struct Cyc_List_List* _temp970; void* _temp972; struct Cyc_Absyn_Tunionfield*
_temp974; struct Cyc_Absyn_Tuniondecl* _temp976; void* _temp978; struct Cyc_Absyn_PtrInfo
_temp980; struct Cyc_Absyn_Conref* _temp982; struct Cyc_Absyn_Tqual _temp984;
struct Cyc_Absyn_Conref* _temp986; void* _temp988; void* _temp990; void*
_temp992; struct Cyc_Absyn_PtrInfo _temp994; struct Cyc_Absyn_Conref* _temp996;
struct Cyc_Absyn_Tqual _temp998; struct Cyc_Absyn_Conref* _temp1000; void*
_temp1002; void* _temp1004; void* _temp1006; void* _temp1008; void* _temp1010;
void* _temp1012; void* _temp1014; void* _temp1016; void* _temp1018; void*
_temp1020; void* _temp1022; void* _temp1024; void* _temp1026; struct Cyc_Absyn_Exp*
_temp1028; struct Cyc_Absyn_Tqual _temp1030; void* _temp1032; void* _temp1034;
struct Cyc_Absyn_Exp* _temp1036; struct Cyc_Absyn_Tqual _temp1038; void*
_temp1040; void* _temp1042; struct Cyc_Absyn_FnInfo _temp1044; struct Cyc_List_List*
_temp1046; struct Cyc_Absyn_VarargInfo* _temp1048; int _temp1050; struct Cyc_List_List*
_temp1052; void* _temp1054; struct Cyc_Core_Opt* _temp1056; struct Cyc_List_List*
_temp1058; void* _temp1060; struct Cyc_Absyn_FnInfo _temp1062; struct Cyc_List_List*
_temp1064; struct Cyc_Absyn_VarargInfo* _temp1066; int _temp1068; struct Cyc_List_List*
_temp1070; void* _temp1072; struct Cyc_Core_Opt* _temp1074; struct Cyc_List_List*
_temp1076; void* _temp1078; struct Cyc_List_List* _temp1080; void* _temp1082;
struct Cyc_List_List* _temp1084; void* _temp1086; struct Cyc_List_List*
_temp1088; void* _temp1090; struct Cyc_List_List* _temp1092; void* _temp1094;
struct Cyc_List_List* _temp1096; void* _temp1098; struct Cyc_List_List*
_temp1100; void* _temp1102; void* _temp1104; void* _temp1106; void* _temp1108;
void* _temp1110; void* _temp1112; void* _temp1114; void* _temp1116; struct Cyc_List_List*
_temp1118; void* _temp1120; struct Cyc_List_List* _temp1122; void* _temp1124;
void* _temp1126; void* _temp1128; void* _temp1130; void* _temp1132; void*
_temp1134; void* _temp1136; _LL814: _LL865: _temp864= _temp812.f1; if((
unsigned int) _temp864 > 4u?*(( int*) _temp864) == Cyc_Absyn_Evar: 0){ _LL873:
_temp872=(( struct Cyc_Absyn_Evar_struct*) _temp864)->f1; goto _LL871; _LL871:
_temp870=(( struct Cyc_Absyn_Evar_struct*) _temp864)->f2; goto _LL869; _LL869:
_temp868=(( struct Cyc_Absyn_Evar_struct*) _temp864)->f3; goto _LL867; _LL867:
_temp866=(( struct Cyc_Absyn_Evar_struct*) _temp864)->f4; goto _LL863;} else{
goto _LL816;} _LL863: _temp862= _temp812.f2; goto _LL815; _LL816: _LL877:
_temp876= _temp812.f1; if( _temp876 ==( void*) Cyc_Absyn_VoidType){ goto _LL875;}
else{ goto _LL818;} _LL875: _temp874= _temp812.f2; if( _temp874 ==( void*) Cyc_Absyn_VoidType){
goto _LL817;} else{ goto _LL818;} _LL818: _LL883: _temp882= _temp812.f1; if((
unsigned int) _temp882 > 4u?*(( int*) _temp882) == Cyc_Absyn_VarType: 0){ _LL885:
_temp884=(( struct Cyc_Absyn_VarType_struct*) _temp882)->f1; goto _LL879;} else{
goto _LL820;} _LL879: _temp878= _temp812.f2; if(( unsigned int) _temp878 > 4u?*((
int*) _temp878) == Cyc_Absyn_VarType: 0){ _LL881: _temp880=(( struct Cyc_Absyn_VarType_struct*)
_temp878)->f1; goto _LL819;} else{ goto _LL820;} _LL820: _LL895: _temp894=
_temp812.f1; if(( unsigned int) _temp894 > 4u?*(( int*) _temp894) == Cyc_Absyn_StructType:
0){ _LL901: _temp900=(( struct Cyc_Absyn_StructType_struct*) _temp894)->f1; goto
_LL899; _LL899: _temp898=(( struct Cyc_Absyn_StructType_struct*) _temp894)->f2;
goto _LL897; _LL897: _temp896=(( struct Cyc_Absyn_StructType_struct*) _temp894)->f3;
goto _LL887;} else{ goto _LL822;} _LL887: _temp886= _temp812.f2; if((
unsigned int) _temp886 > 4u?*(( int*) _temp886) == Cyc_Absyn_StructType: 0){
_LL893: _temp892=(( struct Cyc_Absyn_StructType_struct*) _temp886)->f1; goto
_LL891; _LL891: _temp890=(( struct Cyc_Absyn_StructType_struct*) _temp886)->f2;
goto _LL889; _LL889: _temp888=(( struct Cyc_Absyn_StructType_struct*) _temp886)->f3;
goto _LL821;} else{ goto _LL822;} _LL822: _LL909: _temp908= _temp812.f1; if((
unsigned int) _temp908 > 4u?*(( int*) _temp908) == Cyc_Absyn_EnumType: 0){
_LL913: _temp912=(( struct Cyc_Absyn_EnumType_struct*) _temp908)->f1; goto
_LL911; _LL911: _temp910=(( struct Cyc_Absyn_EnumType_struct*) _temp908)->f2;
goto _LL903;} else{ goto _LL824;} _LL903: _temp902= _temp812.f2; if((
unsigned int) _temp902 > 4u?*(( int*) _temp902) == Cyc_Absyn_EnumType: 0){
_LL907: _temp906=(( struct Cyc_Absyn_EnumType_struct*) _temp902)->f1; goto
_LL905; _LL905: _temp904=(( struct Cyc_Absyn_EnumType_struct*) _temp902)->f2;
goto _LL823;} else{ goto _LL824;} _LL824: _LL923: _temp922= _temp812.f1; if((
unsigned int) _temp922 > 4u?*(( int*) _temp922) == Cyc_Absyn_UnionType: 0){
_LL929: _temp928=(( struct Cyc_Absyn_UnionType_struct*) _temp922)->f1; goto
_LL927; _LL927: _temp926=(( struct Cyc_Absyn_UnionType_struct*) _temp922)->f2;
goto _LL925; _LL925: _temp924=(( struct Cyc_Absyn_UnionType_struct*) _temp922)->f3;
goto _LL915;} else{ goto _LL826;} _LL915: _temp914= _temp812.f2; if((
unsigned int) _temp914 > 4u?*(( int*) _temp914) == Cyc_Absyn_UnionType: 0){
_LL921: _temp920=(( struct Cyc_Absyn_UnionType_struct*) _temp914)->f1; goto
_LL919; _LL919: _temp918=(( struct Cyc_Absyn_UnionType_struct*) _temp914)->f2;
goto _LL917; _LL917: _temp916=(( struct Cyc_Absyn_UnionType_struct*) _temp914)->f3;
goto _LL825;} else{ goto _LL826;} _LL826: _LL943: _temp942= _temp812.f1; if((
unsigned int) _temp942 > 4u?*(( int*) _temp942) == Cyc_Absyn_TunionType: 0){
_LL945: _temp944=(( struct Cyc_Absyn_TunionType_struct*) _temp942)->f1; _LL951:
_temp950=( void*) _temp944.tunion_info; if(*(( int*) _temp950) == Cyc_Absyn_KnownTunion){
_LL953: _temp952=(( struct Cyc_Absyn_KnownTunion_struct*) _temp950)->f1; goto
_LL949;} else{ goto _LL828;} _LL949: _temp948= _temp944.targs; goto _LL947;
_LL947: _temp946=( void*) _temp944.rgn; goto _LL931;} else{ goto _LL828;} _LL931:
_temp930= _temp812.f2; if(( unsigned int) _temp930 > 4u?*(( int*) _temp930) ==
Cyc_Absyn_TunionType: 0){ _LL933: _temp932=(( struct Cyc_Absyn_TunionType_struct*)
_temp930)->f1; _LL939: _temp938=( void*) _temp932.tunion_info; if(*(( int*)
_temp938) == Cyc_Absyn_KnownTunion){ _LL941: _temp940=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp938)->f1; goto _LL937;} else{ goto _LL828;} _LL937: _temp936= _temp932.targs;
goto _LL935; _LL935: _temp934=( void*) _temp932.rgn; goto _LL827;} else{ goto
_LL828;} _LL828: _LL967: _temp966= _temp812.f1; if(( unsigned int) _temp966 > 4u?*((
int*) _temp966) == Cyc_Absyn_TunionFieldType: 0){ _LL969: _temp968=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp966)->f1; _LL973: _temp972=( void*) _temp968.field_info; if(*(( int*)
_temp972) == Cyc_Absyn_KnownTunionfield){ _LL977: _temp976=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp972)->f1; goto _LL975; _LL975: _temp974=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp972)->f2; goto _LL971;} else{ goto _LL830;} _LL971: _temp970= _temp968.targs;
goto _LL955;} else{ goto _LL830;} _LL955: _temp954= _temp812.f2; if((
unsigned int) _temp954 > 4u?*(( int*) _temp954) == Cyc_Absyn_TunionFieldType: 0){
_LL957: _temp956=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp954)->f1;
_LL961: _temp960=( void*) _temp956.field_info; if(*(( int*) _temp960) == Cyc_Absyn_KnownTunionfield){
_LL965: _temp964=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp960)->f1;
goto _LL963; _LL963: _temp962=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp960)->f2; goto _LL959;} else{ goto _LL830;} _LL959: _temp958= _temp956.targs;
goto _LL829;} else{ goto _LL830;} _LL830: _LL993: _temp992= _temp812.f1; if((
unsigned int) _temp992 > 4u?*(( int*) _temp992) == Cyc_Absyn_PointerType: 0){
_LL995: _temp994=(( struct Cyc_Absyn_PointerType_struct*) _temp992)->f1; _LL1005:
_temp1004=( void*) _temp994.elt_typ; goto _LL1003; _LL1003: _temp1002=( void*)
_temp994.rgn_typ; goto _LL1001; _LL1001: _temp1000= _temp994.nullable; goto
_LL999; _LL999: _temp998= _temp994.tq; goto _LL997; _LL997: _temp996= _temp994.bounds;
goto _LL979;} else{ goto _LL832;} _LL979: _temp978= _temp812.f2; if((
unsigned int) _temp978 > 4u?*(( int*) _temp978) == Cyc_Absyn_PointerType: 0){
_LL981: _temp980=(( struct Cyc_Absyn_PointerType_struct*) _temp978)->f1; _LL991:
_temp990=( void*) _temp980.elt_typ; goto _LL989; _LL989: _temp988=( void*)
_temp980.rgn_typ; goto _LL987; _LL987: _temp986= _temp980.nullable; goto _LL985;
_LL985: _temp984= _temp980.tq; goto _LL983; _LL983: _temp982= _temp980.bounds;
goto _LL831;} else{ goto _LL832;} _LL832: _LL1013: _temp1012= _temp812.f1; if((
unsigned int) _temp1012 > 4u?*(( int*) _temp1012) == Cyc_Absyn_IntType: 0){
_LL1017: _temp1016=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1012)->f1;
goto _LL1015; _LL1015: _temp1014=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1012)->f2; goto _LL1007;} else{ goto _LL834;} _LL1007: _temp1006= _temp812.f2;
if(( unsigned int) _temp1006 > 4u?*(( int*) _temp1006) == Cyc_Absyn_IntType: 0){
_LL1011: _temp1010=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1006)->f1;
goto _LL1009; _LL1009: _temp1008=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1006)->f2; goto _LL833;} else{ goto _LL834;} _LL834: _LL1021: _temp1020=
_temp812.f1; if( _temp1020 ==( void*) Cyc_Absyn_FloatType){ goto _LL1019;} else{
goto _LL836;} _LL1019: _temp1018= _temp812.f2; if( _temp1018 ==( void*) Cyc_Absyn_FloatType){
goto _LL835;} else{ goto _LL836;} _LL836: _LL1025: _temp1024= _temp812.f1; if(
_temp1024 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1023;} else{ goto _LL838;}
_LL1023: _temp1022= _temp812.f2; if( _temp1022 ==( void*) Cyc_Absyn_DoubleType){
goto _LL837;} else{ goto _LL838;} _LL838: _LL1035: _temp1034= _temp812.f1; if((
unsigned int) _temp1034 > 4u?*(( int*) _temp1034) == Cyc_Absyn_ArrayType: 0){
_LL1041: _temp1040=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1034)->f1;
goto _LL1039; _LL1039: _temp1038=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1034)->f2; goto _LL1037; _LL1037: _temp1036=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1034)->f3; goto _LL1027;} else{ goto _LL840;} _LL1027: _temp1026= _temp812.f2;
if(( unsigned int) _temp1026 > 4u?*(( int*) _temp1026) == Cyc_Absyn_ArrayType: 0){
_LL1033: _temp1032=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1026)->f1;
goto _LL1031; _LL1031: _temp1030=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1026)->f2; goto _LL1029; _LL1029: _temp1028=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1026)->f3; goto _LL839;} else{ goto _LL840;} _LL840: _LL1061: _temp1060=
_temp812.f1; if(( unsigned int) _temp1060 > 4u?*(( int*) _temp1060) == Cyc_Absyn_FnType:
0){ _LL1063: _temp1062=(( struct Cyc_Absyn_FnType_struct*) _temp1060)->f1;
_LL1077: _temp1076= _temp1062.tvars; goto _LL1075; _LL1075: _temp1074= _temp1062.effect;
goto _LL1073; _LL1073: _temp1072=( void*) _temp1062.ret_typ; goto _LL1071;
_LL1071: _temp1070= _temp1062.args; goto _LL1069; _LL1069: _temp1068= _temp1062.c_varargs;
goto _LL1067; _LL1067: _temp1066= _temp1062.cyc_varargs; goto _LL1065; _LL1065:
_temp1064= _temp1062.attributes; goto _LL1043;} else{ goto _LL842;} _LL1043:
_temp1042= _temp812.f2; if(( unsigned int) _temp1042 > 4u?*(( int*) _temp1042)
== Cyc_Absyn_FnType: 0){ _LL1045: _temp1044=(( struct Cyc_Absyn_FnType_struct*)
_temp1042)->f1; _LL1059: _temp1058= _temp1044.tvars; goto _LL1057; _LL1057:
_temp1056= _temp1044.effect; goto _LL1055; _LL1055: _temp1054=( void*) _temp1044.ret_typ;
goto _LL1053; _LL1053: _temp1052= _temp1044.args; goto _LL1051; _LL1051:
_temp1050= _temp1044.c_varargs; goto _LL1049; _LL1049: _temp1048= _temp1044.cyc_varargs;
goto _LL1047; _LL1047: _temp1046= _temp1044.attributes; goto _LL841;} else{ goto
_LL842;} _LL842: _LL1083: _temp1082= _temp812.f1; if(( unsigned int) _temp1082 >
4u?*(( int*) _temp1082) == Cyc_Absyn_TupleType: 0){ _LL1085: _temp1084=(( struct
Cyc_Absyn_TupleType_struct*) _temp1082)->f1; goto _LL1079;} else{ goto _LL844;}
_LL1079: _temp1078= _temp812.f2; if(( unsigned int) _temp1078 > 4u?*(( int*)
_temp1078) == Cyc_Absyn_TupleType: 0){ _LL1081: _temp1080=(( struct Cyc_Absyn_TupleType_struct*)
_temp1078)->f1; goto _LL843;} else{ goto _LL844;} _LL844: _LL1091: _temp1090=
_temp812.f1; if(( unsigned int) _temp1090 > 4u?*(( int*) _temp1090) == Cyc_Absyn_AnonStructType:
0){ _LL1093: _temp1092=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1090)->f1;
goto _LL1087;} else{ goto _LL846;} _LL1087: _temp1086= _temp812.f2; if((
unsigned int) _temp1086 > 4u?*(( int*) _temp1086) == Cyc_Absyn_AnonStructType: 0){
_LL1089: _temp1088=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1086)->f1;
goto _LL845;} else{ goto _LL846;} _LL846: _LL1099: _temp1098= _temp812.f1; if((
unsigned int) _temp1098 > 4u?*(( int*) _temp1098) == Cyc_Absyn_AnonUnionType: 0){
_LL1101: _temp1100=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1098)->f1;
goto _LL1095;} else{ goto _LL848;} _LL1095: _temp1094= _temp812.f2; if((
unsigned int) _temp1094 > 4u?*(( int*) _temp1094) == Cyc_Absyn_AnonUnionType: 0){
_LL1097: _temp1096=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1094)->f1;
goto _LL847;} else{ goto _LL848;} _LL848: _LL1105: _temp1104= _temp812.f1; if(
_temp1104 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1103;} else{ goto _LL850;}
_LL1103: _temp1102= _temp812.f2; if( _temp1102 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL849;} else{ goto _LL850;} _LL850: _LL1111: _temp1110= _temp812.f1; if((
unsigned int) _temp1110 > 4u?*(( int*) _temp1110) == Cyc_Absyn_RgnHandleType: 0){
_LL1113: _temp1112=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1110)->f1;
goto _LL1107;} else{ goto _LL852;} _LL1107: _temp1106= _temp812.f2; if((
unsigned int) _temp1106 > 4u?*(( int*) _temp1106) == Cyc_Absyn_RgnHandleType: 0){
_LL1109: _temp1108=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1106)->f1;
goto _LL851;} else{ goto _LL852;} _LL852: _LL1117: _temp1116= _temp812.f1; if((
unsigned int) _temp1116 > 4u?*(( int*) _temp1116) == Cyc_Absyn_JoinEff: 0){
_LL1119: _temp1118=(( struct Cyc_Absyn_JoinEff_struct*) _temp1116)->f1; goto
_LL1115;} else{ goto _LL854;} _LL1115: _temp1114= _temp812.f2; goto _LL853;
_LL854: _LL1125: _temp1124= _temp812.f1; goto _LL1121; _LL1121: _temp1120=
_temp812.f2; if(( unsigned int) _temp1120 > 4u?*(( int*) _temp1120) == Cyc_Absyn_JoinEff:
0){ _LL1123: _temp1122=(( struct Cyc_Absyn_JoinEff_struct*) _temp1120)->f1; goto
_LL855;} else{ goto _LL856;} _LL856: _LL1129: _temp1128= _temp812.f1; if((
unsigned int) _temp1128 > 4u?*(( int*) _temp1128) == Cyc_Absyn_AccessEff: 0){
_LL1131: _temp1130=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp1128)->f1;
goto _LL1127;} else{ goto _LL858;} _LL1127: _temp1126= _temp812.f2; goto _LL857;
_LL858: _LL1137: _temp1136= _temp812.f1; goto _LL1133; _LL1133: _temp1132=
_temp812.f2; if(( unsigned int) _temp1132 > 4u?*(( int*) _temp1132) == Cyc_Absyn_AccessEff:
0){ _LL1135: _temp1134=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp1132)->f1;
goto _LL859;} else{ goto _LL860;} _LL860: goto _LL861; _LL815: Cyc_Tcutil_unify_it(
t2, t1); return; _LL817: return; _LL819: { struct _tagged_string* _temp1138=
_temp884->name; goto _LL1139; _LL1139: { struct _tagged_string* _temp1140=
_temp880->name; goto _LL1141; _LL1141: { int _temp1142=*(( int*) _check_null(
_temp884->identity)); goto _LL1143; _LL1143: { int _temp1144=*(( int*)
_check_null( _temp880->identity)); goto _LL1145; _LL1145: { void* _temp1146=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp884->kind);
goto _LL1147; _LL1147: { void* _temp1148=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( _temp880->kind); goto _LL1149; _LL1149: if( _temp1144 ==
_temp1142? Cyc_String_zstrptrcmp( _temp1138, _temp1140) == 0: 0){ if( _temp1146
!= _temp1148){(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ struct _tagged_string _temp1150=* _temp1138; struct
_tagged_string _temp1151= Cyc_Absynpp_kind2string( _temp1146); struct
_tagged_string _temp1152= Cyc_Absynpp_kind2string( _temp1148); xprintf("same type variable %.*s has kinds %.*s and %.*s",
_temp1150.last_plus_one - _temp1150.curr, _temp1150.curr, _temp1151.last_plus_one
- _temp1151.curr, _temp1151.curr, _temp1152.last_plus_one - _temp1152.curr,
_temp1152.curr);}));} return;} goto _LL813;}}}}}} _LL821: if((( _temp892 != 0?
_temp900 != 0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*) _check_null( _temp892),(
struct _tuple1*) _check_null( _temp900)) == 0: 0)? 1:( _temp892 == 0? _temp900
== 0: 0)){ Cyc_Tcutil_unify_list( _temp890, _temp898); return;} goto _LL813;
_LL823: if( Cyc_Absyn_qvar_cmp( _temp912, _temp906) == 0){ return;} goto _LL813;
_LL825: if((( _temp920 != 0? _temp928 != 0: 0)? Cyc_Absyn_qvar_cmp(( struct
_tuple1*) _check_null( _temp920),( struct _tuple1*) _check_null( _temp928)) == 0:
0)? 1:( _temp920 == 0? _temp928 == 0: 0)){ Cyc_Tcutil_unify_list( _temp918,
_temp926); return;} goto _LL813; _LL827: if( _temp952 == _temp940? 1: Cyc_Absyn_qvar_cmp(
_temp952->name, _temp940->name) == 0){ Cyc_Tcutil_unify_it( _temp934, _temp946);
Cyc_Tcutil_unify_list( _temp936, _temp948); return;} goto _LL813; _LL829: if((
_temp976 == _temp964? 1: Cyc_Absyn_qvar_cmp( _temp976->name, _temp964->name) ==
0)? _temp974 == _temp962? 1: Cyc_Absyn_qvar_cmp( _temp974->name, _temp962->name)
== 0: 0){ Cyc_Tcutil_unify_list( _temp958, _temp970); return;} goto _LL813;
_LL831: Cyc_Tcutil_unify_it( _temp990, _temp1004); Cyc_Tcutil_unify_it(
_temp1002, _temp988); Cyc_Tcutil_unify_tqual( _temp984, _temp998);(( void(*)(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_it_conrefs)( Cyc_Tcutil_boundscmp, _temp982, _temp996);{
void* _temp1153=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp982))->v; void* _temp1159; _LL1155: if((
unsigned int) _temp1153 > 1u?*(( int*) _temp1153) == Cyc_Absyn_Eq_constr: 0){
_LL1160: _temp1159=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1153)->f1;
if( _temp1159 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1156;} else{ goto _LL1157;}}
else{ goto _LL1157;} _LL1157: goto _LL1158; _LL1156: return; _LL1158: goto
_LL1154; _LL1154:;}(( void(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)( Cyc_Core_intcmp,
_temp986, _temp1000); return; _LL833: if( _temp1010 == _temp1016? _temp1008 ==
_temp1014: 0){ return;} goto _LL813; _LL835: return; _LL837: return; _LL839: Cyc_Tcutil_unify_it(
_temp1032, _temp1040); Cyc_Tcutil_unify_tqual( _temp1030, _temp1038); if(
_temp1036 == _temp1028){ return;} if( _temp1036 == 0? 1: _temp1028 == 0){ goto
_LL813;} if( Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*) _check_null(
_temp1036)) == Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1028))){ return;} goto _LL813; _LL841: { int done= 0;{ struct
_RegionHandle _temp1161= _new_region(); struct _RegionHandle* rgn=& _temp1161;
_push_region( rgn);{ struct Cyc_List_List* inst= 0; while( _temp1058 != 0) { if(
_temp1076 == 0){ break;} inst=({ struct Cyc_List_List* _temp1162=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1162->hd=( void*)({
struct _tuple5* _temp1163=( struct _tuple5*) _region_malloc( rgn, sizeof( struct
_tuple5)); _temp1163->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp1076))->hd; _temp1163->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1164=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1164[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1165; _temp1165.tag= Cyc_Absyn_VarType;
_temp1165.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp1058))->hd; _temp1165;}); _temp1164;}); _temp1163;}); _temp1162->tl= inst;
_temp1162;}); _temp1058=(( struct Cyc_List_List*) _check_null( _temp1058))->tl;
_temp1076=(( struct Cyc_List_List*) _check_null( _temp1076))->tl;} if( _temp1076
!= 0){ _npop_handler( 0u); goto _LL813;} if( inst != 0){ Cyc_Tcutil_unify_it((
void*)({ struct Cyc_Absyn_FnType_struct* _temp1166=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1166[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1167; _temp1167.tag= Cyc_Absyn_FnType; _temp1167.f1=({ struct Cyc_Absyn_FnInfo
_temp1168; _temp1168.tvars= 0; _temp1168.effect= _temp1056; _temp1168.ret_typ=(
void*) _temp1054; _temp1168.args= _temp1052; _temp1168.c_varargs= _temp1050;
_temp1168.cyc_varargs= _temp1048; _temp1168.attributes= _temp1046; _temp1168;});
_temp1167;}); _temp1166;}), Cyc_Tcutil_rsubstitute( rgn, inst,( void*)({ struct
Cyc_Absyn_FnType_struct* _temp1169=( struct Cyc_Absyn_FnType_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_FnType_struct)); _temp1169[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1170; _temp1170.tag= Cyc_Absyn_FnType; _temp1170.f1=({ struct Cyc_Absyn_FnInfo
_temp1171; _temp1171.tvars= 0; _temp1171.effect= _temp1074; _temp1171.ret_typ=(
void*) _temp1072; _temp1171.args= _temp1070; _temp1171.c_varargs= _temp1068;
_temp1171.cyc_varargs= _temp1066; _temp1171.attributes= _temp1064; _temp1171;});
_temp1170;}); _temp1169;}))); done= 1;}}; _pop_region(& _temp1161);} if( done){
return;} Cyc_Tcutil_unify_it( _temp1054, _temp1072); for( 0; _temp1052 != 0?
_temp1070 != 0: 0;( _temp1052=(( struct Cyc_List_List*) _check_null( _temp1052))->tl,
_temp1070=(( struct Cyc_List_List*) _check_null( _temp1070))->tl)){ Cyc_Tcutil_unify_tqual((*((
struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp1052))->hd)).f2,(*((
struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp1070))->hd)).f2);
Cyc_Tcutil_unify_it((*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null(
_temp1052))->hd)).f3,(*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null(
_temp1070))->hd)).f3);} if( _temp1052 != 0? 1: _temp1070 != 0){ goto _LL813;}
if( _temp1050 != _temp1068){ goto _LL813;}{ int bad_cyc_vararg= 0;{ struct
_tuple11 _temp1173=({ struct _tuple11 _temp1172; _temp1172.f1= _temp1048;
_temp1172.f2= _temp1066; _temp1172;}); struct Cyc_Absyn_VarargInfo* _temp1183;
struct Cyc_Absyn_VarargInfo* _temp1185; struct Cyc_Absyn_VarargInfo* _temp1187;
struct Cyc_Absyn_VarargInfo* _temp1189; struct Cyc_Absyn_VarargInfo* _temp1191;
struct Cyc_Absyn_VarargInfo* _temp1193; struct Cyc_Absyn_VarargInfo* _temp1195;
struct Cyc_Absyn_VarargInfo _temp1197; int _temp1198; void* _temp1200; void*
_temp1202; struct Cyc_Absyn_Tqual _temp1204; struct Cyc_Core_Opt* _temp1206;
struct Cyc_Absyn_VarargInfo* _temp1208; struct Cyc_Absyn_VarargInfo _temp1210;
int _temp1211; void* _temp1213; void* _temp1215; struct Cyc_Absyn_Tqual
_temp1217; struct Cyc_Core_Opt* _temp1219; _LL1175: _LL1186: _temp1185=
_temp1173.f1; if( _temp1185 == 0){ goto _LL1184;} else{ goto _LL1177;} _LL1184:
_temp1183= _temp1173.f2; if( _temp1183 == 0){ goto _LL1176;} else{ goto _LL1177;}
_LL1177: _LL1190: _temp1189= _temp1173.f1; if( _temp1189 == 0){ goto _LL1188;}
else{ goto _LL1179;} _LL1188: _temp1187= _temp1173.f2; goto _LL1178; _LL1179:
_LL1194: _temp1193= _temp1173.f1; goto _LL1192; _LL1192: _temp1191= _temp1173.f2;
if( _temp1191 == 0){ goto _LL1180;} else{ goto _LL1181;} _LL1181: _LL1209:
_temp1208= _temp1173.f1; if( _temp1208 == 0){ goto _LL1174;} else{ _temp1210=*
_temp1208; _LL1220: _temp1219= _temp1210.name; goto _LL1218; _LL1218: _temp1217=
_temp1210.tq; goto _LL1216; _LL1216: _temp1215=( void*) _temp1210.type; goto
_LL1214; _LL1214: _temp1213=( void*) _temp1210.rgn; goto _LL1212; _LL1212:
_temp1211= _temp1210.inject; goto _LL1196;} _LL1196: _temp1195= _temp1173.f2;
if( _temp1195 == 0){ goto _LL1174;} else{ _temp1197=* _temp1195; _LL1207:
_temp1206= _temp1197.name; goto _LL1205; _LL1205: _temp1204= _temp1197.tq; goto
_LL1203; _LL1203: _temp1202=( void*) _temp1197.type; goto _LL1201; _LL1201:
_temp1200=( void*) _temp1197.rgn; goto _LL1199; _LL1199: _temp1198= _temp1197.inject;
goto _LL1182;} _LL1176: goto _LL1174; _LL1178: goto _LL1180; _LL1180:
bad_cyc_vararg= 1; goto _LL1174; _LL1182: Cyc_Tcutil_unify_tqual( _temp1217,
_temp1204); Cyc_Tcutil_unify_it( _temp1215, _temp1202); Cyc_Tcutil_unify_it(
_temp1213, _temp1200); if( _temp1211 != _temp1198){ bad_cyc_vararg= 1;} goto
_LL1174; _LL1174:;} if( bad_cyc_vararg){ goto _LL813;}{ int bad_effect= 0;{
struct _tuple12 _temp1222=({ struct _tuple12 _temp1221; _temp1221.f1= _temp1056;
_temp1221.f2= _temp1074; _temp1221;}); struct Cyc_Core_Opt* _temp1232; struct
Cyc_Core_Opt* _temp1234; struct Cyc_Core_Opt* _temp1236; struct Cyc_Core_Opt*
_temp1238; struct Cyc_Core_Opt* _temp1240; struct Cyc_Core_Opt* _temp1242;
_LL1224: _LL1235: _temp1234= _temp1222.f1; if( _temp1234 == 0){ goto _LL1233;}
else{ goto _LL1226;} _LL1233: _temp1232= _temp1222.f2; if( _temp1232 == 0){ goto
_LL1225;} else{ goto _LL1226;} _LL1226: _LL1239: _temp1238= _temp1222.f1; if(
_temp1238 == 0){ goto _LL1237;} else{ goto _LL1228;} _LL1237: _temp1236=
_temp1222.f2; goto _LL1227; _LL1228: _LL1243: _temp1242= _temp1222.f1; goto
_LL1241; _LL1241: _temp1240= _temp1222.f2; if( _temp1240 == 0){ goto _LL1229;}
else{ goto _LL1230;} _LL1230: goto _LL1231; _LL1225: goto _LL1223; _LL1227: goto
_LL1229; _LL1229: bad_effect= 1; goto _LL1223; _LL1231: bad_effect= ! Cyc_Tcutil_unify_effect((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1056))->v,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1074))->v); goto _LL1223; _LL1223:;} if( bad_effect){ goto
_LL813;} if( ! Cyc_Tcutil_same_atts( _temp1064, _temp1046)){ goto _LL813;}
return;}}} _LL843: for( 0; _temp1080 != 0? _temp1084 != 0: 0;( _temp1080=((
struct Cyc_List_List*) _check_null( _temp1080))->tl, _temp1084=(( struct Cyc_List_List*)
_check_null( _temp1084))->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1080))->hd)).f1,(*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1084))->hd)).f1); Cyc_Tcutil_unify_it((*((
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1080))->hd)).f2,(*((
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1084))->hd)).f2);}
if( _temp1080 == 0? _temp1084 == 0: 0){ return;} goto _LL813; _LL845: _temp1100=
_temp1092; _temp1096= _temp1088; goto _LL847; _LL847: for( 0; _temp1096 != 0?
_temp1100 != 0: 0;( _temp1096=(( struct Cyc_List_List*) _check_null( _temp1096))->tl,
_temp1100=(( struct Cyc_List_List*) _check_null( _temp1100))->tl)){ struct Cyc_Absyn_Structfield*
_temp1244=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1096))->hd; goto _LL1245; _LL1245: { struct Cyc_Absyn_Structfield*
_temp1246=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1100))->hd; goto _LL1247; _LL1247: if( Cyc_String_zstrptrcmp( _temp1244->name,
_temp1246->name) != 0){( void) _throw(( void*) Cyc_Tcutil_Unify);} Cyc_Tcutil_unify_tqual(
_temp1244->tq, _temp1246->tq); Cyc_Tcutil_unify_it(( void*) _temp1244->type,(
void*) _temp1246->type); if((( ! Cyc_Tcutil_same_atts( _temp1244->attributes,
_temp1246->attributes)? 1:( _temp1244->width != 0? _temp1246->width == 0: 0))? 1:(
_temp1246->width != 0? _temp1244->width == 0: 0))? 1:(( _temp1244->width != 0?
_temp1246->width != 0: 0)? Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp1244->width))->v) != Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp1246->width))->v):
0)){( void) _throw(( void*) Cyc_Tcutil_Unify);}}} if( _temp1096 == 0? _temp1100
== 0: 0){ return;} goto _LL813; _LL849: return; _LL851: Cyc_Tcutil_unify_it(
_temp1112, _temp1108); return; _LL853: goto _LL855; _LL855: goto _LL857; _LL857:
goto _LL859; _LL859: if( Cyc_Tcutil_unify_effect( t1, t2)){ return;} else{ goto
_LL813;} _LL861: goto _LL813; _LL813:;}( void) _throw(( void*) Cyc_Tcutil_Unify);}
int Cyc_Tcutil_is_arithmetic_type( void* t){ void* _temp1248= Cyc_Tcutil_compress(
t); void* _temp1260; void* _temp1262; struct Cyc_Absyn_Enumdecl* _temp1264;
struct _tuple1* _temp1266; _LL1250: if(( unsigned int) _temp1248 > 4u?*(( int*)
_temp1248) == Cyc_Absyn_IntType: 0){ _LL1263: _temp1262=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1248)->f1; goto _LL1261; _LL1261: _temp1260=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1248)->f2; goto _LL1251;} else{ goto _LL1252;} _LL1252: if( _temp1248 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1253;} else{ goto _LL1254;} _LL1254: if(
_temp1248 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1255;} else{ goto _LL1256;}
_LL1256: if(( unsigned int) _temp1248 > 4u?*(( int*) _temp1248) == Cyc_Absyn_EnumType:
0){ _LL1267: _temp1266=(( struct Cyc_Absyn_EnumType_struct*) _temp1248)->f1;
goto _LL1265; _LL1265: _temp1264=(( struct Cyc_Absyn_EnumType_struct*) _temp1248)->f2;
goto _LL1257;} else{ goto _LL1258;} _LL1258: goto _LL1259; _LL1251: return 1;
_LL1253: return 1; _LL1255: return 1; _LL1257: return 1; _LL1259: return 0;
_LL1249:;} int Cyc_Tcutil_will_lose_precision( void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8 _temp1269=({ struct _tuple8
_temp1268; _temp1268.f1= t1; _temp1268.f2= t2; _temp1268;}); void* _temp1291;
void* _temp1293; void* _temp1295; void* _temp1297; void* _temp1299; void*
_temp1301; void* _temp1303; void* _temp1305; void* _temp1307; void* _temp1309;
void* _temp1311; void* _temp1313; void* _temp1315; void* _temp1317; void*
_temp1319; void* _temp1321; void* _temp1323; void* _temp1325; void* _temp1327;
void* _temp1329; void* _temp1331; void* _temp1333; void* _temp1335; void*
_temp1337; void* _temp1339; void* _temp1341; void* _temp1343; void* _temp1345;
void* _temp1347; void* _temp1349; void* _temp1351; void* _temp1353; void*
_temp1355; void* _temp1357; void* _temp1359; void* _temp1361; void* _temp1363;
void* _temp1365; void* _temp1367; void* _temp1369; void* _temp1371; void*
_temp1373; _LL1271: _LL1294: _temp1293= _temp1269.f1; if( _temp1293 ==( void*)
Cyc_Absyn_DoubleType){ goto _LL1292;} else{ goto _LL1273;} _LL1292: _temp1291=
_temp1269.f2; if( _temp1291 ==( void*) Cyc_Absyn_FloatType){ goto _LL1272;}
else{ goto _LL1273;} _LL1273: _LL1302: _temp1301= _temp1269.f1; if( _temp1301 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL1296;} else{ goto _LL1275;} _LL1296:
_temp1295= _temp1269.f2; if(( unsigned int) _temp1295 > 4u?*(( int*) _temp1295)
== Cyc_Absyn_IntType: 0){ _LL1300: _temp1299=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1295)->f1; goto _LL1298; _LL1298: _temp1297=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1295)->f2; goto _LL1274;} else{ goto _LL1275;} _LL1275: _LL1310: _temp1309=
_temp1269.f1; if( _temp1309 ==( void*) Cyc_Absyn_FloatType){ goto _LL1304;}
else{ goto _LL1277;} _LL1304: _temp1303= _temp1269.f2; if(( unsigned int)
_temp1303 > 4u?*(( int*) _temp1303) == Cyc_Absyn_IntType: 0){ _LL1308: _temp1307=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1303)->f1; goto _LL1306; _LL1306:
_temp1305=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1303)->f2; goto
_LL1276;} else{ goto _LL1277;} _LL1277: _LL1318: _temp1317= _temp1269.f1; if((
unsigned int) _temp1317 > 4u?*(( int*) _temp1317) == Cyc_Absyn_IntType: 0){
_LL1322: _temp1321=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1317)->f1;
goto _LL1320; _LL1320: _temp1319=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1317)->f2; if( _temp1319 ==( void*) Cyc_Absyn_B8){ goto _LL1312;} else{
goto _LL1279;}} else{ goto _LL1279;} _LL1312: _temp1311= _temp1269.f2; if((
unsigned int) _temp1311 > 4u?*(( int*) _temp1311) == Cyc_Absyn_IntType: 0){
_LL1316: _temp1315=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1311)->f1;
goto _LL1314; _LL1314: _temp1313=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1311)->f2; if( _temp1313 ==( void*) Cyc_Absyn_B8){ goto _LL1278;} else{
goto _LL1279;}} else{ goto _LL1279;} _LL1279: _LL1326: _temp1325= _temp1269.f1;
if(( unsigned int) _temp1325 > 4u?*(( int*) _temp1325) == Cyc_Absyn_IntType: 0){
_LL1330: _temp1329=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1325)->f1;
goto _LL1328; _LL1328: _temp1327=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1325)->f2; if( _temp1327 ==( void*) Cyc_Absyn_B8){ goto _LL1324;} else{
goto _LL1281;}} else{ goto _LL1281;} _LL1324: _temp1323= _temp1269.f2; goto
_LL1280; _LL1281: _LL1334: _temp1333= _temp1269.f1; if(( unsigned int) _temp1333
> 4u?*(( int*) _temp1333) == Cyc_Absyn_IntType: 0){ _LL1338: _temp1337=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1333)->f1; goto _LL1336; _LL1336:
_temp1335=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1333)->f2; if(
_temp1335 ==( void*) Cyc_Absyn_B4){ goto _LL1332;} else{ goto _LL1283;}} else{
goto _LL1283;} _LL1332: _temp1331= _temp1269.f2; if( _temp1331 ==( void*) Cyc_Absyn_FloatType){
goto _LL1282;} else{ goto _LL1283;} _LL1283: _LL1346: _temp1345= _temp1269.f1;
if(( unsigned int) _temp1345 > 4u?*(( int*) _temp1345) == Cyc_Absyn_IntType: 0){
_LL1350: _temp1349=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1345)->f1;
goto _LL1348; _LL1348: _temp1347=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1345)->f2; if( _temp1347 ==( void*) Cyc_Absyn_B4){ goto _LL1340;} else{
goto _LL1285;}} else{ goto _LL1285;} _LL1340: _temp1339= _temp1269.f2; if((
unsigned int) _temp1339 > 4u?*(( int*) _temp1339) == Cyc_Absyn_IntType: 0){
_LL1344: _temp1343=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1339)->f1;
goto _LL1342; _LL1342: _temp1341=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1339)->f2; if( _temp1341 ==( void*) Cyc_Absyn_B2){ goto _LL1284;} else{
goto _LL1285;}} else{ goto _LL1285;} _LL1285: _LL1358: _temp1357= _temp1269.f1;
if(( unsigned int) _temp1357 > 4u?*(( int*) _temp1357) == Cyc_Absyn_IntType: 0){
_LL1362: _temp1361=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1357)->f1;
goto _LL1360; _LL1360: _temp1359=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1357)->f2; if( _temp1359 ==( void*) Cyc_Absyn_B4){ goto _LL1352;} else{
goto _LL1287;}} else{ goto _LL1287;} _LL1352: _temp1351= _temp1269.f2; if((
unsigned int) _temp1351 > 4u?*(( int*) _temp1351) == Cyc_Absyn_IntType: 0){
_LL1356: _temp1355=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1351)->f1;
goto _LL1354; _LL1354: _temp1353=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1351)->f2; if( _temp1353 ==( void*) Cyc_Absyn_B1){ goto _LL1286;} else{
goto _LL1287;}} else{ goto _LL1287;} _LL1287: _LL1370: _temp1369= _temp1269.f1;
if(( unsigned int) _temp1369 > 4u?*(( int*) _temp1369) == Cyc_Absyn_IntType: 0){
_LL1374: _temp1373=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1369)->f1;
goto _LL1372; _LL1372: _temp1371=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1369)->f2; if( _temp1371 ==( void*) Cyc_Absyn_B2){ goto _LL1364;} else{
goto _LL1289;}} else{ goto _LL1289;} _LL1364: _temp1363= _temp1269.f2; if((
unsigned int) _temp1363 > 4u?*(( int*) _temp1363) == Cyc_Absyn_IntType: 0){
_LL1368: _temp1367=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1363)->f1;
goto _LL1366; _LL1366: _temp1365=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1363)->f2; if( _temp1365 ==( void*) Cyc_Absyn_B1){ goto _LL1288;} else{
goto _LL1289;}} else{ goto _LL1289;} _LL1289: goto _LL1290; _LL1272: return 1;
_LL1274: return 1; _LL1276: return 1; _LL1278: return 0; _LL1280: return 1;
_LL1282: return 1; _LL1284: return 1; _LL1286: return 1; _LL1288: return 1;
_LL1290: return 0; _LL1270:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{
struct Cyc_List_List* el= es; for( 0; el != 0; el=(( struct Cyc_List_List*)
_check_null( el))->tl){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);
if( Cyc_Tcutil_is_arithmetic_type( t1)){ if( max_arith_type == 0? 1: Cyc_Tcutil_will_lose_precision(
t1,( void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){
max_arith_type=({ struct Cyc_Core_Opt* _temp1375=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1375->v=( void*) t1; _temp1375;});}}}}
if( max_arith_type != 0){ if( ! Cyc_Tcutil_unify( t,( void*)(( struct Cyc_Core_Opt*)
_check_null( max_arith_type))->v)){ return 0;}}{ struct Cyc_List_List* el= es;
for( 0; el != 0; el=(( struct Cyc_List_List*) _check_null( el))->tl){ if( ! Cyc_Tcutil_coerce_assign(
te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd, t)){
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
el))->hd)->loc,( struct _tagged_string)({ struct _tagged_string _temp1376= Cyc_Absynpp_typ2string(
t); struct _tagged_string _temp1377= Cyc_Absynpp_typ2string(( void*)(( struct
Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( el))->hd)->topt))->v); xprintf("type mismatch: expecting %.*s but found %.*s",
_temp1376.last_plus_one - _temp1376.curr, _temp1376.curr, _temp1377.last_plus_one
- _temp1377.curr, _temp1377.curr);})); return 0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e)){ void* _temp1378= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); struct Cyc_Absyn_PtrInfo _temp1384; _LL1380: if((
unsigned int) _temp1378 > 4u?*(( int*) _temp1378) == Cyc_Absyn_PointerType: 0){
_LL1385: _temp1384=(( struct Cyc_Absyn_PointerType_struct*) _temp1378)->f1; goto
_LL1381;} else{ goto _LL1382;} _LL1382: goto _LL1383; _LL1381: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); goto _LL1379; _LL1383: return 0; _LL1379:;} return 1;}
int Cyc_Tcutil_is_integral_type( void* t){ void* _temp1386= Cyc_Tcutil_compress(
t); void* _temp1394; void* _temp1396; struct Cyc_Absyn_Enumdecl* _temp1398;
struct _tuple1* _temp1400; _LL1388: if(( unsigned int) _temp1386 > 4u?*(( int*)
_temp1386) == Cyc_Absyn_IntType: 0){ _LL1397: _temp1396=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1386)->f1; goto _LL1395; _LL1395: _temp1394=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1386)->f2; goto _LL1389;} else{ goto _LL1390;} _LL1390: if(( unsigned int)
_temp1386 > 4u?*(( int*) _temp1386) == Cyc_Absyn_EnumType: 0){ _LL1401:
_temp1400=(( struct Cyc_Absyn_EnumType_struct*) _temp1386)->f1; goto _LL1399;
_LL1399: _temp1398=(( struct Cyc_Absyn_EnumType_struct*) _temp1386)->f2; goto
_LL1391;} else{ goto _LL1392;} _LL1392: goto _LL1393; _LL1389: return 1; _LL1391:
return 1; _LL1393: return 0; _LL1387:;} int Cyc_Tcutil_coerce_uint_typ( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_uint_t)){ return 1;}
if( Cyc_Tcutil_is_integral_type(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){
if( Cyc_Tcutil_will_lose_precision(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, Cyc_Absyn_uint_t)){ Cyc_Tcutil_warn( e->loc,({ unsigned char*
_temp1402=( unsigned char*)"integral size mismatch; conversion supplied"; struct
_tagged_string _temp1403; _temp1403.curr= _temp1402; _temp1403.base= _temp1402;
_temp1403.last_plus_one= _temp1402 + 44; _temp1403;}));} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;}
if( Cyc_Tcutil_is_integral_type(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){
if( Cyc_Tcutil_will_lose_precision(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, Cyc_Absyn_sint_t)){ Cyc_Tcutil_warn( e->loc,({ unsigned char*
_temp1404=( unsigned char*)"integral size mismatch; conversion supplied"; struct
_tagged_string _temp1405; _temp1405.curr= _temp1404; _temp1405.base= _temp1404;
_temp1405.last_plus_one= _temp1404 + 44; _temp1405;}));} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); return 1;} return 0;} int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8
_temp1407=({ struct _tuple8 _temp1406; _temp1406.f1= t1; _temp1406.f2= t2;
_temp1406;}); void* _temp1419; struct Cyc_Absyn_PtrInfo _temp1421; void*
_temp1423; struct Cyc_Absyn_PtrInfo _temp1425; void* _temp1427; struct Cyc_Absyn_Exp*
_temp1429; struct Cyc_Absyn_Tqual _temp1431; void* _temp1433; void* _temp1435;
struct Cyc_Absyn_Exp* _temp1437; struct Cyc_Absyn_Tqual _temp1439; void*
_temp1441; void* _temp1443; struct Cyc_Absyn_TunionInfo _temp1445; void*
_temp1447; struct Cyc_List_List* _temp1449; void* _temp1451; struct Cyc_Absyn_Tuniondecl*
_temp1453; void* _temp1455; struct Cyc_Absyn_TunionFieldInfo _temp1457; struct
Cyc_List_List* _temp1459; void* _temp1461; struct Cyc_Absyn_Tunionfield*
_temp1463; struct Cyc_Absyn_Tuniondecl* _temp1465; void* _temp1467; struct Cyc_Absyn_TunionInfo
_temp1469; void* _temp1471; struct Cyc_List_List* _temp1473; void* _temp1475;
struct Cyc_Absyn_Tuniondecl* _temp1477; void* _temp1479; struct Cyc_Absyn_PtrInfo
_temp1481; struct Cyc_Absyn_Conref* _temp1483; struct Cyc_Absyn_Tqual _temp1485;
struct Cyc_Absyn_Conref* _temp1487; void* _temp1489; void* _temp1491; _LL1409:
_LL1424: _temp1423= _temp1407.f1; if(( unsigned int) _temp1423 > 4u?*(( int*)
_temp1423) == Cyc_Absyn_PointerType: 0){ _LL1426: _temp1425=(( struct Cyc_Absyn_PointerType_struct*)
_temp1423)->f1; goto _LL1420;} else{ goto _LL1411;} _LL1420: _temp1419=
_temp1407.f2; if(( unsigned int) _temp1419 > 4u?*(( int*) _temp1419) == Cyc_Absyn_PointerType:
0){ _LL1422: _temp1421=(( struct Cyc_Absyn_PointerType_struct*) _temp1419)->f1;
goto _LL1410;} else{ goto _LL1411;} _LL1411: _LL1436: _temp1435= _temp1407.f1;
if(( unsigned int) _temp1435 > 4u?*(( int*) _temp1435) == Cyc_Absyn_ArrayType: 0){
_LL1442: _temp1441=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1435)->f1;
goto _LL1440; _LL1440: _temp1439=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1435)->f2; goto _LL1438; _LL1438: _temp1437=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1435)->f3; goto _LL1428;} else{ goto _LL1413;} _LL1428: _temp1427=
_temp1407.f2; if(( unsigned int) _temp1427 > 4u?*(( int*) _temp1427) == Cyc_Absyn_ArrayType:
0){ _LL1434: _temp1433=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1427)->f1;
goto _LL1432; _LL1432: _temp1431=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1427)->f2; goto _LL1430; _LL1430: _temp1429=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1427)->f3; goto _LL1412;} else{ goto _LL1413;} _LL1413: _LL1456: _temp1455=
_temp1407.f1; if(( unsigned int) _temp1455 > 4u?*(( int*) _temp1455) == Cyc_Absyn_TunionFieldType:
0){ _LL1458: _temp1457=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1455)->f1;
_LL1462: _temp1461=( void*) _temp1457.field_info; if(*(( int*) _temp1461) == Cyc_Absyn_KnownTunionfield){
_LL1466: _temp1465=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1461)->f1;
goto _LL1464; _LL1464: _temp1463=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1461)->f2; goto _LL1460;} else{ goto _LL1415;} _LL1460: _temp1459=
_temp1457.targs; goto _LL1444;} else{ goto _LL1415;} _LL1444: _temp1443=
_temp1407.f2; if(( unsigned int) _temp1443 > 4u?*(( int*) _temp1443) == Cyc_Absyn_TunionType:
0){ _LL1446: _temp1445=(( struct Cyc_Absyn_TunionType_struct*) _temp1443)->f1;
_LL1452: _temp1451=( void*) _temp1445.tunion_info; if(*(( int*) _temp1451) ==
Cyc_Absyn_KnownTunion){ _LL1454: _temp1453=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1451)->f1; goto _LL1450;} else{ goto _LL1415;} _LL1450: _temp1449=
_temp1445.targs; goto _LL1448; _LL1448: _temp1447=( void*) _temp1445.rgn; goto
_LL1414;} else{ goto _LL1415;} _LL1415: _LL1480: _temp1479= _temp1407.f1; if((
unsigned int) _temp1479 > 4u?*(( int*) _temp1479) == Cyc_Absyn_PointerType: 0){
_LL1482: _temp1481=(( struct Cyc_Absyn_PointerType_struct*) _temp1479)->f1;
_LL1492: _temp1491=( void*) _temp1481.elt_typ; goto _LL1490; _LL1490: _temp1489=(
void*) _temp1481.rgn_typ; goto _LL1488; _LL1488: _temp1487= _temp1481.nullable;
goto _LL1486; _LL1486: _temp1485= _temp1481.tq; goto _LL1484; _LL1484: _temp1483=
_temp1481.bounds; goto _LL1468;} else{ goto _LL1417;} _LL1468: _temp1467=
_temp1407.f2; if(( unsigned int) _temp1467 > 4u?*(( int*) _temp1467) == Cyc_Absyn_TunionType:
0){ _LL1470: _temp1469=(( struct Cyc_Absyn_TunionType_struct*) _temp1467)->f1;
_LL1476: _temp1475=( void*) _temp1469.tunion_info; if(*(( int*) _temp1475) ==
Cyc_Absyn_KnownTunion){ _LL1478: _temp1477=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1475)->f1; goto _LL1474;} else{ goto _LL1417;} _LL1474: _temp1473=
_temp1469.targs; goto _LL1472; _LL1472: _temp1471=( void*) _temp1469.rgn; goto
_LL1416;} else{ goto _LL1417;} _LL1417: goto _LL1418; _LL1410: { int okay= 1;
if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1425.nullable, _temp1421.nullable)){
void* _temp1493=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp1425.nullable))->v; int _temp1499; _LL1495:
if(( unsigned int) _temp1493 > 1u?*(( int*) _temp1493) == Cyc_Absyn_Eq_constr: 0){
_LL1500: _temp1499=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1493)->f1;
goto _LL1496;} else{ goto _LL1497;} _LL1497: goto _LL1498; _LL1496: okay= !
_temp1499; goto _LL1494; _LL1498:( void) _throw((( void*(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp1501=(
unsigned char*)"silent_castable conref not eq"; struct _tagged_string _temp1502;
_temp1502.curr= _temp1501; _temp1502.base= _temp1501; _temp1502.last_plus_one=
_temp1501 + 30; _temp1502;}))); _LL1494:;} if( !(( int(*)( int(* cmp)( void*,
void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp1425.bounds, _temp1421.bounds)){ struct _tuple8
_temp1504=({ struct _tuple8 _temp1503; _temp1503.f1=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1425.bounds))->v;
_temp1503.f2=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp1421.bounds))->v; _temp1503;}); void* _temp1514;
void* _temp1516; void* _temp1518; void* _temp1520; struct Cyc_Absyn_Exp*
_temp1522; void* _temp1524; void* _temp1526; struct Cyc_Absyn_Exp* _temp1528;
void* _temp1530; void* _temp1532; struct Cyc_Absyn_Exp* _temp1534; void*
_temp1536; void* _temp1538; void* _temp1540; void* _temp1542; _LL1506: _LL1519:
_temp1518= _temp1504.f1; if(( unsigned int) _temp1518 > 1u?*(( int*) _temp1518)
== Cyc_Absyn_Eq_constr: 0){ _LL1521: _temp1520=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1518)->f1; if(( unsigned int) _temp1520 > 1u?*(( int*) _temp1520) == Cyc_Absyn_Upper_b:
0){ _LL1523: _temp1522=(( struct Cyc_Absyn_Upper_b_struct*) _temp1520)->f1; goto
_LL1515;} else{ goto _LL1508;}} else{ goto _LL1508;} _LL1515: _temp1514=
_temp1504.f2; if(( unsigned int) _temp1514 > 1u?*(( int*) _temp1514) == Cyc_Absyn_Eq_constr:
0){ _LL1517: _temp1516=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1514)->f1;
if( _temp1516 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1507;} else{ goto _LL1508;}}
else{ goto _LL1508;} _LL1508: _LL1531: _temp1530= _temp1504.f1; if((
unsigned int) _temp1530 > 1u?*(( int*) _temp1530) == Cyc_Absyn_Eq_constr: 0){
_LL1533: _temp1532=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1530)->f1;
if(( unsigned int) _temp1532 > 1u?*(( int*) _temp1532) == Cyc_Absyn_Upper_b: 0){
_LL1535: _temp1534=(( struct Cyc_Absyn_Upper_b_struct*) _temp1532)->f1; goto
_LL1525;} else{ goto _LL1510;}} else{ goto _LL1510;} _LL1525: _temp1524=
_temp1504.f2; if(( unsigned int) _temp1524 > 1u?*(( int*) _temp1524) == Cyc_Absyn_Eq_constr:
0){ _LL1527: _temp1526=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1524)->f1;
if(( unsigned int) _temp1526 > 1u?*(( int*) _temp1526) == Cyc_Absyn_Upper_b: 0){
_LL1529: _temp1528=(( struct Cyc_Absyn_Upper_b_struct*) _temp1526)->f1; goto
_LL1509;} else{ goto _LL1510;}} else{ goto _LL1510;} _LL1510: _LL1541: _temp1540=
_temp1504.f1; if(( unsigned int) _temp1540 > 1u?*(( int*) _temp1540) == Cyc_Absyn_Eq_constr:
0){ _LL1543: _temp1542=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1540)->f1;
if( _temp1542 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1537;} else{ goto _LL1512;}}
else{ goto _LL1512;} _LL1537: _temp1536= _temp1504.f2; if(( unsigned int)
_temp1536 > 1u?*(( int*) _temp1536) == Cyc_Absyn_Eq_constr: 0){ _LL1539:
_temp1538=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1536)->f1; if(
_temp1538 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1511;} else{ goto _LL1512;}}
else{ goto _LL1512;} _LL1512: goto _LL1513; _LL1507: okay= 1; goto _LL1505;
_LL1509: okay= okay? Cyc_Evexp_eval_const_uint_exp( _temp1534) >= Cyc_Evexp_eval_const_uint_exp(
_temp1528): 0; Cyc_Tcutil_warn( loc,({ unsigned char* _temp1544=( unsigned char*)"implicit cast to shorter array";
struct _tagged_string _temp1545; _temp1545.curr= _temp1544; _temp1545.base=
_temp1544; _temp1545.last_plus_one= _temp1544 + 31; _temp1545;})); goto _LL1505;
_LL1511: okay= 1; goto _LL1505; _LL1513: okay= 0; goto _LL1505; _LL1505:;}
return(( okay? Cyc_Tcutil_unify(( void*) _temp1425.elt_typ,( void*) _temp1421.elt_typ):
0)? Cyc_Tcutil_unify(( void*) _temp1425.rgn_typ,( void*) _temp1421.rgn_typ): 0)?
!( _temp1425.tq).q_const? 1:( _temp1421.tq).q_const: 0;} _LL1412: { int okay;
okay=( _temp1437 != 0? _temp1429 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1437)) == Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1429)): 0; return( okay? Cyc_Tcutil_unify(
_temp1441, _temp1433): 0)? ! _temp1439.q_const? 1: _temp1431.q_const: 0;}
_LL1414: if(( _temp1465 == _temp1453? 1: Cyc_Absyn_qvar_cmp( _temp1465->name,
_temp1453->name) == 0)? _temp1463->typs == 0: 0){ for( 0; _temp1459 != 0?
_temp1449 != 0: 0;( _temp1459=(( struct Cyc_List_List*) _check_null( _temp1459))->tl,
_temp1449=(( struct Cyc_List_List*) _check_null( _temp1449))->tl)){ if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_List_List*) _check_null( _temp1459))->hd,( void*)(( struct
Cyc_List_List*) _check_null( _temp1449))->hd)){ break;}} if( _temp1459 == 0?
_temp1449 == 0: 0){ return 1;}} return 0; _LL1416:{ void* _temp1546= Cyc_Tcutil_compress(
_temp1491); struct Cyc_Absyn_TunionFieldInfo _temp1552; struct Cyc_List_List*
_temp1554; void* _temp1556; struct Cyc_Absyn_Tunionfield* _temp1558; struct Cyc_Absyn_Tuniondecl*
_temp1560; _LL1548: if(( unsigned int) _temp1546 > 4u?*(( int*) _temp1546) ==
Cyc_Absyn_TunionFieldType: 0){ _LL1553: _temp1552=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1546)->f1; _LL1557: _temp1556=( void*) _temp1552.field_info; if(*(( int*)
_temp1556) == Cyc_Absyn_KnownTunionfield){ _LL1561: _temp1560=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1556)->f1; goto _LL1559; _LL1559: _temp1558=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1556)->f2; goto _LL1555;} else{ goto _LL1550;} _LL1555: _temp1554=
_temp1552.targs; goto _LL1549;} else{ goto _LL1550;} _LL1550: goto _LL1551;
_LL1549: if( ! Cyc_Tcutil_unify( _temp1489, _temp1471)? _temp1489 !=( void*) Cyc_Absyn_HeapRgn:
0){ return 0;} if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1487,(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)( 0))){
return 0;} if( !(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp,
_temp1483,(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)( Cyc_Absyn_bounds_one))){
return 0;} if( Cyc_Absyn_qvar_cmp( _temp1477->name, _temp1560->name) == 0?
_temp1558->typs != 0: 0){ int okay= 1; for( 0; _temp1554 != 0? _temp1473 != 0: 0;(
_temp1554=(( struct Cyc_List_List*) _check_null( _temp1554))->tl, _temp1473=((
struct Cyc_List_List*) _check_null( _temp1473))->tl)){ if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_List_List*) _check_null( _temp1554))->hd,( void*)(( struct
Cyc_List_List*) _check_null( _temp1473))->hd)){ okay= 0; break;}} if(( ! okay? 1:
_temp1554 != 0)? 1: _temp1473 != 0){ return 0;} return 1;} goto _LL1547; _LL1551:
goto _LL1547; _LL1547:;} return 0; _LL1418: return Cyc_Tcutil_unify( t1, t2);
_LL1408:;}} int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} if( Cyc_Tcutil_is_arithmetic_type(
t1)? Cyc_Tcutil_is_arithmetic_type( t2): 0){ if( Cyc_Tcutil_will_lose_precision(
t1, t2)){ Cyc_Tcutil_warn( e->loc,({ unsigned char* _temp1562=( unsigned char*)"integral size mismatch; conversion supplied";
struct _tagged_string _temp1563; _temp1563.curr= _temp1562; _temp1563.base=
_temp1562; _temp1563.last_plus_one= _temp1562 + 44; _temp1563;}));} Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){
Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2); Cyc_Tcutil_warn( e->loc,(
struct _tagged_string)({ struct _tagged_string _temp1564= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp1565= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1564.last_plus_one - _temp1564.curr, _temp1564.curr, _temp1565.last_plus_one
- _temp1565.curr, _temp1565.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg(
te, e, t);} static int Cyc_Tcutil_any_promote( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e){ void* old_typ= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); void* t;{ void* _temp1566= old_typ; struct Cyc_Absyn_Enumdecl*
_temp1578; struct _tuple1* _temp1580; void* _temp1582; void* _temp1584; _LL1568:
if(( unsigned int) _temp1566 > 4u?*(( int*) _temp1566) == Cyc_Absyn_EnumType: 0){
_LL1581: _temp1580=(( struct Cyc_Absyn_EnumType_struct*) _temp1566)->f1; goto
_LL1579; _LL1579: _temp1578=(( struct Cyc_Absyn_EnumType_struct*) _temp1566)->f2;
goto _LL1569;} else{ goto _LL1570;} _LL1570: if(( unsigned int) _temp1566 > 4u?*((
int*) _temp1566) == Cyc_Absyn_IntType: 0){ _LL1585: _temp1584=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1566)->f1; goto _LL1583; _LL1583: _temp1582=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1566)->f2; goto _LL1571;} else{
goto _LL1572;} _LL1572: if( _temp1566 ==( void*) Cyc_Absyn_FloatType){ goto
_LL1573;} else{ goto _LL1574;} _LL1574: if( _temp1566 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1575;} else{ goto _LL1576;} _LL1576: goto _LL1577; _LL1569: t= Cyc_Absyn_sint_t;
goto _LL1567; _LL1571:{ void* _temp1586= _temp1582; _LL1588: if( _temp1586 ==(
void*) Cyc_Absyn_B1){ goto _LL1589;} else{ goto _LL1590;} _LL1590: if( _temp1586
==( void*) Cyc_Absyn_B2){ goto _LL1591;} else{ goto _LL1592;} _LL1592: if(
_temp1586 ==( void*) Cyc_Absyn_B4){ goto _LL1593;} else{ goto _LL1594;} _LL1594:
if( _temp1586 ==( void*) Cyc_Absyn_B8){ goto _LL1595;} else{ goto _LL1587;}
_LL1589: t= Cyc_Absyn_sint_t; goto _LL1587; _LL1591: t= Cyc_Absyn_sint_t; goto
_LL1587; _LL1593: t= _temp1584 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL1587; _LL1595: t= _temp1584 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_ulong_t: Cyc_Absyn_slong_t; goto _LL1587; _LL1587:;} goto _LL1567;
_LL1573: t= Cyc_Absyn_float_t; goto _LL1567; _LL1575: t= Cyc_Absyn_double_t;
goto _LL1567; _LL1577: return 0; _LL1567:;} Cyc_Tcutil_unchecked_cast( te, e, t);
return 1;} int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} Cyc_Tcutil_any_promote(
te, e); if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v,
t2)){ return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); Cyc_Tcutil_warn( e->loc,( struct _tagged_string)({ struct
_tagged_string _temp1596= Cyc_Absynpp_typ2string( t1); struct _tagged_string
_temp1597= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1596.last_plus_one - _temp1596.curr, _temp1596.curr, _temp1597.last_plus_one
- _temp1597.curr, _temp1597.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerceable(
void* t){ void* _temp1598= Cyc_Tcutil_compress( t); void* _temp1608; void*
_temp1610; _LL1600: if(( unsigned int) _temp1598 > 4u?*(( int*) _temp1598) ==
Cyc_Absyn_IntType: 0){ _LL1611: _temp1610=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1598)->f1; goto _LL1609; _LL1609: _temp1608=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1598)->f2; goto _LL1601;} else{ goto _LL1602;} _LL1602: if( _temp1598 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1603;} else{ goto _LL1604;} _LL1604: if(
_temp1598 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1605;} else{ goto _LL1606;}
_LL1606: goto _LL1607; _LL1601: return 1; _LL1603: return 1; _LL1605: return 1;
_LL1607: return 0; _LL1599:;} static struct _tuple4* Cyc_Tcutil_flatten_typ_f(
struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield* x){ return({ struct
_tuple4* _temp1612=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1612->f1= x->tq; _temp1612->f2= Cyc_Tcutil_substitute( inst,( void*) x->type);
_temp1612;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ( struct Cyc_Tcenv_Tenv*
te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void* _temp1613= t1; struct Cyc_List_List*
_temp1625; struct Cyc_Absyn_Structdecl** _temp1627; struct Cyc_List_List*
_temp1629; struct _tuple1* _temp1631; struct Cyc_List_List* _temp1633; _LL1615:
if( _temp1613 ==( void*) Cyc_Absyn_VoidType){ goto _LL1616;} else{ goto _LL1617;}
_LL1617: if(( unsigned int) _temp1613 > 4u?*(( int*) _temp1613) == Cyc_Absyn_TupleType:
0){ _LL1626: _temp1625=(( struct Cyc_Absyn_TupleType_struct*) _temp1613)->f1;
goto _LL1618;} else{ goto _LL1619;} _LL1619: if(( unsigned int) _temp1613 > 4u?*((
int*) _temp1613) == Cyc_Absyn_StructType: 0){ _LL1632: _temp1631=(( struct Cyc_Absyn_StructType_struct*)
_temp1613)->f1; goto _LL1630; _LL1630: _temp1629=(( struct Cyc_Absyn_StructType_struct*)
_temp1613)->f2; goto _LL1628; _LL1628: _temp1627=(( struct Cyc_Absyn_StructType_struct*)
_temp1613)->f3; goto _LL1620;} else{ goto _LL1621;} _LL1621: if(( unsigned int)
_temp1613 > 4u?*(( int*) _temp1613) == Cyc_Absyn_AnonStructType: 0){ _LL1634:
_temp1633=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1613)->f1; goto
_LL1622;} else{ goto _LL1623;} _LL1623: goto _LL1624; _LL1616: return 0; _LL1618:
return _temp1625; _LL1620: if( _temp1627 == 0? 1: _temp1631 == 0){ return({
struct Cyc_List_List* _temp1635=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1635->hd=( void*)({ struct _tuple4* _temp1636=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1636->f1= Cyc_Absyn_empty_tqual();
_temp1636->f2= t1; _temp1636;}); _temp1635->tl= 0; _temp1635;});}{ struct Cyc_Absyn_Structdecl*
_temp1637=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp1627)); goto
_LL1638; _LL1638: { struct _tuple1 _temp1639=*(( struct _tuple1*) _check_null(
_temp1631)); goto _LL1640; _LL1640: { struct Cyc_List_List* _temp1641=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1637->tvs, _temp1629); goto _LL1642; _LL1642: if( _temp1637->fields == 0){
return({ struct Cyc_List_List* _temp1643=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1643->hd=( void*)({ struct _tuple4*
_temp1644=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1644->f1=
Cyc_Absyn_empty_tqual(); _temp1644->f2= t1; _temp1644;}); _temp1643->tl= 0;
_temp1643;});} return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, _temp1641,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1637->fields))->v);}}}
_LL1622: return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, 0, _temp1633); _LL1624: return({
struct Cyc_List_List* _temp1645=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1645->hd=( void*)({ struct _tuple4* _temp1646=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1646->f1= Cyc_Absyn_empty_tqual();
_temp1646->f2= t1; _temp1646;}); _temp1645->tl= 0; _temp1645;}); _LL1614:;}}
static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2); static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;}{ struct Cyc_List_List* a= assume; for( 0; a != 0; a=(( struct
Cyc_List_List*) _check_null( a))->tl){ if( Cyc_Tcutil_unify( t1,(*(( struct
_tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f1)? Cyc_Tcutil_unify(
t2,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f2): 0){
return 1;}}} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct
_tuple8 _temp1648=({ struct _tuple8 _temp1647; _temp1647.f1= t1; _temp1647.f2=
t2; _temp1647;}); void* _temp1654; struct Cyc_Absyn_PtrInfo _temp1656; struct
Cyc_Absyn_Conref* _temp1658; struct Cyc_Absyn_Tqual _temp1660; struct Cyc_Absyn_Conref*
_temp1662; void* _temp1664; void* _temp1666; void* _temp1668; struct Cyc_Absyn_PtrInfo
_temp1670; struct Cyc_Absyn_Conref* _temp1672; struct Cyc_Absyn_Tqual _temp1674;
struct Cyc_Absyn_Conref* _temp1676; void* _temp1678; void* _temp1680; _LL1650:
_LL1669: _temp1668= _temp1648.f1; if(( unsigned int) _temp1668 > 4u?*(( int*)
_temp1668) == Cyc_Absyn_PointerType: 0){ _LL1671: _temp1670=(( struct Cyc_Absyn_PointerType_struct*)
_temp1668)->f1; _LL1681: _temp1680=( void*) _temp1670.elt_typ; goto _LL1679;
_LL1679: _temp1678=( void*) _temp1670.rgn_typ; goto _LL1677; _LL1677: _temp1676=
_temp1670.nullable; goto _LL1675; _LL1675: _temp1674= _temp1670.tq; goto _LL1673;
_LL1673: _temp1672= _temp1670.bounds; goto _LL1655;} else{ goto _LL1652;}
_LL1655: _temp1654= _temp1648.f2; if(( unsigned int) _temp1654 > 4u?*(( int*)
_temp1654) == Cyc_Absyn_PointerType: 0){ _LL1657: _temp1656=(( struct Cyc_Absyn_PointerType_struct*)
_temp1654)->f1; _LL1667: _temp1666=( void*) _temp1656.elt_typ; goto _LL1665;
_LL1665: _temp1664=( void*) _temp1656.rgn_typ; goto _LL1663; _LL1663: _temp1662=
_temp1656.nullable; goto _LL1661; _LL1661: _temp1660= _temp1656.tq; goto _LL1659;
_LL1659: _temp1658= _temp1656.bounds; goto _LL1651;} else{ goto _LL1652;}
_LL1652: goto _LL1653; _LL1651: if( _temp1674.q_const? ! _temp1660.q_const: 0){
return 0;} if(( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1676, _temp1662)?(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1676): 0)? !(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1662): 0){ return 0;}{ void* _temp1682= _temp1678; _LL1684: if( _temp1682
==( void*) Cyc_Absyn_HeapRgn){ goto _LL1685;} else{ goto _LL1686;} _LL1686: goto
_LL1687; _LL1685: goto _LL1683; _LL1687: Cyc_Tcutil_unify( _temp1678, _temp1664);
goto _LL1683; _LL1683:;} if( !(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp,
_temp1672, _temp1658)){ struct _tuple8 _temp1689=({ struct _tuple8 _temp1688;
_temp1688.f1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1672); _temp1688.f2=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1658); _temp1688;}); void* _temp1697; void* _temp1699; struct Cyc_Absyn_Exp*
_temp1701; void* _temp1703; struct Cyc_Absyn_Exp* _temp1705; void* _temp1707;
struct Cyc_Absyn_Exp* _temp1709; _LL1691: _LL1700: _temp1699= _temp1689.f1; if((
unsigned int) _temp1699 > 1u?*(( int*) _temp1699) == Cyc_Absyn_Upper_b: 0){
_LL1702: _temp1701=(( struct Cyc_Absyn_Upper_b_struct*) _temp1699)->f1; goto
_LL1698;} else{ goto _LL1693;} _LL1698: _temp1697= _temp1689.f2; if( _temp1697
==( void*) Cyc_Absyn_Unknown_b){ goto _LL1692;} else{ goto _LL1693;} _LL1693:
_LL1708: _temp1707= _temp1689.f1; if(( unsigned int) _temp1707 > 1u?*(( int*)
_temp1707) == Cyc_Absyn_Upper_b: 0){ _LL1710: _temp1709=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1707)->f1; goto _LL1704;} else{ goto _LL1695;} _LL1704: _temp1703=
_temp1689.f2; if(( unsigned int) _temp1703 > 1u?*(( int*) _temp1703) == Cyc_Absyn_Upper_b:
0){ _LL1706: _temp1705=(( struct Cyc_Absyn_Upper_b_struct*) _temp1703)->f1; goto
_LL1694;} else{ goto _LL1695;} _LL1695: goto _LL1696; _LL1692: goto _LL1690;
_LL1694: if( Cyc_Evexp_eval_const_uint_exp( _temp1709) < Cyc_Evexp_eval_const_uint_exp(
_temp1705)){ return 0;} goto _LL1690; _LL1696: return 0; _LL1690:;} return Cyc_Tcutil_ptrsubtype(
te,({ struct Cyc_List_List* _temp1711=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1711->hd=( void*)({ struct _tuple8*
_temp1712=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp1712->f1=
t1; _temp1712->f2= t2; _temp1712;}); _temp1711->tl= assume; _temp1711;}),
_temp1680, _temp1666); _LL1653: return 0; _LL1649:;}} static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* assume, void* t1, void* t2){
struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ( te, t1); struct Cyc_List_List*
tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0; tqs2 != 0;( tqs2=(( struct Cyc_List_List*)
_check_null( tqs2))->tl, tqs1=(( struct Cyc_List_List*) _check_null( tqs1))->tl)){
if( tqs1 == 0){ return 0;}{ struct _tuple4 _temp1715; void* _temp1716; struct
Cyc_Absyn_Tqual _temp1718; struct _tuple4* _temp1713=( struct _tuple4*)(( struct
Cyc_List_List*) _check_null( tqs1))->hd; _temp1715=* _temp1713; _LL1719:
_temp1718= _temp1715.f1; goto _LL1717; _LL1717: _temp1716= _temp1715.f2; goto
_LL1714; _LL1714: { struct _tuple4 _temp1722; void* _temp1723; struct Cyc_Absyn_Tqual
_temp1725; struct _tuple4* _temp1720=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( tqs2))->hd; _temp1722=* _temp1720; _LL1726: _temp1725= _temp1722.f1;
goto _LL1724; _LL1724: _temp1723= _temp1722.f2; goto _LL1721; _LL1721: if(
_temp1725.q_const? Cyc_Tcutil_subtype( te, assume, _temp1716, _temp1723): 0){
continue;} else{ if( Cyc_Tcutil_unify( _temp1716, _temp1723)){ continue;} else{
return 0;}}}}} return 1;} int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
void* _temp1727= t2; struct Cyc_Absyn_Uniondecl** _temp1735; struct Cyc_List_List*
_temp1737; struct _tuple1* _temp1739; void* _temp1741; void* _temp1743; _LL1729:
if(( unsigned int) _temp1727 > 4u?*(( int*) _temp1727) == Cyc_Absyn_UnionType: 0){
_LL1740: _temp1739=(( struct Cyc_Absyn_UnionType_struct*) _temp1727)->f1; goto
_LL1738; _LL1738: _temp1737=(( struct Cyc_Absyn_UnionType_struct*) _temp1727)->f2;
goto _LL1736; _LL1736: _temp1735=(( struct Cyc_Absyn_UnionType_struct*)
_temp1727)->f3; goto _LL1730;} else{ goto _LL1731;} _LL1731: if(( unsigned int)
_temp1727 > 4u?*(( int*) _temp1727) == Cyc_Absyn_IntType: 0){ _LL1744: _temp1743=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1727)->f1; goto _LL1742; _LL1742:
_temp1741=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1727)->f2; if(
_temp1741 ==( void*) Cyc_Absyn_B4){ goto _LL1732;} else{ goto _LL1733;}} else{
goto _LL1733;} _LL1733: goto _LL1734; _LL1730: if( _temp1735 == 0){ goto _LL1728;}{
struct Cyc_Absyn_Uniondecl* _temp1745=*(( struct Cyc_Absyn_Uniondecl**)
_check_null( _temp1735)); goto _LL1746; _LL1746: if( _temp1745->fields == 0){
goto _LL1728;}{ struct Cyc_List_List* _temp1747=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp1745->tvs,
_temp1737); goto _LL1748; _LL1748:{ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1745->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ if( Cyc_Tcutil_unify( t1, Cyc_Tcutil_substitute(
_temp1747,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ return 1;}}} goto _LL1728;}} _LL1732: if( Cyc_Tcutil_typ_kind(
t2) ==( void*) Cyc_Absyn_BoxKind){ return 1;} goto _LL1728; _LL1734: goto
_LL1728; _LL1728:;}{ void* _temp1749= t1; struct Cyc_Absyn_PtrInfo _temp1765;
struct Cyc_Absyn_Conref* _temp1767; struct Cyc_Absyn_Tqual _temp1769; struct Cyc_Absyn_Conref*
_temp1771; void* _temp1773; void* _temp1775; struct Cyc_Absyn_Exp* _temp1777;
struct Cyc_Absyn_Tqual _temp1779; void* _temp1781; struct Cyc_Absyn_Enumdecl*
_temp1783; struct _tuple1* _temp1785; void* _temp1787; void* _temp1789; _LL1751:
if(( unsigned int) _temp1749 > 4u?*(( int*) _temp1749) == Cyc_Absyn_PointerType:
0){ _LL1766: _temp1765=(( struct Cyc_Absyn_PointerType_struct*) _temp1749)->f1;
_LL1776: _temp1775=( void*) _temp1765.elt_typ; goto _LL1774; _LL1774: _temp1773=(
void*) _temp1765.rgn_typ; goto _LL1772; _LL1772: _temp1771= _temp1765.nullable;
goto _LL1770; _LL1770: _temp1769= _temp1765.tq; goto _LL1768; _LL1768: _temp1767=
_temp1765.bounds; goto _LL1752;} else{ goto _LL1753;} _LL1753: if(( unsigned int)
_temp1749 > 4u?*(( int*) _temp1749) == Cyc_Absyn_ArrayType: 0){ _LL1782:
_temp1781=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1749)->f1; goto
_LL1780; _LL1780: _temp1779=(( struct Cyc_Absyn_ArrayType_struct*) _temp1749)->f2;
goto _LL1778; _LL1778: _temp1777=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1749)->f3; goto _LL1754;} else{ goto _LL1755;} _LL1755: if(( unsigned int)
_temp1749 > 4u?*(( int*) _temp1749) == Cyc_Absyn_EnumType: 0){ _LL1786:
_temp1785=(( struct Cyc_Absyn_EnumType_struct*) _temp1749)->f1; goto _LL1784;
_LL1784: _temp1783=(( struct Cyc_Absyn_EnumType_struct*) _temp1749)->f2; goto
_LL1756;} else{ goto _LL1757;} _LL1757: if(( unsigned int) _temp1749 > 4u?*((
int*) _temp1749) == Cyc_Absyn_IntType: 0){ _LL1790: _temp1789=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1749)->f1; goto _LL1788; _LL1788: _temp1787=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1749)->f2; goto _LL1758;} else{
goto _LL1759;} _LL1759: if( _temp1749 ==( void*) Cyc_Absyn_FloatType){ goto
_LL1760;} else{ goto _LL1761;} _LL1761: if( _temp1749 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1762;} else{ goto _LL1763;} _LL1763: goto _LL1764; _LL1752:{ void*
_temp1791= t2; struct Cyc_Absyn_PtrInfo _temp1797; struct Cyc_Absyn_Conref*
_temp1799; struct Cyc_Absyn_Tqual _temp1801; struct Cyc_Absyn_Conref* _temp1803;
void* _temp1805; void* _temp1807; _LL1793: if(( unsigned int) _temp1791 > 4u?*((
int*) _temp1791) == Cyc_Absyn_PointerType: 0){ _LL1798: _temp1797=(( struct Cyc_Absyn_PointerType_struct*)
_temp1791)->f1; _LL1808: _temp1807=( void*) _temp1797.elt_typ; goto _LL1806;
_LL1806: _temp1805=( void*) _temp1797.rgn_typ; goto _LL1804; _LL1804: _temp1803=
_temp1797.nullable; goto _LL1802; _LL1802: _temp1801= _temp1797.tq; goto _LL1800;
_LL1800: _temp1799= _temp1797.bounds; goto _LL1794;} else{ goto _LL1795;}
_LL1795: goto _LL1796; _LL1794: { struct Cyc_List_List* _temp1811=({ struct Cyc_List_List*
_temp1809=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1809->hd=( void*)({ struct _tuple8* _temp1810=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp1810->f1= t1; _temp1810->f2= t2; _temp1810;});
_temp1809->tl= 0; _temp1809;}); goto _LL1812; _LL1812: { int _temp1813= Cyc_Tcutil_ptrsubtype(
te, _temp1811, _temp1775, _temp1807)? ! _temp1769.q_const? 1: _temp1801.q_const:
0; goto _LL1814; _LL1814: { int bounds_ok=(( int(*)( int(* cmp)( void*, void*),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp1767, _temp1799); if( ! bounds_ok){ struct _tuple8
_temp1816=({ struct _tuple8 _temp1815; _temp1815.f1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp1767); _temp1815.f2=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp1799); _temp1815;}); void* _temp1822; struct Cyc_Absyn_Exp*
_temp1824; void* _temp1826; struct Cyc_Absyn_Exp* _temp1828; _LL1818: _LL1827:
_temp1826= _temp1816.f1; if(( unsigned int) _temp1826 > 1u?*(( int*) _temp1826)
== Cyc_Absyn_Upper_b: 0){ _LL1829: _temp1828=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1826)->f1; goto _LL1823;} else{ goto _LL1820;} _LL1823: _temp1822=
_temp1816.f2; if(( unsigned int) _temp1822 > 1u?*(( int*) _temp1822) == Cyc_Absyn_Upper_b:
0){ _LL1825: _temp1824=(( struct Cyc_Absyn_Upper_b_struct*) _temp1822)->f1; goto
_LL1819;} else{ goto _LL1820;} _LL1820: goto _LL1821; _LL1819: if( Cyc_Evexp_eval_const_uint_exp(
_temp1828) >= Cyc_Evexp_eval_const_uint_exp( _temp1824)){ bounds_ok= 1;} goto
_LL1817; _LL1821: bounds_ok= 1; goto _LL1817; _LL1817:;} return( bounds_ok?
_temp1813: 0)? _temp1773 ==( void*) Cyc_Absyn_HeapRgn? 1: Cyc_Tcutil_unify(
_temp1773, _temp1805): 0;}}} _LL1796: goto _LL1792; _LL1792:;} return 0; _LL1754:{
void* _temp1830= t2; struct Cyc_Absyn_Exp* _temp1836; struct Cyc_Absyn_Tqual
_temp1838; void* _temp1840; _LL1832: if(( unsigned int) _temp1830 > 4u?*(( int*)
_temp1830) == Cyc_Absyn_ArrayType: 0){ _LL1841: _temp1840=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1830)->f1; goto _LL1839; _LL1839: _temp1838=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1830)->f2; goto _LL1837; _LL1837: _temp1836=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1830)->f3; goto _LL1833;} else{ goto _LL1834;} _LL1834: goto _LL1835;
_LL1833: { int okay; okay=( _temp1777 != 0? _temp1836 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1777)) >= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1836)): 0; return( okay? Cyc_Tcutil_unify(
_temp1781, _temp1840): 0)? ! _temp1779.q_const? 1: _temp1838.q_const: 0;}
_LL1835: return 0; _LL1831:;} return 0; _LL1756:{ void* _temp1842= t2; struct
Cyc_Absyn_Enumdecl* _temp1848; struct _tuple1* _temp1850; _LL1844: if((
unsigned int) _temp1842 > 4u?*(( int*) _temp1842) == Cyc_Absyn_EnumType: 0){
_LL1851: _temp1850=(( struct Cyc_Absyn_EnumType_struct*) _temp1842)->f1; goto
_LL1849; _LL1849: _temp1848=(( struct Cyc_Absyn_EnumType_struct*) _temp1842)->f2;
goto _LL1845;} else{ goto _LL1846;} _LL1846: goto _LL1847; _LL1845: if((((
struct Cyc_Absyn_Enumdecl*) _check_null( _temp1783))->fields != 0?(( struct Cyc_Absyn_Enumdecl*)
_check_null( _temp1848))->fields != 0: 0)?(( int(*)( struct Cyc_List_List* x))
Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(((
struct Cyc_Absyn_Enumdecl*) _check_null( _temp1783))->fields))->v) >=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Enumdecl*) _check_null( _temp1848))->fields))->v):
0){ return 1;} goto _LL1843; _LL1847: goto _LL1843; _LL1843:;} goto _LL1758;
_LL1758: goto _LL1760; _LL1760: goto _LL1762; _LL1762: return Cyc_Tcutil_coerceable(
t2); _LL1764: return 0; _LL1750:;}} void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, void* t){ if( ! Cyc_Tcutil_unify(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v, t)){ struct Cyc_Absyn_Exp* _temp1852=
Cyc_Absyn_copy_exp( e); goto _LL1853; _LL1853:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Cast_e_struct* _temp1854=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp1854[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp1855; _temp1855.tag= Cyc_Absyn_Cast_e; _temp1855.f1=( void*) t; _temp1855.f2=
_temp1852; _temp1855;}); _temp1854;}))); e->topt=({ struct Cyc_Core_Opt*
_temp1856=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1856->v=( void*) t; _temp1856;});}} int Cyc_Tcutil_is_integral( struct Cyc_Absyn_Exp*
e){ void* _temp1857= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); struct Cyc_Absyn_Enumdecl* _temp1867; struct _tuple1*
_temp1869; void* _temp1871; void* _temp1873; struct Cyc_Core_Opt* _temp1875; int
_temp1877; struct Cyc_Core_Opt* _temp1879; struct Cyc_Core_Opt* _temp1881;
_LL1859: if(( unsigned int) _temp1857 > 4u?*(( int*) _temp1857) == Cyc_Absyn_EnumType:
0){ _LL1870: _temp1869=(( struct Cyc_Absyn_EnumType_struct*) _temp1857)->f1;
goto _LL1868; _LL1868: _temp1867=(( struct Cyc_Absyn_EnumType_struct*) _temp1857)->f2;
goto _LL1860;} else{ goto _LL1861;} _LL1861: if(( unsigned int) _temp1857 > 4u?*((
int*) _temp1857) == Cyc_Absyn_IntType: 0){ _LL1874: _temp1873=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1857)->f1; goto _LL1872; _LL1872: _temp1871=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1857)->f2; goto _LL1862;} else{
goto _LL1863;} _LL1863: if(( unsigned int) _temp1857 > 4u?*(( int*) _temp1857)
== Cyc_Absyn_Evar: 0){ _LL1882: _temp1881=(( struct Cyc_Absyn_Evar_struct*)
_temp1857)->f1; goto _LL1880; _LL1880: _temp1879=(( struct Cyc_Absyn_Evar_struct*)
_temp1857)->f2; goto _LL1878; _LL1878: _temp1877=(( struct Cyc_Absyn_Evar_struct*)
_temp1857)->f3; goto _LL1876; _LL1876: _temp1875=(( struct Cyc_Absyn_Evar_struct*)
_temp1857)->f4; goto _LL1864;} else{ goto _LL1865;} _LL1865: goto _LL1866;
_LL1860: goto _LL1862; _LL1862: return 1; _LL1864: if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;}
return 0; _LL1866: return 0; _LL1858:;} int Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp*
e){ if( Cyc_Tcutil_is_integral( e)){ return 1;}{ void* _temp1883= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _LL1885: if(
_temp1883 ==( void*) Cyc_Absyn_FloatType){ goto _LL1886;} else{ goto _LL1887;}
_LL1887: if( _temp1883 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1888;} else{
goto _LL1889;} _LL1889: goto _LL1890; _LL1886: goto _LL1888; _LL1888: return 1;
_LL1890: return 0; _LL1884:;}} int Cyc_Tcutil_is_function_type( void* t){ void*
_temp1891= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp1897; _LL1893:
if(( unsigned int) _temp1891 > 4u?*(( int*) _temp1891) == Cyc_Absyn_FnType: 0){
_LL1898: _temp1897=(( struct Cyc_Absyn_FnType_struct*) _temp1891)->f1; goto
_LL1894;} else{ goto _LL1895;} _LL1895: goto _LL1896; _LL1894: return 1; _LL1896:
return 0; _LL1892:;} void* Cyc_Tcutil_max_arithmetic_type( void* t1, void* t2){
struct _tuple8 _temp1900=({ struct _tuple8 _temp1899; _temp1899.f1= t1;
_temp1899.f2= t2; _temp1899;}); void* _temp1924; void* _temp1926; void*
_temp1928; void* _temp1930; void* _temp1932; void* _temp1934; void* _temp1936;
void* _temp1938; void* _temp1940; void* _temp1942; void* _temp1944; void*
_temp1946; void* _temp1948; void* _temp1950; void* _temp1952; void* _temp1954;
void* _temp1956; void* _temp1958; void* _temp1960; void* _temp1962; void*
_temp1964; void* _temp1966; void* _temp1968; void* _temp1970; void* _temp1972;
void* _temp1974; void* _temp1976; void* _temp1978; void* _temp1980; void*
_temp1982; void* _temp1984; void* _temp1986; _LL1902: _LL1927: _temp1926=
_temp1900.f1; if( _temp1926 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1925;}
else{ goto _LL1904;} _LL1925: _temp1924= _temp1900.f2; goto _LL1903; _LL1904:
_LL1931: _temp1930= _temp1900.f1; goto _LL1929; _LL1929: _temp1928= _temp1900.f2;
if( _temp1928 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1905;} else{ goto
_LL1906;} _LL1906: _LL1935: _temp1934= _temp1900.f1; if( _temp1934 ==( void*)
Cyc_Absyn_FloatType){ goto _LL1933;} else{ goto _LL1908;} _LL1933: _temp1932=
_temp1900.f2; goto _LL1907; _LL1908: _LL1939: _temp1938= _temp1900.f1; goto
_LL1937; _LL1937: _temp1936= _temp1900.f2; if( _temp1936 ==( void*) Cyc_Absyn_FloatType){
goto _LL1909;} else{ goto _LL1910;} _LL1910: _LL1943: _temp1942= _temp1900.f1;
if(( unsigned int) _temp1942 > 4u?*(( int*) _temp1942) == Cyc_Absyn_IntType: 0){
_LL1947: _temp1946=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1942)->f1;
if( _temp1946 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1945;} else{ goto _LL1912;}
_LL1945: _temp1944=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1942)->f2;
if( _temp1944 ==( void*) Cyc_Absyn_B8){ goto _LL1941;} else{ goto _LL1912;}}
else{ goto _LL1912;} _LL1941: _temp1940= _temp1900.f2; goto _LL1911; _LL1912:
_LL1955: _temp1954= _temp1900.f1; goto _LL1949; _LL1949: _temp1948= _temp1900.f2;
if(( unsigned int) _temp1948 > 4u?*(( int*) _temp1948) == Cyc_Absyn_IntType: 0){
_LL1953: _temp1952=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1948)->f1;
if( _temp1952 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1951;} else{ goto _LL1914;}
_LL1951: _temp1950=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1948)->f2;
if( _temp1950 ==( void*) Cyc_Absyn_B8){ goto _LL1913;} else{ goto _LL1914;}}
else{ goto _LL1914;} _LL1914: _LL1959: _temp1958= _temp1900.f1; if((
unsigned int) _temp1958 > 4u?*(( int*) _temp1958) == Cyc_Absyn_IntType: 0){
_LL1963: _temp1962=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1958)->f1;
if( _temp1962 ==( void*) Cyc_Absyn_Signed){ goto _LL1961;} else{ goto _LL1916;}
_LL1961: _temp1960=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1958)->f2;
if( _temp1960 ==( void*) Cyc_Absyn_B8){ goto _LL1957;} else{ goto _LL1916;}}
else{ goto _LL1916;} _LL1957: _temp1956= _temp1900.f2; goto _LL1915; _LL1916:
_LL1971: _temp1970= _temp1900.f1; goto _LL1965; _LL1965: _temp1964= _temp1900.f2;
if(( unsigned int) _temp1964 > 4u?*(( int*) _temp1964) == Cyc_Absyn_IntType: 0){
_LL1969: _temp1968=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1964)->f1;
if( _temp1968 ==( void*) Cyc_Absyn_Signed){ goto _LL1967;} else{ goto _LL1918;}
_LL1967: _temp1966=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1964)->f2;
if( _temp1966 ==( void*) Cyc_Absyn_B8){ goto _LL1917;} else{ goto _LL1918;}}
else{ goto _LL1918;} _LL1918: _LL1975: _temp1974= _temp1900.f1; if((
unsigned int) _temp1974 > 4u?*(( int*) _temp1974) == Cyc_Absyn_IntType: 0){
_LL1979: _temp1978=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1974)->f1;
if( _temp1978 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1977;} else{ goto _LL1920;}
_LL1977: _temp1976=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1974)->f2;
if( _temp1976 ==( void*) Cyc_Absyn_B4){ goto _LL1973;} else{ goto _LL1920;}}
else{ goto _LL1920;} _LL1973: _temp1972= _temp1900.f2; goto _LL1919; _LL1920:
_LL1987: _temp1986= _temp1900.f1; goto _LL1981; _LL1981: _temp1980= _temp1900.f2;
if(( unsigned int) _temp1980 > 4u?*(( int*) _temp1980) == Cyc_Absyn_IntType: 0){
_LL1985: _temp1984=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1980)->f1;
if( _temp1984 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1983;} else{ goto _LL1922;}
_LL1983: _temp1982=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1980)->f2;
if( _temp1982 ==( void*) Cyc_Absyn_B4){ goto _LL1921;} else{ goto _LL1922;}}
else{ goto _LL1922;} _LL1922: goto _LL1923; _LL1903: goto _LL1905; _LL1905:
return( void*) Cyc_Absyn_DoubleType; _LL1907: goto _LL1909; _LL1909: return(
void*) Cyc_Absyn_FloatType; _LL1911: goto _LL1913; _LL1913: return Cyc_Absyn_ulong_t;
_LL1915: goto _LL1917; _LL1917: return Cyc_Absyn_slong_t; _LL1919: goto _LL1921;
_LL1921: return Cyc_Absyn_uint_t; _LL1923: return Cyc_Absyn_sint_t; _LL1901:;}
void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp* e){ void* _temp1988=(
void*) e->r; struct Cyc_Absyn_Exp* _temp1994; struct Cyc_Core_Opt* _temp1996;
struct Cyc_Absyn_Exp* _temp1998; _LL1990: if(*(( int*) _temp1988) == Cyc_Absyn_AssignOp_e){
_LL1999: _temp1998=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp1988)->f1; goto
_LL1997; _LL1997: _temp1996=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp1988)->f2;
if( _temp1996 == 0){ goto _LL1995;} else{ goto _LL1992;} _LL1995: _temp1994=((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1988)->f3; goto _LL1991;} else{ goto
_LL1992;} _LL1992: goto _LL1993; _LL1991: Cyc_Tcutil_warn( e->loc,({
unsigned char* _temp2000=( unsigned char*)"assignment in test"; struct
_tagged_string _temp2001; _temp2001.curr= _temp2000; _temp2001.base= _temp2000;
_temp2001.last_plus_one= _temp2000 + 19; _temp2001;})); goto _LL1989; _LL1993:
goto _LL1989; _LL1989:;} static int Cyc_Tcutil_constrain_kinds( struct Cyc_Absyn_Conref*
c1, struct Cyc_Absyn_Conref* c2){ c1=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( c1); c2=(( struct Cyc_Absyn_Conref*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( c2); if( c1 == c2){ return 1;}
if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr){( void*)( c1->v=( void*)((
void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2002=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2002[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2003; _temp2003.tag= Cyc_Absyn_Forward_constr;
_temp2003.f1= c2; _temp2003;}); _temp2002;}))); return 1;} else{ if(( void*) c2->v
==( void*) Cyc_Absyn_No_constr){( void*)( c2->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp2004=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp2004[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp2005; _temp2005.tag= Cyc_Absyn_Forward_constr; _temp2005.f1= c1; _temp2005;});
_temp2004;}))); return 1;} else{ void* k1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( c1); void* k2=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( c2); if( Cyc_Tcutil_kind_leq( k1, k2)){( void*)( c2->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2006=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2006[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2007; _temp2007.tag= Cyc_Absyn_Forward_constr;
_temp2007.f1= c1; _temp2007;}); _temp2006;}))); return 1;} else{ if( Cyc_Tcutil_kind_leq(
k2, k1)){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp2008=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp2008[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp2009; _temp2009.tag= Cyc_Absyn_Forward_constr; _temp2009.f1= c2; _temp2009;});
_temp2008;}))); return 1;} else{ return 0;}}}}} static int Cyc_Tcutil_tvar_id_counter=
0; int* Cyc_Tcutil_new_tvar_id(){ return({ int* _temp2010=( int*)
GC_malloc_atomic( sizeof( int)); _temp2010[ 0]= Cyc_Tcutil_tvar_id_counter ++;
_temp2010;});} static int Cyc_Tcutil_tvar_counter= 0; struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar(
void* k){ int i= Cyc_Tcutil_tvar_counter ++; struct _tagged_string s=( struct
_tagged_string) xprintf("#%d", i); return({ struct Cyc_Absyn_Tvar* _temp2011=(
struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2011->name=({
struct _tagged_string* _temp2012=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp2012[ 0]= s; _temp2012;}); _temp2011->identity=
0; _temp2011->kind=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
k); _temp2011;});} int Cyc_Tcutil_is_temp_tvar( struct Cyc_Absyn_Tvar* t){
struct _tagged_string _temp2013=* t->name; goto _LL2014; _LL2014: return*((
const unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( _temp2013, sizeof( unsigned char), 0) =='#';} void
Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar* t){ if( ! Cyc_Tcutil_is_temp_tvar(
t)){ return;}{ struct _tagged_string _temp2017= Cyc_String_strconcat(({
unsigned char* _temp2015=( unsigned char*)"`"; struct _tagged_string _temp2016;
_temp2016.curr= _temp2015; _temp2016.base= _temp2015; _temp2016.last_plus_one=
_temp2015 + 2; _temp2016;}),* t->name); goto _LL2018; _LL2018:*(( unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
_temp2017, sizeof( unsigned char), 1)='t'; t->name=({ struct _tagged_string*
_temp2019=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2019[ 0]=( struct _tagged_string) _temp2017; _temp2019;});}} struct
_tuple13{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ;
static struct _tuple2* Cyc_Tcutil_fndecl2typ_f( struct _tuple13* x){ return({
struct _tuple2* _temp2020=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2020->f1=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp2021=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2021->v=( void*)(*
x).f1; _temp2021;}); _temp2020->f2=(* x).f2; _temp2020->f3=(* x).f3; _temp2020;});}
void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl* fd){ if( fd->cached_typ ==
0){ struct Cyc_List_List* _temp2022= 0; goto _LL2023; _LL2023:{ struct Cyc_List_List*
atts= fd->attributes; for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ if( Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd)){ _temp2022=({ struct Cyc_List_List* _temp2024=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2024->hd=( void*)((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd); _temp2024->tl=
_temp2022; _temp2024;});}}} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2025=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2025[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2026; _temp2026.tag= Cyc_Absyn_FnType;
_temp2026.f1=({ struct Cyc_Absyn_FnInfo _temp2027; _temp2027.tvars= fd->tvs;
_temp2027.effect= fd->effect; _temp2027.ret_typ=( void*)(( void*) fd->ret_type);
_temp2027.args=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct
_tuple13*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_fndecl2typ_f, fd->args);
_temp2027.c_varargs= fd->c_varargs; _temp2027.cyc_varargs= fd->cyc_varargs;
_temp2027.attributes= _temp2022; _temp2027;}); _temp2026;}); _temp2025;});}
return( void*)(( struct Cyc_Core_Opt*) _check_null( fd->cached_typ))->v;} struct
_tuple14{ void* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; static void* Cyc_Tcutil_fst_fdarg(
struct _tuple14* t){ return(* t).f1;} void* Cyc_Tcutil_snd_tqt( struct _tuple4*
t){ return(* t).f2;} static struct _tuple4* Cyc_Tcutil_map2_tq( struct _tuple4*
pr, void* t){ return({ struct _tuple4* _temp2028=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp2028->f1=(* pr).f1; _temp2028->f2= t; _temp2028;});}
void Cyc_Tcutil_check_fndecl_valid_type( struct Cyc_Position_Segment* loc,
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl* fd){ void* t= Cyc_Tcutil_fndecl2typ(
fd); Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);{ void* _temp2029= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_FnInfo _temp2035; struct Cyc_List_List* _temp2037; struct
Cyc_Absyn_VarargInfo* _temp2039; int _temp2041; struct Cyc_List_List* _temp2043;
void* _temp2045; struct Cyc_Core_Opt* _temp2047; struct Cyc_List_List* _temp2049;
_LL2031: if(( unsigned int) _temp2029 > 4u?*(( int*) _temp2029) == Cyc_Absyn_FnType:
0){ _LL2036: _temp2035=(( struct Cyc_Absyn_FnType_struct*) _temp2029)->f1;
_LL2050: _temp2049= _temp2035.tvars; goto _LL2048; _LL2048: _temp2047= _temp2035.effect;
goto _LL2046; _LL2046: _temp2045=( void*) _temp2035.ret_typ; goto _LL2044;
_LL2044: _temp2043= _temp2035.args; goto _LL2042; _LL2042: _temp2041= _temp2035.c_varargs;
goto _LL2040; _LL2040: _temp2039= _temp2035.cyc_varargs; goto _LL2038; _LL2038:
_temp2037= _temp2035.attributes; goto _LL2032;} else{ goto _LL2033;} _LL2033:
goto _LL2034; _LL2032: fd->tvs= _temp2049; fd->effect= _temp2047; goto _LL2030;
_LL2034:(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
unsigned char* _temp2051=( unsigned char*)"check_fndecl_valid_type: not a FnType";
struct _tagged_string _temp2052; _temp2052.curr= _temp2051; _temp2052.base=
_temp2051; _temp2052.last_plus_one= _temp2051 + 38; _temp2052;})); return;
_LL2030:;}{ struct _RegionHandle _temp2053= _new_region(); struct _RegionHandle*
r=& _temp2053; _push_region( r); Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tagged_string*(* f)( struct _tuple13*), struct
Cyc_List_List* x)) Cyc_List_rmap)( r,( struct _tagged_string*(*)( struct
_tuple13* t)) Cyc_Tcutil_fst_fdarg, fd->args), loc,({ unsigned char* _temp2054=(
unsigned char*)"function declaration has repeated parameter"; struct
_tagged_string _temp2055; _temp2055.curr= _temp2054; _temp2055.base= _temp2054;
_temp2055.last_plus_one= _temp2054 + 44; _temp2055;}));; _pop_region(& _temp2053);}
fd->cached_typ=({ struct Cyc_Core_Opt* _temp2056=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2056->v=( void*) t; _temp2056;});}
struct _tuple15{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; } ; struct
_tuple16{ struct _tuple15* f1; void* f2; } ; static struct _tuple16* Cyc_Tcutil_substitute_f1(
struct _RegionHandle* rgn, struct _tuple2* y){ return({ struct _tuple16*
_temp2057=( struct _tuple16*) _region_malloc( rgn, sizeof( struct _tuple16));
_temp2057->f1=({ struct _tuple15* _temp2058=( struct _tuple15*) _region_malloc(
rgn, sizeof( struct _tuple15)); _temp2058->f1=(* y).f1; _temp2058->f2=(* y).f2;
_temp2058;}); _temp2057->f2=(* y).f3; _temp2057;});} static struct _tuple2* Cyc_Tcutil_substitute_f2(
struct _tuple16* w){ void* _temp2061; struct _tuple15* _temp2063; struct
_tuple16 _temp2059=* w; _LL2064: _temp2063= _temp2059.f1; goto _LL2062; _LL2062:
_temp2061= _temp2059.f2; goto _LL2060; _LL2060: { struct Cyc_Absyn_Tqual
_temp2067; struct Cyc_Core_Opt* _temp2069; struct _tuple15 _temp2065=* _temp2063;
_LL2070: _temp2069= _temp2065.f1; goto _LL2068; _LL2068: _temp2067= _temp2065.f2;
goto _LL2066; _LL2066: return({ struct _tuple2* _temp2071=( struct _tuple2*)
GC_malloc( sizeof( struct _tuple2)); _temp2071->f1= _temp2069; _temp2071->f2=
_temp2067; _temp2071->f3= _temp2061; _temp2071;});}} static void* Cyc_Tcutil_field_type(
struct Cyc_Absyn_Structfield* f){ return( void*) f->type;} static struct Cyc_Absyn_Structfield*
Cyc_Tcutil_zip_field_type( struct Cyc_Absyn_Structfield* f, void* t){ return({
struct Cyc_Absyn_Structfield* _temp2072=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2072->name= f->name;
_temp2072->tq= f->tq; _temp2072->type=( void*) t; _temp2072->width= f->width;
_temp2072->attributes= f->attributes; _temp2072;});} static struct Cyc_List_List*
Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List* inst, struct
Cyc_List_List* ts); void* Cyc_Tcutil_rsubstitute( struct _RegionHandle* rgn,
struct Cyc_List_List* inst, void* t){ void* _temp2073= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp2119; struct Cyc_Absyn_Structdecl** _temp2121;
struct Cyc_List_List* _temp2123; struct _tuple1* _temp2125; struct Cyc_Absyn_Uniondecl**
_temp2127; struct Cyc_List_List* _temp2129; struct _tuple1* _temp2131; struct
Cyc_Absyn_TunionInfo _temp2133; void* _temp2135; struct Cyc_List_List* _temp2137;
void* _temp2139; struct Cyc_Absyn_TunionFieldInfo _temp2141; struct Cyc_List_List*
_temp2143; void* _temp2145; struct Cyc_Core_Opt* _temp2147; struct Cyc_List_List*
_temp2149; struct _tuple1* _temp2151; struct Cyc_Absyn_Exp* _temp2153; struct
Cyc_Absyn_Tqual _temp2155; void* _temp2157; struct Cyc_Absyn_PtrInfo _temp2159;
struct Cyc_Absyn_Conref* _temp2161; struct Cyc_Absyn_Tqual _temp2163; struct Cyc_Absyn_Conref*
_temp2165; void* _temp2167; void* _temp2169; struct Cyc_Absyn_FnInfo _temp2171;
struct Cyc_List_List* _temp2173; struct Cyc_Absyn_VarargInfo* _temp2175; int
_temp2177; struct Cyc_List_List* _temp2179; void* _temp2181; struct Cyc_Core_Opt*
_temp2183; struct Cyc_List_List* _temp2185; struct Cyc_List_List* _temp2187;
struct Cyc_List_List* _temp2189; struct Cyc_List_List* _temp2191; struct Cyc_Core_Opt*
_temp2193; int _temp2195; struct Cyc_Core_Opt* _temp2197; struct Cyc_Core_Opt*
_temp2199; void* _temp2201; struct Cyc_Absyn_Enumdecl* _temp2203; struct _tuple1*
_temp2205; void* _temp2207; void* _temp2209; void* _temp2211; struct Cyc_List_List*
_temp2213; _LL2075: if(( unsigned int) _temp2073 > 4u?*(( int*) _temp2073) ==
Cyc_Absyn_VarType: 0){ _LL2120: _temp2119=(( struct Cyc_Absyn_VarType_struct*)
_temp2073)->f1; goto _LL2076;} else{ goto _LL2077;} _LL2077: if(( unsigned int)
_temp2073 > 4u?*(( int*) _temp2073) == Cyc_Absyn_StructType: 0){ _LL2126:
_temp2125=(( struct Cyc_Absyn_StructType_struct*) _temp2073)->f1; goto _LL2124;
_LL2124: _temp2123=(( struct Cyc_Absyn_StructType_struct*) _temp2073)->f2; goto
_LL2122; _LL2122: _temp2121=(( struct Cyc_Absyn_StructType_struct*) _temp2073)->f3;
goto _LL2078;} else{ goto _LL2079;} _LL2079: if(( unsigned int) _temp2073 > 4u?*((
int*) _temp2073) == Cyc_Absyn_UnionType: 0){ _LL2132: _temp2131=(( struct Cyc_Absyn_UnionType_struct*)
_temp2073)->f1; goto _LL2130; _LL2130: _temp2129=(( struct Cyc_Absyn_UnionType_struct*)
_temp2073)->f2; goto _LL2128; _LL2128: _temp2127=(( struct Cyc_Absyn_UnionType_struct*)
_temp2073)->f3; goto _LL2080;} else{ goto _LL2081;} _LL2081: if(( unsigned int)
_temp2073 > 4u?*(( int*) _temp2073) == Cyc_Absyn_TunionType: 0){ _LL2134:
_temp2133=(( struct Cyc_Absyn_TunionType_struct*) _temp2073)->f1; _LL2140:
_temp2139=( void*) _temp2133.tunion_info; goto _LL2138; _LL2138: _temp2137=
_temp2133.targs; goto _LL2136; _LL2136: _temp2135=( void*) _temp2133.rgn; goto
_LL2082;} else{ goto _LL2083;} _LL2083: if(( unsigned int) _temp2073 > 4u?*((
int*) _temp2073) == Cyc_Absyn_TunionFieldType: 0){ _LL2142: _temp2141=(( struct
Cyc_Absyn_TunionFieldType_struct*) _temp2073)->f1; _LL2146: _temp2145=( void*)
_temp2141.field_info; goto _LL2144; _LL2144: _temp2143= _temp2141.targs; goto
_LL2084;} else{ goto _LL2085;} _LL2085: if(( unsigned int) _temp2073 > 4u?*((
int*) _temp2073) == Cyc_Absyn_TypedefType: 0){ _LL2152: _temp2151=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2073)->f1; goto _LL2150; _LL2150: _temp2149=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2073)->f2; goto _LL2148; _LL2148: _temp2147=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2073)->f3; goto _LL2086;} else{ goto _LL2087;} _LL2087: if(( unsigned int)
_temp2073 > 4u?*(( int*) _temp2073) == Cyc_Absyn_ArrayType: 0){ _LL2158:
_temp2157=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2073)->f1; goto
_LL2156; _LL2156: _temp2155=(( struct Cyc_Absyn_ArrayType_struct*) _temp2073)->f2;
goto _LL2154; _LL2154: _temp2153=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2073)->f3; goto _LL2088;} else{ goto _LL2089;} _LL2089: if(( unsigned int)
_temp2073 > 4u?*(( int*) _temp2073) == Cyc_Absyn_PointerType: 0){ _LL2160:
_temp2159=(( struct Cyc_Absyn_PointerType_struct*) _temp2073)->f1; _LL2170:
_temp2169=( void*) _temp2159.elt_typ; goto _LL2168; _LL2168: _temp2167=( void*)
_temp2159.rgn_typ; goto _LL2166; _LL2166: _temp2165= _temp2159.nullable; goto
_LL2164; _LL2164: _temp2163= _temp2159.tq; goto _LL2162; _LL2162: _temp2161=
_temp2159.bounds; goto _LL2090;} else{ goto _LL2091;} _LL2091: if(( unsigned int)
_temp2073 > 4u?*(( int*) _temp2073) == Cyc_Absyn_FnType: 0){ _LL2172: _temp2171=((
struct Cyc_Absyn_FnType_struct*) _temp2073)->f1; _LL2186: _temp2185= _temp2171.tvars;
goto _LL2184; _LL2184: _temp2183= _temp2171.effect; goto _LL2182; _LL2182:
_temp2181=( void*) _temp2171.ret_typ; goto _LL2180; _LL2180: _temp2179=
_temp2171.args; goto _LL2178; _LL2178: _temp2177= _temp2171.c_varargs; goto
_LL2176; _LL2176: _temp2175= _temp2171.cyc_varargs; goto _LL2174; _LL2174:
_temp2173= _temp2171.attributes; goto _LL2092;} else{ goto _LL2093;} _LL2093:
if(( unsigned int) _temp2073 > 4u?*(( int*) _temp2073) == Cyc_Absyn_TupleType: 0){
_LL2188: _temp2187=(( struct Cyc_Absyn_TupleType_struct*) _temp2073)->f1; goto
_LL2094;} else{ goto _LL2095;} _LL2095: if(( unsigned int) _temp2073 > 4u?*((
int*) _temp2073) == Cyc_Absyn_AnonStructType: 0){ _LL2190: _temp2189=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp2073)->f1; goto _LL2096;} else{ goto
_LL2097;} _LL2097: if(( unsigned int) _temp2073 > 4u?*(( int*) _temp2073) == Cyc_Absyn_AnonUnionType:
0){ _LL2192: _temp2191=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp2073)->f1;
goto _LL2098;} else{ goto _LL2099;} _LL2099: if(( unsigned int) _temp2073 > 4u?*((
int*) _temp2073) == Cyc_Absyn_Evar: 0){ _LL2200: _temp2199=(( struct Cyc_Absyn_Evar_struct*)
_temp2073)->f1; goto _LL2198; _LL2198: _temp2197=(( struct Cyc_Absyn_Evar_struct*)
_temp2073)->f2; goto _LL2196; _LL2196: _temp2195=(( struct Cyc_Absyn_Evar_struct*)
_temp2073)->f3; goto _LL2194; _LL2194: _temp2193=(( struct Cyc_Absyn_Evar_struct*)
_temp2073)->f4; goto _LL2100;} else{ goto _LL2101;} _LL2101: if(( unsigned int)
_temp2073 > 4u?*(( int*) _temp2073) == Cyc_Absyn_RgnHandleType: 0){ _LL2202:
_temp2201=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp2073)->f1;
goto _LL2102;} else{ goto _LL2103;} _LL2103: if(( unsigned int) _temp2073 > 4u?*((
int*) _temp2073) == Cyc_Absyn_EnumType: 0){ _LL2206: _temp2205=(( struct Cyc_Absyn_EnumType_struct*)
_temp2073)->f1; goto _LL2204; _LL2204: _temp2203=(( struct Cyc_Absyn_EnumType_struct*)
_temp2073)->f2; goto _LL2104;} else{ goto _LL2105;} _LL2105: if( _temp2073 ==(
void*) Cyc_Absyn_VoidType){ goto _LL2106;} else{ goto _LL2107;} _LL2107: if((
unsigned int) _temp2073 > 4u?*(( int*) _temp2073) == Cyc_Absyn_IntType: 0){
_LL2210: _temp2209=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2073)->f1;
goto _LL2208; _LL2208: _temp2207=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2073)->f2; goto _LL2108;} else{ goto _LL2109;} _LL2109: if( _temp2073 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2110;} else{ goto _LL2111;} _LL2111: if(
_temp2073 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2112;} else{ goto _LL2113;}
_LL2113: if( _temp2073 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL2114;} else{ goto
_LL2115;} _LL2115: if(( unsigned int) _temp2073 > 4u?*(( int*) _temp2073) == Cyc_Absyn_AccessEff:
0){ _LL2212: _temp2211=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp2073)->f1;
goto _LL2116;} else{ goto _LL2117;} _LL2117: if(( unsigned int) _temp2073 > 4u?*((
int*) _temp2073) == Cyc_Absyn_JoinEff: 0){ _LL2214: _temp2213=(( struct Cyc_Absyn_JoinEff_struct*)
_temp2073)->f1; goto _LL2118;} else{ goto _LL2074;} _LL2076: { struct
_handler_cons _temp2215; _push_handler(& _temp2215);{ int _temp2217= 0; if(
setjmp( _temp2215.handler)){ _temp2217= 1;} if( ! _temp2217){{ void* _temp2218=((
void*(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp,
inst, _temp2119); _npop_handler( 0u); return _temp2218;}; _pop_handler();} else{
void* _temp2216=( void*) _exn_thrown; void* _temp2220= _temp2216; _LL2222: if(
_temp2220 == Cyc_Core_Not_found){ goto _LL2223;} else{ goto _LL2224;} _LL2224:
goto _LL2225; _LL2223: return t; _LL2225:( void) _throw( _temp2220); _LL2221:;}}}
_LL2078: { struct Cyc_List_List* _temp2226= Cyc_Tcutil_substs( rgn, inst,
_temp2123); goto _LL2227; _LL2227: return _temp2226 == _temp2123? t:( void*)({
struct Cyc_Absyn_StructType_struct* _temp2228=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp2228[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp2229; _temp2229.tag= Cyc_Absyn_StructType;
_temp2229.f1= _temp2125; _temp2229.f2= _temp2226; _temp2229.f3= _temp2121;
_temp2229;}); _temp2228;});} _LL2080: { struct Cyc_List_List* _temp2230= Cyc_Tcutil_substs(
rgn, inst, _temp2129); goto _LL2231; _LL2231: return _temp2230 == _temp2129? t:(
void*)({ struct Cyc_Absyn_UnionType_struct* _temp2232=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp2232[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp2233; _temp2233.tag= Cyc_Absyn_UnionType;
_temp2233.f1= _temp2131; _temp2233.f2= _temp2230; _temp2233.f3= _temp2127;
_temp2233;}); _temp2232;});} _LL2082: { struct Cyc_List_List* _temp2234= Cyc_Tcutil_substs(
rgn, inst, _temp2137); goto _LL2235; _LL2235: { void* _temp2236= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2135); goto _LL2237; _LL2237: return( _temp2234 == _temp2137?
_temp2236 == _temp2135: 0)? t:( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp2238=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp2238[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp2239; _temp2239.tag=
Cyc_Absyn_TunionType; _temp2239.f1=({ struct Cyc_Absyn_TunionInfo _temp2240;
_temp2240.tunion_info=( void*) _temp2139; _temp2240.targs= _temp2234; _temp2240.rgn=(
void*) _temp2236; _temp2240;}); _temp2239;}); _temp2238;});}} _LL2084: { struct
Cyc_List_List* _temp2241= Cyc_Tcutil_substs( rgn, inst, _temp2143); goto _LL2242;
_LL2242: return _temp2241 == _temp2143? t:( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp2243=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp2243[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp2244; _temp2244.tag= Cyc_Absyn_TunionFieldType; _temp2244.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2245; _temp2245.field_info=( void*) _temp2145; _temp2245.targs= _temp2241;
_temp2245;}); _temp2244;}); _temp2243;});} _LL2086: { struct Cyc_List_List*
_temp2246= Cyc_Tcutil_substs( rgn, inst, _temp2149); goto _LL2247; _LL2247:
return _temp2246 == _temp2149? t:( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp2248=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2248[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp2249; _temp2249.tag=
Cyc_Absyn_TypedefType; _temp2249.f1= _temp2151; _temp2249.f2= _temp2246;
_temp2249.f3= _temp2147; _temp2249;}); _temp2248;});} _LL2088: { void* _temp2250=
Cyc_Tcutil_rsubstitute( rgn, inst, _temp2157); goto _LL2251; _LL2251: return
_temp2250 == _temp2157? t:( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp2252=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp2252[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp2253; _temp2253.tag= Cyc_Absyn_ArrayType;
_temp2253.f1=( void*) _temp2250; _temp2253.f2= _temp2155; _temp2253.f3=
_temp2153; _temp2253;}); _temp2252;});} _LL2090: { void* _temp2254= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2169); goto _LL2255; _LL2255: { void* _temp2256= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2167); goto _LL2257; _LL2257: if( _temp2254 == _temp2169?
_temp2256 == _temp2167: 0){ return t;} return( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp2258=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp2258[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp2259; _temp2259.tag=
Cyc_Absyn_PointerType; _temp2259.f1=({ struct Cyc_Absyn_PtrInfo _temp2260;
_temp2260.elt_typ=( void*) _temp2254; _temp2260.rgn_typ=( void*) _temp2256;
_temp2260.nullable= _temp2165; _temp2260.tq= _temp2163; _temp2260.bounds=
_temp2161; _temp2260;}); _temp2259;}); _temp2258;});}} _LL2092:{ struct Cyc_List_List*
_temp2261= _temp2185; goto _LL2262; _LL2262: for( 0; _temp2261 != 0; _temp2261=((
struct Cyc_List_List*) _check_null( _temp2261))->tl){ inst=({ struct Cyc_List_List*
_temp2263=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2263->hd=( void*)({ struct _tuple5* _temp2264=( struct _tuple5*)
_region_malloc( rgn, sizeof( struct _tuple5)); _temp2264->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp2261))->hd; _temp2264->f2=( void*)({
struct Cyc_Absyn_VarType_struct* _temp2265=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2265[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2266; _temp2266.tag= Cyc_Absyn_VarType; _temp2266.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp2261))->hd;
_temp2266;}); _temp2265;}); _temp2264;}); _temp2263->tl= inst; _temp2263;});}}{
struct Cyc_List_List* _temp2269; struct Cyc_List_List* _temp2271; struct _tuple0
_temp2267=(( struct _tuple0(*)( struct _RegionHandle* r3, struct _RegionHandle*
r4, struct Cyc_List_List* x)) Cyc_List_rsplit)( rgn, rgn,(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple16*(* f)( struct _RegionHandle*, struct
_tuple2*), struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)(
rgn, Cyc_Tcutil_substitute_f1, rgn, _temp2179)); _LL2272: _temp2271= _temp2267.f1;
goto _LL2270; _LL2270: _temp2269= _temp2267.f2; goto _LL2268; _LL2268: { struct
Cyc_List_List* _temp2273= Cyc_Tcutil_substs( rgn, inst, _temp2269); goto _LL2274;
_LL2274: { struct Cyc_List_List* _temp2275=(( struct Cyc_List_List*(*)( struct
_tuple2*(* f)( struct _tuple16*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct _RegionHandle* r4,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn,
_temp2271, _temp2273)); goto _LL2276; _LL2276: { struct Cyc_Core_Opt* eff2; if(
_temp2183 == 0){ eff2= 0;} else{ void* _temp2277= Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2183))->v); goto _LL2278;
_LL2278: if( _temp2277 ==( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2183))->v){
eff2= _temp2183;} else{ eff2=({ struct Cyc_Core_Opt* _temp2279=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2279->v=( void*) _temp2277;
_temp2279;});}}{ struct Cyc_Absyn_VarargInfo* cyc_varargs2; if( _temp2175 == 0){
cyc_varargs2= 0;} else{ int _temp2282; void* _temp2284; void* _temp2286; struct
Cyc_Absyn_Tqual _temp2288; struct Cyc_Core_Opt* _temp2290; struct Cyc_Absyn_VarargInfo
_temp2280=*(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp2175)); _LL2291:
_temp2290= _temp2280.name; goto _LL2289; _LL2289: _temp2288= _temp2280.tq; goto
_LL2287; _LL2287: _temp2286=( void*) _temp2280.type; goto _LL2285; _LL2285:
_temp2284=( void*) _temp2280.rgn; goto _LL2283; _LL2283: _temp2282= _temp2280.inject;
goto _LL2281; _LL2281: { void* _temp2292= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2286); goto _LL2293; _LL2293: { void* _temp2294= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2284); goto _LL2295; _LL2295: if( _temp2292 == _temp2286?
_temp2284 == _temp2294: 0){ cyc_varargs2= _temp2175;} else{ cyc_varargs2=({
struct Cyc_Absyn_VarargInfo* _temp2296=( struct Cyc_Absyn_VarargInfo*) GC_malloc(
sizeof( struct Cyc_Absyn_VarargInfo)); _temp2296->name= _temp2290; _temp2296->tq=
_temp2288; _temp2296->type=( void*) _temp2292; _temp2296->rgn=( void*) _temp2294;
_temp2296->inject= _temp2282; _temp2296;});}}}} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2297=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2297[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2298; _temp2298.tag= Cyc_Absyn_FnType;
_temp2298.f1=({ struct Cyc_Absyn_FnInfo _temp2299; _temp2299.tvars= _temp2185;
_temp2299.effect= eff2; _temp2299.ret_typ=( void*) Cyc_Tcutil_rsubstitute( rgn,
inst, _temp2181); _temp2299.args= _temp2275; _temp2299.c_varargs= _temp2177;
_temp2299.cyc_varargs= cyc_varargs2; _temp2299.attributes= _temp2173; _temp2299;});
_temp2298;}); _temp2297;});}}}}} _LL2094: { struct Cyc_List_List* _temp2300=((
struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_snd_tqt, _temp2187);
goto _LL2301; _LL2301: { struct Cyc_List_List* _temp2302= Cyc_Tcutil_substs( rgn,
inst, _temp2300); goto _LL2303; _LL2303: if( _temp2302 == _temp2300){ return t;}{
struct Cyc_List_List* _temp2304=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)(
struct _tuple4*, void*), struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_map2)(
Cyc_Tcutil_map2_tq, _temp2187, _temp2302); goto _LL2305; _LL2305: return( void*)({
struct Cyc_Absyn_TupleType_struct* _temp2306=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp2306[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp2307; _temp2307.tag= Cyc_Absyn_TupleType;
_temp2307.f1= _temp2304; _temp2307;}); _temp2306;});}}} _LL2096: { struct Cyc_List_List*
_temp2308=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2189); goto _LL2309; _LL2309: { struct Cyc_List_List* _temp2310= Cyc_Tcutil_substs(
rgn, inst, _temp2308); goto _LL2311; _LL2311: if( _temp2310 == _temp2308){
return t;}{ struct Cyc_List_List* _temp2312=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type,
_temp2189, _temp2310); goto _LL2313; _LL2313: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp2314=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp2314[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp2315; _temp2315.tag= Cyc_Absyn_AnonStructType; _temp2315.f1= _temp2312;
_temp2315;}); _temp2314;});}}} _LL2098: { struct Cyc_List_List* _temp2316=((
struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type, _temp2191);
goto _LL2317; _LL2317: { struct Cyc_List_List* _temp2318= Cyc_Tcutil_substs( rgn,
inst, _temp2316); goto _LL2319; _LL2319: if( _temp2318 == _temp2316){ return t;}{
struct Cyc_List_List* _temp2320=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2191, _temp2318); goto
_LL2321; _LL2321: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp2322=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp2322[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp2323; _temp2323.tag= Cyc_Absyn_AnonStructType; _temp2323.f1= _temp2320;
_temp2323;}); _temp2322;});}}} _LL2100: if( _temp2197 != 0){ return Cyc_Tcutil_rsubstitute(
rgn, inst,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2197))->v);} else{
return t;} _LL2102: { void* _temp2324= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2201); goto _LL2325; _LL2325: return _temp2324 == _temp2201? t:( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp2326=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp2326[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp2327; _temp2327.tag= Cyc_Absyn_RgnHandleType;
_temp2327.f1=( void*) _temp2324; _temp2327;}); _temp2326;});} _LL2104: return t;
_LL2106: return t; _LL2108: return t; _LL2110: return t; _LL2112: return t;
_LL2114: return t; _LL2116: { void* _temp2328= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2211); goto _LL2329; _LL2329: return _temp2328 == _temp2211? t:( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2330=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2330[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2331; _temp2331.tag= Cyc_Absyn_AccessEff;
_temp2331.f1=( void*) _temp2328; _temp2331;}); _temp2330;});} _LL2118: { struct
Cyc_List_List* _temp2332= Cyc_Tcutil_substs( rgn, inst, _temp2213); goto _LL2333;
_LL2333: return _temp2332 == _temp2213? t:( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp2334=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2334[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2335; _temp2335.tag= Cyc_Absyn_JoinEff;
_temp2335.f1= _temp2332; _temp2335;}); _temp2334;});} _LL2074:;} static struct
Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List*
inst, struct Cyc_List_List* ts){ if( ts == 0){ return 0;}{ void* _temp2336=(
void*)(( struct Cyc_List_List*) _check_null( ts))->hd; goto _LL2337; _LL2337: {
struct Cyc_List_List* _temp2338=(( struct Cyc_List_List*) _check_null( ts))->tl;
goto _LL2339; _LL2339: { void* _temp2340= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2336); goto _LL2341; _LL2341: { struct Cyc_List_List* _temp2342= Cyc_Tcutil_substs(
rgn, inst, _temp2338); goto _LL2343; _LL2343: if( _temp2336 == _temp2340?
_temp2338 == _temp2342: 0){ return ts;} return( struct Cyc_List_List*)({ struct
Cyc_List_List* _temp2344=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2344->hd=( void*) _temp2340; _temp2344->tl= _temp2342; _temp2344;});}}}}}
extern void* Cyc_Tcutil_substitute( struct Cyc_List_List* inst, void* t){ return
Cyc_Tcutil_rsubstitute( Cyc_Core_heap_region, inst, t);} struct Cyc_Absyn_Exp*
Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv* te, void* t, struct Cyc_Position_Segment*
loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2394=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2394[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2395; _temp2395.tag= Cyc_Absyn_Const_e;
_temp2395.f1=( void*)(( void*) Cyc_Absyn_Null_c); _temp2395;}); _temp2394;}),
loc);{ void* _temp2345= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp2359; void* _temp2361; void* _temp2363; void* _temp2365; void* _temp2367;
_LL2347: if(( unsigned int) _temp2345 > 4u?*(( int*) _temp2345) == Cyc_Absyn_PointerType:
0){ _LL2360: _temp2359=(( struct Cyc_Absyn_PointerType_struct*) _temp2345)->f1;
goto _LL2348;} else{ goto _LL2349;} _LL2349: if(( unsigned int) _temp2345 > 4u?*((
int*) _temp2345) == Cyc_Absyn_IntType: 0){ _LL2364: _temp2363=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2345)->f1; goto _LL2362; _LL2362: _temp2361=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2345)->f2; if( _temp2361 ==(
void*) Cyc_Absyn_B1){ goto _LL2350;} else{ goto _LL2351;}} else{ goto _LL2351;}
_LL2351: if(( unsigned int) _temp2345 > 4u?*(( int*) _temp2345) == Cyc_Absyn_IntType:
0){ _LL2368: _temp2367=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2345)->f1;
goto _LL2366; _LL2366: _temp2365=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2345)->f2; goto _LL2352;} else{ goto _LL2353;} _LL2353: if( _temp2345 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2354;} else{ goto _LL2355;} _LL2355: if(
_temp2345 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2356;} else{ goto _LL2357;}
_LL2357: goto _LL2358; _LL2348: goto _LL2346; _LL2350:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2369=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2369[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2370; _temp2370.tag= Cyc_Absyn_Const_e; _temp2370.f1=(
void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp2371=( struct Cyc_Absyn_Char_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct)); _temp2371[ 0]=({ struct Cyc_Absyn_Char_c_struct
_temp2372; _temp2372.tag= Cyc_Absyn_Char_c; _temp2372.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp2372.f2='\000'; _temp2372;}); _temp2371;})); _temp2370;}); _temp2369;})));
goto _LL2346; _LL2352:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2373=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2373[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2374; _temp2374.tag= Cyc_Absyn_Const_e;
_temp2374.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp2375=(
struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp2375[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp2376; _temp2376.tag= Cyc_Absyn_Int_c;
_temp2376.f1=( void*) _temp2367; _temp2376.f2= 0; _temp2376;}); _temp2375;}));
_temp2374;}); _temp2373;}))); if( _temp2365 !=( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2377=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2377[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2378; _temp2378.tag= Cyc_Absyn_Cast_e; _temp2378.f1=( void*) t; _temp2378.f2=
e; _temp2378;}); _temp2377;}), loc);} goto _LL2346; _LL2354:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2379=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2379[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2380; _temp2380.tag= Cyc_Absyn_Const_e; _temp2380.f1=(
void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2381=( struct Cyc_Absyn_Float_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct)); _temp2381[ 0]=({ struct
Cyc_Absyn_Float_c_struct _temp2382; _temp2382.tag= Cyc_Absyn_Float_c; _temp2382.f1=({
unsigned char* _temp2383=( unsigned char*)"0.0"; struct _tagged_string _temp2384;
_temp2384.curr= _temp2383; _temp2384.base= _temp2383; _temp2384.last_plus_one=
_temp2383 + 4; _temp2384;}); _temp2382;}); _temp2381;})); _temp2380;});
_temp2379;}))); goto _LL2346; _LL2356:( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_Cast_e_struct* _temp2385=( struct Cyc_Absyn_Cast_e_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2385[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2386; _temp2386.tag= Cyc_Absyn_Cast_e; _temp2386.f1=( void*) t; _temp2386.f2=
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2387=( struct
Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2387[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2388; _temp2388.tag= Cyc_Absyn_Const_e;
_temp2388.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2389=(
struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2389[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2390; _temp2390.tag= Cyc_Absyn_Float_c;
_temp2390.f1=({ unsigned char* _temp2391=( unsigned char*)"0.0"; struct
_tagged_string _temp2392; _temp2392.curr= _temp2391; _temp2392.base= _temp2391;
_temp2392.last_plus_one= _temp2391 + 4; _temp2392;}); _temp2390;}); _temp2389;}));
_temp2388;}); _temp2387;}), loc); _temp2386;}); _temp2385;}))); goto _LL2346;
_LL2358: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string
_temp2393= Cyc_Absynpp_typ2string( t); xprintf("declaration of type %.*s requires initializer",
_temp2393.last_plus_one - _temp2393.curr, _temp2393.curr);})); goto _LL2346;
_LL2346:;} return e;} struct _tuple5* Cyc_Tcutil_make_inst_var( struct Cyc_List_List*
s, struct Cyc_Absyn_Tvar* tv){ void* k=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( tv->kind); return({ struct _tuple5* _temp2396=( struct
_tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp2396->f1= tv; _temp2396->f2=
Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2397=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2397->v=( void*) k; _temp2397;}),({
struct Cyc_Core_Opt* _temp2398=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2398->v=( void*) s; _temp2398;})); _temp2396;});} struct
_tuple5* Cyc_Tcutil_r_make_inst_var( struct _tuple6* env, struct Cyc_Absyn_Tvar*
tv){ struct _tuple6 _temp2401; struct _RegionHandle* _temp2402; struct Cyc_List_List*
_temp2404; struct _tuple6* _temp2399= env; _temp2401=* _temp2399; _LL2405:
_temp2404= _temp2401.f1; goto _LL2403; _LL2403: _temp2402= _temp2401.f2; goto
_LL2400; _LL2400: { void* k=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
tv->kind); return({ struct _tuple5* _temp2406=( struct _tuple5*) _region_malloc(
_temp2402, sizeof( struct _tuple5)); _temp2406->f1= tv; _temp2406->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp2407=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2407->v=( void*) k; _temp2407;}),({ struct Cyc_Core_Opt*
_temp2408=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2408->v=( void*) _temp2404; _temp2408;})); _temp2406;});}} static struct
Cyc_List_List* Cyc_Tcutil_add_free_tvar( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 != 0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
if( Cyc_String_zstrptrcmp((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->name, tv->name) == 0){ struct Cyc_Absyn_Conref* k1=((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->kind;
struct Cyc_Absyn_Conref* k2= tv->kind; if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string _temp2409=*
tv->name; struct _tagged_string _temp2410= Cyc_Absynpp_ckind2string( k1); struct
_tagged_string _temp2411= Cyc_Absynpp_ckind2string( k2); xprintf("type variable %.*s is used with inconsistent kinds %.*s and %.*s",
_temp2409.last_plus_one - _temp2409.curr, _temp2409.curr, _temp2410.last_plus_one
- _temp2410.curr, _temp2410.curr, _temp2411.last_plus_one - _temp2411.curr,
_temp2411.curr);}));} if( tv->identity == 0){ tv->identity=(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->identity;} else{ if(*(( int*)
_check_null( tv->identity)) !=*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->identity))){(( void(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string) xprintf("same type variable has different identity!"));}}
return tvs;}}} tv->identity= Cyc_Tcutil_new_tvar_id(); return({ struct Cyc_List_List*
_temp2412=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2412->hd=( void*) tv; _temp2412->tl= tvs; _temp2412;});} static struct Cyc_List_List*
Cyc_Tcutil_fast_add_free_tvar( struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar*
tv){ if( tv->identity == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
unsigned char* _temp2413=( unsigned char*)"fast_add_free_tvar: bad identity in tv";
struct _tagged_string _temp2414; _temp2414.curr= _temp2413; _temp2414.base=
_temp2413; _temp2414.last_plus_one= _temp2413 + 39; _temp2414;}));}{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 != 0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
if((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->identity
== 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char*
_temp2415=( unsigned char*)"fast_add_free_tvar: bad identity in tvs2"; struct
_tagged_string _temp2416; _temp2416.curr= _temp2415; _temp2416.base= _temp2415;
_temp2416.last_plus_one= _temp2415 + 41; _temp2416;}));} if(*(( int*)
_check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
tvs2))->hd)->identity)) ==*(( int*) _check_null( tv->identity))){ return tvs;}}}
return({ struct Cyc_List_List* _temp2417=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2417->hd=( void*) tv; _temp2417->tl= tvs;
_temp2417;});} static struct Cyc_List_List* Cyc_Tcutil_add_bound_tvar( struct
Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){ if( tv->identity == 0){(( void(*)(
struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({ struct
_tagged_string _temp2418= Cyc_Tcutil_tvar2string( tv); xprintf("bound tvar id for %.*s is null",
_temp2418.last_plus_one - _temp2418.curr, _temp2418.curr);}));} return({ struct
Cyc_List_List* _temp2419=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2419->hd=( void*) tv; _temp2419->tl= tvs; _temp2419;});} static struct Cyc_List_List*
Cyc_Tcutil_add_free_evar( struct Cyc_List_List* es, void* e){ void* _temp2420=
Cyc_Tcutil_compress( e); struct Cyc_Core_Opt* _temp2426; int _temp2428; struct
Cyc_Core_Opt* _temp2430; struct Cyc_Core_Opt* _temp2432; _LL2422: if((
unsigned int) _temp2420 > 4u?*(( int*) _temp2420) == Cyc_Absyn_Evar: 0){ _LL2433:
_temp2432=(( struct Cyc_Absyn_Evar_struct*) _temp2420)->f1; goto _LL2431;
_LL2431: _temp2430=(( struct Cyc_Absyn_Evar_struct*) _temp2420)->f2; goto
_LL2429; _LL2429: _temp2428=(( struct Cyc_Absyn_Evar_struct*) _temp2420)->f3;
goto _LL2427; _LL2427: _temp2426=(( struct Cyc_Absyn_Evar_struct*) _temp2420)->f4;
goto _LL2423;} else{ goto _LL2424;} _LL2424: goto _LL2425; _LL2423:{ struct Cyc_List_List*
es2= es; for( 0; es2 != 0; es2=(( struct Cyc_List_List*) _check_null( es2))->tl){
void* _temp2434= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*)
_check_null( es2))->hd); struct Cyc_Core_Opt* _temp2440; int _temp2442; struct
Cyc_Core_Opt* _temp2444; struct Cyc_Core_Opt* _temp2446; _LL2436: if((
unsigned int) _temp2434 > 4u?*(( int*) _temp2434) == Cyc_Absyn_Evar: 0){ _LL2447:
_temp2446=(( struct Cyc_Absyn_Evar_struct*) _temp2434)->f1; goto _LL2445;
_LL2445: _temp2444=(( struct Cyc_Absyn_Evar_struct*) _temp2434)->f2; goto
_LL2443; _LL2443: _temp2442=(( struct Cyc_Absyn_Evar_struct*) _temp2434)->f3;
goto _LL2441; _LL2441: _temp2440=(( struct Cyc_Absyn_Evar_struct*) _temp2434)->f4;
goto _LL2437;} else{ goto _LL2438;} _LL2438: goto _LL2439; _LL2437: if(
_temp2428 == _temp2442){ return es;} goto _LL2435; _LL2439: goto _LL2435;
_LL2435:;}} return({ struct Cyc_List_List* _temp2448=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2448->hd=( void*) e; _temp2448->tl=
es; _temp2448;}); _LL2425: return es; _LL2421:;} static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars( struct Cyc_List_List* tvs, struct Cyc_List_List*
btvs){ struct Cyc_List_List* r= 0; for( 0; tvs != 0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){ int present= 0;{ struct Cyc_List_List* b= btvs; for( 0;
b != 0; b=(( struct Cyc_List_List*) _check_null( b))->tl){ if(*(( int*)
_check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->identity))
==*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( b))->hd)->identity))){ present= 1; break;}}} if( ! present){ r=({
struct Cyc_List_List* _temp2449=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2449->hd=( void*)(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd); _temp2449->tl= r; _temp2449;});}}
r=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( r);
return r;} struct Cyc_Tcutil_CVTEnv{ struct Cyc_List_List* kind_env; struct Cyc_List_List*
free_vars; struct Cyc_List_List* free_evars; } ; static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcutil_CVTEnv cvtenv, void* expected_kind, void* t){{ void*
_temp2450= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt* _temp2496; int
_temp2498; struct Cyc_Core_Opt* _temp2500; struct Cyc_Core_Opt* _temp2502;
struct Cyc_Core_Opt** _temp2504; struct Cyc_Absyn_Tvar* _temp2505; struct Cyc_Absyn_Enumdecl*
_temp2507; struct Cyc_Absyn_Enumdecl** _temp2509; struct _tuple1* _temp2510;
struct Cyc_Absyn_TunionInfo _temp2512; void* _temp2514; struct Cyc_List_List*
_temp2516; void* _temp2518; void** _temp2520; struct Cyc_Absyn_TunionFieldInfo
_temp2521; struct Cyc_List_List* _temp2523; void* _temp2525; void** _temp2527;
struct Cyc_Absyn_PtrInfo _temp2528; struct Cyc_Absyn_Conref* _temp2530; struct
Cyc_Absyn_Tqual _temp2532; struct Cyc_Absyn_Conref* _temp2534; void* _temp2536;
void* _temp2538; void* _temp2540; void* _temp2542; struct Cyc_Absyn_Exp*
_temp2544; struct Cyc_Absyn_Tqual _temp2546; void* _temp2548; struct Cyc_Absyn_FnInfo
_temp2550; struct Cyc_List_List* _temp2552; struct Cyc_Absyn_VarargInfo*
_temp2554; int _temp2556; struct Cyc_List_List* _temp2558; void* _temp2560;
struct Cyc_Core_Opt* _temp2562; struct Cyc_Core_Opt** _temp2564; struct Cyc_List_List*
_temp2565; struct Cyc_List_List** _temp2567; struct Cyc_List_List* _temp2568;
struct Cyc_List_List* _temp2570; struct Cyc_List_List* _temp2572; struct Cyc_Absyn_Structdecl**
_temp2574; struct Cyc_Absyn_Structdecl*** _temp2576; struct Cyc_List_List*
_temp2577; struct _tuple1* _temp2579; struct Cyc_Absyn_Uniondecl** _temp2581;
struct Cyc_Absyn_Uniondecl*** _temp2583; struct Cyc_List_List* _temp2584; struct
_tuple1* _temp2586; struct Cyc_Core_Opt* _temp2588; struct Cyc_Core_Opt**
_temp2590; struct Cyc_List_List* _temp2591; struct _tuple1* _temp2593; void*
_temp2595; void* _temp2597; struct Cyc_List_List* _temp2599; _LL2452: if(
_temp2450 ==( void*) Cyc_Absyn_VoidType){ goto _LL2453;} else{ goto _LL2454;}
_LL2454: if(( unsigned int) _temp2450 > 4u?*(( int*) _temp2450) == Cyc_Absyn_Evar:
0){ _LL2503: _temp2502=(( struct Cyc_Absyn_Evar_struct*) _temp2450)->f1;
_temp2504=&(( struct Cyc_Absyn_Evar_struct*) _temp2450)->f1; goto _LL2501;
_LL2501: _temp2500=(( struct Cyc_Absyn_Evar_struct*) _temp2450)->f2; goto
_LL2499; _LL2499: _temp2498=(( struct Cyc_Absyn_Evar_struct*) _temp2450)->f3;
goto _LL2497; _LL2497: _temp2496=(( struct Cyc_Absyn_Evar_struct*) _temp2450)->f4;
goto _LL2455;} else{ goto _LL2456;} _LL2456: if(( unsigned int) _temp2450 > 4u?*((
int*) _temp2450) == Cyc_Absyn_VarType: 0){ _LL2506: _temp2505=(( struct Cyc_Absyn_VarType_struct*)
_temp2450)->f1; goto _LL2457;} else{ goto _LL2458;} _LL2458: if(( unsigned int)
_temp2450 > 4u?*(( int*) _temp2450) == Cyc_Absyn_EnumType: 0){ _LL2511:
_temp2510=(( struct Cyc_Absyn_EnumType_struct*) _temp2450)->f1; goto _LL2508;
_LL2508: _temp2507=(( struct Cyc_Absyn_EnumType_struct*) _temp2450)->f2;
_temp2509=&(( struct Cyc_Absyn_EnumType_struct*) _temp2450)->f2; goto _LL2459;}
else{ goto _LL2460;} _LL2460: if(( unsigned int) _temp2450 > 4u?*(( int*)
_temp2450) == Cyc_Absyn_TunionType: 0){ _LL2513: _temp2512=(( struct Cyc_Absyn_TunionType_struct*)
_temp2450)->f1; _LL2519: _temp2518=( void*) _temp2512.tunion_info; _temp2520=&(((
struct Cyc_Absyn_TunionType_struct*) _temp2450)->f1).tunion_info; goto _LL2517;
_LL2517: _temp2516= _temp2512.targs; goto _LL2515; _LL2515: _temp2514=( void*)
_temp2512.rgn; goto _LL2461;} else{ goto _LL2462;} _LL2462: if(( unsigned int)
_temp2450 > 4u?*(( int*) _temp2450) == Cyc_Absyn_TunionFieldType: 0){ _LL2522:
_temp2521=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2450)->f1; _LL2526:
_temp2525=( void*) _temp2521.field_info; _temp2527=&((( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2450)->f1).field_info; goto _LL2524; _LL2524: _temp2523= _temp2521.targs;
goto _LL2463;} else{ goto _LL2464;} _LL2464: if(( unsigned int) _temp2450 > 4u?*((
int*) _temp2450) == Cyc_Absyn_PointerType: 0){ _LL2529: _temp2528=(( struct Cyc_Absyn_PointerType_struct*)
_temp2450)->f1; _LL2539: _temp2538=( void*) _temp2528.elt_typ; goto _LL2537;
_LL2537: _temp2536=( void*) _temp2528.rgn_typ; goto _LL2535; _LL2535: _temp2534=
_temp2528.nullable; goto _LL2533; _LL2533: _temp2532= _temp2528.tq; goto _LL2531;
_LL2531: _temp2530= _temp2528.bounds; goto _LL2465;} else{ goto _LL2466;}
_LL2466: if(( unsigned int) _temp2450 > 4u?*(( int*) _temp2450) == Cyc_Absyn_IntType:
0){ _LL2543: _temp2542=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2450)->f1;
goto _LL2541; _LL2541: _temp2540=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2450)->f2; goto _LL2467;} else{ goto _LL2468;} _LL2468: if( _temp2450 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2469;} else{ goto _LL2470;} _LL2470: if(
_temp2450 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2471;} else{ goto _LL2472;}
_LL2472: if(( unsigned int) _temp2450 > 4u?*(( int*) _temp2450) == Cyc_Absyn_ArrayType:
0){ _LL2549: _temp2548=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2450)->f1;
goto _LL2547; _LL2547: _temp2546=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2450)->f2; goto _LL2545; _LL2545: _temp2544=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2450)->f3; goto _LL2473;} else{ goto _LL2474;} _LL2474: if(( unsigned int)
_temp2450 > 4u?*(( int*) _temp2450) == Cyc_Absyn_FnType: 0){ _LL2551: _temp2550=((
struct Cyc_Absyn_FnType_struct*) _temp2450)->f1; _LL2566: _temp2565= _temp2550.tvars;
_temp2567=&((( struct Cyc_Absyn_FnType_struct*) _temp2450)->f1).tvars; goto
_LL2563; _LL2563: _temp2562= _temp2550.effect; _temp2564=&((( struct Cyc_Absyn_FnType_struct*)
_temp2450)->f1).effect; goto _LL2561; _LL2561: _temp2560=( void*) _temp2550.ret_typ;
goto _LL2559; _LL2559: _temp2558= _temp2550.args; goto _LL2557; _LL2557:
_temp2556= _temp2550.c_varargs; goto _LL2555; _LL2555: _temp2554= _temp2550.cyc_varargs;
goto _LL2553; _LL2553: _temp2552= _temp2550.attributes; goto _LL2475;} else{
goto _LL2476;} _LL2476: if(( unsigned int) _temp2450 > 4u?*(( int*) _temp2450)
== Cyc_Absyn_TupleType: 0){ _LL2569: _temp2568=(( struct Cyc_Absyn_TupleType_struct*)
_temp2450)->f1; goto _LL2477;} else{ goto _LL2478;} _LL2478: if(( unsigned int)
_temp2450 > 4u?*(( int*) _temp2450) == Cyc_Absyn_AnonStructType: 0){ _LL2571:
_temp2570=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2450)->f1; goto
_LL2479;} else{ goto _LL2480;} _LL2480: if(( unsigned int) _temp2450 > 4u?*((
int*) _temp2450) == Cyc_Absyn_AnonUnionType: 0){ _LL2573: _temp2572=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp2450)->f1; goto _LL2481;} else{ goto
_LL2482;} _LL2482: if(( unsigned int) _temp2450 > 4u?*(( int*) _temp2450) == Cyc_Absyn_StructType:
0){ _LL2580: _temp2579=(( struct Cyc_Absyn_StructType_struct*) _temp2450)->f1;
goto _LL2578; _LL2578: _temp2577=(( struct Cyc_Absyn_StructType_struct*)
_temp2450)->f2; goto _LL2575; _LL2575: _temp2574=(( struct Cyc_Absyn_StructType_struct*)
_temp2450)->f3; _temp2576=&(( struct Cyc_Absyn_StructType_struct*) _temp2450)->f3;
goto _LL2483;} else{ goto _LL2484;} _LL2484: if(( unsigned int) _temp2450 > 4u?*((
int*) _temp2450) == Cyc_Absyn_UnionType: 0){ _LL2587: _temp2586=(( struct Cyc_Absyn_UnionType_struct*)
_temp2450)->f1; goto _LL2585; _LL2585: _temp2584=(( struct Cyc_Absyn_UnionType_struct*)
_temp2450)->f2; goto _LL2582; _LL2582: _temp2581=(( struct Cyc_Absyn_UnionType_struct*)
_temp2450)->f3; _temp2583=&(( struct Cyc_Absyn_UnionType_struct*) _temp2450)->f3;
goto _LL2485;} else{ goto _LL2486;} _LL2486: if(( unsigned int) _temp2450 > 4u?*((
int*) _temp2450) == Cyc_Absyn_TypedefType: 0){ _LL2594: _temp2593=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2450)->f1; goto _LL2592; _LL2592: _temp2591=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2450)->f2; goto _LL2589; _LL2589: _temp2588=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2450)->f3; _temp2590=&(( struct Cyc_Absyn_TypedefType_struct*) _temp2450)->f3;
goto _LL2487;} else{ goto _LL2488;} _LL2488: if( _temp2450 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL2489;} else{ goto _LL2490;} _LL2490: if(( unsigned int) _temp2450 > 4u?*((
int*) _temp2450) == Cyc_Absyn_RgnHandleType: 0){ _LL2596: _temp2595=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp2450)->f1; goto _LL2491;} else{
goto _LL2492;} _LL2492: if(( unsigned int) _temp2450 > 4u?*(( int*) _temp2450)
== Cyc_Absyn_AccessEff: 0){ _LL2598: _temp2597=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp2450)->f1; goto _LL2493;} else{ goto _LL2494;} _LL2494: if(( unsigned int)
_temp2450 > 4u?*(( int*) _temp2450) == Cyc_Absyn_JoinEff: 0){ _LL2600: _temp2599=((
struct Cyc_Absyn_JoinEff_struct*) _temp2450)->f1; goto _LL2495;} else{ goto
_LL2451;} _LL2453: goto _LL2451; _LL2455: if(* _temp2504 == 0){* _temp2504=({
struct Cyc_Core_Opt* _temp2601=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2601->v=( void*) expected_kind; _temp2601;});} cvtenv.free_evars=
Cyc_Tcutil_add_free_evar( cvtenv.free_evars, t); goto _LL2451; _LL2457: { struct
Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp2505->kind); if(( void*) c->v ==( void*) Cyc_Absyn_No_constr){(
void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2602=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2602[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2603; _temp2603.tag= Cyc_Absyn_Eq_constr;
_temp2603.f1=( void*) expected_kind; _temp2603;}); _temp2602;})));} cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar( loc, cvtenv.kind_env, _temp2505); cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars, _temp2505); goto _LL2451;}
_LL2459: if(* _temp2509 == 0){ struct _handler_cons _temp2604; _push_handler(&
_temp2604);{ int _temp2606= 0; if( setjmp( _temp2604.handler)){ _temp2606= 1;}
if( ! _temp2606){{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl( te,
loc, _temp2510);* _temp2509=( struct Cyc_Absyn_Enumdecl*)* ed;}; _pop_handler();}
else{ void* _temp2605=( void*) _exn_thrown; void* _temp2608= _temp2605; _LL2610:
if( _temp2608 == Cyc_Dict_Absent){ goto _LL2611;} else{ goto _LL2612;} _LL2612:
goto _LL2613; _LL2611: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp2614= Cyc_Absynpp_qvar2string( _temp2510); xprintf("unbound type enum %.*s",
_temp2614.last_plus_one - _temp2614.curr, _temp2614.curr);})); return cvtenv;
_LL2613:( void) _throw( _temp2608); _LL2609:;}}}{ struct Cyc_Absyn_Enumdecl* ed=(
struct Cyc_Absyn_Enumdecl*) _check_null(* _temp2509);* _temp2510=( ed->name)[
_check_known_subscript_notnull( 1u, 0)]; goto _LL2451;} _LL2461:{ void*
_temp2615=* _temp2520; struct Cyc_Absyn_UnknownTunionInfo _temp2621; int
_temp2623; struct _tuple1* _temp2625; struct Cyc_Absyn_Tuniondecl* _temp2627;
_LL2617: if(*(( int*) _temp2615) == Cyc_Absyn_UnknownTunion){ _LL2622: _temp2621=((
struct Cyc_Absyn_UnknownTunion_struct*) _temp2615)->f1; _LL2626: _temp2625=
_temp2621.name; goto _LL2624; _LL2624: _temp2623= _temp2621.is_xtunion; goto
_LL2618;} else{ goto _LL2619;} _LL2619: if(*(( int*) _temp2615) == Cyc_Absyn_KnownTunion){
_LL2628: _temp2627=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2615)->f1; goto
_LL2620;} else{ goto _LL2616;} _LL2618: { struct Cyc_Absyn_Tuniondecl* tud;{
struct _handler_cons _temp2629; _push_handler(& _temp2629);{ int _temp2631= 0;
if( setjmp( _temp2629.handler)){ _temp2631= 1;} if( ! _temp2631){ tud=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp2625);; _pop_handler();} else{ void* _temp2630=( void*)
_exn_thrown; void* _temp2633= _temp2630; _LL2635: if( _temp2633 == Cyc_Dict_Absent){
goto _LL2636;} else{ goto _LL2637;} _LL2637: goto _LL2638; _LL2636: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ struct _tagged_string _temp2639= Cyc_Absynpp_qvar2string(
_temp2625); xprintf("unbound type [x]tunion %.*s", _temp2639.last_plus_one -
_temp2639.curr, _temp2639.curr);})); return cvtenv; _LL2638:( void) _throw(
_temp2633); _LL2634:;}}} if( tud->is_xtunion != _temp2623){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp2640= Cyc_Absynpp_qvar2string(
_temp2625); xprintf("[x]tunion is different from type declaration %.*s",
_temp2640.last_plus_one - _temp2640.curr, _temp2640.curr);}));}* _temp2520=(
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp2641=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp2641[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp2642; _temp2642.tag= Cyc_Absyn_KnownTunion;
_temp2642.f1= tud; _temp2642;}); _temp2641;}); _temp2627= tud; goto _LL2620;}
_LL2620: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2514);{ struct Cyc_List_List* tvs= _temp2627->tvs; for( 0; _temp2516 != 0?
tvs != 0: 0;( _temp2516=(( struct Cyc_List_List*) _check_null( _temp2516))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2516))->hd; void* k1=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} if( _temp2516 != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp2643= Cyc_Absynpp_qvar2string(
_temp2627->name); xprintf("too many type arguments for tunion %.*s", _temp2643.last_plus_one
- _temp2643.curr, _temp2643.curr);}));} if( tvs != 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp2644= Cyc_Absynpp_qvar2string(
_temp2627->name); xprintf("too few type arguments for tunion %.*s", _temp2644.last_plus_one
- _temp2644.curr, _temp2644.curr);}));} goto _LL2616;} _LL2616:;} goto _LL2451;
_LL2463:{ void* _temp2645=* _temp2527; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp2651; int _temp2653; struct _tuple1* _temp2655; struct _tuple1* _temp2657;
struct Cyc_Absyn_Tunionfield* _temp2659; struct Cyc_Absyn_Tuniondecl* _temp2661;
_LL2647: if(*(( int*) _temp2645) == Cyc_Absyn_UnknownTunionfield){ _LL2652:
_temp2651=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp2645)->f1;
_LL2658: _temp2657= _temp2651.tunion_name; goto _LL2656; _LL2656: _temp2655=
_temp2651.field_name; goto _LL2654; _LL2654: _temp2653= _temp2651.is_xtunion;
goto _LL2648;} else{ goto _LL2649;} _LL2649: if(*(( int*) _temp2645) == Cyc_Absyn_KnownTunionfield){
_LL2662: _temp2661=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp2645)->f1;
goto _LL2660; _LL2660: _temp2659=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2645)->f2; goto _LL2650;} else{ goto _LL2646;} _LL2648: { struct Cyc_Absyn_Tuniondecl*
tud; struct Cyc_Absyn_Tunionfield* tuf;{ struct _handler_cons _temp2663;
_push_handler(& _temp2663);{ int _temp2665= 0; if( setjmp( _temp2663.handler)){
_temp2665= 1;} if( ! _temp2665){* Cyc_Tcenv_lookup_tuniondecl( te, loc,
_temp2657);; _pop_handler();} else{ void* _temp2664=( void*) _exn_thrown; void*
_temp2667= _temp2664; _LL2669: if( _temp2667 == Cyc_Dict_Absent){ goto _LL2670;}
else{ goto _LL2671;} _LL2671: goto _LL2672; _LL2670: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp2673= Cyc_Absynpp_qvar2string(
_temp2657); xprintf("unbound type tunion %.*s", _temp2673.last_plus_one -
_temp2673.curr, _temp2673.curr);})); return cvtenv; _LL2672:( void) _throw(
_temp2667); _LL2668:;}}}{ struct _handler_cons _temp2674; _push_handler(&
_temp2674);{ int _temp2676= 0; if( setjmp( _temp2674.handler)){ _temp2676= 1;}
if( ! _temp2676){{ void* _temp2677= Cyc_Tcenv_lookup_ordinary( te, loc,
_temp2655); struct Cyc_Absyn_Tunionfield* _temp2683; struct Cyc_Absyn_Tuniondecl*
_temp2685; _LL2679: if(*(( int*) _temp2677) == Cyc_Tcenv_TunionRes){ _LL2686:
_temp2685=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2677)->f1; goto _LL2684;
_LL2684: _temp2683=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2677)->f2; goto
_LL2680;} else{ goto _LL2681;} _LL2681: goto _LL2682; _LL2680: tuf= _temp2683;
tud= _temp2685; if( tud->is_xtunion != _temp2653){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp2687= Cyc_Absynpp_qvar2string(
_temp2657); xprintf("[x]tunion is different from type declaration %.*s",
_temp2687.last_plus_one - _temp2687.curr, _temp2687.curr);}));} goto _LL2678;
_LL2682: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string
_temp2688= Cyc_Absynpp_qvar2string( _temp2655); struct _tagged_string _temp2689=
Cyc_Absynpp_qvar2string( _temp2657); xprintf("unbound field %.*s in type tunion %.*s",
_temp2688.last_plus_one - _temp2688.curr, _temp2688.curr, _temp2689.last_plus_one
- _temp2689.curr, _temp2689.curr);}));{ struct Cyc_Tcutil_CVTEnv _temp2690=
cvtenv; _npop_handler( 0u); return _temp2690;} _LL2678:;}; _pop_handler();}
else{ void* _temp2675=( void*) _exn_thrown; void* _temp2692= _temp2675; _LL2694:
if( _temp2692 == Cyc_Dict_Absent){ goto _LL2695;} else{ goto _LL2696;} _LL2696:
goto _LL2697; _LL2695: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp2698= Cyc_Absynpp_qvar2string( _temp2655); struct
_tagged_string _temp2699= Cyc_Absynpp_qvar2string( _temp2657); xprintf("unbound field %.*s in type tunion %.*s",
_temp2698.last_plus_one - _temp2698.curr, _temp2698.curr, _temp2699.last_plus_one
- _temp2699.curr, _temp2699.curr);})); return cvtenv; _LL2697:( void) _throw(
_temp2692); _LL2693:;}}}* _temp2527=( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp2700=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp2700[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp2701; _temp2701.tag= Cyc_Absyn_KnownTunionfield; _temp2701.f1= tud;
_temp2701.f2= tuf; _temp2701;}); _temp2700;}); _temp2661= tud; _temp2659= tuf;
goto _LL2650;} _LL2650: { struct Cyc_List_List* tvs= _temp2661->tvs; for( 0;
_temp2523 != 0? tvs != 0: 0;( _temp2523=(( struct Cyc_List_List*) _check_null(
_temp2523))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2523))->hd; void* k1=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} if( _temp2523 != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp2702= Cyc_Absynpp_qvar2string(
_temp2661->name); struct _tagged_string _temp2703= Cyc_Absynpp_qvar2string(
_temp2659->name); xprintf("too many type arguments for tunion %.*s.%.*s",
_temp2702.last_plus_one - _temp2702.curr, _temp2702.curr, _temp2703.last_plus_one
- _temp2703.curr, _temp2703.curr);}));} if( tvs != 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp2704= Cyc_Absynpp_qvar2string(
_temp2661->name); struct _tagged_string _temp2705= Cyc_Absynpp_qvar2string(
_temp2659->name); xprintf("too few type arguments for tunion %.*s.%.*s",
_temp2704.last_plus_one - _temp2704.curr, _temp2704.curr, _temp2705.last_plus_one
- _temp2705.curr, _temp2705.curr);}));} goto _LL2646;} _LL2646:;} goto _LL2451;
_LL2465: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_AnyKind,
_temp2538); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2536);{ void* _temp2706=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp2530))->v; void* _temp2712; struct Cyc_Absyn_Exp*
_temp2714; _LL2708: if(( unsigned int) _temp2706 > 1u?*(( int*) _temp2706) ==
Cyc_Absyn_Eq_constr: 0){ _LL2713: _temp2712=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2706)->f1; if(( unsigned int) _temp2712 > 1u?*(( int*) _temp2712) == Cyc_Absyn_Upper_b:
0){ _LL2715: _temp2714=(( struct Cyc_Absyn_Upper_b_struct*) _temp2712)->f1; goto
_LL2709;} else{ goto _LL2710;}} else{ goto _LL2710;} _LL2710: goto _LL2711;
_LL2709: if( ! Cyc_Tcutil_is_const_exp( te, _temp2714)){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp2716=( unsigned char*)"pointer bounds expression is not a constant";
struct _tagged_string _temp2717; _temp2717.curr= _temp2716; _temp2717.base=
_temp2716; _temp2717.last_plus_one= _temp2716 + 44; _temp2717;}));} Cyc_Tcexp_tcExp(
te, 0, _temp2714); if( ! Cyc_Tcutil_coerce_uint_typ( te, _temp2714)){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp2718=( unsigned char*)"pointer bounds expression is not an unsigned int";
struct _tagged_string _temp2719; _temp2719.curr= _temp2718; _temp2719.base=
_temp2718; _temp2719.last_plus_one= _temp2718 + 49; _temp2719;}));} Cyc_Evexp_eval_const_uint_exp(
_temp2714); goto _LL2707; _LL2711: goto _LL2707; _LL2707:;} goto _LL2451;
_LL2467: goto _LL2451; _LL2469: goto _LL2451; _LL2471: goto _LL2451; _LL2473:
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,
_temp2548); if( _temp2544 == 0? 1: ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp2544))){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp2720=(
unsigned char*)"array bounds expression is not constant"; struct _tagged_string
_temp2721; _temp2721.curr= _temp2720; _temp2721.base= _temp2720; _temp2721.last_plus_one=
_temp2720 + 40; _temp2721;}));} Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)
_check_null( _temp2544)); if( ! Cyc_Tcutil_coerce_uint_typ( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp2544))){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp2722=(
unsigned char*)"array bounds expression is not an unsigned int"; struct
_tagged_string _temp2723; _temp2723.curr= _temp2722; _temp2723.base= _temp2722;
_temp2723.last_plus_one= _temp2722 + 47; _temp2723;}));} Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2544)); goto _LL2451; _LL2475: { int
num_convs= 0; int seen_cdecl= 0; int seen_stdcall= 0; int seen_fastcall= 0; for(
0; _temp2552 != 0; _temp2552=(( struct Cyc_List_List*) _check_null( _temp2552))->tl){
if( ! Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*) _check_null(
_temp2552))->hd)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp2724= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null( _temp2552))->hd); xprintf("bad function type attribute %.*s",
_temp2724.last_plus_one - _temp2724.curr, _temp2724.curr);}));}{ void* _temp2725=(
void*)(( struct Cyc_List_List*) _check_null( _temp2552))->hd; _LL2727: if(
_temp2725 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL2728;} else{ goto _LL2729;}
_LL2729: if( _temp2725 ==( void*) Cyc_Absyn_Cdecl_att){ goto _LL2730;} else{
goto _LL2731;} _LL2731: if( _temp2725 ==( void*) Cyc_Absyn_Fastcall_att){ goto
_LL2732;} else{ goto _LL2733;} _LL2733: goto _LL2734; _LL2728: if( !
seen_stdcall){ seen_stdcall= 1; ++ num_convs;} goto _LL2726; _LL2730: if( !
seen_cdecl){ seen_cdecl= 1; ++ num_convs;} goto _LL2726; _LL2732: if( !
seen_fastcall){ seen_fastcall= 1; ++ num_convs;} goto _LL2726; _LL2734: goto
_LL2726; _LL2726:;}} if( num_convs > 1){ Cyc_Tcutil_terr( loc,( struct
_tagged_string) xprintf("function can't have multiple calling conventions"));}
Cyc_Tcutil_check_unique_tvars( loc,* _temp2567);{ struct Cyc_List_List* b=*
_temp2567; for( 0; b != 0; b=(( struct Cyc_List_List*) _check_null( b))->tl){((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd)->identity=
Cyc_Tcutil_new_tvar_id(); cvtenv.kind_env= Cyc_Tcutil_add_bound_tvar( cvtenv.kind_env,(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd);{ void*
_temp2735=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( b))->hd)->kind))->v; void* _temp2741; _LL2737: if(( unsigned int)
_temp2735 > 1u?*(( int*) _temp2735) == Cyc_Absyn_Eq_constr: 0){ _LL2742:
_temp2741=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2735)->f1; if(
_temp2741 ==( void*) Cyc_Absyn_MemKind){ goto _LL2738;} else{ goto _LL2739;}}
else{ goto _LL2739;} _LL2739: goto _LL2740; _LL2738: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp2743=*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( b))->hd)->name; xprintf("function attempts to abstract Mem type variable %.*s",
_temp2743.last_plus_one - _temp2743.curr, _temp2743.curr);})); goto _LL2736;
_LL2740: goto _LL2736; _LL2736:;}}}{ struct Cyc_Tcutil_CVTEnv _temp2745=({
struct Cyc_Tcutil_CVTEnv _temp2744; _temp2744.kind_env= cvtenv.kind_env;
_temp2744.free_vars= 0; _temp2744.free_evars= 0; _temp2744;}); goto _LL2746;
_LL2746: _temp2745= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2745,( void*)
Cyc_Absyn_MemKind, _temp2560); for( 0; _temp2558 != 0; _temp2558=(( struct Cyc_List_List*)
_check_null( _temp2558))->tl){ _temp2745= Cyc_Tcutil_i_check_valid_type( loc, te,
_temp2745,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp2558))->hd)).f3);} if( _temp2554 != 0){ if( _temp2556){(( void(*)(
struct _tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp2747=(
unsigned char*)"both c_vararg and cyc_vararg"; struct _tagged_string _temp2748;
_temp2748.curr= _temp2747; _temp2748.base= _temp2747; _temp2748.last_plus_one=
_temp2747 + 29; _temp2748;}));}{ int _temp2751; void* _temp2753; void* _temp2755;
struct Cyc_Absyn_Tqual _temp2757; struct Cyc_Core_Opt* _temp2759; struct Cyc_Absyn_VarargInfo
_temp2749=*(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp2554)); _LL2760:
_temp2759= _temp2749.name; goto _LL2758; _LL2758: _temp2757= _temp2749.tq; goto
_LL2756; _LL2756: _temp2755=( void*) _temp2749.type; goto _LL2754; _LL2754:
_temp2753=( void*) _temp2749.rgn; goto _LL2752; _LL2752: _temp2751= _temp2749.inject;
goto _LL2750; _LL2750: _temp2745= Cyc_Tcutil_i_check_valid_type( loc, te,
_temp2745,( void*) Cyc_Absyn_MemKind, _temp2755);{ void* _temp2761= Cyc_Tcutil_compress(
_temp2753); struct Cyc_Core_Opt* _temp2771; int _temp2773; struct Cyc_Core_Opt*
_temp2775; struct Cyc_Core_Opt* _temp2777; struct Cyc_Absyn_Tvar* _temp2779;
_LL2763: if(( unsigned int) _temp2761 > 4u?*(( int*) _temp2761) == Cyc_Absyn_Evar:
0){ _LL2778: _temp2777=(( struct Cyc_Absyn_Evar_struct*) _temp2761)->f1; goto
_LL2776; _LL2776: _temp2775=(( struct Cyc_Absyn_Evar_struct*) _temp2761)->f2;
goto _LL2774; _LL2774: _temp2773=(( struct Cyc_Absyn_Evar_struct*) _temp2761)->f3;
goto _LL2772; _LL2772: _temp2771=(( struct Cyc_Absyn_Evar_struct*) _temp2761)->f4;
goto _LL2764;} else{ goto _LL2765;} _LL2765: if(( unsigned int) _temp2761 > 4u?*((
int*) _temp2761) == Cyc_Absyn_VarType: 0){ _LL2780: _temp2779=(( struct Cyc_Absyn_VarType_struct*)
_temp2761)->f1; goto _LL2766;} else{ goto _LL2767;} _LL2767: if( _temp2761 ==(
void*) Cyc_Absyn_HeapRgn){ goto _LL2768;} else{ goto _LL2769;} _LL2769: goto
_LL2770; _LL2764: { void* vr_tvar=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2781=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2781[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2782; _temp2782.tag= Cyc_Absyn_VarType;
_temp2782.f1= Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_RgnKind); _temp2782;});
_temp2781;}); Cyc_Tcutil_unify( _temp2753, vr_tvar); goto _LL2762;} _LL2766:
goto _LL2762; _LL2768: Cyc_Tcutil_terr( loc,({ unsigned char* _temp2783=(
unsigned char*)"varargs should be in a region, not heap-allocated"; struct
_tagged_string _temp2784; _temp2784.curr= _temp2783; _temp2784.base= _temp2783;
_temp2784.last_plus_one= _temp2783 + 50; _temp2784;})); goto _LL2762; _LL2770:
goto _LL2762; _LL2762:;} _temp2745= Cyc_Tcutil_i_check_valid_type( loc, te,
_temp2745,( void*) Cyc_Absyn_RgnKind, _temp2753); if( _temp2751){ void*
_temp2785= Cyc_Tcutil_compress( _temp2755); struct Cyc_Absyn_TunionInfo
_temp2791; void* _temp2793; struct Cyc_List_List* _temp2795; void* _temp2797;
struct Cyc_Absyn_Tuniondecl* _temp2799; _LL2787: if(( unsigned int) _temp2785 >
4u?*(( int*) _temp2785) == Cyc_Absyn_TunionType: 0){ _LL2792: _temp2791=((
struct Cyc_Absyn_TunionType_struct*) _temp2785)->f1; _LL2798: _temp2797=( void*)
_temp2791.tunion_info; if(*(( int*) _temp2797) == Cyc_Absyn_KnownTunion){
_LL2800: _temp2799=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2797)->f1; goto
_LL2796;} else{ goto _LL2789;} _LL2796: _temp2795= _temp2791.targs; goto _LL2794;
_LL2794: _temp2793=( void*) _temp2791.rgn; goto _LL2788;} else{ goto _LL2789;}
_LL2789: goto _LL2790; _LL2788:{ void* _temp2801= Cyc_Tcutil_compress( _temp2793);
struct Cyc_Core_Opt* _temp2811; int _temp2813; struct Cyc_Core_Opt* _temp2815;
struct Cyc_Core_Opt* _temp2817; struct Cyc_Absyn_Tvar* _temp2819; _LL2803: if((
unsigned int) _temp2801 > 4u?*(( int*) _temp2801) == Cyc_Absyn_Evar: 0){ _LL2818:
_temp2817=(( struct Cyc_Absyn_Evar_struct*) _temp2801)->f1; goto _LL2816;
_LL2816: _temp2815=(( struct Cyc_Absyn_Evar_struct*) _temp2801)->f2; goto
_LL2814; _LL2814: _temp2813=(( struct Cyc_Absyn_Evar_struct*) _temp2801)->f3;
goto _LL2812; _LL2812: _temp2811=(( struct Cyc_Absyn_Evar_struct*) _temp2801)->f4;
goto _LL2804;} else{ goto _LL2805;} _LL2805: if(( unsigned int) _temp2801 > 4u?*((
int*) _temp2801) == Cyc_Absyn_VarType: 0){ _LL2820: _temp2819=(( struct Cyc_Absyn_VarType_struct*)
_temp2801)->f1; goto _LL2806;} else{ goto _LL2807;} _LL2807: if( _temp2801 ==(
void*) Cyc_Absyn_HeapRgn){ goto _LL2808;} else{ goto _LL2809;} _LL2809: goto
_LL2810; _LL2804: Cyc_Tcutil_unify( _temp2753, _temp2793); goto _LL2802; _LL2806:
goto _LL2802; _LL2808: Cyc_Tcutil_terr( loc,({ unsigned char* _temp2821=(
unsigned char*)"injected vararg [x]tunion should be in a region, not heap-allocated";
struct _tagged_string _temp2822; _temp2822.curr= _temp2821; _temp2822.base=
_temp2821; _temp2822.last_plus_one= _temp2821 + 68; _temp2822;})); goto _LL2802;
_LL2810: goto _LL2802; _LL2802:;} goto _LL2786; _LL2790: Cyc_Tcutil_terr( loc,({
unsigned char* _temp2823=( unsigned char*)"can't inject a non-[x]tunion type";
struct _tagged_string _temp2824; _temp2824.curr= _temp2823; _temp2824.base=
_temp2823; _temp2824.last_plus_one= _temp2823 + 34; _temp2824;})); goto _LL2786;
_LL2786:;}}} if(* _temp2564 != 0){ _temp2745= Cyc_Tcutil_i_check_valid_type( loc,
te, _temp2745,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_Core_Opt*)
_check_null(* _temp2564))->v);} else{ struct Cyc_Absyn_Tvar* e= Cyc_Tcutil_new_tvar((
void*) Cyc_Absyn_EffKind); _temp2745.free_vars= Cyc_Tcutil_add_free_tvar( loc,
_temp2745.free_vars, e);{ struct Cyc_List_List* effect= 0;{ struct Cyc_List_List*
tvs= _temp2745.free_vars; for( 0; tvs != 0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){ void* _temp2825=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind))->v; void* _temp2833; void*
_temp2835; _LL2827: if(( unsigned int) _temp2825 > 1u?*(( int*) _temp2825) ==
Cyc_Absyn_Eq_constr: 0){ _LL2834: _temp2833=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2825)->f1; if( _temp2833 ==( void*) Cyc_Absyn_RgnKind){ goto _LL2828;}
else{ goto _LL2829;}} else{ goto _LL2829;} _LL2829: if(( unsigned int) _temp2825
> 1u?*(( int*) _temp2825) == Cyc_Absyn_Eq_constr: 0){ _LL2836: _temp2835=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2825)->f1; if( _temp2835 ==( void*) Cyc_Absyn_EffKind){
goto _LL2830;} else{ goto _LL2831;}} else{ goto _LL2831;} _LL2831: goto _LL2832;
_LL2828: effect=({ struct Cyc_List_List* _temp2837=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2837->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2838=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2838[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2839; _temp2839.tag= Cyc_Absyn_AccessEff;
_temp2839.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp2840=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2840[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2841; _temp2841.tag= Cyc_Absyn_VarType;
_temp2841.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp2841;}); _temp2840;})); _temp2839;}); _temp2838;})); _temp2837->tl= effect;
_temp2837;}); goto _LL2826; _LL2830: effect=({ struct Cyc_List_List* _temp2842=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2842->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp2843=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2843[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2844; _temp2844.tag= Cyc_Absyn_VarType; _temp2844.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp2844;}); _temp2843;})); _temp2842->tl= effect; _temp2842;}); goto _LL2826;
_LL2832: goto _LL2826; _LL2826:;}}{ struct Cyc_List_List* ts= _temp2745.free_evars;
for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){ void*
_temp2845= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);
_LL2847: if( _temp2845 ==( void*) Cyc_Absyn_RgnKind){ goto _LL2848;} else{ goto
_LL2849;} _LL2849: if( _temp2845 ==( void*) Cyc_Absyn_EffKind){ goto _LL2850;}
else{ goto _LL2851;} _LL2851: goto _LL2852; _LL2848: effect=({ struct Cyc_List_List*
_temp2853=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2853->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2854=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2854[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2855; _temp2855.tag= Cyc_Absyn_AccessEff;
_temp2855.f1=( void*)(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);
_temp2855;}); _temp2854;})); _temp2853->tl= effect; _temp2853;}); goto _LL2846;
_LL2850: effect=({ struct Cyc_List_List* _temp2856=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2856->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( ts))->hd); _temp2856->tl= effect; _temp2856;});
goto _LL2846; _LL2852: goto _LL2846; _LL2846:;}}* _temp2564=({ struct Cyc_Core_Opt*
_temp2857=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2857->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2858=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2858[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2859; _temp2859.tag= Cyc_Absyn_JoinEff;
_temp2859.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
effect); _temp2859;}); _temp2858;})); _temp2857;});}} if(* _temp2567 != 0){
struct Cyc_List_List* bs=* _temp2567; for( 0; bs != 0; bs=(( struct Cyc_List_List*)
_check_null( bs))->tl){ struct Cyc_Absyn_Conref* _temp2860=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( bs))->hd)->kind); goto _LL2861; _LL2861: {
void* _temp2862=( void*) _temp2860->v; void* _temp2870; _LL2864: if( _temp2862
==( void*) Cyc_Absyn_No_constr){ goto _LL2865;} else{ goto _LL2866;} _LL2866:
if(( unsigned int) _temp2862 > 1u?*(( int*) _temp2862) == Cyc_Absyn_Eq_constr: 0){
_LL2871: _temp2870=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2862)->f1;
if( _temp2870 ==( void*) Cyc_Absyn_MemKind){ goto _LL2867;} else{ goto _LL2868;}}
else{ goto _LL2868;} _LL2868: goto _LL2869; _LL2865: Cyc_Tcutil_warn( loc,(
struct _tagged_string)({ struct _tagged_string _temp2872=*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( bs))->hd)->name; xprintf("Type variable %.*s unconstrained, assuming boxed",
_temp2872.last_plus_one - _temp2872.curr, _temp2872.curr);})); goto _LL2867;
_LL2867:( void*)( _temp2860->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2873=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2873[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2874; _temp2874.tag= Cyc_Absyn_Eq_constr;
_temp2874.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp2874;}); _temp2873;})));
goto _LL2863; _LL2869: goto _LL2863; _LL2863:;}}} cvtenv.kind_env= Cyc_Tcutil_remove_bound_tvars(
_temp2745.kind_env,* _temp2567); _temp2745.free_vars= Cyc_Tcutil_remove_bound_tvars(
_temp2745.free_vars,* _temp2567);{ struct Cyc_List_List* tvs= _temp2745.free_vars;
for( 0; tvs != 0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd);}}{ struct Cyc_List_List* evs=
_temp2745.free_evars; for( 0; evs != 0; evs=(( struct Cyc_List_List*)
_check_null( evs))->tl){ cvtenv.free_evars= Cyc_Tcutil_add_free_evar( cvtenv.free_evars,(
void*)(( struct Cyc_List_List*) _check_null( evs))->hd);}} goto _LL2451;}}
_LL2477: for( 0; _temp2568 != 0; _temp2568=(( struct Cyc_List_List*) _check_null(
_temp2568))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*)
Cyc_Absyn_MemKind,(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp2568))->hd)).f2);} goto _LL2451; _LL2479:{ struct _RegionHandle _temp2875=
_new_region(); struct _RegionHandle* sprev_rgn=& _temp2875; _push_region(
sprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for( 0; _temp2570 != 0;
_temp2570=(( struct Cyc_List_List*) _check_null( _temp2570))->tl){ struct Cyc_Absyn_Structfield
_temp2878; struct Cyc_List_List* _temp2879; struct Cyc_Core_Opt* _temp2881; void*
_temp2883; struct Cyc_Absyn_Tqual _temp2885; struct _tagged_string* _temp2887;
struct Cyc_Absyn_Structfield* _temp2876=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2570))->hd; _temp2878=* _temp2876;
_LL2888: _temp2887= _temp2878.name; goto _LL2886; _LL2886: _temp2885= _temp2878.tq;
goto _LL2884; _LL2884: _temp2883=( void*) _temp2878.type; goto _LL2882; _LL2882:
_temp2881= _temp2878.width; goto _LL2880; _LL2880: _temp2879= _temp2878.attributes;
goto _LL2877; _LL2877: if((( int(*)( int(* compare)( struct _tagged_string*,
struct _tagged_string*), struct Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields, _temp2887)){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp2889=* _temp2887; xprintf("duplicate field %.*s in struct",
_temp2889.last_plus_one - _temp2889.curr, _temp2889.curr);}));} if( Cyc_String_strcmp(*
_temp2887,({ unsigned char* _temp2890=( unsigned char*)""; struct _tagged_string
_temp2891; _temp2891.curr= _temp2890; _temp2891.base= _temp2890; _temp2891.last_plus_one=
_temp2890 + 1; _temp2891;})) != 0){ prev_fields=({ struct Cyc_List_List*
_temp2892=( struct Cyc_List_List*) _region_malloc( sprev_rgn, sizeof( struct Cyc_List_List));
_temp2892->hd=( void*) _temp2887; _temp2892->tl= prev_fields; _temp2892;});}
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,
_temp2883); if( _temp2881 != 0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp2881))->v)){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string _temp2893=*
_temp2887; xprintf("bitfield %.*s does not have constant width", _temp2893.last_plus_one
- _temp2893.curr, _temp2893.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp2881))->v);}{
void* _temp2894= Cyc_Tcutil_compress( _temp2883); void* _temp2900; void*
_temp2902; _LL2896: if(( unsigned int) _temp2894 > 4u?*(( int*) _temp2894) ==
Cyc_Absyn_IntType: 0){ _LL2903: _temp2902=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2894)->f1; goto _LL2901; _LL2901: _temp2900=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2894)->f2; goto _LL2897;} else{ goto _LL2898;} _LL2898: goto _LL2899;
_LL2897:{ void* _temp2904= _temp2900; _LL2906: if( _temp2904 ==( void*) Cyc_Absyn_B1){
goto _LL2907;} else{ goto _LL2908;} _LL2908: if( _temp2904 ==( void*) Cyc_Absyn_B2){
goto _LL2909;} else{ goto _LL2910;} _LL2910: if( _temp2904 ==( void*) Cyc_Absyn_B4){
goto _LL2911;} else{ goto _LL2912;} _LL2912: if( _temp2904 ==( void*) Cyc_Absyn_B8){
goto _LL2913;} else{ goto _LL2905;} _LL2907: if( w > 8){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp2914=( unsigned char*)"bitfield larger than type"; struct
_tagged_string _temp2915; _temp2915.curr= _temp2914; _temp2915.base= _temp2914;
_temp2915.last_plus_one= _temp2914 + 26; _temp2915;}));} goto _LL2905; _LL2909:
if( w > 16){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp2916=( unsigned char*)"bitfield larger than type";
struct _tagged_string _temp2917; _temp2917.curr= _temp2916; _temp2917.base=
_temp2916; _temp2917.last_plus_one= _temp2916 + 26; _temp2917;}));} goto _LL2905;
_LL2911: if( w > 32){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp2918=(
unsigned char*)"bitfield larger than type"; struct _tagged_string _temp2919;
_temp2919.curr= _temp2918; _temp2919.base= _temp2918; _temp2919.last_plus_one=
_temp2918 + 26; _temp2919;}));} goto _LL2905; _LL2913: if( w > 64){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp2920=( unsigned char*)"bitfield larger than type";
struct _tagged_string _temp2921; _temp2921.curr= _temp2920; _temp2921.base=
_temp2920; _temp2921.last_plus_one= _temp2920 + 26; _temp2921;}));} goto _LL2905;
_LL2905:;} goto _LL2895; _LL2899: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
struct _tagged_string _temp2922=* _temp2887; struct _tagged_string _temp2923=
Cyc_Absynpp_typ2string( _temp2883); xprintf("bitfield %.*s must have integral type but has type %.*s",
_temp2922.last_plus_one - _temp2922.curr, _temp2922.curr, _temp2923.last_plus_one
- _temp2923.curr, _temp2923.curr);})); goto _LL2895; _LL2895:;}} for( 0;
_temp2879 != 0; _temp2879=(( struct Cyc_List_List*) _check_null( _temp2879))->tl){
void* _temp2924=( void*)(( struct Cyc_List_List*) _check_null( _temp2879))->hd;
int _temp2932; _LL2926: if( _temp2924 ==( void*) Cyc_Absyn_Packed_att){ goto
_LL2927;} else{ goto _LL2928;} _LL2928: if(( unsigned int) _temp2924 > 16u?*((
int*) _temp2924) == Cyc_Absyn_Aligned_att: 0){ _LL2933: _temp2932=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp2924)->f1; goto _LL2929;} else{ goto _LL2930;} _LL2930: goto _LL2931;
_LL2927: continue; _LL2929: continue; _LL2931: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp2934= Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp2879))->hd); struct
_tagged_string _temp2935=* _temp2887; xprintf("bad attribute %.*s on struct member %.*s",
_temp2934.last_plus_one - _temp2934.curr, _temp2934.curr, _temp2935.last_plus_one
- _temp2935.curr, _temp2935.curr);})); goto _LL2925; _LL2925:;}}}; _pop_region(&
_temp2875);} goto _LL2451; _LL2481:{ struct _RegionHandle _temp2936= _new_region();
struct _RegionHandle* uprev_rgn=& _temp2936; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0; for( 0; _temp2572 != 0; _temp2572=(( struct Cyc_List_List*)
_check_null( _temp2572))->tl){ struct Cyc_Absyn_Structfield _temp2939; struct
Cyc_List_List* _temp2940; struct Cyc_Core_Opt* _temp2942; void* _temp2944;
struct Cyc_Absyn_Tqual _temp2946; struct _tagged_string* _temp2948; struct Cyc_Absyn_Structfield*
_temp2937=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2572))->hd; _temp2939=* _temp2937; _LL2949: _temp2948= _temp2939.name; goto
_LL2947; _LL2947: _temp2946= _temp2939.tq; goto _LL2945; _LL2945: _temp2944=(
void*) _temp2939.type; goto _LL2943; _LL2943: _temp2942= _temp2939.width; goto
_LL2941; _LL2941: _temp2940= _temp2939.attributes; goto _LL2938; _LL2938: if(((
int(*)( int(* compare)( struct _tagged_string*, struct _tagged_string*), struct
Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp,
prev_fields, _temp2948)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp2950=* _temp2948; xprintf("duplicate field %.*s in struct",
_temp2950.last_plus_one - _temp2950.curr, _temp2950.curr);}));} if( Cyc_String_strcmp(*
_temp2948,({ unsigned char* _temp2951=( unsigned char*)""; struct _tagged_string
_temp2952; _temp2952.curr= _temp2951; _temp2952.base= _temp2951; _temp2952.last_plus_one=
_temp2951 + 1; _temp2952;})) != 0){ prev_fields=({ struct Cyc_List_List*
_temp2953=( struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp2953->hd=( void*) _temp2948; _temp2953->tl= prev_fields; _temp2953;});}
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,
_temp2944); if( ! Cyc_Tcutil_bits_only( _temp2944)){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp2954=* _temp2948; xprintf("%.*s has a type that is possibly incompatible with other members of union",
_temp2954.last_plus_one - _temp2954.curr, _temp2954.curr);}));} if( _temp2942 !=
0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp2942))->v)){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp2955=* _temp2948; xprintf("bitfield %.*s does not have constant width",
_temp2955.last_plus_one - _temp2955.curr, _temp2955.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp2942))->v);}{
void* _temp2956= Cyc_Tcutil_compress( _temp2944); void* _temp2962; void*
_temp2964; _LL2958: if(( unsigned int) _temp2956 > 4u?*(( int*) _temp2956) ==
Cyc_Absyn_IntType: 0){ _LL2965: _temp2964=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2956)->f1; goto _LL2963; _LL2963: _temp2962=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2956)->f2; goto _LL2959;} else{ goto _LL2960;} _LL2960: goto _LL2961;
_LL2959:{ void* _temp2966= _temp2962; _LL2968: if( _temp2966 ==( void*) Cyc_Absyn_B1){
goto _LL2969;} else{ goto _LL2970;} _LL2970: if( _temp2966 ==( void*) Cyc_Absyn_B2){
goto _LL2971;} else{ goto _LL2972;} _LL2972: if( _temp2966 ==( void*) Cyc_Absyn_B4){
goto _LL2973;} else{ goto _LL2974;} _LL2974: if( _temp2966 ==( void*) Cyc_Absyn_B8){
goto _LL2975;} else{ goto _LL2967;} _LL2969: if( w > 8){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp2976=( unsigned char*)"bitfield larger than type"; struct
_tagged_string _temp2977; _temp2977.curr= _temp2976; _temp2977.base= _temp2976;
_temp2977.last_plus_one= _temp2976 + 26; _temp2977;}));} goto _LL2967; _LL2971:
if( w > 16){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp2978=( unsigned char*)"bitfield larger than type";
struct _tagged_string _temp2979; _temp2979.curr= _temp2978; _temp2979.base=
_temp2978; _temp2979.last_plus_one= _temp2978 + 26; _temp2979;}));} goto _LL2967;
_LL2973: if( w > 32){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp2980=(
unsigned char*)"bitfield larger than type"; struct _tagged_string _temp2981;
_temp2981.curr= _temp2980; _temp2981.base= _temp2980; _temp2981.last_plus_one=
_temp2980 + 26; _temp2981;}));} goto _LL2967; _LL2975: if( w > 64){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp2982=( unsigned char*)"bitfield larger than type";
struct _tagged_string _temp2983; _temp2983.curr= _temp2982; _temp2983.base=
_temp2982; _temp2983.last_plus_one= _temp2982 + 26; _temp2983;}));} goto _LL2967;
_LL2967:;} goto _LL2957; _LL2961: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
struct _tagged_string _temp2984=* _temp2948; struct _tagged_string _temp2985=
Cyc_Absynpp_typ2string( _temp2944); xprintf("bitfield %.*s must have integral type but has type %.*s",
_temp2984.last_plus_one - _temp2984.curr, _temp2984.curr, _temp2985.last_plus_one
- _temp2985.curr, _temp2985.curr);})); goto _LL2957; _LL2957:;}} for( 0;
_temp2940 != 0; _temp2940=(( struct Cyc_List_List*) _check_null( _temp2940))->tl){
void* _temp2986=( void*)(( struct Cyc_List_List*) _check_null( _temp2940))->hd;
int _temp2994; _LL2988: if( _temp2986 ==( void*) Cyc_Absyn_Packed_att){ goto
_LL2989;} else{ goto _LL2990;} _LL2990: if(( unsigned int) _temp2986 > 16u?*((
int*) _temp2986) == Cyc_Absyn_Aligned_att: 0){ _LL2995: _temp2994=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp2986)->f1; goto _LL2991;} else{ goto _LL2992;} _LL2992: goto _LL2993;
_LL2989: continue; _LL2991: continue; _LL2993: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp2996= Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp2940))->hd); struct
_tagged_string _temp2997=* _temp2948; xprintf("bad attribute %.*s on struct member %.*s",
_temp2996.last_plus_one - _temp2996.curr, _temp2996.curr, _temp2997.last_plus_one
- _temp2997.curr, _temp2997.curr);})); goto _LL2987; _LL2987:;}}}; _pop_region(&
_temp2936);} goto _LL2451; _LL2483: if( _temp2579 == 0){ if(* _temp2576 == 0){
Cyc_Tcutil_terr( loc,( struct _tagged_string) xprintf("bad anonymous struct"));
return cvtenv;}{ struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**)
_check_null(* _temp2576));;}} if(* _temp2576 == 0){ struct _handler_cons
_temp2998; _push_handler(& _temp2998);{ int _temp3000= 0; if( setjmp( _temp2998.handler)){
_temp3000= 1;} if( ! _temp3000){* _temp2576=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc,( struct _tuple1*) _check_null( _temp2579));; _pop_handler();} else{
void* _temp2999=( void*) _exn_thrown; void* _temp3002= _temp2999; _LL3004: if(
_temp3002 == Cyc_Dict_Absent){ goto _LL3005;} else{ goto _LL3006;} _LL3006: goto
_LL3007; _LL3005: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp3008= Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null(
_temp2579)); xprintf("unbound type struct %.*s", _temp3008.last_plus_one -
_temp3008.curr, _temp3008.curr);})); return cvtenv; _LL3007:( void) _throw(
_temp3002); _LL3003:;}}}{ struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**)
_check_null(* _temp2576)); if( sd->name != 0){*(( struct _tuple1*) _check_null(
_temp2579))=(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)[
_check_known_subscript_notnull( 1u, 0)];}{ struct Cyc_List_List* tvs= sd->tvs;
int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2577); if( lvs != largs){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string _temp3009=
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp2579)); int
_temp3010= lvs; int _temp3011= largs; xprintf("struct %.*s expects %d type arguments but was given %d",
_temp3009.last_plus_one - _temp3009.curr, _temp3009.curr, _temp3010, _temp3011);}));}
for( 0; _temp2577 != 0;( _temp2577=(( struct Cyc_List_List*) _check_null(
_temp2577))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2577))->hd; void* k1=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} goto _LL2451;}} _LL2485: if( _temp2586 == 0){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp3012=( unsigned char*)"missing union name"; struct
_tagged_string _temp3013; _temp3013.curr= _temp3012; _temp3013.base= _temp3012;
_temp3013.last_plus_one= _temp3012 + 19; _temp3013;})); return cvtenv;} if(*
_temp2583 == 0){ struct _handler_cons _temp3014; _push_handler(& _temp3014);{
int _temp3016= 0; if( setjmp( _temp3014.handler)){ _temp3016= 1;} if( !
_temp3016){* _temp2583=( struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl(
te, loc,( struct _tuple1*) _check_null( _temp2586));; _pop_handler();} else{
void* _temp3015=( void*) _exn_thrown; void* _temp3018= _temp3015; _LL3020: if(
_temp3018 == Cyc_Dict_Absent){ goto _LL3021;} else{ goto _LL3022;} _LL3022: goto
_LL3023; _LL3021: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp3024= Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null(
_temp2586)); xprintf("unbound type union %.*s", _temp3024.last_plus_one -
_temp3024.curr, _temp3024.curr);})); return cvtenv; _LL3023:( void) _throw(
_temp3018); _LL3019:;}}}{ struct Cyc_Absyn_Uniondecl* ud=*(( struct Cyc_Absyn_Uniondecl**)
_check_null(* _temp2583)); if( ud->name != 0){*(( struct _tuple1*) _check_null(
_temp2586))=(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( ud->name))->v)[
_check_known_subscript_notnull( 1u, 0)];}{ struct Cyc_List_List* tvs= ud->tvs;
int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2584); if( lvs != largs){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string _temp3025=
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp2586)); int
_temp3026= lvs; int _temp3027= largs; xprintf("union %.*s expects %d type arguments but was given %d",
_temp3025.last_plus_one - _temp3025.curr, _temp3025.curr, _temp3026, _temp3027);}));}
for( 0; _temp2584 != 0;( _temp2584=(( struct Cyc_List_List*) _check_null(
_temp2584))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2584))->hd; void* k1=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} goto _LL2451;}} _LL2487: { struct Cyc_Absyn_Typedefdecl*
td;{ struct _handler_cons _temp3028; _push_handler(& _temp3028);{ int _temp3030=
0; if( setjmp( _temp3028.handler)){ _temp3030= 1;} if( ! _temp3030){ td= Cyc_Tcenv_lookup_typedefdecl(
te, loc, _temp2593);; _pop_handler();} else{ void* _temp3029=( void*)
_exn_thrown; void* _temp3032= _temp3029; _LL3034: if( _temp3032 == Cyc_Dict_Absent){
goto _LL3035;} else{ goto _LL3036;} _LL3036: goto _LL3037; _LL3035: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ struct _tagged_string _temp3038= Cyc_Absynpp_qvar2string(
_temp2593); xprintf("unbound typedef name %.*s", _temp3038.last_plus_one -
_temp3038.curr, _temp3038.curr);})); return cvtenv; _LL3037:( void) _throw(
_temp3032); _LL3033:;}}} _temp2593[ _check_known_subscript_notnull( 1u, 0)]=( td->name)[
_check_known_subscript_notnull( 1u, 0)];{ struct Cyc_List_List* tvs= td->tvs;
struct Cyc_List_List* ts= _temp2591; struct Cyc_List_List* inst= 0; for( 0; ts
!= 0? tvs != 0: 0;( ts=(( struct Cyc_List_List*) _check_null( ts))->tl, tvs=((
struct Cyc_List_List*) _check_null( tvs))->tl)){ void* k=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te,
cvtenv, k,( void*)(( struct Cyc_List_List*) _check_null( ts))->hd); inst=({
struct Cyc_List_List* _temp3039=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3039->hd=( void*)({ struct _tuple5* _temp3040=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp3040->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd; _temp3040->f2=( void*)(( struct
Cyc_List_List*) _check_null( ts))->hd; _temp3040;}); _temp3039->tl= inst;
_temp3039;});} if( tvs != 0? 1: ts != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp3041= Cyc_Absynpp_qvar2string(
_temp2593); xprintf("typedef %.*s expects a different number of arguments",
_temp3041.last_plus_one - _temp3041.curr, _temp3041.curr);})); return cvtenv;}{
void* new_typ= Cyc_Tcutil_substitute( inst,( void*) td->defn);* _temp2590=({
struct Cyc_Core_Opt* _temp3042=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3042->v=( void*) new_typ; _temp3042;}); goto _LL2451;}}}
_LL2489: goto _LL2451; _LL2491: _temp2597= _temp2595; goto _LL2493; _LL2493:
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2597); goto _LL2451; _LL2495: for( 0; _temp2599 != 0; _temp2599=(( struct
Cyc_List_List*) _check_null( _temp2599))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_List_List*)
_check_null( _temp2599))->hd);} goto _LL2451; _LL2451:;} if( ! Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind( t), expected_kind)){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp3043= Cyc_Absynpp_typ2string( t);
struct _tagged_string _temp3044= Cyc_Absynpp_kind2string( Cyc_Tcutil_typ_kind( t));
struct _tagged_string _temp3045= Cyc_Absynpp_kind2string( expected_kind);
xprintf("type %.*s has kind %.*s but as used here needs kind %.*s", _temp3043.last_plus_one
- _temp3043.curr, _temp3043.curr, _temp3044.last_plus_one - _temp3044.curr,
_temp3044.curr, _temp3045.last_plus_one - _temp3045.curr, _temp3045.curr);}));}
return cvtenv;} struct Cyc_List_List* Cyc_Tcutil_check_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* kind_env, void*
expected_kind, void* t){ struct Cyc_Tcutil_CVTEnv _temp3047= Cyc_Tcutil_i_check_valid_type(
loc, te,({ struct Cyc_Tcutil_CVTEnv _temp3046; _temp3046.kind_env= kind_env;
_temp3046.free_vars= 0; _temp3046.free_evars= 0; _temp3046;}), expected_kind, t);
goto _LL3048; _LL3048:{ struct Cyc_List_List* vs= _temp3047.free_vars; for( 0;
vs != 0; vs=(( struct Cyc_List_List*) _check_null( vs))->tl){ Cyc_Tcutil_fast_add_free_tvar(
kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( vs))->hd);}}{
struct Cyc_List_List* evs= _temp3047.free_evars; for( 0; evs != 0; evs=(( struct
Cyc_List_List*) _check_null( evs))->tl){ void* _temp3049= Cyc_Tcutil_compress((
void*)(( struct Cyc_List_List*) _check_null( evs))->hd); struct Cyc_Core_Opt*
_temp3055; struct Cyc_Core_Opt** _temp3057; int _temp3058; struct Cyc_Core_Opt*
_temp3060; struct Cyc_Core_Opt* _temp3062; _LL3051: if(( unsigned int) _temp3049
> 4u?*(( int*) _temp3049) == Cyc_Absyn_Evar: 0){ _LL3063: _temp3062=(( struct
Cyc_Absyn_Evar_struct*) _temp3049)->f1; goto _LL3061; _LL3061: _temp3060=((
struct Cyc_Absyn_Evar_struct*) _temp3049)->f2; goto _LL3059; _LL3059: _temp3058=((
struct Cyc_Absyn_Evar_struct*) _temp3049)->f3; goto _LL3056; _LL3056: _temp3055=((
struct Cyc_Absyn_Evar_struct*) _temp3049)->f4; _temp3057=&(( struct Cyc_Absyn_Evar_struct*)
_temp3049)->f4; goto _LL3052;} else{ goto _LL3053;} _LL3053: goto _LL3054;
_LL3052: if(* _temp3057 == 0){* _temp3057=({ struct Cyc_Core_Opt* _temp3064=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3064->v=(
void*) kind_env; _temp3064;});} else{ struct Cyc_List_List* _temp3065=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(* _temp3057))->v; goto
_LL3066; _LL3066: { struct Cyc_List_List* _temp3067= 0; goto _LL3068; _LL3068:
for( 0; _temp3065 != 0; _temp3065=(( struct Cyc_List_List*) _check_null(
_temp3065))->tl){ if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct
Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp3065))->hd)){ _temp3067=({ struct Cyc_List_List* _temp3069=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3069->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp3065))->hd);
_temp3069->tl= _temp3067; _temp3069;});}}* _temp3057=({ struct Cyc_Core_Opt*
_temp3070=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3070->v=( void*) _temp3067; _temp3070;});}} goto _LL3050; _LL3054: goto
_LL3050; _LL3050:;}} return _temp3047.free_vars;} void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, void* t){ struct
Cyc_List_List* _temp3071= Cyc_Tcutil_check_valid_type( loc, te, 0,( void*) Cyc_Absyn_MemKind,
t); goto _LL3072; _LL3072:{ struct Cyc_List_List* x= _temp3071; for( 0; x != 0;
x=(( struct Cyc_List_List*) _check_null( x))->tl){ struct Cyc_Absyn_Conref* c=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( x))->hd)->kind);
void* _temp3073=( void*) c->v; void* _temp3081; _LL3075: if( _temp3073 ==( void*)
Cyc_Absyn_No_constr){ goto _LL3076;} else{ goto _LL3077;} _LL3077: if((
unsigned int) _temp3073 > 1u?*(( int*) _temp3073) == Cyc_Absyn_Eq_constr: 0){
_LL3082: _temp3081=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3073)->f1;
if( _temp3081 ==( void*) Cyc_Absyn_MemKind){ goto _LL3078;} else{ goto _LL3079;}}
else{ goto _LL3079;} _LL3079: goto _LL3080; _LL3076: goto _LL3078; _LL3078:(
void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3083=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3083[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3084; _temp3084.tag= Cyc_Absyn_Eq_constr;
_temp3084.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3084;}); _temp3083;})));
goto _LL3074; _LL3080: goto _LL3074; _LL3074:;}} if( _temp3071 != 0){ void*
_temp3085= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp3091; struct
Cyc_List_List* _temp3093; struct Cyc_Absyn_VarargInfo* _temp3095; int _temp3097;
struct Cyc_List_List* _temp3099; void* _temp3101; struct Cyc_Core_Opt* _temp3103;
struct Cyc_List_List* _temp3105; struct Cyc_List_List** _temp3107; _LL3087: if((
unsigned int) _temp3085 > 4u?*(( int*) _temp3085) == Cyc_Absyn_FnType: 0){
_LL3092: _temp3091=(( struct Cyc_Absyn_FnType_struct*) _temp3085)->f1; _LL3106:
_temp3105= _temp3091.tvars; _temp3107=&((( struct Cyc_Absyn_FnType_struct*)
_temp3085)->f1).tvars; goto _LL3104; _LL3104: _temp3103= _temp3091.effect; goto
_LL3102; _LL3102: _temp3101=( void*) _temp3091.ret_typ; goto _LL3100; _LL3100:
_temp3099= _temp3091.args; goto _LL3098; _LL3098: _temp3097= _temp3091.c_varargs;
goto _LL3096; _LL3096: _temp3095= _temp3091.cyc_varargs; goto _LL3094; _LL3094:
_temp3093= _temp3091.attributes; goto _LL3088;} else{ goto _LL3089;} _LL3089:
goto _LL3090; _LL3088: if(* _temp3107 == 0){* _temp3107= _temp3071;} else{ if(
_temp3071 != 0){ struct _tagged_string _temp3108=*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp3071))->hd)->name; goto _LL3109;
_LL3109: if(*(( const unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( _temp3108, sizeof( unsigned char), 0)
=='?'){ Cyc_Tcutil_terr( loc,( struct _tagged_string) xprintf("You failed to abstract over the generated type variables. \nRemove the bound type variables or else make the effects of the type explicit\nand then abstract the free type variables properly."));}
else{ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string
_temp3110= _temp3108; xprintf("unbound type variable %.*s", _temp3110.last_plus_one
- _temp3110.curr, _temp3110.curr);}));}}} goto _LL3086; _LL3090: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ struct _tagged_string _temp3111=*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp3071))->hd)->name; xprintf("unbound type variable %.*s",
_temp3111.last_plus_one - _temp3111.curr, _temp3111.curr);})); return; _LL3086:;}}
void Cyc_Tcutil_add_tvar_identity( struct Cyc_Absyn_Tvar* tv){ if( tv->identity
== 0){ tv->identity= Cyc_Tcutil_new_tvar_id();}} void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List* tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Tcutil_add_tvar_identity, tvs);}
void Cyc_Tcutil_check_type( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* bound_tvars, void* expected_kind, void* t){ struct Cyc_List_List*
_temp3112= Cyc_Tcutil_check_valid_type( loc, te, bound_tvars, expected_kind, t);
goto _LL3113; _LL3113: { struct Cyc_List_List* _temp3114= Cyc_Tcutil_remove_bound_tvars(
_temp3112, bound_tvars); goto _LL3115; _LL3115: { struct Cyc_List_List* fs=
_temp3114; for( 0; fs != 0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){
struct _tagged_string* _temp3116=(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->name; goto _LL3117; _LL3117: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp3118=* _temp3116; struct
_tagged_string _temp3119= Cyc_Absynpp_typ2string( t); xprintf("unbound type variable %.*s in type %.*s",
_temp3118.last_plus_one - _temp3118.curr, _temp3118.curr, _temp3119.last_plus_one
- _temp3119.curr, _temp3119.curr);}));}}}} static void Cyc_Tcutil_check_unique_unsorted(
int(* cmp)( void*, void*), struct Cyc_List_List* vs, struct Cyc_Position_Segment*
loc, struct _tagged_string(* a2string)( void*), struct _tagged_string msg){ for(
0; vs != 0; vs=(( struct Cyc_List_List*) _check_null( vs))->tl){ struct Cyc_List_List*
vs2=(( struct Cyc_List_List*) _check_null( vs))->tl; for( 0; vs2 != 0; vs2=((
struct Cyc_List_List*) _check_null( vs2))->tl){ if( cmp(( void*)(( struct Cyc_List_List*)
_check_null( vs))->hd,( void*)(( struct Cyc_List_List*) _check_null( vs2))->hd)
== 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string
_temp3120= msg; struct _tagged_string _temp3121= a2string(( void*)(( struct Cyc_List_List*)
_check_null( vs))->hd); xprintf("%.*s: %.*s", _temp3120.last_plus_one -
_temp3120.curr, _temp3120.curr, _temp3121.last_plus_one - _temp3121.curr,
_temp3121.curr);}));}}}} static struct _tagged_string Cyc_Tcutil_strptr2string(
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
tvs, loc, Cyc_Tcutil_tvar2string,({ unsigned char* _temp3122=( unsigned char*)"duplicate type variable";
struct _tagged_string _temp3123; _temp3123.curr= _temp3122; _temp3123.base=
_temp3122; _temp3123.last_plus_one= _temp3122 + 24; _temp3123;}));} struct
_tuple17{ struct Cyc_Absyn_Structfield* f1; int f2; } ; struct _tuple18{ struct
Cyc_List_List* f1; void* f2; } ; struct _tuple19{ struct Cyc_Absyn_Structfield*
f1; void* f2; } ; struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* sdfields){ struct Cyc_List_List* fields= 0;{ struct
Cyc_List_List* sd_fields= sdfields; for( 0; sd_fields != 0; sd_fields=(( struct
Cyc_List_List*) _check_null( sd_fields))->tl){ if( Cyc_String_strcmp(*(( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( sd_fields))->hd)->name,({
unsigned char* _temp3124=( unsigned char*)""; struct _tagged_string _temp3125;
_temp3125.curr= _temp3124; _temp3125.base= _temp3124; _temp3125.last_plus_one=
_temp3124 + 1; _temp3125;})) != 0){ fields=({ struct Cyc_List_List* _temp3126=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3126->hd=(
void*)({ struct _tuple17* _temp3127=( struct _tuple17*) GC_malloc( sizeof(
struct _tuple17)); _temp3127->f1=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( sd_fields))->hd; _temp3127->f2= 0; _temp3127;}); _temp3126->tl=
fields; _temp3126;});}}} fields=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( fields);{ struct Cyc_List_List* ans= 0; for( 0; des != 0;
des=(( struct Cyc_List_List*) _check_null( des))->tl){ struct _tuple18 _temp3130;
void* _temp3131; struct Cyc_List_List* _temp3133; struct _tuple18* _temp3128=(
struct _tuple18*)(( struct Cyc_List_List*) _check_null( des))->hd; _temp3130=*
_temp3128; _LL3134: _temp3133= _temp3130.f1; goto _LL3132; _LL3132: _temp3131=
_temp3130.f2; goto _LL3129; _LL3129: if( _temp3133 == 0){ struct Cyc_List_List*
_temp3135= fields; goto _LL3136; _LL3136: for( 0; _temp3135 != 0; _temp3135=((
struct Cyc_List_List*) _check_null( _temp3135))->tl){ if( !(*(( struct _tuple17*)((
struct Cyc_List_List*) _check_null( _temp3135))->hd)).f2){(*(( struct _tuple17*)((
struct Cyc_List_List*) _check_null( _temp3135))->hd)).f2= 1;(*(( struct _tuple18*)((
struct Cyc_List_List*) _check_null( des))->hd)).f1=({ struct Cyc_List_List*
_temp3137=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3137->hd=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp3138=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp3138[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp3139; _temp3139.tag= Cyc_Absyn_FieldName;
_temp3139.f1=((*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3135))->hd)).f1)->name; _temp3139;}); _temp3138;})); _temp3137->tl= 0;
_temp3137;}); ans=({ struct Cyc_List_List* _temp3140=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp3140->hd=( void*)({
struct _tuple19* _temp3141=( struct _tuple19*) _region_malloc( rgn, sizeof(
struct _tuple19)); _temp3141->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3135))->hd)).f1; _temp3141->f2= _temp3131; _temp3141;});
_temp3140->tl= ans; _temp3140;}); break;}} if( _temp3135 == 0){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp3142=( unsigned char*)"too many arguments to struct";
struct _tagged_string _temp3143; _temp3143.curr= _temp3142; _temp3143.base=
_temp3142; _temp3143.last_plus_one= _temp3142 + 29; _temp3143;}));}} else{ if(((
struct Cyc_List_List*) _check_null( _temp3133))->tl != 0){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp3144=( unsigned char*)"multiple designators are not supported";
struct _tagged_string _temp3145; _temp3145.curr= _temp3144; _temp3145.base=
_temp3144; _temp3145.last_plus_one= _temp3144 + 39; _temp3145;}));} else{ void*
_temp3146=( void*)(( struct Cyc_List_List*) _check_null( _temp3133))->hd; struct
Cyc_Absyn_Exp* _temp3152; struct _tagged_string* _temp3154; _LL3148: if(*(( int*)
_temp3146) == Cyc_Absyn_ArrayElement){ _LL3153: _temp3152=(( struct Cyc_Absyn_ArrayElement_struct*)
_temp3146)->f1; goto _LL3149;} else{ goto _LL3150;} _LL3150: if(*(( int*)
_temp3146) == Cyc_Absyn_FieldName){ _LL3155: _temp3154=(( struct Cyc_Absyn_FieldName_struct*)
_temp3146)->f1; goto _LL3151;} else{ goto _LL3147;} _LL3149: Cyc_Tcutil_terr(
loc,({ unsigned char* _temp3156=( unsigned char*)"array designator used in argument to struct";
struct _tagged_string _temp3157; _temp3157.curr= _temp3156; _temp3157.base=
_temp3156; _temp3157.last_plus_one= _temp3156 + 44; _temp3157;})); goto _LL3147;
_LL3151: { struct Cyc_List_List* _temp3158= fields; goto _LL3159; _LL3159: for(
0; _temp3158 != 0; _temp3158=(( struct Cyc_List_List*) _check_null( _temp3158))->tl){
if( Cyc_String_zstrptrcmp( _temp3154,((*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3158))->hd)).f1)->name) == 0){ if((*(( struct _tuple17*)((
struct Cyc_List_List*) _check_null( _temp3158))->hd)).f2){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp3160=* _temp3154; xprintf("field %.*s has already been used as an argument",
_temp3160.last_plus_one - _temp3160.curr, _temp3160.curr);}));}(*(( struct
_tuple17*)(( struct Cyc_List_List*) _check_null( _temp3158))->hd)).f2= 1; ans=({
struct Cyc_List_List* _temp3161=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp3161->hd=( void*)({ struct _tuple19*
_temp3162=( struct _tuple19*) _region_malloc( rgn, sizeof( struct _tuple19));
_temp3162->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3158))->hd)).f1; _temp3162->f2= _temp3131; _temp3162;}); _temp3161->tl= ans;
_temp3161;}); break;}} if( _temp3158 == 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp3163=* _temp3154; xprintf("bad field designator %.*s",
_temp3163.last_plus_one - _temp3163.curr, _temp3163.curr);}));} goto _LL3147;}
_LL3147:;}}} for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null(
fields))->tl){ if( !(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
fields))->hd)).f2){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp3164=(
unsigned char*)"too few arguments to struct"; struct _tagged_string _temp3165;
_temp3165.curr= _temp3164; _temp3165.base= _temp3164; _temp3165.last_plus_one=
_temp3164 + 28; _temp3165;})); break;}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( ans);}} int Cyc_Tcutil_is_tagged_pointer_typ(
void* t){ void* _temp3166= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp3172; struct Cyc_Absyn_Conref* _temp3174; struct Cyc_Absyn_Tqual _temp3176;
struct Cyc_Absyn_Conref* _temp3178; void* _temp3180; void* _temp3182; _LL3168:
if(( unsigned int) _temp3166 > 4u?*(( int*) _temp3166) == Cyc_Absyn_PointerType:
0){ _LL3173: _temp3172=(( struct Cyc_Absyn_PointerType_struct*) _temp3166)->f1;
_LL3183: _temp3182=( void*) _temp3172.elt_typ; goto _LL3181; _LL3181: _temp3180=(
void*) _temp3172.rgn_typ; goto _LL3179; _LL3179: _temp3178= _temp3172.nullable;
goto _LL3177; _LL3177: _temp3176= _temp3172.tq; goto _LL3175; _LL3175: _temp3174=
_temp3172.bounds; goto _LL3169;} else{ goto _LL3170;} _LL3170: goto _LL3171;
_LL3169: { struct Cyc_Absyn_Conref* _temp3184=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3174); goto
_LL3185; _LL3185: { void* _temp3186=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3184))->v; void*
_temp3194; _LL3188: if(( unsigned int) _temp3186 > 1u?*(( int*) _temp3186) ==
Cyc_Absyn_Eq_constr: 0){ _LL3195: _temp3194=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3186)->f1; if( _temp3194 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3189;}
else{ goto _LL3190;}} else{ goto _LL3190;} _LL3190: if( _temp3186 ==( void*) Cyc_Absyn_No_constr){
goto _LL3191;} else{ goto _LL3192;} _LL3192: goto _LL3193; _LL3189: return 1;
_LL3191:( void*)( _temp3184->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3196=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3196[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3197; _temp3197.tag= Cyc_Absyn_Eq_constr;
_temp3197.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp3197;}); _temp3196;})));
return 1; _LL3193: return 0; _LL3187:;}} _LL3171: return 0; _LL3167:;} struct
_tuple7 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e){ struct _tuple7 bogus_ans=({ struct _tuple7 _temp3436; _temp3436.f1= 0;
_temp3436.f2=( void*) Cyc_Absyn_HeapRgn; _temp3436;}); void* _temp3198=( void*)
e->r; void* _temp3212; struct _tuple1* _temp3214; struct _tagged_string*
_temp3216; struct Cyc_Absyn_Exp* _temp3218; struct _tagged_string* _temp3220;
struct Cyc_Absyn_Exp* _temp3222; struct Cyc_Absyn_Exp* _temp3224; struct Cyc_Absyn_Exp*
_temp3226; struct Cyc_Absyn_Exp* _temp3228; _LL3200: if(*(( int*) _temp3198) ==
Cyc_Absyn_Var_e){ _LL3215: _temp3214=(( struct Cyc_Absyn_Var_e_struct*)
_temp3198)->f1; goto _LL3213; _LL3213: _temp3212=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3198)->f2; goto _LL3201;} else{ goto _LL3202;} _LL3202: if(*(( int*)
_temp3198) == Cyc_Absyn_StructMember_e){ _LL3219: _temp3218=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3198)->f1; goto _LL3217; _LL3217: _temp3216=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3198)->f2; goto _LL3203;} else{ goto _LL3204;} _LL3204: if(*(( int*)
_temp3198) == Cyc_Absyn_StructArrow_e){ _LL3223: _temp3222=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3198)->f1; goto _LL3221; _LL3221: _temp3220=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3198)->f2; goto _LL3205;} else{ goto _LL3206;} _LL3206: if(*(( int*)
_temp3198) == Cyc_Absyn_Deref_e){ _LL3225: _temp3224=(( struct Cyc_Absyn_Deref_e_struct*)
_temp3198)->f1; goto _LL3207;} else{ goto _LL3208;} _LL3208: if(*(( int*)
_temp3198) == Cyc_Absyn_Subscript_e){ _LL3229: _temp3228=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3198)->f1; goto _LL3227; _LL3227: _temp3226=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3198)->f2; goto _LL3209;} else{ goto _LL3210;} _LL3210: goto _LL3211;
_LL3201: { void* _temp3230= _temp3212; struct Cyc_Absyn_Fndecl* _temp3244;
struct Cyc_Absyn_Vardecl* _temp3246; struct Cyc_Absyn_Vardecl* _temp3248; struct
Cyc_Absyn_Vardecl* _temp3250; struct Cyc_Absyn_Vardecl* _temp3252; _LL3232: if(
_temp3230 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL3233;} else{ goto _LL3234;}
_LL3234: if(( unsigned int) _temp3230 > 1u?*(( int*) _temp3230) == Cyc_Absyn_Funname_b:
0){ _LL3245: _temp3244=(( struct Cyc_Absyn_Funname_b_struct*) _temp3230)->f1;
goto _LL3235;} else{ goto _LL3236;} _LL3236: if(( unsigned int) _temp3230 > 1u?*((
int*) _temp3230) == Cyc_Absyn_Global_b: 0){ _LL3247: _temp3246=(( struct Cyc_Absyn_Global_b_struct*)
_temp3230)->f1; goto _LL3237;} else{ goto _LL3238;} _LL3238: if(( unsigned int)
_temp3230 > 1u?*(( int*) _temp3230) == Cyc_Absyn_Local_b: 0){ _LL3249: _temp3248=((
struct Cyc_Absyn_Local_b_struct*) _temp3230)->f1; goto _LL3239;} else{ goto
_LL3240;} _LL3240: if(( unsigned int) _temp3230 > 1u?*(( int*) _temp3230) == Cyc_Absyn_Pat_b:
0){ _LL3251: _temp3250=(( struct Cyc_Absyn_Pat_b_struct*) _temp3230)->f1; goto
_LL3241;} else{ goto _LL3242;} _LL3242: if(( unsigned int) _temp3230 > 1u?*((
int*) _temp3230) == Cyc_Absyn_Param_b: 0){ _LL3253: _temp3252=(( struct Cyc_Absyn_Param_b_struct*)
_temp3230)->f1; goto _LL3243;} else{ goto _LL3231;} _LL3233: return bogus_ans;
_LL3235: return({ struct _tuple7 _temp3254; _temp3254.f1= 1; _temp3254.f2=( void*)
Cyc_Absyn_HeapRgn; _temp3254;}); _LL3237: { void* _temp3255= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); struct Cyc_Absyn_Exp*
_temp3261; struct Cyc_Absyn_Tqual _temp3263; void* _temp3265; _LL3257: if((
unsigned int) _temp3255 > 4u?*(( int*) _temp3255) == Cyc_Absyn_ArrayType: 0){
_LL3266: _temp3265=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3255)->f1;
goto _LL3264; _LL3264: _temp3263=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3255)->f2; goto _LL3262; _LL3262: _temp3261=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3255)->f3; goto _LL3258;} else{ goto _LL3259;} _LL3259: goto _LL3260;
_LL3258: return({ struct _tuple7 _temp3267; _temp3267.f1= 1; _temp3267.f2=( void*)
Cyc_Absyn_HeapRgn; _temp3267;}); _LL3260: return({ struct _tuple7 _temp3268;
_temp3268.f1=( _temp3246->tq).q_const; _temp3268.f2=( void*) Cyc_Absyn_HeapRgn;
_temp3268;}); _LL3256:;} _LL3239: { void* _temp3269= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v); struct Cyc_Absyn_Exp*
_temp3275; struct Cyc_Absyn_Tqual _temp3277; void* _temp3279; _LL3271: if((
unsigned int) _temp3269 > 4u?*(( int*) _temp3269) == Cyc_Absyn_ArrayType: 0){
_LL3280: _temp3279=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3269)->f1;
goto _LL3278; _LL3278: _temp3277=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3269)->f2; goto _LL3276; _LL3276: _temp3275=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3269)->f3; goto _LL3272;} else{ goto _LL3273;} _LL3273: goto _LL3274;
_LL3272: return({ struct _tuple7 _temp3281; _temp3281.f1= 1; _temp3281.f2=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp3248->rgn))->v; _temp3281;}); _LL3274:
return({ struct _tuple7 _temp3282; _temp3282.f1=( _temp3248->tq).q_const;
_temp3282.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3248->rgn))->v;
_temp3282;}); _LL3270:;} _LL3241: _temp3252= _temp3250; goto _LL3243; _LL3243:
return({ struct _tuple7 _temp3283; _temp3283.f1=( _temp3252->tq).q_const;
_temp3283.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3252->rgn))->v;
_temp3283;}); _LL3231:;} _LL3203: { void* _temp3284= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp3218->topt))->v); struct Cyc_List_List*
_temp3296; struct Cyc_List_List* _temp3298; struct Cyc_Absyn_Structdecl**
_temp3300; struct Cyc_Absyn_Structdecl* _temp3302; struct Cyc_List_List*
_temp3303; struct _tuple1* _temp3305; struct Cyc_Absyn_Uniondecl** _temp3307;
struct Cyc_Absyn_Uniondecl* _temp3309; struct Cyc_List_List* _temp3310; struct
_tuple1* _temp3312; _LL3286: if(( unsigned int) _temp3284 > 4u?*(( int*)
_temp3284) == Cyc_Absyn_AnonStructType: 0){ _LL3297: _temp3296=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3284)->f1; goto _LL3287;} else{ goto _LL3288;} _LL3288: if(( unsigned int)
_temp3284 > 4u?*(( int*) _temp3284) == Cyc_Absyn_AnonUnionType: 0){ _LL3299:
_temp3298=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3284)->f1; goto
_LL3289;} else{ goto _LL3290;} _LL3290: if(( unsigned int) _temp3284 > 4u?*((
int*) _temp3284) == Cyc_Absyn_StructType: 0){ _LL3306: _temp3305=(( struct Cyc_Absyn_StructType_struct*)
_temp3284)->f1; goto _LL3304; _LL3304: _temp3303=(( struct Cyc_Absyn_StructType_struct*)
_temp3284)->f2; goto _LL3301; _LL3301: _temp3300=(( struct Cyc_Absyn_StructType_struct*)
_temp3284)->f3; if( _temp3300 == 0){ goto _LL3292;} else{ _temp3302=* _temp3300;
goto _LL3291;}} else{ goto _LL3292;} _LL3292: if(( unsigned int) _temp3284 > 4u?*((
int*) _temp3284) == Cyc_Absyn_UnionType: 0){ _LL3313: _temp3312=(( struct Cyc_Absyn_UnionType_struct*)
_temp3284)->f1; goto _LL3311; _LL3311: _temp3310=(( struct Cyc_Absyn_UnionType_struct*)
_temp3284)->f2; goto _LL3308; _LL3308: _temp3307=(( struct Cyc_Absyn_UnionType_struct*)
_temp3284)->f3; if( _temp3307 == 0){ goto _LL3294;} else{ _temp3309=* _temp3307;
goto _LL3293;}} else{ goto _LL3294;} _LL3294: goto _LL3295; _LL3287: _temp3298=
_temp3296; goto _LL3289; _LL3289: { struct Cyc_Absyn_Structfield* _temp3314= Cyc_Absyn_lookup_field(
_temp3298, _temp3216); goto _LL3315; _LL3315: if( _temp3314 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3314))->width != 0: 0){ return({ struct _tuple7 _temp3316;
_temp3316.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3314))->tq).q_const;
_temp3316.f2=( Cyc_Tcutil_addressof_props( te, _temp3218)).f2; _temp3316;});}
return bogus_ans;} _LL3291: { struct Cyc_Absyn_Structfield* _temp3317= Cyc_Absyn_lookup_struct_field(
_temp3302, _temp3216); goto _LL3318; _LL3318: if( _temp3317 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3317))->width != 0: 0){ return({ struct _tuple7 _temp3319;
_temp3319.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3317))->tq).q_const;
_temp3319.f2=( Cyc_Tcutil_addressof_props( te, _temp3218)).f2; _temp3319;});}
return bogus_ans;} _LL3293: { struct Cyc_Absyn_Structfield* _temp3320= Cyc_Absyn_lookup_union_field(
_temp3309, _temp3216); goto _LL3321; _LL3321: if( _temp3320 != 0){ return({
struct _tuple7 _temp3322; _temp3322.f1=((( struct Cyc_Absyn_Structfield*)
_check_null( _temp3320))->tq).q_const; _temp3322.f2=( Cyc_Tcutil_addressof_props(
te, _temp3218)).f2; _temp3322;});} goto _LL3295;} _LL3295: return bogus_ans;
_LL3285:;} _LL3205: { void* _temp3323= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3222->topt))->v); struct Cyc_Absyn_PtrInfo _temp3329; struct
Cyc_Absyn_Conref* _temp3331; struct Cyc_Absyn_Tqual _temp3333; struct Cyc_Absyn_Conref*
_temp3335; void* _temp3337; void* _temp3339; _LL3325: if(( unsigned int)
_temp3323 > 4u?*(( int*) _temp3323) == Cyc_Absyn_PointerType: 0){ _LL3330:
_temp3329=(( struct Cyc_Absyn_PointerType_struct*) _temp3323)->f1; _LL3340:
_temp3339=( void*) _temp3329.elt_typ; goto _LL3338; _LL3338: _temp3337=( void*)
_temp3329.rgn_typ; goto _LL3336; _LL3336: _temp3335= _temp3329.nullable; goto
_LL3334; _LL3334: _temp3333= _temp3329.tq; goto _LL3332; _LL3332: _temp3331=
_temp3329.bounds; goto _LL3326;} else{ goto _LL3327;} _LL3327: goto _LL3328;
_LL3326: { void* _temp3341= Cyc_Tcutil_compress( _temp3339); struct Cyc_List_List*
_temp3353; struct Cyc_List_List* _temp3355; struct Cyc_Absyn_Structdecl**
_temp3357; struct Cyc_Absyn_Structdecl* _temp3359; struct Cyc_List_List*
_temp3360; struct _tuple1* _temp3362; struct Cyc_Absyn_Uniondecl** _temp3364;
struct Cyc_Absyn_Uniondecl* _temp3366; struct Cyc_List_List* _temp3367; struct
_tuple1* _temp3369; _LL3343: if(( unsigned int) _temp3341 > 4u?*(( int*)
_temp3341) == Cyc_Absyn_AnonStructType: 0){ _LL3354: _temp3353=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3341)->f1; goto _LL3344;} else{ goto _LL3345;} _LL3345: if(( unsigned int)
_temp3341 > 4u?*(( int*) _temp3341) == Cyc_Absyn_AnonUnionType: 0){ _LL3356:
_temp3355=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3341)->f1; goto
_LL3346;} else{ goto _LL3347;} _LL3347: if(( unsigned int) _temp3341 > 4u?*((
int*) _temp3341) == Cyc_Absyn_StructType: 0){ _LL3363: _temp3362=(( struct Cyc_Absyn_StructType_struct*)
_temp3341)->f1; goto _LL3361; _LL3361: _temp3360=(( struct Cyc_Absyn_StructType_struct*)
_temp3341)->f2; goto _LL3358; _LL3358: _temp3357=(( struct Cyc_Absyn_StructType_struct*)
_temp3341)->f3; if( _temp3357 == 0){ goto _LL3349;} else{ _temp3359=* _temp3357;
goto _LL3348;}} else{ goto _LL3349;} _LL3349: if(( unsigned int) _temp3341 > 4u?*((
int*) _temp3341) == Cyc_Absyn_UnionType: 0){ _LL3370: _temp3369=(( struct Cyc_Absyn_UnionType_struct*)
_temp3341)->f1; goto _LL3368; _LL3368: _temp3367=(( struct Cyc_Absyn_UnionType_struct*)
_temp3341)->f2; goto _LL3365; _LL3365: _temp3364=(( struct Cyc_Absyn_UnionType_struct*)
_temp3341)->f3; if( _temp3364 == 0){ goto _LL3351;} else{ _temp3366=* _temp3364;
goto _LL3350;}} else{ goto _LL3351;} _LL3351: goto _LL3352; _LL3344: _temp3355=
_temp3353; goto _LL3346; _LL3346: { struct Cyc_Absyn_Structfield* _temp3371= Cyc_Absyn_lookup_field(
_temp3355, _temp3220); goto _LL3372; _LL3372: if( _temp3371 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3371))->width != 0: 0){ return({ struct _tuple7 _temp3373;
_temp3373.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3371))->tq).q_const;
_temp3373.f2= _temp3337; _temp3373;});} return bogus_ans;} _LL3348: { struct Cyc_Absyn_Structfield*
_temp3374= Cyc_Absyn_lookup_struct_field( _temp3359, _temp3220); goto _LL3375;
_LL3375: if( _temp3374 != 0?(( struct Cyc_Absyn_Structfield*) _check_null(
_temp3374))->width != 0: 0){ return({ struct _tuple7 _temp3376; _temp3376.f1=(((
struct Cyc_Absyn_Structfield*) _check_null( _temp3374))->tq).q_const; _temp3376.f2=
_temp3337; _temp3376;});} return bogus_ans;} _LL3350: { struct Cyc_Absyn_Structfield*
_temp3377= Cyc_Absyn_lookup_union_field( _temp3366, _temp3220); goto _LL3378;
_LL3378: if( _temp3377 != 0){ return({ struct _tuple7 _temp3379; _temp3379.f1=(((
struct Cyc_Absyn_Structfield*) _check_null( _temp3377))->tq).q_const; _temp3379.f2=
_temp3337; _temp3379;});} return bogus_ans;} _LL3352: return bogus_ans; _LL3342:;}
_LL3328: return bogus_ans; _LL3324:;} _LL3207: { void* _temp3380= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp3224->topt))->v); struct Cyc_Absyn_PtrInfo
_temp3386; struct Cyc_Absyn_Conref* _temp3388; struct Cyc_Absyn_Tqual _temp3390;
struct Cyc_Absyn_Conref* _temp3392; void* _temp3394; void* _temp3396; _LL3382:
if(( unsigned int) _temp3380 > 4u?*(( int*) _temp3380) == Cyc_Absyn_PointerType:
0){ _LL3387: _temp3386=(( struct Cyc_Absyn_PointerType_struct*) _temp3380)->f1;
_LL3397: _temp3396=( void*) _temp3386.elt_typ; goto _LL3395; _LL3395: _temp3394=(
void*) _temp3386.rgn_typ; goto _LL3393; _LL3393: _temp3392= _temp3386.nullable;
goto _LL3391; _LL3391: _temp3390= _temp3386.tq; goto _LL3389; _LL3389: _temp3388=
_temp3386.bounds; goto _LL3383;} else{ goto _LL3384;} _LL3384: goto _LL3385;
_LL3383: return({ struct _tuple7 _temp3398; _temp3398.f1= _temp3390.q_const;
_temp3398.f2= _temp3394; _temp3398;}); _LL3385: return bogus_ans; _LL3381:;}
_LL3209: { void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3228->topt))->v); void* _temp3399= t; struct Cyc_Absyn_Exp*
_temp3409; struct Cyc_Absyn_Tqual _temp3411; void* _temp3413; struct Cyc_List_List*
_temp3415; struct Cyc_Absyn_PtrInfo _temp3417; struct Cyc_Absyn_Conref*
_temp3419; struct Cyc_Absyn_Tqual _temp3421; struct Cyc_Absyn_Conref* _temp3423;
void* _temp3425; void* _temp3427; _LL3401: if(( unsigned int) _temp3399 > 4u?*((
int*) _temp3399) == Cyc_Absyn_ArrayType: 0){ _LL3414: _temp3413=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3399)->f1; goto _LL3412; _LL3412:
_temp3411=(( struct Cyc_Absyn_ArrayType_struct*) _temp3399)->f2; goto _LL3410;
_LL3410: _temp3409=(( struct Cyc_Absyn_ArrayType_struct*) _temp3399)->f3; goto
_LL3402;} else{ goto _LL3403;} _LL3403: if(( unsigned int) _temp3399 > 4u?*((
int*) _temp3399) == Cyc_Absyn_TupleType: 0){ _LL3416: _temp3415=(( struct Cyc_Absyn_TupleType_struct*)
_temp3399)->f1; goto _LL3404;} else{ goto _LL3405;} _LL3405: if(( unsigned int)
_temp3399 > 4u?*(( int*) _temp3399) == Cyc_Absyn_PointerType: 0){ _LL3418:
_temp3417=(( struct Cyc_Absyn_PointerType_struct*) _temp3399)->f1; _LL3428:
_temp3427=( void*) _temp3417.elt_typ; goto _LL3426; _LL3426: _temp3425=( void*)
_temp3417.rgn_typ; goto _LL3424; _LL3424: _temp3423= _temp3417.nullable; goto
_LL3422; _LL3422: _temp3421= _temp3417.tq; goto _LL3420; _LL3420: _temp3419=
_temp3417.bounds; goto _LL3406;} else{ goto _LL3407;} _LL3407: goto _LL3408;
_LL3402: return({ struct _tuple7 _temp3429; _temp3429.f1= _temp3411.q_const;
_temp3429.f2=( Cyc_Tcutil_addressof_props( te, _temp3228)).f2; _temp3429;});
_LL3404: { struct _tuple4* _temp3430= Cyc_Absyn_lookup_tuple_field( _temp3415,(
int) Cyc_Evexp_eval_const_uint_exp( _temp3226)); goto _LL3431; _LL3431: if(
_temp3430 != 0){ return({ struct _tuple7 _temp3432; _temp3432.f1=((*(( struct
_tuple4*) _check_null( _temp3430))).f1).q_const; _temp3432.f2=( Cyc_Tcutil_addressof_props(
te, _temp3228)).f2; _temp3432;});} return bogus_ans;} _LL3406: return({ struct
_tuple7 _temp3433; _temp3433.f1= _temp3421.q_const; _temp3433.f2= _temp3425;
_temp3433;}); _LL3408: return bogus_ans; _LL3400:;} _LL3211: Cyc_Tcutil_terr( e->loc,({
unsigned char* _temp3434=( unsigned char*)"unary & applied to non-lvalue";
struct _tagged_string _temp3435; _temp3435.curr= _temp3434; _temp3435.base=
_temp3434; _temp3435.last_plus_one= _temp3434 + 30; _temp3435;})); return
bogus_ans; _LL3199:;} void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv* te,
void* e_typ, struct Cyc_Absyn_Exp* e){ void* _temp3437= Cyc_Tcutil_compress(
e_typ); struct Cyc_Absyn_Exp* _temp3443; struct Cyc_Absyn_Tqual _temp3445; void*
_temp3447; _LL3439: if(( unsigned int) _temp3437 > 4u?*(( int*) _temp3437) ==
Cyc_Absyn_ArrayType: 0){ _LL3448: _temp3447=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3437)->f1; goto _LL3446; _LL3446: _temp3445=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3437)->f2; goto _LL3444; _LL3444: _temp3443=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3437)->f3; goto _LL3440;} else{ goto _LL3441;} _LL3441: goto _LL3442;
_LL3440: { void* _temp3451; int _temp3453; struct _tuple7 _temp3449= Cyc_Tcutil_addressof_props(
te, e); _LL3454: _temp3453= _temp3449.f1; goto _LL3452; _LL3452: _temp3451=
_temp3449.f2; goto _LL3450; _LL3450: return Cyc_Absyn_atb_typ( _temp3447,
_temp3451, _temp3445,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3455=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3455[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3456; _temp3456.tag= Cyc_Absyn_Upper_b;
_temp3456.f1= e; _temp3456;}); _temp3455;}));} _LL3442: return e_typ; _LL3438:;}
void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment* loc, unsigned int i,
struct Cyc_Absyn_Conref* b){ b=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( b);{ void* _temp3457=( void*) b->v; void*
_temp3465; void* _temp3467; struct Cyc_Absyn_Exp* _temp3469; _LL3459: if((
unsigned int) _temp3457 > 1u?*(( int*) _temp3457) == Cyc_Absyn_Eq_constr: 0){
_LL3466: _temp3465=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3457)->f1;
if( _temp3465 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3460;} else{ goto _LL3461;}}
else{ goto _LL3461;} _LL3461: if(( unsigned int) _temp3457 > 1u?*(( int*)
_temp3457) == Cyc_Absyn_Eq_constr: 0){ _LL3468: _temp3467=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3457)->f1; if(( unsigned int) _temp3467 > 1u?*(( int*) _temp3467) == Cyc_Absyn_Upper_b:
0){ _LL3470: _temp3469=(( struct Cyc_Absyn_Upper_b_struct*) _temp3467)->f1; goto
_LL3462;} else{ goto _LL3463;}} else{ goto _LL3463;} _LL3463: goto _LL3464;
_LL3460: return; _LL3462: if( Cyc_Evexp_eval_const_uint_exp( _temp3469) <= i){
Cyc_Tcutil_terr( loc,({ unsigned char* _temp3471=( unsigned char*)"dereference is out of bounds";
struct _tagged_string _temp3472; _temp3472.curr= _temp3471; _temp3472.base=
_temp3471; _temp3472.last_plus_one= _temp3471 + 29; _temp3472;}));} return;
_LL3464:( void*)( b->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3473=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3473[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3474; _temp3474.tag= Cyc_Absyn_Eq_constr;
_temp3474.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3475=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3475[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3476; _temp3476.tag= Cyc_Absyn_Upper_b;
_temp3476.f1= Cyc_Absyn_uint_exp( i + 1, 0); _temp3476;}); _temp3475;}));
_temp3474;}); _temp3473;}))); return; _LL3458:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_bits_only( void* t){ void* _temp3477= Cyc_Tcutil_compress(
t); void* _temp3503; void* _temp3505; struct Cyc_Absyn_Enumdecl* _temp3507;
struct _tuple1* _temp3509; struct Cyc_Absyn_Exp* _temp3511; struct Cyc_Absyn_Tqual
_temp3513; void* _temp3515; struct Cyc_List_List* _temp3517; struct Cyc_Absyn_Structdecl**
_temp3519; struct Cyc_List_List* _temp3521; struct _tuple1* _temp3523; struct
Cyc_Absyn_Uniondecl** _temp3525; struct Cyc_List_List* _temp3527; struct _tuple1*
_temp3529; struct Cyc_List_List* _temp3531; struct Cyc_List_List* _temp3533;
_LL3479: if( _temp3477 ==( void*) Cyc_Absyn_VoidType){ goto _LL3480;} else{ goto
_LL3481;} _LL3481: if(( unsigned int) _temp3477 > 4u?*(( int*) _temp3477) == Cyc_Absyn_IntType:
0){ _LL3506: _temp3505=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3477)->f1;
goto _LL3504; _LL3504: _temp3503=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3477)->f2; goto _LL3482;} else{ goto _LL3483;} _LL3483: if( _temp3477 ==(
void*) Cyc_Absyn_FloatType){ goto _LL3484;} else{ goto _LL3485;} _LL3485: if(
_temp3477 ==( void*) Cyc_Absyn_DoubleType){ goto _LL3486;} else{ goto _LL3487;}
_LL3487: if(( unsigned int) _temp3477 > 4u?*(( int*) _temp3477) == Cyc_Absyn_EnumType:
0){ _LL3510: _temp3509=(( struct Cyc_Absyn_EnumType_struct*) _temp3477)->f1;
goto _LL3508; _LL3508: _temp3507=(( struct Cyc_Absyn_EnumType_struct*) _temp3477)->f2;
goto _LL3488;} else{ goto _LL3489;} _LL3489: if(( unsigned int) _temp3477 > 4u?*((
int*) _temp3477) == Cyc_Absyn_ArrayType: 0){ _LL3516: _temp3515=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3477)->f1; goto _LL3514; _LL3514:
_temp3513=(( struct Cyc_Absyn_ArrayType_struct*) _temp3477)->f2; goto _LL3512;
_LL3512: _temp3511=(( struct Cyc_Absyn_ArrayType_struct*) _temp3477)->f3; goto
_LL3490;} else{ goto _LL3491;} _LL3491: if(( unsigned int) _temp3477 > 4u?*((
int*) _temp3477) == Cyc_Absyn_TupleType: 0){ _LL3518: _temp3517=(( struct Cyc_Absyn_TupleType_struct*)
_temp3477)->f1; goto _LL3492;} else{ goto _LL3493;} _LL3493: if(( unsigned int)
_temp3477 > 4u?*(( int*) _temp3477) == Cyc_Absyn_StructType: 0){ _LL3524:
_temp3523=(( struct Cyc_Absyn_StructType_struct*) _temp3477)->f1; goto _LL3522;
_LL3522: _temp3521=(( struct Cyc_Absyn_StructType_struct*) _temp3477)->f2; goto
_LL3520; _LL3520: _temp3519=(( struct Cyc_Absyn_StructType_struct*) _temp3477)->f3;
goto _LL3494;} else{ goto _LL3495;} _LL3495: if(( unsigned int) _temp3477 > 4u?*((
int*) _temp3477) == Cyc_Absyn_UnionType: 0){ _LL3530: _temp3529=(( struct Cyc_Absyn_UnionType_struct*)
_temp3477)->f1; goto _LL3528; _LL3528: _temp3527=(( struct Cyc_Absyn_UnionType_struct*)
_temp3477)->f2; goto _LL3526; _LL3526: _temp3525=(( struct Cyc_Absyn_UnionType_struct*)
_temp3477)->f3; goto _LL3496;} else{ goto _LL3497;} _LL3497: if(( unsigned int)
_temp3477 > 4u?*(( int*) _temp3477) == Cyc_Absyn_AnonStructType: 0){ _LL3532:
_temp3531=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3477)->f1; goto
_LL3498;} else{ goto _LL3499;} _LL3499: if(( unsigned int) _temp3477 > 4u?*((
int*) _temp3477) == Cyc_Absyn_AnonUnionType: 0){ _LL3534: _temp3533=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3477)->f1; goto _LL3500;} else{ goto
_LL3501;} _LL3501: goto _LL3502; _LL3480: return 1; _LL3482: return 1; _LL3484:
return 1; _LL3486: return 1; _LL3488: return 0; _LL3490: return Cyc_Tcutil_bits_only(
_temp3515); _LL3492: for( 0; _temp3517 != 0; _temp3517=(( struct Cyc_List_List*)
_check_null( _temp3517))->tl){ if( ! Cyc_Tcutil_bits_only((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp3517))->hd)).f2)){ return 0;}} return 1;
_LL3494: if( _temp3519 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* _temp3535=*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp3519)); goto _LL3536; _LL3536:
if( _temp3535->fields == 0){ return 0;}{ struct _RegionHandle _temp3537=
_new_region(); struct _RegionHandle* rgn=& _temp3537; _push_region( rgn);{
struct Cyc_List_List* _temp3538=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp3535->tvs, _temp3521); goto _LL3539; _LL3539:{
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp3535->fields))->v; for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute( rgn,
_temp3538,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3540= 0; _npop_handler( 0u); return
_temp3540;}}}{ int _temp3541= 1; _npop_handler( 0u); return _temp3541;}};
_pop_region(& _temp3537);}} _LL3496: if( _temp3525 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
_temp3542=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp3525)); goto
_LL3543; _LL3543: if( _temp3542->fields == 0){ return 0;}{ struct _RegionHandle
_temp3544= _new_region(); struct _RegionHandle* rgn=& _temp3544; _push_region(
rgn);{ struct Cyc_List_List* _temp3545=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp3542->tvs, _temp3527); goto _LL3546; _LL3546:{
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp3542->fields))->v; for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute( rgn,
_temp3545,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3547= 0; _npop_handler( 0u); return
_temp3547;}}}{ int _temp3548= 1; _npop_handler( 0u); return _temp3548;}};
_pop_region(& _temp3544);}} _LL3498: _temp3533= _temp3531; goto _LL3500; _LL3500:
for( 0; _temp3533 != 0; _temp3533=(( struct Cyc_List_List*) _check_null(
_temp3533))->tl){ if( ! Cyc_Tcutil_bits_only(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp3533))->hd)->type)){ return 0;}} return
1; _LL3502: return 0; _LL3478:;} struct _tuple20{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; static int Cyc_Tcutil_cnst_exp( struct Cyc_Tcenv_Tenv*
te, int var_okay, struct Cyc_Absyn_Exp* e){ void* _temp3549=( void*) e->r; void*
_temp3589; void* _temp3591; struct Cyc_Absyn_Exp* _temp3593; struct Cyc_Absyn_Enumfield*
_temp3595; struct Cyc_Absyn_Enumdecl* _temp3597; struct _tuple1* _temp3599; void*
_temp3601; struct _tuple1* _temp3603; struct Cyc_Absyn_Exp* _temp3605; struct
Cyc_Absyn_Exp* _temp3607; struct Cyc_Absyn_Exp* _temp3609; struct Cyc_Absyn_Exp*
_temp3611; struct Cyc_Absyn_Exp* _temp3613; struct Cyc_Absyn_Exp* _temp3615;
struct Cyc_List_List* _temp3617; struct Cyc_Absyn_Exp* _temp3619; struct Cyc_Absyn_Exp*
_temp3621; void* _temp3623; struct Cyc_Absyn_Exp* _temp3625; struct Cyc_Absyn_Exp*
_temp3627; struct Cyc_Absyn_Exp* _temp3629; struct Cyc_Absyn_Vardecl* _temp3631;
struct Cyc_List_List* _temp3633; struct Cyc_List_List* _temp3635; void*
_temp3637; struct Cyc_Absyn_Structdecl* _temp3639; struct Cyc_List_List*
_temp3641; struct Cyc_Core_Opt* _temp3643; struct _tuple1* _temp3645; struct Cyc_List_List*
_temp3647; void* _temp3649; struct Cyc_List_List* _temp3651; struct Cyc_Absyn_Tunionfield*
_temp3653; struct Cyc_Absyn_Tuniondecl* _temp3655; struct Cyc_List_List*
_temp3657; struct Cyc_Core_Opt* _temp3659; struct Cyc_Core_Opt* _temp3661;
_LL3551: if(*(( int*) _temp3549) == Cyc_Absyn_Const_e){ _LL3590: _temp3589=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp3549)->f1; goto _LL3552;} else{
goto _LL3553;} _LL3553: if(*(( int*) _temp3549) == Cyc_Absyn_Sizeoftyp_e){
_LL3592: _temp3591=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp3549)->f1;
goto _LL3554;} else{ goto _LL3555;} _LL3555: if(*(( int*) _temp3549) == Cyc_Absyn_Sizeofexp_e){
_LL3594: _temp3593=(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp3549)->f1; goto
_LL3556;} else{ goto _LL3557;} _LL3557: if(*(( int*) _temp3549) == Cyc_Absyn_Enum_e){
_LL3600: _temp3599=(( struct Cyc_Absyn_Enum_e_struct*) _temp3549)->f1; goto
_LL3598; _LL3598: _temp3597=(( struct Cyc_Absyn_Enum_e_struct*) _temp3549)->f2;
goto _LL3596; _LL3596: _temp3595=(( struct Cyc_Absyn_Enum_e_struct*) _temp3549)->f3;
goto _LL3558;} else{ goto _LL3559;} _LL3559: if(*(( int*) _temp3549) == Cyc_Absyn_Var_e){
_LL3604: _temp3603=(( struct Cyc_Absyn_Var_e_struct*) _temp3549)->f1; goto
_LL3602; _LL3602: _temp3601=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp3549)->f2;
goto _LL3560;} else{ goto _LL3561;} _LL3561: if(*(( int*) _temp3549) == Cyc_Absyn_Conditional_e){
_LL3610: _temp3609=(( struct Cyc_Absyn_Conditional_e_struct*) _temp3549)->f1;
goto _LL3608; _LL3608: _temp3607=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3549)->f2; goto _LL3606; _LL3606: _temp3605=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3549)->f3; goto _LL3562;} else{ goto _LL3563;} _LL3563: if(*(( int*)
_temp3549) == Cyc_Absyn_SeqExp_e){ _LL3614: _temp3613=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3549)->f1; goto _LL3612; _LL3612: _temp3611=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3549)->f2; goto _LL3564;} else{ goto _LL3565;} _LL3565: if(*(( int*)
_temp3549) == Cyc_Absyn_NoInstantiate_e){ _LL3616: _temp3615=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp3549)->f1; goto _LL3566;} else{ goto _LL3567;} _LL3567: if(*(( int*)
_temp3549) == Cyc_Absyn_Instantiate_e){ _LL3620: _temp3619=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3549)->f1; goto _LL3618; _LL3618: _temp3617=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3549)->f2; goto _LL3568;} else{ goto _LL3569;} _LL3569: if(*(( int*)
_temp3549) == Cyc_Absyn_Cast_e){ _LL3624: _temp3623=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp3549)->f1; goto _LL3622; _LL3622: _temp3621=(( struct Cyc_Absyn_Cast_e_struct*)
_temp3549)->f2; goto _LL3570;} else{ goto _LL3571;} _LL3571: if(*(( int*)
_temp3549) == Cyc_Absyn_Address_e){ _LL3626: _temp3625=(( struct Cyc_Absyn_Address_e_struct*)
_temp3549)->f1; goto _LL3572;} else{ goto _LL3573;} _LL3573: if(*(( int*)
_temp3549) == Cyc_Absyn_Comprehension_e){ _LL3632: _temp3631=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3549)->f1; goto _LL3630; _LL3630: _temp3629=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3549)->f2; goto _LL3628; _LL3628: _temp3627=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3549)->f3; goto _LL3574;} else{ goto _LL3575;} _LL3575: if(*(( int*)
_temp3549) == Cyc_Absyn_Array_e){ _LL3634: _temp3633=(( struct Cyc_Absyn_Array_e_struct*)
_temp3549)->f1; goto _LL3576;} else{ goto _LL3577;} _LL3577: if(*(( int*)
_temp3549) == Cyc_Absyn_AnonStruct_e){ _LL3638: _temp3637=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3549)->f1; goto _LL3636; _LL3636: _temp3635=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3549)->f2; goto _LL3578;} else{ goto _LL3579;} _LL3579: if(*(( int*)
_temp3549) == Cyc_Absyn_Struct_e){ _LL3646: _temp3645=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3549)->f1; goto _LL3644; _LL3644: _temp3643=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3549)->f2; goto _LL3642; _LL3642: _temp3641=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3549)->f3; goto _LL3640; _LL3640: _temp3639=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3549)->f4; goto _LL3580;} else{ goto _LL3581;} _LL3581: if(*(( int*)
_temp3549) == Cyc_Absyn_Primop_e){ _LL3650: _temp3649=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp3549)->f1; goto _LL3648; _LL3648: _temp3647=(( struct Cyc_Absyn_Primop_e_struct*)
_temp3549)->f2; goto _LL3582;} else{ goto _LL3583;} _LL3583: if(*(( int*)
_temp3549) == Cyc_Absyn_Tuple_e){ _LL3652: _temp3651=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp3549)->f1; goto _LL3584;} else{ goto _LL3585;} _LL3585: if(*(( int*)
_temp3549) == Cyc_Absyn_Tunion_e){ _LL3662: _temp3661=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3549)->f1; goto _LL3660; _LL3660: _temp3659=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3549)->f2; goto _LL3658; _LL3658: _temp3657=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3549)->f3; goto _LL3656; _LL3656: _temp3655=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3549)->f4; goto _LL3654; _LL3654: _temp3653=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3549)->f5; goto _LL3586;} else{ goto _LL3587;} _LL3587: goto _LL3588;
_LL3552: return 1; _LL3554: return 1; _LL3556: return Cyc_Tcutil_cnst_exp( te, 1,
_temp3593); _LL3558: return 1; _LL3560: { struct _handler_cons _temp3663;
_push_handler(& _temp3663);{ int _temp3665= 0; if( setjmp( _temp3663.handler)){
_temp3665= 1;} if( ! _temp3665){{ void* _temp3666= Cyc_Tcenv_lookup_ordinary( te,
e->loc, _temp3603); void* _temp3674; struct Cyc_Absyn_Fndecl* _temp3676; void*
_temp3678; struct Cyc_Absyn_Vardecl* _temp3680; _LL3668: if(*(( int*) _temp3666)
== Cyc_Tcenv_VarRes){ _LL3675: _temp3674=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3666)->f1; if(( unsigned int) _temp3674 > 1u?*(( int*) _temp3674) == Cyc_Absyn_Funname_b:
0){ _LL3677: _temp3676=(( struct Cyc_Absyn_Funname_b_struct*) _temp3674)->f1;
goto _LL3669;} else{ goto _LL3670;}} else{ goto _LL3670;} _LL3670: if(*(( int*)
_temp3666) == Cyc_Tcenv_VarRes){ _LL3679: _temp3678=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3666)->f1; if(( unsigned int) _temp3678 > 1u?*(( int*) _temp3678) == Cyc_Absyn_Global_b:
0){ _LL3681: _temp3680=(( struct Cyc_Absyn_Global_b_struct*) _temp3678)->f1;
goto _LL3671;} else{ goto _LL3672;}} else{ goto _LL3672;} _LL3672: goto _LL3673;
_LL3669: { int _temp3682= 1; _npop_handler( 0u); return _temp3682;} _LL3671: {
void* _temp3683= Cyc_Tcutil_compress(( void*) _temp3680->type); struct Cyc_Absyn_Exp*
_temp3689; struct Cyc_Absyn_Tqual _temp3691; void* _temp3693; _LL3685: if((
unsigned int) _temp3683 > 4u?*(( int*) _temp3683) == Cyc_Absyn_ArrayType: 0){
_LL3694: _temp3693=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3683)->f1;
goto _LL3692; _LL3692: _temp3691=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3683)->f2; goto _LL3690; _LL3690: _temp3689=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3683)->f3; goto _LL3686;} else{ goto _LL3687;} _LL3687: goto _LL3688;
_LL3686: { int _temp3695= 1; _npop_handler( 0u); return _temp3695;} _LL3688: {
int _temp3696= var_okay; _npop_handler( 0u); return _temp3696;} _LL3684:;}
_LL3673: { int _temp3697= var_okay; _npop_handler( 0u); return _temp3697;}
_LL3667:;}; _pop_handler();} else{ void* _temp3664=( void*) _exn_thrown; void*
_temp3699= _temp3664; _LL3701: if( _temp3699 == Cyc_Dict_Absent){ goto _LL3702;}
else{ goto _LL3703;} _LL3703: goto _LL3704; _LL3702: return 0; _LL3704:( void)
_throw( _temp3699); _LL3700:;}}} _LL3562: return( Cyc_Tcutil_cnst_exp( te, 0,
_temp3609)? Cyc_Tcutil_cnst_exp( te, 0, _temp3607): 0)? Cyc_Tcutil_cnst_exp( te,
0, _temp3605): 0; _LL3564: return Cyc_Tcutil_cnst_exp( te, 0, _temp3613)? Cyc_Tcutil_cnst_exp(
te, 0, _temp3611): 0; _LL3566: _temp3619= _temp3615; goto _LL3568; _LL3568:
return Cyc_Tcutil_cnst_exp( te, var_okay, _temp3619); _LL3570: return Cyc_Tcutil_cnst_exp(
te, var_okay, _temp3621); _LL3572: return Cyc_Tcutil_cnst_exp( te, 1, _temp3625);
_LL3574: return Cyc_Tcutil_cnst_exp( te, 0, _temp3629)? Cyc_Tcutil_cnst_exp( te,
0, _temp3627): 0; _LL3576: _temp3635= _temp3633; goto _LL3578; _LL3578:
_temp3641= _temp3635; goto _LL3580; _LL3580: for( 0; _temp3641 != 0; _temp3641=((
struct Cyc_List_List*) _check_null( _temp3641))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,(*(( struct _tuple20*)(( struct Cyc_List_List*) _check_null( _temp3641))->hd)).f2)){
return 0;}} return 1; _LL3582: _temp3651= _temp3647; goto _LL3584; _LL3584:
_temp3657= _temp3651; goto _LL3586; _LL3586: for( 0; _temp3657 != 0; _temp3657=((
struct Cyc_List_List*) _check_null( _temp3657))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp3657))->hd)){
return 0;}} return 1; _LL3588: return 0; _LL3550:;} int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcutil_cnst_exp(
te, 0, e);} static int Cyc_Tcutil_fields_support_default( struct Cyc_List_List*
tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs); int Cyc_Tcutil_supports_default(
void* t){ void* _temp3705= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt*
_temp3743; int _temp3745; struct Cyc_Core_Opt* _temp3747; struct Cyc_Core_Opt*
_temp3749; struct Cyc_Absyn_Tvar* _temp3751; struct Cyc_Absyn_TunionInfo
_temp3753; struct Cyc_Absyn_TunionFieldInfo _temp3755; struct Cyc_Absyn_PtrInfo
_temp3757; struct Cyc_Absyn_Conref* _temp3759; struct Cyc_Absyn_Tqual _temp3761;
struct Cyc_Absyn_Conref* _temp3763; void* _temp3765; void* _temp3767; void*
_temp3769; void* _temp3771; struct Cyc_Absyn_Exp* _temp3773; struct Cyc_Absyn_Tqual
_temp3775; void* _temp3777; struct Cyc_Absyn_FnInfo _temp3779; struct Cyc_List_List*
_temp3781; struct Cyc_Absyn_Structdecl** _temp3783; struct Cyc_List_List*
_temp3785; struct _tuple1* _temp3787; struct Cyc_Absyn_Uniondecl** _temp3789;
struct Cyc_List_List* _temp3791; struct _tuple1* _temp3793; struct Cyc_List_List*
_temp3795; struct Cyc_List_List* _temp3797; struct Cyc_Absyn_Enumdecl* _temp3799;
struct _tuple1* _temp3801; _LL3707: if( _temp3705 ==( void*) Cyc_Absyn_VoidType){
goto _LL3708;} else{ goto _LL3709;} _LL3709: if(( unsigned int) _temp3705 > 4u?*((
int*) _temp3705) == Cyc_Absyn_Evar: 0){ _LL3750: _temp3749=(( struct Cyc_Absyn_Evar_struct*)
_temp3705)->f1; goto _LL3748; _LL3748: _temp3747=(( struct Cyc_Absyn_Evar_struct*)
_temp3705)->f2; goto _LL3746; _LL3746: _temp3745=(( struct Cyc_Absyn_Evar_struct*)
_temp3705)->f3; goto _LL3744; _LL3744: _temp3743=(( struct Cyc_Absyn_Evar_struct*)
_temp3705)->f4; goto _LL3710;} else{ goto _LL3711;} _LL3711: if(( unsigned int)
_temp3705 > 4u?*(( int*) _temp3705) == Cyc_Absyn_VarType: 0){ _LL3752: _temp3751=((
struct Cyc_Absyn_VarType_struct*) _temp3705)->f1; goto _LL3712;} else{ goto
_LL3713;} _LL3713: if(( unsigned int) _temp3705 > 4u?*(( int*) _temp3705) == Cyc_Absyn_TunionType:
0){ _LL3754: _temp3753=(( struct Cyc_Absyn_TunionType_struct*) _temp3705)->f1;
goto _LL3714;} else{ goto _LL3715;} _LL3715: if(( unsigned int) _temp3705 > 4u?*((
int*) _temp3705) == Cyc_Absyn_TunionFieldType: 0){ _LL3756: _temp3755=(( struct
Cyc_Absyn_TunionFieldType_struct*) _temp3705)->f1; goto _LL3716;} else{ goto
_LL3717;} _LL3717: if(( unsigned int) _temp3705 > 4u?*(( int*) _temp3705) == Cyc_Absyn_PointerType:
0){ _LL3758: _temp3757=(( struct Cyc_Absyn_PointerType_struct*) _temp3705)->f1;
_LL3768: _temp3767=( void*) _temp3757.elt_typ; goto _LL3766; _LL3766: _temp3765=(
void*) _temp3757.rgn_typ; goto _LL3764; _LL3764: _temp3763= _temp3757.nullable;
goto _LL3762; _LL3762: _temp3761= _temp3757.tq; goto _LL3760; _LL3760: _temp3759=
_temp3757.bounds; goto _LL3718;} else{ goto _LL3719;} _LL3719: if(( unsigned int)
_temp3705 > 4u?*(( int*) _temp3705) == Cyc_Absyn_IntType: 0){ _LL3772: _temp3771=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp3705)->f1; goto _LL3770; _LL3770:
_temp3769=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3705)->f2; goto
_LL3720;} else{ goto _LL3721;} _LL3721: if( _temp3705 ==( void*) Cyc_Absyn_FloatType){
goto _LL3722;} else{ goto _LL3723;} _LL3723: if( _temp3705 ==( void*) Cyc_Absyn_DoubleType){
goto _LL3724;} else{ goto _LL3725;} _LL3725: if(( unsigned int) _temp3705 > 4u?*((
int*) _temp3705) == Cyc_Absyn_ArrayType: 0){ _LL3778: _temp3777=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3705)->f1; goto _LL3776; _LL3776:
_temp3775=(( struct Cyc_Absyn_ArrayType_struct*) _temp3705)->f2; goto _LL3774;
_LL3774: _temp3773=(( struct Cyc_Absyn_ArrayType_struct*) _temp3705)->f3; goto
_LL3726;} else{ goto _LL3727;} _LL3727: if(( unsigned int) _temp3705 > 4u?*((
int*) _temp3705) == Cyc_Absyn_FnType: 0){ _LL3780: _temp3779=(( struct Cyc_Absyn_FnType_struct*)
_temp3705)->f1; goto _LL3728;} else{ goto _LL3729;} _LL3729: if(( unsigned int)
_temp3705 > 4u?*(( int*) _temp3705) == Cyc_Absyn_TupleType: 0){ _LL3782:
_temp3781=(( struct Cyc_Absyn_TupleType_struct*) _temp3705)->f1; goto _LL3730;}
else{ goto _LL3731;} _LL3731: if(( unsigned int) _temp3705 > 4u?*(( int*)
_temp3705) == Cyc_Absyn_StructType: 0){ _LL3788: _temp3787=(( struct Cyc_Absyn_StructType_struct*)
_temp3705)->f1; goto _LL3786; _LL3786: _temp3785=(( struct Cyc_Absyn_StructType_struct*)
_temp3705)->f2; goto _LL3784; _LL3784: _temp3783=(( struct Cyc_Absyn_StructType_struct*)
_temp3705)->f3; goto _LL3732;} else{ goto _LL3733;} _LL3733: if(( unsigned int)
_temp3705 > 4u?*(( int*) _temp3705) == Cyc_Absyn_UnionType: 0){ _LL3794:
_temp3793=(( struct Cyc_Absyn_UnionType_struct*) _temp3705)->f1; goto _LL3792;
_LL3792: _temp3791=(( struct Cyc_Absyn_UnionType_struct*) _temp3705)->f2; goto
_LL3790; _LL3790: _temp3789=(( struct Cyc_Absyn_UnionType_struct*) _temp3705)->f3;
goto _LL3734;} else{ goto _LL3735;} _LL3735: if(( unsigned int) _temp3705 > 4u?*((
int*) _temp3705) == Cyc_Absyn_AnonStructType: 0){ _LL3796: _temp3795=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp3705)->f1; goto _LL3736;} else{ goto
_LL3737;} _LL3737: if(( unsigned int) _temp3705 > 4u?*(( int*) _temp3705) == Cyc_Absyn_AnonUnionType:
0){ _LL3798: _temp3797=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3705)->f1;
goto _LL3738;} else{ goto _LL3739;} _LL3739: if(( unsigned int) _temp3705 > 4u?*((
int*) _temp3705) == Cyc_Absyn_EnumType: 0){ _LL3802: _temp3801=(( struct Cyc_Absyn_EnumType_struct*)
_temp3705)->f1; goto _LL3800; _LL3800: _temp3799=(( struct Cyc_Absyn_EnumType_struct*)
_temp3705)->f2; goto _LL3740;} else{ goto _LL3741;} _LL3741: goto _LL3742;
_LL3708: return 1; _LL3710: return 0; _LL3712: return 0; _LL3714: return 0;
_LL3716: return 0; _LL3718: { void* _temp3803=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3759))->v; void*
_temp3811; void* _temp3813; struct Cyc_Absyn_Exp* _temp3815; _LL3805: if((
unsigned int) _temp3803 > 1u?*(( int*) _temp3803) == Cyc_Absyn_Eq_constr: 0){
_LL3812: _temp3811=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3803)->f1;
if( _temp3811 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3806;} else{ goto _LL3807;}}
else{ goto _LL3807;} _LL3807: if(( unsigned int) _temp3803 > 1u?*(( int*)
_temp3803) == Cyc_Absyn_Eq_constr: 0){ _LL3814: _temp3813=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3803)->f1; if(( unsigned int) _temp3813 > 1u?*(( int*) _temp3813) == Cyc_Absyn_Upper_b:
0){ _LL3816: _temp3815=(( struct Cyc_Absyn_Upper_b_struct*) _temp3813)->f1; goto
_LL3808;} else{ goto _LL3809;}} else{ goto _LL3809;} _LL3809: goto _LL3810;
_LL3806: return 1; _LL3808: { void* _temp3817=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3763))->v; int
_temp3823; _LL3819: if(( unsigned int) _temp3817 > 1u?*(( int*) _temp3817) ==
Cyc_Absyn_Eq_constr: 0){ _LL3824: _temp3823=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3817)->f1; goto _LL3820;} else{ goto _LL3821;} _LL3821: goto _LL3822;
_LL3820: return _temp3823; _LL3822: return 0; _LL3818:;} _LL3810: return 0;
_LL3804:;} _LL3720: return 1; _LL3722: return 1; _LL3724: return 1; _LL3726:
return Cyc_Tcutil_supports_default( _temp3777); _LL3728: return 0; _LL3730: for(
0; _temp3781 != 0; _temp3781=(( struct Cyc_List_List*) _check_null( _temp3781))->tl){
if( ! Cyc_Tcutil_supports_default((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp3781))->hd)).f2)){ return 0;}} return 1; _LL3732: if(
_temp3783 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp3783)); if( sd->fields == 0){ return 0;} return Cyc_Tcutil_fields_support_default(
sd->tvs, _temp3785,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
sd->fields))->v);} _LL3734: if( _temp3789 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp3789)); if( ud->fields ==
0){ return 0;} return Cyc_Tcutil_fields_support_default( ud->tvs, _temp3791,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ud->fields))->v);}
_LL3736: _temp3797= _temp3795; goto _LL3738; _LL3738: return Cyc_Tcutil_fields_support_default(
0, 0, _temp3797); _LL3740: return 1; _LL3742: return 0; _LL3706:;} static int
Cyc_Tcutil_fields_support_default( struct Cyc_List_List* tvs, struct Cyc_List_List*
ts, struct Cyc_List_List* fs){{ struct _RegionHandle _temp3825= _new_region();
struct _RegionHandle* rgn=& _temp3825; _push_region( rgn);{ struct Cyc_List_List*
_temp3826=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, tvs, ts); goto _LL3827; _LL3827: for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ void* t= Cyc_Tcutil_rsubstitute( rgn, _temp3826,( void*)((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( fs))->hd)->type);
if( ! Cyc_Tcutil_supports_default( t)){ int _temp3828= 0; _npop_handler( 0u);
return _temp3828;}}}; _pop_region(& _temp3825);} return 1;}