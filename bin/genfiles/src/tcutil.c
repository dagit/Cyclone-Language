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
y); extern void Cyc_List_iter2( void(* f)( void*, void*), struct Cyc_List_List*
x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_revappend(
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
_tagged_string* name; struct Cyc_Absyn_Conref* kind; } ; static const int Cyc_Absyn_Unknown_b=
0; static const int Cyc_Absyn_Upper_b= 0; struct Cyc_Absyn_Upper_b_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_PtrInfo{ void* elt_typ; void*
rgn_typ; struct Cyc_Absyn_Conref* nullable; struct Cyc_Absyn_Tqual tq; struct
Cyc_Absyn_Conref* bounds; } ; struct Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt*
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
void* f1; struct Cyc_Core_Opt* f2; int f3; } ; static const int Cyc_Absyn_VarType=
1; struct Cyc_Absyn_VarType_struct{ int tag; struct Cyc_Absyn_Tvar* f1; } ;
static const int Cyc_Absyn_TunionType= 2; struct Cyc_Absyn_TunionType_struct{
int tag; struct Cyc_Absyn_TunionInfo f1; } ; static const int Cyc_Absyn_TunionFieldType=
3; struct Cyc_Absyn_TunionFieldType_struct{ int tag; struct Cyc_Absyn_TunionFieldInfo
f1; } ; static const int Cyc_Absyn_PointerType= 4; struct Cyc_Absyn_PointerType_struct{
int tag; struct Cyc_Absyn_PtrInfo f1; } ; static const int Cyc_Absyn_IntType= 5;
struct Cyc_Absyn_IntType_struct{ int tag; void* f1; void* f2; } ; static const
int Cyc_Absyn_FloatType= 1; static const int Cyc_Absyn_DoubleType= 2; static
const int Cyc_Absyn_ArrayType= 6; struct Cyc_Absyn_ArrayType_struct{ int tag;
void* f1; struct Cyc_Absyn_Tqual f2; struct Cyc_Absyn_Exp* f3; } ; static const
int Cyc_Absyn_FnType= 7; struct Cyc_Absyn_FnType_struct{ int tag; struct Cyc_Absyn_FnInfo
f1; } ; static const int Cyc_Absyn_TupleType= 8; struct Cyc_Absyn_TupleType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_StructType= 9;
struct Cyc_Absyn_StructType_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Structdecl** f3; } ; static const int Cyc_Absyn_UnionType=
10; struct Cyc_Absyn_UnionType_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Uniondecl** f3; } ; static const int Cyc_Absyn_AnonStructType=
11; struct Cyc_Absyn_AnonStructType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_AnonUnionType= 12; struct Cyc_Absyn_AnonUnionType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_EnumType= 13;
struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl*
f2; } ; static const int Cyc_Absyn_RgnHandleType= 14; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType= 15; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
} ; static const int Cyc_Absyn_HeapRgn= 3; static const int Cyc_Absyn_AccessEff=
16; struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_JoinEff= 17; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_NoTypes= 0; struct Cyc_Absyn_NoTypes_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; static
const int Cyc_Absyn_WithTypes= 1; struct Cyc_Absyn_WithTypes_struct{ int tag;
struct Cyc_List_List* f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt*
f4; } ; static const int Cyc_Absyn_NonNullable_ps= 0; struct Cyc_Absyn_NonNullable_ps_struct{
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
void*); extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulong_t; extern
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
extern struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set; extern void* Cyc_Tcutil_typ_kind(
void* t); extern void* Cyc_Tcutil_compress( void* t); extern void Cyc_Tcutil_unchecked_cast(
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
void* f2; } ; extern struct _tuple5* Cyc_Tcutil_make_inst_var( struct Cyc_Absyn_Tvar*);
extern struct _tuple5* Cyc_Tcutil_r_make_inst_var( struct _RegionHandle*, struct
Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp*);
extern struct Cyc_List_List* Cyc_Tcutil_check_valid_type( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*, void* k, void*); extern void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, void*); extern void Cyc_Tcutil_check_fndecl_valid_type(
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
struct Cyc_Absyn_Exp* e); struct _tuple6{ int f1; void* f2; } ; extern struct
_tuple6 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e); extern struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar( void* k); extern int Cyc_Tcutil_is_temp_tvar(
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
0; void* Cyc_Tcutil_compress( void* t){ void* _temp8= t; int _temp20; struct Cyc_Core_Opt*
_temp22; void* _temp24; int _temp26; struct Cyc_Core_Opt* _temp28; struct Cyc_Core_Opt**
_temp30; void* _temp31; struct Cyc_Core_Opt* _temp33; struct Cyc_List_List*
_temp35; struct _tuple1* _temp37; struct Cyc_Core_Opt* _temp39; struct Cyc_Core_Opt**
_temp41; struct Cyc_List_List* _temp42; struct _tuple1* _temp44; _LL10: if((
unsigned int) _temp8 > 4u?*(( int*) _temp8) == Cyc_Absyn_Evar: 0){ _LL25:
_temp24=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp8)->f1; goto _LL23; _LL23:
_temp22=(( struct Cyc_Absyn_Evar_struct*) _temp8)->f2; if( _temp22 == 0){ goto
_LL21;} else{ goto _LL12;} _LL21: _temp20=(( struct Cyc_Absyn_Evar_struct*)
_temp8)->f3; goto _LL11;} else{ goto _LL12;} _LL12: if(( unsigned int) _temp8 >
4u?*(( int*) _temp8) == Cyc_Absyn_Evar: 0){ _LL32: _temp31=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp8)->f1; goto _LL29; _LL29: _temp28=(( struct Cyc_Absyn_Evar_struct*) _temp8)->f2;
_temp30=&(( struct Cyc_Absyn_Evar_struct*) _temp8)->f2; goto _LL27; _LL27:
_temp26=(( struct Cyc_Absyn_Evar_struct*) _temp8)->f3; goto _LL13;} else{ goto
_LL14;} _LL14: if(( unsigned int) _temp8 > 4u?*(( int*) _temp8) == Cyc_Absyn_TypedefType:
0){ _LL38: _temp37=(( struct Cyc_Absyn_TypedefType_struct*) _temp8)->f1; goto
_LL36; _LL36: _temp35=(( struct Cyc_Absyn_TypedefType_struct*) _temp8)->f2; goto
_LL34; _LL34: _temp33=(( struct Cyc_Absyn_TypedefType_struct*) _temp8)->f3; if(
_temp33 == 0){ goto _LL15;} else{ goto _LL16;}} else{ goto _LL16;} _LL16: if((
unsigned int) _temp8 > 4u?*(( int*) _temp8) == Cyc_Absyn_TypedefType: 0){ _LL45:
_temp44=(( struct Cyc_Absyn_TypedefType_struct*) _temp8)->f1; goto _LL43; _LL43:
_temp42=(( struct Cyc_Absyn_TypedefType_struct*) _temp8)->f2; goto _LL40; _LL40:
_temp39=(( struct Cyc_Absyn_TypedefType_struct*) _temp8)->f3; _temp41=&(( struct
Cyc_Absyn_TypedefType_struct*) _temp8)->f3; goto _LL17;} else{ goto _LL18;}
_LL18: goto _LL19; _LL11: return t; _LL13: { void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null(* _temp30))->v); if( t2 !=( void*)((
struct Cyc_Core_Opt*) _check_null(* _temp30))->v){* _temp30=({ struct Cyc_Core_Opt*
_temp46=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp46->v=( void*) t2; _temp46;});} return t2;} _LL15: return t; _LL17: { void*
t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(* _temp41))->v);
if( t2 !=( void*)(( struct Cyc_Core_Opt*) _check_null(* _temp41))->v){* _temp41=({
struct Cyc_Core_Opt* _temp47=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp47->v=( void*) t2; _temp47;});} return t2;} _LL19: return t;
_LL9:;} struct _tuple7{ void* f1; void* f2; } ; static int Cyc_Tcutil_kind_leq(
void* k1, void* k2){ if( k1 == k2){ return 1;}{ struct _tuple7 _temp49=({ struct
_tuple7 _temp48; _temp48.f1= k1; _temp48.f2= k2; _temp48;}); void* _temp59; void*
_temp61; void* _temp63; void* _temp65; void* _temp67; void* _temp69; _LL51:
_LL62: _temp61= _temp49.f1; if( _temp61 ==( void*) Cyc_Absyn_BoxKind){ goto
_LL60;} else{ goto _LL53;} _LL60: _temp59= _temp49.f2; if( _temp59 ==( void*)
Cyc_Absyn_MemKind){ goto _LL52;} else{ goto _LL53;} _LL53: _LL66: _temp65=
_temp49.f1; if( _temp65 ==( void*) Cyc_Absyn_BoxKind){ goto _LL64;} else{ goto
_LL55;} _LL64: _temp63= _temp49.f2; if( _temp63 ==( void*) Cyc_Absyn_AnyKind){
goto _LL54;} else{ goto _LL55;} _LL55: _LL70: _temp69= _temp49.f1; if( _temp69
==( void*) Cyc_Absyn_MemKind){ goto _LL68;} else{ goto _LL57;} _LL68: _temp67=
_temp49.f2; if( _temp67 ==( void*) Cyc_Absyn_AnyKind){ goto _LL56;} else{ goto
_LL57;} _LL57: goto _LL58; _LL52: return 1; _LL54: return 1; _LL56: return 1;
_LL58: return 0; _LL50:;}} void* Cyc_Tcutil_typ_kind( void* t){ void* _temp71=
Cyc_Tcutil_compress( t); int _temp123; struct Cyc_Core_Opt* _temp125; void*
_temp127; struct Cyc_Absyn_Tvar* _temp129; void* _temp131; void* _temp133;
struct Cyc_Absyn_FnInfo _temp135; void* _temp137; struct Cyc_Absyn_TunionInfo
_temp139; struct Cyc_Absyn_TunionFieldInfo _temp141; struct Cyc_List_List*
_temp143; void* _temp145; struct Cyc_Absyn_Tunionfield* _temp147; struct Cyc_Absyn_Tuniondecl*
_temp149; struct Cyc_Absyn_TunionFieldInfo _temp151; struct Cyc_List_List*
_temp153; void* _temp155; struct Cyc_Absyn_UnknownTunionFieldInfo _temp157;
struct Cyc_Absyn_Structdecl** _temp159; struct Cyc_List_List* _temp161; struct
_tuple1* _temp163; struct Cyc_Absyn_Uniondecl** _temp165; struct Cyc_List_List*
_temp167; struct _tuple1* _temp169; struct Cyc_Absyn_Structdecl** _temp171;
struct Cyc_List_List* _temp173; struct _tuple1* _temp175; struct Cyc_Absyn_Uniondecl**
_temp177; struct Cyc_List_List* _temp179; struct _tuple1* _temp181; struct Cyc_List_List*
_temp183; struct Cyc_List_List* _temp185; struct Cyc_Absyn_Enumdecl* _temp187;
struct _tuple1* _temp189; struct Cyc_Absyn_PtrInfo _temp191; struct Cyc_Absyn_Exp*
_temp193; struct Cyc_Absyn_Tqual _temp195; void* _temp197; struct Cyc_List_List*
_temp199; struct Cyc_Core_Opt* _temp201; struct Cyc_List_List* _temp203; struct
_tuple1* _temp205; void* _temp207; struct Cyc_List_List* _temp209; _LL73: if((
unsigned int) _temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_Evar: 0){ _LL128:
_temp127=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp71)->f1; goto _LL126;
_LL126: _temp125=(( struct Cyc_Absyn_Evar_struct*) _temp71)->f2; goto _LL124;
_LL124: _temp123=(( struct Cyc_Absyn_Evar_struct*) _temp71)->f3; goto _LL74;}
else{ goto _LL75;} _LL75: if(( unsigned int) _temp71 > 4u?*(( int*) _temp71) ==
Cyc_Absyn_VarType: 0){ _LL130: _temp129=(( struct Cyc_Absyn_VarType_struct*)
_temp71)->f1; goto _LL76;} else{ goto _LL77;} _LL77: if( _temp71 ==( void*) Cyc_Absyn_VoidType){
goto _LL78;} else{ goto _LL79;} _LL79: if(( unsigned int) _temp71 > 4u?*(( int*)
_temp71) == Cyc_Absyn_IntType: 0){ _LL134: _temp133=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp71)->f1; goto _LL132; _LL132: _temp131=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp71)->f2; goto _LL80;} else{ goto _LL81;} _LL81: if( _temp71 ==( void*) Cyc_Absyn_FloatType){
goto _LL82;} else{ goto _LL83;} _LL83: if( _temp71 ==( void*) Cyc_Absyn_DoubleType){
goto _LL84;} else{ goto _LL85;} _LL85: if(( unsigned int) _temp71 > 4u?*(( int*)
_temp71) == Cyc_Absyn_FnType: 0){ _LL136: _temp135=(( struct Cyc_Absyn_FnType_struct*)
_temp71)->f1; goto _LL86;} else{ goto _LL87;} _LL87: if(( unsigned int) _temp71
> 4u?*(( int*) _temp71) == Cyc_Absyn_RgnHandleType: 0){ _LL138: _temp137=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp71)->f1; goto _LL88;} else{ goto
_LL89;} _LL89: if( _temp71 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL90;} else{
goto _LL91;} _LL91: if(( unsigned int) _temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_TunionType:
0){ _LL140: _temp139=(( struct Cyc_Absyn_TunionType_struct*) _temp71)->f1; goto
_LL92;} else{ goto _LL93;} _LL93: if(( unsigned int) _temp71 > 4u?*(( int*)
_temp71) == Cyc_Absyn_TunionFieldType: 0){ _LL142: _temp141=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp71)->f1; _LL146: _temp145=( void*) _temp141.field_info; if(*(( int*)
_temp145) == Cyc_Absyn_KnownTunionfield){ _LL150: _temp149=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp145)->f1; goto _LL148; _LL148: _temp147=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp145)->f2; goto _LL144;} else{ goto _LL95;} _LL144: _temp143= _temp141.targs;
goto _LL94;} else{ goto _LL95;} _LL95: if(( unsigned int) _temp71 > 4u?*(( int*)
_temp71) == Cyc_Absyn_TunionFieldType: 0){ _LL152: _temp151=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp71)->f1; _LL156: _temp155=( void*) _temp151.field_info; if(*(( int*)
_temp155) == Cyc_Absyn_UnknownTunionfield){ _LL158: _temp157=(( struct Cyc_Absyn_UnknownTunionfield_struct*)
_temp155)->f1; goto _LL154;} else{ goto _LL97;} _LL154: _temp153= _temp151.targs;
goto _LL96;} else{ goto _LL97;} _LL97: if(( unsigned int) _temp71 > 4u?*(( int*)
_temp71) == Cyc_Absyn_StructType: 0){ _LL164: _temp163=(( struct Cyc_Absyn_StructType_struct*)
_temp71)->f1; goto _LL162; _LL162: _temp161=(( struct Cyc_Absyn_StructType_struct*)
_temp71)->f2; goto _LL160; _LL160: _temp159=(( struct Cyc_Absyn_StructType_struct*)
_temp71)->f3; if( _temp159 == 0){ goto _LL98;} else{ goto _LL99;}} else{ goto
_LL99;} _LL99: if(( unsigned int) _temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_UnionType:
0){ _LL170: _temp169=(( struct Cyc_Absyn_UnionType_struct*) _temp71)->f1; goto
_LL168; _LL168: _temp167=(( struct Cyc_Absyn_UnionType_struct*) _temp71)->f2;
goto _LL166; _LL166: _temp165=(( struct Cyc_Absyn_UnionType_struct*) _temp71)->f3;
if( _temp165 == 0){ goto _LL100;} else{ goto _LL101;}} else{ goto _LL101;}
_LL101: if(( unsigned int) _temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_StructType:
0){ _LL176: _temp175=(( struct Cyc_Absyn_StructType_struct*) _temp71)->f1; goto
_LL174; _LL174: _temp173=(( struct Cyc_Absyn_StructType_struct*) _temp71)->f2;
goto _LL172; _LL172: _temp171=(( struct Cyc_Absyn_StructType_struct*) _temp71)->f3;
goto _LL102;} else{ goto _LL103;} _LL103: if(( unsigned int) _temp71 > 4u?*((
int*) _temp71) == Cyc_Absyn_UnionType: 0){ _LL182: _temp181=(( struct Cyc_Absyn_UnionType_struct*)
_temp71)->f1; goto _LL180; _LL180: _temp179=(( struct Cyc_Absyn_UnionType_struct*)
_temp71)->f2; goto _LL178; _LL178: _temp177=(( struct Cyc_Absyn_UnionType_struct*)
_temp71)->f3; goto _LL104;} else{ goto _LL105;} _LL105: if(( unsigned int)
_temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_AnonStructType: 0){ _LL184:
_temp183=(( struct Cyc_Absyn_AnonStructType_struct*) _temp71)->f1; goto _LL106;}
else{ goto _LL107;} _LL107: if(( unsigned int) _temp71 > 4u?*(( int*) _temp71)
== Cyc_Absyn_AnonUnionType: 0){ _LL186: _temp185=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp71)->f1; goto _LL108;} else{ goto _LL109;} _LL109: if(( unsigned int)
_temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_EnumType: 0){ _LL190: _temp189=((
struct Cyc_Absyn_EnumType_struct*) _temp71)->f1; goto _LL188; _LL188: _temp187=((
struct Cyc_Absyn_EnumType_struct*) _temp71)->f2; goto _LL110;} else{ goto _LL111;}
_LL111: if(( unsigned int) _temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_PointerType:
0){ _LL192: _temp191=(( struct Cyc_Absyn_PointerType_struct*) _temp71)->f1; goto
_LL112;} else{ goto _LL113;} _LL113: if(( unsigned int) _temp71 > 4u?*(( int*)
_temp71) == Cyc_Absyn_ArrayType: 0){ _LL198: _temp197=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp71)->f1; goto _LL196; _LL196: _temp195=(( struct Cyc_Absyn_ArrayType_struct*)
_temp71)->f2; goto _LL194; _LL194: _temp193=(( struct Cyc_Absyn_ArrayType_struct*)
_temp71)->f3; goto _LL114;} else{ goto _LL115;} _LL115: if(( unsigned int)
_temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_TupleType: 0){ _LL200: _temp199=((
struct Cyc_Absyn_TupleType_struct*) _temp71)->f1; goto _LL116;} else{ goto
_LL117;} _LL117: if(( unsigned int) _temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_TypedefType:
0){ _LL206: _temp205=(( struct Cyc_Absyn_TypedefType_struct*) _temp71)->f1; goto
_LL204; _LL204: _temp203=(( struct Cyc_Absyn_TypedefType_struct*) _temp71)->f2;
goto _LL202; _LL202: _temp201=(( struct Cyc_Absyn_TypedefType_struct*) _temp71)->f3;
goto _LL118;} else{ goto _LL119;} _LL119: if(( unsigned int) _temp71 > 4u?*((
int*) _temp71) == Cyc_Absyn_AccessEff: 0){ _LL208: _temp207=( void*)(( struct
Cyc_Absyn_AccessEff_struct*) _temp71)->f1; goto _LL120;} else{ goto _LL121;}
_LL121: if(( unsigned int) _temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_JoinEff:
0){ _LL210: _temp209=(( struct Cyc_Absyn_JoinEff_struct*) _temp71)->f1; goto
_LL122;} else{ goto _LL72;} _LL74: return _temp127; _LL76: return(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp129->kind); _LL78:
return( void*) Cyc_Absyn_MemKind; _LL80: return _temp131 ==( void*) Cyc_Absyn_B4?(
void*) Cyc_Absyn_BoxKind:( void*) Cyc_Absyn_MemKind; _LL82: return( void*) Cyc_Absyn_MemKind;
_LL84: return( void*) Cyc_Absyn_MemKind; _LL86: return( void*) Cyc_Absyn_MemKind;
_LL88: return( void*) Cyc_Absyn_BoxKind; _LL90: return( void*) Cyc_Absyn_RgnKind;
_LL92: return( void*) Cyc_Absyn_BoxKind; _LL94: if( _temp147->typs == 0){ return(
void*) Cyc_Absyn_BoxKind;} else{ return( void*) Cyc_Absyn_MemKind;} _LL96:
return(( void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
unsigned char* _temp211="typ_kind: Unresolved TunionFieldType"; struct
_tagged_string _temp212; _temp212.curr= _temp211; _temp212.base= _temp211;
_temp212.last_plus_one= _temp211 + 37; _temp212;})); _LL98: return( void*) Cyc_Absyn_AnyKind;
_LL100: return( void*) Cyc_Absyn_AnyKind; _LL102: return( void*) Cyc_Absyn_MemKind;
_LL104: return( void*) Cyc_Absyn_MemKind; _LL106: return( void*) Cyc_Absyn_MemKind;
_LL108: return( void*) Cyc_Absyn_MemKind; _LL110: return( void*) Cyc_Absyn_BoxKind;
_LL112: { void* _temp213=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp191.bounds))->v; void* _temp223; void*
_temp225; struct Cyc_Absyn_Exp* _temp227; struct Cyc_Absyn_Conref* _temp229;
_LL215: if(( unsigned int) _temp213 > 1u?*(( int*) _temp213) == Cyc_Absyn_Eq_constr:
0){ _LL224: _temp223=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp213)->f1;
if( _temp223 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL216;} else{ goto _LL217;}}
else{ goto _LL217;} _LL217: if(( unsigned int) _temp213 > 1u?*(( int*) _temp213)
== Cyc_Absyn_Eq_constr: 0){ _LL226: _temp225=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp213)->f1; if(( unsigned int) _temp225 > 1u?*(( int*) _temp225) == Cyc_Absyn_Upper_b:
0){ _LL228: _temp227=(( struct Cyc_Absyn_Upper_b_struct*) _temp225)->f1; goto
_LL218;} else{ goto _LL219;}} else{ goto _LL219;} _LL219: if( _temp213 ==( void*)
Cyc_Absyn_No_constr){ goto _LL220;} else{ goto _LL221;} _LL221: if((
unsigned int) _temp213 > 1u?*(( int*) _temp213) == Cyc_Absyn_Forward_constr: 0){
_LL230: _temp229=(( struct Cyc_Absyn_Forward_constr_struct*) _temp213)->f1; goto
_LL222;} else{ goto _LL214;} _LL216: return( void*) Cyc_Absyn_MemKind; _LL218:
return( void*) Cyc_Absyn_BoxKind; _LL220: return( void*) Cyc_Absyn_MemKind;
_LL222: return(( void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
unsigned char* _temp231="typ_kind: forward constraint in ptr bounds"; struct
_tagged_string _temp232; _temp232.curr= _temp231; _temp232.base= _temp231;
_temp232.last_plus_one= _temp231 + 43; _temp232;})); _LL214:;} _LL114: return(
void*) Cyc_Absyn_MemKind; _LL116: return( void*) Cyc_Absyn_MemKind; _LL118:
return(( void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({ struct
_tagged_string _temp233= Cyc_Absynpp_typ2string( t); xprintf("typ_kind: typedef found: %.*s",
_temp233.last_plus_one - _temp233.curr, _temp233.curr);})); _LL120: return( void*)
Cyc_Absyn_EffKind; _LL122: return( void*) Cyc_Absyn_EffKind; _LL72:;}
unsigned char Cyc_Tcutil_Unify[ 10u]="\000\000\000\000Unify"; int Cyc_Tcutil_unify(
void* t1, void* t2){ struct _handler_cons _temp234; _push_handler(& _temp234);{
int _temp236= 0; if( setjmp( _temp234.handler)){ _temp236= 1;} if( ! _temp236){
Cyc_Tcutil_unify_it( t1, t2);{ int _temp237= 1; _npop_handler( 0u); return
_temp237;}; _pop_handler();} else{ void* _temp235=( void*) _exn_thrown; void*
_temp239= _temp235; _LL241: if( _temp239 == Cyc_Tcutil_Unify){ goto _LL242;}
else{ goto _LL243;} _LL243: goto _LL244; _LL242: return 0; _LL244:( void) _throw(
_temp239); _LL240:;}}} static void Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List*
ts); static void Cyc_Tcutil_occurs( void* evar, void* t){ void* _temp245= Cyc_Tcutil_compress(
t); int _temp277; struct Cyc_Core_Opt* _temp279; void* _temp281; struct Cyc_Absyn_PtrInfo
_temp283; struct Cyc_Absyn_Exp* _temp285; struct Cyc_Absyn_Tqual _temp287; void*
_temp289; struct Cyc_Absyn_FnInfo _temp291; struct Cyc_List_List* _temp293;
struct Cyc_Absyn_VarargInfo* _temp295; int _temp297; struct Cyc_List_List*
_temp299; void* _temp301; struct Cyc_Core_Opt* _temp303; struct Cyc_List_List*
_temp305; struct Cyc_List_List* _temp307; void* _temp309; struct Cyc_Absyn_TunionInfo
_temp311; void* _temp313; struct Cyc_List_List* _temp315; void* _temp317; struct
Cyc_Core_Opt* _temp319; struct Cyc_List_List* _temp321; struct _tuple1* _temp323;
struct Cyc_Absyn_TunionFieldInfo _temp325; struct Cyc_List_List* _temp327; void*
_temp329; struct Cyc_Absyn_Structdecl** _temp331; struct Cyc_List_List* _temp333;
struct _tuple1* _temp335; struct Cyc_List_List* _temp337; struct Cyc_List_List*
_temp339; void* _temp341; struct Cyc_List_List* _temp343; _LL247: if((
unsigned int) _temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_Evar: 0){ _LL282:
_temp281=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp245)->f1; goto _LL280;
_LL280: _temp279=(( struct Cyc_Absyn_Evar_struct*) _temp245)->f2; goto _LL278;
_LL278: _temp277=(( struct Cyc_Absyn_Evar_struct*) _temp245)->f3; goto _LL248;}
else{ goto _LL249;} _LL249: if(( unsigned int) _temp245 > 4u?*(( int*) _temp245)
== Cyc_Absyn_PointerType: 0){ _LL284: _temp283=(( struct Cyc_Absyn_PointerType_struct*)
_temp245)->f1; goto _LL250;} else{ goto _LL251;} _LL251: if(( unsigned int)
_temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_ArrayType: 0){ _LL290: _temp289=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp245)->f1; goto _LL288; _LL288:
_temp287=(( struct Cyc_Absyn_ArrayType_struct*) _temp245)->f2; goto _LL286;
_LL286: _temp285=(( struct Cyc_Absyn_ArrayType_struct*) _temp245)->f3; goto
_LL252;} else{ goto _LL253;} _LL253: if(( unsigned int) _temp245 > 4u?*(( int*)
_temp245) == Cyc_Absyn_FnType: 0){ _LL292: _temp291=(( struct Cyc_Absyn_FnType_struct*)
_temp245)->f1; _LL306: _temp305= _temp291.tvars; goto _LL304; _LL304: _temp303=
_temp291.effect; goto _LL302; _LL302: _temp301=( void*) _temp291.ret_typ; goto
_LL300; _LL300: _temp299= _temp291.args; goto _LL298; _LL298: _temp297= _temp291.c_varargs;
goto _LL296; _LL296: _temp295= _temp291.cyc_varargs; goto _LL294; _LL294:
_temp293= _temp291.attributes; goto _LL254;} else{ goto _LL255;} _LL255: if((
unsigned int) _temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_TupleType: 0){
_LL308: _temp307=(( struct Cyc_Absyn_TupleType_struct*) _temp245)->f1; goto
_LL256;} else{ goto _LL257;} _LL257: if(( unsigned int) _temp245 > 4u?*(( int*)
_temp245) == Cyc_Absyn_RgnHandleType: 0){ _LL310: _temp309=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp245)->f1; goto _LL258;} else{ goto _LL259;} _LL259: if(( unsigned int)
_temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_TunionType: 0){ _LL312: _temp311=((
struct Cyc_Absyn_TunionType_struct*) _temp245)->f1; _LL318: _temp317=( void*)
_temp311.tunion_info; goto _LL316; _LL316: _temp315= _temp311.targs; goto _LL314;
_LL314: _temp313=( void*) _temp311.rgn; goto _LL260;} else{ goto _LL261;} _LL261:
if(( unsigned int) _temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_TypedefType: 0){
_LL324: _temp323=(( struct Cyc_Absyn_TypedefType_struct*) _temp245)->f1; goto
_LL322; _LL322: _temp321=(( struct Cyc_Absyn_TypedefType_struct*) _temp245)->f2;
goto _LL320; _LL320: _temp319=(( struct Cyc_Absyn_TypedefType_struct*) _temp245)->f3;
goto _LL262;} else{ goto _LL263;} _LL263: if(( unsigned int) _temp245 > 4u?*((
int*) _temp245) == Cyc_Absyn_TunionFieldType: 0){ _LL326: _temp325=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp245)->f1; _LL330: _temp329=( void*) _temp325.field_info; goto _LL328;
_LL328: _temp327= _temp325.targs; goto _LL264;} else{ goto _LL265;} _LL265: if((
unsigned int) _temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_StructType: 0){
_LL336: _temp335=(( struct Cyc_Absyn_StructType_struct*) _temp245)->f1; goto
_LL334; _LL334: _temp333=(( struct Cyc_Absyn_StructType_struct*) _temp245)->f2;
goto _LL332; _LL332: _temp331=(( struct Cyc_Absyn_StructType_struct*) _temp245)->f3;
goto _LL266;} else{ goto _LL267;} _LL267: if(( unsigned int) _temp245 > 4u?*((
int*) _temp245) == Cyc_Absyn_AnonStructType: 0){ _LL338: _temp337=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp245)->f1; goto _LL268;} else{ goto _LL269;} _LL269: if(( unsigned int)
_temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_AnonUnionType: 0){ _LL340:
_temp339=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp245)->f1; goto _LL270;}
else{ goto _LL271;} _LL271: if(( unsigned int) _temp245 > 4u?*(( int*) _temp245)
== Cyc_Absyn_AccessEff: 0){ _LL342: _temp341=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp245)->f1; goto _LL272;} else{ goto _LL273;} _LL273: if(( unsigned int)
_temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_JoinEff: 0){ _LL344: _temp343=((
struct Cyc_Absyn_JoinEff_struct*) _temp245)->f1; goto _LL274;} else{ goto _LL275;}
_LL275: goto _LL276; _LL248: if( t == evar){( void) _throw(( void*) Cyc_Tcutil_Unify);}
else{ if( _temp279 != 0){ Cyc_Tcutil_occurs( evar,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp279))->v);}} goto _LL246; _LL250: Cyc_Tcutil_occurs( evar,(
void*) _temp283.elt_typ); Cyc_Tcutil_occurs( evar,( void*) _temp283.rgn_typ);
goto _LL246; _LL252: Cyc_Tcutil_occurs( evar, _temp289); goto _LL246; _LL254:
if( _temp303 != 0){ Cyc_Tcutil_occurs( evar,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp303))->v);} Cyc_Tcutil_occurs( evar, _temp301); for( 0;
_temp299 != 0; _temp299=(( struct Cyc_List_List*) _check_null( _temp299))->tl){
Cyc_Tcutil_occurs( evar,(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp299))->hd)).f3);} if( _temp295 != 0){ int _temp347; void*
_temp349; void* _temp351; struct Cyc_Absyn_Tqual _temp353; struct Cyc_Core_Opt*
_temp355; struct Cyc_Absyn_VarargInfo _temp345=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp295)); _LL356: _temp355= _temp345.name; goto _LL354; _LL354:
_temp353= _temp345.tq; goto _LL352; _LL352: _temp351=( void*) _temp345.type;
goto _LL350; _LL350: _temp349=( void*) _temp345.rgn; goto _LL348; _LL348:
_temp347= _temp345.inject; goto _LL346; _LL346: Cyc_Tcutil_occurs( evar,
_temp351); Cyc_Tcutil_occurs( evar, _temp349);} goto _LL246; _LL256: for( 0;
_temp307 != 0; _temp307=(( struct Cyc_List_List*) _check_null( _temp307))->tl){
Cyc_Tcutil_occurs( evar,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp307))->hd)).f2);} goto _LL246; _LL258: Cyc_Tcutil_occurs( evar,
_temp309); goto _LL246; _LL260: Cyc_Tcutil_occurs( evar, _temp313); Cyc_Tcutil_occurslist(
evar, _temp315); goto _LL246; _LL262: _temp327= _temp321; goto _LL264; _LL264:
_temp333= _temp327; goto _LL266; _LL266: Cyc_Tcutil_occurslist( evar, _temp333);
goto _LL246; _LL268: _temp339= _temp337; goto _LL270; _LL270: for( 0; _temp339
!= 0; _temp339=(( struct Cyc_List_List*) _check_null( _temp339))->tl){ Cyc_Tcutil_occurs(
evar,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp339))->hd)->type);} goto _LL246; _LL272: Cyc_Tcutil_occurs(
evar, _temp341); goto _LL246; _LL274: Cyc_Tcutil_occurslist( evar, _temp343);
goto _LL246; _LL276: goto _LL246; _LL246:;} static void Cyc_Tcutil_occurslist(
void* evar, struct Cyc_List_List* ts){ for( 0; ts != 0; ts=(( struct Cyc_List_List*)
_check_null( ts))->tl){ Cyc_Tcutil_occurs( evar,( void*)(( struct Cyc_List_List*)
_check_null( ts))->hd);}} static void Cyc_Tcutil_unify_list( struct Cyc_List_List*
t1, struct Cyc_List_List* t2){ struct _handler_cons _temp357; _push_handler(&
_temp357);{ int _temp359= 0; if( setjmp( _temp357.handler)){ _temp359= 1;} if( !
_temp359){(( void(*)( void(* f)( void*, void*), struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_iter2)( Cyc_Tcutil_unify_it, t1, t2);; _pop_handler();}
else{ void* _temp358=( void*) _exn_thrown; void* _temp361= _temp358; _LL363: if(
_temp361 == Cyc_List_List_mismatch){ goto _LL364;} else{ goto _LL365;} _LL365:
goto _LL366; _LL364:( void) _throw(( void*) Cyc_Tcutil_Unify); _LL366:( void)
_throw( _temp361); _LL362:;}}} static void Cyc_Tcutil_unify_tqual( struct Cyc_Absyn_Tqual
tq1, struct Cyc_Absyn_Tqual tq2){ if(( tq1.q_const != tq2.q_const? 1: tq1.q_volatile
!= tq2.q_volatile)? 1: tq1.q_restrict != tq2.q_restrict){( void) _throw(( void*)
Cyc_Tcutil_Unify);}} int Cyc_Tcutil_equal_tqual( struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2){ return( tq1.q_const == tq2.q_const? tq1.q_volatile
== tq2.q_volatile: 0)? tq1.q_restrict == tq2.q_restrict: 0;} static void Cyc_Tcutil_unify_it_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y){ x=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
x); y=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
y); if( x == y){ return;}{ void* _temp367=( void*) x->v; struct Cyc_Absyn_Conref*
_temp375; void* _temp377; _LL369: if( _temp367 ==( void*) Cyc_Absyn_No_constr){
goto _LL370;} else{ goto _LL371;} _LL371: if(( unsigned int) _temp367 > 1u?*((
int*) _temp367) == Cyc_Absyn_Forward_constr: 0){ _LL376: _temp375=(( struct Cyc_Absyn_Forward_constr_struct*)
_temp367)->f1; goto _LL372;} else{ goto _LL373;} _LL373: if(( unsigned int)
_temp367 > 1u?*(( int*) _temp367) == Cyc_Absyn_Eq_constr: 0){ _LL378: _temp377=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp367)->f1; goto _LL374;} else{
goto _LL368;} _LL370:( void*)( x->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp379=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp379[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp380; _temp380.tag= Cyc_Absyn_Forward_constr; _temp380.f1= y; _temp380;});
_temp379;}))); return; _LL372:( void) _throw((( void*(*)( struct _tagged_string
msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp381="unify_conref: forward after compress";
struct _tagged_string _temp382; _temp382.curr= _temp381; _temp382.base= _temp381;
_temp382.last_plus_one= _temp381 + 37; _temp382;}))); _LL374: { void* _temp383=(
void*) y->v; struct Cyc_Absyn_Conref* _temp391; void* _temp393; _LL385: if(
_temp383 ==( void*) Cyc_Absyn_No_constr){ goto _LL386;} else{ goto _LL387;}
_LL387: if(( unsigned int) _temp383 > 1u?*(( int*) _temp383) == Cyc_Absyn_Forward_constr:
0){ _LL392: _temp391=(( struct Cyc_Absyn_Forward_constr_struct*) _temp383)->f1;
goto _LL388;} else{ goto _LL389;} _LL389: if(( unsigned int) _temp383 > 1u?*((
int*) _temp383) == Cyc_Absyn_Eq_constr: 0){ _LL394: _temp393=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp383)->f1; goto _LL390;} else{ goto _LL384;}
_LL386:( void*)( y->v=( void*)(( void*) x->v)); return; _LL388:( void) _throw(((
void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char*
_temp395="unify_conref: forward after compress(2)"; struct _tagged_string
_temp396; _temp396.curr= _temp395; _temp396.base= _temp395; _temp396.last_plus_one=
_temp395 + 40; _temp396;}))); _LL390: if( cmp( _temp377, _temp393) != 0){( void)
_throw(( void*) Cyc_Tcutil_Unify);} return; _LL384:;} _LL368:;}} static int Cyc_Tcutil_unify_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y){ struct _handler_cons _temp397; _push_handler(& _temp397);{ int _temp399= 0;
if( setjmp( _temp397.handler)){ _temp399= 1;} if( ! _temp399){(( void(*)( int(*
cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y))
Cyc_Tcutil_unify_it_conrefs)( cmp, x, y);{ int _temp400= 1; _npop_handler( 0u);
return _temp400;}; _pop_handler();} else{ void* _temp398=( void*) _exn_thrown;
void* _temp402= _temp398; _LL404: if( _temp402 == Cyc_Tcutil_Unify){ goto _LL405;}
else{ goto _LL406;} _LL406: goto _LL407; _LL405: return 0; _LL407:( void) _throw(
_temp402); _LL403:;}}} static int Cyc_Tcutil_boundscmp( void* b1, void* b2){
struct _tuple7 _temp409=({ struct _tuple7 _temp408; _temp408.f1= b1; _temp408.f2=
b2; _temp408;}); void* _temp419; void* _temp421; void* _temp423; void* _temp425;
void* _temp427; void* _temp429; void* _temp431; struct Cyc_Absyn_Exp* _temp433;
void* _temp435; struct Cyc_Absyn_Exp* _temp437; _LL411: _LL422: _temp421=
_temp409.f1; if( _temp421 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL420;} else{
goto _LL413;} _LL420: _temp419= _temp409.f2; if( _temp419 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL412;} else{ goto _LL413;} _LL413: _LL426: _temp425= _temp409.f1; if(
_temp425 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL424;} else{ goto _LL415;}
_LL424: _temp423= _temp409.f2; goto _LL414; _LL415: _LL430: _temp429= _temp409.f1;
goto _LL428; _LL428: _temp427= _temp409.f2; if( _temp427 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL416;} else{ goto _LL417;} _LL417: _LL436: _temp435= _temp409.f1; if((
unsigned int) _temp435 > 1u?*(( int*) _temp435) == Cyc_Absyn_Upper_b: 0){ _LL438:
_temp437=(( struct Cyc_Absyn_Upper_b_struct*) _temp435)->f1; goto _LL432;} else{
goto _LL410;} _LL432: _temp431= _temp409.f2; if(( unsigned int) _temp431 > 1u?*((
int*) _temp431) == Cyc_Absyn_Upper_b: 0){ _LL434: _temp433=(( struct Cyc_Absyn_Upper_b_struct*)
_temp431)->f1; goto _LL418;} else{ goto _LL410;} _LL412: return 0; _LL414:
return - 1; _LL416: return 1; _LL418: { int i1=( int) Cyc_Evexp_eval_const_uint_exp(
_temp437); int i2=( int) Cyc_Evexp_eval_const_uint_exp( _temp433); if( i1 == i2){
return 0;} if( i1 < i2){ return - 1;} return 1;} _LL410:;} static int Cyc_Tcutil_equal_att(
void* a1, void* a2){ if( a1 == a2){ return 1;}{ struct _tuple7 _temp440=({
struct _tuple7 _temp439; _temp439.f1= a1; _temp439.f2= a2; _temp439;}); void*
_temp450; int _temp452; void* _temp454; int _temp456; void* _temp458; int
_temp460; void* _temp462; int _temp464; void* _temp466; struct _tagged_string
_temp468; void* _temp470; struct _tagged_string _temp472; _LL442: _LL455:
_temp454= _temp440.f1; if(( unsigned int) _temp454 > 16u?*(( int*) _temp454) ==
Cyc_Absyn_Regparm_att: 0){ _LL457: _temp456=(( struct Cyc_Absyn_Regparm_att_struct*)
_temp454)->f1; goto _LL451;} else{ goto _LL444;} _LL451: _temp450= _temp440.f2;
if(( unsigned int) _temp450 > 16u?*(( int*) _temp450) == Cyc_Absyn_Regparm_att:
0){ _LL453: _temp452=(( struct Cyc_Absyn_Regparm_att_struct*) _temp450)->f1;
goto _LL443;} else{ goto _LL444;} _LL444: _LL463: _temp462= _temp440.f1; if((
unsigned int) _temp462 > 16u?*(( int*) _temp462) == Cyc_Absyn_Aligned_att: 0){
_LL465: _temp464=(( struct Cyc_Absyn_Aligned_att_struct*) _temp462)->f1; goto
_LL459;} else{ goto _LL446;} _LL459: _temp458= _temp440.f2; if(( unsigned int)
_temp458 > 16u?*(( int*) _temp458) == Cyc_Absyn_Aligned_att: 0){ _LL461:
_temp460=(( struct Cyc_Absyn_Aligned_att_struct*) _temp458)->f1; goto _LL445;}
else{ goto _LL446;} _LL446: _LL471: _temp470= _temp440.f1; if(( unsigned int)
_temp470 > 16u?*(( int*) _temp470) == Cyc_Absyn_Section_att: 0){ _LL473:
_temp472=(( struct Cyc_Absyn_Section_att_struct*) _temp470)->f1; goto _LL467;}
else{ goto _LL448;} _LL467: _temp466= _temp440.f2; if(( unsigned int) _temp466 >
16u?*(( int*) _temp466) == Cyc_Absyn_Section_att: 0){ _LL469: _temp468=(( struct
Cyc_Absyn_Section_att_struct*) _temp466)->f1; goto _LL447;} else{ goto _LL448;}
_LL448: goto _LL449; _LL443: _temp464= _temp456; _temp460= _temp452; goto _LL445;
_LL445: return _temp464 == _temp460; _LL447: return Cyc_String_strcmp( _temp472,
_temp468) == 0; _LL449: return 0; _LL441:;}} int Cyc_Tcutil_same_atts( struct
Cyc_List_List* a1, struct Cyc_List_List* a2){{ struct Cyc_List_List* a= a1; for(
0; a != 0; a=(( struct Cyc_List_List*) _check_null( a))->tl){ if( !(( int(*)(
int(* pred)( void*, void*), void* env, struct Cyc_List_List* x)) Cyc_List_exists_c)(
Cyc_Tcutil_equal_att,( void*)(( struct Cyc_List_List*) _check_null( a))->hd, a2)){
return 0;}}}{ struct Cyc_List_List* a= a2; for( 0; a != 0; a=(( struct Cyc_List_List*)
_check_null( a))->tl){ if( !(( int(*)( int(* pred)( void*, void*), void* env,
struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcutil_equal_att,( void*)((
struct Cyc_List_List*) _check_null( a))->hd, a1)){ return 0;}}} return 1;}
static void Cyc_Tcutil_normalize_effect( void* e){ e= Cyc_Tcutil_compress( e);{
void* _temp474= e; struct Cyc_List_List* _temp480; struct Cyc_List_List**
_temp482; _LL476: if(( unsigned int) _temp474 > 4u?*(( int*) _temp474) == Cyc_Absyn_JoinEff:
0){ _LL481: _temp480=(( struct Cyc_Absyn_JoinEff_struct*) _temp474)->f1;
_temp482=&(( struct Cyc_Absyn_JoinEff_struct*) _temp474)->f1; goto _LL477;}
else{ goto _LL478;} _LL478: goto _LL479; _LL477: { int nested_join= 0;{ struct
Cyc_List_List* effs=* _temp482; for( 0; effs != 0; effs=(( struct Cyc_List_List*)
_check_null( effs))->tl){ void* _temp483=( void*)(( struct Cyc_List_List*)
_check_null( effs))->hd; goto _LL484; _LL484: Cyc_Tcutil_normalize_effect(
_temp483);{ void* _temp485= Cyc_Tcutil_compress( _temp483); struct Cyc_List_List*
_temp491; _LL487: if(( unsigned int) _temp485 > 4u?*(( int*) _temp485) == Cyc_Absyn_JoinEff:
0){ _LL492: _temp491=(( struct Cyc_Absyn_JoinEff_struct*) _temp485)->f1; goto
_LL488;} else{ goto _LL489;} _LL489: goto _LL490; _LL488: nested_join= 1; goto
_LL486; _LL490: goto _LL486; _LL486:;}}} if( ! nested_join){ return;}{ struct
Cyc_List_List* effects= 0;{ struct Cyc_List_List* effs=* _temp482; for( 0; effs
!= 0; effs=(( struct Cyc_List_List*) _check_null( effs))->tl){ void* _temp493=
Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*) _check_null( effs))->hd);
struct Cyc_List_List* _temp499; _LL495: if(( unsigned int) _temp493 > 4u?*(( int*)
_temp493) == Cyc_Absyn_JoinEff: 0){ _LL500: _temp499=(( struct Cyc_Absyn_JoinEff_struct*)
_temp493)->f1; goto _LL496;} else{ goto _LL497;} _LL497: goto _LL498; _LL496:
effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_revappend)( _temp499, effects); goto _LL494; _LL498: effects=({
struct Cyc_List_List* _temp501=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp501->hd=( void*) _temp493; _temp501->tl= effects;
_temp501;}); goto _LL494; _LL494:;}}* _temp482=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( effects); goto _LL475;}} _LL479:
goto _LL475; _LL475:;}} struct _tuple8{ void* f1; struct Cyc_List_List* f2; } ;
static struct _tuple8* Cyc_Tcutil_get_effect_evar( void* t){ void* _temp502= Cyc_Tcutil_compress(
t); struct Cyc_List_List* _temp510; struct Cyc_List_List _temp512; struct Cyc_List_List*
_temp513; void* _temp515; int _temp517; struct Cyc_Core_Opt* _temp519; struct
Cyc_Core_Opt** _temp521; void* _temp522; void** _temp524; _LL504: if((
unsigned int) _temp502 > 4u?*(( int*) _temp502) == Cyc_Absyn_JoinEff: 0){ _LL511:
_temp510=(( struct Cyc_Absyn_JoinEff_struct*) _temp502)->f1; if( _temp510 == 0){
goto _LL506;} else{ _temp512=* _temp510; _LL516: _temp515=( void*) _temp512.hd;
goto _LL514; _LL514: _temp513= _temp512.tl; goto _LL505;}} else{ goto _LL506;}
_LL506: if(( unsigned int) _temp502 > 4u?*(( int*) _temp502) == Cyc_Absyn_Evar:
0){ _LL523: _temp522=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp502)->f1;
_temp524=&(( void*)(( struct Cyc_Absyn_Evar_struct*) _temp502)->f1); goto _LL520;
_LL520: _temp519=(( struct Cyc_Absyn_Evar_struct*) _temp502)->f2; _temp521=&((
struct Cyc_Absyn_Evar_struct*) _temp502)->f2; goto _LL518; _LL518: _temp517=((
struct Cyc_Absyn_Evar_struct*) _temp502)->f3; goto _LL507;} else{ goto _LL508;}
_LL508: goto _LL509; _LL505: { void* _temp525= Cyc_Tcutil_compress( _temp515);
int _temp531; struct Cyc_Core_Opt* _temp533; void* _temp535; _LL527: if((
unsigned int) _temp525 > 4u?*(( int*) _temp525) == Cyc_Absyn_Evar: 0){ _LL536:
_temp535=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp525)->f1; goto _LL534;
_LL534: _temp533=(( struct Cyc_Absyn_Evar_struct*) _temp525)->f2; goto _LL532;
_LL532: _temp531=(( struct Cyc_Absyn_Evar_struct*) _temp525)->f3; goto _LL528;}
else{ goto _LL529;} _LL529: goto _LL530; _LL528: return({ struct _tuple8*
_temp537=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp537->f1=
_temp515; _temp537->f2= _temp513; _temp537;}); _LL530: return 0; _LL526:;}
_LL507: if(* _temp524 !=( void*) Cyc_Absyn_EffKind){(( void(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp538="effect evar has wrong kind";
struct _tagged_string _temp539; _temp539.curr= _temp538; _temp539.base= _temp538;
_temp539.last_plus_one= _temp538 + 27; _temp539;}));} return({ struct _tuple8*
_temp540=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp540->f1= t;
_temp540->f2= 0; _temp540;}); _LL509: return 0; _LL503:;} int Cyc_Tcutil_region_in_effect(
int constrain, void* r, void* e){ r= Cyc_Tcutil_compress( r); if( r ==( void*)
Cyc_Absyn_HeapRgn){ return 1;}{ void* _temp541= Cyc_Tcutil_compress( e); void*
_temp551; struct Cyc_List_List* _temp553; int _temp555; struct Cyc_Core_Opt*
_temp557; struct Cyc_Core_Opt** _temp559; void* _temp560; void** _temp562;
_LL543: if(( unsigned int) _temp541 > 4u?*(( int*) _temp541) == Cyc_Absyn_AccessEff:
0){ _LL552: _temp551=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp541)->f1;
goto _LL544;} else{ goto _LL545;} _LL545: if(( unsigned int) _temp541 > 4u?*((
int*) _temp541) == Cyc_Absyn_JoinEff: 0){ _LL554: _temp553=(( struct Cyc_Absyn_JoinEff_struct*)
_temp541)->f1; goto _LL546;} else{ goto _LL547;} _LL547: if(( unsigned int)
_temp541 > 4u?*(( int*) _temp541) == Cyc_Absyn_Evar: 0){ _LL561: _temp560=( void*)((
struct Cyc_Absyn_Evar_struct*) _temp541)->f1; _temp562=&(( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp541)->f1); goto _LL558; _LL558: _temp557=(( struct Cyc_Absyn_Evar_struct*)
_temp541)->f2; _temp559=&(( struct Cyc_Absyn_Evar_struct*) _temp541)->f2; goto
_LL556; _LL556: _temp555=(( struct Cyc_Absyn_Evar_struct*) _temp541)->f3; goto
_LL548;} else{ goto _LL549;} _LL549: goto _LL550; _LL544: if( constrain){ return
Cyc_Tcutil_unify( r, _temp551);} Cyc_Tcutil_compress( _temp551); if( r ==
_temp551){ return 1;}{ struct _tuple7 _temp564=({ struct _tuple7 _temp563;
_temp563.f1= r; _temp563.f2= _temp551; _temp563;}); void* _temp570; struct Cyc_Absyn_Tvar*
_temp572; void* _temp574; struct Cyc_Absyn_Tvar* _temp576; _LL566: _LL575:
_temp574= _temp564.f1; if(( unsigned int) _temp574 > 4u?*(( int*) _temp574) ==
Cyc_Absyn_VarType: 0){ _LL577: _temp576=(( struct Cyc_Absyn_VarType_struct*)
_temp574)->f1; goto _LL571;} else{ goto _LL568;} _LL571: _temp570= _temp564.f2;
if(( unsigned int) _temp570 > 4u?*(( int*) _temp570) == Cyc_Absyn_VarType: 0){
_LL573: _temp572=(( struct Cyc_Absyn_VarType_struct*) _temp570)->f1; goto _LL567;}
else{ goto _LL568;} _LL568: goto _LL569; _LL567: return Cyc_Absyn_tvar_cmp(
_temp576, _temp572) == 0; _LL569: return 0; _LL565:;} _LL546: for( 0; _temp553
!= 0; _temp553=(( struct Cyc_List_List*) _check_null( _temp553))->tl){ if( Cyc_Tcutil_region_in_effect(
constrain, r,( void*)(( struct Cyc_List_List*) _check_null( _temp553))->hd)){
return 1;}} return 0; _LL548: if(* _temp562 !=( void*) Cyc_Absyn_EffKind){((
void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char*
_temp578="effect evar has wrong kind"; struct _tagged_string _temp579; _temp579.curr=
_temp578; _temp579.base= _temp578; _temp579.last_plus_one= _temp578 + 27;
_temp579;}));} if( ! constrain){ return 0;}{ void* _temp580= Cyc_Absyn_new_evar((
void*) Cyc_Absyn_EffKind); goto _LL581; _LL581: { struct Cyc_Absyn_JoinEff_struct*
_temp588=({ struct Cyc_Absyn_JoinEff_struct* _temp582=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp582[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp583; _temp583.tag= Cyc_Absyn_JoinEff; _temp583.f1=({ struct Cyc_List_List*
_temp584=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp584->hd=( void*) _temp580; _temp584->tl=({ struct Cyc_List_List* _temp585=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp585->hd=(
void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp586=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp586[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp587; _temp587.tag= Cyc_Absyn_AccessEff; _temp587.f1=(
void*) r; _temp587;}); _temp586;})); _temp585->tl= 0; _temp585;}); _temp584;});
_temp583;}); _temp582;}); goto _LL589; _LL589:* _temp559=({ struct Cyc_Core_Opt*
_temp590=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp590->v=( void*)(( void*) _temp588); _temp590;}); return 1;}} _LL550: return
0; _LL542:;}} static int Cyc_Tcutil_variable_in_effect( int constrain, struct
Cyc_Absyn_Tvar* v, void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp591= e;
struct Cyc_Absyn_Tvar* _temp601; struct Cyc_List_List* _temp603; int _temp605;
struct Cyc_Core_Opt* _temp607; struct Cyc_Core_Opt** _temp609; void* _temp610;
void** _temp612; _LL593: if(( unsigned int) _temp591 > 4u?*(( int*) _temp591) ==
Cyc_Absyn_VarType: 0){ _LL602: _temp601=(( struct Cyc_Absyn_VarType_struct*)
_temp591)->f1; goto _LL594;} else{ goto _LL595;} _LL595: if(( unsigned int)
_temp591 > 4u?*(( int*) _temp591) == Cyc_Absyn_JoinEff: 0){ _LL604: _temp603=((
struct Cyc_Absyn_JoinEff_struct*) _temp591)->f1; goto _LL596;} else{ goto _LL597;}
_LL597: if(( unsigned int) _temp591 > 4u?*(( int*) _temp591) == Cyc_Absyn_Evar:
0){ _LL611: _temp610=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp591)->f1;
_temp612=&(( void*)(( struct Cyc_Absyn_Evar_struct*) _temp591)->f1); goto _LL608;
_LL608: _temp607=(( struct Cyc_Absyn_Evar_struct*) _temp591)->f2; _temp609=&((
struct Cyc_Absyn_Evar_struct*) _temp591)->f2; goto _LL606; _LL606: _temp605=((
struct Cyc_Absyn_Evar_struct*) _temp591)->f3; goto _LL598;} else{ goto _LL599;}
_LL599: goto _LL600; _LL594: return Cyc_Absyn_tvar_cmp( v, _temp601) == 0;
_LL596: for( 0; _temp603 != 0; _temp603=(( struct Cyc_List_List*) _check_null(
_temp603))->tl){ if( Cyc_Tcutil_variable_in_effect( constrain, v,( void*)((
struct Cyc_List_List*) _check_null( _temp603))->hd)){ return 1;}} return 0;
_LL598: if(* _temp612 !=( void*) Cyc_Absyn_EffKind){(( void(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp613="effect evar has wrong kind";
struct _tagged_string _temp614; _temp614.curr= _temp613; _temp614.base= _temp613;
_temp614.last_plus_one= _temp613 + 27; _temp614;}));}{ void* _temp615= Cyc_Absyn_new_evar((
void*) Cyc_Absyn_EffKind); goto _LL616; _LL616: { struct Cyc_Absyn_JoinEff_struct*
_temp623=({ struct Cyc_Absyn_JoinEff_struct* _temp617=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp617[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp618; _temp618.tag= Cyc_Absyn_JoinEff; _temp618.f1=({ struct Cyc_List_List*
_temp619=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp619->hd=( void*) _temp615; _temp619->tl=({ struct Cyc_List_List* _temp620=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp620->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp621=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp621[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp622; _temp622.tag= Cyc_Absyn_VarType; _temp622.f1= v; _temp622;}); _temp621;}));
_temp620->tl= 0; _temp620;}); _temp619;}); _temp618;}); _temp617;}); goto _LL624;
_LL624:* _temp609=({ struct Cyc_Core_Opt* _temp625=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp625->v=( void*)(( void*) _temp623);
_temp625;}); return 1;}} _LL600: return 0; _LL592:;}} static int Cyc_Tcutil_evar_in_effect(
void* evar, void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp626= e; struct Cyc_List_List*
_temp634; int _temp636; struct Cyc_Core_Opt* _temp638; void* _temp640; _LL628:
if(( unsigned int) _temp626 > 4u?*(( int*) _temp626) == Cyc_Absyn_JoinEff: 0){
_LL635: _temp634=(( struct Cyc_Absyn_JoinEff_struct*) _temp626)->f1; goto _LL629;}
else{ goto _LL630;} _LL630: if(( unsigned int) _temp626 > 4u?*(( int*) _temp626)
== Cyc_Absyn_Evar: 0){ _LL641: _temp640=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp626)->f1; goto _LL639; _LL639: _temp638=(( struct Cyc_Absyn_Evar_struct*)
_temp626)->f2; goto _LL637; _LL637: _temp636=(( struct Cyc_Absyn_Evar_struct*)
_temp626)->f3; goto _LL631;} else{ goto _LL632;} _LL632: goto _LL633; _LL629:
for( 0; _temp634 != 0; _temp634=(( struct Cyc_List_List*) _check_null( _temp634))->tl){
if( Cyc_Tcutil_evar_in_effect( evar,( void*)(( struct Cyc_List_List*)
_check_null( _temp634))->hd)){ return 1;}} return 0; _LL631: return evar == e;
_LL633: return 0; _LL627:;}} int Cyc_Tcutil_subset_effect( int set_to_empty,
void* e1, void* e2){{ struct _tuple8* _temp642= Cyc_Tcutil_get_effect_evar( e2);
struct _tuple8 _temp648; struct Cyc_List_List* _temp649; void* _temp651; _LL644:
if( _temp642 == 0){ goto _LL646;} else{ _temp648=* _temp642; _LL652: _temp651=
_temp648.f1; goto _LL650; _LL650: _temp649= _temp648.f2; goto _LL645;} _LL646:
goto _LL647; _LL645: { void* _temp653= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_EffKind);
goto _LL654; _LL654: { struct Cyc_Absyn_JoinEff_struct* _temp659=({ struct Cyc_Absyn_JoinEff_struct*
_temp655=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp655[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp656; _temp656.tag= Cyc_Absyn_JoinEff;
_temp656.f1=({ struct Cyc_List_List* _temp657=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp657->hd=( void*) _temp653; _temp657->tl=({
struct Cyc_List_List* _temp658=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp658->hd=( void*) e1; _temp658->tl= 0; _temp658;});
_temp657;}); _temp656;}); _temp655;}); goto _LL660; _LL660: return Cyc_Tcutil_unify(
_temp651,( void*) _temp659);}} _LL647: goto _LL643; _LL643:;}{ void* _temp661=
Cyc_Tcutil_compress( e1); struct Cyc_List_List* _temp673; void* _temp675; struct
Cyc_Absyn_Tvar* _temp677; int _temp679; struct Cyc_Core_Opt* _temp681; struct
Cyc_Core_Opt** _temp683; void* _temp684; _LL663: if(( unsigned int) _temp661 > 4u?*((
int*) _temp661) == Cyc_Absyn_JoinEff: 0){ _LL674: _temp673=(( struct Cyc_Absyn_JoinEff_struct*)
_temp661)->f1; goto _LL664;} else{ goto _LL665;} _LL665: if(( unsigned int)
_temp661 > 4u?*(( int*) _temp661) == Cyc_Absyn_AccessEff: 0){ _LL676: _temp675=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp661)->f1; goto _LL666;} else{
goto _LL667;} _LL667: if(( unsigned int) _temp661 > 4u?*(( int*) _temp661) ==
Cyc_Absyn_VarType: 0){ _LL678: _temp677=(( struct Cyc_Absyn_VarType_struct*)
_temp661)->f1; goto _LL668;} else{ goto _LL669;} _LL669: if(( unsigned int)
_temp661 > 4u?*(( int*) _temp661) == Cyc_Absyn_Evar: 0){ _LL685: _temp684=( void*)((
struct Cyc_Absyn_Evar_struct*) _temp661)->f1; goto _LL682; _LL682: _temp681=((
struct Cyc_Absyn_Evar_struct*) _temp661)->f2; _temp683=&(( struct Cyc_Absyn_Evar_struct*)
_temp661)->f2; goto _LL680; _LL680: _temp679=(( struct Cyc_Absyn_Evar_struct*)
_temp661)->f3; goto _LL670;} else{ goto _LL671;} _LL671: goto _LL672; _LL664:
for( 0; _temp673 != 0; _temp673=(( struct Cyc_List_List*) _check_null( _temp673))->tl){
if( ! Cyc_Tcutil_subset_effect( set_to_empty,( void*)(( struct Cyc_List_List*)
_check_null( _temp673))->hd, e2)){ return 0;}} return 1; _LL666: return( Cyc_Tcutil_region_in_effect(
0, _temp675, e2)? 1: Cyc_Tcutil_region_in_effect( 1, _temp675, e2))? 1: Cyc_Tcutil_unify(
_temp675,( void*) Cyc_Absyn_HeapRgn); _LL668: return Cyc_Tcutil_variable_in_effect(
0, _temp677, e2)? 1: Cyc_Tcutil_variable_in_effect( 1, _temp677, e2); _LL670:
if( ! Cyc_Tcutil_evar_in_effect( e1, e2)){ if( set_to_empty){* _temp683=({
struct Cyc_Core_Opt* _temp686=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp686->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp687=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp687[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp688; _temp688.tag= Cyc_Absyn_JoinEff;
_temp688.f1= 0; _temp688;}); _temp687;})); _temp686;});} else{* _temp683=({
struct Cyc_Core_Opt* _temp689=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp689->v=( void*) e2; _temp689;});}} return 1; _LL672: return((
int(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({ struct _tagged_string
_temp690= Cyc_Absynpp_typ2string( e1); xprintf("subset_effect: bad effect: %.*s",
_temp690.last_plus_one - _temp690.curr, _temp690.curr);})); _LL662:;}} struct
_tuple9{ struct _tuple8* f1; struct _tuple8* f2; } ; static int Cyc_Tcutil_unify_effect(
void* e1, void* e2){ e1= Cyc_Tcutil_compress( e1); e2= Cyc_Tcutil_compress( e2);
Cyc_Tcutil_normalize_effect( e1); Cyc_Tcutil_normalize_effect( e2);{ struct
_tuple9 _temp692=({ struct _tuple9 _temp691; _temp691.f1= Cyc_Tcutil_get_effect_evar(
e1); _temp691.f2= Cyc_Tcutil_get_effect_evar( e2); _temp691;}); struct _tuple8*
_temp698; struct _tuple8 _temp700; struct Cyc_List_List* _temp701; void*
_temp703; struct _tuple8* _temp705; struct _tuple8 _temp707; struct Cyc_List_List*
_temp708; void* _temp710; _LL694: _LL706: _temp705= _temp692.f1; if( _temp705 ==
0){ goto _LL696;} else{ _temp707=* _temp705; _LL711: _temp710= _temp707.f1; goto
_LL709; _LL709: _temp708= _temp707.f2; goto _LL699;} _LL699: _temp698= _temp692.f2;
if( _temp698 == 0){ goto _LL696;} else{ _temp700=* _temp698; _LL704: _temp703=
_temp700.f1; goto _LL702; _LL702: _temp701= _temp700.f2; goto _LL695;} _LL696:
goto _LL697; _LL695: { void* _temp712= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_EffKind);
goto _LL713; _LL713: { struct Cyc_Absyn_JoinEff_struct* _temp717=({ struct Cyc_Absyn_JoinEff_struct*
_temp714=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp714[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp715; _temp715.tag= Cyc_Absyn_JoinEff;
_temp715.f1=({ struct Cyc_List_List* _temp716=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp716->hd=( void*) _temp712; _temp716->tl=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp708, _temp701); _temp716;}); _temp715;}); _temp714;}); goto _LL718; _LL718:
return Cyc_Tcutil_unify( _temp710,( void*) _temp717)? Cyc_Tcutil_unify( _temp703,(
void*) _temp717): 0;}} _LL697: return Cyc_Tcutil_subset_effect( 0, e1, e2)? Cyc_Tcutil_subset_effect(
0, e2, e1): 0; _LL693:;}} struct _tuple10{ struct Cyc_Absyn_VarargInfo* f1;
struct Cyc_Absyn_VarargInfo* f2; } ; struct _tuple11{ struct Cyc_Core_Opt* f1;
struct Cyc_Core_Opt* f2; } ; void Cyc_Tcutil_unify_it( void* t1, void* t2){ t1=
Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2); if( t1 == t2){ return;}{
void* _temp719= t1; int _temp725; struct Cyc_Core_Opt* _temp727; struct Cyc_Core_Opt**
_temp729; void* _temp730; _LL721: if(( unsigned int) _temp719 > 4u?*(( int*)
_temp719) == Cyc_Absyn_Evar: 0){ _LL731: _temp730=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp719)->f1; goto _LL728; _LL728: _temp727=(( struct Cyc_Absyn_Evar_struct*)
_temp719)->f2; _temp729=&(( struct Cyc_Absyn_Evar_struct*) _temp719)->f2; goto
_LL726; _LL726: _temp725=(( struct Cyc_Absyn_Evar_struct*) _temp719)->f3; goto
_LL722;} else{ goto _LL723;} _LL723: goto _LL724; _LL722: Cyc_Tcutil_occurs( t1,
t2);{ void* _temp732= Cyc_Tcutil_typ_kind( t2); goto _LL733; _LL733: if( Cyc_Tcutil_kind_leq(
_temp732, _temp730)){* _temp729=({ struct Cyc_Core_Opt* _temp734=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp734->v=( void*) t2; _temp734;});
return;} else{{ void* _temp735= t2; int _temp743; struct Cyc_Core_Opt* _temp745;
struct Cyc_Core_Opt** _temp747; void* _temp748; struct Cyc_Absyn_PtrInfo
_temp751; _LL737: if(( unsigned int) _temp735 > 4u?*(( int*) _temp735) == Cyc_Absyn_Evar:
0){ _LL749: _temp748=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp735)->f1;
goto _LL746; _LL746: _temp745=(( struct Cyc_Absyn_Evar_struct*) _temp735)->f2;
_temp747=&(( struct Cyc_Absyn_Evar_struct*) _temp735)->f2; goto _LL744; _LL744:
_temp743=(( struct Cyc_Absyn_Evar_struct*) _temp735)->f3; goto _LL738;} else{
goto _LL739;} _LL739: if(( unsigned int) _temp735 > 4u?*(( int*) _temp735) ==
Cyc_Absyn_PointerType: 0){ _LL752: _temp751=(( struct Cyc_Absyn_PointerType_struct*)
_temp735)->f1; goto _LL750;} else{ goto _LL741;} _LL750: if( _temp730 ==( void*)
Cyc_Absyn_BoxKind){ goto _LL740;} else{ goto _LL741;} _LL741: goto _LL742;
_LL738: if( Cyc_Tcutil_kind_leq( _temp730, _temp732)){* _temp747=({ struct Cyc_Core_Opt*
_temp753=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp753->v=( void*) t1; _temp753;}); return;} goto _LL736; _LL740: { struct Cyc_Absyn_Conref*
_temp754=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp751.bounds); goto _LL755; _LL755:{ void* _temp756=( void*) _temp754->v;
_LL758: if( _temp756 ==( void*) Cyc_Absyn_No_constr){ goto _LL759;} else{ goto
_LL760;} _LL760: goto _LL761; _LL759:( void*)( _temp754->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_constr_struct* _temp762=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp762[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp763; _temp763.tag= Cyc_Absyn_Eq_constr; _temp763.f1=(
void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp764=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp764[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp765; _temp765.tag= Cyc_Absyn_Upper_b; _temp765.f1= Cyc_Absyn_signed_int_exp(
1, 0); _temp765;}); _temp764;})); _temp763;}); _temp762;})));* _temp729=({
struct Cyc_Core_Opt* _temp766=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp766->v=( void*) t2; _temp766;}); return; _LL761: goto
_LL757; _LL757:;} goto _LL736;} _LL742: goto _LL736; _LL736:;}( void) _throw((
void*) Cyc_Tcutil_Unify);}} _LL724: goto _LL720; _LL720:;}{ struct _tuple7
_temp768=({ struct _tuple7 _temp767; _temp767.f1= t2; _temp767.f2= t1; _temp767;});
void* _temp818; void* _temp820; int _temp822; struct Cyc_Core_Opt* _temp824;
void* _temp826; void* _temp828; void* _temp830; void* _temp832; struct Cyc_Absyn_Tvar*
_temp834; void* _temp836; struct Cyc_Absyn_Tvar* _temp838; void* _temp840;
struct Cyc_Absyn_Structdecl** _temp842; struct Cyc_List_List* _temp844; struct
_tuple1* _temp846; void* _temp848; struct Cyc_Absyn_Structdecl** _temp850;
struct Cyc_List_List* _temp852; struct _tuple1* _temp854; void* _temp856; struct
Cyc_Absyn_Enumdecl* _temp858; struct _tuple1* _temp860; void* _temp862; struct
Cyc_Absyn_Enumdecl* _temp864; struct _tuple1* _temp866; void* _temp868; struct
Cyc_Absyn_Uniondecl** _temp870; struct Cyc_List_List* _temp872; struct _tuple1*
_temp874; void* _temp876; struct Cyc_Absyn_Uniondecl** _temp878; struct Cyc_List_List*
_temp880; struct _tuple1* _temp882; void* _temp884; struct Cyc_Absyn_TunionInfo
_temp886; void* _temp888; struct Cyc_List_List* _temp890; void* _temp892; struct
Cyc_Absyn_Tuniondecl* _temp894; void* _temp896; struct Cyc_Absyn_TunionInfo
_temp898; void* _temp900; struct Cyc_List_List* _temp902; void* _temp904; struct
Cyc_Absyn_Tuniondecl* _temp906; void* _temp908; struct Cyc_Absyn_TunionFieldInfo
_temp910; struct Cyc_List_List* _temp912; void* _temp914; struct Cyc_Absyn_Tunionfield*
_temp916; struct Cyc_Absyn_Tuniondecl* _temp918; void* _temp920; struct Cyc_Absyn_TunionFieldInfo
_temp922; struct Cyc_List_List* _temp924; void* _temp926; struct Cyc_Absyn_Tunionfield*
_temp928; struct Cyc_Absyn_Tuniondecl* _temp930; void* _temp932; struct Cyc_Absyn_PtrInfo
_temp934; struct Cyc_Absyn_Conref* _temp936; struct Cyc_Absyn_Tqual _temp938;
struct Cyc_Absyn_Conref* _temp940; void* _temp942; void* _temp944; void*
_temp946; struct Cyc_Absyn_PtrInfo _temp948; struct Cyc_Absyn_Conref* _temp950;
struct Cyc_Absyn_Tqual _temp952; struct Cyc_Absyn_Conref* _temp954; void*
_temp956; void* _temp958; void* _temp960; void* _temp962; void* _temp964; void*
_temp966; void* _temp968; void* _temp970; void* _temp972; void* _temp974; void*
_temp976; void* _temp978; void* _temp980; struct Cyc_Absyn_Exp* _temp982; struct
Cyc_Absyn_Tqual _temp984; void* _temp986; void* _temp988; struct Cyc_Absyn_Exp*
_temp990; struct Cyc_Absyn_Tqual _temp992; void* _temp994; void* _temp996;
struct Cyc_Absyn_FnInfo _temp998; struct Cyc_List_List* _temp1000; struct Cyc_Absyn_VarargInfo*
_temp1002; int _temp1004; struct Cyc_List_List* _temp1006; void* _temp1008;
struct Cyc_Core_Opt* _temp1010; struct Cyc_List_List* _temp1012; void* _temp1014;
struct Cyc_Absyn_FnInfo _temp1016; struct Cyc_List_List* _temp1018; struct Cyc_Absyn_VarargInfo*
_temp1020; int _temp1022; struct Cyc_List_List* _temp1024; void* _temp1026;
struct Cyc_Core_Opt* _temp1028; struct Cyc_List_List* _temp1030; void* _temp1032;
struct Cyc_List_List* _temp1034; void* _temp1036; struct Cyc_List_List*
_temp1038; void* _temp1040; struct Cyc_List_List* _temp1042; void* _temp1044;
struct Cyc_List_List* _temp1046; void* _temp1048; struct Cyc_List_List*
_temp1050; void* _temp1052; struct Cyc_List_List* _temp1054; void* _temp1056;
void* _temp1058; void* _temp1060; void* _temp1062; void* _temp1064; void*
_temp1066; void* _temp1068; void* _temp1070; struct Cyc_List_List* _temp1072;
void* _temp1074; struct Cyc_List_List* _temp1076; void* _temp1078; void*
_temp1080; void* _temp1082; void* _temp1084; void* _temp1086; void* _temp1088;
void* _temp1090; _LL770: _LL821: _temp820= _temp768.f1; if(( unsigned int)
_temp820 > 4u?*(( int*) _temp820) == Cyc_Absyn_Evar: 0){ _LL827: _temp826=( void*)((
struct Cyc_Absyn_Evar_struct*) _temp820)->f1; goto _LL825; _LL825: _temp824=((
struct Cyc_Absyn_Evar_struct*) _temp820)->f2; goto _LL823; _LL823: _temp822=((
struct Cyc_Absyn_Evar_struct*) _temp820)->f3; goto _LL819;} else{ goto _LL772;}
_LL819: _temp818= _temp768.f2; goto _LL771; _LL772: _LL831: _temp830= _temp768.f1;
if( _temp830 ==( void*) Cyc_Absyn_VoidType){ goto _LL829;} else{ goto _LL774;}
_LL829: _temp828= _temp768.f2; if( _temp828 ==( void*) Cyc_Absyn_VoidType){ goto
_LL773;} else{ goto _LL774;} _LL774: _LL837: _temp836= _temp768.f1; if((
unsigned int) _temp836 > 4u?*(( int*) _temp836) == Cyc_Absyn_VarType: 0){ _LL839:
_temp838=(( struct Cyc_Absyn_VarType_struct*) _temp836)->f1; goto _LL833;} else{
goto _LL776;} _LL833: _temp832= _temp768.f2; if(( unsigned int) _temp832 > 4u?*((
int*) _temp832) == Cyc_Absyn_VarType: 0){ _LL835: _temp834=(( struct Cyc_Absyn_VarType_struct*)
_temp832)->f1; goto _LL775;} else{ goto _LL776;} _LL776: _LL849: _temp848=
_temp768.f1; if(( unsigned int) _temp848 > 4u?*(( int*) _temp848) == Cyc_Absyn_StructType:
0){ _LL855: _temp854=(( struct Cyc_Absyn_StructType_struct*) _temp848)->f1; goto
_LL853; _LL853: _temp852=(( struct Cyc_Absyn_StructType_struct*) _temp848)->f2;
goto _LL851; _LL851: _temp850=(( struct Cyc_Absyn_StructType_struct*) _temp848)->f3;
goto _LL841;} else{ goto _LL778;} _LL841: _temp840= _temp768.f2; if((
unsigned int) _temp840 > 4u?*(( int*) _temp840) == Cyc_Absyn_StructType: 0){
_LL847: _temp846=(( struct Cyc_Absyn_StructType_struct*) _temp840)->f1; goto
_LL845; _LL845: _temp844=(( struct Cyc_Absyn_StructType_struct*) _temp840)->f2;
goto _LL843; _LL843: _temp842=(( struct Cyc_Absyn_StructType_struct*) _temp840)->f3;
goto _LL777;} else{ goto _LL778;} _LL778: _LL863: _temp862= _temp768.f1; if((
unsigned int) _temp862 > 4u?*(( int*) _temp862) == Cyc_Absyn_EnumType: 0){
_LL867: _temp866=(( struct Cyc_Absyn_EnumType_struct*) _temp862)->f1; goto
_LL865; _LL865: _temp864=(( struct Cyc_Absyn_EnumType_struct*) _temp862)->f2;
goto _LL857;} else{ goto _LL780;} _LL857: _temp856= _temp768.f2; if((
unsigned int) _temp856 > 4u?*(( int*) _temp856) == Cyc_Absyn_EnumType: 0){
_LL861: _temp860=(( struct Cyc_Absyn_EnumType_struct*) _temp856)->f1; goto
_LL859; _LL859: _temp858=(( struct Cyc_Absyn_EnumType_struct*) _temp856)->f2;
goto _LL779;} else{ goto _LL780;} _LL780: _LL877: _temp876= _temp768.f1; if((
unsigned int) _temp876 > 4u?*(( int*) _temp876) == Cyc_Absyn_UnionType: 0){
_LL883: _temp882=(( struct Cyc_Absyn_UnionType_struct*) _temp876)->f1; goto
_LL881; _LL881: _temp880=(( struct Cyc_Absyn_UnionType_struct*) _temp876)->f2;
goto _LL879; _LL879: _temp878=(( struct Cyc_Absyn_UnionType_struct*) _temp876)->f3;
goto _LL869;} else{ goto _LL782;} _LL869: _temp868= _temp768.f2; if((
unsigned int) _temp868 > 4u?*(( int*) _temp868) == Cyc_Absyn_UnionType: 0){
_LL875: _temp874=(( struct Cyc_Absyn_UnionType_struct*) _temp868)->f1; goto
_LL873; _LL873: _temp872=(( struct Cyc_Absyn_UnionType_struct*) _temp868)->f2;
goto _LL871; _LL871: _temp870=(( struct Cyc_Absyn_UnionType_struct*) _temp868)->f3;
goto _LL781;} else{ goto _LL782;} _LL782: _LL897: _temp896= _temp768.f1; if((
unsigned int) _temp896 > 4u?*(( int*) _temp896) == Cyc_Absyn_TunionType: 0){
_LL899: _temp898=(( struct Cyc_Absyn_TunionType_struct*) _temp896)->f1; _LL905:
_temp904=( void*) _temp898.tunion_info; if(*(( int*) _temp904) == Cyc_Absyn_KnownTunion){
_LL907: _temp906=(( struct Cyc_Absyn_KnownTunion_struct*) _temp904)->f1; goto
_LL903;} else{ goto _LL784;} _LL903: _temp902= _temp898.targs; goto _LL901;
_LL901: _temp900=( void*) _temp898.rgn; goto _LL885;} else{ goto _LL784;} _LL885:
_temp884= _temp768.f2; if(( unsigned int) _temp884 > 4u?*(( int*) _temp884) ==
Cyc_Absyn_TunionType: 0){ _LL887: _temp886=(( struct Cyc_Absyn_TunionType_struct*)
_temp884)->f1; _LL893: _temp892=( void*) _temp886.tunion_info; if(*(( int*)
_temp892) == Cyc_Absyn_KnownTunion){ _LL895: _temp894=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp892)->f1; goto _LL891;} else{ goto _LL784;} _LL891: _temp890= _temp886.targs;
goto _LL889; _LL889: _temp888=( void*) _temp886.rgn; goto _LL783;} else{ goto
_LL784;} _LL784: _LL921: _temp920= _temp768.f1; if(( unsigned int) _temp920 > 4u?*((
int*) _temp920) == Cyc_Absyn_TunionFieldType: 0){ _LL923: _temp922=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp920)->f1; _LL927: _temp926=( void*) _temp922.field_info; if(*(( int*)
_temp926) == Cyc_Absyn_KnownTunionfield){ _LL931: _temp930=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp926)->f1; goto _LL929; _LL929: _temp928=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp926)->f2; goto _LL925;} else{ goto _LL786;} _LL925: _temp924= _temp922.targs;
goto _LL909;} else{ goto _LL786;} _LL909: _temp908= _temp768.f2; if((
unsigned int) _temp908 > 4u?*(( int*) _temp908) == Cyc_Absyn_TunionFieldType: 0){
_LL911: _temp910=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp908)->f1;
_LL915: _temp914=( void*) _temp910.field_info; if(*(( int*) _temp914) == Cyc_Absyn_KnownTunionfield){
_LL919: _temp918=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp914)->f1;
goto _LL917; _LL917: _temp916=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp914)->f2; goto _LL913;} else{ goto _LL786;} _LL913: _temp912= _temp910.targs;
goto _LL785;} else{ goto _LL786;} _LL786: _LL947: _temp946= _temp768.f1; if((
unsigned int) _temp946 > 4u?*(( int*) _temp946) == Cyc_Absyn_PointerType: 0){
_LL949: _temp948=(( struct Cyc_Absyn_PointerType_struct*) _temp946)->f1; _LL959:
_temp958=( void*) _temp948.elt_typ; goto _LL957; _LL957: _temp956=( void*)
_temp948.rgn_typ; goto _LL955; _LL955: _temp954= _temp948.nullable; goto _LL953;
_LL953: _temp952= _temp948.tq; goto _LL951; _LL951: _temp950= _temp948.bounds;
goto _LL933;} else{ goto _LL788;} _LL933: _temp932= _temp768.f2; if((
unsigned int) _temp932 > 4u?*(( int*) _temp932) == Cyc_Absyn_PointerType: 0){
_LL935: _temp934=(( struct Cyc_Absyn_PointerType_struct*) _temp932)->f1; _LL945:
_temp944=( void*) _temp934.elt_typ; goto _LL943; _LL943: _temp942=( void*)
_temp934.rgn_typ; goto _LL941; _LL941: _temp940= _temp934.nullable; goto _LL939;
_LL939: _temp938= _temp934.tq; goto _LL937; _LL937: _temp936= _temp934.bounds;
goto _LL787;} else{ goto _LL788;} _LL788: _LL967: _temp966= _temp768.f1; if((
unsigned int) _temp966 > 4u?*(( int*) _temp966) == Cyc_Absyn_IntType: 0){ _LL971:
_temp970=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp966)->f1; goto _LL969;
_LL969: _temp968=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp966)->f2;
goto _LL961;} else{ goto _LL790;} _LL961: _temp960= _temp768.f2; if((
unsigned int) _temp960 > 4u?*(( int*) _temp960) == Cyc_Absyn_IntType: 0){ _LL965:
_temp964=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp960)->f1; goto _LL963;
_LL963: _temp962=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp960)->f2;
goto _LL789;} else{ goto _LL790;} _LL790: _LL975: _temp974= _temp768.f1; if(
_temp974 ==( void*) Cyc_Absyn_FloatType){ goto _LL973;} else{ goto _LL792;}
_LL973: _temp972= _temp768.f2; if( _temp972 ==( void*) Cyc_Absyn_FloatType){
goto _LL791;} else{ goto _LL792;} _LL792: _LL979: _temp978= _temp768.f1; if(
_temp978 ==( void*) Cyc_Absyn_DoubleType){ goto _LL977;} else{ goto _LL794;}
_LL977: _temp976= _temp768.f2; if( _temp976 ==( void*) Cyc_Absyn_DoubleType){
goto _LL793;} else{ goto _LL794;} _LL794: _LL989: _temp988= _temp768.f1; if((
unsigned int) _temp988 > 4u?*(( int*) _temp988) == Cyc_Absyn_ArrayType: 0){
_LL995: _temp994=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp988)->f1;
goto _LL993; _LL993: _temp992=(( struct Cyc_Absyn_ArrayType_struct*) _temp988)->f2;
goto _LL991; _LL991: _temp990=(( struct Cyc_Absyn_ArrayType_struct*) _temp988)->f3;
goto _LL981;} else{ goto _LL796;} _LL981: _temp980= _temp768.f2; if((
unsigned int) _temp980 > 4u?*(( int*) _temp980) == Cyc_Absyn_ArrayType: 0){
_LL987: _temp986=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp980)->f1;
goto _LL985; _LL985: _temp984=(( struct Cyc_Absyn_ArrayType_struct*) _temp980)->f2;
goto _LL983; _LL983: _temp982=(( struct Cyc_Absyn_ArrayType_struct*) _temp980)->f3;
goto _LL795;} else{ goto _LL796;} _LL796: _LL1015: _temp1014= _temp768.f1; if((
unsigned int) _temp1014 > 4u?*(( int*) _temp1014) == Cyc_Absyn_FnType: 0){
_LL1017: _temp1016=(( struct Cyc_Absyn_FnType_struct*) _temp1014)->f1; _LL1031:
_temp1030= _temp1016.tvars; goto _LL1029; _LL1029: _temp1028= _temp1016.effect;
goto _LL1027; _LL1027: _temp1026=( void*) _temp1016.ret_typ; goto _LL1025;
_LL1025: _temp1024= _temp1016.args; goto _LL1023; _LL1023: _temp1022= _temp1016.c_varargs;
goto _LL1021; _LL1021: _temp1020= _temp1016.cyc_varargs; goto _LL1019; _LL1019:
_temp1018= _temp1016.attributes; goto _LL997;} else{ goto _LL798;} _LL997:
_temp996= _temp768.f2; if(( unsigned int) _temp996 > 4u?*(( int*) _temp996) ==
Cyc_Absyn_FnType: 0){ _LL999: _temp998=(( struct Cyc_Absyn_FnType_struct*)
_temp996)->f1; _LL1013: _temp1012= _temp998.tvars; goto _LL1011; _LL1011:
_temp1010= _temp998.effect; goto _LL1009; _LL1009: _temp1008=( void*) _temp998.ret_typ;
goto _LL1007; _LL1007: _temp1006= _temp998.args; goto _LL1005; _LL1005:
_temp1004= _temp998.c_varargs; goto _LL1003; _LL1003: _temp1002= _temp998.cyc_varargs;
goto _LL1001; _LL1001: _temp1000= _temp998.attributes; goto _LL797;} else{ goto
_LL798;} _LL798: _LL1037: _temp1036= _temp768.f1; if(( unsigned int) _temp1036 >
4u?*(( int*) _temp1036) == Cyc_Absyn_TupleType: 0){ _LL1039: _temp1038=(( struct
Cyc_Absyn_TupleType_struct*) _temp1036)->f1; goto _LL1033;} else{ goto _LL800;}
_LL1033: _temp1032= _temp768.f2; if(( unsigned int) _temp1032 > 4u?*(( int*)
_temp1032) == Cyc_Absyn_TupleType: 0){ _LL1035: _temp1034=(( struct Cyc_Absyn_TupleType_struct*)
_temp1032)->f1; goto _LL799;} else{ goto _LL800;} _LL800: _LL1045: _temp1044=
_temp768.f1; if(( unsigned int) _temp1044 > 4u?*(( int*) _temp1044) == Cyc_Absyn_AnonStructType:
0){ _LL1047: _temp1046=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1044)->f1;
goto _LL1041;} else{ goto _LL802;} _LL1041: _temp1040= _temp768.f2; if((
unsigned int) _temp1040 > 4u?*(( int*) _temp1040) == Cyc_Absyn_AnonStructType: 0){
_LL1043: _temp1042=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1040)->f1;
goto _LL801;} else{ goto _LL802;} _LL802: _LL1053: _temp1052= _temp768.f1; if((
unsigned int) _temp1052 > 4u?*(( int*) _temp1052) == Cyc_Absyn_AnonUnionType: 0){
_LL1055: _temp1054=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1052)->f1;
goto _LL1049;} else{ goto _LL804;} _LL1049: _temp1048= _temp768.f2; if((
unsigned int) _temp1048 > 4u?*(( int*) _temp1048) == Cyc_Absyn_AnonUnionType: 0){
_LL1051: _temp1050=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1048)->f1;
goto _LL803;} else{ goto _LL804;} _LL804: _LL1059: _temp1058= _temp768.f1; if(
_temp1058 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1057;} else{ goto _LL806;}
_LL1057: _temp1056= _temp768.f2; if( _temp1056 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL805;} else{ goto _LL806;} _LL806: _LL1065: _temp1064= _temp768.f1; if((
unsigned int) _temp1064 > 4u?*(( int*) _temp1064) == Cyc_Absyn_RgnHandleType: 0){
_LL1067: _temp1066=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1064)->f1;
goto _LL1061;} else{ goto _LL808;} _LL1061: _temp1060= _temp768.f2; if((
unsigned int) _temp1060 > 4u?*(( int*) _temp1060) == Cyc_Absyn_RgnHandleType: 0){
_LL1063: _temp1062=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1060)->f1;
goto _LL807;} else{ goto _LL808;} _LL808: _LL1071: _temp1070= _temp768.f1; if((
unsigned int) _temp1070 > 4u?*(( int*) _temp1070) == Cyc_Absyn_JoinEff: 0){
_LL1073: _temp1072=(( struct Cyc_Absyn_JoinEff_struct*) _temp1070)->f1; goto
_LL1069;} else{ goto _LL810;} _LL1069: _temp1068= _temp768.f2; goto _LL809;
_LL810: _LL1079: _temp1078= _temp768.f1; goto _LL1075; _LL1075: _temp1074=
_temp768.f2; if(( unsigned int) _temp1074 > 4u?*(( int*) _temp1074) == Cyc_Absyn_JoinEff:
0){ _LL1077: _temp1076=(( struct Cyc_Absyn_JoinEff_struct*) _temp1074)->f1; goto
_LL811;} else{ goto _LL812;} _LL812: _LL1083: _temp1082= _temp768.f1; if((
unsigned int) _temp1082 > 4u?*(( int*) _temp1082) == Cyc_Absyn_AccessEff: 0){
_LL1085: _temp1084=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp1082)->f1;
goto _LL1081;} else{ goto _LL814;} _LL1081: _temp1080= _temp768.f2; goto _LL813;
_LL814: _LL1091: _temp1090= _temp768.f1; goto _LL1087; _LL1087: _temp1086=
_temp768.f2; if(( unsigned int) _temp1086 > 4u?*(( int*) _temp1086) == Cyc_Absyn_AccessEff:
0){ _LL1089: _temp1088=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp1086)->f1;
goto _LL815;} else{ goto _LL816;} _LL816: goto _LL817; _LL771: Cyc_Tcutil_unify_it(
t2, t1); return; _LL773: return; _LL775: { struct _tagged_string* _temp1092=
_temp838->name; goto _LL1093; _LL1093: { struct _tagged_string* _temp1094=
_temp834->name; goto _LL1095; _LL1095: { void* _temp1096=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp838->kind); goto _LL1097; _LL1097: { void*
_temp1098=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp834->kind); goto _LL1099; _LL1099: if( Cyc_String_zstrptrcmp( _temp1092,
_temp1094) == 0){ if( _temp1096 != _temp1098){(( void(*)( struct _tagged_string
msg)) Cyc_Tcutil_impos)(({ struct _tagged_string _temp1100=* _temp1092; struct
_tagged_string _temp1101= Cyc_Absynpp_kind2string( _temp1096); struct
_tagged_string _temp1102= Cyc_Absynpp_kind2string( _temp1098); xprintf("same type variable %.*s has kinds %.*s and %.*s",
_temp1100.last_plus_one - _temp1100.curr, _temp1100.curr, _temp1101.last_plus_one
- _temp1101.curr, _temp1101.curr, _temp1102.last_plus_one - _temp1102.curr,
_temp1102.curr);}));} return;} goto _LL769;}}}} _LL777: if((( _temp846 != 0?
_temp854 != 0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*) _check_null( _temp846),(
struct _tuple1*) _check_null( _temp854)) == 0: 0)? 1:( _temp846 == 0? _temp854
== 0: 0)){ Cyc_Tcutil_unify_list( _temp844, _temp852); return;} goto _LL769;
_LL779: if( Cyc_Absyn_qvar_cmp( _temp866, _temp860) == 0){ return;} goto _LL769;
_LL781: if((( _temp874 != 0? _temp882 != 0: 0)? Cyc_Absyn_qvar_cmp(( struct
_tuple1*) _check_null( _temp874),( struct _tuple1*) _check_null( _temp882)) == 0:
0)? 1:( _temp874 == 0? _temp882 == 0: 0)){ Cyc_Tcutil_unify_list( _temp872,
_temp880); return;} goto _LL769; _LL783: if( _temp906 == _temp894? 1: Cyc_Absyn_qvar_cmp(
_temp906->name, _temp894->name) == 0){ Cyc_Tcutil_unify_it( _temp888, _temp900);
Cyc_Tcutil_unify_list( _temp890, _temp902); return;} goto _LL769; _LL785: if((
_temp930 == _temp918? 1: Cyc_Absyn_qvar_cmp( _temp930->name, _temp918->name) ==
0)? _temp928 == _temp916? 1: Cyc_Absyn_qvar_cmp( _temp928->name, _temp916->name)
== 0: 0){ Cyc_Tcutil_unify_list( _temp912, _temp924); return;} goto _LL769;
_LL787: Cyc_Tcutil_unify_it( _temp944, _temp958); Cyc_Tcutil_unify_it( _temp956,
_temp942); Cyc_Tcutil_unify_tqual( _temp938, _temp952);(( void(*)( int(* cmp)(
void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)(
Cyc_Tcutil_boundscmp, _temp936, _temp950);{ void* _temp1103=( void*)((( struct
Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp936))->v; void* _temp1109; _LL1105: if(( unsigned int) _temp1103 > 1u?*((
int*) _temp1103) == Cyc_Absyn_Eq_constr: 0){ _LL1110: _temp1109=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1103)->f1; if( _temp1109 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1106;} else{ goto _LL1107;}} else{ goto _LL1107;} _LL1107: goto _LL1108;
_LL1106: return; _LL1108: goto _LL1104; _LL1104:;}(( void(*)( int(* cmp)( int,
int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp, _temp940, _temp954); return; _LL789: if( _temp964 == _temp970?
_temp962 == _temp968: 0){ return;} goto _LL769; _LL791: return; _LL793: return;
_LL795: Cyc_Tcutil_unify_it( _temp986, _temp994); Cyc_Tcutil_unify_tqual(
_temp984, _temp992); if( _temp990 == _temp982){ return;} if( _temp990 == 0? 1:
_temp982 == 0){ goto _LL769;} if( Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp990)) == Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp982))){ return;} goto _LL769; _LL797: { int done= 0;{ struct
_RegionHandle _temp1111= _new_region(); struct _RegionHandle* rgn=& _temp1111;
_push_region( rgn);{ struct Cyc_List_List* inst= 0; while( _temp1012 != 0) { if(
_temp1030 == 0){ break;} inst=({ struct Cyc_List_List* _temp1112=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1112->hd=( void*)({
struct _tuple5* _temp1113=( struct _tuple5*) _region_malloc( rgn, sizeof( struct
_tuple5)); _temp1113->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp1030))->hd; _temp1113->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1114=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1114[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1115; _temp1115.tag= Cyc_Absyn_VarType;
_temp1115.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp1012))->hd; _temp1115;}); _temp1114;}); _temp1113;}); _temp1112->tl= inst;
_temp1112;}); _temp1012=(( struct Cyc_List_List*) _check_null( _temp1012))->tl;
_temp1030=(( struct Cyc_List_List*) _check_null( _temp1030))->tl;} if( _temp1030
!= 0){ _npop_handler( 0u); goto _LL769;} if( inst != 0){ Cyc_Tcutil_unify_it((
void*)({ struct Cyc_Absyn_FnType_struct* _temp1116=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1116[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1117; _temp1117.tag= Cyc_Absyn_FnType; _temp1117.f1=({ struct Cyc_Absyn_FnInfo
_temp1118; _temp1118.tvars= 0; _temp1118.effect= _temp1010; _temp1118.ret_typ=(
void*) _temp1008; _temp1118.args= _temp1006; _temp1118.c_varargs= _temp1004;
_temp1118.cyc_varargs= _temp1002; _temp1118.attributes= _temp1000; _temp1118;});
_temp1117;}); _temp1116;}), Cyc_Tcutil_rsubstitute( rgn, inst,( void*)({ struct
Cyc_Absyn_FnType_struct* _temp1119=( struct Cyc_Absyn_FnType_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_FnType_struct)); _temp1119[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1120; _temp1120.tag= Cyc_Absyn_FnType; _temp1120.f1=({ struct Cyc_Absyn_FnInfo
_temp1121; _temp1121.tvars= 0; _temp1121.effect= _temp1028; _temp1121.ret_typ=(
void*) _temp1026; _temp1121.args= _temp1024; _temp1121.c_varargs= _temp1022;
_temp1121.cyc_varargs= _temp1020; _temp1121.attributes= _temp1018; _temp1121;});
_temp1120;}); _temp1119;}))); done= 1;}}; _pop_region(& _temp1111);} if( done){
return;} Cyc_Tcutil_unify_it( _temp1008, _temp1026); for( 0; _temp1006 != 0?
_temp1024 != 0: 0;( _temp1006=(( struct Cyc_List_List*) _check_null( _temp1006))->tl,
_temp1024=(( struct Cyc_List_List*) _check_null( _temp1024))->tl)){ Cyc_Tcutil_unify_tqual((*((
struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp1006))->hd)).f2,(*((
struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp1024))->hd)).f2);
Cyc_Tcutil_unify_it((*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null(
_temp1006))->hd)).f3,(*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null(
_temp1024))->hd)).f3);} if( _temp1006 != 0? 1: _temp1024 != 0){ goto _LL769;}
if( _temp1004 != _temp1022){ goto _LL769;}{ int bad_cyc_vararg= 0;{ struct
_tuple10 _temp1123=({ struct _tuple10 _temp1122; _temp1122.f1= _temp1002;
_temp1122.f2= _temp1020; _temp1122;}); struct Cyc_Absyn_VarargInfo* _temp1133;
struct Cyc_Absyn_VarargInfo* _temp1135; struct Cyc_Absyn_VarargInfo* _temp1137;
struct Cyc_Absyn_VarargInfo* _temp1139; struct Cyc_Absyn_VarargInfo* _temp1141;
struct Cyc_Absyn_VarargInfo* _temp1143; struct Cyc_Absyn_VarargInfo* _temp1145;
struct Cyc_Absyn_VarargInfo _temp1147; int _temp1148; void* _temp1150; void*
_temp1152; struct Cyc_Absyn_Tqual _temp1154; struct Cyc_Core_Opt* _temp1156;
struct Cyc_Absyn_VarargInfo* _temp1158; struct Cyc_Absyn_VarargInfo _temp1160;
int _temp1161; void* _temp1163; void* _temp1165; struct Cyc_Absyn_Tqual
_temp1167; struct Cyc_Core_Opt* _temp1169; _LL1125: _LL1136: _temp1135=
_temp1123.f1; if( _temp1135 == 0){ goto _LL1134;} else{ goto _LL1127;} _LL1134:
_temp1133= _temp1123.f2; if( _temp1133 == 0){ goto _LL1126;} else{ goto _LL1127;}
_LL1127: _LL1140: _temp1139= _temp1123.f1; if( _temp1139 == 0){ goto _LL1138;}
else{ goto _LL1129;} _LL1138: _temp1137= _temp1123.f2; goto _LL1128; _LL1129:
_LL1144: _temp1143= _temp1123.f1; goto _LL1142; _LL1142: _temp1141= _temp1123.f2;
if( _temp1141 == 0){ goto _LL1130;} else{ goto _LL1131;} _LL1131: _LL1159:
_temp1158= _temp1123.f1; if( _temp1158 == 0){ goto _LL1124;} else{ _temp1160=*
_temp1158; _LL1170: _temp1169= _temp1160.name; goto _LL1168; _LL1168: _temp1167=
_temp1160.tq; goto _LL1166; _LL1166: _temp1165=( void*) _temp1160.type; goto
_LL1164; _LL1164: _temp1163=( void*) _temp1160.rgn; goto _LL1162; _LL1162:
_temp1161= _temp1160.inject; goto _LL1146;} _LL1146: _temp1145= _temp1123.f2;
if( _temp1145 == 0){ goto _LL1124;} else{ _temp1147=* _temp1145; _LL1157:
_temp1156= _temp1147.name; goto _LL1155; _LL1155: _temp1154= _temp1147.tq; goto
_LL1153; _LL1153: _temp1152=( void*) _temp1147.type; goto _LL1151; _LL1151:
_temp1150=( void*) _temp1147.rgn; goto _LL1149; _LL1149: _temp1148= _temp1147.inject;
goto _LL1132;} _LL1126: goto _LL1124; _LL1128: goto _LL1130; _LL1130:
bad_cyc_vararg= 1; goto _LL1124; _LL1132: Cyc_Tcutil_unify_tqual( _temp1167,
_temp1154); Cyc_Tcutil_unify_it( _temp1165, _temp1152); Cyc_Tcutil_unify_it(
_temp1163, _temp1150); if( _temp1161 != _temp1148){ bad_cyc_vararg= 1;} goto
_LL1124; _LL1124:;} if( bad_cyc_vararg){ goto _LL769;}{ int bad_effect= 0;{
struct _tuple11 _temp1172=({ struct _tuple11 _temp1171; _temp1171.f1= _temp1010;
_temp1171.f2= _temp1028; _temp1171;}); struct Cyc_Core_Opt* _temp1182; struct
Cyc_Core_Opt* _temp1184; struct Cyc_Core_Opt* _temp1186; struct Cyc_Core_Opt*
_temp1188; struct Cyc_Core_Opt* _temp1190; struct Cyc_Core_Opt* _temp1192;
_LL1174: _LL1185: _temp1184= _temp1172.f1; if( _temp1184 == 0){ goto _LL1183;}
else{ goto _LL1176;} _LL1183: _temp1182= _temp1172.f2; if( _temp1182 == 0){ goto
_LL1175;} else{ goto _LL1176;} _LL1176: _LL1189: _temp1188= _temp1172.f1; if(
_temp1188 == 0){ goto _LL1187;} else{ goto _LL1178;} _LL1187: _temp1186=
_temp1172.f2; goto _LL1177; _LL1178: _LL1193: _temp1192= _temp1172.f1; goto
_LL1191; _LL1191: _temp1190= _temp1172.f2; if( _temp1190 == 0){ goto _LL1179;}
else{ goto _LL1180;} _LL1180: goto _LL1181; _LL1175: goto _LL1173; _LL1177: goto
_LL1179; _LL1179: bad_effect= 1; goto _LL1173; _LL1181: bad_effect= ! Cyc_Tcutil_unify_effect((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1010))->v,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1028))->v); goto _LL1173; _LL1173:;} if( bad_effect){ goto
_LL769;} if( ! Cyc_Tcutil_same_atts( _temp1018, _temp1000)){ goto _LL769;}
return;}}} _LL799: for( 0; _temp1034 != 0? _temp1038 != 0: 0;( _temp1034=((
struct Cyc_List_List*) _check_null( _temp1034))->tl, _temp1038=(( struct Cyc_List_List*)
_check_null( _temp1038))->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1034))->hd)).f1,(*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1038))->hd)).f1); Cyc_Tcutil_unify_it((*((
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1034))->hd)).f2,(*((
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp1038))->hd)).f2);}
if( _temp1034 == 0? _temp1038 == 0: 0){ return;} goto _LL769; _LL801: _temp1054=
_temp1046; _temp1050= _temp1042; goto _LL803; _LL803: for( 0; _temp1050 != 0?
_temp1054 != 0: 0;( _temp1050=(( struct Cyc_List_List*) _check_null( _temp1050))->tl,
_temp1054=(( struct Cyc_List_List*) _check_null( _temp1054))->tl)){ struct Cyc_Absyn_Structfield*
_temp1194=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1050))->hd; goto _LL1195; _LL1195: { struct Cyc_Absyn_Structfield*
_temp1196=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1054))->hd; goto _LL1197; _LL1197: if( Cyc_String_zstrptrcmp( _temp1194->name,
_temp1196->name) != 0){( void) _throw(( void*) Cyc_Tcutil_Unify);} Cyc_Tcutil_unify_tqual(
_temp1194->tq, _temp1196->tq); Cyc_Tcutil_unify_it(( void*) _temp1194->type,(
void*) _temp1196->type); if((( ! Cyc_Tcutil_same_atts( _temp1194->attributes,
_temp1196->attributes)? 1:( _temp1194->width != 0? _temp1196->width == 0: 0))? 1:(
_temp1196->width != 0? _temp1194->width == 0: 0))? 1:(( _temp1194->width != 0?
_temp1196->width != 0: 0)? Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp1194->width))->v) != Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp1196->width))->v):
0)){( void) _throw(( void*) Cyc_Tcutil_Unify);}}} if( _temp1050 == 0? _temp1054
== 0: 0){ return;} goto _LL769; _LL805: return; _LL807: Cyc_Tcutil_unify_it(
_temp1066, _temp1062); return; _LL809: goto _LL811; _LL811: goto _LL813; _LL813:
goto _LL815; _LL815: if( Cyc_Tcutil_unify_effect( t1, t2)){ return;} else{ goto
_LL769;} _LL817: goto _LL769; _LL769:;}( void) _throw(( void*) Cyc_Tcutil_Unify);}
int Cyc_Tcutil_is_arithmetic_type( void* t){ void* _temp1198= Cyc_Tcutil_compress(
t); void* _temp1210; void* _temp1212; struct Cyc_Absyn_Enumdecl* _temp1214;
struct _tuple1* _temp1216; _LL1200: if(( unsigned int) _temp1198 > 4u?*(( int*)
_temp1198) == Cyc_Absyn_IntType: 0){ _LL1213: _temp1212=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1198)->f1; goto _LL1211; _LL1211: _temp1210=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1198)->f2; goto _LL1201;} else{ goto _LL1202;} _LL1202: if( _temp1198 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1203;} else{ goto _LL1204;} _LL1204: if(
_temp1198 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1205;} else{ goto _LL1206;}
_LL1206: if(( unsigned int) _temp1198 > 4u?*(( int*) _temp1198) == Cyc_Absyn_EnumType:
0){ _LL1217: _temp1216=(( struct Cyc_Absyn_EnumType_struct*) _temp1198)->f1;
goto _LL1215; _LL1215: _temp1214=(( struct Cyc_Absyn_EnumType_struct*) _temp1198)->f2;
goto _LL1207;} else{ goto _LL1208;} _LL1208: goto _LL1209; _LL1201: return 1;
_LL1203: return 1; _LL1205: return 1; _LL1207: return 1; _LL1209: return 0;
_LL1199:;} int Cyc_Tcutil_will_lose_precision( void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple7 _temp1219=({ struct _tuple7
_temp1218; _temp1218.f1= t1; _temp1218.f2= t2; _temp1218;}); void* _temp1241;
void* _temp1243; void* _temp1245; void* _temp1247; void* _temp1249; void*
_temp1251; void* _temp1253; void* _temp1255; void* _temp1257; void* _temp1259;
void* _temp1261; void* _temp1263; void* _temp1265; void* _temp1267; void*
_temp1269; void* _temp1271; void* _temp1273; void* _temp1275; void* _temp1277;
void* _temp1279; void* _temp1281; void* _temp1283; void* _temp1285; void*
_temp1287; void* _temp1289; void* _temp1291; void* _temp1293; void* _temp1295;
void* _temp1297; void* _temp1299; void* _temp1301; void* _temp1303; void*
_temp1305; void* _temp1307; void* _temp1309; void* _temp1311; void* _temp1313;
void* _temp1315; void* _temp1317; void* _temp1319; void* _temp1321; void*
_temp1323; _LL1221: _LL1244: _temp1243= _temp1219.f1; if( _temp1243 ==( void*)
Cyc_Absyn_DoubleType){ goto _LL1242;} else{ goto _LL1223;} _LL1242: _temp1241=
_temp1219.f2; if( _temp1241 ==( void*) Cyc_Absyn_FloatType){ goto _LL1222;}
else{ goto _LL1223;} _LL1223: _LL1252: _temp1251= _temp1219.f1; if( _temp1251 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL1246;} else{ goto _LL1225;} _LL1246:
_temp1245= _temp1219.f2; if(( unsigned int) _temp1245 > 4u?*(( int*) _temp1245)
== Cyc_Absyn_IntType: 0){ _LL1250: _temp1249=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1245)->f1; goto _LL1248; _LL1248: _temp1247=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1245)->f2; goto _LL1224;} else{ goto _LL1225;} _LL1225: _LL1260: _temp1259=
_temp1219.f1; if( _temp1259 ==( void*) Cyc_Absyn_FloatType){ goto _LL1254;}
else{ goto _LL1227;} _LL1254: _temp1253= _temp1219.f2; if(( unsigned int)
_temp1253 > 4u?*(( int*) _temp1253) == Cyc_Absyn_IntType: 0){ _LL1258: _temp1257=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1253)->f1; goto _LL1256; _LL1256:
_temp1255=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1253)->f2; goto
_LL1226;} else{ goto _LL1227;} _LL1227: _LL1268: _temp1267= _temp1219.f1; if((
unsigned int) _temp1267 > 4u?*(( int*) _temp1267) == Cyc_Absyn_IntType: 0){
_LL1272: _temp1271=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1267)->f1;
goto _LL1270; _LL1270: _temp1269=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1267)->f2; if( _temp1269 ==( void*) Cyc_Absyn_B8){ goto _LL1262;} else{
goto _LL1229;}} else{ goto _LL1229;} _LL1262: _temp1261= _temp1219.f2; if((
unsigned int) _temp1261 > 4u?*(( int*) _temp1261) == Cyc_Absyn_IntType: 0){
_LL1266: _temp1265=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1261)->f1;
goto _LL1264; _LL1264: _temp1263=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1261)->f2; if( _temp1263 ==( void*) Cyc_Absyn_B8){ goto _LL1228;} else{
goto _LL1229;}} else{ goto _LL1229;} _LL1229: _LL1276: _temp1275= _temp1219.f1;
if(( unsigned int) _temp1275 > 4u?*(( int*) _temp1275) == Cyc_Absyn_IntType: 0){
_LL1280: _temp1279=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1275)->f1;
goto _LL1278; _LL1278: _temp1277=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1275)->f2; if( _temp1277 ==( void*) Cyc_Absyn_B8){ goto _LL1274;} else{
goto _LL1231;}} else{ goto _LL1231;} _LL1274: _temp1273= _temp1219.f2; goto
_LL1230; _LL1231: _LL1284: _temp1283= _temp1219.f1; if(( unsigned int) _temp1283
> 4u?*(( int*) _temp1283) == Cyc_Absyn_IntType: 0){ _LL1288: _temp1287=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1283)->f1; goto _LL1286; _LL1286:
_temp1285=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1283)->f2; if(
_temp1285 ==( void*) Cyc_Absyn_B4){ goto _LL1282;} else{ goto _LL1233;}} else{
goto _LL1233;} _LL1282: _temp1281= _temp1219.f2; if( _temp1281 ==( void*) Cyc_Absyn_FloatType){
goto _LL1232;} else{ goto _LL1233;} _LL1233: _LL1296: _temp1295= _temp1219.f1;
if(( unsigned int) _temp1295 > 4u?*(( int*) _temp1295) == Cyc_Absyn_IntType: 0){
_LL1300: _temp1299=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1295)->f1;
goto _LL1298; _LL1298: _temp1297=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1295)->f2; if( _temp1297 ==( void*) Cyc_Absyn_B4){ goto _LL1290;} else{
goto _LL1235;}} else{ goto _LL1235;} _LL1290: _temp1289= _temp1219.f2; if((
unsigned int) _temp1289 > 4u?*(( int*) _temp1289) == Cyc_Absyn_IntType: 0){
_LL1294: _temp1293=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1289)->f1;
goto _LL1292; _LL1292: _temp1291=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1289)->f2; if( _temp1291 ==( void*) Cyc_Absyn_B2){ goto _LL1234;} else{
goto _LL1235;}} else{ goto _LL1235;} _LL1235: _LL1308: _temp1307= _temp1219.f1;
if(( unsigned int) _temp1307 > 4u?*(( int*) _temp1307) == Cyc_Absyn_IntType: 0){
_LL1312: _temp1311=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1307)->f1;
goto _LL1310; _LL1310: _temp1309=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1307)->f2; if( _temp1309 ==( void*) Cyc_Absyn_B4){ goto _LL1302;} else{
goto _LL1237;}} else{ goto _LL1237;} _LL1302: _temp1301= _temp1219.f2; if((
unsigned int) _temp1301 > 4u?*(( int*) _temp1301) == Cyc_Absyn_IntType: 0){
_LL1306: _temp1305=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1301)->f1;
goto _LL1304; _LL1304: _temp1303=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1301)->f2; if( _temp1303 ==( void*) Cyc_Absyn_B1){ goto _LL1236;} else{
goto _LL1237;}} else{ goto _LL1237;} _LL1237: _LL1320: _temp1319= _temp1219.f1;
if(( unsigned int) _temp1319 > 4u?*(( int*) _temp1319) == Cyc_Absyn_IntType: 0){
_LL1324: _temp1323=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1319)->f1;
goto _LL1322; _LL1322: _temp1321=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1319)->f2; if( _temp1321 ==( void*) Cyc_Absyn_B2){ goto _LL1314;} else{
goto _LL1239;}} else{ goto _LL1239;} _LL1314: _temp1313= _temp1219.f2; if((
unsigned int) _temp1313 > 4u?*(( int*) _temp1313) == Cyc_Absyn_IntType: 0){
_LL1318: _temp1317=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1313)->f1;
goto _LL1316; _LL1316: _temp1315=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1313)->f2; if( _temp1315 ==( void*) Cyc_Absyn_B1){ goto _LL1238;} else{
goto _LL1239;}} else{ goto _LL1239;} _LL1239: goto _LL1240; _LL1222: return 1;
_LL1224: return 1; _LL1226: return 1; _LL1228: return 0; _LL1230: return 1;
_LL1232: return 1; _LL1234: return 1; _LL1236: return 1; _LL1238: return 1;
_LL1240: return 0; _LL1220:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{
struct Cyc_List_List* el= es; for( 0; el != 0; el=(( struct Cyc_List_List*)
_check_null( el))->tl){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);
if( Cyc_Tcutil_is_arithmetic_type( t1)){ if( max_arith_type == 0? 1: Cyc_Tcutil_will_lose_precision(
t1,( void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){
max_arith_type=({ struct Cyc_Core_Opt* _temp1325=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1325->v=( void*) t1; _temp1325;});}}}}
if( max_arith_type != 0){ if( ! Cyc_Tcutil_unify( t,( void*)(( struct Cyc_Core_Opt*)
_check_null( max_arith_type))->v)){ return 0;}}{ struct Cyc_List_List* el= es;
for( 0; el != 0; el=(( struct Cyc_List_List*) _check_null( el))->tl){ if( ! Cyc_Tcutil_coerce_assign(
te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd, t)){
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
el))->hd)->loc,({ struct _tagged_string _temp1326= Cyc_Absynpp_typ2string( t);
struct _tagged_string _temp1327= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);
xprintf("type mismatch: expecting %.*s but found %.*s", _temp1326.last_plus_one
- _temp1326.curr, _temp1326.curr, _temp1327.last_plus_one - _temp1327.curr,
_temp1327.curr);})); return 0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e)){ void* _temp1328= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); struct Cyc_Absyn_PtrInfo _temp1334; _LL1330: if((
unsigned int) _temp1328 > 4u?*(( int*) _temp1328) == Cyc_Absyn_PointerType: 0){
_LL1335: _temp1334=(( struct Cyc_Absyn_PointerType_struct*) _temp1328)->f1; goto
_LL1331;} else{ goto _LL1332;} _LL1332: goto _LL1333; _LL1331: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); goto _LL1329; _LL1333: return 0; _LL1329:;} return 1;}
int Cyc_Tcutil_is_integral_type( void* t){ void* _temp1336= Cyc_Tcutil_compress(
t); void* _temp1344; void* _temp1346; struct Cyc_Absyn_Enumdecl* _temp1348;
struct _tuple1* _temp1350; _LL1338: if(( unsigned int) _temp1336 > 4u?*(( int*)
_temp1336) == Cyc_Absyn_IntType: 0){ _LL1347: _temp1346=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1336)->f1; goto _LL1345; _LL1345: _temp1344=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1336)->f2; goto _LL1339;} else{ goto _LL1340;} _LL1340: if(( unsigned int)
_temp1336 > 4u?*(( int*) _temp1336) == Cyc_Absyn_EnumType: 0){ _LL1351:
_temp1350=(( struct Cyc_Absyn_EnumType_struct*) _temp1336)->f1; goto _LL1349;
_LL1349: _temp1348=(( struct Cyc_Absyn_EnumType_struct*) _temp1336)->f2; goto
_LL1341;} else{ goto _LL1342;} _LL1342: goto _LL1343; _LL1339: return 1; _LL1341:
return 1; _LL1343: return 0; _LL1337:;} int Cyc_Tcutil_coerce_uint_typ( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_uint_t)){ return 1;}
if( Cyc_Tcutil_is_integral_type(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){
if( Cyc_Tcutil_will_lose_precision(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, Cyc_Absyn_uint_t)){ Cyc_Tcutil_warn( e->loc,({ unsigned char*
_temp1352="integral size mismatch; conversion supplied"; struct _tagged_string
_temp1353; _temp1353.curr= _temp1352; _temp1353.base= _temp1352; _temp1353.last_plus_one=
_temp1352 + 44; _temp1353;}));} Cyc_Tcutil_unchecked_cast( te, e, Cyc_Absyn_uint_t);
return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){ if( Cyc_Tcutil_will_lose_precision((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){
Cyc_Tcutil_warn( e->loc,({ unsigned char* _temp1354="integral size mismatch; conversion supplied";
struct _tagged_string _temp1355; _temp1355.curr= _temp1354; _temp1355.base=
_temp1354; _temp1355.last_plus_one= _temp1354 + 44; _temp1355;}));} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); return 1;} return 0;} int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple7
_temp1357=({ struct _tuple7 _temp1356; _temp1356.f1= t1; _temp1356.f2= t2;
_temp1356;}); void* _temp1369; struct Cyc_Absyn_PtrInfo _temp1371; void*
_temp1373; struct Cyc_Absyn_PtrInfo _temp1375; void* _temp1377; struct Cyc_Absyn_Exp*
_temp1379; struct Cyc_Absyn_Tqual _temp1381; void* _temp1383; void* _temp1385;
struct Cyc_Absyn_Exp* _temp1387; struct Cyc_Absyn_Tqual _temp1389; void*
_temp1391; void* _temp1393; struct Cyc_Absyn_TunionInfo _temp1395; void*
_temp1397; struct Cyc_List_List* _temp1399; void* _temp1401; struct Cyc_Absyn_Tuniondecl*
_temp1403; void* _temp1405; struct Cyc_Absyn_TunionFieldInfo _temp1407; struct
Cyc_List_List* _temp1409; void* _temp1411; struct Cyc_Absyn_Tunionfield*
_temp1413; struct Cyc_Absyn_Tuniondecl* _temp1415; void* _temp1417; struct Cyc_Absyn_TunionInfo
_temp1419; void* _temp1421; struct Cyc_List_List* _temp1423; void* _temp1425;
struct Cyc_Absyn_Tuniondecl* _temp1427; void* _temp1429; struct Cyc_Absyn_PtrInfo
_temp1431; struct Cyc_Absyn_Conref* _temp1433; struct Cyc_Absyn_Tqual _temp1435;
struct Cyc_Absyn_Conref* _temp1437; void* _temp1439; void* _temp1441; _LL1359:
_LL1374: _temp1373= _temp1357.f1; if(( unsigned int) _temp1373 > 4u?*(( int*)
_temp1373) == Cyc_Absyn_PointerType: 0){ _LL1376: _temp1375=(( struct Cyc_Absyn_PointerType_struct*)
_temp1373)->f1; goto _LL1370;} else{ goto _LL1361;} _LL1370: _temp1369=
_temp1357.f2; if(( unsigned int) _temp1369 > 4u?*(( int*) _temp1369) == Cyc_Absyn_PointerType:
0){ _LL1372: _temp1371=(( struct Cyc_Absyn_PointerType_struct*) _temp1369)->f1;
goto _LL1360;} else{ goto _LL1361;} _LL1361: _LL1386: _temp1385= _temp1357.f1;
if(( unsigned int) _temp1385 > 4u?*(( int*) _temp1385) == Cyc_Absyn_ArrayType: 0){
_LL1392: _temp1391=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1385)->f1;
goto _LL1390; _LL1390: _temp1389=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1385)->f2; goto _LL1388; _LL1388: _temp1387=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1385)->f3; goto _LL1378;} else{ goto _LL1363;} _LL1378: _temp1377=
_temp1357.f2; if(( unsigned int) _temp1377 > 4u?*(( int*) _temp1377) == Cyc_Absyn_ArrayType:
0){ _LL1384: _temp1383=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1377)->f1;
goto _LL1382; _LL1382: _temp1381=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1377)->f2; goto _LL1380; _LL1380: _temp1379=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1377)->f3; goto _LL1362;} else{ goto _LL1363;} _LL1363: _LL1406: _temp1405=
_temp1357.f1; if(( unsigned int) _temp1405 > 4u?*(( int*) _temp1405) == Cyc_Absyn_TunionFieldType:
0){ _LL1408: _temp1407=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1405)->f1;
_LL1412: _temp1411=( void*) _temp1407.field_info; if(*(( int*) _temp1411) == Cyc_Absyn_KnownTunionfield){
_LL1416: _temp1415=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1411)->f1;
goto _LL1414; _LL1414: _temp1413=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1411)->f2; goto _LL1410;} else{ goto _LL1365;} _LL1410: _temp1409=
_temp1407.targs; goto _LL1394;} else{ goto _LL1365;} _LL1394: _temp1393=
_temp1357.f2; if(( unsigned int) _temp1393 > 4u?*(( int*) _temp1393) == Cyc_Absyn_TunionType:
0){ _LL1396: _temp1395=(( struct Cyc_Absyn_TunionType_struct*) _temp1393)->f1;
_LL1402: _temp1401=( void*) _temp1395.tunion_info; if(*(( int*) _temp1401) ==
Cyc_Absyn_KnownTunion){ _LL1404: _temp1403=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1401)->f1; goto _LL1400;} else{ goto _LL1365;} _LL1400: _temp1399=
_temp1395.targs; goto _LL1398; _LL1398: _temp1397=( void*) _temp1395.rgn; goto
_LL1364;} else{ goto _LL1365;} _LL1365: _LL1430: _temp1429= _temp1357.f1; if((
unsigned int) _temp1429 > 4u?*(( int*) _temp1429) == Cyc_Absyn_PointerType: 0){
_LL1432: _temp1431=(( struct Cyc_Absyn_PointerType_struct*) _temp1429)->f1;
_LL1442: _temp1441=( void*) _temp1431.elt_typ; goto _LL1440; _LL1440: _temp1439=(
void*) _temp1431.rgn_typ; goto _LL1438; _LL1438: _temp1437= _temp1431.nullable;
goto _LL1436; _LL1436: _temp1435= _temp1431.tq; goto _LL1434; _LL1434: _temp1433=
_temp1431.bounds; goto _LL1418;} else{ goto _LL1367;} _LL1418: _temp1417=
_temp1357.f2; if(( unsigned int) _temp1417 > 4u?*(( int*) _temp1417) == Cyc_Absyn_TunionType:
0){ _LL1420: _temp1419=(( struct Cyc_Absyn_TunionType_struct*) _temp1417)->f1;
_LL1426: _temp1425=( void*) _temp1419.tunion_info; if(*(( int*) _temp1425) ==
Cyc_Absyn_KnownTunion){ _LL1428: _temp1427=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1425)->f1; goto _LL1424;} else{ goto _LL1367;} _LL1424: _temp1423=
_temp1419.targs; goto _LL1422; _LL1422: _temp1421=( void*) _temp1419.rgn; goto
_LL1366;} else{ goto _LL1367;} _LL1367: goto _LL1368; _LL1360: { int okay= 1;
if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1375.nullable, _temp1371.nullable)){
void* _temp1443=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp1375.nullable))->v; int _temp1449; _LL1445:
if(( unsigned int) _temp1443 > 1u?*(( int*) _temp1443) == Cyc_Absyn_Eq_constr: 0){
_LL1450: _temp1449=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1443)->f1;
goto _LL1446;} else{ goto _LL1447;} _LL1447: goto _LL1448; _LL1446: okay= !
_temp1449; goto _LL1444; _LL1448:( void) _throw((( void*(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp1451="silent_castable conref not eq";
struct _tagged_string _temp1452; _temp1452.curr= _temp1451; _temp1452.base=
_temp1451; _temp1452.last_plus_one= _temp1451 + 30; _temp1452;}))); _LL1444:;}
if( !(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct
Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp, _temp1375.bounds,
_temp1371.bounds)){ struct _tuple7 _temp1454=({ struct _tuple7 _temp1453;
_temp1453.f1=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp1375.bounds))->v; _temp1453.f2=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp1371.bounds))->v; _temp1453;}); void* _temp1462; void* _temp1464; void*
_temp1466; void* _temp1468; struct Cyc_Absyn_Exp* _temp1470; void* _temp1472;
void* _temp1474; void* _temp1476; void* _temp1478; _LL1456: _LL1467: _temp1466=
_temp1454.f1; if(( unsigned int) _temp1466 > 1u?*(( int*) _temp1466) == Cyc_Absyn_Eq_constr:
0){ _LL1469: _temp1468=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1466)->f1;
if(( unsigned int) _temp1468 > 1u?*(( int*) _temp1468) == Cyc_Absyn_Upper_b: 0){
_LL1471: _temp1470=(( struct Cyc_Absyn_Upper_b_struct*) _temp1468)->f1; goto
_LL1463;} else{ goto _LL1458;}} else{ goto _LL1458;} _LL1463: _temp1462=
_temp1454.f2; if(( unsigned int) _temp1462 > 1u?*(( int*) _temp1462) == Cyc_Absyn_Eq_constr:
0){ _LL1465: _temp1464=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1462)->f1;
if( _temp1464 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1457;} else{ goto _LL1458;}}
else{ goto _LL1458;} _LL1458: _LL1477: _temp1476= _temp1454.f1; if((
unsigned int) _temp1476 > 1u?*(( int*) _temp1476) == Cyc_Absyn_Eq_constr: 0){
_LL1479: _temp1478=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1476)->f1;
if( _temp1478 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1473;} else{ goto _LL1460;}}
else{ goto _LL1460;} _LL1473: _temp1472= _temp1454.f2; if(( unsigned int)
_temp1472 > 1u?*(( int*) _temp1472) == Cyc_Absyn_Eq_constr: 0){ _LL1475:
_temp1474=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1472)->f1; if(
_temp1474 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1459;} else{ goto _LL1460;}}
else{ goto _LL1460;} _LL1460: goto _LL1461; _LL1457: okay= 1; goto _LL1455;
_LL1459: okay= 1; goto _LL1455; _LL1461: okay= 0; goto _LL1455; _LL1455:;}
return(( okay? Cyc_Tcutil_unify(( void*) _temp1375.elt_typ,( void*) _temp1371.elt_typ):
0)? Cyc_Tcutil_unify(( void*) _temp1375.rgn_typ,( void*) _temp1371.rgn_typ): 0)?
!( _temp1375.tq).q_const? 1:( _temp1371.tq).q_const: 0;} _LL1362: { int okay;
okay=( _temp1387 != 0? _temp1379 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1387)) == Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1379)): 0; return( okay? Cyc_Tcutil_unify(
_temp1391, _temp1383): 0)? ! _temp1389.q_const? 1: _temp1381.q_const: 0;}
_LL1364: if(( _temp1415 == _temp1403? 1: Cyc_Absyn_qvar_cmp( _temp1415->name,
_temp1403->name) == 0)? _temp1413->typs == 0: 0){ for( 0; _temp1409 != 0?
_temp1399 != 0: 0;( _temp1409=(( struct Cyc_List_List*) _check_null( _temp1409))->tl,
_temp1399=(( struct Cyc_List_List*) _check_null( _temp1399))->tl)){ if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_List_List*) _check_null( _temp1409))->hd,( void*)(( struct
Cyc_List_List*) _check_null( _temp1399))->hd)){ break;}} if( _temp1409 == 0?
_temp1399 == 0: 0){ return 1;}} return 0; _LL1366:{ void* _temp1480= Cyc_Tcutil_compress(
_temp1441); struct Cyc_Absyn_TunionFieldInfo _temp1486; struct Cyc_List_List*
_temp1488; void* _temp1490; struct Cyc_Absyn_Tunionfield* _temp1492; struct Cyc_Absyn_Tuniondecl*
_temp1494; _LL1482: if(( unsigned int) _temp1480 > 4u?*(( int*) _temp1480) ==
Cyc_Absyn_TunionFieldType: 0){ _LL1487: _temp1486=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1480)->f1; _LL1491: _temp1490=( void*) _temp1486.field_info; if(*(( int*)
_temp1490) == Cyc_Absyn_KnownTunionfield){ _LL1495: _temp1494=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1490)->f1; goto _LL1493; _LL1493: _temp1492=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1490)->f2; goto _LL1489;} else{ goto _LL1484;} _LL1489: _temp1488=
_temp1486.targs; goto _LL1483;} else{ goto _LL1484;} _LL1484: goto _LL1485;
_LL1483: if( ! Cyc_Tcutil_unify( _temp1439, _temp1421)? _temp1439 !=( void*) Cyc_Absyn_HeapRgn:
0){ return 0;} if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1437,(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)( 0))){
return 0;} if( !(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp,
_temp1433,(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)( Cyc_Absyn_bounds_one))){
return 0;} if( Cyc_Absyn_qvar_cmp( _temp1427->name, _temp1494->name) == 0?
_temp1492->typs != 0: 0){ int okay= 1; for( 0; _temp1488 != 0? _temp1423 != 0: 0;(
_temp1488=(( struct Cyc_List_List*) _check_null( _temp1488))->tl, _temp1423=((
struct Cyc_List_List*) _check_null( _temp1423))->tl)){ if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_List_List*) _check_null( _temp1488))->hd,( void*)(( struct
Cyc_List_List*) _check_null( _temp1423))->hd)){ okay= 0; break;}} if(( ! okay? 1:
_temp1488 != 0)? 1: _temp1423 != 0){ return 0;} return 1;} goto _LL1481; _LL1485:
goto _LL1481; _LL1481:;} return 0; _LL1368: return Cyc_Tcutil_unify( t1, t2);
_LL1358:;}} int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} if( Cyc_Tcutil_is_arithmetic_type(
t1)? Cyc_Tcutil_is_arithmetic_type( t2): 0){ if( Cyc_Tcutil_will_lose_precision(
t1, t2)){ Cyc_Tcutil_warn( e->loc,({ unsigned char* _temp1496="integral size mismatch; conversion supplied";
struct _tagged_string _temp1497; _temp1497.curr= _temp1496; _temp1497.base=
_temp1496; _temp1497.last_plus_one= _temp1496 + 44; _temp1497;}));} Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){
Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2); Cyc_Tcutil_warn( e->loc,({
struct _tagged_string _temp1498= Cyc_Absynpp_typ2string( t1); struct
_tagged_string _temp1499= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1498.last_plus_one - _temp1498.curr, _temp1498.curr, _temp1499.last_plus_one
- _temp1499.curr, _temp1499.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg(
te, e, t);} static int Cyc_Tcutil_any_promote( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e){ void* old_typ= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); void* t;{ void* _temp1500= old_typ; struct Cyc_Absyn_Enumdecl*
_temp1512; struct _tuple1* _temp1514; void* _temp1516; void* _temp1518; _LL1502:
if(( unsigned int) _temp1500 > 4u?*(( int*) _temp1500) == Cyc_Absyn_EnumType: 0){
_LL1515: _temp1514=(( struct Cyc_Absyn_EnumType_struct*) _temp1500)->f1; goto
_LL1513; _LL1513: _temp1512=(( struct Cyc_Absyn_EnumType_struct*) _temp1500)->f2;
goto _LL1503;} else{ goto _LL1504;} _LL1504: if(( unsigned int) _temp1500 > 4u?*((
int*) _temp1500) == Cyc_Absyn_IntType: 0){ _LL1519: _temp1518=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1500)->f1; goto _LL1517; _LL1517: _temp1516=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1500)->f2; goto _LL1505;} else{
goto _LL1506;} _LL1506: if( _temp1500 ==( void*) Cyc_Absyn_FloatType){ goto
_LL1507;} else{ goto _LL1508;} _LL1508: if( _temp1500 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1509;} else{ goto _LL1510;} _LL1510: goto _LL1511; _LL1503: t= Cyc_Absyn_sint_t;
goto _LL1501; _LL1505:{ void* _temp1520= _temp1516; _LL1522: if( _temp1520 ==(
void*) Cyc_Absyn_B1){ goto _LL1523;} else{ goto _LL1524;} _LL1524: if( _temp1520
==( void*) Cyc_Absyn_B2){ goto _LL1525;} else{ goto _LL1526;} _LL1526: if(
_temp1520 ==( void*) Cyc_Absyn_B4){ goto _LL1527;} else{ goto _LL1528;} _LL1528:
if( _temp1520 ==( void*) Cyc_Absyn_B8){ goto _LL1529;} else{ goto _LL1521;}
_LL1523: t= Cyc_Absyn_sint_t; goto _LL1521; _LL1525: t= Cyc_Absyn_sint_t; goto
_LL1521; _LL1527: t= _temp1518 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL1521; _LL1529: t= _temp1518 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_ulong_t: Cyc_Absyn_slong_t; goto _LL1521; _LL1521:;} goto _LL1501;
_LL1507: t= Cyc_Absyn_float_t; goto _LL1501; _LL1509: t= Cyc_Absyn_double_t;
goto _LL1501; _LL1511: return 0; _LL1501:;} Cyc_Tcutil_unchecked_cast( te, e, t);
return 1;} int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} Cyc_Tcutil_any_promote(
te, e); if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v,
t2)){ return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); Cyc_Tcutil_warn( e->loc,({ struct _tagged_string _temp1530= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp1531= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1530.last_plus_one - _temp1530.curr, _temp1530.curr, _temp1531.last_plus_one
- _temp1531.curr, _temp1531.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerceable(
void* t){ void* _temp1532= Cyc_Tcutil_compress( t); void* _temp1542; void*
_temp1544; _LL1534: if(( unsigned int) _temp1532 > 4u?*(( int*) _temp1532) ==
Cyc_Absyn_IntType: 0){ _LL1545: _temp1544=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1532)->f1; goto _LL1543; _LL1543: _temp1542=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1532)->f2; goto _LL1535;} else{ goto _LL1536;} _LL1536: if( _temp1532 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1537;} else{ goto _LL1538;} _LL1538: if(
_temp1532 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1539;} else{ goto _LL1540;}
_LL1540: goto _LL1541; _LL1535: return 1; _LL1537: return 1; _LL1539: return 1;
_LL1541: return 0; _LL1533:;} static struct _tuple4* Cyc_Tcutil_flatten_typ_f(
struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield* x){ return({ struct
_tuple4* _temp1546=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1546->f1= x->tq; _temp1546->f2= Cyc_Tcutil_substitute( inst,( void*) x->type);
_temp1546;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ( struct Cyc_Tcenv_Tenv*
te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void* _temp1547= t1; struct Cyc_List_List*
_temp1559; struct Cyc_Absyn_Structdecl** _temp1561; struct Cyc_List_List*
_temp1563; struct _tuple1* _temp1565; struct Cyc_List_List* _temp1567; _LL1549:
if( _temp1547 ==( void*) Cyc_Absyn_VoidType){ goto _LL1550;} else{ goto _LL1551;}
_LL1551: if(( unsigned int) _temp1547 > 4u?*(( int*) _temp1547) == Cyc_Absyn_TupleType:
0){ _LL1560: _temp1559=(( struct Cyc_Absyn_TupleType_struct*) _temp1547)->f1;
goto _LL1552;} else{ goto _LL1553;} _LL1553: if(( unsigned int) _temp1547 > 4u?*((
int*) _temp1547) == Cyc_Absyn_StructType: 0){ _LL1566: _temp1565=(( struct Cyc_Absyn_StructType_struct*)
_temp1547)->f1; goto _LL1564; _LL1564: _temp1563=(( struct Cyc_Absyn_StructType_struct*)
_temp1547)->f2; goto _LL1562; _LL1562: _temp1561=(( struct Cyc_Absyn_StructType_struct*)
_temp1547)->f3; goto _LL1554;} else{ goto _LL1555;} _LL1555: if(( unsigned int)
_temp1547 > 4u?*(( int*) _temp1547) == Cyc_Absyn_AnonStructType: 0){ _LL1568:
_temp1567=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1547)->f1; goto
_LL1556;} else{ goto _LL1557;} _LL1557: goto _LL1558; _LL1550: return 0; _LL1552:
return _temp1559; _LL1554: if( _temp1561 == 0? 1: _temp1565 == 0){ return({
struct Cyc_List_List* _temp1569=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1569->hd=( void*)({ struct _tuple4* _temp1570=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1570->f1= Cyc_Absyn_empty_tqual();
_temp1570->f2= t1; _temp1570;}); _temp1569->tl= 0; _temp1569;});}{ struct Cyc_Absyn_Structdecl*
_temp1571=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp1561)); goto
_LL1572; _LL1572: { struct _tuple1 _temp1573=*(( struct _tuple1*) _check_null(
_temp1565)); goto _LL1574; _LL1574: { struct Cyc_List_List* _temp1575=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1571->tvs, _temp1563); goto _LL1576; _LL1576: if( _temp1571->fields == 0){
return({ struct Cyc_List_List* _temp1577=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1577->hd=( void*)({ struct _tuple4*
_temp1578=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1578->f1=
Cyc_Absyn_empty_tqual(); _temp1578->f2= t1; _temp1578;}); _temp1577->tl= 0;
_temp1577;});} return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, _temp1575,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1571->fields))->v);}}}
_LL1556: return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, 0, _temp1567); _LL1558: return({
struct Cyc_List_List* _temp1579=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1579->hd=( void*)({ struct _tuple4* _temp1580=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1580->f1= Cyc_Absyn_empty_tqual();
_temp1580->f2= t1; _temp1580;}); _temp1579->tl= 0; _temp1579;}); _LL1548:;}}
static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2); static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;}{ struct Cyc_List_List* a= assume; for( 0; a != 0; a=(( struct
Cyc_List_List*) _check_null( a))->tl){ if( Cyc_Tcutil_unify( t1,(*(( struct
_tuple7*)(( struct Cyc_List_List*) _check_null( a))->hd)).f1)? Cyc_Tcutil_unify(
t2,(*(( struct _tuple7*)(( struct Cyc_List_List*) _check_null( a))->hd)).f2): 0){
return 1;}}} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct
_tuple7 _temp1582=({ struct _tuple7 _temp1581; _temp1581.f1= t1; _temp1581.f2=
t2; _temp1581;}); void* _temp1588; struct Cyc_Absyn_PtrInfo _temp1590; struct
Cyc_Absyn_Conref* _temp1592; struct Cyc_Absyn_Tqual _temp1594; struct Cyc_Absyn_Conref*
_temp1596; void* _temp1598; void* _temp1600; void* _temp1602; struct Cyc_Absyn_PtrInfo
_temp1604; struct Cyc_Absyn_Conref* _temp1606; struct Cyc_Absyn_Tqual _temp1608;
struct Cyc_Absyn_Conref* _temp1610; void* _temp1612; void* _temp1614; _LL1584:
_LL1603: _temp1602= _temp1582.f1; if(( unsigned int) _temp1602 > 4u?*(( int*)
_temp1602) == Cyc_Absyn_PointerType: 0){ _LL1605: _temp1604=(( struct Cyc_Absyn_PointerType_struct*)
_temp1602)->f1; _LL1615: _temp1614=( void*) _temp1604.elt_typ; goto _LL1613;
_LL1613: _temp1612=( void*) _temp1604.rgn_typ; goto _LL1611; _LL1611: _temp1610=
_temp1604.nullable; goto _LL1609; _LL1609: _temp1608= _temp1604.tq; goto _LL1607;
_LL1607: _temp1606= _temp1604.bounds; goto _LL1589;} else{ goto _LL1586;}
_LL1589: _temp1588= _temp1582.f2; if(( unsigned int) _temp1588 > 4u?*(( int*)
_temp1588) == Cyc_Absyn_PointerType: 0){ _LL1591: _temp1590=(( struct Cyc_Absyn_PointerType_struct*)
_temp1588)->f1; _LL1601: _temp1600=( void*) _temp1590.elt_typ; goto _LL1599;
_LL1599: _temp1598=( void*) _temp1590.rgn_typ; goto _LL1597; _LL1597: _temp1596=
_temp1590.nullable; goto _LL1595; _LL1595: _temp1594= _temp1590.tq; goto _LL1593;
_LL1593: _temp1592= _temp1590.bounds; goto _LL1585;} else{ goto _LL1586;}
_LL1586: goto _LL1587; _LL1585: if( _temp1608.q_const? ! _temp1594.q_const: 0){
return 0;} if(( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1610, _temp1596)?(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1610): 0)? !(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1596): 0){ return 0;}{ void* _temp1616= _temp1612; _LL1618: if( _temp1616
==( void*) Cyc_Absyn_HeapRgn){ goto _LL1619;} else{ goto _LL1620;} _LL1620: goto
_LL1621; _LL1619: goto _LL1617; _LL1621: Cyc_Tcutil_unify( _temp1612, _temp1598);
goto _LL1617; _LL1617:;} if( !(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp,
_temp1606, _temp1592)){ struct _tuple7 _temp1623=({ struct _tuple7 _temp1622;
_temp1622.f1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1606); _temp1622.f2=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1592); _temp1622;}); void* _temp1631; void* _temp1633; struct Cyc_Absyn_Exp*
_temp1635; void* _temp1637; struct Cyc_Absyn_Exp* _temp1639; void* _temp1641;
struct Cyc_Absyn_Exp* _temp1643; _LL1625: _LL1634: _temp1633= _temp1623.f1; if((
unsigned int) _temp1633 > 1u?*(( int*) _temp1633) == Cyc_Absyn_Upper_b: 0){
_LL1636: _temp1635=(( struct Cyc_Absyn_Upper_b_struct*) _temp1633)->f1; goto
_LL1632;} else{ goto _LL1627;} _LL1632: _temp1631= _temp1623.f2; if( _temp1631
==( void*) Cyc_Absyn_Unknown_b){ goto _LL1626;} else{ goto _LL1627;} _LL1627:
_LL1642: _temp1641= _temp1623.f1; if(( unsigned int) _temp1641 > 1u?*(( int*)
_temp1641) == Cyc_Absyn_Upper_b: 0){ _LL1644: _temp1643=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1641)->f1; goto _LL1638;} else{ goto _LL1629;} _LL1638: _temp1637=
_temp1623.f2; if(( unsigned int) _temp1637 > 1u?*(( int*) _temp1637) == Cyc_Absyn_Upper_b:
0){ _LL1640: _temp1639=(( struct Cyc_Absyn_Upper_b_struct*) _temp1637)->f1; goto
_LL1628;} else{ goto _LL1629;} _LL1629: goto _LL1630; _LL1626: goto _LL1624;
_LL1628: if( Cyc_Evexp_eval_const_uint_exp( _temp1643) < Cyc_Evexp_eval_const_uint_exp(
_temp1639)){ return 0;} goto _LL1624; _LL1630: return 0; _LL1624:;} return Cyc_Tcutil_ptrsubtype(
te,({ struct Cyc_List_List* _temp1645=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1645->hd=( void*)({ struct _tuple7*
_temp1646=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp1646->f1=
t1; _temp1646->f2= t2; _temp1646;}); _temp1645->tl= assume; _temp1645;}),
_temp1614, _temp1600); _LL1587: return 0; _LL1583:;}} static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* assume, void* t1, void* t2){
struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ( te, t1); struct Cyc_List_List*
tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0; tqs2 != 0;( tqs2=(( struct Cyc_List_List*)
_check_null( tqs2))->tl, tqs1=(( struct Cyc_List_List*) _check_null( tqs1))->tl)){
if( tqs1 == 0){ return 0;}{ struct _tuple4 _temp1649; void* _temp1650; struct
Cyc_Absyn_Tqual _temp1652; struct _tuple4* _temp1647=( struct _tuple4*)(( struct
Cyc_List_List*) _check_null( tqs1))->hd; _temp1649=* _temp1647; _LL1653:
_temp1652= _temp1649.f1; goto _LL1651; _LL1651: _temp1650= _temp1649.f2; goto
_LL1648; _LL1648: { struct _tuple4 _temp1656; void* _temp1657; struct Cyc_Absyn_Tqual
_temp1659; struct _tuple4* _temp1654=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( tqs2))->hd; _temp1656=* _temp1654; _LL1660: _temp1659= _temp1656.f1;
goto _LL1658; _LL1658: _temp1657= _temp1656.f2; goto _LL1655; _LL1655: if(
_temp1659.q_const? Cyc_Tcutil_subtype( te, assume, _temp1650, _temp1657): 0){
continue;} else{ if( Cyc_Tcutil_unify( _temp1650, _temp1657)){ continue;} else{
return 0;}}}}} return 1;} int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
void* _temp1661= t2; struct Cyc_Absyn_Uniondecl** _temp1669; struct Cyc_List_List*
_temp1671; struct _tuple1* _temp1673; void* _temp1675; void* _temp1677; _LL1663:
if(( unsigned int) _temp1661 > 4u?*(( int*) _temp1661) == Cyc_Absyn_UnionType: 0){
_LL1674: _temp1673=(( struct Cyc_Absyn_UnionType_struct*) _temp1661)->f1; goto
_LL1672; _LL1672: _temp1671=(( struct Cyc_Absyn_UnionType_struct*) _temp1661)->f2;
goto _LL1670; _LL1670: _temp1669=(( struct Cyc_Absyn_UnionType_struct*)
_temp1661)->f3; goto _LL1664;} else{ goto _LL1665;} _LL1665: if(( unsigned int)
_temp1661 > 4u?*(( int*) _temp1661) == Cyc_Absyn_IntType: 0){ _LL1678: _temp1677=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1661)->f1; goto _LL1676; _LL1676:
_temp1675=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1661)->f2; if(
_temp1675 ==( void*) Cyc_Absyn_B4){ goto _LL1666;} else{ goto _LL1667;}} else{
goto _LL1667;} _LL1667: goto _LL1668; _LL1664: if( _temp1669 == 0){ goto _LL1662;}{
struct Cyc_Absyn_Uniondecl* _temp1679=*(( struct Cyc_Absyn_Uniondecl**)
_check_null( _temp1669)); goto _LL1680; _LL1680: if( _temp1679->fields == 0){
goto _LL1662;}{ struct Cyc_List_List* _temp1681=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp1679->tvs,
_temp1671); goto _LL1682; _LL1682:{ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1679->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ if( Cyc_Tcutil_unify( t1, Cyc_Tcutil_substitute(
_temp1681,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ return 1;}}} goto _LL1662;}} _LL1666: if( Cyc_Tcutil_typ_kind(
t2) ==( void*) Cyc_Absyn_BoxKind){ return 1;} goto _LL1662; _LL1668: goto
_LL1662; _LL1662:;}{ void* _temp1683= t1; struct Cyc_Absyn_PtrInfo _temp1699;
struct Cyc_Absyn_Conref* _temp1701; struct Cyc_Absyn_Tqual _temp1703; struct Cyc_Absyn_Conref*
_temp1705; void* _temp1707; void* _temp1709; struct Cyc_Absyn_Exp* _temp1711;
struct Cyc_Absyn_Tqual _temp1713; void* _temp1715; struct Cyc_Absyn_Enumdecl*
_temp1717; struct _tuple1* _temp1719; void* _temp1721; void* _temp1723; _LL1685:
if(( unsigned int) _temp1683 > 4u?*(( int*) _temp1683) == Cyc_Absyn_PointerType:
0){ _LL1700: _temp1699=(( struct Cyc_Absyn_PointerType_struct*) _temp1683)->f1;
_LL1710: _temp1709=( void*) _temp1699.elt_typ; goto _LL1708; _LL1708: _temp1707=(
void*) _temp1699.rgn_typ; goto _LL1706; _LL1706: _temp1705= _temp1699.nullable;
goto _LL1704; _LL1704: _temp1703= _temp1699.tq; goto _LL1702; _LL1702: _temp1701=
_temp1699.bounds; goto _LL1686;} else{ goto _LL1687;} _LL1687: if(( unsigned int)
_temp1683 > 4u?*(( int*) _temp1683) == Cyc_Absyn_ArrayType: 0){ _LL1716:
_temp1715=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1683)->f1; goto
_LL1714; _LL1714: _temp1713=(( struct Cyc_Absyn_ArrayType_struct*) _temp1683)->f2;
goto _LL1712; _LL1712: _temp1711=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1683)->f3; goto _LL1688;} else{ goto _LL1689;} _LL1689: if(( unsigned int)
_temp1683 > 4u?*(( int*) _temp1683) == Cyc_Absyn_EnumType: 0){ _LL1720:
_temp1719=(( struct Cyc_Absyn_EnumType_struct*) _temp1683)->f1; goto _LL1718;
_LL1718: _temp1717=(( struct Cyc_Absyn_EnumType_struct*) _temp1683)->f2; goto
_LL1690;} else{ goto _LL1691;} _LL1691: if(( unsigned int) _temp1683 > 4u?*((
int*) _temp1683) == Cyc_Absyn_IntType: 0){ _LL1724: _temp1723=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1683)->f1; goto _LL1722; _LL1722: _temp1721=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1683)->f2; goto _LL1692;} else{
goto _LL1693;} _LL1693: if( _temp1683 ==( void*) Cyc_Absyn_FloatType){ goto
_LL1694;} else{ goto _LL1695;} _LL1695: if( _temp1683 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1696;} else{ goto _LL1697;} _LL1697: goto _LL1698; _LL1686:{ void*
_temp1725= t2; struct Cyc_Absyn_PtrInfo _temp1731; struct Cyc_Absyn_Conref*
_temp1733; struct Cyc_Absyn_Tqual _temp1735; struct Cyc_Absyn_Conref* _temp1737;
void* _temp1739; void* _temp1741; _LL1727: if(( unsigned int) _temp1725 > 4u?*((
int*) _temp1725) == Cyc_Absyn_PointerType: 0){ _LL1732: _temp1731=(( struct Cyc_Absyn_PointerType_struct*)
_temp1725)->f1; _LL1742: _temp1741=( void*) _temp1731.elt_typ; goto _LL1740;
_LL1740: _temp1739=( void*) _temp1731.rgn_typ; goto _LL1738; _LL1738: _temp1737=
_temp1731.nullable; goto _LL1736; _LL1736: _temp1735= _temp1731.tq; goto _LL1734;
_LL1734: _temp1733= _temp1731.bounds; goto _LL1728;} else{ goto _LL1729;}
_LL1729: goto _LL1730; _LL1728: { struct Cyc_List_List* _temp1745=({ struct Cyc_List_List*
_temp1743=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1743->hd=( void*)({ struct _tuple7* _temp1744=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp1744->f1= t1; _temp1744->f2= t2; _temp1744;});
_temp1743->tl= 0; _temp1743;}); goto _LL1746; _LL1746: { int _temp1747= Cyc_Tcutil_ptrsubtype(
te, _temp1745, _temp1709, _temp1741)? ! _temp1703.q_const? 1: _temp1735.q_const:
0; goto _LL1748; _LL1748: { int bounds_ok=(( int(*)( int(* cmp)( void*, void*),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp1701, _temp1733); if( ! bounds_ok){ struct _tuple7
_temp1750=({ struct _tuple7 _temp1749; _temp1749.f1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp1701); _temp1749.f2=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp1733); _temp1749;}); void* _temp1756; struct Cyc_Absyn_Exp*
_temp1758; void* _temp1760; struct Cyc_Absyn_Exp* _temp1762; _LL1752: _LL1761:
_temp1760= _temp1750.f1; if(( unsigned int) _temp1760 > 1u?*(( int*) _temp1760)
== Cyc_Absyn_Upper_b: 0){ _LL1763: _temp1762=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1760)->f1; goto _LL1757;} else{ goto _LL1754;} _LL1757: _temp1756=
_temp1750.f2; if(( unsigned int) _temp1756 > 1u?*(( int*) _temp1756) == Cyc_Absyn_Upper_b:
0){ _LL1759: _temp1758=(( struct Cyc_Absyn_Upper_b_struct*) _temp1756)->f1; goto
_LL1753;} else{ goto _LL1754;} _LL1754: goto _LL1755; _LL1753: if( Cyc_Evexp_eval_const_uint_exp(
_temp1762) >= Cyc_Evexp_eval_const_uint_exp( _temp1758)){ bounds_ok= 1;} goto
_LL1751; _LL1755: bounds_ok= 1; goto _LL1751; _LL1751:;} return( bounds_ok?
_temp1747: 0)? _temp1707 ==( void*) Cyc_Absyn_HeapRgn? 1: Cyc_Tcutil_unify(
_temp1707, _temp1739): 0;}}} _LL1730: goto _LL1726; _LL1726:;} return 0; _LL1688:{
void* _temp1764= t2; struct Cyc_Absyn_Exp* _temp1770; struct Cyc_Absyn_Tqual
_temp1772; void* _temp1774; _LL1766: if(( unsigned int) _temp1764 > 4u?*(( int*)
_temp1764) == Cyc_Absyn_ArrayType: 0){ _LL1775: _temp1774=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1764)->f1; goto _LL1773; _LL1773: _temp1772=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1764)->f2; goto _LL1771; _LL1771: _temp1770=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1764)->f3; goto _LL1767;} else{ goto _LL1768;} _LL1768: goto _LL1769;
_LL1767: { int okay; okay=( _temp1711 != 0? _temp1770 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1711)) >= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1770)): 0; return( okay? Cyc_Tcutil_unify(
_temp1715, _temp1774): 0)? ! _temp1713.q_const? 1: _temp1772.q_const: 0;}
_LL1769: return 0; _LL1765:;} return 0; _LL1690:{ void* _temp1776= t2; struct
Cyc_Absyn_Enumdecl* _temp1782; struct _tuple1* _temp1784; _LL1778: if((
unsigned int) _temp1776 > 4u?*(( int*) _temp1776) == Cyc_Absyn_EnumType: 0){
_LL1785: _temp1784=(( struct Cyc_Absyn_EnumType_struct*) _temp1776)->f1; goto
_LL1783; _LL1783: _temp1782=(( struct Cyc_Absyn_EnumType_struct*) _temp1776)->f2;
goto _LL1779;} else{ goto _LL1780;} _LL1780: goto _LL1781; _LL1779: if((((
struct Cyc_Absyn_Enumdecl*) _check_null( _temp1717))->fields != 0?(( struct Cyc_Absyn_Enumdecl*)
_check_null( _temp1782))->fields != 0: 0)?(( int(*)( struct Cyc_List_List* x))
Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(((
struct Cyc_Absyn_Enumdecl*) _check_null( _temp1717))->fields))->v) >=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Enumdecl*) _check_null( _temp1782))->fields))->v):
0){ return 1;} goto _LL1777; _LL1781: goto _LL1777; _LL1777:;} goto _LL1692;
_LL1692: goto _LL1694; _LL1694: goto _LL1696; _LL1696: return Cyc_Tcutil_coerceable(
t2); _LL1698: return 0; _LL1684:;}} void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, void* t){ if( ! Cyc_Tcutil_unify(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v, t)){ struct Cyc_Absyn_Exp* _temp1786=
Cyc_Absyn_copy_exp( e); goto _LL1787; _LL1787:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Cast_e_struct* _temp1788=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp1788[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp1789; _temp1789.tag= Cyc_Absyn_Cast_e; _temp1789.f1=( void*) t; _temp1789.f2=
_temp1786; _temp1789;}); _temp1788;}))); e->topt=({ struct Cyc_Core_Opt*
_temp1790=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1790->v=( void*) t; _temp1790;});}} int Cyc_Tcutil_is_integral( struct Cyc_Absyn_Exp*
e){ void* _temp1791= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); struct Cyc_Absyn_Enumdecl* _temp1801; struct _tuple1*
_temp1803; void* _temp1805; void* _temp1807; int _temp1809; struct Cyc_Core_Opt*
_temp1811; void* _temp1813; _LL1793: if(( unsigned int) _temp1791 > 4u?*(( int*)
_temp1791) == Cyc_Absyn_EnumType: 0){ _LL1804: _temp1803=(( struct Cyc_Absyn_EnumType_struct*)
_temp1791)->f1; goto _LL1802; _LL1802: _temp1801=(( struct Cyc_Absyn_EnumType_struct*)
_temp1791)->f2; goto _LL1794;} else{ goto _LL1795;} _LL1795: if(( unsigned int)
_temp1791 > 4u?*(( int*) _temp1791) == Cyc_Absyn_IntType: 0){ _LL1808: _temp1807=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1791)->f1; goto _LL1806; _LL1806:
_temp1805=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1791)->f2; goto
_LL1796;} else{ goto _LL1797;} _LL1797: if(( unsigned int) _temp1791 > 4u?*((
int*) _temp1791) == Cyc_Absyn_Evar: 0){ _LL1814: _temp1813=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp1791)->f1; goto _LL1812; _LL1812: _temp1811=(( struct Cyc_Absyn_Evar_struct*)
_temp1791)->f2; goto _LL1810; _LL1810: _temp1809=(( struct Cyc_Absyn_Evar_struct*)
_temp1791)->f3; goto _LL1798;} else{ goto _LL1799;} _LL1799: goto _LL1800;
_LL1794: goto _LL1796; _LL1796: return 1; _LL1798: if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;}
return 0; _LL1800: return 0; _LL1792:;} int Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp*
e){ if( Cyc_Tcutil_is_integral( e)){ return 1;}{ void* _temp1815= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _LL1817: if(
_temp1815 ==( void*) Cyc_Absyn_FloatType){ goto _LL1818;} else{ goto _LL1819;}
_LL1819: if( _temp1815 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1820;} else{
goto _LL1821;} _LL1821: goto _LL1822; _LL1818: goto _LL1820; _LL1820: return 1;
_LL1822: return 0; _LL1816:;}} int Cyc_Tcutil_is_function_type( void* t){ void*
_temp1823= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp1829; _LL1825:
if(( unsigned int) _temp1823 > 4u?*(( int*) _temp1823) == Cyc_Absyn_FnType: 0){
_LL1830: _temp1829=(( struct Cyc_Absyn_FnType_struct*) _temp1823)->f1; goto
_LL1826;} else{ goto _LL1827;} _LL1827: goto _LL1828; _LL1826: return 1; _LL1828:
return 0; _LL1824:;} void* Cyc_Tcutil_max_arithmetic_type( void* t1, void* t2){
struct _tuple7 _temp1832=({ struct _tuple7 _temp1831; _temp1831.f1= t1;
_temp1831.f2= t2; _temp1831;}); void* _temp1856; void* _temp1858; void*
_temp1860; void* _temp1862; void* _temp1864; void* _temp1866; void* _temp1868;
void* _temp1870; void* _temp1872; void* _temp1874; void* _temp1876; void*
_temp1878; void* _temp1880; void* _temp1882; void* _temp1884; void* _temp1886;
void* _temp1888; void* _temp1890; void* _temp1892; void* _temp1894; void*
_temp1896; void* _temp1898; void* _temp1900; void* _temp1902; void* _temp1904;
void* _temp1906; void* _temp1908; void* _temp1910; void* _temp1912; void*
_temp1914; void* _temp1916; void* _temp1918; _LL1834: _LL1859: _temp1858=
_temp1832.f1; if( _temp1858 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1857;}
else{ goto _LL1836;} _LL1857: _temp1856= _temp1832.f2; goto _LL1835; _LL1836:
_LL1863: _temp1862= _temp1832.f1; goto _LL1861; _LL1861: _temp1860= _temp1832.f2;
if( _temp1860 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1837;} else{ goto
_LL1838;} _LL1838: _LL1867: _temp1866= _temp1832.f1; if( _temp1866 ==( void*)
Cyc_Absyn_FloatType){ goto _LL1865;} else{ goto _LL1840;} _LL1865: _temp1864=
_temp1832.f2; goto _LL1839; _LL1840: _LL1871: _temp1870= _temp1832.f1; goto
_LL1869; _LL1869: _temp1868= _temp1832.f2; if( _temp1868 ==( void*) Cyc_Absyn_FloatType){
goto _LL1841;} else{ goto _LL1842;} _LL1842: _LL1875: _temp1874= _temp1832.f1;
if(( unsigned int) _temp1874 > 4u?*(( int*) _temp1874) == Cyc_Absyn_IntType: 0){
_LL1879: _temp1878=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1874)->f1;
if( _temp1878 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1877;} else{ goto _LL1844;}
_LL1877: _temp1876=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1874)->f2;
if( _temp1876 ==( void*) Cyc_Absyn_B8){ goto _LL1873;} else{ goto _LL1844;}}
else{ goto _LL1844;} _LL1873: _temp1872= _temp1832.f2; goto _LL1843; _LL1844:
_LL1887: _temp1886= _temp1832.f1; goto _LL1881; _LL1881: _temp1880= _temp1832.f2;
if(( unsigned int) _temp1880 > 4u?*(( int*) _temp1880) == Cyc_Absyn_IntType: 0){
_LL1885: _temp1884=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1880)->f1;
if( _temp1884 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1883;} else{ goto _LL1846;}
_LL1883: _temp1882=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1880)->f2;
if( _temp1882 ==( void*) Cyc_Absyn_B8){ goto _LL1845;} else{ goto _LL1846;}}
else{ goto _LL1846;} _LL1846: _LL1891: _temp1890= _temp1832.f1; if((
unsigned int) _temp1890 > 4u?*(( int*) _temp1890) == Cyc_Absyn_IntType: 0){
_LL1895: _temp1894=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1890)->f1;
if( _temp1894 ==( void*) Cyc_Absyn_Signed){ goto _LL1893;} else{ goto _LL1848;}
_LL1893: _temp1892=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1890)->f2;
if( _temp1892 ==( void*) Cyc_Absyn_B8){ goto _LL1889;} else{ goto _LL1848;}}
else{ goto _LL1848;} _LL1889: _temp1888= _temp1832.f2; goto _LL1847; _LL1848:
_LL1903: _temp1902= _temp1832.f1; goto _LL1897; _LL1897: _temp1896= _temp1832.f2;
if(( unsigned int) _temp1896 > 4u?*(( int*) _temp1896) == Cyc_Absyn_IntType: 0){
_LL1901: _temp1900=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1896)->f1;
if( _temp1900 ==( void*) Cyc_Absyn_Signed){ goto _LL1899;} else{ goto _LL1850;}
_LL1899: _temp1898=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1896)->f2;
if( _temp1898 ==( void*) Cyc_Absyn_B8){ goto _LL1849;} else{ goto _LL1850;}}
else{ goto _LL1850;} _LL1850: _LL1907: _temp1906= _temp1832.f1; if((
unsigned int) _temp1906 > 4u?*(( int*) _temp1906) == Cyc_Absyn_IntType: 0){
_LL1911: _temp1910=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1906)->f1;
if( _temp1910 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1909;} else{ goto _LL1852;}
_LL1909: _temp1908=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1906)->f2;
if( _temp1908 ==( void*) Cyc_Absyn_B4){ goto _LL1905;} else{ goto _LL1852;}}
else{ goto _LL1852;} _LL1905: _temp1904= _temp1832.f2; goto _LL1851; _LL1852:
_LL1919: _temp1918= _temp1832.f1; goto _LL1913; _LL1913: _temp1912= _temp1832.f2;
if(( unsigned int) _temp1912 > 4u?*(( int*) _temp1912) == Cyc_Absyn_IntType: 0){
_LL1917: _temp1916=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1912)->f1;
if( _temp1916 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1915;} else{ goto _LL1854;}
_LL1915: _temp1914=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1912)->f2;
if( _temp1914 ==( void*) Cyc_Absyn_B4){ goto _LL1853;} else{ goto _LL1854;}}
else{ goto _LL1854;} _LL1854: goto _LL1855; _LL1835: goto _LL1837; _LL1837:
return( void*) Cyc_Absyn_DoubleType; _LL1839: goto _LL1841; _LL1841: return(
void*) Cyc_Absyn_FloatType; _LL1843: goto _LL1845; _LL1845: return Cyc_Absyn_ulong_t;
_LL1847: goto _LL1849; _LL1849: return Cyc_Absyn_slong_t; _LL1851: goto _LL1853;
_LL1853: return Cyc_Absyn_uint_t; _LL1855: return Cyc_Absyn_sint_t; _LL1833:;}
void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp* e){ void* _temp1920=(
void*) e->r; struct Cyc_Absyn_Exp* _temp1926; struct Cyc_Core_Opt* _temp1928;
struct Cyc_Absyn_Exp* _temp1930; _LL1922: if(*(( int*) _temp1920) == Cyc_Absyn_AssignOp_e){
_LL1931: _temp1930=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp1920)->f1; goto
_LL1929; _LL1929: _temp1928=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp1920)->f2;
if( _temp1928 == 0){ goto _LL1927;} else{ goto _LL1924;} _LL1927: _temp1926=((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1920)->f3; goto _LL1923;} else{ goto
_LL1924;} _LL1924: goto _LL1925; _LL1923: Cyc_Tcutil_warn( e->loc,({
unsigned char* _temp1932="assignment in test"; struct _tagged_string _temp1933;
_temp1933.curr= _temp1932; _temp1933.base= _temp1932; _temp1933.last_plus_one=
_temp1932 + 19; _temp1933;})); goto _LL1921; _LL1925: goto _LL1921; _LL1921:;}
static int Cyc_Tcutil_constrain_kinds( struct Cyc_Absyn_Conref* c1, struct Cyc_Absyn_Conref*
c2){ c1=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
c1); c2=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
c2); if( c1 == c2){ return 1;} if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr){(
void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1934=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1934[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp1935; _temp1935.tag= Cyc_Absyn_Forward_constr; _temp1935.f1= c2; _temp1935;});
_temp1934;}))); return 1;} else{ if(( void*) c2->v ==( void*) Cyc_Absyn_No_constr){(
void*)( c2->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1936=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1936[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp1937; _temp1937.tag= Cyc_Absyn_Forward_constr; _temp1937.f1= c1; _temp1937;});
_temp1936;}))); return 1;} else{ void* k1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( c1); void* k2=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( c2); if( Cyc_Tcutil_kind_leq( k1, k2)){( void*)( c2->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp1938=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp1938[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp1939; _temp1939.tag= Cyc_Absyn_Forward_constr;
_temp1939.f1= c1; _temp1939;}); _temp1938;}))); return 1;} else{ if( Cyc_Tcutil_kind_leq(
k2, k1)){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1940=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1940[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp1941; _temp1941.tag= Cyc_Absyn_Forward_constr; _temp1941.f1= c2; _temp1941;});
_temp1940;}))); return 1;} else{ return 0;}}}}} static struct Cyc_List_List* Cyc_Tcutil_add_free_tvar(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar*
tv){ if( tvs == 0){ return({ struct Cyc_List_List* _temp1942=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1942->hd=( void*) tv; _temp1942->tl=
0; _temp1942;});} else{{ struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 != 0;
tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){ if( Cyc_String_zstrptrcmp(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->name,
tv->name) == 0){ struct Cyc_Absyn_Conref* k1=(( struct Cyc_Absyn_Tvar*)(( struct
Cyc_List_List*) _check_null( tvs2))->hd)->kind; struct Cyc_Absyn_Conref* k2= tv->kind;
if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp1943=* tv->name; struct _tagged_string _temp1944= Cyc_Absynpp_ckind2string(
k1); struct _tagged_string _temp1945= Cyc_Absynpp_ckind2string( k2); xprintf("type variable %.*s is used with inconsistent kinds %.*s and %.*s",
_temp1943.last_plus_one - _temp1943.curr, _temp1943.curr, _temp1944.last_plus_one
- _temp1944.curr, _temp1944.curr, _temp1945.last_plus_one - _temp1945.curr,
_temp1945.curr);}));} return tvs;} else{ if((( struct Cyc_List_List*)
_check_null( tvs2))->tl == 0){(( struct Cyc_List_List*) _check_null( tvs2))->tl=({
struct Cyc_List_List* _temp1946=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1946->hd=( void*) tv; _temp1946->tl= 0; _temp1946;});
return tvs;}}}} return(( struct Cyc_List_List*(*)( struct _tagged_string msg))
Cyc_Tcutil_impos)(({ unsigned char* _temp1947="Tcutil::add_free_tvar:expecting to have returned.";
struct _tagged_string _temp1948; _temp1948.curr= _temp1947; _temp1948.base=
_temp1947; _temp1948.last_plus_one= _temp1947 + 50; _temp1948;}));}} static
struct Cyc_List_List* Cyc_Tcutil_remove_bound_tvars( struct Cyc_List_List* tvs,
struct Cyc_List_List* btvs){ struct Cyc_List_List* r= 0; for( 0; tvs != 0; tvs=((
struct Cyc_List_List*) _check_null( tvs))->tl){ int present= 0;{ struct Cyc_List_List*
b= btvs; for( 0; b != 0; b=(( struct Cyc_List_List*) _check_null( b))->tl){ if(
Cyc_String_zstrptrcmp((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd)->name,(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( b))->hd)->name) == 0){ present= 1; break;}}} if( ! present){ r=({
struct Cyc_List_List* _temp1949=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1949->hd=( void*)(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd); _temp1949->tl= r; _temp1949;});}}
r=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( r);
return r;} static int Cyc_Tcutil_tvar_counter= 0; struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar(
void* k){ struct _tagged_string s= xprintf("#%d", Cyc_Tcutil_tvar_counter ++);
return({ struct Cyc_Absyn_Tvar* _temp1950=( struct Cyc_Absyn_Tvar*) GC_malloc(
sizeof( struct Cyc_Absyn_Tvar)); _temp1950->name=({ struct _tagged_string*
_temp1951=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp1951[ 0]= s; _temp1951;}); _temp1950->kind=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)( k); _temp1950;});} int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar* t){ struct _tagged_string _temp1952=* t->name; goto
_LL1953; _LL1953: return*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( _temp1952, sizeof(
unsigned char), 0) =='#';} void Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar*
t){ if( ! Cyc_Tcutil_is_temp_tvar( t)){ return;}{ struct _tagged_string
_temp1956= Cyc_String_strconcat(({ unsigned char* _temp1954="`"; struct
_tagged_string _temp1955; _temp1955.curr= _temp1954; _temp1955.base= _temp1954;
_temp1955.last_plus_one= _temp1954 + 2; _temp1955;}),* t->name); goto _LL1957;
_LL1957:*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( _temp1956, sizeof( unsigned char), 1)='t'; t->name=({
struct _tagged_string* _temp1958=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp1958[ 0]= _temp1956; _temp1958;});}} struct
_tuple12{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ;
static struct _tuple2* Cyc_Tcutil_fndecl2typ_f( struct _tuple12* x){ return({
struct _tuple2* _temp1959=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp1959->f1=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp1960=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1960->v=( void*)(*
x).f1; _temp1960;}); _temp1959->f2=(* x).f2; _temp1959->f3=(* x).f3; _temp1959;});}
void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl* fd){ if( fd->cached_typ ==
0){ struct Cyc_List_List* _temp1961= 0; goto _LL1962; _LL1962:{ struct Cyc_List_List*
atts= fd->attributes; for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ if( Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd)){ _temp1961=({ struct Cyc_List_List* _temp1963=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1963->hd=( void*)((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd); _temp1963->tl=
_temp1961; _temp1963;});}}} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp1964=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1964[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1965; _temp1965.tag= Cyc_Absyn_FnType;
_temp1965.f1=({ struct Cyc_Absyn_FnInfo _temp1966; _temp1966.tvars= fd->tvs;
_temp1966.effect= fd->effect; _temp1966.ret_typ=( void*)(( void*) fd->ret_type);
_temp1966.args=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct
_tuple12*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_fndecl2typ_f, fd->args);
_temp1966.c_varargs= fd->c_varargs; _temp1966.cyc_varargs= fd->cyc_varargs;
_temp1966.attributes= _temp1961; _temp1966;}); _temp1965;}); _temp1964;});}
return( void*)(( struct Cyc_Core_Opt*) _check_null( fd->cached_typ))->v;} struct
_tuple13{ void* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; static void* Cyc_Tcutil_fst_fdarg(
struct _tuple13* t){ return(* t).f1;} void* Cyc_Tcutil_snd_tqt( struct _tuple4*
t){ return(* t).f2;} static struct _tuple4* Cyc_Tcutil_map2_tq( struct _tuple4*
pr, void* t){ return({ struct _tuple4* _temp1967=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1967->f1=(* pr).f1; _temp1967->f2= t; _temp1967;});}
void Cyc_Tcutil_check_fndecl_valid_type( struct Cyc_Position_Segment* loc,
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl* fd){ void* t= Cyc_Tcutil_fndecl2typ(
fd); Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);{ void* _temp1968= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_FnInfo _temp1974; struct Cyc_List_List* _temp1976; struct
Cyc_Absyn_VarargInfo* _temp1978; int _temp1980; struct Cyc_List_List* _temp1982;
void* _temp1984; struct Cyc_Core_Opt* _temp1986; struct Cyc_List_List* _temp1988;
_LL1970: if(( unsigned int) _temp1968 > 4u?*(( int*) _temp1968) == Cyc_Absyn_FnType:
0){ _LL1975: _temp1974=(( struct Cyc_Absyn_FnType_struct*) _temp1968)->f1;
_LL1989: _temp1988= _temp1974.tvars; goto _LL1987; _LL1987: _temp1986= _temp1974.effect;
goto _LL1985; _LL1985: _temp1984=( void*) _temp1974.ret_typ; goto _LL1983;
_LL1983: _temp1982= _temp1974.args; goto _LL1981; _LL1981: _temp1980= _temp1974.c_varargs;
goto _LL1979; _LL1979: _temp1978= _temp1974.cyc_varargs; goto _LL1977; _LL1977:
_temp1976= _temp1974.attributes; goto _LL1971;} else{ goto _LL1972;} _LL1972:
goto _LL1973; _LL1971: fd->tvs= _temp1988; fd->effect= _temp1986; goto _LL1969;
_LL1973:(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
unsigned char* _temp1990="check_fndecl_valid_type: not a FnType"; struct
_tagged_string _temp1991; _temp1991.curr= _temp1990; _temp1991.base= _temp1990;
_temp1991.last_plus_one= _temp1990 + 38; _temp1991;})); return; _LL1969:;}{
struct _RegionHandle _temp1992= _new_region(); struct _RegionHandle* r=&
_temp1992; _push_region( r); Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tagged_string*(* f)( struct _tuple12*), struct
Cyc_List_List* x)) Cyc_List_rmap)( r,( struct _tagged_string*(*)( struct
_tuple12* t)) Cyc_Tcutil_fst_fdarg, fd->args), loc,({ unsigned char* _temp1993="function declaration has repeated parameter";
struct _tagged_string _temp1994; _temp1994.curr= _temp1993; _temp1994.base=
_temp1993; _temp1994.last_plus_one= _temp1993 + 44; _temp1994;}));; _pop_region(&
_temp1992);} fd->cached_typ=({ struct Cyc_Core_Opt* _temp1995=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1995->v=( void*) t; _temp1995;});}
struct _tuple14{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; } ; struct
_tuple15{ struct _tuple14* f1; void* f2; } ; static struct _tuple15* Cyc_Tcutil_substitute_f1(
struct _RegionHandle* rgn, struct _tuple2* y){ return({ struct _tuple15*
_temp1996=( struct _tuple15*) _region_malloc( rgn, sizeof( struct _tuple15));
_temp1996->f1=({ struct _tuple14* _temp1997=( struct _tuple14*) _region_malloc(
rgn, sizeof( struct _tuple14)); _temp1997->f1=(* y).f1; _temp1997->f2=(* y).f2;
_temp1997;}); _temp1996->f2=(* y).f3; _temp1996;});} static struct _tuple2* Cyc_Tcutil_substitute_f2(
struct _tuple15* w){ void* _temp2000; struct _tuple14* _temp2002; struct
_tuple15 _temp1998=* w; _LL2003: _temp2002= _temp1998.f1; goto _LL2001; _LL2001:
_temp2000= _temp1998.f2; goto _LL1999; _LL1999: { struct Cyc_Absyn_Tqual
_temp2006; struct Cyc_Core_Opt* _temp2008; struct _tuple14 _temp2004=* _temp2002;
_LL2009: _temp2008= _temp2004.f1; goto _LL2007; _LL2007: _temp2006= _temp2004.f2;
goto _LL2005; _LL2005: return({ struct _tuple2* _temp2010=( struct _tuple2*)
GC_malloc( sizeof( struct _tuple2)); _temp2010->f1= _temp2008; _temp2010->f2=
_temp2006; _temp2010->f3= _temp2000; _temp2010;});}} static void* Cyc_Tcutil_field_type(
struct Cyc_Absyn_Structfield* f){ return( void*) f->type;} static struct Cyc_Absyn_Structfield*
Cyc_Tcutil_zip_field_type( struct Cyc_Absyn_Structfield* f, void* t){ return({
struct Cyc_Absyn_Structfield* _temp2011=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2011->name= f->name;
_temp2011->tq= f->tq; _temp2011->type=( void*) t; _temp2011->width= f->width;
_temp2011->attributes= f->attributes; _temp2011;});} static struct Cyc_List_List*
Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List* inst, struct
Cyc_List_List* ts); void* Cyc_Tcutil_rsubstitute( struct _RegionHandle* rgn,
struct Cyc_List_List* inst, void* t){ void* _temp2012= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp2058; struct Cyc_Absyn_Structdecl** _temp2060;
struct Cyc_List_List* _temp2062; struct _tuple1* _temp2064; struct Cyc_Absyn_Uniondecl**
_temp2066; struct Cyc_List_List* _temp2068; struct _tuple1* _temp2070; struct
Cyc_Absyn_TunionInfo _temp2072; void* _temp2074; struct Cyc_List_List* _temp2076;
void* _temp2078; struct Cyc_Absyn_TunionFieldInfo _temp2080; struct Cyc_List_List*
_temp2082; void* _temp2084; struct Cyc_Core_Opt* _temp2086; struct Cyc_List_List*
_temp2088; struct _tuple1* _temp2090; struct Cyc_Absyn_Exp* _temp2092; struct
Cyc_Absyn_Tqual _temp2094; void* _temp2096; struct Cyc_Absyn_PtrInfo _temp2098;
struct Cyc_Absyn_Conref* _temp2100; struct Cyc_Absyn_Tqual _temp2102; struct Cyc_Absyn_Conref*
_temp2104; void* _temp2106; void* _temp2108; struct Cyc_Absyn_FnInfo _temp2110;
struct Cyc_List_List* _temp2112; struct Cyc_Absyn_VarargInfo* _temp2114; int
_temp2116; struct Cyc_List_List* _temp2118; void* _temp2120; struct Cyc_Core_Opt*
_temp2122; struct Cyc_List_List* _temp2124; struct Cyc_List_List* _temp2126;
struct Cyc_List_List* _temp2128; struct Cyc_List_List* _temp2130; int _temp2132;
struct Cyc_Core_Opt* _temp2134; void* _temp2136; void* _temp2138; struct Cyc_Absyn_Enumdecl*
_temp2140; struct _tuple1* _temp2142; void* _temp2144; void* _temp2146; void*
_temp2148; struct Cyc_List_List* _temp2150; _LL2014: if(( unsigned int)
_temp2012 > 4u?*(( int*) _temp2012) == Cyc_Absyn_VarType: 0){ _LL2059: _temp2058=((
struct Cyc_Absyn_VarType_struct*) _temp2012)->f1; goto _LL2015;} else{ goto
_LL2016;} _LL2016: if(( unsigned int) _temp2012 > 4u?*(( int*) _temp2012) == Cyc_Absyn_StructType:
0){ _LL2065: _temp2064=(( struct Cyc_Absyn_StructType_struct*) _temp2012)->f1;
goto _LL2063; _LL2063: _temp2062=(( struct Cyc_Absyn_StructType_struct*)
_temp2012)->f2; goto _LL2061; _LL2061: _temp2060=(( struct Cyc_Absyn_StructType_struct*)
_temp2012)->f3; goto _LL2017;} else{ goto _LL2018;} _LL2018: if(( unsigned int)
_temp2012 > 4u?*(( int*) _temp2012) == Cyc_Absyn_UnionType: 0){ _LL2071:
_temp2070=(( struct Cyc_Absyn_UnionType_struct*) _temp2012)->f1; goto _LL2069;
_LL2069: _temp2068=(( struct Cyc_Absyn_UnionType_struct*) _temp2012)->f2; goto
_LL2067; _LL2067: _temp2066=(( struct Cyc_Absyn_UnionType_struct*) _temp2012)->f3;
goto _LL2019;} else{ goto _LL2020;} _LL2020: if(( unsigned int) _temp2012 > 4u?*((
int*) _temp2012) == Cyc_Absyn_TunionType: 0){ _LL2073: _temp2072=(( struct Cyc_Absyn_TunionType_struct*)
_temp2012)->f1; _LL2079: _temp2078=( void*) _temp2072.tunion_info; goto _LL2077;
_LL2077: _temp2076= _temp2072.targs; goto _LL2075; _LL2075: _temp2074=( void*)
_temp2072.rgn; goto _LL2021;} else{ goto _LL2022;} _LL2022: if(( unsigned int)
_temp2012 > 4u?*(( int*) _temp2012) == Cyc_Absyn_TunionFieldType: 0){ _LL2081:
_temp2080=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2012)->f1; _LL2085:
_temp2084=( void*) _temp2080.field_info; goto _LL2083; _LL2083: _temp2082=
_temp2080.targs; goto _LL2023;} else{ goto _LL2024;} _LL2024: if(( unsigned int)
_temp2012 > 4u?*(( int*) _temp2012) == Cyc_Absyn_TypedefType: 0){ _LL2091:
_temp2090=(( struct Cyc_Absyn_TypedefType_struct*) _temp2012)->f1; goto _LL2089;
_LL2089: _temp2088=(( struct Cyc_Absyn_TypedefType_struct*) _temp2012)->f2; goto
_LL2087; _LL2087: _temp2086=(( struct Cyc_Absyn_TypedefType_struct*) _temp2012)->f3;
goto _LL2025;} else{ goto _LL2026;} _LL2026: if(( unsigned int) _temp2012 > 4u?*((
int*) _temp2012) == Cyc_Absyn_ArrayType: 0){ _LL2097: _temp2096=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2012)->f1; goto _LL2095; _LL2095:
_temp2094=(( struct Cyc_Absyn_ArrayType_struct*) _temp2012)->f2; goto _LL2093;
_LL2093: _temp2092=(( struct Cyc_Absyn_ArrayType_struct*) _temp2012)->f3; goto
_LL2027;} else{ goto _LL2028;} _LL2028: if(( unsigned int) _temp2012 > 4u?*((
int*) _temp2012) == Cyc_Absyn_PointerType: 0){ _LL2099: _temp2098=(( struct Cyc_Absyn_PointerType_struct*)
_temp2012)->f1; _LL2109: _temp2108=( void*) _temp2098.elt_typ; goto _LL2107;
_LL2107: _temp2106=( void*) _temp2098.rgn_typ; goto _LL2105; _LL2105: _temp2104=
_temp2098.nullable; goto _LL2103; _LL2103: _temp2102= _temp2098.tq; goto _LL2101;
_LL2101: _temp2100= _temp2098.bounds; goto _LL2029;} else{ goto _LL2030;}
_LL2030: if(( unsigned int) _temp2012 > 4u?*(( int*) _temp2012) == Cyc_Absyn_FnType:
0){ _LL2111: _temp2110=(( struct Cyc_Absyn_FnType_struct*) _temp2012)->f1;
_LL2125: _temp2124= _temp2110.tvars; goto _LL2123; _LL2123: _temp2122= _temp2110.effect;
goto _LL2121; _LL2121: _temp2120=( void*) _temp2110.ret_typ; goto _LL2119;
_LL2119: _temp2118= _temp2110.args; goto _LL2117; _LL2117: _temp2116= _temp2110.c_varargs;
goto _LL2115; _LL2115: _temp2114= _temp2110.cyc_varargs; goto _LL2113; _LL2113:
_temp2112= _temp2110.attributes; goto _LL2031;} else{ goto _LL2032;} _LL2032:
if(( unsigned int) _temp2012 > 4u?*(( int*) _temp2012) == Cyc_Absyn_TupleType: 0){
_LL2127: _temp2126=(( struct Cyc_Absyn_TupleType_struct*) _temp2012)->f1; goto
_LL2033;} else{ goto _LL2034;} _LL2034: if(( unsigned int) _temp2012 > 4u?*((
int*) _temp2012) == Cyc_Absyn_AnonStructType: 0){ _LL2129: _temp2128=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp2012)->f1; goto _LL2035;} else{ goto
_LL2036;} _LL2036: if(( unsigned int) _temp2012 > 4u?*(( int*) _temp2012) == Cyc_Absyn_AnonUnionType:
0){ _LL2131: _temp2130=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp2012)->f1;
goto _LL2037;} else{ goto _LL2038;} _LL2038: if(( unsigned int) _temp2012 > 4u?*((
int*) _temp2012) == Cyc_Absyn_Evar: 0){ _LL2137: _temp2136=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp2012)->f1; goto _LL2135; _LL2135: _temp2134=(( struct Cyc_Absyn_Evar_struct*)
_temp2012)->f2; goto _LL2133; _LL2133: _temp2132=(( struct Cyc_Absyn_Evar_struct*)
_temp2012)->f3; goto _LL2039;} else{ goto _LL2040;} _LL2040: if(( unsigned int)
_temp2012 > 4u?*(( int*) _temp2012) == Cyc_Absyn_RgnHandleType: 0){ _LL2139:
_temp2138=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp2012)->f1;
goto _LL2041;} else{ goto _LL2042;} _LL2042: if(( unsigned int) _temp2012 > 4u?*((
int*) _temp2012) == Cyc_Absyn_EnumType: 0){ _LL2143: _temp2142=(( struct Cyc_Absyn_EnumType_struct*)
_temp2012)->f1; goto _LL2141; _LL2141: _temp2140=(( struct Cyc_Absyn_EnumType_struct*)
_temp2012)->f2; goto _LL2043;} else{ goto _LL2044;} _LL2044: if( _temp2012 ==(
void*) Cyc_Absyn_VoidType){ goto _LL2045;} else{ goto _LL2046;} _LL2046: if((
unsigned int) _temp2012 > 4u?*(( int*) _temp2012) == Cyc_Absyn_IntType: 0){
_LL2147: _temp2146=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2012)->f1;
goto _LL2145; _LL2145: _temp2144=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2012)->f2; goto _LL2047;} else{ goto _LL2048;} _LL2048: if( _temp2012 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2049;} else{ goto _LL2050;} _LL2050: if(
_temp2012 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2051;} else{ goto _LL2052;}
_LL2052: if( _temp2012 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL2053;} else{ goto
_LL2054;} _LL2054: if(( unsigned int) _temp2012 > 4u?*(( int*) _temp2012) == Cyc_Absyn_AccessEff:
0){ _LL2149: _temp2148=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp2012)->f1;
goto _LL2055;} else{ goto _LL2056;} _LL2056: if(( unsigned int) _temp2012 > 4u?*((
int*) _temp2012) == Cyc_Absyn_JoinEff: 0){ _LL2151: _temp2150=(( struct Cyc_Absyn_JoinEff_struct*)
_temp2012)->f1; goto _LL2057;} else{ goto _LL2013;} _LL2015: { struct
_handler_cons _temp2152; _push_handler(& _temp2152);{ int _temp2154= 0; if(
setjmp( _temp2152.handler)){ _temp2154= 1;} if( ! _temp2154){{ void* _temp2155=((
void*(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp,
inst, _temp2058); _npop_handler( 0u); return _temp2155;}; _pop_handler();} else{
void* _temp2153=( void*) _exn_thrown; void* _temp2157= _temp2153; _LL2159: if(
_temp2157 == Cyc_Core_Not_found){ goto _LL2160;} else{ goto _LL2161;} _LL2161:
goto _LL2162; _LL2160: return t; _LL2162:( void) _throw( _temp2157); _LL2158:;}}}
_LL2017: { struct Cyc_List_List* _temp2163= Cyc_Tcutil_substs( rgn, inst,
_temp2062); goto _LL2164; _LL2164: return _temp2163 == _temp2062? t:( void*)({
struct Cyc_Absyn_StructType_struct* _temp2165=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp2165[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp2166; _temp2166.tag= Cyc_Absyn_StructType;
_temp2166.f1= _temp2064; _temp2166.f2= _temp2163; _temp2166.f3= _temp2060;
_temp2166;}); _temp2165;});} _LL2019: { struct Cyc_List_List* _temp2167= Cyc_Tcutil_substs(
rgn, inst, _temp2068); goto _LL2168; _LL2168: return _temp2167 == _temp2068? t:(
void*)({ struct Cyc_Absyn_UnionType_struct* _temp2169=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp2169[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp2170; _temp2170.tag= Cyc_Absyn_UnionType;
_temp2170.f1= _temp2070; _temp2170.f2= _temp2167; _temp2170.f3= _temp2066;
_temp2170;}); _temp2169;});} _LL2021: { struct Cyc_List_List* _temp2171= Cyc_Tcutil_substs(
rgn, inst, _temp2076); goto _LL2172; _LL2172: { void* _temp2173= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2074); goto _LL2174; _LL2174: return( _temp2171 == _temp2076?
_temp2173 == _temp2074: 0)? t:( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp2175=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp2175[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp2176; _temp2176.tag=
Cyc_Absyn_TunionType; _temp2176.f1=({ struct Cyc_Absyn_TunionInfo _temp2177;
_temp2177.tunion_info=( void*) _temp2078; _temp2177.targs= _temp2171; _temp2177.rgn=(
void*) _temp2173; _temp2177;}); _temp2176;}); _temp2175;});}} _LL2023: { struct
Cyc_List_List* _temp2178= Cyc_Tcutil_substs( rgn, inst, _temp2082); goto _LL2179;
_LL2179: return _temp2178 == _temp2082? t:( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp2180=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp2180[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp2181; _temp2181.tag= Cyc_Absyn_TunionFieldType; _temp2181.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2182; _temp2182.field_info=( void*) _temp2084; _temp2182.targs= _temp2178;
_temp2182;}); _temp2181;}); _temp2180;});} _LL2025: { struct Cyc_List_List*
_temp2183= Cyc_Tcutil_substs( rgn, inst, _temp2088); goto _LL2184; _LL2184:
return _temp2183 == _temp2088? t:( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp2185=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2185[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp2186; _temp2186.tag=
Cyc_Absyn_TypedefType; _temp2186.f1= _temp2090; _temp2186.f2= _temp2183;
_temp2186.f3= _temp2086; _temp2186;}); _temp2185;});} _LL2027: { void* _temp2187=
Cyc_Tcutil_rsubstitute( rgn, inst, _temp2096); goto _LL2188; _LL2188: return
_temp2187 == _temp2096? t:( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp2189=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp2189[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp2190; _temp2190.tag= Cyc_Absyn_ArrayType;
_temp2190.f1=( void*) _temp2187; _temp2190.f2= _temp2094; _temp2190.f3=
_temp2092; _temp2190;}); _temp2189;});} _LL2029: { void* _temp2191= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2108); goto _LL2192; _LL2192: { void* _temp2193= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2106); goto _LL2194; _LL2194: if( _temp2191 == _temp2108?
_temp2193 == _temp2106: 0){ return t;} return( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp2195=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp2195[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp2196; _temp2196.tag=
Cyc_Absyn_PointerType; _temp2196.f1=({ struct Cyc_Absyn_PtrInfo _temp2197;
_temp2197.elt_typ=( void*) _temp2191; _temp2197.rgn_typ=( void*) _temp2193;
_temp2197.nullable= _temp2104; _temp2197.tq= _temp2102; _temp2197.bounds=
_temp2100; _temp2197;}); _temp2196;}); _temp2195;});}} _LL2031:{ struct Cyc_List_List*
_temp2198= _temp2124; goto _LL2199; _LL2199: for( 0; _temp2198 != 0; _temp2198=((
struct Cyc_List_List*) _check_null( _temp2198))->tl){ inst=({ struct Cyc_List_List*
_temp2200=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2200->hd=( void*)({ struct _tuple5* _temp2201=( struct _tuple5*)
_region_malloc( rgn, sizeof( struct _tuple5)); _temp2201->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp2198))->hd; _temp2201->f2=( void*)({
struct Cyc_Absyn_VarType_struct* _temp2202=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2202[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2203; _temp2203.tag= Cyc_Absyn_VarType; _temp2203.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp2198))->hd;
_temp2203;}); _temp2202;}); _temp2201;}); _temp2200->tl= inst; _temp2200;});}}{
struct Cyc_List_List* _temp2206; struct Cyc_List_List* _temp2208; struct _tuple0
_temp2204=(( struct _tuple0(*)( struct _RegionHandle* r3, struct _RegionHandle*
r4, struct Cyc_List_List* x)) Cyc_List_rsplit)( rgn, rgn,(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple15*(* f)( struct _RegionHandle*, struct
_tuple2*), struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)(
rgn, Cyc_Tcutil_substitute_f1, rgn, _temp2118)); _LL2209: _temp2208= _temp2204.f1;
goto _LL2207; _LL2207: _temp2206= _temp2204.f2; goto _LL2205; _LL2205: { struct
Cyc_List_List* _temp2210= Cyc_Tcutil_substs( rgn, inst, _temp2206); goto _LL2211;
_LL2211: { struct Cyc_List_List* _temp2212=(( struct Cyc_List_List*(*)( struct
_tuple2*(* f)( struct _tuple15*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct _RegionHandle* r4,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn,
_temp2208, _temp2210)); goto _LL2213; _LL2213: { struct Cyc_Core_Opt* eff2; if(
_temp2122 == 0){ eff2= 0;} else{ void* _temp2214= Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2122))->v); goto _LL2215;
_LL2215: if( _temp2214 ==( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2122))->v){
eff2= _temp2122;} else{ eff2=({ struct Cyc_Core_Opt* _temp2216=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2216->v=( void*) _temp2214;
_temp2216;});}}{ struct Cyc_Absyn_VarargInfo* cyc_varargs2; if( _temp2114 == 0){
cyc_varargs2= 0;} else{ int _temp2219; void* _temp2221; void* _temp2223; struct
Cyc_Absyn_Tqual _temp2225; struct Cyc_Core_Opt* _temp2227; struct Cyc_Absyn_VarargInfo
_temp2217=*(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp2114)); _LL2228:
_temp2227= _temp2217.name; goto _LL2226; _LL2226: _temp2225= _temp2217.tq; goto
_LL2224; _LL2224: _temp2223=( void*) _temp2217.type; goto _LL2222; _LL2222:
_temp2221=( void*) _temp2217.rgn; goto _LL2220; _LL2220: _temp2219= _temp2217.inject;
goto _LL2218; _LL2218: { void* _temp2229= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2223); goto _LL2230; _LL2230: { void* _temp2231= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2221); goto _LL2232; _LL2232: if( _temp2229 == _temp2223?
_temp2221 == _temp2231: 0){ cyc_varargs2= _temp2114;} else{ cyc_varargs2=({
struct Cyc_Absyn_VarargInfo* _temp2233=( struct Cyc_Absyn_VarargInfo*) GC_malloc(
sizeof( struct Cyc_Absyn_VarargInfo)); _temp2233->name= _temp2227; _temp2233->tq=
_temp2225; _temp2233->type=( void*) _temp2229; _temp2233->rgn=( void*) _temp2231;
_temp2233->inject= _temp2219; _temp2233;});}}}} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2234=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2234[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2235; _temp2235.tag= Cyc_Absyn_FnType;
_temp2235.f1=({ struct Cyc_Absyn_FnInfo _temp2236; _temp2236.tvars= _temp2124;
_temp2236.effect= eff2; _temp2236.ret_typ=( void*) Cyc_Tcutil_rsubstitute( rgn,
inst, _temp2120); _temp2236.args= _temp2212; _temp2236.c_varargs= _temp2116;
_temp2236.cyc_varargs= cyc_varargs2; _temp2236.attributes= _temp2112; _temp2236;});
_temp2235;}); _temp2234;});}}}}} _LL2033: { struct Cyc_List_List* _temp2237=((
struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_snd_tqt, _temp2126);
goto _LL2238; _LL2238: { struct Cyc_List_List* _temp2239= Cyc_Tcutil_substs( rgn,
inst, _temp2237); goto _LL2240; _LL2240: if( _temp2239 == _temp2237){ return t;}{
struct Cyc_List_List* _temp2241=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)(
struct _tuple4*, void*), struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_map2)(
Cyc_Tcutil_map2_tq, _temp2126, _temp2239); goto _LL2242; _LL2242: return( void*)({
struct Cyc_Absyn_TupleType_struct* _temp2243=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp2243[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp2244; _temp2244.tag= Cyc_Absyn_TupleType;
_temp2244.f1= _temp2241; _temp2244;}); _temp2243;});}}} _LL2035: { struct Cyc_List_List*
_temp2245=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2128); goto _LL2246; _LL2246: { struct Cyc_List_List* _temp2247= Cyc_Tcutil_substs(
rgn, inst, _temp2245); goto _LL2248; _LL2248: if( _temp2247 == _temp2245){
return t;}{ struct Cyc_List_List* _temp2249=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type,
_temp2128, _temp2247); goto _LL2250; _LL2250: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp2251=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp2251[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp2252; _temp2252.tag= Cyc_Absyn_AnonStructType; _temp2252.f1= _temp2249;
_temp2252;}); _temp2251;});}}} _LL2037: { struct Cyc_List_List* _temp2253=((
struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type, _temp2130);
goto _LL2254; _LL2254: { struct Cyc_List_List* _temp2255= Cyc_Tcutil_substs( rgn,
inst, _temp2253); goto _LL2256; _LL2256: if( _temp2255 == _temp2253){ return t;}{
struct Cyc_List_List* _temp2257=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2130, _temp2255); goto
_LL2258; _LL2258: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp2259=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp2259[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp2260; _temp2260.tag= Cyc_Absyn_AnonStructType; _temp2260.f1= _temp2257;
_temp2260;}); _temp2259;});}}} _LL2039: if( _temp2134 != 0){ return Cyc_Tcutil_rsubstitute(
rgn, inst,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2134))->v);} else{
return t;} _LL2041: { void* _temp2261= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2138); goto _LL2262; _LL2262: return _temp2261 == _temp2138? t:( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp2263=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp2263[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp2264; _temp2264.tag= Cyc_Absyn_RgnHandleType;
_temp2264.f1=( void*) _temp2261; _temp2264;}); _temp2263;});} _LL2043: return t;
_LL2045: return t; _LL2047: return t; _LL2049: return t; _LL2051: return t;
_LL2053: return t; _LL2055: { void* _temp2265= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2148); goto _LL2266; _LL2266: return _temp2265 == _temp2148? t:( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2267=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2267[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2268; _temp2268.tag= Cyc_Absyn_AccessEff;
_temp2268.f1=( void*) _temp2265; _temp2268;}); _temp2267;});} _LL2057: { struct
Cyc_List_List* _temp2269= Cyc_Tcutil_substs( rgn, inst, _temp2150); goto _LL2270;
_LL2270: return _temp2269 == _temp2150? t:( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp2271=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2271[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2272; _temp2272.tag= Cyc_Absyn_JoinEff;
_temp2272.f1= _temp2269; _temp2272;}); _temp2271;});} _LL2013:;} static struct
Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List*
inst, struct Cyc_List_List* ts){ if( ts == 0){ return 0;}{ void* _temp2273=(
void*)(( struct Cyc_List_List*) _check_null( ts))->hd; goto _LL2274; _LL2274: {
struct Cyc_List_List* _temp2275=(( struct Cyc_List_List*) _check_null( ts))->tl;
goto _LL2276; _LL2276: { void* _temp2277= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2273); goto _LL2278; _LL2278: { struct Cyc_List_List* _temp2279= Cyc_Tcutil_substs(
rgn, inst, _temp2275); goto _LL2280; _LL2280: if( _temp2273 == _temp2277?
_temp2275 == _temp2279: 0){ return ts;} return( struct Cyc_List_List*)({ struct
Cyc_List_List* _temp2281=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2281->hd=( void*) _temp2277; _temp2281->tl= _temp2279; _temp2281;});}}}}}
extern void* Cyc_Tcutil_substitute( struct Cyc_List_List* inst, void* t){ return
Cyc_Tcutil_rsubstitute( Cyc_Core_heap_region, inst, t);} struct Cyc_Absyn_Exp*
Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv* te, void* t, struct Cyc_Position_Segment*
loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2331=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2331[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2332; _temp2332.tag= Cyc_Absyn_Const_e;
_temp2332.f1=( void*)(( void*) Cyc_Absyn_Null_c); _temp2332;}); _temp2331;}),
loc);{ void* _temp2282= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp2296; void* _temp2298; void* _temp2300; void* _temp2302; void* _temp2304;
_LL2284: if(( unsigned int) _temp2282 > 4u?*(( int*) _temp2282) == Cyc_Absyn_PointerType:
0){ _LL2297: _temp2296=(( struct Cyc_Absyn_PointerType_struct*) _temp2282)->f1;
goto _LL2285;} else{ goto _LL2286;} _LL2286: if(( unsigned int) _temp2282 > 4u?*((
int*) _temp2282) == Cyc_Absyn_IntType: 0){ _LL2301: _temp2300=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2282)->f1; goto _LL2299; _LL2299: _temp2298=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2282)->f2; if( _temp2298 ==(
void*) Cyc_Absyn_B1){ goto _LL2287;} else{ goto _LL2288;}} else{ goto _LL2288;}
_LL2288: if(( unsigned int) _temp2282 > 4u?*(( int*) _temp2282) == Cyc_Absyn_IntType:
0){ _LL2305: _temp2304=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2282)->f1;
goto _LL2303; _LL2303: _temp2302=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2282)->f2; goto _LL2289;} else{ goto _LL2290;} _LL2290: if( _temp2282 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2291;} else{ goto _LL2292;} _LL2292: if(
_temp2282 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2293;} else{ goto _LL2294;}
_LL2294: goto _LL2295; _LL2285: goto _LL2283; _LL2287:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2306=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2306[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2307; _temp2307.tag= Cyc_Absyn_Const_e; _temp2307.f1=(
void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp2308=( struct Cyc_Absyn_Char_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct)); _temp2308[ 0]=({ struct Cyc_Absyn_Char_c_struct
_temp2309; _temp2309.tag= Cyc_Absyn_Char_c; _temp2309.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp2309.f2='\000'; _temp2309;}); _temp2308;})); _temp2307;}); _temp2306;})));
goto _LL2283; _LL2289:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2310=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2310[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2311; _temp2311.tag= Cyc_Absyn_Const_e;
_temp2311.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp2312=(
struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp2312[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp2313; _temp2313.tag= Cyc_Absyn_Int_c;
_temp2313.f1=( void*) _temp2304; _temp2313.f2= 0; _temp2313;}); _temp2312;}));
_temp2311;}); _temp2310;}))); if( _temp2302 !=( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2314=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2314[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2315; _temp2315.tag= Cyc_Absyn_Cast_e; _temp2315.f1=( void*) t; _temp2315.f2=
e; _temp2315;}); _temp2314;}), loc);} goto _LL2283; _LL2291:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2316=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2316[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2317; _temp2317.tag= Cyc_Absyn_Const_e; _temp2317.f1=(
void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2318=( struct Cyc_Absyn_Float_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct)); _temp2318[ 0]=({ struct
Cyc_Absyn_Float_c_struct _temp2319; _temp2319.tag= Cyc_Absyn_Float_c; _temp2319.f1=({
unsigned char* _temp2320="0.0"; struct _tagged_string _temp2321; _temp2321.curr=
_temp2320; _temp2321.base= _temp2320; _temp2321.last_plus_one= _temp2320 + 4;
_temp2321;}); _temp2319;}); _temp2318;})); _temp2317;}); _temp2316;}))); goto
_LL2283; _LL2293:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp2322=( struct Cyc_Absyn_Cast_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp2322[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp2323; _temp2323.tag= Cyc_Absyn_Cast_e;
_temp2323.f1=( void*) t; _temp2323.f2= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2324=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2324[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2325; _temp2325.tag= Cyc_Absyn_Const_e;
_temp2325.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2326=(
struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2326[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2327; _temp2327.tag= Cyc_Absyn_Float_c;
_temp2327.f1=({ unsigned char* _temp2328="0.0"; struct _tagged_string _temp2329;
_temp2329.curr= _temp2328; _temp2329.base= _temp2328; _temp2329.last_plus_one=
_temp2328 + 4; _temp2329;}); _temp2327;}); _temp2326;})); _temp2325;});
_temp2324;}), loc); _temp2323;}); _temp2322;}))); goto _LL2283; _LL2295: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2330= Cyc_Absynpp_typ2string( t); xprintf("declaration of type %.*s requires initializer",
_temp2330.last_plus_one - _temp2330.curr, _temp2330.curr);})); goto _LL2283;
_LL2283:;} return e;} struct _tuple5* Cyc_Tcutil_make_inst_var( struct Cyc_Absyn_Tvar*
tv){ void* k=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( tv->kind);
return({ struct _tuple5* _temp2333=( struct _tuple5*) GC_malloc( sizeof( struct
_tuple5)); _temp2333->f1= tv; _temp2333->f2= Cyc_Absyn_new_evar( k); _temp2333;});}
struct _tuple5* Cyc_Tcutil_r_make_inst_var( struct _RegionHandle* rgn, struct
Cyc_Absyn_Tvar* tv){ void* k=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
tv->kind); return({ struct _tuple5* _temp2334=( struct _tuple5*) _region_malloc(
rgn, sizeof( struct _tuple5)); _temp2334->f1= tv; _temp2334->f2= Cyc_Absyn_new_evar(
k); _temp2334;});} struct Cyc_List_List* Cyc_Tcutil_check_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* free_vars, void*
expected_kind, void* t){{ void* _temp2335= Cyc_Tcutil_compress( t); int
_temp2381; struct Cyc_Core_Opt* _temp2383; void* _temp2385; struct Cyc_Absyn_Tvar*
_temp2387; struct Cyc_Absyn_Enumdecl* _temp2389; struct Cyc_Absyn_Enumdecl**
_temp2391; struct _tuple1* _temp2392; struct Cyc_Absyn_TunionInfo _temp2394;
void* _temp2396; struct Cyc_List_List* _temp2398; void* _temp2400; void**
_temp2402; struct Cyc_Absyn_TunionFieldInfo _temp2403; struct Cyc_List_List*
_temp2405; void* _temp2407; void** _temp2409; struct Cyc_Absyn_PtrInfo _temp2410;
struct Cyc_Absyn_Conref* _temp2412; struct Cyc_Absyn_Tqual _temp2414; struct Cyc_Absyn_Conref*
_temp2416; void* _temp2418; void* _temp2420; void* _temp2422; void* _temp2424;
struct Cyc_Absyn_Exp* _temp2426; struct Cyc_Absyn_Tqual _temp2428; void*
_temp2430; struct Cyc_Absyn_FnInfo _temp2432; struct Cyc_List_List* _temp2434;
struct Cyc_Absyn_VarargInfo* _temp2436; int _temp2438; struct Cyc_List_List*
_temp2440; void* _temp2442; struct Cyc_Core_Opt* _temp2444; struct Cyc_Core_Opt**
_temp2446; struct Cyc_List_List* _temp2447; struct Cyc_List_List** _temp2449;
struct Cyc_List_List* _temp2450; struct Cyc_List_List* _temp2452; struct Cyc_List_List*
_temp2454; struct Cyc_Absyn_Structdecl** _temp2456; struct Cyc_Absyn_Structdecl***
_temp2458; struct Cyc_List_List* _temp2459; struct _tuple1* _temp2461; struct
Cyc_Absyn_Uniondecl** _temp2463; struct Cyc_Absyn_Uniondecl*** _temp2465; struct
Cyc_List_List* _temp2466; struct _tuple1* _temp2468; struct Cyc_Core_Opt*
_temp2470; struct Cyc_Core_Opt** _temp2472; struct Cyc_List_List* _temp2473;
struct _tuple1* _temp2475; void* _temp2477; void* _temp2479; struct Cyc_List_List*
_temp2481; _LL2337: if( _temp2335 ==( void*) Cyc_Absyn_VoidType){ goto _LL2338;}
else{ goto _LL2339;} _LL2339: if(( unsigned int) _temp2335 > 4u?*(( int*)
_temp2335) == Cyc_Absyn_Evar: 0){ _LL2386: _temp2385=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp2335)->f1; goto _LL2384; _LL2384: _temp2383=(( struct Cyc_Absyn_Evar_struct*)
_temp2335)->f2; goto _LL2382; _LL2382: _temp2381=(( struct Cyc_Absyn_Evar_struct*)
_temp2335)->f3; goto _LL2340;} else{ goto _LL2341;} _LL2341: if(( unsigned int)
_temp2335 > 4u?*(( int*) _temp2335) == Cyc_Absyn_VarType: 0){ _LL2388: _temp2387=((
struct Cyc_Absyn_VarType_struct*) _temp2335)->f1; goto _LL2342;} else{ goto
_LL2343;} _LL2343: if(( unsigned int) _temp2335 > 4u?*(( int*) _temp2335) == Cyc_Absyn_EnumType:
0){ _LL2393: _temp2392=(( struct Cyc_Absyn_EnumType_struct*) _temp2335)->f1;
goto _LL2390; _LL2390: _temp2389=(( struct Cyc_Absyn_EnumType_struct*) _temp2335)->f2;
_temp2391=&(( struct Cyc_Absyn_EnumType_struct*) _temp2335)->f2; goto _LL2344;}
else{ goto _LL2345;} _LL2345: if(( unsigned int) _temp2335 > 4u?*(( int*)
_temp2335) == Cyc_Absyn_TunionType: 0){ _LL2395: _temp2394=(( struct Cyc_Absyn_TunionType_struct*)
_temp2335)->f1; _LL2401: _temp2400=( void*) _temp2394.tunion_info; _temp2402=&(((
struct Cyc_Absyn_TunionType_struct*) _temp2335)->f1).tunion_info; goto _LL2399;
_LL2399: _temp2398= _temp2394.targs; goto _LL2397; _LL2397: _temp2396=( void*)
_temp2394.rgn; goto _LL2346;} else{ goto _LL2347;} _LL2347: if(( unsigned int)
_temp2335 > 4u?*(( int*) _temp2335) == Cyc_Absyn_TunionFieldType: 0){ _LL2404:
_temp2403=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2335)->f1; _LL2408:
_temp2407=( void*) _temp2403.field_info; _temp2409=&((( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2335)->f1).field_info; goto _LL2406; _LL2406: _temp2405= _temp2403.targs;
goto _LL2348;} else{ goto _LL2349;} _LL2349: if(( unsigned int) _temp2335 > 4u?*((
int*) _temp2335) == Cyc_Absyn_PointerType: 0){ _LL2411: _temp2410=(( struct Cyc_Absyn_PointerType_struct*)
_temp2335)->f1; _LL2421: _temp2420=( void*) _temp2410.elt_typ; goto _LL2419;
_LL2419: _temp2418=( void*) _temp2410.rgn_typ; goto _LL2417; _LL2417: _temp2416=
_temp2410.nullable; goto _LL2415; _LL2415: _temp2414= _temp2410.tq; goto _LL2413;
_LL2413: _temp2412= _temp2410.bounds; goto _LL2350;} else{ goto _LL2351;}
_LL2351: if(( unsigned int) _temp2335 > 4u?*(( int*) _temp2335) == Cyc_Absyn_IntType:
0){ _LL2425: _temp2424=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2335)->f1;
goto _LL2423; _LL2423: _temp2422=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2335)->f2; goto _LL2352;} else{ goto _LL2353;} _LL2353: if( _temp2335 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2354;} else{ goto _LL2355;} _LL2355: if(
_temp2335 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2356;} else{ goto _LL2357;}
_LL2357: if(( unsigned int) _temp2335 > 4u?*(( int*) _temp2335) == Cyc_Absyn_ArrayType:
0){ _LL2431: _temp2430=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2335)->f1;
goto _LL2429; _LL2429: _temp2428=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2335)->f2; goto _LL2427; _LL2427: _temp2426=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2335)->f3; goto _LL2358;} else{ goto _LL2359;} _LL2359: if(( unsigned int)
_temp2335 > 4u?*(( int*) _temp2335) == Cyc_Absyn_FnType: 0){ _LL2433: _temp2432=((
struct Cyc_Absyn_FnType_struct*) _temp2335)->f1; _LL2448: _temp2447= _temp2432.tvars;
_temp2449=&((( struct Cyc_Absyn_FnType_struct*) _temp2335)->f1).tvars; goto
_LL2445; _LL2445: _temp2444= _temp2432.effect; _temp2446=&((( struct Cyc_Absyn_FnType_struct*)
_temp2335)->f1).effect; goto _LL2443; _LL2443: _temp2442=( void*) _temp2432.ret_typ;
goto _LL2441; _LL2441: _temp2440= _temp2432.args; goto _LL2439; _LL2439:
_temp2438= _temp2432.c_varargs; goto _LL2437; _LL2437: _temp2436= _temp2432.cyc_varargs;
goto _LL2435; _LL2435: _temp2434= _temp2432.attributes; goto _LL2360;} else{
goto _LL2361;} _LL2361: if(( unsigned int) _temp2335 > 4u?*(( int*) _temp2335)
== Cyc_Absyn_TupleType: 0){ _LL2451: _temp2450=(( struct Cyc_Absyn_TupleType_struct*)
_temp2335)->f1; goto _LL2362;} else{ goto _LL2363;} _LL2363: if(( unsigned int)
_temp2335 > 4u?*(( int*) _temp2335) == Cyc_Absyn_AnonStructType: 0){ _LL2453:
_temp2452=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2335)->f1; goto
_LL2364;} else{ goto _LL2365;} _LL2365: if(( unsigned int) _temp2335 > 4u?*((
int*) _temp2335) == Cyc_Absyn_AnonUnionType: 0){ _LL2455: _temp2454=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp2335)->f1; goto _LL2366;} else{ goto
_LL2367;} _LL2367: if(( unsigned int) _temp2335 > 4u?*(( int*) _temp2335) == Cyc_Absyn_StructType:
0){ _LL2462: _temp2461=(( struct Cyc_Absyn_StructType_struct*) _temp2335)->f1;
goto _LL2460; _LL2460: _temp2459=(( struct Cyc_Absyn_StructType_struct*)
_temp2335)->f2; goto _LL2457; _LL2457: _temp2456=(( struct Cyc_Absyn_StructType_struct*)
_temp2335)->f3; _temp2458=&(( struct Cyc_Absyn_StructType_struct*) _temp2335)->f3;
goto _LL2368;} else{ goto _LL2369;} _LL2369: if(( unsigned int) _temp2335 > 4u?*((
int*) _temp2335) == Cyc_Absyn_UnionType: 0){ _LL2469: _temp2468=(( struct Cyc_Absyn_UnionType_struct*)
_temp2335)->f1; goto _LL2467; _LL2467: _temp2466=(( struct Cyc_Absyn_UnionType_struct*)
_temp2335)->f2; goto _LL2464; _LL2464: _temp2463=(( struct Cyc_Absyn_UnionType_struct*)
_temp2335)->f3; _temp2465=&(( struct Cyc_Absyn_UnionType_struct*) _temp2335)->f3;
goto _LL2370;} else{ goto _LL2371;} _LL2371: if(( unsigned int) _temp2335 > 4u?*((
int*) _temp2335) == Cyc_Absyn_TypedefType: 0){ _LL2476: _temp2475=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2335)->f1; goto _LL2474; _LL2474: _temp2473=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2335)->f2; goto _LL2471; _LL2471: _temp2470=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2335)->f3; _temp2472=&(( struct Cyc_Absyn_TypedefType_struct*) _temp2335)->f3;
goto _LL2372;} else{ goto _LL2373;} _LL2373: if( _temp2335 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL2374;} else{ goto _LL2375;} _LL2375: if(( unsigned int) _temp2335 > 4u?*((
int*) _temp2335) == Cyc_Absyn_RgnHandleType: 0){ _LL2478: _temp2477=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp2335)->f1; goto _LL2376;} else{
goto _LL2377;} _LL2377: if(( unsigned int) _temp2335 > 4u?*(( int*) _temp2335)
== Cyc_Absyn_AccessEff: 0){ _LL2480: _temp2479=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp2335)->f1; goto _LL2378;} else{ goto _LL2379;} _LL2379: if(( unsigned int)
_temp2335 > 4u?*(( int*) _temp2335) == Cyc_Absyn_JoinEff: 0){ _LL2482: _temp2481=((
struct Cyc_Absyn_JoinEff_struct*) _temp2335)->f1; goto _LL2380;} else{ goto
_LL2336;} _LL2338: goto _LL2336; _LL2340: goto _LL2336; _LL2342: { struct Cyc_Absyn_Conref*
c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp2387->kind); if(( void*) c->v ==( void*) Cyc_Absyn_No_constr){( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2483=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp2483[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp2484; _temp2484.tag= Cyc_Absyn_Eq_constr;
_temp2484.f1=( void*) expected_kind; _temp2484;}); _temp2483;})));} free_vars=
Cyc_Tcutil_add_free_tvar( loc, free_vars, _temp2387); goto _LL2336;} _LL2344:
if(* _temp2391 == 0){ struct _handler_cons _temp2485; _push_handler(& _temp2485);{
int _temp2487= 0; if( setjmp( _temp2485.handler)){ _temp2487= 1;} if( !
_temp2487){{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl( te, loc,
_temp2392);* _temp2391=( struct Cyc_Absyn_Enumdecl*)* ed;}; _pop_handler();}
else{ void* _temp2486=( void*) _exn_thrown; void* _temp2489= _temp2486; _LL2491:
if( _temp2489 == Cyc_Dict_Absent){ goto _LL2492;} else{ goto _LL2493;} _LL2493:
goto _LL2494; _LL2492: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2495=
Cyc_Absynpp_qvar2string( _temp2392); xprintf("unbound type enum %.*s", _temp2495.last_plus_one
- _temp2495.curr, _temp2495.curr);})); return free_vars; _LL2494:( void) _throw(
_temp2489); _LL2490:;}}}{ struct Cyc_Absyn_Enumdecl* ed=( struct Cyc_Absyn_Enumdecl*)
_check_null(* _temp2391);* _temp2392=( ed->name)[ _check_known_subscript_notnull(
1u, 0)]; goto _LL2336;} _LL2346:{ void* _temp2496=* _temp2402; struct Cyc_Absyn_UnknownTunionInfo
_temp2502; int _temp2504; struct _tuple1* _temp2506; struct Cyc_Absyn_Tuniondecl*
_temp2508; _LL2498: if(*(( int*) _temp2496) == Cyc_Absyn_UnknownTunion){ _LL2503:
_temp2502=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp2496)->f1; _LL2507:
_temp2506= _temp2502.name; goto _LL2505; _LL2505: _temp2504= _temp2502.is_xtunion;
goto _LL2499;} else{ goto _LL2500;} _LL2500: if(*(( int*) _temp2496) == Cyc_Absyn_KnownTunion){
_LL2509: _temp2508=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2496)->f1; goto
_LL2501;} else{ goto _LL2497;} _LL2499: { struct Cyc_Absyn_Tuniondecl* tud;{
struct _handler_cons _temp2510; _push_handler(& _temp2510);{ int _temp2512= 0;
if( setjmp( _temp2510.handler)){ _temp2512= 1;} if( ! _temp2512){ tud=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp2506);; _pop_handler();} else{ void* _temp2511=( void*)
_exn_thrown; void* _temp2514= _temp2511; _LL2516: if( _temp2514 == Cyc_Dict_Absent){
goto _LL2517;} else{ goto _LL2518;} _LL2518: goto _LL2519; _LL2517: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2520= Cyc_Absynpp_qvar2string( _temp2506);
xprintf("unbound type [x]tunion %.*s", _temp2520.last_plus_one - _temp2520.curr,
_temp2520.curr);})); return free_vars; _LL2519:( void) _throw( _temp2514);
_LL2515:;}}} if( tud->is_xtunion != _temp2504){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2521= Cyc_Absynpp_qvar2string( _temp2506); xprintf("[x]tunion is different from type declaration %.*s",
_temp2521.last_plus_one - _temp2521.curr, _temp2521.curr);}));}* _temp2402=(
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp2522=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp2522[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp2523; _temp2523.tag= Cyc_Absyn_KnownTunion;
_temp2523.f1= tud; _temp2523;}); _temp2522;}); _temp2508= tud; goto _LL2501;}
_LL2501: free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_RgnKind,
_temp2396);{ struct Cyc_List_List* tvs= _temp2508->tvs; for( 0; _temp2398 != 0?
tvs != 0: 0;( _temp2398=(( struct Cyc_List_List*) _check_null( _temp2398))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2398))->hd; void* k1=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} if( _temp2398 != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2524= Cyc_Absynpp_qvar2string( _temp2508->name);
xprintf("too many type arguments for tunion %.*s", _temp2524.last_plus_one -
_temp2524.curr, _temp2524.curr);}));} if( tvs != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2525= Cyc_Absynpp_qvar2string( _temp2508->name);
xprintf("too few type arguments for tunion %.*s", _temp2525.last_plus_one -
_temp2525.curr, _temp2525.curr);}));} goto _LL2497;} _LL2497:;} goto _LL2336;
_LL2348:{ void* _temp2526=* _temp2409; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp2532; int _temp2534; struct _tuple1* _temp2536; struct _tuple1* _temp2538;
struct Cyc_Absyn_Tunionfield* _temp2540; struct Cyc_Absyn_Tuniondecl* _temp2542;
_LL2528: if(*(( int*) _temp2526) == Cyc_Absyn_UnknownTunionfield){ _LL2533:
_temp2532=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp2526)->f1;
_LL2539: _temp2538= _temp2532.tunion_name; goto _LL2537; _LL2537: _temp2536=
_temp2532.field_name; goto _LL2535; _LL2535: _temp2534= _temp2532.is_xtunion;
goto _LL2529;} else{ goto _LL2530;} _LL2530: if(*(( int*) _temp2526) == Cyc_Absyn_KnownTunionfield){
_LL2543: _temp2542=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp2526)->f1;
goto _LL2541; _LL2541: _temp2540=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2526)->f2; goto _LL2531;} else{ goto _LL2527;} _LL2529: { struct Cyc_Absyn_Tuniondecl*
tud; struct Cyc_Absyn_Tunionfield* tuf;{ struct _handler_cons _temp2544;
_push_handler(& _temp2544);{ int _temp2546= 0; if( setjmp( _temp2544.handler)){
_temp2546= 1;} if( ! _temp2546){* Cyc_Tcenv_lookup_tuniondecl( te, loc,
_temp2538);; _pop_handler();} else{ void* _temp2545=( void*) _exn_thrown; void*
_temp2548= _temp2545; _LL2550: if( _temp2548 == Cyc_Dict_Absent){ goto _LL2551;}
else{ goto _LL2552;} _LL2552: goto _LL2553; _LL2551: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2554= Cyc_Absynpp_qvar2string( _temp2538); xprintf("unbound type tunion %.*s",
_temp2554.last_plus_one - _temp2554.curr, _temp2554.curr);})); return free_vars;
_LL2553:( void) _throw( _temp2548); _LL2549:;}}}{ struct _handler_cons _temp2555;
_push_handler(& _temp2555);{ int _temp2557= 0; if( setjmp( _temp2555.handler)){
_temp2557= 1;} if( ! _temp2557){{ void* _temp2558= Cyc_Tcenv_lookup_ordinary( te,
loc, _temp2536); struct Cyc_Absyn_Tunionfield* _temp2564; struct Cyc_Absyn_Tuniondecl*
_temp2566; _LL2560: if(*(( int*) _temp2558) == Cyc_Tcenv_TunionRes){ _LL2567:
_temp2566=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2558)->f1; goto _LL2565;
_LL2565: _temp2564=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2558)->f2; goto
_LL2561;} else{ goto _LL2562;} _LL2562: goto _LL2563; _LL2561: tuf= _temp2564;
tud= _temp2566; if( tud->is_xtunion != _temp2534){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2568= Cyc_Absynpp_qvar2string( _temp2538); xprintf("[x]tunion is different from type declaration %.*s",
_temp2568.last_plus_one - _temp2568.curr, _temp2568.curr);}));} goto _LL2559;
_LL2563: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2569= Cyc_Absynpp_qvar2string(
_temp2536); struct _tagged_string _temp2570= Cyc_Absynpp_qvar2string( _temp2538);
xprintf("unbound field %.*s in type tunion %.*s", _temp2569.last_plus_one -
_temp2569.curr, _temp2569.curr, _temp2570.last_plus_one - _temp2570.curr,
_temp2570.curr);}));{ struct Cyc_List_List* _temp2571= free_vars; _npop_handler(
0u); return _temp2571;} _LL2559:;}; _pop_handler();} else{ void* _temp2556=(
void*) _exn_thrown; void* _temp2573= _temp2556; _LL2575: if( _temp2573 == Cyc_Dict_Absent){
goto _LL2576;} else{ goto _LL2577;} _LL2577: goto _LL2578; _LL2576: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2579= Cyc_Absynpp_qvar2string( _temp2536);
struct _tagged_string _temp2580= Cyc_Absynpp_qvar2string( _temp2538); xprintf("unbound field %.*s in type tunion %.*s",
_temp2579.last_plus_one - _temp2579.curr, _temp2579.curr, _temp2580.last_plus_one
- _temp2580.curr, _temp2580.curr);})); return free_vars; _LL2578:( void) _throw(
_temp2573); _LL2574:;}}}* _temp2409=( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp2581=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp2581[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp2582; _temp2582.tag= Cyc_Absyn_KnownTunionfield; _temp2582.f1= tud;
_temp2582.f2= tuf; _temp2582;}); _temp2581;}); _temp2542= tud; _temp2540= tuf;
goto _LL2531;} _LL2531: { struct Cyc_List_List* tvs= _temp2542->tvs; for( 0;
_temp2405 != 0? tvs != 0: 0;( _temp2405=(( struct Cyc_List_List*) _check_null(
_temp2405))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2405))->hd; void* k1=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} if( _temp2405 != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2583= Cyc_Absynpp_qvar2string( _temp2542->name);
struct _tagged_string _temp2584= Cyc_Absynpp_qvar2string( _temp2540->name);
xprintf("too many type arguments for tunion %.*s.%.*s", _temp2583.last_plus_one
- _temp2583.curr, _temp2583.curr, _temp2584.last_plus_one - _temp2584.curr,
_temp2584.curr);}));} if( tvs != 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2585= Cyc_Absynpp_qvar2string( _temp2542->name); struct
_tagged_string _temp2586= Cyc_Absynpp_qvar2string( _temp2540->name); xprintf("too few type arguments for tunion %.*s.%.*s",
_temp2585.last_plus_one - _temp2585.curr, _temp2585.curr, _temp2586.last_plus_one
- _temp2586.curr, _temp2586.curr);}));} goto _LL2527;} _LL2527:;} goto _LL2336;
_LL2350: free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_AnyKind,
_temp2420); free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*)
Cyc_Absyn_RgnKind, _temp2418);{ void* _temp2587=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2412))->v; void*
_temp2593; struct Cyc_Absyn_Exp* _temp2595; _LL2589: if(( unsigned int)
_temp2587 > 1u?*(( int*) _temp2587) == Cyc_Absyn_Eq_constr: 0){ _LL2594:
_temp2593=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2587)->f1; if((
unsigned int) _temp2593 > 1u?*(( int*) _temp2593) == Cyc_Absyn_Upper_b: 0){
_LL2596: _temp2595=(( struct Cyc_Absyn_Upper_b_struct*) _temp2593)->f1; goto
_LL2590;} else{ goto _LL2591;}} else{ goto _LL2591;} _LL2591: goto _LL2592;
_LL2590: if( ! Cyc_Tcutil_is_const_exp( te, _temp2595)){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp2597="pointer bounds expression is not a constant"; struct
_tagged_string _temp2598; _temp2598.curr= _temp2597; _temp2598.base= _temp2597;
_temp2598.last_plus_one= _temp2597 + 44; _temp2598;}));} Cyc_Tcexp_tcExp( te, 0,
_temp2595); if( ! Cyc_Tcutil_coerce_uint_typ( te, _temp2595)){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp2599="pointer bounds expression is not an unsigned int";
struct _tagged_string _temp2600; _temp2600.curr= _temp2599; _temp2600.base=
_temp2599; _temp2600.last_plus_one= _temp2599 + 49; _temp2600;}));} Cyc_Evexp_eval_const_uint_exp(
_temp2595); goto _LL2588; _LL2592: goto _LL2588; _LL2588:;} goto _LL2336;
_LL2352: goto _LL2336; _LL2354: goto _LL2336; _LL2356: goto _LL2336; _LL2358:
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_MemKind,
_temp2430); if( _temp2426 == 0? 1: ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp2426))){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp2601="array bounds expression is not constant";
struct _tagged_string _temp2602; _temp2602.curr= _temp2601; _temp2602.base=
_temp2601; _temp2602.last_plus_one= _temp2601 + 40; _temp2602;}));} Cyc_Tcexp_tcExp(
te, 0,( struct Cyc_Absyn_Exp*) _check_null( _temp2426)); if( ! Cyc_Tcutil_coerce_uint_typ(
te,( struct Cyc_Absyn_Exp*) _check_null( _temp2426))){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp2603="array bounds expression is not an unsigned int";
struct _tagged_string _temp2604; _temp2604.curr= _temp2603; _temp2604.base=
_temp2603; _temp2604.last_plus_one= _temp2603 + 47; _temp2604;}));} Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2426)); goto _LL2336; _LL2360: { int
num_convs= 0; int seen_cdecl= 0; int seen_stdcall= 0; int seen_fastcall= 0; for(
0; _temp2434 != 0; _temp2434=(( struct Cyc_List_List*) _check_null( _temp2434))->tl){
if( ! Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*) _check_null(
_temp2434))->hd)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2605= Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp2434))->hd); xprintf("bad function type attribute %.*s",
_temp2605.last_plus_one - _temp2605.curr, _temp2605.curr);}));}{ void* _temp2606=(
void*)(( struct Cyc_List_List*) _check_null( _temp2434))->hd; _LL2608: if(
_temp2606 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL2609;} else{ goto _LL2610;}
_LL2610: if( _temp2606 ==( void*) Cyc_Absyn_Cdecl_att){ goto _LL2611;} else{
goto _LL2612;} _LL2612: if( _temp2606 ==( void*) Cyc_Absyn_Fastcall_att){ goto
_LL2613;} else{ goto _LL2614;} _LL2614: goto _LL2615; _LL2609: if( !
seen_stdcall){ seen_stdcall= 1; ++ num_convs;} goto _LL2607; _LL2611: if( !
seen_cdecl){ seen_cdecl= 1; ++ num_convs;} goto _LL2607; _LL2613: if( !
seen_fastcall){ seen_fastcall= 1; ++ num_convs;} goto _LL2607; _LL2615: goto
_LL2607; _LL2607:;}} if( num_convs > 1){ Cyc_Tcutil_terr( loc, xprintf("function can't have multiple calling conventions"));}
Cyc_Tcutil_check_unique_tvars( loc,* _temp2449);{ struct Cyc_List_List* b=*
_temp2449; for( 0; b != 0; b=(( struct Cyc_List_List*) _check_null( b))->tl){
void* _temp2616=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( b))->hd)->kind))->v; void* _temp2622; _LL2618: if(( unsigned int)
_temp2616 > 1u?*(( int*) _temp2616) == Cyc_Absyn_Eq_constr: 0){ _LL2623:
_temp2622=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2616)->f1; if(
_temp2622 ==( void*) Cyc_Absyn_MemKind){ goto _LL2619;} else{ goto _LL2620;}}
else{ goto _LL2620;} _LL2620: goto _LL2621; _LL2619: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2624=*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( b))->hd)->name; xprintf("function attempts to abstract Mem type variable %.*s",
_temp2624.last_plus_one - _temp2624.curr, _temp2624.curr);})); goto _LL2617;
_LL2621: goto _LL2617; _LL2617:;}}{ struct Cyc_List_List* new_free_vars= 0;
new_free_vars= Cyc_Tcutil_check_valid_type( loc, te, new_free_vars,( void*) Cyc_Absyn_MemKind,
_temp2442); for( 0; _temp2440 != 0; _temp2440=(( struct Cyc_List_List*)
_check_null( _temp2440))->tl){ new_free_vars= Cyc_Tcutil_check_valid_type( loc,
te, new_free_vars,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp2440))->hd)).f3);} if( _temp2436 != 0){ if( _temp2438){(( void(*)(
struct _tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp2625="both c_vararg and cyc_vararg";
struct _tagged_string _temp2626; _temp2626.curr= _temp2625; _temp2626.base=
_temp2625; _temp2626.last_plus_one= _temp2625 + 29; _temp2626;}));}{ int
_temp2629; void* _temp2631; void* _temp2633; struct Cyc_Absyn_Tqual _temp2635;
struct Cyc_Core_Opt* _temp2637; struct Cyc_Absyn_VarargInfo _temp2627=*(( struct
Cyc_Absyn_VarargInfo*) _check_null( _temp2436)); _LL2638: _temp2637= _temp2627.name;
goto _LL2636; _LL2636: _temp2635= _temp2627.tq; goto _LL2634; _LL2634: _temp2633=(
void*) _temp2627.type; goto _LL2632; _LL2632: _temp2631=( void*) _temp2627.rgn;
goto _LL2630; _LL2630: _temp2629= _temp2627.inject; goto _LL2628; _LL2628:
new_free_vars= Cyc_Tcutil_check_valid_type( loc, te, new_free_vars,( void*) Cyc_Absyn_MemKind,
_temp2633);{ void* _temp2639= Cyc_Tcutil_compress( _temp2631); int _temp2649;
struct Cyc_Core_Opt* _temp2651; void* _temp2653; struct Cyc_Absyn_Tvar*
_temp2655; _LL2641: if(( unsigned int) _temp2639 > 4u?*(( int*) _temp2639) ==
Cyc_Absyn_Evar: 0){ _LL2654: _temp2653=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp2639)->f1; goto _LL2652; _LL2652: _temp2651=(( struct Cyc_Absyn_Evar_struct*)
_temp2639)->f2; goto _LL2650; _LL2650: _temp2649=(( struct Cyc_Absyn_Evar_struct*)
_temp2639)->f3; goto _LL2642;} else{ goto _LL2643;} _LL2643: if(( unsigned int)
_temp2639 > 4u?*(( int*) _temp2639) == Cyc_Absyn_VarType: 0){ _LL2656: _temp2655=((
struct Cyc_Absyn_VarType_struct*) _temp2639)->f1; goto _LL2644;} else{ goto
_LL2645;} _LL2645: if( _temp2639 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL2646;}
else{ goto _LL2647;} _LL2647: goto _LL2648; _LL2642: { void* vr_tvar=( void*)({
struct Cyc_Absyn_VarType_struct* _temp2657=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2657[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2658; _temp2658.tag= Cyc_Absyn_VarType; _temp2658.f1=
Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_RgnKind); _temp2658;}); _temp2657;}); Cyc_Tcutil_unify(
_temp2631, vr_tvar); goto _LL2640;} _LL2644: goto _LL2640; _LL2646: Cyc_Tcutil_terr(
loc,({ unsigned char* _temp2659="varargs should be in a region, not heap-allocated";
struct _tagged_string _temp2660; _temp2660.curr= _temp2659; _temp2660.base=
_temp2659; _temp2660.last_plus_one= _temp2659 + 50; _temp2660;})); goto _LL2640;
_LL2648: goto _LL2640; _LL2640:;} new_free_vars= Cyc_Tcutil_check_valid_type(
loc, te, new_free_vars,( void*) Cyc_Absyn_RgnKind, _temp2631); if( _temp2629){
void* _temp2661= Cyc_Tcutil_compress( _temp2633); struct Cyc_Absyn_TunionInfo
_temp2667; void* _temp2669; struct Cyc_List_List* _temp2671; void* _temp2673;
struct Cyc_Absyn_Tuniondecl* _temp2675; _LL2663: if(( unsigned int) _temp2661 >
4u?*(( int*) _temp2661) == Cyc_Absyn_TunionType: 0){ _LL2668: _temp2667=((
struct Cyc_Absyn_TunionType_struct*) _temp2661)->f1; _LL2674: _temp2673=( void*)
_temp2667.tunion_info; if(*(( int*) _temp2673) == Cyc_Absyn_KnownTunion){
_LL2676: _temp2675=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2673)->f1; goto
_LL2672;} else{ goto _LL2665;} _LL2672: _temp2671= _temp2667.targs; goto _LL2670;
_LL2670: _temp2669=( void*) _temp2667.rgn; goto _LL2664;} else{ goto _LL2665;}
_LL2665: goto _LL2666; _LL2664:{ void* _temp2677= Cyc_Tcutil_compress( _temp2669);
int _temp2687; struct Cyc_Core_Opt* _temp2689; void* _temp2691; struct Cyc_Absyn_Tvar*
_temp2693; _LL2679: if(( unsigned int) _temp2677 > 4u?*(( int*) _temp2677) ==
Cyc_Absyn_Evar: 0){ _LL2692: _temp2691=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp2677)->f1; goto _LL2690; _LL2690: _temp2689=(( struct Cyc_Absyn_Evar_struct*)
_temp2677)->f2; goto _LL2688; _LL2688: _temp2687=(( struct Cyc_Absyn_Evar_struct*)
_temp2677)->f3; goto _LL2680;} else{ goto _LL2681;} _LL2681: if(( unsigned int)
_temp2677 > 4u?*(( int*) _temp2677) == Cyc_Absyn_VarType: 0){ _LL2694: _temp2693=((
struct Cyc_Absyn_VarType_struct*) _temp2677)->f1; goto _LL2682;} else{ goto
_LL2683;} _LL2683: if( _temp2677 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL2684;}
else{ goto _LL2685;} _LL2685: goto _LL2686; _LL2680: Cyc_Tcutil_unify( _temp2631,
_temp2669); goto _LL2678; _LL2682: goto _LL2678; _LL2684: Cyc_Tcutil_terr( loc,({
unsigned char* _temp2695="injected vararg [x]tunion should be in a region, not heap-allocated";
struct _tagged_string _temp2696; _temp2696.curr= _temp2695; _temp2696.base=
_temp2695; _temp2696.last_plus_one= _temp2695 + 68; _temp2696;})); goto _LL2678;
_LL2686: goto _LL2678; _LL2678:;} goto _LL2662; _LL2666: Cyc_Tcutil_terr( loc,({
unsigned char* _temp2697="can't inject a non-[x]tunion type"; struct
_tagged_string _temp2698; _temp2698.curr= _temp2697; _temp2698.base= _temp2697;
_temp2698.last_plus_one= _temp2697 + 34; _temp2698;})); goto _LL2662; _LL2662:;}}}
if(* _temp2446 != 0){ new_free_vars= Cyc_Tcutil_check_valid_type( loc, te,
new_free_vars,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_Core_Opt*)
_check_null(* _temp2446))->v);} else{ struct Cyc_Absyn_Tvar* e= Cyc_Tcutil_new_tvar((
void*) Cyc_Absyn_EffKind); new_free_vars=({ struct Cyc_List_List* _temp2699=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2699->hd=(
void*) e; _temp2699->tl= new_free_vars; _temp2699;});{ struct Cyc_List_List*
effect= 0;{ struct Cyc_List_List* tvs= new_free_vars; for( 0; tvs != 0; tvs=((
struct Cyc_List_List*) _check_null( tvs))->tl){ void* _temp2700=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind))->v;
void* _temp2708; void* _temp2710; _LL2702: if(( unsigned int) _temp2700 > 1u?*((
int*) _temp2700) == Cyc_Absyn_Eq_constr: 0){ _LL2709: _temp2708=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2700)->f1; if( _temp2708 ==( void*) Cyc_Absyn_RgnKind){
goto _LL2703;} else{ goto _LL2704;}} else{ goto _LL2704;} _LL2704: if((
unsigned int) _temp2700 > 1u?*(( int*) _temp2700) == Cyc_Absyn_Eq_constr: 0){
_LL2711: _temp2710=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2700)->f1;
if( _temp2710 ==( void*) Cyc_Absyn_EffKind){ goto _LL2705;} else{ goto _LL2706;}}
else{ goto _LL2706;} _LL2706: goto _LL2707; _LL2703: effect=({ struct Cyc_List_List*
_temp2712=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2712->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2713=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2713[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2714; _temp2714.tag= Cyc_Absyn_AccessEff;
_temp2714.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp2715=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2715[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2716; _temp2716.tag= Cyc_Absyn_VarType;
_temp2716.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp2716;}); _temp2715;})); _temp2714;}); _temp2713;})); _temp2712->tl= effect;
_temp2712;}); goto _LL2701; _LL2705: effect=({ struct Cyc_List_List* _temp2717=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2717->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp2718=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2718[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2719; _temp2719.tag= Cyc_Absyn_VarType; _temp2719.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp2719;}); _temp2718;})); _temp2717->tl= effect; _temp2717;}); goto _LL2701;
_LL2707: goto _LL2701; _LL2701:;}}* _temp2446=({ struct Cyc_Core_Opt* _temp2720=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2720->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2721=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp2721[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp2722; _temp2722.tag= Cyc_Absyn_JoinEff; _temp2722.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( effect);
_temp2722;}); _temp2721;})); _temp2720;});}} if(* _temp2449 != 0){ struct Cyc_List_List*
new_btvs= 0;{ struct Cyc_List_List* bs=* _temp2449; for( 0; bs != 0; bs=((
struct Cyc_List_List*) _check_null( bs))->tl){ new_free_vars= Cyc_Tcutil_add_free_tvar(
loc, new_free_vars,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( bs))->hd);{ struct Cyc_Absyn_Conref* _temp2723=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( bs))->hd)->kind); goto _LL2724; _LL2724: {
void* _temp2725=( void*) _temp2723->v; void* _temp2733; _LL2727: if( _temp2725
==( void*) Cyc_Absyn_No_constr){ goto _LL2728;} else{ goto _LL2729;} _LL2729:
if(( unsigned int) _temp2725 > 1u?*(( int*) _temp2725) == Cyc_Absyn_Eq_constr: 0){
_LL2734: _temp2733=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2725)->f1;
if( _temp2733 ==( void*) Cyc_Absyn_MemKind){ goto _LL2730;} else{ goto _LL2731;}}
else{ goto _LL2731;} _LL2731: goto _LL2732; _LL2728: Cyc_Tcutil_warn( loc,({
struct _tagged_string _temp2735=*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( bs))->hd)->name; xprintf("Type variable %.*s unconstrained, assuming boxed",
_temp2735.last_plus_one - _temp2735.curr, _temp2735.curr);})); goto _LL2730;
_LL2730:( void*)( _temp2723->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2736=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2736[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2737; _temp2737.tag= Cyc_Absyn_Eq_constr;
_temp2737.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp2737;}); _temp2736;})));
goto _LL2726; _LL2732: goto _LL2726; _LL2726:;}}}} new_free_vars= Cyc_Tcutil_remove_bound_tvars(
new_free_vars,* _temp2449);} for( 0; new_free_vars != 0; new_free_vars=(( struct
Cyc_List_List*) _check_null( new_free_vars))->tl){ free_vars= Cyc_Tcutil_add_free_tvar(
loc, free_vars,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
new_free_vars))->hd);} goto _LL2336;}} _LL2362: for( 0; _temp2450 != 0;
_temp2450=(( struct Cyc_List_List*) _check_null( _temp2450))->tl){ free_vars=
Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_MemKind,(*((
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp2450))->hd)).f2);}
goto _LL2336; _LL2364:{ struct _RegionHandle _temp2738= _new_region(); struct
_RegionHandle* sprev_rgn=& _temp2738; _push_region( sprev_rgn);{ struct Cyc_List_List*
prev_fields= 0; for( 0; _temp2452 != 0; _temp2452=(( struct Cyc_List_List*)
_check_null( _temp2452))->tl){ struct Cyc_Absyn_Structfield _temp2741; struct
Cyc_List_List* _temp2742; struct Cyc_Core_Opt* _temp2744; void* _temp2746;
struct Cyc_Absyn_Tqual _temp2748; struct _tagged_string* _temp2750; struct Cyc_Absyn_Structfield*
_temp2739=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2452))->hd; _temp2741=* _temp2739; _LL2751: _temp2750= _temp2741.name; goto
_LL2749; _LL2749: _temp2748= _temp2741.tq; goto _LL2747; _LL2747: _temp2746=(
void*) _temp2741.type; goto _LL2745; _LL2745: _temp2744= _temp2741.width; goto
_LL2743; _LL2743: _temp2742= _temp2741.attributes; goto _LL2740; _LL2740: if(((
int(*)( int(* compare)( struct _tagged_string*, struct _tagged_string*), struct
Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp,
prev_fields, _temp2750)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2752=* _temp2750; xprintf("duplicate field %.*s in struct", _temp2752.last_plus_one
- _temp2752.curr, _temp2752.curr);}));} if( Cyc_String_strcmp(* _temp2750,({
unsigned char* _temp2753=""; struct _tagged_string _temp2754; _temp2754.curr=
_temp2753; _temp2754.base= _temp2753; _temp2754.last_plus_one= _temp2753 + 1;
_temp2754;})) != 0){ prev_fields=({ struct Cyc_List_List* _temp2755=( struct Cyc_List_List*)
_region_malloc( sprev_rgn, sizeof( struct Cyc_List_List)); _temp2755->hd=( void*)
_temp2750; _temp2755->tl= prev_fields; _temp2755;});} Cyc_Tcutil_check_valid_type(
loc, te, free_vars,( void*) Cyc_Absyn_MemKind, _temp2746); if( _temp2744 != 0){
unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp2744))->v)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2756=* _temp2750; xprintf("bitfield %.*s does not have constant width",
_temp2756.last_plus_one - _temp2756.curr, _temp2756.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp2744))->v);}{
void* _temp2757= Cyc_Tcutil_compress( _temp2746); void* _temp2763; void*
_temp2765; _LL2759: if(( unsigned int) _temp2757 > 4u?*(( int*) _temp2757) ==
Cyc_Absyn_IntType: 0){ _LL2766: _temp2765=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2757)->f1; goto _LL2764; _LL2764: _temp2763=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2757)->f2; goto _LL2760;} else{ goto _LL2761;} _LL2761: goto _LL2762;
_LL2760:{ void* _temp2767= _temp2763; _LL2769: if( _temp2767 ==( void*) Cyc_Absyn_B1){
goto _LL2770;} else{ goto _LL2771;} _LL2771: if( _temp2767 ==( void*) Cyc_Absyn_B2){
goto _LL2772;} else{ goto _LL2773;} _LL2773: if( _temp2767 ==( void*) Cyc_Absyn_B4){
goto _LL2774;} else{ goto _LL2775;} _LL2775: if( _temp2767 ==( void*) Cyc_Absyn_B8){
goto _LL2776;} else{ goto _LL2768;} _LL2770: if( w > 8){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp2777="bitfield larger than type"; struct _tagged_string
_temp2778; _temp2778.curr= _temp2777; _temp2778.base= _temp2777; _temp2778.last_plus_one=
_temp2777 + 26; _temp2778;}));} goto _LL2768; _LL2772: if( w > 16){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp2779="bitfield larger than type"; struct
_tagged_string _temp2780; _temp2780.curr= _temp2779; _temp2780.base= _temp2779;
_temp2780.last_plus_one= _temp2779 + 26; _temp2780;}));} goto _LL2768; _LL2774:
if( w > 32){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp2781="bitfield larger than type";
struct _tagged_string _temp2782; _temp2782.curr= _temp2781; _temp2782.base=
_temp2781; _temp2782.last_plus_one= _temp2781 + 26; _temp2782;}));} goto _LL2768;
_LL2776: if( w > 64){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp2783="bitfield larger than type";
struct _tagged_string _temp2784; _temp2784.curr= _temp2783; _temp2784.base=
_temp2783; _temp2784.last_plus_one= _temp2783 + 26; _temp2784;}));} goto _LL2768;
_LL2768:;} goto _LL2758; _LL2762: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2785=* _temp2750; struct _tagged_string _temp2786= Cyc_Absynpp_typ2string(
_temp2746); xprintf("bitfield %.*s must have integral type but has type %.*s",
_temp2785.last_plus_one - _temp2785.curr, _temp2785.curr, _temp2786.last_plus_one
- _temp2786.curr, _temp2786.curr);})); goto _LL2758; _LL2758:;}} for( 0;
_temp2742 != 0; _temp2742=(( struct Cyc_List_List*) _check_null( _temp2742))->tl){
void* _temp2787=( void*)(( struct Cyc_List_List*) _check_null( _temp2742))->hd;
int _temp2795; _LL2789: if( _temp2787 ==( void*) Cyc_Absyn_Packed_att){ goto
_LL2790;} else{ goto _LL2791;} _LL2791: if(( unsigned int) _temp2787 > 16u?*((
int*) _temp2787) == Cyc_Absyn_Aligned_att: 0){ _LL2796: _temp2795=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp2787)->f1; goto _LL2792;} else{ goto _LL2793;} _LL2793: goto _LL2794;
_LL2790: continue; _LL2792: continue; _LL2794: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2797= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null( _temp2742))->hd); struct _tagged_string _temp2798=* _temp2750;
xprintf("bad attribute %.*s on struct member %.*s", _temp2797.last_plus_one -
_temp2797.curr, _temp2797.curr, _temp2798.last_plus_one - _temp2798.curr,
_temp2798.curr);})); goto _LL2788; _LL2788:;}}}; _pop_region(& _temp2738);} goto
_LL2336; _LL2366:{ struct _RegionHandle _temp2799= _new_region(); struct
_RegionHandle* uprev_rgn=& _temp2799; _push_region( uprev_rgn);{ struct Cyc_List_List*
prev_fields= 0; for( 0; _temp2454 != 0; _temp2454=(( struct Cyc_List_List*)
_check_null( _temp2454))->tl){ struct Cyc_Absyn_Structfield _temp2802; struct
Cyc_List_List* _temp2803; struct Cyc_Core_Opt* _temp2805; void* _temp2807;
struct Cyc_Absyn_Tqual _temp2809; struct _tagged_string* _temp2811; struct Cyc_Absyn_Structfield*
_temp2800=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2454))->hd; _temp2802=* _temp2800; _LL2812: _temp2811= _temp2802.name; goto
_LL2810; _LL2810: _temp2809= _temp2802.tq; goto _LL2808; _LL2808: _temp2807=(
void*) _temp2802.type; goto _LL2806; _LL2806: _temp2805= _temp2802.width; goto
_LL2804; _LL2804: _temp2803= _temp2802.attributes; goto _LL2801; _LL2801: if(((
int(*)( int(* compare)( struct _tagged_string*, struct _tagged_string*), struct
Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp,
prev_fields, _temp2811)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2813=* _temp2811; xprintf("duplicate field %.*s in struct", _temp2813.last_plus_one
- _temp2813.curr, _temp2813.curr);}));} if( Cyc_String_strcmp(* _temp2811,({
unsigned char* _temp2814=""; struct _tagged_string _temp2815; _temp2815.curr=
_temp2814; _temp2815.base= _temp2814; _temp2815.last_plus_one= _temp2814 + 1;
_temp2815;})) != 0){ prev_fields=({ struct Cyc_List_List* _temp2816=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp2816->hd=( void*)
_temp2811; _temp2816->tl= prev_fields; _temp2816;});} Cyc_Tcutil_check_valid_type(
loc, te, free_vars,( void*) Cyc_Absyn_MemKind, _temp2807); if( ! Cyc_Tcutil_bits_only(
_temp2807)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2817=* _temp2811;
xprintf("%.*s has a type that is possibly incompatible with other members of union",
_temp2817.last_plus_one - _temp2817.curr, _temp2817.curr);}));} if( _temp2805 !=
0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp2805))->v)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2818=* _temp2811; xprintf("bitfield %.*s does not have constant width",
_temp2818.last_plus_one - _temp2818.curr, _temp2818.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp2805))->v);}{
void* _temp2819= Cyc_Tcutil_compress( _temp2807); void* _temp2825; void*
_temp2827; _LL2821: if(( unsigned int) _temp2819 > 4u?*(( int*) _temp2819) ==
Cyc_Absyn_IntType: 0){ _LL2828: _temp2827=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2819)->f1; goto _LL2826; _LL2826: _temp2825=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2819)->f2; goto _LL2822;} else{ goto _LL2823;} _LL2823: goto _LL2824;
_LL2822:{ void* _temp2829= _temp2825; _LL2831: if( _temp2829 ==( void*) Cyc_Absyn_B1){
goto _LL2832;} else{ goto _LL2833;} _LL2833: if( _temp2829 ==( void*) Cyc_Absyn_B2){
goto _LL2834;} else{ goto _LL2835;} _LL2835: if( _temp2829 ==( void*) Cyc_Absyn_B4){
goto _LL2836;} else{ goto _LL2837;} _LL2837: if( _temp2829 ==( void*) Cyc_Absyn_B8){
goto _LL2838;} else{ goto _LL2830;} _LL2832: if( w > 8){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp2839="bitfield larger than type"; struct _tagged_string
_temp2840; _temp2840.curr= _temp2839; _temp2840.base= _temp2839; _temp2840.last_plus_one=
_temp2839 + 26; _temp2840;}));} goto _LL2830; _LL2834: if( w > 16){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp2841="bitfield larger than type"; struct
_tagged_string _temp2842; _temp2842.curr= _temp2841; _temp2842.base= _temp2841;
_temp2842.last_plus_one= _temp2841 + 26; _temp2842;}));} goto _LL2830; _LL2836:
if( w > 32){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp2843="bitfield larger than type";
struct _tagged_string _temp2844; _temp2844.curr= _temp2843; _temp2844.base=
_temp2843; _temp2844.last_plus_one= _temp2843 + 26; _temp2844;}));} goto _LL2830;
_LL2838: if( w > 64){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp2845="bitfield larger than type";
struct _tagged_string _temp2846; _temp2846.curr= _temp2845; _temp2846.base=
_temp2845; _temp2846.last_plus_one= _temp2845 + 26; _temp2846;}));} goto _LL2830;
_LL2830:;} goto _LL2820; _LL2824: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2847=* _temp2811; struct _tagged_string _temp2848= Cyc_Absynpp_typ2string(
_temp2807); xprintf("bitfield %.*s must have integral type but has type %.*s",
_temp2847.last_plus_one - _temp2847.curr, _temp2847.curr, _temp2848.last_plus_one
- _temp2848.curr, _temp2848.curr);})); goto _LL2820; _LL2820:;}} for( 0;
_temp2803 != 0; _temp2803=(( struct Cyc_List_List*) _check_null( _temp2803))->tl){
void* _temp2849=( void*)(( struct Cyc_List_List*) _check_null( _temp2803))->hd;
int _temp2857; _LL2851: if( _temp2849 ==( void*) Cyc_Absyn_Packed_att){ goto
_LL2852;} else{ goto _LL2853;} _LL2853: if(( unsigned int) _temp2849 > 16u?*((
int*) _temp2849) == Cyc_Absyn_Aligned_att: 0){ _LL2858: _temp2857=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp2849)->f1; goto _LL2854;} else{ goto _LL2855;} _LL2855: goto _LL2856;
_LL2852: continue; _LL2854: continue; _LL2856: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2859= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null( _temp2803))->hd); struct _tagged_string _temp2860=* _temp2811;
xprintf("bad attribute %.*s on struct member %.*s", _temp2859.last_plus_one -
_temp2859.curr, _temp2859.curr, _temp2860.last_plus_one - _temp2860.curr,
_temp2860.curr);})); goto _LL2850; _LL2850:;}}}; _pop_region(& _temp2799);} goto
_LL2336; _LL2368: if( _temp2461 == 0){ if(* _temp2458 == 0){ Cyc_Tcutil_terr(
loc, xprintf("bad anonymous struct")); return free_vars;}{ struct Cyc_Absyn_Structdecl*
sd=*(( struct Cyc_Absyn_Structdecl**) _check_null(* _temp2458));;}} if(*
_temp2458 == 0){ struct _handler_cons _temp2861; _push_handler(& _temp2861);{
int _temp2863= 0; if( setjmp( _temp2861.handler)){ _temp2863= 1;} if( !
_temp2863){* _temp2458=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc,( struct _tuple1*) _check_null( _temp2461));; _pop_handler();} else{
void* _temp2862=( void*) _exn_thrown; void* _temp2865= _temp2862; _LL2867: if(
_temp2865 == Cyc_Dict_Absent){ goto _LL2868;} else{ goto _LL2869;} _LL2869: goto
_LL2870; _LL2868: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2871= Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp2461)); xprintf("unbound type struct %.*s",
_temp2871.last_plus_one - _temp2871.curr, _temp2871.curr);})); return free_vars;
_LL2870:( void) _throw( _temp2865); _LL2866:;}}}{ struct Cyc_Absyn_Structdecl*
sd=*(( struct Cyc_Absyn_Structdecl**) _check_null(* _temp2458)); if( sd->name !=
0){*(( struct _tuple1*) _check_null( _temp2461))=(( struct _tuple1*)(( struct
Cyc_Core_Opt*) _check_null( sd->name))->v)[ _check_known_subscript_notnull( 1u,
0)];}{ struct Cyc_List_List* tvs= sd->tvs; int lvs=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( tvs); int largs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp2459); if( lvs != largs){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2872= Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp2461));
int _temp2873= lvs; int _temp2874= largs; xprintf("struct %.*s expects %d type arguments but was given %d",
_temp2872.last_plus_one - _temp2872.curr, _temp2872.curr, _temp2873, _temp2874);}));}
for( 0; _temp2459 != 0;( _temp2459=(( struct Cyc_List_List*) _check_null(
_temp2459))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2459))->hd; void* k1=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} goto _LL2336;}} _LL2370: if( _temp2468 == 0){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp2875="missing union name"; struct _tagged_string
_temp2876; _temp2876.curr= _temp2875; _temp2876.base= _temp2875; _temp2876.last_plus_one=
_temp2875 + 19; _temp2876;})); return free_vars;} if(* _temp2465 == 0){ struct
_handler_cons _temp2877; _push_handler(& _temp2877);{ int _temp2879= 0; if(
setjmp( _temp2877.handler)){ _temp2879= 1;} if( ! _temp2879){* _temp2465=(
struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl( te, loc,( struct
_tuple1*) _check_null( _temp2468));; _pop_handler();} else{ void* _temp2878=(
void*) _exn_thrown; void* _temp2881= _temp2878; _LL2883: if( _temp2881 == Cyc_Dict_Absent){
goto _LL2884;} else{ goto _LL2885;} _LL2885: goto _LL2886; _LL2884: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2887= Cyc_Absynpp_qvar2string(( struct _tuple1*)
_check_null( _temp2468)); xprintf("unbound type union %.*s", _temp2887.last_plus_one
- _temp2887.curr, _temp2887.curr);})); return free_vars; _LL2886:( void) _throw(
_temp2881); _LL2882:;}}}{ struct Cyc_Absyn_Uniondecl* ud=*(( struct Cyc_Absyn_Uniondecl**)
_check_null(* _temp2465)); if( ud->name != 0){*(( struct _tuple1*) _check_null(
_temp2468))=(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( ud->name))->v)[
_check_known_subscript_notnull( 1u, 0)];}{ struct Cyc_List_List* tvs= ud->tvs;
int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2466); if( lvs != largs){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2888= Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp2468)); int _temp2889= lvs; int _temp2890=
largs; xprintf("union %.*s expects %d type arguments but was given %d",
_temp2888.last_plus_one - _temp2888.curr, _temp2888.curr, _temp2889, _temp2890);}));}
for( 0; _temp2466 != 0;( _temp2466=(( struct Cyc_List_List*) _check_null(
_temp2466))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2466))->hd; void* k1=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} goto _LL2336;}} _LL2372: { struct Cyc_Absyn_Typedefdecl*
td;{ struct _handler_cons _temp2891; _push_handler(& _temp2891);{ int _temp2893=
0; if( setjmp( _temp2891.handler)){ _temp2893= 1;} if( ! _temp2893){ td= Cyc_Tcenv_lookup_typedefdecl(
te, loc, _temp2475);; _pop_handler();} else{ void* _temp2892=( void*)
_exn_thrown; void* _temp2895= _temp2892; _LL2897: if( _temp2895 == Cyc_Dict_Absent){
goto _LL2898;} else{ goto _LL2899;} _LL2899: goto _LL2900; _LL2898: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2901= Cyc_Absynpp_qvar2string( _temp2475);
xprintf("unbound typedef name %.*s", _temp2901.last_plus_one - _temp2901.curr,
_temp2901.curr);})); return free_vars; _LL2900:( void) _throw( _temp2895);
_LL2896:;}}} _temp2475[ _check_known_subscript_notnull( 1u, 0)]=( td->name)[
_check_known_subscript_notnull( 1u, 0)];{ struct Cyc_List_List* tvs= td->tvs;
struct Cyc_List_List* ts= _temp2473; struct Cyc_List_List* inst= 0; for( 0; ts
!= 0? tvs != 0: 0;( ts=(( struct Cyc_List_List*) _check_null( ts))->tl, tvs=((
struct Cyc_List_List*) _check_null( tvs))->tl)){ void* k=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd)->kind); free_vars= Cyc_Tcutil_check_valid_type( loc, te,
free_vars, k,( void*)(( struct Cyc_List_List*) _check_null( ts))->hd); inst=({
struct Cyc_List_List* _temp2902=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2902->hd=( void*)({ struct _tuple5* _temp2903=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp2903->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd; _temp2903->f2=( void*)(( struct
Cyc_List_List*) _check_null( ts))->hd; _temp2903;}); _temp2902->tl= inst;
_temp2902;});} if( tvs != 0? 1: ts != 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2904= Cyc_Absynpp_qvar2string( _temp2475); xprintf("typedef %.*s expects a different number of arguments",
_temp2904.last_plus_one - _temp2904.curr, _temp2904.curr);})); return free_vars;}{
void* new_typ= Cyc_Tcutil_substitute( inst,( void*) td->defn);* _temp2472=({
struct Cyc_Core_Opt* _temp2905=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2905->v=( void*) new_typ; _temp2905;}); goto _LL2336;}}}
_LL2374: goto _LL2336; _LL2376: _temp2479= _temp2477; goto _LL2378; _LL2378:
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_RgnKind,
_temp2479); goto _LL2336; _LL2380: for( 0; _temp2481 != 0; _temp2481=(( struct
Cyc_List_List*) _check_null( _temp2481))->tl){ free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_List_List*)
_check_null( _temp2481))->hd);} goto _LL2336; _LL2336:;} if( ! Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind( t), expected_kind)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2906= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp2907= Cyc_Absynpp_kind2string( Cyc_Tcutil_typ_kind( t)); struct
_tagged_string _temp2908= Cyc_Absynpp_kind2string( expected_kind); xprintf("type %.*s has kind %.*s but as used here needs kind %.*s",
_temp2906.last_plus_one - _temp2906.curr, _temp2906.curr, _temp2907.last_plus_one
- _temp2907.curr, _temp2907.curr, _temp2908.last_plus_one - _temp2908.curr,
_temp2908.curr);}));} return free_vars;} void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, void* t){ struct
Cyc_List_List* free_tvars= Cyc_Tcutil_check_valid_type( loc, te, 0,( void*) Cyc_Absyn_MemKind,
t);{ struct Cyc_List_List* x= free_tvars; for( 0; x != 0; x=(( struct Cyc_List_List*)
_check_null( x))->tl){ struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( x))->hd)->kind); void* _temp2909=( void*) c->v;
void* _temp2917; _LL2911: if( _temp2909 ==( void*) Cyc_Absyn_No_constr){ goto
_LL2912;} else{ goto _LL2913;} _LL2913: if(( unsigned int) _temp2909 > 1u?*((
int*) _temp2909) == Cyc_Absyn_Eq_constr: 0){ _LL2918: _temp2917=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2909)->f1; if( _temp2917 ==( void*) Cyc_Absyn_MemKind){
goto _LL2914;} else{ goto _LL2915;}} else{ goto _LL2915;} _LL2915: goto _LL2916;
_LL2912: goto _LL2914; _LL2914:( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2919=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2919[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2920; _temp2920.tag= Cyc_Absyn_Eq_constr;
_temp2920.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp2920;}); _temp2919;})));
goto _LL2910; _LL2916: goto _LL2910; _LL2910:;}} if( free_tvars != 0){ void*
_temp2921= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp2927; struct
Cyc_List_List* _temp2929; struct Cyc_Absyn_VarargInfo* _temp2931; int _temp2933;
struct Cyc_List_List* _temp2935; void* _temp2937; struct Cyc_Core_Opt* _temp2939;
struct Cyc_List_List* _temp2941; struct Cyc_List_List** _temp2943; _LL2923: if((
unsigned int) _temp2921 > 4u?*(( int*) _temp2921) == Cyc_Absyn_FnType: 0){
_LL2928: _temp2927=(( struct Cyc_Absyn_FnType_struct*) _temp2921)->f1; _LL2942:
_temp2941= _temp2927.tvars; _temp2943=&((( struct Cyc_Absyn_FnType_struct*)
_temp2921)->f1).tvars; goto _LL2940; _LL2940: _temp2939= _temp2927.effect; goto
_LL2938; _LL2938: _temp2937=( void*) _temp2927.ret_typ; goto _LL2936; _LL2936:
_temp2935= _temp2927.args; goto _LL2934; _LL2934: _temp2933= _temp2927.c_varargs;
goto _LL2932; _LL2932: _temp2931= _temp2927.cyc_varargs; goto _LL2930; _LL2930:
_temp2929= _temp2927.attributes; goto _LL2924;} else{ goto _LL2925;} _LL2925:
goto _LL2926; _LL2924: if(* _temp2943 == 0){* _temp2943= free_tvars;} else{ if(
free_tvars != 0){ struct _tagged_string s=*(( struct Cyc_Absyn_Tvar*)(( struct
Cyc_List_List*) _check_null( free_tvars))->hd)->name; if(*(( unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( unsigned char), 0) =='?'){ Cyc_Tcutil_terr( loc, xprintf("You failed to abstract over the generated type variables. \nRemove the bound type variables or else make the effects of the type explicit\nand then abstract the free type variables properly."));}
else{ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2944= s; xprintf("unbound type variable %.*s",
_temp2944.last_plus_one - _temp2944.curr, _temp2944.curr);}));}}} goto _LL2922;
_LL2926: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2945=*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( free_tvars))->hd)->name; xprintf("unbound type variable %.*s",
_temp2945.last_plus_one - _temp2945.curr, _temp2945.curr);})); return; _LL2922:;}}
void Cyc_Tcutil_check_type( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* bound_tvars, void* expected_kind, void* t){ struct Cyc_List_List*
free_tvars= Cyc_Tcutil_check_valid_type( loc, te, 0, expected_kind, t); struct
Cyc_List_List* fs= free_tvars; for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ struct Cyc_Absyn_Tvar* ftv=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( fs))->hd; struct _tagged_string* _temp2946=
ftv->name; goto _LL2947; _LL2947: { struct Cyc_Absyn_Conref* _temp2948= ftv->kind;
goto _LL2949; _LL2949: { int present= 0;{ struct Cyc_List_List* bs= bound_tvars;
for( 0; bs != 0; bs=(( struct Cyc_List_List*) _check_null( bs))->tl){ struct Cyc_Absyn_Tvar*
btv=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( bs))->hd;
struct _tagged_string* _temp2950= btv->name; goto _LL2951; _LL2951: { struct Cyc_Absyn_Conref*
_temp2952= btv->kind; goto _LL2953; _LL2953: if( Cyc_String_zstrptrcmp(
_temp2946, _temp2950) == 0){ present= 1; if( ! Cyc_Tcutil_constrain_kinds(
_temp2952, _temp2948)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2954=*
_temp2946; struct _tagged_string _temp2955= Cyc_Absynpp_ckind2string( _temp2952);
struct _tagged_string _temp2956= Cyc_Absynpp_ckind2string( _temp2948); xprintf("type variable %.*s has kind %.*s but is used here atkind %.*s",
_temp2954.last_plus_one - _temp2954.curr, _temp2954.curr, _temp2955.last_plus_one
- _temp2955.curr, _temp2955.curr, _temp2956.last_plus_one - _temp2956.curr,
_temp2956.curr);}));} break;}}}} if( ! present){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2957=* _temp2946; struct _tagged_string _temp2958= Cyc_Absynpp_typ2string(
t); xprintf("unbound type variable %.*s in type %.*s", _temp2957.last_plus_one -
_temp2957.curr, _temp2957.curr, _temp2958.last_plus_one - _temp2958.curr,
_temp2958.curr);}));}}}}} static void Cyc_Tcutil_check_unique_unsorted( int(*
cmp)( void*, void*), struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc,
struct _tagged_string(* a2string)( void*), struct _tagged_string msg){ for( 0;
vs != 0; vs=(( struct Cyc_List_List*) _check_null( vs))->tl){ struct Cyc_List_List*
vs2=(( struct Cyc_List_List*) _check_null( vs))->tl; for( 0; vs2 != 0; vs2=((
struct Cyc_List_List*) _check_null( vs2))->tl){ if( cmp(( void*)(( struct Cyc_List_List*)
_check_null( vs))->hd,( void*)(( struct Cyc_List_List*) _check_null( vs2))->hd)
== 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2959= msg; struct
_tagged_string _temp2960= a2string(( void*)(( struct Cyc_List_List*) _check_null(
vs))->hd); xprintf("%.*s: %.*s", _temp2959.last_plus_one - _temp2959.curr,
_temp2959.curr, _temp2960.last_plus_one - _temp2960.curr, _temp2960.curr);}));}}}}
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
tvs, loc, Cyc_Tcutil_tvar2string,({ unsigned char* _temp2961="duplicate type variable";
struct _tagged_string _temp2962; _temp2962.curr= _temp2961; _temp2962.base=
_temp2961; _temp2962.last_plus_one= _temp2961 + 24; _temp2962;}));} struct
_tuple16{ struct Cyc_Absyn_Structfield* f1; int f2; } ; struct _tuple17{ struct
Cyc_List_List* f1; void* f2; } ; struct _tuple18{ struct Cyc_Absyn_Structfield*
f1; void* f2; } ; struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* sdfields){ struct Cyc_List_List* fields= 0;{ struct
Cyc_List_List* sd_fields= sdfields; for( 0; sd_fields != 0; sd_fields=(( struct
Cyc_List_List*) _check_null( sd_fields))->tl){ if( Cyc_String_strcmp(*(( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( sd_fields))->hd)->name,({
unsigned char* _temp2963=""; struct _tagged_string _temp2964; _temp2964.curr=
_temp2963; _temp2964.base= _temp2963; _temp2964.last_plus_one= _temp2963 + 1;
_temp2964;})) != 0){ fields=({ struct Cyc_List_List* _temp2965=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2965->hd=( void*)({ struct
_tuple16* _temp2966=( struct _tuple16*) GC_malloc( sizeof( struct _tuple16));
_temp2966->f1=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( sd_fields))->hd; _temp2966->f2= 0; _temp2966;}); _temp2965->tl=
fields; _temp2965;});}}} fields=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( fields);{ struct Cyc_List_List* ans= 0; for( 0; des != 0;
des=(( struct Cyc_List_List*) _check_null( des))->tl){ struct _tuple17 _temp2969;
void* _temp2970; struct Cyc_List_List* _temp2972; struct _tuple17* _temp2967=(
struct _tuple17*)(( struct Cyc_List_List*) _check_null( des))->hd; _temp2969=*
_temp2967; _LL2973: _temp2972= _temp2969.f1; goto _LL2971; _LL2971: _temp2970=
_temp2969.f2; goto _LL2968; _LL2968: if( _temp2972 == 0){ struct Cyc_List_List*
_temp2974= fields; goto _LL2975; _LL2975: for( 0; _temp2974 != 0; _temp2974=((
struct Cyc_List_List*) _check_null( _temp2974))->tl){ if( !(*(( struct _tuple16*)((
struct Cyc_List_List*) _check_null( _temp2974))->hd)).f2){(*(( struct _tuple16*)((
struct Cyc_List_List*) _check_null( _temp2974))->hd)).f2= 1;(*(( struct _tuple17*)((
struct Cyc_List_List*) _check_null( des))->hd)).f1=({ struct Cyc_List_List*
_temp2976=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2976->hd=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp2977=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp2977[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp2978; _temp2978.tag= Cyc_Absyn_FieldName;
_temp2978.f1=((*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(
_temp2974))->hd)).f1)->name; _temp2978;}); _temp2977;})); _temp2976->tl= 0;
_temp2976;}); ans=({ struct Cyc_List_List* _temp2979=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2979->hd=( void*)({
struct _tuple18* _temp2980=( struct _tuple18*) _region_malloc( rgn, sizeof(
struct _tuple18)); _temp2980->f1=(*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null( _temp2974))->hd)).f1; _temp2980->f2= _temp2970; _temp2980;});
_temp2979->tl= ans; _temp2979;}); break;}} if( _temp2974 == 0){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp2981="too many arguments to struct"; struct
_tagged_string _temp2982; _temp2982.curr= _temp2981; _temp2982.base= _temp2981;
_temp2982.last_plus_one= _temp2981 + 29; _temp2982;}));}} else{ if((( struct Cyc_List_List*)
_check_null( _temp2972))->tl != 0){ Cyc_Tcutil_terr( loc,({ unsigned char*
_temp2983="multiple designators are not supported"; struct _tagged_string
_temp2984; _temp2984.curr= _temp2983; _temp2984.base= _temp2983; _temp2984.last_plus_one=
_temp2983 + 39; _temp2984;}));} else{ void* _temp2985=( void*)(( struct Cyc_List_List*)
_check_null( _temp2972))->hd; struct Cyc_Absyn_Exp* _temp2991; struct
_tagged_string* _temp2993; _LL2987: if(*(( int*) _temp2985) == Cyc_Absyn_ArrayElement){
_LL2992: _temp2991=(( struct Cyc_Absyn_ArrayElement_struct*) _temp2985)->f1;
goto _LL2988;} else{ goto _LL2989;} _LL2989: if(*(( int*) _temp2985) == Cyc_Absyn_FieldName){
_LL2994: _temp2993=(( struct Cyc_Absyn_FieldName_struct*) _temp2985)->f1; goto
_LL2990;} else{ goto _LL2986;} _LL2988: Cyc_Tcutil_terr( loc,({ unsigned char*
_temp2995="array designator used in argument to struct"; struct _tagged_string
_temp2996; _temp2996.curr= _temp2995; _temp2996.base= _temp2995; _temp2996.last_plus_one=
_temp2995 + 44; _temp2996;})); goto _LL2986; _LL2990: { struct Cyc_List_List*
_temp2997= fields; goto _LL2998; _LL2998: for( 0; _temp2997 != 0; _temp2997=((
struct Cyc_List_List*) _check_null( _temp2997))->tl){ if( Cyc_String_zstrptrcmp(
_temp2993,((*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(
_temp2997))->hd)).f1)->name) == 0){ if((*(( struct _tuple16*)(( struct Cyc_List_List*)
_check_null( _temp2997))->hd)).f2){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2999=* _temp2993; xprintf("field %.*s has already been used as an argument",
_temp2999.last_plus_one - _temp2999.curr, _temp2999.curr);}));}(*(( struct
_tuple16*)(( struct Cyc_List_List*) _check_null( _temp2997))->hd)).f2= 1; ans=({
struct Cyc_List_List* _temp3000=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp3000->hd=( void*)({ struct _tuple18*
_temp3001=( struct _tuple18*) _region_malloc( rgn, sizeof( struct _tuple18));
_temp3001->f1=(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(
_temp2997))->hd)).f1; _temp3001->f2= _temp2970; _temp3001;}); _temp3000->tl= ans;
_temp3000;}); break;}} if( _temp2997 == 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp3002=* _temp2993; xprintf("bad field designator %.*s",
_temp3002.last_plus_one - _temp3002.curr, _temp3002.curr);}));} goto _LL2986;}
_LL2986:;}}} for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null(
fields))->tl){ if( !(*(( struct _tuple16*)(( struct Cyc_List_List*) _check_null(
fields))->hd)).f2){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp3003="too few arguments to struct";
struct _tagged_string _temp3004; _temp3004.curr= _temp3003; _temp3004.base=
_temp3003; _temp3004.last_plus_one= _temp3003 + 28; _temp3004;})); break;}}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
ans);}} int Cyc_Tcutil_is_tagged_pointer_typ( void* t){ void* _temp3005= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_PtrInfo _temp3011; struct Cyc_Absyn_Conref* _temp3013;
struct Cyc_Absyn_Tqual _temp3015; struct Cyc_Absyn_Conref* _temp3017; void*
_temp3019; void* _temp3021; _LL3007: if(( unsigned int) _temp3005 > 4u?*(( int*)
_temp3005) == Cyc_Absyn_PointerType: 0){ _LL3012: _temp3011=(( struct Cyc_Absyn_PointerType_struct*)
_temp3005)->f1; _LL3022: _temp3021=( void*) _temp3011.elt_typ; goto _LL3020;
_LL3020: _temp3019=( void*) _temp3011.rgn_typ; goto _LL3018; _LL3018: _temp3017=
_temp3011.nullable; goto _LL3016; _LL3016: _temp3015= _temp3011.tq; goto _LL3014;
_LL3014: _temp3013= _temp3011.bounds; goto _LL3008;} else{ goto _LL3009;}
_LL3009: goto _LL3010; _LL3008: { struct Cyc_Absyn_Conref* _temp3023=(( struct
Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp3013); goto _LL3024; _LL3024: { void* _temp3025=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3023))->v; void*
_temp3033; _LL3027: if(( unsigned int) _temp3025 > 1u?*(( int*) _temp3025) ==
Cyc_Absyn_Eq_constr: 0){ _LL3034: _temp3033=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3025)->f1; if( _temp3033 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3028;}
else{ goto _LL3029;}} else{ goto _LL3029;} _LL3029: if( _temp3025 ==( void*) Cyc_Absyn_No_constr){
goto _LL3030;} else{ goto _LL3031;} _LL3031: goto _LL3032; _LL3028: return 1;
_LL3030:( void*)( _temp3023->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3035=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3035[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3036; _temp3036.tag= Cyc_Absyn_Eq_constr;
_temp3036.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp3036;}); _temp3035;})));
return 1; _LL3032: return 0; _LL3026:;}} _LL3010: return 0; _LL3006:;} struct
_tuple6 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e){ struct _tuple6 bogus_ans=({ struct _tuple6 _temp3275; _temp3275.f1= 0;
_temp3275.f2=( void*) Cyc_Absyn_HeapRgn; _temp3275;}); void* _temp3037=( void*)
e->r; void* _temp3051; struct _tuple1* _temp3053; struct _tagged_string*
_temp3055; struct Cyc_Absyn_Exp* _temp3057; struct _tagged_string* _temp3059;
struct Cyc_Absyn_Exp* _temp3061; struct Cyc_Absyn_Exp* _temp3063; struct Cyc_Absyn_Exp*
_temp3065; struct Cyc_Absyn_Exp* _temp3067; _LL3039: if(*(( int*) _temp3037) ==
Cyc_Absyn_Var_e){ _LL3054: _temp3053=(( struct Cyc_Absyn_Var_e_struct*)
_temp3037)->f1; goto _LL3052; _LL3052: _temp3051=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3037)->f2; goto _LL3040;} else{ goto _LL3041;} _LL3041: if(*(( int*)
_temp3037) == Cyc_Absyn_StructMember_e){ _LL3058: _temp3057=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3037)->f1; goto _LL3056; _LL3056: _temp3055=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3037)->f2; goto _LL3042;} else{ goto _LL3043;} _LL3043: if(*(( int*)
_temp3037) == Cyc_Absyn_StructArrow_e){ _LL3062: _temp3061=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3037)->f1; goto _LL3060; _LL3060: _temp3059=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3037)->f2; goto _LL3044;} else{ goto _LL3045;} _LL3045: if(*(( int*)
_temp3037) == Cyc_Absyn_Deref_e){ _LL3064: _temp3063=(( struct Cyc_Absyn_Deref_e_struct*)
_temp3037)->f1; goto _LL3046;} else{ goto _LL3047;} _LL3047: if(*(( int*)
_temp3037) == Cyc_Absyn_Subscript_e){ _LL3068: _temp3067=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3037)->f1; goto _LL3066; _LL3066: _temp3065=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3037)->f2; goto _LL3048;} else{ goto _LL3049;} _LL3049: goto _LL3050;
_LL3040: { void* _temp3069= _temp3051; struct Cyc_Absyn_Fndecl* _temp3083;
struct Cyc_Absyn_Vardecl* _temp3085; struct Cyc_Absyn_Vardecl* _temp3087; struct
Cyc_Absyn_Vardecl* _temp3089; struct Cyc_Absyn_Vardecl* _temp3091; _LL3071: if(
_temp3069 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL3072;} else{ goto _LL3073;}
_LL3073: if(( unsigned int) _temp3069 > 1u?*(( int*) _temp3069) == Cyc_Absyn_Funname_b:
0){ _LL3084: _temp3083=(( struct Cyc_Absyn_Funname_b_struct*) _temp3069)->f1;
goto _LL3074;} else{ goto _LL3075;} _LL3075: if(( unsigned int) _temp3069 > 1u?*((
int*) _temp3069) == Cyc_Absyn_Global_b: 0){ _LL3086: _temp3085=(( struct Cyc_Absyn_Global_b_struct*)
_temp3069)->f1; goto _LL3076;} else{ goto _LL3077;} _LL3077: if(( unsigned int)
_temp3069 > 1u?*(( int*) _temp3069) == Cyc_Absyn_Local_b: 0){ _LL3088: _temp3087=((
struct Cyc_Absyn_Local_b_struct*) _temp3069)->f1; goto _LL3078;} else{ goto
_LL3079;} _LL3079: if(( unsigned int) _temp3069 > 1u?*(( int*) _temp3069) == Cyc_Absyn_Pat_b:
0){ _LL3090: _temp3089=(( struct Cyc_Absyn_Pat_b_struct*) _temp3069)->f1; goto
_LL3080;} else{ goto _LL3081;} _LL3081: if(( unsigned int) _temp3069 > 1u?*((
int*) _temp3069) == Cyc_Absyn_Param_b: 0){ _LL3092: _temp3091=(( struct Cyc_Absyn_Param_b_struct*)
_temp3069)->f1; goto _LL3082;} else{ goto _LL3070;} _LL3072: return bogus_ans;
_LL3074: return({ struct _tuple6 _temp3093; _temp3093.f1= 1; _temp3093.f2=( void*)
Cyc_Absyn_HeapRgn; _temp3093;}); _LL3076: { void* _temp3094= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); struct Cyc_Absyn_Exp*
_temp3100; struct Cyc_Absyn_Tqual _temp3102; void* _temp3104; _LL3096: if((
unsigned int) _temp3094 > 4u?*(( int*) _temp3094) == Cyc_Absyn_ArrayType: 0){
_LL3105: _temp3104=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3094)->f1;
goto _LL3103; _LL3103: _temp3102=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3094)->f2; goto _LL3101; _LL3101: _temp3100=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3094)->f3; goto _LL3097;} else{ goto _LL3098;} _LL3098: goto _LL3099;
_LL3097: return({ struct _tuple6 _temp3106; _temp3106.f1= 1; _temp3106.f2=( void*)
Cyc_Absyn_HeapRgn; _temp3106;}); _LL3099: return({ struct _tuple6 _temp3107;
_temp3107.f1=( _temp3085->tq).q_const; _temp3107.f2=( void*) Cyc_Absyn_HeapRgn;
_temp3107;}); _LL3095:;} _LL3078: { void* _temp3108= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v); struct Cyc_Absyn_Exp*
_temp3114; struct Cyc_Absyn_Tqual _temp3116; void* _temp3118; _LL3110: if((
unsigned int) _temp3108 > 4u?*(( int*) _temp3108) == Cyc_Absyn_ArrayType: 0){
_LL3119: _temp3118=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3108)->f1;
goto _LL3117; _LL3117: _temp3116=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3108)->f2; goto _LL3115; _LL3115: _temp3114=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3108)->f3; goto _LL3111;} else{ goto _LL3112;} _LL3112: goto _LL3113;
_LL3111: return({ struct _tuple6 _temp3120; _temp3120.f1= 1; _temp3120.f2=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp3087->rgn))->v; _temp3120;}); _LL3113:
return({ struct _tuple6 _temp3121; _temp3121.f1=( _temp3087->tq).q_const;
_temp3121.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3087->rgn))->v;
_temp3121;}); _LL3109:;} _LL3080: _temp3091= _temp3089; goto _LL3082; _LL3082:
return({ struct _tuple6 _temp3122; _temp3122.f1=( _temp3091->tq).q_const;
_temp3122.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3091->rgn))->v;
_temp3122;}); _LL3070:;} _LL3042: { void* _temp3123= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp3057->topt))->v); struct Cyc_List_List*
_temp3135; struct Cyc_List_List* _temp3137; struct Cyc_Absyn_Structdecl**
_temp3139; struct Cyc_Absyn_Structdecl* _temp3141; struct Cyc_List_List*
_temp3142; struct _tuple1* _temp3144; struct Cyc_Absyn_Uniondecl** _temp3146;
struct Cyc_Absyn_Uniondecl* _temp3148; struct Cyc_List_List* _temp3149; struct
_tuple1* _temp3151; _LL3125: if(( unsigned int) _temp3123 > 4u?*(( int*)
_temp3123) == Cyc_Absyn_AnonStructType: 0){ _LL3136: _temp3135=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3123)->f1; goto _LL3126;} else{ goto _LL3127;} _LL3127: if(( unsigned int)
_temp3123 > 4u?*(( int*) _temp3123) == Cyc_Absyn_AnonUnionType: 0){ _LL3138:
_temp3137=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3123)->f1; goto
_LL3128;} else{ goto _LL3129;} _LL3129: if(( unsigned int) _temp3123 > 4u?*((
int*) _temp3123) == Cyc_Absyn_StructType: 0){ _LL3145: _temp3144=(( struct Cyc_Absyn_StructType_struct*)
_temp3123)->f1; goto _LL3143; _LL3143: _temp3142=(( struct Cyc_Absyn_StructType_struct*)
_temp3123)->f2; goto _LL3140; _LL3140: _temp3139=(( struct Cyc_Absyn_StructType_struct*)
_temp3123)->f3; if( _temp3139 == 0){ goto _LL3131;} else{ _temp3141=* _temp3139;
goto _LL3130;}} else{ goto _LL3131;} _LL3131: if(( unsigned int) _temp3123 > 4u?*((
int*) _temp3123) == Cyc_Absyn_UnionType: 0){ _LL3152: _temp3151=(( struct Cyc_Absyn_UnionType_struct*)
_temp3123)->f1; goto _LL3150; _LL3150: _temp3149=(( struct Cyc_Absyn_UnionType_struct*)
_temp3123)->f2; goto _LL3147; _LL3147: _temp3146=(( struct Cyc_Absyn_UnionType_struct*)
_temp3123)->f3; if( _temp3146 == 0){ goto _LL3133;} else{ _temp3148=* _temp3146;
goto _LL3132;}} else{ goto _LL3133;} _LL3133: goto _LL3134; _LL3126: _temp3137=
_temp3135; goto _LL3128; _LL3128: { struct Cyc_Absyn_Structfield* _temp3153= Cyc_Absyn_lookup_field(
_temp3137, _temp3055); goto _LL3154; _LL3154: if( _temp3153 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3153))->width != 0: 0){ return({ struct _tuple6 _temp3155;
_temp3155.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3153))->tq).q_const;
_temp3155.f2=( Cyc_Tcutil_addressof_props( te, _temp3057)).f2; _temp3155;});}
return bogus_ans;} _LL3130: { struct Cyc_Absyn_Structfield* _temp3156= Cyc_Absyn_lookup_struct_field(
_temp3141, _temp3055); goto _LL3157; _LL3157: if( _temp3156 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3156))->width != 0: 0){ return({ struct _tuple6 _temp3158;
_temp3158.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3156))->tq).q_const;
_temp3158.f2=( Cyc_Tcutil_addressof_props( te, _temp3057)).f2; _temp3158;});}
return bogus_ans;} _LL3132: { struct Cyc_Absyn_Structfield* _temp3159= Cyc_Absyn_lookup_union_field(
_temp3148, _temp3055); goto _LL3160; _LL3160: if( _temp3159 != 0){ return({
struct _tuple6 _temp3161; _temp3161.f1=((( struct Cyc_Absyn_Structfield*)
_check_null( _temp3159))->tq).q_const; _temp3161.f2=( Cyc_Tcutil_addressof_props(
te, _temp3057)).f2; _temp3161;});} goto _LL3134;} _LL3134: return bogus_ans;
_LL3124:;} _LL3044: { void* _temp3162= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3061->topt))->v); struct Cyc_Absyn_PtrInfo _temp3168; struct
Cyc_Absyn_Conref* _temp3170; struct Cyc_Absyn_Tqual _temp3172; struct Cyc_Absyn_Conref*
_temp3174; void* _temp3176; void* _temp3178; _LL3164: if(( unsigned int)
_temp3162 > 4u?*(( int*) _temp3162) == Cyc_Absyn_PointerType: 0){ _LL3169:
_temp3168=(( struct Cyc_Absyn_PointerType_struct*) _temp3162)->f1; _LL3179:
_temp3178=( void*) _temp3168.elt_typ; goto _LL3177; _LL3177: _temp3176=( void*)
_temp3168.rgn_typ; goto _LL3175; _LL3175: _temp3174= _temp3168.nullable; goto
_LL3173; _LL3173: _temp3172= _temp3168.tq; goto _LL3171; _LL3171: _temp3170=
_temp3168.bounds; goto _LL3165;} else{ goto _LL3166;} _LL3166: goto _LL3167;
_LL3165: { void* _temp3180= Cyc_Tcutil_compress( _temp3178); struct Cyc_List_List*
_temp3192; struct Cyc_List_List* _temp3194; struct Cyc_Absyn_Structdecl**
_temp3196; struct Cyc_Absyn_Structdecl* _temp3198; struct Cyc_List_List*
_temp3199; struct _tuple1* _temp3201; struct Cyc_Absyn_Uniondecl** _temp3203;
struct Cyc_Absyn_Uniondecl* _temp3205; struct Cyc_List_List* _temp3206; struct
_tuple1* _temp3208; _LL3182: if(( unsigned int) _temp3180 > 4u?*(( int*)
_temp3180) == Cyc_Absyn_AnonStructType: 0){ _LL3193: _temp3192=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3180)->f1; goto _LL3183;} else{ goto _LL3184;} _LL3184: if(( unsigned int)
_temp3180 > 4u?*(( int*) _temp3180) == Cyc_Absyn_AnonUnionType: 0){ _LL3195:
_temp3194=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3180)->f1; goto
_LL3185;} else{ goto _LL3186;} _LL3186: if(( unsigned int) _temp3180 > 4u?*((
int*) _temp3180) == Cyc_Absyn_StructType: 0){ _LL3202: _temp3201=(( struct Cyc_Absyn_StructType_struct*)
_temp3180)->f1; goto _LL3200; _LL3200: _temp3199=(( struct Cyc_Absyn_StructType_struct*)
_temp3180)->f2; goto _LL3197; _LL3197: _temp3196=(( struct Cyc_Absyn_StructType_struct*)
_temp3180)->f3; if( _temp3196 == 0){ goto _LL3188;} else{ _temp3198=* _temp3196;
goto _LL3187;}} else{ goto _LL3188;} _LL3188: if(( unsigned int) _temp3180 > 4u?*((
int*) _temp3180) == Cyc_Absyn_UnionType: 0){ _LL3209: _temp3208=(( struct Cyc_Absyn_UnionType_struct*)
_temp3180)->f1; goto _LL3207; _LL3207: _temp3206=(( struct Cyc_Absyn_UnionType_struct*)
_temp3180)->f2; goto _LL3204; _LL3204: _temp3203=(( struct Cyc_Absyn_UnionType_struct*)
_temp3180)->f3; if( _temp3203 == 0){ goto _LL3190;} else{ _temp3205=* _temp3203;
goto _LL3189;}} else{ goto _LL3190;} _LL3190: goto _LL3191; _LL3183: _temp3194=
_temp3192; goto _LL3185; _LL3185: { struct Cyc_Absyn_Structfield* _temp3210= Cyc_Absyn_lookup_field(
_temp3194, _temp3059); goto _LL3211; _LL3211: if( _temp3210 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3210))->width != 0: 0){ return({ struct _tuple6 _temp3212;
_temp3212.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3210))->tq).q_const;
_temp3212.f2= _temp3176; _temp3212;});} return bogus_ans;} _LL3187: { struct Cyc_Absyn_Structfield*
_temp3213= Cyc_Absyn_lookup_struct_field( _temp3198, _temp3059); goto _LL3214;
_LL3214: if( _temp3213 != 0?(( struct Cyc_Absyn_Structfield*) _check_null(
_temp3213))->width != 0: 0){ return({ struct _tuple6 _temp3215; _temp3215.f1=(((
struct Cyc_Absyn_Structfield*) _check_null( _temp3213))->tq).q_const; _temp3215.f2=
_temp3176; _temp3215;});} return bogus_ans;} _LL3189: { struct Cyc_Absyn_Structfield*
_temp3216= Cyc_Absyn_lookup_union_field( _temp3205, _temp3059); goto _LL3217;
_LL3217: if( _temp3216 != 0){ return({ struct _tuple6 _temp3218; _temp3218.f1=(((
struct Cyc_Absyn_Structfield*) _check_null( _temp3216))->tq).q_const; _temp3218.f2=
_temp3176; _temp3218;});} return bogus_ans;} _LL3191: return bogus_ans; _LL3181:;}
_LL3167: return bogus_ans; _LL3163:;} _LL3046: { void* _temp3219= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp3063->topt))->v); struct Cyc_Absyn_PtrInfo
_temp3225; struct Cyc_Absyn_Conref* _temp3227; struct Cyc_Absyn_Tqual _temp3229;
struct Cyc_Absyn_Conref* _temp3231; void* _temp3233; void* _temp3235; _LL3221:
if(( unsigned int) _temp3219 > 4u?*(( int*) _temp3219) == Cyc_Absyn_PointerType:
0){ _LL3226: _temp3225=(( struct Cyc_Absyn_PointerType_struct*) _temp3219)->f1;
_LL3236: _temp3235=( void*) _temp3225.elt_typ; goto _LL3234; _LL3234: _temp3233=(
void*) _temp3225.rgn_typ; goto _LL3232; _LL3232: _temp3231= _temp3225.nullable;
goto _LL3230; _LL3230: _temp3229= _temp3225.tq; goto _LL3228; _LL3228: _temp3227=
_temp3225.bounds; goto _LL3222;} else{ goto _LL3223;} _LL3223: goto _LL3224;
_LL3222: return({ struct _tuple6 _temp3237; _temp3237.f1= _temp3229.q_const;
_temp3237.f2= _temp3233; _temp3237;}); _LL3224: return bogus_ans; _LL3220:;}
_LL3048: { void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3067->topt))->v); void* _temp3238= t; struct Cyc_Absyn_Exp*
_temp3248; struct Cyc_Absyn_Tqual _temp3250; void* _temp3252; struct Cyc_List_List*
_temp3254; struct Cyc_Absyn_PtrInfo _temp3256; struct Cyc_Absyn_Conref*
_temp3258; struct Cyc_Absyn_Tqual _temp3260; struct Cyc_Absyn_Conref* _temp3262;
void* _temp3264; void* _temp3266; _LL3240: if(( unsigned int) _temp3238 > 4u?*((
int*) _temp3238) == Cyc_Absyn_ArrayType: 0){ _LL3253: _temp3252=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3238)->f1; goto _LL3251; _LL3251:
_temp3250=(( struct Cyc_Absyn_ArrayType_struct*) _temp3238)->f2; goto _LL3249;
_LL3249: _temp3248=(( struct Cyc_Absyn_ArrayType_struct*) _temp3238)->f3; goto
_LL3241;} else{ goto _LL3242;} _LL3242: if(( unsigned int) _temp3238 > 4u?*((
int*) _temp3238) == Cyc_Absyn_TupleType: 0){ _LL3255: _temp3254=(( struct Cyc_Absyn_TupleType_struct*)
_temp3238)->f1; goto _LL3243;} else{ goto _LL3244;} _LL3244: if(( unsigned int)
_temp3238 > 4u?*(( int*) _temp3238) == Cyc_Absyn_PointerType: 0){ _LL3257:
_temp3256=(( struct Cyc_Absyn_PointerType_struct*) _temp3238)->f1; _LL3267:
_temp3266=( void*) _temp3256.elt_typ; goto _LL3265; _LL3265: _temp3264=( void*)
_temp3256.rgn_typ; goto _LL3263; _LL3263: _temp3262= _temp3256.nullable; goto
_LL3261; _LL3261: _temp3260= _temp3256.tq; goto _LL3259; _LL3259: _temp3258=
_temp3256.bounds; goto _LL3245;} else{ goto _LL3246;} _LL3246: goto _LL3247;
_LL3241: return({ struct _tuple6 _temp3268; _temp3268.f1= _temp3250.q_const;
_temp3268.f2=( Cyc_Tcutil_addressof_props( te, _temp3067)).f2; _temp3268;});
_LL3243: { struct _tuple4* _temp3269= Cyc_Absyn_lookup_tuple_field( _temp3254,(
int) Cyc_Evexp_eval_const_uint_exp( _temp3065)); goto _LL3270; _LL3270: if(
_temp3269 != 0){ return({ struct _tuple6 _temp3271; _temp3271.f1=((*(( struct
_tuple4*) _check_null( _temp3269))).f1).q_const; _temp3271.f2=( Cyc_Tcutil_addressof_props(
te, _temp3067)).f2; _temp3271;});} return bogus_ans;} _LL3245: return({ struct
_tuple6 _temp3272; _temp3272.f1= _temp3260.q_const; _temp3272.f2= _temp3264;
_temp3272;}); _LL3247: return bogus_ans; _LL3239:;} _LL3050: Cyc_Tcutil_terr( e->loc,({
unsigned char* _temp3273="unary & applied to non-lvalue"; struct _tagged_string
_temp3274; _temp3274.curr= _temp3273; _temp3274.base= _temp3273; _temp3274.last_plus_one=
_temp3273 + 30; _temp3274;})); return bogus_ans; _LL3038:;} void* Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv* te, void* e_typ, struct Cyc_Absyn_Exp* e){ void*
_temp3276= Cyc_Tcutil_compress( e_typ); struct Cyc_Absyn_Exp* _temp3282; struct
Cyc_Absyn_Tqual _temp3284; void* _temp3286; _LL3278: if(( unsigned int)
_temp3276 > 4u?*(( int*) _temp3276) == Cyc_Absyn_ArrayType: 0){ _LL3287:
_temp3286=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3276)->f1; goto
_LL3285; _LL3285: _temp3284=(( struct Cyc_Absyn_ArrayType_struct*) _temp3276)->f2;
goto _LL3283; _LL3283: _temp3282=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3276)->f3; goto _LL3279;} else{ goto _LL3280;} _LL3280: goto _LL3281;
_LL3279: { void* _temp3290; int _temp3292; struct _tuple6 _temp3288= Cyc_Tcutil_addressof_props(
te, e); _LL3293: _temp3292= _temp3288.f1; goto _LL3291; _LL3291: _temp3290=
_temp3288.f2; goto _LL3289; _LL3289: return Cyc_Absyn_atb_typ( _temp3286,
_temp3290, _temp3284,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3294=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3294[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3295; _temp3295.tag= Cyc_Absyn_Upper_b;
_temp3295.f1= e; _temp3295;}); _temp3294;}));} _LL3281: return e_typ; _LL3277:;}
void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment* loc, unsigned int i,
struct Cyc_Absyn_Conref* b){ b=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( b);{ void* _temp3296=( void*) b->v; void*
_temp3304; void* _temp3306; struct Cyc_Absyn_Exp* _temp3308; _LL3298: if((
unsigned int) _temp3296 > 1u?*(( int*) _temp3296) == Cyc_Absyn_Eq_constr: 0){
_LL3305: _temp3304=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3296)->f1;
if( _temp3304 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3299;} else{ goto _LL3300;}}
else{ goto _LL3300;} _LL3300: if(( unsigned int) _temp3296 > 1u?*(( int*)
_temp3296) == Cyc_Absyn_Eq_constr: 0){ _LL3307: _temp3306=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3296)->f1; if(( unsigned int) _temp3306 > 1u?*(( int*) _temp3306) == Cyc_Absyn_Upper_b:
0){ _LL3309: _temp3308=(( struct Cyc_Absyn_Upper_b_struct*) _temp3306)->f1; goto
_LL3301;} else{ goto _LL3302;}} else{ goto _LL3302;} _LL3302: goto _LL3303;
_LL3299: return; _LL3301: if( Cyc_Evexp_eval_const_uint_exp( _temp3308) <= i){
Cyc_Tcutil_terr( loc,({ unsigned char* _temp3310="dereference is out of bounds";
struct _tagged_string _temp3311; _temp3311.curr= _temp3310; _temp3311.base=
_temp3310; _temp3311.last_plus_one= _temp3310 + 29; _temp3311;}));} return;
_LL3303:( void*)( b->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3312=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3312[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3313; _temp3313.tag= Cyc_Absyn_Eq_constr;
_temp3313.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3314=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3314[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3315; _temp3315.tag= Cyc_Absyn_Upper_b;
_temp3315.f1= Cyc_Absyn_uint_exp( i + 1, 0); _temp3315;}); _temp3314;}));
_temp3313;}); _temp3312;}))); return; _LL3297:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_bits_only( void* t){ void* _temp3316= Cyc_Tcutil_compress(
t); void* _temp3342; void* _temp3344; struct Cyc_Absyn_Enumdecl* _temp3346;
struct _tuple1* _temp3348; struct Cyc_Absyn_Exp* _temp3350; struct Cyc_Absyn_Tqual
_temp3352; void* _temp3354; struct Cyc_List_List* _temp3356; struct Cyc_Absyn_Structdecl**
_temp3358; struct Cyc_List_List* _temp3360; struct _tuple1* _temp3362; struct
Cyc_Absyn_Uniondecl** _temp3364; struct Cyc_List_List* _temp3366; struct _tuple1*
_temp3368; struct Cyc_List_List* _temp3370; struct Cyc_List_List* _temp3372;
_LL3318: if( _temp3316 ==( void*) Cyc_Absyn_VoidType){ goto _LL3319;} else{ goto
_LL3320;} _LL3320: if(( unsigned int) _temp3316 > 4u?*(( int*) _temp3316) == Cyc_Absyn_IntType:
0){ _LL3345: _temp3344=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3316)->f1;
goto _LL3343; _LL3343: _temp3342=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3316)->f2; goto _LL3321;} else{ goto _LL3322;} _LL3322: if( _temp3316 ==(
void*) Cyc_Absyn_FloatType){ goto _LL3323;} else{ goto _LL3324;} _LL3324: if(
_temp3316 ==( void*) Cyc_Absyn_DoubleType){ goto _LL3325;} else{ goto _LL3326;}
_LL3326: if(( unsigned int) _temp3316 > 4u?*(( int*) _temp3316) == Cyc_Absyn_EnumType:
0){ _LL3349: _temp3348=(( struct Cyc_Absyn_EnumType_struct*) _temp3316)->f1;
goto _LL3347; _LL3347: _temp3346=(( struct Cyc_Absyn_EnumType_struct*) _temp3316)->f2;
goto _LL3327;} else{ goto _LL3328;} _LL3328: if(( unsigned int) _temp3316 > 4u?*((
int*) _temp3316) == Cyc_Absyn_ArrayType: 0){ _LL3355: _temp3354=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3316)->f1; goto _LL3353; _LL3353:
_temp3352=(( struct Cyc_Absyn_ArrayType_struct*) _temp3316)->f2; goto _LL3351;
_LL3351: _temp3350=(( struct Cyc_Absyn_ArrayType_struct*) _temp3316)->f3; goto
_LL3329;} else{ goto _LL3330;} _LL3330: if(( unsigned int) _temp3316 > 4u?*((
int*) _temp3316) == Cyc_Absyn_TupleType: 0){ _LL3357: _temp3356=(( struct Cyc_Absyn_TupleType_struct*)
_temp3316)->f1; goto _LL3331;} else{ goto _LL3332;} _LL3332: if(( unsigned int)
_temp3316 > 4u?*(( int*) _temp3316) == Cyc_Absyn_StructType: 0){ _LL3363:
_temp3362=(( struct Cyc_Absyn_StructType_struct*) _temp3316)->f1; goto _LL3361;
_LL3361: _temp3360=(( struct Cyc_Absyn_StructType_struct*) _temp3316)->f2; goto
_LL3359; _LL3359: _temp3358=(( struct Cyc_Absyn_StructType_struct*) _temp3316)->f3;
goto _LL3333;} else{ goto _LL3334;} _LL3334: if(( unsigned int) _temp3316 > 4u?*((
int*) _temp3316) == Cyc_Absyn_UnionType: 0){ _LL3369: _temp3368=(( struct Cyc_Absyn_UnionType_struct*)
_temp3316)->f1; goto _LL3367; _LL3367: _temp3366=(( struct Cyc_Absyn_UnionType_struct*)
_temp3316)->f2; goto _LL3365; _LL3365: _temp3364=(( struct Cyc_Absyn_UnionType_struct*)
_temp3316)->f3; goto _LL3335;} else{ goto _LL3336;} _LL3336: if(( unsigned int)
_temp3316 > 4u?*(( int*) _temp3316) == Cyc_Absyn_AnonStructType: 0){ _LL3371:
_temp3370=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3316)->f1; goto
_LL3337;} else{ goto _LL3338;} _LL3338: if(( unsigned int) _temp3316 > 4u?*((
int*) _temp3316) == Cyc_Absyn_AnonUnionType: 0){ _LL3373: _temp3372=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3316)->f1; goto _LL3339;} else{ goto
_LL3340;} _LL3340: goto _LL3341; _LL3319: return 1; _LL3321: return 1; _LL3323:
return 1; _LL3325: return 1; _LL3327: return 1; _LL3329: return Cyc_Tcutil_bits_only(
_temp3354); _LL3331: for( 0; _temp3356 != 0; _temp3356=(( struct Cyc_List_List*)
_check_null( _temp3356))->tl){ if( ! Cyc_Tcutil_bits_only((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp3356))->hd)).f2)){ return 0;}} return 1;
_LL3333: if( _temp3358 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* _temp3374=*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp3358)); goto _LL3375; _LL3375:
if( _temp3374->fields == 0){ return 0;}{ struct _RegionHandle _temp3376=
_new_region(); struct _RegionHandle* rgn=& _temp3376; _push_region( rgn);{
struct Cyc_List_List* _temp3377=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp3374->tvs, _temp3360); goto _LL3378; _LL3378:{
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp3374->fields))->v; for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute( rgn,
_temp3377,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3379= 0; _npop_handler( 0u); return
_temp3379;}}}{ int _temp3380= 1; _npop_handler( 0u); return _temp3380;}};
_pop_region(& _temp3376);}} _LL3335: if( _temp3364 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
_temp3381=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp3364)); goto
_LL3382; _LL3382: if( _temp3381->fields == 0){ return 0;}{ struct _RegionHandle
_temp3383= _new_region(); struct _RegionHandle* rgn=& _temp3383; _push_region(
rgn);{ struct Cyc_List_List* _temp3384=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp3381->tvs, _temp3366); goto _LL3385; _LL3385:{
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp3381->fields))->v; for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute( rgn,
_temp3384,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3386= 0; _npop_handler( 0u); return
_temp3386;}}}{ int _temp3387= 1; _npop_handler( 0u); return _temp3387;}};
_pop_region(& _temp3383);}} _LL3337: _temp3372= _temp3370; goto _LL3339; _LL3339:
for( 0; _temp3372 != 0; _temp3372=(( struct Cyc_List_List*) _check_null(
_temp3372))->tl){ if( ! Cyc_Tcutil_bits_only(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp3372))->hd)->type)){ return 0;}} return
1; _LL3341: return 0; _LL3317:;} struct _tuple19{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; static int Cyc_Tcutil_cnst_exp( struct Cyc_Tcenv_Tenv*
te, int var_okay, struct Cyc_Absyn_Exp* e){ void* _temp3388=( void*) e->r; void*
_temp3428; void* _temp3430; struct Cyc_Absyn_Exp* _temp3432; struct Cyc_Absyn_Enumfield*
_temp3434; struct Cyc_Absyn_Enumdecl* _temp3436; struct _tuple1* _temp3438; void*
_temp3440; struct _tuple1* _temp3442; struct Cyc_Absyn_Exp* _temp3444; struct
Cyc_Absyn_Exp* _temp3446; struct Cyc_Absyn_Exp* _temp3448; struct Cyc_Absyn_Exp*
_temp3450; struct Cyc_Absyn_Exp* _temp3452; struct Cyc_Absyn_Exp* _temp3454;
struct Cyc_List_List* _temp3456; struct Cyc_Absyn_Exp* _temp3458; struct Cyc_Absyn_Exp*
_temp3460; void* _temp3462; struct Cyc_Absyn_Exp* _temp3464; struct Cyc_Absyn_Exp*
_temp3466; struct Cyc_Absyn_Exp* _temp3468; struct Cyc_Absyn_Vardecl* _temp3470;
struct Cyc_List_List* _temp3472; struct Cyc_List_List* _temp3474; void*
_temp3476; struct Cyc_Absyn_Structdecl* _temp3478; struct Cyc_List_List*
_temp3480; struct Cyc_Core_Opt* _temp3482; struct _tuple1* _temp3484; struct Cyc_List_List*
_temp3486; void* _temp3488; struct Cyc_List_List* _temp3490; struct Cyc_Absyn_Tunionfield*
_temp3492; struct Cyc_Absyn_Tuniondecl* _temp3494; struct Cyc_List_List*
_temp3496; struct Cyc_Core_Opt* _temp3498; struct Cyc_Core_Opt* _temp3500;
_LL3390: if(*(( int*) _temp3388) == Cyc_Absyn_Const_e){ _LL3429: _temp3428=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp3388)->f1; goto _LL3391;} else{
goto _LL3392;} _LL3392: if(*(( int*) _temp3388) == Cyc_Absyn_Sizeoftyp_e){
_LL3431: _temp3430=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp3388)->f1;
goto _LL3393;} else{ goto _LL3394;} _LL3394: if(*(( int*) _temp3388) == Cyc_Absyn_Sizeofexp_e){
_LL3433: _temp3432=(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp3388)->f1; goto
_LL3395;} else{ goto _LL3396;} _LL3396: if(*(( int*) _temp3388) == Cyc_Absyn_Enum_e){
_LL3439: _temp3438=(( struct Cyc_Absyn_Enum_e_struct*) _temp3388)->f1; goto
_LL3437; _LL3437: _temp3436=(( struct Cyc_Absyn_Enum_e_struct*) _temp3388)->f2;
goto _LL3435; _LL3435: _temp3434=(( struct Cyc_Absyn_Enum_e_struct*) _temp3388)->f3;
goto _LL3397;} else{ goto _LL3398;} _LL3398: if(*(( int*) _temp3388) == Cyc_Absyn_Var_e){
_LL3443: _temp3442=(( struct Cyc_Absyn_Var_e_struct*) _temp3388)->f1; goto
_LL3441; _LL3441: _temp3440=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp3388)->f2;
goto _LL3399;} else{ goto _LL3400;} _LL3400: if(*(( int*) _temp3388) == Cyc_Absyn_Conditional_e){
_LL3449: _temp3448=(( struct Cyc_Absyn_Conditional_e_struct*) _temp3388)->f1;
goto _LL3447; _LL3447: _temp3446=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3388)->f2; goto _LL3445; _LL3445: _temp3444=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3388)->f3; goto _LL3401;} else{ goto _LL3402;} _LL3402: if(*(( int*)
_temp3388) == Cyc_Absyn_SeqExp_e){ _LL3453: _temp3452=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3388)->f1; goto _LL3451; _LL3451: _temp3450=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3388)->f2; goto _LL3403;} else{ goto _LL3404;} _LL3404: if(*(( int*)
_temp3388) == Cyc_Absyn_NoInstantiate_e){ _LL3455: _temp3454=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp3388)->f1; goto _LL3405;} else{ goto _LL3406;} _LL3406: if(*(( int*)
_temp3388) == Cyc_Absyn_Instantiate_e){ _LL3459: _temp3458=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3388)->f1; goto _LL3457; _LL3457: _temp3456=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3388)->f2; goto _LL3407;} else{ goto _LL3408;} _LL3408: if(*(( int*)
_temp3388) == Cyc_Absyn_Cast_e){ _LL3463: _temp3462=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp3388)->f1; goto _LL3461; _LL3461: _temp3460=(( struct Cyc_Absyn_Cast_e_struct*)
_temp3388)->f2; goto _LL3409;} else{ goto _LL3410;} _LL3410: if(*(( int*)
_temp3388) == Cyc_Absyn_Address_e){ _LL3465: _temp3464=(( struct Cyc_Absyn_Address_e_struct*)
_temp3388)->f1; goto _LL3411;} else{ goto _LL3412;} _LL3412: if(*(( int*)
_temp3388) == Cyc_Absyn_Comprehension_e){ _LL3471: _temp3470=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3388)->f1; goto _LL3469; _LL3469: _temp3468=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3388)->f2; goto _LL3467; _LL3467: _temp3466=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3388)->f3; goto _LL3413;} else{ goto _LL3414;} _LL3414: if(*(( int*)
_temp3388) == Cyc_Absyn_Array_e){ _LL3473: _temp3472=(( struct Cyc_Absyn_Array_e_struct*)
_temp3388)->f1; goto _LL3415;} else{ goto _LL3416;} _LL3416: if(*(( int*)
_temp3388) == Cyc_Absyn_AnonStruct_e){ _LL3477: _temp3476=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3388)->f1; goto _LL3475; _LL3475: _temp3474=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3388)->f2; goto _LL3417;} else{ goto _LL3418;} _LL3418: if(*(( int*)
_temp3388) == Cyc_Absyn_Struct_e){ _LL3485: _temp3484=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3388)->f1; goto _LL3483; _LL3483: _temp3482=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3388)->f2; goto _LL3481; _LL3481: _temp3480=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3388)->f3; goto _LL3479; _LL3479: _temp3478=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3388)->f4; goto _LL3419;} else{ goto _LL3420;} _LL3420: if(*(( int*)
_temp3388) == Cyc_Absyn_Primop_e){ _LL3489: _temp3488=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp3388)->f1; goto _LL3487; _LL3487: _temp3486=(( struct Cyc_Absyn_Primop_e_struct*)
_temp3388)->f2; goto _LL3421;} else{ goto _LL3422;} _LL3422: if(*(( int*)
_temp3388) == Cyc_Absyn_Tuple_e){ _LL3491: _temp3490=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp3388)->f1; goto _LL3423;} else{ goto _LL3424;} _LL3424: if(*(( int*)
_temp3388) == Cyc_Absyn_Tunion_e){ _LL3501: _temp3500=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3388)->f1; goto _LL3499; _LL3499: _temp3498=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3388)->f2; goto _LL3497; _LL3497: _temp3496=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3388)->f3; goto _LL3495; _LL3495: _temp3494=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3388)->f4; goto _LL3493; _LL3493: _temp3492=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3388)->f5; goto _LL3425;} else{ goto _LL3426;} _LL3426: goto _LL3427;
_LL3391: return 1; _LL3393: return 1; _LL3395: return Cyc_Tcutil_cnst_exp( te, 1,
_temp3432); _LL3397: return 1; _LL3399: { struct _handler_cons _temp3502;
_push_handler(& _temp3502);{ int _temp3504= 0; if( setjmp( _temp3502.handler)){
_temp3504= 1;} if( ! _temp3504){{ void* _temp3505= Cyc_Tcenv_lookup_ordinary( te,
e->loc, _temp3442); void* _temp3513; struct Cyc_Absyn_Fndecl* _temp3515; void*
_temp3517; struct Cyc_Absyn_Vardecl* _temp3519; _LL3507: if(*(( int*) _temp3505)
== Cyc_Tcenv_VarRes){ _LL3514: _temp3513=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3505)->f1; if(( unsigned int) _temp3513 > 1u?*(( int*) _temp3513) == Cyc_Absyn_Funname_b:
0){ _LL3516: _temp3515=(( struct Cyc_Absyn_Funname_b_struct*) _temp3513)->f1;
goto _LL3508;} else{ goto _LL3509;}} else{ goto _LL3509;} _LL3509: if(*(( int*)
_temp3505) == Cyc_Tcenv_VarRes){ _LL3518: _temp3517=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3505)->f1; if(( unsigned int) _temp3517 > 1u?*(( int*) _temp3517) == Cyc_Absyn_Global_b:
0){ _LL3520: _temp3519=(( struct Cyc_Absyn_Global_b_struct*) _temp3517)->f1;
goto _LL3510;} else{ goto _LL3511;}} else{ goto _LL3511;} _LL3511: goto _LL3512;
_LL3508: { int _temp3521= 1; _npop_handler( 0u); return _temp3521;} _LL3510: {
void* _temp3522= Cyc_Tcutil_compress(( void*) _temp3519->type); struct Cyc_Absyn_Exp*
_temp3528; struct Cyc_Absyn_Tqual _temp3530; void* _temp3532; _LL3524: if((
unsigned int) _temp3522 > 4u?*(( int*) _temp3522) == Cyc_Absyn_ArrayType: 0){
_LL3533: _temp3532=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3522)->f1;
goto _LL3531; _LL3531: _temp3530=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3522)->f2; goto _LL3529; _LL3529: _temp3528=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3522)->f3; goto _LL3525;} else{ goto _LL3526;} _LL3526: goto _LL3527;
_LL3525: { int _temp3534= 1; _npop_handler( 0u); return _temp3534;} _LL3527: {
int _temp3535= var_okay; _npop_handler( 0u); return _temp3535;} _LL3523:;}
_LL3512: { int _temp3536= var_okay; _npop_handler( 0u); return _temp3536;}
_LL3506:;}; _pop_handler();} else{ void* _temp3503=( void*) _exn_thrown; void*
_temp3538= _temp3503; _LL3540: if( _temp3538 == Cyc_Dict_Absent){ goto _LL3541;}
else{ goto _LL3542;} _LL3542: goto _LL3543; _LL3541: return 0; _LL3543:( void)
_throw( _temp3538); _LL3539:;}}} _LL3401: return( Cyc_Tcutil_cnst_exp( te, 0,
_temp3448)? Cyc_Tcutil_cnst_exp( te, 0, _temp3446): 0)? Cyc_Tcutil_cnst_exp( te,
0, _temp3444): 0; _LL3403: return Cyc_Tcutil_cnst_exp( te, 0, _temp3452)? Cyc_Tcutil_cnst_exp(
te, 0, _temp3450): 0; _LL3405: _temp3458= _temp3454; goto _LL3407; _LL3407:
return Cyc_Tcutil_cnst_exp( te, var_okay, _temp3458); _LL3409: return Cyc_Tcutil_cnst_exp(
te, var_okay, _temp3460); _LL3411: return Cyc_Tcutil_cnst_exp( te, 1, _temp3464);
_LL3413: return Cyc_Tcutil_cnst_exp( te, 0, _temp3468)? Cyc_Tcutil_cnst_exp( te,
0, _temp3466): 0; _LL3415: _temp3474= _temp3472; goto _LL3417; _LL3417:
_temp3480= _temp3474; goto _LL3419; _LL3419: for( 0; _temp3480 != 0; _temp3480=((
struct Cyc_List_List*) _check_null( _temp3480))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,(*(( struct _tuple19*)(( struct Cyc_List_List*) _check_null( _temp3480))->hd)).f2)){
return 0;}} return 1; _LL3421: _temp3490= _temp3486; goto _LL3423; _LL3423:
_temp3496= _temp3490; goto _LL3425; _LL3425: for( 0; _temp3496 != 0; _temp3496=((
struct Cyc_List_List*) _check_null( _temp3496))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp3496))->hd)){
return 0;}} return 1; _LL3427: return 0; _LL3389:;} int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcutil_cnst_exp(
te, 0, e);} static int Cyc_Tcutil_fields_support_default( struct Cyc_List_List*
tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs); int Cyc_Tcutil_supports_default(
void* t){ void* _temp3544= Cyc_Tcutil_compress( t); int _temp3582; struct Cyc_Core_Opt*
_temp3584; void* _temp3586; struct Cyc_Absyn_Tvar* _temp3588; struct Cyc_Absyn_TunionInfo
_temp3590; struct Cyc_Absyn_TunionFieldInfo _temp3592; struct Cyc_Absyn_PtrInfo
_temp3594; struct Cyc_Absyn_Conref* _temp3596; struct Cyc_Absyn_Tqual _temp3598;
struct Cyc_Absyn_Conref* _temp3600; void* _temp3602; void* _temp3604; void*
_temp3606; void* _temp3608; struct Cyc_Absyn_Exp* _temp3610; struct Cyc_Absyn_Tqual
_temp3612; void* _temp3614; struct Cyc_Absyn_FnInfo _temp3616; struct Cyc_List_List*
_temp3618; struct Cyc_Absyn_Structdecl** _temp3620; struct Cyc_List_List*
_temp3622; struct _tuple1* _temp3624; struct Cyc_Absyn_Uniondecl** _temp3626;
struct Cyc_List_List* _temp3628; struct _tuple1* _temp3630; struct Cyc_List_List*
_temp3632; struct Cyc_List_List* _temp3634; struct Cyc_Absyn_Enumdecl* _temp3636;
struct _tuple1* _temp3638; _LL3546: if( _temp3544 ==( void*) Cyc_Absyn_VoidType){
goto _LL3547;} else{ goto _LL3548;} _LL3548: if(( unsigned int) _temp3544 > 4u?*((
int*) _temp3544) == Cyc_Absyn_Evar: 0){ _LL3587: _temp3586=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp3544)->f1; goto _LL3585; _LL3585: _temp3584=(( struct Cyc_Absyn_Evar_struct*)
_temp3544)->f2; goto _LL3583; _LL3583: _temp3582=(( struct Cyc_Absyn_Evar_struct*)
_temp3544)->f3; goto _LL3549;} else{ goto _LL3550;} _LL3550: if(( unsigned int)
_temp3544 > 4u?*(( int*) _temp3544) == Cyc_Absyn_VarType: 0){ _LL3589: _temp3588=((
struct Cyc_Absyn_VarType_struct*) _temp3544)->f1; goto _LL3551;} else{ goto
_LL3552;} _LL3552: if(( unsigned int) _temp3544 > 4u?*(( int*) _temp3544) == Cyc_Absyn_TunionType:
0){ _LL3591: _temp3590=(( struct Cyc_Absyn_TunionType_struct*) _temp3544)->f1;
goto _LL3553;} else{ goto _LL3554;} _LL3554: if(( unsigned int) _temp3544 > 4u?*((
int*) _temp3544) == Cyc_Absyn_TunionFieldType: 0){ _LL3593: _temp3592=(( struct
Cyc_Absyn_TunionFieldType_struct*) _temp3544)->f1; goto _LL3555;} else{ goto
_LL3556;} _LL3556: if(( unsigned int) _temp3544 > 4u?*(( int*) _temp3544) == Cyc_Absyn_PointerType:
0){ _LL3595: _temp3594=(( struct Cyc_Absyn_PointerType_struct*) _temp3544)->f1;
_LL3605: _temp3604=( void*) _temp3594.elt_typ; goto _LL3603; _LL3603: _temp3602=(
void*) _temp3594.rgn_typ; goto _LL3601; _LL3601: _temp3600= _temp3594.nullable;
goto _LL3599; _LL3599: _temp3598= _temp3594.tq; goto _LL3597; _LL3597: _temp3596=
_temp3594.bounds; goto _LL3557;} else{ goto _LL3558;} _LL3558: if(( unsigned int)
_temp3544 > 4u?*(( int*) _temp3544) == Cyc_Absyn_IntType: 0){ _LL3609: _temp3608=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp3544)->f1; goto _LL3607; _LL3607:
_temp3606=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3544)->f2; goto
_LL3559;} else{ goto _LL3560;} _LL3560: if( _temp3544 ==( void*) Cyc_Absyn_FloatType){
goto _LL3561;} else{ goto _LL3562;} _LL3562: if( _temp3544 ==( void*) Cyc_Absyn_DoubleType){
goto _LL3563;} else{ goto _LL3564;} _LL3564: if(( unsigned int) _temp3544 > 4u?*((
int*) _temp3544) == Cyc_Absyn_ArrayType: 0){ _LL3615: _temp3614=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3544)->f1; goto _LL3613; _LL3613:
_temp3612=(( struct Cyc_Absyn_ArrayType_struct*) _temp3544)->f2; goto _LL3611;
_LL3611: _temp3610=(( struct Cyc_Absyn_ArrayType_struct*) _temp3544)->f3; goto
_LL3565;} else{ goto _LL3566;} _LL3566: if(( unsigned int) _temp3544 > 4u?*((
int*) _temp3544) == Cyc_Absyn_FnType: 0){ _LL3617: _temp3616=(( struct Cyc_Absyn_FnType_struct*)
_temp3544)->f1; goto _LL3567;} else{ goto _LL3568;} _LL3568: if(( unsigned int)
_temp3544 > 4u?*(( int*) _temp3544) == Cyc_Absyn_TupleType: 0){ _LL3619:
_temp3618=(( struct Cyc_Absyn_TupleType_struct*) _temp3544)->f1; goto _LL3569;}
else{ goto _LL3570;} _LL3570: if(( unsigned int) _temp3544 > 4u?*(( int*)
_temp3544) == Cyc_Absyn_StructType: 0){ _LL3625: _temp3624=(( struct Cyc_Absyn_StructType_struct*)
_temp3544)->f1; goto _LL3623; _LL3623: _temp3622=(( struct Cyc_Absyn_StructType_struct*)
_temp3544)->f2; goto _LL3621; _LL3621: _temp3620=(( struct Cyc_Absyn_StructType_struct*)
_temp3544)->f3; goto _LL3571;} else{ goto _LL3572;} _LL3572: if(( unsigned int)
_temp3544 > 4u?*(( int*) _temp3544) == Cyc_Absyn_UnionType: 0){ _LL3631:
_temp3630=(( struct Cyc_Absyn_UnionType_struct*) _temp3544)->f1; goto _LL3629;
_LL3629: _temp3628=(( struct Cyc_Absyn_UnionType_struct*) _temp3544)->f2; goto
_LL3627; _LL3627: _temp3626=(( struct Cyc_Absyn_UnionType_struct*) _temp3544)->f3;
goto _LL3573;} else{ goto _LL3574;} _LL3574: if(( unsigned int) _temp3544 > 4u?*((
int*) _temp3544) == Cyc_Absyn_AnonStructType: 0){ _LL3633: _temp3632=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp3544)->f1; goto _LL3575;} else{ goto
_LL3576;} _LL3576: if(( unsigned int) _temp3544 > 4u?*(( int*) _temp3544) == Cyc_Absyn_AnonUnionType:
0){ _LL3635: _temp3634=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3544)->f1;
goto _LL3577;} else{ goto _LL3578;} _LL3578: if(( unsigned int) _temp3544 > 4u?*((
int*) _temp3544) == Cyc_Absyn_EnumType: 0){ _LL3639: _temp3638=(( struct Cyc_Absyn_EnumType_struct*)
_temp3544)->f1; goto _LL3637; _LL3637: _temp3636=(( struct Cyc_Absyn_EnumType_struct*)
_temp3544)->f2; goto _LL3579;} else{ goto _LL3580;} _LL3580: goto _LL3581;
_LL3547: return 1; _LL3549: return 0; _LL3551: return 0; _LL3553: return 0;
_LL3555: return 0; _LL3557: { void* _temp3640=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3596))->v; void*
_temp3648; void* _temp3650; struct Cyc_Absyn_Exp* _temp3652; _LL3642: if((
unsigned int) _temp3640 > 1u?*(( int*) _temp3640) == Cyc_Absyn_Eq_constr: 0){
_LL3649: _temp3648=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3640)->f1;
if( _temp3648 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3643;} else{ goto _LL3644;}}
else{ goto _LL3644;} _LL3644: if(( unsigned int) _temp3640 > 1u?*(( int*)
_temp3640) == Cyc_Absyn_Eq_constr: 0){ _LL3651: _temp3650=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3640)->f1; if(( unsigned int) _temp3650 > 1u?*(( int*) _temp3650) == Cyc_Absyn_Upper_b:
0){ _LL3653: _temp3652=(( struct Cyc_Absyn_Upper_b_struct*) _temp3650)->f1; goto
_LL3645;} else{ goto _LL3646;}} else{ goto _LL3646;} _LL3646: goto _LL3647;
_LL3643: return 1; _LL3645: { void* _temp3654=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3600))->v; int
_temp3660; _LL3656: if(( unsigned int) _temp3654 > 1u?*(( int*) _temp3654) ==
Cyc_Absyn_Eq_constr: 0){ _LL3661: _temp3660=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3654)->f1; goto _LL3657;} else{ goto _LL3658;} _LL3658: goto _LL3659;
_LL3657: return _temp3660; _LL3659: return 0; _LL3655:;} _LL3647: return 0;
_LL3641:;} _LL3559: return 1; _LL3561: return 1; _LL3563: return 1; _LL3565:
return Cyc_Tcutil_supports_default( _temp3614); _LL3567: return 0; _LL3569: for(
0; _temp3618 != 0; _temp3618=(( struct Cyc_List_List*) _check_null( _temp3618))->tl){
if( ! Cyc_Tcutil_supports_default((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp3618))->hd)).f2)){ return 0;}} return 1; _LL3571: if(
_temp3620 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp3620)); if( sd->fields == 0){ return 0;} return Cyc_Tcutil_fields_support_default(
sd->tvs, _temp3622,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
sd->fields))->v);} _LL3573: if( _temp3626 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp3626)); if( ud->fields ==
0){ return 0;} return Cyc_Tcutil_fields_support_default( ud->tvs, _temp3628,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ud->fields))->v);}
_LL3575: _temp3634= _temp3632; goto _LL3577; _LL3577: return Cyc_Tcutil_fields_support_default(
0, 0, _temp3634); _LL3579: return 1; _LL3581: return 0; _LL3545:;} static int
Cyc_Tcutil_fields_support_default( struct Cyc_List_List* tvs, struct Cyc_List_List*
ts, struct Cyc_List_List* fs){{ struct _RegionHandle _temp3662= _new_region();
struct _RegionHandle* rgn=& _temp3662; _push_region( rgn);{ struct Cyc_List_List*
_temp3663=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, tvs, ts); goto _LL3664; _LL3664: for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ void* t= Cyc_Tcutil_rsubstitute( rgn, _temp3663,( void*)((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( fs))->hd)->type);
if( ! Cyc_Tcutil_supports_default( t)){ int _temp3665= 0; _npop_handler( 0u);
return _temp3665;}}}; _pop_region(& _temp3662);} return 1;}