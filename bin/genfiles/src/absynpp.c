 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern struct _tagged_arr Cyc_Core_new_string( int); extern
unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern struct _tagged_arr Cyc_Core_string_of_int(
int); extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); extern int system( unsigned char*); extern int*
__errno(); struct Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[
19u]; extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; struct Cyc_List_List{ void* hd;
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
f); extern struct _tagged_arr Cyc_PP_string_of_doc( struct Cyc_PP_Doc* d, int w);
extern struct Cyc_PP_Doc* Cyc_PP_nil_doc(); extern struct Cyc_PP_Doc* Cyc_PP_line_doc();
extern struct Cyc_PP_Doc* Cyc_PP_text( struct _tagged_arr s); extern struct Cyc_PP_Doc*
Cyc_PP_textptr( struct _tagged_arr* s); extern struct Cyc_PP_Doc* Cyc_PP_nest(
int k, struct Cyc_PP_Doc* d); extern struct Cyc_PP_Doc* Cyc_PP_cat( struct
_tagged_arr); extern struct Cyc_PP_Doc* Cyc_PP_ppseq( struct Cyc_PP_Doc*(* pp)(
void*), struct _tagged_arr sep, struct Cyc_List_List* l0); extern struct Cyc_PP_Doc*
Cyc_PP_ppseql( struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_arr sep, struct
Cyc_List_List* l0); extern struct Cyc_PP_Doc* Cyc_PP_seql( struct _tagged_arr
sep, struct Cyc_List_List* l0); extern struct Cyc_PP_Doc* Cyc_PP_group( struct
_tagged_arr start, struct _tagged_arr stop, struct _tagged_arr sep, struct Cyc_List_List*
ss); extern struct Cyc_PP_Doc* Cyc_PP_egroup( struct _tagged_arr start, struct
_tagged_arr stop, struct _tagged_arr sep, struct Cyc_List_List* ss); struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file; struct _tagged_arr line; int line_no; int col;
} ; extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment;
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; struct
_tuple0{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual;
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
struct Cyc_Absyn_UnknownTunionInfo{ struct _tuple0* name; int is_xtunion; } ;
static const int Cyc_Absyn_UnknownTunion= 0; struct Cyc_Absyn_UnknownTunion_struct{
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
int tag; struct _tagged_arr f1; } ; static const int Cyc_Absyn_Nocommon_att= 6;
static const int Cyc_Absyn_Shared_att= 7; static const int Cyc_Absyn_Unused_att=
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
int tag; void* f1; struct _tagged_arr* f2; } ; static const int Cyc_Absyn_Deref_e=
19; struct Cyc_Absyn_Deref_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_StructMember_e= 20; struct Cyc_Absyn_StructMember_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tagged_arr* f2; } ; static const int
Cyc_Absyn_StructArrow_e= 21; struct Cyc_Absyn_StructArrow_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct _tagged_arr* f2; } ; static const int Cyc_Absyn_Subscript_e=
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
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_arr* f1; struct Cyc_Absyn_Stmt*
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
struct _tagged_arr* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Do_s=
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
attributes; } ; struct Cyc_Absyn_Structfield{ struct _tagged_arr* name; struct
Cyc_Absyn_Tqual tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List*
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
int tag; struct _tagged_arr* f1; struct Cyc_List_List* f2; } ; static const int
Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{ int tag; struct _tuple0*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d= 11;
struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ; struct
Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static const int
Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int tag; struct
Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1; struct Cyc_Absyn_FieldName_struct{
int tag; struct _tagged_arr* f1; } ; extern unsigned char Cyc_Absyn_EmptyAnnot[
15u]; extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x); extern int Cyc_Absyn_is_format_prim(
void* p); extern struct _tagged_arr Cyc_Absyn_attribute2string( void*); struct
Cyc_Buffer_t; extern int Cyc_String_strptrcmp( struct _tagged_arr* s1, struct
_tagged_arr* s2); extern struct _tagged_arr Cyc_String_str_sepstr( struct Cyc_List_List*,
struct _tagged_arr); extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct
Cyc_Absyn_Exp* e); struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[ 11u];
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
extern struct _tagged_arr Cyc_Absynpp_typ2string( void*); extern struct
_tagged_arr Cyc_Absynpp_kind2string( void*); extern struct _tagged_arr Cyc_Absynpp_ckind2string(
struct Cyc_Absyn_Conref*); extern struct _tagged_arr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*); extern struct _tagged_arr Cyc_Absynpp_stmt2string(
struct Cyc_Absyn_Stmt*); extern struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple0*); extern struct _tagged_arr Cyc_Absynpp_decllist2string( struct
Cyc_List_List* tdl); extern struct _tagged_arr Cyc_Absynpp_prim2string( void* p);
extern struct _tagged_arr Cyc_Absynpp_pat2string( struct Cyc_Absyn_Pat* p);
extern struct _tagged_arr Cyc_Absynpp_scope2string( void* sc); extern struct
_tagged_arr Cyc_Absynpp_cyc_string; extern struct _tagged_arr* Cyc_Absynpp_cyc_stringptr;
extern int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp*); extern struct
_tagged_arr Cyc_Absynpp_char_escape( unsigned char); extern struct _tagged_arr
Cyc_Absynpp_string_escape( struct _tagged_arr); extern struct _tagged_arr Cyc_Absynpp_prim2str(
void* p); extern int Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s);
struct _tuple3{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ;
extern struct _tuple1* Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg); struct
_tuple4{ struct Cyc_Absyn_Tqual f1; void* f2; struct Cyc_List_List* f3; } ;
extern struct _tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual tq, void* t);
struct _tuple5{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; extern
struct Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct _tuple5* dp); extern struct Cyc_PP_Doc*
Cyc_Absynpp_switchclauses2doc( struct Cyc_List_List* cs); extern struct Cyc_PP_Doc*
Cyc_Absynpp_switchCclauses2doc( struct Cyc_List_List* cs); extern struct Cyc_PP_Doc*
Cyc_Absynpp_typ2doc( void*); extern struct Cyc_PP_Doc* Cyc_Absynpp_structfields2doc(
struct Cyc_List_List* fields); extern struct Cyc_PP_Doc* Cyc_Absynpp_scope2doc(
void*); extern struct Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct Cyc_Absyn_Stmt*);
extern struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp*); extern
struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec( int inprec, struct Cyc_Absyn_Exp* e);
extern struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec( int inprec, struct Cyc_List_List*
es); extern struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc( struct _tuple0*); extern
struct Cyc_PP_Doc* Cyc_Absynpp_typedef_name2doc( struct _tuple0*); extern struct
Cyc_PP_Doc* Cyc_Absynpp_cnst2doc( void*); extern struct Cyc_PP_Doc* Cyc_Absynpp_prim2doc(
void*); extern struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc( int inprec, void* p,
struct Cyc_List_List* es); struct _tuple6{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp*
f2; } ; extern struct Cyc_PP_Doc* Cyc_Absynpp_de2doc( struct _tuple6* de);
extern struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc( struct Cyc_Absyn_Tqual tq, void*
t, struct Cyc_Core_Opt* dopt); extern struct Cyc_PP_Doc* Cyc_Absynpp_funargs2doc(
struct Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo*
cyc_varargs, struct Cyc_Core_Opt* effopt, struct Cyc_List_List* rgn_po); extern
struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc( struct Cyc_List_List* fields);
static int Cyc_Absynpp_expand_typedefs; static int Cyc_Absynpp_qvar_to_Cids;
static unsigned char _temp0[ 4u]="Cyc"; struct _tagged_arr Cyc_Absynpp_cyc_string={
_temp0, _temp0, _temp0 + 4u}; struct _tagged_arr* Cyc_Absynpp_cyc_stringptr=&
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
struct _tagged_arr* v){ Cyc_Absynpp_curr_namespace=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_Absynpp_curr_namespace,({
struct Cyc_List_List* _temp1=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp1->hd=( void*) v; _temp1->tl= 0; _temp1;}));} static void
Cyc_Absynpp_suppr_last( struct Cyc_List_List** l){ if((( struct Cyc_List_List*)
_check_null(* l))->tl == 0){* l= 0;} else{(( void(*)( struct Cyc_List_List** l))
Cyc_Absynpp_suppr_last)(&(( struct Cyc_List_List*) _check_null(* l))->tl);}}
static void Cyc_Absynpp_curr_namespace_drop(){(( void(*)( struct Cyc_List_List**
l)) Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);} struct _tagged_arr
Cyc_Absynpp_char_escape( unsigned char c){ switch( c){ case '\a': _LL2: return
_tag_arr("\\a", sizeof( unsigned char), 3u); case '\b': _LL3: return _tag_arr("\\b",
sizeof( unsigned char), 3u); case '\f': _LL4: return _tag_arr("\\f", sizeof(
unsigned char), 3u); case '\n': _LL5: return _tag_arr("\\n", sizeof(
unsigned char), 3u); case '\r': _LL6: return _tag_arr("\\r", sizeof(
unsigned char), 3u); case '\t': _LL7: return _tag_arr("\\t", sizeof(
unsigned char), 3u); case '\v': _LL8: return _tag_arr("\\v", sizeof(
unsigned char), 3u); case '\\': _LL9: return _tag_arr("\\\\", sizeof(
unsigned char), 3u); case '"': _LL10: return _tag_arr("\"", sizeof(
unsigned char), 2u); case '\'': _LL11: return _tag_arr("\\'", sizeof(
unsigned char), 3u); default: _LL12: if( c >=' '? c <='~': 0){ struct
_tagged_arr _temp14= Cyc_Core_new_string( 1);*(( unsigned char*)
_check_unknown_subscript( _temp14, sizeof( unsigned char), 0))= c; return(
struct _tagged_arr) _temp14;} else{ struct _tagged_arr _temp15= Cyc_Core_new_string(
4); int j= 0;*(( unsigned char*) _check_unknown_subscript( _temp15, sizeof(
unsigned char), j ++))='\\';*(( unsigned char*) _check_unknown_subscript(
_temp15, sizeof( unsigned char), j ++))=( unsigned char)('0' +(( unsigned char)
c >> 6 & 7));*(( unsigned char*) _check_unknown_subscript( _temp15, sizeof(
unsigned char), j ++))=( unsigned char)('0' +( c >> 3 & 7));*(( unsigned char*)
_check_unknown_subscript( _temp15, sizeof( unsigned char), j ++))=(
unsigned char)('0' +( c & 7)); return( struct _tagged_arr) _temp15;}}} static
int Cyc_Absynpp_special( struct _tagged_arr s){ int sz=( int)( _get_arr_size( s,
sizeof( unsigned char)) - 1);{ int i= 0; for( 0; i < sz; i ++){ unsigned char c=*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i));
if((( c <=' '? 1: c >='~')? 1: c =='"')? 1: c =='\\'){ return 1;}}} return 0;}
struct _tagged_arr Cyc_Absynpp_string_escape( struct _tagged_arr s){ if( ! Cyc_Absynpp_special(
s)){ return s;}{ int n=( int)( _get_arr_size( s, sizeof( unsigned char)) - 1);
if( n > 0?*(( const unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), n)) =='\000': 0){ n --;}{ int len= 0;{ int i= 0; for( 0; i <= n;
i ++){ unsigned char _temp16=*(( const unsigned char*) _check_unknown_subscript(
s, sizeof( unsigned char), i)); _LL18: if( _temp16 =='\a'){ goto _LL19;} else{
goto _LL20;} _LL20: if( _temp16 =='\b'){ goto _LL21;} else{ goto _LL22;} _LL22:
if( _temp16 =='\f'){ goto _LL23;} else{ goto _LL24;} _LL24: if( _temp16 =='\n'){
goto _LL25;} else{ goto _LL26;} _LL26: if( _temp16 =='\r'){ goto _LL27;} else{
goto _LL28;} _LL28: if( _temp16 =='\t'){ goto _LL29;} else{ goto _LL30;} _LL30:
if( _temp16 =='\v'){ goto _LL31;} else{ goto _LL32;} _LL32: if( _temp16 =='\\'){
goto _LL33;} else{ goto _LL34;} _LL34: if( _temp16 =='"'){ goto _LL35;} else{
goto _LL36;} _LL36: goto _LL37; _LL19: goto _LL21; _LL21: goto _LL23; _LL23:
goto _LL25; _LL25: goto _LL27; _LL27: goto _LL29; _LL29: goto _LL31; _LL31: goto
_LL33; _LL33: goto _LL35; _LL35: len += 2; goto _LL17; _LL37: if( _temp16 >=' '?
_temp16 <='~': 0){ len ++;} else{ len += 4;} goto _LL17; _LL17:;}}{ struct
_tagged_arr t= Cyc_Core_new_string( len); int j= 0;{ int i= 0; for( 0; i <= n; i
++){ unsigned char _temp38=*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), i)); _LL40: if( _temp38 =='\a'){ goto _LL41;} else{ goto
_LL42;} _LL42: if( _temp38 =='\b'){ goto _LL43;} else{ goto _LL44;} _LL44: if(
_temp38 =='\f'){ goto _LL45;} else{ goto _LL46;} _LL46: if( _temp38 =='\n'){
goto _LL47;} else{ goto _LL48;} _LL48: if( _temp38 =='\r'){ goto _LL49;} else{
goto _LL50;} _LL50: if( _temp38 =='\t'){ goto _LL51;} else{ goto _LL52;} _LL52:
if( _temp38 =='\v'){ goto _LL53;} else{ goto _LL54;} _LL54: if( _temp38 =='\\'){
goto _LL55;} else{ goto _LL56;} _LL56: if( _temp38 =='"'){ goto _LL57;} else{
goto _LL58;} _LL58: goto _LL59; _LL41:*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))='\\';*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='a';
goto _LL39; _LL43:*(( unsigned char*) _check_unknown_subscript( t, sizeof(
unsigned char), j ++))='\\';*(( unsigned char*) _check_unknown_subscript( t,
sizeof( unsigned char), j ++))='b'; goto _LL39; _LL45:*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))='\\';*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='f';
goto _LL39; _LL47:*(( unsigned char*) _check_unknown_subscript( t, sizeof(
unsigned char), j ++))='\\';*(( unsigned char*) _check_unknown_subscript( t,
sizeof( unsigned char), j ++))='n'; goto _LL39; _LL49:*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))='\\';*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='r';
goto _LL39; _LL51:*(( unsigned char*) _check_unknown_subscript( t, sizeof(
unsigned char), j ++))='\\';*(( unsigned char*) _check_unknown_subscript( t,
sizeof( unsigned char), j ++))='t'; goto _LL39; _LL53:*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))='\\';*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='v';
goto _LL39; _LL55:*(( unsigned char*) _check_unknown_subscript( t, sizeof(
unsigned char), j ++))='\\';*(( unsigned char*) _check_unknown_subscript( t,
sizeof( unsigned char), j ++))='\\'; goto _LL39; _LL57:*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))='\\';*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='"';
goto _LL39; _LL59: if( _temp38 >=' '? _temp38 <='~': 0){*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))= _temp38;} else{*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='\\';*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))=(
unsigned char)('0' +( _temp38 >> 6 & 7));*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))=( unsigned char)('0'
+( _temp38 >> 3 & 7));*(( unsigned char*) _check_unknown_subscript( t, sizeof(
unsigned char), j ++))=( unsigned char)('0' +( _temp38 & 7));} goto _LL39; _LL39:;}}
return( struct _tagged_arr) t;}}}} static unsigned char _temp60[ 9u]="restrict";
static struct _tagged_arr Cyc_Absynpp_restrict_string={ _temp60, _temp60,
_temp60 + 9u}; static unsigned char _temp61[ 9u]="volatile"; static struct
_tagged_arr Cyc_Absynpp_volatile_string={ _temp61, _temp61, _temp61 + 9u};
static unsigned char _temp62[ 6u]="const"; static struct _tagged_arr Cyc_Absynpp_const_str={
_temp62, _temp62, _temp62 + 6u}; static struct _tagged_arr* Cyc_Absynpp_restrict_sp=&
Cyc_Absynpp_restrict_string; static struct _tagged_arr* Cyc_Absynpp_volatile_sp=&
Cyc_Absynpp_volatile_string; static struct _tagged_arr* Cyc_Absynpp_const_sp=&
Cyc_Absynpp_const_str; struct Cyc_PP_Doc* Cyc_Absynpp_tqual2doc( struct Cyc_Absyn_Tqual
tq){ struct Cyc_List_List* l= 0; if( tq.q_restrict){ l=({ struct Cyc_List_List*
_temp63=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp63->hd=( void*) Cyc_Absynpp_restrict_sp; _temp63->tl= l; _temp63;});} if(
tq.q_volatile){ l=({ struct Cyc_List_List* _temp64=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp64->hd=( void*) Cyc_Absynpp_volatile_sp;
_temp64->tl= l; _temp64;});} if( tq.q_const){ l=({ struct Cyc_List_List* _temp65=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp65->hd=(
void*) Cyc_Absynpp_const_sp; _temp65->tl= l; _temp65;});} return Cyc_PP_egroup(
_tag_arr("", sizeof( unsigned char), 1u), _tag_arr(" ", sizeof( unsigned char),
2u), _tag_arr(" ", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tagged_arr*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_PP_textptr, l));} struct _tagged_arr Cyc_Absynpp_kind2string( void* k){ void*
_temp66= k; _LL68: if( _temp66 ==( void*) Cyc_Absyn_AnyKind){ goto _LL69;} else{
goto _LL70;} _LL70: if( _temp66 ==( void*) Cyc_Absyn_MemKind){ goto _LL71;}
else{ goto _LL72;} _LL72: if( _temp66 ==( void*) Cyc_Absyn_BoxKind){ goto _LL73;}
else{ goto _LL74;} _LL74: if( _temp66 ==( void*) Cyc_Absyn_RgnKind){ goto _LL75;}
else{ goto _LL76;} _LL76: if( _temp66 ==( void*) Cyc_Absyn_EffKind){ goto _LL77;}
else{ goto _LL67;} _LL69: return _tag_arr("A", sizeof( unsigned char), 2u);
_LL71: return _tag_arr("M", sizeof( unsigned char), 2u); _LL73: return _tag_arr("B",
sizeof( unsigned char), 2u); _LL75: return _tag_arr("R", sizeof( unsigned char),
2u); _LL77: return _tag_arr("E", sizeof( unsigned char), 2u); _LL67:;} struct
_tagged_arr Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref* c){ void* _temp78=(
void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
c))->v; void* _temp84; _LL80: if(( unsigned int) _temp78 > 1u?*(( int*) _temp78)
== Cyc_Absyn_Eq_constr: 0){ _LL85: _temp84=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp78)->f1; goto _LL81;} else{ goto _LL82;} _LL82: goto _LL83; _LL81: return
Cyc_Absynpp_kind2string( _temp84); _LL83: return _tag_arr("?", sizeof(
unsigned char), 2u); _LL79:;} struct Cyc_PP_Doc* Cyc_Absynpp_kind2doc( void* k){
return Cyc_PP_text( Cyc_Absynpp_kind2string( k));} struct Cyc_PP_Doc* Cyc_Absynpp_ckind2doc(
struct Cyc_Absyn_Conref* c){ return Cyc_PP_text( Cyc_Absynpp_ckind2string( c));}
struct Cyc_PP_Doc* Cyc_Absynpp_tps2doc( struct Cyc_List_List* ts){ return Cyc_PP_egroup(
_tag_arr("<", sizeof( unsigned char), 2u), _tag_arr(">", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_typ2doc,
ts));} struct Cyc_PP_Doc* Cyc_Absynpp_ktvar2doc( struct Cyc_Absyn_Tvar* tv){
void* _temp86=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( tv->kind))->v; void* _temp94; void* _temp96;
_LL88: if(( unsigned int) _temp86 > 1u?*(( int*) _temp86) == Cyc_Absyn_Eq_constr:
0){ _LL95: _temp94=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp86)->f1;
if( _temp94 ==( void*) Cyc_Absyn_BoxKind){ goto _LL89;} else{ goto _LL90;}}
else{ goto _LL90;} _LL90: if(( unsigned int) _temp86 > 1u?*(( int*) _temp86) ==
Cyc_Absyn_Eq_constr: 0){ _LL97: _temp96=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp86)->f1; goto _LL91;} else{ goto _LL92;} _LL92: goto _LL93; _LL89: return
Cyc_PP_textptr( tv->name); _LL91: return({ struct Cyc_PP_Doc*(* _temp98)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp101= Cyc_PP_textptr( tv->name);
struct Cyc_PP_Doc* _temp102= Cyc_PP_text( _tag_arr("::", sizeof( unsigned char),
3u)); struct Cyc_PP_Doc* _temp103= Cyc_Absynpp_kind2doc( _temp96); struct Cyc_PP_Doc*
_temp99[ 3u]={ _temp101, _temp102, _temp103}; struct _tagged_arr _temp100={(
void*) _temp99,( void*) _temp99,( void*)( _temp99 + 3u)}; _temp98( _temp100);});
_LL93: return({ struct Cyc_PP_Doc*(* _temp104)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp107= Cyc_PP_textptr( tv->name); struct Cyc_PP_Doc*
_temp108= Cyc_PP_text( _tag_arr("/*::?*/", sizeof( unsigned char), 8u)); struct
Cyc_PP_Doc* _temp105[ 2u]={ _temp107, _temp108}; struct _tagged_arr _temp106={(
void*) _temp105,( void*) _temp105,( void*)( _temp105 + 2u)}; _temp104( _temp106);});
_LL87:;} struct Cyc_PP_Doc* Cyc_Absynpp_ktvars2doc( struct Cyc_List_List* tvs){
return Cyc_PP_egroup( _tag_arr("<", sizeof( unsigned char), 2u), _tag_arr(">",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_ktvar2doc, tvs));} static struct _tagged_arr* Cyc_Absynpp_get_name(
struct Cyc_Absyn_Tvar* tv){ return tv->name;} struct Cyc_PP_Doc* Cyc_Absynpp_tvars2doc(
struct Cyc_List_List* tvs){ if( Cyc_Absynpp_print_all_kinds){ return Cyc_Absynpp_ktvars2doc(
tvs);} return Cyc_PP_egroup( _tag_arr("<", sizeof( unsigned char), 2u), _tag_arr(">",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tagged_arr*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_PP_textptr,(( struct Cyc_List_List*(*)( struct
_tagged_arr*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_get_name, tvs)));} struct _tuple7{ struct Cyc_Absyn_Tqual f1; void*
f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_arg2doc( struct _tuple7* t){ return Cyc_Absynpp_tqtd2doc((*
t).f1,(* t).f2, 0);} struct Cyc_PP_Doc* Cyc_Absynpp_args2doc( struct Cyc_List_List*
ts){ return Cyc_PP_group( _tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple7*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg2doc, ts));} struct Cyc_PP_Doc* Cyc_Absynpp_noncallatt2doc(
void* att){ void* _temp109= att; _LL111: if( _temp109 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL112;} else{ goto _LL113;} _LL113: if( _temp109 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL114;} else{ goto _LL115;} _LL115: if( _temp109 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL116;} else{ goto _LL117;} _LL117: goto _LL118; _LL112: return Cyc_PP_nil_doc();
_LL114: return Cyc_PP_nil_doc(); _LL116: return Cyc_PP_nil_doc(); _LL118: return
Cyc_PP_text( Cyc_Absyn_attribute2string( att)); _LL110:;} struct Cyc_PP_Doc* Cyc_Absynpp_callconv2doc(
struct Cyc_List_List* atts){ for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp119=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; _LL121: if( _temp119 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL122;} else{ goto _LL123;} _LL123: if( _temp119 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL124;} else{ goto _LL125;} _LL125: if( _temp119 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL126;} else{ goto _LL127;} _LL127: goto _LL128; _LL122: return Cyc_PP_text(
_tag_arr("_stdcall ", sizeof( unsigned char), 10u)); _LL124: return Cyc_PP_text(
_tag_arr("_cdecl ", sizeof( unsigned char), 8u)); _LL126: return Cyc_PP_text(
_tag_arr("_fastcall ", sizeof( unsigned char), 11u)); _LL128: goto _LL120;
_LL120:;} return Cyc_PP_nil_doc();} struct Cyc_PP_Doc* Cyc_Absynpp_noncallconv2doc(
struct Cyc_List_List* atts){ int hasatt= 0;{ struct Cyc_List_List* atts2= atts;
for( 0; atts2 != 0; atts2=(( struct Cyc_List_List*) _check_null( atts2))->tl){
void* _temp129=( void*)(( struct Cyc_List_List*) _check_null( atts2))->hd;
_LL131: if( _temp129 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL132;} else{ goto
_LL133;} _LL133: if( _temp129 ==( void*) Cyc_Absyn_Cdecl_att){ goto _LL134;}
else{ goto _LL135;} _LL135: if( _temp129 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL136;} else{ goto _LL137;} _LL137: goto _LL138; _LL132: goto _LL130;
_LL134: goto _LL130; _LL136: goto _LL130; _LL138: hasatt= 1; goto _LL130; _LL130:;}}
if( ! hasatt){ return Cyc_PP_nil_doc();} return({ struct Cyc_PP_Doc*(* _temp139)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp142= Cyc_PP_text(
_tag_arr("__declspec(", sizeof( unsigned char), 12u)); struct Cyc_PP_Doc*
_temp143= Cyc_PP_group( _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("",
sizeof( unsigned char), 1u), _tag_arr(" ", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x))
Cyc_List_map)( Cyc_Absynpp_noncallatt2doc, atts)); struct Cyc_PP_Doc* _temp144=
Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp140[ 3u]={ _temp142, _temp143, _temp144}; struct _tagged_arr _temp141={(
void*) _temp140,( void*) _temp140,( void*)( _temp140 + 3u)}; _temp139( _temp141);});}
struct Cyc_PP_Doc* Cyc_Absynpp_att2doc( void* a){ return Cyc_PP_text( Cyc_Absyn_attribute2string(
a));} struct Cyc_PP_Doc* Cyc_Absynpp_atts2doc( struct Cyc_List_List* atts){ if(
atts == 0){ return Cyc_PP_nil_doc();} if( Cyc_Absynpp_to_VC){ return Cyc_Absynpp_noncallconv2doc(
atts);} return({ struct Cyc_PP_Doc*(* _temp145)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp148= Cyc_PP_text( _tag_arr(" __attribute__", sizeof(
unsigned char), 15u)); struct Cyc_PP_Doc* _temp149= Cyc_PP_group( _tag_arr("((",
sizeof( unsigned char), 3u), _tag_arr("))", sizeof( unsigned char), 3u),
_tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct
Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_att2doc,
atts)); struct Cyc_PP_Doc* _temp146[ 2u]={ _temp148, _temp149}; struct
_tagged_arr _temp147={( void*) _temp146,( void*) _temp146,( void*)( _temp146 + 2u)};
_temp145( _temp147);});} int Cyc_Absynpp_next_is_pointer( struct Cyc_List_List*
tms){ if( tms == 0){ return 0;}{ void* _temp150=( void*)(( struct Cyc_List_List*)
_check_null( tms))->hd; struct Cyc_Absyn_Tqual _temp156; void* _temp158; void*
_temp160; _LL152: if(( unsigned int) _temp150 > 1u?*(( int*) _temp150) == Cyc_Absyn_Pointer_mod:
0){ _LL161: _temp160=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp150)->f1;
goto _LL159; _LL159: _temp158=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp150)->f2; goto _LL157; _LL157: _temp156=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp150)->f3; goto _LL153;} else{ goto _LL154;} _LL154: goto _LL155; _LL153:
return 1; _LL155: return 0; _LL151:;}} extern struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc(
void* t); struct Cyc_PP_Doc* Cyc_Absynpp_dtms2doc( struct Cyc_PP_Doc* d, struct
Cyc_List_List* tms){ if( tms == 0){ return d;}{ struct Cyc_PP_Doc* rest= Cyc_Absynpp_dtms2doc(
d,(( struct Cyc_List_List*) _check_null( tms))->tl); struct Cyc_PP_Doc* p_rest=({
struct Cyc_PP_Doc*(* _temp297)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp300= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp301= rest; struct Cyc_PP_Doc* _temp302= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp298[ 3u]={ _temp300, _temp301,
_temp302}; struct _tagged_arr _temp299={( void*) _temp298,( void*) _temp298,(
void*)( _temp298 + 3u)}; _temp297( _temp299);}); void* _temp162=( void*)((
struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Exp* _temp176;
void* _temp178; struct Cyc_List_List* _temp180; struct Cyc_Position_Segment*
_temp182; int _temp184; struct Cyc_Position_Segment* _temp186; struct Cyc_List_List*
_temp188; struct Cyc_Absyn_Tqual _temp190; void* _temp192; void* _temp194;
_LL164: if( _temp162 ==( void*) Cyc_Absyn_Carray_mod){ goto _LL165;} else{ goto
_LL166;} _LL166: if(( unsigned int) _temp162 > 1u?*(( int*) _temp162) == Cyc_Absyn_ConstArray_mod:
0){ _LL177: _temp176=(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp162)->f1;
goto _LL167;} else{ goto _LL168;} _LL168: if(( unsigned int) _temp162 > 1u?*((
int*) _temp162) == Cyc_Absyn_Function_mod: 0){ _LL179: _temp178=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp162)->f1; goto _LL169;} else{ goto
_LL170;} _LL170: if(( unsigned int) _temp162 > 1u?*(( int*) _temp162) == Cyc_Absyn_Attributes_mod:
0){ _LL183: _temp182=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp162)->f1;
goto _LL181; _LL181: _temp180=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp162)->f2; goto _LL171;} else{ goto _LL172;} _LL172: if(( unsigned int)
_temp162 > 1u?*(( int*) _temp162) == Cyc_Absyn_TypeParams_mod: 0){ _LL189:
_temp188=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp162)->f1; goto _LL187;
_LL187: _temp186=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp162)->f2; goto
_LL185; _LL185: _temp184=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp162)->f3;
goto _LL173;} else{ goto _LL174;} _LL174: if(( unsigned int) _temp162 > 1u?*((
int*) _temp162) == Cyc_Absyn_Pointer_mod: 0){ _LL195: _temp194=( void*)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp162)->f1; goto _LL193; _LL193: _temp192=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp162)->f2; goto _LL191;
_LL191: _temp190=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp162)->f3; goto
_LL175;} else{ goto _LL163;} _LL165: if( Cyc_Absynpp_next_is_pointer((( struct
Cyc_List_List*) _check_null( tms))->tl)){ rest= p_rest;} return({ struct Cyc_PP_Doc*(*
_temp196)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp199= rest;
struct Cyc_PP_Doc* _temp200= Cyc_PP_text( _tag_arr("[]", sizeof( unsigned char),
3u)); struct Cyc_PP_Doc* _temp197[ 2u]={ _temp199, _temp200}; struct _tagged_arr
_temp198={( void*) _temp197,( void*) _temp197,( void*)( _temp197 + 2u)};
_temp196( _temp198);}); _LL167: if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*)
_check_null( tms))->tl)){ rest= p_rest;} return({ struct Cyc_PP_Doc*(* _temp201)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp204= rest; struct Cyc_PP_Doc*
_temp205= Cyc_PP_text( _tag_arr("[", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp206= Cyc_Absynpp_exp2doc( _temp176); struct Cyc_PP_Doc* _temp207= Cyc_PP_text(
_tag_arr("]", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp202[ 4u]={
_temp204, _temp205, _temp206, _temp207}; struct _tagged_arr _temp203={( void*)
_temp202,( void*) _temp202,( void*)( _temp202 + 4u)}; _temp201( _temp203);});
_LL169: if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*) _check_null(
tms))->tl)){ rest= p_rest;}{ void* _temp208= _temp178; struct Cyc_List_List*
_temp214; struct Cyc_Core_Opt* _temp216; struct Cyc_Absyn_VarargInfo* _temp218;
int _temp220; struct Cyc_List_List* _temp222; struct Cyc_Position_Segment*
_temp224; struct Cyc_List_List* _temp226; _LL210: if(*(( int*) _temp208) == Cyc_Absyn_WithTypes){
_LL223: _temp222=(( struct Cyc_Absyn_WithTypes_struct*) _temp208)->f1; goto
_LL221; _LL221: _temp220=(( struct Cyc_Absyn_WithTypes_struct*) _temp208)->f2;
goto _LL219; _LL219: _temp218=(( struct Cyc_Absyn_WithTypes_struct*) _temp208)->f3;
goto _LL217; _LL217: _temp216=(( struct Cyc_Absyn_WithTypes_struct*) _temp208)->f4;
goto _LL215; _LL215: _temp214=(( struct Cyc_Absyn_WithTypes_struct*) _temp208)->f5;
goto _LL211;} else{ goto _LL212;} _LL212: if(*(( int*) _temp208) == Cyc_Absyn_NoTypes){
_LL227: _temp226=(( struct Cyc_Absyn_NoTypes_struct*) _temp208)->f1; goto _LL225;
_LL225: _temp224=(( struct Cyc_Absyn_NoTypes_struct*) _temp208)->f2; goto _LL213;}
else{ goto _LL209;} _LL211: return({ struct Cyc_PP_Doc*(* _temp228)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp231= rest; struct Cyc_PP_Doc*
_temp232= Cyc_Absynpp_funargs2doc( _temp222, _temp220, _temp218, _temp216,
_temp214); struct Cyc_PP_Doc* _temp229[ 2u]={ _temp231, _temp232}; struct
_tagged_arr _temp230={( void*) _temp229,( void*) _temp229,( void*)( _temp229 + 2u)};
_temp228( _temp230);}); _LL213: return({ struct Cyc_PP_Doc*(* _temp233)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp236= rest; struct Cyc_PP_Doc*
_temp237= Cyc_PP_group( _tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tagged_arr*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_PP_textptr, _temp226)); struct Cyc_PP_Doc* _temp234[ 2u]={
_temp236, _temp237}; struct _tagged_arr _temp235={( void*) _temp234,( void*)
_temp234,( void*)( _temp234 + 2u)}; _temp233( _temp235);}); _LL209:;} _LL171:
if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*) _check_null( tms))->tl)){
rest= p_rest;} return({ struct Cyc_PP_Doc*(* _temp238)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp241= rest; struct Cyc_PP_Doc* _temp242= Cyc_Absynpp_atts2doc(
_temp180); struct Cyc_PP_Doc* _temp239[ 2u]={ _temp241, _temp242}; struct
_tagged_arr _temp240={( void*) _temp239,( void*) _temp239,( void*)( _temp239 + 2u)};
_temp238( _temp240);}); _LL173: if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*)
_check_null( tms))->tl)){ rest= p_rest;} if( _temp184){ return({ struct Cyc_PP_Doc*(*
_temp243)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp246= rest;
struct Cyc_PP_Doc* _temp247= Cyc_Absynpp_ktvars2doc( _temp188); struct Cyc_PP_Doc*
_temp244[ 2u]={ _temp246, _temp247}; struct _tagged_arr _temp245={( void*)
_temp244,( void*) _temp244,( void*)( _temp244 + 2u)}; _temp243( _temp245);});}
else{ return({ struct Cyc_PP_Doc*(* _temp248)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp251= rest; struct Cyc_PP_Doc* _temp252= Cyc_Absynpp_tvars2doc(
_temp188); struct Cyc_PP_Doc* _temp249[ 2u]={ _temp251, _temp252}; struct
_tagged_arr _temp250={( void*) _temp249,( void*) _temp249,( void*)( _temp249 + 2u)};
_temp248( _temp250);});} _LL175: { struct Cyc_PP_Doc* ptr;{ void* _temp253=
_temp194; struct Cyc_Absyn_Exp* _temp261; struct Cyc_Absyn_Exp* _temp263; _LL255:
if(( unsigned int) _temp253 > 1u?*(( int*) _temp253) == Cyc_Absyn_Nullable_ps: 0){
_LL262: _temp261=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp253)->f1; goto
_LL256;} else{ goto _LL257;} _LL257: if(( unsigned int) _temp253 > 1u?*(( int*)
_temp253) == Cyc_Absyn_NonNullable_ps: 0){ _LL264: _temp263=(( struct Cyc_Absyn_NonNullable_ps_struct*)
_temp253)->f1; goto _LL258;} else{ goto _LL259;} _LL259: if( _temp253 ==( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL260;} else{ goto _LL254;} _LL256: if( Cyc_Evexp_eval_const_uint_exp(
_temp261) == 1){ ptr= Cyc_PP_text( _tag_arr("*", sizeof( unsigned char), 2u));}
else{ ptr=({ struct Cyc_PP_Doc*(* _temp265)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp268= Cyc_PP_text( _tag_arr("*", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp269= Cyc_PP_text( _tag_arr("{", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp270= Cyc_Absynpp_exp2doc( _temp261);
struct Cyc_PP_Doc* _temp271= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp266[ 4u]={ _temp268, _temp269, _temp270, _temp271};
struct _tagged_arr _temp267={( void*) _temp266,( void*) _temp266,( void*)(
_temp266 + 4u)}; _temp265( _temp267);});} goto _LL254; _LL258: if( Cyc_Evexp_eval_const_uint_exp(
_temp263) == 1){ ptr= Cyc_PP_text( _tag_arr("@", sizeof( unsigned char), 2u));}
else{ ptr=({ struct Cyc_PP_Doc*(* _temp272)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp275= Cyc_PP_text( _tag_arr("@", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp276= Cyc_PP_text( _tag_arr("{", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp277= Cyc_Absynpp_exp2doc( _temp263);
struct Cyc_PP_Doc* _temp278= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp273[ 4u]={ _temp275, _temp276, _temp277, _temp278};
struct _tagged_arr _temp274={( void*) _temp273,( void*) _temp273,( void*)(
_temp273 + 4u)}; _temp272( _temp274);});} goto _LL254; _LL260: ptr= Cyc_PP_text(
_tag_arr("?", sizeof( unsigned char), 2u)); goto _LL254; _LL254:;}{ void*
_temp279= Cyc_Tcutil_compress( _temp192); _LL281: if( _temp279 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL282;} else{ goto _LL283;} _LL283: goto _LL284; _LL282: return({ struct
Cyc_PP_Doc*(* _temp285)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp288= ptr; struct Cyc_PP_Doc* _temp289= rest; struct Cyc_PP_Doc* _temp286[ 2u]={
_temp288, _temp289}; struct _tagged_arr _temp287={( void*) _temp286,( void*)
_temp286,( void*)( _temp286 + 2u)}; _temp285( _temp287);}); _LL284: return({
struct Cyc_PP_Doc*(* _temp290)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp293= ptr; struct Cyc_PP_Doc* _temp294= Cyc_Absynpp_ntyp2doc( _temp192);
struct Cyc_PP_Doc* _temp295= Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp296= rest; struct Cyc_PP_Doc* _temp291[ 4u]={
_temp293, _temp294, _temp295, _temp296}; struct _tagged_arr _temp292={( void*)
_temp291,( void*) _temp291,( void*)( _temp291 + 4u)}; _temp290( _temp292);});
_LL280:;}} _LL163:;}} struct Cyc_PP_Doc* Cyc_Absynpp_rgn2doc( void* t){ void*
_temp303= Cyc_Tcutil_compress( t); _LL305: if( _temp303 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL306;} else{ goto _LL307;} _LL307: goto _LL308; _LL306: return Cyc_PP_text(
_tag_arr("`H", sizeof( unsigned char), 3u)); _LL308: return Cyc_Absynpp_ntyp2doc(
t); _LL304:;} static void Cyc_Absynpp_effects2docs( struct Cyc_List_List**
regions, struct Cyc_List_List** effects, void* t){ void* _temp309= Cyc_Tcutil_compress(
t); void* _temp317; struct Cyc_List_List* _temp319; _LL311: if(( unsigned int)
_temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_AccessEff: 0){ _LL318: _temp317=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp309)->f1; goto _LL312;} else{
goto _LL313;} _LL313: if(( unsigned int) _temp309 > 4u?*(( int*) _temp309) ==
Cyc_Absyn_JoinEff: 0){ _LL320: _temp319=(( struct Cyc_Absyn_JoinEff_struct*)
_temp309)->f1; goto _LL314;} else{ goto _LL315;} _LL315: goto _LL316; _LL312:*
regions=({ struct Cyc_List_List* _temp321=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp321->hd=( void*) Cyc_Absynpp_rgn2doc(
_temp317); _temp321->tl=* regions; _temp321;}); goto _LL310; _LL314: for( 0;
_temp319 != 0; _temp319=(( struct Cyc_List_List*) _check_null( _temp319))->tl){
Cyc_Absynpp_effects2docs( regions, effects,( void*)(( struct Cyc_List_List*)
_check_null( _temp319))->hd);} goto _LL310; _LL316:* effects=({ struct Cyc_List_List*
_temp322=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp322->hd=( void*) Cyc_Absynpp_ntyp2doc( t); _temp322->tl=* effects; _temp322;});
goto _LL310; _LL310:;} struct Cyc_PP_Doc* Cyc_Absynpp_eff2doc( void* t){ struct
Cyc_List_List* regions= 0; struct Cyc_List_List* effects= 0; Cyc_Absynpp_effects2docs(&
regions,& effects, t); regions=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( regions); effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( effects); if( regions == 0? effects != 0: 0){ return Cyc_PP_group(
_tag_arr("", sizeof( unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr("+", sizeof( unsigned char), 2u), effects);} else{ struct Cyc_PP_Doc*
_temp323= Cyc_PP_group( _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u), regions);
return Cyc_PP_group( _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("",
sizeof( unsigned char), 1u), _tag_arr("+", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
effects,({ struct Cyc_List_List* _temp324=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp324->hd=( void*) _temp323; _temp324->tl= 0;
_temp324;})));}} struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc( void* t){ struct Cyc_PP_Doc*
s;{ void* _temp325= t; struct Cyc_Absyn_Exp* _temp371; struct Cyc_Absyn_Tqual
_temp373; void* _temp375; struct Cyc_Absyn_FnInfo _temp377; struct Cyc_Absyn_PtrInfo
_temp379; struct Cyc_Core_Opt* _temp381; int _temp383; struct Cyc_Core_Opt*
_temp385; struct Cyc_Core_Opt* _temp387; struct Cyc_Absyn_Tvar* _temp389; struct
Cyc_Absyn_TunionInfo _temp391; void* _temp393; struct Cyc_List_List* _temp395;
void* _temp397; struct Cyc_Absyn_TunionFieldInfo _temp399; struct Cyc_List_List*
_temp401; void* _temp403; void* _temp405; void* _temp407; struct Cyc_List_List*
_temp409; struct Cyc_Absyn_Structdecl** _temp411; struct Cyc_List_List* _temp413;
struct _tuple0* _temp415; struct Cyc_Absyn_Uniondecl** _temp417; struct Cyc_List_List*
_temp419; struct _tuple0* _temp421; struct Cyc_List_List* _temp423; struct Cyc_List_List*
_temp425; struct Cyc_Absyn_Enumdecl* _temp427; struct _tuple0* _temp429; struct
Cyc_Core_Opt* _temp431; struct Cyc_List_List* _temp433; struct _tuple0* _temp435;
void* _temp437; void* _temp439; struct Cyc_List_List* _temp441; _LL327: if((
unsigned int) _temp325 > 4u?*(( int*) _temp325) == Cyc_Absyn_ArrayType: 0){
_LL376: _temp375=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp325)->f1;
goto _LL374; _LL374: _temp373=(( struct Cyc_Absyn_ArrayType_struct*) _temp325)->f2;
goto _LL372; _LL372: _temp371=(( struct Cyc_Absyn_ArrayType_struct*) _temp325)->f3;
goto _LL328;} else{ goto _LL329;} _LL329: if(( unsigned int) _temp325 > 4u?*((
int*) _temp325) == Cyc_Absyn_FnType: 0){ _LL378: _temp377=(( struct Cyc_Absyn_FnType_struct*)
_temp325)->f1; goto _LL330;} else{ goto _LL331;} _LL331: if(( unsigned int)
_temp325 > 4u?*(( int*) _temp325) == Cyc_Absyn_PointerType: 0){ _LL380: _temp379=((
struct Cyc_Absyn_PointerType_struct*) _temp325)->f1; goto _LL332;} else{ goto
_LL333;} _LL333: if( _temp325 ==( void*) Cyc_Absyn_VoidType){ goto _LL334;}
else{ goto _LL335;} _LL335: if(( unsigned int) _temp325 > 4u?*(( int*) _temp325)
== Cyc_Absyn_Evar: 0){ _LL388: _temp387=(( struct Cyc_Absyn_Evar_struct*)
_temp325)->f1; goto _LL386; _LL386: _temp385=(( struct Cyc_Absyn_Evar_struct*)
_temp325)->f2; goto _LL384; _LL384: _temp383=(( struct Cyc_Absyn_Evar_struct*)
_temp325)->f3; goto _LL382; _LL382: _temp381=(( struct Cyc_Absyn_Evar_struct*)
_temp325)->f4; goto _LL336;} else{ goto _LL337;} _LL337: if(( unsigned int)
_temp325 > 4u?*(( int*) _temp325) == Cyc_Absyn_VarType: 0){ _LL390: _temp389=((
struct Cyc_Absyn_VarType_struct*) _temp325)->f1; goto _LL338;} else{ goto _LL339;}
_LL339: if(( unsigned int) _temp325 > 4u?*(( int*) _temp325) == Cyc_Absyn_TunionType:
0){ _LL392: _temp391=(( struct Cyc_Absyn_TunionType_struct*) _temp325)->f1;
_LL398: _temp397=( void*) _temp391.tunion_info; goto _LL396; _LL396: _temp395=
_temp391.targs; goto _LL394; _LL394: _temp393=( void*) _temp391.rgn; goto _LL340;}
else{ goto _LL341;} _LL341: if(( unsigned int) _temp325 > 4u?*(( int*) _temp325)
== Cyc_Absyn_TunionFieldType: 0){ _LL400: _temp399=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp325)->f1; _LL404: _temp403=( void*) _temp399.field_info; goto _LL402;
_LL402: _temp401= _temp399.targs; goto _LL342;} else{ goto _LL343;} _LL343: if((
unsigned int) _temp325 > 4u?*(( int*) _temp325) == Cyc_Absyn_IntType: 0){ _LL408:
_temp407=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp325)->f1; goto _LL406;
_LL406: _temp405=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp325)->f2;
goto _LL344;} else{ goto _LL345;} _LL345: if( _temp325 ==( void*) Cyc_Absyn_FloatType){
goto _LL346;} else{ goto _LL347;} _LL347: if( _temp325 ==( void*) Cyc_Absyn_DoubleType){
goto _LL348;} else{ goto _LL349;} _LL349: if(( unsigned int) _temp325 > 4u?*((
int*) _temp325) == Cyc_Absyn_TupleType: 0){ _LL410: _temp409=(( struct Cyc_Absyn_TupleType_struct*)
_temp325)->f1; goto _LL350;} else{ goto _LL351;} _LL351: if(( unsigned int)
_temp325 > 4u?*(( int*) _temp325) == Cyc_Absyn_StructType: 0){ _LL416: _temp415=((
struct Cyc_Absyn_StructType_struct*) _temp325)->f1; goto _LL414; _LL414:
_temp413=(( struct Cyc_Absyn_StructType_struct*) _temp325)->f2; goto _LL412;
_LL412: _temp411=(( struct Cyc_Absyn_StructType_struct*) _temp325)->f3; goto
_LL352;} else{ goto _LL353;} _LL353: if(( unsigned int) _temp325 > 4u?*(( int*)
_temp325) == Cyc_Absyn_UnionType: 0){ _LL422: _temp421=(( struct Cyc_Absyn_UnionType_struct*)
_temp325)->f1; goto _LL420; _LL420: _temp419=(( struct Cyc_Absyn_UnionType_struct*)
_temp325)->f2; goto _LL418; _LL418: _temp417=(( struct Cyc_Absyn_UnionType_struct*)
_temp325)->f3; goto _LL354;} else{ goto _LL355;} _LL355: if(( unsigned int)
_temp325 > 4u?*(( int*) _temp325) == Cyc_Absyn_AnonStructType: 0){ _LL424:
_temp423=(( struct Cyc_Absyn_AnonStructType_struct*) _temp325)->f1; goto _LL356;}
else{ goto _LL357;} _LL357: if(( unsigned int) _temp325 > 4u?*(( int*) _temp325)
== Cyc_Absyn_AnonUnionType: 0){ _LL426: _temp425=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp325)->f1; goto _LL358;} else{ goto _LL359;} _LL359: if(( unsigned int)
_temp325 > 4u?*(( int*) _temp325) == Cyc_Absyn_EnumType: 0){ _LL430: _temp429=((
struct Cyc_Absyn_EnumType_struct*) _temp325)->f1; goto _LL428; _LL428: _temp427=((
struct Cyc_Absyn_EnumType_struct*) _temp325)->f2; goto _LL360;} else{ goto
_LL361;} _LL361: if(( unsigned int) _temp325 > 4u?*(( int*) _temp325) == Cyc_Absyn_TypedefType:
0){ _LL436: _temp435=(( struct Cyc_Absyn_TypedefType_struct*) _temp325)->f1;
goto _LL434; _LL434: _temp433=(( struct Cyc_Absyn_TypedefType_struct*) _temp325)->f2;
goto _LL432; _LL432: _temp431=(( struct Cyc_Absyn_TypedefType_struct*) _temp325)->f3;
goto _LL362;} else{ goto _LL363;} _LL363: if(( unsigned int) _temp325 > 4u?*((
int*) _temp325) == Cyc_Absyn_RgnHandleType: 0){ _LL438: _temp437=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp325)->f1; goto _LL364;} else{ goto
_LL365;} _LL365: if( _temp325 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL366;} else{
goto _LL367;} _LL367: if(( unsigned int) _temp325 > 4u?*(( int*) _temp325) ==
Cyc_Absyn_AccessEff: 0){ _LL440: _temp439=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp325)->f1; goto _LL368;} else{ goto _LL369;} _LL369: if(( unsigned int)
_temp325 > 4u?*(( int*) _temp325) == Cyc_Absyn_JoinEff: 0){ _LL442: _temp441=((
struct Cyc_Absyn_JoinEff_struct*) _temp325)->f1; goto _LL370;} else{ goto _LL326;}
_LL328: return Cyc_PP_text( _tag_arr("[[[array]]]", sizeof( unsigned char), 12u));
_LL330: return Cyc_PP_nil_doc(); _LL332: return Cyc_PP_nil_doc(); _LL334: s= Cyc_PP_text(
_tag_arr("void", sizeof( unsigned char), 5u)); goto _LL326; _LL336: if( _temp385
!= 0){ return Cyc_Absynpp_ntyp2doc(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp385))->v);} else{ s=({ struct Cyc_PP_Doc*(* _temp443)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp446= Cyc_PP_text( _tag_arr("%", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp447= Cyc_PP_text( _tag_arr("(",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp448= Cyc_PP_text(( struct
_tagged_arr) xprintf("%d", _temp383)); struct Cyc_PP_Doc* _temp449=( ! Cyc_Absynpp_print_full_evars?
1: _temp381 == 0)? Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_tvars2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp381))->v);
struct Cyc_PP_Doc* _temp450= Cyc_PP_text( _tag_arr(")::", sizeof( unsigned char),
4u)); struct Cyc_PP_Doc* _temp451= _temp387 == 0? Cyc_PP_text( _tag_arr("?",
sizeof( unsigned char), 2u)): Cyc_Absynpp_kind2doc(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp387))->v); struct Cyc_PP_Doc* _temp444[ 6u]={ _temp446,
_temp447, _temp448, _temp449, _temp450, _temp451}; struct _tagged_arr _temp445={(
void*) _temp444,( void*) _temp444,( void*)( _temp444 + 6u)}; _temp443( _temp445);});}
goto _LL326; _LL338: s= Cyc_PP_textptr( _temp389->name); if( Cyc_Absynpp_print_all_kinds){
s=({ struct Cyc_PP_Doc*(* _temp452)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp455= s; struct Cyc_PP_Doc* _temp456= Cyc_PP_text( _tag_arr("::", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp457= Cyc_Absynpp_ckind2doc(
_temp389->kind); struct Cyc_PP_Doc* _temp453[ 3u]={ _temp455, _temp456, _temp457};
struct _tagged_arr _temp454={( void*) _temp453,( void*) _temp453,( void*)(
_temp453 + 3u)}; _temp452( _temp454);});} if( Cyc_Absynpp_rewrite_temp_tvars?
Cyc_Tcutil_is_temp_tvar( _temp389): 0){ s=({ struct Cyc_PP_Doc*(* _temp458)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp461= Cyc_PP_text(
_tag_arr("_ /* ", sizeof( unsigned char), 6u)); struct Cyc_PP_Doc* _temp462= s;
struct Cyc_PP_Doc* _temp463= Cyc_PP_text( _tag_arr(" */", sizeof( unsigned char),
4u)); struct Cyc_PP_Doc* _temp459[ 3u]={ _temp461, _temp462, _temp463}; struct
_tagged_arr _temp460={( void*) _temp459,( void*) _temp459,( void*)( _temp459 + 3u)};
_temp458( _temp460);});} goto _LL326; _LL340:{ void* _temp464= _temp397; struct
Cyc_Absyn_UnknownTunionInfo _temp470; int _temp472; struct _tuple0* _temp474;
struct Cyc_Absyn_Tuniondecl* _temp476; struct Cyc_Absyn_Tuniondecl _temp478; int
_temp479; struct Cyc_Core_Opt* _temp481; struct Cyc_List_List* _temp483; struct
_tuple0* _temp485; void* _temp487; _LL466: if(*(( int*) _temp464) == Cyc_Absyn_UnknownTunion){
_LL471: _temp470=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp464)->f1;
_LL475: _temp474= _temp470.name; goto _LL473; _LL473: _temp472= _temp470.is_xtunion;
goto _LL467;} else{ goto _LL468;} _LL468: if(*(( int*) _temp464) == Cyc_Absyn_KnownTunion){
_LL477: _temp476=(( struct Cyc_Absyn_KnownTunion_struct*) _temp464)->f1;
_temp478=* _temp476; _LL488: _temp487=( void*) _temp478.sc; goto _LL486; _LL486:
_temp485= _temp478.name; goto _LL484; _LL484: _temp483= _temp478.tvs; goto
_LL482; _LL482: _temp481= _temp478.fields; goto _LL480; _LL480: _temp479=
_temp478.is_xtunion; goto _LL469;} else{ goto _LL465;} _LL467: _temp485=
_temp474; _temp479= _temp472; goto _LL469; _LL469: { struct Cyc_PP_Doc* _temp489=
Cyc_PP_text( _temp479? _tag_arr("xtunion ", sizeof( unsigned char), 9u):
_tag_arr("tunion ", sizeof( unsigned char), 8u));{ void* _temp490= Cyc_Tcutil_compress(
_temp393); _LL492: if( _temp490 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL493;}
else{ goto _LL494;} _LL494: goto _LL495; _LL493: s=({ struct Cyc_PP_Doc*(*
_temp496)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp499=
_temp489; struct Cyc_PP_Doc* _temp500= Cyc_Absynpp_qvar2doc( _temp485); struct
Cyc_PP_Doc* _temp501= Cyc_Absynpp_tps2doc( _temp395); struct Cyc_PP_Doc*
_temp497[ 3u]={ _temp499, _temp500, _temp501}; struct _tagged_arr _temp498={(
void*) _temp497,( void*) _temp497,( void*)( _temp497 + 3u)}; _temp496( _temp498);});
goto _LL491; _LL495: s=({ struct Cyc_PP_Doc*(* _temp502)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp505= _temp489; struct Cyc_PP_Doc* _temp506=
Cyc_Absynpp_typ2doc( _temp393); struct Cyc_PP_Doc* _temp507= Cyc_PP_text(
_tag_arr(" ", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp508= Cyc_Absynpp_qvar2doc(
_temp485); struct Cyc_PP_Doc* _temp509= Cyc_Absynpp_tps2doc( _temp395); struct
Cyc_PP_Doc* _temp503[ 5u]={ _temp505, _temp506, _temp507, _temp508, _temp509};
struct _tagged_arr _temp504={( void*) _temp503,( void*) _temp503,( void*)(
_temp503 + 5u)}; _temp502( _temp504);}); goto _LL491; _LL491:;} goto _LL465;}
_LL465:;} goto _LL326; _LL342:{ void* _temp510= _temp403; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp516; int _temp518; struct _tuple0* _temp520; struct _tuple0* _temp522;
struct Cyc_Absyn_Tunionfield* _temp524; struct Cyc_Absyn_Tunionfield _temp526;
void* _temp527; struct Cyc_Position_Segment* _temp529; struct Cyc_List_List*
_temp531; struct Cyc_List_List* _temp533; struct _tuple0* _temp535; struct Cyc_Absyn_Tuniondecl*
_temp537; struct Cyc_Absyn_Tuniondecl _temp539; int _temp540; struct Cyc_Core_Opt*
_temp542; struct Cyc_List_List* _temp544; struct _tuple0* _temp546; void*
_temp548; _LL512: if(*(( int*) _temp510) == Cyc_Absyn_UnknownTunionfield){
_LL517: _temp516=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp510)->f1;
_LL523: _temp522= _temp516.tunion_name; goto _LL521; _LL521: _temp520= _temp516.field_name;
goto _LL519; _LL519: _temp518= _temp516.is_xtunion; goto _LL513;} else{ goto
_LL514;} _LL514: if(*(( int*) _temp510) == Cyc_Absyn_KnownTunionfield){ _LL538:
_temp537=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp510)->f1; _temp539=*
_temp537; _LL549: _temp548=( void*) _temp539.sc; goto _LL547; _LL547: _temp546=
_temp539.name; goto _LL545; _LL545: _temp544= _temp539.tvs; goto _LL543; _LL543:
_temp542= _temp539.fields; goto _LL541; _LL541: _temp540= _temp539.is_xtunion;
goto _LL525; _LL525: _temp524=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp510)->f2; _temp526=* _temp524; _LL536: _temp535= _temp526.name; goto _LL534;
_LL534: _temp533= _temp526.tvs; goto _LL532; _LL532: _temp531= _temp526.typs;
goto _LL530; _LL530: _temp529= _temp526.loc; goto _LL528; _LL528: _temp527=(
void*) _temp526.sc; goto _LL515;} else{ goto _LL511;} _LL513: _temp546= _temp522;
_temp540= _temp518; _temp535= _temp520; goto _LL515; _LL515: { struct Cyc_PP_Doc*
_temp550= Cyc_PP_text( _temp540? _tag_arr("xtunion ", sizeof( unsigned char), 9u):
_tag_arr("tunion ", sizeof( unsigned char), 8u)); s=({ struct Cyc_PP_Doc*(*
_temp551)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp554=
_temp550; struct Cyc_PP_Doc* _temp555= Cyc_Absynpp_qvar2doc( _temp546); struct
Cyc_PP_Doc* _temp556= Cyc_PP_text( _tag_arr(".", sizeof( unsigned char), 2u));
struct Cyc_PP_Doc* _temp557= Cyc_Absynpp_qvar2doc( _temp535); struct Cyc_PP_Doc*
_temp552[ 4u]={ _temp554, _temp555, _temp556, _temp557}; struct _tagged_arr
_temp553={( void*) _temp552,( void*) _temp552,( void*)( _temp552 + 4u)};
_temp551( _temp553);}); goto _LL511;} _LL511:;} goto _LL326; _LL344: { struct
_tagged_arr sns; struct _tagged_arr ts;{ void* _temp558= _temp407; _LL560: if(
_temp558 ==( void*) Cyc_Absyn_Signed){ goto _LL561;} else{ goto _LL562;} _LL562:
if( _temp558 ==( void*) Cyc_Absyn_Unsigned){ goto _LL563;} else{ goto _LL559;}
_LL561: sns= _tag_arr("", sizeof( unsigned char), 1u); goto _LL559; _LL563: sns=
_tag_arr("unsigned ", sizeof( unsigned char), 10u); goto _LL559; _LL559:;}{ void*
_temp564= _temp405; _LL566: if( _temp564 ==( void*) Cyc_Absyn_B1){ goto _LL567;}
else{ goto _LL568;} _LL568: if( _temp564 ==( void*) Cyc_Absyn_B2){ goto _LL569;}
else{ goto _LL570;} _LL570: if( _temp564 ==( void*) Cyc_Absyn_B4){ goto _LL571;}
else{ goto _LL572;} _LL572: if( _temp564 ==( void*) Cyc_Absyn_B8){ goto _LL573;}
else{ goto _LL565;} _LL567:{ void* _temp574= _temp407; _LL576: if( _temp574 ==(
void*) Cyc_Absyn_Signed){ goto _LL577;} else{ goto _LL578;} _LL578: if( _temp574
==( void*) Cyc_Absyn_Unsigned){ goto _LL579;} else{ goto _LL575;} _LL577: sns=
_tag_arr("signed ", sizeof( unsigned char), 8u); goto _LL575; _LL579: goto
_LL575; _LL575:;} ts= _tag_arr("char", sizeof( unsigned char), 5u); goto _LL565;
_LL569: ts= _tag_arr("short", sizeof( unsigned char), 6u); goto _LL565; _LL571:
ts= _tag_arr("int", sizeof( unsigned char), 4u); goto _LL565; _LL573: ts= Cyc_Absynpp_to_VC?
_tag_arr("__int64", sizeof( unsigned char), 8u): _tag_arr("long long", sizeof(
unsigned char), 10u); goto _LL565; _LL565:;} s= Cyc_PP_text(( struct _tagged_arr)({
struct _tagged_arr _temp580= sns; struct _tagged_arr _temp581= ts; xprintf("%.*s%.*s",
_get_arr_size( _temp580, 1u), _temp580.curr, _get_arr_size( _temp581, 1u),
_temp581.curr);})); goto _LL326;} _LL346: s= Cyc_PP_text( _tag_arr("float",
sizeof( unsigned char), 6u)); goto _LL326; _LL348: s= Cyc_PP_text( _tag_arr("double",
sizeof( unsigned char), 7u)); goto _LL326; _LL350: s=({ struct Cyc_PP_Doc*(*
_temp582)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp585= Cyc_PP_text(
_tag_arr("$", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp586= Cyc_Absynpp_args2doc(
_temp409); struct Cyc_PP_Doc* _temp583[ 2u]={ _temp585, _temp586}; struct
_tagged_arr _temp584={( void*) _temp583,( void*) _temp583,( void*)( _temp583 + 2u)};
_temp582( _temp584);}); goto _LL326; _LL352: if( _temp415 == 0){ s=({ struct Cyc_PP_Doc*(*
_temp587)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp590= Cyc_PP_text(
_tag_arr("struct ", sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp591=
Cyc_Absynpp_tps2doc( _temp413); struct Cyc_PP_Doc* _temp588[ 2u]={ _temp590,
_temp591}; struct _tagged_arr _temp589={( void*) _temp588,( void*) _temp588,(
void*)( _temp588 + 2u)}; _temp587( _temp589);});} else{ s=({ struct Cyc_PP_Doc*(*
_temp592)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp595= Cyc_PP_text(
_tag_arr("struct ", sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp596=
_temp415 == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*)
_check_null( _temp415)); struct Cyc_PP_Doc* _temp597= Cyc_Absynpp_tps2doc(
_temp413); struct Cyc_PP_Doc* _temp593[ 3u]={ _temp595, _temp596, _temp597};
struct _tagged_arr _temp594={( void*) _temp593,( void*) _temp593,( void*)(
_temp593 + 3u)}; _temp592( _temp594);});} goto _LL326; _LL354: if( _temp421 == 0){
s=({ struct Cyc_PP_Doc*(* _temp598)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp601= Cyc_PP_text( _tag_arr("union ", sizeof( unsigned char), 7u)); struct
Cyc_PP_Doc* _temp602= Cyc_Absynpp_tps2doc( _temp419); struct Cyc_PP_Doc*
_temp599[ 2u]={ _temp601, _temp602}; struct _tagged_arr _temp600={( void*)
_temp599,( void*) _temp599,( void*)( _temp599 + 2u)}; _temp598( _temp600);});}
else{ s=({ struct Cyc_PP_Doc*(* _temp603)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp606= Cyc_PP_text( _tag_arr("union ", sizeof(
unsigned char), 7u)); struct Cyc_PP_Doc* _temp607= _temp421 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc(( struct _tuple0*) _check_null( _temp421)); struct Cyc_PP_Doc*
_temp608= Cyc_Absynpp_tps2doc( _temp419); struct Cyc_PP_Doc* _temp604[ 3u]={
_temp606, _temp607, _temp608}; struct _tagged_arr _temp605={( void*) _temp604,(
void*) _temp604,( void*)( _temp604 + 3u)}; _temp603( _temp605);});} goto _LL326;
_LL356: s=({ struct Cyc_PP_Doc*(* _temp609)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp612= Cyc_PP_text( _tag_arr("struct {", sizeof(
unsigned char), 9u)); struct Cyc_PP_Doc* _temp613= Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc(
_temp423)); struct Cyc_PP_Doc* _temp614= Cyc_PP_text( _tag_arr("}", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp610[ 3u]={ _temp612, _temp613,
_temp614}; struct _tagged_arr _temp611={( void*) _temp610,( void*) _temp610,(
void*)( _temp610 + 3u)}; _temp609( _temp611);}); goto _LL326; _LL358: s=({
struct Cyc_PP_Doc*(* _temp615)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp618= Cyc_PP_text( _tag_arr("union {", sizeof( unsigned char), 8u)); struct
Cyc_PP_Doc* _temp619= Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc( _temp425));
struct Cyc_PP_Doc* _temp620= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp616[ 3u]={ _temp618, _temp619, _temp620}; struct
_tagged_arr _temp617={( void*) _temp616,( void*) _temp616,( void*)( _temp616 + 3u)};
_temp615( _temp617);}); goto _LL326; _LL360: s=({ struct Cyc_PP_Doc*(* _temp621)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp624= Cyc_PP_text(
_tag_arr("enum ", sizeof( unsigned char), 6u)); struct Cyc_PP_Doc* _temp625= Cyc_Absynpp_qvar2doc(
_temp429); struct Cyc_PP_Doc* _temp622[ 2u]={ _temp624, _temp625}; struct
_tagged_arr _temp623={( void*) _temp622,( void*) _temp622,( void*)( _temp622 + 2u)};
_temp621( _temp623);}); goto _LL326; _LL362: s=({ struct Cyc_PP_Doc*(* _temp626)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp629= Cyc_Absynpp_qvar2doc(
_temp435); struct Cyc_PP_Doc* _temp630= Cyc_Absynpp_tps2doc( _temp433); struct
Cyc_PP_Doc* _temp627[ 2u]={ _temp629, _temp630}; struct _tagged_arr _temp628={(
void*) _temp627,( void*) _temp627,( void*)( _temp627 + 2u)}; _temp626( _temp628);});
goto _LL326; _LL364: s=({ struct Cyc_PP_Doc*(* _temp631)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp634= Cyc_PP_text( _tag_arr("region_t<",
sizeof( unsigned char), 10u)); struct Cyc_PP_Doc* _temp635= Cyc_Absynpp_rgn2doc(
_temp437); struct Cyc_PP_Doc* _temp636= Cyc_PP_text( _tag_arr(">", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp632[ 3u]={ _temp634, _temp635,
_temp636}; struct _tagged_arr _temp633={( void*) _temp632,( void*) _temp632,(
void*)( _temp632 + 3u)}; _temp631( _temp633);}); goto _LL326; _LL366: s= Cyc_Absynpp_rgn2doc(
t); goto _LL326; _LL368: s= Cyc_Absynpp_eff2doc( t); goto _LL326; _LL370: s= Cyc_Absynpp_eff2doc(
t); goto _LL326; _LL326:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_vo2doc(
struct Cyc_Core_Opt* vo){ return vo == 0? Cyc_PP_nil_doc(): Cyc_PP_text(*((
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( vo))->v));} struct
_tuple8{ void* f1; void* f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_rgn_cmp2doc(
struct _tuple8* cmp){ struct _tuple8 _temp639; void* _temp640; void* _temp642;
struct _tuple8* _temp637= cmp; _temp639=* _temp637; _LL643: _temp642= _temp639.f1;
goto _LL641; _LL641: _temp640= _temp639.f2; goto _LL638; _LL638: return({ struct
Cyc_PP_Doc*(* _temp644)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp647= Cyc_Absynpp_rgn2doc( _temp642); struct Cyc_PP_Doc* _temp648= Cyc_PP_text(
_tag_arr(" < ", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc* _temp649= Cyc_Absynpp_rgn2doc(
_temp640); struct Cyc_PP_Doc* _temp645[ 3u]={ _temp647, _temp648, _temp649};
struct _tagged_arr _temp646={( void*) _temp645,( void*) _temp645,( void*)(
_temp645 + 3u)}; _temp644( _temp646);});} struct Cyc_PP_Doc* Cyc_Absynpp_rgnpo2doc(
struct Cyc_List_List* po){ return Cyc_PP_group( _tag_arr("", sizeof(
unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple8*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_rgn_cmp2doc,
po));} struct Cyc_PP_Doc* Cyc_Absynpp_funarg2doc( struct _tuple1* t){ struct Cyc_Core_Opt*
dopt=(* t).f1 == 0? 0:({ struct Cyc_Core_Opt* _temp650=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp650->v=( void*) Cyc_PP_text(*((
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v));
_temp650;}); return Cyc_Absynpp_tqtd2doc((* t).f2,(* t).f3, dopt);} struct Cyc_PP_Doc*
Cyc_Absynpp_funargs2doc( struct Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo*
cyc_varargs, struct Cyc_Core_Opt* effopt, struct Cyc_List_List* rgn_po){ struct
Cyc_List_List* _temp651=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc,
args); struct Cyc_PP_Doc* eff_doc; if( c_varargs){ _temp651=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp651,({
struct Cyc_List_List* _temp652=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp652->hd=( void*) Cyc_PP_text( _tag_arr("...",
sizeof( unsigned char), 4u)); _temp652->tl= 0; _temp652;}));} else{ if(
cyc_varargs != 0){ struct Cyc_PP_Doc* _temp653=({ struct Cyc_PP_Doc*(* _temp655)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp658= Cyc_PP_text(
_tag_arr("...", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc* _temp659= Cyc_Absynpp_typ2doc((
void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->rgn); struct
Cyc_PP_Doc* _temp660=(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->inject?
Cyc_PP_text( _tag_arr(" inject ", sizeof( unsigned char), 9u)): Cyc_PP_text(
_tag_arr(" ", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp661= Cyc_Absynpp_funarg2doc(({
struct _tuple1* _temp662=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp662->f1=(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->name;
_temp662->f2=(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->tq;
_temp662->f3=( void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->type;
_temp662;})); struct Cyc_PP_Doc* _temp656[ 4u]={ _temp658, _temp659, _temp660,
_temp661}; struct _tagged_arr _temp657={( void*) _temp656,( void*) _temp656,(
void*)( _temp656 + 4u)}; _temp655( _temp657);}); _temp651=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp651,({
struct Cyc_List_List* _temp654=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp654->hd=( void*) _temp653; _temp654->tl= 0;
_temp654;}));}}{ struct Cyc_PP_Doc* _temp663= Cyc_PP_group( _tag_arr("", sizeof(
unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u), _tag_arr(",",
sizeof( unsigned char), 2u), _temp651); if( effopt != 0){ _temp663=({ struct Cyc_PP_Doc*(*
_temp664)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp667=
_temp663; struct Cyc_PP_Doc* _temp668= Cyc_PP_text( _tag_arr(";", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp669= Cyc_Absynpp_eff2doc(( void*)((
struct Cyc_Core_Opt*) _check_null( effopt))->v); struct Cyc_PP_Doc* _temp665[ 3u]={
_temp667, _temp668, _temp669}; struct _tagged_arr _temp666={( void*) _temp665,(
void*) _temp665,( void*)( _temp665 + 3u)}; _temp664( _temp666);});} if( rgn_po
!= 0){ _temp663=({ struct Cyc_PP_Doc*(* _temp670)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp673= _temp663; struct Cyc_PP_Doc* _temp674= Cyc_PP_text(
_tag_arr(":", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp675= Cyc_Absynpp_rgnpo2doc(
rgn_po); struct Cyc_PP_Doc* _temp671[ 3u]={ _temp673, _temp674, _temp675};
struct _tagged_arr _temp672={( void*) _temp671,( void*) _temp671,( void*)(
_temp671 + 3u)}; _temp670( _temp672);});} return({ struct Cyc_PP_Doc*(* _temp676)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp679= Cyc_PP_text(
_tag_arr("(", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp680=
_temp663; struct Cyc_PP_Doc* _temp681= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp677[ 3u]={ _temp679, _temp680,
_temp681}; struct _tagged_arr _temp678={( void*) _temp677,( void*) _temp677,(
void*)( _temp677 + 3u)}; _temp676( _temp678);});}} struct _tuple1* Cyc_Absynpp_arg_mk_opt(
struct _tuple3* arg){ return({ struct _tuple1* _temp682=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp682->f1=({ struct Cyc_Core_Opt*
_temp683=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp683->v=( void*)(* arg).f1; _temp683;}); _temp682->f2=(* arg).f2; _temp682->f3=(*
arg).f3; _temp682;});} struct Cyc_PP_Doc* Cyc_Absynpp_var2doc( struct
_tagged_arr* v){ return Cyc_PP_text(* v);} struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc(
struct _tuple0* q){ struct Cyc_List_List* _temp684= 0; int match;{ void*
_temp685=(* q).f1; struct Cyc_List_List* _temp693; struct Cyc_List_List*
_temp695; _LL687: if( _temp685 ==( void*) Cyc_Absyn_Loc_n){ goto _LL688;} else{
goto _LL689;} _LL689: if(( unsigned int) _temp685 > 1u?*(( int*) _temp685) ==
Cyc_Absyn_Rel_n: 0){ _LL694: _temp693=(( struct Cyc_Absyn_Rel_n_struct*)
_temp685)->f1; goto _LL690;} else{ goto _LL691;} _LL691: if(( unsigned int)
_temp685 > 1u?*(( int*) _temp685) == Cyc_Absyn_Abs_n: 0){ _LL696: _temp695=((
struct Cyc_Absyn_Abs_n_struct*) _temp685)->f1; goto _LL692;} else{ goto _LL686;}
_LL688: _temp693= 0; goto _LL690; _LL690: match= 0; _temp684= _temp693; goto
_LL686; _LL692: match= Cyc_Absynpp_use_curr_namespace?(( int(*)( int(* cmp)(
struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_List_list_prefix)( Cyc_String_strptrcmp, _temp695, Cyc_Absynpp_curr_namespace):
0; _temp684=( Cyc_Absynpp_qvar_to_Cids? Cyc_Absynpp_add_cyc_prefix: 0)?({ struct
Cyc_List_List* _temp697=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp697->hd=( void*) Cyc_Absynpp_cyc_stringptr; _temp697->tl= _temp695;
_temp697;}): _temp695; goto _LL686; _LL686:;} if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_PP_text(( struct _tagged_arr) Cyc_String_str_sepstr((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp684,({
struct Cyc_List_List* _temp698=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp698->hd=( void*)(* q).f2; _temp698->tl= 0; _temp698;})),
_tag_arr("_", sizeof( unsigned char), 2u)));} else{ if( match){ return Cyc_Absynpp_var2doc((*
q).f2);} else{ return Cyc_PP_text(( struct _tagged_arr) Cyc_String_str_sepstr(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp684,({ struct Cyc_List_List* _temp699=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp699->hd=( void*)(* q).f2; _temp699->tl= 0;
_temp699;})), _tag_arr("::", sizeof( unsigned char), 3u)));}}} struct Cyc_PP_Doc*
Cyc_Absynpp_typedef_name2doc( struct _tuple0* v){ if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_Absynpp_qvar2doc( v);} if( Cyc_Absynpp_use_curr_namespace){ void*
_temp700=(* v).f1; struct Cyc_List_List* _temp710; struct Cyc_List_List*
_temp712; _LL702: if( _temp700 ==( void*) Cyc_Absyn_Loc_n){ goto _LL703;} else{
goto _LL704;} _LL704: if(( unsigned int) _temp700 > 1u?*(( int*) _temp700) ==
Cyc_Absyn_Rel_n: 0){ _LL711: _temp710=(( struct Cyc_Absyn_Rel_n_struct*)
_temp700)->f1; if( _temp710 == 0){ goto _LL705;} else{ goto _LL706;}} else{ goto
_LL706;} _LL706: if(( unsigned int) _temp700 > 1u?*(( int*) _temp700) == Cyc_Absyn_Abs_n:
0){ _LL713: _temp712=(( struct Cyc_Absyn_Abs_n_struct*) _temp700)->f1; goto
_LL707;} else{ goto _LL708;} _LL708: goto _LL709; _LL703: goto _LL705; _LL705:
return Cyc_Absynpp_var2doc((* v).f2); _LL707: if((( int(*)( int(* cmp)( struct
_tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_List_list_cmp)( Cyc_String_strptrcmp, _temp712, Cyc_Absynpp_curr_namespace)
== 0){ return Cyc_Absynpp_var2doc((* v).f2);} else{ goto _LL709;} _LL709: return({
struct Cyc_PP_Doc*(* _temp714)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp717= Cyc_PP_text( _tag_arr("/* bad namespace : */ ", sizeof( unsigned char),
23u)); struct Cyc_PP_Doc* _temp718= Cyc_Absynpp_qvar2doc( v); struct Cyc_PP_Doc*
_temp715[ 2u]={ _temp717, _temp718}; struct _tagged_arr _temp716={( void*)
_temp715,( void*) _temp715,( void*)( _temp715 + 2u)}; _temp714( _temp716);});
_LL701:;} else{ return Cyc_Absynpp_var2doc((* v).f2);}} struct Cyc_PP_Doc* Cyc_Absynpp_typ2doc(
void* t){ return Cyc_Absynpp_tqtd2doc( Cyc_Absyn_empty_tqual(), t, 0);} int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp* e){ void* _temp719=( void*) e->r; void* _temp793; void*
_temp795; struct _tuple0* _temp797; struct _tuple0* _temp799; struct Cyc_List_List*
_temp801; void* _temp803; struct Cyc_Absyn_Exp* _temp805; struct Cyc_Core_Opt*
_temp807; struct Cyc_Absyn_Exp* _temp809; void* _temp811; struct Cyc_Absyn_Exp*
_temp813; struct Cyc_Absyn_Exp* _temp815; struct Cyc_Absyn_Exp* _temp817; struct
Cyc_Absyn_Exp* _temp819; struct Cyc_Absyn_Exp* _temp821; struct Cyc_Absyn_Exp*
_temp823; struct Cyc_List_List* _temp825; struct Cyc_Absyn_Exp* _temp827; struct
Cyc_Absyn_VarargCallInfo* _temp829; struct Cyc_List_List* _temp831; struct Cyc_Absyn_Exp*
_temp833; struct Cyc_Absyn_Exp* _temp835; struct Cyc_Absyn_Exp* _temp837; struct
Cyc_List_List* _temp839; struct Cyc_Absyn_Exp* _temp841; struct Cyc_Absyn_Exp*
_temp843; void* _temp845; struct Cyc_Absyn_Exp* _temp847; struct Cyc_Absyn_Exp*
_temp849; struct Cyc_Absyn_Exp* _temp851; void* _temp853; struct Cyc_Absyn_Exp*
_temp855; struct _tagged_arr* _temp857; void* _temp859; struct Cyc_Absyn_Exp*
_temp861; struct _tagged_arr* _temp863; struct Cyc_Absyn_Exp* _temp865; struct
_tagged_arr* _temp867; struct Cyc_Absyn_Exp* _temp869; struct Cyc_Absyn_Exp*
_temp871; struct Cyc_Absyn_Exp* _temp873; struct Cyc_List_List* _temp875; struct
Cyc_List_List* _temp877; struct _tuple1* _temp879; struct Cyc_List_List*
_temp881; struct Cyc_Absyn_Exp* _temp883; struct Cyc_Absyn_Exp* _temp885; struct
Cyc_Absyn_Vardecl* _temp887; struct Cyc_Absyn_Structdecl* _temp889; struct Cyc_List_List*
_temp891; struct Cyc_Core_Opt* _temp893; struct _tuple0* _temp895; struct Cyc_List_List*
_temp897; void* _temp899; struct Cyc_Absyn_Tunionfield* _temp901; struct Cyc_Absyn_Tuniondecl*
_temp903; struct Cyc_List_List* _temp905; struct Cyc_Core_Opt* _temp907; struct
Cyc_Core_Opt* _temp909; struct Cyc_Absyn_Enumfield* _temp911; struct Cyc_Absyn_Enumdecl*
_temp913; struct _tuple0* _temp915; void* _temp917; struct Cyc_Absyn_Exp*
_temp919; struct Cyc_List_List* _temp921; struct Cyc_Core_Opt* _temp923; struct
Cyc_Absyn_Stmt* _temp925; struct Cyc_Absyn_Fndecl* _temp927; struct Cyc_Absyn_Exp*
_temp929; _LL721: if(*(( int*) _temp719) == Cyc_Absyn_Const_e){ _LL794: _temp793=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp719)->f1; goto _LL722;} else{
goto _LL723;} _LL723: if(*(( int*) _temp719) == Cyc_Absyn_Var_e){ _LL798:
_temp797=(( struct Cyc_Absyn_Var_e_struct*) _temp719)->f1; goto _LL796; _LL796:
_temp795=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp719)->f2; goto _LL724;}
else{ goto _LL725;} _LL725: if(*(( int*) _temp719) == Cyc_Absyn_UnknownId_e){
_LL800: _temp799=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp719)->f1; goto
_LL726;} else{ goto _LL727;} _LL727: if(*(( int*) _temp719) == Cyc_Absyn_Primop_e){
_LL804: _temp803=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp719)->f1;
goto _LL802; _LL802: _temp801=(( struct Cyc_Absyn_Primop_e_struct*) _temp719)->f2;
goto _LL728;} else{ goto _LL729;} _LL729: if(*(( int*) _temp719) == Cyc_Absyn_AssignOp_e){
_LL810: _temp809=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp719)->f1; goto
_LL808; _LL808: _temp807=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp719)->f2;
goto _LL806; _LL806: _temp805=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp719)->f3;
goto _LL730;} else{ goto _LL731;} _LL731: if(*(( int*) _temp719) == Cyc_Absyn_Increment_e){
_LL814: _temp813=(( struct Cyc_Absyn_Increment_e_struct*) _temp719)->f1; goto
_LL812; _LL812: _temp811=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp719)->f2; goto _LL732;} else{ goto _LL733;} _LL733: if(*(( int*) _temp719)
== Cyc_Absyn_Conditional_e){ _LL820: _temp819=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp719)->f1; goto _LL818; _LL818: _temp817=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp719)->f2; goto _LL816; _LL816: _temp815=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp719)->f3; goto _LL734;} else{ goto _LL735;} _LL735: if(*(( int*) _temp719)
== Cyc_Absyn_SeqExp_e){ _LL824: _temp823=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp719)->f1; goto _LL822; _LL822: _temp821=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp719)->f2; goto _LL736;} else{ goto _LL737;} _LL737: if(*(( int*) _temp719)
== Cyc_Absyn_UnknownCall_e){ _LL828: _temp827=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp719)->f1; goto _LL826; _LL826: _temp825=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp719)->f2; goto _LL738;} else{ goto _LL739;} _LL739: if(*(( int*) _temp719)
== Cyc_Absyn_FnCall_e){ _LL834: _temp833=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp719)->f1; goto _LL832; _LL832: _temp831=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp719)->f2; goto _LL830; _LL830: _temp829=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp719)->f3; goto _LL740;} else{ goto _LL741;} _LL741: if(*(( int*) _temp719)
== Cyc_Absyn_Throw_e){ _LL836: _temp835=(( struct Cyc_Absyn_Throw_e_struct*)
_temp719)->f1; goto _LL742;} else{ goto _LL743;} _LL743: if(*(( int*) _temp719)
== Cyc_Absyn_NoInstantiate_e){ _LL838: _temp837=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp719)->f1; goto _LL744;} else{ goto _LL745;} _LL745: if(*(( int*) _temp719)
== Cyc_Absyn_Instantiate_e){ _LL842: _temp841=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp719)->f1; goto _LL840; _LL840: _temp839=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp719)->f2; goto _LL746;} else{ goto _LL747;} _LL747: if(*(( int*) _temp719)
== Cyc_Absyn_Cast_e){ _LL846: _temp845=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp719)->f1; goto _LL844; _LL844: _temp843=(( struct Cyc_Absyn_Cast_e_struct*)
_temp719)->f2; goto _LL748;} else{ goto _LL749;} _LL749: if(*(( int*) _temp719)
== Cyc_Absyn_New_e){ _LL850: _temp849=(( struct Cyc_Absyn_New_e_struct*)
_temp719)->f1; goto _LL848; _LL848: _temp847=(( struct Cyc_Absyn_New_e_struct*)
_temp719)->f2; goto _LL750;} else{ goto _LL751;} _LL751: if(*(( int*) _temp719)
== Cyc_Absyn_Address_e){ _LL852: _temp851=(( struct Cyc_Absyn_Address_e_struct*)
_temp719)->f1; goto _LL752;} else{ goto _LL753;} _LL753: if(*(( int*) _temp719)
== Cyc_Absyn_Sizeoftyp_e){ _LL854: _temp853=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp719)->f1; goto _LL754;} else{ goto _LL755;} _LL755: if(*(( int*) _temp719)
== Cyc_Absyn_Sizeofexp_e){ _LL856: _temp855=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp719)->f1; goto _LL756;} else{ goto _LL757;} _LL757: if(*(( int*) _temp719)
== Cyc_Absyn_Offsetof_e){ _LL860: _temp859=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp719)->f1; goto _LL858; _LL858: _temp857=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp719)->f2; goto _LL758;} else{ goto _LL759;} _LL759: if(*(( int*) _temp719)
== Cyc_Absyn_Deref_e){ _LL862: _temp861=(( struct Cyc_Absyn_Deref_e_struct*)
_temp719)->f1; goto _LL760;} else{ goto _LL761;} _LL761: if(*(( int*) _temp719)
== Cyc_Absyn_StructMember_e){ _LL866: _temp865=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp719)->f1; goto _LL864; _LL864: _temp863=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp719)->f2; goto _LL762;} else{ goto _LL763;} _LL763: if(*(( int*) _temp719)
== Cyc_Absyn_StructArrow_e){ _LL870: _temp869=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp719)->f1; goto _LL868; _LL868: _temp867=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp719)->f2; goto _LL764;} else{ goto _LL765;} _LL765: if(*(( int*) _temp719)
== Cyc_Absyn_Subscript_e){ _LL874: _temp873=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp719)->f1; goto _LL872; _LL872: _temp871=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp719)->f2; goto _LL766;} else{ goto _LL767;} _LL767: if(*(( int*) _temp719)
== Cyc_Absyn_Tuple_e){ _LL876: _temp875=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp719)->f1; goto _LL768;} else{ goto _LL769;} _LL769: if(*(( int*) _temp719)
== Cyc_Absyn_CompoundLit_e){ _LL880: _temp879=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp719)->f1; goto _LL878; _LL878: _temp877=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp719)->f2; goto _LL770;} else{ goto _LL771;} _LL771: if(*(( int*) _temp719)
== Cyc_Absyn_Array_e){ _LL882: _temp881=(( struct Cyc_Absyn_Array_e_struct*)
_temp719)->f1; goto _LL772;} else{ goto _LL773;} _LL773: if(*(( int*) _temp719)
== Cyc_Absyn_Comprehension_e){ _LL888: _temp887=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp719)->f1; goto _LL886; _LL886: _temp885=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp719)->f2; goto _LL884; _LL884: _temp883=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp719)->f3; goto _LL774;} else{ goto _LL775;} _LL775: if(*(( int*) _temp719)
== Cyc_Absyn_Struct_e){ _LL896: _temp895=(( struct Cyc_Absyn_Struct_e_struct*)
_temp719)->f1; goto _LL894; _LL894: _temp893=(( struct Cyc_Absyn_Struct_e_struct*)
_temp719)->f2; goto _LL892; _LL892: _temp891=(( struct Cyc_Absyn_Struct_e_struct*)
_temp719)->f3; goto _LL890; _LL890: _temp889=(( struct Cyc_Absyn_Struct_e_struct*)
_temp719)->f4; goto _LL776;} else{ goto _LL777;} _LL777: if(*(( int*) _temp719)
== Cyc_Absyn_AnonStruct_e){ _LL900: _temp899=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp719)->f1; goto _LL898; _LL898: _temp897=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp719)->f2; goto _LL778;} else{ goto _LL779;} _LL779: if(*(( int*) _temp719)
== Cyc_Absyn_Tunion_e){ _LL910: _temp909=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp719)->f1; goto _LL908; _LL908: _temp907=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp719)->f2; goto _LL906; _LL906: _temp905=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp719)->f3; goto _LL904; _LL904: _temp903=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp719)->f4; goto _LL902; _LL902: _temp901=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp719)->f5; goto _LL780;} else{ goto _LL781;} _LL781: if(*(( int*) _temp719)
== Cyc_Absyn_Enum_e){ _LL916: _temp915=(( struct Cyc_Absyn_Enum_e_struct*)
_temp719)->f1; goto _LL914; _LL914: _temp913=(( struct Cyc_Absyn_Enum_e_struct*)
_temp719)->f2; goto _LL912; _LL912: _temp911=(( struct Cyc_Absyn_Enum_e_struct*)
_temp719)->f3; goto _LL782;} else{ goto _LL783;} _LL783: if(*(( int*) _temp719)
== Cyc_Absyn_Malloc_e){ _LL920: _temp919=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp719)->f1; goto _LL918; _LL918: _temp917=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp719)->f2; goto _LL784;} else{ goto _LL785;} _LL785: if(*(( int*) _temp719)
== Cyc_Absyn_UnresolvedMem_e){ _LL924: _temp923=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp719)->f1; goto _LL922; _LL922: _temp921=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp719)->f2; goto _LL786;} else{ goto _LL787;} _LL787: if(*(( int*) _temp719)
== Cyc_Absyn_StmtExp_e){ _LL926: _temp925=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp719)->f1; goto _LL788;} else{ goto _LL789;} _LL789: if(*(( int*) _temp719)
== Cyc_Absyn_Codegen_e){ _LL928: _temp927=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp719)->f1; goto _LL790;} else{ goto _LL791;} _LL791: if(*(( int*) _temp719)
== Cyc_Absyn_Fill_e){ _LL930: _temp929=(( struct Cyc_Absyn_Fill_e_struct*)
_temp719)->f1; goto _LL792;} else{ goto _LL720;} _LL722: return 10000; _LL724:
return 10000; _LL726: return 10000; _LL728: { void* _temp931= _temp803; _LL933:
if( _temp931 ==( void*) Cyc_Absyn_Plus){ goto _LL934;} else{ goto _LL935;}
_LL935: if( _temp931 ==( void*) Cyc_Absyn_Times){ goto _LL936;} else{ goto
_LL937;} _LL937: if( _temp931 ==( void*) Cyc_Absyn_Minus){ goto _LL938;} else{
goto _LL939;} _LL939: if( _temp931 ==( void*) Cyc_Absyn_Div){ goto _LL940;}
else{ goto _LL941;} _LL941: if( _temp931 ==( void*) Cyc_Absyn_Mod){ goto _LL942;}
else{ goto _LL943;} _LL943: if( _temp931 ==( void*) Cyc_Absyn_Eq){ goto _LL944;}
else{ goto _LL945;} _LL945: if( _temp931 ==( void*) Cyc_Absyn_Neq){ goto _LL946;}
else{ goto _LL947;} _LL947: if( _temp931 ==( void*) Cyc_Absyn_Gt){ goto _LL948;}
else{ goto _LL949;} _LL949: if( _temp931 ==( void*) Cyc_Absyn_Lt){ goto _LL950;}
else{ goto _LL951;} _LL951: if( _temp931 ==( void*) Cyc_Absyn_Gte){ goto _LL952;}
else{ goto _LL953;} _LL953: if( _temp931 ==( void*) Cyc_Absyn_Lte){ goto _LL954;}
else{ goto _LL955;} _LL955: if( _temp931 ==( void*) Cyc_Absyn_Not){ goto _LL956;}
else{ goto _LL957;} _LL957: if( _temp931 ==( void*) Cyc_Absyn_Bitnot){ goto
_LL958;} else{ goto _LL959;} _LL959: if( _temp931 ==( void*) Cyc_Absyn_Bitand){
goto _LL960;} else{ goto _LL961;} _LL961: if( _temp931 ==( void*) Cyc_Absyn_Bitor){
goto _LL962;} else{ goto _LL963;} _LL963: if( _temp931 ==( void*) Cyc_Absyn_Bitxor){
goto _LL964;} else{ goto _LL965;} _LL965: if( _temp931 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL966;} else{ goto _LL967;} _LL967: if( _temp931 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL968;} else{ goto _LL969;} _LL969: if( _temp931 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL970;} else{ goto _LL971;} _LL971: if( _temp931 ==( void*) Cyc_Absyn_Size){
goto _LL972;} else{ goto _LL973;} _LL973: if( _temp931 ==( void*) Cyc_Absyn_Printf){
goto _LL974;} else{ goto _LL975;} _LL975: if( _temp931 ==( void*) Cyc_Absyn_Fprintf){
goto _LL976;} else{ goto _LL977;} _LL977: if( _temp931 ==( void*) Cyc_Absyn_Xprintf){
goto _LL978;} else{ goto _LL979;} _LL979: if( _temp931 ==( void*) Cyc_Absyn_Scanf){
goto _LL980;} else{ goto _LL981;} _LL981: if( _temp931 ==( void*) Cyc_Absyn_Fscanf){
goto _LL982;} else{ goto _LL983;} _LL983: if( _temp931 ==( void*) Cyc_Absyn_Sscanf){
goto _LL984;} else{ goto _LL932;} _LL934: return 100; _LL936: return 110; _LL938:
return 100; _LL940: return 110; _LL942: return 110; _LL944: return 70; _LL946:
return 70; _LL948: return 80; _LL950: return 80; _LL952: return 80; _LL954:
return 80; _LL956: return 130; _LL958: return 130; _LL960: return 60; _LL962:
return 40; _LL964: return 50; _LL966: return 90; _LL968: return 80; _LL970:
return 80; _LL972: return 140; _LL974: return 130; _LL976: return 130; _LL978:
return 130; _LL980: return 130; _LL982: return 130; _LL984: return 130; _LL932:;}
_LL730: return 20; _LL732: return 130; _LL734: return 30; _LL736: return 10;
_LL738: return 140; _LL740: return 140; _LL742: return 130; _LL744: return Cyc_Absynpp_exp_prec(
_temp837); _LL746: return Cyc_Absynpp_exp_prec( _temp841); _LL748: return 120;
_LL750: return 15; _LL752: return 130; _LL754: return 130; _LL756: return 130;
_LL758: return 130; _LL760: return 130; _LL762: return 140; _LL764: return 140;
_LL766: return 140; _LL768: return 150; _LL770: goto _LL772; _LL772: goto _LL774;
_LL774: goto _LL776; _LL776: goto _LL778; _LL778: goto _LL780; _LL780: goto
_LL782; _LL782: goto _LL784; _LL784: return 140; _LL786: return 140; _LL788:
return 10000; _LL790: return 140; _LL792: return 140; _LL720:;} struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp* e){ return Cyc_Absynpp_exp2doc_prec(
0, e);} struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec( int inprec, struct Cyc_Absyn_Exp*
e){ int myprec= Cyc_Absynpp_exp_prec( e); struct Cyc_PP_Doc* s;{ void* _temp985=(
void*) e->r; void* _temp1059; void* _temp1061; struct _tuple0* _temp1063; struct
_tuple0* _temp1065; struct Cyc_List_List* _temp1067; void* _temp1069; struct Cyc_Absyn_Exp*
_temp1071; struct Cyc_Core_Opt* _temp1073; struct Cyc_Absyn_Exp* _temp1075; void*
_temp1077; struct Cyc_Absyn_Exp* _temp1079; struct Cyc_Absyn_Exp* _temp1081;
struct Cyc_Absyn_Exp* _temp1083; struct Cyc_Absyn_Exp* _temp1085; struct Cyc_Absyn_Exp*
_temp1087; struct Cyc_Absyn_Exp* _temp1089; struct Cyc_List_List* _temp1091;
struct Cyc_Absyn_Exp* _temp1093; struct Cyc_Absyn_VarargCallInfo* _temp1095;
struct Cyc_List_List* _temp1097; struct Cyc_Absyn_Exp* _temp1099; struct Cyc_Absyn_Exp*
_temp1101; struct Cyc_Absyn_Exp* _temp1103; struct Cyc_List_List* _temp1105;
struct Cyc_Absyn_Exp* _temp1107; struct Cyc_Absyn_Exp* _temp1109; void*
_temp1111; struct Cyc_Absyn_Exp* _temp1113; struct Cyc_Absyn_Exp* _temp1115;
struct Cyc_Absyn_Exp* _temp1117; void* _temp1119; struct Cyc_Absyn_Exp*
_temp1121; struct _tagged_arr* _temp1123; void* _temp1125; struct Cyc_Absyn_Exp*
_temp1127; struct _tagged_arr* _temp1129; struct Cyc_Absyn_Exp* _temp1131;
struct _tagged_arr* _temp1133; struct Cyc_Absyn_Exp* _temp1135; struct Cyc_Absyn_Exp*
_temp1137; struct Cyc_Absyn_Exp* _temp1139; struct Cyc_List_List* _temp1141;
struct Cyc_List_List* _temp1143; struct _tuple1* _temp1145; struct Cyc_List_List*
_temp1147; struct Cyc_Absyn_Exp* _temp1149; struct Cyc_Absyn_Exp* _temp1151;
struct Cyc_Absyn_Vardecl* _temp1153; struct Cyc_Absyn_Structdecl* _temp1155;
struct Cyc_List_List* _temp1157; struct Cyc_Core_Opt* _temp1159; struct _tuple0*
_temp1161; struct Cyc_List_List* _temp1163; void* _temp1165; struct Cyc_Absyn_Tunionfield*
_temp1167; struct Cyc_Absyn_Tuniondecl* _temp1169; struct Cyc_List_List*
_temp1171; struct Cyc_Core_Opt* _temp1173; struct Cyc_Core_Opt* _temp1175;
struct Cyc_Absyn_Enumfield* _temp1177; struct Cyc_Absyn_Enumdecl* _temp1179;
struct _tuple0* _temp1181; void* _temp1183; struct Cyc_Absyn_Exp* _temp1185;
struct Cyc_List_List* _temp1187; struct Cyc_Core_Opt* _temp1189; struct Cyc_Absyn_Stmt*
_temp1191; struct Cyc_Absyn_Fndecl* _temp1193; struct Cyc_Absyn_Exp* _temp1195;
_LL987: if(*(( int*) _temp985) == Cyc_Absyn_Const_e){ _LL1060: _temp1059=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp985)->f1; goto _LL988;} else{ goto _LL989;}
_LL989: if(*(( int*) _temp985) == Cyc_Absyn_Var_e){ _LL1064: _temp1063=(( struct
Cyc_Absyn_Var_e_struct*) _temp985)->f1; goto _LL1062; _LL1062: _temp1061=( void*)((
struct Cyc_Absyn_Var_e_struct*) _temp985)->f2; goto _LL990;} else{ goto _LL991;}
_LL991: if(*(( int*) _temp985) == Cyc_Absyn_UnknownId_e){ _LL1066: _temp1065=((
struct Cyc_Absyn_UnknownId_e_struct*) _temp985)->f1; goto _LL992;} else{ goto
_LL993;} _LL993: if(*(( int*) _temp985) == Cyc_Absyn_Primop_e){ _LL1070:
_temp1069=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp985)->f1; goto
_LL1068; _LL1068: _temp1067=(( struct Cyc_Absyn_Primop_e_struct*) _temp985)->f2;
goto _LL994;} else{ goto _LL995;} _LL995: if(*(( int*) _temp985) == Cyc_Absyn_AssignOp_e){
_LL1076: _temp1075=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp985)->f1; goto
_LL1074; _LL1074: _temp1073=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp985)->f2;
goto _LL1072; _LL1072: _temp1071=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp985)->f3; goto _LL996;} else{ goto _LL997;} _LL997: if(*(( int*) _temp985)
== Cyc_Absyn_Increment_e){ _LL1080: _temp1079=(( struct Cyc_Absyn_Increment_e_struct*)
_temp985)->f1; goto _LL1078; _LL1078: _temp1077=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp985)->f2; goto _LL998;} else{ goto _LL999;} _LL999: if(*(( int*) _temp985)
== Cyc_Absyn_Conditional_e){ _LL1086: _temp1085=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp985)->f1; goto _LL1084; _LL1084: _temp1083=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp985)->f2; goto _LL1082; _LL1082: _temp1081=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp985)->f3; goto _LL1000;} else{ goto _LL1001;} _LL1001: if(*(( int*)
_temp985) == Cyc_Absyn_SeqExp_e){ _LL1090: _temp1089=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp985)->f1; goto _LL1088; _LL1088: _temp1087=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp985)->f2; goto _LL1002;} else{ goto _LL1003;} _LL1003: if(*(( int*)
_temp985) == Cyc_Absyn_UnknownCall_e){ _LL1094: _temp1093=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp985)->f1; goto _LL1092; _LL1092: _temp1091=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp985)->f2; goto _LL1004;} else{ goto _LL1005;} _LL1005: if(*(( int*)
_temp985) == Cyc_Absyn_FnCall_e){ _LL1100: _temp1099=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp985)->f1; goto _LL1098; _LL1098: _temp1097=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp985)->f2; goto _LL1096; _LL1096: _temp1095=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp985)->f3; goto _LL1006;} else{ goto _LL1007;} _LL1007: if(*(( int*)
_temp985) == Cyc_Absyn_Throw_e){ _LL1102: _temp1101=(( struct Cyc_Absyn_Throw_e_struct*)
_temp985)->f1; goto _LL1008;} else{ goto _LL1009;} _LL1009: if(*(( int*)
_temp985) == Cyc_Absyn_NoInstantiate_e){ _LL1104: _temp1103=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp985)->f1; goto _LL1010;} else{ goto _LL1011;} _LL1011: if(*(( int*)
_temp985) == Cyc_Absyn_Instantiate_e){ _LL1108: _temp1107=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp985)->f1; goto _LL1106; _LL1106: _temp1105=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp985)->f2; goto _LL1012;} else{ goto _LL1013;} _LL1013: if(*(( int*)
_temp985) == Cyc_Absyn_Cast_e){ _LL1112: _temp1111=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp985)->f1; goto _LL1110; _LL1110: _temp1109=(( struct Cyc_Absyn_Cast_e_struct*)
_temp985)->f2; goto _LL1014;} else{ goto _LL1015;} _LL1015: if(*(( int*)
_temp985) == Cyc_Absyn_Address_e){ _LL1114: _temp1113=(( struct Cyc_Absyn_Address_e_struct*)
_temp985)->f1; goto _LL1016;} else{ goto _LL1017;} _LL1017: if(*(( int*)
_temp985) == Cyc_Absyn_New_e){ _LL1118: _temp1117=(( struct Cyc_Absyn_New_e_struct*)
_temp985)->f1; goto _LL1116; _LL1116: _temp1115=(( struct Cyc_Absyn_New_e_struct*)
_temp985)->f2; goto _LL1018;} else{ goto _LL1019;} _LL1019: if(*(( int*)
_temp985) == Cyc_Absyn_Sizeoftyp_e){ _LL1120: _temp1119=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp985)->f1; goto _LL1020;} else{ goto _LL1021;} _LL1021: if(*(( int*)
_temp985) == Cyc_Absyn_Sizeofexp_e){ _LL1122: _temp1121=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp985)->f1; goto _LL1022;} else{ goto _LL1023;} _LL1023: if(*(( int*)
_temp985) == Cyc_Absyn_Offsetof_e){ _LL1126: _temp1125=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp985)->f1; goto _LL1124; _LL1124: _temp1123=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp985)->f2; goto _LL1024;} else{ goto _LL1025;} _LL1025: if(*(( int*)
_temp985) == Cyc_Absyn_Deref_e){ _LL1128: _temp1127=(( struct Cyc_Absyn_Deref_e_struct*)
_temp985)->f1; goto _LL1026;} else{ goto _LL1027;} _LL1027: if(*(( int*)
_temp985) == Cyc_Absyn_StructMember_e){ _LL1132: _temp1131=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp985)->f1; goto _LL1130; _LL1130: _temp1129=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp985)->f2; goto _LL1028;} else{ goto _LL1029;} _LL1029: if(*(( int*)
_temp985) == Cyc_Absyn_StructArrow_e){ _LL1136: _temp1135=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp985)->f1; goto _LL1134; _LL1134: _temp1133=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp985)->f2; goto _LL1030;} else{ goto _LL1031;} _LL1031: if(*(( int*)
_temp985) == Cyc_Absyn_Subscript_e){ _LL1140: _temp1139=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp985)->f1; goto _LL1138; _LL1138: _temp1137=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp985)->f2; goto _LL1032;} else{ goto _LL1033;} _LL1033: if(*(( int*)
_temp985) == Cyc_Absyn_Tuple_e){ _LL1142: _temp1141=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp985)->f1; goto _LL1034;} else{ goto _LL1035;} _LL1035: if(*(( int*)
_temp985) == Cyc_Absyn_CompoundLit_e){ _LL1146: _temp1145=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp985)->f1; goto _LL1144; _LL1144: _temp1143=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp985)->f2; goto _LL1036;} else{ goto _LL1037;} _LL1037: if(*(( int*)
_temp985) == Cyc_Absyn_Array_e){ _LL1148: _temp1147=(( struct Cyc_Absyn_Array_e_struct*)
_temp985)->f1; goto _LL1038;} else{ goto _LL1039;} _LL1039: if(*(( int*)
_temp985) == Cyc_Absyn_Comprehension_e){ _LL1154: _temp1153=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp985)->f1; goto _LL1152; _LL1152: _temp1151=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp985)->f2; goto _LL1150; _LL1150: _temp1149=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp985)->f3; goto _LL1040;} else{ goto _LL1041;} _LL1041: if(*(( int*)
_temp985) == Cyc_Absyn_Struct_e){ _LL1162: _temp1161=(( struct Cyc_Absyn_Struct_e_struct*)
_temp985)->f1; goto _LL1160; _LL1160: _temp1159=(( struct Cyc_Absyn_Struct_e_struct*)
_temp985)->f2; goto _LL1158; _LL1158: _temp1157=(( struct Cyc_Absyn_Struct_e_struct*)
_temp985)->f3; goto _LL1156; _LL1156: _temp1155=(( struct Cyc_Absyn_Struct_e_struct*)
_temp985)->f4; goto _LL1042;} else{ goto _LL1043;} _LL1043: if(*(( int*)
_temp985) == Cyc_Absyn_AnonStruct_e){ _LL1166: _temp1165=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp985)->f1; goto _LL1164; _LL1164: _temp1163=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp985)->f2; goto _LL1044;} else{ goto _LL1045;} _LL1045: if(*(( int*)
_temp985) == Cyc_Absyn_Tunion_e){ _LL1176: _temp1175=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp985)->f1; goto _LL1174; _LL1174: _temp1173=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp985)->f2; goto _LL1172; _LL1172: _temp1171=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp985)->f3; goto _LL1170; _LL1170: _temp1169=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp985)->f4; goto _LL1168; _LL1168: _temp1167=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp985)->f5; goto _LL1046;} else{ goto _LL1047;} _LL1047: if(*(( int*)
_temp985) == Cyc_Absyn_Enum_e){ _LL1182: _temp1181=(( struct Cyc_Absyn_Enum_e_struct*)
_temp985)->f1; goto _LL1180; _LL1180: _temp1179=(( struct Cyc_Absyn_Enum_e_struct*)
_temp985)->f2; goto _LL1178; _LL1178: _temp1177=(( struct Cyc_Absyn_Enum_e_struct*)
_temp985)->f3; goto _LL1048;} else{ goto _LL1049;} _LL1049: if(*(( int*)
_temp985) == Cyc_Absyn_Malloc_e){ _LL1186: _temp1185=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp985)->f1; goto _LL1184; _LL1184: _temp1183=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp985)->f2; goto _LL1050;} else{ goto _LL1051;} _LL1051: if(*(( int*)
_temp985) == Cyc_Absyn_UnresolvedMem_e){ _LL1190: _temp1189=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp985)->f1; goto _LL1188; _LL1188: _temp1187=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp985)->f2; goto _LL1052;} else{ goto _LL1053;} _LL1053: if(*(( int*)
_temp985) == Cyc_Absyn_StmtExp_e){ _LL1192: _temp1191=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp985)->f1; goto _LL1054;} else{ goto _LL1055;} _LL1055: if(*(( int*)
_temp985) == Cyc_Absyn_Codegen_e){ _LL1194: _temp1193=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp985)->f1; goto _LL1056;} else{ goto _LL1057;} _LL1057: if(*(( int*)
_temp985) == Cyc_Absyn_Fill_e){ _LL1196: _temp1195=(( struct Cyc_Absyn_Fill_e_struct*)
_temp985)->f1; goto _LL1058;} else{ goto _LL986;} _LL988: s= Cyc_Absynpp_cnst2doc(
_temp1059); goto _LL986; _LL990: _temp1065= _temp1063; goto _LL992; _LL992: s=
Cyc_Absynpp_qvar2doc( _temp1065); goto _LL986; _LL994: s= Cyc_Absynpp_primapp2doc(
myprec, _temp1069, _temp1067); goto _LL986; _LL996: s=({ struct Cyc_PP_Doc*(*
_temp1197)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1200= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1075); struct Cyc_PP_Doc* _temp1201= Cyc_PP_text( _tag_arr(" ",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1202= _temp1073 == 0? Cyc_PP_text(
_tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_prim2doc(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1073))->v); struct Cyc_PP_Doc* _temp1203=
Cyc_PP_text( _tag_arr("= ", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc*
_temp1204= Cyc_Absynpp_exp2doc_prec( myprec, _temp1071); struct Cyc_PP_Doc*
_temp1198[ 5u]={ _temp1200, _temp1201, _temp1202, _temp1203, _temp1204}; struct
_tagged_arr _temp1199={( void*) _temp1198,( void*) _temp1198,( void*)( _temp1198
+ 5u)}; _temp1197( _temp1199);}); goto _LL986; _LL998: { struct Cyc_PP_Doc*
_temp1205= Cyc_Absynpp_exp2doc_prec( myprec, _temp1079);{ void* _temp1206=
_temp1077; _LL1208: if( _temp1206 ==( void*) Cyc_Absyn_PreInc){ goto _LL1209;}
else{ goto _LL1210;} _LL1210: if( _temp1206 ==( void*) Cyc_Absyn_PreDec){ goto
_LL1211;} else{ goto _LL1212;} _LL1212: if( _temp1206 ==( void*) Cyc_Absyn_PostInc){
goto _LL1213;} else{ goto _LL1214;} _LL1214: if( _temp1206 ==( void*) Cyc_Absyn_PostDec){
goto _LL1215;} else{ goto _LL1207;} _LL1209: s=({ struct Cyc_PP_Doc*(* _temp1216)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1219= Cyc_PP_text(
_tag_arr("++", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1220=
_temp1205; struct Cyc_PP_Doc* _temp1217[ 2u]={ _temp1219, _temp1220}; struct
_tagged_arr _temp1218={( void*) _temp1217,( void*) _temp1217,( void*)( _temp1217
+ 2u)}; _temp1216( _temp1218);}); goto _LL1207; _LL1211: s=({ struct Cyc_PP_Doc*(*
_temp1221)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1224= Cyc_PP_text(
_tag_arr("--", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1225=
_temp1205; struct Cyc_PP_Doc* _temp1222[ 2u]={ _temp1224, _temp1225}; struct
_tagged_arr _temp1223={( void*) _temp1222,( void*) _temp1222,( void*)( _temp1222
+ 2u)}; _temp1221( _temp1223);}); goto _LL1207; _LL1213: s=({ struct Cyc_PP_Doc*(*
_temp1226)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1229=
_temp1205; struct Cyc_PP_Doc* _temp1230= Cyc_PP_text( _tag_arr("++", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp1227[ 2u]={ _temp1229, _temp1230};
struct _tagged_arr _temp1228={( void*) _temp1227,( void*) _temp1227,( void*)(
_temp1227 + 2u)}; _temp1226( _temp1228);}); goto _LL1207; _LL1215: s=({ struct
Cyc_PP_Doc*(* _temp1231)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1234= _temp1205; struct Cyc_PP_Doc* _temp1235= Cyc_PP_text( _tag_arr("--",
sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1232[ 2u]={ _temp1234,
_temp1235}; struct _tagged_arr _temp1233={( void*) _temp1232,( void*) _temp1232,(
void*)( _temp1232 + 2u)}; _temp1231( _temp1233);}); goto _LL1207; _LL1207:;}
goto _LL986;} _LL1000:{ struct _tuple8 _temp1237=({ struct _tuple8 _temp1236;
_temp1236.f1=( void*) _temp1083->r; _temp1236.f2=( void*) _temp1081->r;
_temp1236;}); _LL1239: goto _LL1240; _LL1240: s=({ struct Cyc_PP_Doc*(*
_temp1241)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1244= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1085); struct Cyc_PP_Doc* _temp1245= Cyc_PP_text( _tag_arr(" ? ",
sizeof( unsigned char), 4u)); struct Cyc_PP_Doc* _temp1246= Cyc_Absynpp_exp2doc_prec(
0, _temp1083); struct Cyc_PP_Doc* _temp1247= Cyc_PP_text( _tag_arr(" : ",
sizeof( unsigned char), 4u)); struct Cyc_PP_Doc* _temp1248= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1081); struct Cyc_PP_Doc* _temp1242[ 5u]={ _temp1244, _temp1245,
_temp1246, _temp1247, _temp1248}; struct _tagged_arr _temp1243={( void*)
_temp1242,( void*) _temp1242,( void*)( _temp1242 + 5u)}; _temp1241( _temp1243);});
goto _LL1238; _LL1238:;} goto _LL986; _LL1002: s=({ struct Cyc_PP_Doc*(*
_temp1249)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1252= Cyc_PP_text(
_tag_arr("(", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1253= Cyc_Absynpp_exp2doc(
_temp1089); struct Cyc_PP_Doc* _temp1254= Cyc_PP_text( _tag_arr(", ", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp1255= Cyc_Absynpp_exp2doc(
_temp1087); struct Cyc_PP_Doc* _temp1256= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1250[ 5u]={ _temp1252, _temp1253,
_temp1254, _temp1255, _temp1256}; struct _tagged_arr _temp1251={( void*)
_temp1250,( void*) _temp1250,( void*)( _temp1250 + 5u)}; _temp1249( _temp1251);});
goto _LL986; _LL1004: s=({ struct Cyc_PP_Doc*(* _temp1257)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1260= Cyc_Absynpp_exp2doc_prec( myprec,
_temp1093); struct Cyc_PP_Doc* _temp1261= Cyc_PP_text( _tag_arr("(", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1262= Cyc_Absynpp_exps2doc_prec(
20, _temp1091); struct Cyc_PP_Doc* _temp1263= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1258[ 4u]={ _temp1260, _temp1261,
_temp1262, _temp1263}; struct _tagged_arr _temp1259={( void*) _temp1258,( void*)
_temp1258,( void*)( _temp1258 + 4u)}; _temp1257( _temp1259);}); goto _LL986;
_LL1006: s=({ struct Cyc_PP_Doc*(* _temp1264)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1267= Cyc_Absynpp_exp2doc_prec( myprec, _temp1099);
struct Cyc_PP_Doc* _temp1268= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp1269= Cyc_Absynpp_exps2doc_prec( 20, _temp1097);
struct Cyc_PP_Doc* _temp1270= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp1265[ 4u]={ _temp1267, _temp1268, _temp1269,
_temp1270}; struct _tagged_arr _temp1266={( void*) _temp1265,( void*) _temp1265,(
void*)( _temp1265 + 4u)}; _temp1264( _temp1266);}); goto _LL986; _LL1008: s=({
struct Cyc_PP_Doc*(* _temp1271)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1274= Cyc_PP_text( _tag_arr("throw ", sizeof( unsigned char), 7u)); struct
Cyc_PP_Doc* _temp1275= Cyc_Absynpp_exp2doc_prec( myprec, _temp1101); struct Cyc_PP_Doc*
_temp1272[ 2u]={ _temp1274, _temp1275}; struct _tagged_arr _temp1273={( void*)
_temp1272,( void*) _temp1272,( void*)( _temp1272 + 2u)}; _temp1271( _temp1273);});
goto _LL986; _LL1010: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp1103); goto
_LL986; _LL1012: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp1107); goto _LL986;
_LL1014: s=({ struct Cyc_PP_Doc*(* _temp1276)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1279= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp1280= Cyc_Absynpp_typ2doc( _temp1111); struct Cyc_PP_Doc*
_temp1281= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1282= Cyc_Absynpp_exp2doc_prec( myprec, _temp1109); struct Cyc_PP_Doc*
_temp1277[ 4u]={ _temp1279, _temp1280, _temp1281, _temp1282}; struct _tagged_arr
_temp1278={( void*) _temp1277,( void*) _temp1277,( void*)( _temp1277 + 4u)};
_temp1276( _temp1278);}); goto _LL986; _LL1016: s=({ struct Cyc_PP_Doc*(*
_temp1283)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1286= Cyc_PP_text(
_tag_arr("&", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1287= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1113); struct Cyc_PP_Doc* _temp1284[ 2u]={ _temp1286, _temp1287};
struct _tagged_arr _temp1285={( void*) _temp1284,( void*) _temp1284,( void*)(
_temp1284 + 2u)}; _temp1283( _temp1285);}); goto _LL986; _LL1018: if( _temp1117
== 0){ s=({ struct Cyc_PP_Doc*(* _temp1288)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1291= Cyc_PP_text( _tag_arr("new ", sizeof(
unsigned char), 5u)); struct Cyc_PP_Doc* _temp1292= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1115); struct Cyc_PP_Doc* _temp1289[ 2u]={ _temp1291, _temp1292};
struct _tagged_arr _temp1290={( void*) _temp1289,( void*) _temp1289,( void*)(
_temp1289 + 2u)}; _temp1288( _temp1290);});} else{ s=({ struct Cyc_PP_Doc*(*
_temp1293)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1296= Cyc_PP_text(
_tag_arr("rnew(", sizeof( unsigned char), 6u)); struct Cyc_PP_Doc* _temp1297=
Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp1117)); struct
Cyc_PP_Doc* _temp1298= Cyc_PP_text( _tag_arr(") ", sizeof( unsigned char), 3u));
struct Cyc_PP_Doc* _temp1299= Cyc_Absynpp_exp2doc_prec( myprec, _temp1115);
struct Cyc_PP_Doc* _temp1294[ 4u]={ _temp1296, _temp1297, _temp1298, _temp1299};
struct _tagged_arr _temp1295={( void*) _temp1294,( void*) _temp1294,( void*)(
_temp1294 + 4u)}; _temp1293( _temp1295);});} goto _LL986; _LL1020: s=({ struct
Cyc_PP_Doc*(* _temp1300)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1303= Cyc_PP_text( _tag_arr("sizeof(", sizeof( unsigned char), 8u)); struct
Cyc_PP_Doc* _temp1304= Cyc_Absynpp_typ2doc( _temp1119); struct Cyc_PP_Doc*
_temp1305= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1301[ 3u]={ _temp1303, _temp1304, _temp1305}; struct _tagged_arr _temp1302={(
void*) _temp1301,( void*) _temp1301,( void*)( _temp1301 + 3u)}; _temp1300(
_temp1302);}); goto _LL986; _LL1022: s=({ struct Cyc_PP_Doc*(* _temp1306)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1309= Cyc_PP_text(
_tag_arr("sizeof(", sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp1310=
Cyc_Absynpp_exp2doc( _temp1121); struct Cyc_PP_Doc* _temp1311= Cyc_PP_text(
_tag_arr(")", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1307[ 3u]={
_temp1309, _temp1310, _temp1311}; struct _tagged_arr _temp1308={( void*)
_temp1307,( void*) _temp1307,( void*)( _temp1307 + 3u)}; _temp1306( _temp1308);});
goto _LL986; _LL1024: s=({ struct Cyc_PP_Doc*(* _temp1312)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1315= Cyc_PP_text( _tag_arr("offsetof(",
sizeof( unsigned char), 10u)); struct Cyc_PP_Doc* _temp1316= Cyc_Absynpp_typ2doc(
_temp1125); struct Cyc_PP_Doc* _temp1317= Cyc_PP_text( _tag_arr(",", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1318= Cyc_PP_textptr( _temp1123);
struct Cyc_PP_Doc* _temp1319= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp1313[ 5u]={ _temp1315, _temp1316, _temp1317,
_temp1318, _temp1319}; struct _tagged_arr _temp1314={( void*) _temp1313,( void*)
_temp1313,( void*)( _temp1313 + 5u)}; _temp1312( _temp1314);}); goto _LL986;
_LL1026: s=({ struct Cyc_PP_Doc*(* _temp1320)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1323= Cyc_PP_text( _tag_arr("*", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp1324= Cyc_Absynpp_exp2doc_prec( myprec, _temp1127);
struct Cyc_PP_Doc* _temp1321[ 2u]={ _temp1323, _temp1324}; struct _tagged_arr
_temp1322={( void*) _temp1321,( void*) _temp1321,( void*)( _temp1321 + 2u)};
_temp1320( _temp1322);}); goto _LL986; _LL1028: s=({ struct Cyc_PP_Doc*(*
_temp1325)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1328= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1131); struct Cyc_PP_Doc* _temp1329= Cyc_PP_text( _tag_arr(".",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1330= Cyc_PP_textptr(
_temp1129); struct Cyc_PP_Doc* _temp1326[ 3u]={ _temp1328, _temp1329, _temp1330};
struct _tagged_arr _temp1327={( void*) _temp1326,( void*) _temp1326,( void*)(
_temp1326 + 3u)}; _temp1325( _temp1327);}); goto _LL986; _LL1030: s=({ struct
Cyc_PP_Doc*(* _temp1331)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1334= Cyc_Absynpp_exp2doc_prec( myprec, _temp1135); struct Cyc_PP_Doc*
_temp1335= Cyc_PP_text( _tag_arr("->", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc*
_temp1336= Cyc_PP_textptr( _temp1133); struct Cyc_PP_Doc* _temp1332[ 3u]={
_temp1334, _temp1335, _temp1336}; struct _tagged_arr _temp1333={( void*)
_temp1332,( void*) _temp1332,( void*)( _temp1332 + 3u)}; _temp1331( _temp1333);});
goto _LL986; _LL1032: s=({ struct Cyc_PP_Doc*(* _temp1337)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1340= Cyc_Absynpp_exp2doc_prec( myprec,
_temp1139); struct Cyc_PP_Doc* _temp1341= Cyc_PP_text( _tag_arr("[", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1342= Cyc_Absynpp_exp2doc(
_temp1137); struct Cyc_PP_Doc* _temp1343= Cyc_PP_text( _tag_arr("]", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1338[ 4u]={ _temp1340, _temp1341,
_temp1342, _temp1343}; struct _tagged_arr _temp1339={( void*) _temp1338,( void*)
_temp1338,( void*)( _temp1338 + 4u)}; _temp1337( _temp1339);}); goto _LL986;
_LL1034: s=({ struct Cyc_PP_Doc*(* _temp1344)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1347= Cyc_PP_text( _tag_arr("$(", sizeof( unsigned char),
3u)); struct Cyc_PP_Doc* _temp1348= Cyc_Absynpp_exps2doc_prec( 20, _temp1141);
struct Cyc_PP_Doc* _temp1349= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp1345[ 3u]={ _temp1347, _temp1348, _temp1349};
struct _tagged_arr _temp1346={( void*) _temp1345,( void*) _temp1345,( void*)(
_temp1345 + 3u)}; _temp1344( _temp1346);}); goto _LL986; _LL1036: s=({ struct
Cyc_PP_Doc*(* _temp1350)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1353= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1354= Cyc_Absynpp_typ2doc((* _temp1145).f3); struct Cyc_PP_Doc* _temp1355=
Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1356= Cyc_PP_group( _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_de2doc, _temp1143)); struct Cyc_PP_Doc* _temp1351[
4u]={ _temp1353, _temp1354, _temp1355, _temp1356}; struct _tagged_arr _temp1352={(
void*) _temp1351,( void*) _temp1351,( void*)( _temp1351 + 4u)}; _temp1350(
_temp1352);}); goto _LL986; _LL1038: s= Cyc_PP_group( _tag_arr("{", sizeof(
unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1147)); goto _LL986; _LL1040: s=({ struct Cyc_PP_Doc*(* _temp1357)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1360= Cyc_PP_text( _tag_arr("{for ",
sizeof( unsigned char), 6u)); struct Cyc_PP_Doc* _temp1361= Cyc_PP_text(*(*
_temp1153->name).f2); struct Cyc_PP_Doc* _temp1362= Cyc_PP_text( _tag_arr(" < ",
sizeof( unsigned char), 4u)); struct Cyc_PP_Doc* _temp1363= Cyc_Absynpp_exp2doc(
_temp1151); struct Cyc_PP_Doc* _temp1364= Cyc_PP_text( _tag_arr(" : ", sizeof(
unsigned char), 4u)); struct Cyc_PP_Doc* _temp1365= Cyc_Absynpp_exp2doc(
_temp1149); struct Cyc_PP_Doc* _temp1366= Cyc_PP_text( _tag_arr("}", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1358[ 7u]={ _temp1360, _temp1361,
_temp1362, _temp1363, _temp1364, _temp1365, _temp1366}; struct _tagged_arr
_temp1359={( void*) _temp1358,( void*) _temp1358,( void*)( _temp1358 + 7u)};
_temp1357( _temp1359);}); goto _LL986; _LL1042: s=({ struct Cyc_PP_Doc*(*
_temp1367)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1370= Cyc_Absynpp_qvar2doc(
_temp1161); struct Cyc_PP_Doc* _temp1371= Cyc_PP_group( _tag_arr("{", sizeof(
unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1157)); struct Cyc_PP_Doc* _temp1368[ 2u]={ _temp1370, _temp1371}; struct
_tagged_arr _temp1369={( void*) _temp1368,( void*) _temp1368,( void*)( _temp1368
+ 2u)}; _temp1367( _temp1369);}); goto _LL986; _LL1044: s= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp1163)); goto _LL986; _LL1046: if( _temp1171 == 0){ s=
Cyc_Absynpp_qvar2doc( _temp1167->name);} else{ s=({ struct Cyc_PP_Doc*(*
_temp1372)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1375= Cyc_Absynpp_qvar2doc(
_temp1167->name); struct Cyc_PP_Doc* _temp1376= Cyc_PP_egroup( _tag_arr("(",
sizeof( unsigned char), 2u), _tag_arr(")", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_exp2doc,
_temp1171)); struct Cyc_PP_Doc* _temp1373[ 2u]={ _temp1375, _temp1376}; struct
_tagged_arr _temp1374={( void*) _temp1373,( void*) _temp1373,( void*)( _temp1373
+ 2u)}; _temp1372( _temp1374);});} goto _LL986; _LL1048: s= Cyc_Absynpp_qvar2doc(
_temp1181); goto _LL986; _LL1050: if( _temp1185 == 0){ s=({ struct Cyc_PP_Doc*(*
_temp1377)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1380= Cyc_PP_text(
_tag_arr("malloc(sizeof(", sizeof( unsigned char), 15u)); struct Cyc_PP_Doc*
_temp1381= Cyc_Absynpp_typ2doc( _temp1183); struct Cyc_PP_Doc* _temp1382= Cyc_PP_text(
_tag_arr("))", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1378[ 3u]={
_temp1380, _temp1381, _temp1382}; struct _tagged_arr _temp1379={( void*)
_temp1378,( void*) _temp1378,( void*)( _temp1378 + 3u)}; _temp1377( _temp1379);});}
else{ s=({ struct Cyc_PP_Doc*(* _temp1383)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1386= Cyc_PP_text( _tag_arr("rmalloc(", sizeof(
unsigned char), 9u)); struct Cyc_PP_Doc* _temp1387= Cyc_Absynpp_exp2doc(( struct
Cyc_Absyn_Exp*) _check_null( _temp1185)); struct Cyc_PP_Doc* _temp1388= Cyc_PP_text(
_tag_arr(",", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1389= Cyc_PP_text(
_tag_arr("sizeof(", sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp1390=
Cyc_Absynpp_typ2doc( _temp1183); struct Cyc_PP_Doc* _temp1391= Cyc_PP_text(
_tag_arr("))", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1384[ 6u]={
_temp1386, _temp1387, _temp1388, _temp1389, _temp1390, _temp1391}; struct
_tagged_arr _temp1385={( void*) _temp1384,( void*) _temp1384,( void*)( _temp1384
+ 6u)}; _temp1383( _temp1385);});} goto _LL986; _LL1052: s= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp1187)); goto _LL986; _LL1054: s=({ struct Cyc_PP_Doc*(*
_temp1392)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1395= Cyc_PP_text(
_tag_arr("({ ", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc* _temp1396= Cyc_PP_nest(
2, Cyc_Absynpp_stmt2doc( _temp1191)); struct Cyc_PP_Doc* _temp1397= Cyc_PP_text(
_tag_arr(" })", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc* _temp1393[ 3u]={
_temp1395, _temp1396, _temp1397}; struct _tagged_arr _temp1394={( void*)
_temp1393,( void*) _temp1393,( void*)( _temp1393 + 3u)}; _temp1392( _temp1394);});
goto _LL986; _LL1056: s=({ struct Cyc_PP_Doc*(* _temp1398)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1401= Cyc_PP_text( _tag_arr("codegen(",
sizeof( unsigned char), 9u)); struct Cyc_PP_Doc* _temp1402= Cyc_PP_nest( 2, Cyc_Absynpp_decl2doc(({
struct Cyc_Absyn_Decl* _temp1404=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof(
struct Cyc_Absyn_Decl)); _temp1404->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp1405=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1405[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp1406; _temp1406.tag= Cyc_Absyn_Fn_d;
_temp1406.f1= _temp1193; _temp1406;}); _temp1405;})); _temp1404->loc= e->loc;
_temp1404;}))); struct Cyc_PP_Doc* _temp1403= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1399[ 3u]={ _temp1401, _temp1402,
_temp1403}; struct _tagged_arr _temp1400={( void*) _temp1399,( void*) _temp1399,(
void*)( _temp1399 + 3u)}; _temp1398( _temp1400);}); goto _LL986; _LL1058: s=({
struct Cyc_PP_Doc*(* _temp1407)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1410= Cyc_PP_text( _tag_arr("fill(", sizeof( unsigned char), 6u)); struct
Cyc_PP_Doc* _temp1411= Cyc_PP_nest( 2, Cyc_Absynpp_exp2doc( _temp1195)); struct
Cyc_PP_Doc* _temp1412= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u));
struct Cyc_PP_Doc* _temp1408[ 3u]={ _temp1410, _temp1411, _temp1412}; struct
_tagged_arr _temp1409={( void*) _temp1408,( void*) _temp1408,( void*)( _temp1408
+ 3u)}; _temp1407( _temp1409);}); goto _LL986; _LL986:;} if( inprec >= myprec){
s=({ struct Cyc_PP_Doc*(* _temp1413)( struct _tagged_arr)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp1416= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u));
struct Cyc_PP_Doc* _temp1417= s; struct Cyc_PP_Doc* _temp1418= Cyc_PP_text(
_tag_arr(")", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1414[ 3u]={
_temp1416, _temp1417, _temp1418}; struct _tagged_arr _temp1415={( void*)
_temp1414,( void*) _temp1414,( void*)( _temp1414 + 3u)}; _temp1413( _temp1415);});}
return s;} struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc( void* d){ void*
_temp1419= d; struct Cyc_Absyn_Exp* _temp1425; struct _tagged_arr* _temp1427;
_LL1421: if(*(( int*) _temp1419) == Cyc_Absyn_ArrayElement){ _LL1426: _temp1425=((
struct Cyc_Absyn_ArrayElement_struct*) _temp1419)->f1; goto _LL1422;} else{ goto
_LL1423;} _LL1423: if(*(( int*) _temp1419) == Cyc_Absyn_FieldName){ _LL1428:
_temp1427=(( struct Cyc_Absyn_FieldName_struct*) _temp1419)->f1; goto _LL1424;}
else{ goto _LL1420;} _LL1422: return({ struct Cyc_PP_Doc*(* _temp1429)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1432= Cyc_PP_text( _tag_arr(".[",
sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1433= Cyc_Absynpp_exp2doc(
_temp1425); struct Cyc_PP_Doc* _temp1434= Cyc_PP_text( _tag_arr("]", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1430[ 3u]={ _temp1432, _temp1433,
_temp1434}; struct _tagged_arr _temp1431={( void*) _temp1430,( void*) _temp1430,(
void*)( _temp1430 + 3u)}; _temp1429( _temp1431);}); _LL1424: return({ struct Cyc_PP_Doc*(*
_temp1435)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1438= Cyc_PP_text(
_tag_arr(".", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1439= Cyc_PP_textptr(
_temp1427); struct Cyc_PP_Doc* _temp1436[ 2u]={ _temp1438, _temp1439}; struct
_tagged_arr _temp1437={( void*) _temp1436,( void*) _temp1436,( void*)( _temp1436
+ 2u)}; _temp1435( _temp1437);}); _LL1420:;} struct Cyc_PP_Doc* Cyc_Absynpp_de2doc(
struct _tuple6* de){ if((* de).f1 == 0){ return Cyc_Absynpp_exp2doc((* de).f2);}
else{ return({ struct Cyc_PP_Doc*(* _temp1440)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1443= Cyc_PP_egroup( _tag_arr("", sizeof( unsigned char),
1u), _tag_arr("=", sizeof( unsigned char), 2u), _tag_arr("=", sizeof(
unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(* de).f1));
struct Cyc_PP_Doc* _temp1444= Cyc_Absynpp_exp2doc((* de).f2); struct Cyc_PP_Doc*
_temp1441[ 2u]={ _temp1443, _temp1444}; struct _tagged_arr _temp1442={( void*)
_temp1441,( void*) _temp1441,( void*)( _temp1441 + 2u)}; _temp1440( _temp1442);});}}
struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec( int inprec, struct Cyc_List_List*
es){ return Cyc_PP_group( _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("",
sizeof( unsigned char), 1u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*), int env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec, inprec, es));}
struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc( void* c){ void* _temp1445= c;
unsigned char _temp1463; void* _temp1465; short _temp1467; void* _temp1469; int
_temp1471; void* _temp1473; int _temp1475; void* _temp1477; long long _temp1479;
void* _temp1481; struct _tagged_arr _temp1483; struct _tagged_arr _temp1485;
_LL1447: if(( unsigned int) _temp1445 > 1u?*(( int*) _temp1445) == Cyc_Absyn_Char_c:
0){ _LL1466: _temp1465=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp1445)->f1;
goto _LL1464; _LL1464: _temp1463=(( struct Cyc_Absyn_Char_c_struct*) _temp1445)->f2;
goto _LL1448;} else{ goto _LL1449;} _LL1449: if(( unsigned int) _temp1445 > 1u?*((
int*) _temp1445) == Cyc_Absyn_Short_c: 0){ _LL1470: _temp1469=( void*)(( struct
Cyc_Absyn_Short_c_struct*) _temp1445)->f1; goto _LL1468; _LL1468: _temp1467=((
struct Cyc_Absyn_Short_c_struct*) _temp1445)->f2; goto _LL1450;} else{ goto
_LL1451;} _LL1451: if(( unsigned int) _temp1445 > 1u?*(( int*) _temp1445) == Cyc_Absyn_Int_c:
0){ _LL1474: _temp1473=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1445)->f1;
if( _temp1473 ==( void*) Cyc_Absyn_Signed){ goto _LL1472;} else{ goto _LL1453;}
_LL1472: _temp1471=(( struct Cyc_Absyn_Int_c_struct*) _temp1445)->f2; goto
_LL1452;} else{ goto _LL1453;} _LL1453: if(( unsigned int) _temp1445 > 1u?*((
int*) _temp1445) == Cyc_Absyn_Int_c: 0){ _LL1478: _temp1477=( void*)(( struct
Cyc_Absyn_Int_c_struct*) _temp1445)->f1; if( _temp1477 ==( void*) Cyc_Absyn_Unsigned){
goto _LL1476;} else{ goto _LL1455;} _LL1476: _temp1475=(( struct Cyc_Absyn_Int_c_struct*)
_temp1445)->f2; goto _LL1454;} else{ goto _LL1455;} _LL1455: if(( unsigned int)
_temp1445 > 1u?*(( int*) _temp1445) == Cyc_Absyn_LongLong_c: 0){ _LL1482:
_temp1481=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp1445)->f1; goto
_LL1480; _LL1480: _temp1479=(( struct Cyc_Absyn_LongLong_c_struct*) _temp1445)->f2;
goto _LL1456;} else{ goto _LL1457;} _LL1457: if(( unsigned int) _temp1445 > 1u?*((
int*) _temp1445) == Cyc_Absyn_Float_c: 0){ _LL1484: _temp1483=(( struct Cyc_Absyn_Float_c_struct*)
_temp1445)->f1; goto _LL1458;} else{ goto _LL1459;} _LL1459: if( _temp1445 ==(
void*) Cyc_Absyn_Null_c){ goto _LL1460;} else{ goto _LL1461;} _LL1461: if((
unsigned int) _temp1445 > 1u?*(( int*) _temp1445) == Cyc_Absyn_String_c: 0){
_LL1486: _temp1485=(( struct Cyc_Absyn_String_c_struct*) _temp1445)->f1; goto
_LL1462;} else{ goto _LL1446;} _LL1448: return Cyc_PP_text(( struct _tagged_arr)({
struct _tagged_arr _temp1487= Cyc_Absynpp_char_escape( _temp1463); xprintf("'%.*s'",
_get_arr_size( _temp1487, 1u), _temp1487.curr);})); _LL1450: return Cyc_PP_text((
struct _tagged_arr) xprintf("%d",( int) _temp1467)); _LL1452: return Cyc_PP_text((
struct _tagged_arr) Cyc_Core_string_of_int( _temp1471)); _LL1454: return Cyc_PP_text((
struct _tagged_arr) xprintf("%u",( unsigned int) _temp1475)); _LL1456: return
Cyc_PP_text( _tag_arr("<<FIX LONG LONG CONSTANT>>", sizeof( unsigned char), 27u));
_LL1458: return Cyc_PP_text( _temp1483); _LL1460: return Cyc_PP_text( _tag_arr("null",
sizeof( unsigned char), 5u)); _LL1462: return({ struct Cyc_PP_Doc*(* _temp1488)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1491= Cyc_PP_text(
_tag_arr("\"", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1492= Cyc_PP_text(
Cyc_Absynpp_string_escape( _temp1485)); struct Cyc_PP_Doc* _temp1493= Cyc_PP_text(
_tag_arr("\"", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1489[ 3u]={
_temp1491, _temp1492, _temp1493}; struct _tagged_arr _temp1490={( void*)
_temp1489,( void*) _temp1489,( void*)( _temp1489 + 3u)}; _temp1488( _temp1490);});
_LL1446:;} struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc( int inprec, void* p,
struct Cyc_List_List* es){ struct Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc( p); if(
p ==( void*) Cyc_Absyn_Size){ if( es == 0? 1:(( struct Cyc_List_List*)
_check_null( es))->tl != 0){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp1494=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1494[ 0]=({ struct Cyc_Core_Failure_struct _temp1495; _temp1495.tag= Cyc_Core_Failure;
_temp1495.f1=( struct _tagged_arr)({ struct _tagged_arr _temp1496= Cyc_PP_string_of_doc(
ps, 72); xprintf("Absynpp::primapp2doc Size: %.*s with bad args", _get_arr_size(
_temp1496, 1u), _temp1496.curr);}); _temp1495;}); _temp1494;}));}{ struct Cyc_PP_Doc*
_temp1497= Cyc_Absynpp_exp2doc_prec( inprec,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd); return({ struct Cyc_PP_Doc*(* _temp1498)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1501= _temp1497; struct
Cyc_PP_Doc* _temp1502= Cyc_PP_text( _tag_arr(".size", sizeof( unsigned char), 6u));
struct Cyc_PP_Doc* _temp1499[ 2u]={ _temp1501, _temp1502}; struct _tagged_arr
_temp1500={( void*) _temp1499,( void*) _temp1499,( void*)( _temp1499 + 2u)};
_temp1498( _temp1500);});}} else{ if( ! Cyc_Absyn_is_format_prim( p)){ struct
Cyc_List_List* ds=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int,
struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec,
inprec, es); if( ds == 0){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp1503=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1503[ 0]=({ struct Cyc_Core_Failure_struct _temp1504; _temp1504.tag= Cyc_Core_Failure;
_temp1504.f1=( struct _tagged_arr)({ struct _tagged_arr _temp1505= Cyc_PP_string_of_doc(
ps, 72); xprintf("Absynpp::primapp2doc: %.*s with no args", _get_arr_size(
_temp1505, 1u), _temp1505.curr);}); _temp1504;}); _temp1503;}));} else{ if(((
struct Cyc_List_List*) _check_null( ds))->tl == 0){ return({ struct Cyc_PP_Doc*(*
_temp1506)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1509= ps;
struct Cyc_PP_Doc* _temp1510= Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp1511=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*)
_check_null( ds))->hd; struct Cyc_PP_Doc* _temp1507[ 3u]={ _temp1509, _temp1510,
_temp1511}; struct _tagged_arr _temp1508={( void*) _temp1507,( void*) _temp1507,(
void*)( _temp1507 + 3u)}; _temp1506( _temp1508);});} else{ if((( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( ds))->tl))->tl != 0){( int)
_throw(( void*)({ struct Cyc_Core_Failure_struct* _temp1512=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1512[ 0]=({ struct Cyc_Core_Failure_struct
_temp1513; _temp1513.tag= Cyc_Core_Failure; _temp1513.f1=( struct _tagged_arr)({
struct _tagged_arr _temp1514= Cyc_PP_string_of_doc( ps, 72); xprintf("Absynpp::primapp2doc: %.*s with more than 2 args",
_get_arr_size( _temp1514, 1u), _temp1514.curr);}); _temp1513;}); _temp1512;}));}
else{ return({ struct Cyc_PP_Doc*(* _temp1515)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1518=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*)
_check_null( ds))->hd; struct Cyc_PP_Doc* _temp1519= Cyc_PP_text( _tag_arr(" ",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1520= ps; struct Cyc_PP_Doc*
_temp1521= Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1522=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null((( struct
Cyc_List_List*) _check_null( ds))->tl))->hd; struct Cyc_PP_Doc* _temp1516[ 5u]={
_temp1518, _temp1519, _temp1520, _temp1521, _temp1522}; struct _tagged_arr
_temp1517={( void*) _temp1516,( void*) _temp1516,( void*)( _temp1516 + 5u)};
_temp1515( _temp1517);});}}}} else{ return({ struct Cyc_PP_Doc*(* _temp1523)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1526= ps; struct Cyc_PP_Doc*
_temp1527= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1528= Cyc_Absynpp_exps2doc_prec( 20, es); struct Cyc_PP_Doc* _temp1529= Cyc_PP_text(
_tag_arr(")", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1524[ 4u]={
_temp1526, _temp1527, _temp1528, _temp1529}; struct _tagged_arr _temp1525={(
void*) _temp1524,( void*) _temp1524,( void*)( _temp1524 + 4u)}; _temp1523(
_temp1525);});}}} struct _tagged_arr Cyc_Absynpp_prim2str( void* p){ void*
_temp1530= p; _LL1532: if( _temp1530 ==( void*) Cyc_Absyn_Plus){ goto _LL1533;}
else{ goto _LL1534;} _LL1534: if( _temp1530 ==( void*) Cyc_Absyn_Times){ goto
_LL1535;} else{ goto _LL1536;} _LL1536: if( _temp1530 ==( void*) Cyc_Absyn_Minus){
goto _LL1537;} else{ goto _LL1538;} _LL1538: if( _temp1530 ==( void*) Cyc_Absyn_Div){
goto _LL1539;} else{ goto _LL1540;} _LL1540: if( _temp1530 ==( void*) Cyc_Absyn_Mod){
goto _LL1541;} else{ goto _LL1542;} _LL1542: if( _temp1530 ==( void*) Cyc_Absyn_Eq){
goto _LL1543;} else{ goto _LL1544;} _LL1544: if( _temp1530 ==( void*) Cyc_Absyn_Neq){
goto _LL1545;} else{ goto _LL1546;} _LL1546: if( _temp1530 ==( void*) Cyc_Absyn_Gt){
goto _LL1547;} else{ goto _LL1548;} _LL1548: if( _temp1530 ==( void*) Cyc_Absyn_Lt){
goto _LL1549;} else{ goto _LL1550;} _LL1550: if( _temp1530 ==( void*) Cyc_Absyn_Gte){
goto _LL1551;} else{ goto _LL1552;} _LL1552: if( _temp1530 ==( void*) Cyc_Absyn_Lte){
goto _LL1553;} else{ goto _LL1554;} _LL1554: if( _temp1530 ==( void*) Cyc_Absyn_Not){
goto _LL1555;} else{ goto _LL1556;} _LL1556: if( _temp1530 ==( void*) Cyc_Absyn_Bitnot){
goto _LL1557;} else{ goto _LL1558;} _LL1558: if( _temp1530 ==( void*) Cyc_Absyn_Bitand){
goto _LL1559;} else{ goto _LL1560;} _LL1560: if( _temp1530 ==( void*) Cyc_Absyn_Bitor){
goto _LL1561;} else{ goto _LL1562;} _LL1562: if( _temp1530 ==( void*) Cyc_Absyn_Bitxor){
goto _LL1563;} else{ goto _LL1564;} _LL1564: if( _temp1530 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL1565;} else{ goto _LL1566;} _LL1566: if( _temp1530 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL1567;} else{ goto _LL1568;} _LL1568: if( _temp1530 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL1569;} else{ goto _LL1570;} _LL1570: if( _temp1530 ==( void*) Cyc_Absyn_Size){
goto _LL1571;} else{ goto _LL1572;} _LL1572: if( _temp1530 ==( void*) Cyc_Absyn_Printf){
goto _LL1573;} else{ goto _LL1574;} _LL1574: if( _temp1530 ==( void*) Cyc_Absyn_Fprintf){
goto _LL1575;} else{ goto _LL1576;} _LL1576: if( _temp1530 ==( void*) Cyc_Absyn_Xprintf){
goto _LL1577;} else{ goto _LL1578;} _LL1578: if( _temp1530 ==( void*) Cyc_Absyn_Scanf){
goto _LL1579;} else{ goto _LL1580;} _LL1580: if( _temp1530 ==( void*) Cyc_Absyn_Fscanf){
goto _LL1581;} else{ goto _LL1582;} _LL1582: if( _temp1530 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1583;} else{ goto _LL1531;} _LL1533: return _tag_arr("+", sizeof(
unsigned char), 2u); _LL1535: return _tag_arr("*", sizeof( unsigned char), 2u);
_LL1537: return _tag_arr("-", sizeof( unsigned char), 2u); _LL1539: return
_tag_arr("/", sizeof( unsigned char), 2u); _LL1541: return _tag_arr("%", sizeof(
unsigned char), 2u); _LL1543: return _tag_arr("==", sizeof( unsigned char), 3u);
_LL1545: return _tag_arr("!=", sizeof( unsigned char), 3u); _LL1547: return
_tag_arr(">", sizeof( unsigned char), 2u); _LL1549: return _tag_arr("<", sizeof(
unsigned char), 2u); _LL1551: return _tag_arr(">=", sizeof( unsigned char), 3u);
_LL1553: return _tag_arr("<=", sizeof( unsigned char), 3u); _LL1555: return
_tag_arr("!", sizeof( unsigned char), 2u); _LL1557: return _tag_arr("~", sizeof(
unsigned char), 2u); _LL1559: return _tag_arr("&", sizeof( unsigned char), 2u);
_LL1561: return _tag_arr("|", sizeof( unsigned char), 2u); _LL1563: return
_tag_arr("^", sizeof( unsigned char), 2u); _LL1565: return _tag_arr("<<",
sizeof( unsigned char), 3u); _LL1567: return _tag_arr(">>", sizeof(
unsigned char), 3u); _LL1569: return _tag_arr(">>>", sizeof( unsigned char), 4u);
_LL1571: return _tag_arr("size", sizeof( unsigned char), 5u); _LL1573: return
_tag_arr("printf", sizeof( unsigned char), 7u); _LL1575: return _tag_arr("fprintf",
sizeof( unsigned char), 8u); _LL1577: return _tag_arr("xprintf", sizeof(
unsigned char), 8u); _LL1579: return _tag_arr("scanf", sizeof( unsigned char), 6u);
_LL1581: return _tag_arr("fscanf", sizeof( unsigned char), 7u); _LL1583: return
_tag_arr("sscanf", sizeof( unsigned char), 7u); _LL1531:;} struct Cyc_PP_Doc*
Cyc_Absynpp_prim2doc( void* p){ return Cyc_PP_text( Cyc_Absynpp_prim2str( p));}
struct _tuple9{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ; struct
Cyc_PP_Doc* Cyc_Absynpp_structarg2doc( struct _tuple9* t){ if((* t).f1 == 0){
return Cyc_Absynpp_exp2doc((* t).f2);} else{ return({ struct Cyc_PP_Doc*(*
_temp1584)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1587= Cyc_PP_textptr((
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v); struct
Cyc_PP_Doc* _temp1588= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u));
struct Cyc_PP_Doc* _temp1589= Cyc_Absynpp_exp2doc((* t).f2); struct Cyc_PP_Doc*
_temp1585[ 3u]={ _temp1587, _temp1588, _temp1589}; struct _tagged_arr _temp1586={(
void*) _temp1585,( void*) _temp1585,( void*)( _temp1585 + 3u)}; _temp1584(
_temp1586);});}} int Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s){ void*
_temp1590=( void*) s->r; struct Cyc_Absyn_Stmt* _temp1596; struct Cyc_Absyn_Decl*
_temp1598; _LL1592: if(( unsigned int) _temp1590 > 1u?*(( int*) _temp1590) ==
Cyc_Absyn_Decl_s: 0){ _LL1599: _temp1598=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1590)->f1; goto _LL1597; _LL1597: _temp1596=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1590)->f2; goto _LL1593;} else{ goto _LL1594;} _LL1594: goto _LL1595;
_LL1593: return 1; _LL1595: return 0; _LL1591:;} struct Cyc_PP_Doc* Cyc_Absynpp_stmt2doc(
struct Cyc_Absyn_Stmt* st){ struct Cyc_PP_Doc* s;{ void* _temp1600=( void*) st->r;
struct Cyc_Absyn_Exp* _temp1644; struct Cyc_Absyn_Stmt* _temp1646; struct Cyc_Absyn_Stmt*
_temp1648; struct Cyc_Absyn_Exp* _temp1650; struct Cyc_Absyn_Stmt* _temp1652;
struct Cyc_Absyn_Stmt* _temp1654; struct Cyc_Absyn_Exp* _temp1656; struct Cyc_Absyn_Stmt*
_temp1658; struct _tuple2 _temp1660; struct Cyc_Absyn_Stmt* _temp1662; struct
Cyc_Absyn_Exp* _temp1664; struct Cyc_Absyn_Stmt* _temp1666; struct Cyc_Absyn_Stmt*
_temp1668; struct Cyc_Absyn_Stmt* _temp1670; struct _tagged_arr* _temp1672;
struct Cyc_Absyn_Stmt* _temp1674; struct _tuple2 _temp1676; struct Cyc_Absyn_Stmt*
_temp1678; struct Cyc_Absyn_Exp* _temp1680; struct _tuple2 _temp1682; struct Cyc_Absyn_Stmt*
_temp1684; struct Cyc_Absyn_Exp* _temp1686; struct Cyc_Absyn_Exp* _temp1688;
struct Cyc_List_List* _temp1690; struct Cyc_Absyn_Exp* _temp1692; struct Cyc_List_List*
_temp1694; struct Cyc_Absyn_Exp* _temp1696; struct Cyc_Absyn_Switch_clause**
_temp1698; struct Cyc_List_List* _temp1700; struct Cyc_Absyn_Switch_clause**
_temp1702; struct Cyc_List_List* _temp1704; struct Cyc_Absyn_Stmt* _temp1706;
struct Cyc_Absyn_Decl* _temp1708; struct Cyc_Absyn_Stmt* _temp1710; struct Cyc_Absyn_Stmt*
_temp1712; struct Cyc_Absyn_Stmt* _temp1714; struct _tagged_arr* _temp1716;
struct _tuple2 _temp1718; struct Cyc_Absyn_Stmt* _temp1720; struct Cyc_Absyn_Exp*
_temp1722; struct Cyc_Absyn_Stmt* _temp1724; struct Cyc_List_List* _temp1726;
struct Cyc_Absyn_Stmt* _temp1728; struct Cyc_Absyn_Stmt* _temp1730; struct Cyc_Absyn_Vardecl*
_temp1732; struct Cyc_Absyn_Tvar* _temp1734; _LL1602: if( _temp1600 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL1603;} else{ goto _LL1604;} _LL1604: if((
unsigned int) _temp1600 > 1u?*(( int*) _temp1600) == Cyc_Absyn_Exp_s: 0){
_LL1645: _temp1644=(( struct Cyc_Absyn_Exp_s_struct*) _temp1600)->f1; goto
_LL1605;} else{ goto _LL1606;} _LL1606: if(( unsigned int) _temp1600 > 1u?*((
int*) _temp1600) == Cyc_Absyn_Seq_s: 0){ _LL1649: _temp1648=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1600)->f1; goto _LL1647; _LL1647: _temp1646=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1600)->f2; goto _LL1607;} else{ goto _LL1608;} _LL1608: if(( unsigned int)
_temp1600 > 1u?*(( int*) _temp1600) == Cyc_Absyn_Return_s: 0){ _LL1651:
_temp1650=(( struct Cyc_Absyn_Return_s_struct*) _temp1600)->f1; goto _LL1609;}
else{ goto _LL1610;} _LL1610: if(( unsigned int) _temp1600 > 1u?*(( int*)
_temp1600) == Cyc_Absyn_IfThenElse_s: 0){ _LL1657: _temp1656=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1600)->f1; goto _LL1655; _LL1655: _temp1654=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1600)->f2; goto _LL1653; _LL1653: _temp1652=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1600)->f3; goto _LL1611;} else{ goto _LL1612;} _LL1612: if(( unsigned int)
_temp1600 > 1u?*(( int*) _temp1600) == Cyc_Absyn_While_s: 0){ _LL1661: _temp1660=((
struct Cyc_Absyn_While_s_struct*) _temp1600)->f1; _LL1665: _temp1664= _temp1660.f1;
goto _LL1663; _LL1663: _temp1662= _temp1660.f2; goto _LL1659; _LL1659: _temp1658=((
struct Cyc_Absyn_While_s_struct*) _temp1600)->f2; goto _LL1613;} else{ goto
_LL1614;} _LL1614: if(( unsigned int) _temp1600 > 1u?*(( int*) _temp1600) == Cyc_Absyn_Break_s:
0){ _LL1667: _temp1666=(( struct Cyc_Absyn_Break_s_struct*) _temp1600)->f1; goto
_LL1615;} else{ goto _LL1616;} _LL1616: if(( unsigned int) _temp1600 > 1u?*((
int*) _temp1600) == Cyc_Absyn_Continue_s: 0){ _LL1669: _temp1668=(( struct Cyc_Absyn_Continue_s_struct*)
_temp1600)->f1; goto _LL1617;} else{ goto _LL1618;} _LL1618: if(( unsigned int)
_temp1600 > 1u?*(( int*) _temp1600) == Cyc_Absyn_Goto_s: 0){ _LL1673: _temp1672=((
struct Cyc_Absyn_Goto_s_struct*) _temp1600)->f1; goto _LL1671; _LL1671:
_temp1670=(( struct Cyc_Absyn_Goto_s_struct*) _temp1600)->f2; goto _LL1619;}
else{ goto _LL1620;} _LL1620: if(( unsigned int) _temp1600 > 1u?*(( int*)
_temp1600) == Cyc_Absyn_For_s: 0){ _LL1689: _temp1688=(( struct Cyc_Absyn_For_s_struct*)
_temp1600)->f1; goto _LL1683; _LL1683: _temp1682=(( struct Cyc_Absyn_For_s_struct*)
_temp1600)->f2; _LL1687: _temp1686= _temp1682.f1; goto _LL1685; _LL1685:
_temp1684= _temp1682.f2; goto _LL1677; _LL1677: _temp1676=(( struct Cyc_Absyn_For_s_struct*)
_temp1600)->f3; _LL1681: _temp1680= _temp1676.f1; goto _LL1679; _LL1679:
_temp1678= _temp1676.f2; goto _LL1675; _LL1675: _temp1674=(( struct Cyc_Absyn_For_s_struct*)
_temp1600)->f4; goto _LL1621;} else{ goto _LL1622;} _LL1622: if(( unsigned int)
_temp1600 > 1u?*(( int*) _temp1600) == Cyc_Absyn_Switch_s: 0){ _LL1693:
_temp1692=(( struct Cyc_Absyn_Switch_s_struct*) _temp1600)->f1; goto _LL1691;
_LL1691: _temp1690=(( struct Cyc_Absyn_Switch_s_struct*) _temp1600)->f2; goto
_LL1623;} else{ goto _LL1624;} _LL1624: if(( unsigned int) _temp1600 > 1u?*((
int*) _temp1600) == Cyc_Absyn_SwitchC_s: 0){ _LL1697: _temp1696=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp1600)->f1; goto _LL1695; _LL1695: _temp1694=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp1600)->f2; goto _LL1625;} else{ goto _LL1626;} _LL1626: if(( unsigned int)
_temp1600 > 1u?*(( int*) _temp1600) == Cyc_Absyn_Fallthru_s: 0){ _LL1701:
_temp1700=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp1600)->f1; if( _temp1700
== 0){ goto _LL1699;} else{ goto _LL1628;} _LL1699: _temp1698=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1600)->f2; goto _LL1627;} else{ goto _LL1628;} _LL1628: if(( unsigned int)
_temp1600 > 1u?*(( int*) _temp1600) == Cyc_Absyn_Fallthru_s: 0){ _LL1705:
_temp1704=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp1600)->f1; goto _LL1703;
_LL1703: _temp1702=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp1600)->f2; goto
_LL1629;} else{ goto _LL1630;} _LL1630: if(( unsigned int) _temp1600 > 1u?*((
int*) _temp1600) == Cyc_Absyn_Decl_s: 0){ _LL1709: _temp1708=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1600)->f1; goto _LL1707; _LL1707: _temp1706=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1600)->f2; goto _LL1631;} else{ goto _LL1632;} _LL1632: if(( unsigned int)
_temp1600 > 1u?*(( int*) _temp1600) == Cyc_Absyn_Cut_s: 0){ _LL1711: _temp1710=((
struct Cyc_Absyn_Cut_s_struct*) _temp1600)->f1; goto _LL1633;} else{ goto
_LL1634;} _LL1634: if(( unsigned int) _temp1600 > 1u?*(( int*) _temp1600) == Cyc_Absyn_Splice_s:
0){ _LL1713: _temp1712=(( struct Cyc_Absyn_Splice_s_struct*) _temp1600)->f1;
goto _LL1635;} else{ goto _LL1636;} _LL1636: if(( unsigned int) _temp1600 > 1u?*((
int*) _temp1600) == Cyc_Absyn_Label_s: 0){ _LL1717: _temp1716=(( struct Cyc_Absyn_Label_s_struct*)
_temp1600)->f1; goto _LL1715; _LL1715: _temp1714=(( struct Cyc_Absyn_Label_s_struct*)
_temp1600)->f2; goto _LL1637;} else{ goto _LL1638;} _LL1638: if(( unsigned int)
_temp1600 > 1u?*(( int*) _temp1600) == Cyc_Absyn_Do_s: 0){ _LL1725: _temp1724=((
struct Cyc_Absyn_Do_s_struct*) _temp1600)->f1; goto _LL1719; _LL1719: _temp1718=((
struct Cyc_Absyn_Do_s_struct*) _temp1600)->f2; _LL1723: _temp1722= _temp1718.f1;
goto _LL1721; _LL1721: _temp1720= _temp1718.f2; goto _LL1639;} else{ goto
_LL1640;} _LL1640: if(( unsigned int) _temp1600 > 1u?*(( int*) _temp1600) == Cyc_Absyn_TryCatch_s:
0){ _LL1729: _temp1728=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp1600)->f1;
goto _LL1727; _LL1727: _temp1726=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1600)->f2; goto _LL1641;} else{ goto _LL1642;} _LL1642: if(( unsigned int)
_temp1600 > 1u?*(( int*) _temp1600) == Cyc_Absyn_Region_s: 0){ _LL1735:
_temp1734=(( struct Cyc_Absyn_Region_s_struct*) _temp1600)->f1; goto _LL1733;
_LL1733: _temp1732=(( struct Cyc_Absyn_Region_s_struct*) _temp1600)->f2; goto
_LL1731; _LL1731: _temp1730=(( struct Cyc_Absyn_Region_s_struct*) _temp1600)->f3;
goto _LL1643;} else{ goto _LL1601;} _LL1603: s= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); goto _LL1601; _LL1605: s=({ struct Cyc_PP_Doc*(*
_temp1736)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1739= Cyc_Absynpp_exp2doc(
_temp1644); struct Cyc_PP_Doc* _temp1740= Cyc_PP_text( _tag_arr(";", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1737[ 2u]={ _temp1739, _temp1740};
struct _tagged_arr _temp1738={( void*) _temp1737,( void*) _temp1737,( void*)(
_temp1737 + 2u)}; _temp1736( _temp1738);}); goto _LL1601; _LL1607: if( Cyc_Absynpp_decls_first){
if( Cyc_Absynpp_is_declaration( _temp1648)){ s=({ struct Cyc_PP_Doc*(* _temp1741)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1744= Cyc_PP_text(
_tag_arr("{ ", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1745= Cyc_PP_nest(
2, Cyc_Absynpp_stmt2doc( _temp1648)); struct Cyc_PP_Doc* _temp1746= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1747= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp1748= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp1749= Cyc_Absynpp_is_declaration( _temp1646)?({ struct Cyc_PP_Doc*(*
_temp1750)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1753= Cyc_PP_text(
_tag_arr("{ ", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1754= Cyc_PP_nest(
2, Cyc_Absynpp_stmt2doc( _temp1646)); struct Cyc_PP_Doc* _temp1755= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1756= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1751[ 4u]={ _temp1753, _temp1754, _temp1755, _temp1756};
struct _tagged_arr _temp1752={( void*) _temp1751,( void*) _temp1751,( void*)(
_temp1751 + 4u)}; _temp1750( _temp1752);}): Cyc_Absynpp_stmt2doc( _temp1646);
struct Cyc_PP_Doc* _temp1742[ 6u]={ _temp1744, _temp1745, _temp1746, _temp1747,
_temp1748, _temp1749}; struct _tagged_arr _temp1743={( void*) _temp1742,( void*)
_temp1742,( void*)( _temp1742 + 6u)}; _temp1741( _temp1743);});} else{ if( Cyc_Absynpp_is_declaration(
_temp1646)){ s=({ struct Cyc_PP_Doc*(* _temp1757)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1760= Cyc_Absynpp_stmt2doc( _temp1648); struct Cyc_PP_Doc*
_temp1761= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1762= Cyc_PP_text(
_tag_arr("{ ", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1763= Cyc_PP_nest(
2, Cyc_Absynpp_stmt2doc( _temp1646)); struct Cyc_PP_Doc* _temp1764= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1765= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1758[ 6u]={ _temp1760, _temp1761, _temp1762, _temp1763,
_temp1764, _temp1765}; struct _tagged_arr _temp1759={( void*) _temp1758,( void*)
_temp1758,( void*)( _temp1758 + 6u)}; _temp1757( _temp1759);});} else{ s=((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct
_tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,
_tag_arr("", sizeof( unsigned char), 1u),({ struct Cyc_List_List* _temp1766=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1766->hd=(
void*) _temp1648; _temp1766->tl=({ struct Cyc_List_List* _temp1767=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1767->hd=( void*) _temp1646;
_temp1767->tl= 0; _temp1767;}); _temp1766;}));}}} else{ s=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct _tagged_arr sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc, _tag_arr("",
sizeof( unsigned char), 1u),({ struct Cyc_List_List* _temp1768=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1768->hd=( void*) _temp1648;
_temp1768->tl=({ struct Cyc_List_List* _temp1769=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1769->hd=( void*) _temp1646;
_temp1769->tl= 0; _temp1769;}); _temp1768;}));} goto _LL1601; _LL1609: if(
_temp1650 == 0){ s= Cyc_PP_text( _tag_arr("return;", sizeof( unsigned char), 8u));}
else{ s=({ struct Cyc_PP_Doc*(* _temp1770)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1773= Cyc_PP_text( _tag_arr("return ", sizeof(
unsigned char), 8u)); struct Cyc_PP_Doc* _temp1774= _temp1650 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp1650)); struct
Cyc_PP_Doc* _temp1775= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u));
struct Cyc_PP_Doc* _temp1771[ 3u]={ _temp1773, _temp1774, _temp1775}; struct
_tagged_arr _temp1772={( void*) _temp1771,( void*) _temp1771,( void*)( _temp1771
+ 3u)}; _temp1770( _temp1772);});} goto _LL1601; _LL1611: { int print_else;{
void* _temp1776=( void*) _temp1652->r; _LL1778: if( _temp1776 ==( void*) Cyc_Absyn_Skip_s){
goto _LL1779;} else{ goto _LL1780;} _LL1780: goto _LL1781; _LL1779: print_else=
0; goto _LL1777; _LL1781: print_else= 1; goto _LL1777; _LL1777:;} s=({ struct
Cyc_PP_Doc*(* _temp1782)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1785= Cyc_PP_text( _tag_arr("if (", sizeof( unsigned char), 5u)); struct
Cyc_PP_Doc* _temp1786= Cyc_Absynpp_exp2doc( _temp1656); struct Cyc_PP_Doc*
_temp1787= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc*
_temp1788= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp1805)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1808= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp1809= Cyc_Absynpp_stmt2doc( _temp1654); struct Cyc_PP_Doc* _temp1806[ 2u]={
_temp1808, _temp1809}; struct _tagged_arr _temp1807={( void*) _temp1806,( void*)
_temp1806,( void*)( _temp1806 + 2u)}; _temp1805( _temp1807);})); struct Cyc_PP_Doc*
_temp1789= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1790= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1791=
print_else?({ struct Cyc_PP_Doc*(* _temp1792)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1795= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1796=
Cyc_PP_text( _tag_arr("else {", sizeof( unsigned char), 7u)); struct Cyc_PP_Doc*
_temp1797= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp1800)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1803= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp1804= Cyc_Absynpp_stmt2doc( _temp1652); struct Cyc_PP_Doc* _temp1801[ 2u]={
_temp1803, _temp1804}; struct _tagged_arr _temp1802={( void*) _temp1801,( void*)
_temp1801,( void*)( _temp1801 + 2u)}; _temp1800( _temp1802);})); struct Cyc_PP_Doc*
_temp1798= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1799= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1793[ 5u]={
_temp1795, _temp1796, _temp1797, _temp1798, _temp1799}; struct _tagged_arr
_temp1794={( void*) _temp1793,( void*) _temp1793,( void*)( _temp1793 + 5u)};
_temp1792( _temp1794);}): Cyc_PP_nil_doc(); struct Cyc_PP_Doc* _temp1783[ 7u]={
_temp1785, _temp1786, _temp1787, _temp1788, _temp1789, _temp1790, _temp1791};
struct _tagged_arr _temp1784={( void*) _temp1783,( void*) _temp1783,( void*)(
_temp1783 + 7u)}; _temp1782( _temp1784);}); goto _LL1601;} _LL1613: s=({ struct
Cyc_PP_Doc*(* _temp1810)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1813= Cyc_PP_text( _tag_arr("while (", sizeof( unsigned char), 8u)); struct
Cyc_PP_Doc* _temp1814= Cyc_Absynpp_exp2doc( _temp1664); struct Cyc_PP_Doc*
_temp1815= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc*
_temp1816= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp1819)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1822= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp1823= Cyc_Absynpp_stmt2doc( _temp1658); struct Cyc_PP_Doc* _temp1820[ 2u]={
_temp1822, _temp1823}; struct _tagged_arr _temp1821={( void*) _temp1820,( void*)
_temp1820,( void*)( _temp1820 + 2u)}; _temp1819( _temp1821);})); struct Cyc_PP_Doc*
_temp1817= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1818= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1811[ 6u]={
_temp1813, _temp1814, _temp1815, _temp1816, _temp1817, _temp1818}; struct
_tagged_arr _temp1812={( void*) _temp1811,( void*) _temp1811,( void*)( _temp1811
+ 6u)}; _temp1810( _temp1812);}); goto _LL1601; _LL1615: s= Cyc_PP_text(
_tag_arr("break;", sizeof( unsigned char), 7u)); goto _LL1601; _LL1617: s= Cyc_PP_text(
_tag_arr("continue;", sizeof( unsigned char), 10u)); goto _LL1601; _LL1619: s=
Cyc_PP_text(( struct _tagged_arr)({ struct _tagged_arr _temp1824=* _temp1672;
xprintf("goto %.*s;", _get_arr_size( _temp1824, 1u), _temp1824.curr);})); goto
_LL1601; _LL1621: s=({ struct Cyc_PP_Doc*(* _temp1825)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1828= Cyc_PP_text( _tag_arr("for(", sizeof(
unsigned char), 5u)); struct Cyc_PP_Doc* _temp1829= Cyc_Absynpp_exp2doc(
_temp1688); struct Cyc_PP_Doc* _temp1830= Cyc_PP_text( _tag_arr("; ", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp1831= Cyc_Absynpp_exp2doc(
_temp1686); struct Cyc_PP_Doc* _temp1832= Cyc_PP_text( _tag_arr("; ", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp1833= Cyc_Absynpp_exp2doc(
_temp1680); struct Cyc_PP_Doc* _temp1834= Cyc_PP_text( _tag_arr(") {", sizeof(
unsigned char), 4u)); struct Cyc_PP_Doc* _temp1835= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(*
_temp1838)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1841= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1842= Cyc_Absynpp_stmt2doc( _temp1674); struct Cyc_PP_Doc*
_temp1839[ 2u]={ _temp1841, _temp1842}; struct _tagged_arr _temp1840={( void*)
_temp1839,( void*) _temp1839,( void*)( _temp1839 + 2u)}; _temp1838( _temp1840);}));
struct Cyc_PP_Doc* _temp1836= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1837=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1826[ 10u]={ _temp1828, _temp1829, _temp1830, _temp1831, _temp1832,
_temp1833, _temp1834, _temp1835, _temp1836, _temp1837}; struct _tagged_arr
_temp1827={( void*) _temp1826,( void*) _temp1826,( void*)( _temp1826 + 10u)};
_temp1825( _temp1827);}); goto _LL1601; _LL1623: s=({ struct Cyc_PP_Doc*(*
_temp1843)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1846= Cyc_PP_text(
_tag_arr("switch (", sizeof( unsigned char), 9u)); struct Cyc_PP_Doc* _temp1847=
Cyc_Absynpp_exp2doc( _temp1692); struct Cyc_PP_Doc* _temp1848= Cyc_PP_text(
_tag_arr(") {", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc* _temp1849= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1850= Cyc_Absynpp_switchclauses2doc( _temp1690); struct
Cyc_PP_Doc* _temp1851= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1852= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1844[ 7u]={
_temp1846, _temp1847, _temp1848, _temp1849, _temp1850, _temp1851, _temp1852};
struct _tagged_arr _temp1845={( void*) _temp1844,( void*) _temp1844,( void*)(
_temp1844 + 7u)}; _temp1843( _temp1845);}); goto _LL1601; _LL1625: s=({ struct
Cyc_PP_Doc*(* _temp1853)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1856= Cyc_PP_text( _tag_arr("switch \"C\" (", sizeof( unsigned char), 13u));
struct Cyc_PP_Doc* _temp1857= Cyc_Absynpp_exp2doc( _temp1696); struct Cyc_PP_Doc*
_temp1858= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc*
_temp1859= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1860= Cyc_Absynpp_switchCclauses2doc(
_temp1694); struct Cyc_PP_Doc* _temp1861= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp1862= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1854[ 7u]={ _temp1856, _temp1857, _temp1858, _temp1859, _temp1860,
_temp1861, _temp1862}; struct _tagged_arr _temp1855={( void*) _temp1854,( void*)
_temp1854,( void*)( _temp1854 + 7u)}; _temp1853( _temp1855);}); goto _LL1601;
_LL1627: s= Cyc_PP_text( _tag_arr("fallthru;", sizeof( unsigned char), 10u));
goto _LL1601; _LL1629: s=({ struct Cyc_PP_Doc*(* _temp1863)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1866= Cyc_PP_text( _tag_arr("fallthru(",
sizeof( unsigned char), 10u)); struct Cyc_PP_Doc* _temp1867= Cyc_Absynpp_exps2doc_prec(
20, _temp1704); struct Cyc_PP_Doc* _temp1868= Cyc_PP_text( _tag_arr(");",
sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1864[ 3u]={ _temp1866,
_temp1867, _temp1868}; struct _tagged_arr _temp1865={( void*) _temp1864,( void*)
_temp1864,( void*)( _temp1864 + 3u)}; _temp1863( _temp1865);}); goto _LL1601;
_LL1631: s=({ struct Cyc_PP_Doc*(* _temp1869)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1872= Cyc_Absynpp_decl2doc( _temp1708); struct Cyc_PP_Doc*
_temp1873= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1874= Cyc_Absynpp_stmt2doc(
_temp1706); struct Cyc_PP_Doc* _temp1870[ 3u]={ _temp1872, _temp1873, _temp1874};
struct _tagged_arr _temp1871={( void*) _temp1870,( void*) _temp1870,( void*)(
_temp1870 + 3u)}; _temp1869( _temp1871);}); goto _LL1601; _LL1633: s=({ struct
Cyc_PP_Doc*(* _temp1875)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1878= Cyc_PP_text( _tag_arr("cut ", sizeof( unsigned char), 5u)); struct
Cyc_PP_Doc* _temp1879= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1710)); struct
Cyc_PP_Doc* _temp1876[ 2u]={ _temp1878, _temp1879}; struct _tagged_arr _temp1877={(
void*) _temp1876,( void*) _temp1876,( void*)( _temp1876 + 2u)}; _temp1875(
_temp1877);}); goto _LL1601; _LL1635: s=({ struct Cyc_PP_Doc*(* _temp1880)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1883= Cyc_PP_text(
_tag_arr("splice ", sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp1884=
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1712)); struct Cyc_PP_Doc* _temp1881[
2u]={ _temp1883, _temp1884}; struct _tagged_arr _temp1882={( void*) _temp1881,(
void*) _temp1881,( void*)( _temp1881 + 2u)}; _temp1880( _temp1882);}); goto
_LL1601; _LL1637: if( Cyc_Absynpp_decls_first? Cyc_Absynpp_is_declaration(
_temp1714): 0){ s=({ struct Cyc_PP_Doc*(* _temp1885)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1888= Cyc_PP_textptr( _temp1716); struct Cyc_PP_Doc*
_temp1889= Cyc_PP_text( _tag_arr(": {", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc*
_temp1890= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1891= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1714)); struct Cyc_PP_Doc* _temp1892= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp1893= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1886[ 6u]={ _temp1888, _temp1889, _temp1890, _temp1891, _temp1892,
_temp1893}; struct _tagged_arr _temp1887={( void*) _temp1886,( void*) _temp1886,(
void*)( _temp1886 + 6u)}; _temp1885( _temp1887);});} else{ s=({ struct Cyc_PP_Doc*(*
_temp1894)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1897= Cyc_PP_textptr(
_temp1716); struct Cyc_PP_Doc* _temp1898= Cyc_PP_text( _tag_arr(": ", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp1899= Cyc_Absynpp_stmt2doc(
_temp1714); struct Cyc_PP_Doc* _temp1895[ 3u]={ _temp1897, _temp1898, _temp1899};
struct _tagged_arr _temp1896={( void*) _temp1895,( void*) _temp1895,( void*)(
_temp1895 + 3u)}; _temp1894( _temp1896);});} goto _LL1601; _LL1639: s=({ struct
Cyc_PP_Doc*(* _temp1900)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1903= Cyc_PP_text( _tag_arr("do {", sizeof( unsigned char), 5u)); struct
Cyc_PP_Doc* _temp1904= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1905= Cyc_PP_nest(
2, Cyc_Absynpp_stmt2doc( _temp1724)); struct Cyc_PP_Doc* _temp1906= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1907= Cyc_PP_text( _tag_arr("} while (", sizeof(
unsigned char), 10u)); struct Cyc_PP_Doc* _temp1908= Cyc_Absynpp_exp2doc(
_temp1722); struct Cyc_PP_Doc* _temp1909= Cyc_PP_text( _tag_arr(");", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp1901[ 7u]={ _temp1903, _temp1904,
_temp1905, _temp1906, _temp1907, _temp1908, _temp1909}; struct _tagged_arr
_temp1902={( void*) _temp1901,( void*) _temp1901,( void*)( _temp1901 + 7u)};
_temp1900( _temp1902);}); goto _LL1601; _LL1641: s=({ struct Cyc_PP_Doc*(*
_temp1910)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1913= Cyc_PP_text(
_tag_arr("try {", sizeof( unsigned char), 6u)); struct Cyc_PP_Doc* _temp1914=
Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1915= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1728)); struct Cyc_PP_Doc* _temp1916= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp1917= Cyc_PP_text( _tag_arr("} catch {", sizeof( unsigned char), 10u));
struct Cyc_PP_Doc* _temp1918= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1919=
Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc( _temp1726)); struct Cyc_PP_Doc*
_temp1920= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1921= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1911[ 9u]={
_temp1913, _temp1914, _temp1915, _temp1916, _temp1917, _temp1918, _temp1919,
_temp1920, _temp1921}; struct _tagged_arr _temp1912={( void*) _temp1911,( void*)
_temp1911,( void*)( _temp1911 + 9u)}; _temp1910( _temp1912);}); goto _LL1601;
_LL1643: s=({ struct Cyc_PP_Doc*(* _temp1922)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1925= Cyc_PP_text( _tag_arr("region<", sizeof(
unsigned char), 8u)); struct Cyc_PP_Doc* _temp1926= Cyc_PP_textptr( Cyc_Absynpp_get_name(
_temp1734)); struct Cyc_PP_Doc* _temp1927= Cyc_PP_text( _tag_arr(">", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1928= Cyc_Absynpp_qvar2doc(
_temp1732->name); struct Cyc_PP_Doc* _temp1929= Cyc_PP_text( _tag_arr("{",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1930= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1931= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1730));
struct Cyc_PP_Doc* _temp1932= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1933=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1923[ 9u]={ _temp1925, _temp1926, _temp1927, _temp1928, _temp1929,
_temp1930, _temp1931, _temp1932, _temp1933}; struct _tagged_arr _temp1924={(
void*) _temp1923,( void*) _temp1923,( void*)( _temp1923 + 9u)}; _temp1922(
_temp1924);}); goto _LL1601; _LL1601:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void* _temp1934=( void*) p->r;
int _temp1966; void* _temp1968; unsigned char _temp1970; struct _tagged_arr
_temp1972; struct Cyc_Absyn_Vardecl* _temp1974; struct Cyc_List_List* _temp1976;
struct Cyc_Absyn_Pat* _temp1978; struct Cyc_Absyn_Vardecl* _temp1980; struct
_tuple0* _temp1982; struct Cyc_List_List* _temp1984; struct Cyc_List_List*
_temp1986; struct _tuple0* _temp1988; struct Cyc_List_List* _temp1990; struct
Cyc_List_List* _temp1992; struct _tuple0* _temp1994; struct Cyc_List_List*
_temp1996; struct Cyc_List_List* _temp1998; struct Cyc_Core_Opt* _temp2000;
struct Cyc_Absyn_Structdecl* _temp2002; struct Cyc_Absyn_Enumfield* _temp2004;
struct Cyc_Absyn_Enumdecl* _temp2006; struct Cyc_List_List* _temp2008; struct
Cyc_List_List* _temp2010; struct Cyc_Absyn_Tunionfield* _temp2012; struct Cyc_Absyn_Tuniondecl*
_temp2014; _LL1936: if( _temp1934 ==( void*) Cyc_Absyn_Wild_p){ goto _LL1937;}
else{ goto _LL1938;} _LL1938: if( _temp1934 ==( void*) Cyc_Absyn_Null_p){ goto
_LL1939;} else{ goto _LL1940;} _LL1940: if(( unsigned int) _temp1934 > 2u?*((
int*) _temp1934) == Cyc_Absyn_Int_p: 0){ _LL1969: _temp1968=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1934)->f1; goto _LL1967; _LL1967: _temp1966=((
struct Cyc_Absyn_Int_p_struct*) _temp1934)->f2; goto _LL1941;} else{ goto
_LL1942;} _LL1942: if(( unsigned int) _temp1934 > 2u?*(( int*) _temp1934) == Cyc_Absyn_Char_p:
0){ _LL1971: _temp1970=(( struct Cyc_Absyn_Char_p_struct*) _temp1934)->f1; goto
_LL1943;} else{ goto _LL1944;} _LL1944: if(( unsigned int) _temp1934 > 2u?*((
int*) _temp1934) == Cyc_Absyn_Float_p: 0){ _LL1973: _temp1972=(( struct Cyc_Absyn_Float_p_struct*)
_temp1934)->f1; goto _LL1945;} else{ goto _LL1946;} _LL1946: if(( unsigned int)
_temp1934 > 2u?*(( int*) _temp1934) == Cyc_Absyn_Var_p: 0){ _LL1975: _temp1974=((
struct Cyc_Absyn_Var_p_struct*) _temp1934)->f1; goto _LL1947;} else{ goto
_LL1948;} _LL1948: if(( unsigned int) _temp1934 > 2u?*(( int*) _temp1934) == Cyc_Absyn_Tuple_p:
0){ _LL1977: _temp1976=(( struct Cyc_Absyn_Tuple_p_struct*) _temp1934)->f1; goto
_LL1949;} else{ goto _LL1950;} _LL1950: if(( unsigned int) _temp1934 > 2u?*((
int*) _temp1934) == Cyc_Absyn_Pointer_p: 0){ _LL1979: _temp1978=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1934)->f1; goto _LL1951;} else{ goto _LL1952;} _LL1952: if(( unsigned int)
_temp1934 > 2u?*(( int*) _temp1934) == Cyc_Absyn_Reference_p: 0){ _LL1981:
_temp1980=(( struct Cyc_Absyn_Reference_p_struct*) _temp1934)->f1; goto _LL1953;}
else{ goto _LL1954;} _LL1954: if(( unsigned int) _temp1934 > 2u?*(( int*)
_temp1934) == Cyc_Absyn_UnknownId_p: 0){ _LL1983: _temp1982=(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp1934)->f1; goto _LL1955;} else{ goto _LL1956;} _LL1956: if(( unsigned int)
_temp1934 > 2u?*(( int*) _temp1934) == Cyc_Absyn_UnknownCall_p: 0){ _LL1989:
_temp1988=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1934)->f1; goto
_LL1987; _LL1987: _temp1986=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1934)->f2;
goto _LL1985; _LL1985: _temp1984=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1934)->f3; goto _LL1957;} else{ goto _LL1958;} _LL1958: if(( unsigned int)
_temp1934 > 2u?*(( int*) _temp1934) == Cyc_Absyn_UnknownFields_p: 0){ _LL1995:
_temp1994=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp1934)->f1; goto
_LL1993; _LL1993: _temp1992=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1934)->f2; goto _LL1991; _LL1991: _temp1990=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1934)->f3; goto _LL1959;} else{ goto _LL1960;} _LL1960: if(( unsigned int)
_temp1934 > 2u?*(( int*) _temp1934) == Cyc_Absyn_Struct_p: 0){ _LL2003:
_temp2002=(( struct Cyc_Absyn_Struct_p_struct*) _temp1934)->f1; goto _LL2001;
_LL2001: _temp2000=(( struct Cyc_Absyn_Struct_p_struct*) _temp1934)->f2; goto
_LL1999; _LL1999: _temp1998=(( struct Cyc_Absyn_Struct_p_struct*) _temp1934)->f3;
goto _LL1997; _LL1997: _temp1996=(( struct Cyc_Absyn_Struct_p_struct*) _temp1934)->f4;
goto _LL1961;} else{ goto _LL1962;} _LL1962: if(( unsigned int) _temp1934 > 2u?*((
int*) _temp1934) == Cyc_Absyn_Enum_p: 0){ _LL2007: _temp2006=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1934)->f1; goto _LL2005; _LL2005: _temp2004=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1934)->f2; goto _LL1963;} else{ goto _LL1964;} _LL1964: if(( unsigned int)
_temp1934 > 2u?*(( int*) _temp1934) == Cyc_Absyn_Tunion_p: 0){ _LL2015:
_temp2014=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1934)->f1; goto _LL2013;
_LL2013: _temp2012=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1934)->f2; goto
_LL2011; _LL2011: _temp2010=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1934)->f3;
goto _LL2009; _LL2009: _temp2008=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1934)->f4;
goto _LL1965;} else{ goto _LL1935;} _LL1937: s= Cyc_PP_text( _tag_arr("_",
sizeof( unsigned char), 2u)); goto _LL1935; _LL1939: s= Cyc_PP_text( _tag_arr("null",
sizeof( unsigned char), 5u)); goto _LL1935; _LL1941: if( _temp1968 ==( void*)
Cyc_Absyn_Signed){ s= Cyc_PP_text(( struct _tagged_arr) xprintf("%d", _temp1966));}
else{ s= Cyc_PP_text(( struct _tagged_arr) xprintf("%u",( unsigned int)
_temp1966));} goto _LL1935; _LL1943: s= Cyc_PP_text(( struct _tagged_arr)({
struct _tagged_arr _temp2016= Cyc_Absynpp_char_escape( _temp1970); xprintf("'%.*s'",
_get_arr_size( _temp2016, 1u), _temp2016.curr);})); goto _LL1935; _LL1945: s=
Cyc_PP_text( _temp1972); goto _LL1935; _LL1947: s= Cyc_Absynpp_qvar2doc(
_temp1974->name); goto _LL1935; _LL1949: s=({ struct Cyc_PP_Doc*(* _temp2017)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2020= Cyc_PP_text(
_tag_arr("$(", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp2021=((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Pat*), struct
_tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc,
_tag_arr(",", sizeof( unsigned char), 2u), _temp1976); struct Cyc_PP_Doc*
_temp2022= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp2018[ 3u]={ _temp2020, _temp2021, _temp2022}; struct _tagged_arr _temp2019={(
void*) _temp2018,( void*) _temp2018,( void*)( _temp2018 + 3u)}; _temp2017(
_temp2019);}); goto _LL1935; _LL1951: s=({ struct Cyc_PP_Doc*(* _temp2023)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2026= Cyc_PP_text(
_tag_arr("&", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp2027= Cyc_Absynpp_pat2doc(
_temp1978); struct Cyc_PP_Doc* _temp2024[ 2u]={ _temp2026, _temp2027}; struct
_tagged_arr _temp2025={( void*) _temp2024,( void*) _temp2024,( void*)( _temp2024
+ 2u)}; _temp2023( _temp2025);}); goto _LL1935; _LL1953: s=({ struct Cyc_PP_Doc*(*
_temp2028)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2031= Cyc_PP_text(
_tag_arr("*", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp2032= Cyc_Absynpp_qvar2doc(
_temp1980->name); struct Cyc_PP_Doc* _temp2029[ 2u]={ _temp2031, _temp2032};
struct _tagged_arr _temp2030={( void*) _temp2029,( void*) _temp2029,( void*)(
_temp2029 + 2u)}; _temp2028( _temp2030);}); goto _LL1935; _LL1955: s= Cyc_Absynpp_qvar2doc(
_temp1982); goto _LL1935; _LL1957: s=({ struct Cyc_PP_Doc*(* _temp2033)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2036= Cyc_Absynpp_qvar2doc(
_temp1988); struct Cyc_PP_Doc* _temp2037= Cyc_Absynpp_tvars2doc( _temp1986);
struct Cyc_PP_Doc* _temp2038= Cyc_PP_group( _tag_arr("(", sizeof( unsigned char),
2u), _tag_arr(")", sizeof( unsigned char), 2u), _tag_arr(",", sizeof(
unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc,
_temp1984)); struct Cyc_PP_Doc* _temp2034[ 3u]={ _temp2036, _temp2037, _temp2038};
struct _tagged_arr _temp2035={( void*) _temp2034,( void*) _temp2034,( void*)(
_temp2034 + 3u)}; _temp2033( _temp2035);}); goto _LL1935; _LL1959: s=({ struct
Cyc_PP_Doc*(* _temp2039)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2042= Cyc_Absynpp_qvar2doc( _temp1994); struct Cyc_PP_Doc* _temp2043= Cyc_Absynpp_tvars2doc(
_temp1992); struct Cyc_PP_Doc* _temp2044= Cyc_PP_group( _tag_arr("{", sizeof(
unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1990)); struct Cyc_PP_Doc* _temp2040[ 3u]={ _temp2042, _temp2043, _temp2044};
struct _tagged_arr _temp2041={( void*) _temp2040,( void*) _temp2040,( void*)(
_temp2040 + 3u)}; _temp2039( _temp2041);}); goto _LL1935; _LL1961: s=({ struct
Cyc_PP_Doc*(* _temp2045)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2048= _temp2002->name == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp2002->name))->v); struct Cyc_PP_Doc*
_temp2049= Cyc_Absynpp_tvars2doc( _temp1998); struct Cyc_PP_Doc* _temp2050= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_dp2doc, _temp1996)); struct Cyc_PP_Doc* _temp2046[ 3u]={ _temp2048,
_temp2049, _temp2050}; struct _tagged_arr _temp2047={( void*) _temp2046,( void*)
_temp2046,( void*)( _temp2046 + 3u)}; _temp2045( _temp2047);}); goto _LL1935;
_LL1963: s= Cyc_Absynpp_qvar2doc( _temp2004->name); goto _LL1935; _LL1965: if(
_temp2008 == 0){ s= Cyc_Absynpp_qvar2doc( _temp2012->name);} else{ s=({ struct
Cyc_PP_Doc*(* _temp2051)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2054= Cyc_Absynpp_qvar2doc( _temp2012->name); struct Cyc_PP_Doc* _temp2055=
Cyc_Absynpp_tvars2doc( _temp2010); struct Cyc_PP_Doc* _temp2056= Cyc_PP_egroup(
_tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_pat2doc, _temp2008)); struct Cyc_PP_Doc* _temp2052[ 3u]={ _temp2054,
_temp2055, _temp2056}; struct _tagged_arr _temp2053={( void*) _temp2052,( void*)
_temp2052,( void*)( _temp2052 + 3u)}; _temp2051( _temp2053);});} goto _LL1935;
_LL1935:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct _tuple5* dp){
return({ struct Cyc_PP_Doc*(* _temp2057)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2060= Cyc_PP_egroup( _tag_arr("", sizeof( unsigned char),
1u), _tag_arr("=", sizeof( unsigned char), 2u), _tag_arr("=", sizeof(
unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(* dp).f1));
struct Cyc_PP_Doc* _temp2061= Cyc_Absynpp_pat2doc((* dp).f2); struct Cyc_PP_Doc*
_temp2058[ 2u]={ _temp2060, _temp2061}; struct _tagged_arr _temp2059={( void*)
_temp2058,( void*) _temp2058,( void*)( _temp2058 + 2u)}; _temp2057( _temp2059);});}
struct Cyc_PP_Doc* Cyc_Absynpp_switchclause2doc( struct Cyc_Absyn_Switch_clause*
c){ if( c->where_clause == 0?( void*)( c->pattern)->r ==( void*) Cyc_Absyn_Wild_p:
0){ return({ struct Cyc_PP_Doc*(* _temp2062)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2065= Cyc_PP_text( _tag_arr("default: ", sizeof(
unsigned char), 10u)); struct Cyc_PP_Doc* _temp2066= Cyc_PP_nest( 2,({ struct
Cyc_PP_Doc*(* _temp2067)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2070= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2071= Cyc_Absynpp_stmt2doc(
c->body); struct Cyc_PP_Doc* _temp2068[ 2u]={ _temp2070, _temp2071}; struct
_tagged_arr _temp2069={( void*) _temp2068,( void*) _temp2068,( void*)( _temp2068
+ 2u)}; _temp2067( _temp2069);})); struct Cyc_PP_Doc* _temp2063[ 2u]={ _temp2065,
_temp2066}; struct _tagged_arr _temp2064={( void*) _temp2063,( void*) _temp2063,(
void*)( _temp2063 + 2u)}; _temp2062( _temp2064);});} else{ if( c->where_clause
== 0){ return({ struct Cyc_PP_Doc*(* _temp2072)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2075= Cyc_PP_text( _tag_arr("case ", sizeof(
unsigned char), 6u)); struct Cyc_PP_Doc* _temp2076= Cyc_Absynpp_pat2doc( c->pattern);
struct Cyc_PP_Doc* _temp2077= Cyc_PP_text( _tag_arr(": ", sizeof( unsigned char),
3u)); struct Cyc_PP_Doc* _temp2078= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(*
_temp2079)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2082= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2083= Cyc_Absynpp_stmt2doc( c->body); struct Cyc_PP_Doc*
_temp2080[ 2u]={ _temp2082, _temp2083}; struct _tagged_arr _temp2081={( void*)
_temp2080,( void*) _temp2080,( void*)( _temp2080 + 2u)}; _temp2079( _temp2081);}));
struct Cyc_PP_Doc* _temp2073[ 4u]={ _temp2075, _temp2076, _temp2077, _temp2078};
struct _tagged_arr _temp2074={( void*) _temp2073,( void*) _temp2073,( void*)(
_temp2073 + 4u)}; _temp2072( _temp2074);});} else{ return({ struct Cyc_PP_Doc*(*
_temp2084)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2087= Cyc_PP_text(
_tag_arr("case ", sizeof( unsigned char), 6u)); struct Cyc_PP_Doc* _temp2088=
Cyc_Absynpp_pat2doc( c->pattern); struct Cyc_PP_Doc* _temp2089= Cyc_PP_text(
_tag_arr(" && ", sizeof( unsigned char), 5u)); struct Cyc_PP_Doc* _temp2090= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( c->where_clause)); struct Cyc_PP_Doc*
_temp2091= Cyc_PP_text( _tag_arr(": ", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc*
_temp2092= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2093)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp2096= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2097= Cyc_Absynpp_stmt2doc( c->body); struct Cyc_PP_Doc* _temp2094[ 2u]={
_temp2096, _temp2097}; struct _tagged_arr _temp2095={( void*) _temp2094,( void*)
_temp2094,( void*)( _temp2094 + 2u)}; _temp2093( _temp2095);})); struct Cyc_PP_Doc*
_temp2085[ 6u]={ _temp2087, _temp2088, _temp2089, _temp2090, _temp2091,
_temp2092}; struct _tagged_arr _temp2086={( void*) _temp2085,( void*) _temp2085,(
void*)( _temp2085 + 6u)}; _temp2084( _temp2086);});}}} struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Switch_clause*), struct _tagged_arr sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc, _tag_arr("", sizeof(
unsigned char), 1u), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_switchCclause2doc(
struct Cyc_Absyn_SwitchC_clause* c){ struct Cyc_Absyn_SwitchC_clause _temp2100;
struct Cyc_Position_Segment* _temp2101; struct Cyc_Absyn_Stmt* _temp2103; struct
Cyc_Absyn_Exp* _temp2105; struct Cyc_Absyn_SwitchC_clause* _temp2098= c;
_temp2100=* _temp2098; _LL2106: _temp2105= _temp2100.cnst_exp; goto _LL2104;
_LL2104: _temp2103= _temp2100.body; goto _LL2102; _LL2102: _temp2101= _temp2100.loc;
goto _LL2099; _LL2099: if( _temp2105 == 0){ return({ struct Cyc_PP_Doc*(*
_temp2107)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2110= Cyc_PP_text(
_tag_arr("default: ", sizeof( unsigned char), 10u)); struct Cyc_PP_Doc*
_temp2111= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2112)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp2115= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2116= Cyc_Absynpp_stmt2doc( c->body); struct Cyc_PP_Doc* _temp2113[ 2u]={
_temp2115, _temp2116}; struct _tagged_arr _temp2114={( void*) _temp2113,( void*)
_temp2113,( void*)( _temp2113 + 2u)}; _temp2112( _temp2114);})); struct Cyc_PP_Doc*
_temp2108[ 2u]={ _temp2110, _temp2111}; struct _tagged_arr _temp2109={( void*)
_temp2108,( void*) _temp2108,( void*)( _temp2108 + 2u)}; _temp2107( _temp2109);});}
else{ return({ struct Cyc_PP_Doc*(* _temp2117)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2120= Cyc_PP_text( _tag_arr("case ", sizeof(
unsigned char), 6u)); struct Cyc_PP_Doc* _temp2121= Cyc_Absynpp_exp2doc(( struct
Cyc_Absyn_Exp*) _check_null( _temp2105)); struct Cyc_PP_Doc* _temp2122= Cyc_PP_text(
_tag_arr(": ", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp2123= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc*(* _temp2124)( struct _tagged_arr)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp2127= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2128= Cyc_Absynpp_stmt2doc(
_temp2103); struct Cyc_PP_Doc* _temp2125[ 2u]={ _temp2127, _temp2128}; struct
_tagged_arr _temp2126={( void*) _temp2125,( void*) _temp2125,( void*)( _temp2125
+ 2u)}; _temp2124( _temp2126);})); struct Cyc_PP_Doc* _temp2118[ 4u]={ _temp2120,
_temp2121, _temp2122, _temp2123}; struct _tagged_arr _temp2119={( void*)
_temp2118,( void*) _temp2118,( void*)( _temp2118 + 4u)}; _temp2117( _temp2119);});}}
struct Cyc_PP_Doc* Cyc_Absynpp_switchCclauses2doc( struct Cyc_List_List* cs){
return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_SwitchC_clause*),
struct _tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchCclause2doc,
_tag_arr("", sizeof( unsigned char), 1u), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_enumfield2doc(
struct Cyc_Absyn_Enumfield* f){ if( f->tag == 0){ return Cyc_Absynpp_qvar2doc( f->name);}
else{ return({ struct Cyc_PP_Doc*(* _temp2129)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2132= Cyc_Absynpp_qvar2doc( f->name); struct Cyc_PP_Doc*
_temp2133= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc*
_temp2134= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( f->tag));
struct Cyc_PP_Doc* _temp2130[ 3u]={ _temp2132, _temp2133, _temp2134}; struct
_tagged_arr _temp2131={( void*) _temp2130,( void*) _temp2130,( void*)( _temp2130
+ 3u)}; _temp2129( _temp2131);});}} struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc(
struct Cyc_List_List* fs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Enumfield*), struct _tagged_arr sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc, _tag_arr(",", sizeof(
unsigned char), 2u), fs);} static struct Cyc_PP_Doc* Cyc_Absynpp_id2doc( struct
Cyc_Absyn_Vardecl* v){ return Cyc_Absynpp_qvar2doc( v->name);} static struct Cyc_PP_Doc*
Cyc_Absynpp_ids2doc( struct Cyc_List_List* vds){ return(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Vardecl*), struct _tagged_arr sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseq)( Cyc_Absynpp_id2doc, _tag_arr(",",
sizeof( unsigned char), 2u), vds);} struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl* d){ struct Cyc_PP_Doc* s;{ void* _temp2135=( void*) d->r;
struct Cyc_Absyn_Fndecl* _temp2161; struct Cyc_Absyn_Structdecl* _temp2163;
struct Cyc_Absyn_Uniondecl* _temp2165; struct Cyc_Absyn_Vardecl* _temp2167;
struct Cyc_Absyn_Vardecl _temp2169; struct Cyc_List_List* _temp2170; struct Cyc_Core_Opt*
_temp2172; struct Cyc_Absyn_Exp* _temp2174; void* _temp2176; struct Cyc_Absyn_Tqual
_temp2178; struct _tuple0* _temp2180; void* _temp2182; struct Cyc_Absyn_Tuniondecl*
_temp2184; struct Cyc_Absyn_Tuniondecl _temp2186; int _temp2187; struct Cyc_Core_Opt*
_temp2189; struct Cyc_List_List* _temp2191; struct _tuple0* _temp2193; void*
_temp2195; int _temp2197; struct Cyc_Absyn_Exp* _temp2199; struct Cyc_Core_Opt*
_temp2201; struct Cyc_Core_Opt* _temp2203; struct Cyc_Absyn_Pat* _temp2205;
struct Cyc_List_List* _temp2207; struct Cyc_Absyn_Enumdecl* _temp2209; struct
Cyc_Absyn_Enumdecl _temp2211; struct Cyc_Core_Opt* _temp2212; struct _tuple0*
_temp2214; void* _temp2216; struct Cyc_Absyn_Typedefdecl* _temp2218; struct Cyc_List_List*
_temp2220; struct _tagged_arr* _temp2222; struct Cyc_List_List* _temp2224;
struct _tuple0* _temp2226; struct Cyc_List_List* _temp2228; _LL2137: if(*(( int*)
_temp2135) == Cyc_Absyn_Fn_d){ _LL2162: _temp2161=(( struct Cyc_Absyn_Fn_d_struct*)
_temp2135)->f1; goto _LL2138;} else{ goto _LL2139;} _LL2139: if(*(( int*)
_temp2135) == Cyc_Absyn_Struct_d){ _LL2164: _temp2163=(( struct Cyc_Absyn_Struct_d_struct*)
_temp2135)->f1; goto _LL2140;} else{ goto _LL2141;} _LL2141: if(*(( int*)
_temp2135) == Cyc_Absyn_Union_d){ _LL2166: _temp2165=(( struct Cyc_Absyn_Union_d_struct*)
_temp2135)->f1; goto _LL2142;} else{ goto _LL2143;} _LL2143: if(*(( int*)
_temp2135) == Cyc_Absyn_Var_d){ _LL2168: _temp2167=(( struct Cyc_Absyn_Var_d_struct*)
_temp2135)->f1; _temp2169=* _temp2167; _LL2183: _temp2182=( void*) _temp2169.sc;
goto _LL2181; _LL2181: _temp2180= _temp2169.name; goto _LL2179; _LL2179:
_temp2178= _temp2169.tq; goto _LL2177; _LL2177: _temp2176=( void*) _temp2169.type;
goto _LL2175; _LL2175: _temp2174= _temp2169.initializer; goto _LL2173; _LL2173:
_temp2172= _temp2169.rgn; goto _LL2171; _LL2171: _temp2170= _temp2169.attributes;
goto _LL2144;} else{ goto _LL2145;} _LL2145: if(*(( int*) _temp2135) == Cyc_Absyn_Tunion_d){
_LL2185: _temp2184=(( struct Cyc_Absyn_Tunion_d_struct*) _temp2135)->f1;
_temp2186=* _temp2184; _LL2196: _temp2195=( void*) _temp2186.sc; goto _LL2194;
_LL2194: _temp2193= _temp2186.name; goto _LL2192; _LL2192: _temp2191= _temp2186.tvs;
goto _LL2190; _LL2190: _temp2189= _temp2186.fields; goto _LL2188; _LL2188:
_temp2187= _temp2186.is_xtunion; goto _LL2146;} else{ goto _LL2147;} _LL2147:
if(*(( int*) _temp2135) == Cyc_Absyn_Let_d){ _LL2206: _temp2205=(( struct Cyc_Absyn_Let_d_struct*)
_temp2135)->f1; goto _LL2204; _LL2204: _temp2203=(( struct Cyc_Absyn_Let_d_struct*)
_temp2135)->f2; goto _LL2202; _LL2202: _temp2201=(( struct Cyc_Absyn_Let_d_struct*)
_temp2135)->f3; goto _LL2200; _LL2200: _temp2199=(( struct Cyc_Absyn_Let_d_struct*)
_temp2135)->f4; goto _LL2198; _LL2198: _temp2197=(( struct Cyc_Absyn_Let_d_struct*)
_temp2135)->f5; goto _LL2148;} else{ goto _LL2149;} _LL2149: if(*(( int*)
_temp2135) == Cyc_Absyn_Letv_d){ _LL2208: _temp2207=(( struct Cyc_Absyn_Letv_d_struct*)
_temp2135)->f1; goto _LL2150;} else{ goto _LL2151;} _LL2151: if(*(( int*)
_temp2135) == Cyc_Absyn_Enum_d){ _LL2210: _temp2209=(( struct Cyc_Absyn_Enum_d_struct*)
_temp2135)->f1; _temp2211=* _temp2209; _LL2217: _temp2216=( void*) _temp2211.sc;
goto _LL2215; _LL2215: _temp2214= _temp2211.name; goto _LL2213; _LL2213:
_temp2212= _temp2211.fields; goto _LL2152;} else{ goto _LL2153;} _LL2153: if(*((
int*) _temp2135) == Cyc_Absyn_Typedef_d){ _LL2219: _temp2218=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2135)->f1; goto _LL2154;} else{ goto _LL2155;} _LL2155: if(*(( int*)
_temp2135) == Cyc_Absyn_Namespace_d){ _LL2223: _temp2222=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2135)->f1; goto _LL2221; _LL2221: _temp2220=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2135)->f2; goto _LL2156;} else{ goto _LL2157;} _LL2157: if(*(( int*)
_temp2135) == Cyc_Absyn_Using_d){ _LL2227: _temp2226=(( struct Cyc_Absyn_Using_d_struct*)
_temp2135)->f1; goto _LL2225; _LL2225: _temp2224=(( struct Cyc_Absyn_Using_d_struct*)
_temp2135)->f2; goto _LL2158;} else{ goto _LL2159;} _LL2159: if(*(( int*)
_temp2135) == Cyc_Absyn_ExternC_d){ _LL2229: _temp2228=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2135)->f1; goto _LL2160;} else{ goto _LL2136;} _LL2138: { void* t=( void*)({
struct Cyc_Absyn_FnType_struct* _temp2261=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2261[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2262; _temp2262.tag= Cyc_Absyn_FnType; _temp2262.f1=({ struct Cyc_Absyn_FnInfo
_temp2263; _temp2263.tvars= _temp2161->tvs; _temp2263.effect= _temp2161->effect;
_temp2263.ret_typ=( void*)(( void*) _temp2161->ret_type); _temp2263.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp2161->args); _temp2263.c_varargs=
_temp2161->c_varargs; _temp2263.cyc_varargs= _temp2161->cyc_varargs; _temp2263.rgn_po=
_temp2161->rgn_po; _temp2263.attributes= 0; _temp2263;}); _temp2262;});
_temp2261;}); struct Cyc_PP_Doc* attsdoc= Cyc_Absynpp_atts2doc( _temp2161->attributes);
struct Cyc_PP_Doc* inlinedoc; if( _temp2161->is_inline){ if( Cyc_Absynpp_to_VC){
inlinedoc= Cyc_PP_text( _tag_arr("__inline ", sizeof( unsigned char), 10u));}
else{ inlinedoc= Cyc_PP_text( _tag_arr("inline ", sizeof( unsigned char), 8u));}}
else{ inlinedoc= Cyc_PP_nil_doc();}{ struct Cyc_PP_Doc* scopedoc= Cyc_Absynpp_scope2doc((
void*) _temp2161->sc); struct Cyc_PP_Doc* beforenamedoc= Cyc_Absynpp_to_VC? Cyc_Absynpp_callconv2doc(
_temp2161->attributes): attsdoc; struct Cyc_PP_Doc* namedoc= Cyc_Absynpp_typedef_name2doc(
_temp2161->name); struct Cyc_PP_Doc* tqtddoc= Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual
_temp2254; _temp2254.q_const= 0; _temp2254.q_volatile= 0; _temp2254.q_restrict=
0; _temp2254;}), t,({ struct Cyc_Core_Opt* _temp2255=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2255->v=( void*)({ struct Cyc_PP_Doc*(*
_temp2256)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2259=
beforenamedoc; struct Cyc_PP_Doc* _temp2260= namedoc; struct Cyc_PP_Doc*
_temp2257[ 2u]={ _temp2259, _temp2260}; struct _tagged_arr _temp2258={( void*)
_temp2257,( void*) _temp2257,( void*)( _temp2257 + 2u)}; _temp2256( _temp2258);});
_temp2255;})); struct Cyc_PP_Doc* bodydoc=({ struct Cyc_PP_Doc*(* _temp2242)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2245= Cyc_PP_text(
_tag_arr(" {", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp2246= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc*(* _temp2249)( struct _tagged_arr)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp2252= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2253= Cyc_Absynpp_stmt2doc(
_temp2161->body); struct Cyc_PP_Doc* _temp2250[ 2u]={ _temp2252, _temp2253};
struct _tagged_arr _temp2251={( void*) _temp2250,( void*) _temp2250,( void*)(
_temp2250 + 2u)}; _temp2249( _temp2251);})); struct Cyc_PP_Doc* _temp2247= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2248= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp2243[ 4u]={ _temp2245, _temp2246, _temp2247,
_temp2248}; struct _tagged_arr _temp2244={( void*) _temp2243,( void*) _temp2243,(
void*)( _temp2243 + 4u)}; _temp2242( _temp2244);}); s=({ struct Cyc_PP_Doc*(*
_temp2230)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2233=
inlinedoc; struct Cyc_PP_Doc* _temp2234= scopedoc; struct Cyc_PP_Doc* _temp2235=
tqtddoc; struct Cyc_PP_Doc* _temp2236= bodydoc; struct Cyc_PP_Doc* _temp2231[ 4u]={
_temp2233, _temp2234, _temp2235, _temp2236}; struct _tagged_arr _temp2232={(
void*) _temp2231,( void*) _temp2231,( void*)( _temp2231 + 4u)}; _temp2230(
_temp2232);}); if( Cyc_Absynpp_to_VC){ s=({ struct Cyc_PP_Doc*(* _temp2237)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2240= attsdoc; struct
Cyc_PP_Doc* _temp2241= s; struct Cyc_PP_Doc* _temp2238[ 2u]={ _temp2240,
_temp2241}; struct _tagged_arr _temp2239={( void*) _temp2238,( void*) _temp2238,(
void*)( _temp2238 + 2u)}; _temp2237( _temp2239);});} goto _LL2136;}} _LL2140:
if( _temp2163->fields == 0){ s=({ struct Cyc_PP_Doc*(* _temp2264)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2267= Cyc_Absynpp_scope2doc((
void*) _temp2163->sc); struct Cyc_PP_Doc* _temp2268= Cyc_PP_text( _tag_arr("struct ",
sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp2269= _temp2163->name == 0?
Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_typedef_name2doc((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp2163->name))->v);
struct Cyc_PP_Doc* _temp2270= Cyc_Absynpp_ktvars2doc( _temp2163->tvs); struct
Cyc_PP_Doc* _temp2271= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u));
struct Cyc_PP_Doc* _temp2265[ 5u]={ _temp2267, _temp2268, _temp2269, _temp2270,
_temp2271}; struct _tagged_arr _temp2266={( void*) _temp2265,( void*) _temp2265,(
void*)( _temp2265 + 5u)}; _temp2264( _temp2266);});} else{ s=({ struct Cyc_PP_Doc*(*
_temp2272)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2275= Cyc_Absynpp_scope2doc((
void*) _temp2163->sc); struct Cyc_PP_Doc* _temp2276= Cyc_PP_text( _tag_arr("struct ",
sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp2277= _temp2163->name == 0?
Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_typedef_name2doc((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp2163->name))->v);
struct Cyc_PP_Doc* _temp2278= Cyc_Absynpp_ktvars2doc( _temp2163->tvs); struct
Cyc_PP_Doc* _temp2279= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u));
struct Cyc_PP_Doc* _temp2280= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2285)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2288= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2289= Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp2163->fields))->v); struct Cyc_PP_Doc*
_temp2286[ 2u]={ _temp2288, _temp2289}; struct _tagged_arr _temp2287={( void*)
_temp2286,( void*) _temp2286,( void*)( _temp2286 + 2u)}; _temp2285( _temp2287);}));
struct Cyc_PP_Doc* _temp2281= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2282=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp2283= Cyc_Absynpp_atts2doc( _temp2163->attributes); struct Cyc_PP_Doc*
_temp2284= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp2273[ 10u]={ _temp2275, _temp2276, _temp2277, _temp2278, _temp2279,
_temp2280, _temp2281, _temp2282, _temp2283, _temp2284}; struct _tagged_arr
_temp2274={( void*) _temp2273,( void*) _temp2273,( void*)( _temp2273 + 10u)};
_temp2272( _temp2274);});} goto _LL2136; _LL2142: if( _temp2165->fields == 0){ s=({
struct Cyc_PP_Doc*(* _temp2290)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2293= Cyc_Absynpp_scope2doc(( void*) _temp2165->sc); struct Cyc_PP_Doc*
_temp2294= Cyc_PP_text( _tag_arr("union ", sizeof( unsigned char), 7u)); struct
Cyc_PP_Doc* _temp2295= _temp2165->name == 0? Cyc_PP_text( _tag_arr("", sizeof(
unsigned char), 1u)): Cyc_Absynpp_qvar2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp2165->name))->v); struct Cyc_PP_Doc* _temp2296= Cyc_Absynpp_tvars2doc(
_temp2165->tvs); struct Cyc_PP_Doc* _temp2297= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp2291[ 5u]={ _temp2293,
_temp2294, _temp2295, _temp2296, _temp2297}; struct _tagged_arr _temp2292={(
void*) _temp2291,( void*) _temp2291,( void*)( _temp2291 + 5u)}; _temp2290(
_temp2292);});} else{ s=({ struct Cyc_PP_Doc*(* _temp2298)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp2301= Cyc_Absynpp_scope2doc(( void*)
_temp2165->sc); struct Cyc_PP_Doc* _temp2302= Cyc_PP_text( _tag_arr("union ",
sizeof( unsigned char), 7u)); struct Cyc_PP_Doc* _temp2303= _temp2165->name == 0?
Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_qvar2doc((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp2165->name))->v);
struct Cyc_PP_Doc* _temp2304= Cyc_Absynpp_tvars2doc( _temp2165->tvs); struct Cyc_PP_Doc*
_temp2305= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc*
_temp2306= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2311)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp2314= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2315= Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2165->fields))->v); struct Cyc_PP_Doc* _temp2312[ 2u]={
_temp2314, _temp2315}; struct _tagged_arr _temp2313={( void*) _temp2312,( void*)
_temp2312,( void*)( _temp2312 + 2u)}; _temp2311( _temp2313);})); struct Cyc_PP_Doc*
_temp2307= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2308= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp2309= Cyc_Absynpp_atts2doc(
_temp2165->attributes); struct Cyc_PP_Doc* _temp2310= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp2299[ 10u]={ _temp2301,
_temp2302, _temp2303, _temp2304, _temp2305, _temp2306, _temp2307, _temp2308,
_temp2309, _temp2310}; struct _tagged_arr _temp2300={( void*) _temp2299,( void*)
_temp2299,( void*)( _temp2299 + 10u)}; _temp2298( _temp2300);});} goto _LL2136;
_LL2144: { struct Cyc_PP_Doc* sn= Cyc_Absynpp_typedef_name2doc( _temp2180);
struct Cyc_PP_Doc* attsdoc= Cyc_Absynpp_atts2doc( _temp2170); struct Cyc_PP_Doc*
beforenamedoc= Cyc_Absynpp_to_VC? Cyc_Absynpp_callconv2doc( _temp2170): attsdoc;
s=({ struct Cyc_PP_Doc*(* _temp2316)( struct _tagged_arr)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp2319= Cyc_Absynpp_to_VC? attsdoc: Cyc_PP_nil_doc(); struct Cyc_PP_Doc*
_temp2320= Cyc_Absynpp_scope2doc( _temp2182); struct Cyc_PP_Doc* _temp2321= Cyc_Absynpp_tqtd2doc(
_temp2178, _temp2176,({ struct Cyc_Core_Opt* _temp2329=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2329->v=( void*)({ struct Cyc_PP_Doc*(*
_temp2330)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2333=
beforenamedoc; struct Cyc_PP_Doc* _temp2334= sn; struct Cyc_PP_Doc* _temp2331[ 2u]={
_temp2333, _temp2334}; struct _tagged_arr _temp2332={( void*) _temp2331,( void*)
_temp2331,( void*)( _temp2331 + 2u)}; _temp2330( _temp2332);}); _temp2329;}));
struct Cyc_PP_Doc* _temp2322= _temp2174 == 0? Cyc_PP_text( _tag_arr("", sizeof(
unsigned char), 1u)):({ struct Cyc_PP_Doc*(* _temp2324)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp2327= Cyc_PP_text( _tag_arr(" = ", sizeof(
unsigned char), 4u)); struct Cyc_PP_Doc* _temp2328= Cyc_Absynpp_exp2doc(( struct
Cyc_Absyn_Exp*) _check_null( _temp2174)); struct Cyc_PP_Doc* _temp2325[ 2u]={
_temp2327, _temp2328}; struct _tagged_arr _temp2326={( void*) _temp2325,( void*)
_temp2325,( void*)( _temp2325 + 2u)}; _temp2324( _temp2326);}); struct Cyc_PP_Doc*
_temp2323= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp2317[ 5u]={ _temp2319, _temp2320, _temp2321, _temp2322, _temp2323}; struct
_tagged_arr _temp2318={( void*) _temp2317,( void*) _temp2317,( void*)( _temp2317
+ 5u)}; _temp2316( _temp2318);}); goto _LL2136;} _LL2146: if( _temp2189 == 0){ s=({
struct Cyc_PP_Doc*(* _temp2335)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2338= Cyc_Absynpp_scope2doc( _temp2195); struct Cyc_PP_Doc* _temp2339=
_temp2187? Cyc_PP_text( _tag_arr("xtunion ", sizeof( unsigned char), 9u)): Cyc_PP_text(
_tag_arr("tunion ", sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp2340=
_temp2187? Cyc_Absynpp_qvar2doc( _temp2193): Cyc_Absynpp_typedef_name2doc(
_temp2193); struct Cyc_PP_Doc* _temp2341= Cyc_Absynpp_ktvars2doc( _temp2191);
struct Cyc_PP_Doc* _temp2342= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp2336[ 5u]={ _temp2338, _temp2339, _temp2340,
_temp2341, _temp2342}; struct _tagged_arr _temp2337={( void*) _temp2336,( void*)
_temp2336,( void*)( _temp2336 + 5u)}; _temp2335( _temp2337);});} else{ s=({
struct Cyc_PP_Doc*(* _temp2343)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2346= Cyc_Absynpp_scope2doc( _temp2195); struct Cyc_PP_Doc* _temp2347=
_temp2187? Cyc_PP_text( _tag_arr("xtunion ", sizeof( unsigned char), 9u)): Cyc_PP_text(
_tag_arr("tunion ", sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp2348=
_temp2187? Cyc_Absynpp_qvar2doc( _temp2193): Cyc_Absynpp_typedef_name2doc(
_temp2193); struct Cyc_PP_Doc* _temp2349= Cyc_Absynpp_ktvars2doc( _temp2191);
struct Cyc_PP_Doc* _temp2350= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char),
3u)); struct Cyc_PP_Doc* _temp2351= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(*
_temp2354)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2357= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2358= Cyc_Absynpp_tunionfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp2189))->v); struct Cyc_PP_Doc* _temp2355[
2u]={ _temp2357, _temp2358}; struct _tagged_arr _temp2356={( void*) _temp2355,(
void*) _temp2355,( void*)( _temp2355 + 2u)}; _temp2354( _temp2356);})); struct
Cyc_PP_Doc* _temp2352= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2353= Cyc_PP_text(
_tag_arr("};", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp2344[ 8u]={
_temp2346, _temp2347, _temp2348, _temp2349, _temp2350, _temp2351, _temp2352,
_temp2353}; struct _tagged_arr _temp2345={( void*) _temp2344,( void*) _temp2344,(
void*)( _temp2344 + 8u)}; _temp2343( _temp2345);});} goto _LL2136; _LL2148: s=({
struct Cyc_PP_Doc*(* _temp2359)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2362= Cyc_PP_text( _tag_arr("let ", sizeof( unsigned char), 5u)); struct
Cyc_PP_Doc* _temp2363= Cyc_Absynpp_pat2doc( _temp2205); struct Cyc_PP_Doc*
_temp2364= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc*
_temp2365= Cyc_Absynpp_exp2doc( _temp2199); struct Cyc_PP_Doc* _temp2366= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp2360[ 5u]={
_temp2362, _temp2363, _temp2364, _temp2365, _temp2366}; struct _tagged_arr
_temp2361={( void*) _temp2360,( void*) _temp2360,( void*)( _temp2360 + 5u)};
_temp2359( _temp2361);}); goto _LL2136; _LL2150: s=({ struct Cyc_PP_Doc*(*
_temp2367)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2370= Cyc_PP_text(
_tag_arr("let ", sizeof( unsigned char), 5u)); struct Cyc_PP_Doc* _temp2371= Cyc_Absynpp_ids2doc(
_temp2207); struct Cyc_PP_Doc* _temp2372= Cyc_PP_text( _tag_arr(";", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp2368[ 3u]={ _temp2370, _temp2371,
_temp2372}; struct _tagged_arr _temp2369={( void*) _temp2368,( void*) _temp2368,(
void*)( _temp2368 + 3u)}; _temp2367( _temp2369);}); goto _LL2136; _LL2152: if(
_temp2212 == 0){ s=({ struct Cyc_PP_Doc*(* _temp2373)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2376= Cyc_Absynpp_scope2doc( _temp2216); struct Cyc_PP_Doc*
_temp2377= Cyc_PP_text( _tag_arr("enum ", sizeof( unsigned char), 6u)); struct
Cyc_PP_Doc* _temp2378= Cyc_Absynpp_typedef_name2doc( _temp2214); struct Cyc_PP_Doc*
_temp2379= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp2374[ 4u]={ _temp2376, _temp2377, _temp2378, _temp2379}; struct _tagged_arr
_temp2375={( void*) _temp2374,( void*) _temp2374,( void*)( _temp2374 + 4u)};
_temp2373( _temp2375);});} else{ s=({ struct Cyc_PP_Doc*(* _temp2380)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2383= Cyc_Absynpp_scope2doc(
_temp2216); struct Cyc_PP_Doc* _temp2384= Cyc_PP_text( _tag_arr("enum ", sizeof(
unsigned char), 6u)); struct Cyc_PP_Doc* _temp2385= Cyc_Absynpp_qvar2doc(
_temp2214); struct Cyc_PP_Doc* _temp2386= Cyc_PP_text( _tag_arr(" {", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp2387= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(*
_temp2390)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2393= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2394= Cyc_Absynpp_enumfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp2212))->v); struct Cyc_PP_Doc* _temp2391[
2u]={ _temp2393, _temp2394}; struct _tagged_arr _temp2392={( void*) _temp2391,(
void*) _temp2391,( void*)( _temp2391 + 2u)}; _temp2390( _temp2392);})); struct
Cyc_PP_Doc* _temp2388= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2389= Cyc_PP_text(
_tag_arr("};", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp2381[ 7u]={
_temp2383, _temp2384, _temp2385, _temp2386, _temp2387, _temp2388, _temp2389};
struct _tagged_arr _temp2382={( void*) _temp2381,( void*) _temp2381,( void*)(
_temp2381 + 7u)}; _temp2380( _temp2382);});} goto _LL2136; _LL2154: s=({ struct
Cyc_PP_Doc*(* _temp2395)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2398= Cyc_PP_text( _tag_arr("typedef ", sizeof( unsigned char), 9u));
struct Cyc_PP_Doc* _temp2399= Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual
_temp2401; _temp2401.q_const= 0; _temp2401.q_volatile= 0; _temp2401.q_restrict=
0; _temp2401;}),( void*) _temp2218->defn,({ struct Cyc_Core_Opt* _temp2402=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2402->v=(
void*)({ struct Cyc_PP_Doc*(* _temp2403)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2406= Cyc_Absynpp_typedef_name2doc( _temp2218->name);
struct Cyc_PP_Doc* _temp2407= Cyc_Absynpp_tvars2doc( _temp2218->tvs); struct Cyc_PP_Doc*
_temp2404[ 2u]={ _temp2406, _temp2407}; struct _tagged_arr _temp2405={( void*)
_temp2404,( void*) _temp2404,( void*)( _temp2404 + 2u)}; _temp2403( _temp2405);});
_temp2402;})); struct Cyc_PP_Doc* _temp2400= Cyc_PP_text( _tag_arr(";", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp2396[ 3u]={ _temp2398, _temp2399,
_temp2400}; struct _tagged_arr _temp2397={( void*) _temp2396,( void*) _temp2396,(
void*)( _temp2396 + 3u)}; _temp2395( _temp2397);}); goto _LL2136; _LL2156: if(
Cyc_Absynpp_use_curr_namespace){ Cyc_Absynpp_curr_namespace_add( _temp2222);} s=({
struct Cyc_PP_Doc*(* _temp2408)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2411= Cyc_PP_text( _tag_arr("namespace ", sizeof( unsigned char), 11u));
struct Cyc_PP_Doc* _temp2412= Cyc_PP_textptr( _temp2222); struct Cyc_PP_Doc*
_temp2413= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc*
_temp2414= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2415=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_arr sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc, _tag_arr("",
sizeof( unsigned char), 1u), _temp2220); struct Cyc_PP_Doc* _temp2416= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2417= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp2409[ 7u]={ _temp2411, _temp2412, _temp2413,
_temp2414, _temp2415, _temp2416, _temp2417}; struct _tagged_arr _temp2410={(
void*) _temp2409,( void*) _temp2409,( void*)( _temp2409 + 7u)}; _temp2408(
_temp2410);}); if( Cyc_Absynpp_use_curr_namespace){ Cyc_Absynpp_curr_namespace_drop();}
goto _LL2136; _LL2158: if( Cyc_Absynpp_print_using_stmts){ s=({ struct Cyc_PP_Doc*(*
_temp2418)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2421= Cyc_PP_text(
_tag_arr("using ", sizeof( unsigned char), 7u)); struct Cyc_PP_Doc* _temp2422=
Cyc_Absynpp_qvar2doc( _temp2226); struct Cyc_PP_Doc* _temp2423= Cyc_PP_text(
_tag_arr(" {", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp2424= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2425=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)(
struct Cyc_Absyn_Decl*), struct _tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc, _tag_arr("", sizeof( unsigned char), 1u), _temp2224);
struct Cyc_PP_Doc* _temp2426= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2427=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp2419[ 7u]={ _temp2421, _temp2422, _temp2423, _temp2424, _temp2425,
_temp2426, _temp2427}; struct _tagged_arr _temp2420={( void*) _temp2419,( void*)
_temp2419,( void*)( _temp2419 + 7u)}; _temp2418( _temp2420);});} else{ s=({
struct Cyc_PP_Doc*(* _temp2428)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2431= Cyc_PP_text( _tag_arr("/* using ", sizeof( unsigned char), 10u));
struct Cyc_PP_Doc* _temp2432= Cyc_Absynpp_qvar2doc( _temp2226); struct Cyc_PP_Doc*
_temp2433= Cyc_PP_text( _tag_arr(" { */", sizeof( unsigned char), 6u)); struct
Cyc_PP_Doc* _temp2434= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2435=(( struct
Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp2224); struct Cyc_PP_Doc*
_temp2436= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2437= Cyc_PP_text(
_tag_arr("/* } */", sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp2429[
7u]={ _temp2431, _temp2432, _temp2433, _temp2434, _temp2435, _temp2436,
_temp2437}; struct _tagged_arr _temp2430={( void*) _temp2429,( void*) _temp2429,(
void*)( _temp2429 + 7u)}; _temp2428( _temp2430);});} goto _LL2136; _LL2160: if(
Cyc_Absynpp_print_externC_stmts){ s=({ struct Cyc_PP_Doc*(* _temp2438)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2441= Cyc_PP_text( _tag_arr("extern \"C\" {",
sizeof( unsigned char), 13u)); struct Cyc_PP_Doc* _temp2442= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2443=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)(
struct Cyc_Absyn_Decl*), struct _tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc, _tag_arr("", sizeof( unsigned char), 1u), _temp2228);
struct Cyc_PP_Doc* _temp2444= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2445=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp2439[ 5u]={ _temp2441, _temp2442, _temp2443, _temp2444, _temp2445}; struct
_tagged_arr _temp2440={( void*) _temp2439,( void*) _temp2439,( void*)( _temp2439
+ 5u)}; _temp2438( _temp2440);});} else{ s=({ struct Cyc_PP_Doc*(* _temp2446)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2449= Cyc_PP_text(
_tag_arr("/* extern \"C\" { */", sizeof( unsigned char), 19u)); struct Cyc_PP_Doc*
_temp2450= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2451=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_arr sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc, _tag_arr("",
sizeof( unsigned char), 1u), _temp2228); struct Cyc_PP_Doc* _temp2452= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2453= Cyc_PP_text( _tag_arr("/* } */", sizeof(
unsigned char), 8u)); struct Cyc_PP_Doc* _temp2447[ 5u]={ _temp2449, _temp2450,
_temp2451, _temp2452, _temp2453}; struct _tagged_arr _temp2448={( void*)
_temp2447,( void*) _temp2447,( void*)( _temp2447 + 5u)}; _temp2446( _temp2448);});}
goto _LL2136; _LL2136:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_scope2doc(
void* sc){ void* _temp2454= sc; _LL2456: if( _temp2454 ==( void*) Cyc_Absyn_Static){
goto _LL2457;} else{ goto _LL2458;} _LL2458: if( _temp2454 ==( void*) Cyc_Absyn_Public){
goto _LL2459;} else{ goto _LL2460;} _LL2460: if( _temp2454 ==( void*) Cyc_Absyn_Extern){
goto _LL2461;} else{ goto _LL2462;} _LL2462: if( _temp2454 ==( void*) Cyc_Absyn_ExternC){
goto _LL2463;} else{ goto _LL2464;} _LL2464: if( _temp2454 ==( void*) Cyc_Absyn_Abstract){
goto _LL2465;} else{ goto _LL2455;} _LL2457: return Cyc_PP_text( _tag_arr("static ",
sizeof( unsigned char), 8u)); _LL2459: return Cyc_PP_nil_doc(); _LL2461: return
Cyc_PP_text( _tag_arr("extern ", sizeof( unsigned char), 8u)); _LL2463: return
Cyc_PP_text( _tag_arr("extern \"C\" ", sizeof( unsigned char), 12u)); _LL2465:
return Cyc_PP_text( _tag_arr("abstract ", sizeof( unsigned char), 10u)); _LL2455:;}
int Cyc_Absynpp_exists_temp_tvar_in_effect( void* t){ void* _temp2466= t; struct
Cyc_Absyn_Tvar* _temp2474; struct Cyc_List_List* _temp2476; _LL2468: if((
unsigned int) _temp2466 > 4u?*(( int*) _temp2466) == Cyc_Absyn_VarType: 0){
_LL2475: _temp2474=(( struct Cyc_Absyn_VarType_struct*) _temp2466)->f1; goto
_LL2469;} else{ goto _LL2470;} _LL2470: if(( unsigned int) _temp2466 > 4u?*((
int*) _temp2466) == Cyc_Absyn_JoinEff: 0){ _LL2477: _temp2476=(( struct Cyc_Absyn_JoinEff_struct*)
_temp2466)->f1; goto _LL2471;} else{ goto _LL2472;} _LL2472: goto _LL2473;
_LL2469: return Cyc_Tcutil_is_temp_tvar( _temp2474); _LL2471: return(( int(*)(
int(* pred)( void*), struct Cyc_List_List* x)) Cyc_List_exists)( Cyc_Absynpp_exists_temp_tvar_in_effect,
_temp2476); _LL2473: return 0; _LL2467:;} struct _tuple4 Cyc_Absynpp_to_tms(
struct Cyc_Absyn_Tqual tq, void* t){ void* _temp2478= t; struct Cyc_Absyn_Exp*
_temp2492; struct Cyc_Absyn_Tqual _temp2494; void* _temp2496; struct Cyc_Absyn_PtrInfo
_temp2498; struct Cyc_Absyn_Conref* _temp2500; struct Cyc_Absyn_Tqual _temp2502;
struct Cyc_Absyn_Conref* _temp2504; void* _temp2506; void* _temp2508; struct Cyc_Absyn_FnInfo
_temp2510; struct Cyc_List_List* _temp2512; struct Cyc_List_List* _temp2514;
struct Cyc_Absyn_VarargInfo* _temp2516; int _temp2518; struct Cyc_List_List*
_temp2520; void* _temp2522; struct Cyc_Core_Opt* _temp2524; struct Cyc_List_List*
_temp2526; struct Cyc_Core_Opt* _temp2528; int _temp2530; struct Cyc_Core_Opt*
_temp2532; struct Cyc_Core_Opt* _temp2534; struct Cyc_Core_Opt* _temp2536;
struct Cyc_List_List* _temp2538; struct _tuple0* _temp2540; _LL2480: if((
unsigned int) _temp2478 > 4u?*(( int*) _temp2478) == Cyc_Absyn_ArrayType: 0){
_LL2497: _temp2496=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2478)->f1;
goto _LL2495; _LL2495: _temp2494=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2478)->f2; goto _LL2493; _LL2493: _temp2492=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2478)->f3; goto _LL2481;} else{ goto _LL2482;} _LL2482: if(( unsigned int)
_temp2478 > 4u?*(( int*) _temp2478) == Cyc_Absyn_PointerType: 0){ _LL2499:
_temp2498=(( struct Cyc_Absyn_PointerType_struct*) _temp2478)->f1; _LL2509:
_temp2508=( void*) _temp2498.elt_typ; goto _LL2507; _LL2507: _temp2506=( void*)
_temp2498.rgn_typ; goto _LL2505; _LL2505: _temp2504= _temp2498.nullable; goto
_LL2503; _LL2503: _temp2502= _temp2498.tq; goto _LL2501; _LL2501: _temp2500=
_temp2498.bounds; goto _LL2483;} else{ goto _LL2484;} _LL2484: if(( unsigned int)
_temp2478 > 4u?*(( int*) _temp2478) == Cyc_Absyn_FnType: 0){ _LL2511: _temp2510=((
struct Cyc_Absyn_FnType_struct*) _temp2478)->f1; _LL2527: _temp2526= _temp2510.tvars;
goto _LL2525; _LL2525: _temp2524= _temp2510.effect; goto _LL2523; _LL2523:
_temp2522=( void*) _temp2510.ret_typ; goto _LL2521; _LL2521: _temp2520=
_temp2510.args; goto _LL2519; _LL2519: _temp2518= _temp2510.c_varargs; goto
_LL2517; _LL2517: _temp2516= _temp2510.cyc_varargs; goto _LL2515; _LL2515:
_temp2514= _temp2510.rgn_po; goto _LL2513; _LL2513: _temp2512= _temp2510.attributes;
goto _LL2485;} else{ goto _LL2486;} _LL2486: if(( unsigned int) _temp2478 > 4u?*((
int*) _temp2478) == Cyc_Absyn_Evar: 0){ _LL2535: _temp2534=(( struct Cyc_Absyn_Evar_struct*)
_temp2478)->f1; goto _LL2533; _LL2533: _temp2532=(( struct Cyc_Absyn_Evar_struct*)
_temp2478)->f2; goto _LL2531; _LL2531: _temp2530=(( struct Cyc_Absyn_Evar_struct*)
_temp2478)->f3; goto _LL2529; _LL2529: _temp2528=(( struct Cyc_Absyn_Evar_struct*)
_temp2478)->f4; goto _LL2487;} else{ goto _LL2488;} _LL2488: if(( unsigned int)
_temp2478 > 4u?*(( int*) _temp2478) == Cyc_Absyn_TypedefType: 0){ _LL2541:
_temp2540=(( struct Cyc_Absyn_TypedefType_struct*) _temp2478)->f1; goto _LL2539;
_LL2539: _temp2538=(( struct Cyc_Absyn_TypedefType_struct*) _temp2478)->f2; goto
_LL2537; _LL2537: _temp2536=(( struct Cyc_Absyn_TypedefType_struct*) _temp2478)->f3;
goto _LL2489;} else{ goto _LL2490;} _LL2490: goto _LL2491; _LL2481: { struct Cyc_List_List*
_temp2544; void* _temp2546; struct Cyc_Absyn_Tqual _temp2548; struct _tuple4
_temp2542= Cyc_Absynpp_to_tms( _temp2494, _temp2496); _LL2549: _temp2548=
_temp2542.f1; goto _LL2547; _LL2547: _temp2546= _temp2542.f2; goto _LL2545;
_LL2545: _temp2544= _temp2542.f3; goto _LL2543; _LL2543: { void* tm; if(
_temp2492 == 0){ tm=( void*) Cyc_Absyn_Carray_mod;} else{ tm=( void*)({ struct
Cyc_Absyn_ConstArray_mod_struct* _temp2550=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2550[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2551; _temp2551.tag= Cyc_Absyn_ConstArray_mod;
_temp2551.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp2492); _temp2551;});
_temp2550;});} return({ struct _tuple4 _temp2552; _temp2552.f1= _temp2548;
_temp2552.f2= _temp2546; _temp2552.f3=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp2553=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2553->hd=( void*) tm; _temp2553->tl= _temp2544; _temp2553;}); _temp2552;});}}
_LL2483: { struct Cyc_List_List* _temp2556; void* _temp2558; struct Cyc_Absyn_Tqual
_temp2560; struct _tuple4 _temp2554= Cyc_Absynpp_to_tms( _temp2502, _temp2508);
_LL2561: _temp2560= _temp2554.f1; goto _LL2559; _LL2559: _temp2558= _temp2554.f2;
goto _LL2557; _LL2557: _temp2556= _temp2554.f3; goto _LL2555; _LL2555: { void*
ps;{ struct _tuple8 _temp2563=({ struct _tuple8 _temp2562; _temp2562.f1=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp2504))->v; _temp2562.f2=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp2500))->v; _temp2562;}); void* _temp2573;
void* _temp2575; struct Cyc_Absyn_Exp* _temp2577; void* _temp2579; int _temp2581;
void* _temp2583; void* _temp2585; struct Cyc_Absyn_Exp* _temp2587; void*
_temp2589; int _temp2591; void* _temp2593; void* _temp2595; void* _temp2597;
_LL2565: _LL2580: _temp2579= _temp2563.f1; if(( unsigned int) _temp2579 > 1u?*((
int*) _temp2579) == Cyc_Absyn_Eq_constr: 0){ _LL2582: _temp2581=( int)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp2579)->f1; if( _temp2581 == 1){ goto _LL2574;}
else{ goto _LL2567;}} else{ goto _LL2567;} _LL2574: _temp2573= _temp2563.f2; if((
unsigned int) _temp2573 > 1u?*(( int*) _temp2573) == Cyc_Absyn_Eq_constr: 0){
_LL2576: _temp2575=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2573)->f1;
if(( unsigned int) _temp2575 > 1u?*(( int*) _temp2575) == Cyc_Absyn_Upper_b: 0){
_LL2578: _temp2577=(( struct Cyc_Absyn_Upper_b_struct*) _temp2575)->f1; goto
_LL2566;} else{ goto _LL2567;}} else{ goto _LL2567;} _LL2567: _LL2590: _temp2589=
_temp2563.f1; if(( unsigned int) _temp2589 > 1u?*(( int*) _temp2589) == Cyc_Absyn_Eq_constr:
0){ _LL2592: _temp2591=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2589)->f1;
if( _temp2591 == 0){ goto _LL2584;} else{ goto _LL2569;}} else{ goto _LL2569;}
_LL2584: _temp2583= _temp2563.f2; if(( unsigned int) _temp2583 > 1u?*(( int*)
_temp2583) == Cyc_Absyn_Eq_constr: 0){ _LL2586: _temp2585=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2583)->f1; if(( unsigned int) _temp2585 > 1u?*(( int*) _temp2585) == Cyc_Absyn_Upper_b:
0){ _LL2588: _temp2587=(( struct Cyc_Absyn_Upper_b_struct*) _temp2585)->f1; goto
_LL2568;} else{ goto _LL2569;}} else{ goto _LL2569;} _LL2569: _LL2598: _temp2597=
_temp2563.f1; goto _LL2594; _LL2594: _temp2593= _temp2563.f2; if(( unsigned int)
_temp2593 > 1u?*(( int*) _temp2593) == Cyc_Absyn_Eq_constr: 0){ _LL2596:
_temp2595=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2593)->f1; if(
_temp2595 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL2570;} else{ goto _LL2571;}}
else{ goto _LL2571;} _LL2571: goto _LL2572; _LL2566: ps=( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2599=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2599[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2600; _temp2600.tag=
Cyc_Absyn_Nullable_ps; _temp2600.f1= _temp2577; _temp2600;}); _temp2599;}); goto
_LL2564; _LL2568: ps=( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2601=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2601[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2602; _temp2602.tag= Cyc_Absyn_NonNullable_ps; _temp2602.f1= _temp2587;
_temp2602;}); _temp2601;}); goto _LL2564; _LL2570: ps=( void*) Cyc_Absyn_TaggedArray_ps;
goto _LL2564; _LL2572: ps=( void*) Cyc_Absyn_TaggedArray_ps; goto _LL2564;
_LL2564:;}{ void* tm=( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2605=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp2605[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp2606; _temp2606.tag=
Cyc_Absyn_Pointer_mod; _temp2606.f1=( void*) ps; _temp2606.f2=( void*) _temp2506;
_temp2606.f3= tq; _temp2606;}); _temp2605;}); return({ struct _tuple4 _temp2603;
_temp2603.f1= _temp2560; _temp2603.f2= _temp2558; _temp2603.f3=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp2604=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2604->hd=( void*) tm; _temp2604->tl= _temp2556;
_temp2604;}); _temp2603;});}}} _LL2485: if( ! Cyc_Absynpp_print_all_tvars){ if(
_temp2524 == 0? 1: Cyc_Absynpp_exists_temp_tvar_in_effect(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2524))->v)){ _temp2524= 0; _temp2526= 0;}} else{ if( Cyc_Absynpp_rewrite_temp_tvars){((
void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)(
Cyc_Tcutil_rewrite_temp_tvar, _temp2526);}}{ struct Cyc_List_List* _temp2609;
void* _temp2611; struct Cyc_Absyn_Tqual _temp2613; struct _tuple4 _temp2607= Cyc_Absynpp_to_tms(
Cyc_Absyn_empty_tqual(), _temp2522); _LL2614: _temp2613= _temp2607.f1; goto
_LL2612; _LL2612: _temp2611= _temp2607.f2; goto _LL2610; _LL2610: _temp2609=
_temp2607.f3; goto _LL2608; _LL2608: { struct Cyc_List_List* tms= _temp2609; if(
_temp2512 != 0){ tms=({ struct Cyc_List_List* _temp2615=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2615->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2616=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2616[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2617; _temp2617.tag= Cyc_Absyn_Attributes_mod;
_temp2617.f1= 0; _temp2617.f2= _temp2512; _temp2617;}); _temp2616;})); _temp2615->tl=
tms; _temp2615;});} tms=({ struct Cyc_List_List* _temp2618=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2618->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2619=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2619[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2620; _temp2620.tag= Cyc_Absyn_Function_mod;
_temp2620.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2621=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2621[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2622; _temp2622.tag= Cyc_Absyn_WithTypes;
_temp2622.f1= _temp2520; _temp2622.f2= _temp2518; _temp2622.f3= _temp2516;
_temp2622.f4= _temp2524; _temp2622.f5= _temp2514; _temp2622;}); _temp2621;}));
_temp2620;}); _temp2619;})); _temp2618->tl= tms; _temp2618;}); if( _temp2526 !=
0){ tms=({ struct Cyc_List_List* _temp2623=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2623->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp2624=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp2624[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp2625; _temp2625.tag= Cyc_Absyn_TypeParams_mod; _temp2625.f1= _temp2526;
_temp2625.f2= 0; _temp2625.f3= 1; _temp2625;}); _temp2624;})); _temp2623->tl=
tms; _temp2623;});} return({ struct _tuple4 _temp2626; _temp2626.f1= _temp2613;
_temp2626.f2= _temp2611; _temp2626.f3= tms; _temp2626;});}} _LL2487: if(
_temp2532 == 0){ return({ struct _tuple4 _temp2627; _temp2627.f1= tq; _temp2627.f2=
t; _temp2627.f3= 0; _temp2627;});} else{ return Cyc_Absynpp_to_tms( tq,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2532))->v);} _LL2489: if( _temp2536 == 0?
1: ! Cyc_Absynpp_expand_typedefs){ return({ struct _tuple4 _temp2628; _temp2628.f1=
tq; _temp2628.f2= t; _temp2628.f3= 0; _temp2628;});} else{ return Cyc_Absynpp_to_tms(
tq,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2536))->v);} _LL2491:
return({ struct _tuple4 _temp2629; _temp2629.f1= tq; _temp2629.f2= t; _temp2629.f3=
0; _temp2629;}); _LL2479:;} struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc( struct Cyc_Absyn_Tqual
tq, void* t, struct Cyc_Core_Opt* dopt){ struct Cyc_List_List* _temp2632; void*
_temp2634; struct Cyc_Absyn_Tqual _temp2636; struct _tuple4 _temp2630= Cyc_Absynpp_to_tms(
tq, t); _LL2637: _temp2636= _temp2630.f1; goto _LL2635; _LL2635: _temp2634=
_temp2630.f2; goto _LL2633; _LL2633: _temp2632= _temp2630.f3; goto _LL2631;
_LL2631: _temp2632=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp2632); if( _temp2632 == 0? dopt == 0: 0){ return({ struct Cyc_PP_Doc*(*
_temp2638)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2641= Cyc_Absynpp_tqual2doc(
_temp2636); struct Cyc_PP_Doc* _temp2642= Cyc_Absynpp_ntyp2doc( _temp2634);
struct Cyc_PP_Doc* _temp2639[ 2u]={ _temp2641, _temp2642}; struct _tagged_arr
_temp2640={( void*) _temp2639,( void*) _temp2639,( void*)( _temp2639 + 2u)};
_temp2638( _temp2640);});} else{ return({ struct Cyc_PP_Doc*(* _temp2643)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2646= Cyc_Absynpp_tqual2doc(
_temp2636); struct Cyc_PP_Doc* _temp2647= Cyc_Absynpp_ntyp2doc( _temp2634);
struct Cyc_PP_Doc* _temp2648= Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp2649= Cyc_Absynpp_dtms2doc( dopt == 0? Cyc_PP_nil_doc():(
struct Cyc_PP_Doc*)(( struct Cyc_Core_Opt*) _check_null( dopt))->v, _temp2632);
struct Cyc_PP_Doc* _temp2644[ 4u]={ _temp2646, _temp2647, _temp2648, _temp2649};
struct _tagged_arr _temp2645={( void*) _temp2644,( void*) _temp2644,( void*)(
_temp2644 + 4u)}; _temp2643( _temp2645);});}} struct Cyc_PP_Doc* Cyc_Absynpp_structfield2doc(
struct Cyc_Absyn_Structfield* f){ if( f->width != 0){ return({ struct Cyc_PP_Doc*(*
_temp2650)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2653= Cyc_Absynpp_tqtd2doc(
f->tq,( void*) f->type,({ struct Cyc_Core_Opt* _temp2658=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2658->v=( void*) Cyc_PP_textptr(
f->name); _temp2658;})); struct Cyc_PP_Doc* _temp2654= Cyc_PP_text( _tag_arr(":",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp2655= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( f->width))->v);
struct Cyc_PP_Doc* _temp2656= Cyc_Absynpp_atts2doc( f->attributes); struct Cyc_PP_Doc*
_temp2657= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp2651[ 5u]={ _temp2653, _temp2654, _temp2655, _temp2656, _temp2657}; struct
_tagged_arr _temp2652={( void*) _temp2651,( void*) _temp2651,( void*)( _temp2651
+ 5u)}; _temp2650( _temp2652);});} else{ return({ struct Cyc_PP_Doc*(* _temp2659)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2662= Cyc_Absynpp_tqtd2doc(
f->tq,( void*) f->type,({ struct Cyc_Core_Opt* _temp2665=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2665->v=( void*) Cyc_PP_textptr(
f->name); _temp2665;})); struct Cyc_PP_Doc* _temp2663= Cyc_Absynpp_atts2doc( f->attributes);
struct Cyc_PP_Doc* _temp2664= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp2660[ 3u]={ _temp2662, _temp2663, _temp2664};
struct _tagged_arr _temp2661={( void*) _temp2660,( void*) _temp2660,( void*)(
_temp2660 + 3u)}; _temp2659( _temp2661);});}} struct Cyc_PP_Doc* Cyc_Absynpp_structfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Structfield*), struct _tagged_arr sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_structfield2doc, _tag_arr("", sizeof(
unsigned char), 1u), fields);} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfield2doc(
struct Cyc_Absyn_Tunionfield* f){ return({ struct Cyc_PP_Doc*(* _temp2666)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2669= Cyc_Absynpp_scope2doc((
void*) f->sc); struct Cyc_PP_Doc* _temp2670= Cyc_Absynpp_typedef_name2doc( f->name);
struct Cyc_PP_Doc* _temp2671= f->tvs == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_ktvars2doc(
f->tvs); struct Cyc_PP_Doc* _temp2672= f->typs == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(
f->typs); struct Cyc_PP_Doc* _temp2667[ 4u]={ _temp2669, _temp2670, _temp2671,
_temp2672}; struct _tagged_arr _temp2668={( void*) _temp2667,( void*) _temp2667,(
void*)( _temp2667 + 4u)}; _temp2666( _temp2668);});} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Tunionfield*), struct _tagged_arr sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_tunionfield2doc, _tag_arr(",", sizeof(
unsigned char), 2u), fields);} void Cyc_Absynpp_decllist2file( struct Cyc_List_List*
tdl, struct Cyc_Stdio___sFILE* f){ for( 0; tdl != 0; tdl=(( struct Cyc_List_List*)
_check_null( tdl))->tl){ Cyc_PP_file_of_doc( Cyc_Absynpp_decl2doc(( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( tdl))->hd), 72, f); fprintf( _sfile_to_file(
f),"\n");}} struct _tagged_arr Cyc_Absynpp_decllist2string( struct Cyc_List_List*
tdl){ return Cyc_PP_string_of_doc( Cyc_PP_seql( _tag_arr("", sizeof(
unsigned char), 1u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
Cyc_Absyn_Decl*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_decl2doc,
tdl)), 72);} struct _tagged_arr Cyc_Absynpp_exp2string( struct Cyc_Absyn_Exp* e){
return Cyc_PP_string_of_doc( Cyc_Absynpp_exp2doc( e), 72);} struct _tagged_arr
Cyc_Absynpp_stmt2string( struct Cyc_Absyn_Stmt* s){ return Cyc_PP_string_of_doc(
Cyc_Absynpp_stmt2doc( s), 72);} struct _tagged_arr Cyc_Absynpp_typ2string( void*
t){ return Cyc_PP_string_of_doc( Cyc_Absynpp_typ2doc( t), 72);} struct
_tagged_arr Cyc_Absynpp_qvar2string( struct _tuple0* v){ return Cyc_PP_string_of_doc(
Cyc_Absynpp_qvar2doc( v), 72);} struct _tagged_arr Cyc_Absynpp_prim2string( void*
p){ return Cyc_PP_string_of_doc( Cyc_Absynpp_prim2doc( p), 72);} struct
_tagged_arr Cyc_Absynpp_pat2string( struct Cyc_Absyn_Pat* p){ return Cyc_PP_string_of_doc(
Cyc_Absynpp_pat2doc( p), 72);} struct _tagged_arr Cyc_Absynpp_scope2string( void*
sc){ void* _temp2673= sc; _LL2675: if( _temp2673 ==( void*) Cyc_Absyn_Static){
goto _LL2676;} else{ goto _LL2677;} _LL2677: if( _temp2673 ==( void*) Cyc_Absyn_Public){
goto _LL2678;} else{ goto _LL2679;} _LL2679: if( _temp2673 ==( void*) Cyc_Absyn_Extern){
goto _LL2680;} else{ goto _LL2681;} _LL2681: if( _temp2673 ==( void*) Cyc_Absyn_ExternC){
goto _LL2682;} else{ goto _LL2683;} _LL2683: if( _temp2673 ==( void*) Cyc_Absyn_Abstract){
goto _LL2684;} else{ goto _LL2674;} _LL2676: return _tag_arr("static", sizeof(
unsigned char), 7u); _LL2678: return _tag_arr("public", sizeof( unsigned char),
7u); _LL2680: return _tag_arr("extern", sizeof( unsigned char), 7u); _LL2682:
return _tag_arr("extern \"C\"", sizeof( unsigned char), 11u); _LL2684: return
_tag_arr("abstract", sizeof( unsigned char), 9u); _LL2674:;}