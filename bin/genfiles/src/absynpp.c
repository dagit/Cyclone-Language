 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_dirent{ int d_ino; int d_off; unsigned short
d_reclen; unsigned char d_type; unsigned char d_name[ 256u]; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern
struct _tagged_arr Cyc_Core_new_string( int); extern unsigned char Cyc_Core_InvalidArg[
15u]; struct Cyc_Core_InvalidArg_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern struct _tagged_arr Cyc_Core_string_of_int( int); extern
unsigned char* string_to_Cstring( struct _tagged_arr); extern unsigned char*
underlying_Cstring( struct _tagged_arr); extern struct _tagged_arr
Cstring_to_string( unsigned char*); extern struct _tagged_arr
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
_check_null(* l))->tl == 0){* l= 0;} else{ Cyc_Absynpp_suppr_last(&(( struct Cyc_List_List*)
_check_null(* l))->tl);}} static void Cyc_Absynpp_curr_namespace_drop(){(( void(*)(
struct Cyc_List_List** l)) Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
struct _tagged_arr Cyc_Absynpp_char_escape( unsigned char c){ switch( c){ case '\a':
_LL2: return _tag_arr("\\a", sizeof( unsigned char), 3u); case '\b': _LL3:
return _tag_arr("\\b", sizeof( unsigned char), 3u); case '\f': _LL4: return
_tag_arr("\\f", sizeof( unsigned char), 3u); case '\n': _LL5: return _tag_arr("\\n",
sizeof( unsigned char), 3u); case '\r': _LL6: return _tag_arr("\\r", sizeof(
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
void*)( Cyc_Absyn_compress_conref( c))->v; void* _temp84; _LL80: if((
unsigned int) _temp78 > 1u?*(( int*) _temp78) == Cyc_Absyn_Eq_constr: 0){ _LL85:
_temp84=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp78)->f1; goto _LL81;}
else{ goto _LL82;} _LL82: goto _LL83; _LL81: return Cyc_Absynpp_kind2string(
_temp84); _LL83: return _tag_arr("?", sizeof( unsigned char), 2u); _LL79:;}
struct Cyc_PP_Doc* Cyc_Absynpp_kind2doc( void* k){ return Cyc_PP_text( Cyc_Absynpp_kind2string(
k));} struct Cyc_PP_Doc* Cyc_Absynpp_ckind2doc( struct Cyc_Absyn_Conref* c){
return Cyc_PP_text( Cyc_Absynpp_ckind2string( c));} struct Cyc_PP_Doc* Cyc_Absynpp_tps2doc(
struct Cyc_List_List* ts){ return Cyc_PP_egroup( _tag_arr("<", sizeof(
unsigned char), 2u), _tag_arr(">", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_typ2doc, ts));}
struct Cyc_PP_Doc* Cyc_Absynpp_ktvar2doc( struct Cyc_Absyn_Tvar* tv){ void*
_temp86=( void*)( Cyc_Absyn_compress_conref( tv->kind))->v; void* _temp94; void*
_temp96; _LL88: if(( unsigned int) _temp86 > 1u?*(( int*) _temp86) == Cyc_Absyn_Eq_constr:
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
_tag_arr(" _stdcall ", sizeof( unsigned char), 11u)); _LL124: return Cyc_PP_text(
_tag_arr(" _cdecl ", sizeof( unsigned char), 9u)); _LL126: return Cyc_PP_text(
_tag_arr(" _fastcall ", sizeof( unsigned char), 12u)); _LL128: goto _LL120;
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
_check_null( tms))->hd; _LL152: if(( unsigned int) _temp150 > 1u?*(( int*)
_temp150) == Cyc_Absyn_Pointer_mod: 0){ goto _LL153;} else{ goto _LL154;} _LL154:
if(( unsigned int) _temp150 > 1u?*(( int*) _temp150) == Cyc_Absyn_Attributes_mod:
0){ goto _LL155;} else{ goto _LL156;} _LL156: goto _LL157; _LL153: return 1;
_LL155: if( ! Cyc_Absynpp_to_VC){ return 0;} return Cyc_Absynpp_next_is_pointer(((
struct Cyc_List_List*) _check_null( tms))->tl); _LL157: return 0; _LL151:;}}
extern struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc( void* t); struct Cyc_PP_Doc* Cyc_Absynpp_dtms2doc(
struct Cyc_PP_Doc* d, struct Cyc_List_List* tms){ if( tms == 0){ return d;}{
struct Cyc_PP_Doc* rest= Cyc_Absynpp_dtms2doc( d,(( struct Cyc_List_List*)
_check_null( tms))->tl); struct Cyc_PP_Doc* p_rest=({ struct Cyc_PP_Doc*(*
_temp296)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp299= Cyc_PP_text(
_tag_arr("(", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp300= rest;
struct Cyc_PP_Doc* _temp301= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp297[ 3u]={ _temp299, _temp300, _temp301}; struct
_tagged_arr _temp298={( void*) _temp297,( void*) _temp297,( void*)( _temp297 + 3u)};
_temp296( _temp298);}); void* _temp158=( void*)(( struct Cyc_List_List*)
_check_null( tms))->hd; struct Cyc_Absyn_Exp* _temp172; void* _temp174; struct
Cyc_List_List* _temp176; int _temp178; struct Cyc_Position_Segment* _temp180;
struct Cyc_List_List* _temp182; struct Cyc_Absyn_Tqual _temp184; void* _temp186;
void* _temp188; _LL160: if( _temp158 ==( void*) Cyc_Absyn_Carray_mod){ goto
_LL161;} else{ goto _LL162;} _LL162: if(( unsigned int) _temp158 > 1u?*(( int*)
_temp158) == Cyc_Absyn_ConstArray_mod: 0){ _LL173: _temp172=(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp158)->f1; goto _LL163;} else{ goto _LL164;} _LL164: if(( unsigned int)
_temp158 > 1u?*(( int*) _temp158) == Cyc_Absyn_Function_mod: 0){ _LL175:
_temp174=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp158)->f1; goto
_LL165;} else{ goto _LL166;} _LL166: if(( unsigned int) _temp158 > 1u?*(( int*)
_temp158) == Cyc_Absyn_Attributes_mod: 0){ _LL177: _temp176=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp158)->f2; goto _LL167;} else{ goto _LL168;} _LL168: if(( unsigned int)
_temp158 > 1u?*(( int*) _temp158) == Cyc_Absyn_TypeParams_mod: 0){ _LL183:
_temp182=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp158)->f1; goto _LL181;
_LL181: _temp180=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp158)->f2; goto
_LL179; _LL179: _temp178=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp158)->f3;
goto _LL169;} else{ goto _LL170;} _LL170: if(( unsigned int) _temp158 > 1u?*((
int*) _temp158) == Cyc_Absyn_Pointer_mod: 0){ _LL189: _temp188=( void*)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp158)->f1; goto _LL187; _LL187: _temp186=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp158)->f2; goto _LL185;
_LL185: _temp184=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp158)->f3; goto
_LL171;} else{ goto _LL159;} _LL161: if( Cyc_Absynpp_next_is_pointer((( struct
Cyc_List_List*) _check_null( tms))->tl)){ rest= p_rest;} return({ struct Cyc_PP_Doc*(*
_temp190)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp193= rest;
struct Cyc_PP_Doc* _temp194= Cyc_PP_text( _tag_arr("[]", sizeof( unsigned char),
3u)); struct Cyc_PP_Doc* _temp191[ 2u]={ _temp193, _temp194}; struct _tagged_arr
_temp192={( void*) _temp191,( void*) _temp191,( void*)( _temp191 + 2u)};
_temp190( _temp192);}); _LL163: if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*)
_check_null( tms))->tl)){ rest= p_rest;} return({ struct Cyc_PP_Doc*(* _temp195)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp198= rest; struct Cyc_PP_Doc*
_temp199= Cyc_PP_text( _tag_arr("[", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp200= Cyc_Absynpp_exp2doc( _temp172); struct Cyc_PP_Doc* _temp201= Cyc_PP_text(
_tag_arr("]", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp196[ 4u]={
_temp198, _temp199, _temp200, _temp201}; struct _tagged_arr _temp197={( void*)
_temp196,( void*) _temp196,( void*)( _temp196 + 4u)}; _temp195( _temp197);});
_LL165: if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*) _check_null(
tms))->tl)){ rest= p_rest;}{ void* _temp202= _temp174; struct Cyc_List_List*
_temp208; struct Cyc_Core_Opt* _temp210; struct Cyc_Absyn_VarargInfo* _temp212;
int _temp214; struct Cyc_List_List* _temp216; struct Cyc_Position_Segment*
_temp218; struct Cyc_List_List* _temp220; _LL204: if(*(( int*) _temp202) == Cyc_Absyn_WithTypes){
_LL217: _temp216=(( struct Cyc_Absyn_WithTypes_struct*) _temp202)->f1; goto
_LL215; _LL215: _temp214=(( struct Cyc_Absyn_WithTypes_struct*) _temp202)->f2;
goto _LL213; _LL213: _temp212=(( struct Cyc_Absyn_WithTypes_struct*) _temp202)->f3;
goto _LL211; _LL211: _temp210=(( struct Cyc_Absyn_WithTypes_struct*) _temp202)->f4;
goto _LL209; _LL209: _temp208=(( struct Cyc_Absyn_WithTypes_struct*) _temp202)->f5;
goto _LL205;} else{ goto _LL206;} _LL206: if(*(( int*) _temp202) == Cyc_Absyn_NoTypes){
_LL221: _temp220=(( struct Cyc_Absyn_NoTypes_struct*) _temp202)->f1; goto _LL219;
_LL219: _temp218=(( struct Cyc_Absyn_NoTypes_struct*) _temp202)->f2; goto _LL207;}
else{ goto _LL203;} _LL205: return({ struct Cyc_PP_Doc*(* _temp222)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp225= rest; struct Cyc_PP_Doc*
_temp226= Cyc_Absynpp_funargs2doc( _temp216, _temp214, _temp212, _temp210,
_temp208); struct Cyc_PP_Doc* _temp223[ 2u]={ _temp225, _temp226}; struct
_tagged_arr _temp224={( void*) _temp223,( void*) _temp223,( void*)( _temp223 + 2u)};
_temp222( _temp224);}); _LL207: return({ struct Cyc_PP_Doc*(* _temp227)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp230= rest; struct Cyc_PP_Doc*
_temp231= Cyc_PP_group( _tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tagged_arr*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_PP_textptr, _temp220)); struct Cyc_PP_Doc* _temp228[ 2u]={
_temp230, _temp231}; struct _tagged_arr _temp229={( void*) _temp228,( void*)
_temp228,( void*)( _temp228 + 2u)}; _temp227( _temp229);}); _LL203:;} _LL167:
if( ! Cyc_Absynpp_to_VC){ if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*)
_check_null( tms))->tl)){ rest= p_rest;} return({ struct Cyc_PP_Doc*(* _temp232)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp235= rest; struct Cyc_PP_Doc*
_temp236= Cyc_Absynpp_atts2doc( _temp176); struct Cyc_PP_Doc* _temp233[ 2u]={
_temp235, _temp236}; struct _tagged_arr _temp234={( void*) _temp233,( void*)
_temp233,( void*)( _temp233 + 2u)}; _temp232( _temp234);});} else{ if( Cyc_Absynpp_next_is_pointer(((
struct Cyc_List_List*) _check_null( tms))->tl)){ return({ struct Cyc_PP_Doc*(*
_temp237)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp240= Cyc_Absynpp_callconv2doc(
_temp176); struct Cyc_PP_Doc* _temp241= rest; struct Cyc_PP_Doc* _temp238[ 2u]={
_temp240, _temp241}; struct _tagged_arr _temp239={( void*) _temp238,( void*)
_temp238,( void*)( _temp238 + 2u)}; _temp237( _temp239);});} return rest;}
_LL169: if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*) _check_null(
tms))->tl)){ rest= p_rest;} if( _temp178){ return({ struct Cyc_PP_Doc*(*
_temp242)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp245= rest;
struct Cyc_PP_Doc* _temp246= Cyc_Absynpp_ktvars2doc( _temp182); struct Cyc_PP_Doc*
_temp243[ 2u]={ _temp245, _temp246}; struct _tagged_arr _temp244={( void*)
_temp243,( void*) _temp243,( void*)( _temp243 + 2u)}; _temp242( _temp244);});}
else{ return({ struct Cyc_PP_Doc*(* _temp247)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp250= rest; struct Cyc_PP_Doc* _temp251= Cyc_Absynpp_tvars2doc(
_temp182); struct Cyc_PP_Doc* _temp248[ 2u]={ _temp250, _temp251}; struct
_tagged_arr _temp249={( void*) _temp248,( void*) _temp248,( void*)( _temp248 + 2u)};
_temp247( _temp249);});} _LL171: { struct Cyc_PP_Doc* ptr;{ void* _temp252=
_temp188; struct Cyc_Absyn_Exp* _temp260; struct Cyc_Absyn_Exp* _temp262; _LL254:
if(( unsigned int) _temp252 > 1u?*(( int*) _temp252) == Cyc_Absyn_Nullable_ps: 0){
_LL261: _temp260=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp252)->f1; goto
_LL255;} else{ goto _LL256;} _LL256: if(( unsigned int) _temp252 > 1u?*(( int*)
_temp252) == Cyc_Absyn_NonNullable_ps: 0){ _LL263: _temp262=(( struct Cyc_Absyn_NonNullable_ps_struct*)
_temp252)->f1; goto _LL257;} else{ goto _LL258;} _LL258: if( _temp252 ==( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL259;} else{ goto _LL253;} _LL255: if( Cyc_Evexp_eval_const_uint_exp(
_temp260) == 1){ ptr= Cyc_PP_text( _tag_arr("*", sizeof( unsigned char), 2u));}
else{ ptr=({ struct Cyc_PP_Doc*(* _temp264)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp267= Cyc_PP_text( _tag_arr("*", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp268= Cyc_PP_text( _tag_arr("{", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp269= Cyc_Absynpp_exp2doc( _temp260);
struct Cyc_PP_Doc* _temp270= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp265[ 4u]={ _temp267, _temp268, _temp269, _temp270};
struct _tagged_arr _temp266={( void*) _temp265,( void*) _temp265,( void*)(
_temp265 + 4u)}; _temp264( _temp266);});} goto _LL253; _LL257: if( Cyc_Evexp_eval_const_uint_exp(
_temp262) == 1){ ptr= Cyc_PP_text( _tag_arr("@", sizeof( unsigned char), 2u));}
else{ ptr=({ struct Cyc_PP_Doc*(* _temp271)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp274= Cyc_PP_text( _tag_arr("@", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp275= Cyc_PP_text( _tag_arr("{", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp276= Cyc_Absynpp_exp2doc( _temp262);
struct Cyc_PP_Doc* _temp277= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp272[ 4u]={ _temp274, _temp275, _temp276, _temp277};
struct _tagged_arr _temp273={( void*) _temp272,( void*) _temp272,( void*)(
_temp272 + 4u)}; _temp271( _temp273);});} goto _LL253; _LL259: ptr= Cyc_PP_text(
_tag_arr("?", sizeof( unsigned char), 2u)); goto _LL253; _LL253:;}{ void*
_temp278= Cyc_Tcutil_compress( _temp186); _LL280: if( _temp278 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL281;} else{ goto _LL282;} _LL282: goto _LL283; _LL281: return({ struct
Cyc_PP_Doc*(* _temp284)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp287= ptr; struct Cyc_PP_Doc* _temp288= rest; struct Cyc_PP_Doc* _temp285[ 2u]={
_temp287, _temp288}; struct _tagged_arr _temp286={( void*) _temp285,( void*)
_temp285,( void*)( _temp285 + 2u)}; _temp284( _temp286);}); _LL283: return({
struct Cyc_PP_Doc*(* _temp289)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp292= ptr; struct Cyc_PP_Doc* _temp293= Cyc_Absynpp_ntyp2doc( _temp186);
struct Cyc_PP_Doc* _temp294= Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp295= rest; struct Cyc_PP_Doc* _temp290[ 4u]={
_temp292, _temp293, _temp294, _temp295}; struct _tagged_arr _temp291={( void*)
_temp290,( void*) _temp290,( void*)( _temp290 + 4u)}; _temp289( _temp291);});
_LL279:;}} _LL159:;}} struct Cyc_PP_Doc* Cyc_Absynpp_rgn2doc( void* t){ void*
_temp302= Cyc_Tcutil_compress( t); _LL304: if( _temp302 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL305;} else{ goto _LL306;} _LL306: goto _LL307; _LL305: return Cyc_PP_text(
_tag_arr("`H", sizeof( unsigned char), 3u)); _LL307: return Cyc_Absynpp_ntyp2doc(
t); _LL303:;} static void Cyc_Absynpp_effects2docs( struct Cyc_List_List**
regions, struct Cyc_List_List** effects, void* t){ void* _temp308= Cyc_Tcutil_compress(
t); void* _temp316; struct Cyc_List_List* _temp318; _LL310: if(( unsigned int)
_temp308 > 4u?*(( int*) _temp308) == Cyc_Absyn_AccessEff: 0){ _LL317: _temp316=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp308)->f1; goto _LL311;} else{
goto _LL312;} _LL312: if(( unsigned int) _temp308 > 4u?*(( int*) _temp308) ==
Cyc_Absyn_JoinEff: 0){ _LL319: _temp318=(( struct Cyc_Absyn_JoinEff_struct*)
_temp308)->f1; goto _LL313;} else{ goto _LL314;} _LL314: goto _LL315; _LL311:*
regions=({ struct Cyc_List_List* _temp320=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp320->hd=( void*) Cyc_Absynpp_rgn2doc(
_temp316); _temp320->tl=* regions; _temp320;}); goto _LL309; _LL313: for( 0;
_temp318 != 0; _temp318=(( struct Cyc_List_List*) _check_null( _temp318))->tl){
Cyc_Absynpp_effects2docs( regions, effects,( void*)(( struct Cyc_List_List*)
_check_null( _temp318))->hd);} goto _LL309; _LL315:* effects=({ struct Cyc_List_List*
_temp321=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp321->hd=( void*) Cyc_Absynpp_ntyp2doc( t); _temp321->tl=* effects; _temp321;});
goto _LL309; _LL309:;} struct Cyc_PP_Doc* Cyc_Absynpp_eff2doc( void* t){ struct
Cyc_List_List* regions= 0; struct Cyc_List_List* effects= 0; Cyc_Absynpp_effects2docs(&
regions,& effects, t); regions=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( regions); effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( effects); if( regions == 0? effects != 0: 0){ return Cyc_PP_group(
_tag_arr("", sizeof( unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u),
_tag_arr("+", sizeof( unsigned char), 2u), effects);} else{ struct Cyc_PP_Doc*
_temp322= Cyc_PP_group( _tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}",
sizeof( unsigned char), 2u), _tag_arr(",", sizeof( unsigned char), 2u), regions);
return Cyc_PP_group( _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("",
sizeof( unsigned char), 1u), _tag_arr("+", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
effects,({ struct Cyc_List_List* _temp323=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp323->hd=( void*) _temp322; _temp323->tl= 0;
_temp323;})));}} struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc( void* t){ struct Cyc_PP_Doc*
s;{ void* _temp324= t; struct Cyc_Core_Opt* _temp370; int _temp372; struct Cyc_Core_Opt*
_temp374; struct Cyc_Core_Opt* _temp376; struct Cyc_Absyn_Tvar* _temp378; struct
Cyc_Absyn_TunionInfo _temp380; void* _temp382; struct Cyc_List_List* _temp384;
void* _temp386; struct Cyc_Absyn_TunionFieldInfo _temp388; struct Cyc_List_List*
_temp390; void* _temp392; void* _temp394; void* _temp396; struct Cyc_List_List*
_temp398; struct Cyc_List_List* _temp400; struct _tuple0* _temp402; struct Cyc_List_List*
_temp404; struct _tuple0* _temp406; struct Cyc_List_List* _temp408; struct Cyc_List_List*
_temp410; struct _tuple0* _temp412; struct Cyc_Core_Opt* _temp414; struct Cyc_List_List*
_temp416; struct _tuple0* _temp418; void* _temp420; _LL326: if(( unsigned int)
_temp324 > 4u?*(( int*) _temp324) == Cyc_Absyn_ArrayType: 0){ goto _LL327;}
else{ goto _LL328;} _LL328: if(( unsigned int) _temp324 > 4u?*(( int*) _temp324)
== Cyc_Absyn_FnType: 0){ goto _LL329;} else{ goto _LL330;} _LL330: if((
unsigned int) _temp324 > 4u?*(( int*) _temp324) == Cyc_Absyn_PointerType: 0){
goto _LL331;} else{ goto _LL332;} _LL332: if( _temp324 ==( void*) Cyc_Absyn_VoidType){
goto _LL333;} else{ goto _LL334;} _LL334: if(( unsigned int) _temp324 > 4u?*((
int*) _temp324) == Cyc_Absyn_Evar: 0){ _LL377: _temp376=(( struct Cyc_Absyn_Evar_struct*)
_temp324)->f1; goto _LL375; _LL375: _temp374=(( struct Cyc_Absyn_Evar_struct*)
_temp324)->f2; goto _LL373; _LL373: _temp372=(( struct Cyc_Absyn_Evar_struct*)
_temp324)->f3; goto _LL371; _LL371: _temp370=(( struct Cyc_Absyn_Evar_struct*)
_temp324)->f4; goto _LL335;} else{ goto _LL336;} _LL336: if(( unsigned int)
_temp324 > 4u?*(( int*) _temp324) == Cyc_Absyn_VarType: 0){ _LL379: _temp378=((
struct Cyc_Absyn_VarType_struct*) _temp324)->f1; goto _LL337;} else{ goto _LL338;}
_LL338: if(( unsigned int) _temp324 > 4u?*(( int*) _temp324) == Cyc_Absyn_TunionType:
0){ _LL381: _temp380=(( struct Cyc_Absyn_TunionType_struct*) _temp324)->f1;
_LL387: _temp386=( void*) _temp380.tunion_info; goto _LL385; _LL385: _temp384=
_temp380.targs; goto _LL383; _LL383: _temp382=( void*) _temp380.rgn; goto _LL339;}
else{ goto _LL340;} _LL340: if(( unsigned int) _temp324 > 4u?*(( int*) _temp324)
== Cyc_Absyn_TunionFieldType: 0){ _LL389: _temp388=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp324)->f1; _LL393: _temp392=( void*) _temp388.field_info; goto _LL391;
_LL391: _temp390= _temp388.targs; goto _LL341;} else{ goto _LL342;} _LL342: if((
unsigned int) _temp324 > 4u?*(( int*) _temp324) == Cyc_Absyn_IntType: 0){ _LL397:
_temp396=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp324)->f1; goto _LL395;
_LL395: _temp394=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp324)->f2;
goto _LL343;} else{ goto _LL344;} _LL344: if( _temp324 ==( void*) Cyc_Absyn_FloatType){
goto _LL345;} else{ goto _LL346;} _LL346: if( _temp324 ==( void*) Cyc_Absyn_DoubleType){
goto _LL347;} else{ goto _LL348;} _LL348: if(( unsigned int) _temp324 > 4u?*((
int*) _temp324) == Cyc_Absyn_TupleType: 0){ _LL399: _temp398=(( struct Cyc_Absyn_TupleType_struct*)
_temp324)->f1; goto _LL349;} else{ goto _LL350;} _LL350: if(( unsigned int)
_temp324 > 4u?*(( int*) _temp324) == Cyc_Absyn_StructType: 0){ _LL403: _temp402=((
struct Cyc_Absyn_StructType_struct*) _temp324)->f1; goto _LL401; _LL401:
_temp400=(( struct Cyc_Absyn_StructType_struct*) _temp324)->f2; goto _LL351;}
else{ goto _LL352;} _LL352: if(( unsigned int) _temp324 > 4u?*(( int*) _temp324)
== Cyc_Absyn_UnionType: 0){ _LL407: _temp406=(( struct Cyc_Absyn_UnionType_struct*)
_temp324)->f1; goto _LL405; _LL405: _temp404=(( struct Cyc_Absyn_UnionType_struct*)
_temp324)->f2; goto _LL353;} else{ goto _LL354;} _LL354: if(( unsigned int)
_temp324 > 4u?*(( int*) _temp324) == Cyc_Absyn_AnonStructType: 0){ _LL409:
_temp408=(( struct Cyc_Absyn_AnonStructType_struct*) _temp324)->f1; goto _LL355;}
else{ goto _LL356;} _LL356: if(( unsigned int) _temp324 > 4u?*(( int*) _temp324)
== Cyc_Absyn_AnonUnionType: 0){ _LL411: _temp410=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp324)->f1; goto _LL357;} else{ goto _LL358;} _LL358: if(( unsigned int)
_temp324 > 4u?*(( int*) _temp324) == Cyc_Absyn_EnumType: 0){ _LL413: _temp412=((
struct Cyc_Absyn_EnumType_struct*) _temp324)->f1; goto _LL359;} else{ goto
_LL360;} _LL360: if(( unsigned int) _temp324 > 4u?*(( int*) _temp324) == Cyc_Absyn_TypedefType:
0){ _LL419: _temp418=(( struct Cyc_Absyn_TypedefType_struct*) _temp324)->f1;
goto _LL417; _LL417: _temp416=(( struct Cyc_Absyn_TypedefType_struct*) _temp324)->f2;
goto _LL415; _LL415: _temp414=(( struct Cyc_Absyn_TypedefType_struct*) _temp324)->f3;
goto _LL361;} else{ goto _LL362;} _LL362: if(( unsigned int) _temp324 > 4u?*((
int*) _temp324) == Cyc_Absyn_RgnHandleType: 0){ _LL421: _temp420=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp324)->f1; goto _LL363;} else{ goto
_LL364;} _LL364: if( _temp324 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL365;} else{
goto _LL366;} _LL366: if(( unsigned int) _temp324 > 4u?*(( int*) _temp324) ==
Cyc_Absyn_AccessEff: 0){ goto _LL367;} else{ goto _LL368;} _LL368: if((
unsigned int) _temp324 > 4u?*(( int*) _temp324) == Cyc_Absyn_JoinEff: 0){ goto
_LL369;} else{ goto _LL325;} _LL327: return Cyc_PP_text( _tag_arr("[[[array]]]",
sizeof( unsigned char), 12u)); _LL329: return Cyc_PP_nil_doc(); _LL331: return
Cyc_PP_nil_doc(); _LL333: s= Cyc_PP_text( _tag_arr("void", sizeof( unsigned char),
5u)); goto _LL325; _LL335: if( _temp374 != 0){ return Cyc_Absynpp_ntyp2doc((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp374))->v);} else{ s=({ struct
Cyc_PP_Doc*(* _temp422)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp425= Cyc_PP_text( _tag_arr("%", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp426= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp427= Cyc_PP_text(( struct _tagged_arr) xprintf("%d", _temp372)); struct Cyc_PP_Doc*
_temp428=( ! Cyc_Absynpp_print_full_evars? 1: _temp370 == 0)? Cyc_PP_text(
_tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_tvars2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp370))->v); struct Cyc_PP_Doc* _temp429=
Cyc_PP_text( _tag_arr(")::", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc*
_temp430= _temp376 == 0? Cyc_PP_text( _tag_arr("?", sizeof( unsigned char), 2u)):
Cyc_Absynpp_kind2doc(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp376))->v);
struct Cyc_PP_Doc* _temp423[ 6u]={ _temp425, _temp426, _temp427, _temp428,
_temp429, _temp430}; struct _tagged_arr _temp424={( void*) _temp423,( void*)
_temp423,( void*)( _temp423 + 6u)}; _temp422( _temp424);});} goto _LL325; _LL337:
s= Cyc_PP_textptr( _temp378->name); if( Cyc_Absynpp_print_all_kinds){ s=({
struct Cyc_PP_Doc*(* _temp431)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp434= s; struct Cyc_PP_Doc* _temp435= Cyc_PP_text( _tag_arr("::", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp436= Cyc_Absynpp_ckind2doc(
_temp378->kind); struct Cyc_PP_Doc* _temp432[ 3u]={ _temp434, _temp435, _temp436};
struct _tagged_arr _temp433={( void*) _temp432,( void*) _temp432,( void*)(
_temp432 + 3u)}; _temp431( _temp433);});} if( Cyc_Absynpp_rewrite_temp_tvars?
Cyc_Tcutil_is_temp_tvar( _temp378): 0){ s=({ struct Cyc_PP_Doc*(* _temp437)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp440= Cyc_PP_text(
_tag_arr("_ /* ", sizeof( unsigned char), 6u)); struct Cyc_PP_Doc* _temp441= s;
struct Cyc_PP_Doc* _temp442= Cyc_PP_text( _tag_arr(" */", sizeof( unsigned char),
4u)); struct Cyc_PP_Doc* _temp438[ 3u]={ _temp440, _temp441, _temp442}; struct
_tagged_arr _temp439={( void*) _temp438,( void*) _temp438,( void*)( _temp438 + 3u)};
_temp437( _temp439);});} goto _LL325; _LL339:{ void* _temp443= _temp386; struct
Cyc_Absyn_UnknownTunionInfo _temp449; int _temp451; struct _tuple0* _temp453;
struct Cyc_Absyn_Tuniondecl* _temp455; struct Cyc_Absyn_Tuniondecl _temp457; int
_temp458; struct _tuple0* _temp460; _LL445: if(*(( int*) _temp443) == Cyc_Absyn_UnknownTunion){
_LL450: _temp449=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp443)->f1;
_LL454: _temp453= _temp449.name; goto _LL452; _LL452: _temp451= _temp449.is_xtunion;
goto _LL446;} else{ goto _LL447;} _LL447: if(*(( int*) _temp443) == Cyc_Absyn_KnownTunion){
_LL456: _temp455=(( struct Cyc_Absyn_KnownTunion_struct*) _temp443)->f1;
_temp457=* _temp455; _LL461: _temp460= _temp457.name; goto _LL459; _LL459:
_temp458= _temp457.is_xtunion; goto _LL448;} else{ goto _LL444;} _LL446:
_temp460= _temp453; _temp458= _temp451; goto _LL448; _LL448: { struct Cyc_PP_Doc*
_temp462= Cyc_PP_text( _temp458? _tag_arr("xtunion ", sizeof( unsigned char), 9u):
_tag_arr("tunion ", sizeof( unsigned char), 8u));{ void* _temp463= Cyc_Tcutil_compress(
_temp382); _LL465: if( _temp463 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL466;}
else{ goto _LL467;} _LL467: goto _LL468; _LL466: s=({ struct Cyc_PP_Doc*(*
_temp469)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp472=
_temp462; struct Cyc_PP_Doc* _temp473= Cyc_Absynpp_qvar2doc( _temp460); struct
Cyc_PP_Doc* _temp474= Cyc_Absynpp_tps2doc( _temp384); struct Cyc_PP_Doc*
_temp470[ 3u]={ _temp472, _temp473, _temp474}; struct _tagged_arr _temp471={(
void*) _temp470,( void*) _temp470,( void*)( _temp470 + 3u)}; _temp469( _temp471);});
goto _LL464; _LL468: s=({ struct Cyc_PP_Doc*(* _temp475)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp478= _temp462; struct Cyc_PP_Doc* _temp479=
Cyc_Absynpp_typ2doc( _temp382); struct Cyc_PP_Doc* _temp480= Cyc_PP_text(
_tag_arr(" ", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp481= Cyc_Absynpp_qvar2doc(
_temp460); struct Cyc_PP_Doc* _temp482= Cyc_Absynpp_tps2doc( _temp384); struct
Cyc_PP_Doc* _temp476[ 5u]={ _temp478, _temp479, _temp480, _temp481, _temp482};
struct _tagged_arr _temp477={( void*) _temp476,( void*) _temp476,( void*)(
_temp476 + 5u)}; _temp475( _temp477);}); goto _LL464; _LL464:;} goto _LL444;}
_LL444:;} goto _LL325; _LL341:{ void* _temp483= _temp392; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp489; int _temp491; struct _tuple0* _temp493; struct _tuple0* _temp495;
struct Cyc_Absyn_Tunionfield* _temp497; struct Cyc_Absyn_Tunionfield _temp499;
struct _tuple0* _temp500; struct Cyc_Absyn_Tuniondecl* _temp502; struct Cyc_Absyn_Tuniondecl
_temp504; int _temp505; struct _tuple0* _temp507; _LL485: if(*(( int*) _temp483)
== Cyc_Absyn_UnknownTunionfield){ _LL490: _temp489=(( struct Cyc_Absyn_UnknownTunionfield_struct*)
_temp483)->f1; _LL496: _temp495= _temp489.tunion_name; goto _LL494; _LL494:
_temp493= _temp489.field_name; goto _LL492; _LL492: _temp491= _temp489.is_xtunion;
goto _LL486;} else{ goto _LL487;} _LL487: if(*(( int*) _temp483) == Cyc_Absyn_KnownTunionfield){
_LL503: _temp502=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp483)->f1;
_temp504=* _temp502; _LL508: _temp507= _temp504.name; goto _LL506; _LL506:
_temp505= _temp504.is_xtunion; goto _LL498; _LL498: _temp497=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp483)->f2; _temp499=* _temp497; _LL501: _temp500= _temp499.name; goto _LL488;}
else{ goto _LL484;} _LL486: _temp507= _temp495; _temp505= _temp491; _temp500=
_temp493; goto _LL488; _LL488: { struct Cyc_PP_Doc* _temp509= Cyc_PP_text(
_temp505? _tag_arr("xtunion ", sizeof( unsigned char), 9u): _tag_arr("tunion ",
sizeof( unsigned char), 8u)); s=({ struct Cyc_PP_Doc*(* _temp510)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp513= _temp509; struct Cyc_PP_Doc*
_temp514= Cyc_Absynpp_qvar2doc( _temp507); struct Cyc_PP_Doc* _temp515= Cyc_PP_text(
_tag_arr(".", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp516= Cyc_Absynpp_qvar2doc(
_temp500); struct Cyc_PP_Doc* _temp511[ 4u]={ _temp513, _temp514, _temp515,
_temp516}; struct _tagged_arr _temp512={( void*) _temp511,( void*) _temp511,(
void*)( _temp511 + 4u)}; _temp510( _temp512);}); goto _LL484;} _LL484:;} goto
_LL325; _LL343: { struct _tagged_arr sns; struct _tagged_arr ts;{ void* _temp517=
_temp396; _LL519: if( _temp517 ==( void*) Cyc_Absyn_Signed){ goto _LL520;} else{
goto _LL521;} _LL521: if( _temp517 ==( void*) Cyc_Absyn_Unsigned){ goto _LL522;}
else{ goto _LL518;} _LL520: sns= _tag_arr("", sizeof( unsigned char), 1u); goto
_LL518; _LL522: sns= _tag_arr("unsigned ", sizeof( unsigned char), 10u); goto
_LL518; _LL518:;}{ void* _temp523= _temp394; _LL525: if( _temp523 ==( void*) Cyc_Absyn_B1){
goto _LL526;} else{ goto _LL527;} _LL527: if( _temp523 ==( void*) Cyc_Absyn_B2){
goto _LL528;} else{ goto _LL529;} _LL529: if( _temp523 ==( void*) Cyc_Absyn_B4){
goto _LL530;} else{ goto _LL531;} _LL531: if( _temp523 ==( void*) Cyc_Absyn_B8){
goto _LL532;} else{ goto _LL524;} _LL526:{ void* _temp533= _temp396; _LL535: if(
_temp533 ==( void*) Cyc_Absyn_Signed){ goto _LL536;} else{ goto _LL537;} _LL537:
if( _temp533 ==( void*) Cyc_Absyn_Unsigned){ goto _LL538;} else{ goto _LL534;}
_LL536: sns= _tag_arr("signed ", sizeof( unsigned char), 8u); goto _LL534;
_LL538: goto _LL534; _LL534:;} ts= _tag_arr("char", sizeof( unsigned char), 5u);
goto _LL524; _LL528: ts= _tag_arr("short", sizeof( unsigned char), 6u); goto
_LL524; _LL530: ts= _tag_arr("int", sizeof( unsigned char), 4u); goto _LL524;
_LL532: ts= Cyc_Absynpp_to_VC? _tag_arr("__int64", sizeof( unsigned char), 8u):
_tag_arr("long long", sizeof( unsigned char), 10u); goto _LL524; _LL524:;} s=
Cyc_PP_text(( struct _tagged_arr)({ struct _tagged_arr _temp539= sns; struct
_tagged_arr _temp540= ts; xprintf("%.*s%.*s", _get_arr_size( _temp539, 1u),
_temp539.curr, _get_arr_size( _temp540, 1u), _temp540.curr);})); goto _LL325;}
_LL345: s= Cyc_PP_text( _tag_arr("float", sizeof( unsigned char), 6u)); goto
_LL325; _LL347: s= Cyc_PP_text( _tag_arr("double", sizeof( unsigned char), 7u));
goto _LL325; _LL349: s=({ struct Cyc_PP_Doc*(* _temp541)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp544= Cyc_PP_text( _tag_arr("$", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp545= Cyc_Absynpp_args2doc(
_temp398); struct Cyc_PP_Doc* _temp542[ 2u]={ _temp544, _temp545}; struct
_tagged_arr _temp543={( void*) _temp542,( void*) _temp542,( void*)( _temp542 + 2u)};
_temp541( _temp543);}); goto _LL325; _LL351: if( _temp402 == 0){ s=({ struct Cyc_PP_Doc*(*
_temp546)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp549= Cyc_PP_text(
_tag_arr("struct ", sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp550=
Cyc_Absynpp_tps2doc( _temp400); struct Cyc_PP_Doc* _temp547[ 2u]={ _temp549,
_temp550}; struct _tagged_arr _temp548={( void*) _temp547,( void*) _temp547,(
void*)( _temp547 + 2u)}; _temp546( _temp548);});} else{ s=({ struct Cyc_PP_Doc*(*
_temp551)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp554= Cyc_PP_text(
_tag_arr("struct ", sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp555=
_temp402 == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*)
_check_null( _temp402)); struct Cyc_PP_Doc* _temp556= Cyc_Absynpp_tps2doc(
_temp400); struct Cyc_PP_Doc* _temp552[ 3u]={ _temp554, _temp555, _temp556};
struct _tagged_arr _temp553={( void*) _temp552,( void*) _temp552,( void*)(
_temp552 + 3u)}; _temp551( _temp553);});} goto _LL325; _LL353: if( _temp406 == 0){
s=({ struct Cyc_PP_Doc*(* _temp557)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp560= Cyc_PP_text( _tag_arr("union ", sizeof( unsigned char), 7u)); struct
Cyc_PP_Doc* _temp561= Cyc_Absynpp_tps2doc( _temp404); struct Cyc_PP_Doc*
_temp558[ 2u]={ _temp560, _temp561}; struct _tagged_arr _temp559={( void*)
_temp558,( void*) _temp558,( void*)( _temp558 + 2u)}; _temp557( _temp559);});}
else{ s=({ struct Cyc_PP_Doc*(* _temp562)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp565= Cyc_PP_text( _tag_arr("union ", sizeof(
unsigned char), 7u)); struct Cyc_PP_Doc* _temp566= _temp406 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc(( struct _tuple0*) _check_null( _temp406)); struct Cyc_PP_Doc*
_temp567= Cyc_Absynpp_tps2doc( _temp404); struct Cyc_PP_Doc* _temp563[ 3u]={
_temp565, _temp566, _temp567}; struct _tagged_arr _temp564={( void*) _temp563,(
void*) _temp563,( void*)( _temp563 + 3u)}; _temp562( _temp564);});} goto _LL325;
_LL355: s=({ struct Cyc_PP_Doc*(* _temp568)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp571= Cyc_PP_text( _tag_arr("struct {", sizeof(
unsigned char), 9u)); struct Cyc_PP_Doc* _temp572= Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc(
_temp408)); struct Cyc_PP_Doc* _temp573= Cyc_PP_text( _tag_arr("}", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp569[ 3u]={ _temp571, _temp572,
_temp573}; struct _tagged_arr _temp570={( void*) _temp569,( void*) _temp569,(
void*)( _temp569 + 3u)}; _temp568( _temp570);}); goto _LL325; _LL357: s=({
struct Cyc_PP_Doc*(* _temp574)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp577= Cyc_PP_text( _tag_arr("union {", sizeof( unsigned char), 8u)); struct
Cyc_PP_Doc* _temp578= Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc( _temp410));
struct Cyc_PP_Doc* _temp579= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp575[ 3u]={ _temp577, _temp578, _temp579}; struct
_tagged_arr _temp576={( void*) _temp575,( void*) _temp575,( void*)( _temp575 + 3u)};
_temp574( _temp576);}); goto _LL325; _LL359: s=({ struct Cyc_PP_Doc*(* _temp580)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp583= Cyc_PP_text(
_tag_arr("enum ", sizeof( unsigned char), 6u)); struct Cyc_PP_Doc* _temp584= Cyc_Absynpp_qvar2doc(
_temp412); struct Cyc_PP_Doc* _temp581[ 2u]={ _temp583, _temp584}; struct
_tagged_arr _temp582={( void*) _temp581,( void*) _temp581,( void*)( _temp581 + 2u)};
_temp580( _temp582);}); goto _LL325; _LL361: s=({ struct Cyc_PP_Doc*(* _temp585)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp588= Cyc_Absynpp_qvar2doc(
_temp418); struct Cyc_PP_Doc* _temp589= Cyc_Absynpp_tps2doc( _temp416); struct
Cyc_PP_Doc* _temp586[ 2u]={ _temp588, _temp589}; struct _tagged_arr _temp587={(
void*) _temp586,( void*) _temp586,( void*)( _temp586 + 2u)}; _temp585( _temp587);});
goto _LL325; _LL363: s=({ struct Cyc_PP_Doc*(* _temp590)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp593= Cyc_PP_text( _tag_arr("region_t<",
sizeof( unsigned char), 10u)); struct Cyc_PP_Doc* _temp594= Cyc_Absynpp_rgn2doc(
_temp420); struct Cyc_PP_Doc* _temp595= Cyc_PP_text( _tag_arr(">", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp591[ 3u]={ _temp593, _temp594,
_temp595}; struct _tagged_arr _temp592={( void*) _temp591,( void*) _temp591,(
void*)( _temp591 + 3u)}; _temp590( _temp592);}); goto _LL325; _LL365: s= Cyc_Absynpp_rgn2doc(
t); goto _LL325; _LL367: s= Cyc_Absynpp_eff2doc( t); goto _LL325; _LL369: s= Cyc_Absynpp_eff2doc(
t); goto _LL325; _LL325:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_vo2doc(
struct Cyc_Core_Opt* vo){ return vo == 0? Cyc_PP_nil_doc(): Cyc_PP_text(*((
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null( vo))->v));} struct
_tuple8{ void* f1; void* f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_rgn_cmp2doc(
struct _tuple8* cmp){ struct _tuple8 _temp598; void* _temp599; void* _temp601;
struct _tuple8* _temp596= cmp; _temp598=* _temp596; _LL602: _temp601= _temp598.f1;
goto _LL600; _LL600: _temp599= _temp598.f2; goto _LL597; _LL597: return({ struct
Cyc_PP_Doc*(* _temp603)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp606= Cyc_Absynpp_rgn2doc( _temp601); struct Cyc_PP_Doc* _temp607= Cyc_PP_text(
_tag_arr(" < ", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc* _temp608= Cyc_Absynpp_rgn2doc(
_temp599); struct Cyc_PP_Doc* _temp604[ 3u]={ _temp606, _temp607, _temp608};
struct _tagged_arr _temp605={( void*) _temp604,( void*) _temp604,( void*)(
_temp604 + 3u)}; _temp603( _temp605);});} struct Cyc_PP_Doc* Cyc_Absynpp_rgnpo2doc(
struct Cyc_List_List* po){ return Cyc_PP_group( _tag_arr("", sizeof(
unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple8*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_rgn_cmp2doc,
po));} struct Cyc_PP_Doc* Cyc_Absynpp_funarg2doc( struct _tuple1* t){ struct Cyc_Core_Opt*
dopt=(* t).f1 == 0? 0:({ struct Cyc_Core_Opt* _temp609=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp609->v=( void*) Cyc_PP_text(*((
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v));
_temp609;}); return Cyc_Absynpp_tqtd2doc((* t).f2,(* t).f3, dopt);} struct Cyc_PP_Doc*
Cyc_Absynpp_funargs2doc( struct Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo*
cyc_varargs, struct Cyc_Core_Opt* effopt, struct Cyc_List_List* rgn_po){ struct
Cyc_List_List* _temp610=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc,
args); struct Cyc_PP_Doc* eff_doc; if( c_varargs){ _temp610=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp610,({
struct Cyc_List_List* _temp611=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp611->hd=( void*) Cyc_PP_text( _tag_arr("...",
sizeof( unsigned char), 4u)); _temp611->tl= 0; _temp611;}));} else{ if(
cyc_varargs != 0){ struct Cyc_PP_Doc* _temp612=({ struct Cyc_PP_Doc*(* _temp614)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp617= Cyc_PP_text(
_tag_arr("...", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc* _temp618= Cyc_Absynpp_typ2doc((
void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->rgn); struct
Cyc_PP_Doc* _temp619=(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->inject?
Cyc_PP_text( _tag_arr(" inject ", sizeof( unsigned char), 9u)): Cyc_PP_text(
_tag_arr(" ", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp620= Cyc_Absynpp_funarg2doc(({
struct _tuple1* _temp621=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp621->f1=(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->name;
_temp621->f2=(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->tq;
_temp621->f3=( void*)(( struct Cyc_Absyn_VarargInfo*) _check_null( cyc_varargs))->type;
_temp621;})); struct Cyc_PP_Doc* _temp615[ 4u]={ _temp617, _temp618, _temp619,
_temp620}; struct _tagged_arr _temp616={( void*) _temp615,( void*) _temp615,(
void*)( _temp615 + 4u)}; _temp614( _temp616);}); _temp610=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp610,({
struct Cyc_List_List* _temp613=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp613->hd=( void*) _temp612; _temp613->tl= 0;
_temp613;}));}}{ struct Cyc_PP_Doc* _temp622= Cyc_PP_group( _tag_arr("", sizeof(
unsigned char), 1u), _tag_arr("", sizeof( unsigned char), 1u), _tag_arr(",",
sizeof( unsigned char), 2u), _temp610); if( effopt != 0){ _temp622=({ struct Cyc_PP_Doc*(*
_temp623)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp626=
_temp622; struct Cyc_PP_Doc* _temp627= Cyc_PP_text( _tag_arr(";", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp628= Cyc_Absynpp_eff2doc(( void*)((
struct Cyc_Core_Opt*) _check_null( effopt))->v); struct Cyc_PP_Doc* _temp624[ 3u]={
_temp626, _temp627, _temp628}; struct _tagged_arr _temp625={( void*) _temp624,(
void*) _temp624,( void*)( _temp624 + 3u)}; _temp623( _temp625);});} if( rgn_po
!= 0){ _temp622=({ struct Cyc_PP_Doc*(* _temp629)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp632= _temp622; struct Cyc_PP_Doc* _temp633= Cyc_PP_text(
_tag_arr(":", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp634= Cyc_Absynpp_rgnpo2doc(
rgn_po); struct Cyc_PP_Doc* _temp630[ 3u]={ _temp632, _temp633, _temp634};
struct _tagged_arr _temp631={( void*) _temp630,( void*) _temp630,( void*)(
_temp630 + 3u)}; _temp629( _temp631);});} return({ struct Cyc_PP_Doc*(* _temp635)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp638= Cyc_PP_text(
_tag_arr("(", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp639=
_temp622; struct Cyc_PP_Doc* _temp640= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp636[ 3u]={ _temp638, _temp639,
_temp640}; struct _tagged_arr _temp637={( void*) _temp636,( void*) _temp636,(
void*)( _temp636 + 3u)}; _temp635( _temp637);});}} struct _tuple1* Cyc_Absynpp_arg_mk_opt(
struct _tuple3* arg){ return({ struct _tuple1* _temp641=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp641->f1=({ struct Cyc_Core_Opt*
_temp642=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp642->v=( void*)(* arg).f1; _temp642;}); _temp641->f2=(* arg).f2; _temp641->f3=(*
arg).f3; _temp641;});} struct Cyc_PP_Doc* Cyc_Absynpp_var2doc( struct
_tagged_arr* v){ return Cyc_PP_text(* v);} struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc(
struct _tuple0* q){ struct Cyc_List_List* _temp643= 0; int match;{ void*
_temp644=(* q).f1; struct Cyc_List_List* _temp652; struct Cyc_List_List*
_temp654; _LL646: if( _temp644 ==( void*) Cyc_Absyn_Loc_n){ goto _LL647;} else{
goto _LL648;} _LL648: if(( unsigned int) _temp644 > 1u?*(( int*) _temp644) ==
Cyc_Absyn_Rel_n: 0){ _LL653: _temp652=(( struct Cyc_Absyn_Rel_n_struct*)
_temp644)->f1; goto _LL649;} else{ goto _LL650;} _LL650: if(( unsigned int)
_temp644 > 1u?*(( int*) _temp644) == Cyc_Absyn_Abs_n: 0){ _LL655: _temp654=((
struct Cyc_Absyn_Abs_n_struct*) _temp644)->f1; goto _LL651;} else{ goto _LL645;}
_LL647: _temp652= 0; goto _LL649; _LL649: match= 0; _temp643= _temp652; goto
_LL645; _LL651: match= Cyc_Absynpp_use_curr_namespace?(( int(*)( int(* cmp)(
struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_List_list_prefix)( Cyc_String_strptrcmp, _temp654, Cyc_Absynpp_curr_namespace):
0; _temp643=( Cyc_Absynpp_qvar_to_Cids? Cyc_Absynpp_add_cyc_prefix: 0)?({ struct
Cyc_List_List* _temp656=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp656->hd=( void*) Cyc_Absynpp_cyc_stringptr; _temp656->tl= _temp654;
_temp656;}): _temp654; goto _LL645; _LL645:;} if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_PP_text(( struct _tagged_arr) Cyc_String_str_sepstr((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp643,({
struct Cyc_List_List* _temp657=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp657->hd=( void*)(* q).f2; _temp657->tl= 0; _temp657;})),
_tag_arr("_", sizeof( unsigned char), 2u)));} else{ if( match){ return Cyc_Absynpp_var2doc((*
q).f2);} else{ return Cyc_PP_text(( struct _tagged_arr) Cyc_String_str_sepstr(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp643,({ struct Cyc_List_List* _temp658=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp658->hd=( void*)(* q).f2; _temp658->tl= 0;
_temp658;})), _tag_arr("::", sizeof( unsigned char), 3u)));}}} struct Cyc_PP_Doc*
Cyc_Absynpp_typedef_name2doc( struct _tuple0* v){ if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_Absynpp_qvar2doc( v);} if( Cyc_Absynpp_use_curr_namespace){ void*
_temp659=(* v).f1; struct Cyc_List_List* _temp669; struct Cyc_List_List*
_temp671; _LL661: if( _temp659 ==( void*) Cyc_Absyn_Loc_n){ goto _LL662;} else{
goto _LL663;} _LL663: if(( unsigned int) _temp659 > 1u?*(( int*) _temp659) ==
Cyc_Absyn_Rel_n: 0){ _LL670: _temp669=(( struct Cyc_Absyn_Rel_n_struct*)
_temp659)->f1; if( _temp669 == 0){ goto _LL664;} else{ goto _LL665;}} else{ goto
_LL665;} _LL665: if(( unsigned int) _temp659 > 1u?*(( int*) _temp659) == Cyc_Absyn_Abs_n:
0){ _LL672: _temp671=(( struct Cyc_Absyn_Abs_n_struct*) _temp659)->f1; goto
_LL666;} else{ goto _LL667;} _LL667: goto _LL668; _LL662: goto _LL664; _LL664:
return Cyc_Absynpp_var2doc((* v).f2); _LL666: if((( int(*)( int(* cmp)( struct
_tagged_arr*, struct _tagged_arr*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_List_list_cmp)( Cyc_String_strptrcmp, _temp671, Cyc_Absynpp_curr_namespace)
== 0){ return Cyc_Absynpp_var2doc((* v).f2);} else{ goto _LL668;} _LL668: return({
struct Cyc_PP_Doc*(* _temp673)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp676= Cyc_PP_text( _tag_arr("/* bad namespace : */ ", sizeof( unsigned char),
23u)); struct Cyc_PP_Doc* _temp677= Cyc_Absynpp_qvar2doc( v); struct Cyc_PP_Doc*
_temp674[ 2u]={ _temp676, _temp677}; struct _tagged_arr _temp675={( void*)
_temp674,( void*) _temp674,( void*)( _temp674 + 2u)}; _temp673( _temp675);});
_LL660:;} else{ return Cyc_Absynpp_var2doc((* v).f2);}} struct Cyc_PP_Doc* Cyc_Absynpp_typ2doc(
void* t){ return Cyc_Absynpp_tqtd2doc( Cyc_Absyn_empty_tqual(), t, 0);} int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp* e){ void* _temp678=( void*) e->r; void* _temp752; struct
Cyc_Absyn_Exp* _temp754; struct Cyc_Absyn_Exp* _temp756; _LL680: if(*(( int*)
_temp678) == Cyc_Absyn_Const_e){ goto _LL681;} else{ goto _LL682;} _LL682: if(*((
int*) _temp678) == Cyc_Absyn_Var_e){ goto _LL683;} else{ goto _LL684;} _LL684:
if(*(( int*) _temp678) == Cyc_Absyn_UnknownId_e){ goto _LL685;} else{ goto
_LL686;} _LL686: if(*(( int*) _temp678) == Cyc_Absyn_Primop_e){ _LL753: _temp752=(
void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp678)->f1; goto _LL687;} else{
goto _LL688;} _LL688: if(*(( int*) _temp678) == Cyc_Absyn_AssignOp_e){ goto
_LL689;} else{ goto _LL690;} _LL690: if(*(( int*) _temp678) == Cyc_Absyn_Increment_e){
goto _LL691;} else{ goto _LL692;} _LL692: if(*(( int*) _temp678) == Cyc_Absyn_Conditional_e){
goto _LL693;} else{ goto _LL694;} _LL694: if(*(( int*) _temp678) == Cyc_Absyn_SeqExp_e){
goto _LL695;} else{ goto _LL696;} _LL696: if(*(( int*) _temp678) == Cyc_Absyn_UnknownCall_e){
goto _LL697;} else{ goto _LL698;} _LL698: if(*(( int*) _temp678) == Cyc_Absyn_FnCall_e){
goto _LL699;} else{ goto _LL700;} _LL700: if(*(( int*) _temp678) == Cyc_Absyn_Throw_e){
goto _LL701;} else{ goto _LL702;} _LL702: if(*(( int*) _temp678) == Cyc_Absyn_NoInstantiate_e){
_LL755: _temp754=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp678)->f1;
goto _LL703;} else{ goto _LL704;} _LL704: if(*(( int*) _temp678) == Cyc_Absyn_Instantiate_e){
_LL757: _temp756=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp678)->f1; goto
_LL705;} else{ goto _LL706;} _LL706: if(*(( int*) _temp678) == Cyc_Absyn_Cast_e){
goto _LL707;} else{ goto _LL708;} _LL708: if(*(( int*) _temp678) == Cyc_Absyn_New_e){
goto _LL709;} else{ goto _LL710;} _LL710: if(*(( int*) _temp678) == Cyc_Absyn_Address_e){
goto _LL711;} else{ goto _LL712;} _LL712: if(*(( int*) _temp678) == Cyc_Absyn_Sizeoftyp_e){
goto _LL713;} else{ goto _LL714;} _LL714: if(*(( int*) _temp678) == Cyc_Absyn_Sizeofexp_e){
goto _LL715;} else{ goto _LL716;} _LL716: if(*(( int*) _temp678) == Cyc_Absyn_Offsetof_e){
goto _LL717;} else{ goto _LL718;} _LL718: if(*(( int*) _temp678) == Cyc_Absyn_Deref_e){
goto _LL719;} else{ goto _LL720;} _LL720: if(*(( int*) _temp678) == Cyc_Absyn_StructMember_e){
goto _LL721;} else{ goto _LL722;} _LL722: if(*(( int*) _temp678) == Cyc_Absyn_StructArrow_e){
goto _LL723;} else{ goto _LL724;} _LL724: if(*(( int*) _temp678) == Cyc_Absyn_Subscript_e){
goto _LL725;} else{ goto _LL726;} _LL726: if(*(( int*) _temp678) == Cyc_Absyn_Tuple_e){
goto _LL727;} else{ goto _LL728;} _LL728: if(*(( int*) _temp678) == Cyc_Absyn_CompoundLit_e){
goto _LL729;} else{ goto _LL730;} _LL730: if(*(( int*) _temp678) == Cyc_Absyn_Array_e){
goto _LL731;} else{ goto _LL732;} _LL732: if(*(( int*) _temp678) == Cyc_Absyn_Comprehension_e){
goto _LL733;} else{ goto _LL734;} _LL734: if(*(( int*) _temp678) == Cyc_Absyn_Struct_e){
goto _LL735;} else{ goto _LL736;} _LL736: if(*(( int*) _temp678) == Cyc_Absyn_AnonStruct_e){
goto _LL737;} else{ goto _LL738;} _LL738: if(*(( int*) _temp678) == Cyc_Absyn_Tunion_e){
goto _LL739;} else{ goto _LL740;} _LL740: if(*(( int*) _temp678) == Cyc_Absyn_Enum_e){
goto _LL741;} else{ goto _LL742;} _LL742: if(*(( int*) _temp678) == Cyc_Absyn_Malloc_e){
goto _LL743;} else{ goto _LL744;} _LL744: if(*(( int*) _temp678) == Cyc_Absyn_UnresolvedMem_e){
goto _LL745;} else{ goto _LL746;} _LL746: if(*(( int*) _temp678) == Cyc_Absyn_StmtExp_e){
goto _LL747;} else{ goto _LL748;} _LL748: if(*(( int*) _temp678) == Cyc_Absyn_Codegen_e){
goto _LL749;} else{ goto _LL750;} _LL750: if(*(( int*) _temp678) == Cyc_Absyn_Fill_e){
goto _LL751;} else{ goto _LL679;} _LL681: return 10000; _LL683: return 10000;
_LL685: return 10000; _LL687: { void* _temp758= _temp752; _LL760: if( _temp758
==( void*) Cyc_Absyn_Plus){ goto _LL761;} else{ goto _LL762;} _LL762: if(
_temp758 ==( void*) Cyc_Absyn_Times){ goto _LL763;} else{ goto _LL764;} _LL764:
if( _temp758 ==( void*) Cyc_Absyn_Minus){ goto _LL765;} else{ goto _LL766;}
_LL766: if( _temp758 ==( void*) Cyc_Absyn_Div){ goto _LL767;} else{ goto _LL768;}
_LL768: if( _temp758 ==( void*) Cyc_Absyn_Mod){ goto _LL769;} else{ goto _LL770;}
_LL770: if( _temp758 ==( void*) Cyc_Absyn_Eq){ goto _LL771;} else{ goto _LL772;}
_LL772: if( _temp758 ==( void*) Cyc_Absyn_Neq){ goto _LL773;} else{ goto _LL774;}
_LL774: if( _temp758 ==( void*) Cyc_Absyn_Gt){ goto _LL775;} else{ goto _LL776;}
_LL776: if( _temp758 ==( void*) Cyc_Absyn_Lt){ goto _LL777;} else{ goto _LL778;}
_LL778: if( _temp758 ==( void*) Cyc_Absyn_Gte){ goto _LL779;} else{ goto _LL780;}
_LL780: if( _temp758 ==( void*) Cyc_Absyn_Lte){ goto _LL781;} else{ goto _LL782;}
_LL782: if( _temp758 ==( void*) Cyc_Absyn_Not){ goto _LL783;} else{ goto _LL784;}
_LL784: if( _temp758 ==( void*) Cyc_Absyn_Bitnot){ goto _LL785;} else{ goto
_LL786;} _LL786: if( _temp758 ==( void*) Cyc_Absyn_Bitand){ goto _LL787;} else{
goto _LL788;} _LL788: if( _temp758 ==( void*) Cyc_Absyn_Bitor){ goto _LL789;}
else{ goto _LL790;} _LL790: if( _temp758 ==( void*) Cyc_Absyn_Bitxor){ goto
_LL791;} else{ goto _LL792;} _LL792: if( _temp758 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL793;} else{ goto _LL794;} _LL794: if( _temp758 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL795;} else{ goto _LL796;} _LL796: if( _temp758 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL797;} else{ goto _LL798;} _LL798: if( _temp758 ==( void*) Cyc_Absyn_Size){
goto _LL799;} else{ goto _LL800;} _LL800: if( _temp758 ==( void*) Cyc_Absyn_Printf){
goto _LL801;} else{ goto _LL802;} _LL802: if( _temp758 ==( void*) Cyc_Absyn_Fprintf){
goto _LL803;} else{ goto _LL804;} _LL804: if( _temp758 ==( void*) Cyc_Absyn_Xprintf){
goto _LL805;} else{ goto _LL806;} _LL806: if( _temp758 ==( void*) Cyc_Absyn_Scanf){
goto _LL807;} else{ goto _LL808;} _LL808: if( _temp758 ==( void*) Cyc_Absyn_Fscanf){
goto _LL809;} else{ goto _LL810;} _LL810: if( _temp758 ==( void*) Cyc_Absyn_Sscanf){
goto _LL811;} else{ goto _LL759;} _LL761: return 100; _LL763: return 110; _LL765:
return 100; _LL767: return 110; _LL769: return 110; _LL771: return 70; _LL773:
return 70; _LL775: return 80; _LL777: return 80; _LL779: return 80; _LL781:
return 80; _LL783: return 130; _LL785: return 130; _LL787: return 60; _LL789:
return 40; _LL791: return 50; _LL793: return 90; _LL795: return 80; _LL797:
return 80; _LL799: return 140; _LL801: return 130; _LL803: return 130; _LL805:
return 130; _LL807: return 130; _LL809: return 130; _LL811: return 130; _LL759:;}
_LL689: return 20; _LL691: return 130; _LL693: return 30; _LL695: return 10;
_LL697: return 140; _LL699: return 140; _LL701: return 130; _LL703: return Cyc_Absynpp_exp_prec(
_temp754); _LL705: return Cyc_Absynpp_exp_prec( _temp756); _LL707: return 120;
_LL709: return 15; _LL711: return 130; _LL713: return 130; _LL715: return 130;
_LL717: return 130; _LL719: return 130; _LL721: return 140; _LL723: return 140;
_LL725: return 140; _LL727: return 150; _LL729: goto _LL731; _LL731: goto _LL733;
_LL733: goto _LL735; _LL735: goto _LL737; _LL737: goto _LL739; _LL739: goto
_LL741; _LL741: goto _LL743; _LL743: return 140; _LL745: return 140; _LL747:
return 10000; _LL749: return 140; _LL751: return 140; _LL679:;} struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp* e){ return Cyc_Absynpp_exp2doc_prec(
0, e);} struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec( int inprec, struct Cyc_Absyn_Exp*
e){ int myprec= Cyc_Absynpp_exp_prec( e); struct Cyc_PP_Doc* s;{ void* _temp812=(
void*) e->r; void* _temp886; struct _tuple0* _temp888; struct _tuple0* _temp890;
struct Cyc_List_List* _temp892; void* _temp894; struct Cyc_Absyn_Exp* _temp896;
struct Cyc_Core_Opt* _temp898; struct Cyc_Absyn_Exp* _temp900; void* _temp902;
struct Cyc_Absyn_Exp* _temp904; struct Cyc_Absyn_Exp* _temp906; struct Cyc_Absyn_Exp*
_temp908; struct Cyc_Absyn_Exp* _temp910; struct Cyc_Absyn_Exp* _temp912; struct
Cyc_Absyn_Exp* _temp914; struct Cyc_List_List* _temp916; struct Cyc_Absyn_Exp*
_temp918; struct Cyc_List_List* _temp920; struct Cyc_Absyn_Exp* _temp922; struct
Cyc_Absyn_Exp* _temp924; struct Cyc_Absyn_Exp* _temp926; struct Cyc_Absyn_Exp*
_temp928; struct Cyc_Absyn_Exp* _temp930; void* _temp932; struct Cyc_Absyn_Exp*
_temp934; struct Cyc_Absyn_Exp* _temp936; struct Cyc_Absyn_Exp* _temp938; void*
_temp940; struct Cyc_Absyn_Exp* _temp942; struct _tagged_arr* _temp944; void*
_temp946; struct Cyc_Absyn_Exp* _temp948; struct _tagged_arr* _temp950; struct
Cyc_Absyn_Exp* _temp952; struct _tagged_arr* _temp954; struct Cyc_Absyn_Exp*
_temp956; struct Cyc_Absyn_Exp* _temp958; struct Cyc_Absyn_Exp* _temp960; struct
Cyc_List_List* _temp962; struct Cyc_List_List* _temp964; struct _tuple1*
_temp966; struct Cyc_List_List* _temp968; struct Cyc_Absyn_Exp* _temp970; struct
Cyc_Absyn_Exp* _temp972; struct Cyc_Absyn_Vardecl* _temp974; struct Cyc_List_List*
_temp976; struct _tuple0* _temp978; struct Cyc_List_List* _temp980; struct Cyc_Absyn_Tunionfield*
_temp982; struct Cyc_List_List* _temp984; struct _tuple0* _temp986; void*
_temp988; struct Cyc_Absyn_Exp* _temp990; struct Cyc_List_List* _temp992; struct
Cyc_Core_Opt* _temp994; struct Cyc_Absyn_Stmt* _temp996; struct Cyc_Absyn_Fndecl*
_temp998; struct Cyc_Absyn_Exp* _temp1000; _LL814: if(*(( int*) _temp812) == Cyc_Absyn_Const_e){
_LL887: _temp886=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp812)->f1;
goto _LL815;} else{ goto _LL816;} _LL816: if(*(( int*) _temp812) == Cyc_Absyn_Var_e){
_LL889: _temp888=(( struct Cyc_Absyn_Var_e_struct*) _temp812)->f1; goto _LL817;}
else{ goto _LL818;} _LL818: if(*(( int*) _temp812) == Cyc_Absyn_UnknownId_e){
_LL891: _temp890=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp812)->f1; goto
_LL819;} else{ goto _LL820;} _LL820: if(*(( int*) _temp812) == Cyc_Absyn_Primop_e){
_LL895: _temp894=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp812)->f1;
goto _LL893; _LL893: _temp892=(( struct Cyc_Absyn_Primop_e_struct*) _temp812)->f2;
goto _LL821;} else{ goto _LL822;} _LL822: if(*(( int*) _temp812) == Cyc_Absyn_AssignOp_e){
_LL901: _temp900=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp812)->f1; goto
_LL899; _LL899: _temp898=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp812)->f2;
goto _LL897; _LL897: _temp896=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp812)->f3;
goto _LL823;} else{ goto _LL824;} _LL824: if(*(( int*) _temp812) == Cyc_Absyn_Increment_e){
_LL905: _temp904=(( struct Cyc_Absyn_Increment_e_struct*) _temp812)->f1; goto
_LL903; _LL903: _temp902=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp812)->f2; goto _LL825;} else{ goto _LL826;} _LL826: if(*(( int*) _temp812)
== Cyc_Absyn_Conditional_e){ _LL911: _temp910=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp812)->f1; goto _LL909; _LL909: _temp908=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp812)->f2; goto _LL907; _LL907: _temp906=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp812)->f3; goto _LL827;} else{ goto _LL828;} _LL828: if(*(( int*) _temp812)
== Cyc_Absyn_SeqExp_e){ _LL915: _temp914=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp812)->f1; goto _LL913; _LL913: _temp912=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp812)->f2; goto _LL829;} else{ goto _LL830;} _LL830: if(*(( int*) _temp812)
== Cyc_Absyn_UnknownCall_e){ _LL919: _temp918=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp812)->f1; goto _LL917; _LL917: _temp916=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp812)->f2; goto _LL831;} else{ goto _LL832;} _LL832: if(*(( int*) _temp812)
== Cyc_Absyn_FnCall_e){ _LL923: _temp922=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp812)->f1; goto _LL921; _LL921: _temp920=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp812)->f2; goto _LL833;} else{ goto _LL834;} _LL834: if(*(( int*) _temp812)
== Cyc_Absyn_Throw_e){ _LL925: _temp924=(( struct Cyc_Absyn_Throw_e_struct*)
_temp812)->f1; goto _LL835;} else{ goto _LL836;} _LL836: if(*(( int*) _temp812)
== Cyc_Absyn_NoInstantiate_e){ _LL927: _temp926=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp812)->f1; goto _LL837;} else{ goto _LL838;} _LL838: if(*(( int*) _temp812)
== Cyc_Absyn_Instantiate_e){ _LL929: _temp928=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp812)->f1; goto _LL839;} else{ goto _LL840;} _LL840: if(*(( int*) _temp812)
== Cyc_Absyn_Cast_e){ _LL933: _temp932=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp812)->f1; goto _LL931; _LL931: _temp930=(( struct Cyc_Absyn_Cast_e_struct*)
_temp812)->f2; goto _LL841;} else{ goto _LL842;} _LL842: if(*(( int*) _temp812)
== Cyc_Absyn_Address_e){ _LL935: _temp934=(( struct Cyc_Absyn_Address_e_struct*)
_temp812)->f1; goto _LL843;} else{ goto _LL844;} _LL844: if(*(( int*) _temp812)
== Cyc_Absyn_New_e){ _LL939: _temp938=(( struct Cyc_Absyn_New_e_struct*)
_temp812)->f1; goto _LL937; _LL937: _temp936=(( struct Cyc_Absyn_New_e_struct*)
_temp812)->f2; goto _LL845;} else{ goto _LL846;} _LL846: if(*(( int*) _temp812)
== Cyc_Absyn_Sizeoftyp_e){ _LL941: _temp940=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp812)->f1; goto _LL847;} else{ goto _LL848;} _LL848: if(*(( int*) _temp812)
== Cyc_Absyn_Sizeofexp_e){ _LL943: _temp942=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp812)->f1; goto _LL849;} else{ goto _LL850;} _LL850: if(*(( int*) _temp812)
== Cyc_Absyn_Offsetof_e){ _LL947: _temp946=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp812)->f1; goto _LL945; _LL945: _temp944=(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp812)->f2; goto _LL851;} else{ goto _LL852;} _LL852: if(*(( int*) _temp812)
== Cyc_Absyn_Deref_e){ _LL949: _temp948=(( struct Cyc_Absyn_Deref_e_struct*)
_temp812)->f1; goto _LL853;} else{ goto _LL854;} _LL854: if(*(( int*) _temp812)
== Cyc_Absyn_StructMember_e){ _LL953: _temp952=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp812)->f1; goto _LL951; _LL951: _temp950=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp812)->f2; goto _LL855;} else{ goto _LL856;} _LL856: if(*(( int*) _temp812)
== Cyc_Absyn_StructArrow_e){ _LL957: _temp956=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp812)->f1; goto _LL955; _LL955: _temp954=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp812)->f2; goto _LL857;} else{ goto _LL858;} _LL858: if(*(( int*) _temp812)
== Cyc_Absyn_Subscript_e){ _LL961: _temp960=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp812)->f1; goto _LL959; _LL959: _temp958=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp812)->f2; goto _LL859;} else{ goto _LL860;} _LL860: if(*(( int*) _temp812)
== Cyc_Absyn_Tuple_e){ _LL963: _temp962=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp812)->f1; goto _LL861;} else{ goto _LL862;} _LL862: if(*(( int*) _temp812)
== Cyc_Absyn_CompoundLit_e){ _LL967: _temp966=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp812)->f1; goto _LL965; _LL965: _temp964=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp812)->f2; goto _LL863;} else{ goto _LL864;} _LL864: if(*(( int*) _temp812)
== Cyc_Absyn_Array_e){ _LL969: _temp968=(( struct Cyc_Absyn_Array_e_struct*)
_temp812)->f1; goto _LL865;} else{ goto _LL866;} _LL866: if(*(( int*) _temp812)
== Cyc_Absyn_Comprehension_e){ _LL975: _temp974=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp812)->f1; goto _LL973; _LL973: _temp972=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp812)->f2; goto _LL971; _LL971: _temp970=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp812)->f3; goto _LL867;} else{ goto _LL868;} _LL868: if(*(( int*) _temp812)
== Cyc_Absyn_Struct_e){ _LL979: _temp978=(( struct Cyc_Absyn_Struct_e_struct*)
_temp812)->f1; goto _LL977; _LL977: _temp976=(( struct Cyc_Absyn_Struct_e_struct*)
_temp812)->f3; goto _LL869;} else{ goto _LL870;} _LL870: if(*(( int*) _temp812)
== Cyc_Absyn_AnonStruct_e){ _LL981: _temp980=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp812)->f2; goto _LL871;} else{ goto _LL872;} _LL872: if(*(( int*) _temp812)
== Cyc_Absyn_Tunion_e){ _LL985: _temp984=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp812)->f3; goto _LL983; _LL983: _temp982=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp812)->f5; goto _LL873;} else{ goto _LL874;} _LL874: if(*(( int*) _temp812)
== Cyc_Absyn_Enum_e){ _LL987: _temp986=(( struct Cyc_Absyn_Enum_e_struct*)
_temp812)->f1; goto _LL875;} else{ goto _LL876;} _LL876: if(*(( int*) _temp812)
== Cyc_Absyn_Malloc_e){ _LL991: _temp990=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp812)->f1; goto _LL989; _LL989: _temp988=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp812)->f2; goto _LL877;} else{ goto _LL878;} _LL878: if(*(( int*) _temp812)
== Cyc_Absyn_UnresolvedMem_e){ _LL995: _temp994=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp812)->f1; goto _LL993; _LL993: _temp992=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp812)->f2; goto _LL879;} else{ goto _LL880;} _LL880: if(*(( int*) _temp812)
== Cyc_Absyn_StmtExp_e){ _LL997: _temp996=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp812)->f1; goto _LL881;} else{ goto _LL882;} _LL882: if(*(( int*) _temp812)
== Cyc_Absyn_Codegen_e){ _LL999: _temp998=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp812)->f1; goto _LL883;} else{ goto _LL884;} _LL884: if(*(( int*) _temp812)
== Cyc_Absyn_Fill_e){ _LL1001: _temp1000=(( struct Cyc_Absyn_Fill_e_struct*)
_temp812)->f1; goto _LL885;} else{ goto _LL813;} _LL815: s= Cyc_Absynpp_cnst2doc(
_temp886); goto _LL813; _LL817: _temp890= _temp888; goto _LL819; _LL819: s= Cyc_Absynpp_qvar2doc(
_temp890); goto _LL813; _LL821: s= Cyc_Absynpp_primapp2doc( myprec, _temp894,
_temp892); goto _LL813; _LL823: s=({ struct Cyc_PP_Doc*(* _temp1002)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1005= Cyc_Absynpp_exp2doc_prec(
myprec, _temp900); struct Cyc_PP_Doc* _temp1006= Cyc_PP_text( _tag_arr(" ",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1007= _temp898 == 0? Cyc_PP_text(
_tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_prim2doc(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp898))->v); struct Cyc_PP_Doc* _temp1008=
Cyc_PP_text( _tag_arr("= ", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc*
_temp1009= Cyc_Absynpp_exp2doc_prec( myprec, _temp896); struct Cyc_PP_Doc*
_temp1003[ 5u]={ _temp1005, _temp1006, _temp1007, _temp1008, _temp1009}; struct
_tagged_arr _temp1004={( void*) _temp1003,( void*) _temp1003,( void*)( _temp1003
+ 5u)}; _temp1002( _temp1004);}); goto _LL813; _LL825: { struct Cyc_PP_Doc*
_temp1010= Cyc_Absynpp_exp2doc_prec( myprec, _temp904);{ void* _temp1011=
_temp902; _LL1013: if( _temp1011 ==( void*) Cyc_Absyn_PreInc){ goto _LL1014;}
else{ goto _LL1015;} _LL1015: if( _temp1011 ==( void*) Cyc_Absyn_PreDec){ goto
_LL1016;} else{ goto _LL1017;} _LL1017: if( _temp1011 ==( void*) Cyc_Absyn_PostInc){
goto _LL1018;} else{ goto _LL1019;} _LL1019: if( _temp1011 ==( void*) Cyc_Absyn_PostDec){
goto _LL1020;} else{ goto _LL1012;} _LL1014: s=({ struct Cyc_PP_Doc*(* _temp1021)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1024= Cyc_PP_text(
_tag_arr("++", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1025=
_temp1010; struct Cyc_PP_Doc* _temp1022[ 2u]={ _temp1024, _temp1025}; struct
_tagged_arr _temp1023={( void*) _temp1022,( void*) _temp1022,( void*)( _temp1022
+ 2u)}; _temp1021( _temp1023);}); goto _LL1012; _LL1016: s=({ struct Cyc_PP_Doc*(*
_temp1026)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1029= Cyc_PP_text(
_tag_arr("--", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1030=
_temp1010; struct Cyc_PP_Doc* _temp1027[ 2u]={ _temp1029, _temp1030}; struct
_tagged_arr _temp1028={( void*) _temp1027,( void*) _temp1027,( void*)( _temp1027
+ 2u)}; _temp1026( _temp1028);}); goto _LL1012; _LL1018: s=({ struct Cyc_PP_Doc*(*
_temp1031)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1034=
_temp1010; struct Cyc_PP_Doc* _temp1035= Cyc_PP_text( _tag_arr("++", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp1032[ 2u]={ _temp1034, _temp1035};
struct _tagged_arr _temp1033={( void*) _temp1032,( void*) _temp1032,( void*)(
_temp1032 + 2u)}; _temp1031( _temp1033);}); goto _LL1012; _LL1020: s=({ struct
Cyc_PP_Doc*(* _temp1036)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1039= _temp1010; struct Cyc_PP_Doc* _temp1040= Cyc_PP_text( _tag_arr("--",
sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1037[ 2u]={ _temp1039,
_temp1040}; struct _tagged_arr _temp1038={( void*) _temp1037,( void*) _temp1037,(
void*)( _temp1037 + 2u)}; _temp1036( _temp1038);}); goto _LL1012; _LL1012:;}
goto _LL813;} _LL827:{ struct _tuple8 _temp1042=({ struct _tuple8 _temp1041;
_temp1041.f1=( void*) _temp908->r; _temp1041.f2=( void*) _temp906->r; _temp1041;});
_LL1044: goto _LL1045; _LL1045: s=({ struct Cyc_PP_Doc*(* _temp1046)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1049= Cyc_Absynpp_exp2doc_prec(
myprec, _temp910); struct Cyc_PP_Doc* _temp1050= Cyc_PP_text( _tag_arr(" ? ",
sizeof( unsigned char), 4u)); struct Cyc_PP_Doc* _temp1051= Cyc_Absynpp_exp2doc_prec(
0, _temp908); struct Cyc_PP_Doc* _temp1052= Cyc_PP_text( _tag_arr(" : ", sizeof(
unsigned char), 4u)); struct Cyc_PP_Doc* _temp1053= Cyc_Absynpp_exp2doc_prec(
myprec, _temp906); struct Cyc_PP_Doc* _temp1047[ 5u]={ _temp1049, _temp1050,
_temp1051, _temp1052, _temp1053}; struct _tagged_arr _temp1048={( void*)
_temp1047,( void*) _temp1047,( void*)( _temp1047 + 5u)}; _temp1046( _temp1048);});
goto _LL1043; _LL1043:;} goto _LL813; _LL829: s=({ struct Cyc_PP_Doc*(*
_temp1054)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1057= Cyc_PP_text(
_tag_arr("(", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1058= Cyc_Absynpp_exp2doc(
_temp914); struct Cyc_PP_Doc* _temp1059= Cyc_PP_text( _tag_arr(", ", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp1060= Cyc_Absynpp_exp2doc(
_temp912); struct Cyc_PP_Doc* _temp1061= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1055[ 5u]={ _temp1057, _temp1058,
_temp1059, _temp1060, _temp1061}; struct _tagged_arr _temp1056={( void*)
_temp1055,( void*) _temp1055,( void*)( _temp1055 + 5u)}; _temp1054( _temp1056);});
goto _LL813; _LL831: s=({ struct Cyc_PP_Doc*(* _temp1062)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1065= Cyc_Absynpp_exp2doc_prec( myprec,
_temp918); struct Cyc_PP_Doc* _temp1066= Cyc_PP_text( _tag_arr("(", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1067= Cyc_Absynpp_exps2doc_prec(
20, _temp916); struct Cyc_PP_Doc* _temp1068= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1063[ 4u]={ _temp1065, _temp1066,
_temp1067, _temp1068}; struct _tagged_arr _temp1064={( void*) _temp1063,( void*)
_temp1063,( void*)( _temp1063 + 4u)}; _temp1062( _temp1064);}); goto _LL813;
_LL833: s=({ struct Cyc_PP_Doc*(* _temp1069)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1072= Cyc_Absynpp_exp2doc_prec( myprec, _temp922);
struct Cyc_PP_Doc* _temp1073= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp1074= Cyc_Absynpp_exps2doc_prec( 20, _temp920);
struct Cyc_PP_Doc* _temp1075= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp1070[ 4u]={ _temp1072, _temp1073, _temp1074,
_temp1075}; struct _tagged_arr _temp1071={( void*) _temp1070,( void*) _temp1070,(
void*)( _temp1070 + 4u)}; _temp1069( _temp1071);}); goto _LL813; _LL835: s=({
struct Cyc_PP_Doc*(* _temp1076)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1079= Cyc_PP_text( _tag_arr("throw ", sizeof( unsigned char), 7u)); struct
Cyc_PP_Doc* _temp1080= Cyc_Absynpp_exp2doc_prec( myprec, _temp924); struct Cyc_PP_Doc*
_temp1077[ 2u]={ _temp1079, _temp1080}; struct _tagged_arr _temp1078={( void*)
_temp1077,( void*) _temp1077,( void*)( _temp1077 + 2u)}; _temp1076( _temp1078);});
goto _LL813; _LL837: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp926); goto _LL813;
_LL839: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp928); goto _LL813; _LL841: s=({
struct Cyc_PP_Doc*(* _temp1081)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1084= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1085= Cyc_Absynpp_typ2doc( _temp932); struct Cyc_PP_Doc* _temp1086= Cyc_PP_text(
_tag_arr(")", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1087= Cyc_Absynpp_exp2doc_prec(
myprec, _temp930); struct Cyc_PP_Doc* _temp1082[ 4u]={ _temp1084, _temp1085,
_temp1086, _temp1087}; struct _tagged_arr _temp1083={( void*) _temp1082,( void*)
_temp1082,( void*)( _temp1082 + 4u)}; _temp1081( _temp1083);}); goto _LL813;
_LL843: s=({ struct Cyc_PP_Doc*(* _temp1088)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1091= Cyc_PP_text( _tag_arr("&", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp1092= Cyc_Absynpp_exp2doc_prec( myprec, _temp934);
struct Cyc_PP_Doc* _temp1089[ 2u]={ _temp1091, _temp1092}; struct _tagged_arr
_temp1090={( void*) _temp1089,( void*) _temp1089,( void*)( _temp1089 + 2u)};
_temp1088( _temp1090);}); goto _LL813; _LL845: if( _temp938 == 0){ s=({ struct
Cyc_PP_Doc*(* _temp1093)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1096= Cyc_PP_text( _tag_arr("new ", sizeof( unsigned char), 5u)); struct
Cyc_PP_Doc* _temp1097= Cyc_Absynpp_exp2doc_prec( myprec, _temp936); struct Cyc_PP_Doc*
_temp1094[ 2u]={ _temp1096, _temp1097}; struct _tagged_arr _temp1095={( void*)
_temp1094,( void*) _temp1094,( void*)( _temp1094 + 2u)}; _temp1093( _temp1095);});}
else{ s=({ struct Cyc_PP_Doc*(* _temp1098)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1101= Cyc_PP_text( _tag_arr("rnew(", sizeof(
unsigned char), 6u)); struct Cyc_PP_Doc* _temp1102= Cyc_Absynpp_exp2doc(( struct
Cyc_Absyn_Exp*) _check_null( _temp938)); struct Cyc_PP_Doc* _temp1103= Cyc_PP_text(
_tag_arr(") ", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1104= Cyc_Absynpp_exp2doc_prec(
myprec, _temp936); struct Cyc_PP_Doc* _temp1099[ 4u]={ _temp1101, _temp1102,
_temp1103, _temp1104}; struct _tagged_arr _temp1100={( void*) _temp1099,( void*)
_temp1099,( void*)( _temp1099 + 4u)}; _temp1098( _temp1100);});} goto _LL813;
_LL847: s=({ struct Cyc_PP_Doc*(* _temp1105)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1108= Cyc_PP_text( _tag_arr("sizeof(", sizeof(
unsigned char), 8u)); struct Cyc_PP_Doc* _temp1109= Cyc_Absynpp_typ2doc(
_temp940); struct Cyc_PP_Doc* _temp1110= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1106[ 3u]={ _temp1108, _temp1109,
_temp1110}; struct _tagged_arr _temp1107={( void*) _temp1106,( void*) _temp1106,(
void*)( _temp1106 + 3u)}; _temp1105( _temp1107);}); goto _LL813; _LL849: s=({
struct Cyc_PP_Doc*(* _temp1111)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1114= Cyc_PP_text( _tag_arr("sizeof(", sizeof( unsigned char), 8u)); struct
Cyc_PP_Doc* _temp1115= Cyc_Absynpp_exp2doc( _temp942); struct Cyc_PP_Doc*
_temp1116= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1112[ 3u]={ _temp1114, _temp1115, _temp1116}; struct _tagged_arr _temp1113={(
void*) _temp1112,( void*) _temp1112,( void*)( _temp1112 + 3u)}; _temp1111(
_temp1113);}); goto _LL813; _LL851: s=({ struct Cyc_PP_Doc*(* _temp1117)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1120= Cyc_PP_text( _tag_arr("offsetof(",
sizeof( unsigned char), 10u)); struct Cyc_PP_Doc* _temp1121= Cyc_Absynpp_typ2doc(
_temp946); struct Cyc_PP_Doc* _temp1122= Cyc_PP_text( _tag_arr(",", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1123= Cyc_PP_textptr( _temp944);
struct Cyc_PP_Doc* _temp1124= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp1118[ 5u]={ _temp1120, _temp1121, _temp1122,
_temp1123, _temp1124}; struct _tagged_arr _temp1119={( void*) _temp1118,( void*)
_temp1118,( void*)( _temp1118 + 5u)}; _temp1117( _temp1119);}); goto _LL813;
_LL853: s=({ struct Cyc_PP_Doc*(* _temp1125)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1128= Cyc_PP_text( _tag_arr("*", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp1129= Cyc_Absynpp_exp2doc_prec( myprec, _temp948);
struct Cyc_PP_Doc* _temp1126[ 2u]={ _temp1128, _temp1129}; struct _tagged_arr
_temp1127={( void*) _temp1126,( void*) _temp1126,( void*)( _temp1126 + 2u)};
_temp1125( _temp1127);}); goto _LL813; _LL855: s=({ struct Cyc_PP_Doc*(*
_temp1130)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1133= Cyc_Absynpp_exp2doc_prec(
myprec, _temp952); struct Cyc_PP_Doc* _temp1134= Cyc_PP_text( _tag_arr(".",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1135= Cyc_PP_textptr(
_temp950); struct Cyc_PP_Doc* _temp1131[ 3u]={ _temp1133, _temp1134, _temp1135};
struct _tagged_arr _temp1132={( void*) _temp1131,( void*) _temp1131,( void*)(
_temp1131 + 3u)}; _temp1130( _temp1132);}); goto _LL813; _LL857: s=({ struct Cyc_PP_Doc*(*
_temp1136)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1139= Cyc_Absynpp_exp2doc_prec(
myprec, _temp956); struct Cyc_PP_Doc* _temp1140= Cyc_PP_text( _tag_arr("->",
sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1141= Cyc_PP_textptr(
_temp954); struct Cyc_PP_Doc* _temp1137[ 3u]={ _temp1139, _temp1140, _temp1141};
struct _tagged_arr _temp1138={( void*) _temp1137,( void*) _temp1137,( void*)(
_temp1137 + 3u)}; _temp1136( _temp1138);}); goto _LL813; _LL859: s=({ struct Cyc_PP_Doc*(*
_temp1142)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1145= Cyc_Absynpp_exp2doc_prec(
myprec, _temp960); struct Cyc_PP_Doc* _temp1146= Cyc_PP_text( _tag_arr("[",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1147= Cyc_Absynpp_exp2doc(
_temp958); struct Cyc_PP_Doc* _temp1148= Cyc_PP_text( _tag_arr("]", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1143[ 4u]={ _temp1145, _temp1146,
_temp1147, _temp1148}; struct _tagged_arr _temp1144={( void*) _temp1143,( void*)
_temp1143,( void*)( _temp1143 + 4u)}; _temp1142( _temp1144);}); goto _LL813;
_LL861: s=({ struct Cyc_PP_Doc*(* _temp1149)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1152= Cyc_PP_text( _tag_arr("$(", sizeof( unsigned char),
3u)); struct Cyc_PP_Doc* _temp1153= Cyc_Absynpp_exps2doc_prec( 20, _temp962);
struct Cyc_PP_Doc* _temp1154= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp1150[ 3u]={ _temp1152, _temp1153, _temp1154};
struct _tagged_arr _temp1151={( void*) _temp1150,( void*) _temp1150,( void*)(
_temp1150 + 3u)}; _temp1149( _temp1151);}); goto _LL813; _LL863: s=({ struct Cyc_PP_Doc*(*
_temp1155)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1158= Cyc_PP_text(
_tag_arr("(", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1159= Cyc_Absynpp_typ2doc((*
_temp966).f3); struct Cyc_PP_Doc* _temp1160= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1161= Cyc_PP_group( _tag_arr("{",
sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp964)); struct Cyc_PP_Doc* _temp1156[ 4u]={ _temp1158, _temp1159, _temp1160,
_temp1161}; struct _tagged_arr _temp1157={( void*) _temp1156,( void*) _temp1156,(
void*)( _temp1156 + 4u)}; _temp1155( _temp1157);}); goto _LL813; _LL865: s= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp968)); goto _LL813; _LL867: s=({ struct Cyc_PP_Doc*(*
_temp1162)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1165= Cyc_PP_text(
_tag_arr("{for ", sizeof( unsigned char), 6u)); struct Cyc_PP_Doc* _temp1166=
Cyc_PP_text(*(* _temp974->name).f2); struct Cyc_PP_Doc* _temp1167= Cyc_PP_text(
_tag_arr(" < ", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc* _temp1168= Cyc_Absynpp_exp2doc(
_temp972); struct Cyc_PP_Doc* _temp1169= Cyc_PP_text( _tag_arr(" : ", sizeof(
unsigned char), 4u)); struct Cyc_PP_Doc* _temp1170= Cyc_Absynpp_exp2doc(
_temp970); struct Cyc_PP_Doc* _temp1171= Cyc_PP_text( _tag_arr("}", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1163[ 7u]={ _temp1165, _temp1166,
_temp1167, _temp1168, _temp1169, _temp1170, _temp1171}; struct _tagged_arr
_temp1164={( void*) _temp1163,( void*) _temp1163,( void*)( _temp1163 + 7u)};
_temp1162( _temp1164);}); goto _LL813; _LL869: s=({ struct Cyc_PP_Doc*(*
_temp1172)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1175= Cyc_Absynpp_qvar2doc(
_temp978); struct Cyc_PP_Doc* _temp1176= Cyc_PP_group( _tag_arr("{", sizeof(
unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp976)); struct Cyc_PP_Doc* _temp1173[ 2u]={ _temp1175, _temp1176}; struct
_tagged_arr _temp1174={( void*) _temp1173,( void*) _temp1173,( void*)( _temp1173
+ 2u)}; _temp1172( _temp1174);}); goto _LL813; _LL871: s= Cyc_PP_group( _tag_arr("{",
sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp980)); goto _LL813; _LL873: if( _temp984 == 0){ s= Cyc_Absynpp_qvar2doc(
_temp982->name);} else{ s=({ struct Cyc_PP_Doc*(* _temp1177)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1180= Cyc_Absynpp_qvar2doc( _temp982->name);
struct Cyc_PP_Doc* _temp1181= Cyc_PP_egroup( _tag_arr("(", sizeof( unsigned char),
2u), _tag_arr(")", sizeof( unsigned char), 2u), _tag_arr(",", sizeof(
unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_exp2doc,
_temp984)); struct Cyc_PP_Doc* _temp1178[ 2u]={ _temp1180, _temp1181}; struct
_tagged_arr _temp1179={( void*) _temp1178,( void*) _temp1178,( void*)( _temp1178
+ 2u)}; _temp1177( _temp1179);});} goto _LL813; _LL875: s= Cyc_Absynpp_qvar2doc(
_temp986); goto _LL813; _LL877: if( _temp990 == 0){ s=({ struct Cyc_PP_Doc*(*
_temp1182)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1185= Cyc_PP_text(
_tag_arr("malloc(sizeof(", sizeof( unsigned char), 15u)); struct Cyc_PP_Doc*
_temp1186= Cyc_Absynpp_typ2doc( _temp988); struct Cyc_PP_Doc* _temp1187= Cyc_PP_text(
_tag_arr("))", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1183[ 3u]={
_temp1185, _temp1186, _temp1187}; struct _tagged_arr _temp1184={( void*)
_temp1183,( void*) _temp1183,( void*)( _temp1183 + 3u)}; _temp1182( _temp1184);});}
else{ s=({ struct Cyc_PP_Doc*(* _temp1188)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1191= Cyc_PP_text( _tag_arr("rmalloc(", sizeof(
unsigned char), 9u)); struct Cyc_PP_Doc* _temp1192= Cyc_Absynpp_exp2doc(( struct
Cyc_Absyn_Exp*) _check_null( _temp990)); struct Cyc_PP_Doc* _temp1193= Cyc_PP_text(
_tag_arr(",", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1194= Cyc_PP_text(
_tag_arr("sizeof(", sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp1195=
Cyc_Absynpp_typ2doc( _temp988); struct Cyc_PP_Doc* _temp1196= Cyc_PP_text(
_tag_arr("))", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1189[ 6u]={
_temp1191, _temp1192, _temp1193, _temp1194, _temp1195, _temp1196}; struct
_tagged_arr _temp1190={( void*) _temp1189,( void*) _temp1189,( void*)( _temp1189
+ 6u)}; _temp1188( _temp1190);});} goto _LL813; _LL879: s= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp992)); goto _LL813; _LL881: s=({ struct Cyc_PP_Doc*(*
_temp1197)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1200= Cyc_PP_text(
_tag_arr("({ ", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc* _temp1201= Cyc_PP_nest(
2, Cyc_Absynpp_stmt2doc( _temp996)); struct Cyc_PP_Doc* _temp1202= Cyc_PP_text(
_tag_arr(" })", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc* _temp1198[ 3u]={
_temp1200, _temp1201, _temp1202}; struct _tagged_arr _temp1199={( void*)
_temp1198,( void*) _temp1198,( void*)( _temp1198 + 3u)}; _temp1197( _temp1199);});
goto _LL813; _LL883: s=({ struct Cyc_PP_Doc*(* _temp1203)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1206= Cyc_PP_text( _tag_arr("codegen(",
sizeof( unsigned char), 9u)); struct Cyc_PP_Doc* _temp1207= Cyc_PP_nest( 2, Cyc_Absynpp_decl2doc(({
struct Cyc_Absyn_Decl* _temp1209=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof(
struct Cyc_Absyn_Decl)); _temp1209->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp1210=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1210[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp1211; _temp1211.tag= Cyc_Absyn_Fn_d;
_temp1211.f1= _temp998; _temp1211;}); _temp1210;})); _temp1209->loc= e->loc;
_temp1209;}))); struct Cyc_PP_Doc* _temp1208= Cyc_PP_text( _tag_arr(")", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1204[ 3u]={ _temp1206, _temp1207,
_temp1208}; struct _tagged_arr _temp1205={( void*) _temp1204,( void*) _temp1204,(
void*)( _temp1204 + 3u)}; _temp1203( _temp1205);}); goto _LL813; _LL885: s=({
struct Cyc_PP_Doc*(* _temp1212)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1215= Cyc_PP_text( _tag_arr("fill(", sizeof( unsigned char), 6u)); struct
Cyc_PP_Doc* _temp1216= Cyc_PP_nest( 2, Cyc_Absynpp_exp2doc( _temp1000)); struct
Cyc_PP_Doc* _temp1217= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u));
struct Cyc_PP_Doc* _temp1213[ 3u]={ _temp1215, _temp1216, _temp1217}; struct
_tagged_arr _temp1214={( void*) _temp1213,( void*) _temp1213,( void*)( _temp1213
+ 3u)}; _temp1212( _temp1214);}); goto _LL813; _LL813:;} if( inprec >= myprec){
s=({ struct Cyc_PP_Doc*(* _temp1218)( struct _tagged_arr)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp1221= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u));
struct Cyc_PP_Doc* _temp1222= s; struct Cyc_PP_Doc* _temp1223= Cyc_PP_text(
_tag_arr(")", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1219[ 3u]={
_temp1221, _temp1222, _temp1223}; struct _tagged_arr _temp1220={( void*)
_temp1219,( void*) _temp1219,( void*)( _temp1219 + 3u)}; _temp1218( _temp1220);});}
return s;} struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc( void* d){ void*
_temp1224= d; struct Cyc_Absyn_Exp* _temp1230; struct _tagged_arr* _temp1232;
_LL1226: if(*(( int*) _temp1224) == Cyc_Absyn_ArrayElement){ _LL1231: _temp1230=((
struct Cyc_Absyn_ArrayElement_struct*) _temp1224)->f1; goto _LL1227;} else{ goto
_LL1228;} _LL1228: if(*(( int*) _temp1224) == Cyc_Absyn_FieldName){ _LL1233:
_temp1232=(( struct Cyc_Absyn_FieldName_struct*) _temp1224)->f1; goto _LL1229;}
else{ goto _LL1225;} _LL1227: return({ struct Cyc_PP_Doc*(* _temp1234)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1237= Cyc_PP_text( _tag_arr(".[",
sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1238= Cyc_Absynpp_exp2doc(
_temp1230); struct Cyc_PP_Doc* _temp1239= Cyc_PP_text( _tag_arr("]", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1235[ 3u]={ _temp1237, _temp1238,
_temp1239}; struct _tagged_arr _temp1236={( void*) _temp1235,( void*) _temp1235,(
void*)( _temp1235 + 3u)}; _temp1234( _temp1236);}); _LL1229: return({ struct Cyc_PP_Doc*(*
_temp1240)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1243= Cyc_PP_text(
_tag_arr(".", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1244= Cyc_PP_textptr(
_temp1232); struct Cyc_PP_Doc* _temp1241[ 2u]={ _temp1243, _temp1244}; struct
_tagged_arr _temp1242={( void*) _temp1241,( void*) _temp1241,( void*)( _temp1241
+ 2u)}; _temp1240( _temp1242);}); _LL1225:;} struct Cyc_PP_Doc* Cyc_Absynpp_de2doc(
struct _tuple6* de){ if((* de).f1 == 0){ return Cyc_Absynpp_exp2doc((* de).f2);}
else{ return({ struct Cyc_PP_Doc*(* _temp1245)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1248= Cyc_PP_egroup( _tag_arr("", sizeof( unsigned char),
1u), _tag_arr("=", sizeof( unsigned char), 2u), _tag_arr("=", sizeof(
unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(* de).f1));
struct Cyc_PP_Doc* _temp1249= Cyc_Absynpp_exp2doc((* de).f2); struct Cyc_PP_Doc*
_temp1246[ 2u]={ _temp1248, _temp1249}; struct _tagged_arr _temp1247={( void*)
_temp1246,( void*) _temp1246,( void*)( _temp1246 + 2u)}; _temp1245( _temp1247);});}}
struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec( int inprec, struct Cyc_List_List*
es){ return Cyc_PP_group( _tag_arr("", sizeof( unsigned char), 1u), _tag_arr("",
sizeof( unsigned char), 1u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*), int env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec, inprec, es));}
struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc( void* c){ void* _temp1250= c;
unsigned char _temp1268; void* _temp1270; short _temp1272; void* _temp1274; int
_temp1276; void* _temp1278; int _temp1280; void* _temp1282; long long _temp1284;
void* _temp1286; struct _tagged_arr _temp1288; struct _tagged_arr _temp1290;
_LL1252: if(( unsigned int) _temp1250 > 1u?*(( int*) _temp1250) == Cyc_Absyn_Char_c:
0){ _LL1271: _temp1270=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp1250)->f1;
goto _LL1269; _LL1269: _temp1268=(( struct Cyc_Absyn_Char_c_struct*) _temp1250)->f2;
goto _LL1253;} else{ goto _LL1254;} _LL1254: if(( unsigned int) _temp1250 > 1u?*((
int*) _temp1250) == Cyc_Absyn_Short_c: 0){ _LL1275: _temp1274=( void*)(( struct
Cyc_Absyn_Short_c_struct*) _temp1250)->f1; goto _LL1273; _LL1273: _temp1272=((
struct Cyc_Absyn_Short_c_struct*) _temp1250)->f2; goto _LL1255;} else{ goto
_LL1256;} _LL1256: if(( unsigned int) _temp1250 > 1u?*(( int*) _temp1250) == Cyc_Absyn_Int_c:
0){ _LL1279: _temp1278=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1250)->f1;
if( _temp1278 ==( void*) Cyc_Absyn_Signed){ goto _LL1277;} else{ goto _LL1258;}
_LL1277: _temp1276=(( struct Cyc_Absyn_Int_c_struct*) _temp1250)->f2; goto
_LL1257;} else{ goto _LL1258;} _LL1258: if(( unsigned int) _temp1250 > 1u?*((
int*) _temp1250) == Cyc_Absyn_Int_c: 0){ _LL1283: _temp1282=( void*)(( struct
Cyc_Absyn_Int_c_struct*) _temp1250)->f1; if( _temp1282 ==( void*) Cyc_Absyn_Unsigned){
goto _LL1281;} else{ goto _LL1260;} _LL1281: _temp1280=(( struct Cyc_Absyn_Int_c_struct*)
_temp1250)->f2; goto _LL1259;} else{ goto _LL1260;} _LL1260: if(( unsigned int)
_temp1250 > 1u?*(( int*) _temp1250) == Cyc_Absyn_LongLong_c: 0){ _LL1287:
_temp1286=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp1250)->f1; goto
_LL1285; _LL1285: _temp1284=(( struct Cyc_Absyn_LongLong_c_struct*) _temp1250)->f2;
goto _LL1261;} else{ goto _LL1262;} _LL1262: if(( unsigned int) _temp1250 > 1u?*((
int*) _temp1250) == Cyc_Absyn_Float_c: 0){ _LL1289: _temp1288=(( struct Cyc_Absyn_Float_c_struct*)
_temp1250)->f1; goto _LL1263;} else{ goto _LL1264;} _LL1264: if( _temp1250 ==(
void*) Cyc_Absyn_Null_c){ goto _LL1265;} else{ goto _LL1266;} _LL1266: if((
unsigned int) _temp1250 > 1u?*(( int*) _temp1250) == Cyc_Absyn_String_c: 0){
_LL1291: _temp1290=(( struct Cyc_Absyn_String_c_struct*) _temp1250)->f1; goto
_LL1267;} else{ goto _LL1251;} _LL1253: return Cyc_PP_text(( struct _tagged_arr)({
struct _tagged_arr _temp1292= Cyc_Absynpp_char_escape( _temp1268); xprintf("'%.*s'",
_get_arr_size( _temp1292, 1u), _temp1292.curr);})); _LL1255: return Cyc_PP_text((
struct _tagged_arr) xprintf("%d",( int) _temp1272)); _LL1257: return Cyc_PP_text((
struct _tagged_arr) Cyc_Core_string_of_int( _temp1276)); _LL1259: return Cyc_PP_text((
struct _tagged_arr) xprintf("%u",( unsigned int) _temp1280)); _LL1261: return
Cyc_PP_text( _tag_arr("<<FIX LONG LONG CONSTANT>>", sizeof( unsigned char), 27u));
_LL1263: return Cyc_PP_text( _temp1288); _LL1265: return Cyc_PP_text( _tag_arr("null",
sizeof( unsigned char), 5u)); _LL1267: return({ struct Cyc_PP_Doc*(* _temp1293)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1296= Cyc_PP_text(
_tag_arr("\"", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1297= Cyc_PP_text(
Cyc_Absynpp_string_escape( _temp1290)); struct Cyc_PP_Doc* _temp1298= Cyc_PP_text(
_tag_arr("\"", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1294[ 3u]={
_temp1296, _temp1297, _temp1298}; struct _tagged_arr _temp1295={( void*)
_temp1294,( void*) _temp1294,( void*)( _temp1294 + 3u)}; _temp1293( _temp1295);});
_LL1251:;} struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc( int inprec, void* p,
struct Cyc_List_List* es){ struct Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc( p); if(
p ==( void*) Cyc_Absyn_Size){ if( es == 0? 1:(( struct Cyc_List_List*)
_check_null( es))->tl != 0){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp1299=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1299[ 0]=({ struct Cyc_Core_Failure_struct _temp1300; _temp1300.tag= Cyc_Core_Failure;
_temp1300.f1=( struct _tagged_arr)({ struct _tagged_arr _temp1301= Cyc_PP_string_of_doc(
ps, 72); xprintf("Absynpp::primapp2doc Size: %.*s with bad args", _get_arr_size(
_temp1301, 1u), _temp1301.curr);}); _temp1300;}); _temp1299;}));}{ struct Cyc_PP_Doc*
_temp1302= Cyc_Absynpp_exp2doc_prec( inprec,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd); return({ struct Cyc_PP_Doc*(* _temp1303)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1306= _temp1302; struct
Cyc_PP_Doc* _temp1307= Cyc_PP_text( _tag_arr(".size", sizeof( unsigned char), 6u));
struct Cyc_PP_Doc* _temp1304[ 2u]={ _temp1306, _temp1307}; struct _tagged_arr
_temp1305={( void*) _temp1304,( void*) _temp1304,( void*)( _temp1304 + 2u)};
_temp1303( _temp1305);});}} else{ if( ! Cyc_Absyn_is_format_prim( p)){ struct
Cyc_List_List* ds=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int,
struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec,
inprec, es); if( ds == 0){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp1308=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1308[ 0]=({ struct Cyc_Core_Failure_struct _temp1309; _temp1309.tag= Cyc_Core_Failure;
_temp1309.f1=( struct _tagged_arr)({ struct _tagged_arr _temp1310= Cyc_PP_string_of_doc(
ps, 72); xprintf("Absynpp::primapp2doc: %.*s with no args", _get_arr_size(
_temp1310, 1u), _temp1310.curr);}); _temp1309;}); _temp1308;}));} else{ if(((
struct Cyc_List_List*) _check_null( ds))->tl == 0){ return({ struct Cyc_PP_Doc*(*
_temp1311)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1314= ps;
struct Cyc_PP_Doc* _temp1315= Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp1316=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*)
_check_null( ds))->hd; struct Cyc_PP_Doc* _temp1312[ 3u]={ _temp1314, _temp1315,
_temp1316}; struct _tagged_arr _temp1313={( void*) _temp1312,( void*) _temp1312,(
void*)( _temp1312 + 3u)}; _temp1311( _temp1313);});} else{ if((( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( ds))->tl))->tl != 0){( int)
_throw(( void*)({ struct Cyc_Core_Failure_struct* _temp1317=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1317[ 0]=({ struct Cyc_Core_Failure_struct
_temp1318; _temp1318.tag= Cyc_Core_Failure; _temp1318.f1=( struct _tagged_arr)({
struct _tagged_arr _temp1319= Cyc_PP_string_of_doc( ps, 72); xprintf("Absynpp::primapp2doc: %.*s with more than 2 args",
_get_arr_size( _temp1319, 1u), _temp1319.curr);}); _temp1318;}); _temp1317;}));}
else{ return({ struct Cyc_PP_Doc*(* _temp1320)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1323=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*)
_check_null( ds))->hd; struct Cyc_PP_Doc* _temp1324= Cyc_PP_text( _tag_arr(" ",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1325= ps; struct Cyc_PP_Doc*
_temp1326= Cyc_PP_text( _tag_arr(" ", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1327=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null((( struct
Cyc_List_List*) _check_null( ds))->tl))->hd; struct Cyc_PP_Doc* _temp1321[ 5u]={
_temp1323, _temp1324, _temp1325, _temp1326, _temp1327}; struct _tagged_arr
_temp1322={( void*) _temp1321,( void*) _temp1321,( void*)( _temp1321 + 5u)};
_temp1320( _temp1322);});}}}} else{ return({ struct Cyc_PP_Doc*(* _temp1328)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1331= ps; struct Cyc_PP_Doc*
_temp1332= Cyc_PP_text( _tag_arr("(", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1333= Cyc_Absynpp_exps2doc_prec( 20, es); struct Cyc_PP_Doc* _temp1334= Cyc_PP_text(
_tag_arr(")", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1329[ 4u]={
_temp1331, _temp1332, _temp1333, _temp1334}; struct _tagged_arr _temp1330={(
void*) _temp1329,( void*) _temp1329,( void*)( _temp1329 + 4u)}; _temp1328(
_temp1330);});}}} struct _tagged_arr Cyc_Absynpp_prim2str( void* p){ void*
_temp1335= p; _LL1337: if( _temp1335 ==( void*) Cyc_Absyn_Plus){ goto _LL1338;}
else{ goto _LL1339;} _LL1339: if( _temp1335 ==( void*) Cyc_Absyn_Times){ goto
_LL1340;} else{ goto _LL1341;} _LL1341: if( _temp1335 ==( void*) Cyc_Absyn_Minus){
goto _LL1342;} else{ goto _LL1343;} _LL1343: if( _temp1335 ==( void*) Cyc_Absyn_Div){
goto _LL1344;} else{ goto _LL1345;} _LL1345: if( _temp1335 ==( void*) Cyc_Absyn_Mod){
goto _LL1346;} else{ goto _LL1347;} _LL1347: if( _temp1335 ==( void*) Cyc_Absyn_Eq){
goto _LL1348;} else{ goto _LL1349;} _LL1349: if( _temp1335 ==( void*) Cyc_Absyn_Neq){
goto _LL1350;} else{ goto _LL1351;} _LL1351: if( _temp1335 ==( void*) Cyc_Absyn_Gt){
goto _LL1352;} else{ goto _LL1353;} _LL1353: if( _temp1335 ==( void*) Cyc_Absyn_Lt){
goto _LL1354;} else{ goto _LL1355;} _LL1355: if( _temp1335 ==( void*) Cyc_Absyn_Gte){
goto _LL1356;} else{ goto _LL1357;} _LL1357: if( _temp1335 ==( void*) Cyc_Absyn_Lte){
goto _LL1358;} else{ goto _LL1359;} _LL1359: if( _temp1335 ==( void*) Cyc_Absyn_Not){
goto _LL1360;} else{ goto _LL1361;} _LL1361: if( _temp1335 ==( void*) Cyc_Absyn_Bitnot){
goto _LL1362;} else{ goto _LL1363;} _LL1363: if( _temp1335 ==( void*) Cyc_Absyn_Bitand){
goto _LL1364;} else{ goto _LL1365;} _LL1365: if( _temp1335 ==( void*) Cyc_Absyn_Bitor){
goto _LL1366;} else{ goto _LL1367;} _LL1367: if( _temp1335 ==( void*) Cyc_Absyn_Bitxor){
goto _LL1368;} else{ goto _LL1369;} _LL1369: if( _temp1335 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL1370;} else{ goto _LL1371;} _LL1371: if( _temp1335 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL1372;} else{ goto _LL1373;} _LL1373: if( _temp1335 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL1374;} else{ goto _LL1375;} _LL1375: if( _temp1335 ==( void*) Cyc_Absyn_Size){
goto _LL1376;} else{ goto _LL1377;} _LL1377: if( _temp1335 ==( void*) Cyc_Absyn_Printf){
goto _LL1378;} else{ goto _LL1379;} _LL1379: if( _temp1335 ==( void*) Cyc_Absyn_Fprintf){
goto _LL1380;} else{ goto _LL1381;} _LL1381: if( _temp1335 ==( void*) Cyc_Absyn_Xprintf){
goto _LL1382;} else{ goto _LL1383;} _LL1383: if( _temp1335 ==( void*) Cyc_Absyn_Scanf){
goto _LL1384;} else{ goto _LL1385;} _LL1385: if( _temp1335 ==( void*) Cyc_Absyn_Fscanf){
goto _LL1386;} else{ goto _LL1387;} _LL1387: if( _temp1335 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1388;} else{ goto _LL1336;} _LL1338: return _tag_arr("+", sizeof(
unsigned char), 2u); _LL1340: return _tag_arr("*", sizeof( unsigned char), 2u);
_LL1342: return _tag_arr("-", sizeof( unsigned char), 2u); _LL1344: return
_tag_arr("/", sizeof( unsigned char), 2u); _LL1346: return _tag_arr("%", sizeof(
unsigned char), 2u); _LL1348: return _tag_arr("==", sizeof( unsigned char), 3u);
_LL1350: return _tag_arr("!=", sizeof( unsigned char), 3u); _LL1352: return
_tag_arr(">", sizeof( unsigned char), 2u); _LL1354: return _tag_arr("<", sizeof(
unsigned char), 2u); _LL1356: return _tag_arr(">=", sizeof( unsigned char), 3u);
_LL1358: return _tag_arr("<=", sizeof( unsigned char), 3u); _LL1360: return
_tag_arr("!", sizeof( unsigned char), 2u); _LL1362: return _tag_arr("~", sizeof(
unsigned char), 2u); _LL1364: return _tag_arr("&", sizeof( unsigned char), 2u);
_LL1366: return _tag_arr("|", sizeof( unsigned char), 2u); _LL1368: return
_tag_arr("^", sizeof( unsigned char), 2u); _LL1370: return _tag_arr("<<",
sizeof( unsigned char), 3u); _LL1372: return _tag_arr(">>", sizeof(
unsigned char), 3u); _LL1374: return _tag_arr(">>>", sizeof( unsigned char), 4u);
_LL1376: return _tag_arr("size", sizeof( unsigned char), 5u); _LL1378: return
_tag_arr("printf", sizeof( unsigned char), 7u); _LL1380: return _tag_arr("fprintf",
sizeof( unsigned char), 8u); _LL1382: return _tag_arr("xprintf", sizeof(
unsigned char), 8u); _LL1384: return _tag_arr("scanf", sizeof( unsigned char), 6u);
_LL1386: return _tag_arr("fscanf", sizeof( unsigned char), 7u); _LL1388: return
_tag_arr("sscanf", sizeof( unsigned char), 7u); _LL1336:;} struct Cyc_PP_Doc*
Cyc_Absynpp_prim2doc( void* p){ return Cyc_PP_text( Cyc_Absynpp_prim2str( p));}
struct _tuple9{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ; struct
Cyc_PP_Doc* Cyc_Absynpp_structarg2doc( struct _tuple9* t){ if((* t).f1 == 0){
return Cyc_Absynpp_exp2doc((* t).f2);} else{ return({ struct Cyc_PP_Doc*(*
_temp1389)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1392= Cyc_PP_textptr((
struct _tagged_arr*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v); struct
Cyc_PP_Doc* _temp1393= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u));
struct Cyc_PP_Doc* _temp1394= Cyc_Absynpp_exp2doc((* t).f2); struct Cyc_PP_Doc*
_temp1390[ 3u]={ _temp1392, _temp1393, _temp1394}; struct _tagged_arr _temp1391={(
void*) _temp1390,( void*) _temp1390,( void*)( _temp1390 + 3u)}; _temp1389(
_temp1391);});}} int Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s){ void*
_temp1395=( void*) s->r; _LL1397: if(( unsigned int) _temp1395 > 1u?*(( int*)
_temp1395) == Cyc_Absyn_Decl_s: 0){ goto _LL1398;} else{ goto _LL1399;} _LL1399:
goto _LL1400; _LL1398: return 1; _LL1400: return 0; _LL1396:;} struct Cyc_PP_Doc*
Cyc_Absynpp_stmt2doc( struct Cyc_Absyn_Stmt* st){ struct Cyc_PP_Doc* s;{ void*
_temp1401=( void*) st->r; struct Cyc_Absyn_Exp* _temp1445; struct Cyc_Absyn_Stmt*
_temp1447; struct Cyc_Absyn_Stmt* _temp1449; struct Cyc_Absyn_Exp* _temp1451;
struct Cyc_Absyn_Stmt* _temp1453; struct Cyc_Absyn_Stmt* _temp1455; struct Cyc_Absyn_Exp*
_temp1457; struct Cyc_Absyn_Stmt* _temp1459; struct _tuple2 _temp1461; struct
Cyc_Absyn_Exp* _temp1463; struct _tagged_arr* _temp1465; struct Cyc_Absyn_Stmt*
_temp1467; struct _tuple2 _temp1469; struct Cyc_Absyn_Exp* _temp1471; struct
_tuple2 _temp1473; struct Cyc_Absyn_Exp* _temp1475; struct Cyc_Absyn_Exp*
_temp1477; struct Cyc_List_List* _temp1479; struct Cyc_Absyn_Exp* _temp1481;
struct Cyc_List_List* _temp1483; struct Cyc_Absyn_Exp* _temp1485; struct Cyc_List_List*
_temp1487; struct Cyc_List_List* _temp1489; struct Cyc_Absyn_Stmt* _temp1491;
struct Cyc_Absyn_Decl* _temp1493; struct Cyc_Absyn_Stmt* _temp1495; struct Cyc_Absyn_Stmt*
_temp1497; struct Cyc_Absyn_Stmt* _temp1499; struct _tagged_arr* _temp1501;
struct _tuple2 _temp1503; struct Cyc_Absyn_Exp* _temp1505; struct Cyc_Absyn_Stmt*
_temp1507; struct Cyc_List_List* _temp1509; struct Cyc_Absyn_Stmt* _temp1511;
struct Cyc_Absyn_Stmt* _temp1513; struct Cyc_Absyn_Vardecl* _temp1515; struct
Cyc_Absyn_Tvar* _temp1517; _LL1403: if( _temp1401 ==( void*) Cyc_Absyn_Skip_s){
goto _LL1404;} else{ goto _LL1405;} _LL1405: if(( unsigned int) _temp1401 > 1u?*((
int*) _temp1401) == Cyc_Absyn_Exp_s: 0){ _LL1446: _temp1445=(( struct Cyc_Absyn_Exp_s_struct*)
_temp1401)->f1; goto _LL1406;} else{ goto _LL1407;} _LL1407: if(( unsigned int)
_temp1401 > 1u?*(( int*) _temp1401) == Cyc_Absyn_Seq_s: 0){ _LL1450: _temp1449=((
struct Cyc_Absyn_Seq_s_struct*) _temp1401)->f1; goto _LL1448; _LL1448: _temp1447=((
struct Cyc_Absyn_Seq_s_struct*) _temp1401)->f2; goto _LL1408;} else{ goto
_LL1409;} _LL1409: if(( unsigned int) _temp1401 > 1u?*(( int*) _temp1401) == Cyc_Absyn_Return_s:
0){ _LL1452: _temp1451=(( struct Cyc_Absyn_Return_s_struct*) _temp1401)->f1;
goto _LL1410;} else{ goto _LL1411;} _LL1411: if(( unsigned int) _temp1401 > 1u?*((
int*) _temp1401) == Cyc_Absyn_IfThenElse_s: 0){ _LL1458: _temp1457=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1401)->f1; goto _LL1456; _LL1456: _temp1455=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1401)->f2; goto _LL1454; _LL1454: _temp1453=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1401)->f3; goto _LL1412;} else{ goto _LL1413;} _LL1413: if(( unsigned int)
_temp1401 > 1u?*(( int*) _temp1401) == Cyc_Absyn_While_s: 0){ _LL1462: _temp1461=((
struct Cyc_Absyn_While_s_struct*) _temp1401)->f1; _LL1464: _temp1463= _temp1461.f1;
goto _LL1460; _LL1460: _temp1459=(( struct Cyc_Absyn_While_s_struct*) _temp1401)->f2;
goto _LL1414;} else{ goto _LL1415;} _LL1415: if(( unsigned int) _temp1401 > 1u?*((
int*) _temp1401) == Cyc_Absyn_Break_s: 0){ goto _LL1416;} else{ goto _LL1417;}
_LL1417: if(( unsigned int) _temp1401 > 1u?*(( int*) _temp1401) == Cyc_Absyn_Continue_s:
0){ goto _LL1418;} else{ goto _LL1419;} _LL1419: if(( unsigned int) _temp1401 >
1u?*(( int*) _temp1401) == Cyc_Absyn_Goto_s: 0){ _LL1466: _temp1465=(( struct
Cyc_Absyn_Goto_s_struct*) _temp1401)->f1; goto _LL1420;} else{ goto _LL1421;}
_LL1421: if(( unsigned int) _temp1401 > 1u?*(( int*) _temp1401) == Cyc_Absyn_For_s:
0){ _LL1478: _temp1477=(( struct Cyc_Absyn_For_s_struct*) _temp1401)->f1; goto
_LL1474; _LL1474: _temp1473=(( struct Cyc_Absyn_For_s_struct*) _temp1401)->f2;
_LL1476: _temp1475= _temp1473.f1; goto _LL1470; _LL1470: _temp1469=(( struct Cyc_Absyn_For_s_struct*)
_temp1401)->f3; _LL1472: _temp1471= _temp1469.f1; goto _LL1468; _LL1468:
_temp1467=(( struct Cyc_Absyn_For_s_struct*) _temp1401)->f4; goto _LL1422;}
else{ goto _LL1423;} _LL1423: if(( unsigned int) _temp1401 > 1u?*(( int*)
_temp1401) == Cyc_Absyn_Switch_s: 0){ _LL1482: _temp1481=(( struct Cyc_Absyn_Switch_s_struct*)
_temp1401)->f1; goto _LL1480; _LL1480: _temp1479=(( struct Cyc_Absyn_Switch_s_struct*)
_temp1401)->f2; goto _LL1424;} else{ goto _LL1425;} _LL1425: if(( unsigned int)
_temp1401 > 1u?*(( int*) _temp1401) == Cyc_Absyn_SwitchC_s: 0){ _LL1486:
_temp1485=(( struct Cyc_Absyn_SwitchC_s_struct*) _temp1401)->f1; goto _LL1484;
_LL1484: _temp1483=(( struct Cyc_Absyn_SwitchC_s_struct*) _temp1401)->f2; goto
_LL1426;} else{ goto _LL1427;} _LL1427: if(( unsigned int) _temp1401 > 1u?*((
int*) _temp1401) == Cyc_Absyn_Fallthru_s: 0){ _LL1488: _temp1487=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1401)->f1; if( _temp1487 == 0){ goto _LL1428;} else{ goto _LL1429;}} else{
goto _LL1429;} _LL1429: if(( unsigned int) _temp1401 > 1u?*(( int*) _temp1401)
== Cyc_Absyn_Fallthru_s: 0){ _LL1490: _temp1489=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1401)->f1; goto _LL1430;} else{ goto _LL1431;} _LL1431: if(( unsigned int)
_temp1401 > 1u?*(( int*) _temp1401) == Cyc_Absyn_Decl_s: 0){ _LL1494: _temp1493=((
struct Cyc_Absyn_Decl_s_struct*) _temp1401)->f1; goto _LL1492; _LL1492:
_temp1491=(( struct Cyc_Absyn_Decl_s_struct*) _temp1401)->f2; goto _LL1432;}
else{ goto _LL1433;} _LL1433: if(( unsigned int) _temp1401 > 1u?*(( int*)
_temp1401) == Cyc_Absyn_Cut_s: 0){ _LL1496: _temp1495=(( struct Cyc_Absyn_Cut_s_struct*)
_temp1401)->f1; goto _LL1434;} else{ goto _LL1435;} _LL1435: if(( unsigned int)
_temp1401 > 1u?*(( int*) _temp1401) == Cyc_Absyn_Splice_s: 0){ _LL1498:
_temp1497=(( struct Cyc_Absyn_Splice_s_struct*) _temp1401)->f1; goto _LL1436;}
else{ goto _LL1437;} _LL1437: if(( unsigned int) _temp1401 > 1u?*(( int*)
_temp1401) == Cyc_Absyn_Label_s: 0){ _LL1502: _temp1501=(( struct Cyc_Absyn_Label_s_struct*)
_temp1401)->f1; goto _LL1500; _LL1500: _temp1499=(( struct Cyc_Absyn_Label_s_struct*)
_temp1401)->f2; goto _LL1438;} else{ goto _LL1439;} _LL1439: if(( unsigned int)
_temp1401 > 1u?*(( int*) _temp1401) == Cyc_Absyn_Do_s: 0){ _LL1508: _temp1507=((
struct Cyc_Absyn_Do_s_struct*) _temp1401)->f1; goto _LL1504; _LL1504: _temp1503=((
struct Cyc_Absyn_Do_s_struct*) _temp1401)->f2; _LL1506: _temp1505= _temp1503.f1;
goto _LL1440;} else{ goto _LL1441;} _LL1441: if(( unsigned int) _temp1401 > 1u?*((
int*) _temp1401) == Cyc_Absyn_TryCatch_s: 0){ _LL1512: _temp1511=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1401)->f1; goto _LL1510; _LL1510: _temp1509=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1401)->f2; goto _LL1442;} else{ goto _LL1443;} _LL1443: if(( unsigned int)
_temp1401 > 1u?*(( int*) _temp1401) == Cyc_Absyn_Region_s: 0){ _LL1518:
_temp1517=(( struct Cyc_Absyn_Region_s_struct*) _temp1401)->f1; goto _LL1516;
_LL1516: _temp1515=(( struct Cyc_Absyn_Region_s_struct*) _temp1401)->f2; goto
_LL1514; _LL1514: _temp1513=(( struct Cyc_Absyn_Region_s_struct*) _temp1401)->f3;
goto _LL1444;} else{ goto _LL1402;} _LL1404: s= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); goto _LL1402; _LL1406: s=({ struct Cyc_PP_Doc*(*
_temp1519)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1522= Cyc_Absynpp_exp2doc(
_temp1445); struct Cyc_PP_Doc* _temp1523= Cyc_PP_text( _tag_arr(";", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1520[ 2u]={ _temp1522, _temp1523};
struct _tagged_arr _temp1521={( void*) _temp1520,( void*) _temp1520,( void*)(
_temp1520 + 2u)}; _temp1519( _temp1521);}); goto _LL1402; _LL1408: if( Cyc_Absynpp_decls_first){
if( Cyc_Absynpp_is_declaration( _temp1449)){ s=({ struct Cyc_PP_Doc*(* _temp1524)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1527= Cyc_PP_text(
_tag_arr("{ ", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1528= Cyc_PP_nest(
2, Cyc_Absynpp_stmt2doc( _temp1449)); struct Cyc_PP_Doc* _temp1529= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1530= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp1531= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp1532= Cyc_Absynpp_is_declaration( _temp1447)?({ struct Cyc_PP_Doc*(*
_temp1533)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1536= Cyc_PP_text(
_tag_arr("{ ", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1537= Cyc_PP_nest(
2, Cyc_Absynpp_stmt2doc( _temp1447)); struct Cyc_PP_Doc* _temp1538= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1539= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1534[ 4u]={ _temp1536, _temp1537, _temp1538, _temp1539};
struct _tagged_arr _temp1535={( void*) _temp1534,( void*) _temp1534,( void*)(
_temp1534 + 4u)}; _temp1533( _temp1535);}): Cyc_Absynpp_stmt2doc( _temp1447);
struct Cyc_PP_Doc* _temp1525[ 6u]={ _temp1527, _temp1528, _temp1529, _temp1530,
_temp1531, _temp1532}; struct _tagged_arr _temp1526={( void*) _temp1525,( void*)
_temp1525,( void*)( _temp1525 + 6u)}; _temp1524( _temp1526);});} else{ if( Cyc_Absynpp_is_declaration(
_temp1447)){ s=({ struct Cyc_PP_Doc*(* _temp1540)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1543= Cyc_Absynpp_stmt2doc( _temp1449); struct Cyc_PP_Doc*
_temp1544= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1545= Cyc_PP_text(
_tag_arr("{ ", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1546= Cyc_PP_nest(
2, Cyc_Absynpp_stmt2doc( _temp1447)); struct Cyc_PP_Doc* _temp1547= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1548= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1541[ 6u]={ _temp1543, _temp1544, _temp1545, _temp1546,
_temp1547, _temp1548}; struct _tagged_arr _temp1542={( void*) _temp1541,( void*)
_temp1541,( void*)( _temp1541 + 6u)}; _temp1540( _temp1542);});} else{ s=((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct
_tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,
_tag_arr("", sizeof( unsigned char), 1u),({ struct Cyc_List_List* _temp1549=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1549->hd=(
void*) _temp1449; _temp1549->tl=({ struct Cyc_List_List* _temp1550=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1550->hd=( void*) _temp1447;
_temp1550->tl= 0; _temp1550;}); _temp1549;}));}}} else{ s=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*), struct _tagged_arr sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc, _tag_arr("",
sizeof( unsigned char), 1u),({ struct Cyc_List_List* _temp1551=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1551->hd=( void*) _temp1449;
_temp1551->tl=({ struct Cyc_List_List* _temp1552=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1552->hd=( void*) _temp1447;
_temp1552->tl= 0; _temp1552;}); _temp1551;}));} goto _LL1402; _LL1410: if(
_temp1451 == 0){ s= Cyc_PP_text( _tag_arr("return;", sizeof( unsigned char), 8u));}
else{ s=({ struct Cyc_PP_Doc*(* _temp1553)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1556= Cyc_PP_text( _tag_arr("return ", sizeof(
unsigned char), 8u)); struct Cyc_PP_Doc* _temp1557= _temp1451 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp1451)); struct
Cyc_PP_Doc* _temp1558= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u));
struct Cyc_PP_Doc* _temp1554[ 3u]={ _temp1556, _temp1557, _temp1558}; struct
_tagged_arr _temp1555={( void*) _temp1554,( void*) _temp1554,( void*)( _temp1554
+ 3u)}; _temp1553( _temp1555);});} goto _LL1402; _LL1412: { int print_else;{
void* _temp1559=( void*) _temp1453->r; _LL1561: if( _temp1559 ==( void*) Cyc_Absyn_Skip_s){
goto _LL1562;} else{ goto _LL1563;} _LL1563: goto _LL1564; _LL1562: print_else=
0; goto _LL1560; _LL1564: print_else= 1; goto _LL1560; _LL1560:;} s=({ struct
Cyc_PP_Doc*(* _temp1565)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1568= Cyc_PP_text( _tag_arr("if (", sizeof( unsigned char), 5u)); struct
Cyc_PP_Doc* _temp1569= Cyc_Absynpp_exp2doc( _temp1457); struct Cyc_PP_Doc*
_temp1570= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc*
_temp1571= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp1588)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1591= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp1592= Cyc_Absynpp_stmt2doc( _temp1455); struct Cyc_PP_Doc* _temp1589[ 2u]={
_temp1591, _temp1592}; struct _tagged_arr _temp1590={( void*) _temp1589,( void*)
_temp1589,( void*)( _temp1589 + 2u)}; _temp1588( _temp1590);})); struct Cyc_PP_Doc*
_temp1572= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1573= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1574=
print_else?({ struct Cyc_PP_Doc*(* _temp1575)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1578= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1579=
Cyc_PP_text( _tag_arr("else {", sizeof( unsigned char), 7u)); struct Cyc_PP_Doc*
_temp1580= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp1583)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1586= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp1587= Cyc_Absynpp_stmt2doc( _temp1453); struct Cyc_PP_Doc* _temp1584[ 2u]={
_temp1586, _temp1587}; struct _tagged_arr _temp1585={( void*) _temp1584,( void*)
_temp1584,( void*)( _temp1584 + 2u)}; _temp1583( _temp1585);})); struct Cyc_PP_Doc*
_temp1581= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1582= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1576[ 5u]={
_temp1578, _temp1579, _temp1580, _temp1581, _temp1582}; struct _tagged_arr
_temp1577={( void*) _temp1576,( void*) _temp1576,( void*)( _temp1576 + 5u)};
_temp1575( _temp1577);}): Cyc_PP_nil_doc(); struct Cyc_PP_Doc* _temp1566[ 7u]={
_temp1568, _temp1569, _temp1570, _temp1571, _temp1572, _temp1573, _temp1574};
struct _tagged_arr _temp1567={( void*) _temp1566,( void*) _temp1566,( void*)(
_temp1566 + 7u)}; _temp1565( _temp1567);}); goto _LL1402;} _LL1414: s=({ struct
Cyc_PP_Doc*(* _temp1593)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1596= Cyc_PP_text( _tag_arr("while (", sizeof( unsigned char), 8u)); struct
Cyc_PP_Doc* _temp1597= Cyc_Absynpp_exp2doc( _temp1463); struct Cyc_PP_Doc*
_temp1598= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc*
_temp1599= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp1602)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1605= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp1606= Cyc_Absynpp_stmt2doc( _temp1459); struct Cyc_PP_Doc* _temp1603[ 2u]={
_temp1605, _temp1606}; struct _tagged_arr _temp1604={( void*) _temp1603,( void*)
_temp1603,( void*)( _temp1603 + 2u)}; _temp1602( _temp1604);})); struct Cyc_PP_Doc*
_temp1600= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1601= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1594[ 6u]={
_temp1596, _temp1597, _temp1598, _temp1599, _temp1600, _temp1601}; struct
_tagged_arr _temp1595={( void*) _temp1594,( void*) _temp1594,( void*)( _temp1594
+ 6u)}; _temp1593( _temp1595);}); goto _LL1402; _LL1416: s= Cyc_PP_text(
_tag_arr("break;", sizeof( unsigned char), 7u)); goto _LL1402; _LL1418: s= Cyc_PP_text(
_tag_arr("continue;", sizeof( unsigned char), 10u)); goto _LL1402; _LL1420: s=
Cyc_PP_text(( struct _tagged_arr)({ struct _tagged_arr _temp1607=* _temp1465;
xprintf("goto %.*s;", _get_arr_size( _temp1607, 1u), _temp1607.curr);})); goto
_LL1402; _LL1422: s=({ struct Cyc_PP_Doc*(* _temp1608)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1611= Cyc_PP_text( _tag_arr("for(", sizeof(
unsigned char), 5u)); struct Cyc_PP_Doc* _temp1612= Cyc_Absynpp_exp2doc(
_temp1477); struct Cyc_PP_Doc* _temp1613= Cyc_PP_text( _tag_arr("; ", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp1614= Cyc_Absynpp_exp2doc(
_temp1475); struct Cyc_PP_Doc* _temp1615= Cyc_PP_text( _tag_arr("; ", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp1616= Cyc_Absynpp_exp2doc(
_temp1471); struct Cyc_PP_Doc* _temp1617= Cyc_PP_text( _tag_arr(") {", sizeof(
unsigned char), 4u)); struct Cyc_PP_Doc* _temp1618= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(*
_temp1621)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1624= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1625= Cyc_Absynpp_stmt2doc( _temp1467); struct Cyc_PP_Doc*
_temp1622[ 2u]={ _temp1624, _temp1625}; struct _tagged_arr _temp1623={( void*)
_temp1622,( void*) _temp1622,( void*)( _temp1622 + 2u)}; _temp1621( _temp1623);}));
struct Cyc_PP_Doc* _temp1619= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1620=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1609[ 10u]={ _temp1611, _temp1612, _temp1613, _temp1614, _temp1615,
_temp1616, _temp1617, _temp1618, _temp1619, _temp1620}; struct _tagged_arr
_temp1610={( void*) _temp1609,( void*) _temp1609,( void*)( _temp1609 + 10u)};
_temp1608( _temp1610);}); goto _LL1402; _LL1424: s=({ struct Cyc_PP_Doc*(*
_temp1626)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1629= Cyc_PP_text(
_tag_arr("switch (", sizeof( unsigned char), 9u)); struct Cyc_PP_Doc* _temp1630=
Cyc_Absynpp_exp2doc( _temp1481); struct Cyc_PP_Doc* _temp1631= Cyc_PP_text(
_tag_arr(") {", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc* _temp1632= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1633= Cyc_Absynpp_switchclauses2doc( _temp1479); struct
Cyc_PP_Doc* _temp1634= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1635= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1627[ 7u]={
_temp1629, _temp1630, _temp1631, _temp1632, _temp1633, _temp1634, _temp1635};
struct _tagged_arr _temp1628={( void*) _temp1627,( void*) _temp1627,( void*)(
_temp1627 + 7u)}; _temp1626( _temp1628);}); goto _LL1402; _LL1426: s=({ struct
Cyc_PP_Doc*(* _temp1636)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1639= Cyc_PP_text( _tag_arr("switch \"C\" (", sizeof( unsigned char), 13u));
struct Cyc_PP_Doc* _temp1640= Cyc_Absynpp_exp2doc( _temp1485); struct Cyc_PP_Doc*
_temp1641= Cyc_PP_text( _tag_arr(") {", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc*
_temp1642= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1643= Cyc_Absynpp_switchCclauses2doc(
_temp1483); struct Cyc_PP_Doc* _temp1644= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp1645= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1637[ 7u]={ _temp1639, _temp1640, _temp1641, _temp1642, _temp1643,
_temp1644, _temp1645}; struct _tagged_arr _temp1638={( void*) _temp1637,( void*)
_temp1637,( void*)( _temp1637 + 7u)}; _temp1636( _temp1638);}); goto _LL1402;
_LL1428: s= Cyc_PP_text( _tag_arr("fallthru;", sizeof( unsigned char), 10u));
goto _LL1402; _LL1430: s=({ struct Cyc_PP_Doc*(* _temp1646)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1649= Cyc_PP_text( _tag_arr("fallthru(",
sizeof( unsigned char), 10u)); struct Cyc_PP_Doc* _temp1650= Cyc_Absynpp_exps2doc_prec(
20, _temp1489); struct Cyc_PP_Doc* _temp1651= Cyc_PP_text( _tag_arr(");",
sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1647[ 3u]={ _temp1649,
_temp1650, _temp1651}; struct _tagged_arr _temp1648={( void*) _temp1647,( void*)
_temp1647,( void*)( _temp1647 + 3u)}; _temp1646( _temp1648);}); goto _LL1402;
_LL1432: s=({ struct Cyc_PP_Doc*(* _temp1652)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1655= Cyc_Absynpp_decl2doc( _temp1493); struct Cyc_PP_Doc*
_temp1656= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1657= Cyc_Absynpp_stmt2doc(
_temp1491); struct Cyc_PP_Doc* _temp1653[ 3u]={ _temp1655, _temp1656, _temp1657};
struct _tagged_arr _temp1654={( void*) _temp1653,( void*) _temp1653,( void*)(
_temp1653 + 3u)}; _temp1652( _temp1654);}); goto _LL1402; _LL1434: s=({ struct
Cyc_PP_Doc*(* _temp1658)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1661= Cyc_PP_text( _tag_arr("cut ", sizeof( unsigned char), 5u)); struct
Cyc_PP_Doc* _temp1662= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1495)); struct
Cyc_PP_Doc* _temp1659[ 2u]={ _temp1661, _temp1662}; struct _tagged_arr _temp1660={(
void*) _temp1659,( void*) _temp1659,( void*)( _temp1659 + 2u)}; _temp1658(
_temp1660);}); goto _LL1402; _LL1436: s=({ struct Cyc_PP_Doc*(* _temp1663)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1666= Cyc_PP_text(
_tag_arr("splice ", sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp1667=
Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1497)); struct Cyc_PP_Doc* _temp1664[
2u]={ _temp1666, _temp1667}; struct _tagged_arr _temp1665={( void*) _temp1664,(
void*) _temp1664,( void*)( _temp1664 + 2u)}; _temp1663( _temp1665);}); goto
_LL1402; _LL1438: if( Cyc_Absynpp_decls_first? Cyc_Absynpp_is_declaration(
_temp1499): 0){ s=({ struct Cyc_PP_Doc*(* _temp1668)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1671= Cyc_PP_textptr( _temp1501); struct Cyc_PP_Doc*
_temp1672= Cyc_PP_text( _tag_arr(": {", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc*
_temp1673= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1674= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1499)); struct Cyc_PP_Doc* _temp1675= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp1676= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1669[ 6u]={ _temp1671, _temp1672, _temp1673, _temp1674, _temp1675,
_temp1676}; struct _tagged_arr _temp1670={( void*) _temp1669,( void*) _temp1669,(
void*)( _temp1669 + 6u)}; _temp1668( _temp1670);});} else{ s=({ struct Cyc_PP_Doc*(*
_temp1677)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1680= Cyc_PP_textptr(
_temp1501); struct Cyc_PP_Doc* _temp1681= Cyc_PP_text( _tag_arr(": ", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp1682= Cyc_Absynpp_stmt2doc(
_temp1499); struct Cyc_PP_Doc* _temp1678[ 3u]={ _temp1680, _temp1681, _temp1682};
struct _tagged_arr _temp1679={( void*) _temp1678,( void*) _temp1678,( void*)(
_temp1678 + 3u)}; _temp1677( _temp1679);});} goto _LL1402; _LL1440: s=({ struct
Cyc_PP_Doc*(* _temp1683)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1686= Cyc_PP_text( _tag_arr("do {", sizeof( unsigned char), 5u)); struct
Cyc_PP_Doc* _temp1687= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1688= Cyc_PP_nest(
2, Cyc_Absynpp_stmt2doc( _temp1507)); struct Cyc_PP_Doc* _temp1689= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1690= Cyc_PP_text( _tag_arr("} while (", sizeof(
unsigned char), 10u)); struct Cyc_PP_Doc* _temp1691= Cyc_Absynpp_exp2doc(
_temp1505); struct Cyc_PP_Doc* _temp1692= Cyc_PP_text( _tag_arr(");", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp1684[ 7u]={ _temp1686, _temp1687,
_temp1688, _temp1689, _temp1690, _temp1691, _temp1692}; struct _tagged_arr
_temp1685={( void*) _temp1684,( void*) _temp1684,( void*)( _temp1684 + 7u)};
_temp1683( _temp1685);}); goto _LL1402; _LL1442: s=({ struct Cyc_PP_Doc*(*
_temp1693)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1696= Cyc_PP_text(
_tag_arr("try {", sizeof( unsigned char), 6u)); struct Cyc_PP_Doc* _temp1697=
Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1698= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1511)); struct Cyc_PP_Doc* _temp1699= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp1700= Cyc_PP_text( _tag_arr("} catch {", sizeof( unsigned char), 10u));
struct Cyc_PP_Doc* _temp1701= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1702=
Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc( _temp1509)); struct Cyc_PP_Doc*
_temp1703= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1704= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1694[ 9u]={
_temp1696, _temp1697, _temp1698, _temp1699, _temp1700, _temp1701, _temp1702,
_temp1703, _temp1704}; struct _tagged_arr _temp1695={( void*) _temp1694,( void*)
_temp1694,( void*)( _temp1694 + 9u)}; _temp1693( _temp1695);}); goto _LL1402;
_LL1444: s=({ struct Cyc_PP_Doc*(* _temp1705)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1708= Cyc_PP_text( _tag_arr("region<", sizeof(
unsigned char), 8u)); struct Cyc_PP_Doc* _temp1709= Cyc_PP_textptr( Cyc_Absynpp_get_name(
_temp1517)); struct Cyc_PP_Doc* _temp1710= Cyc_PP_text( _tag_arr(">", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp1711= Cyc_Absynpp_qvar2doc(
_temp1515->name); struct Cyc_PP_Doc* _temp1712= Cyc_PP_text( _tag_arr("{",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1713= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1714= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1513));
struct Cyc_PP_Doc* _temp1715= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1716=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1706[ 9u]={ _temp1708, _temp1709, _temp1710, _temp1711, _temp1712,
_temp1713, _temp1714, _temp1715, _temp1716}; struct _tagged_arr _temp1707={(
void*) _temp1706,( void*) _temp1706,( void*)( _temp1706 + 9u)}; _temp1705(
_temp1707);}); goto _LL1402; _LL1402:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void* _temp1717=( void*) p->r;
int _temp1749; void* _temp1751; unsigned char _temp1753; struct _tagged_arr
_temp1755; struct Cyc_Absyn_Vardecl* _temp1757; struct Cyc_List_List* _temp1759;
struct Cyc_Absyn_Pat* _temp1761; struct Cyc_Absyn_Vardecl* _temp1763; struct
_tuple0* _temp1765; struct Cyc_List_List* _temp1767; struct Cyc_List_List*
_temp1769; struct _tuple0* _temp1771; struct Cyc_List_List* _temp1773; struct
Cyc_List_List* _temp1775; struct _tuple0* _temp1777; struct Cyc_List_List*
_temp1779; struct Cyc_List_List* _temp1781; struct Cyc_Absyn_Structdecl*
_temp1783; struct Cyc_Absyn_Enumfield* _temp1785; struct Cyc_List_List*
_temp1787; struct Cyc_List_List* _temp1789; struct Cyc_Absyn_Tunionfield*
_temp1791; _LL1719: if( _temp1717 ==( void*) Cyc_Absyn_Wild_p){ goto _LL1720;}
else{ goto _LL1721;} _LL1721: if( _temp1717 ==( void*) Cyc_Absyn_Null_p){ goto
_LL1722;} else{ goto _LL1723;} _LL1723: if(( unsigned int) _temp1717 > 2u?*((
int*) _temp1717) == Cyc_Absyn_Int_p: 0){ _LL1752: _temp1751=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1717)->f1; goto _LL1750; _LL1750: _temp1749=((
struct Cyc_Absyn_Int_p_struct*) _temp1717)->f2; goto _LL1724;} else{ goto
_LL1725;} _LL1725: if(( unsigned int) _temp1717 > 2u?*(( int*) _temp1717) == Cyc_Absyn_Char_p:
0){ _LL1754: _temp1753=(( struct Cyc_Absyn_Char_p_struct*) _temp1717)->f1; goto
_LL1726;} else{ goto _LL1727;} _LL1727: if(( unsigned int) _temp1717 > 2u?*((
int*) _temp1717) == Cyc_Absyn_Float_p: 0){ _LL1756: _temp1755=(( struct Cyc_Absyn_Float_p_struct*)
_temp1717)->f1; goto _LL1728;} else{ goto _LL1729;} _LL1729: if(( unsigned int)
_temp1717 > 2u?*(( int*) _temp1717) == Cyc_Absyn_Var_p: 0){ _LL1758: _temp1757=((
struct Cyc_Absyn_Var_p_struct*) _temp1717)->f1; goto _LL1730;} else{ goto
_LL1731;} _LL1731: if(( unsigned int) _temp1717 > 2u?*(( int*) _temp1717) == Cyc_Absyn_Tuple_p:
0){ _LL1760: _temp1759=(( struct Cyc_Absyn_Tuple_p_struct*) _temp1717)->f1; goto
_LL1732;} else{ goto _LL1733;} _LL1733: if(( unsigned int) _temp1717 > 2u?*((
int*) _temp1717) == Cyc_Absyn_Pointer_p: 0){ _LL1762: _temp1761=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1717)->f1; goto _LL1734;} else{ goto _LL1735;} _LL1735: if(( unsigned int)
_temp1717 > 2u?*(( int*) _temp1717) == Cyc_Absyn_Reference_p: 0){ _LL1764:
_temp1763=(( struct Cyc_Absyn_Reference_p_struct*) _temp1717)->f1; goto _LL1736;}
else{ goto _LL1737;} _LL1737: if(( unsigned int) _temp1717 > 2u?*(( int*)
_temp1717) == Cyc_Absyn_UnknownId_p: 0){ _LL1766: _temp1765=(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp1717)->f1; goto _LL1738;} else{ goto _LL1739;} _LL1739: if(( unsigned int)
_temp1717 > 2u?*(( int*) _temp1717) == Cyc_Absyn_UnknownCall_p: 0){ _LL1772:
_temp1771=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1717)->f1; goto
_LL1770; _LL1770: _temp1769=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1717)->f2;
goto _LL1768; _LL1768: _temp1767=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1717)->f3; goto _LL1740;} else{ goto _LL1741;} _LL1741: if(( unsigned int)
_temp1717 > 2u?*(( int*) _temp1717) == Cyc_Absyn_UnknownFields_p: 0){ _LL1778:
_temp1777=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp1717)->f1; goto
_LL1776; _LL1776: _temp1775=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1717)->f2; goto _LL1774; _LL1774: _temp1773=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1717)->f3; goto _LL1742;} else{ goto _LL1743;} _LL1743: if(( unsigned int)
_temp1717 > 2u?*(( int*) _temp1717) == Cyc_Absyn_Struct_p: 0){ _LL1784:
_temp1783=(( struct Cyc_Absyn_Struct_p_struct*) _temp1717)->f1; goto _LL1782;
_LL1782: _temp1781=(( struct Cyc_Absyn_Struct_p_struct*) _temp1717)->f3; goto
_LL1780; _LL1780: _temp1779=(( struct Cyc_Absyn_Struct_p_struct*) _temp1717)->f4;
goto _LL1744;} else{ goto _LL1745;} _LL1745: if(( unsigned int) _temp1717 > 2u?*((
int*) _temp1717) == Cyc_Absyn_Enum_p: 0){ _LL1786: _temp1785=(( struct Cyc_Absyn_Enum_p_struct*)
_temp1717)->f2; goto _LL1746;} else{ goto _LL1747;} _LL1747: if(( unsigned int)
_temp1717 > 2u?*(( int*) _temp1717) == Cyc_Absyn_Tunion_p: 0){ _LL1792:
_temp1791=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1717)->f2; goto _LL1790;
_LL1790: _temp1789=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1717)->f3; goto
_LL1788; _LL1788: _temp1787=(( struct Cyc_Absyn_Tunion_p_struct*) _temp1717)->f4;
goto _LL1748;} else{ goto _LL1718;} _LL1720: s= Cyc_PP_text( _tag_arr("_",
sizeof( unsigned char), 2u)); goto _LL1718; _LL1722: s= Cyc_PP_text( _tag_arr("null",
sizeof( unsigned char), 5u)); goto _LL1718; _LL1724: if( _temp1751 ==( void*)
Cyc_Absyn_Signed){ s= Cyc_PP_text(( struct _tagged_arr) xprintf("%d", _temp1749));}
else{ s= Cyc_PP_text(( struct _tagged_arr) xprintf("%u",( unsigned int)
_temp1749));} goto _LL1718; _LL1726: s= Cyc_PP_text(( struct _tagged_arr)({
struct _tagged_arr _temp1793= Cyc_Absynpp_char_escape( _temp1753); xprintf("'%.*s'",
_get_arr_size( _temp1793, 1u), _temp1793.curr);})); goto _LL1718; _LL1728: s=
Cyc_PP_text( _temp1755); goto _LL1718; _LL1730: s= Cyc_Absynpp_qvar2doc(
_temp1757->name); goto _LL1718; _LL1732: s=({ struct Cyc_PP_Doc*(* _temp1794)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1797= Cyc_PP_text(
_tag_arr("$(", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp1798=((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Pat*), struct
_tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc,
_tag_arr(",", sizeof( unsigned char), 2u), _temp1759); struct Cyc_PP_Doc*
_temp1799= Cyc_PP_text( _tag_arr(")", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp1795[ 3u]={ _temp1797, _temp1798, _temp1799}; struct _tagged_arr _temp1796={(
void*) _temp1795,( void*) _temp1795,( void*)( _temp1795 + 3u)}; _temp1794(
_temp1796);}); goto _LL1718; _LL1734: s=({ struct Cyc_PP_Doc*(* _temp1800)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1803= Cyc_PP_text(
_tag_arr("&", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1804= Cyc_Absynpp_pat2doc(
_temp1761); struct Cyc_PP_Doc* _temp1801[ 2u]={ _temp1803, _temp1804}; struct
_tagged_arr _temp1802={( void*) _temp1801,( void*) _temp1801,( void*)( _temp1801
+ 2u)}; _temp1800( _temp1802);}); goto _LL1718; _LL1736: s=({ struct Cyc_PP_Doc*(*
_temp1805)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1808= Cyc_PP_text(
_tag_arr("*", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp1809= Cyc_Absynpp_qvar2doc(
_temp1763->name); struct Cyc_PP_Doc* _temp1806[ 2u]={ _temp1808, _temp1809};
struct _tagged_arr _temp1807={( void*) _temp1806,( void*) _temp1806,( void*)(
_temp1806 + 2u)}; _temp1805( _temp1807);}); goto _LL1718; _LL1738: s= Cyc_Absynpp_qvar2doc(
_temp1765); goto _LL1718; _LL1740: s=({ struct Cyc_PP_Doc*(* _temp1810)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1813= Cyc_Absynpp_qvar2doc(
_temp1771); struct Cyc_PP_Doc* _temp1814= Cyc_Absynpp_tvars2doc( _temp1769);
struct Cyc_PP_Doc* _temp1815= Cyc_PP_group( _tag_arr("(", sizeof( unsigned char),
2u), _tag_arr(")", sizeof( unsigned char), 2u), _tag_arr(",", sizeof(
unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc,
_temp1767)); struct Cyc_PP_Doc* _temp1811[ 3u]={ _temp1813, _temp1814, _temp1815};
struct _tagged_arr _temp1812={( void*) _temp1811,( void*) _temp1811,( void*)(
_temp1811 + 3u)}; _temp1810( _temp1812);}); goto _LL1718; _LL1742: s=({ struct
Cyc_PP_Doc*(* _temp1816)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1819= Cyc_Absynpp_qvar2doc( _temp1777); struct Cyc_PP_Doc* _temp1820= Cyc_Absynpp_tvars2doc(
_temp1775); struct Cyc_PP_Doc* _temp1821= Cyc_PP_group( _tag_arr("{", sizeof(
unsigned char), 2u), _tag_arr("}", sizeof( unsigned char), 2u), _tag_arr(",",
sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1773)); struct Cyc_PP_Doc* _temp1817[ 3u]={ _temp1819, _temp1820, _temp1821};
struct _tagged_arr _temp1818={( void*) _temp1817,( void*) _temp1817,( void*)(
_temp1817 + 3u)}; _temp1816( _temp1818);}); goto _LL1718; _LL1744: s=({ struct
Cyc_PP_Doc*(* _temp1822)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1825= _temp1783->name == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp1783->name))->v); struct Cyc_PP_Doc*
_temp1826= Cyc_Absynpp_tvars2doc( _temp1781); struct Cyc_PP_Doc* _temp1827= Cyc_PP_group(
_tag_arr("{", sizeof( unsigned char), 2u), _tag_arr("}", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_dp2doc, _temp1779)); struct Cyc_PP_Doc* _temp1823[ 3u]={ _temp1825,
_temp1826, _temp1827}; struct _tagged_arr _temp1824={( void*) _temp1823,( void*)
_temp1823,( void*)( _temp1823 + 3u)}; _temp1822( _temp1824);}); goto _LL1718;
_LL1746: s= Cyc_Absynpp_qvar2doc( _temp1785->name); goto _LL1718; _LL1748: if(
_temp1787 == 0){ s= Cyc_Absynpp_qvar2doc( _temp1791->name);} else{ s=({ struct
Cyc_PP_Doc*(* _temp1828)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1831= Cyc_Absynpp_qvar2doc( _temp1791->name); struct Cyc_PP_Doc* _temp1832=
Cyc_Absynpp_tvars2doc( _temp1789); struct Cyc_PP_Doc* _temp1833= Cyc_PP_egroup(
_tag_arr("(", sizeof( unsigned char), 2u), _tag_arr(")", sizeof( unsigned char),
2u), _tag_arr(",", sizeof( unsigned char), 2u),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_pat2doc, _temp1787)); struct Cyc_PP_Doc* _temp1829[ 3u]={ _temp1831,
_temp1832, _temp1833}; struct _tagged_arr _temp1830={( void*) _temp1829,( void*)
_temp1829,( void*)( _temp1829 + 3u)}; _temp1828( _temp1830);});} goto _LL1718;
_LL1718:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct _tuple5* dp){
return({ struct Cyc_PP_Doc*(* _temp1834)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1837= Cyc_PP_egroup( _tag_arr("", sizeof( unsigned char),
1u), _tag_arr("=", sizeof( unsigned char), 2u), _tag_arr("=", sizeof(
unsigned char), 2u),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(* dp).f1));
struct Cyc_PP_Doc* _temp1838= Cyc_Absynpp_pat2doc((* dp).f2); struct Cyc_PP_Doc*
_temp1835[ 2u]={ _temp1837, _temp1838}; struct _tagged_arr _temp1836={( void*)
_temp1835,( void*) _temp1835,( void*)( _temp1835 + 2u)}; _temp1834( _temp1836);});}
struct Cyc_PP_Doc* Cyc_Absynpp_switchclause2doc( struct Cyc_Absyn_Switch_clause*
c){ if( c->where_clause == 0?( void*)( c->pattern)->r ==( void*) Cyc_Absyn_Wild_p:
0){ return({ struct Cyc_PP_Doc*(* _temp1839)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1842= Cyc_PP_text( _tag_arr("default: ", sizeof(
unsigned char), 10u)); struct Cyc_PP_Doc* _temp1843= Cyc_PP_nest( 2,({ struct
Cyc_PP_Doc*(* _temp1844)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1847= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1848= Cyc_Absynpp_stmt2doc(
c->body); struct Cyc_PP_Doc* _temp1845[ 2u]={ _temp1847, _temp1848}; struct
_tagged_arr _temp1846={( void*) _temp1845,( void*) _temp1845,( void*)( _temp1845
+ 2u)}; _temp1844( _temp1846);})); struct Cyc_PP_Doc* _temp1840[ 2u]={ _temp1842,
_temp1843}; struct _tagged_arr _temp1841={( void*) _temp1840,( void*) _temp1840,(
void*)( _temp1840 + 2u)}; _temp1839( _temp1841);});} else{ if( c->where_clause
== 0){ return({ struct Cyc_PP_Doc*(* _temp1849)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1852= Cyc_PP_text( _tag_arr("case ", sizeof(
unsigned char), 6u)); struct Cyc_PP_Doc* _temp1853= Cyc_Absynpp_pat2doc( c->pattern);
struct Cyc_PP_Doc* _temp1854= Cyc_PP_text( _tag_arr(": ", sizeof( unsigned char),
3u)); struct Cyc_PP_Doc* _temp1855= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(*
_temp1856)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1859= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1860= Cyc_Absynpp_stmt2doc( c->body); struct Cyc_PP_Doc*
_temp1857[ 2u]={ _temp1859, _temp1860}; struct _tagged_arr _temp1858={( void*)
_temp1857,( void*) _temp1857,( void*)( _temp1857 + 2u)}; _temp1856( _temp1858);}));
struct Cyc_PP_Doc* _temp1850[ 4u]={ _temp1852, _temp1853, _temp1854, _temp1855};
struct _tagged_arr _temp1851={( void*) _temp1850,( void*) _temp1850,( void*)(
_temp1850 + 4u)}; _temp1849( _temp1851);});} else{ return({ struct Cyc_PP_Doc*(*
_temp1861)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1864= Cyc_PP_text(
_tag_arr("case ", sizeof( unsigned char), 6u)); struct Cyc_PP_Doc* _temp1865=
Cyc_Absynpp_pat2doc( c->pattern); struct Cyc_PP_Doc* _temp1866= Cyc_PP_text(
_tag_arr(" && ", sizeof( unsigned char), 5u)); struct Cyc_PP_Doc* _temp1867= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( c->where_clause)); struct Cyc_PP_Doc*
_temp1868= Cyc_PP_text( _tag_arr(": ", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc*
_temp1869= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp1870)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1873= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp1874= Cyc_Absynpp_stmt2doc( c->body); struct Cyc_PP_Doc* _temp1871[ 2u]={
_temp1873, _temp1874}; struct _tagged_arr _temp1872={( void*) _temp1871,( void*)
_temp1871,( void*)( _temp1871 + 2u)}; _temp1870( _temp1872);})); struct Cyc_PP_Doc*
_temp1862[ 6u]={ _temp1864, _temp1865, _temp1866, _temp1867, _temp1868,
_temp1869}; struct _tagged_arr _temp1863={( void*) _temp1862,( void*) _temp1862,(
void*)( _temp1862 + 6u)}; _temp1861( _temp1863);});}}} struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Switch_clause*), struct _tagged_arr sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc, _tag_arr("", sizeof(
unsigned char), 1u), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_switchCclause2doc(
struct Cyc_Absyn_SwitchC_clause* c){ struct Cyc_Absyn_SwitchC_clause _temp1877;
struct Cyc_Absyn_Stmt* _temp1878; struct Cyc_Absyn_Exp* _temp1880; struct Cyc_Absyn_SwitchC_clause*
_temp1875= c; _temp1877=* _temp1875; _LL1881: _temp1880= _temp1877.cnst_exp;
goto _LL1879; _LL1879: _temp1878= _temp1877.body; goto _LL1876; _LL1876: if(
_temp1880 == 0){ return({ struct Cyc_PP_Doc*(* _temp1882)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1885= Cyc_PP_text( _tag_arr("default: ",
sizeof( unsigned char), 10u)); struct Cyc_PP_Doc* _temp1886= Cyc_PP_nest( 2,({
struct Cyc_PP_Doc*(* _temp1887)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1890= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp1891= Cyc_Absynpp_stmt2doc(
c->body); struct Cyc_PP_Doc* _temp1888[ 2u]={ _temp1890, _temp1891}; struct
_tagged_arr _temp1889={( void*) _temp1888,( void*) _temp1888,( void*)( _temp1888
+ 2u)}; _temp1887( _temp1889);})); struct Cyc_PP_Doc* _temp1883[ 2u]={ _temp1885,
_temp1886}; struct _tagged_arr _temp1884={( void*) _temp1883,( void*) _temp1883,(
void*)( _temp1883 + 2u)}; _temp1882( _temp1884);});} else{ return({ struct Cyc_PP_Doc*(*
_temp1892)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1895= Cyc_PP_text(
_tag_arr("case ", sizeof( unsigned char), 6u)); struct Cyc_PP_Doc* _temp1896=
Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp1880)); struct
Cyc_PP_Doc* _temp1897= Cyc_PP_text( _tag_arr(": ", sizeof( unsigned char), 3u));
struct Cyc_PP_Doc* _temp1898= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp1899)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1902= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp1903= Cyc_Absynpp_stmt2doc( _temp1878); struct Cyc_PP_Doc*
_temp1900[ 2u]={ _temp1902, _temp1903}; struct _tagged_arr _temp1901={( void*)
_temp1900,( void*) _temp1900,( void*)( _temp1900 + 2u)}; _temp1899( _temp1901);}));
struct Cyc_PP_Doc* _temp1893[ 4u]={ _temp1895, _temp1896, _temp1897, _temp1898};
struct _tagged_arr _temp1894={( void*) _temp1893,( void*) _temp1893,( void*)(
_temp1893 + 4u)}; _temp1892( _temp1894);});}} struct Cyc_PP_Doc* Cyc_Absynpp_switchCclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_SwitchC_clause*), struct _tagged_arr sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchCclause2doc, _tag_arr("", sizeof(
unsigned char), 1u), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_enumfield2doc( struct
Cyc_Absyn_Enumfield* f){ if( f->tag == 0){ return Cyc_Absynpp_qvar2doc( f->name);}
else{ return({ struct Cyc_PP_Doc*(* _temp1904)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1907= Cyc_Absynpp_qvar2doc( f->name); struct Cyc_PP_Doc*
_temp1908= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc*
_temp1909= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( f->tag));
struct Cyc_PP_Doc* _temp1905[ 3u]={ _temp1907, _temp1908, _temp1909}; struct
_tagged_arr _temp1906={( void*) _temp1905,( void*) _temp1905,( void*)( _temp1905
+ 3u)}; _temp1904( _temp1906);});}} struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc(
struct Cyc_List_List* fs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Enumfield*), struct _tagged_arr sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc, _tag_arr(",", sizeof(
unsigned char), 2u), fs);} static struct Cyc_PP_Doc* Cyc_Absynpp_id2doc( struct
Cyc_Absyn_Vardecl* v){ return Cyc_Absynpp_qvar2doc( v->name);} static struct Cyc_PP_Doc*
Cyc_Absynpp_ids2doc( struct Cyc_List_List* vds){ return(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Vardecl*), struct _tagged_arr sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseq)( Cyc_Absynpp_id2doc, _tag_arr(",",
sizeof( unsigned char), 2u), vds);} struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl* d){ struct Cyc_PP_Doc* s;{ void* _temp1910=( void*) d->r;
struct Cyc_Absyn_Fndecl* _temp1936; struct Cyc_Absyn_Structdecl* _temp1938;
struct Cyc_Absyn_Uniondecl* _temp1940; struct Cyc_Absyn_Vardecl* _temp1942;
struct Cyc_Absyn_Vardecl _temp1944; struct Cyc_List_List* _temp1945; struct Cyc_Absyn_Exp*
_temp1947; void* _temp1949; struct Cyc_Absyn_Tqual _temp1951; struct _tuple0*
_temp1953; void* _temp1955; struct Cyc_Absyn_Tuniondecl* _temp1957; struct Cyc_Absyn_Tuniondecl
_temp1959; int _temp1960; struct Cyc_Core_Opt* _temp1962; struct Cyc_List_List*
_temp1964; struct _tuple0* _temp1966; void* _temp1968; struct Cyc_Absyn_Exp*
_temp1970; struct Cyc_Absyn_Pat* _temp1972; struct Cyc_List_List* _temp1974;
struct Cyc_Absyn_Enumdecl* _temp1976; struct Cyc_Absyn_Enumdecl _temp1978;
struct Cyc_Core_Opt* _temp1979; struct _tuple0* _temp1981; void* _temp1983;
struct Cyc_Absyn_Typedefdecl* _temp1985; struct Cyc_List_List* _temp1987; struct
_tagged_arr* _temp1989; struct Cyc_List_List* _temp1991; struct _tuple0*
_temp1993; struct Cyc_List_List* _temp1995; _LL1912: if(*(( int*) _temp1910) ==
Cyc_Absyn_Fn_d){ _LL1937: _temp1936=(( struct Cyc_Absyn_Fn_d_struct*) _temp1910)->f1;
goto _LL1913;} else{ goto _LL1914;} _LL1914: if(*(( int*) _temp1910) == Cyc_Absyn_Struct_d){
_LL1939: _temp1938=(( struct Cyc_Absyn_Struct_d_struct*) _temp1910)->f1; goto
_LL1915;} else{ goto _LL1916;} _LL1916: if(*(( int*) _temp1910) == Cyc_Absyn_Union_d){
_LL1941: _temp1940=(( struct Cyc_Absyn_Union_d_struct*) _temp1910)->f1; goto
_LL1917;} else{ goto _LL1918;} _LL1918: if(*(( int*) _temp1910) == Cyc_Absyn_Var_d){
_LL1943: _temp1942=(( struct Cyc_Absyn_Var_d_struct*) _temp1910)->f1; _temp1944=*
_temp1942; _LL1956: _temp1955=( void*) _temp1944.sc; goto _LL1954; _LL1954:
_temp1953= _temp1944.name; goto _LL1952; _LL1952: _temp1951= _temp1944.tq; goto
_LL1950; _LL1950: _temp1949=( void*) _temp1944.type; goto _LL1948; _LL1948:
_temp1947= _temp1944.initializer; goto _LL1946; _LL1946: _temp1945= _temp1944.attributes;
goto _LL1919;} else{ goto _LL1920;} _LL1920: if(*(( int*) _temp1910) == Cyc_Absyn_Tunion_d){
_LL1958: _temp1957=(( struct Cyc_Absyn_Tunion_d_struct*) _temp1910)->f1;
_temp1959=* _temp1957; _LL1969: _temp1968=( void*) _temp1959.sc; goto _LL1967;
_LL1967: _temp1966= _temp1959.name; goto _LL1965; _LL1965: _temp1964= _temp1959.tvs;
goto _LL1963; _LL1963: _temp1962= _temp1959.fields; goto _LL1961; _LL1961:
_temp1960= _temp1959.is_xtunion; goto _LL1921;} else{ goto _LL1922;} _LL1922:
if(*(( int*) _temp1910) == Cyc_Absyn_Let_d){ _LL1973: _temp1972=(( struct Cyc_Absyn_Let_d_struct*)
_temp1910)->f1; goto _LL1971; _LL1971: _temp1970=(( struct Cyc_Absyn_Let_d_struct*)
_temp1910)->f4; goto _LL1923;} else{ goto _LL1924;} _LL1924: if(*(( int*)
_temp1910) == Cyc_Absyn_Letv_d){ _LL1975: _temp1974=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1910)->f1; goto _LL1925;} else{ goto _LL1926;} _LL1926: if(*(( int*)
_temp1910) == Cyc_Absyn_Enum_d){ _LL1977: _temp1976=(( struct Cyc_Absyn_Enum_d_struct*)
_temp1910)->f1; _temp1978=* _temp1976; _LL1984: _temp1983=( void*) _temp1978.sc;
goto _LL1982; _LL1982: _temp1981= _temp1978.name; goto _LL1980; _LL1980:
_temp1979= _temp1978.fields; goto _LL1927;} else{ goto _LL1928;} _LL1928: if(*((
int*) _temp1910) == Cyc_Absyn_Typedef_d){ _LL1986: _temp1985=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp1910)->f1; goto _LL1929;} else{ goto _LL1930;} _LL1930: if(*(( int*)
_temp1910) == Cyc_Absyn_Namespace_d){ _LL1990: _temp1989=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1910)->f1; goto _LL1988; _LL1988: _temp1987=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1910)->f2; goto _LL1931;} else{ goto _LL1932;} _LL1932: if(*(( int*)
_temp1910) == Cyc_Absyn_Using_d){ _LL1994: _temp1993=(( struct Cyc_Absyn_Using_d_struct*)
_temp1910)->f1; goto _LL1992; _LL1992: _temp1991=(( struct Cyc_Absyn_Using_d_struct*)
_temp1910)->f2; goto _LL1933;} else{ goto _LL1934;} _LL1934: if(*(( int*)
_temp1910) == Cyc_Absyn_ExternC_d){ _LL1996: _temp1995=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp1910)->f1; goto _LL1935;} else{ goto _LL1911;} _LL1913: { void* t=( void*)({
struct Cyc_Absyn_FnType_struct* _temp2028=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2028[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2029; _temp2029.tag= Cyc_Absyn_FnType; _temp2029.f1=({ struct Cyc_Absyn_FnInfo
_temp2030; _temp2030.tvars= _temp1936->tvs; _temp2030.effect= _temp1936->effect;
_temp2030.ret_typ=( void*)(( void*) _temp1936->ret_type); _temp2030.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp1936->args); _temp2030.c_varargs=
_temp1936->c_varargs; _temp2030.cyc_varargs= _temp1936->cyc_varargs; _temp2030.rgn_po=
_temp1936->rgn_po; _temp2030.attributes= 0; _temp2030;}); _temp2029;});
_temp2028;}); struct Cyc_PP_Doc* attsdoc= Cyc_Absynpp_atts2doc( _temp1936->attributes);
struct Cyc_PP_Doc* inlinedoc; if( _temp1936->is_inline){ if( Cyc_Absynpp_to_VC){
inlinedoc= Cyc_PP_text( _tag_arr("__inline ", sizeof( unsigned char), 10u));}
else{ inlinedoc= Cyc_PP_text( _tag_arr("inline ", sizeof( unsigned char), 8u));}}
else{ inlinedoc= Cyc_PP_nil_doc();}{ struct Cyc_PP_Doc* scopedoc= Cyc_Absynpp_scope2doc((
void*) _temp1936->sc); struct Cyc_PP_Doc* beforenamedoc= Cyc_Absynpp_to_VC? Cyc_Absynpp_callconv2doc(
_temp1936->attributes): attsdoc; struct Cyc_PP_Doc* namedoc= Cyc_Absynpp_typedef_name2doc(
_temp1936->name); struct Cyc_PP_Doc* tqtddoc= Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual
_temp2021; _temp2021.q_const= 0; _temp2021.q_volatile= 0; _temp2021.q_restrict=
0; _temp2021;}), t,({ struct Cyc_Core_Opt* _temp2022=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2022->v=( void*)({ struct Cyc_PP_Doc*(*
_temp2023)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2026=
beforenamedoc; struct Cyc_PP_Doc* _temp2027= namedoc; struct Cyc_PP_Doc*
_temp2024[ 2u]={ _temp2026, _temp2027}; struct _tagged_arr _temp2025={( void*)
_temp2024,( void*) _temp2024,( void*)( _temp2024 + 2u)}; _temp2023( _temp2025);});
_temp2022;})); struct Cyc_PP_Doc* bodydoc=({ struct Cyc_PP_Doc*(* _temp2009)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2012= Cyc_PP_text(
_tag_arr(" {", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp2013= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc*(* _temp2016)( struct _tagged_arr)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp2019= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2020= Cyc_Absynpp_stmt2doc(
_temp1936->body); struct Cyc_PP_Doc* _temp2017[ 2u]={ _temp2019, _temp2020};
struct _tagged_arr _temp2018={( void*) _temp2017,( void*) _temp2017,( void*)(
_temp2017 + 2u)}; _temp2016( _temp2018);})); struct Cyc_PP_Doc* _temp2014= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2015= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp2010[ 4u]={ _temp2012, _temp2013, _temp2014,
_temp2015}; struct _tagged_arr _temp2011={( void*) _temp2010,( void*) _temp2010,(
void*)( _temp2010 + 4u)}; _temp2009( _temp2011);}); s=({ struct Cyc_PP_Doc*(*
_temp1997)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2000=
inlinedoc; struct Cyc_PP_Doc* _temp2001= scopedoc; struct Cyc_PP_Doc* _temp2002=
tqtddoc; struct Cyc_PP_Doc* _temp2003= bodydoc; struct Cyc_PP_Doc* _temp1998[ 4u]={
_temp2000, _temp2001, _temp2002, _temp2003}; struct _tagged_arr _temp1999={(
void*) _temp1998,( void*) _temp1998,( void*)( _temp1998 + 4u)}; _temp1997(
_temp1999);}); if( Cyc_Absynpp_to_VC){ s=({ struct Cyc_PP_Doc*(* _temp2004)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2007= attsdoc; struct
Cyc_PP_Doc* _temp2008= s; struct Cyc_PP_Doc* _temp2005[ 2u]={ _temp2007,
_temp2008}; struct _tagged_arr _temp2006={( void*) _temp2005,( void*) _temp2005,(
void*)( _temp2005 + 2u)}; _temp2004( _temp2006);});} goto _LL1911;}} _LL1915:
if( _temp1938->fields == 0){ s=({ struct Cyc_PP_Doc*(* _temp2031)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2034= Cyc_Absynpp_scope2doc((
void*) _temp1938->sc); struct Cyc_PP_Doc* _temp2035= Cyc_PP_text( _tag_arr("struct ",
sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp2036= _temp1938->name == 0?
Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_typedef_name2doc((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp1938->name))->v);
struct Cyc_PP_Doc* _temp2037= Cyc_Absynpp_ktvars2doc( _temp1938->tvs); struct
Cyc_PP_Doc* _temp2038= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u));
struct Cyc_PP_Doc* _temp2032[ 5u]={ _temp2034, _temp2035, _temp2036, _temp2037,
_temp2038}; struct _tagged_arr _temp2033={( void*) _temp2032,( void*) _temp2032,(
void*)( _temp2032 + 5u)}; _temp2031( _temp2033);});} else{ s=({ struct Cyc_PP_Doc*(*
_temp2039)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2042= Cyc_Absynpp_scope2doc((
void*) _temp1938->sc); struct Cyc_PP_Doc* _temp2043= Cyc_PP_text( _tag_arr("struct ",
sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp2044= _temp1938->name == 0?
Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_typedef_name2doc((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp1938->name))->v);
struct Cyc_PP_Doc* _temp2045= Cyc_Absynpp_ktvars2doc( _temp1938->tvs); struct
Cyc_PP_Doc* _temp2046= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u));
struct Cyc_PP_Doc* _temp2047= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2052)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2055= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2056= Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1938->fields))->v); struct Cyc_PP_Doc*
_temp2053[ 2u]={ _temp2055, _temp2056}; struct _tagged_arr _temp2054={( void*)
_temp2053,( void*) _temp2053,( void*)( _temp2053 + 2u)}; _temp2052( _temp2054);}));
struct Cyc_PP_Doc* _temp2048= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2049=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp2050= Cyc_Absynpp_atts2doc( _temp1938->attributes); struct Cyc_PP_Doc*
_temp2051= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp2040[ 10u]={ _temp2042, _temp2043, _temp2044, _temp2045, _temp2046,
_temp2047, _temp2048, _temp2049, _temp2050, _temp2051}; struct _tagged_arr
_temp2041={( void*) _temp2040,( void*) _temp2040,( void*)( _temp2040 + 10u)};
_temp2039( _temp2041);});} goto _LL1911; _LL1917: if( _temp1940->fields == 0){ s=({
struct Cyc_PP_Doc*(* _temp2057)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2060= Cyc_Absynpp_scope2doc(( void*) _temp1940->sc); struct Cyc_PP_Doc*
_temp2061= Cyc_PP_text( _tag_arr("union ", sizeof( unsigned char), 7u)); struct
Cyc_PP_Doc* _temp2062= _temp1940->name == 0? Cyc_PP_text( _tag_arr("", sizeof(
unsigned char), 1u)): Cyc_Absynpp_qvar2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp1940->name))->v); struct Cyc_PP_Doc* _temp2063= Cyc_Absynpp_tvars2doc(
_temp1940->tvs); struct Cyc_PP_Doc* _temp2064= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp2058[ 5u]={ _temp2060,
_temp2061, _temp2062, _temp2063, _temp2064}; struct _tagged_arr _temp2059={(
void*) _temp2058,( void*) _temp2058,( void*)( _temp2058 + 5u)}; _temp2057(
_temp2059);});} else{ s=({ struct Cyc_PP_Doc*(* _temp2065)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp2068= Cyc_Absynpp_scope2doc(( void*)
_temp1940->sc); struct Cyc_PP_Doc* _temp2069= Cyc_PP_text( _tag_arr("union ",
sizeof( unsigned char), 7u)); struct Cyc_PP_Doc* _temp2070= _temp1940->name == 0?
Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)): Cyc_Absynpp_qvar2doc((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp1940->name))->v);
struct Cyc_PP_Doc* _temp2071= Cyc_Absynpp_tvars2doc( _temp1940->tvs); struct Cyc_PP_Doc*
_temp2072= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc*
_temp2073= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2078)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp2081= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2082= Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1940->fields))->v); struct Cyc_PP_Doc* _temp2079[ 2u]={
_temp2081, _temp2082}; struct _tagged_arr _temp2080={( void*) _temp2079,( void*)
_temp2079,( void*)( _temp2079 + 2u)}; _temp2078( _temp2080);})); struct Cyc_PP_Doc*
_temp2074= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2075= Cyc_PP_text(
_tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp2076= Cyc_Absynpp_atts2doc(
_temp1940->attributes); struct Cyc_PP_Doc* _temp2077= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp2066[ 10u]={ _temp2068,
_temp2069, _temp2070, _temp2071, _temp2072, _temp2073, _temp2074, _temp2075,
_temp2076, _temp2077}; struct _tagged_arr _temp2067={( void*) _temp2066,( void*)
_temp2066,( void*)( _temp2066 + 10u)}; _temp2065( _temp2067);});} goto _LL1911;
_LL1919: { struct Cyc_PP_Doc* sn= Cyc_Absynpp_typedef_name2doc( _temp1953);
struct Cyc_PP_Doc* attsdoc= Cyc_Absynpp_atts2doc( _temp1945); struct Cyc_PP_Doc*
beforenamedoc; if( ! Cyc_Absynpp_to_VC){ beforenamedoc= attsdoc;} else{ void*
_temp2083= Cyc_Tcutil_compress( _temp1949); struct Cyc_Absyn_FnInfo _temp2089;
struct Cyc_List_List* _temp2091; _LL2085: if(( unsigned int) _temp2083 > 4u?*((
int*) _temp2083) == Cyc_Absyn_FnType: 0){ _LL2090: _temp2089=(( struct Cyc_Absyn_FnType_struct*)
_temp2083)->f1; _LL2092: _temp2091= _temp2089.attributes; goto _LL2086;} else{
goto _LL2087;} _LL2087: goto _LL2088; _LL2086: beforenamedoc= Cyc_Absynpp_callconv2doc(
_temp2091); goto _LL2084; _LL2088: beforenamedoc= Cyc_PP_nil_doc(); goto _LL2084;
_LL2084:;} s=({ struct Cyc_PP_Doc*(* _temp2093)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2096= Cyc_Absynpp_to_VC? attsdoc: Cyc_PP_nil_doc();
struct Cyc_PP_Doc* _temp2097= Cyc_Absynpp_scope2doc( _temp1955); struct Cyc_PP_Doc*
_temp2098= Cyc_Absynpp_tqtd2doc( _temp1951, _temp1949,({ struct Cyc_Core_Opt*
_temp2106=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2106->v=( void*)({ struct Cyc_PP_Doc*(* _temp2107)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp2110= beforenamedoc; struct Cyc_PP_Doc*
_temp2111= sn; struct Cyc_PP_Doc* _temp2108[ 2u]={ _temp2110, _temp2111}; struct
_tagged_arr _temp2109={( void*) _temp2108,( void*) _temp2108,( void*)( _temp2108
+ 2u)}; _temp2107( _temp2109);}); _temp2106;})); struct Cyc_PP_Doc* _temp2099=
_temp1947 == 0? Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u)):({ struct
Cyc_PP_Doc*(* _temp2101)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2104= Cyc_PP_text( _tag_arr(" = ", sizeof( unsigned char), 4u)); struct Cyc_PP_Doc*
_temp2105= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp1947));
struct Cyc_PP_Doc* _temp2102[ 2u]={ _temp2104, _temp2105}; struct _tagged_arr
_temp2103={( void*) _temp2102,( void*) _temp2102,( void*)( _temp2102 + 2u)};
_temp2101( _temp2103);}); struct Cyc_PP_Doc* _temp2100= Cyc_PP_text( _tag_arr(";",
sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp2094[ 5u]={ _temp2096,
_temp2097, _temp2098, _temp2099, _temp2100}; struct _tagged_arr _temp2095={(
void*) _temp2094,( void*) _temp2094,( void*)( _temp2094 + 5u)}; _temp2093(
_temp2095);}); goto _LL1911;} _LL1921: if( _temp1962 == 0){ s=({ struct Cyc_PP_Doc*(*
_temp2112)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2115= Cyc_Absynpp_scope2doc(
_temp1968); struct Cyc_PP_Doc* _temp2116= _temp1960? Cyc_PP_text( _tag_arr("xtunion ",
sizeof( unsigned char), 9u)): Cyc_PP_text( _tag_arr("tunion ", sizeof(
unsigned char), 8u)); struct Cyc_PP_Doc* _temp2117= _temp1960? Cyc_Absynpp_qvar2doc(
_temp1966): Cyc_Absynpp_typedef_name2doc( _temp1966); struct Cyc_PP_Doc*
_temp2118= Cyc_Absynpp_ktvars2doc( _temp1964); struct Cyc_PP_Doc* _temp2119= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp2113[ 5u]={
_temp2115, _temp2116, _temp2117, _temp2118, _temp2119}; struct _tagged_arr
_temp2114={( void*) _temp2113,( void*) _temp2113,( void*)( _temp2113 + 5u)};
_temp2112( _temp2114);});} else{ s=({ struct Cyc_PP_Doc*(* _temp2120)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2123= Cyc_Absynpp_scope2doc(
_temp1968); struct Cyc_PP_Doc* _temp2124= _temp1960? Cyc_PP_text( _tag_arr("xtunion ",
sizeof( unsigned char), 9u)): Cyc_PP_text( _tag_arr("tunion ", sizeof(
unsigned char), 8u)); struct Cyc_PP_Doc* _temp2125= _temp1960? Cyc_Absynpp_qvar2doc(
_temp1966): Cyc_Absynpp_typedef_name2doc( _temp1966); struct Cyc_PP_Doc*
_temp2126= Cyc_Absynpp_ktvars2doc( _temp1964); struct Cyc_PP_Doc* _temp2127= Cyc_PP_text(
_tag_arr(" {", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp2128= Cyc_PP_nest(
2,({ struct Cyc_PP_Doc*(* _temp2131)( struct _tagged_arr)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp2134= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2135= Cyc_Absynpp_tunionfields2doc((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1962))->v);
struct Cyc_PP_Doc* _temp2132[ 2u]={ _temp2134, _temp2135}; struct _tagged_arr
_temp2133={( void*) _temp2132,( void*) _temp2132,( void*)( _temp2132 + 2u)};
_temp2131( _temp2133);})); struct Cyc_PP_Doc* _temp2129= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2130= Cyc_PP_text( _tag_arr("};", sizeof( unsigned char),
3u)); struct Cyc_PP_Doc* _temp2121[ 8u]={ _temp2123, _temp2124, _temp2125,
_temp2126, _temp2127, _temp2128, _temp2129, _temp2130}; struct _tagged_arr
_temp2122={( void*) _temp2121,( void*) _temp2121,( void*)( _temp2121 + 8u)};
_temp2120( _temp2122);});} goto _LL1911; _LL1923: s=({ struct Cyc_PP_Doc*(*
_temp2136)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2139= Cyc_PP_text(
_tag_arr("let ", sizeof( unsigned char), 5u)); struct Cyc_PP_Doc* _temp2140= Cyc_Absynpp_pat2doc(
_temp1972); struct Cyc_PP_Doc* _temp2141= Cyc_PP_text( _tag_arr(" = ", sizeof(
unsigned char), 4u)); struct Cyc_PP_Doc* _temp2142= Cyc_Absynpp_exp2doc(
_temp1970); struct Cyc_PP_Doc* _temp2143= Cyc_PP_text( _tag_arr(";", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp2137[ 5u]={ _temp2139, _temp2140,
_temp2141, _temp2142, _temp2143}; struct _tagged_arr _temp2138={( void*)
_temp2137,( void*) _temp2137,( void*)( _temp2137 + 5u)}; _temp2136( _temp2138);});
goto _LL1911; _LL1925: s=({ struct Cyc_PP_Doc*(* _temp2144)( struct _tagged_arr)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp2147= Cyc_PP_text( _tag_arr("let ", sizeof(
unsigned char), 5u)); struct Cyc_PP_Doc* _temp2148= Cyc_Absynpp_ids2doc(
_temp1974); struct Cyc_PP_Doc* _temp2149= Cyc_PP_text( _tag_arr(";", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp2145[ 3u]={ _temp2147, _temp2148,
_temp2149}; struct _tagged_arr _temp2146={( void*) _temp2145,( void*) _temp2145,(
void*)( _temp2145 + 3u)}; _temp2144( _temp2146);}); goto _LL1911; _LL1927: if(
_temp1979 == 0){ s=({ struct Cyc_PP_Doc*(* _temp2150)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2153= Cyc_Absynpp_scope2doc( _temp1983); struct Cyc_PP_Doc*
_temp2154= Cyc_PP_text( _tag_arr("enum ", sizeof( unsigned char), 6u)); struct
Cyc_PP_Doc* _temp2155= Cyc_Absynpp_typedef_name2doc( _temp1981); struct Cyc_PP_Doc*
_temp2156= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp2151[ 4u]={ _temp2153, _temp2154, _temp2155, _temp2156}; struct _tagged_arr
_temp2152={( void*) _temp2151,( void*) _temp2151,( void*)( _temp2151 + 4u)};
_temp2150( _temp2152);});} else{ s=({ struct Cyc_PP_Doc*(* _temp2157)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2160= Cyc_Absynpp_scope2doc(
_temp1983); struct Cyc_PP_Doc* _temp2161= Cyc_PP_text( _tag_arr("enum ", sizeof(
unsigned char), 6u)); struct Cyc_PP_Doc* _temp2162= Cyc_Absynpp_qvar2doc(
_temp1981); struct Cyc_PP_Doc* _temp2163= Cyc_PP_text( _tag_arr(" {", sizeof(
unsigned char), 3u)); struct Cyc_PP_Doc* _temp2164= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(*
_temp2167)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2170= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2171= Cyc_Absynpp_enumfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1979))->v); struct Cyc_PP_Doc* _temp2168[
2u]={ _temp2170, _temp2171}; struct _tagged_arr _temp2169={( void*) _temp2168,(
void*) _temp2168,( void*)( _temp2168 + 2u)}; _temp2167( _temp2169);})); struct
Cyc_PP_Doc* _temp2165= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2166= Cyc_PP_text(
_tag_arr("};", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp2158[ 7u]={
_temp2160, _temp2161, _temp2162, _temp2163, _temp2164, _temp2165, _temp2166};
struct _tagged_arr _temp2159={( void*) _temp2158,( void*) _temp2158,( void*)(
_temp2158 + 7u)}; _temp2157( _temp2159);});} goto _LL1911; _LL1929: s=({ struct
Cyc_PP_Doc*(* _temp2172)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2175= Cyc_PP_text( _tag_arr("typedef ", sizeof( unsigned char), 9u));
struct Cyc_PP_Doc* _temp2176= Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual
_temp2178; _temp2178.q_const= 0; _temp2178.q_volatile= 0; _temp2178.q_restrict=
0; _temp2178;}),( void*) _temp1985->defn,({ struct Cyc_Core_Opt* _temp2179=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2179->v=(
void*)({ struct Cyc_PP_Doc*(* _temp2180)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2183= Cyc_Absynpp_typedef_name2doc( _temp1985->name);
struct Cyc_PP_Doc* _temp2184= Cyc_Absynpp_tvars2doc( _temp1985->tvs); struct Cyc_PP_Doc*
_temp2181[ 2u]={ _temp2183, _temp2184}; struct _tagged_arr _temp2182={( void*)
_temp2181,( void*) _temp2181,( void*)( _temp2181 + 2u)}; _temp2180( _temp2182);});
_temp2179;})); struct Cyc_PP_Doc* _temp2177= Cyc_PP_text( _tag_arr(";", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp2173[ 3u]={ _temp2175, _temp2176,
_temp2177}; struct _tagged_arr _temp2174={( void*) _temp2173,( void*) _temp2173,(
void*)( _temp2173 + 3u)}; _temp2172( _temp2174);}); goto _LL1911; _LL1931: if(
Cyc_Absynpp_use_curr_namespace){ Cyc_Absynpp_curr_namespace_add( _temp1989);} s=({
struct Cyc_PP_Doc*(* _temp2185)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2188= Cyc_PP_text( _tag_arr("namespace ", sizeof( unsigned char), 11u));
struct Cyc_PP_Doc* _temp2189= Cyc_PP_textptr( _temp1989); struct Cyc_PP_Doc*
_temp2190= Cyc_PP_text( _tag_arr(" {", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc*
_temp2191= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2192=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_arr sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc, _tag_arr("",
sizeof( unsigned char), 1u), _temp1987); struct Cyc_PP_Doc* _temp2193= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2194= Cyc_PP_text( _tag_arr("}", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp2186[ 7u]={ _temp2188, _temp2189, _temp2190,
_temp2191, _temp2192, _temp2193, _temp2194}; struct _tagged_arr _temp2187={(
void*) _temp2186,( void*) _temp2186,( void*)( _temp2186 + 7u)}; _temp2185(
_temp2187);}); if( Cyc_Absynpp_use_curr_namespace){ Cyc_Absynpp_curr_namespace_drop();}
goto _LL1911; _LL1933: if( Cyc_Absynpp_print_using_stmts){ s=({ struct Cyc_PP_Doc*(*
_temp2195)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2198= Cyc_PP_text(
_tag_arr("using ", sizeof( unsigned char), 7u)); struct Cyc_PP_Doc* _temp2199=
Cyc_Absynpp_qvar2doc( _temp1993); struct Cyc_PP_Doc* _temp2200= Cyc_PP_text(
_tag_arr(" {", sizeof( unsigned char), 3u)); struct Cyc_PP_Doc* _temp2201= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2202=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)(
struct Cyc_Absyn_Decl*), struct _tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc, _tag_arr("", sizeof( unsigned char), 1u), _temp1991);
struct Cyc_PP_Doc* _temp2203= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2204=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp2196[ 7u]={ _temp2198, _temp2199, _temp2200, _temp2201, _temp2202,
_temp2203, _temp2204}; struct _tagged_arr _temp2197={( void*) _temp2196,( void*)
_temp2196,( void*)( _temp2196 + 7u)}; _temp2195( _temp2197);});} else{ s=({
struct Cyc_PP_Doc*(* _temp2205)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2208= Cyc_PP_text( _tag_arr("/* using ", sizeof( unsigned char), 10u));
struct Cyc_PP_Doc* _temp2209= Cyc_Absynpp_qvar2doc( _temp1993); struct Cyc_PP_Doc*
_temp2210= Cyc_PP_text( _tag_arr(" { */", sizeof( unsigned char), 6u)); struct
Cyc_PP_Doc* _temp2211= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2212=(( struct
Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,
_tag_arr("", sizeof( unsigned char), 1u), _temp1991); struct Cyc_PP_Doc*
_temp2213= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2214= Cyc_PP_text(
_tag_arr("/* } */", sizeof( unsigned char), 8u)); struct Cyc_PP_Doc* _temp2206[
7u]={ _temp2208, _temp2209, _temp2210, _temp2211, _temp2212, _temp2213,
_temp2214}; struct _tagged_arr _temp2207={( void*) _temp2206,( void*) _temp2206,(
void*)( _temp2206 + 7u)}; _temp2205( _temp2207);});} goto _LL1911; _LL1935: if(
Cyc_Absynpp_print_externC_stmts){ s=({ struct Cyc_PP_Doc*(* _temp2215)( struct
_tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2218= Cyc_PP_text( _tag_arr("extern \"C\" {",
sizeof( unsigned char), 13u)); struct Cyc_PP_Doc* _temp2219= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2220=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)(
struct Cyc_Absyn_Decl*), struct _tagged_arr sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc, _tag_arr("", sizeof( unsigned char), 1u), _temp1995);
struct Cyc_PP_Doc* _temp2221= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2222=
Cyc_PP_text( _tag_arr("}", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc*
_temp2216[ 5u]={ _temp2218, _temp2219, _temp2220, _temp2221, _temp2222}; struct
_tagged_arr _temp2217={( void*) _temp2216,( void*) _temp2216,( void*)( _temp2216
+ 5u)}; _temp2215( _temp2217);});} else{ s=({ struct Cyc_PP_Doc*(* _temp2223)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2226= Cyc_PP_text(
_tag_arr("/* extern \"C\" { */", sizeof( unsigned char), 19u)); struct Cyc_PP_Doc*
_temp2227= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2228=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_arr sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc, _tag_arr("",
sizeof( unsigned char), 1u), _temp1995); struct Cyc_PP_Doc* _temp2229= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2230= Cyc_PP_text( _tag_arr("/* } */", sizeof(
unsigned char), 8u)); struct Cyc_PP_Doc* _temp2224[ 5u]={ _temp2226, _temp2227,
_temp2228, _temp2229, _temp2230}; struct _tagged_arr _temp2225={( void*)
_temp2224,( void*) _temp2224,( void*)( _temp2224 + 5u)}; _temp2223( _temp2225);});}
goto _LL1911; _LL1911:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_scope2doc(
void* sc){ void* _temp2231= sc; _LL2233: if( _temp2231 ==( void*) Cyc_Absyn_Static){
goto _LL2234;} else{ goto _LL2235;} _LL2235: if( _temp2231 ==( void*) Cyc_Absyn_Public){
goto _LL2236;} else{ goto _LL2237;} _LL2237: if( _temp2231 ==( void*) Cyc_Absyn_Extern){
goto _LL2238;} else{ goto _LL2239;} _LL2239: if( _temp2231 ==( void*) Cyc_Absyn_ExternC){
goto _LL2240;} else{ goto _LL2241;} _LL2241: if( _temp2231 ==( void*) Cyc_Absyn_Abstract){
goto _LL2242;} else{ goto _LL2232;} _LL2234: return Cyc_PP_text( _tag_arr("static ",
sizeof( unsigned char), 8u)); _LL2236: return Cyc_PP_nil_doc(); _LL2238: return
Cyc_PP_text( _tag_arr("extern ", sizeof( unsigned char), 8u)); _LL2240: return
Cyc_PP_text( _tag_arr("extern \"C\" ", sizeof( unsigned char), 12u)); _LL2242:
return Cyc_PP_text( _tag_arr("abstract ", sizeof( unsigned char), 10u)); _LL2232:;}
int Cyc_Absynpp_exists_temp_tvar_in_effect( void* t){ void* _temp2243= t; struct
Cyc_Absyn_Tvar* _temp2251; struct Cyc_List_List* _temp2253; _LL2245: if((
unsigned int) _temp2243 > 4u?*(( int*) _temp2243) == Cyc_Absyn_VarType: 0){
_LL2252: _temp2251=(( struct Cyc_Absyn_VarType_struct*) _temp2243)->f1; goto
_LL2246;} else{ goto _LL2247;} _LL2247: if(( unsigned int) _temp2243 > 4u?*((
int*) _temp2243) == Cyc_Absyn_JoinEff: 0){ _LL2254: _temp2253=(( struct Cyc_Absyn_JoinEff_struct*)
_temp2243)->f1; goto _LL2248;} else{ goto _LL2249;} _LL2249: goto _LL2250;
_LL2246: return Cyc_Tcutil_is_temp_tvar( _temp2251); _LL2248: return Cyc_List_exists(
Cyc_Absynpp_exists_temp_tvar_in_effect, _temp2253); _LL2250: return 0; _LL2244:;}
struct _tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual tq, void* t){ void*
_temp2255= t; struct Cyc_Absyn_Exp* _temp2269; struct Cyc_Absyn_Tqual _temp2271;
void* _temp2273; struct Cyc_Absyn_PtrInfo _temp2275; struct Cyc_Absyn_Conref*
_temp2277; struct Cyc_Absyn_Tqual _temp2279; struct Cyc_Absyn_Conref* _temp2281;
void* _temp2283; void* _temp2285; struct Cyc_Absyn_FnInfo _temp2287; struct Cyc_List_List*
_temp2289; struct Cyc_List_List* _temp2291; struct Cyc_Absyn_VarargInfo*
_temp2293; int _temp2295; struct Cyc_List_List* _temp2297; void* _temp2299;
struct Cyc_Core_Opt* _temp2301; struct Cyc_List_List* _temp2303; int _temp2305;
struct Cyc_Core_Opt* _temp2307; struct Cyc_Core_Opt* _temp2309; struct Cyc_Core_Opt*
_temp2311; struct Cyc_List_List* _temp2313; struct _tuple0* _temp2315; _LL2257:
if(( unsigned int) _temp2255 > 4u?*(( int*) _temp2255) == Cyc_Absyn_ArrayType: 0){
_LL2274: _temp2273=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2255)->f1;
goto _LL2272; _LL2272: _temp2271=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2255)->f2; goto _LL2270; _LL2270: _temp2269=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2255)->f3; goto _LL2258;} else{ goto _LL2259;} _LL2259: if(( unsigned int)
_temp2255 > 4u?*(( int*) _temp2255) == Cyc_Absyn_PointerType: 0){ _LL2276:
_temp2275=(( struct Cyc_Absyn_PointerType_struct*) _temp2255)->f1; _LL2286:
_temp2285=( void*) _temp2275.elt_typ; goto _LL2284; _LL2284: _temp2283=( void*)
_temp2275.rgn_typ; goto _LL2282; _LL2282: _temp2281= _temp2275.nullable; goto
_LL2280; _LL2280: _temp2279= _temp2275.tq; goto _LL2278; _LL2278: _temp2277=
_temp2275.bounds; goto _LL2260;} else{ goto _LL2261;} _LL2261: if(( unsigned int)
_temp2255 > 4u?*(( int*) _temp2255) == Cyc_Absyn_FnType: 0){ _LL2288: _temp2287=((
struct Cyc_Absyn_FnType_struct*) _temp2255)->f1; _LL2304: _temp2303= _temp2287.tvars;
goto _LL2302; _LL2302: _temp2301= _temp2287.effect; goto _LL2300; _LL2300:
_temp2299=( void*) _temp2287.ret_typ; goto _LL2298; _LL2298: _temp2297=
_temp2287.args; goto _LL2296; _LL2296: _temp2295= _temp2287.c_varargs; goto
_LL2294; _LL2294: _temp2293= _temp2287.cyc_varargs; goto _LL2292; _LL2292:
_temp2291= _temp2287.rgn_po; goto _LL2290; _LL2290: _temp2289= _temp2287.attributes;
goto _LL2262;} else{ goto _LL2263;} _LL2263: if(( unsigned int) _temp2255 > 4u?*((
int*) _temp2255) == Cyc_Absyn_Evar: 0){ _LL2310: _temp2309=(( struct Cyc_Absyn_Evar_struct*)
_temp2255)->f1; goto _LL2308; _LL2308: _temp2307=(( struct Cyc_Absyn_Evar_struct*)
_temp2255)->f2; goto _LL2306; _LL2306: _temp2305=(( struct Cyc_Absyn_Evar_struct*)
_temp2255)->f3; goto _LL2264;} else{ goto _LL2265;} _LL2265: if(( unsigned int)
_temp2255 > 4u?*(( int*) _temp2255) == Cyc_Absyn_TypedefType: 0){ _LL2316:
_temp2315=(( struct Cyc_Absyn_TypedefType_struct*) _temp2255)->f1; goto _LL2314;
_LL2314: _temp2313=(( struct Cyc_Absyn_TypedefType_struct*) _temp2255)->f2; goto
_LL2312; _LL2312: _temp2311=(( struct Cyc_Absyn_TypedefType_struct*) _temp2255)->f3;
goto _LL2266;} else{ goto _LL2267;} _LL2267: goto _LL2268; _LL2258: { struct Cyc_List_List*
_temp2319; void* _temp2321; struct Cyc_Absyn_Tqual _temp2323; struct _tuple4
_temp2317= Cyc_Absynpp_to_tms( _temp2271, _temp2273); _LL2324: _temp2323=
_temp2317.f1; goto _LL2322; _LL2322: _temp2321= _temp2317.f2; goto _LL2320;
_LL2320: _temp2319= _temp2317.f3; goto _LL2318; _LL2318: { void* tm; if(
_temp2269 == 0){ tm=( void*) Cyc_Absyn_Carray_mod;} else{ tm=( void*)({ struct
Cyc_Absyn_ConstArray_mod_struct* _temp2325=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2325[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2326; _temp2326.tag= Cyc_Absyn_ConstArray_mod;
_temp2326.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp2269); _temp2326;});
_temp2325;});} return({ struct _tuple4 _temp2327; _temp2327.f1= _temp2323;
_temp2327.f2= _temp2321; _temp2327.f3=({ struct Cyc_List_List* _temp2328=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2328->hd=(
void*) tm; _temp2328->tl= _temp2319; _temp2328;}); _temp2327;});}} _LL2260: {
struct Cyc_List_List* _temp2331; void* _temp2333; struct Cyc_Absyn_Tqual
_temp2335; struct _tuple4 _temp2329= Cyc_Absynpp_to_tms( _temp2279, _temp2285);
_LL2336: _temp2335= _temp2329.f1; goto _LL2334; _LL2334: _temp2333= _temp2329.f2;
goto _LL2332; _LL2332: _temp2331= _temp2329.f3; goto _LL2330; _LL2330: { void*
ps;{ struct _tuple8 _temp2338=({ struct _tuple8 _temp2337; _temp2337.f1=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp2281))->v; _temp2337.f2=( void*)( Cyc_Absyn_compress_conref( _temp2277))->v;
_temp2337;}); void* _temp2348; void* _temp2350; struct Cyc_Absyn_Exp* _temp2352;
void* _temp2354; int _temp2356; void* _temp2358; void* _temp2360; struct Cyc_Absyn_Exp*
_temp2362; void* _temp2364; int _temp2366; void* _temp2368; void* _temp2370;
_LL2340: _LL2355: _temp2354= _temp2338.f1; if(( unsigned int) _temp2354 > 1u?*((
int*) _temp2354) == Cyc_Absyn_Eq_constr: 0){ _LL2357: _temp2356=( int)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp2354)->f1; if( _temp2356 == 1){ goto _LL2349;}
else{ goto _LL2342;}} else{ goto _LL2342;} _LL2349: _temp2348= _temp2338.f2; if((
unsigned int) _temp2348 > 1u?*(( int*) _temp2348) == Cyc_Absyn_Eq_constr: 0){
_LL2351: _temp2350=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2348)->f1;
if(( unsigned int) _temp2350 > 1u?*(( int*) _temp2350) == Cyc_Absyn_Upper_b: 0){
_LL2353: _temp2352=(( struct Cyc_Absyn_Upper_b_struct*) _temp2350)->f1; goto
_LL2341;} else{ goto _LL2342;}} else{ goto _LL2342;} _LL2342: _LL2365: _temp2364=
_temp2338.f1; if(( unsigned int) _temp2364 > 1u?*(( int*) _temp2364) == Cyc_Absyn_Eq_constr:
0){ _LL2367: _temp2366=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2364)->f1;
if( _temp2366 == 0){ goto _LL2359;} else{ goto _LL2344;}} else{ goto _LL2344;}
_LL2359: _temp2358= _temp2338.f2; if(( unsigned int) _temp2358 > 1u?*(( int*)
_temp2358) == Cyc_Absyn_Eq_constr: 0){ _LL2361: _temp2360=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2358)->f1; if(( unsigned int) _temp2360 > 1u?*(( int*) _temp2360) == Cyc_Absyn_Upper_b:
0){ _LL2363: _temp2362=(( struct Cyc_Absyn_Upper_b_struct*) _temp2360)->f1; goto
_LL2343;} else{ goto _LL2344;}} else{ goto _LL2344;} _LL2344: _LL2369: _temp2368=
_temp2338.f2; if(( unsigned int) _temp2368 > 1u?*(( int*) _temp2368) == Cyc_Absyn_Eq_constr:
0){ _LL2371: _temp2370=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2368)->f1;
if( _temp2370 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL2345;} else{ goto _LL2346;}}
else{ goto _LL2346;} _LL2346: goto _LL2347; _LL2341: ps=( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2372=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2372[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2373; _temp2373.tag=
Cyc_Absyn_Nullable_ps; _temp2373.f1= _temp2352; _temp2373;}); _temp2372;}); goto
_LL2339; _LL2343: ps=( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2374=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2374[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2375; _temp2375.tag= Cyc_Absyn_NonNullable_ps; _temp2375.f1= _temp2362;
_temp2375;}); _temp2374;}); goto _LL2339; _LL2345: ps=( void*) Cyc_Absyn_TaggedArray_ps;
goto _LL2339; _LL2347: ps=( void*) Cyc_Absyn_TaggedArray_ps; goto _LL2339;
_LL2339:;}{ void* tm=( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2378=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp2378[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp2379; _temp2379.tag=
Cyc_Absyn_Pointer_mod; _temp2379.f1=( void*) ps; _temp2379.f2=( void*) _temp2283;
_temp2379.f3= tq; _temp2379;}); _temp2378;}); return({ struct _tuple4 _temp2376;
_temp2376.f1= _temp2335; _temp2376.f2= _temp2333; _temp2376.f3=({ struct Cyc_List_List*
_temp2377=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2377->hd=( void*) tm; _temp2377->tl= _temp2331; _temp2377;}); _temp2376;});}}}
_LL2262: if( ! Cyc_Absynpp_print_all_tvars){ if( _temp2301 == 0? 1: Cyc_Absynpp_exists_temp_tvar_in_effect((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp2301))->v)){ _temp2301= 0;
_temp2303= 0;}} else{ if( Cyc_Absynpp_rewrite_temp_tvars){(( void(*)( void(* f)(
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Tcutil_rewrite_temp_tvar,
_temp2303);}}{ struct Cyc_List_List* _temp2382; void* _temp2384; struct Cyc_Absyn_Tqual
_temp2386; struct _tuple4 _temp2380= Cyc_Absynpp_to_tms( Cyc_Absyn_empty_tqual(),
_temp2299); _LL2387: _temp2386= _temp2380.f1; goto _LL2385; _LL2385: _temp2384=
_temp2380.f2; goto _LL2383; _LL2383: _temp2382= _temp2380.f3; goto _LL2381;
_LL2381: { struct Cyc_List_List* tms= _temp2382; if( _temp2289 != 0? ! Cyc_Absynpp_to_VC:
0){ tms=({ struct Cyc_List_List* _temp2388=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2388->hd=( void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct*
_temp2389=( struct Cyc_Absyn_Attributes_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Attributes_mod_struct)); _temp2389[ 0]=({ struct Cyc_Absyn_Attributes_mod_struct
_temp2390; _temp2390.tag= Cyc_Absyn_Attributes_mod; _temp2390.f1= 0; _temp2390.f2=
_temp2289; _temp2390;}); _temp2389;})); _temp2388->tl= tms; _temp2388;});} tms=({
struct Cyc_List_List* _temp2391=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2391->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp2392=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp2392[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp2393; _temp2393.tag=
Cyc_Absyn_Function_mod; _temp2393.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp2394=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2394[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2395; _temp2395.tag= Cyc_Absyn_WithTypes;
_temp2395.f1= _temp2297; _temp2395.f2= _temp2295; _temp2395.f3= _temp2293;
_temp2395.f4= _temp2301; _temp2395.f5= _temp2291; _temp2395;}); _temp2394;}));
_temp2393;}); _temp2392;})); _temp2391->tl= tms; _temp2391;}); if( Cyc_Absynpp_to_VC){
for( 0; _temp2289 != 0; _temp2289=(( struct Cyc_List_List*) _check_null(
_temp2289))->tl){ void* _temp2396=( void*)(( struct Cyc_List_List*) _check_null(
_temp2289))->hd; _LL2398: if( _temp2396 ==( void*) Cyc_Absyn_Stdcall_att){ goto
_LL2399;} else{ goto _LL2400;} _LL2400: if( _temp2396 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL2401;} else{ goto _LL2402;} _LL2402: if( _temp2396 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL2403;} else{ goto _LL2404;} _LL2404: goto _LL2405; _LL2399: goto _LL2401;
_LL2401: goto _LL2403; _LL2403: tms=({ struct Cyc_List_List* _temp2406=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2406->hd=( void*)((
void*)({ struct Cyc_Absyn_Attributes_mod_struct* _temp2407=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2407[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2408; _temp2408.tag= Cyc_Absyn_Attributes_mod;
_temp2408.f1= 0; _temp2408.f2=({ struct Cyc_List_List* _temp2409=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2409->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( _temp2289))->hd); _temp2409->tl= 0;
_temp2409;}); _temp2408;}); _temp2407;})); _temp2406->tl= tms; _temp2406;});
goto AfterAtts; _LL2405: goto _LL2397; _LL2397:;}} AfterAtts: if( _temp2303 != 0){
tms=({ struct Cyc_List_List* _temp2410=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2410->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp2411=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp2411[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp2412; _temp2412.tag= Cyc_Absyn_TypeParams_mod; _temp2412.f1= _temp2303;
_temp2412.f2= 0; _temp2412.f3= 1; _temp2412;}); _temp2411;})); _temp2410->tl=
tms; _temp2410;});} return({ struct _tuple4 _temp2413; _temp2413.f1= _temp2386;
_temp2413.f2= _temp2384; _temp2413.f3= tms; _temp2413;});}} _LL2264: if(
_temp2307 == 0){ return({ struct _tuple4 _temp2414; _temp2414.f1= tq; _temp2414.f2=
t; _temp2414.f3= 0; _temp2414;});} else{ return Cyc_Absynpp_to_tms( tq,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2307))->v);} _LL2266: if( _temp2311 == 0?
1: ! Cyc_Absynpp_expand_typedefs){ return({ struct _tuple4 _temp2415; _temp2415.f1=
tq; _temp2415.f2= t; _temp2415.f3= 0; _temp2415;});} else{ return Cyc_Absynpp_to_tms(
tq,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2311))->v);} _LL2268:
return({ struct _tuple4 _temp2416; _temp2416.f1= tq; _temp2416.f2= t; _temp2416.f3=
0; _temp2416;}); _LL2256:;} struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc( struct Cyc_Absyn_Tqual
tq, void* t, struct Cyc_Core_Opt* dopt){ struct Cyc_List_List* _temp2419; void*
_temp2421; struct Cyc_Absyn_Tqual _temp2423; struct _tuple4 _temp2417= Cyc_Absynpp_to_tms(
tq, t); _LL2424: _temp2423= _temp2417.f1; goto _LL2422; _LL2422: _temp2421=
_temp2417.f2; goto _LL2420; _LL2420: _temp2419= _temp2417.f3; goto _LL2418;
_LL2418: _temp2419= Cyc_List_imp_rev( _temp2419); if( _temp2419 == 0? dopt == 0:
0){ return({ struct Cyc_PP_Doc*(* _temp2425)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2428= Cyc_Absynpp_tqual2doc( _temp2423); struct Cyc_PP_Doc*
_temp2429= Cyc_Absynpp_ntyp2doc( _temp2421); struct Cyc_PP_Doc* _temp2426[ 2u]={
_temp2428, _temp2429}; struct _tagged_arr _temp2427={( void*) _temp2426,( void*)
_temp2426,( void*)( _temp2426 + 2u)}; _temp2425( _temp2427);});} else{ return({
struct Cyc_PP_Doc*(* _temp2430)( struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2433= Cyc_Absynpp_tqual2doc( _temp2423); struct Cyc_PP_Doc* _temp2434= Cyc_Absynpp_ntyp2doc(
_temp2421); struct Cyc_PP_Doc* _temp2435= Cyc_PP_text( _tag_arr(" ", sizeof(
unsigned char), 2u)); struct Cyc_PP_Doc* _temp2436= Cyc_Absynpp_dtms2doc( dopt
== 0? Cyc_PP_nil_doc():( struct Cyc_PP_Doc*)(( struct Cyc_Core_Opt*) _check_null(
dopt))->v, _temp2419); struct Cyc_PP_Doc* _temp2431[ 4u]={ _temp2433, _temp2434,
_temp2435, _temp2436}; struct _tagged_arr _temp2432={( void*) _temp2431,( void*)
_temp2431,( void*)( _temp2431 + 4u)}; _temp2430( _temp2432);});}} struct Cyc_PP_Doc*
Cyc_Absynpp_structfield2doc( struct Cyc_Absyn_Structfield* f){ if( f->width != 0){
return({ struct Cyc_PP_Doc*(* _temp2437)( struct _tagged_arr)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2440= Cyc_Absynpp_tqtd2doc( f->tq,( void*) f->type,({
struct Cyc_Core_Opt* _temp2445=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2445->v=( void*) Cyc_PP_textptr( f->name); _temp2445;}));
struct Cyc_PP_Doc* _temp2441= Cyc_PP_text( _tag_arr(":", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp2442= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( f->width))->v); struct Cyc_PP_Doc* _temp2443=
Cyc_Absynpp_atts2doc( f->attributes); struct Cyc_PP_Doc* _temp2444= Cyc_PP_text(
_tag_arr(";", sizeof( unsigned char), 2u)); struct Cyc_PP_Doc* _temp2438[ 5u]={
_temp2440, _temp2441, _temp2442, _temp2443, _temp2444}; struct _tagged_arr
_temp2439={( void*) _temp2438,( void*) _temp2438,( void*)( _temp2438 + 5u)};
_temp2437( _temp2439);});} else{ return({ struct Cyc_PP_Doc*(* _temp2446)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2449= Cyc_Absynpp_tqtd2doc(
f->tq,( void*) f->type,({ struct Cyc_Core_Opt* _temp2452=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2452->v=( void*) Cyc_PP_textptr(
f->name); _temp2452;})); struct Cyc_PP_Doc* _temp2450= Cyc_Absynpp_atts2doc( f->attributes);
struct Cyc_PP_Doc* _temp2451= Cyc_PP_text( _tag_arr(";", sizeof( unsigned char),
2u)); struct Cyc_PP_Doc* _temp2447[ 3u]={ _temp2449, _temp2450, _temp2451};
struct _tagged_arr _temp2448={( void*) _temp2447,( void*) _temp2447,( void*)(
_temp2447 + 3u)}; _temp2446( _temp2448);});}} struct Cyc_PP_Doc* Cyc_Absynpp_structfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Structfield*), struct _tagged_arr sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_structfield2doc, _tag_arr("", sizeof(
unsigned char), 1u), fields);} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfield2doc(
struct Cyc_Absyn_Tunionfield* f){ return({ struct Cyc_PP_Doc*(* _temp2453)(
struct _tagged_arr)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2456= Cyc_Absynpp_scope2doc((
void*) f->sc); struct Cyc_PP_Doc* _temp2457= Cyc_Absynpp_typedef_name2doc( f->name);
struct Cyc_PP_Doc* _temp2458= f->tvs == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_ktvars2doc(
f->tvs); struct Cyc_PP_Doc* _temp2459= f->typs == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(
f->typs); struct Cyc_PP_Doc* _temp2454[ 4u]={ _temp2456, _temp2457, _temp2458,
_temp2459}; struct _tagged_arr _temp2455={( void*) _temp2454,( void*) _temp2454,(
void*)( _temp2454 + 4u)}; _temp2453( _temp2455);});} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc(
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
sc){ void* _temp2460= sc; _LL2462: if( _temp2460 ==( void*) Cyc_Absyn_Static){
goto _LL2463;} else{ goto _LL2464;} _LL2464: if( _temp2460 ==( void*) Cyc_Absyn_Public){
goto _LL2465;} else{ goto _LL2466;} _LL2466: if( _temp2460 ==( void*) Cyc_Absyn_Extern){
goto _LL2467;} else{ goto _LL2468;} _LL2468: if( _temp2460 ==( void*) Cyc_Absyn_ExternC){
goto _LL2469;} else{ goto _LL2470;} _LL2470: if( _temp2460 ==( void*) Cyc_Absyn_Abstract){
goto _LL2471;} else{ goto _LL2461;} _LL2463: return _tag_arr("static", sizeof(
unsigned char), 7u); _LL2465: return _tag_arr("public", sizeof( unsigned char),
7u); _LL2467: return _tag_arr("extern", sizeof( unsigned char), 7u); _LL2469:
return _tag_arr("extern \"C\"", sizeof( unsigned char), 11u); _LL2471: return
_tag_arr("abstract", sizeof( unsigned char), 9u); _LL2461:;}