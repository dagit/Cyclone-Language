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
extern int Cyc_Absynpp_expand_typedefs; extern int Cyc_Absynpp_qvar_to_Cids;
extern int Cyc_Absynpp_to_VC; extern int Cyc_Absynpp_decls_first; extern int Cyc_Absynpp_print_all_tvars;
extern int Cyc_Absynpp_print_all_kinds; extern int Cyc_Absynpp_print_using_stmts;
extern int Cyc_Absynpp_print_externC_stmts; extern int Cyc_Absynpp_print_full_evars;
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
p); extern struct _tagged_string Cyc_Absynpp_scope2string( void* sc); extern int
Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp*); extern struct _tagged_string Cyc_Absynpp_char_escape(
unsigned char); extern struct _tagged_string Cyc_Absynpp_string_escape( struct
_tagged_string); extern struct _tagged_string Cyc_Absynpp_prim2str( void* p);
extern int Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s); struct _tuple3{
struct _tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; extern
struct _tuple1* Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg); struct _tuple4{
struct Cyc_Absyn_Tqual f1; void* f2; struct Cyc_List_List* f3; } ; extern struct
_tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual tq, void* t); struct _tuple5{
struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; extern struct Cyc_PP_Doc*
Cyc_Absynpp_dp2doc( struct _tuple5* dp); extern struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs); extern struct Cyc_PP_Doc* Cyc_Absynpp_switchCclauses2doc(
struct Cyc_List_List* cs); extern struct Cyc_PP_Doc* Cyc_Absynpp_typ2doc( void*);
extern struct Cyc_PP_Doc* Cyc_Absynpp_structfields2doc( struct Cyc_List_List*
fields); extern struct Cyc_PP_Doc* Cyc_Absynpp_scope2doc( void*); extern struct
Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct Cyc_Absyn_Stmt*); extern struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp*); extern struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec(
int inprec, struct Cyc_Absyn_Exp* e); extern struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec(
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
int Cyc_Absynpp_expand_typedefs= 0; int Cyc_Absynpp_qvar_to_Cids= 0; int Cyc_Absynpp_to_VC=
0; int Cyc_Absynpp_decls_first= 0; int Cyc_Absynpp_dont_rewrite_temp_tvars= 0;
int Cyc_Absynpp_print_all_tvars= 0; int Cyc_Absynpp_print_all_kinds= 0; int Cyc_Absynpp_print_using_stmts=
0; int Cyc_Absynpp_print_externC_stmts= 0; int Cyc_Absynpp_print_full_evars= 0;
static struct Cyc_List_List* Cyc_Absynpp_curr_namespace= 0; static void Cyc_Absynpp_curr_namespace_add(
struct _tagged_string* v){ Cyc_Absynpp_curr_namespace=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_Absynpp_curr_namespace,({
struct Cyc_List_List* _temp0=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp0->hd=( void*) v; _temp0->tl= 0; _temp0;}));} static void
Cyc_Absynpp_suppr_last( struct Cyc_List_List** l){ if((( struct Cyc_List_List*)
_check_null(* l))->tl == 0){* l= 0;} else{(( void(*)( struct Cyc_List_List** l))
Cyc_Absynpp_suppr_last)(&(( struct Cyc_List_List*) _check_null(* l))->tl);}}
static void Cyc_Absynpp_curr_namespace_drop(){(( void(*)( struct Cyc_List_List**
l)) Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);} struct
_tagged_string Cyc_Absynpp_char_escape( unsigned char c){ switch( c){ case '\a':
_LL1: return({ unsigned char* _temp3=( unsigned char*)"\\a"; struct
_tagged_string _temp4; _temp4.curr= _temp3; _temp4.base= _temp3; _temp4.last_plus_one=
_temp3 + 3; _temp4;}); case '\b': _LL2: return({ unsigned char* _temp6=(
unsigned char*)"\\b"; struct _tagged_string _temp7; _temp7.curr= _temp6; _temp7.base=
_temp6; _temp7.last_plus_one= _temp6 + 3; _temp7;}); case '\f': _LL5: return({
unsigned char* _temp9=( unsigned char*)"\\f"; struct _tagged_string _temp10;
_temp10.curr= _temp9; _temp10.base= _temp9; _temp10.last_plus_one= _temp9 + 3;
_temp10;}); case '\n': _LL8: return({ unsigned char* _temp12=( unsigned char*)"\\n";
struct _tagged_string _temp13; _temp13.curr= _temp12; _temp13.base= _temp12;
_temp13.last_plus_one= _temp12 + 3; _temp13;}); case '\r': _LL11: return({
unsigned char* _temp15=( unsigned char*)"\\r"; struct _tagged_string _temp16;
_temp16.curr= _temp15; _temp16.base= _temp15; _temp16.last_plus_one= _temp15 + 3;
_temp16;}); case '\t': _LL14: return({ unsigned char* _temp18=( unsigned char*)"\\t";
struct _tagged_string _temp19; _temp19.curr= _temp18; _temp19.base= _temp18;
_temp19.last_plus_one= _temp18 + 3; _temp19;}); case '\v': _LL17: return({
unsigned char* _temp21=( unsigned char*)"\\v"; struct _tagged_string _temp22;
_temp22.curr= _temp21; _temp22.base= _temp21; _temp22.last_plus_one= _temp21 + 3;
_temp22;}); case '\\': _LL20: return({ unsigned char* _temp24=( unsigned char*)"\\\\";
struct _tagged_string _temp25; _temp25.curr= _temp24; _temp25.base= _temp24;
_temp25.last_plus_one= _temp24 + 3; _temp25;}); case '"': _LL23: return({
unsigned char* _temp27=( unsigned char*)"\""; struct _tagged_string _temp28;
_temp28.curr= _temp27; _temp28.base= _temp27; _temp28.last_plus_one= _temp27 + 2;
_temp28;}); case '\'': _LL26: return({ unsigned char* _temp30=( unsigned char*)"\\'";
struct _tagged_string _temp31; _temp31.curr= _temp30; _temp31.base= _temp30;
_temp31.last_plus_one= _temp30 + 3; _temp31;}); default: _LL29: if( c >=' '? c
<='~': 0){ struct _tagged_string _temp33= Cyc_Core_new_string( 1); goto _LL34;
_LL34:*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( _temp33, sizeof( unsigned char), 0)= c; return(
struct _tagged_string) _temp33;} else{ struct _tagged_string _temp35= Cyc_Core_new_string(
4); goto _LL36; _LL36: { int j= 0;*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( _temp35, sizeof(
unsigned char), j ++)='\\';*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( _temp35, sizeof(
unsigned char), j ++)=( unsigned char)('0' +(( unsigned char) c >> 6 & 7));*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( _temp35, sizeof( unsigned char), j ++)=(
unsigned char)('0' +( c >> 3 & 7));*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( _temp35, sizeof(
unsigned char), j ++)=( unsigned char)('0' +( c & 7)); return( struct
_tagged_string) _temp35;}}}} static int Cyc_Absynpp_special( struct
_tagged_string s){ int sz=( int)(({ struct _tagged_string _temp37= s;(
unsigned int)( _temp37.last_plus_one - _temp37.curr);}) - 1);{ int i= 0; for( 0;
i < sz; i ++){ unsigned char c=*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( unsigned char), i); if((( c <=' '? 1: c >='~')? 1: c =='"')? 1: c =='\\'){
return 1;}}} return 0;} struct _tagged_string Cyc_Absynpp_string_escape( struct
_tagged_string s){ if( ! Cyc_Absynpp_special( s)){ return s;}{ int n=( int)(({
struct _tagged_string _temp82= s;( unsigned int)( _temp82.last_plus_one -
_temp82.curr);}) - 1); if( n > 0?*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( unsigned char), n) =='\000': 0){ n --;}{ int len= 0;{ int i= 0; for( 0;
i <= n; i ++){ unsigned char _temp38=*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( unsigned char), i); _LL40: if( _temp38 =='\a'){ goto _LL41;} else{ goto
_LL42;} _LL42: if( _temp38 =='\b'){ goto _LL43;} else{ goto _LL44;} _LL44: if(
_temp38 =='\f'){ goto _LL45;} else{ goto _LL46;} _LL46: if( _temp38 =='\n'){
goto _LL47;} else{ goto _LL48;} _LL48: if( _temp38 =='\r'){ goto _LL49;} else{
goto _LL50;} _LL50: if( _temp38 =='\t'){ goto _LL51;} else{ goto _LL52;} _LL52:
if( _temp38 =='\v'){ goto _LL53;} else{ goto _LL54;} _LL54: if( _temp38 =='\\'){
goto _LL55;} else{ goto _LL56;} _LL56: if( _temp38 =='"'){ goto _LL57;} else{
goto _LL58;} _LL58: goto _LL59; _LL41: goto _LL43; _LL43: goto _LL45; _LL45:
goto _LL47; _LL47: goto _LL49; _LL49: goto _LL51; _LL51: goto _LL53; _LL53: goto
_LL55; _LL55: goto _LL57; _LL57: len += 2; goto _LL39; _LL59: if( _temp38 >=' '?
_temp38 <='~': 0){ len ++;} else{ len += 4;} goto _LL39; _LL39:;}}{ struct
_tagged_string t= Cyc_Core_new_string( len); int j= 0;{ int i= 0; for( 0; i <= n;
i ++){ unsigned char _temp60=*(( const unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char),
i); _LL62: if( _temp60 =='\a'){ goto _LL63;} else{ goto _LL64;} _LL64: if(
_temp60 =='\b'){ goto _LL65;} else{ goto _LL66;} _LL66: if( _temp60 =='\f'){
goto _LL67;} else{ goto _LL68;} _LL68: if( _temp60 =='\n'){ goto _LL69;} else{
goto _LL70;} _LL70: if( _temp60 =='\r'){ goto _LL71;} else{ goto _LL72;} _LL72:
if( _temp60 =='\t'){ goto _LL73;} else{ goto _LL74;} _LL74: if( _temp60 =='\v'){
goto _LL75;} else{ goto _LL76;} _LL76: if( _temp60 =='\\'){ goto _LL77;} else{
goto _LL78;} _LL78: if( _temp60 =='"'){ goto _LL79;} else{ goto _LL80;} _LL80:
goto _LL81; _LL63:*(( unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( t, sizeof( unsigned char), j ++)='\\';*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='a'; goto _LL61;
_LL65:*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='\\';*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='b'; goto _LL61;
_LL67:*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='\\';*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='f'; goto _LL61;
_LL69:*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='\\';*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='n'; goto _LL61;
_LL71:*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='\\';*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='r'; goto _LL61;
_LL73:*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='\\';*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='t'; goto _LL61;
_LL75:*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='\\';*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='v'; goto _LL61;
_LL77:*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='\\';*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='\\'; goto _LL61;
_LL79:*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='\\';*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='"'; goto _LL61;
_LL81: if( _temp60 >=' '? _temp60 <='~': 0){*(( unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( t,
sizeof( unsigned char), j ++)= _temp60;} else{*(( unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( t,
sizeof( unsigned char), j ++)='\\';*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( t, sizeof( unsigned char),
j ++)=( unsigned char)('0' +( _temp60 >> 6 & 7));*(( unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( t,
sizeof( unsigned char), j ++)=( unsigned char)('0' +( _temp60 >> 3 & 7));*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)=( unsigned char)('0'
+( _temp60 & 7));} goto _LL61; _LL61:;}} return( struct _tagged_string) t;}}}}
static unsigned char _temp85[ 9u]="restrict"; static struct _tagged_string Cyc_Absynpp_restrict_string={
_temp85, _temp85, _temp85 + 9u}; static unsigned char _temp88[ 9u]="volatile";
static struct _tagged_string Cyc_Absynpp_volatile_string={ _temp88, _temp88,
_temp88 + 9u}; static unsigned char _temp91[ 6u]="const"; static struct
_tagged_string Cyc_Absynpp_const_str={ _temp91, _temp91, _temp91 + 6u}; static
struct _tagged_string* Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string;
static struct _tagged_string* Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _tagged_string* Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
struct Cyc_PP_Doc* Cyc_Absynpp_tqual2doc( struct Cyc_Absyn_Tqual tq){ struct Cyc_List_List*
l= 0; if( tq.q_restrict){ l=({ struct Cyc_List_List* _temp92=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp92->hd=( void*) Cyc_Absynpp_restrict_sp;
_temp92->tl= l; _temp92;});} if( tq.q_volatile){ l=({ struct Cyc_List_List*
_temp93=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp93->hd=( void*) Cyc_Absynpp_volatile_sp; _temp93->tl= l; _temp93;});} if(
tq.q_const){ l=({ struct Cyc_List_List* _temp94=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp94->hd=( void*) Cyc_Absynpp_const_sp;
_temp94->tl= l; _temp94;});} return Cyc_PP_egroup(({ unsigned char* _temp95=(
unsigned char*)""; struct _tagged_string _temp96; _temp96.curr= _temp95; _temp96.base=
_temp95; _temp96.last_plus_one= _temp95 + 1; _temp96;}),({ unsigned char*
_temp97=( unsigned char*)" "; struct _tagged_string _temp98; _temp98.curr=
_temp97; _temp98.base= _temp97; _temp98.last_plus_one= _temp97 + 2; _temp98;}),({
unsigned char* _temp99=( unsigned char*)" "; struct _tagged_string _temp100;
_temp100.curr= _temp99; _temp100.base= _temp99; _temp100.last_plus_one= _temp99
+ 2; _temp100;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_PP_textptr, l));}
struct _tagged_string Cyc_Absynpp_kind2string( void* k){ void* _temp101= k;
_LL103: if( _temp101 ==( void*) Cyc_Absyn_AnyKind){ goto _LL104;} else{ goto
_LL105;} _LL105: if( _temp101 ==( void*) Cyc_Absyn_MemKind){ goto _LL106;} else{
goto _LL107;} _LL107: if( _temp101 ==( void*) Cyc_Absyn_BoxKind){ goto _LL108;}
else{ goto _LL109;} _LL109: if( _temp101 ==( void*) Cyc_Absyn_RgnKind){ goto
_LL110;} else{ goto _LL111;} _LL111: if( _temp101 ==( void*) Cyc_Absyn_EffKind){
goto _LL112;} else{ goto _LL102;} _LL104: return({ unsigned char* _temp113=(
unsigned char*)"A"; struct _tagged_string _temp114; _temp114.curr= _temp113;
_temp114.base= _temp113; _temp114.last_plus_one= _temp113 + 2; _temp114;});
_LL106: return({ unsigned char* _temp115=( unsigned char*)"M"; struct
_tagged_string _temp116; _temp116.curr= _temp115; _temp116.base= _temp115;
_temp116.last_plus_one= _temp115 + 2; _temp116;}); _LL108: return({
unsigned char* _temp117=( unsigned char*)"B"; struct _tagged_string _temp118;
_temp118.curr= _temp117; _temp118.base= _temp117; _temp118.last_plus_one=
_temp117 + 2; _temp118;}); _LL110: return({ unsigned char* _temp119=(
unsigned char*)"R"; struct _tagged_string _temp120; _temp120.curr= _temp119;
_temp120.base= _temp119; _temp120.last_plus_one= _temp119 + 2; _temp120;});
_LL112: return({ unsigned char* _temp121=( unsigned char*)"E"; struct
_tagged_string _temp122; _temp122.curr= _temp121; _temp122.base= _temp121;
_temp122.last_plus_one= _temp121 + 2; _temp122;}); _LL102:;} struct
_tagged_string Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref* c){ void*
_temp123=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( c))->v; void* _temp129; _LL125: if(( unsigned int)
_temp123 > 1u?*(( int*) _temp123) == Cyc_Absyn_Eq_constr: 0){ _LL130: _temp129=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp123)->f1; goto _LL126;} else{
goto _LL127;} _LL127: goto _LL128; _LL126: return Cyc_Absynpp_kind2string(
_temp129); _LL128: return({ unsigned char* _temp131=( unsigned char*)"?"; struct
_tagged_string _temp132; _temp132.curr= _temp131; _temp132.base= _temp131;
_temp132.last_plus_one= _temp131 + 2; _temp132;}); _LL124:;} struct Cyc_PP_Doc*
Cyc_Absynpp_kind2doc( void* k){ return Cyc_PP_text( Cyc_Absynpp_kind2string( k));}
struct Cyc_PP_Doc* Cyc_Absynpp_ckind2doc( struct Cyc_Absyn_Conref* c){ return
Cyc_PP_text( Cyc_Absynpp_ckind2string( c));} struct Cyc_PP_Doc* Cyc_Absynpp_tps2doc(
struct Cyc_List_List* ts){ return Cyc_PP_egroup(({ unsigned char* _temp133=(
unsigned char*)"<"; struct _tagged_string _temp134; _temp134.curr= _temp133;
_temp134.base= _temp133; _temp134.last_plus_one= _temp133 + 2; _temp134;}),({
unsigned char* _temp135=( unsigned char*)">"; struct _tagged_string _temp136;
_temp136.curr= _temp135; _temp136.base= _temp135; _temp136.last_plus_one=
_temp135 + 2; _temp136;}),({ unsigned char* _temp137=( unsigned char*)",";
struct _tagged_string _temp138; _temp138.curr= _temp137; _temp138.base= _temp137;
_temp138.last_plus_one= _temp137 + 2; _temp138;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_typ2doc,
ts));} struct Cyc_PP_Doc* Cyc_Absynpp_ktvar2doc( struct Cyc_Absyn_Tvar* tv){
void* _temp139=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( tv->kind))->v; void* _temp147; void* _temp149;
_LL141: if(( unsigned int) _temp139 > 1u?*(( int*) _temp139) == Cyc_Absyn_Eq_constr:
0){ _LL148: _temp147=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp139)->f1;
if( _temp147 ==( void*) Cyc_Absyn_BoxKind){ goto _LL142;} else{ goto _LL143;}}
else{ goto _LL143;} _LL143: if(( unsigned int) _temp139 > 1u?*(( int*) _temp139)
== Cyc_Absyn_Eq_constr: 0){ _LL150: _temp149=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp139)->f1; goto _LL144;} else{ goto _LL145;} _LL145: goto _LL146; _LL142:
return Cyc_PP_textptr( tv->name); _LL144: return({ struct Cyc_PP_Doc*(* _temp151)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp154= Cyc_PP_textptr(
tv->name); struct Cyc_PP_Doc* _temp155= Cyc_PP_text(({ unsigned char* _temp157=(
unsigned char*)"::"; struct _tagged_string _temp158; _temp158.curr= _temp157;
_temp158.base= _temp157; _temp158.last_plus_one= _temp157 + 3; _temp158;}));
struct Cyc_PP_Doc* _temp156= Cyc_Absynpp_kind2doc( _temp149); struct Cyc_PP_Doc*
_temp152[ 3u]={ _temp154, _temp155, _temp156}; struct _tagged_ptr1 _temp153={
_temp152, _temp152, _temp152 + 3u}; _temp151( _temp153);}); _LL146: return({
struct Cyc_PP_Doc*(* _temp159)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp162= Cyc_PP_textptr( tv->name); struct Cyc_PP_Doc* _temp163= Cyc_PP_text(({
unsigned char* _temp164=( unsigned char*)"/*::?*/"; struct _tagged_string
_temp165; _temp165.curr= _temp164; _temp165.base= _temp164; _temp165.last_plus_one=
_temp164 + 8; _temp165;})); struct Cyc_PP_Doc* _temp160[ 2u]={ _temp162,
_temp163}; struct _tagged_ptr1 _temp161={ _temp160, _temp160, _temp160 + 2u};
_temp159( _temp161);}); _LL140:;} struct Cyc_PP_Doc* Cyc_Absynpp_ktvars2doc(
struct Cyc_List_List* tvs){ return Cyc_PP_egroup(({ unsigned char* _temp166=(
unsigned char*)"<"; struct _tagged_string _temp167; _temp167.curr= _temp166;
_temp167.base= _temp166; _temp167.last_plus_one= _temp166 + 2; _temp167;}),({
unsigned char* _temp168=( unsigned char*)">"; struct _tagged_string _temp169;
_temp169.curr= _temp168; _temp169.base= _temp168; _temp169.last_plus_one=
_temp168 + 2; _temp169;}),({ unsigned char* _temp170=( unsigned char*)",";
struct _tagged_string _temp171; _temp171.curr= _temp170; _temp171.base= _temp170;
_temp171.last_plus_one= _temp170 + 2; _temp171;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_ktvar2doc, tvs));} static struct _tagged_string* Cyc_Absynpp_get_name(
struct Cyc_Absyn_Tvar* tv){ return tv->name;} struct Cyc_PP_Doc* Cyc_Absynpp_tvars2doc(
struct Cyc_List_List* tvs){ if( Cyc_Absynpp_print_all_kinds){ return Cyc_Absynpp_ktvars2doc(
tvs);} return Cyc_PP_egroup(({ unsigned char* _temp172=( unsigned char*)"<";
struct _tagged_string _temp173; _temp173.curr= _temp172; _temp173.base= _temp172;
_temp173.last_plus_one= _temp172 + 2; _temp173;}),({ unsigned char* _temp174=(
unsigned char*)">"; struct _tagged_string _temp175; _temp175.curr= _temp174;
_temp175.base= _temp174; _temp175.last_plus_one= _temp174 + 2; _temp175;}),({
unsigned char* _temp176=( unsigned char*)","; struct _tagged_string _temp177;
_temp177.curr= _temp176; _temp177.base= _temp176; _temp177.last_plus_one=
_temp176 + 2; _temp177;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_PP_textptr,((
struct Cyc_List_List*(*)( struct _tagged_string*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_get_name, tvs)));} struct
_tuple7{ struct Cyc_Absyn_Tqual f1; void* f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_arg2doc(
struct _tuple7* t){ return Cyc_Absynpp_tqtd2doc((* t).f1,(* t).f2, 0);} struct
Cyc_PP_Doc* Cyc_Absynpp_args2doc( struct Cyc_List_List* ts){ return Cyc_PP_group(({
unsigned char* _temp178=( unsigned char*)"("; struct _tagged_string _temp179;
_temp179.curr= _temp178; _temp179.base= _temp178; _temp179.last_plus_one=
_temp178 + 2; _temp179;}),({ unsigned char* _temp180=( unsigned char*)")";
struct _tagged_string _temp181; _temp181.curr= _temp180; _temp181.base= _temp180;
_temp181.last_plus_one= _temp180 + 2; _temp181;}),({ unsigned char* _temp182=(
unsigned char*)","; struct _tagged_string _temp183; _temp183.curr= _temp182;
_temp183.base= _temp182; _temp183.last_plus_one= _temp182 + 2; _temp183;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple7*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg2doc, ts));} struct Cyc_PP_Doc* Cyc_Absynpp_noncallatt2doc(
void* att){ void* _temp184= att; _LL186: if( _temp184 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL187;} else{ goto _LL188;} _LL188: if( _temp184 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL189;} else{ goto _LL190;} _LL190: if( _temp184 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL191;} else{ goto _LL192;} _LL192: goto _LL193; _LL187: return Cyc_PP_nil_doc();
_LL189: return Cyc_PP_nil_doc(); _LL191: return Cyc_PP_nil_doc(); _LL193: return
Cyc_PP_text( Cyc_Absyn_attribute2string( att)); _LL185:;} struct Cyc_PP_Doc* Cyc_Absynpp_callconv2doc(
struct Cyc_List_List* atts){ for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp194=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; _LL196: if( _temp194 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL197;} else{ goto _LL198;} _LL198: if( _temp194 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL199;} else{ goto _LL200;} _LL200: if( _temp194 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL201;} else{ goto _LL202;} _LL202: goto _LL203; _LL197: return Cyc_PP_text(({
unsigned char* _temp204=( unsigned char*)"_stdcall"; struct _tagged_string
_temp205; _temp205.curr= _temp204; _temp205.base= _temp204; _temp205.last_plus_one=
_temp204 + 9; _temp205;})); _LL199: return Cyc_PP_text(({ unsigned char*
_temp206=( unsigned char*)"_cdecl"; struct _tagged_string _temp207; _temp207.curr=
_temp206; _temp207.base= _temp206; _temp207.last_plus_one= _temp206 + 7;
_temp207;})); _LL201: return Cyc_PP_text(({ unsigned char* _temp208=(
unsigned char*)"_fastcall"; struct _tagged_string _temp209; _temp209.curr=
_temp208; _temp209.base= _temp208; _temp209.last_plus_one= _temp208 + 10;
_temp209;})); _LL203: goto _LL195; _LL195:;} return Cyc_PP_nil_doc();} struct
Cyc_PP_Doc* Cyc_Absynpp_noncallconv2doc( struct Cyc_List_List* atts){ int hasatt=
0;{ struct Cyc_List_List* atts2= atts; for( 0; atts2 != 0; atts2=(( struct Cyc_List_List*)
_check_null( atts2))->tl){ void* _temp210=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; _LL212: if( _temp210 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL213;} else{ goto _LL214;} _LL214: if( _temp210 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL215;} else{ goto _LL216;} _LL216: if( _temp210 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL217;} else{ goto _LL218;} _LL218: goto _LL219; _LL213: goto _LL211;
_LL215: goto _LL211; _LL217: goto _LL211; _LL219: hasatt= 1; goto _LL211; _LL211:;}}
if( ! hasatt){ return Cyc_PP_nil_doc();} return({ struct Cyc_PP_Doc*(* _temp220)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp223= Cyc_PP_text(({
unsigned char* _temp234=( unsigned char*)"__declspec("; struct _tagged_string
_temp235; _temp235.curr= _temp234; _temp235.base= _temp234; _temp235.last_plus_one=
_temp234 + 12; _temp235;})); struct Cyc_PP_Doc* _temp224= Cyc_PP_group(({
unsigned char* _temp228=( unsigned char*)""; struct _tagged_string _temp229;
_temp229.curr= _temp228; _temp229.base= _temp228; _temp229.last_plus_one=
_temp228 + 1; _temp229;}),({ unsigned char* _temp230=( unsigned char*)""; struct
_tagged_string _temp231; _temp231.curr= _temp230; _temp231.base= _temp230;
_temp231.last_plus_one= _temp230 + 1; _temp231;}),({ unsigned char* _temp232=(
unsigned char*)" "; struct _tagged_string _temp233; _temp233.curr= _temp232;
_temp233.base= _temp232; _temp233.last_plus_one= _temp232 + 2; _temp233;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_noncallatt2doc, atts)); struct Cyc_PP_Doc*
_temp225= Cyc_PP_text(({ unsigned char* _temp226=( unsigned char*)")"; struct
_tagged_string _temp227; _temp227.curr= _temp226; _temp227.base= _temp226;
_temp227.last_plus_one= _temp226 + 2; _temp227;})); struct Cyc_PP_Doc* _temp221[
3u]={ _temp223, _temp224, _temp225}; struct _tagged_ptr1 _temp222={ _temp221,
_temp221, _temp221 + 3u}; _temp220( _temp222);});} struct Cyc_PP_Doc* Cyc_Absynpp_att2doc(
void* a){ return Cyc_PP_text( Cyc_Absyn_attribute2string( a));} struct Cyc_PP_Doc*
Cyc_Absynpp_atts2doc( struct Cyc_List_List* atts){ if( atts == 0){ return Cyc_PP_nil_doc();}
if( Cyc_Absynpp_to_VC){ return Cyc_Absynpp_noncallconv2doc( atts);} return({
struct Cyc_PP_Doc*(* _temp236)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp239= Cyc_PP_text(({ unsigned char* _temp247=( unsigned char*)" __attribute__";
struct _tagged_string _temp248; _temp248.curr= _temp247; _temp248.base= _temp247;
_temp248.last_plus_one= _temp247 + 15; _temp248;})); struct Cyc_PP_Doc* _temp240=
Cyc_PP_group(({ unsigned char* _temp241=( unsigned char*)"(("; struct
_tagged_string _temp242; _temp242.curr= _temp241; _temp242.base= _temp241;
_temp242.last_plus_one= _temp241 + 3; _temp242;}),({ unsigned char* _temp243=(
unsigned char*)"))"; struct _tagged_string _temp244; _temp244.curr= _temp243;
_temp244.base= _temp243; _temp244.last_plus_one= _temp243 + 3; _temp244;}),({
unsigned char* _temp245=( unsigned char*)","; struct _tagged_string _temp246;
_temp246.curr= _temp245; _temp246.base= _temp245; _temp246.last_plus_one=
_temp245 + 2; _temp246;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_att2doc, atts));
struct Cyc_PP_Doc* _temp237[ 2u]={ _temp239, _temp240}; struct _tagged_ptr1
_temp238={ _temp237, _temp237, _temp237 + 2u}; _temp236( _temp238);});} int Cyc_Absynpp_next_is_pointer(
struct Cyc_List_List* tms){ if( tms == 0){ return 0;}{ void* _temp249=( void*)((
struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Tqual _temp255;
void* _temp257; void* _temp259; _LL251: if(( unsigned int) _temp249 > 1u?*(( int*)
_temp249) == Cyc_Absyn_Pointer_mod: 0){ _LL260: _temp259=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp249)->f1; goto _LL258; _LL258: _temp257=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp249)->f2; goto _LL256; _LL256: _temp255=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp249)->f3; goto _LL252;} else{ goto _LL253;} _LL253: goto _LL254; _LL252:
return 1; _LL254: return 0; _LL250:;}} extern struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc(
void* t); struct Cyc_PP_Doc* Cyc_Absynpp_dtms2doc( struct Cyc_PP_Doc* d, struct
Cyc_List_List* tms){ if( tms == 0){ return d;}{ struct Cyc_PP_Doc* rest= Cyc_Absynpp_dtms2doc(
d,(( struct Cyc_List_List*) _check_null( tms))->tl); struct Cyc_PP_Doc* p_rest=({
struct Cyc_PP_Doc*(* _temp428)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp431= Cyc_PP_text(({ unsigned char* _temp436=( unsigned char*)"("; struct
_tagged_string _temp437; _temp437.curr= _temp436; _temp437.base= _temp436;
_temp437.last_plus_one= _temp436 + 2; _temp437;})); struct Cyc_PP_Doc* _temp432=
rest; struct Cyc_PP_Doc* _temp433= Cyc_PP_text(({ unsigned char* _temp434=(
unsigned char*)")"; struct _tagged_string _temp435; _temp435.curr= _temp434;
_temp435.base= _temp434; _temp435.last_plus_one= _temp434 + 2; _temp435;}));
struct Cyc_PP_Doc* _temp429[ 3u]={ _temp431, _temp432, _temp433}; struct
_tagged_ptr1 _temp430={ _temp429, _temp429, _temp429 + 3u}; _temp428( _temp430);});
void* _temp261=( void*)(( struct Cyc_List_List*) _check_null( tms))->hd; struct
Cyc_Absyn_Exp* _temp275; void* _temp277; struct Cyc_List_List* _temp279; struct
Cyc_Position_Segment* _temp281; int _temp283; struct Cyc_Position_Segment*
_temp285; struct Cyc_List_List* _temp287; struct Cyc_Absyn_Tqual _temp289; void*
_temp291; void* _temp293; _LL263: if( _temp261 ==( void*) Cyc_Absyn_Carray_mod){
goto _LL264;} else{ goto _LL265;} _LL265: if(( unsigned int) _temp261 > 1u?*((
int*) _temp261) == Cyc_Absyn_ConstArray_mod: 0){ _LL276: _temp275=(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp261)->f1; goto _LL266;} else{ goto _LL267;} _LL267: if(( unsigned int)
_temp261 > 1u?*(( int*) _temp261) == Cyc_Absyn_Function_mod: 0){ _LL278:
_temp277=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp261)->f1; goto
_LL268;} else{ goto _LL269;} _LL269: if(( unsigned int) _temp261 > 1u?*(( int*)
_temp261) == Cyc_Absyn_Attributes_mod: 0){ _LL282: _temp281=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp261)->f1; goto _LL280; _LL280: _temp279=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp261)->f2; goto _LL270;} else{ goto _LL271;} _LL271: if(( unsigned int)
_temp261 > 1u?*(( int*) _temp261) == Cyc_Absyn_TypeParams_mod: 0){ _LL288:
_temp287=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp261)->f1; goto _LL286;
_LL286: _temp285=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp261)->f2; goto
_LL284; _LL284: _temp283=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp261)->f3;
goto _LL272;} else{ goto _LL273;} _LL273: if(( unsigned int) _temp261 > 1u?*((
int*) _temp261) == Cyc_Absyn_Pointer_mod: 0){ _LL294: _temp293=( void*)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp261)->f1; goto _LL292; _LL292: _temp291=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp261)->f2; goto _LL290;
_LL290: _temp289=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp261)->f3; goto
_LL274;} else{ goto _LL262;} _LL264: if( Cyc_Absynpp_next_is_pointer((( struct
Cyc_List_List*) _check_null( tms))->tl)){ rest= p_rest;} return({ struct Cyc_PP_Doc*(*
_temp295)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp298= rest;
struct Cyc_PP_Doc* _temp299= Cyc_PP_text(({ unsigned char* _temp300=(
unsigned char*)"[]"; struct _tagged_string _temp301; _temp301.curr= _temp300;
_temp301.base= _temp300; _temp301.last_plus_one= _temp300 + 3; _temp301;}));
struct Cyc_PP_Doc* _temp296[ 2u]={ _temp298, _temp299}; struct _tagged_ptr1
_temp297={ _temp296, _temp296, _temp296 + 2u}; _temp295( _temp297);}); _LL266:
if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*) _check_null( tms))->tl)){
rest= p_rest;} return({ struct Cyc_PP_Doc*(* _temp302)( struct _tagged_ptr1)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp305= rest; struct Cyc_PP_Doc* _temp306= Cyc_PP_text(({
unsigned char* _temp311=( unsigned char*)"["; struct _tagged_string _temp312;
_temp312.curr= _temp311; _temp312.base= _temp311; _temp312.last_plus_one=
_temp311 + 2; _temp312;})); struct Cyc_PP_Doc* _temp307= Cyc_Absynpp_exp2doc(
_temp275); struct Cyc_PP_Doc* _temp308= Cyc_PP_text(({ unsigned char* _temp309=(
unsigned char*)"]"; struct _tagged_string _temp310; _temp310.curr= _temp309;
_temp310.base= _temp309; _temp310.last_plus_one= _temp309 + 2; _temp310;}));
struct Cyc_PP_Doc* _temp303[ 4u]={ _temp305, _temp306, _temp307, _temp308};
struct _tagged_ptr1 _temp304={ _temp303, _temp303, _temp303 + 4u}; _temp302(
_temp304);}); _LL268: if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*)
_check_null( tms))->tl)){ rest= p_rest;}{ void* _temp313= _temp277; struct Cyc_List_List*
_temp319; struct Cyc_Core_Opt* _temp321; struct Cyc_Absyn_VarargInfo* _temp323;
int _temp325; struct Cyc_List_List* _temp327; struct Cyc_Position_Segment*
_temp329; struct Cyc_List_List* _temp331; _LL315: if(*(( int*) _temp313) == Cyc_Absyn_WithTypes){
_LL328: _temp327=(( struct Cyc_Absyn_WithTypes_struct*) _temp313)->f1; goto
_LL326; _LL326: _temp325=(( struct Cyc_Absyn_WithTypes_struct*) _temp313)->f2;
goto _LL324; _LL324: _temp323=(( struct Cyc_Absyn_WithTypes_struct*) _temp313)->f3;
goto _LL322; _LL322: _temp321=(( struct Cyc_Absyn_WithTypes_struct*) _temp313)->f4;
goto _LL320; _LL320: _temp319=(( struct Cyc_Absyn_WithTypes_struct*) _temp313)->f5;
goto _LL316;} else{ goto _LL317;} _LL317: if(*(( int*) _temp313) == Cyc_Absyn_NoTypes){
_LL332: _temp331=(( struct Cyc_Absyn_NoTypes_struct*) _temp313)->f1; goto _LL330;
_LL330: _temp329=(( struct Cyc_Absyn_NoTypes_struct*) _temp313)->f2; goto _LL318;}
else{ goto _LL314;} _LL316: return({ struct Cyc_PP_Doc*(* _temp333)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp336= rest; struct Cyc_PP_Doc*
_temp337= Cyc_Absynpp_funargs2doc( _temp327, _temp325, _temp323, _temp321,
_temp319); struct Cyc_PP_Doc* _temp334[ 2u]={ _temp336, _temp337}; struct
_tagged_ptr1 _temp335={ _temp334, _temp334, _temp334 + 2u}; _temp333( _temp335);});
_LL318: return({ struct Cyc_PP_Doc*(* _temp338)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp341= rest; struct Cyc_PP_Doc* _temp342= Cyc_PP_group(({
unsigned char* _temp343=( unsigned char*)"("; struct _tagged_string _temp344;
_temp344.curr= _temp343; _temp344.base= _temp343; _temp344.last_plus_one=
_temp343 + 2; _temp344;}),({ unsigned char* _temp345=( unsigned char*)")";
struct _tagged_string _temp346; _temp346.curr= _temp345; _temp346.base= _temp345;
_temp346.last_plus_one= _temp345 + 2; _temp346;}),({ unsigned char* _temp347=(
unsigned char*)","; struct _tagged_string _temp348; _temp348.curr= _temp347;
_temp348.base= _temp347; _temp348.last_plus_one= _temp347 + 2; _temp348;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tagged_string*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_PP_textptr, _temp331)); struct Cyc_PP_Doc*
_temp339[ 2u]={ _temp341, _temp342}; struct _tagged_ptr1 _temp340={ _temp339,
_temp339, _temp339 + 2u}; _temp338( _temp340);}); _LL314:;} _LL270: if( Cyc_Absynpp_next_is_pointer(((
struct Cyc_List_List*) _check_null( tms))->tl)){ rest= p_rest;} return({ struct
Cyc_PP_Doc*(* _temp349)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp352= rest; struct Cyc_PP_Doc* _temp353= Cyc_Absynpp_atts2doc( _temp279);
struct Cyc_PP_Doc* _temp350[ 2u]={ _temp352, _temp353}; struct _tagged_ptr1
_temp351={ _temp350, _temp350, _temp350 + 2u}; _temp349( _temp351);}); _LL272:
if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*) _check_null( tms))->tl)){
rest= p_rest;} if( _temp283){ return({ struct Cyc_PP_Doc*(* _temp354)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp357= rest; struct Cyc_PP_Doc*
_temp358= Cyc_Absynpp_ktvars2doc( _temp287); struct Cyc_PP_Doc* _temp355[ 2u]={
_temp357, _temp358}; struct _tagged_ptr1 _temp356={ _temp355, _temp355, _temp355
+ 2u}; _temp354( _temp356);});} else{ return({ struct Cyc_PP_Doc*(* _temp359)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp362= rest; struct Cyc_PP_Doc*
_temp363= Cyc_Absynpp_tvars2doc( _temp287); struct Cyc_PP_Doc* _temp360[ 2u]={
_temp362, _temp363}; struct _tagged_ptr1 _temp361={ _temp360, _temp360, _temp360
+ 2u}; _temp359( _temp361);});} _LL274: { struct Cyc_PP_Doc* ptr;{ void*
_temp364= _temp293; struct Cyc_Absyn_Exp* _temp372; struct Cyc_Absyn_Exp*
_temp374; _LL366: if(( unsigned int) _temp364 > 1u?*(( int*) _temp364) == Cyc_Absyn_Nullable_ps:
0){ _LL373: _temp372=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp364)->f1;
goto _LL367;} else{ goto _LL368;} _LL368: if(( unsigned int) _temp364 > 1u?*((
int*) _temp364) == Cyc_Absyn_NonNullable_ps: 0){ _LL375: _temp374=(( struct Cyc_Absyn_NonNullable_ps_struct*)
_temp364)->f1; goto _LL369;} else{ goto _LL370;} _LL370: if( _temp364 ==( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL371;} else{ goto _LL365;} _LL367: if( Cyc_Evexp_eval_const_uint_exp(
_temp372) == 1){ ptr= Cyc_PP_text(({ unsigned char* _temp376=( unsigned char*)"*";
struct _tagged_string _temp377; _temp377.curr= _temp376; _temp377.base= _temp376;
_temp377.last_plus_one= _temp376 + 2; _temp377;}));} else{ ptr=({ struct Cyc_PP_Doc*(*
_temp378)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp381= Cyc_PP_text(({
unsigned char* _temp389=( unsigned char*)"*"; struct _tagged_string _temp390;
_temp390.curr= _temp389; _temp390.base= _temp389; _temp390.last_plus_one=
_temp389 + 2; _temp390;})); struct Cyc_PP_Doc* _temp382= Cyc_PP_text(({
unsigned char* _temp387=( unsigned char*)"{"; struct _tagged_string _temp388;
_temp388.curr= _temp387; _temp388.base= _temp387; _temp388.last_plus_one=
_temp387 + 2; _temp388;})); struct Cyc_PP_Doc* _temp383= Cyc_Absynpp_exp2doc(
_temp372); struct Cyc_PP_Doc* _temp384= Cyc_PP_text(({ unsigned char* _temp385=(
unsigned char*)"}"; struct _tagged_string _temp386; _temp386.curr= _temp385;
_temp386.base= _temp385; _temp386.last_plus_one= _temp385 + 2; _temp386;}));
struct Cyc_PP_Doc* _temp379[ 4u]={ _temp381, _temp382, _temp383, _temp384};
struct _tagged_ptr1 _temp380={ _temp379, _temp379, _temp379 + 4u}; _temp378(
_temp380);});} goto _LL365; _LL369: if( Cyc_Evexp_eval_const_uint_exp( _temp374)
== 1){ ptr= Cyc_PP_text(({ unsigned char* _temp391=( unsigned char*)"@"; struct
_tagged_string _temp392; _temp392.curr= _temp391; _temp392.base= _temp391;
_temp392.last_plus_one= _temp391 + 2; _temp392;}));} else{ ptr=({ struct Cyc_PP_Doc*(*
_temp393)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp396= Cyc_PP_text(({
unsigned char* _temp404=( unsigned char*)"@"; struct _tagged_string _temp405;
_temp405.curr= _temp404; _temp405.base= _temp404; _temp405.last_plus_one=
_temp404 + 2; _temp405;})); struct Cyc_PP_Doc* _temp397= Cyc_PP_text(({
unsigned char* _temp402=( unsigned char*)"{"; struct _tagged_string _temp403;
_temp403.curr= _temp402; _temp403.base= _temp402; _temp403.last_plus_one=
_temp402 + 2; _temp403;})); struct Cyc_PP_Doc* _temp398= Cyc_Absynpp_exp2doc(
_temp374); struct Cyc_PP_Doc* _temp399= Cyc_PP_text(({ unsigned char* _temp400=(
unsigned char*)"}"; struct _tagged_string _temp401; _temp401.curr= _temp400;
_temp401.base= _temp400; _temp401.last_plus_one= _temp400 + 2; _temp401;}));
struct Cyc_PP_Doc* _temp394[ 4u]={ _temp396, _temp397, _temp398, _temp399};
struct _tagged_ptr1 _temp395={ _temp394, _temp394, _temp394 + 4u}; _temp393(
_temp395);});} goto _LL365; _LL371: ptr= Cyc_PP_text(({ unsigned char* _temp406=(
unsigned char*)"?"; struct _tagged_string _temp407; _temp407.curr= _temp406;
_temp407.base= _temp406; _temp407.last_plus_one= _temp406 + 2; _temp407;}));
goto _LL365; _LL365:;}{ void* _temp408= Cyc_Tcutil_compress( _temp291); _LL410:
if( _temp408 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL411;} else{ goto _LL412;}
_LL412: goto _LL413; _LL411: return({ struct Cyc_PP_Doc*(* _temp414)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp417= ptr; struct Cyc_PP_Doc*
_temp418= rest; struct Cyc_PP_Doc* _temp415[ 2u]={ _temp417, _temp418}; struct
_tagged_ptr1 _temp416={ _temp415, _temp415, _temp415 + 2u}; _temp414( _temp416);});
_LL413: return({ struct Cyc_PP_Doc*(* _temp419)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp422= ptr; struct Cyc_PP_Doc* _temp423= Cyc_Absynpp_ntyp2doc(
_temp291); struct Cyc_PP_Doc* _temp424= Cyc_PP_text(({ unsigned char* _temp426=(
unsigned char*)" "; struct _tagged_string _temp427; _temp427.curr= _temp426;
_temp427.base= _temp426; _temp427.last_plus_one= _temp426 + 2; _temp427;}));
struct Cyc_PP_Doc* _temp425= rest; struct Cyc_PP_Doc* _temp420[ 4u]={ _temp422,
_temp423, _temp424, _temp425}; struct _tagged_ptr1 _temp421={ _temp420, _temp420,
_temp420 + 4u}; _temp419( _temp421);}); _LL409:;}} _LL262:;}} struct Cyc_PP_Doc*
Cyc_Absynpp_rgn2doc( void* t){ void* _temp438= Cyc_Tcutil_compress( t); _LL440:
if( _temp438 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL441;} else{ goto _LL442;}
_LL442: goto _LL443; _LL441: return Cyc_PP_text(({ unsigned char* _temp444=(
unsigned char*)"`H"; struct _tagged_string _temp445; _temp445.curr= _temp444;
_temp445.base= _temp444; _temp445.last_plus_one= _temp444 + 3; _temp445;}));
_LL443: return Cyc_Absynpp_ntyp2doc( t); _LL439:;} static void Cyc_Absynpp_effects2docs(
struct Cyc_List_List** regions, struct Cyc_List_List** effects, void* t){ void*
_temp446= Cyc_Tcutil_compress( t); void* _temp454; struct Cyc_List_List*
_temp456; _LL448: if(( unsigned int) _temp446 > 4u?*(( int*) _temp446) == Cyc_Absyn_AccessEff:
0){ _LL455: _temp454=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp446)->f1;
goto _LL449;} else{ goto _LL450;} _LL450: if(( unsigned int) _temp446 > 4u?*((
int*) _temp446) == Cyc_Absyn_JoinEff: 0){ _LL457: _temp456=(( struct Cyc_Absyn_JoinEff_struct*)
_temp446)->f1; goto _LL451;} else{ goto _LL452;} _LL452: goto _LL453; _LL449:*
regions=({ struct Cyc_List_List* _temp458=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp458->hd=( void*) Cyc_Absynpp_rgn2doc(
_temp454); _temp458->tl=* regions; _temp458;}); goto _LL447; _LL451: for( 0;
_temp456 != 0; _temp456=(( struct Cyc_List_List*) _check_null( _temp456))->tl){
Cyc_Absynpp_effects2docs( regions, effects,( void*)(( struct Cyc_List_List*)
_check_null( _temp456))->hd);} goto _LL447; _LL453:* effects=({ struct Cyc_List_List*
_temp459=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp459->hd=( void*) Cyc_Absynpp_ntyp2doc( t); _temp459->tl=* effects; _temp459;});
goto _LL447; _LL447:;} struct Cyc_PP_Doc* Cyc_Absynpp_eff2doc( void* t){ struct
Cyc_List_List* regions= 0; struct Cyc_List_List* effects= 0; Cyc_Absynpp_effects2docs(&
regions,& effects, t); regions=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( regions); effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( effects); if( regions == 0? effects != 0: 0){ return Cyc_PP_group(({
unsigned char* _temp460=( unsigned char*)""; struct _tagged_string _temp461;
_temp461.curr= _temp460; _temp461.base= _temp460; _temp461.last_plus_one=
_temp460 + 1; _temp461;}),({ unsigned char* _temp462=( unsigned char*)""; struct
_tagged_string _temp463; _temp463.curr= _temp462; _temp463.base= _temp462;
_temp463.last_plus_one= _temp462 + 1; _temp463;}),({ unsigned char* _temp464=(
unsigned char*)"+"; struct _tagged_string _temp465; _temp465.curr= _temp464;
_temp465.base= _temp464; _temp465.last_plus_one= _temp464 + 2; _temp465;}),
effects);} else{ struct Cyc_PP_Doc* _temp472= Cyc_PP_group(({ unsigned char*
_temp466=( unsigned char*)"{"; struct _tagged_string _temp467; _temp467.curr=
_temp466; _temp467.base= _temp466; _temp467.last_plus_one= _temp466 + 2;
_temp467;}),({ unsigned char* _temp468=( unsigned char*)"}"; struct
_tagged_string _temp469; _temp469.curr= _temp468; _temp469.base= _temp468;
_temp469.last_plus_one= _temp468 + 2; _temp469;}),({ unsigned char* _temp470=(
unsigned char*)","; struct _tagged_string _temp471; _temp471.curr= _temp470;
_temp471.base= _temp470; _temp471.last_plus_one= _temp470 + 2; _temp471;}),
regions); goto _LL473; _LL473: return Cyc_PP_group(({ unsigned char* _temp474=(
unsigned char*)""; struct _tagged_string _temp475; _temp475.curr= _temp474;
_temp475.base= _temp474; _temp475.last_plus_one= _temp474 + 1; _temp475;}),({
unsigned char* _temp476=( unsigned char*)""; struct _tagged_string _temp477;
_temp477.curr= _temp476; _temp477.base= _temp476; _temp477.last_plus_one=
_temp476 + 1; _temp477;}),({ unsigned char* _temp478=( unsigned char*)"+";
struct _tagged_string _temp479; _temp479.curr= _temp478; _temp479.base= _temp478;
_temp479.last_plus_one= _temp478 + 2; _temp479;}),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( effects,({
struct Cyc_List_List* _temp480=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp480->hd=( void*) _temp472; _temp480->tl= 0;
_temp480;})));}} struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc( void* t){ struct Cyc_PP_Doc*
s;{ void* _temp481= t; struct Cyc_Absyn_Exp* _temp527; struct Cyc_Absyn_Tqual
_temp529; void* _temp531; struct Cyc_Absyn_FnInfo _temp533; struct Cyc_Absyn_PtrInfo
_temp535; struct Cyc_Core_Opt* _temp537; int _temp539; struct Cyc_Core_Opt*
_temp541; struct Cyc_Core_Opt* _temp543; struct Cyc_Absyn_Tvar* _temp545; struct
Cyc_Absyn_TunionInfo _temp547; void* _temp549; struct Cyc_List_List* _temp551;
void* _temp553; struct Cyc_Absyn_TunionFieldInfo _temp555; struct Cyc_List_List*
_temp557; void* _temp559; void* _temp561; void* _temp563; struct Cyc_List_List*
_temp565; struct Cyc_Absyn_Structdecl** _temp567; struct Cyc_List_List* _temp569;
struct _tuple0* _temp571; struct Cyc_Absyn_Uniondecl** _temp573; struct Cyc_List_List*
_temp575; struct _tuple0* _temp577; struct Cyc_List_List* _temp579; struct Cyc_List_List*
_temp581; struct Cyc_Absyn_Enumdecl* _temp583; struct _tuple0* _temp585; struct
Cyc_Core_Opt* _temp587; struct Cyc_List_List* _temp589; struct _tuple0* _temp591;
void* _temp593; void* _temp595; struct Cyc_List_List* _temp597; _LL483: if((
unsigned int) _temp481 > 4u?*(( int*) _temp481) == Cyc_Absyn_ArrayType: 0){
_LL532: _temp531=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp481)->f1;
goto _LL530; _LL530: _temp529=(( struct Cyc_Absyn_ArrayType_struct*) _temp481)->f2;
goto _LL528; _LL528: _temp527=(( struct Cyc_Absyn_ArrayType_struct*) _temp481)->f3;
goto _LL484;} else{ goto _LL485;} _LL485: if(( unsigned int) _temp481 > 4u?*((
int*) _temp481) == Cyc_Absyn_FnType: 0){ _LL534: _temp533=(( struct Cyc_Absyn_FnType_struct*)
_temp481)->f1; goto _LL486;} else{ goto _LL487;} _LL487: if(( unsigned int)
_temp481 > 4u?*(( int*) _temp481) == Cyc_Absyn_PointerType: 0){ _LL536: _temp535=((
struct Cyc_Absyn_PointerType_struct*) _temp481)->f1; goto _LL488;} else{ goto
_LL489;} _LL489: if( _temp481 ==( void*) Cyc_Absyn_VoidType){ goto _LL490;}
else{ goto _LL491;} _LL491: if(( unsigned int) _temp481 > 4u?*(( int*) _temp481)
== Cyc_Absyn_Evar: 0){ _LL544: _temp543=(( struct Cyc_Absyn_Evar_struct*)
_temp481)->f1; goto _LL542; _LL542: _temp541=(( struct Cyc_Absyn_Evar_struct*)
_temp481)->f2; goto _LL540; _LL540: _temp539=(( struct Cyc_Absyn_Evar_struct*)
_temp481)->f3; goto _LL538; _LL538: _temp537=(( struct Cyc_Absyn_Evar_struct*)
_temp481)->f4; goto _LL492;} else{ goto _LL493;} _LL493: if(( unsigned int)
_temp481 > 4u?*(( int*) _temp481) == Cyc_Absyn_VarType: 0){ _LL546: _temp545=((
struct Cyc_Absyn_VarType_struct*) _temp481)->f1; goto _LL494;} else{ goto _LL495;}
_LL495: if(( unsigned int) _temp481 > 4u?*(( int*) _temp481) == Cyc_Absyn_TunionType:
0){ _LL548: _temp547=(( struct Cyc_Absyn_TunionType_struct*) _temp481)->f1;
_LL554: _temp553=( void*) _temp547.tunion_info; goto _LL552; _LL552: _temp551=
_temp547.targs; goto _LL550; _LL550: _temp549=( void*) _temp547.rgn; goto _LL496;}
else{ goto _LL497;} _LL497: if(( unsigned int) _temp481 > 4u?*(( int*) _temp481)
== Cyc_Absyn_TunionFieldType: 0){ _LL556: _temp555=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp481)->f1; _LL560: _temp559=( void*) _temp555.field_info; goto _LL558;
_LL558: _temp557= _temp555.targs; goto _LL498;} else{ goto _LL499;} _LL499: if((
unsigned int) _temp481 > 4u?*(( int*) _temp481) == Cyc_Absyn_IntType: 0){ _LL564:
_temp563=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp481)->f1; goto _LL562;
_LL562: _temp561=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp481)->f2;
goto _LL500;} else{ goto _LL501;} _LL501: if( _temp481 ==( void*) Cyc_Absyn_FloatType){
goto _LL502;} else{ goto _LL503;} _LL503: if( _temp481 ==( void*) Cyc_Absyn_DoubleType){
goto _LL504;} else{ goto _LL505;} _LL505: if(( unsigned int) _temp481 > 4u?*((
int*) _temp481) == Cyc_Absyn_TupleType: 0){ _LL566: _temp565=(( struct Cyc_Absyn_TupleType_struct*)
_temp481)->f1; goto _LL506;} else{ goto _LL507;} _LL507: if(( unsigned int)
_temp481 > 4u?*(( int*) _temp481) == Cyc_Absyn_StructType: 0){ _LL572: _temp571=((
struct Cyc_Absyn_StructType_struct*) _temp481)->f1; goto _LL570; _LL570:
_temp569=(( struct Cyc_Absyn_StructType_struct*) _temp481)->f2; goto _LL568;
_LL568: _temp567=(( struct Cyc_Absyn_StructType_struct*) _temp481)->f3; goto
_LL508;} else{ goto _LL509;} _LL509: if(( unsigned int) _temp481 > 4u?*(( int*)
_temp481) == Cyc_Absyn_UnionType: 0){ _LL578: _temp577=(( struct Cyc_Absyn_UnionType_struct*)
_temp481)->f1; goto _LL576; _LL576: _temp575=(( struct Cyc_Absyn_UnionType_struct*)
_temp481)->f2; goto _LL574; _LL574: _temp573=(( struct Cyc_Absyn_UnionType_struct*)
_temp481)->f3; goto _LL510;} else{ goto _LL511;} _LL511: if(( unsigned int)
_temp481 > 4u?*(( int*) _temp481) == Cyc_Absyn_AnonStructType: 0){ _LL580:
_temp579=(( struct Cyc_Absyn_AnonStructType_struct*) _temp481)->f1; goto _LL512;}
else{ goto _LL513;} _LL513: if(( unsigned int) _temp481 > 4u?*(( int*) _temp481)
== Cyc_Absyn_AnonUnionType: 0){ _LL582: _temp581=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp481)->f1; goto _LL514;} else{ goto _LL515;} _LL515: if(( unsigned int)
_temp481 > 4u?*(( int*) _temp481) == Cyc_Absyn_EnumType: 0){ _LL586: _temp585=((
struct Cyc_Absyn_EnumType_struct*) _temp481)->f1; goto _LL584; _LL584: _temp583=((
struct Cyc_Absyn_EnumType_struct*) _temp481)->f2; goto _LL516;} else{ goto
_LL517;} _LL517: if(( unsigned int) _temp481 > 4u?*(( int*) _temp481) == Cyc_Absyn_TypedefType:
0){ _LL592: _temp591=(( struct Cyc_Absyn_TypedefType_struct*) _temp481)->f1;
goto _LL590; _LL590: _temp589=(( struct Cyc_Absyn_TypedefType_struct*) _temp481)->f2;
goto _LL588; _LL588: _temp587=(( struct Cyc_Absyn_TypedefType_struct*) _temp481)->f3;
goto _LL518;} else{ goto _LL519;} _LL519: if(( unsigned int) _temp481 > 4u?*((
int*) _temp481) == Cyc_Absyn_RgnHandleType: 0){ _LL594: _temp593=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp481)->f1; goto _LL520;} else{ goto
_LL521;} _LL521: if( _temp481 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL522;} else{
goto _LL523;} _LL523: if(( unsigned int) _temp481 > 4u?*(( int*) _temp481) ==
Cyc_Absyn_AccessEff: 0){ _LL596: _temp595=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp481)->f1; goto _LL524;} else{ goto _LL525;} _LL525: if(( unsigned int)
_temp481 > 4u?*(( int*) _temp481) == Cyc_Absyn_JoinEff: 0){ _LL598: _temp597=((
struct Cyc_Absyn_JoinEff_struct*) _temp481)->f1; goto _LL526;} else{ goto _LL482;}
_LL484: return Cyc_PP_text(({ unsigned char* _temp599=( unsigned char*)"[[[array]]]";
struct _tagged_string _temp600; _temp600.curr= _temp599; _temp600.base= _temp599;
_temp600.last_plus_one= _temp599 + 12; _temp600;})); _LL486: return Cyc_PP_nil_doc();
_LL488: return Cyc_PP_nil_doc(); _LL490: s= Cyc_PP_text(({ unsigned char*
_temp601=( unsigned char*)"void"; struct _tagged_string _temp602; _temp602.curr=
_temp601; _temp602.base= _temp601; _temp602.last_plus_one= _temp601 + 5;
_temp602;})); goto _LL482; _LL492: if( _temp541 != 0){ return Cyc_Absynpp_ntyp2doc((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp541))->v);} else{ s=({ struct
Cyc_PP_Doc*(* _temp603)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp606= Cyc_PP_text(({ unsigned char* _temp620=( unsigned char*)"%"; struct
_tagged_string _temp621; _temp621.curr= _temp620; _temp621.base= _temp620;
_temp621.last_plus_one= _temp620 + 2; _temp621;})); struct Cyc_PP_Doc* _temp607=
Cyc_PP_text(({ unsigned char* _temp618=( unsigned char*)"("; struct
_tagged_string _temp619; _temp619.curr= _temp618; _temp619.base= _temp618;
_temp619.last_plus_one= _temp618 + 2; _temp619;})); struct Cyc_PP_Doc* _temp608=
Cyc_PP_text(( struct _tagged_string) xprintf("%d", _temp539)); struct Cyc_PP_Doc*
_temp609=( ! Cyc_Absynpp_print_full_evars? 1: _temp537 == 0)? Cyc_PP_text(({
unsigned char* _temp616=( unsigned char*)""; struct _tagged_string _temp617;
_temp617.curr= _temp616; _temp617.base= _temp616; _temp617.last_plus_one=
_temp616 + 1; _temp617;})): Cyc_Absynpp_tvars2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp537))->v); struct Cyc_PP_Doc* _temp610=
Cyc_PP_text(({ unsigned char* _temp614=( unsigned char*)")::"; struct
_tagged_string _temp615; _temp615.curr= _temp614; _temp615.base= _temp614;
_temp615.last_plus_one= _temp614 + 4; _temp615;})); struct Cyc_PP_Doc* _temp611=
_temp543 == 0? Cyc_PP_text(({ unsigned char* _temp612=( unsigned char*)"?";
struct _tagged_string _temp613; _temp613.curr= _temp612; _temp613.base= _temp612;
_temp613.last_plus_one= _temp612 + 2; _temp613;})): Cyc_Absynpp_kind2doc(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp543))->v); struct Cyc_PP_Doc* _temp604[
6u]={ _temp606, _temp607, _temp608, _temp609, _temp610, _temp611}; struct
_tagged_ptr1 _temp605={ _temp604, _temp604, _temp604 + 6u}; _temp603( _temp605);});}
goto _LL482; _LL494: s= Cyc_PP_textptr( _temp545->name); if( Cyc_Absynpp_print_all_kinds){
s=({ struct Cyc_PP_Doc*(* _temp622)( struct _tagged_ptr1)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp625= s; struct Cyc_PP_Doc* _temp626= Cyc_PP_text(({
unsigned char* _temp628=( unsigned char*)"::"; struct _tagged_string _temp629;
_temp629.curr= _temp628; _temp629.base= _temp628; _temp629.last_plus_one=
_temp628 + 3; _temp629;})); struct Cyc_PP_Doc* _temp627= Cyc_Absynpp_ckind2doc(
_temp545->kind); struct Cyc_PP_Doc* _temp623[ 3u]={ _temp625, _temp626, _temp627};
struct _tagged_ptr1 _temp624={ _temp623, _temp623, _temp623 + 3u}; _temp622(
_temp624);});} if( ! Cyc_Absynpp_dont_rewrite_temp_tvars? Cyc_Tcutil_is_temp_tvar(
_temp545): 0){ s=({ struct Cyc_PP_Doc*(* _temp630)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp633= Cyc_PP_text(({ unsigned char* _temp638=(
unsigned char*)"_ /* "; struct _tagged_string _temp639; _temp639.curr= _temp638;
_temp639.base= _temp638; _temp639.last_plus_one= _temp638 + 6; _temp639;}));
struct Cyc_PP_Doc* _temp634= s; struct Cyc_PP_Doc* _temp635= Cyc_PP_text(({
unsigned char* _temp636=( unsigned char*)" */"; struct _tagged_string _temp637;
_temp637.curr= _temp636; _temp637.base= _temp636; _temp637.last_plus_one=
_temp636 + 4; _temp637;})); struct Cyc_PP_Doc* _temp631[ 3u]={ _temp633,
_temp634, _temp635}; struct _tagged_ptr1 _temp632={ _temp631, _temp631, _temp631
+ 3u}; _temp630( _temp632);});} goto _LL482; _LL496:{ void* _temp640= _temp553;
struct Cyc_Absyn_UnknownTunionInfo _temp646; int _temp648; struct _tuple0*
_temp650; struct Cyc_Absyn_Tuniondecl* _temp652; struct Cyc_Absyn_Tuniondecl
_temp654; int _temp655; struct Cyc_Core_Opt* _temp657; struct Cyc_List_List*
_temp659; struct _tuple0* _temp661; void* _temp663; _LL642: if(*(( int*)
_temp640) == Cyc_Absyn_UnknownTunion){ _LL647: _temp646=(( struct Cyc_Absyn_UnknownTunion_struct*)
_temp640)->f1; _LL651: _temp650= _temp646.name; goto _LL649; _LL649: _temp648=
_temp646.is_xtunion; goto _LL643;} else{ goto _LL644;} _LL644: if(*(( int*)
_temp640) == Cyc_Absyn_KnownTunion){ _LL653: _temp652=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp640)->f1; _temp654=* _temp652; _LL664: _temp663=( void*) _temp654.sc; goto
_LL662; _LL662: _temp661= _temp654.name; goto _LL660; _LL660: _temp659= _temp654.tvs;
goto _LL658; _LL658: _temp657= _temp654.fields; goto _LL656; _LL656: _temp655=
_temp654.is_xtunion; goto _LL645;} else{ goto _LL641;} _LL643: _temp661=
_temp650; _temp655= _temp648; goto _LL645; _LL645: { struct Cyc_PP_Doc* _temp669=
Cyc_PP_text( _temp655?({ unsigned char* _temp665=( unsigned char*)"xtunion ";
struct _tagged_string _temp666; _temp666.curr= _temp665; _temp666.base= _temp665;
_temp666.last_plus_one= _temp665 + 9; _temp666;}):({ unsigned char* _temp667=(
unsigned char*)"tunion "; struct _tagged_string _temp668; _temp668.curr=
_temp667; _temp668.base= _temp667; _temp668.last_plus_one= _temp667 + 8;
_temp668;})); goto _LL670; _LL670:{ void* _temp671= Cyc_Tcutil_compress(
_temp549); _LL673: if( _temp671 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL674;}
else{ goto _LL675;} _LL675: goto _LL676; _LL674: s=({ struct Cyc_PP_Doc*(*
_temp677)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp680=
_temp669; struct Cyc_PP_Doc* _temp681= Cyc_Absynpp_qvar2doc( _temp661); struct
Cyc_PP_Doc* _temp682= Cyc_Absynpp_tps2doc( _temp551); struct Cyc_PP_Doc*
_temp678[ 3u]={ _temp680, _temp681, _temp682}; struct _tagged_ptr1 _temp679={
_temp678, _temp678, _temp678 + 3u}; _temp677( _temp679);}); goto _LL672; _LL676:
s=({ struct Cyc_PP_Doc*(* _temp683)( struct _tagged_ptr1)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp686= _temp669; struct Cyc_PP_Doc* _temp687= Cyc_Absynpp_typ2doc(
_temp549); struct Cyc_PP_Doc* _temp688= Cyc_PP_text(({ unsigned char* _temp691=(
unsigned char*)" "; struct _tagged_string _temp692; _temp692.curr= _temp691;
_temp692.base= _temp691; _temp692.last_plus_one= _temp691 + 2; _temp692;}));
struct Cyc_PP_Doc* _temp689= Cyc_Absynpp_qvar2doc( _temp661); struct Cyc_PP_Doc*
_temp690= Cyc_Absynpp_tps2doc( _temp551); struct Cyc_PP_Doc* _temp684[ 5u]={
_temp686, _temp687, _temp688, _temp689, _temp690}; struct _tagged_ptr1 _temp685={
_temp684, _temp684, _temp684 + 5u}; _temp683( _temp685);}); goto _LL672; _LL672:;}
goto _LL641;} _LL641:;} goto _LL482; _LL498:{ void* _temp693= _temp559; struct
Cyc_Absyn_UnknownTunionFieldInfo _temp699; int _temp701; struct _tuple0*
_temp703; struct _tuple0* _temp705; struct Cyc_Absyn_Tunionfield* _temp707;
struct Cyc_Absyn_Tunionfield _temp709; void* _temp710; struct Cyc_Position_Segment*
_temp712; struct Cyc_List_List* _temp714; struct Cyc_List_List* _temp716; struct
_tuple0* _temp718; struct Cyc_Absyn_Tuniondecl* _temp720; struct Cyc_Absyn_Tuniondecl
_temp722; int _temp723; struct Cyc_Core_Opt* _temp725; struct Cyc_List_List*
_temp727; struct _tuple0* _temp729; void* _temp731; _LL695: if(*(( int*)
_temp693) == Cyc_Absyn_UnknownTunionfield){ _LL700: _temp699=(( struct Cyc_Absyn_UnknownTunionfield_struct*)
_temp693)->f1; _LL706: _temp705= _temp699.tunion_name; goto _LL704; _LL704:
_temp703= _temp699.field_name; goto _LL702; _LL702: _temp701= _temp699.is_xtunion;
goto _LL696;} else{ goto _LL697;} _LL697: if(*(( int*) _temp693) == Cyc_Absyn_KnownTunionfield){
_LL721: _temp720=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp693)->f1;
_temp722=* _temp720; _LL732: _temp731=( void*) _temp722.sc; goto _LL730; _LL730:
_temp729= _temp722.name; goto _LL728; _LL728: _temp727= _temp722.tvs; goto
_LL726; _LL726: _temp725= _temp722.fields; goto _LL724; _LL724: _temp723=
_temp722.is_xtunion; goto _LL708; _LL708: _temp707=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp693)->f2; _temp709=* _temp707; _LL719: _temp718= _temp709.name; goto _LL717;
_LL717: _temp716= _temp709.tvs; goto _LL715; _LL715: _temp714= _temp709.typs;
goto _LL713; _LL713: _temp712= _temp709.loc; goto _LL711; _LL711: _temp710=(
void*) _temp709.sc; goto _LL698;} else{ goto _LL694;} _LL696: _temp729= _temp705;
_temp723= _temp701; _temp718= _temp703; goto _LL698; _LL698: { struct Cyc_PP_Doc*
_temp737= Cyc_PP_text( _temp723?({ unsigned char* _temp733=( unsigned char*)"xtunion ";
struct _tagged_string _temp734; _temp734.curr= _temp733; _temp734.base= _temp733;
_temp734.last_plus_one= _temp733 + 9; _temp734;}):({ unsigned char* _temp735=(
unsigned char*)"tunion "; struct _tagged_string _temp736; _temp736.curr=
_temp735; _temp736.base= _temp735; _temp736.last_plus_one= _temp735 + 8;
_temp736;})); goto _LL738; _LL738: s=({ struct Cyc_PP_Doc*(* _temp739)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp742= _temp737; struct Cyc_PP_Doc*
_temp743= Cyc_Absynpp_qvar2doc( _temp729); struct Cyc_PP_Doc* _temp744= Cyc_PP_text(({
unsigned char* _temp746=( unsigned char*)"."; struct _tagged_string _temp747;
_temp747.curr= _temp746; _temp747.base= _temp746; _temp747.last_plus_one=
_temp746 + 2; _temp747;})); struct Cyc_PP_Doc* _temp745= Cyc_PP_textptr((*
_temp718).f2); struct Cyc_PP_Doc* _temp740[ 4u]={ _temp742, _temp743, _temp744,
_temp745}; struct _tagged_ptr1 _temp741={ _temp740, _temp740, _temp740 + 4u};
_temp739( _temp741);}); goto _LL694;} _LL694:;} goto _LL482; _LL500: { struct
_tagged_string sns; struct _tagged_string ts;{ void* _temp748= _temp563; _LL750:
if( _temp748 ==( void*) Cyc_Absyn_Signed){ goto _LL751;} else{ goto _LL752;}
_LL752: if( _temp748 ==( void*) Cyc_Absyn_Unsigned){ goto _LL753;} else{ goto
_LL749;} _LL751: sns=({ unsigned char* _temp754=( unsigned char*)""; struct
_tagged_string _temp755; _temp755.curr= _temp754; _temp755.base= _temp754;
_temp755.last_plus_one= _temp754 + 1; _temp755;}); goto _LL749; _LL753: sns=({
unsigned char* _temp756=( unsigned char*)"unsigned "; struct _tagged_string
_temp757; _temp757.curr= _temp756; _temp757.base= _temp756; _temp757.last_plus_one=
_temp756 + 10; _temp757;}); goto _LL749; _LL749:;}{ void* _temp758= _temp561;
_LL760: if( _temp758 ==( void*) Cyc_Absyn_B1){ goto _LL761;} else{ goto _LL762;}
_LL762: if( _temp758 ==( void*) Cyc_Absyn_B2){ goto _LL763;} else{ goto _LL764;}
_LL764: if( _temp758 ==( void*) Cyc_Absyn_B4){ goto _LL765;} else{ goto _LL766;}
_LL766: if( _temp758 ==( void*) Cyc_Absyn_B8){ goto _LL767;} else{ goto _LL759;}
_LL761:{ void* _temp768= _temp563; _LL770: if( _temp768 ==( void*) Cyc_Absyn_Signed){
goto _LL771;} else{ goto _LL772;} _LL772: if( _temp768 ==( void*) Cyc_Absyn_Unsigned){
goto _LL773;} else{ goto _LL769;} _LL771: sns=({ unsigned char* _temp774=(
unsigned char*)"signed "; struct _tagged_string _temp775; _temp775.curr=
_temp774; _temp775.base= _temp774; _temp775.last_plus_one= _temp774 + 8;
_temp775;}); goto _LL769; _LL773: goto _LL769; _LL769:;} ts=({ unsigned char*
_temp776=( unsigned char*)"char"; struct _tagged_string _temp777; _temp777.curr=
_temp776; _temp777.base= _temp776; _temp777.last_plus_one= _temp776 + 5;
_temp777;}); goto _LL759; _LL763: ts=({ unsigned char* _temp778=( unsigned char*)"short";
struct _tagged_string _temp779; _temp779.curr= _temp778; _temp779.base= _temp778;
_temp779.last_plus_one= _temp778 + 6; _temp779;}); goto _LL759; _LL765: ts=({
unsigned char* _temp780=( unsigned char*)"int"; struct _tagged_string _temp781;
_temp781.curr= _temp780; _temp781.base= _temp780; _temp781.last_plus_one=
_temp780 + 4; _temp781;}); goto _LL759; _LL767: ts= Cyc_Absynpp_to_VC?({
unsigned char* _temp782=( unsigned char*)"__int64"; struct _tagged_string
_temp783; _temp783.curr= _temp782; _temp783.base= _temp782; _temp783.last_plus_one=
_temp782 + 8; _temp783;}):({ unsigned char* _temp784=( unsigned char*)"long long";
struct _tagged_string _temp785; _temp785.curr= _temp784; _temp785.base= _temp784;
_temp785.last_plus_one= _temp784 + 10; _temp785;}); goto _LL759; _LL759:;} s=
Cyc_PP_text(( struct _tagged_string)({ struct _tagged_string _temp786= sns;
struct _tagged_string _temp787= ts; xprintf("%.*s%.*s", _temp786.last_plus_one -
_temp786.curr, _temp786.curr, _temp787.last_plus_one - _temp787.curr, _temp787.curr);}));
goto _LL482;} _LL502: s= Cyc_PP_text(({ unsigned char* _temp788=( unsigned char*)"float";
struct _tagged_string _temp789; _temp789.curr= _temp788; _temp789.base= _temp788;
_temp789.last_plus_one= _temp788 + 6; _temp789;})); goto _LL482; _LL504: s= Cyc_PP_text(({
unsigned char* _temp790=( unsigned char*)"double"; struct _tagged_string
_temp791; _temp791.curr= _temp790; _temp791.base= _temp790; _temp791.last_plus_one=
_temp790 + 7; _temp791;})); goto _LL482; _LL506: s=({ struct Cyc_PP_Doc*(*
_temp792)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp795= Cyc_PP_text(({
unsigned char* _temp797=( unsigned char*)"$"; struct _tagged_string _temp798;
_temp798.curr= _temp797; _temp798.base= _temp797; _temp798.last_plus_one=
_temp797 + 2; _temp798;})); struct Cyc_PP_Doc* _temp796= Cyc_Absynpp_args2doc(
_temp565); struct Cyc_PP_Doc* _temp793[ 2u]={ _temp795, _temp796}; struct
_tagged_ptr1 _temp794={ _temp793, _temp793, _temp793 + 2u}; _temp792( _temp794);});
goto _LL482; _LL508: if( _temp571 == 0){ s=({ struct Cyc_PP_Doc*(* _temp799)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp802= Cyc_PP_text(({
unsigned char* _temp804=( unsigned char*)"struct "; struct _tagged_string
_temp805; _temp805.curr= _temp804; _temp805.base= _temp804; _temp805.last_plus_one=
_temp804 + 8; _temp805;})); struct Cyc_PP_Doc* _temp803= Cyc_Absynpp_tps2doc(
_temp569); struct Cyc_PP_Doc* _temp800[ 2u]={ _temp802, _temp803}; struct
_tagged_ptr1 _temp801={ _temp800, _temp800, _temp800 + 2u}; _temp799( _temp801);});}
else{ s=({ struct Cyc_PP_Doc*(* _temp806)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp809= Cyc_PP_text(({ unsigned char* _temp812=(
unsigned char*)"struct "; struct _tagged_string _temp813; _temp813.curr=
_temp812; _temp813.base= _temp812; _temp813.last_plus_one= _temp812 + 8;
_temp813;})); struct Cyc_PP_Doc* _temp810= _temp571 == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc((
struct _tuple0*) _check_null( _temp571)); struct Cyc_PP_Doc* _temp811= Cyc_Absynpp_tps2doc(
_temp569); struct Cyc_PP_Doc* _temp807[ 3u]={ _temp809, _temp810, _temp811};
struct _tagged_ptr1 _temp808={ _temp807, _temp807, _temp807 + 3u}; _temp806(
_temp808);});} goto _LL482; _LL510: if( _temp577 == 0){ s=({ struct Cyc_PP_Doc*(*
_temp814)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp817= Cyc_PP_text(({
unsigned char* _temp819=( unsigned char*)"union "; struct _tagged_string
_temp820; _temp820.curr= _temp819; _temp820.base= _temp819; _temp820.last_plus_one=
_temp819 + 7; _temp820;})); struct Cyc_PP_Doc* _temp818= Cyc_Absynpp_tps2doc(
_temp575); struct Cyc_PP_Doc* _temp815[ 2u]={ _temp817, _temp818}; struct
_tagged_ptr1 _temp816={ _temp815, _temp815, _temp815 + 2u}; _temp814( _temp816);});}
else{ s=({ struct Cyc_PP_Doc*(* _temp821)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp824= Cyc_PP_text(({ unsigned char* _temp827=(
unsigned char*)"union "; struct _tagged_string _temp828; _temp828.curr= _temp827;
_temp828.base= _temp827; _temp828.last_plus_one= _temp827 + 7; _temp828;}));
struct Cyc_PP_Doc* _temp825= _temp577 == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc((
struct _tuple0*) _check_null( _temp577)); struct Cyc_PP_Doc* _temp826= Cyc_Absynpp_tps2doc(
_temp575); struct Cyc_PP_Doc* _temp822[ 3u]={ _temp824, _temp825, _temp826};
struct _tagged_ptr1 _temp823={ _temp822, _temp822, _temp822 + 3u}; _temp821(
_temp823);});} goto _LL482; _LL512: s=({ struct Cyc_PP_Doc*(* _temp829)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp832= Cyc_PP_text(({
unsigned char* _temp837=( unsigned char*)"struct {"; struct _tagged_string
_temp838; _temp838.curr= _temp837; _temp838.base= _temp837; _temp838.last_plus_one=
_temp837 + 9; _temp838;})); struct Cyc_PP_Doc* _temp833= Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc(
_temp579)); struct Cyc_PP_Doc* _temp834= Cyc_PP_text(({ unsigned char* _temp835=(
unsigned char*)"}"; struct _tagged_string _temp836; _temp836.curr= _temp835;
_temp836.base= _temp835; _temp836.last_plus_one= _temp835 + 2; _temp836;}));
struct Cyc_PP_Doc* _temp830[ 3u]={ _temp832, _temp833, _temp834}; struct
_tagged_ptr1 _temp831={ _temp830, _temp830, _temp830 + 3u}; _temp829( _temp831);});
goto _LL482; _LL514: s=({ struct Cyc_PP_Doc*(* _temp839)( struct _tagged_ptr1)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp842= Cyc_PP_text(({ unsigned char* _temp847=(
unsigned char*)"union {"; struct _tagged_string _temp848; _temp848.curr=
_temp847; _temp848.base= _temp847; _temp848.last_plus_one= _temp847 + 8;
_temp848;})); struct Cyc_PP_Doc* _temp843= Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc(
_temp581)); struct Cyc_PP_Doc* _temp844= Cyc_PP_text(({ unsigned char* _temp845=(
unsigned char*)"}"; struct _tagged_string _temp846; _temp846.curr= _temp845;
_temp846.base= _temp845; _temp846.last_plus_one= _temp845 + 2; _temp846;}));
struct Cyc_PP_Doc* _temp840[ 3u]={ _temp842, _temp843, _temp844}; struct
_tagged_ptr1 _temp841={ _temp840, _temp840, _temp840 + 3u}; _temp839( _temp841);});
goto _LL482; _LL516: s=({ struct Cyc_PP_Doc*(* _temp849)( struct _tagged_ptr1)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp852= Cyc_PP_text(({ unsigned char* _temp854=(
unsigned char*)"enum "; struct _tagged_string _temp855; _temp855.curr= _temp854;
_temp855.base= _temp854; _temp855.last_plus_one= _temp854 + 6; _temp855;}));
struct Cyc_PP_Doc* _temp853= Cyc_Absynpp_qvar2doc( _temp585); struct Cyc_PP_Doc*
_temp850[ 2u]={ _temp852, _temp853}; struct _tagged_ptr1 _temp851={ _temp850,
_temp850, _temp850 + 2u}; _temp849( _temp851);}); goto _LL482; _LL518: s=({
struct Cyc_PP_Doc*(* _temp856)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp859= Cyc_Absynpp_qvar2doc( _temp591); struct Cyc_PP_Doc* _temp860= Cyc_Absynpp_tps2doc(
_temp589); struct Cyc_PP_Doc* _temp857[ 2u]={ _temp859, _temp860}; struct
_tagged_ptr1 _temp858={ _temp857, _temp857, _temp857 + 2u}; _temp856( _temp858);});
goto _LL482; _LL520: s=({ struct Cyc_PP_Doc*(* _temp861)( struct _tagged_ptr1)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp864= Cyc_PP_text(({ unsigned char* _temp869=(
unsigned char*)"region_t<"; struct _tagged_string _temp870; _temp870.curr=
_temp869; _temp870.base= _temp869; _temp870.last_plus_one= _temp869 + 10;
_temp870;})); struct Cyc_PP_Doc* _temp865= Cyc_Absynpp_rgn2doc( _temp593);
struct Cyc_PP_Doc* _temp866= Cyc_PP_text(({ unsigned char* _temp867=(
unsigned char*)">"; struct _tagged_string _temp868; _temp868.curr= _temp867;
_temp868.base= _temp867; _temp868.last_plus_one= _temp867 + 2; _temp868;}));
struct Cyc_PP_Doc* _temp862[ 3u]={ _temp864, _temp865, _temp866}; struct
_tagged_ptr1 _temp863={ _temp862, _temp862, _temp862 + 3u}; _temp861( _temp863);});
goto _LL482; _LL522: s= Cyc_Absynpp_rgn2doc( t); goto _LL482; _LL524: s= Cyc_Absynpp_eff2doc(
t); goto _LL482; _LL526: s= Cyc_Absynpp_eff2doc( t); goto _LL482; _LL482:;}
return s;} struct Cyc_PP_Doc* Cyc_Absynpp_vo2doc( struct Cyc_Core_Opt* vo){
return vo == 0? Cyc_PP_nil_doc(): Cyc_PP_text(*(( struct _tagged_string*)((
struct Cyc_Core_Opt*) _check_null( vo))->v));} struct _tuple8{ void* f1; void*
f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_rgn_cmp2doc( struct _tuple8* cmp){ struct
_tuple8 _temp873; void* _temp874; void* _temp876; struct _tuple8* _temp871= cmp;
_temp873=* _temp871; _LL877: _temp876= _temp873.f1; goto _LL875; _LL875:
_temp874= _temp873.f2; goto _LL872; _LL872: return({ struct Cyc_PP_Doc*(*
_temp878)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp881= Cyc_Absynpp_rgn2doc(
_temp876); struct Cyc_PP_Doc* _temp882= Cyc_PP_text(({ unsigned char* _temp884=(
unsigned char*)" < "; struct _tagged_string _temp885; _temp885.curr= _temp884;
_temp885.base= _temp884; _temp885.last_plus_one= _temp884 + 4; _temp885;}));
struct Cyc_PP_Doc* _temp883= Cyc_Absynpp_rgn2doc( _temp874); struct Cyc_PP_Doc*
_temp879[ 3u]={ _temp881, _temp882, _temp883}; struct _tagged_ptr1 _temp880={
_temp879, _temp879, _temp879 + 3u}; _temp878( _temp880);});} struct Cyc_PP_Doc*
Cyc_Absynpp_rgnpo2doc( struct Cyc_List_List* po){ return Cyc_PP_group(({
unsigned char* _temp886=( unsigned char*)""; struct _tagged_string _temp887;
_temp887.curr= _temp886; _temp887.base= _temp886; _temp887.last_plus_one=
_temp886 + 1; _temp887;}),({ unsigned char* _temp888=( unsigned char*)""; struct
_tagged_string _temp889; _temp889.curr= _temp888; _temp889.base= _temp888;
_temp889.last_plus_one= _temp888 + 1; _temp889;}),({ unsigned char* _temp890=(
unsigned char*)","; struct _tagged_string _temp891; _temp891.curr= _temp890;
_temp891.base= _temp890; _temp891.last_plus_one= _temp890 + 2; _temp891;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple8*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_rgn_cmp2doc, po));} struct Cyc_PP_Doc* Cyc_Absynpp_funarg2doc(
struct _tuple1* t){ struct Cyc_Core_Opt* dopt=(* t).f1 == 0? 0:({ struct Cyc_Core_Opt*
_temp892=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp892->v=( void*) Cyc_PP_text(*(( struct _tagged_string*)(( struct Cyc_Core_Opt*)
_check_null((* t).f1))->v)); _temp892;}); return Cyc_Absynpp_tqtd2doc((* t).f2,(*
t).f3, dopt);} struct Cyc_PP_Doc* Cyc_Absynpp_funargs2doc( struct Cyc_List_List*
args, int c_varargs, struct Cyc_Absyn_VarargInfo* cyc_varargs, struct Cyc_Core_Opt*
effopt, struct Cyc_List_List* rgn_po){ struct Cyc_List_List* _temp893=(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc, args); goto _LL894; _LL894: { struct
Cyc_PP_Doc* eff_doc; if( c_varargs){ _temp893=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp893,({
struct Cyc_List_List* _temp895=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp895->hd=( void*) Cyc_PP_text(({ unsigned char*
_temp896=( unsigned char*)"..."; struct _tagged_string _temp897; _temp897.curr=
_temp896; _temp897.base= _temp896; _temp897.last_plus_one= _temp896 + 4;
_temp897;})); _temp895->tl= 0; _temp895;}));} else{ if( cyc_varargs != 0){
struct Cyc_PP_Doc* _temp912=({ struct Cyc_PP_Doc*(* _temp898)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp901= Cyc_PP_text(({
unsigned char* _temp910=( unsigned char*)"..."; struct _tagged_string _temp911;
_temp911.curr= _temp910; _temp911.base= _temp910; _temp911.last_plus_one=
_temp910 + 4; _temp911;})); struct Cyc_PP_Doc* _temp902= Cyc_Absynpp_typ2doc((
void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->rgn); struct
Cyc_PP_Doc* _temp903=(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->inject?
Cyc_PP_text(({ unsigned char* _temp906=( unsigned char*)" inject "; struct
_tagged_string _temp907; _temp907.curr= _temp906; _temp907.base= _temp906;
_temp907.last_plus_one= _temp906 + 9; _temp907;})): Cyc_PP_text(({ unsigned char*
_temp908=( unsigned char*)""; struct _tagged_string _temp909; _temp909.curr=
_temp908; _temp909.base= _temp908; _temp909.last_plus_one= _temp908 + 1;
_temp909;})); struct Cyc_PP_Doc* _temp904= Cyc_Absynpp_funarg2doc(({ struct
_tuple1* _temp905=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp905->f1=(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->name;
_temp905->f2=(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->tq;
_temp905->f3=( void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->type;
_temp905;})); struct Cyc_PP_Doc* _temp899[ 4u]={ _temp901, _temp902, _temp903,
_temp904}; struct _tagged_ptr1 _temp900={ _temp899, _temp899, _temp899 + 4u};
_temp898( _temp900);}); goto _LL913; _LL913: _temp893=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp893,({
struct Cyc_List_List* _temp914=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp914->hd=( void*) _temp912; _temp914->tl= 0;
_temp914;}));}}{ struct Cyc_PP_Doc* _temp921= Cyc_PP_group(({ unsigned char*
_temp915=( unsigned char*)""; struct _tagged_string _temp916; _temp916.curr=
_temp915; _temp916.base= _temp915; _temp916.last_plus_one= _temp915 + 1;
_temp916;}),({ unsigned char* _temp917=( unsigned char*)""; struct
_tagged_string _temp918; _temp918.curr= _temp917; _temp918.base= _temp917;
_temp918.last_plus_one= _temp917 + 1; _temp918;}),({ unsigned char* _temp919=(
unsigned char*)","; struct _tagged_string _temp920; _temp920.curr= _temp919;
_temp920.base= _temp919; _temp920.last_plus_one= _temp919 + 2; _temp920;}),
_temp893); goto _LL922; _LL922: if( effopt != 0){ _temp921=({ struct Cyc_PP_Doc*(*
_temp923)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp926=
_temp921; struct Cyc_PP_Doc* _temp927= Cyc_PP_text(({ unsigned char* _temp929=(
unsigned char*)";"; struct _tagged_string _temp930; _temp930.curr= _temp929;
_temp930.base= _temp929; _temp930.last_plus_one= _temp929 + 2; _temp930;}));
struct Cyc_PP_Doc* _temp928= Cyc_Absynpp_eff2doc(( void*)(( struct Cyc_Core_Opt*)
_check_null( effopt))->v); struct Cyc_PP_Doc* _temp924[ 3u]={ _temp926, _temp927,
_temp928}; struct _tagged_ptr1 _temp925={ _temp924, _temp924, _temp924 + 3u};
_temp923( _temp925);});} if( rgn_po != 0){ _temp921=({ struct Cyc_PP_Doc*(*
_temp931)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp934=
_temp921; struct Cyc_PP_Doc* _temp935= Cyc_PP_text(({ unsigned char* _temp937=(
unsigned char*)":"; struct _tagged_string _temp938; _temp938.curr= _temp937;
_temp938.base= _temp937; _temp938.last_plus_one= _temp937 + 2; _temp938;}));
struct Cyc_PP_Doc* _temp936= Cyc_Absynpp_rgnpo2doc( rgn_po); struct Cyc_PP_Doc*
_temp932[ 3u]={ _temp934, _temp935, _temp936}; struct _tagged_ptr1 _temp933={
_temp932, _temp932, _temp932 + 3u}; _temp931( _temp933);});} return({ struct Cyc_PP_Doc*(*
_temp939)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp942= Cyc_PP_text(({
unsigned char* _temp947=( unsigned char*)"("; struct _tagged_string _temp948;
_temp948.curr= _temp947; _temp948.base= _temp947; _temp948.last_plus_one=
_temp947 + 2; _temp948;})); struct Cyc_PP_Doc* _temp943= _temp921; struct Cyc_PP_Doc*
_temp944= Cyc_PP_text(({ unsigned char* _temp945=( unsigned char*)")"; struct
_tagged_string _temp946; _temp946.curr= _temp945; _temp946.base= _temp945;
_temp946.last_plus_one= _temp945 + 2; _temp946;})); struct Cyc_PP_Doc* _temp940[
3u]={ _temp942, _temp943, _temp944}; struct _tagged_ptr1 _temp941={ _temp940,
_temp940, _temp940 + 3u}; _temp939( _temp941);});}}} struct _tuple1* Cyc_Absynpp_arg_mk_opt(
struct _tuple3* arg){ return({ struct _tuple1* _temp949=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp949->f1=({ struct Cyc_Core_Opt*
_temp950=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp950->v=( void*)(* arg).f1; _temp950;}); _temp949->f2=(* arg).f2; _temp949->f3=(*
arg).f3; _temp949;});} struct Cyc_PP_Doc* Cyc_Absynpp_var2doc( struct
_tagged_string* v){ return Cyc_PP_text(* v);} struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc(
struct _tuple0* q){ struct Cyc_List_List* _temp951= 0; goto _LL952; _LL952:{
void* _temp953=(* q).f1; struct Cyc_List_List* _temp965; struct Cyc_List_List*
_temp967; struct Cyc_List_List* _temp969; struct Cyc_List_List* _temp971; _LL955:
if( _temp953 ==( void*) Cyc_Absyn_Loc_n){ goto _LL956;} else{ goto _LL957;}
_LL957: if(( unsigned int) _temp953 > 1u?*(( int*) _temp953) == Cyc_Absyn_Rel_n:
0){ _LL966: _temp965=(( struct Cyc_Absyn_Rel_n_struct*) _temp953)->f1; if(
_temp965 == 0){ goto _LL958;} else{ goto _LL959;}} else{ goto _LL959;} _LL959:
if(( unsigned int) _temp953 > 1u?*(( int*) _temp953) == Cyc_Absyn_Abs_n: 0){
_LL968: _temp967=(( struct Cyc_Absyn_Abs_n_struct*) _temp953)->f1; if( _temp967
== 0){ goto _LL960;} else{ goto _LL961;}} else{ goto _LL961;} _LL961: if((
unsigned int) _temp953 > 1u?*(( int*) _temp953) == Cyc_Absyn_Rel_n: 0){ _LL970:
_temp969=(( struct Cyc_Absyn_Rel_n_struct*) _temp953)->f1; goto _LL962;} else{
goto _LL963;} _LL963: if(( unsigned int) _temp953 > 1u?*(( int*) _temp953) ==
Cyc_Absyn_Abs_n: 0){ _LL972: _temp971=(( struct Cyc_Absyn_Abs_n_struct*)
_temp953)->f1; goto _LL964;} else{ goto _LL954;} _LL956: goto _LL958; _LL958:
goto _LL960; _LL960: return Cyc_Absynpp_var2doc((* q).f2); _LL962: _temp971=
_temp969; goto _LL964; _LL964: _temp951= _temp971; goto _LL954; _LL954:;} if(
Cyc_Absynpp_qvar_to_Cids){ return Cyc_PP_text(( struct _tagged_string) Cyc_String_str_sepstr(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp951,({ struct Cyc_List_List* _temp973=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp973->hd=( void*)(* q).f2; _temp973->tl= 0;
_temp973;})),({ unsigned char* _temp974=( unsigned char*)"_"; struct
_tagged_string _temp975; _temp975.curr= _temp974; _temp975.base= _temp974;
_temp975.last_plus_one= _temp974 + 2; _temp975;})));} else{ if((( int(*)( int(*
cmp)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List* l1,
struct Cyc_List_List* l2)) Cyc_List_list_prefix)( Cyc_String_strptrcmp, _temp951,
Cyc_Absynpp_curr_namespace)){ return Cyc_Absynpp_var2doc((* q).f2);} else{
return Cyc_PP_text(( struct _tagged_string) Cyc_String_str_sepstr((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp951,({
struct Cyc_List_List* _temp976=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp976->hd=( void*)(* q).f2; _temp976->tl= 0; _temp976;})),({
unsigned char* _temp977=( unsigned char*)"::"; struct _tagged_string _temp978;
_temp978.curr= _temp977; _temp978.base= _temp977; _temp978.last_plus_one=
_temp977 + 3; _temp978;})));}}} struct Cyc_PP_Doc* Cyc_Absynpp_typedef_name2doc(
struct _tuple0* v){ if( Cyc_Absynpp_qvar_to_Cids){ return Cyc_Absynpp_qvar2doc(
v);}{ void* _temp979=(* v).f1; struct Cyc_List_List* _temp985; _LL981: if((
unsigned int) _temp979 > 1u?*(( int*) _temp979) == Cyc_Absyn_Abs_n: 0){ _LL986:
_temp985=(( struct Cyc_Absyn_Abs_n_struct*) _temp979)->f1; goto _LL982;} else{
goto _LL983;} _LL983: goto _LL984; _LL982: if((( int(*)( int(* cmp)( struct
_tagged_string*, struct _tagged_string*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_List_list_cmp)( Cyc_String_strptrcmp, _temp985, Cyc_Absynpp_curr_namespace)
== 0){ return Cyc_Absynpp_var2doc((* v).f2);} else{ return({ struct Cyc_PP_Doc*(*
_temp987)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp990= Cyc_PP_text(({
unsigned char* _temp992=( unsigned char*)"/* wrong namespace : */ "; struct
_tagged_string _temp993; _temp993.curr= _temp992; _temp993.base= _temp992;
_temp993.last_plus_one= _temp992 + 25; _temp993;})); struct Cyc_PP_Doc* _temp991=
Cyc_Absynpp_qvar2doc( v); struct Cyc_PP_Doc* _temp988[ 2u]={ _temp990, _temp991};
struct _tagged_ptr1 _temp989={ _temp988, _temp988, _temp988 + 2u}; _temp987(
_temp989);});} _LL984: return Cyc_Absynpp_qvar2doc( v); _LL980:;}} struct Cyc_PP_Doc*
Cyc_Absynpp_typ2doc( void* t){ return Cyc_Absynpp_tqtd2doc( Cyc_Absyn_empty_tqual(),
t, 0);} int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp* e){ void* _temp994=(
void*) e->r; void* _temp1068; void* _temp1070; struct _tuple0* _temp1072; struct
_tuple0* _temp1074; struct Cyc_List_List* _temp1076; void* _temp1078; struct Cyc_Absyn_Exp*
_temp1080; struct Cyc_Core_Opt* _temp1082; struct Cyc_Absyn_Exp* _temp1084; void*
_temp1086; struct Cyc_Absyn_Exp* _temp1088; struct Cyc_Absyn_Exp* _temp1090;
struct Cyc_Absyn_Exp* _temp1092; struct Cyc_Absyn_Exp* _temp1094; struct Cyc_Absyn_Exp*
_temp1096; struct Cyc_Absyn_Exp* _temp1098; struct Cyc_List_List* _temp1100;
struct Cyc_Absyn_Exp* _temp1102; struct Cyc_Absyn_VarargCallInfo* _temp1104;
struct Cyc_List_List* _temp1106; struct Cyc_Absyn_Exp* _temp1108; struct Cyc_Absyn_Exp*
_temp1110; struct Cyc_Absyn_Exp* _temp1112; struct Cyc_List_List* _temp1114;
struct Cyc_Absyn_Exp* _temp1116; struct Cyc_Absyn_Exp* _temp1118; void*
_temp1120; struct Cyc_Absyn_Exp* _temp1122; struct Cyc_Absyn_Exp* _temp1124;
struct Cyc_Absyn_Exp* _temp1126; void* _temp1128; struct Cyc_Absyn_Exp*
_temp1130; struct _tagged_string* _temp1132; void* _temp1134; struct Cyc_Absyn_Exp*
_temp1136; struct _tagged_string* _temp1138; struct Cyc_Absyn_Exp* _temp1140;
struct _tagged_string* _temp1142; struct Cyc_Absyn_Exp* _temp1144; struct Cyc_Absyn_Exp*
_temp1146; struct Cyc_Absyn_Exp* _temp1148; struct Cyc_List_List* _temp1150;
struct Cyc_List_List* _temp1152; struct _tuple1* _temp1154; struct Cyc_List_List*
_temp1156; struct Cyc_Absyn_Exp* _temp1158; struct Cyc_Absyn_Exp* _temp1160;
struct Cyc_Absyn_Vardecl* _temp1162; struct Cyc_Absyn_Structdecl* _temp1164;
struct Cyc_List_List* _temp1166; struct Cyc_Core_Opt* _temp1168; struct _tuple0*
_temp1170; struct Cyc_List_List* _temp1172; void* _temp1174; struct Cyc_Absyn_Tunionfield*
_temp1176; struct Cyc_Absyn_Tuniondecl* _temp1178; struct Cyc_List_List*
_temp1180; struct Cyc_Core_Opt* _temp1182; struct Cyc_Core_Opt* _temp1184;
struct Cyc_Absyn_Enumfield* _temp1186; struct Cyc_Absyn_Enumdecl* _temp1188;
struct _tuple0* _temp1190; void* _temp1192; struct Cyc_Absyn_Exp* _temp1194;
struct Cyc_List_List* _temp1196; struct Cyc_Core_Opt* _temp1198; struct Cyc_Absyn_Stmt*
_temp1200; struct Cyc_Absyn_Fndecl* _temp1202; struct Cyc_Absyn_Exp* _temp1204;
_LL996: if(*(( int*) _temp994) == Cyc_Absyn_Const_e){ _LL1069: _temp1068=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp994)->f1; goto _LL997;} else{ goto _LL998;}
_LL998: if(*(( int*) _temp994) == Cyc_Absyn_Var_e){ _LL1073: _temp1072=(( struct
Cyc_Absyn_Var_e_struct*) _temp994)->f1; goto _LL1071; _LL1071: _temp1070=( void*)((
struct Cyc_Absyn_Var_e_struct*) _temp994)->f2; goto _LL999;} else{ goto _LL1000;}
_LL1000: if(*(( int*) _temp994) == Cyc_Absyn_UnknownId_e){ _LL1075: _temp1074=((
struct Cyc_Absyn_UnknownId_e_struct*) _temp994)->f1; goto _LL1001;} else{ goto
_LL1002;} _LL1002: if(*(( int*) _temp994) == Cyc_Absyn_Primop_e){ _LL1079:
_temp1078=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp994)->f1; goto
_LL1077; _LL1077: _temp1076=(( struct Cyc_Absyn_Primop_e_struct*) _temp994)->f2;
goto _LL1003;} else{ goto _LL1004;} _LL1004: if(*(( int*) _temp994) == Cyc_Absyn_AssignOp_e){
_LL1085: _temp1084=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp994)->f1; goto
_LL1083; _LL1083: _temp1082=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp994)->f2;
goto _LL1081; _LL1081: _temp1080=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp994)->f3; goto _LL1005;} else{ goto _LL1006;} _LL1006: if(*(( int*)
_temp994) == Cyc_Absyn_Increment_e){ _LL1089: _temp1088=(( struct Cyc_Absyn_Increment_e_struct*)
_temp994)->f1; goto _LL1087; _LL1087: _temp1086=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp994)->f2; goto _LL1007;} else{ goto _LL1008;} _LL1008: if(*(( int*)
_temp994) == Cyc_Absyn_Conditional_e){ _LL1095: _temp1094=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp994)->f1; goto _LL1093; _LL1093: _temp1092=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp994)->f2; goto _LL1091; _LL1091: _temp1090=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp994)->f3; goto _LL1009;} else{ goto _LL1010;} _LL1010: if(*(( int*)
_temp994) == Cyc_Absyn_SeqExp_e){ _LL1099: _temp1098=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp994)->f1; goto _LL1097; _LL1097: _temp1096=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp994)->f2; goto _LL1011;} else{ goto _LL1012;} _LL1012: if(*(( int*)
_temp994) == Cyc_Absyn_UnknownCall_e){ _LL1103: _temp1102=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp994)->f1; goto _LL1101; _LL1101: _temp1100=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp994)->f2; goto _LL1013;} else{ goto _LL1014;} _LL1014: if(*(( int*)
_temp994) == Cyc_Absyn_FnCall_e){ _LL1109: _temp1108=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp994)->f1; goto _LL1107; _LL1107: _temp1106=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp994)->f2; goto _LL1105; _LL1105: _temp1104=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp994)->f3; goto _LL1015;} else{ goto _LL1016;} _LL1016: if(*(( int*)
_temp994) == Cyc_Absyn_Throw_e){ _LL1111: _temp1110=(( struct Cyc_Absyn_Throw_e_struct*)
_temp994)->f1; goto _LL1017;} else{ goto _LL1018;} _LL1018: if(*(( int*)
_temp994) == Cyc_Absyn_NoInstantiate_e){ _LL1113: _temp1112=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp994)->f1; goto _LL1019;} else{ goto _LL1020;} _LL1020: if(*(( int*)
_temp994) == Cyc_Absyn_Instantiate_e){ _LL1117: _temp1116=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp994)->f1; goto _LL1115; _LL1115: _temp1114=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp994)->f2; goto _LL1021;} else{ goto _LL1022;} _LL1022: if(*(( int*)
_temp994) == Cyc_Absyn_Cast_e){ _LL1121: _temp1120=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp994)->f1; goto _LL1119; _LL1119: _temp1118=(( struct Cyc_Absyn_Cast_e_struct*)
_temp994)->f2; goto _LL1023;} else{ goto _LL1024;} _LL1024: if(*(( int*)
_temp994) == Cyc_Absyn_New_e){ _LL1125: _temp1124=(( struct Cyc_Absyn_New_e_struct*)
_temp994)->f1; goto _LL1123; _LL1123: _temp1122=(( struct Cyc_Absyn_New_e_struct*)
_temp994)->f2; goto _LL1025;} else{ goto _LL1026;} _LL1026: if(*(( int*)
_temp994) == Cyc_Absyn_Address_e){ _LL1127: _temp1126=(( struct Cyc_Absyn_Address_e_struct*)
_temp994)->f1; goto _LL1027;} else{ goto _LL1028;} _LL1028: if(*(( int*)
_temp994) == Cyc_Absyn_Sizeoftyp_e){ _LL1129: _temp1128=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp994)->f1; goto _LL1029;} else{ goto _LL1030;} _LL1030: if(*(( int*)
_temp994) == Cyc_Absyn_Sizeofexp_e){ _LL1131: _temp1130=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp994)->f1; goto _LL1031;} else{ goto _LL1032;} _LL1032: if(*(( int*)
_temp994) == Cyc_Absyn_Offsetof_e){ _LL1135: _temp1134=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp994)->f1; goto _LL1133; _LL1133: _temp1132=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp994)->f2; goto _LL1033;} else{ goto _LL1034;} _LL1034: if(*(( int*)
_temp994) == Cyc_Absyn_Deref_e){ _LL1137: _temp1136=(( struct Cyc_Absyn_Deref_e_struct*)
_temp994)->f1; goto _LL1035;} else{ goto _LL1036;} _LL1036: if(*(( int*)
_temp994) == Cyc_Absyn_StructMember_e){ _LL1141: _temp1140=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp994)->f1; goto _LL1139; _LL1139: _temp1138=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp994)->f2; goto _LL1037;} else{ goto _LL1038;} _LL1038: if(*(( int*)
_temp994) == Cyc_Absyn_StructArrow_e){ _LL1145: _temp1144=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp994)->f1; goto _LL1143; _LL1143: _temp1142=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp994)->f2; goto _LL1039;} else{ goto _LL1040;} _LL1040: if(*(( int*)
_temp994) == Cyc_Absyn_Subscript_e){ _LL1149: _temp1148=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp994)->f1; goto _LL1147; _LL1147: _temp1146=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp994)->f2; goto _LL1041;} else{ goto _LL1042;} _LL1042: if(*(( int*)
_temp994) == Cyc_Absyn_Tuple_e){ _LL1151: _temp1150=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp994)->f1; goto _LL1043;} else{ goto _LL1044;} _LL1044: if(*(( int*)
_temp994) == Cyc_Absyn_CompoundLit_e){ _LL1155: _temp1154=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp994)->f1; goto _LL1153; _LL1153: _temp1152=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp994)->f2; goto _LL1045;} else{ goto _LL1046;} _LL1046: if(*(( int*)
_temp994) == Cyc_Absyn_Array_e){ _LL1157: _temp1156=(( struct Cyc_Absyn_Array_e_struct*)
_temp994)->f1; goto _LL1047;} else{ goto _LL1048;} _LL1048: if(*(( int*)
_temp994) == Cyc_Absyn_Comprehension_e){ _LL1163: _temp1162=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp994)->f1; goto _LL1161; _LL1161: _temp1160=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp994)->f2; goto _LL1159; _LL1159: _temp1158=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp994)->f3; goto _LL1049;} else{ goto _LL1050;} _LL1050: if(*(( int*)
_temp994) == Cyc_Absyn_Struct_e){ _LL1171: _temp1170=(( struct Cyc_Absyn_Struct_e_struct*)
_temp994)->f1; goto _LL1169; _LL1169: _temp1168=(( struct Cyc_Absyn_Struct_e_struct*)
_temp994)->f2; goto _LL1167; _LL1167: _temp1166=(( struct Cyc_Absyn_Struct_e_struct*)
_temp994)->f3; goto _LL1165; _LL1165: _temp1164=(( struct Cyc_Absyn_Struct_e_struct*)
_temp994)->f4; goto _LL1051;} else{ goto _LL1052;} _LL1052: if(*(( int*)
_temp994) == Cyc_Absyn_AnonStruct_e){ _LL1175: _temp1174=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp994)->f1; goto _LL1173; _LL1173: _temp1172=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp994)->f2; goto _LL1053;} else{ goto _LL1054;} _LL1054: if(*(( int*)
_temp994) == Cyc_Absyn_Tunion_e){ _LL1185: _temp1184=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp994)->f1; goto _LL1183; _LL1183: _temp1182=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp994)->f2; goto _LL1181; _LL1181: _temp1180=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp994)->f3; goto _LL1179; _LL1179: _temp1178=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp994)->f4; goto _LL1177; _LL1177: _temp1176=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp994)->f5; goto _LL1055;} else{ goto _LL1056;} _LL1056: if(*(( int*)
_temp994) == Cyc_Absyn_Enum_e){ _LL1191: _temp1190=(( struct Cyc_Absyn_Enum_e_struct*)
_temp994)->f1; goto _LL1189; _LL1189: _temp1188=(( struct Cyc_Absyn_Enum_e_struct*)
_temp994)->f2; goto _LL1187; _LL1187: _temp1186=(( struct Cyc_Absyn_Enum_e_struct*)
_temp994)->f3; goto _LL1057;} else{ goto _LL1058;} _LL1058: if(*(( int*)
_temp994) == Cyc_Absyn_Malloc_e){ _LL1195: _temp1194=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp994)->f1; goto _LL1193; _LL1193: _temp1192=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp994)->f2; goto _LL1059;} else{ goto _LL1060;} _LL1060: if(*(( int*)
_temp994) == Cyc_Absyn_UnresolvedMem_e){ _LL1199: _temp1198=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp994)->f1; goto _LL1197; _LL1197: _temp1196=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp994)->f2; goto _LL1061;} else{ goto _LL1062;} _LL1062: if(*(( int*)
_temp994) == Cyc_Absyn_StmtExp_e){ _LL1201: _temp1200=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp994)->f1; goto _LL1063;} else{ goto _LL1064;} _LL1064: if(*(( int*)
_temp994) == Cyc_Absyn_Codegen_e){ _LL1203: _temp1202=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp994)->f1; goto _LL1065;} else{ goto _LL1066;} _LL1066: if(*(( int*)
_temp994) == Cyc_Absyn_Fill_e){ _LL1205: _temp1204=(( struct Cyc_Absyn_Fill_e_struct*)
_temp994)->f1; goto _LL1067;} else{ goto _LL995;} _LL997: return 10000; _LL999:
return 10000; _LL1001: return 10000; _LL1003: { void* _temp1206= _temp1078;
_LL1208: if( _temp1206 ==( void*) Cyc_Absyn_Plus){ goto _LL1209;} else{ goto
_LL1210;} _LL1210: if( _temp1206 ==( void*) Cyc_Absyn_Times){ goto _LL1211;}
else{ goto _LL1212;} _LL1212: if( _temp1206 ==( void*) Cyc_Absyn_Minus){ goto
_LL1213;} else{ goto _LL1214;} _LL1214: if( _temp1206 ==( void*) Cyc_Absyn_Div){
goto _LL1215;} else{ goto _LL1216;} _LL1216: if( _temp1206 ==( void*) Cyc_Absyn_Mod){
goto _LL1217;} else{ goto _LL1218;} _LL1218: if( _temp1206 ==( void*) Cyc_Absyn_Eq){
goto _LL1219;} else{ goto _LL1220;} _LL1220: if( _temp1206 ==( void*) Cyc_Absyn_Neq){
goto _LL1221;} else{ goto _LL1222;} _LL1222: if( _temp1206 ==( void*) Cyc_Absyn_Gt){
goto _LL1223;} else{ goto _LL1224;} _LL1224: if( _temp1206 ==( void*) Cyc_Absyn_Lt){
goto _LL1225;} else{ goto _LL1226;} _LL1226: if( _temp1206 ==( void*) Cyc_Absyn_Gte){
goto _LL1227;} else{ goto _LL1228;} _LL1228: if( _temp1206 ==( void*) Cyc_Absyn_Lte){
goto _LL1229;} else{ goto _LL1230;} _LL1230: if( _temp1206 ==( void*) Cyc_Absyn_Not){
goto _LL1231;} else{ goto _LL1232;} _LL1232: if( _temp1206 ==( void*) Cyc_Absyn_Bitnot){
goto _LL1233;} else{ goto _LL1234;} _LL1234: if( _temp1206 ==( void*) Cyc_Absyn_Bitand){
goto _LL1235;} else{ goto _LL1236;} _LL1236: if( _temp1206 ==( void*) Cyc_Absyn_Bitor){
goto _LL1237;} else{ goto _LL1238;} _LL1238: if( _temp1206 ==( void*) Cyc_Absyn_Bitxor){
goto _LL1239;} else{ goto _LL1240;} _LL1240: if( _temp1206 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL1241;} else{ goto _LL1242;} _LL1242: if( _temp1206 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL1243;} else{ goto _LL1244;} _LL1244: if( _temp1206 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL1245;} else{ goto _LL1246;} _LL1246: if( _temp1206 ==( void*) Cyc_Absyn_Size){
goto _LL1247;} else{ goto _LL1248;} _LL1248: if( _temp1206 ==( void*) Cyc_Absyn_Printf){
goto _LL1249;} else{ goto _LL1250;} _LL1250: if( _temp1206 ==( void*) Cyc_Absyn_Fprintf){
goto _LL1251;} else{ goto _LL1252;} _LL1252: if( _temp1206 ==( void*) Cyc_Absyn_Xprintf){
goto _LL1253;} else{ goto _LL1254;} _LL1254: if( _temp1206 ==( void*) Cyc_Absyn_Scanf){
goto _LL1255;} else{ goto _LL1256;} _LL1256: if( _temp1206 ==( void*) Cyc_Absyn_Fscanf){
goto _LL1257;} else{ goto _LL1258;} _LL1258: if( _temp1206 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1259;} else{ goto _LL1207;} _LL1209: return 100; _LL1211: return 110;
_LL1213: return 100; _LL1215: return 110; _LL1217: return 110; _LL1219: return
70; _LL1221: return 70; _LL1223: return 80; _LL1225: return 80; _LL1227: return
80; _LL1229: return 80; _LL1231: return 130; _LL1233: return 130; _LL1235:
return 60; _LL1237: return 40; _LL1239: return 50; _LL1241: return 90; _LL1243:
return 80; _LL1245: return 80; _LL1247: return 140; _LL1249: return 130; _LL1251:
return 130; _LL1253: return 130; _LL1255: return 130; _LL1257: return 130;
_LL1259: return 130; _LL1207:;} _LL1005: return 20; _LL1007: return 130; _LL1009:
return 30; _LL1011: return 10; _LL1013: return 140; _LL1015: return 140; _LL1017:
return 130; _LL1019: return Cyc_Absynpp_exp_prec( _temp1112); _LL1021: return
Cyc_Absynpp_exp_prec( _temp1116); _LL1023: return 120; _LL1025: return 15;
_LL1027: return 130; _LL1029: return 130; _LL1031: return 130; _LL1033: return
130; _LL1035: return 130; _LL1037: return 140; _LL1039: return 140; _LL1041:
return 140; _LL1043: return 150; _LL1045: goto _LL1047; _LL1047: goto _LL1049;
_LL1049: goto _LL1051; _LL1051: goto _LL1053; _LL1053: goto _LL1055; _LL1055:
goto _LL1057; _LL1057: goto _LL1059; _LL1059: return 140; _LL1061: return 140;
_LL1063: return 10000; _LL1065: return 140; _LL1067: return 140; _LL995:;}
struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp* e){ return Cyc_Absynpp_exp2doc_prec(
0, e);} struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec( int inprec, struct Cyc_Absyn_Exp*
e){ int myprec= Cyc_Absynpp_exp_prec( e); struct Cyc_PP_Doc* s;{ void* _temp1260=(
void*) e->r; void* _temp1334; void* _temp1336; struct _tuple0* _temp1338; struct
_tuple0* _temp1340; struct Cyc_List_List* _temp1342; void* _temp1344; struct Cyc_Absyn_Exp*
_temp1346; struct Cyc_Core_Opt* _temp1348; struct Cyc_Absyn_Exp* _temp1350; void*
_temp1352; struct Cyc_Absyn_Exp* _temp1354; struct Cyc_Absyn_Exp* _temp1356;
struct Cyc_Absyn_Exp* _temp1358; struct Cyc_Absyn_Exp* _temp1360; struct Cyc_Absyn_Exp*
_temp1362; struct Cyc_Absyn_Exp* _temp1364; struct Cyc_List_List* _temp1366;
struct Cyc_Absyn_Exp* _temp1368; struct Cyc_Absyn_VarargCallInfo* _temp1370;
struct Cyc_List_List* _temp1372; struct Cyc_Absyn_Exp* _temp1374; struct Cyc_Absyn_Exp*
_temp1376; struct Cyc_Absyn_Exp* _temp1378; struct Cyc_List_List* _temp1380;
struct Cyc_Absyn_Exp* _temp1382; struct Cyc_Absyn_Exp* _temp1384; void*
_temp1386; struct Cyc_Absyn_Exp* _temp1388; struct Cyc_Absyn_Exp* _temp1390;
struct Cyc_Absyn_Exp* _temp1392; void* _temp1394; struct Cyc_Absyn_Exp*
_temp1396; struct _tagged_string* _temp1398; void* _temp1400; struct Cyc_Absyn_Exp*
_temp1402; struct _tagged_string* _temp1404; struct Cyc_Absyn_Exp* _temp1406;
struct _tagged_string* _temp1408; struct Cyc_Absyn_Exp* _temp1410; struct Cyc_Absyn_Exp*
_temp1412; struct Cyc_Absyn_Exp* _temp1414; struct Cyc_List_List* _temp1416;
struct Cyc_List_List* _temp1418; struct _tuple1* _temp1420; struct Cyc_List_List*
_temp1422; struct Cyc_Absyn_Exp* _temp1424; struct Cyc_Absyn_Exp* _temp1426;
struct Cyc_Absyn_Vardecl* _temp1428; struct Cyc_Absyn_Structdecl* _temp1430;
struct Cyc_List_List* _temp1432; struct Cyc_Core_Opt* _temp1434; struct _tuple0*
_temp1436; struct Cyc_List_List* _temp1438; void* _temp1440; struct Cyc_Absyn_Tunionfield*
_temp1442; struct Cyc_Absyn_Tuniondecl* _temp1444; struct Cyc_List_List*
_temp1446; struct Cyc_Core_Opt* _temp1448; struct Cyc_Core_Opt* _temp1450;
struct Cyc_Absyn_Enumfield* _temp1452; struct Cyc_Absyn_Enumdecl* _temp1454;
struct _tuple0* _temp1456; void* _temp1458; struct Cyc_Absyn_Exp* _temp1460;
struct Cyc_List_List* _temp1462; struct Cyc_Core_Opt* _temp1464; struct Cyc_Absyn_Stmt*
_temp1466; struct Cyc_Absyn_Fndecl* _temp1468; struct Cyc_Absyn_Exp* _temp1470;
_LL1262: if(*(( int*) _temp1260) == Cyc_Absyn_Const_e){ _LL1335: _temp1334=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1260)->f1; goto _LL1263;} else{
goto _LL1264;} _LL1264: if(*(( int*) _temp1260) == Cyc_Absyn_Var_e){ _LL1339:
_temp1338=(( struct Cyc_Absyn_Var_e_struct*) _temp1260)->f1; goto _LL1337;
_LL1337: _temp1336=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1260)->f2;
goto _LL1265;} else{ goto _LL1266;} _LL1266: if(*(( int*) _temp1260) == Cyc_Absyn_UnknownId_e){
_LL1341: _temp1340=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1260)->f1; goto
_LL1267;} else{ goto _LL1268;} _LL1268: if(*(( int*) _temp1260) == Cyc_Absyn_Primop_e){
_LL1345: _temp1344=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1260)->f1;
goto _LL1343; _LL1343: _temp1342=(( struct Cyc_Absyn_Primop_e_struct*) _temp1260)->f2;
goto _LL1269;} else{ goto _LL1270;} _LL1270: if(*(( int*) _temp1260) == Cyc_Absyn_AssignOp_e){
_LL1351: _temp1350=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp1260)->f1; goto
_LL1349; _LL1349: _temp1348=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp1260)->f2;
goto _LL1347; _LL1347: _temp1346=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1260)->f3; goto _LL1271;} else{ goto _LL1272;} _LL1272: if(*(( int*)
_temp1260) == Cyc_Absyn_Increment_e){ _LL1355: _temp1354=(( struct Cyc_Absyn_Increment_e_struct*)
_temp1260)->f1; goto _LL1353; _LL1353: _temp1352=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1260)->f2; goto _LL1273;} else{ goto _LL1274;} _LL1274: if(*(( int*)
_temp1260) == Cyc_Absyn_Conditional_e){ _LL1361: _temp1360=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1260)->f1; goto _LL1359; _LL1359: _temp1358=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1260)->f2; goto _LL1357; _LL1357: _temp1356=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1260)->f3; goto _LL1275;} else{ goto _LL1276;} _LL1276: if(*(( int*)
_temp1260) == Cyc_Absyn_SeqExp_e){ _LL1365: _temp1364=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1260)->f1; goto _LL1363; _LL1363: _temp1362=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1260)->f2; goto _LL1277;} else{ goto _LL1278;} _LL1278: if(*(( int*)
_temp1260) == Cyc_Absyn_UnknownCall_e){ _LL1369: _temp1368=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1260)->f1; goto _LL1367; _LL1367: _temp1366=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1260)->f2; goto _LL1279;} else{ goto _LL1280;} _LL1280: if(*(( int*)
_temp1260) == Cyc_Absyn_FnCall_e){ _LL1375: _temp1374=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1260)->f1; goto _LL1373; _LL1373: _temp1372=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1260)->f2; goto _LL1371; _LL1371: _temp1370=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1260)->f3; goto _LL1281;} else{ goto _LL1282;} _LL1282: if(*(( int*)
_temp1260) == Cyc_Absyn_Throw_e){ _LL1377: _temp1376=(( struct Cyc_Absyn_Throw_e_struct*)
_temp1260)->f1; goto _LL1283;} else{ goto _LL1284;} _LL1284: if(*(( int*)
_temp1260) == Cyc_Absyn_NoInstantiate_e){ _LL1379: _temp1378=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1260)->f1; goto _LL1285;} else{ goto _LL1286;} _LL1286: if(*(( int*)
_temp1260) == Cyc_Absyn_Instantiate_e){ _LL1383: _temp1382=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1260)->f1; goto _LL1381; _LL1381: _temp1380=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1260)->f2; goto _LL1287;} else{ goto _LL1288;} _LL1288: if(*(( int*)
_temp1260) == Cyc_Absyn_Cast_e){ _LL1387: _temp1386=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1260)->f1; goto _LL1385; _LL1385: _temp1384=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1260)->f2; goto _LL1289;} else{ goto _LL1290;} _LL1290: if(*(( int*)
_temp1260) == Cyc_Absyn_Address_e){ _LL1389: _temp1388=(( struct Cyc_Absyn_Address_e_struct*)
_temp1260)->f1; goto _LL1291;} else{ goto _LL1292;} _LL1292: if(*(( int*)
_temp1260) == Cyc_Absyn_New_e){ _LL1393: _temp1392=(( struct Cyc_Absyn_New_e_struct*)
_temp1260)->f1; goto _LL1391; _LL1391: _temp1390=(( struct Cyc_Absyn_New_e_struct*)
_temp1260)->f2; goto _LL1293;} else{ goto _LL1294;} _LL1294: if(*(( int*)
_temp1260) == Cyc_Absyn_Sizeoftyp_e){ _LL1395: _temp1394=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1260)->f1; goto _LL1295;} else{ goto _LL1296;} _LL1296: if(*(( int*)
_temp1260) == Cyc_Absyn_Sizeofexp_e){ _LL1397: _temp1396=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1260)->f1; goto _LL1297;} else{ goto _LL1298;} _LL1298: if(*(( int*)
_temp1260) == Cyc_Absyn_Offsetof_e){ _LL1401: _temp1400=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1260)->f1; goto _LL1399; _LL1399: _temp1398=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1260)->f2; goto _LL1299;} else{ goto _LL1300;} _LL1300: if(*(( int*)
_temp1260) == Cyc_Absyn_Deref_e){ _LL1403: _temp1402=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1260)->f1; goto _LL1301;} else{ goto _LL1302;} _LL1302: if(*(( int*)
_temp1260) == Cyc_Absyn_StructMember_e){ _LL1407: _temp1406=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1260)->f1; goto _LL1405; _LL1405: _temp1404=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1260)->f2; goto _LL1303;} else{ goto _LL1304;} _LL1304: if(*(( int*)
_temp1260) == Cyc_Absyn_StructArrow_e){ _LL1411: _temp1410=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1260)->f1; goto _LL1409; _LL1409: _temp1408=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1260)->f2; goto _LL1305;} else{ goto _LL1306;} _LL1306: if(*(( int*)
_temp1260) == Cyc_Absyn_Subscript_e){ _LL1415: _temp1414=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1260)->f1; goto _LL1413; _LL1413: _temp1412=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1260)->f2; goto _LL1307;} else{ goto _LL1308;} _LL1308: if(*(( int*)
_temp1260) == Cyc_Absyn_Tuple_e){ _LL1417: _temp1416=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1260)->f1; goto _LL1309;} else{ goto _LL1310;} _LL1310: if(*(( int*)
_temp1260) == Cyc_Absyn_CompoundLit_e){ _LL1421: _temp1420=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1260)->f1; goto _LL1419; _LL1419: _temp1418=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1260)->f2; goto _LL1311;} else{ goto _LL1312;} _LL1312: if(*(( int*)
_temp1260) == Cyc_Absyn_Array_e){ _LL1423: _temp1422=(( struct Cyc_Absyn_Array_e_struct*)
_temp1260)->f1; goto _LL1313;} else{ goto _LL1314;} _LL1314: if(*(( int*)
_temp1260) == Cyc_Absyn_Comprehension_e){ _LL1429: _temp1428=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1260)->f1; goto _LL1427; _LL1427: _temp1426=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1260)->f2; goto _LL1425; _LL1425: _temp1424=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1260)->f3; goto _LL1315;} else{ goto _LL1316;} _LL1316: if(*(( int*)
_temp1260) == Cyc_Absyn_Struct_e){ _LL1437: _temp1436=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1260)->f1; goto _LL1435; _LL1435: _temp1434=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1260)->f2; goto _LL1433; _LL1433: _temp1432=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1260)->f3; goto _LL1431; _LL1431: _temp1430=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1260)->f4; goto _LL1317;} else{ goto _LL1318;} _LL1318: if(*(( int*)
_temp1260) == Cyc_Absyn_AnonStruct_e){ _LL1441: _temp1440=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1260)->f1; goto _LL1439; _LL1439: _temp1438=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1260)->f2; goto _LL1319;} else{ goto _LL1320;} _LL1320: if(*(( int*)
_temp1260) == Cyc_Absyn_Tunion_e){ _LL1451: _temp1450=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1260)->f1; goto _LL1449; _LL1449: _temp1448=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1260)->f2; goto _LL1447; _LL1447: _temp1446=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1260)->f3; goto _LL1445; _LL1445: _temp1444=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1260)->f4; goto _LL1443; _LL1443: _temp1442=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1260)->f5; goto _LL1321;} else{ goto _LL1322;} _LL1322: if(*(( int*)
_temp1260) == Cyc_Absyn_Enum_e){ _LL1457: _temp1456=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1260)->f1; goto _LL1455; _LL1455: _temp1454=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1260)->f2; goto _LL1453; _LL1453: _temp1452=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1260)->f3; goto _LL1323;} else{ goto _LL1324;} _LL1324: if(*(( int*)
_temp1260) == Cyc_Absyn_Malloc_e){ _LL1461: _temp1460=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1260)->f1; goto _LL1459; _LL1459: _temp1458=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1260)->f2; goto _LL1325;} else{ goto _LL1326;} _LL1326: if(*(( int*)
_temp1260) == Cyc_Absyn_UnresolvedMem_e){ _LL1465: _temp1464=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1260)->f1; goto _LL1463; _LL1463: _temp1462=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1260)->f2; goto _LL1327;} else{ goto _LL1328;} _LL1328: if(*(( int*)
_temp1260) == Cyc_Absyn_StmtExp_e){ _LL1467: _temp1466=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1260)->f1; goto _LL1329;} else{ goto _LL1330;} _LL1330: if(*(( int*)
_temp1260) == Cyc_Absyn_Codegen_e){ _LL1469: _temp1468=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1260)->f1; goto _LL1331;} else{ goto _LL1332;} _LL1332: if(*(( int*)
_temp1260) == Cyc_Absyn_Fill_e){ _LL1471: _temp1470=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1260)->f1; goto _LL1333;} else{ goto _LL1261;} _LL1263: s= Cyc_Absynpp_cnst2doc(
_temp1334); goto _LL1261; _LL1265: _temp1340= _temp1338; goto _LL1267; _LL1267:
s= Cyc_Absynpp_qvar2doc( _temp1340); goto _LL1261; _LL1269: s= Cyc_Absynpp_primapp2doc(
myprec, _temp1344, _temp1342); goto _LL1261; _LL1271: s=({ struct Cyc_PP_Doc*(*
_temp1472)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1475= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1350); struct Cyc_PP_Doc* _temp1476= Cyc_PP_text(({ unsigned char*
_temp1484=( unsigned char*)" "; struct _tagged_string _temp1485; _temp1485.curr=
_temp1484; _temp1485.base= _temp1484; _temp1485.last_plus_one= _temp1484 + 2;
_temp1485;})); struct Cyc_PP_Doc* _temp1477= _temp1348 == 0? Cyc_PP_text(({
unsigned char* _temp1482=( unsigned char*)""; struct _tagged_string _temp1483;
_temp1483.curr= _temp1482; _temp1483.base= _temp1482; _temp1483.last_plus_one=
_temp1482 + 1; _temp1483;})): Cyc_Absynpp_prim2doc(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1348))->v); struct Cyc_PP_Doc* _temp1478= Cyc_PP_text(({
unsigned char* _temp1480=( unsigned char*)"= "; struct _tagged_string _temp1481;
_temp1481.curr= _temp1480; _temp1481.base= _temp1480; _temp1481.last_plus_one=
_temp1480 + 3; _temp1481;})); struct Cyc_PP_Doc* _temp1479= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1346); struct Cyc_PP_Doc* _temp1473[ 5u]={ _temp1475, _temp1476,
_temp1477, _temp1478, _temp1479}; struct _tagged_ptr1 _temp1474={ _temp1473,
_temp1473, _temp1473 + 5u}; _temp1472( _temp1474);}); goto _LL1261; _LL1273: {
struct Cyc_PP_Doc* _temp1486= Cyc_Absynpp_exp2doc_prec( myprec, _temp1354); goto
_LL1487; _LL1487:{ void* _temp1488= _temp1352; _LL1490: if( _temp1488 ==( void*)
Cyc_Absyn_PreInc){ goto _LL1491;} else{ goto _LL1492;} _LL1492: if( _temp1488 ==(
void*) Cyc_Absyn_PreDec){ goto _LL1493;} else{ goto _LL1494;} _LL1494: if(
_temp1488 ==( void*) Cyc_Absyn_PostInc){ goto _LL1495;} else{ goto _LL1496;}
_LL1496: if( _temp1488 ==( void*) Cyc_Absyn_PostDec){ goto _LL1497;} else{ goto
_LL1489;} _LL1491: s=({ struct Cyc_PP_Doc*(* _temp1498)( struct _tagged_ptr1)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1501= Cyc_PP_text(({ unsigned char*
_temp1503=( unsigned char*)"++"; struct _tagged_string _temp1504; _temp1504.curr=
_temp1503; _temp1504.base= _temp1503; _temp1504.last_plus_one= _temp1503 + 3;
_temp1504;})); struct Cyc_PP_Doc* _temp1502= _temp1486; struct Cyc_PP_Doc*
_temp1499[ 2u]={ _temp1501, _temp1502}; struct _tagged_ptr1 _temp1500={
_temp1499, _temp1499, _temp1499 + 2u}; _temp1498( _temp1500);}); goto _LL1489;
_LL1493: s=({ struct Cyc_PP_Doc*(* _temp1505)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1508= Cyc_PP_text(({ unsigned char* _temp1510=(
unsigned char*)"--"; struct _tagged_string _temp1511; _temp1511.curr= _temp1510;
_temp1511.base= _temp1510; _temp1511.last_plus_one= _temp1510 + 3; _temp1511;}));
struct Cyc_PP_Doc* _temp1509= _temp1486; struct Cyc_PP_Doc* _temp1506[ 2u]={
_temp1508, _temp1509}; struct _tagged_ptr1 _temp1507={ _temp1506, _temp1506,
_temp1506 + 2u}; _temp1505( _temp1507);}); goto _LL1489; _LL1495: s=({ struct
Cyc_PP_Doc*(* _temp1512)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1515= _temp1486; struct Cyc_PP_Doc* _temp1516= Cyc_PP_text(({ unsigned char*
_temp1517=( unsigned char*)"++"; struct _tagged_string _temp1518; _temp1518.curr=
_temp1517; _temp1518.base= _temp1517; _temp1518.last_plus_one= _temp1517 + 3;
_temp1518;})); struct Cyc_PP_Doc* _temp1513[ 2u]={ _temp1515, _temp1516}; struct
_tagged_ptr1 _temp1514={ _temp1513, _temp1513, _temp1513 + 2u}; _temp1512(
_temp1514);}); goto _LL1489; _LL1497: s=({ struct Cyc_PP_Doc*(* _temp1519)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1522= _temp1486;
struct Cyc_PP_Doc* _temp1523= Cyc_PP_text(({ unsigned char* _temp1524=(
unsigned char*)"--"; struct _tagged_string _temp1525; _temp1525.curr= _temp1524;
_temp1525.base= _temp1524; _temp1525.last_plus_one= _temp1524 + 3; _temp1525;}));
struct Cyc_PP_Doc* _temp1520[ 2u]={ _temp1522, _temp1523}; struct _tagged_ptr1
_temp1521={ _temp1520, _temp1520, _temp1520 + 2u}; _temp1519( _temp1521);});
goto _LL1489; _LL1489:;} goto _LL1261;} _LL1275:{ struct _tuple8 _temp1527=({
struct _tuple8 _temp1526; _temp1526.f1=( void*) _temp1358->r; _temp1526.f2=(
void*) _temp1356->r; _temp1526;}); _LL1529: goto _LL1530; _LL1530: s=({ struct
Cyc_PP_Doc*(* _temp1531)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1534= Cyc_Absynpp_exp2doc_prec( myprec, _temp1360); struct Cyc_PP_Doc*
_temp1535= Cyc_PP_text(({ unsigned char* _temp1541=( unsigned char*)" ? ";
struct _tagged_string _temp1542; _temp1542.curr= _temp1541; _temp1542.base=
_temp1541; _temp1542.last_plus_one= _temp1541 + 4; _temp1542;})); struct Cyc_PP_Doc*
_temp1536= Cyc_Absynpp_exp2doc_prec( 0, _temp1358); struct Cyc_PP_Doc* _temp1537=
Cyc_PP_text(({ unsigned char* _temp1539=( unsigned char*)" : "; struct
_tagged_string _temp1540; _temp1540.curr= _temp1539; _temp1540.base= _temp1539;
_temp1540.last_plus_one= _temp1539 + 4; _temp1540;})); struct Cyc_PP_Doc*
_temp1538= Cyc_Absynpp_exp2doc_prec( myprec, _temp1356); struct Cyc_PP_Doc*
_temp1532[ 5u]={ _temp1534, _temp1535, _temp1536, _temp1537, _temp1538}; struct
_tagged_ptr1 _temp1533={ _temp1532, _temp1532, _temp1532 + 5u}; _temp1531(
_temp1533);}); goto _LL1528; _LL1528:;} goto _LL1261; _LL1277: s=({ struct Cyc_PP_Doc*(*
_temp1543)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1546= Cyc_PP_text(({
unsigned char* _temp1555=( unsigned char*)"("; struct _tagged_string _temp1556;
_temp1556.curr= _temp1555; _temp1556.base= _temp1555; _temp1556.last_plus_one=
_temp1555 + 2; _temp1556;})); struct Cyc_PP_Doc* _temp1547= Cyc_Absynpp_exp2doc(
_temp1364); struct Cyc_PP_Doc* _temp1548= Cyc_PP_text(({ unsigned char*
_temp1553=( unsigned char*)", "; struct _tagged_string _temp1554; _temp1554.curr=
_temp1553; _temp1554.base= _temp1553; _temp1554.last_plus_one= _temp1553 + 3;
_temp1554;})); struct Cyc_PP_Doc* _temp1549= Cyc_Absynpp_exp2doc( _temp1362);
struct Cyc_PP_Doc* _temp1550= Cyc_PP_text(({ unsigned char* _temp1551=(
unsigned char*)")"; struct _tagged_string _temp1552; _temp1552.curr= _temp1551;
_temp1552.base= _temp1551; _temp1552.last_plus_one= _temp1551 + 2; _temp1552;}));
struct Cyc_PP_Doc* _temp1544[ 5u]={ _temp1546, _temp1547, _temp1548, _temp1549,
_temp1550}; struct _tagged_ptr1 _temp1545={ _temp1544, _temp1544, _temp1544 + 5u};
_temp1543( _temp1545);}); goto _LL1261; _LL1279: s=({ struct Cyc_PP_Doc*(*
_temp1557)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1560= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1368); struct Cyc_PP_Doc* _temp1561= Cyc_PP_text(({ unsigned char*
_temp1566=( unsigned char*)"("; struct _tagged_string _temp1567; _temp1567.curr=
_temp1566; _temp1567.base= _temp1566; _temp1567.last_plus_one= _temp1566 + 2;
_temp1567;})); struct Cyc_PP_Doc* _temp1562= Cyc_Absynpp_exps2doc_prec( 20,
_temp1366); struct Cyc_PP_Doc* _temp1563= Cyc_PP_text(({ unsigned char*
_temp1564=( unsigned char*)")"; struct _tagged_string _temp1565; _temp1565.curr=
_temp1564; _temp1565.base= _temp1564; _temp1565.last_plus_one= _temp1564 + 2;
_temp1565;})); struct Cyc_PP_Doc* _temp1558[ 4u]={ _temp1560, _temp1561,
_temp1562, _temp1563}; struct _tagged_ptr1 _temp1559={ _temp1558, _temp1558,
_temp1558 + 4u}; _temp1557( _temp1559);}); goto _LL1261; _LL1281: s=({ struct
Cyc_PP_Doc*(* _temp1568)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1571= Cyc_Absynpp_exp2doc_prec( myprec, _temp1374); struct Cyc_PP_Doc*
_temp1572= Cyc_PP_text(({ unsigned char* _temp1577=( unsigned char*)"("; struct
_tagged_string _temp1578; _temp1578.curr= _temp1577; _temp1578.base= _temp1577;
_temp1578.last_plus_one= _temp1577 + 2; _temp1578;})); struct Cyc_PP_Doc*
_temp1573= Cyc_Absynpp_exps2doc_prec( 20, _temp1372); struct Cyc_PP_Doc*
_temp1574= Cyc_PP_text(({ unsigned char* _temp1575=( unsigned char*)")"; struct
_tagged_string _temp1576; _temp1576.curr= _temp1575; _temp1576.base= _temp1575;
_temp1576.last_plus_one= _temp1575 + 2; _temp1576;})); struct Cyc_PP_Doc*
_temp1569[ 4u]={ _temp1571, _temp1572, _temp1573, _temp1574}; struct
_tagged_ptr1 _temp1570={ _temp1569, _temp1569, _temp1569 + 4u}; _temp1568(
_temp1570);}); goto _LL1261; _LL1283: s=({ struct Cyc_PP_Doc*(* _temp1579)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1582= Cyc_PP_text(({
unsigned char* _temp1584=( unsigned char*)"throw "; struct _tagged_string
_temp1585; _temp1585.curr= _temp1584; _temp1585.base= _temp1584; _temp1585.last_plus_one=
_temp1584 + 7; _temp1585;})); struct Cyc_PP_Doc* _temp1583= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1376); struct Cyc_PP_Doc* _temp1580[ 2u]={ _temp1582, _temp1583};
struct _tagged_ptr1 _temp1581={ _temp1580, _temp1580, _temp1580 + 2u}; _temp1579(
_temp1581);}); goto _LL1261; _LL1285: s= Cyc_Absynpp_exp2doc_prec( inprec,
_temp1378); goto _LL1261; _LL1287: s= Cyc_Absynpp_exp2doc_prec( inprec,
_temp1382); goto _LL1261; _LL1289: s=({ struct Cyc_PP_Doc*(* _temp1586)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1589= Cyc_PP_text(({
unsigned char* _temp1595=( unsigned char*)"("; struct _tagged_string _temp1596;
_temp1596.curr= _temp1595; _temp1596.base= _temp1595; _temp1596.last_plus_one=
_temp1595 + 2; _temp1596;})); struct Cyc_PP_Doc* _temp1590= Cyc_Absynpp_typ2doc(
_temp1386); struct Cyc_PP_Doc* _temp1591= Cyc_PP_text(({ unsigned char*
_temp1593=( unsigned char*)")"; struct _tagged_string _temp1594; _temp1594.curr=
_temp1593; _temp1594.base= _temp1593; _temp1594.last_plus_one= _temp1593 + 2;
_temp1594;})); struct Cyc_PP_Doc* _temp1592= Cyc_Absynpp_exp2doc_prec( myprec,
_temp1384); struct Cyc_PP_Doc* _temp1587[ 4u]={ _temp1589, _temp1590, _temp1591,
_temp1592}; struct _tagged_ptr1 _temp1588={ _temp1587, _temp1587, _temp1587 + 4u};
_temp1586( _temp1588);}); goto _LL1261; _LL1291: s=({ struct Cyc_PP_Doc*(*
_temp1597)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1600= Cyc_PP_text(({
unsigned char* _temp1602=( unsigned char*)"&"; struct _tagged_string _temp1603;
_temp1603.curr= _temp1602; _temp1603.base= _temp1602; _temp1603.last_plus_one=
_temp1602 + 2; _temp1603;})); struct Cyc_PP_Doc* _temp1601= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1388); struct Cyc_PP_Doc* _temp1598[ 2u]={ _temp1600, _temp1601};
struct _tagged_ptr1 _temp1599={ _temp1598, _temp1598, _temp1598 + 2u}; _temp1597(
_temp1599);}); goto _LL1261; _LL1293: if( _temp1392 == 0){ s=({ struct Cyc_PP_Doc*(*
_temp1604)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1607= Cyc_PP_text(({
unsigned char* _temp1609=( unsigned char*)"new "; struct _tagged_string
_temp1610; _temp1610.curr= _temp1609; _temp1610.base= _temp1609; _temp1610.last_plus_one=
_temp1609 + 5; _temp1610;})); struct Cyc_PP_Doc* _temp1608= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1390); struct Cyc_PP_Doc* _temp1605[ 2u]={ _temp1607, _temp1608};
struct _tagged_ptr1 _temp1606={ _temp1605, _temp1605, _temp1605 + 2u}; _temp1604(
_temp1606);});} else{ s=({ struct Cyc_PP_Doc*(* _temp1611)( struct _tagged_ptr1)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1614= Cyc_PP_text(({ unsigned char*
_temp1620=( unsigned char*)"rnew("; struct _tagged_string _temp1621; _temp1621.curr=
_temp1620; _temp1621.base= _temp1620; _temp1621.last_plus_one= _temp1620 + 6;
_temp1621;})); struct Cyc_PP_Doc* _temp1615= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)
_check_null( _temp1392)); struct Cyc_PP_Doc* _temp1616= Cyc_PP_text(({
unsigned char* _temp1618=( unsigned char*)") "; struct _tagged_string _temp1619;
_temp1619.curr= _temp1618; _temp1619.base= _temp1618; _temp1619.last_plus_one=
_temp1618 + 3; _temp1619;})); struct Cyc_PP_Doc* _temp1617= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1390); struct Cyc_PP_Doc* _temp1612[ 4u]={ _temp1614, _temp1615,
_temp1616, _temp1617}; struct _tagged_ptr1 _temp1613={ _temp1612, _temp1612,
_temp1612 + 4u}; _temp1611( _temp1613);});} goto _LL1261; _LL1295: s=({ struct
Cyc_PP_Doc*(* _temp1622)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1625= Cyc_PP_text(({ unsigned char* _temp1630=( unsigned char*)"sizeof(";
struct _tagged_string _temp1631; _temp1631.curr= _temp1630; _temp1631.base=
_temp1630; _temp1631.last_plus_one= _temp1630 + 8; _temp1631;})); struct Cyc_PP_Doc*
_temp1626= Cyc_Absynpp_typ2doc( _temp1394); struct Cyc_PP_Doc* _temp1627= Cyc_PP_text(({
unsigned char* _temp1628=( unsigned char*)")"; struct _tagged_string _temp1629;
_temp1629.curr= _temp1628; _temp1629.base= _temp1628; _temp1629.last_plus_one=
_temp1628 + 2; _temp1629;})); struct Cyc_PP_Doc* _temp1623[ 3u]={ _temp1625,
_temp1626, _temp1627}; struct _tagged_ptr1 _temp1624={ _temp1623, _temp1623,
_temp1623 + 3u}; _temp1622( _temp1624);}); goto _LL1261; _LL1297: s=({ struct
Cyc_PP_Doc*(* _temp1632)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1635= Cyc_PP_text(({ unsigned char* _temp1640=( unsigned char*)"sizeof(";
struct _tagged_string _temp1641; _temp1641.curr= _temp1640; _temp1641.base=
_temp1640; _temp1641.last_plus_one= _temp1640 + 8; _temp1641;})); struct Cyc_PP_Doc*
_temp1636= Cyc_Absynpp_exp2doc( _temp1396); struct Cyc_PP_Doc* _temp1637= Cyc_PP_text(({
unsigned char* _temp1638=( unsigned char*)")"; struct _tagged_string _temp1639;
_temp1639.curr= _temp1638; _temp1639.base= _temp1638; _temp1639.last_plus_one=
_temp1638 + 2; _temp1639;})); struct Cyc_PP_Doc* _temp1633[ 3u]={ _temp1635,
_temp1636, _temp1637}; struct _tagged_ptr1 _temp1634={ _temp1633, _temp1633,
_temp1633 + 3u}; _temp1632( _temp1634);}); goto _LL1261; _LL1299: s=({ struct
Cyc_PP_Doc*(* _temp1642)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1645= Cyc_PP_text(({ unsigned char* _temp1654=( unsigned char*)"offsetof(";
struct _tagged_string _temp1655; _temp1655.curr= _temp1654; _temp1655.base=
_temp1654; _temp1655.last_plus_one= _temp1654 + 10; _temp1655;})); struct Cyc_PP_Doc*
_temp1646= Cyc_Absynpp_typ2doc( _temp1400); struct Cyc_PP_Doc* _temp1647= Cyc_PP_text(({
unsigned char* _temp1652=( unsigned char*)","; struct _tagged_string _temp1653;
_temp1653.curr= _temp1652; _temp1653.base= _temp1652; _temp1653.last_plus_one=
_temp1652 + 2; _temp1653;})); struct Cyc_PP_Doc* _temp1648= Cyc_PP_textptr(
_temp1398); struct Cyc_PP_Doc* _temp1649= Cyc_PP_text(({ unsigned char*
_temp1650=( unsigned char*)")"; struct _tagged_string _temp1651; _temp1651.curr=
_temp1650; _temp1651.base= _temp1650; _temp1651.last_plus_one= _temp1650 + 2;
_temp1651;})); struct Cyc_PP_Doc* _temp1643[ 5u]={ _temp1645, _temp1646,
_temp1647, _temp1648, _temp1649}; struct _tagged_ptr1 _temp1644={ _temp1643,
_temp1643, _temp1643 + 5u}; _temp1642( _temp1644);}); goto _LL1261; _LL1301: s=({
struct Cyc_PP_Doc*(* _temp1656)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1659= Cyc_PP_text(({ unsigned char* _temp1661=( unsigned char*)"*"; struct
_tagged_string _temp1662; _temp1662.curr= _temp1661; _temp1662.base= _temp1661;
_temp1662.last_plus_one= _temp1661 + 2; _temp1662;})); struct Cyc_PP_Doc*
_temp1660= Cyc_Absynpp_exp2doc_prec( myprec, _temp1402); struct Cyc_PP_Doc*
_temp1657[ 2u]={ _temp1659, _temp1660}; struct _tagged_ptr1 _temp1658={
_temp1657, _temp1657, _temp1657 + 2u}; _temp1656( _temp1658);}); goto _LL1261;
_LL1303: s=({ struct Cyc_PP_Doc*(* _temp1663)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1666= Cyc_Absynpp_exp2doc_prec( myprec, _temp1406);
struct Cyc_PP_Doc* _temp1667= Cyc_PP_text(({ unsigned char* _temp1669=(
unsigned char*)"."; struct _tagged_string _temp1670; _temp1670.curr= _temp1669;
_temp1670.base= _temp1669; _temp1670.last_plus_one= _temp1669 + 2; _temp1670;}));
struct Cyc_PP_Doc* _temp1668= Cyc_PP_textptr( _temp1404); struct Cyc_PP_Doc*
_temp1664[ 3u]={ _temp1666, _temp1667, _temp1668}; struct _tagged_ptr1 _temp1665={
_temp1664, _temp1664, _temp1664 + 3u}; _temp1663( _temp1665);}); goto _LL1261;
_LL1305: s=({ struct Cyc_PP_Doc*(* _temp1671)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1674= Cyc_Absynpp_exp2doc_prec( myprec, _temp1410);
struct Cyc_PP_Doc* _temp1675= Cyc_PP_text(({ unsigned char* _temp1677=(
unsigned char*)"->"; struct _tagged_string _temp1678; _temp1678.curr= _temp1677;
_temp1678.base= _temp1677; _temp1678.last_plus_one= _temp1677 + 3; _temp1678;}));
struct Cyc_PP_Doc* _temp1676= Cyc_PP_textptr( _temp1408); struct Cyc_PP_Doc*
_temp1672[ 3u]={ _temp1674, _temp1675, _temp1676}; struct _tagged_ptr1 _temp1673={
_temp1672, _temp1672, _temp1672 + 3u}; _temp1671( _temp1673);}); goto _LL1261;
_LL1307: s=({ struct Cyc_PP_Doc*(* _temp1679)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1682= Cyc_Absynpp_exp2doc_prec( myprec, _temp1414);
struct Cyc_PP_Doc* _temp1683= Cyc_PP_text(({ unsigned char* _temp1688=(
unsigned char*)"["; struct _tagged_string _temp1689; _temp1689.curr= _temp1688;
_temp1689.base= _temp1688; _temp1689.last_plus_one= _temp1688 + 2; _temp1689;}));
struct Cyc_PP_Doc* _temp1684= Cyc_Absynpp_exp2doc( _temp1412); struct Cyc_PP_Doc*
_temp1685= Cyc_PP_text(({ unsigned char* _temp1686=( unsigned char*)"]"; struct
_tagged_string _temp1687; _temp1687.curr= _temp1686; _temp1687.base= _temp1686;
_temp1687.last_plus_one= _temp1686 + 2; _temp1687;})); struct Cyc_PP_Doc*
_temp1680[ 4u]={ _temp1682, _temp1683, _temp1684, _temp1685}; struct
_tagged_ptr1 _temp1681={ _temp1680, _temp1680, _temp1680 + 4u}; _temp1679(
_temp1681);}); goto _LL1261; _LL1309: s=({ struct Cyc_PP_Doc*(* _temp1690)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1693= Cyc_PP_text(({
unsigned char* _temp1698=( unsigned char*)"$("; struct _tagged_string _temp1699;
_temp1699.curr= _temp1698; _temp1699.base= _temp1698; _temp1699.last_plus_one=
_temp1698 + 3; _temp1699;})); struct Cyc_PP_Doc* _temp1694= Cyc_Absynpp_exps2doc_prec(
20, _temp1416); struct Cyc_PP_Doc* _temp1695= Cyc_PP_text(({ unsigned char*
_temp1696=( unsigned char*)")"; struct _tagged_string _temp1697; _temp1697.curr=
_temp1696; _temp1697.base= _temp1696; _temp1697.last_plus_one= _temp1696 + 2;
_temp1697;})); struct Cyc_PP_Doc* _temp1691[ 3u]={ _temp1693, _temp1694,
_temp1695}; struct _tagged_ptr1 _temp1692={ _temp1691, _temp1691, _temp1691 + 3u};
_temp1690( _temp1692);}); goto _LL1261; _LL1311: s=({ struct Cyc_PP_Doc*(*
_temp1700)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1703= Cyc_PP_text(({
unsigned char* _temp1715=( unsigned char*)"("; struct _tagged_string _temp1716;
_temp1716.curr= _temp1715; _temp1716.base= _temp1715; _temp1716.last_plus_one=
_temp1715 + 2; _temp1716;})); struct Cyc_PP_Doc* _temp1704= Cyc_Absynpp_typ2doc((*
_temp1420).f3); struct Cyc_PP_Doc* _temp1705= Cyc_PP_text(({ unsigned char*
_temp1713=( unsigned char*)")"; struct _tagged_string _temp1714; _temp1714.curr=
_temp1713; _temp1714.base= _temp1713; _temp1714.last_plus_one= _temp1713 + 2;
_temp1714;})); struct Cyc_PP_Doc* _temp1706= Cyc_PP_group(({ unsigned char*
_temp1707=( unsigned char*)"{"; struct _tagged_string _temp1708; _temp1708.curr=
_temp1707; _temp1708.base= _temp1707; _temp1708.last_plus_one= _temp1707 + 2;
_temp1708;}),({ unsigned char* _temp1709=( unsigned char*)"}"; struct
_tagged_string _temp1710; _temp1710.curr= _temp1709; _temp1710.base= _temp1709;
_temp1710.last_plus_one= _temp1709 + 2; _temp1710;}),({ unsigned char* _temp1711=(
unsigned char*)","; struct _tagged_string _temp1712; _temp1712.curr= _temp1711;
_temp1712.base= _temp1711; _temp1712.last_plus_one= _temp1711 + 2; _temp1712;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_de2doc, _temp1418)); struct Cyc_PP_Doc* _temp1701[
4u]={ _temp1703, _temp1704, _temp1705, _temp1706}; struct _tagged_ptr1 _temp1702={
_temp1701, _temp1701, _temp1701 + 4u}; _temp1700( _temp1702);}); goto _LL1261;
_LL1313: s= Cyc_PP_group(({ unsigned char* _temp1717=( unsigned char*)"{";
struct _tagged_string _temp1718; _temp1718.curr= _temp1717; _temp1718.base=
_temp1717; _temp1718.last_plus_one= _temp1717 + 2; _temp1718;}),({ unsigned char*
_temp1719=( unsigned char*)"}"; struct _tagged_string _temp1720; _temp1720.curr=
_temp1719; _temp1720.base= _temp1719; _temp1720.last_plus_one= _temp1719 + 2;
_temp1720;}),({ unsigned char* _temp1721=( unsigned char*)","; struct
_tagged_string _temp1722; _temp1722.curr= _temp1721; _temp1722.base= _temp1721;
_temp1722.last_plus_one= _temp1721 + 2; _temp1722;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp1422)); goto _LL1261; _LL1315: s=({ struct Cyc_PP_Doc*(*
_temp1723)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1726= Cyc_PP_text(({
unsigned char* _temp1739=( unsigned char*)"{for "; struct _tagged_string
_temp1740; _temp1740.curr= _temp1739; _temp1740.base= _temp1739; _temp1740.last_plus_one=
_temp1739 + 6; _temp1740;})); struct Cyc_PP_Doc* _temp1727= Cyc_PP_text(*(*
_temp1428->name).f2); struct Cyc_PP_Doc* _temp1728= Cyc_PP_text(({ unsigned char*
_temp1737=( unsigned char*)" < "; struct _tagged_string _temp1738; _temp1738.curr=
_temp1737; _temp1738.base= _temp1737; _temp1738.last_plus_one= _temp1737 + 4;
_temp1738;})); struct Cyc_PP_Doc* _temp1729= Cyc_Absynpp_exp2doc( _temp1426);
struct Cyc_PP_Doc* _temp1730= Cyc_PP_text(({ unsigned char* _temp1735=(
unsigned char*)" : "; struct _tagged_string _temp1736; _temp1736.curr= _temp1735;
_temp1736.base= _temp1735; _temp1736.last_plus_one= _temp1735 + 4; _temp1736;}));
struct Cyc_PP_Doc* _temp1731= Cyc_Absynpp_exp2doc( _temp1424); struct Cyc_PP_Doc*
_temp1732= Cyc_PP_text(({ unsigned char* _temp1733=( unsigned char*)"}"; struct
_tagged_string _temp1734; _temp1734.curr= _temp1733; _temp1734.base= _temp1733;
_temp1734.last_plus_one= _temp1733 + 2; _temp1734;})); struct Cyc_PP_Doc*
_temp1724[ 7u]={ _temp1726, _temp1727, _temp1728, _temp1729, _temp1730,
_temp1731, _temp1732}; struct _tagged_ptr1 _temp1725={ _temp1724, _temp1724,
_temp1724 + 7u}; _temp1723( _temp1725);}); goto _LL1261; _LL1317: s=({ struct
Cyc_PP_Doc*(* _temp1741)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1744= Cyc_Absynpp_qvar2doc( _temp1436); struct Cyc_PP_Doc* _temp1745= Cyc_PP_group(({
unsigned char* _temp1746=( unsigned char*)"{"; struct _tagged_string _temp1747;
_temp1747.curr= _temp1746; _temp1747.base= _temp1746; _temp1747.last_plus_one=
_temp1746 + 2; _temp1747;}),({ unsigned char* _temp1748=( unsigned char*)"}";
struct _tagged_string _temp1749; _temp1749.curr= _temp1748; _temp1749.base=
_temp1748; _temp1749.last_plus_one= _temp1748 + 2; _temp1749;}),({ unsigned char*
_temp1750=( unsigned char*)","; struct _tagged_string _temp1751; _temp1751.curr=
_temp1750; _temp1751.base= _temp1750; _temp1751.last_plus_one= _temp1750 + 2;
_temp1751;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1432)); struct Cyc_PP_Doc* _temp1742[ 2u]={ _temp1744, _temp1745}; struct
_tagged_ptr1 _temp1743={ _temp1742, _temp1742, _temp1742 + 2u}; _temp1741(
_temp1743);}); goto _LL1261; _LL1319: s= Cyc_PP_group(({ unsigned char*
_temp1752=( unsigned char*)"{"; struct _tagged_string _temp1753; _temp1753.curr=
_temp1752; _temp1753.base= _temp1752; _temp1753.last_plus_one= _temp1752 + 2;
_temp1753;}),({ unsigned char* _temp1754=( unsigned char*)"}"; struct
_tagged_string _temp1755; _temp1755.curr= _temp1754; _temp1755.base= _temp1754;
_temp1755.last_plus_one= _temp1754 + 2; _temp1755;}),({ unsigned char* _temp1756=(
unsigned char*)","; struct _tagged_string _temp1757; _temp1757.curr= _temp1756;
_temp1757.base= _temp1756; _temp1757.last_plus_one= _temp1756 + 2; _temp1757;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_de2doc, _temp1438)); goto _LL1261; _LL1321: if(
_temp1446 == 0){ s= Cyc_Absynpp_qvar2doc( _temp1442->name);} else{ s=({ struct
Cyc_PP_Doc*(* _temp1758)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1761= Cyc_Absynpp_qvar2doc( _temp1442->name); struct Cyc_PP_Doc* _temp1762=
Cyc_PP_egroup(({ unsigned char* _temp1763=( unsigned char*)"("; struct
_tagged_string _temp1764; _temp1764.curr= _temp1763; _temp1764.base= _temp1763;
_temp1764.last_plus_one= _temp1763 + 2; _temp1764;}),({ unsigned char* _temp1765=(
unsigned char*)")"; struct _tagged_string _temp1766; _temp1766.curr= _temp1765;
_temp1766.base= _temp1765; _temp1766.last_plus_one= _temp1765 + 2; _temp1766;}),({
unsigned char* _temp1767=( unsigned char*)","; struct _tagged_string _temp1768;
_temp1768.curr= _temp1767; _temp1768.base= _temp1767; _temp1768.last_plus_one=
_temp1767 + 2; _temp1768;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_exp2doc,
_temp1446)); struct Cyc_PP_Doc* _temp1759[ 2u]={ _temp1761, _temp1762}; struct
_tagged_ptr1 _temp1760={ _temp1759, _temp1759, _temp1759 + 2u}; _temp1758(
_temp1760);});} goto _LL1261; _LL1323: s= Cyc_Absynpp_qvar2doc( _temp1456); goto
_LL1261; _LL1325: if( _temp1460 == 0){ s=({ struct Cyc_PP_Doc*(* _temp1769)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1772= Cyc_PP_text(({
unsigned char* _temp1777=( unsigned char*)"malloc(sizeof("; struct
_tagged_string _temp1778; _temp1778.curr= _temp1777; _temp1778.base= _temp1777;
_temp1778.last_plus_one= _temp1777 + 15; _temp1778;})); struct Cyc_PP_Doc*
_temp1773= Cyc_Absynpp_typ2doc( _temp1458); struct Cyc_PP_Doc* _temp1774= Cyc_PP_text(({
unsigned char* _temp1775=( unsigned char*)"))"; struct _tagged_string _temp1776;
_temp1776.curr= _temp1775; _temp1776.base= _temp1775; _temp1776.last_plus_one=
_temp1775 + 3; _temp1776;})); struct Cyc_PP_Doc* _temp1770[ 3u]={ _temp1772,
_temp1773, _temp1774}; struct _tagged_ptr1 _temp1771={ _temp1770, _temp1770,
_temp1770 + 3u}; _temp1769( _temp1771);});} else{ s=({ struct Cyc_PP_Doc*(*
_temp1779)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1782= Cyc_PP_text(({
unsigned char* _temp1794=( unsigned char*)"rmalloc("; struct _tagged_string
_temp1795; _temp1795.curr= _temp1794; _temp1795.base= _temp1794; _temp1795.last_plus_one=
_temp1794 + 9; _temp1795;})); struct Cyc_PP_Doc* _temp1783= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp1460)); struct Cyc_PP_Doc* _temp1784=
Cyc_PP_text(({ unsigned char* _temp1792=( unsigned char*)","; struct
_tagged_string _temp1793; _temp1793.curr= _temp1792; _temp1793.base= _temp1792;
_temp1793.last_plus_one= _temp1792 + 2; _temp1793;})); struct Cyc_PP_Doc*
_temp1785= Cyc_PP_text(({ unsigned char* _temp1790=( unsigned char*)"sizeof(";
struct _tagged_string _temp1791; _temp1791.curr= _temp1790; _temp1791.base=
_temp1790; _temp1791.last_plus_one= _temp1790 + 8; _temp1791;})); struct Cyc_PP_Doc*
_temp1786= Cyc_Absynpp_typ2doc( _temp1458); struct Cyc_PP_Doc* _temp1787= Cyc_PP_text(({
unsigned char* _temp1788=( unsigned char*)"))"; struct _tagged_string _temp1789;
_temp1789.curr= _temp1788; _temp1789.base= _temp1788; _temp1789.last_plus_one=
_temp1788 + 3; _temp1789;})); struct Cyc_PP_Doc* _temp1780[ 6u]={ _temp1782,
_temp1783, _temp1784, _temp1785, _temp1786, _temp1787}; struct _tagged_ptr1
_temp1781={ _temp1780, _temp1780, _temp1780 + 6u}; _temp1779( _temp1781);});}
goto _LL1261; _LL1327: s= Cyc_PP_group(({ unsigned char* _temp1796=(
unsigned char*)"{"; struct _tagged_string _temp1797; _temp1797.curr= _temp1796;
_temp1797.base= _temp1796; _temp1797.last_plus_one= _temp1796 + 2; _temp1797;}),({
unsigned char* _temp1798=( unsigned char*)"}"; struct _tagged_string _temp1799;
_temp1799.curr= _temp1798; _temp1799.base= _temp1798; _temp1799.last_plus_one=
_temp1798 + 2; _temp1799;}),({ unsigned char* _temp1800=( unsigned char*)",";
struct _tagged_string _temp1801; _temp1801.curr= _temp1800; _temp1801.base=
_temp1800; _temp1801.last_plus_one= _temp1800 + 2; _temp1801;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp1462)); goto _LL1261; _LL1329: s=({ struct Cyc_PP_Doc*(*
_temp1802)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1805= Cyc_PP_text(({
unsigned char* _temp1810=( unsigned char*)"({ "; struct _tagged_string _temp1811;
_temp1811.curr= _temp1810; _temp1811.base= _temp1810; _temp1811.last_plus_one=
_temp1810 + 4; _temp1811;})); struct Cyc_PP_Doc* _temp1806= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1466)); struct Cyc_PP_Doc* _temp1807= Cyc_PP_text(({ unsigned char*
_temp1808=( unsigned char*)" })"; struct _tagged_string _temp1809; _temp1809.curr=
_temp1808; _temp1809.base= _temp1808; _temp1809.last_plus_one= _temp1808 + 4;
_temp1809;})); struct Cyc_PP_Doc* _temp1803[ 3u]={ _temp1805, _temp1806,
_temp1807}; struct _tagged_ptr1 _temp1804={ _temp1803, _temp1803, _temp1803 + 3u};
_temp1802( _temp1804);}); goto _LL1261; _LL1331: s=({ struct Cyc_PP_Doc*(*
_temp1812)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1815= Cyc_PP_text(({
unsigned char* _temp1823=( unsigned char*)"codegen("; struct _tagged_string
_temp1824; _temp1824.curr= _temp1823; _temp1824.base= _temp1823; _temp1824.last_plus_one=
_temp1823 + 9; _temp1824;})); struct Cyc_PP_Doc* _temp1816= Cyc_PP_nest( 2, Cyc_Absynpp_decl2doc(({
struct Cyc_Absyn_Decl* _temp1820=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof(
struct Cyc_Absyn_Decl)); _temp1820->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp1821=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1821[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp1822; _temp1822.tag= Cyc_Absyn_Fn_d;
_temp1822.f1= _temp1468; _temp1822;}); _temp1821;})); _temp1820->loc= e->loc;
_temp1820;}))); struct Cyc_PP_Doc* _temp1817= Cyc_PP_text(({ unsigned char*
_temp1818=( unsigned char*)")"; struct _tagged_string _temp1819; _temp1819.curr=
_temp1818; _temp1819.base= _temp1818; _temp1819.last_plus_one= _temp1818 + 2;
_temp1819;})); struct Cyc_PP_Doc* _temp1813[ 3u]={ _temp1815, _temp1816,
_temp1817}; struct _tagged_ptr1 _temp1814={ _temp1813, _temp1813, _temp1813 + 3u};
_temp1812( _temp1814);}); goto _LL1261; _LL1333: s=({ struct Cyc_PP_Doc*(*
_temp1825)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1828= Cyc_PP_text(({
unsigned char* _temp1833=( unsigned char*)"fill("; struct _tagged_string
_temp1834; _temp1834.curr= _temp1833; _temp1834.base= _temp1833; _temp1834.last_plus_one=
_temp1833 + 6; _temp1834;})); struct Cyc_PP_Doc* _temp1829= Cyc_PP_nest( 2, Cyc_Absynpp_exp2doc(
_temp1470)); struct Cyc_PP_Doc* _temp1830= Cyc_PP_text(({ unsigned char*
_temp1831=( unsigned char*)")"; struct _tagged_string _temp1832; _temp1832.curr=
_temp1831; _temp1832.base= _temp1831; _temp1832.last_plus_one= _temp1831 + 2;
_temp1832;})); struct Cyc_PP_Doc* _temp1826[ 3u]={ _temp1828, _temp1829,
_temp1830}; struct _tagged_ptr1 _temp1827={ _temp1826, _temp1826, _temp1826 + 3u};
_temp1825( _temp1827);}); goto _LL1261; _LL1261:;} if( inprec >= myprec){ s=({
struct Cyc_PP_Doc*(* _temp1835)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1838= Cyc_PP_text(({ unsigned char* _temp1843=( unsigned char*)"("; struct
_tagged_string _temp1844; _temp1844.curr= _temp1843; _temp1844.base= _temp1843;
_temp1844.last_plus_one= _temp1843 + 2; _temp1844;})); struct Cyc_PP_Doc*
_temp1839= s; struct Cyc_PP_Doc* _temp1840= Cyc_PP_text(({ unsigned char*
_temp1841=( unsigned char*)")"; struct _tagged_string _temp1842; _temp1842.curr=
_temp1841; _temp1842.base= _temp1841; _temp1842.last_plus_one= _temp1841 + 2;
_temp1842;})); struct Cyc_PP_Doc* _temp1836[ 3u]={ _temp1838, _temp1839,
_temp1840}; struct _tagged_ptr1 _temp1837={ _temp1836, _temp1836, _temp1836 + 3u};
_temp1835( _temp1837);});} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc(
void* d){ void* _temp1845= d; struct Cyc_Absyn_Exp* _temp1851; struct
_tagged_string* _temp1853; _LL1847: if(*(( int*) _temp1845) == Cyc_Absyn_ArrayElement){
_LL1852: _temp1851=(( struct Cyc_Absyn_ArrayElement_struct*) _temp1845)->f1;
goto _LL1848;} else{ goto _LL1849;} _LL1849: if(*(( int*) _temp1845) == Cyc_Absyn_FieldName){
_LL1854: _temp1853=(( struct Cyc_Absyn_FieldName_struct*) _temp1845)->f1; goto
_LL1850;} else{ goto _LL1846;} _LL1848: return({ struct Cyc_PP_Doc*(* _temp1855)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1858= Cyc_PP_text(({
unsigned char* _temp1863=( unsigned char*)".["; struct _tagged_string _temp1864;
_temp1864.curr= _temp1863; _temp1864.base= _temp1863; _temp1864.last_plus_one=
_temp1863 + 3; _temp1864;})); struct Cyc_PP_Doc* _temp1859= Cyc_Absynpp_exp2doc(
_temp1851); struct Cyc_PP_Doc* _temp1860= Cyc_PP_text(({ unsigned char*
_temp1861=( unsigned char*)"]"; struct _tagged_string _temp1862; _temp1862.curr=
_temp1861; _temp1862.base= _temp1861; _temp1862.last_plus_one= _temp1861 + 2;
_temp1862;})); struct Cyc_PP_Doc* _temp1856[ 3u]={ _temp1858, _temp1859,
_temp1860}; struct _tagged_ptr1 _temp1857={ _temp1856, _temp1856, _temp1856 + 3u};
_temp1855( _temp1857);}); _LL1850: return({ struct Cyc_PP_Doc*(* _temp1865)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1868= Cyc_PP_text(({
unsigned char* _temp1870=( unsigned char*)"."; struct _tagged_string _temp1871;
_temp1871.curr= _temp1870; _temp1871.base= _temp1870; _temp1871.last_plus_one=
_temp1870 + 2; _temp1871;})); struct Cyc_PP_Doc* _temp1869= Cyc_PP_textptr(
_temp1853); struct Cyc_PP_Doc* _temp1866[ 2u]={ _temp1868, _temp1869}; struct
_tagged_ptr1 _temp1867={ _temp1866, _temp1866, _temp1866 + 2u}; _temp1865(
_temp1867);}); _LL1846:;} struct Cyc_PP_Doc* Cyc_Absynpp_de2doc( struct _tuple6*
de){ if((* de).f1 == 0){ return Cyc_Absynpp_exp2doc((* de).f2);} else{ return({
struct Cyc_PP_Doc*(* _temp1872)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1875= Cyc_PP_egroup(({ unsigned char* _temp1877=( unsigned char*)""; struct
_tagged_string _temp1878; _temp1878.curr= _temp1877; _temp1878.base= _temp1877;
_temp1878.last_plus_one= _temp1877 + 1; _temp1878;}),({ unsigned char* _temp1879=(
unsigned char*)"="; struct _tagged_string _temp1880; _temp1880.curr= _temp1879;
_temp1880.base= _temp1879; _temp1880.last_plus_one= _temp1879 + 2; _temp1880;}),({
unsigned char* _temp1881=( unsigned char*)"="; struct _tagged_string _temp1882;
_temp1882.curr= _temp1881; _temp1882.base= _temp1881; _temp1882.last_plus_one=
_temp1881 + 2; _temp1882;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
de).f1)); struct Cyc_PP_Doc* _temp1876= Cyc_Absynpp_exp2doc((* de).f2); struct
Cyc_PP_Doc* _temp1873[ 2u]={ _temp1875, _temp1876}; struct _tagged_ptr1
_temp1874={ _temp1873, _temp1873, _temp1873 + 2u}; _temp1872( _temp1874);});}}
struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec( int inprec, struct Cyc_List_List*
es){ return Cyc_PP_group(({ unsigned char* _temp1883=( unsigned char*)""; struct
_tagged_string _temp1884; _temp1884.curr= _temp1883; _temp1884.base= _temp1883;
_temp1884.last_plus_one= _temp1883 + 1; _temp1884;}),({ unsigned char* _temp1885=(
unsigned char*)""; struct _tagged_string _temp1886; _temp1886.curr= _temp1885;
_temp1886.base= _temp1885; _temp1886.last_plus_one= _temp1885 + 1; _temp1886;}),({
unsigned char* _temp1887=( unsigned char*)","; struct _tagged_string _temp1888;
_temp1888.curr= _temp1887; _temp1888.base= _temp1887; _temp1888.last_plus_one=
_temp1887 + 2; _temp1888;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Absynpp_exp2doc_prec, inprec, es));} struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc(
void* c){ void* _temp1889= c; unsigned char _temp1907; void* _temp1909; short
_temp1911; void* _temp1913; int _temp1915; void* _temp1917; int _temp1919; void*
_temp1921; long long _temp1923; void* _temp1925; struct _tagged_string _temp1927;
struct _tagged_string _temp1929; _LL1891: if(( unsigned int) _temp1889 > 1u?*((
int*) _temp1889) == Cyc_Absyn_Char_c: 0){ _LL1910: _temp1909=( void*)(( struct
Cyc_Absyn_Char_c_struct*) _temp1889)->f1; goto _LL1908; _LL1908: _temp1907=((
struct Cyc_Absyn_Char_c_struct*) _temp1889)->f2; goto _LL1892;} else{ goto
_LL1893;} _LL1893: if(( unsigned int) _temp1889 > 1u?*(( int*) _temp1889) == Cyc_Absyn_Short_c:
0){ _LL1914: _temp1913=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp1889)->f1;
goto _LL1912; _LL1912: _temp1911=(( struct Cyc_Absyn_Short_c_struct*) _temp1889)->f2;
goto _LL1894;} else{ goto _LL1895;} _LL1895: if(( unsigned int) _temp1889 > 1u?*((
int*) _temp1889) == Cyc_Absyn_Int_c: 0){ _LL1918: _temp1917=( void*)(( struct
Cyc_Absyn_Int_c_struct*) _temp1889)->f1; if( _temp1917 ==( void*) Cyc_Absyn_Signed){
goto _LL1916;} else{ goto _LL1897;} _LL1916: _temp1915=(( struct Cyc_Absyn_Int_c_struct*)
_temp1889)->f2; goto _LL1896;} else{ goto _LL1897;} _LL1897: if(( unsigned int)
_temp1889 > 1u?*(( int*) _temp1889) == Cyc_Absyn_Int_c: 0){ _LL1922: _temp1921=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1889)->f1; if( _temp1921 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL1920;} else{ goto _LL1899;} _LL1920: _temp1919=((
struct Cyc_Absyn_Int_c_struct*) _temp1889)->f2; goto _LL1898;} else{ goto
_LL1899;} _LL1899: if(( unsigned int) _temp1889 > 1u?*(( int*) _temp1889) == Cyc_Absyn_LongLong_c:
0){ _LL1926: _temp1925=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp1889)->f1;
goto _LL1924; _LL1924: _temp1923=(( struct Cyc_Absyn_LongLong_c_struct*)
_temp1889)->f2; goto _LL1900;} else{ goto _LL1901;} _LL1901: if(( unsigned int)
_temp1889 > 1u?*(( int*) _temp1889) == Cyc_Absyn_Float_c: 0){ _LL1928: _temp1927=((
struct Cyc_Absyn_Float_c_struct*) _temp1889)->f1; goto _LL1902;} else{ goto
_LL1903;} _LL1903: if( _temp1889 ==( void*) Cyc_Absyn_Null_c){ goto _LL1904;}
else{ goto _LL1905;} _LL1905: if(( unsigned int) _temp1889 > 1u?*(( int*)
_temp1889) == Cyc_Absyn_String_c: 0){ _LL1930: _temp1929=(( struct Cyc_Absyn_String_c_struct*)
_temp1889)->f1; goto _LL1906;} else{ goto _LL1890;} _LL1892: return Cyc_PP_text((
struct _tagged_string)({ struct _tagged_string _temp1931= Cyc_Absynpp_char_escape(
_temp1907); xprintf("'%.*s'", _temp1931.last_plus_one - _temp1931.curr,
_temp1931.curr);})); _LL1894: return Cyc_PP_text(( struct _tagged_string)
xprintf("%d",( int) _temp1911)); _LL1896: return Cyc_PP_text(( struct
_tagged_string) Cyc_Core_string_of_int( _temp1915)); _LL1898: return Cyc_PP_text((
struct _tagged_string) xprintf("%u",( unsigned int) _temp1919)); _LL1900: return
Cyc_PP_text(({ unsigned char* _temp1932=( unsigned char*)"<<FIX LONG LONG CONSTANT>>";
struct _tagged_string _temp1933; _temp1933.curr= _temp1932; _temp1933.base=
_temp1932; _temp1933.last_plus_one= _temp1932 + 27; _temp1933;})); _LL1902:
return Cyc_PP_text( _temp1927); _LL1904: return Cyc_PP_text(({ unsigned char*
_temp1934=( unsigned char*)"null"; struct _tagged_string _temp1935; _temp1935.curr=
_temp1934; _temp1935.base= _temp1934; _temp1935.last_plus_one= _temp1934 + 5;
_temp1935;})); _LL1906: return({ struct Cyc_PP_Doc*(* _temp1936)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1939= Cyc_PP_text(({
unsigned char* _temp1944=( unsigned char*)"\""; struct _tagged_string _temp1945;
_temp1945.curr= _temp1944; _temp1945.base= _temp1944; _temp1945.last_plus_one=
_temp1944 + 2; _temp1945;})); struct Cyc_PP_Doc* _temp1940= Cyc_PP_text( Cyc_Absynpp_string_escape(
_temp1929)); struct Cyc_PP_Doc* _temp1941= Cyc_PP_text(({ unsigned char*
_temp1942=( unsigned char*)"\""; struct _tagged_string _temp1943; _temp1943.curr=
_temp1942; _temp1943.base= _temp1942; _temp1943.last_plus_one= _temp1942 + 2;
_temp1943;})); struct Cyc_PP_Doc* _temp1937[ 3u]={ _temp1939, _temp1940,
_temp1941}; struct _tagged_ptr1 _temp1938={ _temp1937, _temp1937, _temp1937 + 3u};
_temp1936( _temp1938);}); _LL1890:;} struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc(
int inprec, void* p, struct Cyc_List_List* es){ struct Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc(
p); if( p ==( void*) Cyc_Absyn_Size){ if( es == 0? 1:(( struct Cyc_List_List*)
_check_null( es))->tl != 0){( void) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp1946=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1946[ 0]=({ struct Cyc_Core_Failure_struct _temp1947; _temp1947.tag= Cyc_Core_Failure;
_temp1947.f1=( struct _tagged_string)({ struct _tagged_string _temp1948= Cyc_PP_string_of_doc(
ps, 72); xprintf("Absynpp::primapp2doc Size: %.*s with bad args", _temp1948.last_plus_one
- _temp1948.curr, _temp1948.curr);}); _temp1947;}); _temp1946;}));}{ struct Cyc_PP_Doc*
_temp1949= Cyc_Absynpp_exp2doc_prec( inprec,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd); goto _LL1950; _LL1950: return({ struct
Cyc_PP_Doc*(* _temp1951)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1954= _temp1949; struct Cyc_PP_Doc* _temp1955= Cyc_PP_text(({ unsigned char*
_temp1956=( unsigned char*)".size"; struct _tagged_string _temp1957; _temp1957.curr=
_temp1956; _temp1957.base= _temp1956; _temp1957.last_plus_one= _temp1956 + 6;
_temp1957;})); struct Cyc_PP_Doc* _temp1952[ 2u]={ _temp1954, _temp1955}; struct
_tagged_ptr1 _temp1953={ _temp1952, _temp1952, _temp1952 + 2u}; _temp1951(
_temp1953);});}} else{ if( ! Cyc_Absyn_is_format_prim( p)){ struct Cyc_List_List*
ds=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*),
int env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec,
inprec, es); if( ds == 0){( void) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp1958=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1958[ 0]=({ struct Cyc_Core_Failure_struct _temp1959; _temp1959.tag= Cyc_Core_Failure;
_temp1959.f1=( struct _tagged_string)({ struct _tagged_string _temp1960= Cyc_PP_string_of_doc(
ps, 72); xprintf("Absynpp::primapp2doc: %.*s with no args", _temp1960.last_plus_one
- _temp1960.curr, _temp1960.curr);}); _temp1959;}); _temp1958;}));} else{ if(((
struct Cyc_List_List*) _check_null( ds))->tl == 0){ return({ struct Cyc_PP_Doc*(*
_temp1961)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1964= ps;
struct Cyc_PP_Doc* _temp1965= Cyc_PP_text(({ unsigned char* _temp1967=(
unsigned char*)" "; struct _tagged_string _temp1968; _temp1968.curr= _temp1967;
_temp1968.base= _temp1967; _temp1968.last_plus_one= _temp1967 + 2; _temp1968;}));
struct Cyc_PP_Doc* _temp1966=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*)
_check_null( ds))->hd; struct Cyc_PP_Doc* _temp1962[ 3u]={ _temp1964, _temp1965,
_temp1966}; struct _tagged_ptr1 _temp1963={ _temp1962, _temp1962, _temp1962 + 3u};
_temp1961( _temp1963);});} else{ if((( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( ds))->tl))->tl != 0){( void) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp1969=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1969[ 0]=({ struct Cyc_Core_Failure_struct
_temp1970; _temp1970.tag= Cyc_Core_Failure; _temp1970.f1=( struct _tagged_string)({
struct _tagged_string _temp1971= Cyc_PP_string_of_doc( ps, 72); xprintf("Absynpp::primapp2doc: %.*s with more than 2 args",
_temp1971.last_plus_one - _temp1971.curr, _temp1971.curr);}); _temp1970;});
_temp1969;}));} else{ return({ struct Cyc_PP_Doc*(* _temp1972)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1975=( struct Cyc_PP_Doc*)((
struct Cyc_List_List*) _check_null( ds))->hd; struct Cyc_PP_Doc* _temp1976= Cyc_PP_text(({
unsigned char* _temp1982=( unsigned char*)" "; struct _tagged_string _temp1983;
_temp1983.curr= _temp1982; _temp1983.base= _temp1982; _temp1983.last_plus_one=
_temp1982 + 2; _temp1983;})); struct Cyc_PP_Doc* _temp1977= ps; struct Cyc_PP_Doc*
_temp1978= Cyc_PP_text(({ unsigned char* _temp1980=( unsigned char*)" "; struct
_tagged_string _temp1981; _temp1981.curr= _temp1980; _temp1981.base= _temp1980;
_temp1981.last_plus_one= _temp1980 + 2; _temp1981;})); struct Cyc_PP_Doc*
_temp1979=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null((( struct
Cyc_List_List*) _check_null( ds))->tl))->hd; struct Cyc_PP_Doc* _temp1973[ 5u]={
_temp1975, _temp1976, _temp1977, _temp1978, _temp1979}; struct _tagged_ptr1
_temp1974={ _temp1973, _temp1973, _temp1973 + 5u}; _temp1972( _temp1974);});}}}}
else{ return({ struct Cyc_PP_Doc*(* _temp1984)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1987= ps; struct Cyc_PP_Doc* _temp1988= Cyc_PP_text(({
unsigned char* _temp1993=( unsigned char*)"("; struct _tagged_string _temp1994;
_temp1994.curr= _temp1993; _temp1994.base= _temp1993; _temp1994.last_plus_one=
_temp1993 + 2; _temp1994;})); struct Cyc_PP_Doc* _temp1989= Cyc_Absynpp_exps2doc_prec(
20, es); struct Cyc_PP_Doc* _temp1990= Cyc_PP_text(({ unsigned char* _temp1991=(
unsigned char*)")"; struct _tagged_string _temp1992; _temp1992.curr= _temp1991;
_temp1992.base= _temp1991; _temp1992.last_plus_one= _temp1991 + 2; _temp1992;}));
struct Cyc_PP_Doc* _temp1985[ 4u]={ _temp1987, _temp1988, _temp1989, _temp1990};
struct _tagged_ptr1 _temp1986={ _temp1985, _temp1985, _temp1985 + 4u}; _temp1984(
_temp1986);});}}} struct _tagged_string Cyc_Absynpp_prim2str( void* p){ void*
_temp1995= p; _LL1997: if( _temp1995 ==( void*) Cyc_Absyn_Plus){ goto _LL1998;}
else{ goto _LL1999;} _LL1999: if( _temp1995 ==( void*) Cyc_Absyn_Times){ goto
_LL2000;} else{ goto _LL2001;} _LL2001: if( _temp1995 ==( void*) Cyc_Absyn_Minus){
goto _LL2002;} else{ goto _LL2003;} _LL2003: if( _temp1995 ==( void*) Cyc_Absyn_Div){
goto _LL2004;} else{ goto _LL2005;} _LL2005: if( _temp1995 ==( void*) Cyc_Absyn_Mod){
goto _LL2006;} else{ goto _LL2007;} _LL2007: if( _temp1995 ==( void*) Cyc_Absyn_Eq){
goto _LL2008;} else{ goto _LL2009;} _LL2009: if( _temp1995 ==( void*) Cyc_Absyn_Neq){
goto _LL2010;} else{ goto _LL2011;} _LL2011: if( _temp1995 ==( void*) Cyc_Absyn_Gt){
goto _LL2012;} else{ goto _LL2013;} _LL2013: if( _temp1995 ==( void*) Cyc_Absyn_Lt){
goto _LL2014;} else{ goto _LL2015;} _LL2015: if( _temp1995 ==( void*) Cyc_Absyn_Gte){
goto _LL2016;} else{ goto _LL2017;} _LL2017: if( _temp1995 ==( void*) Cyc_Absyn_Lte){
goto _LL2018;} else{ goto _LL2019;} _LL2019: if( _temp1995 ==( void*) Cyc_Absyn_Not){
goto _LL2020;} else{ goto _LL2021;} _LL2021: if( _temp1995 ==( void*) Cyc_Absyn_Bitnot){
goto _LL2022;} else{ goto _LL2023;} _LL2023: if( _temp1995 ==( void*) Cyc_Absyn_Bitand){
goto _LL2024;} else{ goto _LL2025;} _LL2025: if( _temp1995 ==( void*) Cyc_Absyn_Bitor){
goto _LL2026;} else{ goto _LL2027;} _LL2027: if( _temp1995 ==( void*) Cyc_Absyn_Bitxor){
goto _LL2028;} else{ goto _LL2029;} _LL2029: if( _temp1995 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL2030;} else{ goto _LL2031;} _LL2031: if( _temp1995 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL2032;} else{ goto _LL2033;} _LL2033: if( _temp1995 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL2034;} else{ goto _LL2035;} _LL2035: if( _temp1995 ==( void*) Cyc_Absyn_Size){
goto _LL2036;} else{ goto _LL2037;} _LL2037: if( _temp1995 ==( void*) Cyc_Absyn_Printf){
goto _LL2038;} else{ goto _LL2039;} _LL2039: if( _temp1995 ==( void*) Cyc_Absyn_Fprintf){
goto _LL2040;} else{ goto _LL2041;} _LL2041: if( _temp1995 ==( void*) Cyc_Absyn_Xprintf){
goto _LL2042;} else{ goto _LL2043;} _LL2043: if( _temp1995 ==( void*) Cyc_Absyn_Scanf){
goto _LL2044;} else{ goto _LL2045;} _LL2045: if( _temp1995 ==( void*) Cyc_Absyn_Fscanf){
goto _LL2046;} else{ goto _LL2047;} _LL2047: if( _temp1995 ==( void*) Cyc_Absyn_Sscanf){
goto _LL2048;} else{ goto _LL1996;} _LL1998: return({ unsigned char* _temp2049=(
unsigned char*)"+"; struct _tagged_string _temp2050; _temp2050.curr= _temp2049;
_temp2050.base= _temp2049; _temp2050.last_plus_one= _temp2049 + 2; _temp2050;});
_LL2000: return({ unsigned char* _temp2051=( unsigned char*)"*"; struct
_tagged_string _temp2052; _temp2052.curr= _temp2051; _temp2052.base= _temp2051;
_temp2052.last_plus_one= _temp2051 + 2; _temp2052;}); _LL2002: return({
unsigned char* _temp2053=( unsigned char*)"-"; struct _tagged_string _temp2054;
_temp2054.curr= _temp2053; _temp2054.base= _temp2053; _temp2054.last_plus_one=
_temp2053 + 2; _temp2054;}); _LL2004: return({ unsigned char* _temp2055=(
unsigned char*)"/"; struct _tagged_string _temp2056; _temp2056.curr= _temp2055;
_temp2056.base= _temp2055; _temp2056.last_plus_one= _temp2055 + 2; _temp2056;});
_LL2006: return({ unsigned char* _temp2057=( unsigned char*)"%"; struct
_tagged_string _temp2058; _temp2058.curr= _temp2057; _temp2058.base= _temp2057;
_temp2058.last_plus_one= _temp2057 + 2; _temp2058;}); _LL2008: return({
unsigned char* _temp2059=( unsigned char*)"=="; struct _tagged_string _temp2060;
_temp2060.curr= _temp2059; _temp2060.base= _temp2059; _temp2060.last_plus_one=
_temp2059 + 3; _temp2060;}); _LL2010: return({ unsigned char* _temp2061=(
unsigned char*)"!="; struct _tagged_string _temp2062; _temp2062.curr= _temp2061;
_temp2062.base= _temp2061; _temp2062.last_plus_one= _temp2061 + 3; _temp2062;});
_LL2012: return({ unsigned char* _temp2063=( unsigned char*)">"; struct
_tagged_string _temp2064; _temp2064.curr= _temp2063; _temp2064.base= _temp2063;
_temp2064.last_plus_one= _temp2063 + 2; _temp2064;}); _LL2014: return({
unsigned char* _temp2065=( unsigned char*)"<"; struct _tagged_string _temp2066;
_temp2066.curr= _temp2065; _temp2066.base= _temp2065; _temp2066.last_plus_one=
_temp2065 + 2; _temp2066;}); _LL2016: return({ unsigned char* _temp2067=(
unsigned char*)">="; struct _tagged_string _temp2068; _temp2068.curr= _temp2067;
_temp2068.base= _temp2067; _temp2068.last_plus_one= _temp2067 + 3; _temp2068;});
_LL2018: return({ unsigned char* _temp2069=( unsigned char*)"<="; struct
_tagged_string _temp2070; _temp2070.curr= _temp2069; _temp2070.base= _temp2069;
_temp2070.last_plus_one= _temp2069 + 3; _temp2070;}); _LL2020: return({
unsigned char* _temp2071=( unsigned char*)"!"; struct _tagged_string _temp2072;
_temp2072.curr= _temp2071; _temp2072.base= _temp2071; _temp2072.last_plus_one=
_temp2071 + 2; _temp2072;}); _LL2022: return({ unsigned char* _temp2073=(
unsigned char*)"~"; struct _tagged_string _temp2074; _temp2074.curr= _temp2073;
_temp2074.base= _temp2073; _temp2074.last_plus_one= _temp2073 + 2; _temp2074;});
_LL2024: return({ unsigned char* _temp2075=( unsigned char*)"&"; struct
_tagged_string _temp2076; _temp2076.curr= _temp2075; _temp2076.base= _temp2075;
_temp2076.last_plus_one= _temp2075 + 2; _temp2076;}); _LL2026: return({
unsigned char* _temp2077=( unsigned char*)"|"; struct _tagged_string _temp2078;
_temp2078.curr= _temp2077; _temp2078.base= _temp2077; _temp2078.last_plus_one=
_temp2077 + 2; _temp2078;}); _LL2028: return({ unsigned char* _temp2079=(
unsigned char*)"^"; struct _tagged_string _temp2080; _temp2080.curr= _temp2079;
_temp2080.base= _temp2079; _temp2080.last_plus_one= _temp2079 + 2; _temp2080;});
_LL2030: return({ unsigned char* _temp2081=( unsigned char*)"<<"; struct
_tagged_string _temp2082; _temp2082.curr= _temp2081; _temp2082.base= _temp2081;
_temp2082.last_plus_one= _temp2081 + 3; _temp2082;}); _LL2032: return({
unsigned char* _temp2083=( unsigned char*)">>"; struct _tagged_string _temp2084;
_temp2084.curr= _temp2083; _temp2084.base= _temp2083; _temp2084.last_plus_one=
_temp2083 + 3; _temp2084;}); _LL2034: return({ unsigned char* _temp2085=(
unsigned char*)">>>"; struct _tagged_string _temp2086; _temp2086.curr= _temp2085;
_temp2086.base= _temp2085; _temp2086.last_plus_one= _temp2085 + 4; _temp2086;});
_LL2036: return({ unsigned char* _temp2087=( unsigned char*)"size"; struct
_tagged_string _temp2088; _temp2088.curr= _temp2087; _temp2088.base= _temp2087;
_temp2088.last_plus_one= _temp2087 + 5; _temp2088;}); _LL2038: return({
unsigned char* _temp2089=( unsigned char*)"printf"; struct _tagged_string
_temp2090; _temp2090.curr= _temp2089; _temp2090.base= _temp2089; _temp2090.last_plus_one=
_temp2089 + 7; _temp2090;}); _LL2040: return({ unsigned char* _temp2091=(
unsigned char*)"fprintf"; struct _tagged_string _temp2092; _temp2092.curr=
_temp2091; _temp2092.base= _temp2091; _temp2092.last_plus_one= _temp2091 + 8;
_temp2092;}); _LL2042: return({ unsigned char* _temp2093=( unsigned char*)"xprintf";
struct _tagged_string _temp2094; _temp2094.curr= _temp2093; _temp2094.base=
_temp2093; _temp2094.last_plus_one= _temp2093 + 8; _temp2094;}); _LL2044: return({
unsigned char* _temp2095=( unsigned char*)"scanf"; struct _tagged_string
_temp2096; _temp2096.curr= _temp2095; _temp2096.base= _temp2095; _temp2096.last_plus_one=
_temp2095 + 6; _temp2096;}); _LL2046: return({ unsigned char* _temp2097=(
unsigned char*)"fscanf"; struct _tagged_string _temp2098; _temp2098.curr=
_temp2097; _temp2098.base= _temp2097; _temp2098.last_plus_one= _temp2097 + 7;
_temp2098;}); _LL2048: return({ unsigned char* _temp2099=( unsigned char*)"sscanf";
struct _tagged_string _temp2100; _temp2100.curr= _temp2099; _temp2100.base=
_temp2099; _temp2100.last_plus_one= _temp2099 + 7; _temp2100;}); _LL1996:;}
struct Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void* p){ return Cyc_PP_text( Cyc_Absynpp_prim2str(
p));} struct _tuple9{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ;
struct Cyc_PP_Doc* Cyc_Absynpp_structarg2doc( struct _tuple9* t){ if((* t).f1 ==
0){ return Cyc_Absynpp_exp2doc((* t).f2);} else{ return({ struct Cyc_PP_Doc*(*
_temp2101)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2104= Cyc_PP_textptr((
struct _tagged_string*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v);
struct Cyc_PP_Doc* _temp2105= Cyc_PP_text(({ unsigned char* _temp2107=(
unsigned char*)" = "; struct _tagged_string _temp2108; _temp2108.curr= _temp2107;
_temp2108.base= _temp2107; _temp2108.last_plus_one= _temp2107 + 4; _temp2108;}));
struct Cyc_PP_Doc* _temp2106= Cyc_Absynpp_exp2doc((* t).f2); struct Cyc_PP_Doc*
_temp2102[ 3u]={ _temp2104, _temp2105, _temp2106}; struct _tagged_ptr1 _temp2103={
_temp2102, _temp2102, _temp2102 + 3u}; _temp2101( _temp2103);});}} int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt* s){ void* _temp2109=( void*) s->r; struct Cyc_Absyn_Stmt*
_temp2115; struct Cyc_Absyn_Decl* _temp2117; _LL2111: if(( unsigned int)
_temp2109 > 1u?*(( int*) _temp2109) == Cyc_Absyn_Decl_s: 0){ _LL2118: _temp2117=((
struct Cyc_Absyn_Decl_s_struct*) _temp2109)->f1; goto _LL2116; _LL2116:
_temp2115=(( struct Cyc_Absyn_Decl_s_struct*) _temp2109)->f2; goto _LL2112;}
else{ goto _LL2113;} _LL2113: goto _LL2114; _LL2112: return 1; _LL2114: return 0;
_LL2110:;} struct Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct Cyc_Absyn_Stmt* st){
struct Cyc_PP_Doc* s;{ void* _temp2119=( void*) st->r; struct Cyc_Absyn_Exp*
_temp2163; struct Cyc_Absyn_Stmt* _temp2165; struct Cyc_Absyn_Stmt* _temp2167;
struct Cyc_Absyn_Exp* _temp2169; struct Cyc_Absyn_Stmt* _temp2171; struct Cyc_Absyn_Stmt*
_temp2173; struct Cyc_Absyn_Exp* _temp2175; struct Cyc_Absyn_Stmt* _temp2177;
struct _tuple2 _temp2179; struct Cyc_Absyn_Stmt* _temp2181; struct Cyc_Absyn_Exp*
_temp2183; struct Cyc_Absyn_Stmt* _temp2185; struct Cyc_Absyn_Stmt* _temp2187;
struct Cyc_Absyn_Stmt* _temp2189; struct _tagged_string* _temp2191; struct Cyc_Absyn_Stmt*
_temp2193; struct _tuple2 _temp2195; struct Cyc_Absyn_Stmt* _temp2197; struct
Cyc_Absyn_Exp* _temp2199; struct _tuple2 _temp2201; struct Cyc_Absyn_Stmt*
_temp2203; struct Cyc_Absyn_Exp* _temp2205; struct Cyc_Absyn_Exp* _temp2207;
struct Cyc_List_List* _temp2209; struct Cyc_Absyn_Exp* _temp2211; struct Cyc_List_List*
_temp2213; struct Cyc_Absyn_Exp* _temp2215; struct Cyc_Absyn_Switch_clause**
_temp2217; struct Cyc_List_List* _temp2219; struct Cyc_Absyn_Switch_clause**
_temp2221; struct Cyc_List_List* _temp2223; struct Cyc_Absyn_Stmt* _temp2225;
struct Cyc_Absyn_Decl* _temp2227; struct Cyc_Absyn_Stmt* _temp2229; struct Cyc_Absyn_Stmt*
_temp2231; struct Cyc_Absyn_Stmt* _temp2233; struct _tagged_string* _temp2235;
struct _tuple2 _temp2237; struct Cyc_Absyn_Stmt* _temp2239; struct Cyc_Absyn_Exp*
_temp2241; struct Cyc_Absyn_Stmt* _temp2243; struct Cyc_List_List* _temp2245;
struct Cyc_Absyn_Stmt* _temp2247; struct Cyc_Absyn_Stmt* _temp2249; struct Cyc_Absyn_Vardecl*
_temp2251; struct Cyc_Absyn_Tvar* _temp2253; _LL2121: if( _temp2119 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL2122;} else{ goto _LL2123;} _LL2123: if((
unsigned int) _temp2119 > 1u?*(( int*) _temp2119) == Cyc_Absyn_Exp_s: 0){
_LL2164: _temp2163=(( struct Cyc_Absyn_Exp_s_struct*) _temp2119)->f1; goto
_LL2124;} else{ goto _LL2125;} _LL2125: if(( unsigned int) _temp2119 > 1u?*((
int*) _temp2119) == Cyc_Absyn_Seq_s: 0){ _LL2168: _temp2167=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2119)->f1; goto _LL2166; _LL2166: _temp2165=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2119)->f2; goto _LL2126;} else{ goto _LL2127;} _LL2127: if(( unsigned int)
_temp2119 > 1u?*(( int*) _temp2119) == Cyc_Absyn_Return_s: 0){ _LL2170:
_temp2169=(( struct Cyc_Absyn_Return_s_struct*) _temp2119)->f1; goto _LL2128;}
else{ goto _LL2129;} _LL2129: if(( unsigned int) _temp2119 > 1u?*(( int*)
_temp2119) == Cyc_Absyn_IfThenElse_s: 0){ _LL2176: _temp2175=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2119)->f1; goto _LL2174; _LL2174: _temp2173=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2119)->f2; goto _LL2172; _LL2172: _temp2171=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2119)->f3; goto _LL2130;} else{ goto _LL2131;} _LL2131: if(( unsigned int)
_temp2119 > 1u?*(( int*) _temp2119) == Cyc_Absyn_While_s: 0){ _LL2180: _temp2179=((
struct Cyc_Absyn_While_s_struct*) _temp2119)->f1; _LL2184: _temp2183= _temp2179.f1;
goto _LL2182; _LL2182: _temp2181= _temp2179.f2; goto _LL2178; _LL2178: _temp2177=((
struct Cyc_Absyn_While_s_struct*) _temp2119)->f2; goto _LL2132;} else{ goto
_LL2133;} _LL2133: if(( unsigned int) _temp2119 > 1u?*(( int*) _temp2119) == Cyc_Absyn_Break_s:
0){ _LL2186: _temp2185=(( struct Cyc_Absyn_Break_s_struct*) _temp2119)->f1; goto
_LL2134;} else{ goto _LL2135;} _LL2135: if(( unsigned int) _temp2119 > 1u?*((
int*) _temp2119) == Cyc_Absyn_Continue_s: 0){ _LL2188: _temp2187=(( struct Cyc_Absyn_Continue_s_struct*)
_temp2119)->f1; goto _LL2136;} else{ goto _LL2137;} _LL2137: if(( unsigned int)
_temp2119 > 1u?*(( int*) _temp2119) == Cyc_Absyn_Goto_s: 0){ _LL2192: _temp2191=((
struct Cyc_Absyn_Goto_s_struct*) _temp2119)->f1; goto _LL2190; _LL2190:
_temp2189=(( struct Cyc_Absyn_Goto_s_struct*) _temp2119)->f2; goto _LL2138;}
else{ goto _LL2139;} _LL2139: if(( unsigned int) _temp2119 > 1u?*(( int*)
_temp2119) == Cyc_Absyn_For_s: 0){ _LL2208: _temp2207=(( struct Cyc_Absyn_For_s_struct*)
_temp2119)->f1; goto _LL2202; _LL2202: _temp2201=(( struct Cyc_Absyn_For_s_struct*)
_temp2119)->f2; _LL2206: _temp2205= _temp2201.f1; goto _LL2204; _LL2204:
_temp2203= _temp2201.f2; goto _LL2196; _LL2196: _temp2195=(( struct Cyc_Absyn_For_s_struct*)
_temp2119)->f3; _LL2200: _temp2199= _temp2195.f1; goto _LL2198; _LL2198:
_temp2197= _temp2195.f2; goto _LL2194; _LL2194: _temp2193=(( struct Cyc_Absyn_For_s_struct*)
_temp2119)->f4; goto _LL2140;} else{ goto _LL2141;} _LL2141: if(( unsigned int)
_temp2119 > 1u?*(( int*) _temp2119) == Cyc_Absyn_Switch_s: 0){ _LL2212:
_temp2211=(( struct Cyc_Absyn_Switch_s_struct*) _temp2119)->f1; goto _LL2210;
_LL2210: _temp2209=(( struct Cyc_Absyn_Switch_s_struct*) _temp2119)->f2; goto
_LL2142;} else{ goto _LL2143;} _LL2143: if(( unsigned int) _temp2119 > 1u?*((
int*) _temp2119) == Cyc_Absyn_SwitchC_s: 0){ _LL2216: _temp2215=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp2119)->f1; goto _LL2214; _LL2214: _temp2213=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp2119)->f2; goto _LL2144;} else{ goto _LL2145;} _LL2145: if(( unsigned int)
_temp2119 > 1u?*(( int*) _temp2119) == Cyc_Absyn_Fallthru_s: 0){ _LL2220:
_temp2219=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp2119)->f1; if( _temp2219
== 0){ goto _LL2218;} else{ goto _LL2147;} _LL2218: _temp2217=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2119)->f2; goto _LL2146;} else{ goto _LL2147;} _LL2147: if(( unsigned int)
_temp2119 > 1u?*(( int*) _temp2119) == Cyc_Absyn_Fallthru_s: 0){ _LL2224:
_temp2223=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp2119)->f1; goto _LL2222;
_LL2222: _temp2221=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp2119)->f2; goto
_LL2148;} else{ goto _LL2149;} _LL2149: if(( unsigned int) _temp2119 > 1u?*((
int*) _temp2119) == Cyc_Absyn_Decl_s: 0){ _LL2228: _temp2227=(( struct Cyc_Absyn_Decl_s_struct*)
_temp2119)->f1; goto _LL2226; _LL2226: _temp2225=(( struct Cyc_Absyn_Decl_s_struct*)
_temp2119)->f2; goto _LL2150;} else{ goto _LL2151;} _LL2151: if(( unsigned int)
_temp2119 > 1u?*(( int*) _temp2119) == Cyc_Absyn_Cut_s: 0){ _LL2230: _temp2229=((
struct Cyc_Absyn_Cut_s_struct*) _temp2119)->f1; goto _LL2152;} else{ goto
_LL2153;} _LL2153: if(( unsigned int) _temp2119 > 1u?*(( int*) _temp2119) == Cyc_Absyn_Splice_s:
0){ _LL2232: _temp2231=(( struct Cyc_Absyn_Splice_s_struct*) _temp2119)->f1;
goto _LL2154;} else{ goto _LL2155;} _LL2155: if(( unsigned int) _temp2119 > 1u?*((
int*) _temp2119) == Cyc_Absyn_Label_s: 0){ _LL2236: _temp2235=(( struct Cyc_Absyn_Label_s_struct*)
_temp2119)->f1; goto _LL2234; _LL2234: _temp2233=(( struct Cyc_Absyn_Label_s_struct*)
_temp2119)->f2; goto _LL2156;} else{ goto _LL2157;} _LL2157: if(( unsigned int)
_temp2119 > 1u?*(( int*) _temp2119) == Cyc_Absyn_Do_s: 0){ _LL2244: _temp2243=((
struct Cyc_Absyn_Do_s_struct*) _temp2119)->f1; goto _LL2238; _LL2238: _temp2237=((
struct Cyc_Absyn_Do_s_struct*) _temp2119)->f2; _LL2242: _temp2241= _temp2237.f1;
goto _LL2240; _LL2240: _temp2239= _temp2237.f2; goto _LL2158;} else{ goto
_LL2159;} _LL2159: if(( unsigned int) _temp2119 > 1u?*(( int*) _temp2119) == Cyc_Absyn_TryCatch_s:
0){ _LL2248: _temp2247=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp2119)->f1;
goto _LL2246; _LL2246: _temp2245=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2119)->f2; goto _LL2160;} else{ goto _LL2161;} _LL2161: if(( unsigned int)
_temp2119 > 1u?*(( int*) _temp2119) == Cyc_Absyn_Region_s: 0){ _LL2254:
_temp2253=(( struct Cyc_Absyn_Region_s_struct*) _temp2119)->f1; goto _LL2252;
_LL2252: _temp2251=(( struct Cyc_Absyn_Region_s_struct*) _temp2119)->f2; goto
_LL2250; _LL2250: _temp2249=(( struct Cyc_Absyn_Region_s_struct*) _temp2119)->f3;
goto _LL2162;} else{ goto _LL2120;} _LL2122: s= Cyc_PP_text(({ unsigned char*
_temp2255=( unsigned char*)";"; struct _tagged_string _temp2256; _temp2256.curr=
_temp2255; _temp2256.base= _temp2255; _temp2256.last_plus_one= _temp2255 + 2;
_temp2256;})); goto _LL2120; _LL2124: s=({ struct Cyc_PP_Doc*(* _temp2257)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2260= Cyc_Absynpp_exp2doc(
_temp2163); struct Cyc_PP_Doc* _temp2261= Cyc_PP_text(({ unsigned char*
_temp2262=( unsigned char*)";"; struct _tagged_string _temp2263; _temp2263.curr=
_temp2262; _temp2263.base= _temp2262; _temp2263.last_plus_one= _temp2262 + 2;
_temp2263;})); struct Cyc_PP_Doc* _temp2258[ 2u]={ _temp2260, _temp2261}; struct
_tagged_ptr1 _temp2259={ _temp2258, _temp2258, _temp2258 + 2u}; _temp2257(
_temp2259);}); goto _LL2120; _LL2126: if( Cyc_Absynpp_decls_first){ if( Cyc_Absynpp_is_declaration(
_temp2167)){ s=({ struct Cyc_PP_Doc*(* _temp2264)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2267= Cyc_PP_text(({ unsigned char* _temp2286=(
unsigned char*)"{ "; struct _tagged_string _temp2287; _temp2287.curr= _temp2286;
_temp2287.base= _temp2286; _temp2287.last_plus_one= _temp2286 + 3; _temp2287;}));
struct Cyc_PP_Doc* _temp2268= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2167));
struct Cyc_PP_Doc* _temp2269= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2270=
Cyc_PP_text(({ unsigned char* _temp2284=( unsigned char*)"}"; struct
_tagged_string _temp2285; _temp2285.curr= _temp2284; _temp2285.base= _temp2284;
_temp2285.last_plus_one= _temp2284 + 2; _temp2285;})); struct Cyc_PP_Doc*
_temp2271= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2272= Cyc_Absynpp_is_declaration(
_temp2165)?({ struct Cyc_PP_Doc*(* _temp2273)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2276= Cyc_PP_text(({ unsigned char* _temp2282=(
unsigned char*)"{ "; struct _tagged_string _temp2283; _temp2283.curr= _temp2282;
_temp2283.base= _temp2282; _temp2283.last_plus_one= _temp2282 + 3; _temp2283;}));
struct Cyc_PP_Doc* _temp2277= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2165));
struct Cyc_PP_Doc* _temp2278= Cyc_PP_text(({ unsigned char* _temp2280=(
unsigned char*)"}"; struct _tagged_string _temp2281; _temp2281.curr= _temp2280;
_temp2281.base= _temp2280; _temp2281.last_plus_one= _temp2280 + 2; _temp2281;}));
struct Cyc_PP_Doc* _temp2279= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2274[ 4u]={
_temp2276, _temp2277, _temp2278, _temp2279}; struct _tagged_ptr1 _temp2275={
_temp2274, _temp2274, _temp2274 + 4u}; _temp2273( _temp2275);}): Cyc_Absynpp_stmt2doc(
_temp2165); struct Cyc_PP_Doc* _temp2265[ 6u]={ _temp2267, _temp2268, _temp2269,
_temp2270, _temp2271, _temp2272}; struct _tagged_ptr1 _temp2266={ _temp2265,
_temp2265, _temp2265 + 6u}; _temp2264( _temp2266);});} else{ if( Cyc_Absynpp_is_declaration(
_temp2165)){ s=({ struct Cyc_PP_Doc*(* _temp2288)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2291= Cyc_Absynpp_stmt2doc( _temp2167); struct Cyc_PP_Doc*
_temp2292= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2293= Cyc_PP_text(({
unsigned char* _temp2299=( unsigned char*)"{ "; struct _tagged_string _temp2300;
_temp2300.curr= _temp2299; _temp2300.base= _temp2299; _temp2300.last_plus_one=
_temp2299 + 3; _temp2300;})); struct Cyc_PP_Doc* _temp2294= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp2165)); struct Cyc_PP_Doc* _temp2295= Cyc_PP_text(({ unsigned char*
_temp2297=( unsigned char*)"}"; struct _tagged_string _temp2298; _temp2298.curr=
_temp2297; _temp2298.base= _temp2297; _temp2298.last_plus_one= _temp2297 + 2;
_temp2298;})); struct Cyc_PP_Doc* _temp2296= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2289[ 6u]={ _temp2291, _temp2292, _temp2293, _temp2294, _temp2295,
_temp2296}; struct _tagged_ptr1 _temp2290={ _temp2289, _temp2289, _temp2289 + 6u};
_temp2288( _temp2290);});} else{ s=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Stmt*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,({ unsigned char* _temp2301=(
unsigned char*)""; struct _tagged_string _temp2302; _temp2302.curr= _temp2301;
_temp2302.base= _temp2301; _temp2302.last_plus_one= _temp2301 + 1; _temp2302;}),({
struct Cyc_List_List* _temp2303=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2303->hd=( void*) _temp2167; _temp2303->tl=({
struct Cyc_List_List* _temp2304=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2304->hd=( void*) _temp2165; _temp2304->tl= 0;
_temp2304;}); _temp2303;}));}}} else{ s=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Stmt*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,({ unsigned char* _temp2305=(
unsigned char*)""; struct _tagged_string _temp2306; _temp2306.curr= _temp2305;
_temp2306.base= _temp2305; _temp2306.last_plus_one= _temp2305 + 1; _temp2306;}),({
struct Cyc_List_List* _temp2307=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2307->hd=( void*) _temp2167; _temp2307->tl=({
struct Cyc_List_List* _temp2308=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2308->hd=( void*) _temp2165; _temp2308->tl= 0;
_temp2308;}); _temp2307;}));} goto _LL2120; _LL2128: if( _temp2169 == 0){ s= Cyc_PP_text(({
unsigned char* _temp2309=( unsigned char*)"return;"; struct _tagged_string
_temp2310; _temp2310.curr= _temp2309; _temp2310.base= _temp2309; _temp2310.last_plus_one=
_temp2309 + 8; _temp2310;}));} else{ s=({ struct Cyc_PP_Doc*(* _temp2311)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2314= Cyc_PP_text(({
unsigned char* _temp2319=( unsigned char*)"return "; struct _tagged_string
_temp2320; _temp2320.curr= _temp2319; _temp2320.base= _temp2319; _temp2320.last_plus_one=
_temp2319 + 8; _temp2320;})); struct Cyc_PP_Doc* _temp2315= _temp2169 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp2169)); struct
Cyc_PP_Doc* _temp2316= Cyc_PP_text(({ unsigned char* _temp2317=( unsigned char*)";";
struct _tagged_string _temp2318; _temp2318.curr= _temp2317; _temp2318.base=
_temp2317; _temp2318.last_plus_one= _temp2317 + 2; _temp2318;})); struct Cyc_PP_Doc*
_temp2312[ 3u]={ _temp2314, _temp2315, _temp2316}; struct _tagged_ptr1 _temp2313={
_temp2312, _temp2312, _temp2312 + 3u}; _temp2311( _temp2313);});} goto _LL2120;
_LL2130: { int print_else;{ void* _temp2321=( void*) _temp2171->r; _LL2323: if(
_temp2321 ==( void*) Cyc_Absyn_Skip_s){ goto _LL2324;} else{ goto _LL2325;}
_LL2325: goto _LL2326; _LL2324: print_else= 0; goto _LL2322; _LL2326: print_else=
1; goto _LL2322; _LL2322:;} s=({ struct Cyc_PP_Doc*(* _temp2327)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2330= Cyc_PP_text(({
unsigned char* _temp2363=( unsigned char*)"if ("; struct _tagged_string
_temp2364; _temp2364.curr= _temp2363; _temp2364.base= _temp2363; _temp2364.last_plus_one=
_temp2363 + 5; _temp2364;})); struct Cyc_PP_Doc* _temp2331= Cyc_Absynpp_exp2doc(
_temp2175); struct Cyc_PP_Doc* _temp2332= Cyc_PP_text(({ unsigned char*
_temp2361=( unsigned char*)") {"; struct _tagged_string _temp2362; _temp2362.curr=
_temp2361; _temp2362.base= _temp2361; _temp2362.last_plus_one= _temp2361 + 4;
_temp2362;})); struct Cyc_PP_Doc* _temp2333= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(*
_temp2356)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2359= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2360= Cyc_Absynpp_stmt2doc( _temp2173); struct Cyc_PP_Doc*
_temp2357[ 2u]={ _temp2359, _temp2360}; struct _tagged_ptr1 _temp2358={
_temp2357, _temp2357, _temp2357 + 2u}; _temp2356( _temp2358);})); struct Cyc_PP_Doc*
_temp2334= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2335= Cyc_PP_text(({
unsigned char* _temp2354=( unsigned char*)"}"; struct _tagged_string _temp2355;
_temp2355.curr= _temp2354; _temp2355.base= _temp2354; _temp2355.last_plus_one=
_temp2354 + 2; _temp2355;})); struct Cyc_PP_Doc* _temp2336= print_else?({ struct
Cyc_PP_Doc*(* _temp2337)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2340= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2341= Cyc_PP_text(({
unsigned char* _temp2352=( unsigned char*)"else {"; struct _tagged_string
_temp2353; _temp2353.curr= _temp2352; _temp2353.base= _temp2352; _temp2353.last_plus_one=
_temp2352 + 7; _temp2353;})); struct Cyc_PP_Doc* _temp2342= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc*(* _temp2347)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2350= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2351= Cyc_Absynpp_stmt2doc(
_temp2171); struct Cyc_PP_Doc* _temp2348[ 2u]={ _temp2350, _temp2351}; struct
_tagged_ptr1 _temp2349={ _temp2348, _temp2348, _temp2348 + 2u}; _temp2347(
_temp2349);})); struct Cyc_PP_Doc* _temp2343= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2344= Cyc_PP_text(({ unsigned char* _temp2345=( unsigned char*)"}"; struct
_tagged_string _temp2346; _temp2346.curr= _temp2345; _temp2346.base= _temp2345;
_temp2346.last_plus_one= _temp2345 + 2; _temp2346;})); struct Cyc_PP_Doc*
_temp2338[ 5u]={ _temp2340, _temp2341, _temp2342, _temp2343, _temp2344}; struct
_tagged_ptr1 _temp2339={ _temp2338, _temp2338, _temp2338 + 5u}; _temp2337(
_temp2339);}): Cyc_PP_nil_doc(); struct Cyc_PP_Doc* _temp2328[ 7u]={ _temp2330,
_temp2331, _temp2332, _temp2333, _temp2334, _temp2335, _temp2336}; struct
_tagged_ptr1 _temp2329={ _temp2328, _temp2328, _temp2328 + 7u}; _temp2327(
_temp2329);}); goto _LL2120;} _LL2132: s=({ struct Cyc_PP_Doc*(* _temp2365)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2368= Cyc_PP_text(({
unsigned char* _temp2383=( unsigned char*)"while ("; struct _tagged_string
_temp2384; _temp2384.curr= _temp2383; _temp2384.base= _temp2383; _temp2384.last_plus_one=
_temp2383 + 8; _temp2384;})); struct Cyc_PP_Doc* _temp2369= Cyc_Absynpp_exp2doc(
_temp2183); struct Cyc_PP_Doc* _temp2370= Cyc_PP_text(({ unsigned char*
_temp2381=( unsigned char*)") {"; struct _tagged_string _temp2382; _temp2382.curr=
_temp2381; _temp2382.base= _temp2381; _temp2382.last_plus_one= _temp2381 + 4;
_temp2382;})); struct Cyc_PP_Doc* _temp2371= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(*
_temp2376)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2379= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2380= Cyc_Absynpp_stmt2doc( _temp2177); struct Cyc_PP_Doc*
_temp2377[ 2u]={ _temp2379, _temp2380}; struct _tagged_ptr1 _temp2378={
_temp2377, _temp2377, _temp2377 + 2u}; _temp2376( _temp2378);})); struct Cyc_PP_Doc*
_temp2372= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2373= Cyc_PP_text(({
unsigned char* _temp2374=( unsigned char*)"}"; struct _tagged_string _temp2375;
_temp2375.curr= _temp2374; _temp2375.base= _temp2374; _temp2375.last_plus_one=
_temp2374 + 2; _temp2375;})); struct Cyc_PP_Doc* _temp2366[ 6u]={ _temp2368,
_temp2369, _temp2370, _temp2371, _temp2372, _temp2373}; struct _tagged_ptr1
_temp2367={ _temp2366, _temp2366, _temp2366 + 6u}; _temp2365( _temp2367);});
goto _LL2120; _LL2134: s= Cyc_PP_text(({ unsigned char* _temp2385=(
unsigned char*)"break;"; struct _tagged_string _temp2386; _temp2386.curr=
_temp2385; _temp2386.base= _temp2385; _temp2386.last_plus_one= _temp2385 + 7;
_temp2386;})); goto _LL2120; _LL2136: s= Cyc_PP_text(({ unsigned char* _temp2387=(
unsigned char*)"continue;"; struct _tagged_string _temp2388; _temp2388.curr=
_temp2387; _temp2388.base= _temp2387; _temp2388.last_plus_one= _temp2387 + 10;
_temp2388;})); goto _LL2120; _LL2138: s= Cyc_PP_text(( struct _tagged_string)({
struct _tagged_string _temp2389=* _temp2191; xprintf("goto %.*s;", _temp2389.last_plus_one
- _temp2389.curr, _temp2389.curr);})); goto _LL2120; _LL2140: s=({ struct Cyc_PP_Doc*(*
_temp2390)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2393= Cyc_PP_text(({
unsigned char* _temp2416=( unsigned char*)"for("; struct _tagged_string
_temp2417; _temp2417.curr= _temp2416; _temp2417.base= _temp2416; _temp2417.last_plus_one=
_temp2416 + 5; _temp2417;})); struct Cyc_PP_Doc* _temp2394= Cyc_Absynpp_exp2doc(
_temp2207); struct Cyc_PP_Doc* _temp2395= Cyc_PP_text(({ unsigned char*
_temp2414=( unsigned char*)"; "; struct _tagged_string _temp2415; _temp2415.curr=
_temp2414; _temp2415.base= _temp2414; _temp2415.last_plus_one= _temp2414 + 3;
_temp2415;})); struct Cyc_PP_Doc* _temp2396= Cyc_Absynpp_exp2doc( _temp2205);
struct Cyc_PP_Doc* _temp2397= Cyc_PP_text(({ unsigned char* _temp2412=(
unsigned char*)"; "; struct _tagged_string _temp2413; _temp2413.curr= _temp2412;
_temp2413.base= _temp2412; _temp2413.last_plus_one= _temp2412 + 3; _temp2413;}));
struct Cyc_PP_Doc* _temp2398= Cyc_Absynpp_exp2doc( _temp2199); struct Cyc_PP_Doc*
_temp2399= Cyc_PP_text(({ unsigned char* _temp2410=( unsigned char*)") {";
struct _tagged_string _temp2411; _temp2411.curr= _temp2410; _temp2411.base=
_temp2410; _temp2411.last_plus_one= _temp2410 + 4; _temp2411;})); struct Cyc_PP_Doc*
_temp2400= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2405)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2408= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2409= Cyc_Absynpp_stmt2doc( _temp2193); struct Cyc_PP_Doc*
_temp2406[ 2u]={ _temp2408, _temp2409}; struct _tagged_ptr1 _temp2407={
_temp2406, _temp2406, _temp2406 + 2u}; _temp2405( _temp2407);})); struct Cyc_PP_Doc*
_temp2401= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2402= Cyc_PP_text(({
unsigned char* _temp2403=( unsigned char*)"}"; struct _tagged_string _temp2404;
_temp2404.curr= _temp2403; _temp2404.base= _temp2403; _temp2404.last_plus_one=
_temp2403 + 2; _temp2404;})); struct Cyc_PP_Doc* _temp2391[ 10u]={ _temp2393,
_temp2394, _temp2395, _temp2396, _temp2397, _temp2398, _temp2399, _temp2400,
_temp2401, _temp2402}; struct _tagged_ptr1 _temp2392={ _temp2391, _temp2391,
_temp2391 + 10u}; _temp2390( _temp2392);}); goto _LL2120; _LL2142: s=({ struct
Cyc_PP_Doc*(* _temp2418)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2421= Cyc_PP_text(({ unsigned char* _temp2432=( unsigned char*)"switch (";
struct _tagged_string _temp2433; _temp2433.curr= _temp2432; _temp2433.base=
_temp2432; _temp2433.last_plus_one= _temp2432 + 9; _temp2433;})); struct Cyc_PP_Doc*
_temp2422= Cyc_Absynpp_exp2doc( _temp2211); struct Cyc_PP_Doc* _temp2423= Cyc_PP_text(({
unsigned char* _temp2430=( unsigned char*)") {"; struct _tagged_string _temp2431;
_temp2431.curr= _temp2430; _temp2431.base= _temp2430; _temp2431.last_plus_one=
_temp2430 + 4; _temp2431;})); struct Cyc_PP_Doc* _temp2424= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2425= Cyc_Absynpp_switchclauses2doc( _temp2209); struct
Cyc_PP_Doc* _temp2426= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2427= Cyc_PP_text(({
unsigned char* _temp2428=( unsigned char*)"}"; struct _tagged_string _temp2429;
_temp2429.curr= _temp2428; _temp2429.base= _temp2428; _temp2429.last_plus_one=
_temp2428 + 2; _temp2429;})); struct Cyc_PP_Doc* _temp2419[ 7u]={ _temp2421,
_temp2422, _temp2423, _temp2424, _temp2425, _temp2426, _temp2427}; struct
_tagged_ptr1 _temp2420={ _temp2419, _temp2419, _temp2419 + 7u}; _temp2418(
_temp2420);}); goto _LL2120; _LL2144: s=({ struct Cyc_PP_Doc*(* _temp2434)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2437= Cyc_PP_text(({
unsigned char* _temp2448=( unsigned char*)"switch \"C\" ("; struct
_tagged_string _temp2449; _temp2449.curr= _temp2448; _temp2449.base= _temp2448;
_temp2449.last_plus_one= _temp2448 + 13; _temp2449;})); struct Cyc_PP_Doc*
_temp2438= Cyc_Absynpp_exp2doc( _temp2215); struct Cyc_PP_Doc* _temp2439= Cyc_PP_text(({
unsigned char* _temp2446=( unsigned char*)") {"; struct _tagged_string _temp2447;
_temp2447.curr= _temp2446; _temp2447.base= _temp2446; _temp2447.last_plus_one=
_temp2446 + 4; _temp2447;})); struct Cyc_PP_Doc* _temp2440= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2441= Cyc_Absynpp_switchCclauses2doc( _temp2213); struct
Cyc_PP_Doc* _temp2442= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2443= Cyc_PP_text(({
unsigned char* _temp2444=( unsigned char*)"}"; struct _tagged_string _temp2445;
_temp2445.curr= _temp2444; _temp2445.base= _temp2444; _temp2445.last_plus_one=
_temp2444 + 2; _temp2445;})); struct Cyc_PP_Doc* _temp2435[ 7u]={ _temp2437,
_temp2438, _temp2439, _temp2440, _temp2441, _temp2442, _temp2443}; struct
_tagged_ptr1 _temp2436={ _temp2435, _temp2435, _temp2435 + 7u}; _temp2434(
_temp2436);}); goto _LL2120; _LL2146: s= Cyc_PP_text(({ unsigned char* _temp2450=(
unsigned char*)"fallthru;"; struct _tagged_string _temp2451; _temp2451.curr=
_temp2450; _temp2451.base= _temp2450; _temp2451.last_plus_one= _temp2450 + 10;
_temp2451;})); goto _LL2120; _LL2148: s=({ struct Cyc_PP_Doc*(* _temp2452)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2455= Cyc_PP_text(({
unsigned char* _temp2460=( unsigned char*)"fallthru("; struct _tagged_string
_temp2461; _temp2461.curr= _temp2460; _temp2461.base= _temp2460; _temp2461.last_plus_one=
_temp2460 + 10; _temp2461;})); struct Cyc_PP_Doc* _temp2456= Cyc_Absynpp_exps2doc_prec(
20, _temp2223); struct Cyc_PP_Doc* _temp2457= Cyc_PP_text(({ unsigned char*
_temp2458=( unsigned char*)");"; struct _tagged_string _temp2459; _temp2459.curr=
_temp2458; _temp2459.base= _temp2458; _temp2459.last_plus_one= _temp2458 + 3;
_temp2459;})); struct Cyc_PP_Doc* _temp2453[ 3u]={ _temp2455, _temp2456,
_temp2457}; struct _tagged_ptr1 _temp2454={ _temp2453, _temp2453, _temp2453 + 3u};
_temp2452( _temp2454);}); goto _LL2120; _LL2150: s=({ struct Cyc_PP_Doc*(*
_temp2462)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2465= Cyc_Absynpp_decl2doc(
_temp2227); struct Cyc_PP_Doc* _temp2466= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2467= Cyc_Absynpp_stmt2doc( _temp2225); struct Cyc_PP_Doc* _temp2463[ 3u]={
_temp2465, _temp2466, _temp2467}; struct _tagged_ptr1 _temp2464={ _temp2463,
_temp2463, _temp2463 + 3u}; _temp2462( _temp2464);}); goto _LL2120; _LL2152: s=({
struct Cyc_PP_Doc*(* _temp2468)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2471= Cyc_PP_text(({ unsigned char* _temp2473=( unsigned char*)"cut ";
struct _tagged_string _temp2474; _temp2474.curr= _temp2473; _temp2474.base=
_temp2473; _temp2474.last_plus_one= _temp2473 + 5; _temp2474;})); struct Cyc_PP_Doc*
_temp2472= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2229)); struct Cyc_PP_Doc*
_temp2469[ 2u]={ _temp2471, _temp2472}; struct _tagged_ptr1 _temp2470={
_temp2469, _temp2469, _temp2469 + 2u}; _temp2468( _temp2470);}); goto _LL2120;
_LL2154: s=({ struct Cyc_PP_Doc*(* _temp2475)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2478= Cyc_PP_text(({ unsigned char* _temp2480=(
unsigned char*)"splice "; struct _tagged_string _temp2481; _temp2481.curr=
_temp2480; _temp2481.base= _temp2480; _temp2481.last_plus_one= _temp2480 + 8;
_temp2481;})); struct Cyc_PP_Doc* _temp2479= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp2231)); struct Cyc_PP_Doc* _temp2476[ 2u]={ _temp2478, _temp2479}; struct
_tagged_ptr1 _temp2477={ _temp2476, _temp2476, _temp2476 + 2u}; _temp2475(
_temp2477);}); goto _LL2120; _LL2156: if( Cyc_Absynpp_decls_first? Cyc_Absynpp_is_declaration(
_temp2233): 0){ s=({ struct Cyc_PP_Doc*(* _temp2482)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2485= Cyc_PP_textptr( _temp2235); struct Cyc_PP_Doc*
_temp2486= Cyc_PP_text(({ unsigned char* _temp2493=( unsigned char*)": {";
struct _tagged_string _temp2494; _temp2494.curr= _temp2493; _temp2494.base=
_temp2493; _temp2494.last_plus_one= _temp2493 + 4; _temp2494;})); struct Cyc_PP_Doc*
_temp2487= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2488= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp2233)); struct Cyc_PP_Doc* _temp2489= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2490= Cyc_PP_text(({ unsigned char* _temp2491=( unsigned char*)"}"; struct
_tagged_string _temp2492; _temp2492.curr= _temp2491; _temp2492.base= _temp2491;
_temp2492.last_plus_one= _temp2491 + 2; _temp2492;})); struct Cyc_PP_Doc*
_temp2483[ 6u]={ _temp2485, _temp2486, _temp2487, _temp2488, _temp2489,
_temp2490}; struct _tagged_ptr1 _temp2484={ _temp2483, _temp2483, _temp2483 + 6u};
_temp2482( _temp2484);});} else{ s=({ struct Cyc_PP_Doc*(* _temp2495)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2498= Cyc_PP_textptr(
_temp2235); struct Cyc_PP_Doc* _temp2499= Cyc_PP_text(({ unsigned char*
_temp2501=( unsigned char*)": "; struct _tagged_string _temp2502; _temp2502.curr=
_temp2501; _temp2502.base= _temp2501; _temp2502.last_plus_one= _temp2501 + 3;
_temp2502;})); struct Cyc_PP_Doc* _temp2500= Cyc_Absynpp_stmt2doc( _temp2233);
struct Cyc_PP_Doc* _temp2496[ 3u]={ _temp2498, _temp2499, _temp2500}; struct
_tagged_ptr1 _temp2497={ _temp2496, _temp2496, _temp2496 + 3u}; _temp2495(
_temp2497);});} goto _LL2120; _LL2158: s=({ struct Cyc_PP_Doc*(* _temp2503)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2506= Cyc_PP_text(({
unsigned char* _temp2517=( unsigned char*)"do {"; struct _tagged_string
_temp2518; _temp2518.curr= _temp2517; _temp2518.base= _temp2517; _temp2518.last_plus_one=
_temp2517 + 5; _temp2518;})); struct Cyc_PP_Doc* _temp2507= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2508= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2243));
struct Cyc_PP_Doc* _temp2509= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2510=
Cyc_PP_text(({ unsigned char* _temp2515=( unsigned char*)"} while ("; struct
_tagged_string _temp2516; _temp2516.curr= _temp2515; _temp2516.base= _temp2515;
_temp2516.last_plus_one= _temp2515 + 10; _temp2516;})); struct Cyc_PP_Doc*
_temp2511= Cyc_Absynpp_exp2doc( _temp2241); struct Cyc_PP_Doc* _temp2512= Cyc_PP_text(({
unsigned char* _temp2513=( unsigned char*)");"; struct _tagged_string _temp2514;
_temp2514.curr= _temp2513; _temp2514.base= _temp2513; _temp2514.last_plus_one=
_temp2513 + 3; _temp2514;})); struct Cyc_PP_Doc* _temp2504[ 7u]={ _temp2506,
_temp2507, _temp2508, _temp2509, _temp2510, _temp2511, _temp2512}; struct
_tagged_ptr1 _temp2505={ _temp2504, _temp2504, _temp2504 + 7u}; _temp2503(
_temp2505);}); goto _LL2120; _LL2160: s=({ struct Cyc_PP_Doc*(* _temp2519)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2522= Cyc_PP_text(({
unsigned char* _temp2535=( unsigned char*)"try {"; struct _tagged_string
_temp2536; _temp2536.curr= _temp2535; _temp2536.base= _temp2535; _temp2536.last_plus_one=
_temp2535 + 6; _temp2536;})); struct Cyc_PP_Doc* _temp2523= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2524= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2247));
struct Cyc_PP_Doc* _temp2525= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2526=
Cyc_PP_text(({ unsigned char* _temp2533=( unsigned char*)"} catch {"; struct
_tagged_string _temp2534; _temp2534.curr= _temp2533; _temp2534.base= _temp2533;
_temp2534.last_plus_one= _temp2533 + 10; _temp2534;})); struct Cyc_PP_Doc*
_temp2527= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2528= Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc(
_temp2245)); struct Cyc_PP_Doc* _temp2529= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2530= Cyc_PP_text(({ unsigned char* _temp2531=( unsigned char*)"}"; struct
_tagged_string _temp2532; _temp2532.curr= _temp2531; _temp2532.base= _temp2531;
_temp2532.last_plus_one= _temp2531 + 2; _temp2532;})); struct Cyc_PP_Doc*
_temp2520[ 9u]={ _temp2522, _temp2523, _temp2524, _temp2525, _temp2526,
_temp2527, _temp2528, _temp2529, _temp2530}; struct _tagged_ptr1 _temp2521={
_temp2520, _temp2520, _temp2520 + 9u}; _temp2519( _temp2521);}); goto _LL2120;
_LL2162: s=({ struct Cyc_PP_Doc*(* _temp2537)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2540= Cyc_PP_text(({ unsigned char* _temp2555=(
unsigned char*)"region<"; struct _tagged_string _temp2556; _temp2556.curr=
_temp2555; _temp2556.base= _temp2555; _temp2556.last_plus_one= _temp2555 + 8;
_temp2556;})); struct Cyc_PP_Doc* _temp2541= Cyc_PP_textptr( Cyc_Absynpp_get_name(
_temp2253)); struct Cyc_PP_Doc* _temp2542= Cyc_PP_text(({ unsigned char*
_temp2553=( unsigned char*)">"; struct _tagged_string _temp2554; _temp2554.curr=
_temp2553; _temp2554.base= _temp2553; _temp2554.last_plus_one= _temp2553 + 2;
_temp2554;})); struct Cyc_PP_Doc* _temp2543= Cyc_Absynpp_qvar2doc( _temp2251->name);
struct Cyc_PP_Doc* _temp2544= Cyc_PP_text(({ unsigned char* _temp2551=(
unsigned char*)"{"; struct _tagged_string _temp2552; _temp2552.curr= _temp2551;
_temp2552.base= _temp2551; _temp2552.last_plus_one= _temp2551 + 2; _temp2552;}));
struct Cyc_PP_Doc* _temp2545= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2546=
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2249)); struct Cyc_PP_Doc* _temp2547=
Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2548= Cyc_PP_text(({ unsigned char*
_temp2549=( unsigned char*)"}"; struct _tagged_string _temp2550; _temp2550.curr=
_temp2549; _temp2550.base= _temp2549; _temp2550.last_plus_one= _temp2549 + 2;
_temp2550;})); struct Cyc_PP_Doc* _temp2538[ 9u]={ _temp2540, _temp2541,
_temp2542, _temp2543, _temp2544, _temp2545, _temp2546, _temp2547, _temp2548};
struct _tagged_ptr1 _temp2539={ _temp2538, _temp2538, _temp2538 + 9u}; _temp2537(
_temp2539);}); goto _LL2120; _LL2120:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void* _temp2557=( void*) p->r;
int _temp2589; void* _temp2591; unsigned char _temp2593; struct _tagged_string
_temp2595; struct Cyc_Absyn_Vardecl* _temp2597; struct Cyc_List_List* _temp2599;
struct Cyc_Absyn_Pat* _temp2601; struct Cyc_Absyn_Vardecl* _temp2603; struct
_tuple0* _temp2605; struct Cyc_List_List* _temp2607; struct Cyc_List_List*
_temp2609; struct _tuple0* _temp2611; struct Cyc_List_List* _temp2613; struct
Cyc_List_List* _temp2615; struct _tuple0* _temp2617; struct Cyc_List_List*
_temp2619; struct Cyc_List_List* _temp2621; struct Cyc_Core_Opt* _temp2623;
struct Cyc_Absyn_Structdecl* _temp2625; struct Cyc_Absyn_Enumfield* _temp2627;
struct Cyc_Absyn_Enumdecl* _temp2629; struct Cyc_List_List* _temp2631; struct
Cyc_List_List* _temp2633; struct Cyc_Absyn_Tunionfield* _temp2635; struct Cyc_Absyn_Tuniondecl*
_temp2637; _LL2559: if( _temp2557 ==( void*) Cyc_Absyn_Wild_p){ goto _LL2560;}
else{ goto _LL2561;} _LL2561: if( _temp2557 ==( void*) Cyc_Absyn_Null_p){ goto
_LL2562;} else{ goto _LL2563;} _LL2563: if(( unsigned int) _temp2557 > 2u?*((
int*) _temp2557) == Cyc_Absyn_Int_p: 0){ _LL2592: _temp2591=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp2557)->f1; goto _LL2590; _LL2590: _temp2589=((
struct Cyc_Absyn_Int_p_struct*) _temp2557)->f2; goto _LL2564;} else{ goto
_LL2565;} _LL2565: if(( unsigned int) _temp2557 > 2u?*(( int*) _temp2557) == Cyc_Absyn_Char_p:
0){ _LL2594: _temp2593=(( struct Cyc_Absyn_Char_p_struct*) _temp2557)->f1; goto
_LL2566;} else{ goto _LL2567;} _LL2567: if(( unsigned int) _temp2557 > 2u?*((
int*) _temp2557) == Cyc_Absyn_Float_p: 0){ _LL2596: _temp2595=(( struct Cyc_Absyn_Float_p_struct*)
_temp2557)->f1; goto _LL2568;} else{ goto _LL2569;} _LL2569: if(( unsigned int)
_temp2557 > 2u?*(( int*) _temp2557) == Cyc_Absyn_Var_p: 0){ _LL2598: _temp2597=((
struct Cyc_Absyn_Var_p_struct*) _temp2557)->f1; goto _LL2570;} else{ goto
_LL2571;} _LL2571: if(( unsigned int) _temp2557 > 2u?*(( int*) _temp2557) == Cyc_Absyn_Tuple_p:
0){ _LL2600: _temp2599=(( struct Cyc_Absyn_Tuple_p_struct*) _temp2557)->f1; goto
_LL2572;} else{ goto _LL2573;} _LL2573: if(( unsigned int) _temp2557 > 2u?*((
int*) _temp2557) == Cyc_Absyn_Pointer_p: 0){ _LL2602: _temp2601=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2557)->f1; goto _LL2574;} else{ goto _LL2575;} _LL2575: if(( unsigned int)
_temp2557 > 2u?*(( int*) _temp2557) == Cyc_Absyn_Reference_p: 0){ _LL2604:
_temp2603=(( struct Cyc_Absyn_Reference_p_struct*) _temp2557)->f1; goto _LL2576;}
else{ goto _LL2577;} _LL2577: if(( unsigned int) _temp2557 > 2u?*(( int*)
_temp2557) == Cyc_Absyn_UnknownId_p: 0){ _LL2606: _temp2605=(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp2557)->f1; goto _LL2578;} else{ goto _LL2579;} _LL2579: if(( unsigned int)
_temp2557 > 2u?*(( int*) _temp2557) == Cyc_Absyn_UnknownCall_p: 0){ _LL2612:
_temp2611=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp2557)->f1; goto
_LL2610; _LL2610: _temp2609=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp2557)->f2;
goto _LL2608; _LL2608: _temp2607=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2557)->f3; goto _LL2580;} else{ goto _LL2581;} _LL2581: if(( unsigned int)
_temp2557 > 2u?*(( int*) _temp2557) == Cyc_Absyn_UnknownFields_p: 0){ _LL2618:
_temp2617=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp2557)->f1; goto
_LL2616; _LL2616: _temp2615=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2557)->f2; goto _LL2614; _LL2614: _temp2613=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2557)->f3; goto _LL2582;} else{ goto _LL2583;} _LL2583: if(( unsigned int)
_temp2557 > 2u?*(( int*) _temp2557) == Cyc_Absyn_Struct_p: 0){ _LL2626:
_temp2625=(( struct Cyc_Absyn_Struct_p_struct*) _temp2557)->f1; goto _LL2624;
_LL2624: _temp2623=(( struct Cyc_Absyn_Struct_p_struct*) _temp2557)->f2; goto
_LL2622; _LL2622: _temp2621=(( struct Cyc_Absyn_Struct_p_struct*) _temp2557)->f3;
goto _LL2620; _LL2620: _temp2619=(( struct Cyc_Absyn_Struct_p_struct*) _temp2557)->f4;
goto _LL2584;} else{ goto _LL2585;} _LL2585: if(( unsigned int) _temp2557 > 2u?*((
int*) _temp2557) == Cyc_Absyn_Enum_p: 0){ _LL2630: _temp2629=(( struct Cyc_Absyn_Enum_p_struct*)
_temp2557)->f1; goto _LL2628; _LL2628: _temp2627=(( struct Cyc_Absyn_Enum_p_struct*)
_temp2557)->f2; goto _LL2586;} else{ goto _LL2587;} _LL2587: if(( unsigned int)
_temp2557 > 2u?*(( int*) _temp2557) == Cyc_Absyn_Tunion_p: 0){ _LL2638:
_temp2637=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2557)->f1; goto _LL2636;
_LL2636: _temp2635=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2557)->f2; goto
_LL2634; _LL2634: _temp2633=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2557)->f3;
goto _LL2632; _LL2632: _temp2631=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2557)->f4;
goto _LL2588;} else{ goto _LL2558;} _LL2560: s= Cyc_PP_text(({ unsigned char*
_temp2639=( unsigned char*)"_"; struct _tagged_string _temp2640; _temp2640.curr=
_temp2639; _temp2640.base= _temp2639; _temp2640.last_plus_one= _temp2639 + 2;
_temp2640;})); goto _LL2558; _LL2562: s= Cyc_PP_text(({ unsigned char* _temp2641=(
unsigned char*)"null"; struct _tagged_string _temp2642; _temp2642.curr=
_temp2641; _temp2642.base= _temp2641; _temp2642.last_plus_one= _temp2641 + 5;
_temp2642;})); goto _LL2558; _LL2564: if( _temp2591 ==( void*) Cyc_Absyn_Signed){
s= Cyc_PP_text(( struct _tagged_string) xprintf("%d", _temp2589));} else{ s= Cyc_PP_text((
struct _tagged_string) xprintf("%u",( unsigned int) _temp2589));} goto _LL2558;
_LL2566: s= Cyc_PP_text(( struct _tagged_string)({ struct _tagged_string
_temp2643= Cyc_Absynpp_char_escape( _temp2593); xprintf("'%.*s'", _temp2643.last_plus_one
- _temp2643.curr, _temp2643.curr);})); goto _LL2558; _LL2568: s= Cyc_PP_text(
_temp2595); goto _LL2558; _LL2570: s= Cyc_Absynpp_qvar2doc( _temp2597->name);
goto _LL2558; _LL2572: s=({ struct Cyc_PP_Doc*(* _temp2644)( struct _tagged_ptr1)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp2647= Cyc_PP_text(({ unsigned char*
_temp2654=( unsigned char*)"$("; struct _tagged_string _temp2655; _temp2655.curr=
_temp2654; _temp2655.base= _temp2654; _temp2655.last_plus_one= _temp2654 + 3;
_temp2655;})); struct Cyc_PP_Doc* _temp2648=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Pat*), struct _tagged_string sep, struct Cyc_List_List* l0))
Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc,({ unsigned char* _temp2652=( unsigned char*)",";
struct _tagged_string _temp2653; _temp2653.curr= _temp2652; _temp2653.base=
_temp2652; _temp2653.last_plus_one= _temp2652 + 2; _temp2653;}), _temp2599);
struct Cyc_PP_Doc* _temp2649= Cyc_PP_text(({ unsigned char* _temp2650=(
unsigned char*)")"; struct _tagged_string _temp2651; _temp2651.curr= _temp2650;
_temp2651.base= _temp2650; _temp2651.last_plus_one= _temp2650 + 2; _temp2651;}));
struct Cyc_PP_Doc* _temp2645[ 3u]={ _temp2647, _temp2648, _temp2649}; struct
_tagged_ptr1 _temp2646={ _temp2645, _temp2645, _temp2645 + 3u}; _temp2644(
_temp2646);}); goto _LL2558; _LL2574: s=({ struct Cyc_PP_Doc*(* _temp2656)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2659= Cyc_PP_text(({
unsigned char* _temp2661=( unsigned char*)"&"; struct _tagged_string _temp2662;
_temp2662.curr= _temp2661; _temp2662.base= _temp2661; _temp2662.last_plus_one=
_temp2661 + 2; _temp2662;})); struct Cyc_PP_Doc* _temp2660= Cyc_Absynpp_pat2doc(
_temp2601); struct Cyc_PP_Doc* _temp2657[ 2u]={ _temp2659, _temp2660}; struct
_tagged_ptr1 _temp2658={ _temp2657, _temp2657, _temp2657 + 2u}; _temp2656(
_temp2658);}); goto _LL2558; _LL2576: s=({ struct Cyc_PP_Doc*(* _temp2663)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2666= Cyc_PP_text(({
unsigned char* _temp2668=( unsigned char*)"*"; struct _tagged_string _temp2669;
_temp2669.curr= _temp2668; _temp2669.base= _temp2668; _temp2669.last_plus_one=
_temp2668 + 2; _temp2669;})); struct Cyc_PP_Doc* _temp2667= Cyc_Absynpp_qvar2doc(
_temp2603->name); struct Cyc_PP_Doc* _temp2664[ 2u]={ _temp2666, _temp2667};
struct _tagged_ptr1 _temp2665={ _temp2664, _temp2664, _temp2664 + 2u}; _temp2663(
_temp2665);}); goto _LL2558; _LL2578: s= Cyc_Absynpp_qvar2doc( _temp2605); goto
_LL2558; _LL2580: s=({ struct Cyc_PP_Doc*(* _temp2670)( struct _tagged_ptr1)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp2673= Cyc_Absynpp_qvar2doc( _temp2611);
struct Cyc_PP_Doc* _temp2674= Cyc_Absynpp_tvars2doc( _temp2609); struct Cyc_PP_Doc*
_temp2675= Cyc_PP_group(({ unsigned char* _temp2676=( unsigned char*)"("; struct
_tagged_string _temp2677; _temp2677.curr= _temp2676; _temp2677.base= _temp2676;
_temp2677.last_plus_one= _temp2676 + 2; _temp2677;}),({ unsigned char* _temp2678=(
unsigned char*)")"; struct _tagged_string _temp2679; _temp2679.curr= _temp2678;
_temp2679.base= _temp2678; _temp2679.last_plus_one= _temp2678 + 2; _temp2679;}),({
unsigned char* _temp2680=( unsigned char*)","; struct _tagged_string _temp2681;
_temp2681.curr= _temp2680; _temp2681.base= _temp2680; _temp2681.last_plus_one=
_temp2680 + 2; _temp2681;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc,
_temp2607)); struct Cyc_PP_Doc* _temp2671[ 3u]={ _temp2673, _temp2674, _temp2675};
struct _tagged_ptr1 _temp2672={ _temp2671, _temp2671, _temp2671 + 3u}; _temp2670(
_temp2672);}); goto _LL2558; _LL2582: s=({ struct Cyc_PP_Doc*(* _temp2682)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2685= Cyc_Absynpp_qvar2doc(
_temp2617); struct Cyc_PP_Doc* _temp2686= Cyc_Absynpp_tvars2doc( _temp2615);
struct Cyc_PP_Doc* _temp2687= Cyc_PP_group(({ unsigned char* _temp2688=(
unsigned char*)"{"; struct _tagged_string _temp2689; _temp2689.curr= _temp2688;
_temp2689.base= _temp2688; _temp2689.last_plus_one= _temp2688 + 2; _temp2689;}),({
unsigned char* _temp2690=( unsigned char*)"}"; struct _tagged_string _temp2691;
_temp2691.curr= _temp2690; _temp2691.base= _temp2690; _temp2691.last_plus_one=
_temp2690 + 2; _temp2691;}),({ unsigned char* _temp2692=( unsigned char*)",";
struct _tagged_string _temp2693; _temp2693.curr= _temp2692; _temp2693.base=
_temp2692; _temp2693.last_plus_one= _temp2692 + 2; _temp2693;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_dp2doc, _temp2613)); struct Cyc_PP_Doc* _temp2683[ 3u]={ _temp2685,
_temp2686, _temp2687}; struct _tagged_ptr1 _temp2684={ _temp2683, _temp2683,
_temp2683 + 3u}; _temp2682( _temp2684);}); goto _LL2558; _LL2584: s=({ struct
Cyc_PP_Doc*(* _temp2694)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2697= _temp2625->name == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp2625->name))->v); struct Cyc_PP_Doc*
_temp2698= Cyc_Absynpp_tvars2doc( _temp2621); struct Cyc_PP_Doc* _temp2699= Cyc_PP_group(({
unsigned char* _temp2700=( unsigned char*)"{"; struct _tagged_string _temp2701;
_temp2701.curr= _temp2700; _temp2701.base= _temp2700; _temp2701.last_plus_one=
_temp2700 + 2; _temp2701;}),({ unsigned char* _temp2702=( unsigned char*)"}";
struct _tagged_string _temp2703; _temp2703.curr= _temp2702; _temp2703.base=
_temp2702; _temp2703.last_plus_one= _temp2702 + 2; _temp2703;}),({ unsigned char*
_temp2704=( unsigned char*)","; struct _tagged_string _temp2705; _temp2705.curr=
_temp2704; _temp2705.base= _temp2704; _temp2705.last_plus_one= _temp2704 + 2;
_temp2705;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp2619)); struct Cyc_PP_Doc* _temp2695[ 3u]={ _temp2697, _temp2698, _temp2699};
struct _tagged_ptr1 _temp2696={ _temp2695, _temp2695, _temp2695 + 3u}; _temp2694(
_temp2696);}); goto _LL2558; _LL2586: s= Cyc_Absynpp_qvar2doc( _temp2627->name);
goto _LL2558; _LL2588: if( _temp2631 == 0){ s= Cyc_Absynpp_qvar2doc( _temp2635->name);}
else{ s=({ struct Cyc_PP_Doc*(* _temp2706)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2709= Cyc_Absynpp_qvar2doc( _temp2635->name); struct Cyc_PP_Doc*
_temp2710= Cyc_Absynpp_tvars2doc( _temp2633); struct Cyc_PP_Doc* _temp2711= Cyc_PP_egroup(({
unsigned char* _temp2712=( unsigned char*)"("; struct _tagged_string _temp2713;
_temp2713.curr= _temp2712; _temp2713.base= _temp2712; _temp2713.last_plus_one=
_temp2712 + 2; _temp2713;}),({ unsigned char* _temp2714=( unsigned char*)")";
struct _tagged_string _temp2715; _temp2715.curr= _temp2714; _temp2715.base=
_temp2714; _temp2715.last_plus_one= _temp2714 + 2; _temp2715;}),({ unsigned char*
_temp2716=( unsigned char*)","; struct _tagged_string _temp2717; _temp2717.curr=
_temp2716; _temp2717.base= _temp2716; _temp2717.last_plus_one= _temp2716 + 2;
_temp2717;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc, _temp2631));
struct Cyc_PP_Doc* _temp2707[ 3u]={ _temp2709, _temp2710, _temp2711}; struct
_tagged_ptr1 _temp2708={ _temp2707, _temp2707, _temp2707 + 3u}; _temp2706(
_temp2708);});} goto _LL2558; _LL2558:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_dp2doc(
struct _tuple5* dp){ return({ struct Cyc_PP_Doc*(* _temp2718)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2721= Cyc_PP_egroup(({
unsigned char* _temp2723=( unsigned char*)""; struct _tagged_string _temp2724;
_temp2724.curr= _temp2723; _temp2724.base= _temp2723; _temp2724.last_plus_one=
_temp2723 + 1; _temp2724;}),({ unsigned char* _temp2725=( unsigned char*)"=";
struct _tagged_string _temp2726; _temp2726.curr= _temp2725; _temp2726.base=
_temp2725; _temp2726.last_plus_one= _temp2725 + 2; _temp2726;}),({ unsigned char*
_temp2727=( unsigned char*)"="; struct _tagged_string _temp2728; _temp2728.curr=
_temp2727; _temp2728.base= _temp2727; _temp2728.last_plus_one= _temp2727 + 2;
_temp2728;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(* dp).f1));
struct Cyc_PP_Doc* _temp2722= Cyc_Absynpp_pat2doc((* dp).f2); struct Cyc_PP_Doc*
_temp2719[ 2u]={ _temp2721, _temp2722}; struct _tagged_ptr1 _temp2720={
_temp2719, _temp2719, _temp2719 + 2u}; _temp2718( _temp2720);});} struct Cyc_PP_Doc*
Cyc_Absynpp_switchclause2doc( struct Cyc_Absyn_Switch_clause* c){ if( c->where_clause
== 0?( void*)( c->pattern)->r ==( void*) Cyc_Absyn_Wild_p: 0){ return({ struct
Cyc_PP_Doc*(* _temp2729)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2732= Cyc_PP_text(({ unsigned char* _temp2739=( unsigned char*)"default: ";
struct _tagged_string _temp2740; _temp2740.curr= _temp2739; _temp2740.base=
_temp2739; _temp2740.last_plus_one= _temp2739 + 10; _temp2740;})); struct Cyc_PP_Doc*
_temp2733= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2734)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2737= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2738= Cyc_Absynpp_stmt2doc( c->body); struct Cyc_PP_Doc*
_temp2735[ 2u]={ _temp2737, _temp2738}; struct _tagged_ptr1 _temp2736={
_temp2735, _temp2735, _temp2735 + 2u}; _temp2734( _temp2736);})); struct Cyc_PP_Doc*
_temp2730[ 2u]={ _temp2732, _temp2733}; struct _tagged_ptr1 _temp2731={
_temp2730, _temp2730, _temp2730 + 2u}; _temp2729( _temp2731);});} else{ if( c->where_clause
== 0){ return({ struct Cyc_PP_Doc*(* _temp2741)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2744= Cyc_PP_text(({ unsigned char* _temp2755=(
unsigned char*)"case "; struct _tagged_string _temp2756; _temp2756.curr=
_temp2755; _temp2756.base= _temp2755; _temp2756.last_plus_one= _temp2755 + 6;
_temp2756;})); struct Cyc_PP_Doc* _temp2745= Cyc_Absynpp_pat2doc( c->pattern);
struct Cyc_PP_Doc* _temp2746= Cyc_PP_text(({ unsigned char* _temp2753=(
unsigned char*)": "; struct _tagged_string _temp2754; _temp2754.curr= _temp2753;
_temp2754.base= _temp2753; _temp2754.last_plus_one= _temp2753 + 3; _temp2754;}));
struct Cyc_PP_Doc* _temp2747= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2748)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2751= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2752= Cyc_Absynpp_stmt2doc( c->body); struct Cyc_PP_Doc*
_temp2749[ 2u]={ _temp2751, _temp2752}; struct _tagged_ptr1 _temp2750={
_temp2749, _temp2749, _temp2749 + 2u}; _temp2748( _temp2750);})); struct Cyc_PP_Doc*
_temp2742[ 4u]={ _temp2744, _temp2745, _temp2746, _temp2747}; struct
_tagged_ptr1 _temp2743={ _temp2742, _temp2742, _temp2742 + 4u}; _temp2741(
_temp2743);});} else{ return({ struct Cyc_PP_Doc*(* _temp2757)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2760= Cyc_PP_text(({
unsigned char* _temp2775=( unsigned char*)"case "; struct _tagged_string
_temp2776; _temp2776.curr= _temp2775; _temp2776.base= _temp2775; _temp2776.last_plus_one=
_temp2775 + 6; _temp2776;})); struct Cyc_PP_Doc* _temp2761= Cyc_Absynpp_pat2doc(
c->pattern); struct Cyc_PP_Doc* _temp2762= Cyc_PP_text(({ unsigned char*
_temp2773=( unsigned char*)" && "; struct _tagged_string _temp2774; _temp2774.curr=
_temp2773; _temp2774.base= _temp2773; _temp2774.last_plus_one= _temp2773 + 5;
_temp2774;})); struct Cyc_PP_Doc* _temp2763= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)
_check_null( c->where_clause)); struct Cyc_PP_Doc* _temp2764= Cyc_PP_text(({
unsigned char* _temp2771=( unsigned char*)": "; struct _tagged_string _temp2772;
_temp2772.curr= _temp2771; _temp2772.base= _temp2771; _temp2772.last_plus_one=
_temp2771 + 3; _temp2772;})); struct Cyc_PP_Doc* _temp2765= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc*(* _temp2766)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2769= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2770= Cyc_Absynpp_stmt2doc(
c->body); struct Cyc_PP_Doc* _temp2767[ 2u]={ _temp2769, _temp2770}; struct
_tagged_ptr1 _temp2768={ _temp2767, _temp2767, _temp2767 + 2u}; _temp2766(
_temp2768);})); struct Cyc_PP_Doc* _temp2758[ 6u]={ _temp2760, _temp2761,
_temp2762, _temp2763, _temp2764, _temp2765}; struct _tagged_ptr1 _temp2759={
_temp2758, _temp2758, _temp2758 + 6u}; _temp2757( _temp2759);});}}} struct Cyc_PP_Doc*
Cyc_Absynpp_switchclauses2doc( struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Switch_clause*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc,({
unsigned char* _temp2777=( unsigned char*)""; struct _tagged_string _temp2778;
_temp2778.curr= _temp2777; _temp2778.base= _temp2777; _temp2778.last_plus_one=
_temp2777 + 1; _temp2778;}), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_switchCclause2doc(
struct Cyc_Absyn_SwitchC_clause* c){ struct Cyc_Absyn_SwitchC_clause _temp2781;
struct Cyc_Position_Segment* _temp2782; struct Cyc_Absyn_Stmt* _temp2784; struct
Cyc_Absyn_Exp* _temp2786; struct Cyc_Absyn_SwitchC_clause* _temp2779= c;
_temp2781=* _temp2779; _LL2787: _temp2786= _temp2781.cnst_exp; goto _LL2785;
_LL2785: _temp2784= _temp2781.body; goto _LL2783; _LL2783: _temp2782= _temp2781.loc;
goto _LL2780; _LL2780: if( _temp2786 == 0){ return({ struct Cyc_PP_Doc*(*
_temp2788)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2791= Cyc_PP_text(({
unsigned char* _temp2798=( unsigned char*)"default: "; struct _tagged_string
_temp2799; _temp2799.curr= _temp2798; _temp2799.base= _temp2798; _temp2799.last_plus_one=
_temp2798 + 10; _temp2799;})); struct Cyc_PP_Doc* _temp2792= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc*(* _temp2793)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2796= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2797= Cyc_Absynpp_stmt2doc(
c->body); struct Cyc_PP_Doc* _temp2794[ 2u]={ _temp2796, _temp2797}; struct
_tagged_ptr1 _temp2795={ _temp2794, _temp2794, _temp2794 + 2u}; _temp2793(
_temp2795);})); struct Cyc_PP_Doc* _temp2789[ 2u]={ _temp2791, _temp2792};
struct _tagged_ptr1 _temp2790={ _temp2789, _temp2789, _temp2789 + 2u}; _temp2788(
_temp2790);});} else{ return({ struct Cyc_PP_Doc*(* _temp2800)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2803= Cyc_PP_text(({
unsigned char* _temp2814=( unsigned char*)"case "; struct _tagged_string
_temp2815; _temp2815.curr= _temp2814; _temp2815.base= _temp2814; _temp2815.last_plus_one=
_temp2814 + 6; _temp2815;})); struct Cyc_PP_Doc* _temp2804= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp2786)); struct Cyc_PP_Doc* _temp2805=
Cyc_PP_text(({ unsigned char* _temp2812=( unsigned char*)": "; struct
_tagged_string _temp2813; _temp2813.curr= _temp2812; _temp2813.base= _temp2812;
_temp2813.last_plus_one= _temp2812 + 3; _temp2813;})); struct Cyc_PP_Doc*
_temp2806= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2807)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2810= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2811= Cyc_Absynpp_stmt2doc( _temp2784); struct Cyc_PP_Doc*
_temp2808[ 2u]={ _temp2810, _temp2811}; struct _tagged_ptr1 _temp2809={
_temp2808, _temp2808, _temp2808 + 2u}; _temp2807( _temp2809);})); struct Cyc_PP_Doc*
_temp2801[ 4u]={ _temp2803, _temp2804, _temp2805, _temp2806}; struct
_tagged_ptr1 _temp2802={ _temp2801, _temp2801, _temp2801 + 4u}; _temp2800(
_temp2802);});}} struct Cyc_PP_Doc* Cyc_Absynpp_switchCclauses2doc( struct Cyc_List_List*
cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_SwitchC_clause*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchCclause2doc,({
unsigned char* _temp2816=( unsigned char*)""; struct _tagged_string _temp2817;
_temp2817.curr= _temp2816; _temp2817.base= _temp2816; _temp2817.last_plus_one=
_temp2816 + 1; _temp2817;}), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_enumfield2doc(
struct Cyc_Absyn_Enumfield* f){ if( f->tag == 0){ return Cyc_Absynpp_qvar2doc( f->name);}
else{ return({ struct Cyc_PP_Doc*(* _temp2818)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2821= Cyc_Absynpp_qvar2doc( f->name); struct Cyc_PP_Doc*
_temp2822= Cyc_PP_text(({ unsigned char* _temp2824=( unsigned char*)" = ";
struct _tagged_string _temp2825; _temp2825.curr= _temp2824; _temp2825.base=
_temp2824; _temp2825.last_plus_one= _temp2824 + 4; _temp2825;})); struct Cyc_PP_Doc*
_temp2823= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( f->tag));
struct Cyc_PP_Doc* _temp2819[ 3u]={ _temp2821, _temp2822, _temp2823}; struct
_tagged_ptr1 _temp2820={ _temp2819, _temp2819, _temp2819 + 3u}; _temp2818(
_temp2820);});}} struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc( struct Cyc_List_List*
fs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Enumfield*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc,({
unsigned char* _temp2826=( unsigned char*)","; struct _tagged_string _temp2827;
_temp2827.curr= _temp2826; _temp2827.base= _temp2826; _temp2827.last_plus_one=
_temp2826 + 2; _temp2827;}), fs);} static struct Cyc_PP_Doc* Cyc_Absynpp_id2doc(
struct Cyc_Absyn_Vardecl* v){ return Cyc_Absynpp_qvar2doc( v->name);} static
struct Cyc_PP_Doc* Cyc_Absynpp_ids2doc( struct Cyc_List_List* vds){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Vardecl*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseq)( Cyc_Absynpp_id2doc,({
unsigned char* _temp2828=( unsigned char*)","; struct _tagged_string _temp2829;
_temp2829.curr= _temp2828; _temp2829.base= _temp2828; _temp2829.last_plus_one=
_temp2828 + 2; _temp2829;}), vds);} struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl* d){ struct Cyc_PP_Doc* s;{ void* _temp2830=( void*) d->r;
struct Cyc_Absyn_Fndecl* _temp2856; struct Cyc_Absyn_Structdecl* _temp2858;
struct Cyc_Absyn_Uniondecl* _temp2860; struct Cyc_Absyn_Vardecl* _temp2862;
struct Cyc_Absyn_Vardecl _temp2864; struct Cyc_List_List* _temp2865; struct Cyc_Core_Opt*
_temp2867; struct Cyc_Absyn_Exp* _temp2869; void* _temp2871; struct Cyc_Absyn_Tqual
_temp2873; struct _tuple0* _temp2875; void* _temp2877; struct Cyc_Absyn_Tuniondecl*
_temp2879; struct Cyc_Absyn_Tuniondecl _temp2881; int _temp2882; struct Cyc_Core_Opt*
_temp2884; struct Cyc_List_List* _temp2886; struct _tuple0* _temp2888; void*
_temp2890; int _temp2892; struct Cyc_Absyn_Exp* _temp2894; struct Cyc_Core_Opt*
_temp2896; struct Cyc_Core_Opt* _temp2898; struct Cyc_Absyn_Pat* _temp2900;
struct Cyc_List_List* _temp2902; struct Cyc_Absyn_Enumdecl* _temp2904; struct
Cyc_Absyn_Enumdecl _temp2906; struct Cyc_Core_Opt* _temp2907; struct _tuple0*
_temp2909; void* _temp2911; struct Cyc_Absyn_Typedefdecl* _temp2913; struct Cyc_List_List*
_temp2915; struct _tagged_string* _temp2917; struct Cyc_List_List* _temp2919;
struct _tuple0* _temp2921; struct Cyc_List_List* _temp2923; _LL2832: if(*(( int*)
_temp2830) == Cyc_Absyn_Fn_d){ _LL2857: _temp2856=(( struct Cyc_Absyn_Fn_d_struct*)
_temp2830)->f1; goto _LL2833;} else{ goto _LL2834;} _LL2834: if(*(( int*)
_temp2830) == Cyc_Absyn_Struct_d){ _LL2859: _temp2858=(( struct Cyc_Absyn_Struct_d_struct*)
_temp2830)->f1; goto _LL2835;} else{ goto _LL2836;} _LL2836: if(*(( int*)
_temp2830) == Cyc_Absyn_Union_d){ _LL2861: _temp2860=(( struct Cyc_Absyn_Union_d_struct*)
_temp2830)->f1; goto _LL2837;} else{ goto _LL2838;} _LL2838: if(*(( int*)
_temp2830) == Cyc_Absyn_Var_d){ _LL2863: _temp2862=(( struct Cyc_Absyn_Var_d_struct*)
_temp2830)->f1; _temp2864=* _temp2862; _LL2878: _temp2877=( void*) _temp2864.sc;
goto _LL2876; _LL2876: _temp2875= _temp2864.name; goto _LL2874; _LL2874:
_temp2873= _temp2864.tq; goto _LL2872; _LL2872: _temp2871=( void*) _temp2864.type;
goto _LL2870; _LL2870: _temp2869= _temp2864.initializer; goto _LL2868; _LL2868:
_temp2867= _temp2864.rgn; goto _LL2866; _LL2866: _temp2865= _temp2864.attributes;
goto _LL2839;} else{ goto _LL2840;} _LL2840: if(*(( int*) _temp2830) == Cyc_Absyn_Tunion_d){
_LL2880: _temp2879=(( struct Cyc_Absyn_Tunion_d_struct*) _temp2830)->f1;
_temp2881=* _temp2879; _LL2891: _temp2890=( void*) _temp2881.sc; goto _LL2889;
_LL2889: _temp2888= _temp2881.name; goto _LL2887; _LL2887: _temp2886= _temp2881.tvs;
goto _LL2885; _LL2885: _temp2884= _temp2881.fields; goto _LL2883; _LL2883:
_temp2882= _temp2881.is_xtunion; goto _LL2841;} else{ goto _LL2842;} _LL2842:
if(*(( int*) _temp2830) == Cyc_Absyn_Let_d){ _LL2901: _temp2900=(( struct Cyc_Absyn_Let_d_struct*)
_temp2830)->f1; goto _LL2899; _LL2899: _temp2898=(( struct Cyc_Absyn_Let_d_struct*)
_temp2830)->f2; goto _LL2897; _LL2897: _temp2896=(( struct Cyc_Absyn_Let_d_struct*)
_temp2830)->f3; goto _LL2895; _LL2895: _temp2894=(( struct Cyc_Absyn_Let_d_struct*)
_temp2830)->f4; goto _LL2893; _LL2893: _temp2892=(( struct Cyc_Absyn_Let_d_struct*)
_temp2830)->f5; goto _LL2843;} else{ goto _LL2844;} _LL2844: if(*(( int*)
_temp2830) == Cyc_Absyn_Letv_d){ _LL2903: _temp2902=(( struct Cyc_Absyn_Letv_d_struct*)
_temp2830)->f1; goto _LL2845;} else{ goto _LL2846;} _LL2846: if(*(( int*)
_temp2830) == Cyc_Absyn_Enum_d){ _LL2905: _temp2904=(( struct Cyc_Absyn_Enum_d_struct*)
_temp2830)->f1; _temp2906=* _temp2904; _LL2912: _temp2911=( void*) _temp2906.sc;
goto _LL2910; _LL2910: _temp2909= _temp2906.name; goto _LL2908; _LL2908:
_temp2907= _temp2906.fields; goto _LL2847;} else{ goto _LL2848;} _LL2848: if(*((
int*) _temp2830) == Cyc_Absyn_Typedef_d){ _LL2914: _temp2913=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2830)->f1; goto _LL2849;} else{ goto _LL2850;} _LL2850: if(*(( int*)
_temp2830) == Cyc_Absyn_Namespace_d){ _LL2918: _temp2917=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2830)->f1; goto _LL2916; _LL2916: _temp2915=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2830)->f2; goto _LL2851;} else{ goto _LL2852;} _LL2852: if(*(( int*)
_temp2830) == Cyc_Absyn_Using_d){ _LL2922: _temp2921=(( struct Cyc_Absyn_Using_d_struct*)
_temp2830)->f1; goto _LL2920; _LL2920: _temp2919=(( struct Cyc_Absyn_Using_d_struct*)
_temp2830)->f2; goto _LL2853;} else{ goto _LL2854;} _LL2854: if(*(( int*)
_temp2830) == Cyc_Absyn_ExternC_d){ _LL2924: _temp2923=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2830)->f1; goto _LL2855;} else{ goto _LL2831;} _LL2833: { void* t=( void*)({
struct Cyc_Absyn_FnType_struct* _temp2964=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2964[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2965; _temp2965.tag= Cyc_Absyn_FnType; _temp2965.f1=({ struct Cyc_Absyn_FnInfo
_temp2966; _temp2966.tvars= _temp2856->tvs; _temp2966.effect= _temp2856->effect;
_temp2966.ret_typ=( void*)(( void*) _temp2856->ret_type); _temp2966.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp2856->args); _temp2966.c_varargs=
_temp2856->c_varargs; _temp2966.cyc_varargs= _temp2856->cyc_varargs; _temp2966.rgn_po=
_temp2856->rgn_po; _temp2966.attributes= 0; _temp2966;}); _temp2965;});
_temp2964;}); struct Cyc_PP_Doc* attsdoc= Cyc_Absynpp_atts2doc( _temp2856->attributes);
struct Cyc_PP_Doc* inlinedoc; if( _temp2856->is_inline){ if( Cyc_Absynpp_to_VC){
inlinedoc= Cyc_PP_text(({ unsigned char* _temp2925=( unsigned char*)"__inline ";
struct _tagged_string _temp2926; _temp2926.curr= _temp2925; _temp2926.base=
_temp2925; _temp2926.last_plus_one= _temp2925 + 10; _temp2926;}));} else{
inlinedoc= Cyc_PP_text(({ unsigned char* _temp2927=( unsigned char*)"inline ";
struct _tagged_string _temp2928; _temp2928.curr= _temp2927; _temp2928.base=
_temp2927; _temp2928.last_plus_one= _temp2927 + 8; _temp2928;}));}} else{
inlinedoc= Cyc_PP_nil_doc();}{ struct Cyc_PP_Doc* scopedoc= Cyc_Absynpp_scope2doc((
void*) _temp2856->sc); struct Cyc_PP_Doc* beforenamedoc= Cyc_Absynpp_to_VC? Cyc_Absynpp_callconv2doc(
_temp2856->attributes): attsdoc; struct Cyc_PP_Doc* namedoc= Cyc_Absynpp_typedef_name2doc(
_temp2856->name); struct Cyc_PP_Doc* tqtddoc= Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual
_temp2957; _temp2957.q_const= 0; _temp2957.q_volatile= 0; _temp2957.q_restrict=
0; _temp2957;}), t,({ struct Cyc_Core_Opt* _temp2958=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2958->v=( void*)({ struct Cyc_PP_Doc*(*
_temp2959)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2962=
beforenamedoc; struct Cyc_PP_Doc* _temp2963= namedoc; struct Cyc_PP_Doc*
_temp2960[ 2u]={ _temp2962, _temp2963}; struct _tagged_ptr1 _temp2961={
_temp2960, _temp2960, _temp2960 + 2u}; _temp2959( _temp2961);}); _temp2958;}));
struct Cyc_PP_Doc* bodydoc=({ struct Cyc_PP_Doc*(* _temp2941)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2944= Cyc_PP_text(({
unsigned char* _temp2955=( unsigned char*)" {"; struct _tagged_string _temp2956;
_temp2956.curr= _temp2955; _temp2956.base= _temp2955; _temp2956.last_plus_one=
_temp2955 + 3; _temp2956;})); struct Cyc_PP_Doc* _temp2945= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc*(* _temp2950)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2953= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2954= Cyc_Absynpp_stmt2doc(
_temp2856->body); struct Cyc_PP_Doc* _temp2951[ 2u]={ _temp2953, _temp2954};
struct _tagged_ptr1 _temp2952={ _temp2951, _temp2951, _temp2951 + 2u}; _temp2950(
_temp2952);})); struct Cyc_PP_Doc* _temp2946= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2947= Cyc_PP_text(({ unsigned char* _temp2948=( unsigned char*)"}"; struct
_tagged_string _temp2949; _temp2949.curr= _temp2948; _temp2949.base= _temp2948;
_temp2949.last_plus_one= _temp2948 + 2; _temp2949;})); struct Cyc_PP_Doc*
_temp2942[ 4u]={ _temp2944, _temp2945, _temp2946, _temp2947}; struct
_tagged_ptr1 _temp2943={ _temp2942, _temp2942, _temp2942 + 4u}; _temp2941(
_temp2943);}); s=({ struct Cyc_PP_Doc*(* _temp2929)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2932= inlinedoc; struct Cyc_PP_Doc* _temp2933= scopedoc;
struct Cyc_PP_Doc* _temp2934= tqtddoc; struct Cyc_PP_Doc* _temp2935= bodydoc;
struct Cyc_PP_Doc* _temp2930[ 4u]={ _temp2932, _temp2933, _temp2934, _temp2935};
struct _tagged_ptr1 _temp2931={ _temp2930, _temp2930, _temp2930 + 4u}; _temp2929(
_temp2931);}); if( Cyc_Absynpp_to_VC){ s=({ struct Cyc_PP_Doc*(* _temp2936)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2939= attsdoc; struct
Cyc_PP_Doc* _temp2940= s; struct Cyc_PP_Doc* _temp2937[ 2u]={ _temp2939,
_temp2940}; struct _tagged_ptr1 _temp2938={ _temp2937, _temp2937, _temp2937 + 2u};
_temp2936( _temp2938);});} goto _LL2831;}} _LL2835: if( _temp2858->fields == 0){
s=({ struct Cyc_PP_Doc*(* _temp2967)( struct _tagged_ptr1)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp2970= Cyc_Absynpp_scope2doc(( void*) _temp2858->sc); struct Cyc_PP_Doc*
_temp2971= Cyc_PP_text(({ unsigned char* _temp2979=( unsigned char*)"struct ";
struct _tagged_string _temp2980; _temp2980.curr= _temp2979; _temp2980.base=
_temp2979; _temp2980.last_plus_one= _temp2979 + 8; _temp2980;})); struct Cyc_PP_Doc*
_temp2972= _temp2858->name == 0? Cyc_PP_text(({ unsigned char* _temp2977=(
unsigned char*)""; struct _tagged_string _temp2978; _temp2978.curr= _temp2977;
_temp2978.base= _temp2977; _temp2978.last_plus_one= _temp2977 + 1; _temp2978;})):
Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp2858->name))->v); struct Cyc_PP_Doc* _temp2973= Cyc_Absynpp_ktvars2doc(
_temp2858->tvs); struct Cyc_PP_Doc* _temp2974= Cyc_PP_text(({ unsigned char*
_temp2975=( unsigned char*)";"; struct _tagged_string _temp2976; _temp2976.curr=
_temp2975; _temp2976.base= _temp2975; _temp2976.last_plus_one= _temp2975 + 2;
_temp2976;})); struct Cyc_PP_Doc* _temp2968[ 5u]={ _temp2970, _temp2971,
_temp2972, _temp2973, _temp2974}; struct _tagged_ptr1 _temp2969={ _temp2968,
_temp2968, _temp2968 + 5u}; _temp2967( _temp2969);});} else{ s=({ struct Cyc_PP_Doc*(*
_temp2981)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2984= Cyc_Absynpp_scope2doc((
void*) _temp2858->sc); struct Cyc_PP_Doc* _temp2985= Cyc_PP_text(({
unsigned char* _temp3007=( unsigned char*)"struct "; struct _tagged_string
_temp3008; _temp3008.curr= _temp3007; _temp3008.base= _temp3007; _temp3008.last_plus_one=
_temp3007 + 8; _temp3008;})); struct Cyc_PP_Doc* _temp2986= _temp2858->name == 0?
Cyc_PP_text(({ unsigned char* _temp3005=( unsigned char*)""; struct
_tagged_string _temp3006; _temp3006.curr= _temp3005; _temp3006.base= _temp3005;
_temp3006.last_plus_one= _temp3005 + 1; _temp3006;})): Cyc_Absynpp_typedef_name2doc((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp2858->name))->v);
struct Cyc_PP_Doc* _temp2987= Cyc_Absynpp_ktvars2doc( _temp2858->tvs); struct
Cyc_PP_Doc* _temp2988= Cyc_PP_text(({ unsigned char* _temp3003=( unsigned char*)" {";
struct _tagged_string _temp3004; _temp3004.curr= _temp3003; _temp3004.base=
_temp3003; _temp3004.last_plus_one= _temp3003 + 3; _temp3004;})); struct Cyc_PP_Doc*
_temp2989= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2998)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3001= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp3002= Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp2858->fields))->v); struct Cyc_PP_Doc*
_temp2999[ 2u]={ _temp3001, _temp3002}; struct _tagged_ptr1 _temp3000={
_temp2999, _temp2999, _temp2999 + 2u}; _temp2998( _temp3000);})); struct Cyc_PP_Doc*
_temp2990= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2991= Cyc_PP_text(({
unsigned char* _temp2996=( unsigned char*)"}"; struct _tagged_string _temp2997;
_temp2997.curr= _temp2996; _temp2997.base= _temp2996; _temp2997.last_plus_one=
_temp2996 + 2; _temp2997;})); struct Cyc_PP_Doc* _temp2992= Cyc_Absynpp_atts2doc(
_temp2858->attributes); struct Cyc_PP_Doc* _temp2993= Cyc_PP_text(({
unsigned char* _temp2994=( unsigned char*)";"; struct _tagged_string _temp2995;
_temp2995.curr= _temp2994; _temp2995.base= _temp2994; _temp2995.last_plus_one=
_temp2994 + 2; _temp2995;})); struct Cyc_PP_Doc* _temp2982[ 10u]={ _temp2984,
_temp2985, _temp2986, _temp2987, _temp2988, _temp2989, _temp2990, _temp2991,
_temp2992, _temp2993}; struct _tagged_ptr1 _temp2983={ _temp2982, _temp2982,
_temp2982 + 10u}; _temp2981( _temp2983);});} goto _LL2831; _LL2837: if(
_temp2860->fields == 0){ s=({ struct Cyc_PP_Doc*(* _temp3009)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3012= Cyc_Absynpp_scope2doc((
void*) _temp2860->sc); struct Cyc_PP_Doc* _temp3013= Cyc_PP_text(({
unsigned char* _temp3021=( unsigned char*)"union "; struct _tagged_string
_temp3022; _temp3022.curr= _temp3021; _temp3022.base= _temp3021; _temp3022.last_plus_one=
_temp3021 + 7; _temp3022;})); struct Cyc_PP_Doc* _temp3014= _temp2860->name == 0?
Cyc_PP_text(({ unsigned char* _temp3019=( unsigned char*)""; struct
_tagged_string _temp3020; _temp3020.curr= _temp3019; _temp3020.base= _temp3019;
_temp3020.last_plus_one= _temp3019 + 1; _temp3020;})): Cyc_Absynpp_qvar2doc((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp2860->name))->v);
struct Cyc_PP_Doc* _temp3015= Cyc_Absynpp_tvars2doc( _temp2860->tvs); struct Cyc_PP_Doc*
_temp3016= Cyc_PP_text(({ unsigned char* _temp3017=( unsigned char*)";"; struct
_tagged_string _temp3018; _temp3018.curr= _temp3017; _temp3018.base= _temp3017;
_temp3018.last_plus_one= _temp3017 + 2; _temp3018;})); struct Cyc_PP_Doc*
_temp3010[ 5u]={ _temp3012, _temp3013, _temp3014, _temp3015, _temp3016}; struct
_tagged_ptr1 _temp3011={ _temp3010, _temp3010, _temp3010 + 5u}; _temp3009(
_temp3011);});} else{ s=({ struct Cyc_PP_Doc*(* _temp3023)( struct _tagged_ptr1)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp3026= Cyc_Absynpp_scope2doc(( void*)
_temp2860->sc); struct Cyc_PP_Doc* _temp3027= Cyc_PP_text(({ unsigned char*
_temp3049=( unsigned char*)"union "; struct _tagged_string _temp3050; _temp3050.curr=
_temp3049; _temp3050.base= _temp3049; _temp3050.last_plus_one= _temp3049 + 7;
_temp3050;})); struct Cyc_PP_Doc* _temp3028= _temp2860->name == 0? Cyc_PP_text(({
unsigned char* _temp3047=( unsigned char*)""; struct _tagged_string _temp3048;
_temp3048.curr= _temp3047; _temp3048.base= _temp3047; _temp3048.last_plus_one=
_temp3047 + 1; _temp3048;})): Cyc_Absynpp_qvar2doc(( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp2860->name))->v); struct Cyc_PP_Doc* _temp3029=
Cyc_Absynpp_tvars2doc( _temp2860->tvs); struct Cyc_PP_Doc* _temp3030= Cyc_PP_text(({
unsigned char* _temp3045=( unsigned char*)" {"; struct _tagged_string _temp3046;
_temp3046.curr= _temp3045; _temp3046.base= _temp3045; _temp3046.last_plus_one=
_temp3045 + 3; _temp3046;})); struct Cyc_PP_Doc* _temp3031= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc*(* _temp3040)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp3043= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3044= Cyc_Absynpp_structfields2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp2860->fields))->v);
struct Cyc_PP_Doc* _temp3041[ 2u]={ _temp3043, _temp3044}; struct _tagged_ptr1
_temp3042={ _temp3041, _temp3041, _temp3041 + 2u}; _temp3040( _temp3042);}));
struct Cyc_PP_Doc* _temp3032= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3033=
Cyc_PP_text(({ unsigned char* _temp3038=( unsigned char*)"}"; struct
_tagged_string _temp3039; _temp3039.curr= _temp3038; _temp3039.base= _temp3038;
_temp3039.last_plus_one= _temp3038 + 2; _temp3039;})); struct Cyc_PP_Doc*
_temp3034= Cyc_Absynpp_atts2doc( _temp2860->attributes); struct Cyc_PP_Doc*
_temp3035= Cyc_PP_text(({ unsigned char* _temp3036=( unsigned char*)";"; struct
_tagged_string _temp3037; _temp3037.curr= _temp3036; _temp3037.base= _temp3036;
_temp3037.last_plus_one= _temp3036 + 2; _temp3037;})); struct Cyc_PP_Doc*
_temp3024[ 10u]={ _temp3026, _temp3027, _temp3028, _temp3029, _temp3030,
_temp3031, _temp3032, _temp3033, _temp3034, _temp3035}; struct _tagged_ptr1
_temp3025={ _temp3024, _temp3024, _temp3024 + 10u}; _temp3023( _temp3025);});}
goto _LL2831; _LL2839: { struct Cyc_PP_Doc* sn= Cyc_Absynpp_typedef_name2doc(
_temp2875); s=({ struct Cyc_PP_Doc*(* _temp3051)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp3054= Cyc_Absynpp_to_VC? Cyc_Absynpp_atts2doc( _temp2865):
Cyc_PP_nil_doc(); struct Cyc_PP_Doc* _temp3055= Cyc_Absynpp_scope2doc( _temp2877);
struct Cyc_PP_Doc* _temp3056= Cyc_Absynpp_tqtd2doc( _temp2873, _temp2871,({
struct Cyc_Core_Opt* _temp3071=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3071->v=( void*) sn; _temp3071;})); struct Cyc_PP_Doc*
_temp3057= ! Cyc_Absynpp_to_VC? Cyc_Absynpp_atts2doc( _temp2865): Cyc_PP_nil_doc();
struct Cyc_PP_Doc* _temp3058= _temp2869 == 0? Cyc_PP_text(({ unsigned char*
_temp3062=( unsigned char*)""; struct _tagged_string _temp3063; _temp3063.curr=
_temp3062; _temp3063.base= _temp3062; _temp3063.last_plus_one= _temp3062 + 1;
_temp3063;})):({ struct Cyc_PP_Doc*(* _temp3064)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp3067= Cyc_PP_text(({ unsigned char* _temp3069=(
unsigned char*)" = "; struct _tagged_string _temp3070; _temp3070.curr= _temp3069;
_temp3070.base= _temp3069; _temp3070.last_plus_one= _temp3069 + 4; _temp3070;}));
struct Cyc_PP_Doc* _temp3068= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)
_check_null( _temp2869)); struct Cyc_PP_Doc* _temp3065[ 2u]={ _temp3067,
_temp3068}; struct _tagged_ptr1 _temp3066={ _temp3065, _temp3065, _temp3065 + 2u};
_temp3064( _temp3066);}); struct Cyc_PP_Doc* _temp3059= Cyc_PP_text(({
unsigned char* _temp3060=( unsigned char*)";"; struct _tagged_string _temp3061;
_temp3061.curr= _temp3060; _temp3061.base= _temp3060; _temp3061.last_plus_one=
_temp3060 + 2; _temp3061;})); struct Cyc_PP_Doc* _temp3052[ 6u]={ _temp3054,
_temp3055, _temp3056, _temp3057, _temp3058, _temp3059}; struct _tagged_ptr1
_temp3053={ _temp3052, _temp3052, _temp3052 + 6u}; _temp3051( _temp3053);});
goto _LL2831;} _LL2841: if( _temp2884 == 0){ s=({ struct Cyc_PP_Doc*(* _temp3072)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3075= Cyc_Absynpp_scope2doc(
_temp2890); struct Cyc_PP_Doc* _temp3076= _temp2882? Cyc_PP_text(({
unsigned char* _temp3082=( unsigned char*)"xtunion "; struct _tagged_string
_temp3083; _temp3083.curr= _temp3082; _temp3083.base= _temp3082; _temp3083.last_plus_one=
_temp3082 + 9; _temp3083;})): Cyc_PP_text(({ unsigned char* _temp3084=(
unsigned char*)"tunion "; struct _tagged_string _temp3085; _temp3085.curr=
_temp3084; _temp3085.base= _temp3084; _temp3085.last_plus_one= _temp3084 + 8;
_temp3085;})); struct Cyc_PP_Doc* _temp3077= _temp2882? Cyc_Absynpp_qvar2doc(
_temp2888): Cyc_Absynpp_typedef_name2doc( _temp2888); struct Cyc_PP_Doc*
_temp3078= Cyc_Absynpp_ktvars2doc( _temp2886); struct Cyc_PP_Doc* _temp3079= Cyc_PP_text(({
unsigned char* _temp3080=( unsigned char*)";"; struct _tagged_string _temp3081;
_temp3081.curr= _temp3080; _temp3081.base= _temp3080; _temp3081.last_plus_one=
_temp3080 + 2; _temp3081;})); struct Cyc_PP_Doc* _temp3073[ 5u]={ _temp3075,
_temp3076, _temp3077, _temp3078, _temp3079}; struct _tagged_ptr1 _temp3074={
_temp3073, _temp3073, _temp3073 + 5u}; _temp3072( _temp3074);});} else{ s=({
struct Cyc_PP_Doc*(* _temp3086)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp3089= Cyc_Absynpp_scope2doc( _temp2890); struct Cyc_PP_Doc* _temp3090=
_temp2882? Cyc_PP_text(({ unsigned char* _temp3106=( unsigned char*)"xtunion ";
struct _tagged_string _temp3107; _temp3107.curr= _temp3106; _temp3107.base=
_temp3106; _temp3107.last_plus_one= _temp3106 + 9; _temp3107;})): Cyc_PP_text(({
unsigned char* _temp3108=( unsigned char*)"tunion "; struct _tagged_string
_temp3109; _temp3109.curr= _temp3108; _temp3109.base= _temp3108; _temp3109.last_plus_one=
_temp3108 + 8; _temp3109;})); struct Cyc_PP_Doc* _temp3091= _temp2882? Cyc_Absynpp_qvar2doc(
_temp2888): Cyc_Absynpp_typedef_name2doc( _temp2888); struct Cyc_PP_Doc*
_temp3092= Cyc_Absynpp_ktvars2doc( _temp2886); struct Cyc_PP_Doc* _temp3093= Cyc_PP_text(({
unsigned char* _temp3104=( unsigned char*)" {"; struct _tagged_string _temp3105;
_temp3105.curr= _temp3104; _temp3105.base= _temp3104; _temp3105.last_plus_one=
_temp3104 + 3; _temp3105;})); struct Cyc_PP_Doc* _temp3094= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc*(* _temp3099)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp3102= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3103= Cyc_Absynpp_tunionfields2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp2884))->v);
struct Cyc_PP_Doc* _temp3100[ 2u]={ _temp3102, _temp3103}; struct _tagged_ptr1
_temp3101={ _temp3100, _temp3100, _temp3100 + 2u}; _temp3099( _temp3101);}));
struct Cyc_PP_Doc* _temp3095= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3096=
Cyc_PP_text(({ unsigned char* _temp3097=( unsigned char*)"};"; struct
_tagged_string _temp3098; _temp3098.curr= _temp3097; _temp3098.base= _temp3097;
_temp3098.last_plus_one= _temp3097 + 3; _temp3098;})); struct Cyc_PP_Doc*
_temp3087[ 8u]={ _temp3089, _temp3090, _temp3091, _temp3092, _temp3093,
_temp3094, _temp3095, _temp3096}; struct _tagged_ptr1 _temp3088={ _temp3087,
_temp3087, _temp3087 + 8u}; _temp3086( _temp3088);});} goto _LL2831; _LL2843: s=({
struct Cyc_PP_Doc*(* _temp3110)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp3113= Cyc_PP_text(({ unsigned char* _temp3122=( unsigned char*)"let ";
struct _tagged_string _temp3123; _temp3123.curr= _temp3122; _temp3123.base=
_temp3122; _temp3123.last_plus_one= _temp3122 + 5; _temp3123;})); struct Cyc_PP_Doc*
_temp3114= Cyc_Absynpp_pat2doc( _temp2900); struct Cyc_PP_Doc* _temp3115= Cyc_PP_text(({
unsigned char* _temp3120=( unsigned char*)" = "; struct _tagged_string _temp3121;
_temp3121.curr= _temp3120; _temp3121.base= _temp3120; _temp3121.last_plus_one=
_temp3120 + 4; _temp3121;})); struct Cyc_PP_Doc* _temp3116= Cyc_Absynpp_exp2doc(
_temp2894); struct Cyc_PP_Doc* _temp3117= Cyc_PP_text(({ unsigned char*
_temp3118=( unsigned char*)";"; struct _tagged_string _temp3119; _temp3119.curr=
_temp3118; _temp3119.base= _temp3118; _temp3119.last_plus_one= _temp3118 + 2;
_temp3119;})); struct Cyc_PP_Doc* _temp3111[ 5u]={ _temp3113, _temp3114,
_temp3115, _temp3116, _temp3117}; struct _tagged_ptr1 _temp3112={ _temp3111,
_temp3111, _temp3111 + 5u}; _temp3110( _temp3112);}); goto _LL2831; _LL2845: s=({
struct Cyc_PP_Doc*(* _temp3124)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp3127= Cyc_PP_text(({ unsigned char* _temp3132=( unsigned char*)"let ";
struct _tagged_string _temp3133; _temp3133.curr= _temp3132; _temp3133.base=
_temp3132; _temp3133.last_plus_one= _temp3132 + 5; _temp3133;})); struct Cyc_PP_Doc*
_temp3128= Cyc_Absynpp_ids2doc( _temp2902); struct Cyc_PP_Doc* _temp3129= Cyc_PP_text(({
unsigned char* _temp3130=( unsigned char*)";"; struct _tagged_string _temp3131;
_temp3131.curr= _temp3130; _temp3131.base= _temp3130; _temp3131.last_plus_one=
_temp3130 + 2; _temp3131;})); struct Cyc_PP_Doc* _temp3125[ 3u]={ _temp3127,
_temp3128, _temp3129}; struct _tagged_ptr1 _temp3126={ _temp3125, _temp3125,
_temp3125 + 3u}; _temp3124( _temp3126);}); goto _LL2831; _LL2847: if( _temp2907
== 0){ s=({ struct Cyc_PP_Doc*(* _temp3134)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp3137= Cyc_Absynpp_scope2doc( _temp2911); struct Cyc_PP_Doc*
_temp3138= Cyc_PP_text(({ unsigned char* _temp3143=( unsigned char*)"enum ";
struct _tagged_string _temp3144; _temp3144.curr= _temp3143; _temp3144.base=
_temp3143; _temp3144.last_plus_one= _temp3143 + 6; _temp3144;})); struct Cyc_PP_Doc*
_temp3139= Cyc_Absynpp_typedef_name2doc( _temp2909); struct Cyc_PP_Doc*
_temp3140= Cyc_PP_text(({ unsigned char* _temp3141=( unsigned char*)";"; struct
_tagged_string _temp3142; _temp3142.curr= _temp3141; _temp3142.base= _temp3141;
_temp3142.last_plus_one= _temp3141 + 2; _temp3142;})); struct Cyc_PP_Doc*
_temp3135[ 4u]={ _temp3137, _temp3138, _temp3139, _temp3140}; struct
_tagged_ptr1 _temp3136={ _temp3135, _temp3135, _temp3135 + 4u}; _temp3134(
_temp3136);});} else{ s=({ struct Cyc_PP_Doc*(* _temp3145)( struct _tagged_ptr1)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp3148= Cyc_Absynpp_scope2doc( _temp2911);
struct Cyc_PP_Doc* _temp3149= Cyc_PP_text(({ unsigned char* _temp3164=(
unsigned char*)"enum "; struct _tagged_string _temp3165; _temp3165.curr=
_temp3164; _temp3165.base= _temp3164; _temp3165.last_plus_one= _temp3164 + 6;
_temp3165;})); struct Cyc_PP_Doc* _temp3150= Cyc_Absynpp_qvar2doc( _temp2909);
struct Cyc_PP_Doc* _temp3151= Cyc_PP_text(({ unsigned char* _temp3162=(
unsigned char*)" {"; struct _tagged_string _temp3163; _temp3163.curr= _temp3162;
_temp3163.base= _temp3162; _temp3163.last_plus_one= _temp3162 + 3; _temp3163;}));
struct Cyc_PP_Doc* _temp3152= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp3157)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3160= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp3161= Cyc_Absynpp_enumfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp2907))->v); struct Cyc_PP_Doc* _temp3158[
2u]={ _temp3160, _temp3161}; struct _tagged_ptr1 _temp3159={ _temp3158,
_temp3158, _temp3158 + 2u}; _temp3157( _temp3159);})); struct Cyc_PP_Doc*
_temp3153= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3154= Cyc_PP_text(({
unsigned char* _temp3155=( unsigned char*)"};"; struct _tagged_string _temp3156;
_temp3156.curr= _temp3155; _temp3156.base= _temp3155; _temp3156.last_plus_one=
_temp3155 + 3; _temp3156;})); struct Cyc_PP_Doc* _temp3146[ 7u]={ _temp3148,
_temp3149, _temp3150, _temp3151, _temp3152, _temp3153, _temp3154}; struct
_tagged_ptr1 _temp3147={ _temp3146, _temp3146, _temp3146 + 7u}; _temp3145(
_temp3147);});} goto _LL2831; _LL2849: s=({ struct Cyc_PP_Doc*(* _temp3166)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3169= Cyc_PP_text(({
unsigned char* _temp3181=( unsigned char*)"typedef "; struct _tagged_string
_temp3182; _temp3182.curr= _temp3181; _temp3182.base= _temp3181; _temp3182.last_plus_one=
_temp3181 + 9; _temp3182;})); struct Cyc_PP_Doc* _temp3170= Cyc_Absynpp_tqtd2doc(({
struct Cyc_Absyn_Tqual _temp3174; _temp3174.q_const= 0; _temp3174.q_volatile= 0;
_temp3174.q_restrict= 0; _temp3174;}),( void*) _temp2913->defn,({ struct Cyc_Core_Opt*
_temp3175=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3175->v=( void*)({ struct Cyc_PP_Doc*(* _temp3176)( struct _tagged_ptr1)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp3179= Cyc_Absynpp_typedef_name2doc(
_temp2913->name); struct Cyc_PP_Doc* _temp3180= Cyc_Absynpp_tvars2doc( _temp2913->tvs);
struct Cyc_PP_Doc* _temp3177[ 2u]={ _temp3179, _temp3180}; struct _tagged_ptr1
_temp3178={ _temp3177, _temp3177, _temp3177 + 2u}; _temp3176( _temp3178);});
_temp3175;})); struct Cyc_PP_Doc* _temp3171= Cyc_PP_text(({ unsigned char*
_temp3172=( unsigned char*)";"; struct _tagged_string _temp3173; _temp3173.curr=
_temp3172; _temp3173.base= _temp3172; _temp3173.last_plus_one= _temp3172 + 2;
_temp3173;})); struct Cyc_PP_Doc* _temp3167[ 3u]={ _temp3169, _temp3170,
_temp3171}; struct _tagged_ptr1 _temp3168={ _temp3167, _temp3167, _temp3167 + 3u};
_temp3166( _temp3168);}); goto _LL2831; _LL2851: if( ! Cyc_Absynpp_qvar_to_Cids){
Cyc_Absynpp_curr_namespace_add( _temp2917);} s=({ struct Cyc_PP_Doc*(* _temp3183)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3186= Cyc_PP_text(({
unsigned char* _temp3199=( unsigned char*)"namespace "; struct _tagged_string
_temp3200; _temp3200.curr= _temp3199; _temp3200.base= _temp3199; _temp3200.last_plus_one=
_temp3199 + 11; _temp3200;})); struct Cyc_PP_Doc* _temp3187= Cyc_PP_textptr(
_temp2917); struct Cyc_PP_Doc* _temp3188= Cyc_PP_text(({ unsigned char*
_temp3197=( unsigned char*)" {"; struct _tagged_string _temp3198; _temp3198.curr=
_temp3197; _temp3198.base= _temp3197; _temp3198.last_plus_one= _temp3197 + 3;
_temp3198;})); struct Cyc_PP_Doc* _temp3189= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp3190=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({
unsigned char* _temp3195=( unsigned char*)""; struct _tagged_string _temp3196;
_temp3196.curr= _temp3195; _temp3196.base= _temp3195; _temp3196.last_plus_one=
_temp3195 + 1; _temp3196;}), _temp2915); struct Cyc_PP_Doc* _temp3191= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp3192= Cyc_PP_text(({ unsigned char* _temp3193=(
unsigned char*)"}"; struct _tagged_string _temp3194; _temp3194.curr= _temp3193;
_temp3194.base= _temp3193; _temp3194.last_plus_one= _temp3193 + 2; _temp3194;}));
struct Cyc_PP_Doc* _temp3184[ 7u]={ _temp3186, _temp3187, _temp3188, _temp3189,
_temp3190, _temp3191, _temp3192}; struct _tagged_ptr1 _temp3185={ _temp3184,
_temp3184, _temp3184 + 7u}; _temp3183( _temp3185);}); if( ! Cyc_Absynpp_qvar_to_Cids){
Cyc_Absynpp_curr_namespace_drop();} goto _LL2831; _LL2853: if( Cyc_Absynpp_print_using_stmts){
s=({ struct Cyc_PP_Doc*(* _temp3201)( struct _tagged_ptr1)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp3204= Cyc_PP_text(({ unsigned char* _temp3217=( unsigned char*)"using ";
struct _tagged_string _temp3218; _temp3218.curr= _temp3217; _temp3218.base=
_temp3217; _temp3218.last_plus_one= _temp3217 + 7; _temp3218;})); struct Cyc_PP_Doc*
_temp3205= Cyc_Absynpp_qvar2doc( _temp2921); struct Cyc_PP_Doc* _temp3206= Cyc_PP_text(({
unsigned char* _temp3215=( unsigned char*)" {"; struct _tagged_string _temp3216;
_temp3216.curr= _temp3215; _temp3216.base= _temp3215; _temp3216.last_plus_one=
_temp3215 + 3; _temp3216;})); struct Cyc_PP_Doc* _temp3207= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp3208=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)(
struct Cyc_Absyn_Decl*), struct _tagged_string sep, struct Cyc_List_List* l0))
Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({ unsigned char* _temp3213=( unsigned char*)"";
struct _tagged_string _temp3214; _temp3214.curr= _temp3213; _temp3214.base=
_temp3213; _temp3214.last_plus_one= _temp3213 + 1; _temp3214;}), _temp2919);
struct Cyc_PP_Doc* _temp3209= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3210=
Cyc_PP_text(({ unsigned char* _temp3211=( unsigned char*)"}"; struct
_tagged_string _temp3212; _temp3212.curr= _temp3211; _temp3212.base= _temp3211;
_temp3212.last_plus_one= _temp3211 + 2; _temp3212;})); struct Cyc_PP_Doc*
_temp3202[ 7u]={ _temp3204, _temp3205, _temp3206, _temp3207, _temp3208,
_temp3209, _temp3210}; struct _tagged_ptr1 _temp3203={ _temp3202, _temp3202,
_temp3202 + 7u}; _temp3201( _temp3203);});} else{ s=({ struct Cyc_PP_Doc*(*
_temp3219)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3222= Cyc_PP_text(({
unsigned char* _temp3235=( unsigned char*)"/* using "; struct _tagged_string
_temp3236; _temp3236.curr= _temp3235; _temp3236.base= _temp3235; _temp3236.last_plus_one=
_temp3235 + 10; _temp3236;})); struct Cyc_PP_Doc* _temp3223= Cyc_Absynpp_qvar2doc(
_temp2921); struct Cyc_PP_Doc* _temp3224= Cyc_PP_text(({ unsigned char*
_temp3233=( unsigned char*)" { */"; struct _tagged_string _temp3234; _temp3234.curr=
_temp3233; _temp3234.base= _temp3233; _temp3234.last_plus_one= _temp3233 + 6;
_temp3234;})); struct Cyc_PP_Doc* _temp3225= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp3226=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({
unsigned char* _temp3231=( unsigned char*)""; struct _tagged_string _temp3232;
_temp3232.curr= _temp3231; _temp3232.base= _temp3231; _temp3232.last_plus_one=
_temp3231 + 1; _temp3232;}), _temp2919); struct Cyc_PP_Doc* _temp3227= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp3228= Cyc_PP_text(({ unsigned char* _temp3229=(
unsigned char*)"/* } */"; struct _tagged_string _temp3230; _temp3230.curr=
_temp3229; _temp3230.base= _temp3229; _temp3230.last_plus_one= _temp3229 + 8;
_temp3230;})); struct Cyc_PP_Doc* _temp3220[ 7u]={ _temp3222, _temp3223,
_temp3224, _temp3225, _temp3226, _temp3227, _temp3228}; struct _tagged_ptr1
_temp3221={ _temp3220, _temp3220, _temp3220 + 7u}; _temp3219( _temp3221);});}
goto _LL2831; _LL2855: if( Cyc_Absynpp_print_externC_stmts){ s=({ struct Cyc_PP_Doc*(*
_temp3237)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3240= Cyc_PP_text(({
unsigned char* _temp3249=( unsigned char*)"extern \"C\" {"; struct
_tagged_string _temp3250; _temp3250.curr= _temp3249; _temp3250.base= _temp3249;
_temp3250.last_plus_one= _temp3249 + 13; _temp3250;})); struct Cyc_PP_Doc*
_temp3241= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3242=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({ unsigned char*
_temp3247=( unsigned char*)""; struct _tagged_string _temp3248; _temp3248.curr=
_temp3247; _temp3248.base= _temp3247; _temp3248.last_plus_one= _temp3247 + 1;
_temp3248;}), _temp2923); struct Cyc_PP_Doc* _temp3243= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp3244= Cyc_PP_text(({ unsigned char* _temp3245=(
unsigned char*)"}"; struct _tagged_string _temp3246; _temp3246.curr= _temp3245;
_temp3246.base= _temp3245; _temp3246.last_plus_one= _temp3245 + 2; _temp3246;}));
struct Cyc_PP_Doc* _temp3238[ 5u]={ _temp3240, _temp3241, _temp3242, _temp3243,
_temp3244}; struct _tagged_ptr1 _temp3239={ _temp3238, _temp3238, _temp3238 + 5u};
_temp3237( _temp3239);});} else{ s=({ struct Cyc_PP_Doc*(* _temp3251)( struct
_tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3254= Cyc_PP_text(({
unsigned char* _temp3263=( unsigned char*)"/* extern \"C\" { */"; struct
_tagged_string _temp3264; _temp3264.curr= _temp3263; _temp3264.base= _temp3263;
_temp3264.last_plus_one= _temp3263 + 19; _temp3264;})); struct Cyc_PP_Doc*
_temp3255= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3256=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({ unsigned char*
_temp3261=( unsigned char*)""; struct _tagged_string _temp3262; _temp3262.curr=
_temp3261; _temp3262.base= _temp3261; _temp3262.last_plus_one= _temp3261 + 1;
_temp3262;}), _temp2923); struct Cyc_PP_Doc* _temp3257= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp3258= Cyc_PP_text(({ unsigned char* _temp3259=(
unsigned char*)"/* } */"; struct _tagged_string _temp3260; _temp3260.curr=
_temp3259; _temp3260.base= _temp3259; _temp3260.last_plus_one= _temp3259 + 8;
_temp3260;})); struct Cyc_PP_Doc* _temp3252[ 5u]={ _temp3254, _temp3255,
_temp3256, _temp3257, _temp3258}; struct _tagged_ptr1 _temp3253={ _temp3252,
_temp3252, _temp3252 + 5u}; _temp3251( _temp3253);});} goto _LL2831; _LL2831:;}
return s;} struct Cyc_PP_Doc* Cyc_Absynpp_scope2doc( void* sc){ void* _temp3265=
sc; _LL3267: if( _temp3265 ==( void*) Cyc_Absyn_Static){ goto _LL3268;} else{
goto _LL3269;} _LL3269: if( _temp3265 ==( void*) Cyc_Absyn_Public){ goto _LL3270;}
else{ goto _LL3271;} _LL3271: if( _temp3265 ==( void*) Cyc_Absyn_Extern){ goto
_LL3272;} else{ goto _LL3273;} _LL3273: if( _temp3265 ==( void*) Cyc_Absyn_ExternC){
goto _LL3274;} else{ goto _LL3275;} _LL3275: if( _temp3265 ==( void*) Cyc_Absyn_Abstract){
goto _LL3276;} else{ goto _LL3266;} _LL3268: return Cyc_PP_text(({ unsigned char*
_temp3277=( unsigned char*)"static "; struct _tagged_string _temp3278; _temp3278.curr=
_temp3277; _temp3278.base= _temp3277; _temp3278.last_plus_one= _temp3277 + 8;
_temp3278;})); _LL3270: return Cyc_PP_nil_doc(); _LL3272: return Cyc_PP_text(({
unsigned char* _temp3279=( unsigned char*)"extern "; struct _tagged_string
_temp3280; _temp3280.curr= _temp3279; _temp3280.base= _temp3279; _temp3280.last_plus_one=
_temp3279 + 8; _temp3280;})); _LL3274: return Cyc_PP_text(({ unsigned char*
_temp3281=( unsigned char*)"extern \"C\" "; struct _tagged_string _temp3282;
_temp3282.curr= _temp3281; _temp3282.base= _temp3281; _temp3282.last_plus_one=
_temp3281 + 12; _temp3282;})); _LL3276: return Cyc_PP_text(({ unsigned char*
_temp3283=( unsigned char*)"abstract "; struct _tagged_string _temp3284;
_temp3284.curr= _temp3283; _temp3284.base= _temp3283; _temp3284.last_plus_one=
_temp3283 + 10; _temp3284;})); _LL3266:;} int Cyc_Absynpp_exists_temp_tvar_in_effect(
void* t){ void* _temp3285= t; struct Cyc_Absyn_Tvar* _temp3293; struct Cyc_List_List*
_temp3295; _LL3287: if(( unsigned int) _temp3285 > 4u?*(( int*) _temp3285) ==
Cyc_Absyn_VarType: 0){ _LL3294: _temp3293=(( struct Cyc_Absyn_VarType_struct*)
_temp3285)->f1; goto _LL3288;} else{ goto _LL3289;} _LL3289: if(( unsigned int)
_temp3285 > 4u?*(( int*) _temp3285) == Cyc_Absyn_JoinEff: 0){ _LL3296: _temp3295=((
struct Cyc_Absyn_JoinEff_struct*) _temp3285)->f1; goto _LL3290;} else{ goto
_LL3291;} _LL3291: goto _LL3292; _LL3288: return Cyc_Tcutil_is_temp_tvar(
_temp3293); _LL3290: return(( int(*)( int(* pred)( void*), struct Cyc_List_List*
x)) Cyc_List_exists)( Cyc_Absynpp_exists_temp_tvar_in_effect, _temp3295);
_LL3292: return 0; _LL3286:;} struct _tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual
tq, void* t){ void* _temp3297= t; struct Cyc_Absyn_Exp* _temp3311; struct Cyc_Absyn_Tqual
_temp3313; void* _temp3315; struct Cyc_Absyn_PtrInfo _temp3317; struct Cyc_Absyn_Conref*
_temp3319; struct Cyc_Absyn_Tqual _temp3321; struct Cyc_Absyn_Conref* _temp3323;
void* _temp3325; void* _temp3327; struct Cyc_Absyn_FnInfo _temp3329; struct Cyc_List_List*
_temp3331; struct Cyc_List_List* _temp3333; struct Cyc_Absyn_VarargInfo*
_temp3335; int _temp3337; struct Cyc_List_List* _temp3339; void* _temp3341;
struct Cyc_Core_Opt* _temp3343; struct Cyc_List_List* _temp3345; struct Cyc_Core_Opt*
_temp3347; int _temp3349; struct Cyc_Core_Opt* _temp3351; struct Cyc_Core_Opt*
_temp3353; struct Cyc_Core_Opt* _temp3355; struct Cyc_List_List* _temp3357;
struct _tuple0* _temp3359; _LL3299: if(( unsigned int) _temp3297 > 4u?*(( int*)
_temp3297) == Cyc_Absyn_ArrayType: 0){ _LL3316: _temp3315=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3297)->f1; goto _LL3314; _LL3314: _temp3313=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3297)->f2; goto _LL3312; _LL3312: _temp3311=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3297)->f3; goto _LL3300;} else{ goto _LL3301;} _LL3301: if(( unsigned int)
_temp3297 > 4u?*(( int*) _temp3297) == Cyc_Absyn_PointerType: 0){ _LL3318:
_temp3317=(( struct Cyc_Absyn_PointerType_struct*) _temp3297)->f1; _LL3328:
_temp3327=( void*) _temp3317.elt_typ; goto _LL3326; _LL3326: _temp3325=( void*)
_temp3317.rgn_typ; goto _LL3324; _LL3324: _temp3323= _temp3317.nullable; goto
_LL3322; _LL3322: _temp3321= _temp3317.tq; goto _LL3320; _LL3320: _temp3319=
_temp3317.bounds; goto _LL3302;} else{ goto _LL3303;} _LL3303: if(( unsigned int)
_temp3297 > 4u?*(( int*) _temp3297) == Cyc_Absyn_FnType: 0){ _LL3330: _temp3329=((
struct Cyc_Absyn_FnType_struct*) _temp3297)->f1; _LL3346: _temp3345= _temp3329.tvars;
goto _LL3344; _LL3344: _temp3343= _temp3329.effect; goto _LL3342; _LL3342:
_temp3341=( void*) _temp3329.ret_typ; goto _LL3340; _LL3340: _temp3339=
_temp3329.args; goto _LL3338; _LL3338: _temp3337= _temp3329.c_varargs; goto
_LL3336; _LL3336: _temp3335= _temp3329.cyc_varargs; goto _LL3334; _LL3334:
_temp3333= _temp3329.rgn_po; goto _LL3332; _LL3332: _temp3331= _temp3329.attributes;
goto _LL3304;} else{ goto _LL3305;} _LL3305: if(( unsigned int) _temp3297 > 4u?*((
int*) _temp3297) == Cyc_Absyn_Evar: 0){ _LL3354: _temp3353=(( struct Cyc_Absyn_Evar_struct*)
_temp3297)->f1; goto _LL3352; _LL3352: _temp3351=(( struct Cyc_Absyn_Evar_struct*)
_temp3297)->f2; goto _LL3350; _LL3350: _temp3349=(( struct Cyc_Absyn_Evar_struct*)
_temp3297)->f3; goto _LL3348; _LL3348: _temp3347=(( struct Cyc_Absyn_Evar_struct*)
_temp3297)->f4; goto _LL3306;} else{ goto _LL3307;} _LL3307: if(( unsigned int)
_temp3297 > 4u?*(( int*) _temp3297) == Cyc_Absyn_TypedefType: 0){ _LL3360:
_temp3359=(( struct Cyc_Absyn_TypedefType_struct*) _temp3297)->f1; goto _LL3358;
_LL3358: _temp3357=(( struct Cyc_Absyn_TypedefType_struct*) _temp3297)->f2; goto
_LL3356; _LL3356: _temp3355=(( struct Cyc_Absyn_TypedefType_struct*) _temp3297)->f3;
goto _LL3308;} else{ goto _LL3309;} _LL3309: goto _LL3310; _LL3300: { struct Cyc_List_List*
_temp3363; void* _temp3365; struct Cyc_Absyn_Tqual _temp3367; struct _tuple4
_temp3361= Cyc_Absynpp_to_tms( _temp3313, _temp3315); _LL3368: _temp3367=
_temp3361.f1; goto _LL3366; _LL3366: _temp3365= _temp3361.f2; goto _LL3364;
_LL3364: _temp3363= _temp3361.f3; goto _LL3362; _LL3362: { void* tm; if(
_temp3311 == 0){ tm=( void*) Cyc_Absyn_Carray_mod;} else{ tm=( void*)({ struct
Cyc_Absyn_ConstArray_mod_struct* _temp3369=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp3369[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp3370; _temp3370.tag= Cyc_Absyn_ConstArray_mod;
_temp3370.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp3311); _temp3370;});
_temp3369;});} return({ struct _tuple4 _temp3371; _temp3371.f1= _temp3367;
_temp3371.f2= _temp3365; _temp3371.f3=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp3372=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3372->hd=( void*) tm; _temp3372->tl= _temp3363; _temp3372;}); _temp3371;});}}
_LL3302: { struct Cyc_List_List* _temp3375; void* _temp3377; struct Cyc_Absyn_Tqual
_temp3379; struct _tuple4 _temp3373= Cyc_Absynpp_to_tms( _temp3321, _temp3327);
_LL3380: _temp3379= _temp3373.f1; goto _LL3378; _LL3378: _temp3377= _temp3373.f2;
goto _LL3376; _LL3376: _temp3375= _temp3373.f3; goto _LL3374; _LL3374: { void*
ps;{ struct _tuple8 _temp3382=({ struct _tuple8 _temp3381; _temp3381.f1=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp3323))->v; _temp3381.f2=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp3319))->v; _temp3381;}); void* _temp3392;
void* _temp3394; struct Cyc_Absyn_Exp* _temp3396; void* _temp3398; int _temp3400;
void* _temp3402; void* _temp3404; struct Cyc_Absyn_Exp* _temp3406; void*
_temp3408; int _temp3410; void* _temp3412; void* _temp3414; void* _temp3416;
_LL3384: _LL3399: _temp3398= _temp3382.f1; if(( unsigned int) _temp3398 > 1u?*((
int*) _temp3398) == Cyc_Absyn_Eq_constr: 0){ _LL3401: _temp3400=( int)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp3398)->f1; if( _temp3400 == 1){ goto _LL3393;}
else{ goto _LL3386;}} else{ goto _LL3386;} _LL3393: _temp3392= _temp3382.f2; if((
unsigned int) _temp3392 > 1u?*(( int*) _temp3392) == Cyc_Absyn_Eq_constr: 0){
_LL3395: _temp3394=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3392)->f1;
if(( unsigned int) _temp3394 > 1u?*(( int*) _temp3394) == Cyc_Absyn_Upper_b: 0){
_LL3397: _temp3396=(( struct Cyc_Absyn_Upper_b_struct*) _temp3394)->f1; goto
_LL3385;} else{ goto _LL3386;}} else{ goto _LL3386;} _LL3386: _LL3409: _temp3408=
_temp3382.f1; if(( unsigned int) _temp3408 > 1u?*(( int*) _temp3408) == Cyc_Absyn_Eq_constr:
0){ _LL3411: _temp3410=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3408)->f1;
if( _temp3410 == 0){ goto _LL3403;} else{ goto _LL3388;}} else{ goto _LL3388;}
_LL3403: _temp3402= _temp3382.f2; if(( unsigned int) _temp3402 > 1u?*(( int*)
_temp3402) == Cyc_Absyn_Eq_constr: 0){ _LL3405: _temp3404=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3402)->f1; if(( unsigned int) _temp3404 > 1u?*(( int*) _temp3404) == Cyc_Absyn_Upper_b:
0){ _LL3407: _temp3406=(( struct Cyc_Absyn_Upper_b_struct*) _temp3404)->f1; goto
_LL3387;} else{ goto _LL3388;}} else{ goto _LL3388;} _LL3388: _LL3417: _temp3416=
_temp3382.f1; goto _LL3413; _LL3413: _temp3412= _temp3382.f2; if(( unsigned int)
_temp3412 > 1u?*(( int*) _temp3412) == Cyc_Absyn_Eq_constr: 0){ _LL3415:
_temp3414=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3412)->f1; if(
_temp3414 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3389;} else{ goto _LL3390;}}
else{ goto _LL3390;} _LL3390: goto _LL3391; _LL3385: ps=( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp3418=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp3418[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp3419; _temp3419.tag=
Cyc_Absyn_Nullable_ps; _temp3419.f1= _temp3396; _temp3419;}); _temp3418;}); goto
_LL3383; _LL3387: ps=( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp3420=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp3420[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp3421; _temp3421.tag= Cyc_Absyn_NonNullable_ps; _temp3421.f1= _temp3406;
_temp3421;}); _temp3420;}); goto _LL3383; _LL3389: ps=( void*) Cyc_Absyn_TaggedArray_ps;
goto _LL3383; _LL3391: ps=( void*) Cyc_Absyn_TaggedArray_ps; goto _LL3383;
_LL3383:;}{ void* tm=( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp3424=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3424[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp3425; _temp3425.tag=
Cyc_Absyn_Pointer_mod; _temp3425.f1=( void*) ps; _temp3425.f2=( void*) _temp3325;
_temp3425.f3= tq; _temp3425;}); _temp3424;}); return({ struct _tuple4 _temp3422;
_temp3422.f1= _temp3379; _temp3422.f2= _temp3377; _temp3422.f3=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp3423=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3423->hd=( void*) tm; _temp3423->tl= _temp3375;
_temp3423;}); _temp3422;});}}} _LL3304: if( ! Cyc_Absynpp_print_all_tvars){ if(
_temp3343 == 0? 1: Cyc_Absynpp_exists_temp_tvar_in_effect(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3343))->v)){ _temp3343= 0; _temp3345= 0;}} else{ if( ! Cyc_Absynpp_dont_rewrite_temp_tvars){((
void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)(
Cyc_Tcutil_rewrite_temp_tvar, _temp3345);}}{ struct Cyc_List_List* _temp3428;
void* _temp3430; struct Cyc_Absyn_Tqual _temp3432; struct _tuple4 _temp3426= Cyc_Absynpp_to_tms(
Cyc_Absyn_empty_tqual(), _temp3341); _LL3433: _temp3432= _temp3426.f1; goto
_LL3431; _LL3431: _temp3430= _temp3426.f2; goto _LL3429; _LL3429: _temp3428=
_temp3426.f3; goto _LL3427; _LL3427: { struct Cyc_List_List* tms= _temp3428; if(
_temp3331 != 0){ tms=({ struct Cyc_List_List* _temp3434=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3434->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp3435=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp3435[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp3436; _temp3436.tag= Cyc_Absyn_Attributes_mod;
_temp3436.f1= 0; _temp3436.f2= _temp3331; _temp3436;}); _temp3435;})); _temp3434->tl=
tms; _temp3434;});} tms=({ struct Cyc_List_List* _temp3437=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3437->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp3438=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3438[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3439; _temp3439.tag= Cyc_Absyn_Function_mod;
_temp3439.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp3440=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3440[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3441; _temp3441.tag= Cyc_Absyn_WithTypes;
_temp3441.f1= _temp3339; _temp3441.f2= _temp3337; _temp3441.f3= _temp3335;
_temp3441.f4= _temp3343; _temp3441.f5= _temp3333; _temp3441;}); _temp3440;}));
_temp3439;}); _temp3438;})); _temp3437->tl= tms; _temp3437;}); if( _temp3345 !=
0){ tms=({ struct Cyc_List_List* _temp3442=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp3442->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp3443=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp3443[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp3444; _temp3444.tag= Cyc_Absyn_TypeParams_mod; _temp3444.f1= _temp3345;
_temp3444.f2= 0; _temp3444.f3= 1; _temp3444;}); _temp3443;})); _temp3442->tl=
tms; _temp3442;});} return({ struct _tuple4 _temp3445; _temp3445.f1= _temp3432;
_temp3445.f2= _temp3430; _temp3445.f3= tms; _temp3445;});}} _LL3306: if(
_temp3351 == 0){ return({ struct _tuple4 _temp3446; _temp3446.f1= tq; _temp3446.f2=
t; _temp3446.f3= 0; _temp3446;});} else{ return Cyc_Absynpp_to_tms( tq,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp3351))->v);} _LL3308: if( _temp3355 == 0?
1: ! Cyc_Absynpp_expand_typedefs){ return({ struct _tuple4 _temp3447; _temp3447.f1=
tq; _temp3447.f2= t; _temp3447.f3= 0; _temp3447;});} else{ return Cyc_Absynpp_to_tms(
tq,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3355))->v);} _LL3310:
return({ struct _tuple4 _temp3448; _temp3448.f1= tq; _temp3448.f2= t; _temp3448.f3=
0; _temp3448;}); _LL3298:;} struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc( struct Cyc_Absyn_Tqual
tq, void* t, struct Cyc_Core_Opt* dopt){ struct Cyc_List_List* _temp3451; void*
_temp3453; struct Cyc_Absyn_Tqual _temp3455; struct _tuple4 _temp3449= Cyc_Absynpp_to_tms(
tq, t); _LL3456: _temp3455= _temp3449.f1; goto _LL3454; _LL3454: _temp3453=
_temp3449.f2; goto _LL3452; _LL3452: _temp3451= _temp3449.f3; goto _LL3450;
_LL3450: _temp3451=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp3451); if( _temp3451 == 0? dopt == 0: 0){ return({ struct Cyc_PP_Doc*(*
_temp3457)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3460= Cyc_Absynpp_tqual2doc(
_temp3455); struct Cyc_PP_Doc* _temp3461= Cyc_Absynpp_ntyp2doc( _temp3453);
struct Cyc_PP_Doc* _temp3458[ 2u]={ _temp3460, _temp3461}; struct _tagged_ptr1
_temp3459={ _temp3458, _temp3458, _temp3458 + 2u}; _temp3457( _temp3459);});}
else{ return({ struct Cyc_PP_Doc*(* _temp3462)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp3465= Cyc_Absynpp_tqual2doc( _temp3455); struct Cyc_PP_Doc*
_temp3466= Cyc_Absynpp_ntyp2doc( _temp3453); struct Cyc_PP_Doc* _temp3467= Cyc_PP_text(({
unsigned char* _temp3469=( unsigned char*)" "; struct _tagged_string _temp3470;
_temp3470.curr= _temp3469; _temp3470.base= _temp3469; _temp3470.last_plus_one=
_temp3469 + 2; _temp3470;})); struct Cyc_PP_Doc* _temp3468= Cyc_Absynpp_dtms2doc(
dopt == 0? Cyc_PP_nil_doc():( struct Cyc_PP_Doc*)(( struct Cyc_Core_Opt*)
_check_null( dopt))->v, _temp3451); struct Cyc_PP_Doc* _temp3463[ 4u]={
_temp3465, _temp3466, _temp3467, _temp3468}; struct _tagged_ptr1 _temp3464={
_temp3463, _temp3463, _temp3463 + 4u}; _temp3462( _temp3464);});}} struct Cyc_PP_Doc*
Cyc_Absynpp_structfield2doc( struct Cyc_Absyn_Structfield* f){ if( f->width != 0){
return({ struct Cyc_PP_Doc*(* _temp3471)( struct _tagged_ptr1)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp3474= Cyc_Absynpp_tqtd2doc( f->tq,( void*) f->type,({
struct Cyc_Core_Opt* _temp3483=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3483->v=( void*) Cyc_PP_textptr( f->name); _temp3483;}));
struct Cyc_PP_Doc* _temp3475= Cyc_PP_text(({ unsigned char* _temp3481=(
unsigned char*)":"; struct _tagged_string _temp3482; _temp3482.curr= _temp3481;
_temp3482.base= _temp3481; _temp3482.last_plus_one= _temp3481 + 2; _temp3482;}));
struct Cyc_PP_Doc* _temp3476= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( f->width))->v); struct Cyc_PP_Doc* _temp3477=
Cyc_Absynpp_atts2doc( f->attributes); struct Cyc_PP_Doc* _temp3478= Cyc_PP_text(({
unsigned char* _temp3479=( unsigned char*)";"; struct _tagged_string _temp3480;
_temp3480.curr= _temp3479; _temp3480.base= _temp3479; _temp3480.last_plus_one=
_temp3479 + 2; _temp3480;})); struct Cyc_PP_Doc* _temp3472[ 5u]={ _temp3474,
_temp3475, _temp3476, _temp3477, _temp3478}; struct _tagged_ptr1 _temp3473={
_temp3472, _temp3472, _temp3472 + 5u}; _temp3471( _temp3473);});} else{ return({
struct Cyc_PP_Doc*(* _temp3484)( struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp3487= Cyc_Absynpp_tqtd2doc( f->tq,( void*) f->type,({ struct Cyc_Core_Opt*
_temp3492=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3492->v=( void*) Cyc_PP_textptr( f->name); _temp3492;})); struct Cyc_PP_Doc*
_temp3488= Cyc_Absynpp_atts2doc( f->attributes); struct Cyc_PP_Doc* _temp3489=
Cyc_PP_text(({ unsigned char* _temp3490=( unsigned char*)";"; struct
_tagged_string _temp3491; _temp3491.curr= _temp3490; _temp3491.base= _temp3490;
_temp3491.last_plus_one= _temp3490 + 2; _temp3491;})); struct Cyc_PP_Doc*
_temp3485[ 3u]={ _temp3487, _temp3488, _temp3489}; struct _tagged_ptr1 _temp3486={
_temp3485, _temp3485, _temp3485 + 3u}; _temp3484( _temp3486);});}} struct Cyc_PP_Doc*
Cyc_Absynpp_structfields2doc( struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Structfield*), struct _tagged_string
sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_structfield2doc,({
unsigned char* _temp3493=( unsigned char*)""; struct _tagged_string _temp3494;
_temp3494.curr= _temp3493; _temp3494.base= _temp3493; _temp3494.last_plus_one=
_temp3493 + 1; _temp3494;}), fields);} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfield2doc(
struct Cyc_Absyn_Tunionfield* f){ return({ struct Cyc_PP_Doc*(* _temp3495)(
struct _tagged_ptr1)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3498= Cyc_Absynpp_scope2doc((
void*) f->sc); struct Cyc_PP_Doc* _temp3499= Cyc_Absynpp_typedef_name2doc( f->name);
struct Cyc_PP_Doc* _temp3500= f->tvs == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_ktvars2doc(
f->tvs); struct Cyc_PP_Doc* _temp3501= f->typs == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(
f->typs); struct Cyc_PP_Doc* _temp3496[ 4u]={ _temp3498, _temp3499, _temp3500,
_temp3501}; struct _tagged_ptr1 _temp3497={ _temp3496, _temp3496, _temp3496 + 4u};
_temp3495( _temp3497);});} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Tunionfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_tunionfield2doc,({ unsigned char* _temp3502=(
unsigned char*)","; struct _tagged_string _temp3503; _temp3503.curr= _temp3502;
_temp3503.base= _temp3502; _temp3503.last_plus_one= _temp3502 + 2; _temp3503;}),
fields);} void Cyc_Absynpp_decllist2file( struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE*
f){ for( 0; tdl != 0; tdl=(( struct Cyc_List_List*) _check_null( tdl))->tl){ Cyc_PP_file_of_doc(
Cyc_Absynpp_decl2doc(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( tdl))->hd), 72, f); fprintf( sfile_to_file( f),"\n");}} struct
_tagged_string Cyc_Absynpp_decllist2string( struct Cyc_List_List* tdl){ return
Cyc_PP_string_of_doc( Cyc_PP_seql(({ unsigned char* _temp3504=( unsigned char*)"";
struct _tagged_string _temp3505; _temp3505.curr= _temp3504; _temp3505.base=
_temp3504; _temp3505.last_plus_one= _temp3504 + 1; _temp3505;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Decl*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_decl2doc, tdl)), 72);} struct _tagged_string Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp* e){ return Cyc_PP_string_of_doc( Cyc_Absynpp_exp2doc( e),
72);} struct _tagged_string Cyc_Absynpp_stmt2string( struct Cyc_Absyn_Stmt* s){
return Cyc_PP_string_of_doc( Cyc_Absynpp_stmt2doc( s), 72);} struct
_tagged_string Cyc_Absynpp_typ2string( void* t){ Cyc_Absynpp_dont_rewrite_temp_tvars=
1;{ struct _tagged_string _temp3506= Cyc_PP_string_of_doc( Cyc_Absynpp_typ2doc(
t), 72); goto _LL3507; _LL3507: Cyc_Absynpp_dont_rewrite_temp_tvars= 0; return
_temp3506;}} struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple0* v){
return Cyc_PP_string_of_doc( Cyc_Absynpp_qvar2doc( v), 72);} struct
_tagged_string Cyc_Absynpp_prim2string( void* p){ return Cyc_PP_string_of_doc(
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