 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
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
extern struct _tagged_string Cstring_to_string( unsigned char*); struct
_tagged_ptr0{ struct _tagged_string* curr; struct _tagged_string* base; struct
_tagged_string* last_plus_one; } ; extern struct _tagged_ptr0 ntCsl_to_ntsl(
unsigned char**); extern int system( unsigned char*); extern int* __errno();
struct Cyc_Stdio___sFILE; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr;
extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*); extern unsigned char
Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[ 18u];
struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct _tagged_string
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
Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_List_exists_c(
int(* pred)( void*, void*), void* env, struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_List_zip( struct Cyc_List_List* x, struct Cyc_List_List* y);
extern struct Cyc_List_List* Cyc_List_rzip( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y); struct
_tuple0{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ; extern struct
_tuple0 Cyc_List_rsplit( struct _RegionHandle* r3, struct _RegionHandle* r4,
struct Cyc_List_List* x); extern int Cyc_List_mem( int(* compare)( void*, void*),
struct Cyc_List_List* l, void* x); extern void* Cyc_List_assoc_cmp( int(*
compare)( void*, void*), struct Cyc_List_List* l, void* x); struct Cyc_Lineno_Pos{
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
static const int Cyc_Absyn_Offsetof_e= 18; struct Cyc_Absyn_Offsetof_e_struct{
int tag; void* f1; struct _tagged_string* f2; } ; static const int Cyc_Absyn_Deref_e=
19; struct Cyc_Absyn_Deref_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_StructMember_e= 20; struct Cyc_Absyn_StructMember_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tagged_string* f2; } ; static const
int Cyc_Absyn_StructArrow_e= 21; struct Cyc_Absyn_StructArrow_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct _tagged_string* f2; } ; static const int Cyc_Absyn_Subscript_e=
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
cyc_varargs; struct Cyc_List_List* rgn_po; struct Cyc_Absyn_Stmt* body; struct
Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structfield{ struct _tagged_string* name;
struct Cyc_Absyn_Tqual tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List*
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
int tag; struct _tagged_string* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{ int tag; struct
_tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
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
struct _tuple1*); extern int Cyc_Tcenv_region_outlives( struct Cyc_Tcenv_Tenv*,
void* r1, void* r2); extern int Cyc_String_strcmp( struct _tagged_string s1,
struct _tagged_string s2); extern int Cyc_String_zstrptrcmp( struct
_tagged_string*, struct _tagged_string*); extern struct _tagged_string Cyc_String_strconcat(
struct _tagged_string, struct _tagged_string); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern
void* Cyc_Tcutil_impos( struct _tagged_string); extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_string); extern void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*, struct _tagged_string); extern void Cyc_Tcutil_flush_warnings();
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
_temp83;});} struct _tuple8{ void* f1; void* f2; } ; static struct _tuple8* Cyc_Tcutil_copy_rgncmp(
struct _tuple8* x){ struct _tuple8 _temp86; void* _temp87; void* _temp89; struct
_tuple8* _temp84= x; _temp86=* _temp84; _LL90: _temp89= _temp86.f1; goto _LL88;
_LL88: _temp87= _temp86.f2; goto _LL85; _LL85: return({ struct _tuple8* _temp91=(
struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp91->f1= Cyc_Tcutil_copy_type(
_temp89); _temp91->f2= Cyc_Tcutil_copy_type( _temp87); _temp91;});} void* Cyc_Tcutil_copy_type(
void* t){ void* _temp92= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt* _temp138;
int _temp140; struct Cyc_Core_Opt* _temp142; struct Cyc_Core_Opt* _temp144;
struct Cyc_Absyn_Tvar* _temp146; struct Cyc_Absyn_TunionInfo _temp148; void*
_temp150; struct Cyc_List_List* _temp152; void* _temp154; struct Cyc_Absyn_TunionFieldInfo
_temp156; struct Cyc_List_List* _temp158; void* _temp160; struct Cyc_Absyn_PtrInfo
_temp162; struct Cyc_Absyn_Conref* _temp164; struct Cyc_Absyn_Tqual _temp166;
struct Cyc_Absyn_Conref* _temp168; void* _temp170; void* _temp172; void*
_temp174; void* _temp176; struct Cyc_Absyn_Exp* _temp178; struct Cyc_Absyn_Tqual
_temp180; void* _temp182; struct Cyc_Absyn_FnInfo _temp184; struct Cyc_List_List*
_temp186; struct Cyc_List_List* _temp188; struct Cyc_Absyn_VarargInfo* _temp190;
int _temp192; struct Cyc_List_List* _temp194; void* _temp196; struct Cyc_Core_Opt*
_temp198; struct Cyc_List_List* _temp200; struct Cyc_List_List* _temp202; struct
Cyc_Absyn_Structdecl** _temp204; struct Cyc_List_List* _temp206; struct _tuple1*
_temp208; struct Cyc_Absyn_Uniondecl** _temp210; struct Cyc_List_List* _temp212;
struct _tuple1* _temp214; struct Cyc_List_List* _temp216; struct Cyc_List_List*
_temp218; struct Cyc_Absyn_Enumdecl* _temp220; struct _tuple1* _temp222; void*
_temp224; struct Cyc_Core_Opt* _temp226; struct Cyc_List_List* _temp228; struct
_tuple1* _temp230; void* _temp232; struct Cyc_List_List* _temp234; _LL94: if(
_temp92 ==( void*) Cyc_Absyn_VoidType){ goto _LL95;} else{ goto _LL96;} _LL96:
if(( unsigned int) _temp92 > 4u?*(( int*) _temp92) == Cyc_Absyn_Evar: 0){ _LL145:
_temp144=(( struct Cyc_Absyn_Evar_struct*) _temp92)->f1; goto _LL143; _LL143:
_temp142=(( struct Cyc_Absyn_Evar_struct*) _temp92)->f2; goto _LL141; _LL141:
_temp140=(( struct Cyc_Absyn_Evar_struct*) _temp92)->f3; goto _LL139; _LL139:
_temp138=(( struct Cyc_Absyn_Evar_struct*) _temp92)->f4; goto _LL97;} else{ goto
_LL98;} _LL98: if(( unsigned int) _temp92 > 4u?*(( int*) _temp92) == Cyc_Absyn_VarType:
0){ _LL147: _temp146=(( struct Cyc_Absyn_VarType_struct*) _temp92)->f1; goto
_LL99;} else{ goto _LL100;} _LL100: if(( unsigned int) _temp92 > 4u?*(( int*)
_temp92) == Cyc_Absyn_TunionType: 0){ _LL149: _temp148=(( struct Cyc_Absyn_TunionType_struct*)
_temp92)->f1; _LL155: _temp154=( void*) _temp148.tunion_info; goto _LL153;
_LL153: _temp152= _temp148.targs; goto _LL151; _LL151: _temp150=( void*)
_temp148.rgn; goto _LL101;} else{ goto _LL102;} _LL102: if(( unsigned int)
_temp92 > 4u?*(( int*) _temp92) == Cyc_Absyn_TunionFieldType: 0){ _LL157:
_temp156=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp92)->f1; _LL161:
_temp160=( void*) _temp156.field_info; goto _LL159; _LL159: _temp158= _temp156.targs;
goto _LL103;} else{ goto _LL104;} _LL104: if(( unsigned int) _temp92 > 4u?*((
int*) _temp92) == Cyc_Absyn_PointerType: 0){ _LL163: _temp162=(( struct Cyc_Absyn_PointerType_struct*)
_temp92)->f1; _LL173: _temp172=( void*) _temp162.elt_typ; goto _LL171; _LL171:
_temp170=( void*) _temp162.rgn_typ; goto _LL169; _LL169: _temp168= _temp162.nullable;
goto _LL167; _LL167: _temp166= _temp162.tq; goto _LL165; _LL165: _temp164=
_temp162.bounds; goto _LL105;} else{ goto _LL106;} _LL106: if(( unsigned int)
_temp92 > 4u?*(( int*) _temp92) == Cyc_Absyn_IntType: 0){ _LL177: _temp176=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp92)->f1; goto _LL175; _LL175:
_temp174=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp92)->f2; goto _LL107;}
else{ goto _LL108;} _LL108: if( _temp92 ==( void*) Cyc_Absyn_FloatType){ goto
_LL109;} else{ goto _LL110;} _LL110: if( _temp92 ==( void*) Cyc_Absyn_DoubleType){
goto _LL111;} else{ goto _LL112;} _LL112: if(( unsigned int) _temp92 > 4u?*((
int*) _temp92) == Cyc_Absyn_ArrayType: 0){ _LL183: _temp182=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp92)->f1; goto _LL181; _LL181: _temp180=((
struct Cyc_Absyn_ArrayType_struct*) _temp92)->f2; goto _LL179; _LL179: _temp178=((
struct Cyc_Absyn_ArrayType_struct*) _temp92)->f3; goto _LL113;} else{ goto
_LL114;} _LL114: if(( unsigned int) _temp92 > 4u?*(( int*) _temp92) == Cyc_Absyn_FnType:
0){ _LL185: _temp184=(( struct Cyc_Absyn_FnType_struct*) _temp92)->f1; _LL201:
_temp200= _temp184.tvars; goto _LL199; _LL199: _temp198= _temp184.effect; goto
_LL197; _LL197: _temp196=( void*) _temp184.ret_typ; goto _LL195; _LL195:
_temp194= _temp184.args; goto _LL193; _LL193: _temp192= _temp184.c_varargs; goto
_LL191; _LL191: _temp190= _temp184.cyc_varargs; goto _LL189; _LL189: _temp188=
_temp184.rgn_po; goto _LL187; _LL187: _temp186= _temp184.attributes; goto _LL115;}
else{ goto _LL116;} _LL116: if(( unsigned int) _temp92 > 4u?*(( int*) _temp92)
== Cyc_Absyn_TupleType: 0){ _LL203: _temp202=(( struct Cyc_Absyn_TupleType_struct*)
_temp92)->f1; goto _LL117;} else{ goto _LL118;} _LL118: if(( unsigned int)
_temp92 > 4u?*(( int*) _temp92) == Cyc_Absyn_StructType: 0){ _LL209: _temp208=((
struct Cyc_Absyn_StructType_struct*) _temp92)->f1; goto _LL207; _LL207: _temp206=((
struct Cyc_Absyn_StructType_struct*) _temp92)->f2; goto _LL205; _LL205: _temp204=((
struct Cyc_Absyn_StructType_struct*) _temp92)->f3; goto _LL119;} else{ goto
_LL120;} _LL120: if(( unsigned int) _temp92 > 4u?*(( int*) _temp92) == Cyc_Absyn_UnionType:
0){ _LL215: _temp214=(( struct Cyc_Absyn_UnionType_struct*) _temp92)->f1; goto
_LL213; _LL213: _temp212=(( struct Cyc_Absyn_UnionType_struct*) _temp92)->f2;
goto _LL211; _LL211: _temp210=(( struct Cyc_Absyn_UnionType_struct*) _temp92)->f3;
goto _LL121;} else{ goto _LL122;} _LL122: if(( unsigned int) _temp92 > 4u?*((
int*) _temp92) == Cyc_Absyn_AnonStructType: 0){ _LL217: _temp216=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp92)->f1; goto _LL123;} else{ goto _LL124;} _LL124: if(( unsigned int)
_temp92 > 4u?*(( int*) _temp92) == Cyc_Absyn_AnonUnionType: 0){ _LL219: _temp218=((
struct Cyc_Absyn_AnonUnionType_struct*) _temp92)->f1; goto _LL125;} else{ goto
_LL126;} _LL126: if(( unsigned int) _temp92 > 4u?*(( int*) _temp92) == Cyc_Absyn_EnumType:
0){ _LL223: _temp222=(( struct Cyc_Absyn_EnumType_struct*) _temp92)->f1; goto
_LL221; _LL221: _temp220=(( struct Cyc_Absyn_EnumType_struct*) _temp92)->f2;
goto _LL127;} else{ goto _LL128;} _LL128: if(( unsigned int) _temp92 > 4u?*((
int*) _temp92) == Cyc_Absyn_RgnHandleType: 0){ _LL225: _temp224=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp92)->f1; goto _LL129;} else{ goto
_LL130;} _LL130: if(( unsigned int) _temp92 > 4u?*(( int*) _temp92) == Cyc_Absyn_TypedefType:
0){ _LL231: _temp230=(( struct Cyc_Absyn_TypedefType_struct*) _temp92)->f1; goto
_LL229; _LL229: _temp228=(( struct Cyc_Absyn_TypedefType_struct*) _temp92)->f2;
goto _LL227; _LL227: _temp226=(( struct Cyc_Absyn_TypedefType_struct*) _temp92)->f3;
goto _LL131;} else{ goto _LL132;} _LL132: if( _temp92 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL133;} else{ goto _LL134;} _LL134: if(( unsigned int) _temp92 > 4u?*((
int*) _temp92) == Cyc_Absyn_AccessEff: 0){ _LL233: _temp232=( void*)(( struct
Cyc_Absyn_AccessEff_struct*) _temp92)->f1; goto _LL135;} else{ goto _LL136;}
_LL136: if(( unsigned int) _temp92 > 4u?*(( int*) _temp92) == Cyc_Absyn_JoinEff:
0){ _LL235: _temp234=(( struct Cyc_Absyn_JoinEff_struct*) _temp92)->f1; goto
_LL137;} else{ goto _LL93;} _LL95: return t; _LL97: return t; _LL99: return(
void*)({ struct Cyc_Absyn_VarType_struct* _temp236=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp236[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp237; _temp237.tag= Cyc_Absyn_VarType; _temp237.f1= Cyc_Tcutil_copy_tvar(
_temp146); _temp237;}); _temp236;}); _LL101: return( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp238=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp238[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp239; _temp239.tag= Cyc_Absyn_TunionType;
_temp239.f1=({ struct Cyc_Absyn_TunionInfo _temp240; _temp240.tunion_info=( void*)
_temp154; _temp240.targs= Cyc_Tcutil_copy_types( _temp152); _temp240.rgn=( void*)
Cyc_Tcutil_copy_type( _temp150); _temp240;}); _temp239;}); _temp238;}); _LL103:
return( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp241=( struct Cyc_Absyn_TunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp241[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp242; _temp242.tag= Cyc_Absyn_TunionFieldType;
_temp242.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp243; _temp243.field_info=(
void*) _temp160; _temp243.targs= Cyc_Tcutil_copy_types( _temp158); _temp243;});
_temp242;}); _temp241;}); _LL105: { void* _temp244= Cyc_Tcutil_copy_type(
_temp172); goto _LL245; _LL245: { void* _temp246= Cyc_Tcutil_copy_type( _temp170);
goto _LL247; _LL247: { struct Cyc_Absyn_Conref* _temp248=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* c)) Cyc_Tcutil_copy_conref)( _temp168); goto _LL249;
_LL249: { struct Cyc_Absyn_Tqual _temp250= _temp166; goto _LL251; _LL251: {
struct Cyc_Absyn_Conref* _temp252=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
c)) Cyc_Tcutil_copy_conref)( _temp164); goto _LL253; _LL253: return( void*)({
struct Cyc_Absyn_PointerType_struct* _temp254=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp254[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp255; _temp255.tag= Cyc_Absyn_PointerType;
_temp255.f1=({ struct Cyc_Absyn_PtrInfo _temp256; _temp256.elt_typ=( void*)
_temp244; _temp256.rgn_typ=( void*) _temp246; _temp256.nullable= _temp248;
_temp256.tq= _temp250; _temp256.bounds= _temp252; _temp256;}); _temp255;});
_temp254;});}}}}} _LL107: return( void*)({ struct Cyc_Absyn_IntType_struct*
_temp257=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp257[ 0]=({ struct Cyc_Absyn_IntType_struct _temp258; _temp258.tag= Cyc_Absyn_IntType;
_temp258.f1=( void*) _temp176; _temp258.f2=( void*) _temp174; _temp258;});
_temp257;}); _LL109: goto _LL111; _LL111: return t; _LL113: return( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp259=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp259[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp260; _temp260.tag= Cyc_Absyn_ArrayType; _temp260.f1=(
void*) Cyc_Tcutil_copy_type( _temp182); _temp260.f2= _temp180; _temp260.f3=
_temp178; _temp260;}); _temp259;}); _LL115: { struct Cyc_List_List* _temp261=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_tvar, _temp200); goto
_LL262; _LL262: { struct Cyc_Core_Opt* _temp264= _temp198 == 0? 0:({ struct Cyc_Core_Opt*
_temp263=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp263->v=( void*) Cyc_Tcutil_copy_type(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp198))->v); _temp263;}); goto _LL265; _LL265: { void* _temp266=
Cyc_Tcutil_copy_type( _temp196); goto _LL267; _LL267: { struct Cyc_List_List*
_temp268=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple2*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_arg, _temp194); goto
_LL269; _LL269: { int _temp270= _temp192; goto _LL271; _LL271: { struct Cyc_Absyn_VarargInfo*
cyc_varargs2= 0; if( _temp190 != 0){ struct Cyc_Absyn_VarargInfo* cv=( struct
Cyc_Absyn_VarargInfo*) _check_null( _temp190); cyc_varargs2=({ struct Cyc_Absyn_VarargInfo*
_temp272=( struct Cyc_Absyn_VarargInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp272->name= cv->name; _temp272->tq= cv->tq; _temp272->type=( void*) Cyc_Tcutil_copy_type((
void*) cv->type); _temp272->rgn=( void*) Cyc_Tcutil_copy_type(( void*) cv->rgn);
_temp272->inject= cv->inject; _temp272;});}{ struct Cyc_List_List* _temp273=((
struct Cyc_List_List*(*)( struct _tuple8*(* f)( struct _tuple8*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_copy_rgncmp, _temp188); goto _LL274; _LL274: {
struct Cyc_List_List* _temp275= _temp186; goto _LL276; _LL276: return( void*)({
struct Cyc_Absyn_FnType_struct* _temp277=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp277[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp278; _temp278.tag= Cyc_Absyn_FnType; _temp278.f1=({ struct Cyc_Absyn_FnInfo
_temp279; _temp279.tvars= _temp261; _temp279.effect= _temp264; _temp279.ret_typ=(
void*) _temp266; _temp279.args= _temp268; _temp279.c_varargs= _temp270; _temp279.cyc_varargs=
cyc_varargs2; _temp279.rgn_po= _temp273; _temp279.attributes= _temp275; _temp279;});
_temp278;}); _temp277;});}}}}}}}} _LL117: return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp280=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp280[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp281; _temp281.tag= Cyc_Absyn_TupleType;
_temp281.f1=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_tqt, _temp202);
_temp281;}); _temp280;}); _LL119: return( void*)({ struct Cyc_Absyn_StructType_struct*
_temp282=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp282[ 0]=({ struct Cyc_Absyn_StructType_struct _temp283; _temp283.tag= Cyc_Absyn_StructType;
_temp283.f1= _temp208; _temp283.f2= Cyc_Tcutil_copy_types( _temp206); _temp283.f3=
0; _temp283;}); _temp282;}); _LL121: return( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp284=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp284[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp285; _temp285.tag= Cyc_Absyn_UnionType;
_temp285.f1= _temp214; _temp285.f2= Cyc_Tcutil_copy_types( _temp212); _temp285.f3=
0; _temp285;}); _temp284;}); _LL123: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp286=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp286[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp287; _temp287.tag= Cyc_Absyn_AnonStructType; _temp287.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_copy_field, _temp216); _temp287;}); _temp286;});
_LL125: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp288=(
struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp288[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp289; _temp289.tag=
Cyc_Absyn_AnonUnionType; _temp289.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_field,
_temp218); _temp289;}); _temp288;}); _LL127: return( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp290=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp290[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp291; _temp291.tag= Cyc_Absyn_EnumType;
_temp291.f1= _temp222; _temp291.f2= 0; _temp291;}); _temp290;}); _LL129: return(
void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp292=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp292[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp293; _temp293.tag= Cyc_Absyn_RgnHandleType;
_temp293.f1=( void*) Cyc_Tcutil_copy_type( _temp224); _temp293;}); _temp292;});
_LL131: return( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp294=( struct
Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp294[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp295; _temp295.tag= Cyc_Absyn_TypedefType;
_temp295.f1= _temp230; _temp295.f2= Cyc_Tcutil_copy_types( _temp228); _temp295.f3=
0; _temp295;}); _temp294;}); _LL133: return t; _LL135: return( void*)({ struct
Cyc_Absyn_AccessEff_struct* _temp296=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp296[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp297; _temp297.tag= Cyc_Absyn_AccessEff; _temp297.f1=(
void*) Cyc_Tcutil_copy_type( _temp232); _temp297;}); _temp296;}); _LL137: return(
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp298=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp298[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp299; _temp299.tag= Cyc_Absyn_JoinEff; _temp299.f1= Cyc_Tcutil_copy_types(
_temp234); _temp299;}); _temp298;}); _LL93:;} static int Cyc_Tcutil_kind_leq(
void* k1, void* k2){ if( k1 == k2){ return 1;}{ struct _tuple8 _temp301=({
struct _tuple8 _temp300; _temp300.f1= k1; _temp300.f2= k2; _temp300;}); void*
_temp311; void* _temp313; void* _temp315; void* _temp317; void* _temp319; void*
_temp321; _LL303: _LL314: _temp313= _temp301.f1; if( _temp313 ==( void*) Cyc_Absyn_BoxKind){
goto _LL312;} else{ goto _LL305;} _LL312: _temp311= _temp301.f2; if( _temp311 ==(
void*) Cyc_Absyn_MemKind){ goto _LL304;} else{ goto _LL305;} _LL305: _LL318:
_temp317= _temp301.f1; if( _temp317 ==( void*) Cyc_Absyn_BoxKind){ goto _LL316;}
else{ goto _LL307;} _LL316: _temp315= _temp301.f2; if( _temp315 ==( void*) Cyc_Absyn_AnyKind){
goto _LL306;} else{ goto _LL307;} _LL307: _LL322: _temp321= _temp301.f1; if(
_temp321 ==( void*) Cyc_Absyn_MemKind){ goto _LL320;} else{ goto _LL309;} _LL320:
_temp319= _temp301.f2; if( _temp319 ==( void*) Cyc_Absyn_AnyKind){ goto _LL308;}
else{ goto _LL309;} _LL309: goto _LL310; _LL304: return 1; _LL306: return 1;
_LL308: return 1; _LL310: return 0; _LL302:;}} void* Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar* tv){ return(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
tv->kind);} void* Cyc_Tcutil_typ_kind( void* t){ void* _temp323= Cyc_Tcutil_compress(
t); struct Cyc_Core_Opt* _temp375; int _temp377; struct Cyc_Core_Opt* _temp379;
struct Cyc_Core_Opt* _temp381; struct Cyc_Absyn_Tvar* _temp383; void* _temp385;
void* _temp387; struct Cyc_Absyn_FnInfo _temp389; void* _temp391; struct Cyc_Absyn_TunionInfo
_temp393; struct Cyc_Absyn_TunionFieldInfo _temp395; struct Cyc_List_List*
_temp397; void* _temp399; struct Cyc_Absyn_Tunionfield* _temp401; struct Cyc_Absyn_Tuniondecl*
_temp403; struct Cyc_Absyn_TunionFieldInfo _temp405; struct Cyc_List_List*
_temp407; void* _temp409; struct Cyc_Absyn_UnknownTunionFieldInfo _temp411;
struct Cyc_Absyn_Structdecl** _temp413; struct Cyc_List_List* _temp415; struct
_tuple1* _temp417; struct Cyc_Absyn_Uniondecl** _temp419; struct Cyc_List_List*
_temp421; struct _tuple1* _temp423; struct Cyc_Absyn_Structdecl** _temp425;
struct Cyc_List_List* _temp427; struct _tuple1* _temp429; struct Cyc_Absyn_Uniondecl**
_temp431; struct Cyc_List_List* _temp433; struct _tuple1* _temp435; struct Cyc_List_List*
_temp437; struct Cyc_List_List* _temp439; struct Cyc_Absyn_Enumdecl* _temp441;
struct _tuple1* _temp443; struct Cyc_Absyn_PtrInfo _temp445; struct Cyc_Absyn_Exp*
_temp447; struct Cyc_Absyn_Tqual _temp449; void* _temp451; struct Cyc_List_List*
_temp453; struct Cyc_Core_Opt* _temp455; struct Cyc_List_List* _temp457; struct
_tuple1* _temp459; void* _temp461; struct Cyc_List_List* _temp463; _LL325: if((
unsigned int) _temp323 > 4u?*(( int*) _temp323) == Cyc_Absyn_Evar: 0){ _LL382:
_temp381=(( struct Cyc_Absyn_Evar_struct*) _temp323)->f1; goto _LL380; _LL380:
_temp379=(( struct Cyc_Absyn_Evar_struct*) _temp323)->f2; goto _LL378; _LL378:
_temp377=(( struct Cyc_Absyn_Evar_struct*) _temp323)->f3; goto _LL376; _LL376:
_temp375=(( struct Cyc_Absyn_Evar_struct*) _temp323)->f4; goto _LL326;} else{
goto _LL327;} _LL327: if(( unsigned int) _temp323 > 4u?*(( int*) _temp323) ==
Cyc_Absyn_VarType: 0){ _LL384: _temp383=(( struct Cyc_Absyn_VarType_struct*)
_temp323)->f1; goto _LL328;} else{ goto _LL329;} _LL329: if( _temp323 ==( void*)
Cyc_Absyn_VoidType){ goto _LL330;} else{ goto _LL331;} _LL331: if(( unsigned int)
_temp323 > 4u?*(( int*) _temp323) == Cyc_Absyn_IntType: 0){ _LL388: _temp387=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp323)->f1; goto _LL386; _LL386:
_temp385=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp323)->f2; goto _LL332;}
else{ goto _LL333;} _LL333: if( _temp323 ==( void*) Cyc_Absyn_FloatType){ goto
_LL334;} else{ goto _LL335;} _LL335: if( _temp323 ==( void*) Cyc_Absyn_DoubleType){
goto _LL336;} else{ goto _LL337;} _LL337: if(( unsigned int) _temp323 > 4u?*((
int*) _temp323) == Cyc_Absyn_FnType: 0){ _LL390: _temp389=(( struct Cyc_Absyn_FnType_struct*)
_temp323)->f1; goto _LL338;} else{ goto _LL339;} _LL339: if(( unsigned int)
_temp323 > 4u?*(( int*) _temp323) == Cyc_Absyn_RgnHandleType: 0){ _LL392:
_temp391=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp323)->f1; goto
_LL340;} else{ goto _LL341;} _LL341: if( _temp323 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL342;} else{ goto _LL343;} _LL343: if(( unsigned int) _temp323 > 4u?*((
int*) _temp323) == Cyc_Absyn_TunionType: 0){ _LL394: _temp393=(( struct Cyc_Absyn_TunionType_struct*)
_temp323)->f1; goto _LL344;} else{ goto _LL345;} _LL345: if(( unsigned int)
_temp323 > 4u?*(( int*) _temp323) == Cyc_Absyn_TunionFieldType: 0){ _LL396:
_temp395=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp323)->f1; _LL400:
_temp399=( void*) _temp395.field_info; if(*(( int*) _temp399) == Cyc_Absyn_KnownTunionfield){
_LL404: _temp403=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp399)->f1;
goto _LL402; _LL402: _temp401=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp399)->f2; goto _LL398;} else{ goto _LL347;} _LL398: _temp397= _temp395.targs;
goto _LL346;} else{ goto _LL347;} _LL347: if(( unsigned int) _temp323 > 4u?*((
int*) _temp323) == Cyc_Absyn_TunionFieldType: 0){ _LL406: _temp405=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp323)->f1; _LL410: _temp409=( void*) _temp405.field_info; if(*(( int*)
_temp409) == Cyc_Absyn_UnknownTunionfield){ _LL412: _temp411=(( struct Cyc_Absyn_UnknownTunionfield_struct*)
_temp409)->f1; goto _LL408;} else{ goto _LL349;} _LL408: _temp407= _temp405.targs;
goto _LL348;} else{ goto _LL349;} _LL349: if(( unsigned int) _temp323 > 4u?*((
int*) _temp323) == Cyc_Absyn_StructType: 0){ _LL418: _temp417=(( struct Cyc_Absyn_StructType_struct*)
_temp323)->f1; goto _LL416; _LL416: _temp415=(( struct Cyc_Absyn_StructType_struct*)
_temp323)->f2; goto _LL414; _LL414: _temp413=(( struct Cyc_Absyn_StructType_struct*)
_temp323)->f3; if( _temp413 == 0){ goto _LL350;} else{ goto _LL351;}} else{ goto
_LL351;} _LL351: if(( unsigned int) _temp323 > 4u?*(( int*) _temp323) == Cyc_Absyn_UnionType:
0){ _LL424: _temp423=(( struct Cyc_Absyn_UnionType_struct*) _temp323)->f1; goto
_LL422; _LL422: _temp421=(( struct Cyc_Absyn_UnionType_struct*) _temp323)->f2;
goto _LL420; _LL420: _temp419=(( struct Cyc_Absyn_UnionType_struct*) _temp323)->f3;
if( _temp419 == 0){ goto _LL352;} else{ goto _LL353;}} else{ goto _LL353;}
_LL353: if(( unsigned int) _temp323 > 4u?*(( int*) _temp323) == Cyc_Absyn_StructType:
0){ _LL430: _temp429=(( struct Cyc_Absyn_StructType_struct*) _temp323)->f1; goto
_LL428; _LL428: _temp427=(( struct Cyc_Absyn_StructType_struct*) _temp323)->f2;
goto _LL426; _LL426: _temp425=(( struct Cyc_Absyn_StructType_struct*) _temp323)->f3;
goto _LL354;} else{ goto _LL355;} _LL355: if(( unsigned int) _temp323 > 4u?*((
int*) _temp323) == Cyc_Absyn_UnionType: 0){ _LL436: _temp435=(( struct Cyc_Absyn_UnionType_struct*)
_temp323)->f1; goto _LL434; _LL434: _temp433=(( struct Cyc_Absyn_UnionType_struct*)
_temp323)->f2; goto _LL432; _LL432: _temp431=(( struct Cyc_Absyn_UnionType_struct*)
_temp323)->f3; goto _LL356;} else{ goto _LL357;} _LL357: if(( unsigned int)
_temp323 > 4u?*(( int*) _temp323) == Cyc_Absyn_AnonStructType: 0){ _LL438:
_temp437=(( struct Cyc_Absyn_AnonStructType_struct*) _temp323)->f1; goto _LL358;}
else{ goto _LL359;} _LL359: if(( unsigned int) _temp323 > 4u?*(( int*) _temp323)
== Cyc_Absyn_AnonUnionType: 0){ _LL440: _temp439=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp323)->f1; goto _LL360;} else{ goto _LL361;} _LL361: if(( unsigned int)
_temp323 > 4u?*(( int*) _temp323) == Cyc_Absyn_EnumType: 0){ _LL444: _temp443=((
struct Cyc_Absyn_EnumType_struct*) _temp323)->f1; goto _LL442; _LL442: _temp441=((
struct Cyc_Absyn_EnumType_struct*) _temp323)->f2; goto _LL362;} else{ goto
_LL363;} _LL363: if(( unsigned int) _temp323 > 4u?*(( int*) _temp323) == Cyc_Absyn_PointerType:
0){ _LL446: _temp445=(( struct Cyc_Absyn_PointerType_struct*) _temp323)->f1;
goto _LL364;} else{ goto _LL365;} _LL365: if(( unsigned int) _temp323 > 4u?*((
int*) _temp323) == Cyc_Absyn_ArrayType: 0){ _LL452: _temp451=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp323)->f1; goto _LL450; _LL450: _temp449=((
struct Cyc_Absyn_ArrayType_struct*) _temp323)->f2; goto _LL448; _LL448: _temp447=((
struct Cyc_Absyn_ArrayType_struct*) _temp323)->f3; goto _LL366;} else{ goto
_LL367;} _LL367: if(( unsigned int) _temp323 > 4u?*(( int*) _temp323) == Cyc_Absyn_TupleType:
0){ _LL454: _temp453=(( struct Cyc_Absyn_TupleType_struct*) _temp323)->f1; goto
_LL368;} else{ goto _LL369;} _LL369: if(( unsigned int) _temp323 > 4u?*(( int*)
_temp323) == Cyc_Absyn_TypedefType: 0){ _LL460: _temp459=(( struct Cyc_Absyn_TypedefType_struct*)
_temp323)->f1; goto _LL458; _LL458: _temp457=(( struct Cyc_Absyn_TypedefType_struct*)
_temp323)->f2; goto _LL456; _LL456: _temp455=(( struct Cyc_Absyn_TypedefType_struct*)
_temp323)->f3; goto _LL370;} else{ goto _LL371;} _LL371: if(( unsigned int)
_temp323 > 4u?*(( int*) _temp323) == Cyc_Absyn_AccessEff: 0){ _LL462: _temp461=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp323)->f1; goto _LL372;} else{
goto _LL373;} _LL373: if(( unsigned int) _temp323 > 4u?*(( int*) _temp323) ==
Cyc_Absyn_JoinEff: 0){ _LL464: _temp463=(( struct Cyc_Absyn_JoinEff_struct*)
_temp323)->f1; goto _LL374;} else{ goto _LL324;} _LL326: return( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp381))->v; _LL328: return(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp383->kind); _LL330: return( void*) Cyc_Absyn_MemKind;
_LL332: return _temp385 ==( void*) Cyc_Absyn_B4?( void*) Cyc_Absyn_BoxKind:(
void*) Cyc_Absyn_MemKind; _LL334: return( void*) Cyc_Absyn_MemKind; _LL336:
return( void*) Cyc_Absyn_MemKind; _LL338: return( void*) Cyc_Absyn_MemKind;
_LL340: return( void*) Cyc_Absyn_BoxKind; _LL342: return( void*) Cyc_Absyn_RgnKind;
_LL344: return( void*) Cyc_Absyn_BoxKind; _LL346: if( _temp401->typs == 0){
return( void*) Cyc_Absyn_BoxKind;} else{ return( void*) Cyc_Absyn_MemKind;}
_LL348: return(( void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
unsigned char* _temp465=( unsigned char*)"typ_kind: Unresolved TunionFieldType";
struct _tagged_string _temp466; _temp466.curr= _temp465; _temp466.base= _temp465;
_temp466.last_plus_one= _temp465 + 37; _temp466;})); _LL350: return( void*) Cyc_Absyn_AnyKind;
_LL352: return( void*) Cyc_Absyn_AnyKind; _LL354: return( void*) Cyc_Absyn_MemKind;
_LL356: return( void*) Cyc_Absyn_MemKind; _LL358: return( void*) Cyc_Absyn_MemKind;
_LL360: return( void*) Cyc_Absyn_MemKind; _LL362: return( void*) Cyc_Absyn_BoxKind;
_LL364: { void* _temp467=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp445.bounds))->v; void* _temp477; void*
_temp479; struct Cyc_Absyn_Exp* _temp481; struct Cyc_Absyn_Conref* _temp483;
_LL469: if(( unsigned int) _temp467 > 1u?*(( int*) _temp467) == Cyc_Absyn_Eq_constr:
0){ _LL478: _temp477=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp467)->f1;
if( _temp477 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL470;} else{ goto _LL471;}}
else{ goto _LL471;} _LL471: if(( unsigned int) _temp467 > 1u?*(( int*) _temp467)
== Cyc_Absyn_Eq_constr: 0){ _LL480: _temp479=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp467)->f1; if(( unsigned int) _temp479 > 1u?*(( int*) _temp479) == Cyc_Absyn_Upper_b:
0){ _LL482: _temp481=(( struct Cyc_Absyn_Upper_b_struct*) _temp479)->f1; goto
_LL472;} else{ goto _LL473;}} else{ goto _LL473;} _LL473: if( _temp467 ==( void*)
Cyc_Absyn_No_constr){ goto _LL474;} else{ goto _LL475;} _LL475: if((
unsigned int) _temp467 > 1u?*(( int*) _temp467) == Cyc_Absyn_Forward_constr: 0){
_LL484: _temp483=(( struct Cyc_Absyn_Forward_constr_struct*) _temp467)->f1; goto
_LL476;} else{ goto _LL468;} _LL470: return( void*) Cyc_Absyn_MemKind; _LL472:
return( void*) Cyc_Absyn_BoxKind; _LL474: return( void*) Cyc_Absyn_MemKind;
_LL476: return(( void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
unsigned char* _temp485=( unsigned char*)"typ_kind: forward constraint in ptr bounds";
struct _tagged_string _temp486; _temp486.curr= _temp485; _temp486.base= _temp485;
_temp486.last_plus_one= _temp485 + 43; _temp486;})); _LL468:;} _LL366: return(
void*) Cyc_Absyn_MemKind; _LL368: return( void*) Cyc_Absyn_MemKind; _LL370:
return(( void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ struct _tagged_string _temp487= Cyc_Absynpp_typ2string( t);
xprintf("typ_kind: typedef found: %.*s", _temp487.last_plus_one - _temp487.curr,
_temp487.curr);})); _LL372: return( void*) Cyc_Absyn_EffKind; _LL374: return(
void*) Cyc_Absyn_EffKind; _LL324:;} unsigned char Cyc_Tcutil_Unify[ 10u]="\000\000\000\000Unify";
int Cyc_Tcutil_unify( void* t1, void* t2){ struct _handler_cons _temp488;
_push_handler(& _temp488);{ int _temp490= 0; if( setjmp( _temp488.handler)){
_temp490= 1;} if( ! _temp490){ Cyc_Tcutil_unify_it( t1, t2);{ int _temp491= 1;
_npop_handler( 0u); return _temp491;}; _pop_handler();} else{ void* _temp489=(
void*) _exn_thrown; void* _temp493= _temp489; _LL495: if( _temp493 == Cyc_Tcutil_Unify){
goto _LL496;} else{ goto _LL497;} _LL497: goto _LL498; _LL496: return 0; _LL498:(
void) _throw( _temp493); _LL494:;}}} static void Cyc_Tcutil_occurslist( void*
evar, struct Cyc_List_List* env, struct Cyc_List_List* ts); static void Cyc_Tcutil_occurs(
void* evar, struct Cyc_List_List* env, void* t){ void* _temp499= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Tvar* _temp533; struct Cyc_Core_Opt* _temp535; struct Cyc_Core_Opt**
_temp537; int _temp538; struct Cyc_Core_Opt* _temp540; struct Cyc_Core_Opt*
_temp542; struct Cyc_Absyn_PtrInfo _temp544; struct Cyc_Absyn_Exp* _temp546;
struct Cyc_Absyn_Tqual _temp548; void* _temp550; struct Cyc_Absyn_FnInfo
_temp552; struct Cyc_List_List* _temp554; struct Cyc_List_List* _temp556; struct
Cyc_Absyn_VarargInfo* _temp558; int _temp560; struct Cyc_List_List* _temp562;
void* _temp564; struct Cyc_Core_Opt* _temp566; struct Cyc_List_List* _temp568;
struct Cyc_List_List* _temp570; void* _temp572; struct Cyc_Absyn_TunionInfo
_temp574; void* _temp576; struct Cyc_List_List* _temp578; void* _temp580; struct
Cyc_Core_Opt* _temp582; struct Cyc_List_List* _temp584; struct _tuple1* _temp586;
struct Cyc_Absyn_TunionFieldInfo _temp588; struct Cyc_List_List* _temp590; void*
_temp592; struct Cyc_Absyn_Structdecl** _temp594; struct Cyc_List_List* _temp596;
struct _tuple1* _temp598; struct Cyc_List_List* _temp600; struct Cyc_List_List*
_temp602; void* _temp604; struct Cyc_List_List* _temp606; _LL501: if((
unsigned int) _temp499 > 4u?*(( int*) _temp499) == Cyc_Absyn_VarType: 0){ _LL534:
_temp533=(( struct Cyc_Absyn_VarType_struct*) _temp499)->f1; goto _LL502;} else{
goto _LL503;} _LL503: if(( unsigned int) _temp499 > 4u?*(( int*) _temp499) ==
Cyc_Absyn_Evar: 0){ _LL543: _temp542=(( struct Cyc_Absyn_Evar_struct*) _temp499)->f1;
goto _LL541; _LL541: _temp540=(( struct Cyc_Absyn_Evar_struct*) _temp499)->f2;
goto _LL539; _LL539: _temp538=(( struct Cyc_Absyn_Evar_struct*) _temp499)->f3;
goto _LL536; _LL536: _temp535=(( struct Cyc_Absyn_Evar_struct*) _temp499)->f4;
_temp537=&(( struct Cyc_Absyn_Evar_struct*) _temp499)->f4; goto _LL504;} else{
goto _LL505;} _LL505: if(( unsigned int) _temp499 > 4u?*(( int*) _temp499) ==
Cyc_Absyn_PointerType: 0){ _LL545: _temp544=(( struct Cyc_Absyn_PointerType_struct*)
_temp499)->f1; goto _LL506;} else{ goto _LL507;} _LL507: if(( unsigned int)
_temp499 > 4u?*(( int*) _temp499) == Cyc_Absyn_ArrayType: 0){ _LL551: _temp550=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp499)->f1; goto _LL549; _LL549:
_temp548=(( struct Cyc_Absyn_ArrayType_struct*) _temp499)->f2; goto _LL547;
_LL547: _temp546=(( struct Cyc_Absyn_ArrayType_struct*) _temp499)->f3; goto
_LL508;} else{ goto _LL509;} _LL509: if(( unsigned int) _temp499 > 4u?*(( int*)
_temp499) == Cyc_Absyn_FnType: 0){ _LL553: _temp552=(( struct Cyc_Absyn_FnType_struct*)
_temp499)->f1; _LL569: _temp568= _temp552.tvars; goto _LL567; _LL567: _temp566=
_temp552.effect; goto _LL565; _LL565: _temp564=( void*) _temp552.ret_typ; goto
_LL563; _LL563: _temp562= _temp552.args; goto _LL561; _LL561: _temp560= _temp552.c_varargs;
goto _LL559; _LL559: _temp558= _temp552.cyc_varargs; goto _LL557; _LL557:
_temp556= _temp552.rgn_po; goto _LL555; _LL555: _temp554= _temp552.attributes;
goto _LL510;} else{ goto _LL511;} _LL511: if(( unsigned int) _temp499 > 4u?*((
int*) _temp499) == Cyc_Absyn_TupleType: 0){ _LL571: _temp570=(( struct Cyc_Absyn_TupleType_struct*)
_temp499)->f1; goto _LL512;} else{ goto _LL513;} _LL513: if(( unsigned int)
_temp499 > 4u?*(( int*) _temp499) == Cyc_Absyn_RgnHandleType: 0){ _LL573:
_temp572=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp499)->f1; goto
_LL514;} else{ goto _LL515;} _LL515: if(( unsigned int) _temp499 > 4u?*(( int*)
_temp499) == Cyc_Absyn_TunionType: 0){ _LL575: _temp574=(( struct Cyc_Absyn_TunionType_struct*)
_temp499)->f1; _LL581: _temp580=( void*) _temp574.tunion_info; goto _LL579;
_LL579: _temp578= _temp574.targs; goto _LL577; _LL577: _temp576=( void*)
_temp574.rgn; goto _LL516;} else{ goto _LL517;} _LL517: if(( unsigned int)
_temp499 > 4u?*(( int*) _temp499) == Cyc_Absyn_TypedefType: 0){ _LL587: _temp586=((
struct Cyc_Absyn_TypedefType_struct*) _temp499)->f1; goto _LL585; _LL585:
_temp584=(( struct Cyc_Absyn_TypedefType_struct*) _temp499)->f2; goto _LL583;
_LL583: _temp582=(( struct Cyc_Absyn_TypedefType_struct*) _temp499)->f3; goto
_LL518;} else{ goto _LL519;} _LL519: if(( unsigned int) _temp499 > 4u?*(( int*)
_temp499) == Cyc_Absyn_TunionFieldType: 0){ _LL589: _temp588=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp499)->f1; _LL593: _temp592=( void*) _temp588.field_info; goto _LL591;
_LL591: _temp590= _temp588.targs; goto _LL520;} else{ goto _LL521;} _LL521: if((
unsigned int) _temp499 > 4u?*(( int*) _temp499) == Cyc_Absyn_StructType: 0){
_LL599: _temp598=(( struct Cyc_Absyn_StructType_struct*) _temp499)->f1; goto
_LL597; _LL597: _temp596=(( struct Cyc_Absyn_StructType_struct*) _temp499)->f2;
goto _LL595; _LL595: _temp594=(( struct Cyc_Absyn_StructType_struct*) _temp499)->f3;
goto _LL522;} else{ goto _LL523;} _LL523: if(( unsigned int) _temp499 > 4u?*((
int*) _temp499) == Cyc_Absyn_AnonStructType: 0){ _LL601: _temp600=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp499)->f1; goto _LL524;} else{ goto _LL525;} _LL525: if(( unsigned int)
_temp499 > 4u?*(( int*) _temp499) == Cyc_Absyn_AnonUnionType: 0){ _LL603:
_temp602=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp499)->f1; goto _LL526;}
else{ goto _LL527;} _LL527: if(( unsigned int) _temp499 > 4u?*(( int*) _temp499)
== Cyc_Absyn_AccessEff: 0){ _LL605: _temp604=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp499)->f1; goto _LL528;} else{ goto _LL529;} _LL529: if(( unsigned int)
_temp499 > 4u?*(( int*) _temp499) == Cyc_Absyn_JoinEff: 0){ _LL607: _temp606=((
struct Cyc_Absyn_JoinEff_struct*) _temp499)->f1; goto _LL530;} else{ goto _LL531;}
_LL531: goto _LL532; _LL502: if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, env, _temp533)){( void) _throw(( void*) Cyc_Tcutil_Unify);}
goto _LL500; _LL504: if( t == evar){( void) _throw(( void*) Cyc_Tcutil_Unify);}
else{ if( _temp540 != 0){ Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp540))->v);} else{ int problem= 0;{ struct Cyc_List_List* s=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(* _temp537))->v; for(
0; s != 0; s=(( struct Cyc_List_List*) _check_null( s))->tl){ if( !(( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( s))->hd)){ problem=
1; break;}}} if( problem){ struct Cyc_List_List* _temp608= 0; goto _LL609;
_LL609:{ struct Cyc_List_List* s=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null(* _temp537))->v; for( 0; s != 0; s=(( struct Cyc_List_List*)
_check_null( s))->tl){ if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( s))->hd)){ _temp608=({ struct Cyc_List_List* _temp610=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp610->hd=( void*)(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( s))->hd); _temp610->tl= _temp608; _temp610;});}}}*
_temp537=({ struct Cyc_Core_Opt* _temp611=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp611->v=( void*) _temp608; _temp611;});}}}
goto _LL500; _LL506: Cyc_Tcutil_occurs( evar, env,( void*) _temp544.elt_typ);
Cyc_Tcutil_occurs( evar, env,( void*) _temp544.rgn_typ); goto _LL500; _LL508:
Cyc_Tcutil_occurs( evar, env, _temp550); goto _LL500; _LL510: if( _temp566 != 0){
Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp566))->v);} Cyc_Tcutil_occurs( evar, env, _temp564); for( 0; _temp562 != 0;
_temp562=(( struct Cyc_List_List*) _check_null( _temp562))->tl){ Cyc_Tcutil_occurs(
evar, env,(*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp562))->hd)).f3);}
if( _temp558 != 0){ int _temp614; void* _temp616; void* _temp618; struct Cyc_Absyn_Tqual
_temp620; struct Cyc_Core_Opt* _temp622; struct Cyc_Absyn_VarargInfo _temp612=*((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp558)); _LL623: _temp622=
_temp612.name; goto _LL621; _LL621: _temp620= _temp612.tq; goto _LL619; _LL619:
_temp618=( void*) _temp612.type; goto _LL617; _LL617: _temp616=( void*) _temp612.rgn;
goto _LL615; _LL615: _temp614= _temp612.inject; goto _LL613; _LL613: Cyc_Tcutil_occurs(
evar, env, _temp618); Cyc_Tcutil_occurs( evar, env, _temp616);} for( 0; _temp556
!= 0; _temp556=(( struct Cyc_List_List*) _check_null( _temp556))->tl){ struct
_tuple8 _temp626; void* _temp627; void* _temp629; struct _tuple8* _temp624=(
struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp556))->hd; _temp626=*
_temp624; _LL630: _temp629= _temp626.f1; goto _LL628; _LL628: _temp627= _temp626.f2;
goto _LL625; _LL625: Cyc_Tcutil_occurs( evar, env, _temp629); Cyc_Tcutil_occurs(
evar, env, _temp627);} goto _LL500; _LL512: for( 0; _temp570 != 0; _temp570=((
struct Cyc_List_List*) _check_null( _temp570))->tl){ Cyc_Tcutil_occurs( evar,
env,(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp570))->hd)).f2);}
goto _LL500; _LL514: Cyc_Tcutil_occurs( evar, env, _temp572); goto _LL500;
_LL516: Cyc_Tcutil_occurs( evar, env, _temp576); Cyc_Tcutil_occurslist( evar,
env, _temp578); goto _LL500; _LL518: _temp590= _temp584; goto _LL520; _LL520:
_temp596= _temp590; goto _LL522; _LL522: Cyc_Tcutil_occurslist( evar, env,
_temp596); goto _LL500; _LL524: _temp602= _temp600; goto _LL526; _LL526: for( 0;
_temp602 != 0; _temp602=(( struct Cyc_List_List*) _check_null( _temp602))->tl){
Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp602))->hd)->type);} goto _LL500; _LL528: Cyc_Tcutil_occurs(
evar, env, _temp604); goto _LL500; _LL530: Cyc_Tcutil_occurslist( evar, env,
_temp606); goto _LL500; _LL532: goto _LL500; _LL500:;} static void Cyc_Tcutil_occurslist(
void* evar, struct Cyc_List_List* env, struct Cyc_List_List* ts){ for( 0; ts !=
0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){ Cyc_Tcutil_occurs( evar,
env,( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);}} static void Cyc_Tcutil_unify_list(
struct Cyc_List_List* t1, struct Cyc_List_List* t2){ struct _handler_cons
_temp631; _push_handler(& _temp631);{ int _temp633= 0; if( setjmp( _temp631.handler)){
_temp633= 1;} if( ! _temp633){(( void(*)( void(* f)( void*, void*), struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_iter2)( Cyc_Tcutil_unify_it, t1, t2);;
_pop_handler();} else{ void* _temp632=( void*) _exn_thrown; void* _temp635=
_temp632; _LL637: if( _temp635 == Cyc_List_List_mismatch){ goto _LL638;} else{
goto _LL639;} _LL639: goto _LL640; _LL638:( void) _throw(( void*) Cyc_Tcutil_Unify);
_LL640:( void) _throw( _temp635); _LL636:;}}} static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2){ if(( tq1.q_const != tq2.q_const?
1: tq1.q_volatile != tq2.q_volatile)? 1: tq1.q_restrict != tq2.q_restrict){(
void) _throw(( void*) Cyc_Tcutil_Unify);}} int Cyc_Tcutil_equal_tqual( struct
Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2){ return( tq1.q_const == tq2.q_const?
tq1.q_volatile == tq2.q_volatile: 0)? tq1.q_restrict == tq2.q_restrict: 0;}
static void Cyc_Tcutil_unify_it_conrefs( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y){ x=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( x); y=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( y); if( x == y){ return;}{ void* _temp641=( void*)
x->v; struct Cyc_Absyn_Conref* _temp649; void* _temp651; _LL643: if( _temp641 ==(
void*) Cyc_Absyn_No_constr){ goto _LL644;} else{ goto _LL645;} _LL645: if((
unsigned int) _temp641 > 1u?*(( int*) _temp641) == Cyc_Absyn_Forward_constr: 0){
_LL650: _temp649=(( struct Cyc_Absyn_Forward_constr_struct*) _temp641)->f1; goto
_LL646;} else{ goto _LL647;} _LL647: if(( unsigned int) _temp641 > 1u?*(( int*)
_temp641) == Cyc_Absyn_Eq_constr: 0){ _LL652: _temp651=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp641)->f1; goto _LL648;} else{ goto _LL642;} _LL644:( void*)( x->v=( void*)((
void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp653=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp653[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp654; _temp654.tag= Cyc_Absyn_Forward_constr;
_temp654.f1= y; _temp654;}); _temp653;}))); return; _LL646:( void) _throw(((
void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char*
_temp655=( unsigned char*)"unify_conref: forward after compress"; struct
_tagged_string _temp656; _temp656.curr= _temp655; _temp656.base= _temp655;
_temp656.last_plus_one= _temp655 + 37; _temp656;}))); _LL648: { void* _temp657=(
void*) y->v; struct Cyc_Absyn_Conref* _temp665; void* _temp667; _LL659: if(
_temp657 ==( void*) Cyc_Absyn_No_constr){ goto _LL660;} else{ goto _LL661;}
_LL661: if(( unsigned int) _temp657 > 1u?*(( int*) _temp657) == Cyc_Absyn_Forward_constr:
0){ _LL666: _temp665=(( struct Cyc_Absyn_Forward_constr_struct*) _temp657)->f1;
goto _LL662;} else{ goto _LL663;} _LL663: if(( unsigned int) _temp657 > 1u?*((
int*) _temp657) == Cyc_Absyn_Eq_constr: 0){ _LL668: _temp667=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp657)->f1; goto _LL664;} else{ goto _LL658;}
_LL660:( void*)( y->v=( void*)(( void*) x->v)); return; _LL662:( void) _throw(((
void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char*
_temp669=( unsigned char*)"unify_conref: forward after compress(2)"; struct
_tagged_string _temp670; _temp670.curr= _temp669; _temp670.base= _temp669;
_temp670.last_plus_one= _temp669 + 40; _temp670;}))); _LL664: if( cmp( _temp651,
_temp667) != 0){( void) _throw(( void*) Cyc_Tcutil_Unify);} return; _LL658:;}
_LL642:;}} static int Cyc_Tcutil_unify_conrefs( int(* cmp)( void*, void*),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y){ struct _handler_cons
_temp671; _push_handler(& _temp671);{ int _temp673= 0; if( setjmp( _temp671.handler)){
_temp673= 1;} if( ! _temp673){(( void(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)( cmp, x, y);{ int
_temp674= 1; _npop_handler( 0u); return _temp674;}; _pop_handler();} else{ void*
_temp672=( void*) _exn_thrown; void* _temp676= _temp672; _LL678: if( _temp676 ==
Cyc_Tcutil_Unify){ goto _LL679;} else{ goto _LL680;} _LL680: goto _LL681; _LL679:
return 0; _LL681:( void) _throw( _temp676); _LL677:;}}} static int Cyc_Tcutil_boundscmp(
void* b1, void* b2){ struct _tuple8 _temp683=({ struct _tuple8 _temp682;
_temp682.f1= b1; _temp682.f2= b2; _temp682;}); void* _temp693; void* _temp695;
void* _temp697; void* _temp699; void* _temp701; void* _temp703; void* _temp705;
struct Cyc_Absyn_Exp* _temp707; void* _temp709; struct Cyc_Absyn_Exp* _temp711;
_LL685: _LL696: _temp695= _temp683.f1; if( _temp695 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL694;} else{ goto _LL687;} _LL694: _temp693= _temp683.f2; if( _temp693 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL686;} else{ goto _LL687;} _LL687: _LL700:
_temp699= _temp683.f1; if( _temp699 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL698;}
else{ goto _LL689;} _LL698: _temp697= _temp683.f2; goto _LL688; _LL689: _LL704:
_temp703= _temp683.f1; goto _LL702; _LL702: _temp701= _temp683.f2; if( _temp701
==( void*) Cyc_Absyn_Unknown_b){ goto _LL690;} else{ goto _LL691;} _LL691:
_LL710: _temp709= _temp683.f1; if(( unsigned int) _temp709 > 1u?*(( int*)
_temp709) == Cyc_Absyn_Upper_b: 0){ _LL712: _temp711=(( struct Cyc_Absyn_Upper_b_struct*)
_temp709)->f1; goto _LL706;} else{ goto _LL684;} _LL706: _temp705= _temp683.f2;
if(( unsigned int) _temp705 > 1u?*(( int*) _temp705) == Cyc_Absyn_Upper_b: 0){
_LL708: _temp707=(( struct Cyc_Absyn_Upper_b_struct*) _temp705)->f1; goto _LL692;}
else{ goto _LL684;} _LL686: return 0; _LL688: return - 1; _LL690: return 1;
_LL692: { int i1=( int) Cyc_Evexp_eval_const_uint_exp( _temp711); int i2=( int)
Cyc_Evexp_eval_const_uint_exp( _temp707); if( i1 == i2){ return 0;} if( i1 < i2){
return - 1;} return 1;} _LL684:;} static int Cyc_Tcutil_equal_att( void* a1,
void* a2){ if( a1 == a2){ return 1;}{ struct _tuple8 _temp714=({ struct _tuple8
_temp713; _temp713.f1= a1; _temp713.f2= a2; _temp713;}); void* _temp724; int
_temp726; void* _temp728; int _temp730; void* _temp732; int _temp734; void*
_temp736; int _temp738; void* _temp740; struct _tagged_string _temp742; void*
_temp744; struct _tagged_string _temp746; _LL716: _LL729: _temp728= _temp714.f1;
if(( unsigned int) _temp728 > 16u?*(( int*) _temp728) == Cyc_Absyn_Regparm_att:
0){ _LL731: _temp730=(( struct Cyc_Absyn_Regparm_att_struct*) _temp728)->f1;
goto _LL725;} else{ goto _LL718;} _LL725: _temp724= _temp714.f2; if((
unsigned int) _temp724 > 16u?*(( int*) _temp724) == Cyc_Absyn_Regparm_att: 0){
_LL727: _temp726=(( struct Cyc_Absyn_Regparm_att_struct*) _temp724)->f1; goto
_LL717;} else{ goto _LL718;} _LL718: _LL737: _temp736= _temp714.f1; if((
unsigned int) _temp736 > 16u?*(( int*) _temp736) == Cyc_Absyn_Aligned_att: 0){
_LL739: _temp738=(( struct Cyc_Absyn_Aligned_att_struct*) _temp736)->f1; goto
_LL733;} else{ goto _LL720;} _LL733: _temp732= _temp714.f2; if(( unsigned int)
_temp732 > 16u?*(( int*) _temp732) == Cyc_Absyn_Aligned_att: 0){ _LL735:
_temp734=(( struct Cyc_Absyn_Aligned_att_struct*) _temp732)->f1; goto _LL719;}
else{ goto _LL720;} _LL720: _LL745: _temp744= _temp714.f1; if(( unsigned int)
_temp744 > 16u?*(( int*) _temp744) == Cyc_Absyn_Section_att: 0){ _LL747:
_temp746=(( struct Cyc_Absyn_Section_att_struct*) _temp744)->f1; goto _LL741;}
else{ goto _LL722;} _LL741: _temp740= _temp714.f2; if(( unsigned int) _temp740 >
16u?*(( int*) _temp740) == Cyc_Absyn_Section_att: 0){ _LL743: _temp742=(( struct
Cyc_Absyn_Section_att_struct*) _temp740)->f1; goto _LL721;} else{ goto _LL722;}
_LL722: goto _LL723; _LL717: _temp738= _temp730; _temp734= _temp726; goto _LL719;
_LL719: return _temp738 == _temp734; _LL721: return Cyc_String_strcmp( _temp746,
_temp742) == 0; _LL723: return 0; _LL715:;}} int Cyc_Tcutil_same_atts( struct
Cyc_List_List* a1, struct Cyc_List_List* a2){{ struct Cyc_List_List* a= a1; for(
0; a != 0; a=(( struct Cyc_List_List*) _check_null( a))->tl){ if( !(( int(*)(
int(* pred)( void*, void*), void* env, struct Cyc_List_List* x)) Cyc_List_exists_c)(
Cyc_Tcutil_equal_att,( void*)(( struct Cyc_List_List*) _check_null( a))->hd, a2)){
return 0;}}}{ struct Cyc_List_List* a= a2; for( 0; a != 0; a=(( struct Cyc_List_List*)
_check_null( a))->tl){ if( !(( int(*)( int(* pred)( void*, void*), void* env,
struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcutil_equal_att,( void*)((
struct Cyc_List_List*) _check_null( a))->hd, a1)){ return 0;}}} return 1;}
static void Cyc_Tcutil_normalize_effect( void* e){ e= Cyc_Tcutil_compress( e);{
void* _temp748= e; struct Cyc_List_List* _temp754; struct Cyc_List_List**
_temp756; _LL750: if(( unsigned int) _temp748 > 4u?*(( int*) _temp748) == Cyc_Absyn_JoinEff:
0){ _LL755: _temp754=(( struct Cyc_Absyn_JoinEff_struct*) _temp748)->f1;
_temp756=&(( struct Cyc_Absyn_JoinEff_struct*) _temp748)->f1; goto _LL751;}
else{ goto _LL752;} _LL752: goto _LL753; _LL751: { int nested_join= 0;{ struct
Cyc_List_List* effs=* _temp756; for( 0; effs != 0; effs=(( struct Cyc_List_List*)
_check_null( effs))->tl){ void* _temp757=( void*)(( struct Cyc_List_List*)
_check_null( effs))->hd; goto _LL758; _LL758: Cyc_Tcutil_normalize_effect(
_temp757);{ void* _temp759= Cyc_Tcutil_compress( _temp757); struct Cyc_List_List*
_temp765; _LL761: if(( unsigned int) _temp759 > 4u?*(( int*) _temp759) == Cyc_Absyn_JoinEff:
0){ _LL766: _temp765=(( struct Cyc_Absyn_JoinEff_struct*) _temp759)->f1; goto
_LL762;} else{ goto _LL763;} _LL763: goto _LL764; _LL762: nested_join= 1; goto
_LL760; _LL764: goto _LL760; _LL760:;}}} if( ! nested_join){ return;}{ struct
Cyc_List_List* effects= 0;{ struct Cyc_List_List* effs=* _temp756; for( 0; effs
!= 0; effs=(( struct Cyc_List_List*) _check_null( effs))->tl){ void* _temp767=
Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*) _check_null( effs))->hd);
struct Cyc_List_List* _temp773; _LL769: if(( unsigned int) _temp767 > 4u?*(( int*)
_temp767) == Cyc_Absyn_JoinEff: 0){ _LL774: _temp773=(( struct Cyc_Absyn_JoinEff_struct*)
_temp767)->f1; goto _LL770;} else{ goto _LL771;} _LL771: goto _LL772; _LL770:
effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_revappend)( _temp773, effects); goto _LL768; _LL772: effects=({
struct Cyc_List_List* _temp775=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp775->hd=( void*) _temp767; _temp775->tl= effects;
_temp775;}); goto _LL768; _LL768:;}}* _temp756=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( effects); goto _LL749;}} _LL753:
goto _LL749; _LL749:;}} struct _tuple9{ void* f1; struct Cyc_List_List* f2;
struct Cyc_List_List* f3; } ; static struct _tuple9* Cyc_Tcutil_get_effect_evar(
void* t){ void* _temp776= Cyc_Tcutil_compress( t); struct Cyc_List_List*
_temp784; struct Cyc_List_List _temp786; struct Cyc_List_List* _temp787; void*
_temp789; struct Cyc_Core_Opt* _temp791; int _temp793; struct Cyc_Core_Opt*
_temp795; struct Cyc_Core_Opt* _temp797; _LL778: if(( unsigned int) _temp776 > 4u?*((
int*) _temp776) == Cyc_Absyn_JoinEff: 0){ _LL785: _temp784=(( struct Cyc_Absyn_JoinEff_struct*)
_temp776)->f1; if( _temp784 == 0){ goto _LL780;} else{ _temp786=* _temp784;
_LL790: _temp789=( void*) _temp786.hd; goto _LL788; _LL788: _temp787= _temp786.tl;
goto _LL779;}} else{ goto _LL780;} _LL780: if(( unsigned int) _temp776 > 4u?*((
int*) _temp776) == Cyc_Absyn_Evar: 0){ _LL798: _temp797=(( struct Cyc_Absyn_Evar_struct*)
_temp776)->f1; goto _LL796; _LL796: _temp795=(( struct Cyc_Absyn_Evar_struct*)
_temp776)->f2; goto _LL794; _LL794: _temp793=(( struct Cyc_Absyn_Evar_struct*)
_temp776)->f3; goto _LL792; _LL792: _temp791=(( struct Cyc_Absyn_Evar_struct*)
_temp776)->f4; goto _LL781;} else{ goto _LL782;} _LL782: goto _LL783; _LL779: {
void* _temp799= Cyc_Tcutil_compress( _temp789); struct Cyc_Core_Opt* _temp805;
int _temp807; struct Cyc_Core_Opt* _temp809; struct Cyc_Core_Opt* _temp811;
_LL801: if(( unsigned int) _temp799 > 4u?*(( int*) _temp799) == Cyc_Absyn_Evar:
0){ _LL812: _temp811=(( struct Cyc_Absyn_Evar_struct*) _temp799)->f1; goto
_LL810; _LL810: _temp809=(( struct Cyc_Absyn_Evar_struct*) _temp799)->f2; goto
_LL808; _LL808: _temp807=(( struct Cyc_Absyn_Evar_struct*) _temp799)->f3; goto
_LL806; _LL806: _temp805=(( struct Cyc_Absyn_Evar_struct*) _temp799)->f4; goto
_LL802;} else{ goto _LL803;} _LL803: goto _LL804; _LL802: return({ struct
_tuple9* _temp813=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp813->f1= _temp789; _temp813->f2= _temp787; _temp813->f3=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp805))->v; _temp813;}); _LL804: return 0;
_LL800:;} _LL781: if( _temp797 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp797))->v !=( void*) Cyc_Absyn_EffKind){(( void(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp814=(
unsigned char*)"effect evar has wrong kind"; struct _tagged_string _temp815;
_temp815.curr= _temp814; _temp815.base= _temp814; _temp815.last_plus_one=
_temp814 + 27; _temp815;}));} return({ struct _tuple9* _temp816=( struct _tuple9*)
GC_malloc( sizeof( struct _tuple9)); _temp816->f1= t; _temp816->f2= 0; _temp816->f3=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp791))->v;
_temp816;}); _LL783: return 0; _LL777:;} static struct Cyc_Core_Opt Cyc_Tcutil_ek={(
void*)(( void*) 4u)}; int Cyc_Tcutil_region_in_effect( int constrain, void* r,
void* e){ r= Cyc_Tcutil_compress( r); if( r ==( void*) Cyc_Absyn_HeapRgn){
return 1;}{ void* _temp817= Cyc_Tcutil_compress( e); void* _temp827; struct Cyc_List_List*
_temp829; struct Cyc_Core_Opt* _temp831; int _temp833; struct Cyc_Core_Opt*
_temp835; struct Cyc_Core_Opt** _temp837; struct Cyc_Core_Opt* _temp838; _LL819:
if(( unsigned int) _temp817 > 4u?*(( int*) _temp817) == Cyc_Absyn_AccessEff: 0){
_LL828: _temp827=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp817)->f1;
goto _LL820;} else{ goto _LL821;} _LL821: if(( unsigned int) _temp817 > 4u?*((
int*) _temp817) == Cyc_Absyn_JoinEff: 0){ _LL830: _temp829=(( struct Cyc_Absyn_JoinEff_struct*)
_temp817)->f1; goto _LL822;} else{ goto _LL823;} _LL823: if(( unsigned int)
_temp817 > 4u?*(( int*) _temp817) == Cyc_Absyn_Evar: 0){ _LL839: _temp838=((
struct Cyc_Absyn_Evar_struct*) _temp817)->f1; goto _LL836; _LL836: _temp835=((
struct Cyc_Absyn_Evar_struct*) _temp817)->f2; _temp837=&(( struct Cyc_Absyn_Evar_struct*)
_temp817)->f2; goto _LL834; _LL834: _temp833=(( struct Cyc_Absyn_Evar_struct*)
_temp817)->f3; goto _LL832; _LL832: _temp831=(( struct Cyc_Absyn_Evar_struct*)
_temp817)->f4; goto _LL824;} else{ goto _LL825;} _LL825: goto _LL826; _LL820:
if( constrain){ return Cyc_Tcutil_unify( r, _temp827);} Cyc_Tcutil_compress(
_temp827); if( r == _temp827){ return 1;}{ struct _tuple8 _temp841=({ struct
_tuple8 _temp840; _temp840.f1= r; _temp840.f2= _temp827; _temp840;}); void*
_temp847; struct Cyc_Absyn_Tvar* _temp849; void* _temp851; struct Cyc_Absyn_Tvar*
_temp853; _LL843: _LL852: _temp851= _temp841.f1; if(( unsigned int) _temp851 > 4u?*((
int*) _temp851) == Cyc_Absyn_VarType: 0){ _LL854: _temp853=(( struct Cyc_Absyn_VarType_struct*)
_temp851)->f1; goto _LL848;} else{ goto _LL845;} _LL848: _temp847= _temp841.f2;
if(( unsigned int) _temp847 > 4u?*(( int*) _temp847) == Cyc_Absyn_VarType: 0){
_LL850: _temp849=(( struct Cyc_Absyn_VarType_struct*) _temp847)->f1; goto _LL844;}
else{ goto _LL845;} _LL845: goto _LL846; _LL844: return Cyc_Absyn_tvar_cmp(
_temp853, _temp849) == 0; _LL846: return 0; _LL842:;} _LL822: for( 0; _temp829
!= 0; _temp829=(( struct Cyc_List_List*) _check_null( _temp829))->tl){ if( Cyc_Tcutil_region_in_effect(
constrain, r,( void*)(( struct Cyc_List_List*) _check_null( _temp829))->hd)){
return 1;}} return 0; _LL824: if( _temp838 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp838))->v !=( void*) Cyc_Absyn_EffKind){(( void(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp855=(
unsigned char*)"effect evar has wrong kind"; struct _tagged_string _temp856;
_temp856.curr= _temp855; _temp856.base= _temp855; _temp856.last_plus_one=
_temp855 + 27; _temp856;}));} if( ! constrain){ return 0;}{ void* _temp857= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek, _temp831); goto _LL858; _LL858: Cyc_Tcutil_occurs(
_temp857,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp831))->v,
r);{ struct Cyc_Absyn_JoinEff_struct* _temp865=({ struct Cyc_Absyn_JoinEff_struct*
_temp859=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp859[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp860; _temp860.tag= Cyc_Absyn_JoinEff;
_temp860.f1=({ struct Cyc_List_List* _temp861=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp861->hd=( void*) _temp857; _temp861->tl=({
struct Cyc_List_List* _temp862=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp862->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp863=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp863[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp864; _temp864.tag= Cyc_Absyn_AccessEff;
_temp864.f1=( void*) r; _temp864;}); _temp863;})); _temp862->tl= 0; _temp862;});
_temp861;}); _temp860;}); _temp859;}); goto _LL866; _LL866:* _temp837=({ struct
Cyc_Core_Opt* _temp867=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp867->v=( void*)(( void*) _temp865); _temp867;}); return 1;}} _LL826: return
0; _LL818:;}} static int Cyc_Tcutil_variable_in_effect( int constrain, struct
Cyc_Absyn_Tvar* v, void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp868= e;
struct Cyc_Absyn_Tvar* _temp878; struct Cyc_List_List* _temp880; struct Cyc_Core_Opt*
_temp882; int _temp884; struct Cyc_Core_Opt* _temp886; struct Cyc_Core_Opt**
_temp888; struct Cyc_Core_Opt* _temp889; _LL870: if(( unsigned int) _temp868 > 4u?*((
int*) _temp868) == Cyc_Absyn_VarType: 0){ _LL879: _temp878=(( struct Cyc_Absyn_VarType_struct*)
_temp868)->f1; goto _LL871;} else{ goto _LL872;} _LL872: if(( unsigned int)
_temp868 > 4u?*(( int*) _temp868) == Cyc_Absyn_JoinEff: 0){ _LL881: _temp880=((
struct Cyc_Absyn_JoinEff_struct*) _temp868)->f1; goto _LL873;} else{ goto _LL874;}
_LL874: if(( unsigned int) _temp868 > 4u?*(( int*) _temp868) == Cyc_Absyn_Evar:
0){ _LL890: _temp889=(( struct Cyc_Absyn_Evar_struct*) _temp868)->f1; goto
_LL887; _LL887: _temp886=(( struct Cyc_Absyn_Evar_struct*) _temp868)->f2;
_temp888=&(( struct Cyc_Absyn_Evar_struct*) _temp868)->f2; goto _LL885; _LL885:
_temp884=(( struct Cyc_Absyn_Evar_struct*) _temp868)->f3; goto _LL883; _LL883:
_temp882=(( struct Cyc_Absyn_Evar_struct*) _temp868)->f4; goto _LL875;} else{
goto _LL876;} _LL876: goto _LL877; _LL871: return Cyc_Absyn_tvar_cmp( v,
_temp878) == 0; _LL873: for( 0; _temp880 != 0; _temp880=(( struct Cyc_List_List*)
_check_null( _temp880))->tl){ if( Cyc_Tcutil_variable_in_effect( constrain, v,(
void*)(( struct Cyc_List_List*) _check_null( _temp880))->hd)){ return 1;}}
return 0; _LL875: if( _temp889 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp889))->v !=( void*) Cyc_Absyn_EffKind){(( void(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp891=(
unsigned char*)"effect evar has wrong kind"; struct _tagged_string _temp892;
_temp892.curr= _temp891; _temp892.base= _temp891; _temp892.last_plus_one=
_temp891 + 27; _temp892;}));}{ void* _temp893= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek, _temp882); goto _LL894; _LL894: if( !(( int(*)( int(* compare)(
struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct
Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp882))->v, v)){ return 0;}{ struct Cyc_Absyn_JoinEff_struct*
_temp901=({ struct Cyc_Absyn_JoinEff_struct* _temp895=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp895[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp896; _temp896.tag= Cyc_Absyn_JoinEff; _temp896.f1=({ struct Cyc_List_List*
_temp897=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp897->hd=( void*) _temp893; _temp897->tl=({ struct Cyc_List_List* _temp898=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp898->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp899=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp899[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp900; _temp900.tag= Cyc_Absyn_VarType; _temp900.f1= v; _temp900;}); _temp899;}));
_temp898->tl= 0; _temp898;}); _temp897;}); _temp896;}); _temp895;}); goto _LL902;
_LL902:* _temp888=({ struct Cyc_Core_Opt* _temp903=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp903->v=( void*)(( void*) _temp901);
_temp903;}); return 1;}} _LL877: return 0; _LL869:;}} static int Cyc_Tcutil_evar_in_effect(
void* evar, void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp904= e; struct Cyc_List_List*
_temp912; struct Cyc_Core_Opt* _temp914; int _temp916; struct Cyc_Core_Opt*
_temp918; struct Cyc_Core_Opt* _temp920; _LL906: if(( unsigned int) _temp904 > 4u?*((
int*) _temp904) == Cyc_Absyn_JoinEff: 0){ _LL913: _temp912=(( struct Cyc_Absyn_JoinEff_struct*)
_temp904)->f1; goto _LL907;} else{ goto _LL908;} _LL908: if(( unsigned int)
_temp904 > 4u?*(( int*) _temp904) == Cyc_Absyn_Evar: 0){ _LL921: _temp920=((
struct Cyc_Absyn_Evar_struct*) _temp904)->f1; goto _LL919; _LL919: _temp918=((
struct Cyc_Absyn_Evar_struct*) _temp904)->f2; goto _LL917; _LL917: _temp916=((
struct Cyc_Absyn_Evar_struct*) _temp904)->f3; goto _LL915; _LL915: _temp914=((
struct Cyc_Absyn_Evar_struct*) _temp904)->f4; goto _LL909;} else{ goto _LL910;}
_LL910: goto _LL911; _LL907: for( 0; _temp912 != 0; _temp912=(( struct Cyc_List_List*)
_check_null( _temp912))->tl){ if( Cyc_Tcutil_evar_in_effect( evar,( void*)((
struct Cyc_List_List*) _check_null( _temp912))->hd)){ return 1;}} return 0;
_LL909: return evar == e; _LL911: return 0; _LL905:;}} int Cyc_Tcutil_subset_effect(
int set_to_empty, void* e1, void* e2){{ struct _tuple9* _temp922= Cyc_Tcutil_get_effect_evar(
e2); struct _tuple9 _temp928; struct Cyc_List_List* _temp929; struct Cyc_List_List*
_temp931; void* _temp933; _LL924: if( _temp922 == 0){ goto _LL926;} else{
_temp928=* _temp922; _LL934: _temp933= _temp928.f1; goto _LL932; _LL932:
_temp931= _temp928.f2; goto _LL930; _LL930: _temp929= _temp928.f3; goto _LL925;}
_LL926: goto _LL927; _LL925: { void* _temp936= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,({ struct Cyc_Core_Opt* _temp935=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp935->v=( void*) _temp929; _temp935;})); goto
_LL937; _LL937: { struct Cyc_Absyn_JoinEff_struct* _temp942=({ struct Cyc_Absyn_JoinEff_struct*
_temp938=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp938[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp939; _temp939.tag= Cyc_Absyn_JoinEff;
_temp939.f1=({ struct Cyc_List_List* _temp940=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp940->hd=( void*) _temp936; _temp940->tl=({
struct Cyc_List_List* _temp941=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp941->hd=( void*) e1; _temp941->tl= 0; _temp941;});
_temp940;}); _temp939;}); _temp938;}); goto _LL943; _LL943: return Cyc_Tcutil_unify(
_temp933,( void*) _temp942);}} _LL927: goto _LL923; _LL923:;}{ void* _temp944=
Cyc_Tcutil_compress( e1); struct Cyc_List_List* _temp956; void* _temp958; struct
Cyc_Absyn_Tvar* _temp960; struct Cyc_Core_Opt* _temp962; int _temp964; struct
Cyc_Core_Opt* _temp966; struct Cyc_Core_Opt** _temp968; struct Cyc_Core_Opt*
_temp969; _LL946: if(( unsigned int) _temp944 > 4u?*(( int*) _temp944) == Cyc_Absyn_JoinEff:
0){ _LL957: _temp956=(( struct Cyc_Absyn_JoinEff_struct*) _temp944)->f1; goto
_LL947;} else{ goto _LL948;} _LL948: if(( unsigned int) _temp944 > 4u?*(( int*)
_temp944) == Cyc_Absyn_AccessEff: 0){ _LL959: _temp958=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp944)->f1; goto _LL949;} else{ goto _LL950;} _LL950: if(( unsigned int)
_temp944 > 4u?*(( int*) _temp944) == Cyc_Absyn_VarType: 0){ _LL961: _temp960=((
struct Cyc_Absyn_VarType_struct*) _temp944)->f1; goto _LL951;} else{ goto _LL952;}
_LL952: if(( unsigned int) _temp944 > 4u?*(( int*) _temp944) == Cyc_Absyn_Evar:
0){ _LL970: _temp969=(( struct Cyc_Absyn_Evar_struct*) _temp944)->f1; goto
_LL967; _LL967: _temp966=(( struct Cyc_Absyn_Evar_struct*) _temp944)->f2;
_temp968=&(( struct Cyc_Absyn_Evar_struct*) _temp944)->f2; goto _LL965; _LL965:
_temp964=(( struct Cyc_Absyn_Evar_struct*) _temp944)->f3; goto _LL963; _LL963:
_temp962=(( struct Cyc_Absyn_Evar_struct*) _temp944)->f4; goto _LL953;} else{
goto _LL954;} _LL954: goto _LL955; _LL947: for( 0; _temp956 != 0; _temp956=((
struct Cyc_List_List*) _check_null( _temp956))->tl){ if( ! Cyc_Tcutil_subset_effect(
set_to_empty,( void*)(( struct Cyc_List_List*) _check_null( _temp956))->hd, e2)){
return 0;}} return 1; _LL949: return( Cyc_Tcutil_region_in_effect( 0, _temp958,
e2)? 1: Cyc_Tcutil_region_in_effect( 1, _temp958, e2))? 1: Cyc_Tcutil_unify(
_temp958,( void*) Cyc_Absyn_HeapRgn); _LL951: return Cyc_Tcutil_variable_in_effect(
0, _temp960, e2)? 1: Cyc_Tcutil_variable_in_effect( 1, _temp960, e2); _LL953:
if( ! Cyc_Tcutil_evar_in_effect( e1, e2)){ if( set_to_empty){* _temp968=({
struct Cyc_Core_Opt* _temp971=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp971->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp972=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp972[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp973; _temp973.tag= Cyc_Absyn_JoinEff;
_temp973.f1= 0; _temp973;}); _temp972;})); _temp971;});} else{ Cyc_Tcutil_occurs(
e1,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp962))->v,
e2);* _temp968=({ struct Cyc_Core_Opt* _temp974=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp974->v=( void*) e2; _temp974;});}}
return 1; _LL955: return(( int(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)((
struct _tagged_string)({ struct _tagged_string _temp975= Cyc_Absynpp_typ2string(
e1); xprintf("subset_effect: bad effect: %.*s", _temp975.last_plus_one -
_temp975.curr, _temp975.curr);})); _LL945:;}} struct _tuple10{ struct _tuple9*
f1; struct _tuple9* f2; } ; static int Cyc_Tcutil_unify_effect( void* e1, void*
e2){ e1= Cyc_Tcutil_compress( e1); e2= Cyc_Tcutil_compress( e2); Cyc_Tcutil_normalize_effect(
e1); Cyc_Tcutil_normalize_effect( e2);{ struct _tuple10 _temp977=({ struct
_tuple10 _temp976; _temp976.f1= Cyc_Tcutil_get_effect_evar( e1); _temp976.f2=
Cyc_Tcutil_get_effect_evar( e2); _temp976;}); struct _tuple9* _temp983; struct
_tuple9 _temp985; struct Cyc_List_List* _temp986; struct Cyc_List_List* _temp988;
void* _temp990; struct _tuple9* _temp992; struct _tuple9 _temp994; struct Cyc_List_List*
_temp995; struct Cyc_List_List* _temp997; void* _temp999; _LL979: _LL993:
_temp992= _temp977.f1; if( _temp992 == 0){ goto _LL981;} else{ _temp994=*
_temp992; _LL1000: _temp999= _temp994.f1; goto _LL998; _LL998: _temp997=
_temp994.f2; goto _LL996; _LL996: _temp995= _temp994.f3; goto _LL984;} _LL984:
_temp983= _temp977.f2; if( _temp983 == 0){ goto _LL981;} else{ _temp985=*
_temp983; _LL991: _temp990= _temp985.f1; goto _LL989; _LL989: _temp988= _temp985.f2;
goto _LL987; _LL987: _temp986= _temp985.f3; goto _LL980;} _LL981: goto _LL982;
_LL980: { struct Cyc_List_List* _temp1001= 0; goto _LL1002; _LL1002: for( 0;
_temp995 != 0; _temp995=(( struct Cyc_List_List*) _check_null( _temp995))->tl){
if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp,
_temp986,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp995))->hd)){ _temp1001=({ struct Cyc_List_List* _temp1003=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1003->hd=( void*)(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp995))->hd); _temp1003->tl= _temp1001;
_temp1003;});}}{ void* _temp1005= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,({ struct Cyc_Core_Opt* _temp1004=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1004->v=( void*) _temp1001;
_temp1004;})); goto _LL1006; _LL1006: { struct Cyc_Absyn_JoinEff_struct*
_temp1010=({ struct Cyc_Absyn_JoinEff_struct* _temp1007=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp1007[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp1008; _temp1008.tag= Cyc_Absyn_JoinEff; _temp1008.f1=({
struct Cyc_List_List* _temp1009=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1009->hd=( void*) _temp1005; _temp1009->tl=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp997, _temp988); _temp1009;}); _temp1008;}); _temp1007;}); goto _LL1011;
_LL1011: return Cyc_Tcutil_unify( _temp999,( void*) _temp1010)? Cyc_Tcutil_unify(
_temp990,( void*) _temp1010): 0;}}} _LL982: return Cyc_Tcutil_subset_effect( 0,
e1, e2)? Cyc_Tcutil_subset_effect( 0, e2, e1): 0; _LL978:;}} static int Cyc_Tcutil_sub_rgnpo(
struct Cyc_List_List* rpo1, struct Cyc_List_List* rpo2){{ struct Cyc_List_List*
r1= rpo1; for( 0; r1 != 0; r1=(( struct Cyc_List_List*) _check_null( r1))->tl){
struct _tuple8 _temp1014; void* _temp1015; void* _temp1017; struct _tuple8*
_temp1012=( struct _tuple8*)(( struct Cyc_List_List*) _check_null( r1))->hd;
_temp1014=* _temp1012; _LL1018: _temp1017= _temp1014.f1; goto _LL1016; _LL1016:
_temp1015= _temp1014.f2; goto _LL1013; _LL1013: { int found= _temp1017 ==( void*)
Cyc_Absyn_HeapRgn;{ struct Cyc_List_List* r2= rpo2; for( 0; r2 != 0? ! found: 0;
r2=(( struct Cyc_List_List*) _check_null( r2))->tl){ struct _tuple8 _temp1021;
void* _temp1022; void* _temp1024; struct _tuple8* _temp1019=( struct _tuple8*)((
struct Cyc_List_List*) _check_null( r2))->hd; _temp1021=* _temp1019; _LL1025:
_temp1024= _temp1021.f1; goto _LL1023; _LL1023: _temp1022= _temp1021.f2; goto
_LL1020; _LL1020: if( Cyc_Tcutil_unify( _temp1017, _temp1024)? Cyc_Tcutil_unify(
_temp1015, _temp1022): 0){ found= 1; break;}}} if( ! found){ return 0;}}}}
return 1;} static int Cyc_Tcutil_same_rgn_po( struct Cyc_List_List* rpo1, struct
Cyc_List_List* rpo2){ return Cyc_Tcutil_sub_rgnpo( rpo1, rpo2)? Cyc_Tcutil_sub_rgnpo(
rpo2, rpo1): 0;} struct _tuple11{ struct Cyc_Absyn_VarargInfo* f1; struct Cyc_Absyn_VarargInfo*
f2; } ; struct _tuple12{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ;
void Cyc_Tcutil_unify_it( void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2=
Cyc_Tcutil_compress( t2); if( t1 == t2){ return;}{ void* _temp1026= t1; struct
Cyc_Core_Opt* _temp1032; int _temp1034; struct Cyc_Core_Opt* _temp1036; struct
Cyc_Core_Opt** _temp1038; struct Cyc_Core_Opt* _temp1039; _LL1028: if((
unsigned int) _temp1026 > 4u?*(( int*) _temp1026) == Cyc_Absyn_Evar: 0){ _LL1040:
_temp1039=(( struct Cyc_Absyn_Evar_struct*) _temp1026)->f1; goto _LL1037;
_LL1037: _temp1036=(( struct Cyc_Absyn_Evar_struct*) _temp1026)->f2; _temp1038=&((
struct Cyc_Absyn_Evar_struct*) _temp1026)->f2; goto _LL1035; _LL1035: _temp1034=((
struct Cyc_Absyn_Evar_struct*) _temp1026)->f3; goto _LL1033; _LL1033: _temp1032=((
struct Cyc_Absyn_Evar_struct*) _temp1026)->f4; goto _LL1029;} else{ goto _LL1030;}
_LL1030: goto _LL1031; _LL1029: Cyc_Tcutil_occurs( t1,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1032))->v, t2);{ void* _temp1041= Cyc_Tcutil_typ_kind(
t2); goto _LL1042; _LL1042: if( Cyc_Tcutil_kind_leq( _temp1041,( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp1039))->v)){* _temp1038=({ struct Cyc_Core_Opt*
_temp1043=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1043->v=( void*) t2; _temp1043;}); return;} else{{ void* _temp1044= t2;
struct Cyc_Core_Opt* _temp1052; int _temp1054; struct Cyc_Core_Opt* _temp1056;
struct Cyc_Core_Opt** _temp1058; struct Cyc_Core_Opt* _temp1059; struct Cyc_Absyn_PtrInfo
_temp1062; _LL1046: if(( unsigned int) _temp1044 > 4u?*(( int*) _temp1044) ==
Cyc_Absyn_Evar: 0){ _LL1060: _temp1059=(( struct Cyc_Absyn_Evar_struct*)
_temp1044)->f1; goto _LL1057; _LL1057: _temp1056=(( struct Cyc_Absyn_Evar_struct*)
_temp1044)->f2; _temp1058=&(( struct Cyc_Absyn_Evar_struct*) _temp1044)->f2;
goto _LL1055; _LL1055: _temp1054=(( struct Cyc_Absyn_Evar_struct*) _temp1044)->f3;
goto _LL1053; _LL1053: _temp1052=(( struct Cyc_Absyn_Evar_struct*) _temp1044)->f4;
goto _LL1047;} else{ goto _LL1048;} _LL1048: if(( unsigned int) _temp1044 > 4u?*((
int*) _temp1044) == Cyc_Absyn_PointerType: 0){ _LL1063: _temp1062=(( struct Cyc_Absyn_PointerType_struct*)
_temp1044)->f1; goto _LL1061;} else{ goto _LL1050;} _LL1061: if(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1039))->v ==( void*) Cyc_Absyn_BoxKind){
goto _LL1049;} else{ goto _LL1050;} _LL1050: goto _LL1051; _LL1047: { struct Cyc_List_List*
_temp1064=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp1032))->v; goto _LL1065; _LL1065:{ struct Cyc_List_List* s2=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1052))->v; for( 0; s2 != 0; s2=(( struct
Cyc_List_List*) _check_null( s2))->tl){ if( !(( int(*)( int(* compare)( struct
Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar*
x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, _temp1064,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( s2))->hd)){( void) _throw(( void*) Cyc_Tcutil_Unify);}}}
if( Cyc_Tcutil_kind_leq(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1039))->v,
_temp1041)){* _temp1058=({ struct Cyc_Core_Opt* _temp1066=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1066->v=( void*) t1; _temp1066;});
return;} goto _LL1045;} _LL1049: { struct Cyc_Absyn_Conref* _temp1067=(( struct
Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp1062.bounds); goto _LL1068; _LL1068:{ void* _temp1069=( void*) _temp1067->v;
_LL1071: if( _temp1069 ==( void*) Cyc_Absyn_No_constr){ goto _LL1072;} else{
goto _LL1073;} _LL1073: goto _LL1074; _LL1072:( void*)( _temp1067->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp1075=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp1075[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp1076; _temp1076.tag= Cyc_Absyn_Eq_constr;
_temp1076.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1077=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1077[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1078; _temp1078.tag= Cyc_Absyn_Upper_b;
_temp1078.f1= Cyc_Absyn_signed_int_exp( 1, 0); _temp1078;}); _temp1077;}));
_temp1076;}); _temp1075;})));* _temp1038=({ struct Cyc_Core_Opt* _temp1079=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1079->v=(
void*) t2; _temp1079;}); return; _LL1074: goto _LL1070; _LL1070:;} goto _LL1045;}
_LL1051: goto _LL1045; _LL1045:;}( void) _throw(( void*) Cyc_Tcutil_Unify);}}
_LL1031: goto _LL1027; _LL1027:;}{ struct _tuple8 _temp1081=({ struct _tuple8
_temp1080; _temp1080.f1= t2; _temp1080.f2= t1; _temp1080;}); void* _temp1131;
void* _temp1133; struct Cyc_Core_Opt* _temp1135; int _temp1137; struct Cyc_Core_Opt*
_temp1139; struct Cyc_Core_Opt* _temp1141; void* _temp1143; void* _temp1145;
void* _temp1147; struct Cyc_Absyn_Tvar* _temp1149; void* _temp1151; struct Cyc_Absyn_Tvar*
_temp1153; void* _temp1155; struct Cyc_Absyn_Structdecl** _temp1157; struct Cyc_List_List*
_temp1159; struct _tuple1* _temp1161; void* _temp1163; struct Cyc_Absyn_Structdecl**
_temp1165; struct Cyc_List_List* _temp1167; struct _tuple1* _temp1169; void*
_temp1171; struct Cyc_Absyn_Enumdecl* _temp1173; struct _tuple1* _temp1175; void*
_temp1177; struct Cyc_Absyn_Enumdecl* _temp1179; struct _tuple1* _temp1181; void*
_temp1183; struct Cyc_Absyn_Uniondecl** _temp1185; struct Cyc_List_List*
_temp1187; struct _tuple1* _temp1189; void* _temp1191; struct Cyc_Absyn_Uniondecl**
_temp1193; struct Cyc_List_List* _temp1195; struct _tuple1* _temp1197; void*
_temp1199; struct Cyc_Absyn_TunionInfo _temp1201; void* _temp1203; struct Cyc_List_List*
_temp1205; void* _temp1207; struct Cyc_Absyn_Tuniondecl* _temp1209; void*
_temp1211; struct Cyc_Absyn_TunionInfo _temp1213; void* _temp1215; struct Cyc_List_List*
_temp1217; void* _temp1219; struct Cyc_Absyn_Tuniondecl* _temp1221; void*
_temp1223; struct Cyc_Absyn_TunionFieldInfo _temp1225; struct Cyc_List_List*
_temp1227; void* _temp1229; struct Cyc_Absyn_Tunionfield* _temp1231; struct Cyc_Absyn_Tuniondecl*
_temp1233; void* _temp1235; struct Cyc_Absyn_TunionFieldInfo _temp1237; struct
Cyc_List_List* _temp1239; void* _temp1241; struct Cyc_Absyn_Tunionfield*
_temp1243; struct Cyc_Absyn_Tuniondecl* _temp1245; void* _temp1247; struct Cyc_Absyn_PtrInfo
_temp1249; struct Cyc_Absyn_Conref* _temp1251; struct Cyc_Absyn_Tqual _temp1253;
struct Cyc_Absyn_Conref* _temp1255; void* _temp1257; void* _temp1259; void*
_temp1261; struct Cyc_Absyn_PtrInfo _temp1263; struct Cyc_Absyn_Conref*
_temp1265; struct Cyc_Absyn_Tqual _temp1267; struct Cyc_Absyn_Conref* _temp1269;
void* _temp1271; void* _temp1273; void* _temp1275; void* _temp1277; void*
_temp1279; void* _temp1281; void* _temp1283; void* _temp1285; void* _temp1287;
void* _temp1289; void* _temp1291; void* _temp1293; void* _temp1295; struct Cyc_Absyn_Exp*
_temp1297; struct Cyc_Absyn_Tqual _temp1299; void* _temp1301; void* _temp1303;
struct Cyc_Absyn_Exp* _temp1305; struct Cyc_Absyn_Tqual _temp1307; void*
_temp1309; void* _temp1311; struct Cyc_Absyn_FnInfo _temp1313; struct Cyc_List_List*
_temp1315; struct Cyc_List_List* _temp1317; struct Cyc_Absyn_VarargInfo*
_temp1319; int _temp1321; struct Cyc_List_List* _temp1323; void* _temp1325;
struct Cyc_Core_Opt* _temp1327; struct Cyc_List_List* _temp1329; void* _temp1331;
struct Cyc_Absyn_FnInfo _temp1333; struct Cyc_List_List* _temp1335; struct Cyc_List_List*
_temp1337; struct Cyc_Absyn_VarargInfo* _temp1339; int _temp1341; struct Cyc_List_List*
_temp1343; void* _temp1345; struct Cyc_Core_Opt* _temp1347; struct Cyc_List_List*
_temp1349; void* _temp1351; struct Cyc_List_List* _temp1353; void* _temp1355;
struct Cyc_List_List* _temp1357; void* _temp1359; struct Cyc_List_List*
_temp1361; void* _temp1363; struct Cyc_List_List* _temp1365; void* _temp1367;
struct Cyc_List_List* _temp1369; void* _temp1371; struct Cyc_List_List*
_temp1373; void* _temp1375; void* _temp1377; void* _temp1379; void* _temp1381;
void* _temp1383; void* _temp1385; void* _temp1387; void* _temp1389; struct Cyc_List_List*
_temp1391; void* _temp1393; struct Cyc_List_List* _temp1395; void* _temp1397;
void* _temp1399; void* _temp1401; void* _temp1403; void* _temp1405; void*
_temp1407; void* _temp1409; _LL1083: _LL1134: _temp1133= _temp1081.f1; if((
unsigned int) _temp1133 > 4u?*(( int*) _temp1133) == Cyc_Absyn_Evar: 0){ _LL1142:
_temp1141=(( struct Cyc_Absyn_Evar_struct*) _temp1133)->f1; goto _LL1140;
_LL1140: _temp1139=(( struct Cyc_Absyn_Evar_struct*) _temp1133)->f2; goto
_LL1138; _LL1138: _temp1137=(( struct Cyc_Absyn_Evar_struct*) _temp1133)->f3;
goto _LL1136; _LL1136: _temp1135=(( struct Cyc_Absyn_Evar_struct*) _temp1133)->f4;
goto _LL1132;} else{ goto _LL1085;} _LL1132: _temp1131= _temp1081.f2; goto
_LL1084; _LL1085: _LL1146: _temp1145= _temp1081.f1; if( _temp1145 ==( void*) Cyc_Absyn_VoidType){
goto _LL1144;} else{ goto _LL1087;} _LL1144: _temp1143= _temp1081.f2; if(
_temp1143 ==( void*) Cyc_Absyn_VoidType){ goto _LL1086;} else{ goto _LL1087;}
_LL1087: _LL1152: _temp1151= _temp1081.f1; if(( unsigned int) _temp1151 > 4u?*((
int*) _temp1151) == Cyc_Absyn_VarType: 0){ _LL1154: _temp1153=(( struct Cyc_Absyn_VarType_struct*)
_temp1151)->f1; goto _LL1148;} else{ goto _LL1089;} _LL1148: _temp1147=
_temp1081.f2; if(( unsigned int) _temp1147 > 4u?*(( int*) _temp1147) == Cyc_Absyn_VarType:
0){ _LL1150: _temp1149=(( struct Cyc_Absyn_VarType_struct*) _temp1147)->f1; goto
_LL1088;} else{ goto _LL1089;} _LL1089: _LL1164: _temp1163= _temp1081.f1; if((
unsigned int) _temp1163 > 4u?*(( int*) _temp1163) == Cyc_Absyn_StructType: 0){
_LL1170: _temp1169=(( struct Cyc_Absyn_StructType_struct*) _temp1163)->f1; goto
_LL1168; _LL1168: _temp1167=(( struct Cyc_Absyn_StructType_struct*) _temp1163)->f2;
goto _LL1166; _LL1166: _temp1165=(( struct Cyc_Absyn_StructType_struct*)
_temp1163)->f3; goto _LL1156;} else{ goto _LL1091;} _LL1156: _temp1155=
_temp1081.f2; if(( unsigned int) _temp1155 > 4u?*(( int*) _temp1155) == Cyc_Absyn_StructType:
0){ _LL1162: _temp1161=(( struct Cyc_Absyn_StructType_struct*) _temp1155)->f1;
goto _LL1160; _LL1160: _temp1159=(( struct Cyc_Absyn_StructType_struct*)
_temp1155)->f2; goto _LL1158; _LL1158: _temp1157=(( struct Cyc_Absyn_StructType_struct*)
_temp1155)->f3; goto _LL1090;} else{ goto _LL1091;} _LL1091: _LL1178: _temp1177=
_temp1081.f1; if(( unsigned int) _temp1177 > 4u?*(( int*) _temp1177) == Cyc_Absyn_EnumType:
0){ _LL1182: _temp1181=(( struct Cyc_Absyn_EnumType_struct*) _temp1177)->f1;
goto _LL1180; _LL1180: _temp1179=(( struct Cyc_Absyn_EnumType_struct*) _temp1177)->f2;
goto _LL1172;} else{ goto _LL1093;} _LL1172: _temp1171= _temp1081.f2; if((
unsigned int) _temp1171 > 4u?*(( int*) _temp1171) == Cyc_Absyn_EnumType: 0){
_LL1176: _temp1175=(( struct Cyc_Absyn_EnumType_struct*) _temp1171)->f1; goto
_LL1174; _LL1174: _temp1173=(( struct Cyc_Absyn_EnumType_struct*) _temp1171)->f2;
goto _LL1092;} else{ goto _LL1093;} _LL1093: _LL1192: _temp1191= _temp1081.f1;
if(( unsigned int) _temp1191 > 4u?*(( int*) _temp1191) == Cyc_Absyn_UnionType: 0){
_LL1198: _temp1197=(( struct Cyc_Absyn_UnionType_struct*) _temp1191)->f1; goto
_LL1196; _LL1196: _temp1195=(( struct Cyc_Absyn_UnionType_struct*) _temp1191)->f2;
goto _LL1194; _LL1194: _temp1193=(( struct Cyc_Absyn_UnionType_struct*)
_temp1191)->f3; goto _LL1184;} else{ goto _LL1095;} _LL1184: _temp1183=
_temp1081.f2; if(( unsigned int) _temp1183 > 4u?*(( int*) _temp1183) == Cyc_Absyn_UnionType:
0){ _LL1190: _temp1189=(( struct Cyc_Absyn_UnionType_struct*) _temp1183)->f1;
goto _LL1188; _LL1188: _temp1187=(( struct Cyc_Absyn_UnionType_struct*)
_temp1183)->f2; goto _LL1186; _LL1186: _temp1185=(( struct Cyc_Absyn_UnionType_struct*)
_temp1183)->f3; goto _LL1094;} else{ goto _LL1095;} _LL1095: _LL1212: _temp1211=
_temp1081.f1; if(( unsigned int) _temp1211 > 4u?*(( int*) _temp1211) == Cyc_Absyn_TunionType:
0){ _LL1214: _temp1213=(( struct Cyc_Absyn_TunionType_struct*) _temp1211)->f1;
_LL1220: _temp1219=( void*) _temp1213.tunion_info; if(*(( int*) _temp1219) ==
Cyc_Absyn_KnownTunion){ _LL1222: _temp1221=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1219)->f1; goto _LL1218;} else{ goto _LL1097;} _LL1218: _temp1217=
_temp1213.targs; goto _LL1216; _LL1216: _temp1215=( void*) _temp1213.rgn; goto
_LL1200;} else{ goto _LL1097;} _LL1200: _temp1199= _temp1081.f2; if((
unsigned int) _temp1199 > 4u?*(( int*) _temp1199) == Cyc_Absyn_TunionType: 0){
_LL1202: _temp1201=(( struct Cyc_Absyn_TunionType_struct*) _temp1199)->f1;
_LL1208: _temp1207=( void*) _temp1201.tunion_info; if(*(( int*) _temp1207) ==
Cyc_Absyn_KnownTunion){ _LL1210: _temp1209=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1207)->f1; goto _LL1206;} else{ goto _LL1097;} _LL1206: _temp1205=
_temp1201.targs; goto _LL1204; _LL1204: _temp1203=( void*) _temp1201.rgn; goto
_LL1096;} else{ goto _LL1097;} _LL1097: _LL1236: _temp1235= _temp1081.f1; if((
unsigned int) _temp1235 > 4u?*(( int*) _temp1235) == Cyc_Absyn_TunionFieldType:
0){ _LL1238: _temp1237=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1235)->f1;
_LL1242: _temp1241=( void*) _temp1237.field_info; if(*(( int*) _temp1241) == Cyc_Absyn_KnownTunionfield){
_LL1246: _temp1245=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1241)->f1;
goto _LL1244; _LL1244: _temp1243=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1241)->f2; goto _LL1240;} else{ goto _LL1099;} _LL1240: _temp1239=
_temp1237.targs; goto _LL1224;} else{ goto _LL1099;} _LL1224: _temp1223=
_temp1081.f2; if(( unsigned int) _temp1223 > 4u?*(( int*) _temp1223) == Cyc_Absyn_TunionFieldType:
0){ _LL1226: _temp1225=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1223)->f1;
_LL1230: _temp1229=( void*) _temp1225.field_info; if(*(( int*) _temp1229) == Cyc_Absyn_KnownTunionfield){
_LL1234: _temp1233=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1229)->f1;
goto _LL1232; _LL1232: _temp1231=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1229)->f2; goto _LL1228;} else{ goto _LL1099;} _LL1228: _temp1227=
_temp1225.targs; goto _LL1098;} else{ goto _LL1099;} _LL1099: _LL1262: _temp1261=
_temp1081.f1; if(( unsigned int) _temp1261 > 4u?*(( int*) _temp1261) == Cyc_Absyn_PointerType:
0){ _LL1264: _temp1263=(( struct Cyc_Absyn_PointerType_struct*) _temp1261)->f1;
_LL1274: _temp1273=( void*) _temp1263.elt_typ; goto _LL1272; _LL1272: _temp1271=(
void*) _temp1263.rgn_typ; goto _LL1270; _LL1270: _temp1269= _temp1263.nullable;
goto _LL1268; _LL1268: _temp1267= _temp1263.tq; goto _LL1266; _LL1266: _temp1265=
_temp1263.bounds; goto _LL1248;} else{ goto _LL1101;} _LL1248: _temp1247=
_temp1081.f2; if(( unsigned int) _temp1247 > 4u?*(( int*) _temp1247) == Cyc_Absyn_PointerType:
0){ _LL1250: _temp1249=(( struct Cyc_Absyn_PointerType_struct*) _temp1247)->f1;
_LL1260: _temp1259=( void*) _temp1249.elt_typ; goto _LL1258; _LL1258: _temp1257=(
void*) _temp1249.rgn_typ; goto _LL1256; _LL1256: _temp1255= _temp1249.nullable;
goto _LL1254; _LL1254: _temp1253= _temp1249.tq; goto _LL1252; _LL1252: _temp1251=
_temp1249.bounds; goto _LL1100;} else{ goto _LL1101;} _LL1101: _LL1282:
_temp1281= _temp1081.f1; if(( unsigned int) _temp1281 > 4u?*(( int*) _temp1281)
== Cyc_Absyn_IntType: 0){ _LL1286: _temp1285=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1281)->f1; goto _LL1284; _LL1284: _temp1283=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1281)->f2; goto _LL1276;} else{ goto _LL1103;} _LL1276: _temp1275=
_temp1081.f2; if(( unsigned int) _temp1275 > 4u?*(( int*) _temp1275) == Cyc_Absyn_IntType:
0){ _LL1280: _temp1279=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1275)->f1;
goto _LL1278; _LL1278: _temp1277=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1275)->f2; goto _LL1102;} else{ goto _LL1103;} _LL1103: _LL1290: _temp1289=
_temp1081.f1; if( _temp1289 ==( void*) Cyc_Absyn_FloatType){ goto _LL1288;}
else{ goto _LL1105;} _LL1288: _temp1287= _temp1081.f2; if( _temp1287 ==( void*)
Cyc_Absyn_FloatType){ goto _LL1104;} else{ goto _LL1105;} _LL1105: _LL1294:
_temp1293= _temp1081.f1; if( _temp1293 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL1292;} else{ goto _LL1107;} _LL1292: _temp1291= _temp1081.f2; if( _temp1291
==( void*) Cyc_Absyn_DoubleType){ goto _LL1106;} else{ goto _LL1107;} _LL1107:
_LL1304: _temp1303= _temp1081.f1; if(( unsigned int) _temp1303 > 4u?*(( int*)
_temp1303) == Cyc_Absyn_ArrayType: 0){ _LL1310: _temp1309=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1303)->f1; goto _LL1308; _LL1308: _temp1307=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1303)->f2; goto _LL1306; _LL1306: _temp1305=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1303)->f3; goto _LL1296;} else{ goto _LL1109;} _LL1296: _temp1295=
_temp1081.f2; if(( unsigned int) _temp1295 > 4u?*(( int*) _temp1295) == Cyc_Absyn_ArrayType:
0){ _LL1302: _temp1301=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1295)->f1;
goto _LL1300; _LL1300: _temp1299=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1295)->f2; goto _LL1298; _LL1298: _temp1297=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1295)->f3; goto _LL1108;} else{ goto _LL1109;} _LL1109: _LL1332: _temp1331=
_temp1081.f1; if(( unsigned int) _temp1331 > 4u?*(( int*) _temp1331) == Cyc_Absyn_FnType:
0){ _LL1334: _temp1333=(( struct Cyc_Absyn_FnType_struct*) _temp1331)->f1;
_LL1350: _temp1349= _temp1333.tvars; goto _LL1348; _LL1348: _temp1347= _temp1333.effect;
goto _LL1346; _LL1346: _temp1345=( void*) _temp1333.ret_typ; goto _LL1344;
_LL1344: _temp1343= _temp1333.args; goto _LL1342; _LL1342: _temp1341= _temp1333.c_varargs;
goto _LL1340; _LL1340: _temp1339= _temp1333.cyc_varargs; goto _LL1338; _LL1338:
_temp1337= _temp1333.rgn_po; goto _LL1336; _LL1336: _temp1335= _temp1333.attributes;
goto _LL1312;} else{ goto _LL1111;} _LL1312: _temp1311= _temp1081.f2; if((
unsigned int) _temp1311 > 4u?*(( int*) _temp1311) == Cyc_Absyn_FnType: 0){
_LL1314: _temp1313=(( struct Cyc_Absyn_FnType_struct*) _temp1311)->f1; _LL1330:
_temp1329= _temp1313.tvars; goto _LL1328; _LL1328: _temp1327= _temp1313.effect;
goto _LL1326; _LL1326: _temp1325=( void*) _temp1313.ret_typ; goto _LL1324;
_LL1324: _temp1323= _temp1313.args; goto _LL1322; _LL1322: _temp1321= _temp1313.c_varargs;
goto _LL1320; _LL1320: _temp1319= _temp1313.cyc_varargs; goto _LL1318; _LL1318:
_temp1317= _temp1313.rgn_po; goto _LL1316; _LL1316: _temp1315= _temp1313.attributes;
goto _LL1110;} else{ goto _LL1111;} _LL1111: _LL1356: _temp1355= _temp1081.f1;
if(( unsigned int) _temp1355 > 4u?*(( int*) _temp1355) == Cyc_Absyn_TupleType: 0){
_LL1358: _temp1357=(( struct Cyc_Absyn_TupleType_struct*) _temp1355)->f1; goto
_LL1352;} else{ goto _LL1113;} _LL1352: _temp1351= _temp1081.f2; if((
unsigned int) _temp1351 > 4u?*(( int*) _temp1351) == Cyc_Absyn_TupleType: 0){
_LL1354: _temp1353=(( struct Cyc_Absyn_TupleType_struct*) _temp1351)->f1; goto
_LL1112;} else{ goto _LL1113;} _LL1113: _LL1364: _temp1363= _temp1081.f1; if((
unsigned int) _temp1363 > 4u?*(( int*) _temp1363) == Cyc_Absyn_AnonStructType: 0){
_LL1366: _temp1365=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1363)->f1;
goto _LL1360;} else{ goto _LL1115;} _LL1360: _temp1359= _temp1081.f2; if((
unsigned int) _temp1359 > 4u?*(( int*) _temp1359) == Cyc_Absyn_AnonStructType: 0){
_LL1362: _temp1361=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1359)->f1;
goto _LL1114;} else{ goto _LL1115;} _LL1115: _LL1372: _temp1371= _temp1081.f1;
if(( unsigned int) _temp1371 > 4u?*(( int*) _temp1371) == Cyc_Absyn_AnonUnionType:
0){ _LL1374: _temp1373=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1371)->f1;
goto _LL1368;} else{ goto _LL1117;} _LL1368: _temp1367= _temp1081.f2; if((
unsigned int) _temp1367 > 4u?*(( int*) _temp1367) == Cyc_Absyn_AnonUnionType: 0){
_LL1370: _temp1369=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1367)->f1;
goto _LL1116;} else{ goto _LL1117;} _LL1117: _LL1378: _temp1377= _temp1081.f1;
if( _temp1377 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1376;} else{ goto _LL1119;}
_LL1376: _temp1375= _temp1081.f2; if( _temp1375 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL1118;} else{ goto _LL1119;} _LL1119: _LL1384: _temp1383= _temp1081.f1;
if(( unsigned int) _temp1383 > 4u?*(( int*) _temp1383) == Cyc_Absyn_RgnHandleType:
0){ _LL1386: _temp1385=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1383)->f1; goto _LL1380;} else{ goto _LL1121;} _LL1380: _temp1379=
_temp1081.f2; if(( unsigned int) _temp1379 > 4u?*(( int*) _temp1379) == Cyc_Absyn_RgnHandleType:
0){ _LL1382: _temp1381=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1379)->f1; goto _LL1120;} else{ goto _LL1121;} _LL1121: _LL1390: _temp1389=
_temp1081.f1; if(( unsigned int) _temp1389 > 4u?*(( int*) _temp1389) == Cyc_Absyn_JoinEff:
0){ _LL1392: _temp1391=(( struct Cyc_Absyn_JoinEff_struct*) _temp1389)->f1; goto
_LL1388;} else{ goto _LL1123;} _LL1388: _temp1387= _temp1081.f2; goto _LL1122;
_LL1123: _LL1398: _temp1397= _temp1081.f1; goto _LL1394; _LL1394: _temp1393=
_temp1081.f2; if(( unsigned int) _temp1393 > 4u?*(( int*) _temp1393) == Cyc_Absyn_JoinEff:
0){ _LL1396: _temp1395=(( struct Cyc_Absyn_JoinEff_struct*) _temp1393)->f1; goto
_LL1124;} else{ goto _LL1125;} _LL1125: _LL1402: _temp1401= _temp1081.f1; if((
unsigned int) _temp1401 > 4u?*(( int*) _temp1401) == Cyc_Absyn_AccessEff: 0){
_LL1404: _temp1403=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp1401)->f1;
goto _LL1400;} else{ goto _LL1127;} _LL1400: _temp1399= _temp1081.f2; goto
_LL1126; _LL1127: _LL1410: _temp1409= _temp1081.f1; goto _LL1406; _LL1406:
_temp1405= _temp1081.f2; if(( unsigned int) _temp1405 > 4u?*(( int*) _temp1405)
== Cyc_Absyn_AccessEff: 0){ _LL1408: _temp1407=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp1405)->f1; goto _LL1128;} else{ goto _LL1129;} _LL1129: goto _LL1130;
_LL1084: Cyc_Tcutil_unify_it( t2, t1); return; _LL1086: return; _LL1088: {
struct _tagged_string* _temp1411= _temp1153->name; goto _LL1412; _LL1412: {
struct _tagged_string* _temp1413= _temp1149->name; goto _LL1414; _LL1414: { int
_temp1415=*(( int*) _check_null( _temp1153->identity)); goto _LL1416; _LL1416: {
int _temp1417=*(( int*) _check_null( _temp1149->identity)); goto _LL1418;
_LL1418: { void* _temp1419=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1153->kind); goto _LL1420; _LL1420: { void* _temp1421=(( void*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1149->kind); goto _LL1422;
_LL1422: if( _temp1417 == _temp1415? Cyc_String_zstrptrcmp( _temp1411, _temp1413)
== 0: 0){ if( _temp1419 != _temp1421){(( void(*)( struct _tagged_string msg))
Cyc_Tcutil_impos)(( struct _tagged_string)({ struct _tagged_string _temp1423=*
_temp1411; struct _tagged_string _temp1424= Cyc_Absynpp_kind2string( _temp1419);
struct _tagged_string _temp1425= Cyc_Absynpp_kind2string( _temp1421); xprintf("same type variable %.*s has kinds %.*s and %.*s",
_temp1423.last_plus_one - _temp1423.curr, _temp1423.curr, _temp1424.last_plus_one
- _temp1424.curr, _temp1424.curr, _temp1425.last_plus_one - _temp1425.curr,
_temp1425.curr);}));} return;} goto _LL1082;}}}}}} _LL1090: if((( _temp1161 != 0?
_temp1169 != 0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*) _check_null( _temp1161),(
struct _tuple1*) _check_null( _temp1169)) == 0: 0)? 1:( _temp1161 == 0?
_temp1169 == 0: 0)){ Cyc_Tcutil_unify_list( _temp1159, _temp1167); return;} goto
_LL1082; _LL1092: if( Cyc_Absyn_qvar_cmp( _temp1181, _temp1175) == 0){ return;}
goto _LL1082; _LL1094: if((( _temp1189 != 0? _temp1197 != 0: 0)? Cyc_Absyn_qvar_cmp((
struct _tuple1*) _check_null( _temp1189),( struct _tuple1*) _check_null(
_temp1197)) == 0: 0)? 1:( _temp1189 == 0? _temp1197 == 0: 0)){ Cyc_Tcutil_unify_list(
_temp1187, _temp1195); return;} goto _LL1082; _LL1096: if( _temp1221 ==
_temp1209? 1: Cyc_Absyn_qvar_cmp( _temp1221->name, _temp1209->name) == 0){ Cyc_Tcutil_unify_it(
_temp1203, _temp1215); Cyc_Tcutil_unify_list( _temp1205, _temp1217); return;}
goto _LL1082; _LL1098: if(( _temp1245 == _temp1233? 1: Cyc_Absyn_qvar_cmp(
_temp1245->name, _temp1233->name) == 0)? _temp1243 == _temp1231? 1: Cyc_Absyn_qvar_cmp(
_temp1243->name, _temp1231->name) == 0: 0){ Cyc_Tcutil_unify_list( _temp1227,
_temp1239); return;} goto _LL1082; _LL1100: Cyc_Tcutil_unify_it( _temp1259,
_temp1273); Cyc_Tcutil_unify_it( _temp1271, _temp1257); Cyc_Tcutil_unify_tqual(
_temp1253, _temp1267);(( void(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)( Cyc_Tcutil_boundscmp,
_temp1251, _temp1265);{ void* _temp1426=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1251))->v; void*
_temp1432; _LL1428: if(( unsigned int) _temp1426 > 1u?*(( int*) _temp1426) ==
Cyc_Absyn_Eq_constr: 0){ _LL1433: _temp1432=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1426)->f1; if( _temp1432 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1429;}
else{ goto _LL1430;}} else{ goto _LL1430;} _LL1430: goto _LL1431; _LL1429:
return; _LL1431: goto _LL1427; _LL1427:;}(( void(*)( int(* cmp)( int, int),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp, _temp1255, _temp1269); return; _LL1102: if( _temp1279 ==
_temp1285? _temp1277 == _temp1283: 0){ return;} goto _LL1082; _LL1104: return;
_LL1106: return; _LL1108: Cyc_Tcutil_unify_it( _temp1301, _temp1309); Cyc_Tcutil_unify_tqual(
_temp1299, _temp1307); if( _temp1305 == _temp1297){ return;} if( _temp1305 == 0?
1: _temp1297 == 0){ goto _LL1082;} if( Cyc_Evexp_eval_const_uint_exp(( struct
Cyc_Absyn_Exp*) _check_null( _temp1305)) == Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1297))){ return;} goto _LL1082; _LL1110: {
int done= 0;{ struct _RegionHandle _temp1434= _new_region(); struct
_RegionHandle* rgn=& _temp1434; _push_region( rgn);{ struct Cyc_List_List* inst=
0; while( _temp1329 != 0) { if( _temp1349 == 0){ break;} inst=({ struct Cyc_List_List*
_temp1435=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1435->hd=( void*)({ struct _tuple5* _temp1436=( struct _tuple5*)
_region_malloc( rgn, sizeof( struct _tuple5)); _temp1436->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp1349))->hd; _temp1436->f2=( void*)({
struct Cyc_Absyn_VarType_struct* _temp1437=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp1437[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp1438; _temp1438.tag= Cyc_Absyn_VarType; _temp1438.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp1329))->hd;
_temp1438;}); _temp1437;}); _temp1436;}); _temp1435->tl= inst; _temp1435;});
_temp1329=(( struct Cyc_List_List*) _check_null( _temp1329))->tl; _temp1349=((
struct Cyc_List_List*) _check_null( _temp1349))->tl;} if( _temp1349 != 0){
_npop_handler( 0u); goto _LL1082;} if( inst != 0){ Cyc_Tcutil_unify_it(( void*)({
struct Cyc_Absyn_FnType_struct* _temp1439=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1439[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1440; _temp1440.tag= Cyc_Absyn_FnType; _temp1440.f1=({ struct Cyc_Absyn_FnInfo
_temp1441; _temp1441.tvars= 0; _temp1441.effect= _temp1327; _temp1441.ret_typ=(
void*) _temp1325; _temp1441.args= _temp1323; _temp1441.c_varargs= _temp1321;
_temp1441.cyc_varargs= _temp1319; _temp1441.rgn_po= _temp1317; _temp1441.attributes=
_temp1315; _temp1441;}); _temp1440;}); _temp1439;}), Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1442=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1442[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1443; _temp1443.tag= Cyc_Absyn_FnType; _temp1443.f1=({ struct Cyc_Absyn_FnInfo
_temp1444; _temp1444.tvars= 0; _temp1444.effect= _temp1347; _temp1444.ret_typ=(
void*) _temp1345; _temp1444.args= _temp1343; _temp1444.c_varargs= _temp1341;
_temp1444.cyc_varargs= _temp1339; _temp1444.rgn_po= _temp1337; _temp1444.attributes=
_temp1335; _temp1444;}); _temp1443;}); _temp1442;}))); done= 1;}}; _pop_region(&
_temp1434);} if( done){ return;} Cyc_Tcutil_unify_it( _temp1325, _temp1345);
for( 0; _temp1323 != 0? _temp1343 != 0: 0;( _temp1323=(( struct Cyc_List_List*)
_check_null( _temp1323))->tl, _temp1343=(( struct Cyc_List_List*) _check_null(
_temp1343))->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp1323))->hd)).f2,(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp1343))->hd)).f2); Cyc_Tcutil_unify_it((*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp1323))->hd)).f3,(*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp1343))->hd)).f3);} if( _temp1323 != 0?
1: _temp1343 != 0){ goto _LL1082;} if( _temp1321 != _temp1341){ goto _LL1082;}{
int bad_cyc_vararg= 0;{ struct _tuple11 _temp1446=({ struct _tuple11 _temp1445;
_temp1445.f1= _temp1319; _temp1445.f2= _temp1339; _temp1445;}); struct Cyc_Absyn_VarargInfo*
_temp1456; struct Cyc_Absyn_VarargInfo* _temp1458; struct Cyc_Absyn_VarargInfo*
_temp1460; struct Cyc_Absyn_VarargInfo* _temp1462; struct Cyc_Absyn_VarargInfo*
_temp1464; struct Cyc_Absyn_VarargInfo* _temp1466; struct Cyc_Absyn_VarargInfo*
_temp1468; struct Cyc_Absyn_VarargInfo _temp1470; int _temp1471; void* _temp1473;
void* _temp1475; struct Cyc_Absyn_Tqual _temp1477; struct Cyc_Core_Opt*
_temp1479; struct Cyc_Absyn_VarargInfo* _temp1481; struct Cyc_Absyn_VarargInfo
_temp1483; int _temp1484; void* _temp1486; void* _temp1488; struct Cyc_Absyn_Tqual
_temp1490; struct Cyc_Core_Opt* _temp1492; _LL1448: _LL1459: _temp1458=
_temp1446.f1; if( _temp1458 == 0){ goto _LL1457;} else{ goto _LL1450;} _LL1457:
_temp1456= _temp1446.f2; if( _temp1456 == 0){ goto _LL1449;} else{ goto _LL1450;}
_LL1450: _LL1463: _temp1462= _temp1446.f1; if( _temp1462 == 0){ goto _LL1461;}
else{ goto _LL1452;} _LL1461: _temp1460= _temp1446.f2; goto _LL1451; _LL1452:
_LL1467: _temp1466= _temp1446.f1; goto _LL1465; _LL1465: _temp1464= _temp1446.f2;
if( _temp1464 == 0){ goto _LL1453;} else{ goto _LL1454;} _LL1454: _LL1482:
_temp1481= _temp1446.f1; if( _temp1481 == 0){ goto _LL1447;} else{ _temp1483=*
_temp1481; _LL1493: _temp1492= _temp1483.name; goto _LL1491; _LL1491: _temp1490=
_temp1483.tq; goto _LL1489; _LL1489: _temp1488=( void*) _temp1483.type; goto
_LL1487; _LL1487: _temp1486=( void*) _temp1483.rgn; goto _LL1485; _LL1485:
_temp1484= _temp1483.inject; goto _LL1469;} _LL1469: _temp1468= _temp1446.f2;
if( _temp1468 == 0){ goto _LL1447;} else{ _temp1470=* _temp1468; _LL1480:
_temp1479= _temp1470.name; goto _LL1478; _LL1478: _temp1477= _temp1470.tq; goto
_LL1476; _LL1476: _temp1475=( void*) _temp1470.type; goto _LL1474; _LL1474:
_temp1473=( void*) _temp1470.rgn; goto _LL1472; _LL1472: _temp1471= _temp1470.inject;
goto _LL1455;} _LL1449: goto _LL1447; _LL1451: goto _LL1453; _LL1453:
bad_cyc_vararg= 1; goto _LL1447; _LL1455: Cyc_Tcutil_unify_tqual( _temp1490,
_temp1477); Cyc_Tcutil_unify_it( _temp1488, _temp1475); Cyc_Tcutil_unify_it(
_temp1486, _temp1473); if( _temp1484 != _temp1471){ bad_cyc_vararg= 1;} goto
_LL1447; _LL1447:;} if( bad_cyc_vararg){ goto _LL1082;}{ int bad_effect= 0;{
struct _tuple12 _temp1495=({ struct _tuple12 _temp1494; _temp1494.f1= _temp1327;
_temp1494.f2= _temp1347; _temp1494;}); struct Cyc_Core_Opt* _temp1505; struct
Cyc_Core_Opt* _temp1507; struct Cyc_Core_Opt* _temp1509; struct Cyc_Core_Opt*
_temp1511; struct Cyc_Core_Opt* _temp1513; struct Cyc_Core_Opt* _temp1515;
_LL1497: _LL1508: _temp1507= _temp1495.f1; if( _temp1507 == 0){ goto _LL1506;}
else{ goto _LL1499;} _LL1506: _temp1505= _temp1495.f2; if( _temp1505 == 0){ goto
_LL1498;} else{ goto _LL1499;} _LL1499: _LL1512: _temp1511= _temp1495.f1; if(
_temp1511 == 0){ goto _LL1510;} else{ goto _LL1501;} _LL1510: _temp1509=
_temp1495.f2; goto _LL1500; _LL1501: _LL1516: _temp1515= _temp1495.f1; goto
_LL1514; _LL1514: _temp1513= _temp1495.f2; if( _temp1513 == 0){ goto _LL1502;}
else{ goto _LL1503;} _LL1503: goto _LL1504; _LL1498: goto _LL1496; _LL1500: goto
_LL1502; _LL1502: bad_effect= 1; goto _LL1496; _LL1504: bad_effect= ! Cyc_Tcutil_unify_effect((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1327))->v,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1347))->v); goto _LL1496; _LL1496:;} if( bad_effect){ goto
_LL1082;} if( ! Cyc_Tcutil_same_atts( _temp1335, _temp1315)){ goto _LL1082;} if(
! Cyc_Tcutil_same_rgn_po( _temp1337, _temp1317)){ goto _LL1082;} return;}}}
_LL1112: for( 0; _temp1353 != 0? _temp1357 != 0: 0;( _temp1353=(( struct Cyc_List_List*)
_check_null( _temp1353))->tl, _temp1357=(( struct Cyc_List_List*) _check_null(
_temp1357))->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1353))->hd)).f1,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1357))->hd)).f1); Cyc_Tcutil_unify_it((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1353))->hd)).f2,(*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1357))->hd)).f2);} if( _temp1353 == 0?
_temp1357 == 0: 0){ return;} goto _LL1082; _LL1114: _temp1373= _temp1365;
_temp1369= _temp1361; goto _LL1116; _LL1116: for( 0; _temp1369 != 0? _temp1373
!= 0: 0;( _temp1369=(( struct Cyc_List_List*) _check_null( _temp1369))->tl,
_temp1373=(( struct Cyc_List_List*) _check_null( _temp1373))->tl)){ struct Cyc_Absyn_Structfield*
_temp1517=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1369))->hd; goto _LL1518; _LL1518: { struct Cyc_Absyn_Structfield*
_temp1519=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1373))->hd; goto _LL1520; _LL1520: if( Cyc_String_zstrptrcmp( _temp1517->name,
_temp1519->name) != 0){( void) _throw(( void*) Cyc_Tcutil_Unify);} Cyc_Tcutil_unify_tqual(
_temp1517->tq, _temp1519->tq); Cyc_Tcutil_unify_it(( void*) _temp1517->type,(
void*) _temp1519->type); if((( ! Cyc_Tcutil_same_atts( _temp1517->attributes,
_temp1519->attributes)? 1:( _temp1517->width != 0? _temp1519->width == 0: 0))? 1:(
_temp1519->width != 0? _temp1517->width == 0: 0))? 1:(( _temp1517->width != 0?
_temp1519->width != 0: 0)? Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp1517->width))->v) != Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp1519->width))->v):
0)){( void) _throw(( void*) Cyc_Tcutil_Unify);}}} if( _temp1369 == 0? _temp1373
== 0: 0){ return;} goto _LL1082; _LL1118: return; _LL1120: Cyc_Tcutil_unify_it(
_temp1385, _temp1381); return; _LL1122: goto _LL1124; _LL1124: goto _LL1126;
_LL1126: goto _LL1128; _LL1128: if( Cyc_Tcutil_unify_effect( t1, t2)){ return;}
else{ goto _LL1082;} _LL1130: goto _LL1082; _LL1082:;}( void) _throw(( void*)
Cyc_Tcutil_Unify);} int Cyc_Tcutil_is_arithmetic_type( void* t){ void* _temp1521=
Cyc_Tcutil_compress( t); void* _temp1533; void* _temp1535; struct Cyc_Absyn_Enumdecl*
_temp1537; struct _tuple1* _temp1539; _LL1523: if(( unsigned int) _temp1521 > 4u?*((
int*) _temp1521) == Cyc_Absyn_IntType: 0){ _LL1536: _temp1535=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1521)->f1; goto _LL1534; _LL1534: _temp1533=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1521)->f2; goto _LL1524;} else{
goto _LL1525;} _LL1525: if( _temp1521 ==( void*) Cyc_Absyn_FloatType){ goto
_LL1526;} else{ goto _LL1527;} _LL1527: if( _temp1521 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1528;} else{ goto _LL1529;} _LL1529: if(( unsigned int) _temp1521 > 4u?*((
int*) _temp1521) == Cyc_Absyn_EnumType: 0){ _LL1540: _temp1539=(( struct Cyc_Absyn_EnumType_struct*)
_temp1521)->f1; goto _LL1538; _LL1538: _temp1537=(( struct Cyc_Absyn_EnumType_struct*)
_temp1521)->f2; goto _LL1530;} else{ goto _LL1531;} _LL1531: goto _LL1532;
_LL1524: return 1; _LL1526: return 1; _LL1528: return 1; _LL1530: return 1;
_LL1532: return 0; _LL1522:;} int Cyc_Tcutil_will_lose_precision( void* t1, void*
t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8
_temp1542=({ struct _tuple8 _temp1541; _temp1541.f1= t1; _temp1541.f2= t2;
_temp1541;}); void* _temp1564; void* _temp1566; void* _temp1568; void* _temp1570;
void* _temp1572; void* _temp1574; void* _temp1576; void* _temp1578; void*
_temp1580; void* _temp1582; void* _temp1584; void* _temp1586; void* _temp1588;
void* _temp1590; void* _temp1592; void* _temp1594; void* _temp1596; void*
_temp1598; void* _temp1600; void* _temp1602; void* _temp1604; void* _temp1606;
void* _temp1608; void* _temp1610; void* _temp1612; void* _temp1614; void*
_temp1616; void* _temp1618; void* _temp1620; void* _temp1622; void* _temp1624;
void* _temp1626; void* _temp1628; void* _temp1630; void* _temp1632; void*
_temp1634; void* _temp1636; void* _temp1638; void* _temp1640; void* _temp1642;
void* _temp1644; void* _temp1646; _LL1544: _LL1567: _temp1566= _temp1542.f1; if(
_temp1566 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1565;} else{ goto _LL1546;}
_LL1565: _temp1564= _temp1542.f2; if( _temp1564 ==( void*) Cyc_Absyn_FloatType){
goto _LL1545;} else{ goto _LL1546;} _LL1546: _LL1575: _temp1574= _temp1542.f1;
if( _temp1574 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1569;} else{ goto
_LL1548;} _LL1569: _temp1568= _temp1542.f2; if(( unsigned int) _temp1568 > 4u?*((
int*) _temp1568) == Cyc_Absyn_IntType: 0){ _LL1573: _temp1572=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1568)->f1; goto _LL1571; _LL1571: _temp1570=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1568)->f2; goto _LL1547;} else{
goto _LL1548;} _LL1548: _LL1583: _temp1582= _temp1542.f1; if( _temp1582 ==( void*)
Cyc_Absyn_FloatType){ goto _LL1577;} else{ goto _LL1550;} _LL1577: _temp1576=
_temp1542.f2; if(( unsigned int) _temp1576 > 4u?*(( int*) _temp1576) == Cyc_Absyn_IntType:
0){ _LL1581: _temp1580=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1576)->f1;
goto _LL1579; _LL1579: _temp1578=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1576)->f2; goto _LL1549;} else{ goto _LL1550;} _LL1550: _LL1591: _temp1590=
_temp1542.f1; if(( unsigned int) _temp1590 > 4u?*(( int*) _temp1590) == Cyc_Absyn_IntType:
0){ _LL1595: _temp1594=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1590)->f1;
goto _LL1593; _LL1593: _temp1592=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1590)->f2; if( _temp1592 ==( void*) Cyc_Absyn_B8){ goto _LL1585;} else{
goto _LL1552;}} else{ goto _LL1552;} _LL1585: _temp1584= _temp1542.f2; if((
unsigned int) _temp1584 > 4u?*(( int*) _temp1584) == Cyc_Absyn_IntType: 0){
_LL1589: _temp1588=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1584)->f1;
goto _LL1587; _LL1587: _temp1586=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1584)->f2; if( _temp1586 ==( void*) Cyc_Absyn_B8){ goto _LL1551;} else{
goto _LL1552;}} else{ goto _LL1552;} _LL1552: _LL1599: _temp1598= _temp1542.f1;
if(( unsigned int) _temp1598 > 4u?*(( int*) _temp1598) == Cyc_Absyn_IntType: 0){
_LL1603: _temp1602=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1598)->f1;
goto _LL1601; _LL1601: _temp1600=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1598)->f2; if( _temp1600 ==( void*) Cyc_Absyn_B8){ goto _LL1597;} else{
goto _LL1554;}} else{ goto _LL1554;} _LL1597: _temp1596= _temp1542.f2; goto
_LL1553; _LL1554: _LL1607: _temp1606= _temp1542.f1; if(( unsigned int) _temp1606
> 4u?*(( int*) _temp1606) == Cyc_Absyn_IntType: 0){ _LL1611: _temp1610=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1606)->f1; goto _LL1609; _LL1609:
_temp1608=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1606)->f2; if(
_temp1608 ==( void*) Cyc_Absyn_B4){ goto _LL1605;} else{ goto _LL1556;}} else{
goto _LL1556;} _LL1605: _temp1604= _temp1542.f2; if( _temp1604 ==( void*) Cyc_Absyn_FloatType){
goto _LL1555;} else{ goto _LL1556;} _LL1556: _LL1619: _temp1618= _temp1542.f1;
if(( unsigned int) _temp1618 > 4u?*(( int*) _temp1618) == Cyc_Absyn_IntType: 0){
_LL1623: _temp1622=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1618)->f1;
goto _LL1621; _LL1621: _temp1620=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1618)->f2; if( _temp1620 ==( void*) Cyc_Absyn_B4){ goto _LL1613;} else{
goto _LL1558;}} else{ goto _LL1558;} _LL1613: _temp1612= _temp1542.f2; if((
unsigned int) _temp1612 > 4u?*(( int*) _temp1612) == Cyc_Absyn_IntType: 0){
_LL1617: _temp1616=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1612)->f1;
goto _LL1615; _LL1615: _temp1614=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1612)->f2; if( _temp1614 ==( void*) Cyc_Absyn_B2){ goto _LL1557;} else{
goto _LL1558;}} else{ goto _LL1558;} _LL1558: _LL1631: _temp1630= _temp1542.f1;
if(( unsigned int) _temp1630 > 4u?*(( int*) _temp1630) == Cyc_Absyn_IntType: 0){
_LL1635: _temp1634=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1630)->f1;
goto _LL1633; _LL1633: _temp1632=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1630)->f2; if( _temp1632 ==( void*) Cyc_Absyn_B4){ goto _LL1625;} else{
goto _LL1560;}} else{ goto _LL1560;} _LL1625: _temp1624= _temp1542.f2; if((
unsigned int) _temp1624 > 4u?*(( int*) _temp1624) == Cyc_Absyn_IntType: 0){
_LL1629: _temp1628=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1624)->f1;
goto _LL1627; _LL1627: _temp1626=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1624)->f2; if( _temp1626 ==( void*) Cyc_Absyn_B1){ goto _LL1559;} else{
goto _LL1560;}} else{ goto _LL1560;} _LL1560: _LL1643: _temp1642= _temp1542.f1;
if(( unsigned int) _temp1642 > 4u?*(( int*) _temp1642) == Cyc_Absyn_IntType: 0){
_LL1647: _temp1646=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1642)->f1;
goto _LL1645; _LL1645: _temp1644=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1642)->f2; if( _temp1644 ==( void*) Cyc_Absyn_B2){ goto _LL1637;} else{
goto _LL1562;}} else{ goto _LL1562;} _LL1637: _temp1636= _temp1542.f2; if((
unsigned int) _temp1636 > 4u?*(( int*) _temp1636) == Cyc_Absyn_IntType: 0){
_LL1641: _temp1640=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1636)->f1;
goto _LL1639; _LL1639: _temp1638=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1636)->f2; if( _temp1638 ==( void*) Cyc_Absyn_B1){ goto _LL1561;} else{
goto _LL1562;}} else{ goto _LL1562;} _LL1562: goto _LL1563; _LL1545: return 1;
_LL1547: return 1; _LL1549: return 1; _LL1551: return 0; _LL1553: return 1;
_LL1555: return 1; _LL1557: return 1; _LL1559: return 1; _LL1561: return 1;
_LL1563: return 0; _LL1543:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{
struct Cyc_List_List* el= es; for( 0; el != 0; el=(( struct Cyc_List_List*)
_check_null( el))->tl){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);
if( Cyc_Tcutil_is_arithmetic_type( t1)){ if( max_arith_type == 0? 1: Cyc_Tcutil_will_lose_precision(
t1,( void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){
max_arith_type=({ struct Cyc_Core_Opt* _temp1648=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1648->v=( void*) t1; _temp1648;});}}}}
if( max_arith_type != 0){ if( ! Cyc_Tcutil_unify( t,( void*)(( struct Cyc_Core_Opt*)
_check_null( max_arith_type))->v)){ return 0;}}{ struct Cyc_List_List* el= es;
for( 0; el != 0; el=(( struct Cyc_List_List*) _check_null( el))->tl){ if( ! Cyc_Tcutil_coerce_assign(
te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd, t)){
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
el))->hd)->loc,( struct _tagged_string)({ struct _tagged_string _temp1649= Cyc_Absynpp_typ2string(
t); struct _tagged_string _temp1650= Cyc_Absynpp_typ2string(( void*)(( struct
Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( el))->hd)->topt))->v); xprintf("type mismatch: expecting %.*s but found %.*s",
_temp1649.last_plus_one - _temp1649.curr, _temp1649.curr, _temp1650.last_plus_one
- _temp1650.curr, _temp1650.curr);})); return 0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e)){ void* _temp1651= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); struct Cyc_Absyn_PtrInfo _temp1657; _LL1653: if((
unsigned int) _temp1651 > 4u?*(( int*) _temp1651) == Cyc_Absyn_PointerType: 0){
_LL1658: _temp1657=(( struct Cyc_Absyn_PointerType_struct*) _temp1651)->f1; goto
_LL1654;} else{ goto _LL1655;} _LL1655: goto _LL1656; _LL1654: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); goto _LL1652; _LL1656: return 0; _LL1652:;} return 1;}
int Cyc_Tcutil_is_integral_type( void* t){ void* _temp1659= Cyc_Tcutil_compress(
t); void* _temp1667; void* _temp1669; struct Cyc_Absyn_Enumdecl* _temp1671;
struct _tuple1* _temp1673; _LL1661: if(( unsigned int) _temp1659 > 4u?*(( int*)
_temp1659) == Cyc_Absyn_IntType: 0){ _LL1670: _temp1669=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1659)->f1; goto _LL1668; _LL1668: _temp1667=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1659)->f2; goto _LL1662;} else{ goto _LL1663;} _LL1663: if(( unsigned int)
_temp1659 > 4u?*(( int*) _temp1659) == Cyc_Absyn_EnumType: 0){ _LL1674:
_temp1673=(( struct Cyc_Absyn_EnumType_struct*) _temp1659)->f1; goto _LL1672;
_LL1672: _temp1671=(( struct Cyc_Absyn_EnumType_struct*) _temp1659)->f2; goto
_LL1664;} else{ goto _LL1665;} _LL1665: goto _LL1666; _LL1662: return 1; _LL1664:
return 1; _LL1666: return 0; _LL1660:;} int Cyc_Tcutil_coerce_uint_typ( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_uint_t)){ return 1;}
if( Cyc_Tcutil_is_integral_type(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){
if( Cyc_Tcutil_will_lose_precision(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, Cyc_Absyn_uint_t)){ Cyc_Tcutil_warn( e->loc,({ unsigned char*
_temp1675=( unsigned char*)"integral size mismatch; conversion supplied"; struct
_tagged_string _temp1676; _temp1676.curr= _temp1675; _temp1676.base= _temp1675;
_temp1676.last_plus_one= _temp1675 + 44; _temp1676;}));} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;}
if( Cyc_Tcutil_is_integral_type(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){
if( Cyc_Tcutil_will_lose_precision(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, Cyc_Absyn_sint_t)){ Cyc_Tcutil_warn( e->loc,({ unsigned char*
_temp1677=( unsigned char*)"integral size mismatch; conversion supplied"; struct
_tagged_string _temp1678; _temp1678.curr= _temp1677; _temp1678.base= _temp1677;
_temp1678.last_plus_one= _temp1677 + 44; _temp1678;}));} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); return 1;} return 0;} int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8
_temp1680=({ struct _tuple8 _temp1679; _temp1679.f1= t1; _temp1679.f2= t2;
_temp1679;}); void* _temp1692; struct Cyc_Absyn_PtrInfo _temp1694; void*
_temp1696; struct Cyc_Absyn_PtrInfo _temp1698; void* _temp1700; struct Cyc_Absyn_Exp*
_temp1702; struct Cyc_Absyn_Tqual _temp1704; void* _temp1706; void* _temp1708;
struct Cyc_Absyn_Exp* _temp1710; struct Cyc_Absyn_Tqual _temp1712; void*
_temp1714; void* _temp1716; struct Cyc_Absyn_TunionInfo _temp1718; void*
_temp1720; struct Cyc_List_List* _temp1722; void* _temp1724; struct Cyc_Absyn_Tuniondecl*
_temp1726; void* _temp1728; struct Cyc_Absyn_TunionFieldInfo _temp1730; struct
Cyc_List_List* _temp1732; void* _temp1734; struct Cyc_Absyn_Tunionfield*
_temp1736; struct Cyc_Absyn_Tuniondecl* _temp1738; void* _temp1740; struct Cyc_Absyn_TunionInfo
_temp1742; void* _temp1744; struct Cyc_List_List* _temp1746; void* _temp1748;
struct Cyc_Absyn_Tuniondecl* _temp1750; void* _temp1752; struct Cyc_Absyn_PtrInfo
_temp1754; struct Cyc_Absyn_Conref* _temp1756; struct Cyc_Absyn_Tqual _temp1758;
struct Cyc_Absyn_Conref* _temp1760; void* _temp1762; void* _temp1764; _LL1682:
_LL1697: _temp1696= _temp1680.f1; if(( unsigned int) _temp1696 > 4u?*(( int*)
_temp1696) == Cyc_Absyn_PointerType: 0){ _LL1699: _temp1698=(( struct Cyc_Absyn_PointerType_struct*)
_temp1696)->f1; goto _LL1693;} else{ goto _LL1684;} _LL1693: _temp1692=
_temp1680.f2; if(( unsigned int) _temp1692 > 4u?*(( int*) _temp1692) == Cyc_Absyn_PointerType:
0){ _LL1695: _temp1694=(( struct Cyc_Absyn_PointerType_struct*) _temp1692)->f1;
goto _LL1683;} else{ goto _LL1684;} _LL1684: _LL1709: _temp1708= _temp1680.f1;
if(( unsigned int) _temp1708 > 4u?*(( int*) _temp1708) == Cyc_Absyn_ArrayType: 0){
_LL1715: _temp1714=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1708)->f1;
goto _LL1713; _LL1713: _temp1712=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1708)->f2; goto _LL1711; _LL1711: _temp1710=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1708)->f3; goto _LL1701;} else{ goto _LL1686;} _LL1701: _temp1700=
_temp1680.f2; if(( unsigned int) _temp1700 > 4u?*(( int*) _temp1700) == Cyc_Absyn_ArrayType:
0){ _LL1707: _temp1706=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1700)->f1;
goto _LL1705; _LL1705: _temp1704=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1700)->f2; goto _LL1703; _LL1703: _temp1702=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1700)->f3; goto _LL1685;} else{ goto _LL1686;} _LL1686: _LL1729: _temp1728=
_temp1680.f1; if(( unsigned int) _temp1728 > 4u?*(( int*) _temp1728) == Cyc_Absyn_TunionFieldType:
0){ _LL1731: _temp1730=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1728)->f1;
_LL1735: _temp1734=( void*) _temp1730.field_info; if(*(( int*) _temp1734) == Cyc_Absyn_KnownTunionfield){
_LL1739: _temp1738=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1734)->f1;
goto _LL1737; _LL1737: _temp1736=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1734)->f2; goto _LL1733;} else{ goto _LL1688;} _LL1733: _temp1732=
_temp1730.targs; goto _LL1717;} else{ goto _LL1688;} _LL1717: _temp1716=
_temp1680.f2; if(( unsigned int) _temp1716 > 4u?*(( int*) _temp1716) == Cyc_Absyn_TunionType:
0){ _LL1719: _temp1718=(( struct Cyc_Absyn_TunionType_struct*) _temp1716)->f1;
_LL1725: _temp1724=( void*) _temp1718.tunion_info; if(*(( int*) _temp1724) ==
Cyc_Absyn_KnownTunion){ _LL1727: _temp1726=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1724)->f1; goto _LL1723;} else{ goto _LL1688;} _LL1723: _temp1722=
_temp1718.targs; goto _LL1721; _LL1721: _temp1720=( void*) _temp1718.rgn; goto
_LL1687;} else{ goto _LL1688;} _LL1688: _LL1753: _temp1752= _temp1680.f1; if((
unsigned int) _temp1752 > 4u?*(( int*) _temp1752) == Cyc_Absyn_PointerType: 0){
_LL1755: _temp1754=(( struct Cyc_Absyn_PointerType_struct*) _temp1752)->f1;
_LL1765: _temp1764=( void*) _temp1754.elt_typ; goto _LL1763; _LL1763: _temp1762=(
void*) _temp1754.rgn_typ; goto _LL1761; _LL1761: _temp1760= _temp1754.nullable;
goto _LL1759; _LL1759: _temp1758= _temp1754.tq; goto _LL1757; _LL1757: _temp1756=
_temp1754.bounds; goto _LL1741;} else{ goto _LL1690;} _LL1741: _temp1740=
_temp1680.f2; if(( unsigned int) _temp1740 > 4u?*(( int*) _temp1740) == Cyc_Absyn_TunionType:
0){ _LL1743: _temp1742=(( struct Cyc_Absyn_TunionType_struct*) _temp1740)->f1;
_LL1749: _temp1748=( void*) _temp1742.tunion_info; if(*(( int*) _temp1748) ==
Cyc_Absyn_KnownTunion){ _LL1751: _temp1750=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1748)->f1; goto _LL1747;} else{ goto _LL1690;} _LL1747: _temp1746=
_temp1742.targs; goto _LL1745; _LL1745: _temp1744=( void*) _temp1742.rgn; goto
_LL1689;} else{ goto _LL1690;} _LL1690: goto _LL1691; _LL1683: { int okay= 1;
if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1698.nullable, _temp1694.nullable)){
void* _temp1766=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp1698.nullable))->v; int _temp1772; _LL1768:
if(( unsigned int) _temp1766 > 1u?*(( int*) _temp1766) == Cyc_Absyn_Eq_constr: 0){
_LL1773: _temp1772=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1766)->f1;
goto _LL1769;} else{ goto _LL1770;} _LL1770: goto _LL1771; _LL1769: okay= !
_temp1772; goto _LL1767; _LL1771:( void) _throw((( void*(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp1774=(
unsigned char*)"silent_castable conref not eq"; struct _tagged_string _temp1775;
_temp1775.curr= _temp1774; _temp1775.base= _temp1774; _temp1775.last_plus_one=
_temp1774 + 30; _temp1775;}))); _LL1767:;} if( !(( int(*)( int(* cmp)( void*,
void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp1698.bounds, _temp1694.bounds)){ struct _tuple8
_temp1777=({ struct _tuple8 _temp1776; _temp1776.f1=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1698.bounds))->v;
_temp1776.f2=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp1694.bounds))->v; _temp1776;}); void* _temp1787;
void* _temp1789; void* _temp1791; void* _temp1793; struct Cyc_Absyn_Exp*
_temp1795; void* _temp1797; void* _temp1799; struct Cyc_Absyn_Exp* _temp1801;
void* _temp1803; void* _temp1805; struct Cyc_Absyn_Exp* _temp1807; void*
_temp1809; void* _temp1811; void* _temp1813; void* _temp1815; _LL1779: _LL1792:
_temp1791= _temp1777.f1; if(( unsigned int) _temp1791 > 1u?*(( int*) _temp1791)
== Cyc_Absyn_Eq_constr: 0){ _LL1794: _temp1793=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1791)->f1; if(( unsigned int) _temp1793 > 1u?*(( int*) _temp1793) == Cyc_Absyn_Upper_b:
0){ _LL1796: _temp1795=(( struct Cyc_Absyn_Upper_b_struct*) _temp1793)->f1; goto
_LL1788;} else{ goto _LL1781;}} else{ goto _LL1781;} _LL1788: _temp1787=
_temp1777.f2; if(( unsigned int) _temp1787 > 1u?*(( int*) _temp1787) == Cyc_Absyn_Eq_constr:
0){ _LL1790: _temp1789=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1787)->f1;
if( _temp1789 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1780;} else{ goto _LL1781;}}
else{ goto _LL1781;} _LL1781: _LL1804: _temp1803= _temp1777.f1; if((
unsigned int) _temp1803 > 1u?*(( int*) _temp1803) == Cyc_Absyn_Eq_constr: 0){
_LL1806: _temp1805=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1803)->f1;
if(( unsigned int) _temp1805 > 1u?*(( int*) _temp1805) == Cyc_Absyn_Upper_b: 0){
_LL1808: _temp1807=(( struct Cyc_Absyn_Upper_b_struct*) _temp1805)->f1; goto
_LL1798;} else{ goto _LL1783;}} else{ goto _LL1783;} _LL1798: _temp1797=
_temp1777.f2; if(( unsigned int) _temp1797 > 1u?*(( int*) _temp1797) == Cyc_Absyn_Eq_constr:
0){ _LL1800: _temp1799=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1797)->f1;
if(( unsigned int) _temp1799 > 1u?*(( int*) _temp1799) == Cyc_Absyn_Upper_b: 0){
_LL1802: _temp1801=(( struct Cyc_Absyn_Upper_b_struct*) _temp1799)->f1; goto
_LL1782;} else{ goto _LL1783;}} else{ goto _LL1783;} _LL1783: _LL1814: _temp1813=
_temp1777.f1; if(( unsigned int) _temp1813 > 1u?*(( int*) _temp1813) == Cyc_Absyn_Eq_constr:
0){ _LL1816: _temp1815=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1813)->f1;
if( _temp1815 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1810;} else{ goto _LL1785;}}
else{ goto _LL1785;} _LL1810: _temp1809= _temp1777.f2; if(( unsigned int)
_temp1809 > 1u?*(( int*) _temp1809) == Cyc_Absyn_Eq_constr: 0){ _LL1812:
_temp1811=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1809)->f1; if(
_temp1811 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1784;} else{ goto _LL1785;}}
else{ goto _LL1785;} _LL1785: goto _LL1786; _LL1780: okay= 1; goto _LL1778;
_LL1782: okay= okay? Cyc_Evexp_eval_const_uint_exp( _temp1807) >= Cyc_Evexp_eval_const_uint_exp(
_temp1801): 0; Cyc_Tcutil_warn( loc,({ unsigned char* _temp1817=( unsigned char*)"implicit cast to shorter array";
struct _tagged_string _temp1818; _temp1818.curr= _temp1817; _temp1818.base=
_temp1817; _temp1818.last_plus_one= _temp1817 + 31; _temp1818;})); goto _LL1778;
_LL1784: okay= 1; goto _LL1778; _LL1786: okay= 0; goto _LL1778; _LL1778:;} okay=
okay? Cyc_Tcutil_unify(( void*) _temp1698.elt_typ,( void*) _temp1694.elt_typ): 0;
okay= okay? Cyc_Tcutil_unify(( void*) _temp1698.rgn_typ,( void*) _temp1694.rgn_typ):
0; okay= okay? !( _temp1698.tq).q_const? 1:( _temp1694.tq).q_const: 0; return
okay;} _LL1685: { int okay; okay=( _temp1710 != 0? _temp1702 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1710)) == Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1702)): 0; return( okay? Cyc_Tcutil_unify(
_temp1714, _temp1706): 0)? ! _temp1712.q_const? 1: _temp1704.q_const: 0;}
_LL1687: if(( _temp1738 == _temp1726? 1: Cyc_Absyn_qvar_cmp( _temp1738->name,
_temp1726->name) == 0)? _temp1736->typs == 0: 0){ for( 0; _temp1732 != 0?
_temp1722 != 0: 0;( _temp1732=(( struct Cyc_List_List*) _check_null( _temp1732))->tl,
_temp1722=(( struct Cyc_List_List*) _check_null( _temp1722))->tl)){ if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_List_List*) _check_null( _temp1732))->hd,( void*)(( struct
Cyc_List_List*) _check_null( _temp1722))->hd)){ break;}} if( _temp1732 == 0?
_temp1722 == 0: 0){ return 1;}} return 0; _LL1689:{ void* _temp1819= Cyc_Tcutil_compress(
_temp1764); struct Cyc_Absyn_TunionFieldInfo _temp1825; struct Cyc_List_List*
_temp1827; void* _temp1829; struct Cyc_Absyn_Tunionfield* _temp1831; struct Cyc_Absyn_Tuniondecl*
_temp1833; _LL1821: if(( unsigned int) _temp1819 > 4u?*(( int*) _temp1819) ==
Cyc_Absyn_TunionFieldType: 0){ _LL1826: _temp1825=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1819)->f1; _LL1830: _temp1829=( void*) _temp1825.field_info; if(*(( int*)
_temp1829) == Cyc_Absyn_KnownTunionfield){ _LL1834: _temp1833=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1829)->f1; goto _LL1832; _LL1832: _temp1831=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1829)->f2; goto _LL1828;} else{ goto _LL1823;} _LL1828: _temp1827=
_temp1825.targs; goto _LL1822;} else{ goto _LL1823;} _LL1823: goto _LL1824;
_LL1822: if( ! Cyc_Tcutil_unify( _temp1762, _temp1744)? _temp1762 !=( void*) Cyc_Absyn_HeapRgn:
0){ return 0;} if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1760,(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)( 0))){
return 0;} if( !(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp,
_temp1756,(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)( Cyc_Absyn_bounds_one))){
return 0;} if( Cyc_Absyn_qvar_cmp( _temp1750->name, _temp1833->name) == 0?
_temp1831->typs != 0: 0){ int okay= 1; for( 0; _temp1827 != 0? _temp1746 != 0: 0;(
_temp1827=(( struct Cyc_List_List*) _check_null( _temp1827))->tl, _temp1746=((
struct Cyc_List_List*) _check_null( _temp1746))->tl)){ if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_List_List*) _check_null( _temp1827))->hd,( void*)(( struct
Cyc_List_List*) _check_null( _temp1746))->hd)){ okay= 0; break;}} if(( ! okay? 1:
_temp1827 != 0)? 1: _temp1746 != 0){ return 0;} return 1;} goto _LL1820; _LL1824:
goto _LL1820; _LL1820:;} return 0; _LL1691: return Cyc_Tcutil_unify( t1, t2);
_LL1681:;}} int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} if( Cyc_Tcutil_is_arithmetic_type(
t1)? Cyc_Tcutil_is_arithmetic_type( t2): 0){ if( Cyc_Tcutil_will_lose_precision(
t1, t2)){ Cyc_Tcutil_warn( e->loc,({ unsigned char* _temp1835=( unsigned char*)"integral size mismatch; conversion supplied";
struct _tagged_string _temp1836; _temp1836.curr= _temp1835; _temp1836.base=
_temp1835; _temp1836.last_plus_one= _temp1835 + 44; _temp1836;}));} Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){
Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2); Cyc_Tcutil_warn( e->loc,(
struct _tagged_string)({ struct _tagged_string _temp1837= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp1838= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1837.last_plus_one - _temp1837.curr, _temp1837.curr, _temp1838.last_plus_one
- _temp1838.curr, _temp1838.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg(
te, e, t);} static int Cyc_Tcutil_any_promote( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e){ void* old_typ= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); void* t;{ void* _temp1839= old_typ; struct Cyc_Absyn_Enumdecl*
_temp1851; struct _tuple1* _temp1853; void* _temp1855; void* _temp1857; _LL1841:
if(( unsigned int) _temp1839 > 4u?*(( int*) _temp1839) == Cyc_Absyn_EnumType: 0){
_LL1854: _temp1853=(( struct Cyc_Absyn_EnumType_struct*) _temp1839)->f1; goto
_LL1852; _LL1852: _temp1851=(( struct Cyc_Absyn_EnumType_struct*) _temp1839)->f2;
goto _LL1842;} else{ goto _LL1843;} _LL1843: if(( unsigned int) _temp1839 > 4u?*((
int*) _temp1839) == Cyc_Absyn_IntType: 0){ _LL1858: _temp1857=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1839)->f1; goto _LL1856; _LL1856: _temp1855=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1839)->f2; goto _LL1844;} else{
goto _LL1845;} _LL1845: if( _temp1839 ==( void*) Cyc_Absyn_FloatType){ goto
_LL1846;} else{ goto _LL1847;} _LL1847: if( _temp1839 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1848;} else{ goto _LL1849;} _LL1849: goto _LL1850; _LL1842: t= Cyc_Absyn_sint_t;
goto _LL1840; _LL1844:{ void* _temp1859= _temp1855; _LL1861: if( _temp1859 ==(
void*) Cyc_Absyn_B1){ goto _LL1862;} else{ goto _LL1863;} _LL1863: if( _temp1859
==( void*) Cyc_Absyn_B2){ goto _LL1864;} else{ goto _LL1865;} _LL1865: if(
_temp1859 ==( void*) Cyc_Absyn_B4){ goto _LL1866;} else{ goto _LL1867;} _LL1867:
if( _temp1859 ==( void*) Cyc_Absyn_B8){ goto _LL1868;} else{ goto _LL1860;}
_LL1862: t= Cyc_Absyn_sint_t; goto _LL1860; _LL1864: t= Cyc_Absyn_sint_t; goto
_LL1860; _LL1866: t= _temp1857 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL1860; _LL1868: t= _temp1857 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_ulong_t: Cyc_Absyn_slong_t; goto _LL1860; _LL1860:;} goto _LL1840;
_LL1846: t= Cyc_Absyn_float_t; goto _LL1840; _LL1848: t= Cyc_Absyn_double_t;
goto _LL1840; _LL1850: return 0; _LL1840:;} Cyc_Tcutil_unchecked_cast( te, e, t);
return 1;} int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} Cyc_Tcutil_any_promote(
te, e); if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v,
t2)){ return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); Cyc_Tcutil_warn( e->loc,( struct _tagged_string)({ struct
_tagged_string _temp1869= Cyc_Absynpp_typ2string( t1); struct _tagged_string
_temp1870= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1869.last_plus_one - _temp1869.curr, _temp1869.curr, _temp1870.last_plus_one
- _temp1870.curr, _temp1870.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerceable(
void* t){ void* _temp1871= Cyc_Tcutil_compress( t); void* _temp1881; void*
_temp1883; _LL1873: if(( unsigned int) _temp1871 > 4u?*(( int*) _temp1871) ==
Cyc_Absyn_IntType: 0){ _LL1884: _temp1883=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1871)->f1; goto _LL1882; _LL1882: _temp1881=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1871)->f2; goto _LL1874;} else{ goto _LL1875;} _LL1875: if( _temp1871 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1876;} else{ goto _LL1877;} _LL1877: if(
_temp1871 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1878;} else{ goto _LL1879;}
_LL1879: goto _LL1880; _LL1874: return 1; _LL1876: return 1; _LL1878: return 1;
_LL1880: return 0; _LL1872:;} static struct _tuple4* Cyc_Tcutil_flatten_typ_f(
struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield* x){ return({ struct
_tuple4* _temp1885=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1885->f1= x->tq; _temp1885->f2= Cyc_Tcutil_substitute( inst,( void*) x->type);
_temp1885;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ( struct Cyc_Tcenv_Tenv*
te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void* _temp1886= t1; struct Cyc_List_List*
_temp1898; struct Cyc_Absyn_Structdecl** _temp1900; struct Cyc_List_List*
_temp1902; struct _tuple1* _temp1904; struct Cyc_List_List* _temp1906; _LL1888:
if( _temp1886 ==( void*) Cyc_Absyn_VoidType){ goto _LL1889;} else{ goto _LL1890;}
_LL1890: if(( unsigned int) _temp1886 > 4u?*(( int*) _temp1886) == Cyc_Absyn_TupleType:
0){ _LL1899: _temp1898=(( struct Cyc_Absyn_TupleType_struct*) _temp1886)->f1;
goto _LL1891;} else{ goto _LL1892;} _LL1892: if(( unsigned int) _temp1886 > 4u?*((
int*) _temp1886) == Cyc_Absyn_StructType: 0){ _LL1905: _temp1904=(( struct Cyc_Absyn_StructType_struct*)
_temp1886)->f1; goto _LL1903; _LL1903: _temp1902=(( struct Cyc_Absyn_StructType_struct*)
_temp1886)->f2; goto _LL1901; _LL1901: _temp1900=(( struct Cyc_Absyn_StructType_struct*)
_temp1886)->f3; goto _LL1893;} else{ goto _LL1894;} _LL1894: if(( unsigned int)
_temp1886 > 4u?*(( int*) _temp1886) == Cyc_Absyn_AnonStructType: 0){ _LL1907:
_temp1906=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1886)->f1; goto
_LL1895;} else{ goto _LL1896;} _LL1896: goto _LL1897; _LL1889: return 0; _LL1891:
return _temp1898; _LL1893: if( _temp1900 == 0? 1: _temp1904 == 0){ return({
struct Cyc_List_List* _temp1908=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1908->hd=( void*)({ struct _tuple4* _temp1909=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1909->f1= Cyc_Absyn_empty_tqual();
_temp1909->f2= t1; _temp1909;}); _temp1908->tl= 0; _temp1908;});}{ struct Cyc_Absyn_Structdecl*
_temp1910=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp1900)); goto
_LL1911; _LL1911: { struct _tuple1 _temp1912=*(( struct _tuple1*) _check_null(
_temp1904)); goto _LL1913; _LL1913: { struct Cyc_List_List* _temp1914=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1910->tvs, _temp1902); goto _LL1915; _LL1915: if( _temp1910->fields == 0){
return({ struct Cyc_List_List* _temp1916=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1916->hd=( void*)({ struct _tuple4*
_temp1917=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1917->f1=
Cyc_Absyn_empty_tqual(); _temp1917->f2= t1; _temp1917;}); _temp1916->tl= 0;
_temp1916;});} return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, _temp1914,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1910->fields))->v);}}}
_LL1895: return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, 0, _temp1906); _LL1897: return({
struct Cyc_List_List* _temp1918=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1918->hd=( void*)({ struct _tuple4* _temp1919=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1919->f1= Cyc_Absyn_empty_tqual();
_temp1919->f2= t1; _temp1919;}); _temp1918->tl= 0; _temp1918;}); _LL1887:;}}
static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2); static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;}{ struct Cyc_List_List* a= assume; for( 0; a != 0; a=(( struct
Cyc_List_List*) _check_null( a))->tl){ if( Cyc_Tcutil_unify( t1,(*(( struct
_tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f1)? Cyc_Tcutil_unify(
t2,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f2): 0){
return 1;}}} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct
_tuple8 _temp1921=({ struct _tuple8 _temp1920; _temp1920.f1= t1; _temp1920.f2=
t2; _temp1920;}); void* _temp1927; struct Cyc_Absyn_PtrInfo _temp1929; struct
Cyc_Absyn_Conref* _temp1931; struct Cyc_Absyn_Tqual _temp1933; struct Cyc_Absyn_Conref*
_temp1935; void* _temp1937; void* _temp1939; void* _temp1941; struct Cyc_Absyn_PtrInfo
_temp1943; struct Cyc_Absyn_Conref* _temp1945; struct Cyc_Absyn_Tqual _temp1947;
struct Cyc_Absyn_Conref* _temp1949; void* _temp1951; void* _temp1953; _LL1923:
_LL1942: _temp1941= _temp1921.f1; if(( unsigned int) _temp1941 > 4u?*(( int*)
_temp1941) == Cyc_Absyn_PointerType: 0){ _LL1944: _temp1943=(( struct Cyc_Absyn_PointerType_struct*)
_temp1941)->f1; _LL1954: _temp1953=( void*) _temp1943.elt_typ; goto _LL1952;
_LL1952: _temp1951=( void*) _temp1943.rgn_typ; goto _LL1950; _LL1950: _temp1949=
_temp1943.nullable; goto _LL1948; _LL1948: _temp1947= _temp1943.tq; goto _LL1946;
_LL1946: _temp1945= _temp1943.bounds; goto _LL1928;} else{ goto _LL1925;}
_LL1928: _temp1927= _temp1921.f2; if(( unsigned int) _temp1927 > 4u?*(( int*)
_temp1927) == Cyc_Absyn_PointerType: 0){ _LL1930: _temp1929=(( struct Cyc_Absyn_PointerType_struct*)
_temp1927)->f1; _LL1940: _temp1939=( void*) _temp1929.elt_typ; goto _LL1938;
_LL1938: _temp1937=( void*) _temp1929.rgn_typ; goto _LL1936; _LL1936: _temp1935=
_temp1929.nullable; goto _LL1934; _LL1934: _temp1933= _temp1929.tq; goto _LL1932;
_LL1932: _temp1931= _temp1929.bounds; goto _LL1924;} else{ goto _LL1925;}
_LL1925: goto _LL1926; _LL1924: if( _temp1947.q_const? ! _temp1933.q_const: 0){
return 0;} if(( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1949, _temp1935)?(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1949): 0)? !(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1935): 0){ return 0;}{ void* _temp1955= _temp1951; _LL1957: if( _temp1955
==( void*) Cyc_Absyn_HeapRgn){ goto _LL1958;} else{ goto _LL1959;} _LL1959: goto
_LL1960; _LL1958: goto _LL1956; _LL1960: Cyc_Tcutil_unify( _temp1951, _temp1937);
goto _LL1956; _LL1956:;} if( !(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp,
_temp1945, _temp1931)){ struct _tuple8 _temp1962=({ struct _tuple8 _temp1961;
_temp1961.f1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1945); _temp1961.f2=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1931); _temp1961;}); void* _temp1970; void* _temp1972; struct Cyc_Absyn_Exp*
_temp1974; void* _temp1976; struct Cyc_Absyn_Exp* _temp1978; void* _temp1980;
struct Cyc_Absyn_Exp* _temp1982; _LL1964: _LL1973: _temp1972= _temp1962.f1; if((
unsigned int) _temp1972 > 1u?*(( int*) _temp1972) == Cyc_Absyn_Upper_b: 0){
_LL1975: _temp1974=(( struct Cyc_Absyn_Upper_b_struct*) _temp1972)->f1; goto
_LL1971;} else{ goto _LL1966;} _LL1971: _temp1970= _temp1962.f2; if( _temp1970
==( void*) Cyc_Absyn_Unknown_b){ goto _LL1965;} else{ goto _LL1966;} _LL1966:
_LL1981: _temp1980= _temp1962.f1; if(( unsigned int) _temp1980 > 1u?*(( int*)
_temp1980) == Cyc_Absyn_Upper_b: 0){ _LL1983: _temp1982=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1980)->f1; goto _LL1977;} else{ goto _LL1968;} _LL1977: _temp1976=
_temp1962.f2; if(( unsigned int) _temp1976 > 1u?*(( int*) _temp1976) == Cyc_Absyn_Upper_b:
0){ _LL1979: _temp1978=(( struct Cyc_Absyn_Upper_b_struct*) _temp1976)->f1; goto
_LL1967;} else{ goto _LL1968;} _LL1968: goto _LL1969; _LL1965: goto _LL1963;
_LL1967: if( Cyc_Evexp_eval_const_uint_exp( _temp1982) < Cyc_Evexp_eval_const_uint_exp(
_temp1978)){ return 0;} goto _LL1963; _LL1969: return 0; _LL1963:;} return Cyc_Tcutil_ptrsubtype(
te,({ struct Cyc_List_List* _temp1984=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1984->hd=( void*)({ struct _tuple8*
_temp1985=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp1985->f1=
t1; _temp1985->f2= t2; _temp1985;}); _temp1984->tl= assume; _temp1984;}),
_temp1953, _temp1939); _LL1926: return 0; _LL1922:;}} static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* assume, void* t1, void* t2){
struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ( te, t1); struct Cyc_List_List*
tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0; tqs2 != 0;( tqs2=(( struct Cyc_List_List*)
_check_null( tqs2))->tl, tqs1=(( struct Cyc_List_List*) _check_null( tqs1))->tl)){
if( tqs1 == 0){ return 0;}{ struct _tuple4 _temp1988; void* _temp1989; struct
Cyc_Absyn_Tqual _temp1991; struct _tuple4* _temp1986=( struct _tuple4*)(( struct
Cyc_List_List*) _check_null( tqs1))->hd; _temp1988=* _temp1986; _LL1992:
_temp1991= _temp1988.f1; goto _LL1990; _LL1990: _temp1989= _temp1988.f2; goto
_LL1987; _LL1987: { struct _tuple4 _temp1995; void* _temp1996; struct Cyc_Absyn_Tqual
_temp1998; struct _tuple4* _temp1993=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( tqs2))->hd; _temp1995=* _temp1993; _LL1999: _temp1998= _temp1995.f1;
goto _LL1997; _LL1997: _temp1996= _temp1995.f2; goto _LL1994; _LL1994: if(
_temp1998.q_const? Cyc_Tcutil_subtype( te, assume, _temp1989, _temp1996): 0){
continue;} else{ if( Cyc_Tcutil_unify( _temp1989, _temp1996)){ continue;} else{
return 0;}}}}} return 1;} int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
void* _temp2000= t2; struct Cyc_Absyn_Uniondecl** _temp2008; struct Cyc_List_List*
_temp2010; struct _tuple1* _temp2012; void* _temp2014; void* _temp2016; _LL2002:
if(( unsigned int) _temp2000 > 4u?*(( int*) _temp2000) == Cyc_Absyn_UnionType: 0){
_LL2013: _temp2012=(( struct Cyc_Absyn_UnionType_struct*) _temp2000)->f1; goto
_LL2011; _LL2011: _temp2010=(( struct Cyc_Absyn_UnionType_struct*) _temp2000)->f2;
goto _LL2009; _LL2009: _temp2008=(( struct Cyc_Absyn_UnionType_struct*)
_temp2000)->f3; goto _LL2003;} else{ goto _LL2004;} _LL2004: if(( unsigned int)
_temp2000 > 4u?*(( int*) _temp2000) == Cyc_Absyn_IntType: 0){ _LL2017: _temp2016=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2000)->f1; goto _LL2015; _LL2015:
_temp2014=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2000)->f2; if(
_temp2014 ==( void*) Cyc_Absyn_B4){ goto _LL2005;} else{ goto _LL2006;}} else{
goto _LL2006;} _LL2006: goto _LL2007; _LL2003: if( _temp2008 == 0){ goto _LL2001;}{
struct Cyc_Absyn_Uniondecl* _temp2018=*(( struct Cyc_Absyn_Uniondecl**)
_check_null( _temp2008)); goto _LL2019; _LL2019: if( _temp2018->fields == 0){
goto _LL2001;}{ struct Cyc_List_List* _temp2020=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp2018->tvs,
_temp2010); goto _LL2021; _LL2021:{ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp2018->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ if( Cyc_Tcutil_unify( t1, Cyc_Tcutil_substitute(
_temp2020,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ return 1;}}} goto _LL2001;}} _LL2005: if( Cyc_Tcutil_typ_kind(
t2) ==( void*) Cyc_Absyn_BoxKind){ return 1;} goto _LL2001; _LL2007: goto
_LL2001; _LL2001:;}{ void* _temp2022= t1; struct Cyc_Absyn_PtrInfo _temp2038;
struct Cyc_Absyn_Conref* _temp2040; struct Cyc_Absyn_Tqual _temp2042; struct Cyc_Absyn_Conref*
_temp2044; void* _temp2046; void* _temp2048; struct Cyc_Absyn_Exp* _temp2050;
struct Cyc_Absyn_Tqual _temp2052; void* _temp2054; struct Cyc_Absyn_Enumdecl*
_temp2056; struct _tuple1* _temp2058; void* _temp2060; void* _temp2062; _LL2024:
if(( unsigned int) _temp2022 > 4u?*(( int*) _temp2022) == Cyc_Absyn_PointerType:
0){ _LL2039: _temp2038=(( struct Cyc_Absyn_PointerType_struct*) _temp2022)->f1;
_LL2049: _temp2048=( void*) _temp2038.elt_typ; goto _LL2047; _LL2047: _temp2046=(
void*) _temp2038.rgn_typ; goto _LL2045; _LL2045: _temp2044= _temp2038.nullable;
goto _LL2043; _LL2043: _temp2042= _temp2038.tq; goto _LL2041; _LL2041: _temp2040=
_temp2038.bounds; goto _LL2025;} else{ goto _LL2026;} _LL2026: if(( unsigned int)
_temp2022 > 4u?*(( int*) _temp2022) == Cyc_Absyn_ArrayType: 0){ _LL2055:
_temp2054=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2022)->f1; goto
_LL2053; _LL2053: _temp2052=(( struct Cyc_Absyn_ArrayType_struct*) _temp2022)->f2;
goto _LL2051; _LL2051: _temp2050=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2022)->f3; goto _LL2027;} else{ goto _LL2028;} _LL2028: if(( unsigned int)
_temp2022 > 4u?*(( int*) _temp2022) == Cyc_Absyn_EnumType: 0){ _LL2059:
_temp2058=(( struct Cyc_Absyn_EnumType_struct*) _temp2022)->f1; goto _LL2057;
_LL2057: _temp2056=(( struct Cyc_Absyn_EnumType_struct*) _temp2022)->f2; goto
_LL2029;} else{ goto _LL2030;} _LL2030: if(( unsigned int) _temp2022 > 4u?*((
int*) _temp2022) == Cyc_Absyn_IntType: 0){ _LL2063: _temp2062=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2022)->f1; goto _LL2061; _LL2061: _temp2060=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2022)->f2; goto _LL2031;} else{
goto _LL2032;} _LL2032: if( _temp2022 ==( void*) Cyc_Absyn_FloatType){ goto
_LL2033;} else{ goto _LL2034;} _LL2034: if( _temp2022 ==( void*) Cyc_Absyn_DoubleType){
goto _LL2035;} else{ goto _LL2036;} _LL2036: goto _LL2037; _LL2025:{ void*
_temp2064= t2; struct Cyc_Absyn_PtrInfo _temp2070; struct Cyc_Absyn_Conref*
_temp2072; struct Cyc_Absyn_Tqual _temp2074; struct Cyc_Absyn_Conref* _temp2076;
void* _temp2078; void* _temp2080; _LL2066: if(( unsigned int) _temp2064 > 4u?*((
int*) _temp2064) == Cyc_Absyn_PointerType: 0){ _LL2071: _temp2070=(( struct Cyc_Absyn_PointerType_struct*)
_temp2064)->f1; _LL2081: _temp2080=( void*) _temp2070.elt_typ; goto _LL2079;
_LL2079: _temp2078=( void*) _temp2070.rgn_typ; goto _LL2077; _LL2077: _temp2076=
_temp2070.nullable; goto _LL2075; _LL2075: _temp2074= _temp2070.tq; goto _LL2073;
_LL2073: _temp2072= _temp2070.bounds; goto _LL2067;} else{ goto _LL2068;}
_LL2068: goto _LL2069; _LL2067: { struct Cyc_List_List* _temp2084=({ struct Cyc_List_List*
_temp2082=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2082->hd=( void*)({ struct _tuple8* _temp2083=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp2083->f1= t1; _temp2083->f2= t2; _temp2083;});
_temp2082->tl= 0; _temp2082;}); goto _LL2085; _LL2085: { int _temp2086= Cyc_Tcutil_ptrsubtype(
te, _temp2084, _temp2048, _temp2080)? ! _temp2042.q_const? 1: _temp2074.q_const:
0; goto _LL2087; _LL2087: { int bounds_ok=(( int(*)( int(* cmp)( void*, void*),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp2040, _temp2072); if( ! bounds_ok){ struct _tuple8
_temp2089=({ struct _tuple8 _temp2088; _temp2088.f1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp2040); _temp2088.f2=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp2072); _temp2088;}); void* _temp2095; struct Cyc_Absyn_Exp*
_temp2097; void* _temp2099; struct Cyc_Absyn_Exp* _temp2101; _LL2091: _LL2100:
_temp2099= _temp2089.f1; if(( unsigned int) _temp2099 > 1u?*(( int*) _temp2099)
== Cyc_Absyn_Upper_b: 0){ _LL2102: _temp2101=(( struct Cyc_Absyn_Upper_b_struct*)
_temp2099)->f1; goto _LL2096;} else{ goto _LL2093;} _LL2096: _temp2095=
_temp2089.f2; if(( unsigned int) _temp2095 > 1u?*(( int*) _temp2095) == Cyc_Absyn_Upper_b:
0){ _LL2098: _temp2097=(( struct Cyc_Absyn_Upper_b_struct*) _temp2095)->f1; goto
_LL2092;} else{ goto _LL2093;} _LL2093: goto _LL2094; _LL2092: if( Cyc_Evexp_eval_const_uint_exp(
_temp2101) >= Cyc_Evexp_eval_const_uint_exp( _temp2097)){ bounds_ok= 1;} goto
_LL2090; _LL2094: bounds_ok= 1; goto _LL2090; _LL2090:;} return( bounds_ok?
_temp2086: 0)? Cyc_Tcutil_unify( _temp2046, _temp2078)? 1: Cyc_Tcenv_region_outlives(
te, _temp2046, _temp2078): 0;}}} _LL2069: goto _LL2065; _LL2065:;} return 0;
_LL2027:{ void* _temp2103= t2; struct Cyc_Absyn_Exp* _temp2109; struct Cyc_Absyn_Tqual
_temp2111; void* _temp2113; _LL2105: if(( unsigned int) _temp2103 > 4u?*(( int*)
_temp2103) == Cyc_Absyn_ArrayType: 0){ _LL2114: _temp2113=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2103)->f1; goto _LL2112; _LL2112: _temp2111=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2103)->f2; goto _LL2110; _LL2110: _temp2109=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2103)->f3; goto _LL2106;} else{ goto _LL2107;} _LL2107: goto _LL2108;
_LL2106: { int okay; okay=( _temp2050 != 0? _temp2109 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2050)) >= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2109)): 0; return( okay? Cyc_Tcutil_unify(
_temp2054, _temp2113): 0)? ! _temp2052.q_const? 1: _temp2111.q_const: 0;}
_LL2108: return 0; _LL2104:;} return 0; _LL2029:{ void* _temp2115= t2; struct
Cyc_Absyn_Enumdecl* _temp2121; struct _tuple1* _temp2123; _LL2117: if((
unsigned int) _temp2115 > 4u?*(( int*) _temp2115) == Cyc_Absyn_EnumType: 0){
_LL2124: _temp2123=(( struct Cyc_Absyn_EnumType_struct*) _temp2115)->f1; goto
_LL2122; _LL2122: _temp2121=(( struct Cyc_Absyn_EnumType_struct*) _temp2115)->f2;
goto _LL2118;} else{ goto _LL2119;} _LL2119: goto _LL2120; _LL2118: if((((
struct Cyc_Absyn_Enumdecl*) _check_null( _temp2056))->fields != 0?(( struct Cyc_Absyn_Enumdecl*)
_check_null( _temp2121))->fields != 0: 0)?(( int(*)( struct Cyc_List_List* x))
Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(((
struct Cyc_Absyn_Enumdecl*) _check_null( _temp2056))->fields))->v) >=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Enumdecl*) _check_null( _temp2121))->fields))->v):
0){ return 1;} goto _LL2116; _LL2120: goto _LL2116; _LL2116:;} goto _LL2031;
_LL2031: goto _LL2033; _LL2033: goto _LL2035; _LL2035: return Cyc_Tcutil_coerceable(
t2); _LL2037: return 0; _LL2023:;}} void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, void* t){ if( ! Cyc_Tcutil_unify(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v, t)){ struct Cyc_Absyn_Exp* _temp2125=
Cyc_Absyn_copy_exp( e); goto _LL2126; _LL2126:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Cast_e_struct* _temp2127=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2127[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2128; _temp2128.tag= Cyc_Absyn_Cast_e; _temp2128.f1=( void*) t; _temp2128.f2=
_temp2125; _temp2128;}); _temp2127;}))); e->topt=({ struct Cyc_Core_Opt*
_temp2129=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2129->v=( void*) t; _temp2129;});}} int Cyc_Tcutil_is_integral( struct Cyc_Absyn_Exp*
e){ void* _temp2130= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); struct Cyc_Absyn_Enumdecl* _temp2140; struct _tuple1*
_temp2142; void* _temp2144; void* _temp2146; struct Cyc_Core_Opt* _temp2148; int
_temp2150; struct Cyc_Core_Opt* _temp2152; struct Cyc_Core_Opt* _temp2154;
_LL2132: if(( unsigned int) _temp2130 > 4u?*(( int*) _temp2130) == Cyc_Absyn_EnumType:
0){ _LL2143: _temp2142=(( struct Cyc_Absyn_EnumType_struct*) _temp2130)->f1;
goto _LL2141; _LL2141: _temp2140=(( struct Cyc_Absyn_EnumType_struct*) _temp2130)->f2;
goto _LL2133;} else{ goto _LL2134;} _LL2134: if(( unsigned int) _temp2130 > 4u?*((
int*) _temp2130) == Cyc_Absyn_IntType: 0){ _LL2147: _temp2146=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2130)->f1; goto _LL2145; _LL2145: _temp2144=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2130)->f2; goto _LL2135;} else{
goto _LL2136;} _LL2136: if(( unsigned int) _temp2130 > 4u?*(( int*) _temp2130)
== Cyc_Absyn_Evar: 0){ _LL2155: _temp2154=(( struct Cyc_Absyn_Evar_struct*)
_temp2130)->f1; goto _LL2153; _LL2153: _temp2152=(( struct Cyc_Absyn_Evar_struct*)
_temp2130)->f2; goto _LL2151; _LL2151: _temp2150=(( struct Cyc_Absyn_Evar_struct*)
_temp2130)->f3; goto _LL2149; _LL2149: _temp2148=(( struct Cyc_Absyn_Evar_struct*)
_temp2130)->f4; goto _LL2137;} else{ goto _LL2138;} _LL2138: goto _LL2139;
_LL2133: goto _LL2135; _LL2135: return 1; _LL2137: if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;}
return 0; _LL2139: return 0; _LL2131:;} int Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp*
e){ if( Cyc_Tcutil_is_integral( e)){ return 1;}{ void* _temp2156= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _LL2158: if(
_temp2156 ==( void*) Cyc_Absyn_FloatType){ goto _LL2159;} else{ goto _LL2160;}
_LL2160: if( _temp2156 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2161;} else{
goto _LL2162;} _LL2162: goto _LL2163; _LL2159: goto _LL2161; _LL2161: return 1;
_LL2163: return 0; _LL2157:;}} int Cyc_Tcutil_is_function_type( void* t){ void*
_temp2164= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp2170; _LL2166:
if(( unsigned int) _temp2164 > 4u?*(( int*) _temp2164) == Cyc_Absyn_FnType: 0){
_LL2171: _temp2170=(( struct Cyc_Absyn_FnType_struct*) _temp2164)->f1; goto
_LL2167;} else{ goto _LL2168;} _LL2168: goto _LL2169; _LL2167: return 1; _LL2169:
return 0; _LL2165:;} void* Cyc_Tcutil_max_arithmetic_type( void* t1, void* t2){
struct _tuple8 _temp2173=({ struct _tuple8 _temp2172; _temp2172.f1= t1;
_temp2172.f2= t2; _temp2172;}); void* _temp2197; void* _temp2199; void*
_temp2201; void* _temp2203; void* _temp2205; void* _temp2207; void* _temp2209;
void* _temp2211; void* _temp2213; void* _temp2215; void* _temp2217; void*
_temp2219; void* _temp2221; void* _temp2223; void* _temp2225; void* _temp2227;
void* _temp2229; void* _temp2231; void* _temp2233; void* _temp2235; void*
_temp2237; void* _temp2239; void* _temp2241; void* _temp2243; void* _temp2245;
void* _temp2247; void* _temp2249; void* _temp2251; void* _temp2253; void*
_temp2255; void* _temp2257; void* _temp2259; _LL2175: _LL2200: _temp2199=
_temp2173.f1; if( _temp2199 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2198;}
else{ goto _LL2177;} _LL2198: _temp2197= _temp2173.f2; goto _LL2176; _LL2177:
_LL2204: _temp2203= _temp2173.f1; goto _LL2202; _LL2202: _temp2201= _temp2173.f2;
if( _temp2201 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2178;} else{ goto
_LL2179;} _LL2179: _LL2208: _temp2207= _temp2173.f1; if( _temp2207 ==( void*)
Cyc_Absyn_FloatType){ goto _LL2206;} else{ goto _LL2181;} _LL2206: _temp2205=
_temp2173.f2; goto _LL2180; _LL2181: _LL2212: _temp2211= _temp2173.f1; goto
_LL2210; _LL2210: _temp2209= _temp2173.f2; if( _temp2209 ==( void*) Cyc_Absyn_FloatType){
goto _LL2182;} else{ goto _LL2183;} _LL2183: _LL2216: _temp2215= _temp2173.f1;
if(( unsigned int) _temp2215 > 4u?*(( int*) _temp2215) == Cyc_Absyn_IntType: 0){
_LL2220: _temp2219=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2215)->f1;
if( _temp2219 ==( void*) Cyc_Absyn_Unsigned){ goto _LL2218;} else{ goto _LL2185;}
_LL2218: _temp2217=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2215)->f2;
if( _temp2217 ==( void*) Cyc_Absyn_B8){ goto _LL2214;} else{ goto _LL2185;}}
else{ goto _LL2185;} _LL2214: _temp2213= _temp2173.f2; goto _LL2184; _LL2185:
_LL2228: _temp2227= _temp2173.f1; goto _LL2222; _LL2222: _temp2221= _temp2173.f2;
if(( unsigned int) _temp2221 > 4u?*(( int*) _temp2221) == Cyc_Absyn_IntType: 0){
_LL2226: _temp2225=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2221)->f1;
if( _temp2225 ==( void*) Cyc_Absyn_Unsigned){ goto _LL2224;} else{ goto _LL2187;}
_LL2224: _temp2223=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2221)->f2;
if( _temp2223 ==( void*) Cyc_Absyn_B8){ goto _LL2186;} else{ goto _LL2187;}}
else{ goto _LL2187;} _LL2187: _LL2232: _temp2231= _temp2173.f1; if((
unsigned int) _temp2231 > 4u?*(( int*) _temp2231) == Cyc_Absyn_IntType: 0){
_LL2236: _temp2235=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2231)->f1;
if( _temp2235 ==( void*) Cyc_Absyn_Signed){ goto _LL2234;} else{ goto _LL2189;}
_LL2234: _temp2233=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2231)->f2;
if( _temp2233 ==( void*) Cyc_Absyn_B8){ goto _LL2230;} else{ goto _LL2189;}}
else{ goto _LL2189;} _LL2230: _temp2229= _temp2173.f2; goto _LL2188; _LL2189:
_LL2244: _temp2243= _temp2173.f1; goto _LL2238; _LL2238: _temp2237= _temp2173.f2;
if(( unsigned int) _temp2237 > 4u?*(( int*) _temp2237) == Cyc_Absyn_IntType: 0){
_LL2242: _temp2241=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2237)->f1;
if( _temp2241 ==( void*) Cyc_Absyn_Signed){ goto _LL2240;} else{ goto _LL2191;}
_LL2240: _temp2239=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2237)->f2;
if( _temp2239 ==( void*) Cyc_Absyn_B8){ goto _LL2190;} else{ goto _LL2191;}}
else{ goto _LL2191;} _LL2191: _LL2248: _temp2247= _temp2173.f1; if((
unsigned int) _temp2247 > 4u?*(( int*) _temp2247) == Cyc_Absyn_IntType: 0){
_LL2252: _temp2251=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2247)->f1;
if( _temp2251 ==( void*) Cyc_Absyn_Unsigned){ goto _LL2250;} else{ goto _LL2193;}
_LL2250: _temp2249=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2247)->f2;
if( _temp2249 ==( void*) Cyc_Absyn_B4){ goto _LL2246;} else{ goto _LL2193;}}
else{ goto _LL2193;} _LL2246: _temp2245= _temp2173.f2; goto _LL2192; _LL2193:
_LL2260: _temp2259= _temp2173.f1; goto _LL2254; _LL2254: _temp2253= _temp2173.f2;
if(( unsigned int) _temp2253 > 4u?*(( int*) _temp2253) == Cyc_Absyn_IntType: 0){
_LL2258: _temp2257=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2253)->f1;
if( _temp2257 ==( void*) Cyc_Absyn_Unsigned){ goto _LL2256;} else{ goto _LL2195;}
_LL2256: _temp2255=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2253)->f2;
if( _temp2255 ==( void*) Cyc_Absyn_B4){ goto _LL2194;} else{ goto _LL2195;}}
else{ goto _LL2195;} _LL2195: goto _LL2196; _LL2176: goto _LL2178; _LL2178:
return( void*) Cyc_Absyn_DoubleType; _LL2180: goto _LL2182; _LL2182: return(
void*) Cyc_Absyn_FloatType; _LL2184: goto _LL2186; _LL2186: return Cyc_Absyn_ulong_t;
_LL2188: goto _LL2190; _LL2190: return Cyc_Absyn_slong_t; _LL2192: goto _LL2194;
_LL2194: return Cyc_Absyn_uint_t; _LL2196: return Cyc_Absyn_sint_t; _LL2174:;}
void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp* e){ void* _temp2261=(
void*) e->r; struct Cyc_Absyn_Exp* _temp2267; struct Cyc_Core_Opt* _temp2269;
struct Cyc_Absyn_Exp* _temp2271; _LL2263: if(*(( int*) _temp2261) == Cyc_Absyn_AssignOp_e){
_LL2272: _temp2271=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp2261)->f1; goto
_LL2270; _LL2270: _temp2269=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp2261)->f2;
if( _temp2269 == 0){ goto _LL2268;} else{ goto _LL2265;} _LL2268: _temp2267=((
struct Cyc_Absyn_AssignOp_e_struct*) _temp2261)->f3; goto _LL2264;} else{ goto
_LL2265;} _LL2265: goto _LL2266; _LL2264: Cyc_Tcutil_warn( e->loc,({
unsigned char* _temp2273=( unsigned char*)"assignment in test"; struct
_tagged_string _temp2274; _temp2274.curr= _temp2273; _temp2274.base= _temp2273;
_temp2274.last_plus_one= _temp2273 + 19; _temp2274;})); goto _LL2262; _LL2266:
goto _LL2262; _LL2262:;} static int Cyc_Tcutil_constrain_kinds( struct Cyc_Absyn_Conref*
c1, struct Cyc_Absyn_Conref* c2){ c1=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( c1); c2=(( struct Cyc_Absyn_Conref*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( c2); if( c1 == c2){ return 1;}
if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr){( void*)( c1->v=( void*)((
void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2275=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2275[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2276; _temp2276.tag= Cyc_Absyn_Forward_constr;
_temp2276.f1= c2; _temp2276;}); _temp2275;}))); return 1;} else{ if(( void*) c2->v
==( void*) Cyc_Absyn_No_constr){( void*)( c2->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp2277=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp2277[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp2278; _temp2278.tag= Cyc_Absyn_Forward_constr; _temp2278.f1= c1; _temp2278;});
_temp2277;}))); return 1;} else{ void* k1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( c1); void* k2=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( c2); if( Cyc_Tcutil_kind_leq( k1, k2)){( void*)( c2->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2279=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2279[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2280; _temp2280.tag= Cyc_Absyn_Forward_constr;
_temp2280.f1= c1; _temp2280;}); _temp2279;}))); return 1;} else{ if( Cyc_Tcutil_kind_leq(
k2, k1)){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp2281=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp2281[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp2282; _temp2282.tag= Cyc_Absyn_Forward_constr; _temp2282.f1= c2; _temp2282;});
_temp2281;}))); return 1;} else{ return 0;}}}}} static int Cyc_Tcutil_tvar_id_counter=
0; int* Cyc_Tcutil_new_tvar_id(){ return({ int* _temp2283=( int*)
GC_malloc_atomic( sizeof( int)); _temp2283[ 0]= Cyc_Tcutil_tvar_id_counter ++;
_temp2283;});} static int Cyc_Tcutil_tvar_counter= 0; struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar(
void* k){ int i= Cyc_Tcutil_tvar_counter ++; struct _tagged_string s=( struct
_tagged_string) xprintf("#%d", i); return({ struct Cyc_Absyn_Tvar* _temp2284=(
struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2284->name=({
struct _tagged_string* _temp2285=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp2285[ 0]= s; _temp2285;}); _temp2284->identity=
0; _temp2284->kind=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
k); _temp2284;});} int Cyc_Tcutil_is_temp_tvar( struct Cyc_Absyn_Tvar* t){
struct _tagged_string _temp2286=* t->name; goto _LL2287; _LL2287: return*((
const unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( _temp2286, sizeof( unsigned char), 0) =='#';} void
Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar* t){ if( ! Cyc_Tcutil_is_temp_tvar(
t)){ return;}{ struct _tagged_string _temp2290= Cyc_String_strconcat(({
unsigned char* _temp2288=( unsigned char*)"`"; struct _tagged_string _temp2289;
_temp2289.curr= _temp2288; _temp2289.base= _temp2288; _temp2289.last_plus_one=
_temp2288 + 2; _temp2289;}),* t->name); goto _LL2291; _LL2291:*(( unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
_temp2290, sizeof( unsigned char), 1)='t'; t->name=({ struct _tagged_string*
_temp2292=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2292[ 0]=( struct _tagged_string) _temp2290; _temp2292;});}} struct
_tuple13{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ;
static struct _tuple2* Cyc_Tcutil_fndecl2typ_f( struct _tuple13* x){ return({
struct _tuple2* _temp2293=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2293->f1=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp2294=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2294->v=( void*)(*
x).f1; _temp2294;}); _temp2293->f2=(* x).f2; _temp2293->f3=(* x).f3; _temp2293;});}
void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl* fd){ if( fd->cached_typ ==
0){ struct Cyc_List_List* _temp2295= 0; goto _LL2296; _LL2296:{ struct Cyc_List_List*
atts= fd->attributes; for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ if( Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd)){ _temp2295=({ struct Cyc_List_List* _temp2297=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2297->hd=( void*)((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd); _temp2297->tl=
_temp2295; _temp2297;});}}} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp2298=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2298[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2299; _temp2299.tag= Cyc_Absyn_FnType;
_temp2299.f1=({ struct Cyc_Absyn_FnInfo _temp2300; _temp2300.tvars= fd->tvs;
_temp2300.effect= fd->effect; _temp2300.ret_typ=( void*)(( void*) fd->ret_type);
_temp2300.args=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct
_tuple13*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_fndecl2typ_f, fd->args);
_temp2300.c_varargs= fd->c_varargs; _temp2300.cyc_varargs= fd->cyc_varargs;
_temp2300.rgn_po= fd->rgn_po; _temp2300.attributes= _temp2295; _temp2300;});
_temp2299;}); _temp2298;});} return( void*)(( struct Cyc_Core_Opt*) _check_null(
fd->cached_typ))->v;} struct _tuple14{ void* f1; struct Cyc_Absyn_Tqual f2; void*
f3; } ; static void* Cyc_Tcutil_fst_fdarg( struct _tuple14* t){ return(* t).f1;}
void* Cyc_Tcutil_snd_tqt( struct _tuple4* t){ return(* t).f2;} static struct
_tuple4* Cyc_Tcutil_map2_tq( struct _tuple4* pr, void* t){ return({ struct
_tuple4* _temp2301=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp2301->f1=(* pr).f1; _temp2301->f2= t; _temp2301;});} void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl*
fd){ void* t= Cyc_Tcutil_fndecl2typ( fd); Cyc_Tcutil_check_valid_toplevel_type(
loc, te, t);{ void* _temp2302= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo
_temp2308; struct Cyc_List_List* _temp2310; struct Cyc_List_List* _temp2312;
struct Cyc_Absyn_VarargInfo* _temp2314; int _temp2316; struct Cyc_List_List*
_temp2318; void* _temp2320; struct Cyc_Core_Opt* _temp2322; struct Cyc_List_List*
_temp2324; _LL2304: if(( unsigned int) _temp2302 > 4u?*(( int*) _temp2302) ==
Cyc_Absyn_FnType: 0){ _LL2309: _temp2308=(( struct Cyc_Absyn_FnType_struct*)
_temp2302)->f1; _LL2325: _temp2324= _temp2308.tvars; goto _LL2323; _LL2323:
_temp2322= _temp2308.effect; goto _LL2321; _LL2321: _temp2320=( void*) _temp2308.ret_typ;
goto _LL2319; _LL2319: _temp2318= _temp2308.args; goto _LL2317; _LL2317:
_temp2316= _temp2308.c_varargs; goto _LL2315; _LL2315: _temp2314= _temp2308.cyc_varargs;
goto _LL2313; _LL2313: _temp2312= _temp2308.rgn_po; goto _LL2311; _LL2311:
_temp2310= _temp2308.attributes; goto _LL2305;} else{ goto _LL2306;} _LL2306:
goto _LL2307; _LL2305: fd->tvs= _temp2324; fd->effect= _temp2322; goto _LL2303;
_LL2307:(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
unsigned char* _temp2326=( unsigned char*)"check_fndecl_valid_type: not a FnType";
struct _tagged_string _temp2327; _temp2327.curr= _temp2326; _temp2327.base=
_temp2326; _temp2327.last_plus_one= _temp2326 + 38; _temp2327;})); return;
_LL2303:;}{ struct _RegionHandle _temp2328= _new_region(); struct _RegionHandle*
r=& _temp2328; _push_region( r); Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tagged_string*(* f)( struct _tuple13*), struct
Cyc_List_List* x)) Cyc_List_rmap)( r,( struct _tagged_string*(*)( struct
_tuple13* t)) Cyc_Tcutil_fst_fdarg, fd->args), loc,({ unsigned char* _temp2329=(
unsigned char*)"function declaration has repeated parameter"; struct
_tagged_string _temp2330; _temp2330.curr= _temp2329; _temp2330.base= _temp2329;
_temp2330.last_plus_one= _temp2329 + 44; _temp2330;}));; _pop_region(& _temp2328);}
fd->cached_typ=({ struct Cyc_Core_Opt* _temp2331=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2331->v=( void*) t; _temp2331;});}
struct _tuple15{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; } ; struct
_tuple16{ struct _tuple15* f1; void* f2; } ; static struct _tuple16* Cyc_Tcutil_substitute_f1(
struct _RegionHandle* rgn, struct _tuple2* y){ return({ struct _tuple16*
_temp2332=( struct _tuple16*) _region_malloc( rgn, sizeof( struct _tuple16));
_temp2332->f1=({ struct _tuple15* _temp2333=( struct _tuple15*) _region_malloc(
rgn, sizeof( struct _tuple15)); _temp2333->f1=(* y).f1; _temp2333->f2=(* y).f2;
_temp2333;}); _temp2332->f2=(* y).f3; _temp2332;});} static struct _tuple2* Cyc_Tcutil_substitute_f2(
struct _tuple16* w){ void* _temp2336; struct _tuple15* _temp2338; struct
_tuple16 _temp2334=* w; _LL2339: _temp2338= _temp2334.f1; goto _LL2337; _LL2337:
_temp2336= _temp2334.f2; goto _LL2335; _LL2335: { struct Cyc_Absyn_Tqual
_temp2342; struct Cyc_Core_Opt* _temp2344; struct _tuple15 _temp2340=* _temp2338;
_LL2345: _temp2344= _temp2340.f1; goto _LL2343; _LL2343: _temp2342= _temp2340.f2;
goto _LL2341; _LL2341: return({ struct _tuple2* _temp2346=( struct _tuple2*)
GC_malloc( sizeof( struct _tuple2)); _temp2346->f1= _temp2344; _temp2346->f2=
_temp2342; _temp2346->f3= _temp2336; _temp2346;});}} static void* Cyc_Tcutil_field_type(
struct Cyc_Absyn_Structfield* f){ return( void*) f->type;} static struct Cyc_Absyn_Structfield*
Cyc_Tcutil_zip_field_type( struct Cyc_Absyn_Structfield* f, void* t){ return({
struct Cyc_Absyn_Structfield* _temp2347=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2347->name= f->name;
_temp2347->tq= f->tq; _temp2347->type=( void*) t; _temp2347->width= f->width;
_temp2347->attributes= f->attributes; _temp2347;});} static struct Cyc_List_List*
Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List* inst, struct
Cyc_List_List* ts); void* Cyc_Tcutil_rsubstitute( struct _RegionHandle* rgn,
struct Cyc_List_List* inst, void* t){ void* _temp2348= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp2394; struct Cyc_Absyn_Structdecl** _temp2396;
struct Cyc_List_List* _temp2398; struct _tuple1* _temp2400; struct Cyc_Absyn_Uniondecl**
_temp2402; struct Cyc_List_List* _temp2404; struct _tuple1* _temp2406; struct
Cyc_Absyn_TunionInfo _temp2408; void* _temp2410; struct Cyc_List_List* _temp2412;
void* _temp2414; struct Cyc_Absyn_TunionFieldInfo _temp2416; struct Cyc_List_List*
_temp2418; void* _temp2420; struct Cyc_Core_Opt* _temp2422; struct Cyc_List_List*
_temp2424; struct _tuple1* _temp2426; struct Cyc_Absyn_Exp* _temp2428; struct
Cyc_Absyn_Tqual _temp2430; void* _temp2432; struct Cyc_Absyn_PtrInfo _temp2434;
struct Cyc_Absyn_Conref* _temp2436; struct Cyc_Absyn_Tqual _temp2438; struct Cyc_Absyn_Conref*
_temp2440; void* _temp2442; void* _temp2444; struct Cyc_Absyn_FnInfo _temp2446;
struct Cyc_List_List* _temp2448; struct Cyc_List_List* _temp2450; struct Cyc_Absyn_VarargInfo*
_temp2452; int _temp2454; struct Cyc_List_List* _temp2456; void* _temp2458;
struct Cyc_Core_Opt* _temp2460; struct Cyc_List_List* _temp2462; struct Cyc_List_List*
_temp2464; struct Cyc_List_List* _temp2466; struct Cyc_List_List* _temp2468;
struct Cyc_Core_Opt* _temp2470; int _temp2472; struct Cyc_Core_Opt* _temp2474;
struct Cyc_Core_Opt* _temp2476; void* _temp2478; struct Cyc_Absyn_Enumdecl*
_temp2480; struct _tuple1* _temp2482; void* _temp2484; void* _temp2486; void*
_temp2488; struct Cyc_List_List* _temp2490; _LL2350: if(( unsigned int)
_temp2348 > 4u?*(( int*) _temp2348) == Cyc_Absyn_VarType: 0){ _LL2395: _temp2394=((
struct Cyc_Absyn_VarType_struct*) _temp2348)->f1; goto _LL2351;} else{ goto
_LL2352;} _LL2352: if(( unsigned int) _temp2348 > 4u?*(( int*) _temp2348) == Cyc_Absyn_StructType:
0){ _LL2401: _temp2400=(( struct Cyc_Absyn_StructType_struct*) _temp2348)->f1;
goto _LL2399; _LL2399: _temp2398=(( struct Cyc_Absyn_StructType_struct*)
_temp2348)->f2; goto _LL2397; _LL2397: _temp2396=(( struct Cyc_Absyn_StructType_struct*)
_temp2348)->f3; goto _LL2353;} else{ goto _LL2354;} _LL2354: if(( unsigned int)
_temp2348 > 4u?*(( int*) _temp2348) == Cyc_Absyn_UnionType: 0){ _LL2407:
_temp2406=(( struct Cyc_Absyn_UnionType_struct*) _temp2348)->f1; goto _LL2405;
_LL2405: _temp2404=(( struct Cyc_Absyn_UnionType_struct*) _temp2348)->f2; goto
_LL2403; _LL2403: _temp2402=(( struct Cyc_Absyn_UnionType_struct*) _temp2348)->f3;
goto _LL2355;} else{ goto _LL2356;} _LL2356: if(( unsigned int) _temp2348 > 4u?*((
int*) _temp2348) == Cyc_Absyn_TunionType: 0){ _LL2409: _temp2408=(( struct Cyc_Absyn_TunionType_struct*)
_temp2348)->f1; _LL2415: _temp2414=( void*) _temp2408.tunion_info; goto _LL2413;
_LL2413: _temp2412= _temp2408.targs; goto _LL2411; _LL2411: _temp2410=( void*)
_temp2408.rgn; goto _LL2357;} else{ goto _LL2358;} _LL2358: if(( unsigned int)
_temp2348 > 4u?*(( int*) _temp2348) == Cyc_Absyn_TunionFieldType: 0){ _LL2417:
_temp2416=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2348)->f1; _LL2421:
_temp2420=( void*) _temp2416.field_info; goto _LL2419; _LL2419: _temp2418=
_temp2416.targs; goto _LL2359;} else{ goto _LL2360;} _LL2360: if(( unsigned int)
_temp2348 > 4u?*(( int*) _temp2348) == Cyc_Absyn_TypedefType: 0){ _LL2427:
_temp2426=(( struct Cyc_Absyn_TypedefType_struct*) _temp2348)->f1; goto _LL2425;
_LL2425: _temp2424=(( struct Cyc_Absyn_TypedefType_struct*) _temp2348)->f2; goto
_LL2423; _LL2423: _temp2422=(( struct Cyc_Absyn_TypedefType_struct*) _temp2348)->f3;
goto _LL2361;} else{ goto _LL2362;} _LL2362: if(( unsigned int) _temp2348 > 4u?*((
int*) _temp2348) == Cyc_Absyn_ArrayType: 0){ _LL2433: _temp2432=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2348)->f1; goto _LL2431; _LL2431:
_temp2430=(( struct Cyc_Absyn_ArrayType_struct*) _temp2348)->f2; goto _LL2429;
_LL2429: _temp2428=(( struct Cyc_Absyn_ArrayType_struct*) _temp2348)->f3; goto
_LL2363;} else{ goto _LL2364;} _LL2364: if(( unsigned int) _temp2348 > 4u?*((
int*) _temp2348) == Cyc_Absyn_PointerType: 0){ _LL2435: _temp2434=(( struct Cyc_Absyn_PointerType_struct*)
_temp2348)->f1; _LL2445: _temp2444=( void*) _temp2434.elt_typ; goto _LL2443;
_LL2443: _temp2442=( void*) _temp2434.rgn_typ; goto _LL2441; _LL2441: _temp2440=
_temp2434.nullable; goto _LL2439; _LL2439: _temp2438= _temp2434.tq; goto _LL2437;
_LL2437: _temp2436= _temp2434.bounds; goto _LL2365;} else{ goto _LL2366;}
_LL2366: if(( unsigned int) _temp2348 > 4u?*(( int*) _temp2348) == Cyc_Absyn_FnType:
0){ _LL2447: _temp2446=(( struct Cyc_Absyn_FnType_struct*) _temp2348)->f1;
_LL2463: _temp2462= _temp2446.tvars; goto _LL2461; _LL2461: _temp2460= _temp2446.effect;
goto _LL2459; _LL2459: _temp2458=( void*) _temp2446.ret_typ; goto _LL2457;
_LL2457: _temp2456= _temp2446.args; goto _LL2455; _LL2455: _temp2454= _temp2446.c_varargs;
goto _LL2453; _LL2453: _temp2452= _temp2446.cyc_varargs; goto _LL2451; _LL2451:
_temp2450= _temp2446.rgn_po; goto _LL2449; _LL2449: _temp2448= _temp2446.attributes;
goto _LL2367;} else{ goto _LL2368;} _LL2368: if(( unsigned int) _temp2348 > 4u?*((
int*) _temp2348) == Cyc_Absyn_TupleType: 0){ _LL2465: _temp2464=(( struct Cyc_Absyn_TupleType_struct*)
_temp2348)->f1; goto _LL2369;} else{ goto _LL2370;} _LL2370: if(( unsigned int)
_temp2348 > 4u?*(( int*) _temp2348) == Cyc_Absyn_AnonStructType: 0){ _LL2467:
_temp2466=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2348)->f1; goto
_LL2371;} else{ goto _LL2372;} _LL2372: if(( unsigned int) _temp2348 > 4u?*((
int*) _temp2348) == Cyc_Absyn_AnonUnionType: 0){ _LL2469: _temp2468=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp2348)->f1; goto _LL2373;} else{ goto
_LL2374;} _LL2374: if(( unsigned int) _temp2348 > 4u?*(( int*) _temp2348) == Cyc_Absyn_Evar:
0){ _LL2477: _temp2476=(( struct Cyc_Absyn_Evar_struct*) _temp2348)->f1; goto
_LL2475; _LL2475: _temp2474=(( struct Cyc_Absyn_Evar_struct*) _temp2348)->f2;
goto _LL2473; _LL2473: _temp2472=(( struct Cyc_Absyn_Evar_struct*) _temp2348)->f3;
goto _LL2471; _LL2471: _temp2470=(( struct Cyc_Absyn_Evar_struct*) _temp2348)->f4;
goto _LL2375;} else{ goto _LL2376;} _LL2376: if(( unsigned int) _temp2348 > 4u?*((
int*) _temp2348) == Cyc_Absyn_RgnHandleType: 0){ _LL2479: _temp2478=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp2348)->f1; goto _LL2377;} else{
goto _LL2378;} _LL2378: if(( unsigned int) _temp2348 > 4u?*(( int*) _temp2348)
== Cyc_Absyn_EnumType: 0){ _LL2483: _temp2482=(( struct Cyc_Absyn_EnumType_struct*)
_temp2348)->f1; goto _LL2481; _LL2481: _temp2480=(( struct Cyc_Absyn_EnumType_struct*)
_temp2348)->f2; goto _LL2379;} else{ goto _LL2380;} _LL2380: if( _temp2348 ==(
void*) Cyc_Absyn_VoidType){ goto _LL2381;} else{ goto _LL2382;} _LL2382: if((
unsigned int) _temp2348 > 4u?*(( int*) _temp2348) == Cyc_Absyn_IntType: 0){
_LL2487: _temp2486=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2348)->f1;
goto _LL2485; _LL2485: _temp2484=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2348)->f2; goto _LL2383;} else{ goto _LL2384;} _LL2384: if( _temp2348 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2385;} else{ goto _LL2386;} _LL2386: if(
_temp2348 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2387;} else{ goto _LL2388;}
_LL2388: if( _temp2348 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL2389;} else{ goto
_LL2390;} _LL2390: if(( unsigned int) _temp2348 > 4u?*(( int*) _temp2348) == Cyc_Absyn_AccessEff:
0){ _LL2489: _temp2488=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp2348)->f1;
goto _LL2391;} else{ goto _LL2392;} _LL2392: if(( unsigned int) _temp2348 > 4u?*((
int*) _temp2348) == Cyc_Absyn_JoinEff: 0){ _LL2491: _temp2490=(( struct Cyc_Absyn_JoinEff_struct*)
_temp2348)->f1; goto _LL2393;} else{ goto _LL2349;} _LL2351: { struct
_handler_cons _temp2492; _push_handler(& _temp2492);{ int _temp2494= 0; if(
setjmp( _temp2492.handler)){ _temp2494= 1;} if( ! _temp2494){{ void* _temp2495=((
void*(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp,
inst, _temp2394); _npop_handler( 0u); return _temp2495;}; _pop_handler();} else{
void* _temp2493=( void*) _exn_thrown; void* _temp2497= _temp2493; _LL2499: if(
_temp2497 == Cyc_Core_Not_found){ goto _LL2500;} else{ goto _LL2501;} _LL2501:
goto _LL2502; _LL2500: return t; _LL2502:( void) _throw( _temp2497); _LL2498:;}}}
_LL2353: { struct Cyc_List_List* _temp2503= Cyc_Tcutil_substs( rgn, inst,
_temp2398); goto _LL2504; _LL2504: return _temp2503 == _temp2398? t:( void*)({
struct Cyc_Absyn_StructType_struct* _temp2505=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp2505[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp2506; _temp2506.tag= Cyc_Absyn_StructType;
_temp2506.f1= _temp2400; _temp2506.f2= _temp2503; _temp2506.f3= _temp2396;
_temp2506;}); _temp2505;});} _LL2355: { struct Cyc_List_List* _temp2507= Cyc_Tcutil_substs(
rgn, inst, _temp2404); goto _LL2508; _LL2508: return _temp2507 == _temp2404? t:(
void*)({ struct Cyc_Absyn_UnionType_struct* _temp2509=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp2509[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp2510; _temp2510.tag= Cyc_Absyn_UnionType;
_temp2510.f1= _temp2406; _temp2510.f2= _temp2507; _temp2510.f3= _temp2402;
_temp2510;}); _temp2509;});} _LL2357: { struct Cyc_List_List* _temp2511= Cyc_Tcutil_substs(
rgn, inst, _temp2412); goto _LL2512; _LL2512: { void* _temp2513= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2410); goto _LL2514; _LL2514: return( _temp2511 == _temp2412?
_temp2513 == _temp2410: 0)? t:( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp2515=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp2515[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp2516; _temp2516.tag=
Cyc_Absyn_TunionType; _temp2516.f1=({ struct Cyc_Absyn_TunionInfo _temp2517;
_temp2517.tunion_info=( void*) _temp2414; _temp2517.targs= _temp2511; _temp2517.rgn=(
void*) _temp2513; _temp2517;}); _temp2516;}); _temp2515;});}} _LL2359: { struct
Cyc_List_List* _temp2518= Cyc_Tcutil_substs( rgn, inst, _temp2418); goto _LL2519;
_LL2519: return _temp2518 == _temp2418? t:( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp2520=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp2520[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp2521; _temp2521.tag= Cyc_Absyn_TunionFieldType; _temp2521.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2522; _temp2522.field_info=( void*) _temp2420; _temp2522.targs= _temp2518;
_temp2522;}); _temp2521;}); _temp2520;});} _LL2361: { struct Cyc_List_List*
_temp2523= Cyc_Tcutil_substs( rgn, inst, _temp2424); goto _LL2524; _LL2524:
return _temp2523 == _temp2424? t:( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp2525=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2525[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp2526; _temp2526.tag=
Cyc_Absyn_TypedefType; _temp2526.f1= _temp2426; _temp2526.f2= _temp2523;
_temp2526.f3= _temp2422; _temp2526;}); _temp2525;});} _LL2363: { void* _temp2527=
Cyc_Tcutil_rsubstitute( rgn, inst, _temp2432); goto _LL2528; _LL2528: return
_temp2527 == _temp2432? t:( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp2529=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp2529[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp2530; _temp2530.tag= Cyc_Absyn_ArrayType;
_temp2530.f1=( void*) _temp2527; _temp2530.f2= _temp2430; _temp2530.f3=
_temp2428; _temp2530;}); _temp2529;});} _LL2365: { void* _temp2531= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2444); goto _LL2532; _LL2532: { void* _temp2533= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2442); goto _LL2534; _LL2534: if( _temp2531 == _temp2444?
_temp2533 == _temp2442: 0){ return t;} return( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp2535=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp2535[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp2536; _temp2536.tag=
Cyc_Absyn_PointerType; _temp2536.f1=({ struct Cyc_Absyn_PtrInfo _temp2537;
_temp2537.elt_typ=( void*) _temp2531; _temp2537.rgn_typ=( void*) _temp2533;
_temp2537.nullable= _temp2440; _temp2537.tq= _temp2438; _temp2537.bounds=
_temp2436; _temp2537;}); _temp2536;}); _temp2535;});}} _LL2367:{ struct Cyc_List_List*
_temp2538= _temp2462; goto _LL2539; _LL2539: for( 0; _temp2538 != 0; _temp2538=((
struct Cyc_List_List*) _check_null( _temp2538))->tl){ inst=({ struct Cyc_List_List*
_temp2540=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2540->hd=( void*)({ struct _tuple5* _temp2541=( struct _tuple5*)
_region_malloc( rgn, sizeof( struct _tuple5)); _temp2541->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp2538))->hd; _temp2541->f2=( void*)({
struct Cyc_Absyn_VarType_struct* _temp2542=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2542[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2543; _temp2543.tag= Cyc_Absyn_VarType; _temp2543.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp2538))->hd;
_temp2543;}); _temp2542;}); _temp2541;}); _temp2540->tl= inst; _temp2540;});}}{
struct Cyc_List_List* _temp2546; struct Cyc_List_List* _temp2548; struct _tuple0
_temp2544=(( struct _tuple0(*)( struct _RegionHandle* r3, struct _RegionHandle*
r4, struct Cyc_List_List* x)) Cyc_List_rsplit)( rgn, rgn,(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple16*(* f)( struct _RegionHandle*, struct
_tuple2*), struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)(
rgn, Cyc_Tcutil_substitute_f1, rgn, _temp2456)); _LL2549: _temp2548= _temp2544.f1;
goto _LL2547; _LL2547: _temp2546= _temp2544.f2; goto _LL2545; _LL2545: { struct
Cyc_List_List* _temp2550= Cyc_Tcutil_substs( rgn, inst, _temp2546); goto _LL2551;
_LL2551: { struct Cyc_List_List* _temp2552=(( struct Cyc_List_List*(*)( struct
_tuple2*(* f)( struct _tuple16*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct _RegionHandle* r4,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn,
_temp2548, _temp2550)); goto _LL2553; _LL2553: { struct Cyc_Core_Opt* eff2; if(
_temp2460 == 0){ eff2= 0;} else{ void* _temp2554= Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2460))->v); goto _LL2555;
_LL2555: if( _temp2554 ==( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2460))->v){
eff2= _temp2460;} else{ eff2=({ struct Cyc_Core_Opt* _temp2556=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2556->v=( void*) _temp2554;
_temp2556;});}}{ struct Cyc_Absyn_VarargInfo* cyc_varargs2; if( _temp2452 == 0){
cyc_varargs2= 0;} else{ int _temp2559; void* _temp2561; void* _temp2563; struct
Cyc_Absyn_Tqual _temp2565; struct Cyc_Core_Opt* _temp2567; struct Cyc_Absyn_VarargInfo
_temp2557=*(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp2452)); _LL2568:
_temp2567= _temp2557.name; goto _LL2566; _LL2566: _temp2565= _temp2557.tq; goto
_LL2564; _LL2564: _temp2563=( void*) _temp2557.type; goto _LL2562; _LL2562:
_temp2561=( void*) _temp2557.rgn; goto _LL2560; _LL2560: _temp2559= _temp2557.inject;
goto _LL2558; _LL2558: { void* _temp2569= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2563); goto _LL2570; _LL2570: { void* _temp2571= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2561); goto _LL2572; _LL2572: if( _temp2569 == _temp2563?
_temp2561 == _temp2571: 0){ cyc_varargs2= _temp2452;} else{ cyc_varargs2=({
struct Cyc_Absyn_VarargInfo* _temp2573=( struct Cyc_Absyn_VarargInfo*) GC_malloc(
sizeof( struct Cyc_Absyn_VarargInfo)); _temp2573->name= _temp2567; _temp2573->tq=
_temp2565; _temp2573->type=( void*) _temp2569; _temp2573->rgn=( void*) _temp2571;
_temp2573->inject= _temp2559; _temp2573;});}}}}{ struct Cyc_List_List* rgn_po2;
struct Cyc_List_List* _temp2576; struct Cyc_List_List* _temp2578; struct _tuple0
_temp2574=(( struct _tuple0(*)( struct _RegionHandle* r3, struct _RegionHandle*
r4, struct Cyc_List_List* x)) Cyc_List_rsplit)( rgn, rgn, _temp2450); _LL2579:
_temp2578= _temp2574.f1; goto _LL2577; _LL2577: _temp2576= _temp2574.f2; goto
_LL2575; _LL2575: { struct Cyc_List_List* _temp2580= Cyc_Tcutil_substs( rgn,
inst, _temp2578); goto _LL2581; _LL2581: { struct Cyc_List_List* _temp2582= Cyc_Tcutil_substs(
rgn, inst, _temp2576); goto _LL2583; _LL2583: if( _temp2580 == _temp2578?
_temp2582 == _temp2576: 0){ rgn_po2= _temp2450;} else{ rgn_po2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp2580,
_temp2582);} return( void*)({ struct Cyc_Absyn_FnType_struct* _temp2584=( struct
Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2584[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2585; _temp2585.tag= Cyc_Absyn_FnType;
_temp2585.f1=({ struct Cyc_Absyn_FnInfo _temp2586; _temp2586.tvars= _temp2462;
_temp2586.effect= eff2; _temp2586.ret_typ=( void*) Cyc_Tcutil_rsubstitute( rgn,
inst, _temp2458); _temp2586.args= _temp2552; _temp2586.c_varargs= _temp2454;
_temp2586.cyc_varargs= cyc_varargs2; _temp2586.rgn_po= rgn_po2; _temp2586.attributes=
_temp2448; _temp2586;}); _temp2585;}); _temp2584;});}}}}}}}} _LL2369: { struct
Cyc_List_List* _temp2587=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
void*(* f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_snd_tqt,
_temp2464); goto _LL2588; _LL2588: { struct Cyc_List_List* _temp2589= Cyc_Tcutil_substs(
rgn, inst, _temp2587); goto _LL2590; _LL2590: if( _temp2589 == _temp2587){
return t;}{ struct Cyc_List_List* _temp2591=(( struct Cyc_List_List*(*)( struct
_tuple4*(* f)( struct _tuple4*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_map2_tq, _temp2464, _temp2589); goto _LL2592;
_LL2592: return( void*)({ struct Cyc_Absyn_TupleType_struct* _temp2593=( struct
Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp2593[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp2594; _temp2594.tag= Cyc_Absyn_TupleType;
_temp2594.f1= _temp2591; _temp2594;}); _temp2593;});}}} _LL2371: { struct Cyc_List_List*
_temp2595=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2466); goto _LL2596; _LL2596: { struct Cyc_List_List* _temp2597= Cyc_Tcutil_substs(
rgn, inst, _temp2595); goto _LL2598; _LL2598: if( _temp2597 == _temp2595){
return t;}{ struct Cyc_List_List* _temp2599=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type,
_temp2466, _temp2597); goto _LL2600; _LL2600: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp2601=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp2601[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp2602; _temp2602.tag= Cyc_Absyn_AnonStructType; _temp2602.f1= _temp2599;
_temp2602;}); _temp2601;});}}} _LL2373: { struct Cyc_List_List* _temp2603=((
struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type, _temp2468);
goto _LL2604; _LL2604: { struct Cyc_List_List* _temp2605= Cyc_Tcutil_substs( rgn,
inst, _temp2603); goto _LL2606; _LL2606: if( _temp2605 == _temp2603){ return t;}{
struct Cyc_List_List* _temp2607=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2468, _temp2605); goto
_LL2608; _LL2608: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp2609=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp2609[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp2610; _temp2610.tag= Cyc_Absyn_AnonStructType; _temp2610.f1= _temp2607;
_temp2610;}); _temp2609;});}}} _LL2375: if( _temp2474 != 0){ return Cyc_Tcutil_rsubstitute(
rgn, inst,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2474))->v);} else{
return t;} _LL2377: { void* _temp2611= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2478); goto _LL2612; _LL2612: return _temp2611 == _temp2478? t:( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp2613=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp2613[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp2614; _temp2614.tag= Cyc_Absyn_RgnHandleType;
_temp2614.f1=( void*) _temp2611; _temp2614;}); _temp2613;});} _LL2379: return t;
_LL2381: return t; _LL2383: return t; _LL2385: return t; _LL2387: return t;
_LL2389: return t; _LL2391: { void* _temp2615= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2488); goto _LL2616; _LL2616: return _temp2615 == _temp2488? t:( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2617=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2617[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2618; _temp2618.tag= Cyc_Absyn_AccessEff;
_temp2618.f1=( void*) _temp2615; _temp2618;}); _temp2617;});} _LL2393: { struct
Cyc_List_List* _temp2619= Cyc_Tcutil_substs( rgn, inst, _temp2490); goto _LL2620;
_LL2620: return _temp2619 == _temp2490? t:( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp2621=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2621[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2622; _temp2622.tag= Cyc_Absyn_JoinEff;
_temp2622.f1= _temp2619; _temp2622;}); _temp2621;});} _LL2349:;} static struct
Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List*
inst, struct Cyc_List_List* ts){ if( ts == 0){ return 0;}{ void* _temp2623=(
void*)(( struct Cyc_List_List*) _check_null( ts))->hd; goto _LL2624; _LL2624: {
struct Cyc_List_List* _temp2625=(( struct Cyc_List_List*) _check_null( ts))->tl;
goto _LL2626; _LL2626: { void* _temp2627= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2623); goto _LL2628; _LL2628: { struct Cyc_List_List* _temp2629= Cyc_Tcutil_substs(
rgn, inst, _temp2625); goto _LL2630; _LL2630: if( _temp2623 == _temp2627?
_temp2625 == _temp2629: 0){ return ts;} return( struct Cyc_List_List*)({ struct
Cyc_List_List* _temp2631=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2631->hd=( void*) _temp2627; _temp2631->tl= _temp2629; _temp2631;});}}}}}
extern void* Cyc_Tcutil_substitute( struct Cyc_List_List* inst, void* t){ return
Cyc_Tcutil_rsubstitute( Cyc_Core_heap_region, inst, t);} struct Cyc_Absyn_Exp*
Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv* te, void* t, struct Cyc_Position_Segment*
loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2681=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2681[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2682; _temp2682.tag= Cyc_Absyn_Const_e;
_temp2682.f1=( void*)(( void*) Cyc_Absyn_Null_c); _temp2682;}); _temp2681;}),
loc);{ void* _temp2632= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp2646; void* _temp2648; void* _temp2650; void* _temp2652; void* _temp2654;
_LL2634: if(( unsigned int) _temp2632 > 4u?*(( int*) _temp2632) == Cyc_Absyn_PointerType:
0){ _LL2647: _temp2646=(( struct Cyc_Absyn_PointerType_struct*) _temp2632)->f1;
goto _LL2635;} else{ goto _LL2636;} _LL2636: if(( unsigned int) _temp2632 > 4u?*((
int*) _temp2632) == Cyc_Absyn_IntType: 0){ _LL2651: _temp2650=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2632)->f1; goto _LL2649; _LL2649: _temp2648=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2632)->f2; if( _temp2648 ==(
void*) Cyc_Absyn_B1){ goto _LL2637;} else{ goto _LL2638;}} else{ goto _LL2638;}
_LL2638: if(( unsigned int) _temp2632 > 4u?*(( int*) _temp2632) == Cyc_Absyn_IntType:
0){ _LL2655: _temp2654=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2632)->f1;
goto _LL2653; _LL2653: _temp2652=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2632)->f2; goto _LL2639;} else{ goto _LL2640;} _LL2640: if( _temp2632 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2641;} else{ goto _LL2642;} _LL2642: if(
_temp2632 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2643;} else{ goto _LL2644;}
_LL2644: goto _LL2645; _LL2635: goto _LL2633; _LL2637:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2656=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2656[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2657; _temp2657.tag= Cyc_Absyn_Const_e; _temp2657.f1=(
void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp2658=( struct Cyc_Absyn_Char_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct)); _temp2658[ 0]=({ struct Cyc_Absyn_Char_c_struct
_temp2659; _temp2659.tag= Cyc_Absyn_Char_c; _temp2659.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp2659.f2='\000'; _temp2659;}); _temp2658;})); _temp2657;}); _temp2656;})));
goto _LL2633; _LL2639:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2660=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2660[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2661; _temp2661.tag= Cyc_Absyn_Const_e;
_temp2661.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp2662=(
struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp2662[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp2663; _temp2663.tag= Cyc_Absyn_Int_c;
_temp2663.f1=( void*) _temp2654; _temp2663.f2= 0; _temp2663;}); _temp2662;}));
_temp2661;}); _temp2660;}))); if( _temp2652 !=( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2664=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2664[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2665; _temp2665.tag= Cyc_Absyn_Cast_e; _temp2665.f1=( void*) t; _temp2665.f2=
e; _temp2665;}); _temp2664;}), loc);} goto _LL2633; _LL2641:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2666=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2666[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2667; _temp2667.tag= Cyc_Absyn_Const_e; _temp2667.f1=(
void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2668=( struct Cyc_Absyn_Float_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct)); _temp2668[ 0]=({ struct
Cyc_Absyn_Float_c_struct _temp2669; _temp2669.tag= Cyc_Absyn_Float_c; _temp2669.f1=({
unsigned char* _temp2670=( unsigned char*)"0.0"; struct _tagged_string _temp2671;
_temp2671.curr= _temp2670; _temp2671.base= _temp2670; _temp2671.last_plus_one=
_temp2670 + 4; _temp2671;}); _temp2669;}); _temp2668;})); _temp2667;});
_temp2666;}))); goto _LL2633; _LL2643:( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_Cast_e_struct* _temp2672=( struct Cyc_Absyn_Cast_e_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2672[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2673; _temp2673.tag= Cyc_Absyn_Cast_e; _temp2673.f1=( void*) t; _temp2673.f2=
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2674=( struct
Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2674[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2675; _temp2675.tag= Cyc_Absyn_Const_e;
_temp2675.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2676=(
struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2676[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2677; _temp2677.tag= Cyc_Absyn_Float_c;
_temp2677.f1=({ unsigned char* _temp2678=( unsigned char*)"0.0"; struct
_tagged_string _temp2679; _temp2679.curr= _temp2678; _temp2679.base= _temp2678;
_temp2679.last_plus_one= _temp2678 + 4; _temp2679;}); _temp2677;}); _temp2676;}));
_temp2675;}); _temp2674;}), loc); _temp2673;}); _temp2672;}))); goto _LL2633;
_LL2645: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string
_temp2680= Cyc_Absynpp_typ2string( t); xprintf("declaration of type %.*s requires initializer",
_temp2680.last_plus_one - _temp2680.curr, _temp2680.curr);})); goto _LL2633;
_LL2633:;} return e;} struct _tuple5* Cyc_Tcutil_make_inst_var( struct Cyc_List_List*
s, struct Cyc_Absyn_Tvar* tv){ void* k=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( tv->kind); return({ struct _tuple5* _temp2683=( struct
_tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp2683->f1= tv; _temp2683->f2=
Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2684=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2684->v=( void*) k; _temp2684;}),({
struct Cyc_Core_Opt* _temp2685=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2685->v=( void*) s; _temp2685;})); _temp2683;});} struct
_tuple5* Cyc_Tcutil_r_make_inst_var( struct _tuple6* env, struct Cyc_Absyn_Tvar*
tv){ struct _tuple6 _temp2688; struct _RegionHandle* _temp2689; struct Cyc_List_List*
_temp2691; struct _tuple6* _temp2686= env; _temp2688=* _temp2686; _LL2692:
_temp2691= _temp2688.f1; goto _LL2690; _LL2690: _temp2689= _temp2688.f2; goto
_LL2687; _LL2687: { void* k=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
tv->kind); return({ struct _tuple5* _temp2693=( struct _tuple5*) _region_malloc(
_temp2689, sizeof( struct _tuple5)); _temp2693->f1= tv; _temp2693->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp2694=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2694->v=( void*) k; _temp2694;}),({ struct Cyc_Core_Opt*
_temp2695=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2695->v=( void*) _temp2691; _temp2695;})); _temp2693;});}} static struct
Cyc_List_List* Cyc_Tcutil_add_free_tvar( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 != 0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
if( Cyc_String_zstrptrcmp((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->name, tv->name) == 0){ struct Cyc_Absyn_Conref* k1=((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->kind;
struct Cyc_Absyn_Conref* k2= tv->kind; if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string _temp2696=*
tv->name; struct _tagged_string _temp2697= Cyc_Absynpp_ckind2string( k1); struct
_tagged_string _temp2698= Cyc_Absynpp_ckind2string( k2); xprintf("type variable %.*s is used with inconsistent kinds %.*s and %.*s",
_temp2696.last_plus_one - _temp2696.curr, _temp2696.curr, _temp2697.last_plus_one
- _temp2697.curr, _temp2697.curr, _temp2698.last_plus_one - _temp2698.curr,
_temp2698.curr);}));} if( tv->identity == 0){ tv->identity=(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->identity;} else{ if(*(( int*)
_check_null( tv->identity)) !=*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->identity))){(( void(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string) xprintf("same type variable has different identity!"));}}
return tvs;}}} tv->identity= Cyc_Tcutil_new_tvar_id(); return({ struct Cyc_List_List*
_temp2699=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2699->hd=( void*) tv; _temp2699->tl= tvs; _temp2699;});} static struct Cyc_List_List*
Cyc_Tcutil_fast_add_free_tvar( struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar*
tv){ if( tv->identity == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
unsigned char* _temp2700=( unsigned char*)"fast_add_free_tvar: bad identity in tv";
struct _tagged_string _temp2701; _temp2701.curr= _temp2700; _temp2701.base=
_temp2700; _temp2701.last_plus_one= _temp2700 + 39; _temp2701;}));}{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 != 0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
if((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->identity
== 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char*
_temp2702=( unsigned char*)"fast_add_free_tvar: bad identity in tvs2"; struct
_tagged_string _temp2703; _temp2703.curr= _temp2702; _temp2703.base= _temp2702;
_temp2703.last_plus_one= _temp2702 + 41; _temp2703;}));} if(*(( int*)
_check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
tvs2))->hd)->identity)) ==*(( int*) _check_null( tv->identity))){ return tvs;}}}
return({ struct Cyc_List_List* _temp2704=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2704->hd=( void*) tv; _temp2704->tl= tvs;
_temp2704;});} static struct Cyc_List_List* Cyc_Tcutil_add_bound_tvar( struct
Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){ if( tv->identity == 0){(( void(*)(
struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({ struct
_tagged_string _temp2705= Cyc_Tcutil_tvar2string( tv); xprintf("bound tvar id for %.*s is null",
_temp2705.last_plus_one - _temp2705.curr, _temp2705.curr);}));} return({ struct
Cyc_List_List* _temp2706=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2706->hd=( void*) tv; _temp2706->tl= tvs; _temp2706;});} static struct Cyc_List_List*
Cyc_Tcutil_add_free_evar( struct Cyc_List_List* es, void* e){ void* _temp2707=
Cyc_Tcutil_compress( e); struct Cyc_Core_Opt* _temp2713; int _temp2715; struct
Cyc_Core_Opt* _temp2717; struct Cyc_Core_Opt* _temp2719; _LL2709: if((
unsigned int) _temp2707 > 4u?*(( int*) _temp2707) == Cyc_Absyn_Evar: 0){ _LL2720:
_temp2719=(( struct Cyc_Absyn_Evar_struct*) _temp2707)->f1; goto _LL2718;
_LL2718: _temp2717=(( struct Cyc_Absyn_Evar_struct*) _temp2707)->f2; goto
_LL2716; _LL2716: _temp2715=(( struct Cyc_Absyn_Evar_struct*) _temp2707)->f3;
goto _LL2714; _LL2714: _temp2713=(( struct Cyc_Absyn_Evar_struct*) _temp2707)->f4;
goto _LL2710;} else{ goto _LL2711;} _LL2711: goto _LL2712; _LL2710:{ struct Cyc_List_List*
es2= es; for( 0; es2 != 0; es2=(( struct Cyc_List_List*) _check_null( es2))->tl){
void* _temp2721= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*)
_check_null( es2))->hd); struct Cyc_Core_Opt* _temp2727; int _temp2729; struct
Cyc_Core_Opt* _temp2731; struct Cyc_Core_Opt* _temp2733; _LL2723: if((
unsigned int) _temp2721 > 4u?*(( int*) _temp2721) == Cyc_Absyn_Evar: 0){ _LL2734:
_temp2733=(( struct Cyc_Absyn_Evar_struct*) _temp2721)->f1; goto _LL2732;
_LL2732: _temp2731=(( struct Cyc_Absyn_Evar_struct*) _temp2721)->f2; goto
_LL2730; _LL2730: _temp2729=(( struct Cyc_Absyn_Evar_struct*) _temp2721)->f3;
goto _LL2728; _LL2728: _temp2727=(( struct Cyc_Absyn_Evar_struct*) _temp2721)->f4;
goto _LL2724;} else{ goto _LL2725;} _LL2725: goto _LL2726; _LL2724: if(
_temp2715 == _temp2729){ return es;} goto _LL2722; _LL2726: goto _LL2722;
_LL2722:;}} return({ struct Cyc_List_List* _temp2735=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2735->hd=( void*) e; _temp2735->tl=
es; _temp2735;}); _LL2712: return es; _LL2708:;} static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars( struct Cyc_List_List* tvs, struct Cyc_List_List*
btvs){ struct Cyc_List_List* r= 0; for( 0; tvs != 0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){ int present= 0;{ struct Cyc_List_List* b= btvs; for( 0;
b != 0; b=(( struct Cyc_List_List*) _check_null( b))->tl){ if(*(( int*)
_check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->identity))
==*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( b))->hd)->identity))){ present= 1; break;}}} if( ! present){ r=({
struct Cyc_List_List* _temp2736=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2736->hd=( void*)(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd); _temp2736->tl= r; _temp2736;});}}
r=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( r);
return r;} struct Cyc_Tcutil_CVTEnv{ struct Cyc_List_List* kind_env; struct Cyc_List_List*
free_vars; struct Cyc_List_List* free_evars; } ; static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcutil_CVTEnv cvtenv, void* expected_kind, void* t){{ void*
_temp2737= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt* _temp2783; int
_temp2785; struct Cyc_Core_Opt* _temp2787; struct Cyc_Core_Opt* _temp2789;
struct Cyc_Core_Opt** _temp2791; struct Cyc_Absyn_Tvar* _temp2792; struct Cyc_Absyn_Enumdecl*
_temp2794; struct Cyc_Absyn_Enumdecl** _temp2796; struct _tuple1* _temp2797;
struct Cyc_Absyn_TunionInfo _temp2799; void* _temp2801; struct Cyc_List_List*
_temp2803; void* _temp2805; void** _temp2807; struct Cyc_Absyn_TunionFieldInfo
_temp2808; struct Cyc_List_List* _temp2810; void* _temp2812; void** _temp2814;
struct Cyc_Absyn_PtrInfo _temp2815; struct Cyc_Absyn_Conref* _temp2817; struct
Cyc_Absyn_Tqual _temp2819; struct Cyc_Absyn_Conref* _temp2821; void* _temp2823;
void* _temp2825; void* _temp2827; void* _temp2829; struct Cyc_Absyn_Exp*
_temp2831; struct Cyc_Absyn_Tqual _temp2833; void* _temp2835; struct Cyc_Absyn_FnInfo
_temp2837; struct Cyc_List_List* _temp2839; struct Cyc_List_List* _temp2841;
struct Cyc_Absyn_VarargInfo* _temp2843; int _temp2845; struct Cyc_List_List*
_temp2847; void* _temp2849; struct Cyc_Core_Opt* _temp2851; struct Cyc_Core_Opt**
_temp2853; struct Cyc_List_List* _temp2854; struct Cyc_List_List** _temp2856;
struct Cyc_List_List* _temp2857; struct Cyc_List_List* _temp2859; struct Cyc_List_List*
_temp2861; struct Cyc_Absyn_Structdecl** _temp2863; struct Cyc_Absyn_Structdecl***
_temp2865; struct Cyc_List_List* _temp2866; struct _tuple1* _temp2868; struct
Cyc_Absyn_Uniondecl** _temp2870; struct Cyc_Absyn_Uniondecl*** _temp2872; struct
Cyc_List_List* _temp2873; struct _tuple1* _temp2875; struct Cyc_Core_Opt*
_temp2877; struct Cyc_Core_Opt** _temp2879; struct Cyc_List_List* _temp2880;
struct _tuple1* _temp2882; void* _temp2884; void* _temp2886; struct Cyc_List_List*
_temp2888; _LL2739: if( _temp2737 ==( void*) Cyc_Absyn_VoidType){ goto _LL2740;}
else{ goto _LL2741;} _LL2741: if(( unsigned int) _temp2737 > 4u?*(( int*)
_temp2737) == Cyc_Absyn_Evar: 0){ _LL2790: _temp2789=(( struct Cyc_Absyn_Evar_struct*)
_temp2737)->f1; _temp2791=&(( struct Cyc_Absyn_Evar_struct*) _temp2737)->f1;
goto _LL2788; _LL2788: _temp2787=(( struct Cyc_Absyn_Evar_struct*) _temp2737)->f2;
goto _LL2786; _LL2786: _temp2785=(( struct Cyc_Absyn_Evar_struct*) _temp2737)->f3;
goto _LL2784; _LL2784: _temp2783=(( struct Cyc_Absyn_Evar_struct*) _temp2737)->f4;
goto _LL2742;} else{ goto _LL2743;} _LL2743: if(( unsigned int) _temp2737 > 4u?*((
int*) _temp2737) == Cyc_Absyn_VarType: 0){ _LL2793: _temp2792=(( struct Cyc_Absyn_VarType_struct*)
_temp2737)->f1; goto _LL2744;} else{ goto _LL2745;} _LL2745: if(( unsigned int)
_temp2737 > 4u?*(( int*) _temp2737) == Cyc_Absyn_EnumType: 0){ _LL2798:
_temp2797=(( struct Cyc_Absyn_EnumType_struct*) _temp2737)->f1; goto _LL2795;
_LL2795: _temp2794=(( struct Cyc_Absyn_EnumType_struct*) _temp2737)->f2;
_temp2796=&(( struct Cyc_Absyn_EnumType_struct*) _temp2737)->f2; goto _LL2746;}
else{ goto _LL2747;} _LL2747: if(( unsigned int) _temp2737 > 4u?*(( int*)
_temp2737) == Cyc_Absyn_TunionType: 0){ _LL2800: _temp2799=(( struct Cyc_Absyn_TunionType_struct*)
_temp2737)->f1; _LL2806: _temp2805=( void*) _temp2799.tunion_info; _temp2807=&(((
struct Cyc_Absyn_TunionType_struct*) _temp2737)->f1).tunion_info; goto _LL2804;
_LL2804: _temp2803= _temp2799.targs; goto _LL2802; _LL2802: _temp2801=( void*)
_temp2799.rgn; goto _LL2748;} else{ goto _LL2749;} _LL2749: if(( unsigned int)
_temp2737 > 4u?*(( int*) _temp2737) == Cyc_Absyn_TunionFieldType: 0){ _LL2809:
_temp2808=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2737)->f1; _LL2813:
_temp2812=( void*) _temp2808.field_info; _temp2814=&((( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2737)->f1).field_info; goto _LL2811; _LL2811: _temp2810= _temp2808.targs;
goto _LL2750;} else{ goto _LL2751;} _LL2751: if(( unsigned int) _temp2737 > 4u?*((
int*) _temp2737) == Cyc_Absyn_PointerType: 0){ _LL2816: _temp2815=(( struct Cyc_Absyn_PointerType_struct*)
_temp2737)->f1; _LL2826: _temp2825=( void*) _temp2815.elt_typ; goto _LL2824;
_LL2824: _temp2823=( void*) _temp2815.rgn_typ; goto _LL2822; _LL2822: _temp2821=
_temp2815.nullable; goto _LL2820; _LL2820: _temp2819= _temp2815.tq; goto _LL2818;
_LL2818: _temp2817= _temp2815.bounds; goto _LL2752;} else{ goto _LL2753;}
_LL2753: if(( unsigned int) _temp2737 > 4u?*(( int*) _temp2737) == Cyc_Absyn_IntType:
0){ _LL2830: _temp2829=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2737)->f1;
goto _LL2828; _LL2828: _temp2827=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2737)->f2; goto _LL2754;} else{ goto _LL2755;} _LL2755: if( _temp2737 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2756;} else{ goto _LL2757;} _LL2757: if(
_temp2737 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2758;} else{ goto _LL2759;}
_LL2759: if(( unsigned int) _temp2737 > 4u?*(( int*) _temp2737) == Cyc_Absyn_ArrayType:
0){ _LL2836: _temp2835=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2737)->f1;
goto _LL2834; _LL2834: _temp2833=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2737)->f2; goto _LL2832; _LL2832: _temp2831=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2737)->f3; goto _LL2760;} else{ goto _LL2761;} _LL2761: if(( unsigned int)
_temp2737 > 4u?*(( int*) _temp2737) == Cyc_Absyn_FnType: 0){ _LL2838: _temp2837=((
struct Cyc_Absyn_FnType_struct*) _temp2737)->f1; _LL2855: _temp2854= _temp2837.tvars;
_temp2856=&((( struct Cyc_Absyn_FnType_struct*) _temp2737)->f1).tvars; goto
_LL2852; _LL2852: _temp2851= _temp2837.effect; _temp2853=&((( struct Cyc_Absyn_FnType_struct*)
_temp2737)->f1).effect; goto _LL2850; _LL2850: _temp2849=( void*) _temp2837.ret_typ;
goto _LL2848; _LL2848: _temp2847= _temp2837.args; goto _LL2846; _LL2846:
_temp2845= _temp2837.c_varargs; goto _LL2844; _LL2844: _temp2843= _temp2837.cyc_varargs;
goto _LL2842; _LL2842: _temp2841= _temp2837.rgn_po; goto _LL2840; _LL2840:
_temp2839= _temp2837.attributes; goto _LL2762;} else{ goto _LL2763;} _LL2763:
if(( unsigned int) _temp2737 > 4u?*(( int*) _temp2737) == Cyc_Absyn_TupleType: 0){
_LL2858: _temp2857=(( struct Cyc_Absyn_TupleType_struct*) _temp2737)->f1; goto
_LL2764;} else{ goto _LL2765;} _LL2765: if(( unsigned int) _temp2737 > 4u?*((
int*) _temp2737) == Cyc_Absyn_AnonStructType: 0){ _LL2860: _temp2859=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp2737)->f1; goto _LL2766;} else{ goto
_LL2767;} _LL2767: if(( unsigned int) _temp2737 > 4u?*(( int*) _temp2737) == Cyc_Absyn_AnonUnionType:
0){ _LL2862: _temp2861=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp2737)->f1;
goto _LL2768;} else{ goto _LL2769;} _LL2769: if(( unsigned int) _temp2737 > 4u?*((
int*) _temp2737) == Cyc_Absyn_StructType: 0){ _LL2869: _temp2868=(( struct Cyc_Absyn_StructType_struct*)
_temp2737)->f1; goto _LL2867; _LL2867: _temp2866=(( struct Cyc_Absyn_StructType_struct*)
_temp2737)->f2; goto _LL2864; _LL2864: _temp2863=(( struct Cyc_Absyn_StructType_struct*)
_temp2737)->f3; _temp2865=&(( struct Cyc_Absyn_StructType_struct*) _temp2737)->f3;
goto _LL2770;} else{ goto _LL2771;} _LL2771: if(( unsigned int) _temp2737 > 4u?*((
int*) _temp2737) == Cyc_Absyn_UnionType: 0){ _LL2876: _temp2875=(( struct Cyc_Absyn_UnionType_struct*)
_temp2737)->f1; goto _LL2874; _LL2874: _temp2873=(( struct Cyc_Absyn_UnionType_struct*)
_temp2737)->f2; goto _LL2871; _LL2871: _temp2870=(( struct Cyc_Absyn_UnionType_struct*)
_temp2737)->f3; _temp2872=&(( struct Cyc_Absyn_UnionType_struct*) _temp2737)->f3;
goto _LL2772;} else{ goto _LL2773;} _LL2773: if(( unsigned int) _temp2737 > 4u?*((
int*) _temp2737) == Cyc_Absyn_TypedefType: 0){ _LL2883: _temp2882=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2737)->f1; goto _LL2881; _LL2881: _temp2880=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2737)->f2; goto _LL2878; _LL2878: _temp2877=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2737)->f3; _temp2879=&(( struct Cyc_Absyn_TypedefType_struct*) _temp2737)->f3;
goto _LL2774;} else{ goto _LL2775;} _LL2775: if( _temp2737 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL2776;} else{ goto _LL2777;} _LL2777: if(( unsigned int) _temp2737 > 4u?*((
int*) _temp2737) == Cyc_Absyn_RgnHandleType: 0){ _LL2885: _temp2884=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp2737)->f1; goto _LL2778;} else{
goto _LL2779;} _LL2779: if(( unsigned int) _temp2737 > 4u?*(( int*) _temp2737)
== Cyc_Absyn_AccessEff: 0){ _LL2887: _temp2886=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp2737)->f1; goto _LL2780;} else{ goto _LL2781;} _LL2781: if(( unsigned int)
_temp2737 > 4u?*(( int*) _temp2737) == Cyc_Absyn_JoinEff: 0){ _LL2889: _temp2888=((
struct Cyc_Absyn_JoinEff_struct*) _temp2737)->f1; goto _LL2782;} else{ goto
_LL2738;} _LL2740: goto _LL2738; _LL2742: if(* _temp2791 == 0){* _temp2791=({
struct Cyc_Core_Opt* _temp2890=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2890->v=( void*) expected_kind; _temp2890;});} cvtenv.free_evars=
Cyc_Tcutil_add_free_evar( cvtenv.free_evars, t); goto _LL2738; _LL2744: { struct
Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp2792->kind); if(( void*) c->v ==( void*) Cyc_Absyn_No_constr){(
void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2891=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2891[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2892; _temp2892.tag= Cyc_Absyn_Eq_constr;
_temp2892.f1=( void*) expected_kind; _temp2892;}); _temp2891;})));} cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar( loc, cvtenv.kind_env, _temp2792); cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars, _temp2792); goto _LL2738;}
_LL2746: if(* _temp2796 == 0){ struct _handler_cons _temp2893; _push_handler(&
_temp2893);{ int _temp2895= 0; if( setjmp( _temp2893.handler)){ _temp2895= 1;}
if( ! _temp2895){{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl( te,
loc, _temp2797);* _temp2796=( struct Cyc_Absyn_Enumdecl*)* ed;}; _pop_handler();}
else{ void* _temp2894=( void*) _exn_thrown; void* _temp2897= _temp2894; _LL2899:
if( _temp2897 == Cyc_Dict_Absent){ goto _LL2900;} else{ goto _LL2901;} _LL2901:
goto _LL2902; _LL2900: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp2903= Cyc_Absynpp_qvar2string( _temp2797); xprintf("unbound type enum %.*s",
_temp2903.last_plus_one - _temp2903.curr, _temp2903.curr);})); return cvtenv;
_LL2902:( void) _throw( _temp2897); _LL2898:;}}}{ struct Cyc_Absyn_Enumdecl* ed=(
struct Cyc_Absyn_Enumdecl*) _check_null(* _temp2796);* _temp2797=( ed->name)[
_check_known_subscript_notnull( 1u, 0)]; goto _LL2738;} _LL2748:{ void*
_temp2904=* _temp2807; struct Cyc_Absyn_UnknownTunionInfo _temp2910; int
_temp2912; struct _tuple1* _temp2914; struct Cyc_Absyn_Tuniondecl* _temp2916;
_LL2906: if(*(( int*) _temp2904) == Cyc_Absyn_UnknownTunion){ _LL2911: _temp2910=((
struct Cyc_Absyn_UnknownTunion_struct*) _temp2904)->f1; _LL2915: _temp2914=
_temp2910.name; goto _LL2913; _LL2913: _temp2912= _temp2910.is_xtunion; goto
_LL2907;} else{ goto _LL2908;} _LL2908: if(*(( int*) _temp2904) == Cyc_Absyn_KnownTunion){
_LL2917: _temp2916=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2904)->f1; goto
_LL2909;} else{ goto _LL2905;} _LL2907: { struct Cyc_Absyn_Tuniondecl* tud;{
struct _handler_cons _temp2918; _push_handler(& _temp2918);{ int _temp2920= 0;
if( setjmp( _temp2918.handler)){ _temp2920= 1;} if( ! _temp2920){ tud=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp2914);; _pop_handler();} else{ void* _temp2919=( void*)
_exn_thrown; void* _temp2922= _temp2919; _LL2924: if( _temp2922 == Cyc_Dict_Absent){
goto _LL2925;} else{ goto _LL2926;} _LL2926: goto _LL2927; _LL2925: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ struct _tagged_string _temp2928= Cyc_Absynpp_qvar2string(
_temp2914); xprintf("unbound type [x]tunion %.*s", _temp2928.last_plus_one -
_temp2928.curr, _temp2928.curr);})); return cvtenv; _LL2927:( void) _throw(
_temp2922); _LL2923:;}}} if( tud->is_xtunion != _temp2912){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp2929= Cyc_Absynpp_qvar2string(
_temp2914); xprintf("[x]tunion is different from type declaration %.*s",
_temp2929.last_plus_one - _temp2929.curr, _temp2929.curr);}));}* _temp2807=(
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp2930=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp2930[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp2931; _temp2931.tag= Cyc_Absyn_KnownTunion;
_temp2931.f1= tud; _temp2931;}); _temp2930;}); _temp2916= tud; goto _LL2909;}
_LL2909: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2801);{ struct Cyc_List_List* tvs= _temp2916->tvs; for( 0; _temp2803 != 0?
tvs != 0: 0;( _temp2803=(( struct Cyc_List_List*) _check_null( _temp2803))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2803))->hd; void* k1=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} if( _temp2803 != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp2932= Cyc_Absynpp_qvar2string(
_temp2916->name); xprintf("too many type arguments for tunion %.*s", _temp2932.last_plus_one
- _temp2932.curr, _temp2932.curr);}));} if( tvs != 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp2933= Cyc_Absynpp_qvar2string(
_temp2916->name); xprintf("too few type arguments for tunion %.*s", _temp2933.last_plus_one
- _temp2933.curr, _temp2933.curr);}));} goto _LL2905;} _LL2905:;} goto _LL2738;
_LL2750:{ void* _temp2934=* _temp2814; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp2940; int _temp2942; struct _tuple1* _temp2944; struct _tuple1* _temp2946;
struct Cyc_Absyn_Tunionfield* _temp2948; struct Cyc_Absyn_Tuniondecl* _temp2950;
_LL2936: if(*(( int*) _temp2934) == Cyc_Absyn_UnknownTunionfield){ _LL2941:
_temp2940=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp2934)->f1;
_LL2947: _temp2946= _temp2940.tunion_name; goto _LL2945; _LL2945: _temp2944=
_temp2940.field_name; goto _LL2943; _LL2943: _temp2942= _temp2940.is_xtunion;
goto _LL2937;} else{ goto _LL2938;} _LL2938: if(*(( int*) _temp2934) == Cyc_Absyn_KnownTunionfield){
_LL2951: _temp2950=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp2934)->f1;
goto _LL2949; _LL2949: _temp2948=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2934)->f2; goto _LL2939;} else{ goto _LL2935;} _LL2937: { struct Cyc_Absyn_Tuniondecl*
tud; struct Cyc_Absyn_Tunionfield* tuf;{ struct _handler_cons _temp2952;
_push_handler(& _temp2952);{ int _temp2954= 0; if( setjmp( _temp2952.handler)){
_temp2954= 1;} if( ! _temp2954){* Cyc_Tcenv_lookup_tuniondecl( te, loc,
_temp2946);; _pop_handler();} else{ void* _temp2953=( void*) _exn_thrown; void*
_temp2956= _temp2953; _LL2958: if( _temp2956 == Cyc_Dict_Absent){ goto _LL2959;}
else{ goto _LL2960;} _LL2960: goto _LL2961; _LL2959: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp2962= Cyc_Absynpp_qvar2string(
_temp2946); xprintf("unbound type tunion %.*s", _temp2962.last_plus_one -
_temp2962.curr, _temp2962.curr);})); return cvtenv; _LL2961:( void) _throw(
_temp2956); _LL2957:;}}}{ struct _handler_cons _temp2963; _push_handler(&
_temp2963);{ int _temp2965= 0; if( setjmp( _temp2963.handler)){ _temp2965= 1;}
if( ! _temp2965){{ void* _temp2966= Cyc_Tcenv_lookup_ordinary( te, loc,
_temp2944); struct Cyc_Absyn_Tunionfield* _temp2972; struct Cyc_Absyn_Tuniondecl*
_temp2974; _LL2968: if(*(( int*) _temp2966) == Cyc_Tcenv_TunionRes){ _LL2975:
_temp2974=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2966)->f1; goto _LL2973;
_LL2973: _temp2972=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2966)->f2; goto
_LL2969;} else{ goto _LL2970;} _LL2970: goto _LL2971; _LL2969: tuf= _temp2972;
tud= _temp2974; if( tud->is_xtunion != _temp2942){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp2976= Cyc_Absynpp_qvar2string(
_temp2946); xprintf("[x]tunion is different from type declaration %.*s",
_temp2976.last_plus_one - _temp2976.curr, _temp2976.curr);}));} goto _LL2967;
_LL2971: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string
_temp2977= Cyc_Absynpp_qvar2string( _temp2944); struct _tagged_string _temp2978=
Cyc_Absynpp_qvar2string( _temp2946); xprintf("unbound field %.*s in type tunion %.*s",
_temp2977.last_plus_one - _temp2977.curr, _temp2977.curr, _temp2978.last_plus_one
- _temp2978.curr, _temp2978.curr);}));{ struct Cyc_Tcutil_CVTEnv _temp2979=
cvtenv; _npop_handler( 0u); return _temp2979;} _LL2967:;}; _pop_handler();}
else{ void* _temp2964=( void*) _exn_thrown; void* _temp2981= _temp2964; _LL2983:
if( _temp2981 == Cyc_Dict_Absent){ goto _LL2984;} else{ goto _LL2985;} _LL2985:
goto _LL2986; _LL2984: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp2987= Cyc_Absynpp_qvar2string( _temp2944); struct
_tagged_string _temp2988= Cyc_Absynpp_qvar2string( _temp2946); xprintf("unbound field %.*s in type tunion %.*s",
_temp2987.last_plus_one - _temp2987.curr, _temp2987.curr, _temp2988.last_plus_one
- _temp2988.curr, _temp2988.curr);})); return cvtenv; _LL2986:( void) _throw(
_temp2981); _LL2982:;}}}* _temp2814=( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp2989=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp2989[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp2990; _temp2990.tag= Cyc_Absyn_KnownTunionfield; _temp2990.f1= tud;
_temp2990.f2= tuf; _temp2990;}); _temp2989;}); _temp2950= tud; _temp2948= tuf;
goto _LL2939;} _LL2939: { struct Cyc_List_List* tvs= _temp2950->tvs; for( 0;
_temp2810 != 0? tvs != 0: 0;( _temp2810=(( struct Cyc_List_List*) _check_null(
_temp2810))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2810))->hd; void* k1=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} if( _temp2810 != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp2991= Cyc_Absynpp_qvar2string(
_temp2950->name); struct _tagged_string _temp2992= Cyc_Absynpp_qvar2string(
_temp2948->name); xprintf("too many type arguments for tunion %.*s.%.*s",
_temp2991.last_plus_one - _temp2991.curr, _temp2991.curr, _temp2992.last_plus_one
- _temp2992.curr, _temp2992.curr);}));} if( tvs != 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp2993= Cyc_Absynpp_qvar2string(
_temp2950->name); struct _tagged_string _temp2994= Cyc_Absynpp_qvar2string(
_temp2948->name); xprintf("too few type arguments for tunion %.*s.%.*s",
_temp2993.last_plus_one - _temp2993.curr, _temp2993.curr, _temp2994.last_plus_one
- _temp2994.curr, _temp2994.curr);}));} goto _LL2935;} _LL2935:;} goto _LL2738;
_LL2752: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_AnyKind,
_temp2825); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2823);{ void* _temp2995=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp2817))->v; void* _temp3001; struct Cyc_Absyn_Exp*
_temp3003; _LL2997: if(( unsigned int) _temp2995 > 1u?*(( int*) _temp2995) ==
Cyc_Absyn_Eq_constr: 0){ _LL3002: _temp3001=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2995)->f1; if(( unsigned int) _temp3001 > 1u?*(( int*) _temp3001) == Cyc_Absyn_Upper_b:
0){ _LL3004: _temp3003=(( struct Cyc_Absyn_Upper_b_struct*) _temp3001)->f1; goto
_LL2998;} else{ goto _LL2999;}} else{ goto _LL2999;} _LL2999: goto _LL3000;
_LL2998: if( ! Cyc_Tcutil_is_const_exp( te, _temp3003)){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp3005=( unsigned char*)"pointer bounds expression is not a constant";
struct _tagged_string _temp3006; _temp3006.curr= _temp3005; _temp3006.base=
_temp3005; _temp3006.last_plus_one= _temp3005 + 44; _temp3006;}));} Cyc_Tcexp_tcExp(
te, 0, _temp3003); if( ! Cyc_Tcutil_coerce_uint_typ( te, _temp3003)){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp3007=( unsigned char*)"pointer bounds expression is not an unsigned int";
struct _tagged_string _temp3008; _temp3008.curr= _temp3007; _temp3008.base=
_temp3007; _temp3008.last_plus_one= _temp3007 + 49; _temp3008;}));} Cyc_Evexp_eval_const_uint_exp(
_temp3003); goto _LL2996; _LL3000: goto _LL2996; _LL2996:;} goto _LL2738;
_LL2754: goto _LL2738; _LL2756: goto _LL2738; _LL2758: goto _LL2738; _LL2760:
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,
_temp2835); if( _temp2831 == 0? 1: ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp2831))){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp3009=(
unsigned char*)"array bounds expression is not constant"; struct _tagged_string
_temp3010; _temp3010.curr= _temp3009; _temp3010.base= _temp3009; _temp3010.last_plus_one=
_temp3009 + 40; _temp3010;}));} Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)
_check_null( _temp2831)); if( ! Cyc_Tcutil_coerce_uint_typ( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp2831))){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp3011=(
unsigned char*)"array bounds expression is not an unsigned int"; struct
_tagged_string _temp3012; _temp3012.curr= _temp3011; _temp3012.base= _temp3011;
_temp3012.last_plus_one= _temp3011 + 47; _temp3012;}));} Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2831)); goto _LL2738; _LL2762: { int
num_convs= 0; int seen_cdecl= 0; int seen_stdcall= 0; int seen_fastcall= 0; for(
0; _temp2839 != 0; _temp2839=(( struct Cyc_List_List*) _check_null( _temp2839))->tl){
if( ! Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*) _check_null(
_temp2839))->hd)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp3013= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null( _temp2839))->hd); xprintf("bad function type attribute %.*s",
_temp3013.last_plus_one - _temp3013.curr, _temp3013.curr);}));}{ void* _temp3014=(
void*)(( struct Cyc_List_List*) _check_null( _temp2839))->hd; _LL3016: if(
_temp3014 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL3017;} else{ goto _LL3018;}
_LL3018: if( _temp3014 ==( void*) Cyc_Absyn_Cdecl_att){ goto _LL3019;} else{
goto _LL3020;} _LL3020: if( _temp3014 ==( void*) Cyc_Absyn_Fastcall_att){ goto
_LL3021;} else{ goto _LL3022;} _LL3022: goto _LL3023; _LL3017: if( !
seen_stdcall){ seen_stdcall= 1; ++ num_convs;} goto _LL3015; _LL3019: if( !
seen_cdecl){ seen_cdecl= 1; ++ num_convs;} goto _LL3015; _LL3021: if( !
seen_fastcall){ seen_fastcall= 1; ++ num_convs;} goto _LL3015; _LL3023: goto
_LL3015; _LL3015:;}} if( num_convs > 1){ Cyc_Tcutil_terr( loc,( struct
_tagged_string) xprintf("function can't have multiple calling conventions"));}
Cyc_Tcutil_check_unique_tvars( loc,* _temp2856);{ struct Cyc_List_List* b=*
_temp2856; for( 0; b != 0; b=(( struct Cyc_List_List*) _check_null( b))->tl){((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd)->identity=
Cyc_Tcutil_new_tvar_id(); cvtenv.kind_env= Cyc_Tcutil_add_bound_tvar( cvtenv.kind_env,(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd);{ void*
_temp3024=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( b))->hd)->kind))->v; void* _temp3030; _LL3026: if(( unsigned int)
_temp3024 > 1u?*(( int*) _temp3024) == Cyc_Absyn_Eq_constr: 0){ _LL3031:
_temp3030=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3024)->f1; if(
_temp3030 ==( void*) Cyc_Absyn_MemKind){ goto _LL3027;} else{ goto _LL3028;}}
else{ goto _LL3028;} _LL3028: goto _LL3029; _LL3027: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp3032=*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( b))->hd)->name; xprintf("function attempts to abstract Mem type variable %.*s",
_temp3032.last_plus_one - _temp3032.curr, _temp3032.curr);})); goto _LL3025;
_LL3029: goto _LL3025; _LL3025:;}}}{ struct Cyc_Tcutil_CVTEnv _temp3034=({
struct Cyc_Tcutil_CVTEnv _temp3033; _temp3033.kind_env= cvtenv.kind_env;
_temp3033.free_vars= 0; _temp3033.free_evars= 0; _temp3033;}); goto _LL3035;
_LL3035: _temp3034= Cyc_Tcutil_i_check_valid_type( loc, te, _temp3034,( void*)
Cyc_Absyn_MemKind, _temp2849); for( 0; _temp2847 != 0; _temp2847=(( struct Cyc_List_List*)
_check_null( _temp2847))->tl){ _temp3034= Cyc_Tcutil_i_check_valid_type( loc, te,
_temp3034,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp2847))->hd)).f3);} if( _temp2843 != 0){ if( _temp2845){(( void(*)(
struct _tagged_string msg)) Cyc_Tcutil_impos)(({ unsigned char* _temp3036=(
unsigned char*)"both c_vararg and cyc_vararg"; struct _tagged_string _temp3037;
_temp3037.curr= _temp3036; _temp3037.base= _temp3036; _temp3037.last_plus_one=
_temp3036 + 29; _temp3037;}));}{ int _temp3040; void* _temp3042; void* _temp3044;
struct Cyc_Absyn_Tqual _temp3046; struct Cyc_Core_Opt* _temp3048; struct Cyc_Absyn_VarargInfo
_temp3038=*(( struct Cyc_Absyn_VarargInfo*) _check_null( _temp2843)); _LL3049:
_temp3048= _temp3038.name; goto _LL3047; _LL3047: _temp3046= _temp3038.tq; goto
_LL3045; _LL3045: _temp3044=( void*) _temp3038.type; goto _LL3043; _LL3043:
_temp3042=( void*) _temp3038.rgn; goto _LL3041; _LL3041: _temp3040= _temp3038.inject;
goto _LL3039; _LL3039: _temp3034= Cyc_Tcutil_i_check_valid_type( loc, te,
_temp3034,( void*) Cyc_Absyn_MemKind, _temp3044);{ void* _temp3050= Cyc_Tcutil_compress(
_temp3042); struct Cyc_Core_Opt* _temp3060; int _temp3062; struct Cyc_Core_Opt*
_temp3064; struct Cyc_Core_Opt* _temp3066; struct Cyc_Absyn_Tvar* _temp3068;
_LL3052: if(( unsigned int) _temp3050 > 4u?*(( int*) _temp3050) == Cyc_Absyn_Evar:
0){ _LL3067: _temp3066=(( struct Cyc_Absyn_Evar_struct*) _temp3050)->f1; goto
_LL3065; _LL3065: _temp3064=(( struct Cyc_Absyn_Evar_struct*) _temp3050)->f2;
goto _LL3063; _LL3063: _temp3062=(( struct Cyc_Absyn_Evar_struct*) _temp3050)->f3;
goto _LL3061; _LL3061: _temp3060=(( struct Cyc_Absyn_Evar_struct*) _temp3050)->f4;
goto _LL3053;} else{ goto _LL3054;} _LL3054: if(( unsigned int) _temp3050 > 4u?*((
int*) _temp3050) == Cyc_Absyn_VarType: 0){ _LL3069: _temp3068=(( struct Cyc_Absyn_VarType_struct*)
_temp3050)->f1; goto _LL3055;} else{ goto _LL3056;} _LL3056: if( _temp3050 ==(
void*) Cyc_Absyn_HeapRgn){ goto _LL3057;} else{ goto _LL3058;} _LL3058: goto
_LL3059; _LL3053: { void* vr_tvar=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp3070=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3070[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3071; _temp3071.tag= Cyc_Absyn_VarType;
_temp3071.f1= Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_RgnKind); _temp3071;});
_temp3070;}); Cyc_Tcutil_unify( _temp3042, vr_tvar); goto _LL3051;} _LL3055:
goto _LL3051; _LL3057: Cyc_Tcutil_terr( loc,({ unsigned char* _temp3072=(
unsigned char*)"varargs should be in a region, not heap-allocated"; struct
_tagged_string _temp3073; _temp3073.curr= _temp3072; _temp3073.base= _temp3072;
_temp3073.last_plus_one= _temp3072 + 50; _temp3073;})); goto _LL3051; _LL3059:
goto _LL3051; _LL3051:;} _temp3034= Cyc_Tcutil_i_check_valid_type( loc, te,
_temp3034,( void*) Cyc_Absyn_RgnKind, _temp3042); if( _temp3040){ void*
_temp3074= Cyc_Tcutil_compress( _temp3044); struct Cyc_Absyn_TunionInfo
_temp3080; void* _temp3082; struct Cyc_List_List* _temp3084; void* _temp3086;
struct Cyc_Absyn_Tuniondecl* _temp3088; _LL3076: if(( unsigned int) _temp3074 >
4u?*(( int*) _temp3074) == Cyc_Absyn_TunionType: 0){ _LL3081: _temp3080=((
struct Cyc_Absyn_TunionType_struct*) _temp3074)->f1; _LL3087: _temp3086=( void*)
_temp3080.tunion_info; if(*(( int*) _temp3086) == Cyc_Absyn_KnownTunion){
_LL3089: _temp3088=(( struct Cyc_Absyn_KnownTunion_struct*) _temp3086)->f1; goto
_LL3085;} else{ goto _LL3078;} _LL3085: _temp3084= _temp3080.targs; goto _LL3083;
_LL3083: _temp3082=( void*) _temp3080.rgn; goto _LL3077;} else{ goto _LL3078;}
_LL3078: goto _LL3079; _LL3077:{ void* _temp3090= Cyc_Tcutil_compress( _temp3082);
struct Cyc_Core_Opt* _temp3100; int _temp3102; struct Cyc_Core_Opt* _temp3104;
struct Cyc_Core_Opt* _temp3106; struct Cyc_Absyn_Tvar* _temp3108; _LL3092: if((
unsigned int) _temp3090 > 4u?*(( int*) _temp3090) == Cyc_Absyn_Evar: 0){ _LL3107:
_temp3106=(( struct Cyc_Absyn_Evar_struct*) _temp3090)->f1; goto _LL3105;
_LL3105: _temp3104=(( struct Cyc_Absyn_Evar_struct*) _temp3090)->f2; goto
_LL3103; _LL3103: _temp3102=(( struct Cyc_Absyn_Evar_struct*) _temp3090)->f3;
goto _LL3101; _LL3101: _temp3100=(( struct Cyc_Absyn_Evar_struct*) _temp3090)->f4;
goto _LL3093;} else{ goto _LL3094;} _LL3094: if(( unsigned int) _temp3090 > 4u?*((
int*) _temp3090) == Cyc_Absyn_VarType: 0){ _LL3109: _temp3108=(( struct Cyc_Absyn_VarType_struct*)
_temp3090)->f1; goto _LL3095;} else{ goto _LL3096;} _LL3096: if( _temp3090 ==(
void*) Cyc_Absyn_HeapRgn){ goto _LL3097;} else{ goto _LL3098;} _LL3098: goto
_LL3099; _LL3093: Cyc_Tcutil_unify( _temp3042, _temp3082); goto _LL3091; _LL3095:
goto _LL3091; _LL3097: Cyc_Tcutil_terr( loc,({ unsigned char* _temp3110=(
unsigned char*)"injected vararg [x]tunion should be in a region, not heap-allocated";
struct _tagged_string _temp3111; _temp3111.curr= _temp3110; _temp3111.base=
_temp3110; _temp3111.last_plus_one= _temp3110 + 68; _temp3111;})); goto _LL3091;
_LL3099: goto _LL3091; _LL3091:;} goto _LL3075; _LL3079: Cyc_Tcutil_terr( loc,({
unsigned char* _temp3112=( unsigned char*)"can't inject a non-[x]tunion type";
struct _tagged_string _temp3113; _temp3113.curr= _temp3112; _temp3113.base=
_temp3112; _temp3113.last_plus_one= _temp3112 + 34; _temp3113;})); goto _LL3075;
_LL3075:;}}}{ struct Cyc_List_List* rpo= _temp2841; for( 0; rpo != 0; rpo=((
struct Cyc_List_List*) _check_null( rpo))->tl){ struct _tuple8 _temp3116; void*
_temp3117; void* _temp3119; struct _tuple8* _temp3114=( struct _tuple8*)((
struct Cyc_List_List*) _check_null( rpo))->hd; _temp3116=* _temp3114; _LL3120:
_temp3119= _temp3116.f1; goto _LL3118; _LL3118: _temp3117= _temp3116.f2; goto
_LL3115; _LL3115: _temp3034= Cyc_Tcutil_i_check_valid_type( loc, te, _temp3034,(
void*) Cyc_Absyn_RgnKind, _temp3119); _temp3034= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp3034,( void*) Cyc_Absyn_RgnKind, _temp3117);}} if(* _temp2853 != 0){
_temp3034= Cyc_Tcutil_i_check_valid_type( loc, te, _temp3034,( void*) Cyc_Absyn_EffKind,(
void*)(( struct Cyc_Core_Opt*) _check_null(* _temp2853))->v);} else{ struct Cyc_Absyn_Tvar*
e= Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_EffKind); _temp3034.free_vars= Cyc_Tcutil_add_free_tvar(
loc, _temp3034.free_vars, e);{ struct Cyc_List_List* effect= 0;{ struct Cyc_List_List*
tvs= _temp3034.free_vars; for( 0; tvs != 0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){ void* _temp3121=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind))->v; void* _temp3129; void*
_temp3131; _LL3123: if(( unsigned int) _temp3121 > 1u?*(( int*) _temp3121) ==
Cyc_Absyn_Eq_constr: 0){ _LL3130: _temp3129=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3121)->f1; if( _temp3129 ==( void*) Cyc_Absyn_RgnKind){ goto _LL3124;}
else{ goto _LL3125;}} else{ goto _LL3125;} _LL3125: if(( unsigned int) _temp3121
> 1u?*(( int*) _temp3121) == Cyc_Absyn_Eq_constr: 0){ _LL3132: _temp3131=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp3121)->f1; if( _temp3131 ==( void*) Cyc_Absyn_EffKind){
goto _LL3126;} else{ goto _LL3127;}} else{ goto _LL3127;} _LL3127: goto _LL3128;
_LL3124: effect=({ struct Cyc_List_List* _temp3133=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3133->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp3134=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp3134[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp3135; _temp3135.tag= Cyc_Absyn_AccessEff;
_temp3135.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3136=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3136[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3137; _temp3137.tag= Cyc_Absyn_VarType;
_temp3137.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3137;}); _temp3136;})); _temp3135;}); _temp3134;})); _temp3133->tl= effect;
_temp3133;}); goto _LL3122; _LL3126: effect=({ struct Cyc_List_List* _temp3138=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3138->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3139=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp3139[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp3140; _temp3140.tag= Cyc_Absyn_VarType; _temp3140.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3140;}); _temp3139;})); _temp3138->tl= effect; _temp3138;}); goto _LL3122;
_LL3128: goto _LL3122; _LL3122:;}}{ struct Cyc_List_List* ts= _temp3034.free_evars;
for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){ void*
_temp3141= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);
_LL3143: if( _temp3141 ==( void*) Cyc_Absyn_RgnKind){ goto _LL3144;} else{ goto
_LL3145;} _LL3145: if( _temp3141 ==( void*) Cyc_Absyn_EffKind){ goto _LL3146;}
else{ goto _LL3147;} _LL3147: goto _LL3148; _LL3144: effect=({ struct Cyc_List_List*
_temp3149=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3149->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3150=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3150[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3151; _temp3151.tag= Cyc_Absyn_AccessEff;
_temp3151.f1=( void*)(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);
_temp3151;}); _temp3150;})); _temp3149->tl= effect; _temp3149;}); goto _LL3142;
_LL3146: effect=({ struct Cyc_List_List* _temp3152=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3152->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( ts))->hd); _temp3152->tl= effect; _temp3152;});
goto _LL3142; _LL3148: goto _LL3142; _LL3142:;}}* _temp2853=({ struct Cyc_Core_Opt*
_temp3153=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3153->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3154=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3154[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3155; _temp3155.tag= Cyc_Absyn_JoinEff;
_temp3155.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
effect); _temp3155;}); _temp3154;})); _temp3153;});}} if(* _temp2856 != 0){
struct Cyc_List_List* bs=* _temp2856; for( 0; bs != 0; bs=(( struct Cyc_List_List*)
_check_null( bs))->tl){ struct Cyc_Absyn_Conref* _temp3156=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( bs))->hd)->kind); goto _LL3157; _LL3157: {
void* _temp3158=( void*) _temp3156->v; void* _temp3166; _LL3160: if( _temp3158
==( void*) Cyc_Absyn_No_constr){ goto _LL3161;} else{ goto _LL3162;} _LL3162:
if(( unsigned int) _temp3158 > 1u?*(( int*) _temp3158) == Cyc_Absyn_Eq_constr: 0){
_LL3167: _temp3166=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3158)->f1;
if( _temp3166 ==( void*) Cyc_Absyn_MemKind){ goto _LL3163;} else{ goto _LL3164;}}
else{ goto _LL3164;} _LL3164: goto _LL3165; _LL3161: Cyc_Tcutil_warn( loc,(
struct _tagged_string)({ struct _tagged_string _temp3168=*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( bs))->hd)->name; xprintf("Type variable %.*s unconstrained, assuming boxed",
_temp3168.last_plus_one - _temp3168.curr, _temp3168.curr);})); goto _LL3163;
_LL3163:( void*)( _temp3156->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3169=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3169[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3170; _temp3170.tag= Cyc_Absyn_Eq_constr;
_temp3170.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3170;}); _temp3169;})));
goto _LL3159; _LL3165: goto _LL3159; _LL3159:;}}} cvtenv.kind_env= Cyc_Tcutil_remove_bound_tvars(
_temp3034.kind_env,* _temp2856); _temp3034.free_vars= Cyc_Tcutil_remove_bound_tvars(
_temp3034.free_vars,* _temp2856);{ struct Cyc_List_List* tvs= _temp3034.free_vars;
for( 0; tvs != 0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd);}}{ struct Cyc_List_List* evs=
_temp3034.free_evars; for( 0; evs != 0; evs=(( struct Cyc_List_List*)
_check_null( evs))->tl){ cvtenv.free_evars= Cyc_Tcutil_add_free_evar( cvtenv.free_evars,(
void*)(( struct Cyc_List_List*) _check_null( evs))->hd);}} goto _LL2738;}}
_LL2764: for( 0; _temp2857 != 0; _temp2857=(( struct Cyc_List_List*) _check_null(
_temp2857))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*)
Cyc_Absyn_MemKind,(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp2857))->hd)).f2);} goto _LL2738; _LL2766:{ struct _RegionHandle _temp3171=
_new_region(); struct _RegionHandle* sprev_rgn=& _temp3171; _push_region(
sprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for( 0; _temp2859 != 0;
_temp2859=(( struct Cyc_List_List*) _check_null( _temp2859))->tl){ struct Cyc_Absyn_Structfield
_temp3174; struct Cyc_List_List* _temp3175; struct Cyc_Core_Opt* _temp3177; void*
_temp3179; struct Cyc_Absyn_Tqual _temp3181; struct _tagged_string* _temp3183;
struct Cyc_Absyn_Structfield* _temp3172=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2859))->hd; _temp3174=* _temp3172;
_LL3184: _temp3183= _temp3174.name; goto _LL3182; _LL3182: _temp3181= _temp3174.tq;
goto _LL3180; _LL3180: _temp3179=( void*) _temp3174.type; goto _LL3178; _LL3178:
_temp3177= _temp3174.width; goto _LL3176; _LL3176: _temp3175= _temp3174.attributes;
goto _LL3173; _LL3173: if((( int(*)( int(* compare)( struct _tagged_string*,
struct _tagged_string*), struct Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields, _temp3183)){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp3185=* _temp3183; xprintf("duplicate field %.*s in struct",
_temp3185.last_plus_one - _temp3185.curr, _temp3185.curr);}));} if( Cyc_String_strcmp(*
_temp3183,({ unsigned char* _temp3186=( unsigned char*)""; struct _tagged_string
_temp3187; _temp3187.curr= _temp3186; _temp3187.base= _temp3186; _temp3187.last_plus_one=
_temp3186 + 1; _temp3187;})) != 0){ prev_fields=({ struct Cyc_List_List*
_temp3188=( struct Cyc_List_List*) _region_malloc( sprev_rgn, sizeof( struct Cyc_List_List));
_temp3188->hd=( void*) _temp3183; _temp3188->tl= prev_fields; _temp3188;});}
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,
_temp3179); if( _temp3177 != 0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp3177))->v)){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string _temp3189=*
_temp3183; xprintf("bitfield %.*s does not have constant width", _temp3189.last_plus_one
- _temp3189.curr, _temp3189.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp3177))->v);}{
void* _temp3190= Cyc_Tcutil_compress( _temp3179); void* _temp3196; void*
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
struct _tagged_string _temp3218=* _temp3183; struct _tagged_string _temp3219=
Cyc_Absynpp_typ2string( _temp3179); xprintf("bitfield %.*s must have integral type but has type %.*s",
_temp3218.last_plus_one - _temp3218.curr, _temp3218.curr, _temp3219.last_plus_one
- _temp3219.curr, _temp3219.curr);})); goto _LL3191; _LL3191:;}} for( 0;
_temp3175 != 0; _temp3175=(( struct Cyc_List_List*) _check_null( _temp3175))->tl){
void* _temp3220=( void*)(( struct Cyc_List_List*) _check_null( _temp3175))->hd;
int _temp3228; _LL3222: if( _temp3220 ==( void*) Cyc_Absyn_Packed_att){ goto
_LL3223;} else{ goto _LL3224;} _LL3224: if(( unsigned int) _temp3220 > 16u?*((
int*) _temp3220) == Cyc_Absyn_Aligned_att: 0){ _LL3229: _temp3228=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp3220)->f1; goto _LL3225;} else{ goto _LL3226;} _LL3226: goto _LL3227;
_LL3223: continue; _LL3225: continue; _LL3227: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp3230= Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp3175))->hd); struct
_tagged_string _temp3231=* _temp3183; xprintf("bad attribute %.*s on struct member %.*s",
_temp3230.last_plus_one - _temp3230.curr, _temp3230.curr, _temp3231.last_plus_one
- _temp3231.curr, _temp3231.curr);})); goto _LL3221; _LL3221:;}}}; _pop_region(&
_temp3171);} goto _LL2738; _LL2768:{ struct _RegionHandle _temp3232= _new_region();
struct _RegionHandle* uprev_rgn=& _temp3232; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0; for( 0; _temp2861 != 0; _temp2861=(( struct Cyc_List_List*)
_check_null( _temp2861))->tl){ struct Cyc_Absyn_Structfield _temp3235; struct
Cyc_List_List* _temp3236; struct Cyc_Core_Opt* _temp3238; void* _temp3240;
struct Cyc_Absyn_Tqual _temp3242; struct _tagged_string* _temp3244; struct Cyc_Absyn_Structfield*
_temp3233=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2861))->hd; _temp3235=* _temp3233; _LL3245: _temp3244= _temp3235.name; goto
_LL3243; _LL3243: _temp3242= _temp3235.tq; goto _LL3241; _LL3241: _temp3240=(
void*) _temp3235.type; goto _LL3239; _LL3239: _temp3238= _temp3235.width; goto
_LL3237; _LL3237: _temp3236= _temp3235.attributes; goto _LL3234; _LL3234: if(((
int(*)( int(* compare)( struct _tagged_string*, struct _tagged_string*), struct
Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp,
prev_fields, _temp3244)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp3246=* _temp3244; xprintf("duplicate field %.*s in struct",
_temp3246.last_plus_one - _temp3246.curr, _temp3246.curr);}));} if( Cyc_String_strcmp(*
_temp3244,({ unsigned char* _temp3247=( unsigned char*)""; struct _tagged_string
_temp3248; _temp3248.curr= _temp3247; _temp3248.base= _temp3247; _temp3248.last_plus_one=
_temp3247 + 1; _temp3248;})) != 0){ prev_fields=({ struct Cyc_List_List*
_temp3249=( struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp3249->hd=( void*) _temp3244; _temp3249->tl= prev_fields; _temp3249;});}
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,
_temp3240); if( ! Cyc_Tcutil_bits_only( _temp3240)){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp3250=* _temp3244; xprintf("%.*s has a type that is possibly incompatible with other members of union",
_temp3250.last_plus_one - _temp3250.curr, _temp3250.curr);}));} if( _temp3238 !=
0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp3238))->v)){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp3251=* _temp3244; xprintf("bitfield %.*s does not have constant width",
_temp3251.last_plus_one - _temp3251.curr, _temp3251.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp3238))->v);}{
void* _temp3252= Cyc_Tcutil_compress( _temp3240); void* _temp3258; void*
_temp3260; _LL3254: if(( unsigned int) _temp3252 > 4u?*(( int*) _temp3252) ==
Cyc_Absyn_IntType: 0){ _LL3261: _temp3260=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3252)->f1; goto _LL3259; _LL3259: _temp3258=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3252)->f2; goto _LL3255;} else{ goto _LL3256;} _LL3256: goto _LL3257;
_LL3255:{ void* _temp3262= _temp3258; _LL3264: if( _temp3262 ==( void*) Cyc_Absyn_B1){
goto _LL3265;} else{ goto _LL3266;} _LL3266: if( _temp3262 ==( void*) Cyc_Absyn_B2){
goto _LL3267;} else{ goto _LL3268;} _LL3268: if( _temp3262 ==( void*) Cyc_Absyn_B4){
goto _LL3269;} else{ goto _LL3270;} _LL3270: if( _temp3262 ==( void*) Cyc_Absyn_B8){
goto _LL3271;} else{ goto _LL3263;} _LL3265: if( w > 8){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp3272=( unsigned char*)"bitfield larger than type"; struct
_tagged_string _temp3273; _temp3273.curr= _temp3272; _temp3273.base= _temp3272;
_temp3273.last_plus_one= _temp3272 + 26; _temp3273;}));} goto _LL3263; _LL3267:
if( w > 16){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp3274=( unsigned char*)"bitfield larger than type";
struct _tagged_string _temp3275; _temp3275.curr= _temp3274; _temp3275.base=
_temp3274; _temp3275.last_plus_one= _temp3274 + 26; _temp3275;}));} goto _LL3263;
_LL3269: if( w > 32){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp3276=(
unsigned char*)"bitfield larger than type"; struct _tagged_string _temp3277;
_temp3277.curr= _temp3276; _temp3277.base= _temp3276; _temp3277.last_plus_one=
_temp3276 + 26; _temp3277;}));} goto _LL3263; _LL3271: if( w > 64){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp3278=( unsigned char*)"bitfield larger than type";
struct _tagged_string _temp3279; _temp3279.curr= _temp3278; _temp3279.base=
_temp3278; _temp3279.last_plus_one= _temp3278 + 26; _temp3279;}));} goto _LL3263;
_LL3263:;} goto _LL3253; _LL3257: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
struct _tagged_string _temp3280=* _temp3244; struct _tagged_string _temp3281=
Cyc_Absynpp_typ2string( _temp3240); xprintf("bitfield %.*s must have integral type but has type %.*s",
_temp3280.last_plus_one - _temp3280.curr, _temp3280.curr, _temp3281.last_plus_one
- _temp3281.curr, _temp3281.curr);})); goto _LL3253; _LL3253:;}} for( 0;
_temp3236 != 0; _temp3236=(( struct Cyc_List_List*) _check_null( _temp3236))->tl){
void* _temp3282=( void*)(( struct Cyc_List_List*) _check_null( _temp3236))->hd;
int _temp3290; _LL3284: if( _temp3282 ==( void*) Cyc_Absyn_Packed_att){ goto
_LL3285;} else{ goto _LL3286;} _LL3286: if(( unsigned int) _temp3282 > 16u?*((
int*) _temp3282) == Cyc_Absyn_Aligned_att: 0){ _LL3291: _temp3290=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp3282)->f1; goto _LL3287;} else{ goto _LL3288;} _LL3288: goto _LL3289;
_LL3285: continue; _LL3287: continue; _LL3289: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp3292= Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp3236))->hd); struct
_tagged_string _temp3293=* _temp3244; xprintf("bad attribute %.*s on struct member %.*s",
_temp3292.last_plus_one - _temp3292.curr, _temp3292.curr, _temp3293.last_plus_one
- _temp3293.curr, _temp3293.curr);})); goto _LL3283; _LL3283:;}}}; _pop_region(&
_temp3232);} goto _LL2738; _LL2770: if( _temp2868 == 0){ if(* _temp2865 == 0){
Cyc_Tcutil_terr( loc,( struct _tagged_string) xprintf("bad anonymous struct"));
return cvtenv;}{ struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**)
_check_null(* _temp2865));;}} if(* _temp2865 == 0){ struct _handler_cons
_temp3294; _push_handler(& _temp3294);{ int _temp3296= 0; if( setjmp( _temp3294.handler)){
_temp3296= 1;} if( ! _temp3296){* _temp2865=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc,( struct _tuple1*) _check_null( _temp2868));; _pop_handler();} else{
void* _temp3295=( void*) _exn_thrown; void* _temp3298= _temp3295; _LL3300: if(
_temp3298 == Cyc_Dict_Absent){ goto _LL3301;} else{ goto _LL3302;} _LL3302: goto
_LL3303; _LL3301: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp3304= Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null(
_temp2868)); xprintf("unbound type struct %.*s", _temp3304.last_plus_one -
_temp3304.curr, _temp3304.curr);})); return cvtenv; _LL3303:( void) _throw(
_temp3298); _LL3299:;}}}{ struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**)
_check_null(* _temp2865)); if( sd->name != 0){*(( struct _tuple1*) _check_null(
_temp2868))=(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)[
_check_known_subscript_notnull( 1u, 0)];}{ struct Cyc_List_List* tvs= sd->tvs;
int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2866); if( lvs != largs){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string _temp3305=
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp2868)); int
_temp3306= lvs; int _temp3307= largs; xprintf("struct %.*s expects %d type arguments but was given %d",
_temp3305.last_plus_one - _temp3305.curr, _temp3305.curr, _temp3306, _temp3307);}));}
for( 0; _temp2866 != 0;( _temp2866=(( struct Cyc_List_List*) _check_null(
_temp2866))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2866))->hd; void* k1=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} goto _LL2738;}} _LL2772: if( _temp2875 == 0){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp3308=( unsigned char*)"missing union name"; struct
_tagged_string _temp3309; _temp3309.curr= _temp3308; _temp3309.base= _temp3308;
_temp3309.last_plus_one= _temp3308 + 19; _temp3309;})); return cvtenv;} if(*
_temp2872 == 0){ struct _handler_cons _temp3310; _push_handler(& _temp3310);{
int _temp3312= 0; if( setjmp( _temp3310.handler)){ _temp3312= 1;} if( !
_temp3312){* _temp2872=( struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl(
te, loc,( struct _tuple1*) _check_null( _temp2875));; _pop_handler();} else{
void* _temp3311=( void*) _exn_thrown; void* _temp3314= _temp3311; _LL3316: if(
_temp3314 == Cyc_Dict_Absent){ goto _LL3317;} else{ goto _LL3318;} _LL3318: goto
_LL3319; _LL3317: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct
_tagged_string _temp3320= Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null(
_temp2875)); xprintf("unbound type union %.*s", _temp3320.last_plus_one -
_temp3320.curr, _temp3320.curr);})); return cvtenv; _LL3319:( void) _throw(
_temp3314); _LL3315:;}}}{ struct Cyc_Absyn_Uniondecl* ud=*(( struct Cyc_Absyn_Uniondecl**)
_check_null(* _temp2872)); if( ud->name != 0){*(( struct _tuple1*) _check_null(
_temp2875))=(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( ud->name))->v)[
_check_known_subscript_notnull( 1u, 0)];}{ struct Cyc_List_List* tvs= ud->tvs;
int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2873); if( lvs != largs){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string _temp3321=
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp2875)); int
_temp3322= lvs; int _temp3323= largs; xprintf("union %.*s expects %d type arguments but was given %d",
_temp3321.last_plus_one - _temp3321.curr, _temp3321.curr, _temp3322, _temp3323);}));}
for( 0; _temp2873 != 0;( _temp2873=(( struct Cyc_List_List*) _check_null(
_temp2873))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2873))->hd; void* k1=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} goto _LL2738;}} _LL2774: { struct Cyc_Absyn_Typedefdecl*
td;{ struct _handler_cons _temp3324; _push_handler(& _temp3324);{ int _temp3326=
0; if( setjmp( _temp3324.handler)){ _temp3326= 1;} if( ! _temp3326){ td= Cyc_Tcenv_lookup_typedefdecl(
te, loc, _temp2882);; _pop_handler();} else{ void* _temp3325=( void*)
_exn_thrown; void* _temp3328= _temp3325; _LL3330: if( _temp3328 == Cyc_Dict_Absent){
goto _LL3331;} else{ goto _LL3332;} _LL3332: goto _LL3333; _LL3331: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ struct _tagged_string _temp3334= Cyc_Absynpp_qvar2string(
_temp2882); xprintf("unbound typedef name %.*s", _temp3334.last_plus_one -
_temp3334.curr, _temp3334.curr);})); return cvtenv; _LL3333:( void) _throw(
_temp3328); _LL3329:;}}} _temp2882[ _check_known_subscript_notnull( 1u, 0)]=( td->name)[
_check_known_subscript_notnull( 1u, 0)];{ struct Cyc_List_List* tvs= td->tvs;
struct Cyc_List_List* ts= _temp2880; struct Cyc_List_List* inst= 0; for( 0; ts
!= 0? tvs != 0: 0;( ts=(( struct Cyc_List_List*) _check_null( ts))->tl, tvs=((
struct Cyc_List_List*) _check_null( tvs))->tl)){ void* k=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te,
cvtenv, k,( void*)(( struct Cyc_List_List*) _check_null( ts))->hd); inst=({
struct Cyc_List_List* _temp3335=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3335->hd=( void*)({ struct _tuple5* _temp3336=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp3336->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd; _temp3336->f2=( void*)(( struct
Cyc_List_List*) _check_null( ts))->hd; _temp3336;}); _temp3335->tl= inst;
_temp3335;});} if( tvs != 0? 1: ts != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp3337= Cyc_Absynpp_qvar2string(
_temp2882); xprintf("typedef %.*s expects a different number of arguments",
_temp3337.last_plus_one - _temp3337.curr, _temp3337.curr);})); return cvtenv;}{
void* new_typ= Cyc_Tcutil_substitute( inst,( void*) td->defn);* _temp2879=({
struct Cyc_Core_Opt* _temp3338=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3338->v=( void*) new_typ; _temp3338;}); goto _LL2738;}}}
_LL2776: goto _LL2738; _LL2778: _temp2886= _temp2884; goto _LL2780; _LL2780:
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2886); goto _LL2738; _LL2782: for( 0; _temp2888 != 0; _temp2888=(( struct
Cyc_List_List*) _check_null( _temp2888))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_List_List*)
_check_null( _temp2888))->hd);} goto _LL2738; _LL2738:;} if( ! Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind( t), expected_kind)){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp3339= Cyc_Absynpp_typ2string( t);
struct _tagged_string _temp3340= Cyc_Absynpp_kind2string( Cyc_Tcutil_typ_kind( t));
struct _tagged_string _temp3341= Cyc_Absynpp_kind2string( expected_kind);
xprintf("type %.*s has kind %.*s but as used here needs kind %.*s", _temp3339.last_plus_one
- _temp3339.curr, _temp3339.curr, _temp3340.last_plus_one - _temp3340.curr,
_temp3340.curr, _temp3341.last_plus_one - _temp3341.curr, _temp3341.curr);}));}
return cvtenv;} struct Cyc_List_List* Cyc_Tcutil_check_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* kind_env, void*
expected_kind, void* t){ struct Cyc_Tcutil_CVTEnv _temp3343= Cyc_Tcutil_i_check_valid_type(
loc, te,({ struct Cyc_Tcutil_CVTEnv _temp3342; _temp3342.kind_env= kind_env;
_temp3342.free_vars= 0; _temp3342.free_evars= 0; _temp3342;}), expected_kind, t);
goto _LL3344; _LL3344:{ struct Cyc_List_List* vs= _temp3343.free_vars; for( 0;
vs != 0; vs=(( struct Cyc_List_List*) _check_null( vs))->tl){ Cyc_Tcutil_fast_add_free_tvar(
kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( vs))->hd);}}{
struct Cyc_List_List* evs= _temp3343.free_evars; for( 0; evs != 0; evs=(( struct
Cyc_List_List*) _check_null( evs))->tl){ void* _temp3345= Cyc_Tcutil_compress((
void*)(( struct Cyc_List_List*) _check_null( evs))->hd); struct Cyc_Core_Opt*
_temp3351; struct Cyc_Core_Opt** _temp3353; int _temp3354; struct Cyc_Core_Opt*
_temp3356; struct Cyc_Core_Opt* _temp3358; _LL3347: if(( unsigned int) _temp3345
> 4u?*(( int*) _temp3345) == Cyc_Absyn_Evar: 0){ _LL3359: _temp3358=(( struct
Cyc_Absyn_Evar_struct*) _temp3345)->f1; goto _LL3357; _LL3357: _temp3356=((
struct Cyc_Absyn_Evar_struct*) _temp3345)->f2; goto _LL3355; _LL3355: _temp3354=((
struct Cyc_Absyn_Evar_struct*) _temp3345)->f3; goto _LL3352; _LL3352: _temp3351=((
struct Cyc_Absyn_Evar_struct*) _temp3345)->f4; _temp3353=&(( struct Cyc_Absyn_Evar_struct*)
_temp3345)->f4; goto _LL3348;} else{ goto _LL3349;} _LL3349: goto _LL3350;
_LL3348: if(* _temp3353 == 0){* _temp3353=({ struct Cyc_Core_Opt* _temp3360=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3360->v=(
void*) kind_env; _temp3360;});} else{ struct Cyc_List_List* _temp3361=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(* _temp3353))->v; goto
_LL3362; _LL3362: { struct Cyc_List_List* _temp3363= 0; goto _LL3364; _LL3364:
for( 0; _temp3361 != 0; _temp3361=(( struct Cyc_List_List*) _check_null(
_temp3361))->tl){ if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct
Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp3361))->hd)){ _temp3363=({ struct Cyc_List_List* _temp3365=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3365->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp3361))->hd);
_temp3365->tl= _temp3363; _temp3365;});}}* _temp3353=({ struct Cyc_Core_Opt*
_temp3366=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3366->v=( void*) _temp3363; _temp3366;});}} goto _LL3346; _LL3350: goto
_LL3346; _LL3346:;}} return _temp3343.free_vars;} void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, void* t){ struct
Cyc_List_List* _temp3367= Cyc_Tcutil_check_valid_type( loc, te, 0,( void*) Cyc_Absyn_MemKind,
t); goto _LL3368; _LL3368:{ struct Cyc_List_List* x= _temp3367; for( 0; x != 0;
x=(( struct Cyc_List_List*) _check_null( x))->tl){ struct Cyc_Absyn_Conref* c=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( x))->hd)->kind);
void* _temp3369=( void*) c->v; void* _temp3377; _LL3371: if( _temp3369 ==( void*)
Cyc_Absyn_No_constr){ goto _LL3372;} else{ goto _LL3373;} _LL3373: if((
unsigned int) _temp3369 > 1u?*(( int*) _temp3369) == Cyc_Absyn_Eq_constr: 0){
_LL3378: _temp3377=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3369)->f1;
if( _temp3377 ==( void*) Cyc_Absyn_MemKind){ goto _LL3374;} else{ goto _LL3375;}}
else{ goto _LL3375;} _LL3375: goto _LL3376; _LL3372: goto _LL3374; _LL3374:(
void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3379=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3379[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3380; _temp3380.tag= Cyc_Absyn_Eq_constr;
_temp3380.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3380;}); _temp3379;})));
goto _LL3370; _LL3376: goto _LL3370; _LL3370:;}} if( _temp3367 != 0){ void*
_temp3381= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp3387; struct
Cyc_List_List* _temp3389; struct Cyc_List_List* _temp3391; struct Cyc_Absyn_VarargInfo*
_temp3393; int _temp3395; struct Cyc_List_List* _temp3397; void* _temp3399;
struct Cyc_Core_Opt* _temp3401; struct Cyc_List_List* _temp3403; struct Cyc_List_List**
_temp3405; _LL3383: if(( unsigned int) _temp3381 > 4u?*(( int*) _temp3381) ==
Cyc_Absyn_FnType: 0){ _LL3388: _temp3387=(( struct Cyc_Absyn_FnType_struct*)
_temp3381)->f1; _LL3404: _temp3403= _temp3387.tvars; _temp3405=&((( struct Cyc_Absyn_FnType_struct*)
_temp3381)->f1).tvars; goto _LL3402; _LL3402: _temp3401= _temp3387.effect; goto
_LL3400; _LL3400: _temp3399=( void*) _temp3387.ret_typ; goto _LL3398; _LL3398:
_temp3397= _temp3387.args; goto _LL3396; _LL3396: _temp3395= _temp3387.c_varargs;
goto _LL3394; _LL3394: _temp3393= _temp3387.cyc_varargs; goto _LL3392; _LL3392:
_temp3391= _temp3387.rgn_po; goto _LL3390; _LL3390: _temp3389= _temp3387.attributes;
goto _LL3384;} else{ goto _LL3385;} _LL3385: goto _LL3386; _LL3384: if(*
_temp3405 == 0){* _temp3405= _temp3367;} else{ if( _temp3367 != 0){ struct
_tagged_string _temp3406=*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp3367))->hd)->name; goto _LL3407; _LL3407: if(*(( const
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( _temp3406, sizeof( unsigned char), 0) =='?'){ Cyc_Tcutil_terr(
loc,( struct _tagged_string) xprintf("You failed to abstract over the generated type variables. \nRemove the bound type variables or else make the effects of the type explicit\nand then abstract the free type variables properly."));}
else{ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string
_temp3408= _temp3406; xprintf("unbound type variable %.*s", _temp3408.last_plus_one
- _temp3408.curr, _temp3408.curr);}));}}} goto _LL3382; _LL3386: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ struct _tagged_string _temp3409=*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp3367))->hd)->name; xprintf("unbound type variable %.*s",
_temp3409.last_plus_one - _temp3409.curr, _temp3409.curr);})); return; _LL3382:;}}
void Cyc_Tcutil_add_tvar_identity( struct Cyc_Absyn_Tvar* tv){ if( tv->identity
== 0){ tv->identity= Cyc_Tcutil_new_tvar_id();}} void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List* tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Tcutil_add_tvar_identity, tvs);}
void Cyc_Tcutil_check_type( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* bound_tvars, void* expected_kind, void* t){ struct Cyc_List_List*
_temp3410= Cyc_Tcutil_check_valid_type( loc, te, bound_tvars, expected_kind, t);
goto _LL3411; _LL3411: { struct Cyc_List_List* _temp3412= Cyc_Tcutil_remove_bound_tvars(
_temp3410, bound_tvars); goto _LL3413; _LL3413: { struct Cyc_List_List* fs=
_temp3412; for( 0; fs != 0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){
struct _tagged_string* _temp3414=(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->name; goto _LL3415; _LL3415: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp3416=* _temp3414; struct
_tagged_string _temp3417= Cyc_Absynpp_typ2string( t); xprintf("unbound type variable %.*s in type %.*s",
_temp3416.last_plus_one - _temp3416.curr, _temp3416.curr, _temp3417.last_plus_one
- _temp3417.curr, _temp3417.curr);}));}}}} static void Cyc_Tcutil_check_unique_unsorted(
int(* cmp)( void*, void*), struct Cyc_List_List* vs, struct Cyc_Position_Segment*
loc, struct _tagged_string(* a2string)( void*), struct _tagged_string msg){ for(
0; vs != 0; vs=(( struct Cyc_List_List*) _check_null( vs))->tl){ struct Cyc_List_List*
vs2=(( struct Cyc_List_List*) _check_null( vs))->tl; for( 0; vs2 != 0; vs2=((
struct Cyc_List_List*) _check_null( vs2))->tl){ if( cmp(( void*)(( struct Cyc_List_List*)
_check_null( vs))->hd,( void*)(( struct Cyc_List_List*) _check_null( vs2))->hd)
== 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ struct _tagged_string
_temp3418= msg; struct _tagged_string _temp3419= a2string(( void*)(( struct Cyc_List_List*)
_check_null( vs))->hd); xprintf("%.*s: %.*s", _temp3418.last_plus_one -
_temp3418.curr, _temp3418.curr, _temp3419.last_plus_one - _temp3419.curr,
_temp3419.curr);}));}}}} static struct _tagged_string Cyc_Tcutil_strptr2string(
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
tvs, loc, Cyc_Tcutil_tvar2string,({ unsigned char* _temp3420=( unsigned char*)"duplicate type variable";
struct _tagged_string _temp3421; _temp3421.curr= _temp3420; _temp3421.base=
_temp3420; _temp3421.last_plus_one= _temp3420 + 24; _temp3421;}));} struct
_tuple17{ struct Cyc_Absyn_Structfield* f1; int f2; } ; struct _tuple18{ struct
Cyc_List_List* f1; void* f2; } ; struct _tuple19{ struct Cyc_Absyn_Structfield*
f1; void* f2; } ; struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* sdfields){ struct Cyc_List_List* fields= 0;{ struct
Cyc_List_List* sd_fields= sdfields; for( 0; sd_fields != 0; sd_fields=(( struct
Cyc_List_List*) _check_null( sd_fields))->tl){ if( Cyc_String_strcmp(*(( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( sd_fields))->hd)->name,({
unsigned char* _temp3422=( unsigned char*)""; struct _tagged_string _temp3423;
_temp3423.curr= _temp3422; _temp3423.base= _temp3422; _temp3423.last_plus_one=
_temp3422 + 1; _temp3423;})) != 0){ fields=({ struct Cyc_List_List* _temp3424=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3424->hd=(
void*)({ struct _tuple17* _temp3425=( struct _tuple17*) GC_malloc( sizeof(
struct _tuple17)); _temp3425->f1=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( sd_fields))->hd; _temp3425->f2= 0; _temp3425;}); _temp3424->tl=
fields; _temp3424;});}}} fields=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( fields);{ struct Cyc_List_List* ans= 0; for( 0; des != 0;
des=(( struct Cyc_List_List*) _check_null( des))->tl){ struct _tuple18 _temp3428;
void* _temp3429; struct Cyc_List_List* _temp3431; struct _tuple18* _temp3426=(
struct _tuple18*)(( struct Cyc_List_List*) _check_null( des))->hd; _temp3428=*
_temp3426; _LL3432: _temp3431= _temp3428.f1; goto _LL3430; _LL3430: _temp3429=
_temp3428.f2; goto _LL3427; _LL3427: if( _temp3431 == 0){ struct Cyc_List_List*
_temp3433= fields; goto _LL3434; _LL3434: for( 0; _temp3433 != 0; _temp3433=((
struct Cyc_List_List*) _check_null( _temp3433))->tl){ if( !(*(( struct _tuple17*)((
struct Cyc_List_List*) _check_null( _temp3433))->hd)).f2){(*(( struct _tuple17*)((
struct Cyc_List_List*) _check_null( _temp3433))->hd)).f2= 1;(*(( struct _tuple18*)((
struct Cyc_List_List*) _check_null( des))->hd)).f1=({ struct Cyc_List_List*
_temp3435=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3435->hd=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp3436=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp3436[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp3437; _temp3437.tag= Cyc_Absyn_FieldName;
_temp3437.f1=((*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3433))->hd)).f1)->name; _temp3437;}); _temp3436;})); _temp3435->tl= 0;
_temp3435;}); ans=({ struct Cyc_List_List* _temp3438=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp3438->hd=( void*)({
struct _tuple19* _temp3439=( struct _tuple19*) _region_malloc( rgn, sizeof(
struct _tuple19)); _temp3439->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3433))->hd)).f1; _temp3439->f2= _temp3429; _temp3439;});
_temp3438->tl= ans; _temp3438;}); break;}} if( _temp3433 == 0){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp3440=( unsigned char*)"too many arguments to struct";
struct _tagged_string _temp3441; _temp3441.curr= _temp3440; _temp3441.base=
_temp3440; _temp3441.last_plus_one= _temp3440 + 29; _temp3441;}));}} else{ if(((
struct Cyc_List_List*) _check_null( _temp3431))->tl != 0){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp3442=( unsigned char*)"multiple designators are not supported";
struct _tagged_string _temp3443; _temp3443.curr= _temp3442; _temp3443.base=
_temp3442; _temp3443.last_plus_one= _temp3442 + 39; _temp3443;}));} else{ void*
_temp3444=( void*)(( struct Cyc_List_List*) _check_null( _temp3431))->hd; struct
Cyc_Absyn_Exp* _temp3450; struct _tagged_string* _temp3452; _LL3446: if(*(( int*)
_temp3444) == Cyc_Absyn_ArrayElement){ _LL3451: _temp3450=(( struct Cyc_Absyn_ArrayElement_struct*)
_temp3444)->f1; goto _LL3447;} else{ goto _LL3448;} _LL3448: if(*(( int*)
_temp3444) == Cyc_Absyn_FieldName){ _LL3453: _temp3452=(( struct Cyc_Absyn_FieldName_struct*)
_temp3444)->f1; goto _LL3449;} else{ goto _LL3445;} _LL3447: Cyc_Tcutil_terr(
loc,({ unsigned char* _temp3454=( unsigned char*)"array designator used in argument to struct";
struct _tagged_string _temp3455; _temp3455.curr= _temp3454; _temp3455.base=
_temp3454; _temp3455.last_plus_one= _temp3454 + 44; _temp3455;})); goto _LL3445;
_LL3449: { struct Cyc_List_List* _temp3456= fields; goto _LL3457; _LL3457: for(
0; _temp3456 != 0; _temp3456=(( struct Cyc_List_List*) _check_null( _temp3456))->tl){
if( Cyc_String_zstrptrcmp( _temp3452,((*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3456))->hd)).f1)->name) == 0){ if((*(( struct _tuple17*)((
struct Cyc_List_List*) _check_null( _temp3456))->hd)).f2){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ struct _tagged_string _temp3458=* _temp3452; xprintf("field %.*s has already been used as an argument",
_temp3458.last_plus_one - _temp3458.curr, _temp3458.curr);}));}(*(( struct
_tuple17*)(( struct Cyc_List_List*) _check_null( _temp3456))->hd)).f2= 1; ans=({
struct Cyc_List_List* _temp3459=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp3459->hd=( void*)({ struct _tuple19*
_temp3460=( struct _tuple19*) _region_malloc( rgn, sizeof( struct _tuple19));
_temp3460->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3456))->hd)).f1; _temp3460->f2= _temp3429; _temp3460;}); _temp3459->tl= ans;
_temp3459;}); break;}} if( _temp3456 == 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ struct _tagged_string _temp3461=* _temp3452; xprintf("bad field designator %.*s",
_temp3461.last_plus_one - _temp3461.curr, _temp3461.curr);}));} goto _LL3445;}
_LL3445:;}}} for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null(
fields))->tl){ if( !(*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
fields))->hd)).f2){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp3462=(
unsigned char*)"too few arguments to struct"; struct _tagged_string _temp3463;
_temp3463.curr= _temp3462; _temp3463.base= _temp3462; _temp3463.last_plus_one=
_temp3462 + 28; _temp3463;})); break;}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( ans);}} int Cyc_Tcutil_is_tagged_pointer_typ(
void* t){ void* _temp3464= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp3470; struct Cyc_Absyn_Conref* _temp3472; struct Cyc_Absyn_Tqual _temp3474;
struct Cyc_Absyn_Conref* _temp3476; void* _temp3478; void* _temp3480; _LL3466:
if(( unsigned int) _temp3464 > 4u?*(( int*) _temp3464) == Cyc_Absyn_PointerType:
0){ _LL3471: _temp3470=(( struct Cyc_Absyn_PointerType_struct*) _temp3464)->f1;
_LL3481: _temp3480=( void*) _temp3470.elt_typ; goto _LL3479; _LL3479: _temp3478=(
void*) _temp3470.rgn_typ; goto _LL3477; _LL3477: _temp3476= _temp3470.nullable;
goto _LL3475; _LL3475: _temp3474= _temp3470.tq; goto _LL3473; _LL3473: _temp3472=
_temp3470.bounds; goto _LL3467;} else{ goto _LL3468;} _LL3468: goto _LL3469;
_LL3467: { struct Cyc_Absyn_Conref* _temp3482=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3472); goto
_LL3483; _LL3483: { void* _temp3484=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3482))->v; void*
_temp3492; _LL3486: if(( unsigned int) _temp3484 > 1u?*(( int*) _temp3484) ==
Cyc_Absyn_Eq_constr: 0){ _LL3493: _temp3492=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3484)->f1; if( _temp3492 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3487;}
else{ goto _LL3488;}} else{ goto _LL3488;} _LL3488: if( _temp3484 ==( void*) Cyc_Absyn_No_constr){
goto _LL3489;} else{ goto _LL3490;} _LL3490: goto _LL3491; _LL3487: return 1;
_LL3489:( void*)( _temp3482->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3494=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3494[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3495; _temp3495.tag= Cyc_Absyn_Eq_constr;
_temp3495.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp3495;}); _temp3494;})));
return 1; _LL3491: return 0; _LL3485:;}} _LL3469: return 0; _LL3465:;} struct
_tuple7 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e){ struct _tuple7 bogus_ans=({ struct _tuple7 _temp3734; _temp3734.f1= 0;
_temp3734.f2=( void*) Cyc_Absyn_HeapRgn; _temp3734;}); void* _temp3496=( void*)
e->r; void* _temp3510; struct _tuple1* _temp3512; struct _tagged_string*
_temp3514; struct Cyc_Absyn_Exp* _temp3516; struct _tagged_string* _temp3518;
struct Cyc_Absyn_Exp* _temp3520; struct Cyc_Absyn_Exp* _temp3522; struct Cyc_Absyn_Exp*
_temp3524; struct Cyc_Absyn_Exp* _temp3526; _LL3498: if(*(( int*) _temp3496) ==
Cyc_Absyn_Var_e){ _LL3513: _temp3512=(( struct Cyc_Absyn_Var_e_struct*)
_temp3496)->f1; goto _LL3511; _LL3511: _temp3510=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3496)->f2; goto _LL3499;} else{ goto _LL3500;} _LL3500: if(*(( int*)
_temp3496) == Cyc_Absyn_StructMember_e){ _LL3517: _temp3516=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3496)->f1; goto _LL3515; _LL3515: _temp3514=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3496)->f2; goto _LL3501;} else{ goto _LL3502;} _LL3502: if(*(( int*)
_temp3496) == Cyc_Absyn_StructArrow_e){ _LL3521: _temp3520=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3496)->f1; goto _LL3519; _LL3519: _temp3518=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3496)->f2; goto _LL3503;} else{ goto _LL3504;} _LL3504: if(*(( int*)
_temp3496) == Cyc_Absyn_Deref_e){ _LL3523: _temp3522=(( struct Cyc_Absyn_Deref_e_struct*)
_temp3496)->f1; goto _LL3505;} else{ goto _LL3506;} _LL3506: if(*(( int*)
_temp3496) == Cyc_Absyn_Subscript_e){ _LL3527: _temp3526=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3496)->f1; goto _LL3525; _LL3525: _temp3524=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3496)->f2; goto _LL3507;} else{ goto _LL3508;} _LL3508: goto _LL3509;
_LL3499: { void* _temp3528= _temp3510; struct Cyc_Absyn_Fndecl* _temp3542;
struct Cyc_Absyn_Vardecl* _temp3544; struct Cyc_Absyn_Vardecl* _temp3546; struct
Cyc_Absyn_Vardecl* _temp3548; struct Cyc_Absyn_Vardecl* _temp3550; _LL3530: if(
_temp3528 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL3531;} else{ goto _LL3532;}
_LL3532: if(( unsigned int) _temp3528 > 1u?*(( int*) _temp3528) == Cyc_Absyn_Funname_b:
0){ _LL3543: _temp3542=(( struct Cyc_Absyn_Funname_b_struct*) _temp3528)->f1;
goto _LL3533;} else{ goto _LL3534;} _LL3534: if(( unsigned int) _temp3528 > 1u?*((
int*) _temp3528) == Cyc_Absyn_Global_b: 0){ _LL3545: _temp3544=(( struct Cyc_Absyn_Global_b_struct*)
_temp3528)->f1; goto _LL3535;} else{ goto _LL3536;} _LL3536: if(( unsigned int)
_temp3528 > 1u?*(( int*) _temp3528) == Cyc_Absyn_Local_b: 0){ _LL3547: _temp3546=((
struct Cyc_Absyn_Local_b_struct*) _temp3528)->f1; goto _LL3537;} else{ goto
_LL3538;} _LL3538: if(( unsigned int) _temp3528 > 1u?*(( int*) _temp3528) == Cyc_Absyn_Pat_b:
0){ _LL3549: _temp3548=(( struct Cyc_Absyn_Pat_b_struct*) _temp3528)->f1; goto
_LL3539;} else{ goto _LL3540;} _LL3540: if(( unsigned int) _temp3528 > 1u?*((
int*) _temp3528) == Cyc_Absyn_Param_b: 0){ _LL3551: _temp3550=(( struct Cyc_Absyn_Param_b_struct*)
_temp3528)->f1; goto _LL3541;} else{ goto _LL3529;} _LL3531: return bogus_ans;
_LL3533: return({ struct _tuple7 _temp3552; _temp3552.f1= 1; _temp3552.f2=( void*)
Cyc_Absyn_HeapRgn; _temp3552;}); _LL3535: { void* _temp3553= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); struct Cyc_Absyn_Exp*
_temp3559; struct Cyc_Absyn_Tqual _temp3561; void* _temp3563; _LL3555: if((
unsigned int) _temp3553 > 4u?*(( int*) _temp3553) == Cyc_Absyn_ArrayType: 0){
_LL3564: _temp3563=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3553)->f1;
goto _LL3562; _LL3562: _temp3561=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3553)->f2; goto _LL3560; _LL3560: _temp3559=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3553)->f3; goto _LL3556;} else{ goto _LL3557;} _LL3557: goto _LL3558;
_LL3556: return({ struct _tuple7 _temp3565; _temp3565.f1= 1; _temp3565.f2=( void*)
Cyc_Absyn_HeapRgn; _temp3565;}); _LL3558: return({ struct _tuple7 _temp3566;
_temp3566.f1=( _temp3544->tq).q_const; _temp3566.f2=( void*) Cyc_Absyn_HeapRgn;
_temp3566;}); _LL3554:;} _LL3537: { void* _temp3567= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v); struct Cyc_Absyn_Exp*
_temp3573; struct Cyc_Absyn_Tqual _temp3575; void* _temp3577; _LL3569: if((
unsigned int) _temp3567 > 4u?*(( int*) _temp3567) == Cyc_Absyn_ArrayType: 0){
_LL3578: _temp3577=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3567)->f1;
goto _LL3576; _LL3576: _temp3575=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3567)->f2; goto _LL3574; _LL3574: _temp3573=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3567)->f3; goto _LL3570;} else{ goto _LL3571;} _LL3571: goto _LL3572;
_LL3570: return({ struct _tuple7 _temp3579; _temp3579.f1= 1; _temp3579.f2=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp3546->rgn))->v; _temp3579;}); _LL3572:
return({ struct _tuple7 _temp3580; _temp3580.f1=( _temp3546->tq).q_const;
_temp3580.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3546->rgn))->v;
_temp3580;}); _LL3568:;} _LL3539: _temp3550= _temp3548; goto _LL3541; _LL3541:
return({ struct _tuple7 _temp3581; _temp3581.f1=( _temp3550->tq).q_const;
_temp3581.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3550->rgn))->v;
_temp3581;}); _LL3529:;} _LL3501: { void* _temp3582= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp3516->topt))->v); struct Cyc_List_List*
_temp3594; struct Cyc_List_List* _temp3596; struct Cyc_Absyn_Structdecl**
_temp3598; struct Cyc_Absyn_Structdecl* _temp3600; struct Cyc_List_List*
_temp3601; struct _tuple1* _temp3603; struct Cyc_Absyn_Uniondecl** _temp3605;
struct Cyc_Absyn_Uniondecl* _temp3607; struct Cyc_List_List* _temp3608; struct
_tuple1* _temp3610; _LL3584: if(( unsigned int) _temp3582 > 4u?*(( int*)
_temp3582) == Cyc_Absyn_AnonStructType: 0){ _LL3595: _temp3594=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3582)->f1; goto _LL3585;} else{ goto _LL3586;} _LL3586: if(( unsigned int)
_temp3582 > 4u?*(( int*) _temp3582) == Cyc_Absyn_AnonUnionType: 0){ _LL3597:
_temp3596=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3582)->f1; goto
_LL3587;} else{ goto _LL3588;} _LL3588: if(( unsigned int) _temp3582 > 4u?*((
int*) _temp3582) == Cyc_Absyn_StructType: 0){ _LL3604: _temp3603=(( struct Cyc_Absyn_StructType_struct*)
_temp3582)->f1; goto _LL3602; _LL3602: _temp3601=(( struct Cyc_Absyn_StructType_struct*)
_temp3582)->f2; goto _LL3599; _LL3599: _temp3598=(( struct Cyc_Absyn_StructType_struct*)
_temp3582)->f3; if( _temp3598 == 0){ goto _LL3590;} else{ _temp3600=* _temp3598;
goto _LL3589;}} else{ goto _LL3590;} _LL3590: if(( unsigned int) _temp3582 > 4u?*((
int*) _temp3582) == Cyc_Absyn_UnionType: 0){ _LL3611: _temp3610=(( struct Cyc_Absyn_UnionType_struct*)
_temp3582)->f1; goto _LL3609; _LL3609: _temp3608=(( struct Cyc_Absyn_UnionType_struct*)
_temp3582)->f2; goto _LL3606; _LL3606: _temp3605=(( struct Cyc_Absyn_UnionType_struct*)
_temp3582)->f3; if( _temp3605 == 0){ goto _LL3592;} else{ _temp3607=* _temp3605;
goto _LL3591;}} else{ goto _LL3592;} _LL3592: goto _LL3593; _LL3585: _temp3596=
_temp3594; goto _LL3587; _LL3587: { struct Cyc_Absyn_Structfield* _temp3612= Cyc_Absyn_lookup_field(
_temp3596, _temp3514); goto _LL3613; _LL3613: if( _temp3612 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3612))->width != 0: 0){ return({ struct _tuple7 _temp3614;
_temp3614.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3612))->tq).q_const;
_temp3614.f2=( Cyc_Tcutil_addressof_props( te, _temp3516)).f2; _temp3614;});}
return bogus_ans;} _LL3589: { struct Cyc_Absyn_Structfield* _temp3615= Cyc_Absyn_lookup_struct_field(
_temp3600, _temp3514); goto _LL3616; _LL3616: if( _temp3615 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3615))->width != 0: 0){ return({ struct _tuple7 _temp3617;
_temp3617.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3615))->tq).q_const;
_temp3617.f2=( Cyc_Tcutil_addressof_props( te, _temp3516)).f2; _temp3617;});}
return bogus_ans;} _LL3591: { struct Cyc_Absyn_Structfield* _temp3618= Cyc_Absyn_lookup_union_field(
_temp3607, _temp3514); goto _LL3619; _LL3619: if( _temp3618 != 0){ return({
struct _tuple7 _temp3620; _temp3620.f1=((( struct Cyc_Absyn_Structfield*)
_check_null( _temp3618))->tq).q_const; _temp3620.f2=( Cyc_Tcutil_addressof_props(
te, _temp3516)).f2; _temp3620;});} goto _LL3593;} _LL3593: return bogus_ans;
_LL3583:;} _LL3503: { void* _temp3621= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3520->topt))->v); struct Cyc_Absyn_PtrInfo _temp3627; struct
Cyc_Absyn_Conref* _temp3629; struct Cyc_Absyn_Tqual _temp3631; struct Cyc_Absyn_Conref*
_temp3633; void* _temp3635; void* _temp3637; _LL3623: if(( unsigned int)
_temp3621 > 4u?*(( int*) _temp3621) == Cyc_Absyn_PointerType: 0){ _LL3628:
_temp3627=(( struct Cyc_Absyn_PointerType_struct*) _temp3621)->f1; _LL3638:
_temp3637=( void*) _temp3627.elt_typ; goto _LL3636; _LL3636: _temp3635=( void*)
_temp3627.rgn_typ; goto _LL3634; _LL3634: _temp3633= _temp3627.nullable; goto
_LL3632; _LL3632: _temp3631= _temp3627.tq; goto _LL3630; _LL3630: _temp3629=
_temp3627.bounds; goto _LL3624;} else{ goto _LL3625;} _LL3625: goto _LL3626;
_LL3624: { void* _temp3639= Cyc_Tcutil_compress( _temp3637); struct Cyc_List_List*
_temp3651; struct Cyc_List_List* _temp3653; struct Cyc_Absyn_Structdecl**
_temp3655; struct Cyc_Absyn_Structdecl* _temp3657; struct Cyc_List_List*
_temp3658; struct _tuple1* _temp3660; struct Cyc_Absyn_Uniondecl** _temp3662;
struct Cyc_Absyn_Uniondecl* _temp3664; struct Cyc_List_List* _temp3665; struct
_tuple1* _temp3667; _LL3641: if(( unsigned int) _temp3639 > 4u?*(( int*)
_temp3639) == Cyc_Absyn_AnonStructType: 0){ _LL3652: _temp3651=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3639)->f1; goto _LL3642;} else{ goto _LL3643;} _LL3643: if(( unsigned int)
_temp3639 > 4u?*(( int*) _temp3639) == Cyc_Absyn_AnonUnionType: 0){ _LL3654:
_temp3653=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3639)->f1; goto
_LL3644;} else{ goto _LL3645;} _LL3645: if(( unsigned int) _temp3639 > 4u?*((
int*) _temp3639) == Cyc_Absyn_StructType: 0){ _LL3661: _temp3660=(( struct Cyc_Absyn_StructType_struct*)
_temp3639)->f1; goto _LL3659; _LL3659: _temp3658=(( struct Cyc_Absyn_StructType_struct*)
_temp3639)->f2; goto _LL3656; _LL3656: _temp3655=(( struct Cyc_Absyn_StructType_struct*)
_temp3639)->f3; if( _temp3655 == 0){ goto _LL3647;} else{ _temp3657=* _temp3655;
goto _LL3646;}} else{ goto _LL3647;} _LL3647: if(( unsigned int) _temp3639 > 4u?*((
int*) _temp3639) == Cyc_Absyn_UnionType: 0){ _LL3668: _temp3667=(( struct Cyc_Absyn_UnionType_struct*)
_temp3639)->f1; goto _LL3666; _LL3666: _temp3665=(( struct Cyc_Absyn_UnionType_struct*)
_temp3639)->f2; goto _LL3663; _LL3663: _temp3662=(( struct Cyc_Absyn_UnionType_struct*)
_temp3639)->f3; if( _temp3662 == 0){ goto _LL3649;} else{ _temp3664=* _temp3662;
goto _LL3648;}} else{ goto _LL3649;} _LL3649: goto _LL3650; _LL3642: _temp3653=
_temp3651; goto _LL3644; _LL3644: { struct Cyc_Absyn_Structfield* _temp3669= Cyc_Absyn_lookup_field(
_temp3653, _temp3518); goto _LL3670; _LL3670: if( _temp3669 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3669))->width != 0: 0){ return({ struct _tuple7 _temp3671;
_temp3671.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3669))->tq).q_const;
_temp3671.f2= _temp3635; _temp3671;});} return bogus_ans;} _LL3646: { struct Cyc_Absyn_Structfield*
_temp3672= Cyc_Absyn_lookup_struct_field( _temp3657, _temp3518); goto _LL3673;
_LL3673: if( _temp3672 != 0?(( struct Cyc_Absyn_Structfield*) _check_null(
_temp3672))->width != 0: 0){ return({ struct _tuple7 _temp3674; _temp3674.f1=(((
struct Cyc_Absyn_Structfield*) _check_null( _temp3672))->tq).q_const; _temp3674.f2=
_temp3635; _temp3674;});} return bogus_ans;} _LL3648: { struct Cyc_Absyn_Structfield*
_temp3675= Cyc_Absyn_lookup_union_field( _temp3664, _temp3518); goto _LL3676;
_LL3676: if( _temp3675 != 0){ return({ struct _tuple7 _temp3677; _temp3677.f1=(((
struct Cyc_Absyn_Structfield*) _check_null( _temp3675))->tq).q_const; _temp3677.f2=
_temp3635; _temp3677;});} return bogus_ans;} _LL3650: return bogus_ans; _LL3640:;}
_LL3626: return bogus_ans; _LL3622:;} _LL3505: { void* _temp3678= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp3522->topt))->v); struct Cyc_Absyn_PtrInfo
_temp3684; struct Cyc_Absyn_Conref* _temp3686; struct Cyc_Absyn_Tqual _temp3688;
struct Cyc_Absyn_Conref* _temp3690; void* _temp3692; void* _temp3694; _LL3680:
if(( unsigned int) _temp3678 > 4u?*(( int*) _temp3678) == Cyc_Absyn_PointerType:
0){ _LL3685: _temp3684=(( struct Cyc_Absyn_PointerType_struct*) _temp3678)->f1;
_LL3695: _temp3694=( void*) _temp3684.elt_typ; goto _LL3693; _LL3693: _temp3692=(
void*) _temp3684.rgn_typ; goto _LL3691; _LL3691: _temp3690= _temp3684.nullable;
goto _LL3689; _LL3689: _temp3688= _temp3684.tq; goto _LL3687; _LL3687: _temp3686=
_temp3684.bounds; goto _LL3681;} else{ goto _LL3682;} _LL3682: goto _LL3683;
_LL3681: return({ struct _tuple7 _temp3696; _temp3696.f1= _temp3688.q_const;
_temp3696.f2= _temp3692; _temp3696;}); _LL3683: return bogus_ans; _LL3679:;}
_LL3507: { void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3526->topt))->v); void* _temp3697= t; struct Cyc_Absyn_Exp*
_temp3707; struct Cyc_Absyn_Tqual _temp3709; void* _temp3711; struct Cyc_List_List*
_temp3713; struct Cyc_Absyn_PtrInfo _temp3715; struct Cyc_Absyn_Conref*
_temp3717; struct Cyc_Absyn_Tqual _temp3719; struct Cyc_Absyn_Conref* _temp3721;
void* _temp3723; void* _temp3725; _LL3699: if(( unsigned int) _temp3697 > 4u?*((
int*) _temp3697) == Cyc_Absyn_ArrayType: 0){ _LL3712: _temp3711=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3697)->f1; goto _LL3710; _LL3710:
_temp3709=(( struct Cyc_Absyn_ArrayType_struct*) _temp3697)->f2; goto _LL3708;
_LL3708: _temp3707=(( struct Cyc_Absyn_ArrayType_struct*) _temp3697)->f3; goto
_LL3700;} else{ goto _LL3701;} _LL3701: if(( unsigned int) _temp3697 > 4u?*((
int*) _temp3697) == Cyc_Absyn_TupleType: 0){ _LL3714: _temp3713=(( struct Cyc_Absyn_TupleType_struct*)
_temp3697)->f1; goto _LL3702;} else{ goto _LL3703;} _LL3703: if(( unsigned int)
_temp3697 > 4u?*(( int*) _temp3697) == Cyc_Absyn_PointerType: 0){ _LL3716:
_temp3715=(( struct Cyc_Absyn_PointerType_struct*) _temp3697)->f1; _LL3726:
_temp3725=( void*) _temp3715.elt_typ; goto _LL3724; _LL3724: _temp3723=( void*)
_temp3715.rgn_typ; goto _LL3722; _LL3722: _temp3721= _temp3715.nullable; goto
_LL3720; _LL3720: _temp3719= _temp3715.tq; goto _LL3718; _LL3718: _temp3717=
_temp3715.bounds; goto _LL3704;} else{ goto _LL3705;} _LL3705: goto _LL3706;
_LL3700: return({ struct _tuple7 _temp3727; _temp3727.f1= _temp3709.q_const;
_temp3727.f2=( Cyc_Tcutil_addressof_props( te, _temp3526)).f2; _temp3727;});
_LL3702: { struct _tuple4* _temp3728= Cyc_Absyn_lookup_tuple_field( _temp3713,(
int) Cyc_Evexp_eval_const_uint_exp( _temp3524)); goto _LL3729; _LL3729: if(
_temp3728 != 0){ return({ struct _tuple7 _temp3730; _temp3730.f1=((*(( struct
_tuple4*) _check_null( _temp3728))).f1).q_const; _temp3730.f2=( Cyc_Tcutil_addressof_props(
te, _temp3526)).f2; _temp3730;});} return bogus_ans;} _LL3704: return({ struct
_tuple7 _temp3731; _temp3731.f1= _temp3719.q_const; _temp3731.f2= _temp3723;
_temp3731;}); _LL3706: return bogus_ans; _LL3698:;} _LL3509: Cyc_Tcutil_terr( e->loc,({
unsigned char* _temp3732=( unsigned char*)"unary & applied to non-lvalue";
struct _tagged_string _temp3733; _temp3733.curr= _temp3732; _temp3733.base=
_temp3732; _temp3733.last_plus_one= _temp3732 + 30; _temp3733;})); return
bogus_ans; _LL3497:;} void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv* te,
void* e_typ, struct Cyc_Absyn_Exp* e){ void* _temp3735= Cyc_Tcutil_compress(
e_typ); struct Cyc_Absyn_Exp* _temp3741; struct Cyc_Absyn_Tqual _temp3743; void*
_temp3745; _LL3737: if(( unsigned int) _temp3735 > 4u?*(( int*) _temp3735) ==
Cyc_Absyn_ArrayType: 0){ _LL3746: _temp3745=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3735)->f1; goto _LL3744; _LL3744: _temp3743=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3735)->f2; goto _LL3742; _LL3742: _temp3741=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3735)->f3; goto _LL3738;} else{ goto _LL3739;} _LL3739: goto _LL3740;
_LL3738: { void* _temp3749; int _temp3751; struct _tuple7 _temp3747= Cyc_Tcutil_addressof_props(
te, e); _LL3752: _temp3751= _temp3747.f1; goto _LL3750; _LL3750: _temp3749=
_temp3747.f2; goto _LL3748; _LL3748: return Cyc_Absyn_atb_typ( _temp3745,
_temp3749, _temp3743,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3753=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3753[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3754; _temp3754.tag= Cyc_Absyn_Upper_b;
_temp3754.f1= e; _temp3754;}); _temp3753;}));} _LL3740: return e_typ; _LL3736:;}
void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment* loc, unsigned int i,
struct Cyc_Absyn_Conref* b){ b=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( b);{ void* _temp3755=( void*) b->v; void*
_temp3763; void* _temp3765; struct Cyc_Absyn_Exp* _temp3767; _LL3757: if((
unsigned int) _temp3755 > 1u?*(( int*) _temp3755) == Cyc_Absyn_Eq_constr: 0){
_LL3764: _temp3763=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3755)->f1;
if( _temp3763 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3758;} else{ goto _LL3759;}}
else{ goto _LL3759;} _LL3759: if(( unsigned int) _temp3755 > 1u?*(( int*)
_temp3755) == Cyc_Absyn_Eq_constr: 0){ _LL3766: _temp3765=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3755)->f1; if(( unsigned int) _temp3765 > 1u?*(( int*) _temp3765) == Cyc_Absyn_Upper_b:
0){ _LL3768: _temp3767=(( struct Cyc_Absyn_Upper_b_struct*) _temp3765)->f1; goto
_LL3760;} else{ goto _LL3761;}} else{ goto _LL3761;} _LL3761: goto _LL3762;
_LL3758: return; _LL3760: if( Cyc_Evexp_eval_const_uint_exp( _temp3767) <= i){
Cyc_Tcutil_terr( loc,({ unsigned char* _temp3769=( unsigned char*)"dereference is out of bounds";
struct _tagged_string _temp3770; _temp3770.curr= _temp3769; _temp3770.base=
_temp3769; _temp3770.last_plus_one= _temp3769 + 29; _temp3770;}));} return;
_LL3762:( void*)( b->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3771=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3771[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3772; _temp3772.tag= Cyc_Absyn_Eq_constr;
_temp3772.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3773=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3773[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3774; _temp3774.tag= Cyc_Absyn_Upper_b;
_temp3774.f1= Cyc_Absyn_uint_exp( i + 1, 0); _temp3774;}); _temp3773;}));
_temp3772;}); _temp3771;}))); return; _LL3756:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_bits_only( void* t){ void* _temp3775= Cyc_Tcutil_compress(
t); void* _temp3801; void* _temp3803; struct Cyc_Absyn_Enumdecl* _temp3805;
struct _tuple1* _temp3807; struct Cyc_Absyn_Exp* _temp3809; struct Cyc_Absyn_Tqual
_temp3811; void* _temp3813; struct Cyc_List_List* _temp3815; struct Cyc_Absyn_Structdecl**
_temp3817; struct Cyc_List_List* _temp3819; struct _tuple1* _temp3821; struct
Cyc_Absyn_Uniondecl** _temp3823; struct Cyc_List_List* _temp3825; struct _tuple1*
_temp3827; struct Cyc_List_List* _temp3829; struct Cyc_List_List* _temp3831;
_LL3777: if( _temp3775 ==( void*) Cyc_Absyn_VoidType){ goto _LL3778;} else{ goto
_LL3779;} _LL3779: if(( unsigned int) _temp3775 > 4u?*(( int*) _temp3775) == Cyc_Absyn_IntType:
0){ _LL3804: _temp3803=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3775)->f1;
goto _LL3802; _LL3802: _temp3801=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3775)->f2; goto _LL3780;} else{ goto _LL3781;} _LL3781: if( _temp3775 ==(
void*) Cyc_Absyn_FloatType){ goto _LL3782;} else{ goto _LL3783;} _LL3783: if(
_temp3775 ==( void*) Cyc_Absyn_DoubleType){ goto _LL3784;} else{ goto _LL3785;}
_LL3785: if(( unsigned int) _temp3775 > 4u?*(( int*) _temp3775) == Cyc_Absyn_EnumType:
0){ _LL3808: _temp3807=(( struct Cyc_Absyn_EnumType_struct*) _temp3775)->f1;
goto _LL3806; _LL3806: _temp3805=(( struct Cyc_Absyn_EnumType_struct*) _temp3775)->f2;
goto _LL3786;} else{ goto _LL3787;} _LL3787: if(( unsigned int) _temp3775 > 4u?*((
int*) _temp3775) == Cyc_Absyn_ArrayType: 0){ _LL3814: _temp3813=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3775)->f1; goto _LL3812; _LL3812:
_temp3811=(( struct Cyc_Absyn_ArrayType_struct*) _temp3775)->f2; goto _LL3810;
_LL3810: _temp3809=(( struct Cyc_Absyn_ArrayType_struct*) _temp3775)->f3; goto
_LL3788;} else{ goto _LL3789;} _LL3789: if(( unsigned int) _temp3775 > 4u?*((
int*) _temp3775) == Cyc_Absyn_TupleType: 0){ _LL3816: _temp3815=(( struct Cyc_Absyn_TupleType_struct*)
_temp3775)->f1; goto _LL3790;} else{ goto _LL3791;} _LL3791: if(( unsigned int)
_temp3775 > 4u?*(( int*) _temp3775) == Cyc_Absyn_StructType: 0){ _LL3822:
_temp3821=(( struct Cyc_Absyn_StructType_struct*) _temp3775)->f1; goto _LL3820;
_LL3820: _temp3819=(( struct Cyc_Absyn_StructType_struct*) _temp3775)->f2; goto
_LL3818; _LL3818: _temp3817=(( struct Cyc_Absyn_StructType_struct*) _temp3775)->f3;
goto _LL3792;} else{ goto _LL3793;} _LL3793: if(( unsigned int) _temp3775 > 4u?*((
int*) _temp3775) == Cyc_Absyn_UnionType: 0){ _LL3828: _temp3827=(( struct Cyc_Absyn_UnionType_struct*)
_temp3775)->f1; goto _LL3826; _LL3826: _temp3825=(( struct Cyc_Absyn_UnionType_struct*)
_temp3775)->f2; goto _LL3824; _LL3824: _temp3823=(( struct Cyc_Absyn_UnionType_struct*)
_temp3775)->f3; goto _LL3794;} else{ goto _LL3795;} _LL3795: if(( unsigned int)
_temp3775 > 4u?*(( int*) _temp3775) == Cyc_Absyn_AnonStructType: 0){ _LL3830:
_temp3829=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3775)->f1; goto
_LL3796;} else{ goto _LL3797;} _LL3797: if(( unsigned int) _temp3775 > 4u?*((
int*) _temp3775) == Cyc_Absyn_AnonUnionType: 0){ _LL3832: _temp3831=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3775)->f1; goto _LL3798;} else{ goto
_LL3799;} _LL3799: goto _LL3800; _LL3778: return 1; _LL3780: return 1; _LL3782:
return 1; _LL3784: return 1; _LL3786: return 0; _LL3788: return Cyc_Tcutil_bits_only(
_temp3813); _LL3790: for( 0; _temp3815 != 0; _temp3815=(( struct Cyc_List_List*)
_check_null( _temp3815))->tl){ if( ! Cyc_Tcutil_bits_only((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp3815))->hd)).f2)){ return 0;}} return 1;
_LL3792: if( _temp3817 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* _temp3833=*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp3817)); goto _LL3834; _LL3834:
if( _temp3833->fields == 0){ return 0;}{ struct _RegionHandle _temp3835=
_new_region(); struct _RegionHandle* rgn=& _temp3835; _push_region( rgn);{
struct Cyc_List_List* _temp3836=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp3833->tvs, _temp3819); goto _LL3837; _LL3837:{
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp3833->fields))->v; for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute( rgn,
_temp3836,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3838= 0; _npop_handler( 0u); return
_temp3838;}}}{ int _temp3839= 1; _npop_handler( 0u); return _temp3839;}};
_pop_region(& _temp3835);}} _LL3794: if( _temp3823 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
_temp3840=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp3823)); goto
_LL3841; _LL3841: if( _temp3840->fields == 0){ return 0;}{ struct _RegionHandle
_temp3842= _new_region(); struct _RegionHandle* rgn=& _temp3842; _push_region(
rgn);{ struct Cyc_List_List* _temp3843=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp3840->tvs, _temp3825); goto _LL3844; _LL3844:{
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp3840->fields))->v; for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute( rgn,
_temp3843,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3845= 0; _npop_handler( 0u); return
_temp3845;}}}{ int _temp3846= 1; _npop_handler( 0u); return _temp3846;}};
_pop_region(& _temp3842);}} _LL3796: _temp3831= _temp3829; goto _LL3798; _LL3798:
for( 0; _temp3831 != 0; _temp3831=(( struct Cyc_List_List*) _check_null(
_temp3831))->tl){ if( ! Cyc_Tcutil_bits_only(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp3831))->hd)->type)){ return 0;}} return
1; _LL3800: return 0; _LL3776:;} struct _tuple20{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; static int Cyc_Tcutil_cnst_exp( struct Cyc_Tcenv_Tenv*
te, int var_okay, struct Cyc_Absyn_Exp* e){ void* _temp3847=( void*) e->r; void*
_temp3887; void* _temp3889; struct Cyc_Absyn_Exp* _temp3891; struct Cyc_Absyn_Enumfield*
_temp3893; struct Cyc_Absyn_Enumdecl* _temp3895; struct _tuple1* _temp3897; void*
_temp3899; struct _tuple1* _temp3901; struct Cyc_Absyn_Exp* _temp3903; struct
Cyc_Absyn_Exp* _temp3905; struct Cyc_Absyn_Exp* _temp3907; struct Cyc_Absyn_Exp*
_temp3909; struct Cyc_Absyn_Exp* _temp3911; struct Cyc_Absyn_Exp* _temp3913;
struct Cyc_List_List* _temp3915; struct Cyc_Absyn_Exp* _temp3917; struct Cyc_Absyn_Exp*
_temp3919; void* _temp3921; struct Cyc_Absyn_Exp* _temp3923; struct Cyc_Absyn_Exp*
_temp3925; struct Cyc_Absyn_Exp* _temp3927; struct Cyc_Absyn_Vardecl* _temp3929;
struct Cyc_List_List* _temp3931; struct Cyc_List_List* _temp3933; void*
_temp3935; struct Cyc_Absyn_Structdecl* _temp3937; struct Cyc_List_List*
_temp3939; struct Cyc_Core_Opt* _temp3941; struct _tuple1* _temp3943; struct Cyc_List_List*
_temp3945; void* _temp3947; struct Cyc_List_List* _temp3949; struct Cyc_Absyn_Tunionfield*
_temp3951; struct Cyc_Absyn_Tuniondecl* _temp3953; struct Cyc_List_List*
_temp3955; struct Cyc_Core_Opt* _temp3957; struct Cyc_Core_Opt* _temp3959;
_LL3849: if(*(( int*) _temp3847) == Cyc_Absyn_Const_e){ _LL3888: _temp3887=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp3847)->f1; goto _LL3850;} else{
goto _LL3851;} _LL3851: if(*(( int*) _temp3847) == Cyc_Absyn_Sizeoftyp_e){
_LL3890: _temp3889=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp3847)->f1;
goto _LL3852;} else{ goto _LL3853;} _LL3853: if(*(( int*) _temp3847) == Cyc_Absyn_Sizeofexp_e){
_LL3892: _temp3891=(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp3847)->f1; goto
_LL3854;} else{ goto _LL3855;} _LL3855: if(*(( int*) _temp3847) == Cyc_Absyn_Enum_e){
_LL3898: _temp3897=(( struct Cyc_Absyn_Enum_e_struct*) _temp3847)->f1; goto
_LL3896; _LL3896: _temp3895=(( struct Cyc_Absyn_Enum_e_struct*) _temp3847)->f2;
goto _LL3894; _LL3894: _temp3893=(( struct Cyc_Absyn_Enum_e_struct*) _temp3847)->f3;
goto _LL3856;} else{ goto _LL3857;} _LL3857: if(*(( int*) _temp3847) == Cyc_Absyn_Var_e){
_LL3902: _temp3901=(( struct Cyc_Absyn_Var_e_struct*) _temp3847)->f1; goto
_LL3900; _LL3900: _temp3899=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp3847)->f2;
goto _LL3858;} else{ goto _LL3859;} _LL3859: if(*(( int*) _temp3847) == Cyc_Absyn_Conditional_e){
_LL3908: _temp3907=(( struct Cyc_Absyn_Conditional_e_struct*) _temp3847)->f1;
goto _LL3906; _LL3906: _temp3905=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3847)->f2; goto _LL3904; _LL3904: _temp3903=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3847)->f3; goto _LL3860;} else{ goto _LL3861;} _LL3861: if(*(( int*)
_temp3847) == Cyc_Absyn_SeqExp_e){ _LL3912: _temp3911=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3847)->f1; goto _LL3910; _LL3910: _temp3909=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3847)->f2; goto _LL3862;} else{ goto _LL3863;} _LL3863: if(*(( int*)
_temp3847) == Cyc_Absyn_NoInstantiate_e){ _LL3914: _temp3913=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp3847)->f1; goto _LL3864;} else{ goto _LL3865;} _LL3865: if(*(( int*)
_temp3847) == Cyc_Absyn_Instantiate_e){ _LL3918: _temp3917=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3847)->f1; goto _LL3916; _LL3916: _temp3915=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3847)->f2; goto _LL3866;} else{ goto _LL3867;} _LL3867: if(*(( int*)
_temp3847) == Cyc_Absyn_Cast_e){ _LL3922: _temp3921=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp3847)->f1; goto _LL3920; _LL3920: _temp3919=(( struct Cyc_Absyn_Cast_e_struct*)
_temp3847)->f2; goto _LL3868;} else{ goto _LL3869;} _LL3869: if(*(( int*)
_temp3847) == Cyc_Absyn_Address_e){ _LL3924: _temp3923=(( struct Cyc_Absyn_Address_e_struct*)
_temp3847)->f1; goto _LL3870;} else{ goto _LL3871;} _LL3871: if(*(( int*)
_temp3847) == Cyc_Absyn_Comprehension_e){ _LL3930: _temp3929=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3847)->f1; goto _LL3928; _LL3928: _temp3927=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3847)->f2; goto _LL3926; _LL3926: _temp3925=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3847)->f3; goto _LL3872;} else{ goto _LL3873;} _LL3873: if(*(( int*)
_temp3847) == Cyc_Absyn_Array_e){ _LL3932: _temp3931=(( struct Cyc_Absyn_Array_e_struct*)
_temp3847)->f1; goto _LL3874;} else{ goto _LL3875;} _LL3875: if(*(( int*)
_temp3847) == Cyc_Absyn_AnonStruct_e){ _LL3936: _temp3935=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3847)->f1; goto _LL3934; _LL3934: _temp3933=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3847)->f2; goto _LL3876;} else{ goto _LL3877;} _LL3877: if(*(( int*)
_temp3847) == Cyc_Absyn_Struct_e){ _LL3944: _temp3943=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3847)->f1; goto _LL3942; _LL3942: _temp3941=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3847)->f2; goto _LL3940; _LL3940: _temp3939=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3847)->f3; goto _LL3938; _LL3938: _temp3937=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3847)->f4; goto _LL3878;} else{ goto _LL3879;} _LL3879: if(*(( int*)
_temp3847) == Cyc_Absyn_Primop_e){ _LL3948: _temp3947=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp3847)->f1; goto _LL3946; _LL3946: _temp3945=(( struct Cyc_Absyn_Primop_e_struct*)
_temp3847)->f2; goto _LL3880;} else{ goto _LL3881;} _LL3881: if(*(( int*)
_temp3847) == Cyc_Absyn_Tuple_e){ _LL3950: _temp3949=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp3847)->f1; goto _LL3882;} else{ goto _LL3883;} _LL3883: if(*(( int*)
_temp3847) == Cyc_Absyn_Tunion_e){ _LL3960: _temp3959=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3847)->f1; goto _LL3958; _LL3958: _temp3957=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3847)->f2; goto _LL3956; _LL3956: _temp3955=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3847)->f3; goto _LL3954; _LL3954: _temp3953=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3847)->f4; goto _LL3952; _LL3952: _temp3951=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3847)->f5; goto _LL3884;} else{ goto _LL3885;} _LL3885: goto _LL3886;
_LL3850: return 1; _LL3852: return 1; _LL3854: return 1; _LL3856: return 1;
_LL3858: { struct _handler_cons _temp3961; _push_handler(& _temp3961);{ int
_temp3963= 0; if( setjmp( _temp3961.handler)){ _temp3963= 1;} if( ! _temp3963){{
void* _temp3964= Cyc_Tcenv_lookup_ordinary( te, e->loc, _temp3901); void*
_temp3972; struct Cyc_Absyn_Fndecl* _temp3974; void* _temp3976; struct Cyc_Absyn_Vardecl*
_temp3978; _LL3966: if(*(( int*) _temp3964) == Cyc_Tcenv_VarRes){ _LL3973:
_temp3972=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp3964)->f1; if((
unsigned int) _temp3972 > 1u?*(( int*) _temp3972) == Cyc_Absyn_Funname_b: 0){
_LL3975: _temp3974=(( struct Cyc_Absyn_Funname_b_struct*) _temp3972)->f1; goto
_LL3967;} else{ goto _LL3968;}} else{ goto _LL3968;} _LL3968: if(*(( int*)
_temp3964) == Cyc_Tcenv_VarRes){ _LL3977: _temp3976=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3964)->f1; if(( unsigned int) _temp3976 > 1u?*(( int*) _temp3976) == Cyc_Absyn_Global_b:
0){ _LL3979: _temp3978=(( struct Cyc_Absyn_Global_b_struct*) _temp3976)->f1;
goto _LL3969;} else{ goto _LL3970;}} else{ goto _LL3970;} _LL3970: goto _LL3971;
_LL3967: { int _temp3980= 1; _npop_handler( 0u); return _temp3980;} _LL3969: {
void* _temp3981= Cyc_Tcutil_compress(( void*) _temp3978->type); struct Cyc_Absyn_Exp*
_temp3987; struct Cyc_Absyn_Tqual _temp3989; void* _temp3991; _LL3983: if((
unsigned int) _temp3981 > 4u?*(( int*) _temp3981) == Cyc_Absyn_ArrayType: 0){
_LL3992: _temp3991=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3981)->f1;
goto _LL3990; _LL3990: _temp3989=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3981)->f2; goto _LL3988; _LL3988: _temp3987=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3981)->f3; goto _LL3984;} else{ goto _LL3985;} _LL3985: goto _LL3986;
_LL3984: { int _temp3993= 1; _npop_handler( 0u); return _temp3993;} _LL3986: {
int _temp3994= var_okay; _npop_handler( 0u); return _temp3994;} _LL3982:;}
_LL3971: { int _temp3995= var_okay; _npop_handler( 0u); return _temp3995;}
_LL3965:;}; _pop_handler();} else{ void* _temp3962=( void*) _exn_thrown; void*
_temp3997= _temp3962; _LL3999: if( _temp3997 == Cyc_Dict_Absent){ goto _LL4000;}
else{ goto _LL4001;} _LL4001: goto _LL4002; _LL4000: return 0; _LL4002:( void)
_throw( _temp3997); _LL3998:;}}} _LL3860: return( Cyc_Tcutil_cnst_exp( te, 0,
_temp3907)? Cyc_Tcutil_cnst_exp( te, 0, _temp3905): 0)? Cyc_Tcutil_cnst_exp( te,
0, _temp3903): 0; _LL3862: return Cyc_Tcutil_cnst_exp( te, 0, _temp3911)? Cyc_Tcutil_cnst_exp(
te, 0, _temp3909): 0; _LL3864: _temp3917= _temp3913; goto _LL3866; _LL3866:
return Cyc_Tcutil_cnst_exp( te, var_okay, _temp3917); _LL3868: return Cyc_Tcutil_cnst_exp(
te, var_okay, _temp3919); _LL3870: return Cyc_Tcutil_cnst_exp( te, 1, _temp3923);
_LL3872: return Cyc_Tcutil_cnst_exp( te, 0, _temp3927)? Cyc_Tcutil_cnst_exp( te,
0, _temp3925): 0; _LL3874: _temp3933= _temp3931; goto _LL3876; _LL3876:
_temp3939= _temp3933; goto _LL3878; _LL3878: for( 0; _temp3939 != 0; _temp3939=((
struct Cyc_List_List*) _check_null( _temp3939))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,(*(( struct _tuple20*)(( struct Cyc_List_List*) _check_null( _temp3939))->hd)).f2)){
return 0;}} return 1; _LL3880: _temp3949= _temp3945; goto _LL3882; _LL3882:
_temp3955= _temp3949; goto _LL3884; _LL3884: for( 0; _temp3955 != 0; _temp3955=((
struct Cyc_List_List*) _check_null( _temp3955))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp3955))->hd)){
return 0;}} return 1; _LL3886: return 0; _LL3848:;} int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcutil_cnst_exp(
te, 0, e);} static int Cyc_Tcutil_fields_support_default( struct Cyc_List_List*
tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs); int Cyc_Tcutil_supports_default(
void* t){ void* _temp4003= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt*
_temp4041; int _temp4043; struct Cyc_Core_Opt* _temp4045; struct Cyc_Core_Opt*
_temp4047; struct Cyc_Absyn_Tvar* _temp4049; struct Cyc_Absyn_TunionInfo
_temp4051; struct Cyc_Absyn_TunionFieldInfo _temp4053; struct Cyc_Absyn_PtrInfo
_temp4055; struct Cyc_Absyn_Conref* _temp4057; struct Cyc_Absyn_Tqual _temp4059;
struct Cyc_Absyn_Conref* _temp4061; void* _temp4063; void* _temp4065; void*
_temp4067; void* _temp4069; struct Cyc_Absyn_Exp* _temp4071; struct Cyc_Absyn_Tqual
_temp4073; void* _temp4075; struct Cyc_Absyn_FnInfo _temp4077; struct Cyc_List_List*
_temp4079; struct Cyc_Absyn_Structdecl** _temp4081; struct Cyc_List_List*
_temp4083; struct _tuple1* _temp4085; struct Cyc_Absyn_Uniondecl** _temp4087;
struct Cyc_List_List* _temp4089; struct _tuple1* _temp4091; struct Cyc_List_List*
_temp4093; struct Cyc_List_List* _temp4095; struct Cyc_Absyn_Enumdecl* _temp4097;
struct _tuple1* _temp4099; _LL4005: if( _temp4003 ==( void*) Cyc_Absyn_VoidType){
goto _LL4006;} else{ goto _LL4007;} _LL4007: if(( unsigned int) _temp4003 > 4u?*((
int*) _temp4003) == Cyc_Absyn_Evar: 0){ _LL4048: _temp4047=(( struct Cyc_Absyn_Evar_struct*)
_temp4003)->f1; goto _LL4046; _LL4046: _temp4045=(( struct Cyc_Absyn_Evar_struct*)
_temp4003)->f2; goto _LL4044; _LL4044: _temp4043=(( struct Cyc_Absyn_Evar_struct*)
_temp4003)->f3; goto _LL4042; _LL4042: _temp4041=(( struct Cyc_Absyn_Evar_struct*)
_temp4003)->f4; goto _LL4008;} else{ goto _LL4009;} _LL4009: if(( unsigned int)
_temp4003 > 4u?*(( int*) _temp4003) == Cyc_Absyn_VarType: 0){ _LL4050: _temp4049=((
struct Cyc_Absyn_VarType_struct*) _temp4003)->f1; goto _LL4010;} else{ goto
_LL4011;} _LL4011: if(( unsigned int) _temp4003 > 4u?*(( int*) _temp4003) == Cyc_Absyn_TunionType:
0){ _LL4052: _temp4051=(( struct Cyc_Absyn_TunionType_struct*) _temp4003)->f1;
goto _LL4012;} else{ goto _LL4013;} _LL4013: if(( unsigned int) _temp4003 > 4u?*((
int*) _temp4003) == Cyc_Absyn_TunionFieldType: 0){ _LL4054: _temp4053=(( struct
Cyc_Absyn_TunionFieldType_struct*) _temp4003)->f1; goto _LL4014;} else{ goto
_LL4015;} _LL4015: if(( unsigned int) _temp4003 > 4u?*(( int*) _temp4003) == Cyc_Absyn_PointerType:
0){ _LL4056: _temp4055=(( struct Cyc_Absyn_PointerType_struct*) _temp4003)->f1;
_LL4066: _temp4065=( void*) _temp4055.elt_typ; goto _LL4064; _LL4064: _temp4063=(
void*) _temp4055.rgn_typ; goto _LL4062; _LL4062: _temp4061= _temp4055.nullable;
goto _LL4060; _LL4060: _temp4059= _temp4055.tq; goto _LL4058; _LL4058: _temp4057=
_temp4055.bounds; goto _LL4016;} else{ goto _LL4017;} _LL4017: if(( unsigned int)
_temp4003 > 4u?*(( int*) _temp4003) == Cyc_Absyn_IntType: 0){ _LL4070: _temp4069=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp4003)->f1; goto _LL4068; _LL4068:
_temp4067=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp4003)->f2; goto
_LL4018;} else{ goto _LL4019;} _LL4019: if( _temp4003 ==( void*) Cyc_Absyn_FloatType){
goto _LL4020;} else{ goto _LL4021;} _LL4021: if( _temp4003 ==( void*) Cyc_Absyn_DoubleType){
goto _LL4022;} else{ goto _LL4023;} _LL4023: if(( unsigned int) _temp4003 > 4u?*((
int*) _temp4003) == Cyc_Absyn_ArrayType: 0){ _LL4076: _temp4075=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp4003)->f1; goto _LL4074; _LL4074:
_temp4073=(( struct Cyc_Absyn_ArrayType_struct*) _temp4003)->f2; goto _LL4072;
_LL4072: _temp4071=(( struct Cyc_Absyn_ArrayType_struct*) _temp4003)->f3; goto
_LL4024;} else{ goto _LL4025;} _LL4025: if(( unsigned int) _temp4003 > 4u?*((
int*) _temp4003) == Cyc_Absyn_FnType: 0){ _LL4078: _temp4077=(( struct Cyc_Absyn_FnType_struct*)
_temp4003)->f1; goto _LL4026;} else{ goto _LL4027;} _LL4027: if(( unsigned int)
_temp4003 > 4u?*(( int*) _temp4003) == Cyc_Absyn_TupleType: 0){ _LL4080:
_temp4079=(( struct Cyc_Absyn_TupleType_struct*) _temp4003)->f1; goto _LL4028;}
else{ goto _LL4029;} _LL4029: if(( unsigned int) _temp4003 > 4u?*(( int*)
_temp4003) == Cyc_Absyn_StructType: 0){ _LL4086: _temp4085=(( struct Cyc_Absyn_StructType_struct*)
_temp4003)->f1; goto _LL4084; _LL4084: _temp4083=(( struct Cyc_Absyn_StructType_struct*)
_temp4003)->f2; goto _LL4082; _LL4082: _temp4081=(( struct Cyc_Absyn_StructType_struct*)
_temp4003)->f3; goto _LL4030;} else{ goto _LL4031;} _LL4031: if(( unsigned int)
_temp4003 > 4u?*(( int*) _temp4003) == Cyc_Absyn_UnionType: 0){ _LL4092:
_temp4091=(( struct Cyc_Absyn_UnionType_struct*) _temp4003)->f1; goto _LL4090;
_LL4090: _temp4089=(( struct Cyc_Absyn_UnionType_struct*) _temp4003)->f2; goto
_LL4088; _LL4088: _temp4087=(( struct Cyc_Absyn_UnionType_struct*) _temp4003)->f3;
goto _LL4032;} else{ goto _LL4033;} _LL4033: if(( unsigned int) _temp4003 > 4u?*((
int*) _temp4003) == Cyc_Absyn_AnonStructType: 0){ _LL4094: _temp4093=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp4003)->f1; goto _LL4034;} else{ goto
_LL4035;} _LL4035: if(( unsigned int) _temp4003 > 4u?*(( int*) _temp4003) == Cyc_Absyn_AnonUnionType:
0){ _LL4096: _temp4095=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp4003)->f1;
goto _LL4036;} else{ goto _LL4037;} _LL4037: if(( unsigned int) _temp4003 > 4u?*((
int*) _temp4003) == Cyc_Absyn_EnumType: 0){ _LL4100: _temp4099=(( struct Cyc_Absyn_EnumType_struct*)
_temp4003)->f1; goto _LL4098; _LL4098: _temp4097=(( struct Cyc_Absyn_EnumType_struct*)
_temp4003)->f2; goto _LL4038;} else{ goto _LL4039;} _LL4039: goto _LL4040;
_LL4006: return 1; _LL4008: return 0; _LL4010: return 0; _LL4012: return 0;
_LL4014: return 0; _LL4016: { void* _temp4101=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp4057))->v; void*
_temp4109; void* _temp4111; struct Cyc_Absyn_Exp* _temp4113; _LL4103: if((
unsigned int) _temp4101 > 1u?*(( int*) _temp4101) == Cyc_Absyn_Eq_constr: 0){
_LL4110: _temp4109=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp4101)->f1;
if( _temp4109 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL4104;} else{ goto _LL4105;}}
else{ goto _LL4105;} _LL4105: if(( unsigned int) _temp4101 > 1u?*(( int*)
_temp4101) == Cyc_Absyn_Eq_constr: 0){ _LL4112: _temp4111=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp4101)->f1; if(( unsigned int) _temp4111 > 1u?*(( int*) _temp4111) == Cyc_Absyn_Upper_b:
0){ _LL4114: _temp4113=(( struct Cyc_Absyn_Upper_b_struct*) _temp4111)->f1; goto
_LL4106;} else{ goto _LL4107;}} else{ goto _LL4107;} _LL4107: goto _LL4108;
_LL4104: return 1; _LL4106: { void* _temp4115=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp4061))->v; int
_temp4121; _LL4117: if(( unsigned int) _temp4115 > 1u?*(( int*) _temp4115) ==
Cyc_Absyn_Eq_constr: 0){ _LL4122: _temp4121=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp4115)->f1; goto _LL4118;} else{ goto _LL4119;} _LL4119: goto _LL4120;
_LL4118: return _temp4121; _LL4120: return 0; _LL4116:;} _LL4108: return 0;
_LL4102:;} _LL4018: return 1; _LL4020: return 1; _LL4022: return 1; _LL4024:
return Cyc_Tcutil_supports_default( _temp4075); _LL4026: return 0; _LL4028: for(
0; _temp4079 != 0; _temp4079=(( struct Cyc_List_List*) _check_null( _temp4079))->tl){
if( ! Cyc_Tcutil_supports_default((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp4079))->hd)).f2)){ return 0;}} return 1; _LL4030: if(
_temp4081 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp4081)); if( sd->fields == 0){ return 0;} return Cyc_Tcutil_fields_support_default(
sd->tvs, _temp4083,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
sd->fields))->v);} _LL4032: if( _temp4087 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp4087)); if( ud->fields ==
0){ return 0;} return Cyc_Tcutil_fields_support_default( ud->tvs, _temp4089,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ud->fields))->v);}
_LL4034: _temp4095= _temp4093; goto _LL4036; _LL4036: return Cyc_Tcutil_fields_support_default(
0, 0, _temp4095); _LL4038: return 1; _LL4040: return 0; _LL4004:;} static int
Cyc_Tcutil_fields_support_default( struct Cyc_List_List* tvs, struct Cyc_List_List*
ts, struct Cyc_List_List* fs){{ struct _RegionHandle _temp4123= _new_region();
struct _RegionHandle* rgn=& _temp4123; _push_region( rgn);{ struct Cyc_List_List*
_temp4124=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, tvs, ts); goto _LL4125; _LL4125: for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ void* t= Cyc_Tcutil_rsubstitute( rgn, _temp4124,( void*)((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( fs))->hd)->type);
if( ! Cyc_Tcutil_supports_default( t)){ int _temp4126= 0; _npop_handler( 0u);
return _temp4126;}}}; _pop_region(& _temp4123);} return 1;}