 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern int Cyc_Core_intcmp( int, int); extern unsigned char Cyc_Core_InvalidArg[
15u]; struct Cyc_Core_InvalidArg_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern struct _RegionHandle* Cyc_Core_heap_region; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, int); extern
struct _tagged_arr ntCsl_to_ntsl( unsigned char**); extern int system(
unsigned char*); extern int* __errno(); struct Cyc_Stdio___sFILE; extern struct
Cyc_Stdio___sFILE* Cyc_Stdio_stderr; extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*);
extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; }
; extern int Cyc_List_length( struct Cyc_List_List* x); extern unsigned char Cyc_List_List_empty[
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
struct _tagged_arr logical_file; struct _tagged_arr line; int line_no; int col;
} ; extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment;
extern struct Cyc_List_List* Cyc_Position_strings_of_segments( struct Cyc_List_List*);
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern struct Cyc_Position_Error* Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*, struct _tagged_arr); extern unsigned char Cyc_Position_Nocontext[
14u]; extern void Cyc_Position_post_error( struct Cyc_Position_Error*); struct
_tuple1{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual;
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
int tag; void* f1; struct _tagged_arr* f2; } ; static const int Cyc_Absyn_Deref_e=
19; struct Cyc_Absyn_Deref_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_StructMember_e= 20; struct Cyc_Absyn_StructMember_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tagged_arr* f2; } ; static const int
Cyc_Absyn_StructArrow_e= 21; struct Cyc_Absyn_StructArrow_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct _tagged_arr* f2; } ; static const int Cyc_Absyn_Subscript_e=
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
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_arr* f1; struct Cyc_Absyn_Stmt*
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
struct _tagged_arr* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Do_s=
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
attributes; } ; struct Cyc_Absyn_Structfield{ struct _tagged_arr* name; struct
Cyc_Absyn_Tqual tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List*
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
int tag; struct _tagged_arr* f1; struct Cyc_List_List* f2; } ; static const int
Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{ int tag; struct _tuple1*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d= 11;
struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ; struct
Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static const int
Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int tag; struct
Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1; struct Cyc_Absyn_FieldName_struct{
int tag; struct _tagged_arr* f1; } ; extern unsigned char Cyc_Absyn_EmptyAnnot[
15u]; extern int Cyc_Absyn_qvar_cmp( struct _tuple1*, struct _tuple1*); extern
int Cyc_Absyn_tvar_cmp( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*); extern
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref( void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern void*
Cyc_Absyn_new_evar( struct Cyc_Core_Opt* k, struct Cyc_Core_Opt* tenv); extern
void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulonglong_t; extern void* Cyc_Absyn_sint_t;
extern void* Cyc_Absyn_slonglong_t; extern void* Cyc_Absyn_float_t; extern void*
Cyc_Absyn_double_t; extern void* Cyc_Absyn_bounds_one; extern void* Cyc_Absyn_atb_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq, void* b); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp(
int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_field( struct Cyc_List_List*, struct _tagged_arr*); extern
struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field( struct Cyc_Absyn_Structdecl*,
struct _tagged_arr*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_union_field(
struct Cyc_Absyn_Uniondecl*, struct _tagged_arr*); struct _tuple4{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; extern struct _tuple4* Cyc_Absyn_lookup_tuple_field( struct
Cyc_List_List*, int); extern struct _tagged_arr Cyc_Absyn_attribute2string( void*);
extern int Cyc_Absyn_fntype_att( void* a); struct Cyc_PP_Ppstate; struct Cyc_PP_Out;
struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int
qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int
rewrite_temp_tvars: 1; int print_all_tvars: 1; int print_all_kinds: 1; int
print_using_stmts: 1; int print_externC_stmts: 1; int print_full_evars: 1; int
use_curr_namespace: 1; struct Cyc_List_List* curr_namespace; } ; extern struct
_tagged_arr Cyc_Absynpp_typ2string( void*); extern struct _tagged_arr Cyc_Absynpp_kind2string(
void*); extern struct _tagged_arr Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref*);
extern struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple1*); struct Cyc_Set_Set;
extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int tag;
void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
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
struct Cyc_Position_Segment*, struct _tuple1*); extern int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv*, void* r1, void* r2); extern int Cyc_String_strcmp(
struct _tagged_arr s1, struct _tagged_arr s2); extern int Cyc_String_zstrptrcmp(
struct _tagged_arr*, struct _tagged_arr*); extern struct _tagged_arr Cyc_String_strconcat(
struct _tagged_arr, struct _tagged_arr); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern
void* Cyc_Tcutil_impos( struct _tagged_arr); extern void Cyc_Tcutil_terr( struct
Cyc_Position_Segment*, struct _tagged_arr); extern void Cyc_Tcutil_warn( struct
Cyc_Position_Segment*, struct _tagged_arr); extern void Cyc_Tcutil_flush_warnings();
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
struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc, struct _tagged_arr
err_msg); extern void Cyc_Tcutil_check_unique_tvars( struct Cyc_Position_Segment*,
struct Cyc_List_List*); extern void Cyc_Tcutil_check_nonzero_bound( struct Cyc_Position_Segment*,
struct Cyc_Absyn_Conref*); extern void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment*,
unsigned int i, struct Cyc_Absyn_Conref*); extern int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2); extern struct Cyc_List_List*
Cyc_Tcutil_resolve_struct_designators( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields); extern int Cyc_Tcutil_is_tagged_pointer_typ(
void*); extern int Cyc_Tcutil_is_tagged_pointer_typ_elt( void* t, void**
elt_typ_dest); extern void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv*,
void* t, struct Cyc_Absyn_Exp* e); struct _tuple7{ int f1; void* f2; } ; extern
struct _tuple7 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
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
loc, struct _tagged_arr s){ Cyc_Position_post_error( Cyc_Position_mk_err_elab(
loc, s));} void* Cyc_Tcutil_impos( struct _tagged_arr msg){({ struct _tagged_arr
_temp0= msg; fprintf( _sfile_to_file( Cyc_Stdio_stderr),"Error: %.*s\n",
_get_arr_size( _temp0, 1u), _temp0.curr);}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr);( int) _throw(( void*) Cyc_Tcutil_TypeErr);} static struct
_tagged_arr Cyc_Tcutil_tvar2string( struct Cyc_Absyn_Tvar* tv){ return* tv->name;}
void Cyc_Tcutil_print_tvars( struct Cyc_List_List* tvs){ for( 0; tvs != 0; tvs=((
struct Cyc_List_List*) _check_null( tvs))->tl){({ struct _tagged_arr _temp1= Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd); struct
_tagged_arr _temp2= Cyc_Absynpp_ckind2string((( struct Cyc_Absyn_Tvar*)(( struct
Cyc_List_List*) _check_null( tvs))->hd)->kind); fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%.*s::%.*s ",
_get_arr_size( _temp1, 1u), _temp1.curr, _get_arr_size( _temp2, 1u), _temp2.curr);});}
fprintf( _sfile_to_file( Cyc_Stdio_stderr),"\n"); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr);} static struct Cyc_List_List* Cyc_Tcutil_warning_segs= 0;
static struct Cyc_List_List* Cyc_Tcutil_warning_msgs= 0; void Cyc_Tcutil_warn(
struct Cyc_Position_Segment* sg, struct _tagged_arr msg){ Cyc_Tcutil_warning_segs=({
struct Cyc_List_List* _temp3=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp3->hd=( void*) sg; _temp3->tl= Cyc_Tcutil_warning_segs;
_temp3;}); Cyc_Tcutil_warning_msgs=({ struct Cyc_List_List* _temp4=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4->hd=( void*)({ struct
_tagged_arr* _temp5=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)
* 1); _temp5[ 0]= msg; _temp5;}); _temp4->tl= Cyc_Tcutil_warning_msgs; _temp4;});}
void Cyc_Tcutil_flush_warnings(){ if( Cyc_Tcutil_warning_segs == 0){ return;}
fprintf( _sfile_to_file( Cyc_Stdio_stderr),"***Warnings***\n");{ struct Cyc_List_List*
_temp6= Cyc_Position_strings_of_segments( Cyc_Tcutil_warning_segs); Cyc_Tcutil_warning_segs=
0; Cyc_Tcutil_warning_msgs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( Cyc_Tcutil_warning_msgs); while( Cyc_Tcutil_warning_msgs != 0) {({
struct _tagged_arr _temp7=( struct _tagged_arr)*(( struct _tagged_arr*)(( struct
Cyc_List_List*) _check_null( _temp6))->hd); struct _tagged_arr _temp8=*(( struct
_tagged_arr*)(( struct Cyc_List_List*) _check_null( Cyc_Tcutil_warning_msgs))->hd);
fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%.*s: %.*s\n", _get_arr_size( _temp7,
1u), _temp7.curr, _get_arr_size( _temp8, 1u), _temp8.curr);}); _temp6=(( struct
Cyc_List_List*) _check_null( _temp6))->tl; Cyc_Tcutil_warning_msgs=(( struct Cyc_List_List*)
_check_null( Cyc_Tcutil_warning_msgs))->tl;} fprintf( _sfile_to_file( Cyc_Stdio_stderr),"**************\n");
Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);}} struct Cyc_Core_Opt*
Cyc_Tcutil_empty_var_set= 0; static int Cyc_Tcutil_fast_tvar_cmp( struct Cyc_Absyn_Tvar*
tv1, struct Cyc_Absyn_Tvar* tv2){ return*(( int*) _check_null( tv1->identity)) -*((
int*) _check_null( tv2->identity));} void* Cyc_Tcutil_compress( void* t){ void*
_temp9= t; struct Cyc_Core_Opt* _temp21; int _temp23; struct Cyc_Core_Opt*
_temp25; struct Cyc_Core_Opt* _temp27; struct Cyc_Core_Opt* _temp29; int _temp31;
struct Cyc_Core_Opt* _temp33; struct Cyc_Core_Opt** _temp35; struct Cyc_Core_Opt*
_temp36; struct Cyc_Core_Opt* _temp38; struct Cyc_List_List* _temp40; struct
_tuple1* _temp42; struct Cyc_Core_Opt* _temp44; struct Cyc_Core_Opt** _temp46;
struct Cyc_List_List* _temp47; struct _tuple1* _temp49; _LL11: if(( unsigned int)
_temp9 > 4u?*(( int*) _temp9) == Cyc_Absyn_Evar: 0){ _LL28: _temp27=(( struct
Cyc_Absyn_Evar_struct*) _temp9)->f1; goto _LL26; _LL26: _temp25=(( struct Cyc_Absyn_Evar_struct*)
_temp9)->f2; if( _temp25 == 0){ goto _LL24;} else{ goto _LL13;} _LL24: _temp23=((
struct Cyc_Absyn_Evar_struct*) _temp9)->f3; goto _LL22; _LL22: _temp21=(( struct
Cyc_Absyn_Evar_struct*) _temp9)->f4; goto _LL12;} else{ goto _LL13;} _LL13: if((
unsigned int) _temp9 > 4u?*(( int*) _temp9) == Cyc_Absyn_Evar: 0){ _LL37:
_temp36=(( struct Cyc_Absyn_Evar_struct*) _temp9)->f1; goto _LL34; _LL34:
_temp33=(( struct Cyc_Absyn_Evar_struct*) _temp9)->f2; _temp35=&(( struct Cyc_Absyn_Evar_struct*)
_temp9)->f2; goto _LL32; _LL32: _temp31=(( struct Cyc_Absyn_Evar_struct*) _temp9)->f3;
goto _LL30; _LL30: _temp29=(( struct Cyc_Absyn_Evar_struct*) _temp9)->f4; goto
_LL14;} else{ goto _LL15;} _LL15: if(( unsigned int) _temp9 > 4u?*(( int*)
_temp9) == Cyc_Absyn_TypedefType: 0){ _LL43: _temp42=(( struct Cyc_Absyn_TypedefType_struct*)
_temp9)->f1; goto _LL41; _LL41: _temp40=(( struct Cyc_Absyn_TypedefType_struct*)
_temp9)->f2; goto _LL39; _LL39: _temp38=(( struct Cyc_Absyn_TypedefType_struct*)
_temp9)->f3; if( _temp38 == 0){ goto _LL16;} else{ goto _LL17;}} else{ goto
_LL17;} _LL17: if(( unsigned int) _temp9 > 4u?*(( int*) _temp9) == Cyc_Absyn_TypedefType:
0){ _LL50: _temp49=(( struct Cyc_Absyn_TypedefType_struct*) _temp9)->f1; goto
_LL48; _LL48: _temp47=(( struct Cyc_Absyn_TypedefType_struct*) _temp9)->f2; goto
_LL45; _LL45: _temp44=(( struct Cyc_Absyn_TypedefType_struct*) _temp9)->f3;
_temp46=&(( struct Cyc_Absyn_TypedefType_struct*) _temp9)->f3; goto _LL18;}
else{ goto _LL19;} _LL19: goto _LL20; _LL12: return t; _LL14: { void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null(* _temp35))->v); if( t2 !=( void*)((
struct Cyc_Core_Opt*) _check_null(* _temp35))->v){* _temp35=({ struct Cyc_Core_Opt*
_temp51=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp51->v=( void*) t2; _temp51;});} return t2;} _LL16: return t; _LL18: { void*
t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(* _temp46))->v);
if( t2 !=( void*)(( struct Cyc_Core_Opt*) _check_null(* _temp46))->v){* _temp46=({
struct Cyc_Core_Opt* _temp52=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp52->v=( void*) t2; _temp52;});} return t2;} _LL20: return t;
_LL10:;} void* Cyc_Tcutil_copy_type( void* t); static struct Cyc_List_List* Cyc_Tcutil_copy_types(
struct Cyc_List_List* ts){ return(( struct Cyc_List_List*(*)( void*(* f)( void*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_type, ts);} static
struct Cyc_Absyn_Conref* Cyc_Tcutil_copy_conref( struct Cyc_Absyn_Conref* c){
void* _temp53=( void*) c->v; void* _temp61; struct Cyc_Absyn_Conref* _temp63;
_LL55: if( _temp53 ==( void*) Cyc_Absyn_No_constr){ goto _LL56;} else{ goto
_LL57;} _LL57: if(( unsigned int) _temp53 > 1u?*(( int*) _temp53) == Cyc_Absyn_Eq_constr:
0){ _LL62: _temp61=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp53)->f1;
goto _LL58;} else{ goto _LL59;} _LL59: if(( unsigned int) _temp53 > 1u?*(( int*)
_temp53) == Cyc_Absyn_Forward_constr: 0){ _LL64: _temp63=(( struct Cyc_Absyn_Forward_constr_struct*)
_temp53)->f1; goto _LL60;} else{ goto _LL54;} _LL56: return(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _LL58: return(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)( _temp61); _LL60: return(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* c)) Cyc_Tcutil_copy_conref)( _temp63); _LL54:;} static
struct Cyc_Absyn_Tvar* Cyc_Tcutil_copy_tvar( struct Cyc_Absyn_Tvar* tv){ return({
struct Cyc_Absyn_Tvar* _temp65=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof(
struct Cyc_Absyn_Tvar)); _temp65->name= tv->name; _temp65->identity= 0; _temp65->kind=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* c)) Cyc_Tcutil_copy_conref)(
tv->kind); _temp65;});} static struct _tuple2* Cyc_Tcutil_copy_arg( struct
_tuple2* arg){ void* _temp68; struct Cyc_Absyn_Tqual _temp70; struct Cyc_Core_Opt*
_temp72; struct _tuple2 _temp66=* arg; _LL73: _temp72= _temp66.f1; goto _LL71;
_LL71: _temp70= _temp66.f2; goto _LL69; _LL69: _temp68= _temp66.f3; goto _LL67;
_LL67: return({ struct _tuple2* _temp74=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp74->f1= _temp72; _temp74->f2= _temp70; _temp74->f3= Cyc_Tcutil_copy_type(
_temp68); _temp74;});} static struct _tuple4* Cyc_Tcutil_copy_tqt( struct
_tuple4* arg){ void* _temp77; struct Cyc_Absyn_Tqual _temp79; struct _tuple4
_temp75=* arg; _LL80: _temp79= _temp75.f1; goto _LL78; _LL78: _temp77= _temp75.f2;
goto _LL76; _LL76: return({ struct _tuple4* _temp81=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp81->f1= _temp79; _temp81->f2= Cyc_Tcutil_copy_type(
_temp77); _temp81;});} static struct Cyc_Absyn_Structfield* Cyc_Tcutil_copy_field(
struct Cyc_Absyn_Structfield* f){ return({ struct Cyc_Absyn_Structfield* _temp82=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp82->name= f->name; _temp82->tq= f->tq; _temp82->type=( void*) Cyc_Tcutil_copy_type((
void*) f->type); _temp82->width= f->width; _temp82->attributes= f->attributes;
_temp82;});} struct _tuple8{ void* f1; void* f2; } ; static struct _tuple8* Cyc_Tcutil_copy_rgncmp(
struct _tuple8* x){ struct _tuple8 _temp85; void* _temp86; void* _temp88; struct
_tuple8* _temp83= x; _temp85=* _temp83; _LL89: _temp88= _temp85.f1; goto _LL87;
_LL87: _temp86= _temp85.f2; goto _LL84; _LL84: return({ struct _tuple8* _temp90=(
struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp90->f1= Cyc_Tcutil_copy_type(
_temp88); _temp90->f2= Cyc_Tcutil_copy_type( _temp86); _temp90;});} void* Cyc_Tcutil_copy_type(
void* t){ void* _temp91= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt* _temp137;
int _temp139; struct Cyc_Core_Opt* _temp141; struct Cyc_Core_Opt* _temp143;
struct Cyc_Absyn_Tvar* _temp145; struct Cyc_Absyn_TunionInfo _temp147; void*
_temp149; struct Cyc_List_List* _temp151; void* _temp153; struct Cyc_Absyn_TunionFieldInfo
_temp155; struct Cyc_List_List* _temp157; void* _temp159; struct Cyc_Absyn_PtrInfo
_temp161; struct Cyc_Absyn_Conref* _temp163; struct Cyc_Absyn_Tqual _temp165;
struct Cyc_Absyn_Conref* _temp167; void* _temp169; void* _temp171; void*
_temp173; void* _temp175; struct Cyc_Absyn_Exp* _temp177; struct Cyc_Absyn_Tqual
_temp179; void* _temp181; struct Cyc_Absyn_FnInfo _temp183; struct Cyc_List_List*
_temp185; struct Cyc_List_List* _temp187; struct Cyc_Absyn_VarargInfo* _temp189;
int _temp191; struct Cyc_List_List* _temp193; void* _temp195; struct Cyc_Core_Opt*
_temp197; struct Cyc_List_List* _temp199; struct Cyc_List_List* _temp201; struct
Cyc_Absyn_Structdecl** _temp203; struct Cyc_List_List* _temp205; struct _tuple1*
_temp207; struct Cyc_Absyn_Uniondecl** _temp209; struct Cyc_List_List* _temp211;
struct _tuple1* _temp213; struct Cyc_List_List* _temp215; struct Cyc_List_List*
_temp217; struct Cyc_Absyn_Enumdecl* _temp219; struct _tuple1* _temp221; void*
_temp223; struct Cyc_Core_Opt* _temp225; struct Cyc_List_List* _temp227; struct
_tuple1* _temp229; void* _temp231; struct Cyc_List_List* _temp233; _LL93: if(
_temp91 ==( void*) Cyc_Absyn_VoidType){ goto _LL94;} else{ goto _LL95;} _LL95:
if(( unsigned int) _temp91 > 4u?*(( int*) _temp91) == Cyc_Absyn_Evar: 0){ _LL144:
_temp143=(( struct Cyc_Absyn_Evar_struct*) _temp91)->f1; goto _LL142; _LL142:
_temp141=(( struct Cyc_Absyn_Evar_struct*) _temp91)->f2; goto _LL140; _LL140:
_temp139=(( struct Cyc_Absyn_Evar_struct*) _temp91)->f3; goto _LL138; _LL138:
_temp137=(( struct Cyc_Absyn_Evar_struct*) _temp91)->f4; goto _LL96;} else{ goto
_LL97;} _LL97: if(( unsigned int) _temp91 > 4u?*(( int*) _temp91) == Cyc_Absyn_VarType:
0){ _LL146: _temp145=(( struct Cyc_Absyn_VarType_struct*) _temp91)->f1; goto
_LL98;} else{ goto _LL99;} _LL99: if(( unsigned int) _temp91 > 4u?*(( int*)
_temp91) == Cyc_Absyn_TunionType: 0){ _LL148: _temp147=(( struct Cyc_Absyn_TunionType_struct*)
_temp91)->f1; _LL154: _temp153=( void*) _temp147.tunion_info; goto _LL152;
_LL152: _temp151= _temp147.targs; goto _LL150; _LL150: _temp149=( void*)
_temp147.rgn; goto _LL100;} else{ goto _LL101;} _LL101: if(( unsigned int)
_temp91 > 4u?*(( int*) _temp91) == Cyc_Absyn_TunionFieldType: 0){ _LL156:
_temp155=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp91)->f1; _LL160:
_temp159=( void*) _temp155.field_info; goto _LL158; _LL158: _temp157= _temp155.targs;
goto _LL102;} else{ goto _LL103;} _LL103: if(( unsigned int) _temp91 > 4u?*((
int*) _temp91) == Cyc_Absyn_PointerType: 0){ _LL162: _temp161=(( struct Cyc_Absyn_PointerType_struct*)
_temp91)->f1; _LL172: _temp171=( void*) _temp161.elt_typ; goto _LL170; _LL170:
_temp169=( void*) _temp161.rgn_typ; goto _LL168; _LL168: _temp167= _temp161.nullable;
goto _LL166; _LL166: _temp165= _temp161.tq; goto _LL164; _LL164: _temp163=
_temp161.bounds; goto _LL104;} else{ goto _LL105;} _LL105: if(( unsigned int)
_temp91 > 4u?*(( int*) _temp91) == Cyc_Absyn_IntType: 0){ _LL176: _temp175=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp91)->f1; goto _LL174; _LL174:
_temp173=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp91)->f2; goto _LL106;}
else{ goto _LL107;} _LL107: if( _temp91 ==( void*) Cyc_Absyn_FloatType){ goto
_LL108;} else{ goto _LL109;} _LL109: if( _temp91 ==( void*) Cyc_Absyn_DoubleType){
goto _LL110;} else{ goto _LL111;} _LL111: if(( unsigned int) _temp91 > 4u?*((
int*) _temp91) == Cyc_Absyn_ArrayType: 0){ _LL182: _temp181=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp91)->f1; goto _LL180; _LL180: _temp179=((
struct Cyc_Absyn_ArrayType_struct*) _temp91)->f2; goto _LL178; _LL178: _temp177=((
struct Cyc_Absyn_ArrayType_struct*) _temp91)->f3; goto _LL112;} else{ goto
_LL113;} _LL113: if(( unsigned int) _temp91 > 4u?*(( int*) _temp91) == Cyc_Absyn_FnType:
0){ _LL184: _temp183=(( struct Cyc_Absyn_FnType_struct*) _temp91)->f1; _LL200:
_temp199= _temp183.tvars; goto _LL198; _LL198: _temp197= _temp183.effect; goto
_LL196; _LL196: _temp195=( void*) _temp183.ret_typ; goto _LL194; _LL194:
_temp193= _temp183.args; goto _LL192; _LL192: _temp191= _temp183.c_varargs; goto
_LL190; _LL190: _temp189= _temp183.cyc_varargs; goto _LL188; _LL188: _temp187=
_temp183.rgn_po; goto _LL186; _LL186: _temp185= _temp183.attributes; goto _LL114;}
else{ goto _LL115;} _LL115: if(( unsigned int) _temp91 > 4u?*(( int*) _temp91)
== Cyc_Absyn_TupleType: 0){ _LL202: _temp201=(( struct Cyc_Absyn_TupleType_struct*)
_temp91)->f1; goto _LL116;} else{ goto _LL117;} _LL117: if(( unsigned int)
_temp91 > 4u?*(( int*) _temp91) == Cyc_Absyn_StructType: 0){ _LL208: _temp207=((
struct Cyc_Absyn_StructType_struct*) _temp91)->f1; goto _LL206; _LL206: _temp205=((
struct Cyc_Absyn_StructType_struct*) _temp91)->f2; goto _LL204; _LL204: _temp203=((
struct Cyc_Absyn_StructType_struct*) _temp91)->f3; goto _LL118;} else{ goto
_LL119;} _LL119: if(( unsigned int) _temp91 > 4u?*(( int*) _temp91) == Cyc_Absyn_UnionType:
0){ _LL214: _temp213=(( struct Cyc_Absyn_UnionType_struct*) _temp91)->f1; goto
_LL212; _LL212: _temp211=(( struct Cyc_Absyn_UnionType_struct*) _temp91)->f2;
goto _LL210; _LL210: _temp209=(( struct Cyc_Absyn_UnionType_struct*) _temp91)->f3;
goto _LL120;} else{ goto _LL121;} _LL121: if(( unsigned int) _temp91 > 4u?*((
int*) _temp91) == Cyc_Absyn_AnonStructType: 0){ _LL216: _temp215=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp91)->f1; goto _LL122;} else{ goto _LL123;} _LL123: if(( unsigned int)
_temp91 > 4u?*(( int*) _temp91) == Cyc_Absyn_AnonUnionType: 0){ _LL218: _temp217=((
struct Cyc_Absyn_AnonUnionType_struct*) _temp91)->f1; goto _LL124;} else{ goto
_LL125;} _LL125: if(( unsigned int) _temp91 > 4u?*(( int*) _temp91) == Cyc_Absyn_EnumType:
0){ _LL222: _temp221=(( struct Cyc_Absyn_EnumType_struct*) _temp91)->f1; goto
_LL220; _LL220: _temp219=(( struct Cyc_Absyn_EnumType_struct*) _temp91)->f2;
goto _LL126;} else{ goto _LL127;} _LL127: if(( unsigned int) _temp91 > 4u?*((
int*) _temp91) == Cyc_Absyn_RgnHandleType: 0){ _LL224: _temp223=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp91)->f1; goto _LL128;} else{ goto
_LL129;} _LL129: if(( unsigned int) _temp91 > 4u?*(( int*) _temp91) == Cyc_Absyn_TypedefType:
0){ _LL230: _temp229=(( struct Cyc_Absyn_TypedefType_struct*) _temp91)->f1; goto
_LL228; _LL228: _temp227=(( struct Cyc_Absyn_TypedefType_struct*) _temp91)->f2;
goto _LL226; _LL226: _temp225=(( struct Cyc_Absyn_TypedefType_struct*) _temp91)->f3;
goto _LL130;} else{ goto _LL131;} _LL131: if( _temp91 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL132;} else{ goto _LL133;} _LL133: if(( unsigned int) _temp91 > 4u?*((
int*) _temp91) == Cyc_Absyn_AccessEff: 0){ _LL232: _temp231=( void*)(( struct
Cyc_Absyn_AccessEff_struct*) _temp91)->f1; goto _LL134;} else{ goto _LL135;}
_LL135: if(( unsigned int) _temp91 > 4u?*(( int*) _temp91) == Cyc_Absyn_JoinEff:
0){ _LL234: _temp233=(( struct Cyc_Absyn_JoinEff_struct*) _temp91)->f1; goto
_LL136;} else{ goto _LL92;} _LL94: return t; _LL96: return t; _LL98: return(
void*)({ struct Cyc_Absyn_VarType_struct* _temp235=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp235[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp236; _temp236.tag= Cyc_Absyn_VarType; _temp236.f1= Cyc_Tcutil_copy_tvar(
_temp145); _temp236;}); _temp235;}); _LL100: return( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp237=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp237[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp238; _temp238.tag= Cyc_Absyn_TunionType;
_temp238.f1=({ struct Cyc_Absyn_TunionInfo _temp239; _temp239.tunion_info=( void*)
_temp153; _temp239.targs= Cyc_Tcutil_copy_types( _temp151); _temp239.rgn=( void*)
Cyc_Tcutil_copy_type( _temp149); _temp239;}); _temp238;}); _temp237;}); _LL102:
return( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp240=( struct Cyc_Absyn_TunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp240[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp241; _temp241.tag= Cyc_Absyn_TunionFieldType;
_temp241.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp242; _temp242.field_info=(
void*) _temp159; _temp242.targs= Cyc_Tcutil_copy_types( _temp157); _temp242;});
_temp241;}); _temp240;}); _LL104: { void* _temp243= Cyc_Tcutil_copy_type(
_temp171); void* _temp244= Cyc_Tcutil_copy_type( _temp169); struct Cyc_Absyn_Conref*
_temp245=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* c)) Cyc_Tcutil_copy_conref)(
_temp167); struct Cyc_Absyn_Tqual _temp246= _temp165; struct Cyc_Absyn_Conref*
_temp247=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* c)) Cyc_Tcutil_copy_conref)(
_temp163); return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp248=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp248[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp249; _temp249.tag= Cyc_Absyn_PointerType;
_temp249.f1=({ struct Cyc_Absyn_PtrInfo _temp250; _temp250.elt_typ=( void*)
_temp243; _temp250.rgn_typ=( void*) _temp244; _temp250.nullable= _temp245;
_temp250.tq= _temp246; _temp250.bounds= _temp247; _temp250;}); _temp249;});
_temp248;});} _LL106: return( void*)({ struct Cyc_Absyn_IntType_struct* _temp251=(
struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp251[ 0]=({ struct Cyc_Absyn_IntType_struct _temp252; _temp252.tag= Cyc_Absyn_IntType;
_temp252.f1=( void*) _temp175; _temp252.f2=( void*) _temp173; _temp252;});
_temp251;}); _LL108: goto _LL110; _LL110: return t; _LL112: return( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp253=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp253[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp254; _temp254.tag= Cyc_Absyn_ArrayType; _temp254.f1=(
void*) Cyc_Tcutil_copy_type( _temp181); _temp254.f2= _temp179; _temp254.f3=
_temp177; _temp254;}); _temp253;}); _LL114: { struct Cyc_List_List* _temp255=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_tvar, _temp199); struct
Cyc_Core_Opt* _temp256= _temp197 == 0? 0:({ struct Cyc_Core_Opt* _temp266=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp266->v=(
void*) Cyc_Tcutil_copy_type(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp197))->v); _temp266;}); void* _temp257= Cyc_Tcutil_copy_type( _temp195);
struct Cyc_List_List* _temp258=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)(
struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_arg,
_temp193); int _temp259= _temp191; struct Cyc_Absyn_VarargInfo* cyc_varargs2= 0;
if( _temp189 != 0){ struct Cyc_Absyn_VarargInfo* cv=( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp189); cyc_varargs2=({ struct Cyc_Absyn_VarargInfo* _temp260=(
struct Cyc_Absyn_VarargInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp260->name= cv->name; _temp260->tq= cv->tq; _temp260->type=( void*) Cyc_Tcutil_copy_type((
void*) cv->type); _temp260->rgn=( void*) Cyc_Tcutil_copy_type(( void*) cv->rgn);
_temp260->inject= cv->inject; _temp260;});}{ struct Cyc_List_List* _temp261=((
struct Cyc_List_List*(*)( struct _tuple8*(* f)( struct _tuple8*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_copy_rgncmp, _temp187); struct Cyc_List_List*
_temp262= _temp185; return( void*)({ struct Cyc_Absyn_FnType_struct* _temp263=(
struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp263[ 0]=({ struct Cyc_Absyn_FnType_struct _temp264; _temp264.tag= Cyc_Absyn_FnType;
_temp264.f1=({ struct Cyc_Absyn_FnInfo _temp265; _temp265.tvars= _temp255;
_temp265.effect= _temp256; _temp265.ret_typ=( void*) _temp257; _temp265.args=
_temp258; _temp265.c_varargs= _temp259; _temp265.cyc_varargs= cyc_varargs2;
_temp265.rgn_po= _temp261; _temp265.attributes= _temp262; _temp265;}); _temp264;});
_temp263;});}} _LL116: return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp267=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp267[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp268; _temp268.tag= Cyc_Absyn_TupleType;
_temp268.f1=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_tqt, _temp201);
_temp268;}); _temp267;}); _LL118: return( void*)({ struct Cyc_Absyn_StructType_struct*
_temp269=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp269[ 0]=({ struct Cyc_Absyn_StructType_struct _temp270; _temp270.tag= Cyc_Absyn_StructType;
_temp270.f1= _temp207; _temp270.f2= Cyc_Tcutil_copy_types( _temp205); _temp270.f3=
0; _temp270;}); _temp269;}); _LL120: return( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp271=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp271[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp272; _temp272.tag= Cyc_Absyn_UnionType;
_temp272.f1= _temp213; _temp272.f2= Cyc_Tcutil_copy_types( _temp211); _temp272.f3=
0; _temp272;}); _temp271;}); _LL122: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp273=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp273[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp274; _temp274.tag= Cyc_Absyn_AnonStructType; _temp274.f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_copy_field, _temp215); _temp274;}); _temp273;});
_LL124: return( void*)({ struct Cyc_Absyn_AnonUnionType_struct* _temp275=(
struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonUnionType_struct));
_temp275[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct _temp276; _temp276.tag=
Cyc_Absyn_AnonUnionType; _temp276.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_copy_field,
_temp217); _temp276;}); _temp275;}); _LL126: return( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp277=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp277[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp278; _temp278.tag= Cyc_Absyn_EnumType;
_temp278.f1= _temp221; _temp278.f2= 0; _temp278;}); _temp277;}); _LL128: return(
void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp279=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp279[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp280; _temp280.tag= Cyc_Absyn_RgnHandleType;
_temp280.f1=( void*) Cyc_Tcutil_copy_type( _temp223); _temp280;}); _temp279;});
_LL130: return( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp281=( struct
Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp281[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp282; _temp282.tag= Cyc_Absyn_TypedefType;
_temp282.f1= _temp229; _temp282.f2= Cyc_Tcutil_copy_types( _temp227); _temp282.f3=
0; _temp282;}); _temp281;}); _LL132: return t; _LL134: return( void*)({ struct
Cyc_Absyn_AccessEff_struct* _temp283=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp283[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp284; _temp284.tag= Cyc_Absyn_AccessEff; _temp284.f1=(
void*) Cyc_Tcutil_copy_type( _temp231); _temp284;}); _temp283;}); _LL136: return(
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp285=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp285[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp286; _temp286.tag= Cyc_Absyn_JoinEff; _temp286.f1= Cyc_Tcutil_copy_types(
_temp233); _temp286;}); _temp285;}); _LL92:;} static int Cyc_Tcutil_kind_leq(
void* k1, void* k2){ if( k1 == k2){ return 1;}{ struct _tuple8 _temp288=({
struct _tuple8 _temp287; _temp287.f1= k1; _temp287.f2= k2; _temp287;}); void*
_temp298; void* _temp300; void* _temp302; void* _temp304; void* _temp306; void*
_temp308; _LL290: _LL301: _temp300= _temp288.f1; if( _temp300 ==( void*) Cyc_Absyn_BoxKind){
goto _LL299;} else{ goto _LL292;} _LL299: _temp298= _temp288.f2; if( _temp298 ==(
void*) Cyc_Absyn_MemKind){ goto _LL291;} else{ goto _LL292;} _LL292: _LL305:
_temp304= _temp288.f1; if( _temp304 ==( void*) Cyc_Absyn_BoxKind){ goto _LL303;}
else{ goto _LL294;} _LL303: _temp302= _temp288.f2; if( _temp302 ==( void*) Cyc_Absyn_AnyKind){
goto _LL293;} else{ goto _LL294;} _LL294: _LL309: _temp308= _temp288.f1; if(
_temp308 ==( void*) Cyc_Absyn_MemKind){ goto _LL307;} else{ goto _LL296;} _LL307:
_temp306= _temp288.f2; if( _temp306 ==( void*) Cyc_Absyn_AnyKind){ goto _LL295;}
else{ goto _LL296;} _LL296: goto _LL297; _LL291: return 1; _LL293: return 1;
_LL295: return 1; _LL297: return 0; _LL289:;}} void* Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar* tv){ return(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
tv->kind);} void* Cyc_Tcutil_typ_kind( void* t){ void* _temp310= Cyc_Tcutil_compress(
t); struct Cyc_Core_Opt* _temp362; int _temp364; struct Cyc_Core_Opt* _temp366;
struct Cyc_Core_Opt* _temp368; struct Cyc_Absyn_Tvar* _temp370; void* _temp372;
void* _temp374; struct Cyc_Absyn_FnInfo _temp376; void* _temp378; struct Cyc_Absyn_TunionInfo
_temp380; struct Cyc_Absyn_TunionFieldInfo _temp382; struct Cyc_List_List*
_temp384; void* _temp386; struct Cyc_Absyn_Tunionfield* _temp388; struct Cyc_Absyn_Tuniondecl*
_temp390; struct Cyc_Absyn_TunionFieldInfo _temp392; struct Cyc_List_List*
_temp394; void* _temp396; struct Cyc_Absyn_UnknownTunionFieldInfo _temp398;
struct Cyc_Absyn_Structdecl** _temp400; struct Cyc_List_List* _temp402; struct
_tuple1* _temp404; struct Cyc_Absyn_Uniondecl** _temp406; struct Cyc_List_List*
_temp408; struct _tuple1* _temp410; struct Cyc_Absyn_Structdecl** _temp412;
struct Cyc_List_List* _temp414; struct _tuple1* _temp416; struct Cyc_Absyn_Uniondecl**
_temp418; struct Cyc_List_List* _temp420; struct _tuple1* _temp422; struct Cyc_List_List*
_temp424; struct Cyc_List_List* _temp426; struct Cyc_Absyn_Enumdecl* _temp428;
struct _tuple1* _temp430; struct Cyc_Absyn_PtrInfo _temp432; struct Cyc_Absyn_Exp*
_temp434; struct Cyc_Absyn_Tqual _temp436; void* _temp438; struct Cyc_List_List*
_temp440; struct Cyc_Core_Opt* _temp442; struct Cyc_List_List* _temp444; struct
_tuple1* _temp446; void* _temp448; struct Cyc_List_List* _temp450; _LL312: if((
unsigned int) _temp310 > 4u?*(( int*) _temp310) == Cyc_Absyn_Evar: 0){ _LL369:
_temp368=(( struct Cyc_Absyn_Evar_struct*) _temp310)->f1; goto _LL367; _LL367:
_temp366=(( struct Cyc_Absyn_Evar_struct*) _temp310)->f2; goto _LL365; _LL365:
_temp364=(( struct Cyc_Absyn_Evar_struct*) _temp310)->f3; goto _LL363; _LL363:
_temp362=(( struct Cyc_Absyn_Evar_struct*) _temp310)->f4; goto _LL313;} else{
goto _LL314;} _LL314: if(( unsigned int) _temp310 > 4u?*(( int*) _temp310) ==
Cyc_Absyn_VarType: 0){ _LL371: _temp370=(( struct Cyc_Absyn_VarType_struct*)
_temp310)->f1; goto _LL315;} else{ goto _LL316;} _LL316: if( _temp310 ==( void*)
Cyc_Absyn_VoidType){ goto _LL317;} else{ goto _LL318;} _LL318: if(( unsigned int)
_temp310 > 4u?*(( int*) _temp310) == Cyc_Absyn_IntType: 0){ _LL375: _temp374=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp310)->f1; goto _LL373; _LL373:
_temp372=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp310)->f2; goto _LL319;}
else{ goto _LL320;} _LL320: if( _temp310 ==( void*) Cyc_Absyn_FloatType){ goto
_LL321;} else{ goto _LL322;} _LL322: if( _temp310 ==( void*) Cyc_Absyn_DoubleType){
goto _LL323;} else{ goto _LL324;} _LL324: if(( unsigned int) _temp310 > 4u?*((
int*) _temp310) == Cyc_Absyn_FnType: 0){ _LL377: _temp376=(( struct Cyc_Absyn_FnType_struct*)
_temp310)->f1; goto _LL325;} else{ goto _LL326;} _LL326: if(( unsigned int)
_temp310 > 4u?*(( int*) _temp310) == Cyc_Absyn_RgnHandleType: 0){ _LL379:
_temp378=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp310)->f1; goto
_LL327;} else{ goto _LL328;} _LL328: if( _temp310 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL329;} else{ goto _LL330;} _LL330: if(( unsigned int) _temp310 > 4u?*((
int*) _temp310) == Cyc_Absyn_TunionType: 0){ _LL381: _temp380=(( struct Cyc_Absyn_TunionType_struct*)
_temp310)->f1; goto _LL331;} else{ goto _LL332;} _LL332: if(( unsigned int)
_temp310 > 4u?*(( int*) _temp310) == Cyc_Absyn_TunionFieldType: 0){ _LL383:
_temp382=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp310)->f1; _LL387:
_temp386=( void*) _temp382.field_info; if(*(( int*) _temp386) == Cyc_Absyn_KnownTunionfield){
_LL391: _temp390=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp386)->f1;
goto _LL389; _LL389: _temp388=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp386)->f2; goto _LL385;} else{ goto _LL334;} _LL385: _temp384= _temp382.targs;
goto _LL333;} else{ goto _LL334;} _LL334: if(( unsigned int) _temp310 > 4u?*((
int*) _temp310) == Cyc_Absyn_TunionFieldType: 0){ _LL393: _temp392=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp310)->f1; _LL397: _temp396=( void*) _temp392.field_info; if(*(( int*)
_temp396) == Cyc_Absyn_UnknownTunionfield){ _LL399: _temp398=(( struct Cyc_Absyn_UnknownTunionfield_struct*)
_temp396)->f1; goto _LL395;} else{ goto _LL336;} _LL395: _temp394= _temp392.targs;
goto _LL335;} else{ goto _LL336;} _LL336: if(( unsigned int) _temp310 > 4u?*((
int*) _temp310) == Cyc_Absyn_StructType: 0){ _LL405: _temp404=(( struct Cyc_Absyn_StructType_struct*)
_temp310)->f1; goto _LL403; _LL403: _temp402=(( struct Cyc_Absyn_StructType_struct*)
_temp310)->f2; goto _LL401; _LL401: _temp400=(( struct Cyc_Absyn_StructType_struct*)
_temp310)->f3; if( _temp400 == 0){ goto _LL337;} else{ goto _LL338;}} else{ goto
_LL338;} _LL338: if(( unsigned int) _temp310 > 4u?*(( int*) _temp310) == Cyc_Absyn_UnionType:
0){ _LL411: _temp410=(( struct Cyc_Absyn_UnionType_struct*) _temp310)->f1; goto
_LL409; _LL409: _temp408=(( struct Cyc_Absyn_UnionType_struct*) _temp310)->f2;
goto _LL407; _LL407: _temp406=(( struct Cyc_Absyn_UnionType_struct*) _temp310)->f3;
if( _temp406 == 0){ goto _LL339;} else{ goto _LL340;}} else{ goto _LL340;}
_LL340: if(( unsigned int) _temp310 > 4u?*(( int*) _temp310) == Cyc_Absyn_StructType:
0){ _LL417: _temp416=(( struct Cyc_Absyn_StructType_struct*) _temp310)->f1; goto
_LL415; _LL415: _temp414=(( struct Cyc_Absyn_StructType_struct*) _temp310)->f2;
goto _LL413; _LL413: _temp412=(( struct Cyc_Absyn_StructType_struct*) _temp310)->f3;
goto _LL341;} else{ goto _LL342;} _LL342: if(( unsigned int) _temp310 > 4u?*((
int*) _temp310) == Cyc_Absyn_UnionType: 0){ _LL423: _temp422=(( struct Cyc_Absyn_UnionType_struct*)
_temp310)->f1; goto _LL421; _LL421: _temp420=(( struct Cyc_Absyn_UnionType_struct*)
_temp310)->f2; goto _LL419; _LL419: _temp418=(( struct Cyc_Absyn_UnionType_struct*)
_temp310)->f3; goto _LL343;} else{ goto _LL344;} _LL344: if(( unsigned int)
_temp310 > 4u?*(( int*) _temp310) == Cyc_Absyn_AnonStructType: 0){ _LL425:
_temp424=(( struct Cyc_Absyn_AnonStructType_struct*) _temp310)->f1; goto _LL345;}
else{ goto _LL346;} _LL346: if(( unsigned int) _temp310 > 4u?*(( int*) _temp310)
== Cyc_Absyn_AnonUnionType: 0){ _LL427: _temp426=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp310)->f1; goto _LL347;} else{ goto _LL348;} _LL348: if(( unsigned int)
_temp310 > 4u?*(( int*) _temp310) == Cyc_Absyn_EnumType: 0){ _LL431: _temp430=((
struct Cyc_Absyn_EnumType_struct*) _temp310)->f1; goto _LL429; _LL429: _temp428=((
struct Cyc_Absyn_EnumType_struct*) _temp310)->f2; goto _LL349;} else{ goto
_LL350;} _LL350: if(( unsigned int) _temp310 > 4u?*(( int*) _temp310) == Cyc_Absyn_PointerType:
0){ _LL433: _temp432=(( struct Cyc_Absyn_PointerType_struct*) _temp310)->f1;
goto _LL351;} else{ goto _LL352;} _LL352: if(( unsigned int) _temp310 > 4u?*((
int*) _temp310) == Cyc_Absyn_ArrayType: 0){ _LL439: _temp438=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp310)->f1; goto _LL437; _LL437: _temp436=((
struct Cyc_Absyn_ArrayType_struct*) _temp310)->f2; goto _LL435; _LL435: _temp434=((
struct Cyc_Absyn_ArrayType_struct*) _temp310)->f3; goto _LL353;} else{ goto
_LL354;} _LL354: if(( unsigned int) _temp310 > 4u?*(( int*) _temp310) == Cyc_Absyn_TupleType:
0){ _LL441: _temp440=(( struct Cyc_Absyn_TupleType_struct*) _temp310)->f1; goto
_LL355;} else{ goto _LL356;} _LL356: if(( unsigned int) _temp310 > 4u?*(( int*)
_temp310) == Cyc_Absyn_TypedefType: 0){ _LL447: _temp446=(( struct Cyc_Absyn_TypedefType_struct*)
_temp310)->f1; goto _LL445; _LL445: _temp444=(( struct Cyc_Absyn_TypedefType_struct*)
_temp310)->f2; goto _LL443; _LL443: _temp442=(( struct Cyc_Absyn_TypedefType_struct*)
_temp310)->f3; goto _LL357;} else{ goto _LL358;} _LL358: if(( unsigned int)
_temp310 > 4u?*(( int*) _temp310) == Cyc_Absyn_AccessEff: 0){ _LL449: _temp448=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp310)->f1; goto _LL359;} else{
goto _LL360;} _LL360: if(( unsigned int) _temp310 > 4u?*(( int*) _temp310) ==
Cyc_Absyn_JoinEff: 0){ _LL451: _temp450=(( struct Cyc_Absyn_JoinEff_struct*)
_temp310)->f1; goto _LL361;} else{ goto _LL311;} _LL313: return( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp368))->v; _LL315: return(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp370->kind); _LL317: return( void*) Cyc_Absyn_MemKind;
_LL319: return _temp372 ==( void*) Cyc_Absyn_B4?( void*) Cyc_Absyn_BoxKind:(
void*) Cyc_Absyn_MemKind; _LL321: return( void*) Cyc_Absyn_MemKind; _LL323:
return( void*) Cyc_Absyn_MemKind; _LL325: return( void*) Cyc_Absyn_MemKind;
_LL327: return( void*) Cyc_Absyn_BoxKind; _LL329: return( void*) Cyc_Absyn_RgnKind;
_LL331: return( void*) Cyc_Absyn_BoxKind; _LL333: if( _temp388->typs == 0){
return( void*) Cyc_Absyn_BoxKind;} else{ return( void*) Cyc_Absyn_MemKind;}
_LL335: return(( void*(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)( _tag_arr("typ_kind: Unresolved TunionFieldType",
sizeof( unsigned char), 37u)); _LL337: return( void*) Cyc_Absyn_AnyKind; _LL339:
return( void*) Cyc_Absyn_AnyKind; _LL341: return( void*) Cyc_Absyn_MemKind;
_LL343: return( void*) Cyc_Absyn_MemKind; _LL345: return( void*) Cyc_Absyn_MemKind;
_LL347: return( void*) Cyc_Absyn_MemKind; _LL349: return( void*) Cyc_Absyn_BoxKind;
_LL351: { void* _temp452=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp432.bounds))->v; void* _temp462; void*
_temp464; struct Cyc_Absyn_Exp* _temp466; struct Cyc_Absyn_Conref* _temp468;
_LL454: if(( unsigned int) _temp452 > 1u?*(( int*) _temp452) == Cyc_Absyn_Eq_constr:
0){ _LL463: _temp462=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp452)->f1;
if( _temp462 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL455;} else{ goto _LL456;}}
else{ goto _LL456;} _LL456: if(( unsigned int) _temp452 > 1u?*(( int*) _temp452)
== Cyc_Absyn_Eq_constr: 0){ _LL465: _temp464=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp452)->f1; if(( unsigned int) _temp464 > 1u?*(( int*) _temp464) == Cyc_Absyn_Upper_b:
0){ _LL467: _temp466=(( struct Cyc_Absyn_Upper_b_struct*) _temp464)->f1; goto
_LL457;} else{ goto _LL458;}} else{ goto _LL458;} _LL458: if( _temp452 ==( void*)
Cyc_Absyn_No_constr){ goto _LL459;} else{ goto _LL460;} _LL460: if((
unsigned int) _temp452 > 1u?*(( int*) _temp452) == Cyc_Absyn_Forward_constr: 0){
_LL469: _temp468=(( struct Cyc_Absyn_Forward_constr_struct*) _temp452)->f1; goto
_LL461;} else{ goto _LL453;} _LL455: return( void*) Cyc_Absyn_MemKind; _LL457:
return( void*) Cyc_Absyn_BoxKind; _LL459: return( void*) Cyc_Absyn_MemKind;
_LL461: return(( void*(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)( _tag_arr("typ_kind: forward constraint in ptr bounds",
sizeof( unsigned char), 43u)); _LL453:;} _LL353: return( void*) Cyc_Absyn_MemKind;
_LL355: return( void*) Cyc_Absyn_MemKind; _LL357: return(( void*(*)( struct
_tagged_arr msg)) Cyc_Tcutil_impos)(( struct _tagged_arr)({ struct _tagged_arr
_temp470= Cyc_Absynpp_typ2string( t); xprintf("typ_kind: typedef found: %.*s",
_get_arr_size( _temp470, 1u), _temp470.curr);})); _LL359: return( void*) Cyc_Absyn_EffKind;
_LL361: return( void*) Cyc_Absyn_EffKind; _LL311:;} unsigned char Cyc_Tcutil_Unify[
10u]="\000\000\000\000Unify"; int Cyc_Tcutil_unify( void* t1, void* t2){ struct
_handler_cons _temp471; _push_handler(& _temp471);{ int _temp473= 0; if( setjmp(
_temp471.handler)){ _temp473= 1;} if( ! _temp473){ Cyc_Tcutil_unify_it( t1, t2);{
int _temp474= 1; _npop_handler( 0u); return _temp474;}; _pop_handler();} else{
void* _temp472=( void*) _exn_thrown; void* _temp476= _temp472; _LL478: if(
_temp476 == Cyc_Tcutil_Unify){ goto _LL479;} else{ goto _LL480;} _LL480: goto
_LL481; _LL479: return 0; _LL481:( void) _throw( _temp476); _LL477:;}}} static
void Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List* env, struct Cyc_List_List*
ts); static void Cyc_Tcutil_occurs( void* evar, struct Cyc_List_List* env, void*
t){ void* _temp482= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar* _temp516;
struct Cyc_Core_Opt* _temp518; struct Cyc_Core_Opt** _temp520; int _temp521;
struct Cyc_Core_Opt* _temp523; struct Cyc_Core_Opt* _temp525; struct Cyc_Absyn_PtrInfo
_temp527; struct Cyc_Absyn_Exp* _temp529; struct Cyc_Absyn_Tqual _temp531; void*
_temp533; struct Cyc_Absyn_FnInfo _temp535; struct Cyc_List_List* _temp537;
struct Cyc_List_List* _temp539; struct Cyc_Absyn_VarargInfo* _temp541; int
_temp543; struct Cyc_List_List* _temp545; void* _temp547; struct Cyc_Core_Opt*
_temp549; struct Cyc_List_List* _temp551; struct Cyc_List_List* _temp553; void*
_temp555; struct Cyc_Absyn_TunionInfo _temp557; void* _temp559; struct Cyc_List_List*
_temp561; void* _temp563; struct Cyc_Core_Opt* _temp565; struct Cyc_List_List*
_temp567; struct _tuple1* _temp569; struct Cyc_Absyn_TunionFieldInfo _temp571;
struct Cyc_List_List* _temp573; void* _temp575; struct Cyc_Absyn_Structdecl**
_temp577; struct Cyc_List_List* _temp579; struct _tuple1* _temp581; struct Cyc_List_List*
_temp583; struct Cyc_List_List* _temp585; void* _temp587; struct Cyc_List_List*
_temp589; _LL484: if(( unsigned int) _temp482 > 4u?*(( int*) _temp482) == Cyc_Absyn_VarType:
0){ _LL517: _temp516=(( struct Cyc_Absyn_VarType_struct*) _temp482)->f1; goto
_LL485;} else{ goto _LL486;} _LL486: if(( unsigned int) _temp482 > 4u?*(( int*)
_temp482) == Cyc_Absyn_Evar: 0){ _LL526: _temp525=(( struct Cyc_Absyn_Evar_struct*)
_temp482)->f1; goto _LL524; _LL524: _temp523=(( struct Cyc_Absyn_Evar_struct*)
_temp482)->f2; goto _LL522; _LL522: _temp521=(( struct Cyc_Absyn_Evar_struct*)
_temp482)->f3; goto _LL519; _LL519: _temp518=(( struct Cyc_Absyn_Evar_struct*)
_temp482)->f4; _temp520=&(( struct Cyc_Absyn_Evar_struct*) _temp482)->f4; goto
_LL487;} else{ goto _LL488;} _LL488: if(( unsigned int) _temp482 > 4u?*(( int*)
_temp482) == Cyc_Absyn_PointerType: 0){ _LL528: _temp527=(( struct Cyc_Absyn_PointerType_struct*)
_temp482)->f1; goto _LL489;} else{ goto _LL490;} _LL490: if(( unsigned int)
_temp482 > 4u?*(( int*) _temp482) == Cyc_Absyn_ArrayType: 0){ _LL534: _temp533=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp482)->f1; goto _LL532; _LL532:
_temp531=(( struct Cyc_Absyn_ArrayType_struct*) _temp482)->f2; goto _LL530;
_LL530: _temp529=(( struct Cyc_Absyn_ArrayType_struct*) _temp482)->f3; goto
_LL491;} else{ goto _LL492;} _LL492: if(( unsigned int) _temp482 > 4u?*(( int*)
_temp482) == Cyc_Absyn_FnType: 0){ _LL536: _temp535=(( struct Cyc_Absyn_FnType_struct*)
_temp482)->f1; _LL552: _temp551= _temp535.tvars; goto _LL550; _LL550: _temp549=
_temp535.effect; goto _LL548; _LL548: _temp547=( void*) _temp535.ret_typ; goto
_LL546; _LL546: _temp545= _temp535.args; goto _LL544; _LL544: _temp543= _temp535.c_varargs;
goto _LL542; _LL542: _temp541= _temp535.cyc_varargs; goto _LL540; _LL540:
_temp539= _temp535.rgn_po; goto _LL538; _LL538: _temp537= _temp535.attributes;
goto _LL493;} else{ goto _LL494;} _LL494: if(( unsigned int) _temp482 > 4u?*((
int*) _temp482) == Cyc_Absyn_TupleType: 0){ _LL554: _temp553=(( struct Cyc_Absyn_TupleType_struct*)
_temp482)->f1; goto _LL495;} else{ goto _LL496;} _LL496: if(( unsigned int)
_temp482 > 4u?*(( int*) _temp482) == Cyc_Absyn_RgnHandleType: 0){ _LL556:
_temp555=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp482)->f1; goto
_LL497;} else{ goto _LL498;} _LL498: if(( unsigned int) _temp482 > 4u?*(( int*)
_temp482) == Cyc_Absyn_TunionType: 0){ _LL558: _temp557=(( struct Cyc_Absyn_TunionType_struct*)
_temp482)->f1; _LL564: _temp563=( void*) _temp557.tunion_info; goto _LL562;
_LL562: _temp561= _temp557.targs; goto _LL560; _LL560: _temp559=( void*)
_temp557.rgn; goto _LL499;} else{ goto _LL500;} _LL500: if(( unsigned int)
_temp482 > 4u?*(( int*) _temp482) == Cyc_Absyn_TypedefType: 0){ _LL570: _temp569=((
struct Cyc_Absyn_TypedefType_struct*) _temp482)->f1; goto _LL568; _LL568:
_temp567=(( struct Cyc_Absyn_TypedefType_struct*) _temp482)->f2; goto _LL566;
_LL566: _temp565=(( struct Cyc_Absyn_TypedefType_struct*) _temp482)->f3; goto
_LL501;} else{ goto _LL502;} _LL502: if(( unsigned int) _temp482 > 4u?*(( int*)
_temp482) == Cyc_Absyn_TunionFieldType: 0){ _LL572: _temp571=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp482)->f1; _LL576: _temp575=( void*) _temp571.field_info; goto _LL574;
_LL574: _temp573= _temp571.targs; goto _LL503;} else{ goto _LL504;} _LL504: if((
unsigned int) _temp482 > 4u?*(( int*) _temp482) == Cyc_Absyn_StructType: 0){
_LL582: _temp581=(( struct Cyc_Absyn_StructType_struct*) _temp482)->f1; goto
_LL580; _LL580: _temp579=(( struct Cyc_Absyn_StructType_struct*) _temp482)->f2;
goto _LL578; _LL578: _temp577=(( struct Cyc_Absyn_StructType_struct*) _temp482)->f3;
goto _LL505;} else{ goto _LL506;} _LL506: if(( unsigned int) _temp482 > 4u?*((
int*) _temp482) == Cyc_Absyn_AnonStructType: 0){ _LL584: _temp583=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp482)->f1; goto _LL507;} else{ goto _LL508;} _LL508: if(( unsigned int)
_temp482 > 4u?*(( int*) _temp482) == Cyc_Absyn_AnonUnionType: 0){ _LL586:
_temp585=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp482)->f1; goto _LL509;}
else{ goto _LL510;} _LL510: if(( unsigned int) _temp482 > 4u?*(( int*) _temp482)
== Cyc_Absyn_AccessEff: 0){ _LL588: _temp587=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp482)->f1; goto _LL511;} else{ goto _LL512;} _LL512: if(( unsigned int)
_temp482 > 4u?*(( int*) _temp482) == Cyc_Absyn_JoinEff: 0){ _LL590: _temp589=((
struct Cyc_Absyn_JoinEff_struct*) _temp482)->f1; goto _LL513;} else{ goto _LL514;}
_LL514: goto _LL515; _LL485: if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, env, _temp516)){( int) _throw(( void*) Cyc_Tcutil_Unify);}
goto _LL483; _LL487: if( t == evar){( int) _throw(( void*) Cyc_Tcutil_Unify);}
else{ if( _temp523 != 0){ Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp523))->v);} else{ int problem= 0;{ struct Cyc_List_List* s=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(* _temp520))->v; for(
0; s != 0; s=(( struct Cyc_List_List*) _check_null( s))->tl){ if( !(( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( s))->hd)){ problem=
1; break;}}} if( problem){ struct Cyc_List_List* _temp591= 0;{ struct Cyc_List_List*
s=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(* _temp520))->v;
for( 0; s != 0; s=(( struct Cyc_List_List*) _check_null( s))->tl){ if((( int(*)(
int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)( Cyc_Tcutil_fast_tvar_cmp, env,(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( s))->hd)){
_temp591=({ struct Cyc_List_List* _temp592=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp592->hd=( void*)(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( s))->hd); _temp592->tl= _temp591; _temp592;});}}}*
_temp520=({ struct Cyc_Core_Opt* _temp593=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp593->v=( void*) _temp591; _temp593;});}}}
goto _LL483; _LL489: Cyc_Tcutil_occurs( evar, env,( void*) _temp527.elt_typ);
Cyc_Tcutil_occurs( evar, env,( void*) _temp527.rgn_typ); goto _LL483; _LL491:
Cyc_Tcutil_occurs( evar, env, _temp533); goto _LL483; _LL493: if( _temp549 != 0){
Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp549))->v);} Cyc_Tcutil_occurs( evar, env, _temp547); for( 0; _temp545 != 0;
_temp545=(( struct Cyc_List_List*) _check_null( _temp545))->tl){ Cyc_Tcutil_occurs(
evar, env,(*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp545))->hd)).f3);}
if( _temp541 != 0){ int _temp596; void* _temp598; void* _temp600; struct Cyc_Absyn_Tqual
_temp602; struct Cyc_Core_Opt* _temp604; struct Cyc_Absyn_VarargInfo _temp594=*((
struct Cyc_Absyn_VarargInfo*) _check_null( _temp541)); _LL605: _temp604=
_temp594.name; goto _LL603; _LL603: _temp602= _temp594.tq; goto _LL601; _LL601:
_temp600=( void*) _temp594.type; goto _LL599; _LL599: _temp598=( void*) _temp594.rgn;
goto _LL597; _LL597: _temp596= _temp594.inject; goto _LL595; _LL595: Cyc_Tcutil_occurs(
evar, env, _temp600); Cyc_Tcutil_occurs( evar, env, _temp598);} for( 0; _temp539
!= 0; _temp539=(( struct Cyc_List_List*) _check_null( _temp539))->tl){ struct
_tuple8 _temp608; void* _temp609; void* _temp611; struct _tuple8* _temp606=(
struct _tuple8*)(( struct Cyc_List_List*) _check_null( _temp539))->hd; _temp608=*
_temp606; _LL612: _temp611= _temp608.f1; goto _LL610; _LL610: _temp609= _temp608.f2;
goto _LL607; _LL607: Cyc_Tcutil_occurs( evar, env, _temp611); Cyc_Tcutil_occurs(
evar, env, _temp609);} goto _LL483; _LL495: for( 0; _temp553 != 0; _temp553=((
struct Cyc_List_List*) _check_null( _temp553))->tl){ Cyc_Tcutil_occurs( evar,
env,(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp553))->hd)).f2);}
goto _LL483; _LL497: Cyc_Tcutil_occurs( evar, env, _temp555); goto _LL483;
_LL499: Cyc_Tcutil_occurs( evar, env, _temp559); Cyc_Tcutil_occurslist( evar,
env, _temp561); goto _LL483; _LL501: _temp573= _temp567; goto _LL503; _LL503:
_temp579= _temp573; goto _LL505; _LL505: Cyc_Tcutil_occurslist( evar, env,
_temp579); goto _LL483; _LL507: _temp585= _temp583; goto _LL509; _LL509: for( 0;
_temp585 != 0; _temp585=(( struct Cyc_List_List*) _check_null( _temp585))->tl){
Cyc_Tcutil_occurs( evar, env,( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp585))->hd)->type);} goto _LL483; _LL511: Cyc_Tcutil_occurs(
evar, env, _temp587); goto _LL483; _LL513: Cyc_Tcutil_occurslist( evar, env,
_temp589); goto _LL483; _LL515: goto _LL483; _LL483:;} static void Cyc_Tcutil_occurslist(
void* evar, struct Cyc_List_List* env, struct Cyc_List_List* ts){ for( 0; ts !=
0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){ Cyc_Tcutil_occurs( evar,
env,( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);}} static void Cyc_Tcutil_unify_list(
struct Cyc_List_List* t1, struct Cyc_List_List* t2){ struct _handler_cons
_temp613; _push_handler(& _temp613);{ int _temp615= 0; if( setjmp( _temp613.handler)){
_temp615= 1;} if( ! _temp615){(( void(*)( void(* f)( void*, void*), struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_iter2)( Cyc_Tcutil_unify_it, t1, t2);;
_pop_handler();} else{ void* _temp614=( void*) _exn_thrown; void* _temp617=
_temp614; _LL619: if( _temp617 == Cyc_List_List_mismatch){ goto _LL620;} else{
goto _LL621;} _LL621: goto _LL622; _LL620:( int) _throw(( void*) Cyc_Tcutil_Unify);
_LL622:( void) _throw( _temp617); _LL618:;}}} static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2){ if(( tq1.q_const != tq2.q_const?
1: tq1.q_volatile != tq2.q_volatile)? 1: tq1.q_restrict != tq2.q_restrict){( int)
_throw(( void*) Cyc_Tcutil_Unify);}} int Cyc_Tcutil_equal_tqual( struct Cyc_Absyn_Tqual
tq1, struct Cyc_Absyn_Tqual tq2){ return( tq1.q_const == tq2.q_const? tq1.q_volatile
== tq2.q_volatile: 0)? tq1.q_restrict == tq2.q_restrict: 0;} static void Cyc_Tcutil_unify_it_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y){ x=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
x); y=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
y); if( x == y){ return;}{ void* _temp623=( void*) x->v; struct Cyc_Absyn_Conref*
_temp631; void* _temp633; _LL625: if( _temp623 ==( void*) Cyc_Absyn_No_constr){
goto _LL626;} else{ goto _LL627;} _LL627: if(( unsigned int) _temp623 > 1u?*((
int*) _temp623) == Cyc_Absyn_Forward_constr: 0){ _LL632: _temp631=(( struct Cyc_Absyn_Forward_constr_struct*)
_temp623)->f1; goto _LL628;} else{ goto _LL629;} _LL629: if(( unsigned int)
_temp623 > 1u?*(( int*) _temp623) == Cyc_Absyn_Eq_constr: 0){ _LL634: _temp633=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp623)->f1; goto _LL630;} else{
goto _LL624;} _LL626:( void*)( x->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp635=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp635[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp636; _temp636.tag= Cyc_Absyn_Forward_constr; _temp636.f1= y; _temp636;});
_temp635;}))); return; _LL628:( int) _throw((( void*(*)( struct _tagged_arr msg))
Cyc_Tcutil_impos)( _tag_arr("unify_conref: forward after compress", sizeof(
unsigned char), 37u))); _LL630: { void* _temp637=( void*) y->v; struct Cyc_Absyn_Conref*
_temp645; void* _temp647; _LL639: if( _temp637 ==( void*) Cyc_Absyn_No_constr){
goto _LL640;} else{ goto _LL641;} _LL641: if(( unsigned int) _temp637 > 1u?*((
int*) _temp637) == Cyc_Absyn_Forward_constr: 0){ _LL646: _temp645=(( struct Cyc_Absyn_Forward_constr_struct*)
_temp637)->f1; goto _LL642;} else{ goto _LL643;} _LL643: if(( unsigned int)
_temp637 > 1u?*(( int*) _temp637) == Cyc_Absyn_Eq_constr: 0){ _LL648: _temp647=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp637)->f1; goto _LL644;} else{
goto _LL638;} _LL640:( void*)( y->v=( void*)(( void*) x->v)); return; _LL642:(
int) _throw((( void*(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)( _tag_arr("unify_conref: forward after compress(2)",
sizeof( unsigned char), 40u))); _LL644: if( cmp( _temp633, _temp647) != 0){( int)
_throw(( void*) Cyc_Tcutil_Unify);} return; _LL638:;} _LL624:;}} static int Cyc_Tcutil_unify_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y){ struct _handler_cons _temp649; _push_handler(& _temp649);{ int _temp651= 0;
if( setjmp( _temp649.handler)){ _temp651= 1;} if( ! _temp651){(( void(*)( int(*
cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y))
Cyc_Tcutil_unify_it_conrefs)( cmp, x, y);{ int _temp652= 1; _npop_handler( 0u);
return _temp652;}; _pop_handler();} else{ void* _temp650=( void*) _exn_thrown;
void* _temp654= _temp650; _LL656: if( _temp654 == Cyc_Tcutil_Unify){ goto _LL657;}
else{ goto _LL658;} _LL658: goto _LL659; _LL657: return 0; _LL659:( void) _throw(
_temp654); _LL655:;}}} static int Cyc_Tcutil_boundscmp( void* b1, void* b2){
struct _tuple8 _temp661=({ struct _tuple8 _temp660; _temp660.f1= b1; _temp660.f2=
b2; _temp660;}); void* _temp671; void* _temp673; void* _temp675; void* _temp677;
void* _temp679; void* _temp681; void* _temp683; struct Cyc_Absyn_Exp* _temp685;
void* _temp687; struct Cyc_Absyn_Exp* _temp689; _LL663: _LL674: _temp673=
_temp661.f1; if( _temp673 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL672;} else{
goto _LL665;} _LL672: _temp671= _temp661.f2; if( _temp671 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL664;} else{ goto _LL665;} _LL665: _LL678: _temp677= _temp661.f1; if(
_temp677 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL676;} else{ goto _LL667;}
_LL676: _temp675= _temp661.f2; goto _LL666; _LL667: _LL682: _temp681= _temp661.f1;
goto _LL680; _LL680: _temp679= _temp661.f2; if( _temp679 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL668;} else{ goto _LL669;} _LL669: _LL688: _temp687= _temp661.f1; if((
unsigned int) _temp687 > 1u?*(( int*) _temp687) == Cyc_Absyn_Upper_b: 0){ _LL690:
_temp689=(( struct Cyc_Absyn_Upper_b_struct*) _temp687)->f1; goto _LL684;} else{
goto _LL662;} _LL684: _temp683= _temp661.f2; if(( unsigned int) _temp683 > 1u?*((
int*) _temp683) == Cyc_Absyn_Upper_b: 0){ _LL686: _temp685=(( struct Cyc_Absyn_Upper_b_struct*)
_temp683)->f1; goto _LL670;} else{ goto _LL662;} _LL664: return 0; _LL666:
return - 1; _LL668: return 1; _LL670: { int i1=( int) Cyc_Evexp_eval_const_uint_exp(
_temp689); int i2=( int) Cyc_Evexp_eval_const_uint_exp( _temp685); if( i1 == i2){
return 0;} if( i1 < i2){ return - 1;} return 1;} _LL662:;} static int Cyc_Tcutil_equal_att(
void* a1, void* a2){ if( a1 == a2){ return 1;}{ struct _tuple8 _temp692=({
struct _tuple8 _temp691; _temp691.f1= a1; _temp691.f2= a2; _temp691;}); void*
_temp702; int _temp704; void* _temp706; int _temp708; void* _temp710; int
_temp712; void* _temp714; int _temp716; void* _temp718; struct _tagged_arr
_temp720; void* _temp722; struct _tagged_arr _temp724; _LL694: _LL707: _temp706=
_temp692.f1; if(( unsigned int) _temp706 > 16u?*(( int*) _temp706) == Cyc_Absyn_Regparm_att:
0){ _LL709: _temp708=(( struct Cyc_Absyn_Regparm_att_struct*) _temp706)->f1;
goto _LL703;} else{ goto _LL696;} _LL703: _temp702= _temp692.f2; if((
unsigned int) _temp702 > 16u?*(( int*) _temp702) == Cyc_Absyn_Regparm_att: 0){
_LL705: _temp704=(( struct Cyc_Absyn_Regparm_att_struct*) _temp702)->f1; goto
_LL695;} else{ goto _LL696;} _LL696: _LL715: _temp714= _temp692.f1; if((
unsigned int) _temp714 > 16u?*(( int*) _temp714) == Cyc_Absyn_Aligned_att: 0){
_LL717: _temp716=(( struct Cyc_Absyn_Aligned_att_struct*) _temp714)->f1; goto
_LL711;} else{ goto _LL698;} _LL711: _temp710= _temp692.f2; if(( unsigned int)
_temp710 > 16u?*(( int*) _temp710) == Cyc_Absyn_Aligned_att: 0){ _LL713:
_temp712=(( struct Cyc_Absyn_Aligned_att_struct*) _temp710)->f1; goto _LL697;}
else{ goto _LL698;} _LL698: _LL723: _temp722= _temp692.f1; if(( unsigned int)
_temp722 > 16u?*(( int*) _temp722) == Cyc_Absyn_Section_att: 0){ _LL725:
_temp724=(( struct Cyc_Absyn_Section_att_struct*) _temp722)->f1; goto _LL719;}
else{ goto _LL700;} _LL719: _temp718= _temp692.f2; if(( unsigned int) _temp718 >
16u?*(( int*) _temp718) == Cyc_Absyn_Section_att: 0){ _LL721: _temp720=(( struct
Cyc_Absyn_Section_att_struct*) _temp718)->f1; goto _LL699;} else{ goto _LL700;}
_LL700: goto _LL701; _LL695: _temp716= _temp708; _temp712= _temp704; goto _LL697;
_LL697: return _temp716 == _temp712; _LL699: return Cyc_String_strcmp( _temp724,
_temp720) == 0; _LL701: return 0; _LL693:;}} int Cyc_Tcutil_same_atts( struct
Cyc_List_List* a1, struct Cyc_List_List* a2){{ struct Cyc_List_List* a= a1; for(
0; a != 0; a=(( struct Cyc_List_List*) _check_null( a))->tl){ if( !(( int(*)(
int(* pred)( void*, void*), void* env, struct Cyc_List_List* x)) Cyc_List_exists_c)(
Cyc_Tcutil_equal_att,( void*)(( struct Cyc_List_List*) _check_null( a))->hd, a2)){
return 0;}}}{ struct Cyc_List_List* a= a2; for( 0; a != 0; a=(( struct Cyc_List_List*)
_check_null( a))->tl){ if( !(( int(*)( int(* pred)( void*, void*), void* env,
struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcutil_equal_att,( void*)((
struct Cyc_List_List*) _check_null( a))->hd, a1)){ return 0;}}} return 1;}
static void Cyc_Tcutil_normalize_effect( void* e){ e= Cyc_Tcutil_compress( e);{
void* _temp726= e; struct Cyc_List_List* _temp732; struct Cyc_List_List**
_temp734; _LL728: if(( unsigned int) _temp726 > 4u?*(( int*) _temp726) == Cyc_Absyn_JoinEff:
0){ _LL733: _temp732=(( struct Cyc_Absyn_JoinEff_struct*) _temp726)->f1;
_temp734=&(( struct Cyc_Absyn_JoinEff_struct*) _temp726)->f1; goto _LL729;}
else{ goto _LL730;} _LL730: goto _LL731; _LL729: { int nested_join= 0;{ struct
Cyc_List_List* effs=* _temp734; for( 0; effs != 0; effs=(( struct Cyc_List_List*)
_check_null( effs))->tl){ void* _temp735=( void*)(( struct Cyc_List_List*)
_check_null( effs))->hd; Cyc_Tcutil_normalize_effect( _temp735);{ void* _temp736=
Cyc_Tcutil_compress( _temp735); struct Cyc_List_List* _temp742; _LL738: if((
unsigned int) _temp736 > 4u?*(( int*) _temp736) == Cyc_Absyn_JoinEff: 0){ _LL743:
_temp742=(( struct Cyc_Absyn_JoinEff_struct*) _temp736)->f1; goto _LL739;} else{
goto _LL740;} _LL740: goto _LL741; _LL739: nested_join= 1; goto _LL737; _LL741:
goto _LL737; _LL737:;}}} if( ! nested_join){ return;}{ struct Cyc_List_List*
effects= 0;{ struct Cyc_List_List* effs=* _temp734; for( 0; effs != 0; effs=((
struct Cyc_List_List*) _check_null( effs))->tl){ void* _temp744= Cyc_Tcutil_compress((
void*)(( struct Cyc_List_List*) _check_null( effs))->hd); struct Cyc_List_List*
_temp750; _LL746: if(( unsigned int) _temp744 > 4u?*(( int*) _temp744) == Cyc_Absyn_JoinEff:
0){ _LL751: _temp750=(( struct Cyc_Absyn_JoinEff_struct*) _temp744)->f1; goto
_LL747;} else{ goto _LL748;} _LL748: goto _LL749; _LL747: effects=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_revappend)( _temp750,
effects); goto _LL745; _LL749: effects=({ struct Cyc_List_List* _temp752=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp752->hd=(
void*) _temp744; _temp752->tl= effects; _temp752;}); goto _LL745; _LL745:;}}*
_temp734=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
effects); goto _LL727;}} _LL731: goto _LL727; _LL727:;}} struct _tuple9{ void*
f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; } ; static struct
_tuple9* Cyc_Tcutil_get_effect_evar( void* t){ void* _temp753= Cyc_Tcutil_compress(
t); struct Cyc_List_List* _temp761; struct Cyc_List_List _temp763; struct Cyc_List_List*
_temp764; void* _temp766; struct Cyc_Core_Opt* _temp768; int _temp770; struct
Cyc_Core_Opt* _temp772; struct Cyc_Core_Opt* _temp774; _LL755: if(( unsigned int)
_temp753 > 4u?*(( int*) _temp753) == Cyc_Absyn_JoinEff: 0){ _LL762: _temp761=((
struct Cyc_Absyn_JoinEff_struct*) _temp753)->f1; if( _temp761 == 0){ goto _LL757;}
else{ _temp763=* _temp761; _LL767: _temp766=( void*) _temp763.hd; goto _LL765;
_LL765: _temp764= _temp763.tl; goto _LL756;}} else{ goto _LL757;} _LL757: if((
unsigned int) _temp753 > 4u?*(( int*) _temp753) == Cyc_Absyn_Evar: 0){ _LL775:
_temp774=(( struct Cyc_Absyn_Evar_struct*) _temp753)->f1; goto _LL773; _LL773:
_temp772=(( struct Cyc_Absyn_Evar_struct*) _temp753)->f2; goto _LL771; _LL771:
_temp770=(( struct Cyc_Absyn_Evar_struct*) _temp753)->f3; goto _LL769; _LL769:
_temp768=(( struct Cyc_Absyn_Evar_struct*) _temp753)->f4; goto _LL758;} else{
goto _LL759;} _LL759: goto _LL760; _LL756: { void* _temp776= Cyc_Tcutil_compress(
_temp766); struct Cyc_Core_Opt* _temp782; int _temp784; struct Cyc_Core_Opt*
_temp786; struct Cyc_Core_Opt* _temp788; _LL778: if(( unsigned int) _temp776 > 4u?*((
int*) _temp776) == Cyc_Absyn_Evar: 0){ _LL789: _temp788=(( struct Cyc_Absyn_Evar_struct*)
_temp776)->f1; goto _LL787; _LL787: _temp786=(( struct Cyc_Absyn_Evar_struct*)
_temp776)->f2; goto _LL785; _LL785: _temp784=(( struct Cyc_Absyn_Evar_struct*)
_temp776)->f3; goto _LL783; _LL783: _temp782=(( struct Cyc_Absyn_Evar_struct*)
_temp776)->f4; goto _LL779;} else{ goto _LL780;} _LL780: goto _LL781; _LL779:
return({ struct _tuple9* _temp790=( struct _tuple9*) GC_malloc( sizeof( struct
_tuple9)); _temp790->f1= _temp766; _temp790->f2= _temp764; _temp790->f3=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp782))->v; _temp790;});
_LL781: return 0; _LL777:;} _LL758: if( _temp774 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp774))->v !=( void*) Cyc_Absyn_EffKind){(( int(*)( struct
_tagged_arr msg)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u));} return({ struct _tuple9* _temp791=( struct
_tuple9*) GC_malloc( sizeof( struct _tuple9)); _temp791->f1= t; _temp791->f2= 0;
_temp791->f3=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp768))->v; _temp791;}); _LL760: return 0; _LL754:;} static struct Cyc_Core_Opt
Cyc_Tcutil_ek={( void*)(( void*) 4u)}; int Cyc_Tcutil_region_in_effect( int
constrain, void* r, void* e){ r= Cyc_Tcutil_compress( r); if( r ==( void*) Cyc_Absyn_HeapRgn){
return 1;}{ void* _temp792= Cyc_Tcutil_compress( e); void* _temp802; struct Cyc_List_List*
_temp804; struct Cyc_Core_Opt* _temp806; int _temp808; struct Cyc_Core_Opt*
_temp810; struct Cyc_Core_Opt** _temp812; struct Cyc_Core_Opt* _temp813; _LL794:
if(( unsigned int) _temp792 > 4u?*(( int*) _temp792) == Cyc_Absyn_AccessEff: 0){
_LL803: _temp802=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp792)->f1;
goto _LL795;} else{ goto _LL796;} _LL796: if(( unsigned int) _temp792 > 4u?*((
int*) _temp792) == Cyc_Absyn_JoinEff: 0){ _LL805: _temp804=(( struct Cyc_Absyn_JoinEff_struct*)
_temp792)->f1; goto _LL797;} else{ goto _LL798;} _LL798: if(( unsigned int)
_temp792 > 4u?*(( int*) _temp792) == Cyc_Absyn_Evar: 0){ _LL814: _temp813=((
struct Cyc_Absyn_Evar_struct*) _temp792)->f1; goto _LL811; _LL811: _temp810=((
struct Cyc_Absyn_Evar_struct*) _temp792)->f2; _temp812=&(( struct Cyc_Absyn_Evar_struct*)
_temp792)->f2; goto _LL809; _LL809: _temp808=(( struct Cyc_Absyn_Evar_struct*)
_temp792)->f3; goto _LL807; _LL807: _temp806=(( struct Cyc_Absyn_Evar_struct*)
_temp792)->f4; goto _LL799;} else{ goto _LL800;} _LL800: goto _LL801; _LL795:
if( constrain){ return Cyc_Tcutil_unify( r, _temp802);} Cyc_Tcutil_compress(
_temp802); if( r == _temp802){ return 1;}{ struct _tuple8 _temp816=({ struct
_tuple8 _temp815; _temp815.f1= r; _temp815.f2= _temp802; _temp815;}); void*
_temp822; struct Cyc_Absyn_Tvar* _temp824; void* _temp826; struct Cyc_Absyn_Tvar*
_temp828; _LL818: _LL827: _temp826= _temp816.f1; if(( unsigned int) _temp826 > 4u?*((
int*) _temp826) == Cyc_Absyn_VarType: 0){ _LL829: _temp828=(( struct Cyc_Absyn_VarType_struct*)
_temp826)->f1; goto _LL823;} else{ goto _LL820;} _LL823: _temp822= _temp816.f2;
if(( unsigned int) _temp822 > 4u?*(( int*) _temp822) == Cyc_Absyn_VarType: 0){
_LL825: _temp824=(( struct Cyc_Absyn_VarType_struct*) _temp822)->f1; goto _LL819;}
else{ goto _LL820;} _LL820: goto _LL821; _LL819: return Cyc_Absyn_tvar_cmp(
_temp828, _temp824) == 0; _LL821: return 0; _LL817:;} _LL797: for( 0; _temp804
!= 0; _temp804=(( struct Cyc_List_List*) _check_null( _temp804))->tl){ if( Cyc_Tcutil_region_in_effect(
constrain, r,( void*)(( struct Cyc_List_List*) _check_null( _temp804))->hd)){
return 1;}} return 0; _LL799: if( _temp813 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp813))->v !=( void*) Cyc_Absyn_EffKind){(( int(*)( struct
_tagged_arr msg)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u));} if( ! constrain){ return 0;}{ void* _temp830=
Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_ek, _temp806); Cyc_Tcutil_occurs(
_temp830,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp806))->v,
r);{ struct Cyc_Absyn_JoinEff_struct* _temp831=({ struct Cyc_Absyn_JoinEff_struct*
_temp833=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp833[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp834; _temp834.tag= Cyc_Absyn_JoinEff;
_temp834.f1=({ struct Cyc_List_List* _temp835=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp835->hd=( void*) _temp830; _temp835->tl=({
struct Cyc_List_List* _temp836=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp836->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp837=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp837[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp838; _temp838.tag= Cyc_Absyn_AccessEff;
_temp838.f1=( void*) r; _temp838;}); _temp837;})); _temp836->tl= 0; _temp836;});
_temp835;}); _temp834;}); _temp833;});* _temp812=({ struct Cyc_Core_Opt*
_temp832=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp832->v=( void*)(( void*) _temp831); _temp832;}); return 1;}} _LL801: return
0; _LL793:;}} static int Cyc_Tcutil_variable_in_effect( int constrain, struct
Cyc_Absyn_Tvar* v, void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp839= e;
struct Cyc_Absyn_Tvar* _temp849; struct Cyc_List_List* _temp851; struct Cyc_Core_Opt*
_temp853; int _temp855; struct Cyc_Core_Opt* _temp857; struct Cyc_Core_Opt**
_temp859; struct Cyc_Core_Opt* _temp860; _LL841: if(( unsigned int) _temp839 > 4u?*((
int*) _temp839) == Cyc_Absyn_VarType: 0){ _LL850: _temp849=(( struct Cyc_Absyn_VarType_struct*)
_temp839)->f1; goto _LL842;} else{ goto _LL843;} _LL843: if(( unsigned int)
_temp839 > 4u?*(( int*) _temp839) == Cyc_Absyn_JoinEff: 0){ _LL852: _temp851=((
struct Cyc_Absyn_JoinEff_struct*) _temp839)->f1; goto _LL844;} else{ goto _LL845;}
_LL845: if(( unsigned int) _temp839 > 4u?*(( int*) _temp839) == Cyc_Absyn_Evar:
0){ _LL861: _temp860=(( struct Cyc_Absyn_Evar_struct*) _temp839)->f1; goto
_LL858; _LL858: _temp857=(( struct Cyc_Absyn_Evar_struct*) _temp839)->f2;
_temp859=&(( struct Cyc_Absyn_Evar_struct*) _temp839)->f2; goto _LL856; _LL856:
_temp855=(( struct Cyc_Absyn_Evar_struct*) _temp839)->f3; goto _LL854; _LL854:
_temp853=(( struct Cyc_Absyn_Evar_struct*) _temp839)->f4; goto _LL846;} else{
goto _LL847;} _LL847: goto _LL848; _LL842: return Cyc_Absyn_tvar_cmp( v,
_temp849) == 0; _LL844: for( 0; _temp851 != 0; _temp851=(( struct Cyc_List_List*)
_check_null( _temp851))->tl){ if( Cyc_Tcutil_variable_in_effect( constrain, v,(
void*)(( struct Cyc_List_List*) _check_null( _temp851))->hd)){ return 1;}}
return 0; _LL846: if( _temp860 == 0? 1:( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp860))->v !=( void*) Cyc_Absyn_EffKind){(( int(*)( struct
_tagged_arr msg)) Cyc_Tcutil_impos)( _tag_arr("effect evar has wrong kind",
sizeof( unsigned char), 27u));}{ void* _temp862= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek, _temp853); if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp853))->v, v)){ return 0;}{ struct Cyc_Absyn_JoinEff_struct*
_temp863=({ struct Cyc_Absyn_JoinEff_struct* _temp865=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp865[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp866; _temp866.tag= Cyc_Absyn_JoinEff; _temp866.f1=({ struct Cyc_List_List*
_temp867=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp867->hd=( void*) _temp862; _temp867->tl=({ struct Cyc_List_List* _temp868=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp868->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp869=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp869[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp870; _temp870.tag= Cyc_Absyn_VarType; _temp870.f1= v; _temp870;}); _temp869;}));
_temp868->tl= 0; _temp868;}); _temp867;}); _temp866;}); _temp865;});* _temp859=({
struct Cyc_Core_Opt* _temp864=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp864->v=( void*)(( void*) _temp863); _temp864;}); return 1;}}
_LL848: return 0; _LL840:;}} static int Cyc_Tcutil_evar_in_effect( void* evar,
void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp871= e; struct Cyc_List_List*
_temp879; struct Cyc_Core_Opt* _temp881; int _temp883; struct Cyc_Core_Opt*
_temp885; struct Cyc_Core_Opt* _temp887; _LL873: if(( unsigned int) _temp871 > 4u?*((
int*) _temp871) == Cyc_Absyn_JoinEff: 0){ _LL880: _temp879=(( struct Cyc_Absyn_JoinEff_struct*)
_temp871)->f1; goto _LL874;} else{ goto _LL875;} _LL875: if(( unsigned int)
_temp871 > 4u?*(( int*) _temp871) == Cyc_Absyn_Evar: 0){ _LL888: _temp887=((
struct Cyc_Absyn_Evar_struct*) _temp871)->f1; goto _LL886; _LL886: _temp885=((
struct Cyc_Absyn_Evar_struct*) _temp871)->f2; goto _LL884; _LL884: _temp883=((
struct Cyc_Absyn_Evar_struct*) _temp871)->f3; goto _LL882; _LL882: _temp881=((
struct Cyc_Absyn_Evar_struct*) _temp871)->f4; goto _LL876;} else{ goto _LL877;}
_LL877: goto _LL878; _LL874: for( 0; _temp879 != 0; _temp879=(( struct Cyc_List_List*)
_check_null( _temp879))->tl){ if( Cyc_Tcutil_evar_in_effect( evar,( void*)((
struct Cyc_List_List*) _check_null( _temp879))->hd)){ return 1;}} return 0;
_LL876: return evar == e; _LL878: return 0; _LL872:;}} int Cyc_Tcutil_subset_effect(
int set_to_empty, void* e1, void* e2){{ struct _tuple9* _temp889= Cyc_Tcutil_get_effect_evar(
e2); struct _tuple9 _temp895; struct Cyc_List_List* _temp896; struct Cyc_List_List*
_temp898; void* _temp900; _LL891: if( _temp889 == 0){ goto _LL893;} else{
_temp895=* _temp889; _LL901: _temp900= _temp895.f1; goto _LL899; _LL899:
_temp898= _temp895.f2; goto _LL897; _LL897: _temp896= _temp895.f3; goto _LL892;}
_LL893: goto _LL894; _LL892: { void* _temp902= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,({ struct Cyc_Core_Opt* _temp908=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp908->v=( void*) _temp896; _temp908;}));
struct Cyc_Absyn_JoinEff_struct* _temp903=({ struct Cyc_Absyn_JoinEff_struct*
_temp904=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp904[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp905; _temp905.tag= Cyc_Absyn_JoinEff;
_temp905.f1=({ struct Cyc_List_List* _temp906=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp906->hd=( void*) _temp902; _temp906->tl=({
struct Cyc_List_List* _temp907=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp907->hd=( void*) e1; _temp907->tl= 0; _temp907;});
_temp906;}); _temp905;}); _temp904;}); return Cyc_Tcutil_unify( _temp900,( void*)
_temp903);} _LL894: goto _LL890; _LL890:;}{ void* _temp909= Cyc_Tcutil_compress(
e1); struct Cyc_List_List* _temp921; void* _temp923; struct Cyc_Absyn_Tvar*
_temp925; struct Cyc_Core_Opt* _temp927; int _temp929; struct Cyc_Core_Opt*
_temp931; struct Cyc_Core_Opt** _temp933; struct Cyc_Core_Opt* _temp934; _LL911:
if(( unsigned int) _temp909 > 4u?*(( int*) _temp909) == Cyc_Absyn_JoinEff: 0){
_LL922: _temp921=(( struct Cyc_Absyn_JoinEff_struct*) _temp909)->f1; goto _LL912;}
else{ goto _LL913;} _LL913: if(( unsigned int) _temp909 > 4u?*(( int*) _temp909)
== Cyc_Absyn_AccessEff: 0){ _LL924: _temp923=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp909)->f1; goto _LL914;} else{ goto _LL915;} _LL915: if(( unsigned int)
_temp909 > 4u?*(( int*) _temp909) == Cyc_Absyn_VarType: 0){ _LL926: _temp925=((
struct Cyc_Absyn_VarType_struct*) _temp909)->f1; goto _LL916;} else{ goto _LL917;}
_LL917: if(( unsigned int) _temp909 > 4u?*(( int*) _temp909) == Cyc_Absyn_Evar:
0){ _LL935: _temp934=(( struct Cyc_Absyn_Evar_struct*) _temp909)->f1; goto
_LL932; _LL932: _temp931=(( struct Cyc_Absyn_Evar_struct*) _temp909)->f2;
_temp933=&(( struct Cyc_Absyn_Evar_struct*) _temp909)->f2; goto _LL930; _LL930:
_temp929=(( struct Cyc_Absyn_Evar_struct*) _temp909)->f3; goto _LL928; _LL928:
_temp927=(( struct Cyc_Absyn_Evar_struct*) _temp909)->f4; goto _LL918;} else{
goto _LL919;} _LL919: goto _LL920; _LL912: for( 0; _temp921 != 0; _temp921=((
struct Cyc_List_List*) _check_null( _temp921))->tl){ if( ! Cyc_Tcutil_subset_effect(
set_to_empty,( void*)(( struct Cyc_List_List*) _check_null( _temp921))->hd, e2)){
return 0;}} return 1; _LL914: return( Cyc_Tcutil_region_in_effect( 0, _temp923,
e2)? 1: Cyc_Tcutil_region_in_effect( 1, _temp923, e2))? 1: Cyc_Tcutil_unify(
_temp923,( void*) Cyc_Absyn_HeapRgn); _LL916: return Cyc_Tcutil_variable_in_effect(
0, _temp925, e2)? 1: Cyc_Tcutil_variable_in_effect( 1, _temp925, e2); _LL918:
if( ! Cyc_Tcutil_evar_in_effect( e1, e2)){ if( set_to_empty){* _temp933=({
struct Cyc_Core_Opt* _temp936=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp936->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp937=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp937[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp938; _temp938.tag= Cyc_Absyn_JoinEff;
_temp938.f1= 0; _temp938;}); _temp937;})); _temp936;});} else{ Cyc_Tcutil_occurs(
e1,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp927))->v,
e2);* _temp933=({ struct Cyc_Core_Opt* _temp939=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp939->v=( void*) e2; _temp939;});}}
return 1; _LL920: return(( int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)((
struct _tagged_arr)({ struct _tagged_arr _temp940= Cyc_Absynpp_typ2string( e1);
xprintf("subset_effect: bad effect: %.*s", _get_arr_size( _temp940, 1u),
_temp940.curr);})); _LL910:;}} struct _tuple10{ struct _tuple9* f1; struct
_tuple9* f2; } ; static int Cyc_Tcutil_unify_effect( void* e1, void* e2){ e1=
Cyc_Tcutil_compress( e1); e2= Cyc_Tcutil_compress( e2); Cyc_Tcutil_normalize_effect(
e1); Cyc_Tcutil_normalize_effect( e2);{ struct _tuple10 _temp942=({ struct
_tuple10 _temp941; _temp941.f1= Cyc_Tcutil_get_effect_evar( e1); _temp941.f2=
Cyc_Tcutil_get_effect_evar( e2); _temp941;}); struct _tuple9* _temp948; struct
_tuple9 _temp950; struct Cyc_List_List* _temp951; struct Cyc_List_List* _temp953;
void* _temp955; struct _tuple9* _temp957; struct _tuple9 _temp959; struct Cyc_List_List*
_temp960; struct Cyc_List_List* _temp962; void* _temp964; _LL944: _LL958:
_temp957= _temp942.f1; if( _temp957 == 0){ goto _LL946;} else{ _temp959=*
_temp957; _LL965: _temp964= _temp959.f1; goto _LL963; _LL963: _temp962= _temp959.f2;
goto _LL961; _LL961: _temp960= _temp959.f3; goto _LL949;} _LL949: _temp948=
_temp942.f2; if( _temp948 == 0){ goto _LL946;} else{ _temp950=* _temp948; _LL956:
_temp955= _temp950.f1; goto _LL954; _LL954: _temp953= _temp950.f2; goto _LL952;
_LL952: _temp951= _temp950.f3; goto _LL945;} _LL946: goto _LL947; _LL945: {
struct Cyc_List_List* _temp966= 0; for( 0; _temp960 != 0; _temp960=(( struct Cyc_List_List*)
_check_null( _temp960))->tl){ if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, _temp951,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp960))->hd)){ _temp966=({ struct Cyc_List_List* _temp967=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp967->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp960))->hd);
_temp967->tl= _temp966; _temp967;});}}{ void* _temp968= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,({ struct Cyc_Core_Opt* _temp973=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp973->v=( void*)
_temp966; _temp973;})); struct Cyc_Absyn_JoinEff_struct* _temp969=({ struct Cyc_Absyn_JoinEff_struct*
_temp970=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp970[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp971; _temp971.tag= Cyc_Absyn_JoinEff;
_temp971.f1=({ struct Cyc_List_List* _temp972=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp972->hd=( void*) _temp968; _temp972->tl=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp962, _temp953); _temp972;}); _temp971;}); _temp970;}); return Cyc_Tcutil_unify(
_temp964,( void*) _temp969)? Cyc_Tcutil_unify( _temp955,( void*) _temp969): 0;}}
_LL947: return Cyc_Tcutil_subset_effect( 0, e1, e2)? Cyc_Tcutil_subset_effect( 0,
e2, e1): 0; _LL943:;}} static int Cyc_Tcutil_sub_rgnpo( struct Cyc_List_List*
rpo1, struct Cyc_List_List* rpo2){{ struct Cyc_List_List* r1= rpo1; for( 0; r1
!= 0; r1=(( struct Cyc_List_List*) _check_null( r1))->tl){ struct _tuple8
_temp976; void* _temp977; void* _temp979; struct _tuple8* _temp974=( struct
_tuple8*)(( struct Cyc_List_List*) _check_null( r1))->hd; _temp976=* _temp974;
_LL980: _temp979= _temp976.f1; goto _LL978; _LL978: _temp977= _temp976.f2; goto
_LL975; _LL975: { int found= _temp979 ==( void*) Cyc_Absyn_HeapRgn;{ struct Cyc_List_List*
r2= rpo2; for( 0; r2 != 0? ! found: 0; r2=(( struct Cyc_List_List*) _check_null(
r2))->tl){ struct _tuple8 _temp983; void* _temp984; void* _temp986; struct
_tuple8* _temp981=( struct _tuple8*)(( struct Cyc_List_List*) _check_null( r2))->hd;
_temp983=* _temp981; _LL987: _temp986= _temp983.f1; goto _LL985; _LL985:
_temp984= _temp983.f2; goto _LL982; _LL982: if( Cyc_Tcutil_unify( _temp979,
_temp986)? Cyc_Tcutil_unify( _temp977, _temp984): 0){ found= 1; break;}}} if( !
found){ return 0;}}}} return 1;} static int Cyc_Tcutil_same_rgn_po( struct Cyc_List_List*
rpo1, struct Cyc_List_List* rpo2){ return Cyc_Tcutil_sub_rgnpo( rpo1, rpo2)? Cyc_Tcutil_sub_rgnpo(
rpo2, rpo1): 0;} struct _tuple11{ struct Cyc_Absyn_VarargInfo* f1; struct Cyc_Absyn_VarargInfo*
f2; } ; struct _tuple12{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ;
void Cyc_Tcutil_unify_it( void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2=
Cyc_Tcutil_compress( t2); if( t1 == t2){ return;}{ void* _temp988= t1; struct
Cyc_Core_Opt* _temp994; int _temp996; struct Cyc_Core_Opt* _temp998; struct Cyc_Core_Opt**
_temp1000; struct Cyc_Core_Opt* _temp1001; _LL990: if(( unsigned int) _temp988 >
4u?*(( int*) _temp988) == Cyc_Absyn_Evar: 0){ _LL1002: _temp1001=(( struct Cyc_Absyn_Evar_struct*)
_temp988)->f1; goto _LL999; _LL999: _temp998=(( struct Cyc_Absyn_Evar_struct*)
_temp988)->f2; _temp1000=&(( struct Cyc_Absyn_Evar_struct*) _temp988)->f2; goto
_LL997; _LL997: _temp996=(( struct Cyc_Absyn_Evar_struct*) _temp988)->f3; goto
_LL995; _LL995: _temp994=(( struct Cyc_Absyn_Evar_struct*) _temp988)->f4; goto
_LL991;} else{ goto _LL992;} _LL992: goto _LL993; _LL991: Cyc_Tcutil_occurs( t1,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp994))->v, t2);{
void* _temp1003= Cyc_Tcutil_typ_kind( t2); if( Cyc_Tcutil_kind_leq( _temp1003,(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1001))->v)){* _temp1000=({
struct Cyc_Core_Opt* _temp1004=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1004->v=( void*) t2; _temp1004;}); return;} else{{ void*
_temp1005= t2; struct Cyc_Core_Opt* _temp1013; int _temp1015; struct Cyc_Core_Opt*
_temp1017; struct Cyc_Core_Opt** _temp1019; struct Cyc_Core_Opt* _temp1020;
struct Cyc_Absyn_PtrInfo _temp1023; _LL1007: if(( unsigned int) _temp1005 > 4u?*((
int*) _temp1005) == Cyc_Absyn_Evar: 0){ _LL1021: _temp1020=(( struct Cyc_Absyn_Evar_struct*)
_temp1005)->f1; goto _LL1018; _LL1018: _temp1017=(( struct Cyc_Absyn_Evar_struct*)
_temp1005)->f2; _temp1019=&(( struct Cyc_Absyn_Evar_struct*) _temp1005)->f2;
goto _LL1016; _LL1016: _temp1015=(( struct Cyc_Absyn_Evar_struct*) _temp1005)->f3;
goto _LL1014; _LL1014: _temp1013=(( struct Cyc_Absyn_Evar_struct*) _temp1005)->f4;
goto _LL1008;} else{ goto _LL1009;} _LL1009: if(( unsigned int) _temp1005 > 4u?*((
int*) _temp1005) == Cyc_Absyn_PointerType: 0){ _LL1024: _temp1023=(( struct Cyc_Absyn_PointerType_struct*)
_temp1005)->f1; goto _LL1022;} else{ goto _LL1011;} _LL1022: if(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1001))->v ==( void*) Cyc_Absyn_BoxKind){
goto _LL1010;} else{ goto _LL1011;} _LL1011: goto _LL1012; _LL1008: { struct Cyc_List_List*
_temp1025=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp994))->v;{
struct Cyc_List_List* s2=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1013))->v; for( 0; s2 != 0; s2=(( struct Cyc_List_List*)
_check_null( s2))->tl){ if( !(( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, _temp1025,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( s2))->hd)){( int) _throw(( void*) Cyc_Tcutil_Unify);}}} if( Cyc_Tcutil_kind_leq((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1001))->v, _temp1003)){*
_temp1019=({ struct Cyc_Core_Opt* _temp1026=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1026->v=( void*) t1; _temp1026;}); return;}
goto _LL1006;} _LL1010: { struct Cyc_Absyn_Conref* _temp1027=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1023.bounds);{
void* _temp1028=( void*) _temp1027->v; _LL1030: if( _temp1028 ==( void*) Cyc_Absyn_No_constr){
goto _LL1031;} else{ goto _LL1032;} _LL1032: goto _LL1033; _LL1031:( void*)(
_temp1027->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp1034=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp1034[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp1035; _temp1035.tag= Cyc_Absyn_Eq_constr;
_temp1035.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1036=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1036[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1037; _temp1037.tag= Cyc_Absyn_Upper_b;
_temp1037.f1= Cyc_Absyn_signed_int_exp( 1, 0); _temp1037;}); _temp1036;}));
_temp1035;}); _temp1034;})));* _temp1000=({ struct Cyc_Core_Opt* _temp1038=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1038->v=(
void*) t2; _temp1038;}); return; _LL1033: goto _LL1029; _LL1029:;} goto _LL1006;}
_LL1012: goto _LL1006; _LL1006:;}( int) _throw(( void*) Cyc_Tcutil_Unify);}}
_LL993: goto _LL989; _LL989:;}{ struct _tuple8 _temp1040=({ struct _tuple8
_temp1039; _temp1039.f1= t2; _temp1039.f2= t1; _temp1039;}); void* _temp1090;
void* _temp1092; struct Cyc_Core_Opt* _temp1094; int _temp1096; struct Cyc_Core_Opt*
_temp1098; struct Cyc_Core_Opt* _temp1100; void* _temp1102; void* _temp1104;
void* _temp1106; struct Cyc_Absyn_Tvar* _temp1108; void* _temp1110; struct Cyc_Absyn_Tvar*
_temp1112; void* _temp1114; struct Cyc_Absyn_Structdecl** _temp1116; struct Cyc_List_List*
_temp1118; struct _tuple1* _temp1120; void* _temp1122; struct Cyc_Absyn_Structdecl**
_temp1124; struct Cyc_List_List* _temp1126; struct _tuple1* _temp1128; void*
_temp1130; struct Cyc_Absyn_Enumdecl* _temp1132; struct _tuple1* _temp1134; void*
_temp1136; struct Cyc_Absyn_Enumdecl* _temp1138; struct _tuple1* _temp1140; void*
_temp1142; struct Cyc_Absyn_Uniondecl** _temp1144; struct Cyc_List_List*
_temp1146; struct _tuple1* _temp1148; void* _temp1150; struct Cyc_Absyn_Uniondecl**
_temp1152; struct Cyc_List_List* _temp1154; struct _tuple1* _temp1156; void*
_temp1158; struct Cyc_Absyn_TunionInfo _temp1160; void* _temp1162; struct Cyc_List_List*
_temp1164; void* _temp1166; struct Cyc_Absyn_Tuniondecl* _temp1168; void*
_temp1170; struct Cyc_Absyn_TunionInfo _temp1172; void* _temp1174; struct Cyc_List_List*
_temp1176; void* _temp1178; struct Cyc_Absyn_Tuniondecl* _temp1180; void*
_temp1182; struct Cyc_Absyn_TunionFieldInfo _temp1184; struct Cyc_List_List*
_temp1186; void* _temp1188; struct Cyc_Absyn_Tunionfield* _temp1190; struct Cyc_Absyn_Tuniondecl*
_temp1192; void* _temp1194; struct Cyc_Absyn_TunionFieldInfo _temp1196; struct
Cyc_List_List* _temp1198; void* _temp1200; struct Cyc_Absyn_Tunionfield*
_temp1202; struct Cyc_Absyn_Tuniondecl* _temp1204; void* _temp1206; struct Cyc_Absyn_PtrInfo
_temp1208; struct Cyc_Absyn_Conref* _temp1210; struct Cyc_Absyn_Tqual _temp1212;
struct Cyc_Absyn_Conref* _temp1214; void* _temp1216; void* _temp1218; void*
_temp1220; struct Cyc_Absyn_PtrInfo _temp1222; struct Cyc_Absyn_Conref*
_temp1224; struct Cyc_Absyn_Tqual _temp1226; struct Cyc_Absyn_Conref* _temp1228;
void* _temp1230; void* _temp1232; void* _temp1234; void* _temp1236; void*
_temp1238; void* _temp1240; void* _temp1242; void* _temp1244; void* _temp1246;
void* _temp1248; void* _temp1250; void* _temp1252; void* _temp1254; struct Cyc_Absyn_Exp*
_temp1256; struct Cyc_Absyn_Tqual _temp1258; void* _temp1260; void* _temp1262;
struct Cyc_Absyn_Exp* _temp1264; struct Cyc_Absyn_Tqual _temp1266; void*
_temp1268; void* _temp1270; struct Cyc_Absyn_FnInfo _temp1272; struct Cyc_List_List*
_temp1274; struct Cyc_List_List* _temp1276; struct Cyc_Absyn_VarargInfo*
_temp1278; int _temp1280; struct Cyc_List_List* _temp1282; void* _temp1284;
struct Cyc_Core_Opt* _temp1286; struct Cyc_List_List* _temp1288; void* _temp1290;
struct Cyc_Absyn_FnInfo _temp1292; struct Cyc_List_List* _temp1294; struct Cyc_List_List*
_temp1296; struct Cyc_Absyn_VarargInfo* _temp1298; int _temp1300; struct Cyc_List_List*
_temp1302; void* _temp1304; struct Cyc_Core_Opt* _temp1306; struct Cyc_List_List*
_temp1308; void* _temp1310; struct Cyc_List_List* _temp1312; void* _temp1314;
struct Cyc_List_List* _temp1316; void* _temp1318; struct Cyc_List_List*
_temp1320; void* _temp1322; struct Cyc_List_List* _temp1324; void* _temp1326;
struct Cyc_List_List* _temp1328; void* _temp1330; struct Cyc_List_List*
_temp1332; void* _temp1334; void* _temp1336; void* _temp1338; void* _temp1340;
void* _temp1342; void* _temp1344; void* _temp1346; void* _temp1348; struct Cyc_List_List*
_temp1350; void* _temp1352; struct Cyc_List_List* _temp1354; void* _temp1356;
void* _temp1358; void* _temp1360; void* _temp1362; void* _temp1364; void*
_temp1366; void* _temp1368; _LL1042: _LL1093: _temp1092= _temp1040.f1; if((
unsigned int) _temp1092 > 4u?*(( int*) _temp1092) == Cyc_Absyn_Evar: 0){ _LL1101:
_temp1100=(( struct Cyc_Absyn_Evar_struct*) _temp1092)->f1; goto _LL1099;
_LL1099: _temp1098=(( struct Cyc_Absyn_Evar_struct*) _temp1092)->f2; goto
_LL1097; _LL1097: _temp1096=(( struct Cyc_Absyn_Evar_struct*) _temp1092)->f3;
goto _LL1095; _LL1095: _temp1094=(( struct Cyc_Absyn_Evar_struct*) _temp1092)->f4;
goto _LL1091;} else{ goto _LL1044;} _LL1091: _temp1090= _temp1040.f2; goto
_LL1043; _LL1044: _LL1105: _temp1104= _temp1040.f1; if( _temp1104 ==( void*) Cyc_Absyn_VoidType){
goto _LL1103;} else{ goto _LL1046;} _LL1103: _temp1102= _temp1040.f2; if(
_temp1102 ==( void*) Cyc_Absyn_VoidType){ goto _LL1045;} else{ goto _LL1046;}
_LL1046: _LL1111: _temp1110= _temp1040.f1; if(( unsigned int) _temp1110 > 4u?*((
int*) _temp1110) == Cyc_Absyn_VarType: 0){ _LL1113: _temp1112=(( struct Cyc_Absyn_VarType_struct*)
_temp1110)->f1; goto _LL1107;} else{ goto _LL1048;} _LL1107: _temp1106=
_temp1040.f2; if(( unsigned int) _temp1106 > 4u?*(( int*) _temp1106) == Cyc_Absyn_VarType:
0){ _LL1109: _temp1108=(( struct Cyc_Absyn_VarType_struct*) _temp1106)->f1; goto
_LL1047;} else{ goto _LL1048;} _LL1048: _LL1123: _temp1122= _temp1040.f1; if((
unsigned int) _temp1122 > 4u?*(( int*) _temp1122) == Cyc_Absyn_StructType: 0){
_LL1129: _temp1128=(( struct Cyc_Absyn_StructType_struct*) _temp1122)->f1; goto
_LL1127; _LL1127: _temp1126=(( struct Cyc_Absyn_StructType_struct*) _temp1122)->f2;
goto _LL1125; _LL1125: _temp1124=(( struct Cyc_Absyn_StructType_struct*)
_temp1122)->f3; goto _LL1115;} else{ goto _LL1050;} _LL1115: _temp1114=
_temp1040.f2; if(( unsigned int) _temp1114 > 4u?*(( int*) _temp1114) == Cyc_Absyn_StructType:
0){ _LL1121: _temp1120=(( struct Cyc_Absyn_StructType_struct*) _temp1114)->f1;
goto _LL1119; _LL1119: _temp1118=(( struct Cyc_Absyn_StructType_struct*)
_temp1114)->f2; goto _LL1117; _LL1117: _temp1116=(( struct Cyc_Absyn_StructType_struct*)
_temp1114)->f3; goto _LL1049;} else{ goto _LL1050;} _LL1050: _LL1137: _temp1136=
_temp1040.f1; if(( unsigned int) _temp1136 > 4u?*(( int*) _temp1136) == Cyc_Absyn_EnumType:
0){ _LL1141: _temp1140=(( struct Cyc_Absyn_EnumType_struct*) _temp1136)->f1;
goto _LL1139; _LL1139: _temp1138=(( struct Cyc_Absyn_EnumType_struct*) _temp1136)->f2;
goto _LL1131;} else{ goto _LL1052;} _LL1131: _temp1130= _temp1040.f2; if((
unsigned int) _temp1130 > 4u?*(( int*) _temp1130) == Cyc_Absyn_EnumType: 0){
_LL1135: _temp1134=(( struct Cyc_Absyn_EnumType_struct*) _temp1130)->f1; goto
_LL1133; _LL1133: _temp1132=(( struct Cyc_Absyn_EnumType_struct*) _temp1130)->f2;
goto _LL1051;} else{ goto _LL1052;} _LL1052: _LL1151: _temp1150= _temp1040.f1;
if(( unsigned int) _temp1150 > 4u?*(( int*) _temp1150) == Cyc_Absyn_UnionType: 0){
_LL1157: _temp1156=(( struct Cyc_Absyn_UnionType_struct*) _temp1150)->f1; goto
_LL1155; _LL1155: _temp1154=(( struct Cyc_Absyn_UnionType_struct*) _temp1150)->f2;
goto _LL1153; _LL1153: _temp1152=(( struct Cyc_Absyn_UnionType_struct*)
_temp1150)->f3; goto _LL1143;} else{ goto _LL1054;} _LL1143: _temp1142=
_temp1040.f2; if(( unsigned int) _temp1142 > 4u?*(( int*) _temp1142) == Cyc_Absyn_UnionType:
0){ _LL1149: _temp1148=(( struct Cyc_Absyn_UnionType_struct*) _temp1142)->f1;
goto _LL1147; _LL1147: _temp1146=(( struct Cyc_Absyn_UnionType_struct*)
_temp1142)->f2; goto _LL1145; _LL1145: _temp1144=(( struct Cyc_Absyn_UnionType_struct*)
_temp1142)->f3; goto _LL1053;} else{ goto _LL1054;} _LL1054: _LL1171: _temp1170=
_temp1040.f1; if(( unsigned int) _temp1170 > 4u?*(( int*) _temp1170) == Cyc_Absyn_TunionType:
0){ _LL1173: _temp1172=(( struct Cyc_Absyn_TunionType_struct*) _temp1170)->f1;
_LL1179: _temp1178=( void*) _temp1172.tunion_info; if(*(( int*) _temp1178) ==
Cyc_Absyn_KnownTunion){ _LL1181: _temp1180=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1178)->f1; goto _LL1177;} else{ goto _LL1056;} _LL1177: _temp1176=
_temp1172.targs; goto _LL1175; _LL1175: _temp1174=( void*) _temp1172.rgn; goto
_LL1159;} else{ goto _LL1056;} _LL1159: _temp1158= _temp1040.f2; if((
unsigned int) _temp1158 > 4u?*(( int*) _temp1158) == Cyc_Absyn_TunionType: 0){
_LL1161: _temp1160=(( struct Cyc_Absyn_TunionType_struct*) _temp1158)->f1;
_LL1167: _temp1166=( void*) _temp1160.tunion_info; if(*(( int*) _temp1166) ==
Cyc_Absyn_KnownTunion){ _LL1169: _temp1168=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1166)->f1; goto _LL1165;} else{ goto _LL1056;} _LL1165: _temp1164=
_temp1160.targs; goto _LL1163; _LL1163: _temp1162=( void*) _temp1160.rgn; goto
_LL1055;} else{ goto _LL1056;} _LL1056: _LL1195: _temp1194= _temp1040.f1; if((
unsigned int) _temp1194 > 4u?*(( int*) _temp1194) == Cyc_Absyn_TunionFieldType:
0){ _LL1197: _temp1196=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1194)->f1;
_LL1201: _temp1200=( void*) _temp1196.field_info; if(*(( int*) _temp1200) == Cyc_Absyn_KnownTunionfield){
_LL1205: _temp1204=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1200)->f1;
goto _LL1203; _LL1203: _temp1202=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1200)->f2; goto _LL1199;} else{ goto _LL1058;} _LL1199: _temp1198=
_temp1196.targs; goto _LL1183;} else{ goto _LL1058;} _LL1183: _temp1182=
_temp1040.f2; if(( unsigned int) _temp1182 > 4u?*(( int*) _temp1182) == Cyc_Absyn_TunionFieldType:
0){ _LL1185: _temp1184=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1182)->f1;
_LL1189: _temp1188=( void*) _temp1184.field_info; if(*(( int*) _temp1188) == Cyc_Absyn_KnownTunionfield){
_LL1193: _temp1192=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1188)->f1;
goto _LL1191; _LL1191: _temp1190=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1188)->f2; goto _LL1187;} else{ goto _LL1058;} _LL1187: _temp1186=
_temp1184.targs; goto _LL1057;} else{ goto _LL1058;} _LL1058: _LL1221: _temp1220=
_temp1040.f1; if(( unsigned int) _temp1220 > 4u?*(( int*) _temp1220) == Cyc_Absyn_PointerType:
0){ _LL1223: _temp1222=(( struct Cyc_Absyn_PointerType_struct*) _temp1220)->f1;
_LL1233: _temp1232=( void*) _temp1222.elt_typ; goto _LL1231; _LL1231: _temp1230=(
void*) _temp1222.rgn_typ; goto _LL1229; _LL1229: _temp1228= _temp1222.nullable;
goto _LL1227; _LL1227: _temp1226= _temp1222.tq; goto _LL1225; _LL1225: _temp1224=
_temp1222.bounds; goto _LL1207;} else{ goto _LL1060;} _LL1207: _temp1206=
_temp1040.f2; if(( unsigned int) _temp1206 > 4u?*(( int*) _temp1206) == Cyc_Absyn_PointerType:
0){ _LL1209: _temp1208=(( struct Cyc_Absyn_PointerType_struct*) _temp1206)->f1;
_LL1219: _temp1218=( void*) _temp1208.elt_typ; goto _LL1217; _LL1217: _temp1216=(
void*) _temp1208.rgn_typ; goto _LL1215; _LL1215: _temp1214= _temp1208.nullable;
goto _LL1213; _LL1213: _temp1212= _temp1208.tq; goto _LL1211; _LL1211: _temp1210=
_temp1208.bounds; goto _LL1059;} else{ goto _LL1060;} _LL1060: _LL1241:
_temp1240= _temp1040.f1; if(( unsigned int) _temp1240 > 4u?*(( int*) _temp1240)
== Cyc_Absyn_IntType: 0){ _LL1245: _temp1244=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1240)->f1; goto _LL1243; _LL1243: _temp1242=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1240)->f2; goto _LL1235;} else{ goto _LL1062;} _LL1235: _temp1234=
_temp1040.f2; if(( unsigned int) _temp1234 > 4u?*(( int*) _temp1234) == Cyc_Absyn_IntType:
0){ _LL1239: _temp1238=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1234)->f1;
goto _LL1237; _LL1237: _temp1236=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1234)->f2; goto _LL1061;} else{ goto _LL1062;} _LL1062: _LL1249: _temp1248=
_temp1040.f1; if( _temp1248 ==( void*) Cyc_Absyn_FloatType){ goto _LL1247;}
else{ goto _LL1064;} _LL1247: _temp1246= _temp1040.f2; if( _temp1246 ==( void*)
Cyc_Absyn_FloatType){ goto _LL1063;} else{ goto _LL1064;} _LL1064: _LL1253:
_temp1252= _temp1040.f1; if( _temp1252 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL1251;} else{ goto _LL1066;} _LL1251: _temp1250= _temp1040.f2; if( _temp1250
==( void*) Cyc_Absyn_DoubleType){ goto _LL1065;} else{ goto _LL1066;} _LL1066:
_LL1263: _temp1262= _temp1040.f1; if(( unsigned int) _temp1262 > 4u?*(( int*)
_temp1262) == Cyc_Absyn_ArrayType: 0){ _LL1269: _temp1268=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1262)->f1; goto _LL1267; _LL1267: _temp1266=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1262)->f2; goto _LL1265; _LL1265: _temp1264=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1262)->f3; goto _LL1255;} else{ goto _LL1068;} _LL1255: _temp1254=
_temp1040.f2; if(( unsigned int) _temp1254 > 4u?*(( int*) _temp1254) == Cyc_Absyn_ArrayType:
0){ _LL1261: _temp1260=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1254)->f1;
goto _LL1259; _LL1259: _temp1258=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1254)->f2; goto _LL1257; _LL1257: _temp1256=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1254)->f3; goto _LL1067;} else{ goto _LL1068;} _LL1068: _LL1291: _temp1290=
_temp1040.f1; if(( unsigned int) _temp1290 > 4u?*(( int*) _temp1290) == Cyc_Absyn_FnType:
0){ _LL1293: _temp1292=(( struct Cyc_Absyn_FnType_struct*) _temp1290)->f1;
_LL1309: _temp1308= _temp1292.tvars; goto _LL1307; _LL1307: _temp1306= _temp1292.effect;
goto _LL1305; _LL1305: _temp1304=( void*) _temp1292.ret_typ; goto _LL1303;
_LL1303: _temp1302= _temp1292.args; goto _LL1301; _LL1301: _temp1300= _temp1292.c_varargs;
goto _LL1299; _LL1299: _temp1298= _temp1292.cyc_varargs; goto _LL1297; _LL1297:
_temp1296= _temp1292.rgn_po; goto _LL1295; _LL1295: _temp1294= _temp1292.attributes;
goto _LL1271;} else{ goto _LL1070;} _LL1271: _temp1270= _temp1040.f2; if((
unsigned int) _temp1270 > 4u?*(( int*) _temp1270) == Cyc_Absyn_FnType: 0){
_LL1273: _temp1272=(( struct Cyc_Absyn_FnType_struct*) _temp1270)->f1; _LL1289:
_temp1288= _temp1272.tvars; goto _LL1287; _LL1287: _temp1286= _temp1272.effect;
goto _LL1285; _LL1285: _temp1284=( void*) _temp1272.ret_typ; goto _LL1283;
_LL1283: _temp1282= _temp1272.args; goto _LL1281; _LL1281: _temp1280= _temp1272.c_varargs;
goto _LL1279; _LL1279: _temp1278= _temp1272.cyc_varargs; goto _LL1277; _LL1277:
_temp1276= _temp1272.rgn_po; goto _LL1275; _LL1275: _temp1274= _temp1272.attributes;
goto _LL1069;} else{ goto _LL1070;} _LL1070: _LL1315: _temp1314= _temp1040.f1;
if(( unsigned int) _temp1314 > 4u?*(( int*) _temp1314) == Cyc_Absyn_TupleType: 0){
_LL1317: _temp1316=(( struct Cyc_Absyn_TupleType_struct*) _temp1314)->f1; goto
_LL1311;} else{ goto _LL1072;} _LL1311: _temp1310= _temp1040.f2; if((
unsigned int) _temp1310 > 4u?*(( int*) _temp1310) == Cyc_Absyn_TupleType: 0){
_LL1313: _temp1312=(( struct Cyc_Absyn_TupleType_struct*) _temp1310)->f1; goto
_LL1071;} else{ goto _LL1072;} _LL1072: _LL1323: _temp1322= _temp1040.f1; if((
unsigned int) _temp1322 > 4u?*(( int*) _temp1322) == Cyc_Absyn_AnonStructType: 0){
_LL1325: _temp1324=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1322)->f1;
goto _LL1319;} else{ goto _LL1074;} _LL1319: _temp1318= _temp1040.f2; if((
unsigned int) _temp1318 > 4u?*(( int*) _temp1318) == Cyc_Absyn_AnonStructType: 0){
_LL1321: _temp1320=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1318)->f1;
goto _LL1073;} else{ goto _LL1074;} _LL1074: _LL1331: _temp1330= _temp1040.f1;
if(( unsigned int) _temp1330 > 4u?*(( int*) _temp1330) == Cyc_Absyn_AnonUnionType:
0){ _LL1333: _temp1332=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1330)->f1;
goto _LL1327;} else{ goto _LL1076;} _LL1327: _temp1326= _temp1040.f2; if((
unsigned int) _temp1326 > 4u?*(( int*) _temp1326) == Cyc_Absyn_AnonUnionType: 0){
_LL1329: _temp1328=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1326)->f1;
goto _LL1075;} else{ goto _LL1076;} _LL1076: _LL1337: _temp1336= _temp1040.f1;
if( _temp1336 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1335;} else{ goto _LL1078;}
_LL1335: _temp1334= _temp1040.f2; if( _temp1334 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL1077;} else{ goto _LL1078;} _LL1078: _LL1343: _temp1342= _temp1040.f1;
if(( unsigned int) _temp1342 > 4u?*(( int*) _temp1342) == Cyc_Absyn_RgnHandleType:
0){ _LL1345: _temp1344=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1342)->f1; goto _LL1339;} else{ goto _LL1080;} _LL1339: _temp1338=
_temp1040.f2; if(( unsigned int) _temp1338 > 4u?*(( int*) _temp1338) == Cyc_Absyn_RgnHandleType:
0){ _LL1341: _temp1340=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1338)->f1; goto _LL1079;} else{ goto _LL1080;} _LL1080: _LL1349: _temp1348=
_temp1040.f1; if(( unsigned int) _temp1348 > 4u?*(( int*) _temp1348) == Cyc_Absyn_JoinEff:
0){ _LL1351: _temp1350=(( struct Cyc_Absyn_JoinEff_struct*) _temp1348)->f1; goto
_LL1347;} else{ goto _LL1082;} _LL1347: _temp1346= _temp1040.f2; goto _LL1081;
_LL1082: _LL1357: _temp1356= _temp1040.f1; goto _LL1353; _LL1353: _temp1352=
_temp1040.f2; if(( unsigned int) _temp1352 > 4u?*(( int*) _temp1352) == Cyc_Absyn_JoinEff:
0){ _LL1355: _temp1354=(( struct Cyc_Absyn_JoinEff_struct*) _temp1352)->f1; goto
_LL1083;} else{ goto _LL1084;} _LL1084: _LL1361: _temp1360= _temp1040.f1; if((
unsigned int) _temp1360 > 4u?*(( int*) _temp1360) == Cyc_Absyn_AccessEff: 0){
_LL1363: _temp1362=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp1360)->f1;
goto _LL1359;} else{ goto _LL1086;} _LL1359: _temp1358= _temp1040.f2; goto
_LL1085; _LL1086: _LL1369: _temp1368= _temp1040.f1; goto _LL1365; _LL1365:
_temp1364= _temp1040.f2; if(( unsigned int) _temp1364 > 4u?*(( int*) _temp1364)
== Cyc_Absyn_AccessEff: 0){ _LL1367: _temp1366=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp1364)->f1; goto _LL1087;} else{ goto _LL1088;} _LL1088: goto _LL1089;
_LL1043: Cyc_Tcutil_unify_it( t2, t1); return; _LL1045: return; _LL1047: {
struct _tagged_arr* _temp1370= _temp1112->name; struct _tagged_arr* _temp1371=
_temp1108->name; int _temp1372=*(( int*) _check_null( _temp1112->identity)); int
_temp1373=*(( int*) _check_null( _temp1108->identity)); void* _temp1374=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1112->kind); void*
_temp1375=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1108->kind); if( _temp1373 == _temp1372? Cyc_String_zstrptrcmp( _temp1370,
_temp1371) == 0: 0){ if( _temp1374 != _temp1375){(( int(*)( struct _tagged_arr
msg)) Cyc_Tcutil_impos)(( struct _tagged_arr)({ struct _tagged_arr _temp1376=*
_temp1370; struct _tagged_arr _temp1377= Cyc_Absynpp_kind2string( _temp1374);
struct _tagged_arr _temp1378= Cyc_Absynpp_kind2string( _temp1375); xprintf("same type variable %.*s has kinds %.*s and %.*s",
_get_arr_size( _temp1376, 1u), _temp1376.curr, _get_arr_size( _temp1377, 1u),
_temp1377.curr, _get_arr_size( _temp1378, 1u), _temp1378.curr);}));} return;}
goto _LL1041;} _LL1049: if((( _temp1120 != 0? _temp1128 != 0: 0)? Cyc_Absyn_qvar_cmp((
struct _tuple1*) _check_null( _temp1120),( struct _tuple1*) _check_null(
_temp1128)) == 0: 0)? 1:( _temp1120 == 0? _temp1128 == 0: 0)){ Cyc_Tcutil_unify_list(
_temp1118, _temp1126); return;} goto _LL1041; _LL1051: if( Cyc_Absyn_qvar_cmp(
_temp1140, _temp1134) == 0){ return;} goto _LL1041; _LL1053: if((( _temp1148 !=
0? _temp1156 != 0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*) _check_null(
_temp1148),( struct _tuple1*) _check_null( _temp1156)) == 0: 0)? 1:( _temp1148
== 0? _temp1156 == 0: 0)){ Cyc_Tcutil_unify_list( _temp1146, _temp1154); return;}
goto _LL1041; _LL1055: if( _temp1180 == _temp1168? 1: Cyc_Absyn_qvar_cmp(
_temp1180->name, _temp1168->name) == 0){ Cyc_Tcutil_unify_it( _temp1162,
_temp1174); Cyc_Tcutil_unify_list( _temp1164, _temp1176); return;} goto _LL1041;
_LL1057: if(( _temp1204 == _temp1192? 1: Cyc_Absyn_qvar_cmp( _temp1204->name,
_temp1192->name) == 0)? _temp1202 == _temp1190? 1: Cyc_Absyn_qvar_cmp( _temp1202->name,
_temp1190->name) == 0: 0){ Cyc_Tcutil_unify_list( _temp1186, _temp1198); return;}
goto _LL1041; _LL1059: Cyc_Tcutil_unify_it( _temp1218, _temp1232); Cyc_Tcutil_unify_it(
_temp1230, _temp1216); Cyc_Tcutil_unify_tqual( _temp1212, _temp1226);(( void(*)(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_it_conrefs)( Cyc_Tcutil_boundscmp, _temp1210, _temp1224);{
void* _temp1379=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp1210))->v; void* _temp1385; _LL1381: if((
unsigned int) _temp1379 > 1u?*(( int*) _temp1379) == Cyc_Absyn_Eq_constr: 0){
_LL1386: _temp1385=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1379)->f1;
if( _temp1385 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1382;} else{ goto _LL1383;}}
else{ goto _LL1383;} _LL1383: goto _LL1384; _LL1382: return; _LL1384: goto
_LL1380; _LL1380:;}(( void(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)( Cyc_Core_intcmp,
_temp1214, _temp1228); return; _LL1061: if( _temp1238 == _temp1244? _temp1236 ==
_temp1242: 0){ return;} goto _LL1041; _LL1063: return; _LL1065: return; _LL1067:
Cyc_Tcutil_unify_it( _temp1260, _temp1268); Cyc_Tcutil_unify_tqual( _temp1258,
_temp1266); if( _temp1264 == _temp1256){ return;} if( _temp1264 == 0? 1:
_temp1256 == 0){ goto _LL1041;} if( Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1264)) == Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1256))){ return;} goto _LL1041; _LL1069: { int done= 0;{
struct _RegionHandle _temp1387= _new_region(); struct _RegionHandle* rgn=&
_temp1387; _push_region( rgn);{ struct Cyc_List_List* inst= 0; while( _temp1288
!= 0) { if( _temp1308 == 0){ break;} inst=({ struct Cyc_List_List* _temp1388=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1388->hd=( void*)({ struct _tuple5* _temp1389=( struct _tuple5*)
_region_malloc( rgn, sizeof( struct _tuple5)); _temp1389->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp1308))->hd; _temp1389->f2=( void*)({
struct Cyc_Absyn_VarType_struct* _temp1390=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp1390[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp1391; _temp1391.tag= Cyc_Absyn_VarType; _temp1391.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp1288))->hd;
_temp1391;}); _temp1390;}); _temp1389;}); _temp1388->tl= inst; _temp1388;});
_temp1288=(( struct Cyc_List_List*) _check_null( _temp1288))->tl; _temp1308=((
struct Cyc_List_List*) _check_null( _temp1308))->tl;} if( _temp1308 != 0){
_npop_handler( 0u); goto _LL1041;} if( inst != 0){ Cyc_Tcutil_unify_it(( void*)({
struct Cyc_Absyn_FnType_struct* _temp1392=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1392[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1393; _temp1393.tag= Cyc_Absyn_FnType; _temp1393.f1=({ struct Cyc_Absyn_FnInfo
_temp1394; _temp1394.tvars= 0; _temp1394.effect= _temp1286; _temp1394.ret_typ=(
void*) _temp1284; _temp1394.args= _temp1282; _temp1394.c_varargs= _temp1280;
_temp1394.cyc_varargs= _temp1278; _temp1394.rgn_po= _temp1276; _temp1394.attributes=
_temp1274; _temp1394;}); _temp1393;}); _temp1392;}), Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1395=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1395[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1396; _temp1396.tag= Cyc_Absyn_FnType; _temp1396.f1=({ struct Cyc_Absyn_FnInfo
_temp1397; _temp1397.tvars= 0; _temp1397.effect= _temp1306; _temp1397.ret_typ=(
void*) _temp1304; _temp1397.args= _temp1302; _temp1397.c_varargs= _temp1300;
_temp1397.cyc_varargs= _temp1298; _temp1397.rgn_po= _temp1296; _temp1397.attributes=
_temp1294; _temp1397;}); _temp1396;}); _temp1395;}))); done= 1;}}; _pop_region(
rgn);} if( done){ return;} Cyc_Tcutil_unify_it( _temp1284, _temp1304); for( 0;
_temp1282 != 0? _temp1302 != 0: 0;( _temp1282=(( struct Cyc_List_List*)
_check_null( _temp1282))->tl, _temp1302=(( struct Cyc_List_List*) _check_null(
_temp1302))->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp1282))->hd)).f2,(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp1302))->hd)).f2); Cyc_Tcutil_unify_it((*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp1282))->hd)).f3,(*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp1302))->hd)).f3);} if( _temp1282 != 0?
1: _temp1302 != 0){ goto _LL1041;} if( _temp1280 != _temp1300){ goto _LL1041;}{
int bad_cyc_vararg= 0;{ struct _tuple11 _temp1399=({ struct _tuple11 _temp1398;
_temp1398.f1= _temp1278; _temp1398.f2= _temp1298; _temp1398;}); struct Cyc_Absyn_VarargInfo*
_temp1409; struct Cyc_Absyn_VarargInfo* _temp1411; struct Cyc_Absyn_VarargInfo*
_temp1413; struct Cyc_Absyn_VarargInfo* _temp1415; struct Cyc_Absyn_VarargInfo*
_temp1417; struct Cyc_Absyn_VarargInfo* _temp1419; struct Cyc_Absyn_VarargInfo*
_temp1421; struct Cyc_Absyn_VarargInfo _temp1423; int _temp1424; void* _temp1426;
void* _temp1428; struct Cyc_Absyn_Tqual _temp1430; struct Cyc_Core_Opt*
_temp1432; struct Cyc_Absyn_VarargInfo* _temp1434; struct Cyc_Absyn_VarargInfo
_temp1436; int _temp1437; void* _temp1439; void* _temp1441; struct Cyc_Absyn_Tqual
_temp1443; struct Cyc_Core_Opt* _temp1445; _LL1401: _LL1412: _temp1411=
_temp1399.f1; if( _temp1411 == 0){ goto _LL1410;} else{ goto _LL1403;} _LL1410:
_temp1409= _temp1399.f2; if( _temp1409 == 0){ goto _LL1402;} else{ goto _LL1403;}
_LL1403: _LL1416: _temp1415= _temp1399.f1; if( _temp1415 == 0){ goto _LL1414;}
else{ goto _LL1405;} _LL1414: _temp1413= _temp1399.f2; goto _LL1404; _LL1405:
_LL1420: _temp1419= _temp1399.f1; goto _LL1418; _LL1418: _temp1417= _temp1399.f2;
if( _temp1417 == 0){ goto _LL1406;} else{ goto _LL1407;} _LL1407: _LL1435:
_temp1434= _temp1399.f1; if( _temp1434 == 0){ goto _LL1400;} else{ _temp1436=*
_temp1434; _LL1446: _temp1445= _temp1436.name; goto _LL1444; _LL1444: _temp1443=
_temp1436.tq; goto _LL1442; _LL1442: _temp1441=( void*) _temp1436.type; goto
_LL1440; _LL1440: _temp1439=( void*) _temp1436.rgn; goto _LL1438; _LL1438:
_temp1437= _temp1436.inject; goto _LL1422;} _LL1422: _temp1421= _temp1399.f2;
if( _temp1421 == 0){ goto _LL1400;} else{ _temp1423=* _temp1421; _LL1433:
_temp1432= _temp1423.name; goto _LL1431; _LL1431: _temp1430= _temp1423.tq; goto
_LL1429; _LL1429: _temp1428=( void*) _temp1423.type; goto _LL1427; _LL1427:
_temp1426=( void*) _temp1423.rgn; goto _LL1425; _LL1425: _temp1424= _temp1423.inject;
goto _LL1408;} _LL1402: goto _LL1400; _LL1404: goto _LL1406; _LL1406:
bad_cyc_vararg= 1; goto _LL1400; _LL1408: Cyc_Tcutil_unify_tqual( _temp1443,
_temp1430); Cyc_Tcutil_unify_it( _temp1441, _temp1428); Cyc_Tcutil_unify_it(
_temp1439, _temp1426); if( _temp1437 != _temp1424){ bad_cyc_vararg= 1;} goto
_LL1400; _LL1400:;} if( bad_cyc_vararg){ goto _LL1041;}{ int bad_effect= 0;{
struct _tuple12 _temp1448=({ struct _tuple12 _temp1447; _temp1447.f1= _temp1286;
_temp1447.f2= _temp1306; _temp1447;}); struct Cyc_Core_Opt* _temp1458; struct
Cyc_Core_Opt* _temp1460; struct Cyc_Core_Opt* _temp1462; struct Cyc_Core_Opt*
_temp1464; struct Cyc_Core_Opt* _temp1466; struct Cyc_Core_Opt* _temp1468;
_LL1450: _LL1461: _temp1460= _temp1448.f1; if( _temp1460 == 0){ goto _LL1459;}
else{ goto _LL1452;} _LL1459: _temp1458= _temp1448.f2; if( _temp1458 == 0){ goto
_LL1451;} else{ goto _LL1452;} _LL1452: _LL1465: _temp1464= _temp1448.f1; if(
_temp1464 == 0){ goto _LL1463;} else{ goto _LL1454;} _LL1463: _temp1462=
_temp1448.f2; goto _LL1453; _LL1454: _LL1469: _temp1468= _temp1448.f1; goto
_LL1467; _LL1467: _temp1466= _temp1448.f2; if( _temp1466 == 0){ goto _LL1455;}
else{ goto _LL1456;} _LL1456: goto _LL1457; _LL1451: goto _LL1449; _LL1453: goto
_LL1455; _LL1455: bad_effect= 1; goto _LL1449; _LL1457: bad_effect= ! Cyc_Tcutil_unify_effect((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1286))->v,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp1306))->v); goto _LL1449; _LL1449:;} if( bad_effect){ goto
_LL1041;} if( ! Cyc_Tcutil_same_atts( _temp1294, _temp1274)){ goto _LL1041;} if(
! Cyc_Tcutil_same_rgn_po( _temp1296, _temp1276)){ goto _LL1041;} return;}}}
_LL1071: for( 0; _temp1312 != 0? _temp1316 != 0: 0;( _temp1312=(( struct Cyc_List_List*)
_check_null( _temp1312))->tl, _temp1316=(( struct Cyc_List_List*) _check_null(
_temp1316))->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1312))->hd)).f1,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1316))->hd)).f1); Cyc_Tcutil_unify_it((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1312))->hd)).f2,(*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1316))->hd)).f2);} if( _temp1312 == 0?
_temp1316 == 0: 0){ return;} goto _LL1041; _LL1073: _temp1332= _temp1324;
_temp1328= _temp1320; goto _LL1075; _LL1075: for( 0; _temp1328 != 0? _temp1332
!= 0: 0;( _temp1328=(( struct Cyc_List_List*) _check_null( _temp1328))->tl,
_temp1332=(( struct Cyc_List_List*) _check_null( _temp1332))->tl)){ struct Cyc_Absyn_Structfield*
_temp1470=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1328))->hd; struct Cyc_Absyn_Structfield* _temp1471=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp1332))->hd; if( Cyc_String_zstrptrcmp(
_temp1470->name, _temp1471->name) != 0){( int) _throw(( void*) Cyc_Tcutil_Unify);}
Cyc_Tcutil_unify_tqual( _temp1470->tq, _temp1471->tq); Cyc_Tcutil_unify_it((
void*) _temp1470->type,( void*) _temp1471->type); if((( ! Cyc_Tcutil_same_atts(
_temp1470->attributes, _temp1471->attributes)? 1:( _temp1470->width != 0?
_temp1471->width == 0: 0))? 1:( _temp1471->width != 0? _temp1470->width == 0: 0))?
1:(( _temp1470->width != 0? _temp1471->width != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp1470->width))->v)
!= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*)
_check_null( _temp1471->width))->v): 0)){( int) _throw(( void*) Cyc_Tcutil_Unify);}}
if( _temp1328 == 0? _temp1332 == 0: 0){ return;} goto _LL1041; _LL1077: return;
_LL1079: Cyc_Tcutil_unify_it( _temp1344, _temp1340); return; _LL1081: goto
_LL1083; _LL1083: goto _LL1085; _LL1085: goto _LL1087; _LL1087: if( Cyc_Tcutil_unify_effect(
t1, t2)){ return;} else{ goto _LL1041;} _LL1089: goto _LL1041; _LL1041:;}( int)
_throw(( void*) Cyc_Tcutil_Unify);} int Cyc_Tcutil_is_arithmetic_type( void* t){
void* _temp1472= Cyc_Tcutil_compress( t); void* _temp1484; void* _temp1486;
struct Cyc_Absyn_Enumdecl* _temp1488; struct _tuple1* _temp1490; _LL1474: if((
unsigned int) _temp1472 > 4u?*(( int*) _temp1472) == Cyc_Absyn_IntType: 0){
_LL1487: _temp1486=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1472)->f1;
goto _LL1485; _LL1485: _temp1484=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1472)->f2; goto _LL1475;} else{ goto _LL1476;} _LL1476: if( _temp1472 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1477;} else{ goto _LL1478;} _LL1478: if(
_temp1472 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1479;} else{ goto _LL1480;}
_LL1480: if(( unsigned int) _temp1472 > 4u?*(( int*) _temp1472) == Cyc_Absyn_EnumType:
0){ _LL1491: _temp1490=(( struct Cyc_Absyn_EnumType_struct*) _temp1472)->f1;
goto _LL1489; _LL1489: _temp1488=(( struct Cyc_Absyn_EnumType_struct*) _temp1472)->f2;
goto _LL1481;} else{ goto _LL1482;} _LL1482: goto _LL1483; _LL1475: return 1;
_LL1477: return 1; _LL1479: return 1; _LL1481: return 1; _LL1483: return 0;
_LL1473:;} int Cyc_Tcutil_will_lose_precision( void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8 _temp1493=({ struct _tuple8
_temp1492; _temp1492.f1= t1; _temp1492.f2= t2; _temp1492;}); void* _temp1515;
void* _temp1517; void* _temp1519; void* _temp1521; void* _temp1523; void*
_temp1525; void* _temp1527; void* _temp1529; void* _temp1531; void* _temp1533;
void* _temp1535; void* _temp1537; void* _temp1539; void* _temp1541; void*
_temp1543; void* _temp1545; void* _temp1547; void* _temp1549; void* _temp1551;
void* _temp1553; void* _temp1555; void* _temp1557; void* _temp1559; void*
_temp1561; void* _temp1563; void* _temp1565; void* _temp1567; void* _temp1569;
void* _temp1571; void* _temp1573; void* _temp1575; void* _temp1577; void*
_temp1579; void* _temp1581; void* _temp1583; void* _temp1585; void* _temp1587;
void* _temp1589; void* _temp1591; void* _temp1593; void* _temp1595; void*
_temp1597; _LL1495: _LL1518: _temp1517= _temp1493.f1; if( _temp1517 ==( void*)
Cyc_Absyn_DoubleType){ goto _LL1516;} else{ goto _LL1497;} _LL1516: _temp1515=
_temp1493.f2; if( _temp1515 ==( void*) Cyc_Absyn_FloatType){ goto _LL1496;}
else{ goto _LL1497;} _LL1497: _LL1526: _temp1525= _temp1493.f1; if( _temp1525 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL1520;} else{ goto _LL1499;} _LL1520:
_temp1519= _temp1493.f2; if(( unsigned int) _temp1519 > 4u?*(( int*) _temp1519)
== Cyc_Absyn_IntType: 0){ _LL1524: _temp1523=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1519)->f1; goto _LL1522; _LL1522: _temp1521=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1519)->f2; goto _LL1498;} else{ goto _LL1499;} _LL1499: _LL1534: _temp1533=
_temp1493.f1; if( _temp1533 ==( void*) Cyc_Absyn_FloatType){ goto _LL1528;}
else{ goto _LL1501;} _LL1528: _temp1527= _temp1493.f2; if(( unsigned int)
_temp1527 > 4u?*(( int*) _temp1527) == Cyc_Absyn_IntType: 0){ _LL1532: _temp1531=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1527)->f1; goto _LL1530; _LL1530:
_temp1529=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1527)->f2; goto
_LL1500;} else{ goto _LL1501;} _LL1501: _LL1542: _temp1541= _temp1493.f1; if((
unsigned int) _temp1541 > 4u?*(( int*) _temp1541) == Cyc_Absyn_IntType: 0){
_LL1546: _temp1545=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1541)->f1;
goto _LL1544; _LL1544: _temp1543=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1541)->f2; if( _temp1543 ==( void*) Cyc_Absyn_B8){ goto _LL1536;} else{
goto _LL1503;}} else{ goto _LL1503;} _LL1536: _temp1535= _temp1493.f2; if((
unsigned int) _temp1535 > 4u?*(( int*) _temp1535) == Cyc_Absyn_IntType: 0){
_LL1540: _temp1539=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1535)->f1;
goto _LL1538; _LL1538: _temp1537=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1535)->f2; if( _temp1537 ==( void*) Cyc_Absyn_B8){ goto _LL1502;} else{
goto _LL1503;}} else{ goto _LL1503;} _LL1503: _LL1550: _temp1549= _temp1493.f1;
if(( unsigned int) _temp1549 > 4u?*(( int*) _temp1549) == Cyc_Absyn_IntType: 0){
_LL1554: _temp1553=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1549)->f1;
goto _LL1552; _LL1552: _temp1551=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1549)->f2; if( _temp1551 ==( void*) Cyc_Absyn_B8){ goto _LL1548;} else{
goto _LL1505;}} else{ goto _LL1505;} _LL1548: _temp1547= _temp1493.f2; goto
_LL1504; _LL1505: _LL1558: _temp1557= _temp1493.f1; if(( unsigned int) _temp1557
> 4u?*(( int*) _temp1557) == Cyc_Absyn_IntType: 0){ _LL1562: _temp1561=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1557)->f1; goto _LL1560; _LL1560:
_temp1559=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1557)->f2; if(
_temp1559 ==( void*) Cyc_Absyn_B4){ goto _LL1556;} else{ goto _LL1507;}} else{
goto _LL1507;} _LL1556: _temp1555= _temp1493.f2; if( _temp1555 ==( void*) Cyc_Absyn_FloatType){
goto _LL1506;} else{ goto _LL1507;} _LL1507: _LL1570: _temp1569= _temp1493.f1;
if(( unsigned int) _temp1569 > 4u?*(( int*) _temp1569) == Cyc_Absyn_IntType: 0){
_LL1574: _temp1573=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1569)->f1;
goto _LL1572; _LL1572: _temp1571=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1569)->f2; if( _temp1571 ==( void*) Cyc_Absyn_B4){ goto _LL1564;} else{
goto _LL1509;}} else{ goto _LL1509;} _LL1564: _temp1563= _temp1493.f2; if((
unsigned int) _temp1563 > 4u?*(( int*) _temp1563) == Cyc_Absyn_IntType: 0){
_LL1568: _temp1567=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1563)->f1;
goto _LL1566; _LL1566: _temp1565=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1563)->f2; if( _temp1565 ==( void*) Cyc_Absyn_B2){ goto _LL1508;} else{
goto _LL1509;}} else{ goto _LL1509;} _LL1509: _LL1582: _temp1581= _temp1493.f1;
if(( unsigned int) _temp1581 > 4u?*(( int*) _temp1581) == Cyc_Absyn_IntType: 0){
_LL1586: _temp1585=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1581)->f1;
goto _LL1584; _LL1584: _temp1583=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1581)->f2; if( _temp1583 ==( void*) Cyc_Absyn_B4){ goto _LL1576;} else{
goto _LL1511;}} else{ goto _LL1511;} _LL1576: _temp1575= _temp1493.f2; if((
unsigned int) _temp1575 > 4u?*(( int*) _temp1575) == Cyc_Absyn_IntType: 0){
_LL1580: _temp1579=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1575)->f1;
goto _LL1578; _LL1578: _temp1577=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1575)->f2; if( _temp1577 ==( void*) Cyc_Absyn_B1){ goto _LL1510;} else{
goto _LL1511;}} else{ goto _LL1511;} _LL1511: _LL1594: _temp1593= _temp1493.f1;
if(( unsigned int) _temp1593 > 4u?*(( int*) _temp1593) == Cyc_Absyn_IntType: 0){
_LL1598: _temp1597=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1593)->f1;
goto _LL1596; _LL1596: _temp1595=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1593)->f2; if( _temp1595 ==( void*) Cyc_Absyn_B2){ goto _LL1588;} else{
goto _LL1513;}} else{ goto _LL1513;} _LL1588: _temp1587= _temp1493.f2; if((
unsigned int) _temp1587 > 4u?*(( int*) _temp1587) == Cyc_Absyn_IntType: 0){
_LL1592: _temp1591=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1587)->f1;
goto _LL1590; _LL1590: _temp1589=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1587)->f2; if( _temp1589 ==( void*) Cyc_Absyn_B1){ goto _LL1512;} else{
goto _LL1513;}} else{ goto _LL1513;} _LL1513: goto _LL1514; _LL1496: return 1;
_LL1498: return 1; _LL1500: return 1; _LL1502: return 0; _LL1504: return 1;
_LL1506: return 1; _LL1508: return 1; _LL1510: return 1; _LL1512: return 1;
_LL1514: return 0; _LL1494:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{
struct Cyc_List_List* el= es; for( 0; el != 0; el=(( struct Cyc_List_List*)
_check_null( el))->tl){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);
if( Cyc_Tcutil_is_arithmetic_type( t1)){ if( max_arith_type == 0? 1: Cyc_Tcutil_will_lose_precision(
t1,( void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){
max_arith_type=({ struct Cyc_Core_Opt* _temp1599=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1599->v=( void*) t1; _temp1599;});}}}}
if( max_arith_type != 0){ if( ! Cyc_Tcutil_unify( t,( void*)(( struct Cyc_Core_Opt*)
_check_null( max_arith_type))->v)){ return 0;}}{ struct Cyc_List_List* el= es;
for( 0; el != 0; el=(( struct Cyc_List_List*) _check_null( el))->tl){ if( ! Cyc_Tcutil_coerce_assign(
te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd, t)){
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
el))->hd)->loc,( struct _tagged_arr)({ struct _tagged_arr _temp1600= Cyc_Absynpp_typ2string(
t); struct _tagged_arr _temp1601= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);
xprintf("type mismatch: expecting %.*s but found %.*s", _get_arr_size( _temp1600,
1u), _temp1600.curr, _get_arr_size( _temp1601, 1u), _temp1601.curr);})); return
0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ( te, e)){ void*
_temp1602= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
struct Cyc_Absyn_PtrInfo _temp1608; _LL1604: if(( unsigned int) _temp1602 > 4u?*((
int*) _temp1602) == Cyc_Absyn_PointerType: 0){ _LL1609: _temp1608=(( struct Cyc_Absyn_PointerType_struct*)
_temp1602)->f1; goto _LL1605;} else{ goto _LL1606;} _LL1606: goto _LL1607;
_LL1605: Cyc_Tcutil_unchecked_cast( te, e, Cyc_Absyn_uint_t); goto _LL1603;
_LL1607: return 0; _LL1603:;} return 1;} int Cyc_Tcutil_is_integral_type( void*
t){ void* _temp1610= Cyc_Tcutil_compress( t); void* _temp1618; void* _temp1620;
struct Cyc_Absyn_Enumdecl* _temp1622; struct _tuple1* _temp1624; _LL1612: if((
unsigned int) _temp1610 > 4u?*(( int*) _temp1610) == Cyc_Absyn_IntType: 0){
_LL1621: _temp1620=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1610)->f1;
goto _LL1619; _LL1619: _temp1618=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1610)->f2; goto _LL1613;} else{ goto _LL1614;} _LL1614: if(( unsigned int)
_temp1610 > 4u?*(( int*) _temp1610) == Cyc_Absyn_EnumType: 0){ _LL1625:
_temp1624=(( struct Cyc_Absyn_EnumType_struct*) _temp1610)->f1; goto _LL1623;
_LL1623: _temp1622=(( struct Cyc_Absyn_EnumType_struct*) _temp1610)->f2; goto
_LL1615;} else{ goto _LL1616;} _LL1616: goto _LL1617; _LL1613: return 1; _LL1615:
return 1; _LL1617: return 0; _LL1611:;} int Cyc_Tcutil_coerce_uint_typ( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_uint_t)){ return 1;}
if( Cyc_Tcutil_is_integral_type(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){
if( Cyc_Tcutil_will_lose_precision(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, Cyc_Absyn_uint_t)){ Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u));} Cyc_Tcutil_unchecked_cast( te, e, Cyc_Absyn_uint_t);
return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){ if( Cyc_Tcutil_will_lose_precision((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){
Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u));} Cyc_Tcutil_unchecked_cast( te, e, Cyc_Absyn_sint_t);
return 1;} return 0;} int Cyc_Tcutil_silent_castable( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple8 _temp1627=({ struct _tuple8
_temp1626; _temp1626.f1= t1; _temp1626.f2= t2; _temp1626;}); void* _temp1639;
struct Cyc_Absyn_PtrInfo _temp1641; void* _temp1643; struct Cyc_Absyn_PtrInfo
_temp1645; void* _temp1647; struct Cyc_Absyn_Exp* _temp1649; struct Cyc_Absyn_Tqual
_temp1651; void* _temp1653; void* _temp1655; struct Cyc_Absyn_Exp* _temp1657;
struct Cyc_Absyn_Tqual _temp1659; void* _temp1661; void* _temp1663; struct Cyc_Absyn_TunionInfo
_temp1665; void* _temp1667; struct Cyc_List_List* _temp1669; void* _temp1671;
struct Cyc_Absyn_Tuniondecl* _temp1673; void* _temp1675; struct Cyc_Absyn_TunionFieldInfo
_temp1677; struct Cyc_List_List* _temp1679; void* _temp1681; struct Cyc_Absyn_Tunionfield*
_temp1683; struct Cyc_Absyn_Tuniondecl* _temp1685; void* _temp1687; struct Cyc_Absyn_TunionInfo
_temp1689; void* _temp1691; struct Cyc_List_List* _temp1693; void* _temp1695;
struct Cyc_Absyn_Tuniondecl* _temp1697; void* _temp1699; struct Cyc_Absyn_PtrInfo
_temp1701; struct Cyc_Absyn_Conref* _temp1703; struct Cyc_Absyn_Tqual _temp1705;
struct Cyc_Absyn_Conref* _temp1707; void* _temp1709; void* _temp1711; _LL1629:
_LL1644: _temp1643= _temp1627.f1; if(( unsigned int) _temp1643 > 4u?*(( int*)
_temp1643) == Cyc_Absyn_PointerType: 0){ _LL1646: _temp1645=(( struct Cyc_Absyn_PointerType_struct*)
_temp1643)->f1; goto _LL1640;} else{ goto _LL1631;} _LL1640: _temp1639=
_temp1627.f2; if(( unsigned int) _temp1639 > 4u?*(( int*) _temp1639) == Cyc_Absyn_PointerType:
0){ _LL1642: _temp1641=(( struct Cyc_Absyn_PointerType_struct*) _temp1639)->f1;
goto _LL1630;} else{ goto _LL1631;} _LL1631: _LL1656: _temp1655= _temp1627.f1;
if(( unsigned int) _temp1655 > 4u?*(( int*) _temp1655) == Cyc_Absyn_ArrayType: 0){
_LL1662: _temp1661=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1655)->f1;
goto _LL1660; _LL1660: _temp1659=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1655)->f2; goto _LL1658; _LL1658: _temp1657=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1655)->f3; goto _LL1648;} else{ goto _LL1633;} _LL1648: _temp1647=
_temp1627.f2; if(( unsigned int) _temp1647 > 4u?*(( int*) _temp1647) == Cyc_Absyn_ArrayType:
0){ _LL1654: _temp1653=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1647)->f1;
goto _LL1652; _LL1652: _temp1651=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1647)->f2; goto _LL1650; _LL1650: _temp1649=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1647)->f3; goto _LL1632;} else{ goto _LL1633;} _LL1633: _LL1676: _temp1675=
_temp1627.f1; if(( unsigned int) _temp1675 > 4u?*(( int*) _temp1675) == Cyc_Absyn_TunionFieldType:
0){ _LL1678: _temp1677=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1675)->f1;
_LL1682: _temp1681=( void*) _temp1677.field_info; if(*(( int*) _temp1681) == Cyc_Absyn_KnownTunionfield){
_LL1686: _temp1685=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1681)->f1;
goto _LL1684; _LL1684: _temp1683=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1681)->f2; goto _LL1680;} else{ goto _LL1635;} _LL1680: _temp1679=
_temp1677.targs; goto _LL1664;} else{ goto _LL1635;} _LL1664: _temp1663=
_temp1627.f2; if(( unsigned int) _temp1663 > 4u?*(( int*) _temp1663) == Cyc_Absyn_TunionType:
0){ _LL1666: _temp1665=(( struct Cyc_Absyn_TunionType_struct*) _temp1663)->f1;
_LL1672: _temp1671=( void*) _temp1665.tunion_info; if(*(( int*) _temp1671) ==
Cyc_Absyn_KnownTunion){ _LL1674: _temp1673=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1671)->f1; goto _LL1670;} else{ goto _LL1635;} _LL1670: _temp1669=
_temp1665.targs; goto _LL1668; _LL1668: _temp1667=( void*) _temp1665.rgn; goto
_LL1634;} else{ goto _LL1635;} _LL1635: _LL1700: _temp1699= _temp1627.f1; if((
unsigned int) _temp1699 > 4u?*(( int*) _temp1699) == Cyc_Absyn_PointerType: 0){
_LL1702: _temp1701=(( struct Cyc_Absyn_PointerType_struct*) _temp1699)->f1;
_LL1712: _temp1711=( void*) _temp1701.elt_typ; goto _LL1710; _LL1710: _temp1709=(
void*) _temp1701.rgn_typ; goto _LL1708; _LL1708: _temp1707= _temp1701.nullable;
goto _LL1706; _LL1706: _temp1705= _temp1701.tq; goto _LL1704; _LL1704: _temp1703=
_temp1701.bounds; goto _LL1688;} else{ goto _LL1637;} _LL1688: _temp1687=
_temp1627.f2; if(( unsigned int) _temp1687 > 4u?*(( int*) _temp1687) == Cyc_Absyn_TunionType:
0){ _LL1690: _temp1689=(( struct Cyc_Absyn_TunionType_struct*) _temp1687)->f1;
_LL1696: _temp1695=( void*) _temp1689.tunion_info; if(*(( int*) _temp1695) ==
Cyc_Absyn_KnownTunion){ _LL1698: _temp1697=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1695)->f1; goto _LL1694;} else{ goto _LL1637;} _LL1694: _temp1693=
_temp1689.targs; goto _LL1692; _LL1692: _temp1691=( void*) _temp1689.rgn; goto
_LL1636;} else{ goto _LL1637;} _LL1637: goto _LL1638; _LL1630: { int okay= 1;
if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1645.nullable, _temp1641.nullable)){
void* _temp1713=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp1645.nullable))->v; int _temp1719; _LL1715:
if(( unsigned int) _temp1713 > 1u?*(( int*) _temp1713) == Cyc_Absyn_Eq_constr: 0){
_LL1720: _temp1719=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1713)->f1;
goto _LL1716;} else{ goto _LL1717;} _LL1717: goto _LL1718; _LL1716: okay= !
_temp1719; goto _LL1714; _LL1718:( int) _throw((( void*(*)( struct _tagged_arr
msg)) Cyc_Tcutil_impos)( _tag_arr("silent_castable conref not eq", sizeof(
unsigned char), 30u))); _LL1714:;} if( !(( int(*)( int(* cmp)( void*, void*),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp1645.bounds, _temp1641.bounds)){ struct _tuple8
_temp1722=({ struct _tuple8 _temp1721; _temp1721.f1=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1645.bounds))->v;
_temp1721.f2=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp1641.bounds))->v; _temp1721;}); void* _temp1732;
void* _temp1734; void* _temp1736; void* _temp1738; struct Cyc_Absyn_Exp*
_temp1740; void* _temp1742; void* _temp1744; struct Cyc_Absyn_Exp* _temp1746;
void* _temp1748; void* _temp1750; struct Cyc_Absyn_Exp* _temp1752; void*
_temp1754; void* _temp1756; void* _temp1758; void* _temp1760; _LL1724: _LL1737:
_temp1736= _temp1722.f1; if(( unsigned int) _temp1736 > 1u?*(( int*) _temp1736)
== Cyc_Absyn_Eq_constr: 0){ _LL1739: _temp1738=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1736)->f1; if(( unsigned int) _temp1738 > 1u?*(( int*) _temp1738) == Cyc_Absyn_Upper_b:
0){ _LL1741: _temp1740=(( struct Cyc_Absyn_Upper_b_struct*) _temp1738)->f1; goto
_LL1733;} else{ goto _LL1726;}} else{ goto _LL1726;} _LL1733: _temp1732=
_temp1722.f2; if(( unsigned int) _temp1732 > 1u?*(( int*) _temp1732) == Cyc_Absyn_Eq_constr:
0){ _LL1735: _temp1734=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1732)->f1;
if( _temp1734 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1725;} else{ goto _LL1726;}}
else{ goto _LL1726;} _LL1726: _LL1749: _temp1748= _temp1722.f1; if((
unsigned int) _temp1748 > 1u?*(( int*) _temp1748) == Cyc_Absyn_Eq_constr: 0){
_LL1751: _temp1750=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1748)->f1;
if(( unsigned int) _temp1750 > 1u?*(( int*) _temp1750) == Cyc_Absyn_Upper_b: 0){
_LL1753: _temp1752=(( struct Cyc_Absyn_Upper_b_struct*) _temp1750)->f1; goto
_LL1743;} else{ goto _LL1728;}} else{ goto _LL1728;} _LL1743: _temp1742=
_temp1722.f2; if(( unsigned int) _temp1742 > 1u?*(( int*) _temp1742) == Cyc_Absyn_Eq_constr:
0){ _LL1745: _temp1744=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1742)->f1;
if(( unsigned int) _temp1744 > 1u?*(( int*) _temp1744) == Cyc_Absyn_Upper_b: 0){
_LL1747: _temp1746=(( struct Cyc_Absyn_Upper_b_struct*) _temp1744)->f1; goto
_LL1727;} else{ goto _LL1728;}} else{ goto _LL1728;} _LL1728: _LL1759: _temp1758=
_temp1722.f1; if(( unsigned int) _temp1758 > 1u?*(( int*) _temp1758) == Cyc_Absyn_Eq_constr:
0){ _LL1761: _temp1760=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1758)->f1;
if( _temp1760 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1755;} else{ goto _LL1730;}}
else{ goto _LL1730;} _LL1755: _temp1754= _temp1722.f2; if(( unsigned int)
_temp1754 > 1u?*(( int*) _temp1754) == Cyc_Absyn_Eq_constr: 0){ _LL1757:
_temp1756=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1754)->f1; if(
_temp1756 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1729;} else{ goto _LL1730;}}
else{ goto _LL1730;} _LL1730: goto _LL1731; _LL1725: okay= 1; goto _LL1723;
_LL1727: okay= okay? Cyc_Evexp_eval_const_uint_exp( _temp1752) >= Cyc_Evexp_eval_const_uint_exp(
_temp1746): 0; Cyc_Tcutil_warn( loc, _tag_arr("implicit cast to shorter array",
sizeof( unsigned char), 31u)); goto _LL1723; _LL1729: okay= 1; goto _LL1723;
_LL1731: okay= 0; goto _LL1723; _LL1723:;} okay= okay? Cyc_Tcutil_unify(( void*)
_temp1645.elt_typ,( void*) _temp1641.elt_typ): 0; okay= okay? Cyc_Tcutil_unify((
void*) _temp1645.rgn_typ,( void*) _temp1641.rgn_typ): 0; okay= okay? !(
_temp1645.tq).q_const? 1:( _temp1641.tq).q_const: 0; return okay;} _LL1632: {
int okay; okay=( _temp1657 != 0? _temp1649 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1657)) == Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1649)): 0; return( okay? Cyc_Tcutil_unify(
_temp1661, _temp1653): 0)? ! _temp1659.q_const? 1: _temp1651.q_const: 0;}
_LL1634: if(( _temp1685 == _temp1673? 1: Cyc_Absyn_qvar_cmp( _temp1685->name,
_temp1673->name) == 0)? _temp1683->typs == 0: 0){ for( 0; _temp1679 != 0?
_temp1669 != 0: 0;( _temp1679=(( struct Cyc_List_List*) _check_null( _temp1679))->tl,
_temp1669=(( struct Cyc_List_List*) _check_null( _temp1669))->tl)){ if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_List_List*) _check_null( _temp1679))->hd,( void*)(( struct
Cyc_List_List*) _check_null( _temp1669))->hd)){ break;}} if( _temp1679 == 0?
_temp1669 == 0: 0){ return 1;}} return 0; _LL1636:{ void* _temp1762= Cyc_Tcutil_compress(
_temp1711); struct Cyc_Absyn_TunionFieldInfo _temp1768; struct Cyc_List_List*
_temp1770; void* _temp1772; struct Cyc_Absyn_Tunionfield* _temp1774; struct Cyc_Absyn_Tuniondecl*
_temp1776; _LL1764: if(( unsigned int) _temp1762 > 4u?*(( int*) _temp1762) ==
Cyc_Absyn_TunionFieldType: 0){ _LL1769: _temp1768=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1762)->f1; _LL1773: _temp1772=( void*) _temp1768.field_info; if(*(( int*)
_temp1772) == Cyc_Absyn_KnownTunionfield){ _LL1777: _temp1776=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1772)->f1; goto _LL1775; _LL1775: _temp1774=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1772)->f2; goto _LL1771;} else{ goto _LL1766;} _LL1771: _temp1770=
_temp1768.targs; goto _LL1765;} else{ goto _LL1766;} _LL1766: goto _LL1767;
_LL1765: if( ! Cyc_Tcutil_unify( _temp1709, _temp1691)? _temp1709 !=( void*) Cyc_Absyn_HeapRgn:
0){ return 0;} if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1707,(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)( 0))){
return 0;} if( !(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp,
_temp1703,(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)( Cyc_Absyn_bounds_one))){
return 0;} if( Cyc_Absyn_qvar_cmp( _temp1697->name, _temp1776->name) == 0?
_temp1774->typs != 0: 0){ int okay= 1; for( 0; _temp1770 != 0? _temp1693 != 0: 0;(
_temp1770=(( struct Cyc_List_List*) _check_null( _temp1770))->tl, _temp1693=((
struct Cyc_List_List*) _check_null( _temp1693))->tl)){ if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_List_List*) _check_null( _temp1770))->hd,( void*)(( struct
Cyc_List_List*) _check_null( _temp1693))->hd)){ okay= 0; break;}} if(( ! okay? 1:
_temp1770 != 0)? 1: _temp1693 != 0){ return 0;} return 1;} goto _LL1763; _LL1767:
goto _LL1763; _LL1763:;} return 0; _LL1638: return Cyc_Tcutil_unify( t1, t2);
_LL1628:;}} int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} if( Cyc_Tcutil_is_arithmetic_type(
t1)? Cyc_Tcutil_is_arithmetic_type( t2): 0){ if( Cyc_Tcutil_will_lose_precision(
t1, t2)){ Cyc_Tcutil_warn( e->loc, _tag_arr("integral size mismatch; conversion supplied",
sizeof( unsigned char), 44u));} Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;}
else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); Cyc_Tcutil_warn( e->loc,( struct _tagged_arr)({ struct _tagged_arr
_temp1778= Cyc_Absynpp_typ2string( t1); struct _tagged_arr _temp1779= Cyc_Absynpp_typ2string(
t2); xprintf("implicit cast from %.*s to %.*s", _get_arr_size( _temp1778, 1u),
_temp1778.curr, _get_arr_size( _temp1779, 1u), _temp1779.curr);})); return 1;}
else{ return 0;}}}} int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg( te, e, t);}
static int Cyc_Tcutil_any_promote( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e){ void* old_typ= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); void* t;{ void* _temp1780= old_typ; struct Cyc_Absyn_Enumdecl*
_temp1792; struct _tuple1* _temp1794; void* _temp1796; void* _temp1798; _LL1782:
if(( unsigned int) _temp1780 > 4u?*(( int*) _temp1780) == Cyc_Absyn_EnumType: 0){
_LL1795: _temp1794=(( struct Cyc_Absyn_EnumType_struct*) _temp1780)->f1; goto
_LL1793; _LL1793: _temp1792=(( struct Cyc_Absyn_EnumType_struct*) _temp1780)->f2;
goto _LL1783;} else{ goto _LL1784;} _LL1784: if(( unsigned int) _temp1780 > 4u?*((
int*) _temp1780) == Cyc_Absyn_IntType: 0){ _LL1799: _temp1798=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1780)->f1; goto _LL1797; _LL1797: _temp1796=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1780)->f2; goto _LL1785;} else{
goto _LL1786;} _LL1786: if( _temp1780 ==( void*) Cyc_Absyn_FloatType){ goto
_LL1787;} else{ goto _LL1788;} _LL1788: if( _temp1780 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1789;} else{ goto _LL1790;} _LL1790: goto _LL1791; _LL1783: t= Cyc_Absyn_sint_t;
goto _LL1781; _LL1785:{ void* _temp1800= _temp1796; _LL1802: if( _temp1800 ==(
void*) Cyc_Absyn_B1){ goto _LL1803;} else{ goto _LL1804;} _LL1804: if( _temp1800
==( void*) Cyc_Absyn_B2){ goto _LL1805;} else{ goto _LL1806;} _LL1806: if(
_temp1800 ==( void*) Cyc_Absyn_B4){ goto _LL1807;} else{ goto _LL1808;} _LL1808:
if( _temp1800 ==( void*) Cyc_Absyn_B8){ goto _LL1809;} else{ goto _LL1801;}
_LL1803: t= Cyc_Absyn_sint_t; goto _LL1801; _LL1805: t= Cyc_Absyn_sint_t; goto
_LL1801; _LL1807: t= _temp1798 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL1801; _LL1809: t= _temp1798 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_ulonglong_t: Cyc_Absyn_slonglong_t; goto _LL1801; _LL1801:;} goto
_LL1781; _LL1787: t= Cyc_Absyn_float_t; goto _LL1781; _LL1789: t= Cyc_Absyn_double_t;
goto _LL1781; _LL1791: return 0; _LL1781:;} Cyc_Tcutil_unchecked_cast( te, e, t);
return 1;} int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} Cyc_Tcutil_any_promote(
te, e); if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v,
t2)){ return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); Cyc_Tcutil_warn( e->loc,( struct _tagged_arr)({ struct _tagged_arr
_temp1810= Cyc_Absynpp_typ2string( t1); struct _tagged_arr _temp1811= Cyc_Absynpp_typ2string(
t2); xprintf("implicit cast from %.*s to %.*s", _get_arr_size( _temp1810, 1u),
_temp1810.curr, _get_arr_size( _temp1811, 1u), _temp1811.curr);})); return 1;}
else{ return 0;}}}} int Cyc_Tcutil_coerceable( void* t){ void* _temp1812= Cyc_Tcutil_compress(
t); void* _temp1822; void* _temp1824; _LL1814: if(( unsigned int) _temp1812 > 4u?*((
int*) _temp1812) == Cyc_Absyn_IntType: 0){ _LL1825: _temp1824=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1812)->f1; goto _LL1823; _LL1823: _temp1822=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1812)->f2; goto _LL1815;} else{
goto _LL1816;} _LL1816: if( _temp1812 ==( void*) Cyc_Absyn_FloatType){ goto
_LL1817;} else{ goto _LL1818;} _LL1818: if( _temp1812 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1819;} else{ goto _LL1820;} _LL1820: goto _LL1821; _LL1815: return 1;
_LL1817: return 1; _LL1819: return 1; _LL1821: return 0; _LL1813:;} static
struct _tuple4* Cyc_Tcutil_flatten_typ_f( struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield*
x){ return({ struct _tuple4* _temp1826=( struct _tuple4*) GC_malloc( sizeof(
struct _tuple4)); _temp1826->f1= x->tq; _temp1826->f2= Cyc_Tcutil_substitute(
inst,( void*) x->type); _temp1826;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ(
struct Cyc_Tcenv_Tenv* te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void*
_temp1827= t1; struct Cyc_List_List* _temp1839; struct Cyc_Absyn_Structdecl**
_temp1841; struct Cyc_List_List* _temp1843; struct _tuple1* _temp1845; struct
Cyc_List_List* _temp1847; _LL1829: if( _temp1827 ==( void*) Cyc_Absyn_VoidType){
goto _LL1830;} else{ goto _LL1831;} _LL1831: if(( unsigned int) _temp1827 > 4u?*((
int*) _temp1827) == Cyc_Absyn_TupleType: 0){ _LL1840: _temp1839=(( struct Cyc_Absyn_TupleType_struct*)
_temp1827)->f1; goto _LL1832;} else{ goto _LL1833;} _LL1833: if(( unsigned int)
_temp1827 > 4u?*(( int*) _temp1827) == Cyc_Absyn_StructType: 0){ _LL1846:
_temp1845=(( struct Cyc_Absyn_StructType_struct*) _temp1827)->f1; goto _LL1844;
_LL1844: _temp1843=(( struct Cyc_Absyn_StructType_struct*) _temp1827)->f2; goto
_LL1842; _LL1842: _temp1841=(( struct Cyc_Absyn_StructType_struct*) _temp1827)->f3;
goto _LL1834;} else{ goto _LL1835;} _LL1835: if(( unsigned int) _temp1827 > 4u?*((
int*) _temp1827) == Cyc_Absyn_AnonStructType: 0){ _LL1848: _temp1847=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1827)->f1; goto _LL1836;} else{ goto
_LL1837;} _LL1837: goto _LL1838; _LL1830: return 0; _LL1832: return _temp1839;
_LL1834: if( _temp1841 == 0? 1: _temp1845 == 0){ return({ struct Cyc_List_List*
_temp1849=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1849->hd=( void*)({ struct _tuple4* _temp1850=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1850->f1= Cyc_Absyn_empty_tqual(); _temp1850->f2=
t1; _temp1850;}); _temp1849->tl= 0; _temp1849;});}{ struct Cyc_Absyn_Structdecl*
_temp1851=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp1841)); struct
_tuple1 _temp1852=*(( struct _tuple1*) _check_null( _temp1845)); struct Cyc_List_List*
_temp1853=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp1851->tvs, _temp1843); if( _temp1851->fields == 0){
return({ struct Cyc_List_List* _temp1854=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1854->hd=( void*)({ struct _tuple4*
_temp1855=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1855->f1=
Cyc_Absyn_empty_tqual(); _temp1855->f2= t1; _temp1855;}); _temp1854->tl= 0;
_temp1854;});} return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, _temp1853,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1851->fields))->v);}
_LL1836: return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, 0, _temp1847); _LL1838: return({
struct Cyc_List_List* _temp1856=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1856->hd=( void*)({ struct _tuple4* _temp1857=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1857->f1= Cyc_Absyn_empty_tqual();
_temp1857->f2= t1; _temp1857;}); _temp1856->tl= 0; _temp1856;}); _LL1828:;}}
static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2); static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;}{ struct Cyc_List_List* a= assume; for( 0; a != 0; a=(( struct
Cyc_List_List*) _check_null( a))->tl){ if( Cyc_Tcutil_unify( t1,(*(( struct
_tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f1)? Cyc_Tcutil_unify(
t2,(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null( a))->hd)).f2): 0){
return 1;}}} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct
_tuple8 _temp1859=({ struct _tuple8 _temp1858; _temp1858.f1= t1; _temp1858.f2=
t2; _temp1858;}); void* _temp1865; struct Cyc_Absyn_PtrInfo _temp1867; struct
Cyc_Absyn_Conref* _temp1869; struct Cyc_Absyn_Tqual _temp1871; struct Cyc_Absyn_Conref*
_temp1873; void* _temp1875; void* _temp1877; void* _temp1879; struct Cyc_Absyn_PtrInfo
_temp1881; struct Cyc_Absyn_Conref* _temp1883; struct Cyc_Absyn_Tqual _temp1885;
struct Cyc_Absyn_Conref* _temp1887; void* _temp1889; void* _temp1891; _LL1861:
_LL1880: _temp1879= _temp1859.f1; if(( unsigned int) _temp1879 > 4u?*(( int*)
_temp1879) == Cyc_Absyn_PointerType: 0){ _LL1882: _temp1881=(( struct Cyc_Absyn_PointerType_struct*)
_temp1879)->f1; _LL1892: _temp1891=( void*) _temp1881.elt_typ; goto _LL1890;
_LL1890: _temp1889=( void*) _temp1881.rgn_typ; goto _LL1888; _LL1888: _temp1887=
_temp1881.nullable; goto _LL1886; _LL1886: _temp1885= _temp1881.tq; goto _LL1884;
_LL1884: _temp1883= _temp1881.bounds; goto _LL1866;} else{ goto _LL1863;}
_LL1866: _temp1865= _temp1859.f2; if(( unsigned int) _temp1865 > 4u?*(( int*)
_temp1865) == Cyc_Absyn_PointerType: 0){ _LL1868: _temp1867=(( struct Cyc_Absyn_PointerType_struct*)
_temp1865)->f1; _LL1878: _temp1877=( void*) _temp1867.elt_typ; goto _LL1876;
_LL1876: _temp1875=( void*) _temp1867.rgn_typ; goto _LL1874; _LL1874: _temp1873=
_temp1867.nullable; goto _LL1872; _LL1872: _temp1871= _temp1867.tq; goto _LL1870;
_LL1870: _temp1869= _temp1867.bounds; goto _LL1862;} else{ goto _LL1863;}
_LL1863: goto _LL1864; _LL1862: if( _temp1885.q_const? ! _temp1871.q_const: 0){
return 0;} if(( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1887, _temp1873)?(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1887): 0)? !(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1873): 0){ return 0;}{ void* _temp1893= _temp1889; _LL1895: if( _temp1893
==( void*) Cyc_Absyn_HeapRgn){ goto _LL1896;} else{ goto _LL1897;} _LL1897: goto
_LL1898; _LL1896: goto _LL1894; _LL1898: Cyc_Tcutil_unify( _temp1889, _temp1875);
goto _LL1894; _LL1894:;} if( !(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp,
_temp1883, _temp1869)){ struct _tuple8 _temp1900=({ struct _tuple8 _temp1899;
_temp1899.f1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1883); _temp1899.f2=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1869); _temp1899;}); void* _temp1908; void* _temp1910; struct Cyc_Absyn_Exp*
_temp1912; void* _temp1914; struct Cyc_Absyn_Exp* _temp1916; void* _temp1918;
struct Cyc_Absyn_Exp* _temp1920; _LL1902: _LL1911: _temp1910= _temp1900.f1; if((
unsigned int) _temp1910 > 1u?*(( int*) _temp1910) == Cyc_Absyn_Upper_b: 0){
_LL1913: _temp1912=(( struct Cyc_Absyn_Upper_b_struct*) _temp1910)->f1; goto
_LL1909;} else{ goto _LL1904;} _LL1909: _temp1908= _temp1900.f2; if( _temp1908
==( void*) Cyc_Absyn_Unknown_b){ goto _LL1903;} else{ goto _LL1904;} _LL1904:
_LL1919: _temp1918= _temp1900.f1; if(( unsigned int) _temp1918 > 1u?*(( int*)
_temp1918) == Cyc_Absyn_Upper_b: 0){ _LL1921: _temp1920=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1918)->f1; goto _LL1915;} else{ goto _LL1906;} _LL1915: _temp1914=
_temp1900.f2; if(( unsigned int) _temp1914 > 1u?*(( int*) _temp1914) == Cyc_Absyn_Upper_b:
0){ _LL1917: _temp1916=(( struct Cyc_Absyn_Upper_b_struct*) _temp1914)->f1; goto
_LL1905;} else{ goto _LL1906;} _LL1906: goto _LL1907; _LL1903: goto _LL1901;
_LL1905: if( Cyc_Evexp_eval_const_uint_exp( _temp1920) < Cyc_Evexp_eval_const_uint_exp(
_temp1916)){ return 0;} goto _LL1901; _LL1907: return 0; _LL1901:;} return Cyc_Tcutil_ptrsubtype(
te,({ struct Cyc_List_List* _temp1922=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1922->hd=( void*)({ struct _tuple8*
_temp1923=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp1923->f1=
t1; _temp1923->f2= t2; _temp1923;}); _temp1922->tl= assume; _temp1922;}),
_temp1891, _temp1877); _LL1864: return 0; _LL1860:;}} static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* assume, void* t1, void* t2){
struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ( te, t1); struct Cyc_List_List*
tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0; tqs2 != 0;( tqs2=(( struct Cyc_List_List*)
_check_null( tqs2))->tl, tqs1=(( struct Cyc_List_List*) _check_null( tqs1))->tl)){
if( tqs1 == 0){ return 0;}{ struct _tuple4 _temp1926; void* _temp1927; struct
Cyc_Absyn_Tqual _temp1929; struct _tuple4* _temp1924=( struct _tuple4*)(( struct
Cyc_List_List*) _check_null( tqs1))->hd; _temp1926=* _temp1924; _LL1930:
_temp1929= _temp1926.f1; goto _LL1928; _LL1928: _temp1927= _temp1926.f2; goto
_LL1925; _LL1925: { struct _tuple4 _temp1933; void* _temp1934; struct Cyc_Absyn_Tqual
_temp1936; struct _tuple4* _temp1931=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( tqs2))->hd; _temp1933=* _temp1931; _LL1937: _temp1936= _temp1933.f1;
goto _LL1935; _LL1935: _temp1934= _temp1933.f2; goto _LL1932; _LL1932: if(
_temp1936.q_const? Cyc_Tcutil_subtype( te, assume, _temp1927, _temp1934): 0){
continue;} else{ if( Cyc_Tcutil_unify( _temp1927, _temp1934)){ continue;} else{
return 0;}}}}} return 1;} int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
void* _temp1938= t2; struct Cyc_Absyn_Uniondecl** _temp1946; struct Cyc_List_List*
_temp1948; struct _tuple1* _temp1950; void* _temp1952; void* _temp1954; _LL1940:
if(( unsigned int) _temp1938 > 4u?*(( int*) _temp1938) == Cyc_Absyn_UnionType: 0){
_LL1951: _temp1950=(( struct Cyc_Absyn_UnionType_struct*) _temp1938)->f1; goto
_LL1949; _LL1949: _temp1948=(( struct Cyc_Absyn_UnionType_struct*) _temp1938)->f2;
goto _LL1947; _LL1947: _temp1946=(( struct Cyc_Absyn_UnionType_struct*)
_temp1938)->f3; goto _LL1941;} else{ goto _LL1942;} _LL1942: if(( unsigned int)
_temp1938 > 4u?*(( int*) _temp1938) == Cyc_Absyn_IntType: 0){ _LL1955: _temp1954=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1938)->f1; goto _LL1953; _LL1953:
_temp1952=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1938)->f2; if(
_temp1952 ==( void*) Cyc_Absyn_B4){ goto _LL1943;} else{ goto _LL1944;}} else{
goto _LL1944;} _LL1944: goto _LL1945; _LL1941: if( _temp1946 == 0){ goto _LL1939;}{
struct Cyc_Absyn_Uniondecl* _temp1956=*(( struct Cyc_Absyn_Uniondecl**)
_check_null( _temp1946)); if( _temp1956->fields == 0){ goto _LL1939;}{ struct
Cyc_List_List* _temp1957=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_zip)( _temp1956->tvs, _temp1948);{ struct Cyc_List_List*
fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1956->fields))->v;
for( 0; fs != 0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){ if( Cyc_Tcutil_unify(
t1, Cyc_Tcutil_substitute( _temp1957,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( fs))->hd)->type))){ return 1;}}} goto
_LL1939;}} _LL1943: if( Cyc_Tcutil_typ_kind( t2) ==( void*) Cyc_Absyn_BoxKind){
return 1;} goto _LL1939; _LL1945: goto _LL1939; _LL1939:;}{ void* _temp1958= t1;
struct Cyc_Absyn_PtrInfo _temp1974; struct Cyc_Absyn_Conref* _temp1976; struct
Cyc_Absyn_Tqual _temp1978; struct Cyc_Absyn_Conref* _temp1980; void* _temp1982;
void* _temp1984; struct Cyc_Absyn_Exp* _temp1986; struct Cyc_Absyn_Tqual
_temp1988; void* _temp1990; struct Cyc_Absyn_Enumdecl* _temp1992; struct _tuple1*
_temp1994; void* _temp1996; void* _temp1998; _LL1960: if(( unsigned int)
_temp1958 > 4u?*(( int*) _temp1958) == Cyc_Absyn_PointerType: 0){ _LL1975:
_temp1974=(( struct Cyc_Absyn_PointerType_struct*) _temp1958)->f1; _LL1985:
_temp1984=( void*) _temp1974.elt_typ; goto _LL1983; _LL1983: _temp1982=( void*)
_temp1974.rgn_typ; goto _LL1981; _LL1981: _temp1980= _temp1974.nullable; goto
_LL1979; _LL1979: _temp1978= _temp1974.tq; goto _LL1977; _LL1977: _temp1976=
_temp1974.bounds; goto _LL1961;} else{ goto _LL1962;} _LL1962: if(( unsigned int)
_temp1958 > 4u?*(( int*) _temp1958) == Cyc_Absyn_ArrayType: 0){ _LL1991:
_temp1990=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1958)->f1; goto
_LL1989; _LL1989: _temp1988=(( struct Cyc_Absyn_ArrayType_struct*) _temp1958)->f2;
goto _LL1987; _LL1987: _temp1986=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1958)->f3; goto _LL1963;} else{ goto _LL1964;} _LL1964: if(( unsigned int)
_temp1958 > 4u?*(( int*) _temp1958) == Cyc_Absyn_EnumType: 0){ _LL1995:
_temp1994=(( struct Cyc_Absyn_EnumType_struct*) _temp1958)->f1; goto _LL1993;
_LL1993: _temp1992=(( struct Cyc_Absyn_EnumType_struct*) _temp1958)->f2; goto
_LL1965;} else{ goto _LL1966;} _LL1966: if(( unsigned int) _temp1958 > 4u?*((
int*) _temp1958) == Cyc_Absyn_IntType: 0){ _LL1999: _temp1998=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1958)->f1; goto _LL1997; _LL1997: _temp1996=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1958)->f2; goto _LL1967;} else{
goto _LL1968;} _LL1968: if( _temp1958 ==( void*) Cyc_Absyn_FloatType){ goto
_LL1969;} else{ goto _LL1970;} _LL1970: if( _temp1958 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1971;} else{ goto _LL1972;} _LL1972: goto _LL1973; _LL1961:{ void*
_temp2000= t2; struct Cyc_Absyn_PtrInfo _temp2006; struct Cyc_Absyn_Conref*
_temp2008; struct Cyc_Absyn_Tqual _temp2010; struct Cyc_Absyn_Conref* _temp2012;
void* _temp2014; void* _temp2016; _LL2002: if(( unsigned int) _temp2000 > 4u?*((
int*) _temp2000) == Cyc_Absyn_PointerType: 0){ _LL2007: _temp2006=(( struct Cyc_Absyn_PointerType_struct*)
_temp2000)->f1; _LL2017: _temp2016=( void*) _temp2006.elt_typ; goto _LL2015;
_LL2015: _temp2014=( void*) _temp2006.rgn_typ; goto _LL2013; _LL2013: _temp2012=
_temp2006.nullable; goto _LL2011; _LL2011: _temp2010= _temp2006.tq; goto _LL2009;
_LL2009: _temp2008= _temp2006.bounds; goto _LL2003;} else{ goto _LL2004;}
_LL2004: goto _LL2005; _LL2003: { struct Cyc_List_List* _temp2018=({ struct Cyc_List_List*
_temp2035=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2035->hd=( void*)({ struct _tuple8* _temp2036=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp2036->f1= t1; _temp2036->f2= t2; _temp2036;});
_temp2035->tl= 0; _temp2035;}); int _temp2019= Cyc_Tcutil_ptrsubtype( te,
_temp2018, _temp1984, _temp2016)? ! _temp1978.q_const? 1: _temp2010.q_const: 0;
int bounds_ok=(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp,
_temp1976, _temp2008); if( ! bounds_ok){ struct _tuple8 _temp2021=({ struct
_tuple8 _temp2020; _temp2020.f1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1976); _temp2020.f2=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp2008); _temp2020;}); void* _temp2027; struct Cyc_Absyn_Exp* _temp2029; void*
_temp2031; struct Cyc_Absyn_Exp* _temp2033; _LL2023: _LL2032: _temp2031=
_temp2021.f1; if(( unsigned int) _temp2031 > 1u?*(( int*) _temp2031) == Cyc_Absyn_Upper_b:
0){ _LL2034: _temp2033=(( struct Cyc_Absyn_Upper_b_struct*) _temp2031)->f1; goto
_LL2028;} else{ goto _LL2025;} _LL2028: _temp2027= _temp2021.f2; if((
unsigned int) _temp2027 > 1u?*(( int*) _temp2027) == Cyc_Absyn_Upper_b: 0){
_LL2030: _temp2029=(( struct Cyc_Absyn_Upper_b_struct*) _temp2027)->f1; goto
_LL2024;} else{ goto _LL2025;} _LL2025: goto _LL2026; _LL2024: if( Cyc_Evexp_eval_const_uint_exp(
_temp2033) >= Cyc_Evexp_eval_const_uint_exp( _temp2029)){ bounds_ok= 1;} goto
_LL2022; _LL2026: bounds_ok= 1; goto _LL2022; _LL2022:;} return( bounds_ok?
_temp2019: 0)? Cyc_Tcutil_unify( _temp1982, _temp2014)? 1: Cyc_Tcenv_region_outlives(
te, _temp1982, _temp2014): 0;} _LL2005: goto _LL2001; _LL2001:;} return 0;
_LL1963:{ void* _temp2037= t2; struct Cyc_Absyn_Exp* _temp2043; struct Cyc_Absyn_Tqual
_temp2045; void* _temp2047; _LL2039: if(( unsigned int) _temp2037 > 4u?*(( int*)
_temp2037) == Cyc_Absyn_ArrayType: 0){ _LL2048: _temp2047=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2037)->f1; goto _LL2046; _LL2046: _temp2045=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2037)->f2; goto _LL2044; _LL2044: _temp2043=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2037)->f3; goto _LL2040;} else{ goto _LL2041;} _LL2041: goto _LL2042;
_LL2040: { int okay; okay=( _temp1986 != 0? _temp2043 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1986)) >= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2043)): 0; return( okay? Cyc_Tcutil_unify(
_temp1990, _temp2047): 0)? ! _temp1988.q_const? 1: _temp2045.q_const: 0;}
_LL2042: return 0; _LL2038:;} return 0; _LL1965:{ void* _temp2049= t2; struct
Cyc_Absyn_Enumdecl* _temp2055; struct _tuple1* _temp2057; _LL2051: if((
unsigned int) _temp2049 > 4u?*(( int*) _temp2049) == Cyc_Absyn_EnumType: 0){
_LL2058: _temp2057=(( struct Cyc_Absyn_EnumType_struct*) _temp2049)->f1; goto
_LL2056; _LL2056: _temp2055=(( struct Cyc_Absyn_EnumType_struct*) _temp2049)->f2;
goto _LL2052;} else{ goto _LL2053;} _LL2053: goto _LL2054; _LL2052: if((((
struct Cyc_Absyn_Enumdecl*) _check_null( _temp1992))->fields != 0?(( struct Cyc_Absyn_Enumdecl*)
_check_null( _temp2055))->fields != 0: 0)?(( int(*)( struct Cyc_List_List* x))
Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(((
struct Cyc_Absyn_Enumdecl*) _check_null( _temp1992))->fields))->v) >=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Enumdecl*) _check_null( _temp2055))->fields))->v):
0){ return 1;} goto _LL2050; _LL2054: goto _LL2050; _LL2050:;} goto _LL1967;
_LL1967: goto _LL1969; _LL1969: goto _LL1971; _LL1971: return Cyc_Tcutil_coerceable(
t2); _LL1973: return 0; _LL1959:;}} void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, void* t){ if( ! Cyc_Tcutil_unify(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v, t)){ struct Cyc_Absyn_Exp* _temp2059=
Cyc_Absyn_copy_exp( e);( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp2060=( struct Cyc_Absyn_Cast_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp2060[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp2061; _temp2061.tag= Cyc_Absyn_Cast_e;
_temp2061.f1=( void*) t; _temp2061.f2= _temp2059; _temp2061;}); _temp2060;})));
e->topt=({ struct Cyc_Core_Opt* _temp2062=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp2062->v=( void*) t; _temp2062;});}} int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp* e){ void* _temp2063= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v); struct Cyc_Absyn_Enumdecl* _temp2073;
struct _tuple1* _temp2075; void* _temp2077; void* _temp2079; struct Cyc_Core_Opt*
_temp2081; int _temp2083; struct Cyc_Core_Opt* _temp2085; struct Cyc_Core_Opt*
_temp2087; _LL2065: if(( unsigned int) _temp2063 > 4u?*(( int*) _temp2063) ==
Cyc_Absyn_EnumType: 0){ _LL2076: _temp2075=(( struct Cyc_Absyn_EnumType_struct*)
_temp2063)->f1; goto _LL2074; _LL2074: _temp2073=(( struct Cyc_Absyn_EnumType_struct*)
_temp2063)->f2; goto _LL2066;} else{ goto _LL2067;} _LL2067: if(( unsigned int)
_temp2063 > 4u?*(( int*) _temp2063) == Cyc_Absyn_IntType: 0){ _LL2080: _temp2079=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2063)->f1; goto _LL2078; _LL2078:
_temp2077=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2063)->f2; goto
_LL2068;} else{ goto _LL2069;} _LL2069: if(( unsigned int) _temp2063 > 4u?*((
int*) _temp2063) == Cyc_Absyn_Evar: 0){ _LL2088: _temp2087=(( struct Cyc_Absyn_Evar_struct*)
_temp2063)->f1; goto _LL2086; _LL2086: _temp2085=(( struct Cyc_Absyn_Evar_struct*)
_temp2063)->f2; goto _LL2084; _LL2084: _temp2083=(( struct Cyc_Absyn_Evar_struct*)
_temp2063)->f3; goto _LL2082; _LL2082: _temp2081=(( struct Cyc_Absyn_Evar_struct*)
_temp2063)->f4; goto _LL2070;} else{ goto _LL2071;} _LL2071: goto _LL2072;
_LL2066: goto _LL2068; _LL2068: return 1; _LL2070: if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;}
return 0; _LL2072: return 0; _LL2064:;} int Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp*
e){ if( Cyc_Tcutil_is_integral( e)){ return 1;}{ void* _temp2089= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _LL2091: if(
_temp2089 ==( void*) Cyc_Absyn_FloatType){ goto _LL2092;} else{ goto _LL2093;}
_LL2093: if( _temp2089 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2094;} else{
goto _LL2095;} _LL2095: goto _LL2096; _LL2092: goto _LL2094; _LL2094: return 1;
_LL2096: return 0; _LL2090:;}} int Cyc_Tcutil_is_function_type( void* t){ void*
_temp2097= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp2103; _LL2099:
if(( unsigned int) _temp2097 > 4u?*(( int*) _temp2097) == Cyc_Absyn_FnType: 0){
_LL2104: _temp2103=(( struct Cyc_Absyn_FnType_struct*) _temp2097)->f1; goto
_LL2100;} else{ goto _LL2101;} _LL2101: goto _LL2102; _LL2100: return 1; _LL2102:
return 0; _LL2098:;} void* Cyc_Tcutil_max_arithmetic_type( void* t1, void* t2){
struct _tuple8 _temp2106=({ struct _tuple8 _temp2105; _temp2105.f1= t1;
_temp2105.f2= t2; _temp2105;}); void* _temp2130; void* _temp2132; void*
_temp2134; void* _temp2136; void* _temp2138; void* _temp2140; void* _temp2142;
void* _temp2144; void* _temp2146; void* _temp2148; void* _temp2150; void*
_temp2152; void* _temp2154; void* _temp2156; void* _temp2158; void* _temp2160;
void* _temp2162; void* _temp2164; void* _temp2166; void* _temp2168; void*
_temp2170; void* _temp2172; void* _temp2174; void* _temp2176; void* _temp2178;
void* _temp2180; void* _temp2182; void* _temp2184; void* _temp2186; void*
_temp2188; void* _temp2190; void* _temp2192; _LL2108: _LL2133: _temp2132=
_temp2106.f1; if( _temp2132 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2131;}
else{ goto _LL2110;} _LL2131: _temp2130= _temp2106.f2; goto _LL2109; _LL2110:
_LL2137: _temp2136= _temp2106.f1; goto _LL2135; _LL2135: _temp2134= _temp2106.f2;
if( _temp2134 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2111;} else{ goto
_LL2112;} _LL2112: _LL2141: _temp2140= _temp2106.f1; if( _temp2140 ==( void*)
Cyc_Absyn_FloatType){ goto _LL2139;} else{ goto _LL2114;} _LL2139: _temp2138=
_temp2106.f2; goto _LL2113; _LL2114: _LL2145: _temp2144= _temp2106.f1; goto
_LL2143; _LL2143: _temp2142= _temp2106.f2; if( _temp2142 ==( void*) Cyc_Absyn_FloatType){
goto _LL2115;} else{ goto _LL2116;} _LL2116: _LL2149: _temp2148= _temp2106.f1;
if(( unsigned int) _temp2148 > 4u?*(( int*) _temp2148) == Cyc_Absyn_IntType: 0){
_LL2153: _temp2152=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2148)->f1;
if( _temp2152 ==( void*) Cyc_Absyn_Unsigned){ goto _LL2151;} else{ goto _LL2118;}
_LL2151: _temp2150=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2148)->f2;
if( _temp2150 ==( void*) Cyc_Absyn_B8){ goto _LL2147;} else{ goto _LL2118;}}
else{ goto _LL2118;} _LL2147: _temp2146= _temp2106.f2; goto _LL2117; _LL2118:
_LL2161: _temp2160= _temp2106.f1; goto _LL2155; _LL2155: _temp2154= _temp2106.f2;
if(( unsigned int) _temp2154 > 4u?*(( int*) _temp2154) == Cyc_Absyn_IntType: 0){
_LL2159: _temp2158=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2154)->f1;
if( _temp2158 ==( void*) Cyc_Absyn_Unsigned){ goto _LL2157;} else{ goto _LL2120;}
_LL2157: _temp2156=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2154)->f2;
if( _temp2156 ==( void*) Cyc_Absyn_B8){ goto _LL2119;} else{ goto _LL2120;}}
else{ goto _LL2120;} _LL2120: _LL2165: _temp2164= _temp2106.f1; if((
unsigned int) _temp2164 > 4u?*(( int*) _temp2164) == Cyc_Absyn_IntType: 0){
_LL2169: _temp2168=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2164)->f1;
if( _temp2168 ==( void*) Cyc_Absyn_Signed){ goto _LL2167;} else{ goto _LL2122;}
_LL2167: _temp2166=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2164)->f2;
if( _temp2166 ==( void*) Cyc_Absyn_B8){ goto _LL2163;} else{ goto _LL2122;}}
else{ goto _LL2122;} _LL2163: _temp2162= _temp2106.f2; goto _LL2121; _LL2122:
_LL2177: _temp2176= _temp2106.f1; goto _LL2171; _LL2171: _temp2170= _temp2106.f2;
if(( unsigned int) _temp2170 > 4u?*(( int*) _temp2170) == Cyc_Absyn_IntType: 0){
_LL2175: _temp2174=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2170)->f1;
if( _temp2174 ==( void*) Cyc_Absyn_Signed){ goto _LL2173;} else{ goto _LL2124;}
_LL2173: _temp2172=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2170)->f2;
if( _temp2172 ==( void*) Cyc_Absyn_B8){ goto _LL2123;} else{ goto _LL2124;}}
else{ goto _LL2124;} _LL2124: _LL2181: _temp2180= _temp2106.f1; if((
unsigned int) _temp2180 > 4u?*(( int*) _temp2180) == Cyc_Absyn_IntType: 0){
_LL2185: _temp2184=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2180)->f1;
if( _temp2184 ==( void*) Cyc_Absyn_Unsigned){ goto _LL2183;} else{ goto _LL2126;}
_LL2183: _temp2182=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2180)->f2;
if( _temp2182 ==( void*) Cyc_Absyn_B4){ goto _LL2179;} else{ goto _LL2126;}}
else{ goto _LL2126;} _LL2179: _temp2178= _temp2106.f2; goto _LL2125; _LL2126:
_LL2193: _temp2192= _temp2106.f1; goto _LL2187; _LL2187: _temp2186= _temp2106.f2;
if(( unsigned int) _temp2186 > 4u?*(( int*) _temp2186) == Cyc_Absyn_IntType: 0){
_LL2191: _temp2190=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2186)->f1;
if( _temp2190 ==( void*) Cyc_Absyn_Unsigned){ goto _LL2189;} else{ goto _LL2128;}
_LL2189: _temp2188=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2186)->f2;
if( _temp2188 ==( void*) Cyc_Absyn_B4){ goto _LL2127;} else{ goto _LL2128;}}
else{ goto _LL2128;} _LL2128: goto _LL2129; _LL2109: goto _LL2111; _LL2111:
return( void*) Cyc_Absyn_DoubleType; _LL2113: goto _LL2115; _LL2115: return(
void*) Cyc_Absyn_FloatType; _LL2117: goto _LL2119; _LL2119: return Cyc_Absyn_ulonglong_t;
_LL2121: goto _LL2123; _LL2123: return Cyc_Absyn_slonglong_t; _LL2125: goto
_LL2127; _LL2127: return Cyc_Absyn_uint_t; _LL2129: return Cyc_Absyn_sint_t;
_LL2107:;} void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp* e){ void*
_temp2194=( void*) e->r; struct Cyc_Absyn_Exp* _temp2200; struct Cyc_Core_Opt*
_temp2202; struct Cyc_Absyn_Exp* _temp2204; _LL2196: if(*(( int*) _temp2194) ==
Cyc_Absyn_AssignOp_e){ _LL2205: _temp2204=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp2194)->f1; goto _LL2203; _LL2203: _temp2202=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp2194)->f2; if( _temp2202 == 0){ goto _LL2201;} else{ goto _LL2198;} _LL2201:
_temp2200=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp2194)->f3; goto _LL2197;}
else{ goto _LL2198;} _LL2198: goto _LL2199; _LL2197: Cyc_Tcutil_warn( e->loc,
_tag_arr("assignment in test", sizeof( unsigned char), 19u)); goto _LL2195;
_LL2199: goto _LL2195; _LL2195:;} static int Cyc_Tcutil_constrain_kinds( struct
Cyc_Absyn_Conref* c1, struct Cyc_Absyn_Conref* c2){ c1=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( c1); c2=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( c2); if( c1 == c2){
return 1;} if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr){( void*)( c1->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2206=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2206[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2207; _temp2207.tag= Cyc_Absyn_Forward_constr;
_temp2207.f1= c2; _temp2207;}); _temp2206;}))); return 1;} else{ if(( void*) c2->v
==( void*) Cyc_Absyn_No_constr){( void*)( c2->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp2208=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp2208[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp2209; _temp2209.tag= Cyc_Absyn_Forward_constr; _temp2209.f1= c1; _temp2209;});
_temp2208;}))); return 1;} else{ void* k1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( c1); void* k2=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( c2); if( Cyc_Tcutil_kind_leq( k1, k2)){( void*)( c2->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp2210=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp2210[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp2211; _temp2211.tag= Cyc_Absyn_Forward_constr;
_temp2211.f1= c1; _temp2211;}); _temp2210;}))); return 1;} else{ if( Cyc_Tcutil_kind_leq(
k2, k1)){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp2212=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp2212[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp2213; _temp2213.tag= Cyc_Absyn_Forward_constr; _temp2213.f1= c2; _temp2213;});
_temp2212;}))); return 1;} else{ return 0;}}}}} static int Cyc_Tcutil_tvar_id_counter=
0; int* Cyc_Tcutil_new_tvar_id(){ return({ int* _temp2214=( int*)
GC_malloc_atomic( sizeof( int)); _temp2214[ 0]= Cyc_Tcutil_tvar_id_counter ++;
_temp2214;});} static int Cyc_Tcutil_tvar_counter= 0; struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar(
void* k){ int i= Cyc_Tcutil_tvar_counter ++; struct _tagged_arr s=( struct
_tagged_arr) xprintf("#%d", i); return({ struct Cyc_Absyn_Tvar* _temp2215=(
struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp2215->name=({
struct _tagged_arr* _temp2216=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp2216[ 0]= s; _temp2216;}); _temp2215->identity= 0;
_temp2215->kind=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
k); _temp2215;});} int Cyc_Tcutil_is_temp_tvar( struct Cyc_Absyn_Tvar* t){
struct _tagged_arr _temp2217=* t->name; return*(( const unsigned char*)
_check_unknown_subscript( _temp2217, sizeof( unsigned char), 0)) =='#';} void
Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar* t){ if( ! Cyc_Tcutil_is_temp_tvar(
t)){ return;}{ struct _tagged_arr _temp2218= Cyc_String_strconcat( _tag_arr("`",
sizeof( unsigned char), 2u),* t->name);*(( unsigned char*)
_check_unknown_subscript( _temp2218, sizeof( unsigned char), 1))='t'; t->name=({
struct _tagged_arr* _temp2219=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp2219[ 0]=( struct _tagged_arr) _temp2218; _temp2219;});}}
struct _tuple13{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual f2; void* f3; }
; static struct _tuple2* Cyc_Tcutil_fndecl2typ_f( struct _tuple13* x){ return({
struct _tuple2* _temp2220=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2220->f1=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp2221=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2221->v=( void*)(*
x).f1; _temp2221;}); _temp2220->f2=(* x).f2; _temp2220->f3=(* x).f3; _temp2220;});}
void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl* fd){ if( fd->cached_typ ==
0){ struct Cyc_List_List* _temp2222= 0;{ struct Cyc_List_List* atts= fd->attributes;
for( 0; atts != 0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ if(
Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*) _check_null( atts))->hd)){
_temp2222=({ struct Cyc_List_List* _temp2223=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2223->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); _temp2223->tl= _temp2222; _temp2223;});}}} return(
void*)({ struct Cyc_Absyn_FnType_struct* _temp2224=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2224[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2225; _temp2225.tag= Cyc_Absyn_FnType; _temp2225.f1=({ struct Cyc_Absyn_FnInfo
_temp2226; _temp2226.tvars= fd->tvs; _temp2226.effect= fd->effect; _temp2226.ret_typ=(
void*)(( void*) fd->ret_type); _temp2226.args=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple13*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_fndecl2typ_f, fd->args); _temp2226.c_varargs= fd->c_varargs;
_temp2226.cyc_varargs= fd->cyc_varargs; _temp2226.rgn_po= fd->rgn_po; _temp2226.attributes=
_temp2222; _temp2226;}); _temp2225;}); _temp2224;});} return( void*)(( struct
Cyc_Core_Opt*) _check_null( fd->cached_typ))->v;} struct _tuple14{ void* f1;
struct Cyc_Absyn_Tqual f2; void* f3; } ; static void* Cyc_Tcutil_fst_fdarg(
struct _tuple14* t){ return(* t).f1;} void* Cyc_Tcutil_snd_tqt( struct _tuple4*
t){ return(* t).f2;} static struct _tuple4* Cyc_Tcutil_map2_tq( struct _tuple4*
pr, void* t){ return({ struct _tuple4* _temp2227=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp2227->f1=(* pr).f1; _temp2227->f2= t; _temp2227;});}
void Cyc_Tcutil_check_fndecl_valid_type( struct Cyc_Position_Segment* loc,
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl* fd){ void* t= Cyc_Tcutil_fndecl2typ(
fd); Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);{ void* _temp2228= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_FnInfo _temp2234; struct Cyc_List_List* _temp2236; struct
Cyc_List_List* _temp2238; struct Cyc_Absyn_VarargInfo* _temp2240; int _temp2242;
struct Cyc_List_List* _temp2244; void* _temp2246; struct Cyc_Core_Opt* _temp2248;
struct Cyc_List_List* _temp2250; _LL2230: if(( unsigned int) _temp2228 > 4u?*((
int*) _temp2228) == Cyc_Absyn_FnType: 0){ _LL2235: _temp2234=(( struct Cyc_Absyn_FnType_struct*)
_temp2228)->f1; _LL2251: _temp2250= _temp2234.tvars; goto _LL2249; _LL2249:
_temp2248= _temp2234.effect; goto _LL2247; _LL2247: _temp2246=( void*) _temp2234.ret_typ;
goto _LL2245; _LL2245: _temp2244= _temp2234.args; goto _LL2243; _LL2243:
_temp2242= _temp2234.c_varargs; goto _LL2241; _LL2241: _temp2240= _temp2234.cyc_varargs;
goto _LL2239; _LL2239: _temp2238= _temp2234.rgn_po; goto _LL2237; _LL2237:
_temp2236= _temp2234.attributes; goto _LL2231;} else{ goto _LL2232;} _LL2232:
goto _LL2233; _LL2231: fd->tvs= _temp2250; fd->effect= _temp2248; goto _LL2229;
_LL2233:(( int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)( _tag_arr("check_fndecl_valid_type: not a FnType",
sizeof( unsigned char), 38u)); return; _LL2229:;}{ struct _RegionHandle
_temp2252= _new_region(); struct _RegionHandle* r=& _temp2252; _push_region( r);
Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tagged_arr*(* f)( struct _tuple13*), struct Cyc_List_List* x)) Cyc_List_rmap)(
r,( struct _tagged_arr*(*)( struct _tuple13* t)) Cyc_Tcutil_fst_fdarg, fd->args),
loc, _tag_arr("function declaration has repeated parameter", sizeof(
unsigned char), 44u));; _pop_region( r);} fd->cached_typ=({ struct Cyc_Core_Opt*
_temp2253=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2253->v=( void*) t; _temp2253;});} struct _tuple15{ struct Cyc_Core_Opt* f1;
struct Cyc_Absyn_Tqual f2; } ; struct _tuple16{ struct _tuple15* f1; void* f2; }
; static struct _tuple16* Cyc_Tcutil_substitute_f1( struct _RegionHandle* rgn,
struct _tuple2* y){ return({ struct _tuple16* _temp2254=( struct _tuple16*)
_region_malloc( rgn, sizeof( struct _tuple16)); _temp2254->f1=({ struct _tuple15*
_temp2255=( struct _tuple15*) _region_malloc( rgn, sizeof( struct _tuple15));
_temp2255->f1=(* y).f1; _temp2255->f2=(* y).f2; _temp2255;}); _temp2254->f2=(* y).f3;
_temp2254;});} static struct _tuple2* Cyc_Tcutil_substitute_f2( struct _tuple16*
w){ void* _temp2258; struct _tuple15* _temp2260; struct _tuple16 _temp2256=* w;
_LL2261: _temp2260= _temp2256.f1; goto _LL2259; _LL2259: _temp2258= _temp2256.f2;
goto _LL2257; _LL2257: { struct Cyc_Absyn_Tqual _temp2264; struct Cyc_Core_Opt*
_temp2266; struct _tuple15 _temp2262=* _temp2260; _LL2267: _temp2266= _temp2262.f1;
goto _LL2265; _LL2265: _temp2264= _temp2262.f2; goto _LL2263; _LL2263: return({
struct _tuple2* _temp2268=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp2268->f1= _temp2266; _temp2268->f2= _temp2264; _temp2268->f3= _temp2258;
_temp2268;});}} static void* Cyc_Tcutil_field_type( struct Cyc_Absyn_Structfield*
f){ return( void*) f->type;} static struct Cyc_Absyn_Structfield* Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Structfield* f, void* t){ return({ struct Cyc_Absyn_Structfield*
_temp2269=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2269->name= f->name; _temp2269->tq= f->tq; _temp2269->type=( void*) t;
_temp2269->width= f->width; _temp2269->attributes= f->attributes; _temp2269;});}
static struct Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn,
struct Cyc_List_List* inst, struct Cyc_List_List* ts); void* Cyc_Tcutil_rsubstitute(
struct _RegionHandle* rgn, struct Cyc_List_List* inst, void* t){ void* _temp2270=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tvar* _temp2316; struct Cyc_Absyn_Structdecl**
_temp2318; struct Cyc_List_List* _temp2320; struct _tuple1* _temp2322; struct
Cyc_Absyn_Uniondecl** _temp2324; struct Cyc_List_List* _temp2326; struct _tuple1*
_temp2328; struct Cyc_Absyn_TunionInfo _temp2330; void* _temp2332; struct Cyc_List_List*
_temp2334; void* _temp2336; struct Cyc_Absyn_TunionFieldInfo _temp2338; struct
Cyc_List_List* _temp2340; void* _temp2342; struct Cyc_Core_Opt* _temp2344;
struct Cyc_List_List* _temp2346; struct _tuple1* _temp2348; struct Cyc_Absyn_Exp*
_temp2350; struct Cyc_Absyn_Tqual _temp2352; void* _temp2354; struct Cyc_Absyn_PtrInfo
_temp2356; struct Cyc_Absyn_Conref* _temp2358; struct Cyc_Absyn_Tqual _temp2360;
struct Cyc_Absyn_Conref* _temp2362; void* _temp2364; void* _temp2366; struct Cyc_Absyn_FnInfo
_temp2368; struct Cyc_List_List* _temp2370; struct Cyc_List_List* _temp2372;
struct Cyc_Absyn_VarargInfo* _temp2374; int _temp2376; struct Cyc_List_List*
_temp2378; void* _temp2380; struct Cyc_Core_Opt* _temp2382; struct Cyc_List_List*
_temp2384; struct Cyc_List_List* _temp2386; struct Cyc_List_List* _temp2388;
struct Cyc_List_List* _temp2390; struct Cyc_Core_Opt* _temp2392; int _temp2394;
struct Cyc_Core_Opt* _temp2396; struct Cyc_Core_Opt* _temp2398; void* _temp2400;
struct Cyc_Absyn_Enumdecl* _temp2402; struct _tuple1* _temp2404; void* _temp2406;
void* _temp2408; void* _temp2410; struct Cyc_List_List* _temp2412; _LL2272: if((
unsigned int) _temp2270 > 4u?*(( int*) _temp2270) == Cyc_Absyn_VarType: 0){
_LL2317: _temp2316=(( struct Cyc_Absyn_VarType_struct*) _temp2270)->f1; goto
_LL2273;} else{ goto _LL2274;} _LL2274: if(( unsigned int) _temp2270 > 4u?*((
int*) _temp2270) == Cyc_Absyn_StructType: 0){ _LL2323: _temp2322=(( struct Cyc_Absyn_StructType_struct*)
_temp2270)->f1; goto _LL2321; _LL2321: _temp2320=(( struct Cyc_Absyn_StructType_struct*)
_temp2270)->f2; goto _LL2319; _LL2319: _temp2318=(( struct Cyc_Absyn_StructType_struct*)
_temp2270)->f3; goto _LL2275;} else{ goto _LL2276;} _LL2276: if(( unsigned int)
_temp2270 > 4u?*(( int*) _temp2270) == Cyc_Absyn_UnionType: 0){ _LL2329:
_temp2328=(( struct Cyc_Absyn_UnionType_struct*) _temp2270)->f1; goto _LL2327;
_LL2327: _temp2326=(( struct Cyc_Absyn_UnionType_struct*) _temp2270)->f2; goto
_LL2325; _LL2325: _temp2324=(( struct Cyc_Absyn_UnionType_struct*) _temp2270)->f3;
goto _LL2277;} else{ goto _LL2278;} _LL2278: if(( unsigned int) _temp2270 > 4u?*((
int*) _temp2270) == Cyc_Absyn_TunionType: 0){ _LL2331: _temp2330=(( struct Cyc_Absyn_TunionType_struct*)
_temp2270)->f1; _LL2337: _temp2336=( void*) _temp2330.tunion_info; goto _LL2335;
_LL2335: _temp2334= _temp2330.targs; goto _LL2333; _LL2333: _temp2332=( void*)
_temp2330.rgn; goto _LL2279;} else{ goto _LL2280;} _LL2280: if(( unsigned int)
_temp2270 > 4u?*(( int*) _temp2270) == Cyc_Absyn_TunionFieldType: 0){ _LL2339:
_temp2338=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2270)->f1; _LL2343:
_temp2342=( void*) _temp2338.field_info; goto _LL2341; _LL2341: _temp2340=
_temp2338.targs; goto _LL2281;} else{ goto _LL2282;} _LL2282: if(( unsigned int)
_temp2270 > 4u?*(( int*) _temp2270) == Cyc_Absyn_TypedefType: 0){ _LL2349:
_temp2348=(( struct Cyc_Absyn_TypedefType_struct*) _temp2270)->f1; goto _LL2347;
_LL2347: _temp2346=(( struct Cyc_Absyn_TypedefType_struct*) _temp2270)->f2; goto
_LL2345; _LL2345: _temp2344=(( struct Cyc_Absyn_TypedefType_struct*) _temp2270)->f3;
goto _LL2283;} else{ goto _LL2284;} _LL2284: if(( unsigned int) _temp2270 > 4u?*((
int*) _temp2270) == Cyc_Absyn_ArrayType: 0){ _LL2355: _temp2354=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2270)->f1; goto _LL2353; _LL2353:
_temp2352=(( struct Cyc_Absyn_ArrayType_struct*) _temp2270)->f2; goto _LL2351;
_LL2351: _temp2350=(( struct Cyc_Absyn_ArrayType_struct*) _temp2270)->f3; goto
_LL2285;} else{ goto _LL2286;} _LL2286: if(( unsigned int) _temp2270 > 4u?*((
int*) _temp2270) == Cyc_Absyn_PointerType: 0){ _LL2357: _temp2356=(( struct Cyc_Absyn_PointerType_struct*)
_temp2270)->f1; _LL2367: _temp2366=( void*) _temp2356.elt_typ; goto _LL2365;
_LL2365: _temp2364=( void*) _temp2356.rgn_typ; goto _LL2363; _LL2363: _temp2362=
_temp2356.nullable; goto _LL2361; _LL2361: _temp2360= _temp2356.tq; goto _LL2359;
_LL2359: _temp2358= _temp2356.bounds; goto _LL2287;} else{ goto _LL2288;}
_LL2288: if(( unsigned int) _temp2270 > 4u?*(( int*) _temp2270) == Cyc_Absyn_FnType:
0){ _LL2369: _temp2368=(( struct Cyc_Absyn_FnType_struct*) _temp2270)->f1;
_LL2385: _temp2384= _temp2368.tvars; goto _LL2383; _LL2383: _temp2382= _temp2368.effect;
goto _LL2381; _LL2381: _temp2380=( void*) _temp2368.ret_typ; goto _LL2379;
_LL2379: _temp2378= _temp2368.args; goto _LL2377; _LL2377: _temp2376= _temp2368.c_varargs;
goto _LL2375; _LL2375: _temp2374= _temp2368.cyc_varargs; goto _LL2373; _LL2373:
_temp2372= _temp2368.rgn_po; goto _LL2371; _LL2371: _temp2370= _temp2368.attributes;
goto _LL2289;} else{ goto _LL2290;} _LL2290: if(( unsigned int) _temp2270 > 4u?*((
int*) _temp2270) == Cyc_Absyn_TupleType: 0){ _LL2387: _temp2386=(( struct Cyc_Absyn_TupleType_struct*)
_temp2270)->f1; goto _LL2291;} else{ goto _LL2292;} _LL2292: if(( unsigned int)
_temp2270 > 4u?*(( int*) _temp2270) == Cyc_Absyn_AnonStructType: 0){ _LL2389:
_temp2388=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2270)->f1; goto
_LL2293;} else{ goto _LL2294;} _LL2294: if(( unsigned int) _temp2270 > 4u?*((
int*) _temp2270) == Cyc_Absyn_AnonUnionType: 0){ _LL2391: _temp2390=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp2270)->f1; goto _LL2295;} else{ goto
_LL2296;} _LL2296: if(( unsigned int) _temp2270 > 4u?*(( int*) _temp2270) == Cyc_Absyn_Evar:
0){ _LL2399: _temp2398=(( struct Cyc_Absyn_Evar_struct*) _temp2270)->f1; goto
_LL2397; _LL2397: _temp2396=(( struct Cyc_Absyn_Evar_struct*) _temp2270)->f2;
goto _LL2395; _LL2395: _temp2394=(( struct Cyc_Absyn_Evar_struct*) _temp2270)->f3;
goto _LL2393; _LL2393: _temp2392=(( struct Cyc_Absyn_Evar_struct*) _temp2270)->f4;
goto _LL2297;} else{ goto _LL2298;} _LL2298: if(( unsigned int) _temp2270 > 4u?*((
int*) _temp2270) == Cyc_Absyn_RgnHandleType: 0){ _LL2401: _temp2400=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp2270)->f1; goto _LL2299;} else{
goto _LL2300;} _LL2300: if(( unsigned int) _temp2270 > 4u?*(( int*) _temp2270)
== Cyc_Absyn_EnumType: 0){ _LL2405: _temp2404=(( struct Cyc_Absyn_EnumType_struct*)
_temp2270)->f1; goto _LL2403; _LL2403: _temp2402=(( struct Cyc_Absyn_EnumType_struct*)
_temp2270)->f2; goto _LL2301;} else{ goto _LL2302;} _LL2302: if( _temp2270 ==(
void*) Cyc_Absyn_VoidType){ goto _LL2303;} else{ goto _LL2304;} _LL2304: if((
unsigned int) _temp2270 > 4u?*(( int*) _temp2270) == Cyc_Absyn_IntType: 0){
_LL2409: _temp2408=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2270)->f1;
goto _LL2407; _LL2407: _temp2406=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2270)->f2; goto _LL2305;} else{ goto _LL2306;} _LL2306: if( _temp2270 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2307;} else{ goto _LL2308;} _LL2308: if(
_temp2270 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2309;} else{ goto _LL2310;}
_LL2310: if( _temp2270 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL2311;} else{ goto
_LL2312;} _LL2312: if(( unsigned int) _temp2270 > 4u?*(( int*) _temp2270) == Cyc_Absyn_AccessEff:
0){ _LL2411: _temp2410=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp2270)->f1;
goto _LL2313;} else{ goto _LL2314;} _LL2314: if(( unsigned int) _temp2270 > 4u?*((
int*) _temp2270) == Cyc_Absyn_JoinEff: 0){ _LL2413: _temp2412=(( struct Cyc_Absyn_JoinEff_struct*)
_temp2270)->f1; goto _LL2315;} else{ goto _LL2271;} _LL2273: { struct
_handler_cons _temp2414; _push_handler(& _temp2414);{ int _temp2416= 0; if(
setjmp( _temp2414.handler)){ _temp2416= 1;} if( ! _temp2416){{ void* _temp2417=((
void*(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp,
inst, _temp2316); _npop_handler( 0u); return _temp2417;}; _pop_handler();} else{
void* _temp2415=( void*) _exn_thrown; void* _temp2419= _temp2415; _LL2421: if(
_temp2419 == Cyc_Core_Not_found){ goto _LL2422;} else{ goto _LL2423;} _LL2423:
goto _LL2424; _LL2422: return t; _LL2424:( void) _throw( _temp2419); _LL2420:;}}}
_LL2275: { struct Cyc_List_List* _temp2425= Cyc_Tcutil_substs( rgn, inst,
_temp2320); return _temp2425 == _temp2320? t:( void*)({ struct Cyc_Absyn_StructType_struct*
_temp2426=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp2426[ 0]=({ struct Cyc_Absyn_StructType_struct _temp2427; _temp2427.tag=
Cyc_Absyn_StructType; _temp2427.f1= _temp2322; _temp2427.f2= _temp2425;
_temp2427.f3= _temp2318; _temp2427;}); _temp2426;});} _LL2277: { struct Cyc_List_List*
_temp2428= Cyc_Tcutil_substs( rgn, inst, _temp2326); return _temp2428 ==
_temp2326? t:( void*)({ struct Cyc_Absyn_UnionType_struct* _temp2429=( struct
Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp2429[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp2430; _temp2430.tag= Cyc_Absyn_UnionType;
_temp2430.f1= _temp2328; _temp2430.f2= _temp2428; _temp2430.f3= _temp2324;
_temp2430;}); _temp2429;});} _LL2279: { struct Cyc_List_List* _temp2431= Cyc_Tcutil_substs(
rgn, inst, _temp2334); void* _temp2432= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2332); return( _temp2431 == _temp2334? _temp2432 == _temp2332: 0)? t:( void*)({
struct Cyc_Absyn_TunionType_struct* _temp2433=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2433[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2434; _temp2434.tag= Cyc_Absyn_TunionType;
_temp2434.f1=({ struct Cyc_Absyn_TunionInfo _temp2435; _temp2435.tunion_info=(
void*) _temp2336; _temp2435.targs= _temp2431; _temp2435.rgn=( void*) _temp2432;
_temp2435;}); _temp2434;}); _temp2433;});} _LL2281: { struct Cyc_List_List*
_temp2436= Cyc_Tcutil_substs( rgn, inst, _temp2340); return _temp2436 ==
_temp2340? t:( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp2437=(
struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp2437[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp2438; _temp2438.tag=
Cyc_Absyn_TunionFieldType; _temp2438.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2439; _temp2439.field_info=( void*) _temp2342; _temp2439.targs= _temp2436;
_temp2439;}); _temp2438;}); _temp2437;});} _LL2283: { struct Cyc_List_List*
_temp2440= Cyc_Tcutil_substs( rgn, inst, _temp2346); return _temp2440 ==
_temp2346? t:( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp2441=( struct
Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2441[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp2442; _temp2442.tag=
Cyc_Absyn_TypedefType; _temp2442.f1= _temp2348; _temp2442.f2= _temp2440;
_temp2442.f3= _temp2344; _temp2442;}); _temp2441;});} _LL2285: { void* _temp2443=
Cyc_Tcutil_rsubstitute( rgn, inst, _temp2354); return _temp2443 == _temp2354? t:(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp2444=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2444[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2445; _temp2445.tag= Cyc_Absyn_ArrayType;
_temp2445.f1=( void*) _temp2443; _temp2445.f2= _temp2352; _temp2445.f3=
_temp2350; _temp2445;}); _temp2444;});} _LL2287: { void* _temp2446= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2366); void* _temp2447= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2364); if( _temp2446 == _temp2366? _temp2447 == _temp2364: 0){ return t;}
return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp2448=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp2448[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp2449; _temp2449.tag= Cyc_Absyn_PointerType;
_temp2449.f1=({ struct Cyc_Absyn_PtrInfo _temp2450; _temp2450.elt_typ=( void*)
_temp2446; _temp2450.rgn_typ=( void*) _temp2447; _temp2450.nullable= _temp2362;
_temp2450.tq= _temp2360; _temp2450.bounds= _temp2358; _temp2450;}); _temp2449;});
_temp2448;});} _LL2289:{ struct Cyc_List_List* _temp2451= _temp2384; for( 0;
_temp2451 != 0; _temp2451=(( struct Cyc_List_List*) _check_null( _temp2451))->tl){
inst=({ struct Cyc_List_List* _temp2452=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp2452->hd=( void*)({ struct _tuple5*
_temp2453=( struct _tuple5*) _region_malloc( rgn, sizeof( struct _tuple5));
_temp2453->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp2451))->hd; _temp2453->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2454=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2454[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2455; _temp2455.tag= Cyc_Absyn_VarType;
_temp2455.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp2451))->hd; _temp2455;}); _temp2454;}); _temp2453;}); _temp2452->tl= inst;
_temp2452;});}}{ struct Cyc_List_List* _temp2458; struct Cyc_List_List*
_temp2460; struct _tuple0 _temp2456=(( struct _tuple0(*)( struct _RegionHandle*
r3, struct _RegionHandle* r4, struct Cyc_List_List* x)) Cyc_List_rsplit)( rgn,
rgn,(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple16*(* f)(
struct _RegionHandle*, struct _tuple2*), struct _RegionHandle* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_substitute_f1, rgn, _temp2378)); _LL2461:
_temp2460= _temp2456.f1; goto _LL2459; _LL2459: _temp2458= _temp2456.f2; goto
_LL2457; _LL2457: { struct Cyc_List_List* _temp2462= Cyc_Tcutil_substs( rgn,
inst, _temp2458); struct Cyc_List_List* _temp2463=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple16*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_substitute_f2,(( struct Cyc_List_List*(*)( struct _RegionHandle* r3,
struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp2460, _temp2462)); struct Cyc_Core_Opt* eff2; if( _temp2382 == 0){
eff2= 0;} else{ void* _temp2464= Cyc_Tcutil_rsubstitute( rgn, inst,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2382))->v); if( _temp2464 ==( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2382))->v){ eff2= _temp2382;} else{ eff2=({
struct Cyc_Core_Opt* _temp2465=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2465->v=( void*) _temp2464; _temp2465;});}}{ struct Cyc_Absyn_VarargInfo*
cyc_varargs2; if( _temp2374 == 0){ cyc_varargs2= 0;} else{ int _temp2468; void*
_temp2470; void* _temp2472; struct Cyc_Absyn_Tqual _temp2474; struct Cyc_Core_Opt*
_temp2476; struct Cyc_Absyn_VarargInfo _temp2466=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp2374)); _LL2477: _temp2476= _temp2466.name; goto _LL2475;
_LL2475: _temp2474= _temp2466.tq; goto _LL2473; _LL2473: _temp2472=( void*)
_temp2466.type; goto _LL2471; _LL2471: _temp2470=( void*) _temp2466.rgn; goto
_LL2469; _LL2469: _temp2468= _temp2466.inject; goto _LL2467; _LL2467: { void*
_temp2478= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2472); void* _temp2479= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2470); if( _temp2478 == _temp2472? _temp2470 == _temp2479: 0){
cyc_varargs2= _temp2374;} else{ cyc_varargs2=({ struct Cyc_Absyn_VarargInfo*
_temp2480=( struct Cyc_Absyn_VarargInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo));
_temp2480->name= _temp2476; _temp2480->tq= _temp2474; _temp2480->type=( void*)
_temp2478; _temp2480->rgn=( void*) _temp2479; _temp2480->inject= _temp2468;
_temp2480;});}}}{ struct Cyc_List_List* rgn_po2; struct Cyc_List_List* _temp2483;
struct Cyc_List_List* _temp2485; struct _tuple0 _temp2481=(( struct _tuple0(*)(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x))
Cyc_List_rsplit)( rgn, rgn, _temp2372); _LL2486: _temp2485= _temp2481.f1; goto
_LL2484; _LL2484: _temp2483= _temp2481.f2; goto _LL2482; _LL2482: { struct Cyc_List_List*
_temp2487= Cyc_Tcutil_substs( rgn, inst, _temp2485); struct Cyc_List_List*
_temp2488= Cyc_Tcutil_substs( rgn, inst, _temp2483); if( _temp2487 == _temp2485?
_temp2488 == _temp2483: 0){ rgn_po2= _temp2372;} else{ rgn_po2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp2487,
_temp2488);} return( void*)({ struct Cyc_Absyn_FnType_struct* _temp2489=( struct
Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2489[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2490; _temp2490.tag= Cyc_Absyn_FnType;
_temp2490.f1=({ struct Cyc_Absyn_FnInfo _temp2491; _temp2491.tvars= _temp2384;
_temp2491.effect= eff2; _temp2491.ret_typ=( void*) Cyc_Tcutil_rsubstitute( rgn,
inst, _temp2380); _temp2491.args= _temp2463; _temp2491.c_varargs= _temp2376;
_temp2491.cyc_varargs= cyc_varargs2; _temp2491.rgn_po= rgn_po2; _temp2491.attributes=
_temp2370; _temp2491;}); _temp2490;}); _temp2489;});}}}}} _LL2291: { struct Cyc_List_List*
_temp2492=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_snd_tqt,
_temp2386); struct Cyc_List_List* _temp2493= Cyc_Tcutil_substs( rgn, inst,
_temp2492); if( _temp2493 == _temp2492){ return t;}{ struct Cyc_List_List*
_temp2494=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*,
void*), struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_map2)( Cyc_Tcutil_map2_tq,
_temp2386, _temp2493); return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp2495=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp2495[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp2496; _temp2496.tag= Cyc_Absyn_TupleType;
_temp2496.f1= _temp2494; _temp2496;}); _temp2495;});}} _LL2293: { struct Cyc_List_List*
_temp2497=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2388); struct Cyc_List_List* _temp2498= Cyc_Tcutil_substs( rgn, inst,
_temp2497); if( _temp2498 == _temp2497){ return t;}{ struct Cyc_List_List*
_temp2499=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2388, _temp2498); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp2500=( struct Cyc_Absyn_AnonStructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2500[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2501; _temp2501.tag= Cyc_Absyn_AnonStructType;
_temp2501.f1= _temp2499; _temp2501;}); _temp2500;});}} _LL2295: { struct Cyc_List_List*
_temp2502=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2390); struct Cyc_List_List* _temp2503= Cyc_Tcutil_substs( rgn, inst,
_temp2502); if( _temp2503 == _temp2502){ return t;}{ struct Cyc_List_List*
_temp2504=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2390, _temp2503); return(
void*)({ struct Cyc_Absyn_AnonStructType_struct* _temp2505=( struct Cyc_Absyn_AnonStructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2505[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2506; _temp2506.tag= Cyc_Absyn_AnonStructType;
_temp2506.f1= _temp2504; _temp2506;}); _temp2505;});}} _LL2297: if( _temp2396 !=
0){ return Cyc_Tcutil_rsubstitute( rgn, inst,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2396))->v);} else{ return t;} _LL2299: { void* _temp2507= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2400); return _temp2507 == _temp2400? t:( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp2508=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp2508[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp2509; _temp2509.tag= Cyc_Absyn_RgnHandleType; _temp2509.f1=( void*)
_temp2507; _temp2509;}); _temp2508;});} _LL2301: return t; _LL2303: return t;
_LL2305: return t; _LL2307: return t; _LL2309: return t; _LL2311: return t;
_LL2313: { void* _temp2510= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2410);
return _temp2510 == _temp2410? t:( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp2511=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2511[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2512; _temp2512.tag= Cyc_Absyn_AccessEff;
_temp2512.f1=( void*) _temp2510; _temp2512;}); _temp2511;});} _LL2315: { struct
Cyc_List_List* _temp2513= Cyc_Tcutil_substs( rgn, inst, _temp2412); return
_temp2513 == _temp2412? t:( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2514=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2514[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2515; _temp2515.tag= Cyc_Absyn_JoinEff;
_temp2515.f1= _temp2513; _temp2515;}); _temp2514;});} _LL2271:;} static struct
Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List*
inst, struct Cyc_List_List* ts){ if( ts == 0){ return 0;}{ void* _temp2516=(
void*)(( struct Cyc_List_List*) _check_null( ts))->hd; struct Cyc_List_List*
_temp2517=(( struct Cyc_List_List*) _check_null( ts))->tl; void* _temp2518= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2516); struct Cyc_List_List* _temp2519= Cyc_Tcutil_substs( rgn,
inst, _temp2517); if( _temp2516 == _temp2518? _temp2517 == _temp2519: 0){ return
ts;} return( struct Cyc_List_List*)({ struct Cyc_List_List* _temp2520=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2520->hd=( void*)
_temp2518; _temp2520->tl= _temp2519; _temp2520;});}} extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List* inst, void* t){ return Cyc_Tcutil_rsubstitute( Cyc_Core_heap_region,
inst, t);} struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2566=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2566[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2567; _temp2567.tag= Cyc_Absyn_Const_e; _temp2567.f1=(
void*)(( void*) Cyc_Absyn_Null_c); _temp2567;}); _temp2566;}), loc);{ void*
_temp2521= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp2535; void*
_temp2537; void* _temp2539; void* _temp2541; void* _temp2543; _LL2523: if((
unsigned int) _temp2521 > 4u?*(( int*) _temp2521) == Cyc_Absyn_PointerType: 0){
_LL2536: _temp2535=(( struct Cyc_Absyn_PointerType_struct*) _temp2521)->f1; goto
_LL2524;} else{ goto _LL2525;} _LL2525: if(( unsigned int) _temp2521 > 4u?*((
int*) _temp2521) == Cyc_Absyn_IntType: 0){ _LL2540: _temp2539=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2521)->f1; goto _LL2538; _LL2538: _temp2537=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2521)->f2; if( _temp2537 ==(
void*) Cyc_Absyn_B1){ goto _LL2526;} else{ goto _LL2527;}} else{ goto _LL2527;}
_LL2527: if(( unsigned int) _temp2521 > 4u?*(( int*) _temp2521) == Cyc_Absyn_IntType:
0){ _LL2544: _temp2543=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2521)->f1;
goto _LL2542; _LL2542: _temp2541=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2521)->f2; goto _LL2528;} else{ goto _LL2529;} _LL2529: if( _temp2521 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2530;} else{ goto _LL2531;} _LL2531: if(
_temp2521 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2532;} else{ goto _LL2533;}
_LL2533: goto _LL2534; _LL2524: goto _LL2522; _LL2526:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2545=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2545[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2546; _temp2546.tag= Cyc_Absyn_Const_e; _temp2546.f1=(
void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp2547=( struct Cyc_Absyn_Char_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct)); _temp2547[ 0]=({ struct Cyc_Absyn_Char_c_struct
_temp2548; _temp2548.tag= Cyc_Absyn_Char_c; _temp2548.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp2548.f2='\000'; _temp2548;}); _temp2547;})); _temp2546;}); _temp2545;})));
goto _LL2522; _LL2528:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2549=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2549[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2550; _temp2550.tag= Cyc_Absyn_Const_e;
_temp2550.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp2551=(
struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp2551[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp2552; _temp2552.tag= Cyc_Absyn_Int_c;
_temp2552.f1=( void*) _temp2543; _temp2552.f2= 0; _temp2552;}); _temp2551;}));
_temp2550;}); _temp2549;}))); if( _temp2541 !=( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2553=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2553[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2554; _temp2554.tag= Cyc_Absyn_Cast_e; _temp2554.f1=( void*) t; _temp2554.f2=
e; _temp2554;}); _temp2553;}), loc);} goto _LL2522; _LL2530:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2555=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2555[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2556; _temp2556.tag= Cyc_Absyn_Const_e; _temp2556.f1=(
void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2557=( struct Cyc_Absyn_Float_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct)); _temp2557[ 0]=({ struct
Cyc_Absyn_Float_c_struct _temp2558; _temp2558.tag= Cyc_Absyn_Float_c; _temp2558.f1=
_tag_arr("0.0", sizeof( unsigned char), 4u); _temp2558;}); _temp2557;}));
_temp2556;}); _temp2555;}))); goto _LL2522; _LL2532:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2559=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2559[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2560; _temp2560.tag= Cyc_Absyn_Cast_e; _temp2560.f1=( void*) t; _temp2560.f2=
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2561=( struct
Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2561[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2562; _temp2562.tag= Cyc_Absyn_Const_e;
_temp2562.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2563=(
struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2563[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2564; _temp2564.tag= Cyc_Absyn_Float_c;
_temp2564.f1= _tag_arr("0.0", sizeof( unsigned char), 4u); _temp2564;});
_temp2563;})); _temp2562;}); _temp2561;}), loc); _temp2560;}); _temp2559;})));
goto _LL2522; _LL2534: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp2565= Cyc_Absynpp_typ2string( t); xprintf("declaration of type %.*s requires initializer",
_get_arr_size( _temp2565, 1u), _temp2565.curr);})); goto _LL2522; _LL2522:;}
return e;} struct _tuple5* Cyc_Tcutil_make_inst_var( struct Cyc_List_List* s,
struct Cyc_Absyn_Tvar* tv){ void* k=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( tv->kind); return({ struct _tuple5* _temp2568=( struct
_tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp2568->f1= tv; _temp2568->f2=
Cyc_Absyn_new_evar(({ struct Cyc_Core_Opt* _temp2569=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2569->v=( void*) k; _temp2569;}),({
struct Cyc_Core_Opt* _temp2570=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2570->v=( void*) s; _temp2570;})); _temp2568;});} struct
_tuple5* Cyc_Tcutil_r_make_inst_var( struct _tuple6* env, struct Cyc_Absyn_Tvar*
tv){ struct _tuple6 _temp2573; struct _RegionHandle* _temp2574; struct Cyc_List_List*
_temp2576; struct _tuple6* _temp2571= env; _temp2573=* _temp2571; _LL2577:
_temp2576= _temp2573.f1; goto _LL2575; _LL2575: _temp2574= _temp2573.f2; goto
_LL2572; _LL2572: { void* k=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
tv->kind); return({ struct _tuple5* _temp2578=( struct _tuple5*) _region_malloc(
_temp2574, sizeof( struct _tuple5)); _temp2578->f1= tv; _temp2578->f2= Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt* _temp2579=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2579->v=( void*) k; _temp2579;}),({ struct Cyc_Core_Opt*
_temp2580=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2580->v=( void*) _temp2576; _temp2580;})); _temp2578;});}} static struct
Cyc_List_List* Cyc_Tcutil_add_free_tvar( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 != 0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){
if( Cyc_String_zstrptrcmp((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->name, tv->name) == 0){ struct Cyc_Absyn_Conref* k1=((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->kind;
struct Cyc_Absyn_Conref* k2= tv->kind; if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp2581=* tv->name;
struct _tagged_arr _temp2582= Cyc_Absynpp_ckind2string( k1); struct _tagged_arr
_temp2583= Cyc_Absynpp_ckind2string( k2); xprintf("type variable %.*s is used with inconsistent kinds %.*s and %.*s",
_get_arr_size( _temp2581, 1u), _temp2581.curr, _get_arr_size( _temp2582, 1u),
_temp2582.curr, _get_arr_size( _temp2583, 1u), _temp2583.curr);}));} if( tv->identity
== 0){ tv->identity=(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity;} else{ if(*(( int*) _check_null( tv->identity))
!=*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity))){(( int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)((
struct _tagged_arr) xprintf("same type variable has different identity!"));}}
return tvs;}}} tv->identity= Cyc_Tcutil_new_tvar_id(); return({ struct Cyc_List_List*
_temp2584=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2584->hd=( void*) tv; _temp2584->tl= tvs; _temp2584;});} static struct Cyc_List_List*
Cyc_Tcutil_fast_add_free_tvar( struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar*
tv){ if( tv->identity == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)(
_tag_arr("fast_add_free_tvar: bad identity in tv", sizeof( unsigned char), 39u));}{
struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 != 0; tvs2=(( struct Cyc_List_List*)
_check_null( tvs2))->tl){ if((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity == 0){(( int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)(
_tag_arr("fast_add_free_tvar: bad identity in tvs2", sizeof( unsigned char), 41u));}
if(*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs2))->hd)->identity)) ==*(( int*) _check_null( tv->identity))){
return tvs;}}} return({ struct Cyc_List_List* _temp2585=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2585->hd=( void*) tv; _temp2585->tl=
tvs; _temp2585;});} static struct Cyc_List_List* Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar* tv){ if( tv->identity == 0){((
int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)(( struct _tagged_arr)({
struct _tagged_arr _temp2586= Cyc_Tcutil_tvar2string( tv); xprintf("bound tvar id for %.*s is null",
_get_arr_size( _temp2586, 1u), _temp2586.curr);}));} return({ struct Cyc_List_List*
_temp2587=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2587->hd=( void*) tv; _temp2587->tl= tvs; _temp2587;});} static struct Cyc_List_List*
Cyc_Tcutil_add_free_evar( struct Cyc_List_List* es, void* e){ void* _temp2588=
Cyc_Tcutil_compress( e); struct Cyc_Core_Opt* _temp2594; int _temp2596; struct
Cyc_Core_Opt* _temp2598; struct Cyc_Core_Opt* _temp2600; _LL2590: if((
unsigned int) _temp2588 > 4u?*(( int*) _temp2588) == Cyc_Absyn_Evar: 0){ _LL2601:
_temp2600=(( struct Cyc_Absyn_Evar_struct*) _temp2588)->f1; goto _LL2599;
_LL2599: _temp2598=(( struct Cyc_Absyn_Evar_struct*) _temp2588)->f2; goto
_LL2597; _LL2597: _temp2596=(( struct Cyc_Absyn_Evar_struct*) _temp2588)->f3;
goto _LL2595; _LL2595: _temp2594=(( struct Cyc_Absyn_Evar_struct*) _temp2588)->f4;
goto _LL2591;} else{ goto _LL2592;} _LL2592: goto _LL2593; _LL2591:{ struct Cyc_List_List*
es2= es; for( 0; es2 != 0; es2=(( struct Cyc_List_List*) _check_null( es2))->tl){
void* _temp2602= Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*)
_check_null( es2))->hd); struct Cyc_Core_Opt* _temp2608; int _temp2610; struct
Cyc_Core_Opt* _temp2612; struct Cyc_Core_Opt* _temp2614; _LL2604: if((
unsigned int) _temp2602 > 4u?*(( int*) _temp2602) == Cyc_Absyn_Evar: 0){ _LL2615:
_temp2614=(( struct Cyc_Absyn_Evar_struct*) _temp2602)->f1; goto _LL2613;
_LL2613: _temp2612=(( struct Cyc_Absyn_Evar_struct*) _temp2602)->f2; goto
_LL2611; _LL2611: _temp2610=(( struct Cyc_Absyn_Evar_struct*) _temp2602)->f3;
goto _LL2609; _LL2609: _temp2608=(( struct Cyc_Absyn_Evar_struct*) _temp2602)->f4;
goto _LL2605;} else{ goto _LL2606;} _LL2606: goto _LL2607; _LL2605: if(
_temp2596 == _temp2610){ return es;} goto _LL2603; _LL2607: goto _LL2603;
_LL2603:;}} return({ struct Cyc_List_List* _temp2616=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2616->hd=( void*) e; _temp2616->tl=
es; _temp2616;}); _LL2593: return es; _LL2589:;} static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars( struct Cyc_List_List* tvs, struct Cyc_List_List*
btvs){ struct Cyc_List_List* r= 0; for( 0; tvs != 0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){ int present= 0;{ struct Cyc_List_List* b= btvs; for( 0;
b != 0; b=(( struct Cyc_List_List*) _check_null( b))->tl){ if(*(( int*)
_check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->identity))
==*(( int*) _check_null((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( b))->hd)->identity))){ present= 1; break;}}} if( ! present){ r=({
struct Cyc_List_List* _temp2617=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2617->hd=( void*)(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd); _temp2617->tl= r; _temp2617;});}}
r=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( r);
return r;} struct Cyc_Tcutil_CVTEnv{ struct Cyc_List_List* kind_env; struct Cyc_List_List*
free_vars; struct Cyc_List_List* free_evars; } ; static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcutil_CVTEnv cvtenv, void* expected_kind, void* t){{ void*
_temp2618= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt* _temp2664; int
_temp2666; struct Cyc_Core_Opt* _temp2668; struct Cyc_Core_Opt* _temp2670;
struct Cyc_Core_Opt** _temp2672; struct Cyc_Absyn_Tvar* _temp2673; struct Cyc_Absyn_Enumdecl*
_temp2675; struct Cyc_Absyn_Enumdecl** _temp2677; struct _tuple1* _temp2678;
struct Cyc_Absyn_TunionInfo _temp2680; void* _temp2682; struct Cyc_List_List*
_temp2684; void* _temp2686; void** _temp2688; struct Cyc_Absyn_TunionFieldInfo
_temp2689; struct Cyc_List_List* _temp2691; void* _temp2693; void** _temp2695;
struct Cyc_Absyn_PtrInfo _temp2696; struct Cyc_Absyn_Conref* _temp2698; struct
Cyc_Absyn_Tqual _temp2700; struct Cyc_Absyn_Conref* _temp2702; void* _temp2704;
void* _temp2706; void* _temp2708; void* _temp2710; struct Cyc_Absyn_Exp*
_temp2712; struct Cyc_Absyn_Tqual _temp2714; void* _temp2716; struct Cyc_Absyn_FnInfo
_temp2718; struct Cyc_List_List* _temp2720; struct Cyc_List_List* _temp2722;
struct Cyc_Absyn_VarargInfo* _temp2724; int _temp2726; struct Cyc_List_List*
_temp2728; void* _temp2730; struct Cyc_Core_Opt* _temp2732; struct Cyc_Core_Opt**
_temp2734; struct Cyc_List_List* _temp2735; struct Cyc_List_List** _temp2737;
struct Cyc_List_List* _temp2738; struct Cyc_List_List* _temp2740; struct Cyc_List_List*
_temp2742; struct Cyc_Absyn_Structdecl** _temp2744; struct Cyc_Absyn_Structdecl***
_temp2746; struct Cyc_List_List* _temp2747; struct _tuple1* _temp2749; struct
Cyc_Absyn_Uniondecl** _temp2751; struct Cyc_Absyn_Uniondecl*** _temp2753; struct
Cyc_List_List* _temp2754; struct _tuple1* _temp2756; struct Cyc_Core_Opt*
_temp2758; struct Cyc_Core_Opt** _temp2760; struct Cyc_List_List* _temp2761;
struct _tuple1* _temp2763; void* _temp2765; void* _temp2767; struct Cyc_List_List*
_temp2769; _LL2620: if( _temp2618 ==( void*) Cyc_Absyn_VoidType){ goto _LL2621;}
else{ goto _LL2622;} _LL2622: if(( unsigned int) _temp2618 > 4u?*(( int*)
_temp2618) == Cyc_Absyn_Evar: 0){ _LL2671: _temp2670=(( struct Cyc_Absyn_Evar_struct*)
_temp2618)->f1; _temp2672=&(( struct Cyc_Absyn_Evar_struct*) _temp2618)->f1;
goto _LL2669; _LL2669: _temp2668=(( struct Cyc_Absyn_Evar_struct*) _temp2618)->f2;
goto _LL2667; _LL2667: _temp2666=(( struct Cyc_Absyn_Evar_struct*) _temp2618)->f3;
goto _LL2665; _LL2665: _temp2664=(( struct Cyc_Absyn_Evar_struct*) _temp2618)->f4;
goto _LL2623;} else{ goto _LL2624;} _LL2624: if(( unsigned int) _temp2618 > 4u?*((
int*) _temp2618) == Cyc_Absyn_VarType: 0){ _LL2674: _temp2673=(( struct Cyc_Absyn_VarType_struct*)
_temp2618)->f1; goto _LL2625;} else{ goto _LL2626;} _LL2626: if(( unsigned int)
_temp2618 > 4u?*(( int*) _temp2618) == Cyc_Absyn_EnumType: 0){ _LL2679:
_temp2678=(( struct Cyc_Absyn_EnumType_struct*) _temp2618)->f1; goto _LL2676;
_LL2676: _temp2675=(( struct Cyc_Absyn_EnumType_struct*) _temp2618)->f2;
_temp2677=&(( struct Cyc_Absyn_EnumType_struct*) _temp2618)->f2; goto _LL2627;}
else{ goto _LL2628;} _LL2628: if(( unsigned int) _temp2618 > 4u?*(( int*)
_temp2618) == Cyc_Absyn_TunionType: 0){ _LL2681: _temp2680=(( struct Cyc_Absyn_TunionType_struct*)
_temp2618)->f1; _LL2687: _temp2686=( void*) _temp2680.tunion_info; _temp2688=&(((
struct Cyc_Absyn_TunionType_struct*) _temp2618)->f1).tunion_info; goto _LL2685;
_LL2685: _temp2684= _temp2680.targs; goto _LL2683; _LL2683: _temp2682=( void*)
_temp2680.rgn; goto _LL2629;} else{ goto _LL2630;} _LL2630: if(( unsigned int)
_temp2618 > 4u?*(( int*) _temp2618) == Cyc_Absyn_TunionFieldType: 0){ _LL2690:
_temp2689=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2618)->f1; _LL2694:
_temp2693=( void*) _temp2689.field_info; _temp2695=&((( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2618)->f1).field_info; goto _LL2692; _LL2692: _temp2691= _temp2689.targs;
goto _LL2631;} else{ goto _LL2632;} _LL2632: if(( unsigned int) _temp2618 > 4u?*((
int*) _temp2618) == Cyc_Absyn_PointerType: 0){ _LL2697: _temp2696=(( struct Cyc_Absyn_PointerType_struct*)
_temp2618)->f1; _LL2707: _temp2706=( void*) _temp2696.elt_typ; goto _LL2705;
_LL2705: _temp2704=( void*) _temp2696.rgn_typ; goto _LL2703; _LL2703: _temp2702=
_temp2696.nullable; goto _LL2701; _LL2701: _temp2700= _temp2696.tq; goto _LL2699;
_LL2699: _temp2698= _temp2696.bounds; goto _LL2633;} else{ goto _LL2634;}
_LL2634: if(( unsigned int) _temp2618 > 4u?*(( int*) _temp2618) == Cyc_Absyn_IntType:
0){ _LL2711: _temp2710=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2618)->f1;
goto _LL2709; _LL2709: _temp2708=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2618)->f2; goto _LL2635;} else{ goto _LL2636;} _LL2636: if( _temp2618 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2637;} else{ goto _LL2638;} _LL2638: if(
_temp2618 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2639;} else{ goto _LL2640;}
_LL2640: if(( unsigned int) _temp2618 > 4u?*(( int*) _temp2618) == Cyc_Absyn_ArrayType:
0){ _LL2717: _temp2716=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2618)->f1;
goto _LL2715; _LL2715: _temp2714=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2618)->f2; goto _LL2713; _LL2713: _temp2712=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2618)->f3; goto _LL2641;} else{ goto _LL2642;} _LL2642: if(( unsigned int)
_temp2618 > 4u?*(( int*) _temp2618) == Cyc_Absyn_FnType: 0){ _LL2719: _temp2718=((
struct Cyc_Absyn_FnType_struct*) _temp2618)->f1; _LL2736: _temp2735= _temp2718.tvars;
_temp2737=&((( struct Cyc_Absyn_FnType_struct*) _temp2618)->f1).tvars; goto
_LL2733; _LL2733: _temp2732= _temp2718.effect; _temp2734=&((( struct Cyc_Absyn_FnType_struct*)
_temp2618)->f1).effect; goto _LL2731; _LL2731: _temp2730=( void*) _temp2718.ret_typ;
goto _LL2729; _LL2729: _temp2728= _temp2718.args; goto _LL2727; _LL2727:
_temp2726= _temp2718.c_varargs; goto _LL2725; _LL2725: _temp2724= _temp2718.cyc_varargs;
goto _LL2723; _LL2723: _temp2722= _temp2718.rgn_po; goto _LL2721; _LL2721:
_temp2720= _temp2718.attributes; goto _LL2643;} else{ goto _LL2644;} _LL2644:
if(( unsigned int) _temp2618 > 4u?*(( int*) _temp2618) == Cyc_Absyn_TupleType: 0){
_LL2739: _temp2738=(( struct Cyc_Absyn_TupleType_struct*) _temp2618)->f1; goto
_LL2645;} else{ goto _LL2646;} _LL2646: if(( unsigned int) _temp2618 > 4u?*((
int*) _temp2618) == Cyc_Absyn_AnonStructType: 0){ _LL2741: _temp2740=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp2618)->f1; goto _LL2647;} else{ goto
_LL2648;} _LL2648: if(( unsigned int) _temp2618 > 4u?*(( int*) _temp2618) == Cyc_Absyn_AnonUnionType:
0){ _LL2743: _temp2742=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp2618)->f1;
goto _LL2649;} else{ goto _LL2650;} _LL2650: if(( unsigned int) _temp2618 > 4u?*((
int*) _temp2618) == Cyc_Absyn_StructType: 0){ _LL2750: _temp2749=(( struct Cyc_Absyn_StructType_struct*)
_temp2618)->f1; goto _LL2748; _LL2748: _temp2747=(( struct Cyc_Absyn_StructType_struct*)
_temp2618)->f2; goto _LL2745; _LL2745: _temp2744=(( struct Cyc_Absyn_StructType_struct*)
_temp2618)->f3; _temp2746=&(( struct Cyc_Absyn_StructType_struct*) _temp2618)->f3;
goto _LL2651;} else{ goto _LL2652;} _LL2652: if(( unsigned int) _temp2618 > 4u?*((
int*) _temp2618) == Cyc_Absyn_UnionType: 0){ _LL2757: _temp2756=(( struct Cyc_Absyn_UnionType_struct*)
_temp2618)->f1; goto _LL2755; _LL2755: _temp2754=(( struct Cyc_Absyn_UnionType_struct*)
_temp2618)->f2; goto _LL2752; _LL2752: _temp2751=(( struct Cyc_Absyn_UnionType_struct*)
_temp2618)->f3; _temp2753=&(( struct Cyc_Absyn_UnionType_struct*) _temp2618)->f3;
goto _LL2653;} else{ goto _LL2654;} _LL2654: if(( unsigned int) _temp2618 > 4u?*((
int*) _temp2618) == Cyc_Absyn_TypedefType: 0){ _LL2764: _temp2763=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2618)->f1; goto _LL2762; _LL2762: _temp2761=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2618)->f2; goto _LL2759; _LL2759: _temp2758=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2618)->f3; _temp2760=&(( struct Cyc_Absyn_TypedefType_struct*) _temp2618)->f3;
goto _LL2655;} else{ goto _LL2656;} _LL2656: if( _temp2618 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL2657;} else{ goto _LL2658;} _LL2658: if(( unsigned int) _temp2618 > 4u?*((
int*) _temp2618) == Cyc_Absyn_RgnHandleType: 0){ _LL2766: _temp2765=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp2618)->f1; goto _LL2659;} else{
goto _LL2660;} _LL2660: if(( unsigned int) _temp2618 > 4u?*(( int*) _temp2618)
== Cyc_Absyn_AccessEff: 0){ _LL2768: _temp2767=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp2618)->f1; goto _LL2661;} else{ goto _LL2662;} _LL2662: if(( unsigned int)
_temp2618 > 4u?*(( int*) _temp2618) == Cyc_Absyn_JoinEff: 0){ _LL2770: _temp2769=((
struct Cyc_Absyn_JoinEff_struct*) _temp2618)->f1; goto _LL2663;} else{ goto
_LL2619;} _LL2621: goto _LL2619; _LL2623: if(* _temp2672 == 0){* _temp2672=({
struct Cyc_Core_Opt* _temp2771=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2771->v=( void*) expected_kind; _temp2771;});} cvtenv.free_evars=
Cyc_Tcutil_add_free_evar( cvtenv.free_evars, t); goto _LL2619; _LL2625: { struct
Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp2673->kind); if(( void*) c->v ==( void*) Cyc_Absyn_No_constr){(
void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2772=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2772[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2773; _temp2773.tag= Cyc_Absyn_Eq_constr;
_temp2773.f1=( void*) expected_kind; _temp2773;}); _temp2772;})));} cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar( loc, cvtenv.kind_env, _temp2673); cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars, _temp2673); goto _LL2619;}
_LL2627: if(* _temp2677 == 0){ struct _handler_cons _temp2774; _push_handler(&
_temp2774);{ int _temp2776= 0; if( setjmp( _temp2774.handler)){ _temp2776= 1;}
if( ! _temp2776){{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl( te,
loc, _temp2678);* _temp2677=( struct Cyc_Absyn_Enumdecl*)* ed;}; _pop_handler();}
else{ void* _temp2775=( void*) _exn_thrown; void* _temp2778= _temp2775; _LL2780:
if( _temp2778 == Cyc_Dict_Absent){ goto _LL2781;} else{ goto _LL2782;} _LL2782:
goto _LL2783; _LL2781: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp2784= Cyc_Absynpp_qvar2string( _temp2678); xprintf("unbound type enum %.*s",
_get_arr_size( _temp2784, 1u), _temp2784.curr);})); return cvtenv; _LL2783:(
void) _throw( _temp2778); _LL2779:;}}}{ struct Cyc_Absyn_Enumdecl* ed=( struct
Cyc_Absyn_Enumdecl*) _check_null(* _temp2677);* _temp2678=( ed->name)[
_check_known_subscript_notnull( 1u, 0)]; goto _LL2619;} _LL2629:{ void*
_temp2785=* _temp2688; struct Cyc_Absyn_UnknownTunionInfo _temp2791; int
_temp2793; struct _tuple1* _temp2795; struct Cyc_Absyn_Tuniondecl* _temp2797;
_LL2787: if(*(( int*) _temp2785) == Cyc_Absyn_UnknownTunion){ _LL2792: _temp2791=((
struct Cyc_Absyn_UnknownTunion_struct*) _temp2785)->f1; _LL2796: _temp2795=
_temp2791.name; goto _LL2794; _LL2794: _temp2793= _temp2791.is_xtunion; goto
_LL2788;} else{ goto _LL2789;} _LL2789: if(*(( int*) _temp2785) == Cyc_Absyn_KnownTunion){
_LL2798: _temp2797=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2785)->f1; goto
_LL2790;} else{ goto _LL2786;} _LL2788: { struct Cyc_Absyn_Tuniondecl* tud;{
struct _handler_cons _temp2799; _push_handler(& _temp2799);{ int _temp2801= 0;
if( setjmp( _temp2799.handler)){ _temp2801= 1;} if( ! _temp2801){ tud=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp2795);; _pop_handler();} else{ void* _temp2800=( void*)
_exn_thrown; void* _temp2803= _temp2800; _LL2805: if( _temp2803 == Cyc_Dict_Absent){
goto _LL2806;} else{ goto _LL2807;} _LL2807: goto _LL2808; _LL2806: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp2809= Cyc_Absynpp_qvar2string(
_temp2795); xprintf("unbound type [x]tunion %.*s", _get_arr_size( _temp2809, 1u),
_temp2809.curr);})); return cvtenv; _LL2808:( void) _throw( _temp2803); _LL2804:;}}}
if( tud->is_xtunion != _temp2793){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr _temp2810= Cyc_Absynpp_qvar2string( _temp2795); xprintf("[x]tunion is different from type declaration %.*s",
_get_arr_size( _temp2810, 1u), _temp2810.curr);}));}* _temp2688=( void*)({
struct Cyc_Absyn_KnownTunion_struct* _temp2811=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp2811[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp2812; _temp2812.tag= Cyc_Absyn_KnownTunion;
_temp2812.f1= tud; _temp2812;}); _temp2811;}); _temp2797= tud; goto _LL2790;}
_LL2790: cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2682);{ struct Cyc_List_List* tvs= _temp2797->tvs; for( 0; _temp2684 != 0?
tvs != 0: 0;( _temp2684=(( struct Cyc_List_List*) _check_null( _temp2684))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2684))->hd; void* k1=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} if( _temp2684 != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp2813= Cyc_Absynpp_qvar2string( _temp2797->name);
xprintf("too many type arguments for tunion %.*s", _get_arr_size( _temp2813, 1u),
_temp2813.curr);}));} if( tvs != 0){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr _temp2814= Cyc_Absynpp_qvar2string( _temp2797->name); xprintf("too few type arguments for tunion %.*s",
_get_arr_size( _temp2814, 1u), _temp2814.curr);}));} goto _LL2786;} _LL2786:;}
goto _LL2619; _LL2631:{ void* _temp2815=* _temp2695; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp2821; int _temp2823; struct _tuple1* _temp2825; struct _tuple1* _temp2827;
struct Cyc_Absyn_Tunionfield* _temp2829; struct Cyc_Absyn_Tuniondecl* _temp2831;
_LL2817: if(*(( int*) _temp2815) == Cyc_Absyn_UnknownTunionfield){ _LL2822:
_temp2821=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp2815)->f1;
_LL2828: _temp2827= _temp2821.tunion_name; goto _LL2826; _LL2826: _temp2825=
_temp2821.field_name; goto _LL2824; _LL2824: _temp2823= _temp2821.is_xtunion;
goto _LL2818;} else{ goto _LL2819;} _LL2819: if(*(( int*) _temp2815) == Cyc_Absyn_KnownTunionfield){
_LL2832: _temp2831=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp2815)->f1;
goto _LL2830; _LL2830: _temp2829=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2815)->f2; goto _LL2820;} else{ goto _LL2816;} _LL2818: { struct Cyc_Absyn_Tuniondecl*
tud; struct Cyc_Absyn_Tunionfield* tuf;{ struct _handler_cons _temp2833;
_push_handler(& _temp2833);{ int _temp2835= 0; if( setjmp( _temp2833.handler)){
_temp2835= 1;} if( ! _temp2835){* Cyc_Tcenv_lookup_tuniondecl( te, loc,
_temp2827);; _pop_handler();} else{ void* _temp2834=( void*) _exn_thrown; void*
_temp2837= _temp2834; _LL2839: if( _temp2837 == Cyc_Dict_Absent){ goto _LL2840;}
else{ goto _LL2841;} _LL2841: goto _LL2842; _LL2840: Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp2843= Cyc_Absynpp_qvar2string(
_temp2827); xprintf("unbound type tunion %.*s", _get_arr_size( _temp2843, 1u),
_temp2843.curr);})); return cvtenv; _LL2842:( void) _throw( _temp2837); _LL2838:;}}}{
struct _handler_cons _temp2844; _push_handler(& _temp2844);{ int _temp2846= 0;
if( setjmp( _temp2844.handler)){ _temp2846= 1;} if( ! _temp2846){{ void*
_temp2847= Cyc_Tcenv_lookup_ordinary( te, loc, _temp2825); struct Cyc_Absyn_Tunionfield*
_temp2853; struct Cyc_Absyn_Tuniondecl* _temp2855; _LL2849: if(*(( int*)
_temp2847) == Cyc_Tcenv_TunionRes){ _LL2856: _temp2855=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp2847)->f1; goto _LL2854; _LL2854: _temp2853=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp2847)->f2; goto _LL2850;} else{ goto _LL2851;} _LL2851: goto _LL2852;
_LL2850: tuf= _temp2853; tud= _temp2855; if( tud->is_xtunion != _temp2823){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp2857= Cyc_Absynpp_qvar2string(
_temp2827); xprintf("[x]tunion is different from type declaration %.*s",
_get_arr_size( _temp2857, 1u), _temp2857.curr);}));} goto _LL2848; _LL2852: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp2858= Cyc_Absynpp_qvar2string(
_temp2825); struct _tagged_arr _temp2859= Cyc_Absynpp_qvar2string( _temp2827);
xprintf("unbound field %.*s in type tunion %.*s", _get_arr_size( _temp2858, 1u),
_temp2858.curr, _get_arr_size( _temp2859, 1u), _temp2859.curr);}));{ struct Cyc_Tcutil_CVTEnv
_temp2860= cvtenv; _npop_handler( 0u); return _temp2860;} _LL2848:;};
_pop_handler();} else{ void* _temp2845=( void*) _exn_thrown; void* _temp2862=
_temp2845; _LL2864: if( _temp2862 == Cyc_Dict_Absent){ goto _LL2865;} else{ goto
_LL2866;} _LL2866: goto _LL2867; _LL2865: Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp2868= Cyc_Absynpp_qvar2string( _temp2825);
struct _tagged_arr _temp2869= Cyc_Absynpp_qvar2string( _temp2827); xprintf("unbound field %.*s in type tunion %.*s",
_get_arr_size( _temp2868, 1u), _temp2868.curr, _get_arr_size( _temp2869, 1u),
_temp2869.curr);})); return cvtenv; _LL2867:( void) _throw( _temp2862); _LL2863:;}}}*
_temp2695=( void*)({ struct Cyc_Absyn_KnownTunionfield_struct* _temp2870=(
struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct Cyc_Absyn_KnownTunionfield_struct));
_temp2870[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct _temp2871; _temp2871.tag=
Cyc_Absyn_KnownTunionfield; _temp2871.f1= tud; _temp2871.f2= tuf; _temp2871;});
_temp2870;}); _temp2831= tud; _temp2829= tuf; goto _LL2820;} _LL2820: { struct
Cyc_List_List* tvs= _temp2831->tvs; for( 0; _temp2691 != 0? tvs != 0: 0;(
_temp2691=(( struct Cyc_List_List*) _check_null( _temp2691))->tl, tvs=(( struct
Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)(( struct Cyc_List_List*)
_check_null( _temp2691))->hd; void* k1=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te,
cvtenv, k1, t1);} if( _temp2691 != 0){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr _temp2872= Cyc_Absynpp_qvar2string( _temp2831->name); struct
_tagged_arr _temp2873= Cyc_Absynpp_qvar2string( _temp2829->name); xprintf("too many type arguments for tunion %.*s.%.*s",
_get_arr_size( _temp2872, 1u), _temp2872.curr, _get_arr_size( _temp2873, 1u),
_temp2873.curr);}));} if( tvs != 0){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({
struct _tagged_arr _temp2874= Cyc_Absynpp_qvar2string( _temp2831->name); struct
_tagged_arr _temp2875= Cyc_Absynpp_qvar2string( _temp2829->name); xprintf("too few type arguments for tunion %.*s.%.*s",
_get_arr_size( _temp2874, 1u), _temp2874.curr, _get_arr_size( _temp2875, 1u),
_temp2875.curr);}));} goto _LL2816;} _LL2816:;} goto _LL2619; _LL2633: cvtenv=
Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_AnyKind,
_temp2706); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind,
_temp2704);{ void* _temp2876=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp2698))->v; void* _temp2882; struct Cyc_Absyn_Exp*
_temp2884; _LL2878: if(( unsigned int) _temp2876 > 1u?*(( int*) _temp2876) ==
Cyc_Absyn_Eq_constr: 0){ _LL2883: _temp2882=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2876)->f1; if(( unsigned int) _temp2882 > 1u?*(( int*) _temp2882) == Cyc_Absyn_Upper_b:
0){ _LL2885: _temp2884=(( struct Cyc_Absyn_Upper_b_struct*) _temp2882)->f1; goto
_LL2879;} else{ goto _LL2880;}} else{ goto _LL2880;} _LL2880: goto _LL2881;
_LL2879: if( ! Cyc_Tcutil_is_const_exp( te, _temp2884)){ Cyc_Tcutil_terr( loc,
_tag_arr("pointer bounds expression is not a constant", sizeof( unsigned char),
44u));} Cyc_Tcexp_tcExp( te, 0, _temp2884); if( ! Cyc_Tcutil_coerce_uint_typ( te,
_temp2884)){ Cyc_Tcutil_terr( loc, _tag_arr("pointer bounds expression is not an unsigned int",
sizeof( unsigned char), 49u));} Cyc_Evexp_eval_const_uint_exp( _temp2884); goto
_LL2877; _LL2881: goto _LL2877; _LL2877:;} goto _LL2619; _LL2635: goto _LL2619;
_LL2637: goto _LL2619; _LL2639: goto _LL2619; _LL2641: cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp2716); if( _temp2712 == 0? 1: !
Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*) _check_null( _temp2712))){
Cyc_Tcutil_terr( loc, _tag_arr("array bounds expression is not constant",
sizeof( unsigned char), 40u));} Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)
_check_null( _temp2712)); if( ! Cyc_Tcutil_coerce_uint_typ( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp2712))){ Cyc_Tcutil_terr( loc, _tag_arr("array bounds expression is not an unsigned int",
sizeof( unsigned char), 47u));} Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp2712)); goto _LL2619; _LL2643: { int num_convs= 0; int
seen_cdecl= 0; int seen_stdcall= 0; int seen_fastcall= 0; for( 0; _temp2720 != 0;
_temp2720=(( struct Cyc_List_List*) _check_null( _temp2720))->tl){ if( ! Cyc_Absyn_fntype_att((
void*)(( struct Cyc_List_List*) _check_null( _temp2720))->hd)){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp2886= Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp2720))->hd); xprintf("bad function type attribute %.*s",
_get_arr_size( _temp2886, 1u), _temp2886.curr);}));}{ void* _temp2887=( void*)((
struct Cyc_List_List*) _check_null( _temp2720))->hd; _LL2889: if( _temp2887 ==(
void*) Cyc_Absyn_Stdcall_att){ goto _LL2890;} else{ goto _LL2891;} _LL2891: if(
_temp2887 ==( void*) Cyc_Absyn_Cdecl_att){ goto _LL2892;} else{ goto _LL2893;}
_LL2893: if( _temp2887 ==( void*) Cyc_Absyn_Fastcall_att){ goto _LL2894;} else{
goto _LL2895;} _LL2895: goto _LL2896; _LL2890: if( ! seen_stdcall){ seen_stdcall=
1; ++ num_convs;} goto _LL2888; _LL2892: if( ! seen_cdecl){ seen_cdecl= 1; ++
num_convs;} goto _LL2888; _LL2894: if( ! seen_fastcall){ seen_fastcall= 1; ++
num_convs;} goto _LL2888; _LL2896: goto _LL2888; _LL2888:;}} if( num_convs > 1){
Cyc_Tcutil_terr( loc,( struct _tagged_arr) xprintf("function can't have multiple calling conventions"));}
Cyc_Tcutil_check_unique_tvars( loc,* _temp2737);{ struct Cyc_List_List* b=*
_temp2737; for( 0; b != 0; b=(( struct Cyc_List_List*) _check_null( b))->tl){((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd)->identity=
Cyc_Tcutil_new_tvar_id(); cvtenv.kind_env= Cyc_Tcutil_add_bound_tvar( cvtenv.kind_env,(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( b))->hd);{ void*
_temp2897=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( b))->hd)->kind))->v; void* _temp2903; _LL2899: if(( unsigned int)
_temp2897 > 1u?*(( int*) _temp2897) == Cyc_Absyn_Eq_constr: 0){ _LL2904:
_temp2903=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2897)->f1; if(
_temp2903 ==( void*) Cyc_Absyn_MemKind){ goto _LL2900;} else{ goto _LL2901;}}
else{ goto _LL2901;} _LL2901: goto _LL2902; _LL2900: Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp2905=*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( b))->hd)->name; xprintf("function attempts to abstract Mem type variable %.*s",
_get_arr_size( _temp2905, 1u), _temp2905.curr);})); goto _LL2898; _LL2902: goto
_LL2898; _LL2898:;}}}{ struct Cyc_Tcutil_CVTEnv _temp2906=({ struct Cyc_Tcutil_CVTEnv
_temp3033; _temp3033.kind_env= cvtenv.kind_env; _temp3033.free_vars= 0;
_temp3033.free_evars= 0; _temp3033;}); _temp2906= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2906,( void*) Cyc_Absyn_MemKind, _temp2730); for( 0; _temp2728 !=
0; _temp2728=(( struct Cyc_List_List*) _check_null( _temp2728))->tl){ _temp2906=
Cyc_Tcutil_i_check_valid_type( loc, te, _temp2906,( void*) Cyc_Absyn_MemKind,(*((
struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp2728))->hd)).f3);}
if( _temp2724 != 0){ if( _temp2726){(( int(*)( struct _tagged_arr msg)) Cyc_Tcutil_impos)(
_tag_arr("both c_vararg and cyc_vararg", sizeof( unsigned char), 29u));}{ int
_temp2909; void* _temp2911; void* _temp2913; struct Cyc_Absyn_Tqual _temp2915;
struct Cyc_Core_Opt* _temp2917; struct Cyc_Absyn_VarargInfo _temp2907=*(( struct
Cyc_Absyn_VarargInfo*) _check_null( _temp2724)); _LL2918: _temp2917= _temp2907.name;
goto _LL2916; _LL2916: _temp2915= _temp2907.tq; goto _LL2914; _LL2914: _temp2913=(
void*) _temp2907.type; goto _LL2912; _LL2912: _temp2911=( void*) _temp2907.rgn;
goto _LL2910; _LL2910: _temp2909= _temp2907.inject; goto _LL2908; _LL2908:
_temp2906= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2906,( void*) Cyc_Absyn_MemKind,
_temp2913);{ void* _temp2919= Cyc_Tcutil_compress( _temp2911); struct Cyc_Core_Opt*
_temp2929; int _temp2931; struct Cyc_Core_Opt* _temp2933; struct Cyc_Core_Opt*
_temp2935; struct Cyc_Absyn_Tvar* _temp2937; _LL2921: if(( unsigned int)
_temp2919 > 4u?*(( int*) _temp2919) == Cyc_Absyn_Evar: 0){ _LL2936: _temp2935=((
struct Cyc_Absyn_Evar_struct*) _temp2919)->f1; goto _LL2934; _LL2934: _temp2933=((
struct Cyc_Absyn_Evar_struct*) _temp2919)->f2; goto _LL2932; _LL2932: _temp2931=((
struct Cyc_Absyn_Evar_struct*) _temp2919)->f3; goto _LL2930; _LL2930: _temp2929=((
struct Cyc_Absyn_Evar_struct*) _temp2919)->f4; goto _LL2922;} else{ goto _LL2923;}
_LL2923: if(( unsigned int) _temp2919 > 4u?*(( int*) _temp2919) == Cyc_Absyn_VarType:
0){ _LL2938: _temp2937=(( struct Cyc_Absyn_VarType_struct*) _temp2919)->f1; goto
_LL2924;} else{ goto _LL2925;} _LL2925: if( _temp2919 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL2926;} else{ goto _LL2927;} _LL2927: goto _LL2928; _LL2922: { void*
vr_tvar=( void*)({ struct Cyc_Absyn_VarType_struct* _temp2939=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2939[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2940; _temp2940.tag= Cyc_Absyn_VarType; _temp2940.f1=
Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_RgnKind); _temp2940;}); _temp2939;}); Cyc_Tcutil_unify(
_temp2911, vr_tvar); goto _LL2920;} _LL2924: goto _LL2920; _LL2926: Cyc_Tcutil_terr(
loc, _tag_arr("varargs should be in a region, not heap-allocated", sizeof(
unsigned char), 50u)); goto _LL2920; _LL2928: goto _LL2920; _LL2920:;} _temp2906=
Cyc_Tcutil_i_check_valid_type( loc, te, _temp2906,( void*) Cyc_Absyn_RgnKind,
_temp2911); if( _temp2909){ void* _temp2941= Cyc_Tcutil_compress( _temp2913);
struct Cyc_Absyn_TunionInfo _temp2947; void* _temp2949; struct Cyc_List_List*
_temp2951; void* _temp2953; struct Cyc_Absyn_Tuniondecl* _temp2955; _LL2943: if((
unsigned int) _temp2941 > 4u?*(( int*) _temp2941) == Cyc_Absyn_TunionType: 0){
_LL2948: _temp2947=(( struct Cyc_Absyn_TunionType_struct*) _temp2941)->f1;
_LL2954: _temp2953=( void*) _temp2947.tunion_info; if(*(( int*) _temp2953) ==
Cyc_Absyn_KnownTunion){ _LL2956: _temp2955=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp2953)->f1; goto _LL2952;} else{ goto _LL2945;} _LL2952: _temp2951=
_temp2947.targs; goto _LL2950; _LL2950: _temp2949=( void*) _temp2947.rgn; goto
_LL2944;} else{ goto _LL2945;} _LL2945: goto _LL2946; _LL2944:{ void* _temp2957=
Cyc_Tcutil_compress( _temp2949); struct Cyc_Core_Opt* _temp2967; int _temp2969;
struct Cyc_Core_Opt* _temp2971; struct Cyc_Core_Opt* _temp2973; struct Cyc_Absyn_Tvar*
_temp2975; _LL2959: if(( unsigned int) _temp2957 > 4u?*(( int*) _temp2957) ==
Cyc_Absyn_Evar: 0){ _LL2974: _temp2973=(( struct Cyc_Absyn_Evar_struct*)
_temp2957)->f1; goto _LL2972; _LL2972: _temp2971=(( struct Cyc_Absyn_Evar_struct*)
_temp2957)->f2; goto _LL2970; _LL2970: _temp2969=(( struct Cyc_Absyn_Evar_struct*)
_temp2957)->f3; goto _LL2968; _LL2968: _temp2967=(( struct Cyc_Absyn_Evar_struct*)
_temp2957)->f4; goto _LL2960;} else{ goto _LL2961;} _LL2961: if(( unsigned int)
_temp2957 > 4u?*(( int*) _temp2957) == Cyc_Absyn_VarType: 0){ _LL2976: _temp2975=((
struct Cyc_Absyn_VarType_struct*) _temp2957)->f1; goto _LL2962;} else{ goto
_LL2963;} _LL2963: if( _temp2957 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL2964;}
else{ goto _LL2965;} _LL2965: goto _LL2966; _LL2960: Cyc_Tcutil_unify( _temp2911,
_temp2949); goto _LL2958; _LL2962: goto _LL2958; _LL2964: Cyc_Tcutil_terr( loc,
_tag_arr("injected vararg [x]tunion should be in a region, not heap-allocated",
sizeof( unsigned char), 68u)); goto _LL2958; _LL2966: goto _LL2958; _LL2958:;}
goto _LL2942; _LL2946: Cyc_Tcutil_terr( loc, _tag_arr("can't inject a non-[x]tunion type",
sizeof( unsigned char), 34u)); goto _LL2942; _LL2942:;}}}{ struct Cyc_List_List*
rpo= _temp2722; for( 0; rpo != 0; rpo=(( struct Cyc_List_List*) _check_null( rpo))->tl){
struct _tuple8 _temp2979; void* _temp2980; void* _temp2982; struct _tuple8*
_temp2977=( struct _tuple8*)(( struct Cyc_List_List*) _check_null( rpo))->hd;
_temp2979=* _temp2977; _LL2983: _temp2982= _temp2979.f1; goto _LL2981; _LL2981:
_temp2980= _temp2979.f2; goto _LL2978; _LL2978: _temp2906= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2906,( void*) Cyc_Absyn_RgnKind, _temp2982); _temp2906= Cyc_Tcutil_i_check_valid_type(
loc, te, _temp2906,( void*) Cyc_Absyn_RgnKind, _temp2980);}} if(* _temp2734 != 0){
_temp2906= Cyc_Tcutil_i_check_valid_type( loc, te, _temp2906,( void*) Cyc_Absyn_EffKind,(
void*)(( struct Cyc_Core_Opt*) _check_null(* _temp2734))->v);} else{ struct Cyc_Absyn_Tvar*
e= Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_EffKind); _temp2906.free_vars= Cyc_Tcutil_add_free_tvar(
loc, _temp2906.free_vars, e);{ struct Cyc_List_List* effect= 0;{ struct Cyc_List_List*
tvs= _temp2906.free_vars; for( 0; tvs != 0; tvs=(( struct Cyc_List_List*)
_check_null( tvs))->tl){ void* _temp2984=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind))->v; void* _temp2992; void*
_temp2994; _LL2986: if(( unsigned int) _temp2984 > 1u?*(( int*) _temp2984) ==
Cyc_Absyn_Eq_constr: 0){ _LL2993: _temp2992=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2984)->f1; if( _temp2992 ==( void*) Cyc_Absyn_RgnKind){ goto _LL2987;}
else{ goto _LL2988;}} else{ goto _LL2988;} _LL2988: if(( unsigned int) _temp2984
> 1u?*(( int*) _temp2984) == Cyc_Absyn_Eq_constr: 0){ _LL2995: _temp2994=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2984)->f1; if( _temp2994 ==( void*) Cyc_Absyn_EffKind){
goto _LL2989;} else{ goto _LL2990;}} else{ goto _LL2990;} _LL2990: goto _LL2991;
_LL2987: effect=({ struct Cyc_List_List* _temp2996=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2996->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2997=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2997[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2998; _temp2998.tag= Cyc_Absyn_AccessEff;
_temp2998.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp2999=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2999[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3000; _temp3000.tag= Cyc_Absyn_VarType;
_temp3000.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3000;}); _temp2999;})); _temp2998;}); _temp2997;})); _temp2996->tl= effect;
_temp2996;}); goto _LL2985; _LL2989: effect=({ struct Cyc_List_List* _temp3001=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3001->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp3002=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp3002[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp3003; _temp3003.tag= Cyc_Absyn_VarType; _temp3003.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp3003;}); _temp3002;})); _temp3001->tl= effect; _temp3001;}); goto _LL2985;
_LL2991: goto _LL2985; _LL2985:;}}{ struct Cyc_List_List* ts= _temp2906.free_evars;
for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){ void*
_temp3004= Cyc_Tcutil_typ_kind(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);
_LL3006: if( _temp3004 ==( void*) Cyc_Absyn_RgnKind){ goto _LL3007;} else{ goto
_LL3008;} _LL3008: if( _temp3004 ==( void*) Cyc_Absyn_EffKind){ goto _LL3009;}
else{ goto _LL3010;} _LL3010: goto _LL3011; _LL3007: effect=({ struct Cyc_List_List*
_temp3012=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3012->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3013=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3013[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3014; _temp3014.tag= Cyc_Absyn_AccessEff;
_temp3014.f1=( void*)(( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);
_temp3014;}); _temp3013;})); _temp3012->tl= effect; _temp3012;}); goto _LL3005;
_LL3009: effect=({ struct Cyc_List_List* _temp3015=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3015->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( ts))->hd); _temp3015->tl= effect; _temp3015;});
goto _LL3005; _LL3011: goto _LL3005; _LL3005:;}}* _temp2734=({ struct Cyc_Core_Opt*
_temp3016=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3016->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3017=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3017[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3018; _temp3018.tag= Cyc_Absyn_JoinEff;
_temp3018.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
effect); _temp3018;}); _temp3017;})); _temp3016;});}} if(* _temp2737 != 0){
struct Cyc_List_List* bs=* _temp2737; for( 0; bs != 0; bs=(( struct Cyc_List_List*)
_check_null( bs))->tl){ struct Cyc_Absyn_Conref* _temp3019=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( bs))->hd)->kind); void* _temp3020=( void*)
_temp3019->v; void* _temp3028; _LL3022: if( _temp3020 ==( void*) Cyc_Absyn_No_constr){
goto _LL3023;} else{ goto _LL3024;} _LL3024: if(( unsigned int) _temp3020 > 1u?*((
int*) _temp3020) == Cyc_Absyn_Eq_constr: 0){ _LL3029: _temp3028=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp3020)->f1; if( _temp3028 ==( void*) Cyc_Absyn_MemKind){
goto _LL3025;} else{ goto _LL3026;}} else{ goto _LL3026;} _LL3026: goto _LL3027;
_LL3023: Cyc_Tcutil_warn( loc,( struct _tagged_arr)({ struct _tagged_arr
_temp3030=*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( bs))->hd)->name;
xprintf("Type variable %.*s unconstrained, assuming boxed", _get_arr_size(
_temp3030, 1u), _temp3030.curr);})); goto _LL3025; _LL3025:( void*)( _temp3019->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3031=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3031[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3032; _temp3032.tag= Cyc_Absyn_Eq_constr;
_temp3032.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3032;}); _temp3031;})));
goto _LL3021; _LL3027: goto _LL3021; _LL3021:;}} cvtenv.kind_env= Cyc_Tcutil_remove_bound_tvars(
_temp2906.kind_env,* _temp2737); _temp2906.free_vars= Cyc_Tcutil_remove_bound_tvars(
_temp2906.free_vars,* _temp2737);{ struct Cyc_List_List* tvs= _temp2906.free_vars;
for( 0; tvs != 0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar( cvtenv.free_vars,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd);}}{ struct Cyc_List_List* evs=
_temp2906.free_evars; for( 0; evs != 0; evs=(( struct Cyc_List_List*)
_check_null( evs))->tl){ cvtenv.free_evars= Cyc_Tcutil_add_free_evar( cvtenv.free_evars,(
void*)(( struct Cyc_List_List*) _check_null( evs))->hd);}} goto _LL2619;}}
_LL2645: for( 0; _temp2738 != 0; _temp2738=(( struct Cyc_List_List*) _check_null(
_temp2738))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*)
Cyc_Absyn_MemKind,(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null(
_temp2738))->hd)).f2);} goto _LL2619; _LL2647:{ struct _RegionHandle _temp3034=
_new_region(); struct _RegionHandle* sprev_rgn=& _temp3034; _push_region(
sprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for( 0; _temp2740 != 0;
_temp2740=(( struct Cyc_List_List*) _check_null( _temp2740))->tl){ struct Cyc_Absyn_Structfield
_temp3037; struct Cyc_List_List* _temp3038; struct Cyc_Core_Opt* _temp3040; void*
_temp3042; struct Cyc_Absyn_Tqual _temp3044; struct _tagged_arr* _temp3046;
struct Cyc_Absyn_Structfield* _temp3035=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp2740))->hd; _temp3037=* _temp3035;
_LL3047: _temp3046= _temp3037.name; goto _LL3045; _LL3045: _temp3044= _temp3037.tq;
goto _LL3043; _LL3043: _temp3042=( void*) _temp3037.type; goto _LL3041; _LL3041:
_temp3040= _temp3037.width; goto _LL3039; _LL3039: _temp3038= _temp3037.attributes;
goto _LL3036; _LL3036: if((( int(*)( int(* compare)( struct _tagged_arr*, struct
_tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields, _temp3046)){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp3048=* _temp3046; xprintf("duplicate field %.*s in struct",
_get_arr_size( _temp3048, 1u), _temp3048.curr);}));} if( Cyc_String_strcmp(*
_temp3046, _tag_arr("", sizeof( unsigned char), 1u)) != 0){ prev_fields=({
struct Cyc_List_List* _temp3049=( struct Cyc_List_List*) _region_malloc(
sprev_rgn, sizeof( struct Cyc_List_List)); _temp3049->hd=( void*) _temp3046;
_temp3049->tl= prev_fields; _temp3049;});} cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_MemKind, _temp3042); if( _temp3040 != 0){
unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp3040))->v)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp3050=* _temp3046; xprintf("bitfield %.*s does not have constant width",
_get_arr_size( _temp3050, 1u), _temp3050.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp3040))->v);}{
void* _temp3051= Cyc_Tcutil_compress( _temp3042); void* _temp3057; void*
_temp3059; _LL3053: if(( unsigned int) _temp3051 > 4u?*(( int*) _temp3051) ==
Cyc_Absyn_IntType: 0){ _LL3060: _temp3059=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3051)->f1; goto _LL3058; _LL3058: _temp3057=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3051)->f2; goto _LL3054;} else{ goto _LL3055;} _LL3055: goto _LL3056;
_LL3054:{ void* _temp3061= _temp3057; _LL3063: if( _temp3061 ==( void*) Cyc_Absyn_B1){
goto _LL3064;} else{ goto _LL3065;} _LL3065: if( _temp3061 ==( void*) Cyc_Absyn_B2){
goto _LL3066;} else{ goto _LL3067;} _LL3067: if( _temp3061 ==( void*) Cyc_Absyn_B4){
goto _LL3068;} else{ goto _LL3069;} _LL3069: if( _temp3061 ==( void*) Cyc_Absyn_B8){
goto _LL3070;} else{ goto _LL3062;} _LL3064: if( w > 8){ Cyc_Tcutil_terr( loc,
_tag_arr("bitfield larger than type", sizeof( unsigned char), 26u));} goto
_LL3062; _LL3066: if( w > 16){ Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u));} goto _LL3062; _LL3068: if( w > 32){ Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u));} goto
_LL3062; _LL3070: if( w > 64){ Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u));} goto _LL3062; _LL3062:;} goto _LL3052; _LL3056:
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp3071=*
_temp3046; struct _tagged_arr _temp3072= Cyc_Absynpp_typ2string( _temp3042);
xprintf("bitfield %.*s must have integral type but has type %.*s", _get_arr_size(
_temp3071, 1u), _temp3071.curr, _get_arr_size( _temp3072, 1u), _temp3072.curr);}));
goto _LL3052; _LL3052:;}} for( 0; _temp3038 != 0; _temp3038=(( struct Cyc_List_List*)
_check_null( _temp3038))->tl){ void* _temp3073=( void*)(( struct Cyc_List_List*)
_check_null( _temp3038))->hd; int _temp3081; _LL3075: if( _temp3073 ==( void*)
Cyc_Absyn_Packed_att){ goto _LL3076;} else{ goto _LL3077;} _LL3077: if((
unsigned int) _temp3073 > 16u?*(( int*) _temp3073) == Cyc_Absyn_Aligned_att: 0){
_LL3082: _temp3081=(( struct Cyc_Absyn_Aligned_att_struct*) _temp3073)->f1; goto
_LL3078;} else{ goto _LL3079;} _LL3079: goto _LL3080; _LL3076: continue; _LL3078:
continue; _LL3080: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp3083= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null( _temp3038))->hd); struct _tagged_arr _temp3084=* _temp3046; xprintf("bad attribute %.*s on struct member %.*s",
_get_arr_size( _temp3083, 1u), _temp3083.curr, _get_arr_size( _temp3084, 1u),
_temp3084.curr);})); goto _LL3074; _LL3074:;}}}; _pop_region( sprev_rgn);} goto
_LL2619; _LL2649:{ struct _RegionHandle _temp3085= _new_region(); struct
_RegionHandle* uprev_rgn=& _temp3085; _push_region( uprev_rgn);{ struct Cyc_List_List*
prev_fields= 0; for( 0; _temp2742 != 0; _temp2742=(( struct Cyc_List_List*)
_check_null( _temp2742))->tl){ struct Cyc_Absyn_Structfield _temp3088; struct
Cyc_List_List* _temp3089; struct Cyc_Core_Opt* _temp3091; void* _temp3093;
struct Cyc_Absyn_Tqual _temp3095; struct _tagged_arr* _temp3097; struct Cyc_Absyn_Structfield*
_temp3086=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2742))->hd; _temp3088=* _temp3086; _LL3098: _temp3097= _temp3088.name; goto
_LL3096; _LL3096: _temp3095= _temp3088.tq; goto _LL3094; _LL3094: _temp3093=(
void*) _temp3088.type; goto _LL3092; _LL3092: _temp3091= _temp3088.width; goto
_LL3090; _LL3090: _temp3089= _temp3088.attributes; goto _LL3087; _LL3087: if(((
int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List*
l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,
_temp3097)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr
_temp3099=* _temp3097; xprintf("duplicate field %.*s in struct", _get_arr_size(
_temp3099, 1u), _temp3099.curr);}));} if( Cyc_String_strcmp(* _temp3097,
_tag_arr("", sizeof( unsigned char), 1u)) != 0){ prev_fields=({ struct Cyc_List_List*
_temp3100=( struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp3100->hd=( void*) _temp3097; _temp3100->tl= prev_fields; _temp3100;});}
cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*) Cyc_Absyn_MemKind,
_temp3093); if( ! Cyc_Tcutil_bits_only( _temp3093)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp3101=* _temp3097; xprintf("%.*s has a type that is possibly incompatible with other members of union",
_get_arr_size( _temp3101, 1u), _temp3101.curr);}));} if( _temp3091 != 0){
unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp3091))->v)){ Cyc_Tcutil_terr( loc,(
struct _tagged_arr)({ struct _tagged_arr _temp3102=* _temp3097; xprintf("bitfield %.*s does not have constant width",
_get_arr_size( _temp3102, 1u), _temp3102.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp3091))->v);}{
void* _temp3103= Cyc_Tcutil_compress( _temp3093); void* _temp3109; void*
_temp3111; _LL3105: if(( unsigned int) _temp3103 > 4u?*(( int*) _temp3103) ==
Cyc_Absyn_IntType: 0){ _LL3112: _temp3111=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3103)->f1; goto _LL3110; _LL3110: _temp3109=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3103)->f2; goto _LL3106;} else{ goto _LL3107;} _LL3107: goto _LL3108;
_LL3106:{ void* _temp3113= _temp3109; _LL3115: if( _temp3113 ==( void*) Cyc_Absyn_B1){
goto _LL3116;} else{ goto _LL3117;} _LL3117: if( _temp3113 ==( void*) Cyc_Absyn_B2){
goto _LL3118;} else{ goto _LL3119;} _LL3119: if( _temp3113 ==( void*) Cyc_Absyn_B4){
goto _LL3120;} else{ goto _LL3121;} _LL3121: if( _temp3113 ==( void*) Cyc_Absyn_B8){
goto _LL3122;} else{ goto _LL3114;} _LL3116: if( w > 8){ Cyc_Tcutil_terr( loc,
_tag_arr("bitfield larger than type", sizeof( unsigned char), 26u));} goto
_LL3114; _LL3118: if( w > 16){ Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u));} goto _LL3114; _LL3120: if( w > 32){ Cyc_Tcutil_terr(
loc, _tag_arr("bitfield larger than type", sizeof( unsigned char), 26u));} goto
_LL3114; _LL3122: if( w > 64){ Cyc_Tcutil_terr( loc, _tag_arr("bitfield larger than type",
sizeof( unsigned char), 26u));} goto _LL3114; _LL3114:;} goto _LL3104; _LL3108:
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp3123=*
_temp3097; struct _tagged_arr _temp3124= Cyc_Absynpp_typ2string( _temp3093);
xprintf("bitfield %.*s must have integral type but has type %.*s", _get_arr_size(
_temp3123, 1u), _temp3123.curr, _get_arr_size( _temp3124, 1u), _temp3124.curr);}));
goto _LL3104; _LL3104:;}} for( 0; _temp3089 != 0; _temp3089=(( struct Cyc_List_List*)
_check_null( _temp3089))->tl){ void* _temp3125=( void*)(( struct Cyc_List_List*)
_check_null( _temp3089))->hd; int _temp3133; _LL3127: if( _temp3125 ==( void*)
Cyc_Absyn_Packed_att){ goto _LL3128;} else{ goto _LL3129;} _LL3129: if((
unsigned int) _temp3125 > 16u?*(( int*) _temp3125) == Cyc_Absyn_Aligned_att: 0){
_LL3134: _temp3133=(( struct Cyc_Absyn_Aligned_att_struct*) _temp3125)->f1; goto
_LL3130;} else{ goto _LL3131;} _LL3131: goto _LL3132; _LL3128: continue; _LL3130:
continue; _LL3132: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp3135= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null( _temp3089))->hd); struct _tagged_arr _temp3136=* _temp3097; xprintf("bad attribute %.*s on struct member %.*s",
_get_arr_size( _temp3135, 1u), _temp3135.curr, _get_arr_size( _temp3136, 1u),
_temp3136.curr);})); goto _LL3126; _LL3126:;}}}; _pop_region( uprev_rgn);} goto
_LL2619; _LL2651: if( _temp2749 == 0){ if(* _temp2746 == 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr) xprintf("bad anonymous struct")); return cvtenv;}{
struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**) _check_null(*
_temp2746));;}} if(* _temp2746 == 0){ struct _handler_cons _temp3137;
_push_handler(& _temp3137);{ int _temp3139= 0; if( setjmp( _temp3137.handler)){
_temp3139= 1;} if( ! _temp3139){* _temp2746=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc,( struct _tuple1*) _check_null( _temp2749));; _pop_handler();} else{
void* _temp3138=( void*) _exn_thrown; void* _temp3141= _temp3138; _LL3143: if(
_temp3141 == Cyc_Dict_Absent){ goto _LL3144;} else{ goto _LL3145;} _LL3145: goto
_LL3146; _LL3144: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp3147= Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null(
_temp2749)); xprintf("unbound type struct %.*s", _get_arr_size( _temp3147, 1u),
_temp3147.curr);})); return cvtenv; _LL3146:( void) _throw( _temp3141); _LL3142:;}}}{
struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**) _check_null(*
_temp2746)); if( sd->name != 0){*(( struct _tuple1*) _check_null( _temp2749))=((
struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)[
_check_known_subscript_notnull( 1u, 0)];}{ struct Cyc_List_List* tvs= sd->tvs;
int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2747); if( lvs != largs){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp3148= Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp2749)); int _temp3149= lvs; int _temp3150=
largs; xprintf("struct %.*s expects %d type arguments but was given %d",
_get_arr_size( _temp3148, 1u), _temp3148.curr, _temp3149, _temp3150);}));} for(
0; _temp2747 != 0;( _temp2747=(( struct Cyc_List_List*) _check_null( _temp2747))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2747))->hd; void* k1=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} goto _LL2619;}} _LL2653: if( _temp2756 == 0){ Cyc_Tcutil_terr(
loc, _tag_arr("missing union name", sizeof( unsigned char), 19u)); return cvtenv;}
if(* _temp2753 == 0){ struct _handler_cons _temp3151; _push_handler(& _temp3151);{
int _temp3153= 0; if( setjmp( _temp3151.handler)){ _temp3153= 1;} if( !
_temp3153){* _temp2753=( struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl(
te, loc,( struct _tuple1*) _check_null( _temp2756));; _pop_handler();} else{
void* _temp3152=( void*) _exn_thrown; void* _temp3155= _temp3152; _LL3157: if(
_temp3155 == Cyc_Dict_Absent){ goto _LL3158;} else{ goto _LL3159;} _LL3159: goto
_LL3160; _LL3158: Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct
_tagged_arr _temp3161= Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null(
_temp2756)); xprintf("unbound type union %.*s", _get_arr_size( _temp3161, 1u),
_temp3161.curr);})); return cvtenv; _LL3160:( void) _throw( _temp3155); _LL3156:;}}}{
struct Cyc_Absyn_Uniondecl* ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null(*
_temp2753)); if( ud->name != 0){*(( struct _tuple1*) _check_null( _temp2756))=((
struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( ud->name))->v)[
_check_known_subscript_notnull( 1u, 0)];}{ struct Cyc_List_List* tvs= ud->tvs;
int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2754); if( lvs != largs){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp3162= Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp2756)); int _temp3163= lvs; int _temp3164=
largs; xprintf("union %.*s expects %d type arguments but was given %d",
_get_arr_size( _temp3162, 1u), _temp3162.curr, _temp3163, _temp3164);}));} for(
0; _temp2754 != 0;( _temp2754=(( struct Cyc_List_List*) _check_null( _temp2754))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2754))->hd; void* k1=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv, k1, t1);} goto _LL2619;}} _LL2655: { struct Cyc_Absyn_Typedefdecl*
td;{ struct _handler_cons _temp3165; _push_handler(& _temp3165);{ int _temp3167=
0; if( setjmp( _temp3165.handler)){ _temp3167= 1;} if( ! _temp3167){ td= Cyc_Tcenv_lookup_typedefdecl(
te, loc, _temp2763);; _pop_handler();} else{ void* _temp3166=( void*)
_exn_thrown; void* _temp3169= _temp3166; _LL3171: if( _temp3169 == Cyc_Dict_Absent){
goto _LL3172;} else{ goto _LL3173;} _LL3173: goto _LL3174; _LL3172: Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp3175= Cyc_Absynpp_qvar2string(
_temp2763); xprintf("unbound typedef name %.*s", _get_arr_size( _temp3175, 1u),
_temp3175.curr);})); return cvtenv; _LL3174:( void) _throw( _temp3169); _LL3170:;}}}
_temp2763[ _check_known_subscript_notnull( 1u, 0)]=( td->name)[
_check_known_subscript_notnull( 1u, 0)];{ struct Cyc_List_List* tvs= td->tvs;
struct Cyc_List_List* ts= _temp2761; struct Cyc_List_List* inst= 0; for( 0; ts
!= 0? tvs != 0: 0;( ts=(( struct Cyc_List_List*) _check_null( ts))->tl, tvs=((
struct Cyc_List_List*) _check_null( tvs))->tl)){ void* k=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd)->kind); cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te,
cvtenv, k,( void*)(( struct Cyc_List_List*) _check_null( ts))->hd); inst=({
struct Cyc_List_List* _temp3176=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3176->hd=( void*)({ struct _tuple5* _temp3177=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp3177->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd; _temp3177->f2=( void*)(( struct
Cyc_List_List*) _check_null( ts))->hd; _temp3177;}); _temp3176->tl= inst;
_temp3176;});} if( tvs != 0? 1: ts != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp3178= Cyc_Absynpp_qvar2string( _temp2763);
xprintf("typedef %.*s expects a different number of arguments", _get_arr_size(
_temp3178, 1u), _temp3178.curr);})); return cvtenv;}{ void* new_typ= Cyc_Tcutil_substitute(
inst,( void*) td->defn);* _temp2760=({ struct Cyc_Core_Opt* _temp3179=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3179->v=( void*)
new_typ; _temp3179;}); goto _LL2619;}}} _LL2657: goto _LL2619; _LL2659:
_temp2767= _temp2765; goto _LL2661; _LL2661: cvtenv= Cyc_Tcutil_i_check_valid_type(
loc, te, cvtenv,( void*) Cyc_Absyn_RgnKind, _temp2767); goto _LL2619; _LL2663:
for( 0; _temp2769 != 0; _temp2769=(( struct Cyc_List_List*) _check_null(
_temp2769))->tl){ cvtenv= Cyc_Tcutil_i_check_valid_type( loc, te, cvtenv,( void*)
Cyc_Absyn_EffKind,( void*)(( struct Cyc_List_List*) _check_null( _temp2769))->hd);}
goto _LL2619; _LL2619:;} if( ! Cyc_Tcutil_kind_leq( Cyc_Tcutil_typ_kind( t),
expected_kind)){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr
_temp3180= Cyc_Absynpp_typ2string( t); struct _tagged_arr _temp3181= Cyc_Absynpp_kind2string(
Cyc_Tcutil_typ_kind( t)); struct _tagged_arr _temp3182= Cyc_Absynpp_kind2string(
expected_kind); xprintf("type %.*s has kind %.*s but as used here needs kind %.*s",
_get_arr_size( _temp3180, 1u), _temp3180.curr, _get_arr_size( _temp3181, 1u),
_temp3181.curr, _get_arr_size( _temp3182, 1u), _temp3182.curr);}));} return
cvtenv;} struct Cyc_List_List* Cyc_Tcutil_check_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* kind_env, void*
expected_kind, void* t){ struct Cyc_Tcutil_CVTEnv _temp3183= Cyc_Tcutil_i_check_valid_type(
loc, te,({ struct Cyc_Tcutil_CVTEnv _temp3204; _temp3204.kind_env= kind_env;
_temp3204.free_vars= 0; _temp3204.free_evars= 0; _temp3204;}), expected_kind, t);{
struct Cyc_List_List* vs= _temp3183.free_vars; for( 0; vs != 0; vs=(( struct Cyc_List_List*)
_check_null( vs))->tl){ Cyc_Tcutil_fast_add_free_tvar( kind_env,( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( vs))->hd);}}{ struct Cyc_List_List* evs=
_temp3183.free_evars; for( 0; evs != 0; evs=(( struct Cyc_List_List*)
_check_null( evs))->tl){ void* _temp3184= Cyc_Tcutil_compress(( void*)(( struct
Cyc_List_List*) _check_null( evs))->hd); struct Cyc_Core_Opt* _temp3190; struct
Cyc_Core_Opt** _temp3192; int _temp3193; struct Cyc_Core_Opt* _temp3195; struct
Cyc_Core_Opt* _temp3197; _LL3186: if(( unsigned int) _temp3184 > 4u?*(( int*)
_temp3184) == Cyc_Absyn_Evar: 0){ _LL3198: _temp3197=(( struct Cyc_Absyn_Evar_struct*)
_temp3184)->f1; goto _LL3196; _LL3196: _temp3195=(( struct Cyc_Absyn_Evar_struct*)
_temp3184)->f2; goto _LL3194; _LL3194: _temp3193=(( struct Cyc_Absyn_Evar_struct*)
_temp3184)->f3; goto _LL3191; _LL3191: _temp3190=(( struct Cyc_Absyn_Evar_struct*)
_temp3184)->f4; _temp3192=&(( struct Cyc_Absyn_Evar_struct*) _temp3184)->f4;
goto _LL3187;} else{ goto _LL3188;} _LL3188: goto _LL3189; _LL3187: if(*
_temp3192 == 0){* _temp3192=({ struct Cyc_Core_Opt* _temp3199=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3199->v=( void*) kind_env;
_temp3199;});} else{ struct Cyc_List_List* _temp3200=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null(* _temp3192))->v; struct Cyc_List_List*
_temp3201= 0; for( 0; _temp3200 != 0; _temp3200=(( struct Cyc_List_List*)
_check_null( _temp3200))->tl){ if((( int(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp, kind_env,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp3200))->hd)){ _temp3201=({ struct Cyc_List_List* _temp3202=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3202->hd=(
void*)(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp3200))->hd);
_temp3202->tl= _temp3201; _temp3202;});}}* _temp3192=({ struct Cyc_Core_Opt*
_temp3203=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3203->v=( void*) _temp3201; _temp3203;});} goto _LL3185; _LL3189: goto
_LL3185; _LL3185:;}} return _temp3183.free_vars;} void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, void* t){ struct
Cyc_List_List* _temp3205= Cyc_Tcutil_check_valid_type( loc, te, 0,( void*) Cyc_Absyn_MemKind,
t);{ struct Cyc_List_List* x= _temp3205; for( 0; x != 0; x=(( struct Cyc_List_List*)
_check_null( x))->tl){ struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( x))->hd)->kind); void* _temp3206=( void*) c->v;
void* _temp3214; _LL3208: if( _temp3206 ==( void*) Cyc_Absyn_No_constr){ goto
_LL3209;} else{ goto _LL3210;} _LL3210: if(( unsigned int) _temp3206 > 1u?*((
int*) _temp3206) == Cyc_Absyn_Eq_constr: 0){ _LL3215: _temp3214=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp3206)->f1; if( _temp3214 ==( void*) Cyc_Absyn_MemKind){
goto _LL3211;} else{ goto _LL3212;}} else{ goto _LL3212;} _LL3212: goto _LL3213;
_LL3209: goto _LL3211; _LL3211:( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3216=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3216[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3217; _temp3217.tag= Cyc_Absyn_Eq_constr;
_temp3217.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp3217;}); _temp3216;})));
goto _LL3207; _LL3213: goto _LL3207; _LL3207:;}} if( _temp3205 != 0){ void*
_temp3218= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp3224; struct
Cyc_List_List* _temp3226; struct Cyc_List_List* _temp3228; struct Cyc_Absyn_VarargInfo*
_temp3230; int _temp3232; struct Cyc_List_List* _temp3234; void* _temp3236;
struct Cyc_Core_Opt* _temp3238; struct Cyc_List_List* _temp3240; struct Cyc_List_List**
_temp3242; _LL3220: if(( unsigned int) _temp3218 > 4u?*(( int*) _temp3218) ==
Cyc_Absyn_FnType: 0){ _LL3225: _temp3224=(( struct Cyc_Absyn_FnType_struct*)
_temp3218)->f1; _LL3241: _temp3240= _temp3224.tvars; _temp3242=&((( struct Cyc_Absyn_FnType_struct*)
_temp3218)->f1).tvars; goto _LL3239; _LL3239: _temp3238= _temp3224.effect; goto
_LL3237; _LL3237: _temp3236=( void*) _temp3224.ret_typ; goto _LL3235; _LL3235:
_temp3234= _temp3224.args; goto _LL3233; _LL3233: _temp3232= _temp3224.c_varargs;
goto _LL3231; _LL3231: _temp3230= _temp3224.cyc_varargs; goto _LL3229; _LL3229:
_temp3228= _temp3224.rgn_po; goto _LL3227; _LL3227: _temp3226= _temp3224.attributes;
goto _LL3221;} else{ goto _LL3222;} _LL3222: goto _LL3223; _LL3221: if(*
_temp3242 == 0){* _temp3242= _temp3205;} else{ if( _temp3205 != 0){ struct
_tagged_arr _temp3243=*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp3205))->hd)->name; if(*(( const unsigned char*)
_check_unknown_subscript( _temp3243, sizeof( unsigned char), 0)) =='?'){ Cyc_Tcutil_terr(
loc,( struct _tagged_arr) xprintf("You failed to abstract over the generated type variables. \nRemove the bound type variables or else make the effects of the type explicit\nand then abstract the free type variables properly."));}
else{ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp3244=
_temp3243; xprintf("unbound type variable %.*s", _get_arr_size( _temp3244, 1u),
_temp3244.curr);}));}}} goto _LL3219; _LL3223: Cyc_Tcutil_terr( loc,( struct
_tagged_arr)({ struct _tagged_arr _temp3245=*(( struct Cyc_Absyn_Tvar*)(( struct
Cyc_List_List*) _check_null( _temp3205))->hd)->name; xprintf("unbound type variable %.*s",
_get_arr_size( _temp3245, 1u), _temp3245.curr);})); return; _LL3219:;}} void Cyc_Tcutil_add_tvar_identity(
struct Cyc_Absyn_Tvar* tv){ if( tv->identity == 0){ tv->identity= Cyc_Tcutil_new_tvar_id();}}
void Cyc_Tcutil_add_tvar_identities( struct Cyc_List_List* tvs){(( void(*)( void(*
f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Tcutil_add_tvar_identity,
tvs);} void Cyc_Tcutil_check_type( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* bound_tvars, void* expected_kind, void* t){ struct Cyc_List_List*
_temp3246= Cyc_Tcutil_check_valid_type( loc, te, bound_tvars, expected_kind, t);
struct Cyc_List_List* _temp3247= Cyc_Tcutil_remove_bound_tvars( _temp3246,
bound_tvars); struct Cyc_List_List* fs= _temp3247; for( 0; fs != 0; fs=(( struct
Cyc_List_List*) _check_null( fs))->tl){ struct _tagged_arr* _temp3248=(( struct
Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( fs))->hd)->name; Cyc_Tcutil_terr(
loc,( struct _tagged_arr)({ struct _tagged_arr _temp3249=* _temp3248; struct
_tagged_arr _temp3250= Cyc_Absynpp_typ2string( t); xprintf("unbound type variable %.*s in type %.*s",
_get_arr_size( _temp3249, 1u), _temp3249.curr, _get_arr_size( _temp3250, 1u),
_temp3250.curr);}));}} static void Cyc_Tcutil_check_unique_unsorted( int(* cmp)(
void*, void*), struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc,
struct _tagged_arr(* a2string)( void*), struct _tagged_arr msg){ for( 0; vs != 0;
vs=(( struct Cyc_List_List*) _check_null( vs))->tl){ struct Cyc_List_List* vs2=((
struct Cyc_List_List*) _check_null( vs))->tl; for( 0; vs2 != 0; vs2=(( struct
Cyc_List_List*) _check_null( vs2))->tl){ if( cmp(( void*)(( struct Cyc_List_List*)
_check_null( vs))->hd,( void*)(( struct Cyc_List_List*) _check_null( vs2))->hd)
== 0){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp3251=
msg; struct _tagged_arr _temp3252= a2string(( void*)(( struct Cyc_List_List*)
_check_null( vs))->hd); xprintf("%.*s: %.*s", _get_arr_size( _temp3251, 1u),
_temp3251.curr, _get_arr_size( _temp3252, 1u), _temp3252.curr);}));}}}} static
struct _tagged_arr Cyc_Tcutil_strptr2string( struct _tagged_arr* s){ return* s;}
void Cyc_Tcutil_check_unique_vars( struct Cyc_List_List* vs, struct Cyc_Position_Segment*
loc, struct _tagged_arr msg){(( void(*)( int(* cmp)( struct _tagged_arr*, struct
_tagged_arr*), struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc,
struct _tagged_arr(* a2string)( struct _tagged_arr*), struct _tagged_arr msg))
Cyc_Tcutil_check_unique_unsorted)( Cyc_String_zstrptrcmp, vs, loc, Cyc_Tcutil_strptr2string,
msg);} void Cyc_Tcutil_check_unique_tvars( struct Cyc_Position_Segment* loc,
struct Cyc_List_List* tvs){(( void(*)( int(* cmp)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* vs, struct Cyc_Position_Segment*
loc, struct _tagged_arr(* a2string)( struct Cyc_Absyn_Tvar*), struct _tagged_arr
msg)) Cyc_Tcutil_check_unique_unsorted)( Cyc_Absyn_tvar_cmp, tvs, loc, Cyc_Tcutil_tvar2string,
_tag_arr("duplicate type variable", sizeof( unsigned char), 24u));} struct
_tuple17{ struct Cyc_Absyn_Structfield* f1; int f2; } ; struct _tuple18{ struct
Cyc_List_List* f1; void* f2; } ; struct _tuple19{ struct Cyc_Absyn_Structfield*
f1; void* f2; } ; struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* sdfields){ struct Cyc_List_List* fields= 0;{ struct
Cyc_List_List* sd_fields= sdfields; for( 0; sd_fields != 0; sd_fields=(( struct
Cyc_List_List*) _check_null( sd_fields))->tl){ if( Cyc_String_strcmp(*(( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( sd_fields))->hd)->name,
_tag_arr("", sizeof( unsigned char), 1u)) != 0){ fields=({ struct Cyc_List_List*
_temp3253=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3253->hd=( void*)({ struct _tuple17* _temp3254=( struct _tuple17*)
GC_malloc( sizeof( struct _tuple17)); _temp3254->f1=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( sd_fields))->hd; _temp3254->f2= 0; _temp3254;});
_temp3253->tl= fields; _temp3253;});}}} fields=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields);{ struct Cyc_List_List* ans=
0; for( 0; des != 0; des=(( struct Cyc_List_List*) _check_null( des))->tl){
struct _tuple18 _temp3257; void* _temp3258; struct Cyc_List_List* _temp3260;
struct _tuple18* _temp3255=( struct _tuple18*)(( struct Cyc_List_List*)
_check_null( des))->hd; _temp3257=* _temp3255; _LL3261: _temp3260= _temp3257.f1;
goto _LL3259; _LL3259: _temp3258= _temp3257.f2; goto _LL3256; _LL3256: if(
_temp3260 == 0){ struct Cyc_List_List* _temp3262= fields; for( 0; _temp3262 != 0;
_temp3262=(( struct Cyc_List_List*) _check_null( _temp3262))->tl){ if( !(*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp3262))->hd)).f2){(*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp3262))->hd)).f2= 1;(*((
struct _tuple18*)(( struct Cyc_List_List*) _check_null( des))->hd)).f1=({ struct
Cyc_List_List* _temp3263=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3263->hd=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp3264=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp3264[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp3265; _temp3265.tag= Cyc_Absyn_FieldName;
_temp3265.f1=((*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null(
_temp3262))->hd)).f1)->name; _temp3265;}); _temp3264;})); _temp3263->tl= 0;
_temp3263;}); ans=({ struct Cyc_List_List* _temp3266=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp3266->hd=( void*)({
struct _tuple19* _temp3267=( struct _tuple19*) _region_malloc( rgn, sizeof(
struct _tuple19)); _temp3267->f1=(*(( struct _tuple17*)(( struct Cyc_List_List*)
_check_null( _temp3262))->hd)).f1; _temp3267->f2= _temp3258; _temp3267;});
_temp3266->tl= ans; _temp3266;}); break;}} if( _temp3262 == 0){ Cyc_Tcutil_terr(
loc, _tag_arr("too many arguments to struct", sizeof( unsigned char), 29u));}}
else{ if((( struct Cyc_List_List*) _check_null( _temp3260))->tl != 0){ Cyc_Tcutil_terr(
loc, _tag_arr("multiple designators are not supported", sizeof( unsigned char),
39u));} else{ void* _temp3268=( void*)(( struct Cyc_List_List*) _check_null(
_temp3260))->hd; struct Cyc_Absyn_Exp* _temp3274; struct _tagged_arr* _temp3276;
_LL3270: if(*(( int*) _temp3268) == Cyc_Absyn_ArrayElement){ _LL3275: _temp3274=((
struct Cyc_Absyn_ArrayElement_struct*) _temp3268)->f1; goto _LL3271;} else{ goto
_LL3272;} _LL3272: if(*(( int*) _temp3268) == Cyc_Absyn_FieldName){ _LL3277:
_temp3276=(( struct Cyc_Absyn_FieldName_struct*) _temp3268)->f1; goto _LL3273;}
else{ goto _LL3269;} _LL3271: Cyc_Tcutil_terr( loc, _tag_arr("array designator used in argument to struct",
sizeof( unsigned char), 44u)); goto _LL3269; _LL3273: { struct Cyc_List_List*
_temp3278= fields; for( 0; _temp3278 != 0; _temp3278=(( struct Cyc_List_List*)
_check_null( _temp3278))->tl){ if( Cyc_String_zstrptrcmp( _temp3276,((*(( struct
_tuple17*)(( struct Cyc_List_List*) _check_null( _temp3278))->hd)).f1)->name) ==
0){ if((*(( struct _tuple17*)(( struct Cyc_List_List*) _check_null( _temp3278))->hd)).f2){
Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr _temp3279=*
_temp3276; xprintf("field %.*s has already been used as an argument",
_get_arr_size( _temp3279, 1u), _temp3279.curr);}));}(*(( struct _tuple17*)((
struct Cyc_List_List*) _check_null( _temp3278))->hd)).f2= 1; ans=({ struct Cyc_List_List*
_temp3280=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp3280->hd=( void*)({ struct _tuple19* _temp3281=( struct _tuple19*)
_region_malloc( rgn, sizeof( struct _tuple19)); _temp3281->f1=(*(( struct
_tuple17*)(( struct Cyc_List_List*) _check_null( _temp3278))->hd)).f1; _temp3281->f2=
_temp3258; _temp3281;}); _temp3280->tl= ans; _temp3280;}); break;}} if(
_temp3278 == 0){ Cyc_Tcutil_terr( loc,( struct _tagged_arr)({ struct _tagged_arr
_temp3282=* _temp3276; xprintf("bad field designator %.*s", _get_arr_size(
_temp3282, 1u), _temp3282.curr);}));} goto _LL3269;} _LL3269:;}}} for( 0; fields
!= 0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){ if( !(*((
struct _tuple17*)(( struct Cyc_List_List*) _check_null( fields))->hd)).f2){ Cyc_Tcutil_terr(
loc, _tag_arr("too few arguments to struct", sizeof( unsigned char), 28u));
break;}} return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
ans);}} int Cyc_Tcutil_is_tagged_pointer_typ_elt( void* t, void** elt_typ_dest){
void* _temp3283= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp3289;
struct Cyc_Absyn_Conref* _temp3291; struct Cyc_Absyn_Tqual _temp3293; struct Cyc_Absyn_Conref*
_temp3295; void* _temp3297; void* _temp3299; _LL3285: if(( unsigned int)
_temp3283 > 4u?*(( int*) _temp3283) == Cyc_Absyn_PointerType: 0){ _LL3290:
_temp3289=(( struct Cyc_Absyn_PointerType_struct*) _temp3283)->f1; _LL3300:
_temp3299=( void*) _temp3289.elt_typ; goto _LL3298; _LL3298: _temp3297=( void*)
_temp3289.rgn_typ; goto _LL3296; _LL3296: _temp3295= _temp3289.nullable; goto
_LL3294; _LL3294: _temp3293= _temp3289.tq; goto _LL3292; _LL3292: _temp3291=
_temp3289.bounds; goto _LL3286;} else{ goto _LL3287;} _LL3287: goto _LL3288;
_LL3286: { struct Cyc_Absyn_Conref* _temp3301=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3291); void*
_temp3302=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp3301))->v; void* _temp3310; _LL3304: if((
unsigned int) _temp3302 > 1u?*(( int*) _temp3302) == Cyc_Absyn_Eq_constr: 0){
_LL3311: _temp3310=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3302)->f1;
if( _temp3310 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3305;} else{ goto _LL3306;}}
else{ goto _LL3306;} _LL3306: if( _temp3302 ==( void*) Cyc_Absyn_No_constr){
goto _LL3307;} else{ goto _LL3308;} _LL3308: goto _LL3309; _LL3305:*
elt_typ_dest= _temp3299; return 1; _LL3307:( void*)( _temp3301->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3312=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3312[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3313; _temp3313.tag= Cyc_Absyn_Eq_constr;
_temp3313.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp3313;}); _temp3312;})));*
elt_typ_dest= _temp3299; return 1; _LL3309: return 0; _LL3303:;} _LL3288: return
0; _LL3284:;} int Cyc_Tcutil_is_tagged_pointer_typ( void* t){ void* ignore=(
void*) Cyc_Absyn_VoidType; return Cyc_Tcutil_is_tagged_pointer_typ_elt( t,&
ignore);} struct _tuple7 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){ struct _tuple7 bogus_ans=({ struct _tuple7 _temp3543;
_temp3543.f1= 0; _temp3543.f2=( void*) Cyc_Absyn_HeapRgn; _temp3543;}); void*
_temp3314=( void*) e->r; void* _temp3328; struct _tuple1* _temp3330; struct
_tagged_arr* _temp3332; struct Cyc_Absyn_Exp* _temp3334; struct _tagged_arr*
_temp3336; struct Cyc_Absyn_Exp* _temp3338; struct Cyc_Absyn_Exp* _temp3340;
struct Cyc_Absyn_Exp* _temp3342; struct Cyc_Absyn_Exp* _temp3344; _LL3316: if(*((
int*) _temp3314) == Cyc_Absyn_Var_e){ _LL3331: _temp3330=(( struct Cyc_Absyn_Var_e_struct*)
_temp3314)->f1; goto _LL3329; _LL3329: _temp3328=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3314)->f2; goto _LL3317;} else{ goto _LL3318;} _LL3318: if(*(( int*)
_temp3314) == Cyc_Absyn_StructMember_e){ _LL3335: _temp3334=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3314)->f1; goto _LL3333; _LL3333: _temp3332=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3314)->f2; goto _LL3319;} else{ goto _LL3320;} _LL3320: if(*(( int*)
_temp3314) == Cyc_Absyn_StructArrow_e){ _LL3339: _temp3338=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3314)->f1; goto _LL3337; _LL3337: _temp3336=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3314)->f2; goto _LL3321;} else{ goto _LL3322;} _LL3322: if(*(( int*)
_temp3314) == Cyc_Absyn_Deref_e){ _LL3341: _temp3340=(( struct Cyc_Absyn_Deref_e_struct*)
_temp3314)->f1; goto _LL3323;} else{ goto _LL3324;} _LL3324: if(*(( int*)
_temp3314) == Cyc_Absyn_Subscript_e){ _LL3345: _temp3344=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3314)->f1; goto _LL3343; _LL3343: _temp3342=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3314)->f2; goto _LL3325;} else{ goto _LL3326;} _LL3326: goto _LL3327;
_LL3317: { void* _temp3346= _temp3328; struct Cyc_Absyn_Fndecl* _temp3360;
struct Cyc_Absyn_Vardecl* _temp3362; struct Cyc_Absyn_Vardecl* _temp3364; struct
Cyc_Absyn_Vardecl* _temp3366; struct Cyc_Absyn_Vardecl* _temp3368; _LL3348: if(
_temp3346 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL3349;} else{ goto _LL3350;}
_LL3350: if(( unsigned int) _temp3346 > 1u?*(( int*) _temp3346) == Cyc_Absyn_Funname_b:
0){ _LL3361: _temp3360=(( struct Cyc_Absyn_Funname_b_struct*) _temp3346)->f1;
goto _LL3351;} else{ goto _LL3352;} _LL3352: if(( unsigned int) _temp3346 > 1u?*((
int*) _temp3346) == Cyc_Absyn_Global_b: 0){ _LL3363: _temp3362=(( struct Cyc_Absyn_Global_b_struct*)
_temp3346)->f1; goto _LL3353;} else{ goto _LL3354;} _LL3354: if(( unsigned int)
_temp3346 > 1u?*(( int*) _temp3346) == Cyc_Absyn_Local_b: 0){ _LL3365: _temp3364=((
struct Cyc_Absyn_Local_b_struct*) _temp3346)->f1; goto _LL3355;} else{ goto
_LL3356;} _LL3356: if(( unsigned int) _temp3346 > 1u?*(( int*) _temp3346) == Cyc_Absyn_Pat_b:
0){ _LL3367: _temp3366=(( struct Cyc_Absyn_Pat_b_struct*) _temp3346)->f1; goto
_LL3357;} else{ goto _LL3358;} _LL3358: if(( unsigned int) _temp3346 > 1u?*((
int*) _temp3346) == Cyc_Absyn_Param_b: 0){ _LL3369: _temp3368=(( struct Cyc_Absyn_Param_b_struct*)
_temp3346)->f1; goto _LL3359;} else{ goto _LL3347;} _LL3349: return bogus_ans;
_LL3351: return({ struct _tuple7 _temp3370; _temp3370.f1= 1; _temp3370.f2=( void*)
Cyc_Absyn_HeapRgn; _temp3370;}); _LL3353: { void* _temp3371= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); struct Cyc_Absyn_Exp*
_temp3377; struct Cyc_Absyn_Tqual _temp3379; void* _temp3381; _LL3373: if((
unsigned int) _temp3371 > 4u?*(( int*) _temp3371) == Cyc_Absyn_ArrayType: 0){
_LL3382: _temp3381=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3371)->f1;
goto _LL3380; _LL3380: _temp3379=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3371)->f2; goto _LL3378; _LL3378: _temp3377=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3371)->f3; goto _LL3374;} else{ goto _LL3375;} _LL3375: goto _LL3376;
_LL3374: return({ struct _tuple7 _temp3383; _temp3383.f1= 1; _temp3383.f2=( void*)
Cyc_Absyn_HeapRgn; _temp3383;}); _LL3376: return({ struct _tuple7 _temp3384;
_temp3384.f1=( _temp3362->tq).q_const; _temp3384.f2=( void*) Cyc_Absyn_HeapRgn;
_temp3384;}); _LL3372:;} _LL3355: { void* _temp3385= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v); struct Cyc_Absyn_Exp*
_temp3391; struct Cyc_Absyn_Tqual _temp3393; void* _temp3395; _LL3387: if((
unsigned int) _temp3385 > 4u?*(( int*) _temp3385) == Cyc_Absyn_ArrayType: 0){
_LL3396: _temp3395=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3385)->f1;
goto _LL3394; _LL3394: _temp3393=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3385)->f2; goto _LL3392; _LL3392: _temp3391=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3385)->f3; goto _LL3388;} else{ goto _LL3389;} _LL3389: goto _LL3390;
_LL3388: return({ struct _tuple7 _temp3397; _temp3397.f1= 1; _temp3397.f2=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp3364->rgn))->v; _temp3397;}); _LL3390:
return({ struct _tuple7 _temp3398; _temp3398.f1=( _temp3364->tq).q_const;
_temp3398.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3364->rgn))->v;
_temp3398;}); _LL3386:;} _LL3357: _temp3368= _temp3366; goto _LL3359; _LL3359:
return({ struct _tuple7 _temp3399; _temp3399.f1=( _temp3368->tq).q_const;
_temp3399.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3368->rgn))->v;
_temp3399;}); _LL3347:;} _LL3319: { void* _temp3400= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp3334->topt))->v); struct Cyc_List_List*
_temp3412; struct Cyc_List_List* _temp3414; struct Cyc_Absyn_Structdecl**
_temp3416; struct Cyc_Absyn_Structdecl* _temp3418; struct Cyc_List_List*
_temp3419; struct _tuple1* _temp3421; struct Cyc_Absyn_Uniondecl** _temp3423;
struct Cyc_Absyn_Uniondecl* _temp3425; struct Cyc_List_List* _temp3426; struct
_tuple1* _temp3428; _LL3402: if(( unsigned int) _temp3400 > 4u?*(( int*)
_temp3400) == Cyc_Absyn_AnonStructType: 0){ _LL3413: _temp3412=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3400)->f1; goto _LL3403;} else{ goto _LL3404;} _LL3404: if(( unsigned int)
_temp3400 > 4u?*(( int*) _temp3400) == Cyc_Absyn_AnonUnionType: 0){ _LL3415:
_temp3414=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3400)->f1; goto
_LL3405;} else{ goto _LL3406;} _LL3406: if(( unsigned int) _temp3400 > 4u?*((
int*) _temp3400) == Cyc_Absyn_StructType: 0){ _LL3422: _temp3421=(( struct Cyc_Absyn_StructType_struct*)
_temp3400)->f1; goto _LL3420; _LL3420: _temp3419=(( struct Cyc_Absyn_StructType_struct*)
_temp3400)->f2; goto _LL3417; _LL3417: _temp3416=(( struct Cyc_Absyn_StructType_struct*)
_temp3400)->f3; if( _temp3416 == 0){ goto _LL3408;} else{ _temp3418=* _temp3416;
goto _LL3407;}} else{ goto _LL3408;} _LL3408: if(( unsigned int) _temp3400 > 4u?*((
int*) _temp3400) == Cyc_Absyn_UnionType: 0){ _LL3429: _temp3428=(( struct Cyc_Absyn_UnionType_struct*)
_temp3400)->f1; goto _LL3427; _LL3427: _temp3426=(( struct Cyc_Absyn_UnionType_struct*)
_temp3400)->f2; goto _LL3424; _LL3424: _temp3423=(( struct Cyc_Absyn_UnionType_struct*)
_temp3400)->f3; if( _temp3423 == 0){ goto _LL3410;} else{ _temp3425=* _temp3423;
goto _LL3409;}} else{ goto _LL3410;} _LL3410: goto _LL3411; _LL3403: _temp3414=
_temp3412; goto _LL3405; _LL3405: { struct Cyc_Absyn_Structfield* _temp3430= Cyc_Absyn_lookup_field(
_temp3414, _temp3332); if( _temp3430 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3430))->width != 0: 0){ return({ struct _tuple7 _temp3431;
_temp3431.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3430))->tq).q_const;
_temp3431.f2=( Cyc_Tcutil_addressof_props( te, _temp3334)).f2; _temp3431;});}
return bogus_ans;} _LL3407: { struct Cyc_Absyn_Structfield* _temp3432= Cyc_Absyn_lookup_struct_field(
_temp3418, _temp3332); if( _temp3432 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3432))->width != 0: 0){ return({ struct _tuple7 _temp3433;
_temp3433.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3432))->tq).q_const;
_temp3433.f2=( Cyc_Tcutil_addressof_props( te, _temp3334)).f2; _temp3433;});}
return bogus_ans;} _LL3409: { struct Cyc_Absyn_Structfield* _temp3434= Cyc_Absyn_lookup_union_field(
_temp3425, _temp3332); if( _temp3434 != 0){ return({ struct _tuple7 _temp3435;
_temp3435.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3434))->tq).q_const;
_temp3435.f2=( Cyc_Tcutil_addressof_props( te, _temp3334)).f2; _temp3435;});}
goto _LL3411;} _LL3411: return bogus_ans; _LL3401:;} _LL3321: { void* _temp3436=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3338->topt))->v);
struct Cyc_Absyn_PtrInfo _temp3442; struct Cyc_Absyn_Conref* _temp3444; struct
Cyc_Absyn_Tqual _temp3446; struct Cyc_Absyn_Conref* _temp3448; void* _temp3450;
void* _temp3452; _LL3438: if(( unsigned int) _temp3436 > 4u?*(( int*) _temp3436)
== Cyc_Absyn_PointerType: 0){ _LL3443: _temp3442=(( struct Cyc_Absyn_PointerType_struct*)
_temp3436)->f1; _LL3453: _temp3452=( void*) _temp3442.elt_typ; goto _LL3451;
_LL3451: _temp3450=( void*) _temp3442.rgn_typ; goto _LL3449; _LL3449: _temp3448=
_temp3442.nullable; goto _LL3447; _LL3447: _temp3446= _temp3442.tq; goto _LL3445;
_LL3445: _temp3444= _temp3442.bounds; goto _LL3439;} else{ goto _LL3440;}
_LL3440: goto _LL3441; _LL3439: { void* _temp3454= Cyc_Tcutil_compress(
_temp3452); struct Cyc_List_List* _temp3466; struct Cyc_List_List* _temp3468;
struct Cyc_Absyn_Structdecl** _temp3470; struct Cyc_Absyn_Structdecl* _temp3472;
struct Cyc_List_List* _temp3473; struct _tuple1* _temp3475; struct Cyc_Absyn_Uniondecl**
_temp3477; struct Cyc_Absyn_Uniondecl* _temp3479; struct Cyc_List_List*
_temp3480; struct _tuple1* _temp3482; _LL3456: if(( unsigned int) _temp3454 > 4u?*((
int*) _temp3454) == Cyc_Absyn_AnonStructType: 0){ _LL3467: _temp3466=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp3454)->f1; goto _LL3457;} else{ goto
_LL3458;} _LL3458: if(( unsigned int) _temp3454 > 4u?*(( int*) _temp3454) == Cyc_Absyn_AnonUnionType:
0){ _LL3469: _temp3468=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3454)->f1;
goto _LL3459;} else{ goto _LL3460;} _LL3460: if(( unsigned int) _temp3454 > 4u?*((
int*) _temp3454) == Cyc_Absyn_StructType: 0){ _LL3476: _temp3475=(( struct Cyc_Absyn_StructType_struct*)
_temp3454)->f1; goto _LL3474; _LL3474: _temp3473=(( struct Cyc_Absyn_StructType_struct*)
_temp3454)->f2; goto _LL3471; _LL3471: _temp3470=(( struct Cyc_Absyn_StructType_struct*)
_temp3454)->f3; if( _temp3470 == 0){ goto _LL3462;} else{ _temp3472=* _temp3470;
goto _LL3461;}} else{ goto _LL3462;} _LL3462: if(( unsigned int) _temp3454 > 4u?*((
int*) _temp3454) == Cyc_Absyn_UnionType: 0){ _LL3483: _temp3482=(( struct Cyc_Absyn_UnionType_struct*)
_temp3454)->f1; goto _LL3481; _LL3481: _temp3480=(( struct Cyc_Absyn_UnionType_struct*)
_temp3454)->f2; goto _LL3478; _LL3478: _temp3477=(( struct Cyc_Absyn_UnionType_struct*)
_temp3454)->f3; if( _temp3477 == 0){ goto _LL3464;} else{ _temp3479=* _temp3477;
goto _LL3463;}} else{ goto _LL3464;} _LL3464: goto _LL3465; _LL3457: _temp3468=
_temp3466; goto _LL3459; _LL3459: { struct Cyc_Absyn_Structfield* _temp3484= Cyc_Absyn_lookup_field(
_temp3468, _temp3336); if( _temp3484 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3484))->width != 0: 0){ return({ struct _tuple7 _temp3485;
_temp3485.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3484))->tq).q_const;
_temp3485.f2= _temp3450; _temp3485;});} return bogus_ans;} _LL3461: { struct Cyc_Absyn_Structfield*
_temp3486= Cyc_Absyn_lookup_struct_field( _temp3472, _temp3336); if( _temp3486
!= 0?(( struct Cyc_Absyn_Structfield*) _check_null( _temp3486))->width != 0: 0){
return({ struct _tuple7 _temp3487; _temp3487.f1=((( struct Cyc_Absyn_Structfield*)
_check_null( _temp3486))->tq).q_const; _temp3487.f2= _temp3450; _temp3487;});}
return bogus_ans;} _LL3463: { struct Cyc_Absyn_Structfield* _temp3488= Cyc_Absyn_lookup_union_field(
_temp3479, _temp3336); if( _temp3488 != 0){ return({ struct _tuple7 _temp3489;
_temp3489.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3488))->tq).q_const;
_temp3489.f2= _temp3450; _temp3489;});} return bogus_ans;} _LL3465: return
bogus_ans; _LL3455:;} _LL3441: return bogus_ans; _LL3437:;} _LL3323: { void*
_temp3490= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp3340->topt))->v); struct Cyc_Absyn_PtrInfo _temp3496; struct Cyc_Absyn_Conref*
_temp3498; struct Cyc_Absyn_Tqual _temp3500; struct Cyc_Absyn_Conref* _temp3502;
void* _temp3504; void* _temp3506; _LL3492: if(( unsigned int) _temp3490 > 4u?*((
int*) _temp3490) == Cyc_Absyn_PointerType: 0){ _LL3497: _temp3496=(( struct Cyc_Absyn_PointerType_struct*)
_temp3490)->f1; _LL3507: _temp3506=( void*) _temp3496.elt_typ; goto _LL3505;
_LL3505: _temp3504=( void*) _temp3496.rgn_typ; goto _LL3503; _LL3503: _temp3502=
_temp3496.nullable; goto _LL3501; _LL3501: _temp3500= _temp3496.tq; goto _LL3499;
_LL3499: _temp3498= _temp3496.bounds; goto _LL3493;} else{ goto _LL3494;}
_LL3494: goto _LL3495; _LL3493: return({ struct _tuple7 _temp3508; _temp3508.f1=
_temp3500.q_const; _temp3508.f2= _temp3504; _temp3508;}); _LL3495: return
bogus_ans; _LL3491:;} _LL3325: { void* t= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp3344->topt))->v); void* _temp3509= t; struct
Cyc_Absyn_Exp* _temp3519; struct Cyc_Absyn_Tqual _temp3521; void* _temp3523;
struct Cyc_List_List* _temp3525; struct Cyc_Absyn_PtrInfo _temp3527; struct Cyc_Absyn_Conref*
_temp3529; struct Cyc_Absyn_Tqual _temp3531; struct Cyc_Absyn_Conref* _temp3533;
void* _temp3535; void* _temp3537; _LL3511: if(( unsigned int) _temp3509 > 4u?*((
int*) _temp3509) == Cyc_Absyn_ArrayType: 0){ _LL3524: _temp3523=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3509)->f1; goto _LL3522; _LL3522:
_temp3521=(( struct Cyc_Absyn_ArrayType_struct*) _temp3509)->f2; goto _LL3520;
_LL3520: _temp3519=(( struct Cyc_Absyn_ArrayType_struct*) _temp3509)->f3; goto
_LL3512;} else{ goto _LL3513;} _LL3513: if(( unsigned int) _temp3509 > 4u?*((
int*) _temp3509) == Cyc_Absyn_TupleType: 0){ _LL3526: _temp3525=(( struct Cyc_Absyn_TupleType_struct*)
_temp3509)->f1; goto _LL3514;} else{ goto _LL3515;} _LL3515: if(( unsigned int)
_temp3509 > 4u?*(( int*) _temp3509) == Cyc_Absyn_PointerType: 0){ _LL3528:
_temp3527=(( struct Cyc_Absyn_PointerType_struct*) _temp3509)->f1; _LL3538:
_temp3537=( void*) _temp3527.elt_typ; goto _LL3536; _LL3536: _temp3535=( void*)
_temp3527.rgn_typ; goto _LL3534; _LL3534: _temp3533= _temp3527.nullable; goto
_LL3532; _LL3532: _temp3531= _temp3527.tq; goto _LL3530; _LL3530: _temp3529=
_temp3527.bounds; goto _LL3516;} else{ goto _LL3517;} _LL3517: goto _LL3518;
_LL3512: return({ struct _tuple7 _temp3539; _temp3539.f1= _temp3521.q_const;
_temp3539.f2=( Cyc_Tcutil_addressof_props( te, _temp3344)).f2; _temp3539;});
_LL3514: { struct _tuple4* _temp3540= Cyc_Absyn_lookup_tuple_field( _temp3525,(
int) Cyc_Evexp_eval_const_uint_exp( _temp3342)); if( _temp3540 != 0){ return({
struct _tuple7 _temp3541; _temp3541.f1=((*(( struct _tuple4*) _check_null(
_temp3540))).f1).q_const; _temp3541.f2=( Cyc_Tcutil_addressof_props( te,
_temp3344)).f2; _temp3541;});} return bogus_ans;} _LL3516: return({ struct
_tuple7 _temp3542; _temp3542.f1= _temp3531.q_const; _temp3542.f2= _temp3535;
_temp3542;}); _LL3518: return bogus_ans; _LL3510:;} _LL3327: Cyc_Tcutil_terr( e->loc,
_tag_arr("unary & applied to non-lvalue", sizeof( unsigned char), 30u)); return
bogus_ans; _LL3315:;} void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv* te,
void* e_typ, struct Cyc_Absyn_Exp* e){ void* _temp3544= Cyc_Tcutil_compress(
e_typ); struct Cyc_Absyn_Exp* _temp3550; struct Cyc_Absyn_Tqual _temp3552; void*
_temp3554; _LL3546: if(( unsigned int) _temp3544 > 4u?*(( int*) _temp3544) ==
Cyc_Absyn_ArrayType: 0){ _LL3555: _temp3554=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3544)->f1; goto _LL3553; _LL3553: _temp3552=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3544)->f2; goto _LL3551; _LL3551: _temp3550=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3544)->f3; goto _LL3547;} else{ goto _LL3548;} _LL3548: goto _LL3549;
_LL3547: { void* _temp3558; int _temp3560; struct _tuple7 _temp3556= Cyc_Tcutil_addressof_props(
te, e); _LL3561: _temp3560= _temp3556.f1; goto _LL3559; _LL3559: _temp3558=
_temp3556.f2; goto _LL3557; _LL3557: return Cyc_Absyn_atb_typ( _temp3554,
_temp3558, _temp3552,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3562=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3562[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3563; _temp3563.tag= Cyc_Absyn_Upper_b;
_temp3563.f1= e; _temp3563;}); _temp3562;}));} _LL3549: return e_typ; _LL3545:;}
void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment* loc, unsigned int i,
struct Cyc_Absyn_Conref* b){ b=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( b);{ void* _temp3564=( void*) b->v; void*
_temp3572; void* _temp3574; struct Cyc_Absyn_Exp* _temp3576; _LL3566: if((
unsigned int) _temp3564 > 1u?*(( int*) _temp3564) == Cyc_Absyn_Eq_constr: 0){
_LL3573: _temp3572=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3564)->f1;
if( _temp3572 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3567;} else{ goto _LL3568;}}
else{ goto _LL3568;} _LL3568: if(( unsigned int) _temp3564 > 1u?*(( int*)
_temp3564) == Cyc_Absyn_Eq_constr: 0){ _LL3575: _temp3574=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3564)->f1; if(( unsigned int) _temp3574 > 1u?*(( int*) _temp3574) == Cyc_Absyn_Upper_b:
0){ _LL3577: _temp3576=(( struct Cyc_Absyn_Upper_b_struct*) _temp3574)->f1; goto
_LL3569;} else{ goto _LL3570;}} else{ goto _LL3570;} _LL3570: goto _LL3571;
_LL3567: return; _LL3569: if( Cyc_Evexp_eval_const_uint_exp( _temp3576) <= i){
Cyc_Tcutil_terr( loc, _tag_arr("dereference is out of bounds", sizeof(
unsigned char), 29u));} return; _LL3571:( void*)( b->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_constr_struct* _temp3578=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3578[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3579; _temp3579.tag= Cyc_Absyn_Eq_constr;
_temp3579.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3580=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3580[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3581; _temp3581.tag= Cyc_Absyn_Upper_b;
_temp3581.f1= Cyc_Absyn_uint_exp( i + 1, 0); _temp3581;}); _temp3580;}));
_temp3579;}); _temp3578;}))); return; _LL3565:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_bits_only( void* t){ void* _temp3582= Cyc_Tcutil_compress(
t); void* _temp3608; void* _temp3610; struct Cyc_Absyn_Enumdecl* _temp3612;
struct _tuple1* _temp3614; struct Cyc_Absyn_Exp* _temp3616; struct Cyc_Absyn_Tqual
_temp3618; void* _temp3620; struct Cyc_List_List* _temp3622; struct Cyc_Absyn_Structdecl**
_temp3624; struct Cyc_List_List* _temp3626; struct _tuple1* _temp3628; struct
Cyc_Absyn_Uniondecl** _temp3630; struct Cyc_List_List* _temp3632; struct _tuple1*
_temp3634; struct Cyc_List_List* _temp3636; struct Cyc_List_List* _temp3638;
_LL3584: if( _temp3582 ==( void*) Cyc_Absyn_VoidType){ goto _LL3585;} else{ goto
_LL3586;} _LL3586: if(( unsigned int) _temp3582 > 4u?*(( int*) _temp3582) == Cyc_Absyn_IntType:
0){ _LL3611: _temp3610=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3582)->f1;
goto _LL3609; _LL3609: _temp3608=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3582)->f2; goto _LL3587;} else{ goto _LL3588;} _LL3588: if( _temp3582 ==(
void*) Cyc_Absyn_FloatType){ goto _LL3589;} else{ goto _LL3590;} _LL3590: if(
_temp3582 ==( void*) Cyc_Absyn_DoubleType){ goto _LL3591;} else{ goto _LL3592;}
_LL3592: if(( unsigned int) _temp3582 > 4u?*(( int*) _temp3582) == Cyc_Absyn_EnumType:
0){ _LL3615: _temp3614=(( struct Cyc_Absyn_EnumType_struct*) _temp3582)->f1;
goto _LL3613; _LL3613: _temp3612=(( struct Cyc_Absyn_EnumType_struct*) _temp3582)->f2;
goto _LL3593;} else{ goto _LL3594;} _LL3594: if(( unsigned int) _temp3582 > 4u?*((
int*) _temp3582) == Cyc_Absyn_ArrayType: 0){ _LL3621: _temp3620=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3582)->f1; goto _LL3619; _LL3619:
_temp3618=(( struct Cyc_Absyn_ArrayType_struct*) _temp3582)->f2; goto _LL3617;
_LL3617: _temp3616=(( struct Cyc_Absyn_ArrayType_struct*) _temp3582)->f3; goto
_LL3595;} else{ goto _LL3596;} _LL3596: if(( unsigned int) _temp3582 > 4u?*((
int*) _temp3582) == Cyc_Absyn_TupleType: 0){ _LL3623: _temp3622=(( struct Cyc_Absyn_TupleType_struct*)
_temp3582)->f1; goto _LL3597;} else{ goto _LL3598;} _LL3598: if(( unsigned int)
_temp3582 > 4u?*(( int*) _temp3582) == Cyc_Absyn_StructType: 0){ _LL3629:
_temp3628=(( struct Cyc_Absyn_StructType_struct*) _temp3582)->f1; goto _LL3627;
_LL3627: _temp3626=(( struct Cyc_Absyn_StructType_struct*) _temp3582)->f2; goto
_LL3625; _LL3625: _temp3624=(( struct Cyc_Absyn_StructType_struct*) _temp3582)->f3;
goto _LL3599;} else{ goto _LL3600;} _LL3600: if(( unsigned int) _temp3582 > 4u?*((
int*) _temp3582) == Cyc_Absyn_UnionType: 0){ _LL3635: _temp3634=(( struct Cyc_Absyn_UnionType_struct*)
_temp3582)->f1; goto _LL3633; _LL3633: _temp3632=(( struct Cyc_Absyn_UnionType_struct*)
_temp3582)->f2; goto _LL3631; _LL3631: _temp3630=(( struct Cyc_Absyn_UnionType_struct*)
_temp3582)->f3; goto _LL3601;} else{ goto _LL3602;} _LL3602: if(( unsigned int)
_temp3582 > 4u?*(( int*) _temp3582) == Cyc_Absyn_AnonStructType: 0){ _LL3637:
_temp3636=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3582)->f1; goto
_LL3603;} else{ goto _LL3604;} _LL3604: if(( unsigned int) _temp3582 > 4u?*((
int*) _temp3582) == Cyc_Absyn_AnonUnionType: 0){ _LL3639: _temp3638=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3582)->f1; goto _LL3605;} else{ goto
_LL3606;} _LL3606: goto _LL3607; _LL3585: return 1; _LL3587: return 1; _LL3589:
return 1; _LL3591: return 1; _LL3593: return 0; _LL3595: return Cyc_Tcutil_bits_only(
_temp3620); _LL3597: for( 0; _temp3622 != 0; _temp3622=(( struct Cyc_List_List*)
_check_null( _temp3622))->tl){ if( ! Cyc_Tcutil_bits_only((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp3622))->hd)).f2)){ return 0;}} return 1;
_LL3599: if( _temp3624 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* _temp3640=*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp3624)); if( _temp3640->fields
== 0){ return 0;}{ struct _RegionHandle _temp3641= _new_region(); struct
_RegionHandle* rgn=& _temp3641; _push_region( rgn);{ struct Cyc_List_List*
_temp3642=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp3640->tvs, _temp3626);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp3640->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute(
rgn, _temp3642,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3643= 0; _npop_handler( 0u); return
_temp3643;}}}{ int _temp3644= 1; _npop_handler( 0u); return _temp3644;}};
_pop_region( rgn);}} _LL3601: if( _temp3630 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
_temp3645=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp3630)); if(
_temp3645->fields == 0){ return 0;}{ struct _RegionHandle _temp3646= _new_region();
struct _RegionHandle* rgn=& _temp3646; _push_region( rgn);{ struct Cyc_List_List*
_temp3647=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp3645->tvs, _temp3632);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp3645->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute(
rgn, _temp3647,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3648= 0; _npop_handler( 0u); return
_temp3648;}}}{ int _temp3649= 1; _npop_handler( 0u); return _temp3649;}};
_pop_region( rgn);}} _LL3603: _temp3638= _temp3636; goto _LL3605; _LL3605: for(
0; _temp3638 != 0; _temp3638=(( struct Cyc_List_List*) _check_null( _temp3638))->tl){
if( ! Cyc_Tcutil_bits_only(( void*)(( struct Cyc_Absyn_Structfield*)(( struct
Cyc_List_List*) _check_null( _temp3638))->hd)->type)){ return 0;}} return 1;
_LL3607: return 0; _LL3583:;} struct _tuple20{ struct Cyc_List_List* f1; struct
Cyc_Absyn_Exp* f2; } ; static int Cyc_Tcutil_cnst_exp( struct Cyc_Tcenv_Tenv* te,
int var_okay, struct Cyc_Absyn_Exp* e){ void* _temp3650=( void*) e->r; void*
_temp3690; void* _temp3692; struct Cyc_Absyn_Exp* _temp3694; struct Cyc_Absyn_Enumfield*
_temp3696; struct Cyc_Absyn_Enumdecl* _temp3698; struct _tuple1* _temp3700; void*
_temp3702; struct _tuple1* _temp3704; struct Cyc_Absyn_Exp* _temp3706; struct
Cyc_Absyn_Exp* _temp3708; struct Cyc_Absyn_Exp* _temp3710; struct Cyc_Absyn_Exp*
_temp3712; struct Cyc_Absyn_Exp* _temp3714; struct Cyc_Absyn_Exp* _temp3716;
struct Cyc_List_List* _temp3718; struct Cyc_Absyn_Exp* _temp3720; struct Cyc_Absyn_Exp*
_temp3722; void* _temp3724; struct Cyc_Absyn_Exp* _temp3726; struct Cyc_Absyn_Exp*
_temp3728; struct Cyc_Absyn_Exp* _temp3730; struct Cyc_Absyn_Vardecl* _temp3732;
struct Cyc_List_List* _temp3734; struct Cyc_List_List* _temp3736; void*
_temp3738; struct Cyc_Absyn_Structdecl* _temp3740; struct Cyc_List_List*
_temp3742; struct Cyc_Core_Opt* _temp3744; struct _tuple1* _temp3746; struct Cyc_List_List*
_temp3748; void* _temp3750; struct Cyc_List_List* _temp3752; struct Cyc_Absyn_Tunionfield*
_temp3754; struct Cyc_Absyn_Tuniondecl* _temp3756; struct Cyc_List_List*
_temp3758; struct Cyc_Core_Opt* _temp3760; struct Cyc_Core_Opt* _temp3762;
_LL3652: if(*(( int*) _temp3650) == Cyc_Absyn_Const_e){ _LL3691: _temp3690=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp3650)->f1; goto _LL3653;} else{
goto _LL3654;} _LL3654: if(*(( int*) _temp3650) == Cyc_Absyn_Sizeoftyp_e){
_LL3693: _temp3692=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp3650)->f1;
goto _LL3655;} else{ goto _LL3656;} _LL3656: if(*(( int*) _temp3650) == Cyc_Absyn_Sizeofexp_e){
_LL3695: _temp3694=(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp3650)->f1; goto
_LL3657;} else{ goto _LL3658;} _LL3658: if(*(( int*) _temp3650) == Cyc_Absyn_Enum_e){
_LL3701: _temp3700=(( struct Cyc_Absyn_Enum_e_struct*) _temp3650)->f1; goto
_LL3699; _LL3699: _temp3698=(( struct Cyc_Absyn_Enum_e_struct*) _temp3650)->f2;
goto _LL3697; _LL3697: _temp3696=(( struct Cyc_Absyn_Enum_e_struct*) _temp3650)->f3;
goto _LL3659;} else{ goto _LL3660;} _LL3660: if(*(( int*) _temp3650) == Cyc_Absyn_Var_e){
_LL3705: _temp3704=(( struct Cyc_Absyn_Var_e_struct*) _temp3650)->f1; goto
_LL3703; _LL3703: _temp3702=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp3650)->f2;
goto _LL3661;} else{ goto _LL3662;} _LL3662: if(*(( int*) _temp3650) == Cyc_Absyn_Conditional_e){
_LL3711: _temp3710=(( struct Cyc_Absyn_Conditional_e_struct*) _temp3650)->f1;
goto _LL3709; _LL3709: _temp3708=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3650)->f2; goto _LL3707; _LL3707: _temp3706=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3650)->f3; goto _LL3663;} else{ goto _LL3664;} _LL3664: if(*(( int*)
_temp3650) == Cyc_Absyn_SeqExp_e){ _LL3715: _temp3714=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3650)->f1; goto _LL3713; _LL3713: _temp3712=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3650)->f2; goto _LL3665;} else{ goto _LL3666;} _LL3666: if(*(( int*)
_temp3650) == Cyc_Absyn_NoInstantiate_e){ _LL3717: _temp3716=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp3650)->f1; goto _LL3667;} else{ goto _LL3668;} _LL3668: if(*(( int*)
_temp3650) == Cyc_Absyn_Instantiate_e){ _LL3721: _temp3720=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3650)->f1; goto _LL3719; _LL3719: _temp3718=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3650)->f2; goto _LL3669;} else{ goto _LL3670;} _LL3670: if(*(( int*)
_temp3650) == Cyc_Absyn_Cast_e){ _LL3725: _temp3724=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp3650)->f1; goto _LL3723; _LL3723: _temp3722=(( struct Cyc_Absyn_Cast_e_struct*)
_temp3650)->f2; goto _LL3671;} else{ goto _LL3672;} _LL3672: if(*(( int*)
_temp3650) == Cyc_Absyn_Address_e){ _LL3727: _temp3726=(( struct Cyc_Absyn_Address_e_struct*)
_temp3650)->f1; goto _LL3673;} else{ goto _LL3674;} _LL3674: if(*(( int*)
_temp3650) == Cyc_Absyn_Comprehension_e){ _LL3733: _temp3732=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3650)->f1; goto _LL3731; _LL3731: _temp3730=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3650)->f2; goto _LL3729; _LL3729: _temp3728=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3650)->f3; goto _LL3675;} else{ goto _LL3676;} _LL3676: if(*(( int*)
_temp3650) == Cyc_Absyn_Array_e){ _LL3735: _temp3734=(( struct Cyc_Absyn_Array_e_struct*)
_temp3650)->f1; goto _LL3677;} else{ goto _LL3678;} _LL3678: if(*(( int*)
_temp3650) == Cyc_Absyn_AnonStruct_e){ _LL3739: _temp3738=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3650)->f1; goto _LL3737; _LL3737: _temp3736=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3650)->f2; goto _LL3679;} else{ goto _LL3680;} _LL3680: if(*(( int*)
_temp3650) == Cyc_Absyn_Struct_e){ _LL3747: _temp3746=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3650)->f1; goto _LL3745; _LL3745: _temp3744=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3650)->f2; goto _LL3743; _LL3743: _temp3742=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3650)->f3; goto _LL3741; _LL3741: _temp3740=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3650)->f4; goto _LL3681;} else{ goto _LL3682;} _LL3682: if(*(( int*)
_temp3650) == Cyc_Absyn_Primop_e){ _LL3751: _temp3750=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp3650)->f1; goto _LL3749; _LL3749: _temp3748=(( struct Cyc_Absyn_Primop_e_struct*)
_temp3650)->f2; goto _LL3683;} else{ goto _LL3684;} _LL3684: if(*(( int*)
_temp3650) == Cyc_Absyn_Tuple_e){ _LL3753: _temp3752=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp3650)->f1; goto _LL3685;} else{ goto _LL3686;} _LL3686: if(*(( int*)
_temp3650) == Cyc_Absyn_Tunion_e){ _LL3763: _temp3762=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3650)->f1; goto _LL3761; _LL3761: _temp3760=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3650)->f2; goto _LL3759; _LL3759: _temp3758=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3650)->f3; goto _LL3757; _LL3757: _temp3756=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3650)->f4; goto _LL3755; _LL3755: _temp3754=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3650)->f5; goto _LL3687;} else{ goto _LL3688;} _LL3688: goto _LL3689;
_LL3653: return 1; _LL3655: return 1; _LL3657: return 1; _LL3659: return 1;
_LL3661: { struct _handler_cons _temp3764; _push_handler(& _temp3764);{ int
_temp3766= 0; if( setjmp( _temp3764.handler)){ _temp3766= 1;} if( ! _temp3766){{
void* _temp3767= Cyc_Tcenv_lookup_ordinary( te, e->loc, _temp3704); void*
_temp3775; struct Cyc_Absyn_Fndecl* _temp3777; void* _temp3779; struct Cyc_Absyn_Vardecl*
_temp3781; _LL3769: if(*(( int*) _temp3767) == Cyc_Tcenv_VarRes){ _LL3776:
_temp3775=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp3767)->f1; if((
unsigned int) _temp3775 > 1u?*(( int*) _temp3775) == Cyc_Absyn_Funname_b: 0){
_LL3778: _temp3777=(( struct Cyc_Absyn_Funname_b_struct*) _temp3775)->f1; goto
_LL3770;} else{ goto _LL3771;}} else{ goto _LL3771;} _LL3771: if(*(( int*)
_temp3767) == Cyc_Tcenv_VarRes){ _LL3780: _temp3779=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3767)->f1; if(( unsigned int) _temp3779 > 1u?*(( int*) _temp3779) == Cyc_Absyn_Global_b:
0){ _LL3782: _temp3781=(( struct Cyc_Absyn_Global_b_struct*) _temp3779)->f1;
goto _LL3772;} else{ goto _LL3773;}} else{ goto _LL3773;} _LL3773: goto _LL3774;
_LL3770: { int _temp3783= 1; _npop_handler( 0u); return _temp3783;} _LL3772: {
void* _temp3784= Cyc_Tcutil_compress(( void*) _temp3781->type); struct Cyc_Absyn_Exp*
_temp3790; struct Cyc_Absyn_Tqual _temp3792; void* _temp3794; _LL3786: if((
unsigned int) _temp3784 > 4u?*(( int*) _temp3784) == Cyc_Absyn_ArrayType: 0){
_LL3795: _temp3794=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3784)->f1;
goto _LL3793; _LL3793: _temp3792=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3784)->f2; goto _LL3791; _LL3791: _temp3790=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3784)->f3; goto _LL3787;} else{ goto _LL3788;} _LL3788: goto _LL3789;
_LL3787: { int _temp3796= 1; _npop_handler( 0u); return _temp3796;} _LL3789: {
int _temp3797= var_okay; _npop_handler( 0u); return _temp3797;} _LL3785:;}
_LL3774: { int _temp3798= var_okay; _npop_handler( 0u); return _temp3798;}
_LL3768:;}; _pop_handler();} else{ void* _temp3765=( void*) _exn_thrown; void*
_temp3800= _temp3765; _LL3802: if( _temp3800 == Cyc_Dict_Absent){ goto _LL3803;}
else{ goto _LL3804;} _LL3804: goto _LL3805; _LL3803: return 0; _LL3805:( void)
_throw( _temp3800); _LL3801:;}}} _LL3663: return( Cyc_Tcutil_cnst_exp( te, 0,
_temp3710)? Cyc_Tcutil_cnst_exp( te, 0, _temp3708): 0)? Cyc_Tcutil_cnst_exp( te,
0, _temp3706): 0; _LL3665: return Cyc_Tcutil_cnst_exp( te, 0, _temp3714)? Cyc_Tcutil_cnst_exp(
te, 0, _temp3712): 0; _LL3667: _temp3720= _temp3716; goto _LL3669; _LL3669:
return Cyc_Tcutil_cnst_exp( te, var_okay, _temp3720); _LL3671: return Cyc_Tcutil_cnst_exp(
te, var_okay, _temp3722); _LL3673: return Cyc_Tcutil_cnst_exp( te, 1, _temp3726);
_LL3675: return Cyc_Tcutil_cnst_exp( te, 0, _temp3730)? Cyc_Tcutil_cnst_exp( te,
0, _temp3728): 0; _LL3677: _temp3736= _temp3734; goto _LL3679; _LL3679:
_temp3742= _temp3736; goto _LL3681; _LL3681: for( 0; _temp3742 != 0; _temp3742=((
struct Cyc_List_List*) _check_null( _temp3742))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,(*(( struct _tuple20*)(( struct Cyc_List_List*) _check_null( _temp3742))->hd)).f2)){
return 0;}} return 1; _LL3683: _temp3752= _temp3748; goto _LL3685; _LL3685:
_temp3758= _temp3752; goto _LL3687; _LL3687: for( 0; _temp3758 != 0; _temp3758=((
struct Cyc_List_List*) _check_null( _temp3758))->tl){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp3758))->hd)){
return 0;}} return 1; _LL3689: return 0; _LL3651:;} int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcutil_cnst_exp(
te, 0, e);} static int Cyc_Tcutil_fields_support_default( struct Cyc_List_List*
tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs); int Cyc_Tcutil_supports_default(
void* t){ void* _temp3806= Cyc_Tcutil_compress( t); struct Cyc_Core_Opt*
_temp3844; int _temp3846; struct Cyc_Core_Opt* _temp3848; struct Cyc_Core_Opt*
_temp3850; struct Cyc_Absyn_Tvar* _temp3852; struct Cyc_Absyn_TunionInfo
_temp3854; struct Cyc_Absyn_TunionFieldInfo _temp3856; struct Cyc_Absyn_PtrInfo
_temp3858; struct Cyc_Absyn_Conref* _temp3860; struct Cyc_Absyn_Tqual _temp3862;
struct Cyc_Absyn_Conref* _temp3864; void* _temp3866; void* _temp3868; void*
_temp3870; void* _temp3872; struct Cyc_Absyn_Exp* _temp3874; struct Cyc_Absyn_Tqual
_temp3876; void* _temp3878; struct Cyc_Absyn_FnInfo _temp3880; struct Cyc_List_List*
_temp3882; struct Cyc_Absyn_Structdecl** _temp3884; struct Cyc_List_List*
_temp3886; struct _tuple1* _temp3888; struct Cyc_Absyn_Uniondecl** _temp3890;
struct Cyc_List_List* _temp3892; struct _tuple1* _temp3894; struct Cyc_List_List*
_temp3896; struct Cyc_List_List* _temp3898; struct Cyc_Absyn_Enumdecl* _temp3900;
struct _tuple1* _temp3902; _LL3808: if( _temp3806 ==( void*) Cyc_Absyn_VoidType){
goto _LL3809;} else{ goto _LL3810;} _LL3810: if(( unsigned int) _temp3806 > 4u?*((
int*) _temp3806) == Cyc_Absyn_Evar: 0){ _LL3851: _temp3850=(( struct Cyc_Absyn_Evar_struct*)
_temp3806)->f1; goto _LL3849; _LL3849: _temp3848=(( struct Cyc_Absyn_Evar_struct*)
_temp3806)->f2; goto _LL3847; _LL3847: _temp3846=(( struct Cyc_Absyn_Evar_struct*)
_temp3806)->f3; goto _LL3845; _LL3845: _temp3844=(( struct Cyc_Absyn_Evar_struct*)
_temp3806)->f4; goto _LL3811;} else{ goto _LL3812;} _LL3812: if(( unsigned int)
_temp3806 > 4u?*(( int*) _temp3806) == Cyc_Absyn_VarType: 0){ _LL3853: _temp3852=((
struct Cyc_Absyn_VarType_struct*) _temp3806)->f1; goto _LL3813;} else{ goto
_LL3814;} _LL3814: if(( unsigned int) _temp3806 > 4u?*(( int*) _temp3806) == Cyc_Absyn_TunionType:
0){ _LL3855: _temp3854=(( struct Cyc_Absyn_TunionType_struct*) _temp3806)->f1;
goto _LL3815;} else{ goto _LL3816;} _LL3816: if(( unsigned int) _temp3806 > 4u?*((
int*) _temp3806) == Cyc_Absyn_TunionFieldType: 0){ _LL3857: _temp3856=(( struct
Cyc_Absyn_TunionFieldType_struct*) _temp3806)->f1; goto _LL3817;} else{ goto
_LL3818;} _LL3818: if(( unsigned int) _temp3806 > 4u?*(( int*) _temp3806) == Cyc_Absyn_PointerType:
0){ _LL3859: _temp3858=(( struct Cyc_Absyn_PointerType_struct*) _temp3806)->f1;
_LL3869: _temp3868=( void*) _temp3858.elt_typ; goto _LL3867; _LL3867: _temp3866=(
void*) _temp3858.rgn_typ; goto _LL3865; _LL3865: _temp3864= _temp3858.nullable;
goto _LL3863; _LL3863: _temp3862= _temp3858.tq; goto _LL3861; _LL3861: _temp3860=
_temp3858.bounds; goto _LL3819;} else{ goto _LL3820;} _LL3820: if(( unsigned int)
_temp3806 > 4u?*(( int*) _temp3806) == Cyc_Absyn_IntType: 0){ _LL3873: _temp3872=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp3806)->f1; goto _LL3871; _LL3871:
_temp3870=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3806)->f2; goto
_LL3821;} else{ goto _LL3822;} _LL3822: if( _temp3806 ==( void*) Cyc_Absyn_FloatType){
goto _LL3823;} else{ goto _LL3824;} _LL3824: if( _temp3806 ==( void*) Cyc_Absyn_DoubleType){
goto _LL3825;} else{ goto _LL3826;} _LL3826: if(( unsigned int) _temp3806 > 4u?*((
int*) _temp3806) == Cyc_Absyn_ArrayType: 0){ _LL3879: _temp3878=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3806)->f1; goto _LL3877; _LL3877:
_temp3876=(( struct Cyc_Absyn_ArrayType_struct*) _temp3806)->f2; goto _LL3875;
_LL3875: _temp3874=(( struct Cyc_Absyn_ArrayType_struct*) _temp3806)->f3; goto
_LL3827;} else{ goto _LL3828;} _LL3828: if(( unsigned int) _temp3806 > 4u?*((
int*) _temp3806) == Cyc_Absyn_FnType: 0){ _LL3881: _temp3880=(( struct Cyc_Absyn_FnType_struct*)
_temp3806)->f1; goto _LL3829;} else{ goto _LL3830;} _LL3830: if(( unsigned int)
_temp3806 > 4u?*(( int*) _temp3806) == Cyc_Absyn_TupleType: 0){ _LL3883:
_temp3882=(( struct Cyc_Absyn_TupleType_struct*) _temp3806)->f1; goto _LL3831;}
else{ goto _LL3832;} _LL3832: if(( unsigned int) _temp3806 > 4u?*(( int*)
_temp3806) == Cyc_Absyn_StructType: 0){ _LL3889: _temp3888=(( struct Cyc_Absyn_StructType_struct*)
_temp3806)->f1; goto _LL3887; _LL3887: _temp3886=(( struct Cyc_Absyn_StructType_struct*)
_temp3806)->f2; goto _LL3885; _LL3885: _temp3884=(( struct Cyc_Absyn_StructType_struct*)
_temp3806)->f3; goto _LL3833;} else{ goto _LL3834;} _LL3834: if(( unsigned int)
_temp3806 > 4u?*(( int*) _temp3806) == Cyc_Absyn_UnionType: 0){ _LL3895:
_temp3894=(( struct Cyc_Absyn_UnionType_struct*) _temp3806)->f1; goto _LL3893;
_LL3893: _temp3892=(( struct Cyc_Absyn_UnionType_struct*) _temp3806)->f2; goto
_LL3891; _LL3891: _temp3890=(( struct Cyc_Absyn_UnionType_struct*) _temp3806)->f3;
goto _LL3835;} else{ goto _LL3836;} _LL3836: if(( unsigned int) _temp3806 > 4u?*((
int*) _temp3806) == Cyc_Absyn_AnonStructType: 0){ _LL3897: _temp3896=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp3806)->f1; goto _LL3837;} else{ goto
_LL3838;} _LL3838: if(( unsigned int) _temp3806 > 4u?*(( int*) _temp3806) == Cyc_Absyn_AnonUnionType:
0){ _LL3899: _temp3898=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3806)->f1;
goto _LL3839;} else{ goto _LL3840;} _LL3840: if(( unsigned int) _temp3806 > 4u?*((
int*) _temp3806) == Cyc_Absyn_EnumType: 0){ _LL3903: _temp3902=(( struct Cyc_Absyn_EnumType_struct*)
_temp3806)->f1; goto _LL3901; _LL3901: _temp3900=(( struct Cyc_Absyn_EnumType_struct*)
_temp3806)->f2; goto _LL3841;} else{ goto _LL3842;} _LL3842: goto _LL3843;
_LL3809: return 1; _LL3811: return 0; _LL3813: return 0; _LL3815: return 0;
_LL3817: return 0; _LL3819: { void* _temp3904=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3860))->v; void*
_temp3912; void* _temp3914; struct Cyc_Absyn_Exp* _temp3916; _LL3906: if((
unsigned int) _temp3904 > 1u?*(( int*) _temp3904) == Cyc_Absyn_Eq_constr: 0){
_LL3913: _temp3912=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3904)->f1;
if( _temp3912 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3907;} else{ goto _LL3908;}}
else{ goto _LL3908;} _LL3908: if(( unsigned int) _temp3904 > 1u?*(( int*)
_temp3904) == Cyc_Absyn_Eq_constr: 0){ _LL3915: _temp3914=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3904)->f1; if(( unsigned int) _temp3914 > 1u?*(( int*) _temp3914) == Cyc_Absyn_Upper_b:
0){ _LL3917: _temp3916=(( struct Cyc_Absyn_Upper_b_struct*) _temp3914)->f1; goto
_LL3909;} else{ goto _LL3910;}} else{ goto _LL3910;} _LL3910: goto _LL3911;
_LL3907: return 1; _LL3909: { void* _temp3918=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3864))->v; int
_temp3924; _LL3920: if(( unsigned int) _temp3918 > 1u?*(( int*) _temp3918) ==
Cyc_Absyn_Eq_constr: 0){ _LL3925: _temp3924=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3918)->f1; goto _LL3921;} else{ goto _LL3922;} _LL3922: goto _LL3923;
_LL3921: return _temp3924; _LL3923: return 0; _LL3919:;} _LL3911: return 0;
_LL3905:;} _LL3821: return 1; _LL3823: return 1; _LL3825: return 1; _LL3827:
return Cyc_Tcutil_supports_default( _temp3878); _LL3829: return 0; _LL3831: for(
0; _temp3882 != 0; _temp3882=(( struct Cyc_List_List*) _check_null( _temp3882))->tl){
if( ! Cyc_Tcutil_supports_default((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp3882))->hd)).f2)){ return 0;}} return 1; _LL3833: if(
_temp3884 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp3884)); if( sd->fields == 0){ return 0;} return Cyc_Tcutil_fields_support_default(
sd->tvs, _temp3886,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
sd->fields))->v);} _LL3835: if( _temp3890 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp3890)); if( ud->fields ==
0){ return 0;} return Cyc_Tcutil_fields_support_default( ud->tvs, _temp3892,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ud->fields))->v);}
_LL3837: _temp3898= _temp3896; goto _LL3839; _LL3839: return Cyc_Tcutil_fields_support_default(
0, 0, _temp3898); _LL3841: return 1; _LL3843: return 0; _LL3807:;} static int
Cyc_Tcutil_fields_support_default( struct Cyc_List_List* tvs, struct Cyc_List_List*
ts, struct Cyc_List_List* fs){{ struct _RegionHandle _temp3926= _new_region();
struct _RegionHandle* rgn=& _temp3926; _push_region( rgn);{ struct Cyc_List_List*
_temp3927=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, tvs, ts); for( 0; fs != 0; fs=(( struct Cyc_List_List*) _check_null(
fs))->tl){ void* t= Cyc_Tcutil_rsubstitute( rgn, _temp3927,( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( fs))->hd)->type); if( ! Cyc_Tcutil_supports_default(
t)){ int _temp3928= 0; _npop_handler( 0u); return _temp3928;}}}; _pop_region(
rgn);} return 1;}