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
_tagged_arr _temp14= Cyc_Core_new_string( 1); goto _LL15; _LL15:*((
unsigned char*) _check_unknown_subscript( _temp14, sizeof( unsigned char), 0))=
c; return( struct _tagged_arr) _temp14;} else{ struct _tagged_arr _temp16= Cyc_Core_new_string(
4); goto _LL17; _LL17: { int j= 0;*(( unsigned char*) _check_unknown_subscript(
_temp16, sizeof( unsigned char), j ++))='\\';*(( unsigned char*)
_check_unknown_subscript( _temp16, sizeof( unsigned char), j ++))=(
unsigned char)('0' +(( unsigned char) c >> 6 & 7));*(( unsigned char*)
_check_unknown_subscript( _temp16, sizeof( unsigned char), j ++))=(
unsigned char)('0' +( c >> 3 & 7));*(( unsigned char*) _check_unknown_subscript(
_temp16, sizeof( unsigned char), j ++))=( unsigned char)('0' +( c & 7)); return(
struct _tagged_arr) _temp16;}}}} static int Cyc_Absynpp_special( struct
_tagged_arr s){ int sz=( int)( _get_arr_size( s, sizeof( unsigned char)) - 1);{
int i= 0; for( 0; i < sz; i ++){ unsigned char c=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)); if((( c <=' '? 1: c >='~')?
1: c =='"')? 1: c =='\\'){ return 1;}}} return 0;} struct _tagged_arr Cyc_Absynpp_string_escape(
struct _tagged_arr s){ if( ! Cyc_Absynpp_special( s)){ return s;}{ int n=( int)(
_get_arr_size( s, sizeof( unsigned char)) - 1); if( n > 0?*(( const
unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), n)) =='\000':
0){ n --;}{ int len= 0;{ int i= 0; for( 0; i <= n; i ++){ unsigned char _temp18=*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i));
_LL20: if( _temp18 =='\a'){ goto _LL21;} else{ goto _LL22;} _LL22: if( _temp18
=='\b'){ goto _LL23;} else{ goto _LL24;} _LL24: if( _temp18 =='\f'){ goto _LL25;}
else{ goto _LL26;} _LL26: if( _temp18 =='\n'){ goto _LL27;} else{ goto _LL28;}
_LL28: if( _temp18 =='\r'){ goto _LL29;} else{ goto _LL30;} _LL30: if( _temp18
=='\t'){ goto _LL31;} else{ goto _LL32;} _LL32: if( _temp18 =='\v'){ goto _LL33;}
else{ goto _LL34;} _LL34: if( _temp18 =='\\'){ goto _LL35;} else{ goto _LL36;}
_LL36: if( _temp18 =='"'){ goto _LL37;} else{ goto _LL38;} _LL38: goto _LL39;
_LL21: goto _LL23; _LL23: goto _LL25; _LL25: goto _LL27; _LL27: goto _LL29;
_LL29: goto _LL31; _LL31: goto _LL33; _LL33: goto _LL35; _LL35: goto _LL37;
_LL37: len += 2; goto _LL19; _LL39: if( _temp18 >=' '? _temp18 <='~': 0){ len ++;}
else{ len += 4;} goto _LL19; _LL19:;}}{ struct _tagged_arr t= Cyc_Core_new_string(
len); int j= 0;{ int i= 0; for( 0; i <= n; i ++){ unsigned char _temp40=*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i));
_LL42: if( _temp40 =='\a'){ goto _LL43;} else{ goto _LL44;} _LL44: if( _temp40
=='\b'){ goto _LL45;} else{ goto _LL46;} _LL46: if( _temp40 =='\f'){ goto _LL47;}
else{ goto _LL48;} _LL48: if( _temp40 =='\n'){ goto _LL49;} else{ goto _LL50;}
_LL50: if( _temp40 =='\r'){ goto _LL51;} else{ goto _LL52;} _LL52: if( _temp40
=='\t'){ goto _LL53;} else{ goto _LL54;} _LL54: if( _temp40 =='\v'){ goto _LL55;}
else{ goto _LL56;} _LL56: if( _temp40 =='\\'){ goto _LL57;} else{ goto _LL58;}
_LL58: if( _temp40 =='"'){ goto _LL59;} else{ goto _LL60;} _LL60: goto _LL61;
_LL43:*(( unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j
++))='\\';*(( unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char),
j ++))='a'; goto _LL41; _LL45:*(( unsigned char*) _check_unknown_subscript( t,
sizeof( unsigned char), j ++))='\\';*(( unsigned char*) _check_unknown_subscript(
t, sizeof( unsigned char), j ++))='b'; goto _LL41; _LL47:*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))='\\';*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='f';
goto _LL41; _LL49:*(( unsigned char*) _check_unknown_subscript( t, sizeof(
unsigned char), j ++))='\\';*(( unsigned char*) _check_unknown_subscript( t,
sizeof( unsigned char), j ++))='n'; goto _LL41; _LL51:*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))='\\';*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='r';
goto _LL41; _LL53:*(( unsigned char*) _check_unknown_subscript( t, sizeof(
unsigned char), j ++))='\\';*(( unsigned char*) _check_unknown_subscript( t,
sizeof( unsigned char), j ++))='t'; goto _LL41; _LL55:*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))='\\';*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='v';
goto _LL41; _LL57:*(( unsigned char*) _check_unknown_subscript( t, sizeof(
unsigned char), j ++))='\\';*(( unsigned char*) _check_unknown_subscript( t,
sizeof( unsigned char), j ++))='\\'; goto _LL41; _LL59:*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))='\\';*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='"';
goto _LL41; _LL61: if( _temp40 >=' '? _temp40 <='~': 0){*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))= _temp40;} else{*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))='\\';*((
unsigned char*) _check_unknown_subscript( t, sizeof( unsigned char), j ++))=(
unsigned char)('0' +( _temp40 >> 6 & 7));*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), j ++))=( unsigned char)('0'
+( _temp40 >> 3 & 7));*(( unsigned char*) _check_unknown_subscript( t, sizeof(
unsigned char), j ++))=( unsigned char)('0' +( _temp40 & 7));} goto _LL41; _LL41:;}}
return( struct _tagged_arr) t;}}}} static unsigned char _temp62[ 9u]="restrict";
static struct _tagged_arr Cyc_Absynpp_restrict_string={ _temp62, _temp62,
_temp62 + 9u}; static unsigned char _temp63[ 9u]="volatile"; static struct
_tagged_arr Cyc_Absynpp_volatile_string={ _temp63, _temp63, _temp63 + 9u};
static unsigned char _temp64[ 6u]="const"; static struct _tagged_arr Cyc_Absynpp_const_str={
_temp64, _temp64, _temp64 + 6u}; static struct _tagged_arr* Cyc_Absynpp_restrict_sp=&
Cyc_Absynpp_restrict_string; static struct _tagged_arr* Cyc_Absynpp_volatile_sp=&
Cyc_Absynpp_volatile_string; static struct _tagged_arr* Cyc_Absynpp_const_sp=&
Cyc_Absynpp_const_str; struct Cyc_PP_Doc* Cyc_Absynpp_tqual2doc( struct Cyc_Absyn_Tqual
tq){ struct Cyc_List_List* l= 0; if( tq.q_restrict){ l=({ struct Cyc_List_List*
_temp65=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp65->hd=( void*) Cyc_Absynpp_restrict_sp; _temp65->tl= l; _temp65;});} if(
tq.q_volatile){ l=({ struct Cyc_List_List* _temp66=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp66->hd=( void*) Cyc_Absynpp_volatile_sp;
_temp66->tl= l; _temp66;});} if( tq.q_const){ l=({ struct Cyc_List_List* _temp67=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp67->hd=(
void*) Cyc_Absynpp_const_sp; _temp67->tl= l; _temp67;});} return Cyc_PP_egroup(
_tag_arr("", sizeof( unsigned char), 1u), _tag_arr(" ", sizeof( unsigned char),
2u), _tag_arr(" ", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tagged_arr*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_PP_textptr, l));} struct _tagged_arr Cyc_Absynpp_kind2string( void* k){ void*
_temp68= k; _LL70: if( _temp68 ==( void*) Cyc_Absyn_AnyKind){ goto _LL71;} else{
goto _LL72;} _LL72: if( _temp68 ==( void*) Cyc_Absyn_MemKind){ goto _LL73;}
else{ goto _LL74;} _LL74: if( _temp68 ==( void*) Cyc_Absyn_BoxKind){ goto _LL75;}
else{ goto _LL76;} _LL76: if( _temp68 ==( void*) Cyc_Absyn_RgnKind){ goto _LL77;}
else{ goto _LL78;} _LL78: if( _temp68 ==( void*) Cyc_Absyn_EffKind){ goto _LL79;}
else{ goto _LL69;} _LL71: return _tag_arr("A", sizeof( unsigned char), 2u);
_LL73: return _tag_arr("M", sizeof( unsigned char), 2u); _LL75: return _tag_arr("B",
sizeof( unsigned char), 2u); _LL77: return _tag_arr("R", sizeof( unsigned char),
2u); _LL79: return _tag_arr("E", sizeof( unsigned char), 2u); _LL69:;} struct
_tagged_arr Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref* c){ void* _temp80=(
void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
c))->v; void* _temp86; _LL82: if(( unsigned int) _temp80 > 1u?*(( int*) _temp80)
== Cyc_Absyn_Eq_constr: 0){ _LL87: _temp86=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp80)->f1; goto _LL83;} else{ goto _LL84;} _LL84: goto _LL85; _LL83: return
Cyc_Absynpp_kind2string( _temp86); _LL85: return _tag_arr("?", sizeof(
unsigned char), 2u); _LL81:;} struct Cyc_PP_Doc* Cyc_Absynpp_kind2doc( void* k){
return Cyc_PP_text( Cyc_Absynpp_kind2string( k));} struct Cyc_PP_Doc* Cyc_Absynpp_ckind2doc(
struct Cyc_Absyn_Conref* c){ return Cyc_PP_text( Cyc_Absynpp_ckind2string( c));}
struct Cyc_PP_Doc* Cyc_Absynpp_tps2doc( struct Cyc_List_List* ts){ return Cyc_PP_egroup(
_tag_arr("<", sizeof( unsigned char), 2u), _tag_arr(">", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_typ2doc,
ts));} struct Cyc_PP_Doc* Cyc_Absynpp_ktvar2doc( struct Cyc_Absyn_Tvar* tv){
void* _temp88=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( tv->kind))->v; void* _temp96; void* _temp98;
_LL90: if(( unsigned int) _temp88 > 1u?*(( int*) _temp88) == Cyc_Absyn_Eq_constr:
0){ _LL97: _temp96=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp88)->f1;
if( _temp96 ==( void*) Cyc_Absyn_BoxKind){ goto _LL91;} else{ goto _LL92;}}
else{ goto _LL92;} _LL92: if(( unsigned int) _temp88 > 1u?*(( int*) _temp88) ==
Cyc_Absyn_Eq_constr: 0){ _LL99: _temp98=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp88)->f1; goto _LL93;} else{ goto _LL94;} _LL94: goto _LL95; _LL91: return
Cyc_PP_textptr( tv->name); _LL93: return({ struct Cyc_PP_Doc*(* _temp100)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp103= Cyc_PP_textptr( tv->name);
struct Cyc_PP_Doc* _temp104= Cyc_PP_text( _tag_arr("::", sizeof( unsigned char),
3u)); struct Cyc_PP_Doc* _temp105= Cyc_Absynpp_kind2doc( _temp98); struct Cyc_PP_Doc*
_temp101[ 3u]={ _temp103, _temp104, _temp105}; struct _tagged_arr _temp102={(
void*) _temp101,( void*) _temp101,( void*)( _temp101 + 3u)}; _temp100( _temp102);});
_LL95: return({ struct Cyc_PP_Doc*(* _temp106)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp109= Cyc_PP_textptr( tv->name); struct Cyc_PP_Doc*
_temp110= Cyc_PP_text( _tag_arr("/*::?*/", sizeof( unsigned char), 8u)); struct
Cyc_PP_Doc* _temp107[ 2u]={ _temp109, _temp110}; struct _tagged_arr _temp108={(
void*) _temp107,( void*) _temp107,( void*)( _temp107 + 2u)}; _temp106( _temp108);});
_LL89:;} struct Cyc_PP_Doc* Cyc_Absynpp_ktvars2doc( struct Cyc_List_List* tvs){
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
void* att){ void* _temp111= att; _LL113: if( _temp111 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL114;} else{ goto _LL115;} _LL115: if( _temp111 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL116;} else{ goto _LL117;} _LL117: if( _temp111 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL118;} else{ goto _LL119;} _LL119: goto _LL120; _LL114: return Cyc_PP_nil_doc();
_LL116: return Cyc_PP_nil_doc(); _LL118: return Cyc_PP_nil_doc(); _LL120: return
Cyc_PP_text( Cyc_Absyn_attribute2string( att)); _LL112:;} struct Cyc_PP_Doc* Cyc_Absynpp_callconv2doc(
struct Cyc_List_List* atts){ for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp121=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; _LL123: if( _temp121 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL124;} else{ goto _LL125;} _LL125: if( _temp121 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL126;} else{ goto _LL127;} _LL127: if( _temp121 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL128;} else{ goto _LL129;} _LL129: goto _LL130; _LL124: return Cyc_PP_text(
_tag_arr("_stdcall", sizeof( unsigned char), 9u)); _LL126: return Cyc_PP_text(
_tag_arr("_cdecl", sizeof( unsigned char), 7u)); _LL128: return Cyc_PP_text(
_tag_arr("_fastcall", sizeof( unsigned char), 10u)); _LL130: goto _LL122; _LL122:;}
return Cyc_PP_nil_doc();} struct Cyc_PP_Doc* Cyc_Absynpp_noncallconv2doc( struct
Cyc_List_List* atts){ int hasatt= 0;{ struct Cyc_List_List* atts2= atts; for( 0;
atts2 != 0; atts2=(( struct Cyc_List_List*) _check_null( atts2))->tl){ void*
_temp131=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd; _LL133: if(
_temp131 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL134;} else{ goto _LL135;}
_LL135: if( _temp131 ==( void*) Cyc_Absyn_Cdecl_att){ goto _LL136;} else{ goto
_LL137;} _LL137: if( _temp131 ==( void*) Cyc_Absyn_Fastcall_att){ goto _LL138;}
else{ goto _LL139;} _LL139: goto _LL140; _LL134: goto _LL132; _LL136: goto
_LL132; _LL138: goto _LL132; _LL140: hasatt= 1; goto _LL132; _LL132:;}} if( !
hasatt){ return Cyc_PP_nil_doc();} return({ struct Cyc_PP_Doc*(* _temp141)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp144= Cyc_PP_text(
_tag_arr("__declspec(", sizeof( unsigned char), 12u)); struct Cyc_PP_Doc*
_temp145= Cyc_PP_group( _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("",
sizeof( unsigned char), 1u), _tag_arr(" ", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x))
Cyc_List_map)( Cyc_Absynpp_noncallatt2doc, atts)); struct Cyc_PP_Doc* _temp146=
Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp142[ 3u]={ _temp144, _temp145, _temp146}; struct _tagged_arr _temp143={(
void*) _temp142,( void*) _temp142,( void*)( _temp142 + 3u)}; _temp141( _temp143);});}
struct Cyc_PP_Doc* Cyc_Absynpp_att2doc( void* a){ return Cyc_PP_text( Cyc_Absyn_attribute2string(
a));} struct Cyc_PP_Doc* Cyc_Absynpp_atts2doc( struct Cyc_List_List* atts){ if(
atts == 0){ return Cyc_PP_nil_doc();} if( Cyc_Absynpp_to_VC){ return Cyc_Absynpp_noncallconv2doc(
atts);} return({ struct Cyc_PP_Doc*(* _temp147)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp150= Cyc_PP_text( _tag_arr(" __attribute__", sizeof(
unsigned char), 15u)); struct Cyc_PP_Doc* _temp151= Cyc_PP_group( _tag_arr("((",
sizeof( unsigned char), 3u), _tag_arr("))", sizeof( unsigned char), 3u),
_tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct
Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_att2doc,
atts)); struct Cyc_PP_Doc* _temp148[ 2u]={ _temp150, _temp151}; struct
_tagged_arr _temp149={( void*) _temp148,( void*) _temp148,( void*)( _temp148 + 2u)};
_temp147( _temp149);});} int Cyc_Absynpp_next_is_pointer( struct Cyc_List_List*
tms){ if( tms == 0){ return 0;}{ void* _temp152=( void*)(( struct Cyc_List_List*)
_check_null( tms))->hd; struct Cyc_Absyn_Tqual _temp158; void* _temp160; void*
_temp162; _LL154: if(( unsigned int) _temp152 > 1u?*(( int*) _temp152) == Cyc_Absyn_Pointer_mod:
0){ _LL163: _temp162=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp152)->f1;
goto _LL161; _LL161: _temp160=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp152)->f2; goto _LL159; _LL159: _temp158=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp152)->f3; goto _LL155;} else{ goto _LL156;} _LL156: goto _LL157; _LL155:
return 1; _LL157: return 0; _LL153:;}} extern struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc(
void* t); struct Cyc_PP_Doc* Cyc_Absynpp_dtms2doc( struct Cyc_PP_Doc* d, struct
Cyc_List_List* tms){ if( tms == 0){ return d;}{ struct Cyc_PP_Doc* rest= Cyc_Absynpp_dtms2doc(
d,(( struct Cyc_List_List*) _check_null( tms))->tl); struct Cyc_PP_Doc* p_rest=({
struct Cyc_PP_Doc*(* _temp299)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp302= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp303= rest; struct Cyc_PP_Doc* _temp304= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp300[ 3u]={ _temp302, _temp303,
_temp304}; struct _tagged_arr _temp301={( void*) _temp300,( void*) _temp300,(
void*)( _temp300 + 3u)}; _temp299( _temp301);}); void* _temp164=( void*)((
struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Exp* _temp178;
void* _temp180; struct Cyc_List_List* _temp182; struct Cyc_Position_Segment*
_temp184; int _temp186; struct Cyc_Position_Segment* _temp188; struct Cyc_List_List*
_temp190; struct Cyc_Absyn_Tqual _temp192; void* _temp194; void* _temp196;
_LL166: if( _temp164 ==( void*) Cyc_Absyn_Carray_mod){ goto _LL167;} else{ goto
_LL168;} _LL168: if(( unsigned int) _temp164 > 1u?*(( int*) _temp164) == Cyc_Absyn_ConstArray_mod:
0){ _LL179: _temp178=(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp164)->f1;
goto _LL169;} else{ goto _LL170;} _LL170: if(( unsigned int) _temp164 > 1u?*((
int*) _temp164) == Cyc_Absyn_Function_mod: 0){ _LL181: _temp180=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp164)->f1; goto _LL171;} else{ goto
_LL172;} _LL172: if(( unsigned int) _temp164 > 1u?*(( int*) _temp164) == Cyc_Absyn_Attributes_mod:
0){ _LL185: _temp184=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp164)->f1;
goto _LL183; _LL183: _temp182=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp164)->f2; goto _LL173;} else{ goto _LL174;} _LL174: if(( unsigned int)
_temp164 > 1u?*(( int*) _temp164) == Cyc_Absyn_TypeParams_mod: 0){ _LL191:
_temp190=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp164)->f1; goto _LL189;
_LL189: _temp188=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp164)->f2; goto
_LL187; _LL187: _temp186=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp164)->f3;
goto _LL175;} else{ goto _LL176;} _LL176: if(( unsigned int) _temp164 > 1u?*((
int*) _temp164) == Cyc_Absyn_Pointer_mod: 0){ _LL197: _temp196=( void*)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp164)->f1; goto _LL195; _LL195: _temp194=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp164)->f2; goto _LL193;
_LL193: _temp192=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp164)->f3; goto
_LL177;} else{ goto _LL165;} _LL167: if( Cyc_Absynpp_next_is_pointer((( struct
Cyc_List_List*) _check_null( tms))->tl)){ rest= p_rest;} return({ struct Cyc_PP_Doc*(*
_temp198)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp201= rest;
struct Cyc_PP_Doc* _temp202= Cyc_PP_text( _tag_arr("[]", sizeof( unsigned char),
3u)); struct Cyc_PP_Doc* _temp199[ 2u]={ _temp201, _temp202}; struct _tagged_arr
_temp200={( void*) _temp199,( void*) _temp199,( void*)( _temp199 + 2u)};
_temp198( _temp200);}); _LL169: if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*)
_check_null( tms))->tl)){ rest= p_rest;} return({ struct Cyc_PP_Doc*(* _temp203)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp206= rest; struct Cyc_PP_Doc*
_temp207= Cyc_PP_text( _tag_arr("[", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp208= Cyc_Absynpp_exp2doc( _temp178); struct Cyc_PP_Doc* _temp209= Cyc_PP_text(
_tag_arr("]", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp204[ 4u]={
_temp206, _temp207, _temp208, _temp209}; struct _tagged_arr _temp205={( void*)
_temp204,( void*) _temp204,( void*)( _temp204 + 4u)}; _temp203( _temp205);});
_LL171: if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*) _check_null(
tms))->tl)){ rest= p_rest;}{ void* _temp210= _temp180; struct Cyc_List_List*
_temp216; struct Cyc_Core_Opt* _temp218; struct Cyc_Absyn_VarargInfo* _temp220;
int _temp222; struct Cyc_List_List* _temp224; struct Cyc_Position_Segment*
_temp226; struct Cyc_List_List* _temp228; _LL212: if(*(( int*) _temp210) == Cyc_Absyn_WithTypes){
_LL225: _temp224=(( struct Cyc_Absyn_WithTypes_struct*) _temp210)->f1; goto
_LL223; _LL223: _temp222=(( struct Cyc_Absyn_WithTypes_struct*) _temp210)->f2;
goto _LL221; _LL221: _temp220=(( struct Cyc_Absyn_WithTypes_struct*) _temp210)->f3;
goto _LL219; _LL219: _temp218=(( struct Cyc_Absyn_WithTypes_struct*) _temp210)->f4;
goto _LL217; _LL217: _temp216=(( struct Cyc_Absyn_WithTypes_struct*) _temp210)->f5;
goto _LL213;} else{ goto _LL214;} _LL214: if(*(( int*) _temp210) == Cyc_Absyn_NoTypes){
_LL229: _temp228=(( struct Cyc_Absyn_NoTypes_struct*) _temp210)->f1; goto _LL227;
_LL227: _temp226=(( struct Cyc_Absyn_NoTypes_struct*) _temp210)->f2; goto _LL215;}
else{ goto _LL211;} _LL213: return({ struct Cyc_PP_Doc*(* _temp230)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp233= rest; struct Cyc_PP_Doc*
_temp234= Cyc_Absynpp_funargs2doc( _temp224, _temp222, _temp220, _temp218,
_temp216); struct Cyc_PP_Doc* _temp231[ 2u]={ _temp233, _temp234}; struct
_tagged_arr _temp232={( void*) _temp231,( void*) _temp231,( void*)( _temp231 + 2u)};
_temp230( _temp232);}); _LL215: return({ struct Cyc_PP_Doc*(* _temp235)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp238= rest; struct Cyc_PP_Doc*
_temp239= Cyc_PP_group( _tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tagged_arr*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_PP_textptr, _temp228)); struct Cyc_PP_Doc* _temp236[ 2u]={
_temp238, _temp239}; struct _tagged_arr _temp237={( void*) _temp236,( void*)
_temp236,( void*)( _temp236 + 2u)}; _temp235( _temp237);}); _LL211:;} _LL173:
if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*) _check_null( tms))->tl)){
rest= p_rest;} return({ struct Cyc_PP_Doc*(* _temp240)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp243= rest; struct Cyc_PP_Doc* _temp244= Cyc_Absynpp_atts2doc(
_temp182); struct Cyc_PP_Doc* _temp241[ 2u]={ _temp243, _temp244}; struct
_tagged_arr _temp242={( void*) _temp241,( void*) _temp241,( void*)( _temp241 + 2u)};
_temp240( _temp242);}); _LL175: if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*)
_check_null( tms))->tl)){ rest= p_rest;} if( _temp186){ return({ struct Cyc_PP_Doc*(*
_temp245)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp248= rest;
struct Cyc_PP_Doc* _temp249= Cyc_Absynpp_ktvars2doc( _temp190); struct Cyc_PP_Doc*
_temp246[ 2u]={ _temp248, _temp249}; struct _tagged_arr _temp247={( void*)
_temp246,( void*) _temp246,( void*)( _temp246 + 2u)}; _temp245( _temp247);});}
else{ return({ struct Cyc_PP_Doc*(* _temp250)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp253= rest; struct Cyc_PP_Doc* _temp254= Cyc_Absynpp_tvars2doc(
_temp190); struct Cyc_PP_Doc* _temp251[ 2u]={ _temp253, _temp254}; struct
_tagged_arr _temp252={( void*) _temp251,( void*) _temp251,( void*)( _temp251 + 2u)};
_temp250( _temp252);});} _LL177: { struct Cyc_PP_Doc* ptr;{ void* _temp255=
_temp196; struct Cyc_Absyn_Exp* _temp263; struct Cyc_Absyn_Exp* _temp265; _LL257:
if(( unsigned int) _temp255 > 1u?*(( int*) _temp255) == Cyc_Absyn_Nullable_ps: 0){
_LL264: _temp263=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp255)->f1; goto
_LL258;} else{ goto _LL259;} _LL259: if(( unsigned int) _temp255 > 1u?*(( int*)
_temp255) == Cyc_Absyn_NonNullable_ps: 0){ _LL266: _temp265=(( struct Cyc_Absyn_NonNullable_ps_struct*)
_temp255)->f1; goto _LL260;} else{ goto _LL261;} _LL261: if( _temp255 ==( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL262;} else{ goto _LL256;} _LL258: if( Cyc_Evexp_eval_const_uint_exp(
_temp263) == 1){ ptr= Cyc_PP_text( _tag_arr("*", sizeof( unsigned char), 2u));}
else{ ptr=({ struct Cyc_PP_Doc*(* _temp267)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp270= Cyc_PP_text( _tag_arr("*", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp271= Cyc_PP_text( _tag_arr("{", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp272= Cyc_Absynpp_exp2doc( _temp263);
struct Cyc_PP_Doc* _temp273= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp268[ 4u]={ _temp270, _temp271, _temp272, _temp273};
struct _tagged_arr _temp269={( void*) _temp268,( void*) _temp268,( void*)(
_temp268 + 4u)}; _temp267( _temp269);});} goto _LL256; _LL260: if( Cyc_Evexp_eval_const_uint_exp(
_temp265) == 1){ ptr= Cyc_PP_text( _tag_arr("@", sizeof( unsigned char), 2u));}
else{ ptr=({ struct Cyc_PP_Doc*(* _temp274)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp277= Cyc_PP_text( _tag_arr("@", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp278= Cyc_PP_text( _tag_arr("{", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp279= Cyc_Absynpp_exp2doc( _temp265);
struct Cyc_PP_Doc* _temp280= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp275[ 4u]={ _temp277, _temp278, _temp279, _temp280};
struct _tagged_arr _temp276={( void*) _temp275,( void*) _temp275,( void*)(
_temp275 + 4u)}; _temp274( _temp276);});} goto _LL256; _LL262: ptr= Cyc_PP_text(
_tag_arr("?", sizeof( unsigned char), 2u)); goto _LL256; _LL256:;}{ void*
_temp281= Cyc_Tcutil_compress( _temp194); _LL283: if( _temp281 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL284;} else{ goto _LL285;} _LL285: goto _LL286; _LL284: return({ struct
Cyc_PP_Doc*(* _temp287)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp290= ptr; struct Cyc_PP_Doc* _temp291= rest; struct Cyc_PP_Doc* _temp288[ 2u]={
_temp290, _temp291}; struct _tagged_arr _temp289={( void*) _temp288,( void*)
_temp288,( void*)( _temp288 + 2u)}; _temp287( _temp289);}); _LL286: return({
struct Cyc_PP_Doc*(* _temp292)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp295= ptr; struct Cyc_PP_Doc* _temp296= Cyc_Absynpp_ntyp2doc( _temp194);
struct Cyc_PP_Doc* _temp297= Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp298= rest; struct Cyc_PP_Doc* _temp293[ 4u]={
_temp295, _temp296, _temp297, _temp298}; struct _tagged_arr _temp294={( void*)
_temp293,( void*) _temp293,( void*)( _temp293 + 4u)}; _temp292( _temp294);});
_LL282:;}} _LL165:;}} struct Cyc_PP_Doc* Cyc_Absynpp_rgn2doc( void* t){ void*
_temp305= Cyc_Tcutil_compress( t); _LL307: if( _temp305 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL308;} else{ goto _LL309;} _LL309: goto _LL310; _LL308: return Cyc_PP_text(
_tag_arr("`H", sizeof( unsigned char), 3u)); _LL310: return Cyc_Absynpp_ntyp2doc(
t); _LL306:;} static void Cyc_Absynpp_effects2docs( struct Cyc_List_List**
regions, struct Cyc_List_List** effects, void* t){ void* _temp311= Cyc_Tcutil_compress(
t); void* _temp319; struct Cyc_List_List* _temp321; _LL313: if(( unsigned int)
_temp311 > 4u?*(( int*) _temp311) == Cyc_Absyn_AccessEff: 0){ _LL320: _temp319=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp311)->f1; goto _LL314;} else{
goto _LL315;} _LL315: if(( unsigned int) _temp311 > 4u?*(( int*) _temp311) ==
Cyc_Absyn_JoinEff: 0){ _LL322: _temp321=(( struct Cyc_Absyn_JoinEff_struct*)
_temp311)->f1; goto _LL316;} else{ goto _LL317;} _LL317: goto _LL318; _LL314:*
regions=({ struct Cyc_List_List* _temp323=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp323->hd=( void*) Cyc_Absynpp_rgn2doc(
_temp319); _temp323->tl=* regions; _temp323;}); goto _LL312; _LL316: for( 0;
_temp321 != 0; _temp321=(( struct Cyc_List_List*) _check_null( _temp321))->tl){
Cyc_Absynpp_effects2docs( regions, effects,( void*)(( struct Cyc_List_List*)
_check_null( _temp321))->hd);} goto _LL312; _LL318:* effects=({ struct Cyc_List_List*
_temp324=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp324->hd=( void*) Cyc_Absynpp_ntyp2doc( t); _temp324->tl=* effects; _temp324;});
goto _LL312; _LL312:;} struct Cyc_PP_Doc* Cyc_Absynpp_eff2doc( void* t){ struct
Cyc_List_List* regions= 0; struct Cyc_List_List* effects= 0; Cyc_Absynpp_effects2docs(&
regions,& effects, t); regions=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( regions); effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( effects); if( regions == 0? effects != 0: 0){ return Cyc_PP_group(
_tag_arr("", sizeof( unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr("+", sizeof( unsigned char), 2u), effects);} else{ struct Cyc_PP_Doc*
_temp325= Cyc_PP_group( _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u), regions);
goto _LL326; _LL326: return Cyc_PP_group( _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr("", sizeof( unsigned char), 1u), _tag_arr("+", sizeof( unsigned char),
2u),(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( effects,({ struct Cyc_List_List* _temp327=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp327->hd=( void*) _temp325;
_temp327->tl= 0; _temp327;})));}} struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc( void*
t){ struct Cyc_PP_Doc* s;{ void* _temp328= t; struct Cyc_Absyn_Exp* _temp374;
struct Cyc_Absyn_Tqual _temp376; void* _temp378; struct Cyc_Absyn_FnInfo
_temp380; struct Cyc_Absyn_PtrInfo _temp382; struct Cyc_Core_Opt* _temp384; int
_temp386; struct Cyc_Core_Opt* _temp388; struct Cyc_Core_Opt* _temp390; struct
Cyc_Absyn_Tvar* _temp392; struct Cyc_Absyn_TunionInfo _temp394; void* _temp396;
struct Cyc_List_List* _temp398; void* _temp400; struct Cyc_Absyn_TunionFieldInfo
_temp402; struct Cyc_List_List* _temp404; void* _temp406; void* _temp408; void*
_temp410; struct Cyc_List_List* _temp412; struct Cyc_Absyn_Structdecl** _temp414;
struct Cyc_List_List* _temp416; struct _tuple0* _temp418; struct Cyc_Absyn_Uniondecl**
_temp420; struct Cyc_List_List* _temp422; struct _tuple0* _temp424; struct Cyc_List_List*
_temp426; struct Cyc_List_List* _temp428; struct Cyc_Absyn_Enumdecl* _temp430;
struct _tuple0* _temp432; struct Cyc_Core_Opt* _temp434; struct Cyc_List_List*
_temp436; struct _tuple0* _temp438; void* _temp440; void* _temp442; struct Cyc_List_List*
_temp444; _LL330: if(( unsigned int) _temp328 > 4u?*(( int*) _temp328) == Cyc_Absyn_ArrayType:
0){ _LL379: _temp378=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp328)->f1;
goto _LL377; _LL377: _temp376=(( struct Cyc_Absyn_ArrayType_struct*) _temp328)->f2;
goto _LL375; _LL375: _temp374=(( struct Cyc_Absyn_ArrayType_struct*) _temp328)->f3;
goto _LL331;} else{ goto _LL332;} _LL332: if(( unsigned int) _temp328 > 4u?*((
int*) _temp328) == Cyc_Absyn_FnType: 0){ _LL381: _temp380=(( struct Cyc_Absyn_FnType_struct*)
_temp328)->f1; goto _LL333;} else{ goto _LL334;} _LL334: if(( unsigned int)
_temp328 > 4u?*(( int*) _temp328) == Cyc_Absyn_PointerType: 0){ _LL383: _temp382=((
struct Cyc_Absyn_PointerType_struct*) _temp328)->f1; goto _LL335;} else{ goto
_LL336;} _LL336: if( _temp328 ==( void*) Cyc_Absyn_VoidType){ goto _LL337;}
else{ goto _LL338;} _LL338: if(( unsigned int) _temp328 > 4u?*(( int*) _temp328)
== Cyc_Absyn_Evar: 0){ _LL391: _temp390=(( struct Cyc_Absyn_Evar_struct*)
_temp328)->f1; goto _LL389; _LL389: _temp388=(( struct Cyc_Absyn_Evar_struct*)
_temp328)->f2; goto _LL387; _LL387: _temp386=(( struct Cyc_Absyn_Evar_struct*)
_temp328)->f3; goto _LL385; _LL385: _temp384=(( struct Cyc_Absyn_Evar_struct*)
_temp328)->f4; goto _LL339;} else{ goto _LL340;} _LL340: if(( unsigned int)
_temp328 > 4u?*(( int*) _temp328) == Cyc_Absyn_VarType: 0){ _LL393: _temp392=((
struct Cyc_Absyn_VarType_struct*) _temp328)->f1; goto _LL341;} else{ goto _LL342;}
_LL342: if(( unsigned int) _temp328 > 4u?*(( int*) _temp328) == Cyc_Absyn_TunionType:
0){ _LL395: _temp394=(( struct Cyc_Absyn_TunionType_struct*) _temp328)->f1;
_LL401: _temp400=( void*) _temp394.tunion_info; goto _LL399; _LL399: _temp398=
_temp394.targs; goto _LL397; _LL397: _temp396=( void*) _temp394.rgn; goto _LL343;}
else{ goto _LL344;} _LL344: if(( unsigned int) _temp328 > 4u?*(( int*) _temp328)
== Cyc_Absyn_TunionFieldType: 0){ _LL403: _temp402=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp328)->f1; _LL407: _temp406=( void*) _temp402.field_info; goto _LL405;
_LL405: _temp404= _temp402.targs; goto _LL345;} else{ goto _LL346;} _LL346: if((
unsigned int) _temp328 > 4u?*(( int*) _temp328) == Cyc_Absyn_IntType: 0){ _LL411:
_temp410=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp328)->f1; goto _LL409;
_LL409: _temp408=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp328)->f2;
goto _LL347;} else{ goto _LL348;} _LL348: if( _temp328 ==( void*) Cyc_Absyn_FloatType){
goto _LL349;} else{ goto _LL350;} _LL350: if( _temp328 ==( void*) Cyc_Absyn_DoubleType){
goto _LL351;} else{ goto _LL352;} _LL352: if(( unsigned int) _temp328 > 4u?*((
int*) _temp328) == Cyc_Absyn_TupleType: 0){ _LL413: _temp412=(( struct Cyc_Absyn_TupleType_struct*)
_temp328)->f1; goto _LL353;} else{ goto _LL354;} _LL354: if(( unsigned int)
_temp328 > 4u?*(( int*) _temp328) == Cyc_Absyn_StructType: 0){ _LL419: _temp418=((
struct Cyc_Absyn_StructType_struct*) _temp328)->f1; goto _LL417; _LL417:
_temp416=(( struct Cyc_Absyn_StructType_struct*) _temp328)->f2; goto _LL415;
_LL415: _temp414=(( struct Cyc_Absyn_StructType_struct*) _temp328)->f3; goto
_LL355;} else{ goto _LL356;} _LL356: if(( unsigned int) _temp328 > 4u?*(( int*)
_temp328) == Cyc_Absyn_UnionType: 0){ _LL425: _temp424=(( struct Cyc_Absyn_UnionType_struct*)
_temp328)->f1; goto _LL423; _LL423: _temp422=(( struct Cyc_Absyn_UnionType_struct*)
_temp328)->f2; goto _LL421; _LL421: _temp420=(( struct Cyc_Absyn_UnionType_struct*)
_temp328)->f3; goto _LL357;} else{ goto _LL358;} _LL358: if(( unsigned int)
_temp328 > 4u?*(( int*) _temp328) == Cyc_Absyn_AnonStructType: 0){ _LL427:
_temp426=(( struct Cyc_Absyn_AnonStructType_struct*) _temp328)->f1; goto _LL359;}
else{ goto _LL360;} _LL360: if(( unsigned int) _temp328 > 4u?*(( int*) _temp328)
== Cyc_Absyn_AnonUnionType: 0){ _LL429: _temp428=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp328)->f1; goto _LL361;} else{ goto _LL362;} _LL362: if(( unsigned int)
_temp328 > 4u?*(( int*) _temp328) == Cyc_Absyn_EnumType: 0){ _LL433: _temp432=((
struct Cyc_Absyn_EnumType_struct*) _temp328)->f1; goto _LL431; _LL431: _temp430=((
struct Cyc_Absyn_EnumType_struct*) _temp328)->f2; goto _LL363;} else{ goto
_LL364;} _LL364: if(( unsigned int) _temp328 > 4u?*(( int*) _temp328) == Cyc_Absyn_TypedefType:
0){ _LL439: _temp438=(( struct Cyc_Absyn_TypedefType_struct*) _temp328)->f1;
goto _LL437; _LL437: _temp436=(( struct Cyc_Absyn_TypedefType_struct*) _temp328)->f2;
goto _LL435; _LL435: _temp434=(( struct Cyc_Absyn_TypedefType_struct*) _temp328)->f3;
goto _LL365;} else{ goto _LL366;} _LL366: if(( unsigned int) _temp328 > 4u?*((
int*) _temp328) == Cyc_Absyn_RgnHandleType: 0){ _LL441: _temp440=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp328)->f1; goto _LL367;} else{ goto
_LL368;} _LL368: if( _temp328 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL369;} else{
goto _LL370;} _LL370: if(( unsigned int) _temp328 > 4u?*(( int*) _temp328) ==
Cyc_Absyn_AccessEff: 0){ _LL443: _temp442=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp328)->f1; goto _LL371;} else{ goto _LL372;} _LL372: if(( unsigned int)
_temp328 > 4u?*(( int*) _temp328) == Cyc_Absyn_JoinEff: 0){ _LL445: _temp444=((
struct Cyc_Absyn_JoinEff_struct*) _temp328)->f1; goto _LL373;} else{ goto _LL329;}
_LL331: return Cyc_PP_text( _tag_arr("[[[array]]]", sizeof( unsigned char), 12u));
_LL333: return Cyc_PP_nil_doc(); _LL335: return Cyc_PP_nil_doc(); _LL337: s= Cyc_PP_text(
_tag_arr("void", sizeof( unsigned char), 5u)); goto _LL329; _LL339: if( _temp388
!= 0){ return Cyc_Absynpp_ntyp2doc(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp388))->v);} else{ s=({ struct Cyc_PP_Doc*(* _temp446)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp449= Cyc_PP_text( _tag_arr("%", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp450= Cyc_PP_text( _tag_arr("(",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp451= Cyc_PP_text(( struct
_tagged_arr) xprintf("%d", _temp386)); struct Cyc_PP_Doc* _temp452=( ! Cyc_Absynpp_print_full_evars?
1: _temp384 == 0)? Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_tvars2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp384))->v);
struct Cyc_PP_Doc* _temp453= Cyc_PP_text( _tag_arr(")::", sizeof( unsigned char),
4u)); struct Cyc_PP_Doc* _temp454= _temp390 == 0? Cyc_PP_text( _tag_arr("?",
sizeof( unsigned char), 2u)): Cyc_Absynpp_kind2doc(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp390))->v); struct Cyc_PP_Doc* _temp447[ 6u]={ _temp449,
_temp450, _temp451, _temp452, _temp453, _temp454}; struct _tagged_arr _temp448={(
void*) _temp447,( void*) _temp447,( void*)( _temp447 + 6u)}; _temp446( _temp448);});}
goto _LL329; _LL341: s= Cyc_PP_textptr( _temp392->name); if( Cyc_Absynpp_print_all_kinds){
s=({ struct Cyc_PP_Doc*(* _temp455)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp458= s; struct Cyc_PP_Doc* _temp459= Cyc_PP_text( _tag_arr("::", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp460= Cyc_Absynpp_ckind2doc(
_temp392->kind); struct Cyc_PP_Doc* _temp456[ 3u]={ _temp458, _temp459, _temp460};
struct _tagged_arr _temp457={( void*) _temp456,( void*) _temp456,( void*)(
_temp456 + 3u)}; _temp455( _temp457);});} if( Cyc_Absynpp_rewrite_temp_tvars?
Cyc_Tcutil_is_temp_tvar( _temp392): 0){ s=({ struct Cyc_PP_Doc*(* _temp461)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp464= Cyc_PP_text(
_tag_arr("_ /* ", sizeof( unsigned char), 6u)); struct Cyc_PP_Doc* _temp465= s;
struct Cyc_PP_Doc* _temp466= Cyc_PP_text( _tag_arr(" */", sizeof( unsigned char),
4u)); struct Cyc_PP_Doc* _temp462[ 3u]={ _temp464, _temp465, _temp466}; struct
_tagged_arr _temp463={( void*) _temp462,( void*) _temp462,( void*)( _temp462 + 3u)};
_temp461( _temp463);});} goto _LL329; _LL343:{ void* _temp467= _temp400; struct
Cyc_Absyn_UnknownTunionInfo _temp473; int _temp475; struct _tuple0* _temp477;
struct Cyc_Absyn_Tuniondecl* _temp479; struct Cyc_Absyn_Tuniondecl _temp481; int
_temp482; struct Cyc_Core_Opt* _temp484; struct Cyc_List_List* _temp486; struct
_tuple0* _temp488; void* _temp490; _LL469: if(*(( int*) _temp467) == Cyc_Absyn_UnknownTunion){
_LL474: _temp473=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp467)->f1;
_LL478: _temp477= _temp473.name; goto _LL476; _LL476: _temp475= _temp473.is_xtunion;
goto _LL470;} else{ goto _LL471;} _LL471: if(*(( int*) _temp467) == Cyc_Absyn_KnownTunion){
_LL480: _temp479=(( struct Cyc_Absyn_KnownTunion_struct*) _temp467)->f1;
_temp481=* _temp479; _LL491: _temp490=( void*) _temp481.sc; goto _LL489; _LL489:
_temp488= _temp481.name; goto _LL487; _LL487: _temp486= _temp481.tvs; goto
_LL485; _LL485: _temp484= _temp481.fields; goto _LL483; _LL483: _temp482=
_temp481.is_xtunion; goto _LL472;} else{ goto _LL468;} _LL470: _temp488=
_temp477; _temp482= _temp475; goto _LL472; _LL472: { struct Cyc_PP_Doc* _temp492=
Cyc_PP_text( _temp482? _tag_arr("xtunion ", sizeof( unsigned char), 9u):
_tag_arr("tunion ", sizeof( unsigned char), 8u)); goto _LL493; _LL493:{ void*
_temp494= Cyc_Tcutil_compress( _temp396); _LL496: if( _temp494 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL497;} else{ goto _LL498;} _LL498: goto _LL499; _LL497: s=({ struct Cyc_PP_Doc*(*
_temp500)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp503=
_temp492; struct Cyc_PP_Doc* _temp504= Cyc_Absynpp_qvar2doc( _temp488); struct
Cyc_PP_Doc* _temp505= Cyc_Absynpp_tps2doc( _temp398); struct Cyc_PP_Doc*
_temp501[ 3u]={ _temp503, _temp504, _temp505}; struct _tagged_arr _temp502={(
void*) _temp501,( void*) _temp501,( void*)( _temp501 + 3u)}; _temp500( _temp502);});
goto _LL495; _LL499: s=({ struct Cyc_PP_Doc*(* _temp506)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp509= _temp492; struct Cyc_PP_Doc* _temp510=
Cyc_Absynpp_typ2doc( _temp396); struct Cyc_PP_Doc* _temp511= Cyc_PP_text(
_tag_arr(" ", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp512= Cyc_Absynpp_qvar2doc(
_temp488); struct Cyc_PP_Doc* _temp513= Cyc_Absynpp_tps2doc( _temp398); struct
Cyc_PP_Doc* _temp507[ 5u]={ _temp509, _temp510, _temp511, _temp512, _temp513};
struct _tagged_arr _temp508={( void*) _temp507,( void*) _temp507,( void*)(
_temp507 + 5u)}; _temp506( _temp508);}); goto _LL495; _LL495:;} goto _LL468;}
_LL468:;} goto _LL329; _LL345:{ void* _temp514= _temp406; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp520; int _temp522; struct _tuple0* _temp524; struct _tuple0* _temp526;
struct Cyc_Absyn_Tunionfield* _temp528; struct Cyc_Absyn_Tunionfield _temp530;
void* _temp531; struct Cyc_Position_Segment* _temp533; struct Cyc_List_List*
_temp535; struct Cyc_List_List* _temp537; struct _tuple0* _temp539; struct Cyc_Absyn_Tuniondecl*
_temp541; struct Cyc_Absyn_Tuniondecl _temp543; int _temp544; struct Cyc_Core_Opt*
_temp546; struct Cyc_List_List* _temp548; struct _tuple0* _temp550; void*
_temp552; _LL516: if(*(( int*) _temp514) == Cyc_Absyn_UnknownTunionfield){
_LL521: _temp520=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp514)->f1;
_LL527: _temp526= _temp520.tunion_name; goto _LL525; _LL525: _temp524= _temp520.field_name;
goto _LL523; _LL523: _temp522= _temp520.is_xtunion; goto _LL517;} else{ goto
_LL518;} _LL518: if(*(( int*) _temp514) == Cyc_Absyn_KnownTunionfield){ _LL542:
_temp541=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp514)->f1; _temp543=*
_temp541; _LL553: _temp552=( void*) _temp543.sc; goto _LL551; _LL551: _temp550=
_temp543.name; goto _LL549; _LL549: _temp548= _temp543.tvs; goto _LL547; _LL547:
_temp546= _temp543.fields; goto _LL545; _LL545: _temp544= _temp543.is_xtunion;
goto _LL529; _LL529: _temp528=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp514)->f2; _temp530=* _temp528; _LL540: _temp539= _temp530.name; goto _LL538;
_LL538: _temp537= _temp530.tvs; goto _LL536; _LL536: _temp535= _temp530.typs;
goto _LL534; _LL534: _temp533= _temp530.loc; goto _LL532; _LL532: _temp531=(
void*) _temp530.sc; goto _LL519;} else{ goto _LL515;} _LL517: _temp550= _temp526;
_temp544= _temp522; _temp539= _temp524; goto _LL519; _LL519: { struct Cyc_PP_Doc*
_temp554= Cyc_PP_text( _temp544? _tag_arr("xtunion ", sizeof( unsigned char), 9u):
_tag_arr("tunion ", sizeof( unsigned char), 8u)); goto _LL555; _LL555: s=({
struct Cyc_PP_Doc*(* _temp556)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp559= _temp554; struct Cyc_PP_Doc* _temp560= Cyc_Absynpp_qvar2doc( _temp550);
struct Cyc_PP_Doc* _temp561= Cyc_PP_text( _tag_arr(".", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp562= Cyc_Absynpp_qvar2doc( _temp539); struct Cyc_PP_Doc*
_temp557[ 4u]={ _temp559, _temp560, _temp561, _temp562}; struct _tagged_arr
_temp558={( void*) _temp557,( void*) _temp557,( void*)( _temp557 + 4u)};
_temp556( _temp558);}); goto _LL515;} _LL515:;} goto _LL329; _LL347: { struct
_tagged_arr sns; struct _tagged_arr ts;{ void* _temp563= _temp410; _LL565: if(
_temp563 ==( void*) Cyc_Absyn_Signed){ goto _LL566;} else{ goto _LL567;} _LL567:
if( _temp563 ==( void*) Cyc_Absyn_Unsigned){ goto _LL568;} else{ goto _LL564;}
_LL566: sns= _tag_arr("", sizeof( unsigned char), 1u); goto _LL564; _LL568: sns=
_tag_arr("unsigned ", sizeof( unsigned char), 10u); goto _LL564; _LL564:;}{ void*
_temp569= _temp408; _LL571: if( _temp569 ==( void*) Cyc_Absyn_B1){ goto _LL572;}
else{ goto _LL573;} _LL573: if( _temp569 ==( void*) Cyc_Absyn_B2){ goto _LL574;}
else{ goto _LL575;} _LL575: if( _temp569 ==( void*) Cyc_Absyn_B4){ goto _LL576;}
else{ goto _LL577;} _LL577: if( _temp569 ==( void*) Cyc_Absyn_B8){ goto _LL578;}
else{ goto _LL570;} _LL572:{ void* _temp579= _temp410; _LL581: if( _temp579 ==(
void*) Cyc_Absyn_Signed){ goto _LL582;} else{ goto _LL583;} _LL583: if( _temp579
==( void*) Cyc_Absyn_Unsigned){ goto _LL584;} else{ goto _LL580;} _LL582: sns=
_tag_arr("signed ", sizeof( unsigned char), 8u); goto _LL580; _LL584: goto
_LL580; _LL580:;} ts= _tag_arr("char", sizeof( unsigned char), 5u); goto _LL570;
_LL574: ts= _tag_arr("short", sizeof( unsigned char), 6u); goto _LL570; _LL576:
ts= _tag_arr("int", sizeof( unsigned char), 4u); goto _LL570; _LL578: ts= Cyc_Absynpp_to_VC?
_tag_arr("__int64", sizeof( unsigned char), 8u): _tag_arr("long long", sizeof(
unsigned char), 10u); goto _LL570; _LL570:;} s= Cyc_PP_text(( struct _tagged_arr)({
struct _tagged_arr _temp585= sns; struct _tagged_arr _temp586= ts; xprintf("%.*s%.*s",
_get_arr_size( _temp585, 1u), _temp585.curr, _get_arr_size( _temp586, 1u),
_temp586.curr);})); goto _LL329;} _LL349: s= Cyc_PP_text( _tag_arr("float",
sizeof( unsigned char), 6u)); goto _LL329; _LL351: s= Cyc_PP_text( _tag_arr("double",
sizeof( unsigned char), 7u)); goto _LL329; _LL353: s=({ struct Cyc_PP_Doc*(*
_temp587)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp590= Cyc_PP_text(
_tag_arr("$", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp591= Cyc_Absynpp_args2doc(
_temp412); struct Cyc_PP_Doc* _temp588[ 2u]={ _temp590, _temp591}; struct
_tagged_arr _temp589={( void*) _temp588,( void*) _temp588,( void*)( _temp588 + 2u)};
_temp587( _temp589);}); goto _LL329; _LL355: if( _temp418 == 0){ s=({ struct Cyc_PP_Doc*(*
_temp592)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp595= Cyc_PP_text(
_tag_arr("struct ", sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp596=
Cyc_Absynpp_tps2doc( _temp416); struct Cyc_PP_Doc* _temp593[ 2u]={ _temp595,
_temp596}; struct _tagged_arr _temp594={( void*) _temp593,( void*) _temp593,(
void*)( _temp593 + 2u)}; _temp592( _temp594);});} else{ s=({ struct Cyc_PP_Doc*(*
_temp597)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp600= Cyc_PP_text(
_tag_arr("struct ", sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp601=
_temp418 == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*)
_check_null( _temp418)); struct Cyc_PP_Doc* _temp602= Cyc_Absynpp_tps2doc(
_temp416); struct Cyc_PP_Doc* _temp598[ 3u]={ _temp600, _temp601, _temp602};
struct _tagged_arr _temp599={( void*) _temp598,( void*) _temp598,( void*)(
_temp598 + 3u)}; _temp597( _temp599);});} goto _LL329; _LL357: if( _temp424 == 0){
s=({ struct Cyc_PP_Doc*(* _temp603)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp606= Cyc_PP_text( _tag_arr("union ", sizeof( unsigned char), 7u)); struct
Cyc_PP_Doc* _temp607= Cyc_Absynpp_tps2doc( _temp422); struct Cyc_PP_Doc*
_temp604[ 2u]={ _temp606, _temp607}; struct _tagged_arr _temp605={( void*)
_temp604,( void*) _temp604,( void*)( _temp604 + 2u)}; _temp603( _temp605);});}
else{ s=({ struct Cyc_PP_Doc*(* _temp608)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp611= Cyc_PP_text( _tag_arr("union ", sizeof(
unsigned char), 7u)); struct Cyc_PP_Doc* _temp612= _temp424 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc(( struct _tuple0*) _check_null( _temp424)); struct Cyc_PP_Doc*
_temp613= Cyc_Absynpp_tps2doc( _temp422); struct Cyc_PP_Doc* _temp609[ 3u]={
_temp611, _temp612, _temp613}; struct _tagged_arr _temp610={( void*) _temp609,(
void*) _temp609,( void*)( _temp609 + 3u)}; _temp608( _temp610);});} goto _LL329;
_LL359: s=({ struct Cyc_PP_Doc*(* _temp614)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp617= Cyc_PP_text( _tag_arr("struct {", sizeof(
unsigned char), 9u)); struct Cyc_PP_Doc* _temp618= Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc(
_temp426)); struct Cyc_PP_Doc* _temp619= Cyc_PP_text( _tag_arr("}", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp615[ 3u]={ _temp617, _temp618,
_temp619}; struct _tagged_arr _temp616={( void*) _temp615,( void*) _temp615,(
void*)( _temp615 + 3u)}; _temp614( _temp616);}); goto _LL329; _LL361: s=({
struct Cyc_PP_Doc*(* _temp620)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp623= Cyc_PP_text( _tag_arr("union {", sizeof( unsigned char), 8u)); struct
Cyc_PP_Doc* _temp624= Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc( _temp428));
struct Cyc_PP_Doc* _temp625= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp621[ 3u]={ _temp623, _temp624, _temp625}; struct
_tagged_arr _temp622={( void*) _temp621,( void*) _temp621,( void*)( _temp621 + 3u)};
_temp620( _temp622);}); goto _LL329; _LL363: s=({ struct Cyc_PP_Doc*(* _temp626)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp629= Cyc_PP_text(
_tag_arr("enum ", sizeof( unsigned char), 6u)); struct Cyc_PP_Doc* _temp630= Cyc_Absynpp_qvar2doc(
_temp432); struct Cyc_PP_Doc* _temp627[ 2u]={ _temp629, _temp630}; struct
_tagged_arr _temp628={( void*) _temp627,( void*) _temp627,( void*)( _temp627 + 2u)};
_temp626( _temp628);}); goto _LL329; _LL365: s=({ struct Cyc_PP_Doc*(* _temp631)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp634= Cyc_Absynpp_qvar2doc(
_temp438); struct Cyc_PP_Doc* _temp635= Cyc_Absynpp_tps2doc( _temp436); struct
Cyc_PP_Doc* _temp632[ 2u]={ _temp634, _temp635}; struct _tagged_arr _temp633={(
void*) _temp632,( void*) _temp632,( void*)( _temp632 + 2u)}; _temp631( _temp633);});
goto _LL329; _LL367: s=({ struct Cyc_PP_Doc*(* _temp636)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp639= Cyc_PP_text( _tag_arr("region_t<",
sizeof( unsigned char), 10u)); struct Cyc_PP_Doc* _temp640= Cyc_Absynpp_rgn2doc(
_temp440); struct Cyc_PP_Doc* _temp641= Cyc_PP_text( _tag_arr(">", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp637[ 3u]={ _temp639, _temp640,
_temp641}; struct _tagged_arr _temp638={( void*) _temp637,( void*) _temp637,(
void*)( _temp637 + 3u)}; _temp636( _temp638);}); goto _LL329; _LL369: s= Cyc_Absynpp_rgn2doc(
t); goto _LL329; _LL371: s= Cyc_Absynpp_eff2doc( t); goto _LL329; _LL373: s= Cyc_Absynpp_eff2doc(
t); goto _LL329; _LL329:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_vo2doc(
struct Cyc_Core_Opt* vo){ return vo == 0? Cyc_PP_nil_doc(): Cyc_PP_text(*((
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( vo))->v));} struct
_tuple8{ void* f1; void* f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_rgn_cmp2doc(
struct _tuple8* cmp){ struct _tuple8 _temp644; void* _temp645; void* _temp647;
struct _tuple8* _temp642= cmp; _temp644=* _temp642; _LL648: _temp647= _temp644.f1;
goto _LL646; _LL646: _temp645= _temp644.f2; goto _LL643; _LL643: return({ struct
Cyc_PP_Doc*(* _temp649)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp652= Cyc_Absynpp_rgn2doc( _temp647); struct Cyc_PP_Doc* _temp653= Cyc_PP_text(
_tag_arr(" < ", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc* _temp654= Cyc_Absynpp_rgn2doc(
_temp645); struct Cyc_PP_Doc* _temp650[ 3u]={ _temp652, _temp653, _temp654};
struct _tagged_arr _temp651={( void*) _temp650,( void*) _temp650,( void*)(
_temp650 + 3u)}; _temp649( _temp651);});} struct Cyc_PP_Doc* Cyc_Absynpp_rgnpo2doc(
struct Cyc_List_List* po){ return Cyc_PP_group( _tag_arr("", sizeof(
unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple8*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_rgn_cmp2doc,
po));} struct Cyc_PP_Doc* Cyc_Absynpp_funarg2doc( struct _tuple1* t){ struct Cyc_Core_Opt*
dopt=(* t).f1 == 0? 0:({ struct Cyc_Core_Opt* _temp655=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp655->v=( void*) Cyc_PP_text(*((
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v));
_temp655;}); return Cyc_Absynpp_tqtd2doc((* t).f2,(* t).f3, dopt);} struct Cyc_PP_Doc*
Cyc_Absynpp_funargs2doc( struct Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo*
cyc_varargs, struct Cyc_Core_Opt* effopt, struct Cyc_List_List* rgn_po){ struct
Cyc_List_List* _temp656=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc,
args); goto _LL657; _LL657: { struct Cyc_PP_Doc* eff_doc; if( c_varargs){
_temp656=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp656,({ struct Cyc_List_List* _temp658=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp658->hd=( void*) Cyc_PP_text(
_tag_arr("...", sizeof( unsigned char), 4u)); _temp658->tl= 0; _temp658;}));}
else{ if( cyc_varargs != 0){ struct Cyc_PP_Doc* _temp667=({ struct Cyc_PP_Doc*(*
_temp659)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp662= Cyc_PP_text(
_tag_arr("...", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc* _temp663= Cyc_Absynpp_typ2doc((
void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->rgn); struct
Cyc_PP_Doc* _temp664=(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->inject?
Cyc_PP_text( _tag_arr(" inject ", sizeof( unsigned char), 9u)): Cyc_PP_text(
_tag_arr(" ", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp665= Cyc_Absynpp_funarg2doc(({
struct _tuple1* _temp666=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp666->f1=(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->name;
_temp666->f2=(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->tq;
_temp666->f3=( void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->type;
_temp666;})); struct Cyc_PP_Doc* _temp660[ 4u]={ _temp662, _temp663, _temp664,
_temp665}; struct _tagged_arr _temp661={( void*) _temp660,( void*) _temp660,(
void*)( _temp660 + 4u)}; _temp659( _temp661);}); goto _LL668; _LL668: _temp656=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp656,({ struct Cyc_List_List* _temp669=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp669->hd=( void*) _temp667; _temp669->tl= 0;
_temp669;}));}}{ struct Cyc_PP_Doc* _temp670= Cyc_PP_group( _tag_arr("", sizeof(
unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u), _tag_arr(",",
sizeof( unsigned char), 2u), _temp656); goto _LL671; _LL671: if( effopt != 0){
_temp670=({ struct Cyc_PP_Doc*(* _temp672)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp675= _temp670; struct Cyc_PP_Doc* _temp676= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp677= Cyc_Absynpp_eff2doc((
void*)(( struct Cyc_Core_Opt*) _check_null( effopt))->v); struct Cyc_PP_Doc*
_temp673[ 3u]={ _temp675, _temp676, _temp677}; struct _tagged_arr _temp674={(
void*) _temp673,( void*) _temp673,( void*)( _temp673 + 3u)}; _temp672( _temp674);});}
if( rgn_po != 0){ _temp670=({ struct Cyc_PP_Doc*(* _temp678)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp681= _temp670; struct Cyc_PP_Doc* _temp682=
Cyc_PP_text( _tag_arr(":", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp683= Cyc_Absynpp_rgnpo2doc( rgn_po); struct Cyc_PP_Doc* _temp679[ 3u]={
_temp681, _temp682, _temp683}; struct _tagged_arr _temp680={( void*) _temp679,(
void*) _temp679,( void*)( _temp679 + 3u)}; _temp678( _temp680);});} return({
struct Cyc_PP_Doc*(* _temp684)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp687= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp688= _temp670; struct Cyc_PP_Doc* _temp689= Cyc_PP_text( _tag_arr(")",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp685[ 3u]={ _temp687,
_temp688, _temp689}; struct _tagged_arr _temp686={( void*) _temp685,( void*)
_temp685,( void*)( _temp685 + 3u)}; _temp684( _temp686);});}}} struct _tuple1*
Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg){ return({ struct _tuple1* _temp690=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp690->f1=({ struct Cyc_Core_Opt*
_temp691=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp691->v=( void*)(* arg).f1; _temp691;}); _temp690->f2=(* arg).f2; _temp690->f3=(*
arg).f3; _temp690;});} struct Cyc_PP_Doc* Cyc_Absynpp_var2doc( struct
_tagged_arr* v){ return Cyc_PP_text(* v);} struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc(
struct _tuple0* q){ struct Cyc_List_List* _temp692= 0; goto _LL693; _LL693: {
int match;{ void* _temp694=(* q).f1; struct Cyc_List_List* _temp702; struct Cyc_List_List*
_temp704; _LL696: if( _temp694 ==( void*) Cyc_Absyn_Loc_n){ goto _LL697;} else{
goto _LL698;} _LL698: if(( unsigned int) _temp694 > 1u?*(( int*) _temp694) ==
Cyc_Absyn_Rel_n: 0){ _LL703: _temp702=(( struct Cyc_Absyn_Rel_n_struct*)
_temp694)->f1; goto _LL699;} else{ goto _LL700;} _LL700: if(( unsigned int)
_temp694 > 1u?*(( int*) _temp694) == Cyc_Absyn_Abs_n: 0){ _LL705: _temp704=((
struct Cyc_Absyn_Abs_n_struct*) _temp694)->f1; goto _LL701;} else{ goto _LL695;}
_LL697: _temp702= 0; goto _LL699; _LL699: match= 0; _temp692= _temp702; goto
_LL695; _LL701: match= Cyc_Absynpp_use_curr_namespace?(( int(*)( int(* cmp)(
struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_List_list_prefix)( Cyc_String_strptrcmp, _temp704, Cyc_Absynpp_curr_namespace):
0; _temp692=( Cyc_Absynpp_qvar_to_Cids? Cyc_Absynpp_add_cyc_prefix: 0)?({ struct
Cyc_List_List* _temp706=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp706->hd=( void*) Cyc_Absynpp_cyc_stringptr; _temp706->tl= _temp704;
_temp706;}): _temp704; goto _LL695; _LL695:;} if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_PP_text(( struct _tagged_arr) Cyc_String_str_sepstr((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp692,({
struct Cyc_List_List* _temp707=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp707->hd=( void*)(* q).f2; _temp707->tl= 0; _temp707;})),
_tag_arr("_", sizeof( unsigned char), 2u)));} else{ if( match){ return Cyc_Absynpp_var2doc((*
q).f2);} else{ return Cyc_PP_text(( struct _tagged_arr) Cyc_String_str_sepstr(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp692,({ struct Cyc_List_List* _temp708=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp708->hd=( void*)(* q).f2; _temp708->tl= 0;
_temp708;})), _tag_arr("::", sizeof( unsigned char), 3u)));}}}} struct Cyc_PP_Doc*
Cyc_Absynpp_typedef_name2doc( struct _tuple0* v){ if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_Absynpp_qvar2doc( v);} if( Cyc_Absynpp_use_curr_namespace){ void*
_temp709=(* v).f1; struct Cyc_List_List* _temp719; struct Cyc_List_List*
_temp721; _LL711: if( _temp709 ==( void*) Cyc_Absyn_Loc_n){ goto _LL712;} else{
goto _LL713;} _LL713: if(( unsigned int) _temp709 > 1u?*(( int*) _temp709) ==
Cyc_Absyn_Rel_n: 0){ _LL720: _temp719=(( struct Cyc_Absyn_Rel_n_struct*)
_temp709)->f1; if( _temp719 == 0){ goto _LL714;} else{ goto _LL715;}} else{ goto
_LL715;} _LL715: if(( unsigned int) _temp709 > 1u?*(( int*) _temp709) == Cyc_Absyn_Abs_n:
0){ _LL722: _temp721=(( struct Cyc_Absyn_Abs_n_struct*) _temp709)->f1; goto
_LL716;} else{ goto _LL717;} _LL717: goto _LL718; _LL712: goto _LL714; _LL714:
return Cyc_Absynpp_var2doc((* v).f2); _LL716: if((( int(*)( int(* cmp)( struct
_tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_List_list_cmp)( Cyc_String_strptrcmp, _temp721, Cyc_Absynpp_curr_namespace)
== 0){ return Cyc_Absynpp_var2doc((* v).f2);} else{ goto _LL718;} _LL718: return({
struct Cyc_PP_Doc*(* _temp723)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp726= Cyc_PP_text( _tag_arr("/* bad namespace : */ ", sizeof( unsigned char),
23u)); struct Cyc_PP_Doc* _temp727= Cyc_Absynpp_qvar2doc( v); struct Cyc_PP_Doc*
_temp724[ 2u]={ _temp726, _temp727}; struct _tagged_arr _temp725={( void*)
_temp724,( void*) _temp724,( void*)( _temp724 + 2u)}; _temp723( _temp725);});
_LL710:;} else{ return Cyc_Absynpp_var2doc((* v).f2);}} struct Cyc_PP_Doc* Cyc_Absynpp_typ2doc(
void* t){ return Cyc_Absynpp_tqtd2doc( Cyc_Absyn_empty_tqual(), t, 0);} int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp* e){ void* _temp728=( void*) e->r; void* _temp802; void*
_temp804; struct _tuple0* _temp806; struct _tuple0* _temp808; struct Cyc_List_List*
_temp810; void* _temp812; struct Cyc_Absyn_Exp* _temp814; struct Cyc_Core_Opt*
_temp816; struct Cyc_Absyn_Exp* _temp818; void* _temp820; struct Cyc_Absyn_Exp*
_temp822; struct Cyc_Absyn_Exp* _temp824; struct Cyc_Absyn_Exp* _temp826; struct
Cyc_Absyn_Exp* _temp828; struct Cyc_Absyn_Exp* _temp830; struct Cyc_Absyn_Exp*
_temp832; struct Cyc_List_List* _temp834; struct Cyc_Absyn_Exp* _temp836; struct
Cyc_Absyn_VarargCallInfo* _temp838; struct Cyc_List_List* _temp840; struct Cyc_Absyn_Exp*
_temp842; struct Cyc_Absyn_Exp* _temp844; struct Cyc_Absyn_Exp* _temp846; struct
Cyc_List_List* _temp848; struct Cyc_Absyn_Exp* _temp850; struct Cyc_Absyn_Exp*
_temp852; void* _temp854; struct Cyc_Absyn_Exp* _temp856; struct Cyc_Absyn_Exp*
_temp858; struct Cyc_Absyn_Exp* _temp860; void* _temp862; struct Cyc_Absyn_Exp*
_temp864; struct _tagged_arr* _temp866; void* _temp868; struct Cyc_Absyn_Exp*
_temp870; struct _tagged_arr* _temp872; struct Cyc_Absyn_Exp* _temp874; struct
_tagged_arr* _temp876; struct Cyc_Absyn_Exp* _temp878; struct Cyc_Absyn_Exp*
_temp880; struct Cyc_Absyn_Exp* _temp882; struct Cyc_List_List* _temp884; struct
Cyc_List_List* _temp886; struct _tuple1* _temp888; struct Cyc_List_List*
_temp890; struct Cyc_Absyn_Exp* _temp892; struct Cyc_Absyn_Exp* _temp894; struct
Cyc_Absyn_Vardecl* _temp896; struct Cyc_Absyn_Structdecl* _temp898; struct Cyc_List_List*
_temp900; struct Cyc_Core_Opt* _temp902; struct _tuple0* _temp904; struct Cyc_List_List*
_temp906; void* _temp908; struct Cyc_Absyn_Tunionfield* _temp910; struct Cyc_Absyn_Tuniondecl*
_temp912; struct Cyc_List_List* _temp914; struct Cyc_Core_Opt* _temp916; struct
Cyc_Core_Opt* _temp918; struct Cyc_Absyn_Enumfield* _temp920; struct Cyc_Absyn_Enumdecl*
_temp922; struct _tuple0* _temp924; void* _temp926; struct Cyc_Absyn_Exp*
_temp928; struct Cyc_List_List* _temp930; struct Cyc_Core_Opt* _temp932; struct
Cyc_Absyn_Stmt* _temp934; struct Cyc_Absyn_Fndecl* _temp936; struct Cyc_Absyn_Exp*
_temp938; _LL730: if(*(( int*) _temp728) == Cyc_Absyn_Const_e){ _LL803: _temp802=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp728)->f1; goto _LL731;} else{
goto _LL732;} _LL732: if(*(( int*) _temp728) == Cyc_Absyn_Var_e){ _LL807:
_temp806=(( struct Cyc_Absyn_Var_e_struct*) _temp728)->f1; goto _LL805; _LL805:
_temp804=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp728)->f2; goto _LL733;}
else{ goto _LL734;} _LL734: if(*(( int*) _temp728) == Cyc_Absyn_UnknownId_e){
_LL809: _temp808=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp728)->f1; goto
_LL735;} else{ goto _LL736;} _LL736: if(*(( int*) _temp728) == Cyc_Absyn_Primop_e){
_LL813: _temp812=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp728)->f1;
goto _LL811; _LL811: _temp810=(( struct Cyc_Absyn_Primop_e_struct*) _temp728)->f2;
goto _LL737;} else{ goto _LL738;} _LL738: if(*(( int*) _temp728) == Cyc_Absyn_AssignOp_e){
_LL819: _temp818=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp728)->f1; goto
_LL817; _LL817: _temp816=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp728)->f2;
goto _LL815; _LL815: _temp814=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp728)->f3;
goto _LL739;} else{ goto _LL740;} _LL740: if(*(( int*) _temp728) == Cyc_Absyn_Increment_e){
_LL823: _temp822=(( struct Cyc_Absyn_Increment_e_struct*) _temp728)->f1; goto
_LL821; _LL821: _temp820=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp728)->f2; goto _LL741;} else{ goto _LL742;} _LL742: if(*(( int*) _temp728)
== Cyc_Absyn_Conditional_e){ _LL829: _temp828=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp728)->f1; goto _LL827; _LL827: _temp826=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp728)->f2; goto _LL825; _LL825: _temp824=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp728)->f3; goto _LL743;} else{ goto _LL744;} _LL744: if(*(( int*) _temp728)
== Cyc_Absyn_SeqExp_e){ _LL833: _temp832=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp728)->f1; goto _LL831; _LL831: _temp830=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp728)->f2; goto _LL745;} else{ goto _LL746;} _LL746: if(*(( int*) _temp728)
== Cyc_Absyn_UnknownCall_e){ _LL837: _temp836=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp728)->f1; goto _LL835; _LL835: _temp834=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp728)->f2; goto _LL747;} else{ goto _LL748;} _LL748: if(*(( int*) _temp728)
== Cyc_Absyn_FnCall_e){ _LL843: _temp842=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp728)->f1; goto _LL841; _LL841: _temp840=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp728)->f2; goto _LL839; _LL839: _temp838=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp728)->f3; goto _LL749;} else{ goto _LL750;} _LL750: if(*(( int*) _temp728)
== Cyc_Absyn_Throw_e){ _LL845: _temp844=(( struct Cyc_Absyn_Throw_e_struct*)
_temp728)->f1; goto _LL751;} else{ goto _LL752;} _LL752: if(*(( int*) _temp728)
== Cyc_Absyn_NoInstantiate_e){ _LL847: _temp846=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp728)->f1; goto _LL753;} else{ goto _LL754;} _LL754: if(*(( int*) _temp728)
== Cyc_Absyn_Instantiate_e){ _LL851: _temp850=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp728)->f1; goto _LL849; _LL849: _temp848=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp728)->f2; goto _LL755;} else{ goto _LL756;} _LL756: if(*(( int*) _temp728)
== Cyc_Absyn_Cast_e){ _LL855: _temp854=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp728)->f1; goto _LL853; _LL853: _temp852=(( struct Cyc_Absyn_Cast_e_struct*)
_temp728)->f2; goto _LL757;} else{ goto _LL758;} _LL758: if(*(( int*) _temp728)
== Cyc_Absyn_New_e){ _LL859: _temp858=(( struct Cyc_Absyn_New_e_struct*)
_temp728)->f1; goto _LL857; _LL857: _temp856=(( struct Cyc_Absyn_New_e_struct*)
_temp728)->f2; goto _LL759;} else{ goto _LL760;} _LL760: if(*(( int*) _temp728)
== Cyc_Absyn_Address_e){ _LL861: _temp860=(( struct Cyc_Absyn_Address_e_struct*)
_temp728)->f1; goto _LL761;} else{ goto _LL762;} _LL762: if(*(( int*) _temp728)
== Cyc_Absyn_Sizeoftyp_e){ _LL863: _temp862=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp728)->f1; goto _LL763;} else{ goto _LL764;} _LL764: if(*(( int*) _temp728)
== Cyc_Absyn_Sizeofexp_e){ _LL865: _temp864=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp728)->f1; goto _LL765;} else{ goto _LL766;} _LL766: if(*(( int*) _temp728)
== Cyc_Absyn_Offsetof_e){ _LL869: _temp868=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp728)->f1; goto _LL867; _LL867: _temp866=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp728)->f2; goto _LL767;} else{ goto _LL768;} _LL768: if(*(( int*) _temp728)
== Cyc_Absyn_Deref_e){ _LL871: _temp870=(( struct Cyc_Absyn_Deref_e_struct*)
_temp728)->f1; goto _LL769;} else{ goto _LL770;} _LL770: if(*(( int*) _temp728)
== Cyc_Absyn_StructMember_e){ _LL875: _temp874=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp728)->f1; goto _LL873; _LL873: _temp872=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp728)->f2; goto _LL771;} else{ goto _LL772;} _LL772: if(*(( int*) _temp728)
== Cyc_Absyn_StructArrow_e){ _LL879: _temp878=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp728)->f1; goto _LL877; _LL877: _temp876=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp728)->f2; goto _LL773;} else{ goto _LL774;} _LL774: if(*(( int*) _temp728)
== Cyc_Absyn_Subscript_e){ _LL883: _temp882=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp728)->f1; goto _LL881; _LL881: _temp880=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp728)->f2; goto _LL775;} else{ goto _LL776;} _LL776: if(*(( int*) _temp728)
== Cyc_Absyn_Tuple_e){ _LL885: _temp884=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp728)->f1; goto _LL777;} else{ goto _LL778;} _LL778: if(*(( int*) _temp728)
== Cyc_Absyn_CompoundLit_e){ _LL889: _temp888=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp728)->f1; goto _LL887; _LL887: _temp886=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp728)->f2; goto _LL779;} else{ goto _LL780;} _LL780: if(*(( int*) _temp728)
== Cyc_Absyn_Array_e){ _LL891: _temp890=(( struct Cyc_Absyn_Array_e_struct*)
_temp728)->f1; goto _LL781;} else{ goto _LL782;} _LL782: if(*(( int*) _temp728)
== Cyc_Absyn_Comprehension_e){ _LL897: _temp896=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp728)->f1; goto _LL895; _LL895: _temp894=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp728)->f2; goto _LL893; _LL893: _temp892=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp728)->f3; goto _LL783;} else{ goto _LL784;} _LL784: if(*(( int*) _temp728)
== Cyc_Absyn_Struct_e){ _LL905: _temp904=(( struct Cyc_Absyn_Struct_e_struct*)
_temp728)->f1; goto _LL903; _LL903: _temp902=(( struct Cyc_Absyn_Struct_e_struct*)
_temp728)->f2; goto _LL901; _LL901: _temp900=(( struct Cyc_Absyn_Struct_e_struct*)
_temp728)->f3; goto _LL899; _LL899: _temp898=(( struct Cyc_Absyn_Struct_e_struct*)
_temp728)->f4; goto _LL785;} else{ goto _LL786;} _LL786: if(*(( int*) _temp728)
== Cyc_Absyn_AnonStruct_e){ _LL909: _temp908=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp728)->f1; goto _LL907; _LL907: _temp906=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp728)->f2; goto _LL787;} else{ goto _LL788;} _LL788: if(*(( int*) _temp728)
== Cyc_Absyn_Tunion_e){ _LL919: _temp918=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp728)->f1; goto _LL917; _LL917: _temp916=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp728)->f2; goto _LL915; _LL915: _temp914=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp728)->f3; goto _LL913; _LL913: _temp912=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp728)->f4; goto _LL911; _LL911: _temp910=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp728)->f5; goto _LL789;} else{ goto _LL790;} _LL790: if(*(( int*) _temp728)
== Cyc_Absyn_Enum_e){ _LL925: _temp924=(( struct Cyc_Absyn_Enum_e_struct*)
_temp728)->f1; goto _LL923; _LL923: _temp922=(( struct Cyc_Absyn_Enum_e_struct*)
_temp728)->f2; goto _LL921; _LL921: _temp920=(( struct Cyc_Absyn_Enum_e_struct*)
_temp728)->f3; goto _LL791;} else{ goto _LL792;} _LL792: if(*(( int*) _temp728)
== Cyc_Absyn_Malloc_e){ _LL929: _temp928=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp728)->f1; goto _LL927; _LL927: _temp926=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp728)->f2; goto _LL793;} else{ goto _LL794;} _LL794: if(*(( int*) _temp728)
== Cyc_Absyn_UnresolvedMem_e){ _LL933: _temp932=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp728)->f1; goto _LL931; _LL931: _temp930=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp728)->f2; goto _LL795;} else{ goto _LL796;} _LL796: if(*(( int*) _temp728)
== Cyc_Absyn_StmtExp_e){ _LL935: _temp934=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp728)->f1; goto _LL797;} else{ goto _LL798;} _LL798: if(*(( int*) _temp728)
== Cyc_Absyn_Codegen_e){ _LL937: _temp936=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp728)->f1; goto _LL799;} else{ goto _LL800;} _LL800: if(*(( int*) _temp728)
== Cyc_Absyn_Fill_e){ _LL939: _temp938=(( struct Cyc_Absyn_Fill_e_struct*)
_temp728)->f1; goto _LL801;} else{ goto _LL729;} _LL731: return 10000; _LL733:
return 10000; _LL735: return 10000; _LL737: { void* _temp940= _temp812; _LL942:
if( _temp940 ==( void*) Cyc_Absyn_Plus){ goto _LL943;} else{ goto _LL944;}
_LL944: if( _temp940 ==( void*) Cyc_Absyn_Times){ goto _LL945;} else{ goto
_LL946;} _LL946: if( _temp940 ==( void*) Cyc_Absyn_Minus){ goto _LL947;} else{
goto _LL948;} _LL948: if( _temp940 ==( void*) Cyc_Absyn_Div){ goto _LL949;}
else{ goto _LL950;} _LL950: if( _temp940 ==( void*) Cyc_Absyn_Mod){ goto _LL951;}
else{ goto _LL952;} _LL952: if( _temp940 ==( void*) Cyc_Absyn_Eq){ goto _LL953;}
else{ goto _LL954;} _LL954: if( _temp940 ==( void*) Cyc_Absyn_Neq){ goto _LL955;}
else{ goto _LL956;} _LL956: if( _temp940 ==( void*) Cyc_Absyn_Gt){ goto _LL957;}
else{ goto _LL958;} _LL958: if( _temp940 ==( void*) Cyc_Absyn_Lt){ goto _LL959;}
else{ goto _LL960;} _LL960: if( _temp940 ==( void*) Cyc_Absyn_Gte){ goto _LL961;}
else{ goto _LL962;} _LL962: if( _temp940 ==( void*) Cyc_Absyn_Lte){ goto _LL963;}
else{ goto _LL964;} _LL964: if( _temp940 ==( void*) Cyc_Absyn_Not){ goto _LL965;}
else{ goto _LL966;} _LL966: if( _temp940 ==( void*) Cyc_Absyn_Bitnot){ goto
_LL967;} else{ goto _LL968;} _LL968: if( _temp940 ==( void*) Cyc_Absyn_Bitand){
goto _LL969;} else{ goto _LL970;} _LL970: if( _temp940 ==( void*) Cyc_Absyn_Bitor){
goto _LL971;} else{ goto _LL972;} _LL972: if( _temp940 ==( void*) Cyc_Absyn_Bitxor){
goto _LL973;} else{ goto _LL974;} _LL974: if( _temp940 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL975;} else{ goto _LL976;} _LL976: if( _temp940 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL977;} else{ goto _LL978;} _LL978: if( _temp940 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL979;} else{ goto _LL980;} _LL980: if( _temp940 ==( void*) Cyc_Absyn_Size){
goto _LL981;} else{ goto _LL982;} _LL982: if( _temp940 ==( void*) Cyc_Absyn_Printf){
goto _LL983;} else{ goto _LL984;} _LL984: if( _temp940 ==( void*) Cyc_Absyn_Fprintf){
goto _LL985;} else{ goto _LL986;} _LL986: if( _temp940 ==( void*) Cyc_Absyn_Xprintf){
goto _LL987;} else{ goto _LL988;} _LL988: if( _temp940 ==( void*) Cyc_Absyn_Scanf){
goto _LL989;} else{ goto _LL990;} _LL990: if( _temp940 ==( void*) Cyc_Absyn_Fscanf){
goto _LL991;} else{ goto _LL992;} _LL992: if( _temp940 ==( void*) Cyc_Absyn_Sscanf){
goto _LL993;} else{ goto _LL941;} _LL943: return 100; _LL945: return 110; _LL947:
return 100; _LL949: return 110; _LL951: return 110; _LL953: return 70; _LL955:
return 70; _LL957: return 80; _LL959: return 80; _LL961: return 80; _LL963:
return 80; _LL965: return 130; _LL967: return 130; _LL969: return 60; _LL971:
return 40; _LL973: return 50; _LL975: return 90; _LL977: return 80; _LL979:
return 80; _LL981: return 140; _LL983: return 130; _LL985: return 130; _LL987:
return 130; _LL989: return 130; _LL991: return 130; _LL993: return 130; _LL941:;}
_LL739: return 20; _LL741: return 130; _LL743: return 30; _LL745: return 10;
_LL747: return 140; _LL749: return 140; _LL751: return 130; _LL753: return Cyc_Absynpp_exp_prec(
_temp846); _LL755: return Cyc_Absynpp_exp_prec( _temp850); _LL757: return 120;
_LL759: return 15; _LL761: return 130; _LL763: return 130; _LL765: return 130;
_LL767: return 130; _LL769: return 130; _LL771: return 140; _LL773: return 140;
_LL775: return 140; _LL777: return 150; _LL779: goto _LL781; _LL781: goto _LL783;
_LL783: goto _LL785; _LL785: goto _LL787; _LL787: goto _LL789; _LL789: goto
_LL791; _LL791: goto _LL793; _LL793: return 140; _LL795: return 140; _LL797:
return 10000; _LL799: return 140; _LL801: return 140; _LL729:;} struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp* e){ return Cyc_Absynpp_exp2doc_prec(
0, e);} struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec( int inprec, struct Cyc_Absyn_Exp*
e){ int myprec= Cyc_Absynpp_exp_prec( e); struct Cyc_PP_Doc* s;{ void* _temp994=(
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
_temp1130; struct _tagged_arr* _temp1132; void* _temp1134; struct Cyc_Absyn_Exp*
_temp1136; struct _tagged_arr* _temp1138; struct Cyc_Absyn_Exp* _temp1140;
struct _tagged_arr* _temp1142; struct Cyc_Absyn_Exp* _temp1144; struct Cyc_Absyn_Exp*
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
_temp994) == Cyc_Absyn_Address_e){ _LL1123: _temp1122=(( struct Cyc_Absyn_Address_e_struct*)
_temp994)->f1; goto _LL1025;} else{ goto _LL1026;} _LL1026: if(*(( int*)
_temp994) == Cyc_Absyn_New_e){ _LL1127: _temp1126=(( struct Cyc_Absyn_New_e_struct*)
_temp994)->f1; goto _LL1125; _LL1125: _temp1124=(( struct Cyc_Absyn_New_e_struct*)
_temp994)->f2; goto _LL1027;} else{ goto _LL1028;} _LL1028: if(*(( int*)
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
_temp994)->f1; goto _LL1067;} else{ goto _LL995;} _LL997: s= Cyc_Absynpp_cnst2doc(
_temp1068); goto _LL995; _LL999: _temp1074= _temp1072; goto _LL1001; _LL1001: s=
Cyc_Absynpp_qvar2doc( _temp1074); goto _LL995; _LL1003: s= Cyc_Absynpp_primapp2doc(
myprec, _temp1078, _temp1076); goto _LL995; _LL1005: s=({ struct Cyc_PP_Doc*(*
_temp1206)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1209= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1084); struct Cyc_PP_Doc* _temp1210= Cyc_PP_text( _tag_arr(" ",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1211= _temp1082 == 0? Cyc_PP_text(
_tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_prim2doc(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1082))->v); struct Cyc_PP_Doc* _temp1212=
Cyc_PP_text( _tag_arr("= ", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc*
_temp1213= Cyc_Absynpp_exp2doc_prec( myprec, _temp1080); struct Cyc_PP_Doc*
_temp1207[ 5u]={ _temp1209, _temp1210, _temp1211, _temp1212, _temp1213}; struct
_tagged_arr _temp1208={( void*) _temp1207,( void*) _temp1207,( void*)( _temp1207
+ 5u)}; _temp1206( _temp1208);}); goto _LL995; _LL1007: { struct Cyc_PP_Doc*
_temp1214= Cyc_Absynpp_exp2doc_prec( myprec, _temp1088); goto _LL1215; _LL1215:{
void* _temp1216= _temp1086; _LL1218: if( _temp1216 ==( void*) Cyc_Absyn_PreInc){
goto _LL1219;} else{ goto _LL1220;} _LL1220: if( _temp1216 ==( void*) Cyc_Absyn_PreDec){
goto _LL1221;} else{ goto _LL1222;} _LL1222: if( _temp1216 ==( void*) Cyc_Absyn_PostInc){
goto _LL1223;} else{ goto _LL1224;} _LL1224: if( _temp1216 ==( void*) Cyc_Absyn_PostDec){
goto _LL1225;} else{ goto _LL1217;} _LL1219: s=({ struct Cyc_PP_Doc*(* _temp1226)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1229= Cyc_PP_text(
_tag_arr("++", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1230=
_temp1214; struct Cyc_PP_Doc* _temp1227[ 2u]={ _temp1229, _temp1230}; struct
_tagged_arr _temp1228={( void*) _temp1227,( void*) _temp1227,( void*)( _temp1227
+ 2u)}; _temp1226( _temp1228);}); goto _LL1217; _LL1221: s=({ struct Cyc_PP_Doc*(*
_temp1231)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1234= Cyc_PP_text(
_tag_arr("--", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1235=
_temp1214; struct Cyc_PP_Doc* _temp1232[ 2u]={ _temp1234, _temp1235}; struct
_tagged_arr _temp1233={( void*) _temp1232,( void*) _temp1232,( void*)( _temp1232
+ 2u)}; _temp1231( _temp1233);}); goto _LL1217; _LL1223: s=({ struct Cyc_PP_Doc*(*
_temp1236)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1239=
_temp1214; struct Cyc_PP_Doc* _temp1240= Cyc_PP_text( _tag_arr("++", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp1237[ 2u]={ _temp1239, _temp1240};
struct _tagged_arr _temp1238={( void*) _temp1237,( void*) _temp1237,( void*)(
_temp1237 + 2u)}; _temp1236( _temp1238);}); goto _LL1217; _LL1225: s=({ struct
Cyc_PP_Doc*(* _temp1241)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1244= _temp1214; struct Cyc_PP_Doc* _temp1245= Cyc_PP_text( _tag_arr("--",
sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1242[ 2u]={ _temp1244,
_temp1245}; struct _tagged_arr _temp1243={( void*) _temp1242,( void*) _temp1242,(
void*)( _temp1242 + 2u)}; _temp1241( _temp1243);}); goto _LL1217; _LL1217:;}
goto _LL995;} _LL1009:{ struct _tuple8 _temp1247=({ struct _tuple8 _temp1246;
_temp1246.f1=( void*) _temp1092->r; _temp1246.f2=( void*) _temp1090->r;
_temp1246;}); _LL1249: goto _LL1250; _LL1250: s=({ struct Cyc_PP_Doc*(*
_temp1251)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1254= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1094); struct Cyc_PP_Doc* _temp1255= Cyc_PP_text( _tag_arr(" ? ",
sizeof( unsigned char), 4u)); struct Cyc_PP_Doc* _temp1256= Cyc_Absynpp_exp2doc_prec(
0, _temp1092); struct Cyc_PP_Doc* _temp1257= Cyc_PP_text( _tag_arr(" : ",
sizeof( unsigned char), 4u)); struct Cyc_PP_Doc* _temp1258= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1090); struct Cyc_PP_Doc* _temp1252[ 5u]={ _temp1254, _temp1255,
_temp1256, _temp1257, _temp1258}; struct _tagged_arr _temp1253={( void*)
_temp1252,( void*) _temp1252,( void*)( _temp1252 + 5u)}; _temp1251( _temp1253);});
goto _LL1248; _LL1248:;} goto _LL995; _LL1011: s=({ struct Cyc_PP_Doc*(*
_temp1259)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1262= Cyc_PP_text(
_tag_arr("(", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1263= Cyc_Absynpp_exp2doc(
_temp1098); struct Cyc_PP_Doc* _temp1264= Cyc_PP_text( _tag_arr(", ", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp1265= Cyc_Absynpp_exp2doc(
_temp1096); struct Cyc_PP_Doc* _temp1266= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1260[ 5u]={ _temp1262, _temp1263,
_temp1264, _temp1265, _temp1266}; struct _tagged_arr _temp1261={( void*)
_temp1260,( void*) _temp1260,( void*)( _temp1260 + 5u)}; _temp1259( _temp1261);});
goto _LL995; _LL1013: s=({ struct Cyc_PP_Doc*(* _temp1267)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1270= Cyc_Absynpp_exp2doc_prec( myprec,
_temp1102); struct Cyc_PP_Doc* _temp1271= Cyc_PP_text( _tag_arr("(", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1272= Cyc_Absynpp_exps2doc_prec(
20, _temp1100); struct Cyc_PP_Doc* _temp1273= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1268[ 4u]={ _temp1270, _temp1271,
_temp1272, _temp1273}; struct _tagged_arr _temp1269={( void*) _temp1268,( void*)
_temp1268,( void*)( _temp1268 + 4u)}; _temp1267( _temp1269);}); goto _LL995;
_LL1015: s=({ struct Cyc_PP_Doc*(* _temp1274)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1277= Cyc_Absynpp_exp2doc_prec( myprec, _temp1108);
struct Cyc_PP_Doc* _temp1278= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp1279= Cyc_Absynpp_exps2doc_prec( 20, _temp1106);
struct Cyc_PP_Doc* _temp1280= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp1275[ 4u]={ _temp1277, _temp1278, _temp1279,
_temp1280}; struct _tagged_arr _temp1276={( void*) _temp1275,( void*) _temp1275,(
void*)( _temp1275 + 4u)}; _temp1274( _temp1276);}); goto _LL995; _LL1017: s=({
struct Cyc_PP_Doc*(* _temp1281)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1284= Cyc_PP_text( _tag_arr("throw ", sizeof( unsigned char), 7u)); struct
Cyc_PP_Doc* _temp1285= Cyc_Absynpp_exp2doc_prec( myprec, _temp1110); struct Cyc_PP_Doc*
_temp1282[ 2u]={ _temp1284, _temp1285}; struct _tagged_arr _temp1283={( void*)
_temp1282,( void*) _temp1282,( void*)( _temp1282 + 2u)}; _temp1281( _temp1283);});
goto _LL995; _LL1019: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp1112); goto
_LL995; _LL1021: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp1116); goto _LL995;
_LL1023: s=({ struct Cyc_PP_Doc*(* _temp1286)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1289= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp1290= Cyc_Absynpp_typ2doc( _temp1120); struct Cyc_PP_Doc*
_temp1291= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1292= Cyc_Absynpp_exp2doc_prec( myprec, _temp1118); struct Cyc_PP_Doc*
_temp1287[ 4u]={ _temp1289, _temp1290, _temp1291, _temp1292}; struct _tagged_arr
_temp1288={( void*) _temp1287,( void*) _temp1287,( void*)( _temp1287 + 4u)};
_temp1286( _temp1288);}); goto _LL995; _LL1025: s=({ struct Cyc_PP_Doc*(*
_temp1293)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1296= Cyc_PP_text(
_tag_arr("&", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1297= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1122); struct Cyc_PP_Doc* _temp1294[ 2u]={ _temp1296, _temp1297};
struct _tagged_arr _temp1295={( void*) _temp1294,( void*) _temp1294,( void*)(
_temp1294 + 2u)}; _temp1293( _temp1295);}); goto _LL995; _LL1027: if( _temp1126
== 0){ s=({ struct Cyc_PP_Doc*(* _temp1298)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1301= Cyc_PP_text( _tag_arr("new ", sizeof(
unsigned char), 5u)); struct Cyc_PP_Doc* _temp1302= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1124); struct Cyc_PP_Doc* _temp1299[ 2u]={ _temp1301, _temp1302};
struct _tagged_arr _temp1300={( void*) _temp1299,( void*) _temp1299,( void*)(
_temp1299 + 2u)}; _temp1298( _temp1300);});} else{ s=({ struct Cyc_PP_Doc*(*
_temp1303)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1306= Cyc_PP_text(
_tag_arr("rnew(", sizeof( unsigned char), 6u)); struct Cyc_PP_Doc* _temp1307=
Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp1126)); struct
Cyc_PP_Doc* _temp1308= Cyc_PP_text( _tag_arr(") ", sizeof( unsigned char), 3u));
struct Cyc_PP_Doc* _temp1309= Cyc_Absynpp_exp2doc_prec( myprec, _temp1124);
struct Cyc_PP_Doc* _temp1304[ 4u]={ _temp1306, _temp1307, _temp1308, _temp1309};
struct _tagged_arr _temp1305={( void*) _temp1304,( void*) _temp1304,( void*)(
_temp1304 + 4u)}; _temp1303( _temp1305);});} goto _LL995; _LL1029: s=({ struct
Cyc_PP_Doc*(* _temp1310)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1313= Cyc_PP_text( _tag_arr("sizeof(", sizeof( unsigned char), 8u)); struct
Cyc_PP_Doc* _temp1314= Cyc_Absynpp_typ2doc( _temp1128); struct Cyc_PP_Doc*
_temp1315= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1311[ 3u]={ _temp1313, _temp1314, _temp1315}; struct _tagged_arr _temp1312={(
void*) _temp1311,( void*) _temp1311,( void*)( _temp1311 + 3u)}; _temp1310(
_temp1312);}); goto _LL995; _LL1031: s=({ struct Cyc_PP_Doc*(* _temp1316)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1319= Cyc_PP_text(
_tag_arr("sizeof(", sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp1320=
Cyc_Absynpp_exp2doc( _temp1130); struct Cyc_PP_Doc* _temp1321= Cyc_PP_text(
_tag_arr(")", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1317[ 3u]={
_temp1319, _temp1320, _temp1321}; struct _tagged_arr _temp1318={( void*)
_temp1317,( void*) _temp1317,( void*)( _temp1317 + 3u)}; _temp1316( _temp1318);});
goto _LL995; _LL1033: s=({ struct Cyc_PP_Doc*(* _temp1322)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1325= Cyc_PP_text( _tag_arr("offsetof(",
sizeof( unsigned char), 10u)); struct Cyc_PP_Doc* _temp1326= Cyc_Absynpp_typ2doc(
_temp1134); struct Cyc_PP_Doc* _temp1327= Cyc_PP_text( _tag_arr(",", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1328= Cyc_PP_textptr( _temp1132);
struct Cyc_PP_Doc* _temp1329= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp1323[ 5u]={ _temp1325, _temp1326, _temp1327,
_temp1328, _temp1329}; struct _tagged_arr _temp1324={( void*) _temp1323,( void*)
_temp1323,( void*)( _temp1323 + 5u)}; _temp1322( _temp1324);}); goto _LL995;
_LL1035: s=({ struct Cyc_PP_Doc*(* _temp1330)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1333= Cyc_PP_text( _tag_arr("*", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp1334= Cyc_Absynpp_exp2doc_prec( myprec, _temp1136);
struct Cyc_PP_Doc* _temp1331[ 2u]={ _temp1333, _temp1334}; struct _tagged_arr
_temp1332={( void*) _temp1331,( void*) _temp1331,( void*)( _temp1331 + 2u)};
_temp1330( _temp1332);}); goto _LL995; _LL1037: s=({ struct Cyc_PP_Doc*(*
_temp1335)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1338= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1140); struct Cyc_PP_Doc* _temp1339= Cyc_PP_text( _tag_arr(".",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1340= Cyc_PP_textptr(
_temp1138); struct Cyc_PP_Doc* _temp1336[ 3u]={ _temp1338, _temp1339, _temp1340};
struct _tagged_arr _temp1337={( void*) _temp1336,( void*) _temp1336,( void*)(
_temp1336 + 3u)}; _temp1335( _temp1337);}); goto _LL995; _LL1039: s=({ struct
Cyc_PP_Doc*(* _temp1341)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1344= Cyc_Absynpp_exp2doc_prec( myprec, _temp1144); struct Cyc_PP_Doc*
_temp1345= Cyc_PP_text( _tag_arr("->", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc*
_temp1346= Cyc_PP_textptr( _temp1142); struct Cyc_PP_Doc* _temp1342[ 3u]={
_temp1344, _temp1345, _temp1346}; struct _tagged_arr _temp1343={( void*)
_temp1342,( void*) _temp1342,( void*)( _temp1342 + 3u)}; _temp1341( _temp1343);});
goto _LL995; _LL1041: s=({ struct Cyc_PP_Doc*(* _temp1347)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1350= Cyc_Absynpp_exp2doc_prec( myprec,
_temp1148); struct Cyc_PP_Doc* _temp1351= Cyc_PP_text( _tag_arr("[", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1352= Cyc_Absynpp_exp2doc(
_temp1146); struct Cyc_PP_Doc* _temp1353= Cyc_PP_text( _tag_arr("]", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1348[ 4u]={ _temp1350, _temp1351,
_temp1352, _temp1353}; struct _tagged_arr _temp1349={( void*) _temp1348,( void*)
_temp1348,( void*)( _temp1348 + 4u)}; _temp1347( _temp1349);}); goto _LL995;
_LL1043: s=({ struct Cyc_PP_Doc*(* _temp1354)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1357= Cyc_PP_text( _tag_arr("$(", sizeof( unsigned char),
3u)); struct Cyc_PP_Doc* _temp1358= Cyc_Absynpp_exps2doc_prec( 20, _temp1150);
struct Cyc_PP_Doc* _temp1359= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp1355[ 3u]={ _temp1357, _temp1358, _temp1359};
struct _tagged_arr _temp1356={( void*) _temp1355,( void*) _temp1355,( void*)(
_temp1355 + 3u)}; _temp1354( _temp1356);}); goto _LL995; _LL1045: s=({ struct
Cyc_PP_Doc*(* _temp1360)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1363= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1364= Cyc_Absynpp_typ2doc((* _temp1154).f3); struct Cyc_PP_Doc* _temp1365=
Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1366= Cyc_PP_group( _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_de2doc, _temp1152)); struct Cyc_PP_Doc* _temp1361[
4u]={ _temp1363, _temp1364, _temp1365, _temp1366}; struct _tagged_arr _temp1362={(
void*) _temp1361,( void*) _temp1361,( void*)( _temp1361 + 4u)}; _temp1360(
_temp1362);}); goto _LL995; _LL1047: s= Cyc_PP_group( _tag_arr("{", sizeof(
unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1156)); goto _LL995; _LL1049: s=({ struct Cyc_PP_Doc*(* _temp1367)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1370= Cyc_PP_text( _tag_arr("{for ",
sizeof( unsigned char), 6u)); struct Cyc_PP_Doc* _temp1371= Cyc_PP_text(*(*
_temp1162->name).f2); struct Cyc_PP_Doc* _temp1372= Cyc_PP_text( _tag_arr(" < ",
sizeof( unsigned char), 4u)); struct Cyc_PP_Doc* _temp1373= Cyc_Absynpp_exp2doc(
_temp1160); struct Cyc_PP_Doc* _temp1374= Cyc_PP_text( _tag_arr(" : ", sizeof(
unsigned char), 4u)); struct Cyc_PP_Doc* _temp1375= Cyc_Absynpp_exp2doc(
_temp1158); struct Cyc_PP_Doc* _temp1376= Cyc_PP_text( _tag_arr("}", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1368[ 7u]={ _temp1370, _temp1371,
_temp1372, _temp1373, _temp1374, _temp1375, _temp1376}; struct _tagged_arr
_temp1369={( void*) _temp1368,( void*) _temp1368,( void*)( _temp1368 + 7u)};
_temp1367( _temp1369);}); goto _LL995; _LL1051: s=({ struct Cyc_PP_Doc*(*
_temp1377)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1380= Cyc_Absynpp_qvar2doc(
_temp1170); struct Cyc_PP_Doc* _temp1381= Cyc_PP_group( _tag_arr("{", sizeof(
unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1166)); struct Cyc_PP_Doc* _temp1378[ 2u]={ _temp1380, _temp1381}; struct
_tagged_arr _temp1379={( void*) _temp1378,( void*) _temp1378,( void*)( _temp1378
+ 2u)}; _temp1377( _temp1379);}); goto _LL995; _LL1053: s= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp1172)); goto _LL995; _LL1055: if( _temp1180 == 0){ s=
Cyc_Absynpp_qvar2doc( _temp1176->name);} else{ s=({ struct Cyc_PP_Doc*(*
_temp1382)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1385= Cyc_Absynpp_qvar2doc(
_temp1176->name); struct Cyc_PP_Doc* _temp1386= Cyc_PP_egroup( _tag_arr("(",
sizeof( unsigned char), 2u), _tag_arr(")", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_exp2doc,
_temp1180)); struct Cyc_PP_Doc* _temp1383[ 2u]={ _temp1385, _temp1386}; struct
_tagged_arr _temp1384={( void*) _temp1383,( void*) _temp1383,( void*)( _temp1383
+ 2u)}; _temp1382( _temp1384);});} goto _LL995; _LL1057: s= Cyc_Absynpp_qvar2doc(
_temp1190); goto _LL995; _LL1059: if( _temp1194 == 0){ s=({ struct Cyc_PP_Doc*(*
_temp1387)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1390= Cyc_PP_text(
_tag_arr("malloc(sizeof(", sizeof( unsigned char), 15u)); struct Cyc_PP_Doc*
_temp1391= Cyc_Absynpp_typ2doc( _temp1192); struct Cyc_PP_Doc* _temp1392= Cyc_PP_text(
_tag_arr("))", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1388[ 3u]={
_temp1390, _temp1391, _temp1392}; struct _tagged_arr _temp1389={( void*)
_temp1388,( void*) _temp1388,( void*)( _temp1388 + 3u)}; _temp1387( _temp1389);});}
else{ s=({ struct Cyc_PP_Doc*(* _temp1393)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1396= Cyc_PP_text( _tag_arr("rmalloc(", sizeof(
unsigned char), 9u)); struct Cyc_PP_Doc* _temp1397= Cyc_Absynpp_exp2doc(( struct
Cyc_Absyn_Exp*) _check_null( _temp1194)); struct Cyc_PP_Doc* _temp1398= Cyc_PP_text(
_tag_arr(",", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1399= Cyc_PP_text(
_tag_arr("sizeof(", sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp1400=
Cyc_Absynpp_typ2doc( _temp1192); struct Cyc_PP_Doc* _temp1401= Cyc_PP_text(
_tag_arr("))", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1394[ 6u]={
_temp1396, _temp1397, _temp1398, _temp1399, _temp1400, _temp1401}; struct
_tagged_arr _temp1395={( void*) _temp1394,( void*) _temp1394,( void*)( _temp1394
+ 6u)}; _temp1393( _temp1395);});} goto _LL995; _LL1061: s= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp1196)); goto _LL995; _LL1063: s=({ struct Cyc_PP_Doc*(*
_temp1402)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1405= Cyc_PP_text(
_tag_arr("({ ", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc* _temp1406= Cyc_PP_nest(
2, Cyc_Absynpp_stmt2doc( _temp1200)); struct Cyc_PP_Doc* _temp1407= Cyc_PP_text(
_tag_arr(" })", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc* _temp1403[ 3u]={
_temp1405, _temp1406, _temp1407}; struct _tagged_arr _temp1404={( void*)
_temp1403,( void*) _temp1403,( void*)( _temp1403 + 3u)}; _temp1402( _temp1404);});
goto _LL995; _LL1065: s=({ struct Cyc_PP_Doc*(* _temp1408)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1411= Cyc_PP_text( _tag_arr("codegen(",
sizeof( unsigned char), 9u)); struct Cyc_PP_Doc* _temp1412= Cyc_PP_nest( 2, Cyc_Absynpp_decl2doc(({
struct Cyc_Absyn_Decl* _temp1414=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof(
struct Cyc_Absyn_Decl)); _temp1414->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp1415=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1415[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp1416; _temp1416.tag= Cyc_Absyn_Fn_d;
_temp1416.f1= _temp1202; _temp1416;}); _temp1415;})); _temp1414->loc= e->loc;
_temp1414;}))); struct Cyc_PP_Doc* _temp1413= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1409[ 3u]={ _temp1411, _temp1412,
_temp1413}; struct _tagged_arr _temp1410={( void*) _temp1409,( void*) _temp1409,(
void*)( _temp1409 + 3u)}; _temp1408( _temp1410);}); goto _LL995; _LL1067: s=({
struct Cyc_PP_Doc*(* _temp1417)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1420= Cyc_PP_text( _tag_arr("fill(", sizeof( unsigned char), 6u)); struct
Cyc_PP_Doc* _temp1421= Cyc_PP_nest( 2, Cyc_Absynpp_exp2doc( _temp1204)); struct
Cyc_PP_Doc* _temp1422= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u));
struct Cyc_PP_Doc* _temp1418[ 3u]={ _temp1420, _temp1421, _temp1422}; struct
_tagged_arr _temp1419={( void*) _temp1418,( void*) _temp1418,( void*)( _temp1418
+ 3u)}; _temp1417( _temp1419);}); goto _LL995; _LL995:;} if( inprec >= myprec){
s=({ struct Cyc_PP_Doc*(* _temp1423)( struct _tagged_arr)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp1426= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u));
struct Cyc_PP_Doc* _temp1427= s; struct Cyc_PP_Doc* _temp1428= Cyc_PP_text(
_tag_arr(")", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1424[ 3u]={
_temp1426, _temp1427, _temp1428}; struct _tagged_arr _temp1425={( void*)
_temp1424,( void*) _temp1424,( void*)( _temp1424 + 3u)}; _temp1423( _temp1425);});}
return s;} struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc( void* d){ void*
_temp1429= d; struct Cyc_Absyn_Exp* _temp1435; struct _tagged_arr* _temp1437;
_LL1431: if(*(( int*) _temp1429) == Cyc_Absyn_ArrayElement){ _LL1436: _temp1435=((
struct Cyc_Absyn_ArrayElement_struct*) _temp1429)->f1; goto _LL1432;} else{ goto
_LL1433;} _LL1433: if(*(( int*) _temp1429) == Cyc_Absyn_FieldName){ _LL1438:
_temp1437=(( struct Cyc_Absyn_FieldName_struct*) _temp1429)->f1; goto _LL1434;}
else{ goto _LL1430;} _LL1432: return({ struct Cyc_PP_Doc*(* _temp1439)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1442= Cyc_PP_text( _tag_arr(".[",
sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1443= Cyc_Absynpp_exp2doc(
_temp1435); struct Cyc_PP_Doc* _temp1444= Cyc_PP_text( _tag_arr("]", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1440[ 3u]={ _temp1442, _temp1443,
_temp1444}; struct _tagged_arr _temp1441={( void*) _temp1440,( void*) _temp1440,(
void*)( _temp1440 + 3u)}; _temp1439( _temp1441);}); _LL1434: return({ struct Cyc_PP_Doc*(*
_temp1445)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1448= Cyc_PP_text(
_tag_arr(".", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1449= Cyc_PP_textptr(
_temp1437); struct Cyc_PP_Doc* _temp1446[ 2u]={ _temp1448, _temp1449}; struct
_tagged_arr _temp1447={( void*) _temp1446,( void*) _temp1446,( void*)( _temp1446
+ 2u)}; _temp1445( _temp1447);}); _LL1430:;} struct Cyc_PP_Doc* Cyc_Absynpp_de2doc(
struct _tuple6* de){ if((* de).f1 == 0){ return Cyc_Absynpp_exp2doc((* de).f2);}
else{ return({ struct Cyc_PP_Doc*(* _temp1450)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1453= Cyc_PP_egroup( _tag_arr("", sizeof( unsigned char),
1u), _tag_arr("=", sizeof( unsigned char), 2u), _tag_arr("=", sizeof(
unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(* de).f1));
struct Cyc_PP_Doc* _temp1454= Cyc_Absynpp_exp2doc((* de).f2); struct Cyc_PP_Doc*
_temp1451[ 2u]={ _temp1453, _temp1454}; struct _tagged_arr _temp1452={( void*)
_temp1451,( void*) _temp1451,( void*)( _temp1451 + 2u)}; _temp1450( _temp1452);});}}
struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec( int inprec, struct Cyc_List_List*
es){ return Cyc_PP_group( _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("",
sizeof( unsigned char), 1u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*), int env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec, inprec, es));}
struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc( void* c){ void* _temp1455= c;
unsigned char _temp1473; void* _temp1475; short _temp1477; void* _temp1479; int
_temp1481; void* _temp1483; int _temp1485; void* _temp1487; long long _temp1489;
void* _temp1491; struct _tagged_arr _temp1493; struct _tagged_arr _temp1495;
_LL1457: if(( unsigned int) _temp1455 > 1u?*(( int*) _temp1455) == Cyc_Absyn_Char_c:
0){ _LL1476: _temp1475=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp1455)->f1;
goto _LL1474; _LL1474: _temp1473=(( struct Cyc_Absyn_Char_c_struct*) _temp1455)->f2;
goto _LL1458;} else{ goto _LL1459;} _LL1459: if(( unsigned int) _temp1455 > 1u?*((
int*) _temp1455) == Cyc_Absyn_Short_c: 0){ _LL1480: _temp1479=( void*)(( struct
Cyc_Absyn_Short_c_struct*) _temp1455)->f1; goto _LL1478; _LL1478: _temp1477=((
struct Cyc_Absyn_Short_c_struct*) _temp1455)->f2; goto _LL1460;} else{ goto
_LL1461;} _LL1461: if(( unsigned int) _temp1455 > 1u?*(( int*) _temp1455) == Cyc_Absyn_Int_c:
0){ _LL1484: _temp1483=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1455)->f1;
if( _temp1483 ==( void*) Cyc_Absyn_Signed){ goto _LL1482;} else{ goto _LL1463;}
_LL1482: _temp1481=(( struct Cyc_Absyn_Int_c_struct*) _temp1455)->f2; goto
_LL1462;} else{ goto _LL1463;} _LL1463: if(( unsigned int) _temp1455 > 1u?*((
int*) _temp1455) == Cyc_Absyn_Int_c: 0){ _LL1488: _temp1487=( void*)(( struct
Cyc_Absyn_Int_c_struct*) _temp1455)->f1; if( _temp1487 ==( void*) Cyc_Absyn_Unsigned){
goto _LL1486;} else{ goto _LL1465;} _LL1486: _temp1485=(( struct Cyc_Absyn_Int_c_struct*)
_temp1455)->f2; goto _LL1464;} else{ goto _LL1465;} _LL1465: if(( unsigned int)
_temp1455 > 1u?*(( int*) _temp1455) == Cyc_Absyn_LongLong_c: 0){ _LL1492:
_temp1491=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp1455)->f1; goto
_LL1490; _LL1490: _temp1489=(( struct Cyc_Absyn_LongLong_c_struct*) _temp1455)->f2;
goto _LL1466;} else{ goto _LL1467;} _LL1467: if(( unsigned int) _temp1455 > 1u?*((
int*) _temp1455) == Cyc_Absyn_Float_c: 0){ _LL1494: _temp1493=(( struct Cyc_Absyn_Float_c_struct*)
_temp1455)->f1; goto _LL1468;} else{ goto _LL1469;} _LL1469: if( _temp1455 ==(
void*) Cyc_Absyn_Null_c){ goto _LL1470;} else{ goto _LL1471;} _LL1471: if((
unsigned int) _temp1455 > 1u?*(( int*) _temp1455) == Cyc_Absyn_String_c: 0){
_LL1496: _temp1495=(( struct Cyc_Absyn_String_c_struct*) _temp1455)->f1; goto
_LL1472;} else{ goto _LL1456;} _LL1458: return Cyc_PP_text(( struct _tagged_arr)({
struct _tagged_arr _temp1497= Cyc_Absynpp_char_escape( _temp1473); xprintf("'%.*s'",
_get_arr_size( _temp1497, 1u), _temp1497.curr);})); _LL1460: return Cyc_PP_text((
struct _tagged_arr) xprintf("%d",( int) _temp1477)); _LL1462: return Cyc_PP_text((
struct _tagged_arr) Cyc_Core_string_of_int( _temp1481)); _LL1464: return Cyc_PP_text((
struct _tagged_arr) xprintf("%u",( unsigned int) _temp1485)); _LL1466: return
Cyc_PP_text( _tag_arr("<<FIX LONG LONG CONSTANT>>", sizeof( unsigned char), 27u));
_LL1468: return Cyc_PP_text( _temp1493); _LL1470: return Cyc_PP_text( _tag_arr("null",
sizeof( unsigned char), 5u)); _LL1472: return({ struct Cyc_PP_Doc*(* _temp1498)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1501= Cyc_PP_text(
_tag_arr("\"", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1502= Cyc_PP_text(
Cyc_Absynpp_string_escape( _temp1495)); struct Cyc_PP_Doc* _temp1503= Cyc_PP_text(
_tag_arr("\"", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1499[ 3u]={
_temp1501, _temp1502, _temp1503}; struct _tagged_arr _temp1500={( void*)
_temp1499,( void*) _temp1499,( void*)( _temp1499 + 3u)}; _temp1498( _temp1500);});
_LL1456:;} struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc( int inprec, void* p,
struct Cyc_List_List* es){ struct Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc( p); if(
p ==( void*) Cyc_Absyn_Size){ if( es == 0? 1:(( struct Cyc_List_List*)
_check_null( es))->tl != 0){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp1504=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1504[ 0]=({ struct Cyc_Core_Failure_struct _temp1505; _temp1505.tag= Cyc_Core_Failure;
_temp1505.f1=( struct _tagged_arr)({ struct _tagged_arr _temp1506= Cyc_PP_string_of_doc(
ps, 72); xprintf("Absynpp::primapp2doc Size: %.*s with bad args", _get_arr_size(
_temp1506, 1u), _temp1506.curr);}); _temp1505;}); _temp1504;}));}{ struct Cyc_PP_Doc*
_temp1507= Cyc_Absynpp_exp2doc_prec( inprec,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd); goto _LL1508; _LL1508: return({ struct
Cyc_PP_Doc*(* _temp1509)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1512= _temp1507; struct Cyc_PP_Doc* _temp1513= Cyc_PP_text( _tag_arr(".size",
sizeof( unsigned char), 6u)); struct Cyc_PP_Doc* _temp1510[ 2u]={ _temp1512,
_temp1513}; struct _tagged_arr _temp1511={( void*) _temp1510,( void*) _temp1510,(
void*)( _temp1510 + 2u)}; _temp1509( _temp1511);});}} else{ if( ! Cyc_Absyn_is_format_prim(
p)){ struct Cyc_List_List* ds=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(*
f)( int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Absynpp_exp2doc_prec, inprec, es); if( ds == 0){( int) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp1514=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1514[ 0]=({ struct Cyc_Core_Failure_struct
_temp1515; _temp1515.tag= Cyc_Core_Failure; _temp1515.f1=( struct _tagged_arr)({
struct _tagged_arr _temp1516= Cyc_PP_string_of_doc( ps, 72); xprintf("Absynpp::primapp2doc: %.*s with no args",
_get_arr_size( _temp1516, 1u), _temp1516.curr);}); _temp1515;}); _temp1514;}));}
else{ if((( struct Cyc_List_List*) _check_null( ds))->tl == 0){ return({ struct
Cyc_PP_Doc*(* _temp1517)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1520= ps; struct Cyc_PP_Doc* _temp1521= Cyc_PP_text( _tag_arr(" ", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1522=( struct Cyc_PP_Doc*)((
struct Cyc_List_List*) _check_null( ds))->hd; struct Cyc_PP_Doc* _temp1518[ 3u]={
_temp1520, _temp1521, _temp1522}; struct _tagged_arr _temp1519={( void*)
_temp1518,( void*) _temp1518,( void*)( _temp1518 + 3u)}; _temp1517( _temp1519);});}
else{ if((( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( ds))->tl))->tl != 0){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp1523=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1523[ 0]=({ struct Cyc_Core_Failure_struct _temp1524; _temp1524.tag= Cyc_Core_Failure;
_temp1524.f1=( struct _tagged_arr)({ struct _tagged_arr _temp1525= Cyc_PP_string_of_doc(
ps, 72); xprintf("Absynpp::primapp2doc: %.*s with more than 2 args",
_get_arr_size( _temp1525, 1u), _temp1525.curr);}); _temp1524;}); _temp1523;}));}
else{ return({ struct Cyc_PP_Doc*(* _temp1526)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1529=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*)
_check_null( ds))->hd; struct Cyc_PP_Doc* _temp1530= Cyc_PP_text( _tag_arr(" ",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1531= ps; struct Cyc_PP_Doc*
_temp1532= Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1533=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null((( struct
Cyc_List_List*) _check_null( ds))->tl))->hd; struct Cyc_PP_Doc* _temp1527[ 5u]={
_temp1529, _temp1530, _temp1531, _temp1532, _temp1533}; struct _tagged_arr
_temp1528={( void*) _temp1527,( void*) _temp1527,( void*)( _temp1527 + 5u)};
_temp1526( _temp1528);});}}}} else{ return({ struct Cyc_PP_Doc*(* _temp1534)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1537= ps; struct Cyc_PP_Doc*
_temp1538= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1539= Cyc_Absynpp_exps2doc_prec( 20, es); struct Cyc_PP_Doc* _temp1540= Cyc_PP_text(
_tag_arr(")", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1535[ 4u]={
_temp1537, _temp1538, _temp1539, _temp1540}; struct _tagged_arr _temp1536={(
void*) _temp1535,( void*) _temp1535,( void*)( _temp1535 + 4u)}; _temp1534(
_temp1536);});}}} struct _tagged_arr Cyc_Absynpp_prim2str( void* p){ void*
_temp1541= p; _LL1543: if( _temp1541 ==( void*) Cyc_Absyn_Plus){ goto _LL1544;}
else{ goto _LL1545;} _LL1545: if( _temp1541 ==( void*) Cyc_Absyn_Times){ goto
_LL1546;} else{ goto _LL1547;} _LL1547: if( _temp1541 ==( void*) Cyc_Absyn_Minus){
goto _LL1548;} else{ goto _LL1549;} _LL1549: if( _temp1541 ==( void*) Cyc_Absyn_Div){
goto _LL1550;} else{ goto _LL1551;} _LL1551: if( _temp1541 ==( void*) Cyc_Absyn_Mod){
goto _LL1552;} else{ goto _LL1553;} _LL1553: if( _temp1541 ==( void*) Cyc_Absyn_Eq){
goto _LL1554;} else{ goto _LL1555;} _LL1555: if( _temp1541 ==( void*) Cyc_Absyn_Neq){
goto _LL1556;} else{ goto _LL1557;} _LL1557: if( _temp1541 ==( void*) Cyc_Absyn_Gt){
goto _LL1558;} else{ goto _LL1559;} _LL1559: if( _temp1541 ==( void*) Cyc_Absyn_Lt){
goto _LL1560;} else{ goto _LL1561;} _LL1561: if( _temp1541 ==( void*) Cyc_Absyn_Gte){
goto _LL1562;} else{ goto _LL1563;} _LL1563: if( _temp1541 ==( void*) Cyc_Absyn_Lte){
goto _LL1564;} else{ goto _LL1565;} _LL1565: if( _temp1541 ==( void*) Cyc_Absyn_Not){
goto _LL1566;} else{ goto _LL1567;} _LL1567: if( _temp1541 ==( void*) Cyc_Absyn_Bitnot){
goto _LL1568;} else{ goto _LL1569;} _LL1569: if( _temp1541 ==( void*) Cyc_Absyn_Bitand){
goto _LL1570;} else{ goto _LL1571;} _LL1571: if( _temp1541 ==( void*) Cyc_Absyn_Bitor){
goto _LL1572;} else{ goto _LL1573;} _LL1573: if( _temp1541 ==( void*) Cyc_Absyn_Bitxor){
goto _LL1574;} else{ goto _LL1575;} _LL1575: if( _temp1541 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL1576;} else{ goto _LL1577;} _LL1577: if( _temp1541 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL1578;} else{ goto _LL1579;} _LL1579: if( _temp1541 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL1580;} else{ goto _LL1581;} _LL1581: if( _temp1541 ==( void*) Cyc_Absyn_Size){
goto _LL1582;} else{ goto _LL1583;} _LL1583: if( _temp1541 ==( void*) Cyc_Absyn_Printf){
goto _LL1584;} else{ goto _LL1585;} _LL1585: if( _temp1541 ==( void*) Cyc_Absyn_Fprintf){
goto _LL1586;} else{ goto _LL1587;} _LL1587: if( _temp1541 ==( void*) Cyc_Absyn_Xprintf){
goto _LL1588;} else{ goto _LL1589;} _LL1589: if( _temp1541 ==( void*) Cyc_Absyn_Scanf){
goto _LL1590;} else{ goto _LL1591;} _LL1591: if( _temp1541 ==( void*) Cyc_Absyn_Fscanf){
goto _LL1592;} else{ goto _LL1593;} _LL1593: if( _temp1541 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1594;} else{ goto _LL1542;} _LL1544: return _tag_arr("+", sizeof(
unsigned char), 2u); _LL1546: return _tag_arr("*", sizeof( unsigned char), 2u);
_LL1548: return _tag_arr("-", sizeof( unsigned char), 2u); _LL1550: return
_tag_arr("/", sizeof( unsigned char), 2u); _LL1552: return _tag_arr("%", sizeof(
unsigned char), 2u); _LL1554: return _tag_arr("==", sizeof( unsigned char), 3u);
_LL1556: return _tag_arr("!=", sizeof( unsigned char), 3u); _LL1558: return
_tag_arr(">", sizeof( unsigned char), 2u); _LL1560: return _tag_arr("<", sizeof(
unsigned char), 2u); _LL1562: return _tag_arr(">=", sizeof( unsigned char), 3u);
_LL1564: return _tag_arr("<=", sizeof( unsigned char), 3u); _LL1566: return
_tag_arr("!", sizeof( unsigned char), 2u); _LL1568: return _tag_arr("~", sizeof(
unsigned char), 2u); _LL1570: return _tag_arr("&", sizeof( unsigned char), 2u);
_LL1572: return _tag_arr("|", sizeof( unsigned char), 2u); _LL1574: return
_tag_arr("^", sizeof( unsigned char), 2u); _LL1576: return _tag_arr("<<",
sizeof( unsigned char), 3u); _LL1578: return _tag_arr(">>", sizeof(
unsigned char), 3u); _LL1580: return _tag_arr(">>>", sizeof( unsigned char), 4u);
_LL1582: return _tag_arr("size", sizeof( unsigned char), 5u); _LL1584: return
_tag_arr("printf", sizeof( unsigned char), 7u); _LL1586: return _tag_arr("fprintf",
sizeof( unsigned char), 8u); _LL1588: return _tag_arr("xprintf", sizeof(
unsigned char), 8u); _LL1590: return _tag_arr("scanf", sizeof( unsigned char), 6u);
_LL1592: return _tag_arr("fscanf", sizeof( unsigned char), 7u); _LL1594: return
_tag_arr("sscanf", sizeof( unsigned char), 7u); _LL1542:;} struct Cyc_PP_Doc*
Cyc_Absynpp_prim2doc( void* p){ return Cyc_PP_text( Cyc_Absynpp_prim2str( p));}
struct _tuple9{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ; struct
Cyc_PP_Doc* Cyc_Absynpp_structarg2doc( struct _tuple9* t){ if((* t).f1 == 0){
return Cyc_Absynpp_exp2doc((* t).f2);} else{ return({ struct Cyc_PP_Doc*(*
_temp1595)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1598= Cyc_PP_textptr((
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v); struct
Cyc_PP_Doc* _temp1599= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u));
struct Cyc_PP_Doc* _temp1600= Cyc_Absynpp_exp2doc((* t).f2); struct Cyc_PP_Doc*
_temp1596[ 3u]={ _temp1598, _temp1599, _temp1600}; struct _tagged_arr _temp1597={(
void*) _temp1596,( void*) _temp1596,( void*)( _temp1596 + 3u)}; _temp1595(
_temp1597);});}} int Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s){ void*
_temp1601=( void*) s->r; struct Cyc_Absyn_Stmt* _temp1607; struct Cyc_Absyn_Decl*
_temp1609; _LL1603: if(( unsigned int) _temp1601 > 1u?*(( int*) _temp1601) ==
Cyc_Absyn_Decl_s: 0){ _LL1610: _temp1609=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1601)->f1; goto _LL1608; _LL1608: _temp1607=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1601)->f2; goto _LL1604;} else{ goto _LL1605;} _LL1605: goto _LL1606;
_LL1604: return 1; _LL1606: return 0; _LL1602:;} struct Cyc_PP_Doc* Cyc_Absynpp_stmt2doc(
struct Cyc_Absyn_Stmt* st){ struct Cyc_PP_Doc* s;{ void* _temp1611=( void*) st->r;
struct Cyc_Absyn_Exp* _temp1655; struct Cyc_Absyn_Stmt* _temp1657; struct Cyc_Absyn_Stmt*
_temp1659; struct Cyc_Absyn_Exp* _temp1661; struct Cyc_Absyn_Stmt* _temp1663;
struct Cyc_Absyn_Stmt* _temp1665; struct Cyc_Absyn_Exp* _temp1667; struct Cyc_Absyn_Stmt*
_temp1669; struct _tuple2 _temp1671; struct Cyc_Absyn_Stmt* _temp1673; struct
Cyc_Absyn_Exp* _temp1675; struct Cyc_Absyn_Stmt* _temp1677; struct Cyc_Absyn_Stmt*
_temp1679; struct Cyc_Absyn_Stmt* _temp1681; struct _tagged_arr* _temp1683;
struct Cyc_Absyn_Stmt* _temp1685; struct _tuple2 _temp1687; struct Cyc_Absyn_Stmt*
_temp1689; struct Cyc_Absyn_Exp* _temp1691; struct _tuple2 _temp1693; struct Cyc_Absyn_Stmt*
_temp1695; struct Cyc_Absyn_Exp* _temp1697; struct Cyc_Absyn_Exp* _temp1699;
struct Cyc_List_List* _temp1701; struct Cyc_Absyn_Exp* _temp1703; struct Cyc_List_List*
_temp1705; struct Cyc_Absyn_Exp* _temp1707; struct Cyc_Absyn_Switch_clause**
_temp1709; struct Cyc_List_List* _temp1711; struct Cyc_Absyn_Switch_clause**
_temp1713; struct Cyc_List_List* _temp1715; struct Cyc_Absyn_Stmt* _temp1717;
struct Cyc_Absyn_Decl* _temp1719; struct Cyc_Absyn_Stmt* _temp1721; struct Cyc_Absyn_Stmt*
_temp1723; struct Cyc_Absyn_Stmt* _temp1725; struct _tagged_arr* _temp1727;
struct _tuple2 _temp1729; struct Cyc_Absyn_Stmt* _temp1731; struct Cyc_Absyn_Exp*
_temp1733; struct Cyc_Absyn_Stmt* _temp1735; struct Cyc_List_List* _temp1737;
struct Cyc_Absyn_Stmt* _temp1739; struct Cyc_Absyn_Stmt* _temp1741; struct Cyc_Absyn_Vardecl*
_temp1743; struct Cyc_Absyn_Tvar* _temp1745; _LL1613: if( _temp1611 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL1614;} else{ goto _LL1615;} _LL1615: if((
unsigned int) _temp1611 > 1u?*(( int*) _temp1611) == Cyc_Absyn_Exp_s: 0){
_LL1656: _temp1655=(( struct Cyc_Absyn_Exp_s_struct*) _temp1611)->f1; goto
_LL1616;} else{ goto _LL1617;} _LL1617: if(( unsigned int) _temp1611 > 1u?*((
int*) _temp1611) == Cyc_Absyn_Seq_s: 0){ _LL1660: _temp1659=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1611)->f1; goto _LL1658; _LL1658: _temp1657=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1611)->f2; goto _LL1618;} else{ goto _LL1619;} _LL1619: if(( unsigned int)
_temp1611 > 1u?*(( int*) _temp1611) == Cyc_Absyn_Return_s: 0){ _LL1662:
_temp1661=(( struct Cyc_Absyn_Return_s_struct*) _temp1611)->f1; goto _LL1620;}
else{ goto _LL1621;} _LL1621: if(( unsigned int) _temp1611 > 1u?*(( int*)
_temp1611) == Cyc_Absyn_IfThenElse_s: 0){ _LL1668: _temp1667=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1611)->f1; goto _LL1666; _LL1666: _temp1665=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1611)->f2; goto _LL1664; _LL1664: _temp1663=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1611)->f3; goto _LL1622;} else{ goto _LL1623;} _LL1623: if(( unsigned int)
_temp1611 > 1u?*(( int*) _temp1611) == Cyc_Absyn_While_s: 0){ _LL1672: _temp1671=((
struct Cyc_Absyn_While_s_struct*) _temp1611)->f1; _LL1676: _temp1675= _temp1671.f1;
goto _LL1674; _LL1674: _temp1673= _temp1671.f2; goto _LL1670; _LL1670: _temp1669=((
struct Cyc_Absyn_While_s_struct*) _temp1611)->f2; goto _LL1624;} else{ goto
_LL1625;} _LL1625: if(( unsigned int) _temp1611 > 1u?*(( int*) _temp1611) == Cyc_Absyn_Break_s:
0){ _LL1678: _temp1677=(( struct Cyc_Absyn_Break_s_struct*) _temp1611)->f1; goto
_LL1626;} else{ goto _LL1627;} _LL1627: if(( unsigned int) _temp1611 > 1u?*((
int*) _temp1611) == Cyc_Absyn_Continue_s: 0){ _LL1680: _temp1679=(( struct Cyc_Absyn_Continue_s_struct*)
_temp1611)->f1; goto _LL1628;} else{ goto _LL1629;} _LL1629: if(( unsigned int)
_temp1611 > 1u?*(( int*) _temp1611) == Cyc_Absyn_Goto_s: 0){ _LL1684: _temp1683=((
struct Cyc_Absyn_Goto_s_struct*) _temp1611)->f1; goto _LL1682; _LL1682:
_temp1681=(( struct Cyc_Absyn_Goto_s_struct*) _temp1611)->f2; goto _LL1630;}
else{ goto _LL1631;} _LL1631: if(( unsigned int) _temp1611 > 1u?*(( int*)
_temp1611) == Cyc_Absyn_For_s: 0){ _LL1700: _temp1699=(( struct Cyc_Absyn_For_s_struct*)
_temp1611)->f1; goto _LL1694; _LL1694: _temp1693=(( struct Cyc_Absyn_For_s_struct*)
_temp1611)->f2; _LL1698: _temp1697= _temp1693.f1; goto _LL1696; _LL1696:
_temp1695= _temp1693.f2; goto _LL1688; _LL1688: _temp1687=(( struct Cyc_Absyn_For_s_struct*)
_temp1611)->f3; _LL1692: _temp1691= _temp1687.f1; goto _LL1690; _LL1690:
_temp1689= _temp1687.f2; goto _LL1686; _LL1686: _temp1685=(( struct Cyc_Absyn_For_s_struct*)
_temp1611)->f4; goto _LL1632;} else{ goto _LL1633;} _LL1633: if(( unsigned int)
_temp1611 > 1u?*(( int*) _temp1611) == Cyc_Absyn_Switch_s: 0){ _LL1704:
_temp1703=(( struct Cyc_Absyn_Switch_s_struct*) _temp1611)->f1; goto _LL1702;
_LL1702: _temp1701=(( struct Cyc_Absyn_Switch_s_struct*) _temp1611)->f2; goto
_LL1634;} else{ goto _LL1635;} _LL1635: if(( unsigned int) _temp1611 > 1u?*((
int*) _temp1611) == Cyc_Absyn_SwitchC_s: 0){ _LL1708: _temp1707=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp1611)->f1; goto _LL1706; _LL1706: _temp1705=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp1611)->f2; goto _LL1636;} else{ goto _LL1637;} _LL1637: if(( unsigned int)
_temp1611 > 1u?*(( int*) _temp1611) == Cyc_Absyn_Fallthru_s: 0){ _LL1712:
_temp1711=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp1611)->f1; if( _temp1711
== 0){ goto _LL1710;} else{ goto _LL1639;} _LL1710: _temp1709=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1611)->f2; goto _LL1638;} else{ goto _LL1639;} _LL1639: if(( unsigned int)
_temp1611 > 1u?*(( int*) _temp1611) == Cyc_Absyn_Fallthru_s: 0){ _LL1716:
_temp1715=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp1611)->f1; goto _LL1714;
_LL1714: _temp1713=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp1611)->f2; goto
_LL1640;} else{ goto _LL1641;} _LL1641: if(( unsigned int) _temp1611 > 1u?*((
int*) _temp1611) == Cyc_Absyn_Decl_s: 0){ _LL1720: _temp1719=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1611)->f1; goto _LL1718; _LL1718: _temp1717=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1611)->f2; goto _LL1642;} else{ goto _LL1643;} _LL1643: if(( unsigned int)
_temp1611 > 1u?*(( int*) _temp1611) == Cyc_Absyn_Cut_s: 0){ _LL1722: _temp1721=((
struct Cyc_Absyn_Cut_s_struct*) _temp1611)->f1; goto _LL1644;} else{ goto
_LL1645;} _LL1645: if(( unsigned int) _temp1611 > 1u?*(( int*) _temp1611) == Cyc_Absyn_Splice_s:
0){ _LL1724: _temp1723=(( struct Cyc_Absyn_Splice_s_struct*) _temp1611)->f1;
goto _LL1646;} else{ goto _LL1647;} _LL1647: if(( unsigned int) _temp1611 > 1u?*((
int*) _temp1611) == Cyc_Absyn_Label_s: 0){ _LL1728: _temp1727=(( struct Cyc_Absyn_Label_s_struct*)
_temp1611)->f1; goto _LL1726; _LL1726: _temp1725=(( struct Cyc_Absyn_Label_s_struct*)
_temp1611)->f2; goto _LL1648;} else{ goto _LL1649;} _LL1649: if(( unsigned int)
_temp1611 > 1u?*(( int*) _temp1611) == Cyc_Absyn_Do_s: 0){ _LL1736: _temp1735=((
struct Cyc_Absyn_Do_s_struct*) _temp1611)->f1; goto _LL1730; _LL1730: _temp1729=((
struct Cyc_Absyn_Do_s_struct*) _temp1611)->f2; _LL1734: _temp1733= _temp1729.f1;
goto _LL1732; _LL1732: _temp1731= _temp1729.f2; goto _LL1650;} else{ goto
_LL1651;} _LL1651: if(( unsigned int) _temp1611 > 1u?*(( int*) _temp1611) == Cyc_Absyn_TryCatch_s:
0){ _LL1740: _temp1739=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp1611)->f1;
goto _LL1738; _LL1738: _temp1737=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1611)->f2; goto _LL1652;} else{ goto _LL1653;} _LL1653: if(( unsigned int)
_temp1611 > 1u?*(( int*) _temp1611) == Cyc_Absyn_Region_s: 0){ _LL1746:
_temp1745=(( struct Cyc_Absyn_Region_s_struct*) _temp1611)->f1; goto _LL1744;
_LL1744: _temp1743=(( struct Cyc_Absyn_Region_s_struct*) _temp1611)->f2; goto
_LL1742; _LL1742: _temp1741=(( struct Cyc_Absyn_Region_s_struct*) _temp1611)->f3;
goto _LL1654;} else{ goto _LL1612;} _LL1614: s= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); goto _LL1612; _LL1616: s=({ struct Cyc_PP_Doc*(*
_temp1747)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1750= Cyc_Absynpp_exp2doc(
_temp1655); struct Cyc_PP_Doc* _temp1751= Cyc_PP_text( _tag_arr(";", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1748[ 2u]={ _temp1750, _temp1751};
struct _tagged_arr _temp1749={( void*) _temp1748,( void*) _temp1748,( void*)(
_temp1748 + 2u)}; _temp1747( _temp1749);}); goto _LL1612; _LL1618: if( Cyc_Absynpp_decls_first){
if( Cyc_Absynpp_is_declaration( _temp1659)){ s=({ struct Cyc_PP_Doc*(* _temp1752)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1755= Cyc_PP_text(
_tag_arr("{ ", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1756= Cyc_PP_nest(
2, Cyc_Absynpp_stmt2doc( _temp1659)); struct Cyc_PP_Doc* _temp1757= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1758= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp1759= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp1760= Cyc_Absynpp_is_declaration( _temp1657)?({ struct Cyc_PP_Doc*(*
_temp1761)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1764= Cyc_PP_text(
_tag_arr("{ ", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1765= Cyc_PP_nest(
2, Cyc_Absynpp_stmt2doc( _temp1657)); struct Cyc_PP_Doc* _temp1766= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1767= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1762[ 4u]={ _temp1764, _temp1765, _temp1766, _temp1767};
struct _tagged_arr _temp1763={( void*) _temp1762,( void*) _temp1762,( void*)(
_temp1762 + 4u)}; _temp1761( _temp1763);}): Cyc_Absynpp_stmt2doc( _temp1657);
struct Cyc_PP_Doc* _temp1753[ 6u]={ _temp1755, _temp1756, _temp1757, _temp1758,
_temp1759, _temp1760}; struct _tagged_arr _temp1754={( void*) _temp1753,( void*)
_temp1753,( void*)( _temp1753 + 6u)}; _temp1752( _temp1754);});} else{ if( Cyc_Absynpp_is_declaration(
_temp1657)){ s=({ struct Cyc_PP_Doc*(* _temp1768)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1771= Cyc_Absynpp_stmt2doc( _temp1659); struct Cyc_PP_Doc*
_temp1772= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1773= Cyc_PP_text(
_tag_arr("{ ", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1774= Cyc_PP_nest(
2, Cyc_Absynpp_stmt2doc( _temp1657)); struct Cyc_PP_Doc* _temp1775= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1776= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1769[ 6u]={ _temp1771, _temp1772, _temp1773, _temp1774,
_temp1775, _temp1776}; struct _tagged_arr _temp1770={( void*) _temp1769,( void*)
_temp1769,( void*)( _temp1769 + 6u)}; _temp1768( _temp1770);});} else{ s=((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct
_tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,
_tag_arr("", sizeof( unsigned char), 1u),({ struct Cyc_List_List* _temp1777=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1777->hd=(
void*) _temp1659; _temp1777->tl=({ struct Cyc_List_List* _temp1778=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1778->hd=( void*) _temp1657;
_temp1778->tl= 0; _temp1778;}); _temp1777;}));}}} else{ s=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct _tagged_arr sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc, _tag_arr("",
sizeof( unsigned char), 1u),({ struct Cyc_List_List* _temp1779=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1779->hd=( void*) _temp1659;
_temp1779->tl=({ struct Cyc_List_List* _temp1780=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1780->hd=( void*) _temp1657;
_temp1780->tl= 0; _temp1780;}); _temp1779;}));} goto _LL1612; _LL1620: if(
_temp1661 == 0){ s= Cyc_PP_text( _tag_arr("return;", sizeof( unsigned char), 8u));}
else{ s=({ struct Cyc_PP_Doc*(* _temp1781)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1784= Cyc_PP_text( _tag_arr("return ", sizeof(
unsigned char), 8u)); struct Cyc_PP_Doc* _temp1785= _temp1661 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp1661)); struct
Cyc_PP_Doc* _temp1786= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u));
struct Cyc_PP_Doc* _temp1782[ 3u]={ _temp1784, _temp1785, _temp1786}; struct
_tagged_arr _temp1783={( void*) _temp1782,( void*) _temp1782,( void*)( _temp1782
+ 3u)}; _temp1781( _temp1783);});} goto _LL1612; _LL1622: { int print_else;{
void* _temp1787=( void*) _temp1663->r; _LL1789: if( _temp1787 ==( void*) Cyc_Absyn_Skip_s){
goto _LL1790;} else{ goto _LL1791;} _LL1791: goto _LL1792; _LL1790: print_else=
0; goto _LL1788; _LL1792: print_else= 1; goto _LL1788; _LL1788:;} s=({ struct
Cyc_PP_Doc*(* _temp1793)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1796= Cyc_PP_text( _tag_arr("if (", sizeof( unsigned char), 5u)); struct
Cyc_PP_Doc* _temp1797= Cyc_Absynpp_exp2doc( _temp1667); struct Cyc_PP_Doc*
_temp1798= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc*
_temp1799= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp1816)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1819= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp1820= Cyc_Absynpp_stmt2doc( _temp1665); struct Cyc_PP_Doc* _temp1817[ 2u]={
_temp1819, _temp1820}; struct _tagged_arr _temp1818={( void*) _temp1817,( void*)
_temp1817,( void*)( _temp1817 + 2u)}; _temp1816( _temp1818);})); struct Cyc_PP_Doc*
_temp1800= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1801= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1802=
print_else?({ struct Cyc_PP_Doc*(* _temp1803)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1806= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1807=
Cyc_PP_text( _tag_arr("else {", sizeof( unsigned char), 7u)); struct Cyc_PP_Doc*
_temp1808= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp1811)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1814= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp1815= Cyc_Absynpp_stmt2doc( _temp1663); struct Cyc_PP_Doc* _temp1812[ 2u]={
_temp1814, _temp1815}; struct _tagged_arr _temp1813={( void*) _temp1812,( void*)
_temp1812,( void*)( _temp1812 + 2u)}; _temp1811( _temp1813);})); struct Cyc_PP_Doc*
_temp1809= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1810= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1804[ 5u]={
_temp1806, _temp1807, _temp1808, _temp1809, _temp1810}; struct _tagged_arr
_temp1805={( void*) _temp1804,( void*) _temp1804,( void*)( _temp1804 + 5u)};
_temp1803( _temp1805);}): Cyc_PP_nil_doc(); struct Cyc_PP_Doc* _temp1794[ 7u]={
_temp1796, _temp1797, _temp1798, _temp1799, _temp1800, _temp1801, _temp1802};
struct _tagged_arr _temp1795={( void*) _temp1794,( void*) _temp1794,( void*)(
_temp1794 + 7u)}; _temp1793( _temp1795);}); goto _LL1612;} _LL1624: s=({ struct
Cyc_PP_Doc*(* _temp1821)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1824= Cyc_PP_text( _tag_arr("while (", sizeof( unsigned char), 8u)); struct
Cyc_PP_Doc* _temp1825= Cyc_Absynpp_exp2doc( _temp1675); struct Cyc_PP_Doc*
_temp1826= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc*
_temp1827= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp1830)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1833= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp1834= Cyc_Absynpp_stmt2doc( _temp1669); struct Cyc_PP_Doc* _temp1831[ 2u]={
_temp1833, _temp1834}; struct _tagged_arr _temp1832={( void*) _temp1831,( void*)
_temp1831,( void*)( _temp1831 + 2u)}; _temp1830( _temp1832);})); struct Cyc_PP_Doc*
_temp1828= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1829= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1822[ 6u]={
_temp1824, _temp1825, _temp1826, _temp1827, _temp1828, _temp1829}; struct
_tagged_arr _temp1823={( void*) _temp1822,( void*) _temp1822,( void*)( _temp1822
+ 6u)}; _temp1821( _temp1823);}); goto _LL1612; _LL1626: s= Cyc_PP_text(
_tag_arr("break;", sizeof( unsigned char), 7u)); goto _LL1612; _LL1628: s= Cyc_PP_text(
_tag_arr("continue;", sizeof( unsigned char), 10u)); goto _LL1612; _LL1630: s=
Cyc_PP_text(( struct _tagged_arr)({ struct _tagged_arr _temp1835=* _temp1683;
xprintf("goto %.*s;", _get_arr_size( _temp1835, 1u), _temp1835.curr);})); goto
_LL1612; _LL1632: s=({ struct Cyc_PP_Doc*(* _temp1836)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1839= Cyc_PP_text( _tag_arr("for(", sizeof(
unsigned char), 5u)); struct Cyc_PP_Doc* _temp1840= Cyc_Absynpp_exp2doc(
_temp1699); struct Cyc_PP_Doc* _temp1841= Cyc_PP_text( _tag_arr("; ", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp1842= Cyc_Absynpp_exp2doc(
_temp1697); struct Cyc_PP_Doc* _temp1843= Cyc_PP_text( _tag_arr("; ", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp1844= Cyc_Absynpp_exp2doc(
_temp1691); struct Cyc_PP_Doc* _temp1845= Cyc_PP_text( _tag_arr(") {", sizeof(
unsigned char), 4u)); struct Cyc_PP_Doc* _temp1846= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(*
_temp1849)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1852= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1853= Cyc_Absynpp_stmt2doc( _temp1685); struct Cyc_PP_Doc*
_temp1850[ 2u]={ _temp1852, _temp1853}; struct _tagged_arr _temp1851={( void*)
_temp1850,( void*) _temp1850,( void*)( _temp1850 + 2u)}; _temp1849( _temp1851);}));
struct Cyc_PP_Doc* _temp1847= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1848=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1837[ 10u]={ _temp1839, _temp1840, _temp1841, _temp1842, _temp1843,
_temp1844, _temp1845, _temp1846, _temp1847, _temp1848}; struct _tagged_arr
_temp1838={( void*) _temp1837,( void*) _temp1837,( void*)( _temp1837 + 10u)};
_temp1836( _temp1838);}); goto _LL1612; _LL1634: s=({ struct Cyc_PP_Doc*(*
_temp1854)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1857= Cyc_PP_text(
_tag_arr("switch (", sizeof( unsigned char), 9u)); struct Cyc_PP_Doc* _temp1858=
Cyc_Absynpp_exp2doc( _temp1703); struct Cyc_PP_Doc* _temp1859= Cyc_PP_text(
_tag_arr(") {", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc* _temp1860= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1861= Cyc_Absynpp_switchclauses2doc( _temp1701); struct
Cyc_PP_Doc* _temp1862= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1863= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1855[ 7u]={
_temp1857, _temp1858, _temp1859, _temp1860, _temp1861, _temp1862, _temp1863};
struct _tagged_arr _temp1856={( void*) _temp1855,( void*) _temp1855,( void*)(
_temp1855 + 7u)}; _temp1854( _temp1856);}); goto _LL1612; _LL1636: s=({ struct
Cyc_PP_Doc*(* _temp1864)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1867= Cyc_PP_text( _tag_arr("switch \"C\" (", sizeof( unsigned char), 13u));
struct Cyc_PP_Doc* _temp1868= Cyc_Absynpp_exp2doc( _temp1707); struct Cyc_PP_Doc*
_temp1869= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc*
_temp1870= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1871= Cyc_Absynpp_switchCclauses2doc(
_temp1705); struct Cyc_PP_Doc* _temp1872= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp1873= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1865[ 7u]={ _temp1867, _temp1868, _temp1869, _temp1870, _temp1871,
_temp1872, _temp1873}; struct _tagged_arr _temp1866={( void*) _temp1865,( void*)
_temp1865,( void*)( _temp1865 + 7u)}; _temp1864( _temp1866);}); goto _LL1612;
_LL1638: s= Cyc_PP_text( _tag_arr("fallthru;", sizeof( unsigned char), 10u));
goto _LL1612; _LL1640: s=({ struct Cyc_PP_Doc*(* _temp1874)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1877= Cyc_PP_text( _tag_arr("fallthru(",
sizeof( unsigned char), 10u)); struct Cyc_PP_Doc* _temp1878= Cyc_Absynpp_exps2doc_prec(
20, _temp1715); struct Cyc_PP_Doc* _temp1879= Cyc_PP_text( _tag_arr(");",
sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1875[ 3u]={ _temp1877,
_temp1878, _temp1879}; struct _tagged_arr _temp1876={( void*) _temp1875,( void*)
_temp1875,( void*)( _temp1875 + 3u)}; _temp1874( _temp1876);}); goto _LL1612;
_LL1642: s=({ struct Cyc_PP_Doc*(* _temp1880)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1883= Cyc_Absynpp_decl2doc( _temp1719); struct Cyc_PP_Doc*
_temp1884= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1885= Cyc_Absynpp_stmt2doc(
_temp1717); struct Cyc_PP_Doc* _temp1881[ 3u]={ _temp1883, _temp1884, _temp1885};
struct _tagged_arr _temp1882={( void*) _temp1881,( void*) _temp1881,( void*)(
_temp1881 + 3u)}; _temp1880( _temp1882);}); goto _LL1612; _LL1644: s=({ struct
Cyc_PP_Doc*(* _temp1886)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1889= Cyc_PP_text( _tag_arr("cut ", sizeof( unsigned char), 5u)); struct
Cyc_PP_Doc* _temp1890= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1721)); struct
Cyc_PP_Doc* _temp1887[ 2u]={ _temp1889, _temp1890}; struct _tagged_arr _temp1888={(
void*) _temp1887,( void*) _temp1887,( void*)( _temp1887 + 2u)}; _temp1886(
_temp1888);}); goto _LL1612; _LL1646: s=({ struct Cyc_PP_Doc*(* _temp1891)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1894= Cyc_PP_text(
_tag_arr("splice ", sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp1895=
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1723)); struct Cyc_PP_Doc* _temp1892[
2u]={ _temp1894, _temp1895}; struct _tagged_arr _temp1893={( void*) _temp1892,(
void*) _temp1892,( void*)( _temp1892 + 2u)}; _temp1891( _temp1893);}); goto
_LL1612; _LL1648: if( Cyc_Absynpp_decls_first? Cyc_Absynpp_is_declaration(
_temp1725): 0){ s=({ struct Cyc_PP_Doc*(* _temp1896)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1899= Cyc_PP_textptr( _temp1727); struct Cyc_PP_Doc*
_temp1900= Cyc_PP_text( _tag_arr(": {", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc*
_temp1901= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1902= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1725)); struct Cyc_PP_Doc* _temp1903= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp1904= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1897[ 6u]={ _temp1899, _temp1900, _temp1901, _temp1902, _temp1903,
_temp1904}; struct _tagged_arr _temp1898={( void*) _temp1897,( void*) _temp1897,(
void*)( _temp1897 + 6u)}; _temp1896( _temp1898);});} else{ s=({ struct Cyc_PP_Doc*(*
_temp1905)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1908= Cyc_PP_textptr(
_temp1727); struct Cyc_PP_Doc* _temp1909= Cyc_PP_text( _tag_arr(": ", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp1910= Cyc_Absynpp_stmt2doc(
_temp1725); struct Cyc_PP_Doc* _temp1906[ 3u]={ _temp1908, _temp1909, _temp1910};
struct _tagged_arr _temp1907={( void*) _temp1906,( void*) _temp1906,( void*)(
_temp1906 + 3u)}; _temp1905( _temp1907);});} goto _LL1612; _LL1650: s=({ struct
Cyc_PP_Doc*(* _temp1911)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1914= Cyc_PP_text( _tag_arr("do {", sizeof( unsigned char), 5u)); struct
Cyc_PP_Doc* _temp1915= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1916= Cyc_PP_nest(
2, Cyc_Absynpp_stmt2doc( _temp1735)); struct Cyc_PP_Doc* _temp1917= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1918= Cyc_PP_text( _tag_arr("} while (", sizeof(
unsigned char), 10u)); struct Cyc_PP_Doc* _temp1919= Cyc_Absynpp_exp2doc(
_temp1733); struct Cyc_PP_Doc* _temp1920= Cyc_PP_text( _tag_arr(");", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp1912[ 7u]={ _temp1914, _temp1915,
_temp1916, _temp1917, _temp1918, _temp1919, _temp1920}; struct _tagged_arr
_temp1913={( void*) _temp1912,( void*) _temp1912,( void*)( _temp1912 + 7u)};
_temp1911( _temp1913);}); goto _LL1612; _LL1652: s=({ struct Cyc_PP_Doc*(*
_temp1921)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1924= Cyc_PP_text(
_tag_arr("try {", sizeof( unsigned char), 6u)); struct Cyc_PP_Doc* _temp1925=
Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1926= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1739)); struct Cyc_PP_Doc* _temp1927= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp1928= Cyc_PP_text( _tag_arr("} catch {", sizeof( unsigned char), 10u));
struct Cyc_PP_Doc* _temp1929= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1930=
Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc( _temp1737)); struct Cyc_PP_Doc*
_temp1931= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1932= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1922[ 9u]={
_temp1924, _temp1925, _temp1926, _temp1927, _temp1928, _temp1929, _temp1930,
_temp1931, _temp1932}; struct _tagged_arr _temp1923={( void*) _temp1922,( void*)
_temp1922,( void*)( _temp1922 + 9u)}; _temp1921( _temp1923);}); goto _LL1612;
_LL1654: s=({ struct Cyc_PP_Doc*(* _temp1933)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1936= Cyc_PP_text( _tag_arr("region<", sizeof(
unsigned char), 8u)); struct Cyc_PP_Doc* _temp1937= Cyc_PP_textptr( Cyc_Absynpp_get_name(
_temp1745)); struct Cyc_PP_Doc* _temp1938= Cyc_PP_text( _tag_arr(">", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1939= Cyc_Absynpp_qvar2doc(
_temp1743->name); struct Cyc_PP_Doc* _temp1940= Cyc_PP_text( _tag_arr("{",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1941= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1942= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1741));
struct Cyc_PP_Doc* _temp1943= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1944=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1934[ 9u]={ _temp1936, _temp1937, _temp1938, _temp1939, _temp1940,
_temp1941, _temp1942, _temp1943, _temp1944}; struct _tagged_arr _temp1935={(
void*) _temp1934,( void*) _temp1934,( void*)( _temp1934 + 9u)}; _temp1933(
_temp1935);}); goto _LL1612; _LL1612:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void* _temp1945=( void*) p->r;
int _temp1977; void* _temp1979; unsigned char _temp1981; struct _tagged_arr
_temp1983; struct Cyc_Absyn_Vardecl* _temp1985; struct Cyc_List_List* _temp1987;
struct Cyc_Absyn_Pat* _temp1989; struct Cyc_Absyn_Vardecl* _temp1991; struct
_tuple0* _temp1993; struct Cyc_List_List* _temp1995; struct Cyc_List_List*
_temp1997; struct _tuple0* _temp1999; struct Cyc_List_List* _temp2001; struct
Cyc_List_List* _temp2003; struct _tuple0* _temp2005; struct Cyc_List_List*
_temp2007; struct Cyc_List_List* _temp2009; struct Cyc_Core_Opt* _temp2011;
struct Cyc_Absyn_Structdecl* _temp2013; struct Cyc_Absyn_Enumfield* _temp2015;
struct Cyc_Absyn_Enumdecl* _temp2017; struct Cyc_List_List* _temp2019; struct
Cyc_List_List* _temp2021; struct Cyc_Absyn_Tunionfield* _temp2023; struct Cyc_Absyn_Tuniondecl*
_temp2025; _LL1947: if( _temp1945 ==( void*) Cyc_Absyn_Wild_p){ goto _LL1948;}
else{ goto _LL1949;} _LL1949: if( _temp1945 ==( void*) Cyc_Absyn_Null_p){ goto
_LL1950;} else{ goto _LL1951;} _LL1951: if(( unsigned int) _temp1945 > 2u?*((
int*) _temp1945) == Cyc_Absyn_Int_p: 0){ _LL1980: _temp1979=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1945)->f1; goto _LL1978; _LL1978: _temp1977=((
struct Cyc_Absyn_Int_p_struct*) _temp1945)->f2; goto _LL1952;} else{ goto
_LL1953;} _LL1953: if(( unsigned int) _temp1945 > 2u?*(( int*) _temp1945) == Cyc_Absyn_Char_p:
0){ _LL1982: _temp1981=(( struct Cyc_Absyn_Char_p_struct*) _temp1945)->f1; goto
_LL1954;} else{ goto _LL1955;} _LL1955: if(( unsigned int) _temp1945 > 2u?*((
int*) _temp1945) == Cyc_Absyn_Float_p: 0){ _LL1984: _temp1983=(( struct Cyc_Absyn_Float_p_struct*)
_temp1945)->f1; goto _LL1956;} else{ goto _LL1957;} _LL1957: if(( unsigned int)
_temp1945 > 2u?*(( int*) _temp1945) == Cyc_Absyn_Var_p: 0){ _LL1986: _temp1985=((
struct Cyc_Absyn_Var_p_struct*) _temp1945)->f1; goto _LL1958;} else{ goto
_LL1959;} _LL1959: if(( unsigned int) _temp1945 > 2u?*(( int*) _temp1945) == Cyc_Absyn_Tuple_p:
0){ _LL1988: _temp1987=(( struct Cyc_Absyn_Tuple_p_struct*) _temp1945)->f1; goto
_LL1960;} else{ goto _LL1961;} _LL1961: if(( unsigned int) _temp1945 > 2u?*((
int*) _temp1945) == Cyc_Absyn_Pointer_p: 0){ _LL1990: _temp1989=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1945)->f1; goto _LL1962;} else{ goto _LL1963;} _LL1963: if(( unsigned int)
_temp1945 > 2u?*(( int*) _temp1945) == Cyc_Absyn_Reference_p: 0){ _LL1992:
_temp1991=(( struct Cyc_Absyn_Reference_p_struct*) _temp1945)->f1; goto _LL1964;}
else{ goto _LL1965;} _LL1965: if(( unsigned int) _temp1945 > 2u?*(( int*)
_temp1945) == Cyc_Absyn_UnknownId_p: 0){ _LL1994: _temp1993=(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp1945)->f1; goto _LL1966;} else{ goto _LL1967;} _LL1967: if(( unsigned int)
_temp1945 > 2u?*(( int*) _temp1945) == Cyc_Absyn_UnknownCall_p: 0){ _LL2000:
_temp1999=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1945)->f1; goto
_LL1998; _LL1998: _temp1997=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1945)->f2;
goto _LL1996; _LL1996: _temp1995=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1945)->f3; goto _LL1968;} else{ goto _LL1969;} _LL1969: if(( unsigned int)
_temp1945 > 2u?*(( int*) _temp1945) == Cyc_Absyn_UnknownFields_p: 0){ _LL2006:
_temp2005=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp1945)->f1; goto
_LL2004; _LL2004: _temp2003=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1945)->f2; goto _LL2002; _LL2002: _temp2001=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1945)->f3; goto _LL1970;} else{ goto _LL1971;} _LL1971: if(( unsigned int)
_temp1945 > 2u?*(( int*) _temp1945) == Cyc_Absyn_Struct_p: 0){ _LL2014:
_temp2013=(( struct Cyc_Absyn_Struct_p_struct*) _temp1945)->f1; goto _LL2012;
_LL2012: _temp2011=(( struct Cyc_Absyn_Struct_p_struct*) _temp1945)->f2; goto
_LL2010; _LL2010: _temp2009=(( struct Cyc_Absyn_Struct_p_struct*) _temp1945)->f3;
goto _LL2008; _LL2008: _temp2007=(( struct Cyc_Absyn_Struct_p_struct*) _temp1945)->f4;
goto _LL1972;} else{ goto _LL1973;} _LL1973: if(( unsigned int) _temp1945 > 2u?*((
int*) _temp1945) == Cyc_Absyn_Enum_p: 0){ _LL2018: _temp2017=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1945)->f1; goto _LL2016; _LL2016: _temp2015=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1945)->f2; goto _LL1974;} else{ goto _LL1975;} _LL1975: if(( unsigned int)
_temp1945 > 2u?*(( int*) _temp1945) == Cyc_Absyn_Tunion_p: 0){ _LL2026:
_temp2025=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1945)->f1; goto _LL2024;
_LL2024: _temp2023=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1945)->f2; goto
_LL2022; _LL2022: _temp2021=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1945)->f3;
goto _LL2020; _LL2020: _temp2019=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1945)->f4;
goto _LL1976;} else{ goto _LL1946;} _LL1948: s= Cyc_PP_text( _tag_arr("_",
sizeof( unsigned char), 2u)); goto _LL1946; _LL1950: s= Cyc_PP_text( _tag_arr("null",
sizeof( unsigned char), 5u)); goto _LL1946; _LL1952: if( _temp1979 ==( void*)
Cyc_Absyn_Signed){ s= Cyc_PP_text(( struct _tagged_arr) xprintf("%d", _temp1977));}
else{ s= Cyc_PP_text(( struct _tagged_arr) xprintf("%u",( unsigned int)
_temp1977));} goto _LL1946; _LL1954: s= Cyc_PP_text(( struct _tagged_arr)({
struct _tagged_arr _temp2027= Cyc_Absynpp_char_escape( _temp1981); xprintf("'%.*s'",
_get_arr_size( _temp2027, 1u), _temp2027.curr);})); goto _LL1946; _LL1956: s=
Cyc_PP_text( _temp1983); goto _LL1946; _LL1958: s= Cyc_Absynpp_qvar2doc(
_temp1985->name); goto _LL1946; _LL1960: s=({ struct Cyc_PP_Doc*(* _temp2028)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2031= Cyc_PP_text(
_tag_arr("$(", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp2032=((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Pat*), struct
_tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc,
_tag_arr(",", sizeof( unsigned char), 2u), _temp1987); struct Cyc_PP_Doc*
_temp2033= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp2029[ 3u]={ _temp2031, _temp2032, _temp2033}; struct _tagged_arr _temp2030={(
void*) _temp2029,( void*) _temp2029,( void*)( _temp2029 + 3u)}; _temp2028(
_temp2030);}); goto _LL1946; _LL1962: s=({ struct Cyc_PP_Doc*(* _temp2034)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2037= Cyc_PP_text(
_tag_arr("&", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp2038= Cyc_Absynpp_pat2doc(
_temp1989); struct Cyc_PP_Doc* _temp2035[ 2u]={ _temp2037, _temp2038}; struct
_tagged_arr _temp2036={( void*) _temp2035,( void*) _temp2035,( void*)( _temp2035
+ 2u)}; _temp2034( _temp2036);}); goto _LL1946; _LL1964: s=({ struct Cyc_PP_Doc*(*
_temp2039)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2042= Cyc_PP_text(
_tag_arr("*", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp2043= Cyc_Absynpp_qvar2doc(
_temp1991->name); struct Cyc_PP_Doc* _temp2040[ 2u]={ _temp2042, _temp2043};
struct _tagged_arr _temp2041={( void*) _temp2040,( void*) _temp2040,( void*)(
_temp2040 + 2u)}; _temp2039( _temp2041);}); goto _LL1946; _LL1966: s= Cyc_Absynpp_qvar2doc(
_temp1993); goto _LL1946; _LL1968: s=({ struct Cyc_PP_Doc*(* _temp2044)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2047= Cyc_Absynpp_qvar2doc(
_temp1999); struct Cyc_PP_Doc* _temp2048= Cyc_Absynpp_tvars2doc( _temp1997);
struct Cyc_PP_Doc* _temp2049= Cyc_PP_group( _tag_arr("(", sizeof( unsigned char),
2u), _tag_arr(")", sizeof( unsigned char), 2u), _tag_arr(",", sizeof(
unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc,
_temp1995)); struct Cyc_PP_Doc* _temp2045[ 3u]={ _temp2047, _temp2048, _temp2049};
struct _tagged_arr _temp2046={( void*) _temp2045,( void*) _temp2045,( void*)(
_temp2045 + 3u)}; _temp2044( _temp2046);}); goto _LL1946; _LL1970: s=({ struct
Cyc_PP_Doc*(* _temp2050)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2053= Cyc_Absynpp_qvar2doc( _temp2005); struct Cyc_PP_Doc* _temp2054= Cyc_Absynpp_tvars2doc(
_temp2003); struct Cyc_PP_Doc* _temp2055= Cyc_PP_group( _tag_arr("{", sizeof(
unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp2001)); struct Cyc_PP_Doc* _temp2051[ 3u]={ _temp2053, _temp2054, _temp2055};
struct _tagged_arr _temp2052={( void*) _temp2051,( void*) _temp2051,( void*)(
_temp2051 + 3u)}; _temp2050( _temp2052);}); goto _LL1946; _LL1972: s=({ struct
Cyc_PP_Doc*(* _temp2056)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2059= _temp2013->name == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp2013->name))->v); struct Cyc_PP_Doc*
_temp2060= Cyc_Absynpp_tvars2doc( _temp2009); struct Cyc_PP_Doc* _temp2061= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_dp2doc, _temp2007)); struct Cyc_PP_Doc* _temp2057[ 3u]={ _temp2059,
_temp2060, _temp2061}; struct _tagged_arr _temp2058={( void*) _temp2057,( void*)
_temp2057,( void*)( _temp2057 + 3u)}; _temp2056( _temp2058);}); goto _LL1946;
_LL1974: s= Cyc_Absynpp_qvar2doc( _temp2015->name); goto _LL1946; _LL1976: if(
_temp2019 == 0){ s= Cyc_Absynpp_qvar2doc( _temp2023->name);} else{ s=({ struct
Cyc_PP_Doc*(* _temp2062)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2065= Cyc_Absynpp_qvar2doc( _temp2023->name); struct Cyc_PP_Doc* _temp2066=
Cyc_Absynpp_tvars2doc( _temp2021); struct Cyc_PP_Doc* _temp2067= Cyc_PP_egroup(
_tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_pat2doc, _temp2019)); struct Cyc_PP_Doc* _temp2063[ 3u]={ _temp2065,
_temp2066, _temp2067}; struct _tagged_arr _temp2064={( void*) _temp2063,( void*)
_temp2063,( void*)( _temp2063 + 3u)}; _temp2062( _temp2064);});} goto _LL1946;
_LL1946:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct _tuple5* dp){
return({ struct Cyc_PP_Doc*(* _temp2068)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2071= Cyc_PP_egroup( _tag_arr("", sizeof( unsigned char),
1u), _tag_arr("=", sizeof( unsigned char), 2u), _tag_arr("=", sizeof(
unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(* dp).f1));
struct Cyc_PP_Doc* _temp2072= Cyc_Absynpp_pat2doc((* dp).f2); struct Cyc_PP_Doc*
_temp2069[ 2u]={ _temp2071, _temp2072}; struct _tagged_arr _temp2070={( void*)
_temp2069,( void*) _temp2069,( void*)( _temp2069 + 2u)}; _temp2068( _temp2070);});}
struct Cyc_PP_Doc* Cyc_Absynpp_switchclause2doc( struct Cyc_Absyn_Switch_clause*
c){ if( c->where_clause == 0?( void*)( c->pattern)->r ==( void*) Cyc_Absyn_Wild_p:
0){ return({ struct Cyc_PP_Doc*(* _temp2073)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2076= Cyc_PP_text( _tag_arr("default: ", sizeof(
unsigned char), 10u)); struct Cyc_PP_Doc* _temp2077= Cyc_PP_nest( 2,({ struct
Cyc_PP_Doc*(* _temp2078)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2081= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2082= Cyc_Absynpp_stmt2doc(
c->body); struct Cyc_PP_Doc* _temp2079[ 2u]={ _temp2081, _temp2082}; struct
_tagged_arr _temp2080={( void*) _temp2079,( void*) _temp2079,( void*)( _temp2079
+ 2u)}; _temp2078( _temp2080);})); struct Cyc_PP_Doc* _temp2074[ 2u]={ _temp2076,
_temp2077}; struct _tagged_arr _temp2075={( void*) _temp2074,( void*) _temp2074,(
void*)( _temp2074 + 2u)}; _temp2073( _temp2075);});} else{ if( c->where_clause
== 0){ return({ struct Cyc_PP_Doc*(* _temp2083)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2086= Cyc_PP_text( _tag_arr("case ", sizeof(
unsigned char), 6u)); struct Cyc_PP_Doc* _temp2087= Cyc_Absynpp_pat2doc( c->pattern);
struct Cyc_PP_Doc* _temp2088= Cyc_PP_text( _tag_arr(": ", sizeof( unsigned char),
3u)); struct Cyc_PP_Doc* _temp2089= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(*
_temp2090)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2093= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2094= Cyc_Absynpp_stmt2doc( c->body); struct Cyc_PP_Doc*
_temp2091[ 2u]={ _temp2093, _temp2094}; struct _tagged_arr _temp2092={( void*)
_temp2091,( void*) _temp2091,( void*)( _temp2091 + 2u)}; _temp2090( _temp2092);}));
struct Cyc_PP_Doc* _temp2084[ 4u]={ _temp2086, _temp2087, _temp2088, _temp2089};
struct _tagged_arr _temp2085={( void*) _temp2084,( void*) _temp2084,( void*)(
_temp2084 + 4u)}; _temp2083( _temp2085);});} else{ return({ struct Cyc_PP_Doc*(*
_temp2095)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2098= Cyc_PP_text(
_tag_arr("case ", sizeof( unsigned char), 6u)); struct Cyc_PP_Doc* _temp2099=
Cyc_Absynpp_pat2doc( c->pattern); struct Cyc_PP_Doc* _temp2100= Cyc_PP_text(
_tag_arr(" && ", sizeof( unsigned char), 5u)); struct Cyc_PP_Doc* _temp2101= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( c->where_clause)); struct Cyc_PP_Doc*
_temp2102= Cyc_PP_text( _tag_arr(": ", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc*
_temp2103= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2104)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp2107= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2108= Cyc_Absynpp_stmt2doc( c->body); struct Cyc_PP_Doc* _temp2105[ 2u]={
_temp2107, _temp2108}; struct _tagged_arr _temp2106={( void*) _temp2105,( void*)
_temp2105,( void*)( _temp2105 + 2u)}; _temp2104( _temp2106);})); struct Cyc_PP_Doc*
_temp2096[ 6u]={ _temp2098, _temp2099, _temp2100, _temp2101, _temp2102,
_temp2103}; struct _tagged_arr _temp2097={( void*) _temp2096,( void*) _temp2096,(
void*)( _temp2096 + 6u)}; _temp2095( _temp2097);});}}} struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Switch_clause*), struct _tagged_arr sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc, _tag_arr("", sizeof(
unsigned char), 1u), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_switchCclause2doc(
struct Cyc_Absyn_SwitchC_clause* c){ struct Cyc_Absyn_SwitchC_clause _temp2111;
struct Cyc_Position_Segment* _temp2112; struct Cyc_Absyn_Stmt* _temp2114; struct
Cyc_Absyn_Exp* _temp2116; struct Cyc_Absyn_SwitchC_clause* _temp2109= c;
_temp2111=* _temp2109; _LL2117: _temp2116= _temp2111.cnst_exp; goto _LL2115;
_LL2115: _temp2114= _temp2111.body; goto _LL2113; _LL2113: _temp2112= _temp2111.loc;
goto _LL2110; _LL2110: if( _temp2116 == 0){ return({ struct Cyc_PP_Doc*(*
_temp2118)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2121= Cyc_PP_text(
_tag_arr("default: ", sizeof( unsigned char), 10u)); struct Cyc_PP_Doc*
_temp2122= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2123)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp2126= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2127= Cyc_Absynpp_stmt2doc( c->body); struct Cyc_PP_Doc* _temp2124[ 2u]={
_temp2126, _temp2127}; struct _tagged_arr _temp2125={( void*) _temp2124,( void*)
_temp2124,( void*)( _temp2124 + 2u)}; _temp2123( _temp2125);})); struct Cyc_PP_Doc*
_temp2119[ 2u]={ _temp2121, _temp2122}; struct _tagged_arr _temp2120={( void*)
_temp2119,( void*) _temp2119,( void*)( _temp2119 + 2u)}; _temp2118( _temp2120);});}
else{ return({ struct Cyc_PP_Doc*(* _temp2128)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2131= Cyc_PP_text( _tag_arr("case ", sizeof(
unsigned char), 6u)); struct Cyc_PP_Doc* _temp2132= Cyc_Absynpp_exp2doc(( struct
Cyc_Absyn_Exp*) _check_null( _temp2116)); struct Cyc_PP_Doc* _temp2133= Cyc_PP_text(
_tag_arr(": ", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp2134= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc*(* _temp2135)( struct _tagged_arr)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp2138= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2139= Cyc_Absynpp_stmt2doc(
_temp2114); struct Cyc_PP_Doc* _temp2136[ 2u]={ _temp2138, _temp2139}; struct
_tagged_arr _temp2137={( void*) _temp2136,( void*) _temp2136,( void*)( _temp2136
+ 2u)}; _temp2135( _temp2137);})); struct Cyc_PP_Doc* _temp2129[ 4u]={ _temp2131,
_temp2132, _temp2133, _temp2134}; struct _tagged_arr _temp2130={( void*)
_temp2129,( void*) _temp2129,( void*)( _temp2129 + 4u)}; _temp2128( _temp2130);});}}
struct Cyc_PP_Doc* Cyc_Absynpp_switchCclauses2doc( struct Cyc_List_List* cs){
return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_SwitchC_clause*),
struct _tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchCclause2doc,
_tag_arr("", sizeof( unsigned char), 1u), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_enumfield2doc(
struct Cyc_Absyn_Enumfield* f){ if( f->tag == 0){ return Cyc_Absynpp_qvar2doc( f->name);}
else{ return({ struct Cyc_PP_Doc*(* _temp2140)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2143= Cyc_Absynpp_qvar2doc( f->name); struct Cyc_PP_Doc*
_temp2144= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc*
_temp2145= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( f->tag));
struct Cyc_PP_Doc* _temp2141[ 3u]={ _temp2143, _temp2144, _temp2145}; struct
_tagged_arr _temp2142={( void*) _temp2141,( void*) _temp2141,( void*)( _temp2141
+ 3u)}; _temp2140( _temp2142);});}} struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc(
struct Cyc_List_List* fs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Enumfield*), struct _tagged_arr sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc, _tag_arr(",", sizeof(
unsigned char), 2u), fs);} static struct Cyc_PP_Doc* Cyc_Absynpp_id2doc( struct
Cyc_Absyn_Vardecl* v){ return Cyc_Absynpp_qvar2doc( v->name);} static struct Cyc_PP_Doc*
Cyc_Absynpp_ids2doc( struct Cyc_List_List* vds){ return(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Vardecl*), struct _tagged_arr sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseq)( Cyc_Absynpp_id2doc, _tag_arr(",",
sizeof( unsigned char), 2u), vds);} struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl* d){ struct Cyc_PP_Doc* s;{ void* _temp2146=( void*) d->r;
struct Cyc_Absyn_Fndecl* _temp2172; struct Cyc_Absyn_Structdecl* _temp2174;
struct Cyc_Absyn_Uniondecl* _temp2176; struct Cyc_Absyn_Vardecl* _temp2178;
struct Cyc_Absyn_Vardecl _temp2180; struct Cyc_List_List* _temp2181; struct Cyc_Core_Opt*
_temp2183; struct Cyc_Absyn_Exp* _temp2185; void* _temp2187; struct Cyc_Absyn_Tqual
_temp2189; struct _tuple0* _temp2191; void* _temp2193; struct Cyc_Absyn_Tuniondecl*
_temp2195; struct Cyc_Absyn_Tuniondecl _temp2197; int _temp2198; struct Cyc_Core_Opt*
_temp2200; struct Cyc_List_List* _temp2202; struct _tuple0* _temp2204; void*
_temp2206; int _temp2208; struct Cyc_Absyn_Exp* _temp2210; struct Cyc_Core_Opt*
_temp2212; struct Cyc_Core_Opt* _temp2214; struct Cyc_Absyn_Pat* _temp2216;
struct Cyc_List_List* _temp2218; struct Cyc_Absyn_Enumdecl* _temp2220; struct
Cyc_Absyn_Enumdecl _temp2222; struct Cyc_Core_Opt* _temp2223; struct _tuple0*
_temp2225; void* _temp2227; struct Cyc_Absyn_Typedefdecl* _temp2229; struct Cyc_List_List*
_temp2231; struct _tagged_arr* _temp2233; struct Cyc_List_List* _temp2235;
struct _tuple0* _temp2237; struct Cyc_List_List* _temp2239; _LL2148: if(*(( int*)
_temp2146) == Cyc_Absyn_Fn_d){ _LL2173: _temp2172=(( struct Cyc_Absyn_Fn_d_struct*)
_temp2146)->f1; goto _LL2149;} else{ goto _LL2150;} _LL2150: if(*(( int*)
_temp2146) == Cyc_Absyn_Struct_d){ _LL2175: _temp2174=(( struct Cyc_Absyn_Struct_d_struct*)
_temp2146)->f1; goto _LL2151;} else{ goto _LL2152;} _LL2152: if(*(( int*)
_temp2146) == Cyc_Absyn_Union_d){ _LL2177: _temp2176=(( struct Cyc_Absyn_Union_d_struct*)
_temp2146)->f1; goto _LL2153;} else{ goto _LL2154;} _LL2154: if(*(( int*)
_temp2146) == Cyc_Absyn_Var_d){ _LL2179: _temp2178=(( struct Cyc_Absyn_Var_d_struct*)
_temp2146)->f1; _temp2180=* _temp2178; _LL2194: _temp2193=( void*) _temp2180.sc;
goto _LL2192; _LL2192: _temp2191= _temp2180.name; goto _LL2190; _LL2190:
_temp2189= _temp2180.tq; goto _LL2188; _LL2188: _temp2187=( void*) _temp2180.type;
goto _LL2186; _LL2186: _temp2185= _temp2180.initializer; goto _LL2184; _LL2184:
_temp2183= _temp2180.rgn; goto _LL2182; _LL2182: _temp2181= _temp2180.attributes;
goto _LL2155;} else{ goto _LL2156;} _LL2156: if(*(( int*) _temp2146) == Cyc_Absyn_Tunion_d){
_LL2196: _temp2195=(( struct Cyc_Absyn_Tunion_d_struct*) _temp2146)->f1;
_temp2197=* _temp2195; _LL2207: _temp2206=( void*) _temp2197.sc; goto _LL2205;
_LL2205: _temp2204= _temp2197.name; goto _LL2203; _LL2203: _temp2202= _temp2197.tvs;
goto _LL2201; _LL2201: _temp2200= _temp2197.fields; goto _LL2199; _LL2199:
_temp2198= _temp2197.is_xtunion; goto _LL2157;} else{ goto _LL2158;} _LL2158:
if(*(( int*) _temp2146) == Cyc_Absyn_Let_d){ _LL2217: _temp2216=(( struct Cyc_Absyn_Let_d_struct*)
_temp2146)->f1; goto _LL2215; _LL2215: _temp2214=(( struct Cyc_Absyn_Let_d_struct*)
_temp2146)->f2; goto _LL2213; _LL2213: _temp2212=(( struct Cyc_Absyn_Let_d_struct*)
_temp2146)->f3; goto _LL2211; _LL2211: _temp2210=(( struct Cyc_Absyn_Let_d_struct*)
_temp2146)->f4; goto _LL2209; _LL2209: _temp2208=(( struct Cyc_Absyn_Let_d_struct*)
_temp2146)->f5; goto _LL2159;} else{ goto _LL2160;} _LL2160: if(*(( int*)
_temp2146) == Cyc_Absyn_Letv_d){ _LL2219: _temp2218=(( struct Cyc_Absyn_Letv_d_struct*)
_temp2146)->f1; goto _LL2161;} else{ goto _LL2162;} _LL2162: if(*(( int*)
_temp2146) == Cyc_Absyn_Enum_d){ _LL2221: _temp2220=(( struct Cyc_Absyn_Enum_d_struct*)
_temp2146)->f1; _temp2222=* _temp2220; _LL2228: _temp2227=( void*) _temp2222.sc;
goto _LL2226; _LL2226: _temp2225= _temp2222.name; goto _LL2224; _LL2224:
_temp2223= _temp2222.fields; goto _LL2163;} else{ goto _LL2164;} _LL2164: if(*((
int*) _temp2146) == Cyc_Absyn_Typedef_d){ _LL2230: _temp2229=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2146)->f1; goto _LL2165;} else{ goto _LL2166;} _LL2166: if(*(( int*)
_temp2146) == Cyc_Absyn_Namespace_d){ _LL2234: _temp2233=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2146)->f1; goto _LL2232; _LL2232: _temp2231=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2146)->f2; goto _LL2167;} else{ goto _LL2168;} _LL2168: if(*(( int*)
_temp2146) == Cyc_Absyn_Using_d){ _LL2238: _temp2237=(( struct Cyc_Absyn_Using_d_struct*)
_temp2146)->f1; goto _LL2236; _LL2236: _temp2235=(( struct Cyc_Absyn_Using_d_struct*)
_temp2146)->f2; goto _LL2169;} else{ goto _LL2170;} _LL2170: if(*(( int*)
_temp2146) == Cyc_Absyn_ExternC_d){ _LL2240: _temp2239=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2146)->f1; goto _LL2171;} else{ goto _LL2147;} _LL2149: { void* t=( void*)({
struct Cyc_Absyn_FnType_struct* _temp2272=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2272[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2273; _temp2273.tag= Cyc_Absyn_FnType; _temp2273.f1=({ struct Cyc_Absyn_FnInfo
_temp2274; _temp2274.tvars= _temp2172->tvs; _temp2274.effect= _temp2172->effect;
_temp2274.ret_typ=( void*)(( void*) _temp2172->ret_type); _temp2274.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp2172->args); _temp2274.c_varargs=
_temp2172->c_varargs; _temp2274.cyc_varargs= _temp2172->cyc_varargs; _temp2274.rgn_po=
_temp2172->rgn_po; _temp2274.attributes= 0; _temp2274;}); _temp2273;});
_temp2272;}); struct Cyc_PP_Doc* attsdoc= Cyc_Absynpp_atts2doc( _temp2172->attributes);
struct Cyc_PP_Doc* inlinedoc; if( _temp2172->is_inline){ if( Cyc_Absynpp_to_VC){
inlinedoc= Cyc_PP_text( _tag_arr("__inline ", sizeof( unsigned char), 10u));}
else{ inlinedoc= Cyc_PP_text( _tag_arr("inline ", sizeof( unsigned char), 8u));}}
else{ inlinedoc= Cyc_PP_nil_doc();}{ struct Cyc_PP_Doc* scopedoc= Cyc_Absynpp_scope2doc((
void*) _temp2172->sc); struct Cyc_PP_Doc* beforenamedoc= Cyc_Absynpp_to_VC? Cyc_Absynpp_callconv2doc(
_temp2172->attributes): attsdoc; struct Cyc_PP_Doc* namedoc= Cyc_Absynpp_typedef_name2doc(
_temp2172->name); struct Cyc_PP_Doc* tqtddoc= Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual
_temp2265; _temp2265.q_const= 0; _temp2265.q_volatile= 0; _temp2265.q_restrict=
0; _temp2265;}), t,({ struct Cyc_Core_Opt* _temp2266=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2266->v=( void*)({ struct Cyc_PP_Doc*(*
_temp2267)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2270=
beforenamedoc; struct Cyc_PP_Doc* _temp2271= namedoc; struct Cyc_PP_Doc*
_temp2268[ 2u]={ _temp2270, _temp2271}; struct _tagged_arr _temp2269={( void*)
_temp2268,( void*) _temp2268,( void*)( _temp2268 + 2u)}; _temp2267( _temp2269);});
_temp2266;})); struct Cyc_PP_Doc* bodydoc=({ struct Cyc_PP_Doc*(* _temp2253)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2256= Cyc_PP_text(
_tag_arr(" {", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp2257= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc*(* _temp2260)( struct _tagged_arr)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp2263= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2264= Cyc_Absynpp_stmt2doc(
_temp2172->body); struct Cyc_PP_Doc* _temp2261[ 2u]={ _temp2263, _temp2264};
struct _tagged_arr _temp2262={( void*) _temp2261,( void*) _temp2261,( void*)(
_temp2261 + 2u)}; _temp2260( _temp2262);})); struct Cyc_PP_Doc* _temp2258= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2259= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp2254[ 4u]={ _temp2256, _temp2257, _temp2258,
_temp2259}; struct _tagged_arr _temp2255={( void*) _temp2254,( void*) _temp2254,(
void*)( _temp2254 + 4u)}; _temp2253( _temp2255);}); s=({ struct Cyc_PP_Doc*(*
_temp2241)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2244=
inlinedoc; struct Cyc_PP_Doc* _temp2245= scopedoc; struct Cyc_PP_Doc* _temp2246=
tqtddoc; struct Cyc_PP_Doc* _temp2247= bodydoc; struct Cyc_PP_Doc* _temp2242[ 4u]={
_temp2244, _temp2245, _temp2246, _temp2247}; struct _tagged_arr _temp2243={(
void*) _temp2242,( void*) _temp2242,( void*)( _temp2242 + 4u)}; _temp2241(
_temp2243);}); if( Cyc_Absynpp_to_VC){ s=({ struct Cyc_PP_Doc*(* _temp2248)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2251= attsdoc; struct
Cyc_PP_Doc* _temp2252= s; struct Cyc_PP_Doc* _temp2249[ 2u]={ _temp2251,
_temp2252}; struct _tagged_arr _temp2250={( void*) _temp2249,( void*) _temp2249,(
void*)( _temp2249 + 2u)}; _temp2248( _temp2250);});} goto _LL2147;}} _LL2151:
if( _temp2174->fields == 0){ s=({ struct Cyc_PP_Doc*(* _temp2275)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2278= Cyc_Absynpp_scope2doc((
void*) _temp2174->sc); struct Cyc_PP_Doc* _temp2279= Cyc_PP_text( _tag_arr("struct ",
sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp2280= _temp2174->name == 0?
Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_typedef_name2doc((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp2174->name))->v);
struct Cyc_PP_Doc* _temp2281= Cyc_Absynpp_ktvars2doc( _temp2174->tvs); struct
Cyc_PP_Doc* _temp2282= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u));
struct Cyc_PP_Doc* _temp2276[ 5u]={ _temp2278, _temp2279, _temp2280, _temp2281,
_temp2282}; struct _tagged_arr _temp2277={( void*) _temp2276,( void*) _temp2276,(
void*)( _temp2276 + 5u)}; _temp2275( _temp2277);});} else{ s=({ struct Cyc_PP_Doc*(*
_temp2283)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2286= Cyc_Absynpp_scope2doc((
void*) _temp2174->sc); struct Cyc_PP_Doc* _temp2287= Cyc_PP_text( _tag_arr("struct ",
sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp2288= _temp2174->name == 0?
Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_typedef_name2doc((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp2174->name))->v);
struct Cyc_PP_Doc* _temp2289= Cyc_Absynpp_ktvars2doc( _temp2174->tvs); struct
Cyc_PP_Doc* _temp2290= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u));
struct Cyc_PP_Doc* _temp2291= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2296)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2299= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2300= Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp2174->fields))->v); struct Cyc_PP_Doc*
_temp2297[ 2u]={ _temp2299, _temp2300}; struct _tagged_arr _temp2298={( void*)
_temp2297,( void*) _temp2297,( void*)( _temp2297 + 2u)}; _temp2296( _temp2298);}));
struct Cyc_PP_Doc* _temp2292= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2293=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp2294= Cyc_Absynpp_atts2doc( _temp2174->attributes); struct Cyc_PP_Doc*
_temp2295= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp2284[ 10u]={ _temp2286, _temp2287, _temp2288, _temp2289, _temp2290,
_temp2291, _temp2292, _temp2293, _temp2294, _temp2295}; struct _tagged_arr
_temp2285={( void*) _temp2284,( void*) _temp2284,( void*)( _temp2284 + 10u)};
_temp2283( _temp2285);});} goto _LL2147; _LL2153: if( _temp2176->fields == 0){ s=({
struct Cyc_PP_Doc*(* _temp2301)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2304= Cyc_Absynpp_scope2doc(( void*) _temp2176->sc); struct Cyc_PP_Doc*
_temp2305= Cyc_PP_text( _tag_arr("union ", sizeof( unsigned char), 7u)); struct
Cyc_PP_Doc* _temp2306= _temp2176->name == 0? Cyc_PP_text( _tag_arr("", sizeof(
unsigned char), 1u)): Cyc_Absynpp_qvar2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp2176->name))->v); struct Cyc_PP_Doc* _temp2307= Cyc_Absynpp_tvars2doc(
_temp2176->tvs); struct Cyc_PP_Doc* _temp2308= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp2302[ 5u]={ _temp2304,
_temp2305, _temp2306, _temp2307, _temp2308}; struct _tagged_arr _temp2303={(
void*) _temp2302,( void*) _temp2302,( void*)( _temp2302 + 5u)}; _temp2301(
_temp2303);});} else{ s=({ struct Cyc_PP_Doc*(* _temp2309)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp2312= Cyc_Absynpp_scope2doc(( void*)
_temp2176->sc); struct Cyc_PP_Doc* _temp2313= Cyc_PP_text( _tag_arr("union ",
sizeof( unsigned char), 7u)); struct Cyc_PP_Doc* _temp2314= _temp2176->name == 0?
Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_qvar2doc((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp2176->name))->v);
struct Cyc_PP_Doc* _temp2315= Cyc_Absynpp_tvars2doc( _temp2176->tvs); struct Cyc_PP_Doc*
_temp2316= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc*
_temp2317= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2322)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp2325= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2326= Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp2176->fields))->v); struct Cyc_PP_Doc* _temp2323[ 2u]={
_temp2325, _temp2326}; struct _tagged_arr _temp2324={( void*) _temp2323,( void*)
_temp2323,( void*)( _temp2323 + 2u)}; _temp2322( _temp2324);})); struct Cyc_PP_Doc*
_temp2318= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2319= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp2320= Cyc_Absynpp_atts2doc(
_temp2176->attributes); struct Cyc_PP_Doc* _temp2321= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp2310[ 10u]={ _temp2312,
_temp2313, _temp2314, _temp2315, _temp2316, _temp2317, _temp2318, _temp2319,
_temp2320, _temp2321}; struct _tagged_arr _temp2311={( void*) _temp2310,( void*)
_temp2310,( void*)( _temp2310 + 10u)}; _temp2309( _temp2311);});} goto _LL2147;
_LL2155: { struct Cyc_PP_Doc* sn= Cyc_Absynpp_typedef_name2doc( _temp2191); s=({
struct Cyc_PP_Doc*(* _temp2327)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2330= Cyc_Absynpp_to_VC? Cyc_Absynpp_atts2doc( _temp2181): Cyc_PP_nil_doc();
struct Cyc_PP_Doc* _temp2331= Cyc_Absynpp_scope2doc( _temp2193); struct Cyc_PP_Doc*
_temp2332= Cyc_Absynpp_tqtd2doc( _temp2189, _temp2187,({ struct Cyc_Core_Opt*
_temp2341=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2341->v=( void*) sn; _temp2341;})); struct Cyc_PP_Doc* _temp2333= ! Cyc_Absynpp_to_VC?
Cyc_Absynpp_atts2doc( _temp2181): Cyc_PP_nil_doc(); struct Cyc_PP_Doc* _temp2334=
_temp2185 == 0? Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)):({ struct
Cyc_PP_Doc*(* _temp2336)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2339= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc*
_temp2340= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp2185));
struct Cyc_PP_Doc* _temp2337[ 2u]={ _temp2339, _temp2340}; struct _tagged_arr
_temp2338={( void*) _temp2337,( void*) _temp2337,( void*)( _temp2337 + 2u)};
_temp2336( _temp2338);}); struct Cyc_PP_Doc* _temp2335= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp2328[ 6u]={ _temp2330,
_temp2331, _temp2332, _temp2333, _temp2334, _temp2335}; struct _tagged_arr
_temp2329={( void*) _temp2328,( void*) _temp2328,( void*)( _temp2328 + 6u)};
_temp2327( _temp2329);}); goto _LL2147;} _LL2157: if( _temp2200 == 0){ s=({
struct Cyc_PP_Doc*(* _temp2342)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2345= Cyc_Absynpp_scope2doc( _temp2206); struct Cyc_PP_Doc* _temp2346=
_temp2198? Cyc_PP_text( _tag_arr("xtunion ", sizeof( unsigned char), 9u)): Cyc_PP_text(
_tag_arr("tunion ", sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp2347=
_temp2198? Cyc_Absynpp_qvar2doc( _temp2204): Cyc_Absynpp_typedef_name2doc(
_temp2204); struct Cyc_PP_Doc* _temp2348= Cyc_Absynpp_ktvars2doc( _temp2202);
struct Cyc_PP_Doc* _temp2349= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp2343[ 5u]={ _temp2345, _temp2346, _temp2347,
_temp2348, _temp2349}; struct _tagged_arr _temp2344={( void*) _temp2343,( void*)
_temp2343,( void*)( _temp2343 + 5u)}; _temp2342( _temp2344);});} else{ s=({
struct Cyc_PP_Doc*(* _temp2350)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2353= Cyc_Absynpp_scope2doc( _temp2206); struct Cyc_PP_Doc* _temp2354=
_temp2198? Cyc_PP_text( _tag_arr("xtunion ", sizeof( unsigned char), 9u)): Cyc_PP_text(
_tag_arr("tunion ", sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp2355=
_temp2198? Cyc_Absynpp_qvar2doc( _temp2204): Cyc_Absynpp_typedef_name2doc(
_temp2204); struct Cyc_PP_Doc* _temp2356= Cyc_Absynpp_ktvars2doc( _temp2202);
struct Cyc_PP_Doc* _temp2357= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char),
3u)); struct Cyc_PP_Doc* _temp2358= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(*
_temp2361)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2364= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2365= Cyc_Absynpp_tunionfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp2200))->v); struct Cyc_PP_Doc* _temp2362[
2u]={ _temp2364, _temp2365}; struct _tagged_arr _temp2363={( void*) _temp2362,(
void*) _temp2362,( void*)( _temp2362 + 2u)}; _temp2361( _temp2363);})); struct
Cyc_PP_Doc* _temp2359= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2360= Cyc_PP_text(
_tag_arr("};", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp2351[ 8u]={
_temp2353, _temp2354, _temp2355, _temp2356, _temp2357, _temp2358, _temp2359,
_temp2360}; struct _tagged_arr _temp2352={( void*) _temp2351,( void*) _temp2351,(
void*)( _temp2351 + 8u)}; _temp2350( _temp2352);});} goto _LL2147; _LL2159: s=({
struct Cyc_PP_Doc*(* _temp2366)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2369= Cyc_PP_text( _tag_arr("let ", sizeof( unsigned char), 5u)); struct
Cyc_PP_Doc* _temp2370= Cyc_Absynpp_pat2doc( _temp2216); struct Cyc_PP_Doc*
_temp2371= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc*
_temp2372= Cyc_Absynpp_exp2doc( _temp2210); struct Cyc_PP_Doc* _temp2373= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp2367[ 5u]={
_temp2369, _temp2370, _temp2371, _temp2372, _temp2373}; struct _tagged_arr
_temp2368={( void*) _temp2367,( void*) _temp2367,( void*)( _temp2367 + 5u)};
_temp2366( _temp2368);}); goto _LL2147; _LL2161: s=({ struct Cyc_PP_Doc*(*
_temp2374)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2377= Cyc_PP_text(
_tag_arr("let ", sizeof( unsigned char), 5u)); struct Cyc_PP_Doc* _temp2378= Cyc_Absynpp_ids2doc(
_temp2218); struct Cyc_PP_Doc* _temp2379= Cyc_PP_text( _tag_arr(";", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp2375[ 3u]={ _temp2377, _temp2378,
_temp2379}; struct _tagged_arr _temp2376={( void*) _temp2375,( void*) _temp2375,(
void*)( _temp2375 + 3u)}; _temp2374( _temp2376);}); goto _LL2147; _LL2163: if(
_temp2223 == 0){ s=({ struct Cyc_PP_Doc*(* _temp2380)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2383= Cyc_Absynpp_scope2doc( _temp2227); struct Cyc_PP_Doc*
_temp2384= Cyc_PP_text( _tag_arr("enum ", sizeof( unsigned char), 6u)); struct
Cyc_PP_Doc* _temp2385= Cyc_Absynpp_typedef_name2doc( _temp2225); struct Cyc_PP_Doc*
_temp2386= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp2381[ 4u]={ _temp2383, _temp2384, _temp2385, _temp2386}; struct _tagged_arr
_temp2382={( void*) _temp2381,( void*) _temp2381,( void*)( _temp2381 + 4u)};
_temp2380( _temp2382);});} else{ s=({ struct Cyc_PP_Doc*(* _temp2387)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2390= Cyc_Absynpp_scope2doc(
_temp2227); struct Cyc_PP_Doc* _temp2391= Cyc_PP_text( _tag_arr("enum ", sizeof(
unsigned char), 6u)); struct Cyc_PP_Doc* _temp2392= Cyc_Absynpp_qvar2doc(
_temp2225); struct Cyc_PP_Doc* _temp2393= Cyc_PP_text( _tag_arr(" {", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp2394= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(*
_temp2397)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2400= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2401= Cyc_Absynpp_enumfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp2223))->v); struct Cyc_PP_Doc* _temp2398[
2u]={ _temp2400, _temp2401}; struct _tagged_arr _temp2399={( void*) _temp2398,(
void*) _temp2398,( void*)( _temp2398 + 2u)}; _temp2397( _temp2399);})); struct
Cyc_PP_Doc* _temp2395= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2396= Cyc_PP_text(
_tag_arr("};", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp2388[ 7u]={
_temp2390, _temp2391, _temp2392, _temp2393, _temp2394, _temp2395, _temp2396};
struct _tagged_arr _temp2389={( void*) _temp2388,( void*) _temp2388,( void*)(
_temp2388 + 7u)}; _temp2387( _temp2389);});} goto _LL2147; _LL2165: s=({ struct
Cyc_PP_Doc*(* _temp2402)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2405= Cyc_PP_text( _tag_arr("typedef ", sizeof( unsigned char), 9u));
struct Cyc_PP_Doc* _temp2406= Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual
_temp2408; _temp2408.q_const= 0; _temp2408.q_volatile= 0; _temp2408.q_restrict=
0; _temp2408;}),( void*) _temp2229->defn,({ struct Cyc_Core_Opt* _temp2409=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2409->v=(
void*)({ struct Cyc_PP_Doc*(* _temp2410)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2413= Cyc_Absynpp_typedef_name2doc( _temp2229->name);
struct Cyc_PP_Doc* _temp2414= Cyc_Absynpp_tvars2doc( _temp2229->tvs); struct Cyc_PP_Doc*
_temp2411[ 2u]={ _temp2413, _temp2414}; struct _tagged_arr _temp2412={( void*)
_temp2411,( void*) _temp2411,( void*)( _temp2411 + 2u)}; _temp2410( _temp2412);});
_temp2409;})); struct Cyc_PP_Doc* _temp2407= Cyc_PP_text( _tag_arr(";", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp2403[ 3u]={ _temp2405, _temp2406,
_temp2407}; struct _tagged_arr _temp2404={( void*) _temp2403,( void*) _temp2403,(
void*)( _temp2403 + 3u)}; _temp2402( _temp2404);}); goto _LL2147; _LL2167: if(
Cyc_Absynpp_use_curr_namespace){ Cyc_Absynpp_curr_namespace_add( _temp2233);} s=({
struct Cyc_PP_Doc*(* _temp2415)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2418= Cyc_PP_text( _tag_arr("namespace ", sizeof( unsigned char), 11u));
struct Cyc_PP_Doc* _temp2419= Cyc_PP_textptr( _temp2233); struct Cyc_PP_Doc*
_temp2420= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc*
_temp2421= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2422=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_arr sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc, _tag_arr("",
sizeof( unsigned char), 1u), _temp2231); struct Cyc_PP_Doc* _temp2423= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2424= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp2416[ 7u]={ _temp2418, _temp2419, _temp2420,
_temp2421, _temp2422, _temp2423, _temp2424}; struct _tagged_arr _temp2417={(
void*) _temp2416,( void*) _temp2416,( void*)( _temp2416 + 7u)}; _temp2415(
_temp2417);}); if( Cyc_Absynpp_use_curr_namespace){ Cyc_Absynpp_curr_namespace_drop();}
goto _LL2147; _LL2169: if( Cyc_Absynpp_print_using_stmts){ s=({ struct Cyc_PP_Doc*(*
_temp2425)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2428= Cyc_PP_text(
_tag_arr("using ", sizeof( unsigned char), 7u)); struct Cyc_PP_Doc* _temp2429=
Cyc_Absynpp_qvar2doc( _temp2237); struct Cyc_PP_Doc* _temp2430= Cyc_PP_text(
_tag_arr(" {", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp2431= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2432=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)(
struct Cyc_Absyn_Decl*), struct _tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc, _tag_arr("", sizeof( unsigned char), 1u), _temp2235);
struct Cyc_PP_Doc* _temp2433= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2434=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp2426[ 7u]={ _temp2428, _temp2429, _temp2430, _temp2431, _temp2432,
_temp2433, _temp2434}; struct _tagged_arr _temp2427={( void*) _temp2426,( void*)
_temp2426,( void*)( _temp2426 + 7u)}; _temp2425( _temp2427);});} else{ s=({
struct Cyc_PP_Doc*(* _temp2435)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2438= Cyc_PP_text( _tag_arr("/* using ", sizeof( unsigned char), 10u));
struct Cyc_PP_Doc* _temp2439= Cyc_Absynpp_qvar2doc( _temp2237); struct Cyc_PP_Doc*
_temp2440= Cyc_PP_text( _tag_arr(" { */", sizeof( unsigned char), 6u)); struct
Cyc_PP_Doc* _temp2441= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2442=(( struct
Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp2235); struct Cyc_PP_Doc*
_temp2443= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2444= Cyc_PP_text(
_tag_arr("/* } */", sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp2436[
7u]={ _temp2438, _temp2439, _temp2440, _temp2441, _temp2442, _temp2443,
_temp2444}; struct _tagged_arr _temp2437={( void*) _temp2436,( void*) _temp2436,(
void*)( _temp2436 + 7u)}; _temp2435( _temp2437);});} goto _LL2147; _LL2171: if(
Cyc_Absynpp_print_externC_stmts){ s=({ struct Cyc_PP_Doc*(* _temp2445)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2448= Cyc_PP_text( _tag_arr("extern \"C\" {",
sizeof( unsigned char), 13u)); struct Cyc_PP_Doc* _temp2449= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2450=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)(
struct Cyc_Absyn_Decl*), struct _tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc, _tag_arr("", sizeof( unsigned char), 1u), _temp2239);
struct Cyc_PP_Doc* _temp2451= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2452=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp2446[ 5u]={ _temp2448, _temp2449, _temp2450, _temp2451, _temp2452}; struct
_tagged_arr _temp2447={( void*) _temp2446,( void*) _temp2446,( void*)( _temp2446
+ 5u)}; _temp2445( _temp2447);});} else{ s=({ struct Cyc_PP_Doc*(* _temp2453)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2456= Cyc_PP_text(
_tag_arr("/* extern \"C\" { */", sizeof( unsigned char), 19u)); struct Cyc_PP_Doc*
_temp2457= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2458=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_arr sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc, _tag_arr("",
sizeof( unsigned char), 1u), _temp2239); struct Cyc_PP_Doc* _temp2459= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2460= Cyc_PP_text( _tag_arr("/* } */", sizeof(
unsigned char), 8u)); struct Cyc_PP_Doc* _temp2454[ 5u]={ _temp2456, _temp2457,
_temp2458, _temp2459, _temp2460}; struct _tagged_arr _temp2455={( void*)
_temp2454,( void*) _temp2454,( void*)( _temp2454 + 5u)}; _temp2453( _temp2455);});}
goto _LL2147; _LL2147:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_scope2doc(
void* sc){ void* _temp2461= sc; _LL2463: if( _temp2461 ==( void*) Cyc_Absyn_Static){
goto _LL2464;} else{ goto _LL2465;} _LL2465: if( _temp2461 ==( void*) Cyc_Absyn_Public){
goto _LL2466;} else{ goto _LL2467;} _LL2467: if( _temp2461 ==( void*) Cyc_Absyn_Extern){
goto _LL2468;} else{ goto _LL2469;} _LL2469: if( _temp2461 ==( void*) Cyc_Absyn_ExternC){
goto _LL2470;} else{ goto _LL2471;} _LL2471: if( _temp2461 ==( void*) Cyc_Absyn_Abstract){
goto _LL2472;} else{ goto _LL2462;} _LL2464: return Cyc_PP_text( _tag_arr("static ",
sizeof( unsigned char), 8u)); _LL2466: return Cyc_PP_nil_doc(); _LL2468: return
Cyc_PP_text( _tag_arr("extern ", sizeof( unsigned char), 8u)); _LL2470: return
Cyc_PP_text( _tag_arr("extern \"C\" ", sizeof( unsigned char), 12u)); _LL2472:
return Cyc_PP_text( _tag_arr("abstract ", sizeof( unsigned char), 10u)); _LL2462:;}
int Cyc_Absynpp_exists_temp_tvar_in_effect( void* t){ void* _temp2473= t; struct
Cyc_Absyn_Tvar* _temp2481; struct Cyc_List_List* _temp2483; _LL2475: if((
unsigned int) _temp2473 > 4u?*(( int*) _temp2473) == Cyc_Absyn_VarType: 0){
_LL2482: _temp2481=(( struct Cyc_Absyn_VarType_struct*) _temp2473)->f1; goto
_LL2476;} else{ goto _LL2477;} _LL2477: if(( unsigned int) _temp2473 > 4u?*((
int*) _temp2473) == Cyc_Absyn_JoinEff: 0){ _LL2484: _temp2483=(( struct Cyc_Absyn_JoinEff_struct*)
_temp2473)->f1; goto _LL2478;} else{ goto _LL2479;} _LL2479: goto _LL2480;
_LL2476: return Cyc_Tcutil_is_temp_tvar( _temp2481); _LL2478: return(( int(*)(
int(* pred)( void*), struct Cyc_List_List* x)) Cyc_List_exists)( Cyc_Absynpp_exists_temp_tvar_in_effect,
_temp2483); _LL2480: return 0; _LL2474:;} struct _tuple4 Cyc_Absynpp_to_tms(
struct Cyc_Absyn_Tqual tq, void* t){ void* _temp2485= t; struct Cyc_Absyn_Exp*
_temp2499; struct Cyc_Absyn_Tqual _temp2501; void* _temp2503; struct Cyc_Absyn_PtrInfo
_temp2505; struct Cyc_Absyn_Conref* _temp2507; struct Cyc_Absyn_Tqual _temp2509;
struct Cyc_Absyn_Conref* _temp2511; void* _temp2513; void* _temp2515; struct Cyc_Absyn_FnInfo
_temp2517; struct Cyc_List_List* _temp2519; struct Cyc_List_List* _temp2521;
struct Cyc_Absyn_VarargInfo* _temp2523; int _temp2525; struct Cyc_List_List*
_temp2527; void* _temp2529; struct Cyc_Core_Opt* _temp2531; struct Cyc_List_List*
_temp2533; struct Cyc_Core_Opt* _temp2535; int _temp2537; struct Cyc_Core_Opt*
_temp2539; struct Cyc_Core_Opt* _temp2541; struct Cyc_Core_Opt* _temp2543;
struct Cyc_List_List* _temp2545; struct _tuple0* _temp2547; _LL2487: if((
unsigned int) _temp2485 > 4u?*(( int*) _temp2485) == Cyc_Absyn_ArrayType: 0){
_LL2504: _temp2503=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2485)->f1;
goto _LL2502; _LL2502: _temp2501=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2485)->f2; goto _LL2500; _LL2500: _temp2499=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2485)->f3; goto _LL2488;} else{ goto _LL2489;} _LL2489: if(( unsigned int)
_temp2485 > 4u?*(( int*) _temp2485) == Cyc_Absyn_PointerType: 0){ _LL2506:
_temp2505=(( struct Cyc_Absyn_PointerType_struct*) _temp2485)->f1; _LL2516:
_temp2515=( void*) _temp2505.elt_typ; goto _LL2514; _LL2514: _temp2513=( void*)
_temp2505.rgn_typ; goto _LL2512; _LL2512: _temp2511= _temp2505.nullable; goto
_LL2510; _LL2510: _temp2509= _temp2505.tq; goto _LL2508; _LL2508: _temp2507=
_temp2505.bounds; goto _LL2490;} else{ goto _LL2491;} _LL2491: if(( unsigned int)
_temp2485 > 4u?*(( int*) _temp2485) == Cyc_Absyn_FnType: 0){ _LL2518: _temp2517=((
struct Cyc_Absyn_FnType_struct*) _temp2485)->f1; _LL2534: _temp2533= _temp2517.tvars;
goto _LL2532; _LL2532: _temp2531= _temp2517.effect; goto _LL2530; _LL2530:
_temp2529=( void*) _temp2517.ret_typ; goto _LL2528; _LL2528: _temp2527=
_temp2517.args; goto _LL2526; _LL2526: _temp2525= _temp2517.c_varargs; goto
_LL2524; _LL2524: _temp2523= _temp2517.cyc_varargs; goto _LL2522; _LL2522:
_temp2521= _temp2517.rgn_po; goto _LL2520; _LL2520: _temp2519= _temp2517.attributes;
goto _LL2492;} else{ goto _LL2493;} _LL2493: if(( unsigned int) _temp2485 > 4u?*((
int*) _temp2485) == Cyc_Absyn_Evar: 0){ _LL2542: _temp2541=(( struct Cyc_Absyn_Evar_struct*)
_temp2485)->f1; goto _LL2540; _LL2540: _temp2539=(( struct Cyc_Absyn_Evar_struct*)
_temp2485)->f2; goto _LL2538; _LL2538: _temp2537=(( struct Cyc_Absyn_Evar_struct*)
_temp2485)->f3; goto _LL2536; _LL2536: _temp2535=(( struct Cyc_Absyn_Evar_struct*)
_temp2485)->f4; goto _LL2494;} else{ goto _LL2495;} _LL2495: if(( unsigned int)
_temp2485 > 4u?*(( int*) _temp2485) == Cyc_Absyn_TypedefType: 0){ _LL2548:
_temp2547=(( struct Cyc_Absyn_TypedefType_struct*) _temp2485)->f1; goto _LL2546;
_LL2546: _temp2545=(( struct Cyc_Absyn_TypedefType_struct*) _temp2485)->f2; goto
_LL2544; _LL2544: _temp2543=(( struct Cyc_Absyn_TypedefType_struct*) _temp2485)->f3;
goto _LL2496;} else{ goto _LL2497;} _LL2497: goto _LL2498; _LL2488: { struct Cyc_List_List*
_temp2551; void* _temp2553; struct Cyc_Absyn_Tqual _temp2555; struct _tuple4
_temp2549= Cyc_Absynpp_to_tms( _temp2501, _temp2503); _LL2556: _temp2555=
_temp2549.f1; goto _LL2554; _LL2554: _temp2553= _temp2549.f2; goto _LL2552;
_LL2552: _temp2551= _temp2549.f3; goto _LL2550; _LL2550: { void* tm; if(
_temp2499 == 0){ tm=( void*) Cyc_Absyn_Carray_mod;} else{ tm=( void*)({ struct
Cyc_Absyn_ConstArray_mod_struct* _temp2557=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2557[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2558; _temp2558.tag= Cyc_Absyn_ConstArray_mod;
_temp2558.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp2499); _temp2558;});
_temp2557;});} return({ struct _tuple4 _temp2559; _temp2559.f1= _temp2555;
_temp2559.f2= _temp2553; _temp2559.f3=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp2560=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2560->hd=( void*) tm; _temp2560->tl= _temp2551; _temp2560;}); _temp2559;});}}
_LL2490: { struct Cyc_List_List* _temp2563; void* _temp2565; struct Cyc_Absyn_Tqual
_temp2567; struct _tuple4 _temp2561= Cyc_Absynpp_to_tms( _temp2509, _temp2515);
_LL2568: _temp2567= _temp2561.f1; goto _LL2566; _LL2566: _temp2565= _temp2561.f2;
goto _LL2564; _LL2564: _temp2563= _temp2561.f3; goto _LL2562; _LL2562: { void*
ps;{ struct _tuple8 _temp2570=({ struct _tuple8 _temp2569; _temp2569.f1=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp2511))->v; _temp2569.f2=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp2507))->v; _temp2569;}); void* _temp2580;
void* _temp2582; struct Cyc_Absyn_Exp* _temp2584; void* _temp2586; int _temp2588;
void* _temp2590; void* _temp2592; struct Cyc_Absyn_Exp* _temp2594; void*
_temp2596; int _temp2598; void* _temp2600; void* _temp2602; void* _temp2604;
_LL2572: _LL2587: _temp2586= _temp2570.f1; if(( unsigned int) _temp2586 > 1u?*((
int*) _temp2586) == Cyc_Absyn_Eq_constr: 0){ _LL2589: _temp2588=( int)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp2586)->f1; if( _temp2588 == 1){ goto _LL2581;}
else{ goto _LL2574;}} else{ goto _LL2574;} _LL2581: _temp2580= _temp2570.f2; if((
unsigned int) _temp2580 > 1u?*(( int*) _temp2580) == Cyc_Absyn_Eq_constr: 0){
_LL2583: _temp2582=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2580)->f1;
if(( unsigned int) _temp2582 > 1u?*(( int*) _temp2582) == Cyc_Absyn_Upper_b: 0){
_LL2585: _temp2584=(( struct Cyc_Absyn_Upper_b_struct*) _temp2582)->f1; goto
_LL2573;} else{ goto _LL2574;}} else{ goto _LL2574;} _LL2574: _LL2597: _temp2596=
_temp2570.f1; if(( unsigned int) _temp2596 > 1u?*(( int*) _temp2596) == Cyc_Absyn_Eq_constr:
0){ _LL2599: _temp2598=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2596)->f1;
if( _temp2598 == 0){ goto _LL2591;} else{ goto _LL2576;}} else{ goto _LL2576;}
_LL2591: _temp2590= _temp2570.f2; if(( unsigned int) _temp2590 > 1u?*(( int*)
_temp2590) == Cyc_Absyn_Eq_constr: 0){ _LL2593: _temp2592=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2590)->f1; if(( unsigned int) _temp2592 > 1u?*(( int*) _temp2592) == Cyc_Absyn_Upper_b:
0){ _LL2595: _temp2594=(( struct Cyc_Absyn_Upper_b_struct*) _temp2592)->f1; goto
_LL2575;} else{ goto _LL2576;}} else{ goto _LL2576;} _LL2576: _LL2605: _temp2604=
_temp2570.f1; goto _LL2601; _LL2601: _temp2600= _temp2570.f2; if(( unsigned int)
_temp2600 > 1u?*(( int*) _temp2600) == Cyc_Absyn_Eq_constr: 0){ _LL2603:
_temp2602=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2600)->f1; if(
_temp2602 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL2577;} else{ goto _LL2578;}}
else{ goto _LL2578;} _LL2578: goto _LL2579; _LL2573: ps=( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2606=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2606[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2607; _temp2607.tag=
Cyc_Absyn_Nullable_ps; _temp2607.f1= _temp2584; _temp2607;}); _temp2606;}); goto
_LL2571; _LL2575: ps=( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2608=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2608[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2609; _temp2609.tag= Cyc_Absyn_NonNullable_ps; _temp2609.f1= _temp2594;
_temp2609;}); _temp2608;}); goto _LL2571; _LL2577: ps=( void*) Cyc_Absyn_TaggedArray_ps;
goto _LL2571; _LL2579: ps=( void*) Cyc_Absyn_TaggedArray_ps; goto _LL2571;
_LL2571:;}{ void* tm=( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2612=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp2612[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp2613; _temp2613.tag=
Cyc_Absyn_Pointer_mod; _temp2613.f1=( void*) ps; _temp2613.f2=( void*) _temp2513;
_temp2613.f3= tq; _temp2613;}); _temp2612;}); return({ struct _tuple4 _temp2610;
_temp2610.f1= _temp2567; _temp2610.f2= _temp2565; _temp2610.f3=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp2611=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2611->hd=( void*) tm; _temp2611->tl= _temp2563;
_temp2611;}); _temp2610;});}}} _LL2492: if( ! Cyc_Absynpp_print_all_tvars){ if(
_temp2531 == 0? 1: Cyc_Absynpp_exists_temp_tvar_in_effect(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2531))->v)){ _temp2531= 0; _temp2533= 0;}} else{ if( Cyc_Absynpp_rewrite_temp_tvars){((
void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)(
Cyc_Tcutil_rewrite_temp_tvar, _temp2533);}}{ struct Cyc_List_List* _temp2616;
void* _temp2618; struct Cyc_Absyn_Tqual _temp2620; struct _tuple4 _temp2614= Cyc_Absynpp_to_tms(
Cyc_Absyn_empty_tqual(), _temp2529); _LL2621: _temp2620= _temp2614.f1; goto
_LL2619; _LL2619: _temp2618= _temp2614.f2; goto _LL2617; _LL2617: _temp2616=
_temp2614.f3; goto _LL2615; _LL2615: { struct Cyc_List_List* tms= _temp2616; if(
_temp2519 != 0){ tms=({ struct Cyc_List_List* _temp2622=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2622->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2623=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2623[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2624; _temp2624.tag= Cyc_Absyn_Attributes_mod;
_temp2624.f1= 0; _temp2624.f2= _temp2519; _temp2624;}); _temp2623;})); _temp2622->tl=
tms; _temp2622;});} tms=({ struct Cyc_List_List* _temp2625=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2625->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2626=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2626[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2627; _temp2627.tag= Cyc_Absyn_Function_mod;
_temp2627.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2628=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2628[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2629; _temp2629.tag= Cyc_Absyn_WithTypes;
_temp2629.f1= _temp2527; _temp2629.f2= _temp2525; _temp2629.f3= _temp2523;
_temp2629.f4= _temp2531; _temp2629.f5= _temp2521; _temp2629;}); _temp2628;}));
_temp2627;}); _temp2626;})); _temp2625->tl= tms; _temp2625;}); if( _temp2533 !=
0){ tms=({ struct Cyc_List_List* _temp2630=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2630->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp2631=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp2631[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp2632; _temp2632.tag= Cyc_Absyn_TypeParams_mod; _temp2632.f1= _temp2533;
_temp2632.f2= 0; _temp2632.f3= 1; _temp2632;}); _temp2631;})); _temp2630->tl=
tms; _temp2630;});} return({ struct _tuple4 _temp2633; _temp2633.f1= _temp2620;
_temp2633.f2= _temp2618; _temp2633.f3= tms; _temp2633;});}} _LL2494: if(
_temp2539 == 0){ return({ struct _tuple4 _temp2634; _temp2634.f1= tq; _temp2634.f2=
t; _temp2634.f3= 0; _temp2634;});} else{ return Cyc_Absynpp_to_tms( tq,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2539))->v);} _LL2496: if( _temp2543 == 0?
1: ! Cyc_Absynpp_expand_typedefs){ return({ struct _tuple4 _temp2635; _temp2635.f1=
tq; _temp2635.f2= t; _temp2635.f3= 0; _temp2635;});} else{ return Cyc_Absynpp_to_tms(
tq,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2543))->v);} _LL2498:
return({ struct _tuple4 _temp2636; _temp2636.f1= tq; _temp2636.f2= t; _temp2636.f3=
0; _temp2636;}); _LL2486:;} struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc( struct Cyc_Absyn_Tqual
tq, void* t, struct Cyc_Core_Opt* dopt){ struct Cyc_List_List* _temp2639; void*
_temp2641; struct Cyc_Absyn_Tqual _temp2643; struct _tuple4 _temp2637= Cyc_Absynpp_to_tms(
tq, t); _LL2644: _temp2643= _temp2637.f1; goto _LL2642; _LL2642: _temp2641=
_temp2637.f2; goto _LL2640; _LL2640: _temp2639= _temp2637.f3; goto _LL2638;
_LL2638: _temp2639=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp2639); if( _temp2639 == 0? dopt == 0: 0){ return({ struct Cyc_PP_Doc*(*
_temp2645)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2648= Cyc_Absynpp_tqual2doc(
_temp2643); struct Cyc_PP_Doc* _temp2649= Cyc_Absynpp_ntyp2doc( _temp2641);
struct Cyc_PP_Doc* _temp2646[ 2u]={ _temp2648, _temp2649}; struct _tagged_arr
_temp2647={( void*) _temp2646,( void*) _temp2646,( void*)( _temp2646 + 2u)};
_temp2645( _temp2647);});} else{ return({ struct Cyc_PP_Doc*(* _temp2650)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2653= Cyc_Absynpp_tqual2doc(
_temp2643); struct Cyc_PP_Doc* _temp2654= Cyc_Absynpp_ntyp2doc( _temp2641);
struct Cyc_PP_Doc* _temp2655= Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp2656= Cyc_Absynpp_dtms2doc( dopt == 0? Cyc_PP_nil_doc():(
struct Cyc_PP_Doc*)(( struct Cyc_Core_Opt*) _check_null( dopt))->v, _temp2639);
struct Cyc_PP_Doc* _temp2651[ 4u]={ _temp2653, _temp2654, _temp2655, _temp2656};
struct _tagged_arr _temp2652={( void*) _temp2651,( void*) _temp2651,( void*)(
_temp2651 + 4u)}; _temp2650( _temp2652);});}} struct Cyc_PP_Doc* Cyc_Absynpp_structfield2doc(
struct Cyc_Absyn_Structfield* f){ if( f->width != 0){ return({ struct Cyc_PP_Doc*(*
_temp2657)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2660= Cyc_Absynpp_tqtd2doc(
f->tq,( void*) f->type,({ struct Cyc_Core_Opt* _temp2665=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2665->v=( void*) Cyc_PP_textptr(
f->name); _temp2665;})); struct Cyc_PP_Doc* _temp2661= Cyc_PP_text( _tag_arr(":",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp2662= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( f->width))->v);
struct Cyc_PP_Doc* _temp2663= Cyc_Absynpp_atts2doc( f->attributes); struct Cyc_PP_Doc*
_temp2664= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp2658[ 5u]={ _temp2660, _temp2661, _temp2662, _temp2663, _temp2664}; struct
_tagged_arr _temp2659={( void*) _temp2658,( void*) _temp2658,( void*)( _temp2658
+ 5u)}; _temp2657( _temp2659);});} else{ return({ struct Cyc_PP_Doc*(* _temp2666)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2669= Cyc_Absynpp_tqtd2doc(
f->tq,( void*) f->type,({ struct Cyc_Core_Opt* _temp2672=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2672->v=( void*) Cyc_PP_textptr(
f->name); _temp2672;})); struct Cyc_PP_Doc* _temp2670= Cyc_Absynpp_atts2doc( f->attributes);
struct Cyc_PP_Doc* _temp2671= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp2667[ 3u]={ _temp2669, _temp2670, _temp2671};
struct _tagged_arr _temp2668={( void*) _temp2667,( void*) _temp2667,( void*)(
_temp2667 + 3u)}; _temp2666( _temp2668);});}} struct Cyc_PP_Doc* Cyc_Absynpp_structfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Structfield*), struct _tagged_arr sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_structfield2doc, _tag_arr("", sizeof(
unsigned char), 1u), fields);} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfield2doc(
struct Cyc_Absyn_Tunionfield* f){ return({ struct Cyc_PP_Doc*(* _temp2673)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2676= Cyc_Absynpp_scope2doc((
void*) f->sc); struct Cyc_PP_Doc* _temp2677= Cyc_Absynpp_typedef_name2doc( f->name);
struct Cyc_PP_Doc* _temp2678= f->tvs == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_ktvars2doc(
f->tvs); struct Cyc_PP_Doc* _temp2679= f->typs == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(
f->typs); struct Cyc_PP_Doc* _temp2674[ 4u]={ _temp2676, _temp2677, _temp2678,
_temp2679}; struct _tagged_arr _temp2675={( void*) _temp2674,( void*) _temp2674,(
void*)( _temp2674 + 4u)}; _temp2673( _temp2675);});} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc(
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
sc){ void* _temp2680= sc; _LL2682: if( _temp2680 ==( void*) Cyc_Absyn_Static){
goto _LL2683;} else{ goto _LL2684;} _LL2684: if( _temp2680 ==( void*) Cyc_Absyn_Public){
goto _LL2685;} else{ goto _LL2686;} _LL2686: if( _temp2680 ==( void*) Cyc_Absyn_Extern){
goto _LL2687;} else{ goto _LL2688;} _LL2688: if( _temp2680 ==( void*) Cyc_Absyn_ExternC){
goto _LL2689;} else{ goto _LL2690;} _LL2690: if( _temp2680 ==( void*) Cyc_Absyn_Abstract){
goto _LL2691;} else{ goto _LL2681;} _LL2683: return _tag_arr("static", sizeof(
unsigned char), 7u); _LL2685: return _tag_arr("public", sizeof( unsigned char),
7u); _LL2687: return _tag_arr("extern", sizeof( unsigned char), 7u); _LL2689:
return _tag_arr("extern \"C\"", sizeof( unsigned char), 11u); _LL2691: return
_tag_arr("abstract", sizeof( unsigned char), 9u); _LL2681:;}