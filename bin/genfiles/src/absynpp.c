 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern struct _tagged_string Cyc_Core_new_string( int); extern
unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern struct _tagged_string
Cyc_Core_string_of_int( int); extern unsigned char* string_to_Cstring( struct
_tagged_string); extern unsigned char* underlying_Cstring( struct _tagged_string);
extern struct _tagged_string Cstring_to_string( unsigned char*); struct
_tagged_ptr0{ struct _tagged_string* curr; struct _tagged_string* base; struct
_tagged_string* last_plus_one; } ; extern struct _tagged_ptr0 ntCsl_to_ntsl(
unsigned char**); extern int system( unsigned char*); extern int* __errno();
struct Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[ 19u];
extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_string f1; } ; struct Cyc_List_List{ void* hd;
struct Cyc_List_List* tl; } ; extern unsigned char Cyc_List_List_empty[ 15u];
extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_map_c( void*(* f)( void*, void*), void*
env, struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[ 18u];
extern void Cyc_List_iter( void(* f)( void*), struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List* y);
extern struct Cyc_List_List* Cyc_List_imp_append( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_List_exists(
int(* pred)( void*), struct Cyc_List_List* x); extern int Cyc_List_list_cmp( int(*
cmp)( void*, void*), struct Cyc_List_List* l1, struct Cyc_List_List* l2); extern
int Cyc_List_list_prefix( int(* cmp)( void*, void*), struct Cyc_List_List* l1,
struct Cyc_List_List* l2); struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc;
extern void Cyc_PP_file_of_doc( struct Cyc_PP_Doc* d, int w, struct Cyc_Stdio___sFILE*
f); extern struct _tagged_string Cyc_PP_string_of_doc( struct Cyc_PP_Doc* d, int
w); extern struct Cyc_PP_Doc* Cyc_PP_nil_doc(); extern struct Cyc_PP_Doc* Cyc_PP_line_doc();
extern struct Cyc_PP_Doc* Cyc_PP_text( struct _tagged_string s); extern struct
Cyc_PP_Doc* Cyc_PP_textptr( struct _tagged_string* s); extern struct Cyc_PP_Doc*
Cyc_PP_nest( int k, struct Cyc_PP_Doc* d); struct _tagged_ptr1{ struct Cyc_PP_Doc**
curr; struct Cyc_PP_Doc** base; struct Cyc_PP_Doc** last_plus_one; } ; extern
struct Cyc_PP_Doc* Cyc_PP_cat( struct _tagged_ptr1); extern struct Cyc_PP_Doc*
Cyc_PP_ppseq( struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_string sep,
struct Cyc_List_List* l0); extern struct Cyc_PP_Doc* Cyc_PP_ppseql( struct Cyc_PP_Doc*(*
pp)( void*), struct _tagged_string sep, struct Cyc_List_List* l0); extern struct
Cyc_PP_Doc* Cyc_PP_seql( struct _tagged_string sep, struct Cyc_List_List* l0);
extern struct Cyc_PP_Doc* Cyc_PP_group( struct _tagged_string start, struct
_tagged_string stop, struct _tagged_string sep, struct Cyc_List_List* ss);
extern struct Cyc_PP_Doc* Cyc_PP_egroup( struct _tagged_string start, struct
_tagged_string stop, struct _tagged_string sep, struct Cyc_List_List* ss);
struct Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct _tagged_string
line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[ 9u];
struct Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static const
int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{
struct _tagged_string source; struct Cyc_Position_Segment* seg; void* kind;
struct _tagged_string desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u];
struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct Cyc_Absyn_Tvar;
struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref; struct Cyc_Absyn_PtrInfo;
struct Cyc_Absyn_VarargInfo; struct Cyc_Absyn_FnInfo; struct Cyc_Absyn_TunionInfo;
struct Cyc_Absyn_TunionFieldInfo; struct Cyc_Absyn_VarargCallInfo; struct Cyc_Absyn_Exp;
struct Cyc_Absyn_Stmt; struct Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause;
struct Cyc_Absyn_SwitchC_clause; struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl;
struct Cyc_Absyn_Uniondecl; struct Cyc_Absyn_Tuniondecl; struct Cyc_Absyn_Tunionfield;
struct Cyc_Absyn_Enumfield; struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Typedefdecl;
struct Cyc_Absyn_Vardecl; struct Cyc_Absyn_Decl; struct Cyc_Absyn_Structfield;
static const int Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0; struct
Cyc_Absyn_Rel_n_struct{ int tag; struct Cyc_List_List* f1; } ; static const int
Cyc_Absyn_Abs_n= 1; struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_Static= 0; static const int Cyc_Absyn_Abstract=
1; static const int Cyc_Absyn_Public= 2; static const int Cyc_Absyn_Extern= 3;
static const int Cyc_Absyn_ExternC= 4; struct Cyc_Absyn_Tqual{ int q_const: 1;
int q_volatile: 1; int q_restrict: 1; } ; static const int Cyc_Absyn_B1= 0;
static const int Cyc_Absyn_B2= 1; static const int Cyc_Absyn_B4= 2; static const
int Cyc_Absyn_B8= 3; static const int Cyc_Absyn_AnyKind= 0; static const int Cyc_Absyn_MemKind=
1; static const int Cyc_Absyn_BoxKind= 2; static const int Cyc_Absyn_RgnKind= 3;
static const int Cyc_Absyn_EffKind= 4; static const int Cyc_Absyn_Signed= 0;
static const int Cyc_Absyn_Unsigned= 1; struct Cyc_Absyn_Conref{ void* v; } ;
static const int Cyc_Absyn_Eq_constr= 0; struct Cyc_Absyn_Eq_constr_struct{ int
tag; void* f1; } ; static const int Cyc_Absyn_Forward_constr= 1; struct Cyc_Absyn_Forward_constr_struct{
int tag; struct Cyc_Absyn_Conref* f1; } ; static const int Cyc_Absyn_No_constr=
0; struct Cyc_Absyn_Tvar{ struct _tagged_string* name; int* identity; struct Cyc_Absyn_Conref*
kind; } ; static const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b=
0; struct Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
struct Cyc_Absyn_PtrInfo{ void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref*
nullable; struct Cyc_Absyn_Tqual tq; struct Cyc_Absyn_Conref* bounds; } ; struct
Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt* name; struct Cyc_Absyn_Tqual tq; void*
type; void* rgn; int inject; } ; struct Cyc_Absyn_FnInfo{ struct Cyc_List_List*
tvars; struct Cyc_Core_Opt* effect; void* ret_typ; struct Cyc_List_List* args;
int c_varargs; struct Cyc_Absyn_VarargInfo* cyc_varargs; struct Cyc_List_List*
rgn_po; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{
struct _tuple0* name; int is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion=
0; struct Cyc_Absyn_UnknownTunion_struct{ int tag; struct Cyc_Absyn_UnknownTunionInfo
f1; } ; static const int Cyc_Absyn_KnownTunion= 1; struct Cyc_Absyn_KnownTunion_struct{
int tag; struct Cyc_Absyn_Tuniondecl* f1; } ; struct Cyc_Absyn_TunionInfo{ void*
tunion_info; struct Cyc_List_List* targs; void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{
struct _tuple0* tunion_name; struct _tuple0* field_name; int is_xtunion; } ;
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
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; static const int Cyc_Absyn_UnionType= 10; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; static const int Cyc_Absyn_AnonStructType= 11; struct Cyc_Absyn_AnonStructType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType=
12; struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_EnumType= 13; struct Cyc_Absyn_EnumType_struct{ int
tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int Cyc_Absyn_RgnHandleType=
14; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 15; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
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
1; struct Cyc_Absyn_Var_e_struct{ int tag; struct _tuple0* f1; void* f2; } ;
static const int Cyc_Absyn_UnknownId_e= 2; struct Cyc_Absyn_UnknownId_e_struct{
int tag; struct _tuple0* f1; } ; static const int Cyc_Absyn_Primop_e= 3; struct
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
24; struct _tuple1{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple1* f1; struct
Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e= 25; struct Cyc_Absyn_Array_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Comprehension_e=
26; struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Struct_e=
27; struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple0* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4; } ; static const
int Cyc_Absyn_AnonStruct_e= 28; struct Cyc_Absyn_AnonStruct_e_struct{ int tag;
void* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Tunion_e= 29;
struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_e= 30; struct Cyc_Absyn_Enum_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
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
f3; } ; static const int Cyc_Absyn_While_s= 4; struct _tuple2{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple2 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_For_s= 8; struct Cyc_Absyn_For_s_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct _tuple2 f2; struct _tuple2 f3; struct Cyc_Absyn_Stmt*
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
_tuple2 f2; } ; static const int Cyc_Absyn_TryCatch_s= 17; struct Cyc_Absyn_TryCatch_s_struct{
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
struct Cyc_Absyn_UnknownId_p_struct{ int tag; struct _tuple0* f1; } ; static
const int Cyc_Absyn_UnknownCall_p= 11; struct Cyc_Absyn_UnknownCall_p_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; static const int Cyc_Absyn_UnknownFields_p= 12; struct Cyc_Absyn_UnknownFields_p_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
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
void* sc; struct _tuple0* name; struct Cyc_Absyn_Tqual tq; void* type; struct
Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int is_inline; struct _tuple0*
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
attributes; } ; struct Cyc_Absyn_Tunionfield{ struct _tuple0* name; struct Cyc_List_List*
tvs; struct Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ;
struct Cyc_Absyn_Tuniondecl{ void* sc; struct _tuple0* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{
struct _tuple0* name; struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc; struct _tuple0* name; struct Cyc_Core_Opt*
fields; } ; struct Cyc_Absyn_Typedefdecl{ struct _tuple0* name; struct Cyc_List_List*
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
_tuple0* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
11; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static
const int Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1;
struct Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ;
extern unsigned char Cyc_Absyn_EmptyAnnot[ 15u]; extern struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref* x); extern int Cyc_Absyn_is_format_prim( void* p);
extern struct _tagged_string Cyc_Absyn_attribute2string( void*); struct Cyc_Buffer_t;
extern int Cyc_String_strptrcmp( struct _tagged_string* s1, struct
_tagged_string* s2); extern struct _tagged_string Cyc_String_str_sepstr( struct
Cyc_List_List*, struct _tagged_string); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[
11u]; struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u]; extern
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
ae; struct Cyc_Core_Opt* le; } ; extern unsigned char Cyc_Tcutil_TypeErr[ 12u];
extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar*);
struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int qvar_to_Cids: 1; int
add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int rewrite_temp_tvars: 1;
int print_all_tvars: 1; int print_all_kinds: 1; int print_using_stmts: 1; int
print_externC_stmts: 1; int print_full_evars: 1; int use_curr_namespace: 1;
struct Cyc_List_List* curr_namespace; } ; extern void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params* fs); extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r; extern struct Cyc_Absynpp_Params
Cyc_Absynpp_c_params_r; extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
extern void Cyc_Absynpp_decllist2file( struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE*
f); extern struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc( struct Cyc_Absyn_Decl* d);
extern struct _tagged_string Cyc_Absynpp_typ2string( void*); extern struct
_tagged_string Cyc_Absynpp_kind2string( void*); extern struct _tagged_string Cyc_Absynpp_ckind2string(
struct Cyc_Absyn_Conref*); extern struct _tagged_string Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*); extern struct _tagged_string Cyc_Absynpp_stmt2string(
struct Cyc_Absyn_Stmt*); extern struct _tagged_string Cyc_Absynpp_qvar2string(
struct _tuple0*); extern struct _tagged_string Cyc_Absynpp_decllist2string(
struct Cyc_List_List* tdl); extern struct _tagged_string Cyc_Absynpp_prim2string(
void* p); extern struct _tagged_string Cyc_Absynpp_pat2string( struct Cyc_Absyn_Pat*
p); extern struct _tagged_string Cyc_Absynpp_scope2string( void* sc); extern
struct _tagged_string Cyc_Absynpp_cyc_string; extern struct _tagged_string* Cyc_Absynpp_cyc_stringptr;
extern int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp*); extern struct
_tagged_string Cyc_Absynpp_char_escape( unsigned char); extern struct
_tagged_string Cyc_Absynpp_string_escape( struct _tagged_string); extern struct
_tagged_string Cyc_Absynpp_prim2str( void* p); extern int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt* s); struct _tuple3{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual
f2; void* f3; } ; extern struct _tuple1* Cyc_Absynpp_arg_mk_opt( struct _tuple3*
arg); struct _tuple4{ struct Cyc_Absyn_Tqual f1; void* f2; struct Cyc_List_List*
f3; } ; extern struct _tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual tq,
void* t); struct _tuple5{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; }
; extern struct Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct _tuple5* dp); extern
struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc( struct Cyc_List_List* cs);
extern struct Cyc_PP_Doc* Cyc_Absynpp_switchCclauses2doc( struct Cyc_List_List*
cs); extern struct Cyc_PP_Doc* Cyc_Absynpp_typ2doc( void*); extern struct Cyc_PP_Doc*
Cyc_Absynpp_structfields2doc( struct Cyc_List_List* fields); extern struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc( void*); extern struct Cyc_PP_Doc* Cyc_Absynpp_stmt2doc(
struct Cyc_Absyn_Stmt*); extern struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc( struct
Cyc_Absyn_Exp*); extern struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec( int inprec,
struct Cyc_Absyn_Exp* e); extern struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec(
int inprec, struct Cyc_List_List* es); extern struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc(
struct _tuple0*); extern struct Cyc_PP_Doc* Cyc_Absynpp_typedef_name2doc( struct
_tuple0*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc( void*); extern struct
Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void*); extern struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc(
int inprec, void* p, struct Cyc_List_List* es); struct _tuple6{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Exp* f2; } ; extern struct Cyc_PP_Doc* Cyc_Absynpp_de2doc(
struct _tuple6* de); extern struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc( struct Cyc_Absyn_Tqual
tq, void* t, struct Cyc_Core_Opt* dopt); extern struct Cyc_PP_Doc* Cyc_Absynpp_funargs2doc(
struct Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo*
cyc_varargs, struct Cyc_Core_Opt* effopt, struct Cyc_List_List* rgn_po); extern
struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc( struct Cyc_List_List* fields);
static int Cyc_Absynpp_expand_typedefs; static int Cyc_Absynpp_qvar_to_Cids;
static unsigned char _temp2[ 4u]="Cyc"; struct _tagged_string Cyc_Absynpp_cyc_string={
_temp2, _temp2, _temp2 + 4u}; struct _tagged_string* Cyc_Absynpp_cyc_stringptr=&
Cyc_Absynpp_cyc_string; static int Cyc_Absynpp_add_cyc_prefix; static int Cyc_Absynpp_to_VC;
static int Cyc_Absynpp_decls_first; static int Cyc_Absynpp_rewrite_temp_tvars;
static int Cyc_Absynpp_print_all_tvars; static int Cyc_Absynpp_print_all_kinds;
static int Cyc_Absynpp_print_using_stmts; static int Cyc_Absynpp_print_externC_stmts;
static int Cyc_Absynpp_print_full_evars; static int Cyc_Absynpp_use_curr_namespace;
static struct Cyc_List_List* Cyc_Absynpp_curr_namespace= 0; struct Cyc_Absynpp_Params;
void Cyc_Absynpp_set_params( struct Cyc_Absynpp_Params* fs){ Cyc_Absynpp_expand_typedefs=
fs->expand_typedefs; Cyc_Absynpp_qvar_to_Cids= fs->qvar_to_Cids; Cyc_Absynpp_add_cyc_prefix=
fs->add_cyc_prefix; Cyc_Absynpp_to_VC= fs->to_VC; Cyc_Absynpp_decls_first= fs->decls_first;
Cyc_Absynpp_rewrite_temp_tvars= fs->rewrite_temp_tvars; Cyc_Absynpp_print_all_tvars=
fs->print_all_tvars; Cyc_Absynpp_print_all_kinds= fs->print_all_kinds; Cyc_Absynpp_print_using_stmts=
fs->print_using_stmts; Cyc_Absynpp_print_externC_stmts= fs->print_externC_stmts;
Cyc_Absynpp_print_full_evars= fs->print_full_evars; Cyc_Absynpp_use_curr_namespace=
fs->use_curr_namespace; Cyc_Absynpp_curr_namespace= fs->curr_namespace;} struct
Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={ 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1,
0}; struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={ 1, 0, 0, 0, 0, 1, 0, 0,
1, 1, 0, 1, 0}; struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={ 1, 1, 1, 0, 1,
0, 0, 0, 0, 0, 0, 0, 0}; struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={ 0,
0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0}; static void Cyc_Absynpp_curr_namespace_add(
struct _tagged_string* v){ Cyc_Absynpp_curr_namespace=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_Absynpp_curr_namespace,({
struct Cyc_List_List* _temp3=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp3->hd=( void*) v; _temp3->tl= 0; _temp3;}));} static void
Cyc_Absynpp_suppr_last( struct Cyc_List_List** l){ if((( struct Cyc_List_List*)
_check_null(* l))->tl == 0){* l= 0;} else{(( void(*)( struct Cyc_List_List** l))
Cyc_Absynpp_suppr_last)(&(( struct Cyc_List_List*) _check_null(* l))->tl);}}
static void Cyc_Absynpp_curr_namespace_drop(){(( void(*)( struct Cyc_List_List**
l)) Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);} struct
_tagged_string Cyc_Absynpp_char_escape( unsigned char c){ switch( c){ case '\a':
_LL4: return({ unsigned char* _temp6=( unsigned char*)"\\a"; struct
_tagged_string _temp7; _temp7.curr= _temp6; _temp7.base= _temp6; _temp7.last_plus_one=
_temp6 + 3; _temp7;}); case '\b': _LL5: return({ unsigned char* _temp9=(
unsigned char*)"\\b"; struct _tagged_string _temp10; _temp10.curr= _temp9;
_temp10.base= _temp9; _temp10.last_plus_one= _temp9 + 3; _temp10;}); case '\f':
_LL8: return({ unsigned char* _temp12=( unsigned char*)"\\f"; struct
_tagged_string _temp13; _temp13.curr= _temp12; _temp13.base= _temp12; _temp13.last_plus_one=
_temp12 + 3; _temp13;}); case '\n': _LL11: return({ unsigned char* _temp15=(
unsigned char*)"\\n"; struct _tagged_string _temp16; _temp16.curr= _temp15;
_temp16.base= _temp15; _temp16.last_plus_one= _temp15 + 3; _temp16;}); case '\r':
_LL14: return({ unsigned char* _temp18=( unsigned char*)"\\r"; struct
_tagged_string _temp19; _temp19.curr= _temp18; _temp19.base= _temp18; _temp19.last_plus_one=
_temp18 + 3; _temp19;}); case '\t': _LL17: return({ unsigned char* _temp21=(
unsigned char*)"\\t"; struct _tagged_string _temp22; _temp22.curr= _temp21;
_temp22.base= _temp21; _temp22.last_plus_one= _temp21 + 3; _temp22;}); case '\v':
_LL20: return({ unsigned char* _temp24=( unsigned char*)"\\v"; struct
_tagged_string _temp25; _temp25.curr= _temp24; _temp25.base= _temp24; _temp25.last_plus_one=
_temp24 + 3; _temp25;}); case '\\': _LL23: return({ unsigned char* _temp27=(
unsigned char*)"\\\\"; struct _tagged_string _temp28; _temp28.curr= _temp27;
_temp28.base= _temp27; _temp28.last_plus_one= _temp27 + 3; _temp28;}); case '"':
_LL26: return({ unsigned char* _temp30=( unsigned char*)"\""; struct
_tagged_string _temp31; _temp31.curr= _temp30; _temp31.base= _temp30; _temp31.last_plus_one=
_temp30 + 2; _temp31;}); case '\'': _LL29: return({ unsigned char* _temp33=(
unsigned char*)"\\'"; struct _tagged_string _temp34; _temp34.curr= _temp33;
_temp34.base= _temp33; _temp34.last_plus_one= _temp33 + 3; _temp34;}); default:
_LL32: if( c >=' '? c <='~': 0){ struct _tagged_string _temp36= Cyc_Core_new_string(
1); goto _LL37; _LL37:*(( unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( _temp36, sizeof( unsigned char), 0)= c;
return( struct _tagged_string) _temp36;} else{ struct _tagged_string _temp38=
Cyc_Core_new_string( 4); goto _LL39; _LL39: { int j= 0;*(( unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
_temp38, sizeof( unsigned char), j ++)='\\';*(( unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( _temp38,
sizeof( unsigned char), j ++)=( unsigned char)('0' +(( unsigned char) c >> 6 & 7));*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( _temp38, sizeof( unsigned char), j ++)=(
unsigned char)('0' +( c >> 3 & 7));*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( _temp38, sizeof(
unsigned char), j ++)=( unsigned char)('0' +( c & 7)); return( struct
_tagged_string) _temp38;}}}} static int Cyc_Absynpp_special( struct
_tagged_string s){ int sz=( int)(({ struct _tagged_string _temp40= s;(
unsigned int)( _temp40.last_plus_one - _temp40.curr);}) - 1);{ int i= 0; for( 0;
i < sz; i ++){ unsigned char c=*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( unsigned char), i); if((( c <=' '? 1: c >='~')? 1: c =='"')? 1: c =='\\'){
return 1;}}} return 0;} struct _tagged_string Cyc_Absynpp_string_escape( struct
_tagged_string s){ if( ! Cyc_Absynpp_special( s)){ return s;}{ int n=( int)(({
struct _tagged_string _temp85= s;( unsigned int)( _temp85.last_plus_one -
_temp85.curr);}) - 1); if( n > 0?*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( unsigned char), n) =='\000': 0){ n --;}{ int len= 0;{ int i= 0; for( 0;
i <= n; i ++){ unsigned char _temp41=*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( unsigned char), i); _LL43: if( _temp41 =='\a'){ goto _LL44;} else{ goto
_LL45;} _LL45: if( _temp41 =='\b'){ goto _LL46;} else{ goto _LL47;} _LL47: if(
_temp41 =='\f'){ goto _LL48;} else{ goto _LL49;} _LL49: if( _temp41 =='\n'){
goto _LL50;} else{ goto _LL51;} _LL51: if( _temp41 =='\r'){ goto _LL52;} else{
goto _LL53;} _LL53: if( _temp41 =='\t'){ goto _LL54;} else{ goto _LL55;} _LL55:
if( _temp41 =='\v'){ goto _LL56;} else{ goto _LL57;} _LL57: if( _temp41 =='\\'){
goto _LL58;} else{ goto _LL59;} _LL59: if( _temp41 =='"'){ goto _LL60;} else{
goto _LL61;} _LL61: goto _LL62; _LL44: goto _LL46; _LL46: goto _LL48; _LL48:
goto _LL50; _LL50: goto _LL52; _LL52: goto _LL54; _LL54: goto _LL56; _LL56: goto
_LL58; _LL58: goto _LL60; _LL60: len += 2; goto _LL42; _LL62: if( _temp41 >=' '?
_temp41 <='~': 0){ len ++;} else{ len += 4;} goto _LL42; _LL42:;}}{ struct
_tagged_string t= Cyc_Core_new_string( len); int j= 0;{ int i= 0; for( 0; i <= n;
i ++){ unsigned char _temp63=*(( const unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char),
i); _LL65: if( _temp63 =='\a'){ goto _LL66;} else{ goto _LL67;} _LL67: if(
_temp63 =='\b'){ goto _LL68;} else{ goto _LL69;} _LL69: if( _temp63 =='\f'){
goto _LL70;} else{ goto _LL71;} _LL71: if( _temp63 =='\n'){ goto _LL72;} else{
goto _LL73;} _LL73: if( _temp63 =='\r'){ goto _LL74;} else{ goto _LL75;} _LL75:
if( _temp63 =='\t'){ goto _LL76;} else{ goto _LL77;} _LL77: if( _temp63 =='\v'){
goto _LL78;} else{ goto _LL79;} _LL79: if( _temp63 =='\\'){ goto _LL80;} else{
goto _LL81;} _LL81: if( _temp63 =='"'){ goto _LL82;} else{ goto _LL83;} _LL83:
goto _LL84; _LL66:*(( unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( t, sizeof( unsigned char), j ++)='\\';*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='a'; goto _LL64;
_LL68:*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='\\';*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='b'; goto _LL64;
_LL70:*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='\\';*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='f'; goto _LL64;
_LL72:*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='\\';*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='n'; goto _LL64;
_LL74:*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='\\';*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='r'; goto _LL64;
_LL76:*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='\\';*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='t'; goto _LL64;
_LL78:*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='\\';*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='v'; goto _LL64;
_LL80:*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='\\';*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='\\'; goto _LL64;
_LL82:*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='\\';*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='"'; goto _LL64;
_LL84: if( _temp63 >=' '? _temp63 <='~': 0){*(( unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( t,
sizeof( unsigned char), j ++)= _temp63;} else{*(( unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( t,
sizeof( unsigned char), j ++)='\\';*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( t, sizeof( unsigned char),
j ++)=( unsigned char)('0' +( _temp63 >> 6 & 7));*(( unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( t,
sizeof( unsigned char), j ++)=( unsigned char)('0' +( _temp63 >> 3 & 7));*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)=( unsigned char)('0'
+( _temp63 & 7));} goto _LL64; _LL64:;}} return( struct _tagged_string) t;}}}}
static unsigned char _temp88[ 9u]="restrict"; static struct _tagged_string Cyc_Absynpp_restrict_string={
_temp88, _temp88, _temp88 + 9u}; static unsigned char _temp91[ 9u]="volatile";
static struct _tagged_string Cyc_Absynpp_volatile_string={ _temp91, _temp91,
_temp91 + 9u}; static unsigned char _temp94[ 6u]="const"; static struct
_tagged_string Cyc_Absynpp_const_str={ _temp94, _temp94, _temp94 + 6u}; static
struct _tagged_string* Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string;
static struct _tagged_string* Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _tagged_string* Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
struct Cyc_PP_Doc* Cyc_Absynpp_tqual2doc( struct Cyc_Absyn_Tqual tq){ struct Cyc_List_List*
l= 0; if( tq.q_restrict){ l=({ struct Cyc_List_List* _temp95=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp95->hd=( void*) Cyc_Absynpp_restrict_sp;
_temp95->tl= l; _temp95;});} if( tq.q_volatile){ l=({ struct Cyc_List_List*
_temp96=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp96->hd=( void*) Cyc_Absynpp_volatile_sp; _temp96->tl= l; _temp96;});} if(
tq.q_const){ l=({ struct Cyc_List_List* _temp97=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp97->hd=( void*) Cyc_Absynpp_const_sp;
_temp97->tl= l; _temp97;});} return Cyc_PP_egroup(({ unsigned char* _temp98=(
unsigned char*)""; struct _tagged_string _temp99; _temp99.curr= _temp98; _temp99.base=
_temp98; _temp99.last_plus_one= _temp98 + 1; _temp99;}),({ unsigned char*
_temp100=( unsigned char*)" "; struct _tagged_string _temp101; _temp101.curr=
_temp100; _temp101.base= _temp100; _temp101.last_plus_one= _temp100 + 2;
_temp101;}),({ unsigned char* _temp102=( unsigned char*)" "; struct
_tagged_string _temp103; _temp103.curr= _temp102; _temp103.base= _temp102;
_temp103.last_plus_one= _temp102 + 2; _temp103;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_PP_textptr, l));} struct _tagged_string Cyc_Absynpp_kind2string( void* k){
void* _temp104= k; _LL106: if( _temp104 ==( void*) Cyc_Absyn_AnyKind){ goto
_LL107;} else{ goto _LL108;} _LL108: if( _temp104 ==( void*) Cyc_Absyn_MemKind){
goto _LL109;} else{ goto _LL110;} _LL110: if( _temp104 ==( void*) Cyc_Absyn_BoxKind){
goto _LL111;} else{ goto _LL112;} _LL112: if( _temp104 ==( void*) Cyc_Absyn_RgnKind){
goto _LL113;} else{ goto _LL114;} _LL114: if( _temp104 ==( void*) Cyc_Absyn_EffKind){
goto _LL115;} else{ goto _LL105;} _LL107: return({ unsigned char* _temp116=(
unsigned char*)"A"; struct _tagged_string _temp117; _temp117.curr= _temp116;
_temp117.base= _temp116; _temp117.last_plus_one= _temp116 + 2; _temp117;});
_LL109: return({ unsigned char* _temp118=( unsigned char*)"M"; struct
_tagged_string _temp119; _temp119.curr= _temp118; _temp119.base= _temp118;
_temp119.last_plus_one= _temp118 + 2; _temp119;}); _LL111: return({
unsigned char* _temp120=( unsigned char*)"B"; struct _tagged_string _temp121;
_temp121.curr= _temp120; _temp121.base= _temp120; _temp121.last_plus_one=
_temp120 + 2; _temp121;}); _LL113: return({ unsigned char* _temp122=(
unsigned char*)"R"; struct _tagged_string _temp123; _temp123.curr= _temp122;
_temp123.base= _temp122; _temp123.last_plus_one= _temp122 + 2; _temp123;});
_LL115: return({ unsigned char* _temp124=( unsigned char*)"E"; struct
_tagged_string _temp125; _temp125.curr= _temp124; _temp125.base= _temp124;
_temp125.last_plus_one= _temp124 + 2; _temp125;}); _LL105:;} struct
_tagged_string Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref* c){ void*
_temp126=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( c))->v; void* _temp132; _LL128: if(( unsigned int)
_temp126 > 1u?*(( int*) _temp126) == Cyc_Absyn_Eq_constr: 0){ _LL133: _temp132=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp126)->f1; goto _LL129;} else{
goto _LL130;} _LL130: goto _LL131; _LL129: return Cyc_Absynpp_kind2string(
_temp132); _LL131: return({ unsigned char* _temp134=( unsigned char*)"?"; struct
_tagged_string _temp135; _temp135.curr= _temp134; _temp135.base= _temp134;
_temp135.last_plus_one= _temp134 + 2; _temp135;}); _LL127:;} struct Cyc_PP_Doc*
Cyc_Absynpp_kind2doc( void* k){ return Cyc_PP_text( Cyc_Absynpp_kind2string( k));}
struct Cyc_PP_Doc* Cyc_Absynpp_ckind2doc( struct Cyc_Absyn_Conref* c){ return
Cyc_PP_text( Cyc_Absynpp_ckind2string( c));} struct Cyc_PP_Doc* Cyc_Absynpp_tps2doc(
struct Cyc_List_List* ts){ return Cyc_PP_egroup(({ unsigned char* _temp136=(
unsigned char*)"<"; struct _tagged_string _temp137; _temp137.curr= _temp136;
_temp137.base= _temp136; _temp137.last_plus_one= _temp136 + 2; _temp137;}),({
unsigned char* _temp138=( unsigned char*)">"; struct _tagged_string _temp139;
_temp139.curr= _temp138; _temp139.base= _temp138; _temp139.last_plus_one=
_temp138 + 2; _temp139;}),({ unsigned char* _temp140=( unsigned char*)",";
struct _tagged_string _temp141; _temp141.curr= _temp140; _temp141.base= _temp140;
_temp141.last_plus_one= _temp140 + 2; _temp141;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_typ2doc,
ts));} struct Cyc_PP_Doc* Cyc_Absynpp_ktvar2doc( struct Cyc_Absyn_Tvar* tv){
void* _temp142=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( tv->kind))->v; void* _temp150; void* _temp152;
_LL144: if(( unsigned int) _temp142 > 1u?*(( int*) _temp142) == Cyc_Absyn_Eq_constr:
0){ _LL151: _temp150=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp142)->f1;
if( _temp150 ==( void*) Cyc_Absyn_BoxKind){ goto _LL145;} else{ goto _LL146;}}
else{ goto _LL146;} _LL146: if(( unsigned int) _temp142 > 1u?*(( int*) _temp142)
== Cyc_Absyn_Eq_constr: 0){ _LL153: _temp152=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp142)->f1; goto _LL147;} else{ goto _LL148;} _LL148: goto _LL149; _LL145:
return Cyc_PP_textptr( tv->name); _LL147: return({ struct Cyc_PP_Doc*(* _temp154)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp157= Cyc_PP_textptr(
tv->name); struct Cyc_PP_Doc* _temp158= Cyc_PP_text(({ unsigned char* _temp160=(
unsigned char*)"::"; struct _tagged_string _temp161; _temp161.curr= _temp160;
_temp161.base= _temp160; _temp161.last_plus_one= _temp160 + 3; _temp161;}));
struct Cyc_PP_Doc* _temp159= Cyc_Absynpp_kind2doc( _temp152); struct Cyc_PP_Doc*
_temp155[ 3u]={ _temp157, _temp158, _temp159}; struct _tagged_ptr1 _temp156={
_temp155, _temp155, _temp155 + 3u}; _temp154( _temp156);}); _LL149: return({
struct Cyc_PP_Doc*(* _temp162)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp165= Cyc_PP_textptr( tv->name); struct Cyc_PP_Doc* _temp166= Cyc_PP_text(({
unsigned char* _temp167=( unsigned char*)"/*::?*/"; struct _tagged_string
_temp168; _temp168.curr= _temp167; _temp168.base= _temp167; _temp168.last_plus_one=
_temp167 + 8; _temp168;})); struct Cyc_PP_Doc* _temp163[ 2u]={ _temp165,
_temp166}; struct _tagged_ptr1 _temp164={ _temp163, _temp163, _temp163 + 2u};
_temp162( _temp164);}); _LL143:;} struct Cyc_PP_Doc* Cyc_Absynpp_ktvars2doc(
struct Cyc_List_List* tvs){ return Cyc_PP_egroup(({ unsigned char* _temp169=(
unsigned char*)"<"; struct _tagged_string _temp170; _temp170.curr= _temp169;
_temp170.base= _temp169; _temp170.last_plus_one= _temp169 + 2; _temp170;}),({
unsigned char* _temp171=( unsigned char*)">"; struct _tagged_string _temp172;
_temp172.curr= _temp171; _temp172.base= _temp171; _temp172.last_plus_one=
_temp171 + 2; _temp172;}),({ unsigned char* _temp173=( unsigned char*)",";
struct _tagged_string _temp174; _temp174.curr= _temp173; _temp174.base= _temp173;
_temp174.last_plus_one= _temp173 + 2; _temp174;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_ktvar2doc, tvs));} static struct _tagged_string* Cyc_Absynpp_get_name(
struct Cyc_Absyn_Tvar* tv){ return tv->name;} struct Cyc_PP_Doc* Cyc_Absynpp_tvars2doc(
struct Cyc_List_List* tvs){ if( Cyc_Absynpp_print_all_kinds){ return Cyc_Absynpp_ktvars2doc(
tvs);} return Cyc_PP_egroup(({ unsigned char* _temp175=( unsigned char*)"<";
struct _tagged_string _temp176; _temp176.curr= _temp175; _temp176.base= _temp175;
_temp176.last_plus_one= _temp175 + 2; _temp176;}),({ unsigned char* _temp177=(
unsigned char*)">"; struct _tagged_string _temp178; _temp178.curr= _temp177;
_temp178.base= _temp177; _temp178.last_plus_one= _temp177 + 2; _temp178;}),({
unsigned char* _temp179=( unsigned char*)","; struct _tagged_string _temp180;
_temp180.curr= _temp179; _temp180.base= _temp179; _temp180.last_plus_one=
_temp179 + 2; _temp180;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_PP_textptr,((
struct Cyc_List_List*(*)( struct _tagged_string*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_get_name, tvs)));} struct
_tuple7{ struct Cyc_Absyn_Tqual f1; void* f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_arg2doc(
struct _tuple7* t){ return Cyc_Absynpp_tqtd2doc((* t).f1,(* t).f2, 0);} struct
Cyc_PP_Doc* Cyc_Absynpp_args2doc( struct Cyc_List_List* ts){ return Cyc_PP_group(({
unsigned char* _temp181=( unsigned char*)"("; struct _tagged_string _temp182;
_temp182.curr= _temp181; _temp182.base= _temp181; _temp182.last_plus_one=
_temp181 + 2; _temp182;}),({ unsigned char* _temp183=( unsigned char*)")";
struct _tagged_string _temp184; _temp184.curr= _temp183; _temp184.base= _temp183;
_temp184.last_plus_one= _temp183 + 2; _temp184;}),({ unsigned char* _temp185=(
unsigned char*)","; struct _tagged_string _temp186; _temp186.curr= _temp185;
_temp186.base= _temp185; _temp186.last_plus_one= _temp185 + 2; _temp186;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple7*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg2doc, ts));} struct Cyc_PP_Doc* Cyc_Absynpp_noncallatt2doc(
void* att){ void* _temp187= att; _LL189: if( _temp187 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL190;} else{ goto _LL191;} _LL191: if( _temp187 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL192;} else{ goto _LL193;} _LL193: if( _temp187 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL194;} else{ goto _LL195;} _LL195: goto _LL196; _LL190: return Cyc_PP_nil_doc();
_LL192: return Cyc_PP_nil_doc(); _LL194: return Cyc_PP_nil_doc(); _LL196: return
Cyc_PP_text( Cyc_Absyn_attribute2string( att)); _LL188:;} struct Cyc_PP_Doc* Cyc_Absynpp_callconv2doc(
struct Cyc_List_List* atts){ for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp197=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; _LL199: if( _temp197 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL200;} else{ goto _LL201;} _LL201: if( _temp197 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL202;} else{ goto _LL203;} _LL203: if( _temp197 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL204;} else{ goto _LL205;} _LL205: goto _LL206; _LL200: return Cyc_PP_text(({
unsigned char* _temp207=( unsigned char*)"_stdcall"; struct _tagged_string
_temp208; _temp208.curr= _temp207; _temp208.base= _temp207; _temp208.last_plus_one=
_temp207 + 9; _temp208;})); _LL202: return Cyc_PP_text(({ unsigned char*
_temp209=( unsigned char*)"_cdecl"; struct _tagged_string _temp210; _temp210.curr=
_temp209; _temp210.base= _temp209; _temp210.last_plus_one= _temp209 + 7;
_temp210;})); _LL204: return Cyc_PP_text(({ unsigned char* _temp211=(
unsigned char*)"_fastcall"; struct _tagged_string _temp212; _temp212.curr=
_temp211; _temp212.base= _temp211; _temp212.last_plus_one= _temp211 + 10;
_temp212;})); _LL206: goto _LL198; _LL198:;} return Cyc_PP_nil_doc();} struct
Cyc_PP_Doc* Cyc_Absynpp_noncallconv2doc( struct Cyc_List_List* atts){ int hasatt=
0;{ struct Cyc_List_List* atts2= atts; for( 0; atts2 != 0; atts2=(( struct Cyc_List_List*)
_check_null( atts2))->tl){ void* _temp213=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; _LL215: if( _temp213 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL216;} else{ goto _LL217;} _LL217: if( _temp213 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL218;} else{ goto _LL219;} _LL219: if( _temp213 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL220;} else{ goto _LL221;} _LL221: goto _LL222; _LL216: goto _LL214;
_LL218: goto _LL214; _LL220: goto _LL214; _LL222: hasatt= 1; goto _LL214; _LL214:;}}
if( ! hasatt){ return Cyc_PP_nil_doc();} return({ struct Cyc_PP_Doc*(* _temp223)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp226= Cyc_PP_text(({
unsigned char* _temp237=( unsigned char*)"__declspec("; struct _tagged_string
_temp238; _temp238.curr= _temp237; _temp238.base= _temp237; _temp238.last_plus_one=
_temp237 + 12; _temp238;})); struct Cyc_PP_Doc* _temp227= Cyc_PP_group(({
unsigned char* _temp231=( unsigned char*)""; struct _tagged_string _temp232;
_temp232.curr= _temp231; _temp232.base= _temp231; _temp232.last_plus_one=
_temp231 + 1; _temp232;}),({ unsigned char* _temp233=( unsigned char*)""; struct
_tagged_string _temp234; _temp234.curr= _temp233; _temp234.base= _temp233;
_temp234.last_plus_one= _temp233 + 1; _temp234;}),({ unsigned char* _temp235=(
unsigned char*)" "; struct _tagged_string _temp236; _temp236.curr= _temp235;
_temp236.base= _temp235; _temp236.last_plus_one= _temp235 + 2; _temp236;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_noncallatt2doc, atts)); struct Cyc_PP_Doc*
_temp228= Cyc_PP_text(({ unsigned char* _temp229=( unsigned char*)")"; struct
_tagged_string _temp230; _temp230.curr= _temp229; _temp230.base= _temp229;
_temp230.last_plus_one= _temp229 + 2; _temp230;})); struct Cyc_PP_Doc* _temp224[
3u]={ _temp226, _temp227, _temp228}; struct _tagged_ptr1 _temp225={ _temp224,
_temp224, _temp224 + 3u}; _temp223( _temp225);});} struct Cyc_PP_Doc* Cyc_Absynpp_att2doc(
void* a){ return Cyc_PP_text( Cyc_Absyn_attribute2string( a));} struct Cyc_PP_Doc*
Cyc_Absynpp_atts2doc( struct Cyc_List_List* atts){ if( atts == 0){ return Cyc_PP_nil_doc();}
if( Cyc_Absynpp_to_VC){ return Cyc_Absynpp_noncallconv2doc( atts);} return({
struct Cyc_PP_Doc*(* _temp239)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp242= Cyc_PP_text(({ unsigned char* _temp250=( unsigned char*)" __attribute__";
struct _tagged_string _temp251; _temp251.curr= _temp250; _temp251.base= _temp250;
_temp251.last_plus_one= _temp250 + 15; _temp251;})); struct Cyc_PP_Doc* _temp243=
Cyc_PP_group(({ unsigned char* _temp244=( unsigned char*)"(("; struct
_tagged_string _temp245; _temp245.curr= _temp244; _temp245.base= _temp244;
_temp245.last_plus_one= _temp244 + 3; _temp245;}),({ unsigned char* _temp246=(
unsigned char*)"))"; struct _tagged_string _temp247; _temp247.curr= _temp246;
_temp247.base= _temp246; _temp247.last_plus_one= _temp246 + 3; _temp247;}),({
unsigned char* _temp248=( unsigned char*)","; struct _tagged_string _temp249;
_temp249.curr= _temp248; _temp249.base= _temp248; _temp249.last_plus_one=
_temp248 + 2; _temp249;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_att2doc, atts));
struct Cyc_PP_Doc* _temp240[ 2u]={ _temp242, _temp243}; struct _tagged_ptr1
_temp241={ _temp240, _temp240, _temp240 + 2u}; _temp239( _temp241);});} int Cyc_Absynpp_next_is_pointer(
struct Cyc_List_List* tms){ if( tms == 0){ return 0;}{ void* _temp252=( void*)((
struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Tqual _temp258;
void* _temp260; void* _temp262; _LL254: if(( unsigned int) _temp252 > 1u?*(( int*)
_temp252) == Cyc_Absyn_Pointer_mod: 0){ _LL263: _temp262=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp252)->f1; goto _LL261; _LL261: _temp260=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp252)->f2; goto _LL259; _LL259: _temp258=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp252)->f3; goto _LL255;} else{ goto _LL256;} _LL256: goto _LL257; _LL255:
return 1; _LL257: return 0; _LL253:;}} extern struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc(
void* t); struct Cyc_PP_Doc* Cyc_Absynpp_dtms2doc( struct Cyc_PP_Doc* d, struct
Cyc_List_List* tms){ if( tms == 0){ return d;}{ struct Cyc_PP_Doc* rest= Cyc_Absynpp_dtms2doc(
d,(( struct Cyc_List_List*) _check_null( tms))->tl); struct Cyc_PP_Doc* p_rest=({
struct Cyc_PP_Doc*(* _temp431)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp434= Cyc_PP_text(({ unsigned char* _temp439=( unsigned char*)"("; struct
_tagged_string _temp440; _temp440.curr= _temp439; _temp440.base= _temp439;
_temp440.last_plus_one= _temp439 + 2; _temp440;})); struct Cyc_PP_Doc* _temp435=
rest; struct Cyc_PP_Doc* _temp436= Cyc_PP_text(({ unsigned char* _temp437=(
unsigned char*)")"; struct _tagged_string _temp438; _temp438.curr= _temp437;
_temp438.base= _temp437; _temp438.last_plus_one= _temp437 + 2; _temp438;}));
struct Cyc_PP_Doc* _temp432[ 3u]={ _temp434, _temp435, _temp436}; struct
_tagged_ptr1 _temp433={ _temp432, _temp432, _temp432 + 3u}; _temp431( _temp433);});
void* _temp264=( void*)(( struct Cyc_List_List*) _check_null( tms))->hd; struct
Cyc_Absyn_Exp* _temp278; void* _temp280; struct Cyc_List_List* _temp282; struct
Cyc_Position_Segment* _temp284; int _temp286; struct Cyc_Position_Segment*
_temp288; struct Cyc_List_List* _temp290; struct Cyc_Absyn_Tqual _temp292; void*
_temp294; void* _temp296; _LL266: if( _temp264 ==( void*) Cyc_Absyn_Carray_mod){
goto _LL267;} else{ goto _LL268;} _LL268: if(( unsigned int) _temp264 > 1u?*((
int*) _temp264) == Cyc_Absyn_ConstArray_mod: 0){ _LL279: _temp278=(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp264)->f1; goto _LL269;} else{ goto _LL270;} _LL270: if(( unsigned int)
_temp264 > 1u?*(( int*) _temp264) == Cyc_Absyn_Function_mod: 0){ _LL281:
_temp280=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp264)->f1; goto
_LL271;} else{ goto _LL272;} _LL272: if(( unsigned int) _temp264 > 1u?*(( int*)
_temp264) == Cyc_Absyn_Attributes_mod: 0){ _LL285: _temp284=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp264)->f1; goto _LL283; _LL283: _temp282=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp264)->f2; goto _LL273;} else{ goto _LL274;} _LL274: if(( unsigned int)
_temp264 > 1u?*(( int*) _temp264) == Cyc_Absyn_TypeParams_mod: 0){ _LL291:
_temp290=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp264)->f1; goto _LL289;
_LL289: _temp288=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp264)->f2; goto
_LL287; _LL287: _temp286=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp264)->f3;
goto _LL275;} else{ goto _LL276;} _LL276: if(( unsigned int) _temp264 > 1u?*((
int*) _temp264) == Cyc_Absyn_Pointer_mod: 0){ _LL297: _temp296=( void*)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp264)->f1; goto _LL295; _LL295: _temp294=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp264)->f2; goto _LL293;
_LL293: _temp292=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp264)->f3; goto
_LL277;} else{ goto _LL265;} _LL267: if( Cyc_Absynpp_next_is_pointer((( struct
Cyc_List_List*) _check_null( tms))->tl)){ rest= p_rest;} return({ struct Cyc_PP_Doc*(*
_temp298)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp301= rest;
struct Cyc_PP_Doc* _temp302= Cyc_PP_text(({ unsigned char* _temp303=(
unsigned char*)"[]"; struct _tagged_string _temp304; _temp304.curr= _temp303;
_temp304.base= _temp303; _temp304.last_plus_one= _temp303 + 3; _temp304;}));
struct Cyc_PP_Doc* _temp299[ 2u]={ _temp301, _temp302}; struct _tagged_ptr1
_temp300={ _temp299, _temp299, _temp299 + 2u}; _temp298( _temp300);}); _LL269:
if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*) _check_null( tms))->tl)){
rest= p_rest;} return({ struct Cyc_PP_Doc*(* _temp305)( struct _tagged_ptr1)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp308= rest; struct Cyc_PP_Doc* _temp309= Cyc_PP_text(({
unsigned char* _temp314=( unsigned char*)"["; struct _tagged_string _temp315;
_temp315.curr= _temp314; _temp315.base= _temp314; _temp315.last_plus_one=
_temp314 + 2; _temp315;})); struct Cyc_PP_Doc* _temp310= Cyc_Absynpp_exp2doc(
_temp278); struct Cyc_PP_Doc* _temp311= Cyc_PP_text(({ unsigned char* _temp312=(
unsigned char*)"]"; struct _tagged_string _temp313; _temp313.curr= _temp312;
_temp313.base= _temp312; _temp313.last_plus_one= _temp312 + 2; _temp313;}));
struct Cyc_PP_Doc* _temp306[ 4u]={ _temp308, _temp309, _temp310, _temp311};
struct _tagged_ptr1 _temp307={ _temp306, _temp306, _temp306 + 4u}; _temp305(
_temp307);}); _LL271: if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*)
_check_null( tms))->tl)){ rest= p_rest;}{ void* _temp316= _temp280; struct Cyc_List_List*
_temp322; struct Cyc_Core_Opt* _temp324; struct Cyc_Absyn_VarargInfo* _temp326;
int _temp328; struct Cyc_List_List* _temp330; struct Cyc_Position_Segment*
_temp332; struct Cyc_List_List* _temp334; _LL318: if(*(( int*) _temp316) == Cyc_Absyn_WithTypes){
_LL331: _temp330=(( struct Cyc_Absyn_WithTypes_struct*) _temp316)->f1; goto
_LL329; _LL329: _temp328=(( struct Cyc_Absyn_WithTypes_struct*) _temp316)->f2;
goto _LL327; _LL327: _temp326=(( struct Cyc_Absyn_WithTypes_struct*) _temp316)->f3;
goto _LL325; _LL325: _temp324=(( struct Cyc_Absyn_WithTypes_struct*) _temp316)->f4;
goto _LL323; _LL323: _temp322=(( struct Cyc_Absyn_WithTypes_struct*) _temp316)->f5;
goto _LL319;} else{ goto _LL320;} _LL320: if(*(( int*) _temp316) == Cyc_Absyn_NoTypes){
_LL335: _temp334=(( struct Cyc_Absyn_NoTypes_struct*) _temp316)->f1; goto _LL333;
_LL333: _temp332=(( struct Cyc_Absyn_NoTypes_struct*) _temp316)->f2; goto _LL321;}
else{ goto _LL317;} _LL319: return({ struct Cyc_PP_Doc*(* _temp336)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp339= rest; struct Cyc_PP_Doc*
_temp340= Cyc_Absynpp_funargs2doc( _temp330, _temp328, _temp326, _temp324,
_temp322); struct Cyc_PP_Doc* _temp337[ 2u]={ _temp339, _temp340}; struct
_tagged_ptr1 _temp338={ _temp337, _temp337, _temp337 + 2u}; _temp336( _temp338);});
_LL321: return({ struct Cyc_PP_Doc*(* _temp341)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp344= rest; struct Cyc_PP_Doc* _temp345= Cyc_PP_group(({
unsigned char* _temp346=( unsigned char*)"("; struct _tagged_string _temp347;
_temp347.curr= _temp346; _temp347.base= _temp346; _temp347.last_plus_one=
_temp346 + 2; _temp347;}),({ unsigned char* _temp348=( unsigned char*)")";
struct _tagged_string _temp349; _temp349.curr= _temp348; _temp349.base= _temp348;
_temp349.last_plus_one= _temp348 + 2; _temp349;}),({ unsigned char* _temp350=(
unsigned char*)","; struct _tagged_string _temp351; _temp351.curr= _temp350;
_temp351.base= _temp350; _temp351.last_plus_one= _temp350 + 2; _temp351;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tagged_string*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_PP_textptr, _temp334)); struct Cyc_PP_Doc*
_temp342[ 2u]={ _temp344, _temp345}; struct _tagged_ptr1 _temp343={ _temp342,
_temp342, _temp342 + 2u}; _temp341( _temp343);}); _LL317:;} _LL273: if( Cyc_Absynpp_next_is_pointer(((
struct Cyc_List_List*) _check_null( tms))->tl)){ rest= p_rest;} return({ struct
Cyc_PP_Doc*(* _temp352)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp355= rest; struct Cyc_PP_Doc* _temp356= Cyc_Absynpp_atts2doc( _temp282);
struct Cyc_PP_Doc* _temp353[ 2u]={ _temp355, _temp356}; struct _tagged_ptr1
_temp354={ _temp353, _temp353, _temp353 + 2u}; _temp352( _temp354);}); _LL275:
if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*) _check_null( tms))->tl)){
rest= p_rest;} if( _temp286){ return({ struct Cyc_PP_Doc*(* _temp357)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp360= rest; struct Cyc_PP_Doc*
_temp361= Cyc_Absynpp_ktvars2doc( _temp290); struct Cyc_PP_Doc* _temp358[ 2u]={
_temp360, _temp361}; struct _tagged_ptr1 _temp359={ _temp358, _temp358, _temp358
+ 2u}; _temp357( _temp359);});} else{ return({ struct Cyc_PP_Doc*(* _temp362)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp365= rest; struct Cyc_PP_Doc*
_temp366= Cyc_Absynpp_tvars2doc( _temp290); struct Cyc_PP_Doc* _temp363[ 2u]={
_temp365, _temp366}; struct _tagged_ptr1 _temp364={ _temp363, _temp363, _temp363
+ 2u}; _temp362( _temp364);});} _LL277: { struct Cyc_PP_Doc* ptr;{ void*
_temp367= _temp296; struct Cyc_Absyn_Exp* _temp375; struct Cyc_Absyn_Exp*
_temp377; _LL369: if(( unsigned int) _temp367 > 1u?*(( int*) _temp367) == Cyc_Absyn_Nullable_ps:
0){ _LL376: _temp375=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp367)->f1;
goto _LL370;} else{ goto _LL371;} _LL371: if(( unsigned int) _temp367 > 1u?*((
int*) _temp367) == Cyc_Absyn_NonNullable_ps: 0){ _LL378: _temp377=(( struct Cyc_Absyn_NonNullable_ps_struct*)
_temp367)->f1; goto _LL372;} else{ goto _LL373;} _LL373: if( _temp367 ==( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL374;} else{ goto _LL368;} _LL370: if( Cyc_Evexp_eval_const_uint_exp(
_temp375) == 1){ ptr= Cyc_PP_text(({ unsigned char* _temp379=( unsigned char*)"*";
struct _tagged_string _temp380; _temp380.curr= _temp379; _temp380.base= _temp379;
_temp380.last_plus_one= _temp379 + 2; _temp380;}));} else{ ptr=({ struct Cyc_PP_Doc*(*
_temp381)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp384= Cyc_PP_text(({
unsigned char* _temp392=( unsigned char*)"*"; struct _tagged_string _temp393;
_temp393.curr= _temp392; _temp393.base= _temp392; _temp393.last_plus_one=
_temp392 + 2; _temp393;})); struct Cyc_PP_Doc* _temp385= Cyc_PP_text(({
unsigned char* _temp390=( unsigned char*)"{"; struct _tagged_string _temp391;
_temp391.curr= _temp390; _temp391.base= _temp390; _temp391.last_plus_one=
_temp390 + 2; _temp391;})); struct Cyc_PP_Doc* _temp386= Cyc_Absynpp_exp2doc(
_temp375); struct Cyc_PP_Doc* _temp387= Cyc_PP_text(({ unsigned char* _temp388=(
unsigned char*)"}"; struct _tagged_string _temp389; _temp389.curr= _temp388;
_temp389.base= _temp388; _temp389.last_plus_one= _temp388 + 2; _temp389;}));
struct Cyc_PP_Doc* _temp382[ 4u]={ _temp384, _temp385, _temp386, _temp387};
struct _tagged_ptr1 _temp383={ _temp382, _temp382, _temp382 + 4u}; _temp381(
_temp383);});} goto _LL368; _LL372: if( Cyc_Evexp_eval_const_uint_exp( _temp377)
== 1){ ptr= Cyc_PP_text(({ unsigned char* _temp394=( unsigned char*)"@"; struct
_tagged_string _temp395; _temp395.curr= _temp394; _temp395.base= _temp394;
_temp395.last_plus_one= _temp394 + 2; _temp395;}));} else{ ptr=({ struct Cyc_PP_Doc*(*
_temp396)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp399= Cyc_PP_text(({
unsigned char* _temp407=( unsigned char*)"@"; struct _tagged_string _temp408;
_temp408.curr= _temp407; _temp408.base= _temp407; _temp408.last_plus_one=
_temp407 + 2; _temp408;})); struct Cyc_PP_Doc* _temp400= Cyc_PP_text(({
unsigned char* _temp405=( unsigned char*)"{"; struct _tagged_string _temp406;
_temp406.curr= _temp405; _temp406.base= _temp405; _temp406.last_plus_one=
_temp405 + 2; _temp406;})); struct Cyc_PP_Doc* _temp401= Cyc_Absynpp_exp2doc(
_temp377); struct Cyc_PP_Doc* _temp402= Cyc_PP_text(({ unsigned char* _temp403=(
unsigned char*)"}"; struct _tagged_string _temp404; _temp404.curr= _temp403;
_temp404.base= _temp403; _temp404.last_plus_one= _temp403 + 2; _temp404;}));
struct Cyc_PP_Doc* _temp397[ 4u]={ _temp399, _temp400, _temp401, _temp402};
struct _tagged_ptr1 _temp398={ _temp397, _temp397, _temp397 + 4u}; _temp396(
_temp398);});} goto _LL368; _LL374: ptr= Cyc_PP_text(({ unsigned char* _temp409=(
unsigned char*)"?"; struct _tagged_string _temp410; _temp410.curr= _temp409;
_temp410.base= _temp409; _temp410.last_plus_one= _temp409 + 2; _temp410;}));
goto _LL368; _LL368:;}{ void* _temp411= Cyc_Tcutil_compress( _temp294); _LL413:
if( _temp411 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL414;} else{ goto _LL415;}
_LL415: goto _LL416; _LL414: return({ struct Cyc_PP_Doc*(* _temp417)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp420= ptr; struct Cyc_PP_Doc*
_temp421= rest; struct Cyc_PP_Doc* _temp418[ 2u]={ _temp420, _temp421}; struct
_tagged_ptr1 _temp419={ _temp418, _temp418, _temp418 + 2u}; _temp417( _temp419);});
_LL416: return({ struct Cyc_PP_Doc*(* _temp422)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp425= ptr; struct Cyc_PP_Doc* _temp426= Cyc_Absynpp_ntyp2doc(
_temp294); struct Cyc_PP_Doc* _temp427= Cyc_PP_text(({ unsigned char* _temp429=(
unsigned char*)" "; struct _tagged_string _temp430; _temp430.curr= _temp429;
_temp430.base= _temp429; _temp430.last_plus_one= _temp429 + 2; _temp430;}));
struct Cyc_PP_Doc* _temp428= rest; struct Cyc_PP_Doc* _temp423[ 4u]={ _temp425,
_temp426, _temp427, _temp428}; struct _tagged_ptr1 _temp424={ _temp423, _temp423,
_temp423 + 4u}; _temp422( _temp424);}); _LL412:;}} _LL265:;}} struct Cyc_PP_Doc*
Cyc_Absynpp_rgn2doc( void* t){ void* _temp441= Cyc_Tcutil_compress( t); _LL443:
if( _temp441 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL444;} else{ goto _LL445;}
_LL445: goto _LL446; _LL444: return Cyc_PP_text(({ unsigned char* _temp447=(
unsigned char*)"`H"; struct _tagged_string _temp448; _temp448.curr= _temp447;
_temp448.base= _temp447; _temp448.last_plus_one= _temp447 + 3; _temp448;}));
_LL446: return Cyc_Absynpp_ntyp2doc( t); _LL442:;} static void Cyc_Absynpp_effects2docs(
struct Cyc_List_List** regions, struct Cyc_List_List** effects, void* t){ void*
_temp449= Cyc_Tcutil_compress( t); void* _temp457; struct Cyc_List_List*
_temp459; _LL451: if(( unsigned int) _temp449 > 4u?*(( int*) _temp449) == Cyc_Absyn_AccessEff:
0){ _LL458: _temp457=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp449)->f1;
goto _LL452;} else{ goto _LL453;} _LL453: if(( unsigned int) _temp449 > 4u?*((
int*) _temp449) == Cyc_Absyn_JoinEff: 0){ _LL460: _temp459=(( struct Cyc_Absyn_JoinEff_struct*)
_temp449)->f1; goto _LL454;} else{ goto _LL455;} _LL455: goto _LL456; _LL452:*
regions=({ struct Cyc_List_List* _temp461=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp461->hd=( void*) Cyc_Absynpp_rgn2doc(
_temp457); _temp461->tl=* regions; _temp461;}); goto _LL450; _LL454: for( 0;
_temp459 != 0; _temp459=(( struct Cyc_List_List*) _check_null( _temp459))->tl){
Cyc_Absynpp_effects2docs( regions, effects,( void*)(( struct Cyc_List_List*)
_check_null( _temp459))->hd);} goto _LL450; _LL456:* effects=({ struct Cyc_List_List*
_temp462=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp462->hd=( void*) Cyc_Absynpp_ntyp2doc( t); _temp462->tl=* effects; _temp462;});
goto _LL450; _LL450:;} struct Cyc_PP_Doc* Cyc_Absynpp_eff2doc( void* t){ struct
Cyc_List_List* regions= 0; struct Cyc_List_List* effects= 0; Cyc_Absynpp_effects2docs(&
regions,& effects, t); regions=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( regions); effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( effects); if( regions == 0? effects != 0: 0){ return Cyc_PP_group(({
unsigned char* _temp463=( unsigned char*)""; struct _tagged_string _temp464;
_temp464.curr= _temp463; _temp464.base= _temp463; _temp464.last_plus_one=
_temp463 + 1; _temp464;}),({ unsigned char* _temp465=( unsigned char*)""; struct
_tagged_string _temp466; _temp466.curr= _temp465; _temp466.base= _temp465;
_temp466.last_plus_one= _temp465 + 1; _temp466;}),({ unsigned char* _temp467=(
unsigned char*)"+"; struct _tagged_string _temp468; _temp468.curr= _temp467;
_temp468.base= _temp467; _temp468.last_plus_one= _temp467 + 2; _temp468;}),
effects);} else{ struct Cyc_PP_Doc* _temp475= Cyc_PP_group(({ unsigned char*
_temp469=( unsigned char*)"{"; struct _tagged_string _temp470; _temp470.curr=
_temp469; _temp470.base= _temp469; _temp470.last_plus_one= _temp469 + 2;
_temp470;}),({ unsigned char* _temp471=( unsigned char*)"}"; struct
_tagged_string _temp472; _temp472.curr= _temp471; _temp472.base= _temp471;
_temp472.last_plus_one= _temp471 + 2; _temp472;}),({ unsigned char* _temp473=(
unsigned char*)","; struct _tagged_string _temp474; _temp474.curr= _temp473;
_temp474.base= _temp473; _temp474.last_plus_one= _temp473 + 2; _temp474;}),
regions); goto _LL476; _LL476: return Cyc_PP_group(({ unsigned char* _temp477=(
unsigned char*)""; struct _tagged_string _temp478; _temp478.curr= _temp477;
_temp478.base= _temp477; _temp478.last_plus_one= _temp477 + 1; _temp478;}),({
unsigned char* _temp479=( unsigned char*)""; struct _tagged_string _temp480;
_temp480.curr= _temp479; _temp480.base= _temp479; _temp480.last_plus_one=
_temp479 + 1; _temp480;}),({ unsigned char* _temp481=( unsigned char*)"+";
struct _tagged_string _temp482; _temp482.curr= _temp481; _temp482.base= _temp481;
_temp482.last_plus_one= _temp481 + 2; _temp482;}),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( effects,({
struct Cyc_List_List* _temp483=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp483->hd=( void*) _temp475; _temp483->tl= 0;
_temp483;})));}} struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc( void* t){ struct Cyc_PP_Doc*
s;{ void* _temp484= t; struct Cyc_Absyn_Exp* _temp530; struct Cyc_Absyn_Tqual
_temp532; void* _temp534; struct Cyc_Absyn_FnInfo _temp536; struct Cyc_Absyn_PtrInfo
_temp538; struct Cyc_Core_Opt* _temp540; int _temp542; struct Cyc_Core_Opt*
_temp544; struct Cyc_Core_Opt* _temp546; struct Cyc_Absyn_Tvar* _temp548; struct
Cyc_Absyn_TunionInfo _temp550; void* _temp552; struct Cyc_List_List* _temp554;
void* _temp556; struct Cyc_Absyn_TunionFieldInfo _temp558; struct Cyc_List_List*
_temp560; void* _temp562; void* _temp564; void* _temp566; struct Cyc_List_List*
_temp568; struct Cyc_Absyn_Structdecl** _temp570; struct Cyc_List_List* _temp572;
struct _tuple0* _temp574; struct Cyc_Absyn_Uniondecl** _temp576; struct Cyc_List_List*
_temp578; struct _tuple0* _temp580; struct Cyc_List_List* _temp582; struct Cyc_List_List*
_temp584; struct Cyc_Absyn_Enumdecl* _temp586; struct _tuple0* _temp588; struct
Cyc_Core_Opt* _temp590; struct Cyc_List_List* _temp592; struct _tuple0* _temp594;
void* _temp596; void* _temp598; struct Cyc_List_List* _temp600; _LL486: if((
unsigned int) _temp484 > 4u?*(( int*) _temp484) == Cyc_Absyn_ArrayType: 0){
_LL535: _temp534=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp484)->f1;
goto _LL533; _LL533: _temp532=(( struct Cyc_Absyn_ArrayType_struct*) _temp484)->f2;
goto _LL531; _LL531: _temp530=(( struct Cyc_Absyn_ArrayType_struct*) _temp484)->f3;
goto _LL487;} else{ goto _LL488;} _LL488: if(( unsigned int) _temp484 > 4u?*((
int*) _temp484) == Cyc_Absyn_FnType: 0){ _LL537: _temp536=(( struct Cyc_Absyn_FnType_struct*)
_temp484)->f1; goto _LL489;} else{ goto _LL490;} _LL490: if(( unsigned int)
_temp484 > 4u?*(( int*) _temp484) == Cyc_Absyn_PointerType: 0){ _LL539: _temp538=((
struct Cyc_Absyn_PointerType_struct*) _temp484)->f1; goto _LL491;} else{ goto
_LL492;} _LL492: if( _temp484 ==( void*) Cyc_Absyn_VoidType){ goto _LL493;}
else{ goto _LL494;} _LL494: if(( unsigned int) _temp484 > 4u?*(( int*) _temp484)
== Cyc_Absyn_Evar: 0){ _LL547: _temp546=(( struct Cyc_Absyn_Evar_struct*)
_temp484)->f1; goto _LL545; _LL545: _temp544=(( struct Cyc_Absyn_Evar_struct*)
_temp484)->f2; goto _LL543; _LL543: _temp542=(( struct Cyc_Absyn_Evar_struct*)
_temp484)->f3; goto _LL541; _LL541: _temp540=(( struct Cyc_Absyn_Evar_struct*)
_temp484)->f4; goto _LL495;} else{ goto _LL496;} _LL496: if(( unsigned int)
_temp484 > 4u?*(( int*) _temp484) == Cyc_Absyn_VarType: 0){ _LL549: _temp548=((
struct Cyc_Absyn_VarType_struct*) _temp484)->f1; goto _LL497;} else{ goto _LL498;}
_LL498: if(( unsigned int) _temp484 > 4u?*(( int*) _temp484) == Cyc_Absyn_TunionType:
0){ _LL551: _temp550=(( struct Cyc_Absyn_TunionType_struct*) _temp484)->f1;
_LL557: _temp556=( void*) _temp550.tunion_info; goto _LL555; _LL555: _temp554=
_temp550.targs; goto _LL553; _LL553: _temp552=( void*) _temp550.rgn; goto _LL499;}
else{ goto _LL500;} _LL500: if(( unsigned int) _temp484 > 4u?*(( int*) _temp484)
== Cyc_Absyn_TunionFieldType: 0){ _LL559: _temp558=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp484)->f1; _LL563: _temp562=( void*) _temp558.field_info; goto _LL561;
_LL561: _temp560= _temp558.targs; goto _LL501;} else{ goto _LL502;} _LL502: if((
unsigned int) _temp484 > 4u?*(( int*) _temp484) == Cyc_Absyn_IntType: 0){ _LL567:
_temp566=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp484)->f1; goto _LL565;
_LL565: _temp564=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp484)->f2;
goto _LL503;} else{ goto _LL504;} _LL504: if( _temp484 ==( void*) Cyc_Absyn_FloatType){
goto _LL505;} else{ goto _LL506;} _LL506: if( _temp484 ==( void*) Cyc_Absyn_DoubleType){
goto _LL507;} else{ goto _LL508;} _LL508: if(( unsigned int) _temp484 > 4u?*((
int*) _temp484) == Cyc_Absyn_TupleType: 0){ _LL569: _temp568=(( struct Cyc_Absyn_TupleType_struct*)
_temp484)->f1; goto _LL509;} else{ goto _LL510;} _LL510: if(( unsigned int)
_temp484 > 4u?*(( int*) _temp484) == Cyc_Absyn_StructType: 0){ _LL575: _temp574=((
struct Cyc_Absyn_StructType_struct*) _temp484)->f1; goto _LL573; _LL573:
_temp572=(( struct Cyc_Absyn_StructType_struct*) _temp484)->f2; goto _LL571;
_LL571: _temp570=(( struct Cyc_Absyn_StructType_struct*) _temp484)->f3; goto
_LL511;} else{ goto _LL512;} _LL512: if(( unsigned int) _temp484 > 4u?*(( int*)
_temp484) == Cyc_Absyn_UnionType: 0){ _LL581: _temp580=(( struct Cyc_Absyn_UnionType_struct*)
_temp484)->f1; goto _LL579; _LL579: _temp578=(( struct Cyc_Absyn_UnionType_struct*)
_temp484)->f2; goto _LL577; _LL577: _temp576=(( struct Cyc_Absyn_UnionType_struct*)
_temp484)->f3; goto _LL513;} else{ goto _LL514;} _LL514: if(( unsigned int)
_temp484 > 4u?*(( int*) _temp484) == Cyc_Absyn_AnonStructType: 0){ _LL583:
_temp582=(( struct Cyc_Absyn_AnonStructType_struct*) _temp484)->f1; goto _LL515;}
else{ goto _LL516;} _LL516: if(( unsigned int) _temp484 > 4u?*(( int*) _temp484)
== Cyc_Absyn_AnonUnionType: 0){ _LL585: _temp584=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp484)->f1; goto _LL517;} else{ goto _LL518;} _LL518: if(( unsigned int)
_temp484 > 4u?*(( int*) _temp484) == Cyc_Absyn_EnumType: 0){ _LL589: _temp588=((
struct Cyc_Absyn_EnumType_struct*) _temp484)->f1; goto _LL587; _LL587: _temp586=((
struct Cyc_Absyn_EnumType_struct*) _temp484)->f2; goto _LL519;} else{ goto
_LL520;} _LL520: if(( unsigned int) _temp484 > 4u?*(( int*) _temp484) == Cyc_Absyn_TypedefType:
0){ _LL595: _temp594=(( struct Cyc_Absyn_TypedefType_struct*) _temp484)->f1;
goto _LL593; _LL593: _temp592=(( struct Cyc_Absyn_TypedefType_struct*) _temp484)->f2;
goto _LL591; _LL591: _temp590=(( struct Cyc_Absyn_TypedefType_struct*) _temp484)->f3;
goto _LL521;} else{ goto _LL522;} _LL522: if(( unsigned int) _temp484 > 4u?*((
int*) _temp484) == Cyc_Absyn_RgnHandleType: 0){ _LL597: _temp596=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp484)->f1; goto _LL523;} else{ goto
_LL524;} _LL524: if( _temp484 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL525;} else{
goto _LL526;} _LL526: if(( unsigned int) _temp484 > 4u?*(( int*) _temp484) ==
Cyc_Absyn_AccessEff: 0){ _LL599: _temp598=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp484)->f1; goto _LL527;} else{ goto _LL528;} _LL528: if(( unsigned int)
_temp484 > 4u?*(( int*) _temp484) == Cyc_Absyn_JoinEff: 0){ _LL601: _temp600=((
struct Cyc_Absyn_JoinEff_struct*) _temp484)->f1; goto _LL529;} else{ goto _LL485;}
_LL487: return Cyc_PP_text(({ unsigned char* _temp602=( unsigned char*)"[[[array]]]";
struct _tagged_string _temp603; _temp603.curr= _temp602; _temp603.base= _temp602;
_temp603.last_plus_one= _temp602 + 12; _temp603;})); _LL489: return Cyc_PP_nil_doc();
_LL491: return Cyc_PP_nil_doc(); _LL493: s= Cyc_PP_text(({ unsigned char*
_temp604=( unsigned char*)"void"; struct _tagged_string _temp605; _temp605.curr=
_temp604; _temp605.base= _temp604; _temp605.last_plus_one= _temp604 + 5;
_temp605;})); goto _LL485; _LL495: if( _temp544 != 0){ return Cyc_Absynpp_ntyp2doc((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp544))->v);} else{ s=({ struct
Cyc_PP_Doc*(* _temp606)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp609= Cyc_PP_text(({ unsigned char* _temp623=( unsigned char*)"%"; struct
_tagged_string _temp624; _temp624.curr= _temp623; _temp624.base= _temp623;
_temp624.last_plus_one= _temp623 + 2; _temp624;})); struct Cyc_PP_Doc* _temp610=
Cyc_PP_text(({ unsigned char* _temp621=( unsigned char*)"("; struct
_tagged_string _temp622; _temp622.curr= _temp621; _temp622.base= _temp621;
_temp622.last_plus_one= _temp621 + 2; _temp622;})); struct Cyc_PP_Doc* _temp611=
Cyc_PP_text(( struct _tagged_string) xprintf("%d", _temp542)); struct Cyc_PP_Doc*
_temp612=( ! Cyc_Absynpp_print_full_evars? 1: _temp540 == 0)? Cyc_PP_text(({
unsigned char* _temp619=( unsigned char*)""; struct _tagged_string _temp620;
_temp620.curr= _temp619; _temp620.base= _temp619; _temp620.last_plus_one=
_temp619 + 1; _temp620;})): Cyc_Absynpp_tvars2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp540))->v); struct Cyc_PP_Doc* _temp613=
Cyc_PP_text(({ unsigned char* _temp617=( unsigned char*)")::"; struct
_tagged_string _temp618; _temp618.curr= _temp617; _temp618.base= _temp617;
_temp618.last_plus_one= _temp617 + 4; _temp618;})); struct Cyc_PP_Doc* _temp614=
_temp546 == 0? Cyc_PP_text(({ unsigned char* _temp615=( unsigned char*)"?";
struct _tagged_string _temp616; _temp616.curr= _temp615; _temp616.base= _temp615;
_temp616.last_plus_one= _temp615 + 2; _temp616;})): Cyc_Absynpp_kind2doc(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp546))->v); struct Cyc_PP_Doc* _temp607[
6u]={ _temp609, _temp610, _temp611, _temp612, _temp613, _temp614}; struct
_tagged_ptr1 _temp608={ _temp607, _temp607, _temp607 + 6u}; _temp606( _temp608);});}
goto _LL485; _LL497: s= Cyc_PP_textptr( _temp548->name); if( Cyc_Absynpp_print_all_kinds){
s=({ struct Cyc_PP_Doc*(* _temp625)( struct _tagged_ptr1)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp628= s; struct Cyc_PP_Doc* _temp629= Cyc_PP_text(({
unsigned char* _temp631=( unsigned char*)"::"; struct _tagged_string _temp632;
_temp632.curr= _temp631; _temp632.base= _temp631; _temp632.last_plus_one=
_temp631 + 3; _temp632;})); struct Cyc_PP_Doc* _temp630= Cyc_Absynpp_ckind2doc(
_temp548->kind); struct Cyc_PP_Doc* _temp626[ 3u]={ _temp628, _temp629, _temp630};
struct _tagged_ptr1 _temp627={ _temp626, _temp626, _temp626 + 3u}; _temp625(
_temp627);});} if( Cyc_Absynpp_rewrite_temp_tvars? Cyc_Tcutil_is_temp_tvar(
_temp548): 0){ s=({ struct Cyc_PP_Doc*(* _temp633)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp636= Cyc_PP_text(({ unsigned char* _temp641=(
unsigned char*)"_ /* "; struct _tagged_string _temp642; _temp642.curr= _temp641;
_temp642.base= _temp641; _temp642.last_plus_one= _temp641 + 6; _temp642;}));
struct Cyc_PP_Doc* _temp637= s; struct Cyc_PP_Doc* _temp638= Cyc_PP_text(({
unsigned char* _temp639=( unsigned char*)" */"; struct _tagged_string _temp640;
_temp640.curr= _temp639; _temp640.base= _temp639; _temp640.last_plus_one=
_temp639 + 4; _temp640;})); struct Cyc_PP_Doc* _temp634[ 3u]={ _temp636,
_temp637, _temp638}; struct _tagged_ptr1 _temp635={ _temp634, _temp634, _temp634
+ 3u}; _temp633( _temp635);});} goto _LL485; _LL499:{ void* _temp643= _temp556;
struct Cyc_Absyn_UnknownTunionInfo _temp649; int _temp651; struct _tuple0*
_temp653; struct Cyc_Absyn_Tuniondecl* _temp655; struct Cyc_Absyn_Tuniondecl
_temp657; int _temp658; struct Cyc_Core_Opt* _temp660; struct Cyc_List_List*
_temp662; struct _tuple0* _temp664; void* _temp666; _LL645: if(*(( int*)
_temp643) == Cyc_Absyn_UnknownTunion){ _LL650: _temp649=(( struct Cyc_Absyn_UnknownTunion_struct*)
_temp643)->f1; _LL654: _temp653= _temp649.name; goto _LL652; _LL652: _temp651=
_temp649.is_xtunion; goto _LL646;} else{ goto _LL647;} _LL647: if(*(( int*)
_temp643) == Cyc_Absyn_KnownTunion){ _LL656: _temp655=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp643)->f1; _temp657=* _temp655; _LL667: _temp666=( void*) _temp657.sc; goto
_LL665; _LL665: _temp664= _temp657.name; goto _LL663; _LL663: _temp662= _temp657.tvs;
goto _LL661; _LL661: _temp660= _temp657.fields; goto _LL659; _LL659: _temp658=
_temp657.is_xtunion; goto _LL648;} else{ goto _LL644;} _LL646: _temp664=
_temp653; _temp658= _temp651; goto _LL648; _LL648: { struct Cyc_PP_Doc* _temp672=
Cyc_PP_text( _temp658?({ unsigned char* _temp668=( unsigned char*)"xtunion ";
struct _tagged_string _temp669; _temp669.curr= _temp668; _temp669.base= _temp668;
_temp669.last_plus_one= _temp668 + 9; _temp669;}):({ unsigned char* _temp670=(
unsigned char*)"tunion "; struct _tagged_string _temp671; _temp671.curr=
_temp670; _temp671.base= _temp670; _temp671.last_plus_one= _temp670 + 8;
_temp671;})); goto _LL673; _LL673:{ void* _temp674= Cyc_Tcutil_compress(
_temp552); _LL676: if( _temp674 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL677;}
else{ goto _LL678;} _LL678: goto _LL679; _LL677: s=({ struct Cyc_PP_Doc*(*
_temp680)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp683=
_temp672; struct Cyc_PP_Doc* _temp684= Cyc_Absynpp_qvar2doc( _temp664); struct
Cyc_PP_Doc* _temp685= Cyc_Absynpp_tps2doc( _temp554); struct Cyc_PP_Doc*
_temp681[ 3u]={ _temp683, _temp684, _temp685}; struct _tagged_ptr1 _temp682={
_temp681, _temp681, _temp681 + 3u}; _temp680( _temp682);}); goto _LL675; _LL679:
s=({ struct Cyc_PP_Doc*(* _temp686)( struct _tagged_ptr1)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp689= _temp672; struct Cyc_PP_Doc* _temp690= Cyc_Absynpp_typ2doc(
_temp552); struct Cyc_PP_Doc* _temp691= Cyc_PP_text(({ unsigned char* _temp694=(
unsigned char*)" "; struct _tagged_string _temp695; _temp695.curr= _temp694;
_temp695.base= _temp694; _temp695.last_plus_one= _temp694 + 2; _temp695;}));
struct Cyc_PP_Doc* _temp692= Cyc_Absynpp_qvar2doc( _temp664); struct Cyc_PP_Doc*
_temp693= Cyc_Absynpp_tps2doc( _temp554); struct Cyc_PP_Doc* _temp687[ 5u]={
_temp689, _temp690, _temp691, _temp692, _temp693}; struct _tagged_ptr1 _temp688={
_temp687, _temp687, _temp687 + 5u}; _temp686( _temp688);}); goto _LL675; _LL675:;}
goto _LL644;} _LL644:;} goto _LL485; _LL501:{ void* _temp696= _temp562; struct
Cyc_Absyn_UnknownTunionFieldInfo _temp702; int _temp704; struct _tuple0*
_temp706; struct _tuple0* _temp708; struct Cyc_Absyn_Tunionfield* _temp710;
struct Cyc_Absyn_Tunionfield _temp712; void* _temp713; struct Cyc_Position_Segment*
_temp715; struct Cyc_List_List* _temp717; struct Cyc_List_List* _temp719; struct
_tuple0* _temp721; struct Cyc_Absyn_Tuniondecl* _temp723; struct Cyc_Absyn_Tuniondecl
_temp725; int _temp726; struct Cyc_Core_Opt* _temp728; struct Cyc_List_List*
_temp730; struct _tuple0* _temp732; void* _temp734; _LL698: if(*(( int*)
_temp696) == Cyc_Absyn_UnknownTunionfield){ _LL703: _temp702=(( struct Cyc_Absyn_UnknownTunionfield_struct*)
_temp696)->f1; _LL709: _temp708= _temp702.tunion_name; goto _LL707; _LL707:
_temp706= _temp702.field_name; goto _LL705; _LL705: _temp704= _temp702.is_xtunion;
goto _LL699;} else{ goto _LL700;} _LL700: if(*(( int*) _temp696) == Cyc_Absyn_KnownTunionfield){
_LL724: _temp723=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp696)->f1;
_temp725=* _temp723; _LL735: _temp734=( void*) _temp725.sc; goto _LL733; _LL733:
_temp732= _temp725.name; goto _LL731; _LL731: _temp730= _temp725.tvs; goto
_LL729; _LL729: _temp728= _temp725.fields; goto _LL727; _LL727: _temp726=
_temp725.is_xtunion; goto _LL711; _LL711: _temp710=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp696)->f2; _temp712=* _temp710; _LL722: _temp721= _temp712.name; goto _LL720;
_LL720: _temp719= _temp712.tvs; goto _LL718; _LL718: _temp717= _temp712.typs;
goto _LL716; _LL716: _temp715= _temp712.loc; goto _LL714; _LL714: _temp713=(
void*) _temp712.sc; goto _LL701;} else{ goto _LL697;} _LL699: _temp732= _temp708;
_temp726= _temp704; _temp721= _temp706; goto _LL701; _LL701: { struct Cyc_PP_Doc*
_temp740= Cyc_PP_text( _temp726?({ unsigned char* _temp736=( unsigned char*)"xtunion ";
struct _tagged_string _temp737; _temp737.curr= _temp736; _temp737.base= _temp736;
_temp737.last_plus_one= _temp736 + 9; _temp737;}):({ unsigned char* _temp738=(
unsigned char*)"tunion "; struct _tagged_string _temp739; _temp739.curr=
_temp738; _temp739.base= _temp738; _temp739.last_plus_one= _temp738 + 8;
_temp739;})); goto _LL741; _LL741: s=({ struct Cyc_PP_Doc*(* _temp742)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp745= _temp740; struct Cyc_PP_Doc*
_temp746= Cyc_Absynpp_qvar2doc( _temp732); struct Cyc_PP_Doc* _temp747= Cyc_PP_text(({
unsigned char* _temp749=( unsigned char*)"."; struct _tagged_string _temp750;
_temp750.curr= _temp749; _temp750.base= _temp749; _temp750.last_plus_one=
_temp749 + 2; _temp750;})); struct Cyc_PP_Doc* _temp748= Cyc_Absynpp_qvar2doc(
_temp721); struct Cyc_PP_Doc* _temp743[ 4u]={ _temp745, _temp746, _temp747,
_temp748}; struct _tagged_ptr1 _temp744={ _temp743, _temp743, _temp743 + 4u};
_temp742( _temp744);}); goto _LL697;} _LL697:;} goto _LL485; _LL503: { struct
_tagged_string sns; struct _tagged_string ts;{ void* _temp751= _temp566; _LL753:
if( _temp751 ==( void*) Cyc_Absyn_Signed){ goto _LL754;} else{ goto _LL755;}
_LL755: if( _temp751 ==( void*) Cyc_Absyn_Unsigned){ goto _LL756;} else{ goto
_LL752;} _LL754: sns=({ unsigned char* _temp757=( unsigned char*)""; struct
_tagged_string _temp758; _temp758.curr= _temp757; _temp758.base= _temp757;
_temp758.last_plus_one= _temp757 + 1; _temp758;}); goto _LL752; _LL756: sns=({
unsigned char* _temp759=( unsigned char*)"unsigned "; struct _tagged_string
_temp760; _temp760.curr= _temp759; _temp760.base= _temp759; _temp760.last_plus_one=
_temp759 + 10; _temp760;}); goto _LL752; _LL752:;}{ void* _temp761= _temp564;
_LL763: if( _temp761 ==( void*) Cyc_Absyn_B1){ goto _LL764;} else{ goto _LL765;}
_LL765: if( _temp761 ==( void*) Cyc_Absyn_B2){ goto _LL766;} else{ goto _LL767;}
_LL767: if( _temp761 ==( void*) Cyc_Absyn_B4){ goto _LL768;} else{ goto _LL769;}
_LL769: if( _temp761 ==( void*) Cyc_Absyn_B8){ goto _LL770;} else{ goto _LL762;}
_LL764:{ void* _temp771= _temp566; _LL773: if( _temp771 ==( void*) Cyc_Absyn_Signed){
goto _LL774;} else{ goto _LL775;} _LL775: if( _temp771 ==( void*) Cyc_Absyn_Unsigned){
goto _LL776;} else{ goto _LL772;} _LL774: sns=({ unsigned char* _temp777=(
unsigned char*)"signed "; struct _tagged_string _temp778; _temp778.curr=
_temp777; _temp778.base= _temp777; _temp778.last_plus_one= _temp777 + 8;
_temp778;}); goto _LL772; _LL776: goto _LL772; _LL772:;} ts=({ unsigned char*
_temp779=( unsigned char*)"char"; struct _tagged_string _temp780; _temp780.curr=
_temp779; _temp780.base= _temp779; _temp780.last_plus_one= _temp779 + 5;
_temp780;}); goto _LL762; _LL766: ts=({ unsigned char* _temp781=( unsigned char*)"short";
struct _tagged_string _temp782; _temp782.curr= _temp781; _temp782.base= _temp781;
_temp782.last_plus_one= _temp781 + 6; _temp782;}); goto _LL762; _LL768: ts=({
unsigned char* _temp783=( unsigned char*)"int"; struct _tagged_string _temp784;
_temp784.curr= _temp783; _temp784.base= _temp783; _temp784.last_plus_one=
_temp783 + 4; _temp784;}); goto _LL762; _LL770: ts= Cyc_Absynpp_to_VC?({
unsigned char* _temp785=( unsigned char*)"__int64"; struct _tagged_string
_temp786; _temp786.curr= _temp785; _temp786.base= _temp785; _temp786.last_plus_one=
_temp785 + 8; _temp786;}):({ unsigned char* _temp787=( unsigned char*)"long long";
struct _tagged_string _temp788; _temp788.curr= _temp787; _temp788.base= _temp787;
_temp788.last_plus_one= _temp787 + 10; _temp788;}); goto _LL762; _LL762:;} s=
Cyc_PP_text(( struct _tagged_string)({ struct _tagged_string _temp789= sns;
struct _tagged_string _temp790= ts; xprintf("%.*s%.*s", _temp789.last_plus_one -
_temp789.curr, _temp789.curr, _temp790.last_plus_one - _temp790.curr, _temp790.curr);}));
goto _LL485;} _LL505: s= Cyc_PP_text(({ unsigned char* _temp791=( unsigned char*)"float";
struct _tagged_string _temp792; _temp792.curr= _temp791; _temp792.base= _temp791;
_temp792.last_plus_one= _temp791 + 6; _temp792;})); goto _LL485; _LL507: s= Cyc_PP_text(({
unsigned char* _temp793=( unsigned char*)"double"; struct _tagged_string
_temp794; _temp794.curr= _temp793; _temp794.base= _temp793; _temp794.last_plus_one=
_temp793 + 7; _temp794;})); goto _LL485; _LL509: s=({ struct Cyc_PP_Doc*(*
_temp795)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp798= Cyc_PP_text(({
unsigned char* _temp800=( unsigned char*)"$"; struct _tagged_string _temp801;
_temp801.curr= _temp800; _temp801.base= _temp800; _temp801.last_plus_one=
_temp800 + 2; _temp801;})); struct Cyc_PP_Doc* _temp799= Cyc_Absynpp_args2doc(
_temp568); struct Cyc_PP_Doc* _temp796[ 2u]={ _temp798, _temp799}; struct
_tagged_ptr1 _temp797={ _temp796, _temp796, _temp796 + 2u}; _temp795( _temp797);});
goto _LL485; _LL511: if( _temp574 == 0){ s=({ struct Cyc_PP_Doc*(* _temp802)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp805= Cyc_PP_text(({
unsigned char* _temp807=( unsigned char*)"struct "; struct _tagged_string
_temp808; _temp808.curr= _temp807; _temp808.base= _temp807; _temp808.last_plus_one=
_temp807 + 8; _temp808;})); struct Cyc_PP_Doc* _temp806= Cyc_Absynpp_tps2doc(
_temp572); struct Cyc_PP_Doc* _temp803[ 2u]={ _temp805, _temp806}; struct
_tagged_ptr1 _temp804={ _temp803, _temp803, _temp803 + 2u}; _temp802( _temp804);});}
else{ s=({ struct Cyc_PP_Doc*(* _temp809)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp812= Cyc_PP_text(({ unsigned char* _temp815=(
unsigned char*)"struct "; struct _tagged_string _temp816; _temp816.curr=
_temp815; _temp816.base= _temp815; _temp816.last_plus_one= _temp815 + 8;
_temp816;})); struct Cyc_PP_Doc* _temp813= _temp574 == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc((
struct _tuple0*) _check_null( _temp574)); struct Cyc_PP_Doc* _temp814= Cyc_Absynpp_tps2doc(
_temp572); struct Cyc_PP_Doc* _temp810[ 3u]={ _temp812, _temp813, _temp814};
struct _tagged_ptr1 _temp811={ _temp810, _temp810, _temp810 + 3u}; _temp809(
_temp811);});} goto _LL485; _LL513: if( _temp580 == 0){ s=({ struct Cyc_PP_Doc*(*
_temp817)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp820= Cyc_PP_text(({
unsigned char* _temp822=( unsigned char*)"union "; struct _tagged_string
_temp823; _temp823.curr= _temp822; _temp823.base= _temp822; _temp823.last_plus_one=
_temp822 + 7; _temp823;})); struct Cyc_PP_Doc* _temp821= Cyc_Absynpp_tps2doc(
_temp578); struct Cyc_PP_Doc* _temp818[ 2u]={ _temp820, _temp821}; struct
_tagged_ptr1 _temp819={ _temp818, _temp818, _temp818 + 2u}; _temp817( _temp819);});}
else{ s=({ struct Cyc_PP_Doc*(* _temp824)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp827= Cyc_PP_text(({ unsigned char* _temp830=(
unsigned char*)"union "; struct _tagged_string _temp831; _temp831.curr= _temp830;
_temp831.base= _temp830; _temp831.last_plus_one= _temp830 + 7; _temp831;}));
struct Cyc_PP_Doc* _temp828= _temp580 == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc((
struct _tuple0*) _check_null( _temp580)); struct Cyc_PP_Doc* _temp829= Cyc_Absynpp_tps2doc(
_temp578); struct Cyc_PP_Doc* _temp825[ 3u]={ _temp827, _temp828, _temp829};
struct _tagged_ptr1 _temp826={ _temp825, _temp825, _temp825 + 3u}; _temp824(
_temp826);});} goto _LL485; _LL515: s=({ struct Cyc_PP_Doc*(* _temp832)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp835= Cyc_PP_text(({
unsigned char* _temp840=( unsigned char*)"struct {"; struct _tagged_string
_temp841; _temp841.curr= _temp840; _temp841.base= _temp840; _temp841.last_plus_one=
_temp840 + 9; _temp841;})); struct Cyc_PP_Doc* _temp836= Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc(
_temp582)); struct Cyc_PP_Doc* _temp837= Cyc_PP_text(({ unsigned char* _temp838=(
unsigned char*)"}"; struct _tagged_string _temp839; _temp839.curr= _temp838;
_temp839.base= _temp838; _temp839.last_plus_one= _temp838 + 2; _temp839;}));
struct Cyc_PP_Doc* _temp833[ 3u]={ _temp835, _temp836, _temp837}; struct
_tagged_ptr1 _temp834={ _temp833, _temp833, _temp833 + 3u}; _temp832( _temp834);});
goto _LL485; _LL517: s=({ struct Cyc_PP_Doc*(* _temp842)( struct _tagged_ptr1)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp845= Cyc_PP_text(({ unsigned char* _temp850=(
unsigned char*)"union {"; struct _tagged_string _temp851; _temp851.curr=
_temp850; _temp851.base= _temp850; _temp851.last_plus_one= _temp850 + 8;
_temp851;})); struct Cyc_PP_Doc* _temp846= Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc(
_temp584)); struct Cyc_PP_Doc* _temp847= Cyc_PP_text(({ unsigned char* _temp848=(
unsigned char*)"}"; struct _tagged_string _temp849; _temp849.curr= _temp848;
_temp849.base= _temp848; _temp849.last_plus_one= _temp848 + 2; _temp849;}));
struct Cyc_PP_Doc* _temp843[ 3u]={ _temp845, _temp846, _temp847}; struct
_tagged_ptr1 _temp844={ _temp843, _temp843, _temp843 + 3u}; _temp842( _temp844);});
goto _LL485; _LL519: s=({ struct Cyc_PP_Doc*(* _temp852)( struct _tagged_ptr1)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp855= Cyc_PP_text(({ unsigned char* _temp857=(
unsigned char*)"enum "; struct _tagged_string _temp858; _temp858.curr= _temp857;
_temp858.base= _temp857; _temp858.last_plus_one= _temp857 + 6; _temp858;}));
struct Cyc_PP_Doc* _temp856= Cyc_Absynpp_qvar2doc( _temp588); struct Cyc_PP_Doc*
_temp853[ 2u]={ _temp855, _temp856}; struct _tagged_ptr1 _temp854={ _temp853,
_temp853, _temp853 + 2u}; _temp852( _temp854);}); goto _LL485; _LL521: s=({
struct Cyc_PP_Doc*(* _temp859)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp862= Cyc_Absynpp_qvar2doc( _temp594); struct Cyc_PP_Doc* _temp863= Cyc_Absynpp_tps2doc(
_temp592); struct Cyc_PP_Doc* _temp860[ 2u]={ _temp862, _temp863}; struct
_tagged_ptr1 _temp861={ _temp860, _temp860, _temp860 + 2u}; _temp859( _temp861);});
goto _LL485; _LL523: s=({ struct Cyc_PP_Doc*(* _temp864)( struct _tagged_ptr1)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp867= Cyc_PP_text(({ unsigned char* _temp872=(
unsigned char*)"region_t<"; struct _tagged_string _temp873; _temp873.curr=
_temp872; _temp873.base= _temp872; _temp873.last_plus_one= _temp872 + 10;
_temp873;})); struct Cyc_PP_Doc* _temp868= Cyc_Absynpp_rgn2doc( _temp596);
struct Cyc_PP_Doc* _temp869= Cyc_PP_text(({ unsigned char* _temp870=(
unsigned char*)">"; struct _tagged_string _temp871; _temp871.curr= _temp870;
_temp871.base= _temp870; _temp871.last_plus_one= _temp870 + 2; _temp871;}));
struct Cyc_PP_Doc* _temp865[ 3u]={ _temp867, _temp868, _temp869}; struct
_tagged_ptr1 _temp866={ _temp865, _temp865, _temp865 + 3u}; _temp864( _temp866);});
goto _LL485; _LL525: s= Cyc_Absynpp_rgn2doc( t); goto _LL485; _LL527: s= Cyc_Absynpp_eff2doc(
t); goto _LL485; _LL529: s= Cyc_Absynpp_eff2doc( t); goto _LL485; _LL485:;}
return s;} struct Cyc_PP_Doc* Cyc_Absynpp_vo2doc( struct Cyc_Core_Opt* vo){
return vo == 0? Cyc_PP_nil_doc(): Cyc_PP_text(*(( struct _tagged_string*)((
struct Cyc_Core_Opt*) _check_null( vo))->v));} struct _tuple8{ void* f1; void*
f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_rgn_cmp2doc( struct _tuple8* cmp){ struct
_tuple8 _temp876; void* _temp877; void* _temp879; struct _tuple8* _temp874= cmp;
_temp876=* _temp874; _LL880: _temp879= _temp876.f1; goto _LL878; _LL878:
_temp877= _temp876.f2; goto _LL875; _LL875: return({ struct Cyc_PP_Doc*(*
_temp881)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp884= Cyc_Absynpp_rgn2doc(
_temp879); struct Cyc_PP_Doc* _temp885= Cyc_PP_text(({ unsigned char* _temp887=(
unsigned char*)" < "; struct _tagged_string _temp888; _temp888.curr= _temp887;
_temp888.base= _temp887; _temp888.last_plus_one= _temp887 + 4; _temp888;}));
struct Cyc_PP_Doc* _temp886= Cyc_Absynpp_rgn2doc( _temp877); struct Cyc_PP_Doc*
_temp882[ 3u]={ _temp884, _temp885, _temp886}; struct _tagged_ptr1 _temp883={
_temp882, _temp882, _temp882 + 3u}; _temp881( _temp883);});} struct Cyc_PP_Doc*
Cyc_Absynpp_rgnpo2doc( struct Cyc_List_List* po){ return Cyc_PP_group(({
unsigned char* _temp889=( unsigned char*)""; struct _tagged_string _temp890;
_temp890.curr= _temp889; _temp890.base= _temp889; _temp890.last_plus_one=
_temp889 + 1; _temp890;}),({ unsigned char* _temp891=( unsigned char*)""; struct
_tagged_string _temp892; _temp892.curr= _temp891; _temp892.base= _temp891;
_temp892.last_plus_one= _temp891 + 1; _temp892;}),({ unsigned char* _temp893=(
unsigned char*)","; struct _tagged_string _temp894; _temp894.curr= _temp893;
_temp894.base= _temp893; _temp894.last_plus_one= _temp893 + 2; _temp894;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple8*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_rgn_cmp2doc, po));} struct Cyc_PP_Doc* Cyc_Absynpp_funarg2doc(
struct _tuple1* t){ struct Cyc_Core_Opt* dopt=(* t).f1 == 0? 0:({ struct Cyc_Core_Opt*
_temp895=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp895->v=( void*) Cyc_PP_text(*(( struct _tagged_string*)(( struct Cyc_Core_Opt*)
_check_null((* t).f1))->v)); _temp895;}); return Cyc_Absynpp_tqtd2doc((* t).f2,(*
t).f3, dopt);} struct Cyc_PP_Doc* Cyc_Absynpp_funargs2doc( struct Cyc_List_List*
args, int c_varargs, struct Cyc_Absyn_VarargInfo* cyc_varargs, struct Cyc_Core_Opt*
effopt, struct Cyc_List_List* rgn_po){ struct Cyc_List_List* _temp896=(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc, args); goto _LL897; _LL897: { struct
Cyc_PP_Doc* eff_doc; if( c_varargs){ _temp896=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp896,({
struct Cyc_List_List* _temp898=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp898->hd=( void*) Cyc_PP_text(({ unsigned char*
_temp899=( unsigned char*)"..."; struct _tagged_string _temp900; _temp900.curr=
_temp899; _temp900.base= _temp899; _temp900.last_plus_one= _temp899 + 4;
_temp900;})); _temp898->tl= 0; _temp898;}));} else{ if( cyc_varargs != 0){
struct Cyc_PP_Doc* _temp915=({ struct Cyc_PP_Doc*(* _temp901)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp904= Cyc_PP_text(({
unsigned char* _temp913=( unsigned char*)"..."; struct _tagged_string _temp914;
_temp914.curr= _temp913; _temp914.base= _temp913; _temp914.last_plus_one=
_temp913 + 4; _temp914;})); struct Cyc_PP_Doc* _temp905= Cyc_Absynpp_typ2doc((
void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->rgn); struct
Cyc_PP_Doc* _temp906=(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->inject?
Cyc_PP_text(({ unsigned char* _temp909=( unsigned char*)" inject "; struct
_tagged_string _temp910; _temp910.curr= _temp909; _temp910.base= _temp909;
_temp910.last_plus_one= _temp909 + 9; _temp910;})): Cyc_PP_text(({ unsigned char*
_temp911=( unsigned char*)" "; struct _tagged_string _temp912; _temp912.curr=
_temp911; _temp912.base= _temp911; _temp912.last_plus_one= _temp911 + 2;
_temp912;})); struct Cyc_PP_Doc* _temp907= Cyc_Absynpp_funarg2doc(({ struct
_tuple1* _temp908=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp908->f1=(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->name;
_temp908->f2=(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->tq;
_temp908->f3=( void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->type;
_temp908;})); struct Cyc_PP_Doc* _temp902[ 4u]={ _temp904, _temp905, _temp906,
_temp907}; struct _tagged_ptr1 _temp903={ _temp902, _temp902, _temp902 + 4u};
_temp901( _temp903);}); goto _LL916; _LL916: _temp896=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp896,({
struct Cyc_List_List* _temp917=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp917->hd=( void*) _temp915; _temp917->tl= 0;
_temp917;}));}}{ struct Cyc_PP_Doc* _temp924= Cyc_PP_group(({ unsigned char*
_temp918=( unsigned char*)""; struct _tagged_string _temp919; _temp919.curr=
_temp918; _temp919.base= _temp918; _temp919.last_plus_one= _temp918 + 1;
_temp919;}),({ unsigned char* _temp920=( unsigned char*)""; struct
_tagged_string _temp921; _temp921.curr= _temp920; _temp921.base= _temp920;
_temp921.last_plus_one= _temp920 + 1; _temp921;}),({ unsigned char* _temp922=(
unsigned char*)","; struct _tagged_string _temp923; _temp923.curr= _temp922;
_temp923.base= _temp922; _temp923.last_plus_one= _temp922 + 2; _temp923;}),
_temp896); goto _LL925; _LL925: if( effopt != 0){ _temp924=({ struct Cyc_PP_Doc*(*
_temp926)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp929=
_temp924; struct Cyc_PP_Doc* _temp930= Cyc_PP_text(({ unsigned char* _temp932=(
unsigned char*)";"; struct _tagged_string _temp933; _temp933.curr= _temp932;
_temp933.base= _temp932; _temp933.last_plus_one= _temp932 + 2; _temp933;}));
struct Cyc_PP_Doc* _temp931= Cyc_Absynpp_eff2doc(( void*)(( struct Cyc_Core_Opt*)
_check_null( effopt))->v); struct Cyc_PP_Doc* _temp927[ 3u]={ _temp929, _temp930,
_temp931}; struct _tagged_ptr1 _temp928={ _temp927, _temp927, _temp927 + 3u};
_temp926( _temp928);});} if( rgn_po != 0){ _temp924=({ struct Cyc_PP_Doc*(*
_temp934)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp937=
_temp924; struct Cyc_PP_Doc* _temp938= Cyc_PP_text(({ unsigned char* _temp940=(
unsigned char*)":"; struct _tagged_string _temp941; _temp941.curr= _temp940;
_temp941.base= _temp940; _temp941.last_plus_one= _temp940 + 2; _temp941;}));
struct Cyc_PP_Doc* _temp939= Cyc_Absynpp_rgnpo2doc( rgn_po); struct Cyc_PP_Doc*
_temp935[ 3u]={ _temp937, _temp938, _temp939}; struct _tagged_ptr1 _temp936={
_temp935, _temp935, _temp935 + 3u}; _temp934( _temp936);});} return({ struct Cyc_PP_Doc*(*
_temp942)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp945= Cyc_PP_text(({
unsigned char* _temp950=( unsigned char*)"("; struct _tagged_string _temp951;
_temp951.curr= _temp950; _temp951.base= _temp950; _temp951.last_plus_one=
_temp950 + 2; _temp951;})); struct Cyc_PP_Doc* _temp946= _temp924; struct Cyc_PP_Doc*
_temp947= Cyc_PP_text(({ unsigned char* _temp948=( unsigned char*)")"; struct
_tagged_string _temp949; _temp949.curr= _temp948; _temp949.base= _temp948;
_temp949.last_plus_one= _temp948 + 2; _temp949;})); struct Cyc_PP_Doc* _temp943[
3u]={ _temp945, _temp946, _temp947}; struct _tagged_ptr1 _temp944={ _temp943,
_temp943, _temp943 + 3u}; _temp942( _temp944);});}}} struct _tuple1* Cyc_Absynpp_arg_mk_opt(
struct _tuple3* arg){ return({ struct _tuple1* _temp952=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp952->f1=({ struct Cyc_Core_Opt*
_temp953=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp953->v=( void*)(* arg).f1; _temp953;}); _temp952->f2=(* arg).f2; _temp952->f3=(*
arg).f3; _temp952;});} struct Cyc_PP_Doc* Cyc_Absynpp_var2doc( struct
_tagged_string* v){ return Cyc_PP_text(* v);} struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc(
struct _tuple0* q){ struct Cyc_List_List* _temp954= 0; goto _LL955; _LL955: {
int match;{ void* _temp956=(* q).f1; struct Cyc_List_List* _temp964; struct Cyc_List_List*
_temp966; _LL958: if( _temp956 ==( void*) Cyc_Absyn_Loc_n){ goto _LL959;} else{
goto _LL960;} _LL960: if(( unsigned int) _temp956 > 1u?*(( int*) _temp956) ==
Cyc_Absyn_Rel_n: 0){ _LL965: _temp964=(( struct Cyc_Absyn_Rel_n_struct*)
_temp956)->f1; goto _LL961;} else{ goto _LL962;} _LL962: if(( unsigned int)
_temp956 > 1u?*(( int*) _temp956) == Cyc_Absyn_Abs_n: 0){ _LL967: _temp966=((
struct Cyc_Absyn_Abs_n_struct*) _temp956)->f1; goto _LL963;} else{ goto _LL957;}
_LL959: _temp964= 0; goto _LL961; _LL961: match= 0; _temp954= _temp964; goto
_LL957; _LL963: match= Cyc_Absynpp_use_curr_namespace?(( int(*)( int(* cmp)(
struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List* l1,
struct Cyc_List_List* l2)) Cyc_List_list_prefix)( Cyc_String_strptrcmp, _temp966,
Cyc_Absynpp_curr_namespace): 0; _temp954=( Cyc_Absynpp_qvar_to_Cids? Cyc_Absynpp_add_cyc_prefix:
0)?({ struct Cyc_List_List* _temp968=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp968->hd=( void*) Cyc_Absynpp_cyc_stringptr;
_temp968->tl= _temp966; _temp968;}): _temp966; goto _LL957; _LL957:;} if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_PP_text(( struct _tagged_string) Cyc_String_str_sepstr((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp954,({
struct Cyc_List_List* _temp969=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp969->hd=( void*)(* q).f2; _temp969->tl= 0; _temp969;})),({
unsigned char* _temp970=( unsigned char*)"_"; struct _tagged_string _temp971;
_temp971.curr= _temp970; _temp971.base= _temp970; _temp971.last_plus_one=
_temp970 + 2; _temp971;})));} else{ if( match){ return Cyc_Absynpp_var2doc((* q).f2);}
else{ return Cyc_PP_text(( struct _tagged_string) Cyc_String_str_sepstr(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp954,({ struct Cyc_List_List* _temp972=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp972->hd=( void*)(* q).f2; _temp972->tl= 0;
_temp972;})),({ unsigned char* _temp973=( unsigned char*)"::"; struct
_tagged_string _temp974; _temp974.curr= _temp973; _temp974.base= _temp973;
_temp974.last_plus_one= _temp973 + 3; _temp974;})));}}}} struct Cyc_PP_Doc* Cyc_Absynpp_typedef_name2doc(
struct _tuple0* v){ if( Cyc_Absynpp_qvar_to_Cids){ return Cyc_Absynpp_qvar2doc(
v);} if( Cyc_Absynpp_use_curr_namespace){ void* _temp975=(* v).f1; struct Cyc_List_List*
_temp985; struct Cyc_List_List* _temp987; _LL977: if( _temp975 ==( void*) Cyc_Absyn_Loc_n){
goto _LL978;} else{ goto _LL979;} _LL979: if(( unsigned int) _temp975 > 1u?*((
int*) _temp975) == Cyc_Absyn_Rel_n: 0){ _LL986: _temp985=(( struct Cyc_Absyn_Rel_n_struct*)
_temp975)->f1; if( _temp985 == 0){ goto _LL980;} else{ goto _LL981;}} else{ goto
_LL981;} _LL981: if(( unsigned int) _temp975 > 1u?*(( int*) _temp975) == Cyc_Absyn_Abs_n:
0){ _LL988: _temp987=(( struct Cyc_Absyn_Abs_n_struct*) _temp975)->f1; goto
_LL982;} else{ goto _LL983;} _LL983: goto _LL984; _LL978: goto _LL980; _LL980:
return Cyc_Absynpp_var2doc((* v).f2); _LL982: if((( int(*)( int(* cmp)( struct
_tagged_string*, struct _tagged_string*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_List_list_cmp)( Cyc_String_strptrcmp, _temp987, Cyc_Absynpp_curr_namespace)
== 0){ return Cyc_Absynpp_var2doc((* v).f2);} else{ goto _LL984;} _LL984: return({
struct Cyc_PP_Doc*(* _temp989)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp992= Cyc_PP_text(({ unsigned char* _temp994=( unsigned char*)"/* bad namespace : */ ";
struct _tagged_string _temp995; _temp995.curr= _temp994; _temp995.base= _temp994;
_temp995.last_plus_one= _temp994 + 23; _temp995;})); struct Cyc_PP_Doc* _temp993=
Cyc_Absynpp_qvar2doc( v); struct Cyc_PP_Doc* _temp990[ 2u]={ _temp992, _temp993};
struct _tagged_ptr1 _temp991={ _temp990, _temp990, _temp990 + 2u}; _temp989(
_temp991);}); _LL976:;} else{ return Cyc_Absynpp_var2doc((* v).f2);}} struct Cyc_PP_Doc*
Cyc_Absynpp_typ2doc( void* t){ return Cyc_Absynpp_tqtd2doc( Cyc_Absyn_empty_tqual(),
t, 0);} int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp* e){ void* _temp996=(
void*) e->r; void* _temp1070; void* _temp1072; struct _tuple0* _temp1074; struct
_tuple0* _temp1076; struct Cyc_List_List* _temp1078; void* _temp1080; struct Cyc_Absyn_Exp*
_temp1082; struct Cyc_Core_Opt* _temp1084; struct Cyc_Absyn_Exp* _temp1086; void*
_temp1088; struct Cyc_Absyn_Exp* _temp1090; struct Cyc_Absyn_Exp* _temp1092;
struct Cyc_Absyn_Exp* _temp1094; struct Cyc_Absyn_Exp* _temp1096; struct Cyc_Absyn_Exp*
_temp1098; struct Cyc_Absyn_Exp* _temp1100; struct Cyc_List_List* _temp1102;
struct Cyc_Absyn_Exp* _temp1104; struct Cyc_Absyn_VarargCallInfo* _temp1106;
struct Cyc_List_List* _temp1108; struct Cyc_Absyn_Exp* _temp1110; struct Cyc_Absyn_Exp*
_temp1112; struct Cyc_Absyn_Exp* _temp1114; struct Cyc_List_List* _temp1116;
struct Cyc_Absyn_Exp* _temp1118; struct Cyc_Absyn_Exp* _temp1120; void*
_temp1122; struct Cyc_Absyn_Exp* _temp1124; struct Cyc_Absyn_Exp* _temp1126;
struct Cyc_Absyn_Exp* _temp1128; void* _temp1130; struct Cyc_Absyn_Exp*
_temp1132; struct _tagged_string* _temp1134; void* _temp1136; struct Cyc_Absyn_Exp*
_temp1138; struct _tagged_string* _temp1140; struct Cyc_Absyn_Exp* _temp1142;
struct _tagged_string* _temp1144; struct Cyc_Absyn_Exp* _temp1146; struct Cyc_Absyn_Exp*
_temp1148; struct Cyc_Absyn_Exp* _temp1150; struct Cyc_List_List* _temp1152;
struct Cyc_List_List* _temp1154; struct _tuple1* _temp1156; struct Cyc_List_List*
_temp1158; struct Cyc_Absyn_Exp* _temp1160; struct Cyc_Absyn_Exp* _temp1162;
struct Cyc_Absyn_Vardecl* _temp1164; struct Cyc_Absyn_Structdecl* _temp1166;
struct Cyc_List_List* _temp1168; struct Cyc_Core_Opt* _temp1170; struct _tuple0*
_temp1172; struct Cyc_List_List* _temp1174; void* _temp1176; struct Cyc_Absyn_Tunionfield*
_temp1178; struct Cyc_Absyn_Tuniondecl* _temp1180; struct Cyc_List_List*
_temp1182; struct Cyc_Core_Opt* _temp1184; struct Cyc_Core_Opt* _temp1186;
struct Cyc_Absyn_Enumfield* _temp1188; struct Cyc_Absyn_Enumdecl* _temp1190;
struct _tuple0* _temp1192; void* _temp1194; struct Cyc_Absyn_Exp* _temp1196;
struct Cyc_List_List* _temp1198; struct Cyc_Core_Opt* _temp1200; struct Cyc_Absyn_Stmt*
_temp1202; struct Cyc_Absyn_Fndecl* _temp1204; struct Cyc_Absyn_Exp* _temp1206;
_LL998: if(*(( int*) _temp996) == Cyc_Absyn_Const_e){ _LL1071: _temp1070=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp996)->f1; goto _LL999;} else{ goto
_LL1000;} _LL1000: if(*(( int*) _temp996) == Cyc_Absyn_Var_e){ _LL1075:
_temp1074=(( struct Cyc_Absyn_Var_e_struct*) _temp996)->f1; goto _LL1073;
_LL1073: _temp1072=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp996)->f2;
goto _LL1001;} else{ goto _LL1002;} _LL1002: if(*(( int*) _temp996) == Cyc_Absyn_UnknownId_e){
_LL1077: _temp1076=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp996)->f1; goto
_LL1003;} else{ goto _LL1004;} _LL1004: if(*(( int*) _temp996) == Cyc_Absyn_Primop_e){
_LL1081: _temp1080=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp996)->f1;
goto _LL1079; _LL1079: _temp1078=(( struct Cyc_Absyn_Primop_e_struct*) _temp996)->f2;
goto _LL1005;} else{ goto _LL1006;} _LL1006: if(*(( int*) _temp996) == Cyc_Absyn_AssignOp_e){
_LL1087: _temp1086=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp996)->f1; goto
_LL1085; _LL1085: _temp1084=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp996)->f2;
goto _LL1083; _LL1083: _temp1082=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp996)->f3; goto _LL1007;} else{ goto _LL1008;} _LL1008: if(*(( int*)
_temp996) == Cyc_Absyn_Increment_e){ _LL1091: _temp1090=(( struct Cyc_Absyn_Increment_e_struct*)
_temp996)->f1; goto _LL1089; _LL1089: _temp1088=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp996)->f2; goto _LL1009;} else{ goto _LL1010;} _LL1010: if(*(( int*)
_temp996) == Cyc_Absyn_Conditional_e){ _LL1097: _temp1096=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp996)->f1; goto _LL1095; _LL1095: _temp1094=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp996)->f2; goto _LL1093; _LL1093: _temp1092=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp996)->f3; goto _LL1011;} else{ goto _LL1012;} _LL1012: if(*(( int*)
_temp996) == Cyc_Absyn_SeqExp_e){ _LL1101: _temp1100=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp996)->f1; goto _LL1099; _LL1099: _temp1098=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp996)->f2; goto _LL1013;} else{ goto _LL1014;} _LL1014: if(*(( int*)
_temp996) == Cyc_Absyn_UnknownCall_e){ _LL1105: _temp1104=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp996)->f1; goto _LL1103; _LL1103: _temp1102=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp996)->f2; goto _LL1015;} else{ goto _LL1016;} _LL1016: if(*(( int*)
_temp996) == Cyc_Absyn_FnCall_e){ _LL1111: _temp1110=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp996)->f1; goto _LL1109; _LL1109: _temp1108=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp996)->f2; goto _LL1107; _LL1107: _temp1106=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp996)->f3; goto _LL1017;} else{ goto _LL1018;} _LL1018: if(*(( int*)
_temp996) == Cyc_Absyn_Throw_e){ _LL1113: _temp1112=(( struct Cyc_Absyn_Throw_e_struct*)
_temp996)->f1; goto _LL1019;} else{ goto _LL1020;} _LL1020: if(*(( int*)
_temp996) == Cyc_Absyn_NoInstantiate_e){ _LL1115: _temp1114=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp996)->f1; goto _LL1021;} else{ goto _LL1022;} _LL1022: if(*(( int*)
_temp996) == Cyc_Absyn_Instantiate_e){ _LL1119: _temp1118=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp996)->f1; goto _LL1117; _LL1117: _temp1116=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp996)->f2; goto _LL1023;} else{ goto _LL1024;} _LL1024: if(*(( int*)
_temp996) == Cyc_Absyn_Cast_e){ _LL1123: _temp1122=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp996)->f1; goto _LL1121; _LL1121: _temp1120=(( struct Cyc_Absyn_Cast_e_struct*)
_temp996)->f2; goto _LL1025;} else{ goto _LL1026;} _LL1026: if(*(( int*)
_temp996) == Cyc_Absyn_New_e){ _LL1127: _temp1126=(( struct Cyc_Absyn_New_e_struct*)
_temp996)->f1; goto _LL1125; _LL1125: _temp1124=(( struct Cyc_Absyn_New_e_struct*)
_temp996)->f2; goto _LL1027;} else{ goto _LL1028;} _LL1028: if(*(( int*)
_temp996) == Cyc_Absyn_Address_e){ _LL1129: _temp1128=(( struct Cyc_Absyn_Address_e_struct*)
_temp996)->f1; goto _LL1029;} else{ goto _LL1030;} _LL1030: if(*(( int*)
_temp996) == Cyc_Absyn_Sizeoftyp_e){ _LL1131: _temp1130=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp996)->f1; goto _LL1031;} else{ goto _LL1032;} _LL1032: if(*(( int*)
_temp996) == Cyc_Absyn_Sizeofexp_e){ _LL1133: _temp1132=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp996)->f1; goto _LL1033;} else{ goto _LL1034;} _LL1034: if(*(( int*)
_temp996) == Cyc_Absyn_Offsetof_e){ _LL1137: _temp1136=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp996)->f1; goto _LL1135; _LL1135: _temp1134=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp996)->f2; goto _LL1035;} else{ goto _LL1036;} _LL1036: if(*(( int*)
_temp996) == Cyc_Absyn_Deref_e){ _LL1139: _temp1138=(( struct Cyc_Absyn_Deref_e_struct*)
_temp996)->f1; goto _LL1037;} else{ goto _LL1038;} _LL1038: if(*(( int*)
_temp996) == Cyc_Absyn_StructMember_e){ _LL1143: _temp1142=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp996)->f1; goto _LL1141; _LL1141: _temp1140=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp996)->f2; goto _LL1039;} else{ goto _LL1040;} _LL1040: if(*(( int*)
_temp996) == Cyc_Absyn_StructArrow_e){ _LL1147: _temp1146=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp996)->f1; goto _LL1145; _LL1145: _temp1144=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp996)->f2; goto _LL1041;} else{ goto _LL1042;} _LL1042: if(*(( int*)
_temp996) == Cyc_Absyn_Subscript_e){ _LL1151: _temp1150=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp996)->f1; goto _LL1149; _LL1149: _temp1148=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp996)->f2; goto _LL1043;} else{ goto _LL1044;} _LL1044: if(*(( int*)
_temp996) == Cyc_Absyn_Tuple_e){ _LL1153: _temp1152=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp996)->f1; goto _LL1045;} else{ goto _LL1046;} _LL1046: if(*(( int*)
_temp996) == Cyc_Absyn_CompoundLit_e){ _LL1157: _temp1156=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp996)->f1; goto _LL1155; _LL1155: _temp1154=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp996)->f2; goto _LL1047;} else{ goto _LL1048;} _LL1048: if(*(( int*)
_temp996) == Cyc_Absyn_Array_e){ _LL1159: _temp1158=(( struct Cyc_Absyn_Array_e_struct*)
_temp996)->f1; goto _LL1049;} else{ goto _LL1050;} _LL1050: if(*(( int*)
_temp996) == Cyc_Absyn_Comprehension_e){ _LL1165: _temp1164=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp996)->f1; goto _LL1163; _LL1163: _temp1162=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp996)->f2; goto _LL1161; _LL1161: _temp1160=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp996)->f3; goto _LL1051;} else{ goto _LL1052;} _LL1052: if(*(( int*)
_temp996) == Cyc_Absyn_Struct_e){ _LL1173: _temp1172=(( struct Cyc_Absyn_Struct_e_struct*)
_temp996)->f1; goto _LL1171; _LL1171: _temp1170=(( struct Cyc_Absyn_Struct_e_struct*)
_temp996)->f2; goto _LL1169; _LL1169: _temp1168=(( struct Cyc_Absyn_Struct_e_struct*)
_temp996)->f3; goto _LL1167; _LL1167: _temp1166=(( struct Cyc_Absyn_Struct_e_struct*)
_temp996)->f4; goto _LL1053;} else{ goto _LL1054;} _LL1054: if(*(( int*)
_temp996) == Cyc_Absyn_AnonStruct_e){ _LL1177: _temp1176=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp996)->f1; goto _LL1175; _LL1175: _temp1174=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp996)->f2; goto _LL1055;} else{ goto _LL1056;} _LL1056: if(*(( int*)
_temp996) == Cyc_Absyn_Tunion_e){ _LL1187: _temp1186=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp996)->f1; goto _LL1185; _LL1185: _temp1184=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp996)->f2; goto _LL1183; _LL1183: _temp1182=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp996)->f3; goto _LL1181; _LL1181: _temp1180=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp996)->f4; goto _LL1179; _LL1179: _temp1178=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp996)->f5; goto _LL1057;} else{ goto _LL1058;} _LL1058: if(*(( int*)
_temp996) == Cyc_Absyn_Enum_e){ _LL1193: _temp1192=(( struct Cyc_Absyn_Enum_e_struct*)
_temp996)->f1; goto _LL1191; _LL1191: _temp1190=(( struct Cyc_Absyn_Enum_e_struct*)
_temp996)->f2; goto _LL1189; _LL1189: _temp1188=(( struct Cyc_Absyn_Enum_e_struct*)
_temp996)->f3; goto _LL1059;} else{ goto _LL1060;} _LL1060: if(*(( int*)
_temp996) == Cyc_Absyn_Malloc_e){ _LL1197: _temp1196=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp996)->f1; goto _LL1195; _LL1195: _temp1194=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp996)->f2; goto _LL1061;} else{ goto _LL1062;} _LL1062: if(*(( int*)
_temp996) == Cyc_Absyn_UnresolvedMem_e){ _LL1201: _temp1200=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp996)->f1; goto _LL1199; _LL1199: _temp1198=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp996)->f2; goto _LL1063;} else{ goto _LL1064;} _LL1064: if(*(( int*)
_temp996) == Cyc_Absyn_StmtExp_e){ _LL1203: _temp1202=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp996)->f1; goto _LL1065;} else{ goto _LL1066;} _LL1066: if(*(( int*)
_temp996) == Cyc_Absyn_Codegen_e){ _LL1205: _temp1204=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp996)->f1; goto _LL1067;} else{ goto _LL1068;} _LL1068: if(*(( int*)
_temp996) == Cyc_Absyn_Fill_e){ _LL1207: _temp1206=(( struct Cyc_Absyn_Fill_e_struct*)
_temp996)->f1; goto _LL1069;} else{ goto _LL997;} _LL999: return 10000; _LL1001:
return 10000; _LL1003: return 10000; _LL1005: { void* _temp1208= _temp1080;
_LL1210: if( _temp1208 ==( void*) Cyc_Absyn_Plus){ goto _LL1211;} else{ goto
_LL1212;} _LL1212: if( _temp1208 ==( void*) Cyc_Absyn_Times){ goto _LL1213;}
else{ goto _LL1214;} _LL1214: if( _temp1208 ==( void*) Cyc_Absyn_Minus){ goto
_LL1215;} else{ goto _LL1216;} _LL1216: if( _temp1208 ==( void*) Cyc_Absyn_Div){
goto _LL1217;} else{ goto _LL1218;} _LL1218: if( _temp1208 ==( void*) Cyc_Absyn_Mod){
goto _LL1219;} else{ goto _LL1220;} _LL1220: if( _temp1208 ==( void*) Cyc_Absyn_Eq){
goto _LL1221;} else{ goto _LL1222;} _LL1222: if( _temp1208 ==( void*) Cyc_Absyn_Neq){
goto _LL1223;} else{ goto _LL1224;} _LL1224: if( _temp1208 ==( void*) Cyc_Absyn_Gt){
goto _LL1225;} else{ goto _LL1226;} _LL1226: if( _temp1208 ==( void*) Cyc_Absyn_Lt){
goto _LL1227;} else{ goto _LL1228;} _LL1228: if( _temp1208 ==( void*) Cyc_Absyn_Gte){
goto _LL1229;} else{ goto _LL1230;} _LL1230: if( _temp1208 ==( void*) Cyc_Absyn_Lte){
goto _LL1231;} else{ goto _LL1232;} _LL1232: if( _temp1208 ==( void*) Cyc_Absyn_Not){
goto _LL1233;} else{ goto _LL1234;} _LL1234: if( _temp1208 ==( void*) Cyc_Absyn_Bitnot){
goto _LL1235;} else{ goto _LL1236;} _LL1236: if( _temp1208 ==( void*) Cyc_Absyn_Bitand){
goto _LL1237;} else{ goto _LL1238;} _LL1238: if( _temp1208 ==( void*) Cyc_Absyn_Bitor){
goto _LL1239;} else{ goto _LL1240;} _LL1240: if( _temp1208 ==( void*) Cyc_Absyn_Bitxor){
goto _LL1241;} else{ goto _LL1242;} _LL1242: if( _temp1208 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL1243;} else{ goto _LL1244;} _LL1244: if( _temp1208 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL1245;} else{ goto _LL1246;} _LL1246: if( _temp1208 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL1247;} else{ goto _LL1248;} _LL1248: if( _temp1208 ==( void*) Cyc_Absyn_Size){
goto _LL1249;} else{ goto _LL1250;} _LL1250: if( _temp1208 ==( void*) Cyc_Absyn_Printf){
goto _LL1251;} else{ goto _LL1252;} _LL1252: if( _temp1208 ==( void*) Cyc_Absyn_Fprintf){
goto _LL1253;} else{ goto _LL1254;} _LL1254: if( _temp1208 ==( void*) Cyc_Absyn_Xprintf){
goto _LL1255;} else{ goto _LL1256;} _LL1256: if( _temp1208 ==( void*) Cyc_Absyn_Scanf){
goto _LL1257;} else{ goto _LL1258;} _LL1258: if( _temp1208 ==( void*) Cyc_Absyn_Fscanf){
goto _LL1259;} else{ goto _LL1260;} _LL1260: if( _temp1208 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1261;} else{ goto _LL1209;} _LL1211: return 100; _LL1213: return 110;
_LL1215: return 100; _LL1217: return 110; _LL1219: return 110; _LL1221: return
70; _LL1223: return 70; _LL1225: return 80; _LL1227: return 80; _LL1229: return
80; _LL1231: return 80; _LL1233: return 130; _LL1235: return 130; _LL1237:
return 60; _LL1239: return 40; _LL1241: return 50; _LL1243: return 90; _LL1245:
return 80; _LL1247: return 80; _LL1249: return 140; _LL1251: return 130; _LL1253:
return 130; _LL1255: return 130; _LL1257: return 130; _LL1259: return 130;
_LL1261: return 130; _LL1209:;} _LL1007: return 20; _LL1009: return 130; _LL1011:
return 30; _LL1013: return 10; _LL1015: return 140; _LL1017: return 140; _LL1019:
return 130; _LL1021: return Cyc_Absynpp_exp_prec( _temp1114); _LL1023: return
Cyc_Absynpp_exp_prec( _temp1118); _LL1025: return 120; _LL1027: return 15;
_LL1029: return 130; _LL1031: return 130; _LL1033: return 130; _LL1035: return
130; _LL1037: return 130; _LL1039: return 140; _LL1041: return 140; _LL1043:
return 140; _LL1045: return 150; _LL1047: goto _LL1049; _LL1049: goto _LL1051;
_LL1051: goto _LL1053; _LL1053: goto _LL1055; _LL1055: goto _LL1057; _LL1057:
goto _LL1059; _LL1059: goto _LL1061; _LL1061: return 140; _LL1063: return 140;
_LL1065: return 10000; _LL1067: return 140; _LL1069: return 140; _LL997:;}
struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp* e){ return Cyc_Absynpp_exp2doc_prec(
0, e);} struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec( int inprec, struct Cyc_Absyn_Exp*
e){ int myprec= Cyc_Absynpp_exp_prec( e); struct Cyc_PP_Doc* s;{ void* _temp1262=(
void*) e->r; void* _temp1336; void* _temp1338; struct _tuple0* _temp1340; struct
_tuple0* _temp1342; struct Cyc_List_List* _temp1344; void* _temp1346; struct Cyc_Absyn_Exp*
_temp1348; struct Cyc_Core_Opt* _temp1350; struct Cyc_Absyn_Exp* _temp1352; void*
_temp1354; struct Cyc_Absyn_Exp* _temp1356; struct Cyc_Absyn_Exp* _temp1358;
struct Cyc_Absyn_Exp* _temp1360; struct Cyc_Absyn_Exp* _temp1362; struct Cyc_Absyn_Exp*
_temp1364; struct Cyc_Absyn_Exp* _temp1366; struct Cyc_List_List* _temp1368;
struct Cyc_Absyn_Exp* _temp1370; struct Cyc_Absyn_VarargCallInfo* _temp1372;
struct Cyc_List_List* _temp1374; struct Cyc_Absyn_Exp* _temp1376; struct Cyc_Absyn_Exp*
_temp1378; struct Cyc_Absyn_Exp* _temp1380; struct Cyc_List_List* _temp1382;
struct Cyc_Absyn_Exp* _temp1384; struct Cyc_Absyn_Exp* _temp1386; void*
_temp1388; struct Cyc_Absyn_Exp* _temp1390; struct Cyc_Absyn_Exp* _temp1392;
struct Cyc_Absyn_Exp* _temp1394; void* _temp1396; struct Cyc_Absyn_Exp*
_temp1398; struct _tagged_string* _temp1400; void* _temp1402; struct Cyc_Absyn_Exp*
_temp1404; struct _tagged_string* _temp1406; struct Cyc_Absyn_Exp* _temp1408;
struct _tagged_string* _temp1410; struct Cyc_Absyn_Exp* _temp1412; struct Cyc_Absyn_Exp*
_temp1414; struct Cyc_Absyn_Exp* _temp1416; struct Cyc_List_List* _temp1418;
struct Cyc_List_List* _temp1420; struct _tuple1* _temp1422; struct Cyc_List_List*
_temp1424; struct Cyc_Absyn_Exp* _temp1426; struct Cyc_Absyn_Exp* _temp1428;
struct Cyc_Absyn_Vardecl* _temp1430; struct Cyc_Absyn_Structdecl* _temp1432;
struct Cyc_List_List* _temp1434; struct Cyc_Core_Opt* _temp1436; struct _tuple0*
_temp1438; struct Cyc_List_List* _temp1440; void* _temp1442; struct Cyc_Absyn_Tunionfield*
_temp1444; struct Cyc_Absyn_Tuniondecl* _temp1446; struct Cyc_List_List*
_temp1448; struct Cyc_Core_Opt* _temp1450; struct Cyc_Core_Opt* _temp1452;
struct Cyc_Absyn_Enumfield* _temp1454; struct Cyc_Absyn_Enumdecl* _temp1456;
struct _tuple0* _temp1458; void* _temp1460; struct Cyc_Absyn_Exp* _temp1462;
struct Cyc_List_List* _temp1464; struct Cyc_Core_Opt* _temp1466; struct Cyc_Absyn_Stmt*
_temp1468; struct Cyc_Absyn_Fndecl* _temp1470; struct Cyc_Absyn_Exp* _temp1472;
_LL1264: if(*(( int*) _temp1262) == Cyc_Absyn_Const_e){ _LL1337: _temp1336=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1262)->f1; goto _LL1265;} else{
goto _LL1266;} _LL1266: if(*(( int*) _temp1262) == Cyc_Absyn_Var_e){ _LL1341:
_temp1340=(( struct Cyc_Absyn_Var_e_struct*) _temp1262)->f1; goto _LL1339;
_LL1339: _temp1338=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1262)->f2;
goto _LL1267;} else{ goto _LL1268;} _LL1268: if(*(( int*) _temp1262) == Cyc_Absyn_UnknownId_e){
_LL1343: _temp1342=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1262)->f1; goto
_LL1269;} else{ goto _LL1270;} _LL1270: if(*(( int*) _temp1262) == Cyc_Absyn_Primop_e){
_LL1347: _temp1346=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1262)->f1;
goto _LL1345; _LL1345: _temp1344=(( struct Cyc_Absyn_Primop_e_struct*) _temp1262)->f2;
goto _LL1271;} else{ goto _LL1272;} _LL1272: if(*(( int*) _temp1262) == Cyc_Absyn_AssignOp_e){
_LL1353: _temp1352=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp1262)->f1; goto
_LL1351; _LL1351: _temp1350=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp1262)->f2;
goto _LL1349; _LL1349: _temp1348=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1262)->f3; goto _LL1273;} else{ goto _LL1274;} _LL1274: if(*(( int*)
_temp1262) == Cyc_Absyn_Increment_e){ _LL1357: _temp1356=(( struct Cyc_Absyn_Increment_e_struct*)
_temp1262)->f1; goto _LL1355; _LL1355: _temp1354=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1262)->f2; goto _LL1275;} else{ goto _LL1276;} _LL1276: if(*(( int*)
_temp1262) == Cyc_Absyn_Conditional_e){ _LL1363: _temp1362=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1262)->f1; goto _LL1361; _LL1361: _temp1360=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1262)->f2; goto _LL1359; _LL1359: _temp1358=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1262)->f3; goto _LL1277;} else{ goto _LL1278;} _LL1278: if(*(( int*)
_temp1262) == Cyc_Absyn_SeqExp_e){ _LL1367: _temp1366=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1262)->f1; goto _LL1365; _LL1365: _temp1364=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1262)->f2; goto _LL1279;} else{ goto _LL1280;} _LL1280: if(*(( int*)
_temp1262) == Cyc_Absyn_UnknownCall_e){ _LL1371: _temp1370=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1262)->f1; goto _LL1369; _LL1369: _temp1368=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1262)->f2; goto _LL1281;} else{ goto _LL1282;} _LL1282: if(*(( int*)
_temp1262) == Cyc_Absyn_FnCall_e){ _LL1377: _temp1376=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1262)->f1; goto _LL1375; _LL1375: _temp1374=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1262)->f2; goto _LL1373; _LL1373: _temp1372=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1262)->f3; goto _LL1283;} else{ goto _LL1284;} _LL1284: if(*(( int*)
_temp1262) == Cyc_Absyn_Throw_e){ _LL1379: _temp1378=(( struct Cyc_Absyn_Throw_e_struct*)
_temp1262)->f1; goto _LL1285;} else{ goto _LL1286;} _LL1286: if(*(( int*)
_temp1262) == Cyc_Absyn_NoInstantiate_e){ _LL1381: _temp1380=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1262)->f1; goto _LL1287;} else{ goto _LL1288;} _LL1288: if(*(( int*)
_temp1262) == Cyc_Absyn_Instantiate_e){ _LL1385: _temp1384=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1262)->f1; goto _LL1383; _LL1383: _temp1382=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1262)->f2; goto _LL1289;} else{ goto _LL1290;} _LL1290: if(*(( int*)
_temp1262) == Cyc_Absyn_Cast_e){ _LL1389: _temp1388=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1262)->f1; goto _LL1387; _LL1387: _temp1386=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1262)->f2; goto _LL1291;} else{ goto _LL1292;} _LL1292: if(*(( int*)
_temp1262) == Cyc_Absyn_Address_e){ _LL1391: _temp1390=(( struct Cyc_Absyn_Address_e_struct*)
_temp1262)->f1; goto _LL1293;} else{ goto _LL1294;} _LL1294: if(*(( int*)
_temp1262) == Cyc_Absyn_New_e){ _LL1395: _temp1394=(( struct Cyc_Absyn_New_e_struct*)
_temp1262)->f1; goto _LL1393; _LL1393: _temp1392=(( struct Cyc_Absyn_New_e_struct*)
_temp1262)->f2; goto _LL1295;} else{ goto _LL1296;} _LL1296: if(*(( int*)
_temp1262) == Cyc_Absyn_Sizeoftyp_e){ _LL1397: _temp1396=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1262)->f1; goto _LL1297;} else{ goto _LL1298;} _LL1298: if(*(( int*)
_temp1262) == Cyc_Absyn_Sizeofexp_e){ _LL1399: _temp1398=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1262)->f1; goto _LL1299;} else{ goto _LL1300;} _LL1300: if(*(( int*)
_temp1262) == Cyc_Absyn_Offsetof_e){ _LL1403: _temp1402=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1262)->f1; goto _LL1401; _LL1401: _temp1400=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1262)->f2; goto _LL1301;} else{ goto _LL1302;} _LL1302: if(*(( int*)
_temp1262) == Cyc_Absyn_Deref_e){ _LL1405: _temp1404=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1262)->f1; goto _LL1303;} else{ goto _LL1304;} _LL1304: if(*(( int*)
_temp1262) == Cyc_Absyn_StructMember_e){ _LL1409: _temp1408=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1262)->f1; goto _LL1407; _LL1407: _temp1406=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1262)->f2; goto _LL1305;} else{ goto _LL1306;} _LL1306: if(*(( int*)
_temp1262) == Cyc_Absyn_StructArrow_e){ _LL1413: _temp1412=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1262)->f1; goto _LL1411; _LL1411: _temp1410=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1262)->f2; goto _LL1307;} else{ goto _LL1308;} _LL1308: if(*(( int*)
_temp1262) == Cyc_Absyn_Subscript_e){ _LL1417: _temp1416=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1262)->f1; goto _LL1415; _LL1415: _temp1414=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1262)->f2; goto _LL1309;} else{ goto _LL1310;} _LL1310: if(*(( int*)
_temp1262) == Cyc_Absyn_Tuple_e){ _LL1419: _temp1418=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1262)->f1; goto _LL1311;} else{ goto _LL1312;} _LL1312: if(*(( int*)
_temp1262) == Cyc_Absyn_CompoundLit_e){ _LL1423: _temp1422=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1262)->f1; goto _LL1421; _LL1421: _temp1420=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1262)->f2; goto _LL1313;} else{ goto _LL1314;} _LL1314: if(*(( int*)
_temp1262) == Cyc_Absyn_Array_e){ _LL1425: _temp1424=(( struct Cyc_Absyn_Array_e_struct*)
_temp1262)->f1; goto _LL1315;} else{ goto _LL1316;} _LL1316: if(*(( int*)
_temp1262) == Cyc_Absyn_Comprehension_e){ _LL1431: _temp1430=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1262)->f1; goto _LL1429; _LL1429: _temp1428=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1262)->f2; goto _LL1427; _LL1427: _temp1426=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1262)->f3; goto _LL1317;} else{ goto _LL1318;} _LL1318: if(*(( int*)
_temp1262) == Cyc_Absyn_Struct_e){ _LL1439: _temp1438=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1262)->f1; goto _LL1437; _LL1437: _temp1436=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1262)->f2; goto _LL1435; _LL1435: _temp1434=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1262)->f3; goto _LL1433; _LL1433: _temp1432=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1262)->f4; goto _LL1319;} else{ goto _LL1320;} _LL1320: if(*(( int*)
_temp1262) == Cyc_Absyn_AnonStruct_e){ _LL1443: _temp1442=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1262)->f1; goto _LL1441; _LL1441: _temp1440=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1262)->f2; goto _LL1321;} else{ goto _LL1322;} _LL1322: if(*(( int*)
_temp1262) == Cyc_Absyn_Tunion_e){ _LL1453: _temp1452=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1262)->f1; goto _LL1451; _LL1451: _temp1450=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1262)->f2; goto _LL1449; _LL1449: _temp1448=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1262)->f3; goto _LL1447; _LL1447: _temp1446=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1262)->f4; goto _LL1445; _LL1445: _temp1444=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1262)->f5; goto _LL1323;} else{ goto _LL1324;} _LL1324: if(*(( int*)
_temp1262) == Cyc_Absyn_Enum_e){ _LL1459: _temp1458=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1262)->f1; goto _LL1457; _LL1457: _temp1456=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1262)->f2; goto _LL1455; _LL1455: _temp1454=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1262)->f3; goto _LL1325;} else{ goto _LL1326;} _LL1326: if(*(( int*)
_temp1262) == Cyc_Absyn_Malloc_e){ _LL1463: _temp1462=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1262)->f1; goto _LL1461; _LL1461: _temp1460=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1262)->f2; goto _LL1327;} else{ goto _LL1328;} _LL1328: if(*(( int*)
_temp1262) == Cyc_Absyn_UnresolvedMem_e){ _LL1467: _temp1466=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1262)->f1; goto _LL1465; _LL1465: _temp1464=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1262)->f2; goto _LL1329;} else{ goto _LL1330;} _LL1330: if(*(( int*)
_temp1262) == Cyc_Absyn_StmtExp_e){ _LL1469: _temp1468=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1262)->f1; goto _LL1331;} else{ goto _LL1332;} _LL1332: if(*(( int*)
_temp1262) == Cyc_Absyn_Codegen_e){ _LL1471: _temp1470=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1262)->f1; goto _LL1333;} else{ goto _LL1334;} _LL1334: if(*(( int*)
_temp1262) == Cyc_Absyn_Fill_e){ _LL1473: _temp1472=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1262)->f1; goto _LL1335;} else{ goto _LL1263;} _LL1265: s= Cyc_Absynpp_cnst2doc(
_temp1336); goto _LL1263; _LL1267: _temp1342= _temp1340; goto _LL1269; _LL1269:
s= Cyc_Absynpp_qvar2doc( _temp1342); goto _LL1263; _LL1271: s= Cyc_Absynpp_primapp2doc(
myprec, _temp1346, _temp1344); goto _LL1263; _LL1273: s=({ struct Cyc_PP_Doc*(*
_temp1474)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1477= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1352); struct Cyc_PP_Doc* _temp1478= Cyc_PP_text(({ unsigned char*
_temp1486=( unsigned char*)" "; struct _tagged_string _temp1487; _temp1487.curr=
_temp1486; _temp1487.base= _temp1486; _temp1487.last_plus_one= _temp1486 + 2;
_temp1487;})); struct Cyc_PP_Doc* _temp1479= _temp1350 == 0? Cyc_PP_text(({
unsigned char* _temp1484=( unsigned char*)""; struct _tagged_string _temp1485;
_temp1485.curr= _temp1484; _temp1485.base= _temp1484; _temp1485.last_plus_one=
_temp1484 + 1; _temp1485;})): Cyc_Absynpp_prim2doc(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1350))->v); struct Cyc_PP_Doc* _temp1480= Cyc_PP_text(({
unsigned char* _temp1482=( unsigned char*)"= "; struct _tagged_string _temp1483;
_temp1483.curr= _temp1482; _temp1483.base= _temp1482; _temp1483.last_plus_one=
_temp1482 + 3; _temp1483;})); struct Cyc_PP_Doc* _temp1481= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1348); struct Cyc_PP_Doc* _temp1475[ 5u]={ _temp1477, _temp1478,
_temp1479, _temp1480, _temp1481}; struct _tagged_ptr1 _temp1476={ _temp1475,
_temp1475, _temp1475 + 5u}; _temp1474( _temp1476);}); goto _LL1263; _LL1275: {
struct Cyc_PP_Doc* _temp1488= Cyc_Absynpp_exp2doc_prec( myprec, _temp1356); goto
_LL1489; _LL1489:{ void* _temp1490= _temp1354; _LL1492: if( _temp1490 ==( void*)
Cyc_Absyn_PreInc){ goto _LL1493;} else{ goto _LL1494;} _LL1494: if( _temp1490 ==(
void*) Cyc_Absyn_PreDec){ goto _LL1495;} else{ goto _LL1496;} _LL1496: if(
_temp1490 ==( void*) Cyc_Absyn_PostInc){ goto _LL1497;} else{ goto _LL1498;}
_LL1498: if( _temp1490 ==( void*) Cyc_Absyn_PostDec){ goto _LL1499;} else{ goto
_LL1491;} _LL1493: s=({ struct Cyc_PP_Doc*(* _temp1500)( struct _tagged_ptr1)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1503= Cyc_PP_text(({ unsigned char*
_temp1505=( unsigned char*)"++"; struct _tagged_string _temp1506; _temp1506.curr=
_temp1505; _temp1506.base= _temp1505; _temp1506.last_plus_one= _temp1505 + 3;
_temp1506;})); struct Cyc_PP_Doc* _temp1504= _temp1488; struct Cyc_PP_Doc*
_temp1501[ 2u]={ _temp1503, _temp1504}; struct _tagged_ptr1 _temp1502={
_temp1501, _temp1501, _temp1501 + 2u}; _temp1500( _temp1502);}); goto _LL1491;
_LL1495: s=({ struct Cyc_PP_Doc*(* _temp1507)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1510= Cyc_PP_text(({ unsigned char* _temp1512=(
unsigned char*)"--"; struct _tagged_string _temp1513; _temp1513.curr= _temp1512;
_temp1513.base= _temp1512; _temp1513.last_plus_one= _temp1512 + 3; _temp1513;}));
struct Cyc_PP_Doc* _temp1511= _temp1488; struct Cyc_PP_Doc* _temp1508[ 2u]={
_temp1510, _temp1511}; struct _tagged_ptr1 _temp1509={ _temp1508, _temp1508,
_temp1508 + 2u}; _temp1507( _temp1509);}); goto _LL1491; _LL1497: s=({ struct
Cyc_PP_Doc*(* _temp1514)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1517= _temp1488; struct Cyc_PP_Doc* _temp1518= Cyc_PP_text(({ unsigned char*
_temp1519=( unsigned char*)"++"; struct _tagged_string _temp1520; _temp1520.curr=
_temp1519; _temp1520.base= _temp1519; _temp1520.last_plus_one= _temp1519 + 3;
_temp1520;})); struct Cyc_PP_Doc* _temp1515[ 2u]={ _temp1517, _temp1518}; struct
_tagged_ptr1 _temp1516={ _temp1515, _temp1515, _temp1515 + 2u}; _temp1514(
_temp1516);}); goto _LL1491; _LL1499: s=({ struct Cyc_PP_Doc*(* _temp1521)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1524= _temp1488;
struct Cyc_PP_Doc* _temp1525= Cyc_PP_text(({ unsigned char* _temp1526=(
unsigned char*)"--"; struct _tagged_string _temp1527; _temp1527.curr= _temp1526;
_temp1527.base= _temp1526; _temp1527.last_plus_one= _temp1526 + 3; _temp1527;}));
struct Cyc_PP_Doc* _temp1522[ 2u]={ _temp1524, _temp1525}; struct _tagged_ptr1
_temp1523={ _temp1522, _temp1522, _temp1522 + 2u}; _temp1521( _temp1523);});
goto _LL1491; _LL1491:;} goto _LL1263;} _LL1277:{ struct _tuple8 _temp1529=({
struct _tuple8 _temp1528; _temp1528.f1=( void*) _temp1360->r; _temp1528.f2=(
void*) _temp1358->r; _temp1528;}); _LL1531: goto _LL1532; _LL1532: s=({ struct
Cyc_PP_Doc*(* _temp1533)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1536= Cyc_Absynpp_exp2doc_prec( myprec, _temp1362); struct Cyc_PP_Doc*
_temp1537= Cyc_PP_text(({ unsigned char* _temp1543=( unsigned char*)" ? ";
struct _tagged_string _temp1544; _temp1544.curr= _temp1543; _temp1544.base=
_temp1543; _temp1544.last_plus_one= _temp1543 + 4; _temp1544;})); struct Cyc_PP_Doc*
_temp1538= Cyc_Absynpp_exp2doc_prec( 0, _temp1360); struct Cyc_PP_Doc* _temp1539=
Cyc_PP_text(({ unsigned char* _temp1541=( unsigned char*)" : "; struct
_tagged_string _temp1542; _temp1542.curr= _temp1541; _temp1542.base= _temp1541;
_temp1542.last_plus_one= _temp1541 + 4; _temp1542;})); struct Cyc_PP_Doc*
_temp1540= Cyc_Absynpp_exp2doc_prec( myprec, _temp1358); struct Cyc_PP_Doc*
_temp1534[ 5u]={ _temp1536, _temp1537, _temp1538, _temp1539, _temp1540}; struct
_tagged_ptr1 _temp1535={ _temp1534, _temp1534, _temp1534 + 5u}; _temp1533(
_temp1535);}); goto _LL1530; _LL1530:;} goto _LL1263; _LL1279: s=({ struct Cyc_PP_Doc*(*
_temp1545)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1548= Cyc_PP_text(({
unsigned char* _temp1557=( unsigned char*)"("; struct _tagged_string _temp1558;
_temp1558.curr= _temp1557; _temp1558.base= _temp1557; _temp1558.last_plus_one=
_temp1557 + 2; _temp1558;})); struct Cyc_PP_Doc* _temp1549= Cyc_Absynpp_exp2doc(
_temp1366); struct Cyc_PP_Doc* _temp1550= Cyc_PP_text(({ unsigned char*
_temp1555=( unsigned char*)", "; struct _tagged_string _temp1556; _temp1556.curr=
_temp1555; _temp1556.base= _temp1555; _temp1556.last_plus_one= _temp1555 + 3;
_temp1556;})); struct Cyc_PP_Doc* _temp1551= Cyc_Absynpp_exp2doc( _temp1364);
struct Cyc_PP_Doc* _temp1552= Cyc_PP_text(({ unsigned char* _temp1553=(
unsigned char*)")"; struct _tagged_string _temp1554; _temp1554.curr= _temp1553;
_temp1554.base= _temp1553; _temp1554.last_plus_one= _temp1553 + 2; _temp1554;}));
struct Cyc_PP_Doc* _temp1546[ 5u]={ _temp1548, _temp1549, _temp1550, _temp1551,
_temp1552}; struct _tagged_ptr1 _temp1547={ _temp1546, _temp1546, _temp1546 + 5u};
_temp1545( _temp1547);}); goto _LL1263; _LL1281: s=({ struct Cyc_PP_Doc*(*
_temp1559)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1562= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1370); struct Cyc_PP_Doc* _temp1563= Cyc_PP_text(({ unsigned char*
_temp1568=( unsigned char*)"("; struct _tagged_string _temp1569; _temp1569.curr=
_temp1568; _temp1569.base= _temp1568; _temp1569.last_plus_one= _temp1568 + 2;
_temp1569;})); struct Cyc_PP_Doc* _temp1564= Cyc_Absynpp_exps2doc_prec( 20,
_temp1368); struct Cyc_PP_Doc* _temp1565= Cyc_PP_text(({ unsigned char*
_temp1566=( unsigned char*)")"; struct _tagged_string _temp1567; _temp1567.curr=
_temp1566; _temp1567.base= _temp1566; _temp1567.last_plus_one= _temp1566 + 2;
_temp1567;})); struct Cyc_PP_Doc* _temp1560[ 4u]={ _temp1562, _temp1563,
_temp1564, _temp1565}; struct _tagged_ptr1 _temp1561={ _temp1560, _temp1560,
_temp1560 + 4u}; _temp1559( _temp1561);}); goto _LL1263; _LL1283: s=({ struct
Cyc_PP_Doc*(* _temp1570)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1573= Cyc_Absynpp_exp2doc_prec( myprec, _temp1376); struct Cyc_PP_Doc*
_temp1574= Cyc_PP_text(({ unsigned char* _temp1579=( unsigned char*)"("; struct
_tagged_string _temp1580; _temp1580.curr= _temp1579; _temp1580.base= _temp1579;
_temp1580.last_plus_one= _temp1579 + 2; _temp1580;})); struct Cyc_PP_Doc*
_temp1575= Cyc_Absynpp_exps2doc_prec( 20, _temp1374); struct Cyc_PP_Doc*
_temp1576= Cyc_PP_text(({ unsigned char* _temp1577=( unsigned char*)")"; struct
_tagged_string _temp1578; _temp1578.curr= _temp1577; _temp1578.base= _temp1577;
_temp1578.last_plus_one= _temp1577 + 2; _temp1578;})); struct Cyc_PP_Doc*
_temp1571[ 4u]={ _temp1573, _temp1574, _temp1575, _temp1576}; struct
_tagged_ptr1 _temp1572={ _temp1571, _temp1571, _temp1571 + 4u}; _temp1570(
_temp1572);}); goto _LL1263; _LL1285: s=({ struct Cyc_PP_Doc*(* _temp1581)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1584= Cyc_PP_text(({
unsigned char* _temp1586=( unsigned char*)"throw "; struct _tagged_string
_temp1587; _temp1587.curr= _temp1586; _temp1587.base= _temp1586; _temp1587.last_plus_one=
_temp1586 + 7; _temp1587;})); struct Cyc_PP_Doc* _temp1585= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1378); struct Cyc_PP_Doc* _temp1582[ 2u]={ _temp1584, _temp1585};
struct _tagged_ptr1 _temp1583={ _temp1582, _temp1582, _temp1582 + 2u}; _temp1581(
_temp1583);}); goto _LL1263; _LL1287: s= Cyc_Absynpp_exp2doc_prec( inprec,
_temp1380); goto _LL1263; _LL1289: s= Cyc_Absynpp_exp2doc_prec( inprec,
_temp1384); goto _LL1263; _LL1291: s=({ struct Cyc_PP_Doc*(* _temp1588)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1591= Cyc_PP_text(({
unsigned char* _temp1597=( unsigned char*)"("; struct _tagged_string _temp1598;
_temp1598.curr= _temp1597; _temp1598.base= _temp1597; _temp1598.last_plus_one=
_temp1597 + 2; _temp1598;})); struct Cyc_PP_Doc* _temp1592= Cyc_Absynpp_typ2doc(
_temp1388); struct Cyc_PP_Doc* _temp1593= Cyc_PP_text(({ unsigned char*
_temp1595=( unsigned char*)")"; struct _tagged_string _temp1596; _temp1596.curr=
_temp1595; _temp1596.base= _temp1595; _temp1596.last_plus_one= _temp1595 + 2;
_temp1596;})); struct Cyc_PP_Doc* _temp1594= Cyc_Absynpp_exp2doc_prec( myprec,
_temp1386); struct Cyc_PP_Doc* _temp1589[ 4u]={ _temp1591, _temp1592, _temp1593,
_temp1594}; struct _tagged_ptr1 _temp1590={ _temp1589, _temp1589, _temp1589 + 4u};
_temp1588( _temp1590);}); goto _LL1263; _LL1293: s=({ struct Cyc_PP_Doc*(*
_temp1599)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1602= Cyc_PP_text(({
unsigned char* _temp1604=( unsigned char*)"&"; struct _tagged_string _temp1605;
_temp1605.curr= _temp1604; _temp1605.base= _temp1604; _temp1605.last_plus_one=
_temp1604 + 2; _temp1605;})); struct Cyc_PP_Doc* _temp1603= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1390); struct Cyc_PP_Doc* _temp1600[ 2u]={ _temp1602, _temp1603};
struct _tagged_ptr1 _temp1601={ _temp1600, _temp1600, _temp1600 + 2u}; _temp1599(
_temp1601);}); goto _LL1263; _LL1295: if( _temp1394 == 0){ s=({ struct Cyc_PP_Doc*(*
_temp1606)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1609= Cyc_PP_text(({
unsigned char* _temp1611=( unsigned char*)"new "; struct _tagged_string
_temp1612; _temp1612.curr= _temp1611; _temp1612.base= _temp1611; _temp1612.last_plus_one=
_temp1611 + 5; _temp1612;})); struct Cyc_PP_Doc* _temp1610= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1392); struct Cyc_PP_Doc* _temp1607[ 2u]={ _temp1609, _temp1610};
struct _tagged_ptr1 _temp1608={ _temp1607, _temp1607, _temp1607 + 2u}; _temp1606(
_temp1608);});} else{ s=({ struct Cyc_PP_Doc*(* _temp1613)( struct _tagged_ptr1)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1616= Cyc_PP_text(({ unsigned char*
_temp1622=( unsigned char*)"rnew("; struct _tagged_string _temp1623; _temp1623.curr=
_temp1622; _temp1623.base= _temp1622; _temp1623.last_plus_one= _temp1622 + 6;
_temp1623;})); struct Cyc_PP_Doc* _temp1617= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)
_check_null( _temp1394)); struct Cyc_PP_Doc* _temp1618= Cyc_PP_text(({
unsigned char* _temp1620=( unsigned char*)") "; struct _tagged_string _temp1621;
_temp1621.curr= _temp1620; _temp1621.base= _temp1620; _temp1621.last_plus_one=
_temp1620 + 3; _temp1621;})); struct Cyc_PP_Doc* _temp1619= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1392); struct Cyc_PP_Doc* _temp1614[ 4u]={ _temp1616, _temp1617,
_temp1618, _temp1619}; struct _tagged_ptr1 _temp1615={ _temp1614, _temp1614,
_temp1614 + 4u}; _temp1613( _temp1615);});} goto _LL1263; _LL1297: s=({ struct
Cyc_PP_Doc*(* _temp1624)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1627= Cyc_PP_text(({ unsigned char* _temp1632=( unsigned char*)"sizeof(";
struct _tagged_string _temp1633; _temp1633.curr= _temp1632; _temp1633.base=
_temp1632; _temp1633.last_plus_one= _temp1632 + 8; _temp1633;})); struct Cyc_PP_Doc*
_temp1628= Cyc_Absynpp_typ2doc( _temp1396); struct Cyc_PP_Doc* _temp1629= Cyc_PP_text(({
unsigned char* _temp1630=( unsigned char*)")"; struct _tagged_string _temp1631;
_temp1631.curr= _temp1630; _temp1631.base= _temp1630; _temp1631.last_plus_one=
_temp1630 + 2; _temp1631;})); struct Cyc_PP_Doc* _temp1625[ 3u]={ _temp1627,
_temp1628, _temp1629}; struct _tagged_ptr1 _temp1626={ _temp1625, _temp1625,
_temp1625 + 3u}; _temp1624( _temp1626);}); goto _LL1263; _LL1299: s=({ struct
Cyc_PP_Doc*(* _temp1634)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1637= Cyc_PP_text(({ unsigned char* _temp1642=( unsigned char*)"sizeof(";
struct _tagged_string _temp1643; _temp1643.curr= _temp1642; _temp1643.base=
_temp1642; _temp1643.last_plus_one= _temp1642 + 8; _temp1643;})); struct Cyc_PP_Doc*
_temp1638= Cyc_Absynpp_exp2doc( _temp1398); struct Cyc_PP_Doc* _temp1639= Cyc_PP_text(({
unsigned char* _temp1640=( unsigned char*)")"; struct _tagged_string _temp1641;
_temp1641.curr= _temp1640; _temp1641.base= _temp1640; _temp1641.last_plus_one=
_temp1640 + 2; _temp1641;})); struct Cyc_PP_Doc* _temp1635[ 3u]={ _temp1637,
_temp1638, _temp1639}; struct _tagged_ptr1 _temp1636={ _temp1635, _temp1635,
_temp1635 + 3u}; _temp1634( _temp1636);}); goto _LL1263; _LL1301: s=({ struct
Cyc_PP_Doc*(* _temp1644)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1647= Cyc_PP_text(({ unsigned char* _temp1656=( unsigned char*)"offsetof(";
struct _tagged_string _temp1657; _temp1657.curr= _temp1656; _temp1657.base=
_temp1656; _temp1657.last_plus_one= _temp1656 + 10; _temp1657;})); struct Cyc_PP_Doc*
_temp1648= Cyc_Absynpp_typ2doc( _temp1402); struct Cyc_PP_Doc* _temp1649= Cyc_PP_text(({
unsigned char* _temp1654=( unsigned char*)","; struct _tagged_string _temp1655;
_temp1655.curr= _temp1654; _temp1655.base= _temp1654; _temp1655.last_plus_one=
_temp1654 + 2; _temp1655;})); struct Cyc_PP_Doc* _temp1650= Cyc_PP_textptr(
_temp1400); struct Cyc_PP_Doc* _temp1651= Cyc_PP_text(({ unsigned char*
_temp1652=( unsigned char*)")"; struct _tagged_string _temp1653; _temp1653.curr=
_temp1652; _temp1653.base= _temp1652; _temp1653.last_plus_one= _temp1652 + 2;
_temp1653;})); struct Cyc_PP_Doc* _temp1645[ 5u]={ _temp1647, _temp1648,
_temp1649, _temp1650, _temp1651}; struct _tagged_ptr1 _temp1646={ _temp1645,
_temp1645, _temp1645 + 5u}; _temp1644( _temp1646);}); goto _LL1263; _LL1303: s=({
struct Cyc_PP_Doc*(* _temp1658)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1661= Cyc_PP_text(({ unsigned char* _temp1663=( unsigned char*)"*"; struct
_tagged_string _temp1664; _temp1664.curr= _temp1663; _temp1664.base= _temp1663;
_temp1664.last_plus_one= _temp1663 + 2; _temp1664;})); struct Cyc_PP_Doc*
_temp1662= Cyc_Absynpp_exp2doc_prec( myprec, _temp1404); struct Cyc_PP_Doc*
_temp1659[ 2u]={ _temp1661, _temp1662}; struct _tagged_ptr1 _temp1660={
_temp1659, _temp1659, _temp1659 + 2u}; _temp1658( _temp1660);}); goto _LL1263;
_LL1305: s=({ struct Cyc_PP_Doc*(* _temp1665)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1668= Cyc_Absynpp_exp2doc_prec( myprec, _temp1408);
struct Cyc_PP_Doc* _temp1669= Cyc_PP_text(({ unsigned char* _temp1671=(
unsigned char*)"."; struct _tagged_string _temp1672; _temp1672.curr= _temp1671;
_temp1672.base= _temp1671; _temp1672.last_plus_one= _temp1671 + 2; _temp1672;}));
struct Cyc_PP_Doc* _temp1670= Cyc_PP_textptr( _temp1406); struct Cyc_PP_Doc*
_temp1666[ 3u]={ _temp1668, _temp1669, _temp1670}; struct _tagged_ptr1 _temp1667={
_temp1666, _temp1666, _temp1666 + 3u}; _temp1665( _temp1667);}); goto _LL1263;
_LL1307: s=({ struct Cyc_PP_Doc*(* _temp1673)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1676= Cyc_Absynpp_exp2doc_prec( myprec, _temp1412);
struct Cyc_PP_Doc* _temp1677= Cyc_PP_text(({ unsigned char* _temp1679=(
unsigned char*)"->"; struct _tagged_string _temp1680; _temp1680.curr= _temp1679;
_temp1680.base= _temp1679; _temp1680.last_plus_one= _temp1679 + 3; _temp1680;}));
struct Cyc_PP_Doc* _temp1678= Cyc_PP_textptr( _temp1410); struct Cyc_PP_Doc*
_temp1674[ 3u]={ _temp1676, _temp1677, _temp1678}; struct _tagged_ptr1 _temp1675={
_temp1674, _temp1674, _temp1674 + 3u}; _temp1673( _temp1675);}); goto _LL1263;
_LL1309: s=({ struct Cyc_PP_Doc*(* _temp1681)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1684= Cyc_Absynpp_exp2doc_prec( myprec, _temp1416);
struct Cyc_PP_Doc* _temp1685= Cyc_PP_text(({ unsigned char* _temp1690=(
unsigned char*)"["; struct _tagged_string _temp1691; _temp1691.curr= _temp1690;
_temp1691.base= _temp1690; _temp1691.last_plus_one= _temp1690 + 2; _temp1691;}));
struct Cyc_PP_Doc* _temp1686= Cyc_Absynpp_exp2doc( _temp1414); struct Cyc_PP_Doc*
_temp1687= Cyc_PP_text(({ unsigned char* _temp1688=( unsigned char*)"]"; struct
_tagged_string _temp1689; _temp1689.curr= _temp1688; _temp1689.base= _temp1688;
_temp1689.last_plus_one= _temp1688 + 2; _temp1689;})); struct Cyc_PP_Doc*
_temp1682[ 4u]={ _temp1684, _temp1685, _temp1686, _temp1687}; struct
_tagged_ptr1 _temp1683={ _temp1682, _temp1682, _temp1682 + 4u}; _temp1681(
_temp1683);}); goto _LL1263; _LL1311: s=({ struct Cyc_PP_Doc*(* _temp1692)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1695= Cyc_PP_text(({
unsigned char* _temp1700=( unsigned char*)"$("; struct _tagged_string _temp1701;
_temp1701.curr= _temp1700; _temp1701.base= _temp1700; _temp1701.last_plus_one=
_temp1700 + 3; _temp1701;})); struct Cyc_PP_Doc* _temp1696= Cyc_Absynpp_exps2doc_prec(
20, _temp1418); struct Cyc_PP_Doc* _temp1697= Cyc_PP_text(({ unsigned char*
_temp1698=( unsigned char*)")"; struct _tagged_string _temp1699; _temp1699.curr=
_temp1698; _temp1699.base= _temp1698; _temp1699.last_plus_one= _temp1698 + 2;
_temp1699;})); struct Cyc_PP_Doc* _temp1693[ 3u]={ _temp1695, _temp1696,
_temp1697}; struct _tagged_ptr1 _temp1694={ _temp1693, _temp1693, _temp1693 + 3u};
_temp1692( _temp1694);}); goto _LL1263; _LL1313: s=({ struct Cyc_PP_Doc*(*
_temp1702)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1705= Cyc_PP_text(({
unsigned char* _temp1717=( unsigned char*)"("; struct _tagged_string _temp1718;
_temp1718.curr= _temp1717; _temp1718.base= _temp1717; _temp1718.last_plus_one=
_temp1717 + 2; _temp1718;})); struct Cyc_PP_Doc* _temp1706= Cyc_Absynpp_typ2doc((*
_temp1422).f3); struct Cyc_PP_Doc* _temp1707= Cyc_PP_text(({ unsigned char*
_temp1715=( unsigned char*)")"; struct _tagged_string _temp1716; _temp1716.curr=
_temp1715; _temp1716.base= _temp1715; _temp1716.last_plus_one= _temp1715 + 2;
_temp1716;})); struct Cyc_PP_Doc* _temp1708= Cyc_PP_group(({ unsigned char*
_temp1709=( unsigned char*)"{"; struct _tagged_string _temp1710; _temp1710.curr=
_temp1709; _temp1710.base= _temp1709; _temp1710.last_plus_one= _temp1709 + 2;
_temp1710;}),({ unsigned char* _temp1711=( unsigned char*)"}"; struct
_tagged_string _temp1712; _temp1712.curr= _temp1711; _temp1712.base= _temp1711;
_temp1712.last_plus_one= _temp1711 + 2; _temp1712;}),({ unsigned char* _temp1713=(
unsigned char*)","; struct _tagged_string _temp1714; _temp1714.curr= _temp1713;
_temp1714.base= _temp1713; _temp1714.last_plus_one= _temp1713 + 2; _temp1714;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_de2doc, _temp1420)); struct Cyc_PP_Doc* _temp1703[
4u]={ _temp1705, _temp1706, _temp1707, _temp1708}; struct _tagged_ptr1 _temp1704={
_temp1703, _temp1703, _temp1703 + 4u}; _temp1702( _temp1704);}); goto _LL1263;
_LL1315: s= Cyc_PP_group(({ unsigned char* _temp1719=( unsigned char*)"{";
struct _tagged_string _temp1720; _temp1720.curr= _temp1719; _temp1720.base=
_temp1719; _temp1720.last_plus_one= _temp1719 + 2; _temp1720;}),({ unsigned char*
_temp1721=( unsigned char*)"}"; struct _tagged_string _temp1722; _temp1722.curr=
_temp1721; _temp1722.base= _temp1721; _temp1722.last_plus_one= _temp1721 + 2;
_temp1722;}),({ unsigned char* _temp1723=( unsigned char*)","; struct
_tagged_string _temp1724; _temp1724.curr= _temp1723; _temp1724.base= _temp1723;
_temp1724.last_plus_one= _temp1723 + 2; _temp1724;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp1424)); goto _LL1263; _LL1317: s=({ struct Cyc_PP_Doc*(*
_temp1725)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1728= Cyc_PP_text(({
unsigned char* _temp1741=( unsigned char*)"{for "; struct _tagged_string
_temp1742; _temp1742.curr= _temp1741; _temp1742.base= _temp1741; _temp1742.last_plus_one=
_temp1741 + 6; _temp1742;})); struct Cyc_PP_Doc* _temp1729= Cyc_PP_text(*(*
_temp1430->name).f2); struct Cyc_PP_Doc* _temp1730= Cyc_PP_text(({ unsigned char*
_temp1739=( unsigned char*)" < "; struct _tagged_string _temp1740; _temp1740.curr=
_temp1739; _temp1740.base= _temp1739; _temp1740.last_plus_one= _temp1739 + 4;
_temp1740;})); struct Cyc_PP_Doc* _temp1731= Cyc_Absynpp_exp2doc( _temp1428);
struct Cyc_PP_Doc* _temp1732= Cyc_PP_text(({ unsigned char* _temp1737=(
unsigned char*)" : "; struct _tagged_string _temp1738; _temp1738.curr= _temp1737;
_temp1738.base= _temp1737; _temp1738.last_plus_one= _temp1737 + 4; _temp1738;}));
struct Cyc_PP_Doc* _temp1733= Cyc_Absynpp_exp2doc( _temp1426); struct Cyc_PP_Doc*
_temp1734= Cyc_PP_text(({ unsigned char* _temp1735=( unsigned char*)"}"; struct
_tagged_string _temp1736; _temp1736.curr= _temp1735; _temp1736.base= _temp1735;
_temp1736.last_plus_one= _temp1735 + 2; _temp1736;})); struct Cyc_PP_Doc*
_temp1726[ 7u]={ _temp1728, _temp1729, _temp1730, _temp1731, _temp1732,
_temp1733, _temp1734}; struct _tagged_ptr1 _temp1727={ _temp1726, _temp1726,
_temp1726 + 7u}; _temp1725( _temp1727);}); goto _LL1263; _LL1319: s=({ struct
Cyc_PP_Doc*(* _temp1743)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1746= Cyc_Absynpp_qvar2doc( _temp1438); struct Cyc_PP_Doc* _temp1747= Cyc_PP_group(({
unsigned char* _temp1748=( unsigned char*)"{"; struct _tagged_string _temp1749;
_temp1749.curr= _temp1748; _temp1749.base= _temp1748; _temp1749.last_plus_one=
_temp1748 + 2; _temp1749;}),({ unsigned char* _temp1750=( unsigned char*)"}";
struct _tagged_string _temp1751; _temp1751.curr= _temp1750; _temp1751.base=
_temp1750; _temp1751.last_plus_one= _temp1750 + 2; _temp1751;}),({ unsigned char*
_temp1752=( unsigned char*)","; struct _tagged_string _temp1753; _temp1753.curr=
_temp1752; _temp1753.base= _temp1752; _temp1753.last_plus_one= _temp1752 + 2;
_temp1753;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1434)); struct Cyc_PP_Doc* _temp1744[ 2u]={ _temp1746, _temp1747}; struct
_tagged_ptr1 _temp1745={ _temp1744, _temp1744, _temp1744 + 2u}; _temp1743(
_temp1745);}); goto _LL1263; _LL1321: s= Cyc_PP_group(({ unsigned char*
_temp1754=( unsigned char*)"{"; struct _tagged_string _temp1755; _temp1755.curr=
_temp1754; _temp1755.base= _temp1754; _temp1755.last_plus_one= _temp1754 + 2;
_temp1755;}),({ unsigned char* _temp1756=( unsigned char*)"}"; struct
_tagged_string _temp1757; _temp1757.curr= _temp1756; _temp1757.base= _temp1756;
_temp1757.last_plus_one= _temp1756 + 2; _temp1757;}),({ unsigned char* _temp1758=(
unsigned char*)","; struct _tagged_string _temp1759; _temp1759.curr= _temp1758;
_temp1759.base= _temp1758; _temp1759.last_plus_one= _temp1758 + 2; _temp1759;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_de2doc, _temp1440)); goto _LL1263; _LL1323: if(
_temp1448 == 0){ s= Cyc_Absynpp_qvar2doc( _temp1444->name);} else{ s=({ struct
Cyc_PP_Doc*(* _temp1760)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1763= Cyc_Absynpp_qvar2doc( _temp1444->name); struct Cyc_PP_Doc* _temp1764=
Cyc_PP_egroup(({ unsigned char* _temp1765=( unsigned char*)"("; struct
_tagged_string _temp1766; _temp1766.curr= _temp1765; _temp1766.base= _temp1765;
_temp1766.last_plus_one= _temp1765 + 2; _temp1766;}),({ unsigned char* _temp1767=(
unsigned char*)")"; struct _tagged_string _temp1768; _temp1768.curr= _temp1767;
_temp1768.base= _temp1767; _temp1768.last_plus_one= _temp1767 + 2; _temp1768;}),({
unsigned char* _temp1769=( unsigned char*)","; struct _tagged_string _temp1770;
_temp1770.curr= _temp1769; _temp1770.base= _temp1769; _temp1770.last_plus_one=
_temp1769 + 2; _temp1770;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_exp2doc,
_temp1448)); struct Cyc_PP_Doc* _temp1761[ 2u]={ _temp1763, _temp1764}; struct
_tagged_ptr1 _temp1762={ _temp1761, _temp1761, _temp1761 + 2u}; _temp1760(
_temp1762);});} goto _LL1263; _LL1325: s= Cyc_Absynpp_qvar2doc( _temp1458); goto
_LL1263; _LL1327: if( _temp1462 == 0){ s=({ struct Cyc_PP_Doc*(* _temp1771)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1774= Cyc_PP_text(({
unsigned char* _temp1779=( unsigned char*)"malloc(sizeof("; struct
_tagged_string _temp1780; _temp1780.curr= _temp1779; _temp1780.base= _temp1779;
_temp1780.last_plus_one= _temp1779 + 15; _temp1780;})); struct Cyc_PP_Doc*
_temp1775= Cyc_Absynpp_typ2doc( _temp1460); struct Cyc_PP_Doc* _temp1776= Cyc_PP_text(({
unsigned char* _temp1777=( unsigned char*)"))"; struct _tagged_string _temp1778;
_temp1778.curr= _temp1777; _temp1778.base= _temp1777; _temp1778.last_plus_one=
_temp1777 + 3; _temp1778;})); struct Cyc_PP_Doc* _temp1772[ 3u]={ _temp1774,
_temp1775, _temp1776}; struct _tagged_ptr1 _temp1773={ _temp1772, _temp1772,
_temp1772 + 3u}; _temp1771( _temp1773);});} else{ s=({ struct Cyc_PP_Doc*(*
_temp1781)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1784= Cyc_PP_text(({
unsigned char* _temp1796=( unsigned char*)"rmalloc("; struct _tagged_string
_temp1797; _temp1797.curr= _temp1796; _temp1797.base= _temp1796; _temp1797.last_plus_one=
_temp1796 + 9; _temp1797;})); struct Cyc_PP_Doc* _temp1785= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp1462)); struct Cyc_PP_Doc* _temp1786=
Cyc_PP_text(({ unsigned char* _temp1794=( unsigned char*)","; struct
_tagged_string _temp1795; _temp1795.curr= _temp1794; _temp1795.base= _temp1794;
_temp1795.last_plus_one= _temp1794 + 2; _temp1795;})); struct Cyc_PP_Doc*
_temp1787= Cyc_PP_text(({ unsigned char* _temp1792=( unsigned char*)"sizeof(";
struct _tagged_string _temp1793; _temp1793.curr= _temp1792; _temp1793.base=
_temp1792; _temp1793.last_plus_one= _temp1792 + 8; _temp1793;})); struct Cyc_PP_Doc*
_temp1788= Cyc_Absynpp_typ2doc( _temp1460); struct Cyc_PP_Doc* _temp1789= Cyc_PP_text(({
unsigned char* _temp1790=( unsigned char*)"))"; struct _tagged_string _temp1791;
_temp1791.curr= _temp1790; _temp1791.base= _temp1790; _temp1791.last_plus_one=
_temp1790 + 3; _temp1791;})); struct Cyc_PP_Doc* _temp1782[ 6u]={ _temp1784,
_temp1785, _temp1786, _temp1787, _temp1788, _temp1789}; struct _tagged_ptr1
_temp1783={ _temp1782, _temp1782, _temp1782 + 6u}; _temp1781( _temp1783);});}
goto _LL1263; _LL1329: s= Cyc_PP_group(({ unsigned char* _temp1798=(
unsigned char*)"{"; struct _tagged_string _temp1799; _temp1799.curr= _temp1798;
_temp1799.base= _temp1798; _temp1799.last_plus_one= _temp1798 + 2; _temp1799;}),({
unsigned char* _temp1800=( unsigned char*)"}"; struct _tagged_string _temp1801;
_temp1801.curr= _temp1800; _temp1801.base= _temp1800; _temp1801.last_plus_one=
_temp1800 + 2; _temp1801;}),({ unsigned char* _temp1802=( unsigned char*)",";
struct _tagged_string _temp1803; _temp1803.curr= _temp1802; _temp1803.base=
_temp1802; _temp1803.last_plus_one= _temp1802 + 2; _temp1803;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp1464)); goto _LL1263; _LL1331: s=({ struct Cyc_PP_Doc*(*
_temp1804)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1807= Cyc_PP_text(({
unsigned char* _temp1812=( unsigned char*)"({ "; struct _tagged_string _temp1813;
_temp1813.curr= _temp1812; _temp1813.base= _temp1812; _temp1813.last_plus_one=
_temp1812 + 4; _temp1813;})); struct Cyc_PP_Doc* _temp1808= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1468)); struct Cyc_PP_Doc* _temp1809= Cyc_PP_text(({ unsigned char*
_temp1810=( unsigned char*)" })"; struct _tagged_string _temp1811; _temp1811.curr=
_temp1810; _temp1811.base= _temp1810; _temp1811.last_plus_one= _temp1810 + 4;
_temp1811;})); struct Cyc_PP_Doc* _temp1805[ 3u]={ _temp1807, _temp1808,
_temp1809}; struct _tagged_ptr1 _temp1806={ _temp1805, _temp1805, _temp1805 + 3u};
_temp1804( _temp1806);}); goto _LL1263; _LL1333: s=({ struct Cyc_PP_Doc*(*
_temp1814)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1817= Cyc_PP_text(({
unsigned char* _temp1825=( unsigned char*)"codegen("; struct _tagged_string
_temp1826; _temp1826.curr= _temp1825; _temp1826.base= _temp1825; _temp1826.last_plus_one=
_temp1825 + 9; _temp1826;})); struct Cyc_PP_Doc* _temp1818= Cyc_PP_nest( 2, Cyc_Absynpp_decl2doc(({
struct Cyc_Absyn_Decl* _temp1822=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof(
struct Cyc_Absyn_Decl)); _temp1822->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp1823=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1823[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp1824; _temp1824.tag= Cyc_Absyn_Fn_d;
_temp1824.f1= _temp1470; _temp1824;}); _temp1823;})); _temp1822->loc= e->loc;
_temp1822;}))); struct Cyc_PP_Doc* _temp1819= Cyc_PP_text(({ unsigned char*
_temp1820=( unsigned char*)")"; struct _tagged_string _temp1821; _temp1821.curr=
_temp1820; _temp1821.base= _temp1820; _temp1821.last_plus_one= _temp1820 + 2;
_temp1821;})); struct Cyc_PP_Doc* _temp1815[ 3u]={ _temp1817, _temp1818,
_temp1819}; struct _tagged_ptr1 _temp1816={ _temp1815, _temp1815, _temp1815 + 3u};
_temp1814( _temp1816);}); goto _LL1263; _LL1335: s=({ struct Cyc_PP_Doc*(*
_temp1827)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1830= Cyc_PP_text(({
unsigned char* _temp1835=( unsigned char*)"fill("; struct _tagged_string
_temp1836; _temp1836.curr= _temp1835; _temp1836.base= _temp1835; _temp1836.last_plus_one=
_temp1835 + 6; _temp1836;})); struct Cyc_PP_Doc* _temp1831= Cyc_PP_nest( 2, Cyc_Absynpp_exp2doc(
_temp1472)); struct Cyc_PP_Doc* _temp1832= Cyc_PP_text(({ unsigned char*
_temp1833=( unsigned char*)")"; struct _tagged_string _temp1834; _temp1834.curr=
_temp1833; _temp1834.base= _temp1833; _temp1834.last_plus_one= _temp1833 + 2;
_temp1834;})); struct Cyc_PP_Doc* _temp1828[ 3u]={ _temp1830, _temp1831,
_temp1832}; struct _tagged_ptr1 _temp1829={ _temp1828, _temp1828, _temp1828 + 3u};
_temp1827( _temp1829);}); goto _LL1263; _LL1263:;} if( inprec >= myprec){ s=({
struct Cyc_PP_Doc*(* _temp1837)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1840= Cyc_PP_text(({ unsigned char* _temp1845=( unsigned char*)"("; struct
_tagged_string _temp1846; _temp1846.curr= _temp1845; _temp1846.base= _temp1845;
_temp1846.last_plus_one= _temp1845 + 2; _temp1846;})); struct Cyc_PP_Doc*
_temp1841= s; struct Cyc_PP_Doc* _temp1842= Cyc_PP_text(({ unsigned char*
_temp1843=( unsigned char*)")"; struct _tagged_string _temp1844; _temp1844.curr=
_temp1843; _temp1844.base= _temp1843; _temp1844.last_plus_one= _temp1843 + 2;
_temp1844;})); struct Cyc_PP_Doc* _temp1838[ 3u]={ _temp1840, _temp1841,
_temp1842}; struct _tagged_ptr1 _temp1839={ _temp1838, _temp1838, _temp1838 + 3u};
_temp1837( _temp1839);});} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc(
void* d){ void* _temp1847= d; struct Cyc_Absyn_Exp* _temp1853; struct
_tagged_string* _temp1855; _LL1849: if(*(( int*) _temp1847) == Cyc_Absyn_ArrayElement){
_LL1854: _temp1853=(( struct Cyc_Absyn_ArrayElement_struct*) _temp1847)->f1;
goto _LL1850;} else{ goto _LL1851;} _LL1851: if(*(( int*) _temp1847) == Cyc_Absyn_FieldName){
_LL1856: _temp1855=(( struct Cyc_Absyn_FieldName_struct*) _temp1847)->f1; goto
_LL1852;} else{ goto _LL1848;} _LL1850: return({ struct Cyc_PP_Doc*(* _temp1857)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1860= Cyc_PP_text(({
unsigned char* _temp1865=( unsigned char*)".["; struct _tagged_string _temp1866;
_temp1866.curr= _temp1865; _temp1866.base= _temp1865; _temp1866.last_plus_one=
_temp1865 + 3; _temp1866;})); struct Cyc_PP_Doc* _temp1861= Cyc_Absynpp_exp2doc(
_temp1853); struct Cyc_PP_Doc* _temp1862= Cyc_PP_text(({ unsigned char*
_temp1863=( unsigned char*)"]"; struct _tagged_string _temp1864; _temp1864.curr=
_temp1863; _temp1864.base= _temp1863; _temp1864.last_plus_one= _temp1863 + 2;
_temp1864;})); struct Cyc_PP_Doc* _temp1858[ 3u]={ _temp1860, _temp1861,
_temp1862}; struct _tagged_ptr1 _temp1859={ _temp1858, _temp1858, _temp1858 + 3u};
_temp1857( _temp1859);}); _LL1852: return({ struct Cyc_PP_Doc*(* _temp1867)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1870= Cyc_PP_text(({
unsigned char* _temp1872=( unsigned char*)"."; struct _tagged_string _temp1873;
_temp1873.curr= _temp1872; _temp1873.base= _temp1872; _temp1873.last_plus_one=
_temp1872 + 2; _temp1873;})); struct Cyc_PP_Doc* _temp1871= Cyc_PP_textptr(
_temp1855); struct Cyc_PP_Doc* _temp1868[ 2u]={ _temp1870, _temp1871}; struct
_tagged_ptr1 _temp1869={ _temp1868, _temp1868, _temp1868 + 2u}; _temp1867(
_temp1869);}); _LL1848:;} struct Cyc_PP_Doc* Cyc_Absynpp_de2doc( struct _tuple6*
de){ if((* de).f1 == 0){ return Cyc_Absynpp_exp2doc((* de).f2);} else{ return({
struct Cyc_PP_Doc*(* _temp1874)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1877= Cyc_PP_egroup(({ unsigned char* _temp1879=( unsigned char*)""; struct
_tagged_string _temp1880; _temp1880.curr= _temp1879; _temp1880.base= _temp1879;
_temp1880.last_plus_one= _temp1879 + 1; _temp1880;}),({ unsigned char* _temp1881=(
unsigned char*)"="; struct _tagged_string _temp1882; _temp1882.curr= _temp1881;
_temp1882.base= _temp1881; _temp1882.last_plus_one= _temp1881 + 2; _temp1882;}),({
unsigned char* _temp1883=( unsigned char*)"="; struct _tagged_string _temp1884;
_temp1884.curr= _temp1883; _temp1884.base= _temp1883; _temp1884.last_plus_one=
_temp1883 + 2; _temp1884;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
de).f1)); struct Cyc_PP_Doc* _temp1878= Cyc_Absynpp_exp2doc((* de).f2); struct
Cyc_PP_Doc* _temp1875[ 2u]={ _temp1877, _temp1878}; struct _tagged_ptr1
_temp1876={ _temp1875, _temp1875, _temp1875 + 2u}; _temp1874( _temp1876);});}}
struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec( int inprec, struct Cyc_List_List*
es){ return Cyc_PP_group(({ unsigned char* _temp1885=( unsigned char*)""; struct
_tagged_string _temp1886; _temp1886.curr= _temp1885; _temp1886.base= _temp1885;
_temp1886.last_plus_one= _temp1885 + 1; _temp1886;}),({ unsigned char* _temp1887=(
unsigned char*)""; struct _tagged_string _temp1888; _temp1888.curr= _temp1887;
_temp1888.base= _temp1887; _temp1888.last_plus_one= _temp1887 + 1; _temp1888;}),({
unsigned char* _temp1889=( unsigned char*)","; struct _tagged_string _temp1890;
_temp1890.curr= _temp1889; _temp1890.base= _temp1889; _temp1890.last_plus_one=
_temp1889 + 2; _temp1890;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Absynpp_exp2doc_prec, inprec, es));} struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc(
void* c){ void* _temp1891= c; unsigned char _temp1909; void* _temp1911; short
_temp1913; void* _temp1915; int _temp1917; void* _temp1919; int _temp1921; void*
_temp1923; long long _temp1925; void* _temp1927; struct _tagged_string _temp1929;
struct _tagged_string _temp1931; _LL1893: if(( unsigned int) _temp1891 > 1u?*((
int*) _temp1891) == Cyc_Absyn_Char_c: 0){ _LL1912: _temp1911=( void*)(( struct
Cyc_Absyn_Char_c_struct*) _temp1891)->f1; goto _LL1910; _LL1910: _temp1909=((
struct Cyc_Absyn_Char_c_struct*) _temp1891)->f2; goto _LL1894;} else{ goto
_LL1895;} _LL1895: if(( unsigned int) _temp1891 > 1u?*(( int*) _temp1891) == Cyc_Absyn_Short_c:
0){ _LL1916: _temp1915=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp1891)->f1;
goto _LL1914; _LL1914: _temp1913=(( struct Cyc_Absyn_Short_c_struct*) _temp1891)->f2;
goto _LL1896;} else{ goto _LL1897;} _LL1897: if(( unsigned int) _temp1891 > 1u?*((
int*) _temp1891) == Cyc_Absyn_Int_c: 0){ _LL1920: _temp1919=( void*)(( struct
Cyc_Absyn_Int_c_struct*) _temp1891)->f1; if( _temp1919 ==( void*) Cyc_Absyn_Signed){
goto _LL1918;} else{ goto _LL1899;} _LL1918: _temp1917=(( struct Cyc_Absyn_Int_c_struct*)
_temp1891)->f2; goto _LL1898;} else{ goto _LL1899;} _LL1899: if(( unsigned int)
_temp1891 > 1u?*(( int*) _temp1891) == Cyc_Absyn_Int_c: 0){ _LL1924: _temp1923=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1891)->f1; if( _temp1923 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL1922;} else{ goto _LL1901;} _LL1922: _temp1921=((
struct Cyc_Absyn_Int_c_struct*) _temp1891)->f2; goto _LL1900;} else{ goto
_LL1901;} _LL1901: if(( unsigned int) _temp1891 > 1u?*(( int*) _temp1891) == Cyc_Absyn_LongLong_c:
0){ _LL1928: _temp1927=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp1891)->f1;
goto _LL1926; _LL1926: _temp1925=(( struct Cyc_Absyn_LongLong_c_struct*)
_temp1891)->f2; goto _LL1902;} else{ goto _LL1903;} _LL1903: if(( unsigned int)
_temp1891 > 1u?*(( int*) _temp1891) == Cyc_Absyn_Float_c: 0){ _LL1930: _temp1929=((
struct Cyc_Absyn_Float_c_struct*) _temp1891)->f1; goto _LL1904;} else{ goto
_LL1905;} _LL1905: if( _temp1891 ==( void*) Cyc_Absyn_Null_c){ goto _LL1906;}
else{ goto _LL1907;} _LL1907: if(( unsigned int) _temp1891 > 1u?*(( int*)
_temp1891) == Cyc_Absyn_String_c: 0){ _LL1932: _temp1931=(( struct Cyc_Absyn_String_c_struct*)
_temp1891)->f1; goto _LL1908;} else{ goto _LL1892;} _LL1894: return Cyc_PP_text((
struct _tagged_string)({ struct _tagged_string _temp1933= Cyc_Absynpp_char_escape(
_temp1909); xprintf("'%.*s'", _temp1933.last_plus_one - _temp1933.curr,
_temp1933.curr);})); _LL1896: return Cyc_PP_text(( struct _tagged_string)
xprintf("%d",( int) _temp1913)); _LL1898: return Cyc_PP_text(( struct
_tagged_string) Cyc_Core_string_of_int( _temp1917)); _LL1900: return Cyc_PP_text((
struct _tagged_string) xprintf("%u",( unsigned int) _temp1921)); _LL1902: return
Cyc_PP_text(({ unsigned char* _temp1934=( unsigned char*)"<<FIX LONG LONG CONSTANT>>";
struct _tagged_string _temp1935; _temp1935.curr= _temp1934; _temp1935.base=
_temp1934; _temp1935.last_plus_one= _temp1934 + 27; _temp1935;})); _LL1904:
return Cyc_PP_text( _temp1929); _LL1906: return Cyc_PP_text(({ unsigned char*
_temp1936=( unsigned char*)"null"; struct _tagged_string _temp1937; _temp1937.curr=
_temp1936; _temp1937.base= _temp1936; _temp1937.last_plus_one= _temp1936 + 5;
_temp1937;})); _LL1908: return({ struct Cyc_PP_Doc*(* _temp1938)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1941= Cyc_PP_text(({
unsigned char* _temp1946=( unsigned char*)"\""; struct _tagged_string _temp1947;
_temp1947.curr= _temp1946; _temp1947.base= _temp1946; _temp1947.last_plus_one=
_temp1946 + 2; _temp1947;})); struct Cyc_PP_Doc* _temp1942= Cyc_PP_text( Cyc_Absynpp_string_escape(
_temp1931)); struct Cyc_PP_Doc* _temp1943= Cyc_PP_text(({ unsigned char*
_temp1944=( unsigned char*)"\""; struct _tagged_string _temp1945; _temp1945.curr=
_temp1944; _temp1945.base= _temp1944; _temp1945.last_plus_one= _temp1944 + 2;
_temp1945;})); struct Cyc_PP_Doc* _temp1939[ 3u]={ _temp1941, _temp1942,
_temp1943}; struct _tagged_ptr1 _temp1940={ _temp1939, _temp1939, _temp1939 + 3u};
_temp1938( _temp1940);}); _LL1892:;} struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc(
int inprec, void* p, struct Cyc_List_List* es){ struct Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc(
p); if( p ==( void*) Cyc_Absyn_Size){ if( es == 0? 1:(( struct Cyc_List_List*)
_check_null( es))->tl != 0){( void) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp1948=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1948[ 0]=({ struct Cyc_Core_Failure_struct _temp1949; _temp1949.tag= Cyc_Core_Failure;
_temp1949.f1=( struct _tagged_string)({ struct _tagged_string _temp1950= Cyc_PP_string_of_doc(
ps, 72); xprintf("Absynpp::primapp2doc Size: %.*s with bad args", _temp1950.last_plus_one
- _temp1950.curr, _temp1950.curr);}); _temp1949;}); _temp1948;}));}{ struct Cyc_PP_Doc*
_temp1951= Cyc_Absynpp_exp2doc_prec( inprec,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd); goto _LL1952; _LL1952: return({ struct
Cyc_PP_Doc*(* _temp1953)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1956= _temp1951; struct Cyc_PP_Doc* _temp1957= Cyc_PP_text(({ unsigned char*
_temp1958=( unsigned char*)".size"; struct _tagged_string _temp1959; _temp1959.curr=
_temp1958; _temp1959.base= _temp1958; _temp1959.last_plus_one= _temp1958 + 6;
_temp1959;})); struct Cyc_PP_Doc* _temp1954[ 2u]={ _temp1956, _temp1957}; struct
_tagged_ptr1 _temp1955={ _temp1954, _temp1954, _temp1954 + 2u}; _temp1953(
_temp1955);});}} else{ if( ! Cyc_Absyn_is_format_prim( p)){ struct Cyc_List_List*
ds=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*),
int env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec,
inprec, es); if( ds == 0){( void) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp1960=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1960[ 0]=({ struct Cyc_Core_Failure_struct _temp1961; _temp1961.tag= Cyc_Core_Failure;
_temp1961.f1=( struct _tagged_string)({ struct _tagged_string _temp1962= Cyc_PP_string_of_doc(
ps, 72); xprintf("Absynpp::primapp2doc: %.*s with no args", _temp1962.last_plus_one
- _temp1962.curr, _temp1962.curr);}); _temp1961;}); _temp1960;}));} else{ if(((
struct Cyc_List_List*) _check_null( ds))->tl == 0){ return({ struct Cyc_PP_Doc*(*
_temp1963)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1966= ps;
struct Cyc_PP_Doc* _temp1967= Cyc_PP_text(({ unsigned char* _temp1969=(
unsigned char*)" "; struct _tagged_string _temp1970; _temp1970.curr= _temp1969;
_temp1970.base= _temp1969; _temp1970.last_plus_one= _temp1969 + 2; _temp1970;}));
struct Cyc_PP_Doc* _temp1968=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*)
_check_null( ds))->hd; struct Cyc_PP_Doc* _temp1964[ 3u]={ _temp1966, _temp1967,
_temp1968}; struct _tagged_ptr1 _temp1965={ _temp1964, _temp1964, _temp1964 + 3u};
_temp1963( _temp1965);});} else{ if((( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( ds))->tl))->tl != 0){( void) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp1971=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1971[ 0]=({ struct Cyc_Core_Failure_struct
_temp1972; _temp1972.tag= Cyc_Core_Failure; _temp1972.f1=( struct _tagged_string)({
struct _tagged_string _temp1973= Cyc_PP_string_of_doc( ps, 72); xprintf("Absynpp::primapp2doc: %.*s with more than 2 args",
_temp1973.last_plus_one - _temp1973.curr, _temp1973.curr);}); _temp1972;});
_temp1971;}));} else{ return({ struct Cyc_PP_Doc*(* _temp1974)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1977=( struct Cyc_PP_Doc*)((
struct Cyc_List_List*) _check_null( ds))->hd; struct Cyc_PP_Doc* _temp1978= Cyc_PP_text(({
unsigned char* _temp1984=( unsigned char*)" "; struct _tagged_string _temp1985;
_temp1985.curr= _temp1984; _temp1985.base= _temp1984; _temp1985.last_plus_one=
_temp1984 + 2; _temp1985;})); struct Cyc_PP_Doc* _temp1979= ps; struct Cyc_PP_Doc*
_temp1980= Cyc_PP_text(({ unsigned char* _temp1982=( unsigned char*)" "; struct
_tagged_string _temp1983; _temp1983.curr= _temp1982; _temp1983.base= _temp1982;
_temp1983.last_plus_one= _temp1982 + 2; _temp1983;})); struct Cyc_PP_Doc*
_temp1981=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null((( struct
Cyc_List_List*) _check_null( ds))->tl))->hd; struct Cyc_PP_Doc* _temp1975[ 5u]={
_temp1977, _temp1978, _temp1979, _temp1980, _temp1981}; struct _tagged_ptr1
_temp1976={ _temp1975, _temp1975, _temp1975 + 5u}; _temp1974( _temp1976);});}}}}
else{ return({ struct Cyc_PP_Doc*(* _temp1986)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1989= ps; struct Cyc_PP_Doc* _temp1990= Cyc_PP_text(({
unsigned char* _temp1995=( unsigned char*)"("; struct _tagged_string _temp1996;
_temp1996.curr= _temp1995; _temp1996.base= _temp1995; _temp1996.last_plus_one=
_temp1995 + 2; _temp1996;})); struct Cyc_PP_Doc* _temp1991= Cyc_Absynpp_exps2doc_prec(
20, es); struct Cyc_PP_Doc* _temp1992= Cyc_PP_text(({ unsigned char* _temp1993=(
unsigned char*)")"; struct _tagged_string _temp1994; _temp1994.curr= _temp1993;
_temp1994.base= _temp1993; _temp1994.last_plus_one= _temp1993 + 2; _temp1994;}));
struct Cyc_PP_Doc* _temp1987[ 4u]={ _temp1989, _temp1990, _temp1991, _temp1992};
struct _tagged_ptr1 _temp1988={ _temp1987, _temp1987, _temp1987 + 4u}; _temp1986(
_temp1988);});}}} struct _tagged_string Cyc_Absynpp_prim2str( void* p){ void*
_temp1997= p; _LL1999: if( _temp1997 ==( void*) Cyc_Absyn_Plus){ goto _LL2000;}
else{ goto _LL2001;} _LL2001: if( _temp1997 ==( void*) Cyc_Absyn_Times){ goto
_LL2002;} else{ goto _LL2003;} _LL2003: if( _temp1997 ==( void*) Cyc_Absyn_Minus){
goto _LL2004;} else{ goto _LL2005;} _LL2005: if( _temp1997 ==( void*) Cyc_Absyn_Div){
goto _LL2006;} else{ goto _LL2007;} _LL2007: if( _temp1997 ==( void*) Cyc_Absyn_Mod){
goto _LL2008;} else{ goto _LL2009;} _LL2009: if( _temp1997 ==( void*) Cyc_Absyn_Eq){
goto _LL2010;} else{ goto _LL2011;} _LL2011: if( _temp1997 ==( void*) Cyc_Absyn_Neq){
goto _LL2012;} else{ goto _LL2013;} _LL2013: if( _temp1997 ==( void*) Cyc_Absyn_Gt){
goto _LL2014;} else{ goto _LL2015;} _LL2015: if( _temp1997 ==( void*) Cyc_Absyn_Lt){
goto _LL2016;} else{ goto _LL2017;} _LL2017: if( _temp1997 ==( void*) Cyc_Absyn_Gte){
goto _LL2018;} else{ goto _LL2019;} _LL2019: if( _temp1997 ==( void*) Cyc_Absyn_Lte){
goto _LL2020;} else{ goto _LL2021;} _LL2021: if( _temp1997 ==( void*) Cyc_Absyn_Not){
goto _LL2022;} else{ goto _LL2023;} _LL2023: if( _temp1997 ==( void*) Cyc_Absyn_Bitnot){
goto _LL2024;} else{ goto _LL2025;} _LL2025: if( _temp1997 ==( void*) Cyc_Absyn_Bitand){
goto _LL2026;} else{ goto _LL2027;} _LL2027: if( _temp1997 ==( void*) Cyc_Absyn_Bitor){
goto _LL2028;} else{ goto _LL2029;} _LL2029: if( _temp1997 ==( void*) Cyc_Absyn_Bitxor){
goto _LL2030;} else{ goto _LL2031;} _LL2031: if( _temp1997 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL2032;} else{ goto _LL2033;} _LL2033: if( _temp1997 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL2034;} else{ goto _LL2035;} _LL2035: if( _temp1997 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL2036;} else{ goto _LL2037;} _LL2037: if( _temp1997 ==( void*) Cyc_Absyn_Size){
goto _LL2038;} else{ goto _LL2039;} _LL2039: if( _temp1997 ==( void*) Cyc_Absyn_Printf){
goto _LL2040;} else{ goto _LL2041;} _LL2041: if( _temp1997 ==( void*) Cyc_Absyn_Fprintf){
goto _LL2042;} else{ goto _LL2043;} _LL2043: if( _temp1997 ==( void*) Cyc_Absyn_Xprintf){
goto _LL2044;} else{ goto _LL2045;} _LL2045: if( _temp1997 ==( void*) Cyc_Absyn_Scanf){
goto _LL2046;} else{ goto _LL2047;} _LL2047: if( _temp1997 ==( void*) Cyc_Absyn_Fscanf){
goto _LL2048;} else{ goto _LL2049;} _LL2049: if( _temp1997 ==( void*) Cyc_Absyn_Sscanf){
goto _LL2050;} else{ goto _LL1998;} _LL2000: return({ unsigned char* _temp2051=(
unsigned char*)"+"; struct _tagged_string _temp2052; _temp2052.curr= _temp2051;
_temp2052.base= _temp2051; _temp2052.last_plus_one= _temp2051 + 2; _temp2052;});
_LL2002: return({ unsigned char* _temp2053=( unsigned char*)"*"; struct
_tagged_string _temp2054; _temp2054.curr= _temp2053; _temp2054.base= _temp2053;
_temp2054.last_plus_one= _temp2053 + 2; _temp2054;}); _LL2004: return({
unsigned char* _temp2055=( unsigned char*)"-"; struct _tagged_string _temp2056;
_temp2056.curr= _temp2055; _temp2056.base= _temp2055; _temp2056.last_plus_one=
_temp2055 + 2; _temp2056;}); _LL2006: return({ unsigned char* _temp2057=(
unsigned char*)"/"; struct _tagged_string _temp2058; _temp2058.curr= _temp2057;
_temp2058.base= _temp2057; _temp2058.last_plus_one= _temp2057 + 2; _temp2058;});
_LL2008: return({ unsigned char* _temp2059=( unsigned char*)"%"; struct
_tagged_string _temp2060; _temp2060.curr= _temp2059; _temp2060.base= _temp2059;
_temp2060.last_plus_one= _temp2059 + 2; _temp2060;}); _LL2010: return({
unsigned char* _temp2061=( unsigned char*)"=="; struct _tagged_string _temp2062;
_temp2062.curr= _temp2061; _temp2062.base= _temp2061; _temp2062.last_plus_one=
_temp2061 + 3; _temp2062;}); _LL2012: return({ unsigned char* _temp2063=(
unsigned char*)"!="; struct _tagged_string _temp2064; _temp2064.curr= _temp2063;
_temp2064.base= _temp2063; _temp2064.last_plus_one= _temp2063 + 3; _temp2064;});
_LL2014: return({ unsigned char* _temp2065=( unsigned char*)">"; struct
_tagged_string _temp2066; _temp2066.curr= _temp2065; _temp2066.base= _temp2065;
_temp2066.last_plus_one= _temp2065 + 2; _temp2066;}); _LL2016: return({
unsigned char* _temp2067=( unsigned char*)"<"; struct _tagged_string _temp2068;
_temp2068.curr= _temp2067; _temp2068.base= _temp2067; _temp2068.last_plus_one=
_temp2067 + 2; _temp2068;}); _LL2018: return({ unsigned char* _temp2069=(
unsigned char*)">="; struct _tagged_string _temp2070; _temp2070.curr= _temp2069;
_temp2070.base= _temp2069; _temp2070.last_plus_one= _temp2069 + 3; _temp2070;});
_LL2020: return({ unsigned char* _temp2071=( unsigned char*)"<="; struct
_tagged_string _temp2072; _temp2072.curr= _temp2071; _temp2072.base= _temp2071;
_temp2072.last_plus_one= _temp2071 + 3; _temp2072;}); _LL2022: return({
unsigned char* _temp2073=( unsigned char*)"!"; struct _tagged_string _temp2074;
_temp2074.curr= _temp2073; _temp2074.base= _temp2073; _temp2074.last_plus_one=
_temp2073 + 2; _temp2074;}); _LL2024: return({ unsigned char* _temp2075=(
unsigned char*)"~"; struct _tagged_string _temp2076; _temp2076.curr= _temp2075;
_temp2076.base= _temp2075; _temp2076.last_plus_one= _temp2075 + 2; _temp2076;});
_LL2026: return({ unsigned char* _temp2077=( unsigned char*)"&"; struct
_tagged_string _temp2078; _temp2078.curr= _temp2077; _temp2078.base= _temp2077;
_temp2078.last_plus_one= _temp2077 + 2; _temp2078;}); _LL2028: return({
unsigned char* _temp2079=( unsigned char*)"|"; struct _tagged_string _temp2080;
_temp2080.curr= _temp2079; _temp2080.base= _temp2079; _temp2080.last_plus_one=
_temp2079 + 2; _temp2080;}); _LL2030: return({ unsigned char* _temp2081=(
unsigned char*)"^"; struct _tagged_string _temp2082; _temp2082.curr= _temp2081;
_temp2082.base= _temp2081; _temp2082.last_plus_one= _temp2081 + 2; _temp2082;});
_LL2032: return({ unsigned char* _temp2083=( unsigned char*)"<<"; struct
_tagged_string _temp2084; _temp2084.curr= _temp2083; _temp2084.base= _temp2083;
_temp2084.last_plus_one= _temp2083 + 3; _temp2084;}); _LL2034: return({
unsigned char* _temp2085=( unsigned char*)">>"; struct _tagged_string _temp2086;
_temp2086.curr= _temp2085; _temp2086.base= _temp2085; _temp2086.last_plus_one=
_temp2085 + 3; _temp2086;}); _LL2036: return({ unsigned char* _temp2087=(
unsigned char*)">>>"; struct _tagged_string _temp2088; _temp2088.curr= _temp2087;
_temp2088.base= _temp2087; _temp2088.last_plus_one= _temp2087 + 4; _temp2088;});
_LL2038: return({ unsigned char* _temp2089=( unsigned char*)"size"; struct
_tagged_string _temp2090; _temp2090.curr= _temp2089; _temp2090.base= _temp2089;
_temp2090.last_plus_one= _temp2089 + 5; _temp2090;}); _LL2040: return({
unsigned char* _temp2091=( unsigned char*)"printf"; struct _tagged_string
_temp2092; _temp2092.curr= _temp2091; _temp2092.base= _temp2091; _temp2092.last_plus_one=
_temp2091 + 7; _temp2092;}); _LL2042: return({ unsigned char* _temp2093=(
unsigned char*)"fprintf"; struct _tagged_string _temp2094; _temp2094.curr=
_temp2093; _temp2094.base= _temp2093; _temp2094.last_plus_one= _temp2093 + 8;
_temp2094;}); _LL2044: return({ unsigned char* _temp2095=( unsigned char*)"xprintf";
struct _tagged_string _temp2096; _temp2096.curr= _temp2095; _temp2096.base=
_temp2095; _temp2096.last_plus_one= _temp2095 + 8; _temp2096;}); _LL2046: return({
unsigned char* _temp2097=( unsigned char*)"scanf"; struct _tagged_string
_temp2098; _temp2098.curr= _temp2097; _temp2098.base= _temp2097; _temp2098.last_plus_one=
_temp2097 + 6; _temp2098;}); _LL2048: return({ unsigned char* _temp2099=(
unsigned char*)"fscanf"; struct _tagged_string _temp2100; _temp2100.curr=
_temp2099; _temp2100.base= _temp2099; _temp2100.last_plus_one= _temp2099 + 7;
_temp2100;}); _LL2050: return({ unsigned char* _temp2101=( unsigned char*)"sscanf";
struct _tagged_string _temp2102; _temp2102.curr= _temp2101; _temp2102.base=
_temp2101; _temp2102.last_plus_one= _temp2101 + 7; _temp2102;}); _LL1998:;}
struct Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void* p){ return Cyc_PP_text( Cyc_Absynpp_prim2str(
p));} struct _tuple9{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ;
struct Cyc_PP_Doc* Cyc_Absynpp_structarg2doc( struct _tuple9* t){ if((* t).f1 ==
0){ return Cyc_Absynpp_exp2doc((* t).f2);} else{ return({ struct Cyc_PP_Doc*(*
_temp2103)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2106= Cyc_PP_textptr((
struct _tagged_string*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v);
struct Cyc_PP_Doc* _temp2107= Cyc_PP_text(({ unsigned char* _temp2109=(
unsigned char*)" = "; struct _tagged_string _temp2110; _temp2110.curr= _temp2109;
_temp2110.base= _temp2109; _temp2110.last_plus_one= _temp2109 + 4; _temp2110;}));
struct Cyc_PP_Doc* _temp2108= Cyc_Absynpp_exp2doc((* t).f2); struct Cyc_PP_Doc*
_temp2104[ 3u]={ _temp2106, _temp2107, _temp2108}; struct _tagged_ptr1 _temp2105={
_temp2104, _temp2104, _temp2104 + 3u}; _temp2103( _temp2105);});}} int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt* s){ void* _temp2111=( void*) s->r; struct Cyc_Absyn_Stmt*
_temp2117; struct Cyc_Absyn_Decl* _temp2119; _LL2113: if(( unsigned int)
_temp2111 > 1u?*(( int*) _temp2111) == Cyc_Absyn_Decl_s: 0){ _LL2120: _temp2119=((
struct Cyc_Absyn_Decl_s_struct*) _temp2111)->f1; goto _LL2118; _LL2118:
_temp2117=(( struct Cyc_Absyn_Decl_s_struct*) _temp2111)->f2; goto _LL2114;}
else{ goto _LL2115;} _LL2115: goto _LL2116; _LL2114: return 1; _LL2116: return 0;
_LL2112:;} struct Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct Cyc_Absyn_Stmt* st){
struct Cyc_PP_Doc* s;{ void* _temp2121=( void*) st->r; struct Cyc_Absyn_Exp*
_temp2165; struct Cyc_Absyn_Stmt* _temp2167; struct Cyc_Absyn_Stmt* _temp2169;
struct Cyc_Absyn_Exp* _temp2171; struct Cyc_Absyn_Stmt* _temp2173; struct Cyc_Absyn_Stmt*
_temp2175; struct Cyc_Absyn_Exp* _temp2177; struct Cyc_Absyn_Stmt* _temp2179;
struct _tuple2 _temp2181; struct Cyc_Absyn_Stmt* _temp2183; struct Cyc_Absyn_Exp*
_temp2185; struct Cyc_Absyn_Stmt* _temp2187; struct Cyc_Absyn_Stmt* _temp2189;
struct Cyc_Absyn_Stmt* _temp2191; struct _tagged_string* _temp2193; struct Cyc_Absyn_Stmt*
_temp2195; struct _tuple2 _temp2197; struct Cyc_Absyn_Stmt* _temp2199; struct
Cyc_Absyn_Exp* _temp2201; struct _tuple2 _temp2203; struct Cyc_Absyn_Stmt*
_temp2205; struct Cyc_Absyn_Exp* _temp2207; struct Cyc_Absyn_Exp* _temp2209;
struct Cyc_List_List* _temp2211; struct Cyc_Absyn_Exp* _temp2213; struct Cyc_List_List*
_temp2215; struct Cyc_Absyn_Exp* _temp2217; struct Cyc_Absyn_Switch_clause**
_temp2219; struct Cyc_List_List* _temp2221; struct Cyc_Absyn_Switch_clause**
_temp2223; struct Cyc_List_List* _temp2225; struct Cyc_Absyn_Stmt* _temp2227;
struct Cyc_Absyn_Decl* _temp2229; struct Cyc_Absyn_Stmt* _temp2231; struct Cyc_Absyn_Stmt*
_temp2233; struct Cyc_Absyn_Stmt* _temp2235; struct _tagged_string* _temp2237;
struct _tuple2 _temp2239; struct Cyc_Absyn_Stmt* _temp2241; struct Cyc_Absyn_Exp*
_temp2243; struct Cyc_Absyn_Stmt* _temp2245; struct Cyc_List_List* _temp2247;
struct Cyc_Absyn_Stmt* _temp2249; struct Cyc_Absyn_Stmt* _temp2251; struct Cyc_Absyn_Vardecl*
_temp2253; struct Cyc_Absyn_Tvar* _temp2255; _LL2123: if( _temp2121 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL2124;} else{ goto _LL2125;} _LL2125: if((
unsigned int) _temp2121 > 1u?*(( int*) _temp2121) == Cyc_Absyn_Exp_s: 0){
_LL2166: _temp2165=(( struct Cyc_Absyn_Exp_s_struct*) _temp2121)->f1; goto
_LL2126;} else{ goto _LL2127;} _LL2127: if(( unsigned int) _temp2121 > 1u?*((
int*) _temp2121) == Cyc_Absyn_Seq_s: 0){ _LL2170: _temp2169=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2121)->f1; goto _LL2168; _LL2168: _temp2167=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2121)->f2; goto _LL2128;} else{ goto _LL2129;} _LL2129: if(( unsigned int)
_temp2121 > 1u?*(( int*) _temp2121) == Cyc_Absyn_Return_s: 0){ _LL2172:
_temp2171=(( struct Cyc_Absyn_Return_s_struct*) _temp2121)->f1; goto _LL2130;}
else{ goto _LL2131;} _LL2131: if(( unsigned int) _temp2121 > 1u?*(( int*)
_temp2121) == Cyc_Absyn_IfThenElse_s: 0){ _LL2178: _temp2177=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2121)->f1; goto _LL2176; _LL2176: _temp2175=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2121)->f2; goto _LL2174; _LL2174: _temp2173=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2121)->f3; goto _LL2132;} else{ goto _LL2133;} _LL2133: if(( unsigned int)
_temp2121 > 1u?*(( int*) _temp2121) == Cyc_Absyn_While_s: 0){ _LL2182: _temp2181=((
struct Cyc_Absyn_While_s_struct*) _temp2121)->f1; _LL2186: _temp2185= _temp2181.f1;
goto _LL2184; _LL2184: _temp2183= _temp2181.f2; goto _LL2180; _LL2180: _temp2179=((
struct Cyc_Absyn_While_s_struct*) _temp2121)->f2; goto _LL2134;} else{ goto
_LL2135;} _LL2135: if(( unsigned int) _temp2121 > 1u?*(( int*) _temp2121) == Cyc_Absyn_Break_s:
0){ _LL2188: _temp2187=(( struct Cyc_Absyn_Break_s_struct*) _temp2121)->f1; goto
_LL2136;} else{ goto _LL2137;} _LL2137: if(( unsigned int) _temp2121 > 1u?*((
int*) _temp2121) == Cyc_Absyn_Continue_s: 0){ _LL2190: _temp2189=(( struct Cyc_Absyn_Continue_s_struct*)
_temp2121)->f1; goto _LL2138;} else{ goto _LL2139;} _LL2139: if(( unsigned int)
_temp2121 > 1u?*(( int*) _temp2121) == Cyc_Absyn_Goto_s: 0){ _LL2194: _temp2193=((
struct Cyc_Absyn_Goto_s_struct*) _temp2121)->f1; goto _LL2192; _LL2192:
_temp2191=(( struct Cyc_Absyn_Goto_s_struct*) _temp2121)->f2; goto _LL2140;}
else{ goto _LL2141;} _LL2141: if(( unsigned int) _temp2121 > 1u?*(( int*)
_temp2121) == Cyc_Absyn_For_s: 0){ _LL2210: _temp2209=(( struct Cyc_Absyn_For_s_struct*)
_temp2121)->f1; goto _LL2204; _LL2204: _temp2203=(( struct Cyc_Absyn_For_s_struct*)
_temp2121)->f2; _LL2208: _temp2207= _temp2203.f1; goto _LL2206; _LL2206:
_temp2205= _temp2203.f2; goto _LL2198; _LL2198: _temp2197=(( struct Cyc_Absyn_For_s_struct*)
_temp2121)->f3; _LL2202: _temp2201= _temp2197.f1; goto _LL2200; _LL2200:
_temp2199= _temp2197.f2; goto _LL2196; _LL2196: _temp2195=(( struct Cyc_Absyn_For_s_struct*)
_temp2121)->f4; goto _LL2142;} else{ goto _LL2143;} _LL2143: if(( unsigned int)
_temp2121 > 1u?*(( int*) _temp2121) == Cyc_Absyn_Switch_s: 0){ _LL2214:
_temp2213=(( struct Cyc_Absyn_Switch_s_struct*) _temp2121)->f1; goto _LL2212;
_LL2212: _temp2211=(( struct Cyc_Absyn_Switch_s_struct*) _temp2121)->f2; goto
_LL2144;} else{ goto _LL2145;} _LL2145: if(( unsigned int) _temp2121 > 1u?*((
int*) _temp2121) == Cyc_Absyn_SwitchC_s: 0){ _LL2218: _temp2217=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp2121)->f1; goto _LL2216; _LL2216: _temp2215=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp2121)->f2; goto _LL2146;} else{ goto _LL2147;} _LL2147: if(( unsigned int)
_temp2121 > 1u?*(( int*) _temp2121) == Cyc_Absyn_Fallthru_s: 0){ _LL2222:
_temp2221=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp2121)->f1; if( _temp2221
== 0){ goto _LL2220;} else{ goto _LL2149;} _LL2220: _temp2219=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2121)->f2; goto _LL2148;} else{ goto _LL2149;} _LL2149: if(( unsigned int)
_temp2121 > 1u?*(( int*) _temp2121) == Cyc_Absyn_Fallthru_s: 0){ _LL2226:
_temp2225=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp2121)->f1; goto _LL2224;
_LL2224: _temp2223=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp2121)->f2; goto
_LL2150;} else{ goto _LL2151;} _LL2151: if(( unsigned int) _temp2121 > 1u?*((
int*) _temp2121) == Cyc_Absyn_Decl_s: 0){ _LL2230: _temp2229=(( struct Cyc_Absyn_Decl_s_struct*)
_temp2121)->f1; goto _LL2228; _LL2228: _temp2227=(( struct Cyc_Absyn_Decl_s_struct*)
_temp2121)->f2; goto _LL2152;} else{ goto _LL2153;} _LL2153: if(( unsigned int)
_temp2121 > 1u?*(( int*) _temp2121) == Cyc_Absyn_Cut_s: 0){ _LL2232: _temp2231=((
struct Cyc_Absyn_Cut_s_struct*) _temp2121)->f1; goto _LL2154;} else{ goto
_LL2155;} _LL2155: if(( unsigned int) _temp2121 > 1u?*(( int*) _temp2121) == Cyc_Absyn_Splice_s:
0){ _LL2234: _temp2233=(( struct Cyc_Absyn_Splice_s_struct*) _temp2121)->f1;
goto _LL2156;} else{ goto _LL2157;} _LL2157: if(( unsigned int) _temp2121 > 1u?*((
int*) _temp2121) == Cyc_Absyn_Label_s: 0){ _LL2238: _temp2237=(( struct Cyc_Absyn_Label_s_struct*)
_temp2121)->f1; goto _LL2236; _LL2236: _temp2235=(( struct Cyc_Absyn_Label_s_struct*)
_temp2121)->f2; goto _LL2158;} else{ goto _LL2159;} _LL2159: if(( unsigned int)
_temp2121 > 1u?*(( int*) _temp2121) == Cyc_Absyn_Do_s: 0){ _LL2246: _temp2245=((
struct Cyc_Absyn_Do_s_struct*) _temp2121)->f1; goto _LL2240; _LL2240: _temp2239=((
struct Cyc_Absyn_Do_s_struct*) _temp2121)->f2; _LL2244: _temp2243= _temp2239.f1;
goto _LL2242; _LL2242: _temp2241= _temp2239.f2; goto _LL2160;} else{ goto
_LL2161;} _LL2161: if(( unsigned int) _temp2121 > 1u?*(( int*) _temp2121) == Cyc_Absyn_TryCatch_s:
0){ _LL2250: _temp2249=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp2121)->f1;
goto _LL2248; _LL2248: _temp2247=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2121)->f2; goto _LL2162;} else{ goto _LL2163;} _LL2163: if(( unsigned int)
_temp2121 > 1u?*(( int*) _temp2121) == Cyc_Absyn_Region_s: 0){ _LL2256:
_temp2255=(( struct Cyc_Absyn_Region_s_struct*) _temp2121)->f1; goto _LL2254;
_LL2254: _temp2253=(( struct Cyc_Absyn_Region_s_struct*) _temp2121)->f2; goto
_LL2252; _LL2252: _temp2251=(( struct Cyc_Absyn_Region_s_struct*) _temp2121)->f3;
goto _LL2164;} else{ goto _LL2122;} _LL2124: s= Cyc_PP_text(({ unsigned char*
_temp2257=( unsigned char*)";"; struct _tagged_string _temp2258; _temp2258.curr=
_temp2257; _temp2258.base= _temp2257; _temp2258.last_plus_one= _temp2257 + 2;
_temp2258;})); goto _LL2122; _LL2126: s=({ struct Cyc_PP_Doc*(* _temp2259)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2262= Cyc_Absynpp_exp2doc(
_temp2165); struct Cyc_PP_Doc* _temp2263= Cyc_PP_text(({ unsigned char*
_temp2264=( unsigned char*)";"; struct _tagged_string _temp2265; _temp2265.curr=
_temp2264; _temp2265.base= _temp2264; _temp2265.last_plus_one= _temp2264 + 2;
_temp2265;})); struct Cyc_PP_Doc* _temp2260[ 2u]={ _temp2262, _temp2263}; struct
_tagged_ptr1 _temp2261={ _temp2260, _temp2260, _temp2260 + 2u}; _temp2259(
_temp2261);}); goto _LL2122; _LL2128: if( Cyc_Absynpp_decls_first){ if( Cyc_Absynpp_is_declaration(
_temp2169)){ s=({ struct Cyc_PP_Doc*(* _temp2266)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2269= Cyc_PP_text(({ unsigned char* _temp2288=(
unsigned char*)"{ "; struct _tagged_string _temp2289; _temp2289.curr= _temp2288;
_temp2289.base= _temp2288; _temp2289.last_plus_one= _temp2288 + 3; _temp2289;}));
struct Cyc_PP_Doc* _temp2270= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2169));
struct Cyc_PP_Doc* _temp2271= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2272=
Cyc_PP_text(({ unsigned char* _temp2286=( unsigned char*)"}"; struct
_tagged_string _temp2287; _temp2287.curr= _temp2286; _temp2287.base= _temp2286;
_temp2287.last_plus_one= _temp2286 + 2; _temp2287;})); struct Cyc_PP_Doc*
_temp2273= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2274= Cyc_Absynpp_is_declaration(
_temp2167)?({ struct Cyc_PP_Doc*(* _temp2275)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2278= Cyc_PP_text(({ unsigned char* _temp2284=(
unsigned char*)"{ "; struct _tagged_string _temp2285; _temp2285.curr= _temp2284;
_temp2285.base= _temp2284; _temp2285.last_plus_one= _temp2284 + 3; _temp2285;}));
struct Cyc_PP_Doc* _temp2279= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2167));
struct Cyc_PP_Doc* _temp2280= Cyc_PP_text(({ unsigned char* _temp2282=(
unsigned char*)"}"; struct _tagged_string _temp2283; _temp2283.curr= _temp2282;
_temp2283.base= _temp2282; _temp2283.last_plus_one= _temp2282 + 2; _temp2283;}));
struct Cyc_PP_Doc* _temp2281= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2276[ 4u]={
_temp2278, _temp2279, _temp2280, _temp2281}; struct _tagged_ptr1 _temp2277={
_temp2276, _temp2276, _temp2276 + 4u}; _temp2275( _temp2277);}): Cyc_Absynpp_stmt2doc(
_temp2167); struct Cyc_PP_Doc* _temp2267[ 6u]={ _temp2269, _temp2270, _temp2271,
_temp2272, _temp2273, _temp2274}; struct _tagged_ptr1 _temp2268={ _temp2267,
_temp2267, _temp2267 + 6u}; _temp2266( _temp2268);});} else{ if( Cyc_Absynpp_is_declaration(
_temp2167)){ s=({ struct Cyc_PP_Doc*(* _temp2290)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2293= Cyc_Absynpp_stmt2doc( _temp2169); struct Cyc_PP_Doc*
_temp2294= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2295= Cyc_PP_text(({
unsigned char* _temp2301=( unsigned char*)"{ "; struct _tagged_string _temp2302;
_temp2302.curr= _temp2301; _temp2302.base= _temp2301; _temp2302.last_plus_one=
_temp2301 + 3; _temp2302;})); struct Cyc_PP_Doc* _temp2296= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp2167)); struct Cyc_PP_Doc* _temp2297= Cyc_PP_text(({ unsigned char*
_temp2299=( unsigned char*)"}"; struct _tagged_string _temp2300; _temp2300.curr=
_temp2299; _temp2300.base= _temp2299; _temp2300.last_plus_one= _temp2299 + 2;
_temp2300;})); struct Cyc_PP_Doc* _temp2298= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2291[ 6u]={ _temp2293, _temp2294, _temp2295, _temp2296, _temp2297,
_temp2298}; struct _tagged_ptr1 _temp2292={ _temp2291, _temp2291, _temp2291 + 6u};
_temp2290( _temp2292);});} else{ s=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Stmt*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,({ unsigned char* _temp2303=(
unsigned char*)""; struct _tagged_string _temp2304; _temp2304.curr= _temp2303;
_temp2304.base= _temp2303; _temp2304.last_plus_one= _temp2303 + 1; _temp2304;}),({
struct Cyc_List_List* _temp2305=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2305->hd=( void*) _temp2169; _temp2305->tl=({
struct Cyc_List_List* _temp2306=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2306->hd=( void*) _temp2167; _temp2306->tl= 0;
_temp2306;}); _temp2305;}));}}} else{ s=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Stmt*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,({ unsigned char* _temp2307=(
unsigned char*)""; struct _tagged_string _temp2308; _temp2308.curr= _temp2307;
_temp2308.base= _temp2307; _temp2308.last_plus_one= _temp2307 + 1; _temp2308;}),({
struct Cyc_List_List* _temp2309=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2309->hd=( void*) _temp2169; _temp2309->tl=({
struct Cyc_List_List* _temp2310=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2310->hd=( void*) _temp2167; _temp2310->tl= 0;
_temp2310;}); _temp2309;}));} goto _LL2122; _LL2130: if( _temp2171 == 0){ s= Cyc_PP_text(({
unsigned char* _temp2311=( unsigned char*)"return;"; struct _tagged_string
_temp2312; _temp2312.curr= _temp2311; _temp2312.base= _temp2311; _temp2312.last_plus_one=
_temp2311 + 8; _temp2312;}));} else{ s=({ struct Cyc_PP_Doc*(* _temp2313)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2316= Cyc_PP_text(({
unsigned char* _temp2321=( unsigned char*)"return "; struct _tagged_string
_temp2322; _temp2322.curr= _temp2321; _temp2322.base= _temp2321; _temp2322.last_plus_one=
_temp2321 + 8; _temp2322;})); struct Cyc_PP_Doc* _temp2317= _temp2171 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp2171)); struct
Cyc_PP_Doc* _temp2318= Cyc_PP_text(({ unsigned char* _temp2319=( unsigned char*)";";
struct _tagged_string _temp2320; _temp2320.curr= _temp2319; _temp2320.base=
_temp2319; _temp2320.last_plus_one= _temp2319 + 2; _temp2320;})); struct Cyc_PP_Doc*
_temp2314[ 3u]={ _temp2316, _temp2317, _temp2318}; struct _tagged_ptr1 _temp2315={
_temp2314, _temp2314, _temp2314 + 3u}; _temp2313( _temp2315);});} goto _LL2122;
_LL2132: { int print_else;{ void* _temp2323=( void*) _temp2173->r; _LL2325: if(
_temp2323 ==( void*) Cyc_Absyn_Skip_s){ goto _LL2326;} else{ goto _LL2327;}
_LL2327: goto _LL2328; _LL2326: print_else= 0; goto _LL2324; _LL2328: print_else=
1; goto _LL2324; _LL2324:;} s=({ struct Cyc_PP_Doc*(* _temp2329)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2332= Cyc_PP_text(({
unsigned char* _temp2365=( unsigned char*)"if ("; struct _tagged_string
_temp2366; _temp2366.curr= _temp2365; _temp2366.base= _temp2365; _temp2366.last_plus_one=
_temp2365 + 5; _temp2366;})); struct Cyc_PP_Doc* _temp2333= Cyc_Absynpp_exp2doc(
_temp2177); struct Cyc_PP_Doc* _temp2334= Cyc_PP_text(({ unsigned char*
_temp2363=( unsigned char*)") {"; struct _tagged_string _temp2364; _temp2364.curr=
_temp2363; _temp2364.base= _temp2363; _temp2364.last_plus_one= _temp2363 + 4;
_temp2364;})); struct Cyc_PP_Doc* _temp2335= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(*
_temp2358)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2361= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2362= Cyc_Absynpp_stmt2doc( _temp2175); struct Cyc_PP_Doc*
_temp2359[ 2u]={ _temp2361, _temp2362}; struct _tagged_ptr1 _temp2360={
_temp2359, _temp2359, _temp2359 + 2u}; _temp2358( _temp2360);})); struct Cyc_PP_Doc*
_temp2336= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2337= Cyc_PP_text(({
unsigned char* _temp2356=( unsigned char*)"}"; struct _tagged_string _temp2357;
_temp2357.curr= _temp2356; _temp2357.base= _temp2356; _temp2357.last_plus_one=
_temp2356 + 2; _temp2357;})); struct Cyc_PP_Doc* _temp2338= print_else?({ struct
Cyc_PP_Doc*(* _temp2339)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2342= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2343= Cyc_PP_text(({
unsigned char* _temp2354=( unsigned char*)"else {"; struct _tagged_string
_temp2355; _temp2355.curr= _temp2354; _temp2355.base= _temp2354; _temp2355.last_plus_one=
_temp2354 + 7; _temp2355;})); struct Cyc_PP_Doc* _temp2344= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc*(* _temp2349)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2352= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2353= Cyc_Absynpp_stmt2doc(
_temp2173); struct Cyc_PP_Doc* _temp2350[ 2u]={ _temp2352, _temp2353}; struct
_tagged_ptr1 _temp2351={ _temp2350, _temp2350, _temp2350 + 2u}; _temp2349(
_temp2351);})); struct Cyc_PP_Doc* _temp2345= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2346= Cyc_PP_text(({ unsigned char* _temp2347=( unsigned char*)"}"; struct
_tagged_string _temp2348; _temp2348.curr= _temp2347; _temp2348.base= _temp2347;
_temp2348.last_plus_one= _temp2347 + 2; _temp2348;})); struct Cyc_PP_Doc*
_temp2340[ 5u]={ _temp2342, _temp2343, _temp2344, _temp2345, _temp2346}; struct
_tagged_ptr1 _temp2341={ _temp2340, _temp2340, _temp2340 + 5u}; _temp2339(
_temp2341);}): Cyc_PP_nil_doc(); struct Cyc_PP_Doc* _temp2330[ 7u]={ _temp2332,
_temp2333, _temp2334, _temp2335, _temp2336, _temp2337, _temp2338}; struct
_tagged_ptr1 _temp2331={ _temp2330, _temp2330, _temp2330 + 7u}; _temp2329(
_temp2331);}); goto _LL2122;} _LL2134: s=({ struct Cyc_PP_Doc*(* _temp2367)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2370= Cyc_PP_text(({
unsigned char* _temp2385=( unsigned char*)"while ("; struct _tagged_string
_temp2386; _temp2386.curr= _temp2385; _temp2386.base= _temp2385; _temp2386.last_plus_one=
_temp2385 + 8; _temp2386;})); struct Cyc_PP_Doc* _temp2371= Cyc_Absynpp_exp2doc(
_temp2185); struct Cyc_PP_Doc* _temp2372= Cyc_PP_text(({ unsigned char*
_temp2383=( unsigned char*)") {"; struct _tagged_string _temp2384; _temp2384.curr=
_temp2383; _temp2384.base= _temp2383; _temp2384.last_plus_one= _temp2383 + 4;
_temp2384;})); struct Cyc_PP_Doc* _temp2373= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(*
_temp2378)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2381= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2382= Cyc_Absynpp_stmt2doc( _temp2179); struct Cyc_PP_Doc*
_temp2379[ 2u]={ _temp2381, _temp2382}; struct _tagged_ptr1 _temp2380={
_temp2379, _temp2379, _temp2379 + 2u}; _temp2378( _temp2380);})); struct Cyc_PP_Doc*
_temp2374= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2375= Cyc_PP_text(({
unsigned char* _temp2376=( unsigned char*)"}"; struct _tagged_string _temp2377;
_temp2377.curr= _temp2376; _temp2377.base= _temp2376; _temp2377.last_plus_one=
_temp2376 + 2; _temp2377;})); struct Cyc_PP_Doc* _temp2368[ 6u]={ _temp2370,
_temp2371, _temp2372, _temp2373, _temp2374, _temp2375}; struct _tagged_ptr1
_temp2369={ _temp2368, _temp2368, _temp2368 + 6u}; _temp2367( _temp2369);});
goto _LL2122; _LL2136: s= Cyc_PP_text(({ unsigned char* _temp2387=(
unsigned char*)"break;"; struct _tagged_string _temp2388; _temp2388.curr=
_temp2387; _temp2388.base= _temp2387; _temp2388.last_plus_one= _temp2387 + 7;
_temp2388;})); goto _LL2122; _LL2138: s= Cyc_PP_text(({ unsigned char* _temp2389=(
unsigned char*)"continue;"; struct _tagged_string _temp2390; _temp2390.curr=
_temp2389; _temp2390.base= _temp2389; _temp2390.last_plus_one= _temp2389 + 10;
_temp2390;})); goto _LL2122; _LL2140: s= Cyc_PP_text(( struct _tagged_string)({
struct _tagged_string _temp2391=* _temp2193; xprintf("goto %.*s;", _temp2391.last_plus_one
- _temp2391.curr, _temp2391.curr);})); goto _LL2122; _LL2142: s=({ struct Cyc_PP_Doc*(*
_temp2392)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2395= Cyc_PP_text(({
unsigned char* _temp2418=( unsigned char*)"for("; struct _tagged_string
_temp2419; _temp2419.curr= _temp2418; _temp2419.base= _temp2418; _temp2419.last_plus_one=
_temp2418 + 5; _temp2419;})); struct Cyc_PP_Doc* _temp2396= Cyc_Absynpp_exp2doc(
_temp2209); struct Cyc_PP_Doc* _temp2397= Cyc_PP_text(({ unsigned char*
_temp2416=( unsigned char*)"; "; struct _tagged_string _temp2417; _temp2417.curr=
_temp2416; _temp2417.base= _temp2416; _temp2417.last_plus_one= _temp2416 + 3;
_temp2417;})); struct Cyc_PP_Doc* _temp2398= Cyc_Absynpp_exp2doc( _temp2207);
struct Cyc_PP_Doc* _temp2399= Cyc_PP_text(({ unsigned char* _temp2414=(
unsigned char*)"; "; struct _tagged_string _temp2415; _temp2415.curr= _temp2414;
_temp2415.base= _temp2414; _temp2415.last_plus_one= _temp2414 + 3; _temp2415;}));
struct Cyc_PP_Doc* _temp2400= Cyc_Absynpp_exp2doc( _temp2201); struct Cyc_PP_Doc*
_temp2401= Cyc_PP_text(({ unsigned char* _temp2412=( unsigned char*)") {";
struct _tagged_string _temp2413; _temp2413.curr= _temp2412; _temp2413.base=
_temp2412; _temp2413.last_plus_one= _temp2412 + 4; _temp2413;})); struct Cyc_PP_Doc*
_temp2402= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2407)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2410= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2411= Cyc_Absynpp_stmt2doc( _temp2195); struct Cyc_PP_Doc*
_temp2408[ 2u]={ _temp2410, _temp2411}; struct _tagged_ptr1 _temp2409={
_temp2408, _temp2408, _temp2408 + 2u}; _temp2407( _temp2409);})); struct Cyc_PP_Doc*
_temp2403= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2404= Cyc_PP_text(({
unsigned char* _temp2405=( unsigned char*)"}"; struct _tagged_string _temp2406;
_temp2406.curr= _temp2405; _temp2406.base= _temp2405; _temp2406.last_plus_one=
_temp2405 + 2; _temp2406;})); struct Cyc_PP_Doc* _temp2393[ 10u]={ _temp2395,
_temp2396, _temp2397, _temp2398, _temp2399, _temp2400, _temp2401, _temp2402,
_temp2403, _temp2404}; struct _tagged_ptr1 _temp2394={ _temp2393, _temp2393,
_temp2393 + 10u}; _temp2392( _temp2394);}); goto _LL2122; _LL2144: s=({ struct
Cyc_PP_Doc*(* _temp2420)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2423= Cyc_PP_text(({ unsigned char* _temp2434=( unsigned char*)"switch (";
struct _tagged_string _temp2435; _temp2435.curr= _temp2434; _temp2435.base=
_temp2434; _temp2435.last_plus_one= _temp2434 + 9; _temp2435;})); struct Cyc_PP_Doc*
_temp2424= Cyc_Absynpp_exp2doc( _temp2213); struct Cyc_PP_Doc* _temp2425= Cyc_PP_text(({
unsigned char* _temp2432=( unsigned char*)") {"; struct _tagged_string _temp2433;
_temp2433.curr= _temp2432; _temp2433.base= _temp2432; _temp2433.last_plus_one=
_temp2432 + 4; _temp2433;})); struct Cyc_PP_Doc* _temp2426= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2427= Cyc_Absynpp_switchclauses2doc( _temp2211); struct
Cyc_PP_Doc* _temp2428= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2429= Cyc_PP_text(({
unsigned char* _temp2430=( unsigned char*)"}"; struct _tagged_string _temp2431;
_temp2431.curr= _temp2430; _temp2431.base= _temp2430; _temp2431.last_plus_one=
_temp2430 + 2; _temp2431;})); struct Cyc_PP_Doc* _temp2421[ 7u]={ _temp2423,
_temp2424, _temp2425, _temp2426, _temp2427, _temp2428, _temp2429}; struct
_tagged_ptr1 _temp2422={ _temp2421, _temp2421, _temp2421 + 7u}; _temp2420(
_temp2422);}); goto _LL2122; _LL2146: s=({ struct Cyc_PP_Doc*(* _temp2436)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2439= Cyc_PP_text(({
unsigned char* _temp2450=( unsigned char*)"switch \"C\" ("; struct
_tagged_string _temp2451; _temp2451.curr= _temp2450; _temp2451.base= _temp2450;
_temp2451.last_plus_one= _temp2450 + 13; _temp2451;})); struct Cyc_PP_Doc*
_temp2440= Cyc_Absynpp_exp2doc( _temp2217); struct Cyc_PP_Doc* _temp2441= Cyc_PP_text(({
unsigned char* _temp2448=( unsigned char*)") {"; struct _tagged_string _temp2449;
_temp2449.curr= _temp2448; _temp2449.base= _temp2448; _temp2449.last_plus_one=
_temp2448 + 4; _temp2449;})); struct Cyc_PP_Doc* _temp2442= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2443= Cyc_Absynpp_switchCclauses2doc( _temp2215); struct
Cyc_PP_Doc* _temp2444= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2445= Cyc_PP_text(({
unsigned char* _temp2446=( unsigned char*)"}"; struct _tagged_string _temp2447;
_temp2447.curr= _temp2446; _temp2447.base= _temp2446; _temp2447.last_plus_one=
_temp2446 + 2; _temp2447;})); struct Cyc_PP_Doc* _temp2437[ 7u]={ _temp2439,
_temp2440, _temp2441, _temp2442, _temp2443, _temp2444, _temp2445}; struct
_tagged_ptr1 _temp2438={ _temp2437, _temp2437, _temp2437 + 7u}; _temp2436(
_temp2438);}); goto _LL2122; _LL2148: s= Cyc_PP_text(({ unsigned char* _temp2452=(
unsigned char*)"fallthru;"; struct _tagged_string _temp2453; _temp2453.curr=
_temp2452; _temp2453.base= _temp2452; _temp2453.last_plus_one= _temp2452 + 10;
_temp2453;})); goto _LL2122; _LL2150: s=({ struct Cyc_PP_Doc*(* _temp2454)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2457= Cyc_PP_text(({
unsigned char* _temp2462=( unsigned char*)"fallthru("; struct _tagged_string
_temp2463; _temp2463.curr= _temp2462; _temp2463.base= _temp2462; _temp2463.last_plus_one=
_temp2462 + 10; _temp2463;})); struct Cyc_PP_Doc* _temp2458= Cyc_Absynpp_exps2doc_prec(
20, _temp2225); struct Cyc_PP_Doc* _temp2459= Cyc_PP_text(({ unsigned char*
_temp2460=( unsigned char*)");"; struct _tagged_string _temp2461; _temp2461.curr=
_temp2460; _temp2461.base= _temp2460; _temp2461.last_plus_one= _temp2460 + 3;
_temp2461;})); struct Cyc_PP_Doc* _temp2455[ 3u]={ _temp2457, _temp2458,
_temp2459}; struct _tagged_ptr1 _temp2456={ _temp2455, _temp2455, _temp2455 + 3u};
_temp2454( _temp2456);}); goto _LL2122; _LL2152: s=({ struct Cyc_PP_Doc*(*
_temp2464)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2467= Cyc_Absynpp_decl2doc(
_temp2229); struct Cyc_PP_Doc* _temp2468= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2469= Cyc_Absynpp_stmt2doc( _temp2227); struct Cyc_PP_Doc* _temp2465[ 3u]={
_temp2467, _temp2468, _temp2469}; struct _tagged_ptr1 _temp2466={ _temp2465,
_temp2465, _temp2465 + 3u}; _temp2464( _temp2466);}); goto _LL2122; _LL2154: s=({
struct Cyc_PP_Doc*(* _temp2470)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2473= Cyc_PP_text(({ unsigned char* _temp2475=( unsigned char*)"cut ";
struct _tagged_string _temp2476; _temp2476.curr= _temp2475; _temp2476.base=
_temp2475; _temp2476.last_plus_one= _temp2475 + 5; _temp2476;})); struct Cyc_PP_Doc*
_temp2474= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2231)); struct Cyc_PP_Doc*
_temp2471[ 2u]={ _temp2473, _temp2474}; struct _tagged_ptr1 _temp2472={
_temp2471, _temp2471, _temp2471 + 2u}; _temp2470( _temp2472);}); goto _LL2122;
_LL2156: s=({ struct Cyc_PP_Doc*(* _temp2477)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2480= Cyc_PP_text(({ unsigned char* _temp2482=(
unsigned char*)"splice "; struct _tagged_string _temp2483; _temp2483.curr=
_temp2482; _temp2483.base= _temp2482; _temp2483.last_plus_one= _temp2482 + 8;
_temp2483;})); struct Cyc_PP_Doc* _temp2481= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp2233)); struct Cyc_PP_Doc* _temp2478[ 2u]={ _temp2480, _temp2481}; struct
_tagged_ptr1 _temp2479={ _temp2478, _temp2478, _temp2478 + 2u}; _temp2477(
_temp2479);}); goto _LL2122; _LL2158: if( Cyc_Absynpp_decls_first? Cyc_Absynpp_is_declaration(
_temp2235): 0){ s=({ struct Cyc_PP_Doc*(* _temp2484)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2487= Cyc_PP_textptr( _temp2237); struct Cyc_PP_Doc*
_temp2488= Cyc_PP_text(({ unsigned char* _temp2495=( unsigned char*)": {";
struct _tagged_string _temp2496; _temp2496.curr= _temp2495; _temp2496.base=
_temp2495; _temp2496.last_plus_one= _temp2495 + 4; _temp2496;})); struct Cyc_PP_Doc*
_temp2489= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2490= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp2235)); struct Cyc_PP_Doc* _temp2491= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2492= Cyc_PP_text(({ unsigned char* _temp2493=( unsigned char*)"}"; struct
_tagged_string _temp2494; _temp2494.curr= _temp2493; _temp2494.base= _temp2493;
_temp2494.last_plus_one= _temp2493 + 2; _temp2494;})); struct Cyc_PP_Doc*
_temp2485[ 6u]={ _temp2487, _temp2488, _temp2489, _temp2490, _temp2491,
_temp2492}; struct _tagged_ptr1 _temp2486={ _temp2485, _temp2485, _temp2485 + 6u};
_temp2484( _temp2486);});} else{ s=({ struct Cyc_PP_Doc*(* _temp2497)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2500= Cyc_PP_textptr(
_temp2237); struct Cyc_PP_Doc* _temp2501= Cyc_PP_text(({ unsigned char*
_temp2503=( unsigned char*)": "; struct _tagged_string _temp2504; _temp2504.curr=
_temp2503; _temp2504.base= _temp2503; _temp2504.last_plus_one= _temp2503 + 3;
_temp2504;})); struct Cyc_PP_Doc* _temp2502= Cyc_Absynpp_stmt2doc( _temp2235);
struct Cyc_PP_Doc* _temp2498[ 3u]={ _temp2500, _temp2501, _temp2502}; struct
_tagged_ptr1 _temp2499={ _temp2498, _temp2498, _temp2498 + 3u}; _temp2497(
_temp2499);});} goto _LL2122; _LL2160: s=({ struct Cyc_PP_Doc*(* _temp2505)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2508= Cyc_PP_text(({
unsigned char* _temp2519=( unsigned char*)"do {"; struct _tagged_string
_temp2520; _temp2520.curr= _temp2519; _temp2520.base= _temp2519; _temp2520.last_plus_one=
_temp2519 + 5; _temp2520;})); struct Cyc_PP_Doc* _temp2509= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2510= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2245));
struct Cyc_PP_Doc* _temp2511= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2512=
Cyc_PP_text(({ unsigned char* _temp2517=( unsigned char*)"} while ("; struct
_tagged_string _temp2518; _temp2518.curr= _temp2517; _temp2518.base= _temp2517;
_temp2518.last_plus_one= _temp2517 + 10; _temp2518;})); struct Cyc_PP_Doc*
_temp2513= Cyc_Absynpp_exp2doc( _temp2243); struct Cyc_PP_Doc* _temp2514= Cyc_PP_text(({
unsigned char* _temp2515=( unsigned char*)");"; struct _tagged_string _temp2516;
_temp2516.curr= _temp2515; _temp2516.base= _temp2515; _temp2516.last_plus_one=
_temp2515 + 3; _temp2516;})); struct Cyc_PP_Doc* _temp2506[ 7u]={ _temp2508,
_temp2509, _temp2510, _temp2511, _temp2512, _temp2513, _temp2514}; struct
_tagged_ptr1 _temp2507={ _temp2506, _temp2506, _temp2506 + 7u}; _temp2505(
_temp2507);}); goto _LL2122; _LL2162: s=({ struct Cyc_PP_Doc*(* _temp2521)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2524= Cyc_PP_text(({
unsigned char* _temp2537=( unsigned char*)"try {"; struct _tagged_string
_temp2538; _temp2538.curr= _temp2537; _temp2538.base= _temp2537; _temp2538.last_plus_one=
_temp2537 + 6; _temp2538;})); struct Cyc_PP_Doc* _temp2525= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2526= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2249));
struct Cyc_PP_Doc* _temp2527= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2528=
Cyc_PP_text(({ unsigned char* _temp2535=( unsigned char*)"} catch {"; struct
_tagged_string _temp2536; _temp2536.curr= _temp2535; _temp2536.base= _temp2535;
_temp2536.last_plus_one= _temp2535 + 10; _temp2536;})); struct Cyc_PP_Doc*
_temp2529= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2530= Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc(
_temp2247)); struct Cyc_PP_Doc* _temp2531= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2532= Cyc_PP_text(({ unsigned char* _temp2533=( unsigned char*)"}"; struct
_tagged_string _temp2534; _temp2534.curr= _temp2533; _temp2534.base= _temp2533;
_temp2534.last_plus_one= _temp2533 + 2; _temp2534;})); struct Cyc_PP_Doc*
_temp2522[ 9u]={ _temp2524, _temp2525, _temp2526, _temp2527, _temp2528,
_temp2529, _temp2530, _temp2531, _temp2532}; struct _tagged_ptr1 _temp2523={
_temp2522, _temp2522, _temp2522 + 9u}; _temp2521( _temp2523);}); goto _LL2122;
_LL2164: s=({ struct Cyc_PP_Doc*(* _temp2539)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2542= Cyc_PP_text(({ unsigned char* _temp2557=(
unsigned char*)"region<"; struct _tagged_string _temp2558; _temp2558.curr=
_temp2557; _temp2558.base= _temp2557; _temp2558.last_plus_one= _temp2557 + 8;
_temp2558;})); struct Cyc_PP_Doc* _temp2543= Cyc_PP_textptr( Cyc_Absynpp_get_name(
_temp2255)); struct Cyc_PP_Doc* _temp2544= Cyc_PP_text(({ unsigned char*
_temp2555=( unsigned char*)">"; struct _tagged_string _temp2556; _temp2556.curr=
_temp2555; _temp2556.base= _temp2555; _temp2556.last_plus_one= _temp2555 + 2;
_temp2556;})); struct Cyc_PP_Doc* _temp2545= Cyc_Absynpp_qvar2doc( _temp2253->name);
struct Cyc_PP_Doc* _temp2546= Cyc_PP_text(({ unsigned char* _temp2553=(
unsigned char*)"{"; struct _tagged_string _temp2554; _temp2554.curr= _temp2553;
_temp2554.base= _temp2553; _temp2554.last_plus_one= _temp2553 + 2; _temp2554;}));
struct Cyc_PP_Doc* _temp2547= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2548=
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2251)); struct Cyc_PP_Doc* _temp2549=
Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2550= Cyc_PP_text(({ unsigned char*
_temp2551=( unsigned char*)"}"; struct _tagged_string _temp2552; _temp2552.curr=
_temp2551; _temp2552.base= _temp2551; _temp2552.last_plus_one= _temp2551 + 2;
_temp2552;})); struct Cyc_PP_Doc* _temp2540[ 9u]={ _temp2542, _temp2543,
_temp2544, _temp2545, _temp2546, _temp2547, _temp2548, _temp2549, _temp2550};
struct _tagged_ptr1 _temp2541={ _temp2540, _temp2540, _temp2540 + 9u}; _temp2539(
_temp2541);}); goto _LL2122; _LL2122:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void* _temp2559=( void*) p->r;
int _temp2591; void* _temp2593; unsigned char _temp2595; struct _tagged_string
_temp2597; struct Cyc_Absyn_Vardecl* _temp2599; struct Cyc_List_List* _temp2601;
struct Cyc_Absyn_Pat* _temp2603; struct Cyc_Absyn_Vardecl* _temp2605; struct
_tuple0* _temp2607; struct Cyc_List_List* _temp2609; struct Cyc_List_List*
_temp2611; struct _tuple0* _temp2613; struct Cyc_List_List* _temp2615; struct
Cyc_List_List* _temp2617; struct _tuple0* _temp2619; struct Cyc_List_List*
_temp2621; struct Cyc_List_List* _temp2623; struct Cyc_Core_Opt* _temp2625;
struct Cyc_Absyn_Structdecl* _temp2627; struct Cyc_Absyn_Enumfield* _temp2629;
struct Cyc_Absyn_Enumdecl* _temp2631; struct Cyc_List_List* _temp2633; struct
Cyc_List_List* _temp2635; struct Cyc_Absyn_Tunionfield* _temp2637; struct Cyc_Absyn_Tuniondecl*
_temp2639; _LL2561: if( _temp2559 ==( void*) Cyc_Absyn_Wild_p){ goto _LL2562;}
else{ goto _LL2563;} _LL2563: if( _temp2559 ==( void*) Cyc_Absyn_Null_p){ goto
_LL2564;} else{ goto _LL2565;} _LL2565: if(( unsigned int) _temp2559 > 2u?*((
int*) _temp2559) == Cyc_Absyn_Int_p: 0){ _LL2594: _temp2593=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp2559)->f1; goto _LL2592; _LL2592: _temp2591=((
struct Cyc_Absyn_Int_p_struct*) _temp2559)->f2; goto _LL2566;} else{ goto
_LL2567;} _LL2567: if(( unsigned int) _temp2559 > 2u?*(( int*) _temp2559) == Cyc_Absyn_Char_p:
0){ _LL2596: _temp2595=(( struct Cyc_Absyn_Char_p_struct*) _temp2559)->f1; goto
_LL2568;} else{ goto _LL2569;} _LL2569: if(( unsigned int) _temp2559 > 2u?*((
int*) _temp2559) == Cyc_Absyn_Float_p: 0){ _LL2598: _temp2597=(( struct Cyc_Absyn_Float_p_struct*)
_temp2559)->f1; goto _LL2570;} else{ goto _LL2571;} _LL2571: if(( unsigned int)
_temp2559 > 2u?*(( int*) _temp2559) == Cyc_Absyn_Var_p: 0){ _LL2600: _temp2599=((
struct Cyc_Absyn_Var_p_struct*) _temp2559)->f1; goto _LL2572;} else{ goto
_LL2573;} _LL2573: if(( unsigned int) _temp2559 > 2u?*(( int*) _temp2559) == Cyc_Absyn_Tuple_p:
0){ _LL2602: _temp2601=(( struct Cyc_Absyn_Tuple_p_struct*) _temp2559)->f1; goto
_LL2574;} else{ goto _LL2575;} _LL2575: if(( unsigned int) _temp2559 > 2u?*((
int*) _temp2559) == Cyc_Absyn_Pointer_p: 0){ _LL2604: _temp2603=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2559)->f1; goto _LL2576;} else{ goto _LL2577;} _LL2577: if(( unsigned int)
_temp2559 > 2u?*(( int*) _temp2559) == Cyc_Absyn_Reference_p: 0){ _LL2606:
_temp2605=(( struct Cyc_Absyn_Reference_p_struct*) _temp2559)->f1; goto _LL2578;}
else{ goto _LL2579;} _LL2579: if(( unsigned int) _temp2559 > 2u?*(( int*)
_temp2559) == Cyc_Absyn_UnknownId_p: 0){ _LL2608: _temp2607=(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp2559)->f1; goto _LL2580;} else{ goto _LL2581;} _LL2581: if(( unsigned int)
_temp2559 > 2u?*(( int*) _temp2559) == Cyc_Absyn_UnknownCall_p: 0){ _LL2614:
_temp2613=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp2559)->f1; goto
_LL2612; _LL2612: _temp2611=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp2559)->f2;
goto _LL2610; _LL2610: _temp2609=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2559)->f3; goto _LL2582;} else{ goto _LL2583;} _LL2583: if(( unsigned int)
_temp2559 > 2u?*(( int*) _temp2559) == Cyc_Absyn_UnknownFields_p: 0){ _LL2620:
_temp2619=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp2559)->f1; goto
_LL2618; _LL2618: _temp2617=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2559)->f2; goto _LL2616; _LL2616: _temp2615=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2559)->f3; goto _LL2584;} else{ goto _LL2585;} _LL2585: if(( unsigned int)
_temp2559 > 2u?*(( int*) _temp2559) == Cyc_Absyn_Struct_p: 0){ _LL2628:
_temp2627=(( struct Cyc_Absyn_Struct_p_struct*) _temp2559)->f1; goto _LL2626;
_LL2626: _temp2625=(( struct Cyc_Absyn_Struct_p_struct*) _temp2559)->f2; goto
_LL2624; _LL2624: _temp2623=(( struct Cyc_Absyn_Struct_p_struct*) _temp2559)->f3;
goto _LL2622; _LL2622: _temp2621=(( struct Cyc_Absyn_Struct_p_struct*) _temp2559)->f4;
goto _LL2586;} else{ goto _LL2587;} _LL2587: if(( unsigned int) _temp2559 > 2u?*((
int*) _temp2559) == Cyc_Absyn_Enum_p: 0){ _LL2632: _temp2631=(( struct Cyc_Absyn_Enum_p_struct*)
_temp2559)->f1; goto _LL2630; _LL2630: _temp2629=(( struct Cyc_Absyn_Enum_p_struct*)
_temp2559)->f2; goto _LL2588;} else{ goto _LL2589;} _LL2589: if(( unsigned int)
_temp2559 > 2u?*(( int*) _temp2559) == Cyc_Absyn_Tunion_p: 0){ _LL2640:
_temp2639=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2559)->f1; goto _LL2638;
_LL2638: _temp2637=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2559)->f2; goto
_LL2636; _LL2636: _temp2635=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2559)->f3;
goto _LL2634; _LL2634: _temp2633=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2559)->f4;
goto _LL2590;} else{ goto _LL2560;} _LL2562: s= Cyc_PP_text(({ unsigned char*
_temp2641=( unsigned char*)"_"; struct _tagged_string _temp2642; _temp2642.curr=
_temp2641; _temp2642.base= _temp2641; _temp2642.last_plus_one= _temp2641 + 2;
_temp2642;})); goto _LL2560; _LL2564: s= Cyc_PP_text(({ unsigned char* _temp2643=(
unsigned char*)"null"; struct _tagged_string _temp2644; _temp2644.curr=
_temp2643; _temp2644.base= _temp2643; _temp2644.last_plus_one= _temp2643 + 5;
_temp2644;})); goto _LL2560; _LL2566: if( _temp2593 ==( void*) Cyc_Absyn_Signed){
s= Cyc_PP_text(( struct _tagged_string) xprintf("%d", _temp2591));} else{ s= Cyc_PP_text((
struct _tagged_string) xprintf("%u",( unsigned int) _temp2591));} goto _LL2560;
_LL2568: s= Cyc_PP_text(( struct _tagged_string)({ struct _tagged_string
_temp2645= Cyc_Absynpp_char_escape( _temp2595); xprintf("'%.*s'", _temp2645.last_plus_one
- _temp2645.curr, _temp2645.curr);})); goto _LL2560; _LL2570: s= Cyc_PP_text(
_temp2597); goto _LL2560; _LL2572: s= Cyc_Absynpp_qvar2doc( _temp2599->name);
goto _LL2560; _LL2574: s=({ struct Cyc_PP_Doc*(* _temp2646)( struct _tagged_ptr1)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp2649= Cyc_PP_text(({ unsigned char*
_temp2656=( unsigned char*)"$("; struct _tagged_string _temp2657; _temp2657.curr=
_temp2656; _temp2657.base= _temp2656; _temp2657.last_plus_one= _temp2656 + 3;
_temp2657;})); struct Cyc_PP_Doc* _temp2650=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Pat*), struct _tagged_string sep, struct Cyc_List_List* l0))
Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc,({ unsigned char* _temp2654=( unsigned char*)",";
struct _tagged_string _temp2655; _temp2655.curr= _temp2654; _temp2655.base=
_temp2654; _temp2655.last_plus_one= _temp2654 + 2; _temp2655;}), _temp2601);
struct Cyc_PP_Doc* _temp2651= Cyc_PP_text(({ unsigned char* _temp2652=(
unsigned char*)")"; struct _tagged_string _temp2653; _temp2653.curr= _temp2652;
_temp2653.base= _temp2652; _temp2653.last_plus_one= _temp2652 + 2; _temp2653;}));
struct Cyc_PP_Doc* _temp2647[ 3u]={ _temp2649, _temp2650, _temp2651}; struct
_tagged_ptr1 _temp2648={ _temp2647, _temp2647, _temp2647 + 3u}; _temp2646(
_temp2648);}); goto _LL2560; _LL2576: s=({ struct Cyc_PP_Doc*(* _temp2658)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2661= Cyc_PP_text(({
unsigned char* _temp2663=( unsigned char*)"&"; struct _tagged_string _temp2664;
_temp2664.curr= _temp2663; _temp2664.base= _temp2663; _temp2664.last_plus_one=
_temp2663 + 2; _temp2664;})); struct Cyc_PP_Doc* _temp2662= Cyc_Absynpp_pat2doc(
_temp2603); struct Cyc_PP_Doc* _temp2659[ 2u]={ _temp2661, _temp2662}; struct
_tagged_ptr1 _temp2660={ _temp2659, _temp2659, _temp2659 + 2u}; _temp2658(
_temp2660);}); goto _LL2560; _LL2578: s=({ struct Cyc_PP_Doc*(* _temp2665)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2668= Cyc_PP_text(({
unsigned char* _temp2670=( unsigned char*)"*"; struct _tagged_string _temp2671;
_temp2671.curr= _temp2670; _temp2671.base= _temp2670; _temp2671.last_plus_one=
_temp2670 + 2; _temp2671;})); struct Cyc_PP_Doc* _temp2669= Cyc_Absynpp_qvar2doc(
_temp2605->name); struct Cyc_PP_Doc* _temp2666[ 2u]={ _temp2668, _temp2669};
struct _tagged_ptr1 _temp2667={ _temp2666, _temp2666, _temp2666 + 2u}; _temp2665(
_temp2667);}); goto _LL2560; _LL2580: s= Cyc_Absynpp_qvar2doc( _temp2607); goto
_LL2560; _LL2582: s=({ struct Cyc_PP_Doc*(* _temp2672)( struct _tagged_ptr1)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp2675= Cyc_Absynpp_qvar2doc( _temp2613);
struct Cyc_PP_Doc* _temp2676= Cyc_Absynpp_tvars2doc( _temp2611); struct Cyc_PP_Doc*
_temp2677= Cyc_PP_group(({ unsigned char* _temp2678=( unsigned char*)"("; struct
_tagged_string _temp2679; _temp2679.curr= _temp2678; _temp2679.base= _temp2678;
_temp2679.last_plus_one= _temp2678 + 2; _temp2679;}),({ unsigned char* _temp2680=(
unsigned char*)")"; struct _tagged_string _temp2681; _temp2681.curr= _temp2680;
_temp2681.base= _temp2680; _temp2681.last_plus_one= _temp2680 + 2; _temp2681;}),({
unsigned char* _temp2682=( unsigned char*)","; struct _tagged_string _temp2683;
_temp2683.curr= _temp2682; _temp2683.base= _temp2682; _temp2683.last_plus_one=
_temp2682 + 2; _temp2683;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc,
_temp2609)); struct Cyc_PP_Doc* _temp2673[ 3u]={ _temp2675, _temp2676, _temp2677};
struct _tagged_ptr1 _temp2674={ _temp2673, _temp2673, _temp2673 + 3u}; _temp2672(
_temp2674);}); goto _LL2560; _LL2584: s=({ struct Cyc_PP_Doc*(* _temp2684)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2687= Cyc_Absynpp_qvar2doc(
_temp2619); struct Cyc_PP_Doc* _temp2688= Cyc_Absynpp_tvars2doc( _temp2617);
struct Cyc_PP_Doc* _temp2689= Cyc_PP_group(({ unsigned char* _temp2690=(
unsigned char*)"{"; struct _tagged_string _temp2691; _temp2691.curr= _temp2690;
_temp2691.base= _temp2690; _temp2691.last_plus_one= _temp2690 + 2; _temp2691;}),({
unsigned char* _temp2692=( unsigned char*)"}"; struct _tagged_string _temp2693;
_temp2693.curr= _temp2692; _temp2693.base= _temp2692; _temp2693.last_plus_one=
_temp2692 + 2; _temp2693;}),({ unsigned char* _temp2694=( unsigned char*)",";
struct _tagged_string _temp2695; _temp2695.curr= _temp2694; _temp2695.base=
_temp2694; _temp2695.last_plus_one= _temp2694 + 2; _temp2695;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_dp2doc, _temp2615)); struct Cyc_PP_Doc* _temp2685[ 3u]={ _temp2687,
_temp2688, _temp2689}; struct _tagged_ptr1 _temp2686={ _temp2685, _temp2685,
_temp2685 + 3u}; _temp2684( _temp2686);}); goto _LL2560; _LL2586: s=({ struct
Cyc_PP_Doc*(* _temp2696)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2699= _temp2627->name == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp2627->name))->v); struct Cyc_PP_Doc*
_temp2700= Cyc_Absynpp_tvars2doc( _temp2623); struct Cyc_PP_Doc* _temp2701= Cyc_PP_group(({
unsigned char* _temp2702=( unsigned char*)"{"; struct _tagged_string _temp2703;
_temp2703.curr= _temp2702; _temp2703.base= _temp2702; _temp2703.last_plus_one=
_temp2702 + 2; _temp2703;}),({ unsigned char* _temp2704=( unsigned char*)"}";
struct _tagged_string _temp2705; _temp2705.curr= _temp2704; _temp2705.base=
_temp2704; _temp2705.last_plus_one= _temp2704 + 2; _temp2705;}),({ unsigned char*
_temp2706=( unsigned char*)","; struct _tagged_string _temp2707; _temp2707.curr=
_temp2706; _temp2707.base= _temp2706; _temp2707.last_plus_one= _temp2706 + 2;
_temp2707;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp2621)); struct Cyc_PP_Doc* _temp2697[ 3u]={ _temp2699, _temp2700, _temp2701};
struct _tagged_ptr1 _temp2698={ _temp2697, _temp2697, _temp2697 + 3u}; _temp2696(
_temp2698);}); goto _LL2560; _LL2588: s= Cyc_Absynpp_qvar2doc( _temp2629->name);
goto _LL2560; _LL2590: if( _temp2633 == 0){ s= Cyc_Absynpp_qvar2doc( _temp2637->name);}
else{ s=({ struct Cyc_PP_Doc*(* _temp2708)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2711= Cyc_Absynpp_qvar2doc( _temp2637->name); struct Cyc_PP_Doc*
_temp2712= Cyc_Absynpp_tvars2doc( _temp2635); struct Cyc_PP_Doc* _temp2713= Cyc_PP_egroup(({
unsigned char* _temp2714=( unsigned char*)"("; struct _tagged_string _temp2715;
_temp2715.curr= _temp2714; _temp2715.base= _temp2714; _temp2715.last_plus_one=
_temp2714 + 2; _temp2715;}),({ unsigned char* _temp2716=( unsigned char*)")";
struct _tagged_string _temp2717; _temp2717.curr= _temp2716; _temp2717.base=
_temp2716; _temp2717.last_plus_one= _temp2716 + 2; _temp2717;}),({ unsigned char*
_temp2718=( unsigned char*)","; struct _tagged_string _temp2719; _temp2719.curr=
_temp2718; _temp2719.base= _temp2718; _temp2719.last_plus_one= _temp2718 + 2;
_temp2719;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc, _temp2633));
struct Cyc_PP_Doc* _temp2709[ 3u]={ _temp2711, _temp2712, _temp2713}; struct
_tagged_ptr1 _temp2710={ _temp2709, _temp2709, _temp2709 + 3u}; _temp2708(
_temp2710);});} goto _LL2560; _LL2560:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_dp2doc(
struct _tuple5* dp){ return({ struct Cyc_PP_Doc*(* _temp2720)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2723= Cyc_PP_egroup(({
unsigned char* _temp2725=( unsigned char*)""; struct _tagged_string _temp2726;
_temp2726.curr= _temp2725; _temp2726.base= _temp2725; _temp2726.last_plus_one=
_temp2725 + 1; _temp2726;}),({ unsigned char* _temp2727=( unsigned char*)"=";
struct _tagged_string _temp2728; _temp2728.curr= _temp2727; _temp2728.base=
_temp2727; _temp2728.last_plus_one= _temp2727 + 2; _temp2728;}),({ unsigned char*
_temp2729=( unsigned char*)"="; struct _tagged_string _temp2730; _temp2730.curr=
_temp2729; _temp2730.base= _temp2729; _temp2730.last_plus_one= _temp2729 + 2;
_temp2730;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(* dp).f1));
struct Cyc_PP_Doc* _temp2724= Cyc_Absynpp_pat2doc((* dp).f2); struct Cyc_PP_Doc*
_temp2721[ 2u]={ _temp2723, _temp2724}; struct _tagged_ptr1 _temp2722={
_temp2721, _temp2721, _temp2721 + 2u}; _temp2720( _temp2722);});} struct Cyc_PP_Doc*
Cyc_Absynpp_switchclause2doc( struct Cyc_Absyn_Switch_clause* c){ if( c->where_clause
== 0?( void*)( c->pattern)->r ==( void*) Cyc_Absyn_Wild_p: 0){ return({ struct
Cyc_PP_Doc*(* _temp2731)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2734= Cyc_PP_text(({ unsigned char* _temp2741=( unsigned char*)"default: ";
struct _tagged_string _temp2742; _temp2742.curr= _temp2741; _temp2742.base=
_temp2741; _temp2742.last_plus_one= _temp2741 + 10; _temp2742;})); struct Cyc_PP_Doc*
_temp2735= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2736)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2739= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2740= Cyc_Absynpp_stmt2doc( c->body); struct Cyc_PP_Doc*
_temp2737[ 2u]={ _temp2739, _temp2740}; struct _tagged_ptr1 _temp2738={
_temp2737, _temp2737, _temp2737 + 2u}; _temp2736( _temp2738);})); struct Cyc_PP_Doc*
_temp2732[ 2u]={ _temp2734, _temp2735}; struct _tagged_ptr1 _temp2733={
_temp2732, _temp2732, _temp2732 + 2u}; _temp2731( _temp2733);});} else{ if( c->where_clause
== 0){ return({ struct Cyc_PP_Doc*(* _temp2743)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2746= Cyc_PP_text(({ unsigned char* _temp2757=(
unsigned char*)"case "; struct _tagged_string _temp2758; _temp2758.curr=
_temp2757; _temp2758.base= _temp2757; _temp2758.last_plus_one= _temp2757 + 6;
_temp2758;})); struct Cyc_PP_Doc* _temp2747= Cyc_Absynpp_pat2doc( c->pattern);
struct Cyc_PP_Doc* _temp2748= Cyc_PP_text(({ unsigned char* _temp2755=(
unsigned char*)": "; struct _tagged_string _temp2756; _temp2756.curr= _temp2755;
_temp2756.base= _temp2755; _temp2756.last_plus_one= _temp2755 + 3; _temp2756;}));
struct Cyc_PP_Doc* _temp2749= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2750)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2753= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2754= Cyc_Absynpp_stmt2doc( c->body); struct Cyc_PP_Doc*
_temp2751[ 2u]={ _temp2753, _temp2754}; struct _tagged_ptr1 _temp2752={
_temp2751, _temp2751, _temp2751 + 2u}; _temp2750( _temp2752);})); struct Cyc_PP_Doc*
_temp2744[ 4u]={ _temp2746, _temp2747, _temp2748, _temp2749}; struct
_tagged_ptr1 _temp2745={ _temp2744, _temp2744, _temp2744 + 4u}; _temp2743(
_temp2745);});} else{ return({ struct Cyc_PP_Doc*(* _temp2759)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2762= Cyc_PP_text(({
unsigned char* _temp2777=( unsigned char*)"case "; struct _tagged_string
_temp2778; _temp2778.curr= _temp2777; _temp2778.base= _temp2777; _temp2778.last_plus_one=
_temp2777 + 6; _temp2778;})); struct Cyc_PP_Doc* _temp2763= Cyc_Absynpp_pat2doc(
c->pattern); struct Cyc_PP_Doc* _temp2764= Cyc_PP_text(({ unsigned char*
_temp2775=( unsigned char*)" && "; struct _tagged_string _temp2776; _temp2776.curr=
_temp2775; _temp2776.base= _temp2775; _temp2776.last_plus_one= _temp2775 + 5;
_temp2776;})); struct Cyc_PP_Doc* _temp2765= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)
_check_null( c->where_clause)); struct Cyc_PP_Doc* _temp2766= Cyc_PP_text(({
unsigned char* _temp2773=( unsigned char*)": "; struct _tagged_string _temp2774;
_temp2774.curr= _temp2773; _temp2774.base= _temp2773; _temp2774.last_plus_one=
_temp2773 + 3; _temp2774;})); struct Cyc_PP_Doc* _temp2767= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc*(* _temp2768)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2771= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2772= Cyc_Absynpp_stmt2doc(
c->body); struct Cyc_PP_Doc* _temp2769[ 2u]={ _temp2771, _temp2772}; struct
_tagged_ptr1 _temp2770={ _temp2769, _temp2769, _temp2769 + 2u}; _temp2768(
_temp2770);})); struct Cyc_PP_Doc* _temp2760[ 6u]={ _temp2762, _temp2763,
_temp2764, _temp2765, _temp2766, _temp2767}; struct _tagged_ptr1 _temp2761={
_temp2760, _temp2760, _temp2760 + 6u}; _temp2759( _temp2761);});}}} struct Cyc_PP_Doc*
Cyc_Absynpp_switchclauses2doc( struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Switch_clause*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc,({
unsigned char* _temp2779=( unsigned char*)""; struct _tagged_string _temp2780;
_temp2780.curr= _temp2779; _temp2780.base= _temp2779; _temp2780.last_plus_one=
_temp2779 + 1; _temp2780;}), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_switchCclause2doc(
struct Cyc_Absyn_SwitchC_clause* c){ struct Cyc_Absyn_SwitchC_clause _temp2783;
struct Cyc_Position_Segment* _temp2784; struct Cyc_Absyn_Stmt* _temp2786; struct
Cyc_Absyn_Exp* _temp2788; struct Cyc_Absyn_SwitchC_clause* _temp2781= c;
_temp2783=* _temp2781; _LL2789: _temp2788= _temp2783.cnst_exp; goto _LL2787;
_LL2787: _temp2786= _temp2783.body; goto _LL2785; _LL2785: _temp2784= _temp2783.loc;
goto _LL2782; _LL2782: if( _temp2788 == 0){ return({ struct Cyc_PP_Doc*(*
_temp2790)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2793= Cyc_PP_text(({
unsigned char* _temp2800=( unsigned char*)"default: "; struct _tagged_string
_temp2801; _temp2801.curr= _temp2800; _temp2801.base= _temp2800; _temp2801.last_plus_one=
_temp2800 + 10; _temp2801;})); struct Cyc_PP_Doc* _temp2794= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc*(* _temp2795)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2798= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2799= Cyc_Absynpp_stmt2doc(
c->body); struct Cyc_PP_Doc* _temp2796[ 2u]={ _temp2798, _temp2799}; struct
_tagged_ptr1 _temp2797={ _temp2796, _temp2796, _temp2796 + 2u}; _temp2795(
_temp2797);})); struct Cyc_PP_Doc* _temp2791[ 2u]={ _temp2793, _temp2794};
struct _tagged_ptr1 _temp2792={ _temp2791, _temp2791, _temp2791 + 2u}; _temp2790(
_temp2792);});} else{ return({ struct Cyc_PP_Doc*(* _temp2802)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2805= Cyc_PP_text(({
unsigned char* _temp2816=( unsigned char*)"case "; struct _tagged_string
_temp2817; _temp2817.curr= _temp2816; _temp2817.base= _temp2816; _temp2817.last_plus_one=
_temp2816 + 6; _temp2817;})); struct Cyc_PP_Doc* _temp2806= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp2788)); struct Cyc_PP_Doc* _temp2807=
Cyc_PP_text(({ unsigned char* _temp2814=( unsigned char*)": "; struct
_tagged_string _temp2815; _temp2815.curr= _temp2814; _temp2815.base= _temp2814;
_temp2815.last_plus_one= _temp2814 + 3; _temp2815;})); struct Cyc_PP_Doc*
_temp2808= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2809)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2812= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2813= Cyc_Absynpp_stmt2doc( _temp2786); struct Cyc_PP_Doc*
_temp2810[ 2u]={ _temp2812, _temp2813}; struct _tagged_ptr1 _temp2811={
_temp2810, _temp2810, _temp2810 + 2u}; _temp2809( _temp2811);})); struct Cyc_PP_Doc*
_temp2803[ 4u]={ _temp2805, _temp2806, _temp2807, _temp2808}; struct
_tagged_ptr1 _temp2804={ _temp2803, _temp2803, _temp2803 + 4u}; _temp2802(
_temp2804);});}} struct Cyc_PP_Doc* Cyc_Absynpp_switchCclauses2doc( struct Cyc_List_List*
cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_SwitchC_clause*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchCclause2doc,({
unsigned char* _temp2818=( unsigned char*)""; struct _tagged_string _temp2819;
_temp2819.curr= _temp2818; _temp2819.base= _temp2818; _temp2819.last_plus_one=
_temp2818 + 1; _temp2819;}), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_enumfield2doc(
struct Cyc_Absyn_Enumfield* f){ if( f->tag == 0){ return Cyc_Absynpp_qvar2doc( f->name);}
else{ return({ struct Cyc_PP_Doc*(* _temp2820)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2823= Cyc_Absynpp_qvar2doc( f->name); struct Cyc_PP_Doc*
_temp2824= Cyc_PP_text(({ unsigned char* _temp2826=( unsigned char*)" = ";
struct _tagged_string _temp2827; _temp2827.curr= _temp2826; _temp2827.base=
_temp2826; _temp2827.last_plus_one= _temp2826 + 4; _temp2827;})); struct Cyc_PP_Doc*
_temp2825= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( f->tag));
struct Cyc_PP_Doc* _temp2821[ 3u]={ _temp2823, _temp2824, _temp2825}; struct
_tagged_ptr1 _temp2822={ _temp2821, _temp2821, _temp2821 + 3u}; _temp2820(
_temp2822);});}} struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc( struct Cyc_List_List*
fs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Enumfield*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc,({
unsigned char* _temp2828=( unsigned char*)","; struct _tagged_string _temp2829;
_temp2829.curr= _temp2828; _temp2829.base= _temp2828; _temp2829.last_plus_one=
_temp2828 + 2; _temp2829;}), fs);} static struct Cyc_PP_Doc* Cyc_Absynpp_id2doc(
struct Cyc_Absyn_Vardecl* v){ return Cyc_Absynpp_qvar2doc( v->name);} static
struct Cyc_PP_Doc* Cyc_Absynpp_ids2doc( struct Cyc_List_List* vds){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Vardecl*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseq)( Cyc_Absynpp_id2doc,({
unsigned char* _temp2830=( unsigned char*)","; struct _tagged_string _temp2831;
_temp2831.curr= _temp2830; _temp2831.base= _temp2830; _temp2831.last_plus_one=
_temp2830 + 2; _temp2831;}), vds);} struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl* d){ struct Cyc_PP_Doc* s;{ void* _temp2832=( void*) d->r;
struct Cyc_Absyn_Fndecl* _temp2858; struct Cyc_Absyn_Structdecl* _temp2860;
struct Cyc_Absyn_Uniondecl* _temp2862; struct Cyc_Absyn_Vardecl* _temp2864;
struct Cyc_Absyn_Vardecl _temp2866; struct Cyc_List_List* _temp2867; struct Cyc_Core_Opt*
_temp2869; struct Cyc_Absyn_Exp* _temp2871; void* _temp2873; struct Cyc_Absyn_Tqual
_temp2875; struct _tuple0* _temp2877; void* _temp2879; struct Cyc_Absyn_Tuniondecl*
_temp2881; struct Cyc_Absyn_Tuniondecl _temp2883; int _temp2884; struct Cyc_Core_Opt*
_temp2886; struct Cyc_List_List* _temp2888; struct _tuple0* _temp2890; void*
_temp2892; int _temp2894; struct Cyc_Absyn_Exp* _temp2896; struct Cyc_Core_Opt*
_temp2898; struct Cyc_Core_Opt* _temp2900; struct Cyc_Absyn_Pat* _temp2902;
struct Cyc_List_List* _temp2904; struct Cyc_Absyn_Enumdecl* _temp2906; struct
Cyc_Absyn_Enumdecl _temp2908; struct Cyc_Core_Opt* _temp2909; struct _tuple0*
_temp2911; void* _temp2913; struct Cyc_Absyn_Typedefdecl* _temp2915; struct Cyc_List_List*
_temp2917; struct _tagged_string* _temp2919; struct Cyc_List_List* _temp2921;
struct _tuple0* _temp2923; struct Cyc_List_List* _temp2925; _LL2834: if(*(( int*)
_temp2832) == Cyc_Absyn_Fn_d){ _LL2859: _temp2858=(( struct Cyc_Absyn_Fn_d_struct*)
_temp2832)->f1; goto _LL2835;} else{ goto _LL2836;} _LL2836: if(*(( int*)
_temp2832) == Cyc_Absyn_Struct_d){ _LL2861: _temp2860=(( struct Cyc_Absyn_Struct_d_struct*)
_temp2832)->f1; goto _LL2837;} else{ goto _LL2838;} _LL2838: if(*(( int*)
_temp2832) == Cyc_Absyn_Union_d){ _LL2863: _temp2862=(( struct Cyc_Absyn_Union_d_struct*)
_temp2832)->f1; goto _LL2839;} else{ goto _LL2840;} _LL2840: if(*(( int*)
_temp2832) == Cyc_Absyn_Var_d){ _LL2865: _temp2864=(( struct Cyc_Absyn_Var_d_struct*)
_temp2832)->f1; _temp2866=* _temp2864; _LL2880: _temp2879=( void*) _temp2866.sc;
goto _LL2878; _LL2878: _temp2877= _temp2866.name; goto _LL2876; _LL2876:
_temp2875= _temp2866.tq; goto _LL2874; _LL2874: _temp2873=( void*) _temp2866.type;
goto _LL2872; _LL2872: _temp2871= _temp2866.initializer; goto _LL2870; _LL2870:
_temp2869= _temp2866.rgn; goto _LL2868; _LL2868: _temp2867= _temp2866.attributes;
goto _LL2841;} else{ goto _LL2842;} _LL2842: if(*(( int*) _temp2832) == Cyc_Absyn_Tunion_d){
_LL2882: _temp2881=(( struct Cyc_Absyn_Tunion_d_struct*) _temp2832)->f1;
_temp2883=* _temp2881; _LL2893: _temp2892=( void*) _temp2883.sc; goto _LL2891;
_LL2891: _temp2890= _temp2883.name; goto _LL2889; _LL2889: _temp2888= _temp2883.tvs;
goto _LL2887; _LL2887: _temp2886= _temp2883.fields; goto _LL2885; _LL2885:
_temp2884= _temp2883.is_xtunion; goto _LL2843;} else{ goto _LL2844;} _LL2844:
if(*(( int*) _temp2832) == Cyc_Absyn_Let_d){ _LL2903: _temp2902=(( struct Cyc_Absyn_Let_d_struct*)
_temp2832)->f1; goto _LL2901; _LL2901: _temp2900=(( struct Cyc_Absyn_Let_d_struct*)
_temp2832)->f2; goto _LL2899; _LL2899: _temp2898=(( struct Cyc_Absyn_Let_d_struct*)
_temp2832)->f3; goto _LL2897; _LL2897: _temp2896=(( struct Cyc_Absyn_Let_d_struct*)
_temp2832)->f4; goto _LL2895; _LL2895: _temp2894=(( struct Cyc_Absyn_Let_d_struct*)
_temp2832)->f5; goto _LL2845;} else{ goto _LL2846;} _LL2846: if(*(( int*)
_temp2832) == Cyc_Absyn_Letv_d){ _LL2905: _temp2904=(( struct Cyc_Absyn_Letv_d_struct*)
_temp2832)->f1; goto _LL2847;} else{ goto _LL2848;} _LL2848: if(*(( int*)
_temp2832) == Cyc_Absyn_Enum_d){ _LL2907: _temp2906=(( struct Cyc_Absyn_Enum_d_struct*)
_temp2832)->f1; _temp2908=* _temp2906; _LL2914: _temp2913=( void*) _temp2908.sc;
goto _LL2912; _LL2912: _temp2911= _temp2908.name; goto _LL2910; _LL2910:
_temp2909= _temp2908.fields; goto _LL2849;} else{ goto _LL2850;} _LL2850: if(*((
int*) _temp2832) == Cyc_Absyn_Typedef_d){ _LL2916: _temp2915=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2832)->f1; goto _LL2851;} else{ goto _LL2852;} _LL2852: if(*(( int*)
_temp2832) == Cyc_Absyn_Namespace_d){ _LL2920: _temp2919=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2832)->f1; goto _LL2918; _LL2918: _temp2917=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2832)->f2; goto _LL2853;} else{ goto _LL2854;} _LL2854: if(*(( int*)
_temp2832) == Cyc_Absyn_Using_d){ _LL2924: _temp2923=(( struct Cyc_Absyn_Using_d_struct*)
_temp2832)->f1; goto _LL2922; _LL2922: _temp2921=(( struct Cyc_Absyn_Using_d_struct*)
_temp2832)->f2; goto _LL2855;} else{ goto _LL2856;} _LL2856: if(*(( int*)
_temp2832) == Cyc_Absyn_ExternC_d){ _LL2926: _temp2925=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2832)->f1; goto _LL2857;} else{ goto _LL2833;} _LL2835: { void* t=( void*)({
struct Cyc_Absyn_FnType_struct* _temp2966=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2966[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2967; _temp2967.tag= Cyc_Absyn_FnType; _temp2967.f1=({ struct Cyc_Absyn_FnInfo
_temp2968; _temp2968.tvars= _temp2858->tvs; _temp2968.effect= _temp2858->effect;
_temp2968.ret_typ=( void*)(( void*) _temp2858->ret_type); _temp2968.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp2858->args); _temp2968.c_varargs=
_temp2858->c_varargs; _temp2968.cyc_varargs= _temp2858->cyc_varargs; _temp2968.rgn_po=
_temp2858->rgn_po; _temp2968.attributes= 0; _temp2968;}); _temp2967;});
_temp2966;}); struct Cyc_PP_Doc* attsdoc= Cyc_Absynpp_atts2doc( _temp2858->attributes);
struct Cyc_PP_Doc* inlinedoc; if( _temp2858->is_inline){ if( Cyc_Absynpp_to_VC){
inlinedoc= Cyc_PP_text(({ unsigned char* _temp2927=( unsigned char*)"__inline ";
struct _tagged_string _temp2928; _temp2928.curr= _temp2927; _temp2928.base=
_temp2927; _temp2928.last_plus_one= _temp2927 + 10; _temp2928;}));} else{
inlinedoc= Cyc_PP_text(({ unsigned char* _temp2929=( unsigned char*)"inline ";
struct _tagged_string _temp2930; _temp2930.curr= _temp2929; _temp2930.base=
_temp2929; _temp2930.last_plus_one= _temp2929 + 8; _temp2930;}));}} else{
inlinedoc= Cyc_PP_nil_doc();}{ struct Cyc_PP_Doc* scopedoc= Cyc_Absynpp_scope2doc((
void*) _temp2858->sc); struct Cyc_PP_Doc* beforenamedoc= Cyc_Absynpp_to_VC? Cyc_Absynpp_callconv2doc(
_temp2858->attributes): attsdoc; struct Cyc_PP_Doc* namedoc= Cyc_Absynpp_typedef_name2doc(
_temp2858->name); struct Cyc_PP_Doc* tqtddoc= Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual
_temp2959; _temp2959.q_const= 0; _temp2959.q_volatile= 0; _temp2959.q_restrict=
0; _temp2959;}), t,({ struct Cyc_Core_Opt* _temp2960=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2960->v=( void*)({ struct Cyc_PP_Doc*(*
_temp2961)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2964=
beforenamedoc; struct Cyc_PP_Doc* _temp2965= namedoc; struct Cyc_PP_Doc*
_temp2962[ 2u]={ _temp2964, _temp2965}; struct _tagged_ptr1 _temp2963={
_temp2962, _temp2962, _temp2962 + 2u}; _temp2961( _temp2963);}); _temp2960;}));
struct Cyc_PP_Doc* bodydoc=({ struct Cyc_PP_Doc*(* _temp2943)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2946= Cyc_PP_text(({
unsigned char* _temp2957=( unsigned char*)" {"; struct _tagged_string _temp2958;
_temp2958.curr= _temp2957; _temp2958.base= _temp2957; _temp2958.last_plus_one=
_temp2957 + 3; _temp2958;})); struct Cyc_PP_Doc* _temp2947= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc*(* _temp2952)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2955= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2956= Cyc_Absynpp_stmt2doc(
_temp2858->body); struct Cyc_PP_Doc* _temp2953[ 2u]={ _temp2955, _temp2956};
struct _tagged_ptr1 _temp2954={ _temp2953, _temp2953, _temp2953 + 2u}; _temp2952(
_temp2954);})); struct Cyc_PP_Doc* _temp2948= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2949= Cyc_PP_text(({ unsigned char* _temp2950=( unsigned char*)"}"; struct
_tagged_string _temp2951; _temp2951.curr= _temp2950; _temp2951.base= _temp2950;
_temp2951.last_plus_one= _temp2950 + 2; _temp2951;})); struct Cyc_PP_Doc*
_temp2944[ 4u]={ _temp2946, _temp2947, _temp2948, _temp2949}; struct
_tagged_ptr1 _temp2945={ _temp2944, _temp2944, _temp2944 + 4u}; _temp2943(
_temp2945);}); s=({ struct Cyc_PP_Doc*(* _temp2931)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2934= inlinedoc; struct Cyc_PP_Doc* _temp2935= scopedoc;
struct Cyc_PP_Doc* _temp2936= tqtddoc; struct Cyc_PP_Doc* _temp2937= bodydoc;
struct Cyc_PP_Doc* _temp2932[ 4u]={ _temp2934, _temp2935, _temp2936, _temp2937};
struct _tagged_ptr1 _temp2933={ _temp2932, _temp2932, _temp2932 + 4u}; _temp2931(
_temp2933);}); if( Cyc_Absynpp_to_VC){ s=({ struct Cyc_PP_Doc*(* _temp2938)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2941= attsdoc; struct
Cyc_PP_Doc* _temp2942= s; struct Cyc_PP_Doc* _temp2939[ 2u]={ _temp2941,
_temp2942}; struct _tagged_ptr1 _temp2940={ _temp2939, _temp2939, _temp2939 + 2u};
_temp2938( _temp2940);});} goto _LL2833;}} _LL2837: if( _temp2860->fields == 0){
s=({ struct Cyc_PP_Doc*(* _temp2969)( struct _tagged_ptr1)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp2972= Cyc_Absynpp_scope2doc(( void*) _temp2860->sc); struct Cyc_PP_Doc*
_temp2973= Cyc_PP_text(({ unsigned char* _temp2981=( unsigned char*)"struct ";
struct _tagged_string _temp2982; _temp2982.curr= _temp2981; _temp2982.base=
_temp2981; _temp2982.last_plus_one= _temp2981 + 8; _temp2982;})); struct Cyc_PP_Doc*
_temp2974= _temp2860->name == 0? Cyc_PP_text(({ unsigned char* _temp2979=(
unsigned char*)""; struct _tagged_string _temp2980; _temp2980.curr= _temp2979;
_temp2980.base= _temp2979; _temp2980.last_plus_one= _temp2979 + 1; _temp2980;})):
Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp2860->name))->v); struct Cyc_PP_Doc* _temp2975= Cyc_Absynpp_ktvars2doc(
_temp2860->tvs); struct Cyc_PP_Doc* _temp2976= Cyc_PP_text(({ unsigned char*
_temp2977=( unsigned char*)";"; struct _tagged_string _temp2978; _temp2978.curr=
_temp2977; _temp2978.base= _temp2977; _temp2978.last_plus_one= _temp2977 + 2;
_temp2978;})); struct Cyc_PP_Doc* _temp2970[ 5u]={ _temp2972, _temp2973,
_temp2974, _temp2975, _temp2976}; struct _tagged_ptr1 _temp2971={ _temp2970,
_temp2970, _temp2970 + 5u}; _temp2969( _temp2971);});} else{ s=({ struct Cyc_PP_Doc*(*
_temp2983)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2986= Cyc_Absynpp_scope2doc((
void*) _temp2860->sc); struct Cyc_PP_Doc* _temp2987= Cyc_PP_text(({
unsigned char* _temp3009=( unsigned char*)"struct "; struct _tagged_string
_temp3010; _temp3010.curr= _temp3009; _temp3010.base= _temp3009; _temp3010.last_plus_one=
_temp3009 + 8; _temp3010;})); struct Cyc_PP_Doc* _temp2988= _temp2860->name == 0?
Cyc_PP_text(({ unsigned char* _temp3007=( unsigned char*)""; struct
_tagged_string _temp3008; _temp3008.curr= _temp3007; _temp3008.base= _temp3007;
_temp3008.last_plus_one= _temp3007 + 1; _temp3008;})): Cyc_Absynpp_typedef_name2doc((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp2860->name))->v);
struct Cyc_PP_Doc* _temp2989= Cyc_Absynpp_ktvars2doc( _temp2860->tvs); struct
Cyc_PP_Doc* _temp2990= Cyc_PP_text(({ unsigned char* _temp3005=( unsigned char*)" {";
struct _tagged_string _temp3006; _temp3006.curr= _temp3005; _temp3006.base=
_temp3005; _temp3006.last_plus_one= _temp3005 + 3; _temp3006;})); struct Cyc_PP_Doc*
_temp2991= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp3000)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3003= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp3004= Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp2860->fields))->v); struct Cyc_PP_Doc*
_temp3001[ 2u]={ _temp3003, _temp3004}; struct _tagged_ptr1 _temp3002={
_temp3001, _temp3001, _temp3001 + 2u}; _temp3000( _temp3002);})); struct Cyc_PP_Doc*
_temp2992= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2993= Cyc_PP_text(({
unsigned char* _temp2998=( unsigned char*)"}"; struct _tagged_string _temp2999;
_temp2999.curr= _temp2998; _temp2999.base= _temp2998; _temp2999.last_plus_one=
_temp2998 + 2; _temp2999;})); struct Cyc_PP_Doc* _temp2994= Cyc_Absynpp_atts2doc(
_temp2860->attributes); struct Cyc_PP_Doc* _temp2995= Cyc_PP_text(({
unsigned char* _temp2996=( unsigned char*)";"; struct _tagged_string _temp2997;
_temp2997.curr= _temp2996; _temp2997.base= _temp2996; _temp2997.last_plus_one=
_temp2996 + 2; _temp2997;})); struct Cyc_PP_Doc* _temp2984[ 10u]={ _temp2986,
_temp2987, _temp2988, _temp2989, _temp2990, _temp2991, _temp2992, _temp2993,
_temp2994, _temp2995}; struct _tagged_ptr1 _temp2985={ _temp2984, _temp2984,
_temp2984 + 10u}; _temp2983( _temp2985);});} goto _LL2833; _LL2839: if(
_temp2862->fields == 0){ s=({ struct Cyc_PP_Doc*(* _temp3011)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3014= Cyc_Absynpp_scope2doc((
void*) _temp2862->sc); struct Cyc_PP_Doc* _temp3015= Cyc_PP_text(({
unsigned char* _temp3023=( unsigned char*)"union "; struct _tagged_string
_temp3024; _temp3024.curr= _temp3023; _temp3024.base= _temp3023; _temp3024.last_plus_one=
_temp3023 + 7; _temp3024;})); struct Cyc_PP_Doc* _temp3016= _temp2862->name == 0?
Cyc_PP_text(({ unsigned char* _temp3021=( unsigned char*)""; struct
_tagged_string _temp3022; _temp3022.curr= _temp3021; _temp3022.base= _temp3021;
_temp3022.last_plus_one= _temp3021 + 1; _temp3022;})): Cyc_Absynpp_qvar2doc((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp2862->name))->v);
struct Cyc_PP_Doc* _temp3017= Cyc_Absynpp_tvars2doc( _temp2862->tvs); struct Cyc_PP_Doc*
_temp3018= Cyc_PP_text(({ unsigned char* _temp3019=( unsigned char*)";"; struct
_tagged_string _temp3020; _temp3020.curr= _temp3019; _temp3020.base= _temp3019;
_temp3020.last_plus_one= _temp3019 + 2; _temp3020;})); struct Cyc_PP_Doc*
_temp3012[ 5u]={ _temp3014, _temp3015, _temp3016, _temp3017, _temp3018}; struct
_tagged_ptr1 _temp3013={ _temp3012, _temp3012, _temp3012 + 5u}; _temp3011(
_temp3013);});} else{ s=({ struct Cyc_PP_Doc*(* _temp3025)( struct _tagged_ptr1)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp3028= Cyc_Absynpp_scope2doc(( void*)
_temp2862->sc); struct Cyc_PP_Doc* _temp3029= Cyc_PP_text(({ unsigned char*
_temp3051=( unsigned char*)"union "; struct _tagged_string _temp3052; _temp3052.curr=
_temp3051; _temp3052.base= _temp3051; _temp3052.last_plus_one= _temp3051 + 7;
_temp3052;})); struct Cyc_PP_Doc* _temp3030= _temp2862->name == 0? Cyc_PP_text(({
unsigned char* _temp3049=( unsigned char*)""; struct _tagged_string _temp3050;
_temp3050.curr= _temp3049; _temp3050.base= _temp3049; _temp3050.last_plus_one=
_temp3049 + 1; _temp3050;})): Cyc_Absynpp_qvar2doc(( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp2862->name))->v); struct Cyc_PP_Doc* _temp3031=
Cyc_Absynpp_tvars2doc( _temp2862->tvs); struct Cyc_PP_Doc* _temp3032= Cyc_PP_text(({
unsigned char* _temp3047=( unsigned char*)" {"; struct _tagged_string _temp3048;
_temp3048.curr= _temp3047; _temp3048.base= _temp3047; _temp3048.last_plus_one=
_temp3047 + 3; _temp3048;})); struct Cyc_PP_Doc* _temp3033= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc*(* _temp3042)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp3045= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3046= Cyc_Absynpp_structfields2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp2862->fields))->v);
struct Cyc_PP_Doc* _temp3043[ 2u]={ _temp3045, _temp3046}; struct _tagged_ptr1
_temp3044={ _temp3043, _temp3043, _temp3043 + 2u}; _temp3042( _temp3044);}));
struct Cyc_PP_Doc* _temp3034= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3035=
Cyc_PP_text(({ unsigned char* _temp3040=( unsigned char*)"}"; struct
_tagged_string _temp3041; _temp3041.curr= _temp3040; _temp3041.base= _temp3040;
_temp3041.last_plus_one= _temp3040 + 2; _temp3041;})); struct Cyc_PP_Doc*
_temp3036= Cyc_Absynpp_atts2doc( _temp2862->attributes); struct Cyc_PP_Doc*
_temp3037= Cyc_PP_text(({ unsigned char* _temp3038=( unsigned char*)";"; struct
_tagged_string _temp3039; _temp3039.curr= _temp3038; _temp3039.base= _temp3038;
_temp3039.last_plus_one= _temp3038 + 2; _temp3039;})); struct Cyc_PP_Doc*
_temp3026[ 10u]={ _temp3028, _temp3029, _temp3030, _temp3031, _temp3032,
_temp3033, _temp3034, _temp3035, _temp3036, _temp3037}; struct _tagged_ptr1
_temp3027={ _temp3026, _temp3026, _temp3026 + 10u}; _temp3025( _temp3027);});}
goto _LL2833; _LL2841: { struct Cyc_PP_Doc* sn= Cyc_Absynpp_typedef_name2doc(
_temp2877); s=({ struct Cyc_PP_Doc*(* _temp3053)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp3056= Cyc_Absynpp_to_VC? Cyc_Absynpp_atts2doc( _temp2867):
Cyc_PP_nil_doc(); struct Cyc_PP_Doc* _temp3057= Cyc_Absynpp_scope2doc( _temp2879);
struct Cyc_PP_Doc* _temp3058= Cyc_Absynpp_tqtd2doc( _temp2875, _temp2873,({
struct Cyc_Core_Opt* _temp3073=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3073->v=( void*) sn; _temp3073;})); struct Cyc_PP_Doc*
_temp3059= ! Cyc_Absynpp_to_VC? Cyc_Absynpp_atts2doc( _temp2867): Cyc_PP_nil_doc();
struct Cyc_PP_Doc* _temp3060= _temp2871 == 0? Cyc_PP_text(({ unsigned char*
_temp3064=( unsigned char*)""; struct _tagged_string _temp3065; _temp3065.curr=
_temp3064; _temp3065.base= _temp3064; _temp3065.last_plus_one= _temp3064 + 1;
_temp3065;})):({ struct Cyc_PP_Doc*(* _temp3066)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp3069= Cyc_PP_text(({ unsigned char* _temp3071=(
unsigned char*)" = "; struct _tagged_string _temp3072; _temp3072.curr= _temp3071;
_temp3072.base= _temp3071; _temp3072.last_plus_one= _temp3071 + 4; _temp3072;}));
struct Cyc_PP_Doc* _temp3070= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)
_check_null( _temp2871)); struct Cyc_PP_Doc* _temp3067[ 2u]={ _temp3069,
_temp3070}; struct _tagged_ptr1 _temp3068={ _temp3067, _temp3067, _temp3067 + 2u};
_temp3066( _temp3068);}); struct Cyc_PP_Doc* _temp3061= Cyc_PP_text(({
unsigned char* _temp3062=( unsigned char*)";"; struct _tagged_string _temp3063;
_temp3063.curr= _temp3062; _temp3063.base= _temp3062; _temp3063.last_plus_one=
_temp3062 + 2; _temp3063;})); struct Cyc_PP_Doc* _temp3054[ 6u]={ _temp3056,
_temp3057, _temp3058, _temp3059, _temp3060, _temp3061}; struct _tagged_ptr1
_temp3055={ _temp3054, _temp3054, _temp3054 + 6u}; _temp3053( _temp3055);});
goto _LL2833;} _LL2843: if( _temp2886 == 0){ s=({ struct Cyc_PP_Doc*(* _temp3074)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3077= Cyc_Absynpp_scope2doc(
_temp2892); struct Cyc_PP_Doc* _temp3078= _temp2884? Cyc_PP_text(({
unsigned char* _temp3084=( unsigned char*)"xtunion "; struct _tagged_string
_temp3085; _temp3085.curr= _temp3084; _temp3085.base= _temp3084; _temp3085.last_plus_one=
_temp3084 + 9; _temp3085;})): Cyc_PP_text(({ unsigned char* _temp3086=(
unsigned char*)"tunion "; struct _tagged_string _temp3087; _temp3087.curr=
_temp3086; _temp3087.base= _temp3086; _temp3087.last_plus_one= _temp3086 + 8;
_temp3087;})); struct Cyc_PP_Doc* _temp3079= _temp2884? Cyc_Absynpp_qvar2doc(
_temp2890): Cyc_Absynpp_typedef_name2doc( _temp2890); struct Cyc_PP_Doc*
_temp3080= Cyc_Absynpp_ktvars2doc( _temp2888); struct Cyc_PP_Doc* _temp3081= Cyc_PP_text(({
unsigned char* _temp3082=( unsigned char*)";"; struct _tagged_string _temp3083;
_temp3083.curr= _temp3082; _temp3083.base= _temp3082; _temp3083.last_plus_one=
_temp3082 + 2; _temp3083;})); struct Cyc_PP_Doc* _temp3075[ 5u]={ _temp3077,
_temp3078, _temp3079, _temp3080, _temp3081}; struct _tagged_ptr1 _temp3076={
_temp3075, _temp3075, _temp3075 + 5u}; _temp3074( _temp3076);});} else{ s=({
struct Cyc_PP_Doc*(* _temp3088)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp3091= Cyc_Absynpp_scope2doc( _temp2892); struct Cyc_PP_Doc* _temp3092=
_temp2884? Cyc_PP_text(({ unsigned char* _temp3108=( unsigned char*)"xtunion ";
struct _tagged_string _temp3109; _temp3109.curr= _temp3108; _temp3109.base=
_temp3108; _temp3109.last_plus_one= _temp3108 + 9; _temp3109;})): Cyc_PP_text(({
unsigned char* _temp3110=( unsigned char*)"tunion "; struct _tagged_string
_temp3111; _temp3111.curr= _temp3110; _temp3111.base= _temp3110; _temp3111.last_plus_one=
_temp3110 + 8; _temp3111;})); struct Cyc_PP_Doc* _temp3093= _temp2884? Cyc_Absynpp_qvar2doc(
_temp2890): Cyc_Absynpp_typedef_name2doc( _temp2890); struct Cyc_PP_Doc*
_temp3094= Cyc_Absynpp_ktvars2doc( _temp2888); struct Cyc_PP_Doc* _temp3095= Cyc_PP_text(({
unsigned char* _temp3106=( unsigned char*)" {"; struct _tagged_string _temp3107;
_temp3107.curr= _temp3106; _temp3107.base= _temp3106; _temp3107.last_plus_one=
_temp3106 + 3; _temp3107;})); struct Cyc_PP_Doc* _temp3096= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc*(* _temp3101)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp3104= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3105= Cyc_Absynpp_tunionfields2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp2886))->v);
struct Cyc_PP_Doc* _temp3102[ 2u]={ _temp3104, _temp3105}; struct _tagged_ptr1
_temp3103={ _temp3102, _temp3102, _temp3102 + 2u}; _temp3101( _temp3103);}));
struct Cyc_PP_Doc* _temp3097= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3098=
Cyc_PP_text(({ unsigned char* _temp3099=( unsigned char*)"};"; struct
_tagged_string _temp3100; _temp3100.curr= _temp3099; _temp3100.base= _temp3099;
_temp3100.last_plus_one= _temp3099 + 3; _temp3100;})); struct Cyc_PP_Doc*
_temp3089[ 8u]={ _temp3091, _temp3092, _temp3093, _temp3094, _temp3095,
_temp3096, _temp3097, _temp3098}; struct _tagged_ptr1 _temp3090={ _temp3089,
_temp3089, _temp3089 + 8u}; _temp3088( _temp3090);});} goto _LL2833; _LL2845: s=({
struct Cyc_PP_Doc*(* _temp3112)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp3115= Cyc_PP_text(({ unsigned char* _temp3124=( unsigned char*)"let ";
struct _tagged_string _temp3125; _temp3125.curr= _temp3124; _temp3125.base=
_temp3124; _temp3125.last_plus_one= _temp3124 + 5; _temp3125;})); struct Cyc_PP_Doc*
_temp3116= Cyc_Absynpp_pat2doc( _temp2902); struct Cyc_PP_Doc* _temp3117= Cyc_PP_text(({
unsigned char* _temp3122=( unsigned char*)" = "; struct _tagged_string _temp3123;
_temp3123.curr= _temp3122; _temp3123.base= _temp3122; _temp3123.last_plus_one=
_temp3122 + 4; _temp3123;})); struct Cyc_PP_Doc* _temp3118= Cyc_Absynpp_exp2doc(
_temp2896); struct Cyc_PP_Doc* _temp3119= Cyc_PP_text(({ unsigned char*
_temp3120=( unsigned char*)";"; struct _tagged_string _temp3121; _temp3121.curr=
_temp3120; _temp3121.base= _temp3120; _temp3121.last_plus_one= _temp3120 + 2;
_temp3121;})); struct Cyc_PP_Doc* _temp3113[ 5u]={ _temp3115, _temp3116,
_temp3117, _temp3118, _temp3119}; struct _tagged_ptr1 _temp3114={ _temp3113,
_temp3113, _temp3113 + 5u}; _temp3112( _temp3114);}); goto _LL2833; _LL2847: s=({
struct Cyc_PP_Doc*(* _temp3126)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp3129= Cyc_PP_text(({ unsigned char* _temp3134=( unsigned char*)"let ";
struct _tagged_string _temp3135; _temp3135.curr= _temp3134; _temp3135.base=
_temp3134; _temp3135.last_plus_one= _temp3134 + 5; _temp3135;})); struct Cyc_PP_Doc*
_temp3130= Cyc_Absynpp_ids2doc( _temp2904); struct Cyc_PP_Doc* _temp3131= Cyc_PP_text(({
unsigned char* _temp3132=( unsigned char*)";"; struct _tagged_string _temp3133;
_temp3133.curr= _temp3132; _temp3133.base= _temp3132; _temp3133.last_plus_one=
_temp3132 + 2; _temp3133;})); struct Cyc_PP_Doc* _temp3127[ 3u]={ _temp3129,
_temp3130, _temp3131}; struct _tagged_ptr1 _temp3128={ _temp3127, _temp3127,
_temp3127 + 3u}; _temp3126( _temp3128);}); goto _LL2833; _LL2849: if( _temp2909
== 0){ s=({ struct Cyc_PP_Doc*(* _temp3136)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp3139= Cyc_Absynpp_scope2doc( _temp2913); struct Cyc_PP_Doc*
_temp3140= Cyc_PP_text(({ unsigned char* _temp3145=( unsigned char*)"enum ";
struct _tagged_string _temp3146; _temp3146.curr= _temp3145; _temp3146.base=
_temp3145; _temp3146.last_plus_one= _temp3145 + 6; _temp3146;})); struct Cyc_PP_Doc*
_temp3141= Cyc_Absynpp_typedef_name2doc( _temp2911); struct Cyc_PP_Doc*
_temp3142= Cyc_PP_text(({ unsigned char* _temp3143=( unsigned char*)";"; struct
_tagged_string _temp3144; _temp3144.curr= _temp3143; _temp3144.base= _temp3143;
_temp3144.last_plus_one= _temp3143 + 2; _temp3144;})); struct Cyc_PP_Doc*
_temp3137[ 4u]={ _temp3139, _temp3140, _temp3141, _temp3142}; struct
_tagged_ptr1 _temp3138={ _temp3137, _temp3137, _temp3137 + 4u}; _temp3136(
_temp3138);});} else{ s=({ struct Cyc_PP_Doc*(* _temp3147)( struct _tagged_ptr1)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp3150= Cyc_Absynpp_scope2doc( _temp2913);
struct Cyc_PP_Doc* _temp3151= Cyc_PP_text(({ unsigned char* _temp3166=(
unsigned char*)"enum "; struct _tagged_string _temp3167; _temp3167.curr=
_temp3166; _temp3167.base= _temp3166; _temp3167.last_plus_one= _temp3166 + 6;
_temp3167;})); struct Cyc_PP_Doc* _temp3152= Cyc_Absynpp_qvar2doc( _temp2911);
struct Cyc_PP_Doc* _temp3153= Cyc_PP_text(({ unsigned char* _temp3164=(
unsigned char*)" {"; struct _tagged_string _temp3165; _temp3165.curr= _temp3164;
_temp3165.base= _temp3164; _temp3165.last_plus_one= _temp3164 + 3; _temp3165;}));
struct Cyc_PP_Doc* _temp3154= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp3159)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3162= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp3163= Cyc_Absynpp_enumfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp2909))->v); struct Cyc_PP_Doc* _temp3160[
2u]={ _temp3162, _temp3163}; struct _tagged_ptr1 _temp3161={ _temp3160,
_temp3160, _temp3160 + 2u}; _temp3159( _temp3161);})); struct Cyc_PP_Doc*
_temp3155= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3156= Cyc_PP_text(({
unsigned char* _temp3157=( unsigned char*)"};"; struct _tagged_string _temp3158;
_temp3158.curr= _temp3157; _temp3158.base= _temp3157; _temp3158.last_plus_one=
_temp3157 + 3; _temp3158;})); struct Cyc_PP_Doc* _temp3148[ 7u]={ _temp3150,
_temp3151, _temp3152, _temp3153, _temp3154, _temp3155, _temp3156}; struct
_tagged_ptr1 _temp3149={ _temp3148, _temp3148, _temp3148 + 7u}; _temp3147(
_temp3149);});} goto _LL2833; _LL2851: s=({ struct Cyc_PP_Doc*(* _temp3168)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3171= Cyc_PP_text(({
unsigned char* _temp3183=( unsigned char*)"typedef "; struct _tagged_string
_temp3184; _temp3184.curr= _temp3183; _temp3184.base= _temp3183; _temp3184.last_plus_one=
_temp3183 + 9; _temp3184;})); struct Cyc_PP_Doc* _temp3172= Cyc_Absynpp_tqtd2doc(({
struct Cyc_Absyn_Tqual _temp3176; _temp3176.q_const= 0; _temp3176.q_volatile= 0;
_temp3176.q_restrict= 0; _temp3176;}),( void*) _temp2915->defn,({ struct Cyc_Core_Opt*
_temp3177=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3177->v=( void*)({ struct Cyc_PP_Doc*(* _temp3178)( struct _tagged_ptr1)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp3181= Cyc_Absynpp_typedef_name2doc(
_temp2915->name); struct Cyc_PP_Doc* _temp3182= Cyc_Absynpp_tvars2doc( _temp2915->tvs);
struct Cyc_PP_Doc* _temp3179[ 2u]={ _temp3181, _temp3182}; struct _tagged_ptr1
_temp3180={ _temp3179, _temp3179, _temp3179 + 2u}; _temp3178( _temp3180);});
_temp3177;})); struct Cyc_PP_Doc* _temp3173= Cyc_PP_text(({ unsigned char*
_temp3174=( unsigned char*)";"; struct _tagged_string _temp3175; _temp3175.curr=
_temp3174; _temp3175.base= _temp3174; _temp3175.last_plus_one= _temp3174 + 2;
_temp3175;})); struct Cyc_PP_Doc* _temp3169[ 3u]={ _temp3171, _temp3172,
_temp3173}; struct _tagged_ptr1 _temp3170={ _temp3169, _temp3169, _temp3169 + 3u};
_temp3168( _temp3170);}); goto _LL2833; _LL2853: if( Cyc_Absynpp_use_curr_namespace){
Cyc_Absynpp_curr_namespace_add( _temp2919);} s=({ struct Cyc_PP_Doc*(* _temp3185)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3188= Cyc_PP_text(({
unsigned char* _temp3201=( unsigned char*)"namespace "; struct _tagged_string
_temp3202; _temp3202.curr= _temp3201; _temp3202.base= _temp3201; _temp3202.last_plus_one=
_temp3201 + 11; _temp3202;})); struct Cyc_PP_Doc* _temp3189= Cyc_PP_textptr(
_temp2919); struct Cyc_PP_Doc* _temp3190= Cyc_PP_text(({ unsigned char*
_temp3199=( unsigned char*)" {"; struct _tagged_string _temp3200; _temp3200.curr=
_temp3199; _temp3200.base= _temp3199; _temp3200.last_plus_one= _temp3199 + 3;
_temp3200;})); struct Cyc_PP_Doc* _temp3191= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp3192=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({
unsigned char* _temp3197=( unsigned char*)""; struct _tagged_string _temp3198;
_temp3198.curr= _temp3197; _temp3198.base= _temp3197; _temp3198.last_plus_one=
_temp3197 + 1; _temp3198;}), _temp2917); struct Cyc_PP_Doc* _temp3193= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp3194= Cyc_PP_text(({ unsigned char* _temp3195=(
unsigned char*)"}"; struct _tagged_string _temp3196; _temp3196.curr= _temp3195;
_temp3196.base= _temp3195; _temp3196.last_plus_one= _temp3195 + 2; _temp3196;}));
struct Cyc_PP_Doc* _temp3186[ 7u]={ _temp3188, _temp3189, _temp3190, _temp3191,
_temp3192, _temp3193, _temp3194}; struct _tagged_ptr1 _temp3187={ _temp3186,
_temp3186, _temp3186 + 7u}; _temp3185( _temp3187);}); if( Cyc_Absynpp_use_curr_namespace){
Cyc_Absynpp_curr_namespace_drop();} goto _LL2833; _LL2855: if( Cyc_Absynpp_print_using_stmts){
s=({ struct Cyc_PP_Doc*(* _temp3203)( struct _tagged_ptr1)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp3206= Cyc_PP_text(({ unsigned char* _temp3219=( unsigned char*)"using ";
struct _tagged_string _temp3220; _temp3220.curr= _temp3219; _temp3220.base=
_temp3219; _temp3220.last_plus_one= _temp3219 + 7; _temp3220;})); struct Cyc_PP_Doc*
_temp3207= Cyc_Absynpp_qvar2doc( _temp2923); struct Cyc_PP_Doc* _temp3208= Cyc_PP_text(({
unsigned char* _temp3217=( unsigned char*)" {"; struct _tagged_string _temp3218;
_temp3218.curr= _temp3217; _temp3218.base= _temp3217; _temp3218.last_plus_one=
_temp3217 + 3; _temp3218;})); struct Cyc_PP_Doc* _temp3209= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp3210=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)(
struct Cyc_Absyn_Decl*), struct _tagged_string sep, struct Cyc_List_List* l0))
Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({ unsigned char* _temp3215=( unsigned char*)"";
struct _tagged_string _temp3216; _temp3216.curr= _temp3215; _temp3216.base=
_temp3215; _temp3216.last_plus_one= _temp3215 + 1; _temp3216;}), _temp2921);
struct Cyc_PP_Doc* _temp3211= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3212=
Cyc_PP_text(({ unsigned char* _temp3213=( unsigned char*)"}"; struct
_tagged_string _temp3214; _temp3214.curr= _temp3213; _temp3214.base= _temp3213;
_temp3214.last_plus_one= _temp3213 + 2; _temp3214;})); struct Cyc_PP_Doc*
_temp3204[ 7u]={ _temp3206, _temp3207, _temp3208, _temp3209, _temp3210,
_temp3211, _temp3212}; struct _tagged_ptr1 _temp3205={ _temp3204, _temp3204,
_temp3204 + 7u}; _temp3203( _temp3205);});} else{ s=({ struct Cyc_PP_Doc*(*
_temp3221)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3224= Cyc_PP_text(({
unsigned char* _temp3237=( unsigned char*)"/* using "; struct _tagged_string
_temp3238; _temp3238.curr= _temp3237; _temp3238.base= _temp3237; _temp3238.last_plus_one=
_temp3237 + 10; _temp3238;})); struct Cyc_PP_Doc* _temp3225= Cyc_Absynpp_qvar2doc(
_temp2923); struct Cyc_PP_Doc* _temp3226= Cyc_PP_text(({ unsigned char*
_temp3235=( unsigned char*)" { */"; struct _tagged_string _temp3236; _temp3236.curr=
_temp3235; _temp3236.base= _temp3235; _temp3236.last_plus_one= _temp3235 + 6;
_temp3236;})); struct Cyc_PP_Doc* _temp3227= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp3228=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({
unsigned char* _temp3233=( unsigned char*)""; struct _tagged_string _temp3234;
_temp3234.curr= _temp3233; _temp3234.base= _temp3233; _temp3234.last_plus_one=
_temp3233 + 1; _temp3234;}), _temp2921); struct Cyc_PP_Doc* _temp3229= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp3230= Cyc_PP_text(({ unsigned char* _temp3231=(
unsigned char*)"/* } */"; struct _tagged_string _temp3232; _temp3232.curr=
_temp3231; _temp3232.base= _temp3231; _temp3232.last_plus_one= _temp3231 + 8;
_temp3232;})); struct Cyc_PP_Doc* _temp3222[ 7u]={ _temp3224, _temp3225,
_temp3226, _temp3227, _temp3228, _temp3229, _temp3230}; struct _tagged_ptr1
_temp3223={ _temp3222, _temp3222, _temp3222 + 7u}; _temp3221( _temp3223);});}
goto _LL2833; _LL2857: if( Cyc_Absynpp_print_externC_stmts){ s=({ struct Cyc_PP_Doc*(*
_temp3239)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3242= Cyc_PP_text(({
unsigned char* _temp3251=( unsigned char*)"extern \"C\" {"; struct
_tagged_string _temp3252; _temp3252.curr= _temp3251; _temp3252.base= _temp3251;
_temp3252.last_plus_one= _temp3251 + 13; _temp3252;})); struct Cyc_PP_Doc*
_temp3243= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3244=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({ unsigned char*
_temp3249=( unsigned char*)""; struct _tagged_string _temp3250; _temp3250.curr=
_temp3249; _temp3250.base= _temp3249; _temp3250.last_plus_one= _temp3249 + 1;
_temp3250;}), _temp2925); struct Cyc_PP_Doc* _temp3245= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp3246= Cyc_PP_text(({ unsigned char* _temp3247=(
unsigned char*)"}"; struct _tagged_string _temp3248; _temp3248.curr= _temp3247;
_temp3248.base= _temp3247; _temp3248.last_plus_one= _temp3247 + 2; _temp3248;}));
struct Cyc_PP_Doc* _temp3240[ 5u]={ _temp3242, _temp3243, _temp3244, _temp3245,
_temp3246}; struct _tagged_ptr1 _temp3241={ _temp3240, _temp3240, _temp3240 + 5u};
_temp3239( _temp3241);});} else{ s=({ struct Cyc_PP_Doc*(* _temp3253)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3256= Cyc_PP_text(({
unsigned char* _temp3265=( unsigned char*)"/* extern \"C\" { */"; struct
_tagged_string _temp3266; _temp3266.curr= _temp3265; _temp3266.base= _temp3265;
_temp3266.last_plus_one= _temp3265 + 19; _temp3266;})); struct Cyc_PP_Doc*
_temp3257= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3258=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({ unsigned char*
_temp3263=( unsigned char*)""; struct _tagged_string _temp3264; _temp3264.curr=
_temp3263; _temp3264.base= _temp3263; _temp3264.last_plus_one= _temp3263 + 1;
_temp3264;}), _temp2925); struct Cyc_PP_Doc* _temp3259= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp3260= Cyc_PP_text(({ unsigned char* _temp3261=(
unsigned char*)"/* } */"; struct _tagged_string _temp3262; _temp3262.curr=
_temp3261; _temp3262.base= _temp3261; _temp3262.last_plus_one= _temp3261 + 8;
_temp3262;})); struct Cyc_PP_Doc* _temp3254[ 5u]={ _temp3256, _temp3257,
_temp3258, _temp3259, _temp3260}; struct _tagged_ptr1 _temp3255={ _temp3254,
_temp3254, _temp3254 + 5u}; _temp3253( _temp3255);});} goto _LL2833; _LL2833:;}
return s;} struct Cyc_PP_Doc* Cyc_Absynpp_scope2doc( void* sc){ void* _temp3267=
sc; _LL3269: if( _temp3267 ==( void*) Cyc_Absyn_Static){ goto _LL3270;} else{
goto _LL3271;} _LL3271: if( _temp3267 ==( void*) Cyc_Absyn_Public){ goto _LL3272;}
else{ goto _LL3273;} _LL3273: if( _temp3267 ==( void*) Cyc_Absyn_Extern){ goto
_LL3274;} else{ goto _LL3275;} _LL3275: if( _temp3267 ==( void*) Cyc_Absyn_ExternC){
goto _LL3276;} else{ goto _LL3277;} _LL3277: if( _temp3267 ==( void*) Cyc_Absyn_Abstract){
goto _LL3278;} else{ goto _LL3268;} _LL3270: return Cyc_PP_text(({ unsigned char*
_temp3279=( unsigned char*)"static "; struct _tagged_string _temp3280; _temp3280.curr=
_temp3279; _temp3280.base= _temp3279; _temp3280.last_plus_one= _temp3279 + 8;
_temp3280;})); _LL3272: return Cyc_PP_nil_doc(); _LL3274: return Cyc_PP_text(({
unsigned char* _temp3281=( unsigned char*)"extern "; struct _tagged_string
_temp3282; _temp3282.curr= _temp3281; _temp3282.base= _temp3281; _temp3282.last_plus_one=
_temp3281 + 8; _temp3282;})); _LL3276: return Cyc_PP_text(({ unsigned char*
_temp3283=( unsigned char*)"extern \"C\" "; struct _tagged_string _temp3284;
_temp3284.curr= _temp3283; _temp3284.base= _temp3283; _temp3284.last_plus_one=
_temp3283 + 12; _temp3284;})); _LL3278: return Cyc_PP_text(({ unsigned char*
_temp3285=( unsigned char*)"abstract "; struct _tagged_string _temp3286;
_temp3286.curr= _temp3285; _temp3286.base= _temp3285; _temp3286.last_plus_one=
_temp3285 + 10; _temp3286;})); _LL3268:;} int Cyc_Absynpp_exists_temp_tvar_in_effect(
void* t){ void* _temp3287= t; struct Cyc_Absyn_Tvar* _temp3295; struct Cyc_List_List*
_temp3297; _LL3289: if(( unsigned int) _temp3287 > 4u?*(( int*) _temp3287) ==
Cyc_Absyn_VarType: 0){ _LL3296: _temp3295=(( struct Cyc_Absyn_VarType_struct*)
_temp3287)->f1; goto _LL3290;} else{ goto _LL3291;} _LL3291: if(( unsigned int)
_temp3287 > 4u?*(( int*) _temp3287) == Cyc_Absyn_JoinEff: 0){ _LL3298: _temp3297=((
struct Cyc_Absyn_JoinEff_struct*) _temp3287)->f1; goto _LL3292;} else{ goto
_LL3293;} _LL3293: goto _LL3294; _LL3290: return Cyc_Tcutil_is_temp_tvar(
_temp3295); _LL3292: return(( int(*)( int(* pred)( void*), struct Cyc_List_List*
x)) Cyc_List_exists)( Cyc_Absynpp_exists_temp_tvar_in_effect, _temp3297);
_LL3294: return 0; _LL3288:;} struct _tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual
tq, void* t){ void* _temp3299= t; struct Cyc_Absyn_Exp* _temp3313; struct Cyc_Absyn_Tqual
_temp3315; void* _temp3317; struct Cyc_Absyn_PtrInfo _temp3319; struct Cyc_Absyn_Conref*
_temp3321; struct Cyc_Absyn_Tqual _temp3323; struct Cyc_Absyn_Conref* _temp3325;
void* _temp3327; void* _temp3329; struct Cyc_Absyn_FnInfo _temp3331; struct Cyc_List_List*
_temp3333; struct Cyc_List_List* _temp3335; struct Cyc_Absyn_VarargInfo*
_temp3337; int _temp3339; struct Cyc_List_List* _temp3341; void* _temp3343;
struct Cyc_Core_Opt* _temp3345; struct Cyc_List_List* _temp3347; struct Cyc_Core_Opt*
_temp3349; int _temp3351; struct Cyc_Core_Opt* _temp3353; struct Cyc_Core_Opt*
_temp3355; struct Cyc_Core_Opt* _temp3357; struct Cyc_List_List* _temp3359;
struct _tuple0* _temp3361; _LL3301: if(( unsigned int) _temp3299 > 4u?*(( int*)
_temp3299) == Cyc_Absyn_ArrayType: 0){ _LL3318: _temp3317=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3299)->f1; goto _LL3316; _LL3316: _temp3315=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3299)->f2; goto _LL3314; _LL3314: _temp3313=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3299)->f3; goto _LL3302;} else{ goto _LL3303;} _LL3303: if(( unsigned int)
_temp3299 > 4u?*(( int*) _temp3299) == Cyc_Absyn_PointerType: 0){ _LL3320:
_temp3319=(( struct Cyc_Absyn_PointerType_struct*) _temp3299)->f1; _LL3330:
_temp3329=( void*) _temp3319.elt_typ; goto _LL3328; _LL3328: _temp3327=( void*)
_temp3319.rgn_typ; goto _LL3326; _LL3326: _temp3325= _temp3319.nullable; goto
_LL3324; _LL3324: _temp3323= _temp3319.tq; goto _LL3322; _LL3322: _temp3321=
_temp3319.bounds; goto _LL3304;} else{ goto _LL3305;} _LL3305: if(( unsigned int)
_temp3299 > 4u?*(( int*) _temp3299) == Cyc_Absyn_FnType: 0){ _LL3332: _temp3331=((
struct Cyc_Absyn_FnType_struct*) _temp3299)->f1; _LL3348: _temp3347= _temp3331.tvars;
goto _LL3346; _LL3346: _temp3345= _temp3331.effect; goto _LL3344; _LL3344:
_temp3343=( void*) _temp3331.ret_typ; goto _LL3342; _LL3342: _temp3341=
_temp3331.args; goto _LL3340; _LL3340: _temp3339= _temp3331.c_varargs; goto
_LL3338; _LL3338: _temp3337= _temp3331.cyc_varargs; goto _LL3336; _LL3336:
_temp3335= _temp3331.rgn_po; goto _LL3334; _LL3334: _temp3333= _temp3331.attributes;
goto _LL3306;} else{ goto _LL3307;} _LL3307: if(( unsigned int) _temp3299 > 4u?*((
int*) _temp3299) == Cyc_Absyn_Evar: 0){ _LL3356: _temp3355=(( struct Cyc_Absyn_Evar_struct*)
_temp3299)->f1; goto _LL3354; _LL3354: _temp3353=(( struct Cyc_Absyn_Evar_struct*)
_temp3299)->f2; goto _LL3352; _LL3352: _temp3351=(( struct Cyc_Absyn_Evar_struct*)
_temp3299)->f3; goto _LL3350; _LL3350: _temp3349=(( struct Cyc_Absyn_Evar_struct*)
_temp3299)->f4; goto _LL3308;} else{ goto _LL3309;} _LL3309: if(( unsigned int)
_temp3299 > 4u?*(( int*) _temp3299) == Cyc_Absyn_TypedefType: 0){ _LL3362:
_temp3361=(( struct Cyc_Absyn_TypedefType_struct*) _temp3299)->f1; goto _LL3360;
_LL3360: _temp3359=(( struct Cyc_Absyn_TypedefType_struct*) _temp3299)->f2; goto
_LL3358; _LL3358: _temp3357=(( struct Cyc_Absyn_TypedefType_struct*) _temp3299)->f3;
goto _LL3310;} else{ goto _LL3311;} _LL3311: goto _LL3312; _LL3302: { struct Cyc_List_List*
_temp3365; void* _temp3367; struct Cyc_Absyn_Tqual _temp3369; struct _tuple4
_temp3363= Cyc_Absynpp_to_tms( _temp3315, _temp3317); _LL3370: _temp3369=
_temp3363.f1; goto _LL3368; _LL3368: _temp3367= _temp3363.f2; goto _LL3366;
_LL3366: _temp3365= _temp3363.f3; goto _LL3364; _LL3364: { void* tm; if(
_temp3313 == 0){ tm=( void*) Cyc_Absyn_Carray_mod;} else{ tm=( void*)({ struct
Cyc_Absyn_ConstArray_mod_struct* _temp3371=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp3371[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp3372; _temp3372.tag= Cyc_Absyn_ConstArray_mod;
_temp3372.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp3313); _temp3372;});
_temp3371;});} return({ struct _tuple4 _temp3373; _temp3373.f1= _temp3369;
_temp3373.f2= _temp3367; _temp3373.f3=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp3374=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3374->hd=( void*) tm; _temp3374->tl= _temp3365; _temp3374;}); _temp3373;});}}
_LL3304: { struct Cyc_List_List* _temp3377; void* _temp3379; struct Cyc_Absyn_Tqual
_temp3381; struct _tuple4 _temp3375= Cyc_Absynpp_to_tms( _temp3323, _temp3329);
_LL3382: _temp3381= _temp3375.f1; goto _LL3380; _LL3380: _temp3379= _temp3375.f2;
goto _LL3378; _LL3378: _temp3377= _temp3375.f3; goto _LL3376; _LL3376: { void*
ps;{ struct _tuple8 _temp3384=({ struct _tuple8 _temp3383; _temp3383.f1=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp3325))->v; _temp3383.f2=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp3321))->v; _temp3383;}); void* _temp3394;
void* _temp3396; struct Cyc_Absyn_Exp* _temp3398; void* _temp3400; int _temp3402;
void* _temp3404; void* _temp3406; struct Cyc_Absyn_Exp* _temp3408; void*
_temp3410; int _temp3412; void* _temp3414; void* _temp3416; void* _temp3418;
_LL3386: _LL3401: _temp3400= _temp3384.f1; if(( unsigned int) _temp3400 > 1u?*((
int*) _temp3400) == Cyc_Absyn_Eq_constr: 0){ _LL3403: _temp3402=( int)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp3400)->f1; if( _temp3402 == 1){ goto _LL3395;}
else{ goto _LL3388;}} else{ goto _LL3388;} _LL3395: _temp3394= _temp3384.f2; if((
unsigned int) _temp3394 > 1u?*(( int*) _temp3394) == Cyc_Absyn_Eq_constr: 0){
_LL3397: _temp3396=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3394)->f1;
if(( unsigned int) _temp3396 > 1u?*(( int*) _temp3396) == Cyc_Absyn_Upper_b: 0){
_LL3399: _temp3398=(( struct Cyc_Absyn_Upper_b_struct*) _temp3396)->f1; goto
_LL3387;} else{ goto _LL3388;}} else{ goto _LL3388;} _LL3388: _LL3411: _temp3410=
_temp3384.f1; if(( unsigned int) _temp3410 > 1u?*(( int*) _temp3410) == Cyc_Absyn_Eq_constr:
0){ _LL3413: _temp3412=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3410)->f1;
if( _temp3412 == 0){ goto _LL3405;} else{ goto _LL3390;}} else{ goto _LL3390;}
_LL3405: _temp3404= _temp3384.f2; if(( unsigned int) _temp3404 > 1u?*(( int*)
_temp3404) == Cyc_Absyn_Eq_constr: 0){ _LL3407: _temp3406=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3404)->f1; if(( unsigned int) _temp3406 > 1u?*(( int*) _temp3406) == Cyc_Absyn_Upper_b:
0){ _LL3409: _temp3408=(( struct Cyc_Absyn_Upper_b_struct*) _temp3406)->f1; goto
_LL3389;} else{ goto _LL3390;}} else{ goto _LL3390;} _LL3390: _LL3419: _temp3418=
_temp3384.f1; goto _LL3415; _LL3415: _temp3414= _temp3384.f2; if(( unsigned int)
_temp3414 > 1u?*(( int*) _temp3414) == Cyc_Absyn_Eq_constr: 0){ _LL3417:
_temp3416=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3414)->f1; if(
_temp3416 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3391;} else{ goto _LL3392;}}
else{ goto _LL3392;} _LL3392: goto _LL3393; _LL3387: ps=( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp3420=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp3420[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp3421; _temp3421.tag=
Cyc_Absyn_Nullable_ps; _temp3421.f1= _temp3398; _temp3421;}); _temp3420;}); goto
_LL3385; _LL3389: ps=( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp3422=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp3422[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp3423; _temp3423.tag= Cyc_Absyn_NonNullable_ps; _temp3423.f1= _temp3408;
_temp3423;}); _temp3422;}); goto _LL3385; _LL3391: ps=( void*) Cyc_Absyn_TaggedArray_ps;
goto _LL3385; _LL3393: ps=( void*) Cyc_Absyn_TaggedArray_ps; goto _LL3385;
_LL3385:;}{ void* tm=( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp3426=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3426[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp3427; _temp3427.tag=
Cyc_Absyn_Pointer_mod; _temp3427.f1=( void*) ps; _temp3427.f2=( void*) _temp3327;
_temp3427.f3= tq; _temp3427;}); _temp3426;}); return({ struct _tuple4 _temp3424;
_temp3424.f1= _temp3381; _temp3424.f2= _temp3379; _temp3424.f3=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp3425=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3425->hd=( void*) tm; _temp3425->tl= _temp3377;
_temp3425;}); _temp3424;});}}} _LL3306: if( ! Cyc_Absynpp_print_all_tvars){ if(
_temp3345 == 0? 1: Cyc_Absynpp_exists_temp_tvar_in_effect(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3345))->v)){ _temp3345= 0; _temp3347= 0;}} else{ if( Cyc_Absynpp_rewrite_temp_tvars){((
void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)(
Cyc_Tcutil_rewrite_temp_tvar, _temp3347);}}{ struct Cyc_List_List* _temp3430;
void* _temp3432; struct Cyc_Absyn_Tqual _temp3434; struct _tuple4 _temp3428= Cyc_Absynpp_to_tms(
Cyc_Absyn_empty_tqual(), _temp3343); _LL3435: _temp3434= _temp3428.f1; goto
_LL3433; _LL3433: _temp3432= _temp3428.f2; goto _LL3431; _LL3431: _temp3430=
_temp3428.f3; goto _LL3429; _LL3429: { struct Cyc_List_List* tms= _temp3430; if(
_temp3333 != 0){ tms=({ struct Cyc_List_List* _temp3436=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3436->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp3437=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp3437[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp3438; _temp3438.tag= Cyc_Absyn_Attributes_mod;
_temp3438.f1= 0; _temp3438.f2= _temp3333; _temp3438;}); _temp3437;})); _temp3436->tl=
tms; _temp3436;});} tms=({ struct Cyc_List_List* _temp3439=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3439->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp3440=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3440[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3441; _temp3441.tag= Cyc_Absyn_Function_mod;
_temp3441.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp3442=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3442[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3443; _temp3443.tag= Cyc_Absyn_WithTypes;
_temp3443.f1= _temp3341; _temp3443.f2= _temp3339; _temp3443.f3= _temp3337;
_temp3443.f4= _temp3345; _temp3443.f5= _temp3335; _temp3443;}); _temp3442;}));
_temp3441;}); _temp3440;})); _temp3439->tl= tms; _temp3439;}); if( _temp3347 !=
0){ tms=({ struct Cyc_List_List* _temp3444=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp3444->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp3445=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp3445[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp3446; _temp3446.tag= Cyc_Absyn_TypeParams_mod; _temp3446.f1= _temp3347;
_temp3446.f2= 0; _temp3446.f3= 1; _temp3446;}); _temp3445;})); _temp3444->tl=
tms; _temp3444;});} return({ struct _tuple4 _temp3447; _temp3447.f1= _temp3434;
_temp3447.f2= _temp3432; _temp3447.f3= tms; _temp3447;});}} _LL3308: if(
_temp3353 == 0){ return({ struct _tuple4 _temp3448; _temp3448.f1= tq; _temp3448.f2=
t; _temp3448.f3= 0; _temp3448;});} else{ return Cyc_Absynpp_to_tms( tq,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp3353))->v);} _LL3310: if( _temp3357 == 0?
1: ! Cyc_Absynpp_expand_typedefs){ return({ struct _tuple4 _temp3449; _temp3449.f1=
tq; _temp3449.f2= t; _temp3449.f3= 0; _temp3449;});} else{ return Cyc_Absynpp_to_tms(
tq,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3357))->v);} _LL3312:
return({ struct _tuple4 _temp3450; _temp3450.f1= tq; _temp3450.f2= t; _temp3450.f3=
0; _temp3450;}); _LL3300:;} struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc( struct Cyc_Absyn_Tqual
tq, void* t, struct Cyc_Core_Opt* dopt){ struct Cyc_List_List* _temp3453; void*
_temp3455; struct Cyc_Absyn_Tqual _temp3457; struct _tuple4 _temp3451= Cyc_Absynpp_to_tms(
tq, t); _LL3458: _temp3457= _temp3451.f1; goto _LL3456; _LL3456: _temp3455=
_temp3451.f2; goto _LL3454; _LL3454: _temp3453= _temp3451.f3; goto _LL3452;
_LL3452: _temp3453=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp3453); if( _temp3453 == 0? dopt == 0: 0){ return({ struct Cyc_PP_Doc*(*
_temp3459)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3462= Cyc_Absynpp_tqual2doc(
_temp3457); struct Cyc_PP_Doc* _temp3463= Cyc_Absynpp_ntyp2doc( _temp3455);
struct Cyc_PP_Doc* _temp3460[ 2u]={ _temp3462, _temp3463}; struct _tagged_ptr1
_temp3461={ _temp3460, _temp3460, _temp3460 + 2u}; _temp3459( _temp3461);});}
else{ return({ struct Cyc_PP_Doc*(* _temp3464)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp3467= Cyc_Absynpp_tqual2doc( _temp3457); struct Cyc_PP_Doc*
_temp3468= Cyc_Absynpp_ntyp2doc( _temp3455); struct Cyc_PP_Doc* _temp3469= Cyc_PP_text(({
unsigned char* _temp3471=( unsigned char*)" "; struct _tagged_string _temp3472;
_temp3472.curr= _temp3471; _temp3472.base= _temp3471; _temp3472.last_plus_one=
_temp3471 + 2; _temp3472;})); struct Cyc_PP_Doc* _temp3470= Cyc_Absynpp_dtms2doc(
dopt == 0? Cyc_PP_nil_doc():( struct Cyc_PP_Doc*)(( struct Cyc_Core_Opt*)
_check_null( dopt))->v, _temp3453); struct Cyc_PP_Doc* _temp3465[ 4u]={
_temp3467, _temp3468, _temp3469, _temp3470}; struct _tagged_ptr1 _temp3466={
_temp3465, _temp3465, _temp3465 + 4u}; _temp3464( _temp3466);});}} struct Cyc_PP_Doc*
Cyc_Absynpp_structfield2doc( struct Cyc_Absyn_Structfield* f){ if( f->width != 0){
return({ struct Cyc_PP_Doc*(* _temp3473)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp3476= Cyc_Absynpp_tqtd2doc( f->tq,( void*) f->type,({
struct Cyc_Core_Opt* _temp3485=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3485->v=( void*) Cyc_PP_textptr( f->name); _temp3485;}));
struct Cyc_PP_Doc* _temp3477= Cyc_PP_text(({ unsigned char* _temp3483=(
unsigned char*)":"; struct _tagged_string _temp3484; _temp3484.curr= _temp3483;
_temp3484.base= _temp3483; _temp3484.last_plus_one= _temp3483 + 2; _temp3484;}));
struct Cyc_PP_Doc* _temp3478= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( f->width))->v); struct Cyc_PP_Doc* _temp3479=
Cyc_Absynpp_atts2doc( f->attributes); struct Cyc_PP_Doc* _temp3480= Cyc_PP_text(({
unsigned char* _temp3481=( unsigned char*)";"; struct _tagged_string _temp3482;
_temp3482.curr= _temp3481; _temp3482.base= _temp3481; _temp3482.last_plus_one=
_temp3481 + 2; _temp3482;})); struct Cyc_PP_Doc* _temp3474[ 5u]={ _temp3476,
_temp3477, _temp3478, _temp3479, _temp3480}; struct _tagged_ptr1 _temp3475={
_temp3474, _temp3474, _temp3474 + 5u}; _temp3473( _temp3475);});} else{ return({
struct Cyc_PP_Doc*(* _temp3486)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp3489= Cyc_Absynpp_tqtd2doc( f->tq,( void*) f->type,({ struct Cyc_Core_Opt*
_temp3494=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3494->v=( void*) Cyc_PP_textptr( f->name); _temp3494;})); struct Cyc_PP_Doc*
_temp3490= Cyc_Absynpp_atts2doc( f->attributes); struct Cyc_PP_Doc* _temp3491=
Cyc_PP_text(({ unsigned char* _temp3492=( unsigned char*)";"; struct
_tagged_string _temp3493; _temp3493.curr= _temp3492; _temp3493.base= _temp3492;
_temp3493.last_plus_one= _temp3492 + 2; _temp3493;})); struct Cyc_PP_Doc*
_temp3487[ 3u]={ _temp3489, _temp3490, _temp3491}; struct _tagged_ptr1 _temp3488={
_temp3487, _temp3487, _temp3487 + 3u}; _temp3486( _temp3488);});}} struct Cyc_PP_Doc*
Cyc_Absynpp_structfields2doc( struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Structfield*), struct _tagged_string
sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_structfield2doc,({
unsigned char* _temp3495=( unsigned char*)""; struct _tagged_string _temp3496;
_temp3496.curr= _temp3495; _temp3496.base= _temp3495; _temp3496.last_plus_one=
_temp3495 + 1; _temp3496;}), fields);} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfield2doc(
struct Cyc_Absyn_Tunionfield* f){ return({ struct Cyc_PP_Doc*(* _temp3497)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3500= Cyc_Absynpp_scope2doc((
void*) f->sc); struct Cyc_PP_Doc* _temp3501= Cyc_Absynpp_typedef_name2doc( f->name);
struct Cyc_PP_Doc* _temp3502= f->tvs == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_ktvars2doc(
f->tvs); struct Cyc_PP_Doc* _temp3503= f->typs == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(
f->typs); struct Cyc_PP_Doc* _temp3498[ 4u]={ _temp3500, _temp3501, _temp3502,
_temp3503}; struct _tagged_ptr1 _temp3499={ _temp3498, _temp3498, _temp3498 + 4u};
_temp3497( _temp3499);});} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Tunionfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_tunionfield2doc,({ unsigned char* _temp3504=(
unsigned char*)","; struct _tagged_string _temp3505; _temp3505.curr= _temp3504;
_temp3505.base= _temp3504; _temp3505.last_plus_one= _temp3504 + 2; _temp3505;}),
fields);} void Cyc_Absynpp_decllist2file( struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE*
f){ for( 0; tdl != 0; tdl=(( struct Cyc_List_List*) _check_null( tdl))->tl){ Cyc_PP_file_of_doc(
Cyc_Absynpp_decl2doc(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( tdl))->hd), 72, f); fprintf( sfile_to_file( f),"\n");}} struct
_tagged_string Cyc_Absynpp_decllist2string( struct Cyc_List_List* tdl){ return
Cyc_PP_string_of_doc( Cyc_PP_seql(({ unsigned char* _temp3506=( unsigned char*)"";
struct _tagged_string _temp3507; _temp3507.curr= _temp3506; _temp3507.base=
_temp3506; _temp3507.last_plus_one= _temp3506 + 1; _temp3507;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Decl*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_decl2doc, tdl)), 72);} struct _tagged_string Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp* e){ return Cyc_PP_string_of_doc( Cyc_Absynpp_exp2doc( e),
72);} struct _tagged_string Cyc_Absynpp_stmt2string( struct Cyc_Absyn_Stmt* s){
return Cyc_PP_string_of_doc( Cyc_Absynpp_stmt2doc( s), 72);} struct
_tagged_string Cyc_Absynpp_typ2string( void* t){ return Cyc_PP_string_of_doc(
Cyc_Absynpp_typ2doc( t), 72);} struct _tagged_string Cyc_Absynpp_qvar2string(
struct _tuple0* v){ return Cyc_PP_string_of_doc( Cyc_Absynpp_qvar2doc( v), 72);}
struct _tagged_string Cyc_Absynpp_prim2string( void* p){ return Cyc_PP_string_of_doc(
Cyc_Absynpp_prim2doc( p), 72);} struct _tagged_string Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat* p){ return Cyc_PP_string_of_doc( Cyc_Absynpp_pat2doc( p),
72);} struct _tagged_string Cyc_Absynpp_scope2string( void* sc){ void* _temp3508=
sc; _LL3510: if( _temp3508 ==( void*) Cyc_Absyn_Static){ goto _LL3511;} else{
goto _LL3512;} _LL3512: if( _temp3508 ==( void*) Cyc_Absyn_Public){ goto _LL3513;}
else{ goto _LL3514;} _LL3514: if( _temp3508 ==( void*) Cyc_Absyn_Extern){ goto
_LL3515;} else{ goto _LL3516;} _LL3516: if( _temp3508 ==( void*) Cyc_Absyn_ExternC){
goto _LL3517;} else{ goto _LL3518;} _LL3518: if( _temp3508 ==( void*) Cyc_Absyn_Abstract){
goto _LL3519;} else{ goto _LL3509;} _LL3511: return({ unsigned char* _temp3520=(
unsigned char*)"static"; struct _tagged_string _temp3521; _temp3521.curr=
_temp3520; _temp3521.base= _temp3520; _temp3521.last_plus_one= _temp3520 + 7;
_temp3521;}); _LL3513: return({ unsigned char* _temp3522=( unsigned char*)"public";
struct _tagged_string _temp3523; _temp3523.curr= _temp3522; _temp3523.base=
_temp3522; _temp3523.last_plus_one= _temp3522 + 7; _temp3523;}); _LL3515: return({
unsigned char* _temp3524=( unsigned char*)"extern"; struct _tagged_string
_temp3525; _temp3525.curr= _temp3524; _temp3525.base= _temp3524; _temp3525.last_plus_one=
_temp3524 + 7; _temp3525;}); _LL3517: return({ unsigned char* _temp3526=(
unsigned char*)"extern \"C\""; struct _tagged_string _temp3527; _temp3527.curr=
_temp3526; _temp3527.base= _temp3526; _temp3527.last_plus_one= _temp3526 + 11;
_temp3527;}); _LL3519: return({ unsigned char* _temp3528=( unsigned char*)"abstract";
struct _tagged_string _temp3529; _temp3529.curr= _temp3528; _temp3529.base=
_temp3528; _temp3529.last_plus_one= _temp3528 + 9; _temp3529;}); _LL3509:;}