 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 8u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
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
extern struct _tagged_string Cstring_to_string( unsigned char*); extern int
system( unsigned char*); struct Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[
19u]; extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
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
Cyc_PP_nest( int k, struct Cyc_PP_Doc* d); struct _tagged_ptr0{ struct Cyc_PP_Doc**
curr; struct Cyc_PP_Doc** base; struct Cyc_PP_Doc** last_plus_one; } ; extern
struct Cyc_PP_Doc* Cyc_PP_cat( struct _tagged_ptr0); extern struct Cyc_PP_Doc*
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
attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{ struct _tuple0* name; int
is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion= 0; struct Cyc_Absyn_UnknownTunion_struct{
int tag; struct Cyc_Absyn_UnknownTunionInfo f1; } ; static const int Cyc_Absyn_KnownTunion=
1; struct Cyc_Absyn_KnownTunion_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
} ; struct Cyc_Absyn_TunionInfo{ void* tunion_info; struct Cyc_List_List* targs;
void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{ struct _tuple0*
tunion_name; struct _tuple0* field_name; int is_xtunion; } ; static const int
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
static const int Cyc_Absyn_Deref_e= 18; struct Cyc_Absyn_Deref_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_StructMember_e= 19;
struct Cyc_Absyn_StructMember_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct _tagged_string* f2; } ; static const int Cyc_Absyn_StructArrow_e= 20;
struct Cyc_Absyn_StructArrow_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tagged_string* f2; } ; static const int Cyc_Absyn_Subscript_e= 21; struct Cyc_Absyn_Subscript_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; static const
int Cyc_Absyn_Tuple_e= 22; struct Cyc_Absyn_Tuple_e_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_CompoundLit_e= 23; struct _tuple1{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; struct Cyc_Absyn_CompoundLit_e_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e=
24; struct Cyc_Absyn_Array_e_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_Comprehension_e= 25; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Struct_e= 26; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_AnonStruct_e= 27;
struct Cyc_Absyn_AnonStruct_e_struct{ int tag; void* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Absyn_Tunion_e= 28; struct Cyc_Absyn_Tunion_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ;
static const int Cyc_Absyn_Enum_e= 29; struct Cyc_Absyn_Enum_e_struct{ int tag;
struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
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
cyc_varargs; struct Cyc_Absyn_Stmt* body; struct Cyc_Core_Opt* cached_typ;
struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List* attributes; } ;
struct Cyc_Absyn_Structfield{ struct _tagged_string* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List* attributes; }
; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Tunionfield{ struct _tuple0* name; struct Cyc_List_List* tvs; struct
Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ; struct Cyc_Absyn_Tuniondecl{
void* sc; struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{ struct _tuple0* name;
struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Enumdecl{
void* sc; struct _tuple0* name; struct Cyc_Core_Opt* fields; } ; struct Cyc_Absyn_Typedefdecl{
struct _tuple0* name; struct Cyc_List_List* tvs; void* defn; } ; static const
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
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
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
cyc_varargs, struct Cyc_Core_Opt* effopt); extern struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List* fields); int Cyc_Absynpp_expand_typedefs= 0; int Cyc_Absynpp_qvar_to_Cids=
0; int Cyc_Absynpp_to_VC= 0; int Cyc_Absynpp_decls_first= 0; int Cyc_Absynpp_dont_rewrite_temp_tvars=
0; int Cyc_Absynpp_print_all_tvars= 0; int Cyc_Absynpp_print_all_kinds= 0; int
Cyc_Absynpp_print_using_stmts= 0; int Cyc_Absynpp_print_externC_stmts= 0; int
Cyc_Absynpp_print_full_evars= 0; static struct Cyc_List_List* Cyc_Absynpp_curr_namespace=
0; static void Cyc_Absynpp_curr_namespace_add( struct _tagged_string* v){ Cyc_Absynpp_curr_namespace=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_Absynpp_curr_namespace,({ struct Cyc_List_List* _temp0=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp0->hd=( void*) v; _temp0->tl= 0;
_temp0;}));} static void Cyc_Absynpp_suppr_last( struct Cyc_List_List** l){ if(((
struct Cyc_List_List*) _check_null(* l))->tl == 0){* l= 0;} else{(( void(*)(
struct Cyc_List_List** l)) Cyc_Absynpp_suppr_last)(&(( struct Cyc_List_List*)
_check_null(* l))->tl);}} static void Cyc_Absynpp_curr_namespace_drop(){(( void(*)(
struct Cyc_List_List** l)) Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
struct _tagged_string Cyc_Absynpp_char_escape( unsigned char c){ switch( c){
case '\a': _LL1: return({ unsigned char* _temp3=( unsigned char*)"\\a"; struct
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
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp154= Cyc_PP_textptr(
tv->name); struct Cyc_PP_Doc* _temp155= Cyc_PP_text(({ unsigned char* _temp157=(
unsigned char*)"::"; struct _tagged_string _temp158; _temp158.curr= _temp157;
_temp158.base= _temp157; _temp158.last_plus_one= _temp157 + 3; _temp158;}));
struct Cyc_PP_Doc* _temp156= Cyc_Absynpp_kind2doc( _temp149); struct Cyc_PP_Doc*
_temp152[ 3u]={ _temp154, _temp155, _temp156}; struct _tagged_ptr0 _temp153={
_temp152, _temp152, _temp152 + 3u}; _temp151( _temp153);}); _LL146: return({
struct Cyc_PP_Doc*(* _temp159)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp162= Cyc_PP_textptr( tv->name); struct Cyc_PP_Doc* _temp163= Cyc_PP_text(({
unsigned char* _temp164=( unsigned char*)"/*::?*/"; struct _tagged_string
_temp165; _temp165.curr= _temp164; _temp165.base= _temp164; _temp165.last_plus_one=
_temp164 + 8; _temp165;})); struct Cyc_PP_Doc* _temp160[ 2u]={ _temp162,
_temp163}; struct _tagged_ptr0 _temp161={ _temp160, _temp160, _temp160 + 2u};
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
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp223= Cyc_PP_text(({
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
3u]={ _temp223, _temp224, _temp225}; struct _tagged_ptr0 _temp222={ _temp221,
_temp221, _temp221 + 3u}; _temp220( _temp222);});} struct Cyc_PP_Doc* Cyc_Absynpp_att2doc(
void* a){ return Cyc_PP_text( Cyc_Absyn_attribute2string( a));} struct Cyc_PP_Doc*
Cyc_Absynpp_atts2doc( struct Cyc_List_List* atts){ if( atts == 0){ return Cyc_PP_nil_doc();}
if( Cyc_Absynpp_to_VC){ return Cyc_Absynpp_noncallconv2doc( atts);} return({
struct Cyc_PP_Doc*(* _temp236)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
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
struct Cyc_PP_Doc* _temp237[ 2u]={ _temp239, _temp240}; struct _tagged_ptr0
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
struct Cyc_PP_Doc*(* _temp426)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp429= Cyc_PP_text(({ unsigned char* _temp434=( unsigned char*)"("; struct
_tagged_string _temp435; _temp435.curr= _temp434; _temp435.base= _temp434;
_temp435.last_plus_one= _temp434 + 2; _temp435;})); struct Cyc_PP_Doc* _temp430=
rest; struct Cyc_PP_Doc* _temp431= Cyc_PP_text(({ unsigned char* _temp432=(
unsigned char*)")"; struct _tagged_string _temp433; _temp433.curr= _temp432;
_temp433.base= _temp432; _temp433.last_plus_one= _temp432 + 2; _temp433;}));
struct Cyc_PP_Doc* _temp427[ 3u]={ _temp429, _temp430, _temp431}; struct
_tagged_ptr0 _temp428={ _temp427, _temp427, _temp427 + 3u}; _temp426( _temp428);});
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
_temp295)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp298= rest;
struct Cyc_PP_Doc* _temp299= Cyc_PP_text(({ unsigned char* _temp300=(
unsigned char*)"[]"; struct _tagged_string _temp301; _temp301.curr= _temp300;
_temp301.base= _temp300; _temp301.last_plus_one= _temp300 + 3; _temp301;}));
struct Cyc_PP_Doc* _temp296[ 2u]={ _temp298, _temp299}; struct _tagged_ptr0
_temp297={ _temp296, _temp296, _temp296 + 2u}; _temp295( _temp297);}); _LL266:
if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*) _check_null( tms))->tl)){
rest= p_rest;} return({ struct Cyc_PP_Doc*(* _temp302)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp305= rest; struct Cyc_PP_Doc* _temp306= Cyc_PP_text(({
unsigned char* _temp311=( unsigned char*)"["; struct _tagged_string _temp312;
_temp312.curr= _temp311; _temp312.base= _temp311; _temp312.last_plus_one=
_temp311 + 2; _temp312;})); struct Cyc_PP_Doc* _temp307= Cyc_Absynpp_exp2doc(
_temp275); struct Cyc_PP_Doc* _temp308= Cyc_PP_text(({ unsigned char* _temp309=(
unsigned char*)"]"; struct _tagged_string _temp310; _temp310.curr= _temp309;
_temp310.base= _temp309; _temp310.last_plus_one= _temp309 + 2; _temp310;}));
struct Cyc_PP_Doc* _temp303[ 4u]={ _temp305, _temp306, _temp307, _temp308};
struct _tagged_ptr0 _temp304={ _temp303, _temp303, _temp303 + 4u}; _temp302(
_temp304);}); _LL268: if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*)
_check_null( tms))->tl)){ rest= p_rest;}{ void* _temp313= _temp277; struct Cyc_Core_Opt*
_temp319; struct Cyc_Absyn_VarargInfo* _temp321; int _temp323; struct Cyc_List_List*
_temp325; struct Cyc_Position_Segment* _temp327; struct Cyc_List_List* _temp329;
_LL315: if(*(( int*) _temp313) == Cyc_Absyn_WithTypes){ _LL326: _temp325=((
struct Cyc_Absyn_WithTypes_struct*) _temp313)->f1; goto _LL324; _LL324: _temp323=((
struct Cyc_Absyn_WithTypes_struct*) _temp313)->f2; goto _LL322; _LL322: _temp321=((
struct Cyc_Absyn_WithTypes_struct*) _temp313)->f3; goto _LL320; _LL320: _temp319=((
struct Cyc_Absyn_WithTypes_struct*) _temp313)->f4; goto _LL316;} else{ goto
_LL317;} _LL317: if(*(( int*) _temp313) == Cyc_Absyn_NoTypes){ _LL330: _temp329=((
struct Cyc_Absyn_NoTypes_struct*) _temp313)->f1; goto _LL328; _LL328: _temp327=((
struct Cyc_Absyn_NoTypes_struct*) _temp313)->f2; goto _LL318;} else{ goto _LL314;}
_LL316: return({ struct Cyc_PP_Doc*(* _temp331)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp334= rest; struct Cyc_PP_Doc* _temp335= Cyc_Absynpp_funargs2doc(
_temp325, _temp323, _temp321, _temp319); struct Cyc_PP_Doc* _temp332[ 2u]={
_temp334, _temp335}; struct _tagged_ptr0 _temp333={ _temp332, _temp332, _temp332
+ 2u}; _temp331( _temp333);}); _LL318: return({ struct Cyc_PP_Doc*(* _temp336)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp339= rest; struct Cyc_PP_Doc*
_temp340= Cyc_PP_group(({ unsigned char* _temp341=( unsigned char*)"("; struct
_tagged_string _temp342; _temp342.curr= _temp341; _temp342.base= _temp341;
_temp342.last_plus_one= _temp341 + 2; _temp342;}),({ unsigned char* _temp343=(
unsigned char*)")"; struct _tagged_string _temp344; _temp344.curr= _temp343;
_temp344.base= _temp343; _temp344.last_plus_one= _temp343 + 2; _temp344;}),({
unsigned char* _temp345=( unsigned char*)","; struct _tagged_string _temp346;
_temp346.curr= _temp345; _temp346.base= _temp345; _temp346.last_plus_one=
_temp345 + 2; _temp346;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_PP_textptr,
_temp329)); struct Cyc_PP_Doc* _temp337[ 2u]={ _temp339, _temp340}; struct
_tagged_ptr0 _temp338={ _temp337, _temp337, _temp337 + 2u}; _temp336( _temp338);});
_LL314:;} _LL270: if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*)
_check_null( tms))->tl)){ rest= p_rest;} return({ struct Cyc_PP_Doc*(* _temp347)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp350= rest; struct Cyc_PP_Doc*
_temp351= Cyc_Absynpp_atts2doc( _temp279); struct Cyc_PP_Doc* _temp348[ 2u]={
_temp350, _temp351}; struct _tagged_ptr0 _temp349={ _temp348, _temp348, _temp348
+ 2u}; _temp347( _temp349);}); _LL272: if( Cyc_Absynpp_next_is_pointer((( struct
Cyc_List_List*) _check_null( tms))->tl)){ rest= p_rest;} if( _temp283){ return({
struct Cyc_PP_Doc*(* _temp352)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp355= rest; struct Cyc_PP_Doc* _temp356= Cyc_Absynpp_ktvars2doc( _temp287);
struct Cyc_PP_Doc* _temp353[ 2u]={ _temp355, _temp356}; struct _tagged_ptr0
_temp354={ _temp353, _temp353, _temp353 + 2u}; _temp352( _temp354);});} else{
return({ struct Cyc_PP_Doc*(* _temp357)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp360= rest; struct Cyc_PP_Doc* _temp361= Cyc_Absynpp_tvars2doc(
_temp287); struct Cyc_PP_Doc* _temp358[ 2u]={ _temp360, _temp361}; struct
_tagged_ptr0 _temp359={ _temp358, _temp358, _temp358 + 2u}; _temp357( _temp359);});}
_LL274: { struct Cyc_PP_Doc* ptr;{ void* _temp362= _temp293; struct Cyc_Absyn_Exp*
_temp370; struct Cyc_Absyn_Exp* _temp372; _LL364: if(( unsigned int) _temp362 >
1u?*(( int*) _temp362) == Cyc_Absyn_Nullable_ps: 0){ _LL371: _temp370=(( struct
Cyc_Absyn_Nullable_ps_struct*) _temp362)->f1; goto _LL365;} else{ goto _LL366;}
_LL366: if(( unsigned int) _temp362 > 1u?*(( int*) _temp362) == Cyc_Absyn_NonNullable_ps:
0){ _LL373: _temp372=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp362)->f1;
goto _LL367;} else{ goto _LL368;} _LL368: if( _temp362 ==( void*) Cyc_Absyn_TaggedArray_ps){
goto _LL369;} else{ goto _LL363;} _LL365: if( Cyc_Evexp_eval_const_uint_exp(
_temp370) == 1){ ptr= Cyc_PP_text(({ unsigned char* _temp374=( unsigned char*)"*";
struct _tagged_string _temp375; _temp375.curr= _temp374; _temp375.base= _temp374;
_temp375.last_plus_one= _temp374 + 2; _temp375;}));} else{ ptr=({ struct Cyc_PP_Doc*(*
_temp376)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp379= Cyc_PP_text(({
unsigned char* _temp387=( unsigned char*)"*"; struct _tagged_string _temp388;
_temp388.curr= _temp387; _temp388.base= _temp387; _temp388.last_plus_one=
_temp387 + 2; _temp388;})); struct Cyc_PP_Doc* _temp380= Cyc_PP_text(({
unsigned char* _temp385=( unsigned char*)"{"; struct _tagged_string _temp386;
_temp386.curr= _temp385; _temp386.base= _temp385; _temp386.last_plus_one=
_temp385 + 2; _temp386;})); struct Cyc_PP_Doc* _temp381= Cyc_Absynpp_exp2doc(
_temp370); struct Cyc_PP_Doc* _temp382= Cyc_PP_text(({ unsigned char* _temp383=(
unsigned char*)"}"; struct _tagged_string _temp384; _temp384.curr= _temp383;
_temp384.base= _temp383; _temp384.last_plus_one= _temp383 + 2; _temp384;}));
struct Cyc_PP_Doc* _temp377[ 4u]={ _temp379, _temp380, _temp381, _temp382};
struct _tagged_ptr0 _temp378={ _temp377, _temp377, _temp377 + 4u}; _temp376(
_temp378);});} goto _LL363; _LL367: if( Cyc_Evexp_eval_const_uint_exp( _temp372)
== 1){ ptr= Cyc_PP_text(({ unsigned char* _temp389=( unsigned char*)"@"; struct
_tagged_string _temp390; _temp390.curr= _temp389; _temp390.base= _temp389;
_temp390.last_plus_one= _temp389 + 2; _temp390;}));} else{ ptr=({ struct Cyc_PP_Doc*(*
_temp391)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp394= Cyc_PP_text(({
unsigned char* _temp402=( unsigned char*)"@"; struct _tagged_string _temp403;
_temp403.curr= _temp402; _temp403.base= _temp402; _temp403.last_plus_one=
_temp402 + 2; _temp403;})); struct Cyc_PP_Doc* _temp395= Cyc_PP_text(({
unsigned char* _temp400=( unsigned char*)"{"; struct _tagged_string _temp401;
_temp401.curr= _temp400; _temp401.base= _temp400; _temp401.last_plus_one=
_temp400 + 2; _temp401;})); struct Cyc_PP_Doc* _temp396= Cyc_Absynpp_exp2doc(
_temp372); struct Cyc_PP_Doc* _temp397= Cyc_PP_text(({ unsigned char* _temp398=(
unsigned char*)"}"; struct _tagged_string _temp399; _temp399.curr= _temp398;
_temp399.base= _temp398; _temp399.last_plus_one= _temp398 + 2; _temp399;}));
struct Cyc_PP_Doc* _temp392[ 4u]={ _temp394, _temp395, _temp396, _temp397};
struct _tagged_ptr0 _temp393={ _temp392, _temp392, _temp392 + 4u}; _temp391(
_temp393);});} goto _LL363; _LL369: ptr= Cyc_PP_text(({ unsigned char* _temp404=(
unsigned char*)"?"; struct _tagged_string _temp405; _temp405.curr= _temp404;
_temp405.base= _temp404; _temp405.last_plus_one= _temp404 + 2; _temp405;}));
goto _LL363; _LL363:;}{ void* _temp406= Cyc_Tcutil_compress( _temp291); _LL408:
if( _temp406 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL409;} else{ goto _LL410;}
_LL410: goto _LL411; _LL409: return({ struct Cyc_PP_Doc*(* _temp412)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp415= ptr; struct Cyc_PP_Doc*
_temp416= rest; struct Cyc_PP_Doc* _temp413[ 2u]={ _temp415, _temp416}; struct
_tagged_ptr0 _temp414={ _temp413, _temp413, _temp413 + 2u}; _temp412( _temp414);});
_LL411: return({ struct Cyc_PP_Doc*(* _temp417)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp420= ptr; struct Cyc_PP_Doc* _temp421= Cyc_Absynpp_ntyp2doc(
_temp291); struct Cyc_PP_Doc* _temp422= Cyc_PP_text(({ unsigned char* _temp424=(
unsigned char*)" "; struct _tagged_string _temp425; _temp425.curr= _temp424;
_temp425.base= _temp424; _temp425.last_plus_one= _temp424 + 2; _temp425;}));
struct Cyc_PP_Doc* _temp423= rest; struct Cyc_PP_Doc* _temp418[ 4u]={ _temp420,
_temp421, _temp422, _temp423}; struct _tagged_ptr0 _temp419={ _temp418, _temp418,
_temp418 + 4u}; _temp417( _temp419);}); _LL407:;}} _LL262:;}} struct Cyc_PP_Doc*
Cyc_Absynpp_rgn2doc( void* t){ void* _temp436= Cyc_Tcutil_compress( t); _LL438:
if( _temp436 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL439;} else{ goto _LL440;}
_LL440: goto _LL441; _LL439: return Cyc_PP_text(({ unsigned char* _temp442=(
unsigned char*)"`H"; struct _tagged_string _temp443; _temp443.curr= _temp442;
_temp443.base= _temp442; _temp443.last_plus_one= _temp442 + 3; _temp443;}));
_LL441: return Cyc_Absynpp_ntyp2doc( t); _LL437:;} static void Cyc_Absynpp_effects2docs(
struct Cyc_List_List** regions, struct Cyc_List_List** effects, void* t){ void*
_temp444= Cyc_Tcutil_compress( t); void* _temp452; struct Cyc_List_List*
_temp454; _LL446: if(( unsigned int) _temp444 > 4u?*(( int*) _temp444) == Cyc_Absyn_AccessEff:
0){ _LL453: _temp452=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp444)->f1;
goto _LL447;} else{ goto _LL448;} _LL448: if(( unsigned int) _temp444 > 4u?*((
int*) _temp444) == Cyc_Absyn_JoinEff: 0){ _LL455: _temp454=(( struct Cyc_Absyn_JoinEff_struct*)
_temp444)->f1; goto _LL449;} else{ goto _LL450;} _LL450: goto _LL451; _LL447:*
regions=({ struct Cyc_List_List* _temp456=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp456->hd=( void*) Cyc_Absynpp_rgn2doc(
_temp452); _temp456->tl=* regions; _temp456;}); goto _LL445; _LL449: for( 0;
_temp454 != 0; _temp454=(( struct Cyc_List_List*) _check_null( _temp454))->tl){
Cyc_Absynpp_effects2docs( regions, effects,( void*)(( struct Cyc_List_List*)
_check_null( _temp454))->hd);} goto _LL445; _LL451:* effects=({ struct Cyc_List_List*
_temp457=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp457->hd=( void*) Cyc_Absynpp_ntyp2doc( t); _temp457->tl=* effects; _temp457;});
goto _LL445; _LL445:;} struct Cyc_PP_Doc* Cyc_Absynpp_eff2doc( void* t){ struct
Cyc_List_List* regions= 0; struct Cyc_List_List* effects= 0; Cyc_Absynpp_effects2docs(&
regions,& effects, t); regions=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( regions); effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( effects); if( regions == 0? effects != 0: 0){ return Cyc_PP_group(({
unsigned char* _temp458=( unsigned char*)""; struct _tagged_string _temp459;
_temp459.curr= _temp458; _temp459.base= _temp458; _temp459.last_plus_one=
_temp458 + 1; _temp459;}),({ unsigned char* _temp460=( unsigned char*)""; struct
_tagged_string _temp461; _temp461.curr= _temp460; _temp461.base= _temp460;
_temp461.last_plus_one= _temp460 + 1; _temp461;}),({ unsigned char* _temp462=(
unsigned char*)"+"; struct _tagged_string _temp463; _temp463.curr= _temp462;
_temp463.base= _temp462; _temp463.last_plus_one= _temp462 + 2; _temp463;}),
effects);} else{ struct Cyc_PP_Doc* _temp470= Cyc_PP_group(({ unsigned char*
_temp464=( unsigned char*)"{"; struct _tagged_string _temp465; _temp465.curr=
_temp464; _temp465.base= _temp464; _temp465.last_plus_one= _temp464 + 2;
_temp465;}),({ unsigned char* _temp466=( unsigned char*)"}"; struct
_tagged_string _temp467; _temp467.curr= _temp466; _temp467.base= _temp466;
_temp467.last_plus_one= _temp466 + 2; _temp467;}),({ unsigned char* _temp468=(
unsigned char*)","; struct _tagged_string _temp469; _temp469.curr= _temp468;
_temp469.base= _temp468; _temp469.last_plus_one= _temp468 + 2; _temp469;}),
regions); goto _LL471; _LL471: return Cyc_PP_group(({ unsigned char* _temp472=(
unsigned char*)""; struct _tagged_string _temp473; _temp473.curr= _temp472;
_temp473.base= _temp472; _temp473.last_plus_one= _temp472 + 1; _temp473;}),({
unsigned char* _temp474=( unsigned char*)""; struct _tagged_string _temp475;
_temp475.curr= _temp474; _temp475.base= _temp474; _temp475.last_plus_one=
_temp474 + 1; _temp475;}),({ unsigned char* _temp476=( unsigned char*)"+";
struct _tagged_string _temp477; _temp477.curr= _temp476; _temp477.base= _temp476;
_temp477.last_plus_one= _temp476 + 2; _temp477;}),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( effects,({
struct Cyc_List_List* _temp478=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp478->hd=( void*) _temp470; _temp478->tl= 0;
_temp478;})));}} struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc( void* t){ struct Cyc_PP_Doc*
s;{ void* _temp479= t; struct Cyc_Absyn_Exp* _temp525; struct Cyc_Absyn_Tqual
_temp527; void* _temp529; struct Cyc_Absyn_FnInfo _temp531; struct Cyc_Absyn_PtrInfo
_temp533; struct Cyc_Core_Opt* _temp535; int _temp537; struct Cyc_Core_Opt*
_temp539; struct Cyc_Core_Opt* _temp541; struct Cyc_Absyn_Tvar* _temp543; struct
Cyc_Absyn_TunionInfo _temp545; void* _temp547; struct Cyc_List_List* _temp549;
void* _temp551; struct Cyc_Absyn_TunionFieldInfo _temp553; struct Cyc_List_List*
_temp555; void* _temp557; void* _temp559; void* _temp561; struct Cyc_List_List*
_temp563; struct Cyc_Absyn_Structdecl** _temp565; struct Cyc_List_List* _temp567;
struct _tuple0* _temp569; struct Cyc_Absyn_Uniondecl** _temp571; struct Cyc_List_List*
_temp573; struct _tuple0* _temp575; struct Cyc_List_List* _temp577; struct Cyc_List_List*
_temp579; struct Cyc_Absyn_Enumdecl* _temp581; struct _tuple0* _temp583; struct
Cyc_Core_Opt* _temp585; struct Cyc_List_List* _temp587; struct _tuple0* _temp589;
void* _temp591; void* _temp593; struct Cyc_List_List* _temp595; _LL481: if((
unsigned int) _temp479 > 4u?*(( int*) _temp479) == Cyc_Absyn_ArrayType: 0){
_LL530: _temp529=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp479)->f1;
goto _LL528; _LL528: _temp527=(( struct Cyc_Absyn_ArrayType_struct*) _temp479)->f2;
goto _LL526; _LL526: _temp525=(( struct Cyc_Absyn_ArrayType_struct*) _temp479)->f3;
goto _LL482;} else{ goto _LL483;} _LL483: if(( unsigned int) _temp479 > 4u?*((
int*) _temp479) == Cyc_Absyn_FnType: 0){ _LL532: _temp531=(( struct Cyc_Absyn_FnType_struct*)
_temp479)->f1; goto _LL484;} else{ goto _LL485;} _LL485: if(( unsigned int)
_temp479 > 4u?*(( int*) _temp479) == Cyc_Absyn_PointerType: 0){ _LL534: _temp533=((
struct Cyc_Absyn_PointerType_struct*) _temp479)->f1; goto _LL486;} else{ goto
_LL487;} _LL487: if( _temp479 ==( void*) Cyc_Absyn_VoidType){ goto _LL488;}
else{ goto _LL489;} _LL489: if(( unsigned int) _temp479 > 4u?*(( int*) _temp479)
== Cyc_Absyn_Evar: 0){ _LL542: _temp541=(( struct Cyc_Absyn_Evar_struct*)
_temp479)->f1; goto _LL540; _LL540: _temp539=(( struct Cyc_Absyn_Evar_struct*)
_temp479)->f2; goto _LL538; _LL538: _temp537=(( struct Cyc_Absyn_Evar_struct*)
_temp479)->f3; goto _LL536; _LL536: _temp535=(( struct Cyc_Absyn_Evar_struct*)
_temp479)->f4; goto _LL490;} else{ goto _LL491;} _LL491: if(( unsigned int)
_temp479 > 4u?*(( int*) _temp479) == Cyc_Absyn_VarType: 0){ _LL544: _temp543=((
struct Cyc_Absyn_VarType_struct*) _temp479)->f1; goto _LL492;} else{ goto _LL493;}
_LL493: if(( unsigned int) _temp479 > 4u?*(( int*) _temp479) == Cyc_Absyn_TunionType:
0){ _LL546: _temp545=(( struct Cyc_Absyn_TunionType_struct*) _temp479)->f1;
_LL552: _temp551=( void*) _temp545.tunion_info; goto _LL550; _LL550: _temp549=
_temp545.targs; goto _LL548; _LL548: _temp547=( void*) _temp545.rgn; goto _LL494;}
else{ goto _LL495;} _LL495: if(( unsigned int) _temp479 > 4u?*(( int*) _temp479)
== Cyc_Absyn_TunionFieldType: 0){ _LL554: _temp553=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp479)->f1; _LL558: _temp557=( void*) _temp553.field_info; goto _LL556;
_LL556: _temp555= _temp553.targs; goto _LL496;} else{ goto _LL497;} _LL497: if((
unsigned int) _temp479 > 4u?*(( int*) _temp479) == Cyc_Absyn_IntType: 0){ _LL562:
_temp561=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp479)->f1; goto _LL560;
_LL560: _temp559=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp479)->f2;
goto _LL498;} else{ goto _LL499;} _LL499: if( _temp479 ==( void*) Cyc_Absyn_FloatType){
goto _LL500;} else{ goto _LL501;} _LL501: if( _temp479 ==( void*) Cyc_Absyn_DoubleType){
goto _LL502;} else{ goto _LL503;} _LL503: if(( unsigned int) _temp479 > 4u?*((
int*) _temp479) == Cyc_Absyn_TupleType: 0){ _LL564: _temp563=(( struct Cyc_Absyn_TupleType_struct*)
_temp479)->f1; goto _LL504;} else{ goto _LL505;} _LL505: if(( unsigned int)
_temp479 > 4u?*(( int*) _temp479) == Cyc_Absyn_StructType: 0){ _LL570: _temp569=((
struct Cyc_Absyn_StructType_struct*) _temp479)->f1; goto _LL568; _LL568:
_temp567=(( struct Cyc_Absyn_StructType_struct*) _temp479)->f2; goto _LL566;
_LL566: _temp565=(( struct Cyc_Absyn_StructType_struct*) _temp479)->f3; goto
_LL506;} else{ goto _LL507;} _LL507: if(( unsigned int) _temp479 > 4u?*(( int*)
_temp479) == Cyc_Absyn_UnionType: 0){ _LL576: _temp575=(( struct Cyc_Absyn_UnionType_struct*)
_temp479)->f1; goto _LL574; _LL574: _temp573=(( struct Cyc_Absyn_UnionType_struct*)
_temp479)->f2; goto _LL572; _LL572: _temp571=(( struct Cyc_Absyn_UnionType_struct*)
_temp479)->f3; goto _LL508;} else{ goto _LL509;} _LL509: if(( unsigned int)
_temp479 > 4u?*(( int*) _temp479) == Cyc_Absyn_AnonStructType: 0){ _LL578:
_temp577=(( struct Cyc_Absyn_AnonStructType_struct*) _temp479)->f1; goto _LL510;}
else{ goto _LL511;} _LL511: if(( unsigned int) _temp479 > 4u?*(( int*) _temp479)
== Cyc_Absyn_AnonUnionType: 0){ _LL580: _temp579=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp479)->f1; goto _LL512;} else{ goto _LL513;} _LL513: if(( unsigned int)
_temp479 > 4u?*(( int*) _temp479) == Cyc_Absyn_EnumType: 0){ _LL584: _temp583=((
struct Cyc_Absyn_EnumType_struct*) _temp479)->f1; goto _LL582; _LL582: _temp581=((
struct Cyc_Absyn_EnumType_struct*) _temp479)->f2; goto _LL514;} else{ goto
_LL515;} _LL515: if(( unsigned int) _temp479 > 4u?*(( int*) _temp479) == Cyc_Absyn_TypedefType:
0){ _LL590: _temp589=(( struct Cyc_Absyn_TypedefType_struct*) _temp479)->f1;
goto _LL588; _LL588: _temp587=(( struct Cyc_Absyn_TypedefType_struct*) _temp479)->f2;
goto _LL586; _LL586: _temp585=(( struct Cyc_Absyn_TypedefType_struct*) _temp479)->f3;
goto _LL516;} else{ goto _LL517;} _LL517: if(( unsigned int) _temp479 > 4u?*((
int*) _temp479) == Cyc_Absyn_RgnHandleType: 0){ _LL592: _temp591=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp479)->f1; goto _LL518;} else{ goto
_LL519;} _LL519: if( _temp479 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL520;} else{
goto _LL521;} _LL521: if(( unsigned int) _temp479 > 4u?*(( int*) _temp479) ==
Cyc_Absyn_AccessEff: 0){ _LL594: _temp593=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp479)->f1; goto _LL522;} else{ goto _LL523;} _LL523: if(( unsigned int)
_temp479 > 4u?*(( int*) _temp479) == Cyc_Absyn_JoinEff: 0){ _LL596: _temp595=((
struct Cyc_Absyn_JoinEff_struct*) _temp479)->f1; goto _LL524;} else{ goto _LL480;}
_LL482: return Cyc_PP_text(({ unsigned char* _temp597=( unsigned char*)"[[[array]]]";
struct _tagged_string _temp598; _temp598.curr= _temp597; _temp598.base= _temp597;
_temp598.last_plus_one= _temp597 + 12; _temp598;})); _LL484: return Cyc_PP_nil_doc();
_LL486: return Cyc_PP_nil_doc(); _LL488: s= Cyc_PP_text(({ unsigned char*
_temp599=( unsigned char*)"void"; struct _tagged_string _temp600; _temp600.curr=
_temp599; _temp600.base= _temp599; _temp600.last_plus_one= _temp599 + 5;
_temp600;})); goto _LL480; _LL490: if( _temp539 != 0){ return Cyc_Absynpp_ntyp2doc((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp539))->v);} else{ s=({ struct
Cyc_PP_Doc*(* _temp601)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp604= Cyc_PP_text(({ unsigned char* _temp618=( unsigned char*)"%"; struct
_tagged_string _temp619; _temp619.curr= _temp618; _temp619.base= _temp618;
_temp619.last_plus_one= _temp618 + 2; _temp619;})); struct Cyc_PP_Doc* _temp605=
Cyc_PP_text(({ unsigned char* _temp616=( unsigned char*)"("; struct
_tagged_string _temp617; _temp617.curr= _temp616; _temp617.base= _temp616;
_temp617.last_plus_one= _temp616 + 2; _temp617;})); struct Cyc_PP_Doc* _temp606=
Cyc_PP_text(( struct _tagged_string) xprintf("%d", _temp537)); struct Cyc_PP_Doc*
_temp607=( ! Cyc_Absynpp_print_full_evars? 1: _temp535 == 0)? Cyc_PP_text(({
unsigned char* _temp614=( unsigned char*)""; struct _tagged_string _temp615;
_temp615.curr= _temp614; _temp615.base= _temp614; _temp615.last_plus_one=
_temp614 + 1; _temp615;})): Cyc_Absynpp_tvars2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp535))->v); struct Cyc_PP_Doc* _temp608=
Cyc_PP_text(({ unsigned char* _temp612=( unsigned char*)")::"; struct
_tagged_string _temp613; _temp613.curr= _temp612; _temp613.base= _temp612;
_temp613.last_plus_one= _temp612 + 4; _temp613;})); struct Cyc_PP_Doc* _temp609=
_temp541 == 0? Cyc_PP_text(({ unsigned char* _temp610=( unsigned char*)"?";
struct _tagged_string _temp611; _temp611.curr= _temp610; _temp611.base= _temp610;
_temp611.last_plus_one= _temp610 + 2; _temp611;})): Cyc_Absynpp_kind2doc(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp541))->v); struct Cyc_PP_Doc* _temp602[
6u]={ _temp604, _temp605, _temp606, _temp607, _temp608, _temp609}; struct
_tagged_ptr0 _temp603={ _temp602, _temp602, _temp602 + 6u}; _temp601( _temp603);});}
goto _LL480; _LL492: s= Cyc_PP_textptr( _temp543->name); if( Cyc_Absynpp_print_all_kinds){
s=({ struct Cyc_PP_Doc*(* _temp620)( struct _tagged_ptr0)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp623= s; struct Cyc_PP_Doc* _temp624= Cyc_PP_text(({
unsigned char* _temp626=( unsigned char*)"::"; struct _tagged_string _temp627;
_temp627.curr= _temp626; _temp627.base= _temp626; _temp627.last_plus_one=
_temp626 + 3; _temp627;})); struct Cyc_PP_Doc* _temp625= Cyc_Absynpp_ckind2doc(
_temp543->kind); struct Cyc_PP_Doc* _temp621[ 3u]={ _temp623, _temp624, _temp625};
struct _tagged_ptr0 _temp622={ _temp621, _temp621, _temp621 + 3u}; _temp620(
_temp622);});} if( ! Cyc_Absynpp_dont_rewrite_temp_tvars? Cyc_Tcutil_is_temp_tvar(
_temp543): 0){ s=({ struct Cyc_PP_Doc*(* _temp628)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp631= Cyc_PP_text(({ unsigned char* _temp636=(
unsigned char*)"_ /* "; struct _tagged_string _temp637; _temp637.curr= _temp636;
_temp637.base= _temp636; _temp637.last_plus_one= _temp636 + 6; _temp637;}));
struct Cyc_PP_Doc* _temp632= s; struct Cyc_PP_Doc* _temp633= Cyc_PP_text(({
unsigned char* _temp634=( unsigned char*)" */"; struct _tagged_string _temp635;
_temp635.curr= _temp634; _temp635.base= _temp634; _temp635.last_plus_one=
_temp634 + 4; _temp635;})); struct Cyc_PP_Doc* _temp629[ 3u]={ _temp631,
_temp632, _temp633}; struct _tagged_ptr0 _temp630={ _temp629, _temp629, _temp629
+ 3u}; _temp628( _temp630);});} goto _LL480; _LL494:{ void* _temp638= _temp551;
struct Cyc_Absyn_UnknownTunionInfo _temp644; int _temp646; struct _tuple0*
_temp648; struct Cyc_Absyn_Tuniondecl* _temp650; struct Cyc_Absyn_Tuniondecl
_temp652; int _temp653; struct Cyc_Core_Opt* _temp655; struct Cyc_List_List*
_temp657; struct _tuple0* _temp659; void* _temp661; _LL640: if(*(( int*)
_temp638) == Cyc_Absyn_UnknownTunion){ _LL645: _temp644=(( struct Cyc_Absyn_UnknownTunion_struct*)
_temp638)->f1; _LL649: _temp648= _temp644.name; goto _LL647; _LL647: _temp646=
_temp644.is_xtunion; goto _LL641;} else{ goto _LL642;} _LL642: if(*(( int*)
_temp638) == Cyc_Absyn_KnownTunion){ _LL651: _temp650=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp638)->f1; _temp652=* _temp650; _LL662: _temp661=( void*) _temp652.sc; goto
_LL660; _LL660: _temp659= _temp652.name; goto _LL658; _LL658: _temp657= _temp652.tvs;
goto _LL656; _LL656: _temp655= _temp652.fields; goto _LL654; _LL654: _temp653=
_temp652.is_xtunion; goto _LL643;} else{ goto _LL639;} _LL641: _temp659=
_temp648; _temp653= _temp646; goto _LL643; _LL643: { struct Cyc_PP_Doc* _temp667=
Cyc_PP_text( _temp653?({ unsigned char* _temp663=( unsigned char*)"xtunion ";
struct _tagged_string _temp664; _temp664.curr= _temp663; _temp664.base= _temp663;
_temp664.last_plus_one= _temp663 + 9; _temp664;}):({ unsigned char* _temp665=(
unsigned char*)"tunion "; struct _tagged_string _temp666; _temp666.curr=
_temp665; _temp666.base= _temp665; _temp666.last_plus_one= _temp665 + 8;
_temp666;})); goto _LL668; _LL668:{ void* _temp669= Cyc_Tcutil_compress(
_temp547); _LL671: if( _temp669 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL672;}
else{ goto _LL673;} _LL673: goto _LL674; _LL672: s=({ struct Cyc_PP_Doc*(*
_temp675)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp678=
_temp667; struct Cyc_PP_Doc* _temp679= Cyc_Absynpp_qvar2doc( _temp659); struct
Cyc_PP_Doc* _temp680= Cyc_Absynpp_tps2doc( _temp549); struct Cyc_PP_Doc*
_temp676[ 3u]={ _temp678, _temp679, _temp680}; struct _tagged_ptr0 _temp677={
_temp676, _temp676, _temp676 + 3u}; _temp675( _temp677);}); goto _LL670; _LL674:
s=({ struct Cyc_PP_Doc*(* _temp681)( struct _tagged_ptr0)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp684= _temp667; struct Cyc_PP_Doc* _temp685= Cyc_Absynpp_typ2doc(
_temp547); struct Cyc_PP_Doc* _temp686= Cyc_PP_text(({ unsigned char* _temp689=(
unsigned char*)" "; struct _tagged_string _temp690; _temp690.curr= _temp689;
_temp690.base= _temp689; _temp690.last_plus_one= _temp689 + 2; _temp690;}));
struct Cyc_PP_Doc* _temp687= Cyc_Absynpp_qvar2doc( _temp659); struct Cyc_PP_Doc*
_temp688= Cyc_Absynpp_tps2doc( _temp549); struct Cyc_PP_Doc* _temp682[ 5u]={
_temp684, _temp685, _temp686, _temp687, _temp688}; struct _tagged_ptr0 _temp683={
_temp682, _temp682, _temp682 + 5u}; _temp681( _temp683);}); goto _LL670; _LL670:;}
goto _LL639;} _LL639:;} goto _LL480; _LL496:{ void* _temp691= _temp557; struct
Cyc_Absyn_UnknownTunionFieldInfo _temp697; int _temp699; struct _tuple0*
_temp701; struct _tuple0* _temp703; struct Cyc_Absyn_Tunionfield* _temp705;
struct Cyc_Absyn_Tunionfield _temp707; void* _temp708; struct Cyc_Position_Segment*
_temp710; struct Cyc_List_List* _temp712; struct Cyc_List_List* _temp714; struct
_tuple0* _temp716; struct Cyc_Absyn_Tuniondecl* _temp718; struct Cyc_Absyn_Tuniondecl
_temp720; int _temp721; struct Cyc_Core_Opt* _temp723; struct Cyc_List_List*
_temp725; struct _tuple0* _temp727; void* _temp729; _LL693: if(*(( int*)
_temp691) == Cyc_Absyn_UnknownTunionfield){ _LL698: _temp697=(( struct Cyc_Absyn_UnknownTunionfield_struct*)
_temp691)->f1; _LL704: _temp703= _temp697.tunion_name; goto _LL702; _LL702:
_temp701= _temp697.field_name; goto _LL700; _LL700: _temp699= _temp697.is_xtunion;
goto _LL694;} else{ goto _LL695;} _LL695: if(*(( int*) _temp691) == Cyc_Absyn_KnownTunionfield){
_LL719: _temp718=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp691)->f1;
_temp720=* _temp718; _LL730: _temp729=( void*) _temp720.sc; goto _LL728; _LL728:
_temp727= _temp720.name; goto _LL726; _LL726: _temp725= _temp720.tvs; goto
_LL724; _LL724: _temp723= _temp720.fields; goto _LL722; _LL722: _temp721=
_temp720.is_xtunion; goto _LL706; _LL706: _temp705=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp691)->f2; _temp707=* _temp705; _LL717: _temp716= _temp707.name; goto _LL715;
_LL715: _temp714= _temp707.tvs; goto _LL713; _LL713: _temp712= _temp707.typs;
goto _LL711; _LL711: _temp710= _temp707.loc; goto _LL709; _LL709: _temp708=(
void*) _temp707.sc; goto _LL696;} else{ goto _LL692;} _LL694: _temp727= _temp703;
_temp721= _temp699; _temp716= _temp701; goto _LL696; _LL696: { struct Cyc_PP_Doc*
_temp735= Cyc_PP_text( _temp721?({ unsigned char* _temp731=( unsigned char*)"xtunion ";
struct _tagged_string _temp732; _temp732.curr= _temp731; _temp732.base= _temp731;
_temp732.last_plus_one= _temp731 + 9; _temp732;}):({ unsigned char* _temp733=(
unsigned char*)"tunion "; struct _tagged_string _temp734; _temp734.curr=
_temp733; _temp734.base= _temp733; _temp734.last_plus_one= _temp733 + 8;
_temp734;})); goto _LL736; _LL736: s=({ struct Cyc_PP_Doc*(* _temp737)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp740= _temp735; struct Cyc_PP_Doc*
_temp741= Cyc_Absynpp_qvar2doc( _temp727); struct Cyc_PP_Doc* _temp742= Cyc_PP_text(({
unsigned char* _temp744=( unsigned char*)"."; struct _tagged_string _temp745;
_temp745.curr= _temp744; _temp745.base= _temp744; _temp745.last_plus_one=
_temp744 + 2; _temp745;})); struct Cyc_PP_Doc* _temp743= Cyc_PP_textptr((*
_temp716).f2); struct Cyc_PP_Doc* _temp738[ 4u]={ _temp740, _temp741, _temp742,
_temp743}; struct _tagged_ptr0 _temp739={ _temp738, _temp738, _temp738 + 4u};
_temp737( _temp739);}); goto _LL692;} _LL692:;} goto _LL480; _LL498: { struct
_tagged_string sns; struct _tagged_string ts;{ void* _temp746= _temp561; _LL748:
if( _temp746 ==( void*) Cyc_Absyn_Signed){ goto _LL749;} else{ goto _LL750;}
_LL750: if( _temp746 ==( void*) Cyc_Absyn_Unsigned){ goto _LL751;} else{ goto
_LL747;} _LL749: sns=({ unsigned char* _temp752=( unsigned char*)""; struct
_tagged_string _temp753; _temp753.curr= _temp752; _temp753.base= _temp752;
_temp753.last_plus_one= _temp752 + 1; _temp753;}); goto _LL747; _LL751: sns=({
unsigned char* _temp754=( unsigned char*)"unsigned "; struct _tagged_string
_temp755; _temp755.curr= _temp754; _temp755.base= _temp754; _temp755.last_plus_one=
_temp754 + 10; _temp755;}); goto _LL747; _LL747:;}{ void* _temp756= _temp559;
_LL758: if( _temp756 ==( void*) Cyc_Absyn_B1){ goto _LL759;} else{ goto _LL760;}
_LL760: if( _temp756 ==( void*) Cyc_Absyn_B2){ goto _LL761;} else{ goto _LL762;}
_LL762: if( _temp756 ==( void*) Cyc_Absyn_B4){ goto _LL763;} else{ goto _LL764;}
_LL764: if( _temp756 ==( void*) Cyc_Absyn_B8){ goto _LL765;} else{ goto _LL757;}
_LL759:{ void* _temp766= _temp561; _LL768: if( _temp766 ==( void*) Cyc_Absyn_Signed){
goto _LL769;} else{ goto _LL770;} _LL770: if( _temp766 ==( void*) Cyc_Absyn_Unsigned){
goto _LL771;} else{ goto _LL767;} _LL769: sns=({ unsigned char* _temp772=(
unsigned char*)"signed "; struct _tagged_string _temp773; _temp773.curr=
_temp772; _temp773.base= _temp772; _temp773.last_plus_one= _temp772 + 8;
_temp773;}); goto _LL767; _LL771: goto _LL767; _LL767:;} ts=({ unsigned char*
_temp774=( unsigned char*)"char"; struct _tagged_string _temp775; _temp775.curr=
_temp774; _temp775.base= _temp774; _temp775.last_plus_one= _temp774 + 5;
_temp775;}); goto _LL757; _LL761: ts=({ unsigned char* _temp776=( unsigned char*)"short";
struct _tagged_string _temp777; _temp777.curr= _temp776; _temp777.base= _temp776;
_temp777.last_plus_one= _temp776 + 6; _temp777;}); goto _LL757; _LL763: ts=({
unsigned char* _temp778=( unsigned char*)"int"; struct _tagged_string _temp779;
_temp779.curr= _temp778; _temp779.base= _temp778; _temp779.last_plus_one=
_temp778 + 4; _temp779;}); goto _LL757; _LL765: ts= Cyc_Absynpp_to_VC?({
unsigned char* _temp780=( unsigned char*)"__int64"; struct _tagged_string
_temp781; _temp781.curr= _temp780; _temp781.base= _temp780; _temp781.last_plus_one=
_temp780 + 8; _temp781;}):({ unsigned char* _temp782=( unsigned char*)"long long";
struct _tagged_string _temp783; _temp783.curr= _temp782; _temp783.base= _temp782;
_temp783.last_plus_one= _temp782 + 10; _temp783;}); goto _LL757; _LL757:;} s=
Cyc_PP_text(( struct _tagged_string)({ struct _tagged_string _temp784= sns;
struct _tagged_string _temp785= ts; xprintf("%.*s%.*s", _temp784.last_plus_one -
_temp784.curr, _temp784.curr, _temp785.last_plus_one - _temp785.curr, _temp785.curr);}));
goto _LL480;} _LL500: s= Cyc_PP_text(({ unsigned char* _temp786=( unsigned char*)"float";
struct _tagged_string _temp787; _temp787.curr= _temp786; _temp787.base= _temp786;
_temp787.last_plus_one= _temp786 + 6; _temp787;})); goto _LL480; _LL502: s= Cyc_PP_text(({
unsigned char* _temp788=( unsigned char*)"double"; struct _tagged_string
_temp789; _temp789.curr= _temp788; _temp789.base= _temp788; _temp789.last_plus_one=
_temp788 + 7; _temp789;})); goto _LL480; _LL504: s=({ struct Cyc_PP_Doc*(*
_temp790)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp793= Cyc_PP_text(({
unsigned char* _temp795=( unsigned char*)"$"; struct _tagged_string _temp796;
_temp796.curr= _temp795; _temp796.base= _temp795; _temp796.last_plus_one=
_temp795 + 2; _temp796;})); struct Cyc_PP_Doc* _temp794= Cyc_Absynpp_args2doc(
_temp563); struct Cyc_PP_Doc* _temp791[ 2u]={ _temp793, _temp794}; struct
_tagged_ptr0 _temp792={ _temp791, _temp791, _temp791 + 2u}; _temp790( _temp792);});
goto _LL480; _LL506: if( _temp569 == 0){ s=({ struct Cyc_PP_Doc*(* _temp797)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp800= Cyc_PP_text(({
unsigned char* _temp802=( unsigned char*)"struct "; struct _tagged_string
_temp803; _temp803.curr= _temp802; _temp803.base= _temp802; _temp803.last_plus_one=
_temp802 + 8; _temp803;})); struct Cyc_PP_Doc* _temp801= Cyc_Absynpp_tps2doc(
_temp567); struct Cyc_PP_Doc* _temp798[ 2u]={ _temp800, _temp801}; struct
_tagged_ptr0 _temp799={ _temp798, _temp798, _temp798 + 2u}; _temp797( _temp799);});}
else{ s=({ struct Cyc_PP_Doc*(* _temp804)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp807= Cyc_PP_text(({ unsigned char* _temp810=(
unsigned char*)"struct "; struct _tagged_string _temp811; _temp811.curr=
_temp810; _temp811.base= _temp810; _temp811.last_plus_one= _temp810 + 8;
_temp811;})); struct Cyc_PP_Doc* _temp808= _temp569 == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc((
struct _tuple0*) _check_null( _temp569)); struct Cyc_PP_Doc* _temp809= Cyc_Absynpp_tps2doc(
_temp567); struct Cyc_PP_Doc* _temp805[ 3u]={ _temp807, _temp808, _temp809};
struct _tagged_ptr0 _temp806={ _temp805, _temp805, _temp805 + 3u}; _temp804(
_temp806);});} goto _LL480; _LL508: if( _temp575 == 0){ s=({ struct Cyc_PP_Doc*(*
_temp812)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp815= Cyc_PP_text(({
unsigned char* _temp817=( unsigned char*)"union "; struct _tagged_string
_temp818; _temp818.curr= _temp817; _temp818.base= _temp817; _temp818.last_plus_one=
_temp817 + 7; _temp818;})); struct Cyc_PP_Doc* _temp816= Cyc_Absynpp_tps2doc(
_temp573); struct Cyc_PP_Doc* _temp813[ 2u]={ _temp815, _temp816}; struct
_tagged_ptr0 _temp814={ _temp813, _temp813, _temp813 + 2u}; _temp812( _temp814);});}
else{ s=({ struct Cyc_PP_Doc*(* _temp819)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp822= Cyc_PP_text(({ unsigned char* _temp825=(
unsigned char*)"union "; struct _tagged_string _temp826; _temp826.curr= _temp825;
_temp826.base= _temp825; _temp826.last_plus_one= _temp825 + 7; _temp826;}));
struct Cyc_PP_Doc* _temp823= _temp575 == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc((
struct _tuple0*) _check_null( _temp575)); struct Cyc_PP_Doc* _temp824= Cyc_Absynpp_tps2doc(
_temp573); struct Cyc_PP_Doc* _temp820[ 3u]={ _temp822, _temp823, _temp824};
struct _tagged_ptr0 _temp821={ _temp820, _temp820, _temp820 + 3u}; _temp819(
_temp821);});} goto _LL480; _LL510: s=({ struct Cyc_PP_Doc*(* _temp827)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp830= Cyc_PP_text(({
unsigned char* _temp835=( unsigned char*)"struct {"; struct _tagged_string
_temp836; _temp836.curr= _temp835; _temp836.base= _temp835; _temp836.last_plus_one=
_temp835 + 9; _temp836;})); struct Cyc_PP_Doc* _temp831= Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc(
_temp577)); struct Cyc_PP_Doc* _temp832= Cyc_PP_text(({ unsigned char* _temp833=(
unsigned char*)"}"; struct _tagged_string _temp834; _temp834.curr= _temp833;
_temp834.base= _temp833; _temp834.last_plus_one= _temp833 + 2; _temp834;}));
struct Cyc_PP_Doc* _temp828[ 3u]={ _temp830, _temp831, _temp832}; struct
_tagged_ptr0 _temp829={ _temp828, _temp828, _temp828 + 3u}; _temp827( _temp829);});
goto _LL480; _LL512: s=({ struct Cyc_PP_Doc*(* _temp837)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp840= Cyc_PP_text(({ unsigned char* _temp845=(
unsigned char*)"union {"; struct _tagged_string _temp846; _temp846.curr=
_temp845; _temp846.base= _temp845; _temp846.last_plus_one= _temp845 + 8;
_temp846;})); struct Cyc_PP_Doc* _temp841= Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc(
_temp579)); struct Cyc_PP_Doc* _temp842= Cyc_PP_text(({ unsigned char* _temp843=(
unsigned char*)"}"; struct _tagged_string _temp844; _temp844.curr= _temp843;
_temp844.base= _temp843; _temp844.last_plus_one= _temp843 + 2; _temp844;}));
struct Cyc_PP_Doc* _temp838[ 3u]={ _temp840, _temp841, _temp842}; struct
_tagged_ptr0 _temp839={ _temp838, _temp838, _temp838 + 3u}; _temp837( _temp839);});
goto _LL480; _LL514: s=({ struct Cyc_PP_Doc*(* _temp847)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp850= Cyc_PP_text(({ unsigned char* _temp852=(
unsigned char*)"enum "; struct _tagged_string _temp853; _temp853.curr= _temp852;
_temp853.base= _temp852; _temp853.last_plus_one= _temp852 + 6; _temp853;}));
struct Cyc_PP_Doc* _temp851= Cyc_Absynpp_qvar2doc( _temp583); struct Cyc_PP_Doc*
_temp848[ 2u]={ _temp850, _temp851}; struct _tagged_ptr0 _temp849={ _temp848,
_temp848, _temp848 + 2u}; _temp847( _temp849);}); goto _LL480; _LL516: s=({
struct Cyc_PP_Doc*(* _temp854)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp857= Cyc_Absynpp_qvar2doc( _temp589); struct Cyc_PP_Doc* _temp858= Cyc_Absynpp_tps2doc(
_temp587); struct Cyc_PP_Doc* _temp855[ 2u]={ _temp857, _temp858}; struct
_tagged_ptr0 _temp856={ _temp855, _temp855, _temp855 + 2u}; _temp854( _temp856);});
goto _LL480; _LL518: s=({ struct Cyc_PP_Doc*(* _temp859)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp862= Cyc_PP_text(({ unsigned char* _temp867=(
unsigned char*)"region_t<"; struct _tagged_string _temp868; _temp868.curr=
_temp867; _temp868.base= _temp867; _temp868.last_plus_one= _temp867 + 10;
_temp868;})); struct Cyc_PP_Doc* _temp863= Cyc_Absynpp_rgn2doc( _temp591);
struct Cyc_PP_Doc* _temp864= Cyc_PP_text(({ unsigned char* _temp865=(
unsigned char*)">"; struct _tagged_string _temp866; _temp866.curr= _temp865;
_temp866.base= _temp865; _temp866.last_plus_one= _temp865 + 2; _temp866;}));
struct Cyc_PP_Doc* _temp860[ 3u]={ _temp862, _temp863, _temp864}; struct
_tagged_ptr0 _temp861={ _temp860, _temp860, _temp860 + 3u}; _temp859( _temp861);});
goto _LL480; _LL520: s= Cyc_Absynpp_rgn2doc( t); goto _LL480; _LL522: s= Cyc_Absynpp_eff2doc(
t); goto _LL480; _LL524: s= Cyc_Absynpp_eff2doc( t); goto _LL480; _LL480:;}
return s;} struct Cyc_PP_Doc* Cyc_Absynpp_vo2doc( struct Cyc_Core_Opt* vo){
return vo == 0? Cyc_PP_nil_doc(): Cyc_PP_text(*(( struct _tagged_string*)((
struct Cyc_Core_Opt*) _check_null( vo))->v));} struct Cyc_PP_Doc* Cyc_Absynpp_funarg2doc(
struct _tuple1* t){ struct Cyc_Core_Opt* dopt=(* t).f1 == 0? 0:({ struct Cyc_Core_Opt*
_temp869=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp869->v=( void*) Cyc_PP_text(*(( struct _tagged_string*)(( struct Cyc_Core_Opt*)
_check_null((* t).f1))->v)); _temp869;}); return Cyc_Absynpp_tqtd2doc((* t).f2,(*
t).f3, dopt);} struct Cyc_PP_Doc* Cyc_Absynpp_funargs2doc( struct Cyc_List_List*
args, int c_varargs, struct Cyc_Absyn_VarargInfo* cyc_varargs, struct Cyc_Core_Opt*
effopt){ struct Cyc_List_List* _temp870=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(*
f)( struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc,
args); goto _LL871; _LL871: if( effopt == 0){ if( c_varargs){ return Cyc_PP_group(({
unsigned char* _temp872=( unsigned char*)"("; struct _tagged_string _temp873;
_temp873.curr= _temp872; _temp873.base= _temp872; _temp873.last_plus_one=
_temp872 + 2; _temp873;}),({ unsigned char* _temp874=( unsigned char*)")";
struct _tagged_string _temp875; _temp875.curr= _temp874; _temp875.base= _temp874;
_temp875.last_plus_one= _temp874 + 2; _temp875;}),({ unsigned char* _temp876=(
unsigned char*)","; struct _tagged_string _temp877; _temp877.curr= _temp876;
_temp877.base= _temp876; _temp877.last_plus_one= _temp876 + 2; _temp877;}),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp870,({ struct Cyc_List_List* _temp878=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp878->hd=( void*) Cyc_PP_text(({
unsigned char* _temp879=( unsigned char*)"..."; struct _tagged_string _temp880;
_temp880.curr= _temp879; _temp880.base= _temp879; _temp880.last_plus_one=
_temp879 + 4; _temp880;})); _temp878->tl= 0; _temp878;})));} else{ if(
cyc_varargs != 0){ struct Cyc_PP_Doc* _temp882= Cyc_Absynpp_funarg2doc(({ struct
_tuple1* _temp881=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp881->f1=(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->name;
_temp881->f2=(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->tq;
_temp881->f3=( void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->type;
_temp881;})); goto _LL883; _LL883: { struct Cyc_PP_Doc* _temp897=({ struct Cyc_PP_Doc*(*
_temp884)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp887= Cyc_PP_text(({
unsigned char* _temp895=( unsigned char*)"..."; struct _tagged_string _temp896;
_temp896.curr= _temp895; _temp896.base= _temp895; _temp896.last_plus_one=
_temp895 + 4; _temp896;})); struct Cyc_PP_Doc* _temp888= Cyc_Absynpp_typ2doc((
void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->rgn); struct
Cyc_PP_Doc* _temp889=(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->inject?
Cyc_PP_text(({ unsigned char* _temp891=( unsigned char*)" inject "; struct
_tagged_string _temp892; _temp892.curr= _temp891; _temp892.base= _temp891;
_temp892.last_plus_one= _temp891 + 9; _temp892;})): Cyc_PP_text(({ unsigned char*
_temp893=( unsigned char*)""; struct _tagged_string _temp894; _temp894.curr=
_temp893; _temp894.base= _temp893; _temp894.last_plus_one= _temp893 + 1;
_temp894;})); struct Cyc_PP_Doc* _temp890= _temp882; struct Cyc_PP_Doc* _temp885[
4u]={ _temp887, _temp888, _temp889, _temp890}; struct _tagged_ptr0 _temp886={
_temp885, _temp885, _temp885 + 4u}; _temp884( _temp886);}); goto _LL898; _LL898:
return Cyc_PP_group(({ unsigned char* _temp899=( unsigned char*)"("; struct
_tagged_string _temp900; _temp900.curr= _temp899; _temp900.base= _temp899;
_temp900.last_plus_one= _temp899 + 2; _temp900;}),({ unsigned char* _temp901=(
unsigned char*)")"; struct _tagged_string _temp902; _temp902.curr= _temp901;
_temp902.base= _temp901; _temp902.last_plus_one= _temp901 + 2; _temp902;}),({
unsigned char* _temp903=( unsigned char*)","; struct _tagged_string _temp904;
_temp904.curr= _temp903; _temp904.base= _temp903; _temp904.last_plus_one=
_temp903 + 2; _temp904;}),(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( _temp870,({ struct Cyc_List_List*
_temp905=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp905->hd=( void*) _temp897; _temp905->tl= 0; _temp905;})));}} else{ return
Cyc_PP_group(({ unsigned char* _temp906=( unsigned char*)"("; struct
_tagged_string _temp907; _temp907.curr= _temp906; _temp907.base= _temp906;
_temp907.last_plus_one= _temp906 + 2; _temp907;}),({ unsigned char* _temp908=(
unsigned char*)")"; struct _tagged_string _temp909; _temp909.curr= _temp908;
_temp909.base= _temp908; _temp909.last_plus_one= _temp908 + 2; _temp909;}),({
unsigned char* _temp910=( unsigned char*)","; struct _tagged_string _temp911;
_temp911.curr= _temp910; _temp911.base= _temp910; _temp911.last_plus_one=
_temp910 + 2; _temp911;}), _temp870);}}} else{ if( c_varargs){ return Cyc_PP_group(({
unsigned char* _temp912=( unsigned char*)"("; struct _tagged_string _temp913;
_temp913.curr= _temp912; _temp913.base= _temp912; _temp913.last_plus_one=
_temp912 + 2; _temp913;}),({ unsigned char* _temp914=( unsigned char*)")";
struct _tagged_string _temp915; _temp915.curr= _temp914; _temp915.base= _temp914;
_temp915.last_plus_one= _temp914 + 2; _temp915;}),({ unsigned char* _temp916=(
unsigned char*)";"; struct _tagged_string _temp917; _temp917.curr= _temp916;
_temp917.base= _temp916; _temp917.last_plus_one= _temp916 + 2; _temp917;}),({
struct Cyc_List_List* _temp918=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp918->hd=( void*) Cyc_PP_group(({ unsigned char*
_temp920=( unsigned char*)""; struct _tagged_string _temp921; _temp921.curr=
_temp920; _temp921.base= _temp920; _temp921.last_plus_one= _temp920 + 1;
_temp921;}),({ unsigned char* _temp922=( unsigned char*)""; struct
_tagged_string _temp923; _temp923.curr= _temp922; _temp923.base= _temp922;
_temp923.last_plus_one= _temp922 + 1; _temp923;}),({ unsigned char* _temp924=(
unsigned char*)","; struct _tagged_string _temp925; _temp925.curr= _temp924;
_temp925.base= _temp924; _temp925.last_plus_one= _temp924 + 2; _temp925;}),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp870,({ struct Cyc_List_List* _temp926=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp926->hd=( void*) Cyc_PP_text(({
unsigned char* _temp927=( unsigned char*)"..."; struct _tagged_string _temp928;
_temp928.curr= _temp927; _temp928.base= _temp927; _temp928.last_plus_one=
_temp927 + 4; _temp928;})); _temp926->tl= 0; _temp926;}))); _temp918->tl=({
struct Cyc_List_List* _temp919=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp919->hd=( void*) Cyc_Absynpp_eff2doc(( void*)((
struct Cyc_Core_Opt*) _check_null( effopt))->v); _temp919->tl= 0; _temp919;});
_temp918;}));} else{ if( cyc_varargs != 0){ struct Cyc_PP_Doc* _temp930= Cyc_Absynpp_funarg2doc(({
struct _tuple1* _temp929=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp929->f1=(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->name;
_temp929->f2=(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->tq;
_temp929->f3=( void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->type;
_temp929;})); goto _LL931; _LL931: { struct Cyc_PP_Doc* _temp945=({ struct Cyc_PP_Doc*(*
_temp932)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp935= Cyc_PP_text(({
unsigned char* _temp943=( unsigned char*)"..."; struct _tagged_string _temp944;
_temp944.curr= _temp943; _temp944.base= _temp943; _temp944.last_plus_one=
_temp943 + 4; _temp944;})); struct Cyc_PP_Doc* _temp936= Cyc_Absynpp_typ2doc((
void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->rgn); struct
Cyc_PP_Doc* _temp937=(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->inject?
Cyc_PP_text(({ unsigned char* _temp939=( unsigned char*)" inject "; struct
_tagged_string _temp940; _temp940.curr= _temp939; _temp940.base= _temp939;
_temp940.last_plus_one= _temp939 + 9; _temp940;})): Cyc_PP_text(({ unsigned char*
_temp941=( unsigned char*)""; struct _tagged_string _temp942; _temp942.curr=
_temp941; _temp942.base= _temp941; _temp942.last_plus_one= _temp941 + 1;
_temp942;})); struct Cyc_PP_Doc* _temp938= _temp930; struct Cyc_PP_Doc* _temp933[
4u]={ _temp935, _temp936, _temp937, _temp938}; struct _tagged_ptr0 _temp934={
_temp933, _temp933, _temp933 + 4u}; _temp932( _temp934);}); goto _LL946; _LL946:
return Cyc_PP_group(({ unsigned char* _temp947=( unsigned char*)"("; struct
_tagged_string _temp948; _temp948.curr= _temp947; _temp948.base= _temp947;
_temp948.last_plus_one= _temp947 + 2; _temp948;}),({ unsigned char* _temp949=(
unsigned char*)")"; struct _tagged_string _temp950; _temp950.curr= _temp949;
_temp950.base= _temp949; _temp950.last_plus_one= _temp949 + 2; _temp950;}),({
unsigned char* _temp951=( unsigned char*)";"; struct _tagged_string _temp952;
_temp952.curr= _temp951; _temp952.base= _temp951; _temp952.last_plus_one=
_temp951 + 2; _temp952;}),({ struct Cyc_List_List* _temp953=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp953->hd=( void*) Cyc_PP_group(({
unsigned char* _temp955=( unsigned char*)""; struct _tagged_string _temp956;
_temp956.curr= _temp955; _temp956.base= _temp955; _temp956.last_plus_one=
_temp955 + 1; _temp956;}),({ unsigned char* _temp957=( unsigned char*)""; struct
_tagged_string _temp958; _temp958.curr= _temp957; _temp958.base= _temp957;
_temp958.last_plus_one= _temp957 + 1; _temp958;}),({ unsigned char* _temp959=(
unsigned char*)","; struct _tagged_string _temp960; _temp960.curr= _temp959;
_temp960.base= _temp959; _temp960.last_plus_one= _temp959 + 2; _temp960;}),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp870,({ struct Cyc_List_List* _temp961=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp961->hd=( void*) _temp945; _temp961->tl= 0;
_temp961;}))); _temp953->tl=({ struct Cyc_List_List* _temp954=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp954->hd=( void*) Cyc_Absynpp_eff2doc((
void*)(( struct Cyc_Core_Opt*) _check_null( effopt))->v); _temp954->tl= 0;
_temp954;}); _temp953;}));}} else{ return Cyc_PP_group(({ unsigned char*
_temp962=( unsigned char*)"("; struct _tagged_string _temp963; _temp963.curr=
_temp962; _temp963.base= _temp962; _temp963.last_plus_one= _temp962 + 2;
_temp963;}),({ unsigned char* _temp964=( unsigned char*)")"; struct
_tagged_string _temp965; _temp965.curr= _temp964; _temp965.base= _temp964;
_temp965.last_plus_one= _temp964 + 2; _temp965;}),({ unsigned char* _temp966=(
unsigned char*)";"; struct _tagged_string _temp967; _temp967.curr= _temp966;
_temp967.base= _temp966; _temp967.last_plus_one= _temp966 + 2; _temp967;}),({
struct Cyc_List_List* _temp968=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp968->hd=( void*) Cyc_PP_group(({ unsigned char*
_temp970=( unsigned char*)""; struct _tagged_string _temp971; _temp971.curr=
_temp970; _temp971.base= _temp970; _temp971.last_plus_one= _temp970 + 1;
_temp971;}),({ unsigned char* _temp972=( unsigned char*)""; struct
_tagged_string _temp973; _temp973.curr= _temp972; _temp973.base= _temp972;
_temp973.last_plus_one= _temp972 + 1; _temp973;}),({ unsigned char* _temp974=(
unsigned char*)","; struct _tagged_string _temp975; _temp975.curr= _temp974;
_temp975.base= _temp974; _temp975.last_plus_one= _temp974 + 2; _temp975;}),
_temp870); _temp968->tl=({ struct Cyc_List_List* _temp969=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp969->hd=( void*) Cyc_Absynpp_eff2doc((
void*)(( struct Cyc_Core_Opt*) _check_null( effopt))->v); _temp969->tl= 0;
_temp969;}); _temp968;}));}}}} struct _tuple1* Cyc_Absynpp_arg_mk_opt( struct
_tuple3* arg){ return({ struct _tuple1* _temp976=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp976->f1=({ struct Cyc_Core_Opt* _temp977=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp977->v=( void*)(*
arg).f1; _temp977;}); _temp976->f2=(* arg).f2; _temp976->f3=(* arg).f3; _temp976;});}
struct Cyc_PP_Doc* Cyc_Absynpp_var2doc( struct _tagged_string* v){ return Cyc_PP_text(*
v);} struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc( struct _tuple0* q){ struct Cyc_List_List*
_temp978= 0; goto _LL979; _LL979:{ void* _temp980=(* q).f1; struct Cyc_List_List*
_temp992; struct Cyc_List_List* _temp994; struct Cyc_List_List* _temp996; struct
Cyc_List_List* _temp998; _LL982: if( _temp980 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL983;} else{ goto _LL984;} _LL984: if(( unsigned int) _temp980 > 1u?*(( int*)
_temp980) == Cyc_Absyn_Rel_n: 0){ _LL993: _temp992=(( struct Cyc_Absyn_Rel_n_struct*)
_temp980)->f1; if( _temp992 == 0){ goto _LL985;} else{ goto _LL986;}} else{ goto
_LL986;} _LL986: if(( unsigned int) _temp980 > 1u?*(( int*) _temp980) == Cyc_Absyn_Abs_n:
0){ _LL995: _temp994=(( struct Cyc_Absyn_Abs_n_struct*) _temp980)->f1; if(
_temp994 == 0){ goto _LL987;} else{ goto _LL988;}} else{ goto _LL988;} _LL988:
if(( unsigned int) _temp980 > 1u?*(( int*) _temp980) == Cyc_Absyn_Rel_n: 0){
_LL997: _temp996=(( struct Cyc_Absyn_Rel_n_struct*) _temp980)->f1; goto _LL989;}
else{ goto _LL990;} _LL990: if(( unsigned int) _temp980 > 1u?*(( int*) _temp980)
== Cyc_Absyn_Abs_n: 0){ _LL999: _temp998=(( struct Cyc_Absyn_Abs_n_struct*)
_temp980)->f1; goto _LL991;} else{ goto _LL981;} _LL983: goto _LL985; _LL985:
goto _LL987; _LL987: return Cyc_Absynpp_var2doc((* q).f2); _LL989: _temp998=
_temp996; goto _LL991; _LL991: _temp978= _temp998; goto _LL981; _LL981:;} if(
Cyc_Absynpp_qvar_to_Cids){ return Cyc_PP_text(( struct _tagged_string) Cyc_String_str_sepstr(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp978,({ struct Cyc_List_List* _temp1000=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1000->hd=( void*)(* q).f2; _temp1000->tl= 0;
_temp1000;})),({ unsigned char* _temp1001=( unsigned char*)"_"; struct
_tagged_string _temp1002; _temp1002.curr= _temp1001; _temp1002.base= _temp1001;
_temp1002.last_plus_one= _temp1001 + 2; _temp1002;})));} else{ if((( int(*)( int(*
cmp)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List* l1,
struct Cyc_List_List* l2)) Cyc_List_list_prefix)( Cyc_String_strptrcmp, _temp978,
Cyc_Absynpp_curr_namespace)){ return Cyc_Absynpp_var2doc((* q).f2);} else{
return Cyc_PP_text(( struct _tagged_string) Cyc_String_str_sepstr((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp978,({
struct Cyc_List_List* _temp1003=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1003->hd=( void*)(* q).f2; _temp1003->tl= 0;
_temp1003;})),({ unsigned char* _temp1004=( unsigned char*)"::"; struct
_tagged_string _temp1005; _temp1005.curr= _temp1004; _temp1005.base= _temp1004;
_temp1005.last_plus_one= _temp1004 + 3; _temp1005;})));}}} struct Cyc_PP_Doc*
Cyc_Absynpp_typedef_name2doc( struct _tuple0* v){ if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_Absynpp_qvar2doc( v);}{ void* _temp1006=(* v).f1; struct Cyc_List_List*
_temp1012; _LL1008: if(( unsigned int) _temp1006 > 1u?*(( int*) _temp1006) ==
Cyc_Absyn_Abs_n: 0){ _LL1013: _temp1012=(( struct Cyc_Absyn_Abs_n_struct*)
_temp1006)->f1; goto _LL1009;} else{ goto _LL1010;} _LL1010: goto _LL1011;
_LL1009: if((( int(*)( int(* cmp)( struct _tagged_string*, struct _tagged_string*),
struct Cyc_List_List* l1, struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_String_strptrcmp,
_temp1012, Cyc_Absynpp_curr_namespace) == 0){ return Cyc_Absynpp_var2doc((* v).f2);}
else{ return({ struct Cyc_PP_Doc*(* _temp1014)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1017= Cyc_PP_text(({ unsigned char* _temp1019=(
unsigned char*)"/* wrong namespace : */ "; struct _tagged_string _temp1020;
_temp1020.curr= _temp1019; _temp1020.base= _temp1019; _temp1020.last_plus_one=
_temp1019 + 25; _temp1020;})); struct Cyc_PP_Doc* _temp1018= Cyc_Absynpp_qvar2doc(
v); struct Cyc_PP_Doc* _temp1015[ 2u]={ _temp1017, _temp1018}; struct
_tagged_ptr0 _temp1016={ _temp1015, _temp1015, _temp1015 + 2u}; _temp1014(
_temp1016);});} _LL1011: return Cyc_Absynpp_qvar2doc( v); _LL1007:;}} struct Cyc_PP_Doc*
Cyc_Absynpp_typ2doc( void* t){ return Cyc_Absynpp_tqtd2doc( Cyc_Absyn_empty_tqual(),
t, 0);} int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp* e){ void* _temp1021=(
void*) e->r; void* _temp1093; void* _temp1095; struct _tuple0* _temp1097; struct
_tuple0* _temp1099; struct Cyc_List_List* _temp1101; void* _temp1103; struct Cyc_Absyn_Exp*
_temp1105; struct Cyc_Core_Opt* _temp1107; struct Cyc_Absyn_Exp* _temp1109; void*
_temp1111; struct Cyc_Absyn_Exp* _temp1113; struct Cyc_Absyn_Exp* _temp1115;
struct Cyc_Absyn_Exp* _temp1117; struct Cyc_Absyn_Exp* _temp1119; struct Cyc_Absyn_Exp*
_temp1121; struct Cyc_Absyn_Exp* _temp1123; struct Cyc_List_List* _temp1125;
struct Cyc_Absyn_Exp* _temp1127; struct Cyc_Absyn_VarargCallInfo* _temp1129;
struct Cyc_List_List* _temp1131; struct Cyc_Absyn_Exp* _temp1133; struct Cyc_Absyn_Exp*
_temp1135; struct Cyc_Absyn_Exp* _temp1137; struct Cyc_List_List* _temp1139;
struct Cyc_Absyn_Exp* _temp1141; struct Cyc_Absyn_Exp* _temp1143; void*
_temp1145; struct Cyc_Absyn_Exp* _temp1147; struct Cyc_Absyn_Exp* _temp1149;
struct Cyc_Absyn_Exp* _temp1151; void* _temp1153; struct Cyc_Absyn_Exp*
_temp1155; struct Cyc_Absyn_Exp* _temp1157; struct _tagged_string* _temp1159;
struct Cyc_Absyn_Exp* _temp1161; struct _tagged_string* _temp1163; struct Cyc_Absyn_Exp*
_temp1165; struct Cyc_Absyn_Exp* _temp1167; struct Cyc_Absyn_Exp* _temp1169;
struct Cyc_List_List* _temp1171; struct Cyc_List_List* _temp1173; struct _tuple1*
_temp1175; struct Cyc_List_List* _temp1177; struct Cyc_Absyn_Exp* _temp1179;
struct Cyc_Absyn_Exp* _temp1181; struct Cyc_Absyn_Vardecl* _temp1183; struct Cyc_Absyn_Structdecl*
_temp1185; struct Cyc_List_List* _temp1187; struct Cyc_Core_Opt* _temp1189;
struct _tuple0* _temp1191; struct Cyc_List_List* _temp1193; void* _temp1195;
struct Cyc_Absyn_Tunionfield* _temp1197; struct Cyc_Absyn_Tuniondecl* _temp1199;
struct Cyc_List_List* _temp1201; struct Cyc_Core_Opt* _temp1203; struct Cyc_Core_Opt*
_temp1205; struct Cyc_Absyn_Enumfield* _temp1207; struct Cyc_Absyn_Enumdecl*
_temp1209; struct _tuple0* _temp1211; void* _temp1213; struct Cyc_Absyn_Exp*
_temp1215; struct Cyc_List_List* _temp1217; struct Cyc_Core_Opt* _temp1219;
struct Cyc_Absyn_Stmt* _temp1221; struct Cyc_Absyn_Fndecl* _temp1223; struct Cyc_Absyn_Exp*
_temp1225; _LL1023: if(*(( int*) _temp1021) == Cyc_Absyn_Const_e){ _LL1094:
_temp1093=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1021)->f1; goto
_LL1024;} else{ goto _LL1025;} _LL1025: if(*(( int*) _temp1021) == Cyc_Absyn_Var_e){
_LL1098: _temp1097=(( struct Cyc_Absyn_Var_e_struct*) _temp1021)->f1; goto
_LL1096; _LL1096: _temp1095=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1021)->f2;
goto _LL1026;} else{ goto _LL1027;} _LL1027: if(*(( int*) _temp1021) == Cyc_Absyn_UnknownId_e){
_LL1100: _temp1099=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1021)->f1; goto
_LL1028;} else{ goto _LL1029;} _LL1029: if(*(( int*) _temp1021) == Cyc_Absyn_Primop_e){
_LL1104: _temp1103=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1021)->f1;
goto _LL1102; _LL1102: _temp1101=(( struct Cyc_Absyn_Primop_e_struct*) _temp1021)->f2;
goto _LL1030;} else{ goto _LL1031;} _LL1031: if(*(( int*) _temp1021) == Cyc_Absyn_AssignOp_e){
_LL1110: _temp1109=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp1021)->f1; goto
_LL1108; _LL1108: _temp1107=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp1021)->f2;
goto _LL1106; _LL1106: _temp1105=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1021)->f3; goto _LL1032;} else{ goto _LL1033;} _LL1033: if(*(( int*)
_temp1021) == Cyc_Absyn_Increment_e){ _LL1114: _temp1113=(( struct Cyc_Absyn_Increment_e_struct*)
_temp1021)->f1; goto _LL1112; _LL1112: _temp1111=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1021)->f2; goto _LL1034;} else{ goto _LL1035;} _LL1035: if(*(( int*)
_temp1021) == Cyc_Absyn_Conditional_e){ _LL1120: _temp1119=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1021)->f1; goto _LL1118; _LL1118: _temp1117=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1021)->f2; goto _LL1116; _LL1116: _temp1115=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1021)->f3; goto _LL1036;} else{ goto _LL1037;} _LL1037: if(*(( int*)
_temp1021) == Cyc_Absyn_SeqExp_e){ _LL1124: _temp1123=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1021)->f1; goto _LL1122; _LL1122: _temp1121=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1021)->f2; goto _LL1038;} else{ goto _LL1039;} _LL1039: if(*(( int*)
_temp1021) == Cyc_Absyn_UnknownCall_e){ _LL1128: _temp1127=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1021)->f1; goto _LL1126; _LL1126: _temp1125=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1021)->f2; goto _LL1040;} else{ goto _LL1041;} _LL1041: if(*(( int*)
_temp1021) == Cyc_Absyn_FnCall_e){ _LL1134: _temp1133=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1021)->f1; goto _LL1132; _LL1132: _temp1131=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1021)->f2; goto _LL1130; _LL1130: _temp1129=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1021)->f3; goto _LL1042;} else{ goto _LL1043;} _LL1043: if(*(( int*)
_temp1021) == Cyc_Absyn_Throw_e){ _LL1136: _temp1135=(( struct Cyc_Absyn_Throw_e_struct*)
_temp1021)->f1; goto _LL1044;} else{ goto _LL1045;} _LL1045: if(*(( int*)
_temp1021) == Cyc_Absyn_NoInstantiate_e){ _LL1138: _temp1137=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1021)->f1; goto _LL1046;} else{ goto _LL1047;} _LL1047: if(*(( int*)
_temp1021) == Cyc_Absyn_Instantiate_e){ _LL1142: _temp1141=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1021)->f1; goto _LL1140; _LL1140: _temp1139=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1021)->f2; goto _LL1048;} else{ goto _LL1049;} _LL1049: if(*(( int*)
_temp1021) == Cyc_Absyn_Cast_e){ _LL1146: _temp1145=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1021)->f1; goto _LL1144; _LL1144: _temp1143=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1021)->f2; goto _LL1050;} else{ goto _LL1051;} _LL1051: if(*(( int*)
_temp1021) == Cyc_Absyn_New_e){ _LL1150: _temp1149=(( struct Cyc_Absyn_New_e_struct*)
_temp1021)->f1; goto _LL1148; _LL1148: _temp1147=(( struct Cyc_Absyn_New_e_struct*)
_temp1021)->f2; goto _LL1052;} else{ goto _LL1053;} _LL1053: if(*(( int*)
_temp1021) == Cyc_Absyn_Address_e){ _LL1152: _temp1151=(( struct Cyc_Absyn_Address_e_struct*)
_temp1021)->f1; goto _LL1054;} else{ goto _LL1055;} _LL1055: if(*(( int*)
_temp1021) == Cyc_Absyn_Sizeoftyp_e){ _LL1154: _temp1153=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1021)->f1; goto _LL1056;} else{ goto _LL1057;} _LL1057: if(*(( int*)
_temp1021) == Cyc_Absyn_Sizeofexp_e){ _LL1156: _temp1155=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1021)->f1; goto _LL1058;} else{ goto _LL1059;} _LL1059: if(*(( int*)
_temp1021) == Cyc_Absyn_Deref_e){ _LL1158: _temp1157=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1021)->f1; goto _LL1060;} else{ goto _LL1061;} _LL1061: if(*(( int*)
_temp1021) == Cyc_Absyn_StructMember_e){ _LL1162: _temp1161=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1021)->f1; goto _LL1160; _LL1160: _temp1159=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1021)->f2; goto _LL1062;} else{ goto _LL1063;} _LL1063: if(*(( int*)
_temp1021) == Cyc_Absyn_StructArrow_e){ _LL1166: _temp1165=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1021)->f1; goto _LL1164; _LL1164: _temp1163=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1021)->f2; goto _LL1064;} else{ goto _LL1065;} _LL1065: if(*(( int*)
_temp1021) == Cyc_Absyn_Subscript_e){ _LL1170: _temp1169=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1021)->f1; goto _LL1168; _LL1168: _temp1167=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1021)->f2; goto _LL1066;} else{ goto _LL1067;} _LL1067: if(*(( int*)
_temp1021) == Cyc_Absyn_Tuple_e){ _LL1172: _temp1171=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1021)->f1; goto _LL1068;} else{ goto _LL1069;} _LL1069: if(*(( int*)
_temp1021) == Cyc_Absyn_CompoundLit_e){ _LL1176: _temp1175=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1021)->f1; goto _LL1174; _LL1174: _temp1173=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1021)->f2; goto _LL1070;} else{ goto _LL1071;} _LL1071: if(*(( int*)
_temp1021) == Cyc_Absyn_Array_e){ _LL1178: _temp1177=(( struct Cyc_Absyn_Array_e_struct*)
_temp1021)->f1; goto _LL1072;} else{ goto _LL1073;} _LL1073: if(*(( int*)
_temp1021) == Cyc_Absyn_Comprehension_e){ _LL1184: _temp1183=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1021)->f1; goto _LL1182; _LL1182: _temp1181=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1021)->f2; goto _LL1180; _LL1180: _temp1179=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1021)->f3; goto _LL1074;} else{ goto _LL1075;} _LL1075: if(*(( int*)
_temp1021) == Cyc_Absyn_Struct_e){ _LL1192: _temp1191=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1021)->f1; goto _LL1190; _LL1190: _temp1189=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1021)->f2; goto _LL1188; _LL1188: _temp1187=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1021)->f3; goto _LL1186; _LL1186: _temp1185=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1021)->f4; goto _LL1076;} else{ goto _LL1077;} _LL1077: if(*(( int*)
_temp1021) == Cyc_Absyn_AnonStruct_e){ _LL1196: _temp1195=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1021)->f1; goto _LL1194; _LL1194: _temp1193=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1021)->f2; goto _LL1078;} else{ goto _LL1079;} _LL1079: if(*(( int*)
_temp1021) == Cyc_Absyn_Tunion_e){ _LL1206: _temp1205=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1021)->f1; goto _LL1204; _LL1204: _temp1203=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1021)->f2; goto _LL1202; _LL1202: _temp1201=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1021)->f3; goto _LL1200; _LL1200: _temp1199=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1021)->f4; goto _LL1198; _LL1198: _temp1197=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1021)->f5; goto _LL1080;} else{ goto _LL1081;} _LL1081: if(*(( int*)
_temp1021) == Cyc_Absyn_Enum_e){ _LL1212: _temp1211=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1021)->f1; goto _LL1210; _LL1210: _temp1209=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1021)->f2; goto _LL1208; _LL1208: _temp1207=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1021)->f3; goto _LL1082;} else{ goto _LL1083;} _LL1083: if(*(( int*)
_temp1021) == Cyc_Absyn_Malloc_e){ _LL1216: _temp1215=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1021)->f1; goto _LL1214; _LL1214: _temp1213=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1021)->f2; goto _LL1084;} else{ goto _LL1085;} _LL1085: if(*(( int*)
_temp1021) == Cyc_Absyn_UnresolvedMem_e){ _LL1220: _temp1219=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1021)->f1; goto _LL1218; _LL1218: _temp1217=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1021)->f2; goto _LL1086;} else{ goto _LL1087;} _LL1087: if(*(( int*)
_temp1021) == Cyc_Absyn_StmtExp_e){ _LL1222: _temp1221=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1021)->f1; goto _LL1088;} else{ goto _LL1089;} _LL1089: if(*(( int*)
_temp1021) == Cyc_Absyn_Codegen_e){ _LL1224: _temp1223=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1021)->f1; goto _LL1090;} else{ goto _LL1091;} _LL1091: if(*(( int*)
_temp1021) == Cyc_Absyn_Fill_e){ _LL1226: _temp1225=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1021)->f1; goto _LL1092;} else{ goto _LL1022;} _LL1024: return 10000;
_LL1026: return 10000; _LL1028: return 10000; _LL1030: { void* _temp1227=
_temp1103; _LL1229: if( _temp1227 ==( void*) Cyc_Absyn_Plus){ goto _LL1230;}
else{ goto _LL1231;} _LL1231: if( _temp1227 ==( void*) Cyc_Absyn_Times){ goto
_LL1232;} else{ goto _LL1233;} _LL1233: if( _temp1227 ==( void*) Cyc_Absyn_Minus){
goto _LL1234;} else{ goto _LL1235;} _LL1235: if( _temp1227 ==( void*) Cyc_Absyn_Div){
goto _LL1236;} else{ goto _LL1237;} _LL1237: if( _temp1227 ==( void*) Cyc_Absyn_Mod){
goto _LL1238;} else{ goto _LL1239;} _LL1239: if( _temp1227 ==( void*) Cyc_Absyn_Eq){
goto _LL1240;} else{ goto _LL1241;} _LL1241: if( _temp1227 ==( void*) Cyc_Absyn_Neq){
goto _LL1242;} else{ goto _LL1243;} _LL1243: if( _temp1227 ==( void*) Cyc_Absyn_Gt){
goto _LL1244;} else{ goto _LL1245;} _LL1245: if( _temp1227 ==( void*) Cyc_Absyn_Lt){
goto _LL1246;} else{ goto _LL1247;} _LL1247: if( _temp1227 ==( void*) Cyc_Absyn_Gte){
goto _LL1248;} else{ goto _LL1249;} _LL1249: if( _temp1227 ==( void*) Cyc_Absyn_Lte){
goto _LL1250;} else{ goto _LL1251;} _LL1251: if( _temp1227 ==( void*) Cyc_Absyn_Not){
goto _LL1252;} else{ goto _LL1253;} _LL1253: if( _temp1227 ==( void*) Cyc_Absyn_Bitnot){
goto _LL1254;} else{ goto _LL1255;} _LL1255: if( _temp1227 ==( void*) Cyc_Absyn_Bitand){
goto _LL1256;} else{ goto _LL1257;} _LL1257: if( _temp1227 ==( void*) Cyc_Absyn_Bitor){
goto _LL1258;} else{ goto _LL1259;} _LL1259: if( _temp1227 ==( void*) Cyc_Absyn_Bitxor){
goto _LL1260;} else{ goto _LL1261;} _LL1261: if( _temp1227 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL1262;} else{ goto _LL1263;} _LL1263: if( _temp1227 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL1264;} else{ goto _LL1265;} _LL1265: if( _temp1227 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL1266;} else{ goto _LL1267;} _LL1267: if( _temp1227 ==( void*) Cyc_Absyn_Size){
goto _LL1268;} else{ goto _LL1269;} _LL1269: if( _temp1227 ==( void*) Cyc_Absyn_Printf){
goto _LL1270;} else{ goto _LL1271;} _LL1271: if( _temp1227 ==( void*) Cyc_Absyn_Fprintf){
goto _LL1272;} else{ goto _LL1273;} _LL1273: if( _temp1227 ==( void*) Cyc_Absyn_Xprintf){
goto _LL1274;} else{ goto _LL1275;} _LL1275: if( _temp1227 ==( void*) Cyc_Absyn_Scanf){
goto _LL1276;} else{ goto _LL1277;} _LL1277: if( _temp1227 ==( void*) Cyc_Absyn_Fscanf){
goto _LL1278;} else{ goto _LL1279;} _LL1279: if( _temp1227 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1280;} else{ goto _LL1228;} _LL1230: return 100; _LL1232: return 110;
_LL1234: return 100; _LL1236: return 110; _LL1238: return 110; _LL1240: return
70; _LL1242: return 70; _LL1244: return 80; _LL1246: return 80; _LL1248: return
80; _LL1250: return 80; _LL1252: return 130; _LL1254: return 130; _LL1256:
return 60; _LL1258: return 40; _LL1260: return 50; _LL1262: return 90; _LL1264:
return 80; _LL1266: return 80; _LL1268: return 140; _LL1270: return 130; _LL1272:
return 130; _LL1274: return 130; _LL1276: return 130; _LL1278: return 130;
_LL1280: return 130; _LL1228:;} _LL1032: return 20; _LL1034: return 130; _LL1036:
return 30; _LL1038: return 10; _LL1040: return 140; _LL1042: return 140; _LL1044:
return 130; _LL1046: return Cyc_Absynpp_exp_prec( _temp1137); _LL1048: return
Cyc_Absynpp_exp_prec( _temp1141); _LL1050: return 120; _LL1052: return 15;
_LL1054: return 130; _LL1056: return 130; _LL1058: return 130; _LL1060: return
130; _LL1062: return 140; _LL1064: return 140; _LL1066: return 140; _LL1068:
return 150; _LL1070: goto _LL1072; _LL1072: goto _LL1074; _LL1074: goto _LL1076;
_LL1076: goto _LL1078; _LL1078: goto _LL1080; _LL1080: goto _LL1082; _LL1082:
goto _LL1084; _LL1084: return 140; _LL1086: return 140; _LL1088: return 10000;
_LL1090: return 140; _LL1092: return 140; _LL1022:;} struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc(
struct Cyc_Absyn_Exp* e){ return Cyc_Absynpp_exp2doc_prec( 0, e);} struct
_tuple8{ void* f1; void* f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec(
int inprec, struct Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec( e);
struct Cyc_PP_Doc* s;{ void* _temp1281=( void*) e->r; void* _temp1353; void*
_temp1355; struct _tuple0* _temp1357; struct _tuple0* _temp1359; struct Cyc_List_List*
_temp1361; void* _temp1363; struct Cyc_Absyn_Exp* _temp1365; struct Cyc_Core_Opt*
_temp1367; struct Cyc_Absyn_Exp* _temp1369; void* _temp1371; struct Cyc_Absyn_Exp*
_temp1373; struct Cyc_Absyn_Exp* _temp1375; struct Cyc_Absyn_Exp* _temp1377;
struct Cyc_Absyn_Exp* _temp1379; struct Cyc_Absyn_Exp* _temp1381; struct Cyc_Absyn_Exp*
_temp1383; struct Cyc_List_List* _temp1385; struct Cyc_Absyn_Exp* _temp1387;
struct Cyc_Absyn_VarargCallInfo* _temp1389; struct Cyc_List_List* _temp1391;
struct Cyc_Absyn_Exp* _temp1393; struct Cyc_Absyn_Exp* _temp1395; struct Cyc_Absyn_Exp*
_temp1397; struct Cyc_List_List* _temp1399; struct Cyc_Absyn_Exp* _temp1401;
struct Cyc_Absyn_Exp* _temp1403; void* _temp1405; struct Cyc_Absyn_Exp*
_temp1407; struct Cyc_Absyn_Exp* _temp1409; struct Cyc_Absyn_Exp* _temp1411;
void* _temp1413; struct Cyc_Absyn_Exp* _temp1415; struct Cyc_Absyn_Exp*
_temp1417; struct _tagged_string* _temp1419; struct Cyc_Absyn_Exp* _temp1421;
struct _tagged_string* _temp1423; struct Cyc_Absyn_Exp* _temp1425; struct Cyc_Absyn_Exp*
_temp1427; struct Cyc_Absyn_Exp* _temp1429; struct Cyc_List_List* _temp1431;
struct Cyc_List_List* _temp1433; struct _tuple1* _temp1435; struct Cyc_List_List*
_temp1437; struct Cyc_Absyn_Exp* _temp1439; struct Cyc_Absyn_Exp* _temp1441;
struct Cyc_Absyn_Vardecl* _temp1443; struct Cyc_Absyn_Structdecl* _temp1445;
struct Cyc_List_List* _temp1447; struct Cyc_Core_Opt* _temp1449; struct _tuple0*
_temp1451; struct Cyc_List_List* _temp1453; void* _temp1455; struct Cyc_Absyn_Tunionfield*
_temp1457; struct Cyc_Absyn_Tuniondecl* _temp1459; struct Cyc_List_List*
_temp1461; struct Cyc_Core_Opt* _temp1463; struct Cyc_Core_Opt* _temp1465;
struct Cyc_Absyn_Enumfield* _temp1467; struct Cyc_Absyn_Enumdecl* _temp1469;
struct _tuple0* _temp1471; void* _temp1473; struct Cyc_Absyn_Exp* _temp1475;
struct Cyc_List_List* _temp1477; struct Cyc_Core_Opt* _temp1479; struct Cyc_Absyn_Stmt*
_temp1481; struct Cyc_Absyn_Fndecl* _temp1483; struct Cyc_Absyn_Exp* _temp1485;
_LL1283: if(*(( int*) _temp1281) == Cyc_Absyn_Const_e){ _LL1354: _temp1353=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1281)->f1; goto _LL1284;} else{
goto _LL1285;} _LL1285: if(*(( int*) _temp1281) == Cyc_Absyn_Var_e){ _LL1358:
_temp1357=(( struct Cyc_Absyn_Var_e_struct*) _temp1281)->f1; goto _LL1356;
_LL1356: _temp1355=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1281)->f2;
goto _LL1286;} else{ goto _LL1287;} _LL1287: if(*(( int*) _temp1281) == Cyc_Absyn_UnknownId_e){
_LL1360: _temp1359=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1281)->f1; goto
_LL1288;} else{ goto _LL1289;} _LL1289: if(*(( int*) _temp1281) == Cyc_Absyn_Primop_e){
_LL1364: _temp1363=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1281)->f1;
goto _LL1362; _LL1362: _temp1361=(( struct Cyc_Absyn_Primop_e_struct*) _temp1281)->f2;
goto _LL1290;} else{ goto _LL1291;} _LL1291: if(*(( int*) _temp1281) == Cyc_Absyn_AssignOp_e){
_LL1370: _temp1369=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp1281)->f1; goto
_LL1368; _LL1368: _temp1367=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp1281)->f2;
goto _LL1366; _LL1366: _temp1365=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1281)->f3; goto _LL1292;} else{ goto _LL1293;} _LL1293: if(*(( int*)
_temp1281) == Cyc_Absyn_Increment_e){ _LL1374: _temp1373=(( struct Cyc_Absyn_Increment_e_struct*)
_temp1281)->f1; goto _LL1372; _LL1372: _temp1371=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1281)->f2; goto _LL1294;} else{ goto _LL1295;} _LL1295: if(*(( int*)
_temp1281) == Cyc_Absyn_Conditional_e){ _LL1380: _temp1379=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1281)->f1; goto _LL1378; _LL1378: _temp1377=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1281)->f2; goto _LL1376; _LL1376: _temp1375=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1281)->f3; goto _LL1296;} else{ goto _LL1297;} _LL1297: if(*(( int*)
_temp1281) == Cyc_Absyn_SeqExp_e){ _LL1384: _temp1383=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1281)->f1; goto _LL1382; _LL1382: _temp1381=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1281)->f2; goto _LL1298;} else{ goto _LL1299;} _LL1299: if(*(( int*)
_temp1281) == Cyc_Absyn_UnknownCall_e){ _LL1388: _temp1387=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1281)->f1; goto _LL1386; _LL1386: _temp1385=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1281)->f2; goto _LL1300;} else{ goto _LL1301;} _LL1301: if(*(( int*)
_temp1281) == Cyc_Absyn_FnCall_e){ _LL1394: _temp1393=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1281)->f1; goto _LL1392; _LL1392: _temp1391=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1281)->f2; goto _LL1390; _LL1390: _temp1389=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1281)->f3; goto _LL1302;} else{ goto _LL1303;} _LL1303: if(*(( int*)
_temp1281) == Cyc_Absyn_Throw_e){ _LL1396: _temp1395=(( struct Cyc_Absyn_Throw_e_struct*)
_temp1281)->f1; goto _LL1304;} else{ goto _LL1305;} _LL1305: if(*(( int*)
_temp1281) == Cyc_Absyn_NoInstantiate_e){ _LL1398: _temp1397=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1281)->f1; goto _LL1306;} else{ goto _LL1307;} _LL1307: if(*(( int*)
_temp1281) == Cyc_Absyn_Instantiate_e){ _LL1402: _temp1401=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1281)->f1; goto _LL1400; _LL1400: _temp1399=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1281)->f2; goto _LL1308;} else{ goto _LL1309;} _LL1309: if(*(( int*)
_temp1281) == Cyc_Absyn_Cast_e){ _LL1406: _temp1405=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1281)->f1; goto _LL1404; _LL1404: _temp1403=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1281)->f2; goto _LL1310;} else{ goto _LL1311;} _LL1311: if(*(( int*)
_temp1281) == Cyc_Absyn_Address_e){ _LL1408: _temp1407=(( struct Cyc_Absyn_Address_e_struct*)
_temp1281)->f1; goto _LL1312;} else{ goto _LL1313;} _LL1313: if(*(( int*)
_temp1281) == Cyc_Absyn_New_e){ _LL1412: _temp1411=(( struct Cyc_Absyn_New_e_struct*)
_temp1281)->f1; goto _LL1410; _LL1410: _temp1409=(( struct Cyc_Absyn_New_e_struct*)
_temp1281)->f2; goto _LL1314;} else{ goto _LL1315;} _LL1315: if(*(( int*)
_temp1281) == Cyc_Absyn_Sizeoftyp_e){ _LL1414: _temp1413=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1281)->f1; goto _LL1316;} else{ goto _LL1317;} _LL1317: if(*(( int*)
_temp1281) == Cyc_Absyn_Sizeofexp_e){ _LL1416: _temp1415=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1281)->f1; goto _LL1318;} else{ goto _LL1319;} _LL1319: if(*(( int*)
_temp1281) == Cyc_Absyn_Deref_e){ _LL1418: _temp1417=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1281)->f1; goto _LL1320;} else{ goto _LL1321;} _LL1321: if(*(( int*)
_temp1281) == Cyc_Absyn_StructMember_e){ _LL1422: _temp1421=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1281)->f1; goto _LL1420; _LL1420: _temp1419=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1281)->f2; goto _LL1322;} else{ goto _LL1323;} _LL1323: if(*(( int*)
_temp1281) == Cyc_Absyn_StructArrow_e){ _LL1426: _temp1425=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1281)->f1; goto _LL1424; _LL1424: _temp1423=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1281)->f2; goto _LL1324;} else{ goto _LL1325;} _LL1325: if(*(( int*)
_temp1281) == Cyc_Absyn_Subscript_e){ _LL1430: _temp1429=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1281)->f1; goto _LL1428; _LL1428: _temp1427=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1281)->f2; goto _LL1326;} else{ goto _LL1327;} _LL1327: if(*(( int*)
_temp1281) == Cyc_Absyn_Tuple_e){ _LL1432: _temp1431=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1281)->f1; goto _LL1328;} else{ goto _LL1329;} _LL1329: if(*(( int*)
_temp1281) == Cyc_Absyn_CompoundLit_e){ _LL1436: _temp1435=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1281)->f1; goto _LL1434; _LL1434: _temp1433=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1281)->f2; goto _LL1330;} else{ goto _LL1331;} _LL1331: if(*(( int*)
_temp1281) == Cyc_Absyn_Array_e){ _LL1438: _temp1437=(( struct Cyc_Absyn_Array_e_struct*)
_temp1281)->f1; goto _LL1332;} else{ goto _LL1333;} _LL1333: if(*(( int*)
_temp1281) == Cyc_Absyn_Comprehension_e){ _LL1444: _temp1443=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1281)->f1; goto _LL1442; _LL1442: _temp1441=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1281)->f2; goto _LL1440; _LL1440: _temp1439=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1281)->f3; goto _LL1334;} else{ goto _LL1335;} _LL1335: if(*(( int*)
_temp1281) == Cyc_Absyn_Struct_e){ _LL1452: _temp1451=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1281)->f1; goto _LL1450; _LL1450: _temp1449=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1281)->f2; goto _LL1448; _LL1448: _temp1447=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1281)->f3; goto _LL1446; _LL1446: _temp1445=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1281)->f4; goto _LL1336;} else{ goto _LL1337;} _LL1337: if(*(( int*)
_temp1281) == Cyc_Absyn_AnonStruct_e){ _LL1456: _temp1455=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1281)->f1; goto _LL1454; _LL1454: _temp1453=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1281)->f2; goto _LL1338;} else{ goto _LL1339;} _LL1339: if(*(( int*)
_temp1281) == Cyc_Absyn_Tunion_e){ _LL1466: _temp1465=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1281)->f1; goto _LL1464; _LL1464: _temp1463=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1281)->f2; goto _LL1462; _LL1462: _temp1461=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1281)->f3; goto _LL1460; _LL1460: _temp1459=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1281)->f4; goto _LL1458; _LL1458: _temp1457=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1281)->f5; goto _LL1340;} else{ goto _LL1341;} _LL1341: if(*(( int*)
_temp1281) == Cyc_Absyn_Enum_e){ _LL1472: _temp1471=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1281)->f1; goto _LL1470; _LL1470: _temp1469=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1281)->f2; goto _LL1468; _LL1468: _temp1467=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1281)->f3; goto _LL1342;} else{ goto _LL1343;} _LL1343: if(*(( int*)
_temp1281) == Cyc_Absyn_Malloc_e){ _LL1476: _temp1475=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1281)->f1; goto _LL1474; _LL1474: _temp1473=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1281)->f2; goto _LL1344;} else{ goto _LL1345;} _LL1345: if(*(( int*)
_temp1281) == Cyc_Absyn_UnresolvedMem_e){ _LL1480: _temp1479=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1281)->f1; goto _LL1478; _LL1478: _temp1477=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1281)->f2; goto _LL1346;} else{ goto _LL1347;} _LL1347: if(*(( int*)
_temp1281) == Cyc_Absyn_StmtExp_e){ _LL1482: _temp1481=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1281)->f1; goto _LL1348;} else{ goto _LL1349;} _LL1349: if(*(( int*)
_temp1281) == Cyc_Absyn_Codegen_e){ _LL1484: _temp1483=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1281)->f1; goto _LL1350;} else{ goto _LL1351;} _LL1351: if(*(( int*)
_temp1281) == Cyc_Absyn_Fill_e){ _LL1486: _temp1485=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1281)->f1; goto _LL1352;} else{ goto _LL1282;} _LL1284: s= Cyc_Absynpp_cnst2doc(
_temp1353); goto _LL1282; _LL1286: _temp1359= _temp1357; goto _LL1288; _LL1288:
s= Cyc_Absynpp_qvar2doc( _temp1359); goto _LL1282; _LL1290: s= Cyc_Absynpp_primapp2doc(
myprec, _temp1363, _temp1361); goto _LL1282; _LL1292: s=({ struct Cyc_PP_Doc*(*
_temp1487)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1490= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1369); struct Cyc_PP_Doc* _temp1491= Cyc_PP_text(({ unsigned char*
_temp1499=( unsigned char*)" "; struct _tagged_string _temp1500; _temp1500.curr=
_temp1499; _temp1500.base= _temp1499; _temp1500.last_plus_one= _temp1499 + 2;
_temp1500;})); struct Cyc_PP_Doc* _temp1492= _temp1367 == 0? Cyc_PP_text(({
unsigned char* _temp1497=( unsigned char*)""; struct _tagged_string _temp1498;
_temp1498.curr= _temp1497; _temp1498.base= _temp1497; _temp1498.last_plus_one=
_temp1497 + 1; _temp1498;})): Cyc_Absynpp_prim2doc(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1367))->v); struct Cyc_PP_Doc* _temp1493= Cyc_PP_text(({
unsigned char* _temp1495=( unsigned char*)"= "; struct _tagged_string _temp1496;
_temp1496.curr= _temp1495; _temp1496.base= _temp1495; _temp1496.last_plus_one=
_temp1495 + 3; _temp1496;})); struct Cyc_PP_Doc* _temp1494= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1365); struct Cyc_PP_Doc* _temp1488[ 5u]={ _temp1490, _temp1491,
_temp1492, _temp1493, _temp1494}; struct _tagged_ptr0 _temp1489={ _temp1488,
_temp1488, _temp1488 + 5u}; _temp1487( _temp1489);}); goto _LL1282; _LL1294: {
struct Cyc_PP_Doc* _temp1501= Cyc_Absynpp_exp2doc_prec( myprec, _temp1373); goto
_LL1502; _LL1502:{ void* _temp1503= _temp1371; _LL1505: if( _temp1503 ==( void*)
Cyc_Absyn_PreInc){ goto _LL1506;} else{ goto _LL1507;} _LL1507: if( _temp1503 ==(
void*) Cyc_Absyn_PreDec){ goto _LL1508;} else{ goto _LL1509;} _LL1509: if(
_temp1503 ==( void*) Cyc_Absyn_PostInc){ goto _LL1510;} else{ goto _LL1511;}
_LL1511: if( _temp1503 ==( void*) Cyc_Absyn_PostDec){ goto _LL1512;} else{ goto
_LL1504;} _LL1506: s=({ struct Cyc_PP_Doc*(* _temp1513)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1516= Cyc_PP_text(({ unsigned char*
_temp1518=( unsigned char*)"++"; struct _tagged_string _temp1519; _temp1519.curr=
_temp1518; _temp1519.base= _temp1518; _temp1519.last_plus_one= _temp1518 + 3;
_temp1519;})); struct Cyc_PP_Doc* _temp1517= _temp1501; struct Cyc_PP_Doc*
_temp1514[ 2u]={ _temp1516, _temp1517}; struct _tagged_ptr0 _temp1515={
_temp1514, _temp1514, _temp1514 + 2u}; _temp1513( _temp1515);}); goto _LL1504;
_LL1508: s=({ struct Cyc_PP_Doc*(* _temp1520)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1523= Cyc_PP_text(({ unsigned char* _temp1525=(
unsigned char*)"--"; struct _tagged_string _temp1526; _temp1526.curr= _temp1525;
_temp1526.base= _temp1525; _temp1526.last_plus_one= _temp1525 + 3; _temp1526;}));
struct Cyc_PP_Doc* _temp1524= _temp1501; struct Cyc_PP_Doc* _temp1521[ 2u]={
_temp1523, _temp1524}; struct _tagged_ptr0 _temp1522={ _temp1521, _temp1521,
_temp1521 + 2u}; _temp1520( _temp1522);}); goto _LL1504; _LL1510: s=({ struct
Cyc_PP_Doc*(* _temp1527)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1530= _temp1501; struct Cyc_PP_Doc* _temp1531= Cyc_PP_text(({ unsigned char*
_temp1532=( unsigned char*)"++"; struct _tagged_string _temp1533; _temp1533.curr=
_temp1532; _temp1533.base= _temp1532; _temp1533.last_plus_one= _temp1532 + 3;
_temp1533;})); struct Cyc_PP_Doc* _temp1528[ 2u]={ _temp1530, _temp1531}; struct
_tagged_ptr0 _temp1529={ _temp1528, _temp1528, _temp1528 + 2u}; _temp1527(
_temp1529);}); goto _LL1504; _LL1512: s=({ struct Cyc_PP_Doc*(* _temp1534)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1537= _temp1501;
struct Cyc_PP_Doc* _temp1538= Cyc_PP_text(({ unsigned char* _temp1539=(
unsigned char*)"--"; struct _tagged_string _temp1540; _temp1540.curr= _temp1539;
_temp1540.base= _temp1539; _temp1540.last_plus_one= _temp1539 + 3; _temp1540;}));
struct Cyc_PP_Doc* _temp1535[ 2u]={ _temp1537, _temp1538}; struct _tagged_ptr0
_temp1536={ _temp1535, _temp1535, _temp1535 + 2u}; _temp1534( _temp1536);});
goto _LL1504; _LL1504:;} goto _LL1282;} _LL1296:{ struct _tuple8 _temp1542=({
struct _tuple8 _temp1541; _temp1541.f1=( void*) _temp1377->r; _temp1541.f2=(
void*) _temp1375->r; _temp1541;}); _LL1544: goto _LL1545; _LL1545: s=({ struct
Cyc_PP_Doc*(* _temp1546)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1549= Cyc_Absynpp_exp2doc_prec( myprec, _temp1379); struct Cyc_PP_Doc*
_temp1550= Cyc_PP_text(({ unsigned char* _temp1556=( unsigned char*)" ? ";
struct _tagged_string _temp1557; _temp1557.curr= _temp1556; _temp1557.base=
_temp1556; _temp1557.last_plus_one= _temp1556 + 4; _temp1557;})); struct Cyc_PP_Doc*
_temp1551= Cyc_Absynpp_exp2doc_prec( 0, _temp1377); struct Cyc_PP_Doc* _temp1552=
Cyc_PP_text(({ unsigned char* _temp1554=( unsigned char*)" : "; struct
_tagged_string _temp1555; _temp1555.curr= _temp1554; _temp1555.base= _temp1554;
_temp1555.last_plus_one= _temp1554 + 4; _temp1555;})); struct Cyc_PP_Doc*
_temp1553= Cyc_Absynpp_exp2doc_prec( myprec, _temp1375); struct Cyc_PP_Doc*
_temp1547[ 5u]={ _temp1549, _temp1550, _temp1551, _temp1552, _temp1553}; struct
_tagged_ptr0 _temp1548={ _temp1547, _temp1547, _temp1547 + 5u}; _temp1546(
_temp1548);}); goto _LL1543; _LL1543:;} goto _LL1282; _LL1298: s=({ struct Cyc_PP_Doc*(*
_temp1558)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1561= Cyc_PP_text(({
unsigned char* _temp1570=( unsigned char*)"("; struct _tagged_string _temp1571;
_temp1571.curr= _temp1570; _temp1571.base= _temp1570; _temp1571.last_plus_one=
_temp1570 + 2; _temp1571;})); struct Cyc_PP_Doc* _temp1562= Cyc_Absynpp_exp2doc(
_temp1383); struct Cyc_PP_Doc* _temp1563= Cyc_PP_text(({ unsigned char*
_temp1568=( unsigned char*)", "; struct _tagged_string _temp1569; _temp1569.curr=
_temp1568; _temp1569.base= _temp1568; _temp1569.last_plus_one= _temp1568 + 3;
_temp1569;})); struct Cyc_PP_Doc* _temp1564= Cyc_Absynpp_exp2doc( _temp1381);
struct Cyc_PP_Doc* _temp1565= Cyc_PP_text(({ unsigned char* _temp1566=(
unsigned char*)")"; struct _tagged_string _temp1567; _temp1567.curr= _temp1566;
_temp1567.base= _temp1566; _temp1567.last_plus_one= _temp1566 + 2; _temp1567;}));
struct Cyc_PP_Doc* _temp1559[ 5u]={ _temp1561, _temp1562, _temp1563, _temp1564,
_temp1565}; struct _tagged_ptr0 _temp1560={ _temp1559, _temp1559, _temp1559 + 5u};
_temp1558( _temp1560);}); goto _LL1282; _LL1300: s=({ struct Cyc_PP_Doc*(*
_temp1572)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1575= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1387); struct Cyc_PP_Doc* _temp1576= Cyc_PP_text(({ unsigned char*
_temp1581=( unsigned char*)"("; struct _tagged_string _temp1582; _temp1582.curr=
_temp1581; _temp1582.base= _temp1581; _temp1582.last_plus_one= _temp1581 + 2;
_temp1582;})); struct Cyc_PP_Doc* _temp1577= Cyc_Absynpp_exps2doc_prec( 20,
_temp1385); struct Cyc_PP_Doc* _temp1578= Cyc_PP_text(({ unsigned char*
_temp1579=( unsigned char*)")"; struct _tagged_string _temp1580; _temp1580.curr=
_temp1579; _temp1580.base= _temp1579; _temp1580.last_plus_one= _temp1579 + 2;
_temp1580;})); struct Cyc_PP_Doc* _temp1573[ 4u]={ _temp1575, _temp1576,
_temp1577, _temp1578}; struct _tagged_ptr0 _temp1574={ _temp1573, _temp1573,
_temp1573 + 4u}; _temp1572( _temp1574);}); goto _LL1282; _LL1302: s=({ struct
Cyc_PP_Doc*(* _temp1583)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1586= Cyc_Absynpp_exp2doc_prec( myprec, _temp1393); struct Cyc_PP_Doc*
_temp1587= Cyc_PP_text(({ unsigned char* _temp1592=( unsigned char*)"("; struct
_tagged_string _temp1593; _temp1593.curr= _temp1592; _temp1593.base= _temp1592;
_temp1593.last_plus_one= _temp1592 + 2; _temp1593;})); struct Cyc_PP_Doc*
_temp1588= Cyc_Absynpp_exps2doc_prec( 20, _temp1391); struct Cyc_PP_Doc*
_temp1589= Cyc_PP_text(({ unsigned char* _temp1590=( unsigned char*)")"; struct
_tagged_string _temp1591; _temp1591.curr= _temp1590; _temp1591.base= _temp1590;
_temp1591.last_plus_one= _temp1590 + 2; _temp1591;})); struct Cyc_PP_Doc*
_temp1584[ 4u]={ _temp1586, _temp1587, _temp1588, _temp1589}; struct
_tagged_ptr0 _temp1585={ _temp1584, _temp1584, _temp1584 + 4u}; _temp1583(
_temp1585);}); goto _LL1282; _LL1304: s=({ struct Cyc_PP_Doc*(* _temp1594)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1597= Cyc_PP_text(({
unsigned char* _temp1599=( unsigned char*)"throw "; struct _tagged_string
_temp1600; _temp1600.curr= _temp1599; _temp1600.base= _temp1599; _temp1600.last_plus_one=
_temp1599 + 7; _temp1600;})); struct Cyc_PP_Doc* _temp1598= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1395); struct Cyc_PP_Doc* _temp1595[ 2u]={ _temp1597, _temp1598};
struct _tagged_ptr0 _temp1596={ _temp1595, _temp1595, _temp1595 + 2u}; _temp1594(
_temp1596);}); goto _LL1282; _LL1306: s= Cyc_Absynpp_exp2doc_prec( inprec,
_temp1397); goto _LL1282; _LL1308: s= Cyc_Absynpp_exp2doc_prec( inprec,
_temp1401); goto _LL1282; _LL1310: s=({ struct Cyc_PP_Doc*(* _temp1601)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1604= Cyc_PP_text(({
unsigned char* _temp1610=( unsigned char*)"("; struct _tagged_string _temp1611;
_temp1611.curr= _temp1610; _temp1611.base= _temp1610; _temp1611.last_plus_one=
_temp1610 + 2; _temp1611;})); struct Cyc_PP_Doc* _temp1605= Cyc_Absynpp_typ2doc(
_temp1405); struct Cyc_PP_Doc* _temp1606= Cyc_PP_text(({ unsigned char*
_temp1608=( unsigned char*)")"; struct _tagged_string _temp1609; _temp1609.curr=
_temp1608; _temp1609.base= _temp1608; _temp1609.last_plus_one= _temp1608 + 2;
_temp1609;})); struct Cyc_PP_Doc* _temp1607= Cyc_Absynpp_exp2doc_prec( myprec,
_temp1403); struct Cyc_PP_Doc* _temp1602[ 4u]={ _temp1604, _temp1605, _temp1606,
_temp1607}; struct _tagged_ptr0 _temp1603={ _temp1602, _temp1602, _temp1602 + 4u};
_temp1601( _temp1603);}); goto _LL1282; _LL1312: s=({ struct Cyc_PP_Doc*(*
_temp1612)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1615= Cyc_PP_text(({
unsigned char* _temp1617=( unsigned char*)"&"; struct _tagged_string _temp1618;
_temp1618.curr= _temp1617; _temp1618.base= _temp1617; _temp1618.last_plus_one=
_temp1617 + 2; _temp1618;})); struct Cyc_PP_Doc* _temp1616= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1407); struct Cyc_PP_Doc* _temp1613[ 2u]={ _temp1615, _temp1616};
struct _tagged_ptr0 _temp1614={ _temp1613, _temp1613, _temp1613 + 2u}; _temp1612(
_temp1614);}); goto _LL1282; _LL1314: if( _temp1411 == 0){ s=({ struct Cyc_PP_Doc*(*
_temp1619)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1622= Cyc_PP_text(({
unsigned char* _temp1624=( unsigned char*)"new "; struct _tagged_string
_temp1625; _temp1625.curr= _temp1624; _temp1625.base= _temp1624; _temp1625.last_plus_one=
_temp1624 + 5; _temp1625;})); struct Cyc_PP_Doc* _temp1623= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1409); struct Cyc_PP_Doc* _temp1620[ 2u]={ _temp1622, _temp1623};
struct _tagged_ptr0 _temp1621={ _temp1620, _temp1620, _temp1620 + 2u}; _temp1619(
_temp1621);});} else{ s=({ struct Cyc_PP_Doc*(* _temp1626)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1629= Cyc_PP_text(({ unsigned char*
_temp1635=( unsigned char*)"rnew("; struct _tagged_string _temp1636; _temp1636.curr=
_temp1635; _temp1636.base= _temp1635; _temp1636.last_plus_one= _temp1635 + 6;
_temp1636;})); struct Cyc_PP_Doc* _temp1630= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)
_check_null( _temp1411)); struct Cyc_PP_Doc* _temp1631= Cyc_PP_text(({
unsigned char* _temp1633=( unsigned char*)") "; struct _tagged_string _temp1634;
_temp1634.curr= _temp1633; _temp1634.base= _temp1633; _temp1634.last_plus_one=
_temp1633 + 3; _temp1634;})); struct Cyc_PP_Doc* _temp1632= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1409); struct Cyc_PP_Doc* _temp1627[ 4u]={ _temp1629, _temp1630,
_temp1631, _temp1632}; struct _tagged_ptr0 _temp1628={ _temp1627, _temp1627,
_temp1627 + 4u}; _temp1626( _temp1628);});} goto _LL1282; _LL1316: s=({ struct
Cyc_PP_Doc*(* _temp1637)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1640= Cyc_PP_text(({ unsigned char* _temp1645=( unsigned char*)"sizeof(";
struct _tagged_string _temp1646; _temp1646.curr= _temp1645; _temp1646.base=
_temp1645; _temp1646.last_plus_one= _temp1645 + 8; _temp1646;})); struct Cyc_PP_Doc*
_temp1641= Cyc_Absynpp_typ2doc( _temp1413); struct Cyc_PP_Doc* _temp1642= Cyc_PP_text(({
unsigned char* _temp1643=( unsigned char*)")"; struct _tagged_string _temp1644;
_temp1644.curr= _temp1643; _temp1644.base= _temp1643; _temp1644.last_plus_one=
_temp1643 + 2; _temp1644;})); struct Cyc_PP_Doc* _temp1638[ 3u]={ _temp1640,
_temp1641, _temp1642}; struct _tagged_ptr0 _temp1639={ _temp1638, _temp1638,
_temp1638 + 3u}; _temp1637( _temp1639);}); goto _LL1282; _LL1318: s=({ struct
Cyc_PP_Doc*(* _temp1647)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1650= Cyc_PP_text(({ unsigned char* _temp1655=( unsigned char*)"sizeof(";
struct _tagged_string _temp1656; _temp1656.curr= _temp1655; _temp1656.base=
_temp1655; _temp1656.last_plus_one= _temp1655 + 8; _temp1656;})); struct Cyc_PP_Doc*
_temp1651= Cyc_Absynpp_exp2doc( _temp1415); struct Cyc_PP_Doc* _temp1652= Cyc_PP_text(({
unsigned char* _temp1653=( unsigned char*)")"; struct _tagged_string _temp1654;
_temp1654.curr= _temp1653; _temp1654.base= _temp1653; _temp1654.last_plus_one=
_temp1653 + 2; _temp1654;})); struct Cyc_PP_Doc* _temp1648[ 3u]={ _temp1650,
_temp1651, _temp1652}; struct _tagged_ptr0 _temp1649={ _temp1648, _temp1648,
_temp1648 + 3u}; _temp1647( _temp1649);}); goto _LL1282; _LL1320: s=({ struct
Cyc_PP_Doc*(* _temp1657)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1660= Cyc_PP_text(({ unsigned char* _temp1662=( unsigned char*)"*"; struct
_tagged_string _temp1663; _temp1663.curr= _temp1662; _temp1663.base= _temp1662;
_temp1663.last_plus_one= _temp1662 + 2; _temp1663;})); struct Cyc_PP_Doc*
_temp1661= Cyc_Absynpp_exp2doc_prec( myprec, _temp1417); struct Cyc_PP_Doc*
_temp1658[ 2u]={ _temp1660, _temp1661}; struct _tagged_ptr0 _temp1659={
_temp1658, _temp1658, _temp1658 + 2u}; _temp1657( _temp1659);}); goto _LL1282;
_LL1322: s=({ struct Cyc_PP_Doc*(* _temp1664)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1667= Cyc_Absynpp_exp2doc_prec( myprec, _temp1421);
struct Cyc_PP_Doc* _temp1668= Cyc_PP_text(({ unsigned char* _temp1670=(
unsigned char*)"."; struct _tagged_string _temp1671; _temp1671.curr= _temp1670;
_temp1671.base= _temp1670; _temp1671.last_plus_one= _temp1670 + 2; _temp1671;}));
struct Cyc_PP_Doc* _temp1669= Cyc_PP_textptr( _temp1419); struct Cyc_PP_Doc*
_temp1665[ 3u]={ _temp1667, _temp1668, _temp1669}; struct _tagged_ptr0 _temp1666={
_temp1665, _temp1665, _temp1665 + 3u}; _temp1664( _temp1666);}); goto _LL1282;
_LL1324: s=({ struct Cyc_PP_Doc*(* _temp1672)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1675= Cyc_Absynpp_exp2doc_prec( myprec, _temp1425);
struct Cyc_PP_Doc* _temp1676= Cyc_PP_text(({ unsigned char* _temp1678=(
unsigned char*)"->"; struct _tagged_string _temp1679; _temp1679.curr= _temp1678;
_temp1679.base= _temp1678; _temp1679.last_plus_one= _temp1678 + 3; _temp1679;}));
struct Cyc_PP_Doc* _temp1677= Cyc_PP_textptr( _temp1423); struct Cyc_PP_Doc*
_temp1673[ 3u]={ _temp1675, _temp1676, _temp1677}; struct _tagged_ptr0 _temp1674={
_temp1673, _temp1673, _temp1673 + 3u}; _temp1672( _temp1674);}); goto _LL1282;
_LL1326: s=({ struct Cyc_PP_Doc*(* _temp1680)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1683= Cyc_Absynpp_exp2doc_prec( myprec, _temp1429);
struct Cyc_PP_Doc* _temp1684= Cyc_PP_text(({ unsigned char* _temp1689=(
unsigned char*)"["; struct _tagged_string _temp1690; _temp1690.curr= _temp1689;
_temp1690.base= _temp1689; _temp1690.last_plus_one= _temp1689 + 2; _temp1690;}));
struct Cyc_PP_Doc* _temp1685= Cyc_Absynpp_exp2doc( _temp1427); struct Cyc_PP_Doc*
_temp1686= Cyc_PP_text(({ unsigned char* _temp1687=( unsigned char*)"]"; struct
_tagged_string _temp1688; _temp1688.curr= _temp1687; _temp1688.base= _temp1687;
_temp1688.last_plus_one= _temp1687 + 2; _temp1688;})); struct Cyc_PP_Doc*
_temp1681[ 4u]={ _temp1683, _temp1684, _temp1685, _temp1686}; struct
_tagged_ptr0 _temp1682={ _temp1681, _temp1681, _temp1681 + 4u}; _temp1680(
_temp1682);}); goto _LL1282; _LL1328: s=({ struct Cyc_PP_Doc*(* _temp1691)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1694= Cyc_PP_text(({
unsigned char* _temp1699=( unsigned char*)"$("; struct _tagged_string _temp1700;
_temp1700.curr= _temp1699; _temp1700.base= _temp1699; _temp1700.last_plus_one=
_temp1699 + 3; _temp1700;})); struct Cyc_PP_Doc* _temp1695= Cyc_Absynpp_exps2doc_prec(
20, _temp1431); struct Cyc_PP_Doc* _temp1696= Cyc_PP_text(({ unsigned char*
_temp1697=( unsigned char*)")"; struct _tagged_string _temp1698; _temp1698.curr=
_temp1697; _temp1698.base= _temp1697; _temp1698.last_plus_one= _temp1697 + 2;
_temp1698;})); struct Cyc_PP_Doc* _temp1692[ 3u]={ _temp1694, _temp1695,
_temp1696}; struct _tagged_ptr0 _temp1693={ _temp1692, _temp1692, _temp1692 + 3u};
_temp1691( _temp1693);}); goto _LL1282; _LL1330: s=({ struct Cyc_PP_Doc*(*
_temp1701)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1704= Cyc_PP_text(({
unsigned char* _temp1716=( unsigned char*)"("; struct _tagged_string _temp1717;
_temp1717.curr= _temp1716; _temp1717.base= _temp1716; _temp1717.last_plus_one=
_temp1716 + 2; _temp1717;})); struct Cyc_PP_Doc* _temp1705= Cyc_Absynpp_typ2doc((*
_temp1435).f3); struct Cyc_PP_Doc* _temp1706= Cyc_PP_text(({ unsigned char*
_temp1714=( unsigned char*)")"; struct _tagged_string _temp1715; _temp1715.curr=
_temp1714; _temp1715.base= _temp1714; _temp1715.last_plus_one= _temp1714 + 2;
_temp1715;})); struct Cyc_PP_Doc* _temp1707= Cyc_PP_group(({ unsigned char*
_temp1708=( unsigned char*)"{"; struct _tagged_string _temp1709; _temp1709.curr=
_temp1708; _temp1709.base= _temp1708; _temp1709.last_plus_one= _temp1708 + 2;
_temp1709;}),({ unsigned char* _temp1710=( unsigned char*)"}"; struct
_tagged_string _temp1711; _temp1711.curr= _temp1710; _temp1711.base= _temp1710;
_temp1711.last_plus_one= _temp1710 + 2; _temp1711;}),({ unsigned char* _temp1712=(
unsigned char*)","; struct _tagged_string _temp1713; _temp1713.curr= _temp1712;
_temp1713.base= _temp1712; _temp1713.last_plus_one= _temp1712 + 2; _temp1713;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_de2doc, _temp1433)); struct Cyc_PP_Doc* _temp1702[
4u]={ _temp1704, _temp1705, _temp1706, _temp1707}; struct _tagged_ptr0 _temp1703={
_temp1702, _temp1702, _temp1702 + 4u}; _temp1701( _temp1703);}); goto _LL1282;
_LL1332: s= Cyc_PP_group(({ unsigned char* _temp1718=( unsigned char*)"{";
struct _tagged_string _temp1719; _temp1719.curr= _temp1718; _temp1719.base=
_temp1718; _temp1719.last_plus_one= _temp1718 + 2; _temp1719;}),({ unsigned char*
_temp1720=( unsigned char*)"}"; struct _tagged_string _temp1721; _temp1721.curr=
_temp1720; _temp1721.base= _temp1720; _temp1721.last_plus_one= _temp1720 + 2;
_temp1721;}),({ unsigned char* _temp1722=( unsigned char*)","; struct
_tagged_string _temp1723; _temp1723.curr= _temp1722; _temp1723.base= _temp1722;
_temp1723.last_plus_one= _temp1722 + 2; _temp1723;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp1437)); goto _LL1282; _LL1334: s=({ struct Cyc_PP_Doc*(*
_temp1724)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1727= Cyc_PP_text(({
unsigned char* _temp1740=( unsigned char*)"{for "; struct _tagged_string
_temp1741; _temp1741.curr= _temp1740; _temp1741.base= _temp1740; _temp1741.last_plus_one=
_temp1740 + 6; _temp1741;})); struct Cyc_PP_Doc* _temp1728= Cyc_PP_text(*(*
_temp1443->name).f2); struct Cyc_PP_Doc* _temp1729= Cyc_PP_text(({ unsigned char*
_temp1738=( unsigned char*)" < "; struct _tagged_string _temp1739; _temp1739.curr=
_temp1738; _temp1739.base= _temp1738; _temp1739.last_plus_one= _temp1738 + 4;
_temp1739;})); struct Cyc_PP_Doc* _temp1730= Cyc_Absynpp_exp2doc( _temp1441);
struct Cyc_PP_Doc* _temp1731= Cyc_PP_text(({ unsigned char* _temp1736=(
unsigned char*)" : "; struct _tagged_string _temp1737; _temp1737.curr= _temp1736;
_temp1737.base= _temp1736; _temp1737.last_plus_one= _temp1736 + 4; _temp1737;}));
struct Cyc_PP_Doc* _temp1732= Cyc_Absynpp_exp2doc( _temp1439); struct Cyc_PP_Doc*
_temp1733= Cyc_PP_text(({ unsigned char* _temp1734=( unsigned char*)"}"; struct
_tagged_string _temp1735; _temp1735.curr= _temp1734; _temp1735.base= _temp1734;
_temp1735.last_plus_one= _temp1734 + 2; _temp1735;})); struct Cyc_PP_Doc*
_temp1725[ 7u]={ _temp1727, _temp1728, _temp1729, _temp1730, _temp1731,
_temp1732, _temp1733}; struct _tagged_ptr0 _temp1726={ _temp1725, _temp1725,
_temp1725 + 7u}; _temp1724( _temp1726);}); goto _LL1282; _LL1336: s=({ struct
Cyc_PP_Doc*(* _temp1742)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1745= Cyc_Absynpp_qvar2doc( _temp1451); struct Cyc_PP_Doc* _temp1746= Cyc_PP_group(({
unsigned char* _temp1747=( unsigned char*)"{"; struct _tagged_string _temp1748;
_temp1748.curr= _temp1747; _temp1748.base= _temp1747; _temp1748.last_plus_one=
_temp1747 + 2; _temp1748;}),({ unsigned char* _temp1749=( unsigned char*)"}";
struct _tagged_string _temp1750; _temp1750.curr= _temp1749; _temp1750.base=
_temp1749; _temp1750.last_plus_one= _temp1749 + 2; _temp1750;}),({ unsigned char*
_temp1751=( unsigned char*)","; struct _tagged_string _temp1752; _temp1752.curr=
_temp1751; _temp1752.base= _temp1751; _temp1752.last_plus_one= _temp1751 + 2;
_temp1752;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1447)); struct Cyc_PP_Doc* _temp1743[ 2u]={ _temp1745, _temp1746}; struct
_tagged_ptr0 _temp1744={ _temp1743, _temp1743, _temp1743 + 2u}; _temp1742(
_temp1744);}); goto _LL1282; _LL1338: s= Cyc_PP_group(({ unsigned char*
_temp1753=( unsigned char*)"{"; struct _tagged_string _temp1754; _temp1754.curr=
_temp1753; _temp1754.base= _temp1753; _temp1754.last_plus_one= _temp1753 + 2;
_temp1754;}),({ unsigned char* _temp1755=( unsigned char*)"}"; struct
_tagged_string _temp1756; _temp1756.curr= _temp1755; _temp1756.base= _temp1755;
_temp1756.last_plus_one= _temp1755 + 2; _temp1756;}),({ unsigned char* _temp1757=(
unsigned char*)","; struct _tagged_string _temp1758; _temp1758.curr= _temp1757;
_temp1758.base= _temp1757; _temp1758.last_plus_one= _temp1757 + 2; _temp1758;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_de2doc, _temp1453)); goto _LL1282; _LL1340: if(
_temp1461 == 0){ s= Cyc_Absynpp_qvar2doc( _temp1457->name);} else{ s=({ struct
Cyc_PP_Doc*(* _temp1759)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1762= Cyc_Absynpp_qvar2doc( _temp1457->name); struct Cyc_PP_Doc* _temp1763=
Cyc_PP_egroup(({ unsigned char* _temp1764=( unsigned char*)"("; struct
_tagged_string _temp1765; _temp1765.curr= _temp1764; _temp1765.base= _temp1764;
_temp1765.last_plus_one= _temp1764 + 2; _temp1765;}),({ unsigned char* _temp1766=(
unsigned char*)")"; struct _tagged_string _temp1767; _temp1767.curr= _temp1766;
_temp1767.base= _temp1766; _temp1767.last_plus_one= _temp1766 + 2; _temp1767;}),({
unsigned char* _temp1768=( unsigned char*)","; struct _tagged_string _temp1769;
_temp1769.curr= _temp1768; _temp1769.base= _temp1768; _temp1769.last_plus_one=
_temp1768 + 2; _temp1769;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_exp2doc,
_temp1461)); struct Cyc_PP_Doc* _temp1760[ 2u]={ _temp1762, _temp1763}; struct
_tagged_ptr0 _temp1761={ _temp1760, _temp1760, _temp1760 + 2u}; _temp1759(
_temp1761);});} goto _LL1282; _LL1342: s= Cyc_Absynpp_qvar2doc( _temp1471); goto
_LL1282; _LL1344: if( _temp1475 == 0){ s=({ struct Cyc_PP_Doc*(* _temp1770)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1773= Cyc_PP_text(({
unsigned char* _temp1778=( unsigned char*)"malloc(sizeof("; struct
_tagged_string _temp1779; _temp1779.curr= _temp1778; _temp1779.base= _temp1778;
_temp1779.last_plus_one= _temp1778 + 15; _temp1779;})); struct Cyc_PP_Doc*
_temp1774= Cyc_Absynpp_typ2doc( _temp1473); struct Cyc_PP_Doc* _temp1775= Cyc_PP_text(({
unsigned char* _temp1776=( unsigned char*)"))"; struct _tagged_string _temp1777;
_temp1777.curr= _temp1776; _temp1777.base= _temp1776; _temp1777.last_plus_one=
_temp1776 + 3; _temp1777;})); struct Cyc_PP_Doc* _temp1771[ 3u]={ _temp1773,
_temp1774, _temp1775}; struct _tagged_ptr0 _temp1772={ _temp1771, _temp1771,
_temp1771 + 3u}; _temp1770( _temp1772);});} else{ s=({ struct Cyc_PP_Doc*(*
_temp1780)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1783= Cyc_PP_text(({
unsigned char* _temp1795=( unsigned char*)"rmalloc("; struct _tagged_string
_temp1796; _temp1796.curr= _temp1795; _temp1796.base= _temp1795; _temp1796.last_plus_one=
_temp1795 + 9; _temp1796;})); struct Cyc_PP_Doc* _temp1784= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp1475)); struct Cyc_PP_Doc* _temp1785=
Cyc_PP_text(({ unsigned char* _temp1793=( unsigned char*)","; struct
_tagged_string _temp1794; _temp1794.curr= _temp1793; _temp1794.base= _temp1793;
_temp1794.last_plus_one= _temp1793 + 2; _temp1794;})); struct Cyc_PP_Doc*
_temp1786= Cyc_PP_text(({ unsigned char* _temp1791=( unsigned char*)"sizeof(";
struct _tagged_string _temp1792; _temp1792.curr= _temp1791; _temp1792.base=
_temp1791; _temp1792.last_plus_one= _temp1791 + 8; _temp1792;})); struct Cyc_PP_Doc*
_temp1787= Cyc_Absynpp_typ2doc( _temp1473); struct Cyc_PP_Doc* _temp1788= Cyc_PP_text(({
unsigned char* _temp1789=( unsigned char*)"))"; struct _tagged_string _temp1790;
_temp1790.curr= _temp1789; _temp1790.base= _temp1789; _temp1790.last_plus_one=
_temp1789 + 3; _temp1790;})); struct Cyc_PP_Doc* _temp1781[ 6u]={ _temp1783,
_temp1784, _temp1785, _temp1786, _temp1787, _temp1788}; struct _tagged_ptr0
_temp1782={ _temp1781, _temp1781, _temp1781 + 6u}; _temp1780( _temp1782);});}
goto _LL1282; _LL1346: s= Cyc_PP_group(({ unsigned char* _temp1797=(
unsigned char*)"{"; struct _tagged_string _temp1798; _temp1798.curr= _temp1797;
_temp1798.base= _temp1797; _temp1798.last_plus_one= _temp1797 + 2; _temp1798;}),({
unsigned char* _temp1799=( unsigned char*)"}"; struct _tagged_string _temp1800;
_temp1800.curr= _temp1799; _temp1800.base= _temp1799; _temp1800.last_plus_one=
_temp1799 + 2; _temp1800;}),({ unsigned char* _temp1801=( unsigned char*)",";
struct _tagged_string _temp1802; _temp1802.curr= _temp1801; _temp1802.base=
_temp1801; _temp1802.last_plus_one= _temp1801 + 2; _temp1802;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp1477)); goto _LL1282; _LL1348: s=({ struct Cyc_PP_Doc*(*
_temp1803)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1806= Cyc_PP_text(({
unsigned char* _temp1811=( unsigned char*)"({ "; struct _tagged_string _temp1812;
_temp1812.curr= _temp1811; _temp1812.base= _temp1811; _temp1812.last_plus_one=
_temp1811 + 4; _temp1812;})); struct Cyc_PP_Doc* _temp1807= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1481)); struct Cyc_PP_Doc* _temp1808= Cyc_PP_text(({ unsigned char*
_temp1809=( unsigned char*)" })"; struct _tagged_string _temp1810; _temp1810.curr=
_temp1809; _temp1810.base= _temp1809; _temp1810.last_plus_one= _temp1809 + 4;
_temp1810;})); struct Cyc_PP_Doc* _temp1804[ 3u]={ _temp1806, _temp1807,
_temp1808}; struct _tagged_ptr0 _temp1805={ _temp1804, _temp1804, _temp1804 + 3u};
_temp1803( _temp1805);}); goto _LL1282; _LL1350: s=({ struct Cyc_PP_Doc*(*
_temp1813)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1816= Cyc_PP_text(({
unsigned char* _temp1824=( unsigned char*)"codegen("; struct _tagged_string
_temp1825; _temp1825.curr= _temp1824; _temp1825.base= _temp1824; _temp1825.last_plus_one=
_temp1824 + 9; _temp1825;})); struct Cyc_PP_Doc* _temp1817= Cyc_PP_nest( 2, Cyc_Absynpp_decl2doc(({
struct Cyc_Absyn_Decl* _temp1821=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof(
struct Cyc_Absyn_Decl)); _temp1821->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp1822=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1822[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp1823; _temp1823.tag= Cyc_Absyn_Fn_d;
_temp1823.f1= _temp1483; _temp1823;}); _temp1822;})); _temp1821->loc= e->loc;
_temp1821;}))); struct Cyc_PP_Doc* _temp1818= Cyc_PP_text(({ unsigned char*
_temp1819=( unsigned char*)")"; struct _tagged_string _temp1820; _temp1820.curr=
_temp1819; _temp1820.base= _temp1819; _temp1820.last_plus_one= _temp1819 + 2;
_temp1820;})); struct Cyc_PP_Doc* _temp1814[ 3u]={ _temp1816, _temp1817,
_temp1818}; struct _tagged_ptr0 _temp1815={ _temp1814, _temp1814, _temp1814 + 3u};
_temp1813( _temp1815);}); goto _LL1282; _LL1352: s=({ struct Cyc_PP_Doc*(*
_temp1826)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1829= Cyc_PP_text(({
unsigned char* _temp1834=( unsigned char*)"fill("; struct _tagged_string
_temp1835; _temp1835.curr= _temp1834; _temp1835.base= _temp1834; _temp1835.last_plus_one=
_temp1834 + 6; _temp1835;})); struct Cyc_PP_Doc* _temp1830= Cyc_PP_nest( 2, Cyc_Absynpp_exp2doc(
_temp1485)); struct Cyc_PP_Doc* _temp1831= Cyc_PP_text(({ unsigned char*
_temp1832=( unsigned char*)")"; struct _tagged_string _temp1833; _temp1833.curr=
_temp1832; _temp1833.base= _temp1832; _temp1833.last_plus_one= _temp1832 + 2;
_temp1833;})); struct Cyc_PP_Doc* _temp1827[ 3u]={ _temp1829, _temp1830,
_temp1831}; struct _tagged_ptr0 _temp1828={ _temp1827, _temp1827, _temp1827 + 3u};
_temp1826( _temp1828);}); goto _LL1282; _LL1282:;} if( inprec >= myprec){ s=({
struct Cyc_PP_Doc*(* _temp1836)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1839= Cyc_PP_text(({ unsigned char* _temp1844=( unsigned char*)"("; struct
_tagged_string _temp1845; _temp1845.curr= _temp1844; _temp1845.base= _temp1844;
_temp1845.last_plus_one= _temp1844 + 2; _temp1845;})); struct Cyc_PP_Doc*
_temp1840= s; struct Cyc_PP_Doc* _temp1841= Cyc_PP_text(({ unsigned char*
_temp1842=( unsigned char*)")"; struct _tagged_string _temp1843; _temp1843.curr=
_temp1842; _temp1843.base= _temp1842; _temp1843.last_plus_one= _temp1842 + 2;
_temp1843;})); struct Cyc_PP_Doc* _temp1837[ 3u]={ _temp1839, _temp1840,
_temp1841}; struct _tagged_ptr0 _temp1838={ _temp1837, _temp1837, _temp1837 + 3u};
_temp1836( _temp1838);});} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc(
void* d){ void* _temp1846= d; struct Cyc_Absyn_Exp* _temp1852; struct
_tagged_string* _temp1854; _LL1848: if(*(( int*) _temp1846) == Cyc_Absyn_ArrayElement){
_LL1853: _temp1852=(( struct Cyc_Absyn_ArrayElement_struct*) _temp1846)->f1;
goto _LL1849;} else{ goto _LL1850;} _LL1850: if(*(( int*) _temp1846) == Cyc_Absyn_FieldName){
_LL1855: _temp1854=(( struct Cyc_Absyn_FieldName_struct*) _temp1846)->f1; goto
_LL1851;} else{ goto _LL1847;} _LL1849: return({ struct Cyc_PP_Doc*(* _temp1856)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1859= Cyc_PP_text(({
unsigned char* _temp1864=( unsigned char*)".["; struct _tagged_string _temp1865;
_temp1865.curr= _temp1864; _temp1865.base= _temp1864; _temp1865.last_plus_one=
_temp1864 + 3; _temp1865;})); struct Cyc_PP_Doc* _temp1860= Cyc_Absynpp_exp2doc(
_temp1852); struct Cyc_PP_Doc* _temp1861= Cyc_PP_text(({ unsigned char*
_temp1862=( unsigned char*)"]"; struct _tagged_string _temp1863; _temp1863.curr=
_temp1862; _temp1863.base= _temp1862; _temp1863.last_plus_one= _temp1862 + 2;
_temp1863;})); struct Cyc_PP_Doc* _temp1857[ 3u]={ _temp1859, _temp1860,
_temp1861}; struct _tagged_ptr0 _temp1858={ _temp1857, _temp1857, _temp1857 + 3u};
_temp1856( _temp1858);}); _LL1851: return({ struct Cyc_PP_Doc*(* _temp1866)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1869= Cyc_PP_text(({
unsigned char* _temp1871=( unsigned char*)"."; struct _tagged_string _temp1872;
_temp1872.curr= _temp1871; _temp1872.base= _temp1871; _temp1872.last_plus_one=
_temp1871 + 2; _temp1872;})); struct Cyc_PP_Doc* _temp1870= Cyc_PP_textptr(
_temp1854); struct Cyc_PP_Doc* _temp1867[ 2u]={ _temp1869, _temp1870}; struct
_tagged_ptr0 _temp1868={ _temp1867, _temp1867, _temp1867 + 2u}; _temp1866(
_temp1868);}); _LL1847:;} struct Cyc_PP_Doc* Cyc_Absynpp_de2doc( struct _tuple6*
de){ if((* de).f1 == 0){ return Cyc_Absynpp_exp2doc((* de).f2);} else{ return({
struct Cyc_PP_Doc*(* _temp1873)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1876= Cyc_PP_egroup(({ unsigned char* _temp1878=( unsigned char*)""; struct
_tagged_string _temp1879; _temp1879.curr= _temp1878; _temp1879.base= _temp1878;
_temp1879.last_plus_one= _temp1878 + 1; _temp1879;}),({ unsigned char* _temp1880=(
unsigned char*)"="; struct _tagged_string _temp1881; _temp1881.curr= _temp1880;
_temp1881.base= _temp1880; _temp1881.last_plus_one= _temp1880 + 2; _temp1881;}),({
unsigned char* _temp1882=( unsigned char*)"="; struct _tagged_string _temp1883;
_temp1883.curr= _temp1882; _temp1883.base= _temp1882; _temp1883.last_plus_one=
_temp1882 + 2; _temp1883;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
de).f1)); struct Cyc_PP_Doc* _temp1877= Cyc_Absynpp_exp2doc((* de).f2); struct
Cyc_PP_Doc* _temp1874[ 2u]={ _temp1876, _temp1877}; struct _tagged_ptr0
_temp1875={ _temp1874, _temp1874, _temp1874 + 2u}; _temp1873( _temp1875);});}}
struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec( int inprec, struct Cyc_List_List*
es){ return Cyc_PP_group(({ unsigned char* _temp1884=( unsigned char*)""; struct
_tagged_string _temp1885; _temp1885.curr= _temp1884; _temp1885.base= _temp1884;
_temp1885.last_plus_one= _temp1884 + 1; _temp1885;}),({ unsigned char* _temp1886=(
unsigned char*)""; struct _tagged_string _temp1887; _temp1887.curr= _temp1886;
_temp1887.base= _temp1886; _temp1887.last_plus_one= _temp1886 + 1; _temp1887;}),({
unsigned char* _temp1888=( unsigned char*)","; struct _tagged_string _temp1889;
_temp1889.curr= _temp1888; _temp1889.base= _temp1888; _temp1889.last_plus_one=
_temp1888 + 2; _temp1889;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Absynpp_exp2doc_prec, inprec, es));} struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc(
void* c){ void* _temp1890= c; unsigned char _temp1908; void* _temp1910; short
_temp1912; void* _temp1914; int _temp1916; void* _temp1918; int _temp1920; void*
_temp1922; long long _temp1924; void* _temp1926; struct _tagged_string _temp1928;
struct _tagged_string _temp1930; _LL1892: if(( unsigned int) _temp1890 > 1u?*((
int*) _temp1890) == Cyc_Absyn_Char_c: 0){ _LL1911: _temp1910=( void*)(( struct
Cyc_Absyn_Char_c_struct*) _temp1890)->f1; goto _LL1909; _LL1909: _temp1908=((
struct Cyc_Absyn_Char_c_struct*) _temp1890)->f2; goto _LL1893;} else{ goto
_LL1894;} _LL1894: if(( unsigned int) _temp1890 > 1u?*(( int*) _temp1890) == Cyc_Absyn_Short_c:
0){ _LL1915: _temp1914=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp1890)->f1;
goto _LL1913; _LL1913: _temp1912=(( struct Cyc_Absyn_Short_c_struct*) _temp1890)->f2;
goto _LL1895;} else{ goto _LL1896;} _LL1896: if(( unsigned int) _temp1890 > 1u?*((
int*) _temp1890) == Cyc_Absyn_Int_c: 0){ _LL1919: _temp1918=( void*)(( struct
Cyc_Absyn_Int_c_struct*) _temp1890)->f1; if( _temp1918 ==( void*) Cyc_Absyn_Signed){
goto _LL1917;} else{ goto _LL1898;} _LL1917: _temp1916=(( struct Cyc_Absyn_Int_c_struct*)
_temp1890)->f2; goto _LL1897;} else{ goto _LL1898;} _LL1898: if(( unsigned int)
_temp1890 > 1u?*(( int*) _temp1890) == Cyc_Absyn_Int_c: 0){ _LL1923: _temp1922=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1890)->f1; if( _temp1922 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL1921;} else{ goto _LL1900;} _LL1921: _temp1920=((
struct Cyc_Absyn_Int_c_struct*) _temp1890)->f2; goto _LL1899;} else{ goto
_LL1900;} _LL1900: if(( unsigned int) _temp1890 > 1u?*(( int*) _temp1890) == Cyc_Absyn_LongLong_c:
0){ _LL1927: _temp1926=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp1890)->f1;
goto _LL1925; _LL1925: _temp1924=(( struct Cyc_Absyn_LongLong_c_struct*)
_temp1890)->f2; goto _LL1901;} else{ goto _LL1902;} _LL1902: if(( unsigned int)
_temp1890 > 1u?*(( int*) _temp1890) == Cyc_Absyn_Float_c: 0){ _LL1929: _temp1928=((
struct Cyc_Absyn_Float_c_struct*) _temp1890)->f1; goto _LL1903;} else{ goto
_LL1904;} _LL1904: if( _temp1890 ==( void*) Cyc_Absyn_Null_c){ goto _LL1905;}
else{ goto _LL1906;} _LL1906: if(( unsigned int) _temp1890 > 1u?*(( int*)
_temp1890) == Cyc_Absyn_String_c: 0){ _LL1931: _temp1930=(( struct Cyc_Absyn_String_c_struct*)
_temp1890)->f1; goto _LL1907;} else{ goto _LL1891;} _LL1893: return Cyc_PP_text((
struct _tagged_string)({ struct _tagged_string _temp1932= Cyc_Absynpp_char_escape(
_temp1908); xprintf("'%.*s'", _temp1932.last_plus_one - _temp1932.curr,
_temp1932.curr);})); _LL1895: return Cyc_PP_text(( struct _tagged_string)
xprintf("%d",( int) _temp1912)); _LL1897: return Cyc_PP_text(( struct
_tagged_string) Cyc_Core_string_of_int( _temp1916)); _LL1899: return Cyc_PP_text((
struct _tagged_string) xprintf("%u",( unsigned int) _temp1920)); _LL1901: return
Cyc_PP_text(({ unsigned char* _temp1933=( unsigned char*)"<<FIX LONG LONG CONSTANT>>";
struct _tagged_string _temp1934; _temp1934.curr= _temp1933; _temp1934.base=
_temp1933; _temp1934.last_plus_one= _temp1933 + 27; _temp1934;})); _LL1903:
return Cyc_PP_text( _temp1928); _LL1905: return Cyc_PP_text(({ unsigned char*
_temp1935=( unsigned char*)"null"; struct _tagged_string _temp1936; _temp1936.curr=
_temp1935; _temp1936.base= _temp1935; _temp1936.last_plus_one= _temp1935 + 5;
_temp1936;})); _LL1907: return({ struct Cyc_PP_Doc*(* _temp1937)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1940= Cyc_PP_text(({
unsigned char* _temp1945=( unsigned char*)"\""; struct _tagged_string _temp1946;
_temp1946.curr= _temp1945; _temp1946.base= _temp1945; _temp1946.last_plus_one=
_temp1945 + 2; _temp1946;})); struct Cyc_PP_Doc* _temp1941= Cyc_PP_text( Cyc_Absynpp_string_escape(
_temp1930)); struct Cyc_PP_Doc* _temp1942= Cyc_PP_text(({ unsigned char*
_temp1943=( unsigned char*)"\""; struct _tagged_string _temp1944; _temp1944.curr=
_temp1943; _temp1944.base= _temp1943; _temp1944.last_plus_one= _temp1943 + 2;
_temp1944;})); struct Cyc_PP_Doc* _temp1938[ 3u]={ _temp1940, _temp1941,
_temp1942}; struct _tagged_ptr0 _temp1939={ _temp1938, _temp1938, _temp1938 + 3u};
_temp1937( _temp1939);}); _LL1891:;} struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc(
int inprec, void* p, struct Cyc_List_List* es){ struct Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc(
p); if( p ==( void*) Cyc_Absyn_Size){ if( es == 0? 1:(( struct Cyc_List_List*)
_check_null( es))->tl != 0){( void) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp1947=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1947[ 0]=({ struct Cyc_Core_Failure_struct _temp1948; _temp1948.tag= Cyc_Core_Failure;
_temp1948.f1=( struct _tagged_string)({ struct _tagged_string _temp1949= Cyc_PP_string_of_doc(
ps, 72); xprintf("Absynpp::primapp2doc Size: %.*s with bad args", _temp1949.last_plus_one
- _temp1949.curr, _temp1949.curr);}); _temp1948;}); _temp1947;}));}{ struct Cyc_PP_Doc*
_temp1950= Cyc_Absynpp_exp2doc_prec( inprec,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd); goto _LL1951; _LL1951: return({ struct
Cyc_PP_Doc*(* _temp1952)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1955= _temp1950; struct Cyc_PP_Doc* _temp1956= Cyc_PP_text(({ unsigned char*
_temp1957=( unsigned char*)".size"; struct _tagged_string _temp1958; _temp1958.curr=
_temp1957; _temp1958.base= _temp1957; _temp1958.last_plus_one= _temp1957 + 6;
_temp1958;})); struct Cyc_PP_Doc* _temp1953[ 2u]={ _temp1955, _temp1956}; struct
_tagged_ptr0 _temp1954={ _temp1953, _temp1953, _temp1953 + 2u}; _temp1952(
_temp1954);});}} else{ if( ! Cyc_Absyn_is_format_prim( p)){ struct Cyc_List_List*
ds=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*),
int env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec,
inprec, es); if( ds == 0){( void) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp1959=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1959[ 0]=({ struct Cyc_Core_Failure_struct _temp1960; _temp1960.tag= Cyc_Core_Failure;
_temp1960.f1=( struct _tagged_string)({ struct _tagged_string _temp1961= Cyc_PP_string_of_doc(
ps, 72); xprintf("Absynpp::primapp2doc: %.*s with no args", _temp1961.last_plus_one
- _temp1961.curr, _temp1961.curr);}); _temp1960;}); _temp1959;}));} else{ if(((
struct Cyc_List_List*) _check_null( ds))->tl == 0){ return({ struct Cyc_PP_Doc*(*
_temp1962)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1965= ps;
struct Cyc_PP_Doc* _temp1966= Cyc_PP_text(({ unsigned char* _temp1968=(
unsigned char*)" "; struct _tagged_string _temp1969; _temp1969.curr= _temp1968;
_temp1969.base= _temp1968; _temp1969.last_plus_one= _temp1968 + 2; _temp1969;}));
struct Cyc_PP_Doc* _temp1967=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*)
_check_null( ds))->hd; struct Cyc_PP_Doc* _temp1963[ 3u]={ _temp1965, _temp1966,
_temp1967}; struct _tagged_ptr0 _temp1964={ _temp1963, _temp1963, _temp1963 + 3u};
_temp1962( _temp1964);});} else{ if((( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( ds))->tl))->tl != 0){( void) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp1970=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1970[ 0]=({ struct Cyc_Core_Failure_struct
_temp1971; _temp1971.tag= Cyc_Core_Failure; _temp1971.f1=( struct _tagged_string)({
struct _tagged_string _temp1972= Cyc_PP_string_of_doc( ps, 72); xprintf("Absynpp::primapp2doc: %.*s with more than 2 args",
_temp1972.last_plus_one - _temp1972.curr, _temp1972.curr);}); _temp1971;});
_temp1970;}));} else{ return({ struct Cyc_PP_Doc*(* _temp1973)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1976=( struct Cyc_PP_Doc*)((
struct Cyc_List_List*) _check_null( ds))->hd; struct Cyc_PP_Doc* _temp1977= Cyc_PP_text(({
unsigned char* _temp1983=( unsigned char*)" "; struct _tagged_string _temp1984;
_temp1984.curr= _temp1983; _temp1984.base= _temp1983; _temp1984.last_plus_one=
_temp1983 + 2; _temp1984;})); struct Cyc_PP_Doc* _temp1978= ps; struct Cyc_PP_Doc*
_temp1979= Cyc_PP_text(({ unsigned char* _temp1981=( unsigned char*)" "; struct
_tagged_string _temp1982; _temp1982.curr= _temp1981; _temp1982.base= _temp1981;
_temp1982.last_plus_one= _temp1981 + 2; _temp1982;})); struct Cyc_PP_Doc*
_temp1980=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null((( struct
Cyc_List_List*) _check_null( ds))->tl))->hd; struct Cyc_PP_Doc* _temp1974[ 5u]={
_temp1976, _temp1977, _temp1978, _temp1979, _temp1980}; struct _tagged_ptr0
_temp1975={ _temp1974, _temp1974, _temp1974 + 5u}; _temp1973( _temp1975);});}}}}
else{ return({ struct Cyc_PP_Doc*(* _temp1985)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1988= ps; struct Cyc_PP_Doc* _temp1989= Cyc_PP_text(({
unsigned char* _temp1994=( unsigned char*)"("; struct _tagged_string _temp1995;
_temp1995.curr= _temp1994; _temp1995.base= _temp1994; _temp1995.last_plus_one=
_temp1994 + 2; _temp1995;})); struct Cyc_PP_Doc* _temp1990= Cyc_Absynpp_exps2doc_prec(
20, es); struct Cyc_PP_Doc* _temp1991= Cyc_PP_text(({ unsigned char* _temp1992=(
unsigned char*)")"; struct _tagged_string _temp1993; _temp1993.curr= _temp1992;
_temp1993.base= _temp1992; _temp1993.last_plus_one= _temp1992 + 2; _temp1993;}));
struct Cyc_PP_Doc* _temp1986[ 4u]={ _temp1988, _temp1989, _temp1990, _temp1991};
struct _tagged_ptr0 _temp1987={ _temp1986, _temp1986, _temp1986 + 4u}; _temp1985(
_temp1987);});}}} struct _tagged_string Cyc_Absynpp_prim2str( void* p){ void*
_temp1996= p; _LL1998: if( _temp1996 ==( void*) Cyc_Absyn_Plus){ goto _LL1999;}
else{ goto _LL2000;} _LL2000: if( _temp1996 ==( void*) Cyc_Absyn_Times){ goto
_LL2001;} else{ goto _LL2002;} _LL2002: if( _temp1996 ==( void*) Cyc_Absyn_Minus){
goto _LL2003;} else{ goto _LL2004;} _LL2004: if( _temp1996 ==( void*) Cyc_Absyn_Div){
goto _LL2005;} else{ goto _LL2006;} _LL2006: if( _temp1996 ==( void*) Cyc_Absyn_Mod){
goto _LL2007;} else{ goto _LL2008;} _LL2008: if( _temp1996 ==( void*) Cyc_Absyn_Eq){
goto _LL2009;} else{ goto _LL2010;} _LL2010: if( _temp1996 ==( void*) Cyc_Absyn_Neq){
goto _LL2011;} else{ goto _LL2012;} _LL2012: if( _temp1996 ==( void*) Cyc_Absyn_Gt){
goto _LL2013;} else{ goto _LL2014;} _LL2014: if( _temp1996 ==( void*) Cyc_Absyn_Lt){
goto _LL2015;} else{ goto _LL2016;} _LL2016: if( _temp1996 ==( void*) Cyc_Absyn_Gte){
goto _LL2017;} else{ goto _LL2018;} _LL2018: if( _temp1996 ==( void*) Cyc_Absyn_Lte){
goto _LL2019;} else{ goto _LL2020;} _LL2020: if( _temp1996 ==( void*) Cyc_Absyn_Not){
goto _LL2021;} else{ goto _LL2022;} _LL2022: if( _temp1996 ==( void*) Cyc_Absyn_Bitnot){
goto _LL2023;} else{ goto _LL2024;} _LL2024: if( _temp1996 ==( void*) Cyc_Absyn_Bitand){
goto _LL2025;} else{ goto _LL2026;} _LL2026: if( _temp1996 ==( void*) Cyc_Absyn_Bitor){
goto _LL2027;} else{ goto _LL2028;} _LL2028: if( _temp1996 ==( void*) Cyc_Absyn_Bitxor){
goto _LL2029;} else{ goto _LL2030;} _LL2030: if( _temp1996 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL2031;} else{ goto _LL2032;} _LL2032: if( _temp1996 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL2033;} else{ goto _LL2034;} _LL2034: if( _temp1996 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL2035;} else{ goto _LL2036;} _LL2036: if( _temp1996 ==( void*) Cyc_Absyn_Size){
goto _LL2037;} else{ goto _LL2038;} _LL2038: if( _temp1996 ==( void*) Cyc_Absyn_Printf){
goto _LL2039;} else{ goto _LL2040;} _LL2040: if( _temp1996 ==( void*) Cyc_Absyn_Fprintf){
goto _LL2041;} else{ goto _LL2042;} _LL2042: if( _temp1996 ==( void*) Cyc_Absyn_Xprintf){
goto _LL2043;} else{ goto _LL2044;} _LL2044: if( _temp1996 ==( void*) Cyc_Absyn_Scanf){
goto _LL2045;} else{ goto _LL2046;} _LL2046: if( _temp1996 ==( void*) Cyc_Absyn_Fscanf){
goto _LL2047;} else{ goto _LL2048;} _LL2048: if( _temp1996 ==( void*) Cyc_Absyn_Sscanf){
goto _LL2049;} else{ goto _LL1997;} _LL1999: return({ unsigned char* _temp2050=(
unsigned char*)"+"; struct _tagged_string _temp2051; _temp2051.curr= _temp2050;
_temp2051.base= _temp2050; _temp2051.last_plus_one= _temp2050 + 2; _temp2051;});
_LL2001: return({ unsigned char* _temp2052=( unsigned char*)"*"; struct
_tagged_string _temp2053; _temp2053.curr= _temp2052; _temp2053.base= _temp2052;
_temp2053.last_plus_one= _temp2052 + 2; _temp2053;}); _LL2003: return({
unsigned char* _temp2054=( unsigned char*)"-"; struct _tagged_string _temp2055;
_temp2055.curr= _temp2054; _temp2055.base= _temp2054; _temp2055.last_plus_one=
_temp2054 + 2; _temp2055;}); _LL2005: return({ unsigned char* _temp2056=(
unsigned char*)"/"; struct _tagged_string _temp2057; _temp2057.curr= _temp2056;
_temp2057.base= _temp2056; _temp2057.last_plus_one= _temp2056 + 2; _temp2057;});
_LL2007: return({ unsigned char* _temp2058=( unsigned char*)"%"; struct
_tagged_string _temp2059; _temp2059.curr= _temp2058; _temp2059.base= _temp2058;
_temp2059.last_plus_one= _temp2058 + 2; _temp2059;}); _LL2009: return({
unsigned char* _temp2060=( unsigned char*)"=="; struct _tagged_string _temp2061;
_temp2061.curr= _temp2060; _temp2061.base= _temp2060; _temp2061.last_plus_one=
_temp2060 + 3; _temp2061;}); _LL2011: return({ unsigned char* _temp2062=(
unsigned char*)"!="; struct _tagged_string _temp2063; _temp2063.curr= _temp2062;
_temp2063.base= _temp2062; _temp2063.last_plus_one= _temp2062 + 3; _temp2063;});
_LL2013: return({ unsigned char* _temp2064=( unsigned char*)">"; struct
_tagged_string _temp2065; _temp2065.curr= _temp2064; _temp2065.base= _temp2064;
_temp2065.last_plus_one= _temp2064 + 2; _temp2065;}); _LL2015: return({
unsigned char* _temp2066=( unsigned char*)"<"; struct _tagged_string _temp2067;
_temp2067.curr= _temp2066; _temp2067.base= _temp2066; _temp2067.last_plus_one=
_temp2066 + 2; _temp2067;}); _LL2017: return({ unsigned char* _temp2068=(
unsigned char*)">="; struct _tagged_string _temp2069; _temp2069.curr= _temp2068;
_temp2069.base= _temp2068; _temp2069.last_plus_one= _temp2068 + 3; _temp2069;});
_LL2019: return({ unsigned char* _temp2070=( unsigned char*)"<="; struct
_tagged_string _temp2071; _temp2071.curr= _temp2070; _temp2071.base= _temp2070;
_temp2071.last_plus_one= _temp2070 + 3; _temp2071;}); _LL2021: return({
unsigned char* _temp2072=( unsigned char*)"!"; struct _tagged_string _temp2073;
_temp2073.curr= _temp2072; _temp2073.base= _temp2072; _temp2073.last_plus_one=
_temp2072 + 2; _temp2073;}); _LL2023: return({ unsigned char* _temp2074=(
unsigned char*)"~"; struct _tagged_string _temp2075; _temp2075.curr= _temp2074;
_temp2075.base= _temp2074; _temp2075.last_plus_one= _temp2074 + 2; _temp2075;});
_LL2025: return({ unsigned char* _temp2076=( unsigned char*)"&"; struct
_tagged_string _temp2077; _temp2077.curr= _temp2076; _temp2077.base= _temp2076;
_temp2077.last_plus_one= _temp2076 + 2; _temp2077;}); _LL2027: return({
unsigned char* _temp2078=( unsigned char*)"|"; struct _tagged_string _temp2079;
_temp2079.curr= _temp2078; _temp2079.base= _temp2078; _temp2079.last_plus_one=
_temp2078 + 2; _temp2079;}); _LL2029: return({ unsigned char* _temp2080=(
unsigned char*)"^"; struct _tagged_string _temp2081; _temp2081.curr= _temp2080;
_temp2081.base= _temp2080; _temp2081.last_plus_one= _temp2080 + 2; _temp2081;});
_LL2031: return({ unsigned char* _temp2082=( unsigned char*)"<<"; struct
_tagged_string _temp2083; _temp2083.curr= _temp2082; _temp2083.base= _temp2082;
_temp2083.last_plus_one= _temp2082 + 3; _temp2083;}); _LL2033: return({
unsigned char* _temp2084=( unsigned char*)">>"; struct _tagged_string _temp2085;
_temp2085.curr= _temp2084; _temp2085.base= _temp2084; _temp2085.last_plus_one=
_temp2084 + 3; _temp2085;}); _LL2035: return({ unsigned char* _temp2086=(
unsigned char*)">>>"; struct _tagged_string _temp2087; _temp2087.curr= _temp2086;
_temp2087.base= _temp2086; _temp2087.last_plus_one= _temp2086 + 4; _temp2087;});
_LL2037: return({ unsigned char* _temp2088=( unsigned char*)"size"; struct
_tagged_string _temp2089; _temp2089.curr= _temp2088; _temp2089.base= _temp2088;
_temp2089.last_plus_one= _temp2088 + 5; _temp2089;}); _LL2039: return({
unsigned char* _temp2090=( unsigned char*)"printf"; struct _tagged_string
_temp2091; _temp2091.curr= _temp2090; _temp2091.base= _temp2090; _temp2091.last_plus_one=
_temp2090 + 7; _temp2091;}); _LL2041: return({ unsigned char* _temp2092=(
unsigned char*)"fprintf"; struct _tagged_string _temp2093; _temp2093.curr=
_temp2092; _temp2093.base= _temp2092; _temp2093.last_plus_one= _temp2092 + 8;
_temp2093;}); _LL2043: return({ unsigned char* _temp2094=( unsigned char*)"xprintf";
struct _tagged_string _temp2095; _temp2095.curr= _temp2094; _temp2095.base=
_temp2094; _temp2095.last_plus_one= _temp2094 + 8; _temp2095;}); _LL2045: return({
unsigned char* _temp2096=( unsigned char*)"scanf"; struct _tagged_string
_temp2097; _temp2097.curr= _temp2096; _temp2097.base= _temp2096; _temp2097.last_plus_one=
_temp2096 + 6; _temp2097;}); _LL2047: return({ unsigned char* _temp2098=(
unsigned char*)"fscanf"; struct _tagged_string _temp2099; _temp2099.curr=
_temp2098; _temp2099.base= _temp2098; _temp2099.last_plus_one= _temp2098 + 7;
_temp2099;}); _LL2049: return({ unsigned char* _temp2100=( unsigned char*)"sscanf";
struct _tagged_string _temp2101; _temp2101.curr= _temp2100; _temp2101.base=
_temp2100; _temp2101.last_plus_one= _temp2100 + 7; _temp2101;}); _LL1997:;}
struct Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void* p){ return Cyc_PP_text( Cyc_Absynpp_prim2str(
p));} struct _tuple9{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ;
struct Cyc_PP_Doc* Cyc_Absynpp_structarg2doc( struct _tuple9* t){ if((* t).f1 ==
0){ return Cyc_Absynpp_exp2doc((* t).f2);} else{ return({ struct Cyc_PP_Doc*(*
_temp2102)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2105= Cyc_PP_textptr((
struct _tagged_string*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v);
struct Cyc_PP_Doc* _temp2106= Cyc_PP_text(({ unsigned char* _temp2108=(
unsigned char*)" = "; struct _tagged_string _temp2109; _temp2109.curr= _temp2108;
_temp2109.base= _temp2108; _temp2109.last_plus_one= _temp2108 + 4; _temp2109;}));
struct Cyc_PP_Doc* _temp2107= Cyc_Absynpp_exp2doc((* t).f2); struct Cyc_PP_Doc*
_temp2103[ 3u]={ _temp2105, _temp2106, _temp2107}; struct _tagged_ptr0 _temp2104={
_temp2103, _temp2103, _temp2103 + 3u}; _temp2102( _temp2104);});}} int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt* s){ void* _temp2110=( void*) s->r; struct Cyc_Absyn_Stmt*
_temp2116; struct Cyc_Absyn_Decl* _temp2118; _LL2112: if(( unsigned int)
_temp2110 > 1u?*(( int*) _temp2110) == Cyc_Absyn_Decl_s: 0){ _LL2119: _temp2118=((
struct Cyc_Absyn_Decl_s_struct*) _temp2110)->f1; goto _LL2117; _LL2117:
_temp2116=(( struct Cyc_Absyn_Decl_s_struct*) _temp2110)->f2; goto _LL2113;}
else{ goto _LL2114;} _LL2114: goto _LL2115; _LL2113: return 1; _LL2115: return 0;
_LL2111:;} struct Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct Cyc_Absyn_Stmt* st){
struct Cyc_PP_Doc* s;{ void* _temp2120=( void*) st->r; struct Cyc_Absyn_Exp*
_temp2164; struct Cyc_Absyn_Stmt* _temp2166; struct Cyc_Absyn_Stmt* _temp2168;
struct Cyc_Absyn_Exp* _temp2170; struct Cyc_Absyn_Stmt* _temp2172; struct Cyc_Absyn_Stmt*
_temp2174; struct Cyc_Absyn_Exp* _temp2176; struct Cyc_Absyn_Stmt* _temp2178;
struct _tuple2 _temp2180; struct Cyc_Absyn_Stmt* _temp2182; struct Cyc_Absyn_Exp*
_temp2184; struct Cyc_Absyn_Stmt* _temp2186; struct Cyc_Absyn_Stmt* _temp2188;
struct Cyc_Absyn_Stmt* _temp2190; struct _tagged_string* _temp2192; struct Cyc_Absyn_Stmt*
_temp2194; struct _tuple2 _temp2196; struct Cyc_Absyn_Stmt* _temp2198; struct
Cyc_Absyn_Exp* _temp2200; struct _tuple2 _temp2202; struct Cyc_Absyn_Stmt*
_temp2204; struct Cyc_Absyn_Exp* _temp2206; struct Cyc_Absyn_Exp* _temp2208;
struct Cyc_List_List* _temp2210; struct Cyc_Absyn_Exp* _temp2212; struct Cyc_List_List*
_temp2214; struct Cyc_Absyn_Exp* _temp2216; struct Cyc_Absyn_Switch_clause**
_temp2218; struct Cyc_List_List* _temp2220; struct Cyc_Absyn_Switch_clause**
_temp2222; struct Cyc_List_List* _temp2224; struct Cyc_Absyn_Stmt* _temp2226;
struct Cyc_Absyn_Decl* _temp2228; struct Cyc_Absyn_Stmt* _temp2230; struct Cyc_Absyn_Stmt*
_temp2232; struct Cyc_Absyn_Stmt* _temp2234; struct _tagged_string* _temp2236;
struct _tuple2 _temp2238; struct Cyc_Absyn_Stmt* _temp2240; struct Cyc_Absyn_Exp*
_temp2242; struct Cyc_Absyn_Stmt* _temp2244; struct Cyc_List_List* _temp2246;
struct Cyc_Absyn_Stmt* _temp2248; struct Cyc_Absyn_Stmt* _temp2250; struct Cyc_Absyn_Vardecl*
_temp2252; struct Cyc_Absyn_Tvar* _temp2254; _LL2122: if( _temp2120 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL2123;} else{ goto _LL2124;} _LL2124: if((
unsigned int) _temp2120 > 1u?*(( int*) _temp2120) == Cyc_Absyn_Exp_s: 0){
_LL2165: _temp2164=(( struct Cyc_Absyn_Exp_s_struct*) _temp2120)->f1; goto
_LL2125;} else{ goto _LL2126;} _LL2126: if(( unsigned int) _temp2120 > 1u?*((
int*) _temp2120) == Cyc_Absyn_Seq_s: 0){ _LL2169: _temp2168=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2120)->f1; goto _LL2167; _LL2167: _temp2166=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2120)->f2; goto _LL2127;} else{ goto _LL2128;} _LL2128: if(( unsigned int)
_temp2120 > 1u?*(( int*) _temp2120) == Cyc_Absyn_Return_s: 0){ _LL2171:
_temp2170=(( struct Cyc_Absyn_Return_s_struct*) _temp2120)->f1; goto _LL2129;}
else{ goto _LL2130;} _LL2130: if(( unsigned int) _temp2120 > 1u?*(( int*)
_temp2120) == Cyc_Absyn_IfThenElse_s: 0){ _LL2177: _temp2176=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2120)->f1; goto _LL2175; _LL2175: _temp2174=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2120)->f2; goto _LL2173; _LL2173: _temp2172=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2120)->f3; goto _LL2131;} else{ goto _LL2132;} _LL2132: if(( unsigned int)
_temp2120 > 1u?*(( int*) _temp2120) == Cyc_Absyn_While_s: 0){ _LL2181: _temp2180=((
struct Cyc_Absyn_While_s_struct*) _temp2120)->f1; _LL2185: _temp2184= _temp2180.f1;
goto _LL2183; _LL2183: _temp2182= _temp2180.f2; goto _LL2179; _LL2179: _temp2178=((
struct Cyc_Absyn_While_s_struct*) _temp2120)->f2; goto _LL2133;} else{ goto
_LL2134;} _LL2134: if(( unsigned int) _temp2120 > 1u?*(( int*) _temp2120) == Cyc_Absyn_Break_s:
0){ _LL2187: _temp2186=(( struct Cyc_Absyn_Break_s_struct*) _temp2120)->f1; goto
_LL2135;} else{ goto _LL2136;} _LL2136: if(( unsigned int) _temp2120 > 1u?*((
int*) _temp2120) == Cyc_Absyn_Continue_s: 0){ _LL2189: _temp2188=(( struct Cyc_Absyn_Continue_s_struct*)
_temp2120)->f1; goto _LL2137;} else{ goto _LL2138;} _LL2138: if(( unsigned int)
_temp2120 > 1u?*(( int*) _temp2120) == Cyc_Absyn_Goto_s: 0){ _LL2193: _temp2192=((
struct Cyc_Absyn_Goto_s_struct*) _temp2120)->f1; goto _LL2191; _LL2191:
_temp2190=(( struct Cyc_Absyn_Goto_s_struct*) _temp2120)->f2; goto _LL2139;}
else{ goto _LL2140;} _LL2140: if(( unsigned int) _temp2120 > 1u?*(( int*)
_temp2120) == Cyc_Absyn_For_s: 0){ _LL2209: _temp2208=(( struct Cyc_Absyn_For_s_struct*)
_temp2120)->f1; goto _LL2203; _LL2203: _temp2202=(( struct Cyc_Absyn_For_s_struct*)
_temp2120)->f2; _LL2207: _temp2206= _temp2202.f1; goto _LL2205; _LL2205:
_temp2204= _temp2202.f2; goto _LL2197; _LL2197: _temp2196=(( struct Cyc_Absyn_For_s_struct*)
_temp2120)->f3; _LL2201: _temp2200= _temp2196.f1; goto _LL2199; _LL2199:
_temp2198= _temp2196.f2; goto _LL2195; _LL2195: _temp2194=(( struct Cyc_Absyn_For_s_struct*)
_temp2120)->f4; goto _LL2141;} else{ goto _LL2142;} _LL2142: if(( unsigned int)
_temp2120 > 1u?*(( int*) _temp2120) == Cyc_Absyn_Switch_s: 0){ _LL2213:
_temp2212=(( struct Cyc_Absyn_Switch_s_struct*) _temp2120)->f1; goto _LL2211;
_LL2211: _temp2210=(( struct Cyc_Absyn_Switch_s_struct*) _temp2120)->f2; goto
_LL2143;} else{ goto _LL2144;} _LL2144: if(( unsigned int) _temp2120 > 1u?*((
int*) _temp2120) == Cyc_Absyn_SwitchC_s: 0){ _LL2217: _temp2216=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp2120)->f1; goto _LL2215; _LL2215: _temp2214=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp2120)->f2; goto _LL2145;} else{ goto _LL2146;} _LL2146: if(( unsigned int)
_temp2120 > 1u?*(( int*) _temp2120) == Cyc_Absyn_Fallthru_s: 0){ _LL2221:
_temp2220=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp2120)->f1; if( _temp2220
== 0){ goto _LL2219;} else{ goto _LL2148;} _LL2219: _temp2218=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2120)->f2; goto _LL2147;} else{ goto _LL2148;} _LL2148: if(( unsigned int)
_temp2120 > 1u?*(( int*) _temp2120) == Cyc_Absyn_Fallthru_s: 0){ _LL2225:
_temp2224=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp2120)->f1; goto _LL2223;
_LL2223: _temp2222=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp2120)->f2; goto
_LL2149;} else{ goto _LL2150;} _LL2150: if(( unsigned int) _temp2120 > 1u?*((
int*) _temp2120) == Cyc_Absyn_Decl_s: 0){ _LL2229: _temp2228=(( struct Cyc_Absyn_Decl_s_struct*)
_temp2120)->f1; goto _LL2227; _LL2227: _temp2226=(( struct Cyc_Absyn_Decl_s_struct*)
_temp2120)->f2; goto _LL2151;} else{ goto _LL2152;} _LL2152: if(( unsigned int)
_temp2120 > 1u?*(( int*) _temp2120) == Cyc_Absyn_Cut_s: 0){ _LL2231: _temp2230=((
struct Cyc_Absyn_Cut_s_struct*) _temp2120)->f1; goto _LL2153;} else{ goto
_LL2154;} _LL2154: if(( unsigned int) _temp2120 > 1u?*(( int*) _temp2120) == Cyc_Absyn_Splice_s:
0){ _LL2233: _temp2232=(( struct Cyc_Absyn_Splice_s_struct*) _temp2120)->f1;
goto _LL2155;} else{ goto _LL2156;} _LL2156: if(( unsigned int) _temp2120 > 1u?*((
int*) _temp2120) == Cyc_Absyn_Label_s: 0){ _LL2237: _temp2236=(( struct Cyc_Absyn_Label_s_struct*)
_temp2120)->f1; goto _LL2235; _LL2235: _temp2234=(( struct Cyc_Absyn_Label_s_struct*)
_temp2120)->f2; goto _LL2157;} else{ goto _LL2158;} _LL2158: if(( unsigned int)
_temp2120 > 1u?*(( int*) _temp2120) == Cyc_Absyn_Do_s: 0){ _LL2245: _temp2244=((
struct Cyc_Absyn_Do_s_struct*) _temp2120)->f1; goto _LL2239; _LL2239: _temp2238=((
struct Cyc_Absyn_Do_s_struct*) _temp2120)->f2; _LL2243: _temp2242= _temp2238.f1;
goto _LL2241; _LL2241: _temp2240= _temp2238.f2; goto _LL2159;} else{ goto
_LL2160;} _LL2160: if(( unsigned int) _temp2120 > 1u?*(( int*) _temp2120) == Cyc_Absyn_TryCatch_s:
0){ _LL2249: _temp2248=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp2120)->f1;
goto _LL2247; _LL2247: _temp2246=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2120)->f2; goto _LL2161;} else{ goto _LL2162;} _LL2162: if(( unsigned int)
_temp2120 > 1u?*(( int*) _temp2120) == Cyc_Absyn_Region_s: 0){ _LL2255:
_temp2254=(( struct Cyc_Absyn_Region_s_struct*) _temp2120)->f1; goto _LL2253;
_LL2253: _temp2252=(( struct Cyc_Absyn_Region_s_struct*) _temp2120)->f2; goto
_LL2251; _LL2251: _temp2250=(( struct Cyc_Absyn_Region_s_struct*) _temp2120)->f3;
goto _LL2163;} else{ goto _LL2121;} _LL2123: s= Cyc_PP_text(({ unsigned char*
_temp2256=( unsigned char*)";"; struct _tagged_string _temp2257; _temp2257.curr=
_temp2256; _temp2257.base= _temp2256; _temp2257.last_plus_one= _temp2256 + 2;
_temp2257;})); goto _LL2121; _LL2125: s=({ struct Cyc_PP_Doc*(* _temp2258)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2261= Cyc_Absynpp_exp2doc(
_temp2164); struct Cyc_PP_Doc* _temp2262= Cyc_PP_text(({ unsigned char*
_temp2263=( unsigned char*)";"; struct _tagged_string _temp2264; _temp2264.curr=
_temp2263; _temp2264.base= _temp2263; _temp2264.last_plus_one= _temp2263 + 2;
_temp2264;})); struct Cyc_PP_Doc* _temp2259[ 2u]={ _temp2261, _temp2262}; struct
_tagged_ptr0 _temp2260={ _temp2259, _temp2259, _temp2259 + 2u}; _temp2258(
_temp2260);}); goto _LL2121; _LL2127: if( Cyc_Absynpp_decls_first){ if( Cyc_Absynpp_is_declaration(
_temp2168)){ s=({ struct Cyc_PP_Doc*(* _temp2265)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2268= Cyc_PP_text(({ unsigned char* _temp2287=(
unsigned char*)"{ "; struct _tagged_string _temp2288; _temp2288.curr= _temp2287;
_temp2288.base= _temp2287; _temp2288.last_plus_one= _temp2287 + 3; _temp2288;}));
struct Cyc_PP_Doc* _temp2269= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2168));
struct Cyc_PP_Doc* _temp2270= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2271=
Cyc_PP_text(({ unsigned char* _temp2285=( unsigned char*)"}"; struct
_tagged_string _temp2286; _temp2286.curr= _temp2285; _temp2286.base= _temp2285;
_temp2286.last_plus_one= _temp2285 + 2; _temp2286;})); struct Cyc_PP_Doc*
_temp2272= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2273= Cyc_Absynpp_is_declaration(
_temp2166)?({ struct Cyc_PP_Doc*(* _temp2274)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2277= Cyc_PP_text(({ unsigned char* _temp2283=(
unsigned char*)"{ "; struct _tagged_string _temp2284; _temp2284.curr= _temp2283;
_temp2284.base= _temp2283; _temp2284.last_plus_one= _temp2283 + 3; _temp2284;}));
struct Cyc_PP_Doc* _temp2278= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2166));
struct Cyc_PP_Doc* _temp2279= Cyc_PP_text(({ unsigned char* _temp2281=(
unsigned char*)"}"; struct _tagged_string _temp2282; _temp2282.curr= _temp2281;
_temp2282.base= _temp2281; _temp2282.last_plus_one= _temp2281 + 2; _temp2282;}));
struct Cyc_PP_Doc* _temp2280= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2275[ 4u]={
_temp2277, _temp2278, _temp2279, _temp2280}; struct _tagged_ptr0 _temp2276={
_temp2275, _temp2275, _temp2275 + 4u}; _temp2274( _temp2276);}): Cyc_Absynpp_stmt2doc(
_temp2166); struct Cyc_PP_Doc* _temp2266[ 6u]={ _temp2268, _temp2269, _temp2270,
_temp2271, _temp2272, _temp2273}; struct _tagged_ptr0 _temp2267={ _temp2266,
_temp2266, _temp2266 + 6u}; _temp2265( _temp2267);});} else{ if( Cyc_Absynpp_is_declaration(
_temp2166)){ s=({ struct Cyc_PP_Doc*(* _temp2289)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2292= Cyc_Absynpp_stmt2doc( _temp2168); struct Cyc_PP_Doc*
_temp2293= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2294= Cyc_PP_text(({
unsigned char* _temp2300=( unsigned char*)"{ "; struct _tagged_string _temp2301;
_temp2301.curr= _temp2300; _temp2301.base= _temp2300; _temp2301.last_plus_one=
_temp2300 + 3; _temp2301;})); struct Cyc_PP_Doc* _temp2295= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp2166)); struct Cyc_PP_Doc* _temp2296= Cyc_PP_text(({ unsigned char*
_temp2298=( unsigned char*)"}"; struct _tagged_string _temp2299; _temp2299.curr=
_temp2298; _temp2299.base= _temp2298; _temp2299.last_plus_one= _temp2298 + 2;
_temp2299;})); struct Cyc_PP_Doc* _temp2297= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2290[ 6u]={ _temp2292, _temp2293, _temp2294, _temp2295, _temp2296,
_temp2297}; struct _tagged_ptr0 _temp2291={ _temp2290, _temp2290, _temp2290 + 6u};
_temp2289( _temp2291);});} else{ s=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Stmt*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,({ unsigned char* _temp2302=(
unsigned char*)""; struct _tagged_string _temp2303; _temp2303.curr= _temp2302;
_temp2303.base= _temp2302; _temp2303.last_plus_one= _temp2302 + 1; _temp2303;}),({
struct Cyc_List_List* _temp2304=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2304->hd=( void*) _temp2168; _temp2304->tl=({
struct Cyc_List_List* _temp2305=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2305->hd=( void*) _temp2166; _temp2305->tl= 0;
_temp2305;}); _temp2304;}));}}} else{ s=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Stmt*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,({ unsigned char* _temp2306=(
unsigned char*)""; struct _tagged_string _temp2307; _temp2307.curr= _temp2306;
_temp2307.base= _temp2306; _temp2307.last_plus_one= _temp2306 + 1; _temp2307;}),({
struct Cyc_List_List* _temp2308=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2308->hd=( void*) _temp2168; _temp2308->tl=({
struct Cyc_List_List* _temp2309=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2309->hd=( void*) _temp2166; _temp2309->tl= 0;
_temp2309;}); _temp2308;}));} goto _LL2121; _LL2129: if( _temp2170 == 0){ s= Cyc_PP_text(({
unsigned char* _temp2310=( unsigned char*)"return;"; struct _tagged_string
_temp2311; _temp2311.curr= _temp2310; _temp2311.base= _temp2310; _temp2311.last_plus_one=
_temp2310 + 8; _temp2311;}));} else{ s=({ struct Cyc_PP_Doc*(* _temp2312)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2315= Cyc_PP_text(({
unsigned char* _temp2320=( unsigned char*)"return "; struct _tagged_string
_temp2321; _temp2321.curr= _temp2320; _temp2321.base= _temp2320; _temp2321.last_plus_one=
_temp2320 + 8; _temp2321;})); struct Cyc_PP_Doc* _temp2316= _temp2170 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp2170)); struct
Cyc_PP_Doc* _temp2317= Cyc_PP_text(({ unsigned char* _temp2318=( unsigned char*)";";
struct _tagged_string _temp2319; _temp2319.curr= _temp2318; _temp2319.base=
_temp2318; _temp2319.last_plus_one= _temp2318 + 2; _temp2319;})); struct Cyc_PP_Doc*
_temp2313[ 3u]={ _temp2315, _temp2316, _temp2317}; struct _tagged_ptr0 _temp2314={
_temp2313, _temp2313, _temp2313 + 3u}; _temp2312( _temp2314);});} goto _LL2121;
_LL2131: { int print_else;{ void* _temp2322=( void*) _temp2172->r; _LL2324: if(
_temp2322 ==( void*) Cyc_Absyn_Skip_s){ goto _LL2325;} else{ goto _LL2326;}
_LL2326: goto _LL2327; _LL2325: print_else= 0; goto _LL2323; _LL2327: print_else=
1; goto _LL2323; _LL2323:;} s=({ struct Cyc_PP_Doc*(* _temp2328)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2331= Cyc_PP_text(({
unsigned char* _temp2364=( unsigned char*)"if ("; struct _tagged_string
_temp2365; _temp2365.curr= _temp2364; _temp2365.base= _temp2364; _temp2365.last_plus_one=
_temp2364 + 5; _temp2365;})); struct Cyc_PP_Doc* _temp2332= Cyc_Absynpp_exp2doc(
_temp2176); struct Cyc_PP_Doc* _temp2333= Cyc_PP_text(({ unsigned char*
_temp2362=( unsigned char*)") {"; struct _tagged_string _temp2363; _temp2363.curr=
_temp2362; _temp2363.base= _temp2362; _temp2363.last_plus_one= _temp2362 + 4;
_temp2363;})); struct Cyc_PP_Doc* _temp2334= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(*
_temp2357)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2360= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2361= Cyc_Absynpp_stmt2doc( _temp2174); struct Cyc_PP_Doc*
_temp2358[ 2u]={ _temp2360, _temp2361}; struct _tagged_ptr0 _temp2359={
_temp2358, _temp2358, _temp2358 + 2u}; _temp2357( _temp2359);})); struct Cyc_PP_Doc*
_temp2335= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2336= Cyc_PP_text(({
unsigned char* _temp2355=( unsigned char*)"}"; struct _tagged_string _temp2356;
_temp2356.curr= _temp2355; _temp2356.base= _temp2355; _temp2356.last_plus_one=
_temp2355 + 2; _temp2356;})); struct Cyc_PP_Doc* _temp2337= print_else?({ struct
Cyc_PP_Doc*(* _temp2338)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2341= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2342= Cyc_PP_text(({
unsigned char* _temp2353=( unsigned char*)"else {"; struct _tagged_string
_temp2354; _temp2354.curr= _temp2353; _temp2354.base= _temp2353; _temp2354.last_plus_one=
_temp2353 + 7; _temp2354;})); struct Cyc_PP_Doc* _temp2343= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc*(* _temp2348)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2351= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2352= Cyc_Absynpp_stmt2doc(
_temp2172); struct Cyc_PP_Doc* _temp2349[ 2u]={ _temp2351, _temp2352}; struct
_tagged_ptr0 _temp2350={ _temp2349, _temp2349, _temp2349 + 2u}; _temp2348(
_temp2350);})); struct Cyc_PP_Doc* _temp2344= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2345= Cyc_PP_text(({ unsigned char* _temp2346=( unsigned char*)"}"; struct
_tagged_string _temp2347; _temp2347.curr= _temp2346; _temp2347.base= _temp2346;
_temp2347.last_plus_one= _temp2346 + 2; _temp2347;})); struct Cyc_PP_Doc*
_temp2339[ 5u]={ _temp2341, _temp2342, _temp2343, _temp2344, _temp2345}; struct
_tagged_ptr0 _temp2340={ _temp2339, _temp2339, _temp2339 + 5u}; _temp2338(
_temp2340);}): Cyc_PP_nil_doc(); struct Cyc_PP_Doc* _temp2329[ 7u]={ _temp2331,
_temp2332, _temp2333, _temp2334, _temp2335, _temp2336, _temp2337}; struct
_tagged_ptr0 _temp2330={ _temp2329, _temp2329, _temp2329 + 7u}; _temp2328(
_temp2330);}); goto _LL2121;} _LL2133: s=({ struct Cyc_PP_Doc*(* _temp2366)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2369= Cyc_PP_text(({
unsigned char* _temp2384=( unsigned char*)"while ("; struct _tagged_string
_temp2385; _temp2385.curr= _temp2384; _temp2385.base= _temp2384; _temp2385.last_plus_one=
_temp2384 + 8; _temp2385;})); struct Cyc_PP_Doc* _temp2370= Cyc_Absynpp_exp2doc(
_temp2184); struct Cyc_PP_Doc* _temp2371= Cyc_PP_text(({ unsigned char*
_temp2382=( unsigned char*)") {"; struct _tagged_string _temp2383; _temp2383.curr=
_temp2382; _temp2383.base= _temp2382; _temp2383.last_plus_one= _temp2382 + 4;
_temp2383;})); struct Cyc_PP_Doc* _temp2372= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(*
_temp2377)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2380= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2381= Cyc_Absynpp_stmt2doc( _temp2178); struct Cyc_PP_Doc*
_temp2378[ 2u]={ _temp2380, _temp2381}; struct _tagged_ptr0 _temp2379={
_temp2378, _temp2378, _temp2378 + 2u}; _temp2377( _temp2379);})); struct Cyc_PP_Doc*
_temp2373= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2374= Cyc_PP_text(({
unsigned char* _temp2375=( unsigned char*)"}"; struct _tagged_string _temp2376;
_temp2376.curr= _temp2375; _temp2376.base= _temp2375; _temp2376.last_plus_one=
_temp2375 + 2; _temp2376;})); struct Cyc_PP_Doc* _temp2367[ 6u]={ _temp2369,
_temp2370, _temp2371, _temp2372, _temp2373, _temp2374}; struct _tagged_ptr0
_temp2368={ _temp2367, _temp2367, _temp2367 + 6u}; _temp2366( _temp2368);});
goto _LL2121; _LL2135: s= Cyc_PP_text(({ unsigned char* _temp2386=(
unsigned char*)"break;"; struct _tagged_string _temp2387; _temp2387.curr=
_temp2386; _temp2387.base= _temp2386; _temp2387.last_plus_one= _temp2386 + 7;
_temp2387;})); goto _LL2121; _LL2137: s= Cyc_PP_text(({ unsigned char* _temp2388=(
unsigned char*)"continue;"; struct _tagged_string _temp2389; _temp2389.curr=
_temp2388; _temp2389.base= _temp2388; _temp2389.last_plus_one= _temp2388 + 10;
_temp2389;})); goto _LL2121; _LL2139: s= Cyc_PP_text(( struct _tagged_string)({
struct _tagged_string _temp2390=* _temp2192; xprintf("goto %.*s;", _temp2390.last_plus_one
- _temp2390.curr, _temp2390.curr);})); goto _LL2121; _LL2141: s=({ struct Cyc_PP_Doc*(*
_temp2391)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2394= Cyc_PP_text(({
unsigned char* _temp2417=( unsigned char*)"for("; struct _tagged_string
_temp2418; _temp2418.curr= _temp2417; _temp2418.base= _temp2417; _temp2418.last_plus_one=
_temp2417 + 5; _temp2418;})); struct Cyc_PP_Doc* _temp2395= Cyc_Absynpp_exp2doc(
_temp2208); struct Cyc_PP_Doc* _temp2396= Cyc_PP_text(({ unsigned char*
_temp2415=( unsigned char*)"; "; struct _tagged_string _temp2416; _temp2416.curr=
_temp2415; _temp2416.base= _temp2415; _temp2416.last_plus_one= _temp2415 + 3;
_temp2416;})); struct Cyc_PP_Doc* _temp2397= Cyc_Absynpp_exp2doc( _temp2206);
struct Cyc_PP_Doc* _temp2398= Cyc_PP_text(({ unsigned char* _temp2413=(
unsigned char*)"; "; struct _tagged_string _temp2414; _temp2414.curr= _temp2413;
_temp2414.base= _temp2413; _temp2414.last_plus_one= _temp2413 + 3; _temp2414;}));
struct Cyc_PP_Doc* _temp2399= Cyc_Absynpp_exp2doc( _temp2200); struct Cyc_PP_Doc*
_temp2400= Cyc_PP_text(({ unsigned char* _temp2411=( unsigned char*)") {";
struct _tagged_string _temp2412; _temp2412.curr= _temp2411; _temp2412.base=
_temp2411; _temp2412.last_plus_one= _temp2411 + 4; _temp2412;})); struct Cyc_PP_Doc*
_temp2401= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2406)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2409= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2410= Cyc_Absynpp_stmt2doc( _temp2194); struct Cyc_PP_Doc*
_temp2407[ 2u]={ _temp2409, _temp2410}; struct _tagged_ptr0 _temp2408={
_temp2407, _temp2407, _temp2407 + 2u}; _temp2406( _temp2408);})); struct Cyc_PP_Doc*
_temp2402= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2403= Cyc_PP_text(({
unsigned char* _temp2404=( unsigned char*)"}"; struct _tagged_string _temp2405;
_temp2405.curr= _temp2404; _temp2405.base= _temp2404; _temp2405.last_plus_one=
_temp2404 + 2; _temp2405;})); struct Cyc_PP_Doc* _temp2392[ 10u]={ _temp2394,
_temp2395, _temp2396, _temp2397, _temp2398, _temp2399, _temp2400, _temp2401,
_temp2402, _temp2403}; struct _tagged_ptr0 _temp2393={ _temp2392, _temp2392,
_temp2392 + 10u}; _temp2391( _temp2393);}); goto _LL2121; _LL2143: s=({ struct
Cyc_PP_Doc*(* _temp2419)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2422= Cyc_PP_text(({ unsigned char* _temp2433=( unsigned char*)"switch (";
struct _tagged_string _temp2434; _temp2434.curr= _temp2433; _temp2434.base=
_temp2433; _temp2434.last_plus_one= _temp2433 + 9; _temp2434;})); struct Cyc_PP_Doc*
_temp2423= Cyc_Absynpp_exp2doc( _temp2212); struct Cyc_PP_Doc* _temp2424= Cyc_PP_text(({
unsigned char* _temp2431=( unsigned char*)") {"; struct _tagged_string _temp2432;
_temp2432.curr= _temp2431; _temp2432.base= _temp2431; _temp2432.last_plus_one=
_temp2431 + 4; _temp2432;})); struct Cyc_PP_Doc* _temp2425= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2426= Cyc_Absynpp_switchclauses2doc( _temp2210); struct
Cyc_PP_Doc* _temp2427= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2428= Cyc_PP_text(({
unsigned char* _temp2429=( unsigned char*)"}"; struct _tagged_string _temp2430;
_temp2430.curr= _temp2429; _temp2430.base= _temp2429; _temp2430.last_plus_one=
_temp2429 + 2; _temp2430;})); struct Cyc_PP_Doc* _temp2420[ 7u]={ _temp2422,
_temp2423, _temp2424, _temp2425, _temp2426, _temp2427, _temp2428}; struct
_tagged_ptr0 _temp2421={ _temp2420, _temp2420, _temp2420 + 7u}; _temp2419(
_temp2421);}); goto _LL2121; _LL2145: s=({ struct Cyc_PP_Doc*(* _temp2435)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2438= Cyc_PP_text(({
unsigned char* _temp2449=( unsigned char*)"switch \"C\" ("; struct
_tagged_string _temp2450; _temp2450.curr= _temp2449; _temp2450.base= _temp2449;
_temp2450.last_plus_one= _temp2449 + 13; _temp2450;})); struct Cyc_PP_Doc*
_temp2439= Cyc_Absynpp_exp2doc( _temp2216); struct Cyc_PP_Doc* _temp2440= Cyc_PP_text(({
unsigned char* _temp2447=( unsigned char*)") {"; struct _tagged_string _temp2448;
_temp2448.curr= _temp2447; _temp2448.base= _temp2447; _temp2448.last_plus_one=
_temp2447 + 4; _temp2448;})); struct Cyc_PP_Doc* _temp2441= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2442= Cyc_Absynpp_switchCclauses2doc( _temp2214); struct
Cyc_PP_Doc* _temp2443= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2444= Cyc_PP_text(({
unsigned char* _temp2445=( unsigned char*)"}"; struct _tagged_string _temp2446;
_temp2446.curr= _temp2445; _temp2446.base= _temp2445; _temp2446.last_plus_one=
_temp2445 + 2; _temp2446;})); struct Cyc_PP_Doc* _temp2436[ 7u]={ _temp2438,
_temp2439, _temp2440, _temp2441, _temp2442, _temp2443, _temp2444}; struct
_tagged_ptr0 _temp2437={ _temp2436, _temp2436, _temp2436 + 7u}; _temp2435(
_temp2437);}); goto _LL2121; _LL2147: s= Cyc_PP_text(({ unsigned char* _temp2451=(
unsigned char*)"fallthru;"; struct _tagged_string _temp2452; _temp2452.curr=
_temp2451; _temp2452.base= _temp2451; _temp2452.last_plus_one= _temp2451 + 10;
_temp2452;})); goto _LL2121; _LL2149: s=({ struct Cyc_PP_Doc*(* _temp2453)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2456= Cyc_PP_text(({
unsigned char* _temp2461=( unsigned char*)"fallthru("; struct _tagged_string
_temp2462; _temp2462.curr= _temp2461; _temp2462.base= _temp2461; _temp2462.last_plus_one=
_temp2461 + 10; _temp2462;})); struct Cyc_PP_Doc* _temp2457= Cyc_Absynpp_exps2doc_prec(
20, _temp2224); struct Cyc_PP_Doc* _temp2458= Cyc_PP_text(({ unsigned char*
_temp2459=( unsigned char*)");"; struct _tagged_string _temp2460; _temp2460.curr=
_temp2459; _temp2460.base= _temp2459; _temp2460.last_plus_one= _temp2459 + 3;
_temp2460;})); struct Cyc_PP_Doc* _temp2454[ 3u]={ _temp2456, _temp2457,
_temp2458}; struct _tagged_ptr0 _temp2455={ _temp2454, _temp2454, _temp2454 + 3u};
_temp2453( _temp2455);}); goto _LL2121; _LL2151: s=({ struct Cyc_PP_Doc*(*
_temp2463)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2466= Cyc_Absynpp_decl2doc(
_temp2228); struct Cyc_PP_Doc* _temp2467= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2468= Cyc_Absynpp_stmt2doc( _temp2226); struct Cyc_PP_Doc* _temp2464[ 3u]={
_temp2466, _temp2467, _temp2468}; struct _tagged_ptr0 _temp2465={ _temp2464,
_temp2464, _temp2464 + 3u}; _temp2463( _temp2465);}); goto _LL2121; _LL2153: s=({
struct Cyc_PP_Doc*(* _temp2469)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2472= Cyc_PP_text(({ unsigned char* _temp2474=( unsigned char*)"cut ";
struct _tagged_string _temp2475; _temp2475.curr= _temp2474; _temp2475.base=
_temp2474; _temp2475.last_plus_one= _temp2474 + 5; _temp2475;})); struct Cyc_PP_Doc*
_temp2473= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2230)); struct Cyc_PP_Doc*
_temp2470[ 2u]={ _temp2472, _temp2473}; struct _tagged_ptr0 _temp2471={
_temp2470, _temp2470, _temp2470 + 2u}; _temp2469( _temp2471);}); goto _LL2121;
_LL2155: s=({ struct Cyc_PP_Doc*(* _temp2476)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2479= Cyc_PP_text(({ unsigned char* _temp2481=(
unsigned char*)"splice "; struct _tagged_string _temp2482; _temp2482.curr=
_temp2481; _temp2482.base= _temp2481; _temp2482.last_plus_one= _temp2481 + 8;
_temp2482;})); struct Cyc_PP_Doc* _temp2480= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp2232)); struct Cyc_PP_Doc* _temp2477[ 2u]={ _temp2479, _temp2480}; struct
_tagged_ptr0 _temp2478={ _temp2477, _temp2477, _temp2477 + 2u}; _temp2476(
_temp2478);}); goto _LL2121; _LL2157: if( Cyc_Absynpp_decls_first? Cyc_Absynpp_is_declaration(
_temp2234): 0){ s=({ struct Cyc_PP_Doc*(* _temp2483)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2486= Cyc_PP_textptr( _temp2236); struct Cyc_PP_Doc*
_temp2487= Cyc_PP_text(({ unsigned char* _temp2494=( unsigned char*)": {";
struct _tagged_string _temp2495; _temp2495.curr= _temp2494; _temp2495.base=
_temp2494; _temp2495.last_plus_one= _temp2494 + 4; _temp2495;})); struct Cyc_PP_Doc*
_temp2488= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2489= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp2234)); struct Cyc_PP_Doc* _temp2490= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2491= Cyc_PP_text(({ unsigned char* _temp2492=( unsigned char*)"}"; struct
_tagged_string _temp2493; _temp2493.curr= _temp2492; _temp2493.base= _temp2492;
_temp2493.last_plus_one= _temp2492 + 2; _temp2493;})); struct Cyc_PP_Doc*
_temp2484[ 6u]={ _temp2486, _temp2487, _temp2488, _temp2489, _temp2490,
_temp2491}; struct _tagged_ptr0 _temp2485={ _temp2484, _temp2484, _temp2484 + 6u};
_temp2483( _temp2485);});} else{ s=({ struct Cyc_PP_Doc*(* _temp2496)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2499= Cyc_PP_textptr(
_temp2236); struct Cyc_PP_Doc* _temp2500= Cyc_PP_text(({ unsigned char*
_temp2502=( unsigned char*)": "; struct _tagged_string _temp2503; _temp2503.curr=
_temp2502; _temp2503.base= _temp2502; _temp2503.last_plus_one= _temp2502 + 3;
_temp2503;})); struct Cyc_PP_Doc* _temp2501= Cyc_Absynpp_stmt2doc( _temp2234);
struct Cyc_PP_Doc* _temp2497[ 3u]={ _temp2499, _temp2500, _temp2501}; struct
_tagged_ptr0 _temp2498={ _temp2497, _temp2497, _temp2497 + 3u}; _temp2496(
_temp2498);});} goto _LL2121; _LL2159: s=({ struct Cyc_PP_Doc*(* _temp2504)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2507= Cyc_PP_text(({
unsigned char* _temp2518=( unsigned char*)"do {"; struct _tagged_string
_temp2519; _temp2519.curr= _temp2518; _temp2519.base= _temp2518; _temp2519.last_plus_one=
_temp2518 + 5; _temp2519;})); struct Cyc_PP_Doc* _temp2508= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2509= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2244));
struct Cyc_PP_Doc* _temp2510= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2511=
Cyc_PP_text(({ unsigned char* _temp2516=( unsigned char*)"} while ("; struct
_tagged_string _temp2517; _temp2517.curr= _temp2516; _temp2517.base= _temp2516;
_temp2517.last_plus_one= _temp2516 + 10; _temp2517;})); struct Cyc_PP_Doc*
_temp2512= Cyc_Absynpp_exp2doc( _temp2242); struct Cyc_PP_Doc* _temp2513= Cyc_PP_text(({
unsigned char* _temp2514=( unsigned char*)");"; struct _tagged_string _temp2515;
_temp2515.curr= _temp2514; _temp2515.base= _temp2514; _temp2515.last_plus_one=
_temp2514 + 3; _temp2515;})); struct Cyc_PP_Doc* _temp2505[ 7u]={ _temp2507,
_temp2508, _temp2509, _temp2510, _temp2511, _temp2512, _temp2513}; struct
_tagged_ptr0 _temp2506={ _temp2505, _temp2505, _temp2505 + 7u}; _temp2504(
_temp2506);}); goto _LL2121; _LL2161: s=({ struct Cyc_PP_Doc*(* _temp2520)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2523= Cyc_PP_text(({
unsigned char* _temp2536=( unsigned char*)"try {"; struct _tagged_string
_temp2537; _temp2537.curr= _temp2536; _temp2537.base= _temp2536; _temp2537.last_plus_one=
_temp2536 + 6; _temp2537;})); struct Cyc_PP_Doc* _temp2524= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2525= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2248));
struct Cyc_PP_Doc* _temp2526= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2527=
Cyc_PP_text(({ unsigned char* _temp2534=( unsigned char*)"} catch {"; struct
_tagged_string _temp2535; _temp2535.curr= _temp2534; _temp2535.base= _temp2534;
_temp2535.last_plus_one= _temp2534 + 10; _temp2535;})); struct Cyc_PP_Doc*
_temp2528= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2529= Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc(
_temp2246)); struct Cyc_PP_Doc* _temp2530= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2531= Cyc_PP_text(({ unsigned char* _temp2532=( unsigned char*)"}"; struct
_tagged_string _temp2533; _temp2533.curr= _temp2532; _temp2533.base= _temp2532;
_temp2533.last_plus_one= _temp2532 + 2; _temp2533;})); struct Cyc_PP_Doc*
_temp2521[ 9u]={ _temp2523, _temp2524, _temp2525, _temp2526, _temp2527,
_temp2528, _temp2529, _temp2530, _temp2531}; struct _tagged_ptr0 _temp2522={
_temp2521, _temp2521, _temp2521 + 9u}; _temp2520( _temp2522);}); goto _LL2121;
_LL2163: s=({ struct Cyc_PP_Doc*(* _temp2538)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2541= Cyc_PP_text(({ unsigned char* _temp2556=(
unsigned char*)"region<"; struct _tagged_string _temp2557; _temp2557.curr=
_temp2556; _temp2557.base= _temp2556; _temp2557.last_plus_one= _temp2556 + 8;
_temp2557;})); struct Cyc_PP_Doc* _temp2542= Cyc_PP_textptr( Cyc_Absynpp_get_name(
_temp2254)); struct Cyc_PP_Doc* _temp2543= Cyc_PP_text(({ unsigned char*
_temp2554=( unsigned char*)">"; struct _tagged_string _temp2555; _temp2555.curr=
_temp2554; _temp2555.base= _temp2554; _temp2555.last_plus_one= _temp2554 + 2;
_temp2555;})); struct Cyc_PP_Doc* _temp2544= Cyc_Absynpp_qvar2doc( _temp2252->name);
struct Cyc_PP_Doc* _temp2545= Cyc_PP_text(({ unsigned char* _temp2552=(
unsigned char*)"{"; struct _tagged_string _temp2553; _temp2553.curr= _temp2552;
_temp2553.base= _temp2552; _temp2553.last_plus_one= _temp2552 + 2; _temp2553;}));
struct Cyc_PP_Doc* _temp2546= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2547=
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2250)); struct Cyc_PP_Doc* _temp2548=
Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2549= Cyc_PP_text(({ unsigned char*
_temp2550=( unsigned char*)"}"; struct _tagged_string _temp2551; _temp2551.curr=
_temp2550; _temp2551.base= _temp2550; _temp2551.last_plus_one= _temp2550 + 2;
_temp2551;})); struct Cyc_PP_Doc* _temp2539[ 9u]={ _temp2541, _temp2542,
_temp2543, _temp2544, _temp2545, _temp2546, _temp2547, _temp2548, _temp2549};
struct _tagged_ptr0 _temp2540={ _temp2539, _temp2539, _temp2539 + 9u}; _temp2538(
_temp2540);}); goto _LL2121; _LL2121:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void* _temp2558=( void*) p->r;
int _temp2590; void* _temp2592; unsigned char _temp2594; struct _tagged_string
_temp2596; struct Cyc_Absyn_Vardecl* _temp2598; struct Cyc_List_List* _temp2600;
struct Cyc_Absyn_Pat* _temp2602; struct Cyc_Absyn_Vardecl* _temp2604; struct
_tuple0* _temp2606; struct Cyc_List_List* _temp2608; struct Cyc_List_List*
_temp2610; struct _tuple0* _temp2612; struct Cyc_List_List* _temp2614; struct
Cyc_List_List* _temp2616; struct _tuple0* _temp2618; struct Cyc_List_List*
_temp2620; struct Cyc_List_List* _temp2622; struct Cyc_Core_Opt* _temp2624;
struct Cyc_Absyn_Structdecl* _temp2626; struct Cyc_Absyn_Enumfield* _temp2628;
struct Cyc_Absyn_Enumdecl* _temp2630; struct Cyc_List_List* _temp2632; struct
Cyc_List_List* _temp2634; struct Cyc_Absyn_Tunionfield* _temp2636; struct Cyc_Absyn_Tuniondecl*
_temp2638; _LL2560: if( _temp2558 ==( void*) Cyc_Absyn_Wild_p){ goto _LL2561;}
else{ goto _LL2562;} _LL2562: if( _temp2558 ==( void*) Cyc_Absyn_Null_p){ goto
_LL2563;} else{ goto _LL2564;} _LL2564: if(( unsigned int) _temp2558 > 2u?*((
int*) _temp2558) == Cyc_Absyn_Int_p: 0){ _LL2593: _temp2592=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp2558)->f1; goto _LL2591; _LL2591: _temp2590=((
struct Cyc_Absyn_Int_p_struct*) _temp2558)->f2; goto _LL2565;} else{ goto
_LL2566;} _LL2566: if(( unsigned int) _temp2558 > 2u?*(( int*) _temp2558) == Cyc_Absyn_Char_p:
0){ _LL2595: _temp2594=(( struct Cyc_Absyn_Char_p_struct*) _temp2558)->f1; goto
_LL2567;} else{ goto _LL2568;} _LL2568: if(( unsigned int) _temp2558 > 2u?*((
int*) _temp2558) == Cyc_Absyn_Float_p: 0){ _LL2597: _temp2596=(( struct Cyc_Absyn_Float_p_struct*)
_temp2558)->f1; goto _LL2569;} else{ goto _LL2570;} _LL2570: if(( unsigned int)
_temp2558 > 2u?*(( int*) _temp2558) == Cyc_Absyn_Var_p: 0){ _LL2599: _temp2598=((
struct Cyc_Absyn_Var_p_struct*) _temp2558)->f1; goto _LL2571;} else{ goto
_LL2572;} _LL2572: if(( unsigned int) _temp2558 > 2u?*(( int*) _temp2558) == Cyc_Absyn_Tuple_p:
0){ _LL2601: _temp2600=(( struct Cyc_Absyn_Tuple_p_struct*) _temp2558)->f1; goto
_LL2573;} else{ goto _LL2574;} _LL2574: if(( unsigned int) _temp2558 > 2u?*((
int*) _temp2558) == Cyc_Absyn_Pointer_p: 0){ _LL2603: _temp2602=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2558)->f1; goto _LL2575;} else{ goto _LL2576;} _LL2576: if(( unsigned int)
_temp2558 > 2u?*(( int*) _temp2558) == Cyc_Absyn_Reference_p: 0){ _LL2605:
_temp2604=(( struct Cyc_Absyn_Reference_p_struct*) _temp2558)->f1; goto _LL2577;}
else{ goto _LL2578;} _LL2578: if(( unsigned int) _temp2558 > 2u?*(( int*)
_temp2558) == Cyc_Absyn_UnknownId_p: 0){ _LL2607: _temp2606=(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp2558)->f1; goto _LL2579;} else{ goto _LL2580;} _LL2580: if(( unsigned int)
_temp2558 > 2u?*(( int*) _temp2558) == Cyc_Absyn_UnknownCall_p: 0){ _LL2613:
_temp2612=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp2558)->f1; goto
_LL2611; _LL2611: _temp2610=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp2558)->f2;
goto _LL2609; _LL2609: _temp2608=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2558)->f3; goto _LL2581;} else{ goto _LL2582;} _LL2582: if(( unsigned int)
_temp2558 > 2u?*(( int*) _temp2558) == Cyc_Absyn_UnknownFields_p: 0){ _LL2619:
_temp2618=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp2558)->f1; goto
_LL2617; _LL2617: _temp2616=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2558)->f2; goto _LL2615; _LL2615: _temp2614=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2558)->f3; goto _LL2583;} else{ goto _LL2584;} _LL2584: if(( unsigned int)
_temp2558 > 2u?*(( int*) _temp2558) == Cyc_Absyn_Struct_p: 0){ _LL2627:
_temp2626=(( struct Cyc_Absyn_Struct_p_struct*) _temp2558)->f1; goto _LL2625;
_LL2625: _temp2624=(( struct Cyc_Absyn_Struct_p_struct*) _temp2558)->f2; goto
_LL2623; _LL2623: _temp2622=(( struct Cyc_Absyn_Struct_p_struct*) _temp2558)->f3;
goto _LL2621; _LL2621: _temp2620=(( struct Cyc_Absyn_Struct_p_struct*) _temp2558)->f4;
goto _LL2585;} else{ goto _LL2586;} _LL2586: if(( unsigned int) _temp2558 > 2u?*((
int*) _temp2558) == Cyc_Absyn_Enum_p: 0){ _LL2631: _temp2630=(( struct Cyc_Absyn_Enum_p_struct*)
_temp2558)->f1; goto _LL2629; _LL2629: _temp2628=(( struct Cyc_Absyn_Enum_p_struct*)
_temp2558)->f2; goto _LL2587;} else{ goto _LL2588;} _LL2588: if(( unsigned int)
_temp2558 > 2u?*(( int*) _temp2558) == Cyc_Absyn_Tunion_p: 0){ _LL2639:
_temp2638=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2558)->f1; goto _LL2637;
_LL2637: _temp2636=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2558)->f2; goto
_LL2635; _LL2635: _temp2634=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2558)->f3;
goto _LL2633; _LL2633: _temp2632=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2558)->f4;
goto _LL2589;} else{ goto _LL2559;} _LL2561: s= Cyc_PP_text(({ unsigned char*
_temp2640=( unsigned char*)"_"; struct _tagged_string _temp2641; _temp2641.curr=
_temp2640; _temp2641.base= _temp2640; _temp2641.last_plus_one= _temp2640 + 2;
_temp2641;})); goto _LL2559; _LL2563: s= Cyc_PP_text(({ unsigned char* _temp2642=(
unsigned char*)"null"; struct _tagged_string _temp2643; _temp2643.curr=
_temp2642; _temp2643.base= _temp2642; _temp2643.last_plus_one= _temp2642 + 5;
_temp2643;})); goto _LL2559; _LL2565: if( _temp2592 ==( void*) Cyc_Absyn_Signed){
s= Cyc_PP_text(( struct _tagged_string) xprintf("%d", _temp2590));} else{ s= Cyc_PP_text((
struct _tagged_string) xprintf("%u",( unsigned int) _temp2590));} goto _LL2559;
_LL2567: s= Cyc_PP_text(( struct _tagged_string)({ struct _tagged_string
_temp2644= Cyc_Absynpp_char_escape( _temp2594); xprintf("'%.*s'", _temp2644.last_plus_one
- _temp2644.curr, _temp2644.curr);})); goto _LL2559; _LL2569: s= Cyc_PP_text(
_temp2596); goto _LL2559; _LL2571: s= Cyc_Absynpp_qvar2doc( _temp2598->name);
goto _LL2559; _LL2573: s=({ struct Cyc_PP_Doc*(* _temp2645)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp2648= Cyc_PP_text(({ unsigned char*
_temp2655=( unsigned char*)"$("; struct _tagged_string _temp2656; _temp2656.curr=
_temp2655; _temp2656.base= _temp2655; _temp2656.last_plus_one= _temp2655 + 3;
_temp2656;})); struct Cyc_PP_Doc* _temp2649=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Pat*), struct _tagged_string sep, struct Cyc_List_List* l0))
Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc,({ unsigned char* _temp2653=( unsigned char*)",";
struct _tagged_string _temp2654; _temp2654.curr= _temp2653; _temp2654.base=
_temp2653; _temp2654.last_plus_one= _temp2653 + 2; _temp2654;}), _temp2600);
struct Cyc_PP_Doc* _temp2650= Cyc_PP_text(({ unsigned char* _temp2651=(
unsigned char*)")"; struct _tagged_string _temp2652; _temp2652.curr= _temp2651;
_temp2652.base= _temp2651; _temp2652.last_plus_one= _temp2651 + 2; _temp2652;}));
struct Cyc_PP_Doc* _temp2646[ 3u]={ _temp2648, _temp2649, _temp2650}; struct
_tagged_ptr0 _temp2647={ _temp2646, _temp2646, _temp2646 + 3u}; _temp2645(
_temp2647);}); goto _LL2559; _LL2575: s=({ struct Cyc_PP_Doc*(* _temp2657)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2660= Cyc_PP_text(({
unsigned char* _temp2662=( unsigned char*)"&"; struct _tagged_string _temp2663;
_temp2663.curr= _temp2662; _temp2663.base= _temp2662; _temp2663.last_plus_one=
_temp2662 + 2; _temp2663;})); struct Cyc_PP_Doc* _temp2661= Cyc_Absynpp_pat2doc(
_temp2602); struct Cyc_PP_Doc* _temp2658[ 2u]={ _temp2660, _temp2661}; struct
_tagged_ptr0 _temp2659={ _temp2658, _temp2658, _temp2658 + 2u}; _temp2657(
_temp2659);}); goto _LL2559; _LL2577: s=({ struct Cyc_PP_Doc*(* _temp2664)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2667= Cyc_PP_text(({
unsigned char* _temp2669=( unsigned char*)"*"; struct _tagged_string _temp2670;
_temp2670.curr= _temp2669; _temp2670.base= _temp2669; _temp2670.last_plus_one=
_temp2669 + 2; _temp2670;})); struct Cyc_PP_Doc* _temp2668= Cyc_Absynpp_qvar2doc(
_temp2604->name); struct Cyc_PP_Doc* _temp2665[ 2u]={ _temp2667, _temp2668};
struct _tagged_ptr0 _temp2666={ _temp2665, _temp2665, _temp2665 + 2u}; _temp2664(
_temp2666);}); goto _LL2559; _LL2579: s= Cyc_Absynpp_qvar2doc( _temp2606); goto
_LL2559; _LL2581: s=({ struct Cyc_PP_Doc*(* _temp2671)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp2674= Cyc_Absynpp_qvar2doc( _temp2612);
struct Cyc_PP_Doc* _temp2675= Cyc_Absynpp_tvars2doc( _temp2610); struct Cyc_PP_Doc*
_temp2676= Cyc_PP_group(({ unsigned char* _temp2677=( unsigned char*)"("; struct
_tagged_string _temp2678; _temp2678.curr= _temp2677; _temp2678.base= _temp2677;
_temp2678.last_plus_one= _temp2677 + 2; _temp2678;}),({ unsigned char* _temp2679=(
unsigned char*)")"; struct _tagged_string _temp2680; _temp2680.curr= _temp2679;
_temp2680.base= _temp2679; _temp2680.last_plus_one= _temp2679 + 2; _temp2680;}),({
unsigned char* _temp2681=( unsigned char*)","; struct _tagged_string _temp2682;
_temp2682.curr= _temp2681; _temp2682.base= _temp2681; _temp2682.last_plus_one=
_temp2681 + 2; _temp2682;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc,
_temp2608)); struct Cyc_PP_Doc* _temp2672[ 3u]={ _temp2674, _temp2675, _temp2676};
struct _tagged_ptr0 _temp2673={ _temp2672, _temp2672, _temp2672 + 3u}; _temp2671(
_temp2673);}); goto _LL2559; _LL2583: s=({ struct Cyc_PP_Doc*(* _temp2683)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2686= Cyc_Absynpp_qvar2doc(
_temp2618); struct Cyc_PP_Doc* _temp2687= Cyc_Absynpp_tvars2doc( _temp2616);
struct Cyc_PP_Doc* _temp2688= Cyc_PP_group(({ unsigned char* _temp2689=(
unsigned char*)"{"; struct _tagged_string _temp2690; _temp2690.curr= _temp2689;
_temp2690.base= _temp2689; _temp2690.last_plus_one= _temp2689 + 2; _temp2690;}),({
unsigned char* _temp2691=( unsigned char*)"}"; struct _tagged_string _temp2692;
_temp2692.curr= _temp2691; _temp2692.base= _temp2691; _temp2692.last_plus_one=
_temp2691 + 2; _temp2692;}),({ unsigned char* _temp2693=( unsigned char*)",";
struct _tagged_string _temp2694; _temp2694.curr= _temp2693; _temp2694.base=
_temp2693; _temp2694.last_plus_one= _temp2693 + 2; _temp2694;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_dp2doc, _temp2614)); struct Cyc_PP_Doc* _temp2684[ 3u]={ _temp2686,
_temp2687, _temp2688}; struct _tagged_ptr0 _temp2685={ _temp2684, _temp2684,
_temp2684 + 3u}; _temp2683( _temp2685);}); goto _LL2559; _LL2585: s=({ struct
Cyc_PP_Doc*(* _temp2695)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2698= _temp2626->name == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp2626->name))->v); struct Cyc_PP_Doc*
_temp2699= Cyc_Absynpp_tvars2doc( _temp2622); struct Cyc_PP_Doc* _temp2700= Cyc_PP_group(({
unsigned char* _temp2701=( unsigned char*)"{"; struct _tagged_string _temp2702;
_temp2702.curr= _temp2701; _temp2702.base= _temp2701; _temp2702.last_plus_one=
_temp2701 + 2; _temp2702;}),({ unsigned char* _temp2703=( unsigned char*)"}";
struct _tagged_string _temp2704; _temp2704.curr= _temp2703; _temp2704.base=
_temp2703; _temp2704.last_plus_one= _temp2703 + 2; _temp2704;}),({ unsigned char*
_temp2705=( unsigned char*)","; struct _tagged_string _temp2706; _temp2706.curr=
_temp2705; _temp2706.base= _temp2705; _temp2706.last_plus_one= _temp2705 + 2;
_temp2706;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp2620)); struct Cyc_PP_Doc* _temp2696[ 3u]={ _temp2698, _temp2699, _temp2700};
struct _tagged_ptr0 _temp2697={ _temp2696, _temp2696, _temp2696 + 3u}; _temp2695(
_temp2697);}); goto _LL2559; _LL2587: s= Cyc_Absynpp_qvar2doc( _temp2628->name);
goto _LL2559; _LL2589: if( _temp2632 == 0){ s= Cyc_Absynpp_qvar2doc( _temp2636->name);}
else{ s=({ struct Cyc_PP_Doc*(* _temp2707)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2710= Cyc_Absynpp_qvar2doc( _temp2636->name); struct Cyc_PP_Doc*
_temp2711= Cyc_Absynpp_tvars2doc( _temp2634); struct Cyc_PP_Doc* _temp2712= Cyc_PP_egroup(({
unsigned char* _temp2713=( unsigned char*)"("; struct _tagged_string _temp2714;
_temp2714.curr= _temp2713; _temp2714.base= _temp2713; _temp2714.last_plus_one=
_temp2713 + 2; _temp2714;}),({ unsigned char* _temp2715=( unsigned char*)")";
struct _tagged_string _temp2716; _temp2716.curr= _temp2715; _temp2716.base=
_temp2715; _temp2716.last_plus_one= _temp2715 + 2; _temp2716;}),({ unsigned char*
_temp2717=( unsigned char*)","; struct _tagged_string _temp2718; _temp2718.curr=
_temp2717; _temp2718.base= _temp2717; _temp2718.last_plus_one= _temp2717 + 2;
_temp2718;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc, _temp2632));
struct Cyc_PP_Doc* _temp2708[ 3u]={ _temp2710, _temp2711, _temp2712}; struct
_tagged_ptr0 _temp2709={ _temp2708, _temp2708, _temp2708 + 3u}; _temp2707(
_temp2709);});} goto _LL2559; _LL2559:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_dp2doc(
struct _tuple5* dp){ return({ struct Cyc_PP_Doc*(* _temp2719)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2722= Cyc_PP_egroup(({
unsigned char* _temp2724=( unsigned char*)""; struct _tagged_string _temp2725;
_temp2725.curr= _temp2724; _temp2725.base= _temp2724; _temp2725.last_plus_one=
_temp2724 + 1; _temp2725;}),({ unsigned char* _temp2726=( unsigned char*)"=";
struct _tagged_string _temp2727; _temp2727.curr= _temp2726; _temp2727.base=
_temp2726; _temp2727.last_plus_one= _temp2726 + 2; _temp2727;}),({ unsigned char*
_temp2728=( unsigned char*)"="; struct _tagged_string _temp2729; _temp2729.curr=
_temp2728; _temp2729.base= _temp2728; _temp2729.last_plus_one= _temp2728 + 2;
_temp2729;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(* dp).f1));
struct Cyc_PP_Doc* _temp2723= Cyc_Absynpp_pat2doc((* dp).f2); struct Cyc_PP_Doc*
_temp2720[ 2u]={ _temp2722, _temp2723}; struct _tagged_ptr0 _temp2721={
_temp2720, _temp2720, _temp2720 + 2u}; _temp2719( _temp2721);});} struct Cyc_PP_Doc*
Cyc_Absynpp_switchclause2doc( struct Cyc_Absyn_Switch_clause* c){ if( c->where_clause
== 0?( void*)( c->pattern)->r ==( void*) Cyc_Absyn_Wild_p: 0){ return({ struct
Cyc_PP_Doc*(* _temp2730)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2733= Cyc_PP_text(({ unsigned char* _temp2740=( unsigned char*)"default: ";
struct _tagged_string _temp2741; _temp2741.curr= _temp2740; _temp2741.base=
_temp2740; _temp2741.last_plus_one= _temp2740 + 10; _temp2741;})); struct Cyc_PP_Doc*
_temp2734= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2735)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2738= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2739= Cyc_Absynpp_stmt2doc( c->body); struct Cyc_PP_Doc*
_temp2736[ 2u]={ _temp2738, _temp2739}; struct _tagged_ptr0 _temp2737={
_temp2736, _temp2736, _temp2736 + 2u}; _temp2735( _temp2737);})); struct Cyc_PP_Doc*
_temp2731[ 2u]={ _temp2733, _temp2734}; struct _tagged_ptr0 _temp2732={
_temp2731, _temp2731, _temp2731 + 2u}; _temp2730( _temp2732);});} else{ if( c->where_clause
== 0){ return({ struct Cyc_PP_Doc*(* _temp2742)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2745= Cyc_PP_text(({ unsigned char* _temp2756=(
unsigned char*)"case "; struct _tagged_string _temp2757; _temp2757.curr=
_temp2756; _temp2757.base= _temp2756; _temp2757.last_plus_one= _temp2756 + 6;
_temp2757;})); struct Cyc_PP_Doc* _temp2746= Cyc_Absynpp_pat2doc( c->pattern);
struct Cyc_PP_Doc* _temp2747= Cyc_PP_text(({ unsigned char* _temp2754=(
unsigned char*)": "; struct _tagged_string _temp2755; _temp2755.curr= _temp2754;
_temp2755.base= _temp2754; _temp2755.last_plus_one= _temp2754 + 3; _temp2755;}));
struct Cyc_PP_Doc* _temp2748= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2749)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2752= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2753= Cyc_Absynpp_stmt2doc( c->body); struct Cyc_PP_Doc*
_temp2750[ 2u]={ _temp2752, _temp2753}; struct _tagged_ptr0 _temp2751={
_temp2750, _temp2750, _temp2750 + 2u}; _temp2749( _temp2751);})); struct Cyc_PP_Doc*
_temp2743[ 4u]={ _temp2745, _temp2746, _temp2747, _temp2748}; struct
_tagged_ptr0 _temp2744={ _temp2743, _temp2743, _temp2743 + 4u}; _temp2742(
_temp2744);});} else{ return({ struct Cyc_PP_Doc*(* _temp2758)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2761= Cyc_PP_text(({
unsigned char* _temp2776=( unsigned char*)"case "; struct _tagged_string
_temp2777; _temp2777.curr= _temp2776; _temp2777.base= _temp2776; _temp2777.last_plus_one=
_temp2776 + 6; _temp2777;})); struct Cyc_PP_Doc* _temp2762= Cyc_Absynpp_pat2doc(
c->pattern); struct Cyc_PP_Doc* _temp2763= Cyc_PP_text(({ unsigned char*
_temp2774=( unsigned char*)" && "; struct _tagged_string _temp2775; _temp2775.curr=
_temp2774; _temp2775.base= _temp2774; _temp2775.last_plus_one= _temp2774 + 5;
_temp2775;})); struct Cyc_PP_Doc* _temp2764= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)
_check_null( c->where_clause)); struct Cyc_PP_Doc* _temp2765= Cyc_PP_text(({
unsigned char* _temp2772=( unsigned char*)": "; struct _tagged_string _temp2773;
_temp2773.curr= _temp2772; _temp2773.base= _temp2772; _temp2773.last_plus_one=
_temp2772 + 3; _temp2773;})); struct Cyc_PP_Doc* _temp2766= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc*(* _temp2767)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2770= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2771= Cyc_Absynpp_stmt2doc(
c->body); struct Cyc_PP_Doc* _temp2768[ 2u]={ _temp2770, _temp2771}; struct
_tagged_ptr0 _temp2769={ _temp2768, _temp2768, _temp2768 + 2u}; _temp2767(
_temp2769);})); struct Cyc_PP_Doc* _temp2759[ 6u]={ _temp2761, _temp2762,
_temp2763, _temp2764, _temp2765, _temp2766}; struct _tagged_ptr0 _temp2760={
_temp2759, _temp2759, _temp2759 + 6u}; _temp2758( _temp2760);});}}} struct Cyc_PP_Doc*
Cyc_Absynpp_switchclauses2doc( struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Switch_clause*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc,({
unsigned char* _temp2778=( unsigned char*)""; struct _tagged_string _temp2779;
_temp2779.curr= _temp2778; _temp2779.base= _temp2778; _temp2779.last_plus_one=
_temp2778 + 1; _temp2779;}), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_switchCclause2doc(
struct Cyc_Absyn_SwitchC_clause* c){ struct Cyc_Absyn_SwitchC_clause _temp2782;
struct Cyc_Position_Segment* _temp2783; struct Cyc_Absyn_Stmt* _temp2785; struct
Cyc_Absyn_Exp* _temp2787; struct Cyc_Absyn_SwitchC_clause* _temp2780= c;
_temp2782=* _temp2780; _LL2788: _temp2787= _temp2782.cnst_exp; goto _LL2786;
_LL2786: _temp2785= _temp2782.body; goto _LL2784; _LL2784: _temp2783= _temp2782.loc;
goto _LL2781; _LL2781: if( _temp2787 == 0){ return({ struct Cyc_PP_Doc*(*
_temp2789)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2792= Cyc_PP_text(({
unsigned char* _temp2799=( unsigned char*)"default: "; struct _tagged_string
_temp2800; _temp2800.curr= _temp2799; _temp2800.base= _temp2799; _temp2800.last_plus_one=
_temp2799 + 10; _temp2800;})); struct Cyc_PP_Doc* _temp2793= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc*(* _temp2794)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2797= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2798= Cyc_Absynpp_stmt2doc(
c->body); struct Cyc_PP_Doc* _temp2795[ 2u]={ _temp2797, _temp2798}; struct
_tagged_ptr0 _temp2796={ _temp2795, _temp2795, _temp2795 + 2u}; _temp2794(
_temp2796);})); struct Cyc_PP_Doc* _temp2790[ 2u]={ _temp2792, _temp2793};
struct _tagged_ptr0 _temp2791={ _temp2790, _temp2790, _temp2790 + 2u}; _temp2789(
_temp2791);});} else{ return({ struct Cyc_PP_Doc*(* _temp2801)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2804= Cyc_PP_text(({
unsigned char* _temp2815=( unsigned char*)"case "; struct _tagged_string
_temp2816; _temp2816.curr= _temp2815; _temp2816.base= _temp2815; _temp2816.last_plus_one=
_temp2815 + 6; _temp2816;})); struct Cyc_PP_Doc* _temp2805= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp2787)); struct Cyc_PP_Doc* _temp2806=
Cyc_PP_text(({ unsigned char* _temp2813=( unsigned char*)": "; struct
_tagged_string _temp2814; _temp2814.curr= _temp2813; _temp2814.base= _temp2813;
_temp2814.last_plus_one= _temp2813 + 3; _temp2814;})); struct Cyc_PP_Doc*
_temp2807= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2808)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2811= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2812= Cyc_Absynpp_stmt2doc( _temp2785); struct Cyc_PP_Doc*
_temp2809[ 2u]={ _temp2811, _temp2812}; struct _tagged_ptr0 _temp2810={
_temp2809, _temp2809, _temp2809 + 2u}; _temp2808( _temp2810);})); struct Cyc_PP_Doc*
_temp2802[ 4u]={ _temp2804, _temp2805, _temp2806, _temp2807}; struct
_tagged_ptr0 _temp2803={ _temp2802, _temp2802, _temp2802 + 4u}; _temp2801(
_temp2803);});}} struct Cyc_PP_Doc* Cyc_Absynpp_switchCclauses2doc( struct Cyc_List_List*
cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_SwitchC_clause*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchCclause2doc,({
unsigned char* _temp2817=( unsigned char*)""; struct _tagged_string _temp2818;
_temp2818.curr= _temp2817; _temp2818.base= _temp2817; _temp2818.last_plus_one=
_temp2817 + 1; _temp2818;}), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_enumfield2doc(
struct Cyc_Absyn_Enumfield* f){ if( f->tag == 0){ return Cyc_Absynpp_qvar2doc( f->name);}
else{ return({ struct Cyc_PP_Doc*(* _temp2819)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2822= Cyc_Absynpp_qvar2doc( f->name); struct Cyc_PP_Doc*
_temp2823= Cyc_PP_text(({ unsigned char* _temp2825=( unsigned char*)" = ";
struct _tagged_string _temp2826; _temp2826.curr= _temp2825; _temp2826.base=
_temp2825; _temp2826.last_plus_one= _temp2825 + 4; _temp2826;})); struct Cyc_PP_Doc*
_temp2824= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( f->tag));
struct Cyc_PP_Doc* _temp2820[ 3u]={ _temp2822, _temp2823, _temp2824}; struct
_tagged_ptr0 _temp2821={ _temp2820, _temp2820, _temp2820 + 3u}; _temp2819(
_temp2821);});}} struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc( struct Cyc_List_List*
fs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Enumfield*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc,({
unsigned char* _temp2827=( unsigned char*)","; struct _tagged_string _temp2828;
_temp2828.curr= _temp2827; _temp2828.base= _temp2827; _temp2828.last_plus_one=
_temp2827 + 2; _temp2828;}), fs);} static struct Cyc_PP_Doc* Cyc_Absynpp_id2doc(
struct Cyc_Absyn_Vardecl* v){ return Cyc_Absynpp_qvar2doc( v->name);} static
struct Cyc_PP_Doc* Cyc_Absynpp_ids2doc( struct Cyc_List_List* vds){ return((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Vardecl*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseq)( Cyc_Absynpp_id2doc,({
unsigned char* _temp2829=( unsigned char*)","; struct _tagged_string _temp2830;
_temp2830.curr= _temp2829; _temp2830.base= _temp2829; _temp2830.last_plus_one=
_temp2829 + 2; _temp2830;}), vds);} struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl* d){ struct Cyc_PP_Doc* s;{ void* _temp2831=( void*) d->r;
struct Cyc_Absyn_Fndecl* _temp2857; struct Cyc_Absyn_Structdecl* _temp2859;
struct Cyc_Absyn_Uniondecl* _temp2861; struct Cyc_Absyn_Vardecl* _temp2863;
struct Cyc_Absyn_Vardecl _temp2865; struct Cyc_List_List* _temp2866; struct Cyc_Core_Opt*
_temp2868; struct Cyc_Absyn_Exp* _temp2870; void* _temp2872; struct Cyc_Absyn_Tqual
_temp2874; struct _tuple0* _temp2876; void* _temp2878; struct Cyc_Absyn_Tuniondecl*
_temp2880; struct Cyc_Absyn_Tuniondecl _temp2882; int _temp2883; struct Cyc_Core_Opt*
_temp2885; struct Cyc_List_List* _temp2887; struct _tuple0* _temp2889; void*
_temp2891; int _temp2893; struct Cyc_Absyn_Exp* _temp2895; struct Cyc_Core_Opt*
_temp2897; struct Cyc_Core_Opt* _temp2899; struct Cyc_Absyn_Pat* _temp2901;
struct Cyc_List_List* _temp2903; struct Cyc_Absyn_Enumdecl* _temp2905; struct
Cyc_Absyn_Enumdecl _temp2907; struct Cyc_Core_Opt* _temp2908; struct _tuple0*
_temp2910; void* _temp2912; struct Cyc_Absyn_Typedefdecl* _temp2914; struct Cyc_List_List*
_temp2916; struct _tagged_string* _temp2918; struct Cyc_List_List* _temp2920;
struct _tuple0* _temp2922; struct Cyc_List_List* _temp2924; _LL2833: if(*(( int*)
_temp2831) == Cyc_Absyn_Fn_d){ _LL2858: _temp2857=(( struct Cyc_Absyn_Fn_d_struct*)
_temp2831)->f1; goto _LL2834;} else{ goto _LL2835;} _LL2835: if(*(( int*)
_temp2831) == Cyc_Absyn_Struct_d){ _LL2860: _temp2859=(( struct Cyc_Absyn_Struct_d_struct*)
_temp2831)->f1; goto _LL2836;} else{ goto _LL2837;} _LL2837: if(*(( int*)
_temp2831) == Cyc_Absyn_Union_d){ _LL2862: _temp2861=(( struct Cyc_Absyn_Union_d_struct*)
_temp2831)->f1; goto _LL2838;} else{ goto _LL2839;} _LL2839: if(*(( int*)
_temp2831) == Cyc_Absyn_Var_d){ _LL2864: _temp2863=(( struct Cyc_Absyn_Var_d_struct*)
_temp2831)->f1; _temp2865=* _temp2863; _LL2879: _temp2878=( void*) _temp2865.sc;
goto _LL2877; _LL2877: _temp2876= _temp2865.name; goto _LL2875; _LL2875:
_temp2874= _temp2865.tq; goto _LL2873; _LL2873: _temp2872=( void*) _temp2865.type;
goto _LL2871; _LL2871: _temp2870= _temp2865.initializer; goto _LL2869; _LL2869:
_temp2868= _temp2865.rgn; goto _LL2867; _LL2867: _temp2866= _temp2865.attributes;
goto _LL2840;} else{ goto _LL2841;} _LL2841: if(*(( int*) _temp2831) == Cyc_Absyn_Tunion_d){
_LL2881: _temp2880=(( struct Cyc_Absyn_Tunion_d_struct*) _temp2831)->f1;
_temp2882=* _temp2880; _LL2892: _temp2891=( void*) _temp2882.sc; goto _LL2890;
_LL2890: _temp2889= _temp2882.name; goto _LL2888; _LL2888: _temp2887= _temp2882.tvs;
goto _LL2886; _LL2886: _temp2885= _temp2882.fields; goto _LL2884; _LL2884:
_temp2883= _temp2882.is_xtunion; goto _LL2842;} else{ goto _LL2843;} _LL2843:
if(*(( int*) _temp2831) == Cyc_Absyn_Let_d){ _LL2902: _temp2901=(( struct Cyc_Absyn_Let_d_struct*)
_temp2831)->f1; goto _LL2900; _LL2900: _temp2899=(( struct Cyc_Absyn_Let_d_struct*)
_temp2831)->f2; goto _LL2898; _LL2898: _temp2897=(( struct Cyc_Absyn_Let_d_struct*)
_temp2831)->f3; goto _LL2896; _LL2896: _temp2895=(( struct Cyc_Absyn_Let_d_struct*)
_temp2831)->f4; goto _LL2894; _LL2894: _temp2893=(( struct Cyc_Absyn_Let_d_struct*)
_temp2831)->f5; goto _LL2844;} else{ goto _LL2845;} _LL2845: if(*(( int*)
_temp2831) == Cyc_Absyn_Letv_d){ _LL2904: _temp2903=(( struct Cyc_Absyn_Letv_d_struct*)
_temp2831)->f1; goto _LL2846;} else{ goto _LL2847;} _LL2847: if(*(( int*)
_temp2831) == Cyc_Absyn_Enum_d){ _LL2906: _temp2905=(( struct Cyc_Absyn_Enum_d_struct*)
_temp2831)->f1; _temp2907=* _temp2905; _LL2913: _temp2912=( void*) _temp2907.sc;
goto _LL2911; _LL2911: _temp2910= _temp2907.name; goto _LL2909; _LL2909:
_temp2908= _temp2907.fields; goto _LL2848;} else{ goto _LL2849;} _LL2849: if(*((
int*) _temp2831) == Cyc_Absyn_Typedef_d){ _LL2915: _temp2914=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2831)->f1; goto _LL2850;} else{ goto _LL2851;} _LL2851: if(*(( int*)
_temp2831) == Cyc_Absyn_Namespace_d){ _LL2919: _temp2918=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2831)->f1; goto _LL2917; _LL2917: _temp2916=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2831)->f2; goto _LL2852;} else{ goto _LL2853;} _LL2853: if(*(( int*)
_temp2831) == Cyc_Absyn_Using_d){ _LL2923: _temp2922=(( struct Cyc_Absyn_Using_d_struct*)
_temp2831)->f1; goto _LL2921; _LL2921: _temp2920=(( struct Cyc_Absyn_Using_d_struct*)
_temp2831)->f2; goto _LL2854;} else{ goto _LL2855;} _LL2855: if(*(( int*)
_temp2831) == Cyc_Absyn_ExternC_d){ _LL2925: _temp2924=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2831)->f1; goto _LL2856;} else{ goto _LL2832;} _LL2834: { void* t=( void*)({
struct Cyc_Absyn_FnType_struct* _temp2965=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2965[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2966; _temp2966.tag= Cyc_Absyn_FnType; _temp2966.f1=({ struct Cyc_Absyn_FnInfo
_temp2967; _temp2967.tvars= _temp2857->tvs; _temp2967.effect= _temp2857->effect;
_temp2967.ret_typ=( void*)(( void*) _temp2857->ret_type); _temp2967.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp2857->args); _temp2967.c_varargs=
_temp2857->c_varargs; _temp2967.cyc_varargs= _temp2857->cyc_varargs; _temp2967.attributes=
0; _temp2967;}); _temp2966;}); _temp2965;}); struct Cyc_PP_Doc* attsdoc= Cyc_Absynpp_atts2doc(
_temp2857->attributes); struct Cyc_PP_Doc* inlinedoc; if( _temp2857->is_inline){
if( Cyc_Absynpp_to_VC){ inlinedoc= Cyc_PP_text(({ unsigned char* _temp2926=(
unsigned char*)"__inline "; struct _tagged_string _temp2927; _temp2927.curr=
_temp2926; _temp2927.base= _temp2926; _temp2927.last_plus_one= _temp2926 + 10;
_temp2927;}));} else{ inlinedoc= Cyc_PP_text(({ unsigned char* _temp2928=(
unsigned char*)"inline "; struct _tagged_string _temp2929; _temp2929.curr=
_temp2928; _temp2929.base= _temp2928; _temp2929.last_plus_one= _temp2928 + 8;
_temp2929;}));}} else{ inlinedoc= Cyc_PP_nil_doc();}{ struct Cyc_PP_Doc*
scopedoc= Cyc_Absynpp_scope2doc(( void*) _temp2857->sc); struct Cyc_PP_Doc*
beforenamedoc= Cyc_Absynpp_to_VC? Cyc_Absynpp_callconv2doc( _temp2857->attributes):
attsdoc; struct Cyc_PP_Doc* namedoc= Cyc_Absynpp_typedef_name2doc( _temp2857->name);
struct Cyc_PP_Doc* tqtddoc= Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual
_temp2958; _temp2958.q_const= 0; _temp2958.q_volatile= 0; _temp2958.q_restrict=
0; _temp2958;}), t,({ struct Cyc_Core_Opt* _temp2959=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2959->v=( void*)({ struct Cyc_PP_Doc*(*
_temp2960)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2963=
beforenamedoc; struct Cyc_PP_Doc* _temp2964= namedoc; struct Cyc_PP_Doc*
_temp2961[ 2u]={ _temp2963, _temp2964}; struct _tagged_ptr0 _temp2962={
_temp2961, _temp2961, _temp2961 + 2u}; _temp2960( _temp2962);}); _temp2959;}));
struct Cyc_PP_Doc* bodydoc=({ struct Cyc_PP_Doc*(* _temp2942)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2945= Cyc_PP_text(({
unsigned char* _temp2956=( unsigned char*)" {"; struct _tagged_string _temp2957;
_temp2957.curr= _temp2956; _temp2957.base= _temp2956; _temp2957.last_plus_one=
_temp2956 + 3; _temp2957;})); struct Cyc_PP_Doc* _temp2946= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc*(* _temp2951)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2954= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2955= Cyc_Absynpp_stmt2doc(
_temp2857->body); struct Cyc_PP_Doc* _temp2952[ 2u]={ _temp2954, _temp2955};
struct _tagged_ptr0 _temp2953={ _temp2952, _temp2952, _temp2952 + 2u}; _temp2951(
_temp2953);})); struct Cyc_PP_Doc* _temp2947= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2948= Cyc_PP_text(({ unsigned char* _temp2949=( unsigned char*)"}"; struct
_tagged_string _temp2950; _temp2950.curr= _temp2949; _temp2950.base= _temp2949;
_temp2950.last_plus_one= _temp2949 + 2; _temp2950;})); struct Cyc_PP_Doc*
_temp2943[ 4u]={ _temp2945, _temp2946, _temp2947, _temp2948}; struct
_tagged_ptr0 _temp2944={ _temp2943, _temp2943, _temp2943 + 4u}; _temp2942(
_temp2944);}); s=({ struct Cyc_PP_Doc*(* _temp2930)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2933= inlinedoc; struct Cyc_PP_Doc* _temp2934= scopedoc;
struct Cyc_PP_Doc* _temp2935= tqtddoc; struct Cyc_PP_Doc* _temp2936= bodydoc;
struct Cyc_PP_Doc* _temp2931[ 4u]={ _temp2933, _temp2934, _temp2935, _temp2936};
struct _tagged_ptr0 _temp2932={ _temp2931, _temp2931, _temp2931 + 4u}; _temp2930(
_temp2932);}); if( Cyc_Absynpp_to_VC){ s=({ struct Cyc_PP_Doc*(* _temp2937)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2940= attsdoc; struct
Cyc_PP_Doc* _temp2941= s; struct Cyc_PP_Doc* _temp2938[ 2u]={ _temp2940,
_temp2941}; struct _tagged_ptr0 _temp2939={ _temp2938, _temp2938, _temp2938 + 2u};
_temp2937( _temp2939);});} goto _LL2832;}} _LL2836: if( _temp2859->fields == 0){
s=({ struct Cyc_PP_Doc*(* _temp2968)( struct _tagged_ptr0)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp2971= Cyc_Absynpp_scope2doc(( void*) _temp2859->sc); struct Cyc_PP_Doc*
_temp2972= Cyc_PP_text(({ unsigned char* _temp2980=( unsigned char*)"struct ";
struct _tagged_string _temp2981; _temp2981.curr= _temp2980; _temp2981.base=
_temp2980; _temp2981.last_plus_one= _temp2980 + 8; _temp2981;})); struct Cyc_PP_Doc*
_temp2973= _temp2859->name == 0? Cyc_PP_text(({ unsigned char* _temp2978=(
unsigned char*)""; struct _tagged_string _temp2979; _temp2979.curr= _temp2978;
_temp2979.base= _temp2978; _temp2979.last_plus_one= _temp2978 + 1; _temp2979;})):
Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp2859->name))->v); struct Cyc_PP_Doc* _temp2974= Cyc_Absynpp_ktvars2doc(
_temp2859->tvs); struct Cyc_PP_Doc* _temp2975= Cyc_PP_text(({ unsigned char*
_temp2976=( unsigned char*)";"; struct _tagged_string _temp2977; _temp2977.curr=
_temp2976; _temp2977.base= _temp2976; _temp2977.last_plus_one= _temp2976 + 2;
_temp2977;})); struct Cyc_PP_Doc* _temp2969[ 5u]={ _temp2971, _temp2972,
_temp2973, _temp2974, _temp2975}; struct _tagged_ptr0 _temp2970={ _temp2969,
_temp2969, _temp2969 + 5u}; _temp2968( _temp2970);});} else{ s=({ struct Cyc_PP_Doc*(*
_temp2982)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2985= Cyc_Absynpp_scope2doc((
void*) _temp2859->sc); struct Cyc_PP_Doc* _temp2986= Cyc_PP_text(({
unsigned char* _temp3008=( unsigned char*)"struct "; struct _tagged_string
_temp3009; _temp3009.curr= _temp3008; _temp3009.base= _temp3008; _temp3009.last_plus_one=
_temp3008 + 8; _temp3009;})); struct Cyc_PP_Doc* _temp2987= _temp2859->name == 0?
Cyc_PP_text(({ unsigned char* _temp3006=( unsigned char*)""; struct
_tagged_string _temp3007; _temp3007.curr= _temp3006; _temp3007.base= _temp3006;
_temp3007.last_plus_one= _temp3006 + 1; _temp3007;})): Cyc_Absynpp_typedef_name2doc((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp2859->name))->v);
struct Cyc_PP_Doc* _temp2988= Cyc_Absynpp_ktvars2doc( _temp2859->tvs); struct
Cyc_PP_Doc* _temp2989= Cyc_PP_text(({ unsigned char* _temp3004=( unsigned char*)" {";
struct _tagged_string _temp3005; _temp3005.curr= _temp3004; _temp3005.base=
_temp3004; _temp3005.last_plus_one= _temp3004 + 3; _temp3005;})); struct Cyc_PP_Doc*
_temp2990= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2999)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3002= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp3003= Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp2859->fields))->v); struct Cyc_PP_Doc*
_temp3000[ 2u]={ _temp3002, _temp3003}; struct _tagged_ptr0 _temp3001={
_temp3000, _temp3000, _temp3000 + 2u}; _temp2999( _temp3001);})); struct Cyc_PP_Doc*
_temp2991= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2992= Cyc_PP_text(({
unsigned char* _temp2997=( unsigned char*)"}"; struct _tagged_string _temp2998;
_temp2998.curr= _temp2997; _temp2998.base= _temp2997; _temp2998.last_plus_one=
_temp2997 + 2; _temp2998;})); struct Cyc_PP_Doc* _temp2993= Cyc_Absynpp_atts2doc(
_temp2859->attributes); struct Cyc_PP_Doc* _temp2994= Cyc_PP_text(({
unsigned char* _temp2995=( unsigned char*)";"; struct _tagged_string _temp2996;
_temp2996.curr= _temp2995; _temp2996.base= _temp2995; _temp2996.last_plus_one=
_temp2995 + 2; _temp2996;})); struct Cyc_PP_Doc* _temp2983[ 10u]={ _temp2985,
_temp2986, _temp2987, _temp2988, _temp2989, _temp2990, _temp2991, _temp2992,
_temp2993, _temp2994}; struct _tagged_ptr0 _temp2984={ _temp2983, _temp2983,
_temp2983 + 10u}; _temp2982( _temp2984);});} goto _LL2832; _LL2838: if(
_temp2861->fields == 0){ s=({ struct Cyc_PP_Doc*(* _temp3010)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3013= Cyc_Absynpp_scope2doc((
void*) _temp2861->sc); struct Cyc_PP_Doc* _temp3014= Cyc_PP_text(({
unsigned char* _temp3022=( unsigned char*)"union "; struct _tagged_string
_temp3023; _temp3023.curr= _temp3022; _temp3023.base= _temp3022; _temp3023.last_plus_one=
_temp3022 + 7; _temp3023;})); struct Cyc_PP_Doc* _temp3015= _temp2861->name == 0?
Cyc_PP_text(({ unsigned char* _temp3020=( unsigned char*)""; struct
_tagged_string _temp3021; _temp3021.curr= _temp3020; _temp3021.base= _temp3020;
_temp3021.last_plus_one= _temp3020 + 1; _temp3021;})): Cyc_Absynpp_qvar2doc((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp2861->name))->v);
struct Cyc_PP_Doc* _temp3016= Cyc_Absynpp_tvars2doc( _temp2861->tvs); struct Cyc_PP_Doc*
_temp3017= Cyc_PP_text(({ unsigned char* _temp3018=( unsigned char*)";"; struct
_tagged_string _temp3019; _temp3019.curr= _temp3018; _temp3019.base= _temp3018;
_temp3019.last_plus_one= _temp3018 + 2; _temp3019;})); struct Cyc_PP_Doc*
_temp3011[ 5u]={ _temp3013, _temp3014, _temp3015, _temp3016, _temp3017}; struct
_tagged_ptr0 _temp3012={ _temp3011, _temp3011, _temp3011 + 5u}; _temp3010(
_temp3012);});} else{ s=({ struct Cyc_PP_Doc*(* _temp3024)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp3027= Cyc_Absynpp_scope2doc(( void*)
_temp2861->sc); struct Cyc_PP_Doc* _temp3028= Cyc_PP_text(({ unsigned char*
_temp3050=( unsigned char*)"union "; struct _tagged_string _temp3051; _temp3051.curr=
_temp3050; _temp3051.base= _temp3050; _temp3051.last_plus_one= _temp3050 + 7;
_temp3051;})); struct Cyc_PP_Doc* _temp3029= _temp2861->name == 0? Cyc_PP_text(({
unsigned char* _temp3048=( unsigned char*)""; struct _tagged_string _temp3049;
_temp3049.curr= _temp3048; _temp3049.base= _temp3048; _temp3049.last_plus_one=
_temp3048 + 1; _temp3049;})): Cyc_Absynpp_qvar2doc(( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp2861->name))->v); struct Cyc_PP_Doc* _temp3030=
Cyc_Absynpp_tvars2doc( _temp2861->tvs); struct Cyc_PP_Doc* _temp3031= Cyc_PP_text(({
unsigned char* _temp3046=( unsigned char*)" {"; struct _tagged_string _temp3047;
_temp3047.curr= _temp3046; _temp3047.base= _temp3046; _temp3047.last_plus_one=
_temp3046 + 3; _temp3047;})); struct Cyc_PP_Doc* _temp3032= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc*(* _temp3041)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp3044= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3045= Cyc_Absynpp_structfields2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp2861->fields))->v);
struct Cyc_PP_Doc* _temp3042[ 2u]={ _temp3044, _temp3045}; struct _tagged_ptr0
_temp3043={ _temp3042, _temp3042, _temp3042 + 2u}; _temp3041( _temp3043);}));
struct Cyc_PP_Doc* _temp3033= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3034=
Cyc_PP_text(({ unsigned char* _temp3039=( unsigned char*)"}"; struct
_tagged_string _temp3040; _temp3040.curr= _temp3039; _temp3040.base= _temp3039;
_temp3040.last_plus_one= _temp3039 + 2; _temp3040;})); struct Cyc_PP_Doc*
_temp3035= Cyc_Absynpp_atts2doc( _temp2861->attributes); struct Cyc_PP_Doc*
_temp3036= Cyc_PP_text(({ unsigned char* _temp3037=( unsigned char*)";"; struct
_tagged_string _temp3038; _temp3038.curr= _temp3037; _temp3038.base= _temp3037;
_temp3038.last_plus_one= _temp3037 + 2; _temp3038;})); struct Cyc_PP_Doc*
_temp3025[ 10u]={ _temp3027, _temp3028, _temp3029, _temp3030, _temp3031,
_temp3032, _temp3033, _temp3034, _temp3035, _temp3036}; struct _tagged_ptr0
_temp3026={ _temp3025, _temp3025, _temp3025 + 10u}; _temp3024( _temp3026);});}
goto _LL2832; _LL2840: { struct Cyc_PP_Doc* sn= Cyc_Absynpp_typedef_name2doc(
_temp2876); s=({ struct Cyc_PP_Doc*(* _temp3052)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp3055= Cyc_Absynpp_to_VC? Cyc_Absynpp_atts2doc( _temp2866):
Cyc_PP_nil_doc(); struct Cyc_PP_Doc* _temp3056= Cyc_Absynpp_scope2doc( _temp2878);
struct Cyc_PP_Doc* _temp3057= Cyc_Absynpp_tqtd2doc( _temp2874, _temp2872,({
struct Cyc_Core_Opt* _temp3072=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3072->v=( void*) sn; _temp3072;})); struct Cyc_PP_Doc*
_temp3058= ! Cyc_Absynpp_to_VC? Cyc_Absynpp_atts2doc( _temp2866): Cyc_PP_nil_doc();
struct Cyc_PP_Doc* _temp3059= _temp2870 == 0? Cyc_PP_text(({ unsigned char*
_temp3063=( unsigned char*)""; struct _tagged_string _temp3064; _temp3064.curr=
_temp3063; _temp3064.base= _temp3063; _temp3064.last_plus_one= _temp3063 + 1;
_temp3064;})):({ struct Cyc_PP_Doc*(* _temp3065)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp3068= Cyc_PP_text(({ unsigned char* _temp3070=(
unsigned char*)" = "; struct _tagged_string _temp3071; _temp3071.curr= _temp3070;
_temp3071.base= _temp3070; _temp3071.last_plus_one= _temp3070 + 4; _temp3071;}));
struct Cyc_PP_Doc* _temp3069= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)
_check_null( _temp2870)); struct Cyc_PP_Doc* _temp3066[ 2u]={ _temp3068,
_temp3069}; struct _tagged_ptr0 _temp3067={ _temp3066, _temp3066, _temp3066 + 2u};
_temp3065( _temp3067);}); struct Cyc_PP_Doc* _temp3060= Cyc_PP_text(({
unsigned char* _temp3061=( unsigned char*)";"; struct _tagged_string _temp3062;
_temp3062.curr= _temp3061; _temp3062.base= _temp3061; _temp3062.last_plus_one=
_temp3061 + 2; _temp3062;})); struct Cyc_PP_Doc* _temp3053[ 6u]={ _temp3055,
_temp3056, _temp3057, _temp3058, _temp3059, _temp3060}; struct _tagged_ptr0
_temp3054={ _temp3053, _temp3053, _temp3053 + 6u}; _temp3052( _temp3054);});
goto _LL2832;} _LL2842: if( _temp2885 == 0){ s=({ struct Cyc_PP_Doc*(* _temp3073)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3076= Cyc_Absynpp_scope2doc(
_temp2891); struct Cyc_PP_Doc* _temp3077= _temp2883? Cyc_PP_text(({
unsigned char* _temp3083=( unsigned char*)"xtunion "; struct _tagged_string
_temp3084; _temp3084.curr= _temp3083; _temp3084.base= _temp3083; _temp3084.last_plus_one=
_temp3083 + 9; _temp3084;})): Cyc_PP_text(({ unsigned char* _temp3085=(
unsigned char*)"tunion "; struct _tagged_string _temp3086; _temp3086.curr=
_temp3085; _temp3086.base= _temp3085; _temp3086.last_plus_one= _temp3085 + 8;
_temp3086;})); struct Cyc_PP_Doc* _temp3078= _temp2883? Cyc_Absynpp_qvar2doc(
_temp2889): Cyc_Absynpp_typedef_name2doc( _temp2889); struct Cyc_PP_Doc*
_temp3079= Cyc_Absynpp_ktvars2doc( _temp2887); struct Cyc_PP_Doc* _temp3080= Cyc_PP_text(({
unsigned char* _temp3081=( unsigned char*)";"; struct _tagged_string _temp3082;
_temp3082.curr= _temp3081; _temp3082.base= _temp3081; _temp3082.last_plus_one=
_temp3081 + 2; _temp3082;})); struct Cyc_PP_Doc* _temp3074[ 5u]={ _temp3076,
_temp3077, _temp3078, _temp3079, _temp3080}; struct _tagged_ptr0 _temp3075={
_temp3074, _temp3074, _temp3074 + 5u}; _temp3073( _temp3075);});} else{ s=({
struct Cyc_PP_Doc*(* _temp3087)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp3090= Cyc_Absynpp_scope2doc( _temp2891); struct Cyc_PP_Doc* _temp3091=
_temp2883? Cyc_PP_text(({ unsigned char* _temp3107=( unsigned char*)"xtunion ";
struct _tagged_string _temp3108; _temp3108.curr= _temp3107; _temp3108.base=
_temp3107; _temp3108.last_plus_one= _temp3107 + 9; _temp3108;})): Cyc_PP_text(({
unsigned char* _temp3109=( unsigned char*)"tunion "; struct _tagged_string
_temp3110; _temp3110.curr= _temp3109; _temp3110.base= _temp3109; _temp3110.last_plus_one=
_temp3109 + 8; _temp3110;})); struct Cyc_PP_Doc* _temp3092= _temp2883? Cyc_Absynpp_qvar2doc(
_temp2889): Cyc_Absynpp_typedef_name2doc( _temp2889); struct Cyc_PP_Doc*
_temp3093= Cyc_Absynpp_ktvars2doc( _temp2887); struct Cyc_PP_Doc* _temp3094= Cyc_PP_text(({
unsigned char* _temp3105=( unsigned char*)" {"; struct _tagged_string _temp3106;
_temp3106.curr= _temp3105; _temp3106.base= _temp3105; _temp3106.last_plus_one=
_temp3105 + 3; _temp3106;})); struct Cyc_PP_Doc* _temp3095= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc*(* _temp3100)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp3103= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3104= Cyc_Absynpp_tunionfields2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp2885))->v);
struct Cyc_PP_Doc* _temp3101[ 2u]={ _temp3103, _temp3104}; struct _tagged_ptr0
_temp3102={ _temp3101, _temp3101, _temp3101 + 2u}; _temp3100( _temp3102);}));
struct Cyc_PP_Doc* _temp3096= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3097=
Cyc_PP_text(({ unsigned char* _temp3098=( unsigned char*)"};"; struct
_tagged_string _temp3099; _temp3099.curr= _temp3098; _temp3099.base= _temp3098;
_temp3099.last_plus_one= _temp3098 + 3; _temp3099;})); struct Cyc_PP_Doc*
_temp3088[ 8u]={ _temp3090, _temp3091, _temp3092, _temp3093, _temp3094,
_temp3095, _temp3096, _temp3097}; struct _tagged_ptr0 _temp3089={ _temp3088,
_temp3088, _temp3088 + 8u}; _temp3087( _temp3089);});} goto _LL2832; _LL2844: s=({
struct Cyc_PP_Doc*(* _temp3111)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp3114= Cyc_PP_text(({ unsigned char* _temp3123=( unsigned char*)"let ";
struct _tagged_string _temp3124; _temp3124.curr= _temp3123; _temp3124.base=
_temp3123; _temp3124.last_plus_one= _temp3123 + 5; _temp3124;})); struct Cyc_PP_Doc*
_temp3115= Cyc_Absynpp_pat2doc( _temp2901); struct Cyc_PP_Doc* _temp3116= Cyc_PP_text(({
unsigned char* _temp3121=( unsigned char*)" = "; struct _tagged_string _temp3122;
_temp3122.curr= _temp3121; _temp3122.base= _temp3121; _temp3122.last_plus_one=
_temp3121 + 4; _temp3122;})); struct Cyc_PP_Doc* _temp3117= Cyc_Absynpp_exp2doc(
_temp2895); struct Cyc_PP_Doc* _temp3118= Cyc_PP_text(({ unsigned char*
_temp3119=( unsigned char*)";"; struct _tagged_string _temp3120; _temp3120.curr=
_temp3119; _temp3120.base= _temp3119; _temp3120.last_plus_one= _temp3119 + 2;
_temp3120;})); struct Cyc_PP_Doc* _temp3112[ 5u]={ _temp3114, _temp3115,
_temp3116, _temp3117, _temp3118}; struct _tagged_ptr0 _temp3113={ _temp3112,
_temp3112, _temp3112 + 5u}; _temp3111( _temp3113);}); goto _LL2832; _LL2846: s=({
struct Cyc_PP_Doc*(* _temp3125)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp3128= Cyc_PP_text(({ unsigned char* _temp3133=( unsigned char*)"let ";
struct _tagged_string _temp3134; _temp3134.curr= _temp3133; _temp3134.base=
_temp3133; _temp3134.last_plus_one= _temp3133 + 5; _temp3134;})); struct Cyc_PP_Doc*
_temp3129= Cyc_Absynpp_ids2doc( _temp2903); struct Cyc_PP_Doc* _temp3130= Cyc_PP_text(({
unsigned char* _temp3131=( unsigned char*)";"; struct _tagged_string _temp3132;
_temp3132.curr= _temp3131; _temp3132.base= _temp3131; _temp3132.last_plus_one=
_temp3131 + 2; _temp3132;})); struct Cyc_PP_Doc* _temp3126[ 3u]={ _temp3128,
_temp3129, _temp3130}; struct _tagged_ptr0 _temp3127={ _temp3126, _temp3126,
_temp3126 + 3u}; _temp3125( _temp3127);}); goto _LL2832; _LL2848: if( _temp2908
== 0){ s=({ struct Cyc_PP_Doc*(* _temp3135)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp3138= Cyc_Absynpp_scope2doc( _temp2912); struct Cyc_PP_Doc*
_temp3139= Cyc_PP_text(({ unsigned char* _temp3144=( unsigned char*)"enum ";
struct _tagged_string _temp3145; _temp3145.curr= _temp3144; _temp3145.base=
_temp3144; _temp3145.last_plus_one= _temp3144 + 6; _temp3145;})); struct Cyc_PP_Doc*
_temp3140= Cyc_Absynpp_typedef_name2doc( _temp2910); struct Cyc_PP_Doc*
_temp3141= Cyc_PP_text(({ unsigned char* _temp3142=( unsigned char*)";"; struct
_tagged_string _temp3143; _temp3143.curr= _temp3142; _temp3143.base= _temp3142;
_temp3143.last_plus_one= _temp3142 + 2; _temp3143;})); struct Cyc_PP_Doc*
_temp3136[ 4u]={ _temp3138, _temp3139, _temp3140, _temp3141}; struct
_tagged_ptr0 _temp3137={ _temp3136, _temp3136, _temp3136 + 4u}; _temp3135(
_temp3137);});} else{ s=({ struct Cyc_PP_Doc*(* _temp3146)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp3149= Cyc_Absynpp_scope2doc( _temp2912);
struct Cyc_PP_Doc* _temp3150= Cyc_PP_text(({ unsigned char* _temp3165=(
unsigned char*)"enum "; struct _tagged_string _temp3166; _temp3166.curr=
_temp3165; _temp3166.base= _temp3165; _temp3166.last_plus_one= _temp3165 + 6;
_temp3166;})); struct Cyc_PP_Doc* _temp3151= Cyc_Absynpp_qvar2doc( _temp2910);
struct Cyc_PP_Doc* _temp3152= Cyc_PP_text(({ unsigned char* _temp3163=(
unsigned char*)" {"; struct _tagged_string _temp3164; _temp3164.curr= _temp3163;
_temp3164.base= _temp3163; _temp3164.last_plus_one= _temp3163 + 3; _temp3164;}));
struct Cyc_PP_Doc* _temp3153= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp3158)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3161= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp3162= Cyc_Absynpp_enumfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp2908))->v); struct Cyc_PP_Doc* _temp3159[
2u]={ _temp3161, _temp3162}; struct _tagged_ptr0 _temp3160={ _temp3159,
_temp3159, _temp3159 + 2u}; _temp3158( _temp3160);})); struct Cyc_PP_Doc*
_temp3154= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3155= Cyc_PP_text(({
unsigned char* _temp3156=( unsigned char*)"};"; struct _tagged_string _temp3157;
_temp3157.curr= _temp3156; _temp3157.base= _temp3156; _temp3157.last_plus_one=
_temp3156 + 3; _temp3157;})); struct Cyc_PP_Doc* _temp3147[ 7u]={ _temp3149,
_temp3150, _temp3151, _temp3152, _temp3153, _temp3154, _temp3155}; struct
_tagged_ptr0 _temp3148={ _temp3147, _temp3147, _temp3147 + 7u}; _temp3146(
_temp3148);});} goto _LL2832; _LL2850: s=({ struct Cyc_PP_Doc*(* _temp3167)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3170= Cyc_PP_text(({
unsigned char* _temp3182=( unsigned char*)"typedef "; struct _tagged_string
_temp3183; _temp3183.curr= _temp3182; _temp3183.base= _temp3182; _temp3183.last_plus_one=
_temp3182 + 9; _temp3183;})); struct Cyc_PP_Doc* _temp3171= Cyc_Absynpp_tqtd2doc(({
struct Cyc_Absyn_Tqual _temp3175; _temp3175.q_const= 0; _temp3175.q_volatile= 0;
_temp3175.q_restrict= 0; _temp3175;}),( void*) _temp2914->defn,({ struct Cyc_Core_Opt*
_temp3176=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3176->v=( void*)({ struct Cyc_PP_Doc*(* _temp3177)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp3180= Cyc_Absynpp_typedef_name2doc(
_temp2914->name); struct Cyc_PP_Doc* _temp3181= Cyc_Absynpp_tvars2doc( _temp2914->tvs);
struct Cyc_PP_Doc* _temp3178[ 2u]={ _temp3180, _temp3181}; struct _tagged_ptr0
_temp3179={ _temp3178, _temp3178, _temp3178 + 2u}; _temp3177( _temp3179);});
_temp3176;})); struct Cyc_PP_Doc* _temp3172= Cyc_PP_text(({ unsigned char*
_temp3173=( unsigned char*)";"; struct _tagged_string _temp3174; _temp3174.curr=
_temp3173; _temp3174.base= _temp3173; _temp3174.last_plus_one= _temp3173 + 2;
_temp3174;})); struct Cyc_PP_Doc* _temp3168[ 3u]={ _temp3170, _temp3171,
_temp3172}; struct _tagged_ptr0 _temp3169={ _temp3168, _temp3168, _temp3168 + 3u};
_temp3167( _temp3169);}); goto _LL2832; _LL2852: if( ! Cyc_Absynpp_qvar_to_Cids){
Cyc_Absynpp_curr_namespace_add( _temp2918);} s=({ struct Cyc_PP_Doc*(* _temp3184)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3187= Cyc_PP_text(({
unsigned char* _temp3200=( unsigned char*)"namespace "; struct _tagged_string
_temp3201; _temp3201.curr= _temp3200; _temp3201.base= _temp3200; _temp3201.last_plus_one=
_temp3200 + 11; _temp3201;})); struct Cyc_PP_Doc* _temp3188= Cyc_PP_textptr(
_temp2918); struct Cyc_PP_Doc* _temp3189= Cyc_PP_text(({ unsigned char*
_temp3198=( unsigned char*)" {"; struct _tagged_string _temp3199; _temp3199.curr=
_temp3198; _temp3199.base= _temp3198; _temp3199.last_plus_one= _temp3198 + 3;
_temp3199;})); struct Cyc_PP_Doc* _temp3190= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp3191=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({
unsigned char* _temp3196=( unsigned char*)""; struct _tagged_string _temp3197;
_temp3197.curr= _temp3196; _temp3197.base= _temp3196; _temp3197.last_plus_one=
_temp3196 + 1; _temp3197;}), _temp2916); struct Cyc_PP_Doc* _temp3192= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp3193= Cyc_PP_text(({ unsigned char* _temp3194=(
unsigned char*)"}"; struct _tagged_string _temp3195; _temp3195.curr= _temp3194;
_temp3195.base= _temp3194; _temp3195.last_plus_one= _temp3194 + 2; _temp3195;}));
struct Cyc_PP_Doc* _temp3185[ 7u]={ _temp3187, _temp3188, _temp3189, _temp3190,
_temp3191, _temp3192, _temp3193}; struct _tagged_ptr0 _temp3186={ _temp3185,
_temp3185, _temp3185 + 7u}; _temp3184( _temp3186);}); if( ! Cyc_Absynpp_qvar_to_Cids){
Cyc_Absynpp_curr_namespace_drop();} goto _LL2832; _LL2854: if( Cyc_Absynpp_print_using_stmts){
s=({ struct Cyc_PP_Doc*(* _temp3202)( struct _tagged_ptr0)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp3205= Cyc_PP_text(({ unsigned char* _temp3218=( unsigned char*)"using ";
struct _tagged_string _temp3219; _temp3219.curr= _temp3218; _temp3219.base=
_temp3218; _temp3219.last_plus_one= _temp3218 + 7; _temp3219;})); struct Cyc_PP_Doc*
_temp3206= Cyc_Absynpp_qvar2doc( _temp2922); struct Cyc_PP_Doc* _temp3207= Cyc_PP_text(({
unsigned char* _temp3216=( unsigned char*)" {"; struct _tagged_string _temp3217;
_temp3217.curr= _temp3216; _temp3217.base= _temp3216; _temp3217.last_plus_one=
_temp3216 + 3; _temp3217;})); struct Cyc_PP_Doc* _temp3208= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp3209=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)(
struct Cyc_Absyn_Decl*), struct _tagged_string sep, struct Cyc_List_List* l0))
Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({ unsigned char* _temp3214=( unsigned char*)"";
struct _tagged_string _temp3215; _temp3215.curr= _temp3214; _temp3215.base=
_temp3214; _temp3215.last_plus_one= _temp3214 + 1; _temp3215;}), _temp2920);
struct Cyc_PP_Doc* _temp3210= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3211=
Cyc_PP_text(({ unsigned char* _temp3212=( unsigned char*)"}"; struct
_tagged_string _temp3213; _temp3213.curr= _temp3212; _temp3213.base= _temp3212;
_temp3213.last_plus_one= _temp3212 + 2; _temp3213;})); struct Cyc_PP_Doc*
_temp3203[ 7u]={ _temp3205, _temp3206, _temp3207, _temp3208, _temp3209,
_temp3210, _temp3211}; struct _tagged_ptr0 _temp3204={ _temp3203, _temp3203,
_temp3203 + 7u}; _temp3202( _temp3204);});} else{ s=({ struct Cyc_PP_Doc*(*
_temp3220)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3223= Cyc_PP_text(({
unsigned char* _temp3236=( unsigned char*)"/* using "; struct _tagged_string
_temp3237; _temp3237.curr= _temp3236; _temp3237.base= _temp3236; _temp3237.last_plus_one=
_temp3236 + 10; _temp3237;})); struct Cyc_PP_Doc* _temp3224= Cyc_Absynpp_qvar2doc(
_temp2922); struct Cyc_PP_Doc* _temp3225= Cyc_PP_text(({ unsigned char*
_temp3234=( unsigned char*)" { */"; struct _tagged_string _temp3235; _temp3235.curr=
_temp3234; _temp3235.base= _temp3234; _temp3235.last_plus_one= _temp3234 + 6;
_temp3235;})); struct Cyc_PP_Doc* _temp3226= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp3227=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({
unsigned char* _temp3232=( unsigned char*)""; struct _tagged_string _temp3233;
_temp3233.curr= _temp3232; _temp3233.base= _temp3232; _temp3233.last_plus_one=
_temp3232 + 1; _temp3233;}), _temp2920); struct Cyc_PP_Doc* _temp3228= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp3229= Cyc_PP_text(({ unsigned char* _temp3230=(
unsigned char*)"/* } */"; struct _tagged_string _temp3231; _temp3231.curr=
_temp3230; _temp3231.base= _temp3230; _temp3231.last_plus_one= _temp3230 + 8;
_temp3231;})); struct Cyc_PP_Doc* _temp3221[ 7u]={ _temp3223, _temp3224,
_temp3225, _temp3226, _temp3227, _temp3228, _temp3229}; struct _tagged_ptr0
_temp3222={ _temp3221, _temp3221, _temp3221 + 7u}; _temp3220( _temp3222);});}
goto _LL2832; _LL2856: if( Cyc_Absynpp_print_externC_stmts){ s=({ struct Cyc_PP_Doc*(*
_temp3238)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3241= Cyc_PP_text(({
unsigned char* _temp3250=( unsigned char*)"extern \"C\" {"; struct
_tagged_string _temp3251; _temp3251.curr= _temp3250; _temp3251.base= _temp3250;
_temp3251.last_plus_one= _temp3250 + 13; _temp3251;})); struct Cyc_PP_Doc*
_temp3242= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3243=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({ unsigned char*
_temp3248=( unsigned char*)""; struct _tagged_string _temp3249; _temp3249.curr=
_temp3248; _temp3249.base= _temp3248; _temp3249.last_plus_one= _temp3248 + 1;
_temp3249;}), _temp2924); struct Cyc_PP_Doc* _temp3244= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp3245= Cyc_PP_text(({ unsigned char* _temp3246=(
unsigned char*)"}"; struct _tagged_string _temp3247; _temp3247.curr= _temp3246;
_temp3247.base= _temp3246; _temp3247.last_plus_one= _temp3246 + 2; _temp3247;}));
struct Cyc_PP_Doc* _temp3239[ 5u]={ _temp3241, _temp3242, _temp3243, _temp3244,
_temp3245}; struct _tagged_ptr0 _temp3240={ _temp3239, _temp3239, _temp3239 + 5u};
_temp3238( _temp3240);});} else{ s=({ struct Cyc_PP_Doc*(* _temp3252)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3255= Cyc_PP_text(({
unsigned char* _temp3264=( unsigned char*)"/* extern \"C\" { */"; struct
_tagged_string _temp3265; _temp3265.curr= _temp3264; _temp3265.base= _temp3264;
_temp3265.last_plus_one= _temp3264 + 19; _temp3265;})); struct Cyc_PP_Doc*
_temp3256= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3257=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({ unsigned char*
_temp3262=( unsigned char*)""; struct _tagged_string _temp3263; _temp3263.curr=
_temp3262; _temp3263.base= _temp3262; _temp3263.last_plus_one= _temp3262 + 1;
_temp3263;}), _temp2924); struct Cyc_PP_Doc* _temp3258= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp3259= Cyc_PP_text(({ unsigned char* _temp3260=(
unsigned char*)"/* } */"; struct _tagged_string _temp3261; _temp3261.curr=
_temp3260; _temp3261.base= _temp3260; _temp3261.last_plus_one= _temp3260 + 8;
_temp3261;})); struct Cyc_PP_Doc* _temp3253[ 5u]={ _temp3255, _temp3256,
_temp3257, _temp3258, _temp3259}; struct _tagged_ptr0 _temp3254={ _temp3253,
_temp3253, _temp3253 + 5u}; _temp3252( _temp3254);});} goto _LL2832; _LL2832:;}
return s;} struct Cyc_PP_Doc* Cyc_Absynpp_scope2doc( void* sc){ void* _temp3266=
sc; _LL3268: if( _temp3266 ==( void*) Cyc_Absyn_Static){ goto _LL3269;} else{
goto _LL3270;} _LL3270: if( _temp3266 ==( void*) Cyc_Absyn_Public){ goto _LL3271;}
else{ goto _LL3272;} _LL3272: if( _temp3266 ==( void*) Cyc_Absyn_Extern){ goto
_LL3273;} else{ goto _LL3274;} _LL3274: if( _temp3266 ==( void*) Cyc_Absyn_ExternC){
goto _LL3275;} else{ goto _LL3276;} _LL3276: if( _temp3266 ==( void*) Cyc_Absyn_Abstract){
goto _LL3277;} else{ goto _LL3267;} _LL3269: return Cyc_PP_text(({ unsigned char*
_temp3278=( unsigned char*)"static "; struct _tagged_string _temp3279; _temp3279.curr=
_temp3278; _temp3279.base= _temp3278; _temp3279.last_plus_one= _temp3278 + 8;
_temp3279;})); _LL3271: return Cyc_PP_nil_doc(); _LL3273: return Cyc_PP_text(({
unsigned char* _temp3280=( unsigned char*)"extern "; struct _tagged_string
_temp3281; _temp3281.curr= _temp3280; _temp3281.base= _temp3280; _temp3281.last_plus_one=
_temp3280 + 8; _temp3281;})); _LL3275: return Cyc_PP_text(({ unsigned char*
_temp3282=( unsigned char*)"extern \"C\" "; struct _tagged_string _temp3283;
_temp3283.curr= _temp3282; _temp3283.base= _temp3282; _temp3283.last_plus_one=
_temp3282 + 12; _temp3283;})); _LL3277: return Cyc_PP_text(({ unsigned char*
_temp3284=( unsigned char*)"abstract "; struct _tagged_string _temp3285;
_temp3285.curr= _temp3284; _temp3285.base= _temp3284; _temp3285.last_plus_one=
_temp3284 + 10; _temp3285;})); _LL3267:;} int Cyc_Absynpp_exists_temp_tvar_in_effect(
void* t){ void* _temp3286= t; struct Cyc_Absyn_Tvar* _temp3294; struct Cyc_List_List*
_temp3296; _LL3288: if(( unsigned int) _temp3286 > 4u?*(( int*) _temp3286) ==
Cyc_Absyn_VarType: 0){ _LL3295: _temp3294=(( struct Cyc_Absyn_VarType_struct*)
_temp3286)->f1; goto _LL3289;} else{ goto _LL3290;} _LL3290: if(( unsigned int)
_temp3286 > 4u?*(( int*) _temp3286) == Cyc_Absyn_JoinEff: 0){ _LL3297: _temp3296=((
struct Cyc_Absyn_JoinEff_struct*) _temp3286)->f1; goto _LL3291;} else{ goto
_LL3292;} _LL3292: goto _LL3293; _LL3289: return Cyc_Tcutil_is_temp_tvar(
_temp3294); _LL3291: return(( int(*)( int(* pred)( void*), struct Cyc_List_List*
x)) Cyc_List_exists)( Cyc_Absynpp_exists_temp_tvar_in_effect, _temp3296);
_LL3293: return 0; _LL3287:;} struct _tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual
tq, void* t){ void* _temp3298= t; struct Cyc_Absyn_Exp* _temp3312; struct Cyc_Absyn_Tqual
_temp3314; void* _temp3316; struct Cyc_Absyn_PtrInfo _temp3318; struct Cyc_Absyn_Conref*
_temp3320; struct Cyc_Absyn_Tqual _temp3322; struct Cyc_Absyn_Conref* _temp3324;
void* _temp3326; void* _temp3328; struct Cyc_Absyn_FnInfo _temp3330; struct Cyc_List_List*
_temp3332; struct Cyc_Absyn_VarargInfo* _temp3334; int _temp3336; struct Cyc_List_List*
_temp3338; void* _temp3340; struct Cyc_Core_Opt* _temp3342; struct Cyc_List_List*
_temp3344; struct Cyc_Core_Opt* _temp3346; int _temp3348; struct Cyc_Core_Opt*
_temp3350; struct Cyc_Core_Opt* _temp3352; struct Cyc_Core_Opt* _temp3354;
struct Cyc_List_List* _temp3356; struct _tuple0* _temp3358; _LL3300: if((
unsigned int) _temp3298 > 4u?*(( int*) _temp3298) == Cyc_Absyn_ArrayType: 0){
_LL3317: _temp3316=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3298)->f1;
goto _LL3315; _LL3315: _temp3314=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3298)->f2; goto _LL3313; _LL3313: _temp3312=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3298)->f3; goto _LL3301;} else{ goto _LL3302;} _LL3302: if(( unsigned int)
_temp3298 > 4u?*(( int*) _temp3298) == Cyc_Absyn_PointerType: 0){ _LL3319:
_temp3318=(( struct Cyc_Absyn_PointerType_struct*) _temp3298)->f1; _LL3329:
_temp3328=( void*) _temp3318.elt_typ; goto _LL3327; _LL3327: _temp3326=( void*)
_temp3318.rgn_typ; goto _LL3325; _LL3325: _temp3324= _temp3318.nullable; goto
_LL3323; _LL3323: _temp3322= _temp3318.tq; goto _LL3321; _LL3321: _temp3320=
_temp3318.bounds; goto _LL3303;} else{ goto _LL3304;} _LL3304: if(( unsigned int)
_temp3298 > 4u?*(( int*) _temp3298) == Cyc_Absyn_FnType: 0){ _LL3331: _temp3330=((
struct Cyc_Absyn_FnType_struct*) _temp3298)->f1; _LL3345: _temp3344= _temp3330.tvars;
goto _LL3343; _LL3343: _temp3342= _temp3330.effect; goto _LL3341; _LL3341:
_temp3340=( void*) _temp3330.ret_typ; goto _LL3339; _LL3339: _temp3338=
_temp3330.args; goto _LL3337; _LL3337: _temp3336= _temp3330.c_varargs; goto
_LL3335; _LL3335: _temp3334= _temp3330.cyc_varargs; goto _LL3333; _LL3333:
_temp3332= _temp3330.attributes; goto _LL3305;} else{ goto _LL3306;} _LL3306:
if(( unsigned int) _temp3298 > 4u?*(( int*) _temp3298) == Cyc_Absyn_Evar: 0){
_LL3353: _temp3352=(( struct Cyc_Absyn_Evar_struct*) _temp3298)->f1; goto
_LL3351; _LL3351: _temp3350=(( struct Cyc_Absyn_Evar_struct*) _temp3298)->f2;
goto _LL3349; _LL3349: _temp3348=(( struct Cyc_Absyn_Evar_struct*) _temp3298)->f3;
goto _LL3347; _LL3347: _temp3346=(( struct Cyc_Absyn_Evar_struct*) _temp3298)->f4;
goto _LL3307;} else{ goto _LL3308;} _LL3308: if(( unsigned int) _temp3298 > 4u?*((
int*) _temp3298) == Cyc_Absyn_TypedefType: 0){ _LL3359: _temp3358=(( struct Cyc_Absyn_TypedefType_struct*)
_temp3298)->f1; goto _LL3357; _LL3357: _temp3356=(( struct Cyc_Absyn_TypedefType_struct*)
_temp3298)->f2; goto _LL3355; _LL3355: _temp3354=(( struct Cyc_Absyn_TypedefType_struct*)
_temp3298)->f3; goto _LL3309;} else{ goto _LL3310;} _LL3310: goto _LL3311;
_LL3301: { struct Cyc_List_List* _temp3362; void* _temp3364; struct Cyc_Absyn_Tqual
_temp3366; struct _tuple4 _temp3360= Cyc_Absynpp_to_tms( _temp3314, _temp3316);
_LL3367: _temp3366= _temp3360.f1; goto _LL3365; _LL3365: _temp3364= _temp3360.f2;
goto _LL3363; _LL3363: _temp3362= _temp3360.f3; goto _LL3361; _LL3361: { void*
tm; if( _temp3312 == 0){ tm=( void*) Cyc_Absyn_Carray_mod;} else{ tm=( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp3368=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp3368[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp3369; _temp3369.tag= Cyc_Absyn_ConstArray_mod;
_temp3369.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp3312); _temp3369;});
_temp3368;});} return({ struct _tuple4 _temp3370; _temp3370.f1= _temp3366;
_temp3370.f2= _temp3364; _temp3370.f3=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp3371=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3371->hd=( void*) tm; _temp3371->tl= _temp3362; _temp3371;}); _temp3370;});}}
_LL3303: { struct Cyc_List_List* _temp3374; void* _temp3376; struct Cyc_Absyn_Tqual
_temp3378; struct _tuple4 _temp3372= Cyc_Absynpp_to_tms( _temp3322, _temp3328);
_LL3379: _temp3378= _temp3372.f1; goto _LL3377; _LL3377: _temp3376= _temp3372.f2;
goto _LL3375; _LL3375: _temp3374= _temp3372.f3; goto _LL3373; _LL3373: { void*
ps;{ struct _tuple8 _temp3381=({ struct _tuple8 _temp3380; _temp3380.f1=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp3324))->v; _temp3380.f2=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp3320))->v; _temp3380;}); void* _temp3391;
void* _temp3393; struct Cyc_Absyn_Exp* _temp3395; void* _temp3397; int _temp3399;
void* _temp3401; void* _temp3403; struct Cyc_Absyn_Exp* _temp3405; void*
_temp3407; int _temp3409; void* _temp3411; void* _temp3413; void* _temp3415;
_LL3383: _LL3398: _temp3397= _temp3381.f1; if(( unsigned int) _temp3397 > 1u?*((
int*) _temp3397) == Cyc_Absyn_Eq_constr: 0){ _LL3400: _temp3399=( int)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp3397)->f1; if( _temp3399 == 1){ goto _LL3392;}
else{ goto _LL3385;}} else{ goto _LL3385;} _LL3392: _temp3391= _temp3381.f2; if((
unsigned int) _temp3391 > 1u?*(( int*) _temp3391) == Cyc_Absyn_Eq_constr: 0){
_LL3394: _temp3393=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3391)->f1;
if(( unsigned int) _temp3393 > 1u?*(( int*) _temp3393) == Cyc_Absyn_Upper_b: 0){
_LL3396: _temp3395=(( struct Cyc_Absyn_Upper_b_struct*) _temp3393)->f1; goto
_LL3384;} else{ goto _LL3385;}} else{ goto _LL3385;} _LL3385: _LL3408: _temp3407=
_temp3381.f1; if(( unsigned int) _temp3407 > 1u?*(( int*) _temp3407) == Cyc_Absyn_Eq_constr:
0){ _LL3410: _temp3409=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3407)->f1;
if( _temp3409 == 0){ goto _LL3402;} else{ goto _LL3387;}} else{ goto _LL3387;}
_LL3402: _temp3401= _temp3381.f2; if(( unsigned int) _temp3401 > 1u?*(( int*)
_temp3401) == Cyc_Absyn_Eq_constr: 0){ _LL3404: _temp3403=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3401)->f1; if(( unsigned int) _temp3403 > 1u?*(( int*) _temp3403) == Cyc_Absyn_Upper_b:
0){ _LL3406: _temp3405=(( struct Cyc_Absyn_Upper_b_struct*) _temp3403)->f1; goto
_LL3386;} else{ goto _LL3387;}} else{ goto _LL3387;} _LL3387: _LL3416: _temp3415=
_temp3381.f1; goto _LL3412; _LL3412: _temp3411= _temp3381.f2; if(( unsigned int)
_temp3411 > 1u?*(( int*) _temp3411) == Cyc_Absyn_Eq_constr: 0){ _LL3414:
_temp3413=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3411)->f1; if(
_temp3413 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3388;} else{ goto _LL3389;}}
else{ goto _LL3389;} _LL3389: goto _LL3390; _LL3384: ps=( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp3417=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp3417[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp3418; _temp3418.tag=
Cyc_Absyn_Nullable_ps; _temp3418.f1= _temp3395; _temp3418;}); _temp3417;}); goto
_LL3382; _LL3386: ps=( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp3419=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp3419[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp3420; _temp3420.tag= Cyc_Absyn_NonNullable_ps; _temp3420.f1= _temp3405;
_temp3420;}); _temp3419;}); goto _LL3382; _LL3388: ps=( void*) Cyc_Absyn_TaggedArray_ps;
goto _LL3382; _LL3390: ps=( void*) Cyc_Absyn_TaggedArray_ps; goto _LL3382;
_LL3382:;}{ void* tm=( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp3423=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3423[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp3424; _temp3424.tag=
Cyc_Absyn_Pointer_mod; _temp3424.f1=( void*) ps; _temp3424.f2=( void*) _temp3326;
_temp3424.f3= tq; _temp3424;}); _temp3423;}); return({ struct _tuple4 _temp3421;
_temp3421.f1= _temp3378; _temp3421.f2= _temp3376; _temp3421.f3=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp3422=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3422->hd=( void*) tm; _temp3422->tl= _temp3374;
_temp3422;}); _temp3421;});}}} _LL3305: if( ! Cyc_Absynpp_print_all_tvars){ if(
_temp3342 == 0? 1: Cyc_Absynpp_exists_temp_tvar_in_effect(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3342))->v)){ _temp3342= 0; _temp3344= 0;}} else{ if( ! Cyc_Absynpp_dont_rewrite_temp_tvars){((
void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)(
Cyc_Tcutil_rewrite_temp_tvar, _temp3344);}}{ struct Cyc_List_List* _temp3427;
void* _temp3429; struct Cyc_Absyn_Tqual _temp3431; struct _tuple4 _temp3425= Cyc_Absynpp_to_tms(
Cyc_Absyn_empty_tqual(), _temp3340); _LL3432: _temp3431= _temp3425.f1; goto
_LL3430; _LL3430: _temp3429= _temp3425.f2; goto _LL3428; _LL3428: _temp3427=
_temp3425.f3; goto _LL3426; _LL3426: { struct Cyc_List_List* tms= _temp3427; if(
_temp3332 != 0){ tms=({ struct Cyc_List_List* _temp3433=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3433->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp3434=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp3434[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp3435; _temp3435.tag= Cyc_Absyn_Attributes_mod;
_temp3435.f1= 0; _temp3435.f2= _temp3332; _temp3435;}); _temp3434;})); _temp3433->tl=
tms; _temp3433;});} tms=({ struct Cyc_List_List* _temp3436=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3436->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp3437=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3437[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3438; _temp3438.tag= Cyc_Absyn_Function_mod;
_temp3438.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp3439=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3439[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3440; _temp3440.tag= Cyc_Absyn_WithTypes;
_temp3440.f1= _temp3338; _temp3440.f2= _temp3336; _temp3440.f3= _temp3334;
_temp3440.f4= _temp3342; _temp3440;}); _temp3439;})); _temp3438;}); _temp3437;}));
_temp3436->tl= tms; _temp3436;}); if( _temp3344 != 0){ tms=({ struct Cyc_List_List*
_temp3441=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3441->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp3442=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp3442[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp3443; _temp3443.tag= Cyc_Absyn_TypeParams_mod; _temp3443.f1= _temp3344;
_temp3443.f2= 0; _temp3443.f3= 1; _temp3443;}); _temp3442;})); _temp3441->tl=
tms; _temp3441;});} return({ struct _tuple4 _temp3444; _temp3444.f1= _temp3431;
_temp3444.f2= _temp3429; _temp3444.f3= tms; _temp3444;});}} _LL3307: if(
_temp3350 == 0){ return({ struct _tuple4 _temp3445; _temp3445.f1= tq; _temp3445.f2=
t; _temp3445.f3= 0; _temp3445;});} else{ return Cyc_Absynpp_to_tms( tq,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp3350))->v);} _LL3309: if( _temp3354 == 0?
1: ! Cyc_Absynpp_expand_typedefs){ return({ struct _tuple4 _temp3446; _temp3446.f1=
tq; _temp3446.f2= t; _temp3446.f3= 0; _temp3446;});} else{ return Cyc_Absynpp_to_tms(
tq,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3354))->v);} _LL3311:
return({ struct _tuple4 _temp3447; _temp3447.f1= tq; _temp3447.f2= t; _temp3447.f3=
0; _temp3447;}); _LL3299:;} struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc( struct Cyc_Absyn_Tqual
tq, void* t, struct Cyc_Core_Opt* dopt){ struct Cyc_List_List* _temp3450; void*
_temp3452; struct Cyc_Absyn_Tqual _temp3454; struct _tuple4 _temp3448= Cyc_Absynpp_to_tms(
tq, t); _LL3455: _temp3454= _temp3448.f1; goto _LL3453; _LL3453: _temp3452=
_temp3448.f2; goto _LL3451; _LL3451: _temp3450= _temp3448.f3; goto _LL3449;
_LL3449: _temp3450=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp3450); if( _temp3450 == 0? dopt == 0: 0){ return({ struct Cyc_PP_Doc*(*
_temp3456)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3459= Cyc_Absynpp_tqual2doc(
_temp3454); struct Cyc_PP_Doc* _temp3460= Cyc_Absynpp_ntyp2doc( _temp3452);
struct Cyc_PP_Doc* _temp3457[ 2u]={ _temp3459, _temp3460}; struct _tagged_ptr0
_temp3458={ _temp3457, _temp3457, _temp3457 + 2u}; _temp3456( _temp3458);});}
else{ return({ struct Cyc_PP_Doc*(* _temp3461)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp3464= Cyc_Absynpp_tqual2doc( _temp3454); struct Cyc_PP_Doc*
_temp3465= Cyc_Absynpp_ntyp2doc( _temp3452); struct Cyc_PP_Doc* _temp3466= Cyc_PP_text(({
unsigned char* _temp3468=( unsigned char*)" "; struct _tagged_string _temp3469;
_temp3469.curr= _temp3468; _temp3469.base= _temp3468; _temp3469.last_plus_one=
_temp3468 + 2; _temp3469;})); struct Cyc_PP_Doc* _temp3467= Cyc_Absynpp_dtms2doc(
dopt == 0? Cyc_PP_nil_doc():( struct Cyc_PP_Doc*)(( struct Cyc_Core_Opt*)
_check_null( dopt))->v, _temp3450); struct Cyc_PP_Doc* _temp3462[ 4u]={
_temp3464, _temp3465, _temp3466, _temp3467}; struct _tagged_ptr0 _temp3463={
_temp3462, _temp3462, _temp3462 + 4u}; _temp3461( _temp3463);});}} struct Cyc_PP_Doc*
Cyc_Absynpp_structfield2doc( struct Cyc_Absyn_Structfield* f){ if( f->width != 0){
return({ struct Cyc_PP_Doc*(* _temp3470)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp3473= Cyc_Absynpp_tqtd2doc( f->tq,( void*) f->type,({
struct Cyc_Core_Opt* _temp3482=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3482->v=( void*) Cyc_PP_textptr( f->name); _temp3482;}));
struct Cyc_PP_Doc* _temp3474= Cyc_PP_text(({ unsigned char* _temp3480=(
unsigned char*)":"; struct _tagged_string _temp3481; _temp3481.curr= _temp3480;
_temp3481.base= _temp3480; _temp3481.last_plus_one= _temp3480 + 2; _temp3481;}));
struct Cyc_PP_Doc* _temp3475= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( f->width))->v); struct Cyc_PP_Doc* _temp3476=
Cyc_Absynpp_atts2doc( f->attributes); struct Cyc_PP_Doc* _temp3477= Cyc_PP_text(({
unsigned char* _temp3478=( unsigned char*)";"; struct _tagged_string _temp3479;
_temp3479.curr= _temp3478; _temp3479.base= _temp3478; _temp3479.last_plus_one=
_temp3478 + 2; _temp3479;})); struct Cyc_PP_Doc* _temp3471[ 5u]={ _temp3473,
_temp3474, _temp3475, _temp3476, _temp3477}; struct _tagged_ptr0 _temp3472={
_temp3471, _temp3471, _temp3471 + 5u}; _temp3470( _temp3472);});} else{ return({
struct Cyc_PP_Doc*(* _temp3483)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp3486= Cyc_Absynpp_tqtd2doc( f->tq,( void*) f->type,({ struct Cyc_Core_Opt*
_temp3491=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3491->v=( void*) Cyc_PP_textptr( f->name); _temp3491;})); struct Cyc_PP_Doc*
_temp3487= Cyc_Absynpp_atts2doc( f->attributes); struct Cyc_PP_Doc* _temp3488=
Cyc_PP_text(({ unsigned char* _temp3489=( unsigned char*)";"; struct
_tagged_string _temp3490; _temp3490.curr= _temp3489; _temp3490.base= _temp3489;
_temp3490.last_plus_one= _temp3489 + 2; _temp3490;})); struct Cyc_PP_Doc*
_temp3484[ 3u]={ _temp3486, _temp3487, _temp3488}; struct _tagged_ptr0 _temp3485={
_temp3484, _temp3484, _temp3484 + 3u}; _temp3483( _temp3485);});}} struct Cyc_PP_Doc*
Cyc_Absynpp_structfields2doc( struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Structfield*), struct _tagged_string
sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_structfield2doc,({
unsigned char* _temp3492=( unsigned char*)""; struct _tagged_string _temp3493;
_temp3493.curr= _temp3492; _temp3493.base= _temp3492; _temp3493.last_plus_one=
_temp3492 + 1; _temp3493;}), fields);} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfield2doc(
struct Cyc_Absyn_Tunionfield* f){ return({ struct Cyc_PP_Doc*(* _temp3494)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3497= Cyc_Absynpp_scope2doc((
void*) f->sc); struct Cyc_PP_Doc* _temp3498= Cyc_Absynpp_typedef_name2doc( f->name);
struct Cyc_PP_Doc* _temp3499= f->tvs == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_ktvars2doc(
f->tvs); struct Cyc_PP_Doc* _temp3500= f->typs == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(
f->typs); struct Cyc_PP_Doc* _temp3495[ 4u]={ _temp3497, _temp3498, _temp3499,
_temp3500}; struct _tagged_ptr0 _temp3496={ _temp3495, _temp3495, _temp3495 + 4u};
_temp3494( _temp3496);});} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Tunionfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_tunionfield2doc,({ unsigned char* _temp3501=(
unsigned char*)","; struct _tagged_string _temp3502; _temp3502.curr= _temp3501;
_temp3502.base= _temp3501; _temp3502.last_plus_one= _temp3501 + 2; _temp3502;}),
fields);} void Cyc_Absynpp_decllist2file( struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE*
f){ for( 0; tdl != 0; tdl=(( struct Cyc_List_List*) _check_null( tdl))->tl){ Cyc_PP_file_of_doc(
Cyc_Absynpp_decl2doc(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( tdl))->hd), 72, f); fprintf( sfile_to_file( f),"\n");}} struct
_tagged_string Cyc_Absynpp_decllist2string( struct Cyc_List_List* tdl){ return
Cyc_PP_string_of_doc( Cyc_PP_seql(({ unsigned char* _temp3503=( unsigned char*)"";
struct _tagged_string _temp3504; _temp3504.curr= _temp3503; _temp3504.base=
_temp3503; _temp3504.last_plus_one= _temp3503 + 1; _temp3504;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Decl*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_decl2doc, tdl)), 72);} struct _tagged_string Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp* e){ return Cyc_PP_string_of_doc( Cyc_Absynpp_exp2doc( e),
72);} struct _tagged_string Cyc_Absynpp_stmt2string( struct Cyc_Absyn_Stmt* s){
return Cyc_PP_string_of_doc( Cyc_Absynpp_stmt2doc( s), 72);} struct
_tagged_string Cyc_Absynpp_typ2string( void* t){ Cyc_Absynpp_dont_rewrite_temp_tvars=
1;{ struct _tagged_string _temp3505= Cyc_PP_string_of_doc( Cyc_Absynpp_typ2doc(
t), 72); goto _LL3506; _LL3506: Cyc_Absynpp_dont_rewrite_temp_tvars= 0; return
_temp3505;}} struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple0* v){
return Cyc_PP_string_of_doc( Cyc_Absynpp_qvar2doc( v), 72);} struct
_tagged_string Cyc_Absynpp_prim2string( void* p){ return Cyc_PP_string_of_doc(
Cyc_Absynpp_prim2doc( p), 72);} struct _tagged_string Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat* p){ return Cyc_PP_string_of_doc( Cyc_Absynpp_pat2doc( p),
72);} struct _tagged_string Cyc_Absynpp_scope2string( void* sc){ void* _temp3507=
sc; _LL3509: if( _temp3507 ==( void*) Cyc_Absyn_Static){ goto _LL3510;} else{
goto _LL3511;} _LL3511: if( _temp3507 ==( void*) Cyc_Absyn_Public){ goto _LL3512;}
else{ goto _LL3513;} _LL3513: if( _temp3507 ==( void*) Cyc_Absyn_Extern){ goto
_LL3514;} else{ goto _LL3515;} _LL3515: if( _temp3507 ==( void*) Cyc_Absyn_ExternC){
goto _LL3516;} else{ goto _LL3517;} _LL3517: if( _temp3507 ==( void*) Cyc_Absyn_Abstract){
goto _LL3518;} else{ goto _LL3508;} _LL3510: return({ unsigned char* _temp3519=(
unsigned char*)"static"; struct _tagged_string _temp3520; _temp3520.curr=
_temp3519; _temp3520.base= _temp3519; _temp3520.last_plus_one= _temp3519 + 7;
_temp3520;}); _LL3512: return({ unsigned char* _temp3521=( unsigned char*)"public";
struct _tagged_string _temp3522; _temp3522.curr= _temp3521; _temp3522.base=
_temp3521; _temp3522.last_plus_one= _temp3521 + 7; _temp3522;}); _LL3514: return({
unsigned char* _temp3523=( unsigned char*)"extern"; struct _tagged_string
_temp3524; _temp3524.curr= _temp3523; _temp3524.base= _temp3523; _temp3524.last_plus_one=
_temp3523 + 7; _temp3524;}); _LL3516: return({ unsigned char* _temp3525=(
unsigned char*)"extern \"C\""; struct _tagged_string _temp3526; _temp3526.curr=
_temp3525; _temp3526.base= _temp3525; _temp3526.last_plus_one= _temp3525 + 11;
_temp3526;}); _LL3518: return({ unsigned char* _temp3527=( unsigned char*)"abstract";
struct _tagged_string _temp3528; _temp3528.curr= _temp3527; _temp3528.base=
_temp3527; _temp3528.last_plus_one= _temp3527 + 9; _temp3528;}); _LL3508:;}